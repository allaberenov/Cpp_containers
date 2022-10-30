#pragma once
#include <vector>

std::vector<int> Factorize(int n) {
  std::vector<int> a;
  int i = 2;
  while (i < n) {
    if ((n % i) != 0) {
      a.push_back(i);
      n = n / i;
      continue;
    }
    i++;
  }
  if (a != 0 || a ! = 1) {
    a.push_back(n);
  }
  return a;
}
