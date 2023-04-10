#include <stdexcept>
#include <vector>

const size_t kChunkSize = 16;

template <typename T>
class Deque {
 public:
  Deque();
  Deque(const Deque& deque);
  explicit Deque(size_t count);
  Deque(size_t count, const T& value);
  ~Deque();

  Deque& operator=(const Deque& deque);
  const T& operator[](size_t index) const;
  T& operator[](size_t index);
  const T& at(size_t /*i*/) const;
  T& at(size_t /*i*/);

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;
  void push_back(const T& /*arg*/);
  void pop_back();
  void push_front(T /*arg*/);
  void pop_front();
  [[nodiscard]] size_t chunk_size() const;

  template <bool IsConst>
  class Iterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  Iterator<false> begin() {
    return Iterator<false>(head_chunk_ + head_index_ / kChunkSize,
                           head_index_ % kChunkSize, this);
  }

  Iterator<true> cbegin() {
    return Iterator<true>(head_chunk_ + head_index_ / kChunkSize,
                          head_index_ % kChunkSize, this);
  }

  Iterator<false> end() {
    return Iterator<false>(tail_chunk_ + tail_index_ / kChunkSize,
                           tail_index_ % kChunkSize, this);
  }

  Iterator<true> cend() {
    return Iterator<true>(tail_chunk_ + tail_index_ / kChunkSize,
                          tail_index_ % kChunkSize, this);
  }

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
  size_t deque_size_;
  std::vector<T*> chunks_;
  size_t tail_chunk_;
  size_t tail_index_;

  size_t head_chunk_;
  size_t head_index_;
};

template <typename T>
Deque<T>::Deque() {
  deque_size_ = 0;
  head_index_ = head_chunk_ = 0;
  tail_chunk_ = tail_index_ = 0;
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
    chunk_index_ = current_ch;
    index_ = ch_ind;
    deque_ = deq;
    size_ = deq->size();
  }

  reference operator*() const { return deque_->chunks_[chunk_index_][index_]; }

  T* operator->() const { return &deque_->chunks_[chunk_index_][index_]; }

  Iterator& operator++() {
    int total_index = chunk_index_ * kChunkSize + index_ + 1;
    chunk_index_ = total_index / kChunkSize;
    index_ = total_index % kChunkSize;
    return *this;
  }

  Iterator& operator--() {
    int total_index = chunk_index_ * kChunkSize + index_ - 1;
    chunk_index_ = total_index / kChunkSize;
    index_ = total_index % kChunkSize;
    return *this;
  }

  Iterator& operator+=(size_t number) {
    chunk_index_ += (number + index_) / kChunkSize;
    index_ = (index_ + number) % kChunkSize;
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
    int total_index = chunk_index_ * kChunkSize + index_;
    total_index += number;
    return Iterator(total_index / kChunkSize, total_index % kChunkSize, deque_);
  }

  Iterator operator-(size_t number) const { return *this + (-number); }

  friend difference_type operator-(const Iterator& iter1,
                                   const Iterator& iter2) {
    if (iter1 > iter2) {
      return iter1.chunk_index_ * kChunkSize + iter1.index_ -
             (iter2.chunk_index_ * kChunkSize + iter2.index_);
    }
    return iter2.chunk_index_ * kChunkSize + iter2.index_ -
           (iter1.chunk_index_ * kChunkSize + iter1.index_);
  }

  bool operator<(const Iterator& iter) const { return iter > *this; }
  bool operator>(const Iterator& iter) const {
    if (chunk_index_ > iter.chunk_index_) {
      return true;
    }
    if (chunk_index_ == iter.chunk_index_) {
      return index_ > iter.index_;
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
  Deque* deque_;
  size_t chunk_index_;
  size_t index_;
  size_t size_ = 0;
};

template <typename T>
size_t Deque<T>::chunk_size() const {
  return chunks_.size();
}

template <typename T>
Deque<T>::~Deque() {
  while (!this->empty()) {
    pop_back();
  }
  for (size_t i = 0; i < chunk_size(); ++i) {
    delete[] chunks_[i];
  }
  chunks_.clear();
  deque_size_ = 0;
}

template <typename T>
Deque<T>::Deque(const Deque& deque) {
  if (!deque.empty()) {
    while (chunks_.size() != deque.chunk_size()) {
      chunks_.push_back(reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]));
    }

    for (size_t i = 0, j = 0; i < deque.size(); ++i, ++j) {
      new (chunks_[i / kChunkSize] + i % kChunkSize) T(deque[i]);
    }
    deque_size_ = deque.size();
  } else {
    deque_size_ = 0;
  }
  head_chunk_ = 0;
  head_index_ = 0;
  tail_chunk_ = deque.size() / kChunkSize;
  tail_index_ = deque.size() % kChunkSize;
}

template <typename T>
Deque<T>::Deque(size_t count) {
  size_t chunks_size = count / kChunkSize;
  if (count % kChunkSize > 0) {
    ++chunks_size;
  }
  deque_size_ = count;
  for (size_t i = 0; i < chunks_size; ++i) {
    chunks_.push_back(reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]));
  }
  for (size_t i = 0; i < count; ++i) {
    new (chunks_[i / kChunkSize] + i % kChunkSize) T();
  }
  head_chunk_ = 0;
  head_index_ = 0;
  tail_chunk_ = count / kChunkSize;
  tail_index_ = count % kChunkSize;
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  size_t chunks_size = count / kChunkSize;
  if (count % kChunkSize > 0) {
    ++chunks_size;
  }
  chunks_.resize(chunks_size);
  deque_size_ = count;
  for (size_t i = 0; i < chunks_.size(); ++i) {
    chunks_[i] = reinterpret_cast<T*>(new char[kChunkSize * sizeof(T)]);
  }
  for (size_t i = 0; i < count; ++i) {
    new (chunks_[i / kChunkSize] + (i % kChunkSize)) T(value);
  }
  head_chunk_ = 0;
  head_index_ = 0;
  tail_chunk_ = count / kChunkSize;
  tail_index_ = count % kChunkSize;
}

template <typename T>
size_t Deque<T>::size() const {
  return deque_size_;
}

template <typename T>
bool Deque<T>::empty() const {
  return !this->size();
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  return chunks_[head_chunk_ + (index + head_index_) / kChunkSize]
                [(head_index_ + index) % kChunkSize];
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
  return *reinterpret_cast<T*>(chunks_[index / kChunkSize] +
                               (index % kChunkSize));
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return *reinterpret_cast<T*>(chunks_[index / kChunkSize] +
                               (index % kChunkSize));
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return *reinterpret_cast<T*>(chunks_[index / kChunkSize] +
                               (index % kChunkSize));
}

template <typename T>
void Deque<T>::push_back(const T& arg) {
  if (tail_chunk_ == chunk_size()) {
    chunks_.push_back(reinterpret_cast<T*>(new char[kChunkSize * sizeof(T)]));
    tail_index_ = 0;
  } else if (tail_index_ == kChunkSize) {
    chunks_.push_back(reinterpret_cast<T*>(new char[kChunkSize * sizeof(T)]));
    ++tail_chunk_;
    tail_index_ = 0;
  }
  new (&chunks_[tail_chunk_][tail_index_]) T(arg);
  ++tail_index_;
  ++deque_size_;
}

template <typename T>
void Deque<T>::pop_back() {
  if (tail_index_ == 0) {
    --tail_chunk_;
    tail_index_ = kChunkSize;
  }
  chunks_[tail_chunk_][--this->tail_index_].~T();
  if (tail_index_ == 0) {
    delete[] chunks_[tail_chunk_];
    chunks_.pop_back();
  }
  --deque_size_;
}

template <typename T>
void Deque<T>::push_front(T arg) {
  if (head_index_ == 0) {
    if (head_chunk_ > 0) {
      --head_chunk_;
    } else {
      chunks_.push_back(reinterpret_cast<T*>(new char[kChunkSize * sizeof(T)]));
      ++tail_chunk_;
      for (size_t i = chunk_size() - 1; i >= 1; --i) {
        std::swap(chunks_[i], chunks_[i - 1]);
      }
    }
    head_index_ = kChunkSize;
  }
  chunks_[head_chunk_][--head_index_] = arg;
  ++deque_size_;
}

template <typename T>
void Deque<T>::pop_front() {
  if (head_index_ == kChunkSize) {
    head_index_ = 0;
    ++head_chunk_;
  }
  chunks_[head_chunk_][head_index_].~T();
  ++head_index_;
  --deque_size_;
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& deque) {
  while (!this->empty()) {
    pop_back();
  }
  head_index_ = head_chunk_ = 0;
  tail_chunk_ = tail_index_ = 0;
  for (size_t i = 0; i < deque.size(); ++i) {
    push_back(deque[i]);
  }
  return *this;
}
