/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011  Heikki Lindholm
//  Copyright (C) 2011-2013  The Bochs Project
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

// eth_slirp.cc  - slirp backend to eth

#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdint.h>
#ifndef WIN32
#include <arpa/inet.h> /* ntohs, htons */
#else
#include <winsock.h>
#endif

#define LOG_THIS netdev->

#define BX_ETH_SLIRP_LOGGING 0

static unsigned int bx_slirp_instances = 0;

const Bit8u default_host_ipv4addr[4] = {10, 0, 2, 2};
const Bit8u default_dns_ipv4addr[4] = {10, 0, 2, 3};
const Bit8u default_guest_ipv4addr[4] = {10, 0, 2, 15};
const Bit8u broadcast_ipv4addr[3][4] =
{
  {  0,  0,  0,  0},
  {255,255,255,255},
  { 10,  0,  2,255},
};


#define SLIP_END     192
#define SLIP_ESC     219
#define SLIP_ESC_END 220
#define SLIP_ESC_ESC 221

static size_t encode_slip(Bit8u *src, Bit8u *dst, size_t src_len)
{
  Bit8u *dst_start = dst;
  Bit8u *src_end = src + src_len;

  while (src < src_end) {
    switch (*src) {
      case SLIP_END:
        *dst++ = SLIP_ESC;
        *dst++ = SLIP_ESC_END;
        break;
      case SLIP_ESC:
        *dst++ = SLIP_ESC;
        *dst++ = SLIP_ESC_ESC;
        break;
      default:
        *dst++ = *src;
        break;
    }
    src++;
  }
  *dst++ = SLIP_END;

  return dst - dst_start;
}

static int decode_slip(Bit8u *src, size_t *src_len, Bit8u *dst, size_t *dst_len)
{
  Bit8u *dst_start = dst;
  Bit8u *src_start = src;
  Bit8u *src_end = src + *src_len;
  int got_packet = 0;

  if (*src_len == 0) {
    *dst_len = 0;
    return 0;
  }

  while ((src < (src_end - 1)) && (!got_packet)) {
    switch (*src) {
      case SLIP_END:
        if (dst != dst_start) // discard empty packets
          got_packet = 1;
        src++;
        break;
      case SLIP_ESC:
        switch (*++src) {
          case SLIP_ESC_ESC:
            *dst++ = SLIP_ESC;
            src++;
            break;
          case SLIP_ESC_END:
            *dst++ = SLIP_END;
            src++;
            break;
          default:
            *dst++ = *src++;
            break;
        }
        break;
      default:
        *dst++ = *src++;
        break;
    }
  }
  if ((!got_packet) && (src < src_end)) {
    switch (*src) {
      case SLIP_END:
        got_packet = 1;
        src++;
        break;
      case SLIP_ESC:
        break;
      default:
        *dst++ = *src++;
        break;
    }
  }

  *src_len = src - src_start;
  *dst_len = dst - dst_start;

  return got_packet;
}



class bx_slirp_pktmover_c : public eth_pktmover_c {
public:
  bx_slirp_pktmover_c(const char *netif, const char *macaddr,
                     eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                     bx_devmodel_c *dev, const char *script);
  void sendpkt(void *buf, unsigned io_len);
  virtual ~bx_slirp_pktmover_c();
private:
  pid_t slirp_pid;
  int slirp_pipe_fds[2];
  Bit8u slip_output_buffer[4096]; // TODO: reasonable size for these?
  Bit8u slip_input_buffer[4096];
  size_t slip_input_buffer_filled;
  size_t slip_input_buffer_decoded;

  Bit8u reply_buffer[1024];
  int pending_reply_size;

  dhcp_cfg_t dhcp;

  char tftp_rootdir[BX_PATHNAME_LEN];

  int rx_timer_index;
  unsigned netdev_speed;
  unsigned tx_time;

  static void rx_timer_handler(void *);
  void rx_timer();

  bx_bool handle_ipv4(const Bit8u *buf, unsigned len);
  void handle_arp(Bit8u *buf, unsigned len);
  void prepare_builtin_reply(unsigned type);

#if BX_ETH_SLIRP_LOGGING
  FILE *pktlog_txt;
#endif
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

bx_slirp_pktmover_c::bx_slirp_pktmover_c(const char *netif,
                                         const char *macaddr,
                                         eth_rx_handler_t rxh,
                                         eth_rx_status_t rxstat,
                                         bx_devmodel_c *dev,
                                         const char *script)
{
  int flags;

  if (bx_slirp_instances > 0) {
    BX_PANIC(("only one 'slirp' instance supported yet"));
  }
  this->netdev = dev;
  this->rxh   = rxh;
  this->rxstat = rxstat;

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, slirp_pipe_fds))
    BX_PANIC(("socketpair() failed: %s", strerror(errno)));

  // mark our end of the pipe non-blocking because of the timer based poll
  flags = fcntl(slirp_pipe_fds[0], F_GETFL);
  if (flags == -1)
    BX_PANIC(("fcntl(,F_GETFL) failed: %s", strerror(errno)));
  flags |= O_NONBLOCK;
  if (fcntl (slirp_pipe_fds[0], F_SETFL, flags) != 0)
    BX_PANIC(("fcntl(,F_SETFL,) failed: %s", strerror(errno)));

  slirp_pid = fork();
  if (slirp_pid == -1) {
   BX_PANIC(("fork() failed: %s", strerror(errno)));
  }
  else if (slirp_pid == 0) {
    int ret;
    int nfd;

    nfd = open("/dev/null", O_RDWR);
    if (nfd != -1) {
      dup2(nfd, STDERR_FILENO);
    }

    if (dup2(slirp_pipe_fds[1], STDIN_FILENO) == -1) {
      BX_PANIC(("dup2() failed: %s", strerror(errno)));
    }
    /* XXX slirp seems to use stdin bidirectionally 
     * instead of using stdout for SLIP output */
    if (dup2(slirp_pipe_fds[1], STDOUT_FILENO) == -1) {
      BX_PANIC(("dup2() failed: %s", strerror(errno)));
    }
    close(slirp_pipe_fds[0]);
    ret = execlp(script == NULL ? "slirp" : script, 
                 script == NULL ? "slirp" : script, /*"-d", "-1", "-S",*/ NULL);
    if (ret == -1) {
      BX_PANIC(("execlp() failed: %s", strerror(errno)));
    }
  }

  strcpy(this->tftp_rootdir, netif);
  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  this->rx_timer_index =
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
                                1, 1, "eth_slirp");
  memcpy(&dhcp.guest_macaddr[0], macaddr, ETHERNET_MAC_ADDR_LEN);
  // ensure the slirp host has a different mac address
  memcpy(&dhcp.host_macaddr[0], macaddr, ETHERNET_MAC_ADDR_LEN);
  dhcp.host_macaddr[5] ^= 0x03;

  memcpy(&dhcp.host_ipv4addr[0], &default_host_ipv4addr[0], 4);
  memcpy(&dhcp.guest_ipv4addr[0], &broadcast_ipv4addr[1][0], 4);
  dhcp.default_guest_ipv4addr = default_guest_ipv4addr;
  memcpy(&dhcp.dns_ipv4addr[0], &default_dns_ipv4addr[0], 4);
  dhcp.max_guest_ipv4addr = 3;

  pending_reply_size = 0;
  slip_input_buffer_filled = slip_input_buffer_decoded = 0;

  close(slirp_pipe_fds[1]);

  BX_INFO(("'slirp' network driver initialized"));
  bx_slirp_instances++;

#if BX_ETH_SLIRP_LOGGING
  pktlog_txt = fopen("ne2k-pktlog.txt", "wb");
  if (!pktlog_txt) BX_PANIC(("ne2k-pktlog.txt failed"));
  fprintf(pktlog_txt, "slirp packetmover readable log file\n");
  fprintf(pktlog_txt, "host MAC address = ");
  int i;
  for (i=0; i<6; i++)
    fprintf(pktlog_txt, "%02x%s", 0xff & host_mac_addr[i], i<5?":" : "\n");
  fprintf(pktlog_txt, "guest MAC address = ");
  for (i=0; i<6; i++)
    fprintf(pktlog_txt, "%02x%s", 0xff & guest_mac_addr[i], i<5?":" : "\n");
  fprintf(pktlog_txt, "--\n");
  fflush(pktlog_txt);
#endif
}

bx_slirp_pktmover_c::~bx_slirp_pktmover_c()
{
  bx_slirp_instances--;
}

void bx_slirp_pktmover_c::handle_arp(Bit8u *buf, unsigned len)
{
  if (pending_reply_size > 0)
    return;

  pending_reply_size = process_arp(netdev, buf, len, reply_buffer, &dhcp);
  if (pending_reply_size > 0) {
    prepare_builtin_reply(ETHERNET_TYPE_ARP);
  }
}

// detect and handle DHCP request (partly copy & paste from eth_vnet.cc)
bx_bool bx_slirp_pktmover_c::handle_ipv4(const Bit8u *buf, unsigned len)
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
  unsigned udp_reply_size;

  // guest-to-host IPv4
  if (len < (14U+20U)) {
    return 0;
  }

  ip_header_t *iphdr = (ip_header_t *)((Bit8u *)buf +
                                       sizeof(ethernet_header_t));
  if (iphdr->version != 4) {
    return 0;
  }
  l3header_len = (iphdr->header_len << 2);
  if (l3header_len != 20) {
    return 0;
  }
  if (len < (14U+l3header_len)) return 0;
  if (ip_checksum((Bit8u*)iphdr, l3header_len) != (Bit16u)0xffff) {
    return 0;
  }

  total_len = ntohs(iphdr->total_len);

  if (memcmp(&iphdr->dst_addr, dhcp.host_ipv4addr, 4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[0],4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[1],4) &&
      memcmp(&iphdr->dst_addr, broadcast_ipv4addr[2],4))
  {
    return 0;
  }

  fragment_flags = ntohs(iphdr->frag_offs) >> 13;
  fragment_offset = (ntohs(iphdr->frag_offs) & 0x1fff) << 3;
  ipproto = iphdr->protocol;

  if ((fragment_flags & 0x1) || (fragment_offset != 0)) {
    return 0;
  } else {
    l4pkt = &buf[14 + l3header_len];
    l4pkt_len = total_len - l3header_len;
  }

  if (ipproto == 0x11) {
    // guest-to-host UDP IPv4
    if (l4pkt_len < 8) return 0;
    udp_header_t *udphdr = (udp_header_t *)l4pkt;
    udp_sourceport = ntohs(udphdr->src_port);
    udp_targetport = ntohs(udphdr->dst_port);
    if ((udp_targetport == 67) || (udp_targetport == 69)) { // BOOTP & TFTP
      if (udp_targetport == 67) { // BOOTP
        udp_reply_size = process_dhcp(netdev, &l4pkt[8], l4pkt_len-8, &reply_buffer[42], &dhcp);
      } else {
        udp_reply_size = process_tftp(netdev, &l4pkt[8], l4pkt_len-8, udp_sourceport, &reply_buffer[42], tftp_rootdir);
      }
      if (udp_reply_size > 0) {
        pending_reply_size = udp_reply_size + 42;
        // host-to-guest UDP IPv4: pseudo-header
        reply_buffer[22] = 0;
        reply_buffer[23] = 0x11; // UDP
        put_net2(&reply_buffer[24], 8U+udp_reply_size);
        memcpy(&reply_buffer[26], dhcp.host_ipv4addr, 4);
        memcpy(&reply_buffer[30], dhcp.guest_ipv4addr, 4);
        // udp header
        put_net2(&reply_buffer[34], udp_targetport);
        put_net2(&reply_buffer[36], udp_sourceport);
        put_net2(&reply_buffer[38], 8U+udp_reply_size);
        put_net2(&reply_buffer[40], 0);
        put_net2(&reply_buffer[40], ip_checksum(&reply_buffer[22], 12U+8U+udp_reply_size) ^ (Bit16u)0xffff);
        // ip header
        memset(&reply_buffer[14], 0, 20);
        reply_buffer[14] = 0x45;
        reply_buffer[15] = 0x00;
        put_net2(&reply_buffer[16], 20U+8U+udp_reply_size);
        put_net2(&reply_buffer[18], 1);
        reply_buffer[20] = 0x00;
        reply_buffer[21] = 0x00;
        reply_buffer[22] = 0x07; // TTL
        reply_buffer[23] = 0x11; // UDP
        // host-to-guest IPv4
        reply_buffer[14] = (reply_buffer[14] & 0x0f) | 0x40;
        l3header_len = ((unsigned)(reply_buffer[14] & 0x0f) << 2);
        memcpy(&reply_buffer[26], &dhcp.host_ipv4addr[0], 4);
        memcpy(&reply_buffer[30], &dhcp.guest_ipv4addr[0], 4);
        put_net2(&reply_buffer[24], 0);
        put_net2(&reply_buffer[24], ip_checksum(&reply_buffer[14], l3header_len) ^ (Bit16u)0xffff);
        prepare_builtin_reply(ETHERNET_TYPE_IPV4);
      }
      // don't forward DHCP / TFTP requests to Slirp
      return 1;
    }
  }
  return 0;
}

void bx_slirp_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  size_t len;
  ethernet_header_t *ethhdr = (ethernet_header_t *)buf;

#if BX_ETH_SLIRP_LOGGING
  write_pktlog_txt(pktlog_txt, (Bit8u*)buf, io_len, 0);
#endif
  this->tx_time = (64 + 96 + 4 * 8 + io_len * 8) / this->netdev_speed;
  switch (ntohs(ethhdr->type)) {
    case ETHERNET_TYPE_IPV4: 
      if (!handle_ipv4((Bit8u*)buf, io_len)) {
        len = encode_slip((Bit8u *)buf+sizeof(ethernet_header_t),
                          slip_output_buffer,
                          io_len-sizeof(ethernet_header_t));
        len = write(slirp_pipe_fds[0], slip_output_buffer, len);
      }
      break;
    case ETHERNET_TYPE_ARP:
      handle_arp((Bit8u*)buf, io_len);
      break;
    default:
      break;
  }
}

void bx_slirp_pktmover_c::prepare_builtin_reply(unsigned type)
{
  ethernet_header_t *ethhdr;
  unsigned rx_time;

  ethhdr = (ethernet_header_t *)reply_buffer;
  memcpy(ethhdr->dst_mac_addr, dhcp.guest_macaddr, ETHERNET_MAC_ADDR_LEN);
  memcpy(ethhdr->src_mac_addr, dhcp.host_macaddr, ETHERNET_MAC_ADDR_LEN);
  ethhdr->type = htons(type);
  if (pending_reply_size < 60) {
    memset(&reply_buffer[pending_reply_size], 0, 60 - pending_reply_size);
    pending_reply_size = 60;
  }
  rx_time = (64 + 96 + 4 * 8 + pending_reply_size * 8) / this->netdev_speed;
  bx_pc_system.activate_timer(this->rx_timer_index, this->tx_time + rx_time + 100, 0);
}

void bx_slirp_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_slirp_pktmover_c *class_ptr = (bx_slirp_pktmover_c *)this_ptr;
  class_ptr->rx_timer();
}

void bx_slirp_pktmover_c::rx_timer()
{
  Bit8u *packet;
  Bit8u padded_packet[MIN_RX_PACKET_LEN];
  ethernet_header_t *ethhdr;
  size_t packet_len;

  if (pending_reply_size > 0) {
#if BX_ETH_SLIRP_LOGGING
    write_pktlog_txt(pktlog_txt, reply_buffer, pending_reply_size, 1);
#endif
    if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
      this->rxh(this->netdev, reply_buffer, pending_reply_size);
    } else {
      BX_ERROR(("device not ready to receive data"));
    }
    pending_reply_size = 0;
    // restore timer
    bx_pc_system.activate_timer(this->rx_timer_index, 1000, 1);
    return;
  }

  Bit8u *buf = slip_input_buffer + sizeof(ethernet_header_t);
  ssize_t n;
  int got_packet;
  size_t ilen, olen, pos;

  if (slip_input_buffer_filled + sizeof(ethernet_header_t) <
      sizeof(slip_input_buffer)) {
    n = read(slirp_pipe_fds[0], buf+slip_input_buffer_filled,
             (sizeof(slip_input_buffer) - sizeof(ethernet_header_t)) -
             slip_input_buffer_filled);
    if (n < 1)
      return;
    slip_input_buffer_filled += n;
  }

  packet = (Bit8u *)slip_input_buffer;

  ethhdr = (ethernet_header_t *)packet;
  memcpy(ethhdr->dst_mac_addr, dhcp.guest_macaddr, ETHERNET_MAC_ADDR_LEN);
  memcpy(ethhdr->src_mac_addr, dhcp.host_macaddr, ETHERNET_MAC_ADDR_LEN);
  ethhdr->type = htons(ETHERNET_TYPE_IPV4);

  olen = 0;
  pos = slip_input_buffer_decoded; 
  do {
    ilen = slip_input_buffer_filled - slip_input_buffer_decoded;
    got_packet = decode_slip(buf + pos, &ilen, 
                             buf + slip_input_buffer_decoded, &olen);
    pos += ilen;
    slip_input_buffer_filled -= ilen;
    slip_input_buffer_filled += olen;
    slip_input_buffer_decoded += olen;
    if (got_packet) {
      packet_len = sizeof(ethernet_header_t) + slip_input_buffer_decoded;
      if (packet_len < MIN_RX_PACKET_LEN) {
        packet = padded_packet;
        bzero(packet, MIN_RX_PACKET_LEN);
        memcpy(packet, slip_input_buffer, packet_len);
        packet_len = MIN_RX_PACKET_LEN;
      }
#if BX_ETH_SLIRP_LOGGING
      write_pktlog_txt(pktlog_txt, packet, packet_len, 1);
#endif
      (*rxh)(this->netdev, packet, packet_len);
      slip_input_buffer_filled -= slip_input_buffer_decoded;
      slip_input_buffer_decoded = 0;
    }
  } while (got_packet);

  if ((slip_input_buffer_filled - slip_input_buffer_decoded) > 0)
    memmove(slip_input_buffer + slip_input_buffer_decoded,
            slip_input_buffer + pos, 
            (slip_input_buffer_filled - slip_input_buffer_decoded));
}

#endif /* if BX_NETWORKING && BX_NETMOD_SLIRP */
