/////////////////////////////////////////////////////////////////////////
// $Id: eth_null.cc,v 1.5.2.1 2002-03-17 08:57:02 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
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

// Various networking docs:
// http://www.graphcomp.com/info/rfc/
// rfc0826: arp
// rfc0903: rarp

#include "bochs.h"
#define LOG_THIS bx_ne2k.


//
//  Define the class. This is private to this module
//
class bx_null_pktmover_c : public eth_pktmover_c {
public:
  bx_null_pktmover_c(const char *netif, const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
private:
  int rx_timer_index;
  static void rx_timer_handler(void *);
  FILE *txlog, *txlog_txt, *rxlog, *rxlog_txt;
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
#if BX_ETH_NULL_LOGGING
  // Start the rx poll 
  this->rx_timer_index = 
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
				1, 1); // continuous, active
  this->rxh   = rxh;
  this->rxarg = rxarg;
  // eventually Bryce wants txlog to dump in pcap format so that
  // tcpdump -r FILE can read it and interpret packets.
  txlog = fopen ("ne2k-tx.log", "wb");
  if (!txlog) BX_PANIC (("open ne2k-tx.log failed"));
  txlog_txt = fopen ("ne2k-txdump.txt", "wb");
  if (!txlog_txt) BX_PANIC (("open ne2k-txdump.txt failed"));
  fprintf (txlog_txt, "null packetmover readable log file\n");
  fprintf (txlog_txt, "net IF = %s\n", netif);
  fprintf (txlog_txt, "MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (txlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf (txlog_txt, "\n--\n");
  fflush (txlog_txt);
#endif
}

void
bx_null_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
#if BX_ETH_NULL_LOGGING
  BX_DEBUG (("sendpkt length %u", io_len));
  // dump raw bytes to a file, eventually dump in pcap format so that
  // tcpdump -r FILE can interpret them for us.
  int n = fwrite (buf, io_len, 1, txlog);
  if (n != 1) BX_ERROR (("fwrite to txlog failed", io_len));
  // dump packet in hex into an ascii log file
  fprintf (txlog_txt, "NE2K transmitting a packet, length %u\n", io_len);
  Bit8u *charbuf = (Bit8u *)buf;
  for (n=0; n<io_len; n++) {
    if (((n % 16) == 0) && n>0)
      fprintf (txlog_txt, "\n");
    fprintf (txlog_txt, "%02x ", charbuf[n]);
  }
  fprintf (txlog_txt, "\n--\n");
  // flush log so that we see the packets as they arrive w/o buffering
  fflush (txlog);
  fflush (txlog_txt);
#endif
}

void bx_null_pktmover_c::rx_timer_handler (void *this_ptr)
{
#if BX_ETH_NULL_LOGGING
  /// hey wait there is no receive data with a NULL ethernet, is there....

  int io_len = 0;
  Bit8u buf[1];
  bx_null_pktmover_c *class_ptr = (bx_null_pktmover_c *) this_ptr;
  if (io_len > 0) {
    BX_DEBUG (("receive packet length %u", io_len));
    // dump raw bytes to a file, eventually dump in pcap format so that
    // tcpdump -r FILE can interpret them for us.
    int n = fwrite (buf, io_len, 1, class_ptr->rxlog);
    if (n != 1) BX_ERROR (("fwrite to rxlog failed", io_len));
    // dump packet in hex into an ascii log file
    fprintf (class_ptr->rxlog_txt, "NE2K transmitting a packet, length %u\n", io_len);
    Bit8u *charbuf = (Bit8u *)buf;
    for (n=0; n<io_len; n++) {
      if (((n % 16) == 0) && n>0)
	fprintf (class_ptr->rxlog_txt, "\n");
      fprintf (class_ptr->rxlog_txt, "%02x ", charbuf[n]);
    }
    fprintf (class_ptr->rxlog_txt, "\n--\n");
    // flush log so that we see the packets as they arrive w/o buffering
    fflush (class_ptr->rxlog);
    fflush (class_ptr->rxlog_txt);
  }
#endif
}
