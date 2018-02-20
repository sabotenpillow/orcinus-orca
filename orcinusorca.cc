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
#include <boost/thread.hpp>
#include "nfq.hpp"
#include "ncurses.hpp"

#define QUEUE_ID 10
#define BUF_SIZE 0x10000
#define HexFormat(wd, fill) std::hex<<std::setw(wd)<<std::setfill(fill)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

static void printhex(const char *buf, int len);
int nfq_thread(Nfq *nfq);

int main(void) {
  Nfq *nfq = new Nfq;

  system(("iptables -t raw -A PREROUTING -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -i eth1").c_str());
  system(("iptables -t raw -A OUTPUT -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -s 192.168.67.10").c_str());

  if ( nfq->init(QUEUE_ID, NFQNL_COPY_PACKET, sizeof(char)*BUF_SIZE) < 0 )
    return -1;
  boost::thread nfqthread(nfq_thread, nfq);

  initscr();
  Ncurses::updateyx();
  refresh();

  nfqthread.join();

  endwin();

  //nfqthread.interrupt();
  if ( nfq->exit() < 0 )
    return -1;

  system("iptables -t raw -F");
}

static void printhex(const char *buf, int len) {
  int i;
  for (i = 0; i < len; i++) {
    cout <<HexFormat(2,'0')<< (buf[i] & 0xff) <<' ';
    if ((i & 0x0f) == 0x0f)
      cout<<endl;
  }
  cout<<std::dec;
}

int nfq_thread(Nfq *nfq) {
  int fd, rv;
  char buf[BUF_SIZE];

  fd = nfq->get_fd();
  while ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
    nfq->handle(buf, rv);
    Ncurses::listupdate(nfq);
  }
  return 0;
}
