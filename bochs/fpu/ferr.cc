/////////////////////////////////////////////////////////////////////////
// $Id: ferr.cc,v 1.6 2005-05-12 18:07:45 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <stl at fidonet.org.il>
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
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_FPU

#include "softfloat-specialize.h"

void BX_CPU_C::FPU_stack_overflow(void)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
      BX_CPU_THIS_PTR the_i387.FPU_push();
      BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
  }
  FPU_exception(FPU_EX_Stack_Overflow);
}

void BX_CPU_C::FPU_stack_underflow(int stnr, int pop_stack)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
     BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, stnr);
     if (pop_stack) 
          BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  FPU_exception(FPU_EX_Stack_Underflow);
}

/* Returns 1 if unmasked exception occured */
int BX_CPU_C::FPU_exception(int exception)
{
  int unmasked = 0;

  /* Extract only the bits which we use to set the status word */
  exception &= (FPU_SW_Exceptions_Mask);

  /* Set the corresponding exception bits */
  FPU_PARTIAL_STATUS |= exception;

  /* Set summary bits iff exception isn't masked */
  if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask)
  {
      FPU_PARTIAL_STATUS |= (FPU_SW_Summary | FPU_SW_Backward);
      unmasked = 1;
  }

  if (exception & (FPU_SW_Stack_Fault | FPU_EX_Precision))
  {
      if (! (exception & FPU_SW_C1))
        /* This bit distinguishes over- from underflow for a stack fault,
             and roundup from round-down for precision loss. */
        FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
  }

  return unmasked;
}

#endif
