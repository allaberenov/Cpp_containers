#include "deque.hpp"
#include "iostream"

int main() {
  Deque<size_t> deque(10, 5);
  std::cout << *deque.begin() << '\t' << *deque.rbegin();
}
