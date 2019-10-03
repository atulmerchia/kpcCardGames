#ifndef HAND_H
#define HAND_H

#include "card.h"

using namespace std;

static char suits [4] = {'S', 'H', 'D', 'C'};
static char vals [13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
static string type [9] = {"HIGH", "PAIR", "2_PR", "TRIP", "STRT", "FLSH", "FULL", "FOUR", "STFL"};

class hand {
  private:
    vector<int> freq;
    int bitrep[4];
    int numSuits, numRanks, bitOr, bitXor, numOdds;



  protected:

  public:

    // 0 | HIGH              | Ranked by high   | default                                                      | res = bitOr
    // 1 | PAIR              | Ranked by 2      | occurs when 4 ranks                                          | pairs = bitOr ^ bitXor
    // 2 | TWO               | Ranked by 2's    | occurs when 3 ranks, 1 odd                                   | pairs = bitOr ^ bitXor
    // 3 | TRIP              | Ranked by 3      | occurs when 3 ranks, 3 odds                                  | trip = (a&b) | (a&c) | (b&c)
    // 4 | STRAIGHT          | Ranked by high   | occurs when bitOr = (bitOr & -bitOr) * 31 OR bitOr == 0x100F | res = bitOr
    // 5 | FLUSH             | Ranked by high   | occurs when 1 rank                                           | res = bitOr
    // 6 | FULL              | Ranked by 3, 2   | occurs when 2 ranks, !bitAnd                                 | threes = xor, twos = bitOr ^ xor
    // 7 | FOUR              | Ranked by 4      | occurs when 2 ranks, bitAnd                                  | four = bitOr ^ bitXor
    // 8 | STRAIGHT FLUSH    | Ranked by high   | occurs when straight, flush                                  | res = bitOr

    int evaluate() {
      bool straight = 31 * (bitOr & -bitOr) == bitOr || bitOr == 0x100F;
      int straightVal = bitOr == 0x100F ? 0xF : bitOr;

      if(straight && numSuits == 1) return (8 << 26) | straightVal;

      if(numRanks == 2) {
        int bitAnd = bitrep[0];
        for(int i = 1; i < 4; i++)  bitAnd &= bitrep[i];
        if(bitAnd) return (7 << 26) | bitOr;
        else return (6 << 26) | (bitXor << 13) | (bitOr ^ bitXor);
      }

      if(numSuits == 1) return (5 << 26) | bitOr;
      if(straight) return (4 << 26) | straightVal;
      if(numRanks == 3) {
        if(numOdds == 3) {
          int trip = (bitrep[0] & bitrep[1]) | (bitrep[0] & bitrep[2]) | (bitrep[1] & bitrep[2]);
          return (3 << 26) | (trip << 13) | (bitOr ^ trip);
        }
        else return (2 << 26) | ((bitOr ^ bitXor) << 13) | bitXor;
      }

      if(numRanks == 4) return (1 << 26) | ((bitOr ^ bitXor) << 13) | bitXor;
      return bitOr;
    }

    hand() : freq(vector<int>(13,0)) {
      fill(bitrep, bitrep + 4, 0);
      numSuits = 0;
      numRanks = 0;
      bitOr = 0;
      bitXor = 0;
      numOdds = 0;
    }

    void push(int c) {
      int suit = c / 13;
      int val = c % 13;
      int setbit = 1 << val;
      if(!freq[val]) {
        numRanks++;
        bitOr |= setbit;
      }
      if(!bitrep[suit]) numSuits++;
      freq[val]++;
      bitrep[suit] |= setbit;
      bitXor ^= setbit;
      numOdds += freq[val] & 1 ? 1 : -1;
    }

    void pop(int c) {
      int suit = c / 13;
      int val = c % 13;
      int setbit = 1 << val;
      freq[val]--;
      if(!freq[val]) {
        numRanks--;
        bitOr ^= setbit;
      }
      bitrep[suit] ^= setbit;
      bitXor ^= setbit;
      if(!bitrep[suit]) numSuits--;
      numOdds += freq[val] & 1 ? 1 : -1;
    }

    int eval7(vector<int> cards) {
      int best = 0;
      for(int a = 0; a < 7; a++) {
        pop(cards[a]);
        for(int b = a + 1; b < 7; b++) {
          pop(cards[b]);
          int val = evaluate();
          if(val > best) best = val;
          push(cards[b]);
        }
        push(cards[a]);
      }
      return best;
    }
};
#endif
