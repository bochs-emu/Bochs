/////////////////////////////////////////////////////////////////////////
// $Id: stack_pro.cc,v 1.41 2008-03-24 22:13:04 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPP_AttrRegparmN(1) BX_CPU_C::push_16(Bit16u value16)
{
  /* must use StackAddrSize, and either RSP, ESP or SP accordingly */
#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
  {
    write_virtual_word(BX_SEG_REG_SS, RSP-2, value16);
    RSP -= 2;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (ESP-2), value16);
    ESP -= 2;
  }
  else
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (SP-2), value16);
    SP  -= 2;
  }
}

/* push 32 bit operand */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::push_32(Bit32u value32)
{
  /* must use StackAddrSize, and either RSP, ESP or SP accordingly */
#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
  {
    write_virtual_dword(BX_SEG_REG_SS, RSP-4, value32);
    RSP -= 4;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    write_virtual_dword(BX_SEG_REG_SS, (Bit32u) (ESP-4), value32);
    ESP -= 4;
  }
  else
  {
    write_virtual_dword(BX_SEG_REG_SS, (Bit16u) (SP-4), value32);
    SP  -= 4;
  }
}

/* push 64 bit operand */
#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::push_64(Bit64u value64)
{
  BX_ASSERT(StackAddrSize64());

  write_virtual_qword(BX_SEG_REG_SS, RSP-8, value64);
  RSP -= 8;
}
#endif

/* pop 16 bit operand from the stack */
Bit16u BX_CPU_C::pop_16(void)
{
  Bit16u value16;

#if BX_SUPPORT_X86_64
  if (StackAddrSize64()) {
    value16 = read_virtual_word(BX_SEG_REG_SS, RSP);
    RSP += 2;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    value16 = read_virtual_word(BX_SEG_REG_SS, ESP);
    ESP += 2;
  }
  else {
    value16 = read_virtual_word(BX_SEG_REG_SS, SP);
    SP  += 2;
  }

  return value16;
}

/* pop 32 bit operand from the stack */
Bit32u BX_CPU_C::pop_32(void)
{
  Bit32u value32;

#if BX_SUPPORT_X86_64
  if (StackAddrSize64()) {
    value32 = read_virtual_dword(BX_SEG_REG_SS, RSP);
    RSP += 4;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    value32 = read_virtual_dword(BX_SEG_REG_SS, ESP);
    ESP += 4;
  }
  else {
    value32 = read_virtual_dword(BX_SEG_REG_SS, SP);
    SP  += 4;
  }

  return value32;
}

/* pop 64 bit operand from the stack */
#if BX_SUPPORT_X86_64
Bit64u BX_CPU_C::pop_64(void)
{
  BX_ASSERT(StackAddrSize64());

  Bit64u value64 = read_virtual_qword(BX_SEG_REG_SS, RSP);
  RSP += 8;

  return value64;
}
#endif

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::can_pop(Bit32u bytes)
{
  Bit32u temp_ESP, expand_down_limit;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return(1);
  }
#endif

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* Big bit set: use ESP */
    temp_ESP = ESP;
    expand_down_limit = 0xFFFFFFFF;
  }
  else { /* Big bit clear: use SP */
    temp_ESP = SP;
    expand_down_limit = 0xFFFF;
  }

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid==0) {
    BX_ERROR(("can_pop(): SS invalidated"));
    return(0); /* never gets here */
  }

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p==0) {
    BX_ERROR(("can_pop(): SS.p = 0"));
    return(0);
  }

  if (IS_DATA_SEGMENT_EXPAND_DOWN(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type)) { /* expand down */
    if (temp_ESP == expand_down_limit) {
      BX_PANIC(("can_pop(): found SP=ffff"));
      return(0);
    }
    if (((expand_down_limit - temp_ESP) + 1) >= bytes)
      return(1);
    return(0);
  }
  else { /* normal (expand-up) segment */
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled==0) {
      BX_ERROR(("can_pop(): SS.limit = 0"));
      return(0);
    }
    if (temp_ESP == expand_down_limit) {
      BX_ERROR(("can_pop(): found SP=ffff"));
      return(0);
    }
    if (temp_ESP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled) {
      BX_ERROR(("can_pop(): eSP > SS.limit"));
      return(0);
    }
    if (((BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled - temp_ESP) + 1) >= bytes)
      return(1);
    return(0);
  }
}
