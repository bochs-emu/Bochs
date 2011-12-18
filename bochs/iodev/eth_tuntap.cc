/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2011  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// eth_tuntap.cc  - TUN/TAP interface by Renzo Davoli <renzo@cs.unibo.it>

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_TUNTAP

#define LOG_THIS netdev->

#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#ifndef __APPLE__
#include <sys/poll.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>
#ifdef __linux__
#include <asm/types.h>
#else
#include <sys/types.h>
#endif
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#ifdef __linux__
#include <linux/netlink.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#else
#include <net/if.h>
#if !defined(__APPLE__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
#include <net/if_tap.h>
#endif
#endif
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#define BX_ETH_TUNTAP_LOGGING 0

int tun_alloc(char *dev);

//
//  Define the class. This is private to this module
//
class bx_tuntap_pktmover_c : public eth_pktmover_c {
public:
  bx_tuntap_pktmover_c(const char *netif, const char *macaddr,
                       eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                       bx_devmodel_c *dev, const char *script);
  void sendpkt(void *buf, unsigned io_len);
private:
  int fd;
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer ();
  Bit8u guest_macaddr[6];
#if BX_ETH_TUNTAP_LOGGING
  FILE *txlog, *txlog_txt, *rxlog, *rxlog_txt;
#endif
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_tuntap_locator_c : public eth_locator_c {
public:
  bx_tuntap_locator_c(void) : eth_locator_c("tuntap") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           bx_devmodel_c *dev, const char *script) {
    return (new bx_tuntap_pktmover_c(netif, macaddr, rxh, rxstat, dev, script));
  }
} bx_tuntap_match;


//
// Define the methods for the bx_tuntap_pktmover derived class
//

// the constructor
bx_tuntap_pktmover_c::bx_tuntap_pktmover_c(const char *netif,
                                           const char *macaddr,
                                           eth_rx_handler_t rxh,
                                           eth_rx_status_t rxstat,
                                           bx_devmodel_c *dev,
                                           const char *script)
{
  int flags;

  this->netdev = dev;
#ifdef NEVERDEF
  if (strncmp (netif, "tun", 3) != 0) {
    BX_PANIC(("eth_tuntap: interface name (%s) must be tun", netif));
  }
  char filename[BX_PATHNAME_LEN];
  sprintf(filename, "/dev/net/%s", netif);

  // check if the TUN/TAP devices is running, and turn on ARP.  This is based
  // on code from the Mac-On-Linux project. http://http://www.maconlinux.org/
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    BX_PANIC(("socket creation: %s", strerror(errno)));
    return;
  }
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));
  if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
    BX_PANIC(("SIOCGIFFLAGS on %s: %s", netif, strerror (errno)));
    close(sock);
    return;
  }
  if (!(ifr.ifr_flags & IFF_RUNNING)) {
    BX_PANIC(("%s device is not running", netif));
    close(sock);
    return;
  }
  if ((ifr.ifr_flags & IFF_NOARP)) {
    BX_INFO(("turn on ARP for %s device", netif));
    ifr.ifr_flags &= ~IFF_NOARP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
      BX_PANIC(("SIOCSIFFLAGS: %s", strerror(errno)));
      close(sock);
      return;
    }
  }
  close(sock);

  fd = open (filename, O_RDWR);
#endif
  char intname[IFNAMSIZ];
  strcpy(intname,netif);
  fd=tun_alloc(intname);
  if (fd < 0) {
    BX_PANIC(("open failed on %s: %s", netif, strerror (errno)));
    return;
  }

  /* set O_ASYNC flag so that we can poll with read() */
  if ((flags = fcntl(fd, F_GETFL)) < 0) {
    BX_PANIC(("getflags on tun device: %s", strerror (errno)));
  }
  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) < 0) {
    BX_PANIC(("set tun device flags: %s", strerror (errno)));
  }

  BX_INFO(("tuntap network driver: opened %s device", netif));

  /* Execute the configuration script */
  if((script != NULL) && (strcmp(script, "") != 0) && (strcmp(script, "none") != 0))
  {
    if (execute_script(this->netdev, script, intname) < 0)
      BX_ERROR(("execute script '%s' on %s failed", script, intname));
  }

  // Start the rx poll
  this->rx_timer_index =
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
                                1, 1, "eth_tuntap"); // continuous, active
  this->rxh    = rxh;
  this->rxstat = rxstat;
  memcpy(&guest_macaddr[0], macaddr, 6);
#if BX_ETH_TUNTAP_LOGGING
  // eventually Bryce wants txlog to dump in pcap format so that
  // tcpdump -r FILE can read it and interpret packets.
  txlog = fopen("ne2k-tx.log", "wb");
  if (!txlog) BX_PANIC(("open ne2k-tx.log failed"));
  txlog_txt = fopen("ne2k-txdump.txt", "wb");
  if (!txlog_txt) BX_PANIC(("open ne2k-txdump.txt failed"));
  fprintf(txlog_txt, "tuntap packetmover readable log file\n");
  fprintf(txlog_txt, "net IF = %s\n", netif);
  fprintf(txlog_txt, "MAC address = ");
  for (int i=0; i<6; i++)
    fprintf(txlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf(txlog_txt, "\n--\n");
  fflush(txlog_txt);

  rxlog = fopen("ne2k-rx.log", "wb");
  if (!rxlog) BX_PANIC(("open ne2k-rx.log failed"));
  rxlog_txt = fopen("ne2k-rxdump.txt", "wb");
  if (!rxlog_txt) BX_PANIC(("open ne2k-rxdump.txt failed"));
  fprintf(rxlog_txt, "tuntap packetmover readable log file\n");
  fprintf(rxlog_txt, "net IF = %s\n", netif);
  fprintf(rxlog_txt, "MAC address = ");
  for (int i=0; i<6; i++)
    fprintf(rxlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf(rxlog_txt, "\n--\n");
  fflush(rxlog_txt);

#endif
}

void bx_tuntap_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
#ifdef __APPLE__ //FIXME
  unsigned int size = write (fd, (char*)buf+14, io_len-14);
  if (size != io_len-14) {
    BX_PANIC(("write on tuntap device: %s", strerror (errno)));
  } else {
    BX_DEBUG(("wrote %d bytes on tuntap - 14 bytes Ethernet header", io_len));
  }
#elif NEVERDEF
  Bit8u txbuf[BX_PACKET_BUFSIZE];
  txbuf[0] = 0;
  txbuf[1] = 0;
  memcpy (txbuf+2, buf, io_len);
  unsigned int size = write (fd, txbuf, io_len+2);
  if (size != io_len+2) {
    BX_PANIC(("write on tuntap device: %s", strerror (errno)));
  } else {
    BX_DEBUG(("wrote %d bytes + 2 byte pad on tuntap", io_len));
  }
#else
  unsigned int size = write (fd, buf, io_len);
  if (size != io_len) {
    BX_PANIC(("write on tuntap device: %s", strerror (errno)));
  } else {
    BX_DEBUG(("wrote %d bytes on tuntap", io_len));
  }
#endif
#if BX_ETH_TUNTAP_LOGGING
  BX_DEBUG(("sendpkt length %u", io_len));
  // dump raw bytes to a file, eventually dump in pcap format so that
  // tcpdump -r FILE can interpret them for us.
  int n = fwrite(buf, io_len, 1, txlog);
  if (n != 1) BX_ERROR(("fwrite to txlog failed"));
  // dump packet in hex into an ascii log file
  write_pktlog_txt(txlog_txt, (const Bit8u *)buf, io_len, 0);
  // flush log so that we see the packets as they arrive w/o buffering
  fflush(txlog);
#endif
}

void bx_tuntap_pktmover_c::rx_timer_handler (void *this_ptr)
{
  bx_tuntap_pktmover_c *class_ptr = (bx_tuntap_pktmover_c *) this_ptr;
  class_ptr->rx_timer();
}

void bx_tuntap_pktmover_c::rx_timer()
{
  int nbytes;
  Bit8u buf[BX_PACKET_BUFSIZE];
  Bit8u *rxbuf;
  if (fd<0) return;

#ifdef __APPLE__ //FIXME:hack
  nbytes = 14;
  bzero(buf, nbytes);
  buf[0] = buf[6] = 0xFE;
  buf[1] = buf[7] = 0xFD;
  buf[12] = 8;
  nbytes += read (fd, buf+nbytes, sizeof(buf)-nbytes);
  rxbuf=buf;
#elif NEVERDEF
  nbytes = read (fd, buf, sizeof(buf));
  // hack: discard first two bytes
  rxbuf = buf+2;
  nbytes-=2;
#else
  nbytes = read (fd, buf, sizeof(buf));
  rxbuf=buf;
#endif

  // hack: TUN/TAP device likes to create an ethernet header which has
  // the same source and destination address FE:FD:00:00:00:00.
  // Change the dest address to FE:FD:00:00:00:01.
  if (!memcmp(&rxbuf[0], &rxbuf[6], 6)) {
    rxbuf[5] = guest_macaddr[5];
  }

#ifdef __APPLE__ //FIXME:hack
  if (nbytes>14)
#else
  if (nbytes>0)
#endif
    BX_DEBUG(("tuntap read returned %d bytes", nbytes));
#ifdef __APPLE__ //FIXME:hack
  if (nbytes<14) {
#else
  if (nbytes<0) {
#endif
    if (errno != EAGAIN)
      BX_ERROR(("tuntap read error: %s", strerror(errno)));
    return;
  }
#if BX_ETH_TUNTAP_LOGGING
  if (nbytes > 0) {
    BX_DEBUG(("receive packet length %u", nbytes));
    // dump raw bytes to a file, eventually dump in pcap format so that
    // tcpdump -r FILE can interpret them for us.
    int n = fwrite(rxbuf, nbytes, 1, rxlog);
    if (n != 1) BX_ERROR (("fwrite to rxlog failed"));
    // dump packet in hex into an ascii log file
    write_pktlog_txt(rxlog_txt, rxbuf, nbytes, 1);
    // flush log so that we see the packets as they arrive w/o buffering
    fflush(rxlog);
  }
#endif
  BX_DEBUG(("eth_tuntap: got packet: %d bytes, dst=%02x:%02x:%02x:%02x:%02x:%02x, src=%02x:%02x:%02x:%02x:%02x:%02x", nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
  if (nbytes < 60) {
    BX_INFO(("packet too short (%d), padding to 60", nbytes));
    nbytes = 60;
  }
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    this->rxh(this->netdev, rxbuf, nbytes);
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

int tun_alloc(char *dev)
{
  struct ifreq ifr;
  char *ifname;
  int fd, err;

  // split name into device:ifname if applicable, to allow for opening
  // persistent tuntap devices
  for (ifname = dev; *ifname; ifname++) {
    if (*ifname == ':') {
       *(ifname++) = '\0';
        break;
    }
  }
  if ((fd = open(dev, O_RDWR)) < 0)
    return -1;
#ifdef __linux__
  memset(&ifr, 0, sizeof(ifr));

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
   *        IFF_TAP   - TAP device
   *
   *        IFF_NO_PI - Do not provide packet information
   */
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
  if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
    close(fd);
    return err;
  }
  strncpy(dev, ifr.ifr_name, IFNAMSIZ);
  dev[IFNAMSIZ-1]=0;

  ioctl(fd, TUNSETNOCSUM, 1);
#endif

  return fd;
}

#endif /* if BX_NETWORKING && BX_NETMOD_TUNTAP */
