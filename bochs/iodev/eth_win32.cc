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

// eth_win32.cc  - skeleton code for an ethernet pktmover
// All win32 coding by Don Becker <x-odus@iname.com>
//
// Various networking docs:
// http://www.graphcomp.com/info/rfc/
// rfc0826: arp
// rfc0903: rarp
//

#include <windows.h>
#include "bochs.h"
#define LOG_THIS bx_ne2k.

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

typedef struct _ADAPTER {
	HANDLE hFile;
	TCHAR  SymbolicLink[MAX_LINK_NAME_LENGTH];
	int NumWrites;
	HANDLE ReadEvent;
} ADAPTER, *LPADAPTER;

typedef struct _PACKET {  
	HANDLE       hEvent;
    OVERLAPPED   OverLapped;
    PVOID        Buffer;
    UINT         Length;
	UINT         ulBytesReceived;
	BOOLEAN      bIoComplete;
}  PACKET, *LPPACKET;

HINSTANCE hPacket;
LPADAPTER lpAdapterRX = 0;
LPADAPTER lpAdapterTX = 0;
LPPACKET  pkSend;
LPPACKET  pkRecv;
char      buffer[256000];
DWORD     dwVersion, dwMajorVersion;
char      AdapterList[10][1024];
char      cMacAddr[6];
void      *rx_Arg;
eth_rx_handler_t rx_handler;

LPADAPTER (*PacketOpenAdapter)     (LPTSTR);
VOID      (*PacketCloseAdapter)    (LPADAPTER);
BOOLEAN   (*PacketSetHwFilter)     (LPADAPTER, ULONG);
BOOLEAN   (*PacketSetBpf)          (LPADAPTER, struct bpf_program *);
BOOLEAN   (*PacketGetAdapterNames) (PTSTR, PULONG);
BOOLEAN   (*PacketSendPacket)      (LPADAPTER, LPPACKET, BOOLEAN);
BOOLEAN   (*PacketReceivePacket)   (LPADAPTER, LPPACKET, BOOLEAN);
BOOLEAN   (*PacketSetBuff)         (LPADAPTER, int);
BOOLEAN   (*PacketSetReadTimeout)  (LPADAPTER, int);
LPPACKET  (*PacketAllocatePacket)  (void);
VOID      (*PacketInitPacket)      (LPPACKET, PVOID, UINT);
VOID      (*PacketFreePacket)      (LPPACKET);

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
		     eth_rx_handler_t rxh,
		     void *rxarg);
  void sendpkt(void *buf, unsigned io_len);
private: 
  struct bpf_insn filter[8];
  int rx_timer_index;
  static void rx_timer_handler(void *);
};


//
//  Define the static class that registers the derived pktmover class,
// and allocates one on request.
//
class bx_win32_locator_c : public eth_locator_c {
public:
  bx_win32_locator_c(void) : eth_locator_c("win32") {}
protected:
  eth_pktmover_c *allocate(const char *netif, const char *macaddr, eth_rx_handler_t rxh, void *rxarg) {
    return (new bx_win32_pktmover_c(netif, macaddr, rxh, rxarg));
  }
} bx_win32_match;


//
// Define the methods for the bx_win32_pktmover derived class
//

// the constructor
bx_win32_pktmover_c::bx_win32_pktmover_c(const char *netif, 
				       const char *macaddr,
				       eth_rx_handler_t rxh,
				       void *rxarg)
{
     // Open Packet Driver Here.
     int i = 0;
     WCHAR     AdapterNameW[512];
     char      AdapterNameA[512];  
     int       AdapterNum = 0;
     ULONG     AdapterLength;
     struct bpf_program bp;

     BX_INFO(("bx_win32_pktmover_c"));
     rx_Arg     = rxarg;
     rx_handler = rxh;
     hPacket = LoadLibrary("PACKET.DLL");

     memcpy(cMacAddr, macaddr, 6);

     if(hPacket) {
          PacketOpenAdapter     = (LPADAPTER (*)(LPTSTR))                          GetProcAddress(hPacket, "PacketOpenAdapter");
          PacketCloseAdapter    = (VOID      (*)(LPADAPTER))                       GetProcAddress(hPacket, "PacketCloseAdapter");
          PacketSetHwFilter     = (BOOLEAN   (*)(LPADAPTER, ULONG))                GetProcAddress(hPacket, "PacketSetHwFilter");
	  PacketSetBpf          = (BOOLEAN   (*)(LPADAPTER, struct bpf_program *)) GetProcAddress(hPacket, "PacketSetBpf");
          PacketGetAdapterNames = (BOOLEAN   (*)(PTSTR, PULONG))                   GetProcAddress(hPacket, "PacketGetAdapterNames");
          PacketSendPacket      = (BOOLEAN   (*)(LPADAPTER, LPPACKET, BOOLEAN))    GetProcAddress(hPacket, "PacketSendPacket");
          PacketReceivePacket   = (BOOLEAN   (*)(LPADAPTER, LPPACKET, BOOLEAN))    GetProcAddress(hPacket, "PacketReceivePacket");
          PacketSetBuff         = (BOOLEAN   (*)(LPADAPTER, int))                  GetProcAddress(hPacket, "PacketSetBuff");
          PacketSetReadTimeout  = (BOOLEAN   (*)(LPADAPTER, int))                  GetProcAddress(hPacket, "PacketSetReadTimeout");
          PacketAllocatePacket  = (LPPACKET  (*)(void))                            GetProcAddress(hPacket, "PacketAllocatePacket");
          PacketInitPacket      = (VOID      (*)(LPPACKET, PVOID, UINT))           GetProcAddress(hPacket, "PacketInitPacket");
          PacketFreePacket      = (VOID      (*)(LPPACKET))                        GetProcAddress(hPacket, "PacketFreePacket");
     } else {
          BX_PANIC(("Could not load WPCap Drivers for ethernet support!"));
     }

     AdapterLength = 512;

	dwVersion      = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if(!(dwVersion >= 0x80000000 && dwMajorVersion >= 4)) {
		// Windows NT/2k
		WCHAR *tempW, *tempW2;
		PacketGetAdapterNames((char *)AdapterNameW, &AdapterLength);
		tempW = AdapterNameW;
		tempW2 = AdapterNameW;
		BX_INFO(((char *)AdapterNameW));
		while((*tempW != '\0') || (*(tempW - 1) != '\0')) {
			if(*tempW == '\0') {
				memcpy(AdapterList[i], tempW, (tempW - tempW) * 2);
				tempW2 = tempW + 1;
				i++;
			}
			tempW++;
		}
		AdapterNum = i;
	} else {
		// Windows 95/98/NT
		char *tempA, *tempA2;
		PacketGetAdapterNames(AdapterNameA, &AdapterLength);
		tempA  = AdapterNameA;
		tempA2 = AdapterNameA;
		while((*tempA != '\0') || (*(tempA - 1) != '\0')) {
			if(*tempA == '\0') {
				memcpy(AdapterList[i], tempA2, tempA - tempA2);
				tempA2 = tempA + 1;
				i++;
			}
			tempA++;
		}
		AdapterNum = i;
	}

/*
  The following calls to PacketOpenAdapter() have my
  NIC ID hard coded into it.  This will need to be 
  changed if you are testing the code on a different
  system
                    --Don Becker
*/
     lpAdapterRX = PacketOpenAdapter("\\Device\\Packet_{563A922D-5D5E-45CB-9DDA-6AD74A57E3F4}");
     if(!lpAdapterRX || (lpAdapterRX->hFile == INVALID_HANDLE_VALUE)) {
          BX_PANIC(("Could not open adapter for ethernet reception"));
          return;
     }
     lpAdapterTX = PacketOpenAdapter("\\Device\\Packet_{563A922D-5D5E-45CB-9DDA-6AD74A57E3F4}");
     if(!lpAdapterTX || (lpAdapterTX->hFile == INVALID_HANDLE_VALUE)) {
          BX_PANIC(("Could not open adapter for ethernet transmition"));
          return;
     }

     PacketSetHwFilter(lpAdapterRX, NDIS_PACKET_TYPE_PROMISCUOUS);
     PacketSetHwFilter(lpAdapterTX, NDIS_PACKET_TYPE_PROMISCUOUS);

/* The code below sets a BPF mac address filter
   that seems to really kill performance, for now
   im just using code to filter, and it works
   better
*/

//     memcpy(&this->filter, macfilter, sizeof(macfilter));
//     this->filter[1].k = (macaddr[2] & 0xff) << 24 | (macaddr[3] & 0xff) << 16 | (macaddr[4] & 0xff) << 8  | (macaddr[5] & 0xff);
//     this->filter[3].k = (macaddr[0] & 0xff) << 8 | (macaddr[1] & 0xff);
//     bp.bf_len   = 8;
//     bp.bf_insns = &this->filter[0];
//     if(!PacketSetBpf(lpAdapterRX, &bp)) {
//       BX_PANIC(("Could not set mac address BPF filter"));
 //    }

     PacketSetBuff(lpAdapterRX, 512000);
     PacketSetReadTimeout(lpAdapterRX, 1000);
     PacketSetBuff(lpAdapterTX, 512000);
     PacketSetReadTimeout(lpAdapterTX, 1000);

     if((pkSend = PacketAllocatePacket()) == NULL) {
          BX_PANIC(("Could not allocate a send packet"));
     }
     if((pkRecv = PacketAllocatePacket()) == NULL) {
           BX_PANIC(("Could not allocate a recv packet"));
     }

     rx_timer_index = bx_pc_system.register_timer(this, this->rx_timer_handler, 10000, 1, 1);
}

void
bx_win32_pktmover_c::sendpkt(void *buf, unsigned io_len)
{
	// SendPacket Here.
        fprintf(stderr, "[ETH-WIN32] Sending Packet: size=%i\n", io_len);
	PacketInitPacket(pkSend, (char *)buf, io_len);
	if(!PacketSendPacket(lpAdapterTX, pkSend, TRUE)) {
          fprintf(stderr, "[ETH-WIN32] Error sending packet: %lu\n", GetLastError());
        }
}

void bx_win32_pktmover_c::rx_timer_handler (void *this_ptr)
{
	// Recieve Packet ????
	char           *pBuf;
        unsigned char  *pPacket;
	unsigned int   iOffset = 0;
	struct bpf_hdr *hdr;
        static unsigned char bcast_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

	PacketInitPacket(pkRecv, (char *)buffer, 256000);
        PacketReceivePacket(lpAdapterRX, pkRecv, TRUE);


	pBuf = (char *)pkRecv->Buffer;
	iOffset = 0;
	while(iOffset < pkRecv->ulBytesReceived) {
		hdr = (struct bpf_hdr *)(pBuf + iOffset);
                pPacket = (unsigned char *)(pBuf + iOffset + hdr->bh_hdrlen);
		if(memcmp(pPacket + 6, cMacAddr, 6) == 0) return;

                if(memcmp(pPacket, cMacAddr, 6) == 0 || memcmp(pPacket, bcast_addr, 6) == 0 || pPacket[0] & 0x01) {
                      fprintf(stderr, "[ETH-WIN32] packet: size=%i, dst=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, src=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", hdr->bh_caplen, pPacket[0], pPacket[1], pPacket[2], pPacket[3], pPacket[4], pPacket[5], pPacket[6], pPacket[7], pPacket[8], pPacket[9], pPacket[10], pPacket[11]);
                      (*rx_handler)(rx_Arg, pPacket, hdr->bh_caplen);
                }
		iOffset = Packet_WORDALIGN(iOffset + (hdr->bh_hdrlen + hdr->bh_caplen));
	}
}
