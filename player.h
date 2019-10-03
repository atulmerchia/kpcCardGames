#ifndef PLAYER_H
#define PLAYER_H

#include "iohelpers.h"
#include "commonHand.h"

class player {
private:
  int money;
  int bet;
  vector<int> cards;
  static int toPlay;
  static int pot;
  static commonHand river;

  double getRoR() { return (double) bet / pot; }

public:
  player() {
    money = 2000;
    bet = 0;
    cards = vector<int>(2);
    river = commonHand();
  }

  void deal(vector<int>& deck) {
    bet = 0;
    for(int i = 0; i < 2; i++) cards[i] = deck[river.topCard()];
  }

  pair<double, double> getHs() { return river.simulate(100000, cards); }
  int getValue() {
    for(int i = 0; i < 2; i++) river.push(i+5, cards[i]);
    int val = river.eval7();
    for(int i : cards) river.pop(i);
    return val;
  }
  double getPotOdds() { return (double) bet/pot; }

  char takeTurn(player opponent, vector<pair<int, double>> rorAdjustments, int& carrier) {
    if(money == 0) { call(); return 'C'; }
    pair<double, double> p = river.simulate(100000, cards);

    double pRoR = opponent.getRoR();

    int index = min((int) (10*pRoR - 5), 15);
    if(rorAdjustments[index].first)   p.first = (p.first + 1 - rorAdjustments[index].second) /  2;
    double hs = p.first + p.second;

    double mRoR = hs * pot / min(money + bet, toPlay);
    int probVal = rand() % 100;
    if(mRoR < 0.8) {
      if(bet < toPlay && probVal < 75) return 'F';
      if(probVal < 95) { call(); return 'C'; }
    }
    if(mRoR < 1.0) {
      if(bet < toPlay && probVal < 25) return 'F';
      if(probVal < 75) {call(); return 'C';}
    }
    if(mRoR < 1.3) {
      if(probVal < 60) {call(); return 'C';}
    }
    if(probVal < 30) { call(); return 'C';}

    double c = 10 * hs / (rand() % 5 + 8);
    double add = (c * pot) / (bet - c);

    add = ceil(add / 10) * 10;
    carrier = min((int) add, money);

    if(carrier == 0) { call(); return 'C'; };
    raise(carrier); return 'R';
  }

  void collect() { money += pot; }
  void reset() { money += bet; }

  bool call(bool moneyRequired = false) {
    int toAdd = toPlay - bet;
    if(toAdd > money)
      if(moneyRequired) return false;
      else toAdd = money;

    bet += toAdd;
    pot += toAdd;
    money -= toAdd;
    return true;
  }

  boolean raise(int r) {
    if(!r || money < r) return false;
    money -= r;
    bet += r;
    pot += r;
    toPlay = bet;
    return true;
  }

  void allIn() {
    bet += money;
    pot += money;
    toPlay = bet;
    money = 0;
  }

  void printGameState() {
    rlutil::locate(1, 34);
    wcout
    << "  Your funds: " << money << "    \n"
    << "  Your bet:   " << bet << "    \n"
    << "  CPU funds:  " << 4000 - pot - money << "    \n"
    << "  Pot:        " << pot << "    \n";
  }

  void showCards(int y, bool refreshRiver = true) {
    for(int x = 0; x < 2; x++) {
      int tmp = y;
      (card(cards[x])).printFull(3 + CARD_W * (1.5 + x), tmp);
    }
    if(refreshRiver) river.print();
  }

  static void startRound() {
    pot = 0;
    river = commonHand();
  }

  static void riverCard(vector<int>&deck, int num) { river.flipCard(deck, num); }
};

#endif
