#ifndef IOHELPERS_H
#define IOHELPERS_H

#include <bits/stdc++.h>
#include <unistd.h>
#include "rlutil.h"

using namespace std;

#define _O_U16TEXT 0x10000
#define TABLEAU_START 9
#define TABLEAU_END 31
#define CARD_W 8
#define BLANK wstring(100, ' ')

static const string suitstring = "SHDC";

static void setup() {
  _setmode(STDOUT_FILENO, _O_U16TEXT);
  system(&"cls && mode 650"[0]);
  system(&"color A0"[0]);

  srand(time(0));
}

static char input() {
  string s;
  getline(cin, s);
  return toupper(s[0]);
}

static char prompt(bool err, wstring s = L"Your move: ", int level = 0) {
  rlutil::locate(3, TABLEAU_END + 8 + level);
  wcout << BLANK << L"\n" << BLANK << flush;
  rlutil::locate(3, TABLEAU_END + 8 + level);
  if(err) wcout << L"Invalid input. ";
  wcout << s;
  return input();
}

static char secondary(bool err, wstring s = L"Move to (x to cancel): ") {
  return prompt(err, s, 1);
}

static int getQuant(bool err, wstring s = L"Quantity (-1 to cancel): ") {
  rlutil::locate(3, TABLEAU_END + 9);
  wcout << BLANK << L"\n" << BLANK << flush;
  rlutil::locate(3, TABLEAU_END + 9);
  if(err) wcout << L"Invalid input. ";
  wcout << s;
  string input; getline(cin, input);
  stringstream ss(input); int x;
  ss >> x;
  return x;
}

static bool getIndex(char &c, int min) {
  if('0' + min <= c && c <= '7') {
    c -= '0';
    return true;
  }
  int idx = suitstring.find(c);
  if(idx != string::npos) {
    c = idx + 8;
    return true;
  }
  return false;
}

static bool getFile(fstream& file, ios_base::openmode mode) {
  rlutil::locate(3, TABLEAU_END + 8);
  wcout << BLANK << L"\n" << BLANK << flush;
  rlutil::locate(3, TABLEAU_END + 8);
  wcout << "Enter the name of the save file.";
  string path;
  wstring msg = L"Filepath (x to cancel): ";
  do {
    rlutil::locate(3, TABLEAU_END + 9);
    wcout << msg;
    getline(cin, path);
    msg = L"Invalid. Filepath (x to cancel): ";
    if(path == "X" || path == "x") return false;
    if(path.find('.') == string::npos)
      file.open(path + ".dat", mode);
  } while(!file.is_open());
  return true;
}

#endif
