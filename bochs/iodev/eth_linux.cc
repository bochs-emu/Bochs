/////////////////////////////////////////////////////////////////////////
// $Id: eth_linux.cc,v 1.3 2001-10-03 13:10:38 bdenney Exp $
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

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

// eth_linux.cc - A Linux socket filter adaptation of the FreeBSD BPF driver
// <splite@purdue.edu> 21 June 2001
//
// Problems and limitations:
//   - packets cannot be sent from BOCHS to the host
//   - Linux kernel sometimes gets network watchdog timeouts under emulation
//   - author doesn't know C++
//
//   The config line in .bochsrc should look something like:
//
//  ne2k: ioaddr=0x280, irq=10, mac=00:a:b:c:1:2, ethmod=linux, ethdev=eth0
//

#include "bochs.h"
#ifdef ETH_LINUX
#define LOG_THIS this->

extern "C" {
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <linux/types.h>
#include <linux/filter.h>
};

#define BX_PACKET_POLL  1000    // Poll for a frame every 1000 usecs

#define BX_PACKET_BUFSIZ 2048	// Enough for an ether frame

// template filter for a unicast mac address and all
// multicast/broadcast frames
static const struct sock_filter macfilter[] = {
  BPF_STMT(BPF_LD|BPF_W|BPF_ABS, 2),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0xaaaaaaaa, 0, 2),
  BPF_STMT(BPF_LD|BPF_H|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0x0000aaaa, 2, 0),
  BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JSET|BPF_K, 0x01, 0, 1),
  BPF_STMT(BPF_RET, 1514),
  BPF_STMT(BPF_RET, 0),
};
#define BX_LSF_ICNT 8    // number of lsf instructions in macfilter

#if 0
// template filter for all frames
static const struct sock_filter promiscfilter[] = {
  BPF_STMT(BPF_RET, 1514)
};
#endif

//
//  Define the class. This is private to this module
//
class bx_linux_pktmover_c : public eth_pktmover_c {
public:
  bx_linux_pktmover_c(const char *netif, 
		     const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);

private:
  unsigned char *linux_macaddr[6];
  int fd;
  int ifindex;
  static void rx_timer_handler(void *);
  void rx_timer(void);
  int rx_timer_index;
  struct sock_filter filter[BX_LSF_ICNT];
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_linux_locator_c : public eth_locator_c {
public:
  bx_linux_locator_c(void) : eth_locator_c("linux") {}
protected:
  eth_pktmover_c *allocate(const char *netif, 
			   const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg) {
    return (new bx_linux_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_linux_match;


//
// Define the methods for the bx_linux_pktmover derived class
//

// the constructor
//
bx_linux_pktmover_c::bx_linux_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
  struct sockaddr_ll sll;
  struct packet_mreq mr;
  struct ifreq ifr;
  struct sock_fprog fp;

  memcpy(linux_macaddr, macaddr, 6);

  // Open packet socket
  //
  if ((this->fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
    if (errno == EACCES)
      BX_INFO(("eth_linux: must be root or have CAP_NET_RAW capability to open socket"));
    else
      BX_INFO(("eth_linux: could not open socket"));
    this->fd = -1;
    return;
  }
  
  // Translate interface name to index
  //
  memset(&ifr, 0, sizeof(ifr));
  strcpy(ifr.ifr_name, netif);
  if (ioctl(this->fd, SIOCGIFINDEX, &ifr) == -1) {
    BX_INFO(("eth_linux: could not get index for interface %s\n", netif));
    close(fd);
    this->fd = -1;
    return;
  }
  this->ifindex = ifr.ifr_ifindex;


  // Bind to given interface
  //
  memset(&sll, 0, sizeof(sll));
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = this->ifindex;
  if (bind(fd, (struct sockaddr *)&sll, (socklen_t)sizeof(sll)) == -1) {
    BX_INFO(("eth_linux: could not bind to interface %s\n", netif));
    close(fd);
    this->fd = -1;
    return;
  }

  // Put the device into promisc mode.
  //
  memset(&mr, 0, sizeof(mr));
  mr.mr_ifindex = this->ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  if (setsockopt(this->fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, (void *)&mr, (socklen_t)sizeof(mr)) == -1) {
    BX_INFO(("eth_linux: could not enable promisc mode: %s\n", strerror(errno)));
    close(this->fd);
    this->fd = -1;
    return;
  }

  // Set up non-blocking i/o
  if (fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
    BX_INFO(("eth_linux: could not set non-blocking i/o on socket"));
    close(this->fd);
    this->fd = -1;
    return;
  }

  // Install a filter
#ifdef notdef
  memcpy(&this->filter, promiscfilter, sizeof(promiscfilter));
  fp.len   = 1;
#endif
  memcpy(&this->filter, macfilter, sizeof(macfilter));
  this->filter[1].k = (macaddr[2] & 0xff) << 24 | (macaddr[3] & 0xff) << 16 |
    (macaddr[4] & 0xff) << 8  | (macaddr[5] & 0xff);
  this->filter[3].k = (macaddr[0] & 0xff) << 8 | (macaddr[1] & 0xff);
  fp.len = BX_LSF_ICNT;
  fp.filter = this->filter;
BX_INFO(("eth_linux: fp.len=%d fp.filter=%lx", fp.len, fp.filter));
  if (setsockopt(this->fd, SOL_SOCKET, SO_ATTACH_FILTER, &fp, sizeof(fp)) < 0) {
    BX_INFO(("eth_linux: could not set socket filter: %s", strerror(errno)));
    close(this->fd);
    this->fd = -1;
    return;
  }

  // Start the rx poll 
  this->rx_timer_index = 
    bx_pc_system.register_timer(this, this->rx_timer_handler, BX_PACKET_POLL,
				1, 1); // continuous, active

  this->rxh   = rxh;
  this->rxarg = rxarg;
  BX_INFO(("eth_linux: enabled NE2K emulation on interface %s\n", netif));
}

// the output routine - called with pre-formatted ethernet frame.
void
bx_linux_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  int status;

  if (this->fd != -1)
    status = write(this->fd, buf, io_len);
  if (status == -1)
    BX_INFO(("eth_linux: write failed: %s", strerror(errno)));
}

// The receive poll process
void
bx_linux_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_linux_pktmover_c *class_ptr = (bx_linux_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void
bx_linux_pktmover_c::rx_timer(void)
{
  int nbytes = 0;
  Bit8u rxbuf[BX_PACKET_BUFSIZ]; 
  struct sockaddr_ll sll;
  socklen_t fromlen;
  static unsigned char bcast_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

  if (this->fd == -1)
    return;

  fromlen = sizeof(sll);
  nbytes = recvfrom(this->fd, rxbuf, sizeof(rxbuf), 0, (struct sockaddr *)&sll, &fromlen);

  if (nbytes == -1) {
    if (errno != EAGAIN)
      BX_INFO(("eth_linux: error receiving packet: %s\n", strerror(errno)));
    return;
  }

  // this should be done with LSF someday
  // filter out packets sourced by us
  if (memcmp(sll.sll_addr, this->linux_macaddr, 6) == 0)
    return;
  // let through broadcast, multicast, and our mac address
//  if ((memcmp(rxbuf, bcast_addr, 6) == 0) || (memcmp(rxbuf, this->linux_macaddr, 6) == 0) || rxbuf[0] & 0x01) {
    BX_DEBUG(("eth_linux: got packet: %d bytes, dst=%x:%x:%x:%x:%x:%x, src=%x:%x:%x:%x:%x:%x\n", nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
    (*rxh)(rxarg, rxbuf, nbytes);
//  }
}
#endif
