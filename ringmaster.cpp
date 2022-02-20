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
void RingMaster::startGame(size_t num_players) {
  while (players.size() < num_players) {
    playerInfo info;
    acceptRequest(&info);
    printConnectionInfo(info);
    players.push_back(info);
  }

  buildPlayerRing();
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
    printRingMasterRecvInfo(playerNeighborMsg, player);
    // send back neighbor message
    playerInfo neighbor = players[(i + 1) % players.size()];
    Network::sendRequest(neighbor.playerConnectInfo.connectionSocketfd,
                         &playerNeighborMsg,
                         sizeof(playerNeighborMsg));
  }

  int count = 3;
  Network::sendRequest(
      players[0].playerConnectInfo.connectionSocketfd, &count, sizeof(count));
  while (1) {
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

void RingMaster::shutDownGame() {
  // close all the socket with client
}

RingMaster::~RingMaster() {
  delete connectInfo;
}

RingMaster::RingMaster() : connectInfo(new Network()) {
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"port", "num_players", "num_hops"};
  std::unordered_map<std::string, int> parsedOpt = parseOpt(argc, argv, opts);

  for (auto iter = parsedOpt.begin(); iter != parsedOpt.end(); iter++) {
    std::cout << iter->first << ": " << iter->second << std::endl;
  }

  RingMaster rm;

  rm.setupServer(parsedOpt["port"]);
  rm.startGame(parsedOpt["num_players"]);
}
