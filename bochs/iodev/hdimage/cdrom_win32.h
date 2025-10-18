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

#ifdef WIN32

#define CDROM_TYPE_UNKNOWN  0
#define CDROM_TYPE_DATA     1
#define CDROM_TYPE_AUDIO    2

struct CDROM_TRACK_INFO {
  Bit32u address;
  Bit32u length;
};

#if !WIN_CDROM_FORCE_IOCTRL
  #include <mmsystem.h>
#endif
  
#define AUDIO_STATUS_NOT_VALID    0x00
#define AUDIO_STATUS_PLAYING      0x11
#define AUDIO_STATUS_PAUSED       0x12
#define AUDIO_STATUS_DONE         0x13
#define AUDIO_STATUS_STOPPED_ERR  0x14
#define AUDIO_STATUS_NO_CURRENT   0x15

// Header file for low-level OS specific CDROM emulation

class cdrom_win32_c : public cdrom_base_c {
public:
  cdrom_win32_c(const char *dev);
  virtual ~cdrom_win32_c(void);
  bool insert_cdrom(const char *dev = NULL);
  void eject_cdrom();
  int read_sub_channel(Bit8u* buf, bool sub_q, bool msf, int start_track, int format, int alloc_length);
  bool read_toc(Bit8u* buf, int* length, bool msf, int start_track, int format);
  Bit32u capacity();
  bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3);
  bool seek(Bit32u lba);

  bool play_audio(Bit32u lba, Bit32u length);
  bool play_audio_msf(Bit8u* buf);
  bool stop_audio(void);
  bool pause_resume_audio(bool pause);

private:
  bool lock_cdrom(bool lock);
  Bit32u msf2lba(Bit8u mins, Bit8u secs, Bit8u frames);
  void lba2msf(int lba, Bit8u *mins, Bit8u *secs, Bit8u *frames);
  int msf2tmsf(Bit8u *mins, Bit8u *secs, Bit8u *frames);
  HANDLE hFile;
  int  cdrom_type;  // 0 = unknown, 1 = is a data cdrom, 2 = is an audio cdrom
  int  tot_tracks;
  struct CDROM_TRACK_INFO track_info[100];
  Bit8u AudioStatus;

#if !WIN_CDROM_FORCE_IOCTRL
  UINT wDeviceID;
  MCI_OPEN_PARMS mciOpenParms;
  MCI_SET_PARMS mciSetParms;
  MCI_PLAY_PARMS mciPlayParms;
#endif
};

#endif
