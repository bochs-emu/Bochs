#include "eth_packetmaker.h"
#include "bochs.h"

void
eth_ETHmaker::init(void) {
  arper.init();
}

Boolean
eth_ETHmaker::getpacket(eth_packet& inpacket) {
  return arper.getpacket(inpacket);
}

Boolean
eth_ETHmaker::ishandler(const eth_packet& outpacket) {
  if((outpacket.len>=60) &&
     ( (!memcmp(outpacket.buf, external_mac, 6))
       || (!memcmp(outpacket.buf, broadcast_mac, 6)) ) &&
     ( (!memcmp(outpacket.buf+12, ethtype_arp, 2)) ||
       (!memcmp(outpacket.buf+12, ethtype_ip, 2)) ) &&
     (outpacket.len<PACKET_BUF_SIZE)
     ) {
    return 1;
  }
  return 0;  
}

Boolean
eth_ETHmaker::sendpacket(const eth_packet& outpacket) {
  return arper.sendpacket(outpacket);
}



void
eth_ARPmaker::init(void) {
  is_pending=0;
  pending.len=0;
}

Boolean
eth_ARPmaker::getpacket(eth_packet& inpacket) {
  if(is_pending) {
    memcpy(inpacket.buf,pending.buf,pending.len);
    inpacket.len=pending.len;
    is_pending=0;
    return 1;
  }
  return 0;
}

Boolean
eth_ARPmaker::ishandler(const eth_packet& outpacket) {
  if((outpacket.len>=60) &&
     (!memcmp(outpacket.buf+12, ethtype_arp, 2)) &&
     (outpacket.len<PACKET_BUF_SIZE) &&
     ( (!memcmp(outpacket.buf, external_mac, 6))
       || (!memcmp(outpacket.buf, broadcast_mac, 6)) )
     ) {
    return 1;
  }
  return 0;
}

Boolean
eth_ARPmaker::sendpacket(const eth_packet& outpacket) {
  if(is_pending || !ishandler(outpacket)) {
    return 0;
  } else {
    Bit32u tempcrc;
    memcpy(pending.buf,outpacket.buf,outpacket.len); //move to temporary buffer
    memcpy(pending.buf, pending.buf+6, 6); //set destination to sender
    memcpy(pending.buf+6, external_mac, 6); //set sender to us
    memcpy(pending.buf+32, pending.buf+22, 10); //move destination to sender
    memcpy(pending.buf+22, external_mac, 6); //set sender to us
    memcpy(pending.buf+28, external_ip, 4); //set sender to us
    pending.buf[21]=2; //make this a reply and not a request
    //tempcrc=mycrc.get_CRC(pending.buf,len);
    //memcpy(pending.buf+len, &tempcrc, 4);
    pending.len=outpacket.len; //+4
    is_pending=1;
    return 1;
  }
}



