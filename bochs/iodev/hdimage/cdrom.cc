/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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

bx_bool cdrom_base_c::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO(("load cdrom with path='%s'", path));
  // all platforms except win32
  fd = open(path, O_RDONLY);
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

bx_bool cdrom_base_c::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
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
      BX_PANIC(("cdrom: read_toc(): unknown format"));
      return 0;
  }

  *length = len;

  return 1;
}

bx_bool BX_CPP_AttrRegparmN(3) cdrom_base_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
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

bx_bool cdrom_base_c::start_cdrom()
{
  // Spin up the cdrom drive.

  if (fd >= 0) {
    BX_INFO(("start_cdrom: your OS is not supported yet"));
    return 0; // OS not supported yet, return 0 always
  }
  return 0;
}

bx_bool cdrom_base_c::seek(Bit32u lba)
{
  unsigned char buffer[BX_CD_FRAMESIZE];

  return read_block(buffer, lba, BX_CD_FRAMESIZE);
}
