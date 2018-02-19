#include <iostream>
//#include <sstream>
//#include <netinet/in.h>
//#include <linux/netfilter.h>
//#include <libnetfilter_queue/libnetfilter_queue.h>
//#include <netinet/ip.h>
//#include <netinet/tcp.h>
//#include <netinet/udp.h>
//#include <netinet/ip_icmp.h>
//#include <arpa/inet.h>
//#include <linux/if_ether.h>
#include <iomanip>
#include <ncurses.h>
#include "nfq.hpp"

#define QUEUE_ID 10
#define HexFormat(wd, fill) std::hex<<std::setw(wd)<<std::setfill(fill)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

// class Ncurses {
//  private:
//   const unsigned int max_y;
//   const unsigned int max_x;
//   unsigned int listtop_index;
//  public:
//   static void operater() {
//   }
// };

static void printhex(const char *buf, int len) {
  int i;
  for (i = 0; i < len; i++) {
    cout <<HexFormat(2,'0')<< (buf[i] & 0xff) <<' ';
    if ((i & 0x0f) == 0x0f)
      cout<<endl;
  }
  cout<<std::dec;
}

int main(void) {
  Nfq *nfq = new Nfq;
  int fd;
  int rv;
  char buf[0x10000];
  int x, y;

  system(("iptables -t raw -A PREROUTING -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -i eth1").c_str());
  system(("iptables -t raw -A OUTPUT -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -s 192.168.67.10").c_str());

  if ( nfq->init(QUEUE_ID, NFQNL_COPY_PACKET, sizeof(buf)) < 0 )
    return -1;
  fd = nfq->get_fd();

  initscr();
  getmaxyx(stdscr, y, x);
  while ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
    nfq->handle(buf, rv);
  }

  endwin();
  if ( nfq->exit() < 0 )
    return -1;

  system("iptables -t raw -F");
}
