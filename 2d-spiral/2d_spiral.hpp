int** MakeSpiral(int end_col) {
  int** arr = new int*[end_col];
  for (int i = 0; i < end_col; i++) {
    arr[i] = new int[end_col];
  }
  int iterator = 0;
  int start_row = 0;
  int start_col = 0;
  int end_row = end_col;
  int count = 0;
  while (start_row < end_row && start_col < end_col) {
    for (iterator = start_col; iterator < end_col; ++iterator) {
      arr[start_row][iterator] = ++iterator;
    }
    start_row++;

    for (iterator = start_row; iterator < end_row; ++iterator) {
      arr[iterator][end_col - 1] = ++count;
    }
    end_col--;
    if (start_row < end_row) {
      for (iterator = end_col - 1; iterator >= start_col; --iterator) {
        arr[end_row - 1][iterator] = ++count;
      }
      end_row--;
    }

    if (start_col < end_col) {
      for (iterator = end_row - 1; iterator >= start_row; --iterator) {
        arr[iterator][start_col] = ++count;
      }
      start_col++;
    }
  }
  return arr;
}
