#define ACELOW

#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "circular_queue.h"
#include "linked_list.h"
#include "shallowstack.h"
#include "node.h"
#include "card.h"
#include "iohelpers.h"

using namespace std;

#define WHT L"\u001b[107m"

collection<card> *decks [12];
stack<int> undo;

void deal(bool init = false){
  if(!init) {
    for(int i = 0; i < 12; i++)
      delete decks[i];
    while(undo.size())
      undo.pop();
  }
  decks[0] = new circular_queue<card>();
  for(int i = 1; i <= 7; i++) decks[i] = new linked_list<card>();
  for(int i = 8; i < 12; i++) decks[i] = new shallowstack<card>(card(i-8, -1));

  vector<card> deck = card::shuffle<card>();
  int idx = 0;
  for(int i = 0; i < 7; i++) {
    decks[i+1]->push(deck[idx++]);
    for(int j = i + 1; j < 7; j++)
      decks[j+1]->pack(deck[idx++]);
  }
  while(idx < 52)
    decks[0]->push(deck[idx++]);
}

void printCollection(int i) {
  if(i == 0) decks[0]->print(3, 2);
  else if(i < 8) decks[i]->print(3 + CARD_W * (i-1), TABLEAU_START, TABLEAU_END);
  else decks[i]->print(3 + CARD_W * (i-5), 2);
}

void draw() {
  system(&"cls"[0]);
  for(int i = 0; i < 12; i++)
    printCollection(i);

  rlutil::locate(2, TABLEAU_START - 1);
  wcout << GRN << L"    1       2       3       4       5       6       7\n\n";

  rlutil::locate(1, TABLEAU_END + 1);
  wcout << L"  _____________________________________________________ \n";
  wcout << L" | Use S, H, C, and D to refer to decks    || U = Undo |\n";
  wcout << L" | Enter column number (1-7) to move cards || Y = Hint |\n";
  wcout << L" | Enter 0 to remove card from stock pile  || O = Open |\n";
  wcout << L" | Enter L/R to rotate the stock pile      || N = New  |\n";
  wcout << L" | Resized window? Enter V to repair view  || Q = Quit |\n";
  wcout << L"  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ \n";
}

void reverseOp() {
  if(!undo.size()) return;
  int op = undo.top(); undo.pop();
  if(op == 'L'){
    decks[0]->next();
    printCollection(0);
  }
  else if (op == 'R'){
    decks[0]->prev();
    printCollection(0);
  }
  else {
    bool unpacked = op;
    int size = undo.top(); undo.pop();
    int to = undo.top(); undo.pop();
    int from = undo.top(); undo.pop();

    if(unpacked) decks[from]->repack();
    if(size == 1) decks[from]->push(decks[to]->pop());
    else decks[from]->transferFrom(*decks[to], size, unpacked, true);

    printCollection(to);
    printCollection(from);
  }
}

void printHints() {
  rlutil::locate(3, TABLEAU_END + 8);
  wcout << "Possible moves: ";
  if(decks[0]->size()) {
    wcout << "L/R";
    card c = decks[0]->peek();
    for(int i = 0; i < 12; i++)
      if(card::validTransfer(c, c, decks[i]->peek()) != -1)
        wcout << " (" << 0 << " " << wstring(1, i < 8 ? i + '0' : suitstring[i-8]) << ")";
  }
  for(int i = 1; i <= 7; i++) {
    if(decks[i]->size()) {
      for(int j = 1; j < 7; j++)
        if(i != j && card::validTransfer(decks[i]->head()->data(), decks[i]->peek(), decks[j]->peek()) != -1)
          wcout << " (" << i << " " << j << ")";
      for(int j = 8; j < 12; j++)
        if(card::validTransfer(decks[i]->peek(), decks[j]->peek()))
          wcout << " (" << i << " " << wstring(1, suitstring[j-8]) << ")";
    }
  }
  for(int i = 8; i < 12; i++) {
    card c = decks[i]->peek();
    for(int j = 1; j <= 7; j++)
      if(card::validTransfer(c, c, decks[i]->peek()) != -1)
        wcout << " (" << wstring(1, suitstring[i-8]) << " " << j << ")";
  }
  wcout << ". Press enter to dismiss.";
  secondary(false, L"");
}

void readInGameState() {
  for(int i = 0; i < 12; i++) delete decks[i];
  while(undo.size()) undo.pop();

  decks[0] = new circular_queue<card>();
  for(int i = 1; i <= 7; i++) decks[i] = new linked_list<card>();
  for(int i = 8; i < 12; i++) decks[i] = new shallowstack<card>(card(i-8, -1));

  fstream file; if(!getFile(file, ios_base::in)) return;

  for(int i = 0; i < 12; i++)
    decks[i]->deserialize(file);
  stack<int> tmp; int i;
  while(file >> i) tmp.push(i);
  while(tmp.size()){
    undo.push(tmp.top());
    tmp.pop();
  }

  file.close();
  draw();
}

void writeGameState() {
  fstream file; if(!getFile(file, ios_base::out)) return;
  for(int i = 0; i < 12; i++)
    decks[i]->serialize(file);
  while(undo.size()) {
    file << undo.top() << " ";
    undo.pop();
  }
  file.close();
}

int main() {
  setup();
  deal(true);
  draw();

  bool err = false;
  char choice;
  char moveTo;

  while(true) {
    choice = prompt(err);
    err = false;

    if(getIndex(choice, 0)) {
      if(decks[choice]->size() == 0) {
        err = true;
        continue;
      }
      int size;
      bool unpacked;
      do {
        moveTo = secondary(err);
        if(moveTo == 'X') break;
        size = 1; unpacked = false;
        err = !getIndex(moveTo, 1) || !(decks[moveTo]->transferFrom(*decks[choice], size, unpacked));
      } while(err);
      err = false;

      if(moveTo != 'X'){
        printCollection(choice);
        printCollection(moveTo);
        undo.push(choice);
        undo.push(moveTo);
        undo.push(size);
        undo.push(unpacked);
      }
    }

    else switch(choice) {
      case 'L':
        decks[0]->prev();
        printCollection(0);
        undo.push('L');
        break;
      case 'R':
        decks[0]->next();
        printCollection(0);
        undo.push('R');
        break;
      case 'U':
        reverseOp();
        break;
      case 'Y':
        printHints();
        break;
      case 'O':
        readInGameState();
        break;
      case 'N':
        if(prompt(false, L"Are you sure? y/n: ") == 'Y'){
          deal();
          draw();
        }
        break;
      case 'V':
        draw();
        break;
      case 'Q':
        do {
          choice = secondary(false, L"Save progress? y/n (x to cancel): ");
        } while (choice != 'Y' && choice != 'N' && choice != 'X');
        switch(choice) {
          case 'Y':
            writeGameState();
          case 'N':
            goto shutdown;
        }
        break;
      default:
        err = true;
    }
  }
  shutdown:
  for(int i = 0; i < 12; i++)
    delete decks[i];

  return 0;
}
