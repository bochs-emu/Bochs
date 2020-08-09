/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * Copyright (C) 2003       by Mariusz Matuszek
 *                             [NOmrmmSPAM @ users.sourceforge.net]
 * Copyright (C) 2017-2020  The Bochs Project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

// bxhub.c: a simple, two-port software 'ethernet hub' for use with
// eth_socket Bochs ethernet pktmover.

// Extensions by Volker Ruppert (2017-2020):
// - Windows support added.
// - Integrated 'vnet' server features (ARP, ICMP-echo, DHCP, DNS, FTP and TFTP).
// - Added DNS service support for the server 'vnet' and connected clients.
// - Command line options added for base UDP port and 'vnet' server features.
// - Support for connects from up to 6 Bochs sessions.
// - Support for connecting from other machines.

#ifdef __CYGWIN__
#define __USE_W32_SOCKETS
#endif

#include "config.h"

extern "C" {
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define closesocket(s)    close(s)
typedef int SOCKET;
#endif
#include <signal.h>
};

#ifndef BXHUB
#define BXHUB
#endif

#ifdef __APPLE__
#define MSG_NOSIGNAL 0
#endif

#ifdef WIN32
#define MSG_NOSIGNAL 0
#define MSG_DONTWAIT 0
#endif

#include "misc/bxcompat.h"
#include "osdep.h"
#include "iodev/network/netmod.h"
#include "iodev/network/netutil.h"

#define BXHUB_MAX_CLIENTS 6

typedef struct {
  Bit8u      id;
  SOCKET     so;
  struct     sockaddr_in sin, sout;
  bx_bool    init;
  Bit8u      macaddr[6];
  Bit8u      default_ipv4addr[4];
  Bit8u      *reply_buffer;
  unsigned   pending_reply_size;
} hub_client_t;

const Bit8u default_host_macaddr[6] = {0xb0, 0xc4, 0x20, 0x00, 0x00, 0x0f};
const Bit8u default_host_ipv4addr[4] = {10, 0, 2, 2};
const Bit8u default_dns_ipv4addr[4] = {10, 0, 2, 3};
const Bit8u default_ftp_ipv4addr[4] = {10, 0, 2, 4};
const Bit8u dhcp_base_ipv4addr[4] = {10, 0, 2, 15};
const char  default_bootfile[] = "pxelinux.0";

static Bit16u port_base = 40000;
static char tftp_root[BX_PATHNAME_LEN];
static char dhcp_bootfile[128];
static Bit8u host_macaddr[6];
static int client_max;
static Bit8u n_clients;
static hub_client_t hclient[BXHUB_MAX_CLIENTS];
static dhcp_cfg_t dhcp;
static vnet_server_c vnet_server;
int bx_loglev;
static char bx_logfname[BX_PATHNAME_LEN];


bx_bool handle_packet(hub_client_t *client, Bit8u *buf, unsigned len)
{
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;

  if (!client->init) {
    if (memcmp(ethhdr->src_mac_addr, host_macaddr, 6) == 0) {
      client->init = -1;
      return 0;
    } else {
      client->sout.sin_addr.s_addr = client->sin.sin_addr.s_addr;
      client->id = n_clients++;
      memcpy(client->macaddr, ethhdr->src_mac_addr, 6);
      vnet_server.init_client(client->id, client->macaddr);
      client->reply_buffer = new Bit8u[BX_PACKET_BUFSIZE];
      client->init = 1;
    }
  }

  vnet_server.handle_packet(buf, len);
  client->pending_reply_size = vnet_server.get_packet(client->reply_buffer);
  return (client->pending_reply_size > 0);
}

void send_packet(hub_client_t *client, Bit8u *buf, unsigned len)
{
  sendto(client->so, (char*)buf, len, (MSG_NOSIGNAL|MSG_DONTWAIT),
         (struct sockaddr*) &client->sout, sizeof(client->sout));
}

void broadcast_packet(int clientid, Bit8u *buf, unsigned len)
{
  if (handle_packet(&hclient[clientid], buf, len))
    return;
  for (int i = 0; i < client_max; i++) {
    if (i != clientid) {
      send_packet(&hclient[i], buf, len);
    }
  }
}

bx_bool find_client(Bit8u *dst_mac_addr, int *clientid)
{
  *clientid = -1;
  for (int i = 0; i < client_max; i++) {
    if (hclient[i].init) {
      if (memcmp(dst_mac_addr, hclient[i].macaddr, ETHERNET_MAC_ADDR_LEN) == 0) {
        *clientid = i;
        break;
      }
    }
  }
  return (*clientid >= 0);
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bxhub [options]\n\n"
    "Supported options:\n"
    "  -ports=...    number of virtual ethernet ports (2 - 6)\n"
    "  -base=...     base UDP port (bxhub uses 2 ports per Bochs session)\n"
    "  -mac=...      host MAC address (default is b0:c4:20:00:00:0f)\n"
    "  -tftp=...     enable FTP and TFTP support using specified directory as root\n"
    "  -bootfile=... network bootfile reported by DHCP - located on TFTP server\n"
    "  -loglev=...   set log level (0 - 3, default 1)\n"
    "  -logfile=...  send log output to file\n"
    "  --help        display this help and exit\n\n");
}

int parse_cmdline(int argc, char *argv[])
{
  int arg = 1;
  int ret = 1;
  int n;
  int tmp[6];

  client_max = 2;
  bx_loglev = 1;
  port_base = 40000;
  tftp_root[0] = 0;
  dhcp_bootfile[0] = 0;
  bx_logfname[0] = 0;
  memcpy(host_macaddr, default_host_macaddr, ETHERNET_MAC_ADDR_LEN);
  while ((arg < argc) && (ret == 1)) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("/?", argv[arg], 2)) {
      print_usage();
      ret = 0;
    }
    else if (!strncmp("-ports=", argv[arg], 7)) {
      n = atoi(&argv[arg][7]);
      if ((n > 1) && (n < BXHUB_MAX_CLIENTS)) {
        client_max = n;
      } else {
        printf("Number of virtual ethernet ports out of range\n\n");
        ret = 0;
      }
    }
    else if (!strncmp("-base=", argv[arg], 6)) {
      n = atoi(&argv[arg][6]);
      if ((n >= 1000) && (n <= 65530)) {
        port_base = n;
      } else {
        printf("UDP base port number out of range\n\n");
        ret = 0;
      }
    }
    else if (!strncmp("-tftp=", argv[arg], 6)) {
      strcpy(tftp_root, &argv[arg][6]);
    }
    else if (!strncmp("-bootfile=", argv[arg], 10)) {
      strcpy(dhcp_bootfile, &argv[arg][10]);
    }
    else if (!strncmp("-mac=", argv[arg], 5)) {
      n = sscanf(&argv[arg][5], "%x:%x:%x:%x:%x:%x",
                 &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
      if (n != 6) {
        printf("Host MAC address malformed.\n\n");
        ret = 0;
      } else {
        for (n=0; n<6; n++) host_macaddr[n] = (Bit8u)tmp[n];
      }
    }
    else if (!strncmp("-loglev=", argv[arg], 8)) {
      n = atoi(&argv[arg][8]);
      if ((n >= 0) && (n <= 3)) {
        bx_loglev = n;
      } else {
        printf("Unsupported log level %d (must be 0 - 3)\n\n", n);
        ret = 0;
      }
    }
    else if (!strncmp("-logfile=", argv[arg], 9)) {
      strcpy(bx_logfname, &argv[arg][9]);
    }
    else if (argv[arg][0] == '-') {
      printf("Unknown option: %s\n\n", argv[arg]);
      ret = 0;
    } else {
      printf("Ignoring extra parameter: %s\n\n", argv[arg]);
    }
    arg++;
  }
  return ret;
}

void CDECL intHandler(int sig)
{
  if (sig == SIGINT) {
    for (int i = 0; i < client_max; i++) {
      if (hclient[i].init) {
        delete [] hclient[i].reply_buffer;
      }
      closesocket(hclient[i].so);
    }
#ifdef WIN32
    WSACleanup();
#endif
  }
  exit(0);
}

int CDECL main(int argc, char **argv)
{
  int c, i, n;
  socklen_t slen;
  fd_set rfds;
  Bit8u buf[BX_PACKET_BUFSIZE];
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;

  if (!parse_cmdline(argc, argv))
    exit(0);

#ifdef WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  wVersionRequested = MAKEWORD(2, 0);
  err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    fprintf(stderr, "WSAStartup failed\n");
    return 0;
  }
#endif

  signal(SIGINT, intHandler);

  n_clients = 0;
  for (i = 0; i < client_max; i++) {
    memset(&hclient[i], 0, sizeof(hub_client_t));

    /* create sockets */
    if ((hclient[i].so = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("bxhub - cannot create socket");
      exit(1);
    }

    /* fill addres structures */
    hclient[i].sin.sin_family = AF_INET;
    hclient[i].sin.sin_port = htons(port_base + (i * 2) + 1);
    hclient[i].sin.sin_addr.s_addr = htonl(INADDR_ANY);

    hclient[i].sout.sin_family = AF_INET;
    hclient[i].sout.sin_port = htons(port_base + (i * 2));
    hclient[i].sout.sin_addr.s_addr = htonl(INADDR_ANY);

    /* configure (bind) sockets */
    if (bind(hclient[i].so, (struct sockaddr *) &hclient[i].sin, sizeof(hclient[i].sin)) < 0) {
      perror("bxhub - cannot bind socket");
      exit(2);
    }

    printf("RX port #%d in use: %d\n", i + 1, ntohs(hclient[i].sin.sin_port));
  }

  memcpy(dhcp.host_macaddr, host_macaddr, ETHERNET_MAC_ADDR_LEN);
  memcpy(dhcp.srv_ipv4addr[VNET_SRV], default_host_ipv4addr, 4);
  memcpy(dhcp.srv_ipv4addr[VNET_DNS], default_dns_ipv4addr, 4);
  memcpy(dhcp.srv_ipv4addr[VNET_MISC], default_ftp_ipv4addr, 4);
  memcpy(dhcp.client_base_ipv4addr, &dhcp_base_ipv4addr, 4);
  if (strlen(dhcp_bootfile) > 0) {
    strcpy(dhcp.bootfile, dhcp_bootfile);
  } else {
    strcpy(dhcp.bootfile, default_bootfile);
  }
  vnet_server.init(NULL, &dhcp, tftp_root);
  printf("Host MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
         host_macaddr[0], host_macaddr[1], host_macaddr[2],
         host_macaddr[3], host_macaddr[4], host_macaddr[5]
         );
  if (strlen(tftp_root) > 0) {
    printf("FTP / TFTP using root directory '%s'\n", tftp_root);
    printf("Network bootfile set to '%s'\n", dhcp.bootfile);
  } else {
    printf("FTP / TFTP support and network boot disabled\n");
  }

  if (strlen(bx_logfname) > 0) {
    vnet_server.init_log(bx_logfname);
    printf("Using log file '%s'\n", bx_logfname);
  }
  printf("Press CTRL+C to quit bxhub\n");

  while (1) {

    /* wait for input */

    FD_ZERO(&rfds);
    for (i = 0; i < client_max; i++) {
      FD_SET(hclient[i].so, &rfds);
    }

    n = select(hclient[client_max-1].so+1, &rfds, NULL, NULL, NULL);

    /* data is available somewhere */

    for (i = 0; i < client_max; i++) {
      // check input
      if (FD_ISSET(hclient[i].so, &rfds)) {
        slen = sizeof(hclient[i].sin);
        n = recvfrom(hclient[i].so, (char*)buf, sizeof(buf), 0,
                     (struct sockaddr*) &hclient[i].sin, &slen);
        if (n > 0) {
          if (memcmp(ethhdr->dst_mac_addr, broadcast_macaddr, ETHERNET_MAC_ADDR_LEN) == 0) {
            broadcast_packet(i, buf, n);
          } else if (memcmp(ethhdr->dst_mac_addr, host_macaddr, ETHERNET_MAC_ADDR_LEN) == 0) {
            handle_packet(&hclient[i], buf, n);
          } else if (find_client(ethhdr->dst_mac_addr, &c)) {
            send_packet(&hclient[c], buf, n);
          }
        }
      }
      // send reply from builtin service
      while (hclient[i].pending_reply_size > 0) {
        send_packet(&hclient[i], hclient[i].reply_buffer, hclient[i].pending_reply_size);
        // check for another pending packet
        hclient[i].pending_reply_size = vnet_server.get_packet(hclient[i].reply_buffer);
      }
      // check MAC address of new client
      if (hclient[i].init != 0) {
        if (hclient[i].init < 0) {
          fprintf(stderr, "bxhub - wrong MAC address configuration\n");
          break;
        }
      }
    }
  }
  return 0;
}
