/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2021  The Bochs Project
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

#include "bochs.h"
#include "plugin.h"
#include "gui/siminterface.h"

#if BX_NETWORKING

#include "netmod.h"

#define LOG_THIS bx_netmod_ctl.

bx_netmod_ctl_c bx_netmod_ctl;

const char **net_module_names;

bx_netmod_ctl_c::bx_netmod_ctl_c()
{
  put("netmodctl", "NETCTL");
}

void bx_netmod_ctl_c::init(void)
{
  Bit8u i, count = 0;

  count = PLUG_get_plugins_count(PLUGTYPE_NET);
  net_module_names = (const char**) malloc((count + 1) * sizeof(char*));
  for (i = 0; i < count; i++) {
    net_module_names[i] = PLUG_get_plugin_name(PLUGTYPE_NET, i);
  }
  net_module_names[count] = NULL;
  // move 'null' module to the top of the list
  if (strcmp(net_module_names[0], "null")) {
    for (i = 1; i < count; i++) {
      if (!strcmp(net_module_names[i], "null")) {
        net_module_names[i] = net_module_names[0];
        net_module_names[0] = "null";
        break;
      }
    }
  }
}

const char **bx_netmod_ctl_c::get_module_names(void)
{
  return net_module_names;
}

void bx_netmod_ctl_c::list_modules(void)
{
  char list[60];
  Bit8u i = 0;
  size_t len = 0, len1;

  BX_INFO(("Networking modules"));
  list[0] = 0;
  while (net_module_names[i] != NULL) {
    len1 = strlen(net_module_names[i]);
    if ((len + len1 + 1) > 58) {
      BX_INFO((" %s", list));
      list[0] = 0;
      len = 0;
    }
    strcat(list, " ");
    strcat(list, net_module_names[i]);
    len = strlen(list);
    i++;
  }
  if (len > 0) {
    BX_INFO((" %s", list));
  }
}

void bx_netmod_ctl_c::exit(void)
{
  free(net_module_names);
  eth_locator_c::cleanup();
}

void* bx_netmod_ctl_c::init_module(bx_list_c *base, void *rxh, void *rxstat, logfunctions *netdev)
{
  eth_pktmover_c *ethmod;

  // Attach to the selected ethernet module
  const char *modname = SIM->get_param_enum("ethmod", base)->get_selected();
  if (!eth_locator_c::module_present(modname)) {
#if BX_PLUGINS
    PLUG_load_plugin_var(modname, PLUGTYPE_NET);
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
  eth_locator_c *ptr;

  this->type = type;
  this->next = NULL;
  if (all == NULL) {
    all = this;
  } else {
    ptr = all;
    while (ptr->next) ptr = ptr->next;
    ptr->next = this;
  }
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

bool eth_locator_c::module_present(const char *type)
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
    PLUG_unload_plugin_type(all->type, PLUGTYPE_NET);
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
                      logfunctions *netdev, const char *script)
{
  eth_locator_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return (ptr->allocate(netif, macaddr, rxh, rxstat, netdev, script));
  }
  return NULL;
}

#if (BX_NETMOD_TAP==1) || (BX_NETMOD_TUNTAP==1) || (BX_NETMOD_VDE==1)

extern "C" {
#include <sys/wait.h>
};

// This is a utility script used for tuntap or ethertap
int execute_script(logfunctions *netdev, const char* scriptname, char* arg1)
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

void write_pktlog_txt(FILE *pktlog_txt, const Bit8u *buf, unsigned len, bool host_to_guest)
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

size_t strip_whitespace(char *s)
{
  size_t ptr = 0;
  char *tmp = (char*)malloc(strlen(s)+1);
  strcpy(tmp, s);
  while (s[ptr] == ' ') ptr++;
  if (ptr > 0) strcpy(s, tmp+ptr);
  free(tmp);
  ptr = strlen(s);
  while ((ptr > 0) && (s[ptr-1] == ' ')) {
    s[--ptr] = 0;
  }
  return ptr;
}

#endif /* if BX_NETWORKING */
