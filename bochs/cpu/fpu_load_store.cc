/////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2004  MandrakeSoft S.A.
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_FPU
#include "softfloat-fpu.h"
#endif

void BX_CPU_C::FLD_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  if (IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  int sti_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(i->rm());
  floatx80 sti_reg = BX_READ_FPU_REG(i->rm());

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);
#else
  BX_INFO(("FLD_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  // convert to floatx80 format
  softfloat_status_word_t status = 
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = float32_to_floatx80(load_reg, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
     return;

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FLD_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  // convert to floatx80 format
  softfloat_status_word_t status = 
     FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = float64_to_floatx80(load_reg, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
     return;

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FLD_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_EXTENDED_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  floatx80 result;
  read_virtual_tword(i->seg(), RMAddr(i), &result);

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FLD_EXTENDED_REAL: required FPU, configure --enable-fpu"));
#endif
}

/* DF /0 */
void BX_CPU_C::FILD_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));
  floatx80 result = int32_to_floatx80((Bit32s) load_reg);

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FILD_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

/* DB /0 */
void BX_CPU_C::FILD_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));
  floatx80 result = int32_to_floatx80(load_reg);

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FILD_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

/* DF /5 */
void BX_CPU_C::FILD_QWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
      BX_CPU_THIS_PTR FPU_stack_overflow();
      return; 
  }

  Bit64s load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), (Bit64u*)(&load_reg));
  floatx80 result = int64_to_floatx80(load_reg);

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FILD_QWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

/* DF /4 */
void BX_CPU_C::FBLD_PACKED_BCD(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
//#else
  BX_INFO(("FBLD_PACKED_BCD: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);

  if (st0_tag == FPU_Tag_Empty)
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  floatx80 st0_reg = BX_READ_FPU_REG(0);

  BX_WRITE_FPU_REGISTER_AND_TAG(st0_reg, st0_tag, i->rm());

  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FST(P)_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  float32 save_reg = float32_default_nan; /* The masked response */

  int pop_stack = (i->b1() & 0x10) >> 1;

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_float32(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_dword(i->seg(), RMAddr(i), &save_reg);

  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FST(P)_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  float64 save_reg = float64_default_nan; /* The masked response */

  int pop_stack = (i->b1() & 0x10) >> 1;

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_float64(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_qword(i->seg(), RMAddr(i), &save_reg);

  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FST(P)_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

/* DB /7 */
void BX_CPU_C::FSTP_EXTENDED_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  floatx80 save_reg = Const_QNaN; /* The masked response */

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     save_reg = BX_READ_FPU_REG(0);
  }

  write_virtual_tword(i->seg(), RMAddr(i), &save_reg);

  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FSTP_EXTENDED_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIST_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit16s save_reg = int16_indefinite;

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int16(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&save_reg));

  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FIST(P)_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIST_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit32s save_reg = int32_indefinite; /* The masked response */

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int32(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&save_reg));

  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FIST(P)_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISTP_QWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit64s save_reg = int64_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int64(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_qword(i->seg(), RMAddr(i), (Bit64u*)(&save_reg));
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FISTP_QWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FBSTP_PACKED_BCD(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
//#else
  BX_INFO(("FBSTP_PACKED_BCD: required FPU, configure --enable-fpu"));
#endif
}

/* DF /1 */
void BX_CPU_C::FISTTP16(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit16s save_reg = int16_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int16_round_to_zero(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&save_reg));
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FISTTP16: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}

/* DB /1 */
void BX_CPU_C::FISTTP32(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit32s save_reg = int32_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int32_round_to_zero(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&save_reg));
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FISTTP32: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}

/* DD /1 */
void BX_CPU_C::FISTTP64(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareFPU(i);

  Bit64s save_reg = int64_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

     if (! (BX_CPU_THIS_PTR the_i387.is_IA_masked()))
        return;
  }
  else
  {
     softfloat_status_word_t status = 
        FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = floatx80_to_int64_round_to_zero(BX_READ_FPU_REG(0), status);

     if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
        return;
  }

  write_virtual_qword(i->seg(), RMAddr(i), (Bit64u*)(&save_reg));
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FISTTP64: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}
