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

class cdrom_win32_c : public cdrom_base_c {
public:
  cdrom_win32_c(const char *dev);
  virtual ~cdrom_win32_c(void);
  bx_bool insert_cdrom(const char *dev = NULL);
  void eject_cdrom();
  bx_bool read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format);
  Bit32u capacity();
  bx_bool read_block(Bit8u* buf, Bit32u lba, int blocksize) BX_CPP_AttrRegparmN(3);
private:
#ifdef WIN32
  HANDLE hFile;
#endif
};
