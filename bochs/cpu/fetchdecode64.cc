/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode64.cc,v 1.141 2007-11-18 21:07:40 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

///////////////////////////
// prefix bytes
// opcode bytes
// modrm/sib
// address displacement
// immediate constant
///////////////////////////

// The table for 64-bit is slightly different from the
// table for 32-bit due to undefined opcodes, which
// were valid in 32-bit mode
 
static const bx_bool BxOpcodeHasModrm64[512] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f          */
  /*       -------------------------------          */
  /* 00 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 10 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 20 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 30 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 60 */ 0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0, 
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 80 */ 1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1, 
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* A0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* B0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* C0 */ 1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0, 
  /* D0 */ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, 
  /* E0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* F0 */ 0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
  /*       -------------------------------           */
           1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1, /* 0F 00 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 10 */
           1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, /* 0F 20 */
           0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0, /* 0F 30 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 40 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 50 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 60 */
           1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1, /* 0F 70 */
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0F 80 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 90 */
           0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 0F A0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F B0 */
           1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* 0F C0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F D0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F E0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0  /* 0F F0 */
  /*       -------------------------------           */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
};

/* *********** */
// LOCK PREFIX //
/* *********** */

/* 
 *  The  LOCK  prefix  can be prepended only to the following instructions
 *  and  only  to  those  forms  of the instructions where the destination
 *  operand  is  a  memory operand: ADD, ADC, AND, BTC, BTR, BTS, CMPXCHG,
 *  CMPXCH8B,  DEC,  INC,  NEG, NOT, OR, SBB, SUB, XOR, XADD, and XCHG. If
 *  the  LOCK prefix is used with one of these instructions and the source
 *  operand  is a memory operand, an undefined opcode exception (#UD) will
 *  be  generated. An undefined opcode exception will also be generated if
 *  the  LOCK  prefix  is used with any instruction not in the above list.
 *  The XCHG instruction always asserts the LOCK# signal regardless of the
 *  presence or absence of the LOCK prefix.
 */

// Segment override prefixes
// -------------------------
// In 64-bit mode the CS, DS, ES, and SS segment overrides are ignored.


static BxExecutePtr_tR Resolve32Rm[8] = {
  &BX_CPU_C::BxResolve32Rm,
  &BX_CPU_C::BxResolve32Rm,
  &BX_CPU_C::BxResolve32Rm,
  &BX_CPU_C::BxResolve32Rm,
  NULL, // escape to SIB-byte
  &BX_CPU_C::BxResolve32Rip,
  &BX_CPU_C::BxResolve32Rm,
  &BX_CPU_C::BxResolve32Rm
};

static BxExecutePtr_tR Resolve32Base[8] = {
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Disp,
  &BX_CPU_C::BxResolve32Base,
  &BX_CPU_C::BxResolve32Base,
};

static BxExecutePtr_tR Resolve32BaseIndex[8] = {
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32DispIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
  &BX_CPU_C::BxResolve32BaseIndex,
};

static BxExecutePtr_tR Resolve64[16] = {
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  NULL, // escape to SIB-byte
  &BX_CPU_C::BxResolve64Rip,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm,
  NULL, // escape to SIB-byte
  &BX_CPU_C::BxResolve64Rip,
  &BX_CPU_C::BxResolve64Rm,
  &BX_CPU_C::BxResolve64Rm
};

static BxExecutePtr_tR Resolve64Base[16] = {
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Disp,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Disp,
  &BX_CPU_C::BxResolve64Base,
  &BX_CPU_C::BxResolve64Base,
};

static BxExecutePtr_tR Resolve64BaseIndex[16] = {
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64DispIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64DispIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
  &BX_CPU_C::BxResolve64BaseIndex,
};

// decoding instructions; accessing seg reg's by index
static unsigned sreg_mod01or10_rm32[16] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_NULL, // escape to SIB-byte
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_NULL, // escape to SIB-byte
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
};

static unsigned sreg_mod0_base32[16] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS
};

static unsigned sreg_mod1or2_base32[16] = {
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_SS,
  BX_SEG_REG_SS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS,
  BX_SEG_REG_DS
};

// common fetchdecode32/64 opcode tables
#include "fetchdecode.h"


// 512 entries for 16bit operand size
// 512 entries for 32bit operand size
// 512 entries for 64bit operand size

static const BxOpcodeInfo_t BxOpcodeInfo64R[512*3] = {
  // 512 entries for 16bit operand size
  /* 00 /wr */ { 0, &BX_CPU_C::ADD_EbGbR },
  /* 01 /wr */ { 0, &BX_CPU_C::ADD_EwGwR },
  /* 02 /wr */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /wr */ { 0, &BX_CPU_C::ADD_GwEwR },
  /* 04 /wr */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /wr */ { BxImmediate_Iv, &BX_CPU_C::ADD_AXIw },
  /* 06 /wr */ { 0, &BX_CPU_C::BxError },
  /* 07 /wr */ { 0, &BX_CPU_C::BxError },
  /* 08 /wr */ { 0, &BX_CPU_C::OR_EbGbR },
  /* 09 /wr */ { 0, &BX_CPU_C::OR_EwGwR },
  /* 0A /wr */ { 0, &BX_CPU_C::OR_GbEb },
  /* 0B /wr */ { 0, &BX_CPU_C::OR_GwEw },
  /* 0C /wr */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /wr */ { BxImmediate_Iv, &BX_CPU_C::OR_AXIw },
  /* 0E /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F /wr */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /wr */ { 0, &BX_CPU_C::ADC_EbGbR },
  /* 11 /wr */ { 0, &BX_CPU_C::ADC_EwGwR },
  /* 12 /wr */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /wr */ { 0, &BX_CPU_C::ADC_GwEw },
  /* 14 /wr */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /wr */ { BxImmediate_Iv, &BX_CPU_C::ADC_AXIw },
  /* 16 /wr */ { 0, &BX_CPU_C::BxError },
  /* 17 /wr */ { 0, &BX_CPU_C::BxError },
  /* 18 /wr */ { 0, &BX_CPU_C::SBB_EbGbR },
  /* 19 /wr */ { 0, &BX_CPU_C::SBB_EwGwR },
  /* 1A /wr */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /wr */ { 0, &BX_CPU_C::SBB_GwEw },
  /* 1C /wr */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /wr */ { BxImmediate_Iv, &BX_CPU_C::SBB_AXIw },
  /* 1E /wr */ { 0, &BX_CPU_C::BxError },
  /* 1F /wr */ { 0, &BX_CPU_C::BxError },
  /* 20 /wr */ { 0, &BX_CPU_C::AND_EbGbR },
  /* 21 /wr */ { 0, &BX_CPU_C::AND_EwGwR },
  /* 22 /wr */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /wr */ { 0, &BX_CPU_C::AND_GwEw },
  /* 24 /wr */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /wr */ { BxImmediate_Iv, &BX_CPU_C::AND_AXIw },
  /* 26 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /wr */ { 0, &BX_CPU_C::BxError },
  /* 28 /wr */ { 0, &BX_CPU_C::SUB_EbGbR },
  /* 29 /wr */ { 0, &BX_CPU_C::SUB_EwGwR },
  /* 2A /wr */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /wr */ { 0, &BX_CPU_C::SUB_GwEw },
  /* 2C /wr */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /wr */ { BxImmediate_Iv, &BX_CPU_C::SUB_AXIw },
  /* 2E /wr */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /wr */ { 0, &BX_CPU_C::BxError },
  /* 30 /wr */ { 0, &BX_CPU_C::XOR_EbGbR },
  /* 31 /wr */ { 0, &BX_CPU_C::XOR_EwGwR },
  /* 32 /wr */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /wr */ { 0, &BX_CPU_C::XOR_GwEw },
  /* 34 /wr */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /wr */ { BxImmediate_Iv, &BX_CPU_C::XOR_AXIw },
  /* 36 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /wr */ { 0, &BX_CPU_C::BxError },
  /* 38 /wr */ { 0, &BX_CPU_C::CMP_EbGbR },
  /* 39 /wr */ { 0, &BX_CPU_C::CMP_EwGwR },
  /* 3A /wr */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /wr */ { 0, &BX_CPU_C::CMP_GwEw },
  /* 3C /wr */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /wr */ { BxImmediate_Iv, &BX_CPU_C::CMP_AXIw },
  /* 3E /wr */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /wr */ { 0, &BX_CPU_C::BxError },
  /* 40 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 51 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 52 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 53 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 54 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 55 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 56 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 57 /wr */ { 0, &BX_CPU_C::PUSH_RX },
  /* 58 /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 59 /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5A /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5B /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5C /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5D /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5E /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 5F /wr */ { 0, &BX_CPU_C::POP_RX },
  /* 60 /wr */ { 0, &BX_CPU_C::BxError },
  /* 61 /wr */ { 0, &BX_CPU_C::BxError },
  /* 62 /wr */ { 0, &BX_CPU_C::BxError },
  /* 63 /wr */ { 0, &BX_CPU_C::MOV_GwEwR }, // MOVSX_GwEw
  /* 64 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /wr */ { BxImmediate_Iv, &BX_CPU_C::PUSH_Iw },
  /* 69 /wr */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GwEwIw },
  /* 6A /wr */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH_Iw },
  /* 6B /wr */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GwEwIw },
  /* 6C /wr */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /wr */ { 0, &BX_CPU_C::REP_INSW_YwDX },
  /* 6E /wr */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /wr */ { 0, &BX_CPU_C::REP_OUTSW_DXXw },
  /* 70 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /wr */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbR },
  /* 81 /wr */ { BxGroup1 | BxImmediate_Iv,    NULL, BxOpcodeInfoG1EwR },
  /* 82 /wr */ { 0, &BX_CPU_C::BxError },
  /* 83 /wr */ { BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1EwR },
  /* 84 /wr */ { 0, &BX_CPU_C::TEST_EbGbR },
  /* 85 /wr */ { 0, &BX_CPU_C::TEST_EwGwR },
  /* 86 /wr */ { 0, &BX_CPU_C::XCHG_EbGbR },
  /* 87 /wr */ { 0, &BX_CPU_C::XCHG_EwGw },
  /* 88 /wr */ { 0, &BX_CPU_C::MOV_EbGbR },
  /* 89 /wr */ { 0, &BX_CPU_C::MOV_EwGwR },
  /* 8A /wr */ { 0, &BX_CPU_C::MOV_GbEbR },
  /* 8B /wr */ { 0, &BX_CPU_C::MOV_GwEwR },
  /* 8C /wr */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /wr */ { 0, &BX_CPU_C::BxError },   // LEA
  /* 8E /wr */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /wr */ { 0, &BX_CPU_C::POP_EwR },
  /* 90 /wr */ { 0, &BX_CPU_C::XCHG_RXAX }, // handles XCHG R8w, AX
  /* 91 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 92 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 93 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 94 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 95 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 96 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 97 /wr */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 98 /wr */ { 0, &BX_CPU_C::CBW },
  /* 99 /wr */ { 0, &BX_CPU_C::CWD },
  /* 9A /wr */ { 0, &BX_CPU_C::BxError },
  /* 9B /wr */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /wr */ { 0, &BX_CPU_C::PUSHF_Fw },
  /* 9D /wr */ { 0, &BX_CPU_C::POPF_Fw },
  /* 9E /wr */ { 0, &BX_CPU_C::SAHF },
  /* 9F /wr */ { 0, &BX_CPU_C::LAHF },
  /* A0 /wr */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /wr */ { BxImmediate_O, &BX_CPU_C::MOV_AXOq },
  /* A2 /wr */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /wr */ { BxImmediate_O, &BX_CPU_C::MOV_OqAX },
  /* A4 /wr */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /wr */ { 0, &BX_CPU_C::REP_MOVSW_XwYw },
  /* A6 /wr */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /wr */ { 0, &BX_CPU_C::REP_CMPSW_XwYw },
  /* A8 /wr */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /wr */ { BxImmediate_Iv, &BX_CPU_C::TEST_AXIw },
  /* AA /wr */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /wr */ { 0, &BX_CPU_C::REP_STOSW_YwAX },
  /* AC /wr */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /wr */ { 0, &BX_CPU_C::REP_LODSW_AXXw },
  /* AE /wr */ { 0, &BX_CPU_C::REP_SCASB_ALXb },
  /* AF /wr */ { 0, &BX_CPU_C::REP_SCASW_AXXw },
  /* B0 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* B9 /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BA /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BB /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BC /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BD /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BE /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BF /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* C0 /wr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /wr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ew },
  /* C2 /wr */ { BxImmediate_Iw, &BX_CPU_C::RETnear16_Iw },
  /* C3 /wr */ { 0,              &BX_CPU_C::RETnear16 },
  /* C4 /wr */ { 0, &BX_CPU_C::BxError },
  /* C5 /wr */ { 0, &BX_CPU_C::BxError },
  /* C6 /wr */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbR },
  /* C7 /wr */ { BxImmediate_Iv, &BX_CPU_C::MOV_EwIwR },
  /* C8 /wr */ { BxImmediate_IwIb, &BX_CPU_C::ENTER_IwIb },
  /* C9 /wr */ { 0, &BX_CPU_C::LEAVE },
  /* CA /wr */ { BxImmediate_Iw, &BX_CPU_C::RETfar16_Iw },
  /* CB /wr */ { 0, &BX_CPU_C::RETfar16 },
  /* CC /wr */ { 0, &BX_CPU_C::INT3 },
  /* CD /wr */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /wr */ { 0, &BX_CPU_C::BxError },
  /* CF /wr */ { 0, &BX_CPU_C::IRET16 },
  /* D0 /wr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /wr */ { BxGroup2, NULL, BxOpcodeInfoG2Ew },
  /* D2 /wr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /wr */ { BxGroup2, NULL, BxOpcodeInfoG2Ew },
  /* D4 /wr */ { 0, &BX_CPU_C::BxError },
  /* D5 /wr */ { 0, &BX_CPU_C::BxError },
  /* D6 /wr */ { 0, &BX_CPU_C::BxError },
  /* D7 /wr */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /wr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /wr */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /wr */ { BxImmediate_Ib, &BX_CPU_C::IN_AXIb },
  /* E6 /wr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /wr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAX },
  /* E8 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /wr */ { 0, &BX_CPU_C::BxError },
  /* EB /wr */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /wr */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /wr */ { 0, &BX_CPU_C::IN_AXDX },
  /* EE /wr */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /wr */ { 0, &BX_CPU_C::OUT_DXAX },
  /* F0 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK
  /* F1 /wr */ { 0, &BX_CPU_C::INT1 },
  /* F2 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /wr */ { BxPrefix, &BX_CPU_C::BxError }, // REP, REPE/REPZ
  /* F4 /wr */ { 0, &BX_CPU_C::HLT },
  /* F5 /wr */ { 0, &BX_CPU_C::CMC },
  /* F6 /wr */ { BxGroup3, NULL, BxOpcodeInfoG3EbR },
  /* F7 /wr */ { BxGroup3, NULL, BxOpcodeInfoG3EwR },
  /* F8 /wr */ { 0, &BX_CPU_C::CLC },
  /* F9 /wr */ { 0, &BX_CPU_C::STC },
  /* FA /wr */ { 0, &BX_CPU_C::CLI },
  /* FB /wr */ { 0, &BX_CPU_C::STI },
  /* FC /wr */ { 0, &BX_CPU_C::CLD },
  /* FD /wr */ { 0, &BX_CPU_C::STD },
  /* FE /wr */ { BxGroup4, NULL, BxOpcodeInfoG4R },
  /* FF /wr */ { BxGroup5, NULL, BxOpcodeInfo64G5wR },

  /* 0F 00 /wr */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /wr */ { BxGroup7, NULL, BxOpcodeInfo64G7R },
  /* 0F 02 /wr */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /wr */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /wr */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /wr */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /wr */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /wr */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /wr */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /wr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /wr */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /wr */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /wr */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /wr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /wr */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /wr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /wr */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /wr */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /wr */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /wr */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /wr */ { 0, &BX_CPU_C::BxError }, // MOVNTPS/PD/SS/SD
  /* 0F 2C /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /wr */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /wr */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /wr */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /wr */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /wr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /wr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /wr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /wr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /wr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /wr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /wr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /wr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 41 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 42 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 43 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 44 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 45 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 46 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 47 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 48 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 49 /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4A /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4B /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4C /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4D /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4E /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4F /wr */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 50 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /wr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /wr */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /wr */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /wr */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /wr */ { 0, &BX_CPU_C::EMMS }, 
  /* 0F 78 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /wr */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /wr */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /wr */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /wr */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /wr */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /wr */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /wr */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /wr */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /wr */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /wr */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /wr */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /wr */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /wr */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /wr */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /wr */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /wr */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /wr */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /wr */ { 0, &BX_CPU_C::PUSH16_FS },
  /* 0F A1 /wr */ { 0, &BX_CPU_C::POP16_FS },
  /* 0F A2 /wr */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /wr */ { 0, &BX_CPU_C::BT_EwGwR },
  /* 0F A4 /wr */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EwGw },
  /* 0F A5 /wr */ { 0,              &BX_CPU_C::SHLD_EwGw },
  /* 0F A6 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /wr */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /wr */ { 0, &BX_CPU_C::PUSH16_GS },
  /* 0F A9 /wr */ { 0, &BX_CPU_C::POP16_GS },
  /* 0F AA /wr */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /wr */ { 0, &BX_CPU_C::BTS_EwGwR },
  /* 0F AC /wr */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EwGw },
  /* 0F AD /wr */ { 0,              &BX_CPU_C::SHRD_EwGw },
  /* 0F AE /wr */ { BxGroup15, NULL, BxOpcodeInfoG15R },
  /* 0F AF /wr */ { 0, &BX_CPU_C::IMUL_GwEw },
  /* 0F B0 /wr */ { 0, &BX_CPU_C::CMPXCHG_EbGbR },
  /* 0F B1 /wr */ { 0, &BX_CPU_C::CMPXCHG_EwGwR },
  /* 0F B2 /wr */ { 0, &BX_CPU_C::BxError }, // LSS
  /* 0F B3 /wr */ { 0, &BX_CPU_C::BTR_EwGwR },
  /* 0F B4 /wr */ { 0, &BX_CPU_C::BxError }, // LFS
  /* 0F B5 /wr */ { 0, &BX_CPU_C::BxError }, // LGS
  /* 0F B6 /wr */ { 0, &BX_CPU_C::MOVZX_GwEbR },
  /* 0F B7 /wr */ { 0, &BX_CPU_C::MOV_GwEwR }, // MOVZX_GwEw
  /* 0F B8 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8w },
  /* 0F B9 /wr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /wr */ { BxGroup8, NULL, BxOpcodeInfoG8EwIb },
  /* 0F BB /wr */ { 0, &BX_CPU_C::BTC_EwGwR },
  /* 0F BC /wr */ { 0, &BX_CPU_C::BSF_GwEw },
  /* 0F BD /wr */ { 0, &BX_CPU_C::BSR_GwEw },
  /* 0F BE /wr */ { 0, &BX_CPU_C::MOVSX_GwEbR },
  /* 0F BF /wr */ { 0, &BX_CPU_C::MOV_GwEwR }, // MOVSX_GwEw
  /* 0F C0 /wr */ { 0, &BX_CPU_C::XADD_EbGbR },
  /* 0F C1 /wr */ { 0, &BX_CPU_C::XADD_EwGwR },
  /* 0F C2 /wr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /wr */ { 0, &BX_CPU_C::BxError }, // MOVNTI SSE group
  /* 0F C4 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /wr */ { 0, &BX_CPU_C::BxError }, // CMPXCHG8B G9 group
  /* 0F C8 /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F C9 /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CA /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CB /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CC /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CD /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CE /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CF /wr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F D0 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /wr */ { 0, &BX_CPU_C::BxError }, // MOVNTQ/MOVNTDQ SSE group
  /* 0F E8 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /wr */ { 0, &BX_CPU_C::BxError }, // LDDQU SSE group
  /* 0F F1 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /wr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /wr */ { 0, &BX_CPU_C::BxError },

  // 512 entries for 32bit operand size
  /* 00 /dr */ { 0, &BX_CPU_C::ADD_EbGbR },
  /* 01 /dr */ { 0, &BX_CPU_C::ADD_EdGdR },
  /* 02 /dr */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /dr */ { 0, &BX_CPU_C::ADD_GdEdR },
  /* 04 /dr */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /dr */ { BxImmediate_Iv, &BX_CPU_C::ADD_EAXId },
  /* 06 /dr */ { 0, &BX_CPU_C::BxError },
  /* 07 /dr */ { 0, &BX_CPU_C::BxError },
  /* 08 /dr */ { 0, &BX_CPU_C::OR_EbGbR },
  /* 09 /dr */ { 0, &BX_CPU_C::OR_EdGdR },
  /* 0A /dr */ { 0, &BX_CPU_C::OR_GbEb },
  /* 0B /dr */ { 0, &BX_CPU_C::OR_GdEd },
  /* 0C /dr */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /dr */ { BxImmediate_Iv, &BX_CPU_C::OR_EAXId },
  /* 0E /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F /dr */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /dr */ { 0, &BX_CPU_C::ADC_EbGbR },
  /* 11 /dr */ { 0, &BX_CPU_C::ADC_EdGdR },
  /* 12 /dr */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /dr */ { 0, &BX_CPU_C::ADC_GdEd },
  /* 14 /dr */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /dr */ { BxImmediate_Iv, &BX_CPU_C::ADC_EAXId },
  /* 16 /dr */ { 0, &BX_CPU_C::BxError },
  /* 17 /dr */ { 0, &BX_CPU_C::BxError },
  /* 18 /dr */ { 0, &BX_CPU_C::SBB_EbGbR },
  /* 19 /dr */ { 0, &BX_CPU_C::SBB_EdGdR },
  /* 1A /dr */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /dr */ { 0, &BX_CPU_C::SBB_GdEd },
  /* 1C /dr */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /dr */ { BxImmediate_Iv, &BX_CPU_C::SBB_EAXId },
  /* 1E /dr */ { 0, &BX_CPU_C::BxError },
  /* 1F /dr */ { 0, &BX_CPU_C::BxError },
  /* 20 /dr */ { 0, &BX_CPU_C::AND_EbGbR },
  /* 21 /dr */ { 0, &BX_CPU_C::AND_EdGdR },
  /* 22 /dr */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /dr */ { 0, &BX_CPU_C::AND_GdEd },
  /* 24 /dr */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /dr */ { BxImmediate_Iv, &BX_CPU_C::AND_EAXId },
  /* 26 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /dr */ { 0, &BX_CPU_C::BxError },
  /* 28 /dr */ { 0, &BX_CPU_C::SUB_EbGbR },
  /* 29 /dr */ { 0, &BX_CPU_C::SUB_EdGdR },
  /* 2A /dr */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /dr */ { 0, &BX_CPU_C::SUB_GdEd },
  /* 2C /dr */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /dr */ { BxImmediate_Iv, &BX_CPU_C::SUB_EAXId },
  /* 2E /dr */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /dr */ { 0, &BX_CPU_C::BxError },
  /* 30 /dr */ { 0, &BX_CPU_C::XOR_EbGbR },
  /* 31 /dr */ { 0, &BX_CPU_C::XOR_EdGdR },
  /* 32 /dr */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /dr */ { 0, &BX_CPU_C::XOR_GdEd },
  /* 34 /dr */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /dr */ { BxImmediate_Iv, &BX_CPU_C::XOR_EAXId },
  /* 36 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /dr */ { 0, &BX_CPU_C::BxError },
  /* 38 /dr */ { 0, &BX_CPU_C::CMP_EbGbR },
  /* 39 /dr */ { 0, &BX_CPU_C::CMP_EdGdR },
  /* 3A /dr */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /dr */ { 0, &BX_CPU_C::CMP_GdEd },
  /* 3C /dr */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /dr */ { BxImmediate_Iv, &BX_CPU_C::CMP_EAXId },
  /* 3E /dr */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /dr */ { 0, &BX_CPU_C::BxError },
  /* 40 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 /dr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 59 /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5A /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5B /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5C /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5D /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5E /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5F /dr */ { 0, &BX_CPU_C::POP_RRX },
  /* 60 /dr */ { 0, &BX_CPU_C::BxError },
  /* 61 /dr */ { 0, &BX_CPU_C::BxError },
  /* 62 /dr */ { 0, &BX_CPU_C::BxError },
  /* 63 /dr */ { 0, &BX_CPU_C::MOV_GdEdR }, // MOVSX_GdEd
  /* 64 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /dr */ { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 /dr */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GdEdId },
  /* 6A /dr */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B /dr */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GdEdId },
  /* 6C /dr */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /dr */ { 0, &BX_CPU_C::REP_INSD_YdDX },
  /* 6E /dr */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /dr */ { 0, &BX_CPU_C::REP_OUTSD_DXXd },
  /* 70 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /dr */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbR },
  /* 81 /dr */ { BxGroup5 | BxImmediate_Iv,    NULL, BxOpcodeInfoG1EdR },
  /* 82 /dr */ { 0, &BX_CPU_C::BxError },
  /* 83 /dr */ { BxGroup5 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1EdR },
  /* 84 /dr */ { 0, &BX_CPU_C::TEST_EbGbR },
  /* 85 /dr */ { 0, &BX_CPU_C::TEST_EdGdR },
  /* 86 /dr */ { 0, &BX_CPU_C::XCHG_EbGbR },
  /* 87 /dr */ { 0, &BX_CPU_C::XCHG_EdGdR },
  /* 88 /dr */ { 0, &BX_CPU_C::MOV_EbGbR },
  /* 89 /dr */ { 0, &BX_CPU_C::MOV_EdGdR },
  /* 8A /dr */ { 0, &BX_CPU_C::MOV_GbEbR },
  /* 8B /dr */ { 0, &BX_CPU_C::MOV_GdEdR },
  /* 8C /dr */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /dr */ { 0, &BX_CPU_C::BxError },     // LEA
  /* 8E /dr */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /dr */ { 0, &BX_CPU_C::POP_EqR },
  /* 90 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX }, // handles XCHG R8d, EAX
  /* 91 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 92 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 93 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 94 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 95 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 96 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 97 /dr */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 98 /dr */ { 0, &BX_CPU_C::CWDE },
  /* 99 /dr */ { 0, &BX_CPU_C::CDQ },
  /* 9A /dr */ { 0, &BX_CPU_C::BxError },
  /* 9B /dr */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /dr */ { 0, &BX_CPU_C::PUSHF_Fq },
  /* 9D /dr */ { 0, &BX_CPU_C::POPF_Fq },
  /* 9E /dr */ { 0, &BX_CPU_C::SAHF },
  /* 9F /dr */ { 0, &BX_CPU_C::LAHF },
  /* A0 /dr */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /dr */ { BxImmediate_O, &BX_CPU_C::MOV_EAXOq },
  /* A2 /dr */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /dr */ { BxImmediate_O, &BX_CPU_C::MOV_OqEAX },
  /* A4 /dr */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /dr */ { 0, &BX_CPU_C::REP_MOVSD_XdYd },
  /* A6 /dr */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /dr */ { 0, &BX_CPU_C::REP_CMPSD_XdYd },
  /* A8 /dr */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /dr */ { BxImmediate_Iv, &BX_CPU_C::TEST_EAXId },
  /* AA /dr */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /dr */ { 0, &BX_CPU_C::REP_STOSD_YdEAX },
  /* AC /dr */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /dr */ { 0, &BX_CPU_C::REP_LODSD_EAXXd },
  /* AE /dr */ { 0, &BX_CPU_C::REP_SCASB_ALXb  },
  /* AF /dr */ { 0, &BX_CPU_C::REP_SCASD_EAXXd },
  /* B0 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* B9 /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BA /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BB /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BC /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BD /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BE /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BF /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* C0 /dr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /dr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ed },
  /* C2 /dr */ { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 /dr */ { 0,              &BX_CPU_C::RETnear64 },
  /* C4 /dr */ { 0, &BX_CPU_C::BxError },
  /* C5 /dr */ { 0, &BX_CPU_C::BxError },
  /* C6 /dr */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbR },
  /* C7 /dr */ { BxImmediate_Iv, &BX_CPU_C::MOV_EdIdR },
  /* C8 /dr */ { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 /dr */ { 0, &BX_CPU_C::LEAVE64 },
  /* CA /dr */ { BxImmediate_Iw, &BX_CPU_C::RETfar32_Iw },
  /* CB /dr */ { 0, &BX_CPU_C::RETfar32 },
  /* CC /dr */ { 0, &BX_CPU_C::INT3 },
  /* CD /dr */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /dr */ { 0, &BX_CPU_C::BxError },
  /* CF /dr */ { 0, &BX_CPU_C::IRET32 },
  /* D0 /dr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /dr */ { BxGroup2, NULL, BxOpcodeInfoG2Ed },
  /* D2 /dr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /dr */ { BxGroup2, NULL, BxOpcodeInfoG2Ed },
  /* D4 /dr */ { 0, &BX_CPU_C::BxError },
  /* D5 /dr */ { 0, &BX_CPU_C::BxError },
  /* D6 /dr */ { 0, &BX_CPU_C::BxError },
  /* D7 /dr */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /dr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /dr */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /dr */ { BxImmediate_Ib, &BX_CPU_C::IN_EAXIb },
  /* E6 /dr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /dr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbEAX },
  /* E8 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /dr */ { 0, &BX_CPU_C::BxError },
  /* EB /dr */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /dr */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /dr */ { 0, &BX_CPU_C::IN_EAXDX },
  /* EE /dr */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /dr */ { 0, &BX_CPU_C::OUT_DXEAX },
  /* F0 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK:
  /* F1 /dr */ { 0, &BX_CPU_C::INT1 },
  /* F2 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /dr */ { BxPrefix, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 /dr */ { 0, &BX_CPU_C::HLT },
  /* F5 /dr */ { 0, &BX_CPU_C::CMC },
  /* F6 /dr */ { BxGroup3, NULL, BxOpcodeInfoG3EbR },
  /* F7 /dr */ { BxGroup3, NULL, BxOpcodeInfoG3EdR },
  /* F8 /dr */ { 0, &BX_CPU_C::CLC },
  /* F9 /dr */ { 0, &BX_CPU_C::STC },
  /* FA /dr */ { 0, &BX_CPU_C::CLI },
  /* FB /dr */ { 0, &BX_CPU_C::STI },
  /* FC /dr */ { 0, &BX_CPU_C::CLD },
  /* FD /dr */ { 0, &BX_CPU_C::STD },
  /* FE /dr */ { BxGroup4, NULL, BxOpcodeInfoG4R },
  /* FF /dr */ { BxGroup5, NULL, BxOpcodeInfo64G5dR },

  /* 0F 00 /dr */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /dr */ { BxGroup7, NULL, BxOpcodeInfo64G7R },
  /* 0F 02 /dr */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /dr */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /dr */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /dr */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /dr */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /dr */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /dr */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /dr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /dr */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /dr */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /dr */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /dr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /dr */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /dr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /dr */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /dr */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /dr */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /dr */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /dr */ { 0, &BX_CPU_C::BxError }, // MOVNTPS/PD/SS/SD
  /* 0F 2C /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /dr */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /dr */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /dr */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /dr */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /dr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /dr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /dr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /dr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /dr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /dr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /dr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /dr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 41 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 42 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 43 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 44 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 45 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 46 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 47 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 48 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 49 /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4A /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4B /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4C /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4D /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4E /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4F /dr */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 50 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /dr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /dr */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /dr */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /dr */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /dr */ { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /dr */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /dr */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /dr */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /dr */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /dr */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /dr */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /dr */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /dr */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /dr */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /dr */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /dr */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /dr */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /dr */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /dr */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /dr */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /dr */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /dr */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /dr */ { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 /dr */ { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 /dr */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /dr */ { 0, &BX_CPU_C::BT_EdGdR },
  /* 0F A4 /dr */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EdGd },
  /* 0F A5 /dr */ { 0,              &BX_CPU_C::SHLD_EdGd },
  /* 0F A6 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /dr */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /dr */ { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 /dr */ { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA /dr */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /dr */ { 0, &BX_CPU_C::BTS_EdGdR },
  /* 0F AC /dr */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EdGd },
  /* 0F AD /dr */ { 0,              &BX_CPU_C::SHRD_EdGd },
  /* 0F AE /dr */ { BxGroup15, NULL, BxOpcodeInfoG15R },
  /* 0F AF /dr */ { 0, &BX_CPU_C::IMUL_GdEd },
  /* 0F B0 /dr */ { 0, &BX_CPU_C::CMPXCHG_EbGbR },
  /* 0F B1 /dr */ { 0, &BX_CPU_C::CMPXCHG_EdGdR },
  /* 0F B2 /dr */ { 0, &BX_CPU_C::BxError }, // LSS
  /* 0F B3 /dr */ { 0, &BX_CPU_C::BTR_EdGdR },
  /* 0F B4 /dr */ { 0, &BX_CPU_C::BxError }, // LFS
  /* 0F B5 /dr */ { 0, &BX_CPU_C::BxError }, // LFS
  /* 0F B6 /dr */ { 0, &BX_CPU_C::MOVZX_GdEbR },
  /* 0F B7 /dr */ { 0, &BX_CPU_C::MOVZX_GdEwR },
  /* 0F B8 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8d },
  /* 0F B9 /dr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /dr */ { BxGroup8, NULL, BxOpcodeInfoG8EdIb },
  /* 0F BB /dr */ { 0, &BX_CPU_C::BTC_EdGdR },
  /* 0F BC /dr */ { 0, &BX_CPU_C::BSF_GdEd },
  /* 0F BD /dr */ { 0, &BX_CPU_C::BSR_GdEd },
  /* 0F BE /dr */ { 0, &BX_CPU_C::MOVSX_GdEbR },
  /* 0F BF /dr */ { 0, &BX_CPU_C::MOVSX_GdEwR },
  /* 0F C0 /dr */ { 0, &BX_CPU_C::XADD_EbGbR },
  /* 0F C1 /dr */ { 0, &BX_CPU_C::XADD_EdGdR },
  /* 0F C2 /dr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /dr */ { 0, &BX_CPU_C::BxError }, // MOVNTI SSE group
  /* 0F C4 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /dr */ { 0, &BX_CPU_C::BxError }, // CMPXCHG8B G9 group
  /* 0F C8 /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F C9 /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CA /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CB /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CC /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CD /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CE /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CF /dr */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F D0 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /dr */ { 0, &BX_CPU_C::BxError }, // MOVNTQ/MOVNTDQ SSE group
  /* 0F E8 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /dr */ { 0, &BX_CPU_C::BxError }, // LDDQU SSE group
  /* 0F F1 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /dr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /dr */ { 0, &BX_CPU_C::BxError },

  // 512 entries for 64bit operand size
  /* 00 /qr */ { 0, &BX_CPU_C::ADD_EbGbR },
  /* 01 /qr */ { 0, &BX_CPU_C::ADD_EqGqR },
  /* 02 /qr */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /qr */ { 0, &BX_CPU_C::ADD_GqEq },
  /* 04 /qr */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /qr */ { BxImmediate_Iv, &BX_CPU_C::ADD_RAXId },
  /* 06 /qr */ { 0, &BX_CPU_C::BxError },
  /* 07 /qr */ { 0, &BX_CPU_C::BxError },
  /* 08 /qr */ { 0, &BX_CPU_C::OR_EbGbR },
  /* 09 /qr */ { 0, &BX_CPU_C::OR_EqGqR },
  /* 0A /qr */ { 0, &BX_CPU_C::OR_GbEb },
  /* 0B /qr */ { 0, &BX_CPU_C::OR_GqEq },
  /* 0C /qr */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /qr */ { BxImmediate_Iv, &BX_CPU_C::OR_RAXId },
  /* 0E /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F /qr */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /qr */ { 0, &BX_CPU_C::ADC_EbGbR },
  /* 11 /qr */ { 0, &BX_CPU_C::ADC_EqGqR },
  /* 12 /qr */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /qr */ { 0, &BX_CPU_C::ADC_GqEq },
  /* 14 /qr */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /qr */ { BxImmediate_Iv, &BX_CPU_C::ADC_RAXId },
  /* 16 /qr */ { 0, &BX_CPU_C::BxError },
  /* 17 /qr */ { 0, &BX_CPU_C::BxError },
  /* 18 /qr */ { 0, &BX_CPU_C::SBB_EbGbR },
  /* 19 /qr */ { 0, &BX_CPU_C::SBB_EqGqR },
  /* 1A /qr */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /qr */ { 0, &BX_CPU_C::SBB_GqEq },
  /* 1C /qr */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /qr */ { BxImmediate_Iv, &BX_CPU_C::SBB_RAXId },
  /* 1E /qr */ { 0, &BX_CPU_C::BxError },
  /* 1F /qr */ { 0, &BX_CPU_C::BxError },
  /* 20 /qr */ { 0, &BX_CPU_C::AND_EbGbR },
  /* 21 /qr */ { 0, &BX_CPU_C::AND_EqGqR },
  /* 22 /qr */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /qr */ { 0, &BX_CPU_C::AND_GqEq },
  /* 24 /qr */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /qr */ { BxImmediate_Iv, &BX_CPU_C::AND_RAXId },
  /* 26 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /qr */ { 0, &BX_CPU_C::BxError },
  /* 28 /qr */ { 0, &BX_CPU_C::SUB_EbGbR },
  /* 29 /qr */ { 0, &BX_CPU_C::SUB_EqGqR },
  /* 2A /qr */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /qr */ { 0, &BX_CPU_C::SUB_GqEq },
  /* 2C /qr */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /qr */ { BxImmediate_Iv, &BX_CPU_C::SUB_RAXId },
  /* 2E /qr */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /qr */ { 0, &BX_CPU_C::BxError },
  /* 30 /qr */ { 0, &BX_CPU_C::XOR_EbGbR },
  /* 31 /qr */ { 0, &BX_CPU_C::XOR_EqGqR },
  /* 32 /qr */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /qr */ { 0, &BX_CPU_C::XOR_GqEq },
  /* 34 /qr */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /qr */ { BxImmediate_Iv, &BX_CPU_C::XOR_RAXId },
  /* 36 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /qr */ { 0, &BX_CPU_C::BxError },
  /* 38 /qr */ { 0, &BX_CPU_C::CMP_EbGbR },
  /* 39 /qr */ { 0, &BX_CPU_C::CMP_EqGqR },
  /* 3A /qr */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /qr */ { 0, &BX_CPU_C::CMP_GqEq },
  /* 3C /qr */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /qr */ { BxImmediate_Iv, &BX_CPU_C::CMP_RAXId },
  /* 3E /qr */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /qr */ { 0, &BX_CPU_C::BxError },
  /* 40 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 /qr */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 59 /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5A /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5B /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5C /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5D /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5E /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 5F /qr */ { 0, &BX_CPU_C::POP_RRX },
  /* 60 /qr */ { 0, &BX_CPU_C::BxError },
  /* 61 /qr */ { 0, &BX_CPU_C::BxError },
  /* 62 /qr */ { 0, &BX_CPU_C::BxError },
  /* 63 /qr */ { 0, &BX_CPU_C::MOVSX_GqEdR },
  /* 64 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /qr */ { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 /qr */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GqEqId },
  /* 6A /qr */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B /qr */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GqEqId },
  /* 6C /qr */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /qr */ { 0, &BX_CPU_C::REP_INSD_YdDX },
  /* 6E /qr */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /qr */ { 0, &BX_CPU_C::REP_OUTSD_DXXd },
  /* 70 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /qr */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbR },
  /* 81 /qr */ { BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfo64G1EqR },
  /* 82 /qr */ { 0, &BX_CPU_C::BxError },
  /* 83 /qr */ { BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfo64G1EqR },
  /* 84 /qr */ { 0, &BX_CPU_C::TEST_EbGbR },
  /* 85 /qr */ { 0, &BX_CPU_C::TEST_EqGqR },
  /* 86 /qr */ { 0, &BX_CPU_C::XCHG_EbGbR },
  /* 87 /qr */ { 0, &BX_CPU_C::XCHG_EqGqR },
  /* 88 /qr */ { 0, &BX_CPU_C::MOV_EbGbR },
  /* 89 /qr */ { 0, &BX_CPU_C::MOV_EqGqR },
  /* 8A /qr */ { 0, &BX_CPU_C::MOV_GbEbR },
  /* 8B /qr */ { 0, &BX_CPU_C::MOV_GqEqR },
  /* 8C /qr */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /qr */ { 0, &BX_CPU_C::BxError },     // LEA
  /* 8E /qr */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /qr */ { 0, &BX_CPU_C::POP_EqR },
  /* 90 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX }, // handles XCHG R8, RAX
  /* 91 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 92 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 93 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 94 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 95 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 96 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 97 /qr */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 98 /qr */ { 0, &BX_CPU_C::CDQE },
  /* 99 /qr */ { 0, &BX_CPU_C::CQO },
  /* 9A /qr */ { 0, &BX_CPU_C::BxError },
  /* 9B /qr */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /qr */ { 0, &BX_CPU_C::PUSHF_Fq },
  /* 9D /qr */ { 0, &BX_CPU_C::POPF_Fq },
  /* 9E /qr */ { 0, &BX_CPU_C::SAHF },
  /* 9F /qr */ { 0, &BX_CPU_C::LAHF },
  /* A0 /qr */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /qr */ { BxImmediate_O, &BX_CPU_C::MOV_RAXOq },
  /* A2 /qr */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /qr */ { BxImmediate_O, &BX_CPU_C::MOV_OqRAX },
  /* A4 /qr */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /qr */ { 0, &BX_CPU_C::REP_MOVSQ_XqYq },
  /* A6 /qr */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /qr */ { 0, &BX_CPU_C::REP_CMPSQ_XqYq },
  /* A8 /qr */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /qr */ { BxImmediate_Iv, &BX_CPU_C::TEST_RAXId },
  /* AA /qr */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /qr */ { 0, &BX_CPU_C::REP_STOSQ_YqRAX },
  /* AC /qr */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /qr */ { 0, &BX_CPU_C::REP_LODSQ_RAXXq },
  /* AE /qr */ { 0, &BX_CPU_C::REP_SCASB_ALXb  },
  /* AF /qr */ { 0, &BX_CPU_C::REP_SCASQ_RAXXq },
  /* B0 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* B9 /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BA /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BB /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BC /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BD /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BE /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BF /qr */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* C0 /qr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /qr */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eq },
  /* C2 /qr */ { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 /qr */ { 0,              &BX_CPU_C::RETnear64 },
  /* C4 /qr */ { 0, &BX_CPU_C::BxError },
  /* C5 /qr */ { 0, &BX_CPU_C::BxError },
  /* C6 /qr */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbR },
  /* C7 /qr */ { BxImmediate_Iv, &BX_CPU_C::MOV_EqIdR },
  /* C8 /qr */ { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 /qr */ { 0, &BX_CPU_C::LEAVE64 },
  /* CA /qr */ { BxImmediate_Iw, &BX_CPU_C::RETfar64_Iw },
  /* CB /qr */ { 0, &BX_CPU_C::RETfar64 },
  /* CC /qr */ { 0, &BX_CPU_C::INT3 },
  /* CD /qr */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /qr */ { 0, &BX_CPU_C::BxError },
  /* CF /qr */ { 0, &BX_CPU_C::IRET64 },
  /* D0 /qr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /qr */ { BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D2 /qr */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /qr */ { BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D4 /qr */ { 0, &BX_CPU_C::BxError },
  /* D5 /qr */ { 0, &BX_CPU_C::BxError },
  /* D6 /qr */ { 0, &BX_CPU_C::BxError },
  /* D7 /qr */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /qr */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /qr */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /qr */ { BxImmediate_Ib, &BX_CPU_C::IN_EAXIb },
  /* E6 /qr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /qr */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbEAX },
  /* E8 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /qr */ { 0, &BX_CPU_C::BxError },
  /* EB /qr */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /qr */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /qr */ { 0, &BX_CPU_C::IN_EAXDX },
  /* EE /qr */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /qr */ { 0, &BX_CPU_C::OUT_DXEAX },
  /* F0 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK:
  /* F1 /qr */ { 0, &BX_CPU_C::INT1 },
  /* F2 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /qr */ { BxPrefix, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 /qr */ { 0, &BX_CPU_C::HLT },
  /* F5 /qr */ { 0, &BX_CPU_C::CMC },
  /* F6 /qr */ { BxGroup3, NULL, BxOpcodeInfoG3EbR },
  /* F7 /qr */ { BxGroup3, NULL, BxOpcodeInfo64G3EqR },
  /* F8 /qr */ { 0, &BX_CPU_C::CLC },
  /* F9 /qr */ { 0, &BX_CPU_C::STC },
  /* FA /qr */ { 0, &BX_CPU_C::CLI },
  /* FB /qr */ { 0, &BX_CPU_C::STI },
  /* FC /qr */ { 0, &BX_CPU_C::CLD },
  /* FD /qr */ { 0, &BX_CPU_C::STD },
  /* FE /qr */ { BxGroup4, NULL, BxOpcodeInfoG4R },
  /* FF /qr */ { BxGroup5, NULL, BxOpcodeInfo64G5qR },

  /* 0F 00 /qr */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /qr */ { BxGroup7, NULL, BxOpcodeInfo64G7R },
  /* 0F 02 /qr */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /qr */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /qr */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /qr */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /qr */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /qr */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /qr */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /qr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /qr */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /qr */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /qr */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /qr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /qr */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /qr */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /qr */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /qr */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /qr */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /qr */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /qr */ { 0, &BX_CPU_C::BxError }, // MOVNTPS/PD/SS/SD
  /* 0F 2C /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /qr */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /qr */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /qr */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /qr */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /qr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /qr */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /qr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /qr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /qr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /qr */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /qr */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /qr */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 41 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 42 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 43 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 44 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 45 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 46 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 47 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 48 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 49 /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4A /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4B /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4C /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4D /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4E /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4F /qr */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 50 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /qr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /qr */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /qr */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /qr */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /qr */ { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /qr */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /qr */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /qr */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /qr */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /qr */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /qr */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /qr */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /qr */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /qr */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /qr */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /qr */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /qr */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /qr */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /qr */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /qr */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /qr */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /qr */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /qr */ { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 /qr */ { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 /qr */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /qr */ { 0, &BX_CPU_C::BT_EqGqR },
  /* 0F A4 /qr */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EqGq },
  /* 0F A5 /qr */ { 0,              &BX_CPU_C::SHLD_EqGq },
  /* 0F A6 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /qr */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /qr */ { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 /qr */ { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA /qr */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /qr */ { 0, &BX_CPU_C::BTS_EqGqR },
  /* 0F AC /qr */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EqGq },
  /* 0F AD /qr */ { 0,              &BX_CPU_C::SHRD_EqGq },
  /* 0F AE /qr */ { BxGroup15, NULL, BxOpcodeInfoG15R },
  /* 0F AF /qr */ { 0, &BX_CPU_C::IMUL_GqEq },
  /* 0F B0 /qr */ { 0, &BX_CPU_C::CMPXCHG_EbGbR },
  /* 0F B1 /qr */ { 0, &BX_CPU_C::CMPXCHG_EqGqR },
  /* 0F B2 /qr */ { 0, &BX_CPU_C::BxError }, // LSS
  /* 0F B3 /qr */ { 0, &BX_CPU_C::BTR_EqGqR },
  /* 0F B4 /qr */ { 0, &BX_CPU_C::BxError }, // LFS
  /* 0F B5 /qr */ { 0, &BX_CPU_C::BxError }, // LGS
  /* 0F B6 /qr */ { 0, &BX_CPU_C::MOVZX_GqEbR },
  /* 0F B7 /qr */ { 0, &BX_CPU_C::MOVZX_GqEwR },
  /* 0F B8 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8q },
  /* 0F B9 /qr */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /qr */ { BxGroup8, NULL, BxOpcodeInfo64G8EqIb },
  /* 0F BB /qr */ { 0, &BX_CPU_C::BTC_EqGqR },
  /* 0F BC /qr */ { 0, &BX_CPU_C::BSF_GqEq },
  /* 0F BD /qr */ { 0, &BX_CPU_C::BSR_GqEq },
  /* 0F BE /qr */ { 0, &BX_CPU_C::MOVSX_GqEbR },
  /* 0F BF /qr */ { 0, &BX_CPU_C::MOVSX_GqEwR },
  /* 0F C0 /qr */ { 0, &BX_CPU_C::XADD_EbGbR },
  /* 0F C1 /qr */ { 0, &BX_CPU_C::XADD_EqGqR },
  /* 0F C2 /qr */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /qr */ { 0, &BX_CPU_C::BxError }, // MOVNTI SSE group
  /* 0F C4 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /qr */ { 0, &BX_CPU_C::BxError }, // CMPXCHG16B G9 group
  /* 0F C8 /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F C9 /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CA /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CB /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CC /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CD /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CE /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CF /qr */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F D0 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /qr */ { 0, &BX_CPU_C::BxError }, // MOVNTQ/MOVNTDQ SSE group
  /* 0F E8 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /qr */ { 0, &BX_CPU_C::BxError }, // LDDQU SSE group
  /* 0F F1 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /qr */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /qr */ { 0, &BX_CPU_C::BxError }
};

// 512 entries for 16bit operand size
// 512 entries for 32bit operand size
// 512 entries for 64bit operand size

static const BxOpcodeInfo_t BxOpcodeInfo64M[512*3] = {
  // 512 entries for 16bit operand size
  /* 00 /wm */ { BxLockable, &BX_CPU_C::ADD_EbGbM },
  /* 01 /wm */ { BxLockable, &BX_CPU_C::ADD_EwGwM },
  /* 02 /wm */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /wm */ { 0, &BX_CPU_C::ADD_GwEwM },
  /* 04 /wm */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /wm */ { BxImmediate_Iv, &BX_CPU_C::ADD_AXIw },
  /* 06 /wm */ { 0, &BX_CPU_C::BxError },
  /* 07 /wm */ { 0, &BX_CPU_C::BxError },
  /* 08 /wm */ { BxLockable, &BX_CPU_C::OR_EbGbM },
  /* 09 /wm */ { BxLockable, &BX_CPU_C::OR_EwGwM },
  /* 0A /wm */ { 0, &BX_CPU_C::OR_GbEb },
  /* 0B /wm */ { 0, &BX_CPU_C::OR_GwEw },
  /* 0C /wm */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /wm */ { BxImmediate_Iv, &BX_CPU_C::OR_AXIw },
  /* 0E /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F /wm */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /wm */ { BxLockable, &BX_CPU_C::ADC_EbGbM },
  /* 11 /wm */ { BxLockable, &BX_CPU_C::ADC_EwGwM },
  /* 12 /wm */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /wm */ { 0, &BX_CPU_C::ADC_GwEw },
  /* 14 /wm */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /wm */ { BxImmediate_Iv, &BX_CPU_C::ADC_AXIw },
  /* 16 /wm */ { 0, &BX_CPU_C::BxError },
  /* 17 /wm */ { 0, &BX_CPU_C::BxError },
  /* 18 /wm */ { BxLockable, &BX_CPU_C::SBB_EbGbM },
  /* 19 /wm */ { BxLockable, &BX_CPU_C::SBB_EwGwM },
  /* 1A /wm */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /wm */ { 0, &BX_CPU_C::SBB_GwEw },
  /* 1C /wm */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /wm */ { BxImmediate_Iv, &BX_CPU_C::SBB_AXIw },
  /* 1E /wm */ { 0, &BX_CPU_C::BxError },
  /* 1F /wm */ { 0, &BX_CPU_C::BxError },
  /* 20 /wm */ { BxLockable, &BX_CPU_C::AND_EbGbM },
  /* 21 /wm */ { BxLockable, &BX_CPU_C::AND_EwGwM },
  /* 22 /wm */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /wm */ { 0, &BX_CPU_C::AND_GwEw },
  /* 24 /wm */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /wm */ { BxImmediate_Iv, &BX_CPU_C::AND_AXIw },
  /* 26 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /wm */ { 0, &BX_CPU_C::BxError },
  /* 28 /wm */ { BxLockable, &BX_CPU_C::SUB_EbGbM },
  /* 29 /wm */ { BxLockable, &BX_CPU_C::SUB_EwGwM },
  /* 2A /wm */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /wm */ { 0, &BX_CPU_C::SUB_GwEw },
  /* 2C /wm */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /wm */ { BxImmediate_Iv, &BX_CPU_C::SUB_AXIw },
  /* 2E /wm */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /wm */ { 0, &BX_CPU_C::BxError },
  /* 30 /wm */ { BxLockable, &BX_CPU_C::XOR_EbGbM },
  /* 31 /wm */ { BxLockable, &BX_CPU_C::XOR_EwGwM },
  /* 32 /wm */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /wm */ { 0, &BX_CPU_C::XOR_GwEw },
  /* 34 /wm */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /wm */ { BxImmediate_Iv, &BX_CPU_C::XOR_AXIw },
  /* 36 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /wm */ { 0, &BX_CPU_C::BxError },
  /* 38 /wm */ { 0, &BX_CPU_C::CMP_EbGbM },
  /* 39 /wm */ { 0, &BX_CPU_C::CMP_EwGwM },
  /* 3A /wm */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /wm */ { 0, &BX_CPU_C::CMP_GwEw },
  /* 3C /wm */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /wm */ { BxImmediate_Iv, &BX_CPU_C::CMP_AXIw },
  /* 3E /wm */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /wm */ { 0, &BX_CPU_C::BxError },
  /* 40 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 51 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 52 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 53 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 54 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 55 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 56 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 57 /wm */ { 0, &BX_CPU_C::PUSH_RX },
  /* 58 /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 59 /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5A /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5B /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5C /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5D /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5E /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 5F /wm */ { 0, &BX_CPU_C::POP_RX },
  /* 60 /wm */ { 0, &BX_CPU_C::BxError },
  /* 61 /wm */ { 0, &BX_CPU_C::BxError },
  /* 62 /wm */ { 0, &BX_CPU_C::BxError },
  /* 63 /wm */ { 0, &BX_CPU_C::MOV_GwEwM }, // MOVSX_GwEw
  /* 64 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /wm */ { BxImmediate_Iv, &BX_CPU_C::PUSH_Iw },
  /* 69 /wm */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GwEwIw },
  /* 6A /wm */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH_Iw },
  /* 6B /wm */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GwEwIw },
  /* 6C /wm */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /wm */ { 0, &BX_CPU_C::REP_INSW_YwDX },
  /* 6E /wm */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /wm */ { 0, &BX_CPU_C::REP_OUTSW_DXXw },
  /* 70 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /wm */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbM },
  /* 81 /wm */ { BxGroup1 | BxImmediate_Iv,    NULL, BxOpcodeInfoG1EwM },
  /* 82 /wm */ { 0, &BX_CPU_C::BxError },
  /* 83 /wm */ { BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1EwM },
  /* 84 /wm */ { 0, &BX_CPU_C::TEST_EbGbM },
  /* 85 /wm */ { 0, &BX_CPU_C::TEST_EwGwM },
  /* 86 /wm */ { BxLockable, &BX_CPU_C::XCHG_EbGbM },
  /* 87 /wm */ { BxLockable, &BX_CPU_C::XCHG_EwGw },
  /* 88 /wm */ { 0, &BX_CPU_C::MOV_EbGbM },
  /* 89 /wm */ { 0, &BX_CPU_C::MOV_EwGwM },
  /* 8A /wm */ { 0, &BX_CPU_C::MOV_GbEbM },
  /* 8B /wm */ { 0, &BX_CPU_C::MOV_GwEwM },
  /* 8C /wm */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /wm */ { 0, &BX_CPU_C::LEA_GwM },
  /* 8E /wm */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /wm */ { 0, &BX_CPU_C::POP_EwM },
  /* 90 /wm */ { 0, &BX_CPU_C::XCHG_RXAX }, // handles XCHG R8w, AX
  /* 91 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 92 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 93 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 94 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 95 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 96 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 97 /wm */ { 0, &BX_CPU_C::XCHG_RXAX },
  /* 98 /wm */ { 0, &BX_CPU_C::CBW },
  /* 99 /wm */ { 0, &BX_CPU_C::CWD },
  /* 9A /wm */ { 0, &BX_CPU_C::BxError },
  /* 9B /wm */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /wm */ { 0, &BX_CPU_C::PUSHF_Fw },
  /* 9D /wm */ { 0, &BX_CPU_C::POPF_Fw },
  /* 9E /wm */ { 0, &BX_CPU_C::SAHF },
  /* 9F /wm */ { 0, &BX_CPU_C::LAHF },
  /* A0 /wm */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /wm */ { BxImmediate_O, &BX_CPU_C::MOV_AXOq },
  /* A2 /wm */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /wm */ { BxImmediate_O, &BX_CPU_C::MOV_OqAX },
  /* A4 /wm */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /wm */ { 0, &BX_CPU_C::REP_MOVSW_XwYw },
  /* A6 /wm */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /wm */ { 0, &BX_CPU_C::REP_CMPSW_XwYw },
  /* A8 /wm */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /wm */ { BxImmediate_Iv, &BX_CPU_C::TEST_AXIw },
  /* AA /wm */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /wm */ { 0, &BX_CPU_C::REP_STOSW_YwAX },
  /* AC /wm */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /wm */ { 0, &BX_CPU_C::REP_LODSW_AXXw },
  /* AE /wm */ { 0, &BX_CPU_C::REP_SCASB_ALXb },
  /* AF /wm */ { 0, &BX_CPU_C::REP_SCASW_AXXw },
  /* B0 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* B9 /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BA /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BB /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BC /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BD /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BE /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BF /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* C0 /wm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /wm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ew },
  /* C2 /wm */ { BxImmediate_Iw, &BX_CPU_C::RETnear16_Iw },
  /* C3 /wm */ { 0,              &BX_CPU_C::RETnear16 },
  /* C4 /wm */ { 0, &BX_CPU_C::BxError },
  /* C5 /wm */ { 0, &BX_CPU_C::BxError },
  /* C6 /wm */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbM },
  /* C7 /wm */ { BxImmediate_Iv, &BX_CPU_C::MOV_EwIwM },
  /* C8 /wm */ { BxImmediate_IwIb, &BX_CPU_C::ENTER_IwIb },
  /* C9 /wm */ { 0, &BX_CPU_C::LEAVE },
  /* CA /wm */ { BxImmediate_Iw, &BX_CPU_C::RETfar16_Iw },
  /* CB /wm */ { 0, &BX_CPU_C::RETfar16 },
  /* CC /wm */ { 0, &BX_CPU_C::INT3 },
  /* CD /wm */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /wm */ { 0, &BX_CPU_C::BxError },
  /* CF /wm */ { 0, &BX_CPU_C::IRET16 },
  /* D0 /wm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /wm */ { BxGroup2, NULL, BxOpcodeInfoG2Ew },
  /* D2 /wm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /wm */ { BxGroup2, NULL, BxOpcodeInfoG2Ew },
  /* D4 /wm */ { 0, &BX_CPU_C::BxError },
  /* D5 /wm */ { 0, &BX_CPU_C::BxError },
  /* D6 /wm */ { 0, &BX_CPU_C::BxError },
  /* D7 /wm */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /wm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /wm */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /wm */ { BxImmediate_Ib, &BX_CPU_C::IN_AXIb },
  /* E6 /wm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /wm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAX },
  /* E8 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /wm */ { 0, &BX_CPU_C::BxError },
  /* EB /wm */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /wm */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /wm */ { 0, &BX_CPU_C::IN_AXDX },
  /* EE /wm */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /wm */ { 0, &BX_CPU_C::OUT_DXAX },
  /* F0 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK
  /* F1 /wm */ { 0, &BX_CPU_C::INT1 },
  /* F2 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /wm */ { BxPrefix, &BX_CPU_C::BxError }, // REP, REPE/REPZ
  /* F4 /wm */ { 0, &BX_CPU_C::HLT },
  /* F5 /wm */ { 0, &BX_CPU_C::CMC },
  /* F6 /wm */ { BxGroup3, NULL, BxOpcodeInfoG3EbM },
  /* F7 /wm */ { BxGroup3, NULL, BxOpcodeInfoG3EwM },
  /* F8 /wm */ { 0, &BX_CPU_C::CLC },
  /* F9 /wm */ { 0, &BX_CPU_C::STC },
  /* FA /wm */ { 0, &BX_CPU_C::CLI },
  /* FB /wm */ { 0, &BX_CPU_C::STI },
  /* FC /wm */ { 0, &BX_CPU_C::CLD },
  /* FD /wm */ { 0, &BX_CPU_C::STD },
  /* FE /wm */ { BxGroup4, NULL, BxOpcodeInfoG4M },
  /* FF /wm */ { BxGroup5, NULL, BxOpcodeInfo64G5wM },

  /* 0F 00 /wm */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /wm */ { BxGroup7, NULL, BxOpcodeInfo64G7M },
  /* 0F 02 /wm */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /wm */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /wm */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /wm */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /wm */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /wm */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /wm */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /wm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /wm */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /wm */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /wm */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /wm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /wm */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /wm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /wm */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /wm */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /wm */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /wm */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2bM },
  /* 0F 2C /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /wm */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /wm */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /wm */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /wm */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /wm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /wm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /wm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /wm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /wm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /wm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /wm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /wm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 41 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 42 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 43 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 44 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 45 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 46 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 47 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 48 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 49 /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4A /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4B /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4C /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4D /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4E /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4F /wm */ { 0, &BX_CPU_C::CMOV_GwEw },
  /* 0F 50 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /wm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /wm */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /wm */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /wm */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /wm */ { 0, &BX_CPU_C::EMMS }, 
  /* 0F 78 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /wm */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /wm */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /wm */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /wm */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /wm */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /wm */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /wm */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /wm */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /wm */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /wm */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /wm */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /wm */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /wm */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /wm */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /wm */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /wm */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /wm */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /wm */ { 0, &BX_CPU_C::PUSH16_FS },
  /* 0F A1 /wm */ { 0, &BX_CPU_C::POP16_FS },
  /* 0F A2 /wm */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /wm */ { 0, &BX_CPU_C::BT_EwGwM },
  /* 0F A4 /wm */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EwGw },
  /* 0F A5 /wm */ { 0,              &BX_CPU_C::SHLD_EwGw },
  /* 0F A6 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /wm */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /wm */ { 0, &BX_CPU_C::PUSH16_GS },
  /* 0F A9 /wm */ { 0, &BX_CPU_C::POP16_GS },
  /* 0F AA /wm */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /wm */ { BxLockable, &BX_CPU_C::BTS_EwGwM },
  /* 0F AC /wm */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EwGw },
  /* 0F AD /wm */ { 0,              &BX_CPU_C::SHRD_EwGw },
  /* 0F AE /wm */ { BxGroup15, NULL, BxOpcodeInfoG15M },
  /* 0F AF /wm */ { 0, &BX_CPU_C::IMUL_GwEw },
  /* 0F B0 /wm */ { BxLockable, &BX_CPU_C::CMPXCHG_EbGbM },
  /* 0F B1 /wm */ { BxLockable, &BX_CPU_C::CMPXCHG_EwGwM },
  /* 0F B2 /wm */ { 0, &BX_CPU_C::LSS_GwMp },
  /* 0F B3 /wm */ { BxLockable, &BX_CPU_C::BTR_EwGwM },
  /* 0F B4 /wm */ { 0, &BX_CPU_C::LFS_GwMp },
  /* 0F B5 /wm */ { 0, &BX_CPU_C::LGS_GwMp },
  /* 0F B6 /wm */ { 0, &BX_CPU_C::MOVZX_GwEbM },
  /* 0F B7 /wm */ { 0, &BX_CPU_C::MOV_GwEwM }, // MOVZX_GwEw
  /* 0F B8 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8w },
  /* 0F B9 /wm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /wm */ { BxGroup8, NULL, BxOpcodeInfoG8EwIb },
  /* 0F BB /wm */ { BxLockable, &BX_CPU_C::BTC_EwGwM },
  /* 0F BC /wm */ { 0, &BX_CPU_C::BSF_GwEw },
  /* 0F BD /wm */ { 0, &BX_CPU_C::BSR_GwEw },
  /* 0F BE /wm */ { 0, &BX_CPU_C::MOVSX_GwEbM },
  /* 0F BF /wm */ { 0, &BX_CPU_C::MOV_GwEwM }, // MOVSX_GwEw
  /* 0F C0 /wm */ { BxLockable, &BX_CPU_C::XADD_EbGbM },
  /* 0F C1 /wm */ { BxLockable, &BX_CPU_C::XADD_EwGwM },
  /* 0F C2 /wm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3M },
  /* 0F C4 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /wm */ { BxGroup9, NULL, BxOpcodeInfoG9M },
  /* 0F C8 /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F C9 /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CA /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CB /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CC /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CD /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CE /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CF /wm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F D0 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7M },
  /* 0F E8 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff0M }, 
  /* 0F F1 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /wm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /wm */ { 0, &BX_CPU_C::BxError },

  // 512 entries for 32bit operand size
  /* 00 /dm */ { BxLockable, &BX_CPU_C::ADD_EbGbM },
  /* 01 /dm */ { BxLockable, &BX_CPU_C::ADD_EdGdM },
  /* 02 /dm */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /dm */ { 0, &BX_CPU_C::ADD_GdEdM },
  /* 04 /dm */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /dm */ { BxImmediate_Iv, &BX_CPU_C::ADD_EAXId },
  /* 06 /dm */ { 0, &BX_CPU_C::BxError },
  /* 07 /dm */ { 0, &BX_CPU_C::BxError },
  /* 08 /dm */ { BxLockable, &BX_CPU_C::OR_EbGbM },
  /* 09 /dm */ { BxLockable, &BX_CPU_C::OR_EdGdM },
  /* 0A /dm */ { 0, &BX_CPU_C::OR_GbEb },
  /* 0B /dm */ { 0, &BX_CPU_C::OR_GdEd },
  /* 0C /dm */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /dm */ { BxImmediate_Iv, &BX_CPU_C::OR_EAXId },
  /* 0E /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F /dm */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /dm */ { BxLockable, &BX_CPU_C::ADC_EbGbM },
  /* 11 /dm */ { BxLockable, &BX_CPU_C::ADC_EdGdM },
  /* 12 /dm */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /dm */ { 0, &BX_CPU_C::ADC_GdEd },
  /* 14 /dm */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /dm */ { BxImmediate_Iv, &BX_CPU_C::ADC_EAXId },
  /* 16 /dm */ { 0, &BX_CPU_C::BxError },
  /* 17 /dm */ { 0, &BX_CPU_C::BxError },
  /* 18 /dm */ { BxLockable, &BX_CPU_C::SBB_EbGbM },
  /* 19 /dm */ { BxLockable, &BX_CPU_C::SBB_EdGdM },
  /* 1A /dm */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /dm */ { 0, &BX_CPU_C::SBB_GdEd },
  /* 1C /dm */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /dm */ { BxImmediate_Iv, &BX_CPU_C::SBB_EAXId },
  /* 1E /dm */ { 0, &BX_CPU_C::BxError },
  /* 1F /dm */ { 0, &BX_CPU_C::BxError },
  /* 20 /dm */ { BxLockable, &BX_CPU_C::AND_EbGbM },
  /* 21 /dm */ { BxLockable, &BX_CPU_C::AND_EdGdM },
  /* 22 /dm */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /dm */ { 0, &BX_CPU_C::AND_GdEd },
  /* 24 /dm */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /dm */ { BxImmediate_Iv, &BX_CPU_C::AND_EAXId },
  /* 26 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /dm */ { 0, &BX_CPU_C::BxError },
  /* 28 /dm */ { BxLockable, &BX_CPU_C::SUB_EbGbM },
  /* 29 /dm */ { BxLockable, &BX_CPU_C::SUB_EdGdM },
  /* 2A /dm */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /dm */ { 0, &BX_CPU_C::SUB_GdEd },
  /* 2C /dm */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /dm */ { BxImmediate_Iv, &BX_CPU_C::SUB_EAXId },
  /* 2E /dm */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /dm */ { 0, &BX_CPU_C::BxError },
  /* 30 /dm */ { BxLockable, &BX_CPU_C::XOR_EbGbM },
  /* 31 /dm */ { BxLockable, &BX_CPU_C::XOR_EdGdM },
  /* 32 /dm */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /dm */ { 0, &BX_CPU_C::XOR_GdEd },
  /* 34 /dm */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /dm */ { BxImmediate_Iv, &BX_CPU_C::XOR_EAXId },
  /* 36 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /dm */ { 0, &BX_CPU_C::BxError },
  /* 38 /dm */ { 0, &BX_CPU_C::CMP_EbGbM },
  /* 39 /dm */ { 0, &BX_CPU_C::CMP_EdGdM },
  /* 3A /dm */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /dm */ { 0, &BX_CPU_C::CMP_GdEd },
  /* 3C /dm */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /dm */ { BxImmediate_Iv, &BX_CPU_C::CMP_EAXId },
  /* 3E /dm */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /dm */ { 0, &BX_CPU_C::BxError },
  /* 40 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 /dm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 59 /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5A /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5B /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5C /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5D /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5E /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5F /dm */ { 0, &BX_CPU_C::POP_RRX },
  /* 60 /dm */ { 0, &BX_CPU_C::BxError },
  /* 61 /dm */ { 0, &BX_CPU_C::BxError },
  /* 62 /dm */ { 0, &BX_CPU_C::BxError },
  /* 63 /dm */ { 0, &BX_CPU_C::MOV_GdEdM }, // MOVSX_GdEd
  /* 64 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /dm */ { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 /dm */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GdEdId },
  /* 6A /dm */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B /dm */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GdEdId },
  /* 6C /dm */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /dm */ { 0, &BX_CPU_C::REP_INSD_YdDX },
  /* 6E /dm */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /dm */ { 0, &BX_CPU_C::REP_OUTSD_DXXd },
  /* 70 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /dm */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbM },
  /* 81 /dm */ { BxGroup1 | BxImmediate_Iv,    NULL, BxOpcodeInfoG1EdM },
  /* 82 /dm */ { 0, &BX_CPU_C::BxError },
  /* 83 /dm */ { BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1EdM },
  /* 84 /dm */ { 0, &BX_CPU_C::TEST_EbGbM },
  /* 85 /dm */ { 0, &BX_CPU_C::TEST_EdGdM },
  /* 86 /dm */ { BxLockable, &BX_CPU_C::XCHG_EbGbM },
  /* 87 /dm */ { BxLockable, &BX_CPU_C::XCHG_EdGdM },
  /* 88 /dm */ { 0, &BX_CPU_C::MOV_EbGbM },
  /* 89 /dm */ { 0, &BX_CPU_C::MOV_EdGdM },
  /* 8A /dm */ { 0, &BX_CPU_C::MOV_GbEbM },
  /* 8B /dm */ { 0, &BX_CPU_C::MOV_GdEdM },
  /* 8C /dm */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /dm */ { 0, &BX_CPU_C::LEA_GdM },
  /* 8E /dm */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /dm */ { 0, &BX_CPU_C::POP_EqM },
  /* 90 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX }, // handles XCHG R8d, EAX
  /* 91 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 92 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 93 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 94 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 95 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 96 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 97 /dm */ { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 98 /dm */ { 0, &BX_CPU_C::CWDE },
  /* 99 /dm */ { 0, &BX_CPU_C::CDQ },
  /* 9A /dm */ { 0, &BX_CPU_C::BxError },
  /* 9B /dm */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /dm */ { 0, &BX_CPU_C::PUSHF_Fq },
  /* 9D /dm */ { 0, &BX_CPU_C::POPF_Fq },
  /* 9E /dm */ { 0, &BX_CPU_C::SAHF },
  /* 9F /dm */ { 0, &BX_CPU_C::LAHF },
  /* A0 /dm */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /dm */ { BxImmediate_O, &BX_CPU_C::MOV_EAXOq },
  /* A2 /dm */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /dm */ { BxImmediate_O, &BX_CPU_C::MOV_OqEAX },
  /* A4 /dm */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /dm */ { 0, &BX_CPU_C::REP_MOVSD_XdYd },
  /* A6 /dm */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /dm */ { 0, &BX_CPU_C::REP_CMPSD_XdYd },
  /* A8 /dm */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /dm */ { BxImmediate_Iv, &BX_CPU_C::TEST_EAXId },
  /* AA /dm */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /dm */ { 0, &BX_CPU_C::REP_STOSD_YdEAX },
  /* AC /dm */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /dm */ { 0, &BX_CPU_C::REP_LODSD_EAXXd },
  /* AE /dm */ { 0, &BX_CPU_C::REP_SCASB_ALXb  },
  /* AF /dm */ { 0, &BX_CPU_C::REP_SCASD_EAXXd },
  /* B0 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* B9 /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BA /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BB /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BC /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BD /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BE /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BF /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* C0 /dm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /dm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ed },
  /* C2 /dm */ { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 /dm */ { 0,              &BX_CPU_C::RETnear64 },
  /* C4 /dm */ { 0, &BX_CPU_C::BxError },
  /* C5 /dm */ { 0, &BX_CPU_C::BxError },
  /* C6 /dm */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbM },
  /* C7 /dm */ { BxImmediate_Iv, &BX_CPU_C::MOV_EdIdM },
  /* C8 /dm */ { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 /dm */ { 0, &BX_CPU_C::LEAVE64 },
  /* CA /dm */ { BxImmediate_Iw, &BX_CPU_C::RETfar32_Iw },
  /* CB /dm */ { 0, &BX_CPU_C::RETfar32 },
  /* CC /dm */ { 0, &BX_CPU_C::INT3 },
  /* CD /dm */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /dm */ { 0, &BX_CPU_C::BxError },
  /* CF /dm */ { 0, &BX_CPU_C::IRET32 },
  /* D0 /dm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /dm */ { BxGroup2, NULL, BxOpcodeInfoG2Ed },
  /* D2 /dm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /dm */ { BxGroup2, NULL, BxOpcodeInfoG2Ed },
  /* D4 /dm */ { 0, &BX_CPU_C::BxError },
  /* D5 /dm */ { 0, &BX_CPU_C::BxError },
  /* D6 /dm */ { 0, &BX_CPU_C::BxError },
  /* D7 /dm */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /dm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /dm */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /dm */ { BxImmediate_Ib, &BX_CPU_C::IN_EAXIb },
  /* E6 /dm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /dm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbEAX },
  /* E8 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /dm */ { 0, &BX_CPU_C::BxError },
  /* EB /dm */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /dm */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /dm */ { 0, &BX_CPU_C::IN_EAXDX },
  /* EE /dm */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /dm */ { 0, &BX_CPU_C::OUT_DXEAX },
  /* F0 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK:
  /* F1 /dm */ { 0, &BX_CPU_C::INT1 },
  /* F2 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /dm */ { BxPrefix, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 /dm */ { 0, &BX_CPU_C::HLT },
  /* F5 /dm */ { 0, &BX_CPU_C::CMC },
  /* F6 /dm */ { BxGroup3, NULL, BxOpcodeInfoG3EbM },
  /* F7 /dm */ { BxGroup3, NULL, BxOpcodeInfoG3EdM },
  /* F8 /dm */ { 0, &BX_CPU_C::CLC },
  /* F9 /dm */ { 0, &BX_CPU_C::STC },
  /* FA /dm */ { 0, &BX_CPU_C::CLI },
  /* FB /dm */ { 0, &BX_CPU_C::STI },
  /* FC /dm */ { 0, &BX_CPU_C::CLD },
  /* FD /dm */ { 0, &BX_CPU_C::STD },
  /* FE /dm */ { BxGroup4, NULL, BxOpcodeInfoG4M },
  /* FF /dm */ { BxGroup5, NULL, BxOpcodeInfo64G5dM },

  /* 0F 00 /dm */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /dm */ { BxGroup7, NULL, BxOpcodeInfo64G7M },
  /* 0F 02 /dm */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /dm */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /dm */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /dm */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /dm */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /dm */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /dm */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /dm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /dm */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /dm */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /dm */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /dm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /dm */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /dm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /dm */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /dm */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /dm */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /dm */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2bM },
  /* 0F 2C /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /dm */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /dm */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /dm */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /dm */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /dm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /dm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /dm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /dm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /dm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /dm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /dm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /dm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 41 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 42 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 43 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 44 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 45 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 46 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 47 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 48 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 49 /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4A /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4B /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4C /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4D /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4E /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4F /dm */ { 0, &BX_CPU_C::CMOV_GdEd },
  /* 0F 50 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /dm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /dm */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /dm */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /dm */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /dm */ { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /dm */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /dm */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /dm */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /dm */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /dm */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /dm */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /dm */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /dm */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /dm */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /dm */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /dm */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /dm */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /dm */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /dm */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /dm */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /dm */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /dm */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /dm */ { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 /dm */ { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 /dm */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /dm */ { 0, &BX_CPU_C::BT_EdGdM },
  /* 0F A4 /dm */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EdGd },
  /* 0F A5 /dm */ { 0,              &BX_CPU_C::SHLD_EdGd },
  /* 0F A6 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /dm */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /dm */ { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 /dm */ { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA /dm */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /dm */ { BxLockable, &BX_CPU_C::BTS_EdGdM },
  /* 0F AC /dm */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EdGd },
  /* 0F AD /dm */ { 0,              &BX_CPU_C::SHRD_EdGd },
  /* 0F AE /dm */ { BxGroup15, NULL, BxOpcodeInfoG15M },
  /* 0F AF /dm */ { 0, &BX_CPU_C::IMUL_GdEd },
  /* 0F B0 /dm */ { BxLockable, &BX_CPU_C::CMPXCHG_EbGbM },
  /* 0F B1 /dm */ { BxLockable, &BX_CPU_C::CMPXCHG_EdGdM },
  /* 0F B2 /dm */ { 0, &BX_CPU_C::LSS_GdMp },
  /* 0F B3 /dm */ { BxLockable, &BX_CPU_C::BTR_EdGdM },
  /* 0F B4 /dm */ { 0, &BX_CPU_C::LFS_GdMp },
  /* 0F B5 /dm */ { 0, &BX_CPU_C::LGS_GdMp },
  /* 0F B6 /dm */ { 0, &BX_CPU_C::MOVZX_GdEbM },
  /* 0F B7 /dm */ { 0, &BX_CPU_C::MOVZX_GdEwM },
  /* 0F B8 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8d },
  /* 0F B9 /dm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /dm */ { BxGroup8, NULL, BxOpcodeInfoG8EdIb },
  /* 0F BB /dm */ { BxLockable, &BX_CPU_C::BTC_EdGdM },
  /* 0F BC /dm */ { 0, &BX_CPU_C::BSF_GdEd },
  /* 0F BD /dm */ { 0, &BX_CPU_C::BSR_GdEd },
  /* 0F BE /dm */ { 0, &BX_CPU_C::MOVSX_GdEbM },
  /* 0F BF /dm */ { 0, &BX_CPU_C::MOVSX_GdEwM },
  /* 0F C0 /dm */ { BxLockable, &BX_CPU_C::XADD_EbGbM },
  /* 0F C1 /dm */ { BxLockable, &BX_CPU_C::XADD_EdGdM },
  /* 0F C2 /dm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3M },
  /* 0F C4 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /dm */ { BxGroup9, NULL, BxOpcodeInfoG9M },
  /* 0F C8 /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F C9 /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CA /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CB /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CC /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CD /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CE /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F CF /dm */ { 0, &BX_CPU_C::BSWAP_ERX },
  /* 0F D0 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7M },
  /* 0F E8 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff0M }, 
  /* 0F F1 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /dm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /dm */ { 0, &BX_CPU_C::BxError },

  // 512 entries for 64bit operand size
  /* 00 /qm */ { BxLockable, &BX_CPU_C::ADD_EbGbM },
  /* 01 /qm */ { BxLockable, &BX_CPU_C::ADD_EqGqM },
  /* 02 /qm */ { 0, &BX_CPU_C::ADD_GbEb },
  /* 03 /qm */ { 0, &BX_CPU_C::ADD_GqEq },
  /* 04 /qm */ { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 /qm */ { BxImmediate_Iv, &BX_CPU_C::ADD_RAXId },
  /* 06 /qm */ { 0, &BX_CPU_C::BxError },
  /* 07 /qm */ { 0, &BX_CPU_C::BxError },
  /* 08 /qm */ { 0, &BX_CPU_C::OR_EbGbM },
  /* 09 /qm */ { BxLockable, &BX_CPU_C::OR_EqGqM },
  /* 0A /qm */ { BxLockable, &BX_CPU_C::OR_GbEb },
  /* 0B /qm */ { 0, &BX_CPU_C::OR_GqEq },
  /* 0C /qm */ { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D /qm */ { BxImmediate_Iv, &BX_CPU_C::OR_RAXId },
  /* 0E /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F /qm */ { 0, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 /qm */ { BxLockable, &BX_CPU_C::ADC_EbGbM },
  /* 11 /qm */ { BxLockable, &BX_CPU_C::ADC_EqGqM },
  /* 12 /qm */ { 0, &BX_CPU_C::ADC_GbEb },
  /* 13 /qm */ { 0, &BX_CPU_C::ADC_GqEq },
  /* 14 /qm */ { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 /qm */ { BxImmediate_Iv, &BX_CPU_C::ADC_RAXId },
  /* 16 /qm */ { 0, &BX_CPU_C::BxError },
  /* 17 /qm */ { 0, &BX_CPU_C::BxError },
  /* 18 /qm */ { BxLockable, &BX_CPU_C::SBB_EbGbM },
  /* 19 /qm */ { BxLockable, &BX_CPU_C::SBB_EqGqM },
  /* 1A /qm */ { 0, &BX_CPU_C::SBB_GbEb },
  /* 1B /qm */ { 0, &BX_CPU_C::SBB_GqEq },
  /* 1C /qm */ { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D /qm */ { BxImmediate_Iv, &BX_CPU_C::SBB_RAXId },
  /* 1E /qm */ { 0, &BX_CPU_C::BxError },
  /* 1F /qm */ { 0, &BX_CPU_C::BxError },
  /* 20 /qm */ { BxLockable, &BX_CPU_C::AND_EbGbM },
  /* 21 /qm */ { BxLockable, &BX_CPU_C::AND_EqGqM },
  /* 22 /qm */ { 0, &BX_CPU_C::AND_GbEb },
  /* 23 /qm */ { 0, &BX_CPU_C::AND_GqEq },
  /* 24 /qm */ { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 /qm */ { BxImmediate_Iv, &BX_CPU_C::AND_RAXId },
  /* 26 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // ES:
  /* 27 /qm */ { 0, &BX_CPU_C::BxError },
  /* 28 /qm */ { BxLockable, &BX_CPU_C::SUB_EbGbM },
  /* 29 /qm */ { BxLockable, &BX_CPU_C::SUB_EqGqM },
  /* 2A /qm */ { 0, &BX_CPU_C::SUB_GbEb },
  /* 2B /qm */ { 0, &BX_CPU_C::SUB_GqEq },
  /* 2C /qm */ { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D /qm */ { BxImmediate_Iv, &BX_CPU_C::SUB_RAXId },
  /* 2E /qm */ { BxPrefix, &BX_CPU_C::BxError }, // CS:
  /* 2F /qm */ { 0, &BX_CPU_C::BxError },
  /* 30 /qm */ { BxLockable, &BX_CPU_C::XOR_EbGbM },
  /* 31 /qm */ { BxLockable, &BX_CPU_C::XOR_EqGqM },
  /* 32 /qm */ { 0, &BX_CPU_C::XOR_GbEb },
  /* 33 /qm */ { 0, &BX_CPU_C::XOR_GqEq },
  /* 34 /qm */ { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 /qm */ { BxImmediate_Iv, &BX_CPU_C::XOR_RAXId },
  /* 36 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // SS:
  /* 37 /qm */ { 0, &BX_CPU_C::BxError },
  /* 38 /qm */ { 0, &BX_CPU_C::CMP_EbGbM },
  /* 39 /qm */ { 0, &BX_CPU_C::CMP_EqGqM },
  /* 3A /qm */ { 0, &BX_CPU_C::CMP_GbEb },
  /* 3B /qm */ { 0, &BX_CPU_C::CMP_GqEq },
  /* 3C /qm */ { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D /qm */ { BxImmediate_Iv, &BX_CPU_C::CMP_RAXId },
  /* 3E /qm */ { BxPrefix, &BX_CPU_C::BxError }, // DS:
  /* 3F /qm */ { 0, &BX_CPU_C::BxError },
  /* 40 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 41 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 42 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 43 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 44 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 45 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 46 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 47 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 48 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 49 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4A /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4B /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4C /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4D /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4E /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 4F /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REX:
  /* 50 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 /qm */ { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 59 /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5A /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5B /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5C /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5D /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5E /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 5F /qm */ { 0, &BX_CPU_C::POP_RRX },
  /* 60 /qm */ { 0, &BX_CPU_C::BxError },
  /* 61 /qm */ { 0, &BX_CPU_C::BxError },
  /* 62 /qm */ { 0, &BX_CPU_C::BxError },
  /* 63 /qm */ { 0, &BX_CPU_C::MOVSX_GqEdM },
  /* 64 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // FS:
  /* 65 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // GS:
  /* 66 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // OS:
  /* 67 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // AS:
  /* 68 /qm */ { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 /qm */ { BxImmediate_Iv, &BX_CPU_C::IMUL_GqEqId },
  /* 6A /qm */ { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B /qm */ { BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GqEqId },
  /* 6C /qm */ { 0, &BX_CPU_C::REP_INSB_YbDX },
  /* 6D /qm */ { 0, &BX_CPU_C::REP_INSD_YdDX },
  /* 6E /qm */ { 0, &BX_CPU_C::REP_OUTSB_DXXb },
  /* 6F /qm */ { 0, &BX_CPU_C::REP_OUTSD_DXXd },
  /* 70 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JO_Jq },
  /* 71 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNO_Jq },
  /* 72 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JB_Jq },
  /* 73 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNB_Jq },
  /* 74 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JZ_Jq },
  /* 75 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNZ_Jq },
  /* 76 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JBE_Jq },
  /* 77 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNBE_Jq },
  /* 78 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JS_Jq },
  /* 79 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNS_Jq },
  /* 7A /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JP_Jq },
  /* 7B /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNP_Jq },
  /* 7C /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JL_Jq },
  /* 7D /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNL_Jq },
  /* 7E /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JLE_Jq },
  /* 7F /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JNLE_Jq },
  /* 80 /qm */ { BxGroup1 | BxImmediate_Ib, NULL, BxOpcodeInfoG1EbIbM },
  /* 81 /qm */ { BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfo64G1EqM },
  /* 82 /qm */ { 0, &BX_CPU_C::BxError },
  /* 83 /qm */ { BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfo64G1EqM },
  /* 84 /qm */ { 0, &BX_CPU_C::TEST_EbGbM },
  /* 85 /qm */ { 0, &BX_CPU_C::TEST_EqGqM },
  /* 86 /qm */ { BxLockable, &BX_CPU_C::XCHG_EbGbM },
  /* 87 /qm */ { BxLockable, &BX_CPU_C::XCHG_EqGqM },
  /* 88 /qm */ { 0, &BX_CPU_C::MOV_EbGbM },
  /* 89 /qm */ { 0, &BX_CPU_C::MOV_EqGqM },
  /* 8A /qm */ { 0, &BX_CPU_C::MOV_GbEbM },
  /* 8B /qm */ { 0, &BX_CPU_C::MOV_GqEqM },
  /* 8C /qm */ { 0, &BX_CPU_C::MOV_EwSw },
  /* 8D /qm */ { 0, &BX_CPU_C::LEA_GqM },
  /* 8E /qm */ { 0, &BX_CPU_C::MOV_SwEw },
  /* 8F /qm */ { 0, &BX_CPU_C::POP_EqM },
  /* 90 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX }, // handles XCHG R8, RAX
  /* 91 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 92 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 93 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 94 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 95 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 96 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 97 /qm */ { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 98 /qm */ { 0, &BX_CPU_C::CDQE },
  /* 99 /qm */ { 0, &BX_CPU_C::CQO },
  /* 9A /qm */ { 0, &BX_CPU_C::BxError },
  /* 9B /qm */ { 0, &BX_CPU_C::FWAIT },
  /* 9C /qm */ { 0, &BX_CPU_C::PUSHF_Fq },
  /* 9D /qm */ { 0, &BX_CPU_C::POPF_Fq },
  /* 9E /qm */ { 0, &BX_CPU_C::SAHF },
  /* 9F /qm */ { 0, &BX_CPU_C::LAHF },
  /* A0 /qm */ { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 /qm */ { BxImmediate_O, &BX_CPU_C::MOV_RAXOq },
  /* A2 /qm */ { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 /qm */ { BxImmediate_O, &BX_CPU_C::MOV_OqRAX },
  /* A4 /qm */ { 0, &BX_CPU_C::REP_MOVSB_XbYb },
  /* A5 /qm */ { 0, &BX_CPU_C::REP_MOVSQ_XqYq },
  /* A6 /qm */ { 0, &BX_CPU_C::REP_CMPSB_XbYb },
  /* A7 /qm */ { 0, &BX_CPU_C::REP_CMPSQ_XqYq },
  /* A8 /qm */ { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 /qm */ { BxImmediate_Iv, &BX_CPU_C::TEST_RAXId },
  /* AA /qm */ { 0, &BX_CPU_C::REP_STOSB_YbAL },
  /* AB /qm */ { 0, &BX_CPU_C::REP_STOSQ_YqRAX },
  /* AC /qm */ { 0, &BX_CPU_C::REP_LODSB_ALXb },
  /* AD /qm */ { 0, &BX_CPU_C::REP_LODSQ_RAXXq },
  /* AE /qm */ { 0, &BX_CPU_C::REP_SCASB_ALXb  },
  /* AF /qm */ { 0, &BX_CPU_C::REP_SCASQ_RAXXq },
  /* B0 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* B9 /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BA /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BB /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BC /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BD /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BE /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BF /qm */ { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* C0 /qm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 /qm */ { BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eq },
  /* C2 /qm */ { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 /qm */ { 0,              &BX_CPU_C::RETnear64 },
  /* C4 /qm */ { 0, &BX_CPU_C::BxError },
  /* C5 /qm */ { 0, &BX_CPU_C::BxError },
  /* C6 /qm */ { BxImmediate_Ib, &BX_CPU_C::MOV_EbIbM },
  /* C7 /qm */ { BxImmediate_Iv, &BX_CPU_C::MOV_EqIdM },
  /* C8 /qm */ { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 /qm */ { 0, &BX_CPU_C::LEAVE64 },
  /* CA /qm */ { BxImmediate_Iw, &BX_CPU_C::RETfar64_Iw },
  /* CB /qm */ { 0, &BX_CPU_C::RETfar64 },
  /* CC /qm */ { 0, &BX_CPU_C::INT3 },
  /* CD /qm */ { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE /qm */ { 0, &BX_CPU_C::BxError },
  /* CF /qm */ { 0, &BX_CPU_C::IRET64 },
  /* D0 /qm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D1 /qm */ { BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D2 /qm */ { BxGroup2, NULL, BxOpcodeInfoG2Eb },
  /* D3 /qm */ { BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D4 /qm */ { 0, &BX_CPU_C::BxError },
  /* D5 /qm */ { 0, &BX_CPU_C::BxError },
  /* D6 /qm */ { 0, &BX_CPU_C::BxError },
  /* D7 /qm */ { 0, &BX_CPU_C::XLAT },
  //    by default we have here pointer to the group .. as if mod <> 11b
  /* D8 /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD8 },
  /* D9 /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupD9 },
  /* DA /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDA },
  /* DB /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDB },
  /* DC /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDC },
  /* DD /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDD },
  /* DE /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDE },
  /* DF /qm */ { BxFPGroup, NULL, BxOpcodeInfo_FPGroupDF },
  /* E0 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 /qm */ { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 /qm */ { BxImmediate_Ib, &BX_CPU_C::IN_EAXIb },
  /* E6 /qm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 /qm */ { BxImmediate_Ib, &BX_CPU_C::OUT_IbEAX },
  /* E8 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::CALL_Jq },
  /* E9 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA /qm */ { 0, &BX_CPU_C::BxError },
  /* EB /qm */ { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC /qm */ { 0, &BX_CPU_C::IN_ALDX },
  /* ED /qm */ { 0, &BX_CPU_C::IN_EAXDX },
  /* EE /qm */ { 0, &BX_CPU_C::OUT_DXAL },
  /* EF /qm */ { 0, &BX_CPU_C::OUT_DXEAX },
  /* F0 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // LOCK:
  /* F1 /qm */ { 0, &BX_CPU_C::INT1 },
  /* F2 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 /qm */ { BxPrefix, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 /qm */ { 0, &BX_CPU_C::HLT },
  /* F5 /qm */ { 0, &BX_CPU_C::CMC },
  /* F6 /qm */ { BxGroup3, NULL, BxOpcodeInfoG3EbM },
  /* F7 /qm */ { BxGroup3, NULL, BxOpcodeInfo64G3EqM },
  /* F8 /qm */ { 0, &BX_CPU_C::CLC },
  /* F9 /qm */ { 0, &BX_CPU_C::STC },
  /* FA /qm */ { 0, &BX_CPU_C::CLI },
  /* FB /qm */ { 0, &BX_CPU_C::STI },
  /* FC /qm */ { 0, &BX_CPU_C::CLD },
  /* FD /qm */ { 0, &BX_CPU_C::STD },
  /* FE /qm */ { BxGroup4, NULL, BxOpcodeInfoG4M },
  /* FF /qm */ { BxGroup5, NULL, BxOpcodeInfo64G5qM },

  /* 0F 00 /qm */ { BxGroup6, NULL, BxOpcodeInfoG6 },
  /* 0F 01 /qm */ { BxGroup7, NULL, BxOpcodeInfo64G7M },
  /* 0F 02 /qm */ { 0, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 /qm */ { 0, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 05 /qm */ { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 /qm */ { 0, &BX_CPU_C::CLTS },
  /* 0F 07 /qm */ { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 /qm */ { 0, &BX_CPU_C::INVD },
  /* 0F 09 /qm */ { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0B /qm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0D /qm */ { 0, &BX_CPU_C::NOP },             // 3DNow! PREFETCH on AMD, NOP on Intel
#if BX_SUPPORT_3DNOW
  /* 0F 0E /qm */ { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F /qm */ { BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0E /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 0F /qm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 /qm */ { 0, &BX_CPU_C::PREFETCH }, // opcode group G16, PREFETCH hints
  /* 0F 19 /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1A /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1B /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1C /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1D /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1E /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 1F /qm */ { 0, &BX_CPU_C::NOP },      // multi-byte NOP
  /* 0F 20 /qm */ { 0, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 /qm */ { 0, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 /qm */ { 0, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 /qm */ { 0, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 25 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 26 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 27 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 28 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2bM },
  /* 0F 2C /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 /qm */ { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 /qm */ { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 /qm */ { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 /qm */ { 0, &BX_CPU_C::RDPMC },
  /* 0F 34 /qm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 35 /qm */ { 0, &BX_CPU_C::BxError },  // SYSENTER/SYSEXIT not recognized in long mode
  /* 0F 36 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 37 /qm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 38 /qm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA4 }, // 3-byte escape
#else
  /* 0F 38 /qm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 39 /qm */ { 0, &BX_CPU_C::BxError },
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* 0F 3A /qm */ { Bx3ByteOpcode | Bx3ByteOpTable, NULL, BxOpcode3ByteTableA5 }, // 3-byte escape
#else
  /* 0F 3A /qm */ { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 3B /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3C /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3D /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3E /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 3F /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 40 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 41 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 42 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 43 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 44 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 45 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 46 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 47 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 48 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 49 /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4A /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4B /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4C /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4D /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4E /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4F /qm */ { 0, &BX_CPU_C::CMOV_GqEq },
  /* 0F 50 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 /qm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 /qm */ { BxGroup12, NULL, BxOpcodeInfoG12 },
  /* 0F 72 /qm */ { BxGroup13, NULL, BxOpcodeInfoG13 },
  /* 0F 73 /qm */ { BxGroup14, NULL, BxOpcodeInfoG14 },
  /* 0F 74 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 /qm */ { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 79 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7A /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7B /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F 7C /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7c }, 
  /* 0F 7D /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7d }, 
  /* 0F 7E /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JO_Jq },
  /* 0F 81 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNO_Jq },
  /* 0F 82 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JB_Jq },
  /* 0F 83 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNB_Jq },
  /* 0F 84 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JZ_Jq },
  /* 0F 85 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNZ_Jq },
  /* 0F 86 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JBE_Jq },
  /* 0F 87 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNBE_Jq },
  /* 0F 88 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JS_Jq },
  /* 0F 89 /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNS_Jq },
  /* 0F 8A /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JP_Jq },
  /* 0F 8B /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNP_Jq },
  /* 0F 8C /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JL_Jq },
  /* 0F 8D /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNL_Jq },
  /* 0F 8E /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JLE_Jq },
  /* 0F 8F /qm */ { BxImmediate_BrOff32, &BX_CPU_C::JNLE_Jq },
  /* 0F 90 /qm */ { 0, &BX_CPU_C::SETO_Eb },
  /* 0F 91 /qm */ { 0, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 /qm */ { 0, &BX_CPU_C::SETB_Eb },
  /* 0F 93 /qm */ { 0, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 /qm */ { 0, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 /qm */ { 0, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 /qm */ { 0, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 /qm */ { 0, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 /qm */ { 0, &BX_CPU_C::SETS_Eb },
  /* 0F 99 /qm */ { 0, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A /qm */ { 0, &BX_CPU_C::SETP_Eb },
  /* 0F 9B /qm */ { 0, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C /qm */ { 0, &BX_CPU_C::SETL_Eb },
  /* 0F 9D /qm */ { 0, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E /qm */ { 0, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F /qm */ { 0, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 /qm */ { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 /qm */ { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 /qm */ { 0, &BX_CPU_C::CPUID },
  /* 0F A3 /qm */ { 0, &BX_CPU_C::BT_EqGqM },
  /* 0F A4 /qm */ { BxImmediate_Ib, &BX_CPU_C::SHLD_EqGq },
  /* 0F A5 /qm */ { 0,              &BX_CPU_C::SHLD_EqGq },
  /* 0F A6 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F A7 /qm */ { 0, &BX_CPU_C::BxError },
  /* 0F A8 /qm */ { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 /qm */ { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA /qm */ { 0, &BX_CPU_C::RSM },
  /* 0F AB /qm */ { BxLockable, &BX_CPU_C::BTS_EqGqM },
  /* 0F AC /qm */ { BxImmediate_Ib, &BX_CPU_C::SHRD_EqGq },
  /* 0F AD /qm */ { 0,              &BX_CPU_C::SHRD_EqGq },
  /* 0F AE /qm */ { BxGroup15, NULL, BxOpcodeInfoG15M },
  /* 0F AF /qm */ { 0, &BX_CPU_C::IMUL_GqEq },
  /* 0F B0 /qm */ { BxLockable, &BX_CPU_C::CMPXCHG_EbGbM },
  /* 0F B1 /qm */ { BxLockable, &BX_CPU_C::CMPXCHG_EqGqM },
  /* 0F B2 /qm */ { 0, &BX_CPU_C::LSS_GqMp }, // TODO: LSS_GdMp for AMD CPU
  /* 0F B3 /qm */ { BxLockable, &BX_CPU_C::BTR_EqGqM },
  /* 0F B4 /qm */ { 0, &BX_CPU_C::LFS_GqMp }, // TODO: LFS_GdMp for AMD CPU
  /* 0F B5 /qm */ { 0, &BX_CPU_C::LGS_GqMp }, // TODO: LGS_GdMp for AMD CPU
  /* 0F B6 /qm */ { 0, &BX_CPU_C::MOVZX_GqEbM },
  /* 0F B7 /qm */ { 0, &BX_CPU_C::MOVZX_GqEwM },
  /* 0F B8 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fb8q },
  /* 0F B9 /qm */ { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F BA /qm */ { BxGroup8, NULL, BxOpcodeInfo64G8EqIb },
  /* 0F BB /qm */ { BxLockable, &BX_CPU_C::BTC_EqGqM },
  /* 0F BC /qm */ { 0, &BX_CPU_C::BSF_GqEq },
  /* 0F BD /qm */ { 0, &BX_CPU_C::BSR_GqEq },
  /* 0F BE /qm */ { 0, &BX_CPU_C::MOVSX_GqEbM },
  /* 0F BF /qm */ { 0, &BX_CPU_C::MOVSX_GqEwM },
  /* 0F C0 /qm */ { BxLockable, &BX_CPU_C::XADD_EbGbM },
  /* 0F C1 /qm */ { BxLockable, &BX_CPU_C::XADD_EqGqM },
  /* 0F C2 /qm */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3qM },
  /* 0F C4 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 /qm */ { BxGroup9, NULL, BxOpcodeInfo64G9qM },
  /* 0F C8 /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F C9 /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CA /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CB /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CC /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CD /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CE /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F CF /qm */ { 0, &BX_CPU_C::BSWAP_RRX },
  /* 0F D0 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd0 },
  /* 0F D1 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7M },
  /* 0F E8 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff0M }, 
  /* 0F F1 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE /qm */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF /qm */ { 0, &BX_CPU_C::BxError }
};

  unsigned
BX_CPU_C::fetchDecode64(Bit8u *iptr, bxInstruction_c *instruction, unsigned remain)
{
  // remain must be at least 1

  unsigned b1, b2, ilen=0, attr, lock=0;
  unsigned imm_mode, offset, rex_r = 0, rex_x = 0, rex_b = 0;
  unsigned rm = 0, mod = 0, nnn = 0;
#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F2   2
#define SSE_PREFIX_F3   3
  unsigned sse_prefix = SSE_PREFIX_NONE;
  unsigned rex_prefix = 0;

  offset = 512*1;

  instruction->ResolveModrm = BxResolveDummy;
  instruction->initMetaInfo(/*os32*/ 1,  // operand size 32 override defaults to 1
                            /*as32*/ 1,  // address size 32 override defaults to 1
                            /*os64*/ 0,  // operand size 64 override defaults to 0
                            /*as64*/ 1); // address size 64 override defaults to 1

fetch_b1:
  b1 = *iptr++;
  ilen++;
  attr = BxOpcodeInfo64R[b1+offset].Attr;

  if (attr & BxPrefix) {
    BX_INSTR_PREFIX(BX_CPU_ID, b1);
    rex_prefix = 0;
    switch (b1) {
      case 0x66: // OpSize
        if(!sse_prefix) sse_prefix = SSE_PREFIX_66;
        if (!instruction->os64L()) {
          instruction->setOs32B(0);
          offset = 0;
        }
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0x67: // AddrSize
        instruction->setAs64B(0);
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
      case 0x48:
      case 0x49:
      case 0x4A:
      case 0x4B:
      case 0x4C:
      case 0x4D:
      case 0x4E:
      case 0x4F:
        rex_prefix = b1;
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0xf2: // REPNE/REPNZ
        sse_prefix = SSE_PREFIX_F2;
        instruction->setRepUsed(b1 & 3);
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0xf3: // REP/REPE/REPZ
        sse_prefix = SSE_PREFIX_F3;
        instruction->setRepUsed(b1 & 3);
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0x2e: // CS:
      case 0x26: // ES:
      case 0x36: // SS:
      case 0x3e: // DS:
        /* ignore segment override prefix */
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0x64: // FS:
        instruction->setSeg(BX_SEG_REG_FS);
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0x65: // GS:
        instruction->setSeg(BX_SEG_REG_GS);
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      case 0xf0: // LOCK:
        lock = 1;
        if (ilen < remain) {
          goto fetch_b1;
        }
        return(0);
      default:
        BX_PANIC(("fetchdecode64: prefix default = 0x%02x", b1));
        return(0);
    }
  }

  if (rex_prefix) {
    instruction->assertExtend8bit();
    if (rex_prefix & 0x8) {
      instruction->assertOs64();
      instruction->assertOs32();
      offset = 512*2;
    }
    rex_r = ((rex_prefix & 0x4) << 1);
    rex_x = ((rex_prefix & 0x2) << 2);
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  // handle 2-byte escape
  if (b1 == 0x0f) {
    if (ilen < remain) {
      ilen++;
      b1 = 0x100 | *iptr++;
    }
    else
      return(0);
  }

  attr = BxOpcodeInfo64R[b1+offset].Attr;

  if (BxOpcodeHasModrm64[b1]) {

#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
    unsigned b3 = 0;
    // handle 3-byte escape
    if (attr & Bx3ByteOpcode) {
      if (ilen < remain) {
        ilen++;
        b3 = *iptr++;
      }
      else
        return(0);
    }
#endif

    // opcode requires modrm byte
    if (ilen < remain) {
      ilen++;
      b2 = *iptr++;
    }
    else
      return(0);

    // Parse mod-nnn-rm and related bytes
    mod = b2 & 0xc0;
    nnn = ((b2 >> 3) & 0x07) | rex_r;
    rm  = (b2 & 0x07) | rex_b;

    // MOVs with CRx and DRx always use register ops and ignore the mod field.
    if ((b1 & ~3) == 0x120)
      mod = 0xc0;

    instruction->modRMForm.modRMData1 = rm;
    instruction->modRMForm.modRMData2 = mod;
    instruction->modRMForm.modRMData3 = 0;
    instruction->modRMForm.modRMData4 = nnn;
    instruction->modRMForm.displ32u = 0;

    if (mod == 0xc0) { // mod == 11b
      instruction->assertModC0();
      goto modrm_done;
    }

    if (instruction->as64L()) {
      // 64-bit addressing modes; note that mod==11b handled above
      if ((rm & 0x7) != 4) { // no s-i-b byte
        if (mod == 0x00) { // mod == 00b
          instruction->ResolveModrm = Resolve64[rm];
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_SEG_REG_DS);
          if ((rm & 0x7) == 5) {
            if ((ilen+3) < remain) {
              instruction->modRMForm.displ32u = FetchDWORD(iptr);
              iptr += 4;
              ilen += 4;
              goto modrm_done;
            }
            else return(0);
          }
          // mod==00b, rm!=4, rm!=5
          goto modrm_done;
        }
        if (mod == 0x40) { // mod == 01b
          instruction->ResolveModrm = BxResolve64Rm;
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod01or10_rm32[rm]);
          if (ilen < remain) {
            // 8 sign extended to 32
            instruction->modRMForm.displ32u = (Bit8s) *iptr++;
            ilen++;
            goto modrm_done;
          }
          else return(0);
        }
        // (mod == 0x80) mod == 10b
        instruction->ResolveModrm = BxResolve64Rm;
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(sreg_mod01or10_rm32[rm]);
        if ((ilen+3) < remain) {
          instruction->modRMForm.displ32u = FetchDWORD(iptr);
          iptr += 4;
          ilen += 4;
          goto modrm_done;
        }
        else return(0);
      }
      else { // mod!=11b, rm==4, s-i-b byte follows
        unsigned sib, base, index, scale;
        if (ilen < remain) {
          sib = *iptr++;
          ilen++;
        }
        else {
          return(0);
        }
        base  = (sib & 0x07) | rex_b; sib >>= 3;
        index = (sib & 0x07) | rex_x; sib >>= 3;
        scale =  sib;
        instruction->modRMForm.modRMData3 |= (base);
        instruction->modRMForm.modRMData2 |= (index);
        instruction->modRMForm.modRMData2 |= (scale<<4);
        if (mod == 0x00) { // mod==00b, rm==4
          if (index == 4)
            instruction->ResolveModrm = Resolve64Base[base];
          else
            instruction->ResolveModrm = Resolve64BaseIndex[base];
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod0_base32[base]);
          if ((base & 0x7) == 5)
            goto get_32bit_displ;
          // mod==00b, rm==4, base!=5
          goto modrm_done;
        }
        if (mod == 0x40) { // mod==01b, rm==4
          if (index == 4)
            instruction->ResolveModrm = BxResolve64Base;
          else 
            instruction->ResolveModrm = BxResolve64BaseIndex;
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod1or2_base32[base]);
          goto get_8bit_displ;
        }
        // (mod == 0x80),  mod==10b, rm==4
        if (index == 4)
          instruction->ResolveModrm = BxResolve64Base;
        else 
          instruction->ResolveModrm = BxResolve64BaseIndex;
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(sreg_mod1or2_base32[base]);
        goto get_32bit_displ;
      }
    }
    else {
      // 32-bit addressing modes; note that mod==11b handled above
      if ((rm & 0x7) != 4) { // no s-i-b byte
        if (mod == 0x00) { // mod == 00b
          instruction->ResolveModrm = Resolve32Rm[rm];
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_SEG_REG_DS);
          if ((rm & 0x7) == 5) {
            if ((ilen+3) < remain) {
              instruction->modRMForm.displ32u = FetchDWORD(iptr);
              iptr += 4;
              ilen += 4;
              goto modrm_done;
            }
            else return(0);
          }
          // mod==00b, rm!=4, rm!=5
          goto modrm_done;
        }
        if (mod == 0x40) { // mod == 01b
          instruction->ResolveModrm = BxResolve32Rm;
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod01or10_rm32[rm]);
get_8bit_displ:
          if (ilen < remain) {
            // 8 sign extended to 32
            instruction->modRMForm.displ32u = (Bit8s) *iptr++;
            ilen++;
            goto modrm_done;
          }
          else return(0);
        }
        // (mod == 0x80) mod == 10b
        instruction->ResolveModrm = BxResolve32Rm;
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(sreg_mod01or10_rm32[rm]);
get_32bit_displ:
        if ((ilen+3) < remain) {
          instruction->modRMForm.displ32u = FetchDWORD(iptr);
          iptr += 4;
          ilen += 4;
          goto modrm_done;
        }
        else return(0);
      }
      else { // mod!=11b, rm==4, s-i-b byte follows
        unsigned sib, base, index, scale;
        if (ilen < remain) {
          sib = *iptr++;
          ilen++;
        }
        else {
          return(0);
        }
        base  = (sib & 0x07) | rex_b; sib >>= 3;
        index = (sib & 0x07) | rex_x; sib >>= 3;
        scale =  sib;
        instruction->modRMForm.modRMData3 |= (base);
        instruction->modRMForm.modRMData2 |= (index);
        instruction->modRMForm.modRMData2 |= (scale<<4);
        if (mod == 0x00) { // mod==00b, rm==4
          if (index == 4)
            instruction->ResolveModrm = Resolve32Base[base];
          else
            instruction->ResolveModrm = Resolve32BaseIndex[base];
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod0_base32[base]);
          if ((base & 0x7) == 5)
            goto get_32bit_displ;
          // mod==00b, rm==4, base!=5
          goto modrm_done;
        }
        if (mod == 0x40) { // mod==01b, rm==4
          if (index == 4)
            instruction->ResolveModrm = BxResolve32Base;
          else
            instruction->ResolveModrm = BxResolve32BaseIndex;
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(sreg_mod1or2_base32[base]);
          goto get_8bit_displ;
        }
        // (mod == 0x80),  mod==10b, rm==4
        if (index == 4)
          instruction->ResolveModrm = BxResolve32Base;
        else
          instruction->ResolveModrm = BxResolve32BaseIndex;
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(sreg_mod1or2_base32[base]);
        goto get_32bit_displ;
      }
    }

modrm_done:

    /*
    BX_DEBUG (("as_64=%d os_64=%d as_32=%d os_32=%d b1=%04x b2=%04x ofs=%4d rm=%d mod=%d nnn=%d",
                     instruction->as64L(),
                     instruction->os64L(),
                     instruction->as32L(),
                     instruction->os32L(),
                     b1,b2,offset,
                     instruction->modRMForm.rm,
                     mod,
                     nnn));
    */

    // Resolve ExecutePtr and additional opcode Attr
    const BxOpcodeInfo_t *OpcodeInfoPtr;
    if (mod == 0xc0) {
      OpcodeInfoPtr = &(BxOpcodeInfo64R[b1+offset]);
      attr = BxOpcodeInfo64R[b1+offset].Attr;
    }
    else {
      OpcodeInfoPtr = &(BxOpcodeInfo64M[b1+offset]);
      attr = BxOpcodeInfo64M[b1+offset].Attr;
    }

    while(attr & BxGroupX) 
    {
       Bit32u Group = attr & BxGroupX;
       attr &= ~BxGroupX;
      
       switch(Group) {
         case BxGroupN:
             OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn & 0x7]);
             break;
         case BxRMGroup:
             OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[rm  & 0x7]);
             break;
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
         case Bx3ByteOpTable:
             OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[b3 >> 4]);
             break;
         case Bx3ByteOpIndex:
             OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[b3 & 15]);
             break;
#endif
         case BxPrefixSSE:
             /* For SSE opcodes, look into another 4 entries table 
                      with the opcode prefixes (NONE, 0x66, 0xF2, 0xF3) */
             OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix]);
             break;
         case BxFPGroup:
             if (mod != 0xc0)  // mod != 11b
                OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn & 0x7]);
             else
             {
                int index = (b1-0xD8)*64 + (0x3f & b2);
                OpcodeInfoPtr = &(BxOpcodeInfo_FloatingPoint[index]);
             }
             break;
         default:
             BX_PANIC(("fetchdecode: Unknown opcode group"));
       }

       /* get additional attributes from group table */
       attr |= OpcodeInfoPtr->Attr;
    }

    instruction->execute = OpcodeInfoPtr->ExecutePtr;
  }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.
    instruction->execute = BxOpcodeInfo64R[b1+offset].ExecutePtr;
    instruction->IxForm.opcodeReg = (b1 & 7) | rex_b;
  }

  if (lock) { // lock prefix invalid opcode
    // lock prefix not allowed or destination operand is not memory
    // only memory destination is marked BxLockable
    if (/*(mod == 0xc0) || */ !(attr & BxLockable)) {
      BX_INFO(("LOCK prefix unallowed (op1=0x%x, mod=%u, nnn=%u)", b1, mod, nnn));
      UndefinedOpcode(instruction);
    }
  }

  imm_mode = attr & BxImmediate;
  if (imm_mode) {
    switch (imm_mode) {
      case BxImmediate_Ib:
        if (ilen < remain) {
          instruction->modRMForm.Ib = *iptr;
          ilen++;
        }
        else {
          return(0);
        }
        break;
      case BxImmediate_Ib_SE: // Sign extend to OS size
        if (ilen < remain) {
          Bit8s temp8s = *iptr;
          if (instruction->os32L())
            instruction->modRMForm.Id = (Bit32s) temp8s;
          else
            instruction->modRMForm.Iw = (Bit16s) temp8s;
          ilen++;
        }
        else {
          return(0);
        }
        break;
      case BxImmediate_Iv:
        if (instruction->os32L()) {
          if ((ilen+3) < remain) {
            instruction->modRMForm.Id = FetchDWORD(iptr);
            ilen += 4;
          }
          else return(0);
        }
        else {
          if ((ilen+1) < remain) {
            instruction->modRMForm.Iw = FetchWORD(iptr);
            ilen += 2;
          }
          else return(0);
        }
        break;
      case BxImmediate_Iq: // MOV Rx,imm64
        if ((ilen+7) < remain) {
          instruction->IqForm.Iq = FetchQWORD(iptr);
          ilen += 8;
        }
        else {
          return(0);
        }
        break;
      case BxImmediate_O:
        // For instructions which embed the address in the opcode.  Note
        // there is only 64/32-bit addressing available in long-mode.
        if (instruction->as64L()) {
          if ((ilen+7) < remain) {
            instruction->IqForm.Iq = FetchQWORD(iptr);
            ilen += 8;
          }
          else return(0);
        }
        else {
          if ((ilen+3) < remain) {
            instruction->IqForm.Iq = (Bit64u) FetchDWORD(iptr);
            ilen += 4;
          }
          else return(0);
        }
        break;
      case BxImmediate_Iw:
      case BxImmediate_IwIb:
        if ((ilen+1) < remain) {
          instruction->modRMForm.Iw = FetchWORD(iptr);
          iptr += 2;
          ilen += 2;
        }
        else return(0);
        if (imm_mode == BxImmediate_Iw) break;
        if (ilen < remain) {
          instruction->IxIxForm.Ib2 = *iptr;
          ilen++;
        }
        else {
          return(0);
        }
        break;
      case BxImmediate_BrOff8:
        if (ilen < remain) {
          Bit8s temp8s = *iptr;
          instruction->modRMForm.Id = temp8s;
          ilen++;
        }
        else {
          return(0);
        }
        break;
      case BxImmediate_BrOff32:
        if ((ilen+3) < remain) {
          instruction->modRMForm.Id = (Bit32s) FetchDWORD(iptr);
          ilen += 4;
        }
        else {
          return(0);
        }
        break;
      default:
        BX_INFO(("b1 was %x", b1));
        BX_PANIC(("fetchdecode: imm_mode = %u", imm_mode));
    }
  }

#if BX_SUPPORT_3DNOW
  if(b1 == 0x10f)
     instruction->execute = Bx3DNowOpcodeInfo[instruction->modRMForm.Ib].ExecutePtr;
#endif

  if (BX_NULL_SEG_REG(instruction->seg()))
     instruction->setSeg(BX_SEG_REG_DS);

  instruction->setB1(b1);
  instruction->setILen(ilen);
  return(1);
}

#endif /* if BX_SUPPORT_X86_64 */
