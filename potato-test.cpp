#include "potato.hpp"

#include <iostream>

int main() {
  Potato p(10);
  for (int i = 0; i < 10; i++) {
    p.appendHistory(i);
  }
  p.printTrace();

  Potato p2 = p;
  p.printTrace();
  for (int i = 0; i < 5; i++) {
    p2.appendHistory(i);
  }
  p2.printTrace();

  Potato p3;
  p3 = p2;
  p3.appendHistory(100);

  std::cout << "======= p's trace ========" << std::endl;
  p.printTrace();
  std::cout << "======= p2's trace ========" << std::endl;
  p2.printTrace();
  std::cout << "======= p3's trace ========" << std::endl;
  p3.printTrace();
}
