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

// eth_fbsd.cc - A FreeBSD packet filter implementation of
// an ethernet pktmover. There are some problems and limitations
// with FreeBSD:
//   - the source address of the frame is overwritten by
//    the hosts's source address. This causes problems with
//    learning bridges - since they cannot determine where
//    BOCHS 'is', they broadcast the frame to all ports.
//   - packets cannot be sent from BOCHS to the host
//   - TCP performance seems to be abysmal; I think this is
//     a timing problem somewhere.
//   - I haven't handled the case where multiple frames arrive
//     in a single BPF read. 
//
//   The /dev/bpf* devices need to be set up with the appropriate
//  permissions for this to work.
//
//   The config line in .bochsrc should look something like:
//
//  ne2k: ioaddr=0x280, irq=9, mac=00:a:b:c:1:2, ethmod=fbsd, ethdev=fxp0
//

#include "bochs.h"
#define LOG_THIS this->

extern "C" {
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/bpf.h>
};

#define BX_BPF_POLL  1000    // Poll for a frame every 1000 usecs

#define BX_BPF_BUFSIZ 2048   // enough for an ether frame + bpf hdr

#define BX_BPF_INSNSIZ  8    // number of bpf insns

// template filter for a unicast mac address and all 
// multicast/broadcast frames
static const struct bpf_insn macfilter[] = {
  BPF_STMT(BPF_LD|BPF_W|BPF_ABS, 2),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0xaaaaaaaa, 0, 2),
  BPF_STMT(BPF_LD|BPF_H|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0x0000aaaa, 2, 0),
  BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JSET|BPF_K, 0x01, 0, 1),
  BPF_STMT(BPF_RET, 1514),
  BPF_STMT(BPF_RET, 0),
};

// template filter for all frames
static const struct bpf_insn promiscfilter[] = {
  BPF_STMT(BPF_RET, 1514)
};

//
//  Define the class. This is private to this module
//
class bx_fbsd_pktmover_c : public eth_pktmover_c {
public:
  bx_fbsd_pktmover_c(const char *netif, 
		     const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);

private:
  char *fbsd_macaddr[6];
  int bpf_fd;
  static void rx_timer_handler(void *);
  void rx_timer(void);
  int rx_timer_index;
  struct bpf_insn filter[BX_BPF_INSNSIZ];
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_fbsd_locator_c : public eth_locator_c {
public:
  bx_fbsd_locator_c(void) : eth_locator_c("fbsd") {}
protected:
  eth_pktmover_c *allocate(const char *netif, 
			   const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg) {
    return (new bx_fbsd_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_fbsd_match;


//
// Define the methods for the bx_fbsd_pktmover derived class
//

// the constructor
//
// Open a bpf file descriptor, and attempt to bind to
// the specified netif (Replicates libpcap open code)
//
bx_fbsd_pktmover_c::bx_fbsd_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
  char device[sizeof "/dev/bpf000"];
  int tmpfd;
  int n = 0;
  struct ifreq ifr;
  struct bpf_version bv;
  struct bpf_program bp;
  u_int v;

  memcpy(fbsd_macaddr, macaddr, 6);

  do {
    (void)sprintf(device, "/dev/bpf%d", n++);
    this->bpf_fd = open(device, O_RDWR);
  } while (this->bpf_fd < 0);
  
  if (this->bpf_fd < 0) {
    BX_INFO(("eth_freebsd: could not open packet filter"));
    return;
  }

  if (ioctl(this->bpf_fd, BIOCVERSION, (caddr_t)&bv) < 0) {
    BX_INFO(("eth_freebsd: could not retrieve bpf version"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }
  if (bv.bv_major != BPF_MAJOR_VERSION ||
      bv.bv_minor < BPF_MINOR_VERSION) {
    BX_INFO(("eth_freebsd: bpf version mismatch"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  // Set buffer size
  v = BX_BPF_BUFSIZ;
  if (ioctl(this->bpf_fd, BIOCSBLEN, (caddr_t)&v) < 0) {
    BX_INFO(("eth_freebsd: could not set buffer size"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  (void)strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));
  if (ioctl(this->bpf_fd, BIOCSETIF, (caddr_t)&ifr) < 0) {
    BX_INFO(("eth_freebsd: could not enable interface %s", netif));
    close(this->bpf_fd);
    this->bpf_fd == -1;
  }
  
  // Verify that the device is an ethernet.
  if (ioctl(this->bpf_fd, BIOCGDLT, (caddr_t)&v) < 0) {
    BX_INFO(("eth_freebsd: could not retrieve datalink type"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }
  if (v != DLT_EN10MB) {
    BX_INFO(("eth_freebsd: incorrect datalink type %d", v));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  // Put the device into promisc mode. This could be optimised
  // to filter on a MAC address, broadcast, and all-multi,
  // but this will do for now.
  //
  if (ioctl(this->bpf_fd, BIOCPROMISC, NULL) < 0) {
    BX_INFO(("eth_freebsd: could not enable promisc mode"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  // Set up non-blocking i/o
  v = 1;
  if (ioctl(this->bpf_fd, FIONBIO, &v) < 0) {
    BX_INFO(("eth_freebsd: could not enable non-blocking i/o"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  // Install a filter
#ifdef notdef
  memcpy(&this->filter, promiscfilter, sizeof(promiscfilter));
  bp.bf_len   = 1;
#endif
  memcpy(&this->filter, macfilter, sizeof(macfilter));
  this->filter[1].k = (macaddr[2] & 0xff) << 24 |
    (macaddr[3] & 0xff) << 16 |
    (macaddr[4] & 0xff) << 8  |
    (macaddr[5] & 0xff);
  this->filter[3].k = (macaddr[0] & 0xff) << 8 |
    (macaddr[1] & 0xff);
  bp.bf_len   = 8;
  bp.bf_insns = &this->filter[0];
  if (ioctl(this->bpf_fd, BIOCSETF, &bp) < 0) {
    BX_INFO(("eth_freebsd: could not set filter"));
    close(this->bpf_fd);
    this->bpf_fd = -1;
    return;
  }

  // Start the rx poll 
  this->rx_timer_index = 
    bx_pc_system.register_timer(this, this->rx_timer_handler, BX_BPF_POLL,
				1, 1); // continuous, active

  this->rxh   = rxh;
  this->rxarg = rxarg;
}

// the output routine - called with pre-formatted ethernet frame.
void
bx_fbsd_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  int status;

  if (this->bpf_fd != -1)
    status = write(this->bpf_fd, buf, io_len);
}

// The receive poll process
void
bx_fbsd_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_fbsd_pktmover_c *class_ptr = (bx_fbsd_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void
bx_fbsd_pktmover_c::rx_timer(void)
{
  int nbytes = 0;
  Bit8u rxbuf[BX_BPF_BUFSIZ]; 
  struct bpf_hdr *bhdr;

  nbytes = read(this->bpf_fd, rxbuf, sizeof(rxbuf));

  if (nbytes > 0) {    
    bhdr = (struct bpf_hdr *) rxbuf;     

    // filter out packets sourced from this node
    if (memcmp(rxbuf + bhdr->bh_hdrlen + 6, this->fbsd_macaddr, 6)) {
      (*rxh)(rxarg, rxbuf + bhdr->bh_hdrlen, bhdr->bh_caplen);
    }
  }  
}
