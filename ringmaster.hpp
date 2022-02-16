#ifndef RINGMASTER_HPP
#define RINGMASTER_HPP
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <vector>

#include "network.hpp"
#include "player.hpp"
#include "potato.hpp"

struct ConnectionInfo {
  int connectionSocketfd;
  struct sockaddr_storage client_addr;
  char recvmsg[1024];
};

struct serverResponse {
  int id;
};

typedef struct ConnectionInfo ConnectionInfo;
typedef struct serverResponse serverResponse;

#define BACKLOG 10
class RingMaster {
  Potato potato;
  std::vector<int> players;
  Network * connectInfo;

  static int playerId;

  int unpackData(ConnectionInfo & info);
  void packResponseMsg(serverResponse & resp);

 public:
  // TODO: finish rule of five
  RingMaster();
  ~RingMaster();
  void setupServer(int port_num);
  void startGame(size_t num_players);
  void shutDownGame();
  void acceptRequest(ConnectionInfo * resp);
  void printConnectionInfo(ConnectionInfo info);
};

int RingMaster::playerId = 0;
#endif
