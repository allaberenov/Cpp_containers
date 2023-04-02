#include "deque.hpp"
#include "iostream"

int main() {
  Deque<int> deque(7, 4);
  Deque<int> first(10, 10);
  Deque<int> second(9, 9);
  deque = first;
  for (size_t i = 0; i < deque.size(); ++i) {
    std::cout << deque[i] << '\t';
  }
}
