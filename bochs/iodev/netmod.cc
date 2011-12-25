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

// Common networking and helper code to find and create pktmover classes

// Peter Grehan (grehan@iprg.nokia.com) coded the initial version of the
// NE2000/ether stuff.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_NETWORKING

#include "netmod.h"

#define LOG_THIS netdev->

bx_netmod_ctl_c* theNetModCtl = NULL;

int libnetmod_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theNetModCtl = new bx_netmod_ctl_c;
    bx_devices.pluginNetModCtl = theNetModCtl;
    return 0; // Success
  } else {
    return -1;
  }
}

void libnetmod_LTX_plugin_fini(void)
{
  delete theNetModCtl;
}

void* bx_netmod_ctl_c::init_module(bx_list_c *base, void *rxh, void *rxstat, bx_devmodel_c *netdev)
{
  eth_pktmover_c *ethmod;

  // Attach to the selected ethernet module
  const char *modname = SIM->get_param_enum("ethmod", base)->get_selected();
  ethmod = eth_locator_c::create(modname,
                                 SIM->get_param_string("ethdev", base)->getptr(),
                                 (const char *) SIM->get_param_string("macaddr", base)->getptr(),
                                 (eth_rx_handler_t)rxh, (eth_rx_status_t)rxstat, netdev,
                                 SIM->get_param_string("script", base)->getptr());

  if (ethmod == NULL) {
    BX_PANIC(("could not find eth module %s", modname));
    // if they continue, use null.
    BX_INFO(("could not find eth module %s - using null instead", modname));

    ethmod = eth_locator_c::create("null", NULL,
                                   (const char *) SIM->get_param_string("macaddr", base)->getptr(),
                                   (eth_rx_handler_t)rxh, (eth_rx_status_t)rxstat, netdev, "");
    if (ethmod == NULL)
      BX_PANIC(("could not locate null module"));
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
                      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                      bx_devmodel_c *dev, const char *script)
{
#ifdef eth_static_constructors
  for (eth_locator_c *p = all; p != NULL; p = p->next) {
    if (strcmp(type, p->type) == 0)
      return (p->allocate(netif, macaddr, rxh, rxstat, dev, script));
  }
#else
  eth_locator_c *ptr = 0;

  if (!strcmp(type, "null")) {
    ptr = (eth_locator_c *) &bx_null_match;
  }
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
    return (ptr->allocate(netif, macaddr, rxh, rxstat, dev, script));
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

#define BOOTREQUEST 1
#define BOOTREPLY 2

#define BOOTPOPT_PADDING 0
#define BOOTPOPT_END 255
#define BOOTPOPT_SUBNETMASK 1
#define BOOTPOPT_TIMEOFFSET 2
#define BOOTPOPT_ROUTER_OPTION 3
#define BOOTPOPT_DOMAIN_NAMESERVER 6
#define BOOTPOPT_HOST_NAME 12
#define BOOTPOPT_DOMAIN_NAME 15
#define BOOTPOPT_MAX_DATAGRAM_SIZE 22
#define BOOTPOPT_DEFAULT_IP_TTL 23
#define BOOTPOPT_BROADCAST_ADDRESS 28
#define BOOTPOPT_ARPCACHE_TIMEOUT 35
#define BOOTPOPT_DEFAULT_TCP_TTL 37
#define BOOTPOPT_NTP_SERVER 42
#define BOOTPOPT_NETBIOS_NAMESERVER 44
#define BOOTPOPT_X_FONTSERVER 48
#define BOOTPOPT_REQUESTED_IP_ADDRESS 50
#define BOOTPOPT_IP_ADDRESS_LEASE_TIME 51
#define BOOTPOPT_OPTION_OVRLOAD 52
#define BOOTPOPT_DHCP_MESSAGETYPE 53
#define BOOTPOPT_SERVER_IDENTIFIER 54
#define BOOTPOPT_PARAMETER_REQUEST_LIST 55
#define BOOTPOPT_MAX_DHCP_MESSAGE_SIZE 57
#define BOOTPOPT_RENEWAL_TIME 58
#define BOOTPOPT_REBINDING_TIME 59
#define BOOTPOPT_CLASS_IDENTIFIER 60
#define BOOTPOPT_CLIENT_IDENTIFIER 61

#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPDECLINE  4
#define DHCPACK      5
#define DHCPNAK      6
#define DHCPRELEASE  7
#define DHCPINFORM   8

#define DEFAULT_LEASE_TIME 28800

static const Bit8u subnetmask_ipv4addr[4] = {0xff,0xff,0xff,0x00};
static const Bit8u broadcast_ipv4addr1[4] = {0xff,0xff,0xff,0xff};

int process_dhcp(bx_devmodel_c *netdev, const Bit8u *data, unsigned data_len, Bit8u *reply, dhcp_cfg_t *dhcp)
{
  const Bit8u *opts;
  unsigned opts_len;
  unsigned extcode;
  unsigned extlen;
  const Bit8u *extdata;
  unsigned dhcpmsgtype = 0;
  bx_bool found_serverid = false;
  bx_bool found_leasetime = false;
  bx_bool found_guest_ipaddr = false;
  bx_bool found_host_name = false;
  Bit32u leasetime = BX_MAX_BIT32U;
  const Bit8u *dhcpreqparams = NULL;
  unsigned dhcpreqparams_len = 0;
  Bit8u dhcpreqparam_default[8];
  bx_bool dhcpreqparam_default_validflag = false;
  unsigned dhcpreqparams_default_len = 0;
  Bit8u *replyopts;
  Bit8u replybuf[576];
  char *hostname = NULL;
  unsigned hostname_len = 0;

  if (data_len < (236U+4U)) return 0;
  if (data[0] != BOOTREQUEST) return 0;
  if (data[1] != 1 || data[2] != 6) return 0;
  if (memcmp(&data[28U], dhcp->guest_macaddr, 6)) return 0;
  if (data[236] != 0x63 || data[237] != 0x82 ||
      data[238] != 0x53 || data[239] != 0x63) return 0;

  opts = &data[240];
  opts_len = data_len - 240U;

  while (1) {
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extcode = *opts++;
    opts_len--;

    if (extcode == BOOTPOPT_PADDING) continue;
    if (extcode == BOOTPOPT_END) break;
    if (opts_len < 1) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extlen = *opts++;
    opts_len--;
    if (opts_len < extlen) {
      BX_ERROR(("dhcp: invalid request"));
      return 0;
    }
    extdata = opts;
    opts += extlen;
    opts_len -= extlen;

    switch (extcode)
    {
    case BOOTPOPT_DHCP_MESSAGETYPE:
      if (extlen != 1)
        break;
      dhcpmsgtype = *extdata;
      break;
    case BOOTPOPT_PARAMETER_REQUEST_LIST:
      if (extlen < 1)
        break;
      dhcpreqparams = extdata;
      dhcpreqparams_len = extlen;
      break;
    case BOOTPOPT_SERVER_IDENTIFIER:
      if (extlen != 4)
        break;
      if (memcmp(extdata, dhcp->host_ipv4addr, 4)) {
        BX_INFO(("dhcp: request to another server"));
        return 0;
      }
      found_serverid = true;
      break;
    case BOOTPOPT_IP_ADDRESS_LEASE_TIME:
      if (extlen != 4)
        break;
      leasetime = get_net4(&extdata[0]);
      found_leasetime = true;
      break;
    case BOOTPOPT_REQUESTED_IP_ADDRESS:
      if (extlen != 4)
        break;
      if (!memcmp(extdata, dhcp->default_guest_ipv4addr,4)) {
        found_guest_ipaddr = true;
        memcpy(dhcp->guest_ipv4addr, dhcp->default_guest_ipv4addr, 4);
      }
      break;
    case BOOTPOPT_HOST_NAME:
      if (extlen < 1)
        break;
      hostname = (char*)malloc(extlen);
      memcpy(hostname, extdata, extlen);
      hostname_len = extlen;
      found_host_name = true;
      break;
    default:
      BX_ERROR(("extcode %d not supported yet", extcode));
      break;
    }
  }

  memset(&dhcpreqparam_default,0,sizeof(dhcpreqparam_default));
  memset(&replybuf[0],0,sizeof(replybuf));
  replybuf[0] = BOOTREPLY;
  replybuf[1] = 1;
  replybuf[2] = 6;
  memcpy(&replybuf[4],&data[4],4);
  memcpy(&replybuf[16], dhcp->default_guest_ipv4addr, 4);
  memcpy(&replybuf[20], dhcp->host_ipv4addr, 4);
  memcpy(&replybuf[28],&data[28],6);
  memcpy(&replybuf[44],"vnet",4);
  memcpy(&replybuf[108],"pxelinux.0",10);
  replybuf[236] = 0x63;
  replybuf[237] = 0x82;
  replybuf[238] = 0x53;
  replybuf[239] = 0x63;
  replyopts = &replybuf[240];
  opts_len = sizeof(replybuf)/sizeof(replybuf[0])-240;
  switch (dhcpmsgtype) {
  case DHCPDISCOVER:
    BX_INFO(("dhcp server: DHCPDISCOVER"));
    // reset guest address; answer must be broadcasted to unconfigured IP
    memcpy(dhcp->guest_ipv4addr, broadcast_ipv4addr1, 4);
    *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
    *replyopts ++ = 1;
    *replyopts ++ = DHCPOFFER;
    opts_len -= 3;
    dhcpreqparam_default[0] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
    dhcpreqparam_default[1] = BOOTPOPT_SERVER_IDENTIFIER;
    if (found_host_name) {
      dhcpreqparam_default[2] = BOOTPOPT_HOST_NAME;
    }
    dhcpreqparam_default_validflag = true;
    break;
  case DHCPREQUEST:
    BX_INFO(("dhcp server: DHCPREQUEST"));
    // check ciaddr.
    if (found_serverid || found_guest_ipaddr || (!memcmp(&data[12], dhcp->default_guest_ipv4addr, 4))) {
      *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
      *replyopts ++ = 1;
      *replyopts ++ = DHCPACK;
      opts_len -= 3;
      dhcpreqparam_default[0] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
      if (!found_serverid) {
        dhcpreqparam_default[1] = BOOTPOPT_SERVER_IDENTIFIER;
      }
      dhcpreqparam_default_validflag = true;
    } else {
      *replyopts ++ = BOOTPOPT_DHCP_MESSAGETYPE;
      *replyopts ++ = 1;
      *replyopts ++ = DHCPNAK;
      opts_len -= 3;
      if (found_leasetime) {
        dhcpreqparam_default[dhcpreqparams_default_len++] = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
        dhcpreqparam_default_validflag = true;
      }
      if (!found_serverid) {
        dhcpreqparam_default[dhcpreqparams_default_len++] = BOOTPOPT_SERVER_IDENTIFIER;
        dhcpreqparam_default_validflag = true;
      }
    }
    break;
  default:
    BX_ERROR(("dhcp server: unsupported message type %u",dhcpmsgtype));
    return 0;
  }

  while (1) {
    while (dhcpreqparams_len-- > 0) {
      switch (*dhcpreqparams++) {
      case BOOTPOPT_SUBNETMASK:
        BX_INFO(("provide BOOTPOPT_SUBNETMASK"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SUBNETMASK;
        *replyopts ++ = 4;
        memcpy(replyopts,subnetmask_ipv4addr,4);
        replyopts += 4;
        break;
      case BOOTPOPT_ROUTER_OPTION:
        BX_INFO(("provide BOOTPOPT_ROUTER_OPTION"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_ROUTER_OPTION;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr, 4);
        replyopts += 4;
        break;
      case BOOTPOPT_DOMAIN_NAMESERVER:
        if (dhcp->dns_ipv4addr[0] != 0) {
          BX_INFO(("provide BOOTPOPT_DOMAIN_NAMESERVER"));
          if (opts_len < 6) {
            BX_ERROR(("option buffer is insufficient"));
            return 0;
          }
          opts_len -= 6;
          *replyopts ++ = BOOTPOPT_DOMAIN_NAMESERVER;
          *replyopts ++ = 4;
          memcpy(replyopts, dhcp->dns_ipv4addr, 4);
          replyopts += 4;
        }
        break;
      case BOOTPOPT_BROADCAST_ADDRESS:
        BX_INFO(("provide BOOTPOPT_BROADCAST_ADDRESS"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_BROADCAST_ADDRESS;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr, 3);
        replyopts += 3;
        *replyopts ++ = 0xff;
        break;
      case BOOTPOPT_IP_ADDRESS_LEASE_TIME:
        BX_INFO(("provide BOOTPOPT_IP_ADDRESS_LEASE_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_IP_ADDRESS_LEASE_TIME;
        *replyopts ++ = 4;
        if (leasetime < DEFAULT_LEASE_TIME) {
          put_net4(replyopts, leasetime);
        } else {
          put_net4(replyopts, DEFAULT_LEASE_TIME);
        }
        replyopts += 4;
        break;
      case BOOTPOPT_SERVER_IDENTIFIER:
        BX_INFO(("provide BOOTPOPT_SERVER_IDENTIFIER"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_SERVER_IDENTIFIER;
        *replyopts ++ = 4;
        memcpy(replyopts, dhcp->host_ipv4addr,4);
        replyopts += 4;
        break;
      case BOOTPOPT_RENEWAL_TIME:
        BX_INFO(("provide BOOTPOPT_RENEWAL_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_RENEWAL_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 600);
        replyopts += 4;
        break;
      case BOOTPOPT_REBINDING_TIME:
        BX_INFO(("provide BOOTPOPT_REBINDING_TIME"));
        if (opts_len < 6) {
          BX_ERROR(("option buffer is insufficient"));
          return 0;
        }
        opts_len -= 6;
        *replyopts ++ = BOOTPOPT_REBINDING_TIME;
        *replyopts ++ = 4;
        put_net4(replyopts, 1800);
        replyopts += 4;
        break;
      case BOOTPOPT_HOST_NAME:
        if (hostname != NULL) {
          BX_INFO(("provide BOOTPOPT_HOST_NAME"));
          if (opts_len < (hostname_len + 2)) {
            free(hostname);
            BX_ERROR(("option buffer is insufficient"));
            return 0;
          }
          opts_len -= (hostname_len + 2);
          *replyopts ++ = BOOTPOPT_HOST_NAME;
          *replyopts ++ = hostname_len;
          memcpy(replyopts, hostname, hostname_len);
          replyopts += hostname_len;
          free(hostname);
          hostname = NULL;
          break;
        }
      default:
        if (*(dhcpreqparams-1) != 0) {
          BX_ERROR(("dhcp server: requested parameter %u not supported yet",*(dhcpreqparams-1)));
        }
        break;
      }
    }

    if (!dhcpreqparam_default_validflag) break;
    dhcpreqparams = &dhcpreqparam_default[0];
    dhcpreqparams_len = sizeof(dhcpreqparam_default);
    dhcpreqparam_default_validflag = false;
  }

  if (opts_len < 1) {
    BX_ERROR(("option buffer is insufficient"));
    return 0;
  }
  opts_len -= 2;
  *replyopts ++ = BOOTPOPT_END;

  opts_len = replyopts - &replybuf[0];
  if (opts_len < (236U+64U)) {
    opts_len = (236U+64U); // BOOTP
  }
  if (opts_len < (548U)) {
    opts_len = 548U; // DHCP
  }
  memcpy(reply, replybuf, opts_len);
  return opts_len;
}

#endif /* if BX_NETWORKING */
