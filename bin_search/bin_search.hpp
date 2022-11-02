#pragma once

int *LowerBound(int *first, int *last, int val) {
  last--;
  while (first < last) {
    int middle = (last - first) / 2;
    if (*(first + middle) > val) {
      last = first + middle;
      continue;
    }
    if (*(first + middle) < val) {
      first = first + middle + 1;
      continue;
    }
    if (*(first + middle) >= val) {
      return first + middle;
    }
  }
  return last;
}
