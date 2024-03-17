/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2019 Stanislav Shwartsman
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

//
// Metadata for decoder...
//

enum {
  SSE_PREFIX_NONE = 0,
  SSE_PREFIX_66   = 1,
  SSE_PREFIX_F3   = 2,
  SSE_PREFIX_F2   = 3
};

enum BxDecodeError {
  BX_DECODE_OK = 0,
  BX_ILLEGAL_OPCODE,
  BX_ILLEGAL_LOCK_PREFIX,
  BX_ILLEGAL_VEX_XOP_VVV,
  BX_ILLEGAL_VEX_XOP_WITH_SSE_PREFIX,
  BX_ILLEGAL_VEX_XOP_WITH_REX_PREFIX,
  BX_ILLEGAL_VEX_XOP_OPCODE_MAP,
  BX_VEX_XOP_BAD_VECTOR_LENGTH,
  BX_VSIB_FORBIDDEN_ASIZE16,
  BX_VSIB_ILLEGAL_SIB_INDEX,
  BX_EVEX_RESERVED_BITS_SET,
  BX_EVEX_ILLEGAL_EVEX_B_SAE_NOT_ALLOWED,
  BX_EVEX_ILLEGAL_EVEX_B_BROADCAST_NOT_ALLOWED,
  BX_EVEX_ILLEGAL_KMASK_REGISTER,
  BX_EVEX_ILLEGAL_ZERO_MASKING_WITH_KMASK_SRC_OR_DEST,
  BX_EVEX_ILLEGAL_ZERO_MASKING_VSIB,
  BX_EVEX_ILLEGAL_ZERO_MASKING_MEMORY_DESTINATION,
  BX_AMX_ILLEGAL_TILE_REGISTER
};

//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

BX_CPP_INLINE Bit16u FetchWORD(const Bit8u *iptr)
{
   return ReadHostWordFromLittleEndian((Bit16u*)iptr);
}

BX_CPP_INLINE Bit32u FetchDWORD(const Bit8u *iptr)
{
   return ReadHostDWordFromLittleEndian((Bit32u*)iptr);
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u FetchQWORD(const Bit8u *iptr)
{
   return ReadHostQWordFromLittleEndian((Bit64u*)iptr);
}
#endif

#define BX_PREPARE_AMX               (0x400)
#define BX_PREPARE_EVEX_NO_BROADCAST (0x200 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX_NO_SAE       (0x100 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX              (0x80)
#define BX_PREPARE_OPMASK            (0x40)
#define BX_PREPARE_AVX               (0x20)
#define BX_PREPARE_SSE               (0x10)
#define BX_PREPARE_MMX               (0x08)
#define BX_PREPARE_FPU               (0x04)
#define BX_LOCKABLE                  (0x02)
#define BX_TRACE_END                 (0x01)

struct bxIAOpcodeTable {
#ifndef BX_STANDALONE_DECODER
  BxExecutePtr_tR execute1;
  BxExecutePtr_tR execute2;
#endif
  Bit8u src[4];
#if BX_SUPPORT_EVEX || BX_SUPPORT_AMX
  Bit16u opflags;
#else
  Bit8u  opflags;
#endif
};

#ifdef BX_STANDALONE_DECODER
// disable all the logging for stand-alone decoder
#undef BX_INFO
#undef BX_DEBUG
#undef BX_ERROR
#undef BX_PANIC
#undef BX_FATAL
#undef BX_ASSERT

#define BX_INFO(x)
#define BX_DEBUG(x)
#define BX_ERROR(x)
#define BX_PANIC(x)
#define BX_FATAL(x)
#define BX_ASSERT(x)
#endif

// where the source should be taken from
enum {
  BX_SRC_NONE = 0,          // no source, implicit source or immediate
  BX_SRC_EAX = 1,           // the src is AL/AX/EAX/RAX or ST(0) for x87
  BX_SRC_NNN = 2,           // the src should be taken from modrm.nnn
  BX_SRC_RM = 3,            // the src is register or memory reference, register should be taken from modrm.rm
  BX_SRC_VECTOR_RM = 4,     // the src is register or EVEX memory reference, register should be taken from modrm.rm
  BX_SRC_VVV = 5,           // the src should be taken from (e)vex.vvv
  BX_SRC_VIB = 6,           // the src should be taken from immediate byte
  BX_SRC_VSIB = 7,          // the src is gather/scatter vector index
  BX_SRC_IMM = 8,           // the src is immediate value
  BX_SRC_BRANCH_OFFSET = 9, // the src is immediate value used as branch offset
  BX_SRC_IMPLICIT = 10,     // the src is implicit register or memory reference
};

// for disasm:
// when the source is register, indicates the register type and size
// when the source is memory reference, give hint about the memory access size
enum {
  BX_NO_REGISTER = 0,
  BX_GPR8 = 0x1,
  BX_GPR16 = 0x2,
  BX_GPR32 = 0x3,
  BX_GPR64 = 0x4,
  BX_FPU_REG = 0x5,
  BX_MMX_REG = 0x6,
  BX_MMX_HALF_REG = 0x7,
  BX_VMM_REG = 0x8,
  BX_KMASK_REG = 0x9,
  BX_KMASK_REG_PAIR = 0xA,
  BX_TMM_REG = 0xB,
  BX_SEGREG = 0xC,
  BX_CREG = 0xD,
  BX_DREG = 0xE
  // encoding 0xF is still free
};

// to be used together with BX_SRC_VECTOR_RM
enum {
  BX_VMM_FULL_VECTOR = 0x0,
  BX_VMM_FULL_VECTOR_W = 0x1,
  BX_VMM_SCALAR_BYTE = 0x2,
  BX_VMM_SCALAR_WORD = 0x3,
  BX_VMM_SCALAR_DWORD = 0x4,
  BX_VMM_SCALAR_QWORD = 0x5,
  BX_VMM_SCALAR = 0x6,
  BX_VMM_HALF_VECTOR = 0x7,
  BX_VMM_HALF_VECTOR_W = 0x8,
  BX_VMM_QUARTER_VECTOR = 0x9,
  BX_VMM_QUARTER_VECTOR_W = 0xA,
  BX_VMM_EIGHTH_VECTOR = 0xB,
  BX_VMM_VEC128 = 0xC,
  BX_VMM_VEC256 = 0xD
  // encodings 0xE to 0xF are still free
};

// immediate forms
enum {
  BX_IMM1 = 0x1,
  BX_IMMB = 0x2,
  BX_IMMBW_SE = 0x3,
  BX_IMMBD_SE = 0x4,
  BX_IMMW = 0x5,
  BX_IMMD = 0x6,
  BX_IMMQ = 0x7,
  BX_IMMB2 = 0x8,
  BX_DIRECT_PTR = 0x9,
  // encodings 0xA to 0xB are still free
  BX_DIRECT_MEMREF_B = 0xC,
  BX_DIRECT_MEMREF_W = 0xD,
  BX_DIRECT_MEMREF_D = 0xE,
  BX_DIRECT_MEMREF_Q = 0xF,
};

// implicit register or memory references
enum {
  BX_RSIREF_B = 0x1,
  BX_RSIREF_W = 0x2,
  BX_RSIREF_D = 0x3,
  BX_RSIREF_Q = 0x4,
  BX_RDIREF_B = 0x5,
  BX_RDIREF_W = 0x6,
  BX_RDIREF_D = 0x7,
  BX_RDIREF_Q = 0x8,
  BX_MMX_RDIREF = 0x9,
  BX_VEC_RDIREF = 0xA,
  BX_USECL = 0xB,
  BX_USEDX = 0xC,
  // encodings 0xD to 0xF are still free
};

#define BX_FORM_SRC(type, src) (((type) << 4) | (src))

#define BX_DISASM_SRC_ORIGIN(desc) (desc & 0xf)
#define BX_DISASM_SRC_TYPE(desc) (desc >> 4)

const Bit8u OP_NONE = BX_SRC_NONE;

const Bit8u OP_Eb = BX_FORM_SRC(BX_GPR8, BX_SRC_RM);
const Bit8u OP_Ew = BX_FORM_SRC(BX_GPR16, BX_SRC_RM);
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

const Bit8u OP_CLReg  = BX_FORM_SRC(BX_USECL, BX_SRC_IMPLICIT);
const Bit8u OP_DXReg  = BX_FORM_SRC(BX_USEDX, BX_SRC_IMPLICIT);

const Bit8u OP_I1 = BX_FORM_SRC(BX_IMM1, BX_SRC_IMM);
const Bit8u OP_Ib = BX_FORM_SRC(BX_IMMB, BX_SRC_IMM);
const Bit8u OP_sIbw = BX_FORM_SRC(BX_IMMBW_SE, BX_SRC_IMM);
const Bit8u OP_sIbd = BX_FORM_SRC(BX_IMMBD_SE, BX_SRC_IMM);
const Bit8u OP_Iw = BX_FORM_SRC(BX_IMMW, BX_SRC_IMM);
const Bit8u OP_Id = BX_FORM_SRC(BX_IMMD, BX_SRC_IMM);
const Bit8u OP_sId = BX_FORM_SRC(BX_IMMD, BX_SRC_IMM);
const Bit8u OP_Iq = BX_FORM_SRC(BX_IMMQ, BX_SRC_IMM);
const Bit8u OP_Ib2 = BX_FORM_SRC(BX_IMMB2, BX_SRC_IMM);

const Bit8u OP_Jw = BX_FORM_SRC(BX_IMMW, BX_SRC_BRANCH_OFFSET);
const Bit8u OP_Jd = BX_FORM_SRC(BX_IMMD, BX_SRC_BRANCH_OFFSET);
const Bit8u OP_Jq = BX_FORM_SRC(BX_IMMD, BX_SRC_BRANCH_OFFSET);

const Bit8u OP_Jbw = BX_FORM_SRC(BX_IMMBW_SE, BX_SRC_BRANCH_OFFSET);
const Bit8u OP_Jbd = BX_FORM_SRC(BX_IMMBD_SE, BX_SRC_BRANCH_OFFSET);
const Bit8u OP_Jbq = BX_FORM_SRC(BX_IMMBD_SE, BX_SRC_BRANCH_OFFSET);

const Bit8u OP_M  = BX_FORM_SRC(BX_NO_REGISTER, BX_SRC_RM);
const Bit8u OP_Mt = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);
const Bit8u OP_Mdq = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);

const Bit8u OP_Mb = OP_Eb;
const Bit8u OP_Mw = OP_Ew;
const Bit8u OP_Md = OP_Ed;
const Bit8u OP_Mq = OP_Eq;

const Bit8u OP_Pq = BX_FORM_SRC(BX_MMX_REG, BX_SRC_NNN);
const Bit8u OP_Qd = BX_FORM_SRC(BX_MMX_HALF_REG, BX_SRC_RM);
const Bit8u OP_Qq = BX_FORM_SRC(BX_MMX_REG, BX_SRC_RM);

const Bit8u OP_Vdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vqq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vbf16 = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vph = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vsh = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vss = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vsd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vq  = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);
const Bit8u OP_Vd  = BX_FORM_SRC(BX_VMM_REG, BX_SRC_NNN);

const Bit8u OP_Wq = BX_FORM_SRC(BX_VMM_SCALAR_QWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_Wd = BX_FORM_SRC(BX_VMM_SCALAR_DWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_Ww = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_VECTOR_RM);
const Bit8u OP_Wb = BX_FORM_SRC(BX_VMM_SCALAR_BYTE, BX_SRC_VECTOR_RM);
const Bit8u OP_Wdq = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_Wbf16 = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_Wph = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_Wps = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_Wpd = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_Wsh = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_VECTOR_RM);
const Bit8u OP_Wss = BX_FORM_SRC(BX_VMM_SCALAR_DWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_Wsd = BX_FORM_SRC(BX_VMM_SCALAR_QWORD, BX_SRC_VECTOR_RM);

const Bit8u OP_mVph = BX_FORM_SRC(BX_VMM_FULL_VECTOR_W, BX_SRC_VECTOR_RM);
const Bit8u OP_mVps = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVpd = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVph16 = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVps32 = BX_FORM_SRC(BX_VMM_SCALAR_DWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVpd64 = BX_FORM_SRC(BX_VMM_SCALAR_QWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVqq = BX_FORM_SRC(BX_VMM_FULL_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVsh = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVss = BX_FORM_SRC(BX_VMM_SCALAR_DWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVsd = BX_FORM_SRC(BX_VMM_SCALAR_QWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq8 = BX_FORM_SRC(BX_VMM_SCALAR_BYTE, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq16 = BX_FORM_SRC(BX_VMM_SCALAR_WORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq32 = BX_FORM_SRC(BX_VMM_SCALAR_DWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq64 = BX_FORM_SRC(BX_VMM_SCALAR_QWORD, BX_SRC_VECTOR_RM);
const Bit8u OP_mVHV = BX_FORM_SRC(BX_VMM_HALF_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVHVw = BX_FORM_SRC(BX_VMM_HALF_VECTOR_W, BX_SRC_VECTOR_RM);
const Bit8u OP_mVQV = BX_FORM_SRC(BX_VMM_QUARTER_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVQVw = BX_FORM_SRC(BX_VMM_QUARTER_VECTOR_W, BX_SRC_VECTOR_RM);
const Bit8u OP_mVOV = BX_FORM_SRC(BX_VMM_EIGHTH_VECTOR, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq128 = BX_FORM_SRC(BX_VMM_VEC128, BX_SRC_VECTOR_RM);
const Bit8u OP_mVdq256 = BX_FORM_SRC(BX_VMM_VEC256, BX_SRC_VECTOR_RM);

const Bit8u OP_VSib = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_VSIB);

const Bit8u OP_Hdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hph = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hsh = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hss = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hsd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);

const Bit8u OP_Bd = BX_FORM_SRC(BX_GPR32, BX_SRC_VVV);
const Bit8u OP_Bq = BX_FORM_SRC(BX_GPR64, BX_SRC_VVV);

const Bit8u OP_VIb = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VIB);

const Bit8u OP_Cd = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Cq = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Dd = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);
const Bit8u OP_Dq = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);

const Bit8u OP_Sw = BX_FORM_SRC(BX_SEGREG, BX_SRC_NNN);

const Bit8u OP_Ob = BX_FORM_SRC(BX_DIRECT_MEMREF_B, BX_SRC_IMM);
const Bit8u OP_Ow = BX_FORM_SRC(BX_DIRECT_MEMREF_W, BX_SRC_IMM);
const Bit8u OP_Od = BX_FORM_SRC(BX_DIRECT_MEMREF_D, BX_SRC_IMM);
const Bit8u OP_Oq = BX_FORM_SRC(BX_DIRECT_MEMREF_Q, BX_SRC_IMM);

const Bit8u OP_Ap = BX_FORM_SRC(BX_DIRECT_PTR, BX_SRC_IMM);

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

const Bit8u OP_KGq2 = BX_FORM_SRC(BX_KMASK_REG_PAIR, BX_SRC_NNN);

const Bit8u OP_Trm  = BX_FORM_SRC(BX_TMM_REG, BX_SRC_RM);
const Bit8u OP_Tnnn = BX_FORM_SRC(BX_TMM_REG, BX_SRC_NNN);
const Bit8u OP_Treg = BX_FORM_SRC(BX_TMM_REG, BX_SRC_VVV);

const Bit8u OP_ST0 = BX_FORM_SRC(BX_FPU_REG, BX_SRC_EAX);
const Bit8u OP_STi = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);

const Bit8u OP_Xb = BX_FORM_SRC(BX_RSIREF_B, BX_SRC_IMPLICIT);
const Bit8u OP_Xw = BX_FORM_SRC(BX_RSIREF_W, BX_SRC_IMPLICIT);
const Bit8u OP_Xd = BX_FORM_SRC(BX_RSIREF_D, BX_SRC_IMPLICIT);
const Bit8u OP_Xq = BX_FORM_SRC(BX_RSIREF_Q, BX_SRC_IMPLICIT);

const Bit8u OP_Yb = BX_FORM_SRC(BX_RDIREF_B, BX_SRC_IMPLICIT);
const Bit8u OP_Yw = BX_FORM_SRC(BX_RDIREF_W, BX_SRC_IMPLICIT);
const Bit8u OP_Yd = BX_FORM_SRC(BX_RDIREF_D, BX_SRC_IMPLICIT);
const Bit8u OP_Yq = BX_FORM_SRC(BX_RDIREF_Q, BX_SRC_IMPLICIT);

const Bit8u OP_sYq  = BX_FORM_SRC(BX_MMX_RDIREF, BX_SRC_IMPLICIT);
const Bit8u OP_sYdq = BX_FORM_SRC(BX_VEC_RDIREF, BX_SRC_IMPLICIT);

struct bx_modrm {
  unsigned modrm, mod, nnn, rm;
};

#include "ia_opcodes.h"

// New Opcode Tables

/*
2222 1111 1111 11
3210 9876 5432 1098 7654 3210
-----------------------------
OOAA SSLM IVEX VVVM SRRR  NNN
SSSS SSoo SEVO EEEA R
6363 EEcd 6XEP XXXS C
4242 PPkC 4 X  ...K =
 / / PR 0      VVWK D
 1 1 EE        LL 0 S
 6 6 FF        50   T
     II        1/
	 XX        21
*/

const unsigned OS64_OFFSET = 23;
const unsigned OS32_OFFSET = 22;
const unsigned AS64_OFFSET = 21;
const unsigned AS32_OFFSET = 20;
const unsigned SSE_PREFIX_F2_F3_OFFSET = 19;
const unsigned SSE_PREFIX_OFFSET = 18;
const unsigned LOCK_PREFIX_OFFSET = 17;
const unsigned MODC0_OFFSET = 16;
const unsigned IS64_OFFSET = 15;
const unsigned VEX_OFFSET = 14;
const unsigned EVEX_OFFSET = 13;
const unsigned XOP_OFFSET = 12;
const unsigned VEX_VL_512_OFFSET = 11;
const unsigned VEX_VL_128_256_OFFSET = 10;
const unsigned VEX_W_OFFSET = 9;
const unsigned MASK_K0_OFFSET = 8;
const unsigned SRC_EQ_DST_OFFSET = 7;
const unsigned RRR_OFFSET = 4;
const unsigned NNN_OFFSET = 0;

const Bit64u ATTR_OS64 = ((BX_CONST64(3)<<OS32_OFFSET) << 24) | (BX_CONST64(3)<<OS32_OFFSET);
const Bit64u ATTR_OS32 = ((BX_CONST64(1)<<OS32_OFFSET) << 24) | (BX_CONST64(3)<<OS32_OFFSET);
const Bit64u ATTR_OS16 = ((BX_CONST64(0)<<OS32_OFFSET) << 24) | (BX_CONST64(3)<<OS32_OFFSET);

const Bit64u ATTR_OS16_32 = ((BX_CONST64(0)<<OS64_OFFSET) << 24) | (BX_CONST64(1)<<OS64_OFFSET);
const Bit64u ATTR_OS32_64 = ((BX_CONST64(1)<<OS32_OFFSET) << 24) | (BX_CONST64(1)<<OS32_OFFSET);

const Bit64u ATTR_AS64 = ((BX_CONST64(3)<<AS32_OFFSET) << 24) | (BX_CONST64(3)<<AS32_OFFSET);
const Bit64u ATTR_AS32 = ((BX_CONST64(1)<<AS32_OFFSET) << 24) | (BX_CONST64(3)<<AS32_OFFSET);
const Bit64u ATTR_AS16 = ((BX_CONST64(0)<<AS32_OFFSET) << 24) | (BX_CONST64(3)<<AS32_OFFSET);

const Bit64u ATTR_AS16_32 = ((BX_CONST64(0)<<AS64_OFFSET) << 24) | (BX_CONST64(1)<<AS64_OFFSET);
const Bit64u ATTR_AS32_64 = ((BX_CONST64(1)<<AS32_OFFSET) << 24) | (BX_CONST64(1)<<AS32_OFFSET);

const Bit64u ATTR_IS32 = ((BX_CONST64(0)<<IS64_OFFSET) << 24) | (BX_CONST64(1)<<IS64_OFFSET);
const Bit64u ATTR_IS64 = ((BX_CONST64(1)<<IS64_OFFSET) << 24) | (BX_CONST64(1)<<IS64_OFFSET);

const Bit64u ATTR_SSE_NO_PREFIX = ((BX_CONST64(0)<<SSE_PREFIX_OFFSET) << 24) | (BX_CONST64(3)<<SSE_PREFIX_OFFSET);
const Bit64u ATTR_SSE_PREFIX_66 = ((BX_CONST64(1)<<SSE_PREFIX_OFFSET) << 24) | (BX_CONST64(3)<<SSE_PREFIX_OFFSET);
const Bit64u ATTR_SSE_PREFIX_F3 = ((BX_CONST64(2)<<SSE_PREFIX_OFFSET) << 24) | (BX_CONST64(3)<<SSE_PREFIX_OFFSET);
const Bit64u ATTR_SSE_PREFIX_F2 = ((BX_CONST64(3)<<SSE_PREFIX_OFFSET) << 24) | (BX_CONST64(3)<<SSE_PREFIX_OFFSET);

const Bit64u ATTR_NO_SSE_PREFIX_F2_F3 = ((BX_CONST64(0)<<SSE_PREFIX_F2_F3_OFFSET) << 24) | (BX_CONST64(1)<<SSE_PREFIX_F2_F3_OFFSET);

const Bit64u ATTR_LOCK_PREFIX_NOT_ALLOWED = ((BX_CONST64(0)<<LOCK_PREFIX_OFFSET) << 24) | (BX_CONST64(1)<<LOCK_PREFIX_OFFSET);
const Bit64u ATTR_LOCK = ((BX_CONST64(1)<<LOCK_PREFIX_OFFSET) << 24) | (BX_CONST64(1)<<LOCK_PREFIX_OFFSET);

const Bit64u ATTR_MODC0    = ((BX_CONST64(1)<<MODC0_OFFSET) << 24) | (BX_CONST64(1)<<MODC0_OFFSET);
const Bit64u ATTR_NO_MODC0 = ((BX_CONST64(0)<<MODC0_OFFSET) << 24) | (BX_CONST64(1)<<MODC0_OFFSET);

const Bit64u ATTR_MOD_REG = ATTR_MODC0;
const Bit64u ATTR_MOD_MEM = ATTR_NO_MODC0;

const Bit64u ATTR_VEX = ((BX_CONST64(1)<<VEX_OFFSET) << 24) | (BX_CONST64(1)<<VEX_OFFSET);
const Bit64u ATTR_EVEX = ((BX_CONST64(1)<<EVEX_OFFSET) << 24) | (BX_CONST64(1)<<EVEX_OFFSET);
const Bit64u ATTR_XOP = ((BX_CONST64(1)<<XOP_OFFSET) << 24) | (BX_CONST64(1)<<XOP_OFFSET);

const Bit64u ATTR_VL128 =     ((BX_CONST64(0)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL256 =     ((BX_CONST64(1)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL512 =     ((BX_CONST64(3)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL256_512 = ((BX_CONST64(1)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(1)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL128_256 = ((BX_CONST64(0)<<VEX_VL_512_OFFSET)     << 24) | (BX_CONST64(1)<<VEX_VL_512_OFFSET);

const Bit64u ATTR_VEX_L0 = ATTR_VL128;

const Bit64u ATTR_VEX_W0 = ((BX_CONST64(0)<<VEX_W_OFFSET) << 24) | (BX_CONST64(1)<<VEX_W_OFFSET);
const Bit64u ATTR_VEX_W1 = ((BX_CONST64(1)<<VEX_W_OFFSET) << 24) | (BX_CONST64(1)<<VEX_W_OFFSET);

const Bit64u ATTR_NO_VEX_EVEX_XOP = ((BX_CONST64(0)<<XOP_OFFSET) << 24) | (BX_CONST64(3)<<XOP_OFFSET);

const Bit64u ATTR_MASK_K0 = ((BX_CONST64(1)<<MASK_K0_OFFSET) << 24) | (BX_CONST64(1)<<MASK_K0_OFFSET);
const Bit64u ATTR_MASK_REQUIRED = ((BX_CONST64(0)<<MASK_K0_OFFSET) << 24) | (BX_CONST64(1)<<MASK_K0_OFFSET);

const Bit64u ATTR_SRC_EQ_DST = ATTR_MOD_REG | ((BX_CONST64(1)<<SRC_EQ_DST_OFFSET) << 24) | (BX_CONST64(1)<<SRC_EQ_DST_OFFSET);

const Bit64u ATTR_RRR0 = ((BX_CONST64(0)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR1 = ((BX_CONST64(1)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR2 = ((BX_CONST64(2)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR3 = ((BX_CONST64(3)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR4 = ((BX_CONST64(4)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR5 = ((BX_CONST64(5)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR6 = ((BX_CONST64(6)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);
const Bit64u ATTR_RRR7 = ((BX_CONST64(7)<<RRR_OFFSET) << 24) | (BX_CONST64(7)<<RRR_OFFSET);

const Bit64u ATTR_NNN0 = ((BX_CONST64(0)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN1 = ((BX_CONST64(1)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN2 = ((BX_CONST64(2)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN3 = ((BX_CONST64(3)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN4 = ((BX_CONST64(4)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN5 = ((BX_CONST64(5)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN6 = ((BX_CONST64(6)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);
const Bit64u ATTR_NNN7 = ((BX_CONST64(7)<<NNN_OFFSET) << 24) | (BX_CONST64(7)<<NNN_OFFSET);

const Bit64u ATTR_LAST_OPCODE = BX_CONST64(0x8000000000000000);

#define form_opcode(attr, ia_opcode) 		        ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LOCK_PREFIX_NOT_ALLOWED)
#define last_opcode(attr, ia_opcode) 		        ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LOCK_PREFIX_NOT_ALLOWED | ATTR_LAST_OPCODE)

#define form_opcode_lockable(attr, ia_opcode)       ((attr) | (Bit64u(ia_opcode) << 48))
#define last_opcode_lockable(attr, ia_opcode)       ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LAST_OPCODE)

/* ************************************************************************ */
/* Opcode Groups */

static const Bit64u BxOpcodeGroup_ERR[] = { last_opcode(0, BX_IA_ERROR) };

#endif // BX_COMMON_FETCHDECODE_TABLES_H
