/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2024  The Bochs Project
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
#define NEED_CPU_REG_SHORTCUTS 1
#define NEED_CPU_TEMPLATE_METHODS 1
#include "../cpu.h"
#include "../cpuid.h"
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

extern const Bit64u *BxOpcodeTable0F38[];
extern const Bit64u *BxOpcodeTable0F3A[];

#include "fetchdecode_x87.h"

#if BX_SUPPORT_AVX
extern const Bit64u *BxOpcodeTableVEX[];
extern const Bit64u *BxOpcodeTableXOP[];
#endif

#if BX_SUPPORT_EVEX
extern const Bit64u *BxOpcodeTableEVEX[];
#endif

extern int decoder_simple32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder_creg32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder32_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder32_3dnow(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder_vex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder_evex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder_xop32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder_ud32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder32_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);
extern int decoder32_modrm(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);

typedef int (*BxFetchDecode32Ptr)(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table);

struct BxOpcodeDecodeDescriptor32 {
  BxFetchDecode32Ptr decode_method;
  const void *opcode_table;
};

#ifndef BX_STANDALONE_DECODER
#include "cpu/simd_int.h"
#include "cpu/softfloat3e/include/softfloat.h"
#include "cpu/simd_pfp.h"
#include "cpu/simd_compare.h"
#include "cpu/simd_vnni.h"
#include "cpu/simd_bf16.h"
#endif

// table of all Bochs opcodes
bxIAOpcodeTable BxOpcodesTable[] = {
#ifndef BX_STANDALONE_DECODER
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) { d, e, { s1, s2, s3, s4 }, g },
#else
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) {       { s1, s2, s3, s4 }, g },
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
   /*    00 */ { &decoder32_modrm, BxOpcodeTable00 },
   /*    01 */ { &decoder32_modrm, BxOpcodeTable01 },
   /*    02 */ { &decoder32_modrm, BxOpcodeTable02 },
   /*    03 */ { &decoder32_modrm, BxOpcodeTable03 },
   /*    04 */ { &decoder32, BxOpcodeTable04 },
   /*    05 */ { &decoder32, BxOpcodeTable05 },
   /*    06 */ { &decoder32, BxOpcodeTable06 },
   /*    07 */ { &decoder32, BxOpcodeTable07 },
   /*    08 */ { &decoder32_modrm, BxOpcodeTable08 },
   /*    09 */ { &decoder32_modrm, BxOpcodeTable09 },
   /*    0A */ { &decoder32_modrm, BxOpcodeTable0A },
   /*    0B */ { &decoder32_modrm, BxOpcodeTable0B },
   /*    0C */ { &decoder32, BxOpcodeTable0C },
   /*    0D */ { &decoder32, BxOpcodeTable0D },
   /*    0E */ { &decoder32, BxOpcodeTable0E },
   /*    0F */ { &decoder_ud32, NULL },           // 2-byte escape
   /*    10 */ { &decoder32_modrm, BxOpcodeTable10 },
   /*    11 */ { &decoder32_modrm, BxOpcodeTable11 },
   /*    12 */ { &decoder32_modrm, BxOpcodeTable12 },
   /*    13 */ { &decoder32_modrm, BxOpcodeTable13 },
   /*    14 */ { &decoder32, BxOpcodeTable14 },
   /*    15 */ { &decoder32, BxOpcodeTable15 },
   /*    16 */ { &decoder32, BxOpcodeTable16 },
   /*    17 */ { &decoder32, BxOpcodeTable17 },
   /*    18 */ { &decoder32_modrm, BxOpcodeTable18 },
   /*    19 */ { &decoder32_modrm, BxOpcodeTable19 },
   /*    1A */ { &decoder32_modrm, BxOpcodeTable1A },
   /*    1B */ { &decoder32_modrm, BxOpcodeTable1B },
   /*    1C */ { &decoder32, BxOpcodeTable1C },
   /*    1D */ { &decoder32, BxOpcodeTable1D },
   /*    1E */ { &decoder32, BxOpcodeTable1E },
   /*    1F */ { &decoder32, BxOpcodeTable1F },
   /*    20 */ { &decoder32_modrm, BxOpcodeTable20 },
   /*    21 */ { &decoder32_modrm, BxOpcodeTable21 },
   /*    22 */ { &decoder32_modrm, BxOpcodeTable22 },
   /*    23 */ { &decoder32_modrm, BxOpcodeTable23 },
   /*    24 */ { &decoder32, BxOpcodeTable24 },
   /*    25 */ { &decoder32, BxOpcodeTable25 },
   /*    26 */ { &decoder_ud32, NULL },           // ES:
   /*    27 */ { &decoder_simple32, BxOpcodeTable27 },
   /*    28 */ { &decoder32_modrm, BxOpcodeTable28 },
   /*    29 */ { &decoder32_modrm, BxOpcodeTable29 },
   /*    2A */ { &decoder32_modrm, BxOpcodeTable2A },
   /*    2B */ { &decoder32_modrm, BxOpcodeTable2B },
   /*    2C */ { &decoder32, BxOpcodeTable2C },
   /*    2D */ { &decoder32, BxOpcodeTable2D },
   /*    2E */ { &decoder_ud32, NULL },           // CS:
   /*    2F */ { &decoder_simple32, BxOpcodeTable2F },
   /*    30 */ { &decoder32_modrm, BxOpcodeTable30 },
   /*    31 */ { &decoder32_modrm, BxOpcodeTable31 },
   /*    32 */ { &decoder32_modrm, BxOpcodeTable32 },
   /*    33 */ { &decoder32_modrm, BxOpcodeTable33 },
   /*    34 */ { &decoder32, BxOpcodeTable34 },
   /*    35 */ { &decoder32, BxOpcodeTable35 },
   /*    36 */ { &decoder_ud32, NULL },           // SS:
   /*    37 */ { &decoder_simple32, BxOpcodeTable37 },
   /*    38 */ { &decoder32_modrm, BxOpcodeTable38 },
   /*    39 */ { &decoder32_modrm, BxOpcodeTable39 },
   /*    3A */ { &decoder32_modrm, BxOpcodeTable3A },
   /*    3B */ { &decoder32_modrm, BxOpcodeTable3B },
   /*    3C */ { &decoder32, BxOpcodeTable3C },
   /*    3D */ { &decoder32, BxOpcodeTable3D },
   /*    3E */ { &decoder_ud32, NULL },           // DS:
   /*    3F */ { &decoder_simple32, BxOpcodeTable3F },
   /*    40 */ { &decoder32, BxOpcodeTable40x47 },
   /*    41 */ { &decoder32, BxOpcodeTable40x47 },
   /*    42 */ { &decoder32, BxOpcodeTable40x47 },
   /*    43 */ { &decoder32, BxOpcodeTable40x47 },
   /*    44 */ { &decoder32, BxOpcodeTable40x47 },
   /*    45 */ { &decoder32, BxOpcodeTable40x47 },
   /*    46 */ { &decoder32, BxOpcodeTable40x47 },
   /*    47 */ { &decoder32, BxOpcodeTable40x47 },
   /*    48 */ { &decoder32, BxOpcodeTable48x4F },
   /*    49 */ { &decoder32, BxOpcodeTable48x4F },
   /*    4A */ { &decoder32, BxOpcodeTable48x4F },
   /*    4B */ { &decoder32, BxOpcodeTable48x4F },
   /*    4C */ { &decoder32, BxOpcodeTable48x4F },
   /*    4D */ { &decoder32, BxOpcodeTable48x4F },
   /*    4E */ { &decoder32, BxOpcodeTable48x4F },
   /*    4F */ { &decoder32, BxOpcodeTable48x4F },
   /*    50 */ { &decoder32, BxOpcodeTable50x57 },
   /*    51 */ { &decoder32, BxOpcodeTable50x57 },
   /*    52 */ { &decoder32, BxOpcodeTable50x57 },
   /*    53 */ { &decoder32, BxOpcodeTable50x57 },
   /*    54 */ { &decoder32, BxOpcodeTable50x57 },
   /*    55 */ { &decoder32, BxOpcodeTable50x57 },
   /*    56 */ { &decoder32, BxOpcodeTable50x57 },
   /*    57 */ { &decoder32, BxOpcodeTable50x57 },
   /*    58 */ { &decoder32, BxOpcodeTable58x5F },
   /*    59 */ { &decoder32, BxOpcodeTable58x5F },
   /*    5A */ { &decoder32, BxOpcodeTable58x5F },
   /*    5B */ { &decoder32, BxOpcodeTable58x5F },
   /*    5C */ { &decoder32, BxOpcodeTable58x5F },
   /*    5D */ { &decoder32, BxOpcodeTable58x5F },
   /*    5E */ { &decoder32, BxOpcodeTable58x5F },
   /*    5F */ { &decoder32, BxOpcodeTable58x5F },
   /*    60 */ { &decoder32, BxOpcodeTable60 },
   /*    61 */ { &decoder32, BxOpcodeTable61 },
   /*    62 */ { &decoder_evex32, BxOpcodeTable62 },     // EVEX prefix
   /*    63 */ { &decoder32_modrm, BxOpcodeTable63_32 },
   /*    64 */ { &decoder_ud32, NULL },                     // FS:
   /*    65 */ { &decoder_ud32, NULL },                     // GS:
   /*    66 */ { &decoder_ud32, NULL },                     // OSIZE:
   /*    67 */ { &decoder_ud32, NULL },                     // ASIZE:
   /*    68 */ { &decoder32, BxOpcodeTable68 },
   /*    69 */ { &decoder32_modrm, BxOpcodeTable69 },
   /*    6A */ { &decoder32, BxOpcodeTable6A },
   /*    6B */ { &decoder32_modrm, BxOpcodeTable6B },
   /*    6C */ { &decoder32, BxOpcodeTable6C },
   /*    6D */ { &decoder32, BxOpcodeTable6D },
   /*    6E */ { &decoder32, BxOpcodeTable6E },
   /*    6F */ { &decoder32, BxOpcodeTable6F },
   /*    70 */ { &decoder32, BxOpcodeTable70_32 },
   /*    71 */ { &decoder32, BxOpcodeTable71_32 },
   /*    72 */ { &decoder32, BxOpcodeTable72_32 },
   /*    73 */ { &decoder32, BxOpcodeTable73_32 },
   /*    74 */ { &decoder32, BxOpcodeTable74_32 },
   /*    75 */ { &decoder32, BxOpcodeTable75_32 },
   /*    76 */ { &decoder32, BxOpcodeTable76_32 },
   /*    77 */ { &decoder32, BxOpcodeTable77_32 },
   /*    78 */ { &decoder32, BxOpcodeTable78_32 },
   /*    79 */ { &decoder32, BxOpcodeTable79_32 },
   /*    7A */ { &decoder32, BxOpcodeTable7A_32 },
   /*    7B */ { &decoder32, BxOpcodeTable7B_32 },
   /*    7C */ { &decoder32, BxOpcodeTable7C_32 },
   /*    7D */ { &decoder32, BxOpcodeTable7D_32 },
   /*    7E */ { &decoder32, BxOpcodeTable7E_32 },
   /*    7F */ { &decoder32, BxOpcodeTable7F_32 },
   /*    80 */ { &decoder32_modrm, BxOpcodeTable80 },
   /*    81 */ { &decoder32_modrm, BxOpcodeTable81 },
   /*    82 */ { &decoder32_modrm, BxOpcodeTable80 },  // opcode 0x82 is copy of the 0x80
   /*    83 */ { &decoder32_modrm, BxOpcodeTable83 },
   /*    84 */ { &decoder32_modrm, BxOpcodeTable84 },
   /*    85 */ { &decoder32_modrm, BxOpcodeTable85 },
   /*    86 */ { &decoder32_modrm, BxOpcodeTable86 },
   /*    87 */ { &decoder32_modrm, BxOpcodeTable87 },
   /*    88 */ { &decoder32_modrm, BxOpcodeTable88 },
   /*    89 */ { &decoder32_modrm, BxOpcodeTable89 },
   /*    8A */ { &decoder32_modrm, BxOpcodeTable8A },
   /*    8B */ { &decoder32_modrm, BxOpcodeTable8B },
   /*    8C */ { &decoder32_modrm, BxOpcodeTable8C },
   /*    8D */ { &decoder32_modrm, BxOpcodeTable8D },
   /*    8E */ { &decoder32_modrm, BxOpcodeTable8E },
   /*    8F */ { &decoder_xop32, BxOpcodeTable8F },      // XOP prefix
   /*    90 */ { &decoder32_nop, NULL },
   /*    91 */ { &decoder32, BxOpcodeTable90x97 },
   /*    92 */ { &decoder32, BxOpcodeTable90x97 },
   /*    93 */ { &decoder32, BxOpcodeTable90x97 },
   /*    94 */ { &decoder32, BxOpcodeTable90x97 },
   /*    95 */ { &decoder32, BxOpcodeTable90x97 },
   /*    96 */ { &decoder32, BxOpcodeTable90x97 },
   /*    97 */ { &decoder32, BxOpcodeTable90x97 },
   /*    98 */ { &decoder32, BxOpcodeTable98 },
   /*    99 */ { &decoder32, BxOpcodeTable99 },
   /*    9A */ { &decoder32, BxOpcodeTable9A },
   /*    9B */ { &decoder_simple32, BxOpcodeTable9B },
   /*    9C */ { &decoder32, BxOpcodeTable9C },
   /*    9D */ { &decoder32, BxOpcodeTable9D },
   /*    9E */ { &decoder_simple32, BxOpcodeTable9E_32 },
   /*    9F */ { &decoder_simple32, BxOpcodeTable9F_32 },
   /*    A0 */ { &decoder32, BxOpcodeTableA0_32 },
   /*    A1 */ { &decoder32, BxOpcodeTableA1_32 },
   /*    A2 */ { &decoder32, BxOpcodeTableA2_32 },
   /*    A3 */ { &decoder32, BxOpcodeTableA3_32 },
   /*    A4 */ { &decoder32, BxOpcodeTableA4 },
   /*    A5 */ { &decoder32, BxOpcodeTableA5 },
   /*    A6 */ { &decoder32, BxOpcodeTableA6 },
   /*    A7 */ { &decoder32, BxOpcodeTableA7 },
   /*    A8 */ { &decoder32, BxOpcodeTableA8 },
   /*    A9 */ { &decoder32, BxOpcodeTableA9 },
   /*    AA */ { &decoder32, BxOpcodeTableAA },
   /*    AB */ { &decoder32, BxOpcodeTableAB },
   /*    AC */ { &decoder32, BxOpcodeTableAC },
   /*    AD */ { &decoder32, BxOpcodeTableAD },
   /*    AE */ { &decoder32, BxOpcodeTableAE },
   /*    AF */ { &decoder32, BxOpcodeTableAF },
   /*    B0 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B1 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B2 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B3 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B4 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B5 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B6 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B7 */ { &decoder32, BxOpcodeTableB0xB7 },
   /*    B8 */ { &decoder32, BxOpcodeTableB8xBF },
   /*    B9 */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BA */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BB */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BC */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BD */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BE */ { &decoder32, BxOpcodeTableB8xBF },
   /*    BF */ { &decoder32, BxOpcodeTableB8xBF },
   /*    C0 */ { &decoder32_modrm, BxOpcodeTableC0 },
   /*    C1 */ { &decoder32_modrm, BxOpcodeTableC1 },
   /*    C2 */ { &decoder32, BxOpcodeTableC2_32 },
   /*    C3 */ { &decoder32, BxOpcodeTableC3_32 },
   /*    C4 */ { &decoder_vex32, BxOpcodeTableC4_32 },        // VEX prefix
   /*    C5 */ { &decoder_vex32, BxOpcodeTableC5_32 },        // VEX prefix
   /*    C6 */ { &decoder32_modrm, BxOpcodeTableC6 },
   /*    C7 */ { &decoder32_modrm, BxOpcodeTableC7 },
   /*    C8 */ { &decoder32, BxOpcodeTableC8_32 },
   /*    C9 */ { &decoder32, BxOpcodeTableC9_32 },
   /*    CA */ { &decoder32, BxOpcodeTableCA },
   /*    CB */ { &decoder32, BxOpcodeTableCB },
   /*    CC */ { &decoder_simple32, BxOpcodeTableCC },
   /*    CD */ { &decoder32, BxOpcodeTableCD },
   /*    CE */ { &decoder_simple32, BxOpcodeTableCE },
   /*    CF */ { &decoder32, BxOpcodeTableCF_32 },
   /*    D0 */ { &decoder32_modrm, BxOpcodeTableD0 },
   /*    D1 */ { &decoder32_modrm, BxOpcodeTableD1 },
   /*    D2 */ { &decoder32_modrm, BxOpcodeTableD2 },
   /*    D3 */ { &decoder32_modrm, BxOpcodeTableD3 },
   /*    D4 */ { &decoder32, BxOpcodeTableD4 },
   /*    D5 */ { &decoder32, BxOpcodeTableD5 },
   /*    D6 */ { &decoder_simple32, BxOpcodeTableD6 },
   /*    D7 */ { &decoder_simple32, BxOpcodeTableD7 },
   /*    D8 */ { &decoder32_fp_escape, NULL },
   /*    D9 */ { &decoder32_fp_escape, NULL },
   /*    DA */ { &decoder32_fp_escape, NULL },
   /*    DB */ { &decoder32_fp_escape, NULL },
   /*    DC */ { &decoder32_fp_escape, NULL },
   /*    DD */ { &decoder32_fp_escape, NULL },
   /*    DE */ { &decoder32_fp_escape, NULL },
   /*    DF */ { &decoder32_fp_escape, NULL },
   /*    E0 */ { &decoder32, BxOpcodeTableE0_32 },
   /*    E1 */ { &decoder32, BxOpcodeTableE1_32 },
   /*    E2 */ { &decoder32, BxOpcodeTableE2_32 },
   /*    E3 */ { &decoder32, BxOpcodeTableE3_32 },
   /*    E4 */ { &decoder32, BxOpcodeTableE4 },
   /*    E5 */ { &decoder32, BxOpcodeTableE5 },
   /*    E6 */ { &decoder32, BxOpcodeTableE6 },
   /*    E7 */ { &decoder32, BxOpcodeTableE7 },
   /*    E8 */ { &decoder32, BxOpcodeTableE8_32 },
   /*    E9 */ { &decoder32, BxOpcodeTableE9_32 },
   /*    EA */ { &decoder32, BxOpcodeTableEA_32 },
   /*    EB */ { &decoder32, BxOpcodeTableEB_32 },
   /*    EC */ { &decoder32, BxOpcodeTableEC },
   /*    ED */ { &decoder32, BxOpcodeTableED },
   /*    EE */ { &decoder32, BxOpcodeTableEE },
   /*    EF */ { &decoder32, BxOpcodeTableEF },
   /*    F0 */ { &decoder_ud32, NULL },           // LOCK:
   /*    F1 */ { &decoder_simple32, BxOpcodeTableF1 },
   /*    F2 */ { &decoder_ud32, NULL },           // REPNE/REPNZ
   /*    F3 */ { &decoder_ud32, NULL },           // REP, REPE/REPZ
   /*    F4 */ { &decoder_simple32, BxOpcodeTableF4 },
   /*    F5 */ { &decoder_simple32, BxOpcodeTableF5 },
   /*    F6 */ { &decoder32_modrm, BxOpcodeTableF6 },
   /*    F7 */ { &decoder32_modrm, BxOpcodeTableF7 },
   /*    F8 */ { &decoder_simple32, BxOpcodeTableF8 },
   /*    F9 */ { &decoder_simple32, BxOpcodeTableF9 },
   /*    FA */ { &decoder_simple32, BxOpcodeTableFA },
   /*    FB */ { &decoder_simple32, BxOpcodeTableFB },
   /*    FC */ { &decoder_simple32, BxOpcodeTableFC },
   /*    FD */ { &decoder_simple32, BxOpcodeTableFD },
   /*    FE */ { &decoder32_modrm, BxOpcodeTableFE },
   /*    FF */ { &decoder32_modrm, BxOpcodeTableFF },
   /* 0F 00 */ { &decoder32_modrm, BxOpcodeTable0F00 },
   /* 0F 01 */ { &decoder32_modrm, BxOpcodeTable0F01 },
   /* 0F 02 */ { &decoder32_modrm, BxOpcodeTable0F02 },
   /* 0F 03 */ { &decoder32_modrm, BxOpcodeTable0F03 },
   /* 0F 04 */ { &decoder_ud32, NULL },
   /* 0F 05 */ { &decoder_simple32, BxOpcodeTable0F05_32 },
   /* 0F 06 */ { &decoder_simple32, BxOpcodeTable0F06 },
   /* 0F 07 */ { &decoder_simple32, BxOpcodeTable0F07_32 },
   /* 0F 08 */ { &decoder_simple32, BxOpcodeTable0F08 },
   /* 0F 09 */ { &decoder_simple32, BxOpcodeTable0F09 },
   /* 0F 0A */ { &decoder_ud32, NULL },
   /* 0F 0B */ { &decoder_simple32, BxOpcodeTable0F0B },
   /* 0F 0C */ { &decoder_ud32, NULL },
   /* 0F 0D */ { &decoder32_modrm, BxOpcodeTable0F0D },
   /* 0F 0E */ { &decoder_simple32, BxOpcodeTable0F0E },
   /* 0F 0F */ { &decoder32_3dnow, NULL },
   /* 0F 10 */ { &decoder32_modrm, BxOpcodeTable0F10 },
   /* 0F 11 */ { &decoder32_modrm, BxOpcodeTable0F11 },
   /* 0F 12 */ { &decoder32_modrm, BxOpcodeTable0F12 },
   /* 0F 13 */ { &decoder32_modrm, BxOpcodeTable0F13 },
   /* 0F 14 */ { &decoder32_modrm, BxOpcodeTable0F14 },
   /* 0F 15 */ { &decoder32_modrm, BxOpcodeTable0F15 },
   /* 0F 16 */ { &decoder32_modrm, BxOpcodeTable0F16 },
   /* 0F 17 */ { &decoder32_modrm, BxOpcodeTable0F17 },
#if BX_CPU_LEVEL >= 6
   /* 0F 18 */ { &decoder32_modrm, BxOpcodeTable0F18 },
   /* 0F 19 */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1A */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1B */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1C */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1D */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
#if BX_SUPPORT_CET
   /* 0F 1E */ { &decoder32_modrm, BxOpcodeTable0F1E },
#else
   /* 0F 1E */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
#endif
   /* 0F 1F */ { &decoder32_modrm, BxOpcodeTableMultiByteNOP },
#else
   /* 0F 18 */ { &decoder_ud32, NULL },
   /* 0F 19 */ { &decoder_ud32, NULL },
   /* 0F 1A */ { &decoder_ud32, NULL },
   /* 0F 1B */ { &decoder_ud32, NULL },
   /* 0F 1C */ { &decoder_ud32, NULL },
   /* 0F 1D */ { &decoder_ud32, NULL },
   /* 0F 1E */ { &decoder_ud32, NULL },
   /* 0F 1F */ { &decoder_ud32, NULL },
#endif
   /* 0F 20 */ { &decoder_creg32, BxOpcodeTable0F20_32 },
   /* 0F 21 */ { &decoder_creg32, BxOpcodeTable0F21_32 },
   /* 0F 22 */ { &decoder_creg32, BxOpcodeTable0F22_32 },
   /* 0F 23 */ { &decoder_creg32, BxOpcodeTable0F23_32 },
   /* 0F 24 */ { &decoder_creg32, BxOpcodeTable0F24 },
   /* 0F 25 */ { &decoder_ud32, NULL },
   /* 0F 26 */ { &decoder_creg32, BxOpcodeTable0F26 },
   /* 0F 27 */ { &decoder_ud32, NULL },
   /* 0F 28 */ { &decoder32_modrm, BxOpcodeTable0F28 },
   /* 0F 29 */ { &decoder32_modrm, BxOpcodeTable0F29 },
   /* 0F 2A */ { &decoder32_modrm, BxOpcodeTable0F2A },
   /* 0F 2B */ { &decoder32_modrm, BxOpcodeTable0F2B },
   /* 0F 2C */ { &decoder32_modrm, BxOpcodeTable0F2C },
   /* 0F 2D */ { &decoder32_modrm, BxOpcodeTable0F2D },
   /* 0F 2E */ { &decoder32_modrm, BxOpcodeTable0F2E },
   /* 0F 2F */ { &decoder32_modrm, BxOpcodeTable0F2F },
   /* 0F 30 */ { &decoder_simple32, BxOpcodeTable0F30 },
   /* 0F 31 */ { &decoder_simple32, BxOpcodeTable0F31 },
   /* 0F 32 */ { &decoder_simple32, BxOpcodeTable0F32 },
   /* 0F 33 */ { &decoder_simple32, BxOpcodeTable0F33 },
   /* 0F 34 */ { &decoder_simple32, BxOpcodeTable0F34 },
   /* 0F 35 */ { &decoder_simple32, BxOpcodeTable0F35 },
   /* 0F 36 */ { &decoder_ud32, NULL },
   /* 0F 37 */ { &decoder32, BxOpcodeTable0F37 },
   /* 0F 38 */ { &decoder32_modrm, NULL },          // 3-byte escape
   /* 0F 39 */ { &decoder_ud32, NULL },
   /* 0F 3A */ { &decoder32_modrm, NULL },          // 3-byte escape
   /* 0F 3B */ { &decoder_ud32, NULL },
   /* 0F 3C */ { &decoder_ud32, NULL },
   /* 0F 3D */ { &decoder_ud32, NULL },
   /* 0F 3E */ { &decoder_ud32, NULL },
   /* 0F 3F */ { &decoder_ud32, NULL },
   /* 0F 40 */ { &decoder32_modrm, BxOpcodeTable0F40 },
   /* 0F 41 */ { &decoder32_modrm, BxOpcodeTable0F41 },
   /* 0F 42 */ { &decoder32_modrm, BxOpcodeTable0F42 },
   /* 0F 43 */ { &decoder32_modrm, BxOpcodeTable0F43 },
   /* 0F 44 */ { &decoder32_modrm, BxOpcodeTable0F44 },
   /* 0F 45 */ { &decoder32_modrm, BxOpcodeTable0F45 },
   /* 0F 46 */ { &decoder32_modrm, BxOpcodeTable0F46 },
   /* 0F 47 */ { &decoder32_modrm, BxOpcodeTable0F47 },
   /* 0F 48 */ { &decoder32_modrm, BxOpcodeTable0F48 },
   /* 0F 49 */ { &decoder32_modrm, BxOpcodeTable0F49 },
   /* 0F 4A */ { &decoder32_modrm, BxOpcodeTable0F4A },
   /* 0F 4B */ { &decoder32_modrm, BxOpcodeTable0F4B },
   /* 0F 4C */ { &decoder32_modrm, BxOpcodeTable0F4C },
   /* 0F 4D */ { &decoder32_modrm, BxOpcodeTable0F4D },
   /* 0F 4E */ { &decoder32_modrm, BxOpcodeTable0F4E },
   /* 0F 4F */ { &decoder32_modrm, BxOpcodeTable0F4F },
   /* 0F 50 */ { &decoder32_modrm, BxOpcodeTable0F50 },
   /* 0F 51 */ { &decoder32_modrm, BxOpcodeTable0F51 },
   /* 0F 52 */ { &decoder32_modrm, BxOpcodeTable0F52 },
   /* 0F 53 */ { &decoder32_modrm, BxOpcodeTable0F53 },
   /* 0F 54 */ { &decoder32_modrm, BxOpcodeTable0F54 },
   /* 0F 55 */ { &decoder32_modrm, BxOpcodeTable0F55 },
   /* 0F 56 */ { &decoder32_modrm, BxOpcodeTable0F56 },
   /* 0F 57 */ { &decoder32_modrm, BxOpcodeTable0F57 },
   /* 0F 58 */ { &decoder32_modrm, BxOpcodeTable0F58 },
   /* 0F 59 */ { &decoder32_modrm, BxOpcodeTable0F59 },
   /* 0F 5A */ { &decoder32_modrm, BxOpcodeTable0F5A },
   /* 0F 5B */ { &decoder32_modrm, BxOpcodeTable0F5B },
   /* 0F 5C */ { &decoder32_modrm, BxOpcodeTable0F5C },
   /* 0F 5D */ { &decoder32_modrm, BxOpcodeTable0F5D },
   /* 0F 5E */ { &decoder32_modrm, BxOpcodeTable0F5E },
   /* 0F 5F */ { &decoder32_modrm, BxOpcodeTable0F5F },
   /* 0F 60 */ { &decoder32_modrm, BxOpcodeTable0F60 },
   /* 0F 61 */ { &decoder32_modrm, BxOpcodeTable0F61 },
   /* 0F 62 */ { &decoder32_modrm, BxOpcodeTable0F62 },
   /* 0F 63 */ { &decoder32_modrm, BxOpcodeTable0F63 },
   /* 0F 64 */ { &decoder32_modrm, BxOpcodeTable0F64 },
   /* 0F 65 */ { &decoder32_modrm, BxOpcodeTable0F65 },
   /* 0F 66 */ { &decoder32_modrm, BxOpcodeTable0F66 },
   /* 0F 67 */ { &decoder32_modrm, BxOpcodeTable0F67 },
   /* 0F 68 */ { &decoder32_modrm, BxOpcodeTable0F68 },
   /* 0F 69 */ { &decoder32_modrm, BxOpcodeTable0F69 },
   /* 0F 6A */ { &decoder32_modrm, BxOpcodeTable0F6A },
   /* 0F 6B */ { &decoder32_modrm, BxOpcodeTable0F6B },
   /* 0F 6C */ { &decoder32_modrm, BxOpcodeTable0F6C },
   /* 0F 6D */ { &decoder32_modrm, BxOpcodeTable0F6D },
   /* 0F 6E */ { &decoder32_modrm, BxOpcodeTable0F6E },
   /* 0F 6F */ { &decoder32_modrm, BxOpcodeTable0F6F },
   /* 0F 70 */ { &decoder32_modrm, BxOpcodeTable0F70 },
   /* 0F 71 */ { &decoder32_modrm, BxOpcodeTable0F71 },
   /* 0F 72 */ { &decoder32_modrm, BxOpcodeTable0F72 },
   /* 0F 73 */ { &decoder32_modrm, BxOpcodeTable0F73 },
   /* 0F 74 */ { &decoder32_modrm, BxOpcodeTable0F74 },
   /* 0F 75 */ { &decoder32_modrm, BxOpcodeTable0F75 },
   /* 0F 76 */ { &decoder32_modrm, BxOpcodeTable0F76 },
   /* 0F 77 */ { &decoder32, BxOpcodeTable0F77 },
   /* 0F 78 */ { &decoder32_modrm, BxOpcodeTable0F78 },
   /* 0F 79 */ { &decoder32_modrm, BxOpcodeTable0F79 },
   /* 0F 7A */ { &decoder_ud32, NULL },
   /* 0F 7B */ { &decoder_ud32, NULL },
   /* 0F 7C */ { &decoder32_modrm, BxOpcodeTable0F7C },
   /* 0F 7D */ { &decoder32_modrm, BxOpcodeTable0F7D },
   /* 0F 7E */ { &decoder32_modrm, BxOpcodeTable0F7E },
   /* 0F 7F */ { &decoder32_modrm, BxOpcodeTable0F7F },
   /* 0F 80 */ { &decoder32, BxOpcodeTable0F80_32 },
   /* 0F 81 */ { &decoder32, BxOpcodeTable0F81_32 },
   /* 0F 82 */ { &decoder32, BxOpcodeTable0F82_32 },
   /* 0F 83 */ { &decoder32, BxOpcodeTable0F83_32 },
   /* 0F 84 */ { &decoder32, BxOpcodeTable0F84_32 },
   /* 0F 85 */ { &decoder32, BxOpcodeTable0F85_32 },
   /* 0F 86 */ { &decoder32, BxOpcodeTable0F86_32 },
   /* 0F 87 */ { &decoder32, BxOpcodeTable0F87_32 },
   /* 0F 88 */ { &decoder32, BxOpcodeTable0F88_32 },
   /* 0F 89 */ { &decoder32, BxOpcodeTable0F89_32 },
   /* 0F 8A */ { &decoder32, BxOpcodeTable0F8A_32 },
   /* 0F 8B */ { &decoder32, BxOpcodeTable0F8B_32 },
   /* 0F 8C */ { &decoder32, BxOpcodeTable0F8C_32 },
   /* 0F 8D */ { &decoder32, BxOpcodeTable0F8D_32 },
   /* 0F 8E */ { &decoder32, BxOpcodeTable0F8E_32 },
   /* 0F 8F */ { &decoder32, BxOpcodeTable0F8F_32 },
   /* 0F 90 */ { &decoder32_modrm, BxOpcodeTable0F90 },
   /* 0F 91 */ { &decoder32_modrm, BxOpcodeTable0F91 },
   /* 0F 92 */ { &decoder32_modrm, BxOpcodeTable0F92 },
   /* 0F 93 */ { &decoder32_modrm, BxOpcodeTable0F93 },
   /* 0F 94 */ { &decoder32_modrm, BxOpcodeTable0F94 },
   /* 0F 95 */ { &decoder32_modrm, BxOpcodeTable0F95 },
   /* 0F 96 */ { &decoder32_modrm, BxOpcodeTable0F96 },
   /* 0F 97 */ { &decoder32_modrm, BxOpcodeTable0F97 },
   /* 0F 98 */ { &decoder32_modrm, BxOpcodeTable0F98 },
   /* 0F 99 */ { &decoder32_modrm, BxOpcodeTable0F99 },
   /* 0F 9A */ { &decoder32_modrm, BxOpcodeTable0F9A },
   /* 0F 9B */ { &decoder32_modrm, BxOpcodeTable0F9B },
   /* 0F 9C */ { &decoder32_modrm, BxOpcodeTable0F9C },
   /* 0F 9D */ { &decoder32_modrm, BxOpcodeTable0F9D },
   /* 0F 9E */ { &decoder32_modrm, BxOpcodeTable0F9E },
   /* 0F 9F */ { &decoder32_modrm, BxOpcodeTable0F9F },
   /* 0F A0 */ { &decoder32, BxOpcodeTable0FA0 },
   /* 0F A1 */ { &decoder32, BxOpcodeTable0FA1 },
   /* 0F A2 */ { &decoder_simple32, BxOpcodeTable0FA2 },
   /* 0F A3 */ { &decoder32_modrm, BxOpcodeTable0FA3 },
   /* 0F A4 */ { &decoder32_modrm, BxOpcodeTable0FA4 },
   /* 0F A5 */ { &decoder32_modrm, BxOpcodeTable0FA5 },
   /* 0F A6 */ { &decoder_ud32, NULL },
   /* 0F A7 */ { &decoder_ud32, NULL },
   /* 0F A8 */ { &decoder32, BxOpcodeTable0FA8 },
   /* 0F A9 */ { &decoder32, BxOpcodeTable0FA9 },
   /* 0F AA */ { &decoder_simple32, BxOpcodeTable0FAA },
   /* 0F AB */ { &decoder32_modrm, BxOpcodeTable0FAB },
   /* 0F AC */ { &decoder32_modrm, BxOpcodeTable0FAC },
   /* 0F AD */ { &decoder32_modrm, BxOpcodeTable0FAD },
   /* 0F AE */ { &decoder32_modrm, BxOpcodeTable0FAE },
   /* 0F AF */ { &decoder32_modrm, BxOpcodeTable0FAF },
   /* 0F B0 */ { &decoder32_modrm, BxOpcodeTable0FB0 },
   /* 0F B1 */ { &decoder32_modrm, BxOpcodeTable0FB1 },
   /* 0F B2 */ { &decoder32_modrm, BxOpcodeTable0FB2 },
   /* 0F B3 */ { &decoder32_modrm, BxOpcodeTable0FB3 },
   /* 0F B4 */ { &decoder32_modrm, BxOpcodeTable0FB4 },
   /* 0F B5 */ { &decoder32_modrm, BxOpcodeTable0FB5 },
   /* 0F B6 */ { &decoder32_modrm, BxOpcodeTable0FB6 },
   /* 0F B7 */ { &decoder32_modrm, BxOpcodeTable0FB7 },
   /* 0F B8 */ { &decoder32_modrm, BxOpcodeTable0FB8 },
   /* 0F B9 */ { &decoder32_modrm, BxOpcodeTable0FB9 },
   /* 0F BA */ { &decoder32_modrm, BxOpcodeTable0FBA },
   /* 0F BB */ { &decoder32_modrm, BxOpcodeTable0FBB },
   /* 0F BC */ { &decoder32_modrm, BxOpcodeTable0FBC },
   /* 0F BD */ { &decoder32_modrm, BxOpcodeTable0FBD },
   /* 0F BE */ { &decoder32_modrm, BxOpcodeTable0FBE },
   /* 0F BF */ { &decoder32_modrm, BxOpcodeTable0FBF },
   /* 0F C0 */ { &decoder32_modrm, BxOpcodeTable0FC0 },
   /* 0F C1 */ { &decoder32_modrm, BxOpcodeTable0FC1 },
   /* 0F C2 */ { &decoder32_modrm, BxOpcodeTable0FC2 },
   /* 0F C3 */ { &decoder32_modrm, BxOpcodeTable0FC3 },
   /* 0F C4 */ { &decoder32_modrm, BxOpcodeTable0FC4 },
   /* 0F C5 */ { &decoder32_modrm, BxOpcodeTable0FC5 },
   /* 0F C6 */ { &decoder32_modrm, BxOpcodeTable0FC6 },
   /* 0F C7 */ { &decoder32_modrm, BxOpcodeTable0FC7 },
   /* 0F C8 */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F C9 */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CA */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CB */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CC */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CD */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CE */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F CF */ { &decoder32, BxOpcodeTable0FC8x0FCF },
   /* 0F D0 */ { &decoder32_modrm, BxOpcodeTable0FD0 },
   /* 0F D1 */ { &decoder32_modrm, BxOpcodeTable0FD1 },
   /* 0F D2 */ { &decoder32_modrm, BxOpcodeTable0FD2 },
   /* 0F D3 */ { &decoder32_modrm, BxOpcodeTable0FD3 },
   /* 0F D4 */ { &decoder32_modrm, BxOpcodeTable0FD4 },
   /* 0F D5 */ { &decoder32_modrm, BxOpcodeTable0FD5 },
   /* 0F D6 */ { &decoder32_modrm, BxOpcodeTable0FD6 },
   /* 0F D7 */ { &decoder32_modrm, BxOpcodeTable0FD7 },
   /* 0F D8 */ { &decoder32_modrm, BxOpcodeTable0FD8 },
   /* 0F D9 */ { &decoder32_modrm, BxOpcodeTable0FD9 },
   /* 0F DA */ { &decoder32_modrm, BxOpcodeTable0FDA },
   /* 0F DB */ { &decoder32_modrm, BxOpcodeTable0FDB },
   /* 0F DC */ { &decoder32_modrm, BxOpcodeTable0FDC },
   /* 0F DD */ { &decoder32_modrm, BxOpcodeTable0FDD },
   /* 0F DE */ { &decoder32_modrm, BxOpcodeTable0FDE },
   /* 0F DF */ { &decoder32_modrm, BxOpcodeTable0FDF },
   /* 0F E0 */ { &decoder32_modrm, BxOpcodeTable0FE0 },
   /* 0F E1 */ { &decoder32_modrm, BxOpcodeTable0FE1 },
   /* 0F E2 */ { &decoder32_modrm, BxOpcodeTable0FE2 },
   /* 0F E3 */ { &decoder32_modrm, BxOpcodeTable0FE3 },
   /* 0F E4 */ { &decoder32_modrm, BxOpcodeTable0FE4 },
   /* 0F E5 */ { &decoder32_modrm, BxOpcodeTable0FE5 },
   /* 0F E6 */ { &decoder32_modrm, BxOpcodeTable0FE6 },
   /* 0F E7 */ { &decoder32_modrm, BxOpcodeTable0FE7 },
   /* 0F E8 */ { &decoder32_modrm, BxOpcodeTable0FE8 },
   /* 0F E9 */ { &decoder32_modrm, BxOpcodeTable0FE9 },
   /* 0F EA */ { &decoder32_modrm, BxOpcodeTable0FEA },
   /* 0F EB */ { &decoder32_modrm, BxOpcodeTable0FEB },
   /* 0F EC */ { &decoder32_modrm, BxOpcodeTable0FEC },
   /* 0F ED */ { &decoder32_modrm, BxOpcodeTable0FED },
   /* 0F EE */ { &decoder32_modrm, BxOpcodeTable0FEE },
   /* 0F EF */ { &decoder32_modrm, BxOpcodeTable0FEF },
   /* 0F F0 */ { &decoder32_modrm, BxOpcodeTable0FF0 },
   /* 0F F1 */ { &decoder32_modrm, BxOpcodeTable0FF1 },
   /* 0F F2 */ { &decoder32_modrm, BxOpcodeTable0FF2 },
   /* 0F F3 */ { &decoder32_modrm, BxOpcodeTable0FF3 },
   /* 0F F4 */ { &decoder32_modrm, BxOpcodeTable0FF4 },
   /* 0F F5 */ { &decoder32_modrm, BxOpcodeTable0FF5 },
   /* 0F F6 */ { &decoder32_modrm, BxOpcodeTable0FF6 },
   /* 0F F7 */ { &decoder32_modrm, BxOpcodeTable0FF7 },
   /* 0F F8 */ { &decoder32_modrm, BxOpcodeTable0FF8 },
   /* 0F F9 */ { &decoder32_modrm, BxOpcodeTable0FF9 },
   /* 0F FA */ { &decoder32_modrm, BxOpcodeTable0FFA },
   /* 0F FB */ { &decoder32_modrm, BxOpcodeTable0FFB },
   /* 0F FC */ { &decoder32_modrm, BxOpcodeTable0FFC },
   /* 0F FD */ { &decoder32_modrm, BxOpcodeTable0FFD },
   /* 0F FE */ { &decoder32_modrm, BxOpcodeTable0FFE },
   /* 0F FF */ { &decoder_simple32, BxOpcodeTable0FFF }
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

extern int fetchImmediate(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, Bit16u ia_opcode, bool is_64);
extern Bit16u findOpcode(const Bit64u *opMap, Bit32u opMsk);

static const Bit8u *decodeModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm)
{
  unsigned seg = BX_SEG_REG_DS;

  // initialize displ32 with zero to include cases with no diplacement
  i->modRMForm.displ32u = 0;

  // note that mod==11b handled outside

  if (i->as32L()) {
    i->setSibBase(rm);
    i->setSibIndex(4); // no Index encoding by default
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

static const Bit8u *parseModrm32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, struct bx_modrm *modrm)
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

int fetchImmediate(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, Bit16u ia_opcode, bool is_64)
{
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = BX_DISASM_SRC_TYPE(src);
    src = BX_DISASM_SRC_ORIGIN(src);

    if (src == BX_SRC_IMM || src == BX_SRC_BRANCH_OFFSET) {
      switch(type) {
      case BX_IMM1:
        i->modRMForm.Ib[0] = 1;
        break;
      case BX_IMMB:
        if (remain != 0) {
          i->modRMForm.Ib[0] = *iptr++;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BX_IMMBW_SE: // Sign extend 8-bit to 16-bit
        if (remain != 0) {
          Bit8s temp8s = *iptr++;
          i->modRMForm.Iw[0] = (Bit16s) temp8s;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BX_IMMBD_SE: // Sign extend 8-bit to 32-bit
        if (remain != 0) {
          Bit8s temp8s = *iptr++;
          i->modRMForm.Id    = (Bit32s) temp8s;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BX_IMMB2:
        if (remain != 0) {
          i->modRMForm.Ib2[0] = *iptr++;
          remain--;
        }
        else {
          return(-1);
        }
        break;
      case BX_IMMW:
        if (remain > 1) {
          i->modRMForm.Iw[0] = FetchWORD(iptr);
          iptr += 2;
          remain -= 2;
        }
        else {
          return(-1);
        }
        break;
      case BX_IMMD:
        if (remain > 3) {
          i->modRMForm.Id = FetchDWORD(iptr);
          iptr += 4;
          remain -= 4;
        }
        else {
          return(-1);
        }
        break;
#if BX_SUPPORT_X86_64
      case BX_IMMQ:
        if (remain > 7) {
          i->IqForm.Iq = FetchQWORD(iptr);
          iptr += 8;
          remain -= 8;
        }
        else {
          return(-1);
        }
        break;
#endif
      case BX_DIRECT_PTR:
        // IdIw2 in 32-bit mode, IwIw2 in 16-bit mode
        if (i->os32L()) {
          if (remain > 3) {
            i->modRMForm.Id = FetchDWORD(iptr);
            iptr += 4;
            remain -= 4;
          }
          else {
            return(-1);
          }
        }
        else {
          if (remain > 1) {
            i->modRMForm.Iw[0] = FetchWORD(iptr);
            iptr += 2;
            remain -= 2;
          }
          else {
            return(-1);
          }
        }

        if (remain > 1) {
          i->modRMForm.Iw2[0] = FetchWORD(iptr);
          iptr += 2;
          remain -= 2;
        }
        else {
          return(-1);
        }
        break;

      case BX_DIRECT_MEMREF_B:
      case BX_DIRECT_MEMREF_W:
      case BX_DIRECT_MEMREF_D:
      case BX_DIRECT_MEMREF_Q:
#if BX_SUPPORT_X86_64
        if (is_64) {
          // For instructions which embed the address in the opcode.
          // There is only 64/32-bit addressing available in long64 mode.
          if (i->as64L()) {
            if (remain > 7) {
              i->IqForm.Iq = FetchQWORD(iptr);
              iptr += 8;
              remain -= 8;
            }
            else {
              return(-1);
            }
          }
          else { // as32
            if (remain > 3) {
              i->IqForm.Iq = (Bit64u) FetchDWORD(iptr);
              iptr += 4;
              remain -= 4;
            }
            else {
              return(-1);
            }
          }
        }
        else
#endif
        {
          if (i->as32L()) {
            // fetch 32bit address into Id
            if (remain > 3) {
              i->modRMForm.Id = FetchDWORD(iptr);
              iptr += 4;
              remain -= 4;
            }
            else {
              return(-1);
            }
          }
          else {
            // fetch 16bit address into Id
            if (remain > 1) {
              i->modRMForm.Id = (Bit32u) FetchWORD(iptr);
              iptr += 2;
              remain -= 2;
            }
            else {
              return(-1);
            }
          }
        }
        break;

      default:
        break;
      }
    }
    else {
      if (src == BX_SRC_VIB) {
        if (remain != 0) {
          i->modRMForm.Ib[0] = *iptr++;
          remain--;
        }
        else {
          return(-1);
        }
      }
    }
  }
  return 0;
}

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

  // VMOVDDUP special case
#if BX_SUPPORT_EVEX
  if ((ia_opcode == BX_IA_EVEX_VMOVDDUP_VpdWpd || ia_opcode == BX_IA_EVEX_VMOVDDUP_VpdWpd_Kmask) && (i->getVL() == BX_VL128))
    return 8;
#endif

  unsigned len = i->getVL();
  if (len == BX_NO_VL) len = BX_VL128;

  switch (type) {
  case BX_VMM_FULL_VECTOR:
#if BX_SUPPORT_EVEX
    if (i->getEvexb()) // broadcast
      return (4 << vex_w);
    else
#endif
      return (16 * len);

  case BX_VMM_FULL_VECTOR_W:
#if BX_SUPPORT_EVEX
    if (i->getEvexb()) // broadcast
      return 2;
    else
#endif
      return (16 * len);

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
#if BX_SUPPORT_EVEX
    if (i->getEvexb()) // broadcast
      return (4 << vex_w);
    else
#endif
      return (8 * len);

  case BX_VMM_HALF_VECTOR_W:
#if BX_SUPPORT_EVEX
    if (i->getEvexb()) // broadcast
      return 2;
    else
#endif
      return (8 * len);

  case BX_VMM_QUARTER_VECTOR:
    return (4 * len);

  case BX_VMM_QUARTER_VECTOR_W:
#if BX_SUPPORT_EVEX
    if (i->getEvexb()) // broadcast
      return 2;
    else
#endif
      return (4 * len);

  case BX_VMM_EIGHTH_VECTOR:
#if BX_SUPPORT_EVEX
    BX_ASSERT(! i->getEvexb());
#endif
    return (2 * len);

  case BX_VMM_VEC128:
    return 16;

  case BX_VMM_VEC256:
    return 32;
  }

  return 1;
}

BxDecodeError assign_srcs(bxInstruction_c *i, unsigned ia_opcode, unsigned nnn, unsigned rm)
{
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = BX_DISASM_SRC_TYPE(src);
    unsigned index = BX_DISASM_SRC_ORIGIN(src);
    switch(index) {
    case BX_SRC_NONE:
    case BX_SRC_IMM:
    case BX_SRC_BRANCH_OFFSET:
    case BX_SRC_IMPLICIT:
      break;
    case BX_SRC_EAX:
      i->setSrcReg(n, 0);
      break;
    case BX_SRC_NNN:
      i->setSrcReg(n, nnn);
      break;
    case BX_SRC_RM:
      if (i->modC0()) {
        i->setSrcReg(n, rm);
      }
      else {
        unsigned tmpreg = BX_TMP_REGISTER;
        if (type == BX_VMM_REG) tmpreg = BX_VECTOR_TMP_REGISTER;
        i->setSrcReg(n, tmpreg);
      }
      break;
    case BX_SRC_VECTOR_RM:
      if (i->modC0()) {
        i->setSrcReg(n, rm);
      }
      else {
        i->setSrcReg(n, BX_VECTOR_TMP_REGISTER);
      }
      break;
    default:
      BX_FATAL(("assign_srcs: unknown definition %d for src %d", src, n));
      break;
    }
  }

  return BX_DECODE_OK;
}

#if BX_SUPPORT_AVX
BxDecodeError assign_srcs(bxInstruction_c *i, unsigned ia_opcode, bool is_64, unsigned nnn, unsigned rm, unsigned vvv, unsigned vex_w, bool had_evex = false, bool displ8 = false)
{
  bool use_vvv = false;
#if BX_SUPPORT_EVEX
  unsigned displ8_scale = 1;
#endif

  // assign sources
  for (unsigned n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned type = BX_DISASM_SRC_TYPE(src);
    src = BX_DISASM_SRC_ORIGIN(src);
#if BX_SUPPORT_EVEX
    bool mem_src = false;
#endif

    switch(src) {
    case BX_SRC_NONE:
    case BX_SRC_IMM:
    case BX_SRC_BRANCH_OFFSET:
    case BX_SRC_IMPLICIT:
      break;
    case BX_SRC_EAX:
      i->setSrcReg(n, 0);
      break;
    case BX_SRC_NNN:
      i->setSrcReg(n, nnn);
#if BX_SUPPORT_EVEX
      if (type == BX_KMASK_REG) {
        if (nnn >= 8) return BX_EVEX_ILLEGAL_KMASK_REGISTER;
        // vector instruction using opmask as source or dest
        if (i->isZeroMasking())
          return BX_EVEX_ILLEGAL_ZERO_MASKING_WITH_KMASK_SRC_OR_DEST;
      }
#endif
#if BX_SUPPORT_AMX
      if (type == BX_TMM_REG) {
        if (nnn >= 8) return BX_AMX_ILLEGAL_TILE_REGISTER;
      }
#endif
      break;
    case BX_SRC_RM:
      if (i->modC0()) {
#if BX_SUPPORT_EVEX
        if (type == BX_KMASK_REG) {
          rm &= 0x7;
          // vector instruction using opmask as source or dest
          if (i->isZeroMasking())
            return BX_EVEX_ILLEGAL_ZERO_MASKING_WITH_KMASK_SRC_OR_DEST;
        }
#endif
#if BX_SUPPORT_AMX
        if (type == BX_TMM_REG) {
          if (rm >= 8) return BX_AMX_ILLEGAL_TILE_REGISTER;
        }
#endif
        i->setSrcReg(n, rm);
      }
      else {
#if BX_SUPPORT_EVEX
        mem_src = true;
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
        mem_src = true;
        if (n == 0) // zero masking is not allowed for memory destination
          if (i->isZeroMasking()) return BX_EVEX_ILLEGAL_ZERO_MASKING_MEMORY_DESTINATION;
#endif
      }
      break;
    case BX_SRC_VVV:
      i->setSrcReg(n, vvv);
      use_vvv = true;
#if BX_SUPPORT_EVEX
      if (type == BX_KMASK_REG) {
        if (vvv >= 8) return BX_EVEX_ILLEGAL_KMASK_REGISTER;
        // vector instruction using opmask as source or dest
        if (i->isZeroMasking())
          return BX_EVEX_ILLEGAL_ZERO_MASKING_WITH_KMASK_SRC_OR_DEST;
      }
#endif
#if BX_SUPPORT_AMX
      if (type == BX_TMM_REG) {
        if (vvv >= 8) return BX_AMX_ILLEGAL_TILE_REGISTER;
      }
#endif
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
      if (! i->as32L())
        return BX_VSIB_FORBIDDEN_ASIZE16;
      if (i->sibIndex() == BX_NIL_REGISTER)
        return BX_VSIB_ILLEGAL_SIB_INDEX;
#if BX_SUPPORT_EVEX
      i->setSibIndex(i->sibIndex() | (vvv & 0x10));
      // zero masking is not allowed for gather/scatter
      if (i->isZeroMasking()) return BX_EVEX_ILLEGAL_ZERO_MASKING_VSIB;
      mem_src = true;
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
    return BX_ILLEGAL_VEX_XOP_VVV;
  }

  return BX_DECODE_OK;
}
#endif

int decoder_vex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure VEX 0xC4 or VEX 0xC5
  assert((b1 & ~0x1) == 0xc4);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc0) != 0xc0) {
    return decoder32_modrm(iptr, remain, i, b1, sse_prefix, opcode_table);
  }

#if BX_SUPPORT_AVX
  unsigned rm = 0, nnn = 0;

  if (sse_prefix)
    return(BX_IA_ERROR);

  unsigned vex_w = 0;
  unsigned vex_opc_map = 1;
  unsigned vex = *iptr++;
  remain--;

  if (b1 == 0xc4) {
    // decode 3-byte VEX prefix
    vex_opc_map = vex & 0x1f;
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
  // there are instructions only from maps 1,2,3 for now in 32-bit mode
  if (vex_opc_map < 1 || vex_opc_map >= 4)
    return(ia_opcode);
  opcode_byte += 256 * vex_opc_map;
  bool has_modrm = (opcode_byte != 0x177); // if not VZEROUPPER/VZEROALL opcode
  opcode_byte -= 256;

  if (has_modrm) {
    // opcode requires modrm byte
    struct bx_modrm modrm;
    iptr = parseModrm32(iptr, remain, i, &modrm);
    if (! iptr)
      return(-1);

    nnn = modrm.nnn;
    rm  = modrm.rm;
  }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.

    rm = b1 & 0x7;
    nnn = (b1 >> 3) & 0x7;
    i->assertModC0();
  }

  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   (nnn << NNN_OFFSET) |
                   (rm << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vex_l << VEX_VL_128_256_OFFSET);
  if (i->modC0() && nnn == rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  ia_opcode = findOpcode(BxOpcodeTableVEX[opcode_byte], decmask);

  bool has_immediate = (opcode_byte >= 0x70 && opcode_byte <= 0x73) || (opcode_byte >= 0xC2 && opcode_byte <= 0xC6) || (opcode_byte >= 0x200);
  if (has_immediate) {
    if (remain != 0) {
      i->modRMForm.Ib[0] = *iptr;
      remain--;
    }
    else {
      return(-1);
    }
  }

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, false, nnn, rm, vvv, vex_w);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;
#endif

  return ia_opcode;
}

int decoder_evex32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure EVEX 0x62 prefix
  assert(b1 == 0x62);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc0) != 0xc0) {
    return decoder32_modrm(iptr, remain, i, b1, sse_prefix, opcode_table);
  }

#if BX_SUPPORT_EVEX
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

  // EVEX format: 0x62 P0 P1 P2
  // -----------

  //    7 6 5 4 3 2 1 0
  //    ---------------
  // P0 R X B R'0 m m m
  // P1 w v v v v u p p
  // P2 z L'L b V'a a a

  // EVEX.mmmm - opcode group
  // EVEX.pp   - compressed legacy SSE prefix
  // EVEX.RXB  - RXB to combine with MODRM.reg and MODRM.nnn
  // EVEX.R'   - combine with EVEX.R and MODRM.reg
  // EVEX.vvvv - save as VEX.vvvv
  // EVEX.V'   - combine with EVEX.vvvv or when VSIB present
  // EVEX.aaa  - embedded opmask register
  // EVEX.W    - opsize promotion / opcode extension
  // EVEX.z    - zero masking / merging
  // EVEX.b    - broadcast / round control / SAE
  // EVEX.LL   - vector length control

  // check for reserved EVEX bits
  if ((evex & 0x08) != 0)
    return(ia_opcode);

  // EVEX.U must be '1
  if ((evex & 0x400) == 0)
    return(ia_opcode);

  unsigned evex_opc_map = evex & 0x7;
  if (evex_opc_map == 0 || evex_opc_map == 4 || evex_opc_map == 7)
    return(ia_opcode);
  if (evex_opc_map >= 4) evex_opc_map--; // skipped map4 in the table

  sse_prefix = (evex >> 8) & 0x3;
  int vvv = 15 - ((evex >> 11) & 0xf);
  if (vvv >= 8)
    return(ia_opcode);

  unsigned vex_w = (evex >> 15) & 0x1;
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
  opcode_byte += 256 * (evex_opc_map-1);

  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr)
    return(-1);

  bool displ8 = (modrm.mod == 0x40); // mod==01b

  if (modrm.mod == 0xc0) {
    // EVEX.b in reg form implies 512-bit vector length
    if (i->getEvexb()) i->setVL(BX_VL512);
  }

  Bit32u vl = i->getVL()-1; // 0: VL128, 1: VL256, 3: VL512
  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   (modrm.nnn << NNN_OFFSET) |
                   (modrm.rm << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vl << VEX_VL_128_256_OFFSET);
  if (i->modC0() && modrm.nnn == modrm.rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);
  if (!opmask)
    decmask |= (1 << MASK_K0_OFFSET);

  ia_opcode = findOpcode(BxOpcodeTableEVEX[opcode_byte], decmask);

  bool has_immediate = (opcode_byte >= 0x70 && opcode_byte <= 0x73) || (opcode_byte >= 0xC2 && opcode_byte <= 0xC6) || (opcode_byte >= 0x200 && opcode_byte < 0x300);
  if (has_immediate) {
    if (remain != 0) {
      i->modRMForm.Ib[0] = *iptr;
      remain--;
    }
    else {
      return(-1);
    }
  }

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, false, modrm.nnn, modrm.rm, vvv, vex_w, true, displ8);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;

  // EVEX specific #UD conditions
  if (i->getVL() > BX_VL512) {
    ia_opcode = BX_IA_ERROR;
  }
#endif

  return ia_opcode;
}

int decoder_xop32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // make sure XOP 0x8f prefix
  assert(b1 == 0x8f);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0xc8) != 0xc8) {
    // not XOP prefix, decode regular opcode
    return decoder32_modrm(iptr, remain, i, b1, sse_prefix, opcode_table);
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

  unsigned vex_w = (vex >> 7) & 0x1;
  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  i->setVexW(vex_w);
  sse_prefix = vex & 0x3;
  if (sse_prefix)
    return(ia_opcode);

  unsigned opcode_byte = *iptr++;
  opcode_byte += 256 * xop_opcext;

  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr)
    return(-1);

  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   (modrm.nnn << NNN_OFFSET) |
                   (modrm.rm << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vex_l << VEX_VL_128_256_OFFSET);
  if (i->modC0() && modrm.nnn == modrm.rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  ia_opcode = findOpcode(BxOpcodeTableXOP[opcode_byte], decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, false) < 0)
    return (-1);

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, false, modrm.nnn, modrm.rm, vvv, vex_w);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;
#endif

  return ia_opcode;
}

int decoder32_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
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

int decoder32_modrm(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr)
    return(-1);

  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   (modrm.nnn << NNN_OFFSET) |
                   (modrm.rm << RRR_OFFSET);
  if (i->modC0() && modrm.nnn == modrm.rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  Bit16u ia_opcode = findOpcode((Bit64u *) opcode_table, decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, false) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  unsigned rm = b1 & 0x7;
  unsigned nnn = (b1 >> 3) & 0x7;
  i->assertModC0();

  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (1 << MODC0_OFFSET);
  if (nnn == rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  Bit16u ia_opcode = findOpcode((Bit64u *) opcode_table, decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, false) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder_creg32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  // MOVs with CRx and DRx always use register ops and ignore the mod field.
  assert((b1 & ~7) == 0x120);

  // opcode requires modrm byte
  if (remain == 0)
    return(-1);
  remain--;
  unsigned b2 = *iptr++; // fetch modrm byte

  // Parse mod-nnn-rm and related bytes
  unsigned nnn = (b2 >> 3) & 0x7;
  unsigned rm  = b2 & 0x7;

  i->assertModC0();

  Bit32u decmask = (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (1 << MODC0_OFFSET) |
                   (nnn << NNN_OFFSET) |
                   (rm << RRR_OFFSET);

  Bit16u ia_opcode = findOpcode((Bit64u *) opcode_table, decmask);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder32_3dnow(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  Bit16u ia_opcode = BX_IA_ERROR;

#if BX_SUPPORT_3DNOW
  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm32(iptr, remain, i, &modrm);
  if (! iptr)
    return(-1);

  if (remain != 0) {
    i->modRMForm.Ib[0] = *iptr;
    remain--;
  }
  else {
    return(-1);
  }

  ia_opcode = Bx3DNowOpcode[i->modRMForm.Ib[0]];

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);
#endif

  return ia_opcode;
}

int decoder32_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  assert(b1 == 0x90);

  i->assertModC0();

  if (sse_prefix == SSE_PREFIX_F3)
    return BX_IA_PAUSE;
  else
    return BX_IA_NOP;
}

int decoder_simple32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  i->assertModC0();

  const Bit64u *op = (const Bit64u *) opcode_table;

  // no immediate expected, no sources expected, take first opcode
  // check attributes ?
  Bit16u ia_opcode = Bit16u(*op >> 48) & 0x7FFF; // upper bit indicates that parsing is done and doesn't belong to opcode
  return ia_opcode;
}

int decoder_ud32(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, const void *opcode_table)
{
  return BX_IA_ERROR;
}

Bit16u findOpcode(const Bit64u *opMap, Bit32u decmask)
{
  Bit16u ia_opcode = BX_IA_ERROR;
  Bit64u op;

  do {
    op = *opMap++;
    Bit32u ignmsk = Bit32u(op) & 0xFFFFFF;
    Bit32u opmsk  = Bit32u(op >> 24);
    if ((opmsk & ignmsk) == (decmask & ignmsk)) {
      ia_opcode = Bit16u(op >> 48) & 0x7FFF; // upper bit indicates that parsing is done and doesn't belong to opcode
      break;
    }
  } while(Bit64s(op) > 0);

  return ia_opcode;
}

int fetchDecode32(const Bit8u *iptr, bool is_32, bxInstruction_c *i, unsigned remainingInPage)
{
  if (remainingInPage > 15) remainingInPage = 15;
  i->setILen(remainingInPage);

  unsigned remain = remainingInPage; // remain must be at least 1
  unsigned b1;
  int ia_opcode = BX_IA_ERROR;
  unsigned seg_override = BX_SEG_REG_NULL;
#if BX_SUPPORT_CET
  unsigned seg_override_cet = BX_SEG_REG_NULL;
#endif
  bool os_32 = is_32, lock = 0;
  unsigned sse_prefix = SSE_PREFIX_NONE;

  i->init(/*os32*/ is_32,  /*as32*/ is_32,
          /*os64*/     0,  /*as64*/     0);

fetch_b1:
  b1 = *iptr++;
  remain--;

#if BX_SUPPORT_CET
  // DS prefix is still recorded for CET Endbranch suppress hint even if overridden by other prefixes later
  if (b1 == 0x3e)
    seg_override_cet = BX_SEG_REG_DS;
#endif

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

  BxOpcodeDecodeDescriptor32 *decode_descriptor = &decode32_descriptor[b1];
  BxFetchDecode32Ptr decode_method = decode_descriptor->decode_method;
  const void *opcode_table = decode_descriptor->opcode_table;

#if BX_CPU_LEVEL >= 6
  if (b1 == 0x138 || b1 == 0x13a) {
    if (remain == 0)
      return(-1);
    Bit8u opcode = *iptr++;
    if (b1 == 0x138) {
      opcode_table = BxOpcodeTable0F38[opcode];
      b1 = 0x200 | opcode;
    }
    else if (b1 == 0x13a) {
      opcode_table = BxOpcodeTable0F3A[opcode];
      b1 = 0x300 | opcode;
    }
    remain--;
  }
#endif

  i->setSeg(BX_SEG_REG_DS); // default segment is DS:
#if BX_SUPPORT_CET
  i->setCetSegOverride(seg_override_cet);
#endif

  i->modRMForm.Id = 0;

  ia_opcode = (*decode_method)(iptr, remain, i, b1, sse_prefix, opcode_table);
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

int BX_CPU_C::assignHandler(bxInstruction_c *i, Bit32u fetchModeMask)
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
  if ((op_flags & BX_PREPARE_EVEX) != 0) {
    if (i->getEvexb()) {
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
  }
#endif

  if (! (fetchModeMask & BX_FETCH_MODE_FPU_MMX_OK)) {
     if (op_flags & BX_PREPARE_FPU) {
        if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoFPU;
        return(1);
     }
     if (op_flags & BX_PREPARE_MMX) {
        if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoMMX;
        return(1);
     }
  }
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
#if BX_SUPPORT_AMX
  if (! (fetchModeMask & BX_FETCH_MODE_AMX_OK)) {
    if (op_flags & BX_PREPARE_AMX) {
       if (i->execute1 != &BX_CPU_C::BxError) i->execute1 = &BX_CPU_C::BxNoAMX;
       return(1);
    }
  }
#endif
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
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) f,
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
      case BX_IA_MASKMOVQ_PqNq:
      case BX_IA_MOVNTQ_MqPq:
      case BX_IA_PAVGB_PqQq:
      case BX_IA_PAVGW_PqQq:
      case BX_IA_PEXTRW_GdNqIb:
      case BX_IA_PINSRW_PqEwIb:
      case BX_IA_PMAXSW_PqQq:
      case BX_IA_PMAXUB_PqQq:
      case BX_IA_PMINSW_PqQq:
      case BX_IA_PMINUB_PqQq:
      case BX_IA_PMOVMSKB_GdNq:
      case BX_IA_PMULHUW_PqQq:
      case BX_IA_PSADBW_PqQq:
      case BX_IA_PSHUFW_PqQqIb:
      case BX_IA_SFENCE:
        if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_3DNOW_EXT)) continue;

      default: break;
    }

    unsigned ia_opcode_feature = BxOpcodeFeatures[n];
    if (! BX_CPUID_SUPPORT_ISA_EXTENSION(ia_opcode_feature)) {
      switch(ia_opcode_feature) {
        case BX_ISA_AVX512:
        case BX_ISA_AVX512_DQ:
        case BX_ISA_AVX512_BW:
        case BX_ISA_AVX512_CD:
        case BX_ISA_AVX512_VBMI:
        case BX_ISA_AVX512_VBMI2:
        case BX_ISA_AVX512_IFMA52:
        case BX_ISA_AVX512_VPOPCNTDQ:
        case BX_ISA_AVX512_VNNI:
        case BX_ISA_AVX512_BITALG:
        case BX_ISA_AVX512_BF16:
        case BX_ISA_AVX512_FP16:
          // It is possible that AVX512 is not supported on this processor but AVX10 is (for example AVX10_VL256 only)
          // AVX10_1 includes all above AVX512 extensions
          if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_AVX10_1)) continue;

        default: break;
      }
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

#if BX_SUPPORT_EVEX
  // EVEX extensions for SM4 require AVX10_2
  if (!BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_SM4) || !BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_AVX10_2)) {
    BxOpcodesTable[BX_IA_EVEX_VSM4KEY4_VdqHdqWdq].execute1 = &BX_CPU_C::BxError;
    BxOpcodesTable[BX_IA_EVEX_VSM4KEY4_VdqHdqWdq].opflags = 0;   // won't allow this new #UD opcode to check prepare_EVEX

    BxOpcodesTable[BX_IA_EVEX_VSM4RNDS4_VdqHdqWdq].execute1 = &BX_CPU_C::BxError;
    BxOpcodesTable[BX_IA_EVEX_VSM4RNDS4_VdqHdqWdq].opflags = 0;  // won't allow this new #UD opcode to check prepare_EVEX
  }
#endif
}

#endif

const char *get_bx_opcode_name(Bit16u ia_opcode)
{
  static const char* BxOpcodeNamesTable[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) #a,
#include "ia_opcodes.def"
  };
#undef  bx_define_opcode

  return (ia_opcode < BX_IA_LAST) ? BxOpcodeNamesTable[ia_opcode] : 0;
}

const char *get_intel_disasm_opcode_name(Bit16u ia_opcode)
{
  static const char* BxOpcodeNamesTable[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) b,
#include "ia_opcodes.def"
  };
#undef  bx_define_opcode

  return (ia_opcode < BX_IA_LAST) ? BxOpcodeNamesTable[ia_opcode] : 0;
}

const char *get_gas_disasm_opcode_name(Bit16u ia_opcode)
{
  static const char* BxOpcodeNamesTable[BX_IA_LAST] =
  {
#define bx_define_opcode(a, b, c, d, e, f, s1, s2, s3, s4, g) c,
#include "ia_opcodes.def"
  };
#undef  bx_define_opcode

  return (ia_opcode < BX_IA_LAST) ? BxOpcodeNamesTable[ia_opcode] : 0;
}

