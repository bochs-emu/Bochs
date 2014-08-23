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
#include "cdrom_osx.h"

#define LOG_THIS /* no SMF tricks here, not needed */

extern "C" {
#include <errno.h>
}

#if defined(__APPLE__)
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

#include <stdio.h>


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


bx_bool cdrom_osx_c::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO (("load cdrom with path=%s", path));
  if (strcmp(path, "drive") == 0) {
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
  } else {
    fd = open(path, O_RDONLY);
  }
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

bx_bool cdrom_osx_c::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
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
}

Bit32u cdrom_osx_c::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    return cdrom_base_c::capacity();
  }

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

  BX_DEBUG(("reading %d toc entries\n", (int)toc_entries));

  int start_sector = -1;
  int data_track = -1;

  // Iterate through the list backward. Pick the first data track and
  // get the address of the immediately previous (or following depending
  // on how you look at it).  The difference in the sector numbers
  // is returned as the sized of the data track.
  for (int i=toc_entries - 1; i>=0; i--) {
    BX_DEBUG(("session %d ctl_adr %d tno %d point %d lba %ld z %d p lba %ld\n",
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
}

bx_bool BX_CPP_AttrRegparmN(3) cdrom_osx_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
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
  } while ((n != BX_CD_FRAMESIZE) && (--try_count > 0));

  return (n == BX_CD_FRAMESIZE);
}

#endif /* if defined(__APPLE__) */

#endif /* if BX_SUPPORT_CDROM */
