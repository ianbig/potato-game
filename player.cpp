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
  std::pair<int, struct addrinfo *> socketInfo =
      client_connect->connectSetup<int, struct addrinfo *>(hostname.c_str(),
                                                           std::stoi(port));
  int socketfd = socketInfo.first;
  struct addrinfo * sockInfo = socketInfo.second;

  if (connect(socketfd, sockInfo->ai_addr, sockInfo->ai_addrlen) == -1) {
    perror("connect");
    throw std::exception();
  }

  char msg[] = "hello from client";
  if (send(socketfd, msg, strlen(msg) + 1, 0) == -1) {
    perror("send");
    throw std::exception();
  }

  char buf[MAX_RECV_DATA] = {0};
  if (recv(socketfd, buf, MAX_RECV_DATA, 0) == -1) {
    perror("recv");
    throw std::exception();
  }

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
