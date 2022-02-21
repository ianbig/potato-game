#include "player.hpp"

#include <poll.h>

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "network.hpp"
#define PLAYER_COMMAND_LENGTH 3

std::unordered_map<std::string, std::string> getOpt(int argc,
                                                    char ** argv,
                                                    std::vector<std::string> opts) {
  if (argc != PLAYER_COMMAND_LENGTH) {
    std::cerr << "Error: player <machine_domain_name> <port_name>" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<std::string, std::string> parsedOpt;
  for (int i = 1; i < argc; i++) {
    parsedOpt[opts[i - 1]] = argv[i];
  }

  return parsedOpt;
}

Player::Player() :
    id(0),
    tunnelCount(3),
    client_connect(new Network[tunnelCount]),
    pollArr(new struct pollfd[tunnelCount]) {
}

int Player::startConnection(std::string hostname, std::string port) {
  client_connect[RINGMASTER_TUNNEL].connectSetup(hostname.c_str(), std::stoi(port));

  if (connect(client_connect[RINGMASTER_TUNNEL].socket_fd,
              client_connect[RINGMASTER_TUNNEL].serviceinfo->ai_addr,
              client_connect[RINGMASTER_TUNNEL].serviceinfo->ai_addrlen) == -1) {
    perror("connect");
    throw std::exception();
  }

  freeaddrinfo(client_connect[RINGMASTER_TUNNEL].serviceinfo);

  // pack connection info for ringmaster to assign neighbor
  PortIP ip_port = client_connect[LISTEN_TUNNEL].getIpPort(
      client_connect[LISTEN_TUNNEL].serviceinfo->ai_addr);

  playerRequest request;
  memset(&request, 0, sizeof(request));
  request.port = ip_port.second;

  Network::sendRequest(
      client_connect[RINGMASTER_TUNNEL].socket_fd, &request, sizeof(request));

  masterToPlayerInfo neighborInfo;
  memset(&neighborInfo, 0, sizeof(neighborInfo));
  Network::recvResponse(
      client_connect[RINGMASTER_TUNNEL].socket_fd, &neighborInfo, sizeof(neighborInfo));
  setupConnectionToNeighbor(neighborInfo);
  setupIOMUX();

  return 0;
}

void Player::printPort() {
  std::cerr << "listen: " << client_connect[LISTEN_TUNNEL].socket_fd
            << " connect: " << client_connect[CONNECT_TUNNEL].socket_fd
            << " ringmaster: " << client_connect[RINGMASTER_TUNNEL].socket_fd
            << std::endl;
}

void Player::setupIOMUX() {
  memset(pollArr, 0, sizeof(*pollArr) * tunnelCount);
  for (size_t i = 0; i < tunnelCount; i++) {
    pollArr[i].fd = client_connect[i].socket_fd;
    pollArr[i].events = POLLIN;
  }
}

void Player::setupConnectionToNeighbor(masterToPlayerInfo & neighborInfo) {
  client_connect[CONNECT_TUNNEL].connectSetup(neighborInfo.ip, neighborInfo.port);

  if (connect(client_connect[CONNECT_TUNNEL].socket_fd,
              client_connect[CONNECT_TUNNEL].serviceinfo->ai_addr,
              client_connect[CONNECT_TUNNEL].serviceinfo->ai_addrlen) == -1) {
    perror("connect");
    throw std::exception();
  }

  freeaddrinfo(client_connect[CONNECT_TUNNEL].serviceinfo);

  freeaddrinfo(client_connect[LISTEN_TUNNEL].serviceinfo);
  socklen_t size = sizeof(client_connect[LISTEN_TUNNEL].serviceinfo);
  if ((client_connect[LISTEN_TUNNEL].socket_fd =
           accept(client_connect[LISTEN_TUNNEL].socket_fd,
                  (struct sockaddr *)&(client_connect[LISTEN_TUNNEL].serviceinfo),
                  &size)) == -1) {
    perror("accept");
    throw std::exception();
  }
}

void Player::setupListenPort() {
  client_connect[LISTEN_TUNNEL].connectSetup(NULL, 0);

  int yes = 1;
  if (setsockopt(client_connect[LISTEN_TUNNEL].socket_fd,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 &yes,
                 sizeof(yes)) == -1) {
    // TODO: throw exception
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(client_connect[LISTEN_TUNNEL].socket_fd,
           client_connect[LISTEN_TUNNEL].serviceinfo->ai_addr,
           client_connect[LISTEN_TUNNEL].serviceinfo->ai_addrlen) == -1) {
    // TODO: throw exception
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(client_connect[LISTEN_TUNNEL].socket_fd, BACKLOG) == -1) {
    perror("listen");
    throw std::exception();
  }
}

Player::~Player() {
  delete[] client_connect;
  close(pollArr[LISTEN_TUNNEL].fd);
  delete[] pollArr;
}

void Player::playGame() {
  while (1) {
    poll(pollArr, tunnelCount, -1);
    for (size_t i = 0; i < tunnelCount; i++) {
      if (pollArr[i].revents & POLLIN) {
        int count = 0;
        Network::recvResponse(pollArr[i].fd, &count, sizeof(count));
        // end of game send back to notify ringmaster
        if (checkResult(count) == -1) {
          return;
        }
        // send to next player
        int sendTo = generateNextPass();
        count -= 1;
        std::cout << "count is " << count << " index sendTo: " << sendTo << std::endl;
        Network::sendRequest(client_connect[sendTo].socket_fd, &count, sizeof(count));
      }
    }
  }
}

/**
 * Check whether the hops reach zero if reach zero push the message back to ring master server
 * @ return -1: means the hops reach zero, need to pass to ringmaster, 0 means the game still running
 **/
int Player::checkResult(int & count) {
  if (count <= 0) {
    std::string msg = "player " + std::to_string(id) + "is reporting the end of game";
    Network::sendRequest(
        client_connect[RINGMASTER_TUNNEL].socket_fd, msg.c_str(), msg.size());
    return -1;
  }

  return 0;
}

int Player::generateNextPass() {
  srand((unsigned int)time(NULL) + id);
  size_t sendTo = (rand() % 2) + 1;  // left and right neighbor
  assert(sendTo == 1 || sendTo == 2);
  return sendTo;
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"machine_name", "port_name"};
  std::unordered_map<std::string, std::string> parsed = getOpt(argc, argv, opts);

  Player p;
  p.setupListenPort();

  int status = p.startConnection(parsed["machine_name"], parsed["port_name"]);
  if (status != 0) {
    std::cerr << "Error in Connecting the Game" << std::endl;
  }
  p.playGame();
}
