
#ifndef _ETH_PACKETMAKER_H_
#define _ETH_PACKETMAKER_H_

#include "../config.h"

#define PACKET_BUF_SIZE 2048
static const Bit8u internal_mac[]={0xB0, 0xC4, 0x20, 0x20, 0x00, 0x00, 0x00};
static const Bit8u external_mac[]={0xB0, 0xC4, 0x20, 0x20, 0x00, 0x00, 0x00};
static const Bit8u external_ip[]={ 192, 168, 0, 2, 0x00 };
static const Bit8u broadcast_mac[]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
static const Bit8u ethtype_arp[]={0x08, 0x06, 0x00};
static const Bit8u ethtype_ip[]={0x08, 0x00, 0x00};


class eth_packet {
public:
  Bit8u buf[PACKET_BUF_SIZE];
  Bit32u len;
};


class eth_packetmaker {
public:
  virtual Boolean getpacket(eth_packet& inpacket) = 0;
  virtual Boolean ishandler(const eth_packet& outpacket) = 0;
  virtual Boolean sendpacket(const eth_packet& outpacket) = 0;
};


class eth_ARPmaker : public eth_packetmaker {
public:
  void init(void);
  Boolean ishandler(const eth_packet& outpacket);
  Boolean sendpacket(const eth_packet& outpacket);
  Boolean getpacket(eth_packet& inpacket);
private:
  eth_packet pending;
  Boolean is_pending;
};


class eth_IPmaker : eth_packetmaker {
public:
  void init(void);
  virtual Boolean ishandler(const eth_packet& outpacket)=0;
  virtual Boolean sendpacket(const eth_packet& outpacket)=0;
  virtual Boolean getpacket(eth_packet& inpacket)=0;

protected:
  Boolean sendable(const eth_packet& outpacket);

  Bit32u source(const eth_packet& outpacket);
  Bit32u destination(const eth_packet& outpacket);
  Bit8u protocol(const eth_packet& outpacket);

  const Bit8u * datagram(const eth_packet& outpacket);
  Bit32u datalen(const eth_packet& outpacket);

  //Build a header in pending, return header length in bytes.
  Bit32u build_packet_header(Bit32u source, Bit32u dest, Bit8u protocol, Bit32u datalen);

  eth_packet pending;
  Boolean is_pending;

  //Bit8u Version; //=4 (4 bits)
  //It better be!

  //Bit8u IHL; //Header length in 32-bit bytes (4 bits)
  //Used to strip layer

  //Bit8u Type_of_Service; //not relevent, set to 0;
  //Ignore on receive, set to 0 on send.

  //Bit16u Total_Length;//length of the datagram in octets. use 576 or less;
  //Use 576 or less on send.
  //Use to get length on receive

  //Bit16u Identification;//Identifier for assembling fragments
  //Ignore, we'll drop fragments

  //Bit8u Flags;//0,Don't fragment, More Fragments (vs last fragment)
  //Set to 0 on send
  //Drop if more fragments set.

  //Bit16u Fragment Offset;//where in the datagram this fragment belongs
  //Should be 0 for send and receive.

  //Bit8u TTL;//Set to something sorta big.
  //Shouldn't be 0 on receive
  //Set to something big on send

  //Bit8u Protocol;
  //Defines Protocol.
  //TCP=?, UDP=?

  //Bit16u Header_Checksum;//16-bit one's complement of the one's complement
		  //sum of all 16-bit words in header;
  //Could check on receive, must set on send.

  //Bit32u Source;//source address
  //Bit32u Destination;//destination address
};

/*
class eth_TCPmaker : eth_packetmaker {
};

class eth_UDPmaker : eth_packetmaker {
};
*/

class eth_ETHmaker : public eth_packetmaker {
public:
  //handles all packets to a MAC addr.
  void init(void);
  virtual Boolean getpacket(eth_packet& inpacket);
  virtual Boolean ishandler(const eth_packet& outpacket);
  virtual Boolean sendpacket(const eth_packet& outpacket);
private:
  eth_ARPmaker arper;
};


#endif // _ETH_PACKETMAKER_H_

