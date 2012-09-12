/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2012 Stanislav Shwartsman
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

#include "softfloatx80.h"

/* D9 C8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FXCH_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);
  int sti_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(i->src());

  floatx80 st0_reg = BX_READ_FPU_REG(0);
  floatx80 sti_reg = BX_READ_FPU_REG(i->src());

  clear_C1();

  if (st0_tag == FPU_Tag_Empty || sti_tag == FPU_Tag_Empty)
  {
     FPU_exception(FPU_EX_Stack_Underflow);

     if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
     {
         /* Masked response */
         if (st0_tag == FPU_Tag_Empty)
             st0_reg = floatx80_default_nan;

         if (sti_tag == FPU_Tag_Empty)
             sti_reg = floatx80_default_nan;
     }
     else {
         BX_NEXT_INSTR(i);
     }
  }

  BX_WRITE_FPU_REG(st0_reg, i->src());
  BX_WRITE_FPU_REG(sti_reg, 0);

  BX_NEXT_INSTR(i);
}

/* D9 E0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCHS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(0);
  }
  else {
     clear_C1();
     floatx80 st0_reg = BX_READ_FPU_REG(0);
     BX_WRITE_FPU_REG(floatx80_chs(st0_reg), 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 E1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FABS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(0);
  }
  else {
     clear_C1();
     floatx80 st0_reg = BX_READ_FPU_REG(0);
     BX_WRITE_FPU_REG(floatx80_abs(st0_reg), 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F6 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FDECSTP(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.tos = (BX_CPU_THIS_PTR the_i387.tos-1) & 7;

  BX_NEXT_INSTR(i);
}

/* D9 F7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FINCSTP(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.tos = (BX_CPU_THIS_PTR the_i387.tos+1) & 7;

  BX_NEXT_INSTR(i);
}

/* DD C0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FFREE_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.FPU_settagi(FPU_Tag_Empty, i->dst());

  BX_NEXT_INSTR(i);
}

/*
 * Free the st(0) register and pop it from the FPU stack.
 * "Undocumented" by Intel & AMD but mentioned in AMDs Athlon Docs.
 */

/* DF C0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FFREEP_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.FPU_settagi(FPU_Tag_Empty, i->dst());
  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

#endif
