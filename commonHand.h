#ifndef COMMON_HAND_H
#define COMMON_HAND_H

class commonHand {
private:
  vector<int> cards;
  vector<bool> used;
  int mTopCard;
  hand simhand;
  int size;

public:
  commonHand() : cards(vector<int> (7)), used(vector<bool> (52, false)), mTopCard(0), size(0) {}

  void push(int i, int v) {
    simhand.push(v);
    cards[i] = v;
  }

  void pop(int i) { simhand.pop(i); }

  void print() {
    for(int i = 0; i < size; i++) {
      int x = 3 + CARD_W * i;
      int y = 12;
      (card(cards[i])).printFull(x, y);
    }
  }

  int topCard() { return mTopCard++; }
  int eval7() { return simhand.eval7(cards); }

  void flipCard(vector<int>& deck, int num) {
    while(num--) {
      int i = deck[topCard()];
      cards[size] = i;
      simhand.push(i);
      used[i] = true;
      int x = 3 + CARD_W * size;
      int y = 12;
      (card(i)).printFull(x, y);
      size++;
    }
  }

  pair<double, double> simulate(int numSims, vector<int>& playerCards) {
    int wins = 0;
    int ties = 0;

    for(int i = 0; i < 2; i++) {
      simhand.push(playerCards[i]);
      used[playerCards[i]] = true;
    }

    for(int i = 0; i < numSims; i++) {
      hand h = simhand;

      cards[5] = playerCards[0];
      cards[6] = playerCards[1];

      for(int k = size; k < 5; k++) {
        int j;
        do { j = rand() % 52; } while(used[j]);
        cards[k] = j; h.push(j); used[j] = true;
      }

      int playerScore = h.eval7(cards);
      for(int i : playerCards) h.pop(i);

        for(int k = 0; k < 2; k++) {
          int j;
          do { j = rand() % 52; } while(used[j]);
          cards[k+5] = j; h.push(j); used[j] = true;
        }
        int oppScore = h.eval7(cards);
        if(playerScore > oppScore) wins++;
        else if(playerScore == oppScore) ties++;
        h.pop(cards[5]); h.pop(cards[6]);
        for(int k = size; k < 7; k++) used[cards[k]] = false;
    }

    for(int i : playerCards) {
      simhand.pop(i);
      used[i] = false;
    }

    return pair<double, double>((double) wins / numSims, (double) ties / numSims);;
  }
};
#endif
