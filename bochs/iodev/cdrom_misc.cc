/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2011  The Bochs Project
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

#ifndef BX_IODEV_CDROM_H
#define BX_IODEV_CDROM_H

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_SUPPORT_CDROM

#include "cdrom.h"

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

#elif BX_WITH_MACOS
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048

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

#elif defined(__APPLE__)
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#if defined (__GNUC__) && (__GNUC__ >= 4)
#include <sys/disk.h>
#else
#include <dev/disk.h>
#endif
#include <errno.h>
#include <paths.h>
#include <sys/param.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDTypes.h>
#include <CoreFoundation/CoreFoundation.h>

// These definitions were taken from mount_cd9660.c
// There are some similar definitions in IOCDTypes.h
// however there seems to be some dissagreement in
// the definition of CDTOC.length
struct _CDMSF {
        u_char   minute;
        u_char   second;
        u_char   frame;
};

#define MSF_TO_LBA(msf)                \
        (((((msf).minute * 60UL) + (msf).second) * 75UL) + (msf).frame - 150)

struct _CDTOC_Desc {
        u_char        session;
        u_char        ctrl_adr;  /* typed to be machine and compiler independent */
        u_char        tno;
        u_char        point;
        struct _CDMSF address;
        u_char        zero;
        struct _CDMSF p;
};

struct _CDTOC {
        u_short            length;  /* in native cpu endian */
        u_char             first_session;
        u_char             last_session;
        struct _CDTOC_Desc trackdesc[1];
};

static kern_return_t FindEjectableCDMedia(io_iterator_t *mediaIterator, mach_port_t *masterPort);
static kern_return_t GetDeviceFilePath(io_iterator_t mediaIterator, char *deviceFilePath, CFIndex maxPathSize);
//int OpenDrive(const char *deviceFilePath);
static struct _CDTOC *ReadTOC(const char *devpath);

static char CDDevicePath[MAXPATHLEN];

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE    2048

#elif !defined(WIN32) // all others (Irix, Tru64)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
#endif

#include <stdio.h>

#ifdef __APPLE__
static kern_return_t FindEjectableCDMedia(io_iterator_t *mediaIterator,
                                           mach_port_t *masterPort)
{
  kern_return_t kernResult;
  CFMutableDictionaryRef     classesToMatch;
  kernResult = IOMasterPort(bootstrap_port, masterPort);
  if (kernResult != KERN_SUCCESS)
  {
      fprintf (stderr, "IOMasterPort returned %d\n", kernResult);
      return kernResult;
  }
  // CD media are instances of class kIOCDMediaClass.
  classesToMatch = IOServiceMatching(kIOCDMediaClass);
  if (classesToMatch == NULL)
    fprintf (stderr, "IOServiceMatching returned a NULL dictionary.\n");
  else
  {
    // Each IOMedia object has a property with key kIOMediaEjectableKey
    // which is true if the media is indeed ejectable. So add property
    // to CFDictionary for matching.
    CFDictionarySetValue(classesToMatch,
                         CFSTR(kIOMediaEjectableKey), kCFBooleanTrue);
  }
  kernResult = IOServiceGetMatchingServices(*masterPort,
                                             classesToMatch, mediaIterator);
  if ((kernResult != KERN_SUCCESS) || (*mediaIterator == NULL))
    fprintf(stderr, "No ejectable CD media found.\n kernResult = %d\n", kernResult);

  return kernResult;
}

static kern_return_t GetDeviceFilePath(io_iterator_t mediaIterator,
                                        char *deviceFilePath, CFIndex maxPathSize)
{
  io_object_t nextMedia;
  kern_return_t kernResult = KERN_FAILURE;
  nextMedia = IOIteratorNext(mediaIterator);
  if (nextMedia == NULL)
  {
      *deviceFilePath = '\0';
  }
  else
  {
      CFTypeRef deviceFilePathAsCFString;
      deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(nextMedia, CFSTR(kIOBSDNameKey),
                                                                 kCFAllocatorDefault, 0);
      *deviceFilePath = '\0';
      if (deviceFilePathAsCFString)
      {
          size_t devPathLength = strlen(_PATH_DEV);
          strcpy(deviceFilePath, _PATH_DEV);
          if (CFStringGetCString((const __CFString *) deviceFilePathAsCFString,
                                  deviceFilePath + devPathLength,
                                  maxPathSize - devPathLength,
                                  kCFStringEncodingASCII))
          {
              // fprintf(stderr, "BSD path: %s\n", deviceFilePath);
              kernResult = KERN_SUCCESS;
          }
          CFRelease(deviceFilePathAsCFString);
      }
  }

  IOObjectRelease(nextMedia);
  return kernResult;
}

static int OpenDrive(const char *deviceFilePath)
{
  int fileDescriptor = open(deviceFilePath, O_RDONLY);
  if (fileDescriptor == -1)
    fprintf(stderr, "Error %d opening device %s.\n", errno, deviceFilePath);
  return fileDescriptor;
}

static struct _CDTOC * ReadTOC(const char *devpath)
{
  struct _CDTOC * toc_p = NULL;
  io_iterator_t iterator = 0;
  io_registry_entry_t service = 0;
  CFDictionaryRef properties = 0;
  CFDataRef data = 0;
  mach_port_t port = 0;
  char *devname;

  if ((devname = strrchr(devpath, '/')) != NULL) {
    ++devname;
  }
  else {
    devname = (char *) devpath;
  }

  if (IOMasterPort(bootstrap_port, &port) != KERN_SUCCESS) {
    fprintf(stderr, "IOMasterPort failed\n");
    goto Exit;
  }

  if (IOServiceGetMatchingServices(port, IOBSDNameMatching(port, 0, devname),
                                  &iterator) != KERN_SUCCESS) {
    fprintf(stderr, "IOServiceGetMatchingServices failed\n");
    goto Exit;
  }

  service = IOIteratorNext(iterator);

  IOObjectRelease(iterator);

  iterator = 0;

  while (service && !IOObjectConformsTo(service, "IOCDMedia")) {
    if (IORegistryEntryGetParentIterator(service, kIOServicePlane,
                                        &iterator) != KERN_SUCCESS)
    {
      fprintf(stderr, "IORegistryEntryGetParentIterator failed\n");
      goto Exit;
    }

    IOObjectRelease(service);
    service = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
  }

  if (service == NULL) {
    fprintf(stderr, "CD media not found\n");
    goto Exit;
  }

  if (IORegistryEntryCreateCFProperties(service, (__CFDictionary **) &properties,
                                          kCFAllocatorDefault,
                                          kNilOptions) != KERN_SUCCESS)
  {
    fprintf(stderr, "IORegistryEntryGetParentIterator failed\n");
    goto Exit;
  }

  data = (CFDataRef) CFDictionaryGetValue(properties, CFSTR(kIOCDMediaTOCKey));
  if (data == NULL) {
    fprintf(stderr, "CFDictionaryGetValue failed\n");
    goto Exit;
  }
  else {
    CFRange range;
    CFIndex buflen;

    buflen = CFDataGetLength(data) + 1;
    range = CFRangeMake(0, buflen);
    toc_p = (struct _CDTOC *) malloc(buflen);
    if (toc_p == NULL) {
      fprintf(stderr, "Out of memory\n");
      goto Exit;
    }
    else {
      CFDataGetBytes(data, range, (unsigned char *) toc_p);
    }

    /*
    fprintf(stderr, "Table of contents\n length %d first %d last %d\n",
            toc_p->length, toc_p->first_session, toc_p->last_session);
    */

    CFRelease(properties);
  }

Exit:

  if (service) {
    IOObjectRelease(service);
  }

  return toc_p;
}
#endif


static unsigned int cdrom_count = 0;

cdrom_interface::cdrom_interface(const char *dev)
{
  char prefix[6];

  sprintf(prefix, "CD%d", ++cdrom_count);
  put(prefix);
  fd = -1; // File descriptor not yet allocated

  if (dev == NULL) {
    path = NULL;
  } else {
    path = strdup(dev);
  }
  using_file=0;
}

cdrom_interface::~cdrom_interface(void)
{
  if (fd >= 0)
    close(fd);
  if (path)
    free(path);
  BX_DEBUG(("Exit"));
}

bx_bool cdrom_interface::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO (("load cdrom with path=%s", path));
#if defined(__APPLE__)
      if(strcmp(path, "drive") == 0)
      {
        mach_port_t masterPort = NULL;
        io_iterator_t mediaIterator;
        kern_return_t kernResult;

        BX_INFO(("Insert CDROM"));

        kernResult = FindEjectableCDMedia(&mediaIterator, &masterPort);
        if (kernResult != KERN_SUCCESS) {
          BX_INFO(("Unable to find CDROM"));
          return 0;
        }

        kernResult = GetDeviceFilePath(mediaIterator, CDDevicePath, sizeof(CDDevicePath));
        if (kernResult != KERN_SUCCESS) {
          BX_INFO(("Unable to get CDROM device file path"));
          return 0;
        }

        // Here a cdrom was found so see if we can read from it.
        // At this point a failure will result in panic.
        if (strlen(CDDevicePath)) {
          fd = open(CDDevicePath, O_RDONLY);
        }
      }
      else {
        fd = open(path, O_RDONLY);
      }
#else
      // all platforms except win32
      fd = open(path, O_RDONLY);
#endif
    if (fd < 0) {
       BX_ERROR(("open cd failed for %s: %s", path, strerror(errno)));
       return 0;
    }
    // do fstat to determine if it's a file or a device, then set using_file.
    struct stat stat_buf;
    ret = fstat (fd, &stat_buf);
    if (ret) {
      BX_PANIC (("fstat cdrom file returned error: %s", strerror (errno)));
    }
    if (S_ISREG (stat_buf.st_mode)) {
      using_file = 1;
      BX_INFO (("Opening image file as a cd."));
    } else {
      using_file = 0;
      BX_INFO (("Using direct access for cdrom."));
    }

  // I just see if I can read a sector to verify that a
  // CD is in the drive and readable.
  return read_block(buffer, 0, 2048);
}

bx_bool cdrom_interface::start_cdrom()
{
  // Spin up the cdrom drive.

  if (fd >= 0) {
#if defined(__NetBSD__) || defined(__NetBSD_kernel__)
    if (ioctl (fd, CDIOCSTART) < 0)
       BX_DEBUG(("start_cdrom: start returns error: %s", strerror (errno)));
    return 1;
#else
    BX_INFO(("start_cdrom: your OS is not supported yet"));
    return 0; // OS not supported yet, return 0 always
#endif
  }
  return 0;
}

void cdrom_interface::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
#if (defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))
    (void) ioctl (fd, CDIOCALLOW);
    if (ioctl (fd, CDIOCEJECT) < 0)
      BX_DEBUG(("eject_cdrom: eject returns error"));
#endif


#if __linux__
    if (!using_file)
      ioctl (fd, CDROMEJECT, NULL);
#endif

    close(fd);
    fd = -1;
  }
}

bx_bool cdrom_interface::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
{
  // Read CD TOC. Returns 0 if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    return 0;
  }

  // This is a hack and works okay if there's one rom track only
  if (using_file || (format != 0)) {
    return create_toc(buf, length, msf, start_track, format);
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
#elif defined(__APPLE__)
  // Read CD TOC. Returns 0 if start track is out of bounds.

#if 1
  {
  struct _CDTOC *toc = ReadTOC(CDDevicePath);

  if ((start_track > toc->last_session) && (start_track != 0xaa))
    return 0;

  buf[2] = toc->first_session;
  buf[3] = toc->last_session;

  if (start_track < toc->first_session)
    start_track = toc->first_session;

  int len = 4;
  for (int i = start_track; i <= toc->last_session; i++) {
    buf[len++] = 0; // Reserved
    buf[len++] = toc->trackdesc[i].ctrl_adr ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = toc->trackdesc[i].address.minute;
      buf[len++] = toc->trackdesc[i].address.second;
      buf[len++] = toc->trackdesc[i].address.frame;
    } else {
      unsigned lba = (unsigned)(MSF_TO_LBA(toc->trackdesc[i].address));
      buf[len++] = (lba >> 24) & 0xff;
      buf[len++] = (lba >> 16) & 0xff;
      buf[len++] = (lba >> 8) & 0xff;
      buf[len++] = (lba >> 0) & 0xff;
    }
  }

  // Lead out track
  buf[len++] = 0; // Reserved
  buf[len++] = 0x16; // ADR, control
  buf[len++] = 0xaa; // Track number
  buf[len++] = 0; // Reserved

  Bit32u blocks = capacity();

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

  *length = len;

  return 1;
  }
#else
  BX_INFO(("Read TOC - Not Implemented"));
  return 0;
#endif
#else
  BX_INFO(("read_toc: your OS is not supported yet"));
  return 0; // OS not supported yet, return 0 always.
#endif
}

Bit32u cdrom_interface::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    // return length of the image file
    struct stat stat_buf;
    int ret = fstat (fd, &stat_buf);
    if (ret) {
       BX_PANIC (("fstat on cdrom image returned err: %s", strerror(errno)));
    }
    if ((stat_buf.st_size % 2048) != 0)  {
      BX_ERROR (("expected cdrom image to be a multiple of 2048 bytes"));
    }
    return (stat_buf.st_size / 2048);
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
#elif defined __APPLE__
// Find the size of the first data track on the cd.  This has produced
// the same results as the linux version on every cd I have tried, about
// 5.  The differences here seem to be that the entries in the TOC when
// retrieved from the IOKit interface appear in a reversed order when
// compared with the linux READTOCENTRY ioctl.
  {
  // Return CD-ROM capacity.  I believe you want to return
  // the number of bytes of capacity the actual media has.

  BX_INFO(("Capacity"));

  struct _CDTOC *toc = ReadTOC(CDDevicePath);

  if (toc == NULL) {
    BX_PANIC(("capacity: Failed to read toc"));
  }

  size_t toc_entries = (toc->length - 2) / sizeof(struct _CDTOC_Desc);

  BX_DEBUG(("reading %d toc entries\n", toc_entries));

  int start_sector = -1;
  int data_track = -1;

  // Iterate through the list backward. Pick the first data track and
  // get the address of the immediately previous (or following depending
  // on how you look at it).  The difference in the sector numbers
  // is returned as the sized of the data track.
  for (int i=toc_entries - 1; i>=0; i--) {
    BX_DEBUG(("session %d ctl_adr %d tno %d point %d lba %d z %d p lba %d\n",
             (int)toc->trackdesc[i].session,
             (int)toc->trackdesc[i].ctrl_adr,
             (int)toc->trackdesc[i].tno,
             (int)toc->trackdesc[i].point,
             MSF_TO_LBA(toc->trackdesc[i].address),
             (int)toc->trackdesc[i].zero,
             MSF_TO_LBA(toc->trackdesc[i].p)));

    if (start_sector != -1) {
      start_sector = MSF_TO_LBA(toc->trackdesc[i].p) - start_sector;
      break;
    }

    if ((toc->trackdesc[i].ctrl_adr >> 4) != 1) continue;

    if (toc->trackdesc[i].ctrl_adr & 0x04) {
      data_track = toc->trackdesc[i].point;
      start_sector = MSF_TO_LBA(toc->trackdesc[i].p);
    }
  }

  free(toc);

  if (start_sector == -1) {
    start_sector = 0;
  }

  BX_INFO(("first data track %d data size is %d", data_track, start_sector));

  return start_sector;
  }
#else
  BX_ERROR(("capacity: your OS is not supported yet"));
  return(0);
#endif
}

bx_bool BX_CPP_AttrRegparmN(3) cdrom_interface::read_block(Bit8u* buf, Bit32u lba, int blocksize)
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
#if defined(__APPLE__)
#define CD_SEEK_DISTANCE kCDSectorSizeWhole
    if(using_file)
    {
      pos = lseek(fd, (off_t) lba * BX_CD_FRAMESIZE, SEEK_SET);
      if (pos < 0) {
        BX_PANIC(("cdrom: read_block: lseek returned error."));
      } else {
        n = read(fd, buf1, BX_CD_FRAMESIZE);
      }
    }
    else
    {
      // This seek will leave us 16 bytes from the start of the data
      // hence the magic number.
      pos = lseek(fd, (off_t) lba * CD_SEEK_DISTANCE + 16, SEEK_SET);
      if (pos < 0) {
        BX_PANIC(("cdrom: read_block: lseek returned error."));
      } else {
        n = read(fd, buf1, CD_FRAMESIZE);
      }
    }
#else
    pos = lseek(fd, (off_t) lba * BX_CD_FRAMESIZE, SEEK_SET);
    if (pos < 0) {
      BX_PANIC(("cdrom: read_block: lseek returned error."));
    } else {
      n = read(fd, (char*) buf1, BX_CD_FRAMESIZE);
    }
#endif
  } while ((n != BX_CD_FRAMESIZE) && (--try_count > 0));

  return (n == BX_CD_FRAMESIZE);
}

#endif /* if BX_SUPPORT_CDROM */

#endif
