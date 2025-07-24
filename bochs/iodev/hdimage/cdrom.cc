/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2025  The Bochs Project
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

#include "bochs.h"
#include "cdrom.h"

#include <stdio.h>

#define LOG_THIS /* no SMF tricks here, not needed */

#define BX_CD_FRAMESIZE 2048

unsigned int bx_cdrom_count = 0;

cdrom_base_c::cdrom_base_c(const char *dev)
{
  char prefix[6];

  sprintf(prefix, "CD%d", ++bx_cdrom_count);
  put(prefix);
  fd = -1; // File descriptor not yet allocated

  if (dev == NULL) {
    path = NULL;
  } else {
    path = strdup(dev);
  }
  using_file = 0;
}

cdrom_base_c::~cdrom_base_c(void)
{
  if (fd >= 0)
    close(fd);
  if (path)
    free(path);
  BX_DEBUG(("Exit"));
}

bool cdrom_base_c::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO(("load cdrom with path='%s'", path));

  // all platforms except win32
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_ERROR(("open cd failed for '%s': %s", path, strerror(errno)));
    return 0;
  }
  // do fstat to determine if it's a file or a device, then set using_file.
  struct stat stat_buf;
  ret = fstat(fd, &stat_buf);
  if (ret) {
    BX_PANIC(("fstat cdrom file returned error: %s", strerror (errno)));
  }
  if (S_ISREG(stat_buf.st_mode)) {
    using_file = 1;
    BX_INFO(("Opening image file as a cd."));
  } else {
    using_file = 0;
    BX_INFO(("Using direct access for cdrom."));
  }
  // I just see if I can read a sector to verify that a
  // CD is in the drive and readable.
  return read_block(buffer, 0, 2048);
}

void cdrom_base_c::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
    close(fd);
    fd = -1;
  }
}

// note: when the data returned is verified to work in all cases, to make this
//  faster and lighter code, we can simply hard code each format's return as a binary array.
// (especially format 2)
bool cdrom_base_c::read_toc(Bit8u* buf, int* length, bool msf, int start_track, int format)
{
  Bit32u blocks = capacity();
  Bit8u *p;
  int len = 0;

  // we only support a starting track of 1
  // mmc4r05a.pdf, 6.29.2.5 states that the track number should be 1 to 99 only...
  // 6.30.3.2.3 states that a one track device has a track number of 1
  if (start_track > 1) {
    BX_ERROR(("Read TOC: Starting Track should be 0 or 1 (is %i)", start_track));
    return false;
  }

  switch (format) {
    case 0:
      // we are 20 bytes in length
      buf[0] = (((20 - 2) & 0xFF00) >> 8);
      buf[1] = (((20 - 2) & 0x00FF) >> 0);
      
      // first and last track is 1
      buf[2] = 1;
      buf[3] = 1;

      ////////////////////////////////////
      // first descriptor
      buf[4] = 0x00; // Reserved
      buf[5] = 0x15; // ADR = 1, control = 5
      buf[6] = 0x01; // Track number
      buf[7] = 0x00; // Reserved

      // Start address
      if (msf) {
        buf[8] = 0x00;  // reserved
        buf[9] = 0x00;  // minute
        buf[10] = 0x02; // second
        buf[11] = 0x00; // frame
      } else {
        // logical sector 0
        buf[8] = 0x00;
        buf[9] = 0x00;
        buf[10] = 0x00;
        buf[11] = 0x00;
      }

      ////////////////////////////////////
      // second descriptor
      
      // Lead out track
      buf[12] = 0x00; // Reserved
      buf[13] = 0x16; // ADR = 1, control = 6
      buf[14] = 0xAA; // Track number
      buf[15] = 0x00; // Reserved

      // Start address of next
      // i.e.: size of this track
      if (msf) {
        buf[16] = 0; // reserved
        buf[17] = (Bit8u) (((blocks + 150) / 75) / 60); // minute
        buf[18] = (Bit8u) (((blocks + 150) / 75) % 60); // second
        buf[19] = (Bit8u) ((blocks + 150) % 75);        // frame;
      } else {
        buf[16] = (blocks >> 24) & 0xFF;
        buf[17] = (blocks >> 16) & 0xFF;
        buf[18] = (blocks >> 8) & 0xFF;
        buf[19] = (blocks >> 0) & 0xFF;
      }
      len = 20;
      break;

    case 1: // multi session stuff - emulate a single session only
      // we are 12 bytes in length
      buf[0] = (((12 - 2) & 0xFF00) >> 8);
      buf[1] = (((12 - 2) & 0x00FF) >> 0);
      buf[2] = 1;
      buf[3] = 1;
      memset(&buf[4], 0, 8);
      len = 12;
      break;

    case 3: // (return zero bytes)
    case 4: //
      break;

    case 2:
      // we are 48 bytes in length
      buf[0] = (((48 - 2) & 0xFF00) >> 8);
      buf[1] = (((48 - 2) & 0x00FF) >> 0);
      
      // first and last track is 1
      buf[2] = 1;
      buf[3] = 1;

      // raw toc - emulate a single session only
      p = &buf[4];
      p[0] = 0x01;    // session number (only 1)
      p[1] = 0x16;    // ADR = 1, Control = 6 (Data track, recorded uninterrupted)
      p[2] = 0x00;    // TNO = 0
      p[3] = 0xA0;    // POINT = 0xA0
      p[4] = 0x00;    // Min
      p[5] = 0x00;    // Sec
      p[6] = 0x00;    // Frame
      p[7] = 0x00;    // Zero or (Hour/PHour)
      p[8] = 0x01;    // PMin
      p[9] = 0x20;    // PSec
      p[10] = 0x00;    // PFrame

      p += 11;
      p[0] = 0x01;    // session number (only 1)
      p[1] = 0x16;    // ADR = 1, Control = 6 (Data track, recorded uninterrupted)
      p[2] = 0x00;    // TNO = 0
      p[3] = 0xA1;    // POINT = 0xA1
      p[4] = 0x00;    // Min
      p[5] = 0x00;    // Sec
      p[6] = 0x00;    // Frame
      p[7] = 0x00;    // Zero or (Hour/PHour)
      p[8] = 0x01;    // PMin
      p[9] = 0x00;    // PSec
      p[10] = 0x00;    // PFrame

      p += 11;
      p[0] = 0x01;    // session number (only 1)
      p[1] = 0x16;    // ADR = 1, Control = 6 (Data track, recorded uninterrupted)
      p[2] = 0x00;    // TNO = 0
      p[3] = 0xA2;    // POINT = 0xA2
      p[4] = 0x00;    // Min
      p[5] = 0x00;    // Sec
      p[6] = 0x00;    // Frame
      p[7] = 0x00;    // Zero or (Hour/PHour)
      p[8] = (Bit8u) (((blocks + 150) / 75) / 60); // Pminute
      p[9] = (Bit8u) (((blocks + 150) / 75) % 60); // Psecond
      p[10] = (Bit8u) ((blocks + 150) % 75);       // Pframe;

      p += 11;
      p[0] = 0x01;    // session number (only 1)
      p[1] = 0x15;    // ADR = 1, Control = 5 (Data track, recorded incremental)
      p[2] = 0x00;    // TNO = 0
      p[3] = 0x01;    // POINT = 0x01
      p[4] = 0x00;    // Min
      p[5] = 0x00;    // Sec
      p[6] = 0x00;    // Frame
      p[7] = 0x00;    // Zero or (Hour/PHour)
      p[8] = 0x00;    // Pminute
      p[9] = 0x02;    // Psecond
      p[10] = 0x00;   // Pframe

      len = 48;
      break;

    case 5:
      // return the header only
      buf[0] = ((2 & 0xFF00) >> 8);
      buf[1] = ((2 & 0x00FF) >> 0);
      
      // first and last track is 0
      buf[2] = 0;
      buf[3] = 0;
      len = 4;
      break;
      
    // don't do the 'default' here.
    // the calling function catches unknown format values
    //default:
    //  BX_PANIC(("cdrom: read_toc(): unknown format"));
    //  return false;
  }

  if (len <= 0)
    return false;
  
  if (length) *length = len;
  return true;
}

bool BX_CPP_AttrRegparmN(3) cdrom_base_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
{
  // Read a single block from the CD

  off_t pos;
  ssize_t n = 0;
  Bit8u try_count = 3;
  Bit8u* buf1;

  if (blocksize == 2352) {
    memset(buf, 0, 2352);
    memset(buf+1, 0xff, 10);
    Bit32u raw_block = lba + 150;
    buf[12] = (raw_block / 75) / 60;
    buf[13] = (raw_block / 75) % 60;
    buf[14] = (raw_block % 75);
    buf[15] = 0x01;
    buf1 = buf + 16;
  } else {
    buf1 = buf;
  }
  do {
    pos = lseek(fd, (off_t) lba * BX_CD_FRAMESIZE, SEEK_SET);
    if (pos < 0) {
      BX_PANIC(("cdrom: read_block: lseek returned error."));
    } else {
      n = read(fd, (char*) buf1, BX_CD_FRAMESIZE);
    }
  } while ((n != BX_CD_FRAMESIZE) && (--try_count > 0));

  return (n == BX_CD_FRAMESIZE);
}

Bit32u cdrom_base_c::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    // return length of the image file
    struct stat stat_buf;
    int ret = fstat(fd, &stat_buf);
    if (ret) {
       BX_PANIC(("fstat on cdrom image returned err: %s", strerror(errno)));
    }
    if ((stat_buf.st_size % 2048) != 0)  {
      BX_ERROR(("expected cdrom image to be a multiple of 2048 bytes"));
    }
    return (Bit32u)(stat_buf.st_size / 2048);
  } else {
    BX_ERROR(("capacity: your OS is not supported yet"));
    return 0;
  }
}

bool cdrom_base_c::start_cdrom()
{
  // Spin up the cdrom drive.

  if (fd >= 0) {
    BX_INFO(("start_cdrom: your OS is not supported yet"));
    return 0; // OS not supported yet, return 0 always
  }
  return 0;
}

bool cdrom_base_c::seek(Bit32u lba)
{
  unsigned char buffer[BX_CD_FRAMESIZE];

  return read_block(buffer, lba, BX_CD_FRAMESIZE);
}
