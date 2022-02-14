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

void * get_in_addr(struct sockaddr * sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

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
  Network connectInfo;
  std::pair<int, struct addrinfo *> socketInfo =
      connectInfo.connectSetup<int, struct addrinfo *>(NULL, port_num);

  this->socket_fd = socketInfo.first;
  this->serviceinfo = socketInfo.second;

  if (bind(this->socket_fd, this->serviceinfo->ai_addr, this->serviceinfo->ai_addrlen) ==
      -1) {
    // TODO: throw exception
    perror("bind");
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    // TODO: throw exception
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (listen(this->socket_fd, BACKLOG) == -1) {
    perror("listen");
    throw std::exception();
  }
}

void RingMaster::acceptRequest(ConnectionInfo * resp) {
  socklen_t client_addr_size = sizeof(resp->client_addr);
  if ((resp->connectionSocketfd = accept(this->socket_fd,
                                         (struct sockaddr *)&(resp->client_addr),
                                         &client_addr_size)) == -1) {
    perror("accept");
    throw std::exception();
  }
}

void RingMaster::printConnectionInfo(ConnectionInfo info) {
  char s[INET_ADDRSTRLEN];
  inet_ntop(info.client_addr.ss_family,
            get_in_addr((struct sockaddr *)&info.client_addr),
            s,
            sizeof(s));
  std::cout << "server: got connection from " << s << std::endl;
}

void RingMaster::startGame(size_t num_players) {
  while (players.size() < num_players) {
    ConnectionInfo info;
    acceptRequest(&info);
    printConnectionInfo(info);
    char msg[] = "greeting from server";
    if (send(info.connectionSocketfd, msg, strlen(msg) + 1, 0) == -1) {
      perror("send");
      throw std::exception();
    }
  }
  assert(players.size() == num_players);
}

void RingMaster::shutDownGame() {
  freeaddrinfo(serviceinfo);

  if (close(socket_fd) == -1) {
    // TODO: throw exception
    perror("close");
    exit(EXIT_FAILURE);
  }
}

RingMaster::~RingMaster() {
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
