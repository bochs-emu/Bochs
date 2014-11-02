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
#include "cdrom_win32.h"

#define LOG_THIS /* no SMF tricks here, not needed */

extern "C" {
#include <errno.h>
}

#if defined(WIN32)
// windows.h included by bochs.h
#include <winioctl.h>


static BOOL isWindowsXP;
static BOOL isOldWindows;

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE    2048

// READ_TOC_EX structure(s) and #defines

#define CDROM_READ_TOC_EX_FORMAT_TOC      0x00
#define CDROM_READ_TOC_EX_FORMAT_SESSION  0x01
#define CDROM_READ_TOC_EX_FORMAT_FULL_TOC 0x02
#define CDROM_READ_TOC_EX_FORMAT_PMA      0x03
#define CDROM_READ_TOC_EX_FORMAT_ATIP     0x04
#define CDROM_READ_TOC_EX_FORMAT_CDTEXT   0x05

#define IOCTL_CDROM_BASE              FILE_DEVICE_CD_ROM
#define IOCTL_CDROM_READ_TOC_EX       CTL_CODE(IOCTL_CDROM_BASE, 0x0015, METHOD_BUFFERED, FILE_READ_ACCESS)
#ifndef IOCTL_DISK_GET_LENGTH_INFO
#define IOCTL_DISK_GET_LENGTH_INFO    CTL_CODE(IOCTL_DISK_BASE, 0x0017, METHOD_BUFFERED, FILE_READ_ACCESS)
#endif

typedef struct _CDROM_READ_TOC_EX {
    UCHAR Format    : 4;
    UCHAR Reserved1 : 3; // future expansion
    UCHAR Msf       : 1;
    UCHAR SessionTrack;
    UCHAR Reserved2;     // future expansion
    UCHAR Reserved3;     // future expansion
} CDROM_READ_TOC_EX, *PCDROM_READ_TOC_EX;

typedef struct _TRACK_DATA {
    UCHAR Reserved;
    UCHAR Control : 4;
    UCHAR Adr : 4;
    UCHAR TrackNumber;
    UCHAR Reserved1;
    UCHAR Address[4];
} TRACK_DATA, *PTRACK_DATA;

typedef struct _CDROM_TOC_SESSION_DATA {
    // Header
    UCHAR Length[2];  // add two bytes for this field
    UCHAR FirstCompleteSession;
    UCHAR LastCompleteSession;
    // One track, representing the first track
    // of the last finished session
    TRACK_DATA TrackData[1];
} CDROM_TOC_SESSION_DATA, *PCDROM_TOC_SESSION_DATA;

// End READ_TOC_EX structure(s) and #defines

#include <stdio.h>

cdrom_win32_c::cdrom_win32_c(const char *dev)
{
  char prefix[6];
  OSVERSIONINFO osinfo;

  sprintf(prefix, "CD%d", ++bx_cdrom_count);
  put(prefix);
  fd = -1; // File descriptor not yet allocated

  if (dev == NULL) {
    path = NULL;
  } else {
    path = strdup(dev);
  }
  using_file = 0;
  osinfo.dwOSVersionInfoSize = sizeof(osinfo);
  GetVersionEx(&osinfo);
  isWindowsXP = (osinfo.dwMajorVersion > 5) ||
                ((osinfo.dwMajorVersion == 5) && (osinfo.dwMinorVersion >= 1));
  isOldWindows = (osinfo.dwPlatformId != VER_PLATFORM_WIN32_NT);
}

cdrom_win32_c::~cdrom_win32_c(void)
{
  if (hFile != INVALID_HANDLE_VALUE)
    CloseHandle(hFile);
}

bx_bool cdrom_win32_c::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO (("load cdrom with path='%s'", path));
  char drive[256];
  if ((path[1] == ':') && (strlen(path) == 2))
  {
    if (!isOldWindows) {
      // Use direct device access under windows NT/2k/XP

      // With all the backslashes it's hard to see, but to open D: drive
      // the name would be: \\.\d:
      sprintf(drive, "\\\\.\\%s", path);
      BX_INFO (("Using direct access for cdrom."));
      // This trick only works for Win2k and WinNT, so warn the user of that.
      using_file = 0;
    } else {
      BX_ERROR(("Your Windows version is no longer supported for direct access."));
      return 0;
    }
  } else {
    strcpy(drive,path);
    using_file = 1;
    BX_INFO (("Opening image file as a cd"));
  }
  if (!isOldWindows) {
    hFile = CreateFile((char *)&drive, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
      fd = 1;
    if (!using_file) {
      DWORD lpBytesReturned;
      DeviceIoControl(hFile, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &lpBytesReturned, NULL);
    }
  }
  if (fd < 0) {
     BX_ERROR(("open cd failed for %s: %s", path, strerror(errno)));
     return 0;
  }

  // I just see if I can read a sector to verify that a
  // CD is in the drive and readable.
  return read_block(buffer, 0, 2048);
}

void cdrom_win32_c::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
    if (using_file == 0) {
      if (!isOldWindows) {
        DWORD lpBytesReturned;
        DeviceIoControl(hFile, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &lpBytesReturned, NULL);
      }
    }
    fd = -1;
  }
}

bx_bool cdrom_win32_c::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
{
  // Read CD TOC. Returns 0 if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    return 0;
  }

  // This is a hack and works okay if there's one rom track only
  if (!isWindowsXP || using_file) {
    return cdrom_base_c::read_toc(buf, length, msf, start_track, format);
  }
  // the implementation below is the platform-dependent code required
  // to read the TOC from a physical cdrom.
  if (isWindowsXP) {
    // This only works with WinXP
    CDROM_READ_TOC_EX input;
    memset(&input, 0, sizeof(input));
    input.Format = format;
    input.Msf = msf;
    input.SessionTrack = start_track;

    // We have to allocate a chunk of memory to make sure it is aligned on a sector base.
    UCHAR *data = (UCHAR *) VirtualAlloc(NULL, 2048*2, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    DWORD iBytesReturned;
    DeviceIoControl(hFile, IOCTL_CDROM_READ_TOC_EX, &input, sizeof(input), data, 804, &iBytesReturned, NULL);
    // now copy it to the users buffer and free our buffer
    *length = data[1] + (data[0] << 8) + 2;
    memcpy(buf, data, *length);
    VirtualFree(data, 0, MEM_RELEASE);

    return 1;
  } else {
    return 0;
  }
}

Bit32u cdrom_win32_c::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    ULARGE_INTEGER FileSize;
    FileSize.LowPart = GetFileSize(hFile, &FileSize.HighPart);
    return (Bit32u)(FileSize.QuadPart / 2048);
  } else if (!isOldWindows) {  /* direct device access */
    if (isWindowsXP) {
      LARGE_INTEGER length;
      DWORD iBytesReturned;
      DeviceIoControl(hFile, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &length, sizeof(length), &iBytesReturned, NULL);
      return (Bit32u)(length.QuadPart / 2048);
    } else {
      ULARGE_INTEGER FreeBytesForCaller;
      ULARGE_INTEGER TotalNumOfBytes;
      ULARGE_INTEGER TotalFreeBytes;
      GetDiskFreeSpaceEx(path, &FreeBytesForCaller, &TotalNumOfBytes, &TotalFreeBytes);
      return (Bit32u)(TotalNumOfBytes.QuadPart / 2048);
    }
  } else {
    return 0;
  }
}

bx_bool BX_CPP_AttrRegparmN(3) cdrom_win32_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
{
  // Read a single block from the CD

  LARGE_INTEGER pos;
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
    if (using_file || !isOldWindows) {
      pos.QuadPart = (LONGLONG)lba*BX_CD_FRAMESIZE;
      pos.LowPart = SetFilePointer(hFile, pos.LowPart, &pos.HighPart, SEEK_SET);
      if ((pos.LowPart == 0xffffffff) && (GetLastError() != NO_ERROR)) {
        BX_PANIC(("cdrom: read_block: SetFilePointer returned error."));
      } else {
        ReadFile(hFile, (void *) buf1, BX_CD_FRAMESIZE, (LPDWORD) &n, NULL);
      }
    }
  } while ((n != BX_CD_FRAMESIZE) && (--try_count > 0));

  return (n == BX_CD_FRAMESIZE);
}

#endif /* WIN32 */

#endif /* if BX_SUPPORT_CDROM */
