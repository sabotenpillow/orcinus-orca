#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <iomanip>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <ncurses.h>

#define QUEUE_ID 10
#define HexFormat(wd, fill) std::hex<<std::setw(wd)<<std::setfill(fill)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

static int x, y;
static int pktlist_i = 0;

static void printhex(const char *buf, int len) {
  int i;
  for (i = 0; i < len; i++) {
    cout <<HexFormat(2,'0')<< (buf[i] & 0xff) <<' ';
    if ((i & 0x0f) == 0x0f)
      cout<<endl;
  }
  cout<<std::dec;
}

int cb(struct nfq_q_handle *qh, struct nfgenmsg *msg, struct nfq_data *nfdata, void *data) {
  struct nfqnl_msg_packet_hdr *header;
  char *payload;
  int len;
  unsigned int datasize;
  std::string buf;
  std::stringstream disp_line;

  header = nfq_get_msg_packet_hdr(nfdata);
  len = nfq_get_payload(nfdata, (unsigned char **)&payload);
  struct iphdr *iph = (struct iphdr *)payload;
  disp_line << inet_ntoa(*(struct in_addr *)&iph->saddr) << " -> "
            << inet_ntoa(*(struct in_addr *)&iph->daddr);
  switch ( iph->protocol ) {
  case IPPROTO_TCP: {
      struct tcphdr *tcph = (struct tcphdr *)(payload + sizeof(struct iphdr));
      datasize = iph->tot_len - (iph->ihl<<10) - (tcph->doff<<10);
      buf = std::string(payload
              + (iph->ihl<<2) + (tcph->doff<<2)).substr(0, datasize);
      disp_line << " TCP ";
      disp_line << ntohs(tcph->source) << "->"
                << ntohs(tcph->dest) <<endl;
      disp_line << buf;
    } break;
  case IPPROTO_UDP: {
      disp_line << " UDP ";
    } break;
  case IPPROTO_ICMP: {
      disp_line << " ICMP ";
    } break;
  default: break;
  }
  mvprintw(pktlist_i, 0, disp_line.str().c_str());
  refresh();
  pktlist_i+=1;
  //printhex(payload, len);

  return nfq_set_verdict(qh, ntohl(header->packet_id), NF_ACCEPT, 0, NULL);
}

int main(void) {
  struct nfq_handle *h;
  struct nfq_q_handle *qh;
  int fd;
  int rv;
  char buf[0x10000];

  system(("iptables -t raw -A PREROUTING -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -i eth1").c_str());
  system(("iptables -t raw -A OUTPUT -j NFQUEUE --queue-num "
    + std::to_string(QUEUE_ID) + " -s 192.168.67.10").c_str());

  h = nfq_open();
  if (!h) {
    cerr << "error during nfq_open()" << endl;
    exit(1);}
  if ( nfq_unbind_pf(h, AF_INET) < 0 ) {
    cerr << "error during nfq_unbind_pf()" << endl;
    exit(1);}
  if ( nfq_bind_pf(h, AF_INET) < 0 ) {
    cerr << "error during nfq_bind_pf()" << endl;
    exit(1);}
  if ( !(qh = nfq_create_queue(h, QUEUE_ID, &cb, NULL)) ) {
    cerr << "error during nfq_create_queue()" << endl;
    exit(1);}
  if ( nfq_set_mode(qh, NFQNL_COPY_PACKET, sizeof(buf)) < 0 ) {
    cerr << "failed to set packet_copy mode" << endl;
    exit(1);}

  fd = nfq_fd(h);
  //cout << "file descriptor is " << fd << endl;
  //cout << "start nfq handle" << endl;
  //cout<<endl;

  initscr();
  getmaxyx(stdscr, y, x);
  while ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
    //cout << endl << " == getpacket ==" << endl;
    nfq_handle_packet(h, buf, rv);
  }

  endwin();
  if (nfq_unbind_pf(h, AF_INET) < 0) {
    cerr << "error during nfq_unbind_pf()" << endl;
    exit(1);}
  nfq_close(h);
  system("iptables -t raw -F");
}
