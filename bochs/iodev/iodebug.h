/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

#ifndef _BX_IODEBUG_H
#define _BX_IODEBUG_H

#if BX_SUPPORT_IODEBUG

#define BX_IODEBUG_MAX_AREAS 30

class bx_iodebug_c : public bx_iodebug_stub_c {
public:
  bx_iodebug_c();
  virtual ~bx_iodebug_c() {}
  virtual void init(void);
  virtual void reset (unsigned type) {}
  virtual void mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  virtual void mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);

private:
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned io_len);
  static unsigned range_test(bx_phy_address addr, unsigned len);
  static void add_range(bx_phy_address addr_start, bx_phy_address addr_end);
};

#endif

#endif
