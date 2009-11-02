/////////////////////////////////////////////////////////////////////////
// $Id: crregs.h,v 1.20 2009-11-02 15:00:47 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2009 Stanislav Shwartsman
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

#ifndef BX_CRREGS
#define BX_CRREGS

struct bx_cr0_t {
  Bit32u  val32; // 32bit value of register

  // Accessors for all cr0 bitfields.
#define IMPLEMENT_CRREG_ACCESSORS(name,bitnum)               \
  BX_CPP_INLINE bx_bool get_##name () {                      \
    return 1 & (val32 >> bitnum);                            \
  }                                                          \
  BX_CPP_INLINE void set_##name (Bit8u val) {                \
    val32 = (val32&~(1<<bitnum)) | (val ? (1<<bitnum) : 0);  \
  }

// CR0 notes:
//   Each x86 level has its own quirks regarding how it handles
//   reserved bits.  I used DOS DEBUG.EXE in real mode on the
//   following processors, tried to clear bits 1..30, then tried
//   to set bits 1..30, to see how these bits are handled.
//   I found the following:
//
//   Processor    try to clear bits 1..30    try to set bits 1..30
//   386          7FFFFFF0                   7FFFFFFE
//   486DX2       00000010                   6005003E
//   Pentium      00000010                   7FFFFFFE
//   Pentium-II   00000010                   6005003E
//
// My assumptions:
//   All processors: bit 4 is hardwired to 1 (not true on all clones)
//   386: bits 5..30 of CR0 are also hardwired to 1
//   Pentium: reserved bits retain value set using mov cr0, reg32
//   486DX2/Pentium-II: reserved bits are hardwired to 0

  IMPLEMENT_CRREG_ACCESSORS(PE, 0);
  IMPLEMENT_CRREG_ACCESSORS(MP, 1);
  IMPLEMENT_CRREG_ACCESSORS(EM, 2);
  IMPLEMENT_CRREG_ACCESSORS(TS, 3);
#if BX_CPU_LEVEL >= 4
  IMPLEMENT_CRREG_ACCESSORS(ET, 4);
  IMPLEMENT_CRREG_ACCESSORS(NE, 5);
  IMPLEMENT_CRREG_ACCESSORS(WP, 16);
  IMPLEMENT_CRREG_ACCESSORS(AM, 18);
  IMPLEMENT_CRREG_ACCESSORS(CD, 29);
  IMPLEMENT_CRREG_ACCESSORS(NW, 30);
#endif
  IMPLEMENT_CRREG_ACCESSORS(PG, 31);

  BX_CPP_INLINE Bit32u get32() { return val32; }
  // ET is hardwired bit in CR0
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val | 0x10; }
};

#if BX_CPU_LEVEL >= 4
struct bx_cr4_t {
  Bit32u  val32; // 32bit value of register

#if BX_CPU_LEVEL >= 5
  IMPLEMENT_CRREG_ACCESSORS(VME, 0);
  IMPLEMENT_CRREG_ACCESSORS(PVI, 1);
#endif
  IMPLEMENT_CRREG_ACCESSORS(TSD, 2);
  IMPLEMENT_CRREG_ACCESSORS(DE,  3);
  IMPLEMENT_CRREG_ACCESSORS(PSE, 4);
  IMPLEMENT_CRREG_ACCESSORS(PAE, 5);
  IMPLEMENT_CRREG_ACCESSORS(MCE, 6);
  IMPLEMENT_CRREG_ACCESSORS(PGE, 7);
  IMPLEMENT_CRREG_ACCESSORS(PCE, 8);
  IMPLEMENT_CRREG_ACCESSORS(OSFXSR, 9);
  IMPLEMENT_CRREG_ACCESSORS(OSXMMEXCPT, 10);
#if BX_SUPPORT_VMX
  IMPLEMENT_CRREG_ACCESSORS(VMXE, 13);
#endif
#if BX_SUPPORT_XSAVE
  IMPLEMENT_CRREG_ACCESSORS(OSXSAVE, 18);
#endif

  BX_CPP_INLINE Bit32u get32() { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};
#endif  // #if BX_CPU_LEVEL >= 4

extern bx_address get_cr4_allow_mask(void);

#if BX_SUPPORT_X86_64

struct bx_efer_t {
  Bit32u val32; // 32bit value of register

  IMPLEMENT_CRREG_ACCESSORS(SCE,    0);
  IMPLEMENT_CRREG_ACCESSORS(LME,    8);
  IMPLEMENT_CRREG_ACCESSORS(LMA,   10);
  IMPLEMENT_CRREG_ACCESSORS(NXE,   11);
  IMPLEMENT_CRREG_ACCESSORS(FFXSR, 14);

  BX_CPP_INLINE Bit32u get32() { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

#define BX_EFER_LMA_MASK       (1<<10)
#define BX_EFER_SUPPORTED_BITS BX_CONST64(0x00004d01)

#endif

#if BX_SUPPORT_XSAVE
struct xcr0_t {
  Bit32u  val32; // 32bit value of register

#define BX_XCR0_SUPPORTED_BITS 0x3

#define BX_XCR0_FPU_BIT   0
#define BX_XCR0_FPU_MASK (1<<BX_XCR0_FPU_BIT)
#define BX_XCR0_SSE_BIT   1
#define BX_XCR0_SSE_MASK (1<<BX_XCR0_SSE_BIT)

  IMPLEMENT_CRREG_ACCESSORS(FPU, BX_XCR0_FPU_BIT);
#if BX_SUPPORT_SSE
  IMPLEMENT_CRREG_ACCESSORS(SSE, BX_XCR0_SSE_BIT);
#endif

  BX_CPP_INLINE Bit32u get32() { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};
#endif

#undef IMPLEMENT_CRREG_ACCESSORS

typedef struct msr {
  unsigned index;          // MSR index
  unsigned type;           // MSR type: 1 - lin address, 2 - phy address
#define BX_LIN_ADDRESS_MSR 1
#define BX_PHY_ADDRESS_MSR 2
  Bit64u val64;            // current MSR value
  Bit64u reset_value;      // reset value
  Bit64u reserved;         // r/o bits - fault on write
  Bit64u ignored;          // hardwired bits - ignored on write

  msr(unsigned idx, unsigned msr_type = 0, Bit64u reset_val = 0, Bit64u rsrv = 0, Bit64u ign = 0):
     index(idx), type(msr_type), val64(reset_val), reset_value(reset_val),
     reserved(rsrv), ignored(ign) {}

  msr(unsigned idx, Bit64u reset_val = 0, Bit64u rsrv = 0, Bit64u ign = 0):
     index(idx), type(0), val64(reset_val), reset_value(reset_val),
     reserved(rsrv), ignored(ign) {}

  BX_CPP_INLINE void reset() { val64 = reset_value; }
  BX_CPP_INLINE Bit64u get64() { return val64; }

  BX_CPP_INLINE bx_bool set64(Bit64u new_val) {
     new_val = (new_val & ~ignored) | (val64 & ignored);
     switch(type) {
#if BX_SUPPORT_X86_64
       case BX_LIN_ADDRESS_MSR:
         if (! IsCanonical(new_val)) return 0;
         break;
#endif
       case BX_PHY_ADDRESS_MSR:
         if (! IsValidPhyAddr(new_val)) return 0;
         break;
       default:
         if ((val64 ^ new_val) & reserved) return 0;
         break;
     }
     val64 = new_val;
     return 1;
  }
} MSR;

#endif
