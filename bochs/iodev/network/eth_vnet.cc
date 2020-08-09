/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2020  The Bochs Project
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

// virtual Ethernet locator
//
// An implementation of ARP, ping(ICMP-echo), DHCP and read/write FTP and TFTP.
// Virtual host acts as a DHCP server for guest.
// Limited DNS server for 'vnet' and the client only.
// There are no connections between the virtual host and real ethernets.
//
// Virtual host name:     vnet
// Virtual server IP:     192.168.10.1
// Virtual DNS server IP: 192.168.10.2
// Virtual FTP server IP: 192.168.10.3
// Guest IP:              192.168.10.15
// Guest netmask:         255.255.255.0
// Guest broadcast:       192.168.10.255
// FTP and TFTP server using the ethdev value for the root directory
// TFTP doesn't overwrite files

#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"
#include "netutil.h"

#if BX_NETWORKING

static unsigned int bx_vnet_instances = 0;

// network driver plugin entry points

int CDECL libvnet_net_plugin_init(plugin_t *plugin, plugintype_t type)
{
  return 0; // Success
}

void CDECL libvnet_net_plugin_fini(void)
{
  // Nothing here yet
}

// network driver implementation

#define LOG_THIS netdev->

#define BX_ETH_VNET_PCAP_LOGGING 0

#if BX_ETH_VNET_PCAP_LOGGING
#include <pcap.h>
#endif

/////////////////////////////////////////////////////////////////////////
// handler to send/receive packets
/////////////////////////////////////////////////////////////////////////

static const Bit8u default_host_ipv4addr[4] = {192,168,10,1};
static const Bit8u default_dns_ipv4addr[4] = {192,168,10,2};
static const Bit8u default_ftp_ipv4addr[4] = {192,168,10,3};
static const Bit8u dhcp_base_ipv4addr[4] = {192,168,10,15};
static const char default_bootfile[] = "pxelinux.0";

static Bit8u    packet_buffer[BX_PACKET_BUFSIZE];
static unsigned packet_len;

class bx_vnet_pktmover_c : public eth_pktmover_c {
public:
  bx_vnet_pktmover_c();
  virtual ~bx_vnet_pktmover_c();
  void pktmover_init(
    const char *netif, const char *macaddr,
    eth_rx_handler_t rxh, eth_rx_status_t rxstat,
    bx_devmodel_c *dev, const char *script);
  void sendpkt(void *buf, unsigned io_len);
private:
  void guest_to_host(const Bit8u *buf, unsigned io_len);
  void host_to_guest(void);

  vnet_server_c vnet_server;

  dhcp_cfg_t dhcp;

  static void rx_timer_handler(void *);
  void rx_timer(void);

  int rx_timer_index;
  bx_bool rx_timer_pending;
  unsigned netdev_speed;
  unsigned tx_time;

  bx_bool vnet_logging;
  FILE *pktlog_txt;
#if BX_ETH_VNET_PCAP_LOGGING
  pcap_t *pcapp;
  pcap_dumper_t *pktlog_pcap;
  struct pcap_pkthdr pcaphdr;
#endif // BX_ETH_VNET_PCAP_LOGGING
};

class bx_vnet_locator_c : public eth_locator_c {
public:
  bx_vnet_locator_c(void) : eth_locator_c("vnet") {}
protected:
  eth_pktmover_c *allocate(
      const char *netif, const char *macaddr,
      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
      bx_devmodel_c *dev, const char *script) {
    bx_vnet_pktmover_c *pktmover;
    pktmover = new bx_vnet_pktmover_c();
    pktmover->pktmover_init(netif, macaddr, rxh, rxstat, dev, script);
    return pktmover;
  }
} bx_vnet_match;


bx_vnet_pktmover_c::bx_vnet_pktmover_c()
{
}

void bx_vnet_pktmover_c::pktmover_init(
  const char *netif, const char *macaddr,
  eth_rx_handler_t rxh, eth_rx_status_t rxstat,
  bx_devmodel_c *dev, const char *script)
{
  if (bx_vnet_instances > 0) {
    BX_PANIC(("only one 'vnet' instance supported yet"));
  }
  this->netdev = dev;
  this->rxh    = rxh;
  this->rxstat = rxstat;

  memcpy(&dhcp.host_macaddr[0], macaddr, 6);
  dhcp.host_macaddr[5] ^= 0x03;

  memcpy(dhcp.srv_ipv4addr[VNET_SRV], default_host_ipv4addr, 4);
  memcpy(dhcp.srv_ipv4addr[VNET_DNS], default_dns_ipv4addr, 4);
  memcpy(dhcp.srv_ipv4addr[VNET_MISC], default_ftp_ipv4addr, 4);
  memcpy(dhcp.client_base_ipv4addr, dhcp_base_ipv4addr, 4);
  strcpy(dhcp.bootfile, default_bootfile);
  vnet_server.init(dev, &dhcp, netif);
  vnet_server.init_client(0, (Bit8u*)macaddr);

  Bit32u status = this->rxstat(this->netdev) & BX_NETDEV_SPEED;
  this->netdev_speed = (status == BX_NETDEV_1GBIT) ? 1000 :
                       (status == BX_NETDEV_100MBIT) ? 100 : 10;
  this->rx_timer_index =
    DEV_register_timer(this, this->rx_timer_handler, 1000, 0, 0, "eth_vnet");
  rx_timer_pending = 0;

  BX_INFO(("'vnet' network driver initialized"));
  bx_vnet_instances++;

  if ((strlen(script) > 0) && (strcmp(script, "none"))) {
    pktlog_txt = fopen(script, "wb");
    vnet_logging = (pktlog_txt != NULL);
  } else {
    vnet_logging = 0;
  }
  if (vnet_logging) {
    fprintf(pktlog_txt, "vnet packetmover readable log file\n");
    fprintf(pktlog_txt, "TFTP root = %s\n", netif);
    fprintf(pktlog_txt, "host MAC address = ");
    int i;
    for (i=0; i<6; i++)
      fprintf(pktlog_txt, "%02x%s", 0xff & dhcp.host_macaddr[i], i<5?":" : "\n");
    fprintf(pktlog_txt, "guest MAC address = ");
    for (i=0; i<6; i++)
      fprintf(pktlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "\n");
    fprintf(pktlog_txt, "--\n");
    fflush(pktlog_txt);
  }
#if BX_ETH_VNET_PCAP_LOGGING
  pcapp = pcap_open_dead(DLT_EN10MB, BX_PACKET_BUFSIZE);
  pktlog_pcap = pcap_dump_open(pcapp, "vnet-pktlog.pcap");
  if (pktlog_pcap == NULL) BX_PANIC(("vnet-pktlog.pcap failed"));
#endif
}

bx_vnet_pktmover_c::~bx_vnet_pktmover_c()
{
  if (vnet_logging) {
    fclose(pktlog_txt);
  }
  bx_vnet_instances--;
}

void bx_vnet_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  guest_to_host((const Bit8u *)buf,io_len);
}

void bx_vnet_pktmover_c::guest_to_host(const Bit8u *buf, unsigned io_len)
{
  if (vnet_logging) {
    write_pktlog_txt(pktlog_txt, buf, io_len, 0);
  }
#if BX_ETH_VNET_PCAP_LOGGING
  if (pktlog_pcap && !ferror((FILE *)pktlog_pcap)) {
    Bit64u time = bx_pc_system.time_usec();
    pcaphdr.ts.tv_usec = time % 1000000;
    pcaphdr.ts.tv_sec = time / 1000000;
    pcaphdr.caplen = io_len;
    pcaphdr.len = io_len;
    pcap_dump((u_char *)pktlog_pcap, &pcaphdr, buf);
    fflush((FILE *)pktlog_pcap);
  }
#endif

  this->tx_time = (64 + 96 + 4 * 8 + io_len * 8) / this->netdev_speed;
  vnet_server.handle_packet(buf, io_len);

  host_to_guest();
}

void bx_vnet_pktmover_c::host_to_guest(void)
{
  if (!rx_timer_pending) {
    packet_len = vnet_server.get_packet(packet_buffer);
    if (packet_len > 0) {
      unsigned rx_time = (64 + 96 + 4 * 8 + packet_len * 8) / this->netdev_speed;
      bx_pc_system.activate_timer(this->rx_timer_index, this->tx_time + rx_time + 100, 0);
      rx_timer_pending = 1;
    }
  }
}

// The receive poll process
void bx_vnet_pktmover_c::rx_timer_handler(void *this_ptr)
{
  bx_vnet_pktmover_c *class_ptr = (bx_vnet_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void bx_vnet_pktmover_c::rx_timer(void)
{
  if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
    this->rxh(this->netdev, (void *)packet_buffer, packet_len);
    if (vnet_logging) {
      write_pktlog_txt(pktlog_txt, packet_buffer, packet_len, 1);
    }
#if BX_ETH_VNET_PCAP_LOGGING
    if (pktlog_pcap && !ferror((FILE *)pktlog_pcap)) {
      Bit64u time = bx_pc_system.time_usec();
      pcaphdr.ts.tv_usec = time % 1000000;
      pcaphdr.ts.tv_sec = time / 1000000;
      pcaphdr.caplen = packet_len;
      pcaphdr.len = packet_len;
      pcap_dump((u_char *)pktlog_pcap, &pcaphdr, packet_buffer);
      fflush((FILE *)pktlog_pcap);
    }
#endif
    rx_timer_pending = 0;
    // check for another pending packet
    host_to_guest();
  } else {
    BX_ERROR(("device not ready to receive data"));
  }
}

#endif /* if BX_NETWORKING */
