/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2005-2014  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

// virtual Ethernet locator
//
// An implementation of ARP, ping(ICMP-echo), DHCP and read/write TFTP.
// Virtual host acts as a DHCP server for guest.
// There are no connections between the virtual host and real ethernets.
//
// Virtual host name: vnet
// Virtual host IP: 192.168.10.1
// Guest IP: 192.168.10.2
// Guest netmask: 255.255.255.0
// Guest broadcast: 192.168.10.255
// TFTP server uses ethdev value for the root directory and doesn't overwrite files

#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING

#define LOG_THIS netdev->

#define BX_ETH_VNET_LOGGING 1
#define BX_ETH_VNET_PCAP_LOGGING 0

#if !defined(WIN32) || defined(__CYGWIN__)
#include <arpa/inet.h> /* ntohs, htons */
#else
#include <winsock2.h>
#endif

#if BX_ETH_VNET_PCAP_LOGGING
#include <pcap.h>
#endif

static unsigned int bx_vnet_instances = 0;

/////////////////////////////////////////////////////////////////////////
// handler to send/receive packets
/////////////////////////////////////////////////////////////////////////

static const Bit8u default_host_ipv4addr[4] = {192,168,10,1};
static const Bit8u subnetmask_ipv4addr[4] = {0xff,0xff,0xff,0x00};
static const Bit8u default_guest_ipv4addr[4] = {192,168,10,2};
static const Bit8u broadcast_ipv4addr[3][4] =
{
  {  0,  0,  0,  0},
  {255,255,255,255},
  {192,168, 10,255},
};

#define ICMP_ECHO_PACKET_MAX  128
#define LAYER4_LISTEN_MAX  128

static Bit8u    packet_buffer[BX_PACKET_BUFSIZE];
static unsigned packet_len;

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

// TFTP server support by EaseWay <easeway@123.com>

#define INET_PORT_TFTP_SERVER 69

class bx_vnet_pktmover_c : public eth_pktmover_c {
public:
  bx_vnet_pktmover_c();
  virtual ~bx_vnet_pktmover_c();
  void pktmover_init(
    const char *netif, const char *macaddr,
    eth_rx_handler_t rxh, eth_rx_status_t rxstat,
    bx_devmodel_c *dev, const char *script);
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
  static void udpipv4_tftp_handler(
    void *this_ptr,
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len);
  void udpipv4_tftp_handler_ns(
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len);

  dhcp_cfg_t dhcp;

  char tftp_rootdir[BX_PATHNAME_LEN];

  struct {
    unsigned ipprotocol;
    unsigned port;
    layer4_handler_t func;
  } l4data[LAYER4_LISTEN_MAX];
  unsigned l4data_used;

  static void rx_timer_handler(void *);
  void rx_timer(void);

  int rx_timer_index;
  unsigned netdev_speed;
  unsigned tx_time;

#if BX_ETH_VNET_LOGGING
  FILE *pktlog_txt;
#endif // BX_ETH_VNET_LOGGING
#if BX_ETH_VNET_PCAP_LOGGING
  pcap_t *pcapp;
  pcap_dumper_t *pktlog_pcap;
  struct pcap_pkthdr pcaphdr;
#endif // BX_ETH_VNET_PCAP_LOGGING
};

class bx_vnet_locator_c : public eth_locator_c {
public:
  bx_vnet_locator_c(void) : eth_locator_c("vnet") {}
protected:
  eth_pktmover_c *allocate(
      const char *netif, const char *macaddr,
      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
      bx_devmodel_c *dev, const char *script) {
    bx_vnet_pktmover_c *pktmover;
    pktmover = new bx_vnet_pktmover_c();
    pktmover->pktmover_init(netif, macaddr, rxh, rxstat, dev, script);
    return pktmover;
  }
} bx_vnet_match;


bx_vnet_pktmover_c::bx_vnet_pktmover_c()
{
}

void bx_vnet_pktmover_c::pktmover_init(
  const char *netif, const char *macaddr,
  eth_rx_handler_t rxh, eth_rx_status_t rxstat,
  bx_devmodel_c *dev, const char *script)
{
  if (bx_vnet_instances > 0) {
    BX_PANIC(("only one 'vnet' instance supported yet"));
  }
  this->netdev = dev;
  this->rxh    = rxh;
  this->rxstat = rxstat;
  strcpy(this->tftp_rootdir, netif);

  memcpy(&dhcp.host_macaddr[0], macaddr, 6);
  memcpy(&dhcp.guest_macaddr[0], macaddr, 6);
  dhcp.host_macaddr[5] ^= 0x03;

  memcpy(&dhcp.host_ipv4addr[0], &default_host_ipv4addr[0], 4);
  memcpy(&dhcp.guest_ipv4addr[0], &broadcast_ipv4addr[1][0], 4);
  dhcp.default_guest_ipv4addr = default_guest_ipv4addr;
  memcpy(&dhcp.dns_ipv4addr[0], &broadcast_ipv4addr[0][0], 4);

  l4data_used = 0;

  register_layer4_handler(0x11,INET_PORT_BOOTP_SERVER,udpipv4_dhcp_handler);
  register_layer4_handler(0x11,INET_PORT_TFTP_SERVER,udpipv4_tftp_handler);

  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  this->rx_timer_index =
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
                              	 0, 0, "eth_vnet");

  BX_INFO(("'vnet' network driver initialized"));
  bx_vnet_instances++;

#if BX_ETH_VNET_LOGGING
  pktlog_txt = fopen("ne2k-pktlog.txt", "wb");
  if (!pktlog_txt) BX_PANIC(("ne2k-pktlog.txt failed"));
  fprintf(pktlog_txt, "vnet packetmover readable log file\n");
  fprintf(pktlog_txt, "TFTP root = %s\n", netif);
  fprintf(pktlog_txt, "host MAC address = ");
  int i;
  for (i=0; i<6; i++)
    fprintf(pktlog_txt, "%02x%s", 0xff & dhcp.host_macaddr[i], i<5?":" : "\n");
  fprintf(pktlog_txt, "guest MAC address = ");
  for (i=0; i<6; i++)
    fprintf(pktlog_txt, "%02x%s", 0xff & dhcp.guest_macaddr[i], i<5?":" : "\n");
  fprintf(pktlog_txt, "--\n");
  fflush(pktlog_txt);
#endif
#if BX_ETH_VNET_PCAP_LOGGING
  pcapp = pcap_open_dead(DLT_EN10MB, BX_PACKET_BUFSIZE);
  pktlog_pcap = pcap_dump_open(pcapp, "ne2k-pktlog.pcap");
  if (pktlog_pcap == NULL) BX_PANIC(("ne2k-pktlog.pcap failed"));
#endif
}

bx_vnet_pktmover_c::~bx_vnet_pktmover_c()
{
  bx_vnet_instances--;
}

void bx_vnet_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  guest_to_host((const Bit8u *)buf,io_len);
}

void bx_vnet_pktmover_c::guest_to_host(const Bit8u *buf, unsigned io_len)
{
#if BX_ETH_VNET_LOGGING
  write_pktlog_txt(pktlog_txt, buf, io_len, 0);
#endif
#if BX_ETH_VNET_PCAP_LOGGING
  if (pktlog_pcap && !ferror((FILE *)pktlog_pcap)) {
    Bit64u time = bx_pc_system.time_usec();
    pcaphdr.ts.tv_usec = time % 1000000;
    pcaphdr.ts.tv_sec = time / 1000000;
    pcaphdr.caplen = io_len;
    pcaphdr.len = io_len;
    pcap_dump((u_char *)pktlog_pcap, &pcaphdr, buf);
    fflush((FILE *)pktlog_pcap);
  }
#endif

  this->tx_time = (64 + 96 + 4 * 8 + io_len * 8) / this->netdev_speed;
  if ((io_len >= 14) &&
      (!memcmp(&buf[6],&dhcp.guest_macaddr[0],6)) &&
      (!memcmp(&buf[0],&dhcp.host_macaddr[0],6) ||
       !memcmp(&buf[0],&broadcast_macaddr[0],6))) {
    switch (get_net2(&buf[12])) {
      case ETHERNET_TYPE_IPV4:
        process_ipv4(buf, io_len);
        break;
      case ETHERNET_TYPE_ARP:
        process_arp(buf, io_len);
        break;
      default: // unknown packet type.
        break;
    }
  }
}

// The receive poll process
void bx_vnet_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_vnet_pktmover_c *class_ptr = (bx_vnet_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void bx_vnet_pktmover_c::rx_timer(void)
{
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    this->rxh(this->netdev, (void *)packet_buffer, packet_len);
#if BX_ETH_VNET_LOGGING
    write_pktlog_txt(pktlog_txt, packet_buffer, packet_len, 1);
#endif
#if BX_ETH_VNET_PCAP_LOGGING
    if (pktlog_pcap && !ferror((FILE *)pktlog_pcap)) {
      Bit64u time = bx_pc_system.time_usec();
      pcaphdr.ts.tv_usec = time % 1000000;
      pcaphdr.ts.tv_sec = time / 1000000;
      pcaphdr.caplen = packet_len;
      pcaphdr.len = packet_len;
      pcap_dump((u_char *)pktlog_pcap, &pcaphdr, packet_buffer);
      fflush((FILE *)pktlog_pcap);
    }
#endif
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

void bx_vnet_pktmover_c::host_to_guest(Bit8u *buf, unsigned io_len)
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

  packet_len = io_len;
  memcpy(&packet_buffer, &buf[0], io_len);
  unsigned rx_time = (64 + 96 + 4 * 8 + io_len * 8) / this->netdev_speed;
  bx_pc_system.activate_timer(this->rx_timer_index, this->tx_time + rx_time + 100, 0);
}

/////////////////////////////////////////////////////////////////////////
// ARP
/////////////////////////////////////////////////////////////////////////

void bx_vnet_pktmover_c::process_arp(const Bit8u *buf, unsigned io_len)
{
  unsigned opcode;
  unsigned protocol;
  Bit8u replybuf[MIN_RX_PACKET_LEN];

  if (io_len < 22) return;
  if (io_len < (unsigned)(22+buf[18]*2+buf[19]*2)) return;
  // hardware:Ethernet
  if (buf[14] != 0x00 || buf[15] != 0x01 || buf[18] != 0x06) return;
  opcode = get_net2(&buf[20]);
  protocol = get_net2(&buf[16]);
  memset(&replybuf[0], 0, MIN_RX_PACKET_LEN);

  // protocol
  switch (protocol) {
  case 0x0800: // IPv4
    if (buf[19] == 0x04) {
      switch (opcode) {
      case 0x0001: // ARP REQUEST
        if (!memcmp(&buf[22],&dhcp.guest_macaddr[0],6)) {
          memcpy(&dhcp.guest_ipv4addr[0],&buf[28],4);
          if (!memcmp(&buf[38],&dhcp.host_ipv4addr[0],4)) {
            memcpy(&replybuf[14],&buf[14],6);
            replybuf[20]=0x00;
            replybuf[21]=0x02;
            memcpy(&replybuf[22],&dhcp.host_macaddr[0],6);
            memcpy(&replybuf[28],&dhcp.host_ipv4addr[0],4);
            memcpy(&replybuf[32],&dhcp.guest_macaddr[0],6);
            memcpy(&replybuf[38],&dhcp.guest_ipv4addr[0],4);

            host_to_guest_arp(replybuf, MIN_RX_PACKET_LEN);
          }
        }
        break;
      case 0x0002: // ARP REPLY
        BX_INFO(("unexpected ARP REPLY"));
        break;
      case 0x0003: // RARP REQUEST
        BX_ERROR(("RARP is not implemented"));
        break;
      case 0x0004: // RARP REPLY
        BX_INFO(("unexpected RARP REPLY"));
        break;
      default:
        BX_INFO(("arp: unknown ARP opcode %04x", opcode));
        break;
      }
    } else {
      BX_INFO(("arp: unknown address length %u", (unsigned)buf[19]));
    }
    break;
  default:
    BX_INFO(("arp: unknown protocol 0x%04x", protocol));
    break;
  }
}

void bx_vnet_pktmover_c::host_to_guest_arp(Bit8u *buf, unsigned io_len)
{
  memcpy(&buf[0],&dhcp.guest_macaddr[0],6);
  memcpy(&buf[6],&dhcp.host_macaddr[0],6);
  buf[12]=0x08;
  buf[13]=0x06;
  host_to_guest(buf,io_len);
}

/////////////////////////////////////////////////////////////////////////
// IPv4
/////////////////////////////////////////////////////////////////////////

void bx_vnet_pktmover_c::process_ipv4(const Bit8u *buf, unsigned io_len)
{
  unsigned total_len;
//  unsigned packet_id;
  unsigned fragment_flags;
  unsigned fragment_offset;
  unsigned ipproto;
  unsigned l3header_len;
  const Bit8u *l4pkt;
  unsigned l4pkt_len;

  if (io_len < (14U+20U)) {
    BX_INFO(("ip packet - too small packet"));
    return;
  }

  ip_header_t *iphdr = (ip_header_t *)((Bit8u *)buf +
                                       sizeof(ethernet_header_t));
  if (iphdr->version != 4) {
    BX_INFO(("ipv%u packet - not implemented", iphdr->version));
    return;
  }
  l3header_len = (iphdr->header_len << 2);
  if (l3header_len != 20) {
    BX_ERROR(("ip: option header is not implemented"));
    return;
  }
  if (io_len < (14U+l3header_len)) return;
  if (ip_checksum((Bit8u*)iphdr, l3header_len) != (Bit16u)0xffff) {
    BX_INFO(("ip: invalid checksum"));
    return;
  }

  total_len = ntohs(iphdr->total_len);

  // FIXED By EaseWay
  // Ignore this check to tolerant some cases
  //if (io_len > (14U+total_len)) return;

  if (memcmp(&iphdr->dst_addr, dhcp.host_ipv4addr, 4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[0],4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[1],4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[2],4))
  {
    BX_INFO(("target IP address %u.%u.%u.%u is unknown",
      (unsigned)buf[14+16],(unsigned)buf[14+17],
      (unsigned)buf[14+18],(unsigned)buf[14+19]));
    return;
  }

  fragment_flags = ntohs(iphdr->frag_offs) >> 13;
  fragment_offset = (ntohs(iphdr->frag_offs) & 0x1fff) << 3;
  ipproto = iphdr->protocol;

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    BX_INFO(("ignore fragmented packet!"));
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
    BX_INFO(("unknown IP protocol %02x",ipproto));
    break;
  }
}

void bx_vnet_pktmover_c::host_to_guest_ipv4(Bit8u *buf, unsigned io_len)
{
  unsigned l3header_len;

  memcpy(&buf[0],&dhcp.guest_macaddr[0],6);
  memcpy(&buf[6],&dhcp.host_macaddr[0],6);
  buf[12]=0x08;
  buf[13]=0x00;
  buf[14+0] = (buf[14+0] & 0x0f) | 0x40;
  l3header_len = ((unsigned)(buf[14+0] & 0x0f) << 2);
  memcpy(&buf[14+12],&dhcp.host_ipv4addr[0],4);
  memcpy(&buf[14+16],&dhcp.guest_ipv4addr[0],4);
  put_net2(&buf[14+10], 0);
  put_net2(&buf[14+10], ip_checksum(&buf[14],l3header_len) ^ (Bit16u)0xffff);

  host_to_guest(buf,io_len);
}

layer4_handler_t bx_vnet_pktmover_c::get_layer4_handler(
  unsigned ipprotocol, unsigned port)
{
  unsigned n;

  for (n = 0; n < l4data_used; n++) {
    if (l4data[n].ipprotocol == ipprotocol && l4data[n].port == port)
      return l4data[n].func;
  }

  return (layer4_handler_t)NULL;
}

bx_bool bx_vnet_pktmover_c::register_layer4_handler(
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

bx_bool bx_vnet_pktmover_c::unregister_layer4_handler(
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

void bx_vnet_pktmover_c::process_icmpipv4(
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

void bx_vnet_pktmover_c::process_tcpipv4(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  if (l4pkt_len < 20) return;

  BX_INFO(("tcp packet - not implemented"));
}

void bx_vnet_pktmover_c::process_udpipv4(
  const Bit8u *ipheader, unsigned ipheader_len,
  const Bit8u *l4pkt, unsigned l4pkt_len)
{
  unsigned udp_targetport;
  unsigned udp_sourceport;
//  unsigned udp_len;
  layer4_handler_t func;

  if (l4pkt_len < 8) return;
  udp_header_t *udphdr = (udp_header_t *)l4pkt;
  udp_sourceport = ntohs(udphdr->src_port);
  udp_targetport = ntohs(udphdr->dst_port);
//  udp_len = ntohs(udphdr->length);

  func = get_layer4_handler(0x11, udp_targetport);
  if (func != (layer4_handler_t)NULL) {
    (*func)((void *)this,ipheader, ipheader_len,
      udp_sourceport, udp_targetport, &l4pkt[8], l4pkt_len-8);
  } else {
    BX_INFO(("udp - unhandled port %u", udp_targetport));
  }
}

void bx_vnet_pktmover_c::host_to_guest_udpipv4_packet(
  unsigned target_port, unsigned source_port,
  const Bit8u *udpdata, unsigned udpdata_len)
{
  Bit8u ipbuf[BX_PACKET_BUFSIZE];

  if ((udpdata_len + 42U) > BX_PACKET_BUFSIZE) {
    BX_PANIC(("generated udp data is too long"));
    return;
  }

  // udp pseudo-header
  ipbuf[34U-12U]=0;
  ipbuf[34U-11U]=0x11; // UDP
  put_net2(&ipbuf[34U-10U],8U+udpdata_len);
  memcpy(&ipbuf[34U-8U],dhcp.host_ipv4addr,4);
  memcpy(&ipbuf[34U-4U],dhcp.guest_ipv4addr,4);
  // udp header
  put_net2(&ipbuf[34U+0],source_port);
  put_net2(&ipbuf[34U+2],target_port);
  put_net2(&ipbuf[34U+4],8U+udpdata_len);
  put_net2(&ipbuf[34U+6],0);
  memcpy(&ipbuf[42U],udpdata,udpdata_len);
  put_net2(&ipbuf[34U+6], ip_checksum(&ipbuf[34U-12U],12U+8U+udpdata_len) ^ (Bit16u)0xffff);
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

  host_to_guest_ipv4(ipbuf,udpdata_len + 42U);
}

/////////////////////////////////////////////////////////////////////////
// ICMP/IPv4
/////////////////////////////////////////////////////////////////////////

void bx_vnet_pktmover_c::process_icmpipv4_echo(
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

void bx_vnet_pktmover_c::udpipv4_dhcp_handler(
  void *this_ptr,
  const Bit8u *ipheader, unsigned ipheader_len,
  unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len)
{
  ((bx_vnet_pktmover_c *)this_ptr)->udpipv4_dhcp_handler_ns(
    ipheader,ipheader_len,sourceport,targetport,data,data_len);
}

void bx_vnet_pktmover_c::udpipv4_dhcp_handler_ns(
    const Bit8u *ipheader, unsigned ipheader_len,
    unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len)
{
  Bit8u replybuf[576];
  unsigned opts_len;

  opts_len = process_dhcp(netdev, data, data_len, replybuf, &dhcp);
  if (opts_len > 0) {
    host_to_guest_udpipv4_packet(sourceport, targetport, replybuf, opts_len);
  }
}

void bx_vnet_pktmover_c::udpipv4_tftp_handler(
  void *this_ptr,
  const Bit8u *ipheader, unsigned ipheader_len,
  unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len)
{
  ((bx_vnet_pktmover_c *)this_ptr)->udpipv4_tftp_handler_ns(
    ipheader,ipheader_len,sourceport,targetport,data,data_len);
}

void bx_vnet_pktmover_c::udpipv4_tftp_handler_ns(
  const Bit8u *ipheader, unsigned ipheader_len,
  unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len)
{
  Bit8u replybuf[TFTP_BUFFER_SIZE + 4];
  int len;

  len = process_tftp(netdev, data, data_len, sourceport, replybuf, tftp_rootdir);
  if (len > 0) {
    host_to_guest_udpipv4_packet(sourceport, targetport, replybuf, len);
  }
}

#endif /* if BX_NETWORKING */
