/////////////////////////////////////////////////////////////////////////
// $Id: soft_int.cc,v 1.6 2001-10-03 13:10:37 bdenney Exp $
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
BX_CPU_C::BOUND_GvMa(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("BOUND_GvMa: not supported on 8086!"));
#else

  if (i->mod == 0xc0) {
    /* undefined opcode exception */
    BX_PANIC(("bound: op2 must be mem ref"));
    UndefinedOpcode(i);
    }

  if (i->os_32) {
    Bit32s bound_min, bound_max;
    Bit32s op1_32;

    op1_32 = BX_READ_32BIT_REG(i->nnn);

    read_virtual_dword(i->seg, i->rm_addr, (Bit32u *) &bound_min);
    read_virtual_dword(i->seg, i->rm_addr+4, (Bit32u *) &bound_max);

    /* ??? */
    if ( (op1_32 < bound_min) || (op1_32 > bound_max) ) {
      BX_INFO(("BOUND: fails bounds test"));
      exception(5, 0, 0);
      }
    }
  else {
    Bit16s bound_min, bound_max;
    Bit16s op1_16;

    op1_16 = BX_READ_16BIT_REG(i->nnn);

    read_virtual_word(i->seg, i->rm_addr, (Bit16u *) &bound_min);
    read_virtual_word(i->seg, i->rm_addr+2, (Bit16u *) &bound_max);

    /* ??? */
    if ( (op1_16 < bound_min) || (op1_16 > bound_max) ) {
      BX_INFO(("BOUND: fails bounds test"));
      exception(5, 0, 0);
      }
    }

#endif
}

  void
BX_CPU_C::INT1(BxInstruction_t *i)
{
  // This is an undocumented instrucion (opcode 0xf1)
  // which is useful for an ICE system.

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_int;
#endif

  interrupt(1, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      BX_CPU_THIS_PTR eip);
}

  void
BX_CPU_C::INT3(BxInstruction_t *i)
{
  // INT 3 is not IOPL sensitive

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_int;
#endif

//BX_PANIC(("INT3: bailing"));
  interrupt(3, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      BX_CPU_THIS_PTR eip);
}


  void
BX_CPU_C::INT_Ib(BxInstruction_t *i)
{
  Bit8u imm8;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_int;
#endif

  imm8 = i->Ib;

  if (v8086_mode() && (IOPL<3)) {
    //BX_INFO(("int_ib: v8086: IOPL<3"));
    exception(BX_GP_EXCEPTION, 0, 0);
    }

#ifdef SHOW_EXIT_STATUS
if ( (imm8 == 0x21) && (AH == 0x4c) ) {
  BX_INFO(("INT 21/4C called AL=0x%02x, BX=0x%04x", (unsigned) AL, (unsigned) BX));
  }
#endif

  interrupt(imm8, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      BX_CPU_THIS_PTR eip);
}


  void
BX_CPU_C::INTO(BxInstruction_t *i)
{

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_int;
#endif

  /* ??? is this IOPL sensitive ? */
  if (v8086_mode()) BX_PANIC(("soft_int: v8086 mode unsupported"));

  if (get_OF()) {
    interrupt(4, 1, 0, 0);
    BX_INSTR_FAR_BRANCH(BX_INSTR_IS_INT,
                        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                        BX_CPU_THIS_PTR eip);
    }
}
