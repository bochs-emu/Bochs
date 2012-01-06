/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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


class cdrom_base_c : public logfunctions {
public:
  virtual ~cdrom_base_c(void) {}

  // Load CD-ROM. Returns 0 if CD is not ready.
  virtual bx_bool insert_cdrom(const char *dev = NULL) = 0;

  // Logically eject the CD.
  virtual void eject_cdrom() = 0;

  // Read CD TOC. Returns 0 if start track is out of bounds.
  virtual bx_bool read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format) = 0;

  // Return CD-ROM capacity (in 2048 byte frames)
  virtual Bit32u capacity() = 0;

  // Read a single block from the CD. Returns 0 on failure.
  virtual bx_bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3) = 0;

  // Start (spin up) the CD.
  virtual bx_bool start_cdrom() = 0;

  // Seek for new block address.
  virtual bx_bool seek(Bit32u lba) = 0;

  // Create CD TOC from image. Called from read_toc().
  virtual bx_bool create_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format) = 0;
};

class cdrom_interface : public cdrom_base_c {
public:
  cdrom_interface(const char *dev);
  virtual ~cdrom_interface(void);

  bx_bool insert_cdrom(const char *dev = NULL);
  void eject_cdrom();
  bx_bool read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format);
  Bit32u capacity();
  bx_bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3);
  bx_bool start_cdrom();
  bx_bool seek(Bit32u lba);
  bx_bool create_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format);

private:
  int fd;
  char *path;

  int using_file;
#ifdef WIN32
  BOOL bUseASPI;
  HANDLE hFile;
  int hid;
  int tid;
  int lun;
#endif
};

