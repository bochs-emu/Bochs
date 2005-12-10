/////////////////////////////////////////////////////////////////////////
// $Id: eth_vde.cc,v 1.11 2005-12-10 18:37:35 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003  Renzo Davoli
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

// eth_vde.cc  - Virtual Distributed Ethernet interface by Renzo Davoli <renzo@cs.unibo.it>
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE
 
#define NO_DEVICE_INCLUDES
#include "iodev.h"

#if BX_NETWORKING && defined(HAVE_VDE)

#include "eth.h"

#define LOG_THIS bx_devices.pluginNE2kDevice->

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
#include <sys/un.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#define SWITCH_MAGIC 0xfeedface

#define BX_ETH_VDE_LOGGING 0

int vde_alloc(char *dev,int *fdp,struct sockaddr_un *pdataout);

//
//  Define the class. This is private to this module
//
class bx_vde_pktmover_c : public eth_pktmover_c {
public:
  bx_vde_pktmover_c(const char *netif, const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg, char *script);
  void sendpkt(void *buf, unsigned io_len);
private:
  int fd;
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer ();
  FILE *txlog, *txlog_txt, *rxlog, *rxlog_txt;
  int fddata;
  struct sockaddr_un dataout;
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_vde_locator_c : public eth_locator_c {
public:
  bx_vde_locator_c(void) : eth_locator_c("vde") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg, char *script) {
    return (new bx_vde_pktmover_c(netif, macaddr, rxh, rxarg, script));
  }
} bx_vde_match;


//
// Define the methods for the bx_vde_pktmover derived class
//

// the constructor
bx_vde_pktmover_c::bx_vde_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg,
				       char *script)
{
  int flags;
  //if (strncmp (netif, "vde", 3) != 0) {
   // BX_PANIC (("eth_vde: interface name (%s) must be vde", netif));
  //}
  char intname[IFNAMSIZ];
  if (netif == NULL || strcmp(netif,"") == 0) 
	  strcpy(intname,"/tmp/vde.ctl");
  else
  	strcpy(intname,netif);
  fd=vde_alloc(intname,&fddata,&dataout);
  if (fd < 0) {
    BX_PANIC (("open failed on %s: %s", netif, strerror (errno)));
    return;
  }

  /* set O_ASYNC flag so that we can poll with read() */
  if ((flags = fcntl( fd, F_GETFL)) < 0) {
    BX_PANIC (("getflags on vde device: %s", strerror (errno)));
  }
  flags |= O_NONBLOCK;
  if (fcntl( fd, F_SETFL, flags ) < 0) {
    BX_PANIC (("set vde device flags: %s", strerror (errno)));
  }

  BX_INFO (("eth_vde: opened %s device", netif));

  /* Execute the configuration script */
  if((script != NULL)
   &&(strcmp(script, "") != 0)
   &&(strcmp(script, "none") != 0)) {
    if (execute_script(script, intname) < 0)
      BX_ERROR (("execute script '%s' on %s failed", script, intname));
    }

  // Start the rx poll 
  this->rx_timer_index = 
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
				1, 1, "eth_vde"); // continuous, active
  this->rxh   = rxh;
  this->rxarg = rxarg;
#if BX_ETH_VDE_LOGGING
  // eventually Bryce wants txlog to dump in pcap format so that
  // tcpdump -r FILE can read it and interpret packets.
  txlog = fopen ("ne2k-tx.log", "wb");
  if (!txlog) BX_PANIC (("open ne2k-tx.log failed"));
  txlog_txt = fopen ("ne2k-txdump.txt", "wb");
  if (!txlog_txt) BX_PANIC (("open ne2k-txdump.txt failed"));
  fprintf (txlog_txt, "vde packetmover readable log file\n");
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
  fprintf (rxlog_txt, "vde packetmover readable log file\n");
  fprintf (rxlog_txt, "net IF = %s\n", netif);
  fprintf (rxlog_txt, "MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (rxlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf (rxlog_txt, "\n--\n");
  fflush (rxlog_txt);

#endif
}

void
bx_vde_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  unsigned int size;
  //size = write (fd, buf, io_len);
  //size=send(fd,buf,io_len,0);
  size=sendto(fddata,buf,io_len,0,(struct sockaddr *) &dataout, sizeof(struct sockaddr_un));
  if (size != io_len) {
    BX_PANIC (("write on vde device: %s", strerror (errno)));
  } else {
    BX_INFO (("wrote %d bytes on vde", io_len));
  }
#if BX_ETH_VDE_LOGGING
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

void bx_vde_pktmover_c::rx_timer_handler (void *this_ptr)
{
  bx_vde_pktmover_c *class_ptr = (bx_vde_pktmover_c *) this_ptr;
  class_ptr->rx_timer();
}

void bx_vde_pktmover_c::rx_timer ()
{
  int nbytes;
  Bit8u buf[BX_PACKET_BUFSIZE];
  Bit8u *rxbuf;
  struct sockaddr_un datain;
  socklen_t datainsize;

  if (fd<0) return;
  //nbytes = read (fd, buf, sizeof(buf));
  nbytes=recvfrom(fddata,buf,sizeof(buf),MSG_DONTWAIT|MSG_WAITALL,(struct sockaddr *) &datain, &datainsize);

  rxbuf=buf;

  if (nbytes>0)
    BX_INFO (("vde read returned %d bytes", nbytes));
  if (nbytes<0) {
    if (errno != EAGAIN)
      BX_ERROR (("vde read error: %s", strerror(errno)));
    return;
  }
#if BX_ETH_VDE_LOGGING
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
  BX_DEBUG(("eth_vde: got packet: %d bytes, dst=%x:%x:%x:%x:%x:%x, src=%x:%x:%x:%x:%x:%x\n", nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
  if (nbytes < 60) {
    BX_INFO (("packet too short (%d), padding to 60", nbytes));
    nbytes = 60;
  }
  (*rxh)(rxarg, rxbuf, nbytes);
}

//enum request_type { REQ_NEW_CONTROL };
#define REQ_NEW_CONTROL 0

struct request_v3 {
	Bit32u magic;
	Bit32u version;
	//enum request_type type;
	int type;
	struct sockaddr_un sock;
};

static int send_fd(char *name, int fddata, struct sockaddr_un *datasock, int group)
{
	int pid = getpid();
	struct request_v3 req;
	int fdctl;
	struct sockaddr_un sock;
	if((fdctl = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("socket");
		return(-1);
	}
	sock.sun_family = AF_UNIX;
	snprintf(sock.sun_path, sizeof(sock.sun_path), "%s", name);
	if(connect(fdctl, (struct sockaddr *) &sock, sizeof(sock))){
		perror("connect");
		return(-1);
	}
	req.magic=SWITCH_MAGIC;
	req.version=3;
	req.type=((int)REQ_NEW_CONTROL)+((group > 0)?((geteuid()<<8) + group) << 8:0);
	req.sock.sun_family=AF_UNIX;
	memset(req.sock.sun_path, 0, sizeof(req.sock.sun_path));
	sprintf(&req.sock.sun_path[1], "%5d", pid);
	if(bind(fddata, (struct sockaddr *) &req.sock, sizeof(req.sock)) < 0){
		perror("bind");
		return(-1);
	}
	if (send(fdctl,&req,sizeof(req),0) < 0) {
		perror("send");
		return(-1);
	}
	if (recv(fdctl,datasock,sizeof(struct sockaddr_un),0)<0) {
		perror("recv");
		return(-1);
	}
	return fdctl;
}

int vde_alloc(char *dev, int *fdp, struct sockaddr_un *pdataout)
{
      //struct ifreq ifr;
      //int err;
      int fd, fddata;

      if((fddata = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){ 
	      return -1;
      }


      if( (fd = send_fd(dev, fddata, pdataout, 0)) < 0 )
         return -1;

      //memset(&ifr, 0, sizeof(ifr));
      *fdp=fddata;

      return fd;
}              

#endif /* if BX_NETWORKING && defined HAVE_VDE */
