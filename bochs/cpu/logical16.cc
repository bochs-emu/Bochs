/////////////////////////////////////////////////////////////////////////
// $Id: logical16.cc,v 1.14 2002-10-03 18:12:40 kevinlawton Exp $
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
BX_CPU_C::XOR_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "xorw %3, %1 \n\t"
      "pushfl      \n\t"
      "popl   %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
        (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
        (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 ^ op2_16;
#endif
    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "xorw %3, %1 \n\t"
      "pushfl      \n\t"
      "popl   %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
        (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
        (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 ^ op2_16;
#endif
    Write_RMW_virtual_word(result_16);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
#endif
}


  void
BX_CPU_C::XOR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

  result_16 = op1_16 ^ op2_16;

  BX_WRITE_16BIT_REG(i->nnn(), result_16);

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
}


  void
BX_CPU_C::XOR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, sum_16;

  op1_16 = AX;
  op2_16 = i->Iw();

  sum_16 = op1_16 ^ op2_16;

  AX = sum_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_XOR16);
}

  void
BX_CPU_C::XOR_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;


  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 ^ op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 ^ op2_16;
    Write_RMW_virtual_word(result_16);
    }

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
}


  void
BX_CPU_C::OR_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;


  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 | op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 | op2_16;
    Write_RMW_virtual_word(result_16);
    }

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
}


  void
BX_CPU_C::NOT_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = ~op1_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = ~op1_16;
    Write_RMW_virtual_word(result_16);
    }
}


  void
BX_CPU_C::OR_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 | op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 | op2_16;
    Write_RMW_virtual_word(result_16);
    }

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
}


  void
BX_CPU_C::OR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "orw %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
      (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  result_16 = op1_16 | op2_16;
#endif

  BX_WRITE_16BIT_REG(i->nnn(), result_16);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
#endif
}


  void
BX_CPU_C::OR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, sum_16;

  op1_16 = AX;
  op2_16 = i->Iw();

  sum_16 = op1_16 | op2_16;

  AX = sum_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_OR16);
}



  void
BX_CPU_C::AND_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "andw %3, %1\n\t"
      "pushfl     \n\t"
      "popl %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
      (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 & op2_16;
#endif

    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "andw %3, %1\n\t"
      "pushfl     \n\t"
      "popl %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
      (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 & op2_16;
#endif

    Write_RMW_virtual_word(result_16);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
#endif
}


  void
BX_CPU_C::AND_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "andw %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  result_16 = op1_16 & op2_16;
#endif

  BX_WRITE_16BIT_REG(i->nnn(), result_16);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
#endif
}


  void
BX_CPU_C::AND_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = AX;
  op2_16 = i->Iw();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "andw %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  result_16 = op1_16 & op2_16;
#endif

  AX = result_16;

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
#endif
}

  void
BX_CPU_C::AND_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "andw %3, %1\n\t"
      "pushfl     \n\t"
      "popl %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
      (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 & op2_16;
#endif

    BX_WRITE_16BIT_REG(i->rm(), result_16);
    }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "andw %3, %1\n\t"
      "pushfl     \n\t"
      "popl %0"
      : "=g" (flags32), "=r" (result_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
      (flags32 & EFlagsOSZAPCMask);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    result_16 = op1_16 & op2_16;
#endif

    Write_RMW_virtual_word(result_16);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
#endif
}


  void
BX_CPU_C::TEST_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "testw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u result_16;
  result_16 = op1_16 & op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
#endif
}



  void
BX_CPU_C::TEST_AXIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op1_16 = AX;
  op2_16 = i->Iw();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "testw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u result_16;
  result_16 = op1_16 & op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
#endif
}


  void
BX_CPU_C::TEST_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "testw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) |
    (flags32 & EFlagsOSZAPCMask);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u result_16;
  result_16 = op1_16 & op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
#endif
}
