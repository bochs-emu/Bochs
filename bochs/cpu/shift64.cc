/////////////////////////////////////////////////////////////////////////
// $Id: shift64.cc,v 1.6 2002-09-22 18:22:24 kevinlawton Exp $
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






#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR


  void
BX_CPU_C::SHLD_EqGq(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, result_64;
  unsigned count;

  /* op1:op2 << count.  result stored in op1 */

  if (i->b1() == 0x1a4)
    count = i->Ib() & 0x3f;
  else // 0x1a5
    count = CL & 0x3f;

    if (!count) return; /* NOP */

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    result_64 = (op1_64 << count) | (op2_64 >> (64 - count));

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    /* set eflags:
     * SHLD count affects the following flags: S,Z,P,C,O
     */
    set_CF((op1_64 >> (64 - count)) & 0x01);
    if (count == 1)
      set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
    set_ZF(result_64 == 0);
    set_PF_base(result_64);
    set_SF(result_64 >> 63);
}


  void
BX_CPU_C::SHRD_EqGq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("shrd_evgvib: not supported on < 386"));
#else
  Bit64u op1_64, op2_64, result_64;
  unsigned count;

  if (i->b1() == 0x1ac)
    count = i->Ib() & 0x3f;
  else // 0x1ad
    count = CL & 0x3f;

  if (!count) return; /* NOP */


    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    result_64 = (op2_64 << (64 - count)) | (op1_64 >> count);

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    /* set eflags:
     * SHRD count affects the following flags: S,Z,P,C,O
     */

    set_CF((op1_64 >> (count - 1)) & 0x01);
    set_ZF(result_64 == 0);
    set_SF(result_64 >> 63);
    /* for shift of 1, OF set if sign change occurred. */
    if (count == 1)
      set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
    set_PF_base(result_64);
#endif /* BX_CPU_LEVEL >= 3 */
}



  void
BX_CPU_C::ROL_Eq(bxInstruction_c *i)
{

  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (count) {
      result_64 = (op1_64 << count) | (op1_64 >> (64 - count));

      /* now write result back to destination */
      if (i->modC0()) {
        BX_WRITE_64BIT_REG(i->rm(), result_64);
        }
      else {
        Write_RMW_virtual_qword(result_64);
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      set_CF(result_64 & 0x01);
      if (count == 1)
        set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
      }
}




  void
BX_CPU_C::ROR_Eq(bxInstruction_c *i)
{
    Bit64u op1_64, result_64, result_b63;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (count) {
      result_64 = (op1_64 >> count) | (op1_64 << (64 - count));

      /* now write result back to destination */
      if (i->modC0()) {
        BX_WRITE_64BIT_REG(i->rm(), result_64);
        }
      else {
        Write_RMW_virtual_qword(result_64);
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b63 = result_64 & BX_CONST64(0x8000000000000000);

      set_CF(result_b63 != 0);
      if (count == 1)
        set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
      }
}



  void
BX_CPU_C::RCL_Eq(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;


    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (!count) return;

    if (count==1) {
      result_64 = (op1_64 << 1) | getB_CF();
      }
    else {
      result_64 = (op1_64 << count) |
                (getB_CF() << (count - 1)) |
                (op1_64 >> (65 - count));
      }

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    /* set eflags:
     * RCL count affects the following flags: C
     */
    if (count == 1)
      set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
    set_CF((op1_64 >> (64 - count)) & 0x01);
}



  void
BX_CPU_C::RCR_Eq(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;


    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (!count) return;

    if (count==1) {
      result_64 = (op1_64 >> 1) | (((Bit64u) getB_CF()) << 63);
      }
    else {
      result_64 = (op1_64 >> count) |
                (getB_CF() << (64 - count)) |
                (op1_64 << (65 - count));
      }

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    /* set eflags:
     * RCR count affects the following flags: C
     */

    set_CF((op1_64 >> (count - 1)) & 0x01);
    if (count == 1)
      set_OF(((op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0);
}




  void
BX_CPU_C::SHL_Eq(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (!count) return;

    result_64 = (op1_64 << count);

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, count, result_64, BX_INSTR_SHL64);
}




  void
BX_CPU_C::SHR_Eq(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (!count) return;

    result_64 = (op1_64 >> count);

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, count, result_64, BX_INSTR_SHR64);
}



  void
BX_CPU_C::SAR_Eq(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x3f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x3f;

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    if (!count) return;

    /* count < 64, since only lower 5 bits used */
    if (op1_64 & BX_CONST64(0x8000000000000000)) {
      result_64 = (op1_64 >> count) | (BX_CONST64(0xffffffffffffffff) << (64 - count));
      }               
    else {
      result_64 = (op1_64 >> count);
      }

    /* now write result back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), result_64);
      }
    else {
      Write_RMW_virtual_qword(result_64);
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */

    set_CF((op1_64 >> (count - 1)) & 0x01);
    set_ZF(result_64 == 0);
    set_SF(result_64 >> 63);
    if (count == 1)
      set_OF(0);
    set_PF_base(result_64);
}
