/////////////////////////////////////////////////////////////////////////
// $Id: eth.h,v 1.24 2009-12-04 19:50:27 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

//  eth.h  - see eth_null.cc for implementation details

#ifndef BX_ETH_H
#define BX_ETH_H

#define BX_PACKET_BUFSIZE 2048 // Enough for an ether frame

typedef void (*eth_rx_handler_t)(void *arg, const void *buf, unsigned len);

static const Bit8u broadcast_macaddr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

int execute_script(bx_devmodel_c *netdev,  const char *name, char* arg1);
void write_pktlog_txt(FILE *pktlog_txt, const Bit8u *buf, unsigned len, bx_bool host_to_guest);

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
                                bx_devmodel_c *dev,
                                const char *script);
protected:
  eth_locator_c(const char *type);
  virtual ~eth_locator_c() {}
  virtual eth_pktmover_c *allocate(const char *netif,
                                   const char *macaddr,
                                   eth_rx_handler_t rxh,
                                   bx_devmodel_c *dev,
                                   const char *script) = 0;
private:
  static eth_locator_c *all;
  eth_locator_c *next;
  const char *type;
};

#endif
