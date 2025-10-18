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


// Header file for low-level OS specific CDROM emulation

extern unsigned int bx_cdrom_count;


class cdrom_base_c : public logfunctions {
public:
  cdrom_base_c() {}
  cdrom_base_c(const char *dev);
  virtual ~cdrom_base_c(void);

  // Load CD-ROM. Returns 0 if CD is not ready.
  virtual bool insert_cdrom(const char *dev = NULL);

  // Logically eject the CD.
  virtual void eject_cdrom();

  // get the sub channel information
  virtual int read_sub_channel(Bit8u* buf, bool sub_q, bool msf, int start_track, int format, int alloc_length);

  // Read CD TOC. Returns 0 if start track is out of bounds.
  virtual bool read_toc(Bit8u* buf, int* length, bool msf, int start_track, int format);

  // Return CD-ROM capacity (in 2048 byte frames)
  virtual Bit32u capacity();

  // Read a single block from the CD. Returns 0 on failure.
  virtual bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3);

  // Start (spin up) the CD.
  virtual bool start_cdrom();

  // Seek for new block address.
  virtual bool seek(Bit32u lba);

  // play audio msf
  virtual bool play_audio(Bit32u lba, Bit32u length);
  virtual bool play_audio_msf(Bit8u* buf);
  virtual bool stop_audio(void);
  virtual bool pause_resume_audio(bool pause);

protected:
  int fd;
  char *path;
  bool using_file;
};
