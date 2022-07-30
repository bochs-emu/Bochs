/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2020 Stanislav Shwartsman
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

#define BX_CR0_PE_MASK         (1 <<  0)
#define BX_CR0_MP_MASK         (1 <<  1)
#define BX_CR0_EM_MASK         (1 <<  2)
#define BX_CR0_TS_MASK         (1 <<  3)
#define BX_CR0_ET_MASK         (1 <<  4)
#define BX_CR0_NE_MASK         (1 <<  5)
#define BX_CR0_WP_MASK         (1 << 16)
#define BX_CR0_AM_MASK         (1 << 18)
#define BX_CR0_NW_MASK         (1 << 29)
#define BX_CR0_CD_MASK         (1 << 30)
#define BX_CR0_PG_MASK         (1 << 31)

struct bx_cr0_t {
  Bit32u  val32; // 32bit value of register

  // Accessors for all cr0 bitfields.
#define IMPLEMENT_CRREG_ACCESSORS(name, bitnum)            \
  BX_CPP_INLINE bool get_##name() const {               \
    return 1 & (val32 >> bitnum);                          \
  }                                                        \
  BX_CPP_INLINE void set_##name(Bit8u val) {               \
    val32 = (val32 & ~(1<<bitnum)) | ((!!val) << bitnum);  \
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
  IMPLEMENT_CRREG_ACCESSORS(NW, 29);
  IMPLEMENT_CRREG_ACCESSORS(CD, 30);
#endif
  IMPLEMENT_CRREG_ACCESSORS(PG, 31);

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  // ET is hardwired bit in CR0
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val | 0x10; }
};

#if BX_CPU_LEVEL >= 5

#define BX_CR4_VME_MASK        (1 << 0)
#define BX_CR4_PVI_MASK        (1 << 1)
#define BX_CR4_TSD_MASK        (1 << 2)
#define BX_CR4_DE_MASK         (1 << 3)
#define BX_CR4_PSE_MASK        (1 << 4)
#define BX_CR4_PAE_MASK        (1 << 5)
#define BX_CR4_MCE_MASK        (1 << 6)
#define BX_CR4_PGE_MASK        (1 << 7)
#define BX_CR4_PCE_MASK        (1 << 8)
#define BX_CR4_OSFXSR_MASK     (1 << 9)
#define BX_CR4_OSXMMEXCPT_MASK (1 << 10)
#define BX_CR4_UMIP_MASK       (1 << 11)
#define BX_CR4_LA57_MASK       (1 << 12)
#define BX_CR4_VMXE_MASK       (1 << 13)
#define BX_CR4_SMXE_MASK       (1 << 14)
#define BX_CR4_FSGSBASE_MASK   (1 << 16)
#define BX_CR4_PCIDE_MASK      (1 << 17)
#define BX_CR4_OSXSAVE_MASK    (1 << 18)
#define BX_CR4_KEYLOCKER_MASK  (1 << 19)
#define BX_CR4_SMEP_MASK       (1 << 20)
#define BX_CR4_SMAP_MASK       (1 << 21)
#define BX_CR4_PKE_MASK        (1 << 22)
#define BX_CR4_CET_MASK        (1 << 23)
#define BX_CR4_PKS_MASK        (1 << 24)
#define BX_CR4_UINTR_MASK      (1 << 25)

struct bx_cr4_t {
  Bit32u  val32; // 32bit value of register

  IMPLEMENT_CRREG_ACCESSORS(VME, 0);
  IMPLEMENT_CRREG_ACCESSORS(PVI, 1);
  IMPLEMENT_CRREG_ACCESSORS(TSD, 2);
  IMPLEMENT_CRREG_ACCESSORS(DE,  3);
  IMPLEMENT_CRREG_ACCESSORS(PSE, 4);
  IMPLEMENT_CRREG_ACCESSORS(PAE, 5);
  IMPLEMENT_CRREG_ACCESSORS(MCE, 6);
  IMPLEMENT_CRREG_ACCESSORS(PGE, 7);
  IMPLEMENT_CRREG_ACCESSORS(PCE, 8);
  IMPLEMENT_CRREG_ACCESSORS(OSFXSR, 9);
  IMPLEMENT_CRREG_ACCESSORS(OSXMMEXCPT, 10);
  IMPLEMENT_CRREG_ACCESSORS(UMIP, 11);
  IMPLEMENT_CRREG_ACCESSORS(LA57, 12);
#if BX_SUPPORT_VMX
  IMPLEMENT_CRREG_ACCESSORS(VMXE, 13);
#endif
  IMPLEMENT_CRREG_ACCESSORS(SMXE, 14);
#if BX_SUPPORT_X86_64
  IMPLEMENT_CRREG_ACCESSORS(FSGSBASE, 16);
#endif
  IMPLEMENT_CRREG_ACCESSORS(PCIDE, 17);
  IMPLEMENT_CRREG_ACCESSORS(OSXSAVE, 18);
  IMPLEMENT_CRREG_ACCESSORS(KEYLOCKER, 19);
  IMPLEMENT_CRREG_ACCESSORS(SMEP, 20);
  IMPLEMENT_CRREG_ACCESSORS(SMAP, 21);
  IMPLEMENT_CRREG_ACCESSORS(PKE, 22);
  IMPLEMENT_CRREG_ACCESSORS(CET, 23);
  IMPLEMENT_CRREG_ACCESSORS(PKS, 24);
  IMPLEMENT_CRREG_ACCESSORS(UINTR, 25);

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

const Bit32u BX_CR4_FLUSH_TLB_MASK = (BX_CR4_PSE_MASK | BX_CR4_PAE_MASK | BX_CR4_PGE_MASK | BX_CR4_PCIDE_MASK | BX_CR4_SMEP_MASK | BX_CR4_SMAP_MASK | BX_CR4_PKE_MASK | BX_CR4_CET_MASK | BX_CR4_PKS_MASK);

#endif  // #if BX_CPU_LEVEL >= 5

struct bx_dr6_t {
  Bit32u val32; // 32bit value of register

  IMPLEMENT_CRREG_ACCESSORS(B0, 0);
  IMPLEMENT_CRREG_ACCESSORS(B1, 1);
  IMPLEMENT_CRREG_ACCESSORS(B2, 2);
  IMPLEMENT_CRREG_ACCESSORS(B3, 3);

#define BX_DEBUG_TRAP_HIT             (1 << 12)
#define BX_DEBUG_DR_ACCESS_BIT        (1 << 13)
#define BX_DEBUG_SINGLE_STEP_BIT      (1 << 14)
#define BX_DEBUG_TRAP_TASK_SWITCH_BIT (1 << 15)

  IMPLEMENT_CRREG_ACCESSORS(BD, 13);
  IMPLEMENT_CRREG_ACCESSORS(BS, 14);
  IMPLEMENT_CRREG_ACCESSORS(BT, 15);

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

struct bx_dr7_t {
  Bit32u val32; // 32bit value of register

  IMPLEMENT_CRREG_ACCESSORS(L0, 0);
  IMPLEMENT_CRREG_ACCESSORS(G0, 1);
  IMPLEMENT_CRREG_ACCESSORS(L1, 2);
  IMPLEMENT_CRREG_ACCESSORS(G1, 3);
  IMPLEMENT_CRREG_ACCESSORS(L2, 4);
  IMPLEMENT_CRREG_ACCESSORS(G2, 5);
  IMPLEMENT_CRREG_ACCESSORS(L3, 6);
  IMPLEMENT_CRREG_ACCESSORS(G3, 7);
  IMPLEMENT_CRREG_ACCESSORS(LE, 8);
  IMPLEMENT_CRREG_ACCESSORS(GE, 9);
  IMPLEMENT_CRREG_ACCESSORS(GD, 13);

#define IMPLEMENT_DRREG_ACCESSORS(name, bitmask, bitnum)      \
  int get_##name() const {                                    \
    return (val32 & (bitmask)) >> (bitnum);                   \
  }

  IMPLEMENT_DRREG_ACCESSORS(R_W0, 0x00030000, 16);
  IMPLEMENT_DRREG_ACCESSORS(LEN0, 0x000C0000, 18);
  IMPLEMENT_DRREG_ACCESSORS(R_W1, 0x00300000, 20);
  IMPLEMENT_DRREG_ACCESSORS(LEN1, 0x00C00000, 22);
  IMPLEMENT_DRREG_ACCESSORS(R_W2, 0x03000000, 24);
  IMPLEMENT_DRREG_ACCESSORS(LEN2, 0x0C000000, 26);
  IMPLEMENT_DRREG_ACCESSORS(R_W3, 0x30000000, 28);
  IMPLEMENT_DRREG_ACCESSORS(LEN3, 0xC0000000, 30);

  IMPLEMENT_DRREG_ACCESSORS(bp_enabled, 0xFF, 0);

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

#if BX_CPU_LEVEL >= 5

#define BX_EFER_SCE_MASK       (1 <<  0)
#define BX_EFER_LME_MASK       (1 <<  8)
#define BX_EFER_LMA_MASK       (1 << 10)
#define BX_EFER_NXE_MASK       (1 << 11)
#define BX_EFER_SVME_MASK      (1 << 12)
#define BX_EFER_LMSLE_MASK     (1 << 13)
#define BX_EFER_FFXSR_MASK     (1 << 14)
#define BX_EFER_TCE_MASK       (1 << 15)

struct bx_efer_t {
  Bit32u val32; // 32bit value of register

  IMPLEMENT_CRREG_ACCESSORS(SCE,    0);
#if BX_SUPPORT_X86_64
  IMPLEMENT_CRREG_ACCESSORS(LME,    8);
  IMPLEMENT_CRREG_ACCESSORS(LMA,   10);
#endif
  IMPLEMENT_CRREG_ACCESSORS(NXE,   11);
#if BX_SUPPORT_X86_64
  IMPLEMENT_CRREG_ACCESSORS(SVME,  12); /* AMD Secure Virtual Machine */
  IMPLEMENT_CRREG_ACCESSORS(LMSLE, 13); /* AMD Long Mode Segment Limit */
  IMPLEMENT_CRREG_ACCESSORS(FFXSR, 14);
  IMPLEMENT_CRREG_ACCESSORS(TCE,   15); /* AMD Translation Cache Extensions */
#endif

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

#endif

#if BX_CPU_LEVEL >= 6

const unsigned XSAVE_FPU_STATE_LEN          = 160;
const unsigned XSAVE_SSE_STATE_LEN          = 256;
const unsigned XSAVE_YMM_STATE_LEN          = 256;
const unsigned XSAVE_OPMASK_STATE_LEN       = 64;
const unsigned XSAVE_ZMM_HI256_STATE_LEN    = 512;
const unsigned XSAVE_HI_ZMM_STATE_LEN       = 1024;
const unsigned XSAVE_PKRU_STATE_LEN         = 64;
const unsigned XSAVE_CET_U_STATE_LEN        = 16;
const unsigned XSAVE_CET_S_STATE_LEN        = 24;

const unsigned XSAVE_SSE_STATE_OFFSET       = 160;
const unsigned XSAVE_YMM_STATE_OFFSET       = 576;
const unsigned XSAVE_OPMASK_STATE_OFFSET    = 1088;
const unsigned XSAVE_ZMM_HI256_STATE_OFFSET = 1152;
const unsigned XSAVE_HI_ZMM_STATE_OFFSET    = 1664;
const unsigned XSAVE_PKRU_STATE_OFFSET      = 2688;

struct xcr0_t {
  Bit32u  val32; // 32bit value of register

  enum {
    BX_XCR0_FPU_BIT = 0,
    BX_XCR0_SSE_BIT = 1,
    BX_XCR0_YMM_BIT = 2,
    BX_XCR0_BNDREGS_BIT = 3,
    BX_XCR0_BNDCFG_BIT = 4,
    BX_XCR0_OPMASK_BIT = 5,
    BX_XCR0_ZMM_HI256_BIT = 6,
    BX_XCR0_HI_ZMM_BIT = 7,
    BX_XCR0_PT_BIT = 8,
    BX_XCR0_PKRU_BIT = 9,
    BX_XCR0_CET_U_BIT = 11,
    BX_XCR0_CET_S_BIT = 12,
    BX_XCR0_UINTR_BIT = 14,
    BX_XCR0_XTILECFG_BIT = 17,
    BX_XCR0_XTILEDATA_BIT = 18,
    BX_XCR0_LAST
  };

#define BX_XCR0_FPU_MASK       (1 << xcr0_t::BX_XCR0_FPU_BIT)
#define BX_XCR0_SSE_MASK       (1 << xcr0_t::BX_XCR0_SSE_BIT)
#define BX_XCR0_YMM_MASK       (1 << xcr0_t::BX_XCR0_YMM_BIT)
#define BX_XCR0_BNDREGS_MASK   (1 << xcr0_t::BX_XCR0_BNDREGS_BIT)
#define BX_XCR0_BNDCFG_MASK    (1 << xcr0_t::BX_XCR0_BNDCFG_BIT)
#define BX_XCR0_OPMASK_MASK    (1 << xcr0_t::BX_XCR0_OPMASK_BIT)
#define BX_XCR0_ZMM_HI256_MASK (1 << xcr0_t::BX_XCR0_ZMM_HI256_BIT)
#define BX_XCR0_HI_ZMM_MASK    (1 << xcr0_t::BX_XCR0_HI_ZMM_BIT)
#define BX_XCR0_PT_MASK        (1 << xcr0_t::BX_XCR0_PT_BIT)
#define BX_XCR0_PKRU_MASK      (1 << xcr0_t::BX_XCR0_PKRU_BIT)
#define BX_XCR0_CET_U_MASK     (1 << xcr0_t::BX_XCR0_CET_U_BIT)
#define BX_XCR0_CET_S_MASK     (1 << xcr0_t::BX_XCR0_CET_S_BIT)
#define BX_XCR0_UINTR_MASK     (1 << xcr0_t::BX_XCR0_UINTR_BIT)
#define BX_XCR0_XTILECFG_MASK  (1 << xcr0_t::BX_XCR0_XTILECFG_BIT)
#define BX_XCR0_XTILEDATA_MASK (1 << xcr0_t::BX_XCR0_XTILEDATA_BIT)

  IMPLEMENT_CRREG_ACCESSORS(FPU, BX_XCR0_FPU_BIT);
  IMPLEMENT_CRREG_ACCESSORS(SSE, BX_XCR0_SSE_BIT);
  IMPLEMENT_CRREG_ACCESSORS(YMM, BX_XCR0_YMM_BIT);
  IMPLEMENT_CRREG_ACCESSORS(BNDREGS, BX_XCR0_BNDREGS_BIT);
  IMPLEMENT_CRREG_ACCESSORS(BNDCFG, BX_XCR0_BNDCFG_BIT);
  IMPLEMENT_CRREG_ACCESSORS(OPMASK, BX_XCR0_OPMASK_BIT);
  IMPLEMENT_CRREG_ACCESSORS(ZMM_HI256, BX_XCR0_ZMM_HI256_BIT);
  IMPLEMENT_CRREG_ACCESSORS(HI_ZMM, BX_XCR0_HI_ZMM_BIT);
  IMPLEMENT_CRREG_ACCESSORS(PT, BX_XCR0_PT_BIT);
  IMPLEMENT_CRREG_ACCESSORS(PKRU, BX_XCR0_PKRU_BIT);
  IMPLEMENT_CRREG_ACCESSORS(CET_U, BX_XCR0_CET_U_BIT);
  IMPLEMENT_CRREG_ACCESSORS(CET_S, BX_XCR0_CET_S_BIT);
  IMPLEMENT_CRREG_ACCESSORS(XTILECFG, BX_XCR0_XTILECFG_BIT);
  IMPLEMENT_CRREG_ACCESSORS(XTILEDATA, BX_XCR0_XTILEDATA_BIT);

  BX_CPP_INLINE Bit32u get32() const { return val32; }
  BX_CPP_INLINE void set32(Bit32u val) { val32 = val; }
};

#if BX_USE_CPU_SMF
typedef bool (*XSaveStateInUsePtr_tR)(void);
typedef void (*XSavePtr_tR)(bxInstruction_c *i, bx_address offset);
typedef void (*XRestorPtr_tR)(bxInstruction_c *i, bx_address offset);
typedef void (*XRestorInitPtr_tR)(void);
#else
typedef bool (BX_CPU_C::*XSaveStateInUsePtr_tR)(void);
typedef void (BX_CPU_C::*XSavePtr_tR)(bxInstruction_c *i, bx_address offset);
typedef void (BX_CPU_C::*XRestorPtr_tR)(bxInstruction_c *i, bx_address offset);
typedef void (BX_CPU_C::*XRestorInitPtr_tR)(void);
#endif

struct XSaveRestoreStateHelper {
  unsigned len;
  unsigned offset;
  XSaveStateInUsePtr_tR xstate_in_use_method;
  XSavePtr_tR xsave_method;
  XRestorPtr_tR xrstor_method;
  XRestorInitPtr_tR xrstor_init_method;
};

#endif

#undef IMPLEMENT_CRREG_ACCESSORS
#undef IMPLEMENT_DRREG_ACCESSORS

#if BX_CPU_LEVEL >= 5

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
  BX_CPP_INLINE Bit64u get64() const { return val64; }

  BX_CPP_INLINE bool set64(Bit64u new_val) {
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

#endif // BX_CPU_LEVEL >= 5

#endif
