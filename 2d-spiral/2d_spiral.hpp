int** MakeSpiral(int n) {
  int** arr = new int*[n];
  for (int i = 0; i < n; i++) {
    arr[i] = new int[n];
  }
  int counter = 0;
  int iterator = 0;
  while (iterator < n) {
    for (int j = iterator; j < n - iterator - 1; ++j) {
      arr[iterator][j] = ++counter;
    }
    for (int j = iterator; j < n - iterator - 1; ++j) {
      arr[j][n - iterator - 1] = ++counter;
    }
    for (int j = n - iterator - 1; j >= iterator; --j) {
      arr[n - iterator - 1][j] = ++counter;
    }
    for (int j = n - iterator - 2; j > iterator; --j) {
      arr[j][iterator] = ++counter;
    }
    ++iterator;
  }
  return arr;
}