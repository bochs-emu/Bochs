/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2020  The Bochs Project
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
//

//  netutil.cc  - shared code for eth_vnet.cc and bxhub.cc

#define BX_PLUGGABLE

#ifdef BXHUB
#include "config.h"
#define WIN32_LEAN_AND_MEAN
#include "misc/bxcompat.h"
#else
#include "iodev.h"
#endif

#if BX_NETWORKING

#include "netmod.h"
#include "netutil.h"

#if !defined(WIN32) || defined(__CYGWIN__)
#include <arpa/inet.h> /* ntohs, htons */
#include <dirent.h> /* opendir, readdir */
#include <locale.h> /* setlocale */
#else
#include <winsock2.h>
#endif

typedef struct ftp_session {
  Bit8u state;
  bx_bool anonymous;
  Bit16u pasv_port;
  Bit16u client_cmd_port;
  Bit16u client_data_port;
  char *rel_path;
  struct ftp_session *next;
} ftp_session_t;


Bit16u ip_checksum(const Bit8u *buf, unsigned buf_len)
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

// VNET server definitions

#ifdef BXHUB
#include <stdarg.h>
#include "misc/bxcompat.h"
#include "osdep.h"
#else
#define LOG_THIS netdev->
#endif

#define ETHERNET_TYPE_IPV4 0x0800
#define ETHERNET_TYPE_ARP  0x0806

#define ARP_OPCODE_REQUEST     1
#define ARP_OPCODE_REPLY       2
#define ARP_OPCODE_REV_REQUEST 3
#define ARP_OPCODE_REV_REPLY   4

#define ICMP_ECHO_PACKET_MAX  128

#define INET_PORT_FTPDATA 20
#define INET_PORT_FTP 21
#define INET_PORT_TIME 37
#define INET_PORT_NAME 42
#define INET_PORT_DOMAIN 53
#define INET_PORT_BOOTP_SERVER 67
#define INET_PORT_BOOTP_CLIENT 68
#define INET_PORT_HTTP 80
#define INET_PORT_NTP 123

// DHCP server

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
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

#define DEFAULT_LEASE_TIME 28800

// TFTP server support by EaseWay <easeway@123.com>

#define INET_PORT_TFTP_SERVER 69

#define TFTP_RRQ    1
#define TFTP_WRQ    2
#define TFTP_DATA   3
#define TFTP_ACK    4
#define TFTP_ERROR  5
#define TFTP_OPTACK 6

#define TFTP_OPTION_OCTET   0x1
#define TFTP_OPTION_BLKSIZE 0x2
#define TFTP_OPTION_TSIZE   0x4
#define TFTP_OPTION_TIMEOUT 0x8

#define TFTP_DEFAULT_BLKSIZE 512
#define TFTP_DEFAULT_TIMEOUT   5

#define TFTP_BUFFER_SIZE 1024

static Bit8u broadcast_ipv4addr[3][4] =
{
  {  0,  0,  0,  0},
  {255,255,255,255},
  {192,168, 10,255}
};

static const Bit8u subnetmask_ipv4addr[4] = {0xff,0xff,0xff,0x00};

// VNET server code

vnet_server_c::vnet_server_c()
{
#ifdef BXHUB
  logfd = stderr;
#endif
  l4data_used = 0;

  for (Bit8u c = 0; c < VNET_MAX_CLIENTS; c++) {
    client[c].init = 0;
  }
  packet_counter = 0;
  packets = NULL;
}

vnet_server_c::~vnet_server_c()
{
  for (Bit8u c = 0; c < VNET_MAX_CLIENTS; c++) {
    if (client[c].init) {
      delete [] client[c].hostname;
    }
  }
#ifdef BXHUB
  if (logfd != stderr) {
    fclose(logfd);
  }
  logfd = stderr;
#endif
}

void vnet_server_c::init(bx_devmodel_c *_netdev, dhcp_cfg_t *dhcpc, const char *tftp_rootdir)
{
  netdev = _netdev;
  dhcp = dhcpc;
  memcpy(broadcast_ipv4addr[2], dhcp->host_ipv4addr, 3);
  tftp_root = tftp_rootdir;

  register_layer4_handler(0x11, INET_PORT_BOOTP_SERVER, udpipv4_dhcp_handler);
  register_layer4_handler(0x11, INET_PORT_DOMAIN, udpipv4_dns_handler);
  if (strlen(tftp_root) > 0) {
    register_layer4_handler(0x11, INET_PORT_TFTP_SERVER, udpipv4_tftp_handler);
    register_tcp_handler(INET_PORT_FTP, tcpipv4_ftp_handler);
    srand((unsigned)time(NULL)); // for random FTP data port
  }
}

void vnet_server_c::init_client(Bit8u clientid, const Bit8u *macaddr, const Bit8u *default_ipv4addr)
{
  if (clientid < VNET_MAX_CLIENTS) {
    client[clientid].macaddr = macaddr;
    client[clientid].default_ipv4addr = default_ipv4addr;
    memset(client[clientid].ipv4addr, 0, 4);
    client[clientid].hostname = new char[256];
    client[clientid].hostname[0] = 0;
    client[clientid].init = 1;
  }
}

#ifdef BXHUB
void vnet_server_c::init_log(const char *logfn)
{
  logfd = fopen(logfn, "w");
}

void vnet_server_c::bx_printf(const char *fmt, ...)
{
  va_list ap;
  char msg[128];

  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  fprintf(logfd, "%s\n", msg);
  va_end(ap);
}
#endif

bx_bool vnet_server_c::find_client(const Bit8u *mac_addr, Bit8u *clientid)
{
  for (Bit8u c = 0; c < VNET_MAX_CLIENTS; c++) {
    if (client[c].init) {
      if (memcmp(mac_addr, client[c].macaddr, ETHERNET_MAC_ADDR_LEN) == 0) {
        *clientid = c;
        break;
      }
    }
  }
  return (*clientid < VNET_MAX_CLIENTS);
}

void vnet_server_c::handle_packet(const Bit8u *buf, unsigned len)
{
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;
  Bit8u clientid = 0xff;

  if (len >= 14) {
    if (!find_client(ethhdr->src_mac_addr, &clientid))
      return;
    if (!memcmp(&ethhdr->dst_mac_addr, dhcp->host_macaddr, 6) ||
        !memcmp(&ethhdr->dst_mac_addr, &broadcast_macaddr[0], 6)) {
      switch (ntohs(ethhdr->type)) {
        case ETHERNET_TYPE_IPV4: 
          process_ipv4(clientid, buf, len);
          break;
        case ETHERNET_TYPE_ARP:
          process_arp(clientid, buf, len);
          break;
        default: // unknown packet type.
          break;
      }
    }
  }
}

unsigned vnet_server_c::get_packet(Bit8u *buf)
{
  unsigned len = 0;
  packet_item_t *tmp;

  if (packets != NULL) {
    len = packets->len;
    memcpy(buf, packets->buffer, len);
    tmp = packets->next;
    delete [] packets->buffer;
    delete packets;
    packets = tmp;
  }
  return len;
}

void vnet_server_c::host_to_guest(Bit8u clientid, Bit8u *buf, unsigned len, unsigned l3type)
{
  packet_item_t *pitem, *tmp;

  if (len < 14) {
    BX_ERROR(("host_to_guest: io_len < 14!"));
    return;
  }
  if (len < MIN_RX_PACKET_LEN) {
    len = MIN_RX_PACKET_LEN;
  }
  ethernet_header_t *ethrhdr = (ethernet_header_t *)buf;
  memcpy(ethrhdr->dst_mac_addr, client[clientid].macaddr, ETHERNET_MAC_ADDR_LEN);
  memcpy(ethrhdr->src_mac_addr, dhcp->host_macaddr, ETHERNET_MAC_ADDR_LEN);
  ethrhdr->type = htons(l3type);

  pitem = new packet_item_t;
  pitem->buffer = new Bit8u[len];
  memcpy(pitem->buffer, buf, len);
  pitem->len = len;
  pitem->next = NULL;
  if (packets == NULL) {
    packets = pitem;
  } else {
    tmp = packets;
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = pitem;
  }
}

/////////////////////////////////////////////////////////////////////////
// ARP
/////////////////////////////////////////////////////////////////////////

void vnet_server_c::process_arp(Bit8u clientid, const Bit8u *buf, unsigned len)
{
  Bit8u replybuf[MIN_RX_PACKET_LEN];

  if (len < 22) return;
  if (len < (unsigned)(22+buf[18]*2+buf[19]*2)) return;

  arp_header_t *arphdr = (arp_header_t *)((Bit8u *)buf +
                                          sizeof(ethernet_header_t));
  arp_header_t *arprhdr = (arp_header_t *)(replybuf + sizeof(ethernet_header_t));

  if ((ntohs(arphdr->hw_addr_space) != 0x0001) ||
      (ntohs(arphdr->proto_addr_space) != 0x0800) ||
      (arphdr->hw_addr_len != ETHERNET_MAC_ADDR_LEN) ||
      (arphdr->proto_addr_len != 4)) {
    BX_ERROR(("Unhandled ARP message hw: 0x%04x (%d) proto: 0x%04x (%d)",
              ntohs(arphdr->hw_addr_space), arphdr->hw_addr_len,
              ntohs(arphdr->proto_addr_space), arphdr->proto_addr_len));
    return;
  }

  switch (ntohs(arphdr->opcode)) {
    case ARP_OPCODE_REQUEST:
      if (!memcmp(&buf[22], client[clientid].macaddr, 6)) {
        memcpy(client[clientid].ipv4addr, &buf[28], 4);
        if (!memcmp(&buf[38], dhcp->host_ipv4addr, 4) ||
            !memcmp(&buf[38], dhcp->dns_ipv4addr, 4)) {
          memset(replybuf, 0, MIN_RX_PACKET_LEN);
          memcpy(arprhdr, &buf[14], 6);
          put_net2((Bit8u*)&arprhdr->opcode, ARP_OPCODE_REPLY);
          memcpy((Bit8u *)arprhdr+8, dhcp->host_macaddr, ETHERNET_MAC_ADDR_LEN);
          memcpy((Bit8u *)arprhdr+14, &buf[38], 4);
          memcpy((Bit8u *)arprhdr+18, client[clientid].macaddr, ETHERNET_MAC_ADDR_LEN);
          memcpy((Bit8u *)arprhdr+24, client[clientid].ipv4addr, 4);
          host_to_guest(clientid, replybuf, MIN_RX_PACKET_LEN, ETHERNET_TYPE_ARP);
        }
      }
      break;
    case ARP_OPCODE_REPLY:
      BX_ERROR(("unexpected ARP REPLY"));
      break;
    case ARP_OPCODE_REV_REQUEST:
      BX_ERROR(("RARP is not implemented"));
      break;
    case ARP_OPCODE_REV_REPLY:
      BX_ERROR(("unexpected RARP REPLY"));
      break;
    default:
      BX_ERROR(("arp: unknown ARP opcode 0x%04x", ntohs(arphdr->opcode)));
      break;
  }
}

/////////////////////////////////////////////////////////////////////////
// IPv4
/////////////////////////////////////////////////////////////////////////

void vnet_server_c::process_ipv4(Bit8u clientid, const Bit8u *buf, unsigned len)
{
  unsigned total_len;
  unsigned fragment_flags;
  unsigned fragment_offset;
  unsigned l3header_len;
  const Bit8u *l4pkt;
  unsigned l4pkt_len;
  Bit8u srv_ipv4addr[4];
  bx_bool dns_service = 0;

  if (len < (14U+20U)) {
    BX_ERROR(("ip packet - too small packet"));
    return;
  }

  ip_header_t *iphdr = (ip_header_t *)((Bit8u *)buf +
                                       sizeof(ethernet_header_t));
  if (iphdr->version != 4) {
    BX_ERROR(("ipv%u packet - not implemented", iphdr->version));
    return;
  }
  l3header_len = (iphdr->header_len << 2);
  if (l3header_len != 20) {
    BX_ERROR(("ip: option header is not implemented"));
    return;
  }
  if (ip_checksum((Bit8u*)iphdr, l3header_len) != (Bit16u)0xffff) {
    BX_ERROR(("ip: invalid checksum"));
    return;
  }

  total_len = ntohs(iphdr->total_len);

  if (!memcmp(&iphdr->dst_addr, dhcp->dns_ipv4addr, 4)) {
    memcpy(srv_ipv4addr, dhcp->dns_ipv4addr, 4);
    dns_service = 1;
  } else {
    memcpy(srv_ipv4addr, dhcp->host_ipv4addr, 4);
  }

  if (memcmp(&iphdr->dst_addr, dhcp->host_ipv4addr, 4) && !dns_service &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[0], 4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[1], 4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[2], 4))
  {
    BX_ERROR(("target IP address %u.%u.%u.%u is unknown",
      (unsigned)buf[14+16],(unsigned)buf[14+17],
      (unsigned)buf[14+18],(unsigned)buf[14+19]));
    return;
  }

  fragment_flags = ntohs(iphdr->frag_offs) >> 13;
  fragment_offset = (ntohs(iphdr->frag_offs) & 0x1fff) << 3;

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    BX_ERROR(("ignore fragmented packet!"));
    return;
  }

  l4pkt = &buf[14 + l3header_len];
  l4pkt_len = total_len - l3header_len;

  switch (iphdr->protocol) {
    case 0x01: // ICMP
      process_icmpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len);
      break;
    case 0x06: // TCP
      process_tcpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len);
      break;
    case 0x11: // UDP
      process_udpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len);
      break;
    default:
      BX_ERROR(("unknown IP protocol %02x", iphdr->protocol));
      break;
  }
}

void vnet_server_c::host_to_guest_ipv4(const Bit8u clientid, bx_bool dns_srv, Bit8u *buf, unsigned len)
{
  unsigned l3header_len;
  ip_header_t *iphdr = (ip_header_t *)((Bit8u *)buf +
                                       sizeof(ethernet_header_t));

  iphdr->version = 4;
  l3header_len = (iphdr->header_len << 2);
  iphdr->id = htons(++packet_counter);
  if (dns_srv) {
    memcpy(iphdr->src_addr, dhcp->dns_ipv4addr, 4);
  } else {
    memcpy(iphdr->src_addr, dhcp->host_ipv4addr, 4);
  }
  memcpy(iphdr->dst_addr, client[clientid].ipv4addr, 4);
  iphdr->checksum = 0;
  iphdr->checksum = htons(ip_checksum((Bit8u*)iphdr, l3header_len) ^ (Bit16u)0xffff);

  host_to_guest(clientid, buf, len, ETHERNET_TYPE_IPV4);
}

/////////////////////////////////////////////////////////////////////////
// ICMP/IPv4
/////////////////////////////////////////////////////////////////////////

void vnet_server_c::process_icmpipv4(Bit8u clientid, const Bit8u *ipheader,
                                     unsigned ipheader_len, const Bit8u *l4pkt,
                                     unsigned l4pkt_len)
{
  unsigned icmptype;
  unsigned icmpcode;
  Bit8u replybuf[ICMP_ECHO_PACKET_MAX];

  if (l4pkt_len < 8) return;
  icmptype = l4pkt[0];
  icmpcode = l4pkt[1];
  if (ip_checksum(l4pkt, l4pkt_len) != (Bit16u)0xffff) {
    BX_ERROR(("icmp: invalid checksum"));
    return;
  }

  switch (icmptype) {
    case 0x08: // ECHO
      if (icmpcode == 0) {
        if ((14U+ipheader_len+l4pkt_len) > ICMP_ECHO_PACKET_MAX) {
          return;
        }
        memcpy(&replybuf[14], ipheader, ipheader_len);
        memcpy(&replybuf[14+ipheader_len], l4pkt, l4pkt_len);
        replybuf[14+ipheader_len+0] = 0x00; // echo reply
        put_net2(&replybuf[14+ipheader_len+2],0);
        put_net2(&replybuf[14+ipheader_len+2],
          ip_checksum(&replybuf[14+ipheader_len],l4pkt_len) ^ (Bit16u)0xffff);
        host_to_guest_ipv4(clientid, 0, replybuf, 14U+ipheader_len+l4pkt_len);
      }
      break;
    default:
      BX_ERROR(("unhandled icmp packet: type=%u code=%u", icmptype, icmpcode));
      break;
  }
}

/////////////////////////////////////////////////////////////////////////
// TCP/IPv4
/////////////////////////////////////////////////////////////////////////

#define TCP_DISCONNECTED  0
#define TCP_CONNECTING    1
#define TCP_CONNECTED     2
#define TCP_DISCONNECTING 3

// TCP handler methods

tcp_handler_t vnet_server_c::get_tcp_handler(unsigned port)
{
  unsigned n;

  for (n = 0; n < tcpfn_used; n++) {
    if (tcpfn[n].port == port)
      return tcpfn[n].func;
  }

  return (tcp_handler_t)NULL;
}

bx_bool vnet_server_c::register_tcp_handler(unsigned port, tcp_handler_t func)
{
  if (get_tcp_handler(port) != (tcp_handler_t)NULL) {
    BX_ERROR(("TCP port %u is already in use", port));
    return 0;
  }

  unsigned n;

  for (n = 0; n < tcpfn_used; n++) {
    if (tcpfn[n].func == (tcp_handler_t)NULL) {
      break;
    }
  }

  if (n == tcpfn_used) {
    if (n >= LAYER4_LISTEN_MAX) {
      BX_ERROR(("vnet: LAYER4_LISTEN_MAX is too small"));
      return 0;
    }
    tcpfn_used++;
  }

  tcpfn[n].port = port;
  tcpfn[n].func = func;

  return 1;
}

bx_bool vnet_server_c::unregister_tcp_handler(unsigned port)
{
  unsigned n;

  for (n = 0; n < tcpfn_used; n++) {
    if (tcpfn[n].port == port) {
      tcpfn[n].func = (tcp_handler_t)NULL;
      return 1;
    }
  }

  BX_ERROR(("TCP port %u is not registered", port));
  return 0;
}

// TCP connection handling functions

tcp_conn_t *tcp_connections = NULL;

tcp_conn_t *tcp_new_connection(Bit8u clientid, Bit16u src_port, Bit16u dst_port)
{
  tcp_conn_t *tc = new tcp_conn_t;
  memset(tc, 0, sizeof(tcp_conn_t));
  tc->clientid = clientid;
  tc->src_port = src_port;
  tc->dst_port = dst_port;
  tc->next = tcp_connections;
  tcp_connections = tc;
  return tc;
}

tcp_conn_t *tcp_find_connection(Bit8u clientid, Bit16u src_port, Bit16u dst_port)
{
  tcp_conn_t *tc = tcp_connections;
  while (tc != NULL) {
    if ((tc->clientid != clientid) || (tc->src_port != src_port) || (tc->dst_port != dst_port))
      tc = tc->next;
    else
      break;
  }
  return tc;
}

void tcp_remove_connection(tcp_conn_t *tc)
{
  tcp_conn_t *last;

  if (tcp_connections == tc) {
    tcp_connections = tc->next;
  } else {
    last = tcp_connections;
    while (last != NULL) {
      if (last->next != tc)
        last = last->next;
      else
        break;
    }
    if (last) {
      last->next = tc->next;
    }
  }
  delete tc;
}

void vnet_server_c::process_tcpipv4(Bit8u clientid, const Bit8u *ipheader,
                                    unsigned ipheader_len, const Bit8u *l4pkt,
                                    unsigned l4pkt_len)
{
  unsigned tcphdr_len, tcpdata_len, tcprhdr_len, tcprdata_len = 0;
  Bit32u tcp_seq_num, tcp_ack_num;
  Bit16u tcp_src_port, tcp_dst_port, tcp_window;
  Bit8u replybuf[MIN_RX_PACKET_LEN];
  const Bit8u *tcp_data;
  tcp_handler_t func;
  tcp_conn_t *tcp_conn;
  bx_bool tcp_error = 1;
  Bit8u option, optlen;
  Bit16u value16;

  if (l4pkt_len < 20) return;
  memset(replybuf, 0, MIN_RX_PACKET_LEN);
  tcp_header_t *tcphdr = (tcp_header_t *)l4pkt;
  tcp_header_t *tcprhdr = (tcp_header_t *)&replybuf[34];
  tcp_src_port = ntohs(tcphdr->src_port);
  tcp_dst_port = ntohs(tcphdr->dst_port);
  tcp_seq_num = ntohl(tcphdr->seq_num);
  tcp_ack_num = ntohl(tcphdr->ack_num);
  tcphdr_len = tcphdr->data_offset << 2;
  tcp_window = ntohs(tcphdr->window);
  tcpdata_len = l4pkt_len - tcphdr_len;
  tcp_data = (Bit8u*)tcphdr + tcphdr_len;
  // TODO: parse options
  if (tcphdr_len > 20) {
    int i = 0;
    do {
      option = l4pkt[20+i];
      if (option < 2) {
        optlen = 1;
      } else {
        optlen = l4pkt[20+i+1];
      }
      if (option == 0) { // EOL
        break;
      }
      if (option == 2) {
        value16 = get_net2(&l4pkt[20+i+2]);
        if (value16 != 1460) {
          BX_ERROR(("TCP: MSS = %d (unhandled)", value16));
        }
      } else if (option != 1) { // NOP
        BX_ERROR(("TCP option %d not supported yet", option));
      }
      i += optlen;
    } while (i < (int)(tcphdr_len - 20));
  }
  // check header
  func = get_tcp_handler(tcp_dst_port);
  tcp_conn = tcp_find_connection(clientid, tcp_src_port, tcp_dst_port);
  tcprhdr_len = sizeof(tcp_header_t);
  if (func != (tcp_handler_t)NULL) {
    if (tcp_conn == (tcp_conn_t*)NULL) {
      if (tcphdr->flags.syn) {
        tcprhdr->flags.syn = 1;
        tcprhdr->flags.ack = 1;
        tcprhdr->seq_num = htonl(1);
        tcprhdr->ack_num = htonl(tcp_seq_num+1);
        tcprhdr->window = htons(tcp_window);
        tcprdata_len = 0;
        tcp_conn = tcp_new_connection(clientid, tcp_src_port, tcp_dst_port);
        tcp_conn->state = TCP_CONNECTING;
        tcp_error = 0;
      }
    } else {
      if (tcphdr->flags.rst) {
        tcp_remove_connection(tcp_conn);
        if (tcp_dst_port > 1023) {
          unregister_tcp_handler(tcp_dst_port);
        }
        return;
      } else if (tcphdr->flags.fin) {
        if (tcp_conn->state == TCP_CONNECTED) {
          tcprhdr->flags.fin = 1;
          tcprhdr->flags.ack = 1;
          tcprhdr->seq_num = htonl(tcp_conn->host_seq_num);
          tcprhdr->ack_num = htonl(tcp_seq_num+1);
          tcprhdr->window = htons(tcp_window);
          tcprdata_len = 0;
          tcp_conn->state = TCP_DISCONNECTING;
          tcp_error = 0;
        } else if (tcp_conn->state == TCP_DISCONNECTING) {
          tcpipv4_send_ack(tcp_conn, 1);
          tcp_remove_connection(tcp_conn);
          if (tcp_dst_port > 1023) {
            unregister_tcp_handler(tcp_dst_port);
          }
          return;
        }
      } else if (tcphdr->flags.ack) {
        if (tcp_conn->state == TCP_CONNECTING) {
          tcp_conn->state = TCP_CONNECTED;
          tcp_conn->guest_seq_num = tcp_seq_num;
          tcp_conn->host_seq_num = tcp_ack_num;
          tcp_conn->window = tcp_window;
          (*func)((void *)this, tcp_conn, tcp_data, tcpdata_len);
        } else if (tcp_conn->state == TCP_DISCONNECTING) {
          if (!tcp_conn->host_port_fin) {
            tcp_remove_connection(tcp_conn);
          } else {
            tcp_conn->host_seq_num = tcp_ack_num;
          }
        } else {
          tcp_conn->guest_seq_num = tcp_seq_num;
          tcp_conn->guest_ack_num = tcp_ack_num;
          if ((tcp_ack_num == tcp_conn->host_seq_num) && (tcp_window > 0)) {
            tcp_conn->window = tcp_window;
            if (tcp_conn->buffer_size > 0) {
              tcpipv4_send_buffer(tcp_conn);
            } else {
              tcp_conn->host_xfer_fin = 1;
            }
          }
          if ((tcpdata_len > 0) || tcp_conn->host_xfer_fin) {
            if (tcpdata_len > 0) {
              tcpipv4_send_ack(tcp_conn, tcpdata_len);
            }
            (*func)((void *)this, tcp_conn, tcp_data, tcpdata_len);
            tcp_conn->host_xfer_fin = 0;
          }
        }
        return;
      } else {
        BX_ERROR(("TCP: unhandled flag"));
        return;
      }
    }
  }
  if (tcp_error) {
    tcprhdr->flags.syn = 0;
    tcprhdr->flags.rst = 1;
    tcprhdr->flags.ack = 1;
    tcprhdr->seq_num = htonl(1);
    tcprhdr->ack_num = htonl(tcp_seq_num+1);
    tcprhdr->window = 0;
    tcprdata_len = 0;
    BX_ERROR(("tcp - port %u unhandled or in use", tcp_dst_port));
  }
  host_to_guest_tcpipv4(clientid, tcp_dst_port, tcp_src_port, replybuf,
                        tcprdata_len, tcprhdr_len);
}

void vnet_server_c::host_to_guest_tcpipv4(Bit8u clientid, Bit16u src_port,
                                          Bit16u dst_port, Bit8u *data,
                                          unsigned data_len, unsigned hdr_len)
{
  tcp_header_t *tcphdr = (tcp_header_t *)&data[34];

  // tcp pseudo-header
  data[34U-12U]=0;
  data[34U-11U]=0x06; // TCP
  put_net2(&data[34U-10U], hdr_len+data_len);
  memcpy(&data[34U-8U], dhcp->host_ipv4addr, 4);
  memcpy(&data[34U-4U], client[clientid].ipv4addr, 4);
  // tcp header
  tcphdr->src_port = htons(src_port);
  tcphdr->dst_port = htons(dst_port);
  tcphdr->data_offset = hdr_len >> 2;
  tcphdr->checksum = 0;
  put_net2(&data[34U+16U], ip_checksum(&data[34U-12U],12U+hdr_len+data_len) ^ (Bit16u)0xffff);
  // ip header
  memset(&data[14U],0,20U);
  data[14U+0] = 0x45;
  data[14U+1] = 0x00;
  put_net2(&data[14U+2],20U+hdr_len+data_len);
  put_net2(&data[14U+4],1);
  data[14U+6] = 0x00;
  data[14U+7] = 0x00;
  data[14U+8] = 0x07; // TTL
  data[14U+9] = 0x06; // TCP

  host_to_guest_ipv4(clientid, 0, data, data_len + hdr_len + 34U);
}

void vnet_server_c::tcpipv4_send_buffer(tcp_conn_t *tcp_conn)
{
  Bit8u sendbuf[BX_PACKET_BUFSIZE];
  tcp_header_t *tcphdr = (tcp_header_t *)&sendbuf[34];
  unsigned tcphdr_len = sizeof(tcp_header_t);
  Bit8u *tcp_data;
  unsigned count = 0, sendsize, total;

  if (tcp_conn->host_xfer_fin) {
    delete [] tcp_conn->buffer;
    tcp_conn->buffer_size = 0;
    return;
  }
  memset(tcphdr, 0, tcphdr_len);
  if ((tcp_conn->buffer_size + 54U) <= BX_PACKET_BUFSIZE) {
    tcphdr->flags.psh = 1;
  }
  tcphdr->flags.ack = 1;
  tcphdr->ack_num = htonl(tcp_conn->guest_seq_num);
  tcphdr->window = htons(tcp_conn->window);
  tcp_data = (Bit8u*)tcphdr + tcphdr_len;
  do {
    total = tcp_conn->buffer_size - tcp_conn->buffer_pos;
    if ((total + 54U) > BX_PACKET_BUFSIZE) {
      sendsize = BX_PACKET_BUFSIZE - 54U;
    } else {
      sendsize = total;
    }
    if ((count + sendsize) > tcp_conn->window)
      break;
    tcphdr->seq_num = htonl(tcp_conn->host_seq_num);
    if (sendsize > 0) {
      memcpy(tcp_data, tcp_conn->buffer+tcp_conn->buffer_pos, sendsize);
    }
    host_to_guest_tcpipv4(tcp_conn->clientid, tcp_conn->dst_port, tcp_conn->src_port,
                          sendbuf, sendsize, tcphdr_len);
    tcp_conn->buffer_pos += sendsize;
    if (tcp_conn->buffer_pos == tcp_conn->buffer_size) {
      delete [] tcp_conn->buffer;
      tcp_conn->buffer_size = 0;
    }
    tcp_conn->host_seq_num += sendsize;
    count += sendsize;
  } while (tcp_conn->buffer_size > 0);
}

void vnet_server_c::tcpipv4_send_data(tcp_conn_t *tcp_conn, const Bit8u *data, unsigned data_len)
{
  if (data_len > 0) {
    tcp_conn->host_xfer_fin = 0;
    tcp_conn->buffer = data;
    tcp_conn->buffer_size = data_len;
    tcp_conn->buffer_pos = 0;
    tcpipv4_send_buffer(tcp_conn);
  } else {
    tcp_conn->host_xfer_fin = 1;
    tcpipv4_send_fin(tcp_conn);
  }
}

void vnet_server_c::tcpipv4_send_ack(tcp_conn_t *tcp_conn, unsigned data_len)
{
  Bit8u replybuf[MIN_RX_PACKET_LEN];
  tcp_header_t *tcphdr = (tcp_header_t *)&replybuf[34];
  unsigned tcphdr_len = sizeof(tcp_header_t);

  memset(replybuf, 0, MIN_RX_PACKET_LEN);
  tcphdr->flags.ack = 1;
  tcphdr->seq_num = htonl(tcp_conn->host_seq_num);
  tcp_conn->guest_seq_num += data_len;
  tcphdr->ack_num = htonl(tcp_conn->guest_seq_num);
  tcphdr->window = htons(tcp_conn->window);
  host_to_guest_tcpipv4(tcp_conn->clientid, tcp_conn->dst_port, tcp_conn->src_port,
                        replybuf, 0, tcphdr_len);
}

void vnet_server_c::tcpipv4_send_fin(tcp_conn_t *tcp_conn)
{
  Bit8u replybuf[MIN_RX_PACKET_LEN];
  tcp_header_t *tcphdr = (tcp_header_t *)&replybuf[34];
  unsigned tcphdr_len = sizeof(tcp_header_t);

  memset(replybuf, 0, MIN_RX_PACKET_LEN);
  tcphdr->flags.fin = 1;
  tcphdr->flags.ack = 1;
  tcphdr->seq_num = htonl(tcp_conn->host_seq_num);
  tcphdr->ack_num = htonl(tcp_conn->guest_seq_num);
  tcphdr->window = htons(tcp_conn->window);
  tcp_conn->state = TCP_DISCONNECTING;
  tcp_conn->host_port_fin = 1;
  host_to_guest_tcpipv4(tcp_conn->clientid, tcp_conn->dst_port, tcp_conn->src_port,
                        replybuf, 0, tcphdr_len);
}

// FTP support

ftp_session_t *ftp_sessions = NULL;

ftp_session_t *ftp_new_session(tcp_conn_t *tcp_conn, Bit16u client_cmd_port)
{
  ftp_session_t *fs = new ftp_session_t;
  memset(fs, 0, sizeof(ftp_session_t));
  fs->state = 1;
  fs->client_cmd_port = client_cmd_port;
  fs->rel_path = new char[BX_PATHNAME_LEN];
  strcpy(fs->rel_path, "/");
  fs->next = ftp_sessions;
  ftp_sessions = fs;
  return fs;
}

ftp_session_t *ftp_find_cmd_session(Bit16u pasv_port)
{
  ftp_session_t *fs = ftp_sessions;
  while (fs != NULL) {
    if (fs->pasv_port != pasv_port)
      fs = fs->next;
    else
      break;
  }
  return fs;
}

void ftp_remove_session(ftp_session_t *fs)
{
  ftp_session_t *last;

  if (ftp_sessions == fs) {
    ftp_sessions = fs->next;
  } else {
    last = ftp_sessions;
    while (last != NULL) {
      if (last->next != fs)
        last = last->next;
      else
        break;
    }
    if (last) {
      last->next = fs->next;
    }
  }
  delete [] fs->rel_path;
  delete fs;
}

bx_bool ftp_directory_exists(const char *root, const char *rel_path)
{
  char abspath[BX_PATHNAME_LEN];
#ifndef WIN32
  DIR *dir;
#endif

  if (!strcmp(rel_path, "/")) {
    strcpy(abspath, root);
  } else {
    sprintf(abspath, "%s%s", root, rel_path);
  }
#ifndef WIN32
  dir = opendir(abspath);
  if (dir != NULL) {
    closedir(dir);
    return 1;
  } else {
    return 0;
  }
#else
  DWORD dwAttrib = GetFileAttributes(abspath);
  return ((dwAttrib != INVALID_FILE_ATTRIBUTES) &&
          (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#endif
}

void vnet_server_c::tcpipv4_ftp_handler(void *this_ptr, tcp_conn_t *tcp_conn, const Bit8u *data, unsigned data_len)
{
  ((vnet_server_c *)this_ptr)->tcpipv4_ftp_handler_ns(tcp_conn, data, data_len);
}

void vnet_server_c::tcpipv4_ftp_handler_ns(tcp_conn_t *tcp_conn, const Bit8u *data, unsigned data_len)
{
  char *ftpcmd, *cmd = NULL, *arg = NULL;
  char reply[80];
  ftp_session_t *fs;
  const Bit8u *hostip;
  bx_bool pasv_port_ok, path_ok;
  tcp_conn_t *tcp_conn_2;
  char tmp_path[BX_PATHNAME_LEN];
  unsigned len;

  if (tcp_conn->dst_port == INET_PORT_FTP) {
    if (tcp_conn->data == NULL) {
      // send welcome message
      ftp_send_reply(tcp_conn, "220 Bochs FTP server ready.");
      fs = ftp_new_session(tcp_conn, tcp_conn->src_port);
      tcp_conn->data = fs;
    } else if (data_len > 0) {
      ftpcmd = new char[data_len + 1];
      memcpy(ftpcmd, data, data_len);
      ftpcmd[data_len] = 0;
      cmd = strtok(ftpcmd, " \r");
      arg = strtok(NULL, "\r");
      fs = (ftp_session_t*)tcp_conn->data;
      if (fs->state == 1) {
        if (!stricmp(cmd, "USER")) {
          fs->anonymous = !strcmp(arg, "anonymous");
          if (!strcmp(arg, "bochs") || fs->anonymous) {
            sprintf(reply, "331 Password required for %s.", arg);
            ftp_send_reply(tcp_conn, reply);
            fs->state = 2;
          } else {
            ftp_send_reply(tcp_conn, "430 Invalid username or password.");
          }
        }
      } else if (fs->state == 2) {
        if (!stricmp(cmd, "PASS")) {
          if (!strcmp(arg, "bochs") || fs->anonymous) {
            sprintf(reply, "230 User %s logged in.", fs->anonymous ? "anonymous":"bochs");
            ftp_send_reply(tcp_conn, reply);
            fs->state = 3;
          } else {
            ftp_send_reply(tcp_conn, "530 Login incorrect.");
          }
        }
      } else {
        if (!stricmp(cmd, "ABOR")) {
          tcp_conn_2 = tcp_find_connection(tcp_conn->clientid, fs->client_data_port, fs->pasv_port);
          tcp_conn_2->host_xfer_fin = 1;
          ftp_send_reply(tcp_conn, "226 Transfer complete.");
        } else if (!stricmp(cmd, "CDUP")) {
          if (!strcmp(fs->rel_path, "/")) {
            ftp_send_reply(tcp_conn, "550 CDUP operation not permitted.");
          } else {
            len = strlen(fs->rel_path);
            do {
              if (len < 2) break;
            } while (fs->rel_path[--len] != '/');
            fs->rel_path[len] = 0;
            ftp_send_reply(tcp_conn, "250 CDUP command successful.");
          }
        } else if (!stricmp(cmd, "CWD")) {
          path_ok = 0;
          if (!strcmp(arg, "/")) {
            strcpy(tmp_path, "/");
            path_ok = 1;
          } else if (arg[0] != '/') {
            if (!strcmp(fs->rel_path, "/")) {
              sprintf(tmp_path, "/%s", arg);
            } else {
              sprintf(tmp_path, "%s/%s", fs->rel_path, arg);
            }
            path_ok = ftp_directory_exists(tftp_root, tmp_path);
          } else {
            strcpy(tmp_path, arg);
            path_ok = ftp_directory_exists(tftp_root, tmp_path);
          }
          if (path_ok) {
            strcpy(fs->rel_path, tmp_path);
            ftp_send_reply(tcp_conn, "250 CWD command successful.");
          } else {
            ftp_send_reply(tcp_conn, "550 CWD operation not permitted.");
          }
        } else if (!stricmp(cmd, "FEAT")) {
          ftp_send_reply(tcp_conn, "211 end");
        } else if (!stricmp(cmd, "LIST")) {
          tcp_conn_2 = tcp_find_connection(tcp_conn->clientid, fs->client_data_port, fs->pasv_port);
          if (tcp_conn_2 != NULL) {
            ftp_send_reply(tcp_conn, "150 Opening ASCII mode connection for file list.");
            if (!ftp_read_directory(tcp_conn_2)) {
              ftp_send_reply(tcp_conn, "226 Transfer complete.");
            }
          } else {
            BX_ERROR(("FTP data connection not found"));
          }
        } else if (!stricmp(cmd, "NOOP")) {
          ftp_send_reply(tcp_conn, "200 Command OK.");
        } else if (!stricmp(cmd, "OPTS")) {
          sprintf(reply, "501 Feature '%s' not supported.", arg);
          ftp_send_reply(tcp_conn, reply);
        } else if (!stricmp(cmd, "PASV")) {
          do {
            fs->pasv_port = (((rand() & 0x7f) | 0x80) << 8) | (rand() & 0xff);
            pasv_port_ok = register_tcp_handler(fs->pasv_port, tcpipv4_ftp_handler);
          } while (!pasv_port_ok);
          BX_DEBUG(("Passive FTP mode using port %d", fs->pasv_port));
          hostip = dhcp->host_ipv4addr;
          sprintf(reply, "227 Entering passive mode (%d, %d, %d, %d, %d, %d).",
                  hostip[0], hostip[1], hostip[2], hostip[3], (fs->pasv_port >> 8),
                  (fs->pasv_port & 0xff));
          ftp_send_reply(tcp_conn, reply);
        } else if (!stricmp(cmd, "PWD")) {
          sprintf(reply, "257 \"%s\" is current directory.", fs->rel_path);
          ftp_send_reply(tcp_conn, reply);
        } else if (!stricmp(cmd, "QUIT")) {
          ftp_send_reply(tcp_conn, "221 Goodbye.");
          ftp_remove_session(fs);
          tcp_conn->data = NULL;
        } else if (!stricmp(cmd, "RETR")) {
          tcp_conn_2 = tcp_find_connection(tcp_conn->clientid, fs->client_data_port, fs->pasv_port);
          if (tcp_conn_2 != NULL) {
            ftp_send_reply(tcp_conn, "150 Opening binary mode connection for file download.");
            if (!ftp_download_file(tcp_conn_2, arg)) {
              ftp_send_reply(tcp_conn, "226 Transfer complete.");
            }
          } else {
            BX_ERROR(("FTP data connection not found"));
          }
        } else if (!stricmp(cmd, "SYST")) {
          ftp_send_reply(tcp_conn, "215 UNIX Type: Bochs Version: 2.6.11");
        } else if (!stricmp(cmd, "TYPE")) {
          sprintf(reply, "200 Type set to %s.", arg);
          ftp_send_reply(tcp_conn, reply);
        } else {
          sprintf(reply, "502 Command '%s' not implemented.", cmd);
          ftp_send_reply(tcp_conn, reply);
        }
      }
      delete [] ftpcmd;
    }
  } else {
    // FTP data port
    fs = ftp_find_cmd_session(tcp_conn->dst_port);
    if (fs == NULL) {
      BX_ERROR(("FTP command connection not found"));
      return;
    }
    if (tcp_conn->data == NULL) {
      fs->client_data_port = tcp_conn->src_port;
      tcp_conn->data = fs;
    } else if (tcp_conn->host_xfer_fin) {
      tcp_conn_2 = tcp_find_connection(tcp_conn->clientid, fs->client_cmd_port, INET_PORT_FTP);
      if (tcp_conn_2 != NULL) {
        tcpipv4_send_fin(tcp_conn);
        ftp_send_reply(tcp_conn_2, "226 Transfer complete.");
      } else {
        BX_ERROR(("FTP command connection not found"));
      }
    } else if (data_len > 0) {
      fs = (ftp_session_t*)tcp_conn->data;
      BX_INFO(("FTP data port %d (data)", fs->pasv_port));
    }
  }
}

void vnet_server_c::ftp_send_reply(tcp_conn_t *tcp_conn, const char *msg)
{
  if (strlen(msg) > 0) {
    char *reply = new char[strlen(msg) + 3];
    sprintf(reply, "%s%c%c", msg, 13, 10);
    tcpipv4_send_data(tcp_conn, (Bit8u*)reply, strlen(reply));
  }
}

bx_bool vnet_server_c::ftp_read_directory(tcp_conn_t *tcp_conn)
{
  Bit8u *dirlist = NULL;
  ftp_session_t *fs;
  char abspath[BX_PATHNAME_LEN];
  char linebuf[BX_PATHNAME_LEN], tmptime[20];
  unsigned size = 0, pos = 0;
  Bit8u pass = 1;
#ifndef WIN32
  DIR *dir;
  struct dirent *dent;
  struct stat st;
  char path[BX_PATHNAME_LEN];
  time_t now = time(NULL);
#endif

  fs = (ftp_session_t*)tcp_conn->data;
  if (!strcmp(fs->rel_path, "/")) {
    strcpy(abspath, tftp_root);
  } else {
    sprintf(abspath, "%s%s", tftp_root, fs->rel_path);
  }
#ifndef WIN32
  setlocale(LC_ALL, "en_US");
  do {
    dir = opendir(abspath);
    if (dir != NULL) {
      while ((dent=readdir(dir)) != NULL) {
        if (strcmp(dent->d_name, ".") && strcmp(dent->d_name, "..") && (dent->d_name[0] != '.' )) {
          sprintf(path, "%s/%s", abspath, dent->d_name);
          if (stat(path, &st) >= 0) {
            if (st.st_mtime < (now - 31536000)) {
              strftime(tmptime, 20, "%b %d %Y", localtime(&st.st_mtime));
            } else {
              strftime(tmptime, 20, "%b %d %H:%M", localtime(&st.st_mtime));
            }
            if (S_ISDIR(st.st_mode)) {
              sprintf(linebuf, "drwxrwxr-x 1 ftp ftp %ld %s %s%c%c", st.st_size,
                      tmptime, dent->d_name, 13, 10);
            } else {
              sprintf(linebuf, "-rw-rw-r-- 1 ftp ftp %ld %s %s%c%c", st.st_size,
                      tmptime, dent->d_name, 13, 10);
            }
            if (pass == 1) {
              size += strlen(linebuf);
            } else {
              memcpy(dirlist + pos, linebuf, strlen(linebuf));
              pos += strlen(linebuf);
            }
          }
        }
      }
      closedir(dir);
      if ((pass == 1) && (size > 0)) {
        dirlist = new Bit8u[size];
      }
    }
  } while ((++pass <= 2) && (size > 0));
#else
  WIN32_FIND_DATA finddata;
  HANDLE hFind;
  SYSTEMTIME gmtsystime, systime, now;
  TIME_ZONE_INFORMATION tzi;
  const char month[][12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
                            "Aug", "Sep", "Oct", "Nov", "Dec"};
  char filter[MAX_PATH];

  sprintf(filter, "%s\\*.*", abspath);
  do {
    hFind = FindFirstFile(filter, &finddata);
    if (hFind == INVALID_HANDLE_VALUE) {
      break;
    }
    do {
      if (strcmp(finddata.cFileName, ".") && strcmp(finddata.cFileName, "..")) {
        FileTimeToSystemTime(&finddata.ftLastWriteTime, &gmtsystime);
        GetTimeZoneInformation(&tzi);
        SystemTimeToTzSpecificLocalTime(&tzi, &gmtsystime, &systime);
        GetLocalTime(&now);
        if ((systime.wYear == now.wYear) ||
            ((systime.wYear == (now.wYear - 1)) && (systime.wMonth > now.wMonth))) {
          sprintf(tmptime, "%s %d %02d:%02d", month[systime.wMonth-1], systime.wDay,
                  systime.wHour, systime.wMinute);
        } else {
          sprintf(tmptime, "%s %d %d", month[systime.wMonth-1], systime.wDay,
                  systime.wYear);
        }
        if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          sprintf(linebuf, "drwxrwxr-x 1 ftp ftp %ld %s %s%c%c", 0, tmptime,
                  finddata.cFileName, 13, 10);
        } else {
          sprintf(linebuf, "-rw-rw-r-- 1 ftp ftp %ld %s %s%c%c", finddata.nFileSizeLow,
                  tmptime, finddata.cFileName, 13, 10);
        }
        if (pass == 1) {
          size += strlen(linebuf);
        } else {
          memcpy(dirlist + pos, linebuf, strlen(linebuf));
          pos += strlen(linebuf);
        }
      }
    } while (FindNextFile(hFind, &finddata));
    FindClose(hFind);
    if ((pass == 1) && (size > 0)) {
      dirlist = new Bit8u[size];
    }
  } while ((++pass <= 2) && (size > 0));
#endif
  tcpipv4_send_data(tcp_conn, dirlist, size);
  return (size > 0);
}

bx_bool vnet_server_c::ftp_download_file(tcp_conn_t *tcp_conn, const char *fname)
{
  char path[BX_PATHNAME_LEN];
  ftp_session_t *fs;
  Bit8u *buffer = NULL;
  unsigned size = 0;
  int fd = -1;
#ifndef WIN32
  struct stat stat_buf;
#endif

  fs = (ftp_session_t*)tcp_conn->data;
  sprintf(path, "%s%s/%s", tftp_root, fs->rel_path, fname);
#ifdef WIN32
  HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    ULARGE_INTEGER FileSize;
    FileSize.LowPart = GetFileSize(hFile, &FileSize.HighPart);
    CloseHandle(hFile);
    if (((FileSize.LowPart != INVALID_FILE_SIZE) || (GetLastError() == NO_ERROR)) &&
        (FileSize.HighPart == 0)) {
      size = FileSize.LowPart;
    } else {
      size = 0;
    }
  } else {
    size = 0;
  }
#endif
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
            );
  if (fd >= 0) {
#ifndef WIN32
    if (fstat(fd, &stat_buf) == 0) {
      size = stat_buf.st_size;
    } else {
      size = 0;
    }
#endif
    if (size > 0) {
      buffer = new Bit8u[size];
      read(fd, buffer, size);
    }
    close(fd);
  }
  tcpipv4_send_data(tcp_conn, buffer, size);
  return (size > 0);
}

// Layer 4 handler methods

layer4_handler_t vnet_server_c::get_layer4_handler(
  unsigned ipprotocol, unsigned port)
{
  unsigned n;

  for (n = 0; n < l4data_used; n++) {
    if (l4data[n].ipprotocol == ipprotocol && l4data[n].port == port)
      return l4data[n].func;
  }

  return (layer4_handler_t)NULL;
}

bx_bool vnet_server_c::register_layer4_handler(
  unsigned ipprotocol, unsigned port,layer4_handler_t func)
{
  if (get_layer4_handler(ipprotocol,port) != (layer4_handler_t)NULL) {
    BX_ERROR(("IP protocol 0x%02x port %u is already in use",
              ipprotocol, port));
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

bx_bool vnet_server_c::unregister_layer4_handler(
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

/////////////////////////////////////////////////////////////////////////
// UDP/IPv4
/////////////////////////////////////////////////////////////////////////

void vnet_server_c::process_udpipv4(Bit8u clientid, const Bit8u *ipheader,
                                   unsigned ipheader_len, const Bit8u *l4pkt,
                                   unsigned l4pkt_len)
{
  unsigned udp_dst_port;
  unsigned udp_src_port;
  int udp_len = 0;
  Bit8u replybuf[BX_PACKET_BUFSIZE];
  Bit8u *udpreply = &replybuf[42];
  layer4_handler_t func;

  if (l4pkt_len < 8) return;
  udp_header_t *udphdr = (udp_header_t *)l4pkt;
  udp_src_port = ntohs(udphdr->src_port);
  udp_dst_port = ntohs(udphdr->dst_port);
//  udp_len = ntohs(udphdr->length);
  ip_header_t *iphdr = (ip_header_t *)ipheader;
  bx_bool dns_srv = !memcmp(&iphdr->dst_addr, dhcp->dns_ipv4addr, 4);
  if (dns_srv != (udp_dst_port ==INET_PORT_DOMAIN))
    return;

  func = get_layer4_handler(0x11, udp_dst_port);
  if (func != (layer4_handler_t)NULL) {
    udp_len = (*func)((void *)this,ipheader, ipheader_len,
              udp_src_port, udp_dst_port, &l4pkt[8], l4pkt_len-8, udpreply);
  } else {
    BX_ERROR(("udp - unhandled port %u", udp_dst_port));
  }
  if (udp_len > 0) {
    if ((udp_len + 42U) > BX_PACKET_BUFSIZE) {
      BX_ERROR(("generated udp data is too long"));
      return;
    }
    // udp pseudo-header
    replybuf[34U-12U] = 0;
    replybuf[34U-11U] = 0x11; // UDP
    put_net2(&replybuf[34U-10U], 8U+udp_len);
    if (dns_srv) {
      memcpy(&replybuf[34U-8U], dhcp->dns_ipv4addr, 4);
    } else {
      memcpy(&replybuf[34U-8U], dhcp->host_ipv4addr, 4);
    }
    memcpy(&replybuf[34U-4U], client[clientid].ipv4addr, 4);
    // udp header
    put_net2(&replybuf[34U+0], udp_dst_port);
    put_net2(&replybuf[34U+2], udp_src_port);
    put_net2(&replybuf[34U+4],8U+udp_len);
    put_net2(&replybuf[34U+6],0);
    put_net2(&replybuf[34U+6], ip_checksum(&replybuf[34U-12U],12U+8U+udp_len) ^ (Bit16u)0xffff);
    // ip header
    memset(&replybuf[14U], 0, 20U);
    replybuf[14U+0] = 0x45;
    replybuf[14U+1] = 0x00;
    put_net2(&replybuf[14U+2], 20U+8U+udp_len);
    put_net2(&replybuf[14U+4], 1);
    replybuf[14U+6] = 0x00;
    replybuf[14U+7] = 0x00;
    replybuf[14U+8] = 0x07; // TTL
    replybuf[14U+9] = 0x11; // UDP

    host_to_guest_ipv4(clientid, dns_srv, replybuf, udp_len + 42U);
  }
}

int vnet_server_c::udpipv4_dhcp_handler(void *this_ptr, const Bit8u *ipheader,
  unsigned ipheader_len, unsigned sourceport, unsigned targetport,
  const Bit8u *l4pkt, unsigned l4pkt_len, Bit8u *reply)
{
  return ((vnet_server_c *)this_ptr)->udpipv4_dhcp_handler_ns(ipheader,
    ipheader_len, sourceport, targetport, l4pkt, l4pkt_len, reply);
}

int vnet_server_c::udpipv4_dhcp_handler_ns(const Bit8u *ipheader,
    unsigned ipheader_len, unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len, Bit8u *reply)
{
  Bit8u clientid = 0;
  const Bit8u *opts;
  unsigned opts_len;
  unsigned extcode;
  unsigned extlen;
  const Bit8u *extdata;
  unsigned dhcpmsgtype = 0;
  bx_bool found_serverid = false;
  bx_bool found_leasetime = false;
  bx_bool found_guest_ipaddr = false;
  bx_bool found_host_name = false;
  Bit32u leasetime = BX_MAX_BIT32U;
  const Bit8u *dhcpreqparams = NULL;
  unsigned dhcpreqparams_len = 0;
  Bit8u dhcpreqparam_default[8];
  bx_bool dhcpreqparam_default_validflag = false;
  unsigned dhcpreqparams_default_len = 0;
  Bit8u *replyopts;
  Bit8u replybuf[576];
  char *hostname = NULL;
  unsigned hostname_len = 0;
  Bit16u maxsize = 0;

  if (data_len < (236U+4U)) return 0;
  if (data[0] != BOOTREQUEST) return 0;
  if (data[1] != 1 || data[2] != 6) return 0;
  if (!find_client(&data[28U], &clientid)) return 0;
  if (data[236] != 0x63 || data[237] != 0x82 ||
      data[238] != 0x53 || data[239] != 0x63) return 0;

  opts = &data[240];
  opts_len = data_len - 240U;

  while (1) {
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extcode = *opts++;
    opts_len--;

    if (extcode == BOOTPOPT_PADDING) continue;
    if (extcode == BOOTPOPT_END) break;
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extlen = *opts++;
    opts_len--;
    if (opts_len < extlen) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extdata = opts;
    opts += extlen;
    opts_len -= extlen;

    switch (extcode) {
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
      if (memcmp(extdata, dhcp->host_ipv4addr, 4)) {
        BX_INFO(("dhcp: request to another server"));
        return 0;
      }
      found_serverid = true;
      break;
    case BOOTPOPT_IP_ADDRESS_LEASE_TIME:
      if (extlen != 4)
        break;
      leasetime = get_net4(&extdata[0]);
      found_leasetime = true;
      break;
    case BOOTPOPT_REQUESTED_IP_ADDRESS:
      if (extlen != 4)
        break;
      if (!memcmp(extdata, client[clientid].default_ipv4addr, 4)) {
        found_guest_ipaddr = true;
        memcpy(client[clientid].ipv4addr, client[clientid].default_ipv4addr, 4);
      }
      break;
    case BOOTPOPT_HOST_NAME:
      if (extlen < 1)
        break;
      hostname = (char*)malloc(extlen);
      memcpy(hostname, extdata, extlen);
      hostname_len = extlen;
      if ((client[clientid].hostname != NULL) && (extlen < 256)) {
        memcpy(client[clientid].hostname, extdata, extlen);
        client[clientid].hostname[extlen] = 0;
      }
      found_host_name = true;
      break;
    case BOOTPOPT_MAX_DHCP_MESSAGE_SIZE:
      if (extlen < 2)
        break;
      maxsize = get_net2(&extdata[0]);
      if (maxsize < 548U) {
        BX_ERROR(("invalid max. DHCP message size = %d", maxsize));
      }
      break;
    default:
      BX_ERROR(("extcode %d not supported yet (len=%d)", extcode, extlen));
      break;
    }
  }

  memset(&dhcpreqparam_default,0,sizeof(dhcpreqparam_default));
  memset(&replybuf[0],0,sizeof(replybuf));
  replybuf[0] = BOOTREPLY;
  replybuf[1] = 1;
  replybuf[2] = 6;
  memcpy(&replybuf[4],&data[4],4);
  memcpy(&replybuf[16], client[clientid].default_ipv4addr, 4);
  memcpy(&replybuf[20], dhcp->host_ipv4addr, 4);
  memcpy(&replybuf[28],&data[28],6);
  memcpy(&replybuf[44],"vnet",4);
  memcpy(&replybuf[108],"pxelinux.0",10);
  replybuf[236] = 0x63;
  replybuf[237] = 0x82;
  replybuf[238] = 0x53;
  replybuf[239] = 0x63;
  replyopts = &replybuf[240];
  opts_len = sizeof(replybuf)/sizeof(replybuf[0])-240;
  switch (dhcpmsgtype) {
  case DHCPDISCOVER:
    BX_DEBUG(("dhcp server: DHCPDISCOVER"));
    // reset guest address; answer must be broadcasted to unconfigured IP
    memcpy(client[clientid].ipv4addr, broadcast_ipv4addr[1], 4);
    *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
    *replyopts ++ = 1;
    *replyopts ++ = DHCPOFFER;
    opts_len -= 3;
    dhcpreqparam_default[0] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
    dhcpreqparam_default[1] = BOOTPOPT_SERVER_IDENTIFIER;
    if (found_host_name) {
      dhcpreqparam_default[2] = BOOTPOPT_HOST_NAME;
    }
    dhcpreqparam_default_validflag = true;
    break;
  case DHCPREQUEST:
    BX_DEBUG(("dhcp server: DHCPREQUEST"));
    // check ciaddr.
    if (found_serverid || found_guest_ipaddr || (!memcmp(&data[12], client[clientid].default_ipv4addr, 4))) {
      *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
      *replyopts ++ = 1;
      *replyopts ++ = DHCPACK;
      opts_len -= 3;
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
      if (found_leasetime) {
        dhcpreqparam_default[dhcpreqparams_default_len++] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
        dhcpreqparam_default_validflag = true;
      }
      if (!found_serverid) {
        dhcpreqparam_default[dhcpreqparams_default_len++] = BOOTPOPT_SERVER_IDENTIFIER;
        dhcpreqparam_default_validflag = true;
      }
    }
    break;
  default:
    BX_ERROR(("dhcp server: unsupported message type %u",dhcpmsgtype));
    return 0;
  }

  while (1) {
    while (dhcpreqparams_len-- > 0) {
      switch (*dhcpreqparams++) {
      case BOOTPOPT_SUBNETMASK:
        BX_DEBUG(("provide BOOTPOPT_SUBNETMASK"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SUBNETMASK;
        *replyopts ++ = 4;
        memcpy(replyopts,subnetmask_ipv4addr,4);
        replyopts += 4;
        break;
      case BOOTPOPT_ROUTER_OPTION:
        BX_DEBUG(("provide BOOTPOPT_ROUTER_OPTION"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_ROUTER_OPTION;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr, 4);
        replyopts += 4;
        break;
      case BOOTPOPT_DOMAIN_NAMESERVER:
        if (dhcp->dns_ipv4addr[0] != 0) {
          BX_DEBUG(("provide BOOTPOPT_DOMAIN_NAMESERVER"));
          if (opts_len < 6) {
            BX_ERROR(("option buffer is insufficient"));
            return 0;
          }
          opts_len -= 6;
          *replyopts ++ = BOOTPOPT_DOMAIN_NAMESERVER;
          *replyopts ++ = 4;
          memcpy(replyopts, dhcp->dns_ipv4addr, 4);
          replyopts += 4;
        }
        break;
      case BOOTPOPT_BROADCAST_ADDRESS:
        BX_DEBUG(("provide BOOTPOPT_BROADCAST_ADDRESS"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_BROADCAST_ADDRESS;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr, 3);
        replyopts += 3;
        *replyopts ++ = 0xff;
        break;
      case BOOTPOPT_IP_ADDRESS_LEASE_TIME:
        BX_DEBUG(("provide BOOTPOPT_IP_ADDRESS_LEASE_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
        *replyopts ++ = 4;
        if (leasetime < DEFAULT_LEASE_TIME) {
          put_net4(replyopts, leasetime);
        } else {
          put_net4(replyopts, DEFAULT_LEASE_TIME);
        }
        replyopts += 4;
        break;
      case BOOTPOPT_SERVER_IDENTIFIER:
        BX_DEBUG(("provide BOOTPOPT_SERVER_IDENTIFIER"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SERVER_IDENTIFIER;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr,4);
        replyopts += 4;
        break;
      case BOOTPOPT_RENEWAL_TIME:
        BX_DEBUG(("provide BOOTPOPT_RENEWAL_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_RENEWAL_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 600);
        replyopts += 4;
        break;
      case BOOTPOPT_REBINDING_TIME:
        BX_DEBUG(("provide BOOTPOPT_REBINDING_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_REBINDING_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 1800);
        replyopts += 4;
        break;
      case BOOTPOPT_HOST_NAME:
        if (hostname != NULL) {
          BX_DEBUG(("provide BOOTPOPT_HOST_NAME"));
          if (opts_len < (hostname_len + 2)) {
            free(hostname);
            BX_ERROR(("option buffer is insufficient"));
            return 0;
          }
          opts_len -= (hostname_len + 2);
          *replyopts ++ = BOOTPOPT_HOST_NAME;
          *replyopts ++ = hostname_len;
          memcpy(replyopts, hostname, hostname_len);
          replyopts += hostname_len;
          free(hostname);
          hostname = NULL;
          break;
        }
      default:
        if (*(dhcpreqparams-1) != 0) {
          BX_ERROR(("dhcp server: requested parameter %u not supported yet",*(dhcpreqparams-1)));
        }
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
    return 0;
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
  memcpy(reply, replybuf, opts_len);
  return opts_len;
}

// TFTP support

tftp_session_t *tftp_sessions = NULL;

tftp_session_t *tftp_new_session(Bit16u req_tid, bx_bool mode, const char *tpath, const char *tname)
{
  tftp_session_t *s = new tftp_session_t;
  s->tid = req_tid;
  s->iswrite = mode;
  s->options = 0;
  s->blksize_val = TFTP_DEFAULT_BLKSIZE;
  s->timeout_val = TFTP_DEFAULT_TIMEOUT;
  s->next = tftp_sessions;
  tftp_sessions = s;
  if ((strlen(tname) > 0) && ((strlen(tpath) + strlen(tname)) < BX_PATHNAME_LEN)) {
    sprintf(s->filename, "%s/%s", tpath, tname);
  } else {
    s->filename[0] = 0;
  }
  return s;
}

tftp_session_t *tftp_find_session(Bit16u tid)
{
  tftp_session_t *s = tftp_sessions;
  while (s != NULL) {
    if (s->tid != tid)
      s = s->next;
    else
      break;
  }
  return s;
}

void tftp_remove_session(tftp_session_t *s)
{
  tftp_session_t *last;

  if (tftp_sessions == s) {
    tftp_sessions = s->next;
  } else {
    last = tftp_sessions;
    while (last != NULL) {
      if (last->next != s)
        last = last->next;
      else
        break;
    }
    if (last) {
      last->next = s->next;
    }
  }
  delete s;
}

void tftp_update_timestamp(tftp_session_t *s)
{
#ifndef BXHUB
  s->timestamp = (unsigned)(bx_pc_system.time_usec() / 1000000);
#else
  s->timestamp = (unsigned)time(NULL);
#endif
}

void tftp_timeout_check()
{
#ifndef BXHUB
  unsigned curtime = (unsigned)(bx_pc_system.time_usec() / 1000000);
#else
  unsigned curtime = (unsigned)time(NULL);
#endif
  tftp_session_t *next, *s = tftp_sessions;

  while (s != NULL) {
    if ((curtime - s->timestamp) > s->timeout_val) {
      next = s->next;
      tftp_remove_session(s);
      s = next;
    } else {
      s = s->next;
    }
  }
}

int tftp_send_error(Bit8u *buffer, unsigned code, const char *msg, tftp_session_t *s)
{
  put_net2(buffer, TFTP_ERROR);
  put_net2(buffer + 2, code);
  strcpy((char*)buffer + 4, msg);
  if (s != NULL) {
    tftp_remove_session(s);
  }
  return (strlen(msg) + 5);
}

int tftp_send_data(Bit8u *buffer, unsigned block_nr, tftp_session_t *s)
{
  char msg[BX_PATHNAME_LEN + 16];
  int rd;

  FILE *fp = fopen(s->filename, "rb");
  if (!fp) {
    sprintf(msg, "File not found: %s", s->filename);
    return tftp_send_error(buffer, 1, msg, s);
  }

  if (fseek(fp, (block_nr - 1) * s->blksize_val, SEEK_SET) < 0) {
    fclose(fp);
    return tftp_send_error(buffer, 3, "Block not seekable", s);
  }

  rd = fread(buffer + 4, 1, s->blksize_val, fp);
  fclose(fp);

  if (rd < 0) {
    return tftp_send_error(buffer, 3, "Block not readable", s);
  }

  put_net2(buffer, TFTP_DATA);
  put_net2(buffer + 2, block_nr);
  if (rd < (int)s->blksize_val) {
    tftp_remove_session(s);
  } else {
    tftp_update_timestamp(s);
  }
  return (rd + 4);
}

int tftp_send_ack(Bit8u *buffer, unsigned block_nr)
{
  put_net2(buffer, TFTP_ACK);
  put_net2(buffer + 2, block_nr);
  return 4;
}

int tftp_send_optack(Bit8u *buffer, tftp_session_t *s)
{
  Bit8u *p = buffer;
  put_net2(p, TFTP_OPTACK);
  p += 2;
  if (s->options & TFTP_OPTION_TSIZE) {
    *p++='t'; *p++='s'; *p++='i'; *p++='z'; *p++='e'; *p++='\0';
    sprintf((char *)p, "%lu", (unsigned long)s->tsize_val);
    p += strlen((const char *)p) + 1;
  }
  if (s->options & TFTP_OPTION_BLKSIZE) {
    *p++='b'; *p++='l'; *p++='k'; *p++='s'; *p++='i'; *p++='z'; *p++='e'; *p++='\0';
    sprintf((char *)p, "%u", s->blksize_val);
    p += strlen((const char *)p) + 1;
  }
  if (s->options & TFTP_OPTION_TIMEOUT) {
    *p++='t'; *p++='i'; *p++='m'; *p++='e'; *p++='o'; *p++='u'; *p++='t'; *p++='\0';
    sprintf((char *)p, "%u", s->timeout_val);
    p += strlen((const char *)p) + 1;
  }
  tftp_update_timestamp(s);
  return (p - buffer);
}

void vnet_server_c::tftp_parse_options(const char *mode, const Bit8u *data,
                                       unsigned data_len, tftp_session_t *s)
{
  while (mode < (const char*)data + data_len) {
    if (memcmp(mode, "octet\0", 6) == 0) {
      s->options |= TFTP_OPTION_OCTET;
      mode += 6;
    } else if (memcmp(mode, "tsize\0", 6) == 0) {
      s->options |= TFTP_OPTION_TSIZE;             // size needed
      mode += 6;
      if (s->iswrite) {
        s->tsize_val = atoi(mode);
      }
      mode += strlen(mode)+1;
    } else if (memcmp(mode, "blksize\0", 8) == 0) {
      s->options |= TFTP_OPTION_BLKSIZE;
      mode += 8;
      s->blksize_val = atoi(mode);
      if (s->blksize_val > TFTP_BUFFER_SIZE) {
        BX_ERROR(("tftp req: blksize value %d not supported - using %d instead",
                  s->blksize_val, TFTP_BUFFER_SIZE));
        s->blksize_val = TFTP_BUFFER_SIZE;
      }
      mode += strlen(mode)+1;
    } else if (memcmp(mode, "timeout\0", 8) == 0) {
      s->options |= TFTP_OPTION_TIMEOUT;
      mode += 8;
      s->timeout_val = atoi(mode);
      if ((s->timeout_val < 1) || (s->timeout_val > 255)) {
        BX_ERROR(("tftp req: timeout value %d not supported - using %d instead",
                  s->timeout_val, TFTP_DEFAULT_TIMEOUT));
        s->timeout_val = TFTP_DEFAULT_TIMEOUT;
      }
      mode += strlen(mode)+1;
    } else {
      BX_ERROR(("tftp req: unknown option %s", mode));
      break;
    }
  }
}

int vnet_server_c::udpipv4_tftp_handler(void *this_ptr, const Bit8u *ipheader,
  unsigned ipheader_len, unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len, Bit8u *reply)
{
  return ((vnet_server_c *)this_ptr)->udpipv4_tftp_handler_ns(ipheader,
    ipheader_len, sourceport, targetport, data, data_len, reply);
}

int vnet_server_c::udpipv4_tftp_handler_ns(const Bit8u *ipheader,
    unsigned ipheader_len, unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len, Bit8u *reply)
{
  FILE *fp;
  unsigned block_nr;
  unsigned tftp_len;
  unsigned req_tid = sourceport;
  tftp_session_t *s;

  tftp_timeout_check();
  s = tftp_find_session(req_tid);
  switch (get_net2(data)) {
    case TFTP_RRQ:
      {
        if (s != NULL) {
          tftp_remove_session(s);
        }
        strncpy((char*)reply, (const char*)data + 2, data_len - 2);
        reply[data_len - 4] = 0;

        s = tftp_new_session(req_tid, 0, tftp_root, (const char*)reply);
        if (strlen(s->filename) == 0) {
          return tftp_send_error(reply, 1, "Illegal file name", s);
        }
        // options
        if (strlen((char*)reply) < data_len - 2) {
          const char *mode = (const char*)data + 2 + strlen((char*)reply) + 1;
          tftp_parse_options(mode, data, data_len, s);
        }
        if (!(s->options & TFTP_OPTION_OCTET)) {
          return tftp_send_error(reply, 4, "Unsupported transfer mode", NULL);
        }
        if (s->options & TFTP_OPTION_TSIZE) {
          struct stat stbuf;
          if (stat(s->filename, &stbuf) < 0) {
            s->options &= ~TFTP_OPTION_TSIZE;
          } else {
            s->tsize_val = (size_t)stbuf.st_size;
            BX_DEBUG(("tftp filesize: %lu", (unsigned long)s->tsize_val));
          }
        }
        if ((s->options & ~TFTP_OPTION_OCTET) > 0) {
          return tftp_send_optack(reply, s);
        } else {
          return tftp_send_data(reply, 1, s);
        }
      }
      break;

    case TFTP_WRQ:
      {
        if (s != NULL) {
          tftp_remove_session(s);
        }
        strncpy((char*)reply, (const char*)data + 2, data_len - 2);
        reply[data_len - 4] = 0;

        s = tftp_new_session(req_tid, 1, tftp_root, (const char*)reply);
        if (strlen(s->filename) == 0) {
          return tftp_send_error(reply, 1, "Illegal file name", s);
        }
        // options
        if (strlen((char*)reply) < data_len - 2) {
          const char *mode = (const char*)data + 2 + strlen((char*)reply) + 1;
          tftp_parse_options(mode, data, data_len, s);
        }
        if (!(s->options & TFTP_OPTION_OCTET)) {
          return tftp_send_error(reply, 4, "Unsupported transfer mode", NULL);
        }

        fp = fopen(s->filename, "rb");
        if (fp) {
          fclose(fp);
          return tftp_send_error(reply, 6, "File exists", s);
        }
        fp = fopen(s->filename, "wb");
        if (!fp) {
          return tftp_send_error(reply, 2, "Access violation", s);
        }
        fclose(fp);

        if ((s->options & ~TFTP_OPTION_OCTET) > 0) {
          return tftp_send_optack(reply, s);
        } else {
          tftp_update_timestamp(s);
          return tftp_send_ack(reply, 0);
        }
      }
      break;

    case TFTP_DATA:
      if (s != NULL) {
        if (s->iswrite == 1) {
          block_nr = get_net2(data + 2);
          strncpy((char*)reply, (const char*)data + 4, data_len - 4);
          tftp_len = data_len - 4;
          reply[tftp_len] = 0;
          if (tftp_len <= s->blksize_val) {
            fp = fopen(s->filename, "ab");
            if (!fp) {
              return tftp_send_error(reply, 2, "Access violation", s);
            }
            if (fseek(fp, (block_nr - 1) * TFTP_BUFFER_SIZE, SEEK_SET) < 0) {
              fclose(fp);
              return tftp_send_error(reply, 3, "Block not seekable", s);
            }
            fwrite(reply, 1, tftp_len, fp);
            fclose(fp);
            if (tftp_len < s->blksize_val) {
              tftp_remove_session(s);
            } else {
              tftp_update_timestamp(s);
            }
            return tftp_send_ack(reply, block_nr);
          } else {
            return tftp_send_error(reply, 4, "Illegal request", s);
          }
        } else {
          return tftp_send_error(reply, 4, "Illegal request", s);
        }
      } else {
        return tftp_send_error(reply, 5, "Unknown transfer ID", s);
      }
      break;

    case TFTP_ACK:
      if (s != NULL) {
        if (s->iswrite == 0) {
          return tftp_send_data(reply, get_net2(data + 2) + 1, s);
        } else {
          return tftp_send_error(reply, 4, "Illegal request", s);
        }
      }
      break;

    case TFTP_ERROR:
      if (s != NULL) {
        tftp_remove_session(s);
      }
      break;

    default:
      BX_ERROR(("TFTP unknown opt %d", get_net2(data)));
  }
  return 0;
}

int vnet_server_c::udpipv4_dns_handler(void *this_ptr, const Bit8u *ipheader,
  unsigned ipheader_len, unsigned sourceport, unsigned targetport,
  const Bit8u *data, unsigned data_len, Bit8u *reply)
{
  return ((vnet_server_c *)this_ptr)->udpipv4_dns_handler_ns(ipheader,
    ipheader_len, sourceport, targetport, data, data_len, reply);
}

int vnet_server_c::udpipv4_dns_handler_ns(const Bit8u *ipheader,
    unsigned ipheader_len, unsigned sourceport, unsigned targetport,
    const Bit8u *data, unsigned data_len, Bit8u *reply)
{
  char host[256];
  Bit8u len1, p1, p2;
  Bit8u ipaddr[4];
  Bit16u val16[6];
  bx_bool host_found = 0;
  int i, rlen = 0;

  for (i = 0; i < 6; i++) {
    val16[i] = get_net2(&data[i * 2]);
  }
  if ((val16[1] != 0x0100) || (val16[2] != 0x0001))
    return 0;
  p1 = 12;
  p2 = 0;
  while (data[p1] != 0) {
    len1 = data[p1];
    if (p2 > 0) {
      host[p2++] = '.';
    }
    memcpy(&host[p2], &data[p1 + 1], len1);
    p2 += len1;
    p1 += (len1 + 1);
  };
  host[p2] = 0;
  if ((get_net2(&data[p1 + 1]) != 0x0001) || (get_net2(&data[p1 + 3]) != 0x0001))
    return 0;
  if (!stricmp(host, "vnet")) {
    host_found = 1;
    memcpy(&ipaddr, dhcp->host_ipv4addr, 4);
  } else {
    for (i = 0; i < VNET_MAX_CLIENTS; i++) {
      if (client[i].init) {
        if (!stricmp(host, client[i].hostname)) {
          host_found = 1;
          memcpy(&ipaddr, client[i].ipv4addr, 4);
          break;
        }
      }
    }
  }
  if (host_found) {
    memcpy(reply, data, data_len);
    put_net2(&reply[2], 0x8180);
    put_net2(&reply[6], 0x0001);
    reply[data_len] = 0xc0;
    reply[data_len + 1] = 12;
    put_net2(&reply[data_len + 2], 0x0001);
    put_net2(&reply[data_len + 4], 0x0001);
    put_net4(&reply[data_len + 6], 86400);
    put_net2(&reply[data_len + 10], 4);
    memcpy(&reply[data_len + 12], ipaddr, 4);
    rlen = data_len + 16;
  } else {
    BX_ERROR(("DNS: unknown host '%s'", host));
    memcpy(reply, data, data_len);
    put_net2(&reply[2], 0x8183);
    rlen = data_len;
  }
  return rlen;
}

#endif /* if BX_NETWORKING */
