#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <vector>

class Player {
  size_t player_id;
  std::vector<int> socket_fds;

 public:
  Player();
};

#endif
