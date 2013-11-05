/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2013  The Bochs Project
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

// shared code for the low-level cdrom support

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_SUPPORT_CDROM

#include "cdrom.h"

#define LOG_THIS /* no SMF tricks here, not needed */

#define BX_CD_FRAMESIZE 2048

unsigned int bx_cdrom_count = 0;

bx_bool cdrom_interface::create_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
{
  unsigned i;
  Bit32u blocks;
  int len = 4;

  switch (format) {
    case 0:
      // From atapi specs : start track can be 0-63, AA
      if ((start_track > 1) && (start_track != 0xaa))
        return 0;

      buf[2] = 1;
      buf[3] = 1;

      if (start_track <= 1) {
        buf[len++] = 0; // Reserved
        buf[len++] = 0x14; // ADR, control
        buf[len++] = 1; // Track number
        buf[len++] = 0; // Reserved

        // Start address
        if (msf) {
          buf[len++] = 0; // reserved
          buf[len++] = 0; // minute
          buf[len++] = 2; // second
          buf[len++] = 0; // frame
        } else {
          buf[len++] = 0;
          buf[len++] = 0;
          buf[len++] = 0;
          buf[len++] = 0; // logical sector 0
        }
      }

      // Lead out track
      buf[len++] = 0; // Reserved
      buf[len++] = 0x16; // ADR, control
      buf[len++] = 0xaa; // Track number
      buf[len++] = 0; // Reserved

      blocks = capacity();

      // Start address
      if (msf) {
        buf[len++] = 0; // reserved
        buf[len++] = (Bit8u)(((blocks + 150) / 75) / 60); // minute
        buf[len++] = (Bit8u)(((blocks + 150) / 75) % 60); // second
        buf[len++] = (Bit8u)((blocks + 150) % 75); // frame;
      } else {
        buf[len++] = (blocks >> 24) & 0xff;
        buf[len++] = (blocks >> 16) & 0xff;
        buf[len++] = (blocks >> 8) & 0xff;
        buf[len++] = (blocks >> 0) & 0xff;
      }
      buf[0] = ((len-2) >> 8) & 0xff;
      buf[1] = (len-2) & 0xff;
      break;

    case 1:
      // multi session stuff - emulate a single session only
      buf[0] = 0;
      buf[1] = 0x0a;
      buf[2] = 1;
      buf[3] = 1;
      for (i = 0; i < 8; i++)
        buf[4+i] = 0;
      len = 12;
      break;

    case 2:
      // raw toc - emulate a single session only (ported from qemu)
      buf[2] = 1;
      buf[3] = 1;

      for (i = 0; i < 4; i++) {
        buf[len++] = 1;
        buf[len++] = 0x14;
        buf[len++] = 0;
        if (i < 3) {
          buf[len++] = 0xa0 + i;
        } else {
          buf[len++] = 1;
        }
        buf[len++] = 0;
        buf[len++] = 0;
        buf[len++] = 0;
        if (i < 2) {
          buf[len++] = 0;
          buf[len++] = 1;
          buf[len++] = 0;
          buf[len++] = 0;
        } else if (i == 2) {
          blocks = capacity();
          if (msf) {
            buf[len++] = 0; // reserved
            buf[len++] = (Bit8u)(((blocks + 150) / 75) / 60); // minute
            buf[len++] = (Bit8u)(((blocks + 150) / 75) % 60); // second
            buf[len++] = (Bit8u)((blocks + 150) % 75); // frame;
          } else {
            buf[len++] = (blocks >> 24) & 0xff;
            buf[len++] = (blocks >> 16) & 0xff;
            buf[len++] = (blocks >> 8) & 0xff;
            buf[len++] = (blocks >> 0) & 0xff;
          }
        } else {
          buf[len++] = 0;
          buf[len++] = 0;
          buf[len++] = 0;
          buf[len++] = 0;
        }
      }
      buf[0] = ((len-2) >> 8) & 0xff;
      buf[1] = (len-2) & 0xff;
      break;

    default:
      BX_PANIC(("cdrom: create_toc(): unknown format"));
      return 0;
  }

  *length = len;

  return 1;
}

bx_bool cdrom_interface::seek(Bit32u lba)
{
  unsigned char buffer[BX_CD_FRAMESIZE];

  return read_block(buffer, lba, BX_CD_FRAMESIZE);
}

#endif /* if BX_SUPPORT_CDROM */
