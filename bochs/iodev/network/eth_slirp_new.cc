/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014  The Bochs Project
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

// eth_slirp_new.cc  - Bochs port of Qemu's slirp implementation

#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP_NEW

#include "slirp/slirp.h"
#include "slirp/libslirp.h"

#define LOG_THIS netdev->

static int rx_timer_index = BX_NULL_TIMER_HANDLE;
static unsigned int bx_slirp_instances = 0;
fd_set rfds, wfds, xfds;
int nfds;

class bx_slirp_new_pktmover_c : public eth_pktmover_c {
public:
  bx_slirp_new_pktmover_c();
  bx_slirp_new_pktmover_c(const char *netif, const char *macaddr,
                         eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                         bx_devmodel_c *dev, const char *script);
  virtual ~bx_slirp_new_pktmover_c();
  void sendpkt(void *buf, unsigned io_len);
  void receive(void *pkt, unsigned pkt_len);
  int can_receive(void);
private:
  Slirp *slirp;
  unsigned netdev_speed;

  static void rx_timer_handler(void *);
};

class bx_slirp_new_locator_c : public eth_locator_c {
public:
  bx_slirp_new_locator_c(void) : eth_locator_c("slirp_new") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           bx_devmodel_c *dev, const char *script) {
    return (new bx_slirp_new_pktmover_c(netif, macaddr, rxh, rxstat, dev, script));
  }
} bx_slirp_new_match;


bx_slirp_new_pktmover_c::bx_slirp_new_pktmover_c()
{
  slirp = NULL;
}

bx_slirp_new_pktmover_c::~bx_slirp_new_pktmover_c()
{
  if (slirp != NULL) {
    slirp_cleanup(slirp);
    if (--bx_slirp_instances == 0) {
      bx_pc_system.deactivate_timer(rx_timer_index);
#ifndef WIN32
      signal(SIGPIPE, SIG_DFL);
#endif
    }
  }
}

bx_slirp_new_pktmover_c::bx_slirp_new_pktmover_c(const char *netif,
                                                 const char *macaddr,
                                                 eth_rx_handler_t rxh,
                                                 eth_rx_status_t rxstat,
                                                 bx_devmodel_c *dev,
                                                 const char *script)
{
  struct in_addr net, mask, host, dhcp, dns;

  this->netdev = dev;
  BX_INFO(("slirp_new network driver"));

  this->rxh   = rxh;
  this->rxstat = rxstat;
  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  if (bx_slirp_instances == 0) {
    rx_timer_index =
      bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
                                1, 1, "eth_slirp_new");
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
  }

  /* default settings according to historic slirp */
  net.s_addr  = htonl(0x0a000200); /* 10.0.2.0 */
  mask.s_addr = htonl(0xffffff00); /* 255.255.255.0 */
  host.s_addr = htonl(0x0a000202); /* 10.0.2.2 */
  dhcp.s_addr = htonl(0x0a00020f); /* 10.0.2.15 */
  dns.s_addr  = htonl(0x0a000203); /* 10.0.2.3 */
  slirp = slirp_init(0, net, mask, host, NULL, netif, NULL, dhcp, dns, this);

  bx_slirp_instances++;
}

void bx_slirp_new_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  slirp_input(slirp, (Bit8u*)buf, io_len);
}

void bx_slirp_new_pktmover_c::rx_timer_handler(void *this_ptr)
{
  Bit32u timeout = 0;
  int ret;
  struct timeval tv;

  nfds = -1;
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&xfds);
  slirp_select_fill(&nfds, &rfds, &wfds, &xfds, &timeout);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ret = select(nfds + 1, &rfds, &wfds, &xfds, &tv);
  slirp_select_poll(&rfds, &wfds, &xfds, (ret < 0));
}

int slirp_can_output(void *this_ptr)
{
  bx_slirp_new_pktmover_c *class_ptr = (bx_slirp_new_pktmover_c *)this_ptr;
  return class_ptr->can_receive();
}

int bx_slirp_new_pktmover_c::can_receive()
{
  return ((this->rxstat(this->netdev) & BX_NETDEV_RXREADY) != 0);
}

void slirp_output(void *this_ptr, const Bit8u *pkt, int pkt_len)
{
  bx_slirp_new_pktmover_c *class_ptr = (bx_slirp_new_pktmover_c *)this_ptr;
  class_ptr->receive((void*)pkt, pkt_len);
}

void bx_slirp_new_pktmover_c::receive(void *pkt, unsigned pkt_len)
{
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    if (pkt_len < 60) pkt_len = 60;
    this->rxh(this->netdev, pkt, pkt_len);
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

#endif /* if BX_NETWORKING && BX_NETMOD_SLIRP_NEW */
