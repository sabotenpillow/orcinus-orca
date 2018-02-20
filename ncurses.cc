#include "ncurses.hpp"

unsigned int Ncurses::listtop = 0;
int Ncurses::max_y = -1;
int Ncurses::max_x = -1;

void Ncurses::listupdate(Nfq *nfq)
{
  int pktnum = nfq->get_pktnum();
  if ( pktnum > max_y ) return;
  for ( int i=listtop, y=0; y < max_y && y < pktnum; i++, y++ ) {
    mvprintw(y, 0, nfq->get_pktdata(i).about.c_str());
  }
  refresh();
}
