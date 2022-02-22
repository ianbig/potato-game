#ifndef POTATO_HPP
#define POTATO_HPP

#include <vector>

class Potato {
  int trace[512];
  size_t currentIndex;

 public:
  int nhops;
  Potato() {}
  Potato(int nhops_t);
  Potato(const Potato & rhs);
  Potato & operator=(const Potato & rhs);
  void appendHistory(size_t id);
  void printTrace();
};
#endif
