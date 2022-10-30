int** MakeSpiral(int n) {
  int** a = new int*[n];
  for (int i = 0; i < n; i++) {
    a[i] = new int[n];
  }
  int counter = 0;
  for(int i = 0; i < n; i++){
    for(int j = i; j < n - i - 1; j++){
      a[i][j] = ++counter;
    }
    for(int j = i; j < n - i - 1; j++){
      a[j][n - i - 1] = ++counter;
    }
    for(int j = n - i - 1; j >= i; j--){
      a[n - i - 1][j] = ++counter;
    }
    for(int j = n - i - 2; j > i; j--){
      a[j][i] = ++counter;
    }
  }
  return a;
}
