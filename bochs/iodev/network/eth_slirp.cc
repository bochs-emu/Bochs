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

// eth_slirp.cc  - Bochs port of Qemu's slirp implementation

#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

#include "slirp/slirp.h"
#include "slirp/libslirp.h"

#define LOG_THIS netdev->

#define MAX_HOSTFWD 5

static int rx_timer_index = BX_NULL_TIMER_HANDLE;
static unsigned int bx_slirp_instances = 0;
fd_set rfds, wfds, xfds;
int nfds;

extern int slirp_hostfwd(Slirp *s, const char *redir_str, int legacy_format);
#ifndef WIN32
extern int slirp_smb(Slirp *s, char *smb_tmpdir, const char *exported_dir,
                     struct in_addr vserver_addr);
void slirp_smb_cleanup(Slirp *s, char *smb_tmpdir);
#endif

class bx_slirp_pktmover_c : public eth_pktmover_c {
public:
  bx_slirp_pktmover_c(const char *netif, const char *macaddr,
                      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                      bx_devmodel_c *dev, const char *script);
  virtual ~bx_slirp_pktmover_c();
  void sendpkt(void *buf, unsigned io_len);
  void receive(void *pkt, unsigned pkt_len);
  int can_receive(void);
private:
  Slirp *slirp;
  unsigned netdev_speed;

  int restricted;
  struct in_addr net, mask, host, dhcp, dns;
  char *bootfile, *hostname, **dnssearch;
  char *hostfwd[MAX_HOSTFWD];
  int n_hostfwd;
#ifndef WIN32
  char *smb_export, *smb_tmpdir;
  struct in_addr smb_srv;
#endif

  bx_bool parse_slirp_conf(const char *conf);
  static void rx_timer_handler(void *);
};

class bx_slirp_locator_c : public eth_locator_c {
public:
  bx_slirp_locator_c(void) : eth_locator_c("slirp") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           bx_devmodel_c *dev, const char *script) {
    return (new bx_slirp_pktmover_c(netif, macaddr, rxh, rxstat, dev, script));
  }
} bx_slirp_match;


bx_slirp_pktmover_c::~bx_slirp_pktmover_c()
{
  if (slirp != NULL) {
    slirp_cleanup(slirp);
#ifndef WIN32
    if ((smb_export != NULL) && (smb_tmpdir != NULL)) {
      slirp_smb_cleanup(slirp, smb_tmpdir);
      free(smb_tmpdir);
      free(smb_export);
    }
#endif
    if (bootfile != NULL) free(bootfile);
    if (hostname != NULL) free(hostname);
    if (dnssearch != NULL) {
      size_t i = 0;
      while (dnssearch[i] != NULL) {
        free(dnssearch[i++]);
      }
      free(dnssearch);
    }
    while (n_hostfwd > 0) {
      free(hostfwd[--n_hostfwd]);
    }
    if (--bx_slirp_instances == 0) {
      bx_pc_system.deactivate_timer(rx_timer_index);
#ifndef WIN32
      signal(SIGPIPE, SIG_DFL);
#endif
    }
  }
}

static size_t strip_whitespace(char *s)
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

bx_bool bx_slirp_pktmover_c::parse_slirp_conf(const char *conf)
{
  FILE *fd = NULL;
  char line[512];
  char *ret, *param, *val, *tmp;
  bx_bool format_checked = 0;
  size_t len1 = 0, len2;
  unsigned i, count;

  fd = fopen(conf, "r");
  if (fd == NULL) return 0;

  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    size_t len = strlen(line);
    if ((len>0) && (line[len-1] < ' '))
      line[len-1] = '\0';
    if ((ret != NULL) && (strlen(line) > 0)) {
      if (!format_checked) {
        if (!strncmp(line, "# slirp config", 14)) {
          format_checked = 1;
        } else {
          BX_ERROR(("slirp config: wrong file format"));
          fclose(fd);
          return 0;
        }
      } else {
        if (line[0] == '#') continue;
        param = strtok(line, "=");
        if (param != NULL) {
          len1 = strip_whitespace(param);
          val = strtok(NULL, "");
          if (val == NULL) {
            BX_ERROR(("slirp config: missing value for parameter '%s'", param));
            continue;
          }
        } else {
          continue;
        }
        len2 = strip_whitespace(val);
        if ((len1 == 0) || (len2 == 0)) continue;
        if (!stricmp(param, "restricted")) {
          restricted = atoi(val);
        } else if (!stricmp(param, "hostname")) {
          if (len2 < 33) {
            hostname = (char*)malloc(len2+1);
            strcpy(hostname, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'hostname'"));
          }
        } else if (!stricmp(param, "bootfile")) {
          if (len2 < 128) {
            bootfile = (char*)malloc(len2+1);
            strcpy(bootfile, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'bootfile'"));
          }
        } else if (!stricmp(param, "dnssearch")) {
          if (len2 < 256) {
            count = 1;
            for (i = 0; i < len2; i++) {
              if (val[i] == ',') count++;
            }
            dnssearch = (char**)malloc((count + 1) * sizeof(char*));
            i = 0;
            tmp = strtok(val, ",");
            while (tmp != NULL) {
              len2 = strip_whitespace(tmp);
              dnssearch[i] = (char*)malloc(len2+1);
              strcpy(dnssearch[i], tmp);
              i++;
              tmp = strtok(NULL, ",");
            }
            dnssearch[i] = NULL;
          } else {
            BX_ERROR(("slirp: wrong format for 'dnssearch'"));
          }
        } else if (!stricmp(param, "net")) {
          if (!inet_aton(val, &net)) {
            BX_ERROR(("slirp: wrong format for 'net'"));
          }
        } else if (!stricmp(param, "mask")) {
          if (!inet_aton(val, &mask)) {
            BX_ERROR(("slirp: wrong format for 'mask'"));
          }
        } else if (!stricmp(param, "host")) {
          if (!inet_aton(val, &host)) {
            BX_ERROR(("slirp: wrong format for 'host'"));
          }
        } else if (!stricmp(param, "dhcpstart")) {
          if (!inet_aton(val, &dhcp)) {
            BX_ERROR(("slirp: wrong format for 'dhcpstart'"));
          }
        } else if (!stricmp(param, "dns")) {
          if (!inet_aton(val, &dns)) {
            BX_ERROR(("slirp: wrong format for 'dns'"));
          }
#ifndef WIN32
        } else if (!stricmp(param, "smb_export")) {
          if ((len2 < 256) && (val[0] == '/')) {
            smb_export = (char*)malloc(len2+1);
            strcpy(smb_export, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'smb_export'"));
          }
        } else if (!stricmp(param, "smb_srv")) {
          if (!inet_aton(val, &smb_srv)) {
            BX_ERROR(("slirp: wrong format for 'smb_srv'"));
          }
#endif
        } else if (!stricmp(param, "hostfwd")) {
          if (len2 < 256) {
            if (n_hostfwd < MAX_HOSTFWD) {
              hostfwd[n_hostfwd] = (char*)malloc(len2+1);
              strcpy(hostfwd[n_hostfwd], val);
              n_hostfwd++;
            } else {
              BX_ERROR(("slirp: too many 'hostfwd' rules"));
            }
          } else {
            BX_ERROR(("slirp: wrong format for 'hostfwd'"));
          }
        } else {
          BX_ERROR(("slirp: unknown option '%s'", line));
        }
      }
    }
  } while (!feof(fd));
  fclose(fd);
  return 1;
}

bx_slirp_pktmover_c::bx_slirp_pktmover_c(const char *netif,
                                         const char *macaddr,
                                         eth_rx_handler_t rxh,
                                         eth_rx_status_t rxstat,
                                         bx_devmodel_c *dev,
                                         const char *script)
{
  logfunctions *slirplog;
  char prefix[10];

  restricted = 0;
  slirp = NULL;
  hostname = NULL;
  bootfile = NULL;
  dnssearch = NULL;
  n_hostfwd = 0;
  /* default settings according to historic slirp */
  net.s_addr  = htonl(0x0a000200); /* 10.0.2.0 */
  mask.s_addr = htonl(0xffffff00); /* 255.255.255.0 */
  host.s_addr = htonl(0x0a000202); /* 10.0.2.2 */
  dhcp.s_addr = htonl(0x0a00020f); /* 10.0.2.15 */
  dns.s_addr  = htonl(0x0a000203); /* 10.0.2.3 */
#ifndef WIN32
  smb_export = NULL;
  smb_tmpdir = NULL;
  smb_srv.s_addr = 0;
#endif

  this->netdev = dev;
  if (sizeof(struct arphdr) != 28) {
    BX_PANIC(("system error: invalid ARP header structure size"));
  }
  BX_INFO(("slirp network driver"));

  this->rxh   = rxh;
  this->rxstat = rxstat;
  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  if (bx_slirp_instances == 0) {
    rx_timer_index =
      bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
                                1, 1, "eth_slirp");
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
  }

  if ((strlen(script) > 0) && (strcmp(script, "none"))) {
    if (!parse_slirp_conf(script)) {
      BX_ERROR(("reading slirp config failed"));
    }
  }
  slirplog = new logfunctions();
  sprintf(prefix, "SLIRP%d", bx_slirp_instances);
  slirplog->put(prefix);
  slirp = slirp_init(restricted, net, mask, host, hostname, netif, bootfile, dhcp, dns,
                     (const char**)dnssearch, this, slirplog);
  if (n_hostfwd > 0) {
    for (int i = 0; i < n_hostfwd; i++) {
      slirp_hostfwd(slirp, hostfwd[i], 0);
    }
  }
#ifndef WIN32
  if (smb_export != NULL) {
    smb_tmpdir = (char*)malloc(128);
    if (slirp_smb(slirp, smb_tmpdir, smb_export, smb_srv) < 0) {
      BX_ERROR(("failed to initialize SMB support"));
    }
  }
#endif
  bx_slirp_instances++;
}

void bx_slirp_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  slirp_input(slirp, (Bit8u*)buf, io_len);
}

void bx_slirp_pktmover_c::rx_timer_handler(void *this_ptr)
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
  bx_slirp_pktmover_c *class_ptr = (bx_slirp_pktmover_c *)this_ptr;
  return class_ptr->can_receive();
}

int bx_slirp_pktmover_c::can_receive()
{
  return ((this->rxstat(this->netdev) & BX_NETDEV_RXREADY) != 0);
}

void slirp_output(void *this_ptr, const Bit8u *pkt, int pkt_len)
{
  bx_slirp_pktmover_c *class_ptr = (bx_slirp_pktmover_c *)this_ptr;
  class_ptr->receive((void*)pkt, pkt_len);
}

void bx_slirp_pktmover_c::receive(void *pkt, unsigned pkt_len)
{
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    if (pkt_len < 60) pkt_len = 60;
    this->rxh(this->netdev, pkt, pkt_len);
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

#endif /* if BX_NETWORKING && BX_NETMOD_SLIRP */
