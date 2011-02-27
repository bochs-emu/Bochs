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

// eth.cc  - helper code to find and create pktmover classes

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_NETWORKING

#include "eth.h"

#define LOG_THIS /* not needed */

eth_locator_c *eth_locator_c::all;

//
// Each pktmover module has a static locator class that registers
// here
//
eth_locator_c::eth_locator_c(const char *type)
{
  next = all;
  all  = this;
  this->type = type;
}

extern class bx_null_locator_c bx_null_match;
#if BX_NETMOD_FBSD
extern class bx_fbsd_locator_c bx_fbsd_match;
#endif
#if BX_NETMOD_LINUX
extern class bx_linux_locator_c bx_linux_match;
#endif
#if BX_NETMOD_WIN32
extern class bx_win32_locator_c bx_win32_match;
#endif
#if BX_NETMOD_TAP
extern class bx_tap_locator_c bx_tap_match;
#endif
#if BX_NETMOD_TUNTAP
extern class bx_tuntap_locator_c bx_tuntap_match;
#endif
#if BX_NETMOD_VDE
extern class bx_vde_locator_c bx_vde_match;
#endif
#if BX_NETMOD_ARPBACK
extern class bx_arpback_locator_c bx_arpback_match;
#endif
#if BX_NETMOD_SLIRP
extern class bx_slirp_locator_c bx_slirp_match;
#endif
extern class bx_vnet_locator_c bx_vnet_match;

//
// Called by ethernet chip emulations to locate and create a pktmover
// object
//
eth_pktmover_c *
eth_locator_c::create(const char *type, const char *netif,
                      const char *macaddr,
                      eth_rx_handler_t rxh, bx_devmodel_c *dev,
                      const char *script)
{
#ifdef eth_static_constructors
  for (eth_locator_c *p = all; p != NULL; p = p->next) {
    if (strcmp(type, p->type) == 0)
      return (p->allocate(netif, macaddr, rxh, dev, script));
  }
#else
  eth_locator_c *ptr = 0;

  if (!strcmp(type, "null")) {
    ptr = (eth_locator_c *) &bx_null_match;
  }
#if BX_NETMOD_ARPBACK
  {
    if (!strcmp(type, "arpback"))
      ptr = (eth_locator_c *) &bx_arpback_match;
  }
#endif
#if BX_NETMOD_FBSD
  {
    if (!strcmp(type, "fbsd"))
      ptr = (eth_locator_c *) &bx_fbsd_match;
  }
#endif
#if BX_NETMOD_LINUX
  {
    if (!strcmp(type, "linux"))
      ptr = (eth_locator_c *) &bx_linux_match;
  }
#endif
#if BX_NETMOD_TUNTAP
  {
    if (!strcmp(type, "tuntap"))
      ptr = (eth_locator_c *) &bx_tuntap_match;
  }
#endif
#if BX_NETMOD_VDE
  {
    if (!strcmp(type, "vde"))
      ptr = (eth_locator_c *) &bx_vde_match;
  }
#endif
#if BX_NETMOD_SLIRP
  {
    if (!strcmp(type, "slirp"))
      ptr = (eth_locator_c *) &bx_slirp_match;
  }
#endif
#if BX_NETMOD_TAP
  {
    if (!strcmp(type, "tap"))
      ptr = (eth_locator_c *) &bx_tap_match;
  }
#endif
#if BX_NETMOD_WIN32
  {
    if(!strcmp(type, "win32"))
      ptr = (eth_locator_c *) &bx_win32_match;
  }
#endif
  if (!strcmp(type, "vnet")) {
    ptr = (eth_locator_c *) &bx_vnet_match;
  }
  if (ptr) {
    return (ptr->allocate(netif, macaddr, rxh, dev, script));
  }
#endif

  return (NULL);
}

#if (BX_NETMOD_TAP==1) || (BX_NETMOD_TUNTAP==1) || (BX_NETMOD_VDE==1)

extern "C" {
#include <sys/wait.h>
};

// This is a utility script used for tuntap or ethertap
int execute_script(bx_devmodel_c *netdev, const char* scriptname, char* arg1)
{
  int pid,status;

  if (!(pid=fork())) {
    char filename[BX_PATHNAME_LEN];
    if (scriptname[0]=='/') {
      strcpy(filename, scriptname);
    }
    else {
      getcwd(filename, BX_PATHNAME_LEN);
      strcat(filename, "/");
      strcat(filename, scriptname);
    }

    // execute the script
    netdev->info("Executing script '%s %s'",filename,arg1);
    execle(filename, scriptname, arg1, NULL, NULL);

    // if we get here there has been a problem
    exit(-1);
  }

  wait (&status);
  if (!WIFEXITED(status)) {
    return -1;
  }
  return WEXITSTATUS(status);
}

#endif

void write_pktlog_txt(FILE *pktlog_txt, const Bit8u *buf, unsigned len, bx_bool host_to_guest)
{
  Bit8u *charbuf = (Bit8u *)buf;
  unsigned n;

  if (!host_to_guest) {
    fprintf(pktlog_txt, "a packet from guest to host, length %u\n", len);
  } else {
    fprintf(pktlog_txt, "a packet from host to guest, length %u\n", len);
  }
  for (n = 0; n < len; n++) {
    if (((n % 16) == 0) && (n > 0))
      fprintf(pktlog_txt, "\n");
    fprintf(pktlog_txt, "%02x ", (unsigned)charbuf[n]);
  }
  fprintf(pktlog_txt, "\n--\n");
  fflush(pktlog_txt);
}

Bit16u get_net2(const Bit8u *buf)
{
  return (((Bit16u)*buf) << 8) |
         ((Bit16u)*(buf+1));
}

Bit16u ip_checksum(const Bit8u *buf, unsigned buf_len)
{
  Bit32u sum = 0;
  unsigned n;

  for (n = 0; n < buf_len; n++) {
    if (n & 1) {
      sum += (Bit32u)(*buf++);
    } else {
      sum += (Bit32u)(*buf++) << 8;
    }
  }
  while (sum > 0xffff) {
    sum = (sum >> 16) + (sum & 0xffff);
  }

  return (Bit16u)sum;
}

#endif /* if BX_NETWORKING */
