/////////////////////////////////////////////////////////////////////////
// $Id: crregs.h,v 1.5 2007-11-17 23:28:31 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007 Stanislav Shwartsman
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
  IMPLEMENT_CRREG_ACCESSORS(AM, 18);
  IMPLEMENT_CRREG_ACCESSORS(WP, 16);
  IMPLEMENT_CRREG_ACCESSORS(CD, 29);
  IMPLEMENT_CRREG_ACCESSORS(NW, 30);
#endif
  IMPLEMENT_CRREG_ACCESSORS(PG, 31);

  BX_CPP_INLINE Bit32u getRegister() { return val32; }
  BX_CPP_INLINE void setRegister(Bit32u val) { val32 = val; }
};

#if BX_CPU_LEVEL >= 4
struct bx_cr4_t {
  Bit32u  val32; // 32bit value of register

#if BX_SUPPORT_VME
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

  BX_CPP_INLINE Bit32u getRegister() { return val32; }
  BX_CPP_INLINE void setRegister(Bit32u val) { val32 = val; }
};
#endif  // #if BX_CPU_LEVEL >= 4

#if BX_SUPPORT_VME
  #define CR4_VME_ENABLED (BX_CPU_THIS_PTR cr4.get_VME())
#else
  #define CR4_VME_ENABLED (0)
#endif

#if BX_SUPPORT_X86_64
typedef struct bx_efer_t {
  // x86-64 EFER bits
  bx_bool sce;		// system call extensions
  bx_bool lme;		// long mode enable
  bx_bool lma;		// long mode active
  bx_bool nxe;		// no-execute enable
  bx_bool ffxsr;	// fast FXSAVE/FXRSTOR
} bx_efer_t;
#endif

#undef IMPLEMENT_CRREG_ACCESSORS

#endif
