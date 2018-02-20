
#pragma once

#include <ncurses.h>
#include "nfq.hpp"

class Ncurses {
 private:
  static unsigned int listtop;
  static int max_y, max_x;
 public:
  static void updateyx() { getmaxyx(stdscr, max_y, max_x); };
  static void listupdate(Nfq *nfq);
};
