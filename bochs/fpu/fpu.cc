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



  void
BX_CPU_C::ESC0(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC0 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC1(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC1 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC2(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC2 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC3(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }

//fprintf(stderr, "CS:EIP = %04x:%08x\n",
//  BX_CPU.sregs[BX_SEG_REG_CS].selector.value, BX_CPU.prev_eip);

#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC3 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC4(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC4 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC5(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC5 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC6(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC6 not implemented\n"));
#endif
}

  void
BX_CPU_C::ESC7(BxInstruction_t *i)
{
  if ( BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts ) {
    exception(BX_NM_EXCEPTION, 0, 0);
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("ESC7 not implemented\n"));
#endif
}

  void
BX_CPU_C::FWAIT(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  // WAIT doesn't generate single steps on 8086.
  // The same goes for prefix instructions, and instructions which
  // modify segment registers. (pg4-16)
  // BX_CPU_THIS_PTR single_step_event = 0;
  genlog->panic("WAIT: not implemented for < 386\n");
#else // BX_CPU_LEVEL >= 3

  if ( BX_CPU_THIS_PTR cr0.ts && BX_CPU_THIS_PTR cr0.mp ) {
    exception(BX_NM_EXCEPTION, 0, 0); // no error
    }
#if BX_SUPPORT_FPU
  fpu_execute(i);
#else
  BX_INFO(("FWAIT: no FPU\n"));
#endif

#endif
}


#if BX_SUPPORT_FPU==0
  // if supporting FPU, this function in glue logic file
  void
BX_CPU_C::fpu_init(void)
{
}
#endif
