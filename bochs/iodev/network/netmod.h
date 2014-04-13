/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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

// Peter Grehan (grehan@iprg.nokia.com) coded the initial version of the
// NE2000/ether stuff.

//  netmod.h  - see eth_null.cc for implementation details

#ifndef BX_NETMOD_H
#define BX_NETMOD_H

// Pseudo device that loads the lowlevel networking module
class bx_netmod_ctl_c : public bx_netmod_ctl_stub_c {
public:
  bx_netmod_ctl_c();
  virtual ~bx_netmod_ctl_c() {}
  virtual void* init_module(bx_list_c *base, void* rxh, void* rxstat, bx_devmodel_c *dev);
};

#define BX_PACKET_BUFSIZE 2048 // Enough for an ether frame

// device receive status definitions
#define BX_NETDEV_RXREADY  0x0001
#define BX_NETDEV_SPEED    0x000e
#define BX_NETDEV_10MBIT   0x0002
#define BX_NETDEV_100MBIT  0x0004
#define BX_NETDEV_1GBIT    0x0008

// this should not be smaller than an arp reply with an ethernet header
#define MIN_RX_PACKET_LEN 60

#define ETHERNET_MAC_ADDR_LEN   6
#define ETHERNET_TYPE_IPV4 0x0800
#define ETHERNET_TYPE_ARP  0x0806

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

typedef void (*eth_rx_handler_t)(void *arg, const void *buf, unsigned len);
typedef Bit32u (*eth_rx_status_t)(void *arg);

typedef struct {
  Bit8u host_macaddr[6];
  Bit8u guest_macaddr[6];
  Bit8u host_ipv4addr[4];
  const Bit8u *default_guest_ipv4addr;
  Bit8u guest_ipv4addr[4];
  Bit8u dns_ipv4addr[4];
} dhcp_cfg_t;

static const Bit8u broadcast_macaddr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

int execute_script(bx_devmodel_c *netdev, const char *name, char* arg1);
void write_pktlog_txt(FILE *pktlog_txt, const Bit8u *buf, unsigned len, bx_bool host_to_guest);

BX_CPP_INLINE Bit16u get_net2(const Bit8u *buf)
{
  return (((Bit16u)*buf) << 8) |
         ((Bit16u)*(buf+1));
}

BX_CPP_INLINE void put_net2(Bit8u *buf,Bit16u data)
{
  *buf = (Bit8u)(data >> 8);
  *(buf+1) = (Bit8u)(data & 0xff);
}

BX_CPP_INLINE Bit32u get_net4(const Bit8u *buf)
{
  return (((Bit32u)*buf) << 24) |
         (((Bit32u)*(buf+1)) << 16) |
         (((Bit32u)*(buf+2)) << 8) |
         ((Bit32u)*(buf+3));
}

BX_CPP_INLINE void put_net4(Bit8u *buf,Bit32u data)
{
  *buf = (Bit8u)((data >> 24) & 0xff);
  *(buf+1) = (Bit8u)((data >> 16) & 0xff);
  *(buf+2) = (Bit8u)((data >> 8) & 0xff);
  *(buf+3) = (Bit8u)(data & 0xff);
}

Bit16u ip_checksum(const Bit8u *buf, unsigned buf_len);
int process_dhcp(bx_devmodel_c *netdev, const Bit8u *data, unsigned data_len, Bit8u *reply, dhcp_cfg_t *dhcp);
int process_tftp(bx_devmodel_c *netdev, const Bit8u *data, unsigned data_len, Bit16u req_tid, Bit8u *reply, const char *tftp_rootdir);

//
//  The eth_pktmover class is used by ethernet chip emulations
// to interface to the outside world. An instance of this
// would allow frames to be sent to and received from some
// entity. An example would be the packet filter on a Unix
// system, an NDIS driver in promisc mode on WinNT, or maybe
// a simulated network that talks to another process.
//
class eth_pktmover_c {
public:
  virtual void sendpkt(void *buf, unsigned io_len) = 0;
  virtual ~eth_pktmover_c () {}
protected:
  bx_devmodel_c *netdev;
  eth_rx_handler_t  rxh;   // receive callback
  eth_rx_status_t  rxstat; // receive status callback
};


//
//  The eth_locator class is used by pktmover classes to register
// their name. Chip emulations use the static 'create' method
// to locate and instantiate a pktmover class.
//
class eth_locator_c {
public:
  static eth_pktmover_c *create(const char *type, const char *netif,
                                const char *macaddr,
                                eth_rx_handler_t rxh,
                                eth_rx_status_t rxstat,
                                bx_devmodel_c *dev,
                                const char *script);
protected:
  eth_locator_c(const char *type);
  virtual ~eth_locator_c() {}
  virtual eth_pktmover_c *allocate(const char *netif,
                                   const char *macaddr,
                                   eth_rx_handler_t rxh,
                                   eth_rx_status_t rxstat,
                                   bx_devmodel_c *dev,
                                   const char *script) = 0;
private:
  static eth_locator_c *all;
  eth_locator_c *next;
  const char *type;
};

#endif
