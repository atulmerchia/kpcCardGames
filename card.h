#ifndef CARD_H
#define CARD_H

#include "rlutil.h"

using namespace std;

#define RED L"\u001b[107;91m"
#define BLK L"\u001b[107;30m"
#define GRN L"\u001b[102;30m"

static const wstring SUIT [4] = { L"♠", L"♥", L"♦",  L"♣"};
#ifdef ACELOW
  static const wstring VAL [13] = {L"A", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10", L"J", L"Q", L"K" };
#else
  static const wstring VAL [13] = {L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10", L"J", L"Q", L"K", L"A" };
#endif

static const wstring ROW = L"       ";

static void print(int x, int y, wstring s) {
  rlutil::locate(x, y);
  wcout << s;
}

class card {
private:
  int mSuit, mVal;
  wstring padding;
  setPadding() { padding = VAL[mVal] == L"10" ? L"" : L" "; }

public:
  card (int suit, int val) : mSuit(suit), mVal(val) { setPadding(); }
  card (int numrep = 0) : mSuit(numrep/13), mVal(numrep%13) { setPadding(); }

  int numrep() { return 13 * mSuit + mVal; }
  int suit() { return mSuit; };
  int val() { return mVal; };

  wstring wrap(wstring w) { return (mSuit % 3 ? RED : BLK) + w; }

  card& operator++(int _) { mVal++; if(mVal != -1) setPadding(); }
  card& operator--(int _) { mVal--; if(mVal != -1) setPadding(); }

  void printStub(int x, int &y) {
    print(x, y++, wrap(L"\u001b[107;4m" + SUIT[mSuit] + VAL[mVal] + padding + L"    \u001b[107;24m"));
  }

  void printFull(int x, int &y) {
    print(x, y++, wrap(SUIT[mSuit] + VAL[mVal]) + padding + L"    ");
    print(x, y++, ROW);
    print(x, y++, wrap(L"  " + padding + VAL[mVal] + L"   "));
    print(x, y++, ROW);
    print(x, y++, wrap(L"    " + padding + VAL[mVal] + SUIT[mSuit]) + GRN);
  }

  int kColor() { return (!!(mSuit % 3) + mVal) % 2; }

  static void printEmpty(int x, int &y, int mSuit = -1) {
    wstring icon = mSuit == -1 ? L" " : SUIT[mSuit];
    print(x, y++, L"\u001b[42m" + ROW);
    print(x, y++, L"\u001b[42m" + ROW);
    print(x, y++, L"\u001b[97;42m   " + icon + L"   ");
    print(x, y++, L"\u001b[42m" + ROW);
    print(x, y++, L"\u001b[42m" + ROW + GRN);
  }

  static void printBack(int x, int &y) {
    print(x, y++, L"\u001b[107;94;4m░░░░░░░\u001b[30;24m");
  }

  static void printFullBack(int x, int y) {
    for(int i = 0; i < 5; i++)
      print(x, y++, L"\u001b[107;94m░░░░░░░\u001b[102;30m" );
  }

  static int validTransfer(card top, card bot, card tail) {
    if(tail.suit() == -1) return top.val() == 12 ? 0 : -1;
    if(top.kColor() != tail.kColor()) return -1;
    if(tail.val() - top.val() > 1 || tail.val() <= bot.val()) return -1;
    return top.val() - tail.val() + 1;
  }

  static bool validTransfer(card top, card bot) {
    return top.suit() == bot.suit() && top.val() == bot.val() + 1;
  }

  template <class T>
  static vector<T> shuffle() {
    vector<T> deck(52);
    for(int i = 0; i < 52; i++)
      deck[i] = T(i);
    for(int i = 0; i < 1000; i++)
      swap(deck[rand() % 52], deck[rand()%52]);
    return deck;
  }
};

#endif
