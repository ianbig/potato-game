#include "player.hpp"

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

Player::Player() : tunnelCount(3), client_connect(new Network[tunnelCount]) {
}

void Player::startConnection(std::string hostname, std::string port) {
  client_connect[RINGMASTER_TUNNEL].connectSetup(hostname.c_str(), std::stoi(port));

  if (connect(client_connect[RINGMASTER_TUNNEL].socket_fd,
              client_connect[RINGMASTER_TUNNEL].serviceinfo->ai_addr,
              client_connect[RINGMASTER_TUNNEL].serviceinfo->ai_addrlen) == -1) {
    perror("connect");
    throw std::exception();
  }

  // pack connection info for ringmaster to assign neighbor
  PortIP ip_port = client_connect[LISTEN_TUNNEL].getIpPort(
      client_connect[LISTEN_TUNNEL].serviceinfo->ai_addr);

  playerRequest request;
  memset(&request, 0, sizeof(request));
  request.port = ip_port.second;

  std::cout << "player send out port: " << request.port << std::endl;
  Network::sendRequest(
      client_connect[RINGMASTER_TUNNEL].socket_fd, &request, sizeof(request));

  masterToPlayerInfo neighborInfo;
  memset(&neighborInfo, 0, sizeof(neighborInfo));
  Network::recvResponse(
      client_connect[RINGMASTER_TUNNEL].socket_fd, &neighborInfo, sizeof(neighborInfo));
  setupConnectionToNeighbor(neighborInfo);
}

void Player::setupConnectionToNeighbor(masterToPlayerInfo & neighborInfo) {
  client_connect[CONNECT_TUNNEL].connectSetup(neighborInfo.ip, neighborInfo.port);

  if (connect(client_connect[CONNECT_TUNNEL].socket_fd,
              client_connect[CONNECT_TUNNEL].serviceinfo->ai_addr,
              client_connect[CONNECT_TUNNEL].serviceinfo->ai_addrlen) == -1) {
    perror("connect");
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
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"machine_name", "port_name"};
  std::unordered_map<std::string, std::string> parsed = getOpt(argc, argv, opts);

  Player p;
  p.setupListenPort();

  p.startConnection(parsed["machine_name"], parsed["port_name"]);
}
