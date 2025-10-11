/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2021  The Bochs Project
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

// These are the low-level CDROM functions which are called from 'harddrv.cc'.
// Notes:
//  1) Assumes the Host will be WinXP or higher (the check has been removed)
//  2) Win10 still supports a lot of the DeviceIoControl() functions for the
//     audio CDROM, but *does not* support the playback DeviceIoControl() even
//     though it returns success.
//  3) Therefore, the playback calls a technique newer than DeviceIoControl() 
//     but older than MediaPlayer() called Media Control Interface (MCI).
//  4) MCI is old enough, it assumes all addresses will be below the 4 gig
//     mark and all addresses will be 32-bits wide. If Bochs is ran above
//     the 4gig mark, this code will fail. (Checks are in place)
//  5) Since MCI doesn't support all calls, *and* DeviceIoControl() doesn't
//     support all calls (on Windows Vista and above), a mix of each is used.
//  6) If your Windows system is before Vista, you can set the #define below
//     to force this code to use the DeviceIoControl() techniqe for all calls.
//  7) This does not check to see if the Guest has a sound device installed.
//     If the guest doesn't have a sound device, this should not play. FIX ME.

#include "bochs.h"
#if BX_SUPPORT_CDROM

// set this to 1 if you run this on a Windows Host that is before Windows Vista
#define WIN_CDROM_FORCE_IOCTRL  0

#include "cdrom.h"
#include "cdrom_win32.h"

#define LOG_THIS /* no SMF tricks here, not needed */

extern "C" {
#include <errno.h>
}

#if defined(WIN32)
// windows.h included by bochs.h
#include <winioctl.h>
#include <Ntddcdrm.h>

#define BX_CD_FRAMESIZE     2048
#define BX_RAW_SECTOR_SIZE  2352

#include <stdio.h>

cdrom_win32_c::cdrom_win32_c(const char *dev)
{
  char prefix[6];

  sprintf(prefix, "CD%d", ++bx_cdrom_count);
  put(prefix);
  fd = -1; // File descriptor not yet allocated
  cdrom_type = CDROM_TYPE_DATA;  // assume a data CD
  tot_tracks = 0;

  if (dev == NULL) {
    path = NULL;
  } else {
    path = strdup(dev);
  }
  using_file = 0;

#if !WIN_CDROM_FORCE_IOCTRL
  // We also need to check that we aren't above 32-bit wide
  if ((Bit64u) &wDeviceID > 0xFFFFFFFFULL)
    BX_PANIC(("The MCI code assumes 32-bit addresses!"));
  mciStatus = AUDIO_STATUS_NO_CURRENT;
#endif
}

cdrom_win32_c::~cdrom_win32_c(void)
{
  if (fd >= 0) {
    if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle(hFile);
#if !WIN_CDROM_FORCE_IOCTRL
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
#endif
    fd = -1;
  }
}

// lock or unlock the cdrom
bool cdrom_win32_c::lock_cdrom(bool lock)
{
  PREVENT_MEDIA_REMOVAL pmr;
  DWORD iBytesReturned;

  pmr.PreventMediaRemoval = lock;
 	return DeviceIoControl(hFile, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &iBytesReturned, NULL);
}

// convert MSF to LBA
Bit32u cdrom_win32_c::msf2lba(Bit8u mins, Bit8u secs, Bit8u frames)
{
  // LBA = (((m * 60) + s) * 75 + f) - LEAD_OFF_FRAMES
  return ((((mins * 60) + secs) * 75) + frames) - 150;
}

// convert LBA to MSF
void cdrom_win32_c::lba2msf(int lba, Bit8u *mins, Bit8u *secs, Bit8u *frames)
{
  *mins = lba / 4500;
  *secs = (lba % 4500) / 75;
  *frames = lba % 75;
}

// convert a whole disk MSF to a Track and a MSF on that track
int cdrom_win32_c::msf2tmsf(Bit8u *mins, Bit8u *secs, Bit8u *frames)
{
  int lba = msf2lba(*mins, *secs, *frames);

  for (int i=0; i<tot_tracks; i++) {
    if ((lba < track_info[0].address) ||
       ((lba >= track_info[i].address) && (lba < (track_info[i].address + track_info[i].length)))) {
      lba -= track_info[i].address;
      lba2msf(lba, mins, secs, frames);
      return i + 1; // one-based
    }
  }
  
  return 0;
}

bool cdrom_win32_c::insert_cdrom(const char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  CDROM_READ_TOC_EX input;
  DWORD iBytesReturned = 0;
  CDROM_TOC *toc_table;

  // Load CD-ROM. Returns 0 if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO (("load cdrom with path='%s'", path));
  char drive[256];
  if ((path[1] == ':') && (strlen(path) == 2))
  {
    // Use direct device access under Windows XP or newer

    // With all the backslashes it's hard to see, but to open D: drive
    // the name would be: \\.\d:
    sprintf(drive, "\\\\.\\%s", path);
    BX_INFO (("Using direct access for cdrom."));
    // This trick only works for Win2k and WinNT, so warn the user of that.
    using_file = 0;
  } else {
    strcpy(drive,path);
    using_file = 1;
    BX_INFO (("Opening image file as a cd"));
  }
  hFile = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
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

  if (!using_file) {
    // We need to determine if the disc is a DATA disk or an Audio disk.
    // To do so, we get the TOC and then check the CONTROL field of each
    //  track returned: (bit 1 indicates copy protection and is ignored here)
    //  00c0 = audio
    //  00c1 = audio
    //  10c1 = audio
    //  10c1 = audio
    //  01c0 = data
    //  01c1 = data
    //  11xx = reserved
    
    // lock drive
    lock_cdrom(TRUE);
    
    // read the TOC
    toc_table = (CDROM_TOC *) buffer;
    memset(toc_table, 0, sizeof(CDROM_TOC)); // make sure all are zero, so the track_info.length calculation below is accurate on the last track calculation
    memset(&input, 0, sizeof(input));        // the call fails if the reserved bytes are not zero!!!!!
    input.Format = CDROM_READ_TOC_EX_FORMAT_TOC;
    input.Msf = TRUE;
    input.SessionTrack = 1;
    if (DeviceIoControl(hFile, IOCTL_CDROM_READ_TOC_EX, &input, sizeof(input), toc_table, sizeof(CDROM_TOC), &iBytesReturned, NULL)) {
      // unlock drive
      lock_cdrom(FALSE);
      
      // check all of the tracks to see if they are all one type
      // (the returned list will include the AA track)
      tot_tracks = toc_table->LastTrack - toc_table->FirstTrack + 1;
      bool data = FALSE, audio = FALSE;
      for (int i=0; i<tot_tracks + 1; i++) {
        switch (toc_table->TrackData[i].Control & 0b1101) {
          case 0b0000:
          case 0b0001:
          case 0b1000:
          case 0b1001:
            // if is an audio track, store the information
            track_info[i].address = msf2lba(toc_table->TrackData[i].Address[1], toc_table->TrackData[i].Address[2], toc_table->TrackData[i].Address[3]);
            track_info[i].length = msf2lba(toc_table->TrackData[i+1].Address[1], toc_table->TrackData[i+1].Address[2], toc_table->TrackData[i+1].Address[3]) - track_info[i].address;
            audio = TRUE;
            break;
          case 0b0100:
          case 0b0101:
            data = TRUE;
            break;
        }
      }
      
      // if after the above, neither or both 'data' and 'audio' are set,
      //  we don't recognize the disc.
      // if only one or the other is set, we can continue.
      if ((data && audio) || (!data && !audio))  {
        BX_ERROR(("Unrecognized data/audio disc in drive. Returning a non-inserted state."));
        cdrom_type = CDROM_TYPE_UNKNOWN;
        return 0;
      } else if (data && !audio) {
        // I just see if I can read a sector to verify that a CD is in the drive and readable.
        fd = (read_block(buffer, 0x10, BX_CD_FRAMESIZE)) ? 1 : -1;
        if (fd && (memcmp(buffer, "\x00\x42\x45\x41\x30\x31\x01\x00", 8) == 0)) {
          BX_INFO(("* Direct Media Access detected a Universal Disc Format (UDF) file system. (DVD?)"));
          BX_INFO(("* Older Guests may not recognize the volume, giving a false indication that"));
          BX_INFO(("*  Bochs' Direct Disc access is malfunctioning."));
        }
        cdrom_type = CDROM_TYPE_DATA;
      } else if (!data && audio) {
        // if it is audio, we got the track information above
        cdrom_type = CDROM_TYPE_AUDIO;
      }
    }

#if !WIN_CDROM_FORCE_IOCTRL
    // Open the CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = "cdaudio";
    if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms)) {
      // Failed to open device. Don't close it; just return error.
      return 0;
    }
    
    // The device opened successfully; get the device ID.
    wDeviceID = mciOpenParms.wDeviceID;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    if (mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms)) {
      mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
      return 0;
    } 
    fd = 1;
#endif
  }

  return (fd == 1);
}

int cdrom_win32_c::read_sub_channel(Bit8u* buf, bool sub_q, bool msf, int start_track, int format, int alloc_length)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  CDROM_SUB_Q_DATA_FORMAT q_data_format;
  DWORD iBytesReturned = 0;
  
  if (using_file) {
    return cdrom_base_c::read_sub_channel(buf, sub_q, msf, start_track, format, alloc_length);
  } else {
    // IOCTL_CDROM_CURRENT_POSITION = 1, IOCTL_CDROM_MEDIA_CATALOG = 2, IOCTL_CDROM_TRACK_ISRC = 3
    q_data_format.Format = format;
    q_data_format.Track = start_track;
    if (DeviceIoControl(hFile, IOCTL_CDROM_READ_Q_CHANNEL, &q_data_format, sizeof(q_data_format), buffer, BX_CD_FRAMESIZE, &iBytesReturned, NULL)) {
      // WinXP expects the Audio Status byte to be valid, if not it assumes 
      //  an error with that track, and moves to the next one.
      buffer[1] = mciStatus;
      if (!sub_q) iBytesReturned = 4;
      memcpy(buf, buffer, BX_MIN(iBytesReturned, alloc_length));
      return iBytesReturned;
    } else
      return 0;
  }
}

void cdrom_win32_c::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
    if (!using_file) {
      DWORD lpBytesReturned;
      DeviceIoControl(hFile, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &lpBytesReturned, NULL);
#if !WIN_CDROM_FORCE_IOCTRL
      mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
#endif
    } else {
      if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
      }
    }
    fd = -1;
  }
}

// Read CD TOC. Returns 0 if start track is out of bounds.
bool cdrom_win32_c::read_toc(Bit8u* buf, int* length, bool msf, int start_track, int format)
{
  CDROM_READ_TOC_EX input;
  DWORD iBytesReturned;
  UCHAR data[804];

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    return 0;
  }
  
  // This is a hack and works okay if there's one rom track only
  if (using_file) {
    return cdrom_base_c::read_toc(buf, length, msf, start_track, format);
  } else {
    // the implementation below is the platform-dependent code required
    // to read the TOC from a physical cdrom.
    // This only works with WinXP or newer
    memset(&input, 0, sizeof(input)); // the call fails if the reserved bytes are not zero!!!!!
    input.Format = format;
    input.Msf = msf;
    input.SessionTrack = start_track;
    
    if (DeviceIoControl(hFile, IOCTL_CDROM_READ_TOC_EX, &input, sizeof(input), data, 804, &iBytesReturned, NULL)) {
      // now copy it to the users buffer and free our buffer
      *length = data[1] + (data[0] << 8) + 2;
      memcpy(buf, data, *length);
      return 1;
    }
  }
  
  return 0;
}

Bit32u cdrom_win32_c::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

  if (using_file) {
    ULARGE_INTEGER FileSize;
    FileSize.LowPart = GetFileSize(hFile, &FileSize.HighPart);
    return (Bit32u)(FileSize.QuadPart / 2048);
  } else {  /* direct device access */
    LARGE_INTEGER length;
    DWORD iBytesReturned;
    DeviceIoControl(hFile, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &length, sizeof(length), &iBytesReturned, NULL);
    return (Bit32u)(length.QuadPart / 2048);
  }
}

bool BX_CPP_AttrRegparmN(3) cdrom_win32_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
{
  // Read a single block from the CD
  LARGE_INTEGER pos;
  DWORD iBytesReturned = 0;
  Bit8u try_count = 3;
  Bit8u* buf1;

  if (using_file && (blocksize == 2352)) {
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
  
  if (cdrom_type == CDROM_TYPE_DATA) {
    do {
      pos.QuadPart = (LONGLONG) lba * BX_CD_FRAMESIZE;
      pos.LowPart = SetFilePointer(hFile, pos.LowPart, &pos.HighPart, SEEK_SET);
      if ((pos.LowPart == 0xffffffff) && (GetLastError() != NO_ERROR)) {
        BX_PANIC(("cdrom: read_block: SetFilePointer returned error."));
      } else {
        ReadFile(hFile, (void *) buf1, BX_CD_FRAMESIZE, &iBytesReturned, NULL);
      }
    } while ((iBytesReturned != BX_CD_FRAMESIZE) && (--try_count > 0));
    return (iBytesReturned == BX_CD_FRAMESIZE);
      
  } else if ((cdrom_type == CDROM_TYPE_AUDIO) && !using_file) {
    do {
      // lock drive
      lock_cdrom(TRUE);
        
      void *buffer = VirtualAlloc(NULL, BX_RAW_SECTOR_SIZE, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
      if (buffer != NULL) {
        RAW_READ_INFO raw_read;
        raw_read.DiskOffset.QuadPart = (/*(LONGLONG) track_info[0].address +*/ lba) * BX_CD_FRAMESIZE; // TODO: we need a track number?
        raw_read.SectorCount = 1;
        raw_read.TrackMode = CDDA;
        if (DeviceIoControl(hFile, IOCTL_CDROM_RAW_READ, &raw_read, sizeof(raw_read), buffer, BX_RAW_SECTOR_SIZE, &iBytesReturned, NULL)) {
          memcpy(buf, buffer, blocksize);
        } else
          iBytesReturned = 0;
        VirtualFree(buffer, 0, MEM_RELEASE);
      }
        
      // unlock drive
      lock_cdrom(FALSE);
    } while ((iBytesReturned != BX_RAW_SECTOR_SIZE) && (--try_count > 0));
    
    return (iBytesReturned == BX_RAW_SECTOR_SIZE);
  }

  return 0;
}

bool cdrom_win32_c::seek(Bit32u lba)
{
  unsigned char buffer[BX_RAW_SECTOR_SIZE];

  Bit32u block_size = (cdrom_type == CDROM_TYPE_AUDIO) ? BX_RAW_SECTOR_SIZE : BX_CD_FRAMESIZE;
  return read_block(buffer, lba, block_size);
}

// play audio lba
bool cdrom_win32_c::play_audio(Bit32u lba, Bit32u length) {
  bool ret = 0;

#if WIN_CDROM_FORCE_IOCTRL
  if (!using_file) {
    CDROM_PLAY_AUDIO_MSF input;
    input.StartingM = lba / 4500;
    input.StartingS = (lba % 4500) / 75;
    input.StartingF = lba % 75;
    lba += length;
    input.EndingM = lba / 4500;
    input.EndingS = (lba % 4500) / 75;
    input.EndingF = lba % 75;
    ret = DeviceIoControl(hFile, IOCTL_CDROM_PLAY_AUDIO_MSF, &input, sizeof(input), NULL, 0, NULL, NULL);
  }
#else
  Bit8u mins, secs, frames;
  int start_track, end_track;

  // The call will have starting and ending values for the whole disc.
  //  We have to calculate and adjust for the track
  lba2msf(lba, &mins, &secs, &frames);
  start_track = msf2tmsf(&mins, &secs, &frames);
//  BX_INFO(("start: %i %i %i   %i: %i %i %i", buf[3], buf[4], buf[5], start_track, mins, secs, frames));
  if (!start_track)
    return 0;

  mciPlayParms.dwFrom = MCI_MAKE_TMSF(start_track, mins, secs, frames);

  lba += length;
  lba2msf(lba, &mins, &secs, &frames);
  end_track = msf2tmsf(&mins, &secs, &frames);
//  BX_INFO(("end: %i %i %i   %i: %i %i %i", buf[6], buf[7], buf[8], end_track, mins, secs, frames));
  if (!end_track)
    return 0;
  mciPlayParms.dwTo = MCI_MAKE_TMSF(end_track, mins, secs, frames);

  if (mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD)(LPVOID) &mciPlayParms)) {
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
  } else
    ret = 1;
  
  if (ret == 1)
    mciStatus = AUDIO_STATUS_PLAYING;
#endif
  
  return ret;
}

// play audio msf
bool cdrom_win32_c::play_audio_msf(Bit8u* buf) {
  bool ret = 0;

#if WIN_CDROM_FORCE_IOCTRL
  if (!using_file) {
    CDROM_PLAY_AUDIO_MSF input;
    input.StartingM = buf[3];
    input.StartingS = buf[4];
    input.StartingF = buf[5];
    input.EndingM = buf[6];
    input.EndingS = buf[7];
    input.EndingF = buf[8];
    ret = DeviceIoControl(hFile, IOCTL_CDROM_PLAY_AUDIO_MSF, &input, sizeof(input), NULL, 0, NULL, NULL);
  }
#else  
  Bit8u mins, secs, frames;
  int start_track, end_track;

  // The call will have starting and ending values for the whole disc.
  //  We have to calculate and adjust for the track
  mins = buf[3];
  secs = buf[4];
  frames = buf[5];
  start_track = msf2tmsf(&mins, &secs, &frames);
//  BX_INFO(("start: %i %i %i   %i: %i %i %i", buf[3], buf[4], buf[5], start_track, mins, secs, frames));
  if (!start_track)
    return 0;

  mciPlayParms.dwFrom = MCI_MAKE_TMSF(start_track, mins, secs, frames);

  mins = buf[6];
  secs = buf[7];
  frames = buf[8];
  end_track = msf2tmsf(&mins, &secs, &frames);
//  BX_INFO(("end: %i %i %i   %i: %i %i %i", buf[6], buf[7], buf[8], end_track, mins, secs, frames));
  if (!end_track)
    return 0;
  mciPlayParms.dwTo = MCI_MAKE_TMSF(end_track, mins, secs, frames);

  if (mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD)(LPVOID) &mciPlayParms)) {
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
  } else
    ret = 1;
  
  if (ret == 1)
    mciStatus = AUDIO_STATUS_PLAYING;
#endif

  return ret;
}

bool cdrom_win32_c::stop_audio(void) {
  bool ret = 0;
  
#if WIN_CDROM_FORCE_IOCTRL
  if (!using_file)
    ret = DeviceIoControl(hFile, IOCTL_CDROM_STOP_AUDIO, NULL, 0, NULL, 0, NULL, NULL);
#else
  mciSendCommand(wDeviceID, MCI_STOP, 0, NULL);
  mciStatus = AUDIO_STATUS_DONE;
  ret = 1;
#endif
  return ret;
}

bool cdrom_win32_c::pause_resume_audio(bool pause) {
  bool ret = 0;

  if (!using_file) {
    if (pause) {
#if WIN_CDROM_FORCE_IOCTRL
      ret = DeviceIoControl(hFile, IOCTL_CDROM_PAUSE_AUDIO, NULL, 0, NULL, 0, NULL, NULL);
#else
      mciSendCommand(wDeviceID, MCI_PAUSE, 0, NULL);
      mciStatus = AUDIO_STATUS_PAUSED;
      ret = 1;
#endif
    } else {
#if WIN_CDROM_FORCE_IOCTRL
      ret = DeviceIoControl(hFile, IOCTL_CDROM_RESUME_AUDIO, NULL, 0, NULL, 0, NULL, NULL);
#else      
      mciSendCommand(wDeviceID, MCI_RESUME, 0, NULL);
      mciStatus = AUDIO_STATUS_PLAYING;
      ret = 1;
#endif
    }
  }
  
  return ret;
}

#endif /* WIN32 */

#endif /* if BX_SUPPORT_CDROM */
