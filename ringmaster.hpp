#ifndef RINGMASTER_HPP
#define RINGMASTER_HPP
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <vector>

#include "player.hpp"
#include "potato.hpp"

struct ConnectionInfo {
  int connectionSocketfd;
  struct sockaddr_storage client_addr;
};

typedef struct ConnectionInfo ConnectionInfo;

#define BACKLOG 10
class RingMaster {
  Potato potato;
  std::vector<int> players;
  int socket_fd;
  struct addrinfo * serviceinfo;

 public:
  ~RingMaster();
  void setupServer(int port_num);
  void startGame(size_t num_players);
  void shutDownGame();
  void acceptRequest(ConnectionInfo * resp);
  void printConnectionInfo(ConnectionInfo info);
};
#endif
