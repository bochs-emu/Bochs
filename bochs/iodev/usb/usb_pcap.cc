/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2021-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//  Copyright (C) 2002-2023  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Notes by Ben Lunt:
//
// Experimental writing of packets to a Packet Capture (pcap) file for WireShark.
//  This will create a pcap formatted file of all packets sent to a
//  specific device.  The using a pcap viewer, such as WireShark, you can
//  see what packets have been sent on the "wire" as well as let the viewer
//  give you details.
//
// References used:
//   https://wiki.wireshark.org/Development/LibpcapFileFormat
//   http://www.tcpdump.org/linktypes.html
//   https://www.kernel.org/doc/Documentation/usb/usbmon.txt
// Other references (May or may not be added):
//   https://desowin.org/usbpcap/captureformat.html
//   http://www.tcpdump.org/linktypes/LINKTYPE_USB_DARWIN.html
//
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB

#include "usb_pcap.h"
#include "usb_common.h"

#ifdef linux
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#endif

/*** base class pcap_image_t ***/

pcap_image_t::~pcap_image_t()
{
  if (fd > -1)
    close(fd);
  fd = -1;
}

void pcap_image_t::pcap_image_init()
{
  fd = -1;
  last_pos = 0;
  time_usecs = 0;
}

int pcap_image_t::create_pcap(const char *pathname)
{
  pcap_hdr_t header;

  if (strlen(pathname) == 0)
    return 0;

  fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC
#ifdef O_BINARY
                | O_BINARY
#endif
                , S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP
                );

  if (fd < 0)
    return fd;

  // else write the header
  header.magic_number = 0xA1B2C3D4;  // noted as little endian
  header.version_major = 0x0002;     // major version number
  header.version_minor = 0x0004;     // minor version number
  header.thiszone = 0;               // GMT to local correction
  header.sigfigs = 0;                // accuracy of timestamps
  header.snaplen = 0x00040000;       // max length of captured packets, in octets
  header.network = BX_PCAP_LINKTYPE; // data link type (http://www.tcpdump.org/linktypes.html)
  write(fd, &header, sizeof(pcap_hdr_t));

  // This relies on the fact that the time_t epoc is the same
  //  epoc that the WireShark uses.  We want it in Microseconds as well.
  // TODO: I only tested that this epoc is the same for a Windows Host.
  //  * One will need to test for a non-Win32 host to make sure the epoc is the same.
  time_t mtime;
  time(&mtime);
  time_usecs = (Bit64u) mtime * 1000000; //  'NOW' in mS

#if BX_PCAP_APPEND_PACKETS
  last_pos = sizeof(sizeof(pcap_hdr_t));
  last_ep = 0xFF;
#endif

  return fd;
}

void pcap_image_t::write_packet(void *data, int len, int bus, int dev_addr, int ep, int type, bool is_setup, bool can_append)
{
  // if we haven't opened the file, do nothing
  if ((fd < 0) || (len == 0) || (data == NULL))
    return;

#if BX_PCAP_APPEND_PACKETS
  // can we append it to the last packet?
  // To make WireShark understand the packets, we need to
  //  append like packets.  i.e.: All IN packets of the GET_DESCRIPTOR request
  //  need to be one (1) packet here for WireShark.
  if (can_append) {
    if ((last_packet.xfer_type == type) &&
        (last_ep == ep)                 &&
        (last_packet.devnum == dev_addr)) {
      last_hdr.incl_len += len;
      last_hdr.orig_len += len;
      last_packet.length += len;
      last_packet.len_cap += len;
      lseek(fd, last_pos, SEEK_SET); // seek to last pos
      write(fd, &last_hdr, sizeof(pcaprec_hdr_t));
      write(fd, &last_packet, sizeof(struct usbmon_packet));
      lseek(fd, 0, SEEK_END);  // move to end of file again
      write(fd, data, len);
      return;
    }
  }
#endif

  Bit32u length = sizeof(struct usbmon_packet) + ((!is_setup) ? len : 0);

  const Bit64u t = time_usecs + bx_pc_system.time_usec();
  Bit64u seconds = (t / 1000000);
  Bit32u usecs = (Bit32u) (t % 1000000);

  pcaprec_hdr_t header;
  header.ts_sec = (Bit32u) seconds;   // timestamp seconds
  header.ts_usec = usecs;             // timestamp microseconds
  header.incl_len = length;           // number of octets of packet saved in file
  header.orig_len = length;           // actual length of packet

  struct usbmon_packet usb_hdr;
  usb_hdr.id = 0;
  usb_hdr.type = (ep & 0x80) ? 'C' : 'S';
  usb_hdr.xfer_type = type;
  usb_hdr.epnum = ep & 0x7F;
  usb_hdr.devnum = dev_addr;
  usb_hdr.busnum = 0;
  usb_hdr.flag_setup = (is_setup) ? 0 : '-';
  usb_hdr.flag_data = '='; // ???
  usb_hdr.ts_sec = seconds;
  usb_hdr.ts_usec = usecs;
  usb_hdr.status = 0; // assume success each time?
  usb_hdr.length = length - sizeof(struct usbmon_packet);
  usb_hdr.len_cap = length;
  if (is_setup)
    memcpy(usb_hdr.setup, data, 8);
  else
    memset(usb_hdr.setup, 0, 8);
#if BX_PCAP_LINKTYPE == 220
  usb_hdr.interval = 0;
  usb_hdr.start_frame = 0;
  usb_hdr.xfer_flags = 0;
  usb_hdr.ndesc = 0;  // we don't support ISO's, so the count is always zero
#endif

#if BX_PCAP_APPEND_PACKETS
  // save current position
  last_pos = lseek(fd, 0, SEEK_CUR);
#endif

  write(fd, &header, sizeof(pcaprec_hdr_t));
  write(fd, &usb_hdr, sizeof(struct usbmon_packet));
  if (length > sizeof(struct usbmon_packet))
    write(fd, data, length - sizeof(struct usbmon_packet));

#if BX_PCAP_APPEND_PACKETS
  // save this for next time
  last_ep = ep;
  memcpy(&last_hdr, &header, sizeof(pcaprec_hdr_t));
  memcpy(&last_packet, &usb_hdr, sizeof(struct usbmon_packet));
#endif
}

#endif
