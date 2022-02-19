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

#define BACKLOG 10

struct playerInfo {
  int id;
  ConnectionInfo playerConnectInfo;
};

typedef struct playerInfo playerInfo;

class RingMaster {
  Potato potato;
  std::vector<playerInfo> players;
  Network * connectInfo;

  static int playerId;

  void acceptRequest(playerInfo * resp);
  void printConnectionInfo(playerInfo & info);
  void buildPlayerRing();

 public:
  // TODO: finish rule of five
  RingMaster();
  ~RingMaster();
  void setupServer(int port_num);
  void startGame(size_t num_players);
  void shutDownGame();
};

int RingMaster::playerId = 0;
#endif
