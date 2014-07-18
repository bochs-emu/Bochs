/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2014 Stanislav Shwartsman
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

#ifndef BX_COMMON_FETCHDECODE_TABLES_H
#define BX_COMMON_FETCHDECODE_TABLES_H

enum {
  BX_ILLEGAL_OPCODE,
  BX_ILLEGAL_LOCK_PREFIX,
  BX_ILLEGAL_VEX_XOP_VVV,
  BX_ILLEGAL_VEX_XOP_WITH_SSE_PREFIX,
  BX_ILLEGAL_VEX_XOP_WITH_REX_PREFIX,
  BX_ILLEGAL_VEX_XOP_OPCODE_MAP,
  BX_VEX_XOP_W0_ONLY,
  BX_VEX_XOP_W1_ONLY,
  BX_VEX_XOP_BAD_VECTOR_LENGTH,
  BX_VSIB_FORBIDDEN_ASIZE16,
  BX_VSIB_ILLEGAL_SIB_INDEX,
  BX_EVEX_RESERVED_BITS_SET,
  BX_EVEX_ILLEGAL_EVEX_B_SAE_NOT_ALLOWED,
  BX_EVEX_ILLEGAL_EVEX_B_BROADCAST_NOT_ALLOWED,
  BX_EVEX_ILLEGAL_KMASK_REGISTER,
  BX_EVEX_ILLEGAL_ZERO_MASKING_NO_OPMASK,
  BX_EVEX_ILLEGAL_ZERO_MASKING_VSIB,
  BX_EVEX_ILLEGAL_ZERO_MASKING_MEMORY_DESTINATION,
};

typedef struct BxOpcodeInfo_t {
  Bit16u Attr;
  Bit16u IA;
  const BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;

//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

BX_CPP_INLINE Bit16u FetchWORD(const Bit8u *iptr)
{
   Bit16u data;
   ReadHostWordFromLittleEndian(iptr, data);
   return data;
}

BX_CPP_INLINE Bit32u FetchDWORD(const Bit8u *iptr)
{
   Bit32u data;
   ReadHostDWordFromLittleEndian(iptr, data);
   return data;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u FetchQWORD(const Bit8u *iptr)
{
   Bit64u data;
   ReadHostQWordFromLittleEndian(iptr, data);
   return data;
}
#endif

#define BX_PREPARE_EVEX_NO_BROADCAST (0x40 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX_NO_SAE       (0x20 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX              (0x10)
#define BX_PREPARE_OPMASK            (0x08)
#define BX_PREPARE_AVX               (0x04)
#define BX_PREPARE_SSE               (0x02)
#define BX_TRACE_END                 (0x01)

struct bxIAOpcodeTable {
  BxExecutePtr_tR execute1;
  BxExecutePtr_tR execute2;
  Bit8u src[4];
  Bit8u opflags;
};

enum {
  BX_SRC_NONE = 0,
  BX_SRC_EAX = 1,
  BX_SRC_NNN = 2,
  BX_SRC_RM = 3,
  BX_SRC_EVEX_RM = 4,
  BX_SRC_VVV = 5,
  BX_SRC_VIB = 6,
  BX_SRC_VSIB = 7    // gather/scatter vector index
};

enum {
  BX_NO_REG = 0,
  BX_GPR8 = 0x1,
  BX_GPR8_32 = 0x2,  // 8-bit memory reference but 32-bit GPR
  BX_GPR16 = 0x3,
  BX_GPR16_32 = 0x4, // 16-bit memory reference but 32-bit GPR
  BX_GPR32 = 0x5,
  BX_GPR64 = 0x6,
  BX_FPU_REG = 0x7,
  BX_MMX_REG = 0x8,
  BX_VMM_REG = 0x9,
  BX_KMASK_REG = 0xA,
  BX_SEGREG = 0xB,
  BX_CREG = 0xC,
  BX_DREG = 0xD,
  BX_BOUNDS_REG = 0xE
};

enum {
  BX_VMM_FULL_VECTOR = 0,
  BX_VMM_SCALAR_BYTE = 1,
  BX_VMM_SCALAR_WORD = 2,
  BX_VMM_SCALAR = 3,
  BX_VMM_HALF_VECTOR = 4,
  BX_VMM_QUARTER_VECTOR = 5,
  BX_VMM_OCT_VECTOR = 6,
  BX_VMM_VEC128 = 7,
  BX_VMM_VEC256 = 8
};

enum {
  BX_IMMB = 0x10,
  BX_IMMW = 0x11,
  BX_IMMD = 0x12,
  BX_IMMD_SE = 0x13,
  BX_IMMQ = 0x14,
  BX_IMMB2 = 0x15,
  BX_IMM_BrOff16 = 0x16,
  BX_IMM_BrOff32 = 0x17,
  BX_IMM_BrOff64 = 0x18,
  BX_RSIREF = 0x19,
  BX_RDIREF = 0x1A,
  BX_USECL = 0x1B,
  BX_USEDX = 0x1C,
  BX_DIRECT_PTR = 0x1D,
  BX_DIRECT_MEMREF32 = 0x1E,
  BX_DIRECT_MEMREF64 = 0x1F
};

#define BX_FORM_SRC(type, src) (((type) << 3) | (src))

const Bit8u OP_NONE = BX_SRC_NONE;

const Bit8u OP_Eb = BX_FORM_SRC(BX_GPR8, BX_SRC_RM);
const Bit8u OP_Ebd = BX_FORM_SRC(BX_GPR8_32, BX_SRC_RM);
const Bit8u OP_Ew = BX_FORM_SRC(BX_GPR16, BX_SRC_RM);
const Bit8u OP_Ewd = BX_FORM_SRC(BX_GPR16_32, BX_SRC_RM);
const Bit8u OP_Ed = BX_FORM_SRC(BX_GPR32, BX_SRC_RM);
const Bit8u OP_Eq = BX_FORM_SRC(BX_GPR64, BX_SRC_RM);

const Bit8u OP_Gb = BX_FORM_SRC(BX_GPR8, BX_SRC_NNN);
const Bit8u OP_Gw = BX_FORM_SRC(BX_GPR16, BX_SRC_NNN);
const Bit8u OP_Gd = BX_FORM_SRC(BX_GPR32, BX_SRC_NNN);
const Bit8u OP_Gq = BX_FORM_SRC(BX_GPR64, BX_SRC_NNN);

const Bit8u OP_ALReg  = BX_FORM_SRC(BX_GPR8, BX_SRC_EAX);
const Bit8u OP_AXReg  = BX_FORM_SRC(BX_GPR16, BX_SRC_EAX);
const Bit8u OP_EAXReg = BX_FORM_SRC(BX_GPR32, BX_SRC_EAX);
const Bit8u OP_RAXReg = BX_FORM_SRC(BX_GPR64, BX_SRC_EAX);

const Bit8u OP_CLReg  = BX_FORM_SRC(BX_USECL, BX_SRC_NONE);
const Bit8u OP_DXReg  = BX_FORM_SRC(BX_USEDX, BX_SRC_NONE);

const Bit8u OP_Ib = BX_FORM_SRC(BX_IMMB, BX_SRC_NONE);
const Bit8u OP_Iw = BX_FORM_SRC(BX_IMMW, BX_SRC_NONE);
const Bit8u OP_Id = BX_FORM_SRC(BX_IMMD, BX_SRC_NONE);
const Bit8u OP_sId = BX_FORM_SRC(BX_IMMD_SE, BX_SRC_NONE);
const Bit8u OP_Iq = BX_FORM_SRC(BX_IMMQ, BX_SRC_NONE);
const Bit8u OP_Ib2 = BX_FORM_SRC(BX_IMMB2, BX_SRC_NONE);

const Bit8u OP_Jw = BX_FORM_SRC(BX_IMM_BrOff16, BX_SRC_NONE);
const Bit8u OP_Jd = BX_FORM_SRC(BX_IMM_BrOff32, BX_SRC_NONE);
const Bit8u OP_Jq = BX_FORM_SRC(BX_IMM_BrOff64, BX_SRC_NONE);

const Bit8u OP_M  = BX_SRC_RM;
const Bit8u OP_Mb = BX_SRC_RM;
const Bit8u OP_Mw = BX_SRC_RM;
const Bit8u OP_Md = BX_SRC_RM;
const Bit8u OP_Mq = BX_SRC_RM;
const Bit8u OP_Mp = BX_SRC_RM;
const Bit8u OP_Mt = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);

const Bit8u OP_Mdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);

const Bit8u OP_Pq = BX_FORM_SRC(BX_MMX_REG, BX_SRC_NNN);
const Bit8u OP_Qq = BX_FORM_SRC(BX_MMX_REG, BX_SRC_RM);
const Bit8u OP_Qd = BX_FORM_SRC(BX_MMX_REG, BX_SRC_RM);

const Bit8u OP_Vdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vss = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vsd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vq  = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vd  = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);

const Bit8u OP_Wq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Ww = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wb = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wss = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);
const Bit8u OP_Wsd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);

const Bit8u OP_mVps = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVpd = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVps32 = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVpd64 = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVss = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVsd = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq8 = BX_FORM_SRC(BX_VMM_SCALAR_BYTE, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq16 = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq32 = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq64 = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_EVEX_RM);
const Bit8u OP_mVHV = BX_FORM_SRC(BX_VMM_HALF_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVQV = BX_FORM_SRC(BX_VMM_QUARTER_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVOV = BX_FORM_SRC(BX_VMM_OCT_VECTOR, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq128 = BX_FORM_SRC(BX_VMM_VEC128, BX_SRC_EVEX_RM);
const Bit8u OP_mVdq256 = BX_FORM_SRC(BX_VMM_VEC256, BX_SRC_EVEX_RM);

const Bit8u OP_Hdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hss = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hsd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);

const Bit8u OP_Bd = BX_FORM_SRC(BX_GPR32, BX_SRC_VVV);
const Bit8u OP_Bq = BX_FORM_SRC(BX_GPR64, BX_SRC_VVV);

const Bit8u OP_VIb = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VIB);

const Bit8u OP_VSib = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VSIB);

const Bit8u OP_Cd = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Cq = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Dd = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);
const Bit8u OP_Dq = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);

const Bit8u OP_Sw = BX_FORM_SRC(BX_SEGREG, BX_SRC_NNN);

const Bit8u OP_Od = BX_FORM_SRC(BX_DIRECT_MEMREF32, BX_SRC_NONE);
const Bit8u OP_Oq = BX_FORM_SRC(BX_DIRECT_MEMREF64, BX_SRC_NONE);

const Bit8u OP_Ap = BX_FORM_SRC(BX_DIRECT_PTR, BX_SRC_NONE);

const Bit8u OP_KGb = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEb = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_RM);
const Bit8u OP_KHb = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGw = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEw = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_RM);
const Bit8u OP_KHw = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGd = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEd = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_RM);
const Bit8u OP_KHd = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_RM);
const Bit8u OP_KHq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_ST0 = BX_FORM_SRC(BX_FPU_REG, BX_SRC_EAX);
const Bit8u OP_STi = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);

const Bit8u OP_Xb = BX_FORM_SRC(BX_RSIREF, BX_SRC_NONE);
const Bit8u OP_Xw = BX_FORM_SRC(BX_RSIREF, BX_SRC_NONE);
const Bit8u OP_Xd = BX_FORM_SRC(BX_RSIREF, BX_SRC_NONE);
const Bit8u OP_Xq = BX_FORM_SRC(BX_RSIREF, BX_SRC_NONE);

const Bit8u OP_Yb = BX_FORM_SRC(BX_RDIREF, BX_SRC_NONE);
const Bit8u OP_Yw = BX_FORM_SRC(BX_RDIREF, BX_SRC_NONE);
const Bit8u OP_Yd = BX_FORM_SRC(BX_RDIREF, BX_SRC_NONE);
const Bit8u OP_Yq = BX_FORM_SRC(BX_RDIREF, BX_SRC_NONE);

//
// Common FetchDecode Opcode Tables
//

#include "fetchdecode_x87.h"
#include "fetchdecode_sse.h"

#if BX_SUPPORT_AVX
#include "fetchdecode_avx.h"
#include "fetchdecode_xop.h"
#endif

#if BX_SUPPORT_EVEX
#include "fetchdecode_evex.h"
#endif

/* ************************************************************************ */
/* Opcode Groups */

/* ******* */
/* Group 1 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIb[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EbIb },
  /* 1 */ { BxLockable, BX_IA_OR_EbIb  },
  /* 2 */ { BxLockable, BX_IA_ADC_EbIb },
  /* 3 */ { BxLockable, BX_IA_SBB_EbIb },
  /* 4 */ { BxLockable, BX_IA_AND_EbIb },
  /* 5 */ { BxLockable, BX_IA_SUB_EbIb },
  /* 6 */ { BxLockable, BX_IA_XOR_EbIb },
  /* 7 */ { 0,          BX_IA_CMP_EbIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1Ew[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EwIw },
  /* 1 */ { BxLockable, BX_IA_OR_EwIw },
  /* 2 */ { BxLockable, BX_IA_ADC_EwIw },
  /* 3 */ { BxLockable, BX_IA_SBB_EwIw },
  /* 4 */ { BxLockable, BX_IA_AND_EwIw },
  /* 5 */ { BxLockable, BX_IA_SUB_EwIw },
  /* 6 */ { BxLockable, BX_IA_XOR_EwIw },
  /* 7 */ { 0,          BX_IA_CMP_EwIw }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1Ed[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EdId },
  /* 1 */ { BxLockable, BX_IA_OR_EdId },
  /* 2 */ { BxLockable, BX_IA_ADC_EdId },
  /* 3 */ { BxLockable, BX_IA_SBB_EdId },
  /* 4 */ { BxLockable, BX_IA_AND_EdId },
  /* 5 */ { BxLockable, BX_IA_SUB_EdId },
  /* 6 */ { BxLockable, BX_IA_XOR_EdId },
  /* 7 */ { 0,          BX_IA_CMP_EdId }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1Eq[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EqId },
  /* 1 */ { BxLockable, BX_IA_OR_EqId },
  /* 2 */ { BxLockable, BX_IA_ADC_EqId },
  /* 3 */ { BxLockable, BX_IA_SBB_EqId },
  /* 4 */ { BxLockable, BX_IA_AND_EqId },
  /* 5 */ { BxLockable, BX_IA_SUB_EqId },
  /* 6 */ { BxLockable, BX_IA_XOR_EqId },
  /* 7 */ { 0,          BX_IA_CMP_EqId }
};
#endif

/* ******** */
/* Group 1A */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG1AEw[8] = {
  /* 0 */ { 0, BX_IA_POP_Ew },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1AEd[8] = {
  /* 0 */ { 0, BX_IA_POP_Ed },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1AEq[8] = {
  /* 0 */ { 0, BX_IA_POP_Eq },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 2 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG2Eb[8] = {
  /* 0 */ { 0, BX_IA_ROL_Eb },
  /* 1 */ { 0, BX_IA_ROR_Eb },
  /* 2 */ { 0, BX_IA_RCL_Eb },
  /* 3 */ { 0, BX_IA_RCR_Eb },
  /* 4 */ { 0, BX_IA_SHL_Eb },
  /* 5 */ { 0, BX_IA_SHR_Eb },
  /* 6 */ { 0, BX_IA_SHL_Eb },
  /* 7 */ { 0, BX_IA_SAR_Eb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2Ew[8] = {
  /* 0 */ { 0, BX_IA_ROL_Ew },
  /* 1 */ { 0, BX_IA_ROR_Ew },
  /* 2 */ { 0, BX_IA_RCL_Ew },
  /* 3 */ { 0, BX_IA_RCR_Ew },
  /* 4 */ { 0, BX_IA_SHL_Ew },
  /* 5 */ { 0, BX_IA_SHR_Ew },
  /* 6 */ { 0, BX_IA_SHL_Ew },
  /* 7 */ { 0, BX_IA_SAR_Ew }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2Ed[8] = {
  /* 0 */ { 0, BX_IA_ROL_Ed },
  /* 1 */ { 0, BX_IA_ROR_Ed },
  /* 2 */ { 0, BX_IA_RCL_Ed },
  /* 3 */ { 0, BX_IA_RCR_Ed },
  /* 4 */ { 0, BX_IA_SHL_Ed },
  /* 5 */ { 0, BX_IA_SHR_Ed },
  /* 6 */ { 0, BX_IA_SHL_Ed },
  /* 7 */ { 0, BX_IA_SAR_Ed }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G2Eq[8] = {
  /* 0 */ { 0, BX_IA_ROL_Eq },
  /* 1 */ { 0, BX_IA_ROR_Eq },
  /* 2 */ { 0, BX_IA_RCL_Eq },
  /* 3 */ { 0, BX_IA_RCR_Eq },
  /* 4 */ { 0, BX_IA_SHL_Eq },
  /* 5 */ { 0, BX_IA_SHR_Eq },
  /* 6 */ { 0, BX_IA_SHL_Eq },
  /* 7 */ { 0, BX_IA_SAR_Eq }
};
#endif

/* ********* */
/* Group2 Ib */
/* ********* */

static const BxOpcodeInfo_t BxOpcodeInfoG2EbIb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EbIb },
  /* 1 */ { 0, BX_IA_ROR_EbIb },
  /* 2 */ { 0, BX_IA_RCL_EbIb },
  /* 3 */ { 0, BX_IA_RCR_EbIb },
  /* 4 */ { 0, BX_IA_SHL_EbIb },
  /* 5 */ { 0, BX_IA_SHR_EbIb },
  /* 6 */ { 0, BX_IA_SHL_EbIb },
  /* 7 */ { 0, BX_IA_SAR_EbIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2EwIb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EwIb },
  /* 1 */ { 0, BX_IA_ROR_EwIb },
  /* 2 */ { 0, BX_IA_RCL_EwIb },
  /* 3 */ { 0, BX_IA_RCR_EwIb },
  /* 4 */ { 0, BX_IA_SHL_EwIb },
  /* 5 */ { 0, BX_IA_SHR_EwIb },
  /* 6 */ { 0, BX_IA_SHL_EwIb },
  /* 7 */ { 0, BX_IA_SAR_EwIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2EdIb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EdIb },
  /* 1 */ { 0, BX_IA_ROR_EdIb },
  /* 2 */ { 0, BX_IA_RCL_EdIb },
  /* 3 */ { 0, BX_IA_RCR_EdIb },
  /* 4 */ { 0, BX_IA_SHL_EdIb },
  /* 5 */ { 0, BX_IA_SHR_EdIb },
  /* 6 */ { 0, BX_IA_SHL_EdIb },
  /* 7 */ { 0, BX_IA_SAR_EdIb }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G2EqIb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EqIb },
  /* 1 */ { 0, BX_IA_ROR_EqIb },
  /* 2 */ { 0, BX_IA_RCL_EqIb },
  /* 3 */ { 0, BX_IA_RCR_EqIb },
  /* 4 */ { 0, BX_IA_SHL_EqIb },
  /* 5 */ { 0, BX_IA_SHR_EqIb },
  /* 6 */ { 0, BX_IA_SHL_EqIb },
  /* 7 */ { 0, BX_IA_SAR_EqIb }
};
#endif

/* ******* */
/* Group 3 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG3Eb[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_TEST_EbIb },
  /* 1 */ { BxImmediate_Ib, BX_IA_TEST_EbIb },
  /* 2 */ { BxLockable,     BX_IA_NOT_Eb },
  /* 3 */ { BxLockable,     BX_IA_NEG_Eb },
  /* 4 */ { 0,              BX_IA_MUL_ALEb },
  /* 5 */ { 0,              BX_IA_IMUL_ALEb },
  /* 6 */ { 0,              BX_IA_DIV_ALEb },
  /* 7 */ { 0,              BX_IA_IDIV_ALEb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3Ew[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_TEST_EwIw },
  /* 1 */ { BxImmediate_Iw, BX_IA_TEST_EwIw },
  /* 2 */ { BxLockable,     BX_IA_NOT_Ew },
  /* 3 */ { BxLockable,     BX_IA_NEG_Ew },
  /* 4 */ { 0,              BX_IA_MUL_AXEw },
  /* 5 */ { 0,              BX_IA_IMUL_AXEw },
  /* 6 */ { 0,              BX_IA_DIV_AXEw },
  /* 7 */ { 0,              BX_IA_IDIV_AXEw }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3Ed[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EdId },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EdId },
  /* 2 */ { BxLockable,     BX_IA_NOT_Ed },
  /* 3 */ { BxLockable,     BX_IA_NEG_Ed },
  /* 4 */ { 0,              BX_IA_MUL_EAXEd },
  /* 5 */ { 0,              BX_IA_IMUL_EAXEd },
  /* 6 */ { 0,              BX_IA_DIV_EAXEd },
  /* 7 */ { 0,              BX_IA_IDIV_EAXEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G3Eq[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EqId },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EqId },
  /* 2 */ { BxLockable,     BX_IA_NOT_Eq },
  /* 3 */ { BxLockable,     BX_IA_NEG_Eq },
  /* 4 */ { 0,              BX_IA_MUL_RAXEq },
  /* 5 */ { 0,              BX_IA_IMUL_RAXEq },
  /* 6 */ { 0,              BX_IA_DIV_RAXEq },
  /* 7 */ { 0,              BX_IA_IDIV_RAXEq }
};
#endif

/* ******* */
/* Group 4 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG4[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_Eb },
  /* 1 */ { BxLockable, BX_IA_DEC_Eb },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 5 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG5w[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_Ew },
  /* 1 */ { BxLockable, BX_IA_DEC_Ew },
  /* 2 */ { 0, BX_IA_CALL_Ew },
  /* 3 */ { 0, BX_IA_CALLF_Op16_Ep },
  /* 4 */ { 0, BX_IA_JMP_Ew },
  /* 5 */ { 0, BX_IA_JMPF_Op16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_Ew },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5d[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_Ed },
  /* 1 */ { BxLockable, BX_IA_DEC_Ed },
  /* 2 */ { 0, BX_IA_CALL_Ed },
  /* 3 */ { 0, BX_IA_CALLF_Op32_Ep },
  /* 4 */ { 0, BX_IA_JMP_Ed },
  /* 5 */ { 0, BX_IA_JMPF_Op32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_Ed },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G5w[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_Ew },
  /* 1 */ { BxLockable, BX_IA_DEC_Ew },
  /* 2 */ { 0, BX_IA_CALL_Eq },
  /* 3 */ { 0, BX_IA_CALLF_Op16_Ep },
  /* 4 */ { 0, BX_IA_JMP_Eq },
  /* 5 */ { 0, BX_IA_JMPF_Op16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_Ew },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5d[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_Ed },
  /* 1 */ { BxLockable, BX_IA_DEC_Ed },
  /* 2 */ { 0, BX_IA_CALL_Eq },
  /* 3 */ { 0, BX_IA_CALLF_Op32_Ep },
  /* 4 */ { 0, BX_IA_JMP_Eq },
  /* 5 */ { 0, BX_IA_JMPF_Op32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_Eq },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5q[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_Eq },
  /* 1 */ { BxLockable, BX_IA_DEC_Eq },
  /* 2 */ { 0, BX_IA_CALL_Eq },
  /* 3 */ { 0, BX_IA_CALLF_Op64_Ep }, // TODO: 64-bit offset for Intel
  /* 4 */ { 0, BX_IA_JMP_Eq },
  /* 5 */ { 0, BX_IA_JMPF_Op64_Ep },  // TODO: 64-bit offset for Intel
  /* 6 */ { 0, BX_IA_PUSH_Eq },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 6 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG6[8] = {
  /* 0 */ { 0, BX_IA_SLDT_Ew },
  /* 1 */ { 0, BX_IA_STR_Ew },
  /* 2 */ { 0, BX_IA_LLDT_Ew },
  /* 3 */ { 0, BX_IA_LTR_Ew },
  /* 4 */ { 0, BX_IA_VERR_Ew },
  /* 5 */ { 0, BX_IA_VERW_Ew },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 7 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG7[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_SGDT_Ms },
  /* 1 */ { 0, BX_IA_SIDT_Ms },
  /* 2 */ { 0, BX_IA_LGDT_Ms },
  /* 3 */ { 0, BX_IA_LIDT_Ms },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_LMSW_Ew },
  /* 7 */ { 0, BX_IA_INVLPG },

  /* /r form */
  /* 0F 01 C0 */ { 0, BX_IA_ERROR },
  /* 0F 01 C1 */ { BxPrefixSSE, BX_IA_VMCALL, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C2 */ { BxPrefixSSE, BX_IA_VMLAUNCH, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C3 */ { BxPrefixSSE, BX_IA_VMRESUME, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C4 */ { BxPrefixSSE, BX_IA_VMXOFF, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C5 */ { 0, BX_IA_ERROR },
  /* 0F 01 C6 */ { 0, BX_IA_ERROR },
  /* 0F 01 C7 */ { 0, BX_IA_ERROR },
  /* 0F 01 C8 */ { BxPrefixSSE, BX_IA_MONITOR, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C9 */ { BxPrefixSSE, BX_IA_MWAIT, BxOpcodeGroupSSE_ERR },
  /* 0F 01 CA */ { 0, BX_IA_CLAC },
  /* 0F 01 CB */ { 0, BX_IA_STAC },
  /* 0F 01 CC */ { 0, BX_IA_ERROR },
  /* 0F 01 CD */ { 0, BX_IA_ERROR },
  /* 0F 01 CE */ { 0, BX_IA_ERROR },
  /* 0F 01 CF */ { 0, BX_IA_ERROR },
  /* 0F 01 D0 */ { BxPrefixSSE, BX_IA_XGETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D1 */ { BxPrefixSSE, BX_IA_XSETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D2 */ { 0, BX_IA_ERROR },
  /* 0F 01 D3 */ { 0, BX_IA_ERROR },
  /* 0F 01 D4 */ { BxPrefixSSE, BX_IA_VMFUNC, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D5 */ { 0, BX_IA_ERROR },
  /* 0F 01 D6 */ { 0, BX_IA_ERROR },
  /* 0F 01 D7 */ { 0, BX_IA_ERROR },
  /* 0F 01 D8 */ { 0, BX_IA_VMRUN },
  /* 0F 01 D9 */ { 0, BX_IA_VMMCALL },
  /* 0F 01 DA */ { 0, BX_IA_VMLOAD },
  /* 0F 01 DB */ { 0, BX_IA_VMSAVE },
  /* 0F 01 DC */ { 0, BX_IA_STGI },
  /* 0F 01 DD */ { 0, BX_IA_CLGI },
  /* 0F 01 DE */ { 0, BX_IA_SKINIT },
  /* 0F 01 DF */ { 0, BX_IA_INVLPGA },
  /* 0F 01 E0 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E1 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E2 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E3 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E4 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E5 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E6 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E7 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E8 */ { 0, BX_IA_ERROR },
  /* 0F 01 E9 */ { 0, BX_IA_ERROR },
  /* 0F 01 EA */ { 0, BX_IA_ERROR },
  /* 0F 01 EB */ { 0, BX_IA_ERROR },
  /* 0F 01 EC */ { 0, BX_IA_ERROR },
  /* 0F 01 ED */ { 0, BX_IA_ERROR },
  /* 0F 01 EE */ { 0, BX_IA_ERROR },
  /* 0F 01 EF */ { 0, BX_IA_ERROR },
  /* 0F 01 F0 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F1 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F2 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F3 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F4 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F5 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F6 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F7 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F8 */ { 0, BX_IA_ERROR },
  /* 0F 01 F9 */ { 0, BX_IA_RDTSCP }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 01 FA */ { 0, BX_IA_ERROR },
  /* 0F 01 FB */ { 0, BX_IA_ERROR },
  /* 0F 01 FC */ { 0, BX_IA_ERROR },
  /* 0F 01 FD */ { 0, BX_IA_ERROR },
  /* 0F 01 FE */ { 0, BX_IA_ERROR },
  /* 0F 01 FF */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfoG7q[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_SGDT_Op64_Ms },
  /* 1 */ { 0, BX_IA_SIDT_Op64_Ms },
  /* 2 */ { 0, BX_IA_LGDT_Op64_Ms },
  /* 3 */ { 0, BX_IA_LIDT_Op64_Ms },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_LMSW_Ew },
  /* 7 */ { 0, BX_IA_INVLPG },

  /* /r form */
  /* 0F 01 C0 */ { 0, BX_IA_ERROR },
  /* 0F 01 C1 */ { BxPrefixSSE, BX_IA_VMCALL, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C2 */ { BxPrefixSSE, BX_IA_VMLAUNCH, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C3 */ { BxPrefixSSE, BX_IA_VMRESUME, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C4 */ { BxPrefixSSE, BX_IA_VMXOFF, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C5 */ { 0, BX_IA_ERROR },
  /* 0F 01 C6 */ { 0, BX_IA_ERROR },
  /* 0F 01 C7 */ { 0, BX_IA_ERROR },
  /* 0F 01 C8 */ { BxPrefixSSE, BX_IA_MONITOR, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C9 */ { BxPrefixSSE, BX_IA_MWAIT, BxOpcodeGroupSSE_ERR },
  /* 0F 01 CA */ { 0, BX_IA_CLAC },
  /* 0F 01 CB */ { 0, BX_IA_STAC },
  /* 0F 01 CC */ { 0, BX_IA_ERROR },
  /* 0F 01 CD */ { 0, BX_IA_ERROR },
  /* 0F 01 CE */ { 0, BX_IA_ERROR },
  /* 0F 01 CF */ { 0, BX_IA_ERROR },
  /* 0F 01 D0 */ { BxPrefixSSE, BX_IA_XGETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D1 */ { BxPrefixSSE, BX_IA_XSETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D2 */ { 0, BX_IA_ERROR },
  /* 0F 01 D3 */ { 0, BX_IA_ERROR },
  /* 0F 01 D4 */ { BxPrefixSSE, BX_IA_VMFUNC, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D5 */ { 0, BX_IA_ERROR },
  /* 0F 01 D6 */ { 0, BX_IA_ERROR },
  /* 0F 01 D7 */ { 0, BX_IA_ERROR },
  /* 0F 01 D8 */ { 0, BX_IA_VMRUN },
  /* 0F 01 D9 */ { 0, BX_IA_VMMCALL },
  /* 0F 01 DA */ { 0, BX_IA_VMLOAD },
  /* 0F 01 DB */ { 0, BX_IA_VMSAVE },
  /* 0F 01 DC */ { 0, BX_IA_STGI },
  /* 0F 01 DD */ { 0, BX_IA_CLGI },
  /* 0F 01 DE */ { 0, BX_IA_SKINIT },
  /* 0F 01 DF */ { 0, BX_IA_INVLPGA },
  /* 0F 01 E0 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E1 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E2 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E3 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E4 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E5 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E6 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E7 */ { 0, BX_IA_SMSW_Ew },
  /* 0F 01 E8 */ { 0, BX_IA_ERROR },
  /* 0F 01 E9 */ { 0, BX_IA_ERROR },
  /* 0F 01 EA */ { 0, BX_IA_ERROR },
  /* 0F 01 EB */ { 0, BX_IA_ERROR },
  /* 0F 01 EC */ { 0, BX_IA_ERROR },
  /* 0F 01 ED */ { 0, BX_IA_ERROR },
  /* 0F 01 EE */ { 0, BX_IA_ERROR },
  /* 0F 01 EF */ { 0, BX_IA_ERROR },
  /* 0F 01 F0 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F1 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F2 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F3 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F4 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F5 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F6 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F7 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F8 */ { 0, BX_IA_SWAPGS },
  /* 0F 01 F9 */ { 0, BX_IA_RDTSCP }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 01 FA */ { 0, BX_IA_ERROR },
  /* 0F 01 FB */ { 0, BX_IA_ERROR },
  /* 0F 01 FC */ { 0, BX_IA_ERROR },
  /* 0F 01 FD */ { 0, BX_IA_ERROR },
  /* 0F 01 FE */ { 0, BX_IA_ERROR },
  /* 0F 01 FF */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 8 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIb[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EwIb },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EwIb },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EwIb },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EwIb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIb[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EdIb },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EdIb },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EdIb },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EdIb }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIb[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EqIb },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EqIb },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EqIb },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EqIb }
};
#endif

/* ******* */
/* Group 9 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG9w[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Ew, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Ew, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG9d[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Ed, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Ed, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G9q[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Eq, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Eq, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG16B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};
#endif

/* ******** */
/* Group 11 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG11Eb[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11Ew[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11Ed[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G11Eq[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EqId },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1202 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1204 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1206 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG13R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1302 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1304 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1306 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1402 },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_PSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_G1406 },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_PSLLDQ_UdqIb }
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeGroupSSE_CLFLUSH[2] = {
  /* -- */ { 0, BX_IA_CLFLUSH },
  /* 66 */ { 0, BX_IA_CLFLUSHOPT }
};

static const BxOpcodeInfo_t BxOpcodeInfoG15[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { BxPrefixSSE, BX_IA_LFENCE, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_MFENCE, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE, BX_IA_SFENCE, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { BxPrefixSSE, BX_IA_FXSAVE,   BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSE, BX_IA_FXRSTOR,  BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_LDMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_STMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVE,    BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XRSTOR,   BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_XSAVEOPT, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE2, BX_IA_ERROR,  BxOpcodeGroupSSE_CLFLUSH }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G15d[8*2] = {
  /* /r form */
  /* 0 */ { BxPrefixSSEF3, BX_IA_RDFSBASE_Ed },
  /* 1 */ { BxPrefixSSEF3, BX_IA_RDGSBASE_Ed },
  /* 2 */ { BxPrefixSSEF3, BX_IA_WRFSBASE_Ed },
  /* 3 */ { BxPrefixSSEF3, BX_IA_WRGSBASE_Ed },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { BxPrefixSSE, BX_IA_LFENCE, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_MFENCE, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE, BX_IA_SFENCE, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { BxPrefixSSE, BX_IA_FXSAVE,   BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSE, BX_IA_FXRSTOR,  BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_LDMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_STMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVE,    BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XRSTOR,   BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_XSAVEOPT, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE2, BX_IA_ERROR,  BxOpcodeGroupSSE_CLFLUSH }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G15q[8*2] = {
  /* /r form */
  /* 0 */ { BxPrefixSSEF3, BX_IA_RDFSBASE_Eq },
  /* 1 */ { BxPrefixSSEF3, BX_IA_RDGSBASE_Eq },
  /* 2 */ { BxPrefixSSEF3, BX_IA_WRFSBASE_Eq },
  /* 3 */ { BxPrefixSSEF3, BX_IA_WRGSBASE_Eq },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { BxPrefixSSE, BX_IA_LFENCE, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_MFENCE, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE, BX_IA_SFENCE, BxOpcodeGroupSSE_ERR },

  /* /m form */
  /* 0 */ { BxPrefixSSE, BX_IA_FXSAVE,   BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSE, BX_IA_FXRSTOR,  BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_LDMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_STMXCSR,  BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVE,    BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XRSTOR,   BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_XSAVEOPT, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSE2, BX_IA_ERROR,  BxOpcodeGroupSSE_CLFLUSH }
};
#endif

/* ******** */
/* Group 16 */
/* ******** */

#if BX_CPU_LEVEL >= 6
static const BxOpcodeInfo_t BxOpcodeInfoG16[8] = {
  /* 0 */ { 0, BX_IA_PREFETCHNTA },
  /* 1 */ { 0, BX_IA_PREFETCHT0 },
  /* 2 */ { 0, BX_IA_PREFETCHT1 },
  /* 3 */ { 0, BX_IA_PREFETCHT2 },
  /* 4 */ { 0, BX_IA_PREFETCH },
  /* 5 */ { 0, BX_IA_PREFETCH },
  /* 6 */ { 0, BX_IA_PREFETCH },
  /* 7 */ { 0, BX_IA_PREFETCH }
};
#endif

static const BxOpcodeInfo_t BxOpcodeInfoMOV_RdCd[8] = {
  /* 0 */ { 0, BX_IA_MOV_RdCR0 },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_MOV_RdCR2 },
  /* 3 */ { 0, BX_IA_MOV_RdCR3 },
  /* 4 */ { 0, BX_IA_MOV_RdCR4 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoMOV_CdRd[8] = {
  /* 0 */ { 0, BX_IA_MOV_CR0Rd },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_MOV_CR2Rd },
  /* 3 */ { 0, BX_IA_MOV_CR3Rd },
  /* 4 */ { 0, BX_IA_MOV_CR4Rd },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfoMOV_RqCq[8] = {
  /* 0 */ { 0, BX_IA_MOV_RqCR0 },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_MOV_RqCR2 },
  /* 3 */ { 0, BX_IA_MOV_RqCR3 },
  /* 4 */ { 0, BX_IA_MOV_RqCR4 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoMOV_CqRq[8] = {
  /* 0 */ { 0, BX_IA_MOV_CR0Rq },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_MOV_CR2Rq },
  /* 3 */ { 0, BX_IA_MOV_CR3Rq },
  /* 4 */ { 0, BX_IA_MOV_CR4Rq },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

#endif // BX_COMMON_FETCHDECODE_TABLES_H
