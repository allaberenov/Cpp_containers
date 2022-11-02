#pragma once

int* LowerBound(int* first, int* last, int val) {
  while (first < last - 1) {
    int middle = ((last - 1) - first) / 2;
    if (*(first + middle) > val) {
      last = first + middle - 1;
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