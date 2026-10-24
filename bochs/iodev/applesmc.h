/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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
//

// Emulation of the Apple SMC (System Management Controller) found in
// Intel-based Macs. Real macOS guests probe this device early in boot and
// hang/panic if it doesn't respond. Port protocol and key table ported from
// QEMU's isa-applesmc device (hw/misc/applesmc.c, Alexander Graf, LGPLv2.1+),
// which is the de-facto reference implementation used by the hackintosh
// community for exactly this purpose.

#ifndef BX_IODEV_APPLESMC_H
#define BX_IODEV_APPLESMC_H

#if BX_USE_APPLESMC_SMF
#  define BX_APPLESMC_SMF  static
#  define BX_APPLESMC_THIS theAppleSMC->
#else
#  define BX_APPLESMC_SMF
#  define BX_APPLESMC_THIS this->
#endif

#define BX_APPLESMC_MAX_DATA_LENGTH 32
#define BX_APPLESMC_OSK_LENGTH      64

struct bx_applesmc_key_t {
  char key[4];
  Bit8u len;
  const Bit8u *data;
};

class bx_applesmc_c : public bx_applesmc_stub_c {
public:
  bx_applesmc_c();
  virtual ~bx_applesmc_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  void set_osk(const char *osk) { memcpy(s.osk, osk, BX_APPLESMC_OSK_LENGTH); }

private:
  struct {
    Bit8u  cmd;
    Bit8u  status;
    Bit8u  status_1e;
    Bit8u  last_ret;
    char   key[4];
    Bit8u  read_pos;
    Bit8u  data_len;
    Bit8u  data_pos;
    Bit8u  data[255];
    char   osk[BX_APPLESMC_OSK_LENGTH];
  } s;

  static const bx_applesmc_key_t static_keys[];

  const bx_applesmc_key_t* find_key(const char key[4]);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_APPLESMC_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
