#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>
#include <vector>
#define MAX_RECV_DATA 1024
#include "network.hpp"
class Player {
  size_t player_id;
  std::vector<int> socket_fds;
  Network * client_connect;

 public:
  Player();
  ~Player();
  void startConnection(std::string hostname, std::string port);
};

#endif
