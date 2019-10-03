#ifndef NODE_H
#define NODE_H

template<class T>
class node {
private:
  node<T> *mPrev, *mNext;
  T mData;
public:
  node(T d, node<T> *p = nullptr, node<T> *n = nullptr) : mData(d), mPrev(p), mNext(n) {}

  T data() { return mData; }
  void data(T d) { mData = d; }

  node<T>* prev() { return mPrev; }
  void prev(node<T>* p) { mPrev = p; }

  node<T>* next() { return mNext; }
  void next(node<T>* n) { mNext = n; }
};

#endif
