/////////////////////////////////////////////////////////////////////////
// $Id: string.cc,v 1.11 2002-09-15 05:09:18 kevinlawton Exp $
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
#define RSI ESI
#define RDI EDI
#define RAX EAX
#endif



/* MOVSB ES:[EDI], DS:[ESI]   DS may be overridden
 *   mov string from DS:[ESI] into ES:[EDI]
 */

  void
BX_CPU_C::MOVSB_XbYb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u temp8;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;

    read_virtual_byte(seg, rsi, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, rdi, &temp8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement RSI, RDI */
      rsi--;
      rdi--;
      }
    else {
      /* increment RSI, RDI */
      rsi++;
      rdi++;
      }

    RSI = rsi;
    RDI = rdi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, edi, &temp8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI, EDI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI, EDI */
      esi++;
      edi++;
      }

    // zero extension of RSI/RDI

    RSI = esi;
    RDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16 bit address mode */
    unsigned incr;
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u byteCount;

      if (i->as_32)
        byteCount = ECX;
      else
        byteCount = CX;

      if (byteCount) {
        Bit32u bytesFitSrc, bytesFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;
        unsigned pointerDelta;
        bx_segment_reg_t *srcSegPtr, *dstSegPtr;
        Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

        srcSegPtr = &BX_CPU_THIS_PTR sregs[seg];
        dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SREG_ES];

        // Do segment checks for the 1st word.  We do not want to
        // trip an exception beyond this, because the address would
        // be incorrect.  After we know how many bytes we will directly
        // transfer, we can do the full segment limit check ourselves
        // without generating an exception.
        read_virtual_checks(srcSegPtr, si, 1);
        laddrSrc = srcSegPtr->cache.u.segment.base + si;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
          }
        else {
          paddrSrc = laddrSrc;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrSrc = A20ADDR(paddrSrc);

        write_virtual_checks(dstSegPtr, di, 1);
        laddrDst = dstSegPtr->cache.u.segment.base + di;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
          }
        else {
          paddrDst = laddrDst;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrDst = A20ADDR(paddrDst);

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many bytes can fit in the rest of this page.
          if (BX_CPU_THIS_PTR get_DF ()) {
            // Counting downward.
            bytesFitSrc = 1 + (paddrSrc & 0xfff);
            bytesFitDst = 1 + (paddrDst & 0xfff);
            pointerDelta = (unsigned) -1;
            }
          else {
            // Counting upward.
            bytesFitSrc = (0x1000 - (paddrSrc & 0xfff));
            bytesFitDst = (0x1000 - (paddrDst & 0xfff));
            pointerDelta =  1;
            }
          // Restrict count to the number that will fit in either
          // source or dest pages.
          if (byteCount > bytesFitSrc)
            byteCount = bytesFitSrc;
          if (byteCount > bytesFitDst)
            byteCount = bytesFitDst;
          if (byteCount > bx_pc_system.num_cpu_ticks_left)
            byteCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (byteCount) {
            unsigned j;
            Bit32u srcSegLimit, dstSegLimit;

            srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
            dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;
            // For 16-bit addressing mode, clamp the segment limits to 16bits
            // so we don't have to worry about computations using si/di
            // rolling over 16-bit boundaries.
            if (!i->as_32) {
              if (srcSegLimit > 0xffff)
                srcSegLimit = 0xffff;
              if (dstSegLimit > 0xffff)
                dstSegLimit = 0xffff;
              }

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non expand-down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            // Now make sure transfer will fit within the constraints of the
            // segment boundaries, 0..limit for non expand-down.  We know
            // byteCount >= 1 here.
            if (BX_CPU_THIS_PTR get_DF ()) {
              // Counting downward.
              Bit32u minOffset = (byteCount-1);
              if ( si < minOffset )
                goto noAcceleration16;
              if ( di < minOffset )
                goto noAcceleration16;
              }
            else {
              // Counting upward.
              Bit32u srcMaxOffset = (srcSegLimit - byteCount) + 1;
              Bit32u dstMaxOffset = (dstSegLimit - byteCount) + 1;
              if ( si > srcMaxOffset )
                goto noAcceleration16;
              if ( di > dstMaxOffset )
                goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            for (j=0; j<byteCount; j++) {
              * (Bit8u *) hostAddrDst = * (Bit8u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            BX_TICKN(byteCount-1);
//bx_pc_system.num_cpu_ticks_left -= (byteCount-1);

            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (byteCount-1);
            else
              CX  -= (byteCount-1);
            incr = byteCount;
            goto doIncr16;
            }
          }
        }
      }

noAcceleration16:

#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    read_virtual_byte(seg, si, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, di, &temp8);
    incr = 1;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr16:
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement SI, DI */
      si -= incr;
      di -= incr;
      }
    else {
      /* increment SI, DI */
      si += incr;
      di += incr;
      }

    SI = si;
    DI = di;
    }
}

  void
BX_CPU_C::MOVSW_XvYv(BxInstruction_t *i)
{
  unsigned seg;
  unsigned incr;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {

    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;

    if (i->os_64) {
      Bit64u  temp64;
      read_virtual_qword(seg, rsi, &temp64);

      write_virtual_qword(BX_SEG_REG_ES, rdi, &temp64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement RSI */
        rsi -= 8;
        rdi -= 8;
        }
      else {
        /* increment ESI */
        rsi += 8;
        rdi += 8;
        }
      } /* if (i->os_64) ... */
    else
    if (i->os_32) {
      Bit32u  temp32;
      read_virtual_dword(seg, rsi, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, rdi, &temp32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement RSI */
        rsi -= 4;
        rdi -= 4;
        }
      else {
        /* increment ESI */
        rsi += 4;
        rdi += 4;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u  temp16;

      read_virtual_word(seg, rsi, &temp16);

      write_virtual_word(BX_SEG_REG_ES, rdi, &temp16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement RSI */
        rsi -= 2;
        rdi -= 2;
        }
      else {
        /* increment RSI */
        rsi += 2;
        rdi += 2;
        }
      }

    RSI = rsi;
    RDI = rdi;
    }

  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {

    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

#if BX_SUPPORT_X86_64
    if (i->os_64) {
      Bit64u  temp64;
      read_virtual_qword(seg, esi, &temp64);

      write_virtual_qword(BX_SEG_REG_ES, edi, &temp64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 8;
        edi -= 8;
        }
      else {
        /* increment ESI */
        esi += 8;
        edi += 8;
        }
      } /* if (i->os_32) ... */
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os_32) {

      Bit32u  temp32;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u dwordCount;

      if (i->as_32)
        dwordCount = ECX;
      else
        dwordCount = CX;

      if (dwordCount) {
        Bit32u dwordsFitSrc, dwordsFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;
        unsigned pointerDelta;
        bx_segment_reg_t *srcSegPtr, *dstSegPtr;
        Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

        srcSegPtr = &BX_CPU_THIS_PTR sregs[seg];
        dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SREG_ES];

        // Do segment checks for the 1st word.  We do not want to
        // trip an exception beyond this, because the address would
        // be incorrect.  After we know how many bytes we will directly
        // transfer, we can do the full segment limit check ourselves
        // without generating an exception.
        read_virtual_checks(srcSegPtr, esi, 4);
        laddrSrc = srcSegPtr->cache.u.segment.base + esi;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
          }
        else {
          paddrSrc = laddrSrc;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrSrc = A20ADDR(paddrSrc);

        write_virtual_checks(dstSegPtr, edi, 4);
        laddrDst = dstSegPtr->cache.u.segment.base + edi;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
          }
        else {
          paddrDst = laddrDst;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrDst = A20ADDR(paddrDst);

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many dwords can fit in the rest of this page.
          if (BX_CPU_THIS_PTR get_DF ()) {
            // Counting downward.
            // Note: 1st dword must not cross page boundary.
            if ( ((paddrSrc & 0xfff) > 0xffc) ||
                 ((paddrDst & 0xfff) > 0xffc) )
              goto noAcceleration32;
            dwordsFitSrc = (4 + (paddrSrc & 0xfff)) >> 2;
            dwordsFitDst = (4 + (paddrDst & 0xfff)) >> 2;
            pointerDelta = (unsigned) -4;
            }
          else {
            // Counting upward.
            dwordsFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 2;
            dwordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 2;
            pointerDelta =  4;
            }
          // Restrict dword count to the number that will fit in either
          // source or dest pages.
          if (dwordCount > dwordsFitSrc)
            dwordCount = dwordsFitSrc;
          if (dwordCount > dwordsFitDst)
            dwordCount = dwordsFitDst;
          if (dwordCount > bx_pc_system.num_cpu_ticks_left)
            dwordCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (dwordCount) {
            unsigned j;
            Bit32u srcSegLimit, dstSegLimit;

            srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
            dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;
            // For 16-bit addressing mode, clamp the segment limits to 16bits
            // so we don't have to worry about computations using si/di
            // rolling over 16-bit boundaries.
            if (!i->as_32) {
              if (srcSegLimit > 0xffff)
                srcSegLimit = 0xffff;
              if (dstSegLimit > 0xffff)
                dstSegLimit = 0xffff;
              }

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non expand-down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration32;
              }
            // Now make sure transfer will fit within the constraints of the
            // segment boundaries, 0..limit for non expand-down.  We know
            // dwordCount >= 1 here.
            if (BX_CPU_THIS_PTR get_DF ()) {
              // Counting downward.
              Bit32u minOffset = (dwordCount-1) << 2;
              if ( esi < minOffset )
                goto noAcceleration32;
              if ( edi < minOffset )
                goto noAcceleration32;
              }
            else {
              // Counting upward.
              Bit32u srcMaxOffset = (srcSegLimit - (dwordCount<<2)) + 1;
              Bit32u dstMaxOffset = (dstSegLimit - (dwordCount<<2)) + 1;
              if ( esi > srcMaxOffset )
                goto noAcceleration32;
              if ( edi > dstMaxOffset )
                goto noAcceleration32;
              }

            // Transfer data directly using host addresses.
            for (j=0; j<dwordCount; j++) {
              * (Bit32u *) hostAddrDst = * (Bit32u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            BX_TICKN(dwordCount-1);
//bx_pc_system.num_cpu_ticks_left -= (dwordCount-1);

            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (dwordCount-1);
            else
              CX  -= (dwordCount-1);
            incr = dwordCount << 2; // count * 4.
            goto doIncr32;
            }
          }
        }
      }

noAcceleration32:

#endif  // __i386__
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

      read_virtual_dword(seg, esi, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, edi, &temp32);
      incr = 4;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
doIncr32:
#endif
#endif
#endif

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= incr;
        edi -= incr;
        }
      else {
        /* increment ESI */
        esi += incr;
        edi += incr;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u  temp16;

      read_virtual_word(seg, esi, &temp16);

      write_virtual_word(BX_SEG_REG_ES, edi, &temp16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }

    // zero extension of RSI/RDI

    RSI = esi;
    RDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u temp32;

      read_virtual_dword(seg, si, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, di, &temp32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      } /* if (i->os_32) ... */
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
      Bit16u  temp16;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u wordCount;

      if (i->as_32)
        wordCount = ECX;
      else
        wordCount = CX;

      if (wordCount) {
        Bit32u wordsFitSrc, wordsFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;
        unsigned pointerDelta;
        bx_segment_reg_t *srcSegPtr, *dstSegPtr;
        Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

        srcSegPtr = &BX_CPU_THIS_PTR sregs[seg];
        dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SREG_ES];

        // Do segment checks for the 1st word.  We do not want to
        // trip an exception beyond this, because the address would
        // be incorrect.  After we know how many bytes we will directly
        // transfer, we can do the full segment limit check ourselves
        // without generating an exception.
        read_virtual_checks(srcSegPtr, si, 2);
        laddrSrc = srcSegPtr->cache.u.segment.base + si;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
          }
        else {
          paddrSrc = laddrSrc;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrSrc = A20ADDR(paddrSrc);

        write_virtual_checks(dstSegPtr, di, 2);
        laddrDst = dstSegPtr->cache.u.segment.base + di;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
          }
        else {
          paddrDst = laddrDst;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrDst = A20ADDR(paddrDst);

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many words can fit in the rest of this page.
          if (BX_CPU_THIS_PTR get_DF ()) {
            // Counting downward.
            // Note: 1st word must not cross page boundary.
            if ( ((paddrSrc & 0xfff) > 0xffe) ||
                 ((paddrDst & 0xfff) > 0xffe) )
              goto noAcceleration16;
            wordsFitSrc = (2 + (paddrSrc & 0xfff)) >> 1;
            wordsFitDst = (2 + (paddrDst & 0xfff)) >> 1;
            pointerDelta = (unsigned) -2;
            }
          else {
            // Counting upward.
            wordsFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 1;
            wordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 1;
            pointerDelta =  2;
            }
          // Restrict word count to the number that will fit in either
          // source or dest pages.
          if (wordCount > wordsFitSrc)
            wordCount = wordsFitSrc;
          if (wordCount > wordsFitDst)
            wordCount = wordsFitDst;
          if (wordCount > bx_pc_system.num_cpu_ticks_left)
            wordCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (wordCount) {
            unsigned j;
            Bit32u srcSegLimit, dstSegLimit;

            srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
            dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;
            // For 16-bit addressing mode, clamp the segment limits to 16bits
            // so we don't have to worry about computations using si/di
            // rolling over 16-bit boundaries.
            if (!i->as_32) {
              if (srcSegLimit > 0xffff)
                srcSegLimit = 0xffff;
              if (dstSegLimit > 0xffff)
                dstSegLimit = 0xffff;
              }

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non expand-down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            // Now make sure transfer will fit within the constraints of the
            // segment boundaries, 0..limit for non expand-down.  We know
            // wordCount >= 1 here.
            if (BX_CPU_THIS_PTR get_DF ()) {
              // Counting downward.
              Bit32u minOffset = (wordCount-1) << 1;
              if ( si < minOffset )
                goto noAcceleration16;
              if ( di < minOffset )
                goto noAcceleration16;
              }
            else {
              // Counting upward.
              Bit32u srcMaxOffset = (srcSegLimit - (wordCount<<1)) + 1;
              Bit32u dstMaxOffset = (dstSegLimit - (wordCount<<1)) + 1;
              if ( si > srcMaxOffset )
                goto noAcceleration16;
              if ( di > dstMaxOffset )
                goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            for (j=0; j<wordCount; j++) {
              * (Bit16u *) hostAddrDst = * (Bit16u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            BX_TICKN(wordCount-1);
//bx_pc_system.num_cpu_ticks_left -= (wordCount-1);

            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (wordCount-1);
            else
              CX  -= (wordCount-1);
            incr = wordCount << 1; // count * 2.
            goto doIncr16;
            }
          }
        }
      }

noAcceleration16:

#endif  // __i386__
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

      read_virtual_word(seg, si, &temp16);

      write_virtual_word(BX_SEG_REG_ES, di, &temp16);
      incr = 2;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
doIncr16:
#endif
#endif
#endif

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement SI, DI */
        si -= incr;
        di -= incr;
        }
      else {
        /* increment SI, DI */
        si += incr;
        di += incr;
        }
      }

    SI = si;
    DI = di;
    }
}

  void
BX_CPU_C::CMPSB_XbYb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u op1_8, op2_8, diff_8;


  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;

    read_virtual_byte(seg, rsi, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, rdi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement RSI */
      rsi--;
      rdi--;
      }
    else {
      /* increment RSI */
      rsi++;
      rdi++;
      }

    RDI = rdi;
    RSI = rsi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI */
      esi++;
      edi++;
      }

    // zero extension of RSI/RDI

    RDI = edi;
    RSI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

    read_virtual_byte(seg, si, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      si--;
      di--;
      }
    else {
      /* increment ESI */
      si++;
      di++;
      }

    DI = di;
    SI = si;
    }
}

  void
BX_CPU_C::CMPSW_XvYv(BxInstruction_t *i)
{
  unsigned seg;


  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;


    if (i->os_64) {
      Bit64u op1_64, op2_64, diff_64;

      read_virtual_qword(seg, rsi, &op1_64);

      read_virtual_qword(BX_SEG_REG_ES, rdi, &op2_64);

      diff_64 = op1_64 - op2_64;

      SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMPS64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 8;
        rdi -= 8;
        }
      else {
        /* increment ESI */
        rsi += 8;
        rdi += 8;
        }
      }
    else
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      read_virtual_dword(seg, rsi, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, rdi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 4;
        rdi -= 4;
        }
      else {
        /* increment ESI */
        rsi += 4;
        rdi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      read_virtual_word(seg, rsi, &op1_16);

      read_virtual_word(BX_SEG_REG_ES, rdi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 2;
        rdi -= 2;
        }
      else {
        /* increment ESI */
        rsi += 2;
        rdi += 2;
        }
      }


    RDI = rdi;
    RSI = rsi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;


#if BX_SUPPORT_X86_64
    if (i->os_64) {
      Bit64u op1_64, op2_64, diff_64;
      read_virtual_qword(seg, esi, &op1_64);

      read_virtual_qword(BX_SEG_REG_ES, edi, &op2_64);

      diff_64 = op1_64 - op2_64;

      SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMPS64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 8;
        edi -= 8;
        }
      else {
        /* increment ESI */
        esi += 8;
        edi += 8;
        }
      }
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;
      read_virtual_dword(seg, esi, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 4;
        edi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      read_virtual_word(seg, esi, &op1_16);

      read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }


    // zero extension of RSI/RDI

    RDI = edi;
    RSI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16 bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      read_virtual_dword(seg, si, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      read_virtual_word(seg, si, &op1_16);

      read_virtual_word(BX_SEG_REG_ES, di, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        si -= 2;
        di -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        di += 2;
        }
      }


    DI = di;
    SI = si;
    }
}

  void
BX_CPU_C::SCASB_ALXb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;


#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rdi;

    rdi = RDI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, rdi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);


    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      rdi--;
      }
    else {
      /* increment ESI */
      rdi++;
      }

    RDI = rdi;
    }

  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);


    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      edi--;
      }
    else {
      /* increment ESI */
      edi++;
      }

    // zero extension of RDI

    RDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      di--;
      }
    else {
      /* increment ESI */
      di++;
      }

    DI = di;
    }
}

  void
BX_CPU_C::SCASW_eAXXv(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rdi;

    rdi = RDI;

    if (i->os_64) {
      Bit64u op1_64, op2_64, diff_64;

      op1_64 = RAX;
      read_virtual_qword(BX_SEG_REG_ES, rdi, &op2_64);

      diff_64 = op1_64 - op2_64;

      SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SCAS64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement EDI */
        rdi -= 8;
        }
      else {
        /* increment EDI */
        rdi += 8;
        }
      }
    else
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, rdi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement EDI */
        rdi -= 4;
        }
      else {
        /* increment EDI */
        rdi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      read_virtual_word(BX_SEG_REG_ES, rdi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rdi -= 2;
        }
      else {
        /* increment ESI */
        rdi += 2;
        }
      }

    RDI = rdi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

#if BX_SUPPORT_X86_64
    if (i->os_64) {
      Bit64u op1_64, op2_64, diff_64;

      op1_64 = RAX;
      read_virtual_qword(BX_SEG_REG_ES, edi, &op2_64);

      diff_64 = op1_64 - op2_64;

      SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SCAS64);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        edi -= 8;
        }
      else {
        /* increment ESI */
        edi += 8;
        }
      }
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        edi -= 4;
        }
      else {
        /* increment ESI */
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        edi -= 2;
        }
      else {
        /* increment ESI */
        edi += 2;
        }
      }

    // zero extension of RDI

    RDI = edi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        di -= 4;
        }
      else {
        /* increment ESI */
        di += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      read_virtual_word(BX_SEG_REG_ES, di, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        di -= 2;
        }
      else {
        /* increment ESI */
        di += 2;
        }
      }

    DI = di;
    }
}

  void
BX_CPU_C::STOSB_YbAL(BxInstruction_t *i)
{
  Bit8u al;

#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rdi;

    rdi = RDI;

    al = AL;
    write_virtual_byte(BX_SEG_REG_ES, rdi, &al);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement EDI */
      rdi--;
      }
    else {
      /* increment EDI */
      rdi++;
      }

    RDI = rdi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  {
  Bit32u edi;
  unsigned incr;

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    edi = EDI;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address size */
    edi = DI;
    }

  al = AL;


#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u byteCount;

      if (i->as_32)
        byteCount = ECX;
      else
        byteCount = CX;

      if (byteCount) {
        Bit32u bytesFitDst;
        Bit8u  *hostAddrDst;
        unsigned pointerDelta;
        bx_segment_reg_t *dstSegPtr;
        Bit32u laddrDst, paddrDst;

        dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SREG_ES];

        // Do segment checks for the 1st word.  We do not want to
        // trip an exception beyond this, because the address would
        // be incorrect.  After we know how many bytes we will directly
        // transfer, we can do the full segment limit check ourselves
        // without generating an exception.
        write_virtual_checks(dstSegPtr, edi, 1);
        laddrDst = dstSegPtr->cache.u.segment.base + edi;
        if (BX_CPU_THIS_PTR cr0.pg) {
          paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
          }
        else {
          paddrDst = laddrDst;
          }
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddrDst = A20ADDR(paddrDst);

        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrDst ) {
          // See how many bytes can fit in the rest of this page.
          if (BX_CPU_THIS_PTR get_DF ()) {
            // Counting downward.
            bytesFitDst = 1 + (paddrDst & 0xfff);
            pointerDelta = (unsigned) -1;
            }
          else {
            // Counting upward.
            bytesFitDst = (0x1000 - (paddrDst & 0xfff));
            pointerDelta =  1;
            }
          // Restrict count to the number that will fit in either
          // source or dest pages.
          if (byteCount > bytesFitDst)
            byteCount = bytesFitDst;
          if (byteCount > bx_pc_system.num_cpu_ticks_left)
            byteCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (byteCount) {
            unsigned j;
            Bit32u dstSegLimit;

            dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;
            // For 16-bit addressing mode, clamp the segment limits to 16bits
            // so we don't have to worry about computations using si/di
            // rolling over 16-bit boundaries.
            if (!i->as_32) {
              if (dstSegLimit > 0xffff)
                dstSegLimit = 0xffff;
              }

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non expand-down (thus we can make a simple limit check).
            if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            // Now make sure transfer will fit within the constraints of the
            // segment boundaries, 0..limit for non expand-down.  We know
            // byteCount >= 1 here.
            if (BX_CPU_THIS_PTR get_DF ()) {
              // Counting downward.
              Bit32u minOffset = (byteCount-1);
              if ( edi < minOffset )
                goto noAcceleration16;
              }
            else {
              // Counting upward.
              Bit32u dstMaxOffset = (dstSegLimit - byteCount) + 1;
              if ( edi > dstMaxOffset )
                goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            for (j=0; j<byteCount; j++) {
              * (Bit8u *) hostAddrDst = al;
              hostAddrDst += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            BX_TICKN(byteCount-1);
//bx_pc_system.num_cpu_ticks_left -= (byteCount-1);

            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (byteCount-1);
            else
              CX  -= (byteCount-1);
            incr = byteCount;
            goto doIncr16;
            }
          }
        }
      }

noAcceleration16:

#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    write_virtual_byte(BX_SEG_REG_ES, edi, &al);
    incr = 1;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr16:
#endif
#endif


  if (BX_CPU_THIS_PTR get_DF ()) {
    /* decrement EDI */
    edi -= incr;
    }
  else {
    /* increment EDI */
    edi += incr;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32)
    // zero extension of RDI
    RDI = edi;
  else
#endif
    DI = edi;
  }
}

  void
BX_CPU_C::STOSW_YveAX(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rdi;

    rdi = RDI;

    if (i->os_64) {
        Bit64u rax;

        rax = RAX;
        write_virtual_qword(BX_SEG_REG_ES, rdi, &rax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          rdi -= 8;
          }
        else {
          /* increment EDI */
          rdi += 8;
          }
      } /* if (i->os_64) ... */
    else
    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, rdi, &eax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          rdi -= 4;
          }
        else {
          /* increment EDI */
          rdi += 4;
          }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        write_virtual_word(BX_SEG_REG_ES, rdi, &ax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          rdi -= 2;
          }
        else {
          /* increment EDI */
          rdi += 2;
          }
      }

    RDI = rdi;
    }

  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

#if BX_SUPPORT_X86_64
    if (i->os_64) {
        Bit64u rax;

        rax = RAX;
        write_virtual_qword(BX_SEG_REG_ES, edi, &rax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          edi -= 8;
          }
        else {
          /* increment EDI */
          edi += 8;
          }
      } /* if (i->os_4) ... */
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, edi, &eax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          edi -= 4;
          }
        else {
          /* increment EDI */
          edi += 4;
          }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        write_virtual_word(BX_SEG_REG_ES, edi, &ax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          edi -= 2;
          }
        else {
          /* increment EDI */
          edi += 2;
          }
      }

    // zero extension of RDI

    RDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address size */
    Bit16u di;

    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, di, &eax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          di -= 4;
          }
        else {
          /* increment EDI */
          di += 4;
          }
      } /* if (i->os_32) ... */
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        write_virtual_word(BX_SEG_REG_ES, di, &ax);

        if (BX_CPU_THIS_PTR get_DF ()) {
          /* decrement EDI */
          di -= 2;
          }
        else {
          /* increment EDI */
          di += 2;
          }
      }

    DI = di;
    }
}


  void
BX_CPU_C::LODSB_ALXb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u al;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rsi;

    rsi = RSI;

    read_virtual_byte(seg, rsi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      rsi--;
      }
    else {
      /* increment ESI */
      rsi++;
      }

    RSI = rsi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

    read_virtual_byte(seg, esi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      esi--;
      }
    else {
      /* increment ESI */
      esi++;
      }

    // zero extension of RSI

    RSI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

    read_virtual_byte(seg, si, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI */
      si--;
      }
    else {
      /* increment ESI */
      si++;
      }

    SI = si;
    }
}

  void
BX_CPU_C::LODSW_eAXXv(BxInstruction_t *i)
{
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (i->as_64) {
    Bit64u rsi;

    rsi = RSI;

    if (i->os_64) {
      Bit64u rax;

      read_virtual_qword(seg, rsi, &rax);

      RAX = rax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 8;
        }
      else {
        /* increment ESI */
        rsi += 8;
        }
      } /* if (i->os_64) ... */
    else
    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, rsi, &eax);

      RAX = eax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 4;
        }
      else {
        /* increment ESI */
        rsi += 4;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u ax;
      read_virtual_word(seg, rsi, &ax);

      AX = ax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        rsi -= 2;
        }
      else {
        /* increment ESI */
        rsi += 2;
        }
      }

    RSI = rsi;
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

#if BX_SUPPORT_X86_64
    if (i->os_64) {
      Bit64u rax;

      read_virtual_qword(seg, esi, &rax);

      RAX = rax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 8;
        }
      else {
        /* increment ESI */
        esi += 8;
        }
      } /* if (i->os_64) ... */
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, esi, &eax);

      RAX = eax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u ax;
      read_virtual_word(seg, esi, &ax);

      AX = ax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        esi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        }
      }

    // zero extension of RSI

    RSI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, si, &eax);

      RAX = eax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        si -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
      Bit16u ax;

      read_virtual_word(seg, si, &ax);

      AX = ax;
      if (BX_CPU_THIS_PTR get_DF ()) {
        /* decrement ESI */
        si -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        }
      }

    SI = si;
    }
}
