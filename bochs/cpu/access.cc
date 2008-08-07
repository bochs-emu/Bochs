/////////////////////////////////////////////////////////////////////////
// $Id: access.cc,v 1.117 2008-08-07 22:14:38 sshwarts Exp $
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned length)
{
  Bit32u upper_limit;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    // Mark cache as being OK type for succeeding reads/writes
    seg->cache.valid |= SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    return 1;
  }
#endif

  if (seg->cache.valid==0) {
    BX_DEBUG(("write_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("write_virtual_checks(): segment not present"));
    return 0;
  }

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
      if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
          || (length-1 > seg->cache.u.segment.limit_scaled))
      {
        BX_ERROR(("write_virtual_checks(): write beyond limit, r/w"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= 15) {
        // Mark cache as being OK type for succeeding read/writes. The limit
        // checks still needs to be done though, but is more simple. We
        // could probably also optimize that out with a flag for the case
        // when limit is the maximum 32bit value. Limit should accomodate
        // at least a dword, since we subtract from it in the simple
        // limit check in other functions, and we don't want the value to roll.
        // Only normal segments (not expand down) are handled this way.
        seg->cache.valid |= SegAccessROK | SegAccessWOK;
 
        if (seg->cache.u.segment.limit_scaled == 0xffffffff)
          seg->cache.valid |= SegAccessROK4G | SegAccessWOK4G;
      }
      break;

    case 6: case 7: /* read/write, expand down */
      if (seg->cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if ((offset <= seg->cache.u.segment.limit_scaled) ||
           (offset > upper_limit) || ((upper_limit - offset) < (length - 1)))
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

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    // Mark cache as being OK type for succeeding reads/writes
    seg->cache.valid |= SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    return 1;
  }
#endif

  if (seg->cache.valid==0) {
    BX_DEBUG(("read_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("read_virtual_checks(): segment not present"));
    return 0;
  }

  switch (seg->cache.type) {
    case 0: case 1: /* read only */
    case 2: case 3: /* read/write */
    case 10: case 11: /* execute/read */
    case 14: case 15: /* execute/read-only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
          || (length-1 > seg->cache.u.segment.limit_scaled))
      {
        BX_ERROR(("read_virtual_checks(): read beyond limit"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= 15) {
        // Mark cache as being OK type for succeeding reads. See notes for
        // write checks; similar code.
        seg->cache.valid |= SegAccessROK;
        if (seg->cache.u.segment.limit_scaled == 0xffffffff)
          seg->cache.valid |= SegAccessROK4G;
      }
      break;

    case 4: case 5: /* read only, expand down */
    case 6: case 7: /* read/write, expand down */
      if (seg->cache.u.segment.d_b)
        upper_limit = 0xffffffff;
      else
        upper_limit = 0x0000ffff;
      if ((offset <= seg->cache.u.segment.limit_scaled) ||
           (offset > upper_limit) || ((upper_limit - offset) < (length - 1)))
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

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    // Mark cache as being OK type for succeeding reads/writes
    seg->cache.valid |= SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    return 1;
  }
#endif

  if (seg->cache.valid==0) {
    BX_DEBUG(("execute_virtual_checks(): segment descriptor not valid"));
    return 0;
  }

  if (seg->cache.p == 0) { /* not present */
    BX_ERROR(("execute_virtual_checks(): segment not present"));
    return 0;
  }

  switch (seg->cache.type) {
    case 0: case 1: /* read only */
    case 2: case 3: /* read/write */
    case 10: case 11: /* execute/read */
    case 14: case 15: /* execute/read-only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
          || (length-1 > seg->cache.u.segment.limit_scaled))
      {
        BX_ERROR(("execute_virtual_checks(): read beyond limit"));
        return 0;
      }
      if (seg->cache.u.segment.limit_scaled >= 15) {
        // Mark cache as being OK type for succeeding reads. See notes for
        // write checks; similar code.
        seg->cache.valid |= SegAccessROK;
        if (seg->cache.u.segment.limit_scaled == 0xffffffff)
          seg->cache.valid |= SegAccessROK4G;
      }
      break;

    case 8: case 9: /* execute only */
    case 12: case 13: /* execute only, conforming */
      if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
          || (length-1 > seg->cache.u.segment.limit_scaled))
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
      if ((offset <= seg->cache.u.segment.limit_scaled) ||
           (offset > upper_limit) || ((upper_limit - offset) < (length - 1)))
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

#if BX_SupportGuest2HostTLB
  Bit8u* BX_CPP_AttrRegparmN(2)
BX_CPU_C::v2h_read_byte(bx_address laddr, bx_bool user)
{
  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (! (tlbEntry->accessBits & (0x4 | user))) { // Read this pl OK.
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
    if (! (tlbEntry->accessBits & (0x6 | user))) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
#if BX_SUPPORT_ICACHE
      pageWriteStampTable.decWriteStamp(tlbEntry->ppf);
#endif
      return hostAddr;
    }
  }

  return 0;
}
#endif   // BX_SupportGuest2HostTLB
