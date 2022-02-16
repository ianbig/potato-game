#ifndef NETWORK_H
#define NETWORK_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>

class Network {
 public:
  int socket_fd;
  struct addrinfo * serviceinfo;

  // TODO: finish rule of five
  ~Network();
  void connectSetup(const char * hostname, int port_num);
  static void sendRequest(int connectSocket, void * msg, size_t msg_len);
  static void recvResponse(int recvSocket, void * buffer, size_t buffer_len);
};

#endif
