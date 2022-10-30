#pragma once
#include <vector>

std::vector<int> Factorize(int n) {
  std::vector<int> a;
  int i = 2;
  while (i < n) {
    if (!(n % i)) {
      a.push_back(i);
      n = n / i;
      continue ;
    }
    i++;
  }
  return a;
}
