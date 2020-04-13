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
#include "misc/bxcompat.h"
#else
#include "iodev.h"
#endif

#if BX_NETWORKING

#include "netmod.h"
#include "netutil.h"

#if !defined(WIN32) || defined(__CYGWIN__)
#include <arpa/inet.h> /* ntohs, htons */
#else
#include <winsock2.h>
#endif

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

  register_layer4_handler(0x11, INET_PORT_BOOTP_SERVER, udpipv4_dhcp_handler);
  register_layer4_handler(0x11, INET_PORT_TFTP_SERVER, udpipv4_tftp_handler);
  register_layer4_handler(0x11, INET_PORT_DOMAIN, udpipv4_dns_handler);

  for (Bit8u c = 0; c < VNET_MAX_CLIENTS; c++) {
    client[c].init = 0;
  }
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

int vnet_server_c::handle_packet(const Bit8u *buf, unsigned len, Bit8u *reply)
{
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;
  Bit8u clientid = 0xff;
  int ret = 0;

  if (len >= 14) {
    if (!find_client(ethhdr->src_mac_addr, &clientid))
      return ret;
    if (!memcmp(&ethhdr->dst_mac_addr, dhcp->host_macaddr, 6) ||
        !memcmp(&ethhdr->dst_mac_addr, &broadcast_macaddr[0], 6)) {
      switch (ntohs(ethhdr->type)) {
        case ETHERNET_TYPE_IPV4: 
          ret = process_ipv4((Bit8u)clientid, buf, len, reply);
          break;
        case ETHERNET_TYPE_ARP:
          ret = process_arp((Bit8u)clientid, buf, len, reply);
          break;
        default: // unknown packet type.
          break;
      }
    }
  }
  if (ret > 0) {
    if (ret < 14) {
      BX_ERROR(("host_to_guest: io_len < 14!"));
      return 0;
    }
    if (ret < MIN_RX_PACKET_LEN) {
      ret = MIN_RX_PACKET_LEN;
    }
    ethernet_header_t *ethrhdr = (ethernet_header_t *)reply;
    memcpy(ethrhdr->dst_mac_addr, ethhdr->src_mac_addr, ETHERNET_MAC_ADDR_LEN);
    memcpy(ethrhdr->src_mac_addr, dhcp->host_macaddr, ETHERNET_MAC_ADDR_LEN);
    ethrhdr->type = ethhdr->type;
  }
  return ret;
}

int vnet_server_c::process_arp(Bit8u clientid, const Bit8u *buf, unsigned len, Bit8u *reply)
{
  int ret = 0;

  if (len < 22) return 0;
  if (len < (unsigned)(22+buf[18]*2+buf[19]*2)) return 0;

  arp_header_t *arphdr = (arp_header_t *)((Bit8u *)buf +
                                          sizeof(ethernet_header_t));
  arp_header_t *arprhdr = (arp_header_t *)(reply + sizeof(ethernet_header_t));

  if ((ntohs(arphdr->hw_addr_space) != 0x0001) ||
      (ntohs(arphdr->proto_addr_space) != 0x0800) ||
      (arphdr->hw_addr_len != ETHERNET_MAC_ADDR_LEN) ||
      (arphdr->proto_addr_len != 4)) {
    BX_ERROR(("Unhandled ARP message hw: 0x%04x (%d) proto: 0x%04x (%d)",
              ntohs(arphdr->hw_addr_space), arphdr->hw_addr_len,
              ntohs(arphdr->proto_addr_space), arphdr->proto_addr_len));
    return 0;
  }

  switch (ntohs(arphdr->opcode)) {
    case ARP_OPCODE_REQUEST:
      if (!memcmp(&buf[22], client[clientid].macaddr, 6)) {
        memcpy(client[clientid].ipv4addr, &buf[28], 4);
        if (!memcmp(&buf[38], dhcp->host_ipv4addr, 4) ||
            !memcmp(&buf[38], dhcp->dns_ipv4addr, 4)) {
          memset(reply, 0, MIN_RX_PACKET_LEN);
          memcpy(arprhdr, &buf[14], 6);
          put_net2((Bit8u*)&arprhdr->opcode, ARP_OPCODE_REPLY);
          memcpy((Bit8u *)arprhdr+8, dhcp->host_macaddr, ETHERNET_MAC_ADDR_LEN);
          memcpy((Bit8u *)arprhdr+14, &buf[38], 4);
          memcpy((Bit8u *)arprhdr+18, client[clientid].macaddr, ETHERNET_MAC_ADDR_LEN);
          memcpy((Bit8u *)arprhdr+24, client[clientid].ipv4addr, 4);
          ret = MIN_RX_PACKET_LEN;
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
  return ret;
}

int vnet_server_c::process_ipv4(Bit8u clientid, const Bit8u *buf, unsigned len, Bit8u *reply)
{
  int ret = 0;
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
    return 0;
  }

  ip_header_t *iphdr = (ip_header_t *)((Bit8u *)buf +
                                       sizeof(ethernet_header_t));
  if (iphdr->version != 4) {
    BX_ERROR(("ipv%u packet - not implemented", iphdr->version));
    return 0;
  }
  l3header_len = (iphdr->header_len << 2);
  if (l3header_len != 20) {
    BX_ERROR(("ip: option header is not implemented"));
    return 0;
  }
  if (ip_checksum((Bit8u*)iphdr, l3header_len) != (Bit16u)0xffff) {
    BX_ERROR(("ip: invalid checksum"));
    return 0;
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
    return 0;
  }

  fragment_flags = ntohs(iphdr->frag_offs) >> 13;
  fragment_offset = (ntohs(iphdr->frag_offs) & 0x1fff) << 3;

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    BX_ERROR(("ignore fragmented packet!"));
    return 0;
  }

  l4pkt = &buf[14 + l3header_len];
  l4pkt_len = total_len - l3header_len;

  switch (iphdr->protocol) {
    case 0x01: // ICMP
      ret = process_icmpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len, reply);
      break;
    case 0x06: // TCP
      ret = process_tcpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len, reply);
      break;
    case 0x11: // UDP
      ret = process_udpipv4(clientid, &buf[14], l3header_len, l4pkt, l4pkt_len, reply);
      break;
    default:
      BX_ERROR(("unknown IP protocol %02x", iphdr->protocol));
      break;
  }
  if (ret > 0) {
    // host-to-guest IPv4
    reply[14] = (reply[14] & 0x0f) | 0x40;
    l3header_len = ((unsigned)(reply[14] & 0x0f) << 2);
    memcpy(&reply[26], srv_ipv4addr, 4);
    memcpy(&reply[30], client[clientid].ipv4addr, 4);
    put_net2(&reply[24], 0);
    put_net2(&reply[24], ip_checksum(&reply[14], l3header_len) ^ (Bit16u)0xffff);
  }
  return ret;
}

int vnet_server_c::process_icmpipv4(Bit8u clientid, const Bit8u *ipheader,
                                    unsigned ipheader_len, const Bit8u *l4pkt,
                                    unsigned l4pkt_len, Bit8u *reply)
{
  int ret = 0;
  unsigned icmptype;
  unsigned icmpcode;

  if (l4pkt_len < 8) return 0;
  icmptype = l4pkt[0];
  icmpcode = l4pkt[1];
  if (ip_checksum(l4pkt, l4pkt_len) != (Bit16u)0xffff) {
    BX_ERROR(("icmp: invalid checksum"));
    return 0;
  }

  switch (icmptype) {
    case 0x08: // ECHO
      if (icmpcode == 0) {
        if ((14U+ipheader_len+l4pkt_len) > ICMP_ECHO_PACKET_MAX) {
          return 0;
        }
        memcpy(&reply[14], ipheader, ipheader_len);
        memcpy(&reply[14+ipheader_len], l4pkt, l4pkt_len);
        reply[14+ipheader_len+0] = 0x00; // echo reply
        put_net2(&reply[14+ipheader_len+2],0);
        put_net2(&reply [14+ipheader_len+2],
          ip_checksum(&reply[14+ipheader_len],l4pkt_len) ^ (Bit16u)0xffff);
        ret = ICMP_ECHO_PACKET_MAX;
      }
      break;
    default:
      BX_ERROR(("unhandled icmp packet: type=%u code=%u", icmptype, icmpcode));
      break;
  }
  return ret;
}

int vnet_server_c::process_tcpipv4(Bit8u clientid, const Bit8u *ipheader,
                                unsigned ipheader_len, const Bit8u *l4pkt,
                                unsigned l4pkt_len, Bit8u *reply)
{
  if (l4pkt_len < 20) return 0;

  BX_ERROR(("tcp packet - not implemented"));
  // TODO
  return 0;
}

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

int vnet_server_c::process_udpipv4(Bit8u clientid, const Bit8u *ipheader,
                                   unsigned ipheader_len, const Bit8u *l4pkt,
                                   unsigned l4pkt_len, Bit8u *reply)
{
  unsigned udp_dst_port;
  unsigned udp_src_port;
  int udp_len = 0;
  Bit8u *udpreply = &reply[42];
  layer4_handler_t func;

  if (l4pkt_len < 8) return 0;
  udp_header_t *udphdr = (udp_header_t *)l4pkt;
  udp_src_port = ntohs(udphdr->src_port);
  udp_dst_port = ntohs(udphdr->dst_port);
//  udp_len = ntohs(udphdr->length);
  ip_header_t *iphdr = (ip_header_t *)ipheader;
  bx_bool dns_service = !memcmp(&iphdr->dst_addr, dhcp->dns_ipv4addr, 4);
  if (dns_service != (udp_dst_port ==INET_PORT_DOMAIN))
    return 0;

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
      return 0;
    }
    // udp pseudo-header
    reply[34U-12U] = 0;
    reply[34U-11U] = 0x11; // UDP
    put_net2(&reply[34U-10U], 8U+udp_len);
    if (dns_service) {
      memcpy(&reply[34U-8U], dhcp->dns_ipv4addr, 4);
    } else {
      memcpy(&reply[34U-8U], dhcp->host_ipv4addr, 4);
    }
    memcpy(&reply[34U-4U], client[clientid].ipv4addr, 4);
    // udp header
    put_net2(&reply[34U+0], udp_dst_port);
    put_net2(&reply[34U+2], udp_src_port);
    put_net2(&reply[34U+4],8U+udp_len);
    put_net2(&reply[34U+6],0);
    put_net2(&reply[34U+6], ip_checksum(&reply[34U-12U],12U+8U+udp_len) ^ (Bit16u)0xffff);
    // ip header
    memset(&reply[14U], 0, 20U);
    reply[14U+0] = 0x45;
    reply[14U+1] = 0x00;
    put_net2(&reply[14U+2], 20U+8U+udp_len);
    put_net2(&reply[14U+4], 1);
    reply[14U+6] = 0x00;
    reply[14U+7] = 0x00;
    reply[14U+8] = 0x07; // TTL
    reply[14U+9] = 0x11; // UDP
  }
  return (udp_len + 42);
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
  s->write = mode;
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
      if (s->write) {
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
        if (s->write == 1) {
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
        if (s->write == 0) {
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
