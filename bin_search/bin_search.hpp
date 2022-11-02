#pragma once

int* LowerBound(int* first, int* last, int val) {
  last--;
  int* begin = first;
  int* end = last;
  while (begin < end) {
    int middle = (end - begin) / 2;
    if (*(begin + middle) > val) {
      end = begin + middle;
      continue;
    }
    if (*(begin + middle) < val) {
      begin = begin + middle + 1;
      continue;
    }
    if (*(begin + middle) >= val) {
      return begin + middle;
    }
  }
  return last;
}