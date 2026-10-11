/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_MPX

unsigned BX_CPU_C::MPX_Enabled(void)
{
  if (BX_CPU_THIS_PTR cr4.get_OSXSAVE() && ((BX_CPU_THIS_PTR xcr0.get32() & BX_XCR0_BNDCFG_MASK) != 0) && (BX_CPU_THIS_PTR bndcfg[USER_PL] & 0x1) != 0)
    return BX_CPU_THIS_PTR bndcfg[USER_PL] & 0x3;
  else
    return 0;
}

// Near branch (CALL, RET, JMP and Jcc) without BND prefix initializes BND0-3 when MPX is
// enabled and BNDCFGx.BNDPRESERVE is clear. Assigned to execute1 by the decoder when MPX
// could be enabled at any CPL, the original branch handler is in execute2.
// BND0-3 are cleared lazily (see sync_bndregs), if the branch faults the request is
// cancelled using bndregs_clear_icount.
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxMpxBndRegsInitBranch(bxInstruction_c *i)
{
  sync_bndregs();

  if (/*i->lockRepUsedValue() != 2 && */(BX_CPU_THIS_PTR bndcfg[BX_CPU_THIS_PTR user_pl] & 0x3) == BX_BNDCFG_MPX_ENABLED) {
    BX_CPU_THIS_PTR bndregs_clear = true;
    BX_CPU_THIS_PTR bndregs_clear_icount = get_icount();
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMK_BndEd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->as16L()) {
      BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    BX_CPU_THIS_PTR bndl[i->dst()] = BX_READ_32BIT_REG(i->sibBase());
    Bit32u eaddr = BX_CPU_RESOLVE_ADDR_32(i);
    BX_CPU_THIS_PTR bndh[i->dst()] = (Bit64u)(~eaddr);
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMK_BndEq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->sibBase() == BX_64BIT_REG_RIP) {
      BX_ERROR(("%s: cannot be executed with RIP-relative addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    BX_CPU_THIS_PTR bndl[i->dst()] = BX_READ_64BIT_REG(i->sibBase());
    Bit64u eaddr = BX_CPU_RESOLVE_ADDR_64(i);
    BX_CPU_THIS_PTR bndh[i->dst()] = ~eaddr;
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCL_BndEd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u bound = BX_CPU_THIS_PTR bndl[i->dst()];
    Bit32u val32;

    if (i->modC0()) {
      val32 = BX_READ_32BIT_REG(i->src());
    }
    else {
      if (i->as16L()) {
        BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
        exception(BX_UD_EXCEPTION, 0);
      }

      val32 = BX_CPU_RESOLVE_ADDR_32(i);
    }

    if (val32 < bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCL_BndEq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u bound = BX_CPU_THIS_PTR bndl[i->dst()];
    Bit64u val64;

    if (i->modC0()) {
      val64 = BX_READ_64BIT_REG(i->src());
    }
    else {
      val64 = BX_CPU_RESOLVE_ADDR_64(i);
    }

    if (val64 < bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCU_BndEd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit32u bound = (Bit32u) BX_CPU_THIS_PTR bndh[i->dst()];
    Bit32u val32;

    if (i->modC0()) {
      val32 = BX_READ_32BIT_REG(i->src());
    }
    else {
      if (i->as16L()) {
        BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
        exception(BX_UD_EXCEPTION, 0);
      }

      val32 = BX_CPU_RESOLVE_ADDR_32(i);
    }

    if (val32 > ~bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCU_BndEq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u bound = BX_CPU_THIS_PTR bndh[i->dst()];
    Bit64u val64;

    if (i->modC0()) {
      val64 = BX_READ_64BIT_REG(i->src());
    }
    else {
      val64 = BX_CPU_RESOLVE_ADDR_64(i);
    }

    if (val64 > ~bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCN_BndEd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u bound = BX_CPU_THIS_PTR bndh[i->dst()];
    Bit32u val32;

    if (i->modC0()) {
      val32 = BX_READ_32BIT_REG(i->src());
    }
    else {
      if (i->as16L()) {
        BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
        exception(BX_UD_EXCEPTION, 0);
      }

      val32 = BX_CPU_RESOLVE_ADDR_32(i);
    }

    if (val32 > bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDCN_BndEq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u bound = BX_CPU_THIS_PTR bndh[i->dst()];
    Bit64u val64;

    if (i->modC0()) {
      val64 = BX_READ_64BIT_REG(i->src());
    }
    else {
      val64 = BX_CPU_RESOLVE_ADDR_64(i);
    }

    if (val64 > bound) {
      BX_CPU_THIS_PTR bndstatus = BX_MPX_BOUNDS_VIOLATION;
      exception(BX_BR_EXCEPTION, 0);
    }
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMOV_GBndEBndR(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }
    if (i->src() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->src()));
      exception(BX_UD_EXCEPTION, 0);
    }

    BX_CPU_THIS_PTR bndl[i->dst()] = BX_CPU_THIS_PTR bndl[i->src()];
    BX_CPU_THIS_PTR bndh[i->dst()] = BX_CPU_THIS_PTR bndh[i->src()];
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMOV_GBndMq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    bx_address eaddr = BX_CPU_RESOLVE_ADDR_32(i);

    Bit32u lo = read_virtual_dword_32(i->seg(), eaddr);
    Bit32u hi = read_virtual_dword_32(i->seg(), eaddr + 4);

    BX_CPU_THIS_PTR bndl[i->dst()] = lo;
    BX_CPU_THIS_PTR bndh[i->dst()] = hi;
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMOV_GBndMdq(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
    bx_address laddr = get_laddr64(i->seg(), eaddr);

    Bit64u lo = read_linear_qword(i->seg(), laddr);
    Bit64u hi = read_linear_qword(i->seg(), laddr + 8);

    BX_CPU_THIS_PTR bndl[i->dst()] = lo;
    BX_CPU_THIS_PTR bndh[i->dst()] = hi;
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMOV_MqGBnd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->src() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->src()));
      exception(BX_UD_EXCEPTION, 0);
    }

    bx_address eaddr = BX_CPU_RESOLVE_ADDR_32(i);

    write_virtual_dword_32(i->seg(), eaddr,     BX_CPU_THIS_PTR bndl[i->src()]);
    write_virtual_dword_32(i->seg(), eaddr + 4, BX_CPU_THIS_PTR bndh[i->src()]);
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDMOV_MdqGBnd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->src() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->src()));
      exception(BX_UD_EXCEPTION, 0);
    }

    bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
    bx_address laddr = get_laddr64(i->seg(), eaddr);

    write_linear_qword(i->seg(), laddr,     BX_CPU_THIS_PTR bndl[i->src()]);
    write_linear_qword(i->seg(), laddr + 8, BX_CPU_THIS_PTR bndh[i->src()]);
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDLDX32_BndM(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->as16L()) {
      BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit32u base = 0, ptr_val = 0;

    if (i->sibBase() != BX_NIL_REGISTER)
      base = BX_READ_32BIT_REG(i->sibBase());
    base += i->displ32s();
    base = get_laddr32(i->seg(), base);
    if (i->sibIndex() != 4)
      ptr_val = BX_READ_32BIT_REG(i->sibIndex());

    BX_DEBUG(("BASE: %08x", base));
    BX_DEBUG(("PTR:  %08x", ptr_val));

    Bit32u A_BDE = ((base >> 12) << 2) + (BX_CPU_THIS_PTR bndcfg[USER_PL] & ~0xfff);

    BX_DEBUG(("A_BDE:  %08x", A_BDE));

    Bit32u A_BT = read_linear_dword(BX_SEG_REG_DS, A_BDE);
    if ((A_BT & 0x1) == 0) {
      BX_CPU_THIS_PTR bndstatus = A_BDE | BX_MPX_INVALID_BD_ENTRY;
      exception(BX_BR_EXCEPTION, 0);
    }

    Bit32u A_BTE = ((base & 0xffc) << 2) + (A_BT & ~0x3);

    BX_DEBUG(("A_BTE:  %08x", A_BTE));

    Bit32u lb  = read_virtual_dword_32(BX_SEG_REG_DS, A_BTE);
    Bit32u ub  = read_virtual_dword_32(BX_SEG_REG_DS, A_BTE+4);
    Bit32u ptr = read_virtual_dword_32(BX_SEG_REG_DS, A_BTE+8);

    if (ptr != ptr_val)
      lb = ub = 0;

    BX_CPU_THIS_PTR bndl[i->dst()] = (Bit64u) lb;
    BX_CPU_THIS_PTR bndh[i->dst()] = (Bit64u) ub;
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDLDX64_BndM(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->dst() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->dst()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->sibBase() == BX_64BIT_REG_RIP) {
      BX_ERROR(("%s: cannot be executed with RIP-relative addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u base = 0, ptr_val = 0;

    if (i->sibBase() != BX_NIL_REGISTER)
      base = BX_READ_64BIT_REG(i->sibBase());
    base += (Bit64s) i->displ32s();
    base = get_laddr64(i->seg(), base);
    if (i->sibIndex() != 4)
      ptr_val = BX_READ_64BIT_REG(i->sibIndex());

    base &= (BX_CPU_THIS_PTR linaddr_width == 57) ? BX_CONST64(0x1ffffffffffffff) : BX_CONST64(0xffffffffffff);

    BX_DEBUG(("BASE: %08x_%08x", GET32H(base), GET32L(base)));
    BX_DEBUG(("PTR:  %08x_%08x", GET32H(ptr_val), GET32L(ptr_val)));

    Bit64u A_BDE = ((base >> 20) << 3) + (BX_CPU_THIS_PTR bndcfg[USER_PL] & ~BX_CONST64(0xfff));

    BX_DEBUG(("A_BDE: %08x_%08x", GET32H(A_BDE), GET32L(A_BDE)));

    Bit64u A_BT = read_linear_qword(BX_SEG_REG_DS, A_BDE);
    if ((A_BT & 0x1) == 0) {
      BX_CPU_THIS_PTR bndstatus = A_BDE | BX_MPX_INVALID_BD_ENTRY;
      exception(BX_BR_EXCEPTION, 0);
    }

    Bit64u A_BTE = ((base & 0xffff8) << 2) + (A_BT & ~BX_CONST64(0x7));

    BX_DEBUG(("A_BTE: %08x_%08x", GET32H(A_BTE), GET32L(A_BTE)));

    Bit64u lb  = read_linear_qword(BX_SEG_REG_DS, A_BTE);
    Bit64u ub  = read_linear_qword(BX_SEG_REG_DS, A_BTE+8);
    Bit64u ptr = read_linear_qword(BX_SEG_REG_DS, A_BTE+16);

    if (ptr != ptr_val)
      lb = ub = 0;

    BX_CPU_THIS_PTR bndl[i->dst()] = lb;
    BX_CPU_THIS_PTR bndh[i->dst()] = ub;
  }

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDSTX32_MGBnd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->src() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->src()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->as16L()) {
      BX_ERROR(("%s: cannot be executed with 16-bit addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit32u base = 0, ptr_val = 0;

    if (i->sibBase() != BX_NIL_REGISTER)
      base = BX_READ_32BIT_REG(i->sibBase());
    base += i->displ32s();
    base = get_laddr32(i->seg(), base);
    if (i->sibIndex() != 4)
      ptr_val = BX_READ_32BIT_REG(i->sibIndex());

    BX_DEBUG(("BASE: %08x", base));
    BX_DEBUG(("PTR:  %08x", ptr_val));

    Bit32u A_BDE = ((base >> 12) << 2) + (BX_CPU_THIS_PTR bndcfg[USER_PL] & ~0xfff);

    BX_DEBUG(("A_BDE:  %08x", A_BDE));

    Bit32u A_BT = read_linear_dword(BX_SEG_REG_DS, A_BDE);
    if ((A_BT & 0x1) == 0) {
      BX_CPU_THIS_PTR bndstatus = A_BDE | BX_MPX_INVALID_BD_ENTRY;
      exception(BX_BR_EXCEPTION, 0);
    }

    Bit32u A_BTE = ((base & 0xffc) << 2) + (A_BT & ~0x3);

    BX_DEBUG(("A_BTE:  %08x", A_BTE));

    write_virtual_dword_32(BX_SEG_REG_DS, A_BTE,     BX_CPU_THIS_PTR bndl[i->src()]);
    write_virtual_dword_32(BX_SEG_REG_DS, A_BTE + 4, BX_CPU_THIS_PTR bndh[i->src()]);
    write_virtual_dword_32(BX_SEG_REG_DS, A_BTE + 8, ptr_val);
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BNDSTX64_MGBnd(bxInstruction_c *i)
{
  if (MPX_Enabled()) {
    sync_bndregs();

    if (i->src() > 3) {
      BX_ERROR(("%s: access to BND%d causes #UD", i->getIaOpcodeNameShort(), i->src()));
      exception(BX_UD_EXCEPTION, 0);
    }

    if (i->sibBase() == BX_64BIT_REG_RIP) {
      BX_ERROR(("%s: cannot be executed with RIP-relative addressing", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

    Bit64u base = 0, ptr_val = 0;

    if (i->sibBase() != BX_NIL_REGISTER)
      base = BX_READ_64BIT_REG(i->sibBase());
    base += (Bit64s) i->displ32s();
    base = get_laddr64(i->seg(), base);
    if (i->sibIndex() != 4)
      ptr_val = BX_READ_64BIT_REG(i->sibIndex());

    base &= (BX_CPU_THIS_PTR linaddr_width == 57) ? BX_CONST64(0x1ffffffffffffff) : BX_CONST64(0xffffffffffff);

    BX_DEBUG(("BASE: %08x_%08x", GET32H(base), GET32L(base)));
    BX_DEBUG(("PTR:  %08x_%08x", GET32H(ptr_val), GET32L(ptr_val)));

    Bit64u A_BDE = ((base >> 20) << 3) + (BX_CPU_THIS_PTR bndcfg[USER_PL] & ~BX_CONST64(0xfff));

    BX_DEBUG(("A_BDE: %08x_%08x", GET32H(A_BDE), GET32L(A_BDE)));

    Bit64u A_BT = read_linear_qword(BX_SEG_REG_DS, A_BDE);
    if ((A_BT & 0x1) == 0) {
      BX_CPU_THIS_PTR bndstatus = A_BDE | BX_MPX_INVALID_BD_ENTRY;
      exception(BX_BR_EXCEPTION, 0);
    }

    Bit64u A_BTE = ((base & 0xffff8) << 2) + (A_BT & ~BX_CONST64(0x7));

    BX_DEBUG(("A_BTE: %08x_%08x", GET32H(A_BTE), GET32L(A_BTE)));

    write_linear_qword(BX_SEG_REG_DS, A_BTE,      BX_CPU_THIS_PTR bndl[i->src()]);
    write_linear_qword(BX_SEG_REG_DS, A_BTE + 8,  BX_CPU_THIS_PTR bndh[i->src()]);
    write_linear_qword(BX_SEG_REG_DS, A_BTE + 16, ptr_val);
  }

  BX_NEXT_INSTR(i);
}
#endif

#endif
