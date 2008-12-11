/////////////////////////////////////////////////////////////////////////
// $Id: load32.cc,v 1.3 2008-12-11 21:19:38 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP8L = read_virtual_byte(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb_Resolve16BaseIndex(bxInstruction_c *i)
{
  Bit16u offset = BX_READ_16BIT_REG(i->sibBase()) + BX_READ_16BIT_REG(i->sibIndex()) + (Bit16s) i->displ16u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 1, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        TMP8L = *hostAddr;
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 1, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 1, CPL, BX_READ, (Bit8u*) &TMP8L);
      }
      else {
        access_read_linear(laddr, 1, CPL, BX_READ, (void *) &TMP8L);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Eb(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 1))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb_Resolve32Base(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 1, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        TMP8L = *hostAddr;
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 1, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 1, CPL, BX_READ, (Bit8u*) &TMP8L);
      }
      else {
        access_read_linear(laddr, 1, CPL, BX_READ, (void *) &TMP8L);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Eb(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 1))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb_Resolve32BaseIndex(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 1, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        TMP8L = *hostAddr;
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 1, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 1, CPL, BX_READ, (Bit8u*) &TMP8L);
      }
      else {
        access_read_linear(laddr, 1, CPL, BX_READ, (void *) &TMP8L);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Eb(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 1))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP16 = read_virtual_word(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew_Resolve16BaseIndex(bxInstruction_c *i)
{
  Bit16u offset = BX_READ_16BIT_REG(i->sibBase()) + BX_READ_16BIT_REG(i->sibIndex()) + (Bit16s) i->displ16u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 2, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
        ReadHostWordFromLittleEndian(hostAddr, TMP16);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 2, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 2, CPL, BX_READ, (Bit8u*) &TMP16);
      }
      else {
        access_read_linear(laddr, 2, CPL, BX_READ, (void *) &TMP16);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ew(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 2))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew_Resolve32Base(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 2, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
        ReadHostWordFromLittleEndian(hostAddr, TMP16);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 2, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 2, CPL, BX_READ, (Bit8u*) &TMP16);
      }
      else {
        access_read_linear(laddr, 2, CPL, BX_READ, (void *) &TMP16);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ew(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 2))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew_Resolve32BaseIndex(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 2, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
        ReadHostWordFromLittleEndian(hostAddr, TMP16);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 2, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 2, CPL, BX_READ, (Bit8u*) &TMP16);
      }
      else {
        access_read_linear(laddr, 2, CPL, BX_READ, (void *) &TMP16);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ew(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 2))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP32 = read_virtual_dword(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed_Resolve16BaseIndex(bxInstruction_c *i)
{
  Bit16u offset = BX_READ_16BIT_REG(i->sibBase()) + BX_READ_16BIT_REG(i->sibIndex()) + (Bit16s) i->displ16u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 4, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
        ReadHostDWordFromLittleEndian(hostAddr, TMP32);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 4, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 4, CPL, BX_READ, (Bit8u*) &TMP32);
      }
      else {
        access_read_linear(laddr, 4, CPL, BX_READ, (void *) &TMP32);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ed(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 4))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed_Resolve32Base(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 4, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && ! (tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
        ReadHostDWordFromLittleEndian(hostAddr, TMP32);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 4, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
            tlbEntry->ppf | pageOffset, 4, CPL, BX_READ, (Bit8u*) &TMP32);
      }
      else {
        access_read_linear(laddr, 4, CPL, BX_READ, (void *) &TMP32);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ed(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 4))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed_Resolve32BaseIndex(bxInstruction_c *i)
{
  Bit32u offset = BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  Bit32u laddr;

  unsigned s = i->seg();
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];
  BX_INSTR_MEM_DATA_ACCESS(BX_CPU_ID, s, offset, 4, BX_READ);

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  if (seg->cache.valid & SegAccessROK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = BX_CPU_THIS_PTR get_laddr32(s, offset);
      unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
      Bit32u lpf = LPFOf(laddr);
      bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
      if (tlbEntry->lpf == lpf && !(tlbEntry->accessBits & USER_PL)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
        Bit32u pageOffset = PAGE_OFFSET(laddr);
        Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
        ReadHostDWordFromLittleEndian(hostAddr, TMP32);
        BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, tlbEntry->ppf | pageOffset, 4, BX_READ);
        BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
              tlbEntry->ppf | pageOffset, 4, CPL, BX_READ, (Bit8u*) &TMP32);
      }
      else {
        access_read_linear(laddr, 4, CPL, BX_READ, (void *) &TMP32);
      }

      BX_CPU_CALL_METHOD(i->execute2, (i));
      return;
    }
    else {
      BX_ERROR(("LOAD_Ed(): segment limit violation"));
      exception(int_number(s), 0, 0);
    }
  }

  if (!read_virtual_checks(seg, offset, 4))
    exception(int_number(s), 0, 0);
  goto accessOK;
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP64 = read_virtual_qword_64(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}
#endif

#if BX_SUPPORT_SSE >= 1

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wdq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *)(&BX_READ_XMM_REG(BX_TMP_REGISTER)));
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wss(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_DWORD(BX_TMP_REGISTER, val_32);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wsd(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_QWORD(BX_TMP_REGISTER, val_64);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

#endif

