#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>
#include <vector>
#define MAX_RECV_DATA 1024
#define MAX_SEND_DATA 1024
#include "network.hpp"

#define RINGMASTER_TUNNEL 0
#define LISTEN_TUNNEL 1
#define CONNECT_TUNNEL 2
#define BACKLOG 10

struct playerRequest {
  size_t port;
};

typedef struct playerRequest playerRequest;

class Player {
  size_t tunnelCount;
  Network * client_connect;
  void setupConnectionToNeighbor(masterToPlayerInfo & neighborInfo);

 public:
  Player();
  ~Player();
  // TODO: finish rule of five
  void startConnection(std::string hostname, std::string port);
  void setupListenPort();
};

#endif
