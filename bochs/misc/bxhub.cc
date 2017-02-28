/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * bxhub.c: a simple, two-port software 'ethernet hub' for use with
 * eth_socket Bochs ethernet pktmover.
 *
 * Copyright (C) 2003  by Mariusz Matuszek [NOmrmmSPAM @ users.sourceforge.net]
 * Copyright (C) 2017  The Bochs Project
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
 *
 *
 * Problems:
 * - only two 'interfaces' (easily extendable),
 * - in general - this is a quick hack :-)
 *
 * compile as you like, but 'gcc bxhub.c -o bxhub' will be enough.
 */

#include "config.h"

extern "C" {
#ifdef WIN32
#include <winsock2.h>
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

#ifdef WIN32
#define MSG_NOSIGNAL 0
#define MSG_DONTWAIT 0
#endif

#include "misc/bxcompat.h"
#include "osdep.h"
#include "iodev/network/netmod.h"

#define BUFSIZE 4096

#define ICMP_ECHO_PACKET_MAX  128

#define ARP_OPCODE_REQUEST 1
#define ARP_OPCODE_REPLY   2

typedef struct arp_header {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  Bit16u  hw_addr_space;
  Bit16u  proto_addr_space;
  Bit8u   hw_addr_len;
  Bit8u   proto_addr_len;
  Bit16u  opcode;
  /* HW address of sender */
  /* Protocol address of sender */
  /* HW address of target*/
  /* Protocol address of target */
}
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
arp_header_t;

typedef struct {
  SOCKET so;
  struct sockaddr_in sin, sout;
  bx_bool init;
  dhcp_cfg_t dhcp;
  Bit8u *reply_buffer;
  int pending_reply_size;
} hub_client_t;

const Bit8u default_host_macaddr[6] = {0xb0, 0xc4, 0x20, 0x00, 0x00, 0x03};
const Bit8u default_host_ipv4addr[4] = {10, 0, 2, 2};
const Bit8u default_dns_ipv4addr[4] = {10, 0, 2, 3};

const Bit8u default_guest_ipv4addr[2][4] =
{
  {10, 0, 2, 15},
  {10, 0, 2, 16},
};

const Bit8u broadcast_ipv4addr[3][4] =
{
  {  0,  0,  0,  0},
  {255,255,255,255},
  { 10,  0,  2,255},
};

static Bit16u port_base = 40000;
static char tftp_root[BX_PATHNAME_LEN];
static Bit8u host_macaddr[6];
static int client_count;


void prepare_builtin_reply(hub_client_t *client, unsigned type)
{
  dhcp_cfg_t *dhcpc = &client->dhcp;
  ethernet_header_t *ethhdr = (ethernet_header_t *)client->reply_buffer;

  memcpy(ethhdr->dst_mac_addr, dhcpc->guest_macaddr, ETHERNET_MAC_ADDR_LEN);
  memcpy(ethhdr->src_mac_addr, dhcpc->host_macaddr, ETHERNET_MAC_ADDR_LEN);
  ethhdr->type = htons(type);
}

bx_bool handle_ipv4(hub_client_t *client, Bit8u *buf, unsigned len)
{
  unsigned total_len;
  unsigned fragment_flags;
  unsigned fragment_offset;
  unsigned ipproto;
  unsigned l3header_len;
  const Bit8u *l4pkt;
  unsigned l4pkt_len;
  unsigned udp_sourceport;
  unsigned udp_targetport;
  unsigned udp_reply_size = 0;
  unsigned icmptype;
  unsigned icmpcode;
  Bit8u *replybuf = client->reply_buffer;
  dhcp_cfg_t *dhcpc = &client->dhcp;

  // guest-to-host IPv4
  if (len < (14U+20U)) {
    return 0;
  }
  if ((buf[14+0] & 0xf0) != 0x40) {
    return 0;
  }
  l3header_len = ((unsigned)(buf[14+0] & 0x0f) << 2);
  if (l3header_len != 20) {
    return 0;
  }
  if (len < (14U+l3header_len)) return 0;
  if (ip_checksum(&buf[14],l3header_len) != (Bit16u)0xffff) {
    return 0;
  }

  total_len = get_net2(&buf[14+2]);

  if (memcmp(&buf[14+16],dhcpc->host_ipv4addr, 4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[0],4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[1],4) &&
      memcmp(&buf[14+16],broadcast_ipv4addr[2],4))
  {
    return 0;
  }

  fragment_flags = (unsigned)buf[14+6] >> 5;
  fragment_offset = ((unsigned)get_net2(&buf[14+6]) & 0x1fff) << 3;
  ipproto = buf[14+9];

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    return 0;
  } else {
    l4pkt = &buf[14 + l3header_len];
    l4pkt_len = total_len - l3header_len;
  }

  if (ipproto == 0x11) {
    // guest-to-host UDP IPv4
    if (l4pkt_len < 8) return 0;
    udp_sourceport = get_net2(&l4pkt[0]);
    udp_targetport = get_net2(&l4pkt[2]);
    if ((udp_targetport == 67) || (udp_targetport == 69)) { // BOOTP & TFTP
      if (udp_targetport == 67) { // BOOTP
        udp_reply_size = process_dhcp(&l4pkt[8], l4pkt_len-8, &replybuf[42], dhcpc);
      } else if (strlen(tftp_root) > 0) {
        udp_reply_size = process_tftp(&l4pkt[8], l4pkt_len-8, udp_sourceport, &replybuf[42], tftp_root);
      }
      if (udp_reply_size > 0) {
        // host-to-guest UDP IPv4: pseudo-header
        replybuf[22] = 0;
        replybuf[23] = 0x11; // UDP
        put_net2(&replybuf[24], 8U+udp_reply_size);
        memcpy(&replybuf[26], dhcpc->host_ipv4addr, 4);
        memcpy(&replybuf[30], dhcpc->guest_ipv4addr, 4);
        // udp header
        put_net2(&replybuf[34], udp_targetport);
        put_net2(&replybuf[36], udp_sourceport);
        put_net2(&replybuf[38], 8U+udp_reply_size);
        put_net2(&replybuf[40], 0);
        put_net2(&replybuf[40],
                 ip_checksum(&replybuf[22], 12U+8U+udp_reply_size) ^ (Bit16u)0xffff);
        // ip header
        memset(&replybuf[14], 0, 20);
        replybuf[14] = 0x45;
        replybuf[15] = 0x00;
        put_net2(&replybuf[16], 20U+8U+udp_reply_size);
        put_net2(&replybuf[18], 1);
        replybuf[20] = 0x00;
        replybuf[21] = 0x00;
        replybuf[22] = 0x07; // TTL
        replybuf[23] = 0x11; // UDP
        // host-to-guest IPv4
        replybuf[14] = (replybuf[14] & 0x0f) | 0x40;
        l3header_len = ((unsigned)(replybuf[14] & 0x0f) << 2);
        memcpy(&replybuf[26], dhcpc->host_ipv4addr, 4);
        memcpy(&replybuf[30], dhcpc->guest_ipv4addr, 4);
        put_net2(&replybuf[24], 0);
        put_net2(&replybuf[24], ip_checksum(&replybuf[14], l3header_len) ^ (Bit16u)0xffff);
        client->pending_reply_size = udp_reply_size + 42;
        prepare_builtin_reply(client, ETHERNET_TYPE_IPV4);
      }
      // don't forward DHCP / TFTP requests to other client
      return 1;
    }
  } else if (ipproto == 0x01) {
    // guest-to-host ICMP
    if (l4pkt_len < 8) return 0;
    icmptype = l4pkt[0];
    icmpcode = l4pkt[1];
    if (ip_checksum(l4pkt,l4pkt_len) != (Bit16u)0xffff) {
      BX_ERROR(("icmp: invalid checksum"));
      return 0;
    }

    switch (icmptype) {
      case 0x08: // ECHO
        if (icmpcode == 0) {
          if ((14U+l3header_len+l4pkt_len) > ICMP_ECHO_PACKET_MAX) {
            BX_ERROR(("icmp echo: size of an echo packet is too long"));
            return 0;
          }
          memcpy(&replybuf[14], &buf[14], l3header_len);
          memcpy(&replybuf[14+l3header_len], l4pkt, l4pkt_len);
          replybuf[14+l3header_len+0] = 0x00; // echo reply
          put_net2(&replybuf[14+l3header_len+2],0);
          put_net2(&replybuf[14+l3header_len+2],
                   ip_checksum(&replybuf[14+l3header_len],l4pkt_len) ^ (Bit16u)0xffff);
          // host-to-guest IPv4
          replybuf[14] = (replybuf[14] & 0x0f) | 0x40;
          l3header_len = ((unsigned)(replybuf[14] & 0x0f) << 2);
          memcpy(&replybuf[26], dhcpc->host_ipv4addr, 4);
          memcpy(&replybuf[30], dhcpc->guest_ipv4addr, 4);
          put_net2(&replybuf[24], 0);
          put_net2(&replybuf[24], ip_checksum(&replybuf[14], l3header_len) ^ (Bit16u)0xffff);
          client->pending_reply_size = 14U+l3header_len+l4pkt_len;
          prepare_builtin_reply(client, ETHERNET_TYPE_IPV4);
          return 1;
        }
        break;
      default:
        BX_INFO(("unhandled icmp packet: type=%u code=%u",
                 icmptype, icmpcode));
        break;
    }
  }
  return 0;
}

bx_bool handle_arp(hub_client_t *client, Bit8u *buf, unsigned len)
{
  dhcp_cfg_t *dhcpc = &client->dhcp;
  arp_header_t *arphdr = (arp_header_t *)((Bit8u *)buf +
                                          sizeof(ethernet_header_t));
  arp_header_t *arprhdr = (arp_header_t *)((Bit8u *)client->reply_buffer +
                                          sizeof(ethernet_header_t));

  if ((ntohs(arphdr->hw_addr_space) != 0x0001) ||
      (ntohs(arphdr->proto_addr_space) != 0x0800) ||
      (arphdr->hw_addr_len != ETHERNET_MAC_ADDR_LEN) ||
      (arphdr->proto_addr_len != 4)) {
    fprintf(stderr, "Unhandled ARP message hw: %04x (%d) proto: %04x (%d)\n",
              ntohs(arphdr->hw_addr_space), arphdr->hw_addr_len,
              ntohs(arphdr->proto_addr_space), arphdr->proto_addr_len);
    return 0;
  }

  switch(ntohs(arphdr->opcode)) {
    case ARP_OPCODE_REQUEST:
      if (((Bit8u *)arphdr)[27] > 3)
        break;
      memset(client->reply_buffer, 0, MIN_RX_PACKET_LEN);
      arprhdr->hw_addr_space = htons(0x0001);
      arprhdr->proto_addr_space = htons(0x0800);
      arprhdr->hw_addr_len = ETHERNET_MAC_ADDR_LEN;
      arprhdr->proto_addr_len = 4;
      arprhdr->opcode = htons(ARP_OPCODE_REPLY);
      memcpy((Bit8u *)arprhdr+8, dhcpc->host_macaddr, ETHERNET_MAC_ADDR_LEN);
      memcpy((Bit8u *)arprhdr+14, (Bit8u *)arphdr+24, 4);
      memcpy((Bit8u *)arprhdr+18, dhcpc->guest_macaddr, ETHERNET_MAC_ADDR_LEN);
      memcpy((Bit8u *)arprhdr+24, (Bit8u *)arphdr+14, 4);
      client->pending_reply_size = MIN_RX_PACKET_LEN;
      prepare_builtin_reply(client, ETHERNET_TYPE_ARP);
      return 1;
    case ARP_OPCODE_REPLY:
      break;
    default:
      break;
  }
  return 0;
}

int handle_packet(hub_client_t *client, Bit8u *buf, unsigned len)
{
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;
  dhcp_cfg_t *dhcpc = &client->dhcp;
  int ret = 1;

  if (!client->init) {
    if (memcmp(ethhdr->src_mac_addr, host_macaddr, 6) == 0) {
      client->init = -1;
    } else {
      memcpy(dhcpc->host_macaddr, host_macaddr, ETHERNET_MAC_ADDR_LEN);
      memcpy(dhcpc->guest_macaddr, ethhdr->src_mac_addr, ETHERNET_MAC_ADDR_LEN);
      memcpy(dhcpc->host_ipv4addr, &default_host_ipv4addr[0], 4);
      memcpy(dhcpc->guest_ipv4addr, &broadcast_ipv4addr[1][0], 4);
      dhcpc->default_guest_ipv4addr = default_guest_ipv4addr[client_count++];
      memcpy(dhcpc->dns_ipv4addr, &default_dns_ipv4addr[0], 4);
      client->reply_buffer = new Bit8u[BUFSIZE];
      client->init = 1;
    }
  }
  if ((memcmp(ethhdr->dst_mac_addr, host_macaddr, ETHERNET_MAC_ADDR_LEN) != 0) &&
      (memcmp(ethhdr->dst_mac_addr, broadcast_macaddr, ETHERNET_MAC_ADDR_LEN) != 0)) {
    return 0;
  }

  if (client->pending_reply_size > 0)
    return 0;

  switch (ntohs(ethhdr->type)) {
    case ETHERNET_TYPE_IPV4: 
      ret = handle_ipv4(client, buf, len);
      break;
    case ETHERNET_TYPE_ARP:
      ret = handle_arp(client, buf, len);
      break;
    default:
      break;
  }
  return ret;
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bxhub [options]\n\n"
    "Supported options:\n"
    "  -port=...     base UDP port (bxhub uses 4 ports for 2 Bochs sessions)\n"
    "  -mac=...      host MAC address (default is b0:c4:20:00:00:03)\n"
    "  -tftp=...     enable TFTP support using specified directory\n"
    "  --help        display this help and exit\n\n");
}

int parse_cmdline(int argc, char *argv[])
{
  int arg = 1;
  int ret = 1;
  int n;
  int tmp[6];

  port_base = 40000;
  tftp_root[0] = 0;
  memcpy(host_macaddr, default_host_macaddr, ETHERNET_MAC_ADDR_LEN);
  while ((arg < argc) && (ret == 1)) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("/?", argv[arg], 2)) {
      print_usage();
      ret = 0;
    }
    else if (!strncmp("-port=", argv[arg], 6)) {
      port_base = atoi(&argv[arg][6]);
    }
    else if (!strncmp("-tftp=", argv[arg], 6)) {
      strcpy(tftp_root, &argv[arg][6]);
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

void send_packet(hub_client_t *client, Bit8u *buf, unsigned len)
{
  sendto(client->so, (char*)buf, len, (MSG_DONTROUTE|MSG_NOSIGNAL|MSG_DONTWAIT),
         (struct sockaddr*) &client->sout, sizeof(client->sout));
}

static hub_client_t hclient[2];

void CDECL intHandler(int sig)
{
  if (sig == SIGINT) {
    for (int i = 0; i < 2; i++) {
      if (hclient[i].init)
        delete [] hclient[i].reply_buffer;
      closesocket(hclient[i].so);
    }
  }
  exit(0);
}

int CDECL main(int argc, char **argv)
{
  int i, n;
  fd_set rfds;
  Bit8u buf[BUFSIZE];

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
  client_count = 0;

  for (i = 0; i < 2; i++) {
    memset(&hclient[i], 0, sizeof(hub_client_t));

    /* create sockets */
    if ((hclient[i].so = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("bxhub - cannot create socket");
      exit(1);
    }

    /* fill addres structures */
    hclient[i].sin.sin_family = AF_INET;
    hclient[i].sin.sin_port = htons(port_base + (i * 2) + 1); 
    hclient[i].sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

    hclient[i].sout.sin_family = AF_INET;
    hclient[i].sout.sin_port = htons(port_base + (i * 2));
    hclient[i].sout.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    /* configure (bind) sockets */
    if (bind(hclient[i].so, (struct sockaddr *) &hclient[i].sin, sizeof(hclient[i].sin)) < 0) {
      perror("bxhub - cannot bind socket");
      exit(2);
    }

    printf("RX port #%d in use: %d\n", i + 1, ntohs(hclient[i].sin.sin_port));
  }

  printf("Host MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
         host_macaddr[0], host_macaddr[1], host_macaddr[2],
         host_macaddr[3], host_macaddr[4], host_macaddr[5]
         );
  if (strlen(tftp_root) > 0) {
    printf("TFTP using root directory '%s'\n", tftp_root);
  } else {
    printf("TFTP support disabled\n");
  }
  printf("Press CTRL+C to quit bxhub\n");

  while (1) {

    /* wait for input */

    FD_ZERO(&rfds);
    FD_SET(hclient[0].so, &rfds);
    FD_SET(hclient[1].so, &rfds);

    n = select(hclient[1].so+1, &rfds, NULL, NULL, NULL);

    /* data is available somewhere */

    if (FD_ISSET(hclient[0].so, &rfds)) { // check input 1
      n = recv(hclient[0].so, (char*)buf, sizeof(buf), 0);
      if (n > 0) {
        if (!handle_packet(&hclient[0], buf, n)) {
          send_packet(&hclient[1], buf, n);
        }
      }
    }

    if (FD_ISSET(hclient[1].so, &rfds)) { // check input 2
      n = recv(hclient[1].so, (char*)buf, sizeof(buf), 0);
      if (n > 0) {
        if (!handle_packet(&hclient[1], buf, n)) {
          send_packet(&hclient[0], buf, n);
        }
      }
    }

    if ((hclient[0].init < 0) || (hclient[1].init < 0)) {
      fprintf(stderr, "bxhub - wrong MAC address configuration\n");
      break;
    }

    if (hclient[0].pending_reply_size > 0) {
      send_packet(&hclient[0], hclient[0].reply_buffer, hclient[0].pending_reply_size);
      hclient[0].pending_reply_size = 0;
    }

    if (hclient[1].pending_reply_size > 0) {
      send_packet(&hclient[1], hclient[1].reply_buffer, hclient[1].pending_reply_size);
      hclient[1].pending_reply_size = 0;
    }
  }
  return 0;
}
