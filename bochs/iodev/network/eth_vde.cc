/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003       Renzo Davoli
//  Copyright (C) 2003-2026  The Bochs Project
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

// eth_vde.cc  - Virtual Distributed Ethernet interface by Renzo Davoli <renzo@cs.unibo.it>
//
// If libvdeplug is available at build time (BX_HAVE_LIBVDEPLUG), it is used
// to connect to the switch. Otherwise a builtin implementation of the
// vde_switch control protocol (version 3) is used.
//
// The 'ethdev' parameter is the vde_switch socket directory (the argument of
// 'vde_switch -s'). The path of the 'ctl' socket inside that directory is
// accepted as well.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "plugin.h"
#include "pc_system.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_VDE

// network driver plugin entry point

PLUGIN_ENTRY_FOR_NET_MODULE(vde)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_NET;
  }
  return 0; // Success
}

// network driver implementation

#define LOG_THIS netdev->

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#if BX_HAVE_LIBVDEPLUG
extern "C" {
#include <libvdeplug.h>
}
#endif

#define BX_ETH_VDE_LOGGING 0

#define BX_VDE_DEFAULT_SWITCH "/tmp/vde.ctl"
#define BX_VDE_PATH_LEN       sizeof(((struct sockaddr_un *)0)->sun_path)

//
//  Define the class. This is private to this module
//
class bx_vde_pktmover_c : public eth_pktmover_c {
public:
  bx_vde_pktmover_c(const char *netif, const char *macaddr,
                    eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                    logfunctions *netdev, const char *script);
  virtual ~bx_vde_pktmover_c();
  void sendpkt(void *buf, unsigned io_len);
private:
  bool vde_connect(const char *sw);
  void vde_disconnect();
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer();
#if BX_ETH_VDE_LOGGING
  FILE *txlog, *txlog_txt, *rxlog, *rxlog_txt;
#endif
  int fddata;
#if BX_HAVE_LIBVDEPLUG
  VDECONN *conn;
#else
  int fdctl;
  struct sockaddr_un dataout;
  struct sockaddr_un datain;
#endif
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
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           logfunctions *netdev, const char *script) {
    return (new bx_vde_pktmover_c(netif, macaddr, rxh, rxstat, netdev, script));
  }
} bx_vde_match;


//
// Define the methods for the bx_vde_pktmover derived class
//

// the constructor
bx_vde_pktmover_c::bx_vde_pktmover_c(const char *netif,
                                     const char *macaddr,
                                     eth_rx_handler_t rxh,
                                     eth_rx_status_t rxstat,
                                     logfunctions *netdev,
                                     const char *script)
{
  char swname[BX_PATHNAME_LEN];
  struct stat st;
  size_t len;
  int flags;

  this->netdev = netdev;
  fddata = -1;
#if BX_HAVE_LIBVDEPLUG
  conn = NULL;
#else
  fdctl = -1;
  memset(&datain, 0, sizeof(datain));
#endif

  if ((netif == NULL) || (strcmp(netif, "") == 0)) {
    netif = BX_VDE_DEFAULT_SWITCH;
  }
  len = strlen(netif);
  if (len >= sizeof(swname)) {
    BX_PANIC(("eth_vde: switch path too long: %s", netif));
    return;
  }
  strcpy(swname, netif);
  // accept the path of the switch control socket instead of its directory
  if ((len > 4) && (strcmp(&swname[len - 4], "/ctl") == 0) &&
      (stat(swname, &st) == 0) && S_ISSOCK(st.st_mode)) {
    swname[len - 4] = '\0';
  }

  if (!vde_connect(swname)) {
    BX_PANIC(("eth_vde: cannot connect to vde_switch at %s: %s", swname, strerror(errno)));
    return;
  }

  // the rx timer must not block on the data socket
  if (((flags = fcntl(fddata, F_GETFL)) < 0) ||
      (fcntl(fddata, F_SETFL, flags | O_NONBLOCK) < 0)) {
    BX_PANIC(("eth_vde: cannot set vde data socket non-blocking: %s", strerror(errno)));
  }

  BX_INFO(("eth_vde: connected to vde_switch at %s", swname));

  /* Execute the configuration script */
  if((script != NULL) && (strcmp(script, "") != 0) && (strcmp(script, "none") != 0))
  {
    if (execute_script(this->netdev, script, swname) < 0)
      BX_ERROR(("execute script '%s' on %s failed", script, swname));
  }

  // Start the rx poll
  this->rx_timer_index =
    DEV_register_timer(this, this->rx_timer_handler, 1000, 1, 1,
                       "eth_vde"); // continuous, active
  this->rxh    = rxh;
  this->rxstat = rxstat;
#if BX_ETH_VDE_LOGGING
  // eventually Bryce wants txlog to dump in pcap format so that
  // tcpdump -r FILE can read it and interpret packets.
  txlog = fopen("eth_vde-tx.log", "wb");
  if (!txlog) BX_PANIC(("open eth_vde-tx.log failed"));
  txlog_txt = fopen("eth_vde-txdump.txt", "wb");
  if (!txlog_txt) BX_PANIC(("open eth_vde-txdump.txt failed"));
  fprintf(txlog_txt, "vde packetmover readable log file\n");
  fprintf(txlog_txt, "net IF = %s\n", netif);
  fprintf(txlog_txt, "MAC address = ");
  for (int i=0; i<6; i++)
    fprintf(txlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf(txlog_txt, "\n--\n");
  fflush(txlog_txt);

  rxlog = fopen("eth_vde-rx.log", "wb");
  if (!rxlog) BX_PANIC(("open eth_vde-rx.log failed"));
  rxlog_txt = fopen("eth_vde-rxdump.txt", "wb");
  if (!rxlog_txt) BX_PANIC(("open eth_vde-rxdump.txt failed"));
  fprintf(rxlog_txt, "vde packetmover readable log file\n");
  fprintf(rxlog_txt, "net IF = %s\n", netif);
  fprintf(rxlog_txt, "MAC address = ");
  for (int i=0; i<6; i++)
    fprintf(rxlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf(rxlog_txt, "\n--\n");
  fflush(rxlog_txt);

#endif
}

bx_vde_pktmover_c::~bx_vde_pktmover_c()
{
  vde_disconnect();
#if BX_ETH_VDE_LOGGING
  fclose(txlog);
  fclose(txlog_txt);
  fclose(rxlog);
  fclose(rxlog_txt);
#endif
}

void bx_vde_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  ssize_t size;

  if (fddata < 0) return;
#if BX_HAVE_LIBVDEPLUG
  size = vde_send(conn, buf, io_len, 0);
#else
  size = sendto(fddata, buf, io_len, 0, (struct sockaddr *) &dataout, sizeof(dataout));
#endif
  if (size != (ssize_t)io_len) {
    BX_ERROR(("write on vde device: %s", strerror(errno)));
  } else {
    BX_DEBUG(("wrote %d bytes on vde", io_len));
  }
#if BX_ETH_VDE_LOGGING
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

void bx_vde_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_vde_pktmover_c *class_ptr = (bx_vde_pktmover_c *) this_ptr;
  class_ptr->rx_timer();
}

void bx_vde_pktmover_c::rx_timer()
{
  ssize_t nbytes;
  Bit8u buf[BX_PACKET_BUFSIZE];
  Bit8u *rxbuf;

  if (fddata < 0) return;
#if BX_HAVE_LIBVDEPLUG
  nbytes = vde_recv(conn, buf, sizeof(buf), 0);
#else
  nbytes = recv(fddata, buf, sizeof(buf), 0);
#endif

  rxbuf=buf;

  if (nbytes < 0) {
    if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
      BX_ERROR(("vde read error: %s", strerror(errno)));
    return;
  }
  // libvdeplug may return a 1 byte packet for internal messages
  if (nbytes < 14) {
    return;
  }
  BX_DEBUG(("vde read returned %d bytes", (int)nbytes));
#if BX_ETH_VDE_LOGGING
  BX_DEBUG(("receive packet length %u", (unsigned)nbytes));
  // dump raw bytes to a file, eventually dump in pcap format so that
  // tcpdump -r FILE can interpret them for us.
  int n = fwrite(rxbuf, nbytes, 1, rxlog);
  if (n != 1) BX_ERROR(("fwrite to rxlog failed"));
  // dump packet in hex into an ascii log file
  write_pktlog_txt(rxlog_txt, rxbuf, nbytes, 1);

  // flush log so that we see the packets as they arrive w/o buffering
  fflush(rxlog);
#endif
  BX_DEBUG(("eth_vde: got packet: %d bytes, dst=%02x:%02x:%02x:%02x:%02x:%02x, src=%02x:%02x:%02x:%02x:%02x:%02x", (int)nbytes, rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7], rxbuf[8], rxbuf[9], rxbuf[10], rxbuf[11]));
  if (nbytes < MIN_RX_PACKET_LEN) {
    BX_DEBUG(("packet too short (%d), padding to %d", (int)nbytes, MIN_RX_PACKET_LEN));
    memset(rxbuf + nbytes, 0, MIN_RX_PACKET_LEN - nbytes);
    nbytes = MIN_RX_PACKET_LEN;
  }
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    this->rxh(this->netdev, rxbuf, (unsigned)nbytes);
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

#if BX_HAVE_LIBVDEPLUG

bool bx_vde_pktmover_c::vde_connect(const char *sw)
{
  char swname[BX_PATHNAME_LEN];
  char descr[] = "Bochs";

  // vde_open() may modify the switch name (port number suffix)
  strcpy(swname, sw);
  conn = vde_open(swname, descr, NULL);
  if (conn == NULL)
    return false;
  fddata = vde_datafd(conn);
  return true;
}

void bx_vde_pktmover_c::vde_disconnect()
{
  if (conn != NULL) {
    vde_close(conn);
    conn = NULL;
  }
  fddata = -1;
}

#else

// builtin implementation of the vde_switch control protocol

#define SWITCH_MAGIC 0xfeedface
#define REQ_NEW_CONTROL 0

struct request_v3 {
  Bit32u magic;
  Bit32u version;
  int type;
  struct sockaddr_un sock;
};

bool bx_vde_pktmover_c::vde_connect(const char *sw)
{
  static unsigned instance = 0;
  struct request_v3 req;
  struct sockaddr_un ctl;
  struct stat st;
  const char *datadir[2];
  ssize_t n;
  int err, i;

  memset(&ctl, 0, sizeof(ctl));
  memset(&datain, 0, sizeof(datain));
  memset(&dataout, 0, sizeof(dataout));
  ctl.sun_family = AF_UNIX;
  // vde2 uses a socket directory, vde 1.x used a plain control socket
  if ((stat(sw, &st) == 0) && S_ISDIR(st.st_mode)) {
    if (strlen(sw) + 4 >= BX_VDE_PATH_LEN) {
      errno = ENAMETOOLONG;
      return false;
    }
    snprintf(ctl.sun_path, sizeof(ctl.sun_path), "%s/ctl", sw);
    datadir[0] = sw;
  } else {
    if (strlen(sw) >= BX_VDE_PATH_LEN) {
      errno = ENAMETOOLONG;
      return false;
    }
    snprintf(ctl.sun_path, sizeof(ctl.sun_path), "%s", sw);
    datadir[0] = NULL;
  }
  datadir[1] = "/tmp";

  if ((fdctl = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    return false;
  if (connect(fdctl, (struct sockaddr *) &ctl, sizeof(ctl)) < 0)
    goto error;

  // bind the data socket to a filesystem path (abstract sockets are Linux only)
  if ((fddata = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    goto error;
  datain.sun_family = AF_UNIX;
  errno = ENOENT;
  for (i = 0; i < 2; i++) {
    if (datadir[i] == NULL) continue;
    n = snprintf(datain.sun_path, sizeof(datain.sun_path), "%s/.bochs-%05d-%u",
                 datadir[i], (int)getpid(), instance);
    if ((n < 0) || (n >= (ssize_t)sizeof(datain.sun_path))) {
      errno = ENAMETOOLONG;
      continue;
    }
    unlink(datain.sun_path);
    if (bind(fddata, (struct sockaddr *) &datain, sizeof(datain)) == 0)
      break;
  }
  if (i == 2) {
    datain.sun_path[0] = '\0';
    goto error;
  }
  instance++;

  memset(&req, 0, sizeof(req));
  req.magic = SWITCH_MAGIC;
  req.version = 3;
  req.type = REQ_NEW_CONTROL;
  req.sock = datain;
  if (send(fdctl, &req, sizeof(req), 0) < 0)
    goto error;
  n = recv(fdctl, &dataout, sizeof(dataout), 0);
  if (n != (ssize_t)sizeof(dataout)) {
    // the switch closes the connection if it rejects the request
    if (n >= 0) errno = ECONNREFUSED;
    goto error;
  }
  return true;

error:
  err = errno;
  vde_disconnect();
  errno = err;
  return false;
}

void bx_vde_pktmover_c::vde_disconnect()
{
  if (fddata >= 0) {
    close(fddata);
    fddata = -1;
  }
  if (fdctl >= 0) {
    close(fdctl);
    fdctl = -1;
  }
  if (datain.sun_path[0] != '\0') {
    unlink(datain.sun_path);
    datain.sun_path[0] = '\0';
  }
}

#endif /* BX_HAVE_LIBVDEPLUG */

#endif /* if BX_NETWORKING && BX_NETMOD_VDE */
