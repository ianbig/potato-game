#include "network.hpp"

void Network::connectSetup(const char * hostname, int port_num) {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int status = 0;
  if ((status = getaddrinfo(
           hostname, std::to_string(port_num).c_str(), &hints, &serviceinfo)) != 0) {
    // TODO: throw exception
    fprintf(stderr, "Error: getaddrinfo error\n");
    exit(EXIT_FAILURE);
  }

  // TODO: refactor to loop through linked list
  if ((socket_fd = socket(
           serviceinfo->ai_family, serviceinfo->ai_socktype, serviceinfo->ai_protocol)) ==
      -1) {
    // TODO: throw exception
    perror("socket error");
    exit(EXIT_FAILURE);
  }
}

Network::~Network() {
  freeaddrinfo(serviceinfo);

  if (close(socket_fd) == -1) {
    // TODO: throw exception
    perror("close");
    exit(EXIT_FAILURE);
  }
}
