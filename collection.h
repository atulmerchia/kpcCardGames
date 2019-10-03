#ifndef COLLECTION_H
#define COLLECTION_H

#include "node.h"

template<class T> class collection{
public:
  virtual T pop() { return T(); }
  virtual T peek() { return T(); }
  virtual int size() { return -1; }
  virtual void prev() {}
  virtual void next() {}
  virtual void pack(T) {}
  virtual void push(T) {}
  virtual void repack() {}
  virtual void yank(node<T>*, int) {}
  virtual node<T>* head() { return nullptr; }
  virtual node<T>* tail() { return nullptr; }
  virtual void print(int, int) {}
  virtual void print(int, int, int) {}
  virtual void serialize(std::fstream&) {}
  virtual void deserialize(std::fstream&) {}
  virtual bool transferFrom(collection<T>&, int&, bool&, bool = false) { return false; }
};

#endif
