#include "ncurses.hpp"

unsigned int Ncurses::listtop = 0;
int Ncurses::max_y = -1;
int Ncurses::max_x = -1;

Ncurses::Ncurses()
{
  cursor_y = cursor_x = 0;
}

void Ncurses::init()
{
  initscr();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_RED);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_BLACK, COLOR_BLUE);
  init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(7, COLOR_BLACK, COLOR_CYAN);
  init_pair(8, COLOR_BLACK, COLOR_WHITE);
  keypad(stdscr, true);
  Ncurses::updatemaxyx();
  refresh();
  cbreak();
  noecho();
}

void Ncurses::dec_listtop()
{
  if ( listtop > 0 ) listtop--;
};
void Ncurses::inc_listtop(int pktnum)
{
  if ( pktnum > listtop + max_y ) listtop++;
};

void Ncurses::cursor_down(int pktnum)
{
  if ( cursor_y < max_y-1 ) {
    if ( pktnum <= max_y && cursor_y >= pktnum-1 ) return;
    cursor_y++;
  } else inc_listtop(pktnum);
  update_curpos();
};
void Ncurses::cursor_up()
{
  if ( cursor_y > 0 ) cursor_y--;
  else dec_listtop();
  update_curpos();
};

void Ncurses::print_curposline(Nfq *nfq)
{
  int i = listtop + cursor_y;
  attrset(COLOR_PAIR(4));
  mvprintw(cursor_y, 0, nfq->get_pktdata(i).about.c_str());
  attrset(COLOR_PAIR(0));
}

void Ncurses::printlist(Nfq *nfq)
{
  Ncurses::listupdate(nfq);
  print_curposline(nfq);
  update_curpos();
  refresh();
}

int Ncurses::keyinput(int k, Nfq *nfq) {
  switch(k) {
    case 'j': case '': case KEY_DOWN:
      cursor_down(nfq->get_pktnum());
      break;
    case 'k': case '': case KEY_UP:
      cursor_up();
      break;
    case 'Q': return -1;
  }
  printlist(nfq);
  return 0;
}

void Ncurses::listupdate(Nfq *nfq)
{
  int pktnum = nfq->get_pktnum();
  //if ( pktnum > max_y ) return;
  erase();
  for ( int i=listtop, y=0; y < max_y && y < pktnum; i++, y++ ) {
    mvprintw(y, 0, nfq->get_pktdata(i).about.c_str());
  }
}
