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

  Deque& operator=(const Deque& other);
  T operator[](size_t i) const;
  T& operator[](size_t i);
  T at(size_t) const;
  T& at(size_t);

  size_t size() const;

  bool empty() const;
  void push_back(T);
  void pop_back();
  void push_front(T);
  void pop_front();

  size_t chunk_size() const;

 private:
  size_t deque_size;
  std::vector<T*> chunks;
  std::vector<size_t> free_place;

  bool HeadAvailability();
  bool TailAvailability();
};

template <typename T>
size_t Deque<T>::chunk_size() const {
  return chunks.size();
}

template <typename T>
Deque<T>::~Deque() {
  for (auto& i : chunks) {
    delete[] i;
  }
  chunks.clear();
  deque_size = 0;
}

template <typename T>
Deque<T>::Deque(const Deque& deque) {
  chunks.resize(deque.chunk_size());
  free_place.resize(deque.chunk_size(), CHUNK_SIZE);
  for (size_t i = 0; i < deque.chunk_size(); ++i) {
    chunks[i] = new T(CHUNK_SIZE);
    for (size_t j = 0; j < CHUNK_SIZE && j + i * CHUNK_SIZE < deque.size();
         ++j) {
      chunks[i][j] = deque[i * CHUNK_SIZE + j];
      free_place[i]--;
    }
  }
  deque_size = deque.size();
}

template <typename T>
Deque<T>::Deque(size_t count) {
  size_t chunks_size = (count / CHUNK_SIZE + !!(count % CHUNK_SIZE));
  free_place.resize(chunk_size(), CHUNK_SIZE);
  for (size_t i = 0; i < chunks_size; ++i) {
    chunks.push_back(new T(CHUNK_SIZE));
  }
  deque_size = count;
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  size_t chunks_size = count / CHUNK_SIZE + !!(count % CHUNK_SIZE);
  free_place.resize(chunks_size, CHUNK_SIZE);
  deque_size = count;
  for (size_t i = 0; i < chunks_size; ++i) {
    chunks.push_back(new T(CHUNK_SIZE));
    for (size_t j = 0; j < CHUNK_SIZE && j + 1 + i * CHUNK_SIZE < count; ++j) {
      chunks[i][j] = value;
      free_place[i]--;
    }
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  if (!this->empty()) {
    for (size_t i = int(chunk_size() - 1); i >= 0; --i) {
      delete[] chunks[i];
    }
  }
  chunks.clear();
  deque_size = other.size();
  free_place.resize(chunk_size(), CHUNK_SIZE);

  for (size_t i = 0; i < chunk_size(); ++i) {
    chunks.push_back(new T(CHUNK_SIZE));
    for (size_t j = 0; j < CHUNK_SIZE && j + 1 + i * CHUNK_SIZE < other.size();
         ++j) {
      chunks[i][j] = other[i * CHUNK_SIZE + j];
      free_place[i]--;
    }
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
T Deque<T>::operator[](size_t i) const {
  return chunks[i / CHUNK_SIZE][i % CHUNK_SIZE];
}

template <typename T>
T& Deque<T>::operator[](size_t i) {
  return chunks[i / CHUNK_SIZE][i % CHUNK_SIZE];
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return chunks[index / CHUNK_SIZE][index % CHUNK_SIZE];
}

template <typename T>
T Deque<T>::at(size_t index) const {
  if (index >= this->size()) {
    throw std::out_of_range("Out of range");
  }
  return chunks[index];
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
void Deque<T>::push_back(T arg) {
  if (free_place.back() == 0) {
    chunks.push_back(new T(CHUNK_SIZE));
    free_place.push_back(CHUNK_SIZE);
  }
  chunks[deque_size] = arg;
  deque_size++;
  free_place.back()--;
}

template <typename T>
void Deque<T>::pop_back() {
  if (free_place.back() == CHUNK_SIZE - 1) {
    delete [] chunks.back();
    chunks.pop_back();
    free_place.pop_back();
  } else {
    free_place.back()++;
  }
}

template <typename T>
void Deque<T>::push_front(T arg) {
  if (free_place.front() == 0) {
    chunks.pushback();
    T* next;
    T* temp;
    for (size_t i = 1; i < chunk_size(); ++i) {
      if (i == 1) {
        next = chunks[i];
        chunks[i] = chunks[i - 1];
      }
      temp = chunks[i];
      chunks[i] = next;
      next = temp;
    }
    free_place.front() = CHUNK_SIZE;
  }
  chunks.front()[free_place.front() - 1] = arg;
  free_place.front()--;
}

template <typename T>
void Deque<T>::pop_front() {
  if (free_place.front() == CHUNK_SIZE - 1) {
    T* trash = chunks.front();
    for (size_t i = 1; i < chunk_size(); ++i) {
      chunks[i - 1] = chunks[i];
      free_place[i - 1] = free_place[i];
    }
    free_place.pop_back();
    chunks.back() = trash;
    delete[] chunks.back();
    chunks.pop_back(chunks.back());
  }
  free_place.front()++;
}

template <typename T>
Deque<T>::Deque() {
  deque_size = 0;
}
