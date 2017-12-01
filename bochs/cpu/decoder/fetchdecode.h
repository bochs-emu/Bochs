/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2017 Stanislav Shwartsman
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

#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F3   2
#define SSE_PREFIX_F2   3

// If the BxImmediate mask is set, the lowest 4 bits of the attribute
// specify which kinds of immediate data required by instruction.

#define BxImmediate         0x000f // bits 3..0: any immediate
#define BxImmediate_I1      0x0001 // imm8 = 1
#define BxImmediate_Ib      0x0002 // 8 bit
#define BxImmediate_Ib_SE   0x0003 // sign extend to operand size
#define BxImmediate_Iw      0x0004 // 16 bit
#define BxImmediate_Id      0x0005 // 32 bit
#define BxImmediate_O       0x0006 // MOV_ALOd, mov_OdAL, mov_eAXOv, mov_OveAX
#if BX_SUPPORT_X86_64
#define BxImmediate_Iq      0x0007 // 64 bit override
#endif
#define BxImmediate_BrOff8  0x0008 // Relative branch offset byte
#define BxImmediate_BrOff16 BxImmediate_Iw // Relative branch offset word, not encodable in 64-bit mode
#define BxImmediate_BrOff32 BxImmediate_Id // Relative branch offset dword

#define BxImmediate_Ib4     BxImmediate_Ib // Register encoded in Ib[7:4]
#define BxImmediate_Ib5     BxImmediate_Ib

// Lookup for opcode and attributes in another opcode tables
// Totally 15 opcode groups supported
#define BxGroupX            0x00f0 // bits 7..4: opcode groups definition
#define BxPrefixSSE66       0x0010 // Group encoding: 0001, SSE_PREFIX_66 only
#define BxPrefixSSEF3       0x0020 // Group encoding: 0010, SSE_PREFIX_F3 only
#define BxPrefixSSEF2       0x0030 // Group encoding: 0011, SSE_PREFIX_F2 only
#define BxPrefixSSE         0x0040 // Group encoding: 0100
#define BxPrefixSSE2        0x0050 // Group encoding: 0101, do not allow SSE_PREFIX_F2 or SSE_PREFIX_F3
#define BxPrefixSSE4        0x0060 // Group encoding: 0110
#define BxPrefixSSEF2F3     0x0070 // Group encoding: 0111, ignore SSE_PREFIX_66
#define BxNoPrefixSSE       0x0080 // Group encoding: 1000, no SSE prefix allowed
#define BxGroupN            0x0090 // Group encoding: 1001
#define BxSplitGroupN       0x00A0 // Group encoding: 1010
#define BxSplitMod11B       0x00B0 // Group encoding: 1011
#define BxSplitVexVL        0x00C0 // Group encoding: 1100

// The BxImmediate2 mask specifies kind of second immediate data
// required by instruction.
#define BxImmediate2        0x0300 // bits 8.9: any immediate
#define BxImmediate_Ib2     0x0100
#define BxImmediate_Iw2     0x0200
#define BxImmediate_Id2     0x0300

#define BxVexL0             0x0100 // bit 8 (aliased with imm2)
#define BxVexL1             0x0200 // bit 9 (aliased with imm2)
#define BxVexW0             0x0400 // bit 10
#define BxVexW1             0x0800 // bit 11

#define BxAlias             0x3000 // bits 12..13
#define BxAliasSSE          0x1000 // Encoding 01: form final opcode using SSE prefix and current opcode
#define BxAliasVexW         0x2000 // Encoding 10: form final opcode using VEX.W and current opcode
#define BxAliasVexW64       0x3000 // Encoding 11: form final opcode using VEX.W and current opcode in 64-bit mode only

#define BxGroup1          BxGroupN
#define BxGroup1A         BxGroupN
#define BxGroup2          BxGroupN
#define BxGroup3          BxGroupN
#define BxGroup4          BxGroupN
#define BxGroup5          BxGroupN
#define BxGroup6          BxGroupN
//      BxGroup7          handled separately
#define BxGroup8          BxGroupN
#define BxGroup9          BxSplitGroupN

#define BxGroup11         BxGroupN
#define BxGroup12         BxGroupN
#define BxGroup13         BxGroupN
#define BxGroup14         BxGroupN
//      BxGroup15         handled separately
#define BxGroup16         BxGroupN
#define BxGroup17         BxGroupN
//      BxGroup17         handled separately

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
} BxOpcodeInfo_t;

typedef struct BxExtOpcodeInfo_t {
  Bit16u Attr;
  Bit16u IA;
  const BxExtOpcodeInfo_t *AnotherArray;
} BxExtOpcodeInfo_t;

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

#define BX_PREPARE_EVEX_NO_BROADCAST (0x80 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX_NO_SAE       (0x40 | BX_PREPARE_EVEX)
#define BX_PREPARE_EVEX              (0x20)
#define BX_PREPARE_OPMASK            (0x10)
#define BX_PREPARE_AVX               (0x08)
#define BX_PREPARE_SSE               (0x04)
#define BX_LOCKABLE                  (0x02)
#define BX_TRACE_END                 (0x01)

struct bxIAOpcodeTable {
#ifndef BX_STANDALONE_DECODER
  BxExecutePtr_tR execute1;
  BxExecutePtr_tR execute2;
#endif
  Bit8u src[4];
  Bit8u opflags;
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
  BX_SRC_EVEX_RM = 4,       // the src is register or EVEX memory reference, register should be taken from modrm.rm
  BX_SRC_VVV = 5,           // the src should be taken from (e)vex.vvv
  BX_SRC_VIB = 6,           // the src should be taken from immediate byte
  BX_SRC_VSIB = 7,          // the src is gather/scatter vector index
  BX_SRC_IMM = 8,           // the src is immediate value
  BX_SRC_IMPLICIT = 9,      // the src is implicit register or memory reference
};

// for diassembly:
// when the source is register, indicates the register type and size
// when the source is memory reference, give hint about the memory access size
enum {
  BX_NO_REGISTER = 0,
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
  // encodings 0xE to 0xF are still free
};

// to be used together with BX_SRC_EVEX_RM
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
  // encodings 0x9 to 0xF are still free
};

// immediate forms
enum {
  BX_IMMB = 0x1,
  BX_IMMW = 0x2,
  BX_IMMD = 0x3,
  BX_IMMD_SE = 0x4,
  BX_IMMQ = 0x5,
  BX_IMMB2 = 0x6,
  BX_IMM_BrOff16 = 0x7,
  BX_IMM_BrOff32 = 0x8,
  BX_IMM_BrOff64 = 0x9,
  BX_DIRECT_PTR = 0xA,
  BX_DIRECT_MEMREF_B = 0xB,
  BX_DIRECT_MEMREF_W = 0xC,
  BX_DIRECT_MEMREF_D = 0xD,
  BX_DIRECT_MEMREF_Q = 0xE,
  // encoding 0xF still free
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
  BX_USECL = 0x9,
  BX_USEDX = 0xA,
  // encodings 0xB to 0xF are still free
};

#define BX_FORM_SRC(type, src) (((type) << 4) | (src))

#define BX_DISASM_SRC_ORIGIN(desc) (desc & 0xf)
#define BX_DISASM_SRC_TYPE(desc) (desc >> 4)

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

const Bit8u OP_CLReg  = BX_FORM_SRC(BX_USECL, BX_SRC_IMPLICIT);
const Bit8u OP_DXReg  = BX_FORM_SRC(BX_USEDX, BX_SRC_IMPLICIT);

const Bit8u OP_Ib = BX_FORM_SRC(BX_IMMB, BX_SRC_IMM);
const Bit8u OP_Iw = BX_FORM_SRC(BX_IMMW, BX_SRC_IMM);
const Bit8u OP_Id = BX_FORM_SRC(BX_IMMD, BX_SRC_IMM);
const Bit8u OP_sId = BX_FORM_SRC(BX_IMMD_SE, BX_SRC_IMM);
const Bit8u OP_Iq = BX_FORM_SRC(BX_IMMQ, BX_SRC_IMM);
const Bit8u OP_Ib2 = BX_FORM_SRC(BX_IMMB2, BX_SRC_IMM);

const Bit8u OP_Jw = BX_FORM_SRC(BX_IMM_BrOff16, BX_SRC_IMM);
const Bit8u OP_Jd = BX_FORM_SRC(BX_IMM_BrOff32, BX_SRC_IMM);
const Bit8u OP_Jq = BX_FORM_SRC(BX_IMM_BrOff64, BX_SRC_IMM);

const Bit8u OP_M  = BX_FORM_SRC(BX_NO_REGISTER, BX_SRC_RM);
const Bit8u OP_Mt = BX_FORM_SRC(BX_FPU_REG, BX_SRC_RM);
const Bit8u OP_Mdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_RM);

const Bit8u OP_Mb = OP_Eb;
const Bit8u OP_Mw = OP_Ew;
const Bit8u OP_Md = OP_Ed;
const Bit8u OP_Mq = OP_Eq;

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

const Bit8u OP_VSib = BX_FORM_SRC(BX_VMM_SCALAR, BX_SRC_VSIB);

const Bit8u OP_Hdq = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hps = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
const Bit8u OP_Hpd = BX_FORM_SRC(BX_VMM_REG, BX_SRC_VVV);
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

struct bx_modrm {
  unsigned modrm, mod, nnn, rm;
};

#include "ia_opcodes.h"

//
// Common FetchDecode Opcode Tables
//

#include "fetchdecode_x87.h"

/* ************************************************************************ */
/* Opcode Groups */

static const BxExtOpcodeInfo_t BxOpcodeGroupSSE_ERR[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 9 */
/* ******* */

static const BxExtOpcodeInfo_t BxOpcodeGroupSSE_RDPID[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RDPID_Ed },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxExtOpcodeInfo_t BxOpcodeGroupSSE_G9VMX6[3] = {
  /* 66 */ { 0, BX_IA_VMCLEAR_Mq },
  /* F3 */ { 0, BX_IA_VMXON_Mq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxExtOpcodeInfo_t BxOpcodeInfoG9w[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Ew, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Ew, BxOpcodeGroupSSE_RDPID },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { BxPrefixSSE, BX_IA_XRSTORS, BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XSAVES, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};

static const BxExtOpcodeInfo_t BxOpcodeInfoG9d[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Ed, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Ed, BxOpcodeGroupSSE_RDPID },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { BxPrefixSSE, BX_IA_XRSTORS, BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XSAVES, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};

#if BX_SUPPORT_X86_64
static const BxExtOpcodeInfo_t BxOpcodeInfo64G9q[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSEF2F3, BX_IA_RDRAND_Eq, BxOpcodeGroupSSE_ERR },
  /* 7 */ { BxPrefixSSEF2F3, BX_IA_RDSEED_Eq, BxOpcodeGroupSSE_RDPID },

  /* /m form */
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_CMPXCHG16B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { BxPrefixSSE, BX_IA_XRSTORS, BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVEC, BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XSAVES, BxOpcodeGroupSSE_ERR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};
#endif

#if BX_SUPPORT_AVX
#include "fetchdecode_avx.h"
#include "fetchdecode_xop.h"
#endif

#if BX_SUPPORT_EVEX
#include "fetchdecode_evex.h"
#endif

#endif // BX_COMMON_FETCHDECODE_TABLES_H
