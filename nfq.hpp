#include <iostream>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <sstream>
//#include <iomanip>
#include <ncurses.h>

class Nfq {
 private:
  unsigned int pktnum = 0;
  struct nfq_handle *h;
  struct nfq_q_handle *qh;
  struct listElem {
    struct nfq_data *nfdata;
    std::string about;
  };
  static int cb(struct nfq_q_handle *qh, struct nfgenmsg *msg,
                struct nfq_data *nfdata, void *data);
 public:
  void inc_pktnum() { pktnum++; }
  void dec_pktnum() { if (pktnum > 0) pktnum--; }
  int  get_pktnum() { return pktnum; }
  int  get_fd()     { return nfq_fd(h); }
  int  init(unsigned short int queue_id,
            unsigned int nfq_mode, unsigned int range);
  void handle(char *buf, int rv) { nfq_handle_packet(h, buf, rv); }
  int  exit();
};
