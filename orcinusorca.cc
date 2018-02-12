#include <iostream>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <iomanip>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstdint>
#include <linux/if_ether.h>
#include <stdio.h>

#define QUEUE_ID 10
#define HexFormat(wd, fill) std::hex<<std::setw(wd)<<std::setfill(fill)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

static void printhex(const char *buf, int len) {
  int i;
  for (i = 0; i < len; i++) {
    cout << HexFormat(2, '0') << (buf[i] & 0xff) << ' ';
    if ((i & 0x0f) == 0x0f)
      cout<<endl;
  }
}

int cb(struct nfq_q_handle *qh, struct nfgenmsg *msg, struct nfq_data *nfdata, void *data) {
  struct nfqnl_msg_packet_hdr *header;
  char *payload;
  int len;

  header = nfq_get_msg_packet_hdr(nfdata);
  len = nfq_get_payload(nfdata, (unsigned char **)&payload);
  printhex(payload, len);

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
  cout << "file descriptor is " << fd << endl;
  cout << "start nfq handle" << endl;
  cout << endl;
  while ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
    cout << endl << " == getpacket ==" << endl;
    nfq_handle_packet(h, buf, rv);
  }

  if (nfq_unbind_pf(h, AF_INET) < 0) {
    cerr << "error during nfq_unbind_pf()" << endl;
    exit(1);}
  nfq_close(h);
  system("iptables -t raw -F");
}
