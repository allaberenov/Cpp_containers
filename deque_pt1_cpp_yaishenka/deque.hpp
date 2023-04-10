#include <stdexcept>
#include <vector>

const size_t CHUNK_SIZE = 16;

template <typename T>
class Deque {
 public:
  Deque();
  Deque(const Deque&);
  explicit Deque(size_t count);
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
  class Iterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  Iterator<false> begin() {
    return Iterator<false>(head_chunk + head_index / CHUNK_SIZE, head_index % CHUNK_SIZE, this);
  }

  Iterator<true> cbegin() {
    return Iterator<true>(head_chunk, head_index, this);
  }

  Iterator<false> end() {
    return Iterator<false>(tail_chunk + tail_index / CHUNK_SIZE, tail_index %  CHUNK_SIZE, this);
  }

  Iterator<true> cend() { return Iterator<true>(tail_chunk, tail_index, this); }

  void insert(iterator iter, const T& arg) {
    T temp = arg;
    for (auto i = iter; i < end(); ++i) {
      std::swap(*i, temp);
    }
    push_back(temp);
  }

  void erase(iterator iter) {
    for (auto i = iter + 1; i < end(); ++i) {
      std::swap(*(i - 1), *i);
    }
    pop_back();
  }

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end() - 1);
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin() - 1);
  }

 private:
  size_t deque_size;
  std::vector<T*> chunks;
  size_t tail_chunk;
  size_t tail_index;

  size_t head_chunk;
  size_t head_index;

  [[nodiscard]] size_t GetTail() const;
  [[nodiscard]] size_t GetHead() const;
};
template <typename T>
Deque<T>::Deque() {
  deque_size = 0;
  head_index = head_chunk = 0;
  tail_chunk = tail_index = 0;
}

template <typename T>
template <bool IsConst>
class Deque<T>::Iterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using iterator_category = std::random_access_iterator_tag;

  Iterator(size_t current_ch, size_t ch_ind, Deque* deq) {
    chunk_index = current_ch;
    index = ch_ind;
    deque = deq;
    size = deq->size();
  }

  reference operator*() const { return deque->chunks[chunk_index][index]; }

  T* operator->() const { return &deque->chunks[chunk_index][index]; }

  Iterator& operator++() {
    int total_index = chunk_index * CHUNK_SIZE + index + 1;
    chunk_index = total_index / CHUNK_SIZE;
    index = total_index % CHUNK_SIZE;
    return *this;
  }

  Iterator& operator--() {
    int total_index = chunk_index * CHUNK_SIZE + index - 1;
    chunk_index = total_index / CHUNK_SIZE;
    index = total_index % CHUNK_SIZE;
    return *this;
  }

  Iterator& operator+=(size_t number) {
    chunk_index += (number + index) / CHUNK_SIZE;
    index = (index + number) % CHUNK_SIZE;
    return *this;
  }

  Iterator operator++(int) {
    Iterator copy = *this;
    ++*this;
    return copy;
  }

  Iterator operator--(int) {
    Iterator copy = *this;
    --*this;
    return copy;
  }

  Iterator operator+(size_t number) const {
    int total_index = chunk_index * CHUNK_SIZE + index;
    total_index += number;
    return Iterator(total_index / CHUNK_SIZE, total_index % CHUNK_SIZE, deque);
  }

  Iterator operator-(size_t number) const { return *this + (-number); }

  friend difference_type operator-(const Iterator& iter1,
                                   const Iterator& iter2) {
    if (iter1 > iter2) {
      return iter1.chunk_index * CHUNK_SIZE + iter1.index -
             (iter2.chunk_index * CHUNK_SIZE + iter2.index);
    }
    return iter2.chunk_index * CHUNK_SIZE + iter2.index -
           (iter1.chunk_index * CHUNK_SIZE + iter1.index);
  }

  bool operator<(const Iterator& iter) const { return iter > *this; }
  bool operator>(const Iterator& iter) const {
    if (chunk_index > iter.chunk_index) {
      return true;
    }
    if (chunk_index == iter.chunk_index) {
      return index > iter.index;
    }
    return false;
  }

  bool operator==(const Iterator& iter) const {
    return !(*this > iter) && !(*this < iter);
  }
  bool operator<=(const Iterator& iter) const {
    return (*this < iter) || (*this == iter);
  }
  bool operator>=(const Iterator& iter) const {
    return (*this > iter) || (*this == iter);
  }
  bool operator!=(const Iterator& iter) const { return !(*this == iter); }

 private:
  Deque* deque;
  size_t chunk_index;
  size_t index;
  size_t size = 0;
};

template <typename T>
size_t Deque<T>::chunk_size() const {
  return chunks.size();
}

template <typename T>
Deque<T>::~Deque() {
  while (!this->empty()) {
    pop_back();
  }
  for (size_t i = 0; i < chunk_size(); ++i) {
    delete[] chunks[i];
  }
  chunks.clear();
  deque_size = 0;
}

template <typename T>
Deque<T>::Deque(const Deque& deque) {
  if (!deque.empty()) {
    while (chunks.size() != deque.chunk_size()) {
      chunks.push_back(reinterpret_cast<T*>(new char[sizeof(T) * CHUNK_SIZE]));
    }

    for (size_t i = 0, j = 0; i < deque.size(); ++i, ++j) {
      new (chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T(deque[i]);
    }
    deque_size = deque.size();
  } else {
    deque_size = 0;
  }
  head_chunk = 0;
  head_index = 0;
  tail_chunk = deque.size() / CHUNK_SIZE;
  tail_index = deque.size() % CHUNK_SIZE;
}

template <typename T>
Deque<T>::Deque(size_t count) {
  size_t chunks_size = count / CHUNK_SIZE + !!(count % CHUNK_SIZE);
  deque_size = count;
  for (size_t i = 0; i < chunks_size; ++i) {
    chunks.push_back(reinterpret_cast<T*>(new char[sizeof(T) * CHUNK_SIZE]));
  }
  for (size_t i = 0; i < count; ++i) {
    new (chunks[i / CHUNK_SIZE] + i % CHUNK_SIZE) T();
  }
  head_chunk = 0;
  head_index = 0;
  tail_chunk = count / CHUNK_SIZE;
  tail_index = count % CHUNK_SIZE;
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  chunks.resize(count / CHUNK_SIZE + !!(count % CHUNK_SIZE));
  deque_size = count;
  for (size_t i = 0; i < chunks.size(); ++i) {
    chunks[i] = reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]);
  }
  for (size_t i = 0; i < count; ++i) {
    new (chunks[i / CHUNK_SIZE] + (i % CHUNK_SIZE)) T(value);
  }
  head_chunk = 0;
  head_index = 0;
  tail_chunk = count / CHUNK_SIZE;
  tail_index = count % CHUNK_SIZE;
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
  return chunks[head_chunk + (i + head_index) / CHUNK_SIZE]
               [(head_index + i) % CHUNK_SIZE];
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
void Deque<T>::push_back(const T& arg) {
  if (tail_chunk == chunk_size()) {
    chunks.push_back(reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]));
    tail_index = 0;
  } else if (tail_index == CHUNK_SIZE) {
    chunks.push_back(reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]));
    ++tail_chunk;
    tail_index = 0;
  }
  chunks[tail_chunk][tail_index] = arg;
  ++tail_index;
  ++deque_size;
}

template <typename T>
void Deque<T>::pop_back() {
  if (tail_index == 0) {
    --tail_chunk;
    tail_index = CHUNK_SIZE - 1;
  }
  chunks[tail_chunk][this->tail_index].~T();
  if (tail_index == 0) {
    delete[] chunks[tail_chunk];
    chunks.pop_back();
  }
  --deque_size;
}

template <typename T>
void Deque<T>::push_front(T arg) {
  if (head_index == 0) {
    if (head_chunk > 0) {
      --head_chunk;
    } else {
      chunks.push_back(reinterpret_cast<T*>(new char[CHUNK_SIZE * sizeof(T)]));
      ++tail_chunk;
      for (size_t i = chunk_size() - 1; i >= 1; --i) {
        std::swap(chunks[i], chunks[i - 1]);
      }
    }
    head_index = CHUNK_SIZE;
  }
  chunks[head_chunk][--head_index] = arg;
  ++deque_size;
}

template <typename T>
void Deque<T>::pop_front() {
  if (head_index == CHUNK_SIZE) {
    head_index = 0;
    ++head_chunk;
  }
  chunks[head_chunk][head_index].~T();
  ++head_index;
  --deque_size;
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& deque) {
  while (!this->empty()) {
    pop_back();
  }
  head_index = head_chunk = 0;
  tail_chunk = tail_index = 0;
  for (size_t i = 0; i < deque.size(); ++i) {
    push_back(deque[i]);
  }
  return *this;
}
