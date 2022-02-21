#ifndef POTATO_HPP
#define POTATO_HPP

#include <vector>

class Potato {
  std::vector<int> trace;

 public:
  void appendHistory(std::vector<int> & trace);
};
#endif
