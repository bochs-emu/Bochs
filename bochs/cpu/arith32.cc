/////////////////////////////////////////////////////////////////////////
// $Id: arith32.cc,v 1.25 2002-10-02 04:01:45 bdenney Exp $
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


#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu code.
#define RAX EAX
#define RDX EDX
#endif


  void
BX_CPU_C::INC_ERX(bxInstruction_c *i)
{
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "incl %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx)
    : "1" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPMask) | (flags32 & EFlagsOSZAPMask);
  BX_CPU_THIS_PTR lf_flags_status &= 0x00000f;
#else
  Bit32u erx;

  erx = ++ BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx;
#endif

#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.hrx = 0;
#endif

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAP_32(0, 0, erx, BX_INSTR_INC32);
#endif
}

  void
BX_CPU_C::DEC_ERX(bxInstruction_c *i)
{
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "decl %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx)
    : "1" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPMask) | (flags32 & EFlagsOSZAPMask);
  BX_CPU_THIS_PTR lf_flags_status &= 0x00000f;
#else
  Bit32u erx;

  erx = -- BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.erx;
#endif

#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].dword.hrx = 0;
#endif

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAP_32(0, 0, erx, BX_INSTR_DEC32);
#endif
}




  void
BX_CPU_C::ADD_EdGd(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32, sum_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    sum_32 = op1_32 + op2_32;
    BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    sum_32 = op1_32 + op2_32;
    Write_RMW_virtual_dword(sum_32);
    }

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
}


  void
BX_CPU_C::ADD_GdEEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "addl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  sum_32 = op1_32 + op2_32;
#endif

  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
#endif
}

  void
BX_CPU_C::ADD_GdEGd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "addl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  sum_32 = op1_32 + op2_32;
#endif

  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
#endif
}


  void
BX_CPU_C::ADD_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = EAX;
  op2_32 = i->Id();

  sum_32 = op1_32 + op2_32;

  RAX = sum_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
}

  void
BX_CPU_C::ADC_EdGd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op2_32, op1_32, sum_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    sum_32 = op1_32 + op2_32 + temp_CF;
    BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    sum_32 = op1_32 + op2_32 + temp_CF;
    Write_RMW_virtual_dword(sum_32);
    }

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                         temp_CF);
}


  void
BX_CPU_C::ADC_GdEd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
    }

  sum_32 = op1_32 + op2_32 + temp_CF;

  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                         temp_CF);
}


  void
BX_CPU_C::ADC_EAXId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = EAX;
  op2_32 = i->Id();

  sum_32 = op1_32 + op2_32 + temp_CF;

  RAX = sum_32;

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                         temp_CF);
}




  void
BX_CPU_C::SBB_EdGd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op2_32, op1_32, diff_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    diff_32 = op1_32 - (op2_32 + temp_CF);
    BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    diff_32 = op1_32 - (op2_32 + temp_CF);
    Write_RMW_virtual_dword(diff_32);
    }

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                         temp_CF);
}


  void
BX_CPU_C::SBB_GdEd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
    }

  diff_32 = op1_32 - (op2_32 + temp_CF);

  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                         temp_CF);
}


  void
BX_CPU_C::SBB_EAXId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;
  op2_32 = i->Id();

  diff_32 = op1_32 - (op2_32 + temp_CF);

  RAX = diff_32;

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                         temp_CF);
}



  void
BX_CPU_C::SBB_EdId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op2_32, op1_32, diff_32;

  op2_32 = i->Id();

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    diff_32 = op1_32 - (op2_32 + temp_CF);
    BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    diff_32 = op1_32 - (op2_32 + temp_CF);
    Write_RMW_virtual_dword(diff_32);
    }

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                         temp_CF);
}


  void
BX_CPU_C::SUB_EdGd(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32, diff_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    diff_32 = op1_32 - op2_32;
    BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    diff_32 = op1_32 - op2_32;
    Write_RMW_virtual_dword(diff_32);
    }

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}


  void
BX_CPU_C::SUB_GdEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
    }

  diff_32 = op1_32 - op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}

  void
BX_CPU_C::SUB_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;
  op2_32 = i->Id();

  diff_32 = op1_32 - op2_32;

  RAX = diff_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}


  void
BX_CPU_C::CMP_EdGd(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit32u diff_32;
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
#endif
}


  void
BX_CPU_C::CMP_GdEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit32u diff_32;
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
#endif
}


  void
BX_CPU_C::CMP_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit32u diff_32;
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
#endif
}


  void
BX_CPU_C::CWDE(bxInstruction_c *i)
{
  /* CBW: no flags are effected */
  Bit32u temp;

  temp = (Bit16s) AX;
  RAX = temp;
}

  void
BX_CPU_C::CDQ(bxInstruction_c *i)
{
  /* CDQ: no flags are affected */

  if (EAX & 0x80000000) {
    RDX = 0xFFFFFFFF;
    }
  else {
    RDX = 0x00000000;
    }
}

// Some info on the opcodes at {0F,A6} and {0F,A7}
// On 386 steps A0-B0:
//   {OF,A6} = XBTS
//   {OF,A7} = IBTS
// On 486 steps A0-B0:
//   {OF,A6} = CMPXCHG 8
//   {OF,A7} = CMPXCHG 16|32
//
// On 486 >= B steps, and further processors, the
// CMPXCHG instructions were moved to opcodes:
//   {OF,B0} = CMPXCHG 8
//   {OF,B1} = CMPXCHG 16|32

  void
BX_CPU_C::CMPXCHG_XBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_XBTS:"));
  UndefinedOpcode(i);
}

  void
BX_CPU_C::CMPXCHG_IBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_IBTS:"));
  UndefinedOpcode(i);
}


  void
BX_CPU_C::XADD_EdGd(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u op2_32, op1_32, sum_32;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    sum_32 = op1_32 + op2_32;
    // and write destination into source
    // Note: if both op1 & op2 are registers, the last one written
    //       should be the sum, as op1 & op2 may be the same register.
    //       For example:  XADD AL, AL
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
    BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    sum_32 = op1_32 + op2_32;
    Write_RMW_virtual_dword(sum_32);
    /* and write destination into source */
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
    }

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_XADD32);
#else
  BX_INFO (("XADD_EdGd not supported for cpulevel <= 3"))
  UndefinedOpcode(i);
#endif
}



  void
BX_CPU_C::ADD_EEdId(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32, sum_32;

  op2_32 = i->Id();

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "addl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  sum_32 = op1_32 + op2_32;
#endif

  Write_RMW_virtual_dword(sum_32);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
#endif
}

  void
BX_CPU_C::ADD_EGdId(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32, sum_32;

  op2_32 = i->Id();
  op1_32 = BX_READ_32BIT_REG(i->rm());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "addl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  sum_32 = op1_32 + op2_32;
#endif

  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
#endif
}

  void
BX_CPU_C::ADC_EdId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();

  Bit32u op2_32, op1_32, sum_32;

  op2_32 = i->Id();

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    sum_32 = op1_32 + op2_32 + temp_CF;
    BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    sum_32 = op1_32 + op2_32 + temp_CF;
    Write_RMW_virtual_dword(sum_32);
    }

  SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                         temp_CF);
}


  void
BX_CPU_C::SUB_EdId(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32, diff_32;

  op2_32 = i->Id();

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    diff_32 = op1_32 - op2_32;
    BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    diff_32 = op1_32 - op2_32;
    Write_RMW_virtual_dword(diff_32);
    }

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}

  void
BX_CPU_C::CMP_EdId(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32;

  op2_32 = i->Id();

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit32u diff_32;
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
#endif
}




  void
BX_CPU_C::NEG_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, diff_32;

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    diff_32 = 0 - op1_32;
    BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    diff_32 = 0 - op1_32;
    Write_RMW_virtual_dword(diff_32);
    }

  SET_FLAGS_OSZAPC_32(op1_32, 0, diff_32, BX_INSTR_NEG32);
}


  void
BX_CPU_C::INC_Ed(bxInstruction_c *i)
{
  Bit32u op1_32;

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op1_32++;
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    op1_32++;
    Write_RMW_virtual_dword(op1_32);
    }

  SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_INC32);
}


  void
BX_CPU_C::DEC_Ed(bxInstruction_c *i)
{
  Bit32u op1_32;

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op1_32--;
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    op1_32--;
    Write_RMW_virtual_dword(op1_32);
    }

  SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_DEC32);
}


  void
BX_CPU_C::CMPXCHG_EdGd(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u op2_32, op1_32, diff_32;

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    }
  else {
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
    }

  diff_32 = EAX - op1_32;

  SET_FLAGS_OSZAPC_32(EAX, op1_32, diff_32, BX_INSTR_CMP32);

  if (diff_32 == 0) {  // if accumulator == dest
    // ZF = 1
    set_ZF(1);
    // dest <-- src
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
      }
    else {
      Write_RMW_virtual_dword(op2_32);
      }
    }
  else {
    // ZF = 0
    set_ZF(0);
    // accumulator <-- dest
    RAX = op1_32;
    }
#else
  BX_PANIC(("CMPXCHG_EdGd:"));
#endif
}

  void
BX_CPU_C::CMPXCHG8B(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 5) || (BX_CPU_LEVEL_HACKED >= 5)

  Bit32u op1_64_lo, op1_64_hi, diff;

  if (i->modC0()) {
    BX_INFO(("CMPXCHG8B: dest is reg: #UD"));
    UndefinedOpcode(i);
    }

  read_virtual_dword(i->seg(), RMAddr(i), &op1_64_lo);
  read_RMW_virtual_dword(i->seg(), RMAddr(i) + 4, &op1_64_hi);

  diff = EAX - op1_64_lo;
  diff |= EDX - op1_64_hi;

//     SET_FLAGS_OSZAPC_32(EAX, op1_32, diff_32, BX_INSTR_CMP32);

  if (diff == 0) {  // if accumulator == dest
    // ZF = 1
    set_ZF(1);
    // dest <-- src
    Write_RMW_virtual_dword(ECX);
    write_virtual_dword(i->seg(), RMAddr(i), &EBX);
    }
  else {
    // ZF = 0
    set_ZF(0);
    // accumulator <-- dest
    RAX = op1_64_lo;
    RDX = op1_64_hi;
    }

#else
  BX_INFO(("CMPXCHG8B: not implemented yet"));
  UndefinedOpcode(i);
#endif
}
