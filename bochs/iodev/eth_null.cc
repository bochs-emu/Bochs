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

// eth_null.cc  - skeleton code for an ethernet pktmover

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

#include "bochs.h"
#define LOG_THIS /* not needed */


//
//  Define the class. This is private to this module
//
class bx_null_pktmover_c : public eth_pktmover_c {
public:
  bx_null_pktmover_c(const char *netif, const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_null_locator_c : public eth_locator_c {
public:
  bx_null_locator_c(void) : eth_locator_c("null") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg) {
    return (new bx_null_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_null_match;


//
// Define the methods for the bx_null_pktmover derived class
//

// the constructor
bx_null_pktmover_c::bx_null_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
  // Nothing here.
}

// the output routine - called with pre-formatted ethernet frame.
void
bx_null_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  // silent discard.
}

