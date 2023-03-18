/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2005-2023  The Bochs Project
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

#ifndef BX_USB_PCAP_H
#define BX_USB_PCAP_H

#define BX_PCAP_LINKTYPE  189
//#define BX_PCAP_LINKTYPE  220

// define this to non-zero to append like packets so that WireShark will parse them correctly
// define this to zero so that you can see each (ex: 8-byte) packet sent across the 'wire'
#define BX_PCAP_APPEND_PACKETS  1

#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

typedef struct pcap_hdr_s {
  Bit32u magic_number;   /* magic number */
  Bit16u version_major;  /* major version number */
  Bit16u version_minor;  /* minor version number */
  Bit32s thiszone;       /* GMT to local correction */
  Bit32u sigfigs;        /* accuracy of timestamps */
  Bit32u snaplen;        /* max length of captured packets, in octets */
  Bit32u network;        /* data link type */
}
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
pcap_hdr_t;

typedef struct pcaprec_hdr_s {
  Bit32u ts_sec;         /* timestamp seconds */
  Bit32u ts_usec;        /* timestamp microseconds */
  Bit32u incl_len;       /* number of octets of packet saved in file */
  Bit32u orig_len;       /* actual length of packet */
}
#if !defined(_MSC_VER)
  GCC_ATTRIBUTE((packed))
#endif
pcaprec_hdr_t;

struct usbmon_packet {
  Bit64u id;
  Bit8u  type;
  Bit8u  xfer_type;
  Bit8u  epnum;
  Bit8u  devnum;
  Bit16u busnum;
  Bit8s  flag_setup;
  Bit8s  flag_data;
  Bit64s ts_sec;
  Bit32s ts_usec;
  Bit32u status;
  Bit32u length;
  Bit32u len_cap;
  Bit8u  setup[8];
#if BX_PCAP_LINKTYPE == 220
  Bit32u interval;
  Bit32u start_frame;
  Bit32u xfer_flags;
  Bit32u ndesc;
#endif
};

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

// base class
class BOCHSAPI_MSVCONLY pcap_image_t
{
  public:
    // Uses a default constructor
    ~pcap_image_t();
    void pcap_image_init();

    // append a packet to the pcap file
    int create_pcap(const char *filename);
    void write_packet(void *data, int len, int bus, int dev_addr, int ep, int type, bool is_setup, bool can_append);

  private:
    int    fd;
    off_t  last_pos;

    Bit8u last_ep;
    pcaprec_hdr_t last_hdr;
    struct usbmon_packet last_packet;

    Bit64u time_usecs;
};

#endif
