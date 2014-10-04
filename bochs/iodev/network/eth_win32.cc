/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// eth_win32.cc  - packet mover for win32
// All win32 coding by Don Becker <x-odus@iname.com>
// with patches from various sources
//
// Various networking docs:
// http://www.graphcomp.com/info/rfc/
// rfc0826: arp
// rfc0903: rarp
//
// For ethernet support under win32 to work, you must install WinPCap.
// Download it from http://netgroup-serv.polito.it/winpcap

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "netmod.h"

#if BX_NETWORKING && BX_NETMOD_WIN32

#ifndef __CYGWIN__
#include <winsock2.h>
#endif

// windows.h included by bochs.h
#define LOG_THIS netdev->

#define BX_ETH_WIN32_LOGGING 0

#define NDIS_PACKET_TYPE_PROMISCUOUS			0x0020
#define Packet_ALIGNMENT sizeof(int)
#define Packet_WORDALIGN(x) (((x)+(Packet_ALIGNMENT-1))&~(Packet_ALIGNMENT-1))

typedef	int bpf_int32;
typedef	u_int bpf_u_int32;

/*
 * The instruction encondings.
 */

/* instruction classes */

#define BPF_CLASS(code) ((code) & 0x07)
#define		BPF_LD		0x00
#define		BPF_LDX		0x01
#define		BPF_ST		0x02
#define		BPF_STX		0x03
#define		BPF_ALU		0x04
#define		BPF_JMP		0x05
#define		BPF_RET		0x06
#define		BPF_MISC	0x07

/* ld/ldx fields */
#define BPF_SIZE(code)	((code) & 0x18)
#define		BPF_W		0x00
#define		BPF_H		0x08
#define		BPF_B		0x10
#define BPF_MODE(code)	((code) & 0xe0)
#define		BPF_IMM 	0x00
#define		BPF_ABS		0x20
#define		BPF_IND		0x40
#define		BPF_MEM		0x60
#define		BPF_LEN		0x80
#define		BPF_MSH		0xa0

/* alu/jmp fields */
#define BPF_OP(code)	((code) & 0xf0)
#define		BPF_ADD		0x00
#define		BPF_SUB		0x10
#define		BPF_MUL		0x20
#define		BPF_DIV		0x30
#define		BPF_OR		0x40
#define		BPF_AND		0x50
#define		BPF_LSH		0x60
#define		BPF_RSH		0x70
#define		BPF_NEG		0x80
#define		BPF_JA		0x00
#define		BPF_JEQ		0x10
#define		BPF_JGT		0x20
#define		BPF_JGE		0x30
#define		BPF_JSET	0x40
#define BPF_SRC(code)	((code) & 0x08)
#define		BPF_K		0x00
#define		BPF_X		0x08

/* ret - BPF_K and BPF_X also apply */
#define BPF_RVAL(code)	((code) & 0x18)
#define		BPF_A		0x10

/* misc */
#define BPF_MISCOP(code) ((code) & 0xf8)
#define		BPF_TAX		0x00
#define		BPF_TXA		0x80

#define BPF_STMT(code, k) { (u_short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) { (u_short)(code), jt, jf, k }

/*
 * The instruction data structure.
 */
struct bpf_insn {
	u_short	code;
	u_char 	jt;
	u_char 	jf;
	bpf_int32 k;
};

struct bpf_program {
	u_int bf_len;
	struct bpf_insn *bf_insns;
};

struct bpf_hdr {
	struct timeval	bh_tstamp;	/* time stamp */
	UINT	bh_caplen;	/* length of captured portion */
	UINT	bh_datalen;	/* original length of packet */
	USHORT		bh_hdrlen;	/* length of bpf header (this struct
					   plus alignment padding) */
};

#define MAX_LINK_NAME_LENGTH 64

// Why don't these definitions come from including winpcap.h or something?
// -Bryce
typedef struct _ADAPTER {
	HANDLE hFile;
	TCHAR  SymbolicLink[MAX_LINK_NAME_LENGTH];
	int NumWrites;
	HANDLE ReadEvent;
	UINT ReadTimeOut; // WARNING: maybe invalid before winpcap 2.2
} ADAPTER, *LPADAPTER;

typedef struct _PACKET {
	HANDLE       hEvent;
	OVERLAPPED   OverLapped;
	PVOID        Buffer;
	UINT         Length;
	UINT         ulBytesReceived;
	BOOLEAN      bIoComplete;
} PACKET, *LPPACKET;

HINSTANCE hPacket;
LPADAPTER lpAdapter = 0;
LPPACKET  pkSend;
LPPACKET  pkRecv;
char      buffer[256000];
DWORD     dwVersion, dwMajorVersion;
char      AdapterList[10][1024];
char      cMacAddr[6];
char      NetDev[512];
BOOL      IsNT = FALSE;

typedef LPADAPTER (CDECL *POpenAdapter)     (LPTSTR);
typedef VOID      (CDECL *PCloseAdapter)    (LPADAPTER);
typedef BOOLEAN   (CDECL *PSetHwFilter)     (LPADAPTER, ULONG);
typedef BOOLEAN   (CDECL *PSetBpf)          (LPADAPTER, struct bpf_program *);
typedef BOOLEAN   (CDECL *PGetAdapterNames) (PTSTR, PULONG);
typedef BOOLEAN   (CDECL *PSendPacket)      (LPADAPTER, LPPACKET, BOOLEAN);
typedef BOOLEAN   (CDECL *PReceivePacket)   (LPADAPTER, LPPACKET, BOOLEAN);
typedef BOOLEAN   (CDECL *PSetBuff)         (LPADAPTER, int);
typedef BOOLEAN   (CDECL *PSetReadTimeout)  (LPADAPTER, int);
typedef LPPACKET  (CDECL *PAllocatePacket)  (void);
typedef VOID      (CDECL *PInitPacket)      (LPPACKET, PVOID, UINT);
typedef VOID      (CDECL *PFreePacket)      (LPPACKET);

POpenAdapter     PacketOpenAdapter;
PCloseAdapter    PacketCloseAdapter;
PSetHwFilter     PacketSetHwFilter;
PSetBpf          PacketSetBpf;
PGetAdapterNames PacketGetAdapterNames;
PSendPacket      PacketSendPacket;
PReceivePacket   PacketReceivePacket;
PSetBuff         PacketSetBuff;
PSetReadTimeout  PacketSetReadTimeout;
PAllocatePacket  PacketAllocatePacket;
PInitPacket      PacketInitPacket;
PFreePacket      PacketFreePacket;

// template filter for a unicast mac address and all
// multicast/broadcast frames
static const struct bpf_insn macfilter[] = {
  BPF_STMT(BPF_LD|BPF_W|BPF_ABS, 2),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0xaaaaaaaa, 0, 2),
  BPF_STMT(BPF_LD|BPF_H|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0x0000aaaa, 2, 0),
  BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 0),
  BPF_JUMP(BPF_JMP|BPF_JSET|BPF_K, 0x01, 0, 1),
  BPF_STMT(BPF_RET, 1514),
  BPF_STMT(BPF_RET, 0),
};

//
//  Define the class. This is private to this module
//
class bx_win32_pktmover_c : public eth_pktmover_c {
public:
  bx_win32_pktmover_c(const char *netif, const char *macaddr,
                      eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                      bx_devmodel_c *dev,
                      const char *script);
  void sendpkt(void *buf, unsigned io_len);
private:
  struct bpf_insn filter[8];
  int rx_timer_index;
  static void rx_timer_handler(void *);
  void rx_timer(void);
#if BX_ETH_WIN32_LOGGING
  FILE *pktlog_txt;
#endif
};

//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_win32_locator_c : public eth_locator_c {
public:
  bx_win32_locator_c(void) : eth_locator_c("win32") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr,
                           eth_rx_handler_t rxh, eth_rx_status_t rxstat,
                           bx_devmodel_c *dev, const char *script) {
    return (new bx_win32_pktmover_c(netif, macaddr, rxh, rxstat, dev, script));
  }
} bx_win32_match;

//
// Define the methods for the bx_win32_pktmover derived class
//

// the constructor
bx_win32_pktmover_c::bx_win32_pktmover_c(
  const char *netif, const char *macaddr,
  eth_rx_handler_t rxh, eth_rx_status_t rxstat,
  bx_devmodel_c *dev, const char *script)
{
  this->netdev = dev;
  BX_INFO(("win32 network driver"));
  // Open Packet Driver Here.
  DWORD dwVersion;
  DWORD dwWindowsMajorVersion;

  this->rxh    = rxh;
  this->rxstat = rxstat;

  hPacket = LoadLibrary("PACKET.DLL");
  memcpy(cMacAddr, macaddr, 6);
  if (hPacket) {
    PacketOpenAdapter     = (POpenAdapter)     GetProcAddress(hPacket, "PacketOpenAdapter");
    PacketCloseAdapter    = (PCloseAdapter)    GetProcAddress(hPacket, "PacketCloseAdapter");
    PacketSetHwFilter     = (PSetHwFilter)     GetProcAddress(hPacket, "PacketSetHwFilter");
    PacketSetBpf          = (PSetBpf)          GetProcAddress(hPacket, "PacketSetBpf");
    PacketGetAdapterNames = (PGetAdapterNames) GetProcAddress(hPacket, "PacketGetAdapterNames");
    PacketSendPacket      = (PSendPacket)      GetProcAddress(hPacket, "PacketSendPacket");
    PacketReceivePacket   = (PReceivePacket)   GetProcAddress(hPacket, "PacketReceivePacket");
    PacketSetBuff         = (PSetBuff)         GetProcAddress(hPacket, "PacketSetBuff");
    PacketSetReadTimeout  = (PSetReadTimeout)  GetProcAddress(hPacket, "PacketSetReadTimeout");
    PacketAllocatePacket  = (PAllocatePacket)  GetProcAddress(hPacket, "PacketAllocatePacket");
    PacketInitPacket      = (PInitPacket)      GetProcAddress(hPacket, "PacketInitPacket");
    PacketFreePacket      = (PFreePacket)      GetProcAddress(hPacket, "PacketFreePacket");
  } else {
    BX_PANIC(("Could not load WPCap Drivers for ethernet support!"));
  }

  memset(&NetDev, 0, sizeof(NetDev));
  dwVersion=GetVersion();
  dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
  if (!(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4))
  {  // Windows NT/2k
    int nLen = MultiByteToWideChar(CP_ACP, 0, netif, -1, NULL, 0);
    MultiByteToWideChar(CP_ACP, 0, netif, -1, (WCHAR *)NetDev, nLen);
    IsNT = TRUE;
  } else { // Win9x
    strcpy(NetDev, netif);
  }

  lpAdapter = PacketOpenAdapter(NetDev);
  if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE)) {
    BX_PANIC(("Could not open adapter for ethernet reception"));
    return;
  }
  PacketSetHwFilter(lpAdapter, NDIS_PACKET_TYPE_PROMISCUOUS);

/* The code below sets a BPF mac address filter
   that seems to really kill performance, for now
   im just using code to filter, and it works
   better
*/

//  memcpy(&this->filter, macfilter, sizeof(macfilter));
//  this->filter[1].k = (macaddr[2] & 0xff) << 24 | (macaddr[3] & 0xff) << 16 | (macaddr[4] & 0xff) << 8  | (macaddr[5] & 0xff);
//  this->filter[3].k = (macaddr[0] & 0xff) << 8 | (macaddr[1] & 0xff);
//  bp.bf_len   = 8;
//  bp.bf_insns = &this->filter[0];
//  if (!PacketSetBpf(lpAdapter, &bp)) {
//    BX_PANIC(("Could not set mac address BPF filter"));
//  }

  PacketSetBuff(lpAdapter, 512000);
  PacketSetReadTimeout(lpAdapter, -1);

  if ((pkSend = PacketAllocatePacket()) == NULL) {
    BX_PANIC(("Could not allocate a send packet"));
  }

  if ((pkRecv = PacketAllocatePacket()) == NULL) {
    BX_PANIC(("Could not allocate a recv packet"));
  }
  rx_timer_index =
    bx_pc_system.register_timer(this, this->rx_timer_handler, 10000, 1, 1, "eth_win32");

#if BX_ETH_WIN32_LOGGING
  pktlog_txt = fopen("ne2k-pktlog.txt", "wb");
  if (!pktlog_txt) BX_PANIC(("ne2k-pktlog.txt failed"));
  fprintf(pktlog_txt, "win32 packetmover readable log file\n");
  fprintf(pktlog_txt, "host adapter = %s\n", netif);
  fprintf(pktlog_txt, "guest MAC address = ");
  int i;
  for (i=0; i<6; i++)
    fprintf(pktlog_txt, "%02x%s", 0xff & macaddr[i], i<5?":" : "\n");
  fprintf(pktlog_txt, "--\n");
  fflush(pktlog_txt);
#endif
}

void bx_win32_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
#if BX_ETH_WIN32_LOGGING
  write_pktlog_txt(pktlog_txt, (const Bit8u *)buf, io_len, 0);
#endif

  // SendPacket Here.
  PacketInitPacket(pkSend, (char *)buf, io_len);

  if (!PacketSendPacket(lpAdapter, pkSend, TRUE)) {
    fprintf(stderr, "[ETH-WIN32] Error sending packet: %lu\n", GetLastError());
  }
}

void bx_win32_pktmover_c::rx_timer_handler (void *this_ptr)
{
  bx_win32_pktmover_c *class_ptr = (bx_win32_pktmover_c *) this_ptr;

  class_ptr->rx_timer();
}

void bx_win32_pktmover_c::rx_timer(void)
{
  // Recieve Packet ????
  char           *pBuf;
  unsigned char  *pPacket;
  unsigned int   iOffset = 0;
  struct bpf_hdr *hdr;
  int pktlen;

  PacketInitPacket(pkRecv, (char *)buffer, 256000);
  if (WaitForSingleObject(lpAdapter->ReadEvent,0) == WAIT_OBJECT_0 || IsNT) {
    PacketReceivePacket(lpAdapter, pkRecv, TRUE);
    pBuf = (char *)pkRecv->Buffer;
    iOffset = 0;
    while(iOffset < pkRecv->ulBytesReceived)
    {
      hdr = (struct bpf_hdr *)(pBuf + iOffset);
      pPacket = (unsigned char *)(pBuf + iOffset + hdr->bh_hdrlen);
      if (memcmp(pPacket + 6, cMacAddr, 6) != 0) // src field != ours
      {
        if(memcmp(pPacket, cMacAddr, 6) == 0 || memcmp(pPacket, broadcast_macaddr, 6) == 0)
        {
          pktlen = hdr->bh_caplen;
          if (pktlen < 60) pktlen = 60;
#if BX_ETH_WIN32_LOGGING
          write_pktlog_txt(pktlog_txt, pPacket, pktlen, 1);
#endif
          if (this->rxstat(this->netdev) & BX_NETDEV_RXREADY) {
            this->rxh(this->netdev, pPacket, pktlen);
          } else {
            BX_ERROR(("device not ready to receive data"));
          }
        }
      }
      iOffset = Packet_WORDALIGN(iOffset + (hdr->bh_hdrlen + hdr->bh_caplen));
    }
  }
}
#endif /* if BX_NETWORKING && BX_NETMOD_WIN32 */
