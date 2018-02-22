#include "nfq.hpp"

using std::cerr;
using std::endl;

int Nfq::cb(struct nfq_q_handle *qh, struct nfgenmsg *msg,
            struct nfq_data *nfdata, void *nfq_instance)
{
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
                << ntohs(tcph->dest);
      //disp_line << buf;
    } break;
  case IPPROTO_UDP: {
      disp_line << " UDP ";
    } break;
  case IPPROTO_ICMP: {
      disp_line << " ICMP ";
    } break;
  default: break;
  }
  //printhex(payload, len);
  ((Nfq *)nfq_instance)->inspkt(nfdata, disp_line.str());
  ((Nfq *)nfq_instance)->inc_pktnum();

  return nfq_set_verdict(qh, ntohl(header->packet_id), NF_ACCEPT, 0, NULL);
}

int Nfq::init(unsigned short int queue_id,
              unsigned int nfq_mode, unsigned int range)
{
  h = nfq_open();
  if (!h) {
    cerr << "error during nfq_open()" << endl;
    return -1;}
  if ( nfq_unbind_pf(h, AF_INET) < 0 ) {
    cerr << "error during nfq_unbind_pf()" << endl;
    return -1;}
  if ( nfq_bind_pf(h, AF_INET) < 0 ) {
    cerr << "error during nfq_bind_pf()" << endl;
    return -1;}
  if ( !(qh = nfq_create_queue(h, queue_id, &cb, this)) ) {
    cerr << "error during nfq_create_queue()" << endl;
    return -1;}
  if ( nfq_set_mode(qh, nfq_mode, range) < 0 ) {
    cerr << "failed to set packet_copy mode" << endl;
    return -1;}
  return 0;
}

int Nfq::exit()
{
  if (nfq_unbind_pf(h, AF_INET) < 0) {
    cerr << "error during nfq_unbind_pf()" << endl;
    return -1;}
  nfq_close(h);
  return 0;
}

void Nfq::inspkt(struct nfq_data *nfd, std::string about) {
  struct listElem e = {nfd, about};
  this->pktlist.push_back(e);
}
