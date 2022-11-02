int* LowerBound(int* first, int* last, int val) {
  last--;
  while (first <= last) {
    int middle = (last - first) / 2;
    if (first == last && *first >= val) {
      return first;
    }
    if (*last > val) {
      last = first + middle;
      continue;
    }
    if (*first < val) {
      first = first + middle + 1;
      continue;
    }
    if (*(first + middle) > val) {
      return first + middle;
    }
  }
  last++;
  return last;
}