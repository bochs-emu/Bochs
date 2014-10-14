/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012-2014 Stanislav Shwartsman
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

#include "cpustats.h"

void BX_CPP_AttrRegparmN(2) BX_CPU_C::stackPrefetch(bx_address offset, unsigned len)
{
  bx_address laddr;
  unsigned pageOffset;

  INC_STACK_PREFETCH_STAT(stackPrefetch);

  BX_CPU_THIS_PTR espHostPtr = 0; // initialize with NULL pointer
  BX_CPU_THIS_PTR espPageWindowSize = 0;

  len--;

#if BX_SUPPORT_X86_64
  if (long64_mode()) {
    laddr = offset;
    pageOffset = PAGE_OFFSET(offset);

    // canonical violations will miss the TLB below

    if (pageOffset + len >= 4096) // don't care for page split accesses
      return;

    BX_CPU_THIS_PTR espPageWindowSize = 4096;
  }
  else
#endif
  {
    laddr = get_laddr32(BX_SEG_REG_SS, (Bit32u) offset);
    pageOffset = PAGE_OFFSET(laddr);
    if (pageOffset + len >= 4096) // don't care for page split accesses
      return;

    Bit32u limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled;
    Bit32u pageStart = (Bit32u) offset - pageOffset;

    if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid) {
      BX_ERROR(("stackPrefetch: SS not valid"));
      exception(BX_SS_EXCEPTION, 0);
    }
      
    BX_ASSERT(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p);
    BX_ASSERT(IS_DATA_SEGMENT_WRITEABLE(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type));

    // check that the begining of the page is within stack segment limits
    // problem can happen with EXPAND DOWN segments
    if (IS_DATA_SEGMENT_EXPAND_DOWN(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type)) {
      Bit32u upper_limit;
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if (offset <= limit || offset > upper_limit || (upper_limit - offset) < len) {
        BX_ERROR(("stackPrefetch(%d): access [0x%08x] > SS.limit [0x%08x] ED", len+1, (Bit32u) offset, limit));
        exception(BX_SS_EXCEPTION, 0);
      }

      // check that the begining of the page is within stack segment limits
      // handle correctly the wrap corner case for EXPAND DOWN 
      Bit32u pageEnd = pageStart + 0xfff;
      if (pageStart > limit && pageStart < pageEnd) {
        BX_CPU_THIS_PTR espPageWindowSize = 4096;
        if ((upper_limit - offset) < (4096 - pageOffset))
          BX_CPU_THIS_PTR espPageWindowSize = (Bit32u)(upper_limit - offset + 1);
      }
    }
    else {
      if (offset > (limit - len) || len > limit) {
        BX_ERROR(("stackPrefetch(%d): access [0x%08x] > SS.limit [0x%08x]", len+1, (Bit32u) offset, limit));
        exception(BX_SS_EXCEPTION, 0);
      }

      if (pageStart <= limit) {
        BX_CPU_THIS_PTR espPageWindowSize = 4096;
        if ((limit - offset) < (4096 - pageOffset))
          BX_CPU_THIS_PTR espPageWindowSize = (Bit32u)(limit - offset + 1);
      }
    }
  }

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    // Assuming that we always can read if write access is OK
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      BX_CPU_THIS_PTR espPageBias = (bx_address) pageOffset - offset;
      BX_CPU_THIS_PTR pAddrStackPage = tlbEntry->ppf;
      BX_CPU_THIS_PTR espHostPtr = (Bit8u*) tlbEntry->hostPageAddr;
    }
  }

  if (! BX_CPU_THIS_PTR espHostPtr || BX_CPU_THIS_PTR espPageWindowSize < 7)
    BX_CPU_THIS_PTR espPageWindowSize = 0;
  else
    BX_CPU_THIS_PTR espPageWindowSize -= 7;
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::stack_write_byte(bx_address offset, Bit8u data)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 1);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit8u *hostPageAddr = (Bit8u*)(BX_CPU_THIS_PTR espHostPtr + espBiased);
    bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset), pAddr, 1, CPL, BX_WRITE, (Bit8u*) &data);
    pageWriteStampTable.decWriteStamp(pAddr, 1);
    *hostPageAddr = data;
  }
  else {
    write_virtual_byte(BX_SEG_REG_SS, offset, data);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::stack_write_word(bx_address offset, Bit16u data)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 2);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit16u *hostPageAddr = (Bit16u*)(BX_CPU_THIS_PTR espHostPtr + espBiased);
    bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check() && (pAddr & 1) != 0) {
      BX_ERROR(("stack_write_word(): #AC misaligned access"));
      exception(BX_AC_EXCEPTION, 0);
    }
#endif
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset), pAddr, 2, CPL, BX_WRITE, (Bit8u*) &data);
    pageWriteStampTable.decWriteStamp(pAddr, 2);
    WriteHostWordToLittleEndian(hostPageAddr, data);
  }
  else {
    write_virtual_word(BX_SEG_REG_SS, offset, data);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::stack_write_dword(bx_address offset, Bit32u data)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 4);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit32u *hostPageAddr = (Bit32u*)(BX_CPU_THIS_PTR espHostPtr + espBiased);
    bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check() && (pAddr & 3) != 0) {
      BX_ERROR(("stack_write_dword(): #AC misaligned access"));
      exception(BX_AC_EXCEPTION, 0);
    }
#endif
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset), pAddr, 4, CPL, BX_WRITE, (Bit8u*) &data);
    pageWriteStampTable.decWriteStamp(pAddr, 4);
    WriteHostDWordToLittleEndian(hostPageAddr, data);
  }
  else {
    write_virtual_dword(BX_SEG_REG_SS, offset, data);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::stack_write_qword(bx_address offset, Bit64u data)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 8);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit64u *hostPageAddr = (Bit64u*)(BX_CPU_THIS_PTR espHostPtr + espBiased);
    bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check() && (pAddr & 7) != 0) {
      BX_ERROR(("stack_write_qword(): #AC misaligned access"));
      exception(BX_AC_EXCEPTION, 0);
    }
#endif
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset), pAddr, 8, CPL, BX_WRITE, (Bit8u*) &data);
    pageWriteStampTable.decWriteStamp(pAddr, 8);
    WriteHostQWordToLittleEndian(hostPageAddr, data);
  }
  else {
    write_virtual_qword(BX_SEG_REG_SS, offset, data);
  }
}

Bit8u BX_CPP_AttrRegparmN(1) BX_CPU_C::stack_read_byte(bx_address offset)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 1);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit8u *hostPageAddr = (Bit8u*)(BX_CPU_THIS_PTR espHostPtr + espBiased), data;
    data = *hostPageAddr;
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset),
        (BX_CPU_THIS_PTR pAddrStackPage + espBiased), 1, CPL, BX_READ, (Bit8u*) &data);
    return data;
  }
  else {
    return read_virtual_byte(BX_SEG_REG_SS, offset);
  }
}

Bit16u BX_CPP_AttrRegparmN(1) BX_CPU_C::stack_read_word(bx_address offset)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 2);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit16u *hostPageAddr = (Bit16u*)(BX_CPU_THIS_PTR espHostPtr + espBiased), data;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
      if (pAddr & 1) {
        BX_ERROR(("stack_read_word(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif
    ReadHostWordFromLittleEndian(hostPageAddr, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset),
        (BX_CPU_THIS_PTR pAddrStackPage + espBiased), 2, CPL, BX_READ, (Bit8u*) &data);
    return data;
  }
  else {
    return read_virtual_word(BX_SEG_REG_SS, offset);
  }
}

Bit32u BX_CPP_AttrRegparmN(1) BX_CPU_C::stack_read_dword(bx_address offset)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 4);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit32u *hostPageAddr = (Bit32u*)(BX_CPU_THIS_PTR espHostPtr + espBiased), data;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
      if (pAddr & 3) {
        BX_ERROR(("stack_read_dword(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif
    ReadHostDWordFromLittleEndian(hostPageAddr, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset),
        (BX_CPU_THIS_PTR pAddrStackPage + espBiased), 4, CPL, BX_READ, (Bit8u*) &data);
    return data;
  }
  else {
    return read_virtual_dword(BX_SEG_REG_SS, offset);
  }
}

Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::stack_read_qword(bx_address offset)
{
  bx_address espBiased = offset + BX_CPU_THIS_PTR espPageBias;

  if (espBiased >= BX_CPU_THIS_PTR espPageWindowSize) {
    stackPrefetch(offset, 8);
    espBiased = offset + BX_CPU_THIS_PTR espPageBias;
  }

  if (BX_CPU_THIS_PTR espHostPtr) {
    Bit64u *hostPageAddr = (Bit64u*)(BX_CPU_THIS_PTR espHostPtr + espBiased), data;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    if (BX_CPU_THIS_PTR alignment_check()) {
      bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrStackPage + espBiased;
      if (pAddr & 7) {
        BX_ERROR(("stack_read_qword(): #AC misaligned access"));
        exception(BX_AC_EXCEPTION, 0);
      }
    }
#endif
    ReadHostQWordFromLittleEndian(hostPageAddr, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(get_laddr(BX_SEG_REG_SS, offset),
        (BX_CPU_THIS_PTR pAddrStackPage + espBiased), 8, CPL, BX_READ, (Bit8u*) &data);
    return data;
  }
  else {
    return read_virtual_qword(BX_SEG_REG_SS, offset);
  }
}
