#pragma once

#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
  struct Node {
    T value;
    Node* next = nullptr;
    Node* prev = nullptr;

    Node() = default;
    explicit Node(const T& arg) : value(arg) {}
    explicit Node(T&& arg) : value(std::move(arg)) {}
    ~Node() = default;
  };

 public:
  using allocator_type = Allocator;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using AllocTraits = std::allocator_traits<allocator_type>;
  using NodeAlloc = typename AllocTraits::template rebind_alloc<Node>;
  using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

  template <bool IsConst>
  struct Iterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

 private:
  Node* head_;
  Node* tail_;
  size_t size_;
  NodeAlloc alloc_;

  void clear();

 public:
  List();

  List(size_t count, const T& value, const Allocator& alloc = Allocator());

  explicit List(size_t count, const Allocator& alloc = Allocator());

  List(const List& other);

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  ~List();

  Iterator<false> begin();

  Iterator<true> begin() const;

  Iterator<false> rbegin();

  Iterator<true> cbegin();

  Iterator<true> crbegin();

  Iterator<false> end();

  Iterator<true> end() const;

  Iterator<false> rend();

  Iterator<true> cend();

  Iterator<true> crend();

  T& front();

  const T& front() const;

  T& back();

  const T& back() const;

  bool empty() { return size_ == 0; }

  size_t size() const { return size_; };

  void push_back(const T& /*arg*/);

  void push_back(T&& /*arg*/);

  void push_back();

  void pop_back();

  void push_front(const T& /*arg*/);

  void push_front(T&& /*arg*/);

  void pop_front();

  allocator_type get_allocator() const { return allocator_type(alloc_); }

  void set_allocator(const allocator_type& allocator) { alloc_ = allocator; }

  List& operator=(const List& other);
};
template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  while (!empty()) {
    pop_back();
  }
}
template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (this == &other) {
    return *this;
  }
  clear();
  if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.get_allocator();
  }

  for (const auto& i : other) {
    push_back(i);
  }
  return *this;
}

template <typename T, typename Alloc>
List<T, Alloc>::List() {
  size_ = 0;
  head_ = tail_ = nullptr;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) {
  head_ = tail_ = nullptr;
  size_ = 0;
  alloc_ = alloc;
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
  while (!empty()) {
    pop_back();
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const Alloc& alloc) {
  try {
    alloc_ = alloc;
    head_ = tail_ = nullptr;
    size_ = 0;
    for (size_t i = 0; i < count; ++i) {
      push_back();
    }
  } catch (...) {
    while (!empty()) {
      pop_back();
    }
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) {
  head_ = tail_ = nullptr;
  size_ = 0;
  alloc_ = NodeAllocTraits::select_on_container_copy_construction(other.get_allocator());
  try {
    for (const auto& i : other) {
      push_back(i);
    }
  } catch (...) {
    clear();
    NodeAllocTraits::deallocate(alloc_, head_, 1);
    head_ = tail_ = nullptr;
    size_ = 0;
    throw;
  }
}


template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init,
                         const Allocator& alloc) {
  alloc_ = alloc;
  head_ = tail_ = nullptr;
  size_ = 0;
  for (const auto& i : init) {
    push_back(i);
  }
}

template <typename T, typename Alloc>
template <bool IsConst>
struct List<T, Alloc>::Iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using difference_type = std::ptrdiff_t;

  Node* current;
  Node* head;
  Node* tail;
  size_t index;

  Iterator<IsConst>(Node* cur, Node* top = nullptr, Node* end = nullptr,
                    size_t ind = 0) {
    head = top;
    tail = end;
    current = cur;
    index = ind;
  }

  Iterator& operator++() {
    if (current != nullptr) {
      current = current->next;
    } else {
      current = tail;
    }
    ++index;
    return *this;
  }

  Iterator operator++(int) {
    Iterator copy(current);
    if (current != nullptr) {
      current = current->next;
    } else {
      current = head;
    }
    ++index;
    return copy;
  }

  Iterator& operator--() {
    if (current != nullptr) {
      current = current->prev;
    } else {
      current = tail;
    }
    --index;
    return *this;
  }

  Iterator operator--(int) {
    Iterator copy(current);
    if (current != nullptr) {
      current = current->prev;
    } else {
      current = tail;
    }
    --index;
    return copy;
  }

  reference operator*() { return current->value; }

  reference operator*() const { return current->value; }

  friend bool operator==(const Iterator& iter1, const Iterator& iter2) {
    if (iter1.current != nullptr && iter2.current != nullptr ||
        iter1.index == iter2.index) {
      if (iter1.current == iter2.current) {
        return true;
      }
      if ((iter1.current->next == nullptr) ^ (iter2.current->next == nullptr) ||
          (iter1.current->prev == nullptr) ^ (iter2.current->prev == nullptr)) {
        return false;
      }
      return iter1.current->next == iter2.current->next &&
             iter1.current->prev == iter2.current->prev;
    }
    return false;
  }

  friend bool operator!=(const Iterator& iter1, const Iterator& iter2) {
    return !(iter1 == iter2);
  }

  const_pointer operator->() const { return &current->value; }
};

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<false> List<T, Alloc>::begin() {
  return Iterator<false>(head_, head_, tail_, 0);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<false> List<T, Alloc>::rbegin() {
  return Iterator<false>(tail_, head_, tail_, size() - 1);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<false> List<T, Alloc>::end() {
  return Iterator<false>(tail_->next, head_, tail_, size());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<false> List<T, Alloc>::rend() {
  return Iterator<false>(head_->prev, head_, tail_, -1);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::begin() const {
  return Iterator<true>(head_, head_, tail_, 0);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::end() const {
  return Iterator<true>(tail_->next, head_, tail_, size());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::cbegin() {
  return Iterator<true>(head_, head_, tail_, 0);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::cend() {
  return Iterator<true>(tail_->next, head_, tail_, size());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::crbegin() {
  return List::Iterator<true>(tail_, head_, tail_, size() - 1);
}
template <typename T, typename Alloc>
typename List<T, Alloc>::template Iterator<true> List<T, Alloc>::crend() {
  return Iterator<true>(head_->prev, head_, tail_, -1);
}

template <typename T, typename Alloc>
T& List<T, Alloc>::front() {
  return head_->value;
}

template <typename T, typename Alloc>
const T& List<T, Alloc>::front() const {
  return head_->value;
}

template <typename T, typename Alloc>
T& List<T, Alloc>::back() {
  return tail_->value;
}

template <typename T, typename Alloc>
const T& List<T, Alloc>::back() const {
  return tail_->value;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& arg) {
  Node* new_node = nullptr;
  try {
    new_node = NodeAllocTraits::allocate(alloc_, 1);
    NodeAllocTraits::construct(alloc_, new_node, std::move(arg));
    if (size() > 0) {
      tail_->next = new_node;
      new_node->prev = tail_;
      tail_ = tail_->next;

    } else {
      tail_ = head_ = new_node;
    }
    ++size_;
  } catch (...) {
    NodeAllocTraits::destroy(alloc_, new_node);
    NodeAllocTraits::deallocate(alloc_, new_node, 1);
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back() {
  Node* new_node = nullptr;
  try {
    new_node = NodeAllocTraits::allocate(alloc_, 1);
    NodeAllocTraits::construct(alloc_, new_node);
    if (size() > 0) {
      tail_->next = new_node;
      new_node->prev = tail_;
      tail_ = tail_->next;
    } else {
      head_ = tail_ = new_node;
    }
    ++size_;
  } catch (...) {
    NodeAllocTraits::destroy(alloc_, new_node);
    NodeAllocTraits::deallocate(alloc_, new_node, 1);
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(T&& arg) {
  auto new_node = NodeAllocTraits::allocate(alloc_, 1);
  NodeAllocTraits::construct(alloc_, new_node, arg);
  if (size() > 0) {
    tail_->next = new_node;
    new_node->prev = tail_;
    tail_ = tail_->next;
  } else {
    head_ = tail_ = new_node;
  }
  ++size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& arg) {
  auto new_node = NodeAllocTraits::allocate(alloc_, 1);
  NodeAllocTraits::construct(alloc_, new_node, arg);
  if (size() > 0) {
    head_->prev = new_node;
    new_node->next = head_;
    head_ = head_->prev;
  } else {
    head_ = tail_ = new_node;
  }
  ++size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(T&& arg) {
  auto new_node = NodeAllocTraits::allocate(alloc_, 1);
  NodeAllocTraits::construct(alloc_, new_node, arg);
  if (size() > 0) {
    head_->prev = new_node;
    new_node->next = head_;
    head_ = head_->prev;
  } else {
    tail_ = head_ = new_node;
  }
  ++size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  Node* temp = head_;
  if (size() == 1) {
    tail_ = head_ = nullptr;
  } else {
    head_ = head_->next;
    head_->prev = nullptr;
  }
  NodeAllocTraits::destroy(alloc_, temp);
  NodeAllocTraits::deallocate(alloc_, temp, 1);
  --size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  Node* temp = tail_;
  if (size() == 1) {
    head_ = tail_ = nullptr;
  } else {
    tail_ = tail_->prev;
    tail_->next = nullptr;
  }
  NodeAllocTraits::destroy(alloc_, temp);
  NodeAllocTraits::deallocate(alloc_, temp, 1);
  --size_;
}
