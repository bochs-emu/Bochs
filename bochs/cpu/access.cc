/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2005-2014  The Bochs Project
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

bx_address bx_asize_mask[] = {
  0xffff,                         // as16 (asize = '00)
  0xffffffff,                     // as32 (asize = '01)
#if BX_SUPPORT_X86_64
  BX_CONST64(0xffffffffffffffff), // as64 (asize = '10)
  BX_CONST64(0xffffffffffffffff)  // as64 (asize = '11)
#endif
};

#if BX_SUPPORT_EVEX
  #define BX_MAX_MEM_ACCESS_LENGTH 64
#else
  #if BX_SUPPORT_AVX
    #define BX_MAX_MEM_ACCESS_LENGTH 32
  #else
    #define BX_MAX_MEM_ACCESS_LENGTH 16
  #endif
#endif

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned length)
{
  Bit32u upper_limit;

  if (seg->cache.valid==0) {
    BX_DEBUG(("write_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("write_virtual_checks(): segment not present"));
    return 0;
  }

  length--;

  switch (seg->cache.type) {
    case 0: case 1:   // read only
    case 4: case 5:   // read only, expand down
    case 8: case 9:   // execute only
    case 10: case 11: // execute/read
    case 12: case 13: // execute only, conforming
    case 14: case 15: // execute/read-only, conforming
      BX_ERROR(("write_virtual_checks(): no write access to seg"));
      return 0;

    case 2: case 3: /* read/write */
      if (offset > (seg->cache.u.segment.limit_scaled - length)
          || length > seg->cache.u.segment.limit_scaled)
      {
        BX_ERROR(("write_virtual_checks(): write beyond limit, r/w"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= (BX_MAX_MEM_ACCESS_LENGTH-1)) {
        // Mark cache as being OK type for succeeding read/writes. The limit
        // checks still needs to be done though, but is more simple. We
        // could probably also optimize that out with a flag for the case
        // when limit is the maximum 32bit value. Limit should accomodate
        // at least a dword, since we subtract from it in the simple
        // limit check in other functions, and we don't want the value to roll.
        // Only normal segments (not expand down) are handled this way.
        seg->cache.valid |= SegAccessROK | SegAccessWOK;
      }
      break;

    case 6: case 7: /* read/write, expand down */
      if (seg->cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if (offset <= seg->cache.u.segment.limit_scaled ||
           offset > upper_limit || (upper_limit - offset) < length)
      {
        BX_ERROR(("write_virtual_checks(): write beyond limit, r/w ED"));
        return 0;
      }
      break;

    default:
      BX_PANIC(("write_virtual_checks(): unknown descriptor type=%d", seg->cache.type));
  }

  return 1;
}

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned length)
{
  Bit32u upper_limit;

  if (seg->cache.valid==0) {
    BX_DEBUG(("read_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("read_virtual_checks(): segment not present"));
    return 0;
  }

  length--;

  switch (seg->cache.type) {
    case 0: case 1: /* read only */
    case 2: case 3: /* read/write */
    case 10: case 11: /* execute/read */
    case 14: case 15: /* execute/read-only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length)
          || length > seg->cache.u.segment.limit_scaled)
      {
        BX_ERROR(("read_virtual_checks(): read beyond limit"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= (BX_MAX_MEM_ACCESS_LENGTH-1)) {
        // Mark cache as being OK type for succeeding reads. See notes for
        // write checks; similar code.
        seg->cache.valid |= SegAccessROK;
      }
      break;

    case 4: case 5: /* read only, expand down */
    case 6: case 7: /* read/write, expand down */
      if (seg->cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if (offset <= seg->cache.u.segment.limit_scaled ||
           offset > upper_limit || (upper_limit - offset) < length)
      {
        BX_ERROR(("read_virtual_checks(): read beyond limit ED"));
        return 0;
      }
      break;

    case 8: case 9: /* execute only */
    case 12: case 13: /* execute only, conforming */
      /* can't read or write an execute-only segment */
      BX_ERROR(("read_virtual_checks(): execute only"));
      return 0;

    default:
      BX_PANIC(("read_virtual_checks(): unknown descriptor type=%d", seg->cache.type));
  }

  return 1;
}

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::execute_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned length)
{
  Bit32u upper_limit;

  if (seg->cache.valid==0) {
    BX_DEBUG(("execute_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("execute_virtual_checks(): segment not present"));
    return 0;
  }

  length--;

  switch (seg->cache.type) {
    case 0: case 1: /* read only */
    case 2: case 3: /* read/write */
    case 10: case 11: /* execute/read */
    case 14: case 15: /* execute/read-only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length)
          || length > seg->cache.u.segment.limit_scaled)
      {
        BX_ERROR(("execute_virtual_checks(): read beyond limit"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= (BX_MAX_MEM_ACCESS_LENGTH-1)) {
        // Mark cache as being OK type for succeeding reads. See notes for
        // write checks; similar code.
        seg->cache.valid |= SegAccessROK;
      }
      break;

    case 8: case 9: /* execute only */
    case 12: case 13: /* execute only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length)
          || length > seg->cache.u.segment.limit_scaled)
      {
        BX_ERROR(("execute_virtual_checks(): read beyond limit execute only"));
        return 0;
      }
      break;
 
    case 4: case 5: /* read only, expand down */
    case 6: case 7: /* read/write, expand down */
      if (seg->cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if (offset <= seg->cache.u.segment.limit_scaled ||
           offset > upper_limit || (upper_limit - offset) < length)
      {
        BX_ERROR(("execute_virtual_checks(): read beyond limit ED"));
        return 0;
      }
      break;

    default:
      BX_PANIC(("execute_virtual_checks(): unknown descriptor type=%d", seg->cache.type));
  }

  return 1;
}

const char *BX_CPU_C::strseg(bx_segment_reg_t *seg)
{
  if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]) return("ES");
  else if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]) return("CS");
  else if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]) return("SS");
  else if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]) return("DS");
  else if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]) return("FS");
  else if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS]) return("GS");
  else {
    BX_PANIC(("undefined segment passed to strseg()!"));
    return("??");
  }
}

int BX_CPU_C::int_number(unsigned s)
{
  if (s == BX_SEG_REG_SS)
    return BX_SS_EXCEPTION;
  else
    return BX_GP_EXCEPTION;
}

  Bit8u BX_CPP_AttrRegparmN(1)
BX_CPU_C::system_read_byte(bx_address laddr)
{
  Bit8u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & 0x01) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      data = *hostAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 1, 0, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 1, 0, BX_READ, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

  Bit16u BX_CPP_AttrRegparmN(1)
BX_CPU_C::system_read_word(bx_address laddr)
{
  Bit16u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & 0x01) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      ReadHostWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 2, 0, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 2, 0, BX_READ, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::system_read_dword(bx_address laddr)
{
  Bit32u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & 0x01) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      ReadHostDWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 4, 0, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 4, 0, BX_READ, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

  Bit64u BX_CPP_AttrRegparmN(1)
BX_CPU_C::system_read_qword(bx_address laddr)
{
  Bit64u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & 0x01) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      ReadHostQWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 8, 0, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 8, 0, BX_READ, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::system_write_byte(bx_address laddr, Bit8u data)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit32u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & 0x04) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, 0, BX_WRITE, (Bit8u*) &data);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 1);
     *hostAddr = data;
      return;
    }
  }

  if (access_write_linear(laddr, 1, 0, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::system_write_word(bx_address laddr, Bit16u data)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
  Bit32u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & 0x04) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, 0, BX_WRITE, (Bit8u*) &data);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      WriteHostWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 2, 0, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::system_write_dword(bx_address laddr, Bit32u data)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
  Bit32u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & 0x04) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, 0, BX_WRITE, (Bit8u*) &data);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      WriteHostDWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 4, 0, 0x0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);
}

  Bit8u* BX_CPP_AttrRegparmN(2)
BX_CPU_C::v2h_read_byte(bx_address laddr, bx_bool user)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      return hostAddr;
    }
  }

  return 0;
}

  Bit8u* BX_CPP_AttrRegparmN(2)
BX_CPU_C::v2h_write_byte(bx_address laddr, bx_bool user)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf)
  {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(tlbEntry->ppf);
      return hostAddr;
    }
  }

  return 0;
}
