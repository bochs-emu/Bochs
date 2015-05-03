/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

///////////////////////////
// prefix bytes
// opcode bytes
// modrm/sib
// address displacement
// immediate constant
///////////////////////////

#define X 0 /* undefined opcode */

static const Bit8u BxOpcodeHasModrm32[512] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f          */
  /*       -------------------------------          */
  /* 00 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,X,
  /* 10 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,
  /* 20 */ 1,1,1,1,0,0,X,0,1,1,1,1,0,0,X,0,
  /* 30 */ 1,1,1,1,0,0,X,0,1,1,1,1,0,0,X,0,
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* 60 */ 0,0,1,1,X,X,X,X,0,1,0,1,0,0,0,0,
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* 80 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* A0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* B0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* C0 */ 1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
  /* D0 */ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,
  /* E0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /* F0 */ X,0,X,X,0,0,1,1,0,0,0,0,0,0,1,1,
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
  /*       -------------------------------           */
           1,1,1,1,X,0,0,0,0,0,X,0,X,1,0,1, /* 0F 00 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 10 */
           1,1,1,1,1,X,1,X,1,1,1,1,1,1,1,1, /* 0F 20 */
           0,0,0,0,0,0,X,X,1,X,1,X,X,X,X,X, /* 0F 30 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 40 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 50 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 60 */
           1,1,1,1,1,1,1,0,1,1,X,X,1,1,1,1, /* 0F 70 */
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0F 80 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 90 */
           0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 0F A0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F B0 */
           1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* 0F C0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F D0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F E0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,X  /* 0F F0 */
  /*       -------------------------------           */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
};

#undef X

// Some info on the opcodes at {0F A6} and {0F A7}
//
// On 386 steps A0-B0:
//   {OF A6} = XBTS
//   {OF A7} = IBTS
// On 486 steps A0-B0:
//   {OF A6} = CMPXCHG 8
//   {OF A7} = CMPXCHG 16|32
//
// On 486 >= B steps, and further processors, the
// CMPXCHG instructions were moved to opcodes:
//   {OF B0} = CMPXCHG 8
//   {OF B1} = CMPXCHG 16|32

static unsigned Resolve16BaseReg[8] = {
  BX_16BIT_REG_BX,
  BX_16BIT_REG_BX,
  BX_16BIT_REG_BP,
  BX_16BIT_REG_BP,
  BX_16BIT_REG_SI,
  BX_16BIT_REG_DI,
  BX_16BIT_REG_BP,
  BX_16BIT_REG_BX
};

static unsigned Resolve16IndexReg[8] = {
  BX_16BIT_REG_SI,
  BX_16BIT_REG_DI,
  BX_16BIT_REG_SI,
  BX_16BIT_REG_DI,
  BX_NIL_REGISTER,
  BX_NIL_REGISTER,
  BX_NIL_REGISTER,
  BX_NIL_REGISTER
};

// decoding instructions; accessing seg reg's by index
static unsigned sreg_mod00_rm16[8] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS
};

static unsigned sreg_mod01or10_rm16[8] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS
};

// decoding instructions; accessing seg reg's by index
static unsigned sreg_mod0_base32[8] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS
};

static unsigned sreg_mod1or2_base32[8] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS
};

// common fetchdecode32/64 opcode tables
#include "fetchdecode.h"

// table of all Bochs opcodes
bxIAOpcodeTable BxOpcodesTable[] = {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) { b, c, { s1, s2, s3, s4 }, e },
#include "ia_opcodes.h"
};
#undef  bx_define_opcode

/* ************************** */
/* 512 entries for 16bit mode */
/* 512 entries for 32bit mode */
/* ************************** */

static const BxOpcodeInfo_t BxOpcodeInfo32[512*2] = {
  // 512 entries for 16bit mode
  /* 00 /w */ { BxLockable, BX_IA_ADD_EbGb },
  /* 01 /w */ { BxLockable, BX_IA_ADD_EwGw },
  /* 02 /w */ { 0, BX_IA_ADD_GbEb },
  /* 03 /w */ { 0, BX_IA_ADD_GwEw },
  /* 04 /w */ { BxImmediate_Ib, BX_IA_ADD_ALIb },
  /* 05 /w */ { BxImmediate_Iw, BX_IA_ADD_AXIw },
  /* 06 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 07 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 08 /w */ { BxLockable, BX_IA_OR_EbGb },
  /* 09 /w */ { BxLockable, BX_IA_OR_EwGw },
  /* 0A /w */ { 0, BX_IA_OR_GbEb },
  /* 0B /w */ { 0, BX_IA_OR_GwEw },
  /* 0C /w */ { BxImmediate_Ib, BX_IA_OR_ALIb },
  /* 0D /w */ { BxImmediate_Iw, BX_IA_OR_AXIw },
  /* 0E /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F /w */ { 0, BX_IA_ERROR }, // 2-byte escape
  /* 10 /w */ { BxLockable, BX_IA_ADC_EbGb },
  /* 11 /w */ { BxLockable, BX_IA_ADC_EwGw },
  /* 12 /w */ { 0, BX_IA_ADC_GbEb },
  /* 13 /w */ { 0, BX_IA_ADC_GwEw },
  /* 14 /w */ { BxImmediate_Ib, BX_IA_ADC_ALIb },
  /* 15 /w */ { BxImmediate_Iw, BX_IA_ADC_AXIw },
  /* 16 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 17 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 18 /w */ { BxLockable, BX_IA_SBB_EbGb },
  /* 19 /w */ { BxLockable, BX_IA_SBB_EwGw },
  /* 1A /w */ { 0, BX_IA_SBB_GbEb },
  /* 1B /w */ { 0, BX_IA_SBB_GwEw },
  /* 1C /w */ { BxImmediate_Ib, BX_IA_SBB_ALIb },
  /* 1D /w */ { BxImmediate_Iw, BX_IA_SBB_AXIw },
  /* 1E /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 1F /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 20 /w */ { BxLockable, BX_IA_AND_EbGb },
  /* 21 /w */ { BxLockable, BX_IA_AND_EwGw },
  /* 22 /w */ { 0, BX_IA_AND_GbEb },
  /* 23 /w */ { 0, BX_IA_AND_GwEw },
  /* 24 /w */ { BxImmediate_Ib, BX_IA_AND_ALIb },
  /* 25 /w */ { BxImmediate_Iw, BX_IA_AND_AXIw },
  /* 26 /w */ { 0, BX_IA_ERROR }, // ES:
  /* 27 /w */ { 0, BX_IA_DAA },
  /* 28 /w */ { BxLockable, BX_IA_SUB_EbGb },
  /* 29 /w */ { BxLockable, BX_IA_SUB_EwGw },
  /* 2A /w */ { 0, BX_IA_SUB_GbEb },
  /* 2B /w */ { 0, BX_IA_SUB_GwEw },
  /* 2C /w */ { BxImmediate_Ib, BX_IA_SUB_ALIb },
  /* 2D /w */ { BxImmediate_Iw, BX_IA_SUB_AXIw },
  /* 2E /w */ { 0, BX_IA_ERROR }, // CS:
  /* 2F /w */ { 0, BX_IA_DAS },
  /* 30 /w */ { BxLockable, BX_IA_XOR_EbGb },
  /* 31 /w */ { BxLockable, BX_IA_XOR_EwGw },
  /* 32 /w */ { 0, BX_IA_XOR_GbEb },
  /* 33 /w */ { 0, BX_IA_XOR_GwEw },
  /* 34 /w */ { BxImmediate_Ib, BX_IA_XOR_ALIb },
  /* 35 /w */ { BxImmediate_Iw, BX_IA_XOR_AXIw },
  /* 36 /w */ { 0, BX_IA_ERROR }, // SS:
  /* 37 /w */ { 0, BX_IA_AAA },
  /* 38 /w */ { 0, BX_IA_CMP_EbGb },
  /* 39 /w */ { 0, BX_IA_CMP_EwGw },
  /* 3A /w */ { 0, BX_IA_CMP_GbEb },
  /* 3B /w */ { 0, BX_IA_CMP_GwEw },
  /* 3C /w */ { BxImmediate_Ib, BX_IA_CMP_ALIb },
  /* 3D /w */ { BxImmediate_Iw, BX_IA_CMP_AXIw },
  /* 3E /w */ { 0, BX_IA_ERROR }, // DS:
  /* 3F /w */ { 0, BX_IA_AAS },
  /* 40 /w */ { 0, BX_IA_INC_Ew },
  /* 41 /w */ { 0, BX_IA_INC_Ew },
  /* 42 /w */ { 0, BX_IA_INC_Ew },
  /* 43 /w */ { 0, BX_IA_INC_Ew },
  /* 44 /w */ { 0, BX_IA_INC_Ew },
  /* 45 /w */ { 0, BX_IA_INC_Ew },
  /* 46 /w */ { 0, BX_IA_INC_Ew },
  /* 47 /w */ { 0, BX_IA_INC_Ew },
  /* 48 /w */ { 0, BX_IA_DEC_Ew },
  /* 49 /w */ { 0, BX_IA_DEC_Ew },
  /* 4A /w */ { 0, BX_IA_DEC_Ew },
  /* 4B /w */ { 0, BX_IA_DEC_Ew },
  /* 4C /w */ { 0, BX_IA_DEC_Ew },
  /* 4D /w */ { 0, BX_IA_DEC_Ew },
  /* 4E /w */ { 0, BX_IA_DEC_Ew },
  /* 4F /w */ { 0, BX_IA_DEC_Ew },
  /* 50 /w */ { 0, BX_IA_PUSH_Ew },
  /* 51 /w */ { 0, BX_IA_PUSH_Ew },
  /* 52 /w */ { 0, BX_IA_PUSH_Ew },
  /* 53 /w */ { 0, BX_IA_PUSH_Ew },
  /* 54 /w */ { 0, BX_IA_PUSH_Ew },
  /* 55 /w */ { 0, BX_IA_PUSH_Ew },
  /* 56 /w */ { 0, BX_IA_PUSH_Ew },
  /* 57 /w */ { 0, BX_IA_PUSH_Ew },
  /* 58 /w */ { 0, BX_IA_POP_Ew },
  /* 59 /w */ { 0, BX_IA_POP_Ew },
  /* 5A /w */ { 0, BX_IA_POP_Ew },
  /* 5B /w */ { 0, BX_IA_POP_Ew },
  /* 5C /w */ { 0, BX_IA_POP_Ew },
  /* 5D /w */ { 0, BX_IA_POP_Ew },
  /* 5E /w */ { 0, BX_IA_POP_Ew },
  /* 5F /w */ { 0, BX_IA_POP_Ew },
  /* 60 /w */ { 0, BX_IA_PUSHA_Op16 },
  /* 61 /w */ { 0, BX_IA_POPA_Op16 },
  /* 62 /w */ { 0, BX_IA_BOUND_GwMa },
  /* 63 /w */ { 0, BX_IA_ARPL_EwGw },
  /* 64 /w */ { 0, BX_IA_ERROR }, // FS:
  /* 65 /w */ { 0, BX_IA_ERROR }, // GS:
  /* 66 /w */ { 0, BX_IA_ERROR }, // OS:
  /* 67 /w */ { 0, BX_IA_ERROR }, // AS:
  /* 68 /w */ { BxImmediate_Iw, BX_IA_PUSH_Iw },
  /* 69 /w */ { BxImmediate_Iw, BX_IA_IMUL_GwEwIw },
  /* 6A /w */ { BxImmediate_Ib_SE, BX_IA_PUSH_Iw },
  /* 6B /w */ { BxImmediate_Ib_SE, BX_IA_IMUL_GwEwIw },
  /* 6C /w */ { 0, BX_IA_REP_INSB_YbDX },
  /* 6D /w */ { 0, BX_IA_REP_INSW_YwDX },
  /* 6E /w */ { 0, BX_IA_REP_OUTSB_DXXb },
  /* 6F /w */ { 0, BX_IA_REP_OUTSW_DXXw },
  /* 70 /w */ { BxImmediate_BrOff8, BX_IA_JO_Jw },
  /* 71 /w */ { BxImmediate_BrOff8, BX_IA_JNO_Jw },
  /* 72 /w */ { BxImmediate_BrOff8, BX_IA_JB_Jw },
  /* 73 /w */ { BxImmediate_BrOff8, BX_IA_JNB_Jw },
  /* 74 /w */ { BxImmediate_BrOff8, BX_IA_JZ_Jw },
  /* 75 /w */ { BxImmediate_BrOff8, BX_IA_JNZ_Jw },
  /* 76 /w */ { BxImmediate_BrOff8, BX_IA_JBE_Jw },
  /* 77 /w */ { BxImmediate_BrOff8, BX_IA_JNBE_Jw },
  /* 78 /w */ { BxImmediate_BrOff8, BX_IA_JS_Jw },
  /* 79 /w */ { BxImmediate_BrOff8, BX_IA_JNS_Jw },
  /* 7A /w */ { BxImmediate_BrOff8, BX_IA_JP_Jw },
  /* 7B /w */ { BxImmediate_BrOff8, BX_IA_JNP_Jw },
  /* 7C /w */ { BxImmediate_BrOff8, BX_IA_JL_Jw },
  /* 7D /w */ { BxImmediate_BrOff8, BX_IA_JNL_Jw },
  /* 7E /w */ { BxImmediate_BrOff8, BX_IA_JLE_Jw },
  /* 7F /w */ { BxImmediate_BrOff8, BX_IA_JNLE_Jw },
  /* 80 /w */ { BxGroup1 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG1EbIb },
  /* 81 /w */ { BxGroup1 | BxImmediate_Iw, BX_IA_ERROR, BxOpcodeInfoG1Ew },
  /* 82 /w */ { BxGroup1 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG1EbIb },
  /* 83 /w */ { BxGroup1 | BxImmediate_Ib_SE, BX_IA_ERROR, BxOpcodeInfoG1Ew },
  /* 84 /w */ { 0, BX_IA_TEST_EbGb },
  /* 85 /w */ { 0, BX_IA_TEST_EwGw },
  /* 86 /w */ { BxLockable, BX_IA_XCHG_EbGb },
  /* 87 /w */ { BxLockable, BX_IA_XCHG_EwGw },
  /* 88 /w */ { 0, BX_IA_MOV_EbGb },
  /* 89 /w */ { 0, BX_IA_MOV_EwGw },
  /* 8A /w */ { 0, BX_IA_MOV_GbEb },
  /* 8B /w */ { 0, BX_IA_MOV_GwEw },
  /* 8C /w */ { 0, BX_IA_MOV_EwSw },
  /* 8D /w */ { 0, BX_IA_LEA_GwM },
  /* 8E /w */ { 0, BX_IA_MOV_SwEw },
  /* 8F /w */ { BxGroup1A, BX_IA_ERROR, BxOpcodeInfoG1AEw },
  /* 90 /w */ { BxPrefixSSE, BX_IA_NOP, BxOpcodeGroupSSE_PAUSE },
  /* 91 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 92 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 93 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 94 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 95 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 96 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 97 /w */ { 0, BX_IA_XCHG_RXAX },
  /* 98 /w */ { 0, BX_IA_CBW },
  /* 99 /w */ { 0, BX_IA_CWD },
  /* 9A /w */ { BxImmediate_Iw | BxImmediate_Iw2, BX_IA_CALLF_Op16_Ap },
  /* 9B /w */ { 0, BX_IA_FWAIT },
  /* 9C /w */ { 0, BX_IA_PUSHF_Fw },
  /* 9D /w */ { 0, BX_IA_POPF_Fw },
  /* 9E /w */ { 0, BX_IA_SAHF },
  /* 9F /w */ { 0, BX_IA_LAHF },
  /* A0 /w */ { BxImmediate_O, BX_IA_MOV_ALOd },
  /* A1 /w */ { BxImmediate_O, BX_IA_MOV_AXOd },
  /* A2 /w */ { BxImmediate_O, BX_IA_MOV_OdAL },
  /* A3 /w */ { BxImmediate_O, BX_IA_MOV_OdAX },
  /* A4 /w */ { 0, BX_IA_REP_MOVSB_YbXb },
  /* A5 /w */ { 0, BX_IA_REP_MOVSW_YwXw },
  /* A6 /w */ { 0, BX_IA_REP_CMPSB_XbYb },
  /* A7 /w */ { 0, BX_IA_REP_CMPSW_XwYw },
  /* A8 /w */ { BxImmediate_Ib, BX_IA_TEST_ALIb },
  /* A9 /w */ { BxImmediate_Iw, BX_IA_TEST_AXIw },
  /* AA /w */ { 0, BX_IA_REP_STOSB_YbAL },
  /* AB /w */ { 0, BX_IA_REP_STOSW_YwAX },
  /* AC /w */ { 0, BX_IA_REP_LODSB_ALXb },
  /* AD /w */ { 0, BX_IA_REP_LODSW_AXXw },
  /* AE /w */ { 0, BX_IA_REP_SCASB_ALYb },
  /* AF /w */ { 0, BX_IA_REP_SCASW_AXYw },
  /* B0 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B1 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B2 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B3 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B4 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B5 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B6 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B7 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B8 /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* B9 /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BA /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BB /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BC /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BD /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BE /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* BF /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* C0 /w */ { BxGroup2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG2EbIb },
  /* C1 /w */ { BxGroup2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG2EwIb },
  /* C2 /w */ { BxImmediate_Iw, BX_IA_RET_Op16_Iw },
  /* C3 /w */ { 0, BX_IA_RET_Op16 },
  /* C4 /w */ { 0, BX_IA_LES_GwMp },
  /* C5 /w */ { 0, BX_IA_LDS_GwMp },
  /* C6 /w */ { BxGroup11, BX_IA_ERROR, BxOpcodeInfoG11Eb },
  /* C7 /w */ { BxGroup11, BX_IA_ERROR, BxOpcodeInfoG11Ew },
  /* C8 /w */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op16_IwIb },
  /* C9 /w */ { 0, BX_IA_LEAVE_Op16 },
  /* CA /w */ { BxImmediate_Iw, BX_IA_RETF_Op16_Iw },
  /* CB /w */ { 0, BX_IA_RETF_Op16 },
  /* CC /w */ { 0, BX_IA_INT3 },
  /* CD /w */ { BxImmediate_Ib, BX_IA_INT_Ib },
  /* CE /w */ { 0, BX_IA_INTO },
  /* CF /w */ { 0, BX_IA_IRET_Op16 },
  /* D0 /w */ { BxGroup2 | BxImmediate_I1, BX_IA_ERROR, BxOpcodeInfoG2EbIb },
  /* D1 /w */ { BxGroup2 | BxImmediate_I1, BX_IA_ERROR, BxOpcodeInfoG2EwIb },
  /* D2 /w */ { BxGroup2, BX_IA_ERROR, BxOpcodeInfoG2Eb },
  /* D3 /w */ { BxGroup2, BX_IA_ERROR, BxOpcodeInfoG2Ew },
  /* D4 /w */ { BxImmediate_Ib, BX_IA_AAM },
  /* D5 /w */ { BxImmediate_Ib, BX_IA_AAD },
  /* D6 /w */ { 0, BX_IA_SALC },
  /* D7 /w */ { 0, BX_IA_XLAT },
#if BX_SUPPORT_FPU
  /* D8 /w */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupD8 },
  /* D9 /w */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointD9 },
  /* DA /w */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDA },
  /* DB /w */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDB },
  /* DC /w */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupDC },
  /* DD /w */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupDD },
  /* DE /w */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDE },
  /* DF /w */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDF },
#else
  /* D8 /w */ { 0, BX_IA_FPUESC },
  /* D9 /w */ { 0, BX_IA_FPUESC },
  /* DA /w */ { 0, BX_IA_FPUESC },
  /* DB /w */ { 0, BX_IA_FPUESC },
  /* DC /w */ { 0, BX_IA_FPUESC },
  /* DD /w */ { 0, BX_IA_FPUESC },
  /* DE /w */ { 0, BX_IA_FPUESC },
  /* DF /w */ { 0, BX_IA_FPUESC },
#endif
  /* E0 /w */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op16_Jb },
  /* E1 /w */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op16_Jb },
  /* E2 /w */ { BxImmediate_BrOff8, BX_IA_LOOP_Op16_Jb },
  /* E3 /w */ { BxImmediate_BrOff8, BX_IA_JCXZ_Jb },
  /* E4 /w */ { BxImmediate_Ib, BX_IA_IN_ALIb },
  /* E5 /w */ { BxImmediate_Ib, BX_IA_IN_AXIb },
  /* E6 /w */ { BxImmediate_Ib, BX_IA_OUT_IbAL },
  /* E7 /w */ { BxImmediate_Ib, BX_IA_OUT_IbAX },
  /* E8 /w */ { BxImmediate_BrOff16, BX_IA_CALL_Jw },
  /* E9 /w */ { BxImmediate_BrOff16, BX_IA_JMP_Jw },
  /* EA /w */ { BxImmediate_Iw | BxImmediate_Iw2, BX_IA_JMPF_Ap },
  /* EB /w */ { BxImmediate_BrOff8, BX_IA_JMP_Jw },
  /* EC /w */ { 0, BX_IA_IN_ALDX },
  /* ED /w */ { 0, BX_IA_IN_AXDX },
  /* EE /w */ { 0, BX_IA_OUT_DXAL },
  /* EF /w */ { 0, BX_IA_OUT_DXAX },
  /* F0 /w */ { 0, BX_IA_ERROR }, // LOCK
  /* F1 /w */ { 0, BX_IA_INT1 },
  /* F2 /w */ { 0, BX_IA_ERROR }, // REPNE/REPNZ
  /* F3 /w */ { 0, BX_IA_ERROR }, // REP, REPE/REPZ
  /* F4 /w */ { 0, BX_IA_HLT },
  /* F5 /w */ { 0, BX_IA_CMC },
  /* F6 /w */ { BxGroup3, BX_IA_ERROR, BxOpcodeInfoG3Eb },
  /* F7 /w */ { BxGroup3, BX_IA_ERROR, BxOpcodeInfoG3Ew },
  /* F8 /w */ { 0, BX_IA_CLC },
  /* F9 /w */ { 0, BX_IA_STC },
  /* FA /w */ { 0, BX_IA_CLI },
  /* FB /w */ { 0, BX_IA_STI },
  /* FC /w */ { 0, BX_IA_CLD },
  /* FD /w */ { 0, BX_IA_STD },
  /* FE /w */ { BxGroup4, BX_IA_ERROR, BxOpcodeInfoG4 },
  /* FF /w */ { BxGroup5, BX_IA_ERROR, BxOpcodeInfoG5w },

  /* 0F 00 /w */ { BxGroup6, BX_IA_ERROR, BxOpcodeInfoG6 },
  /* 0F 01 /w */ { BxGroup7, BX_IA_ERROR, BxOpcodeInfoG7 },
  /* 0F 02 /w */ { 0, BX_IA_LAR_GwEw },
  /* 0F 03 /w */ { 0, BX_IA_LSL_GwEw },
  /* 0F 04 /w */ { 0, BX_IA_ERROR },
  /* 0F 05 /w */ { 0, BX_IA_SYSCALL_LEGACY },
  /* 0F 06 /w */ { 0, BX_IA_CLTS },
  /* 0F 07 /w */ { 0, BX_IA_SYSRET_LEGACY },
  /* 0F 08 /w */ { 0, BX_IA_INVD },
  /* 0F 09 /w */ { 0, BX_IA_WBINVD },
  /* 0F 0A /w */ { 0, BX_IA_ERROR },
  /* 0F 0B /w */ { 0, BX_IA_UD2A },
  /* 0F 0C /w */ { 0, BX_IA_ERROR },
  /* 0F 0D /w */ { 0, BX_IA_PREFETCHW },          // 3DNow! PREFETCHW on AMD, NOP on Intel
  /* 0F 0E /w */ { 0, BX_IA_FEMMS },              // 3DNow! FEMMS
  /* 0F 0F /w */ { BxImmediate_Ib, BX_IA_ERROR }, // 3DNow! Opcode Table
  /* 0F 10 /w */ { BxPrefixSSE, BX_IA_MOVUPS_VpsWps, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /w */ { BxPrefixSSE, BX_IA_MOVUPS_WpsVps, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /w */ { BxPrefixSSE, BX_IA_MOVLPS_VpsMq, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f13M },
  /* 0F 14 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /w */ { BxPrefixSSE, BX_IA_MOVHPS_VpsMq, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f17M },
#if BX_CPU_LEVEL >= 6
  /* 0F 18 /w */ { BxGroup16, BX_IA_ERROR, BxOpcodeInfoG16 }, // opcode group G16, PREFETCH hints
  /* 0F 19 /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1A /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1B /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1C /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1D /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1E /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1F /w */ { 0, BX_IA_NOP },      // multi-byte NOP
#else
  /* 0F 18 /w */ { 0, BX_IA_ERROR },
  /* 0F 19 /w */ { 0, BX_IA_ERROR },
  /* 0F 1A /w */ { 0, BX_IA_ERROR },
  /* 0F 1B /w */ { 0, BX_IA_ERROR },
  /* 0F 1C /w */ { 0, BX_IA_ERROR },
  /* 0F 1D /w */ { 0, BX_IA_ERROR },
  /* 0F 1E /w */ { 0, BX_IA_ERROR },
  /* 0F 1F /w */ { 0, BX_IA_ERROR },
#endif
  /* 0F 20 /w */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoMOV_RdCd },
  /* 0F 21 /w */ { 0, BX_IA_MOV_RdDd },
  /* 0F 22 /w */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoMOV_CdRd },
  /* 0F 23 /w */ { 0, BX_IA_MOV_DdRd },
  /* 0F 24 /w */ { 0, BX_IA_ERROR }, // BX_IA_MOV_RdTd not implemented
  /* 0F 25 /w */ { 0, BX_IA_ERROR },
  /* 0F 26 /w */ { 0, BX_IA_ERROR }, // BX_IA_MOV_TdRd not implemented
  /* 0F 27 /w */ { 0, BX_IA_ERROR },
  /* 0F 28 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /w */ { BxPrefixSSE, BX_IA_CVTPI2PS_VpsQq, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /w */ { BxPrefixSSE, BX_IA_MOVNTPS_MpsVps, BxOpcodeGroupSSE_0f2bM },
  /* 0F 2C /w */ { BxPrefixSSE, BX_IA_CVTTPS2PI_PqWps, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /w */ { BxPrefixSSE, BX_IA_CVTPS2PI_PqWps, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /w */ { 0, BX_IA_WRMSR },
  /* 0F 31 /w */ { 0, BX_IA_RDTSC }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 32 /w */ { 0, BX_IA_RDMSR }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 33 /w */ { 0, BX_IA_RDPMC },
  /* 0F 34 /w */ { 0, BX_IA_SYSENTER },
  /* 0F 35 /w */ { 0, BX_IA_SYSEXIT },
  /* 0F 36 /w */ { 0, BX_IA_ERROR },
  /* 0F 37 /w */ { 0, BX_IA_GETSEC },
#if BX_CPU_LEVEL >= 6
  /* 0F 38 /w */ { 0, BX_IA_ERROR, BxOpcode3ByteTable0f38 }, // 3-byte escape
#else
  /* 0F 38 /w */ { 0, BX_IA_ERROR },
#endif
  /* 0F 39 /w */ { 0, BX_IA_ERROR },
#if BX_CPU_LEVEL >= 6
  /* 0F 3A /w */ { BxImmediate_Ib, BX_IA_ERROR, BxOpcode3ByteTable0f3a }, // 3-byte escape
#else
  /* 0F 3A /w */ { 0, BX_IA_ERROR },
#endif
  /* 0F 3B /w */ { 0, BX_IA_ERROR },
  /* 0F 3C /w */ { 0, BX_IA_ERROR },
  /* 0F 3D /w */ { 0, BX_IA_ERROR },
  /* 0F 3E /w */ { 0, BX_IA_ERROR },
  /* 0F 3F /w */ { 0, BX_IA_ERROR },
  /* 0F 40 /w */ { 0, BX_IA_CMOVO_GwEw },
  /* 0F 41 /w */ { 0, BX_IA_CMOVNO_GwEw },
  /* 0F 42 /w */ { 0, BX_IA_CMOVB_GwEw },
  /* 0F 43 /w */ { 0, BX_IA_CMOVNB_GwEw },
  /* 0F 44 /w */ { 0, BX_IA_CMOVZ_GwEw },
  /* 0F 45 /w */ { 0, BX_IA_CMOVNZ_GwEw },
  /* 0F 46 /w */ { 0, BX_IA_CMOVBE_GwEw },
  /* 0F 47 /w */ { 0, BX_IA_CMOVNBE_GwEw },
  /* 0F 48 /w */ { 0, BX_IA_CMOVS_GwEw },
  /* 0F 49 /w */ { 0, BX_IA_CMOVNS_GwEw },
  /* 0F 4A /w */ { 0, BX_IA_CMOVP_GwEw },
  /* 0F 4B /w */ { 0, BX_IA_CMOVNP_GwEw },
  /* 0F 4C /w */ { 0, BX_IA_CMOVL_GwEw },
  /* 0F 4D /w */ { 0, BX_IA_CMOVNL_GwEw },
  /* 0F 4E /w */ { 0, BX_IA_CMOVLE_GwEw },
  /* 0F 4F /w */ { 0, BX_IA_CMOVNLE_GwEw },
  /* 0F 50 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f50R },
  /* 0F 51 /w */ { BxAliasSSE, BX_IA_SQRTPS_VpsWps },
  /* 0F 52 /w */ { BxPrefixSSE, BX_IA_RSQRTPS_VpsWps, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /w */ { BxPrefixSSE, BX_IA_RCPPS_VpsWps, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /w */ { BxAliasSSE, BX_IA_ADDPS_VpsWps },
  /* 0F 59 /w */ { BxAliasSSE, BX_IA_MULPS_VpsWps },
  /* 0F 5A /w */ { BxAliasSSE, BX_IA_CVTPS2PD_VpdWps },
  /* 0F 5B /w */ { BxPrefixSSE, BX_IA_CVTDQ2PS_VpsWdq, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /w */ { BxAliasSSE, BX_IA_SUBPS_VpsWps },
  /* 0F 5D /w */ { BxAliasSSE, BX_IA_MINPS_VpsWps },
  /* 0F 5E /w */ { BxAliasSSE, BX_IA_DIVPS_VpsWps },
  /* 0F 5F /w */ { BxAliasSSE, BX_IA_MAXPS_VpsWps },
  /* 0F 60 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f61 },
  /* 0F 62 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f62 },
  /* 0F 63 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f63 },
  /* 0F 64 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f64 },
  /* 0F 65 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f65 },
  /* 0F 66 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f66 },
  /* 0F 67 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f67 },
  /* 0F 68 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f68 },
  /* 0F 69 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f69 },
  /* 0F 6A /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6a },
  /* 0F 6B /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6b },
  /* 0F 6C /w */ { BxPrefixSSE66, BX_IA_PUNPCKLQDQ_VdqWdq },
  /* 0F 6D /w */ { BxPrefixSSE66, BX_IA_PUNPCKHQDQ_VdqWdq },
  /* 0F 6E /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6e },
  /* 0F 6F /w */ { BxPrefixSSE, BX_IA_MOVQ_PqQq, BxOpcodeGroupSSE_0f6f },
  /* 0F 70 /w */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_PSHUFW_PqQqIb, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /w */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoG12R },
  /* 0F 72 /w */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoG13R },
  /* 0F 73 /w */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoG14R },
  /* 0F 74 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f74 },
  /* 0F 75 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f75 },
  /* 0F 76 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f76 },
  /* 0F 77 /w */ { BxPrefixSSE, BX_IA_EMMS, BxOpcodeGroupSSE_ERR },
  /* 0F 78 /w */ { BxPrefixSSE, BX_IA_VMREAD_EdGd, BxOpcodeGroupSSE4A_0f78 },
  /* 0F 79 /w */ { BxPrefixSSE, BX_IA_VMWRITE_GdEd, BxOpcodeGroupSSE4A_0f79 },
  /* 0F 7A /w */ { 0, BX_IA_ERROR },
  /* 0F 7B /w */ { 0, BX_IA_ERROR },
  /* 0F 7C /w */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f7c },
  /* 0F 7D /w */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f7d },
  /* 0F 7E /w */ { BxPrefixSSE, BX_IA_MOVD_EdPq, BxOpcodeGroupSSE_0f7e },
  /* 0F 7F /w */ { BxPrefixSSE, BX_IA_MOVQ_QqPq, BxOpcodeGroupSSE_0f7f },
  /* 0F 80 /w */ { BxImmediate_BrOff16, BX_IA_JO_Jw },
  /* 0F 81 /w */ { BxImmediate_BrOff16, BX_IA_JNO_Jw },
  /* 0F 82 /w */ { BxImmediate_BrOff16, BX_IA_JB_Jw },
  /* 0F 83 /w */ { BxImmediate_BrOff16, BX_IA_JNB_Jw },
  /* 0F 84 /w */ { BxImmediate_BrOff16, BX_IA_JZ_Jw },
  /* 0F 85 /w */ { BxImmediate_BrOff16, BX_IA_JNZ_Jw },
  /* 0F 86 /w */ { BxImmediate_BrOff16, BX_IA_JBE_Jw },
  /* 0F 87 /w */ { BxImmediate_BrOff16, BX_IA_JNBE_Jw },
  /* 0F 88 /w */ { BxImmediate_BrOff16, BX_IA_JS_Jw },
  /* 0F 89 /w */ { BxImmediate_BrOff16, BX_IA_JNS_Jw },
  /* 0F 8A /w */ { BxImmediate_BrOff16, BX_IA_JP_Jw },
  /* 0F 8B /w */ { BxImmediate_BrOff16, BX_IA_JNP_Jw },
  /* 0F 8C /w */ { BxImmediate_BrOff16, BX_IA_JL_Jw },
  /* 0F 8D /w */ { BxImmediate_BrOff16, BX_IA_JNL_Jw },
  /* 0F 8E /w */ { BxImmediate_BrOff16, BX_IA_JLE_Jw },
  /* 0F 8F /w */ { BxImmediate_BrOff16, BX_IA_JNLE_Jw },
  /* 0F 90 /w */ { 0, BX_IA_SETO_Eb },
  /* 0F 91 /w */ { 0, BX_IA_SETNO_Eb },
  /* 0F 92 /w */ { 0, BX_IA_SETB_Eb },
  /* 0F 93 /w */ { 0, BX_IA_SETNB_Eb },
  /* 0F 94 /w */ { 0, BX_IA_SETZ_Eb },
  /* 0F 95 /w */ { 0, BX_IA_SETNZ_Eb },
  /* 0F 96 /w */ { 0, BX_IA_SETBE_Eb },
  /* 0F 97 /w */ { 0, BX_IA_SETNBE_Eb },
  /* 0F 98 /w */ { 0, BX_IA_SETS_Eb },
  /* 0F 99 /w */ { 0, BX_IA_SETNS_Eb },
  /* 0F 9A /w */ { 0, BX_IA_SETP_Eb },
  /* 0F 9B /w */ { 0, BX_IA_SETNP_Eb },
  /* 0F 9C /w */ { 0, BX_IA_SETL_Eb },
  /* 0F 9D /w */ { 0, BX_IA_SETNL_Eb },
  /* 0F 9E /w */ { 0, BX_IA_SETLE_Eb },
  /* 0F 9F /w */ { 0, BX_IA_SETNLE_Eb },
  /* 0F A0 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A1 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F A2 /w */ { 0, BX_IA_CPUID },
  /* 0F A3 /w */ { 0, BX_IA_BT_EwGw },
  /* 0F A4 /w */ { BxImmediate_Ib, BX_IA_SHLD_EwGwIb },
  /* 0F A5 /w */ { 0,              BX_IA_SHLD_EwGw   },
  /* 0F A6 /w */ { 0, BX_IA_ERROR }, // CMPXCHG_XBTS not implemented
  /* 0F A7 /w */ { 0, BX_IA_ERROR }, // CMPXCHG_IBTS not implemented
  /* 0F A8 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A9 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F AA /w */ { 0, BX_IA_RSM },
  /* 0F AB /w */ { BxLockable, BX_IA_BTS_EwGw },
  /* 0F AC /w */ { BxImmediate_Ib, BX_IA_SHRD_EwGwIb },
  /* 0F AD /w */ { 0,              BX_IA_SHRD_EwGw   },
  /* 0F AE /w */ { BxGroup15, BX_IA_ERROR, BxOpcodeInfoG15 },
  /* 0F AF /w */ { 0, BX_IA_IMUL_GwEw },
  /* 0F B0 /w */ { BxLockable, BX_IA_CMPXCHG_EbGb },
  /* 0F B1 /w */ { BxLockable, BX_IA_CMPXCHG_EwGw },
  /* 0F B2 /w */ { 0, BX_IA_LSS_GwMp },
  /* 0F B3 /w */ { BxLockable, BX_IA_BTR_EwGw },
  /* 0F B4 /w */ { 0, BX_IA_LFS_GwMp },
  /* 0F B5 /w */ { 0, BX_IA_LGS_GwMp },
  /* 0F B6 /w */ { 0, BX_IA_MOVZX_GwEb },
  /* 0F B7 /w */ { 0, BX_IA_MOV_GwEw }, // MOVZX_GwEw
  /* 0F B8 /w */ { BxPrefixSSEF3, BX_IA_POPCNT_GwEw },
  /* 0F B9 /w */ { 0, BX_IA_UD2B },
  /* 0F BA /w */ { BxGroup8, BX_IA_ERROR, BxOpcodeInfoG8EwIb },
  /* 0F BB /w */ { BxLockable, BX_IA_BTC_EwGw },
  /* 0F BC /w */ { BxPrefixSSE, BX_IA_BSF_GwEw, BxOpcodeGroupSSE_TZCNT16 },
  /* 0F BD /w */ { BxPrefixSSE, BX_IA_BSR_GwEw, BxOpcodeGroupSSE_LZCNT16 },
  /* 0F BE /w */ { 0, BX_IA_MOVSX_GwEb },
  /* 0F BF /w */ { 0, BX_IA_MOV_GwEw }, // MOVSX_GwEw
  /* 0F C0 /w */ { BxLockable, BX_IA_XADD_EbGb },
  /* 0F C1 /w */ { BxLockable, BX_IA_XADD_EwGw },
  /* 0F C2 /w */ { BxAliasSSE | BxImmediate_Ib, BX_IA_CMPPS_VpsWpsIb },
  /* 0F C3 /w */ { BxPrefixSSE, BX_IA_MOVNTI_Op32_MdGd, BxOpcodeGroupSSE_ERR },
  /* 0F C4 /w */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /w */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc5R },
  /* 0F C6 /w */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /w */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9w },
  /* 0F C8 /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F C9 /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CA /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CB /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CC /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CD /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CE /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F CF /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F D0 /w */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd5 },
  /* 0F D6 /w */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd7R },
  /* 0F D8 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fda },
  /* 0F DB /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fde },
  /* 0F DF /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe1 },
  /* 0F E2 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe2 },
  /* 0F E3 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /w */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe7M },
  /* 0F E8 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fea },
  /* 0F EB /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0feb },
  /* 0F EC /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fec },
  /* 0F ED /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fed },
  /* 0F EE /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fee },
  /* 0F EF /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /w */ { BxPrefixSSEF2, BX_IA_LDDQU_VdqMdq },
  /* 0F F1 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff1 },
  /* 0F F2 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff2 },
  /* 0F F3 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff3 },
  /* 0F F4 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff4 },
  /* 0F F5 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff5 },
  /* 0F F6 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff6 },
  /* 0F F7 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff7R },
  /* 0F F8 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff8 },
  /* 0F F9 /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff9 },
  /* 0F FA /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffa },
  /* 0F FB /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffb },
  /* 0F FC /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffc },
  /* 0F FD /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffd },
  /* 0F FE /w */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffe },
  /* 0F FF /w */ { 0, BX_IA_ERROR },

  // 512 entries for 32bit mode
  /* 00 /d */ { BxLockable, BX_IA_ADD_EbGb },
  /* 01 /d */ { BxLockable, BX_IA_ADD_EdGd },
  /* 02 /d */ { 0, BX_IA_ADD_GbEb },
  /* 03 /d */ { 0, BX_IA_ADD_GdEd },
  /* 04 /d */ { BxImmediate_Ib, BX_IA_ADD_ALIb },
  /* 05 /d */ { BxImmediate_Id, BX_IA_ADD_EAXId },
  /* 06 /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 07 /d */ { 0, BX_IA_POP_Op32_Sw },
  /* 08 /d */ { BxLockable, BX_IA_OR_EbGb },
  /* 09 /d */ { BxLockable, BX_IA_OR_EdGd },
  /* 0A /d */ { 0, BX_IA_OR_GbEb },
  /* 0B /d */ { 0, BX_IA_OR_GdEd },
  /* 0C /d */ { BxImmediate_Ib, BX_IA_OR_ALIb },
  /* 0D /d */ { BxImmediate_Id, BX_IA_OR_EAXId },
  /* 0E /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 0F /d */ { 0, BX_IA_ERROR }, // 2-byte escape
  /* 10 /d */ { BxLockable, BX_IA_ADC_EbGb },
  /* 11 /d */ { BxLockable, BX_IA_ADC_EdGd },
  /* 12 /d */ { 0, BX_IA_ADC_GbEb },
  /* 13 /d */ { 0, BX_IA_ADC_GdEd },
  /* 14 /d */ { BxImmediate_Ib, BX_IA_ADC_ALIb },
  /* 15 /d */ { BxImmediate_Id, BX_IA_ADC_EAXId },
  /* 16 /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 17 /d */ { 0, BX_IA_POP_Op32_Sw },
  /* 18 /d */ { BxLockable, BX_IA_SBB_EbGb },
  /* 19 /d */ { BxLockable, BX_IA_SBB_EdGd },
  /* 1A /d */ { 0, BX_IA_SBB_GbEb },
  /* 1B /d */ { 0, BX_IA_SBB_GdEd },
  /* 1C /d */ { BxImmediate_Ib, BX_IA_SBB_ALIb },
  /* 1D /d */ { BxImmediate_Id, BX_IA_SBB_EAXId },
  /* 1E /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 1F /d */ { 0, BX_IA_POP_Op32_Sw },
  /* 20 /d */ { BxLockable, BX_IA_AND_EbGb },
  /* 21 /d */ { BxLockable, BX_IA_AND_EdGd },
  /* 22 /d */ { 0, BX_IA_AND_GbEb },
  /* 23 /d */ { 0, BX_IA_AND_GdEd },
  /* 24 /d */ { BxImmediate_Ib, BX_IA_AND_ALIb },
  /* 25 /d */ { BxImmediate_Id, BX_IA_AND_EAXId },
  /* 26 /d */ { 0, BX_IA_ERROR }, // ES:
  /* 27 /d */ { 0, BX_IA_DAA },
  /* 28 /d */ { BxLockable, BX_IA_SUB_EbGb },
  /* 29 /d */ { BxLockable, BX_IA_SUB_EdGd },
  /* 2A /d */ { 0, BX_IA_SUB_GbEb },
  /* 2B /d */ { 0, BX_IA_SUB_GdEd },
  /* 2C /d */ { BxImmediate_Ib, BX_IA_SUB_ALIb },
  /* 2D /d */ { BxImmediate_Id, BX_IA_SUB_EAXId },
  /* 2E /d */ { 0, BX_IA_ERROR }, // CS:
  /* 2F /d */ { 0, BX_IA_DAS },
  /* 30 /d */ { BxLockable, BX_IA_XOR_EbGb },
  /* 31 /d */ { BxLockable, BX_IA_XOR_EdGd },
  /* 32 /d */ { 0, BX_IA_XOR_GbEb },
  /* 33 /d */ { 0, BX_IA_XOR_GdEd },
  /* 34 /d */ { BxImmediate_Ib, BX_IA_XOR_ALIb },
  /* 35 /d */ { BxImmediate_Id, BX_IA_XOR_EAXId },
  /* 36 /d */ { 0, BX_IA_ERROR }, // SS:
  /* 37 /d */ { 0, BX_IA_AAA },
  /* 38 /d */ { 0, BX_IA_CMP_EbGb },
  /* 39 /d */ { 0, BX_IA_CMP_EdGd },
  /* 3A /d */ { 0, BX_IA_CMP_GbEb },
  /* 3B /d */ { 0, BX_IA_CMP_GdEd },
  /* 3C /d */ { BxImmediate_Ib, BX_IA_CMP_ALIb },
  /* 3D /d */ { BxImmediate_Id, BX_IA_CMP_EAXId },
  /* 3E /d */ { 0, BX_IA_ERROR }, // DS:
  /* 3F /d */ { 0, BX_IA_AAS },
  /* 40 /d */ { 0, BX_IA_INC_Ed },
  /* 41 /d */ { 0, BX_IA_INC_Ed },
  /* 42 /d */ { 0, BX_IA_INC_Ed },
  /* 43 /d */ { 0, BX_IA_INC_Ed },
  /* 44 /d */ { 0, BX_IA_INC_Ed },
  /* 45 /d */ { 0, BX_IA_INC_Ed },
  /* 46 /d */ { 0, BX_IA_INC_Ed },
  /* 47 /d */ { 0, BX_IA_INC_Ed },
  /* 48 /d */ { 0, BX_IA_DEC_Ed },
  /* 49 /d */ { 0, BX_IA_DEC_Ed },
  /* 4A /d */ { 0, BX_IA_DEC_Ed },
  /* 4B /d */ { 0, BX_IA_DEC_Ed },
  /* 4C /d */ { 0, BX_IA_DEC_Ed },
  /* 4D /d */ { 0, BX_IA_DEC_Ed },
  /* 4E /d */ { 0, BX_IA_DEC_Ed },
  /* 4F /d */ { 0, BX_IA_DEC_Ed },
  /* 50 /d */ { 0, BX_IA_PUSH_Ed },
  /* 51 /d */ { 0, BX_IA_PUSH_Ed },
  /* 52 /d */ { 0, BX_IA_PUSH_Ed },
  /* 53 /d */ { 0, BX_IA_PUSH_Ed },
  /* 54 /d */ { 0, BX_IA_PUSH_Ed },
  /* 55 /d */ { 0, BX_IA_PUSH_Ed },
  /* 56 /d */ { 0, BX_IA_PUSH_Ed },
  /* 57 /d */ { 0, BX_IA_PUSH_Ed },
  /* 58 /d */ { 0, BX_IA_POP_Ed },
  /* 59 /d */ { 0, BX_IA_POP_Ed },
  /* 5A /d */ { 0, BX_IA_POP_Ed },
  /* 5B /d */ { 0, BX_IA_POP_Ed },
  /* 5C /d */ { 0, BX_IA_POP_Ed },
  /* 5D /d */ { 0, BX_IA_POP_Ed },
  /* 5E /d */ { 0, BX_IA_POP_Ed },
  /* 5F /d */ { 0, BX_IA_POP_Ed },
  /* 60 /d */ { 0, BX_IA_PUSHA_Op32 },
  /* 61 /d */ { 0, BX_IA_POPA_Op32 },
  /* 62 /d */ { 0, BX_IA_BOUND_GdMa },
  /* 63 /d */ { 0, BX_IA_ARPL_EwGw },
  /* 64 /d */ { 0, BX_IA_ERROR }, // FS:
  /* 65 /d */ { 0, BX_IA_ERROR }, // GS:
  /* 66 /d */ { 0, BX_IA_ERROR }, // OS:
  /* 67 /d */ { 0, BX_IA_ERROR }, // AS:
  /* 68 /d */ { BxImmediate_Id, BX_IA_PUSH_Id },
  /* 69 /d */ { BxImmediate_Id, BX_IA_IMUL_GdEdId },
  /* 6A /d */ { BxImmediate_Ib_SE, BX_IA_PUSH_Id },
  /* 6B /d */ { BxImmediate_Ib_SE, BX_IA_IMUL_GdEdId },
  /* 6C /d */ { 0, BX_IA_REP_INSB_YbDX },
  /* 6D /d */ { 0, BX_IA_REP_INSD_YdDX },
  /* 6E /d */ { 0, BX_IA_REP_OUTSB_DXXb },
  /* 6F /d */ { 0, BX_IA_REP_OUTSD_DXXd },
  /* 70 /d */ { BxImmediate_BrOff8, BX_IA_JO_Jd },
  /* 71 /d */ { BxImmediate_BrOff8, BX_IA_JNO_Jd },
  /* 72 /d */ { BxImmediate_BrOff8, BX_IA_JB_Jd },
  /* 73 /d */ { BxImmediate_BrOff8, BX_IA_JNB_Jd },
  /* 74 /d */ { BxImmediate_BrOff8, BX_IA_JZ_Jd },
  /* 75 /d */ { BxImmediate_BrOff8, BX_IA_JNZ_Jd },
  /* 76 /d */ { BxImmediate_BrOff8, BX_IA_JBE_Jd },
  /* 77 /d */ { BxImmediate_BrOff8, BX_IA_JNBE_Jd },
  /* 78 /d */ { BxImmediate_BrOff8, BX_IA_JS_Jd },
  /* 79 /d */ { BxImmediate_BrOff8, BX_IA_JNS_Jd },
  /* 7A /d */ { BxImmediate_BrOff8, BX_IA_JP_Jd },
  /* 7B /d */ { BxImmediate_BrOff8, BX_IA_JNP_Jd },
  /* 7C /d */ { BxImmediate_BrOff8, BX_IA_JL_Jd },
  /* 7D /d */ { BxImmediate_BrOff8, BX_IA_JNL_Jd },
  /* 7E /d */ { BxImmediate_BrOff8, BX_IA_JLE_Jd },
  /* 7F /d */ { BxImmediate_BrOff8, BX_IA_JNLE_Jd },
  /* 80 /d */ { BxGroup1 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG1EbIb },
  /* 81 /d */ { BxGroup1 | BxImmediate_Id, BX_IA_ERROR, BxOpcodeInfoG1Ed },
  /* 82 /d */ { BxGroup1 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG1EbIb },
  /* 83 /d */ { BxGroup1 | BxImmediate_Ib_SE, BX_IA_ERROR, BxOpcodeInfoG1Ed },
  /* 84 /d */ { 0, BX_IA_TEST_EbGb },
  /* 85 /d */ { 0, BX_IA_TEST_EdGd },
  /* 86 /d */ { BxLockable, BX_IA_XCHG_EbGb },
  /* 87 /d */ { BxLockable, BX_IA_XCHG_EdGd },
  /* 88 /d */ { 0, BX_IA_MOV_EbGb },
  /* 89 /d */ { 0, BX_IA_MOV_Op32_EdGd },
  /* 8A /d */ { 0, BX_IA_MOV_GbEb },
  /* 8B /d */ { 0, BX_IA_MOV_Op32_GdEd },
  /* 8C /d */ { 0, BX_IA_MOV_EwSw },
  /* 8D /d */ { 0, BX_IA_LEA_GdM },
  /* 8E /d */ { 0, BX_IA_MOV_SwEw },
  /* 8F /d */ { BxGroup1A, BX_IA_ERROR, BxOpcodeInfoG1AEd },
  /* 90 /d */ { BxPrefixSSE, BX_IA_NOP, BxOpcodeGroupSSE_PAUSE },
  /* 91 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 92 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 93 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 94 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 95 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 96 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 97 /d */ { 0, BX_IA_XCHG_ERXEAX },
  /* 98 /d */ { 0, BX_IA_CWDE },
  /* 99 /d */ { 0, BX_IA_CDQ },
  /* 9A /d */ { BxImmediate_Id | BxImmediate_Iw2, BX_IA_CALLF_Op32_Ap },
  /* 9B /d */ { 0, BX_IA_FWAIT },
  /* 9C /d */ { 0, BX_IA_PUSHF_Fd },
  /* 9D /d */ { 0, BX_IA_POPF_Fd },
  /* 9E /d */ { 0, BX_IA_SAHF },
  /* 9F /d */ { 0, BX_IA_LAHF },
  /* A0 /d */ { BxImmediate_O, BX_IA_MOV_ALOd },
  /* A1 /d */ { BxImmediate_O, BX_IA_MOV_EAXOd },
  /* A2 /d */ { BxImmediate_O, BX_IA_MOV_OdAL },
  /* A3 /d */ { BxImmediate_O, BX_IA_MOV_OdEAX },
  /* A4 /d */ { 0, BX_IA_REP_MOVSB_YbXb },
  /* A5 /d */ { 0, BX_IA_REP_MOVSD_YdXd },
  /* A6 /d */ { 0, BX_IA_REP_CMPSB_XbYb },
  /* A7 /d */ { 0, BX_IA_REP_CMPSD_XdYd },
  /* A8 /d */ { BxImmediate_Ib, BX_IA_TEST_ALIb },
  /* A9 /d */ { BxImmediate_Id, BX_IA_TEST_EAXId },
  /* AA /d */ { 0, BX_IA_REP_STOSB_YbAL },
  /* AB /d */ { 0, BX_IA_REP_STOSD_YdEAX },
  /* AC /d */ { 0, BX_IA_REP_LODSB_ALXb },
  /* AD /d */ { 0, BX_IA_REP_LODSD_EAXXd },
  /* AE /d */ { 0, BX_IA_REP_SCASB_ALYb  },
  /* AF /d */ { 0, BX_IA_REP_SCASD_EAXYd },
  /* B0 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B1 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B2 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B3 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B4 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B5 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B6 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B7 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B8 /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* B9 /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BA /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BB /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BC /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BD /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BE /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* BF /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* C0 /d */ { BxGroup2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG2EbIb },
  /* C1 /d */ { BxGroup2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoG2EdIb },
  /* C2 /d */ { BxImmediate_Iw, BX_IA_RET_Op32_Iw },
  /* C3 /d */ { 0, BX_IA_RET_Op32 },
  /* C4 /d */ { 0, BX_IA_LES_GdMp },
  /* C5 /d */ { 0, BX_IA_LDS_GdMp },
  /* C6 /d */ { BxGroup11, BX_IA_ERROR, BxOpcodeInfoG11Eb },
  /* C7 /d */ { BxGroup11, BX_IA_ERROR, BxOpcodeInfoG11Ed },
  /* C8 /d */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op32_IwIb },
  /* C9 /d */ { 0, BX_IA_LEAVE_Op32 },
  /* CA /d */ { BxImmediate_Iw, BX_IA_RETF_Op32_Iw },
  /* CB /d */ { 0, BX_IA_RETF_Op32 },
  /* CC /d */ { 0, BX_IA_INT3 },
  /* CD /d */ { BxImmediate_Ib, BX_IA_INT_Ib },
  /* CE /d */ { 0, BX_IA_INTO },
  /* CF /d */ { 0, BX_IA_IRET_Op32 },
  /* D0 /d */ { BxGroup2 | BxImmediate_I1, BX_IA_ERROR, BxOpcodeInfoG2EbIb },
  /* D1 /d */ { BxGroup2 | BxImmediate_I1, BX_IA_ERROR, BxOpcodeInfoG2EdIb },
  /* D2 /d */ { BxGroup2, BX_IA_ERROR, BxOpcodeInfoG2Eb },
  /* D3 /d */ { BxGroup2, BX_IA_ERROR, BxOpcodeInfoG2Ed },
  /* D4 /d */ { BxImmediate_Ib, BX_IA_AAM },
  /* D5 /d */ { BxImmediate_Ib, BX_IA_AAD },
  /* D6 /d */ { 0, BX_IA_SALC },
  /* D7 /d */ { 0, BX_IA_XLAT },
#if BX_SUPPORT_FPU
  /* D8 /d */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupD8 },
  /* D9 /d */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointD9 },
  /* DA /d */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDA },
  /* DB /d */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDB },
  /* DC /d */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupDC },
  /* DD /d */ { BxGroupFP, BX_IA_ERROR, BxOpcodeInfo_FPGroupDD },
  /* DE /d */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDE },
  /* DF /d */ { BxFPEscape, BX_IA_ERROR, BxOpcodeInfo_FloatingPointDF },
#else
  /* D8 /d */ { 0, BX_IA_FPUESC },
  /* D9 /d */ { 0, BX_IA_FPUESC },
  /* DA /d */ { 0, BX_IA_FPUESC },
  /* DB /d */ { 0, BX_IA_FPUESC },
  /* DC /d */ { 0, BX_IA_FPUESC },
  /* DD /d */ { 0, BX_IA_FPUESC },
  /* DE /d */ { 0, BX_IA_FPUESC },
  /* DF /d */ { 0, BX_IA_FPUESC },
#endif
  /* E0 /d */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op32_Jb },
  /* E1 /d */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op32_Jb },
  /* E2 /d */ { BxImmediate_BrOff8, BX_IA_LOOP_Op32_Jb },
  /* E3 /d */ { BxImmediate_BrOff8, BX_IA_JECXZ_Jb },
  /* E4 /d */ { BxImmediate_Ib, BX_IA_IN_ALIb },
  /* E5 /d */ { BxImmediate_Ib, BX_IA_IN_EAXIb },
  /* E6 /d */ { BxImmediate_Ib, BX_IA_OUT_IbAL },
  /* E7 /d */ { BxImmediate_Ib, BX_IA_OUT_IbEAX },
  /* E8 /d */ { BxImmediate_BrOff32, BX_IA_CALL_Jd },
  /* E9 /d */ { BxImmediate_BrOff32, BX_IA_JMP_Jd },
  /* EA /d */ { BxImmediate_Id | BxImmediate_Iw2, BX_IA_JMPF_Ap },
  /* EB /d */ { BxImmediate_BrOff8, BX_IA_JMP_Jd },
  /* EC /d */ { 0, BX_IA_IN_ALDX },
  /* ED /d */ { 0, BX_IA_IN_EAXDX },
  /* EE /d */ { 0, BX_IA_OUT_DXAL },
  /* EF /d */ { 0, BX_IA_OUT_DXEAX },
  /* F0 /d */ { 0, BX_IA_ERROR }, // LOCK:
  /* F1 /d */ { 0, BX_IA_INT1 },
  /* F2 /d */ { 0, BX_IA_ERROR }, // REPNE/REPNZ
  /* F3 /d */ { 0, BX_IA_ERROR }, // REP,REPE/REPZ
  /* F4 /d */ { 0, BX_IA_HLT },
  /* F5 /d */ { 0, BX_IA_CMC },
  /* F6 /d */ { BxGroup3, BX_IA_ERROR, BxOpcodeInfoG3Eb },
  /* F7 /d */ { BxGroup3, BX_IA_ERROR, BxOpcodeInfoG3Ed },
  /* F8 /d */ { 0, BX_IA_CLC },
  /* F9 /d */ { 0, BX_IA_STC },
  /* FA /d */ { 0, BX_IA_CLI },
  /* FB /d */ { 0, BX_IA_STI },
  /* FC /d */ { 0, BX_IA_CLD },
  /* FD /d */ { 0, BX_IA_STD },
  /* FE /d */ { BxGroup4, BX_IA_ERROR, BxOpcodeInfoG4 },
  /* FF /d */ { BxGroup5, BX_IA_ERROR, BxOpcodeInfoG5d },

  /* 0F 00 /d */ { BxGroup6, BX_IA_ERROR, BxOpcodeInfoG6 },
  /* 0F 01 /d */ { BxGroup7, BX_IA_ERROR, BxOpcodeInfoG7 },
  /* 0F 02 /d */ { 0, BX_IA_LAR_GdEw },
  /* 0F 03 /d */ { 0, BX_IA_LSL_GdEw },
  /* 0F 04 /d */ { 0, BX_IA_ERROR },
  /* 0F 05 /d */ { 0, BX_IA_SYSCALL_LEGACY },
  /* 0F 06 /d */ { 0, BX_IA_CLTS },
  /* 0F 07 /d */ { 0, BX_IA_SYSRET_LEGACY },
  /* 0F 08 /d */ { 0, BX_IA_INVD },
  /* 0F 09 /d */ { 0, BX_IA_WBINVD },
  /* 0F 0A /d */ { 0, BX_IA_ERROR },
  /* 0F 0B /d */ { 0, BX_IA_UD2A },
  /* 0F 0C /d */ { 0, BX_IA_ERROR },
  /* 0F 0D /d */ { 0, BX_IA_PREFETCHW },          // 3DNow! PREFETCHW on AMD, NOP on Intel
  /* 0F 0E /d */ { 0, BX_IA_FEMMS },              // 3DNow! FEMMS
  /* 0F 0F /d */ { BxImmediate_Ib, BX_IA_ERROR }, // 3DNow! Opcode Table
  /* 0F 10 /d */ { BxPrefixSSE, BX_IA_MOVUPS_VpsWps, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /d */ { BxPrefixSSE, BX_IA_MOVUPS_WpsVps, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /d */ { BxPrefixSSE, BX_IA_MOVLPS_VpsMq, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f13M },
  /* 0F 14 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /d */ { BxPrefixSSE, BX_IA_MOVHPS_VpsMq, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f17M },
#if BX_CPU_LEVEL >= 6
  /* 0F 18 /d */ { BxGroup16, BX_IA_ERROR, BxOpcodeInfoG16 }, // opcode group G16, PREFETCH hints
  /* 0F 19 /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1A /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1B /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1C /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1D /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1E /d */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 1F /d */ { 0, BX_IA_NOP },      // multi-byte NOP
#else
  /* 0F 18 /d */ { 0, BX_IA_ERROR },
  /* 0F 19 /d */ { 0, BX_IA_ERROR },
  /* 0F 1A /d */ { 0, BX_IA_ERROR },
  /* 0F 1B /d */ { 0, BX_IA_ERROR },
  /* 0F 1C /d */ { 0, BX_IA_ERROR },
  /* 0F 1D /d */ { 0, BX_IA_ERROR },
  /* 0F 1E /d */ { 0, BX_IA_ERROR },
  /* 0F 1F /d */ { 0, BX_IA_ERROR },
#endif
  /* 0F 20 /d */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoMOV_RdCd },
  /* 0F 21 /d */ { 0, BX_IA_MOV_RdDd },
  /* 0F 22 /d */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoMOV_CdRd },
  /* 0F 23 /d */ { 0, BX_IA_MOV_DdRd },
  /* 0F 24 /d */ { 0, BX_IA_ERROR }, // BX_IA_MOV_RdTd not implemented
  /* 0F 25 /d */ { 0, BX_IA_ERROR },
  /* 0F 26 /d */ { 0, BX_IA_ERROR }, // BX_IA_MOV_TdRd not implemented
  /* 0F 27 /d */ { 0, BX_IA_ERROR },
  /* 0F 28 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /d */ { BxPrefixSSE, BX_IA_CVTPI2PS_VpsQq, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /d */ { BxPrefixSSE, BX_IA_MOVNTPS_MpsVps, BxOpcodeGroupSSE_0f2bM },
  /* 0F 2C /d */ { BxPrefixSSE, BX_IA_CVTTPS2PI_PqWps, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /d */ { BxPrefixSSE, BX_IA_CVTPS2PI_PqWps, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /d */ { 0, BX_IA_WRMSR },
  /* 0F 31 /d */ { 0, BX_IA_RDTSC }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 32 /d */ { 0, BX_IA_RDMSR }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 33 /d */ { 0, BX_IA_RDPMC },
  /* 0F 34 /d */ { 0, BX_IA_SYSENTER },
  /* 0F 35 /d */ { 0, BX_IA_SYSEXIT },
  /* 0F 36 /d */ { 0, BX_IA_ERROR },
  /* 0F 37 /d */ { 0, BX_IA_GETSEC },
#if BX_CPU_LEVEL >= 6
  /* 0F 38 /d */ { 0, BX_IA_ERROR, BxOpcode3ByteTable0f38 }, // 3-byte escape
#else
  /* 0F 38 /d */ { 0, BX_IA_ERROR },
#endif
  /* 0F 39 /d */ { 0, BX_IA_ERROR },
#if BX_CPU_LEVEL >= 6
  /* 0F 3A /d */ { BxImmediate_Ib, BX_IA_ERROR, BxOpcode3ByteTable0f3a }, // 3-byte escape
#else
  /* 0F 3A /d */ { 0, BX_IA_ERROR },
#endif
  /* 0F 3B /d */ { 0, BX_IA_ERROR },
  /* 0F 3C /d */ { 0, BX_IA_ERROR },
  /* 0F 3D /d */ { 0, BX_IA_ERROR },
  /* 0F 3E /d */ { 0, BX_IA_ERROR },
  /* 0F 3F /d */ { 0, BX_IA_ERROR },
  /* 0F 40 /d */ { 0, BX_IA_CMOVO_GdEd },
  /* 0F 41 /d */ { 0, BX_IA_CMOVNO_GdEd },
  /* 0F 42 /d */ { 0, BX_IA_CMOVB_GdEd },
  /* 0F 43 /d */ { 0, BX_IA_CMOVNB_GdEd },
  /* 0F 44 /d */ { 0, BX_IA_CMOVZ_GdEd },
  /* 0F 45 /d */ { 0, BX_IA_CMOVNZ_GdEd },
  /* 0F 46 /d */ { 0, BX_IA_CMOVBE_GdEd },
  /* 0F 47 /d */ { 0, BX_IA_CMOVNBE_GdEd },
  /* 0F 48 /d */ { 0, BX_IA_CMOVS_GdEd },
  /* 0F 49 /d */ { 0, BX_IA_CMOVNS_GdEd },
  /* 0F 4A /d */ { 0, BX_IA_CMOVP_GdEd },
  /* 0F 4B /d */ { 0, BX_IA_CMOVNP_GdEd },
  /* 0F 4C /d */ { 0, BX_IA_CMOVL_GdEd },
  /* 0F 4D /d */ { 0, BX_IA_CMOVNL_GdEd },
  /* 0F 4E /d */ { 0, BX_IA_CMOVLE_GdEd },
  /* 0F 4F /d */ { 0, BX_IA_CMOVNLE_GdEd },
  /* 0F 50 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f50R },
  /* 0F 51 /d */ { BxAliasSSE, BX_IA_SQRTPS_VpsWps },
  /* 0F 52 /d */ { BxPrefixSSE, BX_IA_RSQRTPS_VpsWps, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /d */ { BxPrefixSSE, BX_IA_RCPPS_VpsWps, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /d */ { BxAliasSSE, BX_IA_ADDPS_VpsWps },
  /* 0F 59 /d */ { BxAliasSSE, BX_IA_MULPS_VpsWps },
  /* 0F 5A /d */ { BxAliasSSE, BX_IA_CVTPS2PD_VpdWps },
  /* 0F 5B /d */ { BxPrefixSSE, BX_IA_CVTDQ2PS_VpsWdq, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /d */ { BxAliasSSE, BX_IA_SUBPS_VpsWps },
  /* 0F 5D /d */ { BxAliasSSE, BX_IA_MINPS_VpsWps },
  /* 0F 5E /d */ { BxAliasSSE, BX_IA_DIVPS_VpsWps },
  /* 0F 5F /d */ { BxAliasSSE, BX_IA_MAXPS_VpsWps },
  /* 0F 60 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f61 },
  /* 0F 62 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f62 },
  /* 0F 63 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f63 },
  /* 0F 64 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f64 },
  /* 0F 65 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f65 },
  /* 0F 66 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f66 },
  /* 0F 67 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f67 },
  /* 0F 68 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f68 },
  /* 0F 69 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f69 },
  /* 0F 6A /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6a },
  /* 0F 6B /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6b },
  /* 0F 6C /d */ { BxPrefixSSE66, BX_IA_PUNPCKLQDQ_VdqWdq },
  /* 0F 6D /d */ { BxPrefixSSE66, BX_IA_PUNPCKHQDQ_VdqWdq },
  /* 0F 6E /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f6e },
  /* 0F 6F /d */ { BxPrefixSSE, BX_IA_MOVQ_PqQq, BxOpcodeGroupSSE_0f6f },
  /* 0F 70 /d */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_PSHUFW_PqQqIb, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /d */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoG12R },
  /* 0F 72 /d */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoG13R },
  /* 0F 73 /d */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoG14R },
  /* 0F 74 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f74 },
  /* 0F 75 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f75 },
  /* 0F 76 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f76 },
  /* 0F 77 /d */ { BxPrefixSSE, BX_IA_EMMS, BxOpcodeGroupSSE_ERR },
  /* 0F 78 /d */ { BxPrefixSSE, BX_IA_VMREAD_EdGd, BxOpcodeGroupSSE4A_0f78 },
  /* 0F 79 /d */ { BxPrefixSSE, BX_IA_VMWRITE_GdEd, BxOpcodeGroupSSE4A_0f79 },
  /* 0F 7A /d */ { 0, BX_IA_ERROR },
  /* 0F 7B /d */ { 0, BX_IA_ERROR },
  /* 0F 7C /d */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f7c },
  /* 0F 7D /d */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f7d },
  /* 0F 7E /d */ { BxPrefixSSE, BX_IA_MOVD_EdPq, BxOpcodeGroupSSE_0f7e },
  /* 0F 7F /d */ { BxPrefixSSE, BX_IA_MOVQ_QqPq, BxOpcodeGroupSSE_0f7f },
  /* 0F 80 /d */ { BxImmediate_BrOff32, BX_IA_JO_Jd },
  /* 0F 81 /d */ { BxImmediate_BrOff32, BX_IA_JNO_Jd },
  /* 0F 82 /d */ { BxImmediate_BrOff32, BX_IA_JB_Jd },
  /* 0F 83 /d */ { BxImmediate_BrOff32, BX_IA_JNB_Jd },
  /* 0F 84 /d */ { BxImmediate_BrOff32, BX_IA_JZ_Jd },
  /* 0F 85 /d */ { BxImmediate_BrOff32, BX_IA_JNZ_Jd },
  /* 0F 86 /d */ { BxImmediate_BrOff32, BX_IA_JBE_Jd },
  /* 0F 87 /d */ { BxImmediate_BrOff32, BX_IA_JNBE_Jd },
  /* 0F 88 /d */ { BxImmediate_BrOff32, BX_IA_JS_Jd },
  /* 0F 89 /d */ { BxImmediate_BrOff32, BX_IA_JNS_Jd },
  /* 0F 8A /d */ { BxImmediate_BrOff32, BX_IA_JP_Jd },
  /* 0F 8B /d */ { BxImmediate_BrOff32, BX_IA_JNP_Jd },
  /* 0F 8C /d */ { BxImmediate_BrOff32, BX_IA_JL_Jd },
  /* 0F 8D /d */ { BxImmediate_BrOff32, BX_IA_JNL_Jd },
  /* 0F 8E /d */ { BxImmediate_BrOff32, BX_IA_JLE_Jd },
  /* 0F 8F /d */ { BxImmediate_BrOff32, BX_IA_JNLE_Jd },
  /* 0F 90 /d */ { 0, BX_IA_SETO_Eb },
  /* 0F 91 /d */ { 0, BX_IA_SETNO_Eb },
  /* 0F 92 /d */ { 0, BX_IA_SETB_Eb },
  /* 0F 93 /d */ { 0, BX_IA_SETNB_Eb },
  /* 0F 94 /d */ { 0, BX_IA_SETZ_Eb },
  /* 0F 95 /d */ { 0, BX_IA_SETNZ_Eb },
  /* 0F 96 /d */ { 0, BX_IA_SETBE_Eb },
  /* 0F 97 /d */ { 0, BX_IA_SETNBE_Eb },
  /* 0F 98 /d */ { 0, BX_IA_SETS_Eb },
  /* 0F 99 /d */ { 0, BX_IA_SETNS_Eb },
  /* 0F 9A /d */ { 0, BX_IA_SETP_Eb },
  /* 0F 9B /d */ { 0, BX_IA_SETNP_Eb },
  /* 0F 9C /d */ { 0, BX_IA_SETL_Eb },
  /* 0F 9D /d */ { 0, BX_IA_SETNL_Eb },
  /* 0F 9E /d */ { 0, BX_IA_SETLE_Eb },
  /* 0F 9F /d */ { 0, BX_IA_SETNLE_Eb },
  /* 0F A0 /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 0F A1 /d */ { 0, BX_IA_POP_Op32_Sw },
  /* 0F A2 /d */ { 0, BX_IA_CPUID },
  /* 0F A3 /d */ { 0, BX_IA_BT_EdGd },
  /* 0F A4 /d */ { BxImmediate_Ib, BX_IA_SHLD_EdGdIb },
  /* 0F A5 /d */ { 0,              BX_IA_SHLD_EdGd   },
  /* 0F A6 /d */ { 0, BX_IA_ERROR }, // CMPXCHG_XBTS not implemented
  /* 0F A7 /d */ { 0, BX_IA_ERROR }, // CMPXCHG_IBTS not implemented
  /* 0F A8 /d */ { 0, BX_IA_PUSH_Op32_Sw },
  /* 0F A9 /d */ { 0, BX_IA_POP_Op32_Sw },
  /* 0F AA /d */ { 0, BX_IA_RSM },
  /* 0F AB /d */ { BxLockable, BX_IA_BTS_EdGd },
  /* 0F AC /d */ { BxImmediate_Ib, BX_IA_SHRD_EdGdIb },
  /* 0F AD /d */ { 0,              BX_IA_SHRD_EdGd   },
  /* 0F AE /d */ { BxGroup15, BX_IA_ERROR, BxOpcodeInfoG15 },
  /* 0F AF /d */ { 0, BX_IA_IMUL_GdEd },
  /* 0F B0 /d */ { BxLockable, BX_IA_CMPXCHG_EbGb },
  /* 0F B1 /d */ { BxLockable, BX_IA_CMPXCHG_EdGd },
  /* 0F B2 /d */ { 0, BX_IA_LSS_GdMp },
  /* 0F B3 /d */ { BxLockable, BX_IA_BTR_EdGd },
  /* 0F B4 /d */ { 0, BX_IA_LFS_GdMp },
  /* 0F B5 /d */ { 0, BX_IA_LGS_GdMp },
  /* 0F B6 /d */ { 0, BX_IA_MOVZX_GdEb },
  /* 0F B7 /d */ { 0, BX_IA_MOVZX_GdEw },
  /* 0F B8 /d */ { BxPrefixSSEF3, BX_IA_POPCNT_GdEd },
  /* 0F B9 /d */ { 0, BX_IA_UD2B },
  /* 0F BA /d */ { BxGroup8, BX_IA_ERROR, BxOpcodeInfoG8EdIb },
  /* 0F BB /d */ { BxLockable, BX_IA_BTC_EdGd },
  /* 0F BC /d */ { BxPrefixSSE, BX_IA_BSF_GdEd, BxOpcodeGroupSSE_TZCNT32 },
  /* 0F BD /d */ { BxPrefixSSE, BX_IA_BSR_GdEd, BxOpcodeGroupSSE_LZCNT32 },
  /* 0F BE /d */ { 0, BX_IA_MOVSX_GdEb },
  /* 0F BF /d */ { 0, BX_IA_MOVSX_GdEw },
  /* 0F C0 /d */ { BxLockable, BX_IA_XADD_EbGb },
  /* 0F C1 /d */ { BxLockable, BX_IA_XADD_EdGd },
  /* 0F C2 /d */ { BxAliasSSE | BxImmediate_Ib, BX_IA_CMPPS_VpsWpsIb },
  /* 0F C3 /d */ { BxPrefixSSE, BX_IA_MOVNTI_Op32_MdGd, BxOpcodeGroupSSE_ERR },
  /* 0F C4 /d */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /d */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc5R },
  /* 0F C6 /d */ { BxPrefixSSE2 | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /d */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9d },
  /* 0F C8 /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F C9 /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CA /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CB /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CC /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CD /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CE /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F CF /d */ { 0, BX_IA_BSWAP_ERX },
  /* 0F D0 /d */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd5 },
  /* 0F D6 /d */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd7R },
  /* 0F D8 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fda },
  /* 0F DB /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fde },
  /* 0F DF /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe1 },
  /* 0F E2 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe2 },
  /* 0F E3 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /d */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe7M },
  /* 0F E8 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fea },
  /* 0F EB /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0feb },
  /* 0F EC /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fec },
  /* 0F ED /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fed },
  /* 0F EE /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fee },
  /* 0F EF /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /d */ { BxPrefixSSEF2, BX_IA_LDDQU_VdqMdq },
  /* 0F F1 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff1 },
  /* 0F F2 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff2 },
  /* 0F F3 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff3 },
  /* 0F F4 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff4 },
  /* 0F F5 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff5 },
  /* 0F F6 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff6 },
  /* 0F F7 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff7R },
  /* 0F F8 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff8 },
  /* 0F F9 /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ff9 },
  /* 0F FA /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffa },
  /* 0F FB /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffb },
  /* 0F FC /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffc },
  /* 0F FD /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffd },
  /* 0F FE /d */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0ffe },
  /* 0F FF /d */ { 0, BX_IA_ERROR }
};

  int BX_CPP_AttrRegparmN(3)
BX_CPU_C::fetchDecode32(const Bit8u *iptr, Bit32u fetchModeMask, bxInstruction_c *i, unsigned remainingInPage)
{
  if (remainingInPage > 15) remainingInPage = 15;

  unsigned remain = remainingInPage; // remain must be at least 1
  bx_bool is_32, lock=0;
  unsigned b1, b2 = 0, os_32, ia_opcode = BX_IA_ERROR, imm_mode = 0;
  unsigned rm = 0, mod=0, nnn=0, mod_mem = 0;
  unsigned seg = BX_SEG_REG_DS, seg_override = BX_SEG_REG_NULL;

#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F3   2
#define SSE_PREFIX_F2   3
  unsigned sse_prefix = SSE_PREFIX_NONE;

  bx_bool vex_w = 0;
#if BX_SUPPORT_AVX
  int had_vex_xop = 0, vvv = -1;
  bx_bool use_vvv = 0;
#endif
#if BX_SUPPORT_EVEX
  bx_bool displ8 = 0;
#endif

  os_32 = is_32 = fetchModeMask & BX_FETCH_MODE_IS32_MASK;

  i->ResolveModrm = 0;
  i->init(/*os32*/ is_32,  /*as32*/ is_32,
          /*os64*/     0,  /*as64*/     0);

fetch_b1:
  b1 = *iptr++;
  remain--;

  switch (b1) {
    case 0x0f: // 2-byte escape
      if (remain != 0) {
        remain--;
        b1 = 0x100 | *iptr++;
        break;
      }
      return(-1);
    case 0x66: // OpSize
      os_32 = !is_32;
      if(!sse_prefix) sse_prefix = SSE_PREFIX_66;
      i->setOs32B(os_32);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x67: // AddrSize
      i->setAs32B(!is_32);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0xf2: // REPNE/REPNZ
    case 0xf3: // REP/REPE/REPZ
      sse_prefix = (b1 & 3) ^ 1;
      i->setLockRepUsed(b1 & 3);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x26: // ES:
    case 0x2e: // CS:
    case 0x36: // SS:
    case 0x3e: // DS:
      seg_override = (b1 >> 3) & 3;
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x64: // FS:
    case 0x65: // GS:
      seg_override = (b1 & 0xf);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0xf0: // LOCK:
      lock = 1;
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    default:
      break;
  }

  i->modRMForm.Id = 0;

  unsigned index = b1 + (os_32 << 9); // *512

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(BxOpcodeInfo32[index]);
  Bit16u attr = OpcodeInfoPtr->Attr;

  bx_bool has_modrm = 0;

#if BX_SUPPORT_AVX
  if ((b1 & ~0x1) == 0xc4 && (*iptr & 0xc0) == 0xc0) {
    // VEX 0xC4 and VEX 0xC5
    had_vex_xop = b1;
    if (sse_prefix || ! protected_mode())
      goto decode_done;

    unsigned vex, vex_opcext = 1;
    if (remain != 0) {
      remain--;
      vex = *iptr++;
    }
    else
      return(-1);

    if (b1 == 0xc4) {
      // decode 3-byte VEX prefix
      vex_opcext = vex & 0x1f;
      if (remain != 0) {
        remain--;
        vex = *iptr++;  // fetch VEX3
      }
      else
        return(-1);

      vex_w = (vex >> 7) & 0x1;
    }

    vvv = 15 - ((vex >> 3) & 0xf);
    unsigned vex_l = (vex >> 2) & 0x1;
    i->setVL(BX_VL128 + vex_l);
    sse_prefix = vex & 0x3;

    unsigned opcode_byte = 0;
    if (remain != 0) {
      remain--;
      opcode_byte = *iptr++;
    }
    else
      return(-1);

    opcode_byte += 256 * vex_opcext;
    if (opcode_byte < 256 || opcode_byte >= 1024) goto decode_done;
    has_modrm = (opcode_byte != 0x177); // if not VZEROUPPER/VZEROALL opcode

    OpcodeInfoPtr = &BxOpcodeTableAVX[(opcode_byte-256)*2 + vex_l];
  }
#if BX_SUPPORT_EVEX
  else if (b1 == 0x62 && (*iptr & 0xc0) == 0xc0) {
    had_vex_xop = b1;
    if (sse_prefix || ! protected_mode())
      goto decode_done;

    Bit32u evex;
    if (remain > 3) {
      evex = FetchDWORD(iptr);
      iptr += 4;
      remain -= 4;
    }
    else {
      return(-1);
    }

    // check for reserved EVEX bits
    if ((evex & 0x0c) != 0 || (evex & 0x400) == 0)
      goto decode_done;

    unsigned evex_opcext = evex & 0x3;
    if (evex_opcext == 0)
      goto decode_done;

    sse_prefix = (evex >> 8) & 0x3;
    vvv = 15 - ((evex >> 11) & 0xf);
    if (vvv >= 8)
      goto decode_done;

    vex_w = (evex >> 15) & 0x1;
    unsigned opmask = (evex >> 16) & 0x7;
    i->setOpmask(opmask);
    unsigned evex_v = ((evex >> 19) & 0x1) ^ 0x1;
    if (evex_v)
      goto decode_done;
    unsigned evex_b = (evex >> 20) & 0x1;
    i->setEvexb(evex_b);

    unsigned evex_vl_rc = (evex >> 21) & 0x3;
    i->setRC(evex_vl_rc);
    i->setVL(1 << evex_vl_rc);

    unsigned evex_z = (evex >> 23) & 0x1;
    i->setZeroMasking(evex_z);

    if (evex_z && ! opmask)
      goto decode_done;
    
    unsigned opcode_byte = (evex >> 24);
    opcode_byte += 256 * (evex_opcext-1);
    has_modrm = 1;

    OpcodeInfoPtr = &BxOpcodeTableEVEX[opcode_byte*2 + (opmask != 0)];
  }
#endif
  else if (b1 == 0x8f && (*iptr & 0xc8) == 0xc8) {
    // 3 byte XOP prefix
    had_vex_xop = b1;
    if (sse_prefix || ! protected_mode())
      goto decode_done;

    unsigned vex;
    if (remain > 2) {
      remain -= 3;
      vex = *iptr++; // fetch XOP2
    }
    else
      return(-1);

    unsigned xop_opcext = (vex & 0x1f) - 8;
    if (xop_opcext >= 3)
      goto decode_done;

    vex = *iptr++; // fetch XOP3

    vex_w = (vex >> 7) & 0x1;
    vvv = 15 - ((vex >> 3) & 0xf);
    unsigned vex_l = (vex >> 2) & 0x1;
    i->setVL(BX_VL128 + vex_l);
    sse_prefix = vex & 0x3;
    if (sse_prefix) goto decode_done;

    unsigned opcode_byte = *iptr++;
    has_modrm = 1;
    opcode_byte += 256 * xop_opcext;

    OpcodeInfoPtr = &BxOpcodeTableXOP[opcode_byte];
  }
  else
#endif
  {
    has_modrm = BxOpcodeHasModrm32[b1];
  }

  if (has_modrm) {

#if BX_CPU_LEVEL >= 6
    // handle 3-byte escape
    if (b1 == 0x138 || b1 == 0x13a) {
      if (remain != 0) {
        remain--;
        unsigned b3 = *iptr++;
        OpcodeInfoPtr = &OpcodeInfoPtr->AnotherArray[b3];
      }
      else
        return(-1);
    }
#endif

    // opcode requires modrm byte
    if (remain != 0) {
      remain--;
      b2 = *iptr++;
    }
    else
      return(-1);

    // Parse mod-nnn-rm and related bytes
    mod = b2 & 0xc0; // leave unshifted
    nnn = (b2 >> 3) & 0x7;
    rm  = b2 & 0x7;

    if (b1 >= 0xd8 && b1 <= 0xdf)
      i->setFoo((b2 | (b1 << 8)) & 0x7ff); /* for x87 */

    // MOVs with CRx and DRx always use register ops and ignore the mod field.
    if ((b1 & ~3) == 0x120)
      mod = 0xc0;

    if (mod == 0xc0) { // mod == 11b
      i->assertModC0();
      goto modrm_done;
    }

    mod_mem = 1;
    i->setSibBase(rm);      // initialize with rm to use BxResolve32Base
    i->setSibIndex(BX_NIL_REGISTER);
    // initialize displ32 with zero to include cases with no diplacement
    i->modRMForm.displ32u = 0;

    if (i->as32L()) {
      // 32-bit addressing modes; note that mod==11b handled above
      i->ResolveModrm = &BX_CPU_C::BxResolve32Base;
      if (rm != 4) { // no s-i-b byte
        if (mod == 0x00) { // mod == 00b
          if (rm == 5) {
            i->setSibBase(BX_NIL_REGISTER);
            if (remain > 3) {
              i->modRMForm.displ32u = FetchDWORD(iptr);
              iptr += 4;
              remain -= 4;
            }
            else return(-1);
          }
          // mod==00b, rm!=4, rm!=5
          goto modrm_done;
        }
        seg = sreg_mod1or2_base32[rm];
      }
      else { // mod!=11b, rm==4, s-i-b byte follows
        unsigned sib, base, index, scale;
        if (remain != 0) {
          sib = *iptr++;
          remain--;
        }
        else {
          return(-1);
        }
        base  = sib & 0x7; sib >>= 3;
        index = sib & 0x7; sib >>= 3;
        scale = sib;
        i->setSibScale(scale);
        i->setSibBase(base);
        // this part is a little tricky - assign index value always,
        // it will be really used if the instruction is Gather. Others
        // assume that BxResolve32Base will do the right thing.
        i->setSibIndex(index);
        if (index != 4) {
          i->ResolveModrm = &BX_CPU_C::BxResolve32BaseIndex;
        }
        if (mod == 0x00) { // mod==00b, rm==4
          seg = sreg_mod0_base32[base];
          if (base == 5) {
            i->setSibBase(BX_NIL_REGISTER);
            if (remain > 3) {
              i->modRMForm.displ32u = FetchDWORD(iptr);
              iptr += 4;
              remain -= 4;
            }
            else {
              return(-1);
            }
          }
          // mod==00b, rm==4, base!=5
          goto modrm_done;
        }
        seg = sreg_mod1or2_base32[base];
      }

      if (mod == 0x40) { // mod==01b
        if (remain != 0) {
          // 8 sign extended to 32
          i->modRMForm.displ32u = (Bit8s) *iptr++;
#if BX_SUPPORT_EVEX
          displ8 = 1;
#endif
          remain--;
          goto modrm_done;
        }
        else {
          return(-1);
        }
      }

      // (mod == 0x80),     mod==10b
      if (remain > 3) {
        i->modRMForm.displ32u = FetchDWORD(iptr);
        iptr += 4;
        remain -= 4;
      }
      else {
        return(-1);
      }
    }
    else {
      // 16-bit addressing modes, mod==11b handled above
      i->ResolveModrm = &BX_CPU_C::BxResolve16BaseIndex;
      i->setSibBase(Resolve16BaseReg[rm]);
      i->setSibIndex(Resolve16IndexReg[rm]);
      i->setSibScale(0);
      if (mod == 0x00) { // mod == 00b
        seg = sreg_mod00_rm16[rm];
        if (rm == 6) {
          i->setSibBase(BX_NIL_REGISTER);
          if (remain > 1) {
            i->modRMForm.displ16u = FetchWORD(iptr);
            iptr += 2;
            remain -= 2;
            goto modrm_done;
          }
          else return(-1);
        }
        goto modrm_done;
      }
      seg = sreg_mod01or10_rm16[rm];
      if (mod == 0x40) { // mod == 01b
        if (remain != 0) {
          // 8 sign extended to 16
          i->modRMForm.displ16u = (Bit8s) *iptr++;
#if BX_SUPPORT_EVEX
          displ8 = 1;
#endif
          remain--;
          goto modrm_done;
        }
        else {
          return(-1);
        }
      }
      // (mod == 0x80)      mod == 10b
      if (remain > 1) {
        i->modRMForm.displ16u = FetchWORD(iptr);
        iptr += 2;
        remain -= 2;
      }
      else {
        return(-1);
      }
    }

modrm_done:

#if BX_SUPPORT_EVEX
    // EVEX.b in reg form implies 512-bit vector length
    if (i->modC0() && i->getEvexb()) {
      i->setVL(BX_VL512);
    }
#endif

    ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, b2, sse_prefix, os_32, i->getVL(), vex_w);
  }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.

    unsigned group = attr & BxGroupX;
    if (group == BxPrefixSSE && sse_prefix) {
      OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix-1]);
    }

    ia_opcode = OpcodeInfoPtr->IA;
    rm = b1 & 0x7;
    nnn = (b1 >> 3) & 0x7;
    i->assertModC0();
  }

  if (lock) { // lock prefix invalid opcode
    // lock prefix not allowed or destination operand is not memory
    if (!mod_mem || !(attr & BxLockable)) {
#if BX_CPU_LEVEL >= 6
      if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_ALT_MOV_CR8) && 
         (ia_opcode == BX_IA_MOV_CR0Rd || ia_opcode == BX_IA_MOV_RdCR0)) {
        nnn = 8; // extend CR0 -> CR8
      }
      else
#endif
      {
        BX_INFO(("LOCK prefix unallowed (op1=0x%x, modrm=0x%02x)", b1, b2));
        // replace execution function with undefined-opcode
        ia_opcode = BX_IA_ERROR;
      }
    }
  }

  imm_mode = attr & BxImmediate;
  if (imm_mode) {
    // make sure iptr was advanced after Ib(), Iw() and Id()
    switch (imm_mode) {
      case BxImmediate_I1:
        i->modRMForm.Ib[0] = 1;
        break;
      case BxImmediate_Ib:
        if (remain != 0) {
          i->modRMForm.Ib[0] = *iptr++;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_BrOff8:
      case BxImmediate_Ib_SE: // Sign extend to OS size
        if (remain != 0) {
          Bit8s temp8s = *iptr;
          // this code works correctly both for LE and BE hosts
          if (i->os32L())
            i->modRMForm.Id    = (Bit32s) temp8s;
          else
            i->modRMForm.Iw[0] = (Bit16s) temp8s;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_Iw:
        if (remain > 1) {
          i->modRMForm.Iw[0] = FetchWORD(iptr);
          iptr += 2;
          remain -= 2;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_Id:
        if (remain > 3) {
          i->modRMForm.Id = FetchDWORD(iptr);
          iptr += 4;
          remain -= 4;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_O:
        // For instructions which embed the address in the opcode.
        if (i->as32L()) {
          // fetch 32bit address into Id
          if (remain > 3) {
            i->modRMForm.Id = FetchDWORD(iptr);
            remain -= 4;
          }
          else return(-1);
        }
        else {
          // fetch 16bit address into Id
          if (remain > 1) {
            i->modRMForm.Id = (Bit32u) FetchWORD(iptr);
            remain -= 2;
          }
          else return(-1);
        }
        break;
      default:
        BX_INFO(("b1 was %x", b1));
        BX_PANIC(("fetchdecode32: imm_mode = %u", imm_mode));
    }

#if BX_SUPPORT_AVX
    if (! had_vex_xop)
#endif
    {
      unsigned imm_mode2 = attr & BxImmediate2;
      if (imm_mode2) {
        switch (imm_mode2) {
          case BxImmediate_Ib2:
            if (remain != 0) {
              i->modRMForm.Ib2[0] = *iptr;
              remain--;
            }
            else {
              return(-1);
            }
            break;
          case BxImmediate_Iw2:
            if (remain > 1) {
              i->modRMForm.Iw2[0] = FetchWORD(iptr);
              remain -= 2;
            }
            else {
              return(-1);
            }
            break;
          default:
            BX_INFO(("b1 was %x", b1));
            BX_PANIC(("fetchdecode: imm_mode2 = %u", imm_mode2));
        }
      }
    }
  }

#if BX_SUPPORT_3DNOW
  if(b1 == 0x10f)
    ia_opcode = Bx3DNowOpcode[i->modRMForm.Ib[0]];
#endif

  // assign sources
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = src >> 3;
    switch(src & 0x7) {
    case BX_SRC_NONE:
      break;
    case BX_SRC_EAX:
      i->setSrcReg(n, 0);
      break;
    case BX_SRC_NNN:
      i->setSrcReg(n, nnn);
      break;
    case BX_SRC_RM:
      if (! mod_mem) {
        i->setSrcReg(n, rm);
      }
      else {
        unsigned tmpreg = BX_TMP_REGISTER;
#if BX_SUPPORT_FPU
        if (type == BX_VMM_REG) tmpreg = BX_VECTOR_TMP_REGISTER;
#endif
        i->setSrcReg(n, tmpreg);
#if BX_SUPPORT_EVEX
        if (b1 == 0x62 && displ8) {
          int displ_mult = 1;
          if (type == BX_GPR16) displ_mult = 2;
          else if (type == BX_GPR32) displ_mult = 4;

          if (i->as32L())
            i->modRMForm.displ32u *= displ_mult;
          else
            i->modRMForm.displ16u *= displ_mult;
        }
#endif
      }
      break;
#if BX_SUPPORT_EVEX
    case BX_SRC_EVEX_RM:
      if (! mod_mem) {
        i->setSrcReg(n, rm);
      }
      else {
        i->setSrcReg(n, BX_VECTOR_TMP_REGISTER);
        if (displ8) {
          if (i->as32L())
            i->modRMForm.displ32u *= evex_displ8_compression(i, ia_opcode, type, vex_w);
          else
            i->modRMForm.displ16u *= evex_displ8_compression(i, ia_opcode, type, vex_w);
        }
        if (n == 0 && i->isZeroMasking()) // zero masking is not allowed for memory destination
          ia_opcode = BX_IA_ERROR;
      }
      break;
#endif
#if BX_SUPPORT_AVX
    case BX_SRC_VVV:
      i->setSrcReg(n, vvv);
      use_vvv = 1;
      break;
    case BX_SRC_VIB:
      i->setSrcReg(n, (i->Ib() >> 4) & 7);
      break;
    case BX_SRC_VSIB:
      if (! i->as32L() || i->sibIndex() == BX_NIL_REGISTER) {
        ia_opcode = BX_IA_ERROR;
      }
#if BX_SUPPORT_EVEX
      if (displ8) {
        if (i->as32L())
          i->modRMForm.displ32u *= 4 << vex_w;
        else
          i->modRMForm.displ16u *= 4 << vex_w;
      }
      // zero masking is not allowed for gather/scatter
      if (i->isZeroMasking()) {
        ia_opcode = BX_IA_ERROR;
      }
      break;
#endif
#endif
    default:
      BX_PANIC(("fetchdecode32: unknown definition %d for src %d", src, n));
    }
  }

  // assign memory segment override
  if (! BX_NULL_SEG_REG(seg_override))
     seg = seg_override;
  i->setSeg(seg);

#if BX_SUPPORT_AVX
  if (had_vex_xop) {
    if (! use_vvv && vvv != 0) {
      ia_opcode = BX_IA_ERROR;
    }
    else if ((attr & BxVexW0) != 0 && vex_w) {
      ia_opcode = BX_IA_ERROR;
    }
    else if ((attr & BxVexW1) != 0 && !vex_w) {
      ia_opcode = BX_IA_ERROR;
    }
#if BX_SUPPORT_EVEX
    // EVEX specific #UD conditions
    else if (i->getVL() > BX_VL512) {
      ia_opcode = BX_IA_ERROR;
    }
#endif
    else if ((attr & BxVexL0) != 0 && i->getVL() != BX_VL128) {
      ia_opcode = BX_IA_ERROR;
    }
    else if ((attr & BxVexL1) != 0 && i->getVL() == BX_VL128) {
      ia_opcode = BX_IA_ERROR;
    }
  }
  else {
    BX_ASSERT(! use_vvv);
  }
#endif

decode_done:

  i->setILen(remainingInPage - remain);
  i->setIaOpcode(ia_opcode);
  if (lock) i->setLockRepUsed(1);

  if (mod_mem) {
    i->execute1 = BxOpcodesTable[ia_opcode].execute1;
    i->handlers.execute2 = BxOpcodesTable[ia_opcode].execute2;

    if (ia_opcode == BX_IA_MOV_Op32_GdEd) {
      if (seg == BX_SEG_REG_SS)
        i->execute1 = &BX_CPU_C::MOV32S_GdEdM;
    }
    if (ia_opcode == BX_IA_MOV_Op32_EdGd) {
      if (seg == BX_SEG_REG_SS)
        i->execute1 = &BX_CPU_C::MOV32S_EdGdM;
    }
  }
  else {
    i->execute1 = BxOpcodesTable[ia_opcode].execute2;
    i->handlers.execute2 = NULL;
  }

  BX_ASSERT(i->execute1);

  Bit32u op_flags = BxOpcodesTable[ia_opcode].opflags;
#if BX_SUPPORT_EVEX
  if ((op_flags & BX_PREPARE_EVEX) != 0 && i->getEvexb()) {
    if (mod_mem) {
      if ((op_flags & BX_PREPARE_EVEX_NO_BROADCAST) == BX_PREPARE_EVEX_NO_BROADCAST) {
        BX_DEBUG(("%s: broadcast is not supported for this instruction", i->getIaOpcodeNameShort()));
        i->execute1 = &BX_CPU_C::BxError;
      }
    }
    else {
      if ((op_flags & BX_PREPARE_EVEX_NO_SAE) == BX_PREPARE_EVEX_NO_SAE) {
        BX_DEBUG(("%s: EVEX.b in reg form is not allowed for instructions which cannot cause floating point exception", i->getIaOpcodeNameShort()));
        i->execute1 = &BX_CPU_C::BxError;
      }
    }
  }
#endif
#if BX_CPU_LEVEL >= 6
  if (! (fetchModeMask & BX_FETCH_MODE_SSE_OK)) {
     if (op_flags & BX_PREPARE_SSE) {
        if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoSSE;
        return(1);
     }
  }
#if BX_SUPPORT_AVX
  if (! (fetchModeMask & BX_FETCH_MODE_AVX_OK)) {
    if (op_flags & BX_PREPARE_AVX) {
       if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoAVX;
       return(1);
    }
  }
#if BX_SUPPORT_EVEX
  if (! (fetchModeMask & BX_FETCH_MODE_OPMASK_OK)) {
    if (op_flags & BX_PREPARE_OPMASK) {
       if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoOpMask;
       return(1);
    }
  }
  if (! (fetchModeMask & BX_FETCH_MODE_EVEX_OK)) {
    if (op_flags & BX_PREPARE_EVEX) {
       if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoEVEX;
       return(1);
    }
  }
#endif
#endif
#endif

  if ((op_flags & BX_TRACE_END) != 0 || i->execute1 == &BX_CPU_C::BxError)
     return(1);

  return(0);
}

#if BX_SUPPORT_EVEX
unsigned BX_CPU_C::evex_displ8_compression(bxInstruction_c *i, unsigned ia_opcode, unsigned type, unsigned vex_w)
{
  if (ia_opcode == BX_IA_V512_VMOVDDUP_VpdWpd && i->getVL() == BX_VL128)
    return 8;

  unsigned len = i->getVL();

  switch (type) {
  case BX_VMM_FULL_VECTOR:
    if (i->getEvexb()) { // broadcast
       return (4 << vex_w);
    }
    else {
       return (16 * len);
    }

  case BX_VMM_SCALAR_BYTE:
    return 1;

  case BX_VMM_SCALAR_WORD:
    return 2;

  case BX_VMM_SCALAR:
    return (4 << vex_w);

  case BX_VMM_HALF_VECTOR:
    if (i->getEvexb()) { // broadcast
       return (4 << vex_w);
    }
    else {
       return (8 * len);
    }

  case BX_VMM_QUARTER_VECTOR:
    BX_ASSERT(! i->getEvexb());
    return (4 * len);

  case BX_VMM_OCT_VECTOR:
    BX_ASSERT(! i->getEvexb());
    return (2 * len);

  case BX_VMM_VEC128:
    return 16;

  case BX_VMM_VEC256:
    return 32;
  }

  return 1;
}
#endif

Bit16u BX_CPU_C::WalkOpcodeTables(const BxOpcodeInfo_t *OpcodeInfoPtr, Bit16u &attr, unsigned modrm, unsigned sse_prefix, unsigned osize, unsigned vex_vl, bx_bool vex_w)
{
  // Parse mod-nnn-rm and related bytes
  unsigned mod_mem = (modrm & 0xc0) != 0xc0;
  unsigned nnn = (modrm >> 3) & 0x7;
//rm  = modrm & 0x7;

  attr |= OpcodeInfoPtr->Attr;

  while(attr & BxGroupX) {
    Bit32u group = attr & BxGroupX;
    attr &= ~BxGroupX;

    // ignore 0x66 SSE prefix is required
    if (group == BxPrefixSSEF2F3) {
      if (sse_prefix == SSE_PREFIX_66) sse_prefix = SSE_PREFIX_NONE;
      group = BxPrefixSSE;
    }

    if (group < BxPrefixSSE) {
      /* For opcodes with only one allowed SSE prefix */
      if (sse_prefix != (group >> 4)) {
        attr = 0;
        return BX_IA_ERROR;
      }
      break;
    }

    switch(group) {
      case BxGroupN:
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn]);
        break;
      case BxSplitGroupN:
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn + (mod_mem << 3)]);
        break;
#if BX_SUPPORT_AVX
      case BxSplitMod11B:
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[mod_mem]);
        break;
#endif
#if BX_SUPPORT_EVEX
      case BxSplitVexVL:
        if (vex_vl > BX_VLMAX) {
          attr = 0;
          return BX_IA_ERROR;
        }
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[vex_vl]);
        break;
#endif
      case BxOSizeGrp:
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[osize]);
        break;
      case BxPrefixSSE:
        /* For SSE opcodes look into another 3-entry table
                   with the opcode prefixes (NONE, 0x66, 0xF3, 0xF2) */
        if (sse_prefix) {
          OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix-1]);
          break;
        }
        continue;
      case BxPrefixSSE4:
        /* For SSE opcodes look into another 4-entry table
           with the opcode prefixes (NONE, 0x66, 0xF3, 0xF2) */
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix]);
        break;
      case BxPrefixSSE2:
        /* For SSE opcodes look into another 2-entry table
           with the opcode prefixes (NONE, 0x66), 0xF2 and 0xF3 not allowed */
        if (sse_prefix > SSE_PREFIX_66) {
          attr = 0;
          return BX_IA_ERROR;
        }
        OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix]);
        break;
      case BxFPEscape:
        if (mod_mem)
          OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn]);
        else
          OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[(modrm & 0x3f) + 8]);
        break;
      default:
        BX_PANIC(("fetchdecode64: Unknown opcode group %d", group));
    }

    /* get additional attributes from group table */
    attr |= OpcodeInfoPtr->Attr;
  }

  Bit16u ia_opcode = OpcodeInfoPtr->IA;

  if (ia_opcode != BX_IA_ERROR) {
    unsigned has_alias = attr & BxAlias;
    if (has_alias) {
      unsigned alias = 0;
      if (has_alias == BxAliasSSE) {
        alias = sse_prefix;
      }
#if BX_SUPPORT_AVX
      else {
        // VexW64 is ignored in 32-bit mode
        if (has_alias == BxAliasVexW || long64_mode()) {
          alias = vex_w;
        }
      }
#endif
      ia_opcode += alias;
    }
  }

  return (ia_opcode);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BxError(bxInstruction_c *i)
{
  unsigned ia_opcode = i->getIaOpcode();
 
  if (ia_opcode == BX_IA_ERROR) {
    BX_DEBUG(("BxError: Encountered an unknown instruction (signalling #UD)"));

#if BX_DISASM && BX_DEBUGGER == 0 // with debugger it easy to see the #UD
    if (LOG_THIS getonoff(LOGLEV_DEBUG))
      debug_disasm_instruction(BX_CPU_THIS_PTR prev_rip);
#endif
  }
  else {
    BX_DEBUG(("%s: instruction not supported - signalling #UD", get_bx_opcode_name(ia_opcode)));
    for (unsigned n=0; n<BX_ISA_EXTENSIONS_ARRAY_SIZE; n++)
      BX_DEBUG(("ia_extensions_bitmask[%d]: %08x", n, BX_CPU_THIS_PTR ia_extensions_bitmask[n]));
  }

  exception(BX_UD_EXCEPTION, 0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

const char *get_bx_opcode_name(Bit16u ia_opcode)
{
  static const char* BxOpcodeNamesTable[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) #a,
#include "ia_opcodes.h"
  };
#undef  bx_define_opcode

  return (ia_opcode < BX_IA_LAST) ? BxOpcodeNamesTable[ia_opcode] : 0;
}

void BX_CPU_C::init_FetchDecodeTables(void)
{
  static Bit8u BxOpcodeFeatures[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) d,
#include "ia_opcodes.h"
  };
#undef  bx_define_opcode

#if BX_CPU_LEVEL > 3
  if (! BX_CPU_THIS_PTR ia_extensions_bitmask[0])
    BX_PANIC(("init_FetchDecodeTables: CPU features bitmask is empty !"));
#endif

  if (BX_IA_LAST > 0xfff)
    BX_PANIC(("init_FetchDecodeTables: too many opcodes defined !"));

  for (unsigned n=0; n < BX_IA_LAST; n++) {

    switch(n) {
      // special case: these opcodes also supported if 3DNOW! Extensions are supported
      case BX_IA_PSHUFW_PqQqIb:
      case BX_IA_PINSRW_PqEwIb:
      case BX_IA_PEXTRW_GdNqIb:
      case BX_IA_PMOVMSKB_GdNq:
      case BX_IA_PMINUB_PqQq:
      case BX_IA_PMAXUB_PqQq:
      case BX_IA_PMULHUW_PqQq:
      case BX_IA_MOVNTQ_MqPq:
      case BX_IA_PMINSW_PqQq:
      case BX_IA_PSADBW_PqQq:
      case BX_IA_MASKMOVQ_PqNq:
        if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_3DNOW)) continue;

      default: break;
    }

    unsigned ia_opcode_feature = BxOpcodeFeatures[n];
    if (! BX_CPUID_SUPPORT_ISA_EXTENSION(ia_opcode_feature)) {
      BxOpcodesTable[n].execute1 = &BX_CPU_C::BxError;
      BxOpcodesTable[n].execute2 = &BX_CPU_C::BxError;
      // won't allow this new #UD opcode to check prepare_SSE and similar
      BxOpcodesTable[n].src[3] = 0;
    }
  }

  // handle special case - BSF/BSR vs TZCNT/LZCNT
  if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_LZCNT)) {
    BxOpcodesTable[BX_IA_LZCNT_GwEw] = BxOpcodesTable[BX_IA_BSR_GwEw];
    BxOpcodesTable[BX_IA_LZCNT_GdEd] = BxOpcodesTable[BX_IA_BSR_GdEd];
#if BX_SUPPORT_X86_64
    BxOpcodesTable[BX_IA_LZCNT_GqEq] = BxOpcodesTable[BX_IA_BSR_GqEq];
#endif
  }

  if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_BMI1)) {
    BxOpcodesTable[BX_IA_TZCNT_GwEw] = BxOpcodesTable[BX_IA_BSF_GwEw];
    BxOpcodesTable[BX_IA_TZCNT_GdEd] = BxOpcodesTable[BX_IA_BSF_GdEd];
#if BX_SUPPORT_X86_64
    BxOpcodesTable[BX_IA_TZCNT_GqEq] = BxOpcodesTable[BX_IA_BSF_GqEq];
#endif
  }
}
