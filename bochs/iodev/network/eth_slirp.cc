/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014-2024  The Bochs Project
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

#ifdef __CYGWIN__
#define __USE_W32_SOCKETS
#define _WIN32
#endif

#include "bochs.h"
#include "plugin.h"
#include "pc_system.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

#if BX_HAVE_LIBSLIRP
#include <slirp/libslirp.h>
#include <signal.h>
#else
#include "slirp/slirp.h"
#include "slirp/libslirp.h"
#endif

static unsigned int bx_slirp_instances = 0;

// network driver plugin entry point

PLUGIN_ENTRY_FOR_NET_MODULE(slirp)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_NET;
  }
  return 0; // Success
}

// network driver implementation

#define LOG_THIS netdev->

#define MAX_HOSTFWD 5

static int rx_timer_index = BX_NULL_TIMER_HANDLE;
fd_set rfds, wfds, xfds;
int nfds;

class bx_slirp_pktmover_c : public eth_pktmover_c {
public:
  bx_slirp_pktmover_c(const char *netif, const char *macaddr,
                      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                      logfunctions *netdev, const char *script);
  virtual ~bx_slirp_pktmover_c();
  void sendpkt(void *buf, unsigned io_len);
  void receive(void *pkt, unsigned pkt_len);
  int can_receive(void);
private:
  Slirp *slirp;
  unsigned netdev_speed;

  SlirpConfig config;
  char *hostfwd[MAX_HOSTFWD];
  int n_hostfwd;
#ifndef WIN32
  char *smb_export, *smb_tmpdir;
  struct in_addr smb_srv;
#endif
  char *pktlog_fn;
  FILE *pktlog_txt;
  bool slirp_logging;

  bool parse_slirp_conf(const char *conf);
  static void rx_timer_handler(void *);
  void rx_timer(void);

#ifndef WIN32
  int slirp_smb(Slirp *s, char *smb_tmpdir, const char *exported_dir,
                struct in_addr vserver_addr);
  void slirp_smb_cleanup(Slirp *s, char *smb_tmpdir);
#endif
  int slirp_hostfwd(Slirp *s, const char *redir_str, int legacy_format);
};

class bx_slirp_locator_c : public eth_locator_c {
public:
  bx_slirp_locator_c(void) : eth_locator_c("slirp") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           logfunctions *netdev, const char *script) {
    return (new bx_slirp_pktmover_c(netif, macaddr, rxh, rxstat, netdev, script));
  }
} bx_slirp_match;


#if BX_HAVE_LIBSLIRP
static slirp_ssize_t send_packet(const void *buf, size_t len, void *opaque);

static void guest_error(const char *msg, void *opaque)
{
  fprintf(stderr, "guest_error\n");
}

static int64_t clock_get_ns(void *opaque)
{
  return bx_pc_system.time_usec() * 1000;
}

static int npoll;

static void register_poll_fd(int fd, void *opaque)
{
  npoll++;
}

static void unregister_poll_fd(int fd, void *opaque)
{
  npoll--;
}

static void notify(void *opaque)
{
  // Nothing here yet
}

static struct SlirpCb callbacks = {
    .send_packet = send_packet,
    .guest_error = guest_error,
    .clock_get_ns = clock_get_ns,
    .register_poll_fd = register_poll_fd,
    .unregister_poll_fd = unregister_poll_fd,
    .notify = notify,
};
#endif

bx_slirp_pktmover_c::bx_slirp_pktmover_c(const char *netif,
                                         const char *macaddr,
                                         eth_rx_handler_t rxh,
                                         eth_rx_status_t rxstat,
                                         logfunctions *netdev,
                                         const char *script)
{
#if !BX_HAVE_LIBSLIRP
  logfunctions *slirplog;
  char prefix[10];
#endif

  slirp = NULL;
  pktlog_fn = NULL;
  n_hostfwd = 0;
  /* default settings according to historic slirp */
  memset(&config, 0, sizeof(config));
#if BX_HAVE_LIBSLIRP
  config.version = 4;
  config.in_enabled = true;
  config.disable_host_loopback = false;
  config.enable_emu = false;
  config.disable_dns = false;
  config.tftp_server_name = "tftp";
#endif
  config.restricted = false;
  config.vnetwork.s_addr = htonl(0x0a000200);    /* 10.0.2.0 */
  config.vnetmask.s_addr = htonl(0xffffff00);    /* 255.255.255.0 */
  config.vhost.s_addr = htonl(0x0a000202);       /* 10.0.2.2 */
  config.vdhcp_start.s_addr = htonl(0x0a00020f); /* 10.0.2.15 */
  config.vnameserver.s_addr = htonl(0x0a000203); /* 10.0.2.3 */
  config.tftp_path = netif;
#ifndef WIN32
  smb_export = NULL;
  smb_tmpdir = NULL;
  smb_srv.s_addr = 0;
#endif

  this->netdev = netdev;
#if BX_HAVE_LIBSLIRP
  BX_INFO(("slirp network driver (libslirp version %s)", slirp_version_string()));
#else
  if (sizeof(struct arphdr) != 28) {
    BX_FATAL(("system error: invalid ARP header structure size"));
  }
  BX_INFO(("slirp network driver"));
#endif

  this->rxh   = rxh;
  this->rxstat = rxstat;
  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  if (bx_slirp_instances == 0) {
    rx_timer_index =
      DEV_register_timer(this, this->rx_timer_handler, 1000, 1, 1,
                         "eth_slirp");
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
  }

  if ((strlen(script) > 0) && (strcmp(script, "none"))) {
    if (!parse_slirp_conf(script)) {
      BX_ERROR(("reading slirp config failed"));
    }
  }
#if BX_HAVE_LIBSLIRP
  slirp = slirp_new(&config, &callbacks, this);
#else
  slirplog = new logfunctions();
  sprintf(prefix, "SLIRP%d", bx_slirp_instances);
  slirplog->put(prefix);
  slirp = slirp_new(&config, this, slirplog);
#endif
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
  if (pktlog_fn != NULL) {
    pktlog_txt = fopen(pktlog_fn, "wb");
    slirp_logging = (pktlog_txt != NULL);
    if (slirp_logging) {
      fprintf(pktlog_txt, "slirp packetmover readable log file\n");
      if (strlen(netif) > 0) {
        fprintf(pktlog_txt, "TFTP root = %s\n", netif);
      } else {
        fprintf(pktlog_txt, "TFTP service disabled\n");
      }
      fprintf(pktlog_txt, "guest MAC address = ");
      int i;
      for (i=0; i<6; i++)
        fprintf(pktlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "\n");
      fprintf(pktlog_txt, "--\n");
      fflush(pktlog_txt);
    }
    free(pktlog_fn);
  } else {
    slirp_logging = 0;
  }
  bx_slirp_instances++;
}

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
    if (config.bootfile != NULL) free((void*)config.bootfile);
    if (config.vhostname != NULL) free((void*)config.vhostname);
    if (config.vdnssearch != NULL) {
      size_t i = 0;
      while (config.vdnssearch[i] != NULL) {
        free((void*)config.vdnssearch[i++]);
      }
      free((void*)config.vdnssearch);
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
    if (slirp_logging) {
      fclose(pktlog_txt);
    }
  }
}

bool bx_slirp_pktmover_c::parse_slirp_conf(const char *conf)
{
  FILE *fd = NULL;
  char line[512];
  char *ret, *param, *val, *tmp;
  bool format_checked = 0;
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
          config.restricted = atoi(val);
        } else if (!stricmp(param, "hostname")) {
          if (len2 < 33) {
            config.vhostname = (char*)malloc(len2+1);
            strcpy((char*)config.vhostname, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'hostname'"));
          }
        } else if (!stricmp(param, "bootfile")) {
          if (len2 < 128) {
            config.bootfile = (char*)malloc(len2+1);
            strcpy((char*)config.bootfile, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'bootfile'"));
          }
        } else if (!stricmp(param, "dnssearch")) {
          if (len2 < 256) {
            count = 1;
            for (i = 0; i < len2; i++) {
              if (val[i] == ',') count++;
            }
            config.vdnssearch = (const char**)malloc((count + 1) * sizeof(char*));
            i = 0;
            tmp = strtok(val, ",");
            while (tmp != NULL) {
              len2 = strip_whitespace(tmp);
              config.vdnssearch[i] = (char*)malloc(len2+1);
              strcpy((char*)config.vdnssearch[i], tmp);
              i++;
              tmp = strtok(NULL, ",");
            }
            config.vdnssearch[i] = NULL;
          } else {
            BX_ERROR(("slirp: wrong format for 'dnssearch'"));
          }
        } else if (!stricmp(param, "net")) {
          if (!inet_aton(val, &config.vnetwork)) {
            BX_ERROR(("slirp: wrong format for 'net'"));
          }
        } else if (!stricmp(param, "mask")) {
          if (!inet_aton(val, &config.vnetmask)) {
            BX_ERROR(("slirp: wrong format for 'mask'"));
          }
        } else if (!stricmp(param, "host")) {
          if (!inet_aton(val, &config.vhost)) {
            BX_ERROR(("slirp: wrong format for 'host'"));
          }
        } else if (!stricmp(param, "dhcpstart")) {
          if (!inet_aton(val, &config.vdhcp_start)) {
            BX_ERROR(("slirp: wrong format for 'dhcpstart'"));
          }
        } else if (!stricmp(param, "dns")) {
          if (!inet_aton(val, &config.vnameserver)) {
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
        } else if (!stricmp(param, "pktlog")) {
          if (len2 < BX_PATHNAME_LEN) {
            pktlog_fn = (char*)malloc(len2+1);
            strcpy(pktlog_fn, val);
          } else {
            BX_ERROR(("slirp: wrong format for 'pktlog'"));
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

void bx_slirp_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  if (slirp_logging) {
    write_pktlog_txt(pktlog_txt, (const Bit8u*)buf, io_len, 0);
  }
  slirp_input(slirp, (Bit8u*)buf, io_len);
}

void bx_slirp_pktmover_c::rx_timer_handler(void *this_ptr)
{
  ((bx_slirp_pktmover_c*)this_ptr)->rx_timer();
}


#if BX_HAVE_LIBSLIRP
static int add_poll_cb(int fd, int events, void *opaque)
{
    if (events & SLIRP_POLL_IN)
        FD_SET(fd, &rfds);
    if (events & SLIRP_POLL_OUT)
        FD_SET(fd, &wfds);
    if (events & SLIRP_POLL_PRI)
        FD_SET(fd, &xfds);
    if (nfds < fd)
        nfds = fd;
    return fd;
}

static int get_revents_cb(int idx, void *opaque)
{
    int event = 0;
    if (FD_ISSET(idx, &rfds))
        event |= SLIRP_POLL_IN;
    if (FD_ISSET(idx, &wfds))
        event |= SLIRP_POLL_OUT;
    if (FD_ISSET(idx, &xfds))
        event |= SLIRP_POLL_PRI;
    return event;
}
#endif

void bx_slirp_pktmover_c::rx_timer(void)
{
  Bit32u timeout = 0;
  int ret;
#ifdef WIN32
  TIMEVAL tv;
#else
  struct timeval tv;
#endif

  nfds = -1;
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&xfds);
#if BX_HAVE_LIBSLIRP
  slirp_pollfds_fill(slirp, &timeout, add_poll_cb, this);
#else
  slirp_select_fill(&nfds, &rfds, &wfds, &xfds, &timeout);
#endif
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  ret = select(nfds + 1, &rfds, &wfds, &xfds, &tv);
#if BX_HAVE_LIBSLIRP
  slirp_pollfds_poll(slirp, (ret < 0), get_revents_cb, this);
#else
  slirp_select_poll(&rfds, &wfds, &xfds, (ret < 0));
#endif
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
    if (pkt_len < MIN_RX_PACKET_LEN) pkt_len = MIN_RX_PACKET_LEN;
    if (slirp_logging) {
      write_pktlog_txt(pktlog_txt, (const Bit8u*)pkt, pkt_len, 1);
    }
    this->rxh(this->netdev, pkt, pkt_len);
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

#if BX_HAVE_LIBSLIRP
static slirp_ssize_t send_packet(const void *buf, size_t len, void *opaque)
{
  bx_slirp_pktmover_c *class_ptr = (bx_slirp_pktmover_c *)opaque;
  class_ptr->receive((void*)buf, len);
  return len;
}
#endif

#if !defined(_WIN32) && !defined(__CYGWIN__)

#define CONFIG_SMBD_COMMAND "/usr/sbin/smbd"

#include <pwd.h>

/* automatic user mode samba server configuration */
int bx_slirp_pktmover_c::slirp_smb(Slirp *s, char *smb_tmpdir, const char *exported_dir,
                                   struct in_addr vserver_addr)
{
    static int instance;
    int i;
    char smb_conf[128], smb_cmdline[150];
    char share[64], error_msg[256];
    struct passwd *passwd;
    FILE *f;

    passwd = getpwuid(geteuid());
    if (!passwd) {
        BX_ERROR(("failed to retrieve user name"));
        return -1;
    }

    if (access(CONFIG_SMBD_COMMAND, F_OK)) {
        BX_ERROR(("could not find '%s', please install it", CONFIG_SMBD_COMMAND));
        return -1;
    }

    if (access(exported_dir, R_OK | X_OK)) {
        snprintf(error_msg, sizeof(error_msg), "error accessing shared directory '%s': %s",
                 exported_dir, strerror(errno));
        BX_ERROR(("%s", error_msg));
        return -1;
    }

    i = strlen(exported_dir) - 2;
    while ((i > 0) && (exported_dir[i] != '/')) i--;
    snprintf(share, sizeof(share), "%s", &exported_dir[i+1]);
    if (share[strlen(share)-1] == '/') share[strlen(share)-1] = '\0';

    snprintf(smb_tmpdir, 128, "/tmp/bochs-smb.%ld-%d",
             (long)getpid(), instance++);
    if (mkdir(smb_tmpdir, 0700) < 0) {
        snprintf(error_msg, sizeof(error_msg), "could not create samba server dir '%s'",
                 smb_tmpdir);
        BX_ERROR(("%s", error_msg));
        return -1;
    }
    snprintf(smb_conf, sizeof(smb_conf), "%s/%s", smb_tmpdir, "smb.conf");

    f = fopen(smb_conf, "w");
    if (!f) {
        slirp_smb_cleanup(s, smb_tmpdir);
        snprintf(error_msg, sizeof(error_msg), "could not create samba server configuration file '%s'",
                 smb_conf);
        BX_ERROR(("%s", error_msg));
        return -1;
    }
    fprintf(f,
            "[global]\n"
            "private dir=%s\n"
            "interfaces=127.0.0.1\n"
            "bind interfaces only=yes\n"
            "pid directory=%s\n"
            "lock directory=%s\n"
            "state directory=%s\n"
            "cache directory=%s\n"
            "ncalrpc dir=%s/ncalrpc\n"
            "log file=%s/log.smbd\n"
            "smb passwd file=%s/smbpasswd\n"
            "security = user\n"
            "map to guest = Bad User\n"
            "[%s]\n"
            "path=%s\n"
            "read only=no\n"
            "guest ok=yes\n"
            "force user=%s\n",
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            share,
            exported_dir,
            passwd->pw_name
            );
    fclose(f);

    snprintf(smb_cmdline, sizeof(smb_cmdline), "%s -s %s",
             CONFIG_SMBD_COMMAND, smb_conf);

    if ((slirp_add_exec(s, smb_cmdline, &vserver_addr, 139) < 0) ||
        (slirp_add_exec(s, smb_cmdline, &vserver_addr, 445) < 0)) {
        slirp_smb_cleanup(s, smb_tmpdir);
        BX_ERROR(("conflicting/invalid smbserver address"));
        return -1;
    }
    return 0;
}

void bx_slirp_pktmover_c::slirp_smb_cleanup(Slirp *s, char *smb_tmpdir)
{
    char cmd[128];
    char error_msg[256];
    int ret;

    if (smb_tmpdir[0] != '\0') {
        snprintf(cmd, sizeof(cmd), "rm -rf %s", smb_tmpdir);
        ret = system(cmd);
        if (ret == -1 || !WIFEXITED(ret)) {
            snprintf(error_msg, sizeof(error_msg), "'%s' failed.", cmd);
            BX_ERROR(("%s", error_msg));
        } else if (WEXITSTATUS(ret)) {
            snprintf(error_msg, sizeof(error_msg), "'%s' failed. Error code: %d",
                     cmd, WEXITSTATUS(ret));
            BX_ERROR(("%s", error_msg));
        }
        smb_tmpdir[0] = '\0';
    }
}
#endif

static int get_str_sep(char *buf, int buf_size, const char **pp, int sep)
{
    const char *p, *p1;
    int len;
    p = *pp;
    p1 = strchr(p, sep);
    if (!p1)
        return -1;
    len = p1 - p;
    p1++;
    if (buf_size > 0) {
        if (len > buf_size - 1)
            len = buf_size - 1;
        memcpy(buf, p, len);
        buf[len] = '\0';
    }
    *pp = p1;
    return 0;
}

int bx_slirp_pktmover_c::slirp_hostfwd(Slirp *s, const char *redir_str, int legacy_format)
{
    struct in_addr host_addr;
    struct in_addr guest_addr;
    int host_port, guest_port;
    const char *p;
    char buf[256];
    int is_udp;
    char *end;

    host_addr.s_addr = INADDR_ANY;
    guest_addr.s_addr = 0;
    p = redir_str;
    if (!p || get_str_sep(buf, sizeof(buf), &p, ':') < 0) {
        goto fail_syntax;
    }
    if (!strcmp(buf, "tcp") || buf[0] == '\0') {
        is_udp = 0;
    } else if (!strcmp(buf, "udp")) {
        is_udp = 1;
    } else {
        goto fail_syntax;
    }

    if (!legacy_format) {
        if (get_str_sep(buf, sizeof(buf), &p, ':') < 0) {
            goto fail_syntax;
        }
        if (buf[0] != '\0' && !inet_aton(buf, &host_addr)) {
            goto fail_syntax;
        }
    }

    if (get_str_sep(buf, sizeof(buf), &p, legacy_format ? ':' : '-') < 0) {
        goto fail_syntax;
    }
    host_port = strtol(buf, &end, 0);
    if (*end != '\0' || host_port < 1 || host_port > 65535) {
        goto fail_syntax;
    }

    if (get_str_sep(buf, sizeof(buf), &p, ':') < 0) {
        goto fail_syntax;
    }
    if (buf[0] != '\0' && !inet_aton(buf, &guest_addr)) {
        goto fail_syntax;
    }

    guest_port = strtol(p, &end, 0);
    if (*end != '\0' || guest_port < 1 || guest_port > 65535) {
        goto fail_syntax;
    }

    if (slirp_add_hostfwd(s, is_udp, host_addr, host_port, guest_addr,
                          guest_port) < 0) {
        BX_ERROR(("could not set up host forwarding rule '%s'", redir_str));
        return -1;
    }
    return 0;

 fail_syntax:
    BX_ERROR(("invalid host forwarding rule '%s'", redir_str));
    return -1;
}

#endif /* if BX_NETWORKING && BX_NETMOD_SLIRP */
