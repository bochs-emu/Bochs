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


// These are the low-level CDROM functions which are called
// from 'harddrv.cc'.  They effect the OS specific functionality
// needed by the CDROM emulation in 'harddrv.cc'.  Mostly, just
// ioctl() calls and such.  Should be fairly easy to add support
// for your OS if it is not supported yet.


#include "bochs.h"

#define LOG_THIS /* no SMF tricks here, not needed */

#ifdef __linux__
extern "C" {
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/cdrom.h>
// I use the framesize in non OS specific code too
#define BX_CD_FRAMESIZE CD_FRAMESIZE
}
#endif

#ifdef __sun
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/cdio.h>
#define BX_CD_FRAMESIZE CDROM_BLK_2048
}
#endif /* __sun */

#if (defined(__OpenBSD__) || defined(__FreeBSD__))
// OpenBSD pre version 2.7 may require extern "C" { } structure around
// all the includes, because the i386 sys/disklabel.h contains code which
// c++ considers invalid.
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/cdio.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>

// XXX
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#endif

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
HANDLE hFile = NULL;
#endif


#include <stdio.h>

cdrom_interface::cdrom_interface(char *dev)
{
  setprefix("[CD  ]");
  settype(CDLOG);
  fd = -1; // File descriptor not yet allocated

  if ( dev == NULL )
    path = NULL;
  else {
    path = strdup(dev);
    BX_INFO(("Init, file = '%s'",dev));
  }
  using_file=0;
}

cdrom_interface::~cdrom_interface(void)
{
  if (fd >= 0)
    close(fd);
  if (path)
    free(path);
}

  bool
cdrom_interface::insert_cdrom()
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns false if CD is not ready.
#ifdef WIN32
    char drive[256];
printf(path);
printf('\n');
    if ( (path[1] == ':') && (strlen(path) == 2) )
    {
      sprintf(drive, "\\\\.\\%s", path);
      using_file = 0;
printf("using cd");
    }
    else
    {
      strcpy(drive,path);
      using_file = 1;
printf("using file");
    }
printf('\n');
    hFile=CreateFile((char *)&drive,  GENERIC_READ, 0 , NULL, OPEN_EXISTING, 
FILE_FLAG_RANDOM_ACCESS, NULL);
      //printf("%s", path);
  //hFile=CreateFile(path,  GENERIC_READ, 0 , NULL, OPEN_EXISTING, 
FILE_FLAG_RANDOM_ACCESS, NULL);
  if (hFile !=(void *)0xFFFFFFFF)
         fd=1;
#else
    fd = open(path, O_RDONLY);
#endif
    if (fd < 0) {
       BX_INFO(( "::cdrom_interface: open failed on dev '%s'.", path));
       return(false);
    }

  // I just see if I can read a sector to verify that a
  // CD is in the drive and readable.
#ifdef WIN32
    ReadFile(hFile, (void *) buffer, BX_CD_FRAMESIZE, (unsigned long *) &ret, NULL);
    if (ret < 0) {
       CloseHandle(hFile);
       fd = -1;
       BX_DEBUG(( "insert_cdrom: read returns error." ));
       return(false);
      }
#else
    ret = read(fd, &buffer, BX_CD_FRAMESIZE);
    if (ret < 0) {
       close(fd);
       fd = -1;
       BX_DEBUG(( "insert_cdrom: read returns error." ));
       return(false);
        }
#endif
    return(true);
}


  void
cdrom_interface::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
#if (defined(__OpenBSD__) || defined(__FreeBSD__))
    (void) ioctl (fd, CDIOCALLOW);
    if (ioctl (fd, CDIOCEJECT) < 0)
	  BX_DEBUG(( "eject_cdrom: eject returns error." ));
#endif

#ifdef WIN32
if (using_file == 0)
{
  DWORD lpBytesReturned;
  DeviceIoControl(hFile, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, 
&lpBytesReturned, NULL);
}
#endif

    close(fd);
    fd = -1;
    }
}


  bool
cdrom_interface::read_toc(uint8* buf, int* length, bool msf, int 
start_track)
{
  // Read CD TOC. Returns false if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    }

#ifdef WIN32
  {
/*     #define IOCTL_CDROM_BASE                 FILE_DEVICE_CD_ROM
     #define IOCTL_CDROM_READ_TOC         CTL_CODE(IOCTL_CDROM_BASE, 0x0000, 
METHOD_BUFFERED, FILE_READ_ACCESS)
     unsigned long iBytesReturned;
     DeviceIoControl(hFile, IOCTL_CDROM_READ_TOC, NULL, 0, NULL, 0, 
&iBytesReturned, NULL);       */
         return true;
  }
#elif __linux__ || defined(__sun)
  {
  struct cdrom_tochdr tochdr;
  if (ioctl(fd, CDROMREADTOCHDR, &tochdr))
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if (start_track > tochdr.cdth_trk1)
    return false;

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
    buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, 
control
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
  buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, 
control
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

  return true;
  }
#elif (defined(__OpenBSD__) || defined(__FreeBSD__))
  {
  struct ioc_toc_header h;
  struct ioc_read_toc_entry t;

  if (ioctl (fd, CDIOREADTOCHEADER, &h) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if (start_track > h.ending_track)
    return false;

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

    if (ioctl (fd, CDIOREADTOCENTRYS, &tocentry) < 0)
      BX_PANIC(("cdrom: read_toc: READTOCENTRY failed."));

    buf[len++] = 0; // Reserved
    buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, 
control
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

  if (ioctl (fd, CDIOREADTOCENTRYS, &tocentry) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCENTRY lead-out failed."));

  buf[len++] = 0; // Reserved
  buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, 
control
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

  return true;
  }
#else
  BX_INFO(("read_toc: your OS is not supported yet."));
  return(false); // OS not supported yet, return false always.
#endif
}


  uint32
cdrom_interface::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of bytes of capacity the actual media has.
#ifdef __sun
  {
    struct stat buf = {0};

    if (fd < 0) {
      BX_PANIC(("cdrom: capacity: file not open."));
    } 
    
    if( fstat(fd, &buf) != 0 )
      BX_PANIC(("cdrom: capacity: stat() failed."));
  
    return(buf.st_size);
  }
#elif __linux__
  {
  // I just looked through the Linux kernel source to see
  // what it does with the ATAPI capacity command, and reversed
  // that process here.
  uint32 nr_sects;

  if (fd < 0) {
    BX_PANIC(("cdrom: capacity: file not open."));
    }
  if (ioctl(fd, BLKGETSIZE, &nr_sects) != 0) {
    BX_PANIC(("cdrom: ioctl(BLKGETSIZE) failed"));
    }
  nr_sects /= (CD_FRAMESIZE / 512);

  BX_DEBUG(( "capacity: %u", nr_sects));
  return(nr_sects);
  }
#elif defined(__OpenBSD__)
  {
  // We just read the disklabel, imagine that...
  struct disklabel lp;

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, DIOCGDINFO, &lp) < 0)
    BX_PANIC(("cdrom: ioctl(DIOCGDINFO) failed"));

  BX_DEBUG(( "capacity: %u", lp.d_secperunit ));
  return(lp.d_secperunit);
  }
#elif defined(__FreeBSD__)
  {
  // Read the TOC to get the data size, since disklabel doesn't appear
  // to work, sadly.
  // Keith Jones, 16 January 2000

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
    if (rte.data[i].control & 4) {	/* data track */
      num_sectors = ntohl(rte.data[i + 1].addr.lba)
          - ntohl(rte.data[i].addr.lba);
      BX_ERROR(( "cdrom: Data track %d, length %d",
        rte.data[i].track, num_sectors));
      break;
      }
    }

  if (num_sectors < 0)
    BX_PANIC(("cdrom: no data track found"));

  return(num_sectors);

  }
#elif defined WIN32
  {
      unsigned long FileSize;
          return (GetFileSize(hFile, &FileSize)
  );

  }
#else
  BX_INFO(( "capacity: your OS is not supported yet." ));
  return(0);
#endif
}

  void
cdrom_interface::read_block(uint8* buf, int lba)
{
  // Read a single block from the CD

  off_t pos;
  ssize_t n;

#ifdef WIN32
  pos = SetFilePointer(hFile, lba*BX_CD_FRAMESIZE, NULL, SEEK_SET);
  if (pos == 0xffffffff) {
    BX_PANIC(("cdrom: read_block: lseek returned error."));
    }
#else
  pos = lseek(fd, lba*BX_CD_FRAMESIZE, SEEK_SET);
  if (pos < 0) {
    BX_PANIC(("cdrom: read_block: lseek returned error."));
    }
#endif

#ifdef WIN32
  ReadFile(hFile, (void *) buf, BX_CD_FRAMESIZE, (unsigned long *) &n, 
NULL);
#else
  n = read(fd, buf, BX_CD_FRAMESIZE);
#endif

  if (n != BX_CD_FRAMESIZE) {
    BX_PANIC(("cdrom: read_block: read returned %d",
      (int) n));
    }
}

