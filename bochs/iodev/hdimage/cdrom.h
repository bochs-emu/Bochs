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


// Header file for low-level OS specific CDROM emulation

extern unsigned int bx_cdrom_count;


class cdrom_base_c : public logfunctions {
public:
  cdrom_base_c() {}
  cdrom_base_c(const char *dev);
  virtual ~cdrom_base_c(void);

  // Load CD-ROM. Returns 0 if CD is not ready.
  virtual bx_bool insert_cdrom(const char *dev = NULL);

  // Logically eject the CD.
  virtual void eject_cdrom();

  // Read CD TOC. Returns 0 if start track is out of bounds.
  virtual bx_bool read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format);

  // Return CD-ROM capacity (in 2048 byte frames)
  virtual Bit32u capacity();

  // Read a single block from the CD. Returns 0 on failure.
  virtual bx_bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3);

  // Start (spin up) the CD.
  virtual bx_bool start_cdrom();

  // Seek for new block address.
  virtual bx_bool seek(Bit32u lba);

protected:
  int fd;
  char *path;
  bx_bool using_file;
};
