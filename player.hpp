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
#include "potato.hpp"

#define RINGMASTER_TUNNEL 0
#define LISTEN_TUNNEL 1
#define CONNECT_TUNNEL 2
#define BACKLOG 10

struct playerRequest {
  size_t port;
};

typedef struct playerRequest playerRequest;

class Player {
  size_t id;  // TODO: assign id
  size_t tunnelCount;
  Network * client_connect;
  struct pollfd * pollArr;
  void setupConnectionToNeighbor(masterToPlayerInfo & neighborInfo);
  void setupIOMUX();
  int checkResult(int & count);
  int generateNextPass();
  void printPort();

 public:
  Player();
  ~Player();
  // TODO: finish rule of five
  int startConnection(std::string hostname, std::string port);
  void setupListenPort();
  void playGame();
};

#endif
