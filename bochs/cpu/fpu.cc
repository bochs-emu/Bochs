/////////////////////////////////////////////////////////////////////////
// $Id: fpu.cc,v 1.1 2003-04-22 20:21:32 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
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


#include "bochs.h"

#define LOG_THIS genlog->

// Nomenclature used to signify argument types
//
// Es = single real
// El = double real
// Ea = 14/28 bytes    98/108b bytes (FRSTOR,FSAVE)???
// Ew = word integer (2 bytes)
// Ed = dword integer (4 bytes) (short int)
// Et = extended real
// Eb = packed BCD
// Eq = quadword integer (8 bytes) (long integer)



void BX_CPU_C::ESC0(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC0 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC1(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC1 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC2(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC2 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC3(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }

#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC3 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC4(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC4 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC5(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC5 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC6(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC6 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::ESC7(bxInstruction_c *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FPU: ESC7 not implemented, use --enable-fpu"));
#endif
}

void BX_CPU_C::FWAIT(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  // WAIT doesn't generate single steps on 8086.
  // The same goes for prefix instructions, and instructions which
  // modify segment registers. (pg4-16)
  BX_PANIC(("WAIT: not implemented for < 386"));
#else // BX_CPU_LEVEL >= 3

  if ( BX_CPU_THIS_PTR cr0.ts && BX_CPU_THIS_PTR cr0.mp ) {
    exception(BX_NM_EXCEPTION, 0, 0); 
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FWAIT: requred FPU, use --enable-fpu"));
#endif

#endif
}
