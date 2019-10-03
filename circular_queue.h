#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include "collection.h"

template<class T>
class circular_queue : public collection<T> {
private:
  node<T> *head;
  int mSize;
public:
  circular_queue() : head(nullptr), mSize(0) {}

  void push(T data) {
    if(head == nullptr) {
      head = new node<T>(data);
      head->next(head);
      head->prev(head);
    }
    else{
      node<T> *n = new node<T>(data, head->prev(), head);
      head->prev()->next(n);
      head->prev(n);
      head = head->prev();
    }
    mSize++;
  }

  T pop() {
    T data = head->data();
    node<T>* next = head->next();
    node<T>* prev = head->prev();

    delete head;
    if(next != head){
      prev->next(next);
      next->prev(prev);
      head = next;
    }
    else head = nullptr;
    mSize--;
    return data;
  }

  T peek() { return head->data(); }
  int size() { return mSize; }
  void next() { if(head != nullptr) head = head->next(); }
  void prev() { if(head != nullptr) head = head->prev(); }
  void print(int x, int y) { mSize ? head->data().printFull(x, y) : T::printEmpty(x, y); }
  void serialize(std::fstream& file) {
    node<T>* n = head;
    for(int i = 0; i < mSize; i++) {
      file << n->data().numrep() << " ";
      n = n->prev();
    }
    file << std::endl;
  }
  void deserialize(std::fstream& file) {
    std::string s; std::getline(file, s);
    std::stringstream ss(s);
    int i; while(ss >> i) push(T(i));
    prev();
  }
};

#endif
