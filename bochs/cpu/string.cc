/////////////////////////////////////////////////////////////////////////
// $Id: string.cc,v 1.6 2002-09-01 20:12:09 kevinlawton Exp $
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
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, edi, &temp8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI, EDI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI, EDI */
      esi++;
      edi++;
      }

    ESI = esi;
    EDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16 bit address mode */
    unsigned incr;
    Bit16u si, di;

    si = SI;
    di = DI;

#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u byteCount;
      bx_segment_reg_t *srcSegPtr, *dstSegPtr;
      Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

      if (i->as_32)
        byteCount = ECX;
      else
        byteCount = CX;
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

      if (byteCount) {
        Bit32u bytesCanFitSrc, bytesCanFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many bytes can fit in the rest of this page.
          bytesCanFitSrc = (0x1000 - (paddrSrc & 0xfff));
          bytesCanFitDst = (0x1000 - (paddrDst & 0xfff));
          // Restrict count to the number that will fit in either
          // source or dest pages.
          if (byteCount > bytesCanFitSrc)
            byteCount = bytesCanFitSrc;
          if (byteCount > bytesCanFitDst)
            byteCount = bytesCanFitDst;
          if (byteCount > bx_pc_system.num_cpu_ticks_left)
            byteCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (byteCount) {
            unsigned transferLen;
            Bit32u roomSrc, roomDst;
            unsigned j;
            unsigned pointerDelta;

            transferLen = byteCount; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            roomSrc = (srcSegPtr->cache.u.segment.limit_scaled - si) + 1;
            roomDst = (dstSegPtr->cache.u.segment.limit_scaled - di) + 1; 
            if ( (roomSrc < transferLen) || (roomDst < transferLen) ) {
              goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -1;
            else
              pointerDelta =  1;
            for (j=0; j<byteCount; j++) {
              * (Bit8u *) hostAddrDst = * (Bit8u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            bx_pc_system.num_cpu_ticks_left -= (byteCount-1);

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
#endif

noAcceleration16:

    read_virtual_byte(seg, si, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, di, &temp8);
    incr = 1;

doIncr16:

    if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u  temp32;

    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    if (i->os_32) {

#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u dwordCount;
      bx_segment_reg_t *srcSegPtr, *dstSegPtr;
      Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

      if (i->as_32)
        dwordCount = ECX;
      else
        dwordCount = CX;
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

      if (dwordCount) {
        Bit32u dwordsCanFitSrc, dwordsCanFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many dwords can fit in the rest of this page.
          dwordsCanFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 2;
          dwordsCanFitDst = (0x1000 - (paddrDst & 0xfff)) >> 2;
          // Restrict dword count to the number that will fit in either
          // source or dest pages.
          if (dwordCount > dwordsCanFitSrc)
            dwordCount = dwordsCanFitSrc;
          if (dwordCount > dwordsCanFitDst)
            dwordCount = dwordsCanFitDst;
          if (dwordCount > bx_pc_system.num_cpu_ticks_left)
            dwordCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (dwordCount) {
            unsigned transferLen;
            Bit32u roomSrc, roomDst;
            unsigned j;
            unsigned pointerDelta;

            transferLen = dwordCount<<2; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration32;
              }
            roomSrc = (srcSegPtr->cache.u.segment.limit_scaled - esi) + 1;
            roomDst = (dstSegPtr->cache.u.segment.limit_scaled - edi) + 1; 
            if ( (roomSrc < transferLen) || (roomDst < transferLen) ) {
              goto noAcceleration32;
              }

            // Transfer data directly using host addresses.
            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -4;
            else
              pointerDelta =  4;
            for (j=0; j<dwordCount; j++) {
              * (Bit32u *) hostAddrDst = * (Bit32u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            bx_pc_system.num_cpu_ticks_left -= (dwordCount-1);

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
#endif // __i386__
#endif

noAcceleration32:

      read_virtual_dword(seg, esi, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, edi, &temp32);
      incr = 4;

doIncr32:

      if (BX_CPU_THIS_PTR eflags.df) {
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

      if (BX_CPU_THIS_PTR eflags.df) {
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

    ESI = esi;
    EDI = edi;
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

      if (BX_CPU_THIS_PTR eflags.df) {
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

#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u wordCount;
      bx_segment_reg_t *srcSegPtr, *dstSegPtr;
      Bit32u laddrDst, laddrSrc, paddrDst, paddrSrc;

      if (i->as_32)
        wordCount = ECX;
      else
        wordCount = CX;
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

      if (wordCount) {
        Bit32u wordsCanFitSrc, wordsCanFitDst;
        Bit8u  *hostAddrSrc, *hostAddrDst;

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrSrc, BX_READ);
        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrSrc && hostAddrDst ) {
          // See how many words can fit in the rest of this page.
          wordsCanFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 1;
          wordsCanFitDst = (0x1000 - (paddrDst & 0xfff)) >> 1;
          // Restrict dword count to the number that will fit in either
          // source or dest pages.
          if (wordCount > wordsCanFitSrc)
            wordCount = wordsCanFitSrc;
          if (wordCount > wordsCanFitDst)
            wordCount = wordsCanFitDst;
          if (wordCount > bx_pc_system.num_cpu_ticks_left)
            wordCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (wordCount) {
            unsigned transferLen;
            Bit32u roomSrc, roomDst;
            unsigned j;
            unsigned pointerDelta;

            transferLen = wordCount<<1; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
                 !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            roomSrc = (srcSegPtr->cache.u.segment.limit_scaled - si) + 1;
            roomDst = (dstSegPtr->cache.u.segment.limit_scaled - di) + 1; 
            if ( (roomSrc < transferLen) || (roomDst < transferLen) ) {
              goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -2;
            else
              pointerDelta =  2;
            for (j=0; j<wordCount; j++) {
              * (Bit16u *) hostAddrDst = * (Bit16u *) hostAddrSrc;
              hostAddrDst += pointerDelta;
              hostAddrSrc += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            bx_pc_system.num_cpu_ticks_left -= (wordCount-1);

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
#endif // __i386__
#endif

noAcceleration16:

      read_virtual_word(seg, si, &temp16);

      write_virtual_word(BX_SEG_REG_ES, di, &temp16);
      incr = 2;

doIncr16:

      if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI */
      esi++;
      edi++;
      }

    EDI = edi;
    ESI = esi;
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

    if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u op1_32, op2_32, diff_32;
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;


    if (i->os_32) {
      read_virtual_dword(seg, esi, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR eflags.df) {
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

      if (BX_CPU_THIS_PTR eflags.df) {
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


    EDI = edi;
    ESI = esi;
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

      if (BX_CPU_THIS_PTR eflags.df) {
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

      if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);


    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      edi--;
      }
    else {
      /* increment ESI */
      edi++;
      }

    EDI = edi;
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

    if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR eflags.df) {
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

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        edi -= 2;
        }
      else {
        /* increment ESI */
        edi += 2;
        }
      }

    EDI = edi;
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

      if (BX_CPU_THIS_PTR eflags.df) {
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

      if (BX_CPU_THIS_PTR eflags.df) {
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
  al  = AL;


#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u byteCount;
      bx_segment_reg_t *dstSegPtr;
      Bit32u laddrDst, paddrDst;

      if (i->as_32)
        byteCount = ECX;
      else
        byteCount = CX;
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

      if (byteCount) {
        Bit32u bytesCanFitDst;
        Bit8u  *hostAddrDst;

        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddrDst, BX_WRITE);

        if ( hostAddrDst ) {
          // See how many bytes can fit in the rest of this page.
          bytesCanFitDst = (0x1000 - (paddrDst & 0xfff));
          // Restrict count to the number that will fit in either
          // source or dest pages.
          if (byteCount > bytesCanFitDst)
            byteCount = bytesCanFitDst;
          if (byteCount > bx_pc_system.num_cpu_ticks_left)
            byteCount = bx_pc_system.num_cpu_ticks_left;

          // If after all the restrictions, there is anything left to do...
          if (byteCount) {
            unsigned transferLen;
            Bit32u roomDst;
            unsigned j;
            unsigned pointerDelta;

            transferLen = byteCount; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration16;
              }
            roomDst = (dstSegPtr->cache.u.segment.limit_scaled - edi) + 1; 
            if ( roomDst < transferLen ) {
              goto noAcceleration16;
              }

            // Transfer data directly using host addresses.
            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -1;
            else
              pointerDelta =  1;
            for (j=0; j<byteCount; j++) {
              * (Bit8u *) hostAddrDst = al;
              hostAddrDst += pointerDelta;
              }
            // Decrement the ticks count by the number of iterations, minus
            // one, since the main cpu loop will decrement one.  Also,
            // the count is predecremented before examined, so defintely
            // don't roll it under zero.
            bx_pc_system.num_cpu_ticks_left -= (byteCount-1);

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
#endif

noAcceleration16:

    write_virtual_byte(BX_SEG_REG_ES, edi, &al);
    incr = 1;

doIncr16:

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement EDI */
      edi -= incr;
      }
    else {
      /* increment EDI */
      edi += incr;
      }

#if BX_CPU_LEVEL >= 3
  if (i->as_32)
    EDI = edi;
  else
#endif
    DI = edi;
}

  void
BX_CPU_C::STOSW_YveAX(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, edi, &eax);

        if (BX_CPU_THIS_PTR eflags.df) {
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

        if (BX_CPU_THIS_PTR eflags.df) {
          /* decrement EDI */
          edi -= 2;
          }
        else {
          /* increment EDI */
          edi += 2;
          }
      }

    EDI = edi;
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

        if (BX_CPU_THIS_PTR eflags.df) {
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

        if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

    read_virtual_byte(seg, esi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      esi--;
      }
    else {
      /* increment ESI */
      esi++;
      }

    ESI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

    read_virtual_byte(seg, si, &al);

    AL = al;
    if (BX_CPU_THIS_PTR eflags.df) {
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
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, esi, &eax);

      EAX = eax;
      if (BX_CPU_THIS_PTR eflags.df) {
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
      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        }
      }

    ESI = esi;
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

      EAX = eax;
      if (BX_CPU_THIS_PTR eflags.df) {
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
      if (BX_CPU_THIS_PTR eflags.df) {
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
