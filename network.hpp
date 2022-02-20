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

/**
 * record connection information in a socket connection
 */
struct ConnectionInfo {
  int connectionSocketfd;
  struct sockaddr_storage client_addr;
};

struct masterToPlayerInfo {
  size_t id;
  char ip[INET_ADDRSTRLEN];
  size_t port;
};

typedef struct ConnectionInfo ConnectionInfo;
typedef std::pair<std::string, size_t> PortIP;
typedef struct masterToPlayerInfo masterToPlayerInfo;

class Network {
 public:
  int socket_fd;
  struct addrinfo * serviceinfo;

  static void * get_in_addr(struct sockaddr * sa);

  // TODO: finish rule of five
  Network();
  ~Network();
  void connectSetup(const char * hostname, int port_num);
  std::pair<std::string, size_t> getIpPort(struct sockaddr * serviceinfo);
  static void sendRequest(int connectSocket, void * msg, size_t msg_len);
  static void recvResponse(int recvSocket, void * buffer, size_t buffer_len);
  static PortIP getConnectionIp(ConnectionInfo & info);
};

#endif
