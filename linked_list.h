#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "collection.h"
#include "rlutil.h"

template <class T>
class linked_list : public collection<T> {
private:
  node<T> *root;
  node<T> *mTail;
  int mSize;
  int mPack;

  void unpack() {
    if(mSize == 0 && mPack != 0) {
      node<T>* n = root->prev();
      root->prev(n->prev());
      n->prev(root);
      n->next(nullptr);
      root->next(n);
      mPack--;
      mSize++;
      mTail = n;
    }
  }

public:
  linked_list() : mSize(0), mPack(0) {
    root = new node<T>(T(-1, 0));
    mTail = root;
  }

  void push(T data) {
    mTail->next(new node<T>(data, mTail));
    mTail = mTail->next();
    mSize++;
  }

  void pack(T data) {
    root->prev(new node<T>(data, root->prev()));
    mPack++;
  }

  void repack() {
    mSize--;
    mPack++;
    node<T>* toPack = root->next();
    root->next(toPack->next());
    toPack->prev(root->prev());
    root->prev(toPack);
    toPack->next(nullptr);
    if(mTail == toPack) mTail = root;
  }

  T peek() { return mTail->data(); }
  node<T>* head() { return root->next(); }
  node<T>* tail() { return mTail; }
  int size() { return mSize; }
  int pack() { return mPack; }

  bool transferFrom (collection<T> &other, int& sizeMoved, bool& unpacked, bool force) {
    int transferSize = sizeMoved;
    if(!force) transferSize = other.head() == nullptr
    ? T::validTransfer(other.peek(), other.peek(), mTail->data())
    : T::validTransfer(other.head()->data(), other.tail()->data(), mTail->data());


    if(transferSize == -1) return false;
    else if(other.head() == nullptr) {
      push(other.pop());
      return true;
    }
    else if(!force) transferSize = other.size() - transferSize;

    sizeMoved = transferSize;
    unpacked = other.size() == transferSize && other.head()->prev()->prev() != nullptr;

    node<T>* n = other.head()->prev();

    if(transferSize < other.size()/2) {
      n = other.tail();
      for(int i = 0; i < transferSize; i++)
        n = n->prev();
    }
    else
      for(int i = other.size(); i > transferSize; i--)
        n = n->next();

    mTail->next(n->next());
    n->next()->prev(mTail);
    mTail = other.tail();
    mSize += transferSize;
    other.yank(n, transferSize);
    return true;
  }

  void yank(node<T>* newTail, int removed) {
    mTail = newTail;
    mSize -= removed;
    mTail->next(nullptr);
    unpack();
  }

  T pop() {
    mSize--;
    T data = mTail->data();
    mTail = mTail->prev();
    delete mTail->next();
    mTail->next(nullptr);
    unpack();
    return data;
  }

  void print(int x, int y, int end) {
    if(mSize == 0){
      T::printEmpty(x, y);
    }
    else {
      for(int i = 0; i < mPack; i++)
        T::printBack(x, y);
        node<T>*n = head();

      for(int i = 1; i < mSize; i++) {
        n->data().printStub(x, y);
        n = n->next();
      }
      n->data().printFull(x, y);
    }

    while(y < end) {
      rlutil::locate(x, y++);
      std::wcout << std::setw(8) << L" ";
    }
  }

  void serialize(std::fstream& file) {
    for(node<T>* n = root->prev(); n != nullptr; n = n->prev())
      file << n->data().numrep() << " ";
    file << std::endl;
    for(node<T>* n = root->next(); n != nullptr; n = n->next())
      file << n->data().numrep() << " ";
    file << std::endl;
  }

  void deserialize(std::fstream& file) {
    std::string s; std::getline(file, s);
    std::stringstream ss(s);
    std::stack<int> buff; int i;
    while(ss >> i) buff.push(i);
    while(buff.size()) { pack(T(buff.top())); buff.pop(); }

    std::getline(file, s);
    ss = std::stringstream(s);
    while(ss >> i) push(i);
  }
};

#endif
