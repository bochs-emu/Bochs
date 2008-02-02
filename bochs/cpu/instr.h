/////////////////////////////////////////////////////////////////////////
// $Id: instr.h,v 1.3 2008-02-02 21:46:51 sshwarts Exp $
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

#ifndef BX_INSTR_H
#  define BX_INSTR_H 1

// <TAG-CLASS-INSTRUCTION-START>
class bxInstruction_c {
public:
  // Function pointers; a function to resolve the modRM address
  // given the current state of the CPU and the instruction data,
  // and a function to execute the instruction after resolving
  // the memory address (if any).
#if BX_USE_CPU_SMF
  void (BX_CPP_AttrRegparmN(1) *ResolveModrm)(bxInstruction_c *);
  void (*execute)(bxInstruction_c *);
#else
  void (BX_CPU_C::*ResolveModrm)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  void (BX_CPU_C::*execute)(bxInstruction_c *);
#endif

  struct {
    // 31..29  (unused)
    // 28..20  b1 (9bits of opcode; 1byte-op=0..255, 2byte-op=256..511
    //            (leave this one on top so no mask is needed)
    // 19..19  stop trace (used with trace cache)
    // 18..18  mod==c0 (modrm)
    // 17..16  repUsed (0=none, 2=0xF2, 3=0xF3)
    Bit16u metaInfo3;

    // 15..12  (unused)
    // 11...8  ilen (0..15)
    Bit8u  metaInfo2;

    //  7...7  extend8bit
    //  6...6  as64
    //  5...5  os64
    //  4...4  as32
    //  3...3  os32
    //  2...0  seg
    Bit8u  metaInfo1;
  } metaInfo;

  struct {
    //  31..28 (unused)
    //  27..24 nnn     (modrm)
    Bit8u  modRMData4;

    //  23..20 (unused)
    //  19..16 base            (sib)
    Bit8u  modRMData3;

    //  15..14 mod     (modrm)
    //  13..12 scale           (sib)
    //  11...8 index           (sib)
    Bit8u  modRMData2;

    //   7...4 (unused)
    //   3...0 rm      (modrm)   // also used for opcodeReg()
    Bit8u  modRMData1;
  } metaData;

  union {
    // Form (longest case): [opcode+modrm+sib/displacement32/immediate32]
    struct {
      union {
        Bit32u Id;
        Bit16u Iw;
        Bit8u  Ib;
      };
      union {
        Bit16u displ16u; // for 16-bit modrm forms
        Bit32u displ32u; // for 32-bit modrm forms
      };
    } modRMForm;

    struct {
      union {
        Bit32u Id;
        Bit16u Iw;
        Bit8u  Ib;
      };
      union {
        Bit32u Id2; // Not used (for alignment)
        Bit16u Iw2;
        Bit8u  Ib2;
      };
    } IxIxForm;

#if BX_SUPPORT_X86_64
    struct {
      Bit64u   Iq;  // for MOV Rx,imm64
    } IqForm;
#endif
  };

  BX_CPP_INLINE unsigned opcodeReg() {
    // The opcodeReg form (low 3 bits of the opcode byte (extended
    // by REX.B on x86-64) to be used with IxIxForm or IqForm.
    return metaData.modRMData1;
  }
  // used in FPU only
  BX_CPP_INLINE unsigned modrm() {
#if BX_SUPPORT_X86_64
    return mod() | (rm() & 7) | ((nnn() & 7) << 3);
#else
    return mod() | rm() | (nnn() << 3);
#endif
  }
  BX_CPP_INLINE unsigned mod() { return metaData.modRMData2 & 0xc0; }
  BX_CPP_INLINE unsigned modC0()
  {
    // This is a cheaper way to test for modRM instructions where
    // the mod field is 0xc0.  FetchDecode flags this condition since
    // it is quite common to be tested for.
    return metaInfo.metaInfo3 & (1<<2);
  }
  BX_CPP_INLINE unsigned assertModC0()
  {
    return metaInfo.metaInfo3 |= (1<<2);
  }
  BX_CPP_INLINE unsigned nnn() {
    return metaData.modRMData4;
  }
  BX_CPP_INLINE unsigned rm() {
    return metaData.modRMData1;
  }
  BX_CPP_INLINE unsigned sibScale() {
    return (metaData.modRMData2 >> 4) & 0x3;
  }
  BX_CPP_INLINE unsigned sibIndex() {
    return (metaData.modRMData2) & 0xf;
  }
  BX_CPP_INLINE void setSibBase(unsigned base) {
    metaData.modRMData3 = base;
  }
  BX_CPP_INLINE unsigned sibBase() {
    return metaData.modRMData3;
  }
  BX_CPP_INLINE Bit32u displ32u() { return modRMForm.displ32u; }
  BX_CPP_INLINE Bit16u displ16u() { return modRMForm.displ16u; }
  BX_CPP_INLINE Bit32u Id()  { return modRMForm.Id; }
  BX_CPP_INLINE Bit16u Iw()  { return modRMForm.Iw; }
  BX_CPP_INLINE Bit8u  Ib()  { return modRMForm.Ib; }
  BX_CPP_INLINE Bit16u Iw2() { return IxIxForm.Iw2; } // Legacy
  BX_CPP_INLINE Bit8u  Ib2() { return IxIxForm.Ib2; } // Legacy
#if BX_SUPPORT_X86_64
  BX_CPP_INLINE Bit64u Iq()  { return IqForm.Iq; }
#endif

  // Info in the metaInfo field.
  // Note: the 'L' at the end of certain flags, means the value returned
  // is for Logical comparisons, eg if (i->os32L() && i->as32L()).  If you
  // want a bx_bool value, use os32B() etc.  This makes for smaller
  // code, when a strict 0 or 1 is not necessary.
  BX_CPP_INLINE void initMetaInfo(unsigned os32, unsigned as32,
                                  unsigned os64, unsigned as64)
  {
    metaInfo.metaInfo1 = BX_SEG_REG_NULL | (os32<<3) | (as32<<4) | (os64<<5) | (as64<<6);
    metaInfo.metaInfo2 = 0;
    metaInfo.metaInfo3 = 0;
  }
  BX_CPP_INLINE unsigned seg(void) {
    return metaInfo.metaInfo1 & 7;
  }
  BX_CPP_INLINE void setSeg(unsigned val) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~7) | val;
  }

  BX_CPP_INLINE unsigned os32L(void) {
    return metaInfo.metaInfo1 & (1<<3);
  }
  BX_CPP_INLINE unsigned os32B(void) {
    return (metaInfo.metaInfo1 >> 3) & 1;
  }
  BX_CPP_INLINE void setOs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<3)) | (bit<<3);
  }
  BX_CPP_INLINE void assertOs32(void) {
    metaInfo.metaInfo1 |= (1<<3);
  }

  BX_CPP_INLINE unsigned as32L(void) {
    return metaInfo.metaInfo1 & (1<<4);
  }
  BX_CPP_INLINE unsigned as32B(void) {
    return (metaInfo.metaInfo1 >> 4) & 1;
  }
  BX_CPP_INLINE void setAs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<4)) | (bit<<4);
  }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned os64L(void) {
    return metaInfo.metaInfo1 & (1<<5);
  }
  BX_CPP_INLINE void assertOs64(void) {
    metaInfo.metaInfo1 |= (1<<5);
  }
#else
  BX_CPP_INLINE unsigned os64L(void) { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned as64L(void) {
    return metaInfo.metaInfo1 & (1<<6);
  }
  BX_CPP_INLINE void setAs64B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<6)) | (bit<<6);
  }
#else
  BX_CPP_INLINE unsigned as64L(void) { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned extend8bitL(void) {
    return metaInfo.metaInfo1 & (1<<7);
  }
  BX_CPP_INLINE void assertExtend8bit(void) {
    metaInfo.metaInfo1 |= (1<<7);
  }
#endif

  BX_CPP_INLINE unsigned ilen(void) {
    return metaInfo.metaInfo2;
  }
  BX_CPP_INLINE void setILen(unsigned ilen) {
    metaInfo.metaInfo2 = ilen;
  }

  BX_CPP_INLINE unsigned repUsedL(void) {
    return metaInfo.metaInfo3 & 3;
  }
  BX_CPP_INLINE unsigned repUsedValue(void) {
    return metaInfo.metaInfo3 & 3;
  }
  BX_CPP_INLINE void setRepUsed(unsigned value) {
    metaInfo.metaInfo3 = (metaInfo.metaInfo3 & ~3) | (value);
  }

#if BX_SUPPORT_TRACE_CACHE
  BX_CPP_INLINE void setStopTraceAttr(void) {
   metaInfo.metaInfo3 |= (1<<3);
  }
  BX_CPP_INLINE unsigned getStopTraceAttr(void) {
    return metaInfo.metaInfo3 & (1<<3);
  }
#endif

  // Note this is the highest field, and thus needs no masking.
  // DON'T PUT ANY FIELDS HIGHER THAN THIS ONE WITHOUT ADDING A MASK.
  BX_CPP_INLINE unsigned b1(void) {
    return metaInfo.metaInfo3 >> 4;
  }
  BX_CPP_INLINE void setB1(unsigned b1) {
    metaInfo.metaInfo3 = (metaInfo.metaInfo3 & ~(0x1ff << 4)) | ((b1 & 0x1ff) << 4);
  }
};
// <TAG-CLASS-INSTRUCTION-END>


// <TAG-TYPE-EXECUTEPTR-START>
#if BX_USE_CPU_SMF
typedef void (*BxExecutePtr_t)(bxInstruction_c *);
typedef void (BX_CPP_AttrRegparmN(1) *BxExecutePtr_tR)(bxInstruction_c *);
#else
typedef void (BX_CPU_C::*BxExecutePtr_t)(bxInstruction_c *);
typedef void (BX_CPU_C::*BxExecutePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
// <TAG-TYPE-EXECUTEPTR-END>

#endif
