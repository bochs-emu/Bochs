/////////////////////////////////////////////////////////////////////////
// $Id: bit.cc,v 1.13 2002-10-25 17:23:33 sshwarts Exp $
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
// Make life easier merging cpu64 and cpu code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#define RSP ESP
#define RSI ESI
#define RDI EDI
#define RBP EBP
#endif


  void
BX_CPU_C::SETO_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETO: not available on < 386"));
#else
  Bit8u result_8;


  if (get_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNO_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNO: not available on < 386"));
#else
  Bit8u result_8;


  if (get_OF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETB_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETB: not available on < 386"));
#else
  Bit8u result_8;


  if (get_CF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNB_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNB: not available on < 386"));
#else
  Bit8u result_8;


  if (get_CF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETZ_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETZ: not available on < 386"));
#else
  Bit8u result_8;


  if (get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNZ_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNZ: not available on < 386"));
#else
  Bit8u result_8;


  if (get_ZF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETBE_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETBE: not available on < 386"));
#else
  Bit8u result_8;


  if (get_CF() || get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNBE_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNBE: not available on < 386"));
#else
  Bit8u result_8;


  if ((get_CF()==0) && (get_ZF()==0))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETS_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETS: not available on < 386"));
#else
  Bit8u result_8;


  if (get_SF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNS_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNL: not available on < 386"));
#else
  Bit8u result_8;


  if (get_SF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETP_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETP: not available on < 386"));
#else
  Bit8u result_8;


  if (get_PF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNP_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNP: not available on < 386"));
#else
  Bit8u result_8;


  if (get_PF() == 0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETL_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETL: not available on < 386"));
#else
  Bit8u result_8;


  if (getB_SF() != getB_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNL_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNL: not available on < 386"));
#else
  Bit8u result_8;


  if (getB_SF() == getB_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETLE_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETLE: not available on < 386"));
#else
  Bit8u result_8;


  if (get_ZF() || (getB_SF()!=getB_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}

  void
BX_CPU_C::SETNLE_Eb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("SETNLE: not available on < 386"));
#else
  Bit8u result_8;


  if ((get_ZF()==0) && (getB_SF()==getB_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
    }
#endif
}


  void
BX_CPU_C::BSF_GvEv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BSF_GvEv(): not supported on < 386"));
#else


#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64;

    /* op2_64 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
      }

    if (op2_64 == 0) {
      set_ZF(1);
      /* op1_64 undefined */
      return;
      }

    op1_64 = 0;
    while ( (op2_64 & 0x01) == 0 ) {
      op1_64++;
      op2_64 >>= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_64BIT_REG(i->nnn(), op1_64);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32;

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    if (op2_32 == 0) {
      set_ZF(1);
      /* op1_32 undefined */
      return;
      }

    op1_32 = 0;
    while ( (op2_32 & 0x01) == 0 ) {
      op1_32++;
      op2_32 >>= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16;

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    if (op2_16 == 0) {
      set_ZF(1);
      /* op1_16 undefined */
      return;
      }

    op1_16 = 0;
    while ( (op2_16 & 0x01) == 0 ) {
      op1_16++;
      op2_16 >>= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(i->nnn(), op1_16);
    }
#endif
}

  void
BX_CPU_C::BSR_GvEv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BSR_GvEv(): not supported on < 386"));
#else


#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64;

    /* op2_64 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
      }

    if (op2_64 == 0) {
      set_ZF(1);
      /* op1_64 undefined */
      return;
      }

    op1_64 = 63;
    while ( (op2_64 & BX_CONST64(0x8000000000000000)) == 0 ) {
      op1_64--;
      op2_64 <<= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_64BIT_REG(i->nnn(), op1_64);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32;

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    if (op2_32 == 0) {
      set_ZF(1);
      /* op1_32 undefined */
      return;
      }

    op1_32 = 31;
    while ( (op2_32 & 0x80000000) == 0 ) {
      op1_32--;
      op2_32 <<= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16;

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    if (op2_16 == 0) {
      set_ZF(1);
      /* op1_16 undefined */
      return;
      }

    op1_16 = 15;
    while ( (op2_16 & 0x8000) == 0 ) {
      op1_16--;
      op2_16 <<= 1;
      }
    set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(i->nnn(), op1_16);
    }
#endif
}


  void
BX_CPU_C::BSWAP_EAX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u eax, b0, b1, b2, b3;

  eax = EAX;
  b0  = eax & 0xff; eax >>= 8;
  b1  = eax & 0xff; eax >>= 8;
  b2  = eax & 0xff; eax >>= 8;
  b3  = eax;

  RAX = (b0<<24) | (b1<<16) | (b2<<8) | b3;  // zero extended
#else
  BX_PANIC(("BSWAP_EAX: not implemented CPU <= 3"));
#endif
}

  void
BX_CPU_C::BSWAP_ECX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u ecx, b0, b1, b2, b3;

  ecx = ECX;
  b0  = ecx & 0xff; ecx >>= 8;
  b1  = ecx & 0xff; ecx >>= 8;
  b2  = ecx & 0xff; ecx >>= 8;
  b3  = ecx;

  RCX = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_ECX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_EDX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u edx, b0, b1, b2, b3;

  edx = EDX;
  b0  = edx & 0xff; edx >>= 8;
  b1  = edx & 0xff; edx >>= 8;
  b2  = edx & 0xff; edx >>= 8;
  b3  = edx;

  RDX = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_EDX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_EBX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u ebx, b0, b1, b2, b3;

  ebx = EBX;
  b0  = ebx & 0xff; ebx >>= 8;
  b1  = ebx & 0xff; ebx >>= 8;
  b2  = ebx & 0xff; ebx >>= 8;
  b3  = ebx;

  RBX = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_EBX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_ESP(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u esp, b0, b1, b2, b3;

  esp = ESP;
  b0  = esp & 0xff; esp >>= 8;
  b1  = esp & 0xff; esp >>= 8;
  b2  = esp & 0xff; esp >>= 8;
  b3  = esp;

  RSP = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_ESP: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_EBP(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u ebp, b0, b1, b2, b3;

  ebp = EBP;
  b0  = ebp & 0xff; ebp >>= 8;
  b1  = ebp & 0xff; ebp >>= 8;
  b2  = ebp & 0xff; ebp >>= 8;
  b3  = ebp;

  RBP = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_EBP: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_ESI(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u esi, b0, b1, b2, b3;

  esi = ESI;
  b0  = esi & 0xff; esi >>= 8;
  b1  = esi & 0xff; esi >>= 8;
  b2  = esi & 0xff; esi >>= 8;
  b3  = esi;

  RSI = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_ESI: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_EDI(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit32u edi, b0, b1, b2, b3;

  edi = EDI;
  b0  = edi & 0xff; edi >>= 8;
  b1  = edi & 0xff; edi >>= 8;
  b2  = edi & 0xff; edi >>= 8;
  b3  = edi;

  RDI = (b0<<24) | (b1<<16) | (b2<<8) | b3;
#else
  BX_PANIC(("BSWAP_EDI: not implemented CPU <= 3"));
#endif
}


#if BX_SUPPORT_X86_64
  void
BX_CPU_C::BSWAP_RAX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rax, b0, b1, b2, b3, b4, b5, b6, b7;

  rax = RAX;
  b0  = rax & 0xff; rax >>= 8;
  b1  = rax & 0xff; rax >>= 8;
  b2  = rax & 0xff; rax >>= 8;
  b3  = rax & 0xff; rax >>= 8;
  b4  = rax & 0xff; rax >>= 8;
  b5  = rax & 0xff; rax >>= 8;
  b6  = rax & 0xff; rax >>= 8;
  b7  = rax;

  RAX = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b4<<16) | (b4<<8) | b7;
#else
  BX_PANIC(("BSWAP_RAX: not implemented CPU <= 3"));
#endif
}

  void
BX_CPU_C::BSWAP_RCX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rcx, b0, b1, b2, b3, b4, b5, b6, b7;

  rcx = RCX;
  b0  = rcx & 0xff; rcx >>= 8;
  b1  = rcx & 0xff; rcx >>= 8;
  b2  = rcx & 0xff; rcx >>= 8;
  b3  = rcx & 0xff; rcx >>= 8;
  b4  = rcx & 0xff; rcx >>= 8;
  b5  = rcx & 0xff; rcx >>= 8;
  b6  = rcx & 0xff; rcx >>= 8;
  b7  = rcx;

  RCX = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_ECX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RDX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rdx, b0, b1, b2, b3, b4, b5, b6, b7;

  rdx = RDX;
  b0  = rdx & 0xff; rdx >>= 8;
  b1  = rdx & 0xff; rdx >>= 8;
  b2  = rdx & 0xff; rdx >>= 8;
  b3  = rdx & 0xff; rdx >>= 8;
  b4  = rdx & 0xff; rdx >>= 8;
  b5  = rdx & 0xff; rdx >>= 8;
  b6  = rdx & 0xff; rdx >>= 8;
  b7  = rdx;

  RDX = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_EDX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RBX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rbx, b0, b1, b2, b3, b4, b5, b6, b7;

  rbx = RBX;
  b0  = rbx & 0xff; rbx >>= 8;
  b1  = rbx & 0xff; rbx >>= 8;
  b2  = rbx & 0xff; rbx >>= 8;
  b3  = rbx & 0xff; rbx >>= 8;
  b4  = rbx & 0xff; rbx >>= 8;
  b5  = rbx & 0xff; rbx >>= 8;
  b6  = rbx & 0xff; rbx >>= 8;
  b7  = rbx;

  RBX = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_EBX: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RSP(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rsp, b0, b1, b2, b3, b4, b5, b6, b7;

  rsp = RSP;
  b0  = rsp & 0xff; rsp >>= 8;
  b1  = rsp & 0xff; rsp >>= 8;
  b2  = rsp & 0xff; rsp >>= 8;
  b3  = rsp & 0xff; rsp >>= 8;
  b4  = rsp & 0xff; rsp >>= 8;
  b5  = rsp & 0xff; rsp >>= 8;
  b6  = rsp & 0xff; rsp >>= 8;
  b7  = rsp;

  RSP = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_ESP: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RBP(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rbp, b0, b1, b2, b3, b4, b5, b6, b7;

  rbp = RBP;
  b0  = rbp & 0xff; rbp >>= 8;
  b1  = rbp & 0xff; rbp >>= 8;
  b2  = rbp & 0xff; rbp >>= 8;
  b3  = rbp & 0xff; rbp >>= 8;
  b4  = rbp & 0xff; rbp >>= 8;
  b5  = rbp & 0xff; rbp >>= 8;
  b6  = rbp & 0xff; rbp >>= 8;
  b7  = rbp;

  RBP = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_EBP: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RSI(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rsi, b0, b1, b2, b3, b4, b5, b6, b7;

  rsi = RSI;
  b0  = rsi & 0xff; rsi >>= 8;
  b1  = rsi & 0xff; rsi >>= 8;
  b2  = rsi & 0xff; rsi >>= 8;
  b3  = rsi & 0xff; rsi >>= 8;
  b4  = rsi & 0xff; rsi >>= 8;
  b5  = rsi & 0xff; rsi >>= 8;
  b6  = rsi & 0xff; rsi >>= 8;
  b7  = rsi;

  RSI = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_ESI: not implemented CPU <= 3"));
#endif
}
  void
BX_CPU_C::BSWAP_RDI(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit64u rdi, b0, b1, b2, b3, b4, b5, b6, b7;

  rdi = RDI;
  b0  = rdi & 0xff; rdi >>= 8;
  b1  = rdi & 0xff; rdi >>= 8;
  b2  = rdi & 0xff; rdi >>= 8;
  b3  = rdi & 0xff; rdi >>= 8;
  b4  = rdi & 0xff; rdi >>= 8;
  b5  = rdi & 0xff; rdi >>= 8;
  b6  = rdi & 0xff; rdi >>= 8;
  b7  = rdi;

  RDI = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;
#else
  BX_PANIC(("BSWAP_EDI: not implemented CPU <= 3"));
#endif
}
#endif  // #if BX_SUPPORT_X86_64


  void
BX_CPU_C::BT_EvGv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BT_EvGv: not available on <386"));
#else
  bx_address op1_addr;

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64;
    Bit64s displacement64;
    Bit64u index;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      op2_64 &= 0x3f;
      set_CF((op1_64 >> op2_64) & 0x01);
      return;
      }

    index = op2_64 & 0x3f;
    displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
    op1_addr = RMAddr(i) + 8 * displacement64;

    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), op1_addr, &op1_64);

    set_CF((op1_64 >> index) & 0x01);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, index;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      op2_32 &= 0x1f;
      set_CF((op1_32 >> op2_32) & 0x01);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr = RMAddr(i) + 4 * displacement32;

    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), op1_addr, &op1_32);

    set_CF((op1_32 >> index) & 0x01);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, index;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      op2_16 &= 0x0f;
      set_CF((op1_16 >> op2_16) & 0x01);
      return;
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr = RMAddr(i) + 2 * displacement32;

    /* pointer, segment address pair */
    read_virtual_word(i->seg(), op1_addr, &op1_16);

    set_CF((op1_16 >> index) & 0x01);
    }
#endif
}

  void
BX_CPU_C::BTS_EvGv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTS_EvGv: not available on <386"));
#else
  bx_address op1_addr;

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, index;
    Bit64s displacement64;
    Bit64u bit_i;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      op2_64 &= 0x3f;
      set_CF((op1_64 >> op2_64) & 0x01);
      op1_64 |= (((Bit64u) 1) << op2_64);

      /* now write diff back to destination */
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      return;
      }

    index = op2_64 & 0x3f;
    displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
    op1_addr = RMAddr(i) + 8 * displacement64;

    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

    bit_i = (op1_64 >> index) & 0x01;
    op1_64 |= (((Bit64u) 1) << index);

    write_RMW_virtual_qword(op1_64);

    set_CF(bit_i);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, bit_i, index;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      op2_32 &= 0x1f;
      set_CF((op1_32 >> op2_32) & 0x01);
      op1_32 |= (((Bit32u) 1) << op2_32);

      /* now write diff back to destination */
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr = RMAddr(i) + 4 * displacement32;

    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

    bit_i = (op1_32 >> index) & 0x01;
    op1_32 |= (((Bit32u) 1) << index);

    write_RMW_virtual_dword(op1_32);

    set_CF(bit_i);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, bit_i, index;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      op2_16 &= 0x0f;
      set_CF((op1_16 >> op2_16) & 0x01);
      op1_16 |= (((Bit16u) 1) << op2_16);

      /* now write diff back to destination */
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      return;
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr = RMAddr(i) + 2 * displacement32;

    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

    bit_i = (op1_16 >> index) & 0x01;
    op1_16 |= (((Bit16u) 1) << index);

    write_RMW_virtual_word(op1_16);

    set_CF(bit_i);
    }
#endif
}

  void
BX_CPU_C::BTR_EvGv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTR_EvGv: not available on <386"));
#else
  bx_address op1_addr;


#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, index;
    Bit64s displacement64;
    Bit64u temp_cf;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      op2_64 &= 0x3f;
      set_CF((op1_64 >> op2_64) & 0x01);
      op1_64 &= ~(((Bit64u) 1) << op2_64);

      /* now write diff back to destination */
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      return;
      }

    index = op2_64 & 0x3f;
    displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
    op1_addr = RMAddr(i) + 8 * displacement64;

    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

    temp_cf = (op1_64 >> index) & 0x01;
    op1_64 &= ~(((Bit64u) 1) << index);

    /* now write back to destination */
    write_RMW_virtual_qword(op1_64);

    set_CF(temp_cf);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, index, temp_cf;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      op2_32 &= 0x1f;
      set_CF((op1_32 >> op2_32) & 0x01);
      op1_32 &= ~(((Bit32u) 1) << op2_32);

      /* now write diff back to destination */
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr = RMAddr(i) + 4 * displacement32;

    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

    temp_cf = (op1_32 >> index) & 0x01;
    op1_32 &= ~(((Bit32u) 1) << index);

    /* now write back to destination */
    write_RMW_virtual_dword(op1_32);

    set_CF(temp_cf);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, index, temp_cf;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      op2_16 &= 0x0f;
      set_CF((op1_16 >> op2_16) & 0x01);
      op1_16 &= ~(((Bit16u) 1) << op2_16);

      /* now write diff back to destination */
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      return;
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr = RMAddr(i) + 2 * displacement32;

    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

    temp_cf = (op1_16 >> index) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << index);

    /* now write back to destination */
    write_RMW_virtual_word(op1_16);

    set_CF(temp_cf);
    }
#endif
}

  void
BX_CPU_C::BTC_EvGv(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTC_EvGv: not available on <386"));
#else
  bx_address op1_addr;

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64;
    Bit64s displacement64;
    Bit64u temp_CF, index;

    op2_64 = BX_READ_64BIT_REG(i->nnn());
    index = op2_64 & 0x3f;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      op1_addr = 0; // keep compiler happy
      }
    else {
      displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
      op1_addr = RMAddr(i) + 8 * displacement64;
      read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);
      }

    temp_CF = (op1_64 >> index) & 0x01;

    // old code not as efficient???

    op1_64 &= ~(((Bit64u) 1) << index);  /* clear out bit */
    op1_64 |= (((Bit64u) !temp_CF) << index); /* set to complement */

    //op1_64 ^= (((Bit64u) 1) << index);  /* toggle bit  wrong??? */

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }
    set_CF(temp_CF);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, index_32, temp_CF;
    Bit32s displacement32;

    op2_32 = BX_READ_32BIT_REG(i->nnn());
    index_32 = op2_32 & 0x1f;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      op1_addr = 0; // keep compiler happy
      }
    else {
      displacement32 = ((Bit32s) (op2_32 & 0xffffffe0)) / 32;
      op1_addr = RMAddr(i) + 4 * displacement32;
      read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);
      }

    temp_CF = (op1_32 >> index_32) & 0x01;
    op1_32 &= ~(((Bit32u) 1) << index_32);  /* clear out bit */
    op1_32 |= (((Bit32u) !temp_CF) << index_32); /* set to complement */

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      write_RMW_virtual_dword(op1_32);
      }
    set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, index_16, temp_CF;
    Bit16s displacement16;

    op2_16 = BX_READ_16BIT_REG(i->nnn());
    index_16 = op2_16 & 0x0f;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      op1_addr = 0; // keep compiler happy
      }
    else {
      displacement16 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
      op1_addr = RMAddr(i) + 2 * displacement16;
      read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);
      }

    temp_CF = (op1_16 >> index_16) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << index_16);  /* clear out bit */
    op1_16 |= (((Bit16u) !temp_CF) << index_16); /* set to complement */

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }
    set_CF(temp_CF);
    }
#endif
}

  void
BX_CPU_C::BT_EvIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BT_EvIb: not available on <386"));
#else

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64;
    Bit8u  op2_8;

    op2_8 = i->Ib() & 0x3f;
    op2_8 %= 64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    set_CF((op1_64 >> op2_8) & 0x01);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    set_CF((op1_32 >> op2_8) & 0x01);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16;
    Bit8u  op2_8;


    op2_8 = i->Ib();
    op2_8 %= 16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    set_CF((op1_16 >> op2_8) & 0x01);
    }
#endif
}

  void
BX_CPU_C::BTS_EvIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTS_EvIb: not available on <386"));
#else

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    temp_CF = (op1_64 >> op2_8) & 0x01;
    op1_64 |= (((Bit64u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }
    set_CF(temp_CF);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;
    op1_32 |= (((Bit32u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      write_RMW_virtual_dword(op1_32);
      }
    set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = i->Ib();
    op2_8 %= 16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 |= (((Bit16u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }
    set_CF(temp_CF);
    }
#endif
}

  void
BX_CPU_C::BTC_EvIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTC_EvIb: not available on <386"));
#else

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    temp_CF = (op1_64 >> op2_8) & 0x01;

    op1_64 &= ~(((Bit64u) 1) << op2_8);  /* clear out bit */
    op1_64 |= (((Bit64u) !temp_CF) << op2_8); /* set to complement */
    //op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */


    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }
    set_CF(temp_CF);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;

    op1_32 &= ~(((Bit32u) 1) << op2_8);  /* clear out bit */
    op1_32 |= (((Bit32u) !temp_CF) << op2_8); /* set to complement */

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      write_RMW_virtual_dword(op1_32);
      }
    set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = i->Ib();
    op2_8 %= 16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << op2_8);  /* clear out bit */
    op1_16 |= (((Bit16u) !temp_CF) << op2_8); /* set to complement */

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }
    set_CF(temp_CF);
    }
#endif
}

  void
BX_CPU_C::BTR_EvIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("BTR_EvIb: not available on <386"));
#else

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    /* for 64 bit operand size mode */
    Bit64u op1_64, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    temp_CF = (op1_64 >> op2_8) & 0x01;
    op1_64 &= ~(((Bit64u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }
    set_CF(temp_CF);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->os32L()) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = i->Ib();
    op2_8 %= 32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;
    op1_32 &= ~(((Bit32u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      write_RMW_virtual_dword(op1_32);
      }
    set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = i->Ib();
    op2_8 %= 16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << op2_8);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }
    set_CF(temp_CF);
    }
#endif
}
