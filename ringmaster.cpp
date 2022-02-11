#include "ringmaster.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#define RINGMASTER_COMMAND_LENGTH 4

std::unordered_map<const char *, int> parseOpt(int argc,
                                               char ** argv,
                                               std::vector<std::string> opts) {
  if (argc != RINGMASTER_COMMAND_LENGTH) {
    // TODO: refactor this to throw exception
    fprintf(stderr, "Error: ringmaster <port_num> <num_players> <num_hops>\n");
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
  struct addrinfo hints;
  struct addrinfo * serviceinfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  int status = 0;
  if ((status = getaddrinfo(
           NULL, std::to_string(port_num).c_str(), &hints, &serviceinfo)) != 0) {
    // TODO: throw exception
    fprintf(stderr, "Error: getaddrinfo error\n");
    exit(EXIT_FAILURE);
  }

  // TODO: refactor to loop through linked list
  int socket_fd = -1;
  if ((socket_fd = socket(
           serviceinfo->ai_family, serviceinfo->ai_socktype, serviceinfo->ai_protocol)) ==
      -1) {
    // TODO: throw exception
    perror("socket error");
    exit(EXIT_FAILURE);
  }

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
