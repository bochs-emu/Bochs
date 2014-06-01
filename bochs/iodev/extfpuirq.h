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
//

#ifndef BX_IODEV_EXTFPUIRQ_H
#define BX_IODEV_EXTFPUIRQ_H


#if BX_USE_EFI_SMF
#  define BX_EXTFPUIRQ_SMF  static
#  define BX_EXTFPUIRQ_THIS theExternalFpuIrq->
#else
#  define BX_EXTFPUIRQ_SMF
#  define BX_EXTFPUIRQ_THIS this->
#endif


class bx_extfpuirq_c : public bx_devmodel_c {
public:
  bx_extfpuirq_c();
  virtual ~bx_extfpuirq_c();
  virtual void   init(void);
  virtual void   reset(unsigned type);

private:

  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_EFI_SMF
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
