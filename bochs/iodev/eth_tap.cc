/////////////////////////////////////////////////////////////////////////
// $Id: eth_tap.cc,v 1.1 2002-03-09 00:38:53 bdenney Exp $
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

// eth_tap.cc  - TAP interface
//
// Example configuration:
//   ifconfig tap0 10.0.0.123 up
//   route add -host 10.0.0.124 gw 10.0.0.123
// Testing:
//   in one window, cat /dev/tap0
//   in another window, ping 10.0.0.124
//   you should see data coming out of /dev/tap0 with each ping.

#include "bochs.h"
#define LOG_THIS bx_ne2k.

#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#define TAP_VIRTUAL_HW_ADDR             0xDEADBEEF
#define BX_ETH_TAP_LOGGING 1
#define BX_PACKET_BUFSIZ 2048	// Enough for an ether frame

extern int check_netdev( char *ifname );
extern int tap_open( const char *intf_name);

//
//  Define the class. This is private to this module
//
class bx_tap_pktmover_c : public eth_pktmover_c {
public:
  bx_tap_pktmover_c(const char *netif, const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
private:
  int fd;
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer ();
  FILE *txlog, *txlog_txt, *rxlog, *rxlog_txt;
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_tap_locator_c : public eth_locator_c {
public:
  bx_tap_locator_c(void) : eth_locator_c("tap") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg) {
    return (new bx_tap_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_tap_match;


//
// Define the methods for the bx_tap_pktmover derived class
//

// the constructor
bx_tap_pktmover_c::bx_tap_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
  int flags;
  fd = tap_open (netif);
  if (fd < 0) {
    BX_PANIC (("tap_open failed on %s", netif));
    return;
  }

/* setup O_ASYNC, O_NONBLOCK */
if( (flags = fcntl( fd, F_GETFL)) == -1 ) {
  BX_PANIC (("getflags on tap device failed"));
}
flags |= O_NONBLOCK;
if( fcntl( fd, F_SETFL, flags ) == -1 ) {
  BX_PANIC (("could not set tap device flags"));
}

BX_INFO (("eth_tap: opened %s device", netif));

#if BX_ETH_TAP_LOGGING
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
  fprintf (txlog_txt, "tap packetmover readable log file\n");
  fprintf (txlog_txt, "net IF = %s\n", netif);
  fprintf (txlog_txt, "MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (txlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf (txlog_txt, "\n--\n");
  fflush (txlog_txt);

  rxlog = fopen ("ne2k-rx.log", "wb");
  if (!rxlog) BX_PANIC (("open ne2k-rx.log failed"));
  rxlog_txt = fopen ("ne2k-rxdump.txt", "wb");
  if (!rxlog_txt) BX_PANIC (("open ne2k-rxdump.txt failed"));
  fprintf (rxlog_txt, "tap packetmover readable log file\n");
  fprintf (rxlog_txt, "net IF = %s\n", netif);
  fprintf (rxlog_txt, "MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (rxlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf (rxlog_txt, "\n--\n");
  fflush (rxlog_txt);

#endif
}

void
bx_tap_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  Bit8u txbuf[BX_PACKET_BUFSIZ];
  txbuf[0] = 0;
  txbuf[1] = 0;
  memcpy (txbuf+2, buf, io_len);
  unsigned int size = write (fd, txbuf, io_len+2);
  if (size != io_len+2) {
    perror("write");
    BX_PANIC (("write on tap device failed"));
  } else {
    BX_INFO (("wrote %d bytes + 2 byte pad on tap", io_len));
  }
#if BX_ETH_TAP_LOGGING
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

void bx_tap_pktmover_c::rx_timer_handler (void *this_ptr)
{
  bx_tap_pktmover_c *class_ptr = (bx_tap_pktmover_c *) this_ptr;
  class_ptr->rx_timer();
}

void bx_tap_pktmover_c::rx_timer ()
{
  int nbytes;
  Bit8u buf[BX_PACKET_BUFSIZ];
  Bit8u *rxbuf;
  if (fd<0) return;
  nbytes = read (fd, buf, sizeof(buf));

  // hack: discard first two bytes
  rxbuf = buf+2;
  nbytes-=2;
  
  // hack: TAP device likes to create an ethernet header which has
  // the same source and destination address FE:FD:00:00:00:00.
  // Change the dest address to FE:FD:00:00:00:01.
  rxbuf[5] = 1;

  if (nbytes>0)
    BX_INFO (("tap read returned %d bytes", nbytes));
  if (nbytes<0) {
    if (errno != EAGAIN)
      BX_ERROR (("tap read error: %s", strerror(errno)));
    return;
  }
#if BX_ETH_TAP_LOGGING
  if (nbytes > 0) {
    BX_DEBUG (("receive packet length %u", nbytes));
    // dump raw bytes to a file, eventually dump in pcap format so that
    // tcpdump -r FILE can interpret them for us.
    int n = fwrite (rxbuf, nbytes, 1, rxlog);
    if (n != 1) BX_ERROR (("fwrite to rxlog failed", nbytes));
    // dump packet in hex into an ascii log file
    fprintf (rxlog_txt, "NE2K received a packet, length %u\n", nbytes);
    for (n=0; n<nbytes; n++) {
      if (((n % 16) == 0) && n>0)
	fprintf (rxlog_txt, "\n");
      fprintf (rxlog_txt, "%02x ", rxbuf[n]);
    }
    fprintf (rxlog_txt, "\n--\n");
    // flush log so that we see the packets as they arrive w/o buffering
    fflush (rxlog);
    fflush (rxlog_txt);
  }
#endif
  BX_DEBUG(("eth_tap: got packet: %d bytes, dst=%x:%x:%x:%x:%x:%x, src=%x:%x:%x:%x:%x:%x\n", nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
  if (nbytes < 60) {
    BX_INFO (("packet too short (%d), padding to 60", nbytes));
    nbytes = 60;
  }
  (*rxh)(rxarg, rxbuf, nbytes);
}


// still some dependence on GPL code from Mac-On-Linux.
// I will rewrite these functions from scratch.
#include "eth_tap_gpl.cc"
