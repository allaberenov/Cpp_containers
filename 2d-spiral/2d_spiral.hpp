int** MakeSpiral(int n) {
  int** arr = new int*[n];
  for (int i = 0; i < n; i++) {
    arr[i] = new int[n];
  }
  int i, k = 0, l = 0;
  int m = n;
  /*  k - starting row index
      m - ending row index
      l - starting column index
      n - ending column index
      i - iterator
  */
  int count = 0;
  while (k < m && l < n) {
    for (i = l; i < n; ++i) {
      arr[k][i] = ++i;
    }
    k++;

    for (i = k; i < m; ++i) {
      arr[i][n - 1] = ++count;
    }
    n--;
    if (k < m) {
      for (i = n - 1; i >= l; --i) {
        arr[m - 1][i] = ++count;
      }
      m--;
    }

    if (l < n) {
      for (i = m - 1; i >= k; --i) {
        arr[i][l] = ++count;
      }
      l++;
    }
  }
  return arr;
}
