
#include <cmath>
#include <stdexcept>
#include <vector>

const size_t CHUNK_SIZE = 16;

template <typename T>
class Deque {
 public:
  Deque();
  Deque(const Deque&);
  Deque(size_t count);
  Deque(size_t count, const T& value);
  ~Deque();

  Deque& operator=(const Deque& deque);
  const T& operator[](size_t i) const;
  T& operator[](size_t i);
  const T& at(size_t) const;
  T& at(size_t);

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;
  void push_back(const T&);
  void pop_back();
  void push_front(T);
  void pop_front();
  [[nodiscard]] size_t chunk_size() const;


  template <bool IsConst>
  class common_iterator;
  common_iterator<false> begin() { return common_iterator<false>(0, this); }

  common_iterator<true> cbegin() { return common_iterator<true>(0, this); }

  common_iterator<false> end() { return common_iterator<false>(size(), this); }

  common_iterator<true> cend() { return common_iterator<true>(size(), this); }


 private:
  size_t deque_size;
  std::vector<T*> chunks;
  std::vector<size_t> free_place;

  bool HeadAvailability();
  bool TailAvailability();
};
template <typename T>
Deque<T>::Deque() {
  deque_size = 0;
}

template <typename T>
template <bool IsConst>
class Deque<T>::common_iterator {
 public:
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;
  using iterator_category = std::random_access_iterator_tag;
  using pointer = T*;

  common_iterator(size_t ind, Deque* deq) {
    index = ind;
    deque = deq;
    size = deq->size();
  }

  reference operator*() const {
    return *reinterpret_cast<T*>(deque->chunks[index / CHUNK_SIZE] +
                                 index % CHUNK_SIZE);
  }

  pointer operator->() {
    return reinterpret_cast<T*>(deque->chunks[index / CHUNK_SIZE] +
                                index % CHUNK_SIZE);
  }

  iterator operator++() {
    ++index;
    return *this;
  };

  iterator operator++(int) { return iterator(index + 1); }

  iterator& operator--() {
    --index;
    return *this;
  }

  iterator operator--(int) { return iterator(index - 1); }
  iterator& operator+=(size_t number) {
    index += number;
    return *this;
  }

  const_iterator operator++() const {
    return common_iterator(index + 1, deque);
  }

  const_iterator operator++(int) const {
    const_iterator iterator(index + 1, deque);
    return iterator;
  }

  const_iterator& operator--() const {
    const_iterator iterator(index - 1, deque);
    return iterator;
  }

  const_iterator operator--(int) const {
    const_iterator iterator(index - 1, deque);
    return iterator;
  }

  iterator operator+(size_t number) {
    index += number;
    return *this;
  }

  iterator operator-(size_t number) {
    index -= number;
    return *this;
  }

  const_iterator operator+(size_t number) const { return common_iterator(index + number, this); }
  const_iterator operator-(size_t number) const { return common_iterator(index - number, this); }
  bool operator<(const common_iterator& iter) const { return index < iter.index; }
  bool operator>(const common_iterator& iter) const { return index > iter.index; }
  bool operator==(const common_iterator& iter) const { return index == iter.index; }
  bool operator<=(const common_iterator& iter) const {
    return (this < iter) || (this == iter);
  }
  bool operator>=(const common_iterator& iter) const {
    return (this > iter) || (this == iter);
  }
  bool operator!=(const common_iterator& iter) const { return !(this == iter); }
  size_t operator-(const common_iterator& iter) {
    return std::abs(static_cast<long int>(this->index - iter.index));
  }

 private:
  Deque* deque;
  size_t index;
  size_t size = 0;
};

template <typename T>
size_t Deque<T>::chunk_size() const {
  return chunks.size();
}

template <typename T>
Deque<T>::~Deque() {
  if (!this->empty()) {
    for (size_t i = 0; i < size(); ++i) {
      chunks[i / CHUNK_SIZE][i % CHUNK_SIZE].~T ();
    }
  }
  for (size_t i = 0; i < chunk_size(); ++i) {
    delete[] reinterpret_cast<char*>(chunks[i]);
  }
  chunks.clear();
  free_place.clear();
  deque_size = 0;
}

template <typename T>
Deque<T>::Deque(const Deque& deque) {
  if (!deque.empty()) {
    chunks.resize(deque.chunk_size(),
                  reinterpret_cast<T*>(new char[sizeof(T) * CHUNK_SIZE]));
    free_place.resize(deque.chunk_size(), CHUNK_SIZE);
    for (size_t i = 0, j = 0; i < deque.size(); ++i, ++j) {
      new (chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(deque[i]);
      --free_place[i / CHUNK_SIZE];
    }
    deque_size = deque.size();
  } else {
    deque_size = 0;
  }
}

template <typename T>
Deque<T>::Deque(size_t count) {
  size_t chunks_size = count / CHUNK_SIZE + !!(count % CHUNK_SIZE);
  deque_size = count;
  for (size_t i = 0; i < chunks_size; ++i) {
    chunks.push_back(reinterpret_cast<T*>(new char[sizeof(T) * CHUNK_SIZE]));
  }
  free_place.resize(chunk_size(), CHUNK_SIZE);
  for (size_t i = 0; i < count; ++i) {
    new (chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T();
    --free_place[i / CHUNK_SIZE];
  }
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  chunks.resize(count / CHUNK_SIZE + !!(count % CHUNK_SIZE));
  free_place.resize(chunks.size(), CHUNK_SIZE);
  deque_size = count;
  for (size_t i = 0; i < chunks.size(); ++i) {
    chunks[i] = reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]);
  }
  for (size_t i = 0; i < count; ++i) {
    new (chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE)) T(value);
    free_place[i / CHUNK_SIZE]--;
  }
}

template <typename T>
size_t Deque<T>::size() const {
  return deque_size;
}

template <typename T>
bool Deque<T>::empty() const {
  return !this->size();
}

template <typename T>
const T& Deque<T>::operator[](size_t i) const {
  return *reinterpret_cast<T*>(chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE));
}

template <typename T>
T& Deque<T>::operator[](size_t i) {
  return *reinterpret_cast<T*>(chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE));
}

template <typename T>
T& Deque<T>::at(size_t i) {
  if (i >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return *reinterpret_cast<T*>(chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE));
}

template <typename T>
const T& Deque<T>::at(size_t i) const {
  if (i >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return *reinterpret_cast<T*>(chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE));
}

template <typename T>
bool Deque<T>::HeadAvailability() {
  return free_place.front() > 0;
}

template <typename T>
bool Deque<T>::TailAvailability() {
  return free_place.back() > 0;
}

template <typename T>
void Deque<T>::push_back(const T& arg) {
  if (free_place.back() == 0) {
    chunks.push_back(reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]));
    free_place.push_back(CHUNK_SIZE);
  }
  chunks[size() / CHUNK_SIZE][size() % CHUNK_SIZE] = arg;
  deque_size++;
  free_place.back()--;
}

template <typename T>
void Deque<T>::pop_back() {
  reinterpret_cast<T*>(chunks.back() + (CHUNK_SIZE - free_place.back()))->~T();
  if (free_place.back() == CHUNK_SIZE - 1) {
    delete[] reinterpret_cast<char*>(chunks.back());
    chunks.pop_back();
    free_place.pop_back();
  } else {
    free_place.back()++;
  }
}

template <typename T>
void Deque<T>::push_front(T arg) {
  if (free_place.front() == 0) {
    chunks.push_back(new char[CHUNK_SIZE * sizeof(T)]);
    reinterpret_cast<T*>(chunks.front() + (CHUNK_SIZE - 1)) = arg;
    free_place.push_back(CHUNK_SIZE);
    for (size_t i = chunk_size() - 1; i >= 1; --i) {
      std::swap(chunks[i], chunks[i - 1]);
      std::swap(free_place[i], free_place[i - 1]);
    }
  }
  --free_place.front();
  chunks.front()[free_place.front()] = arg;
}

template <typename T>
void Deque<T>::pop_front() {
  if (free_place.front() == CHUNK_SIZE - 1) {
    reinterpret_cast<T*>(chunks.front() + (free_place.front() - 1))->~T();
    for (size_t i = 0; i <= chunk_size() - 2; ++i) {
      std::swap(chunks[i], chunks[i + 1]);
      std::swap(free_place[i], free_place[i + 1]);
    }
    delete[] chunks.back();
    chunks.pop_back();
    free_place.pop_back();
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& deque) {
  if (!this->empty()) {
    for (size_t i = 0; i < size(); ++i) {
      reinterpret_cast<T*>(chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE))->~T();
    }
    for (auto& i : chunks) {
      delete[] i;
    }
    chunks.clear();
    free_place.clear();
  }
  chunks.resize(deque.chunk_size(),
                reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]));
  free_place.resize(deque.chunk_size(), CHUNK_SIZE);
  deque_size = deque.size();
  for (size_t i = 0; i < deque.size(); ++i) {
    T* ptr = chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE);
    new (chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE)) T(deque[i]);
    ++free_place[i / CHUNK_SIZE];
  }
  return *this;
}