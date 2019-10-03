#ifndef SHALLOWSTACK_H
#define SHALLOWSTACK_H

#include "collection.h"

template<class T>
class shallowstack : public collection<T> {
private:
  T mTop;
public:
  shallowstack(T dummyTop) : mTop(dummyTop) {}
  int size() { return mTop.val() + 1; }
  void push(T data) { mTop++;}
  T pop() { T res = mTop; mTop--; return res; }
  T peek() { return mTop; }
  void print(int x, int y) { size() ? mTop.printFull(x, y) : T::printEmpty(x, y, mTop.suit()); }

  bool transferFrom(collection<T> &other, int& sizeMoved, bool& unpacked, bool force = false) {
    bool valid = force || T::validTransfer(other.peek(), peek());
    unpacked = other.size() == 1 && other.head()->prev()->prev() != nullptr;
    if(valid) push(other.pop());
    return valid;
  }

  void serialize(std::fstream& file) { file << (mTop.val() >= 0 ? mTop.numrep() : -1-mTop.suit()) << std::endl; }
  void deserialize(std::fstream& file) { int i; file >> i; mTop = (i >= 0 ? T(i) : T(-1-i, -1));}
};

#endif
