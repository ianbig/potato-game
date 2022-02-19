#include "network.hpp"

#include <arpa/inet.h>
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

/** 
 * API for sending message
 * @param recvSocket: socket fd
 * @param buffer: memory space for receiving data
 * @param buffer_len: memory space size for receiving data
 * @throw: std::exception is recv failed
 **/
void Network::sendRequest(int connectSocket, void * msg, size_t msg_len) {
  if (send(connectSocket, msg, msg_len, 0) == -1) {
    perror("send");
    throw std::exception();
  }
}

/** 
 * API for receive message
 * @param recvSocket: socket fd
 * @param buffer: valid memory space for receiving data (Note: allocate the memeory before passing in the function)
 * @param buffer_len: memory space size for receiving data
 * @throw: std::exception is recv failed
 **/
void Network::recvResponse(int recvSocket, void * buffer, size_t buffer_len) {
  ssize_t numbytes = 0;
  if ((numbytes = recv(recvSocket, buffer, buffer_len, 0)) == -1) {
    perror("recv");
    throw std::exception();
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

std::string Network::getConnectionIp(ConnectionInfo & info) {
  char s[INET_ADDRSTRLEN];
  inet_ntop(info.client_addr.ss_family,
            get_in_addr((struct sockaddr *)&info.client_addr),
            s,
            sizeof(s));
  return std::string(s);
}

void * Network::get_in_addr(struct sockaddr * sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

Network::Network() : socket_fd(-1), serviceinfo(nullptr) {
}

std::pair<std::string, size_t> Network::getIpPort(struct sockaddr * addr) {
  socklen_t len = sizeof(addr);
  getsockname(socket_fd, addr, &len);
  // get IP
  char IP[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), IP, sizeof(IP));
  // get port
  size_t port = ntohs(((struct sockaddr_in *)addr)->sin_port);

  return std::pair<std::string, size_t>(IP, port);
}
