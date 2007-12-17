/////////////////////////////////////////////////////////////////////////
// $Id: io_pro.cc,v 1.26 2007-12-17 21:13:55 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

  Bit16u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp16(Bit16u addr)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 2)) {
    BX_DEBUG(("inp16(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit16u ret16 = BX_INP(addr, 2);
  return ret16;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp16(Bit16u addr, Bit16u value)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 2)) {
    BX_DEBUG(("outp16(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(addr, value, 2);
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp32(Bit16u addr)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 4)) {
    BX_DEBUG(("inp32(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit32u ret32 = BX_INP(addr, 4);
  return ret32;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp32(Bit16u addr, Bit32u value)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 4)) {
    BX_DEBUG(("outp32(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(addr, value, 4);
}

  Bit8u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp8(Bit16u addr)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 1)) {
    BX_DEBUG(("inp8(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit8u ret8 = BX_INP(addr, 1);
  return ret8;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp8(Bit16u addr, Bit8u value)
{
  if (! BX_CPU_THIS_PTR allow_io(addr, 1)) {
    BX_DEBUG(("outp8(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(addr, value, 1);
}

bx_bool BX_CPU_C::allow_io(Bit16u addr, unsigned len)
{
  Bit16u io_base, permission16;

  /* If CPL <= IOPL, then all IO addresses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */

  if (BX_CPU_THIS_PTR cr0.get_PE() && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL())))
  {
    if (BX_CPU_THIS_PTR tr.cache.valid==0 || 
        BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_386_TSS)
    {
      BX_ERROR(("allow_io(): TR doesn't point to a valid 32bit TSS"));
      return(0);
    }

    if (BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled < 103) {
      BX_ERROR(("allow_io(): TR.limit < 103"));
      return(0);
    }

    access_linear(BX_CPU_THIS_PTR tr.cache.u.system.base + 102, 
                   2, 0, BX_READ, &io_base);

    if (io_base <= 103) {
      BX_ERROR(("allow_io(): TR:io_base (%u) <= 103", io_base));
      return(0);
    }

    if ( (Bit32s) (addr/8) >= (Bit32s) (BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled - io_base)) {
      BX_ERROR(("allow_io(): IO addr %x (len %d) outside TSS IO permission map (base=%x, limit=%x) #GP(0)",
        addr, len, io_base, BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled));
      return(0);
    }

    access_linear(BX_CPU_THIS_PTR tr.cache.u.system.base + io_base + addr/8,
                   2, 0, BX_READ, &permission16);

    unsigned bit_index = addr & 0x07;
    permission16 >>= bit_index;
    for (unsigned i=0; i<len; i++) {
      if (permission16 & 0x01)
        return(0);
      permission16 >>= 1;
    }
  }

  return(1);
}
