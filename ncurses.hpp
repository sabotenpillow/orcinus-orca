
#pragma once

#include <ncurses.h>
#include "nfq.hpp"

class Ncurses {
 private:
  static unsigned int listtop;
  static int max_y, max_x;
  int cursor_x, cursor_y;
 public:
  Ncurses();
  static void init();
  static void exit() { endwin(); }
  static void updatemaxyx() { getmaxyx(stdscr, max_y, max_x); };
  static void dec_listtop();
  static void inc_listtop(int pktnum);
  void set_curpos(int y, int x) { cursor_y=y; cursor_x=x; update_curpos(); };
  void update_curpos()          { move(cursor_y, cursor_x); };
  void cursor_down(int pktnum);
  void cursor_up();
  void print_curposline(Nfq *nfq);
  void printlist(Nfq *nfq);
  int  keyinput(int k, Nfq *nfq);
  static void listupdate(Nfq *nfq);
};
