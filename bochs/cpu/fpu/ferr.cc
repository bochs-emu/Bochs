/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2017 Stanislav Shwartsman
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

void BX_CPU_C::FPU_stack_overflow(bxInstruction_c *i)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(floatx80_default_nan, 0);
  }
  FPU_exception(i, FPU_EX_Stack_Overflow);
}

void BX_CPU_C::FPU_stack_underflow(bxInstruction_c *i, int stnr, int pop_stack)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_WRITE_FPU_REG(floatx80_default_nan, stnr);
    if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  FPU_exception(i, FPU_EX_Stack_Underflow);
}

/* Returns unmasked exceptions if occurred */
unsigned BX_CPU_C::FPU_exception(bxInstruction_c *i, unsigned exception, bx_bool is_store)
{
  /* Extract only the bits which we use to set the status word */
  exception &= (FPU_SW_Exceptions_Mask);

  Bit32u status = FPU_PARTIAL_STATUS;

  unsigned unmasked = exception & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask;
  // if IE or DZ exception happen nothing else will be reported
  if (exception & (FPU_EX_Invalid | FPU_EX_Zero_Div))
      unmasked &= (FPU_EX_Invalid | FPU_EX_Zero_Div);

  /* Set summary bits iff exception isn't masked */
  if (unmasked) {
    FPU_PARTIAL_STATUS |= (FPU_SW_Summary | FPU_SW_Backward);

    // when FOPCODE deprecation is set, FOPCODE is updated only when unmasked x87 exception occurs
    if (is_cpu_extension_supported(BX_ISA_FOPCODE_DEPRECATION))
      BX_CPU_THIS_PTR the_i387.foo = i->foo();

    // when FOPCODE deprecation is set, FCS/FDP are updated only when unmasked x87 exception occurs
    if (is_cpu_extension_supported(BX_ISA_FDP_DEPRECATION)) {
      if (! i->modC0()) {
        BX_CPU_THIS_PTR the_i387.fds = BX_CPU_THIS_PTR sregs[i->seg()].selector.value;
        BX_CPU_THIS_PTR the_i387.fdp = RMAddr(i);
      }
    }
  }

  if (exception & FPU_EX_Invalid) {
     // FPU_EX_Invalid cannot come with any other exception but x87 stack fault
     FPU_PARTIAL_STATUS |= exception;
     if (exception & FPU_SW_Stack_Fault) {
        if (! (exception & FPU_SW_C1)) {
           /* This bit distinguishes over- from underflow for a stack fault,
              and roundup from round-down for precision loss. */
           FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
        }
     }
     return unmasked;
  }

  if (exception & FPU_EX_Zero_Div) {
     FPU_PARTIAL_STATUS |= FPU_EX_Zero_Div;
     return unmasked;
  }

  if (exception & FPU_EX_Denormal) {
     FPU_PARTIAL_STATUS |= FPU_EX_Denormal;
     if (unmasked & FPU_EX_Denormal)
        return unmasked & FPU_EX_Denormal;
  }

  /* Set the corresponding exception bits */
  FPU_PARTIAL_STATUS |= exception;

  if (exception & FPU_EX_Precision)
  {
    if (! (exception & FPU_SW_C1)) {
      /* This bit distinguishes over- from underflow for a stack fault,
           and roundup from round-down for precision loss. */
      FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
    }
  }

  // If #P unmasked exception occurred the result still has to be
  // written to the destination.
  unmasked &= ~FPU_EX_Precision;

  if (unmasked & (FPU_EX_Underflow | FPU_EX_Overflow)) {
    // If unmasked over- or underflow occurs and dest is a memory location:
    //   - the TOS and destination operands remain unchanged
    //   - the inexact-result condition is not reported and C1 flag is cleared
    //   - no result is stored in the memory
    // If the destination is in the register stack, adjusted resulting value
    // is stored in the destination operand.
    if (! is_store) {
       unmasked &= ~(FPU_EX_Underflow | FPU_EX_Overflow);
    }
    else {
       FPU_PARTIAL_STATUS &= ~FPU_SW_C1; // clear C1 flag
       if (! (status & FPU_EX_Precision))
          FPU_PARTIAL_STATUS &= ~FPU_EX_Precision;
    }
  }

  return unmasked;
}

#endif
