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

#include "instr.h"
#include "decoder.h"

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

// table of all Bochs opcodes
extern struct bxIAOpcodeTable BxOpcodesTable[];

extern Bit16u findOpcode(const Bit64u *opMap, Bit32u opMsk);

extern BxDecodeError assign_srcs(bxInstruction_c *i, unsigned ia_opcode, unsigned nnn, unsigned rm);
#if BX_SUPPORT_AVX
extern BxDecodeError assign_srcs(bxInstruction_c *i, unsigned ia_opcode, bool is_64, unsigned nnn, unsigned rm, unsigned vvv, unsigned vex_w, bool had_evex = false, bool displ8 = false);
#endif

extern int fetchImmediate(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, Bit16u ia_opcode, bool is_64);

extern int decoder_simple64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder_creg64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder64_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder64_3dnow(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder_vex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder_evex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder_xop64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder_ud64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder64_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);
extern int decoder64_modrm(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);

typedef int (*BxFetchDecode64Ptr)(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table);

struct BxOpcodeDecodeDescriptor64 {
  BxFetchDecode64Ptr decode_method;
  const void *opcode_table;
};

static BxOpcodeDecodeDescriptor64 decode64_descriptor[] =
{
   /*    00 */ { &decoder64_modrm, BxOpcodeTable00 },
   /*    01 */ { &decoder64_modrm, BxOpcodeTable01 },
   /*    02 */ { &decoder64_modrm, BxOpcodeTable02 },
   /*    03 */ { &decoder64_modrm, BxOpcodeTable03 },
   /*    04 */ { &decoder64, BxOpcodeTable04 },
   /*    05 */ { &decoder64, BxOpcodeTable05 },
   /*    06 */ { &decoder_ud64, NULL },
   /*    07 */ { &decoder_ud64, NULL },
   /*    08 */ { &decoder64_modrm, BxOpcodeTable08 },
   /*    09 */ { &decoder64_modrm, BxOpcodeTable09 },
   /*    0A */ { &decoder64_modrm, BxOpcodeTable0A },
   /*    0B */ { &decoder64_modrm, BxOpcodeTable0B },
   /*    0C */ { &decoder64, BxOpcodeTable0C },
   /*    0D */ { &decoder64, BxOpcodeTable0D },
   /*    0E */ { &decoder_ud64, NULL },
   /*    0F */ { &decoder_ud64, NULL },             // 2-byte escape
   /*    10 */ { &decoder64_modrm, BxOpcodeTable10 },
   /*    11 */ { &decoder64_modrm, BxOpcodeTable11 },
   /*    12 */ { &decoder64_modrm, BxOpcodeTable12 },
   /*    13 */ { &decoder64_modrm, BxOpcodeTable13 },
   /*    14 */ { &decoder64, BxOpcodeTable14 },
   /*    15 */ { &decoder64, BxOpcodeTable15 },
   /*    16 */ { &decoder_ud64, NULL },
   /*    17 */ { &decoder_ud64, NULL },
   /*    18 */ { &decoder64_modrm, BxOpcodeTable18 },
   /*    19 */ { &decoder64_modrm, BxOpcodeTable19 },
   /*    1A */ { &decoder64_modrm, BxOpcodeTable1A },
   /*    1B */ { &decoder64_modrm, BxOpcodeTable1B },
   /*    1C */ { &decoder64, BxOpcodeTable1C },
   /*    1D */ { &decoder64, BxOpcodeTable1D },
   /*    1E */ { &decoder_ud64, NULL },
   /*    1F */ { &decoder_ud64, NULL },
   /*    20 */ { &decoder64_modrm, BxOpcodeTable20 },
   /*    21 */ { &decoder64_modrm, BxOpcodeTable21 },
   /*    22 */ { &decoder64_modrm, BxOpcodeTable22 },
   /*    23 */ { &decoder64_modrm, BxOpcodeTable23 },
   /*    24 */ { &decoder64, BxOpcodeTable24 },
   /*    25 */ { &decoder64, BxOpcodeTable25 },
   /*    26 */ { &decoder_ud64, NULL },             // ES:
   /*    27 */ { &decoder_ud64, NULL },
   /*    28 */ { &decoder64_modrm, BxOpcodeTable28 },
   /*    29 */ { &decoder64_modrm, BxOpcodeTable29 },
   /*    2A */ { &decoder64_modrm, BxOpcodeTable2A },
   /*    2B */ { &decoder64_modrm, BxOpcodeTable2B },
   /*    2C */ { &decoder64, BxOpcodeTable2C },
   /*    2D */ { &decoder64, BxOpcodeTable2D },
   /*    2E */ { &decoder_ud64, NULL },             // CS:
   /*    2F */ { &decoder_ud64, NULL },
   /*    30 */ { &decoder64_modrm, BxOpcodeTable30 },
   /*    31 */ { &decoder64_modrm, BxOpcodeTable31 },
   /*    32 */ { &decoder64_modrm, BxOpcodeTable32 },
   /*    33 */ { &decoder64_modrm, BxOpcodeTable33 },
   /*    34 */ { &decoder64, BxOpcodeTable34 },
   /*    35 */ { &decoder64, BxOpcodeTable35 },
   /*    36 */ { &decoder_ud64, NULL },             // SS:
   /*    37 */ { &decoder_ud64, NULL },
   /*    38 */ { &decoder64_modrm, BxOpcodeTable38 },
   /*    39 */ { &decoder64_modrm, BxOpcodeTable39 },
   /*    3A */ { &decoder64_modrm, BxOpcodeTable3A },
   /*    3B */ { &decoder64_modrm, BxOpcodeTable3B },
   /*    3C */ { &decoder64, BxOpcodeTable3C },
   /*    3D */ { &decoder64, BxOpcodeTable3D },
   /*    3E */ { &decoder_ud64, NULL },             // DS:
   /*    3F */ { &decoder_ud64, NULL },
   /*    40 */ { &decoder_ud64, NULL },             // REX prefix
   /*    41 */ { &decoder_ud64, NULL },             // REX prefix
   /*    42 */ { &decoder_ud64, NULL },             // REX prefix
   /*    43 */ { &decoder_ud64, NULL },             // REX prefix
   /*    44 */ { &decoder_ud64, NULL },             // REX prefix
   /*    45 */ { &decoder_ud64, NULL },             // REX prefix
   /*    46 */ { &decoder_ud64, NULL },             // REX prefix
   /*    47 */ { &decoder_ud64, NULL },             // REX prefix
   /*    48 */ { &decoder_ud64, NULL },             // REX prefix
   /*    49 */ { &decoder_ud64, NULL },             // REX prefix
   /*    4A */ { &decoder_ud64, NULL },             // REX prefix
   /*    4B */ { &decoder_ud64, NULL },             // REX prefix
   /*    4C */ { &decoder_ud64, NULL },             // REX prefix
   /*    4D */ { &decoder_ud64, NULL },             // REX prefix
   /*    4E */ { &decoder_ud64, NULL },             // REX prefix
   /*    4F */ { &decoder_ud64, NULL },             // REX prefix
   /*    50 */ { &decoder64, BxOpcodeTable50x57 },
   /*    51 */ { &decoder64, BxOpcodeTable50x57 },
   /*    52 */ { &decoder64, BxOpcodeTable50x57 },
   /*    53 */ { &decoder64, BxOpcodeTable50x57 },
   /*    54 */ { &decoder64, BxOpcodeTable50x57 },
   /*    55 */ { &decoder64, BxOpcodeTable50x57 },
   /*    56 */ { &decoder64, BxOpcodeTable50x57 },
   /*    57 */ { &decoder64, BxOpcodeTable50x57 },
   /*    58 */ { &decoder64, BxOpcodeTable58x5F },
   /*    59 */ { &decoder64, BxOpcodeTable58x5F },
   /*    5A */ { &decoder64, BxOpcodeTable58x5F },
   /*    5B */ { &decoder64, BxOpcodeTable58x5F },
   /*    5C */ { &decoder64, BxOpcodeTable58x5F },
   /*    5D */ { &decoder64, BxOpcodeTable58x5F },
   /*    5E */ { &decoder64, BxOpcodeTable58x5F },
   /*    5F */ { &decoder64, BxOpcodeTable58x5F },
   /*    60 */ { &decoder_ud64, NULL },
   /*    61 */ { &decoder_ud64, NULL },
   /*    62 */ { &decoder_evex64, NULL },         // EVEX prefix
   /*    63 */ { &decoder64_modrm, BxOpcodeTable63_64 },
   /*    64 */ { &decoder_ud64, NULL },           // FS:
   /*    65 */ { &decoder_ud64, NULL },           // GS:
   /*    66 */ { &decoder_ud64, NULL },           // OSIZE:
   /*    67 */ { &decoder_ud64, NULL },           // ASIZE:
   /*    68 */ { &decoder64, BxOpcodeTable68 },
   /*    69 */ { &decoder64_modrm, BxOpcodeTable69 },
   /*    6A */ { &decoder64, BxOpcodeTable6A },
   /*    6B */ { &decoder64_modrm, BxOpcodeTable6B },
   /*    6C */ { &decoder64, BxOpcodeTable6C },
   /*    6D */ { &decoder64, BxOpcodeTable6D },
   /*    6E */ { &decoder64, BxOpcodeTable6E },
   /*    6F */ { &decoder64, BxOpcodeTable6F },
   /*    70 */ { &decoder64, BxOpcodeTable70_64 },
   /*    71 */ { &decoder64, BxOpcodeTable71_64 },
   /*    72 */ { &decoder64, BxOpcodeTable72_64 },
   /*    73 */ { &decoder64, BxOpcodeTable73_64 },
   /*    74 */ { &decoder64, BxOpcodeTable74_64 },
   /*    75 */ { &decoder64, BxOpcodeTable75_64 },
   /*    76 */ { &decoder64, BxOpcodeTable76_64 },
   /*    77 */ { &decoder64, BxOpcodeTable77_64 },
   /*    78 */ { &decoder64, BxOpcodeTable78_64 },
   /*    79 */ { &decoder64, BxOpcodeTable79_64 },
   /*    7A */ { &decoder64, BxOpcodeTable7A_64 },
   /*    7B */ { &decoder64, BxOpcodeTable7B_64 },
   /*    7C */ { &decoder64, BxOpcodeTable7C_64 },
   /*    7D */ { &decoder64, BxOpcodeTable7D_64 },
   /*    7E */ { &decoder64, BxOpcodeTable7E_64 },
   /*    7F */ { &decoder64, BxOpcodeTable7F_64 },
   /*    80 */ { &decoder64_modrm, BxOpcodeTable80 },
   /*    81 */ { &decoder64_modrm, BxOpcodeTable81 },
   /*    82 */ { &decoder_ud64, NULL },
   /*    83 */ { &decoder64_modrm, BxOpcodeTable83 },
   /*    84 */ { &decoder64_modrm, BxOpcodeTable84 },
   /*    85 */ { &decoder64_modrm, BxOpcodeTable85 },
   /*    86 */ { &decoder64_modrm, BxOpcodeTable86 },
   /*    87 */ { &decoder64_modrm, BxOpcodeTable87 },
   /*    88 */ { &decoder64_modrm, BxOpcodeTable88 },
   /*    89 */ { &decoder64_modrm, BxOpcodeTable89 },
   /*    8A */ { &decoder64_modrm, BxOpcodeTable8A },
   /*    8B */ { &decoder64_modrm, BxOpcodeTable8B },
   /*    8C */ { &decoder64_modrm, BxOpcodeTable8C },
   /*    8D */ { &decoder64_modrm, BxOpcodeTable8D },
   /*    8E */ { &decoder64_modrm, BxOpcodeTable8E },
   /*    8F */ { &decoder_xop64, BxOpcodeTable8F },          // XOP prefix
   /*    90 */ { &decoder64_nop, BxOpcodeTable90x97 },
   /*    91 */ { &decoder64, BxOpcodeTable90x97 },
   /*    92 */ { &decoder64, BxOpcodeTable90x97 },
   /*    93 */ { &decoder64, BxOpcodeTable90x97 },
   /*    94 */ { &decoder64, BxOpcodeTable90x97 },
   /*    95 */ { &decoder64, BxOpcodeTable90x97 },
   /*    96 */ { &decoder64, BxOpcodeTable90x97 },
   /*    97 */ { &decoder64, BxOpcodeTable90x97 },
   /*    98 */ { &decoder64, BxOpcodeTable98 },
   /*    99 */ { &decoder64, BxOpcodeTable99 },
   /*    9A */ { &decoder_ud64, NULL },
   /*    9B */ { &decoder_simple64, BxOpcodeTable9B },
   /*    9C */ { &decoder64, BxOpcodeTable9C },
   /*    9D */ { &decoder64, BxOpcodeTable9D },
   /*    9E */ { &decoder_simple64, BxOpcodeTable9E_64 },
   /*    9F */ { &decoder_simple64, BxOpcodeTable9F_64 },
   /*    A0 */ { &decoder64, BxOpcodeTableA0_64 },
   /*    A1 */ { &decoder64, BxOpcodeTableA1_64 },
   /*    A2 */ { &decoder64, BxOpcodeTableA2_64 },
   /*    A3 */ { &decoder64, BxOpcodeTableA3_64 },
   /*    A4 */ { &decoder64, BxOpcodeTableA4 },
   /*    A5 */ { &decoder64, BxOpcodeTableA5 },
   /*    A6 */ { &decoder64, BxOpcodeTableA6 },
   /*    A7 */ { &decoder64, BxOpcodeTableA7 },
   /*    A8 */ { &decoder64, BxOpcodeTableA8 },
   /*    A9 */ { &decoder64, BxOpcodeTableA9 },
   /*    AA */ { &decoder64, BxOpcodeTableAA },
   /*    AB */ { &decoder64, BxOpcodeTableAB },
   /*    AC */ { &decoder64, BxOpcodeTableAC },
   /*    AD */ { &decoder64, BxOpcodeTableAD },
   /*    AE */ { &decoder64, BxOpcodeTableAE },
   /*    AF */ { &decoder64, BxOpcodeTableAF },
   /*    B0 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B1 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B2 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B3 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B4 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B5 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B6 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B7 */ { &decoder64, BxOpcodeTableB0xB7 },
   /*    B8 */ { &decoder64, BxOpcodeTableB8xBF },
   /*    B9 */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BA */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BB */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BC */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BD */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BE */ { &decoder64, BxOpcodeTableB8xBF },
   /*    BF */ { &decoder64, BxOpcodeTableB8xBF },
   /*    C0 */ { &decoder64_modrm, BxOpcodeTableC0 },
   /*    C1 */ { &decoder64_modrm, BxOpcodeTableC1 },
   /*    C2 */ { &decoder64, BxOpcodeTableC2_64 },
   /*    C3 */ { &decoder64, BxOpcodeTableC3_64 },
   /*    C4 */ { &decoder_vex64, NULL },            // VEX prefix
   /*    C5 */ { &decoder_vex64, NULL },            // VEX prefix
   /*    C6 */ { &decoder64_modrm, BxOpcodeTableC6 },
   /*    C7 */ { &decoder64_modrm, BxOpcodeTableC7 },
   /*    C8 */ { &decoder64, BxOpcodeTableC8_64 },
   /*    C9 */ { &decoder64, BxOpcodeTableC9_64 },
   /*    CA */ { &decoder64, BxOpcodeTableCA },
   /*    CB */ { &decoder64, BxOpcodeTableCB },
   /*    CC */ { &decoder_simple64, BxOpcodeTableCC },
   /*    CD */ { &decoder64, BxOpcodeTableCD },
   /*    CE */ { &decoder_ud64, NULL },
   /*    CF */ { &decoder64, BxOpcodeTableCF_64 },
   /*    D0 */ { &decoder64_modrm, BxOpcodeTableD0 },
   /*    D1 */ { &decoder64_modrm, BxOpcodeTableD1 },
   /*    D2 */ { &decoder64_modrm, BxOpcodeTableD2 },
   /*    D3 */ { &decoder64_modrm, BxOpcodeTableD3 },
   /*    D4 */ { &decoder_ud64, NULL },
   /*    D5 */ { &decoder_ud64, NULL },
   /*    D6 */ { &decoder_ud64, NULL },
   /*    D7 */ { &decoder_simple64, BxOpcodeTableD7 },
   /*    D8 */ { &decoder64_fp_escape, NULL },
   /*    D9 */ { &decoder64_fp_escape, NULL },
   /*    DA */ { &decoder64_fp_escape, NULL },
   /*    DB */ { &decoder64_fp_escape, NULL },
   /*    DC */ { &decoder64_fp_escape, NULL },
   /*    DD */ { &decoder64_fp_escape, NULL },
   /*    DE */ { &decoder64_fp_escape, NULL },
   /*    DF */ { &decoder64_fp_escape, NULL },
   /*    E0 */ { &decoder64, BxOpcodeTableE0_64 },
   /*    E1 */ { &decoder64, BxOpcodeTableE1_64 },
   /*    E2 */ { &decoder64, BxOpcodeTableE2_64 },
   /*    E3 */ { &decoder64, BxOpcodeTableE3_64 },
   /*    E4 */ { &decoder64, BxOpcodeTableE4 },
   /*    E5 */ { &decoder64, BxOpcodeTableE5 },
   /*    E6 */ { &decoder64, BxOpcodeTableE6 },
   /*    E7 */ { &decoder64, BxOpcodeTableE7 },
   /*    E8 */ { &decoder64, BxOpcodeTableE8_64 },
   /*    E9 */ { &decoder64, BxOpcodeTableE9_64},
   /*    EA */ { &decoder_ud64, NULL },
   /*    EB */ { &decoder64, BxOpcodeTableEB_64 },
   /*    EC */ { &decoder64, BxOpcodeTableEC },
   /*    ED */ { &decoder64, BxOpcodeTableED },
   /*    EE */ { &decoder64, BxOpcodeTableEE },
   /*    EF */ { &decoder64, BxOpcodeTableEF },
   /*    F0 */ { &decoder_ud64, NULL },           // LOCK
   /*    F1 */ { &decoder_simple64, BxOpcodeTableF1 },
   /*    F2 */ { &decoder_ud64, NULL },           // REPNE/REPNZ
   /*    F3 */ { &decoder_ud64, NULL },           // REP, REPE/REPZ
   /*    F4 */ { &decoder_simple64, BxOpcodeTableF4 },
   /*    F5 */ { &decoder_simple64, BxOpcodeTableF5 },
   /*    F6 */ { &decoder64_modrm, BxOpcodeTableF6 },
   /*    F7 */ { &decoder64_modrm, BxOpcodeTableF7 },
   /*    F8 */ { &decoder_simple64, BxOpcodeTableF8 },
   /*    F9 */ { &decoder_simple64, BxOpcodeTableF9 },
   /*    FA */ { &decoder_simple64, BxOpcodeTableFA },
   /*    FB */ { &decoder_simple64, BxOpcodeTableFB },
   /*    FC */ { &decoder_simple64, BxOpcodeTableFC },
   /*    FD */ { &decoder_simple64, BxOpcodeTableFD },
   /*    FE */ { &decoder64_modrm, BxOpcodeTableFE },
   /*    FF */ { &decoder64_modrm, BxOpcodeTableFF },
   /* 0F 00 */ { &decoder64_modrm, BxOpcodeTable0F00 },
   /* 0F 01 */ { &decoder64_modrm, BxOpcodeTable0F01 },
   /* 0F 02 */ { &decoder64_modrm, BxOpcodeTable0F02 },
   /* 0F 03 */ { &decoder64_modrm, BxOpcodeTable0F03 },
   /* 0F 04 */ { &decoder_ud64, NULL },
   /* 0F 05 */ { &decoder_simple64, BxOpcodeTable0F05_64 },
   /* 0F 06 */ { &decoder_simple64, BxOpcodeTable0F06 },
   /* 0F 07 */ { &decoder_simple64, BxOpcodeTable0F07_64 },
   /* 0F 08 */ { &decoder_simple64, BxOpcodeTable0F08 },
   /* 0F 09 */ { &decoder_simple64, BxOpcodeTable0F09 },
   /* 0F 0A */ { &decoder_ud64, NULL },
   /* 0F 0B */ { &decoder_simple64, BxOpcodeTable0F0B },
   /* 0F 0C */ { &decoder_ud64, NULL },
   /* 0F 0D */ { &decoder64_modrm, BxOpcodeTable0F0D },
   /* 0F 0E */ { &decoder_simple64, BxOpcodeTable0F0E },
   /* 0F 0F */ { &decoder64_3dnow, NULL },
   /* 0F 10 */ { &decoder64_modrm, BxOpcodeTable0F10 },
   /* 0F 11 */ { &decoder64_modrm, BxOpcodeTable0F11 },
   /* 0F 12 */ { &decoder64_modrm, BxOpcodeTable0F12 },
   /* 0F 13 */ { &decoder64_modrm, BxOpcodeTable0F13 },
   /* 0F 14 */ { &decoder64_modrm, BxOpcodeTable0F14 },
   /* 0F 15 */ { &decoder64_modrm, BxOpcodeTable0F15 },
   /* 0F 16 */ { &decoder64_modrm, BxOpcodeTable0F16 },
   /* 0F 17 */ { &decoder64_modrm, BxOpcodeTable0F17 },
   /* 0F 18 */ { &decoder64_modrm, BxOpcodeTable0F18 },
   /* 0F 19 */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1A */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1B */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1C */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 1D */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
#if BX_SUPPORT_CET
   /* 0F 1E */ { &decoder64_modrm, BxOpcodeTable0F1E },
#else
   /* 0F 1E */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
#endif
   /* 0F 1F */ { &decoder64_modrm, BxOpcodeTableMultiByteNOP },
   /* 0F 20 */ { &decoder_creg64, BxOpcodeTable0F20_64 },
   /* 0F 21 */ { &decoder_creg64, BxOpcodeTable0F21_64 },
   /* 0F 22 */ { &decoder_creg64, BxOpcodeTable0F22_64 },
   /* 0F 23 */ { &decoder_creg64, BxOpcodeTable0F23_64 },
   /* 0F 24 */ { &decoder_ud64, NULL },
   /* 0F 25 */ { &decoder_ud64, NULL },
   /* 0F 26 */ { &decoder_ud64, NULL },
   /* 0F 27 */ { &decoder_ud64, NULL },
   /* 0F 28 */ { &decoder64_modrm, BxOpcodeTable0F28 },
   /* 0F 29 */ { &decoder64_modrm, BxOpcodeTable0F29 },
   /* 0F 2A */ { &decoder64_modrm, BxOpcodeTable0F2A },
   /* 0F 2B */ { &decoder64_modrm, BxOpcodeTable0F2B },
   /* 0F 2C */ { &decoder64_modrm, BxOpcodeTable0F2C },
   /* 0F 2D */ { &decoder64_modrm, BxOpcodeTable0F2D },
   /* 0F 2E */ { &decoder64_modrm, BxOpcodeTable0F2E },
   /* 0F 2F */ { &decoder64_modrm, BxOpcodeTable0F2F },
   /* 0F 30 */ { &decoder_simple64, BxOpcodeTable0F30 },
   /* 0F 31 */ { &decoder_simple64, BxOpcodeTable0F31 },
   /* 0F 32 */ { &decoder_simple64, BxOpcodeTable0F32 },
   /* 0F 33 */ { &decoder_simple64, BxOpcodeTable0F33 },
   /* 0F 34 */ { &decoder_simple64, BxOpcodeTable0F34 },
   /* 0F 35 */ { &decoder_simple64, BxOpcodeTable0F35 },
   /* 0F 36 */ { &decoder_ud64, NULL },
   /* 0F 37 */ { &decoder64, BxOpcodeTable0F37 },
   /* 0F 38 */ { &decoder64_modrm, NULL }, // 3-byte escape
   /* 0F 39 */ { &decoder_ud64, NULL },
   /* 0F 3A */ { &decoder64_modrm, NULL }, // 3-byte escape
   /* 0F 3B */ { &decoder_ud64, NULL },
   /* 0F 3C */ { &decoder_ud64, NULL },
   /* 0F 3D */ { &decoder_ud64, NULL },
   /* 0F 3E */ { &decoder_ud64, NULL },
   /* 0F 3F */ { &decoder_ud64, NULL },
   /* 0F 40 */ { &decoder64_modrm, BxOpcodeTable0F40 },
   /* 0F 41 */ { &decoder64_modrm, BxOpcodeTable0F41 },
   /* 0F 42 */ { &decoder64_modrm, BxOpcodeTable0F42 },
   /* 0F 43 */ { &decoder64_modrm, BxOpcodeTable0F43 },
   /* 0F 44 */ { &decoder64_modrm, BxOpcodeTable0F44 },
   /* 0F 45 */ { &decoder64_modrm, BxOpcodeTable0F45 },
   /* 0F 46 */ { &decoder64_modrm, BxOpcodeTable0F46 },
   /* 0F 47 */ { &decoder64_modrm, BxOpcodeTable0F47 },
   /* 0F 48 */ { &decoder64_modrm, BxOpcodeTable0F48 },
   /* 0F 49 */ { &decoder64_modrm, BxOpcodeTable0F49 },
   /* 0F 4A */ { &decoder64_modrm, BxOpcodeTable0F4A },
   /* 0F 4B */ { &decoder64_modrm, BxOpcodeTable0F4B },
   /* 0F 4C */ { &decoder64_modrm, BxOpcodeTable0F4C },
   /* 0F 4D */ { &decoder64_modrm, BxOpcodeTable0F4D },
   /* 0F 4E */ { &decoder64_modrm, BxOpcodeTable0F4E },
   /* 0F 4F */ { &decoder64_modrm, BxOpcodeTable0F4F },
   /* 0F 50 */ { &decoder64_modrm, BxOpcodeTable0F50 },
   /* 0F 51 */ { &decoder64_modrm, BxOpcodeTable0F51 },
   /* 0F 52 */ { &decoder64_modrm, BxOpcodeTable0F52 },
   /* 0F 53 */ { &decoder64_modrm, BxOpcodeTable0F53 },
   /* 0F 54 */ { &decoder64_modrm, BxOpcodeTable0F54 },
   /* 0F 55 */ { &decoder64_modrm, BxOpcodeTable0F55 },
   /* 0F 56 */ { &decoder64_modrm, BxOpcodeTable0F56 },
   /* 0F 57 */ { &decoder64_modrm, BxOpcodeTable0F57 },
   /* 0F 58 */ { &decoder64_modrm, BxOpcodeTable0F58 },
   /* 0F 59 */ { &decoder64_modrm, BxOpcodeTable0F59 },
   /* 0F 5A */ { &decoder64_modrm, BxOpcodeTable0F5A },
   /* 0F 5B */ { &decoder64_modrm, BxOpcodeTable0F5B },
   /* 0F 5C */ { &decoder64_modrm, BxOpcodeTable0F5C },
   /* 0F 5D */ { &decoder64_modrm, BxOpcodeTable0F5D },
   /* 0F 5E */ { &decoder64_modrm, BxOpcodeTable0F5E },
   /* 0F 5F */ { &decoder64_modrm, BxOpcodeTable0F5F },
   /* 0F 60 */ { &decoder64_modrm, BxOpcodeTable0F60 },
   /* 0F 61 */ { &decoder64_modrm, BxOpcodeTable0F61 },
   /* 0F 62 */ { &decoder64_modrm, BxOpcodeTable0F62 },
   /* 0F 63 */ { &decoder64_modrm, BxOpcodeTable0F63 },
   /* 0F 64 */ { &decoder64_modrm, BxOpcodeTable0F64 },
   /* 0F 65 */ { &decoder64_modrm, BxOpcodeTable0F65 },
   /* 0F 66 */ { &decoder64_modrm, BxOpcodeTable0F66 },
   /* 0F 67 */ { &decoder64_modrm, BxOpcodeTable0F67 },
   /* 0F 68 */ { &decoder64_modrm, BxOpcodeTable0F68 },
   /* 0F 69 */ { &decoder64_modrm, BxOpcodeTable0F69 },
   /* 0F 6A */ { &decoder64_modrm, BxOpcodeTable0F6A },
   /* 0F 6B */ { &decoder64_modrm, BxOpcodeTable0F6B },
   /* 0F 6C */ { &decoder64_modrm, BxOpcodeTable0F6C },
   /* 0F 6D */ { &decoder64_modrm, BxOpcodeTable0F6D },
   /* 0F 6E */ { &decoder64_modrm, BxOpcodeTable0F6E },
   /* 0F 6F */ { &decoder64_modrm, BxOpcodeTable0F6F },
   /* 0F 70 */ { &decoder64_modrm, BxOpcodeTable0F70 },
   /* 0F 71 */ { &decoder64_modrm, BxOpcodeTable0F71 },
   /* 0F 72 */ { &decoder64_modrm, BxOpcodeTable0F72 },
   /* 0F 73 */ { &decoder64_modrm, BxOpcodeTable0F73 },
   /* 0F 74 */ { &decoder64_modrm, BxOpcodeTable0F74 },
   /* 0F 75 */ { &decoder64_modrm, BxOpcodeTable0F75 },
   /* 0F 76 */ { &decoder64_modrm, BxOpcodeTable0F76 },
   /* 0F 77 */ { &decoder64, BxOpcodeTable0F77 },
   /* 0F 78 */ { &decoder64_modrm, BxOpcodeTable0F78 },
   /* 0F 79 */ { &decoder64_modrm, BxOpcodeTable0F79 },
   /* 0F 7A */ { &decoder_ud64, NULL },
   /* 0F 7B */ { &decoder_ud64, NULL },
   /* 0F 7C */ { &decoder64_modrm, BxOpcodeTable0F7C },
   /* 0F 7D */ { &decoder64_modrm, BxOpcodeTable0F7D },
   /* 0F 7E */ { &decoder64_modrm, BxOpcodeTable0F7E },
   /* 0F 7F */ { &decoder64_modrm, BxOpcodeTable0F7F },
   /* 0F 80 */ { &decoder64, BxOpcodeTable0F80_64 },
   /* 0F 81 */ { &decoder64, BxOpcodeTable0F81_64 },
   /* 0F 82 */ { &decoder64, BxOpcodeTable0F82_64 },
   /* 0F 83 */ { &decoder64, BxOpcodeTable0F83_64 },
   /* 0F 84 */ { &decoder64, BxOpcodeTable0F84_64 },
   /* 0F 85 */ { &decoder64, BxOpcodeTable0F85_64 },
   /* 0F 86 */ { &decoder64, BxOpcodeTable0F86_64 },
   /* 0F 87 */ { &decoder64, BxOpcodeTable0F87_64 },
   /* 0F 88 */ { &decoder64, BxOpcodeTable0F88_64 },
   /* 0F 89 */ { &decoder64, BxOpcodeTable0F89_64 },
   /* 0F 8A */ { &decoder64, BxOpcodeTable0F8A_64 },
   /* 0F 8B */ { &decoder64, BxOpcodeTable0F8B_64 },
   /* 0F 8C */ { &decoder64, BxOpcodeTable0F8C_64 },
   /* 0F 8D */ { &decoder64, BxOpcodeTable0F8D_64 },
   /* 0F 8E */ { &decoder64, BxOpcodeTable0F8E_64 },
   /* 0F 8F */ { &decoder64, BxOpcodeTable0F8F_64 },
   /* 0F 90 */ { &decoder64_modrm, BxOpcodeTable0F90 },
   /* 0F 91 */ { &decoder64_modrm, BxOpcodeTable0F91 },
   /* 0F 92 */ { &decoder64_modrm, BxOpcodeTable0F92 },
   /* 0F 93 */ { &decoder64_modrm, BxOpcodeTable0F93 },
   /* 0F 94 */ { &decoder64_modrm, BxOpcodeTable0F94 },
   /* 0F 95 */ { &decoder64_modrm, BxOpcodeTable0F95 },
   /* 0F 96 */ { &decoder64_modrm, BxOpcodeTable0F96 },
   /* 0F 97 */ { &decoder64_modrm, BxOpcodeTable0F97 },
   /* 0F 98 */ { &decoder64_modrm, BxOpcodeTable0F98 },
   /* 0F 99 */ { &decoder64_modrm, BxOpcodeTable0F99 },
   /* 0F 9A */ { &decoder64_modrm, BxOpcodeTable0F9A },
   /* 0F 9B */ { &decoder64_modrm, BxOpcodeTable0F9B },
   /* 0F 9C */ { &decoder64_modrm, BxOpcodeTable0F9C },
   /* 0F 9D */ { &decoder64_modrm, BxOpcodeTable0F9D },
   /* 0F 9E */ { &decoder64_modrm, BxOpcodeTable0F9E },
   /* 0F 9F */ { &decoder64_modrm, BxOpcodeTable0F9F },
   /* 0F A0 */ { &decoder64, BxOpcodeTable0FA0 },
   /* 0F A1 */ { &decoder64, BxOpcodeTable0FA1 },
   /* 0F A2 */ { &decoder_simple64, BxOpcodeTable0FA2 },
   /* 0F A3 */ { &decoder64_modrm, BxOpcodeTable0FA3 },
   /* 0F A4 */ { &decoder64_modrm, BxOpcodeTable0FA4 },
   /* 0F A5 */ { &decoder64_modrm, BxOpcodeTable0FA5 },
   /* 0F A6 */ { &decoder_ud64, NULL },
   /* 0F A7 */ { &decoder_ud64, NULL },
   /* 0F A8 */ { &decoder64, BxOpcodeTable0FA8 },
   /* 0F A9 */ { &decoder64, BxOpcodeTable0FA9 },
   /* 0F AA */ { &decoder_simple64, BxOpcodeTable0FAA },
   /* 0F AB */ { &decoder64_modrm, BxOpcodeTable0FAB },
   /* 0F AC */ { &decoder64_modrm, BxOpcodeTable0FAC },
   /* 0F AD */ { &decoder64_modrm, BxOpcodeTable0FAD },
   /* 0F AE */ { &decoder64_modrm, BxOpcodeTable0FAE },
   /* 0F AF */ { &decoder64_modrm, BxOpcodeTable0FAF },
   /* 0F B0 */ { &decoder64_modrm, BxOpcodeTable0FB0 },
   /* 0F B1 */ { &decoder64_modrm, BxOpcodeTable0FB1 },
   /* 0F B2 */ { &decoder64_modrm, BxOpcodeTable0FB2 },
   /* 0F B3 */ { &decoder64_modrm, BxOpcodeTable0FB3 },
   /* 0F B4 */ { &decoder64_modrm, BxOpcodeTable0FB4 },
   /* 0F B5 */ { &decoder64_modrm, BxOpcodeTable0FB5 },
   /* 0F B6 */ { &decoder64_modrm, BxOpcodeTable0FB6 },
   /* 0F B7 */ { &decoder64_modrm, BxOpcodeTable0FB7 },
   /* 0F B8 */ { &decoder64_modrm, BxOpcodeTable0FB8 },
   /* 0F B9 */ { &decoder64_modrm, BxOpcodeTable0FB9 },
   /* 0F BA */ { &decoder64_modrm, BxOpcodeTable0FBA },
   /* 0F BB */ { &decoder64_modrm, BxOpcodeTable0FBB },
   /* 0F BC */ { &decoder64_modrm, BxOpcodeTable0FBC },
   /* 0F BD */ { &decoder64_modrm, BxOpcodeTable0FBD },
   /* 0F BE */ { &decoder64_modrm, BxOpcodeTable0FBE },
   /* 0F BF */ { &decoder64_modrm, BxOpcodeTable0FBF },
   /* 0F C0 */ { &decoder64_modrm, BxOpcodeTable0FC0 },
   /* 0F C1 */ { &decoder64_modrm, BxOpcodeTable0FC1 },
   /* 0F C2 */ { &decoder64_modrm, BxOpcodeTable0FC2 },
   /* 0F C3 */ { &decoder64_modrm, BxOpcodeTable0FC3 },
   /* 0F C4 */ { &decoder64_modrm, BxOpcodeTable0FC4 },
   /* 0F C5 */ { &decoder64_modrm, BxOpcodeTable0FC5 },
   /* 0F C6 */ { &decoder64_modrm, BxOpcodeTable0FC6 },
   /* 0F C7 */ { &decoder64_modrm, BxOpcodeTable0FC7 },
   /* 0F C8 */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F C9 */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CA */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CB */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CC */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CD */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CE */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F CF */ { &decoder64, BxOpcodeTable0FC8x0FCF },
   /* 0F D0 */ { &decoder64_modrm, BxOpcodeTable0FD0 },
   /* 0F D1 */ { &decoder64_modrm, BxOpcodeTable0FD1 },
   /* 0F D2 */ { &decoder64_modrm, BxOpcodeTable0FD2 },
   /* 0F D3 */ { &decoder64_modrm, BxOpcodeTable0FD3 },
   /* 0F D4 */ { &decoder64_modrm, BxOpcodeTable0FD4 },
   /* 0F D5 */ { &decoder64_modrm, BxOpcodeTable0FD5 },
   /* 0F D6 */ { &decoder64_modrm, BxOpcodeTable0FD6 },
   /* 0F D7 */ { &decoder64_modrm, BxOpcodeTable0FD7 },
   /* 0F D8 */ { &decoder64_modrm, BxOpcodeTable0FD8 },
   /* 0F D9 */ { &decoder64_modrm, BxOpcodeTable0FD9 },
   /* 0F DA */ { &decoder64_modrm, BxOpcodeTable0FDA },
   /* 0F DB */ { &decoder64_modrm, BxOpcodeTable0FDB },
   /* 0F DC */ { &decoder64_modrm, BxOpcodeTable0FDC },
   /* 0F DD */ { &decoder64_modrm, BxOpcodeTable0FDD },
   /* 0F DE */ { &decoder64_modrm, BxOpcodeTable0FDE },
   /* 0F DF */ { &decoder64_modrm, BxOpcodeTable0FDF },
   /* 0F E0 */ { &decoder64_modrm, BxOpcodeTable0FE0 },
   /* 0F E1 */ { &decoder64_modrm, BxOpcodeTable0FE1 },
   /* 0F E2 */ { &decoder64_modrm, BxOpcodeTable0FE2 },
   /* 0F E3 */ { &decoder64_modrm, BxOpcodeTable0FE3 },
   /* 0F E4 */ { &decoder64_modrm, BxOpcodeTable0FE4 },
   /* 0F E5 */ { &decoder64_modrm, BxOpcodeTable0FE5 },
   /* 0F E6 */ { &decoder64_modrm, BxOpcodeTable0FE6 },
   /* 0F E7 */ { &decoder64_modrm, BxOpcodeTable0FE7 },
   /* 0F E8 */ { &decoder64_modrm, BxOpcodeTable0FE8 },
   /* 0F E9 */ { &decoder64_modrm, BxOpcodeTable0FE9 },
   /* 0F EA */ { &decoder64_modrm, BxOpcodeTable0FEA },
   /* 0F EB */ { &decoder64_modrm, BxOpcodeTable0FEB },
   /* 0F EC */ { &decoder64_modrm, BxOpcodeTable0FEC },
   /* 0F ED */ { &decoder64_modrm, BxOpcodeTable0FED },
   /* 0F EE */ { &decoder64_modrm, BxOpcodeTable0FEE },
   /* 0F EF */ { &decoder64_modrm, BxOpcodeTable0FEF },
   /* 0F F0 */ { &decoder64_modrm, BxOpcodeTable0FF0 },
   /* 0F F1 */ { &decoder64_modrm, BxOpcodeTable0FF1 },
   /* 0F F2 */ { &decoder64_modrm, BxOpcodeTable0FF2 },
   /* 0F F3 */ { &decoder64_modrm, BxOpcodeTable0FF3 },
   /* 0F F4 */ { &decoder64_modrm, BxOpcodeTable0FF4 },
   /* 0F F5 */ { &decoder64_modrm, BxOpcodeTable0FF5 },
   /* 0F F6 */ { &decoder64_modrm, BxOpcodeTable0FF6 },
   /* 0F F7 */ { &decoder64_modrm, BxOpcodeTable0FF7 },
   /* 0F F8 */ { &decoder64_modrm, BxOpcodeTable0FF8 },
   /* 0F F9 */ { &decoder64_modrm, BxOpcodeTable0FF9 },
   /* 0F FA */ { &decoder64_modrm, BxOpcodeTable0FFA },
   /* 0F FB */ { &decoder64_modrm, BxOpcodeTable0FFB },
   /* 0F FC */ { &decoder64_modrm, BxOpcodeTable0FFC },
   /* 0F FD */ { &decoder64_modrm, BxOpcodeTable0FFD },
   /* 0F FE */ { &decoder64_modrm, BxOpcodeTable0FFE },
   /* 0F FF */ { &decoder_simple64, BxOpcodeTable0FFF }
};

const Bit8u *decodeModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned mod, unsigned nnn, unsigned rm, unsigned rex_r, unsigned rex_x, unsigned rex_b)
{
  unsigned seg = BX_SEG_REG_DS;

  // initialize displ32 with zero to include cases with no diplacement
  i->modRMForm.displ32u = 0;

  // note that mod==11b handled outside

  if ((rm & 0x7) != 4) { // no s-i-b byte
    i->setSibBase(rm & 0xf);
    i->setSibIndex(4); // no Index encoding by default
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

static const Bit8u *parseModrm64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned rex_r, unsigned rex_x, unsigned rex_b, struct bx_modrm *modrm)
{
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

int decoder_vex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  if (remain == 0)
    return(-1);

#if BX_SUPPORT_AVX
  // VEX
  assert((b1 & ~0x1) == 0xc4);

  if (sse_prefix | rex_prefix)
    return(BX_IA_ERROR);

  unsigned vex_w = 0;
  unsigned vex_opc_map = 1;
  unsigned vex = *iptr++;
  remain--;

  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  rex_r = ((vex >> 4) & 0x8) ^ 0x8;
  if (b1 == 0xc4) {
    rex_x = ((vex >> 3) & 0x8) ^ 0x8;
    rex_b = ((vex >> 2) & 0x8) ^ 0x8;

    // decode 3-byte VEX prefix
    vex_opc_map = vex & 0x1f;
    if (remain == 0)
      return(-1);
    remain--;
    vex = *iptr++;  // fetch VEX3

    if (vex & 0x80) {
      vex_w = 1;
      i->assertOs64();
      i->assertOs32();
    }
  }

  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  i->setVexW(vex_w);
  sse_prefix = vex & 0x3;

  if (remain == 0)
    return(-1);
  remain--;
  unsigned opcode_byte = *iptr++; // fetch new b1
  // there are instructions only from maps 1,2,3 and 7 for now
  if (vex_opc_map != 1 && vex_opc_map != 2 && vex_opc_map != 3 && vex_opc_map != 7)
    return(ia_opcode);
  opcode_byte += 256 * vex_opc_map;
  bool has_modrm = (opcode_byte != 0x177); // if not VZEROUPPER/VZEROALL opcode
  opcode_byte -= 256;
  if (vex_opc_map > 3) {
    // tables skip maps 4, 5 and 6
    opcode_byte -= 768;
  }

  unsigned rm = 0, nnn = 0;
  if (has_modrm) {
    // opcode requires modrm byte
    struct bx_modrm modrm;
    iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
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
    rm = (b1 & 7) | rex_b;
    nnn = (b1 >> 3) & 7;
    i->assertModC0();
  }

  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   ((nnn & 0x7) << NNN_OFFSET) |
                   ((rm  & 0x7) << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vex_l << VEX_VL_128_256_OFFSET);
  if (i->modC0() && nnn == rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  ia_opcode = findOpcode(BxOpcodeTableVEX[opcode_byte], decmask);

  bool has_immediate = (opcode_byte >= 0x70 && opcode_byte <= 0x73) || (opcode_byte >= 0xC2 && opcode_byte <= 0xC6) || (opcode_byte >= 0x200);
  if (has_immediate) {
    if (vex_opc_map == 7) {
      if (remain > 3) {
        i->modRMForm.Id = FetchDWORD(iptr);
        remain -= 4;
      }
      else {
        return(-1);
      }
    }
    else {
      if (remain != 0) {
        i->modRMForm.Ib[0] = *iptr;
        remain--;
      }
      else {
        return(-1);
      }
    }
  }

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, true, nnn, rm, vvv, vex_w);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;
#endif

  return ia_opcode;
}

int decoder_evex64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  if (remain == 0)
    return(-1);

#if BX_SUPPORT_EVEX
  // EVEX prefix 0x62
  assert(b1 == 0x62);

  if (sse_prefix | rex_prefix)
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

  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
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
  }

  unsigned opmask = (evex >> 16) & 0x7;
  i->setOpmask(opmask);
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

  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
  if (! iptr)
    return(-1);

  unsigned nnn = modrm.nnn;
  unsigned rm  = modrm.rm;

  bool displ8 = (modrm.mod == 0x40); // mod == 01b

  if (i->modC0()) {
    // EVEX.b in reg form implies 512-bit vector length
    if (i->getEvexb()) i->setVL(BX_VL512);
  }

  Bit32u vl = i->getVL()-1; // 0: VL128, 1: VL256, 3: VL512
  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   ((nnn & 0x7) << NNN_OFFSET) |
                   ((rm  & 0x7) << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vl << VEX_VL_128_256_OFFSET);
  if (i->modC0() && nnn == rm)
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

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, true, nnn, rm, vvv, vex_w, true, displ8);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;

  // EVEX specific #UD conditions
  if (i->getVL() > BX_VL512) {
    ia_opcode = BX_IA_ERROR;
  }
#endif

  return ia_opcode;
}

int decoder_xop64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  // 3 byte XOP prefix
  assert(b1 == 0x8f);

  if (remain == 0)
    return(-1);

  if ((*iptr & 0x08) != 0x08) {
    // not XOP prefix, decode regular opcode
    return decoder64_modrm(iptr, remain, i, b1, sse_prefix, rex_prefix, opcode_table);
  }

#if BX_SUPPORT_AVX
  if (sse_prefix | rex_prefix)
    return(ia_opcode);

  unsigned vex;
  if (remain > 2) {
    remain -= 3;
    vex = *iptr++; // fetch XOP2
  }
  else
    return(-1);

  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  rex_r = ((vex >> 4) & 0x8) ^ 0x8;
  rex_x = ((vex >> 3) & 0x8) ^ 0x8;
  rex_b = ((vex >> 2) & 0x8) ^ 0x8;

  unsigned xop_opcext = (vex & 0x1f) - 8;
  if (xop_opcext >= 3)
    return(ia_opcode);

  vex = *iptr++; // fetch XOP3

  unsigned vex_w = 0;
  if (vex & 0x80) {
    vex_w = 1;
    i->assertOs64();
    i->assertOs32();
  }

  int vvv = 15 - ((vex >> 3) & 0xf);
  unsigned vex_l = (vex >> 2) & 0x1;
  i->setVL(BX_VL128 + vex_l);
  i->setVexW(vex_w);
  sse_prefix = vex & 0x3;
  if (sse_prefix)
    return(ia_opcode);

  unsigned opcode_byte = *iptr++;
  opcode_byte += 256 * xop_opcext;

  // opcode requires modrm byte
  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
  if (! iptr)
    return(-1);

  unsigned nnn = modrm.nnn;
  unsigned rm  = modrm.rm;

  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   ((nnn & 0x7) << NNN_OFFSET) |
                   ((rm  & 0x7) << RRR_OFFSET) |
                   (vex_w << VEX_W_OFFSET) |
                   (vex_l << VEX_VL_128_256_OFFSET);
  if (i->modC0() && nnn == rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  ia_opcode = findOpcode(BxOpcodeTableXOP[opcode_byte], decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, true) < 0)
    return (-1);

  BxDecodeError decode_err = assign_srcs(i, ia_opcode, true, nnn, rm, vvv, vex_w);
  if (decode_err != BX_DECODE_OK)
    ia_opcode = BX_IA_ERROR;
#endif

  return ia_opcode;
}

int decoder_ud64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  return BX_IA_ERROR;
}

int decoder64_fp_escape(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  int ia_opcode = BX_IA_ERROR;

  assert(b1 >= 0xd8 && b1 <= 0xdf);

  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  if (rex_prefix) {
    rex_r = ((rex_prefix & 0x4) << 1);
    rex_x = ((rex_prefix & 0x2) << 2);
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
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

int decoder64_modrm(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  if (rex_prefix) {
    rex_r = ((rex_prefix & 0x4) << 1);
    rex_x = ((rex_prefix & 0x2) << 2);
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
  if (! iptr)
    return(-1);

  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (i->modC0() ? (1 << MODC0_OFFSET) : 0) |
                   ((modrm.nnn & 0x7) << NNN_OFFSET) |
                   ((modrm.rm  & 0x7) << RRR_OFFSET);
  if (i->modC0() && modrm.nnn == modrm.rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  Bit16u ia_opcode = findOpcode((const Bit64u*) opcode_table, decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, true) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, modrm.nnn, modrm.rm);

  return ia_opcode;
}

int decoder64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  unsigned rex_b = 0;
  if (rex_prefix) {
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  unsigned rm = (b1 & 7) | rex_b;
  unsigned nnn = (b1 >> 3) & 7;
  i->assertModC0();

  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (1 << MODC0_OFFSET) |
                   ((nnn & 0x7) << NNN_OFFSET) |
                   ((rm  & 0x7) << RRR_OFFSET);
  if (nnn == rm)
    decmask |= (1 << SRC_EQ_DST_OFFSET);

  Bit16u ia_opcode = findOpcode((const Bit64u*) opcode_table, decmask);

  if (fetchImmediate(iptr, remain, i, ia_opcode, true) < 0)
    return (-1);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder_creg64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  // MOVs with CRx and DRx always use register ops and ignore the mod field.
  assert((b1 & ~7) == 0x120);

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
  unsigned b2 = *iptr++; // fetch modrm byte

  // Parse mod-nnn-rm and related bytes
  unsigned nnn = ((b2 >> 3) & 0x7) | rex_r;
  unsigned rm  = (b2 & 0x7) | rex_b;

  i->assertModC0();

  Bit32u decmask = (1 << IS64_OFFSET) |
                   (i->osize() << OS32_OFFSET) |
                   (i->asize() << AS32_OFFSET) |
                   (sse_prefix << SSE_PREFIX_OFFSET) |
                   (1 << MODC0_OFFSET) |
                   ((nnn & 0x7) << NNN_OFFSET) |
                   ((rm  & 0x7) << RRR_OFFSET);

  Bit16u ia_opcode = findOpcode((Bit64u *) opcode_table, decmask);

  assign_srcs(i, ia_opcode, nnn, rm);

  return ia_opcode;
}

int decoder64_3dnow(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  Bit16u ia_opcode = BX_IA_ERROR;

#if BX_SUPPORT_3DNOW
  unsigned rex_r = 0, rex_x = 0, rex_b = 0;
  if (rex_prefix) {
    rex_r = ((rex_prefix & 0x4) << 1);
    rex_x = ((rex_prefix & 0x2) << 2);
    rex_b = ((rex_prefix & 0x1) << 3);
  }

  struct bx_modrm modrm;
  iptr = parseModrm64(iptr, remain, i, rex_r, rex_x, rex_b, &modrm);
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

int decoder64_nop(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  assert(b1 == 0x90);

  i->assertModC0();

  unsigned rex_b = (rex_prefix & 0x1);
  if (rex_b) {
    return decoder64(iptr, remain, i, b1, sse_prefix, rex_prefix, opcode_table);
  }
  else {
    if (sse_prefix == SSE_PREFIX_F3)
      return BX_IA_PAUSE;
    else
      return BX_IA_NOP;
  }
}

int decoder_simple64(const Bit8u *iptr, unsigned &remain, bxInstruction_c *i, unsigned b1, unsigned sse_prefix, unsigned rex_prefix, const void *opcode_table)
{
  i->assertModC0();

  const Bit64u *op = (const Bit64u *) opcode_table;

  // no immediate expected, no sources expected, take first opcode
  // check attributes ?
  Bit16u ia_opcode = Bit16u(*op >> 48) & 0x7FFF; // upper bit indicates that parsing is done and doesn't belong to opcode
  return ia_opcode;
}

int fetchDecode64(const Bit8u *iptr, bxInstruction_c *i, unsigned remainingInPage)
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
  bool lock = 0;
  unsigned sse_prefix = SSE_PREFIX_NONE;
  unsigned rex_prefix = 0;

  i->init(/*os32*/ 1,  // operand size 32 override defaults to 1
          /*as32*/ 1,  // address size 32 override defaults to 1
          /*os64*/ 0,  // operand size 64 override defaults to 0
          /*as64*/ 1); // address size 64 override defaults to 1

fetch_b1:
  b1 = *iptr++;
  remain--;

#if BX_SUPPORT_CET
  // in 64-bit mode DS prefix is ignored but still recorded for CET Endbranch suppress hint
  // keep it even if overridden by FS: or GS:
  if (b1 == 0x3e)
    seg_override_cet = BX_SEG_REG_DS;
#endif

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

  BxOpcodeDecodeDescriptor64 *decode_descriptor = &decode64_descriptor[b1];
  BxFetchDecode64Ptr decode_method = decode_descriptor->decode_method;
  const void *opcode_table = decode_descriptor->opcode_table;

  // handle 3-byte opcode
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

  if (rex_prefix) {
    i->assertExtend8bit();
    if (rex_prefix & 0x8) {
      i->assertOs64();
      i->assertOs32();
    }
  }

  i->setSeg(BX_SEG_REG_DS); // default segment is DS:
#if BX_SUPPORT_CET
  i->setCetSegOverride(seg_override_cet);
#endif

  i->modRMForm.Id = 0;

  ia_opcode = (*decode_method)(iptr, remain, i, b1, sse_prefix, rex_prefix, opcode_table);
  if (ia_opcode < 0)
    return(-1);

  i->setILen(remainingInPage - remain);
  i->setIaOpcode(ia_opcode);

  // assign memory segment override
  if (seg_override == BX_SEG_REG_FS || seg_override == BX_SEG_REG_GS)
     i->setSeg(seg_override);

  Bit32u op_flags = BxOpcodesTable[ia_opcode].opflags;

  if (lock) {
    i->setLock();
    // lock prefix not allowed or destination operand is not memory
    if (i->modC0() || !(op_flags & BX_LOCKABLE)) {
      if ((op_flags & BX_LOCKABLE) != 0) {
        if (ia_opcode == BX_IA_MOV_CR0Rq)
          i->setSrcReg(0, 8); // extend CR0 -> CR8
        else if (ia_opcode == BX_IA_MOV_RqCR0)
          i->setSrcReg(1, 8); // extend CR0 -> CR8
        else
          i->setIaOpcode(BX_IA_ERROR); // replace execution function with undefined-opcode
      }
      else {
        // replace execution function with undefined-opcode
        i->setIaOpcode(BX_IA_ERROR);
      }
    }
  }

  return(0);
}

#endif /* if BX_SUPPORT_X86_64 */
