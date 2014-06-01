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

// These are the low-level CDROM functions which are called
// from 'harddrv.cc'.  They effect the OS specific functionality
// needed by the CDROM emulation in 'harddrv.cc'.  Mostly, just
// ioctl() calls and such.  Should be fairly easy to add support
// for your OS if it is not supported yet.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_SUPPORT_CDROM

#include "cdrom.h"
#include "cdrom_misc.h"

#define LOG_THIS /* no SMF tricks here, not needed */

extern "C" {
#include <errno.h>
}

#ifdef __linux__
extern "C" {
#include <sys/ioctl.h>
#include <linux/cdrom.h>
// I use the framesize in non OS specific code too
#define BX_CD_FRAMESIZE CD_FRAMESIZE
}

#elif defined(__GNU__) || (defined(__CYGWIN32__) && !defined(WIN32))
extern "C" {
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
}

#elif defined(__sun)
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/cdio.h>
#define BX_CD_FRAMESIZE CDROM_BLK_2048
}

#elif defined(__DJGPP__)
extern "C" {
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
}

#elif (defined(__NetBSD__) || defined(__NetBSD_kernel__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))
// OpenBSD pre version 2.7 may require extern "C" { } structure around
// all the includes, because the i386 sys/disklabel.h contains code which
// c++ considers invalid.
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/cdio.h>
#if defined(__OpenBSD__)
#include <sys/dkio.h>
#endif
#include <sys/ioctl.h>
#include <sys/disklabel.h>
// ntohl(x) et al have been moved out of sys/param.h in FreeBSD 5
#include <netinet/in.h>

// XXX
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE    2048

#elif !defined(WIN32) // all others (Irix, Tru64)

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048

#else // WIN32

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048

#endif

#include <stdio.h>


bx_bool cdrom_misc_c::start_cdrom()
{
  // Spin up the cdrom drive.

  if (fd >= 0) {
    if (!using_file) {
#if defined(__NetBSD__) || defined(__NetBSD_kernel__)
      if (ioctl (fd, CDIOCSTART) < 0)
        BX_DEBUG(("start_cdrom: start returns error: %s", strerror (errno)));
      return 1;
#else
      BX_INFO(("start_cdrom: your OS is not supported yet"));
      return 0; // OS not supported yet, return 0 always
#endif
    }
  }
  return 0;
}

void cdrom_misc_c::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
    if (!using_file) {
#if (defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))
      (void) ioctl (fd, CDIOCALLOW);
      if (ioctl (fd, CDIOCEJECT) < 0)
        BX_DEBUG(("eject_cdrom: eject returns error"));
#elif defined(__linux__)
      ioctl (fd, CDROMEJECT, NULL);
#endif
    }
    close(fd);
    fd = -1;
  }
}

bx_bool cdrom_misc_c::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
{
  // Read CD TOC. Returns 0 if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    return 0;
  }

  // This is a hack and works okay if there's one rom track only
  if (using_file || (format != 0)) {
    return cdrom_base_c::read_toc(buf, length, msf, start_track, format);
  }
  // all these implementations below are the platform-dependent code required
  // to read the TOC from a physical cdrom.
#if __linux__ || defined(__sun)
  {
  struct cdrom_tochdr tochdr;
  if (ioctl(fd, CDROMREADTOCHDR, &tochdr))
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if ((start_track > tochdr.cdth_trk1) && (start_track != 0xaa))
    return 0;

  buf[2] = tochdr.cdth_trk0;
  buf[3] = tochdr.cdth_trk1;

  if (start_track < tochdr.cdth_trk0)
    start_track = tochdr.cdth_trk0;

  int len = 4;
  for (int i = start_track; i <= tochdr.cdth_trk1; i++) {
    struct cdrom_tocentry tocentry;
    tocentry.cdte_format = (msf) ? CDROM_MSF : CDROM_LBA;
    tocentry.cdte_track = i;
    if (ioctl(fd, CDROMREADTOCENTRY, &tocentry))
      BX_PANIC(("cdrom: read_toc: READTOCENTRY failed."));
    buf[len++] = 0; // Reserved
    buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = tocentry.cdte_addr.msf.minute;
      buf[len++] = tocentry.cdte_addr.msf.second;
      buf[len++] = tocentry.cdte_addr.msf.frame;
    } else {
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 24) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 16) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 8) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 0) & 0xff;
    }
  }

  // Lead out track
  struct cdrom_tocentry tocentry;
  tocentry.cdte_format = (msf) ? CDROM_MSF : CDROM_LBA;
#ifdef CDROM_LEADOUT
  tocentry.cdte_track = CDROM_LEADOUT;
#else
  tocentry.cdte_track = 0xaa;
#endif
  if (ioctl(fd, CDROMREADTOCENTRY, &tocentry))
    BX_PANIC(("cdrom: read_toc: READTOCENTRY lead-out failed."));
  buf[len++] = 0; // Reserved
  buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, control
  buf[len++] = 0xaa; // Track number
  buf[len++] = 0; // Reserved

  // Start address
  if (msf) {
    buf[len++] = 0; // reserved
    buf[len++] = tocentry.cdte_addr.msf.minute;
    buf[len++] = tocentry.cdte_addr.msf.second;
    buf[len++] = tocentry.cdte_addr.msf.frame;
  } else {
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 24) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 16) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 8) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 0) & 0xff;
  }

  buf[0] = ((len-2) >> 8) & 0xff;
  buf[1] = (len-2) & 0xff;

  *length = len;

  return 1;
  }
#elif (defined(__NetBSD__) || defined(__NetBSD_kernel__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))
  {
  struct ioc_toc_header h;
  struct ioc_read_toc_entry t;

  if (ioctl (fd, CDIOREADTOCHEADER, &h) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if ((start_track > h.ending_track) && (start_track != 0xaa))
    return 0;

  buf[2] = h.starting_track;
  buf[3] = h.ending_track;

  if (start_track < h.starting_track)
    start_track = h.starting_track;

  int len = 4;
  for (int i = start_track; i <= h.ending_track; i++) {
    struct cd_toc_entry tocentry;
    t.address_format = (msf) ? CD_MSF_FORMAT : CD_LBA_FORMAT;
    t.starting_track = i;
    t.data_len = sizeof(tocentry);
    t.data = &tocentry;

    if (ioctl (fd, CDIOREADTOCENTRYS, &t) < 0)
      BX_PANIC(("cdrom: read_toc: READTOCENTRY failed."));

    buf[len++] = 0; // Reserved
    buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = tocentry.addr.msf.minute;
      buf[len++] = tocentry.addr.msf.second;
      buf[len++] = tocentry.addr.msf.frame;
    } else {
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 24) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 16) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 8) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 0) & 0xff;
    }
  }

  // Lead out track
  struct cd_toc_entry tocentry;
  t.address_format = (msf) ? CD_MSF_FORMAT : CD_LBA_FORMAT;
  t.starting_track = 0xaa;
  t.data_len = sizeof(tocentry);
  t.data = &tocentry;

  if (ioctl (fd, CDIOREADTOCENTRYS, &t) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCENTRY lead-out failed."));

  buf[len++] = 0; // Reserved
  buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, control
  buf[len++] = 0xaa; // Track number
  buf[len++] = 0; // Reserved

  // Start address
  if (msf) {
    buf[len++] = 0; // reserved
    buf[len++] = tocentry.addr.msf.minute;
    buf[len++] = tocentry.addr.msf.second;
    buf[len++] = tocentry.addr.msf.frame;
  } else {
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 24) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 16) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 8) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 0) & 0xff;
  }

  buf[0] = ((len-2) >> 8) & 0xff;
  buf[1] = (len-2) & 0xff;

  *length = len;

  return 1;
  }
#else
  BX_INFO(("read_toc: your OS is not supported yet"));
  return 0; // OS not supported yet, return 0 always.
#endif
}

Bit32u cdrom_misc_c::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    return cdrom_base_c::capacity();
  }

#if defined(__sun)
  {
    struct stat buf = {0};

    if (fd < 0) {
      BX_PANIC(("cdrom: capacity: file not open."));
    }

    if(fstat(fd, &buf) != 0)
      BX_PANIC(("cdrom: capacity: stat() failed."));

    return(buf.st_size);
  }
#elif (defined(__NetBSD__) || defined(__NetBSD_kernel__) || defined(__OpenBSD__))
  {
  // We just read the disklabel, imagine that...
  struct disklabel lp;

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, DIOCGDINFO, &lp) < 0)
    BX_PANIC(("cdrom: ioctl(DIOCGDINFO) failed"));

  BX_DEBUG(("capacity: %u", lp.d_secperunit));
  return(lp.d_secperunit);
  }
#elif defined(__linux__)
  {
  // Read the TOC to get the data size, since BLKGETSIZE doesn't work on
  // non-ATAPI drives.  This is based on Keith Jones code below.
  // <splite@purdue.edu> 21 June 2001

  int i, dtrk_lba, num_sectors;
  int dtrk = 0;
  struct cdrom_tochdr td;
  struct cdrom_tocentry te;

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, CDROMREADTOCHDR, &td) < 0)
    BX_PANIC(("cdrom: ioctl(CDROMREADTOCHDR) failed"));

  num_sectors = -1;
  dtrk_lba = -1;

  for (i = td.cdth_trk0; i <= td.cdth_trk1; i++) {
    te.cdte_track = i;
    te.cdte_format = CDROM_LBA;
    if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
      BX_PANIC(("cdrom: ioctl(CDROMREADTOCENTRY) failed"));

    if (dtrk_lba != -1) {
      num_sectors = te.cdte_addr.lba - dtrk_lba;
      break;
    }
    if (te.cdte_ctrl & CDROM_DATA_TRACK) {
      dtrk = i;
      dtrk_lba = te.cdte_addr.lba;
    }
  }

  if (num_sectors < 0) {
    if (dtrk_lba != -1) {
      te.cdte_track = CDROM_LEADOUT;
      te.cdte_format = CDROM_LBA;
      if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
        BX_PANIC(("cdrom: ioctl(CDROMREADTOCENTRY) failed"));
      num_sectors = te.cdte_addr.lba - dtrk_lba;
    } else
      BX_PANIC(("cdrom: no data track found"));
  }

  BX_INFO(("cdrom: Data track %d, length %d", dtrk, num_sectors));

  return(num_sectors);

  }
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
  {
  // Read the TOC to get the size of the data track.
  // Keith Jones <freebsd.dev@blueyonder.co.uk>, 16 January 2000

#define MAX_TRACKS 100

  int i, num_tracks, num_sectors;
  struct ioc_toc_header td;
  struct ioc_read_toc_entry rte;
  struct cd_toc_entry toc_buffer[MAX_TRACKS + 1];

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, CDIOREADTOCHEADER, &td) < 0)
    BX_PANIC(("cdrom: ioctl(CDIOREADTOCHEADER) failed"));

  num_tracks = (td.ending_track - td.starting_track) + 1;
  if (num_tracks > MAX_TRACKS)
    BX_PANIC(("cdrom: TOC is too large"));

  rte.address_format = CD_LBA_FORMAT;
  rte.starting_track = td.starting_track;
  rte.data_len = (num_tracks + 1) * sizeof(struct cd_toc_entry);
  rte.data = toc_buffer;
  if (ioctl(fd, CDIOREADTOCENTRYS, &rte) < 0)
    BX_PANIC(("cdrom: ioctl(CDIOREADTOCENTRYS) failed"));

  num_sectors = -1;
  for (i = 0; i < num_tracks; i++) {
    if (rte.data[i].control & 4) {  /* data track */
      num_sectors = ntohl(rte.data[i + 1].addr.lba)
          - ntohl(rte.data[i].addr.lba);
      BX_INFO(("cdrom: Data track %d, length %d",
        rte.data[i].track, num_sectors));
      break;
      }
    }

  if (num_sectors < 0)
    BX_PANIC(("cdrom: no data track found"));

  return(num_sectors);

  }
#else
  BX_ERROR(("capacity: your OS is not supported yet"));
  return(0);
#endif
}

#endif /* if BX_SUPPORT_CDROM */
