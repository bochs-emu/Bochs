/////////////////////////////////////////////////////////////////////////
// $Id: fpu_trans.cc,v 1.10 2006-03-06 22:03:04 sshwarts Exp $
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
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "softfloatx80.h"
#include "softfloat-specialize.h"

extern float_status_t FPU_pre_exception_handling(Bit16u control_word);

/* D9 F0 */
void BX_CPU_C::F2XM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = f2xm1(BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("F2XM1: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F1 */
void BX_CPU_C::FYL2X(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0, 1);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fyl2x(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FYL2X: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F2 */
void BX_CPU_C::FPTAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();
  clear_C2();

  if (! IS_TAG_EMPTY(-1) || IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Overflow);

      /* The masked response */
      if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
          BX_CPU_THIS_PTR the_i387.FPU_push();
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
      }

      return; 
  }

  extern const floatx80 Const_1;

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (ftan(y, status) == -1)
  {
      FPU_PARTIAL_STATUS |= FPU_SW_C2;
      return;
  }

  if (floatx80_is_nan(y))
  {
      if (! BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      {
          BX_WRITE_FPU_REGISTER_AND_TAG(y, FPU_Tag_Special, 0);
          BX_CPU_THIS_PTR the_i387.FPU_push();
          BX_WRITE_FPU_REGISTER_AND_TAG(y, FPU_Tag_Special, 0);
      }

      return;
  }

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(y, 0);
  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REGISTER_AND_TAG(Const_1, FPU_Tag_Valid, 0);
#else
  BX_INFO(("FPTAN: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F3 */
void BX_CPU_C::FPATAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0, 1);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fpatan(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FPATAN: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F4 */
void BX_CPU_C::FXTRACT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1) || IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Overflow);

      /* The masked response */
      if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
          BX_CPU_THIS_PTR the_i387.FPU_push();
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
      }

      return; 
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = floatx80_extract(a, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(b, 0);	// exponent
  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(a, 0);	// fraction
#else
  BX_INFO(("FXTRACT: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F5 */
void BX_CPU_C::FPREM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);

  floatx80 result = floatx80_ieee754_remainder(a, b, quotient, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  int cc = 0;
  if (quotient == (Bit64u) -1) cc = FPU_SW_C2;
  else
  {
      if (quotient & 1) cc |= FPU_SW_C1;
      if (quotient & 2) cc |= FPU_SW_C3;
      if (quotient & 4) cc |= FPU_SW_C0;
  }
  setcc(cc);

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FPREM1: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F8 */
void BX_CPU_C::FPREM(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);

  floatx80 result = floatx80_remainder(a, b, quotient, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  int cc = 0;
  if (quotient == (Bit64u) -1) cc = FPU_SW_C2;
  else
  {
      if (quotient & 1) cc |= FPU_SW_C1;
      if (quotient & 2) cc |= FPU_SW_C3;
      if (quotient & 4) cc |= FPU_SW_C0;
  }
  setcc(cc);

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FPREM: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F9 */
void BX_CPU_C::FYL2XP1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0, 1);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 result = fyl2xp1(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FYL2XP1: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FB */
void BX_CPU_C::FSINCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();
  clear_C2();

  if (! IS_TAG_EMPTY(-1) || IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Overflow);

      /* The masked response */
      if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
          BX_CPU_THIS_PTR the_i387.FPU_push();
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
      }

      return; 
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  floatx80 sin_y, cos_y;
  if (fsincos(y, &sin_y, &cos_y, status) == -1) 
  {
      FPU_PARTIAL_STATUS |= FPU_SW_C2;
      return;
  }

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(sin_y, 0);
  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(cos_y, 0);
#else
  BX_INFO(("FSINCOS: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FD */
void BX_CPU_C::FSCALE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_scale(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSCALE: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FE */
void BX_CPU_C::FSIN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (fsin(y, status) == -1)
  {
      FPU_PARTIAL_STATUS |= FPU_SW_C2;
      return;
  }

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(y, 0);
#else
  BX_INFO(("FSIN: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FF */
void BX_CPU_C::FCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();
  clear_C2();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float_status_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word() | FPU_PR_80_BITS);

  floatx80 y = BX_READ_FPU_REG(0);
  if (fcos(y, status) == -1)
  {
      FPU_PARTIAL_STATUS |= FPU_SW_C2;
      return;
  }

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(y, 0);
#else
  BX_INFO(("FCOS: required FPU, configure --enable-fpu"));
#endif
}
