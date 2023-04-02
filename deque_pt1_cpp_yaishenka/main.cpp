#include "deque.hpp"
#include "iostream"

int main() {
  Deque<std::string> deque(10, "Kerim");
  Deque<int> first(5, 4);
  Deque<int> second(10, 9);
  first = second;
  for (size_t i = 0; i < deque.size(); ++i) {
    std::cout << deque[i] << '\t';
  }
  std::cout << '\n';
  for (size_t i = 0; i < first.size(); ++i) {
    std::cout << first[i] << '\t';
  }
  std::cout << '\n';
  for (size_t i = 0; i < second.size(); ++i) {
    std::cout << second[i] << '\t';
  }
  std::cout << '\n';
}
