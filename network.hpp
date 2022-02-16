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

  void connectSetup(const char * hostname, int port_num);
  ~Network();
};

#endif
