/////////////////////////////////////////////////////////////////////////
// $Id: eth_arpback.cc,v 1.6 2001-10-03 13:10:38 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// eth_arpback.cc  - basic ethernet packetmover, only responds to ARP

// Various networking docs:
// http://www.graphcomp.com/info/rfc/
// rfc0826: arp
// rfc0903: rarp

#include "bochs.h"

#ifdef ETH_ARPBACK

#include "crc32.h"
#include "eth_packetmaker.h"
#define LOG_THIS bx_ne2k.


//static const Bit8u external_mac[]={0xB0, 0xC4, 0x20, 0x20, 0x00, 0x00, 0x00};
//static const Bit8u internal_mac[]={0xB0, 0xC4, 0x20, 0x00, 0x00, 0x00, 0x00};
//static const Bit8u external_ip[]={ 192, 168, 0, 2, 0x00 };
//static const Bit8u broadcast_mac[]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
//static const Bit8u ethtype_arp[]={0x08, 0x06, 0x00};

#define MAX_FRAME_SIZE 2048

//
//  Define the class. This is private to this module
//
class bx_arpback_pktmover_c : public eth_pktmover_c {
public:
  bx_arpback_pktmover_c(const char *netif, const char *macaddr,
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
private:
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer(void);
  FILE *txlog, *txlog_txt;
  //Bit8u arpbuf[MAX_FRAME_SIZE];
  //Bit32u buflen;
  //Boolean bufvalid;
  //CRC_Generator mycrc;
  eth_ETHmaker packetmaker;
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_arpback_locator_c : public eth_locator_c {
public:
  bx_arpback_locator_c(void) : eth_locator_c("arpback") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
			   eth_rx_handler_t rxh,
			   void *rxarg) {
    return (new bx_arpback_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_arpback_match;


//
// Define the methods for the bx_arpback_pktmover derived class
//

// the constructor
bx_arpback_pktmover_c::bx_arpback_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
  this->rx_timer_index = 
    bx_pc_system.register_timer(this, this->rx_timer_handler, 1000,
				1, 1); // continuous, active
  this->rxh   = rxh;
  this->rxarg = rxarg;
  //bufvalid=0;
  packetmaker.init();
#if BX_ETH_NULL_LOGGING
  // Start the rx poll 
  // eventually Bryce wants txlog to dump in pcap format so that
  // tcpdump -r FILE can read it and interpret packets.
  txlog = fopen ("ne2k-tx.log", "wb");
  if (!txlog) BX_PANIC (("open ne2k-tx.log failed"));
  txlog_txt = fopen ("ne2k-txdump.txt", "wb");
  if (!txlog_txt) BX_PANIC (("open ne2k-txdump.txt failed"));
  fprintf (txlog_txt, "arpback packetmover readable log file\n");
  fprintf (txlog_txt, "net IF = %s\n", netif);
  fprintf (txlog_txt, "MAC address = ");
  for (int i=0; i<6; i++) 
    fprintf (txlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "");
  fprintf (txlog_txt, "\n--\n");
  fflush (txlog_txt);
#endif
}

void
bx_arpback_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
  if(io_len<MAX_FRAME_SIZE) {
    eth_packet barney;
    memcpy(barney.buf,buf,io_len);
    barney.len=io_len;
    if(packetmaker.ishandler(barney)) {
      packetmaker.sendpacket(barney);
    }
    /*
    if(( (!memcmp(buf, external_mac, 6)) || (!memcmp(buf, broadcast_mac, 6)) )
       && (!memcmp(((Bit8u *)buf)+12, ethtype_arp, 2)) ) {
      Bit32u tempcrc;
      memcpy(arpbuf,buf,io_len); //move to temporary buffer
      memcpy(arpbuf, arpbuf+6, 6); //set destination to sender
      memcpy(arpbuf+6, external_mac, 6); //set sender to us
      memcpy(arpbuf+32, arpbuf+22, 10); //move destination to sender
      memcpy(arpbuf+22, external_mac, 6); //set sender to us
      memcpy(arpbuf+28, external_ip, 4); //set sender to us
      arpbuf[21]=2; //make this a reply and not a request
      tempcrc=mycrc.get_CRC(arpbuf,io_len);
      memcpy(arpbuf+io_len, &tempcrc, 4);
      buflen=io_len;//+4
      bufvalid=1;
    }
    */
  }
#if BX_ETH_NULL_LOGGING
  BX_DEBUG (("sendpkt length %u", io_len));
  // dump raw bytes to a file, eventually dump in pcap format so that
  // tcpdump -r FILE can interpret them for us.
  int n = fwrite (buf, io_len, 1, txlog);
  if (n != 1) BX_ERROR (("fwrite to txlog failed", io_len));
  // dump packet in hex into an ascii log file
  fprintf (txlog_txt, "NE2K transmitting a packet, length %u\n", io_len);
  Bit8u *charbuf = (Bit8u *)buf;
  for (n=0; n<io_len; n++) {
    if (((n % 16) == 0) && n>0)
      fprintf (txlog_txt, "\n");
    fprintf (txlog_txt, "%02x ", charbuf[n]);
  }
  fprintf (txlog_txt, "\n--\n");
  // flush log so that we see the packets as they arrive w/o buffering
  fflush (txlog);
  fflush (txlog_txt);
#endif
}

void bx_arpback_pktmover_c::rx_timer_handler (void * this_ptr)
{
#if BX_ETH_NULL_LOGGING
  BX_DEBUG (("rx_timer_handler"));
#endif
  bx_arpback_pktmover_c *class_ptr = ((bx_arpback_pktmover_c *)this_ptr);

  class_ptr->rx_timer();
}

void bx_arpback_pktmover_c::rx_timer (void)
{
  int nbytes = 0;
  struct bpf_hdr *bhdr;
  eth_packet rubble;
  
  if(packetmaker.getpacket(rubble)) {
    //bufvalid=0;
    void * buf=rubble.buf;
    unsigned io_len=rubble.len;
    Bit32u n;
    fprintf (txlog_txt, "NE2K receiving a packet, length %u\n", io_len);
    Bit8u *charbuf = (Bit8u *)buf;
    for (n=0; n<io_len; n++) {
      if (((n % 16) == 0) && n>0)
	fprintf (txlog_txt, "\n");
      fprintf (txlog_txt, "%02x ", charbuf[n]);
    }
    fprintf (txlog_txt, "\n--\n");
    fflush (txlog_txt);

    (*rxh)(rxarg, buf, io_len);
  }
}

#endif

