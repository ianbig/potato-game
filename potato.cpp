#include "potato.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

Potato::Potato(int nhops_t) : currentIndex(0), nhops(nhops_t) {
  memset(trace, 0, sizeof(trace));
}

void Potato::appendHistory(size_t id) {
  assert(currentIndex < 512);
  trace[currentIndex] = id;
  currentIndex += 1;
}

void Potato::printTrace() {
  std::cout << "Trace of potato" << std::endl;
  for (size_t i = 0; i < currentIndex - 1; i++) {
    std::cout << trace[i] << ",";
  }

  std::cout << trace[currentIndex - 1] << std::endl;
}

Potato::Potato(const Potato & rhs) {
  nhops = rhs.nhops;
  currentIndex = rhs.currentIndex;
  for (size_t i = 0; i < currentIndex; i++) {
    trace[i] = rhs.trace[i];
  }
}

Potato & Potato::operator=(const Potato & rhs) {
  if (this != &rhs) {
    nhops = rhs.nhops;
    currentIndex = rhs.currentIndex;
    for (size_t i = 0; i < currentIndex; i++) {
      trace[i] = rhs.trace[i];
    }
  }

  return *this;
}
