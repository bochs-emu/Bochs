/////////////////////////////////////////////////////////////////////////
// $Id: eth_packetmaker.cc,v 1.5 2001-10-03 13:10:38 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
#include "bochs.h"

#ifdef ETH_ARPBACK

#include "eth_packetmaker.h"


Boolean sendable(const eth_packet& outpacket) {
  //FINISH ME!
}

Bit32u eth_IPmaker::datalen(const eth_packet& outpacket) {
  Bit32u out;
  out=((outpacket.buf[16]<<8)+outpacket.buf[17])-(4*(0xF & outpacket.buf[14]));
  return out;
}

const Bit8u * eth_IPmaker::datagram(const eth_packet& outpacket) {
  const Bit8u * out;
  out=outpacket.buf+14+(4*(0xF & outpacket.buf[14]));
  return out;
}

Bit32u eth_IPmaker::build_packet_header(Bit32u source, Bit32u dest, Bit8u protocol, Bit32u datalen) {
  Bit32u temp;
  Bit32u i;
  memcpy(pending.buf,internal_mac,6);
  memcpy(pending.buf+6,external_mac,6);
  memcpy(pending.buf+12,ethtype_ip,2);
  pending.buf[14]=0x45;
  pending.buf[15]=0;
  temp=datalen+20;
  pending.buf[16]=(temp>>8) & 0xFF;
  pending.buf[17]=temp & 0xFF;
  pending.buf[18]=0;
  pending.buf[19]=0;
  pending.buf[20]=0;
  pending.buf[21]=0;
  pending.buf[22]=30;
  pending.buf[23]=protocol;
  pending.buf[24]=0;
  pending.buf[25]=0;
  pending.buf[26]=(source>>24) & 0xFF;
  pending.buf[27]=(source>>16) & 0xFF;
  pending.buf[28]=(source>>8) & 0xFF;
  pending.buf[29]=(source) & 0xFF;
  pending.buf[30]=(dest>>24) & 0xFF;
  pending.buf[31]=(dest>>16) & 0xFF;
  pending.buf[32]=(dest>>8) & 0xFF;
  pending.buf[33]=(dest) & 0xFF;
  //Compute Checksum
  temp=0;
  for(i=14;i<34;i=i+2) {
    Bit32u addee=pending.buf[i];
    addee=(addee<<8) & pending.buf[i+1];
    temp=temp+addee;
  }
  temp=(temp>>16)+(temp&0xFFFF);
  temp=(temp>>16)+(temp&0xFFFF);
  pending.buf[24]=~(Bit8u)((temp>>8) & 0xFF);
  pending.buf[25]=~(Bit8u)(temp & 0xFF);
  return(34);
}

Bit8u eth_IPmaker::protocol(const eth_packet& outpacket) {
  Bit8u out;
  out=0xFF & *(outpacket.buf+23);
}

Bit32u eth_IPmaker::source(const eth_packet& outpacket) {
  Bit32u out;
  out=0xFF & *(outpacket.buf+26);
  out=(out<<8) | (0xFF & *(outpacket.buf+27));
  out=(out<<8) | (0xFF & *(outpacket.buf+28));
  out=(out<<8) | (0xFF & *(outpacket.buf+29));
  return out;
}

Bit32u eth_IPmaker::destination(const eth_packet& outpacket) {
  Bit32u out;
  out=0xFF & *(outpacket.buf+30);
  out=(out<<8) | (0xFF & *(outpacket.buf+31));
  out=(out<<8) | (0xFF & *(outpacket.buf+32));
  out=(out<<8) | (0xFF & *(outpacket.buf+33));
  return out;
}

void eth_IPmaker::init(void) {
  is_pending=0;
}

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
       || (!memcmp(outpacket.buf, broadcast_mac, 6)) ) &&
     (!memcmp(outpacket.buf+38, external_ip, 4))
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

#endif

