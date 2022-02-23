#include "ringmaster.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "network.hpp"

#define RINGMASTER_COMMAND_LENGTH 4

std::unordered_map<std::string, int> parseOpt(int argc,
                                              char ** argv,
                                              std::vector<std::string> opts) {
  if (argc != RINGMASTER_COMMAND_LENGTH) {
    // TODO: refactor this to throw exception
    std::cerr << "Error: ringmaster <port_num> <num_players> <num_hops>" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<std::string, int> parsedOpt;
  for (int i = 1; i < argc; i++) {
    std::stringstream ss;
    ss << argv[i];
    int tmp = 0;
    ss >> tmp;

    if (ss.fail()) {
      // TODO: throw exception
      fprintf(stderr, "Exception: invalid converion for parameter %s\n", argv[i]);
      exit(EXIT_FAILURE);
    }

    std::string option =
        opts[i - 1];  // argv corresponding value is 1 more that parsedOpt
    parsedOpt[option] = tmp;
  }

  return parsedOpt;
}

void RingMaster::setupServer(const int port_num) {
  connectInfo->connectSetup(NULL, port_num);

  int yes = 1;
  if (setsockopt(connectInfo->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    // TODO: throw exception
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(connectInfo->socket_fd,
           connectInfo->serviceinfo->ai_addr,
           connectInfo->serviceinfo->ai_addrlen) == -1) {
    // TODO: throw exception
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(connectInfo->socket_fd, BACKLOG) == -1) {
    perror("listen");
    throw std::exception();
  }
}

/** 
 * Accept reuqest in listen port queue, store the new socetid, and playerid
 * in the provding structure
 * @params resp: strucutre to store new connection information
 **/
void RingMaster::acceptRequest(playerInfo * resp) {
  socklen_t client_addr_size = sizeof(resp->playerConnectInfo.client_addr);
  if ((resp->playerConnectInfo.connectionSocketfd =
           accept(connectInfo->socket_fd,
                  (struct sockaddr *)&(resp->playerConnectInfo.client_addr),
                  &client_addr_size)) == -1) {
    perror("accept");
    throw std::exception();
  }

  resp->id = playerId++;
  // memset(resp, 0, sizeof(*resp));
  Network::sendRequest(resp->playerConnectInfo.connectionSocketfd, resp, sizeof(*resp));
}

/**
 * Print out Connectiontion information according to struct ConnectionInfo
 * @param info: connection information to printout
 **/
void RingMaster::printConnectionInfo(playerInfo & info) {
  PortIP portIPInfo = Network::getConnectionIp(info.playerConnectInfo);
  std::cout << "server: got connection from " << portIPInfo.first << " with player id "
            << info.id << std::endl;
}

/** 
 * the function would set up the game (i.e. building the ring)
 * @ param num_players: players in the game
 **/
void RingMaster::startGame(size_t num_players, size_t nhops) {
  while (players.size() < num_players) {
    playerInfo info;
    acceptRequest(&info);
    players.push_back(info);
    std::cout << "Player " << info.id << " is ready to play" << std::endl;
  }

  buildPlayerRing();
  setupIoMux();
  // send out information to random player
  sendToRandomPlayer(nhops);
  listenforIT();
}

void RingMaster::listenforIT() {
  poll(pollArr, players.size(), -1);
  for (size_t i = 0; i < players.size(); i++) {
    if (pollArr[i].revents & POLLIN) {
      Potato endPotato;
      Network::recvResponse(pollArr[i].fd, &endPotato, sizeof(endPotato));
      endPotato.printTrace();
      shutDownGame();
      return;
    }
  }
}

void RingMaster::sendToRandomPlayer(int nhops) {
  srand((unsigned int)time(NULL) + playerId);
  int startPlayer = rand() % players.size();
  std::cout << "Ready to start the game, sending potato to player " << startPlayer
            << std::endl;
  Potato initPotato(nhops);
  Network::sendRequest(players[startPlayer].playerConnectInfo.connectionSocketfd,
                       &initPotato,
                       sizeof(initPotato));
}

void RingMaster::setupIoMux() {
  pollArr = new struct pollfd[players.size()];
  for (size_t i = 0; i < players.size(); i++) {
    pollArr[i].fd = players[i].playerConnectInfo.connectionSocketfd;
    pollArr[i].events = POLLIN;
  }
}

/**
 * bulding up the ring in ringmaster process
 * @implementation: send neighbor info to player process to create concept
 * of ring
 **/
void RingMaster::buildPlayerRing() {
  for (size_t i = 0; i < players.size(); i++) {
    playerInfo player = players[i];
    playerRequest info;
    Network::recvResponse(
        player.playerConnectInfo.connectionSocketfd, &info, sizeof(info));
    // unpack receive message
    masterToPlayerInfo playerNeighborMsg;
    memset(&playerNeighborMsg, 0, sizeof(playerNeighborMsg));
    strncpy(playerNeighborMsg.ip,
            Network::getConnectionIp(player.playerConnectInfo).first.c_str(),
            sizeof(playerNeighborMsg
                       .ip));  // the real ip could only get from information after accept
    playerNeighborMsg.port = info.port;
    playerNeighborMsg.total_player = players.size();

    // send back neighbor message
    playerInfo neighbor = players[(i + 1) % players.size()];
    Network::sendRequest(neighbor.playerConnectInfo.connectionSocketfd,
                         &playerNeighborMsg,
                         sizeof(playerNeighborMsg));
  }
}

/** 
 * Debug function for ringmaster to check information recveive from player
 **/
void RingMaster::printRingMasterRecvInfo(masterToPlayerInfo & playerNeighborMsg,
                                         playerInfo & player) {
  std::cout << "player id: " << player.id << " with ip: " << playerNeighborMsg.ip
            << " with port: " << playerNeighborMsg.port << std::endl;
}

/**
 * Shutdown the game and clean the resources
 **/
void RingMaster::shutDownGame() {
  // close all the socket with client
  Potato endPotato;
  endPotato.nhops = -1;
  for (size_t i = 0; i < players.size(); i++) {
    Network::sendRequest(
        players[i].playerConnectInfo.connectionSocketfd, &endPotato, sizeof(endPotato));
  }

  delete[] pollArr;
  freeaddrinfo(connectInfo->serviceinfo);
}

RingMaster::~RingMaster() {
  delete connectInfo;
}

RingMaster::RingMaster() : connectInfo(new Network()) {
}

void printPrompt(int numPlayers, int num_hops) {
  std::cout << "Potato Ringmaster" << std::endl;
  std::cout << "Players = " << numPlayers << std::endl;
  std::cout << "Hops = " << num_hops << std::endl;
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"port", "num_players", "num_hops"};
  std::unordered_map<std::string, int> parsedOpt = parseOpt(argc, argv, opts);

  printPrompt(parsedOpt["num_players"], parsedOpt["num_hops"]);

  if (parsedOpt["num_players"] == 0 || parsedOpt["num_hops"] == 0) {
    return EXIT_SUCCESS;
  }

  RingMaster rm;

  rm.setupServer(parsedOpt["port"]);
  rm.startGame(parsedOpt["num_players"], parsedOpt["num_hops"]);
}
