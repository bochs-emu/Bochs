/////////////////////////////////////////////////////////////////////////
// $Id: extfpuirq.h,v 1.2 2003-01-07 08:17:15 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#if BX_USE_EFI_SMF
#  define BX_EXTFPUIRQ_SMF  static
#  define BX_EXTFPUIRQ_THIS theExternalFpuIrq->
#else
#  define BX_EXTFPUIRQ_SMF
#  define BX_EXTFPUIRQ_THIS this->
#endif


class bx_extfpuirq_c : public bx_devmodel_c {

public:
  bx_extfpuirq_c(void);
  ~bx_extfpuirq_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);

private:

  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_EFI_SMF
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  };
