#include "iohelpers.h"
#include "card.h"
#include "hand.h"
#include "player.cpp"
#include <chrono>

using namespace std;

vector<int> deck;
vector<pair<int, double>> rorAdjustments;

classifyUserAction(player user, bool folded = false) {
  double conveyed = folded ? 0.5 : user.getPotOdds();
  pair <double, double> probs = user.getHs();
  double actual = probs.first + probs.second;

  if(conveyed < 0.5) conveyed = 0.5;
  else if(conveyed > 2) conveyed = 2;

  int index = round(conveyed * 10 - 5);
  pair<int, double> p = rorAdjustments[index];
  p.second = (p.first * p.second + actual) / (p.first + 1);
  p.first++;
  rorAdjustments[index] = p;
}

void draw() {
  for(int x = 0; x < 2; x++) card::printFullBack(3 + CARD_W * (1.5 + x), 5);
  for(int x = 0; x < 5; x++) card::printFullBack(3 + CARD_W * x, 12);
  for(int x = 0; x < 2; x++) card::printFullBack(3 + CARD_W * (1.5 + x), 19);

  rlutil::locate(1, TABLEAU_END - 4);
  wcout << L"  _________________________________  \n";
  wcout << L" | F = Fold   || Resized window?   | \n";
  wcout << L" | C = Call   || V to repair view  | \n";
  wcout << L" | R = Raise  ||                   | \n";
  wcout << L" | A = All In || Want help?        | \n";
  wcout << L" | Q = Quit   || Y to analyze hand | \n";
  wcout << L"  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾  \n";
}

int main() {
  setup();

  player user, cpu;

  vector<int> numToFlip = {0,3,1,1};
  rorAdjustments = vector<pair<int, double>> (16, pair<int, double>(0, 0));
  vector<pair<int, double>> tmpRor = rorAdjustments;
  bool err = false; int quant = 0;
  char s[256]; string str;
  pair<double, double> p;

  while(true) {
    draw();
    player::startRound();
    deck = card::shuffle<int>();
    user.deal(deck);
    cpu.deal(deck);

    user.printGameState();
    char c = prompt(false, L"Shall we play a game? (Y/N): ");
    if(c == 'N') {
      err = false;
      do { c = secondary(err, L"Are you sure? (Y/N): "); err = true; } while(c != 'N' && c != 'Y');
      if(c == 'N') continue;
      else return 0;
    }
    else if(c == 'V') {
      setup(); draw(); continue;
    }
    else if(c != 'Y') continue;

    user.showCards(19);
    user.raise(20);
    if(!cpu.call()) {
      prompt("Game over. You done robbed the CPU blind.");
      return 0;
    }
    user.printGameState();

    for(int i : numToFlip) {
      player::riverCard(deck, i);
      user.printGameState();
      start:
      c = prompt(false);
      switch(c) {
        case 'F':
          classifyUserAction(user, true);
          cpu.collect();
          goto end;
          break;
        case 'C':
          user.call();
          break;
        case 'R':
          err = false;
          do { quant = getQuant(err); err = true; } while(quant != -1 && !user.raise(quant));
          if(quant == -1) goto start;
          break;
        case 'A':
          user.allIn();
          break;
        case 'Q':
          err = false;
          do {
            c = secondary(err, L"Are you sure? (Y/N): ");
            err = true;
          } while(c != 'N' && c != 'Y');
          if(c == 'Y') return 0;
          if(c == 'N') goto start;
          break;
        case 'V':
          setup();
          draw();
          user.showCards(19);
          goto start;
        case 'Y':
          p = user.getHs();
          sprintf(s, "You have a %d%% chance of a win and a %d%% chance of a tie using this hand. Press enter to dismiss.", (int)round(100 * p.first), (int)round(100*p.second));
          str = string(s);
          prompt(false, wstring(str.begin(), str.end()));
          goto start;
          break;
        default:
          err = true;
          goto start;
          break;
      }
      classifyUserAction(user);
      int carrier;
      switch(cpu.takeTurn(user, tmpRor, carrier)) {
        case 'F': prompt(false, L"CPU folds. Press enter to dismiss"); user.collect(); goto end; break;
        case 'C': prompt(false, L"CPU calls. Press enter to dismiss"); break;
        case 'R':
          err = false;
          char s[256]; sprintf(s, "CPU raises by %d. Call or fold? (C/F): ", carrier);
          string str(s);
          wstring promptString = wstring(str.begin(), str.end());
          do { c = prompt(err, promptString); err = true; } while(c != 'C' && c != 'F');
          if (c == 'C') { user.call(); classifyUserAction(user); }
          else if (c == 'F') { classifyUserAction(user, true); cpu.collect(); goto end;}
      }
    }
    cpu.showCards(5, false);
    quant = user.getValue() - cpu.getValue();
    if(quant < 0) { prompt(false, L"CPU wins this hand. Press enter to dismiss"); cpu.collect(); }
    else if(quant > 0) { prompt(false, L"You win this hand. Press enter to dismiss"); user.collect(); }
    else { prompt(false, L"Tie game. Press enter to dismiss"); user.reset(); cpu.reset(); }
    tmpRor = rorAdjustments;

    end:;
  }

  return 0;
}
