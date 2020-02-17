/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003       by Mariusz Matuszek
//                              [NOmrmmSPAM @ users.sourceforge.net]
//  Copyright (C) 2017-2020  The Bochs Project
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

// Mariusz Matuszek wrote the original version of eth_socket.cc based on
// eth_linux.cc.
//
// problems and limitations:
//   - external program 'bxhub' is required
//   - sends big packets using UDP, so only localhost is likely to work
//   - author definitely doesn't know C++ and generally doesn't know what
//     he is doing :)
//
// the idea is to provide a software multiport 'ethernet hub' and allow
// communication between multiple bochs instances on the same machine
// entirely in userspace and without need for root privileges.
//
// The config line in .bochsrc should look like:
//
// ne2k: ioaddr=0x280, irq=10, mac=00:a:b:c:1:2, ethmod=socket, ethdev=<socknum>
//
// e.g.
//
// ne2k: ioaddr=0x280, irq=10, mac=00:a:b:c:1:2, ethmod=socket, ethdev=40000
//
// this module will bind to 127.0.0.1:<socknum> for RX packets
// TX packets will be sent to 127.0.0.1:<socknum + 1>

// Extensions by Volker Ruppert (2017):
// - Windows support
// - Support for connecting other machine using format 'host:port'.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#ifdef __CYGWIN__
#define __USE_W32_SOCKETS
#endif

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_SOCKET

// network driver plugin entry points

int CDECL libsocket_net_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL libsocket_net_plugin_fini(void)
{
  // Nothing here yet
}

// network driver implementation

#define LOG_THIS netdev->

extern "C" {
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#if defined(__CYGWIN__) && defined(_WIN64)
#undef FIONBIO
#define FIONBIO 0x8004667e
#endif
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <net/if.h>
#ifdef __linux__
#include <linux/types.h>
#endif
#include <netdb.h>
#define closesocket(s) close(s)
typedef int SOCKET;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#endif
};

#ifdef __APPLE__
#define MSG_NOSIGNAL 0
#endif

#ifdef WIN32
#define MSG_NOSIGNAL 0
#define MSG_DONTWAIT 0
#endif

#define BX_PACKET_POLL  1000    // Poll for a frame every 1000 usecs

//
//  Define the class. This is private to this module
//
class bx_socket_pktmover_c : public eth_pktmover_c {
public:
  bx_socket_pktmover_c(const char *netif, const char *macaddr,
                       eth_rx_handler_t rxh,
                       eth_rx_status_t rxstat,
                       bx_devmodel_c *dev, const char *script);
  virtual ~bx_socket_pktmover_c();

  void sendpkt(void *buf, unsigned io_len);

private:
  unsigned char *socket_macaddr[6];
  SOCKET fd;                               // socket we listen on
  struct sockaddr_in sin, sout;            // target address for RX / TX
  static void rx_timer_handler(void *);
  void rx_timer(void);
  int rx_timer_index;
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_socket_locator_c : public eth_locator_c {
public:
  bx_socket_locator_c(void) : eth_locator_c("socket") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           bx_devmodel_c *dev, const char *script) {
    return (new bx_socket_pktmover_c(netif, macaddr, rxh, rxstat, dev, script));
  }
} bx_socket_match;


//
// Define the methods for the bx_socket_pktmover derived class
//

// the constructor
//
bx_socket_pktmover_c::bx_socket_pktmover_c(const char *netif,
                                           const char *macaddr,
                                           eth_rx_handler_t rxh,
                                           eth_rx_status_t rxstat,
                                           bx_devmodel_c *dev,
                                           const char *script)
{
  struct hostent *hp;
  int port;
#ifdef WIN32
  ULONG nbl = 1;
#endif

  this->netdev = dev;
  BX_INFO(("socket network driver"));
  memcpy(socket_macaddr, macaddr, 6);
  this->fd = INVALID_SOCKET;

#ifdef WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  wVersionRequested = MAKEWORD(2, 0);
  err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    BX_PANIC(("WSAStartup failed"));
    return;
  }
#endif

  if (isalpha(netif[0])) {
    // Expecting format 'host:port', so split up 'netif' string.
    char *host = strdup(netif);
    char *substr = strtok(host, ":");
    substr = strtok(NULL, ":");
    if (!substr) {
      BX_PANIC(("eth_socket: inet address is wrong (%s)", netif));
      free(host);
      return;
    }
    hp = gethostbyname(host);
    if (!hp) {
      BX_PANIC(("eth_socket: gethostbyname failed (%s)", host));
      free(host);
      return;
    }
    free(host);
    port = atoi(substr);
    if (port == 0) {
      BX_PANIC(("eth_socket: could not translate socket number '%s'", substr));
      return;
    }
  } else {
    // Use localhost and translate 'netif' to port number.
    hp = gethostbyname("localhost");
    port = atoi(netif);
    if (port == 0) {
      BX_PANIC(("eth_socket: could not translate socket number '%s'", netif));
      return;
    }
  }

  // Open RX socket
  //
  if ((this->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
#ifndef WIN32
    this->fd = INVALID_SOCKET;
    if (errno == EACCES)
      BX_PANIC(("eth_socket: insufficient privileges to open socket"));
    else
      BX_PANIC(("eth_socket: could not open socket: %s", strerror(errno)));
#else
    BX_PANIC(("eth_socket: could not open socket: error=%d", WSAGetLastError()));
#endif
    return;
  }

  // Bind to given interface
  //
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    BX_PANIC(("eth_socket: could not bind to socket '%s' (%s)", netif, strerror(errno)));
    closesocket(fd);
    this->fd = INVALID_SOCKET;
    return;
  }

  // Set up non-blocking i/o
  //
#ifdef WIN32
  if (ioctlsocket(this->fd, FIONBIO, &nbl) != 0) {
#else
  if (fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
#endif
    BX_PANIC(("eth_socket: could not set non-blocking i/o on socket"));
    closesocket(this->fd);
    this->fd = INVALID_SOCKET;
    return;
  }

  // Set up destination address for TX ( sendto() )
  //
  sout.sin_family = AF_INET;
  sout.sin_port = htons(port+1); // set TX to RX + 1
  memcpy((char*) &(sout.sin_addr), hp->h_addr, hp->h_length);

  // Start the rx poll
  //
  this->rx_timer_index =
    DEV_register_timer(this, this->rx_timer_handler, BX_PACKET_POLL, 1, 1,
                       "eth_socket"); // continuous, active

  this->rxh    = rxh;
  this->rxstat = rxstat;
  BX_INFO(("socket network driver initialized: using socket '%s'", netif));
}


// the destructor
//
bx_socket_pktmover_c::~bx_socket_pktmover_c()
{
#ifdef WIN32
  WSACleanup();
#endif
}


// the output routine - called with pre-formatted ethernet frame.
void bx_socket_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  int status;

  if (this->fd != INVALID_SOCKET) {
    status = sendto(this->fd, (char*)buf, io_len,
                    (MSG_NOSIGNAL | MSG_DONTWAIT),
                    (struct sockaddr*) &sout, sizeof(sout));
    if (status == -1) {
      BX_INFO(("eth_socket: write failed: %s", strerror(errno)));
    }
  }
}


// The receive poll process
//
void bx_socket_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_socket_pktmover_c *class_ptr = (bx_socket_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void bx_socket_pktmover_c::rx_timer(void)
{
  int nbytes = 0;
  socklen_t slen = sizeof(sin);
  Bit8u rxbuf[BX_PACKET_BUFSIZE];

  // is socket open and bound?
  if (this->fd == INVALID_SOCKET)
    return;

  // receive packet
  nbytes = recvfrom(this->fd, (char*)rxbuf, sizeof(rxbuf), MSG_NOSIGNAL,
                    (struct sockaddr*) &sin, &slen);

  if (nbytes == -1) {
#ifdef WIN32
    if (WSAGetLastError() != WSAEWOULDBLOCK)
      BX_INFO(("eth_socket: error receiving packet: %d", WSAGetLastError()));
#else
    if (errno != EAGAIN)
      BX_INFO(("eth_socket: error receiving packet: %s", strerror(errno)));
#endif
    return;
  }

  // let through broadcast and our mac address
  if ((memcmp(rxbuf, this->socket_macaddr, 6) != 0) &&
      (memcmp(rxbuf, broadcast_macaddr, 6) != 0)) {
    return;
  }

  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    BX_DEBUG(("eth_socket: got packet: %d bytes, dst=%x:%x:%x:%x:%x:%x, src=%x:%x:%x:%x:%x:%x", nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
    this->rxh(this->netdev, rxbuf, nbytes);
  }
}
#endif /* if BX_NETWORKING && BX_NETMOD_SOCKET */
