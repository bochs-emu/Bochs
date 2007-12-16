/////////////////////////////////////////////////////////////////////////
// $Id: stack_pro.cc,v 1.35 2007-12-16 21:46:39 sshwarts Exp $
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


void BX_CPU_C::push_16(Bit16u value16)
{
  /* must use StackAddrSize, and either RSP, ESP or SP accordingly */
#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
  {
    write_virtual_word(BX_SEG_REG_SS, RSP-2, &value16);
    RSP -= 2;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (ESP-2), &value16);
    ESP -= 2;
  }
  else
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (SP-2), &value16);
    SP  -= 2;
  }
}

/* push 32 bit operand */
void BX_CPU_C::push_32(Bit32u value32)
{
  /* must use StackAddrSize, and either RSP, ESP or SP accordingly */
#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
  {
    write_virtual_dword(BX_SEG_REG_SS, RSP-4, &value32);
    RSP -= 4;
  }
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* StackAddrSize = 32 */
    write_virtual_dword(BX_SEG_REG_SS, (Bit32u) (ESP-4), &value32);
    ESP -= 4;
  }
  else
  {
    write_virtual_dword(BX_SEG_REG_SS, (Bit16u) (SP-4), &value32);
    SP  -= 4;
  }
}

/* push 64 bit operand */
#if BX_SUPPORT_X86_64
void BX_CPU_C::push_64(Bit64u value64)
{
  BX_ASSERT(StackAddrSize64());

  write_virtual_qword(BX_SEG_REG_SS, RSP-8, &value64);
  RSP -= 8;
}
#endif

/* pop 16 bit operand from the stack */
void BX_CPU_C::pop_16(Bit16u *value16_ptr)
{
  bx_address temp_RSP;

#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
    temp_RSP = RSP;
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_RSP = ESP;
  else
    temp_RSP = SP;

  read_virtual_word(BX_SEG_REG_SS, temp_RSP, value16_ptr);

#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
    RSP += 2;
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    ESP += 2;
  else
    SP  += 2;
}

/* pop 32 bit operand from the stack */
void BX_CPU_C::pop_32(Bit32u *value32_ptr)
{
  bx_address temp_RSP;

#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
    temp_RSP = RSP;
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_RSP = ESP;
  else
    temp_RSP = SP;

  read_virtual_dword(BX_SEG_REG_SS, temp_RSP, value32_ptr);

#if BX_SUPPORT_X86_64
  if (StackAddrSize64())
    RSP += 4;
  else
#endif
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    ESP += 4;
  else
    SP  += 4;
}

/* pop 64 bit operand from the stack */
#if BX_SUPPORT_X86_64
void BX_CPU_C::pop_64(Bit64u *value64_ptr)
{
  BX_ASSERT(StackAddrSize64());

  read_virtual_qword(BX_SEG_REG_SS, RSP, value64_ptr);
  RSP += 8;
}
#endif

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::can_push(bx_descriptor_t *descriptor, Bit32u esp, Bit32u bytes)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return(1);
  }
#endif

  // small stack compares against 16-bit SP
  if (!descriptor->u.segment.d_b)
    esp &= 0x0000ffff;

  if (descriptor->valid==0) {
    BX_ERROR(("can_push(): SS invalidated."));
    return(0);
  }

  if (descriptor->p==0) {
    BX_ERROR(("can_push(): descriptor not present"));
    return(0);
  }

  if (IS_DATA_SEGMENT_EXPAND_DOWN(descriptor->type)) /* expand down segment */
  {
    Bit32u expand_down_limit;

    if (descriptor->u.segment.d_b)
      expand_down_limit = 0xffffffff;
    else
      expand_down_limit = 0x0000ffff;

    if (esp==0) {
      BX_PANIC(("can_push(): esp=0, wraparound?"));
      return(0);
    }

    if (esp < bytes) {
      BX_PANIC(("can_push(): expand-down: esp < N"));
      return(0);
    }
    if ((esp - bytes) <= descriptor->u.segment.limit_scaled) {
      BX_ERROR(("can_push(): expand-down: esp-N < limit"));
      return(0);
    }
    if (esp > expand_down_limit) {
      BX_ERROR(("can_push(): esp > expand-down-limit"));
      return(0);
    }
    return(1);
  }
  else { /* normal (expand-up) segment */
    if (descriptor->u.segment.limit_scaled==0) {
      BX_PANIC(("can_push(): found limit of 0"));
      return(0);
    }

    // Look at case where esp==0.  Possibly, it's an intentional wraparound
    // If so, limit must be the maximum for the given stack size
    if (esp==0) {
      if (descriptor->u.segment.d_b && (descriptor->u.segment.limit_scaled==0xffffffff))
        return(1);
      if ((descriptor->u.segment.d_b==0) && (descriptor->u.segment.limit_scaled>=0xffff))
        return(1);
      BX_ERROR(("can_push(): esp=0, normal, wraparound? limit=%08x",
        descriptor->u.segment.limit_scaled));
      return(0);
    }

    if (!descriptor->u.segment.d_b) {
      // Weird case for 16-bit SP.
      esp = ((esp-bytes) & 0xffff) + bytes;
    }
    if (esp < bytes) {
      BX_ERROR(("can_push(): expand-up: esp < N"));
      return(0);
    }
    if ((esp-1) > descriptor->u.segment.limit_scaled) {
      BX_ERROR(("can_push(): expand-up: SP > limit"));
      return(0);
    }
    /* all checks pass */
    return(1);
  }
}

bx_bool BX_CPU_C::can_pop(Bit32u bytes)
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
