/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2017  The Bochs Project
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

//  netutil.h  - shared code for eth_vnet.cc and bxhub.cc

#ifndef BX_NETUTIL_H
#define BX_NETUTIL_H

#define ETHERNET_MAC_ADDR_LEN   6
#define ETHERNET_TYPE_IPV4 0x0800
#define ETHERNET_TYPE_ARP  0x0806

#define ARP_OPCODE_REQUEST     1
#define ARP_OPCODE_REPLY       2
#define ARP_OPCODE_REV_REQUEST 3
#define ARP_OPCODE_REV_REPLY   4

#define ICMP_ECHO_PACKET_MAX  128

#define TFTP_BUFFER_SIZE 1024

#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

typedef struct ethernet_header {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  Bit8u  dst_mac_addr[ETHERNET_MAC_ADDR_LEN];
  Bit8u  src_mac_addr[ETHERNET_MAC_ADDR_LEN];
  Bit16u type;
} 
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
ethernet_header_t;

typedef struct arp_header {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  Bit16u  hw_addr_space;
  Bit16u  proto_addr_space;
  Bit8u   hw_addr_len;
  Bit8u   proto_addr_len;
  Bit16u  opcode;
  /* HW address of sender */
  /* Protocol address of sender */
  /* HW address of target*/
  /* Protocol address of target */
}
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
arp_header_t;

typedef struct ip_header {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
#ifdef BX_LITTLE_ENDIAN
  Bit8u header_len : 4;
  Bit8u version : 4;
#else
  Bit8u version : 4;
  Bit8u header_len : 4;
#endif
  Bit8u tos;
  Bit16u total_len;
  Bit16u id;
  Bit16u frag_offs;
  Bit8u ttl;
  Bit8u protocol;
  Bit16u checksum;
  Bit32u src_addr;
  Bit32u dst_addr;
} 
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
ip_header_t;

typedef struct udp_header {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  Bit16u src_port;
  Bit16u dst_port;
  Bit16u length;
  Bit16u checksum;
} 
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
udp_header_t;

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

// DHCP configuration structure
typedef struct {
  Bit8u host_macaddr[6];
  Bit8u guest_macaddr[6];
  Bit8u host_ipv4addr[4];
  Bit8u default_guest_ipv4addr[4];
  Bit8u guest_ipv4addr[4];
  Bit8u dns_ipv4addr[4];
  char *hostname;
} dhcp_cfg_t;

// vnet functions shared with bxhub
#ifdef BXHUB
#define bx_devmodel_c void
#endif
Bit16u ip_checksum(const Bit8u *buf, unsigned buf_len);
void vnet_prepare_reply(Bit8u *replybuf, unsigned l3type, dhcp_cfg_t *dhcpc);
bx_bool vnet_process_arp_request(const Bit8u *buf, Bit8u *reply, dhcp_cfg_t *dhcp);
bx_bool vnet_process_icmp_echo(const Bit8u *l3pkt, unsigned l3header_len,
                               const Bit8u *l4pkt, unsigned l4pkt_len,
                               Bit8u *reply);
int vnet_process_dhcp(bx_devmodel_c *netdev, const Bit8u *data, unsigned data_len,
                      Bit8u *reply, dhcp_cfg_t *dhcp);
int vnet_process_tftp(bx_devmodel_c *netdev, const Bit8u *data, unsigned data_len,
                      Bit16u req_tid, Bit8u *reply, const char *tftp_rootdir);

#endif
