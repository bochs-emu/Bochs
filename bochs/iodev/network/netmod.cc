/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2017  The Bochs Project
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

// Common networking and helper code to find and create pktmover classes

// Peter Grehan (grehan@iprg.nokia.com) coded the initial version of the
// NE2000/ether stuff.

#include "iodev.h"

#if BX_NETWORKING

#include "netmod.h"

#define LOG_THIS bx_netmod_ctl.

bx_netmod_ctl_c bx_netmod_ctl;

bx_netmod_ctl_c::bx_netmod_ctl_c()
{
  put("netmodctl", "NETCTL");
}

void bx_netmod_ctl_c::init(void)
{
  // Nothing here yet
}

void bx_netmod_ctl_c::exit(void)
{
  eth_locator_c::cleanup();
}

void* bx_netmod_ctl_c::init_module(bx_list_c *base, void *rxh, void *rxstat, bx_devmodel_c *netdev)
{
  eth_pktmover_c *ethmod;

  // Attach to the selected ethernet module
  const char *modname = SIM->get_param_enum("ethmod", base)->get_selected();
  if (!eth_locator_c::module_present(modname)) {
#if BX_PLUGINS
    PLUG_load_net_plugin(modname);
#else
    BX_PANIC(("could not find networking module '%s'", modname));
#endif
  }
  ethmod = eth_locator_c::create(modname,
                                 SIM->get_param_string("ethdev", base)->getptr(),
                                 (const char *) SIM->get_param_string("mac", base)->getptr(),
                                 (eth_rx_handler_t)rxh, (eth_rx_status_t)rxstat, netdev,
                                 SIM->get_param_string("script", base)->getptr());

  if (ethmod == NULL) {
    BX_PANIC(("could not find networking module '%s'", modname));
    // if they continue, use null.
    BX_INFO(("could not find networking module '%s' - using 'null' instead", modname));

    ethmod = eth_locator_c::create("null", NULL,
                                   (const char *) SIM->get_param_string("mac", base)->getptr(),
                                   (eth_rx_handler_t)rxh, (eth_rx_status_t)rxstat, netdev, "");
    if (ethmod == NULL)
      BX_PANIC(("could not locate 'null' module"));
  }
  return ethmod;
}

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

eth_locator_c::~eth_locator_c()
{
  eth_locator_c *ptr = 0;

  if (this == all) {
    all = all->next;
  } else {
    ptr = all;
    while (ptr != NULL) {
      if (ptr->next != this) {
        ptr = ptr->next;
      } else {
        break;
      }
    }
  }
  if (ptr) {
    ptr->next = this->next;
  }
}

bx_bool eth_locator_c::module_present(const char *type)
{
  eth_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return 1;
  }
  return 0;
}

void eth_locator_c::cleanup()
{
#if BX_PLUGINS
  while (all != NULL) {
    PLUG_unload_net_plugin(all->type);
  }
#endif
}

//
// Called by ethernet chip emulations to locate and create a pktmover
// object
//
eth_pktmover_c *
eth_locator_c::create(const char *type, const char *netif,
                      const char *macaddr,
                      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                      bx_devmodel_c *dev, const char *script)
{
  eth_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return (ptr->allocate(netif, macaddr, rxh, rxstat, dev, script));
  }
  return NULL;
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
    BX_INFO(("Executing script '%s %s'", filename, arg1));
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
  Bit8u rawbuf[18];
  unsigned c, n;

  if (!host_to_guest) {
    fprintf(pktlog_txt, "a packet from guest to host, length %u\n", len);
  } else {
    fprintf(pktlog_txt, "a packet from host to guest, length %u\n", len);
  }
  n = 0;
  c = 0;
  while (n < len) {
    fprintf(pktlog_txt, "%02x ", (unsigned)charbuf[n]);
    if ((charbuf[n] >= 0x20) && (charbuf[n] < 0x80)) {
      rawbuf[c++] = charbuf[n];
    } else {
      rawbuf[c++] = '.';
    }
    n++;
    if (((n % 16) == 0) || (n == len)) {
      rawbuf[c] = 0;
      if (n == len) {
        while (c++ < 16) fprintf(pktlog_txt, "   ");
      }
      fprintf(pktlog_txt, " %s\n", rawbuf);
      c = 0;
    }
  }
  fprintf(pktlog_txt, "--\n");
  fflush(pktlog_txt);
}

#endif /* if BX_NETWORKING */
