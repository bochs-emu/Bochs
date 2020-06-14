/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2020  The Bochs Project
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

#define BX_PACKET_BUFSIZE 1514 // Maximum size of an ethernet frame

// this should not be smaller than an arp reply with an ethernet header
#define MIN_RX_PACKET_LEN 60

static const Bit8u broadcast_macaddr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

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

#ifndef BXHUB

// Pseudo device that loads the lowlevel networking module
class BOCHSAPI bx_netmod_ctl_c : public logfunctions {
public:
  bx_netmod_ctl_c();
  virtual ~bx_netmod_ctl_c() {}
  void init(void);
  void exit(void);
  virtual void* init_module(bx_list_c *base, void* rxh, void* rxstat, bx_devmodel_c *dev);
};

BOCHSAPI extern bx_netmod_ctl_c bx_netmod_ctl;

// device receive status definitions
#define BX_NETDEV_RXREADY  0x0001
#define BX_NETDEV_SPEED    0x000e
#define BX_NETDEV_10MBIT   0x0002
#define BX_NETDEV_100MBIT  0x0004
#define BX_NETDEV_1GBIT    0x0008

typedef void (*eth_rx_handler_t)(void *arg, const void *buf, unsigned len);
typedef Bit32u (*eth_rx_status_t)(void *arg);

int execute_script(bx_devmodel_c *netdev, const char *name, char* arg1);
void BOCHSAPI_MSVCONLY write_pktlog_txt(FILE *pktlog_txt, const Bit8u *buf, unsigned len, bx_bool host_to_guest);

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
class BOCHSAPI_MSVCONLY eth_locator_c {
public:
  static bx_bool module_present(const char *type);
  static void cleanup();
  static eth_pktmover_c *create(const char *type, const char *netif,
                                const char *macaddr,
                                eth_rx_handler_t rxh,
                                eth_rx_status_t rxstat,
                                bx_devmodel_c *dev,
                                const char *script);
protected:
  eth_locator_c(const char *type);
  virtual ~eth_locator_c();
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

#endif
