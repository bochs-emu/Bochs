//
// virtual Ethernet locator
//
// An implementation of ARP, ping(ICMP-echo) and DHCP.
// Virtual host acts as a DHCP server for guest.
// There are no connections between the virtual host and real ethernets.
//
// Virtual host IP: 192.168.10.1
// Guest IP: 192.168.10.2
// Guest netmask: 255.255.255.0
// Guest broadcast: 192.168.10.255
//

#define BX_PLUGGABLE

#include "bochs.h"
#if BX_NE2K_SUPPORT

#define LOG_THIS bx_devices.pluginNE2kDevice->

#define BX_ETH_VNET_LOGGING 1

/////////////////////////////////////////////////////////////////////////
// handler to send/receive packets
/////////////////////////////////////////////////////////////////////////

static const Bit8u broadcast_macaddr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
static const Bit8u default_host_ipv4addr[4] = {192,168,10,1};
static const Bit8u subnetmask_ipv4addr[4] = {0xff,0xff,0xff,0x00};
static const Bit8u default_guest_ipv4addr[4] = {192,168,10,2};
static const Bit8u broadcast_ipv4addr[3][4] =
{
  {  0,  0,  0,  0},
  {255,255,255,255},
  {192,168, 10,255},
};

#define IPPACKET_MAX          2048
#define ICMP_ECHO_PACKET_MAX  128
#define LAYER4_LISTEN_MAX  128
typedef void (*layer4_handler_t)(
  void *this_ptr,
  const Bit8u *ipheader,
  unsigned ipheader_len,
  unsigned sourceport,
  unsigned targetport,
  const Bit8u *data,
  unsigned data_len
  );

#define INET_PORT_FTPDATA 20
#define INET_PORT_FTP 21
#define INET_PORT_TIME 37
#define INET_PORT_NAME 42
#define INET_PORT_DOMAIN 53
#define INET_PORT_BOOTP_SERVER 67
#define INET_PORT_BOOTP_CLIENT 68
#define INET_PORT_HTTP 80
#define INET_PORT_NTP 123

#define BOOTREQUEST 1
#define BOOTREPLY 2
#define BOOTPOPT_PADDING 0
#define BOOTPOPT_END 255
#define BOOTPOPT_SUBNETMASK 1
#define BOOTPOPT_TIMEOFFSET 2
#define BOOTPOPT_ROUTER_OPTION 3
#define BOOTPOPT_DOMAIN_NAMESERVER 6
#define BOOTPOPT_HOST_NAME 12
#define BOOTPOPT_DOMAIN_NAME 15
#define BOOTPOPT_MAX_DATAGRAM_SIZE 22
#define BOOTPOPT_DEFAULT_IP_TTL 23
#define BOOTPOPT_BROADCAST_ADDRESS 28
#define BOOTPOPT_ARPCACHE_TIMEOUT 35
#define BOOTPOPT_DEFAULT_TCP_TTL 37
#define BOOTPOPT_NTP_SERVER 42
#define BOOTPOPT_NETBIOS_NAMESERVER 44
#define BOOTPOPT_X_FONTSERVER 48
#define BOOTPOPT_REQUESTED_IP_ADDRESS 50
#define BOOTPOPT_IP_ADDRESS_LEASE_TIME 51
#define BOOTPOPT_OPTION_OVRLOAD 52
#define BOOTPOPT_DHCP_MESSAGETYPE 53
#define BOOTPOPT_SERVER_IDENTIFIER 54
#define BOOTPOPT_PARAMETER_REQUEST_LIST 55
#define BOOTPOPT_MAX_DHCP_MESSAGE_SIZE 57
#define BOOTPOPT_RENEWAL_TIME 58
#define BOOTPOPT_REBINDING_TIME 59
#define BOOTPOPT_CLASS_IDENTIFIER 60
#define BOOTPOPT_CLIENT_IDENTIFIER 61

#define DHCPDISCOVER 1
#define DHCPOFFER 2
#define DHCPREQUEST 3
#define DHCPDECLINE 4
#define DHCPACK 5
#define DHCPNAK 6
#define DHCPRELEASE 7

class bx_vnet_pktmover_c : public eth_pktmover_c {
public:
  bx_vnet_pktmover_c();
  void pktmover_init(
    const char *netif, const char *macaddr,
    eth_rx_handler_t rxh, void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
private:
  void guest_to_host(const Bit8u *buf, unsigned io_len);
  void host_to_guest(Bit8u *buf, unsigned io_len);
  void process_arp(const Bit8u *buf, unsigned io_len);
  void host_to_guest_arp(Bit8u *buf, unsigned io_len);
  void process_ipv4(const Bit8u *buf, unsigned io_len);
  void host_to_guest_ipv4(Bit8u *buf, unsigned io_len);

  layer4_handler_t get_layer4_handler(
    unsigned ipprotocol, unsigned port);
  bx_bool register_layer4_handler(
    unsigned ipprotocol, unsigned port,layer4_handler_t func);
  bx_bool unregister_layer4_handler(
    unsigned ipprotocol, unsigned port);

  void process_icmpipv4(
    const Bit8u *ipheader, unsigned ipheader_len,
    const Bit8u *l4pkt, unsigned l4pkt_len);
  void process_tcpipv4(
    const Bit8u *ipheader, unsigned ipheader_len,
    const Bit8u *l4pkt, unsigned l4pkt_len);
  void process_udpipv4(
    const Bit8u *ipheader, unsigned ipheader_len,
    const Bit8u *l4pkt, unsigned l4pkt_len);
  void host_to_guest_udpipv4_packet(
    unsigned target_port, unsigned source_port,
    const Bit8u *udpdata, unsigned udpdata_len);

  void process_icmpipv4_echo(
    const Bit8u *ipheader, unsigned ipheader_len,
    const Bit8u *l4pkt, unsigned l4pkt_len);

  static void udpipv4_dhcp_handler(
    void *this_ptr,
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len);
  void udpipv4_dhcp_handler_ns(
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len);

  Bit8u host_macaddr[6];
  Bit8u guest_macaddr[6];
  Bit8u host_ipv4addr[4];
  Bit8u guest_ipv4addr[4];

  struct {
    unsigned ipprotocol;
    unsigned port;
    layer4_handler_t func;
  } l4data[LAYER4_LISTEN_MAX];
  unsigned l4data_used;

#if BX_ETH_VNET_LOGGING
  FILE *pktlog_txt;
#endif // BX_ETH_VNET_LOGGING
};

class bx_vnet_locator_c : public eth_locator_c {
public:
  bx_vnet_locator_c(void) : eth_locator_c("vnet") {}
protected:
  eth_pktmover_c *allocate(
      const char *netif, const char *macaddr,
      eth_rx_handler_t rxh,
      void *rxarg) {
    bx_vnet_pktmover_c *pktmover;
    pktmover = new bx_vnet_pktmover_c();
    pktmover->pktmover_init(netif, macaddr, rxh, rxarg);
    return pktmover;
  }
} bx_vnet_match;

static void put_net2(Bit8u *buf,Bit16u data)
{
  *buf = (Bit8u)(data >> 8);
  *(buf+1) = (Bit8u)(data & 0xff);
}

static void put_net4(Bit8u *buf,Bit32u data)
{
  *buf = (Bit8u)((data >> 24) & 0xff);
  *(buf+1) = (Bit8u)((data >> 16) & 0xff);
  *(buf+2) = (Bit8u)((data >> 8) & 0xff);
  *(buf+3) = (Bit8u)(data & 0xff);
}

static Bit16u get_net2(const Bit8u *buf)
{
  return (((Bit16u)*buf) << 8) |
         ((Bit16u)*(buf+1));
}

static Bit32u get_net4(const Bit8u *buf)
{
  return (((Bit32u)*buf) << 24) |
         (((Bit32u)*(buf+1)) << 16) |
         (((Bit32u)*(buf+2)) << 8) |
         ((Bit32u)*(buf+3));
}

static Bit16u ip_checksum(const Bit8u *buf, unsigned buf_len)
{
  Bit32u sum = 0;
  unsigned n;

  for (n = 0; n < buf_len; n++) {
    if (n & 1) {
      sum += (Bit32u)(*buf++);
    } else {
      sum += (Bit32u)(*buf++) << 8;
    }
  }
  while (sum > 0xffff) {
    sum = (sum >> 16) + (sum & 0xffff);
  }

  return (Bit16u)sum;
}


bx_vnet_pktmover_c::bx_vnet_pktmover_c()
{
}

void
bx_vnet_pktmover_c::pktmover_init(
  const char *netif, const char *macaddr,
  eth_rx_handler_t rxh, void *rxarg)
{
  BX_INFO(("ne2k vnet driver"));
  this->rxh   = rxh;
  this->rxarg = rxarg;

  memcpy(&host_macaddr[0], macaddr, 6);
  memcpy(&guest_macaddr[0], macaddr, 6);
  host_macaddr[5] = (host_macaddr[5] & (~0x01)) ^ 0x02;

  memcpy(&host_ipv4addr[0], &default_host_ipv4addr[0], 6);
  memcpy(&guest_ipv4addr[0], &broadcast_ipv4addr[0][0], 6);

  l4data_used = 0;

  register_layer4_handler(0x11,INET_PORT_BOOTP_SERVER,udpipv4_dhcp_handler);

#if BX_ETH_VNET_LOGGING
  pktlog_txt = fopen ("ne2k-pktlog.txt", "wb");
  if (!pktlog_txt) BX_PANIC (("ne2k-pktlog.txt failed"));
  fprintf (pktlog_txt, "vnet packetmover readable log file\n");
  fprintf (pktlog_txt, "net IF = %s\n", netif);
  fprintf (pktlog_txt, "host MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (pktlog_txt, "%02x%s", 0xff & host_macaddr[i], i<5?":" : "\n");
  fprintf (pktlog_txt, "guest MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (pktlog_txt, "%02x%s", 0xff & guest_macaddr[i], i<5?":" : "\n");
  fprintf (pktlog_txt, "--\n");
  fflush (pktlog_txt);
#endif
}

void
bx_vnet_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  guest_to_host((const Bit8u *)buf,io_len);
}

void
bx_vnet_pktmover_c::guest_to_host(const Bit8u *buf, unsigned io_len)
{
#if BX_ETH_VNET_LOGGING
  fprintf (pktlog_txt, "a packet from guest to host, length %u\n", io_len);
  Bit8u *charbuf = (Bit8u *)buf;
  unsigned n;
  for (n=0; n<io_len; n++) {
    if (((n % 16) == 0) && n>0)
      fprintf (pktlog_txt, "\n");
    fprintf (pktlog_txt, "%02x ", (unsigned)charbuf[n]);
  }
  fprintf (pktlog_txt, "\n--\n");
  fflush (pktlog_txt);
#endif

  if ((io_len >= 14) &&
      (!memcmp(&buf[6],&this->guest_macaddr[0],6)) &&
      (!memcmp(&buf[0],&this->host_macaddr[0],6) ||
       !memcmp(&buf[0],&broadcast_macaddr[0],6))) {
    switch (get_net2(&buf[12])) {
    case 0x0800: // IPv4.
      process_ipv4(buf, io_len);
      break;
    case 0x0806: // ARP.
      process_arp(buf, io_len);
      break;
    default: // unknown packet type.
      break;
    }
  }
}

void
bx_vnet_pktmover_c::host_to_guest(Bit8u *buf, unsigned io_len)
{
  Bit8u localbuf[60];

  if (io_len < 14) {
    BX_PANIC(("host_to_guest: io_len < 14!"));
    return;
  }

  if (io_len < 60) {
    memcpy(&localbuf[0],&buf[0],io_len);
    memset(&localbuf[io_len],0,60-io_len);
    buf=localbuf;
    io_len=60;
  }

  this->rxh(this->rxarg, (void *)buf, io_len);
#if BX_ETH_VNET_LOGGING
  fprintf (pktlog_txt, "a packet from host to guest, length %u\n", io_len);
  Bit8u *charbuf = (Bit8u *)buf;
  unsigned n;
  for (n=0; n<io_len; n++) {
    if (((n % 16) == 0) && n>0)
      fprintf (pktlog_txt, "\n");
    fprintf (pktlog_txt, "%02x ", (unsigned)charbuf[n]);
  }
  fprintf (pktlog_txt, "\n--\n");
  fflush (pktlog_txt);
#endif
}

/////////////////////////////////////////////////////////////////////////
// ARP
/////////////////////////////////////////////////////////////////////////

void
bx_vnet_pktmover_c::process_arp(const Bit8u *buf, unsigned io_len)
{
  unsigned opcode;
  unsigned protocol;
  Bit8u replybuf[60];

  if (io_len < 22) return;
  if (io_len < (22+buf[18]*2+buf[19]*2)) return;
  // hardware:Ethernet
  if (buf[14] != 0x00 || buf[15] != 0x01 || buf[18] != 0x06) return;
  opcode = get_net2(&buf[20]);
  protocol = get_net2(&buf[16]);
  memset(&replybuf[0],0,60);

  // protocol
  switch (protocol) {
  case 0x0800: // IPv4
    if (buf[19] == 0x04) {
      switch (opcode) {
      case 0x0001: // ARP REQUEST
        if (!memcmp(&buf[22],&this->guest_macaddr[0],6)) {
          memcpy(&this->guest_ipv4addr[0],&buf[28],4);
          if (!memcmp(&buf[38],&this->host_ipv4addr[0],4)) {
            memcpy(&replybuf[14],&buf[14],6);
            replybuf[20]=0x00;
            replybuf[21]=0x02;
            memcpy(&replybuf[22],&this->host_macaddr[0],6);
            memcpy(&replybuf[28],&this->host_ipv4addr[0],4);
            memcpy(&replybuf[32],&this->guest_macaddr[0],6);
            memcpy(&replybuf[38],&this->guest_ipv4addr[0],4);

            host_to_guest_arp(replybuf,60);
          }
        }
        break;
      case 0x0002: // ARP REPLY
        BX_INFO(("unexpected ARP REPLY\n"));
        break;
      case 0x0003: // RARP REQUEST
        BX_ERROR(("RARP is not implemented\n"));
        break;
      case 0x0004: // RARP REPLY
        BX_INFO(("unexpected RARP REPLY\n"));
        break;
      default:
        BX_INFO(("arp: unknown ARP opcode %04x",opcode));
        break;
      }
    }
    else
    {
      BX_INFO(("arp: unknown address length %u",(unsigned)buf[19]));
    }
    break;
  default:
    BX_INFO(("arp: unknown protocol 0x%04x",protocol));
    break;
  }
}

void
bx_vnet_pktmover_c::host_to_guest_arp(Bit8u *buf, unsigned io_len)
{
  memcpy(&buf[0],&this->guest_macaddr[0],6);
  memcpy(&buf[6],&this->host_macaddr[0],6);
  buf[12]=0x08;
  buf[13]=0x06;
  host_to_guest(buf,io_len);
}

/////////////////////////////////////////////////////////////////////////
// IPv4
/////////////////////////////////////////////////////////////////////////

void
bx_vnet_pktmover_c::process_ipv4(const Bit8u *buf, unsigned io_len)
{
  unsigned total_len;
  unsigned packet_id;
  unsigned fragment_flags;
  unsigned fragment_offset;
  unsigned ipproto;
  unsigned l3header_len;
  const Bit8u *l4pkt;
  unsigned l4pkt_len;

  if (io_len < (14U+20U)) {
    BX_INFO(("ip packet - too small packet\n"));
    return;
  }
  if ((buf[14+0] & 0xf0) != 0x40) {
    BX_INFO(("ipv%u packet - not implemented\n",((unsigned)buf[14+0] >> 4)));
    return;
  }
  l3header_len = ((unsigned)(buf[14+0] & 0x0f) << 2);
  if (l3header_len != 20) {
    BX_ERROR(("ip: option header is not implemented"));
    return;
  }
  if (io_len < (14U+l3header_len)) return;
  if (ip_checksum(&buf[14],l3header_len) != (Bit16u)0xffff) {
    BX_INFO(("ip: invalid checksum"));
    return;
  }

  total_len = get_net2(&buf[14+2]);
  if (io_len > (14U+total_len)) return;

  if (memcmp(&buf[14+16],host_ipv4addr,4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[0],4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[1],4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[2],4))
  {
    BX_INFO(("target IP address %u.%u.%u.%u is unknown",
      (unsigned)buf[14+16],(unsigned)buf[14+17],
      (unsigned)buf[14+18],(unsigned)buf[14+19]));
    return;
  }

  packet_id = get_net2(&buf[14+4]);
  fragment_flags = (unsigned)buf[14+6] >> 5;
  fragment_offset = ((unsigned)get_net2(&buf[14+6]) & 0x1fff) << 3;
  ipproto = buf[14+9];

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    BX_INFO(("ignore fragmented packet!\n"));
    return;
  } else {
    l4pkt = &buf[14 + l3header_len];
    l4pkt_len = total_len - l3header_len;
  }

  switch (ipproto) {
  case 0x01: // ICMP
    process_icmpipv4(&buf[14],l3header_len,l4pkt,l4pkt_len);
    break;
  case 0x06: // TCP
    process_tcpipv4(&buf[14],l3header_len,l4pkt,l4pkt_len);
    break;
  case 0x11: // UDP
    process_udpipv4(&buf[14],l3header_len,l4pkt,l4pkt_len);
    break;
  default:
    BX_INFO(("unknown IP protocol %02x\n",ipproto));
    break;
  }
}

void
bx_vnet_pktmover_c::host_to_guest_ipv4(Bit8u *buf, unsigned io_len)
{
  unsigned l3header_len;

  memcpy(&buf[0],&this->guest_macaddr[0],6);
  memcpy(&buf[6],&this->host_macaddr[0],6);
  buf[12]=0x08;
  buf[13]=0x00;
  buf[14+0] = (buf[14+0] & 0x0f) | 0x40;
  l3header_len = ((unsigned)(buf[14+0] & 0x0f) << 2);
  memcpy(&buf[14+12],&this->host_ipv4addr[0],4);
  memcpy(&buf[14+16],&this->guest_ipv4addr[0],4);
  put_net2(&buf[14+10], 0);
  put_net2(&buf[14+10], ip_checksum(&buf[14],l3header_len) ^ (Bit16u)0xffff);

  host_to_guest(buf,io_len);
}

layer4_handler_t
bx_vnet_pktmover_c::get_layer4_handler(
  unsigned ipprotocol, unsigned port)
{
  unsigned n;

  for (n = 0; n < l4data_used; n++) {
    if (l4data[n].ipprotocol == ipprotocol && l4data[n].port == port)
      return l4data[n].func;
  }

  return (layer4_handler_t)NULL;
}

bx_bool
bx_vnet_pktmover_c::register_layer4_handler(
  unsigned ipprotocol, unsigned port,layer4_handler_t func)
{
  if (get_layer4_handler(ipprotocol,port) != (layer4_handler_t)NULL) {
    BX_INFO(("IP protocol 0x%02x port %u is already in use",
      ipprotocol,port));
    return false;
  }

  unsigned n;

  for (n = 0; n < l4data_used; n++) {
    if (l4data[n].func == (layer4_handler_t)NULL) {
      break;
    }
  }

  if (n == l4data_used) {
    if (n >= LAYER4_LISTEN_MAX) {
      BX_ERROR(("vnet: LAYER4_LISTEN_MAX is too small"));
      return false;
    }
    l4data_used++;
  }

  l4data[n].ipprotocol = ipprotocol;
  l4data[n].port = port;
  l4data[n].func = func;

  return true;
}

bx_bool
bx_vnet_pktmover_c::unregister_layer4_handler(
  unsigned ipprotocol, unsigned port)
{
  unsigned n;

  for (n = 0; n < l4data_used; n++) {
    if (l4data[n].ipprotocol == ipprotocol && l4data[n].port == port) {
      l4data[n].func = (layer4_handler_t)NULL;
      return true;
    }
  }

  BX_ERROR(("IP protocol 0x%02x port %u is not registered",
    ipprotocol,port));
  return false;
}

void
bx_vnet_pktmover_c::process_icmpipv4(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  unsigned icmptype;
  unsigned icmpcode;

  if (l4pkt_len < 8) return;
  icmptype = l4pkt[0];
  icmpcode = l4pkt[1];
  if (ip_checksum(l4pkt,l4pkt_len) != (Bit16u)0xffff) {
    BX_INFO(("icmp: invalid checksum"));
    return;
  }

  switch (icmptype) {
  case 0x08: // ECHO
    if (icmpcode == 0) {
      process_icmpipv4_echo(ipheader,ipheader_len,l4pkt,l4pkt_len);
    }
    break;
  default:
    BX_INFO(("unhandled icmp packet: type=%u code=%u",
      icmptype, icmpcode));
    break;
  }
}

void
bx_vnet_pktmover_c::process_tcpipv4(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  if (l4pkt_len < 20) return;

  BX_INFO(("tcp packet - not implemented"));
}

void
bx_vnet_pktmover_c::process_udpipv4(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  unsigned udp_targetport;
  unsigned udp_sourceport;
  unsigned udp_len;
  layer4_handler_t func;

  if (l4pkt_len < 8) return;
  udp_sourceport = get_net2(&l4pkt[0]); 
  udp_targetport = get_net2(&l4pkt[2]); 
  udp_len = get_net2(&l4pkt[4]); 

  func = get_layer4_handler(0x11,udp_targetport);
  if (func != (layer4_handler_t)NULL) {
    (*func)((void *)this,ipheader,ipheader_len,
      udp_sourceport,udp_targetport,&l4pkt[8],l4pkt_len-8);
  } else {
    BX_INFO(("udp - unhandled port %u",udp_targetport));
  }
}

void
bx_vnet_pktmover_c::host_to_guest_udpipv4_packet(
  unsigned target_port, unsigned source_port,
  const Bit8u *udpdata, unsigned udpdata_len)
{
  Bit8u ipbuf[IPPACKET_MAX];

  if ((udpdata_len + 42U) > IPPACKET_MAX) {
    BX_PANIC(("generated udp data is too long"));
    return;
  }

  // udp pseudo-header
  memcpy(&ipbuf[34U-12U],host_ipv4addr,4);
  memcpy(&ipbuf[34U-8U],guest_ipv4addr,4);
  ipbuf[34U-4U]=0;
  ipbuf[34U-3U]=0x11; // UDP
  put_net2(&ipbuf[34U-2U],8U+udpdata_len);
  // udp header
  put_net2(&ipbuf[34U+0],source_port);
  put_net2(&ipbuf[34U+2],target_port);
  put_net2(&ipbuf[34U+4],8U+udpdata_len);
  put_net2(&ipbuf[34U+6],0);
  put_net2(&ipbuf[34U+6], ip_checksum(&ipbuf[34U-12U],12U+8U+udpdata_len));
  // ip header
  memset(&ipbuf[14U],0,20U);
  ipbuf[14U+0] = 0x45;
  ipbuf[14U+1] = 0x00;
  put_net2(&ipbuf[14U+2],20U+8U+udpdata_len);
  put_net2(&ipbuf[14U+4],1);
  ipbuf[14U+6] = 0x00;
  ipbuf[14U+7] = 0x00;
  ipbuf[14U+8] = 0x07; // TTL
  ipbuf[14U+9] = 0x11; // UDP
  memcpy(&ipbuf[42U],udpdata,udpdata_len);

  host_to_guest_ipv4(ipbuf,udpdata_len + 42U);
}

/////////////////////////////////////////////////////////////////////////
// ICMP/IPv4
/////////////////////////////////////////////////////////////////////////

void
bx_vnet_pktmover_c::process_icmpipv4_echo(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  Bit8u replybuf[ICMP_ECHO_PACKET_MAX];

  if ((14U+ipheader_len+l4pkt_len) > ICMP_ECHO_PACKET_MAX) {
    BX_ERROR(("icmp echo: size of an echo packet is too long"));
    return;
  }

  memcpy(&replybuf[14],ipheader,ipheader_len);
  memcpy(&replybuf[14+ipheader_len],l4pkt,l4pkt_len);

  replybuf[14+ipheader_len+0] = 0x00; // echo reply
  put_net2(&replybuf[14+ipheader_len+2],0);
  put_net2(&replybuf[14+ipheader_len+2],
    ip_checksum(&replybuf[14+ipheader_len],l4pkt_len) ^ (Bit16u)0xffff);

  host_to_guest_ipv4(replybuf,14U+ipheader_len+l4pkt_len);
}

/////////////////////////////////////////////////////////////////////////
// DHCP/UDP/IPv4
/////////////////////////////////////////////////////////////////////////

void
bx_vnet_pktmover_c::udpipv4_dhcp_handler(
  void *this_ptr,
  const Bit8u *ipheader, unsigned ipheader_len,
  unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len)
{
  ((bx_vnet_pktmover_c *)this_ptr)->udpipv4_dhcp_handler_ns(
    ipheader,ipheader_len,sourceport,targetport,data,data_len);
}

void
bx_vnet_pktmover_c::udpipv4_dhcp_handler_ns(
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len)
{
  const Bit8u *opts;
  unsigned opts_len;
  unsigned extcode;
  unsigned extlen;
  const Bit8u *extdata;
  unsigned dhcpmsgtype = 0;
  bx_bool found_serverid = false;
  const Bit8u *dhcpreqparams = NULL;
  unsigned dhcpreqparams_len = 0;
  Bit8u dhcpreqparam_default[8];
  bx_bool dhcpreqparam_default_validflag = false;
  Bit8u *replyopts;
  Bit8u replybuf[576];

  if (data_len < (236U+64U)) return;
  if (data[0] != BOOTREQUEST) return;
  if (data[1] != 1 || data[2] != 6) return;
  if (memcmp(&data[28U],guest_macaddr,6)) return;
  if (data[236] != 0x63 || data[237] != 0x82 ||
      data[238] != 0x53 || data[239] != 0x63) return;

  opts = &data[240];
  opts_len = data_len - 240U;

  while (1) {
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return;
    }
    extcode = *opts++;
    opts_len--;

    if (extcode == BOOTPOPT_PADDING) continue;
    if (extcode == BOOTPOPT_END) break;
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return;
    }
    extlen = *opts++;
    opts_len--;
    if (opts_len < extlen) {
      BX_ERROR(("dhcp: invalid request"));
      return;
    }
    extdata = opts;
    opts += extlen;
    opts_len -= extlen;

    switch (extcode)
    {
    case BOOTPOPT_DHCP_MESSAGETYPE:
      if (extlen != 1)
        break;
      dhcpmsgtype = *extdata;
      break;
    case BOOTPOPT_PARAMETER_REQUEST_LIST:
      if (extlen < 1)
        break;
      dhcpreqparams = extdata;
      dhcpreqparams_len = extlen;
      break;
    case BOOTPOPT_SERVER_IDENTIFIER:
      if (extlen != 4)
        break;
      if (memcmp(extdata,host_ipv4addr,4)) {
        BX_INFO(("dhcp: request to another server"));
        return;
      }
      found_serverid = true;
      break;
    default:
      break;
    }
  }

  memset(&dhcpreqparam_default,0,sizeof(dhcpreqparam_default));
  memset(&replybuf[0],0,sizeof(replybuf));
  replybuf[0] = BOOTREPLY;
  replybuf[1] = 1;
  replybuf[2] = 6;
  memcpy(&replybuf[4],&data[4],4);
  memcpy(&replybuf[16],default_guest_ipv4addr,4);
  memcpy(&replybuf[28],&data[28],6);
  replybuf[236] = 0x63;
  replybuf[237] = 0x82;
  replybuf[238] = 0x53;
  replybuf[239] = 0x63;
  replyopts = &replybuf[240];
  opts_len = sizeof(replybuf)/sizeof(replybuf[0])-240;
  switch (dhcpmsgtype) {
  case DHCPDISCOVER:
    BX_INFO(("dhcp server: DHCPDISCOVER"));
    *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
    *replyopts ++ = 1;
    *replyopts ++ = DHCPOFFER;
    opts_len -= 3;
    dhcpreqparam_default[0] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
    dhcpreqparam_default[1] = BOOTPOPT_SERVER_IDENTIFIER;
    dhcpreqparam_default_validflag = true;
    break;
  case DHCPREQUEST:
    BX_INFO(("dhcp server: DHCPREQUEST"));
    // check ciaddr.
    if (found_serverid || (!memcmp(&data[12],default_guest_ipv4addr,4))) {
      *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
      *replyopts ++ = 1;
      *replyopts ++ = DHCPACK;
      opts_len -= 3;
      memcpy(guest_ipv4addr,default_guest_ipv4addr,4);
      dhcpreqparam_default[0] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
      if (!found_serverid) {
        dhcpreqparam_default[1] = BOOTPOPT_SERVER_IDENTIFIER;
      }
      dhcpreqparam_default_validflag = true;
    } else {
      *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
      *replyopts ++ = 1;
      *replyopts ++ = DHCPNAK;
      opts_len -= 3;
      if (!found_serverid) {
        dhcpreqparam_default[0] = BOOTPOPT_SERVER_IDENTIFIER;
        dhcpreqparam_default_validflag = true;
      }
    }
    break;
  default:
    BX_INFO(("dhcp server: unknown message type %u",dhcpmsgtype));
    return;
  }

  while (1) {
    while (dhcpreqparams_len-- > 0) {
      switch (*dhcpreqparams++) {
      case BOOTPOPT_SUBNETMASK:
        BX_INFO(("provide BOOTPOPT_SUBNETMASK"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SUBNETMASK;
        *replyopts ++ = 4;
        memcpy(replyopts,subnetmask_ipv4addr,4);
        replyopts += 4;
        break;
#if 0 // DNS is not implemented.
      case BOOTPOPT_DOMAIN_NAMESERVER:
        BX_INFO(("provide BOOTPOPT_DOMAIN_NAMESERVER"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_DOMAIN_NAMESERVER;
        *replyopts ++ = 4;
        memcpy(replyopts,host_ipv4addr,4);
        replyopts += 4;
        break;
#endif
      case BOOTPOPT_BROADCAST_ADDRESS:
        BX_INFO(("provide BOOTPOPT_BROADCAST_ADDRESS"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_BROADCAST_ADDRESS;
        *replyopts ++ = 4;
        memcpy(replyopts,broadcast_ipv4addr[2],4);
        replyopts += 4;
        break;
      case BOOTPOPT_IP_ADDRESS_LEASE_TIME:
        BX_INFO(("provide BOOTPOPT_IP_ADDRESS_LEASE_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 900);
        replyopts += 4;
        break;
      case BOOTPOPT_SERVER_IDENTIFIER:
        BX_INFO(("provide BOOTPOPT_SERVER_IDENTIFIER"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SERVER_IDENTIFIER;
        *replyopts ++ = 4;
        memcpy(replyopts,host_ipv4addr,4);
        replyopts += 4;
        break;
      case BOOTPOPT_RENEWAL_TIME:
        BX_INFO(("provide BOOTPOPT_RENEWAL_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_RENEWAL_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 600);
        replyopts += 4;
        break;
      case BOOTPOPT_REBINDING_TIME:
        BX_INFO(("provide BOOTPOPT_REBINDING_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_REBINDING_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 1800);
        replyopts += 4;
        break;
      default:
        break;
      }
    }

    if (!dhcpreqparam_default_validflag) break;
    dhcpreqparams = &dhcpreqparam_default[0];
    dhcpreqparams_len = sizeof(dhcpreqparam_default);
    dhcpreqparam_default_validflag = false;
  }

  if (opts_len < 1) {
    BX_ERROR(("option buffer is insufficient"));
    return;
  }
  opts_len -= 2;
  *replyopts ++ = BOOTPOPT_END;

  opts_len = replyopts - &replybuf[0];
  if (opts_len < (236U+64U)) {
    opts_len = (236U+64U); // BOOTP
  }
  if (opts_len < (548U)) {
    opts_len = 548U; // DHCP
  }
  host_to_guest_udpipv4_packet(
    sourceport, targetport,
    replybuf, opts_len);
}



#endif /* if BX_NE2K_SUPPORT */
