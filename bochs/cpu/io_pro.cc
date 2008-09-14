/////////////////////////////////////////////////////////////////////////
// $Id: io_pro.cc,v 1.39 2008-09-14 20:58:20 sshwarts Exp $
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

  Bit16u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp16(Bit16u port)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 2)) {
    BX_DEBUG(("inp16(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit16u ret16 = BX_INP(port, 2);
  return ret16;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp16(Bit16u port, Bit16u value)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 2)) {
    BX_DEBUG(("outp16(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, value, 2);
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp32(Bit16u port)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 4)) {
    BX_DEBUG(("inp32(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit32u ret32 = BX_INP(port, 4);
  return ret32;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp32(Bit16u port, Bit32u value)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 4)) {
    BX_DEBUG(("outp32(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, value, 4);
}

  Bit8u BX_CPP_AttrRegparmN(1)
BX_CPU_C::inp8(Bit16u port)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 1)) {
    BX_DEBUG(("inp8(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit8u ret8 = BX_INP(port, 1);
  return ret8;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::outp8(Bit16u port, Bit8u value)
{
  if (! BX_CPU_THIS_PTR allow_io(port, 1)) {
    BX_DEBUG(("outp8(): I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, value, 1);
}

bx_bool BX_CPU_C::allow_io(Bit16u port, unsigned len)
{
  /* If CPL <= IOPL, then all IO portesses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */

  if (BX_CPU_THIS_PTR cr0.get_PE() && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL())))
  {
    if (BX_CPU_THIS_PTR tr.cache.valid==0 ||
       (BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_386_TSS &&
        BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_386_TSS))
    {
      BX_ERROR(("allow_io(): TR doesn't point to a valid 32bit TSS, TR.TYPE=%u", BX_CPU_THIS_PTR tr.cache.type));
      return(0);
    }

    if (BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled < 103) {
      BX_ERROR(("allow_io(): TR.limit < 103"));
      return(0);
    }

    Bit16u io_base = system_read_word(BX_CPU_THIS_PTR tr.cache.u.system.base + 102);

    if ((io_base + port/8) >= BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled) {
      BX_DEBUG(("allow_io(): IO port %x (len %d) outside TSS IO permission map (base=%x, limit=%x) #GP(0)",
        port, len, io_base, BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled));
      return(0);
    }

    Bit16u permission16 = system_read_word(BX_CPU_THIS_PTR tr.cache.u.system.base + io_base + port/8);

    unsigned bit_index = port & 0x7;
    unsigned mask = (1 << len) - 1;
    if ((permission16 >> bit_index) & mask)
      return(0);
  }

#if BX_X86_DEBUGGER
  iobreakpoint_match(port, len);
#endif

  return(1);
}
