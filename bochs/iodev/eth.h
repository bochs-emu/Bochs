//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

//  eth.h  - see eth_null.cc for implementation details

typedef void (*eth_rx_handler_t)(void *arg, const void *buf, unsigned len);


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
protected:
  eth_rx_handler_t  rxh;   // receive callback
  void *rxarg;
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
				void *rxarg);
protected:
  eth_locator_c(const char *type);
  virtual eth_pktmover_c *allocate(const char *netif,
				const char *macaddr,
				eth_rx_handler_t rxh, 
				void *rxarg) = 0;
private:
  static eth_locator_c *all;
  eth_locator_c *next;
  const char *type;
};


// Define the known pktmover modules
#define ETH_NULL  1
#ifdef __FreeBSD__
#define ETH_FBSD  1
#endif
