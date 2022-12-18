#pragma once

#include <algorithm>
#include <vector>

#include "iostream"
template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 private:
  std::vector<std::vector<T>> matrix_;

 public:
  explicit Matrix();
  explicit Matrix(const std::vector<std::vector<T>>& /*arr*/);
  Matrix(T /*val*/);

  Matrix& operator+=(const Matrix& /*matrix*/);
  Matrix& operator-=(const Matrix& /*matrix*/);

  template <size_t K>
  Matrix<N, K, T> operator*=(const Matrix<M, K, T>& /*matrix*/);
  Matrix& operator*=(const T& /*val*/);

  T Trace();

  T& operator()(const size_t& /*line*/, const size_t& /*row*/);
  const T& operator()(const size_t& /*first*/, const size_t& /*second*/) const;
  bool operator==(const Matrix& /*matrix*/);
  bool operator!=(const Matrix& /*matrix*/);

  Matrix<M, N, T> Transposed();
};
template <size_t N, size_t M, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, M, T>::operator*=(const Matrix<M, K, T>& matrix) {
  Matrix<N, K, T> result;
  for (size_t k = 0; k < K; ++k) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(i, k) += this->matrix_[i][j] * matrix(j, k);
      }
    }
  }
  return result;
}

template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::Trace() {
  T res = 0;
  for (size_t i = 0; i < N; ++i) {
    res += matrix_[i][i];
  }
  return res;
}

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() {
  Matrix<M, N, T> res;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      res(j, i) = matrix_[i][j];
    }
  }
  return res;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator*=(const T& val) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] *= val;
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T, typename P>
Matrix<N, M, T> operator*(const Matrix<N, M, T>& left, const P& right) {
  Matrix result(left);
  return result *= right;
}

template <size_t N, size_t M, size_t L, typename T>
Matrix<N, M, T> operator*(const Matrix<N, L, T>& left,
                          const Matrix<L, M, T>& right) {
  Matrix result(left);
  return result *= right;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator+(const Matrix<N, M, T>& left,
                          const Matrix<N, M, T>& right) {
  Matrix<N, M, T> copy(left);
  return copy += right;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator-(const Matrix<N, M, T>& left,
                          const Matrix<N, M, T>& right) {
  Matrix<N, M, T> result(left);
  return result -= right;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix& matrix) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      this->matrix_[i][j] += matrix.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix& matrix) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      this->matrix_[i][j] -= matrix.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
const T& Matrix<N, M, T>::operator()(const size_t& first,
                                     const size_t& second) const {
  return matrix_[first][second];
}

template <size_t N, size_t M, typename T>
bool Matrix<N, M, T>::operator!=(const Matrix& matrix) {
  return !(*this == matrix);
}
template <size_t N, size_t M, typename T>
bool Matrix<N, M, T>::operator==(const Matrix& matrix) {
  if (M == matrix._width && M == matrix.height) {
    for (size_t i = 0; i < N; ++i) {
      if (matrix_[i] != matrix[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

template <size_t N, size_t M, typename T>
T& Matrix<N, M, T>::operator()(const size_t& line, const size_t& row) {
  return matrix_[line][row];
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(T val) {
  matrix_.resize(N);
  for (size_t i = 0; i < N; ++i) {
    matrix_[i].resize(M);
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] = val;
    }
  }
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(const std::vector<std::vector<T>>& arr) {
  matrix_ = arr;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix() {
  matrix_.resize(N);
  for (size_t i = 0; i < matrix_.size(); ++i) {
    matrix_[i].resize(M, 0);
  }
}
