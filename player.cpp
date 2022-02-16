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

Player::Player() : client_connect(new Network()) {
}

void Player::startConnection(std::string hostname, std::string port) {
  client_connect->connectSetup(hostname.c_str(), std::stoi(port));

  if (connect(client_connect->socket_fd,
              client_connect->serviceinfo->ai_addr,
              client_connect->serviceinfo->ai_addrlen) == -1) {
    perror("connect");
    throw std::exception();
  }

  char msg[1024] = "Hi server";
  Network::sendRequest(client_connect->socket_fd, msg, sizeof(msg));

  char buf[MAX_RECV_DATA] = {0};
  Network::recvResponse(client_connect->socket_fd, buf, sizeof(buf));

  //TODO: write client unpack function to unpack server msg

  std::cout << "get from server: " << buf << std::endl;
}

Player::~Player() {
  delete client_connect;
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"machine_name", "port_name"};
  std::unordered_map<std::string, std::string> parsed = getOpt(argc, argv, opts);

  Player p;
  p.startConnection(parsed["machine_name"], parsed["port_name"]);
}
