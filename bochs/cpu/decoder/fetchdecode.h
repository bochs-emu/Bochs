/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2026 Stanislav Shwartsman
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
  BX_EVEX_ILLEGAL_GPR_REGISTER,
  BX_EVEX_ILLEGAL_ZERO_MASKING_WITH_KMASK_SRC_OR_DEST,
  BX_EVEX_ILLEGAL_ZERO_MASKING_VSIB,
  BX_EVEX_ILLEGAL_ZERO_MASKING_MEMORY_DESTINATION,
  BX_AMX_ILLEGAL_TILE_REGISTER,
  BX_AMX_ILLEGAL_BSR_REGISTER,
  BX_AMX_SIB_REQUIRED
};

// APX state of EVEX instruction decode (assign_srcs)
enum BxApxDecode {
  BX_APX_DISABLED = 0, // APX is not enabled: extended GPRs cannot be encoded in EVEX GPR operands
  BX_APX_ENABLED,      // APX is enabled, regular EVEX instruction: extended GPRs allowed
  BX_APX_PROMOTED      // APX is enabled, legacy/VEX instruction promoted to EVEX: extended GPRs allowed,
                       // EVEX.aaa[1:0] are reserved (except CCMP/CTEST where aaa is source condition code)
};

//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

#define FetchDWORD(iptr) ReadHostDWordFromLittleEndian((Bit32u*)(iptr))
#define FetchWORD(iptr) ReadHostWordFromLittleEndian((Bit16u*)(iptr))
#define FetchQWORD(iptr) ReadHostQWordFromLittleEndian((Bit64u*)(iptr));

#if BX_SUPPORT_MPX
#define BX_MPX_BNDREGS_CLEAR         (0x2000)
#else
#define BX_MPX_BNDREGS_CLEAR         (0)
#endif
#define BX_PREPARE_APX               (0x1000)
#define BX_PREPARE_SCALEDATA         (0x800 | BX_PREPARE_AMX)
#define BX_PREPARE_AMX               (0x400)
#define BX_PREPARE_EVEX_NO_BROADCAST (0x200 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX_NO_SAE       (0x100 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX              (0x80)
#define BX_PREPARE_OPMASK            (BX_PREPARE_EVEX)
#define BX_PREPARE_AVX               (0x40)
#define BX_PREPARE_SSE               (0x20)
#define BX_PREPARE_MMX               (0x10)
#define BX_PREPARE_FPU               (0x08)
#define BX_PROTECTED_MODE_ONLY       (0x04)
#define BX_LOCKABLE                  (0x02)
#define BX_TRACE_END                 (0x01)

struct bxIAOpcodeTable {
#ifndef BX_STANDALONE_DECODER
  BxExecutePtr_tR execute1;
  BxExecutePtr_tR execute2;
#endif
  Bit8u src[4];
#if BX_SUPPORT_EVEX || BX_SUPPORT_AMX || BX_SUPPORT_MPX
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

// where the source should be taken from (3 bits). Immediates/branch-offsets/
// implicit refs fold into BX_SRC_NONE and are told apart by type value
// instead (below) -- frees a bit for the type field without growing the byte.
enum {
  BX_SRC_NONE = 0,          // no source / immediate / branch offset / implicit ref (see type field)
  BX_SRC_EAX = 1,           // the src is AL/AX/EAX/RAX or ST(0) for x87
  BX_SRC_NNN = 2,           // the src should be taken from modrm.nnn
  BX_SRC_RM = 3,            // the src is register or memory reference, register should be taken from modrm.rm
  BX_SRC_VECTOR_RM = 4,     // the src is register or EVEX memory reference, register should be taken from modrm.rm
  BX_SRC_VVV = 5,           // the src should be taken from (e)vex.vvv
  BX_SRC_VIB = 6,           // the src should be taken from immediate byte
  BX_SRC_VSIB = 7,          // the src is gather/scatter vector index
};

// for disasm: register type/size when source is a register, or memory-access
// size hint when it's a memory reference. 5 bits (0-31); 21 used, 11 free.
enum {
  BX_NO_REGISTER = 0,
  BX_GPR8 = 0x1,
  BX_GPR16 = 0x2,
  BX_GPR32 = 0x3,
  BX_GPR64 = 0x4,
  BX_BOUND_REG = 0x5,
  BX_FPU_REG = 0x6,
  BX_MMX_REG = 0x7,
  BX_MMX_HALF_REG = 0x8,
  BX_VMM_REG = 0x9,
  BX_KMASK_REG = 0xA,       // opmask register or qword memory reference (KMOVQ)
  BX_KMASK_REG_B = 0xB,     // opmask register or byte memory reference (KMOVB)
  BX_KMASK_REG_W = 0xC,     // opmask register or word memory reference (KMOVW)
  BX_KMASK_REG_D = 0xD,     // opmask register or dword memory reference (KMOVD)
  BX_KMASK_REG_PAIR = 0xE,
  BX_TMM_REG = 0xF,
  BX_SEGREG = 0x10,
  BX_CREG = 0x11,
  BX_DREG = 0x12,
  BX_BSR_REG = 0x13,        // ACE v1 Block Scale Register (BSRMOVF/BSRMOVL/BSRMOVH/BSRINIT)
  BX_SIB_MEMORY = 0x14,     // memory reference which must be encoded with SIB byte (AMX TILELOADD/TILESTORED)
  BX_DFV = 0x15             // APX
  // 0x16-0x1F: 10 more free slots
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

// type-field values when origin == BX_SRC_NONE (5 bits, 0-31): immediate,
// branch-offset, and implicit-ref forms share this space now, explicitly
// numbered so used-vs-free is a glance.
enum {
  BX_NONE_ABSENT = 0,   // OP_NONE: no operand at all

  BX_IMM1 = 1,
  BX_IMMB = 2,
  BX_IMMBW_SE = 3,
  BX_IMMBD_SE = 4,
  BX_IMMDQ_SE = 5,
  BX_IMMW = 6,
  BX_IMMD = 7,
  BX_IMMQ = 8,
  BX_IMMB2 = 9,
  BX_DIRECT_PTR = 10,
  BX_DIRECT_MEMREF_B = 11,
  BX_DIRECT_MEMREF_W = 12,
  BX_DIRECT_MEMREF_D = 13,
  BX_DIRECT_MEMREF_Q = 14,

  BX_JIMMBW_SE = 15,    // same widths as BX_IMMBW_SE.. above, own codes since
  BX_JIMMBD_SE = 16,    // origin no longer separates plain imm from branch imm
  BX_JIMMW = 17,
  BX_JIMMD = 18,

  BX_RSIREF_B = 19,
  BX_RSIREF_W = 20,
  BX_RSIREF_D = 21,
  BX_RSIREF_Q = 22,
  BX_RDIREF_B = 23,
  BX_RDIREF_W = 24,
  BX_RDIREF_D = 25,
  BX_RDIREF_Q = 26,
  BX_MMX_RDIREF = 27,
  BX_VEC_RDIREF = 28,
  BX_USECL = 29,
  BX_USEDX = 30,
  // 31 is free
};

#define BX_FORM_SRC(type, src) (((type) << 3) | (src))

#define BX_DISASM_SRC_ORIGIN(desc) ((desc) & 0x7)
#define BX_DISASM_SRC_TYPE(desc) ((desc) >> 3)

// opmask register source type (byte/word/dword/qword memory reference size)
#define BX_IS_KMASK_REG(type) ((type) >= BX_KMASK_REG && (type) <= BX_KMASK_REG_D)

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

const Bit8u OP_CLReg  = BX_FORM_SRC(BX_USECL, BX_SRC_NONE);
const Bit8u OP_DXReg  = BX_FORM_SRC(BX_USEDX, BX_SRC_NONE);

const Bit8u OP_I1 = BX_FORM_SRC(BX_IMM1, BX_SRC_NONE);
const Bit8u OP_Ib = BX_FORM_SRC(BX_IMMB, BX_SRC_NONE);
const Bit8u OP_sIbw = BX_FORM_SRC(BX_IMMBW_SE, BX_SRC_NONE);
const Bit8u OP_sIbd = BX_FORM_SRC(BX_IMMBD_SE, BX_SRC_NONE);
const Bit8u OP_Iw = BX_FORM_SRC(BX_IMMW, BX_SRC_NONE);
const Bit8u OP_Id = BX_FORM_SRC(BX_IMMD, BX_SRC_NONE);
const Bit8u OP_sId = BX_FORM_SRC(BX_IMMDQ_SE, BX_SRC_NONE);
const Bit8u OP_Iq = BX_FORM_SRC(BX_IMMQ, BX_SRC_NONE);
const Bit8u OP_Ib2 = BX_FORM_SRC(BX_IMMB2, BX_SRC_NONE);

const Bit8u OP_Jw = BX_FORM_SRC(BX_JIMMW, BX_SRC_NONE);
const Bit8u OP_Jd = BX_FORM_SRC(BX_JIMMD, BX_SRC_NONE);
const Bit8u OP_Jq = BX_FORM_SRC(BX_JIMMD, BX_SRC_NONE);

const Bit8u OP_Jbw = BX_FORM_SRC(BX_JIMMBW_SE, BX_SRC_NONE);
const Bit8u OP_Jbd = BX_FORM_SRC(BX_JIMMBD_SE, BX_SRC_NONE);
const Bit8u OP_Jbq = BX_FORM_SRC(BX_JIMMBD_SE, BX_SRC_NONE);

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

const Bit8u OP_Bb = BX_FORM_SRC(BX_GPR8, BX_SRC_VVV);
const Bit8u OP_Bw = BX_FORM_SRC(BX_GPR16, BX_SRC_VVV);
const Bit8u OP_Bd = BX_FORM_SRC(BX_GPR32, BX_SRC_VVV);
const Bit8u OP_Bq = BX_FORM_SRC(BX_GPR64, BX_SRC_VVV);

const Bit8u OP_DFV = BX_FORM_SRC(BX_DFV, BX_SRC_VVV);

const Bit8u OP_VIb = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VIB);

const Bit8u OP_Cd = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Cq = BX_FORM_SRC(BX_CREG, BX_SRC_NNN);
const Bit8u OP_Dd = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);
const Bit8u OP_Dq = BX_FORM_SRC(BX_DREG, BX_SRC_NNN);

const Bit8u OP_Sw = BX_FORM_SRC(BX_SEGREG, BX_SRC_NNN);

const Bit8u OP_Ob = BX_FORM_SRC(BX_DIRECT_MEMREF_B, BX_SRC_NONE);
const Bit8u OP_Ow = BX_FORM_SRC(BX_DIRECT_MEMREF_W, BX_SRC_NONE);
const Bit8u OP_Od = BX_FORM_SRC(BX_DIRECT_MEMREF_D, BX_SRC_NONE);
const Bit8u OP_Oq = BX_FORM_SRC(BX_DIRECT_MEMREF_Q, BX_SRC_NONE);

const Bit8u OP_Ap = BX_FORM_SRC(BX_DIRECT_PTR, BX_SRC_NONE);

const Bit8u OP_NNN = BX_FORM_SRC(BX_NO_REGISTER, BX_SRC_NNN);
const Bit8u OP_BsrVdq = BX_FORM_SRC(BX_BSR_REG, BX_SRC_NNN);

const Bit8u OP_GBnd = BX_FORM_SRC(BX_BOUND_REG, BX_SRC_NNN);
const Bit8u OP_EBnd = BX_FORM_SRC(BX_BOUND_REG, BX_SRC_RM);

const Bit8u OP_KGb = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEb = BX_FORM_SRC(BX_KMASK_REG_B, BX_SRC_RM);
const Bit8u OP_KHb = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGw = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEw = BX_FORM_SRC(BX_KMASK_REG_W, BX_SRC_RM);
const Bit8u OP_KHw = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGd = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEd = BX_FORM_SRC(BX_KMASK_REG_D, BX_SRC_RM);
const Bit8u OP_KHd = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_NNN);
const Bit8u OP_KEq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_RM);
const Bit8u OP_KHq = BX_FORM_SRC(BX_KMASK_REG, BX_SRC_VVV);

const Bit8u OP_KGq2 = BX_FORM_SRC(BX_KMASK_REG_PAIR, BX_SRC_NNN);

const Bit8u OP_Trm  = BX_FORM_SRC(BX_TMM_REG, BX_SRC_RM);
const Bit8u OP_Tnnn = BX_FORM_SRC(BX_TMM_REG, BX_SRC_NNN);
const Bit8u OP_Treg = BX_FORM_SRC(BX_TMM_REG, BX_SRC_VVV);

const Bit8u OP_Msib = BX_FORM_SRC(BX_SIB_MEMORY, BX_SRC_RM);

const Bit8u OP_ST0 = BX_FORM_SRC(BX_FPU_REG, BX_SRC_EAX);
const Bit8u OP_STi = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);

const Bit8u OP_Xb = BX_FORM_SRC(BX_RSIREF_B, BX_SRC_NONE);
const Bit8u OP_Xw = BX_FORM_SRC(BX_RSIREF_W, BX_SRC_NONE);
const Bit8u OP_Xd = BX_FORM_SRC(BX_RSIREF_D, BX_SRC_NONE);
const Bit8u OP_Xq = BX_FORM_SRC(BX_RSIREF_Q, BX_SRC_NONE);

const Bit8u OP_Yb = BX_FORM_SRC(BX_RDIREF_B, BX_SRC_NONE);
const Bit8u OP_Yw = BX_FORM_SRC(BX_RDIREF_W, BX_SRC_NONE);
const Bit8u OP_Yd = BX_FORM_SRC(BX_RDIREF_D, BX_SRC_NONE);
const Bit8u OP_Yq = BX_FORM_SRC(BX_RDIREF_Q, BX_SRC_NONE);

const Bit8u OP_sYq  = BX_FORM_SRC(BX_MMX_RDIREF, BX_SRC_NONE);
const Bit8u OP_sYdq = BX_FORM_SRC(BX_VEC_RDIREF, BX_SRC_NONE);

struct bx_modrm {
  unsigned modrm, mod, nnn, rm;
};

#include "ia_opcodes.h"

// New Opcode Tables

/*
2222 1111 1111 11
3210 9876 5432 1098 7654 3210
-----------------------------
OOAA SSLM I.ER VVVM  RRR SNNN
SSSS SSoo S VE EEEA ---- R
6363 EEcd 6 EX XXXS EEEE C
4242 PPkC 4 X2 ...K VVVV =
 / / PR 0   .  VVWK EEEE D
 1 1 EE     B  LL 0 XXXX S
 6 6 FF        50   .... T
     II        1/   SSSS
     XX        21   CCCC
                    3210
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
const unsigned EVEX_B_OFFSET = 13;
const unsigned APX_REX2_OFFSET = 12;  // REX2 prefix or APX instruction promoted to EVEX
const unsigned VEX_VL_512_OFFSET = 11;
const unsigned VEX_VL_128_256_OFFSET = 10;
const unsigned VEX_W_OFFSET = 9;
const unsigned MASK_K0_OFFSET = 8;
const unsigned SRC_EQ_DST_OFFSET = 3;
const unsigned RRR_OFFSET = 4;
const unsigned APX_SCC_OFFSET = 4;
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

// REX2 prefix used or legacy/VEX instruction promoted to EVEX by APX (64-bit mode with APX enabled only)
const Bit64u ATTR_NO_APX_REX2 = ((BX_CONST64(0)<<APX_REX2_OFFSET) << 24) | (BX_CONST64(1)<<APX_REX2_OFFSET);
const Bit64u ATTR_APX_REX2    = ((BX_CONST64(1)<<APX_REX2_OFFSET) << 24) | (BX_CONST64(1)<<APX_REX2_OFFSET);

const Bit64u ATTR_VL128 =     ((BX_CONST64(0)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL256 =     ((BX_CONST64(1)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL512 =     ((BX_CONST64(3)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(3)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL256_512 = ((BX_CONST64(1)<<VEX_VL_128_256_OFFSET) << 24) | (BX_CONST64(1)<<VEX_VL_128_256_OFFSET);
const Bit64u ATTR_VL128_256 = ((BX_CONST64(0)<<VEX_VL_512_OFFSET)     << 24) | (BX_CONST64(1)<<VEX_VL_512_OFFSET);

const Bit64u ATTR_VEX_L0 = ATTR_VL128;

const Bit64u ATTR_VEX_W0 = ((BX_CONST64(0)<<VEX_W_OFFSET) << 24) | (BX_CONST64(1)<<VEX_W_OFFSET);
const Bit64u ATTR_VEX_W1 = ((BX_CONST64(1)<<VEX_W_OFFSET) << 24) | (BX_CONST64(1)<<VEX_W_OFFSET);

const Bit64u ATTR_EVEX_B0 = ((BX_CONST64(0)<<EVEX_B_OFFSET) << 24) | (BX_CONST64(1)<<EVEX_B_OFFSET);
const Bit64u ATTR_EVEX_B1 = ((BX_CONST64(1)<<EVEX_B_OFFSET) << 24) | (BX_CONST64(1)<<EVEX_B_OFFSET);

const Bit64u ATTR_APX_ND0 = ATTR_EVEX_B0;
const Bit64u ATTR_APX_ND1 = ATTR_EVEX_B1;

const Bit64u ATTR_MASK_K0 = ((BX_CONST64(1)<<MASK_K0_OFFSET) << 24) | (BX_CONST64(1)<<MASK_K0_OFFSET);
const Bit64u ATTR_MASK_REQUIRED = ((BX_CONST64(0)<<MASK_K0_OFFSET) << 24) | (BX_CONST64(1)<<MASK_K0_OFFSET);

const Bit64u ATTR_APX_NF0 = ATTR_MASK_K0;
const Bit64u ATTR_APX_NF1 = ATTR_MASK_REQUIRED;

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

// special case when both NNN0 and NNN1 can be accepted
const Bit64u ATTR_NNN0_1 = ((BX_CONST64(0)<<(NNN_OFFSET+1)) << 24) | (BX_CONST64(3)<<(NNN_OFFSET+1));

const Bit64u ATTR_APX_SCC_0 = ((BX_CONST64(0x0)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_1 = ((BX_CONST64(0x1)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_2 = ((BX_CONST64(0x2)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_3 = ((BX_CONST64(0x3)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_4 = ((BX_CONST64(0x4)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_5 = ((BX_CONST64(0x5)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_6 = ((BX_CONST64(0x6)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_7 = ((BX_CONST64(0x7)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_8 = ((BX_CONST64(0x8)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_9 = ((BX_CONST64(0x9)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_A = ((BX_CONST64(0xA)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_B = ((BX_CONST64(0xB)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_C = ((BX_CONST64(0xC)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_D = ((BX_CONST64(0xD)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_E = ((BX_CONST64(0xE)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);
const Bit64u ATTR_APX_SCC_F = ((BX_CONST64(0xF)<<RRR_OFFSET) << 24) | (BX_CONST64(0xF)<<RRR_OFFSET);

const Bit64u ATTR_LAST_OPCODE = BX_CONST64(0x8000000000000000);

#define form_opcode(attr, ia_opcode) 		        ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LOCK_PREFIX_NOT_ALLOWED)
#define last_opcode(attr, ia_opcode) 		        ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LOCK_PREFIX_NOT_ALLOWED | ATTR_LAST_OPCODE)

#define form_opcode_lockable(attr, ia_opcode)       ((attr) | (Bit64u(ia_opcode) << 48))
#define last_opcode_lockable(attr, ia_opcode)       ((attr) | (Bit64u(ia_opcode) << 48) | ATTR_LAST_OPCODE)

/* ************************************************************************ */
/* Opcode Groups */

static const Bit64u BxOpcodeGroup_ERR[] = { last_opcode(0, BX_IA_ERROR) };

#endif // BX_COMMON_FETCHDECODE_TABLES_H
