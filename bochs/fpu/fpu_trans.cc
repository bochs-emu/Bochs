/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2009 Stanislav Shwartsman
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
#include "softfloat-specialize.h"

extern float_status_t FPU_pre_exception_handling(Bit16u control_word);

/* D9 F0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::F2XM1(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = f2xm1(BX_READ_FPU_REG(0), status);

  if (! FPU_exception(status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

/* D9 F1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FYL2X(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(1, 1 /* pop_stack */);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fyl2x(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (! FPU_exception(status.float_exception_flags)) {
     BX_CPU_THIS_PTR the_i387.FPU_pop();
     BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F2 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FPTAN(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0) || ! IS_TAG_EMPTY(-1))
  {
     if(IS_TAG_EMPTY(0))
       FPU_exception(FPU_EX_Stack_Underflow);
     else
       FPU_exception(FPU_EX_Stack_Overflow);

     /* The masked response */
     if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
     {
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
         BX_CPU_THIS_PTR the_i387.FPU_push();
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
     }

     BX_NEXT_INSTR(i);
  }

  extern const floatx80 Const_1;

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (ftan(y, status) == -1)
  {
     FPU_PARTIAL_STATUS |= FPU_SW_C2;
     BX_NEXT_INSTR(i);
  }

  if (floatx80_is_nan(y))
  {
     if (! FPU_exception(status.float_exception_flags))
     {
         BX_WRITE_FPU_REG(y, 0);
         BX_CPU_THIS_PTR the_i387.FPU_push();
         BX_WRITE_FPU_REG(y, 0);
     }

     BX_NEXT_INSTR(i);
  }

  if (! FPU_exception(status.float_exception_flags)) {
     BX_WRITE_FPU_REG(y, 0);
     BX_CPU_THIS_PTR the_i387.FPU_push();
     BX_WRITE_FPU_REG(Const_1, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F3 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FPATAN(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(1, 1 /* pop_stack */);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fpatan(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (! FPU_exception(status.float_exception_flags)) {
     BX_CPU_THIS_PTR the_i387.FPU_pop();
     BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FXTRACT(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || ! IS_TAG_EMPTY(-1))
  {
     if(IS_TAG_EMPTY(0))
       FPU_exception(FPU_EX_Stack_Underflow);
     else
       FPU_exception(FPU_EX_Stack_Overflow);

     /* The masked response */
     if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
     {
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
         BX_CPU_THIS_PTR the_i387.FPU_push();
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
     }

     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = floatx80_extract(a, status);

  if (! FPU_exception(status.float_exception_flags)) {
     BX_WRITE_FPU_REG(b, 0);     // exponent
     BX_CPU_THIS_PTR the_i387.FPU_push();
     BX_WRITE_FPU_REG(a, 0);     // fraction
  }

  BX_NEXT_INSTR(i);
}

/* D9 F5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FPREM1(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);
  floatx80 result;

  int flags = floatx80_ieee754_remainder(a, b, result, quotient, status);

  if (! FPU_exception(status.float_exception_flags)) {
     if (flags >= 0) {
        int cc = 0;
        if (flags) cc = FPU_SW_C2;
        else {
           if (quotient & 1) cc |= FPU_SW_C1;
           if (quotient & 2) cc |= FPU_SW_C3;
           if (quotient & 4) cc |= FPU_SW_C0;
        }
        setcc(cc);
     }
     BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FPREM(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);
  floatx80 result;

  int flags = floatx80_remainder(a, b, result, quotient, status);

  if (! FPU_exception(status.float_exception_flags)) {
     if (flags >= 0) {
        int cc = 0;
        if (flags) cc = FPU_SW_C2;
        else {
           if (quotient & 1) cc |= FPU_SW_C1;
           if (quotient & 2) cc |= FPU_SW_C3;
           if (quotient & 4) cc |= FPU_SW_C0;
        }
        setcc(cc);
     }
     BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 F9 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FYL2XP1(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(1, 1 /* pop_stack */);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fyl2xp1(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (! FPU_exception(status.float_exception_flags)) {
     BX_CPU_THIS_PTR the_i387.FPU_pop();
     BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 FB */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FSINCOS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0) || ! IS_TAG_EMPTY(-1))
  {
     if(IS_TAG_EMPTY(0))
       FPU_exception(FPU_EX_Stack_Underflow);
     else
       FPU_exception(FPU_EX_Stack_Overflow);

     /* The masked response */
     if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
     {
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
         BX_CPU_THIS_PTR the_i387.FPU_push();
         BX_WRITE_FPU_REG(floatx80_default_nan, 0);
     }

     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  floatx80 sin_y, cos_y;
  if (fsincos(y, &sin_y, &cos_y, status) == -1)
  {
     FPU_PARTIAL_STATUS |= FPU_SW_C2;
     BX_NEXT_INSTR(i);
  }

  if (! FPU_exception(status.float_exception_flags)) {
     BX_WRITE_FPU_REG(sin_y, 0);
     BX_CPU_THIS_PTR the_i387.FPU_push();
     BX_WRITE_FPU_REG(cos_y, 0);
  }

  BX_NEXT_INSTR(i);
}

/* D9 FD */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FSCALE(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_scale(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (! FPU_exception(status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

/* D9 FE */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FSIN(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (fsin(y, status) == -1)
  {
     FPU_PARTIAL_STATUS |= FPU_SW_C2;
     BX_NEXT_INSTR(i);
  }

  if (! FPU_exception(status.float_exception_flags))
     BX_WRITE_FPU_REG(y, 0);

  BX_NEXT_INSTR(i);
}

/* D9 FF */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (fcos(y, status) == -1)
  {
     FPU_PARTIAL_STATUS |= FPU_SW_C2;
     BX_NEXT_INSTR(i);
  }

  if (! FPU_exception(status.float_exception_flags))
     BX_WRITE_FPU_REG(y, 0);

  BX_NEXT_INSTR(i);
}

#endif
