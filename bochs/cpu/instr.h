/////////////////////////////////////////////////////////////////////////
// $Id: instr.h,v 1.15 2008-08-09 19:18:09 sshwarts Exp $
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

class bxInstruction_c;

// <TAG-TYPE-EXECUTEPTR-START>
#if BX_USE_CPU_SMF
typedef void (BX_CPP_AttrRegparmN(1) *BxExecutePtr_tR)(bxInstruction_c *);
typedef bx_address (BX_CPP_AttrRegparmN(1) *BxResolvePtr_tR)(bxInstruction_c *);
#else
typedef void (BX_CPU_C::*BxExecutePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
typedef bx_address (BX_CPU_C::*BxResolvePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
// <TAG-TYPE-EXECUTEPTR-END>

// <TAG-CLASS-INSTRUCTION-START>
class bxInstruction_c {
public:
  // Function pointers; a function to resolve the modRM address
  // given the current state of the CPU and the instruction data,
  // and a function to execute the instruction after resolving
  // the memory address (if any).
  BxResolvePtr_tR ResolveModrm;
  BxExecutePtr_tR execute;
#if BX_INSTRUMENTATION
  Bit16u ia_opcode;
#endif

  struct {
    //  7...1 (unused)
    //  0...0 stop trace (used with trace cache)
    Bit8u metaInfo4;

    //  7...0 b1 - opcode byte
    Bit8u metaInfo3;

    //  7...4 (unused)
    //  3...0 ilen (0..15)
    Bit8u metaInfo2;

    //  7...7 extend8bit
    //  6...6 as64
    //  5...5 os64
    //  4...4 as32
    //  3...3 os32
    //  2...2 mod==c0 (modrm)
    //  1...0 repUsed (0=none, 2=0xF2, 3=0xF3)
    Bit8u metaInfo1;
  } metaInfo;

#define BX_INSTR_METADATA_SEG   0
#define BX_INSTR_METADATA_DEST  1
#define BX_INSTR_METADATA_NNN   2
#define BX_INSTR_METADATA_RM    3
#define BX_INSTR_METADATA_BASE  4
#define BX_INSTR_METADATA_INDEX 5
#define BX_INSTR_METADATA_SCALE 6
#define BX_INSTR_METADATA_MODRM 7

  // using 5-bit field for registers (16 regs in 64-bit, RIP, NIL)
  Bit8u metaData[8];

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

  BX_CPP_INLINE unsigned modC0() const
  {
    // This is a cheaper way to test for modRM instructions where
    // the mod field is 0xc0.  FetchDecode flags this condition since
    // it is quite common to be tested for.
    return metaInfo.metaInfo1 & (1<<2);
  }
  BX_CPP_INLINE unsigned assertModC0()
  {
    return metaInfo.metaInfo1 |= (1<<2);
  }
  BX_CPP_INLINE void setOpcodeReg(unsigned opreg) {
    // The opcodeReg form (low 3 bits of the opcode byte (extended
    // by REX.B on x86-64) to be used with IxIxForm or IqForm.
    metaData[BX_INSTR_METADATA_RM] = opreg;
  }
  BX_CPP_INLINE unsigned opcodeReg() const {
    return metaData[BX_INSTR_METADATA_RM];
  }
  BX_CPP_INLINE void setModRM(unsigned modrm) {
    metaData[BX_INSTR_METADATA_MODRM] = modrm;
  }
  BX_CPP_INLINE unsigned modrm() const {
    return metaData[BX_INSTR_METADATA_MODRM];
  }
  BX_CPP_INLINE void setNnn(unsigned nnn) {
    metaData[BX_INSTR_METADATA_NNN] = nnn;
  }
  BX_CPP_INLINE unsigned nnn() const {
    return metaData[BX_INSTR_METADATA_NNN];
  }
  BX_CPP_INLINE void setRm(unsigned rm) {
    metaData[BX_INSTR_METADATA_RM] = rm;
  }
  BX_CPP_INLINE unsigned rm() const {
    return metaData[BX_INSTR_METADATA_RM];
  }
  BX_CPP_INLINE void setSibScale(unsigned scale) {
    metaData[BX_INSTR_METADATA_SCALE] = scale;
  }
  BX_CPP_INLINE unsigned sibScale() const {
    return metaData[BX_INSTR_METADATA_SCALE];
  }
  BX_CPP_INLINE void setSibIndex(unsigned index) {
    metaData[BX_INSTR_METADATA_INDEX] = index;
  }
  BX_CPP_INLINE unsigned sibIndex() const {
    return metaData[BX_INSTR_METADATA_INDEX];
  }
  BX_CPP_INLINE void setSibBase(unsigned base) {
    metaData[BX_INSTR_METADATA_BASE] = base;
  }
  BX_CPP_INLINE unsigned sibBase() const {
    return metaData[BX_INSTR_METADATA_BASE];
  }
  BX_CPP_INLINE Bit32u displ32u() const { return modRMForm.displ32u; }
  BX_CPP_INLINE Bit16u displ16u() const { return modRMForm.displ16u; }
  BX_CPP_INLINE Bit32u Id() const  { return modRMForm.Id; }
  BX_CPP_INLINE Bit16u Iw() const  { return modRMForm.Iw; }
  BX_CPP_INLINE Bit8u  Ib() const  { return modRMForm.Ib; }
  BX_CPP_INLINE Bit16u Iw2() const { return IxIxForm.Iw2; } // Legacy
  BX_CPP_INLINE Bit8u  Ib2() const { return IxIxForm.Ib2; } // Legacy
#if BX_SUPPORT_X86_64
  BX_CPP_INLINE Bit64u Iq() const  { return IqForm.Iq; }
#endif

  // Info in the metaInfo field.
  // Note: the 'L' at the end of certain flags, means the value returned
  // is for Logical comparisons, eg if (i->os32L() && i->as32L()).  If you
  // want a bx_bool value, use os32B() etc.  This makes for smaller
  // code, when a strict 0 or 1 is not necessary.
  BX_CPP_INLINE void init(unsigned os32, unsigned as32, unsigned os64, unsigned as64)
  {
    metaInfo.metaInfo1 = (os32<<3) | (as32<<4) | (os64<<5) | (as64<<6);
    metaInfo.metaInfo4 = 0;
    metaData[BX_INSTR_METADATA_SEG] = BX_SEG_REG_NULL;
  }
  BX_CPP_INLINE unsigned seg(void) const {
    return metaData[BX_INSTR_METADATA_SEG];
  }
  BX_CPP_INLINE void setSeg(unsigned val) {
    metaData[BX_INSTR_METADATA_SEG] = val;
  }

  BX_CPP_INLINE unsigned os32L(void) const {
    return metaInfo.metaInfo1 & (1<<3);
  }
  BX_CPP_INLINE void setOs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<3)) | (bit<<3);
  }
  BX_CPP_INLINE void assertOs32(void) {
    metaInfo.metaInfo1 |= (1<<3);
  }

  BX_CPP_INLINE unsigned as32L(void) const {
    return metaInfo.metaInfo1 & (1<<4);
  }
  BX_CPP_INLINE void setAs32B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<4)) | (bit<<4);
  }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned os64L(void) const {
    return metaInfo.metaInfo1 & (1<<5);
  }
  BX_CPP_INLINE void assertOs64(void) {
    metaInfo.metaInfo1 |= (1<<5);
  }
#else
  BX_CPP_INLINE unsigned os64L(void) const { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned as64L(void) const {
    return metaInfo.metaInfo1 & (1<<6);
  }
  BX_CPP_INLINE void setAs64B(unsigned bit) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~(1<<6)) | (bit<<6);
  }
#else
  BX_CPP_INLINE unsigned as64L(void) const { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned extend8bitL(void) const {
    return metaInfo.metaInfo1 & (1<<7);
  }
  BX_CPP_INLINE void assertExtend8bit(void) {
    metaInfo.metaInfo1 |= (1<<7);
  }
#endif

  BX_CPP_INLINE unsigned ilen(void) const {
    return metaInfo.metaInfo2;
  }
  BX_CPP_INLINE void setILen(unsigned ilen) {
    metaInfo.metaInfo2 = ilen;
  }

  BX_CPP_INLINE unsigned repUsedL(void) const {
    return metaInfo.metaInfo1 & 3;
  }
  BX_CPP_INLINE unsigned repUsedValue(void) const {
    return metaInfo.metaInfo1 & 3;
  }
  BX_CPP_INLINE void setRepUsed(unsigned value) {
    metaInfo.metaInfo1 = (metaInfo.metaInfo1 & ~3) | (value);
  }

  BX_CPP_INLINE unsigned b1(void) const {
    return metaInfo.metaInfo3;
  }
  BX_CPP_INLINE void setB1(unsigned b1) {
    metaInfo.metaInfo3 = b1 & 0xff;
  }

#if BX_SUPPORT_TRACE_CACHE
  BX_CPP_INLINE void setStopTraceAttr(void) {
   metaInfo.metaInfo4 |= 1;
  }
  BX_CPP_INLINE unsigned getStopTraceAttr(void) const {
    return metaInfo.metaInfo4 & 1;
  }
#endif
};
// <TAG-CLASS-INSTRUCTION-END>

#endif
