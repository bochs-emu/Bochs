/////////////////////////////////////////////////////////////////////////
// $Id: ferr.cc,v 1.15 2009-02-08 17:29:34 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_FPU

#include "softfloat-specialize.h"

void BX_CPU_C::FPU_stack_overflow(void)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(floatx80_default_nan, 0);
  }
  FPU_exception(FPU_EX_Stack_Overflow);
}

void BX_CPU_C::FPU_stack_underflow(int stnr, int pop_stack)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_WRITE_FPU_REG(floatx80_default_nan, stnr);
    if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  FPU_exception(FPU_EX_Stack_Underflow);
}

/* Returns unmasked exceptions if occured */
unsigned BX_CPU_C::FPU_exception(unsigned exception, bx_bool is_mem)
{
  /* Extract only the bits which we use to set the status word */
  exception &= (FPU_SW_Exceptions_Mask);

  /* Set the corresponding exception bits */
  FPU_PARTIAL_STATUS |= exception;

  unsigned unmasked = FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask;

  /* Set summary bits iff exception isn't masked */
  if (unmasked)
    FPU_PARTIAL_STATUS |= (FPU_SW_Summary | FPU_SW_Backward);

  if (exception & (FPU_SW_Stack_Fault | FPU_EX_Precision))
  {
    if (! (exception & FPU_SW_C1)) {
      /* This bit distinguishes over- from underflow for a stack fault,
           and roundup from round-down for precision loss. */
      FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
    }
  }

  // If #P unmasked exception occured the result still has to be
  // written to the destination.
  unmasked &= ~FPU_CW_Precision;

  // If unmasked overflow or underflow occurs and destination is a memory 
  // location, the TOS and destination operands remain unchanged and no 
  // result is stored in the memory. If the destination is in the register
  // stack, adjusted resulting value is stored in the destination operand.
  if (! is_mem)
    unmasked &= ~(FPU_CW_Underflow | FPU_CW_Overflow);

  return unmasked;
}

#endif
