#pragma once

#include <cstddef>

class RingBuffer {
 public:
  struct Node {
    int val;
    Node* next;

    explicit Node(int i) {
      val = i;
      next = nullptr;
    }
  };

  size_t curr_size;
  size_t size;
  Node* first;
  Node* end;

  explicit RingBuffer(size_t capacity) {
    size = capacity;
    curr_size = 0;
    first = nullptr;
    end = nullptr;
  }

  size_t Size() const { return curr_size; }

  bool Empty() const { return curr_size == 0; }

  bool TryPush(int element) {
    if (curr_size == size) {
      return false;
    }
    if (first == nullptr) {
      first = new Node(element);
      end = first;
      curr_size++;
    } else {
      end->next = new Node(element);
      end = end->next;
      curr_size++;
    }
    return true;
  }

  bool TryPop(int* element) {
    if (Empty()) {
      return false;
    }
    Node* temp = first;
    first = first->next;
    *element = temp->val;
    delete temp;
    this->curr_size--;
    return true;
  }

  ~RingBuffer() {
    for (size_t i = 0; i < curr_size - 1; i++) {
      Node* temp = first;
      first = first->next;
      delete temp;
    }
    delete first;
  }
};
