/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2017  The Bochs Project
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

#include "bochs.h"
#ifndef BX_STANDALONE_DECODER
#include "../cpu.h"
#endif

#include "decoder.h"
#include "instr.h"

#define LOG_THIS genlog->

///////////////////////////
// prefix bytes
// opcode bytes
// modrm/sib
// address displacement
// immediate constant
///////////////////////////

// common fetchdecode32/64 opcode tables
#include "fetchdecode.h"
#include "fetchdecode_opmap.h"
#include "fetchdecode_opmap_0f38.h"
#include "fetchdecode_opmap_0f3a.h"

extern int decoder32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_simple32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_creg32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_modrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_sse(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_sse_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_vex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_evex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_xop32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_ud32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_lzcnt_tzcnt32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group_nnn_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_sse_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group7(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group9(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group15(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder32_group17a(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);

typedef int (*BxFetchDecode32Ptr)(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table);

struct BxOpcodeDecodeDescriptor32 {
  BxFetchDecode32Ptr decode_method;
  const BxOpcodeInfo_t *opcode_table;
};

// table of all Bochs opcodes
bxIAOpcodeTable BxOpcodesTable[] = {
#ifndef BX_STANDALONE_DECODER
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) { b, c, { s1, s2, s3, s4 }, e },
#else
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) {       { s1, s2, s3, s4 }, e },
#endif
#include "ia_opcodes.def"
};
#undef  bx_define_opcode

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

static BxOpcodeDecodeDescriptor32 decode32_descriptor[] =
{
   /*       00 */ { &decoder_modrm32, BxOpcodeTable00 },
   /*       01 */ { &decoder_modrm32, BxOpcodeTable01 },
   /*       02 */ { &decoder_modrm32, BxOpcodeTable02 },
   /*       03 */ { &decoder_modrm32, BxOpcodeTable03 },
   /*       04 */ { &decoder32, BxOpcodeTable04 },
   /*       05 */ { &decoder32, BxOpcodeTable05 },
   /*       06 */ { &decoder32, BxOpcodeTable06_32 },
   /*       07 */ { &decoder32, BxOpcodeTable07_32 },
   /*       08 */ { &decoder_modrm32, BxOpcodeTable08 },
   /*       09 */ { &decoder_modrm32, BxOpcodeTable09 },
   /*       0A */ { &decoder_modrm32, BxOpcodeTable0A },
   /*       0B */ { &decoder_modrm32, BxOpcodeTable0B },
   /*       0C */ { &decoder32, BxOpcodeTable0C },
   /*       0D */ { &decoder32, BxOpcodeTable0D },
   /*       0E */ { &decoder32, BxOpcodeTable0E_32 },
   /*       0F */ { &decoder_ud32, NULL },           // 2-byte escape
   /*       10 */ { &decoder_modrm32, BxOpcodeTable10 },
   /*       11 */ { &decoder_modrm32, BxOpcodeTable11 },
   /*       12 */ { &decoder_modrm32, BxOpcodeTable12 },
   /*       13 */ { &decoder_modrm32, BxOpcodeTable13 },
   /*       14 */ { &decoder32, BxOpcodeTable14 },
   /*       15 */ { &decoder32, BxOpcodeTable15 },
   /*       16 */ { &decoder32, BxOpcodeTable16_32 },
   /*       17 */ { &decoder32, BxOpcodeTable17_32 },
   /*       18 */ { &decoder_modrm32, BxOpcodeTable18 },
   /*       19 */ { &decoder_modrm32, BxOpcodeTable19 },
   /*       1A */ { &decoder_modrm32, BxOpcodeTable1A },
   /*       1B */ { &decoder_modrm32, BxOpcodeTable1B },
   /*       1C */ { &decoder32, BxOpcodeTable1C },
   /*       1D */ { &decoder32, BxOpcodeTable1D },
   /*       1E */ { &decoder32, BxOpcodeTable1E_32 },
   /*       1F */ { &decoder32, BxOpcodeTable1F_32 },
   /*       20 */ { &decoder_modrm32, BxOpcodeTable20 },
   /*       21 */ { &decoder_modrm32, BxOpcodeTable21 },
   /*       22 */ { &decoder_modrm32, BxOpcodeTable22 },
   /*       23 */ { &decoder_modrm32, BxOpcodeTable23 },
   /*       24 */ { &decoder32, BxOpcodeTable24 },
   /*       25 */ { &decoder32, BxOpcodeTable25 },
   /*       26 */ { &decoder_ud32, NULL },           // ES:
   /*       27 */ { &decoder_simple32, BxOpcodeTable27_32 },
   /*       28 */ { &decoder_modrm32, BxOpcodeTable28 },
   /*       29 */ { &decoder_modrm32, BxOpcodeTable29 },
   /*       2A */ { &decoder_modrm32, BxOpcodeTable2A },
   /*       2B */ { &decoder_modrm32, BxOpcodeTable2B },
   /*       2C */ { &decoder32, BxOpcodeTable2C },
   /*       2D */ { &decoder32, BxOpcodeTable2D },
   /*       2E */ { &decoder_ud32, NULL },           // CS:
   /*       2F */ { &decoder_simple32, BxOpcodeTable2F_32 },
   /*       30 */ { &decoder_modrm32, BxOpcodeTable30 },
   /*       31 */ { &decoder_modrm32, BxOpcodeTable31 },
   /*       32 */ { &decoder_modrm32, BxOpcodeTable32 },
   /*       33 */ { &decoder_modrm32, BxOpcodeTable33 },
   /*       34 */ { &decoder32, BxOpcodeTable34 },
   /*       35 */ { &decoder32, BxOpcodeTable35 },
   /*       36 */ { &decoder_ud32, NULL },           // SS:
   /*       37 */ { &decoder_simple32, BxOpcodeTable37_32 },
   /*       38 */ { &decoder_modrm32, BxOpcodeTable38 },
   /*       39 */ { &decoder_modrm32, BxOpcodeTable39 },
   /*       3A */ { &decoder_modrm32, BxOpcodeTable3A },
   /*       3B */ { &decoder_modrm32, BxOpcodeTable3B },
   /*       3C */ { &decoder32, BxOpcodeTable3C },
   /*       3D */ { &decoder32, BxOpcodeTable3D },
   /*       3E */ { &decoder_ud32, NULL },           // DS:
   /*       3F */ { &decoder_simple32, BxOpcodeTable3F_32 },
   /*       40 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       41 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       42 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       43 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       44 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       45 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       46 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       47 */ { &decoder32, BxOpcodeTable40x47_32 },
   /*       48 */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       49 */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4A */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4B */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4C */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4D */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4E */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       4F */ { &decoder32, BxOpcodeTable48x4F_32 },
   /*       50 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       51 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       52 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       53 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       54 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       55 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       56 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       57 */ { &decoder32, BxOpcodeTable50x57_32 },
   /*       58 */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       59 */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5A */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5B */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5C */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5D */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5E */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       5F */ { &decoder32, BxOpcodeTable58x5F_32 },
   /*       60 */ { &decoder32, BxOpcodeTable60_32 },
   /*       61 */ { &decoder32, BxOpcodeTable61_32 },
   /*       62 */ { &decoder_evex32, BxOpcodeTable62_32 },     // EVEX prefix
   /*       63 */ { &decoder_modrm32, BxOpcodeTable63_32 },
   /*       64 */ { &decoder_ud32, NULL },                     // FS:
   /*       65 */ { &decoder_ud32, NULL },                     // GS:
   /*       66 */ { &decoder_ud32, NULL },                     // OSIZE:
   /*       67 */ { &decoder_ud32, NULL },                     // ASIZE:
   /*       68 */ { &decoder32, BxOpcodeTable68_32 },
   /*       69 */ { &decoder_modrm32, BxOpcodeTable69 },
   /*       6A */ { &decoder32, BxOpcodeTable6A_32 },
   /*       6B */ { &decoder_modrm32, BxOpcodeTable6B },
   /*       6C */ { &decoder32, BxOpcodeTable6C },
   /*       6D */ { &decoder32, BxOpcodeTable6D },
   /*       6E */ { &decoder32, BxOpcodeTable6E },
   /*       6F */ { &decoder32, BxOpcodeTable6F },
   /*       70 */ { &decoder32, BxOpcodeTable70_32 },
   /*       71 */ { &decoder32, BxOpcodeTable71_32 },
   /*       72 */ { &decoder32, BxOpcodeTable72_32 },
   /*       73 */ { &decoder32, BxOpcodeTable73_32 },
   /*       74 */ { &decoder32, BxOpcodeTable74_32 },
   /*       75 */ { &decoder32, BxOpcodeTable75_32 },
   /*       76 */ { &decoder32, BxOpcodeTable76_32 },
   /*       77 */ { &decoder32, BxOpcodeTable77_32 },
   /*       78 */ { &decoder32, BxOpcodeTable78_32 },
   /*       79 */ { &decoder32, BxOpcodeTable79_32 },
   /*       7A */ { &decoder32, BxOpcodeTable7A_32 },
   /*       7B */ { &decoder32, BxOpcodeTable7B_32 },
   /*       7C */ { &decoder32, BxOpcodeTable7C_32 },
   /*       7D */ { &decoder32, BxOpcodeTable7D_32 },
   /*       7E */ { &decoder32, BxOpcodeTable7E_32 },
   /*       7F */ { &decoder32, BxOpcodeTable7F_32 },
   /*       80 */ { &decoder32_group_nnn,       BxOpcodeTable80_G1Eb },
   /*       81 */ { &decoder32_group_nnn_osize, BxOpcodeTable81_G1Ev },
   /*       82 */ { &decoder32_group_nnn,       BxOpcodeTable80_G1Eb },  // opcode 0x82 is copy of the 0x80
   /*       83 */ { &decoder32_group_nnn_osize, BxOpcodeTable83_G1Ev },
   /*       84 */ { &decoder_modrm32, BxOpcodeTable84 },
   /*       85 */ { &decoder_modrm32, BxOpcodeTable85 },
   /*       86 */ { &decoder_modrm32, BxOpcodeTable86 },
   /*       87 */ { &decoder_modrm32, BxOpcodeTable87 },
   /*       88 */ { &decoder_modrm32, BxOpcodeTable88 },
   /*       89 */ { &decoder_modrm32, BxOpcodeTable89_32 },
   /*       8A */ { &decoder_modrm32, BxOpcodeTable8A },
   /*       8B */ { &decoder_modrm32, BxOpcodeTable8B_32 },
   /*       8C */ { &decoder_modrm32, BxOpcodeTable8C },
   /*       8D */ { &decoder_modrm32, BxOpcodeTable8D },
   /*       8E */ { &decoder_modrm32, BxOpcodeTable8E },
   /*       8F */ { &decoder_xop32, BxOpcodeTable8F },      // XOP prefix
   /*       90 */ { &decoder32_nop, NULL },
   /*       91 */ { &decoder32, BxOpcodeTable90x97 },
   /*       92 */ { &decoder32, BxOpcodeTable90x97 },
   /*       93 */ { &decoder32, BxOpcodeTable90x97 },
   /*       94 */ { &decoder32, BxOpcodeTable90x97 },
   /*       95 */ { &decoder32, BxOpcodeTable90x97 },
   /*       96 */ { &decoder32, BxOpcodeTable90x97 },
   /*       97 */ { &decoder32, BxOpcodeTable90x97 },
   /*       98 */ { &decoder32, BxOpcodeTable98 },
   /*       99 */ { &decoder32, BxOpcodeTable99 },
   /*       9A */ { &decoder32, BxOpcodeTable9A_32 },
   /*       9B */ { &decoder_simple32, BxOpcodeTable9B },
   /*       9C */ { &decoder32, BxOpcodeTable9C_32 },
   /*       9D */ { &decoder32, BxOpcodeTable9D_32 },
   /*       9E */ { &decoder_simple32, BxOpcodeTable9E_32 },
   /*       9F */ { &decoder_simple32, BxOpcodeTable9F_32 },
   /*       A0 */ { &decoder32, BxOpcodeTableA0_32 },
   /*       A1 */ { &decoder32, BxOpcodeTableA1_32 },
   /*       A2 */ { &decoder32, BxOpcodeTableA2_32 },
   /*       A3 */ { &decoder32, BxOpcodeTableA3_32 },
   /*       A4 */ { &decoder32, BxOpcodeTableA4 },
   /*       A5 */ { &decoder32, BxOpcodeTableA5 },
   /*       A6 */ { &decoder32, BxOpcodeTableA6 },
   /*       A7 */ { &decoder32, BxOpcodeTableA7 },
   /*       A8 */ { &decoder32, BxOpcodeTableA8 },
   /*       A9 */ { &decoder32, BxOpcodeTableA9 },
   /*       AA */ { &decoder32, BxOpcodeTableAA },
   /*       AB */ { &decoder32, BxOpcodeTableAB },
   /*       AC */ { &decoder32, BxOpcodeTableAC },
   /*       AD */ { &decoder32, BxOpcodeTableAD },
   /*       AE */ { &decoder32, BxOpcodeTableAE },
   /*       AF */ { &decoder32, BxOpcodeTableAF },
   /*       B0 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B1 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B2 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B3 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B4 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B5 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B6 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B7 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*       B8 */ { &decoder32, BxOpcodeTableB8xBF },
   /*       B9 */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BA */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BB */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BC */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BD */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BE */ { &decoder32, BxOpcodeTableB8xBF },
   /*       BF */ { &decoder32, BxOpcodeTableB8xBF },
   /*       C0 */ { &decoder32_group_nnn,       BxOpcodeTableC0_G2EbIb },
   /*       C1 */ { &decoder32_group_nnn_osize, BxOpcodeTableC1_G2EvIb },
   /*       C2 */ { &decoder32, BxOpcodeTableC2_32 },
   /*       C3 */ { &decoder32, BxOpcodeTableC3_32 },
   /*       C4 */ { &decoder_vex32, BxOpcodeTableC4_32 },        // VEX prefix
   /*       C5 */ { &decoder_vex32, BxOpcodeTableC5_32 },        // VEX prefix
   /*       C6 */ { &decoder32_group_nnn,       BxOpcodeTableC6_G11EbIb },
   /*       C7 */ { &decoder32_group_nnn_osize, BxOpcodeTableC7_G11EvIv },
   /*       C8 */ { &decoder32, BxOpcodeTableC8_32 },
   /*       C9 */ { &decoder32, BxOpcodeTableC9_32 },
   /*       CA */ { &decoder32, BxOpcodeTableCA },
   /*       CB */ { &decoder32, BxOpcodeTableCB },
   /*       CC */ { &decoder_simple32, BxOpcodeTableCC },
   /*       CD */ { &decoder32, BxOpcodeTableCD },
   /*       CE */ { &decoder_simple32, BxOpcodeTableCE_32 },
   /*       CF */ { &decoder32, BxOpcodeTableCF_32 },
   /*       D0 */ { &decoder32_group_nnn,       BxOpcodeTableD0_G2EbI1 },
   /*       D1 */ { &decoder32_group_nnn_osize, BxOpcodeTableD1_G2EvI1 },
   /*       D2 */ { &decoder32_group_nnn,       BxOpcodeTableD2_G2Eb },
   /*       D3 */ { &decoder32_group_nnn_osize, BxOpcodeTableD3_G2Ev },
   /*       D4 */ { &decoder32, BxOpcodeTableD4_32 },
   /*       D5 */ { &decoder32, BxOpcodeTableD5_32 },
   /*       D6 */ { &decoder_simple32, BxOpcodeTableD6_32 },
   /*       D7 */ { &decoder_simple32, BxOpcodeTableD7 },
   /*       D8 */ { &decoder32_fp_escape, NULL },
   /*       D9 */ { &decoder32_fp_escape, NULL },
   /*       DA */ { &decoder32_fp_escape, NULL },
   /*       DB */ { &decoder32_fp_escape, NULL },
   /*       DC */ { &decoder32_fp_escape, NULL },
   /*       DD */ { &decoder32_fp_escape, NULL },
   /*       DE */ { &decoder32_fp_escape, NULL },
   /*       DF */ { &decoder32_fp_escape, NULL },
   /*       E0 */ { &decoder32, BxOpcodeTableE0_32 },
   /*       E1 */ { &decoder32, BxOpcodeTableE1_32 },
   /*       E2 */ { &decoder32, BxOpcodeTableE2_32 },
   /*       E3 */ { &decoder32, BxOpcodeTableE3_32 },
   /*       E4 */ { &decoder32, BxOpcodeTableE4 },
   /*       E5 */ { &decoder32, BxOpcodeTableE5 },
   /*       E6 */ { &decoder32, BxOpcodeTableE6 },
   /*       E7 */ { &decoder32, BxOpcodeTableE7 },
   /*       E8 */ { &decoder32, BxOpcodeTableE8_32 },
   /*       E9 */ { &decoder32, BxOpcodeTableE9_32 },
   /*       EA */ { &decoder32, BxOpcodeTableEA_32 },
   /*       EB */ { &decoder32, BxOpcodeTableEB_32 },
   /*       EC */ { &decoder32, BxOpcodeTableEC },
   /*       ED */ { &decoder32, BxOpcodeTableED },
   /*       EE */ { &decoder32, BxOpcodeTableEE },
   /*       EF */ { &decoder32, BxOpcodeTableEF },
   /*       F0 */ { &decoder_ud32, NULL },           // LOCK:
   /*       F1 */ { &decoder_simple32, BxOpcodeTableF1 },
   /*       F2 */ { &decoder_ud32, NULL },           // REPNE/REPNZ
   /*       F3 */ { &decoder_ud32, NULL },           // REP, REPE/REPZ
   /*       F4 */ { &decoder_simple32, BxOpcodeTableF4 },
   /*       F5 */ { &decoder_simple32, BxOpcodeTableF5 },
   /*       F6 */ { &decoder32_group_nnn,       BxOpcodeTableF6_G3Eb },
   /*       F7 */ { &decoder32_group_nnn_osize, BxOpcodeTableF7_G3Ev },
   /*       F8 */ { &decoder_simple32, BxOpcodeTableF8 },
   /*       F9 */ { &decoder_simple32, BxOpcodeTableF9 },
   /*       FA */ { &decoder_simple32, BxOpcodeTableFA },
   /*       FB */ { &decoder_simple32, BxOpcodeTableFB },
   /*       FC */ { &decoder_simple32, BxOpcodeTableFC },
   /*       FD */ { &decoder_simple32, BxOpcodeTableFD },
   /*       FE */ { &decoder32_group_nnn, BxOpcodeTableG4 },
   /*       FF */ { &decoder32_group_nnn_osize, BxOpcodeTableFF_32G5v },
   /*    0F 00 */ { &decoder32_group_nnn, BxOpcodeTableG6 },
   /*    0F 01 */ { &decoder32_group7, BxOpcodeTable0F01_32 },
   /*    0F 02 */ { &decoder_modrm32, BxOpcodeTable0F02 },
   /*    0F 03 */ { &decoder_modrm32, BxOpcodeTable0F03 },
   /*    0F 04 */ { &decoder_ud32, NULL },
   /*    0F 05 */ { &decoder_simple32, BxOpcodeTable0F05_32 },
   /*    0F 06 */ { &decoder_simple32, BxOpcodeTable0F06 },
   /*    0F 07 */ { &decoder_simple32, BxOpcodeTable0F07_32 },
   /*    0F 08 */ { &decoder_simple32, BxOpcodeTable0F08 },
   /*    0F 09 */ { &decoder_simple32, BxOpcodeTable0F09 },
   /*    0F 0A */ { &decoder_ud32, NULL },
   /*    0F 0B */ { &decoder_simple32, BxOpcodeTable0F0B },
   /*    0F 0C */ { &decoder_ud32, NULL },
   /*    0F 0D */ { &decoder_modrm32, BxOpcodeTable0F0D },
   /*    0F 0E */ { &decoder_simple32, BxOpcodeTable0F0E },
   /*    0F 0F */ { &decoder_modrm32, BxOpcodeTable0F0F },          // 3dnow! escape
   /*    0F 10 */ { &decoder32_sse, BxOpcodeGroupSSE_0F10 },
   /*    0F 11 */ { &decoder32_sse, BxOpcodeGroupSSE_0F11 },
   /*    0F 12 */ { &decoder32_sse, BxOpcodeGroupSSE_0F12 },
   /*    0F 13 */ { &decoder32_sse, BxOpcodeGroupSSE_0F13 },
   /*    0F 14 */ { &decoder32_sse, BxOpcodeGroupSSE_0F14 },
   /*    0F 15 */ { &decoder32_sse, BxOpcodeGroupSSE_0F15 },
   /*    0F 16 */ { &decoder32_sse, BxOpcodeGroupSSE_0F16 },
   /*    0F 17 */ { &decoder32_sse, BxOpcodeGroupSSE_0F17 },
#if BX_CPU_LEVEL >= 6
   /*    0F 18 */ { &decoder32_group_nnn, BxOpcodeTable0F18_G16 },
   /*    0F 19 */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1A */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1B */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1C */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1D */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1E */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
   /*    0F 1F */ { &decoder_modrm32, BxOpcodeTableMultiByteNOP },
#else
   /*    0F 18 */ { &decoder_ud32, NULL },
   /*    0F 19 */ { &decoder_ud32, NULL },
   /*    0F 1A */ { &decoder_ud32, NULL },
   /*    0F 1B */ { &decoder_ud32, NULL },
   /*    0F 1C */ { &decoder_ud32, NULL },
   /*    0F 1D */ { &decoder_ud32, NULL },
   /*    0F 1E */ { &decoder_ud32, NULL },
   /*    0F 1F */ { &decoder_ud32, NULL },
#endif
   /*    0F 20 */ { &decoder_creg32, BxOpcodeTableMOV_RdCd },
   /*    0F 21 */ { &decoder_creg32, BxOpcodeTable0F21_32 },
   /*    0F 22 */ { &decoder_creg32, BxOpcodeTableMOV_CdRd },
   /*    0F 23 */ { &decoder_creg32, BxOpcodeTable0F23_32 },
   /*    0F 24 */ { &decoder_creg32, BxOpcodeTable0F24_32 },
   /*    0F 25 */ { &decoder_ud32, NULL },
   /*    0F 26 */ { &decoder_creg32, BxOpcodeTable0F26_32 },
   /*    0F 27 */ { &decoder_ud32, NULL },
   /*    0F 28 */ { &decoder32_sse, BxOpcodeGroupSSE_0F28 },
   /*    0F 29 */ { &decoder32_sse, BxOpcodeGroupSSE_0F29 },
   /*    0F 2A */ { &decoder32_sse, BxOpcodeGroupSSE_0F2A },
   /*    0F 2B */ { &decoder32_sse, BxOpcodeGroupSSE_0F2B },
   /*    0F 2C */ { &decoder32_sse, BxOpcodeGroupSSE_0F2C },
   /*    0F 2D */ { &decoder32_sse, BxOpcodeGroupSSE_0F2D },
   /*    0F 2E */ { &decoder32_sse, BxOpcodeGroupSSE_0F2E },
   /*    0F 2F */ { &decoder32_sse, BxOpcodeGroupSSE_0F2F },
   /*    0F 30 */ { &decoder_simple32, BxOpcodeTable0F30 },
   /*    0F 31 */ { &decoder_simple32, BxOpcodeTable0F31 },
   /*    0F 32 */ { &decoder_simple32, BxOpcodeTable0F32 },
   /*    0F 33 */ { &decoder_simple32, BxOpcodeTable0F33 },
   /*    0F 34 */ { &decoder_simple32, BxOpcodeTable0F34 },
   /*    0F 35 */ { &decoder_simple32, BxOpcodeTable0F35 },
   /*    0F 36 */ { &decoder_ud32, NULL },
   /*    0F 37 */ { &decoder_simple32, BxOpcodeTable0F37 },
   /*    0F 38 */ { &decoder_ud32, NULL },          // 3-byte escape
   /*    0F 39 */ { &decoder_ud32, NULL },
   /*    0F 3A */ { &decoder_ud32, NULL },          // 3-byte escape
   /*    0F 3B */ { &decoder_ud32, NULL },
   /*    0F 3C */ { &decoder_ud32, NULL },
   /*    0F 3D */ { &decoder_ud32, NULL },
   /*    0F 3E */ { &decoder_ud32, NULL },
   /*    0F 3F */ { &decoder_ud32, NULL },
   /*    0F 40 */ { &decoder_modrm32, BxOpcodeTable0F40 },
   /*    0F 41 */ { &decoder_modrm32, BxOpcodeTable0F41 },
   /*    0F 42 */ { &decoder_modrm32, BxOpcodeTable0F42 },
   /*    0F 43 */ { &decoder_modrm32, BxOpcodeTable0F43 },
   /*    0F 44 */ { &decoder_modrm32, BxOpcodeTable0F44 },
   /*    0F 45 */ { &decoder_modrm32, BxOpcodeTable0F45 },
   /*    0F 46 */ { &decoder_modrm32, BxOpcodeTable0F46 },
   /*    0F 47 */ { &decoder_modrm32, BxOpcodeTable0F47 },
   /*    0F 48 */ { &decoder_modrm32, BxOpcodeTable0F48 },
   /*    0F 49 */ { &decoder_modrm32, BxOpcodeTable0F49 },
   /*    0F 4A */ { &decoder_modrm32, BxOpcodeTable0F4A },
   /*    0F 4B */ { &decoder_modrm32, BxOpcodeTable0F4B },
   /*    0F 4C */ { &decoder_modrm32, BxOpcodeTable0F4C },
   /*    0F 4D */ { &decoder_modrm32, BxOpcodeTable0F4D },
   /*    0F 4E */ { &decoder_modrm32, BxOpcodeTable0F4E },
   /*    0F 4F */ { &decoder_modrm32, BxOpcodeTable0F4F },
   /*    0F 50 */ { &decoder32_sse, BxOpcodeGroupSSE_0F50 },
   /*    0F 51 */ { &decoder32_sse, BxOpcodeGroupSSE_0F51 },
   /*    0F 52 */ { &decoder32_sse, BxOpcodeGroupSSE_0F52 },
   /*    0F 53 */ { &decoder32_sse, BxOpcodeGroupSSE_0F53 },
   /*    0F 54 */ { &decoder32_sse, BxOpcodeGroupSSE_0F54 },
   /*    0F 55 */ { &decoder32_sse, BxOpcodeGroupSSE_0F55 },
   /*    0F 56 */ { &decoder32_sse, BxOpcodeGroupSSE_0F56 },
   /*    0F 57 */ { &decoder32_sse, BxOpcodeGroupSSE_0F57 },
   /*    0F 58 */ { &decoder32_sse, BxOpcodeGroupSSE_0F58 },
   /*    0F 59 */ { &decoder32_sse, BxOpcodeGroupSSE_0F59 },
   /*    0F 5A */ { &decoder32_sse, BxOpcodeGroupSSE_0F5A },
   /*    0F 5B */ { &decoder32_sse, BxOpcodeGroupSSE_0F5B },
   /*    0F 5C */ { &decoder32_sse, BxOpcodeGroupSSE_0F5C },
   /*    0F 5D */ { &decoder32_sse, BxOpcodeGroupSSE_0F5D },
   /*    0F 5E */ { &decoder32_sse, BxOpcodeGroupSSE_0F5E },
   /*    0F 5F */ { &decoder32_sse, BxOpcodeGroupSSE_0F5F },
   /*    0F 60 */ { &decoder32_sse, BxOpcodeGroupSSE_0F60 },
   /*    0F 61 */ { &decoder32_sse, BxOpcodeGroupSSE_0F61 },
   /*    0F 62 */ { &decoder32_sse, BxOpcodeGroupSSE_0F62 },
   /*    0F 63 */ { &decoder32_sse, BxOpcodeGroupSSE_0F63 },
   /*    0F 64 */ { &decoder32_sse, BxOpcodeGroupSSE_0F64 },
   /*    0F 65 */ { &decoder32_sse, BxOpcodeGroupSSE_0F65 },
   /*    0F 66 */ { &decoder32_sse, BxOpcodeGroupSSE_0F66 },
   /*    0F 67 */ { &decoder32_sse, BxOpcodeGroupSSE_0F67 },
   /*    0F 68 */ { &decoder32_sse, BxOpcodeGroupSSE_0F68 },
   /*    0F 69 */ { &decoder32_sse, BxOpcodeGroupSSE_0F69 },
   /*    0F 6A */ { &decoder32_sse, BxOpcodeGroupSSE_0F6A },
   /*    0F 6B */ { &decoder32_sse, BxOpcodeGroupSSE_0F6B },
   /*    0F 6C */ { &decoder32_sse, BxOpcodeGroupSSE_0F6C },
   /*    0F 6D */ { &decoder32_sse, BxOpcodeGroupSSE_0F6D },
   /*    0F 6E */ { &decoder32_sse, BxOpcodeGroupSSE_0F6E },
   /*    0F 6F */ { &decoder32_sse, BxOpcodeGroupSSE_0F6F },
   /*    0F 70 */ { &decoder32_sse, BxOpcodeGroupSSE_0F70 },
   /*    0F 71 */ { &decoder32_sse_group_nnn, BxOpcodeTable0F71_G12 },
   /*    0F 72 */ { &decoder32_sse_group_nnn, BxOpcodeTable0F72_G13 },
   /*    0F 73 */ { &decoder32_sse_group_nnn, BxOpcodeTable0F73_G14 },
   /*    0F 74 */ { &decoder32_sse, BxOpcodeGroupSSE_0F74 },
   /*    0F 75 */ { &decoder32_sse, BxOpcodeGroupSSE_0F75 },
   /*    0F 76 */ { &decoder32_sse, BxOpcodeGroupSSE_0F76 },
   /*    0F 77 */ { &decoder32, BxOpcodeTable0F77 },
   /*    0F 78 */ { &decoder32_group17a, BxOpcodeTable0F78 },
   /*    0F 79 */ { &decoder32_sse, BxOpcodeGroupSSE_0F79_32 },
   /*    0F 7A */ { &decoder_ud32, NULL },
   /*    0F 7B */ { &decoder_ud32, NULL },
   /*    0F 7C */ { &decoder32_sse, BxOpcodeGroupSSE_0F7C },
   /*    0F 7D */ { &decoder32_sse, BxOpcodeGroupSSE_0F7D },
   /*    0F 7E */ { &decoder32_sse, BxOpcodeGroupSSE_0F7E },
   /*    0F 7F */ { &decoder32_sse, BxOpcodeGroupSSE_0F7F },
   /*    0F 80 */ { &decoder32, BxOpcodeTable0F80_32 },
   /*    0F 81 */ { &decoder32, BxOpcodeTable0F81_32 },
   /*    0F 82 */ { &decoder32, BxOpcodeTable0F82_32 },
   /*    0F 83 */ { &decoder32, BxOpcodeTable0F83_32 },
   /*    0F 84 */ { &decoder32, BxOpcodeTable0F84_32 },
   /*    0F 85 */ { &decoder32, BxOpcodeTable0F85_32 },
   /*    0F 86 */ { &decoder32, BxOpcodeTable0F86_32 },
   /*    0F 87 */ { &decoder32, BxOpcodeTable0F87_32 },
   /*    0F 88 */ { &decoder32, BxOpcodeTable0F88_32 },
   /*    0F 89 */ { &decoder32, BxOpcodeTable0F89_32 },
   /*    0F 8A */ { &decoder32, BxOpcodeTable0F8A_32 },
   /*    0F 8B */ { &decoder32, BxOpcodeTable0F8B_32 },
   /*    0F 8C */ { &decoder32, BxOpcodeTable0F8C_32 },
   /*    0F 8D */ { &decoder32, BxOpcodeTable0F8D_32 },
   /*    0F 8E */ { &decoder32, BxOpcodeTable0F8E_32 },
   /*    0F 8F */ { &decoder32, BxOpcodeTable0F8F_32 },
   /*    0F 90 */ { &decoder_modrm32, BxOpcodeTable0F90 },
   /*    0F 91 */ { &decoder_modrm32, BxOpcodeTable0F91 },
   /*    0F 92 */ { &decoder_modrm32, BxOpcodeTable0F92 },
   /*    0F 93 */ { &decoder_modrm32, BxOpcodeTable0F93 },
   /*    0F 94 */ { &decoder_modrm32, BxOpcodeTable0F94 },
   /*    0F 95 */ { &decoder_modrm32, BxOpcodeTable0F95 },
   /*    0F 96 */ { &decoder_modrm32, BxOpcodeTable0F96 },
   /*    0F 97 */ { &decoder_modrm32, BxOpcodeTable0F97 },
   /*    0F 98 */ { &decoder_modrm32, BxOpcodeTable0F98 },
   /*    0F 99 */ { &decoder_modrm32, BxOpcodeTable0F99 },
   /*    0F 9A */ { &decoder_modrm32, BxOpcodeTable0F9A },
   /*    0F 9B */ { &decoder_modrm32, BxOpcodeTable0F9B },
   /*    0F 9C */ { &decoder_modrm32, BxOpcodeTable0F9C },
   /*    0F 9D */ { &decoder_modrm32, BxOpcodeTable0F9D },
   /*    0F 9E */ { &decoder_modrm32, BxOpcodeTable0F9E },
   /*    0F 9F */ { &decoder_modrm32, BxOpcodeTable0F9F },
   /*    0F A0 */ { &decoder32, BxOpcodeTable0FA0_32 },
   /*    0F A1 */ { &decoder32, BxOpcodeTable0FA1_32 },
   /*    0F A2 */ { &decoder_simple32, BxOpcodeTable0FA2 },
   /*    0F A3 */ { &decoder_modrm32, BxOpcodeTable0FA3 },
   /*    0F A4 */ { &decoder_modrm32, BxOpcodeTable0FA4 },
   /*    0F A5 */ { &decoder_modrm32, BxOpcodeTable0FA5 },
   /*    0F A6 */ { &decoder_ud32, NULL },
   /*    0F A7 */ { &decoder_ud32, NULL },
   /*    0F A8 */ { &decoder32, BxOpcodeTable0FA8_32 },
   /*    0F A9 */ { &decoder32, BxOpcodeTable0FA9_32 },
   /*    0F AA */ { &decoder_simple32, BxOpcodeTable0FAA },
   /*    0F AB */ { &decoder_modrm32, BxOpcodeTable0FAB },
   /*    0F AC */ { &decoder_modrm32, BxOpcodeTable0FAC },
   /*    0F AD */ { &decoder_modrm32, BxOpcodeTable0FAD },
   /*    0F AE */ { &decoder32_group15, NULL },
   /*    0F AF */ { &decoder_modrm32, BxOpcodeTable0FAF },
   /*    0F B0 */ { &decoder_modrm32, BxOpcodeTable0FB0 },
   /*    0F B1 */ { &decoder_modrm32, BxOpcodeTable0FB1 },
   /*    0F B2 */ { &decoder_modrm32, BxOpcodeTable0FB2 },
   /*    0F B3 */ { &decoder_modrm32, BxOpcodeTable0FB3 },
   /*    0F B4 */ { &decoder_modrm32, BxOpcodeTable0FB4 },
   /*    0F B5 */ { &decoder_modrm32, BxOpcodeTable0FB5 },
   /*    0F B6 */ { &decoder_modrm32, BxOpcodeTable0FB6 },
   /*    0F B7 */ { &decoder_modrm32, BxOpcodeTable0FB7 },
   /*    0F B8 */ { &decoder_modrm32, BxOpcodeTable0FB8 },
   /*    0F B9 */ { &decoder_modrm32, BxOpcodeTable0FB9 },
   /*    0F BA */ { &decoder32_group_nnn_osize, BxOpcodeTable0FBA_G8EvIv },
   /*    0F BB */ { &decoder_modrm32, BxOpcodeTable0FBB },
   /*    0F BC */ { &decoder_lzcnt_tzcnt32, BxOpcodeTable0FBC },
   /*    0F BD */ { &decoder_lzcnt_tzcnt32, BxOpcodeTable0FBD },
   /*    0F BE */ { &decoder_modrm32, BxOpcodeTable0FBE },
   /*    0F BF */ { &decoder_modrm32, BxOpcodeTable0FBF },
   /*    0F C0 */ { &decoder_modrm32, BxOpcodeTable0FC0 },
   /*    0F C1 */ { &decoder_modrm32, BxOpcodeTable0FC1 },
   /*    0F C2 */ { &decoder32_sse, BxOpcodeGroupSSE_0FC2 },
   /*    0F C3 */ { &decoder_modrm32, BxOpcodeTable0FC3_32 },
   /*    0F C4 */ { &decoder32_sse, BxOpcodeGroupSSE_0FC4 },
   /*    0F C5 */ { &decoder32_sse, BxOpcodeGroupSSE_0FC5 },
   /*    0F C6 */ { &decoder32_sse, BxOpcodeGroupSSE_0FC6 },
   /*    0F C7 */ { &decoder32_group9, NULL },
   /*    0F C8 */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F C9 */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CA */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CB */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CC */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CD */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CE */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F CF */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /*    0F D0 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD0 },
   /*    0F D1 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD1 },
   /*    0F D2 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD2 },
   /*    0F D3 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD3 },
   /*    0F D4 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD4 },
   /*    0F D5 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD5 },
   /*    0F D6 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD6 },
   /*    0F D7 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD7 },
   /*    0F D8 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD8 },
   /*    0F D9 */ { &decoder32_sse, BxOpcodeGroupSSE_0FD9 },
   /*    0F DA */ { &decoder32_sse, BxOpcodeGroupSSE_0FDA },
   /*    0F DB */ { &decoder32_sse, BxOpcodeGroupSSE_0FDB },
   /*    0F DC */ { &decoder32_sse, BxOpcodeGroupSSE_0FDC },
   /*    0F DD */ { &decoder32_sse, BxOpcodeGroupSSE_0FDD },
   /*    0F DE */ { &decoder32_sse, BxOpcodeGroupSSE_0FDE },
   /*    0F DF */ { &decoder32_sse, BxOpcodeGroupSSE_0FDF },
   /*    0F E0 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE0 },
   /*    0F E1 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE1 },
   /*    0F E2 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE2 },
   /*    0F E3 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE3 },
   /*    0F E4 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE4 },
   /*    0F E5 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE5 },
   /*    0F E6 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE6 },
   /*    0F E7 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE7 },
   /*    0F E8 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE8 },
   /*    0F E9 */ { &decoder32_sse, BxOpcodeGroupSSE_0FE9 },
   /*    0F EA */ { &decoder32_sse, BxOpcodeGroupSSE_0FEA },
   /*    0F EB */ { &decoder32_sse, BxOpcodeGroupSSE_0FEB },
   /*    0F EC */ { &decoder32_sse, BxOpcodeGroupSSE_0FEC },
   /*    0F ED */ { &decoder32_sse, BxOpcodeGroupSSE_0FED },
   /*    0F EE */ { &decoder32_sse, BxOpcodeGroupSSE_0FEE },
   /*    0F EF */ { &decoder32_sse, BxOpcodeGroupSSE_0FEF },
   /*    0F F0 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF0 },
   /*    0F F1 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF1 },
   /*    0F F2 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF2 },
   /*    0F F3 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF3 },
   /*    0F F4 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF4 },
   /*    0F F5 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF5 },
   /*    0F F6 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF6 },
   /*    0F F7 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF7 },
   /*    0F F8 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF8 },
   /*    0F F9 */ { &decoder32_sse, BxOpcodeGroupSSE_0FF9 },
   /*    0F FA */ { &decoder32_sse, BxOpcodeGroupSSE_0FFA },
   /*    0F FB */ { &decoder32_sse, BxOpcodeGroupSSE_0FFB },
   /*    0F FC */ { &decoder32_sse, BxOpcodeGroupSSE_0FFC },
   /*    0F FD */ { &decoder32_sse, BxOpcodeGroupSSE_0FFD },
   /*    0F FE */ { &decoder32_sse, BxOpcodeGroupSSE_0FFE },
   /*    0F FF */ { &decoder_simple32, BxOpcodeTable0FFF },
#if BX_CPU_LEVEL >= 6
   // 3-byte opcode 0x0F 0x38
   /* 0F 38 00 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3800 },
   /* 0F 38 01 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3801 },
   /* 0F 38 02 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3802 },
   /* 0F 38 03 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3803 },
   /* 0F 38 04 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3804 },
   /* 0F 38 05 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3805 },
   /* 0F 38 06 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3806 },
   /* 0F 38 07 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3807 },
   /* 0F 38 08 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3808 },
   /* 0F 38 09 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3809 },
   /* 0F 38 0A */ { &decoder32_sse, BxOpcodeGroupSSE_0F380A },
   /* 0F 38 0B */ { &decoder32_sse, BxOpcodeGroupSSE_0F380B },
   /* 0F 38 0C */ { &decoder_ud32, NULL },
   /* 0F 38 0D */ { &decoder_ud32, NULL },
   /* 0F 38 0E */ { &decoder_ud32, NULL },
   /* 0F 38 0F */ { &decoder_ud32, NULL },
   /* 0F 38 10 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3810 },
   /* 0F 38 11 */ { &decoder_ud32, NULL },
   /* 0F 38 12 */ { &decoder_ud32, NULL },
   /* 0F 38 13 */ { &decoder_ud32, NULL },
   /* 0F 38 14 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3814 },
   /* 0F 38 15 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3815 },
   /* 0F 38 16 */ { &decoder_ud32, NULL },
   /* 0F 38 17 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3817 },
   /* 0F 38 18 */ { &decoder_ud32, NULL },
   /* 0F 38 19 */ { &decoder_ud32, NULL },
   /* 0F 38 1A */ { &decoder_ud32, NULL },
   /* 0F 38 1B */ { &decoder_ud32, NULL },
   /* 0F 38 1C */ { &decoder32_sse, BxOpcodeGroupSSE_0F381C },
   /* 0F 38 1D */ { &decoder32_sse, BxOpcodeGroupSSE_0F381D },
   /* 0F 38 1E */ { &decoder32_sse, BxOpcodeGroupSSE_0F381E },
   /* 0F 38 1F */ { &decoder_ud32, NULL },
   /* 0F 38 20 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3820 },
   /* 0F 38 21 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3821 },
   /* 0F 38 22 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3822 },
   /* 0F 38 23 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3823 },
   /* 0F 38 24 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3824 },
   /* 0F 38 25 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3825 },
   /* 0F 38 26 */ { &decoder_ud32, NULL },
   /* 0F 38 27 */ { &decoder_ud32, NULL },
   /* 0F 38 28 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3828 },
   /* 0F 38 29 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3829 },
   /* 0F 38 2A */ { &decoder32_sse, BxOpcodeGroupSSE_0F382A },
   /* 0F 38 2B */ { &decoder32_sse, BxOpcodeGroupSSE_0F382B },
   /* 0F 38 2C */ { &decoder_ud32, NULL },
   /* 0F 38 2D */ { &decoder_ud32, NULL },
   /* 0F 38 2E */ { &decoder_ud32, NULL },
   /* 0F 38 2F */ { &decoder_ud32, NULL },
   /* 0F 38 30 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3830 },
   /* 0F 38 31 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3831 },
   /* 0F 38 32 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3832 },
   /* 0F 38 33 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3833 },
   /* 0F 38 34 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3834 },
   /* 0F 38 35 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3835 },
   /* 0F 38 36 */ { &decoder_ud32, NULL },
   /* 0F 38 37 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3837 },
   /* 0F 38 38 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3838 },
   /* 0F 38 39 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3839 },
   /* 0F 38 3A */ { &decoder32_sse, BxOpcodeGroupSSE_0F383A },
   /* 0F 38 3B */ { &decoder32_sse, BxOpcodeGroupSSE_0F383B },
   /* 0F 38 3C */ { &decoder32_sse, BxOpcodeGroupSSE_0F383C },
   /* 0F 38 3D */ { &decoder32_sse, BxOpcodeGroupSSE_0F383D },
   /* 0F 38 3E */ { &decoder32_sse, BxOpcodeGroupSSE_0F383E },
   /* 0F 38 3F */ { &decoder32_sse, BxOpcodeGroupSSE_0F383F },
   /* 0F 38 40 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3840 },
   /* 0F 38 41 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3841 },
   /* 0F 38 42 */ { &decoder_ud32, NULL },
   /* 0F 38 43 */ { &decoder_ud32, NULL },
   /* 0F 38 44 */ { &decoder_ud32, NULL },
   /* 0F 38 45 */ { &decoder_ud32, NULL },
   /* 0F 38 46 */ { &decoder_ud32, NULL },
   /* 0F 38 47 */ { &decoder_ud32, NULL },
   /* 0F 38 48 */ { &decoder_ud32, NULL },
   /* 0F 38 49 */ { &decoder_ud32, NULL },
   /* 0F 38 4A */ { &decoder_ud32, NULL },
   /* 0F 38 4B */ { &decoder_ud32, NULL },
   /* 0F 38 4C */ { &decoder_ud32, NULL },
   /* 0F 38 4D */ { &decoder_ud32, NULL },
   /* 0F 38 4E */ { &decoder_ud32, NULL },
   /* 0F 38 4F */ { &decoder_ud32, NULL },
   /* 0F 38 50 */ { &decoder_ud32, NULL },
   /* 0F 38 51 */ { &decoder_ud32, NULL },
   /* 0F 38 52 */ { &decoder_ud32, NULL },
   /* 0F 38 53 */ { &decoder_ud32, NULL },
   /* 0F 38 54 */ { &decoder_ud32, NULL },
   /* 0F 38 55 */ { &decoder_ud32, NULL },
   /* 0F 38 56 */ { &decoder_ud32, NULL },
   /* 0F 38 57 */ { &decoder_ud32, NULL },
   /* 0F 38 58 */ { &decoder_ud32, NULL },
   /* 0F 38 59 */ { &decoder_ud32, NULL },
   /* 0F 38 5A */ { &decoder_ud32, NULL },
   /* 0F 38 5B */ { &decoder_ud32, NULL },
   /* 0F 38 5C */ { &decoder_ud32, NULL },
   /* 0F 38 5D */ { &decoder_ud32, NULL },
   /* 0F 38 5E */ { &decoder_ud32, NULL },
   /* 0F 38 5F */ { &decoder_ud32, NULL },
   /* 0F 38 60 */ { &decoder_ud32, NULL },
   /* 0F 38 61 */ { &decoder_ud32, NULL },
   /* 0F 38 62 */ { &decoder_ud32, NULL },
   /* 0F 38 63 */ { &decoder_ud32, NULL },
   /* 0F 38 64 */ { &decoder_ud32, NULL },
   /* 0F 38 65 */ { &decoder_ud32, NULL },
   /* 0F 38 66 */ { &decoder_ud32, NULL },
   /* 0F 38 67 */ { &decoder_ud32, NULL },
   /* 0F 38 68 */ { &decoder_ud32, NULL },
   /* 0F 38 69 */ { &decoder_ud32, NULL },
   /* 0F 38 6A */ { &decoder_ud32, NULL },
   /* 0F 38 6B */ { &decoder_ud32, NULL },
   /* 0F 38 6C */ { &decoder_ud32, NULL },
   /* 0F 38 6D */ { &decoder_ud32, NULL },
   /* 0F 38 6E */ { &decoder_ud32, NULL },
   /* 0F 38 6F */ { &decoder_ud32, NULL },
   /* 0F 38 70 */ { &decoder_ud32, NULL },
   /* 0F 38 71 */ { &decoder_ud32, NULL },
   /* 0F 38 72 */ { &decoder_ud32, NULL },
   /* 0F 38 73 */ { &decoder_ud32, NULL },
   /* 0F 38 74 */ { &decoder_ud32, NULL },
   /* 0F 38 75 */ { &decoder_ud32, NULL },
   /* 0F 38 76 */ { &decoder_ud32, NULL },
   /* 0F 38 77 */ { &decoder_ud32, NULL },
   /* 0F 38 78 */ { &decoder_ud32, NULL },
   /* 0F 38 79 */ { &decoder_ud32, NULL },
   /* 0F 38 7A */ { &decoder_ud32, NULL },
   /* 0F 38 7B */ { &decoder_ud32, NULL },
   /* 0F 38 7C */ { &decoder_ud32, NULL },
   /* 0F 38 7D */ { &decoder_ud32, NULL },
   /* 0F 38 7E */ { &decoder_ud32, NULL },
   /* 0F 38 7F */ { &decoder_ud32, NULL },
   /* 0F 38 80 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3880 },
   /* 0F 38 81 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3881 },
   /* 0F 38 82 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3882 },
   /* 0F 38 83 */ { &decoder_ud32, NULL },
   /* 0F 38 84 */ { &decoder_ud32, NULL },
   /* 0F 38 85 */ { &decoder_ud32, NULL },
   /* 0F 38 86 */ { &decoder_ud32, NULL },
   /* 0F 38 87 */ { &decoder_ud32, NULL },
   /* 0F 38 88 */ { &decoder_ud32, NULL },
   /* 0F 38 89 */ { &decoder_ud32, NULL },
   /* 0F 38 8A */ { &decoder_ud32, NULL },
   /* 0F 38 8B */ { &decoder_ud32, NULL },
   /* 0F 38 8C */ { &decoder_ud32, NULL },
   /* 0F 38 8D */ { &decoder_ud32, NULL },
   /* 0F 38 8E */ { &decoder_ud32, NULL },
   /* 0F 38 8F */ { &decoder_ud32, NULL },
   /* 0F 38 90 */ { &decoder_ud32, NULL },
   /* 0F 38 91 */ { &decoder_ud32, NULL },
   /* 0F 38 92 */ { &decoder_ud32, NULL },
   /* 0F 38 93 */ { &decoder_ud32, NULL },
   /* 0F 38 94 */ { &decoder_ud32, NULL },
   /* 0F 38 95 */ { &decoder_ud32, NULL },
   /* 0F 38 96 */ { &decoder_ud32, NULL },
   /* 0F 38 97 */ { &decoder_ud32, NULL },
   /* 0F 38 98 */ { &decoder_ud32, NULL },
   /* 0F 38 99 */ { &decoder_ud32, NULL },
   /* 0F 38 9A */ { &decoder_ud32, NULL },
   /* 0F 38 9B */ { &decoder_ud32, NULL },
   /* 0F 38 9C */ { &decoder_ud32, NULL },
   /* 0F 38 9D */ { &decoder_ud32, NULL },
   /* 0F 38 9E */ { &decoder_ud32, NULL },
   /* 0F 38 9F */ { &decoder_ud32, NULL },
   /* 0F 38 A0 */ { &decoder_ud32, NULL },
   /* 0F 38 A1 */ { &decoder_ud32, NULL },
   /* 0F 38 A2 */ { &decoder_ud32, NULL },
   /* 0F 38 A3 */ { &decoder_ud32, NULL },
   /* 0F 38 A4 */ { &decoder_ud32, NULL },
   /* 0F 38 A5 */ { &decoder_ud32, NULL },
   /* 0F 38 A6 */ { &decoder_ud32, NULL },
   /* 0F 38 A7 */ { &decoder_ud32, NULL },
   /* 0F 38 A8 */ { &decoder_ud32, NULL },
   /* 0F 38 A9 */ { &decoder_ud32, NULL },
   /* 0F 38 AA */ { &decoder_ud32, NULL },
   /* 0F 38 AB */ { &decoder_ud32, NULL },
   /* 0F 38 AC */ { &decoder_ud32, NULL },
   /* 0F 38 AD */ { &decoder_ud32, NULL },
   /* 0F 38 AE */ { &decoder_ud32, NULL },
   /* 0F 38 AF */ { &decoder_ud32, NULL },
   /* 0F 38 B0 */ { &decoder_ud32, NULL },
   /* 0F 38 B1 */ { &decoder_ud32, NULL },
   /* 0F 38 B2 */ { &decoder_ud32, NULL },
   /* 0F 38 B3 */ { &decoder_ud32, NULL },
   /* 0F 38 B4 */ { &decoder_ud32, NULL },
   /* 0F 38 B5 */ { &decoder_ud32, NULL },
   /* 0F 38 B6 */ { &decoder_ud32, NULL },
   /* 0F 38 B7 */ { &decoder_ud32, NULL },
   /* 0F 38 B8 */ { &decoder_ud32, NULL },
   /* 0F 38 B9 */ { &decoder_ud32, NULL },
   /* 0F 38 BA */ { &decoder_ud32, NULL },
   /* 0F 38 BB */ { &decoder_ud32, NULL },
   /* 0F 38 BC */ { &decoder_ud32, NULL },
   /* 0F 38 BD */ { &decoder_ud32, NULL },
   /* 0F 38 BE */ { &decoder_ud32, NULL },
   /* 0F 38 BF */ { &decoder_ud32, NULL },
   /* 0F 38 C0 */ { &decoder_ud32, NULL },
   /* 0F 38 C1 */ { &decoder_ud32, NULL },
   /* 0F 38 C2 */ { &decoder_ud32, NULL },
   /* 0F 38 C3 */ { &decoder_ud32, NULL },
   /* 0F 38 C4 */ { &decoder_ud32, NULL },
   /* 0F 38 C5 */ { &decoder_ud32, NULL },
   /* 0F 38 C6 */ { &decoder_ud32, NULL },
   /* 0F 38 C7 */ { &decoder_ud32, NULL },
   /* 0F 38 C8 */ { &decoder32_sse, BxOpcodeGroupSSE_0F38C8 },
   /* 0F 38 C9 */ { &decoder32_sse, BxOpcodeGroupSSE_0F38C9 },
   /* 0F 38 CA */ { &decoder32_sse, BxOpcodeGroupSSE_0F38CA },
   /* 0F 38 CB */ { &decoder32_sse, BxOpcodeGroupSSE_0F38CB },
   /* 0F 38 CC */ { &decoder32_sse, BxOpcodeGroupSSE_0F38CC },
   /* 0F 38 CD */ { &decoder32_sse, BxOpcodeGroupSSE_0F38CD },
   /* 0F 38 CE */ { &decoder_ud32, NULL },
   /* 0F 38 CF */ { &decoder32_sse, BxOpcodeGroupSSE_0F38CF },
   /* 0F 38 D0 */ { &decoder_ud32, NULL },
   /* 0F 38 D1 */ { &decoder_ud32, NULL },
   /* 0F 38 D2 */ { &decoder_ud32, NULL },
   /* 0F 38 D3 */ { &decoder_ud32, NULL },
   /* 0F 38 D4 */ { &decoder_ud32, NULL },
   /* 0F 38 D5 */ { &decoder_ud32, NULL },
   /* 0F 38 D6 */ { &decoder_ud32, NULL },
   /* 0F 38 D7 */ { &decoder_ud32, NULL },
   /* 0F 38 D8 */ { &decoder_ud32, NULL },
   /* 0F 38 D9 */ { &decoder_ud32, NULL },
   /* 0F 38 DA */ { &decoder_ud32, NULL },
   /* 0F 38 DB */ { &decoder32_sse, BxOpcodeGroupSSE_0F38DB },
   /* 0F 38 DC */ { &decoder32_sse, BxOpcodeGroupSSE_0F38DC },
   /* 0F 38 DD */ { &decoder32_sse, BxOpcodeGroupSSE_0F38DD },
   /* 0F 38 DE */ { &decoder32_sse, BxOpcodeGroupSSE_0F38DE },
   /* 0F 38 DF */ { &decoder32_sse, BxOpcodeGroupSSE_0F38DF },
   /* 0F 38 E0 */ { &decoder_ud32, NULL },
   /* 0F 38 E1 */ { &decoder_ud32, NULL },
   /* 0F 38 E2 */ { &decoder_ud32, NULL },
   /* 0F 38 E3 */ { &decoder_ud32, NULL },
   /* 0F 38 E4 */ { &decoder_ud32, NULL },
   /* 0F 38 E5 */ { &decoder_ud32, NULL },
   /* 0F 38 E6 */ { &decoder_ud32, NULL },
   /* 0F 38 E7 */ { &decoder_ud32, NULL },
   /* 0F 38 E8 */ { &decoder_ud32, NULL },
   /* 0F 38 E9 */ { &decoder_ud32, NULL },
   /* 0F 38 EA */ { &decoder_ud32, NULL },
   /* 0F 38 EB */ { &decoder_ud32, NULL },
   /* 0F 38 EC */ { &decoder_ud32, NULL },
   /* 0F 38 ED */ { &decoder_ud32, NULL },
   /* 0F 38 EE */ { &decoder_ud32, NULL },
   /* 0F 38 EF */ { &decoder_ud32, NULL },
   /* 0F 38 F0 */ { &decoder32_sse_osize, BxOpcodeGroupSSE_0F38F0 },
   /* 0F 38 F1 */ { &decoder32_sse_osize, BxOpcodeGroupSSE_0F38F1 },
   /* 0F 38 F2 */ { &decoder_ud32, NULL },
   /* 0F 38 F3 */ { &decoder_ud32, NULL },
   /* 0F 38 F4 */ { &decoder_ud32, NULL },
   /* 0F 38 F5 */ { &decoder_ud32, NULL },
   /* 0F 38 F6 */ { &decoder32_sse, BxOpcodeGroupSSE_0F38F6 },
   /* 0F 38 F7 */ { &decoder_ud32, NULL },
   /* 0F 38 F8 */ { &decoder_ud32, NULL },
   /* 0F 38 F9 */ { &decoder_ud32, NULL },
   /* 0F 38 FA */ { &decoder_ud32, NULL },
   /* 0F 38 FB */ { &decoder_ud32, NULL },
   /* 0F 38 FC */ { &decoder_ud32, NULL },
   /* 0F 38 FD */ { &decoder_ud32, NULL },
   /* 0F 38 FE */ { &decoder_ud32, NULL },
   /* 0F 38 FF */ { &decoder_ud32, NULL },
   /* 0F 38 00 */ { &decoder_ud32, NULL },

   // 3-byte opcode 0x0F 0x3A
   /* 0F 3A 01 */ { &decoder_ud32, NULL },
   /* 0F 3A 02 */ { &decoder_ud32, NULL },
   /* 0F 3A 03 */ { &decoder_ud32, NULL },
   /* 0F 3A 04 */ { &decoder_ud32, NULL },
   /* 0F 3A 05 */ { &decoder_ud32, NULL },
   /* 0F 3A 06 */ { &decoder_ud32, NULL },
   /* 0F 3A 07 */ { &decoder_ud32, NULL },
   /* 0F 3A 08 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A08 },
   /* 0F 3A 09 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A09 },
   /* 0F 3A 0A */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0A },
   /* 0F 3A 0B */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0B },
   /* 0F 3A 0C */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0C },
   /* 0F 3A 0D */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0D },
   /* 0F 3A 0E */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0E },
   /* 0F 3A 0F */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A0F },
   /* 0F 3A 10 */ { &decoder_ud32, NULL },
   /* 0F 3A 11 */ { &decoder_ud32, NULL },
   /* 0F 3A 12 */ { &decoder_ud32, NULL },
   /* 0F 3A 13 */ { &decoder_ud32, NULL },
   /* 0F 3A 14 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A14 },
   /* 0F 3A 15 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A15 },
   /* 0F 3A 16 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A16 },
   /* 0F 3A 17 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A17 },
   /* 0F 3A 18 */ { &decoder_ud32, NULL },
   /* 0F 3A 19 */ { &decoder_ud32, NULL },
   /* 0F 3A 1A */ { &decoder_ud32, NULL },
   /* 0F 3A 1B */ { &decoder_ud32, NULL },
   /* 0F 3A 1C */ { &decoder_ud32, NULL },
   /* 0F 3A 1D */ { &decoder_ud32, NULL },
   /* 0F 3A 1E */ { &decoder_ud32, NULL },
   /* 0F 3A 1F */ { &decoder_ud32, NULL },
   /* 0F 3A 20 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A20 },
   /* 0F 3A 21 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A21 },
   /* 0F 3A 22 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A22 },
   /* 0F 3A 23 */ { &decoder_ud32, NULL },
   /* 0F 3A 24 */ { &decoder_ud32, NULL },
   /* 0F 3A 25 */ { &decoder_ud32, NULL },
   /* 0F 3A 26 */ { &decoder_ud32, NULL },
   /* 0F 3A 27 */ { &decoder_ud32, NULL },
   /* 0F 3A 28 */ { &decoder_ud32, NULL },
   /* 0F 3A 29 */ { &decoder_ud32, NULL },
   /* 0F 3A 2A */ { &decoder_ud32, NULL },
   /* 0F 3A 2B */ { &decoder_ud32, NULL },
   /* 0F 3A 2C */ { &decoder_ud32, NULL },
   /* 0F 3A 2D */ { &decoder_ud32, NULL },
   /* 0F 3A 2E */ { &decoder_ud32, NULL },
   /* 0F 3A 2F */ { &decoder_ud32, NULL },
   /* 0F 3A 30 */ { &decoder_ud32, NULL },
   /* 0F 3A 31 */ { &decoder_ud32, NULL },
   /* 0F 3A 32 */ { &decoder_ud32, NULL },
   /* 0F 3A 33 */ { &decoder_ud32, NULL },
   /* 0F 3A 34 */ { &decoder_ud32, NULL },
   /* 0F 3A 35 */ { &decoder_ud32, NULL },
   /* 0F 3A 36 */ { &decoder_ud32, NULL },
   /* 0F 3A 37 */ { &decoder_ud32, NULL },
   /* 0F 3A 38 */ { &decoder_ud32, NULL },
   /* 0F 3A 39 */ { &decoder_ud32, NULL },
   /* 0F 3A 3A */ { &decoder_ud32, NULL },
   /* 0F 3A 3B */ { &decoder_ud32, NULL },
   /* 0F 3A 3C */ { &decoder_ud32, NULL },
   /* 0F 3A 3D */ { &decoder_ud32, NULL },
   /* 0F 3A 3E */ { &decoder_ud32, NULL },
   /* 0F 3A 3F */ { &decoder_ud32, NULL },
   /* 0F 3A 40 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A40 },
   /* 0F 3A 41 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A41 },
   /* 0F 3A 42 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A42 },
   /* 0F 3A 43 */ { &decoder_ud32, NULL },
   /* 0F 3A 44 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A44 },
   /* 0F 3A 45 */ { &decoder_ud32, NULL },
   /* 0F 3A 46 */ { &decoder_ud32, NULL },
   /* 0F 3A 47 */ { &decoder_ud32, NULL },
   /* 0F 3A 48 */ { &decoder_ud32, NULL },
   /* 0F 3A 49 */ { &decoder_ud32, NULL },
   /* 0F 3A 4A */ { &decoder_ud32, NULL },
   /* 0F 3A 4B */ { &decoder_ud32, NULL },
   /* 0F 3A 4C */ { &decoder_ud32, NULL },
   /* 0F 3A 4D */ { &decoder_ud32, NULL },
   /* 0F 3A 4E */ { &decoder_ud32, NULL },
   /* 0F 3A 4F */ { &decoder_ud32, NULL },
   /* 0F 3A 50 */ { &decoder_ud32, NULL },
   /* 0F 3A 51 */ { &decoder_ud32, NULL },
   /* 0F 3A 52 */ { &decoder_ud32, NULL },
   /* 0F 3A 53 */ { &decoder_ud32, NULL },
   /* 0F 3A 54 */ { &decoder_ud32, NULL },
   /* 0F 3A 55 */ { &decoder_ud32, NULL },
   /* 0F 3A 56 */ { &decoder_ud32, NULL },
   /* 0F 3A 57 */ { &decoder_ud32, NULL },
   /* 0F 3A 58 */ { &decoder_ud32, NULL },
   /* 0F 3A 59 */ { &decoder_ud32, NULL },
   /* 0F 3A 5A */ { &decoder_ud32, NULL },
   /* 0F 3A 5B */ { &decoder_ud32, NULL },
   /* 0F 3A 5C */ { &decoder_ud32, NULL },
   /* 0F 3A 5D */ { &decoder_ud32, NULL },
   /* 0F 3A 5E */ { &decoder_ud32, NULL },
   /* 0F 3A 5F */ { &decoder_ud32, NULL },
   /* 0F 3A 60 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A60 },
   /* 0F 3A 61 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A61 },
   /* 0F 3A 62 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A62 },
   /* 0F 3A 64 */ { &decoder32_sse, BxOpcodeGroupSSE_0F3A63 },
   /* 0F 3A 64 */ { &decoder_ud32, NULL },
   /* 0F 3A 65 */ { &decoder_ud32, NULL },
   /* 0F 3A 66 */ { &decoder_ud32, NULL },
   /* 0F 3A 67 */ { &decoder_ud32, NULL },
   /* 0F 3A 68 */ { &decoder_ud32, NULL },
   /* 0F 3A 69 */ { &decoder_ud32, NULL },
   /* 0F 3A 6A */ { &decoder_ud32, NULL },
   /* 0F 3A 6B */ { &decoder_ud32, NULL },
   /* 0F 3A 6C */ { &decoder_ud32, NULL },
   /* 0F 3A 6D */ { &decoder_ud32, NULL },
   /* 0F 3A 6E */ { &decoder_ud32, NULL },
   /* 0F 3A 6F */ { &decoder_ud32, NULL },
   /* 0F 3A 70 */ { &decoder_ud32, NULL },
   /* 0F 3A 71 */ { &decoder_ud32, NULL },
   /* 0F 3A 72 */ { &decoder_ud32, NULL },
   /* 0F 3A 73 */ { &decoder_ud32, NULL },
   /* 0F 3A 74 */ { &decoder_ud32, NULL },
   /* 0F 3A 75 */ { &decoder_ud32, NULL },
   /* 0F 3A 76 */ { &decoder_ud32, NULL },
   /* 0F 3A 77 */ { &decoder_ud32, NULL },
   /* 0F 3A 78 */ { &decoder_ud32, NULL },
   /* 0F 3A 79 */ { &decoder_ud32, NULL },
   /* 0F 3A 7A */ { &decoder_ud32, NULL },
   /* 0F 3A 7B */ { &decoder_ud32, NULL },
   /* 0F 3A 7C */ { &decoder_ud32, NULL },
   /* 0F 3A 7D */ { &decoder_ud32, NULL },
   /* 0F 3A 7E */ { &decoder_ud32, NULL },
   /* 0F 3A 7F */ { &decoder_ud32, NULL },
   /* 0F 3A 80 */ { &decoder_ud32, NULL },
   /* 0F 3A 81 */ { &decoder_ud32, NULL },
   /* 0F 3A 82 */ { &decoder_ud32, NULL },
   /* 0F 3A 83 */ { &decoder_ud32, NULL },
   /* 0F 3A 84 */ { &decoder_ud32, NULL },
   /* 0F 3A 85 */ { &decoder_ud32, NULL },
   /* 0F 3A 86 */ { &decoder_ud32, NULL },
   /* 0F 3A 87 */ { &decoder_ud32, NULL },
   /* 0F 3A 88 */ { &decoder_ud32, NULL },
   /* 0F 3A 89 */ { &decoder_ud32, NULL },
   /* 0F 3A 8A */ { &decoder_ud32, NULL },
   /* 0F 3A 8B */ { &decoder_ud32, NULL },
   /* 0F 3A 8C */ { &decoder_ud32, NULL },
   /* 0F 3A 8D */ { &decoder_ud32, NULL },
   /* 0F 3A 8E */ { &decoder_ud32, NULL },
   /* 0F 3A 8F */ { &decoder_ud32, NULL },
   /* 0F 3A 90 */ { &decoder_ud32, NULL },
   /* 0F 3A 91 */ { &decoder_ud32, NULL },
   /* 0F 3A 92 */ { &decoder_ud32, NULL },
   /* 0F 3A 93 */ { &decoder_ud32, NULL },
   /* 0F 3A 94 */ { &decoder_ud32, NULL },
   /* 0F 3A 95 */ { &decoder_ud32, NULL },
   /* 0F 3A 96 */ { &decoder_ud32, NULL },
   /* 0F 3A 97 */ { &decoder_ud32, NULL },
   /* 0F 3A 98 */ { &decoder_ud32, NULL },
   /* 0F 3A 99 */ { &decoder_ud32, NULL },
   /* 0F 3A 9A */ { &decoder_ud32, NULL },
   /* 0F 3A 9B */ { &decoder_ud32, NULL },
   /* 0F 3A 9C */ { &decoder_ud32, NULL },
   /* 0F 3A 9D */ { &decoder_ud32, NULL },
   /* 0F 3A 9E */ { &decoder_ud32, NULL },
   /* 0F 3A 9F */ { &decoder_ud32, NULL },
   /* 0F 3A A0 */ { &decoder_ud32, NULL },
   /* 0F 3A A1 */ { &decoder_ud32, NULL },
   /* 0F 3A A2 */ { &decoder_ud32, NULL },
   /* 0F 3A A3 */ { &decoder_ud32, NULL },
   /* 0F 3A A4 */ { &decoder_ud32, NULL },
   /* 0F 3A A5 */ { &decoder_ud32, NULL },
   /* 0F 3A A6 */ { &decoder_ud32, NULL },
   /* 0F 3A A7 */ { &decoder_ud32, NULL },
   /* 0F 3A A8 */ { &decoder_ud32, NULL },
   /* 0F 3A A9 */ { &decoder_ud32, NULL },
   /* 0F 3A AA */ { &decoder_ud32, NULL },
   /* 0F 3A AB */ { &decoder_ud32, NULL },
   /* 0F 3A AC */ { &decoder_ud32, NULL },
   /* 0F 3A AD */ { &decoder_ud32, NULL },
   /* 0F 3A AE */ { &decoder_ud32, NULL },
   /* 0F 3A AF */ { &decoder_ud32, NULL },
   /* 0F 3A B0 */ { &decoder_ud32, NULL },
   /* 0F 3A B1 */ { &decoder_ud32, NULL },
   /* 0F 3A B2 */ { &decoder_ud32, NULL },
   /* 0F 3A B3 */ { &decoder_ud32, NULL },
   /* 0F 3A B4 */ { &decoder_ud32, NULL },
   /* 0F 3A B5 */ { &decoder_ud32, NULL },
   /* 0F 3A B6 */ { &decoder_ud32, NULL },
   /* 0F 3A B7 */ { &decoder_ud32, NULL },
   /* 0F 3A B8 */ { &decoder_ud32, NULL },
   /* 0F 3A B9 */ { &decoder_ud32, NULL },
   /* 0F 3A BA */ { &decoder_ud32, NULL },
   /* 0F 3A BB */ { &decoder_ud32, NULL },
   /* 0F 3A BC */ { &decoder_ud32, NULL },
   /* 0F 3A BD */ { &decoder_ud32, NULL },
   /* 0F 3A BE */ { &decoder_ud32, NULL },
   /* 0F 3A BF */ { &decoder_ud32, NULL },
   /* 0F 3A C0 */ { &decoder_ud32, NULL },
   /* 0F 3A C1 */ { &decoder_ud32, NULL },
   /* 0F 3A C2 */ { &decoder_ud32, NULL },
   /* 0F 3A C3 */ { &decoder_ud32, NULL },
   /* 0F 3A C4 */ { &decoder_ud32, NULL },
   /* 0F 3A C5 */ { &decoder_ud32, NULL },
   /* 0F 3A C6 */ { &decoder_ud32, NULL },
   /* 0F 3A C7 */ { &decoder_ud32, NULL },
   /* 0F 3A C8 */ { &decoder_ud32, NULL },
   /* 0F 3A C9 */ { &decoder_ud32, NULL },
   /* 0F 3A CA */ { &decoder_ud32, NULL },
   /* 0F 3A CB */ { &decoder_ud32, NULL },
   /* 0F 3A CC */ { &decoder32_sse, BxOpcodeGroupSSE_0F3ACC },
   /* 0F 3A CD */ { &decoder_ud32, NULL },
   /* 0F 3A CE */ { &decoder32_sse, BxOpcodeGroupSSE_0F3ACE },
   /* 0F 3A CF */ { &decoder32_sse, BxOpcodeGroupSSE_0F3ACF },
   /* 0F 3A D0 */ { &decoder_ud32, NULL },
   /* 0F 3A D1 */ { &decoder_ud32, NULL },
   /* 0F 3A D2 */ { &decoder_ud32, NULL },
   /* 0F 3A D3 */ { &decoder_ud32, NULL },
   /* 0F 3A D4 */ { &decoder_ud32, NULL },
   /* 0F 3A D5 */ { &decoder_ud32, NULL },
   /* 0F 3A D6 */ { &decoder_ud32, NULL },
   /* 0F 3A D7 */ { &decoder_ud32, NULL },
   /* 0F 3A D8 */ { &decoder_ud32, NULL },
   /* 0F 3A D9 */ { &decoder_ud32, NULL },
   /* 0F 3A DA */ { &decoder_ud32, NULL },
   /* 0F 3A DB */ { &decoder_ud32, NULL },
   /* 0F 3A DC */ { &decoder_ud32, NULL },
   /* 0F 3A DD */ { &decoder_ud32, NULL },
   /* 0F 3A DE */ { &decoder_ud32, NULL },
   /* 0F 3A DF */ { &decoder32_sse, BxOpcodeGroupSSE_0F3ADF },
   /* 0F 3A E0 */ { &decoder_ud32, NULL },
   /* 0F 3A E1 */ { &decoder_ud32, NULL },
   /* 0F 3A E2 */ { &decoder_ud32, NULL },
   /* 0F 3A E3 */ { &decoder_ud32, NULL },
   /* 0F 3A E4 */ { &decoder_ud32, NULL },
   /* 0F 3A E5 */ { &decoder_ud32, NULL },
   /* 0F 3A E6 */ { &decoder_ud32, NULL },
   /* 0F 3A E7 */ { &decoder_ud32, NULL },
   /* 0F 3A E8 */ { &decoder_ud32, NULL },
   /* 0F 3A E9 */ { &decoder_ud32, NULL },
   /* 0F 3A EA */ { &decoder_ud32, NULL },
   /* 0F 3A EB */ { &decoder_ud32, NULL },
   /* 0F 3A EC */ { &decoder_ud32, NULL },
   /* 0F 3A ED */ { &decoder_ud32, NULL },
   /* 0F 3A EE */ { &decoder_ud32, NULL },
   /* 0F 3A EF */ { &decoder_ud32, NULL },
   /* 0F 3A F0 */ { &decoder_ud32, NULL },
   /* 0F 3A F1 */ { &decoder_ud32, NULL },
   /* 0F 3A F2 */ { &decoder_ud32, NULL },
   /* 0F 3A F3 */ { &decoder_ud32, NULL },
   /* 0F 3A F4 */ { &decoder_ud32, NULL },
   /* 0F 3A F5 */ { &decoder_ud32, NULL },
   /* 0F 3A F6 */ { &decoder_ud32, NULL },
   /* 0F 3A F7 */ { &decoder_ud32, NULL },
   /* 0F 3A F8 */ { &decoder_ud32, NULL },
   /* 0F 3A F9 */ { &decoder_ud32, NULL },
   /* 0F 3A FA */ { &decoder_ud32, NULL },
   /* 0F 3A FB */ { &decoder_ud32, NULL },
   /* 0F 3A FC */ { &decoder_ud32, NULL },
   /* 0F 3A FD */ { &decoder_ud32, NULL },
   /* 0F 3A FE */ { &decoder_ud32, NULL },
   /* 0F 3A FF */ { &decoder_ud32, NULL },
#endif
};

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

extern const Bit8u *decodeModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm);
extern const Bit8u *parseModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, struct bx_modrm *modrm);
extern int decodeImmediate32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned imm_mode, unsigned imm_mode2);
extern Bit16u WalkOpcodeTables(const BxExtOpcodeInfo_t *OpcodeInfoPtr, Bit16u &attr, bx_bool is_64, unsigned modrm, unsigned sse_prefix, unsigned osize, unsigned vex_vl, bx_bool vex_w);

const Bit8u *parseModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, struct bx_modrm *modrm)
{
  // opcode requires modrm byte
  if (remain == 0)
    return NULL;
  remain--;
  unsigned b2 = *iptr++;

  // Keep original modrm byte
  modrm->modrm = b2;

  // Parse mod-nnn-rm and related bytes
  modrm->mod = b2 & 0xc0; // leave unshifted
  modrm->nnn = (b2 >> 3) & 0x7;
  modrm->rm  = b2 & 0x7;

  if (modrm->mod == 0xc0) { // mod == 11b
    i->assertModC0();
  }
  else {
    iptr = decodeModrm32(iptr, remain, i, modrm->mod, modrm->nnn, modrm->rm);
  }

  return iptr;
}

const Bit8u *decodeModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm)
{
  unsigned seg = BX_SEG_REG_DS;

  i->setSibBase(rm);      // initialize with rm to use BxResolve32Base
  i->setSibIndex(4);      // no Index encoding by default
  // initialize displ32 with zero to include cases with no diplacement
  i->modRMForm.displ32u = 0;

  // note that mod==11b handled outside

  if (i->as32L()) {
    if (rm != 4) { // no s-i-b byte
      if (mod == 0x00) { // mod == 00b
        if (rm == 5) {
          i->setSibBase(BX_NIL_REGISTER);
          if (remain > 3) {
            i->modRMForm.displ32u = FetchDWORD(iptr);
            iptr += 4;
            remain -= 4;
          }
          else return NULL;
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
        return NULL;
      }
      base  = sib & 0x7; sib >>= 3;
      index = sib & 0x7; sib >>= 3;
      scale = sib;

      i->setSibScale(scale);
      i->setSibBase(base);
      // this part is a little tricky - assign index value always,
      // it will be really used if the instruction is Gather. Others
      // assume that resolve function will do the right thing.
      i->setSibIndex(index);

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
            return NULL;
          }
        }
        // mod==00b, rm==4, base!=5
        goto modrm_done;
      }
      else {
        seg = sreg_mod1or2_base32[base];
      }
    }

    if (mod == 0x40) { // mod==01b
      if (remain != 0) {
        // 8 sign extended to 32
        i->modRMForm.displ32u = (Bit8s) *iptr++;
        remain--;
      }
      else {
        return NULL;
      }
    }
    else {
      // (mod == 0x80), mod==10b
      if (remain > 3) {
        i->modRMForm.displ32u = FetchDWORD(iptr);
        iptr += 4;
        remain -= 4;
      }
      else {
        return NULL;
      }
    }
  }
  else {
    // 16-bit addressing modes, mod==11b handled above
    i->setSibBase(Resolve16BaseReg[rm]);
    i->setSibIndex(Resolve16IndexReg[rm]);
    i->setSibScale(0);

    if (mod == 0x00) { // mod == 00b
      seg = sreg_mod00_rm16[rm];
      if (rm == 6) {
        i->setSibBase(BX_NIL_REGISTER);
        if (remain > 1) {
          i->modRMForm.displ32u = (Bit32s) (Bit16s) FetchWORD(iptr);
          iptr += 2;
          remain -= 2;
        }
        else {
          return NULL;
        }
      }
      goto modrm_done;
    }
    else {
      seg = sreg_mod01or10_rm16[rm];
    }

    if (mod == 0x40) { // mod == 01b
      if (remain != 0) {
        // 8 sign extended to 16
        i->modRMForm.displ32u = (Bit32s) (Bit8s) *iptr++;
        remain--;
      }
      else {
        return NULL;
      }
    }
    else {
      // (mod == 0x80)      mod == 10b
      if (remain > 1) {
        i->modRMForm.displ32u = (Bit32s) (Bit16s) FetchWORD(iptr);
        iptr += 2;
        remain -= 2;
      }
      else {
        return NULL;
      }
    }
  }

modrm_done:

  i->setSeg(seg);
  return iptr;
}

int decodeImmediate32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned imm_mode, unsigned imm_mode2)
{
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
          Bit8s temp8s = *iptr++;
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
            iptr += 4;
            remain -= 4;
          }
          else return(-1);
        }
        else {
          // fetch 16bit address into Id
          if (remain > 1) {
            i->modRMForm.Id = (Bit32u) FetchWORD(iptr);
            iptr += 2;
            remain -= 2;
          }
          else return(-1);
        }
        break;
      default:
        BX_PANIC(("decoder32: imm_mode = %u", imm_mode));
        break;
    }

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
          BX_PANIC(("decoder32: imm_mode2 = %u", imm_mode2));
          break;
      }
    }
  }

  return 0;
}

#if BX_SUPPORT_EVEX
unsigned evex_displ8_compression(const bxInstruction_c *i, unsigned ia_opcode, unsigned src, unsigned type, unsigned vex_w)
{
  if (src == BX_SRC_RM) {
    switch (type) {
    case BX_GPR64:
      return 8;
    case BX_GPR32:
      return 4;
    case BX_GPR16:
      return 2;
    }

    return 1;
  }

  if (ia_opcode == BX_IA_V512_VMOVDDUP_VpdWpd && i->getVL() == BX_VL128)
    return 8;

  unsigned len = i->getVL();
  if (len == BX_NO_VL) len = BX_VL128;

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

  case BX_VMM_SCALAR_DWORD:
    return 4;

  case BX_VMM_SCALAR_QWORD:
    return 8;

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

bx_bool assign_srcs(bxInstruction_c *i, unsigned ia_opcode, unsigned nnn, unsigned rm)
{
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = BX_DISASM_SRC_TYPE(src);
    unsigned index = BX_DISASM_SRC_ORIGIN(src);
    switch(index) {
    case BX_SRC_NONE:
    case BX_SRC_IMM:
    case BX_SRC_IMPLICIT:
      break;
    case BX_SRC_EAX:
      i->setSrcReg(n, 0);
      break;
    case BX_SRC_NNN:
      i->setSrcReg(n, nnn);
      break;
    case BX_SRC_RM:
    case BX_SRC_VECTOR_RM:
      if (i->modC0()) {
        i->setSrcReg(n, rm);
      }
      else {
        unsigned tmpreg = BX_TMP_REGISTER;
#if BX_SUPPORT_FPU
        if (type == BX_VMM_REG) tmpreg = BX_VECTOR_TMP_REGISTER;
#endif
        i->setSrcReg(n, tmpreg);
      }
      break;
    default:
      BX_FATAL(("assign_srcs: unknown definition %d for src %d", src, n));
      break;
    }
  }

  return BX_TRUE;
}

#if BX_SUPPORT_AVX
bx_bool assign_srcs(bxInstruction_c *i, unsigned ia_opcode, bx_bool is_64, unsigned nnn, unsigned rm, unsigned vvv, unsigned vex_w, bx_bool had_evex = BX_FALSE, bx_bool displ8 = BX_FALSE)
{
  bx_bool use_vvv = BX_FALSE;
#if BX_SUPPORT_EVEX
  unsigned displ8_scale = 1;
#endif

  // assign sources
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = BX_DISASM_SRC_TYPE(src);
    src = BX_DISASM_SRC_ORIGIN(src);
#if BX_SUPPORT_EVEX
    bx_bool mem_src = BX_FALSE;
#endif

    switch(src) {
    case BX_SRC_NONE:
    case BX_SRC_IMM:
    case BX_SRC_IMPLICIT:
      break;
    case BX_SRC_EAX:
      i->setSrcReg(n, 0);
      break;
    case BX_SRC_NNN:
      i->setSrcReg(n, nnn);
      if (type == BX_KMASK_REG) {
        if (nnn >= 8) return BX_FALSE;
      }
      break;
    case BX_SRC_RM:
      if (i->modC0()) {
        if (type == BX_KMASK_REG)
          rm &= 0x7;

        i->setSrcReg(n, rm);
      }
      else {
#if BX_SUPPORT_EVEX
        mem_src = BX_TRUE;
#endif
        i->setSrcReg(n, (type == BX_VMM_REG) ? BX_VECTOR_TMP_REGISTER : BX_TMP_REGISTER);
      }
      break;
    case BX_SRC_VECTOR_RM:
      if (i->modC0()) {
        i->setSrcReg(n, rm);
      }
      else {
        i->setSrcReg(n, BX_VECTOR_TMP_REGISTER);
#if BX_SUPPORT_EVEX
        mem_src = BX_TRUE;
        if (n == 0) // zero masking is not allowed for memory destination
          if (i->isZeroMasking()) return BX_FALSE;
#endif
      }
      break;
    case BX_SRC_VVV:
      i->setSrcReg(n, vvv);
      use_vvv = BX_TRUE;
      if (type == BX_KMASK_REG) {
        if (vvv >= 8) return BX_FALSE;
      }
      break;
    case BX_SRC_VIB:
      if (is_64) {
#if BX_SUPPORT_EVEX
        if (had_evex)
          i->setSrcReg(n, ((i->Ib() << 1) & 0x10) | (i->Ib() >> 4));
        else
#endif
          i->setSrcReg(n, (i->Ib() >> 4));
      }
      else {
        i->setSrcReg(n, (i->Ib() >> 4) & 7);
      }
      break;
    case BX_SRC_VSIB:
      if (! i->as32L() || i->sibIndex() == BX_NIL_REGISTER) {
        return BX_FALSE;
      }
#if BX_SUPPORT_EVEX
      i->setSibIndex(i->sibIndex() | (vvv & 0x10));
      // zero masking is not allowed for gather/scatter
      if (i->isZeroMasking()) return BX_FALSE;
      mem_src = BX_TRUE;
#endif
      break;
    default:
      BX_FATAL(("assign_srcs: unknown definition %d for src %d", src, n));
      break;
    }

#if BX_SUPPORT_EVEX
    if (had_evex && displ8 && mem_src) {
      displ8_scale = evex_displ8_compression(i, ia_opcode, src, type, vex_w);
    }
#endif
  }

#if BX_SUPPORT_EVEX
  if (displ8_scale > 1) {
    if (i->as32L())
      i->modRMForm.displ32u *= displ8_scale;
    else
      i->modRMForm.displ16u *= displ8_scale;
  }
#endif

  if (! use_vvv && vvv != 0) {
    return BX_FALSE;
  }

  return BX_TRUE;
}
#endif

int decoder_vex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure VEX 0xC4 or VEX 0xC5
  assert((b1 & ~0x1) == 0xc4);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc0) != 0xc0) {
    return decoder_modrm32(iptr, remain, i, b1, sse_prefix, opcode_table);
  }

#if BX_SUPPORT_AVX
  unsigned rm = 0, mod = 0, nnn = 0;

  if (sse_prefix)
    return(BX_IA_ERROR);

  bx_bool vex_w = 0;
  unsigned vex_opcext = 1;
  unsigned vex = *iptr++;
  remain--;

  if (b1 == 0xc4) {
    // decode 3-byte VEX prefix
    vex_opcext = vex & 0x1f;
    if (remain == 0)
      return(-1);
    remain--;
    vex = *iptr++;  // fetch VEX3
    vex_w = (vex >> 7) & 0x1;
  }

  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  i->setVexW(vex_w);
  sse_prefix = vex & 0x3;

  if (remain == 0)
    return(-1);
  remain--;

  unsigned opcode_byte = *iptr++;
  opcode_byte += 256 * vex_opcext;
  if (opcode_byte < 256 || opcode_byte >= 1024)
    return(ia_opcode);
  bx_bool has_modrm = (opcode_byte != 0x177); // if not VZEROUPPER/VZEROALL opcode

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &BxOpcodeTableAVX[(opcode_byte-256)*2 + vex_l];
  Bit16u attr = OpcodeInfoPtr->Attr;

  if (has_modrm) {
    // opcode requires modrm byte
    if (remain == 0)
      return(-1);
    remain--;
    unsigned b2 = *iptr++;

    // Parse mod-nnn-rm and related bytes
    mod = b2 & 0xc0; // leave unshifted
    nnn = (b2 >> 3) & 0x7;
    rm  = b2 & 0x7;

    if (mod == 0xc0) { // mod == 11b
      i->assertModC0();
    }
    else {
      iptr = decodeModrm32(iptr, remain, i, mod, nnn, rm);
      if (! iptr) 
        return(-1);
    }

    ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_FALSE, b2, sse_prefix, i->osize(), i->getVL(), vex_w);
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

  unsigned imm_mode = attr & BxImmediate;
  if (imm_mode) {
    if (BxImmediate_Ib == imm_mode) {
      if (remain != 0) {
        i->modRMForm.Ib[0] = *iptr;
        remain--;
      }
      else {
        return(-1);
      }
    }
    else {
      BX_PANIC(("fetchdecode: VEX with imm_mode = %u", imm_mode));
    }
  }

  if (! assign_srcs(i, ia_opcode, BX_FALSE, nnn, rm, vvv, vex_w))
    ia_opcode = BX_IA_ERROR;

  // invalid opcode sanity checks
  if ((attr & BxVexW0) != 0 && vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexW1) != 0 && !vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL0) != 0 && i->getVL() != BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL1) != 0 && i->getVL() == BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
#endif

  return ia_opcode;
}

int decoder_evex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure EVEX 0x62 prefix
  assert(b1 == 0x62);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc0) != 0xc0) {
    return decoder_modrm32(iptr, remain, i, b1, sse_prefix, opcode_table);
  }

#if BX_SUPPORT_EVEX
  bx_bool displ8 = BX_FALSE;

  if (sse_prefix)
    return(BX_IA_ERROR);

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
    return(ia_opcode);

  unsigned evex_opcext = evex & 0x3;
  if (evex_opcext == 0)
    return(ia_opcode);

  sse_prefix = (evex >> 8) & 0x3;
  int vvv = 15 - ((evex >> 11) & 0xf);
  if (vvv >= 8)
    return(ia_opcode);

  bx_bool vex_w = (evex >> 15) & 0x1;
  unsigned opmask = (evex >> 16) & 0x7;
  i->setOpmask(opmask);
  unsigned evex_v = ((evex >> 19) & 0x1) ^ 0x1;
  if (evex_v)
    return(ia_opcode);
  unsigned evex_b = (evex >> 20) & 0x1;
  i->setEvexb(evex_b);

  unsigned evex_vl_rc = (evex >> 21) & 0x3;
  i->setRC(evex_vl_rc);
  i->setVL(1 << evex_vl_rc);
  i->setVexW(vex_w);

  unsigned evex_z = (evex >> 23) & 0x1;
  i->setZeroMasking(evex_z);

  if (evex_z && ! opmask)
    return(ia_opcode);
    
  unsigned opcode_byte = (evex >> 24);
  opcode_byte += 256 * (evex_opcext-1);

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &BxOpcodeTableEVEX[opcode_byte*2 + (opmask != 0)];
  Bit16u attr = OpcodeInfoPtr->Attr;

  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  if (modrm.mod == 0x40) { // mod==01b
    displ8 = BX_TRUE;
  }

  // EVEX.b in reg form implies 512-bit vector length
  if (modrm.mod == 0xc0 && i->getEvexb()) {
    i->setVL(BX_VL512);
  }

  ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_FALSE, modrm.modrm, sse_prefix, i->osize(), i->getVL(), vex_w);

  unsigned imm_mode = attr & BxImmediate;
  if (imm_mode) {
    if (BxImmediate_Ib == imm_mode) {
      if (remain != 0) {
        i->modRMForm.Ib[0] = *iptr;
        remain--;
      }
      else {
        return(-1);
      }
    }
    else {
      BX_PANIC(("fetchdecode: EVEX with imm_mode = %u", imm_mode));
    }
  }

  if (! assign_srcs(i, ia_opcode, BX_FALSE, modrm.nnn, modrm.rm, vvv, vex_w, BX_TRUE, displ8))
    ia_opcode = BX_IA_ERROR;

  // invalid opcode sanity checks
  if ((attr & BxVexW0) != 0 && vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexW1) != 0 && !vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  // EVEX specific #UD conditions
  else if (i->getVL() > BX_VL512) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL0) != 0 && i->getVL() != BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL1) != 0 && i->getVL() == BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
#endif

  return ia_opcode;
}

int decoder_xop32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure XOP 0x8f prefix
  assert(b1 == 0x8f);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc8) != 0xc8) {
    // not XOP prefix, decode regular opcode
    struct bx_modrm modrm;
    iptr = parseModrm32(iptr, remain, i, &modrm);
    if (! iptr) 
      return(-1);

    if (modrm.nnn != 0)
      return BX_IA_ERROR;

    const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[i->osize()]);
    ia_opcode = OpcodeInfoPtr->IA;

    assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

    return ia_opcode;
  }

#if BX_SUPPORT_AVX
  // 3 byte XOP prefix
  if (sse_prefix)
    return(ia_opcode);

  unsigned vex;
  if (remain > 2) {
    remain -= 3;
    vex = *iptr++; // fetch XOP2
  }
  else
    return(-1);

  unsigned xop_opcext = (vex & 0x1f) - 8;
  if (xop_opcext >= 3)
    return(ia_opcode);

  vex = *iptr++; // fetch XOP3

  bx_bool vex_w = (vex >> 7) & 0x1;
  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  i->setVexW(vex_w);
  sse_prefix = vex & 0x3;
  if (sse_prefix)
    return(ia_opcode);

  unsigned opcode_byte = *iptr++;
  opcode_byte += 256 * xop_opcext;

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &BxOpcodeTableXOP[opcode_byte];
  Bit16u attr = OpcodeInfoPtr->Attr;

  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_FALSE, modrm.modrm, sse_prefix, i->osize(), i->getVL(), vex_w);

  unsigned imm_mode = attr & BxImmediate;
  if (imm_mode) {
    switch (imm_mode) {
      case BxImmediate_Ib:
        if (remain != 0) {
          i->modRMForm.Ib[0] = *iptr;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_Id:
        if (remain > 3) {
          i->modRMForm.Id = FetchDWORD(iptr);
          remain -= 4;
        }
        else {
          return(-1);
        }
        break;
      default:
        BX_PANIC(("fetchdecode: XOP with imm_mode = %u", imm_mode));
        break;
    }
  }

  if (! assign_srcs(i, ia_opcode, BX_FALSE, modrm.nnn, modrm.rm, vvv, vex_w))
    ia_opcode = BX_IA_ERROR;

  // invalid opcode sanity checks
  if ((attr & BxVexW0) != 0 && vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexW1) != 0 && !vex_w) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL0) != 0 && i->getVL() != BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
  else if ((attr & BxVexL1) != 0 && i->getVL() == BX_VL128) {
    ia_opcode = BX_IA_ERROR;
  }
#endif

  return ia_opcode;
}

int decoder32_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
#if BX_SUPPORT_FPU == 0
  return BX_IA_FPUESC;
#else
  int ia_opcode = BX_IA_ERROR;

  assert(b1 >= 0xd8 && b1 <= 0xdf);

  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  i->setFoo((modrm.modrm | (b1 << 8)) & 0x7ff); /* for x87 */

  const Bit16u *x87_opmap[8] = {
    /* D8 */ BxOpcodeInfo_FloatingPointD8,
    /* D9 */ BxOpcodeInfo_FloatingPointD9,
    /* DA */ BxOpcodeInfo_FloatingPointDA,
    /* DB */ BxOpcodeInfo_FloatingPointDB,
    /* DC */ BxOpcodeInfo_FloatingPointDC,
    /* DD */ BxOpcodeInfo_FloatingPointDD,
    /* DE */ BxOpcodeInfo_FloatingPointDE,
    /* DF */ BxOpcodeInfo_FloatingPointDF
  };

  Bit16u *opcodes = (Bit16u *) x87_opmap[b1 - 0xd8];
  if (modrm.mod != 0xc0)
    ia_opcode = opcodes[modrm.nnn];
  else
    ia_opcode = opcodes[(modrm.modrm & 0x3f) + 8];

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
#endif
}

int decoder32_sse(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[sse_prefix]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate32(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32_sse_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  unsigned osize = i->osize();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize * 4 + sse_prefix]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate32(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[modrm.nnn]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate32(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32_group_nnn_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  unsigned osize = i->osize();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[modrm.nnn + osize * 8]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate32(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32_sse_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[sse_prefix + modrm.nnn * 4]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate32(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F 01 opcode - G7 group
int decoder32_group7(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr;
  if (modrm.mod != 0xc0) {
    // mem form
    OpcodeInfoPtr = &(opcode_table[modrm.nnn]);
  }
  else {
    // reg form - special opcode table
    OpcodeInfoPtr = &(BxOpcodeTable0F01[modrm.modrm & 0x3f]);
  }

  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  if (attr == BxNoPrefixSSE && sse_prefix)
    ia_opcode = BX_IA_ERROR;
#if BX_SUPPORT_X86_64
  // SWAPGS is valid only in 64-bit mode
  if (ia_opcode == BX_IA_SWAPGS)
    ia_opcode = BX_IA_ERROR;
#endif

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F C7 opcode - G9 group
int decoder32_group9(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();

  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  static const BxExtOpcodeInfo_t BxOpcodeTable0FC7[2] = {
    /* 0F C7 /w */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9w },
    /* 0F C7 /d */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9d }
  };

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &(BxOpcodeTable0FC7[osize]);
  Bit16u attr = OpcodeInfoPtr->Attr;
  int ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_FALSE, modrm.modrm, sse_prefix, osize, i->getVL(), 0 /* vex_w */);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F AE opcode - G15 group
int decoder32_group15(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr;
  if (modrm.mod == 0xc0) {
    OpcodeInfoPtr = &(BxOpcodeTable0FAE_G15_R32[modrm.nnn]);
  }
  else {
    OpcodeInfoPtr = &(BxOpcodeTable0FAE_G15M[modrm.nnn * 4 + sse_prefix]);
  }
  int ia_opcode = OpcodeInfoPtr->IA;

  Bit16u attr = OpcodeInfoPtr->Attr;
  if (attr == BxNoPrefixSSE && sse_prefix)
    ia_opcode = BX_IA_ERROR;

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F 78 opcode (SSE4A)
int decoder32_group17a(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  int ia_opcode = BX_IA_ERROR;
  if (sse_prefix) {
    // handle SSE4A extension, decode manually
    switch (sse_prefix) {
    case SSE_PREFIX_66:
      if (modrm.nnn == 0) ia_opcode = BX_IA_EXTRQ_UdqIbIb;
      break;
    case SSE_PREFIX_F3:
      ia_opcode = BX_IA_ERROR;
      break;
    case SSE_PREFIX_F2:
      ia_opcode = BX_IA_INSERTQ_VdqUqIbIb;
      break;
    default:
      break;
    }

    if (decodeImmediate32(iptr, remain, i, BxImmediate_Ib, BxImmediate_Ib2) < 0)
      return (-1);
  }
  else {
    const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[0]);
    ia_opcode = OpcodeInfoPtr->IA;
  }

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder_creg32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  // opcode requires modrm byte
  if (remain == 0)
    return(-1);
  remain--;
  unsigned b2 = *iptr++;

  // MOVs with CRx and DRx always use register ops and ignore the mod field.
  assert((b1 & ~7) == 0x120);

  // Parse mod-nnn-rm and related bytes
  unsigned nnn = (b2 >> 3) & 0x7;
  unsigned rm  = b2 & 0x7;

  i->assertModC0();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &opcode_table[0];
  if (b1 == 0x120 || b1 == 0x122)
     OpcodeInfoPtr = &(opcode_table[nnn]);
  int ia_opcode = OpcodeInfoPtr->IA;

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder_lzcnt_tzcnt32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();

  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize]);
  if (sse_prefix == SSE_PREFIX_F3) {
    if (b1 == 0x1bc)
      OpcodeInfoPtr = &(BxOpcodeTable0FBC_TZCNT[osize]);
    else
      OpcodeInfoPtr = &(BxOpcodeTable0FBD_LZCNT[osize]);
  }
  int ia_opcode = OpcodeInfoPtr->IA;

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder_modrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();

  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize]);
  Bit16u attr = OpcodeInfoPtr->Attr;
  int ia_opcode = OpcodeInfoPtr->IA;

  unsigned imm_mode  = attr & BxImmediate,
           imm_mode2 = attr & BxImmediate2; // for SSE4A from AMD
  if (decodeImmediate32(iptr, remain, i, imm_mode, imm_mode2) < 0)
    return (-1);

  // check forbidden SSE prefixes
  unsigned group = attr & BxGroupX;
  if (group) {
    if (group < BxPrefixSSE) {
      /* For opcodes with only one allowed SSE prefix */
      if (sse_prefix != (group >> 4)) {
        return BX_IA_ERROR;
      }
    }
    if (group & BxNoPrefixSSE) {
      if (sse_prefix)
        return BX_IA_ERROR;
    }
  }

#if BX_SUPPORT_3DNOW
  if(b1 == 0x10f)
    ia_opcode = Bx3DNowOpcode[i->modRMForm.Ib[0]];
#endif

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[i->osize()]);
  Bit16u attr = OpcodeInfoPtr->Attr;

  // Opcode does not require a MODRM byte.
  unsigned group = attr & BxGroupX;
  if (group == BxNoPrefixSSE && sse_prefix)
    return BX_IA_ERROR;
  int ia_opcode = OpcodeInfoPtr->IA;

  unsigned rm = b1 & 0x7;
  unsigned nnn = (b1 >> 3) & 0x7;
  i->assertModC0();

  unsigned imm_mode  = attr & BxImmediate,
           imm_mode2 = attr & BxImmediate2;
  if (decodeImmediate32(iptr, remain, i, imm_mode, imm_mode2) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder32_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  assert(b1 == 0x90);

  i->assertModC0();

  if (sse_prefix == SSE_PREFIX_F3)
    return BX_IA_PAUSE;
  else
    return BX_IA_NOP;
}

int decoder_simple32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  i->assertModC0();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[0]);
  int ia_opcode = OpcodeInfoPtr->IA;
  return ia_opcode;
}

int decoder_ud32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const BxOpcodeInfo_t *opcode_table)
{
  return BX_IA_ERROR;
}

Bit16u WalkOpcodeTables(const BxExtOpcodeInfo_t *OpcodeInfoPtr, Bit16u &attr, bx_bool is_64, unsigned modrm, unsigned sse_prefix, unsigned osize, unsigned vex_vl, bx_bool vex_w)
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

    if (group == BxNoPrefixSSE) {
      /* For opcodes with no SSE prefix allowed */
      if (sse_prefix) {
        attr = 0;
        return BX_IA_ERROR;
      }
      break;
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
      default:
        BX_PANIC(("WalkOpcodeTables: Unknown opcode group %x", group));
        return BX_IA_ERROR;
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
        if (has_alias == BxAliasVexW || is_64) {
          alias = vex_w;
        }
      }
#endif
      ia_opcode += alias;
    }
  }

  return (ia_opcode);
}

int fetchDecode32(const Bit8u *iptr, bx_bool is_32, bxInstruction_c *i, unsigned remainingInPage)
{
  if (remainingInPage > 15) remainingInPage = 15;

  unsigned remain = remainingInPage; // remain must be at least 1
  unsigned b1;
  int ia_opcode = BX_IA_ERROR;
  unsigned seg_override = BX_SEG_REG_NULL;
  bx_bool os_32 = is_32, lock = 0;
  unsigned sse_prefix = SSE_PREFIX_NONE;

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

#if BX_CPU_LEVEL >= 6
  if (b1 == 0x138 || b1 == 0x13a) {
    if (remain == 0)
      return(-1);
    if (b1 == 0x138) b1 = 0x200 | *iptr++;
    else             b1 = 0x300 | *iptr++;
    remain--;
  }
#endif

  i->setSeg(BX_SEG_REG_DS); // default segment is DS:

  i->modRMForm.Id = 0;
 
  BxOpcodeDecodeDescriptor32 *decode_descriptor = &decode32_descriptor[b1];
  ia_opcode = decode_descriptor->decode_method(iptr, remain, i, b1, sse_prefix, decode_descriptor->opcode_table);
  if (ia_opcode < 0)
    return(-1);

  i->setILen(remainingInPage - remain);
  i->setIaOpcode(ia_opcode);

  // assign memory segment override
  if (! BX_NULL_SEG_REG(seg_override))
    i->setSeg(seg_override);

  Bit32u op_flags = BxOpcodesTable[ia_opcode].opflags;

  if (lock) {
    i->setLock();
    // lock prefix not allowed or destination operand is not memory
    if (i->modC0() || !(op_flags & BX_LOCKABLE)) {
#if BX_CPU_LEVEL >= 6
      if ((op_flags & BX_LOCKABLE) != 0) {
        if (ia_opcode == BX_IA_MOV_CR0Rd)
          i->setSrcReg(0, 8); // extend CR0 -> CR8
        else if (ia_opcode == BX_IA_MOV_RdCR0)
          i->setSrcReg(1, 8); // extend CR0 -> CR8
        else
          i->setIaOpcode(BX_IA_ERROR); // replace execution function with undefined-opcode
      }
      else
#endif
      {
        // replace execution function with undefined-opcode
        i->setIaOpcode(BX_IA_ERROR);
      }
    }
  }

  return(0);
}

#ifndef BX_STANDALONE_DECODER

int assignHandler(bxInstruction_c *i, Bit32u fetchModeMask)
{
  unsigned ia_opcode = i->getIaOpcode();

  if (! i->modC0()) {
    i->execute1 = BxOpcodesTable[ia_opcode].execute1;
    i->handlers.execute2 = BxOpcodesTable[ia_opcode].execute2;

    if (ia_opcode == BX_IA_MOV_Op32_GdEd) {
      if (i->seg() == BX_SEG_REG_SS)
        i->execute1 = &BX_CPU_C::MOV32S_GdEdM;
    }
    if (ia_opcode == BX_IA_MOV_Op32_EdGd) {
      if (i->seg() == BX_SEG_REG_SS)
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
    if (! i->modC0()) {
      if ((op_flags & BX_PREPARE_EVEX_NO_BROADCAST) == BX_PREPARE_EVEX_NO_BROADCAST) {
//      BX_DEBUG(("%s: broadcast is not supported for this instruction", i->getIaOpcodeNameShort()));
        i->execute1 = &BX_CPU_C::BxError;
      }
    }
    else {
      if ((op_flags & BX_PREPARE_EVEX_NO_SAE) == BX_PREPARE_EVEX_NO_SAE) {
//      BX_DEBUG(("%s: EVEX.b in reg form is not allowed for instructions which cannot cause floating point exception", i->getIaOpcodeNameShort()));
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

void BX_CPU_C::init_FetchDecodeTables(void)
{
  static Bit8u BxOpcodeFeatures[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) d,
#include "ia_opcodes.def"
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
      BxOpcodesTable[n].opflags = 0;
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

  // handle lock MOV CR0 AMD extension
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_ALT_MOV_CR8)) {
    BxOpcodesTable[BX_IA_MOV_CR0Rd].opflags |= BX_LOCKABLE;
    BxOpcodesTable[BX_IA_MOV_RdCR0].opflags |= BX_LOCKABLE;
#if BX_SUPPORT_X86_64
    BxOpcodesTable[BX_IA_MOV_CR0Rq].opflags |= BX_LOCKABLE;
    BxOpcodesTable[BX_IA_MOV_RqCR0].opflags |= BX_LOCKABLE;
#endif
  }
}

#endif

const char *get_bx_opcode_name(Bit16u ia_opcode)
{
  static const char* BxOpcodeNamesTable[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, s1, s2, s3, s4, e) #a,
#include "ia_opcodes.def"
  };
#undef  bx_define_opcode

  return (ia_opcode < BX_IA_LAST) ? BxOpcodeNamesTable[ia_opcode] : 0;
}
