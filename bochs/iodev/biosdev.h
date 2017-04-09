////////////////////////////////////////////////////////////////////////
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


#ifndef BX_IODEV_BIOSDEV_H
#define BX_IODEV_BIOSDEV_H

#define BX_BIOS_MESSAGE_SIZE 80

#if BX_USE_BIOS_SMF
#  define BX_BIOS_SMF  static
#  define BX_BIOS_THIS theBiosDevice->
#else
#  define BX_BIOS_SMF
#  define BX_BIOS_THIS this->
#endif

class bx_biosdev_c : public bx_devmodel_c {
public:
  bx_biosdev_c();
  virtual ~bx_biosdev_c();

  virtual void init(void);
  virtual void reset(unsigned type) {}

private:

  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_BIOS_SMF
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  struct {
    Bit8u bios_message[BX_BIOS_MESSAGE_SIZE];
    unsigned int bios_message_i;
    bx_bool bios_panic_flag;

    Bit8u vgabios_message[BX_BIOS_MESSAGE_SIZE];
    unsigned int vgabios_message_i;
    bx_bool vgabios_panic_flag;
  } s;  // state information
};

#endif
