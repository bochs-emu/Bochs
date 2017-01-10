/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2016  The Bochs Project
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
#include "../cpu.h"

#define LOG_THIS genlog->

#if BX_SUPPORT_X86_64

///////////////////////////
// prefix bytes
// opcode bytes
// modrm/sib
// address displacement
// immediate constant
///////////////////////////

// Segment override prefixes
// -------------------------
// In 64-bit mode the CS, DS, ES, and SS segment overrides are ignored.

// decoding instructions; accessing seg reg's by index
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
#include "fetchdecode_opmap.h"
#include "fetchdecode_opmap_0f38.h"
#include "fetchdecode_opmap_0f3a.h"

// table of all Bochs opcodes
extern struct bxIAOpcodeTable BxOpcodesTable[];

extern Bit16u WalkOpcodeTables(const BxExtOpcodeInfo_t *OpcodeInfoPtr, Bit16u &attr, bx_bool is_64, unsigned modrm, unsigned sse_prefix, unsigned osize, unsigned vex_vl, bx_bool vex_w);

extern bx_bool assign_srcs(bxInstruction_c *i, unsigned ia_opcode, unsigned nnn, unsigned rm);
#if BX_SUPPORT_AVX
extern bx_bool assign_srcs(bxInstruction_c *i, unsigned ia_opcode, unsigned nnn, unsigned rm, unsigned vvv, unsigned vex_w, bx_bool had_evex = BX_FALSE, bx_bool displ8 = BX_FALSE);
#endif

extern const Bit8u *decodeModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm, unsigned rex_r, unsigned rex_x, unsigned rex_b);
extern const Bit8u *parseModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned rex_prefix, struct bx_modrm *modrm);
extern int decodeImmediate64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned imm_mode, unsigned imm_mode2);

extern int decoder64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_simple64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_creg64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_modrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_sse(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_sseq(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_sse_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_vex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_evex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_xop64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_ud64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder_lzcnt_tzcnt64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group_nnn_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_sse_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group7(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group9(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group15(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);
extern int decoder64_group17a(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);

typedef int (*BxFetchDecode64Ptr)(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table);

struct BxOpcodeDecodeDescriptor64 {
  BxFetchDecode64Ptr decode_method;
  const BxOpcodeInfo_t *opcode_table;
};

static BxOpcodeDecodeDescriptor64 decode64_descriptor[] =
{
   /*       00 */ { &decoder_modrm64, BxOpcodeTable00 },
   /*       01 */ { &decoder_modrm64, BxOpcodeTable01 },
   /*       02 */ { &decoder_modrm64, BxOpcodeTable02 },
   /*       03 */ { &decoder_modrm64, BxOpcodeTable03 },
   /*       04 */ { &decoder64, BxOpcodeTable04 },
   /*       05 */ { &decoder64, BxOpcodeTable05 },
   /*       06 */ { &decoder_ud64, NULL },
   /*       07 */ { &decoder_ud64, NULL },
   /*       08 */ { &decoder_modrm64, BxOpcodeTable08 },
   /*       09 */ { &decoder_modrm64, BxOpcodeTable09 },
   /*       0A */ { &decoder_modrm64, BxOpcodeTable0A },
   /*       0B */ { &decoder_modrm64, BxOpcodeTable0B },
   /*       0C */ { &decoder64, BxOpcodeTable0C },
   /*       0D */ { &decoder64, BxOpcodeTable0D },
   /*       0E */ { &decoder_ud64, NULL },
   /*       0F */ { &decoder_ud64, NULL },             // 2-byte escape
   /*       10 */ { &decoder_modrm64, BxOpcodeTable10 },
   /*       11 */ { &decoder_modrm64, BxOpcodeTable11 },
   /*       12 */ { &decoder_modrm64, BxOpcodeTable12 },
   /*       13 */ { &decoder_modrm64, BxOpcodeTable13 },
   /*       14 */ { &decoder64, BxOpcodeTable14 },
   /*       15 */ { &decoder64, BxOpcodeTable15 },
   /*       16 */ { &decoder_ud64, NULL },
   /*       17 */ { &decoder_ud64, NULL },
   /*       18 */ { &decoder_modrm64, BxOpcodeTable18 },
   /*       19 */ { &decoder_modrm64, BxOpcodeTable19 },
   /*       1A */ { &decoder_modrm64, BxOpcodeTable1A },
   /*       1B */ { &decoder_modrm64, BxOpcodeTable1B },
   /*       1C */ { &decoder64, BxOpcodeTable1C },
   /*       1D */ { &decoder64, BxOpcodeTable1D },
   /*       1E */ { &decoder_ud64, NULL },
   /*       1F */ { &decoder_ud64, NULL },
   /*       20 */ { &decoder_modrm64, BxOpcodeTable20 },
   /*       21 */ { &decoder_modrm64, BxOpcodeTable21 },
   /*       22 */ { &decoder_modrm64, BxOpcodeTable22 },
   /*       23 */ { &decoder_modrm64, BxOpcodeTable23 },
   /*       24 */ { &decoder64, BxOpcodeTable24 },
   /*       25 */ { &decoder64, BxOpcodeTable25 },
   /*       26 */ { &decoder_ud64, NULL },             // ES:
   /*       27 */ { &decoder_ud64, NULL },
   /*       28 */ { &decoder_modrm64, BxOpcodeTable28 },
   /*       29 */ { &decoder_modrm64, BxOpcodeTable29 },
   /*       2A */ { &decoder_modrm64, BxOpcodeTable2A },
   /*       2B */ { &decoder_modrm64, BxOpcodeTable2B },
   /*       2C */ { &decoder64, BxOpcodeTable2C },
   /*       2D */ { &decoder64, BxOpcodeTable2D },
   /*       2E */ { &decoder_ud64, NULL },             // CS:
   /*       2F */ { &decoder_ud64, NULL },
   /*       30 */ { &decoder_modrm64, BxOpcodeTable30 },
   /*       31 */ { &decoder_modrm64, BxOpcodeTable31 },
   /*       32 */ { &decoder_modrm64, BxOpcodeTable32 },
   /*       33 */ { &decoder_modrm64, BxOpcodeTable33 },
   /*       34 */ { &decoder64, BxOpcodeTable34 },
   /*       35 */ { &decoder64, BxOpcodeTable35 },
   /*       36 */ { &decoder_ud64, NULL },             // SS:
   /*       37 */ { &decoder_ud64, NULL },
   /*       38 */ { &decoder_modrm64, BxOpcodeTable38 },
   /*       39 */ { &decoder_modrm64, BxOpcodeTable39 },
   /*       3A */ { &decoder_modrm64, BxOpcodeTable3A },
   /*       3B */ { &decoder_modrm64, BxOpcodeTable3B },
   /*       3C */ { &decoder64, BxOpcodeTable3C },
   /*       3D */ { &decoder64, BxOpcodeTable3D },
   /*       3E */ { &decoder_ud64, NULL },             // DS:
   /*       3F */ { &decoder_ud64, NULL },
   /*       40 */ { &decoder_ud64, NULL },             // REX prefix
   /*       41 */ { &decoder_ud64, NULL },             // REX prefix
   /*       42 */ { &decoder_ud64, NULL },             // REX prefix
   /*       43 */ { &decoder_ud64, NULL },             // REX prefix
   /*       44 */ { &decoder_ud64, NULL },             // REX prefix
   /*       45 */ { &decoder_ud64, NULL },             // REX prefix
   /*       46 */ { &decoder_ud64, NULL },             // REX prefix
   /*       47 */ { &decoder_ud64, NULL },             // REX prefix
   /*       48 */ { &decoder_ud64, NULL },             // REX prefix
   /*       49 */ { &decoder_ud64, NULL },             // REX prefix
   /*       4A */ { &decoder_ud64, NULL },             // REX prefix
   /*       4B */ { &decoder_ud64, NULL },             // REX prefix
   /*       4C */ { &decoder_ud64, NULL },             // REX prefix
   /*       4D */ { &decoder_ud64, NULL },             // REX prefix
   /*       4E */ { &decoder_ud64, NULL },             // REX prefix
   /*       4F */ { &decoder_ud64, NULL },             // REX prefix
   /*       50 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       51 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       52 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       53 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       54 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       55 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       56 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       57 */ { &decoder64, BxOpcodeTable50x57_64 },
   /*       58 */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       59 */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5A */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5B */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5C */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5D */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5E */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       5F */ { &decoder64, BxOpcodeTable58x5F_64 },
   /*       60 */ { &decoder_ud64, NULL },
   /*       61 */ { &decoder_ud64, NULL },
   /*       62 */ { &decoder_evex64, NULL },         // EVEX prefix
   /*       63 */ { &decoder_modrm64, BxOpcodeTable63_64 },
   /*       64 */ { &decoder_ud64, NULL },           // FS:
   /*       65 */ { &decoder_ud64, NULL },           // GS:
   /*       66 */ { &decoder_ud64, NULL },           // OSIZE:
   /*       67 */ { &decoder_ud64, NULL },           // ASIZE:
   /*       68 */ { &decoder64, BxOpcodeTable68_64 },
   /*       69 */ { &decoder_modrm64, BxOpcodeTable69 },
   /*       6A */ { &decoder64, BxOpcodeTable6A_64 },
   /*       6B */ { &decoder_modrm64, BxOpcodeTable6B },
   /*       6C */ { &decoder64, BxOpcodeTable6C },
   /*       6D */ { &decoder64, BxOpcodeTable6D },
   /*       6E */ { &decoder64, BxOpcodeTable6E },
   /*       6F */ { &decoder64, BxOpcodeTable6F },
   /*       70 */ { &decoder64, BxOpcodeTable70_64 },
   /*       71 */ { &decoder64, BxOpcodeTable71_64 },
   /*       72 */ { &decoder64, BxOpcodeTable72_64 },
   /*       73 */ { &decoder64, BxOpcodeTable73_64 },
   /*       74 */ { &decoder64, BxOpcodeTable74_64 },
   /*       75 */ { &decoder64, BxOpcodeTable75_64 },
   /*       76 */ { &decoder64, BxOpcodeTable76_64 },
   /*       77 */ { &decoder64, BxOpcodeTable77_64 },
   /*       78 */ { &decoder64, BxOpcodeTable78_64 },
   /*       79 */ { &decoder64, BxOpcodeTable79_64 },
   /*       7A */ { &decoder64, BxOpcodeTable7A_64 },
   /*       7B */ { &decoder64, BxOpcodeTable7B_64 },
   /*       7C */ { &decoder64, BxOpcodeTable7C_64 },
   /*       7D */ { &decoder64, BxOpcodeTable7D_64 },
   /*       7E */ { &decoder64, BxOpcodeTable7E_64 },
   /*       7F */ { &decoder64, BxOpcodeTable7F_64 },
   /*       80 */ { &decoder64_group_nnn,       BxOpcodeTable80_G1Eb },
   /*       81 */ { &decoder64_group_nnn_osize, BxOpcodeTable81_G1Ev },
   /*       82 */ { &decoder_ud64, NULL },
   /*       83 */ { &decoder64_group_nnn_osize, BxOpcodeTable83_G1Ev },
   /*       84 */ { &decoder_modrm64, BxOpcodeTable84 },
   /*       85 */ { &decoder_modrm64, BxOpcodeTable85 },
   /*       86 */ { &decoder_modrm64, BxOpcodeTable86 },
   /*       87 */ { &decoder_modrm64, BxOpcodeTable87 },
   /*       88 */ { &decoder_modrm64, BxOpcodeTable88 },
   /*       89 */ { &decoder_modrm64, BxOpcodeTable89_64 },
   /*       8A */ { &decoder_modrm64, BxOpcodeTable8A },
   /*       8B */ { &decoder_modrm64, BxOpcodeTable8B_64 },
   /*       8C */ { &decoder_modrm64, BxOpcodeTable8C },
   /*       8D */ { &decoder_modrm64, BxOpcodeTable8D },
   /*       8E */ { &decoder_modrm64, BxOpcodeTable8E },
   /*       8F */ { &decoder_xop64, BxOpcodeTable8F },          // XOP prefix
   /*       90 */ { &decoder64_nop, BxOpcodeTable90x97 },
   /*       91 */ { &decoder64, BxOpcodeTable90x97 },
   /*       92 */ { &decoder64, BxOpcodeTable90x97 },
   /*       93 */ { &decoder64, BxOpcodeTable90x97 },
   /*       94 */ { &decoder64, BxOpcodeTable90x97 },
   /*       95 */ { &decoder64, BxOpcodeTable90x97 },
   /*       96 */ { &decoder64, BxOpcodeTable90x97 },
   /*       97 */ { &decoder64, BxOpcodeTable90x97 },
   /*       98 */ { &decoder64, BxOpcodeTable98 },
   /*       99 */ { &decoder64, BxOpcodeTable99 },
   /*       9A */ { &decoder_ud64, NULL },
   /*       9B */ { &decoder_simple64, BxOpcodeTable9B },
   /*       9C */ { &decoder64, BxOpcodeTable9C_64 },
   /*       9D */ { &decoder64, BxOpcodeTable9D_64 },
   /*       9E */ { &decoder_simple64, BxOpcodeTable9E_64 },
   /*       9F */ { &decoder_simple64, BxOpcodeTable9F_64 },
   /*       A0 */ { &decoder64, BxOpcodeTableA0_64 },
   /*       A1 */ { &decoder64, BxOpcodeTableA1_64 },
   /*       A2 */ { &decoder64, BxOpcodeTableA2_64 },
   /*       A3 */ { &decoder64, BxOpcodeTableA3_64 },
   /*       A4 */ { &decoder64, BxOpcodeTableA4 },
   /*       A5 */ { &decoder64, BxOpcodeTableA5 },
   /*       A6 */ { &decoder64, BxOpcodeTableA6 },
   /*       A7 */ { &decoder64, BxOpcodeTableA7 },
   /*       A8 */ { &decoder64, BxOpcodeTableA8 },
   /*       A9 */ { &decoder64, BxOpcodeTableA9 },
   /*       AA */ { &decoder64, BxOpcodeTableAA },
   /*       AB */ { &decoder64, BxOpcodeTableAB },
   /*       AC */ { &decoder64, BxOpcodeTableAC },
   /*       AD */ { &decoder64, BxOpcodeTableAD },
   /*       AE */ { &decoder64, BxOpcodeTableAE },
   /*       AF */ { &decoder64, BxOpcodeTableAF },
   /*       B0 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B1 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B2 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B3 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B4 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B5 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B6 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B7 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*       B8 */ { &decoder64, BxOpcodeTableB8xBF },
   /*       B9 */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BA */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BB */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BC */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BD */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BE */ { &decoder64, BxOpcodeTableB8xBF },
   /*       BF */ { &decoder64, BxOpcodeTableB8xBF },
   /*       C0 */ { &decoder64_group_nnn,       BxOpcodeTableC0_G2EbIb },
   /*       C1 */ { &decoder64_group_nnn_osize, BxOpcodeTableC1_G2EvIb },
   /*       C2 */ { &decoder64, BxOpcodeTableC2_64 },
   /*       C3 */ { &decoder64, BxOpcodeTableC3_64 },
   /*       C4 */ { &decoder_vex64, NULL },            // VEX prefix
   /*       C5 */ { &decoder_vex64, NULL },            // VEX prefix
   /*       C6 */ { &decoder64_group_nnn,       BxOpcodeTableC6_G11EbIb },
   /*       C7 */ { &decoder64_group_nnn_osize, BxOpcodeTableC7_G11EvIv },
   /*       C8 */ { &decoder64, BxOpcodeTableC8_64 },
   /*       C9 */ { &decoder64, BxOpcodeTableC9_64 },
   /*       CA */ { &decoder64, BxOpcodeTableCA },
   /*       CB */ { &decoder64, BxOpcodeTableCB },
   /*       CC */ { &decoder_simple64, BxOpcodeTableCC },
   /*       CD */ { &decoder64, BxOpcodeTableCD },
   /*       CE */ { &decoder_ud64, NULL },
   /*       CF */ { &decoder64, BxOpcodeTableCF_64 },
   /*       D0 */ { &decoder64_group_nnn,       BxOpcodeTableD0_G2EbI1 },
   /*       D1 */ { &decoder64_group_nnn_osize, BxOpcodeTableD1_G2EvI1 },
   /*       D2 */ { &decoder64_group_nnn,       BxOpcodeTableD2_G2Eb },
   /*       D3 */ { &decoder64_group_nnn_osize, BxOpcodeTableD3_G2Ev },
   /*       D4 */ { &decoder_ud64, NULL },
   /*       D5 */ { &decoder_ud64, NULL },
   /*       D6 */ { &decoder_ud64, NULL },
   /*       D7 */ { &decoder_simple64, BxOpcodeTableD7 },
   /*       D8 */ { &decoder64_fp_escape, NULL },
   /*       D9 */ { &decoder64_fp_escape, NULL },
   /*       DA */ { &decoder64_fp_escape, NULL },
   /*       DB */ { &decoder64_fp_escape, NULL },
   /*       DC */ { &decoder64_fp_escape, NULL },
   /*       DD */ { &decoder64_fp_escape, NULL },
   /*       DE */ { &decoder64_fp_escape, NULL },
   /*       DF */ { &decoder64_fp_escape, NULL },
   /*       E0 */ { &decoder64, BxOpcodeTableE0_64 },
   /*       E1 */ { &decoder64, BxOpcodeTableE1_64 },
   /*       E2 */ { &decoder64, BxOpcodeTableE2_64 },
   /*       E3 */ { &decoder64, BxOpcodeTableE3_64 },
   /*       E4 */ { &decoder64, BxOpcodeTableE4 },
   /*       E5 */ { &decoder64, BxOpcodeTableE5 },
   /*       E6 */ { &decoder64, BxOpcodeTableE6 },
   /*       E7 */ { &decoder64, BxOpcodeTableE7 },
   /*       E8 */ { &decoder64, BxOpcodeTableE8_64 },
   /*       E9 */ { &decoder64, BxOpcodeTableE9_64 },
   /*       EA */ { &decoder_ud64, NULL },
   /*       EB */ { &decoder64, BxOpcodeTableEB_64 },
   /*       EC */ { &decoder64, BxOpcodeTableEC },
   /*       ED */ { &decoder64, BxOpcodeTableED },
   /*       EE */ { &decoder64, BxOpcodeTableEE },
   /*       EF */ { &decoder64, BxOpcodeTableEF },
   /*       F0 */ { &decoder_ud64, NULL },           // LOCK
   /*       F1 */ { &decoder_simple64, BxOpcodeTableF1 },
   /*       F2 */ { &decoder_ud64, NULL },           // REPNE/REPNZ
   /*       F3 */ { &decoder_ud64, NULL },           // REP, REPE/REPZ
   /*       F4 */ { &decoder_simple64, BxOpcodeTableF4 },
   /*       F5 */ { &decoder_simple64, BxOpcodeTableF5 },
   /*       F6 */ { &decoder64_group_nnn,       BxOpcodeTableF6_G3Eb },
   /*       F7 */ { &decoder64_group_nnn_osize, BxOpcodeTableF7_G3Ev },
   /*       F8 */ { &decoder_simple64, BxOpcodeTableF8 },
   /*       F9 */ { &decoder_simple64, BxOpcodeTableF9 },
   /*       FA */ { &decoder_simple64, BxOpcodeTableFA },
   /*       FB */ { &decoder_simple64, BxOpcodeTableFB },
   /*       FC */ { &decoder_simple64, BxOpcodeTableFC },
   /*       FD */ { &decoder_simple64, BxOpcodeTableFD },
   /*       FE */ { &decoder64_group_nnn, BxOpcodeTableG4 },
   /*       FF */ { &decoder64_group_nnn_osize, BxOpcodeTableFF_64G5v },
   /*    0F 00 */ { &decoder64_group_nnn, BxOpcodeTableG6 },
   /*    0F 01 */ { &decoder64_group7, BxOpcodeTable0F01_64 },
   /*    0F 02 */ { &decoder_modrm64, BxOpcodeTable0F02 },
   /*    0F 03 */ { &decoder_modrm64, BxOpcodeTable0F03 },
   /*    0F 04 */ { &decoder_ud64, NULL },
   /*    0F 05 */ { &decoder_simple64, BxOpcodeTable0F05_64 },
   /*    0F 06 */ { &decoder_simple64, BxOpcodeTable0F06 },
   /*    0F 07 */ { &decoder_simple64, BxOpcodeTable0F07_64 },
   /*    0F 08 */ { &decoder_simple64, BxOpcodeTable0F08 },
   /*    0F 09 */ { &decoder_simple64, BxOpcodeTable0F09 },
   /*    0F 0A */ { &decoder_ud64, NULL },
   /*    0F 0B */ { &decoder_simple64, BxOpcodeTable0F0B },
   /*    0F 0C */ { &decoder_ud64, NULL },
   /*    0F 0D */ { &decoder_modrm64, BxOpcodeTable0F0D },
   /*    0F 0E */ { &decoder_simple64, BxOpcodeTable0F0E },
   /*    0F 0F */ { &decoder_modrm64, BxOpcodeTable0F0F },
   /*    0F 10 */ { &decoder64_sse, BxOpcodeGroupSSE_0F10 },
   /*    0F 11 */ { &decoder64_sse, BxOpcodeGroupSSE_0F11 },
   /*    0F 12 */ { &decoder64_sse, BxOpcodeGroupSSE_0F12 },
   /*    0F 13 */ { &decoder64_sse, BxOpcodeGroupSSE_0F13 },
   /*    0F 14 */ { &decoder64_sse, BxOpcodeGroupSSE_0F14 },
   /*    0F 15 */ { &decoder64_sse, BxOpcodeGroupSSE_0F15 },
   /*    0F 16 */ { &decoder64_sse, BxOpcodeGroupSSE_0F16 },
   /*    0F 17 */ { &decoder64_sse, BxOpcodeGroupSSE_0F17 },
   /*    0F 18 */ { &decoder64_group_nnn, BxOpcodeTable0F18_G16 },
   /*    0F 19 */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1A */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1B */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1C */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1D */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1E */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 1F */ { &decoder_modrm64, BxOpcodeTableMultiByteNOP },
   /*    0F 20 */ { &decoder_creg64, BxOpcodeTableMOV_RqCq },
   /*    0F 21 */ { &decoder_creg64, BxOpcodeTable0F21_64 },
   /*    0F 22 */ { &decoder_creg64, BxOpcodeTableMOV_CqRq },
   /*    0F 23 */ { &decoder_creg64, BxOpcodeTable0F23_64 },
   /*    0F 24 */ { &decoder_ud64, NULL },
   /*    0F 25 */ { &decoder_ud64, NULL },
   /*    0F 26 */ { &decoder_ud64, NULL },
   /*    0F 27 */ { &decoder_ud64, NULL },
   /*    0F 28 */ { &decoder64_sse,  BxOpcodeGroupSSE_0F28 },
   /*    0F 29 */ { &decoder64_sse,  BxOpcodeGroupSSE_0F29 },
   /*    0F 2A */ { &decoder64_sseq, BxOpcodeGroupSSE_0F2A },
   /*    0F 2B */ { &decoder64_sse,  BxOpcodeGroupSSE_0F2B },
   /*    0F 2C */ { &decoder64_sseq, BxOpcodeGroupSSE_0F2C },
   /*    0F 2D */ { &decoder64_sseq, BxOpcodeGroupSSE_0F2D },
   /*    0F 2E */ { &decoder64_sse,  BxOpcodeGroupSSE_0F2E },
   /*    0F 2F */ { &decoder64_sse,  BxOpcodeGroupSSE_0F2F },
   /*    0F 30 */ { &decoder_simple64, BxOpcodeTable0F30 },
   /*    0F 31 */ { &decoder_simple64, BxOpcodeTable0F31 },
   /*    0F 32 */ { &decoder_simple64, BxOpcodeTable0F32 },
   /*    0F 33 */ { &decoder_simple64, BxOpcodeTable0F33 },
   /*    0F 34 */ { &decoder_simple64, BxOpcodeTable0F34 },
   /*    0F 35 */ { &decoder_simple64, BxOpcodeTable0F35 },
   /*    0F 36 */ { &decoder_ud64, NULL },
   /*    0F 37 */ { &decoder_simple64, BxOpcodeTable0F37 },
   /*    0F 38 */ { &decoder_ud64, NULL }, // 3-byte escape
   /*    0F 39 */ { &decoder_ud64, NULL },
   /*    0F 3A */ { &decoder_ud64, NULL }, // 3-byte escape
   /*    0F 3B */ { &decoder_ud64, NULL },
   /*    0F 3C */ { &decoder_ud64, NULL },
   /*    0F 3D */ { &decoder_ud64, NULL },
   /*    0F 3E */ { &decoder_ud64, NULL },
   /*    0F 3F */ { &decoder_ud64, NULL },
   /*    0F 40 */ { &decoder_modrm64, BxOpcodeTable0F40 },
   /*    0F 41 */ { &decoder_modrm64, BxOpcodeTable0F41 },
   /*    0F 42 */ { &decoder_modrm64, BxOpcodeTable0F42 },
   /*    0F 43 */ { &decoder_modrm64, BxOpcodeTable0F43 },
   /*    0F 44 */ { &decoder_modrm64, BxOpcodeTable0F44 },
   /*    0F 45 */ { &decoder_modrm64, BxOpcodeTable0F45 },
   /*    0F 46 */ { &decoder_modrm64, BxOpcodeTable0F46 },
   /*    0F 47 */ { &decoder_modrm64, BxOpcodeTable0F47 },
   /*    0F 48 */ { &decoder_modrm64, BxOpcodeTable0F48 },
   /*    0F 49 */ { &decoder_modrm64, BxOpcodeTable0F49 },
   /*    0F 4A */ { &decoder_modrm64, BxOpcodeTable0F4A },
   /*    0F 4B */ { &decoder_modrm64, BxOpcodeTable0F4B },
   /*    0F 4C */ { &decoder_modrm64, BxOpcodeTable0F4C },
   /*    0F 4D */ { &decoder_modrm64, BxOpcodeTable0F4D },
   /*    0F 4E */ { &decoder_modrm64, BxOpcodeTable0F4E },
   /*    0F 4F */ { &decoder_modrm64, BxOpcodeTable0F4F },
   /*    0F 50 */ { &decoder64_sse, BxOpcodeGroupSSE_0F50 },
   /*    0F 51 */ { &decoder64_sse, BxOpcodeGroupSSE_0F51 },
   /*    0F 52 */ { &decoder64_sse, BxOpcodeGroupSSE_0F52 },
   /*    0F 53 */ { &decoder64_sse, BxOpcodeGroupSSE_0F53 },
   /*    0F 54 */ { &decoder64_sse, BxOpcodeGroupSSE_0F54 },
   /*    0F 55 */ { &decoder64_sse, BxOpcodeGroupSSE_0F55 },
   /*    0F 56 */ { &decoder64_sse, BxOpcodeGroupSSE_0F56 },
   /*    0F 57 */ { &decoder64_sse, BxOpcodeGroupSSE_0F57 },
   /*    0F 58 */ { &decoder64_sse, BxOpcodeGroupSSE_0F58 },
   /*    0F 59 */ { &decoder64_sse, BxOpcodeGroupSSE_0F59 },
   /*    0F 5A */ { &decoder64_sse, BxOpcodeGroupSSE_0F5A },
   /*    0F 5B */ { &decoder64_sse, BxOpcodeGroupSSE_0F5B },
   /*    0F 5C */ { &decoder64_sse, BxOpcodeGroupSSE_0F5C },
   /*    0F 5D */ { &decoder64_sse, BxOpcodeGroupSSE_0F5D },
   /*    0F 5E */ { &decoder64_sse, BxOpcodeGroupSSE_0F5E },
   /*    0F 5F */ { &decoder64_sse, BxOpcodeGroupSSE_0F5F },
   /*    0F 60 */ { &decoder64_sse, BxOpcodeGroupSSE_0F60 },
   /*    0F 61 */ { &decoder64_sse, BxOpcodeGroupSSE_0F61 },
   /*    0F 62 */ { &decoder64_sse, BxOpcodeGroupSSE_0F62 },
   /*    0F 63 */ { &decoder64_sse, BxOpcodeGroupSSE_0F63 },
   /*    0F 64 */ { &decoder64_sse, BxOpcodeGroupSSE_0F64 },
   /*    0F 65 */ { &decoder64_sse, BxOpcodeGroupSSE_0F65 },
   /*    0F 66 */ { &decoder64_sse, BxOpcodeGroupSSE_0F66 },
   /*    0F 67 */ { &decoder64_sse, BxOpcodeGroupSSE_0F67 },
   /*    0F 68 */ { &decoder64_sse, BxOpcodeGroupSSE_0F68 },
   /*    0F 69 */ { &decoder64_sse, BxOpcodeGroupSSE_0F69 },
   /*    0F 6A */ { &decoder64_sse, BxOpcodeGroupSSE_0F6A },
   /*    0F 6B */ { &decoder64_sse, BxOpcodeGroupSSE_0F6B },
   /*    0F 6C */ { &decoder64_sse, BxOpcodeGroupSSE_0F6C },
   /*    0F 6D */ { &decoder64_sse, BxOpcodeGroupSSE_0F6D },
   /*    0F 6E */ { &decoder64_sseq, BxOpcodeGroupSSE_0F6E },
   /*    0F 6F */ { &decoder64_sse, BxOpcodeGroupSSE_0F6F },
   /*    0F 70 */ { &decoder64_sse, BxOpcodeGroupSSE_0F70 },
   /*    0F 71 */ { &decoder64_sse_group_nnn, BxOpcodeTable0F71_G12 },
   /*    0F 72 */ { &decoder64_sse_group_nnn, BxOpcodeTable0F72_G13 },
   /*    0F 73 */ { &decoder64_sse_group_nnn, BxOpcodeTable0F73_G14 },
   /*    0F 74 */ { &decoder64_sse, BxOpcodeGroupSSE_0F74 },
   /*    0F 75 */ { &decoder64_sse, BxOpcodeGroupSSE_0F75 },
   /*    0F 76 */ { &decoder64_sse, BxOpcodeGroupSSE_0F76 },
   /*    0F 77 */ { &decoder64, BxOpcodeTable0F77 },
   /*    0F 78 */ { &decoder64_group17a, BxOpcodeTable0F78 },
   /*    0F 79 */ { &decoder64_sse, BxOpcodeGroupSSE_0F79_64 },
   /*    0F 7A */ { &decoder_ud64, NULL },
   /*    0F 7B */ { &decoder_ud64, NULL },
   /*    0F 7C */ { &decoder64_sse, BxOpcodeGroupSSE_0F7C },
   /*    0F 7D */ { &decoder64_sse, BxOpcodeGroupSSE_0F7D },
   /*    0F 7E */ { &decoder64_sseq, BxOpcodeGroupSSE_0F7E },
   /*    0F 7F */ { &decoder64_sse, BxOpcodeGroupSSE_0F7F },
   /*    0F 80 */ { &decoder64, BxOpcodeTable0F80_64 },
   /*    0F 81 */ { &decoder64, BxOpcodeTable0F81_64 },
   /*    0F 82 */ { &decoder64, BxOpcodeTable0F82_64 },
   /*    0F 83 */ { &decoder64, BxOpcodeTable0F83_64 },
   /*    0F 84 */ { &decoder64, BxOpcodeTable0F84_64 },
   /*    0F 85 */ { &decoder64, BxOpcodeTable0F85_64 },
   /*    0F 86 */ { &decoder64, BxOpcodeTable0F86_64 },
   /*    0F 87 */ { &decoder64, BxOpcodeTable0F87_64 },
   /*    0F 88 */ { &decoder64, BxOpcodeTable0F88_64 },
   /*    0F 89 */ { &decoder64, BxOpcodeTable0F89_64 },
   /*    0F 8A */ { &decoder64, BxOpcodeTable0F8A_64 },
   /*    0F 8B */ { &decoder64, BxOpcodeTable0F8B_64 },
   /*    0F 8C */ { &decoder64, BxOpcodeTable0F8C_64 },
   /*    0F 8D */ { &decoder64, BxOpcodeTable0F8D_64 },
   /*    0F 8E */ { &decoder64, BxOpcodeTable0F8E_64 },
   /*    0F 8F */ { &decoder64, BxOpcodeTable0F8F_64 },
   /*    0F 90 */ { &decoder_modrm64, BxOpcodeTable0F90 },
   /*    0F 91 */ { &decoder_modrm64, BxOpcodeTable0F91 },
   /*    0F 92 */ { &decoder_modrm64, BxOpcodeTable0F92 },
   /*    0F 93 */ { &decoder_modrm64, BxOpcodeTable0F93 },
   /*    0F 94 */ { &decoder_modrm64, BxOpcodeTable0F94 },
   /*    0F 95 */ { &decoder_modrm64, BxOpcodeTable0F95 },
   /*    0F 96 */ { &decoder_modrm64, BxOpcodeTable0F96 },
   /*    0F 97 */ { &decoder_modrm64, BxOpcodeTable0F97 },
   /*    0F 98 */ { &decoder_modrm64, BxOpcodeTable0F98 },
   /*    0F 99 */ { &decoder_modrm64, BxOpcodeTable0F99 },
   /*    0F 9A */ { &decoder_modrm64, BxOpcodeTable0F9A },
   /*    0F 9B */ { &decoder_modrm64, BxOpcodeTable0F9B },
   /*    0F 9C */ { &decoder_modrm64, BxOpcodeTable0F9C },
   /*    0F 9D */ { &decoder_modrm64, BxOpcodeTable0F9D },
   /*    0F 9E */ { &decoder_modrm64, BxOpcodeTable0F9E },
   /*    0F 9F */ { &decoder_modrm64, BxOpcodeTable0F9F },
   /*    0F A0 */ { &decoder64, BxOpcodeTable0FA0_64 },
   /*    0F A1 */ { &decoder64, BxOpcodeTable0FA1_64 },
   /*    0F A2 */ { &decoder_simple64, BxOpcodeTable0FA2 },
   /*    0F A3 */ { &decoder_modrm64, BxOpcodeTable0FA3 },
   /*    0F A4 */ { &decoder_modrm64, BxOpcodeTable0FA4 },
   /*    0F A5 */ { &decoder_modrm64, BxOpcodeTable0FA5 },
   /*    0F A6 */ { &decoder_ud64, NULL },
   /*    0F A7 */ { &decoder_ud64, NULL },
   /*    0F A8 */ { &decoder64, BxOpcodeTable0FA8_64 },
   /*    0F A9 */ { &decoder64, BxOpcodeTable0FA9_64 },
   /*    0F AA */ { &decoder_simple64, BxOpcodeTable0FAA },
   /*    0F AB */ { &decoder_modrm64, BxOpcodeTable0FAB },
   /*    0F AC */ { &decoder_modrm64, BxOpcodeTable0FAC },
   /*    0F AD */ { &decoder_modrm64, BxOpcodeTable0FAD },
   /*    0F AE */ { &decoder64_group15, NULL },
   /*    0F AF */ { &decoder_modrm64, BxOpcodeTable0FAF },
   /*    0F B0 */ { &decoder_modrm64, BxOpcodeTable0FB0 },
   /*    0F B1 */ { &decoder_modrm64, BxOpcodeTable0FB1 },
   /*    0F B2 */ { &decoder_modrm64, BxOpcodeTable0FB2 },
   /*    0F B3 */ { &decoder_modrm64, BxOpcodeTable0FB3 },
   /*    0F B4 */ { &decoder_modrm64, BxOpcodeTable0FB4 },
   /*    0F B5 */ { &decoder_modrm64, BxOpcodeTable0FB5 },
   /*    0F B6 */ { &decoder_modrm64, BxOpcodeTable0FB6 },
   /*    0F B7 */ { &decoder_modrm64, BxOpcodeTable0FB7 },
   /*    0F B8 */ { &decoder_modrm64, BxOpcodeTable0FB8 },
   /*    0F B9 */ { &decoder_modrm64, BxOpcodeTable0FB9 },
   /*    0F BA */ { &decoder64_group_nnn_osize, BxOpcodeTable0FBA_G8EvIv },
   /*    0F BB */ { &decoder_modrm64, BxOpcodeTable0FBB },
   /*    0F BC */ { &decoder_lzcnt_tzcnt64, BxOpcodeTable0FBC },
   /*    0F BD */ { &decoder_lzcnt_tzcnt64, BxOpcodeTable0FBD },
   /*    0F BE */ { &decoder_modrm64, BxOpcodeTable0FBE },
   /*    0F BF */ { &decoder_modrm64, BxOpcodeTable0FBF },
   /*    0F C0 */ { &decoder_modrm64, BxOpcodeTable0FC0 },
   /*    0F C1 */ { &decoder_modrm64, BxOpcodeTable0FC1 },
   /*    0F C2 */ { &decoder64_sse, BxOpcodeGroupSSE_0FC2 },
   /*    0F C3 */ { &decoder_modrm64, BxOpcodeTable0FC3_64 },
   /*    0F C4 */ { &decoder64_sse, BxOpcodeGroupSSE_0FC4 },
   /*    0F C5 */ { &decoder64_sse, BxOpcodeGroupSSE_0FC5 },
   /*    0F C6 */ { &decoder64_sse, BxOpcodeGroupSSE_0FC6 },
   /*    0F C7 */ { &decoder64_group9, NULL },
   /*    0F C8 */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F C9 */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CA */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CB */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CC */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CD */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CE */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F CF */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /*    0F D0 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD0 },
   /*    0F D1 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD1 },
   /*    0F D2 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD2 },
   /*    0F D3 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD3 },
   /*    0F D4 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD4 },
   /*    0F D5 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD5 },
   /*    0F D6 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD6 },
   /*    0F D7 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD7 },
   /*    0F D8 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD8 },
   /*    0F D9 */ { &decoder64_sse, BxOpcodeGroupSSE_0FD9 },
   /*    0F DA */ { &decoder64_sse, BxOpcodeGroupSSE_0FDA },
   /*    0F DB */ { &decoder64_sse, BxOpcodeGroupSSE_0FDB },
   /*    0F DC */ { &decoder64_sse, BxOpcodeGroupSSE_0FDC },
   /*    0F DD */ { &decoder64_sse, BxOpcodeGroupSSE_0FDD },
   /*    0F DE */ { &decoder64_sse, BxOpcodeGroupSSE_0FDE },
   /*    0F DF */ { &decoder64_sse, BxOpcodeGroupSSE_0FDF },
   /*    0F E0 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE0 },
   /*    0F E1 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE1 },
   /*    0F E2 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE2 },
   /*    0F E3 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE3 },
   /*    0F E4 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE4 },
   /*    0F E5 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE5 },
   /*    0F E6 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE6 },
   /*    0F E7 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE7 },
   /*    0F E8 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE8 },
   /*    0F E9 */ { &decoder64_sse, BxOpcodeGroupSSE_0FE9 },
   /*    0F EA */ { &decoder64_sse, BxOpcodeGroupSSE_0FEA },
   /*    0F EB */ { &decoder64_sse, BxOpcodeGroupSSE_0FEB },
   /*    0F EC */ { &decoder64_sse, BxOpcodeGroupSSE_0FEC },
   /*    0F ED */ { &decoder64_sse, BxOpcodeGroupSSE_0FED },
   /*    0F EE */ { &decoder64_sse, BxOpcodeGroupSSE_0FEE },
   /*    0F EF */ { &decoder64_sse, BxOpcodeGroupSSE_0FEF },
   /*    0F F0 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF0 },
   /*    0F F1 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF1 },
   /*    0F F2 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF2 },
   /*    0F F3 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF3 },
   /*    0F F4 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF4 },
   /*    0F F5 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF5 },
   /*    0F F6 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF6 },
   /*    0F F7 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF7 },
   /*    0F F8 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF8 },
   /*    0F F9 */ { &decoder64_sse, BxOpcodeGroupSSE_0FF9 },
   /*    0F FA */ { &decoder64_sse, BxOpcodeGroupSSE_0FFA },
   /*    0F FB */ { &decoder64_sse, BxOpcodeGroupSSE_0FFB },
   /*    0F FC */ { &decoder64_sse, BxOpcodeGroupSSE_0FFC },
   /*    0F FD */ { &decoder64_sse, BxOpcodeGroupSSE_0FFD },
   /*    0F FE */ { &decoder64_sse, BxOpcodeGroupSSE_0FFE },
   /*    0F FF */ { &decoder_ud64, NULL },

   // 3-byte opcode 0x0F 0x38
   /* 0F 38 00 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3800 },
   /* 0F 38 01 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3801 },
   /* 0F 38 02 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3802 },
   /* 0F 38 03 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3803 },
   /* 0F 38 04 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3804 },
   /* 0F 38 05 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3805 },
   /* 0F 38 06 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3806 },
   /* 0F 38 07 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3807 },
   /* 0F 38 08 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3808 },
   /* 0F 38 09 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3809 },
   /* 0F 38 0A */ { &decoder64_sse, BxOpcodeGroupSSE_0F380A },
   /* 0F 38 0B */ { &decoder64_sse, BxOpcodeGroupSSE_0F380B },
   /* 0F 38 0C */ { &decoder_ud64, NULL },
   /* 0F 38 0D */ { &decoder_ud64, NULL },
   /* 0F 38 0E */ { &decoder_ud64, NULL },
   /* 0F 38 0F */ { &decoder_ud64, NULL },
   /* 0F 38 10 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3810 },
   /* 0F 38 11 */ { &decoder_ud64, NULL },
   /* 0F 38 12 */ { &decoder_ud64, NULL },
   /* 0F 38 13 */ { &decoder_ud64, NULL },
   /* 0F 38 14 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3814 },
   /* 0F 38 15 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3815 },
   /* 0F 38 16 */ { &decoder_ud64, NULL },
   /* 0F 38 17 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3817 },
   /* 0F 38 18 */ { &decoder_ud64, NULL },
   /* 0F 38 19 */ { &decoder_ud64, NULL },
   /* 0F 38 1A */ { &decoder_ud64, NULL },
   /* 0F 38 1B */ { &decoder_ud64, NULL },
   /* 0F 38 1C */ { &decoder64_sse, BxOpcodeGroupSSE_0F381C },
   /* 0F 38 1D */ { &decoder64_sse, BxOpcodeGroupSSE_0F381D },
   /* 0F 38 1E */ { &decoder64_sse, BxOpcodeGroupSSE_0F381E },
   /* 0F 38 1F */ { &decoder_ud64, NULL },
   /* 0F 38 20 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3820 },
   /* 0F 38 21 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3821 },
   /* 0F 38 22 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3822 },
   /* 0F 38 23 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3823 },
   /* 0F 38 24 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3824 },
   /* 0F 38 25 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3825 },
   /* 0F 38 26 */ { &decoder_ud64, NULL },
   /* 0F 38 27 */ { &decoder_ud64, NULL },
   /* 0F 38 28 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3828 },
   /* 0F 38 29 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3829 },
   /* 0F 38 2A */ { &decoder64_sse, BxOpcodeGroupSSE_0F382A },
   /* 0F 38 2B */ { &decoder64_sse, BxOpcodeGroupSSE_0F382B },
   /* 0F 38 2C */ { &decoder_ud64, NULL },
   /* 0F 38 2D */ { &decoder_ud64, NULL },
   /* 0F 38 2E */ { &decoder_ud64, NULL },
   /* 0F 38 2F */ { &decoder_ud64, NULL },
   /* 0F 38 30 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3830 },
   /* 0F 38 31 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3831 },
   /* 0F 38 32 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3832 },
   /* 0F 38 33 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3833 },
   /* 0F 38 34 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3834 },
   /* 0F 38 35 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3835 },
   /* 0F 38 36 */ { &decoder_ud64, NULL },
   /* 0F 38 37 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3837 },
   /* 0F 38 38 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3838 },
   /* 0F 38 39 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3839 },
   /* 0F 38 3A */ { &decoder64_sse, BxOpcodeGroupSSE_0F383A },
   /* 0F 38 3B */ { &decoder64_sse, BxOpcodeGroupSSE_0F383B },
   /* 0F 38 3C */ { &decoder64_sse, BxOpcodeGroupSSE_0F383C },
   /* 0F 38 3D */ { &decoder64_sse, BxOpcodeGroupSSE_0F383D },
   /* 0F 38 3E */ { &decoder64_sse, BxOpcodeGroupSSE_0F383E },
   /* 0F 38 3F */ { &decoder64_sse, BxOpcodeGroupSSE_0F383F },
   /* 0F 38 40 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3840 },
   /* 0F 38 41 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3841 },
   /* 0F 38 42 */ { &decoder_ud64, NULL },
   /* 0F 38 43 */ { &decoder_ud64, NULL },
   /* 0F 38 44 */ { &decoder_ud64, NULL },
   /* 0F 38 45 */ { &decoder_ud64, NULL },
   /* 0F 38 46 */ { &decoder_ud64, NULL },
   /* 0F 38 47 */ { &decoder_ud64, NULL },
   /* 0F 38 48 */ { &decoder_ud64, NULL },
   /* 0F 38 49 */ { &decoder_ud64, NULL },
   /* 0F 38 4A */ { &decoder_ud64, NULL },
   /* 0F 38 4B */ { &decoder_ud64, NULL },
   /* 0F 38 4C */ { &decoder_ud64, NULL },
   /* 0F 38 4D */ { &decoder_ud64, NULL },
   /* 0F 38 4E */ { &decoder_ud64, NULL },
   /* 0F 38 4F */ { &decoder_ud64, NULL },
   /* 0F 38 50 */ { &decoder_ud64, NULL },
   /* 0F 38 51 */ { &decoder_ud64, NULL },
   /* 0F 38 52 */ { &decoder_ud64, NULL },
   /* 0F 38 53 */ { &decoder_ud64, NULL },
   /* 0F 38 54 */ { &decoder_ud64, NULL },
   /* 0F 38 55 */ { &decoder_ud64, NULL },
   /* 0F 38 56 */ { &decoder_ud64, NULL },
   /* 0F 38 57 */ { &decoder_ud64, NULL },
   /* 0F 38 58 */ { &decoder_ud64, NULL },
   /* 0F 38 59 */ { &decoder_ud64, NULL },
   /* 0F 38 5A */ { &decoder_ud64, NULL },
   /* 0F 38 5B */ { &decoder_ud64, NULL },
   /* 0F 38 5C */ { &decoder_ud64, NULL },
   /* 0F 38 5D */ { &decoder_ud64, NULL },
   /* 0F 38 5E */ { &decoder_ud64, NULL },
   /* 0F 38 5F */ { &decoder_ud64, NULL },
   /* 0F 38 60 */ { &decoder_ud64, NULL },
   /* 0F 38 61 */ { &decoder_ud64, NULL },
   /* 0F 38 62 */ { &decoder_ud64, NULL },
   /* 0F 38 63 */ { &decoder_ud64, NULL },
   /* 0F 38 64 */ { &decoder_ud64, NULL },
   /* 0F 38 65 */ { &decoder_ud64, NULL },
   /* 0F 38 66 */ { &decoder_ud64, NULL },
   /* 0F 38 67 */ { &decoder_ud64, NULL },
   /* 0F 38 68 */ { &decoder_ud64, NULL },
   /* 0F 38 69 */ { &decoder_ud64, NULL },
   /* 0F 38 6A */ { &decoder_ud64, NULL },
   /* 0F 38 6B */ { &decoder_ud64, NULL },
   /* 0F 38 6C */ { &decoder_ud64, NULL },
   /* 0F 38 6D */ { &decoder_ud64, NULL },
   /* 0F 38 6E */ { &decoder_ud64, NULL },
   /* 0F 38 6F */ { &decoder_ud64, NULL },
   /* 0F 38 70 */ { &decoder_ud64, NULL },
   /* 0F 38 71 */ { &decoder_ud64, NULL },
   /* 0F 38 72 */ { &decoder_ud64, NULL },
   /* 0F 38 73 */ { &decoder_ud64, NULL },
   /* 0F 38 74 */ { &decoder_ud64, NULL },
   /* 0F 38 75 */ { &decoder_ud64, NULL },
   /* 0F 38 76 */ { &decoder_ud64, NULL },
   /* 0F 38 77 */ { &decoder_ud64, NULL },
   /* 0F 38 78 */ { &decoder_ud64, NULL },
   /* 0F 38 79 */ { &decoder_ud64, NULL },
   /* 0F 38 7A */ { &decoder_ud64, NULL },
   /* 0F 38 7B */ { &decoder_ud64, NULL },
   /* 0F 38 7C */ { &decoder_ud64, NULL },
   /* 0F 38 7D */ { &decoder_ud64, NULL },
   /* 0F 38 7E */ { &decoder_ud64, NULL },
   /* 0F 38 7F */ { &decoder_ud64, NULL },
   /* 0F 38 80 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3880 },
   /* 0F 38 81 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3881 },
   /* 0F 38 82 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3882 },
   /* 0F 38 83 */ { &decoder_ud64, NULL },
   /* 0F 38 84 */ { &decoder_ud64, NULL },
   /* 0F 38 85 */ { &decoder_ud64, NULL },
   /* 0F 38 86 */ { &decoder_ud64, NULL },
   /* 0F 38 87 */ { &decoder_ud64, NULL },
   /* 0F 38 88 */ { &decoder_ud64, NULL },
   /* 0F 38 89 */ { &decoder_ud64, NULL },
   /* 0F 38 8A */ { &decoder_ud64, NULL },
   /* 0F 38 8B */ { &decoder_ud64, NULL },
   /* 0F 38 8C */ { &decoder_ud64, NULL },
   /* 0F 38 8D */ { &decoder_ud64, NULL },
   /* 0F 38 8E */ { &decoder_ud64, NULL },
   /* 0F 38 8F */ { &decoder_ud64, NULL },
   /* 0F 38 90 */ { &decoder_ud64, NULL },
   /* 0F 38 91 */ { &decoder_ud64, NULL },
   /* 0F 38 92 */ { &decoder_ud64, NULL },
   /* 0F 38 93 */ { &decoder_ud64, NULL },
   /* 0F 38 94 */ { &decoder_ud64, NULL },
   /* 0F 38 95 */ { &decoder_ud64, NULL },
   /* 0F 38 96 */ { &decoder_ud64, NULL },
   /* 0F 38 97 */ { &decoder_ud64, NULL },
   /* 0F 38 98 */ { &decoder_ud64, NULL },
   /* 0F 38 99 */ { &decoder_ud64, NULL },
   /* 0F 38 9A */ { &decoder_ud64, NULL },
   /* 0F 38 9B */ { &decoder_ud64, NULL },
   /* 0F 38 9C */ { &decoder_ud64, NULL },
   /* 0F 38 9D */ { &decoder_ud64, NULL },
   /* 0F 38 9E */ { &decoder_ud64, NULL },
   /* 0F 38 9F */ { &decoder_ud64, NULL },
   /* 0F 38 A0 */ { &decoder_ud64, NULL },
   /* 0F 38 A1 */ { &decoder_ud64, NULL },
   /* 0F 38 A2 */ { &decoder_ud64, NULL },
   /* 0F 38 A3 */ { &decoder_ud64, NULL },
   /* 0F 38 A4 */ { &decoder_ud64, NULL },
   /* 0F 38 A5 */ { &decoder_ud64, NULL },
   /* 0F 38 A6 */ { &decoder_ud64, NULL },
   /* 0F 38 A7 */ { &decoder_ud64, NULL },
   /* 0F 38 A8 */ { &decoder_ud64, NULL },
   /* 0F 38 A9 */ { &decoder_ud64, NULL },
   /* 0F 38 AA */ { &decoder_ud64, NULL },
   /* 0F 38 AB */ { &decoder_ud64, NULL },
   /* 0F 38 AC */ { &decoder_ud64, NULL },
   /* 0F 38 AD */ { &decoder_ud64, NULL },
   /* 0F 38 AE */ { &decoder_ud64, NULL },
   /* 0F 38 AF */ { &decoder_ud64, NULL },
   /* 0F 38 B0 */ { &decoder_ud64, NULL },
   /* 0F 38 B1 */ { &decoder_ud64, NULL },
   /* 0F 38 B2 */ { &decoder_ud64, NULL },
   /* 0F 38 B3 */ { &decoder_ud64, NULL },
   /* 0F 38 B4 */ { &decoder_ud64, NULL },
   /* 0F 38 B5 */ { &decoder_ud64, NULL },
   /* 0F 38 B6 */ { &decoder_ud64, NULL },
   /* 0F 38 B7 */ { &decoder_ud64, NULL },
   /* 0F 38 B8 */ { &decoder_ud64, NULL },
   /* 0F 38 B9 */ { &decoder_ud64, NULL },
   /* 0F 38 BA */ { &decoder_ud64, NULL },
   /* 0F 38 BB */ { &decoder_ud64, NULL },
   /* 0F 38 BC */ { &decoder_ud64, NULL },
   /* 0F 38 BD */ { &decoder_ud64, NULL },
   /* 0F 38 BE */ { &decoder_ud64, NULL },
   /* 0F 38 BF */ { &decoder_ud64, NULL },
   /* 0F 38 C0 */ { &decoder_ud64, NULL },
   /* 0F 38 C1 */ { &decoder_ud64, NULL },
   /* 0F 38 C2 */ { &decoder_ud64, NULL },
   /* 0F 38 C3 */ { &decoder_ud64, NULL },
   /* 0F 38 C4 */ { &decoder_ud64, NULL },
   /* 0F 38 C5 */ { &decoder_ud64, NULL },
   /* 0F 38 C6 */ { &decoder_ud64, NULL },
   /* 0F 38 C7 */ { &decoder_ud64, NULL },
   /* 0F 38 C8 */ { &decoder64_sse, BxOpcodeGroupSSE_0F38C8 },
   /* 0F 38 C9 */ { &decoder64_sse, BxOpcodeGroupSSE_0F38C9 },
   /* 0F 38 CA */ { &decoder64_sse, BxOpcodeGroupSSE_0F38CA },
   /* 0F 38 CB */ { &decoder64_sse, BxOpcodeGroupSSE_0F38CB },
   /* 0F 38 CC */ { &decoder64_sse, BxOpcodeGroupSSE_0F38CC },
   /* 0F 38 CD */ { &decoder64_sse, BxOpcodeGroupSSE_0F38CD },
   /* 0F 38 CE */ { &decoder_ud64, NULL },
   /* 0F 38 CF */ { &decoder_ud64, NULL },
   /* 0F 38 D0 */ { &decoder_ud64, NULL },
   /* 0F 38 D1 */ { &decoder_ud64, NULL },
   /* 0F 38 D2 */ { &decoder_ud64, NULL },
   /* 0F 38 D3 */ { &decoder_ud64, NULL },
   /* 0F 38 D4 */ { &decoder_ud64, NULL },
   /* 0F 38 D5 */ { &decoder_ud64, NULL },
   /* 0F 38 D6 */ { &decoder_ud64, NULL },
   /* 0F 38 D7 */ { &decoder_ud64, NULL },
   /* 0F 38 D8 */ { &decoder_ud64, NULL },
   /* 0F 38 D9 */ { &decoder_ud64, NULL },
   /* 0F 38 DA */ { &decoder_ud64, NULL },
   /* 0F 38 DB */ { &decoder64_sse, BxOpcodeGroupSSE_0F38DB },
   /* 0F 38 DC */ { &decoder64_sse, BxOpcodeGroupSSE_0F38DC },
   /* 0F 38 DD */ { &decoder64_sse, BxOpcodeGroupSSE_0F38DD },
   /* 0F 38 DE */ { &decoder64_sse, BxOpcodeGroupSSE_0F38DE },
   /* 0F 38 DF */ { &decoder64_sse, BxOpcodeGroupSSE_0F38DF },
   /* 0F 38 E0 */ { &decoder_ud64, NULL },
   /* 0F 38 E1 */ { &decoder_ud64, NULL },
   /* 0F 38 E2 */ { &decoder_ud64, NULL },
   /* 0F 38 E3 */ { &decoder_ud64, NULL },
   /* 0F 38 E4 */ { &decoder_ud64, NULL },
   /* 0F 38 E5 */ { &decoder_ud64, NULL },
   /* 0F 38 E6 */ { &decoder_ud64, NULL },
   /* 0F 38 E7 */ { &decoder_ud64, NULL },
   /* 0F 38 E8 */ { &decoder_ud64, NULL },
   /* 0F 38 E9 */ { &decoder_ud64, NULL },
   /* 0F 38 EA */ { &decoder_ud64, NULL },
   /* 0F 38 EB */ { &decoder_ud64, NULL },
   /* 0F 38 EC */ { &decoder_ud64, NULL },
   /* 0F 38 ED */ { &decoder_ud64, NULL },
   /* 0F 38 EE */ { &decoder_ud64, NULL },
   /* 0F 38 EF */ { &decoder_ud64, NULL },
   /* 0F 38 F0 */ { &decoder64_sse_osize, BxOpcodeGroupSSE_0F38F0 },
   /* 0F 38 F1 */ { &decoder64_sse_osize, BxOpcodeGroupSSE_0F38F1 },
   /* 0F 38 F2 */ { &decoder_ud64, NULL },
   /* 0F 38 F3 */ { &decoder_ud64, NULL },
   /* 0F 38 F4 */ { &decoder_ud64, NULL },
   /* 0F 38 F5 */ { &decoder_ud64, NULL },
   /* 0F 38 F6 */ { &decoder64_sseq, BxOpcodeGroupSSE_0F38F6 },
   /* 0F 38 F7 */ { &decoder_ud64, NULL },
   /* 0F 38 F8 */ { &decoder_ud64, NULL },
   /* 0F 38 F9 */ { &decoder_ud64, NULL },
   /* 0F 38 FA */ { &decoder_ud64, NULL },
   /* 0F 38 FB */ { &decoder_ud64, NULL },
   /* 0F 38 FC */ { &decoder_ud64, NULL },
   /* 0F 38 FD */ { &decoder_ud64, NULL },
   /* 0F 38 FE */ { &decoder_ud64, NULL },
   /* 0F 38 FF */ { &decoder_ud64, NULL },
   /* 0F 38 00 */ { &decoder_ud64, NULL },

   // 3-byte opcode 0x0F 0x3A
   /* 0F 3A 01 */ { &decoder_ud64, NULL },
   /* 0F 3A 02 */ { &decoder_ud64, NULL },
   /* 0F 3A 03 */ { &decoder_ud64, NULL },
   /* 0F 3A 04 */ { &decoder_ud64, NULL },
   /* 0F 3A 05 */ { &decoder_ud64, NULL },
   /* 0F 3A 06 */ { &decoder_ud64, NULL },
   /* 0F 3A 07 */ { &decoder_ud64, NULL },
   /* 0F 3A 08 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A08 },
   /* 0F 3A 09 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A09 },
   /* 0F 3A 0A */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0A },
   /* 0F 3A 0B */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0B },
   /* 0F 3A 0C */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0C },
   /* 0F 3A 0D */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0D },
   /* 0F 3A 0E */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0E },
   /* 0F 3A 0F */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A0F },
   /* 0F 3A 10 */ { &decoder_ud64, NULL },
   /* 0F 3A 11 */ { &decoder_ud64, NULL },
   /* 0F 3A 12 */ { &decoder_ud64, NULL },
   /* 0F 3A 13 */ { &decoder_ud64, NULL },
   /* 0F 3A 14 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A14 },
   /* 0F 3A 15 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A15 },
   /* 0F 3A 16 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A16 },
   /* 0F 3A 17 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A17 },
   /* 0F 3A 18 */ { &decoder_ud64, NULL },
   /* 0F 3A 19 */ { &decoder_ud64, NULL },
   /* 0F 3A 1A */ { &decoder_ud64, NULL },
   /* 0F 3A 1B */ { &decoder_ud64, NULL },
   /* 0F 3A 1C */ { &decoder_ud64, NULL },
   /* 0F 3A 1D */ { &decoder_ud64, NULL },
   /* 0F 3A 1E */ { &decoder_ud64, NULL },
   /* 0F 3A 1F */ { &decoder_ud64, NULL },
   /* 0F 3A 20 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A20 },
   /* 0F 3A 21 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A21 },
   /* 0F 3A 22 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A22 },
   /* 0F 3A 23 */ { &decoder_ud64, NULL },
   /* 0F 3A 24 */ { &decoder_ud64, NULL },
   /* 0F 3A 25 */ { &decoder_ud64, NULL },
   /* 0F 3A 26 */ { &decoder_ud64, NULL },
   /* 0F 3A 27 */ { &decoder_ud64, NULL },
   /* 0F 3A 28 */ { &decoder_ud64, NULL },
   /* 0F 3A 29 */ { &decoder_ud64, NULL },
   /* 0F 3A 2A */ { &decoder_ud64, NULL },
   /* 0F 3A 2B */ { &decoder_ud64, NULL },
   /* 0F 3A 2C */ { &decoder_ud64, NULL },
   /* 0F 3A 2D */ { &decoder_ud64, NULL },
   /* 0F 3A 2E */ { &decoder_ud64, NULL },
   /* 0F 3A 2F */ { &decoder_ud64, NULL },
   /* 0F 3A 30 */ { &decoder_ud64, NULL },
   /* 0F 3A 31 */ { &decoder_ud64, NULL },
   /* 0F 3A 32 */ { &decoder_ud64, NULL },
   /* 0F 3A 33 */ { &decoder_ud64, NULL },
   /* 0F 3A 34 */ { &decoder_ud64, NULL },
   /* 0F 3A 35 */ { &decoder_ud64, NULL },
   /* 0F 3A 36 */ { &decoder_ud64, NULL },
   /* 0F 3A 37 */ { &decoder_ud64, NULL },
   /* 0F 3A 38 */ { &decoder_ud64, NULL },
   /* 0F 3A 39 */ { &decoder_ud64, NULL },
   /* 0F 3A 3A */ { &decoder_ud64, NULL },
   /* 0F 3A 3B */ { &decoder_ud64, NULL },
   /* 0F 3A 3C */ { &decoder_ud64, NULL },
   /* 0F 3A 3D */ { &decoder_ud64, NULL },
   /* 0F 3A 3E */ { &decoder_ud64, NULL },
   /* 0F 3A 3F */ { &decoder_ud64, NULL },
   /* 0F 3A 40 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A40 },
   /* 0F 3A 41 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A41 },
   /* 0F 3A 42 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A42 },
   /* 0F 3A 43 */ { &decoder_ud64, NULL },
   /* 0F 3A 44 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A44 },
   /* 0F 3A 45 */ { &decoder_ud64, NULL },
   /* 0F 3A 46 */ { &decoder_ud64, NULL },
   /* 0F 3A 47 */ { &decoder_ud64, NULL },
   /* 0F 3A 48 */ { &decoder_ud64, NULL },
   /* 0F 3A 49 */ { &decoder_ud64, NULL },
   /* 0F 3A 4A */ { &decoder_ud64, NULL },
   /* 0F 3A 4B */ { &decoder_ud64, NULL },
   /* 0F 3A 4C */ { &decoder_ud64, NULL },
   /* 0F 3A 4D */ { &decoder_ud64, NULL },
   /* 0F 3A 4E */ { &decoder_ud64, NULL },
   /* 0F 3A 4F */ { &decoder_ud64, NULL },
   /* 0F 3A 50 */ { &decoder_ud64, NULL },
   /* 0F 3A 51 */ { &decoder_ud64, NULL },
   /* 0F 3A 52 */ { &decoder_ud64, NULL },
   /* 0F 3A 53 */ { &decoder_ud64, NULL },
   /* 0F 3A 54 */ { &decoder_ud64, NULL },
   /* 0F 3A 55 */ { &decoder_ud64, NULL },
   /* 0F 3A 56 */ { &decoder_ud64, NULL },
   /* 0F 3A 57 */ { &decoder_ud64, NULL },
   /* 0F 3A 58 */ { &decoder_ud64, NULL },
   /* 0F 3A 59 */ { &decoder_ud64, NULL },
   /* 0F 3A 5A */ { &decoder_ud64, NULL },
   /* 0F 3A 5B */ { &decoder_ud64, NULL },
   /* 0F 3A 5C */ { &decoder_ud64, NULL },
   /* 0F 3A 5D */ { &decoder_ud64, NULL },
   /* 0F 3A 5E */ { &decoder_ud64, NULL },
   /* 0F 3A 5F */ { &decoder_ud64, NULL },
   /* 0F 3A 60 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A60 },
   /* 0F 3A 61 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A61 },
   /* 0F 3A 62 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A62 },
   /* 0F 3A 64 */ { &decoder64_sse, BxOpcodeGroupSSE_0F3A63 },
   /* 0F 3A 64 */ { &decoder_ud64, NULL },
   /* 0F 3A 65 */ { &decoder_ud64, NULL },
   /* 0F 3A 66 */ { &decoder_ud64, NULL },
   /* 0F 3A 67 */ { &decoder_ud64, NULL },
   /* 0F 3A 68 */ { &decoder_ud64, NULL },
   /* 0F 3A 69 */ { &decoder_ud64, NULL },
   /* 0F 3A 6A */ { &decoder_ud64, NULL },
   /* 0F 3A 6B */ { &decoder_ud64, NULL },
   /* 0F 3A 6C */ { &decoder_ud64, NULL },
   /* 0F 3A 6D */ { &decoder_ud64, NULL },
   /* 0F 3A 6E */ { &decoder_ud64, NULL },
   /* 0F 3A 6F */ { &decoder_ud64, NULL },
   /* 0F 3A 70 */ { &decoder_ud64, NULL },
   /* 0F 3A 71 */ { &decoder_ud64, NULL },
   /* 0F 3A 72 */ { &decoder_ud64, NULL },
   /* 0F 3A 73 */ { &decoder_ud64, NULL },
   /* 0F 3A 74 */ { &decoder_ud64, NULL },
   /* 0F 3A 75 */ { &decoder_ud64, NULL },
   /* 0F 3A 76 */ { &decoder_ud64, NULL },
   /* 0F 3A 77 */ { &decoder_ud64, NULL },
   /* 0F 3A 78 */ { &decoder_ud64, NULL },
   /* 0F 3A 79 */ { &decoder_ud64, NULL },
   /* 0F 3A 7A */ { &decoder_ud64, NULL },
   /* 0F 3A 7B */ { &decoder_ud64, NULL },
   /* 0F 3A 7C */ { &decoder_ud64, NULL },
   /* 0F 3A 7D */ { &decoder_ud64, NULL },
   /* 0F 3A 7E */ { &decoder_ud64, NULL },
   /* 0F 3A 7F */ { &decoder_ud64, NULL },
   /* 0F 3A 80 */ { &decoder_ud64, NULL },
   /* 0F 3A 81 */ { &decoder_ud64, NULL },
   /* 0F 3A 82 */ { &decoder_ud64, NULL },
   /* 0F 3A 83 */ { &decoder_ud64, NULL },
   /* 0F 3A 84 */ { &decoder_ud64, NULL },
   /* 0F 3A 85 */ { &decoder_ud64, NULL },
   /* 0F 3A 86 */ { &decoder_ud64, NULL },
   /* 0F 3A 87 */ { &decoder_ud64, NULL },
   /* 0F 3A 88 */ { &decoder_ud64, NULL },
   /* 0F 3A 89 */ { &decoder_ud64, NULL },
   /* 0F 3A 8A */ { &decoder_ud64, NULL },
   /* 0F 3A 8B */ { &decoder_ud64, NULL },
   /* 0F 3A 8C */ { &decoder_ud64, NULL },
   /* 0F 3A 8D */ { &decoder_ud64, NULL },
   /* 0F 3A 8E */ { &decoder_ud64, NULL },
   /* 0F 3A 8F */ { &decoder_ud64, NULL },
   /* 0F 3A 90 */ { &decoder_ud64, NULL },
   /* 0F 3A 91 */ { &decoder_ud64, NULL },
   /* 0F 3A 92 */ { &decoder_ud64, NULL },
   /* 0F 3A 93 */ { &decoder_ud64, NULL },
   /* 0F 3A 94 */ { &decoder_ud64, NULL },
   /* 0F 3A 95 */ { &decoder_ud64, NULL },
   /* 0F 3A 96 */ { &decoder_ud64, NULL },
   /* 0F 3A 97 */ { &decoder_ud64, NULL },
   /* 0F 3A 98 */ { &decoder_ud64, NULL },
   /* 0F 3A 99 */ { &decoder_ud64, NULL },
   /* 0F 3A 9A */ { &decoder_ud64, NULL },
   /* 0F 3A 9B */ { &decoder_ud64, NULL },
   /* 0F 3A 9C */ { &decoder_ud64, NULL },
   /* 0F 3A 9D */ { &decoder_ud64, NULL },
   /* 0F 3A 9E */ { &decoder_ud64, NULL },
   /* 0F 3A 9F */ { &decoder_ud64, NULL },
   /* 0F 3A A0 */ { &decoder_ud64, NULL },
   /* 0F 3A A1 */ { &decoder_ud64, NULL },
   /* 0F 3A A2 */ { &decoder_ud64, NULL },
   /* 0F 3A A3 */ { &decoder_ud64, NULL },
   /* 0F 3A A4 */ { &decoder_ud64, NULL },
   /* 0F 3A A5 */ { &decoder_ud64, NULL },
   /* 0F 3A A6 */ { &decoder_ud64, NULL },
   /* 0F 3A A7 */ { &decoder_ud64, NULL },
   /* 0F 3A A8 */ { &decoder_ud64, NULL },
   /* 0F 3A A9 */ { &decoder_ud64, NULL },
   /* 0F 3A AA */ { &decoder_ud64, NULL },
   /* 0F 3A AB */ { &decoder_ud64, NULL },
   /* 0F 3A AC */ { &decoder_ud64, NULL },
   /* 0F 3A AD */ { &decoder_ud64, NULL },
   /* 0F 3A AE */ { &decoder_ud64, NULL },
   /* 0F 3A AF */ { &decoder_ud64, NULL },
   /* 0F 3A B0 */ { &decoder_ud64, NULL },
   /* 0F 3A B1 */ { &decoder_ud64, NULL },
   /* 0F 3A B2 */ { &decoder_ud64, NULL },
   /* 0F 3A B3 */ { &decoder_ud64, NULL },
   /* 0F 3A B4 */ { &decoder_ud64, NULL },
   /* 0F 3A B5 */ { &decoder_ud64, NULL },
   /* 0F 3A B6 */ { &decoder_ud64, NULL },
   /* 0F 3A B7 */ { &decoder_ud64, NULL },
   /* 0F 3A B8 */ { &decoder_ud64, NULL },
   /* 0F 3A B9 */ { &decoder_ud64, NULL },
   /* 0F 3A BA */ { &decoder_ud64, NULL },
   /* 0F 3A BB */ { &decoder_ud64, NULL },
   /* 0F 3A BC */ { &decoder_ud64, NULL },
   /* 0F 3A BD */ { &decoder_ud64, NULL },
   /* 0F 3A BE */ { &decoder_ud64, NULL },
   /* 0F 3A BF */ { &decoder_ud64, NULL },
   /* 0F 3A C0 */ { &decoder_ud64, NULL },
   /* 0F 3A C1 */ { &decoder_ud64, NULL },
   /* 0F 3A C2 */ { &decoder_ud64, NULL },
   /* 0F 3A C3 */ { &decoder_ud64, NULL },
   /* 0F 3A C4 */ { &decoder_ud64, NULL },
   /* 0F 3A C5 */ { &decoder_ud64, NULL },
   /* 0F 3A C6 */ { &decoder_ud64, NULL },
   /* 0F 3A C7 */ { &decoder_ud64, NULL },
   /* 0F 3A C8 */ { &decoder_ud64, NULL },
   /* 0F 3A C9 */ { &decoder_ud64, NULL },
   /* 0F 3A CA */ { &decoder_ud64, NULL },
   /* 0F 3A CB */ { &decoder_ud64, NULL },
   /* 0F 3A CC */ { &decoder64_sse, BxOpcodeGroupSSE_0F3ACC },
   /* 0F 3A CD */ { &decoder_ud64, NULL },
   /* 0F 3A CE */ { &decoder_ud64, NULL },
   /* 0F 3A CF */ { &decoder_ud64, NULL },
   /* 0F 3A D0 */ { &decoder_ud64, NULL },
   /* 0F 3A D1 */ { &decoder_ud64, NULL },
   /* 0F 3A D2 */ { &decoder_ud64, NULL },
   /* 0F 3A D3 */ { &decoder_ud64, NULL },
   /* 0F 3A D4 */ { &decoder_ud64, NULL },
   /* 0F 3A D5 */ { &decoder_ud64, NULL },
   /* 0F 3A D6 */ { &decoder_ud64, NULL },
   /* 0F 3A D7 */ { &decoder_ud64, NULL },
   /* 0F 3A D8 */ { &decoder_ud64, NULL },
   /* 0F 3A D9 */ { &decoder_ud64, NULL },
   /* 0F 3A DA */ { &decoder_ud64, NULL },
   /* 0F 3A DB */ { &decoder_ud64, NULL },
   /* 0F 3A DC */ { &decoder_ud64, NULL },
   /* 0F 3A DD */ { &decoder_ud64, NULL },
   /* 0F 3A DE */ { &decoder_ud64, NULL },
   /* 0F 3A DF */ { &decoder64_sse, BxOpcodeGroupSSE_0F3ADF },
   /* 0F 3A E0 */ { &decoder_ud64, NULL },
   /* 0F 3A E1 */ { &decoder_ud64, NULL },
   /* 0F 3A E2 */ { &decoder_ud64, NULL },
   /* 0F 3A E3 */ { &decoder_ud64, NULL },
   /* 0F 3A E4 */ { &decoder_ud64, NULL },
   /* 0F 3A E5 */ { &decoder_ud64, NULL },
   /* 0F 3A E6 */ { &decoder_ud64, NULL },
   /* 0F 3A E7 */ { &decoder_ud64, NULL },
   /* 0F 3A E8 */ { &decoder_ud64, NULL },
   /* 0F 3A E9 */ { &decoder_ud64, NULL },
   /* 0F 3A EA */ { &decoder_ud64, NULL },
   /* 0F 3A EB */ { &decoder_ud64, NULL },
   /* 0F 3A EC */ { &decoder_ud64, NULL },
   /* 0F 3A ED */ { &decoder_ud64, NULL },
   /* 0F 3A EE */ { &decoder_ud64, NULL },
   /* 0F 3A EF */ { &decoder_ud64, NULL },
   /* 0F 3A F0 */ { &decoder_ud64, NULL },
   /* 0F 3A F1 */ { &decoder_ud64, NULL },
   /* 0F 3A F2 */ { &decoder_ud64, NULL },
   /* 0F 3A F3 */ { &decoder_ud64, NULL },
   /* 0F 3A F4 */ { &decoder_ud64, NULL },
   /* 0F 3A F5 */ { &decoder_ud64, NULL },
   /* 0F 3A F6 */ { &decoder_ud64, NULL },
   /* 0F 3A F7 */ { &decoder_ud64, NULL },
   /* 0F 3A F8 */ { &decoder_ud64, NULL },
   /* 0F 3A F9 */ { &decoder_ud64, NULL },
   /* 0F 3A FA */ { &decoder_ud64, NULL },
   /* 0F 3A FB */ { &decoder_ud64, NULL },
   /* 0F 3A FC */ { &decoder_ud64, NULL },
   /* 0F 3A FD */ { &decoder_ud64, NULL },
   /* 0F 3A FE */ { &decoder_ud64, NULL },
   /* 0F 3A FF */ { &decoder_ud64, NULL },
};

int decoder_vex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

#if BX_SUPPORT_AVX
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  unsigned rm = 0, mod = 0, nnn = 0;
  unsigned b2 = 0;

  unsigned offset = 512; 
  if (! i->os32L())
    offset = 0;

  // VEX
  assert((b1 & ~0x1) == 0xc4);

  if (sse_prefix | rex_prefix)
    return(ia_opcode);

  bx_bool vex_w = 0;
  unsigned vex_opcext = 1;
  if (remain == 0)
    return(-1);
  remain--;
  unsigned vex = *iptr++;

  rex_r = ((vex >> 4) & 0x8) ^ 0x8;
  if (b1 == 0xc4) {
    rex_x = ((vex >> 3) & 0x8) ^ 0x8;
    rex_b = ((vex >> 2) & 0x8) ^ 0x8;

    // decode 3-byte VEX prefix
    vex_opcext = vex & 0x1f;
    if (remain == 0)
      return(-1);
    remain--;
    vex = *iptr++;  // fetch VEX3

    if (vex & 0x80) {
      vex_w = 1;
      i->assertOs64();
      i->assertOs32();
      offset = 512*2;
    }
  }

  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  sse_prefix = vex & 0x3;

  if (remain == 0)
    return(-1);
  remain--;
  unsigned opcode_byte = *iptr++; // fetch new b1
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
    b2 = *iptr++;

    // Parse mod-nnn-rm and related bytes
    mod = b2 & 0xc0;
    nnn = ((b2 >> 3) & 0x7) | rex_r;
    rm  = (b2 & 0x7) | rex_b;

    if (mod == 0xc0) { // mod == 11b
      i->assertModC0();
    }
    else {
      iptr = decodeModrm64(iptr, remain, i, mod, nnn, rm, rex_r, rex_x, rex_b);
      if (! iptr) 
        return(-1);
    }

    ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_TRUE, b2, sse_prefix, offset >> 9, i->getVL(), vex_w);
  }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.

    unsigned group = attr & BxGroupX;
    if (group == BxPrefixSSE && sse_prefix)
      OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[sse_prefix-1]);

    ia_opcode = OpcodeInfoPtr->IA;
    rm = (b1 & 7) | rex_b;
    nnn = (b1 >> 3) & 7;
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
      BX_PANIC(("decoder_vex64: VEX with imm_mode = %u", imm_mode));
    }
  }

  if (! assign_srcs(i, ia_opcode, nnn, rm, vvv, vex_w))
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

int decoder_evex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

#if BX_SUPPORT_EVEX
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  unsigned rm = 0, mod = 0, nnn = 0;
  unsigned b2 = 0;

  bx_bool displ8 = BX_FALSE;

  unsigned offset = 512; 
  if (! i->os32L())
    offset = 0;

  // EVEX prefix 0x62
  assert(b1 == 0x62);

  if (sse_prefix | rex_prefix)
    return(ia_opcode);

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

  rex_r = ((evex >> 4) & 0x8) ^ 0x8;
  rex_r |= (evex & 0x10) ^ 0x10;
  rex_x = ((evex >> 3) & 0x8) ^ 0x8;
  rex_b = ((evex >> 2) & 0x8) ^ 0x8;
  rex_b |= (rex_x << 1);

  sse_prefix = (evex >> 8) & 0x3;
  int vvv = 15 - ((evex >> 11) & 0xf);
  unsigned evex_v = ((evex >> 15) & 0x10) ^ 0x10;
  vvv |= evex_v;
  unsigned vex_w = (evex >> 15) & 0x1;
  if (vex_w) {
    i->assertOs64();
    i->assertOs32();
    offset = 512*2;
  }

  unsigned opmask = (evex >> 16) & 0x7;
  i->setOpmask(opmask);
  unsigned evex_b = (evex >> 20) & 0x1;
  i->setEvexb(evex_b);

  unsigned evex_vl_rc = (evex >> 21) & 0x3;
  i->setRC(evex_vl_rc);
  i->setVL(1 << evex_vl_rc);

  unsigned evex_z = (evex >> 23) & 0x1;
  i->setZeroMasking(evex_z);

  if (evex_z && ! opmask)
    return(ia_opcode);
   
  unsigned opcode_byte = (evex >> 24);
  opcode_byte += 256 * (evex_opcext-1);

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &BxOpcodeTableEVEX[opcode_byte*2 + (opmask != 0)];
  Bit16u attr = OpcodeInfoPtr->Attr;

  // opcode requires modrm byte
  if (remain == 0)
    return(-1);
  remain--;
  b2 = *iptr++;

  // Parse mod-nnn-rm and related bytes
  mod = b2 & 0xc0;
  nnn = ((b2 >> 3) & 0x7) | rex_r;
  rm  = (b2 & 0x7) | rex_b;

  if (mod == 0xc0) { // mod == 11b
    i->assertModC0();
  }
  else {
    iptr = decodeModrm64(iptr, remain, i, mod, nnn, rm, rex_r, rex_x, rex_b);
    if (! iptr) 
      return(-1);
    if (mod == 0x40) { // mod==01b
      displ8 = BX_TRUE;
    }
  }

  // EVEX.b in reg form implies 512-bit vector length
  if (i->modC0() && i->getEvexb()) {
    i->setVL(BX_VL512);
  }

  ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_TRUE, b2, sse_prefix, offset >> 9, i->getVL(), vex_w);

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
      BX_PANIC(("decoder_evex64: EVEX with imm_mode = %u", imm_mode));
    }
  }

  if (! assign_srcs(i, ia_opcode, nnn, rm, vvv, vex_w, BX_TRUE, displ8))
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

int decoder_xop64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // 3 byte XOP prefix
  assert(b1 == 0x8f);

  if ((*iptr & 0x08) != 0x08) {
    // not XOP prefix, decode regular opcode
    struct bx_modrm modrm;
    iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
    if (! iptr) 
      return(-1);

    if (modrm.nnn != 0)
      return BX_IA_ERROR;

    if (i->osize())
      ia_opcode = BX_IA_POP_Eq;
    else
      ia_opcode = BX_IA_POP_Ew;

    assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

    return ia_opcode;
  }

#if BX_SUPPORT_AVX
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  unsigned rm = 0, mod = 0, nnn = 0;
  unsigned b2 = 0;
  bx_bool vex_w = 0;

  unsigned offset = 512;
  if (! i->os32L())
    offset = 0;

  if (sse_prefix | rex_prefix)
    return(ia_opcode);

  unsigned vex;
  if (remain > 2) {
    remain -= 3;
    vex = *iptr++; // fetch XOP2
  }
  else
    return(-1);

  rex_r = ((vex >> 4) & 0x8) ^ 0x8;
  rex_x = ((vex >> 3) & 0x8) ^ 0x8;
  rex_b = ((vex >> 2) & 0x8) ^ 0x8;

  unsigned xop_opcext = (vex & 0x1f) - 8;
  if (xop_opcext >= 3)
    return(ia_opcode);

  vex = *iptr++; // fetch XOP3

  if (vex & 0x80) {
    vex_w = 1;
    i->assertOs64();
    i->assertOs32();
    offset = 512*2;
  }

  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  sse_prefix = vex & 0x3;
  if (sse_prefix)
    return(ia_opcode);

  unsigned opcode_byte = *iptr++;
  opcode_byte += 256 * xop_opcext;

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &BxOpcodeTableXOP[opcode_byte];
  Bit16u attr = OpcodeInfoPtr->Attr;

  // opcode requires modrm byte
  if (remain == 0)
    return(-1);
  remain--;
  b2 = *iptr++;

  // Parse mod-nnn-rm and related bytes
  mod = b2 & 0xc0;
  nnn = ((b2 >> 3) & 0x7) | rex_r;
  rm  = (b2 & 0x7) | rex_b;

  if (mod == 0xc0) { // mod == 11b
    i->assertModC0();
  }
  else {
    iptr = decodeModrm64(iptr, remain, i, mod, nnn, rm, rex_r, rex_x, rex_b);
    if (! iptr) 
      return(-1);
  }

  ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_TRUE, b2, sse_prefix, offset >> 9, i->getVL(), vex_w);

  unsigned imm_mode = attr & BxImmediate;
  if (imm_mode) {
    // make sure iptr was advanced after Ib(), Iw() and Id()
    switch (imm_mode) {
      case BxImmediate_Ib:
        if (remain != 0) {
          i->modRMForm.Ib[0] = *iptr++;
          remain--;
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
      default:
        BX_PANIC(("decoder_xop64: XOP with imm_mode = %u", imm_mode));
        break;
    }
  }

  if (! assign_srcs(i, ia_opcode, nnn, rm, vvv, vex_w))
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

int decoder_ud64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  return BX_IA_ERROR;
}

int decoder64_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  assert(b1 >= 0xd8 && b1 <= 0xdf);

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
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
    ia_opcode = opcodes[modrm.nnn & 0x7];
  else
    ia_opcode = opcodes[(modrm.modrm & 0x3f) + 8];

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_sse(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[sse_prefix]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_sseq(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[sse_prefix]);
  // 64-bit operand size
  if (i->os64L())
    OpcodeInfoPtr += 4;

  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_sse_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize * 4 + sse_prefix]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[modrm.nnn & 0x7]);

  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;
  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_group_nnn_osize(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[(modrm.nnn & 0x7) + (osize * 8)]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;
  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_sse_group_nnn(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[sse_prefix + (modrm.nnn & 0x7) * 4]);
  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;
  unsigned imm_mode = attr & BxImmediate;
  if (decodeImmediate64(iptr, remain, i, imm_mode, 0 /* imm_mode2 */) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F 01 opcode - G7 group
int decoder64_group7(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr;
  if (modrm.mod != 0xc0) {
    // mem form
    OpcodeInfoPtr = &(opcode_table[modrm.nnn & 0x7]);
  }
  else {
    // reg form - special opcode table
    OpcodeInfoPtr = &(BxOpcodeTable0F01[modrm.modrm & 0x3f]);
  }

  int ia_opcode = OpcodeInfoPtr->IA;
  Bit16u attr = OpcodeInfoPtr->Attr;

  if (attr == BxNoPrefixSSE && sse_prefix)
    ia_opcode = BX_IA_ERROR;

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F C7 opcode - G9 group
int decoder64_group9(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  static const BxExtOpcodeInfo_t BxOpcodeTable0FC7[3] = {
    /* 0F C7 /w */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9w },
    /* 0F C7 /d */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfoG9d },
    /* 0F C7 /q */ { BxGroup9, BX_IA_ERROR, BxOpcodeInfo64G9q },
  };

  const BxExtOpcodeInfo_t *OpcodeInfoPtr = &(BxOpcodeTable0FC7[osize]);
  Bit16u attr = OpcodeInfoPtr->Attr;
  int ia_opcode = WalkOpcodeTables(OpcodeInfoPtr, attr, BX_TRUE, modrm.modrm, sse_prefix, osize, i->getVL(), 0);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F AE opcode - G15 group
int decoder64_group15(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr;
  if (modrm.mod == 0xc0) {
    if (i->os64L())
      OpcodeInfoPtr = &(BxOpcodeTable0FAE_G15q_R64[modrm.nnn & 0x7]);
    else
      OpcodeInfoPtr = &(BxOpcodeTable0FAE_G15d_R64[modrm.nnn & 0x7]);
  }
  else {
    OpcodeInfoPtr = &(BxOpcodeTable0FAE_G15M[sse_prefix + (modrm.nnn & 0x7) * 4]);
  }

  int ia_opcode = OpcodeInfoPtr->IA;

  Bit16u attr = OpcodeInfoPtr->Attr;
  if (attr == BxPrefixSSEF3) {
    if (sse_prefix != SSE_PREFIX_F3)
      return ia_opcode = BX_IA_ERROR;
  }
  if (attr == BxNoPrefixSSE) {
    if (sse_prefix)
      return ia_opcode = BX_IA_ERROR;
  }

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

// special case for 0F 78 opcode (SSE4A)
int decoder64_group17a(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  int ia_opcode = BX_IA_ERROR;
  if (sse_prefix) {
    // handle SSE4A extension, decode manually
    switch (sse_prefix) {
    case SSE_PREFIX_66:
      if ((modrm.nnn & 0x7) == 0) ia_opcode = BX_IA_EXTRQ_UdqIbIb;
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

    if (decodeImmediate64(iptr, remain, i, BxImmediate_Ib, BxImmediate_Ib2) < 0)
      return (-1);
  }
  else {
    const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[1]);
    ia_opcode = OpcodeInfoPtr->IA;
  }

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder_creg64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned rm = 0, nnn = 0;
  unsigned b2 = 0;

  unsigned rex_r=0, rex_b=0;
  if (rex_prefix) {
    rex_r = ((rex_prefix & 0x4) << 1);
    // rex_x not used
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  // opcode requires modrm byte
  if (remain == 0)
    return(-1);
  remain--;
  b2 = *iptr++;

  // MOVs with CRx and DRx always use register ops and ignore the mod field.
  assert((b1 & ~7) == 0x120);

  // Parse mod-nnn-rm and related bytes
  nnn = ((b2 >> 3) & 0x7) | rex_r;
  rm  = (b2 & 0x7) | rex_b;

  i->assertModC0();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &opcode_table[0];
  if (b1 == 0x120 || b1 == 0x122)
     OpcodeInfoPtr = &(opcode_table[nnn & 0x7]);
  int ia_opcode = OpcodeInfoPtr->IA;

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder_lzcnt_tzcnt64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
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

int decoder_modrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_prefix, &modrm);
  if (! iptr) 
    return(-1);

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize]);
  Bit16u attr = OpcodeInfoPtr->Attr;
  int ia_opcode = OpcodeInfoPtr->IA;

  unsigned imm_mode  = attr & BxImmediate,
           imm_mode2 = attr & BxImmediate2; // for SSE4A from AMD
  if (decodeImmediate64(iptr, remain, i, imm_mode, imm_mode2) < 0)
    return (-1);

  // check forbidden SSE prefixes
  unsigned group = attr & BxGroupX;
  if (group & BxPrefixSSE66) {
    if (sse_prefix != SSE_PREFIX_66)
      return BX_IA_ERROR;
  }
  if (group & BxPrefixSSEF2) {
    if (sse_prefix != SSE_PREFIX_F2)
      return BX_IA_ERROR;
  }
  if (group & BxPrefixSSEF3) {
    if (sse_prefix != SSE_PREFIX_F3)
      return BX_IA_ERROR;
  }
  if (group & BxNoPrefixSSE) {
    if (sse_prefix)
      return BX_IA_ERROR;
  }

#if BX_SUPPORT_3DNOW
  if(b1 == 0x10f)
    ia_opcode = Bx3DNowOpcode[i->modRMForm.Ib[0]];
#endif

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  assert(b1 == 0x90);

  i->assertModC0();

  unsigned rex_b = ((rex_prefix & 0x1) << 3);
  if (rex_b) {
    unsigned osize = i->osize();
    if (osize > 2) // 64-bit was encoded as '11
      osize = 2;

    const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize]);
    int ia_opcode = OpcodeInfoPtr->IA;

    unsigned rm = (b1 & 7) | rex_b;
    unsigned nnn = (b1 >> 3) & 7;

    assign_srcs(i, ia_opcode, nnn, rm);

    return ia_opcode;
  }
  else {

    if (sse_prefix == SSE_PREFIX_F3)
      return BX_IA_PAUSE;
    else
      return BX_IA_NOP;
  }
}

int decoder_simple64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  i->assertModC0();

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[0]);
  int ia_opcode = OpcodeInfoPtr->IA;
  return ia_opcode;
}

int decoder64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const BxOpcodeInfo_t *opcode_table)
{
  unsigned osize = i->osize();
  if (osize > 2) // 64-bit was encoded as '11
    osize = 2;

  unsigned rex_b = 0;
  if (rex_prefix) {
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  const BxOpcodeInfo_t *OpcodeInfoPtr = &(opcode_table[osize]);
  Bit16u attr = OpcodeInfoPtr->Attr;

  // Opcode does not require a MODRM byte.
  unsigned group = attr & BxGroupX;
  if (group == BxNoPrefixSSE && sse_prefix)
    return BX_IA_ERROR;
  int ia_opcode = OpcodeInfoPtr->IA;

  unsigned rm = (b1 & 7) | rex_b;
  unsigned nnn = (b1 >> 3) & 7;
  i->assertModC0();

  unsigned imm_mode  = attr & BxImmediate,
           imm_mode2 = attr & BxImmediate2;
  if (decodeImmediate64(iptr, remain, i, imm_mode, imm_mode2) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

const Bit8u *parseModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned rex_prefix, struct bx_modrm *modrm)
{
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  if (rex_prefix) {
    rex_r = ((rex_prefix & 0x4) << 1);
    rex_x = ((rex_prefix & 0x2) << 2);
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  // opcode requires modrm byte
  if (remain == 0)
    return NULL;
  remain--;
  unsigned b2 = *iptr++;

  // Keep original modrm byte
  modrm->modrm = b2;

  // Parse mod-nnn-rm and related bytes
  modrm->mod = b2 & 0xc0;
  modrm->nnn = ((b2 >> 3) & 0x7) | rex_r;
  modrm->rm  = (b2 & 0x7) | rex_b;

  if (modrm->mod == 0xc0) { // mod == 11b
    i->assertModC0();
  }
  else {
    iptr = decodeModrm64(iptr, remain, i, modrm->mod, modrm->nnn, modrm->rm, rex_r, rex_x, rex_b);
  }

  return iptr;
}

const Bit8u *decodeModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm, unsigned rex_r, unsigned rex_x, unsigned rex_b)
{
  unsigned seg = BX_SEG_REG_DS;

  i->setSibBase(rm & 0xf); // initialize with rm to use BxResolve64Base
  i->setSibIndex(4);
  // initialize displ32 with zero to include cases with no diplacement
  i->modRMForm.displ32u = 0;

  // note that mod==11b handled outside

  if ((rm & 0x7) != 4) { // no s-i-b byte
    if (mod == 0x00) { // mod == 00b
      if ((rm & 0x7) == 5) {
        i->setSibBase(BX_64BIT_REG_RIP);
        goto get_32bit_displ;
      }
      // mod==00b, rm!=4, rm!=5
      goto modrm_done;
    }
    // (mod == 0x40), mod==01b or (mod == 0x80), mod==10b
    seg = sreg_mod1or2_base32[rm & 0xf];
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
    base  = (sib & 0x7) | rex_b; sib >>= 3;
    index = (sib & 0x7) | rex_x; sib >>= 3;
    scale =  sib;
    i->setSibScale(scale);
    i->setSibBase(base & 0xf);
    // this part is a little tricky - assign index value always,
    // it will be really used if the instruction is Gather. Others
    // assume that resolve function will do the right thing.
    i->setSibIndex(index & 0xf);
    if (mod == 0x00) { // mod==00b, rm==4
      seg = sreg_mod0_base32[base & 0xf];
      if ((base & 0x7) == 5) {
        i->setSibBase(BX_NIL_REGISTER);
        goto get_32bit_displ;
      }
      // mod==00b, rm==4, base!=5
      goto modrm_done;
    }
    // (mod == 0x40), mod==01b or (mod == 0x80), mod==10b
    seg = sreg_mod1or2_base32[base & 0xf];
  }

  // (mod == 0x40), mod==01b
  if (mod == 0x40) {
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

get_32bit_displ:

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

modrm_done:

  i->setSeg(seg);
  return iptr;  
}

int decodeImmediate64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned imm_mode, unsigned imm_mode2)
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
      case BxImmediate_BrOff8:
        if (remain != 0) {
          Bit8s temp8s = *iptr++;
          i->modRMForm.Id = (Bit32s) temp8s;
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
      case BxImmediate_Iq: // MOV Rx,imm64
        if (remain > 7) {
          i->IqForm.Iq = FetchQWORD(iptr);
          iptr += 8;
          remain -= 8;
        }
        else {
          return(-1);
        }
        break;
      case BxImmediate_O:
        // For instructions which embed the address in the opcode.
        // There is only 64/32-bit addressing available in long64 mode.
        if (i->as64L()) {
          if (remain > 7) {
            i->IqForm.Iq = FetchQWORD(iptr);
            iptr += 8;
            remain -= 8;
          }
          else return(-1);
        }
        else { // as32
          if (remain > 3) {
            i->IqForm.Iq = (Bit64u) FetchDWORD(iptr);
            iptr += 4;
            remain -= 4;
          }
          else return(-1);
        }
        break;
      default:
        BX_PANIC(("decoder64: imm_mode = %u", imm_mode));
        break;
    }

    if (imm_mode2) {
      if (imm_mode2 == BxImmediate_Ib2) {
        if (remain != 0) {
          i->modRMForm.Ib2[0] = *iptr;
          remain--;
        }
        else {
          return(-1);
        }
      }
      else {
        BX_PANIC(("decoder64: imm_mode2 = %u", imm_mode2));
      }
    }
  }

  return 0;
}

int fetchDecode64(const Bit8u *iptr, Bit32u fetchModeMask, bx_bool handle_lock_cr0, bxInstruction_c *i, unsigned remainingInPage)
{
  if (remainingInPage > 15) remainingInPage = 15;

  unsigned remain = remainingInPage; // remain must be at least 1
  unsigned b1;
  int ia_opcode = BX_IA_ERROR;
  unsigned seg_override = BX_SEG_REG_NULL;
  bx_bool lock = 0;
  unsigned sse_prefix = SSE_PREFIX_NONE;
  unsigned rex_prefix = 0;

  i->init(/*os32*/ 1,  // operand size 32 override defaults to 1
          /*as32*/ 1,  // address size 32 override defaults to 1
          /*os64*/ 0,  // operand size 64 override defaults to 0
          /*as64*/ 1); // address size 64 override defaults to 1

fetch_b1:
  b1 = *iptr++;
  remain--;

  switch (b1) {
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
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x0f: // 2 byte escape
      if (remain != 0) {
        remain--;
        b1 = 0x100 | *iptr++;
        break;
      }
      return(-1);
    case 0xf2: // REPNE/REPNZ
    case 0xf3: // REP/REPE/REPZ
      rex_prefix = 0;
      sse_prefix = (b1 & 3) ^ 1;
      i->setLockRepUsed(b1 & 3);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x2e: // CS:
    case 0x26: // ES:
    case 0x36: // SS:
    case 0x3e: // DS:
      /* ignore segment override prefix */
      rex_prefix = 0;
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x64: // FS:
    case 0x65: // GS:
      rex_prefix = 0;
      seg_override = b1 & 0xf;
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x66: // OpSize
      rex_prefix = 0;
      if(!sse_prefix) sse_prefix = SSE_PREFIX_66;
      i->setOs32B(0);
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0x67: // AddrSize
      rex_prefix = 0;
      i->clearAs64();
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    case 0xf0: // LOCK:
      rex_prefix = 0;
      lock = 1;
      if (remain != 0) {
        goto fetch_b1;
      }
      return(-1);
    default:
      break;
  }

  // handle 3-byte opcode
  if (b1 == 0x138 || b1 == 0x13a) {
    if (remain == 0)
      return(-1);
    if (b1 == 0x138) b1 = 0x200 | *iptr++;
    else             b1 = 0x300 | *iptr++;
    remain--;
  }

  if (rex_prefix) {
    i->assertExtend8bit();
    if (rex_prefix & 0x8) {
      i->assertOs64();
      i->assertOs32();
    }
  }

  i->setSeg(BX_SEG_REG_DS); // default segment is DS:

  i->modRMForm.Id = 0;

  BxOpcodeDecodeDescriptor64 *decode_descriptor = &decode64_descriptor[b1];
  ia_opcode = decode_descriptor->decode_method(iptr, remain, i, b1, sse_prefix, rex_prefix, decode_descriptor->opcode_table);
  if (ia_opcode < 0)
    return(-1);

  i->setILen(remainingInPage - remain);
  i->setIaOpcode(ia_opcode);

  // assign memory segment override
  if (! BX_NULL_SEG_REG(seg_override))
     i->setSeg(seg_override);

  if (! i->modC0()) {
    i->execute1 = BxOpcodesTable[ia_opcode].execute1;
    i->handlers.execute2 = BxOpcodesTable[ia_opcode].execute2;

    if (ia_opcode == BX_IA_MOV_GqEq) {
      if (i->seg() == BX_SEG_REG_SS)
        i->execute1 = &BX_CPU_C::MOV64S_GqEqM;
    }
    if (ia_opcode == BX_IA_MOV_EqGq) {
      if (i->seg() == BX_SEG_REG_SS)
        i->execute1 = &BX_CPU_C::MOV64S_EqGqM;
    }
  }
  else {
    i->execute1 = BxOpcodesTable[ia_opcode].execute2;
    i->handlers.execute2 = NULL;
  }

  BX_ASSERT(i->execute1);

  Bit32u op_flags = BxOpcodesTable[ia_opcode].opflags;

  if (lock) {
    i->setLock();
    // lock prefix not allowed or destination operand is not memory
    if (i->modC0() || !(op_flags & BX_LOCKABLE)) {
      if (handle_lock_cr0 && (ia_opcode == BX_IA_MOV_CR0Rq || ia_opcode == BX_IA_MOV_RqCR0)) {
        if (ia_opcode == BX_IA_MOV_CR0Rq)
          i->setSrcReg(0, 8); // extend CR0 -> CR8
        if (ia_opcode == BX_IA_MOV_RqCR0)
          i->setSrcReg(1, 8); // extend CR0 -> CR8
      }
      else {
        // replace execution function with undefined-opcode
        i->execute1 = &BX_CPU_C::BxError;
      }
    }
  }

#if BX_SUPPORT_EVEX
  if ((op_flags & BX_PREPARE_EVEX) != 0 && i->getEvexb()) {
    if (! i->modC0()) {
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

  if ((op_flags & BX_TRACE_END) != 0 || i->execute1 == &BX_CPU_C::BxError)
     return(1);

  return(0);
}

#endif /* if BX_SUPPORT_X86_64 */
