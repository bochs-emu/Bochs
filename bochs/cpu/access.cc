/////////////////////////////////////////////////////////////////////////
// $Id: access.cc,v 1.36 2003-02-13 15:03:56 sshwarts Exp $
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

#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif

#if BX_SUPPORT_X86_64
#define IsLongMode() (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
#define LPFOf(laddr) ((laddr) & BX_CONST64(0xfffffffffffff000))
#else
#define IsLongMode() (0)
#define LPFOf(laddr) ((laddr) & 0xfffff000)
#endif



  void
BX_CPU_C::write_virtual_checks(bx_segment_reg_t *seg, bx_address offset,
                               unsigned length)
{
  Bit32u upper_limit;


#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    seg->cache.valid |= SegAccessWOK;
    return;
    }
#endif
  if ( protected_mode() ) {
    if ( seg->cache.valid==0 ) {
      BX_ERROR(("seg = %s", BX_CPU_THIS_PTR strseg(seg)));
      BX_ERROR(("seg->selector.value = %04x", (unsigned) seg->selector.value));
      BX_ERROR(("write_virtual_checks: valid bit = 0"));
      BX_ERROR(("CS: %04x", (unsigned) BX_CPU_THIS_PTR sregs[1].selector.value));
      BX_ERROR(("IP: %04x", (unsigned) BX_CPU_THIS_PTR prev_eip));
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (seg->cache.p == 0) { /* not present */
      BX_INFO(("write_virtual_checks(): segment not present"));
      exception(int_number(seg), 0, 0);
      return;
      }
    switch ( seg->cache.type ) {
      case 0: case 1:   // read only
      case 4: case 5:   // read only, expand down
      case 8: case 9:   // execute only
      case 10: case 11: // execute/read
      case 12: case 13: // execute only, conforming
      case 14: case 15: // execute/read-only, conforming
        BX_INFO(("write_virtual_checks(): no write access to seg"));
        exception(int_number(seg), 0, 0);
        return;

      case 2: case 3: /* read/write */

        if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
            || (length-1 > seg->cache.u.segment.limit_scaled)) {
                  BX_INFO(("write_virtual_checks(): write beyond limit, r/w"));
          exception(int_number(seg), 0, 0);
          return;
          }
        if (seg->cache.u.segment.limit_scaled >= 7) {
          // Mark cache as being OK type for succeeding writes.  The limit
          // checks still needs to be done though, but is more simple.  We
          // could probably also optimize that out with a flag for the case
          // when limit is the maximum 32bit value.  Limit should accomodate
          // at least a dword, since we subtract from it in the simple
          // limit check in other functions, and we don't want the value to roll.
          // Only normal segments (not expand down) are handled this way.
          seg->cache.valid |= SegAccessWOK;
          }
        break;

      case 6: case 7: /* read write, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
                  BX_INFO(("write_virtual_checks(): write beyond limit, r/w ED"));
          exception(int_number(seg), 0, 0);
          return;
          }
        break;
      }

    return;
    }

  else { /* real mode */
      if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
          || (length-1 > seg->cache.u.segment.limit_scaled)) {
      //BX_INFO(("write_virtual_checks() SEG EXCEPTION:  %x:%x + %x",
      //  (unsigned) seg->selector.value, (unsigned) offset, (unsigned) length));
      if (seg == & BX_CPU_THIS_PTR sregs[2]) exception(BX_SS_EXCEPTION, 0, 0);
      else exception(BX_GP_EXCEPTION, 0, 0);
      }
    if (seg->cache.u.segment.limit_scaled >= 7) {
      // Mark cache as being OK type for succeeding writes.  See notes above.
      seg->cache.valid |= SegAccessWOK;
      }
    }
}

  void
BX_CPU_C::read_virtual_checks(bx_segment_reg_t *seg, bx_address offset,
                              unsigned length)
{
  Bit32u upper_limit;


#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    seg->cache.valid |= SegAccessROK;
    return;
    }
#endif
  if ( protected_mode() ) {
    if ( seg->cache.valid==0 ) {
      BX_ERROR(("seg = %s", BX_CPU_THIS_PTR strseg(seg)));
      BX_ERROR(("seg->selector.value = %04x", (unsigned) seg->selector.value));
      //BX_ERROR(("read_virtual_checks: valid bit = 0"));
      //BX_ERROR(("CS: %04x", (unsigned)
      //   BX_CPU_THIS_PTR sregs[1].selector.value));
      //BX_ERROR(("IP: %04x", (unsigned) BX_CPU_THIS_PTR prev_eip));
      //debug(EIP);
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (seg->cache.p == 0) { /* not present */
          BX_INFO(("read_virtual_checks(): segment not present"));
      exception(int_number(seg), 0, 0);
      return;
      }

    switch ( seg->cache.type ) {
      case 0: case 1: /* read only */
      case 10: case 11: /* execute/read */
      case 14: case 15: /* execute/read-only, conforming */
        if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
            || (length-1 > seg->cache.u.segment.limit_scaled)) {
                  BX_INFO(("read_virtual_checks(): write beyond limit"));
          exception(int_number(seg), 0, 0);
          return;
          }
        if (seg->cache.u.segment.limit_scaled >= 7) {
          // Mark cache as being OK type for succeeding writes.  See notes for
          // write checks; similar code.
          seg->cache.valid |= SegAccessROK;
          }
        break;

      case 2: case 3: /* read/write */
        if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
            || (length-1 > seg->cache.u.segment.limit_scaled)) {
                  BX_INFO(("read_virtual_checks(): write beyond limit"));
          exception(int_number(seg), 0, 0);
          return;
          }
        if (seg->cache.u.segment.limit_scaled >= 7) {
          // Mark cache as being OK type for succeeding writes.  See notes for
          // write checks; similar code.
          seg->cache.valid |= SegAccessROK;
          }
        break;

      case 4: case 5: /* read only, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
                  BX_INFO(("read_virtual_checks(): write beyond limit"));
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 6: case 7: /* read write, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
                  BX_INFO(("read_virtual_checks(): write beyond limit"));
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 8: case 9: /* execute only */
      case 12: case 13: /* execute only, conforming */
        /* can't read or write an execute-only segment */
                BX_INFO(("read_virtual_checks(): execute only"));
        exception(int_number(seg), 0, 0);
        return;
        break;
      }
    return;
    }

  else { /* real mode */
    if (offset > (seg->cache.u.segment.limit_scaled - length + 1)
        || (length-1 > seg->cache.u.segment.limit_scaled)) {
      //BX_ERROR(("read_virtual_checks() SEG EXCEPTION:  %x:%x + %x",
      //  (unsigned) seg->selector.value, (unsigned) offset, (unsigned) length));
      if (seg == & BX_CPU_THIS_PTR sregs[2]) exception(BX_SS_EXCEPTION, 0, 0);
      else exception(BX_GP_EXCEPTION, 0, 0);
      }
    if (seg->cache.u.segment.limit_scaled >= 7) {
      // Mark cache as being OK type for succeeding writes.  See notes for
      // write checks; similar code.
      seg->cache.valid |= SegAccessROK;
      }
    return;
    }
}




  char *
BX_CPU_C::strseg(bx_segment_reg_t *seg)
{
  if (seg == &BX_CPU_THIS_PTR sregs[0]) return("ES");
  else if (seg == & BX_CPU_THIS_PTR sregs[1]) return("CS");
  else if (seg == & BX_CPU_THIS_PTR sregs[2]) return("SS");
  else if (seg == &BX_CPU_THIS_PTR sregs[3]) return("DS");
  else if (seg == &BX_CPU_THIS_PTR sregs[4]) return("FS");
  else if (seg == &BX_CPU_THIS_PTR sregs[5]) return("GS");
  else {
    BX_ERROR(("undefined segment passed to strseg()!"));
    return("??");
    }
}


  void
BX_CPU_C::write_virtual_byte(unsigned s, bx_address offset, Bit8u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset <= seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 1, BX_WRITE);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      tlbIndex = BX_TLB_INDEX_OF(laddr);
      lpf = LPFOf(laddr);
      if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf)))
        {
        Bit32u accessBits;
        Bit32u hostPageAddr;
        Bit8u *hostAddr;

        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
        hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
        if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
          Bit32u *pageStamp;
          pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
              BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
          // Current write access has privilege.
          if (hostPageAddr
#if BX_SupportICache
           && (*pageStamp & ICacheWriteStampMask)
#endif
              ) {
            *hostAddr = *data;
#if BX_SupportICache
            (*pageStamp)--;
#endif
            return;
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 1, pl, BX_WRITE, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 1);
  goto accessOK;
}

  void
BX_CPU_C::write_virtual_word(unsigned s, bx_address offset, Bit16u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset < seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 2, BX_WRITE);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffe) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit16u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
              && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              WriteHostWordToLittleEndian(hostAddr, *data);
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 2, pl, BX_WRITE, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 2);
  goto accessOK;
}

  void
BX_CPU_C::write_virtual_dword(unsigned s, bx_address offset, Bit32u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset < (seg->cache.u.segment.limit_scaled-2)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 4, BX_WRITE);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffc) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit32u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
             && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              WriteHostDWordToLittleEndian(hostAddr, *data);
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 4, pl, BX_WRITE, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 4);
  goto accessOK;
}


  void
BX_CPU_C::read_virtual_byte(unsigned s, bx_address offset, Bit8u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessROK) {
    if ( IsLongMode() || (offset <= seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 1, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      tlbIndex = BX_TLB_INDEX_OF(laddr);
      lpf = LPFOf(laddr);
      if (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf)) {
        // See if the TLB entry privilege level allows us read access
        // from this CPL.
        Bit32u accessBits;
        Bit32u hostPageAddr;
        Bit8u *hostAddr;

        hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
        hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
        if ( accessBits & (1<<pl) ) { // Read this pl OK.
          if (hostPageAddr) {
            *data = *hostAddr;
            return;
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 1, pl, BX_READ, (void *) data);
      return;
      }
    }
  read_virtual_checks(seg, offset, 1);
  goto accessOK;
}


  void
BX_CPU_C::read_virtual_word(unsigned s, bx_address offset, Bit16u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessROK) {
    if ( IsLongMode() || (offset < seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 2, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffe) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf)) {
          // See if the TLB entry privilege level allows us read access
          // from this CPL.
          Bit32u accessBits;
          Bit32u hostPageAddr;
          Bit16u *hostAddr;

          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1<<pl) ) { // Read this pl OK.
            if (hostPageAddr) {
              ReadHostWordFromLittleEndian(hostAddr, *data);
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 2, pl, BX_READ, (void *) data);
      return;
      }
    }
  read_virtual_checks(seg, offset, 2);
  goto accessOK;
}


  void
BX_CPU_C::read_virtual_dword(unsigned s, bx_address offset, Bit32u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessROK) {
    if ( IsLongMode() || (offset < (seg->cache.u.segment.limit_scaled-2)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 4, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffc) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf)) {
          // See if the TLB entry privilege level allows us read access
          // from this CPL.
          Bit32u accessBits;
          Bit32u hostPageAddr;
          Bit32u *hostAddr;

          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1<<pl) ) { // Read this pl OK.
            if (hostPageAddr) {
              ReadHostDWordFromLittleEndian(hostAddr, *data);
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 4, pl, BX_READ, (void *) data);
      return;
      }
    }
  read_virtual_checks(seg, offset, 4);
  goto accessOK;
}


//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  void
BX_CPU_C::read_RMW_virtual_byte(unsigned s, bx_address offset, Bit8u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset <= seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 1, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      tlbIndex = BX_TLB_INDEX_OF(laddr);
      lpf = LPFOf(laddr);
      if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
           ) {
        Bit32u accessBits;
        Bit32u hostPageAddr;
        Bit8u *hostAddr;

        // See if the TLB entry privilege level allows us write access
        // from this CPL.
        hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
        hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
        accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
        if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
          Bit32u *pageStamp;
          pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
              BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
          // Current write access has privilege.
          if (hostPageAddr
#if BX_SupportICache
            && (*pageStamp & ICacheWriteStampMask)
#endif
              ) {
            *data = *hostAddr;
            BX_CPU_THIS_PTR address_xlation.pages = (Bit32u) hostAddr;
#if BX_SupportICache
            (*pageStamp)--;
#endif
            return;
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      // Accelerated attempt falls through to long path.  Do it the
      // old fashioned way...
      access_linear(laddr, 1, pl, BX_RW, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 1);
  goto accessOK;
}


  void
BX_CPU_C::read_RMW_virtual_word(unsigned s, bx_address offset, Bit16u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset < seg->cache.u.segment.limit_scaled) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 2, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffe) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit16u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
             && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              ReadHostWordFromLittleEndian(hostAddr, *data);
              BX_CPU_THIS_PTR address_xlation.pages = (Bit32u) hostAddr;
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 2, pl, BX_RW, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 2);
  goto accessOK;
}

  void
BX_CPU_C::read_RMW_virtual_dword(unsigned s, bx_address offset, Bit32u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset < (seg->cache.u.segment.limit_scaled-2)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 4, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xffc) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit32u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
             && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              ReadHostDWordFromLittleEndian(hostAddr, *data);
              BX_CPU_THIS_PTR address_xlation.pages = (Bit32u) hostAddr;
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 4, pl, BX_RW, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 4);
  goto accessOK;
}

  void
BX_CPU_C::write_RMW_virtual_byte(Bit8u val8)
{
  BX_INSTR_MEM_DATA(BX_CPU_ID, BX_CPU_THIS_PTR address_xlation.paddress1, 1, BX_WRITE);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit8u * hostAddr = (Bit8u *) BX_CPU_THIS_PTR address_xlation.pages;
    * hostAddr = val8;
    }
  else {
    // address_xlation.pages must be 1
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val8);
    }
}

  void
BX_CPU_C::write_RMW_virtual_word(Bit16u val16)
{
  BX_INSTR_MEM_DATA(BX_CPU_ID, BX_CPU_THIS_PTR address_xlation.paddress1, 2, BX_WRITE);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit16u *hostAddr = (Bit16u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostWordToLittleEndian(hostAddr, val16);
    }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
      BX_CPU_THIS_PTR mem->writePhysicalPage(this,
          BX_CPU_THIS_PTR address_xlation.paddress1, 2, &val16);
    }
  else {
#ifdef BX_LITTLE_ENDIAN
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val16);
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, ((Bit8u *) &val16) + 1);
#else
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, ((Bit8u *) &val16) + 1);
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, &val16);
#endif
    }
}

  void
BX_CPU_C::write_RMW_virtual_dword(Bit32u val32)
{
  BX_INSTR_MEM_DATA(BX_CPU_ID, BX_CPU_THIS_PTR address_xlation.paddress1, 4, BX_WRITE);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit32u *hostAddr = (Bit32u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostDWordToLittleEndian(hostAddr, val32);
    }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1, 4, &val32);
    }
  else {
#ifdef BX_LITTLE_ENDIAN
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        &val32);
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        &val32);
#endif
    }
}


  void
BX_CPU_C::write_virtual_qword(unsigned s, bx_address offset, Bit64u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset <= (seg->cache.u.segment.limit_scaled-7)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 8, BX_WRITE);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xff8) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit64u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
             && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              WriteHostQWordToLittleEndian(hostAddr, *data);
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 8, pl, BX_WRITE, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 8);
  goto accessOK;
}


  void
BX_CPU_C::read_virtual_qword(unsigned s, bx_address offset, Bit64u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessROK) {
    if ( IsLongMode() || (offset <= (seg->cache.u.segment.limit_scaled-7)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 8, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xff8) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf)) {
          // See if the TLB entry privilege level allows us read access
          // from this CPL.
          Bit32u accessBits;
          Bit32u hostPageAddr;
          Bit64u *hostAddr;

          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1<<pl) ) { // Read this pl OK.
            if (hostPageAddr) {
              ReadHostQWordFromLittleEndian(hostAddr, *data);
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 8, pl, BX_READ, (void *) data);
      return;
      }
    }
  read_virtual_checks(seg, offset, 8);
  goto accessOK;
}

  void
BX_CPU_C::write_RMW_virtual_qword(Bit64u val64)
{
  BX_INSTR_MEM_DATA(BX_CPU_ID, BX_CPU_THIS_PTR address_xlation.paddress1, 8, BX_WRITE);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit64u *hostAddr = (Bit64u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostQWordToLittleEndian(hostAddr, val64);
    }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1, 8, &val64);
    }
  else {
#ifdef BX_LITTLE_ENDIAN
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        &val64);
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val64) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val64) + (8 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_CPU_THIS_PTR mem->writePhysicalPage(this,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        &val64);
#endif
    }
}

  void
BX_CPU_C::read_RMW_virtual_qword(unsigned s, bx_address offset, Bit64u *data)
{
  bx_address laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  if (seg->cache.valid & SegAccessWOK) {
    if ( IsLongMode() || (offset <= (seg->cache.u.segment.limit_scaled-7)) ) {
      unsigned pl;
accessOK:
      laddr = seg->cache.u.segment.base + offset;
      BX_INSTR_MEM_DATA(BX_CPU_ID, laddr, 8, BX_READ);
      pl = (CPL==3);

#if BX_SupportGuest2HostTLB
      {
      bx_address lpf;
      Bit32u tlbIndex, pageOffset;

      pageOffset = laddr & 0xfff;
      if (pageOffset <= 0xff8) { // Make sure access does not span 2 pages.
        tlbIndex = BX_TLB_INDEX_OF(laddr);
        lpf = LPFOf(laddr);
        if ( (BX_CPU_THIS_PTR TLB.entry[tlbIndex].lpf == BX_TLB_LPF_VALUE(lpf))
             ) {
          Bit32u accessBits;
          Bit32u  hostPageAddr;
          Bit32u *hostAddr;

          // See if the TLB entry privilege level allows us write access
          // from this CPL.
          hostPageAddr = BX_CPU_THIS_PTR TLB.entry[tlbIndex].hostPageAddr;
          hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
          accessBits = BX_CPU_THIS_PTR TLB.entry[tlbIndex].accessBits;
          if ( accessBits & (1 << (2 | pl)) ) {
#if BX_SupportICache
            Bit32u *pageStamp;
            pageStamp = & BX_CPU_THIS_PTR iCache.pageWriteStampTable[
                BX_CPU_THIS_PTR TLB.entry[tlbIndex].ppf>>12];
#endif
            // Current write access has privilege.
            if (hostPageAddr
#if BX_SupportICache
             && (*pageStamp & ICacheWriteStampMask)
#endif
                ) {
              ReadHostQWordFromLittleEndian(hostAddr, *data);
              BX_CPU_THIS_PTR address_xlation.pages = (Bit32u) hostAddr;
#if BX_SupportICache
              (*pageStamp)--;
#endif
              return;
              }
            }
          }
        }
      }
#endif  // BX_SupportGuest2HostTLB

      access_linear(laddr, 8, pl, BX_RW, (void *) data);
      return;
      }
    }
  write_virtual_checks(seg, offset, 8);
  goto accessOK;
}


#if BX_SUPPORT_SSE != 0

// Some macro defs to make things cleaner for endian-ness issues.
// The following routines access a double qword, ie 16-bytes.
// For the moment, I redirect these to use the single qword routines
// by splitting one access into two.
//
// Endian  Host byte order         Guest (x86) byte order
// ======================================================
// Little  0..7 8..15               0..7 8..15
// Big    15..8 7...0               0..7 8..15
//
// Below are the host memory offsets to each of 2 single quadwords, which
// are different across big an little endian machines.  The memory
// accessing routines take care of the access endian issues when accessing
// the physical memory image.

#ifdef BX_LITTLE_ENDIAN
#  define Host1stDWordOffset 0
#  define Host2ndDWordOffset 8
#else
#  define Host1stDWordOffset 8
#  define Host2ndDWordOffset 0
#endif


  void
BX_CPU_C::readVirtualDQword(unsigned s, bx_address offset, Bit8u *data)
{
  // Read Double Quadword.
  Bit64u *qwords = (Bit64u*) data;

  read_virtual_qword(s, offset+Host1stDWordOffset, &qwords[0]);
  read_virtual_qword(s, offset+Host2ndDWordOffset, &qwords[1]);
}

  void
BX_CPU_C::readVirtualDQwordAligned(unsigned s, bx_address offset, Bit8u *data)
{
  // Read Double Quadword; access must be aligned on 16-byte boundary.
  Bit64u *qwords = (Bit64u*) data;

  // If double quadword access is unaligned, #GP(0).
  if (offset & 0xf)
    exception(BX_GP_EXCEPTION, 0, 0);

  read_virtual_qword(s, offset+Host1stDWordOffset, &qwords[0]);
  read_virtual_qword(s, offset+Host2ndDWordOffset, &qwords[1]);
}

  void
BX_CPU_C::writeVirtualDQword(unsigned s, bx_address offset, Bit8u *data)
{
  // Write Double Quadword.
  Bit64u *qwords = (Bit64u*) data;

  write_virtual_qword(s, offset+Host1stDWordOffset, &qwords[0]);
  write_virtual_qword(s, offset+Host2ndDWordOffset, &qwords[1]);
}


  void
BX_CPU_C::writeVirtualDQwordAligned(unsigned s, bx_address offset, Bit8u *data)
{
  // Write Double Quadword; access must be aligned on 16-byte boundary.
  Bit64u *qwords = (Bit64u*) data;

  // If double quadword access is unaligned, #GP(0).
  if (offset & 0xf)
    exception(BX_GP_EXCEPTION, 0, 0);

  write_virtual_qword(s, offset+Host1stDWordOffset, &qwords[0]);
  write_virtual_qword(s, offset+Host2ndDWordOffset, &qwords[1]);
}

#endif  // #if BX_SUPPORT_SSE
