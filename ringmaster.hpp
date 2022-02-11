#ifndef RINGMASTER_HPP
#define RINGMASTER_HPP
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <vector>

#include "player.hpp"
#include "potato.hpp"
class RingMaster {
  Potato potato;
  std::vector<Player> players;

 public:
  void setupServer(int port_num);
  void shutDownGame();
};
#endif
