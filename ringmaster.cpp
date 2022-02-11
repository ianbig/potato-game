#include "ringmaster.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "network.hpp"

#define RINGMASTER_COMMAND_LENGTH 4

std::unordered_map<const char *, int> parseOpt(int argc,
                                               char ** argv,
                                               std::vector<std::string> opts) {
  if (argc != RINGMASTER_COMMAND_LENGTH) {
    // TODO: refactor this to throw exception
    std::cerr << "Error: ringmaster <port_num> <num_players> <num_hops>" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<const char *, int> parsedOpt;
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
    parsedOpt[option.c_str()] = tmp;
  }

  return parsedOpt;
}

void RingMaster::setupServer(const int port_num) {
  Network * connect = new Network();
  std::pair<int, struct addrinfo *> socketInfo =
      connect->connectSetup<int, struct addrinfo *>(port_num);

  int socket_fd = socketInfo.first;
  struct addrinfo * serviceinfo = socketInfo.second;

  if (bind(socket_fd, serviceinfo->ai_addr, serviceinfo->ai_addrlen) == -1) {
    // TODO: throw exception
    perror("bind");
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    // TODO: throw exception
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(serviceinfo);

  if (close(socket_fd) == -1) {
    // TODO: throw exception
    perror("close");
    exit(EXIT_FAILURE);
  }
}

void RingMaster::shutDownGame() {
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"port", "num_players", "num_hops"};
  std::unordered_map<const char *, int> parsedOpt = parseOpt(argc, argv, opts);
  RingMaster rm;
  rm.setupServer(parsedOpt["port"]);
}
