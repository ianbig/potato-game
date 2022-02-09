#include "ringmaster.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, int> parseOpt(int argc,
                                              char ** argv,
                                              std::vector<std::string> opts) {
  if (argc != 4) {
    // TODO: refactor this to throw exception
    fprintf(stderr, "Error: ringmaster <port_num> <num_players> <num_hops>\n");
    exit(EXIT_FAILURE);
  }

  std::unordered_map<std::string, int> parsedOpt;
  for (int i = 1; i < argc; i++) {
    std::stringstream ss;
    ss << argv[i];
    int tmp = 0;
    ss >> tmp;

    if (ss.fail()) {
      // TODO: throw exception
      fprintf(stderr, "Exception: invalid converion for parameter %s\n", argv[i]);
      exit(EXIT_FAILURE);
    }

    std::string option =
        opts[i - 1];  // argv corresponding value is 1 more that parsedOpt
    parsedOpt[option] = tmp;
  }

  return parsedOpt;
}

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"port", "num_players", "num_hops"};
  std::unordered_map<std::string, int> parsedOpt = parseOpt(argc, argv, opts);
  for (auto iter = parsedOpt.begin(); iter != parsedOpt.end(); iter++) {
    std::cout << iter->first << ": " << iter->second << std::endl;
  }
}
