/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017 Stanislav Shwartsman
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

#ifndef BX_FETCHDECODE_OPMAP_H
#define BX_FETCHDECODE_OPMAP_H

// opcode 00
static const BxOpcodeInfo_t BxOpcodeTable00[] = {
  /* 00 /w */ { 0, BX_IA_ADD_EbGb },
  /* 00 /d */ { 0, BX_IA_ADD_EbGb },
#if BX_SUPPORT_X86_64
  /* 00 /q */ { 0, BX_IA_ADD_EbGb },
#endif
};

// opcode 01
static const BxOpcodeInfo_t BxOpcodeTable01[] = {
  /* 01 /w */ { 0, BX_IA_ADD_EwGw },
  /* 01 /d */ { 0, BX_IA_ADD_EdGd },
#if BX_SUPPORT_X86_64
  /* 01 /q */ { 0, BX_IA_ADD_EqGq },
#endif
};

// opcode 02
static const BxOpcodeInfo_t BxOpcodeTable02[] = {
  /* 02 /w */ { 0, BX_IA_ADD_GbEb },
  /* 02 /d */ { 0, BX_IA_ADD_GbEb },
#if BX_SUPPORT_X86_64
  /* 02 /q */ { 0, BX_IA_ADD_GbEb },
#endif
};

// opcode 03
static const BxOpcodeInfo_t BxOpcodeTable03[] = {
  /* 03 /w */ { 0, BX_IA_ADD_GwEw },
  /* 03 /d */ { 0, BX_IA_ADD_GdEd },
#if BX_SUPPORT_X86_64
  /* 03 /q */ { 0, BX_IA_ADD_GqEq },
#endif
};

// opcode 04
static const BxOpcodeInfo_t BxOpcodeTable04[] = {
  /* 04 /w */ { BxImmediate_Ib, BX_IA_ADD_ALIb },
  /* 04 /d */ { BxImmediate_Ib, BX_IA_ADD_ALIb },
#if BX_SUPPORT_X86_64
  /* 04 /q */ { BxImmediate_Ib, BX_IA_ADD_ALIb },
#endif
};

// opcode 05
static const BxOpcodeInfo_t BxOpcodeTable05[] = {
  /* 05 /w */ { BxImmediate_Iw, BX_IA_ADD_AXIw },
  /* 05 /d */ { BxImmediate_Id, BX_IA_ADD_EAXId },
#if BX_SUPPORT_X86_64
  /* 05 /q */ { BxImmediate_Id, BX_IA_ADD_RAXId },
#endif
};

// opcode 06
static const BxOpcodeInfo_t BxOpcodeTable06_32[] = {
  /* 06 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 06 /d */ { 0, BX_IA_PUSH_Op32_Sw },
};

// opcode 07
static const BxOpcodeInfo_t BxOpcodeTable07_32[] = {
  /* 07 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 07 /d */ { 0, BX_IA_POP_Op32_Sw },
};

// opcode 08
static const BxOpcodeInfo_t BxOpcodeTable08[] = {
  /* 08 /w */ { 0, BX_IA_OR_EbGb },
  /* 08 /d */ { 0, BX_IA_OR_EbGb },
#if BX_SUPPORT_X86_64
  /* 08 /q */ { 0, BX_IA_OR_EbGb },
#endif
};

// opcode 09
static const BxOpcodeInfo_t BxOpcodeTable09[] = {
  /* 09 /w */ { 0, BX_IA_OR_EwGw },
  /* 09 /d */ { 0, BX_IA_OR_EdGd },
#if BX_SUPPORT_X86_64
  /* 09 /q */ { 0, BX_IA_OR_EqGq },
#endif
};

// opcode 0A
static const BxOpcodeInfo_t BxOpcodeTable0A[] = {
  /* 0A /w */ { 0, BX_IA_OR_GbEb },
  /* 0A /d */ { 0, BX_IA_OR_GbEb },
#if BX_SUPPORT_X86_64
  /* 0A /q */ { 0, BX_IA_OR_GbEb },
#endif
};

// opcode 0B
static const BxOpcodeInfo_t BxOpcodeTable0B[] = {
  /* 0B /w */ { 0, BX_IA_OR_GwEw },
  /* 0B /d */ { 0, BX_IA_OR_GdEd },
#if BX_SUPPORT_X86_64
  /* 0B /q */ { 0, BX_IA_OR_GqEq },
#endif
};

// opcode 0C
static const BxOpcodeInfo_t BxOpcodeTable0C[] = {
  /* 0C /w */ { BxImmediate_Ib, BX_IA_OR_ALIb },
  /* 0C /d */ { BxImmediate_Ib, BX_IA_OR_ALIb },
#if BX_SUPPORT_X86_64
  /* 0C /q */ { BxImmediate_Ib, BX_IA_OR_ALIb },
#endif
};

// opcode 0D
static const BxOpcodeInfo_t BxOpcodeTable0D[] = {
  /* 0D /w */ { BxImmediate_Iw, BX_IA_OR_AXIw },
  /* 0D /d */ { BxImmediate_Id, BX_IA_OR_EAXId },
#if BX_SUPPORT_X86_64
  /* 0D /q */ { BxImmediate_Id, BX_IA_OR_RAXId },
#endif
};

// opcode 0E
static const BxOpcodeInfo_t BxOpcodeTable0E_32[] = {
  /* 0E /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0E /d */ { 0, BX_IA_PUSH_Op32_Sw },
};

// opcode 10
static const BxOpcodeInfo_t BxOpcodeTable10[] = {
  /* 10 /w */ { 0, BX_IA_ADC_EbGb },
  /* 10 /d */ { 0, BX_IA_ADC_EbGb },
#if BX_SUPPORT_X86_64
  /* 10 /q */ { 0, BX_IA_ADC_EbGb },
#endif
};

// opcode 11
static const BxOpcodeInfo_t BxOpcodeTable11[] = {
  /* 11 /w */ { 0, BX_IA_ADC_EwGw },
  /* 11 /d */ { 0, BX_IA_ADC_EdGd },
#if BX_SUPPORT_X86_64
  /* 11 /q */ { 0, BX_IA_ADC_EqGq },
#endif
};

// opcode 12
static const BxOpcodeInfo_t BxOpcodeTable12[] = {
  /* 12 /w */ { 0, BX_IA_ADC_GbEb },
  /* 12 /d */ { 0, BX_IA_ADC_GbEb },
#if BX_SUPPORT_X86_64
  /* 12 /q */ { 0, BX_IA_ADC_GbEb },
#endif
};

// opcode 13
static const BxOpcodeInfo_t BxOpcodeTable13[] = {
  /* 13 /w */ { 0, BX_IA_ADC_GwEw },
  /* 13 /d */ { 0, BX_IA_ADC_GdEd },
#if BX_SUPPORT_X86_64
  /* 13 /q */ { 0, BX_IA_ADC_GqEq },
#endif
};

// opcode 14
static const BxOpcodeInfo_t BxOpcodeTable14[] = {
  /* 14 /w */ { BxImmediate_Ib, BX_IA_ADC_ALIb },
  /* 14 /d */ { BxImmediate_Ib, BX_IA_ADC_ALIb },
#if BX_SUPPORT_X86_64
  /* 14 /q */ { BxImmediate_Ib, BX_IA_ADC_ALIb },
#endif
};

// opcode 15
static const BxOpcodeInfo_t BxOpcodeTable15[] = {
  /* 15 /w */ { BxImmediate_Iw, BX_IA_ADC_AXIw },
  /* 15 /d */ { BxImmediate_Id, BX_IA_ADC_EAXId },
#if BX_SUPPORT_X86_64
  /* 15 /q */ { BxImmediate_Id, BX_IA_ADC_RAXId },
#endif
};

// opcode 16
static const BxOpcodeInfo_t BxOpcodeTable16_32[] = {
  /* 16 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 16 /d */ { 0, BX_IA_PUSH_Op32_Sw },
};

// opcode 17
static const BxOpcodeInfo_t BxOpcodeTable17_32[] = {
  /* 17 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 17 /d */ { 0, BX_IA_POP_Op32_Sw },
};

// opcode 18
static const BxOpcodeInfo_t BxOpcodeTable18[] = {
  /* 18 /w */ { 0, BX_IA_SBB_EbGb },
  /* 18 /d */ { 0, BX_IA_SBB_EbGb },
#if BX_SUPPORT_X86_64
  /* 18 /q */ { 0, BX_IA_SBB_EbGb },
#endif
};

// opcode 19
static const BxOpcodeInfo_t BxOpcodeTable19[] = {
  /* 19 /w */ { 0, BX_IA_SBB_EwGw },
  /* 19 /d */ { 0, BX_IA_SBB_EdGd },
#if BX_SUPPORT_X86_64
  /* 19 /q */ { 0, BX_IA_SBB_EqGq },
#endif
};

// opcode 1A
static const BxOpcodeInfo_t BxOpcodeTable1A[] = {
  /* 1A /w */ { 0, BX_IA_SBB_GbEb },
  /* 1A /d */ { 0, BX_IA_SBB_GbEb },
#if BX_SUPPORT_X86_64
  /* 1A /q */ { 0, BX_IA_SBB_GbEb },
#endif
};

// opcode 1B
static const BxOpcodeInfo_t BxOpcodeTable1B[] = {
  /* 1B /w */ { 0, BX_IA_SBB_GwEw },
  /* 1B /d */ { 0, BX_IA_SBB_GdEd },
#if BX_SUPPORT_X86_64
  /* 1B /q */ { 0, BX_IA_SBB_GqEq },
#endif
};

// opcode 1C
static const BxOpcodeInfo_t BxOpcodeTable1C[] = {
  /* 1C /w */ { BxImmediate_Ib, BX_IA_SBB_ALIb },
  /* 1C /d */ { BxImmediate_Ib, BX_IA_SBB_ALIb },
#if BX_SUPPORT_X86_64
  /* 1C /q */ { BxImmediate_Ib, BX_IA_SBB_ALIb },
#endif
};

// opcode 1D
static const BxOpcodeInfo_t BxOpcodeTable1D[] = {
  /* 1D /w */ { BxImmediate_Iw, BX_IA_SBB_AXIw },
  /* 1D /d */ { BxImmediate_Id, BX_IA_SBB_EAXId },
#if BX_SUPPORT_X86_64
  /* 1D /q */ { BxImmediate_Id, BX_IA_SBB_RAXId },
#endif
};

// opcode 1E
static const BxOpcodeInfo_t BxOpcodeTable1E_32[] = {
  /* 1E /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 1E /d */ { 0, BX_IA_PUSH_Op32_Sw },
};

// opcode 1F
static const BxOpcodeInfo_t BxOpcodeTable1F_32[] = {
  /* 1F /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 1F /d */ { 0, BX_IA_POP_Op32_Sw },
};

// opcode 20
static const BxOpcodeInfo_t BxOpcodeTable20[] = {
  /* 20 /w */ { 0, BX_IA_AND_EbGb },
  /* 20 /d */ { 0, BX_IA_AND_EbGb },
#if BX_SUPPORT_X86_64
  /* 20 /q */ { 0, BX_IA_AND_EbGb },
#endif
};

// opcode 21
static const BxOpcodeInfo_t BxOpcodeTable21[] = {
  /* 21 /w */ { 0, BX_IA_AND_EwGw },
  /* 21 /d */ { 0, BX_IA_AND_EdGd },
#if BX_SUPPORT_X86_64
  /* 21 /q */ { 0, BX_IA_AND_EqGq },
#endif
};

// opcode 22
static const BxOpcodeInfo_t BxOpcodeTable22[] = {
  /* 22 /w */ { 0, BX_IA_AND_GbEb },
  /* 22 /d */ { 0, BX_IA_AND_GbEb },
#if BX_SUPPORT_X86_64
  /* 22 /q */ { 0, BX_IA_AND_GbEb },
#endif
};

// opcode 23
static const BxOpcodeInfo_t BxOpcodeTable23[] = {
  /* 23 /w */ { 0, BX_IA_AND_GwEw },
  /* 23 /d */ { 0, BX_IA_AND_GdEd },
#if BX_SUPPORT_X86_64
  /* 23 /q */ { 0, BX_IA_AND_GqEq },
#endif
};

// opcode 24
static const BxOpcodeInfo_t BxOpcodeTable24[] = {
  /* 24 /w */ { BxImmediate_Ib, BX_IA_AND_ALIb },
  /* 24 /d */ { BxImmediate_Ib, BX_IA_AND_ALIb },
#if BX_SUPPORT_X86_64
  /* 24 /q */ { BxImmediate_Ib, BX_IA_AND_ALIb },
#endif
};

// opcode 25
static const BxOpcodeInfo_t BxOpcodeTable25[] = {
  /* 25 /w */ { BxImmediate_Iw, BX_IA_AND_AXIw },
  /* 25 /d */ { BxImmediate_Id, BX_IA_AND_EAXId },
#if BX_SUPPORT_X86_64
  /* 25 /q */ { BxImmediate_Id, BX_IA_AND_RAXId },
#endif
};

// opcode 27
static const BxOpcodeInfo_t BxOpcodeTable27_32[] = {
  /* 27 */ { 0, BX_IA_DAA },
};

// opcode 28
static const BxOpcodeInfo_t BxOpcodeTable28[] = {
  /* 28 /w */ { 0, BX_IA_SUB_EbGb },
  /* 28 /d */ { 0, BX_IA_SUB_EbGb },
#if BX_SUPPORT_X86_64
  /* 28 /q */ { 0, BX_IA_SUB_EbGb },
#endif
};

// opcode 29
static const BxOpcodeInfo_t BxOpcodeTable29[] = {
  /* 29 /w */ { 0, BX_IA_SUB_EwGw },
  /* 29 /d */ { 0, BX_IA_SUB_EdGd },
#if BX_SUPPORT_X86_64
  /* 29 /q */ { 0, BX_IA_SUB_EqGq },
#endif
};

// opcode 2A
static const BxOpcodeInfo_t BxOpcodeTable2A[] = {
  /* 2A /w */ { 0, BX_IA_SUB_GbEb },
  /* 2A /d */ { 0, BX_IA_SUB_GbEb },
#if BX_SUPPORT_X86_64
  /* 2A /q */ { 0, BX_IA_SUB_GbEb },
#endif
};

// opcode 2B
static const BxOpcodeInfo_t BxOpcodeTable2B[] = {
  /* 2B /w */ { 0, BX_IA_SUB_GwEw },
  /* 2B /d */ { 0, BX_IA_SUB_GdEd },
#if BX_SUPPORT_X86_64
  /* 2B /q */ { 0, BX_IA_SUB_GqEq },
#endif
};

// opcode 2C
static const BxOpcodeInfo_t BxOpcodeTable2C[] = {
  /* 2C /w */ { BxImmediate_Ib, BX_IA_SUB_ALIb },
  /* 2C /d */ { BxImmediate_Ib, BX_IA_SUB_ALIb },
#if BX_SUPPORT_X86_64
  /* 2C /q */ { BxImmediate_Ib, BX_IA_SUB_ALIb },
#endif
};

// opcode 2D
static const BxOpcodeInfo_t BxOpcodeTable2D[] = {
  /* 2D /w */ { BxImmediate_Iw, BX_IA_SUB_AXIw },
  /* 2D /d */ { BxImmediate_Id, BX_IA_SUB_EAXId },
#if BX_SUPPORT_X86_64
  /* 2D /q */ { BxImmediate_Id, BX_IA_SUB_RAXId },
#endif
};

// opcode 2F
static const BxOpcodeInfo_t BxOpcodeTable2F_32[] = {
  /* 2F */ { 0, BX_IA_DAS },
};

// opcode 30
static const BxOpcodeInfo_t BxOpcodeTable30[] = {
  /* 30 /w */ { 0, BX_IA_XOR_EbGb },
  /* 30 /d */ { 0, BX_IA_XOR_EbGb },
#if BX_SUPPORT_X86_64
  /* 30 /q */ { 0, BX_IA_XOR_EbGb },
#endif
};

// opcode 31
static const BxOpcodeInfo_t BxOpcodeTable31[] = {
  /* 31 /w */ { 0, BX_IA_XOR_EwGw },
  /* 31 /d */ { 0, BX_IA_XOR_EdGd },
#if BX_SUPPORT_X86_64
  /* 31 /q */ { 0, BX_IA_XOR_EqGq },
#endif
};

// opcode 32
static const BxOpcodeInfo_t BxOpcodeTable32[] = {
  /* 32 /w */ { 0, BX_IA_XOR_GbEb },
  /* 32 /d */ { 0, BX_IA_XOR_GbEb },
#if BX_SUPPORT_X86_64
  /* 32 /q */ { 0, BX_IA_XOR_GbEb },
#endif
};

// opcode 33
static const BxOpcodeInfo_t BxOpcodeTable33[] = {
  /* 33 /w */ { 0, BX_IA_XOR_GwEw },
  /* 33 /d */ { 0, BX_IA_XOR_GdEd },
#if BX_SUPPORT_X86_64
  /* 33 /q */ { 0, BX_IA_XOR_GqEq },
#endif
};

// opcode 34
static const BxOpcodeInfo_t BxOpcodeTable34[] = {
  /* 34 /w */ { BxImmediate_Ib, BX_IA_XOR_ALIb },
  /* 34 /d */ { BxImmediate_Ib, BX_IA_XOR_ALIb },
#if BX_SUPPORT_X86_64
  /* 34 /q */ { BxImmediate_Ib, BX_IA_XOR_ALIb },
#endif
};

// opcode 35
static const BxOpcodeInfo_t BxOpcodeTable35[] = {
  /* 35 /w */ { BxImmediate_Iw, BX_IA_XOR_AXIw },
  /* 35 /d */ { BxImmediate_Id, BX_IA_XOR_EAXId },
#if BX_SUPPORT_X86_64
  /* 35 /q */ { BxImmediate_Id, BX_IA_XOR_RAXId },
#endif
};

// opcode 37
static const BxOpcodeInfo_t BxOpcodeTable37_32[] = {
  /* 37 */ { 0, BX_IA_AAA },
};

// opcode 38
static const BxOpcodeInfo_t BxOpcodeTable38[] = {
  /* 38 /w */ { 0, BX_IA_CMP_EbGb },
  /* 38 /d */ { 0, BX_IA_CMP_EbGb },
#if BX_SUPPORT_X86_64
  /* 38 /q */ { 0, BX_IA_CMP_EbGb },
#endif
};

// opcode 39
static const BxOpcodeInfo_t BxOpcodeTable39[] = {
  /* 39 /w */ { 0, BX_IA_CMP_EwGw },
  /* 39 /d */ { 0, BX_IA_CMP_EdGd },
#if BX_SUPPORT_X86_64
  /* 39 /q */ { 0, BX_IA_CMP_EqGq },
#endif
};

// opcode 3A
static const BxOpcodeInfo_t BxOpcodeTable3A[] = {
  /* 3A /w */ { 0, BX_IA_CMP_GbEb },
  /* 3A /d */ { 0, BX_IA_CMP_GbEb },
#if BX_SUPPORT_X86_64
  /* 3A /q */ { 0, BX_IA_CMP_GbEb },
#endif
};

// opcode 3B
static const BxOpcodeInfo_t BxOpcodeTable3B[] = {
  /* 3B /w */ { 0, BX_IA_CMP_GwEw },
  /* 3B /d */ { 0, BX_IA_CMP_GdEd },
#if BX_SUPPORT_X86_64
  /* 3B /q */ { 0, BX_IA_CMP_GqEq },
#endif
};

// opcode 3C
static const BxOpcodeInfo_t BxOpcodeTable3C[] = {
  /* 3C /w */ { BxImmediate_Ib, BX_IA_CMP_ALIb },
  /* 3C /d */ { BxImmediate_Ib, BX_IA_CMP_ALIb },
#if BX_SUPPORT_X86_64
  /* 3C /q */ { BxImmediate_Ib, BX_IA_CMP_ALIb },
#endif
};

// opcode 3D
static const BxOpcodeInfo_t BxOpcodeTable3D[] = {
  /* 3D /w */ { BxImmediate_Iw, BX_IA_CMP_AXIw },
  /* 3D /d */ { BxImmediate_Id, BX_IA_CMP_EAXId },
#if BX_SUPPORT_X86_64
  /* 3D /q */ { BxImmediate_Id, BX_IA_CMP_RAXId },
#endif
};

// opcode 3F
static const BxOpcodeInfo_t BxOpcodeTable3F_32[] = {
  /* 3F */ { 0, BX_IA_AAS },
};

// opcode 40 - 47
static const BxOpcodeInfo_t BxOpcodeTable40x47_32[] = {
  /* 40 /w */ { 0, BX_IA_INC_Ew },
  /* 40 /d */ { 0, BX_IA_INC_Ed },
};

// opcode 48 - 4F
static const BxOpcodeInfo_t BxOpcodeTable48x4F_32[] = {
  /* 49 /w */ { 0, BX_IA_DEC_Ew },
  /* 49 /d */ { 0, BX_IA_DEC_Ed },
};

// opcode 50 - 57
static const BxOpcodeInfo_t BxOpcodeTable50x57_32[] = {
  /* 50 /w */ { 0, BX_IA_PUSH_Ew },
  /* 50 /d */ { 0, BX_IA_PUSH_Ed },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable50x57_64[] = {
  /* 50 /w */ { 0, BX_IA_PUSH_Ew },
  /* 50 /d */ { 0, BX_IA_PUSH_Eq },
  /* 50 /q */ { 0, BX_IA_PUSH_Eq },
};
#endif

// opcode 58 - 5F
static const BxOpcodeInfo_t BxOpcodeTable58x5F_32[] = {
  /* 58 /w */ { 0, BX_IA_POP_Ew },
  /* 58 /d */ { 0, BX_IA_POP_Ed },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable58x5F_64[] = {
  /* 58 /w */ { 0, BX_IA_POP_Ew },
  /* 58 /d */ { 0, BX_IA_POP_Eq },
  /* 58 /q */ { 0, BX_IA_POP_Eq },
};
#endif

// opcode 60
static const BxOpcodeInfo_t BxOpcodeTable60_32[] = {
  /* 60 /w */ { 0, BX_IA_PUSHA_Op16 },
  /* 60 /d */ { 0, BX_IA_PUSHA_Op32 },
};

// opcode 61
static const BxOpcodeInfo_t BxOpcodeTable61_32[] = {
  /* 61 /w */ { 0, BX_IA_POPA_Op16 },
  /* 61 /d */ { 0, BX_IA_POPA_Op32 },
};

// opcode 62
static const BxOpcodeInfo_t BxOpcodeTable62_32[] = {
  /* 62 /w */ { 0, BX_IA_BOUND_GwMa },
  /* 62 /d */ { 0, BX_IA_BOUND_GdMa },
};

// opcode 63
static const BxOpcodeInfo_t BxOpcodeTable63_32[] = {
  /* 63 /w */ { 0, BX_IA_ARPL_EwGw },
  /* 63 /d */ { 0, BX_IA_ARPL_EwGw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable63_64[] = {
  /* 63 /w */ { 0, BX_IA_MOV_GwEw },        // MOVSX_GwEw
  /* 63 /d */ { 0, BX_IA_MOV_Op64_GdEd },   // MOVSX_GdEd
  /* 63 /q */ { 0, BX_IA_MOVSXD_GqEd },
};
#endif

// opcode 68
static const BxOpcodeInfo_t BxOpcodeTable68_32[] = {
  /* 68 /w */ { BxImmediate_Iw, BX_IA_PUSH_Iw },
  /* 68 /d */ { BxImmediate_Id, BX_IA_PUSH_Id },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable68_64[] = {
  /* 68 /w */ { BxImmediate_Iw, BX_IA_PUSH_Iw },
  /* 68 /d */ { BxImmediate_Id, BX_IA_PUSH_Op64_Id },
  /* 68 /q */ { BxImmediate_Id, BX_IA_PUSH_Op64_Id },
};
#endif

// opcode 69
static const BxOpcodeInfo_t BxOpcodeTable69[] = {
  /* 69 /w */ { BxImmediate_Iw, BX_IA_IMUL_GwEwIw },
  /* 69 /d */ { BxImmediate_Id, BX_IA_IMUL_GdEdId },
#if BX_SUPPORT_X86_64
  /* 69 /q */ { BxImmediate_Id, BX_IA_IMUL_GqEqId },
#endif
};

// opcode 6A
static const BxOpcodeInfo_t BxOpcodeTable6A_32[] = {
  /* 6A /w */ { BxImmediate_Ib_SE, BX_IA_PUSH_Iw },
  /* 6A /d */ { BxImmediate_Ib_SE, BX_IA_PUSH_Id },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable6A_64[] = {
  /* 6A /w */ { BxImmediate_Ib_SE, BX_IA_PUSH_Iw },
  /* 6A /d */ { BxImmediate_Ib_SE, BX_IA_PUSH_Op64_Id },
  /* 6A /q */ { BxImmediate_Ib_SE, BX_IA_PUSH_Op64_Id },
};
#endif

// opcode 6B
static const BxOpcodeInfo_t BxOpcodeTable6B[] = {
  /* 6B /w */ { BxImmediate_Ib_SE, BX_IA_IMUL_GwEwIw },
  /* 6B /d */ { BxImmediate_Ib_SE, BX_IA_IMUL_GdEdId },
#if BX_SUPPORT_X86_64
  /* 6B /q */ { BxImmediate_Ib_SE, BX_IA_IMUL_GqEqId },
#endif
};

// opcode 6C
static const BxOpcodeInfo_t BxOpcodeTable6C[] = {
  /* 6C /w */ { 0, BX_IA_REP_INSB_YbDX },
  /* 6C /d */ { 0, BX_IA_REP_INSB_YbDX },
#if BX_SUPPORT_X86_64
  /* 6C /q */ { 0, BX_IA_REP_INSB_YbDX },
#endif
};

// opcode 6D
static const BxOpcodeInfo_t BxOpcodeTable6D[] = {
  /* 6D /w */ { 0, BX_IA_REP_INSW_YwDX },
  /* 6D /d */ { 0, BX_IA_REP_INSD_YdDX },
#if BX_SUPPORT_X86_64
  /* 6D /q */ { 0, BX_IA_REP_INSD_YdDX },
#endif
};

// opcode 6E
static const BxOpcodeInfo_t BxOpcodeTable6E[] = {
  /* 6E /w */ { 0, BX_IA_REP_OUTSB_DXXb },
  /* 6E /d */ { 0, BX_IA_REP_OUTSB_DXXb },
#if BX_SUPPORT_X86_64
  /* 6E /q */ { 0, BX_IA_REP_OUTSB_DXXb },
#endif
};

// opcode 6F
static const BxOpcodeInfo_t BxOpcodeTable6F[] = {
  /* 6F /w */ { 0, BX_IA_REP_OUTSW_DXXw },
  /* 6F /d */ { 0, BX_IA_REP_OUTSD_DXXd },
#if BX_SUPPORT_X86_64
  /* 6F /q */ { 0, BX_IA_REP_OUTSD_DXXd },
#endif
};

// opcode 70
static const BxOpcodeInfo_t BxOpcodeTable70_32[] = {
  /* 70 /w */ { BxImmediate_BrOff8, BX_IA_JO_Jw },
  /* 70 /d */ { BxImmediate_BrOff8, BX_IA_JO_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable70_64[] = {
  /* 70 /w */ { BxImmediate_BrOff8, BX_IA_JO_Jq },
  /* 70 /d */ { BxImmediate_BrOff8, BX_IA_JO_Jq },
  /* 70 /q */ { BxImmediate_BrOff8, BX_IA_JO_Jq },
};
#endif

// opcode 71
static const BxOpcodeInfo_t BxOpcodeTable71_32[] = {
  /* 71 /w */ { BxImmediate_BrOff8, BX_IA_JNO_Jw },
  /* 71 /d */ { BxImmediate_BrOff8, BX_IA_JNO_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable71_64[] = {
  /* 71 /w */ { BxImmediate_BrOff8, BX_IA_JNO_Jq },
  /* 71 /d */ { BxImmediate_BrOff8, BX_IA_JNO_Jq },
  /* 71 /q */ { BxImmediate_BrOff8, BX_IA_JNO_Jq },
};
#endif

// opcode 72
static const BxOpcodeInfo_t BxOpcodeTable72_32[] = {
  /* 72 /w */ { BxImmediate_BrOff8, BX_IA_JB_Jw },
  /* 72 /d */ { BxImmediate_BrOff8, BX_IA_JB_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable72_64[] = {
  /* 72 /w */ { BxImmediate_BrOff8, BX_IA_JB_Jq },
  /* 72 /d */ { BxImmediate_BrOff8, BX_IA_JB_Jq },
  /* 72 /q */ { BxImmediate_BrOff8, BX_IA_JB_Jq },
};
#endif

// opcode 73
static const BxOpcodeInfo_t BxOpcodeTable73_32[] = {
  /* 73 /w */ { BxImmediate_BrOff8, BX_IA_JNB_Jw },
  /* 73 /d */ { BxImmediate_BrOff8, BX_IA_JNB_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable73_64[] = {
  /* 73 /w */ { BxImmediate_BrOff8, BX_IA_JNB_Jq },
  /* 73 /d */ { BxImmediate_BrOff8, BX_IA_JNB_Jq },
  /* 73 /q */ { BxImmediate_BrOff8, BX_IA_JNB_Jq },
};
#endif

// opcode 74
static const BxOpcodeInfo_t BxOpcodeTable74_32[] = {
  /* 74 /w */ { BxImmediate_BrOff8, BX_IA_JZ_Jw },
  /* 74 /d */ { BxImmediate_BrOff8, BX_IA_JZ_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable74_64[] = {
  /* 74 /w */ { BxImmediate_BrOff8, BX_IA_JZ_Jq },
  /* 74 /d */ { BxImmediate_BrOff8, BX_IA_JZ_Jq },
  /* 74 /q */ { BxImmediate_BrOff8, BX_IA_JZ_Jq },
};
#endif

// opcode 75
static const BxOpcodeInfo_t BxOpcodeTable75_32[] = {
  /* 75 /w */ { BxImmediate_BrOff8, BX_IA_JNZ_Jw },
  /* 75 /d */ { BxImmediate_BrOff8, BX_IA_JNZ_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable75_64[] = {
  /* 75 /w */ { BxImmediate_BrOff8, BX_IA_JNZ_Jq },
  /* 75 /d */ { BxImmediate_BrOff8, BX_IA_JNZ_Jq },
  /* 75 /q */ { BxImmediate_BrOff8, BX_IA_JNZ_Jq },
};
#endif

// opcode 76
static const BxOpcodeInfo_t BxOpcodeTable76_32[] = {
  /* 76 /w */ { BxImmediate_BrOff8, BX_IA_JBE_Jw },
  /* 76 /d */ { BxImmediate_BrOff8, BX_IA_JBE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable76_64[] = {
  /* 76 /w */ { BxImmediate_BrOff8, BX_IA_JBE_Jq },
  /* 76 /d */ { BxImmediate_BrOff8, BX_IA_JBE_Jq },
  /* 76 /q */ { BxImmediate_BrOff8, BX_IA_JBE_Jq },
};
#endif

// opcode 77
static const BxOpcodeInfo_t BxOpcodeTable77_32[] = {
  /* 77 /w */ { BxImmediate_BrOff8, BX_IA_JNBE_Jw },
  /* 77 /d */ { BxImmediate_BrOff8, BX_IA_JNBE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable77_64[] = {
  /* 77 /w */ { BxImmediate_BrOff8, BX_IA_JNBE_Jq },
  /* 77 /d */ { BxImmediate_BrOff8, BX_IA_JNBE_Jq },
  /* 77 /q */ { BxImmediate_BrOff8, BX_IA_JNBE_Jq },
};
#endif

// opcode 78
static const BxOpcodeInfo_t BxOpcodeTable78_32[] = {
  /* 78 /w */ { BxImmediate_BrOff8, BX_IA_JS_Jw },
  /* 78 /d */ { BxImmediate_BrOff8, BX_IA_JS_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable78_64[] = {
  /* 78 /w */ { BxImmediate_BrOff8, BX_IA_JS_Jq },
  /* 78 /d */ { BxImmediate_BrOff8, BX_IA_JS_Jq },
  /* 78 /q */ { BxImmediate_BrOff8, BX_IA_JS_Jq },
};
#endif

// opcode 79
static const BxOpcodeInfo_t BxOpcodeTable79_32[] = {
  /* 79 /w */ { BxImmediate_BrOff8, BX_IA_JNS_Jw },
  /* 79 /d */ { BxImmediate_BrOff8, BX_IA_JNS_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable79_64[] = {
  /* 79 /w */ { BxImmediate_BrOff8, BX_IA_JNS_Jq },
  /* 79 /d */ { BxImmediate_BrOff8, BX_IA_JNS_Jq },
  /* 79 /q */ { BxImmediate_BrOff8, BX_IA_JNS_Jq },
};
#endif

// opcode 7A
static const BxOpcodeInfo_t BxOpcodeTable7A_32[] = {
  /* 7A /w */ { BxImmediate_BrOff8, BX_IA_JP_Jw },
  /* 7A /d */ { BxImmediate_BrOff8, BX_IA_JP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7A_64[] = {
  /* 7A /w */ { BxImmediate_BrOff8, BX_IA_JP_Jq },
  /* 7A /d */ { BxImmediate_BrOff8, BX_IA_JP_Jq },
  /* 7A /q */ { BxImmediate_BrOff8, BX_IA_JP_Jq },
};
#endif

// opcode 7B
static const BxOpcodeInfo_t BxOpcodeTable7B_32[] = {
  /* 7B /w */ { BxImmediate_BrOff8, BX_IA_JNP_Jw },
  /* 7B /d */ { BxImmediate_BrOff8, BX_IA_JNP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7B_64[] = {
  /* 7B /w */ { BxImmediate_BrOff8, BX_IA_JNP_Jq },
  /* 7B /d */ { BxImmediate_BrOff8, BX_IA_JNP_Jq },
  /* 7B /q */ { BxImmediate_BrOff8, BX_IA_JNP_Jq },
};
#endif

// opcode 7C
static const BxOpcodeInfo_t BxOpcodeTable7C_32[] = {
  /* 7C /w */ { BxImmediate_BrOff8, BX_IA_JL_Jw },
  /* 7C /d */ { BxImmediate_BrOff8, BX_IA_JL_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7C_64[] = {
  /* 7C /w */ { BxImmediate_BrOff8, BX_IA_JL_Jq },
  /* 7C /d */ { BxImmediate_BrOff8, BX_IA_JL_Jq },
  /* 7C /q */ { BxImmediate_BrOff8, BX_IA_JL_Jq },
};
#endif

// opcode 7D
static const BxOpcodeInfo_t BxOpcodeTable7D_32[] = {
  /* 7D /w */ { BxImmediate_BrOff8, BX_IA_JNL_Jw },
  /* 7D /d */ { BxImmediate_BrOff8, BX_IA_JNL_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7D_64[] = {
  /* 7D /w */ { BxImmediate_BrOff8, BX_IA_JNL_Jq },
  /* 7D /d */ { BxImmediate_BrOff8, BX_IA_JNL_Jq },
  /* 7D /q */ { BxImmediate_BrOff8, BX_IA_JNL_Jq },
};
#endif

// opcode 7E
static const BxOpcodeInfo_t BxOpcodeTable7E_32[] = {
  /* 7E /w */ { BxImmediate_BrOff8, BX_IA_JLE_Jw },
  /* 7E /d */ { BxImmediate_BrOff8, BX_IA_JLE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7E_64[] = {
  /* 7E /w */ { BxImmediate_BrOff8, BX_IA_JLE_Jq },
  /* 7E /d */ { BxImmediate_BrOff8, BX_IA_JLE_Jq },
  /* 7E /q */ { BxImmediate_BrOff8, BX_IA_JLE_Jq },
};
#endif

// opcode 7F
static const BxOpcodeInfo_t BxOpcodeTable7F_32[] = {
  /* 7F /w */ { BxImmediate_BrOff8, BX_IA_JNLE_Jw },
  /* 7F /d */ { BxImmediate_BrOff8, BX_IA_JNLE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable7F_64[] = {
  /* 7F /w */ { BxImmediate_BrOff8, BX_IA_JNLE_Jq },
  /* 7F /d */ { BxImmediate_BrOff8, BX_IA_JNLE_Jq },
  /* 7F /q */ { BxImmediate_BrOff8, BX_IA_JNLE_Jq },
};
#endif

// opcode 80
static const BxOpcodeInfo_t BxOpcodeTable80_G1Eb[8] = {
  /* /0 */ { BxImmediate_Ib, BX_IA_ADD_EbIb },
  /* /1 */ { BxImmediate_Ib, BX_IA_OR_EbIb  },
  /* /2 */ { BxImmediate_Ib, BX_IA_ADC_EbIb },
  /* /3 */ { BxImmediate_Ib, BX_IA_SBB_EbIb },
  /* /4 */ { BxImmediate_Ib, BX_IA_AND_EbIb },
  /* /5 */ { BxImmediate_Ib, BX_IA_SUB_EbIb },
  /* /6 */ { BxImmediate_Ib, BX_IA_XOR_EbIb },
  /* /7 */ { BxImmediate_Ib, BX_IA_CMP_EbIb }
};

// opcode 81
static const BxOpcodeInfo_t BxOpcodeTable81_G1Ev[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_Iw, BX_IA_ADD_EwIw },
  /* /1 /w */ { BxImmediate_Iw, BX_IA_OR_EwIw  },
  /* /2 /w */ { BxImmediate_Iw, BX_IA_ADC_EwIw },
  /* /3 /w */ { BxImmediate_Iw, BX_IA_SBB_EwIw },
  /* /4 /w */ { BxImmediate_Iw, BX_IA_AND_EwIw },
  /* /5 /w */ { BxImmediate_Iw, BX_IA_SUB_EwIw },
  /* /6 /w */ { BxImmediate_Iw, BX_IA_XOR_EwIw },
  /* /7 /w */ { BxImmediate_Iw, BX_IA_CMP_EwIw },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_Id, BX_IA_ADD_EdId },
  /* /1 /d */ { BxImmediate_Id, BX_IA_OR_EdId  },
  /* /2 /d */ { BxImmediate_Id, BX_IA_ADC_EdId },
  /* /3 /d */ { BxImmediate_Id, BX_IA_SBB_EdId },
  /* /4 /d */ { BxImmediate_Id, BX_IA_AND_EdId },
  /* /5 /d */ { BxImmediate_Id, BX_IA_SUB_EdId },
  /* /6 /d */ { BxImmediate_Id, BX_IA_XOR_EdId },
  /* /7 /d */ { BxImmediate_Id, BX_IA_CMP_EdId },

#if BX_SUPPORT_X86_64
  /* /0 /q */ { BxImmediate_Id, BX_IA_ADD_EqId },
  /* /1 /q */ { BxImmediate_Id, BX_IA_OR_EqId },
  /* /2 /q */ { BxImmediate_Id, BX_IA_ADC_EqId },
  /* /3 /q */ { BxImmediate_Id, BX_IA_SBB_EqId },
  /* /4 /q */ { BxImmediate_Id, BX_IA_AND_EqId },
  /* /5 /q */ { BxImmediate_Id, BX_IA_SUB_EqId },
  /* /6 /q */ { BxImmediate_Id, BX_IA_XOR_EqId },
  /* /7 /q */ { BxImmediate_Id, BX_IA_CMP_EqId },
#endif
};

// opcode 83
static const BxOpcodeInfo_t BxOpcodeTable83_G1Ev[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_Ib_SE, BX_IA_ADD_EwIw },
  /* /1 /w */ { BxImmediate_Ib_SE, BX_IA_OR_EwIw  },
  /* /2 /w */ { BxImmediate_Ib_SE, BX_IA_ADC_EwIw },
  /* /3 /w */ { BxImmediate_Ib_SE, BX_IA_SBB_EwIw },
  /* /4 /w */ { BxImmediate_Ib_SE, BX_IA_AND_EwIw },
  /* /5 /w */ { BxImmediate_Ib_SE, BX_IA_SUB_EwIw },
  /* /6 /w */ { BxImmediate_Ib_SE, BX_IA_XOR_EwIw },
  /* /7 /w */ { BxImmediate_Ib_SE, BX_IA_CMP_EwIw },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_Ib_SE, BX_IA_ADD_EdId },
  /* /1 /d */ { BxImmediate_Ib_SE, BX_IA_OR_EdId  },
  /* /2 /d */ { BxImmediate_Ib_SE, BX_IA_ADC_EdId },
  /* /3 /d */ { BxImmediate_Ib_SE, BX_IA_SBB_EdId },
  /* /4 /d */ { BxImmediate_Ib_SE, BX_IA_AND_EdId },
  /* /5 /d */ { BxImmediate_Ib_SE, BX_IA_SUB_EdId },
  /* /6 /d */ { BxImmediate_Ib_SE, BX_IA_XOR_EdId },
  /* /7 /d */ { BxImmediate_Ib_SE, BX_IA_CMP_EdId },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { BxImmediate_Ib_SE, BX_IA_ADD_EqId },
  /* /1 /q */ { BxImmediate_Ib_SE, BX_IA_OR_EqId },
  /* /2 /q */ { BxImmediate_Ib_SE, BX_IA_ADC_EqId },
  /* /3 /q */ { BxImmediate_Ib_SE, BX_IA_SBB_EqId },
  /* /4 /q */ { BxImmediate_Ib_SE, BX_IA_AND_EqId },
  /* /5 /q */ { BxImmediate_Ib_SE, BX_IA_SUB_EqId },
  /* /6 /q */ { BxImmediate_Ib_SE, BX_IA_XOR_EqId },
  /* /7 /q */ { BxImmediate_Ib_SE, BX_IA_CMP_EqId },
#endif
};

// opcode 84
static const BxOpcodeInfo_t BxOpcodeTable84[] = {
  /* 84 /w */ { 0, BX_IA_TEST_EbGb },
  /* 84 /d */ { 0, BX_IA_TEST_EbGb },
#if BX_SUPPORT_X86_64
  /* 84 /q */ { 0, BX_IA_TEST_EbGb },
#endif
};

// opcode 85
static const BxOpcodeInfo_t BxOpcodeTable85[] = {
  /* 85 /w */ { 0, BX_IA_TEST_EwGw },
  /* 85 /d */ { 0, BX_IA_TEST_EdGd },
#if BX_SUPPORT_X86_64
  /* 85 /q */ { 0, BX_IA_TEST_EqGq },
#endif
};

// opcode 86
static const BxOpcodeInfo_t BxOpcodeTable86[] = {
  /* 86 /w */ { 0, BX_IA_XCHG_EbGb },
  /* 86 /d */ { 0, BX_IA_XCHG_EbGb },
#if BX_SUPPORT_X86_64
  /* 86 /q */ { 0, BX_IA_XCHG_EbGb },
#endif
};

// opcode 87
static const BxOpcodeInfo_t BxOpcodeTable87[] = {
  /* 87 /w */ { 0, BX_IA_XCHG_EwGw },
  /* 87 /d */ { 0, BX_IA_XCHG_EdGd },
#if BX_SUPPORT_X86_64
  /* 87 /q */ { 0, BX_IA_XCHG_EqGq },
#endif
};

// opcode 88
static const BxOpcodeInfo_t BxOpcodeTable88[] = {
  /* 88 /w */ { 0, BX_IA_MOV_EbGb },
  /* 88 /d */ { 0, BX_IA_MOV_EbGb },
#if BX_SUPPORT_X86_64
  /* 88 /q */ { 0, BX_IA_MOV_EbGb },
#endif
};

// opcode 89 - split for better emulation performance
static const BxOpcodeInfo_t BxOpcodeTable89_32[] = {
  /* 89 /w */ { 0, BX_IA_MOV_EwGw },
  /* 89 /d */ { 0, BX_IA_MOV_Op32_EdGd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable89_64[] = {
  /* 89 /w */ { 0, BX_IA_MOV_EwGw },
  /* 89 /d */ { 0, BX_IA_MOV_Op64_EdGd },
  /* 89 /q */ { 0, BX_IA_MOV_EqGq },
};
#endif

// opcode 8A
static const BxOpcodeInfo_t BxOpcodeTable8A[] = {
  /* 8A /w */ { 0, BX_IA_MOV_GbEb },
  /* 8A /d */ { 0, BX_IA_MOV_GbEb },
#if BX_SUPPORT_X86_64
  /* 8A /q */ { 0, BX_IA_MOV_GbEb },
#endif
};

// opcode 8B - split for better emulation performance
static const BxOpcodeInfo_t BxOpcodeTable8B_32[] = {
  /* 8B /w */ { 0, BX_IA_MOV_GwEw },
  /* 8B /d */ { 0, BX_IA_MOV_Op32_GdEd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable8B_64[] = {
  /* 8B /w */ { 0, BX_IA_MOV_GwEw },
  /* 8B /d */ { 0, BX_IA_MOV_Op64_GdEd },
  /* 8B /q */ { 0, BX_IA_MOV_GqEq },
};
#endif

// opcode 8C
static const BxOpcodeInfo_t BxOpcodeTable8C[] = {
  /* 8C /w */ { 0, BX_IA_MOV_EwSw },
  /* 8C /d */ { 0, BX_IA_MOV_EwSw },
#if BX_SUPPORT_X86_64
  /* 8C /q */ { 0, BX_IA_MOV_EwSw },
#endif
};

// opcode 8D
static const BxOpcodeInfo_t BxOpcodeTable8D[] = {
  /* 8D /w */ { 0, BX_IA_LEA_GwM },
  /* 8D /d */ { 0, BX_IA_LEA_GdM },
#if BX_SUPPORT_X86_64
  /* 8D /q */ { 0, BX_IA_LEA_GqM },
#endif
};

// opcode 8E
static const BxOpcodeInfo_t BxOpcodeTable8E[] = {
  /* 8E /w */ { 0, BX_IA_MOV_SwEw },
  /* 8E /d */ { 0, BX_IA_MOV_SwEw },
#if BX_SUPPORT_X86_64
  /* 8E /q */ { 0, BX_IA_MOV_SwEw },
#endif
};

// opcode 8F - GroupN / XOP prefix
static const BxOpcodeInfo_t BxOpcodeTable8F[] = {
  /* 8F /w */ { 0, BX_IA_POP_Ew },
  /* 8F /d */ { 0, BX_IA_POP_Ed },
#if BX_SUPPORT_X86_64
  /* 8F /q */ { 0, BX_IA_POP_Eq },
#endif
};

// opcode 90 - 97
static const BxOpcodeInfo_t BxOpcodeTable90x97[] = {
  /* 91 /w */ { 0, BX_IA_XCHG_RXAX },   // handles also XCHG R8w, AX
  /* 91 /d */ { 0, BX_IA_XCHG_ERXEAX }, // handles also XCHG R8d, EAX
#if BX_SUPPORT_X86_64
  /* 91 /q */ { 0, BX_IA_XCHG_RRXRAX }, // handles also XCHG R8, RAX
#endif
};

// opcode 98
static const BxOpcodeInfo_t BxOpcodeTable98[] = {
  /* 98 /w */ { 0, BX_IA_CBW },
  /* 98 /d */ { 0, BX_IA_CWDE },
#if BX_SUPPORT_X86_64
  /* 98 /q */ { 0, BX_IA_CDQE },
#endif
};

// opcode 99
static const BxOpcodeInfo_t BxOpcodeTable99[] = {
  /* 99 /w */ { 0, BX_IA_CWD },
  /* 99 /d */ { 0, BX_IA_CDQ },
#if BX_SUPPORT_X86_64
  /* 99 /q */ { 0, BX_IA_CQO },
#endif
};

// opcode 9A
static const BxOpcodeInfo_t BxOpcodeTable9A_32[] = {
  /* 9A /w */ { BxImmediate_Iw | BxImmediate_Iw2, BX_IA_CALLF_Op16_Ap },
  /* 9A /d */ { BxImmediate_Id | BxImmediate_Iw2, BX_IA_CALLF_Op32_Ap },
};

// opcode 9B
static const BxOpcodeInfo_t BxOpcodeTable9B[] = {
  /* 9B */ { 0, BX_IA_FWAIT },
};

// opcode 9C
static const BxOpcodeInfo_t BxOpcodeTable9C_32[] = {
  /* 9C /w */ { 0, BX_IA_PUSHF_Fw },
  /* 9C /d */ { 0, BX_IA_PUSHF_Fd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable9C_64[] = {
  /* 9C /w */ { 0, BX_IA_PUSHF_Fw },
  /* 9C /d */ { 0, BX_IA_PUSHF_Fq },
  /* 9C /q */ { 0, BX_IA_PUSHF_Fq },
};
#endif

// opcode 9D
static const BxOpcodeInfo_t BxOpcodeTable9D_32[] = {
  /* 9D /w */ { 0, BX_IA_POPF_Fw },
  /* 9D /d */ { 0, BX_IA_POPF_Fd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable9D_64[] = {
  /* 9D /w */ { 0, BX_IA_POPF_Fw },
  /* 9D /d */ { 0, BX_IA_POPF_Fq },
  /* 9D /q */ { 0, BX_IA_POPF_Fq },
};
#endif

// opcode 9E
static const BxOpcodeInfo_t BxOpcodeTable9E_32[] = {
  /* 9E */ { 0, BX_IA_SAHF },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable9E_64[] = {
  /* 9E */ { 0, BX_IA_SAHF_LM },
};
#endif

// opcode 9F
static const BxOpcodeInfo_t BxOpcodeTable9F_32[] = {
  /* 9F */ { 0, BX_IA_LAHF },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable9F_64[] = {
  /* 9F */ { 0, BX_IA_LAHF_LM },
};
#endif

// opcode A0
static const BxOpcodeInfo_t BxOpcodeTableA0_32[] = {
  /* A0 /w */ { BxImmediate_O, BX_IA_MOV_ALOd },
  /* A0 /d */ { BxImmediate_O, BX_IA_MOV_ALOd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableA0_64[] = {
  /* A0 /w */ { BxImmediate_O, BX_IA_MOV_ALOq },
  /* A0 /d */ { BxImmediate_O, BX_IA_MOV_ALOq },
  /* A0 /q */ { BxImmediate_O, BX_IA_MOV_ALOq },
};
#endif

// opcode A1
static const BxOpcodeInfo_t BxOpcodeTableA1_32[] = {
  /* A1 /w */ { BxImmediate_O, BX_IA_MOV_AXOd },
  /* A1 /d */ { BxImmediate_O, BX_IA_MOV_EAXOd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableA1_64[] = {
  /* A1 /w */ { BxImmediate_O, BX_IA_MOV_AXOq },
  /* A1 /d */ { BxImmediate_O, BX_IA_MOV_EAXOq },
  /* A1 /q */ { BxImmediate_O, BX_IA_MOV_RAXOq },
};
#endif

// opcode A2
static const BxOpcodeInfo_t BxOpcodeTableA2_32[] = {
  /* A2 /w */ { BxImmediate_O, BX_IA_MOV_OdAL },
  /* A2 /d */ { BxImmediate_O, BX_IA_MOV_OdAL },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableA2_64[] = {
  /* A2 /w */ { BxImmediate_O, BX_IA_MOV_OqAL },
  /* A2 /d */ { BxImmediate_O, BX_IA_MOV_OqAL },
  /* A2 /q */ { BxImmediate_O, BX_IA_MOV_OqAL },
};
#endif

// opcode A3
static const BxOpcodeInfo_t BxOpcodeTableA3_32[] = {
  /* A3 /w */ { BxImmediate_O, BX_IA_MOV_OdAX },
  /* A3 /d */ { BxImmediate_O, BX_IA_MOV_OdEAX },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableA3_64[] = {
  /* A3 /w */ { BxImmediate_O, BX_IA_MOV_OqAX },
  /* A3 /d */ { BxImmediate_O, BX_IA_MOV_OqEAX },
  /* A3 /q */ { BxImmediate_O, BX_IA_MOV_OqRAX },
};
#endif

// opcode A4
static const BxOpcodeInfo_t BxOpcodeTableA4[] = {
  /* A4 /w */ { 0, BX_IA_REP_MOVSB_YbXb },
  /* A4 /d */ { 0, BX_IA_REP_MOVSB_YbXb },
#if BX_SUPPORT_X86_64
  /* A4 /q */ { 0, BX_IA_REP_MOVSB_YbXb },
#endif
};

// opcode A5
static const BxOpcodeInfo_t BxOpcodeTableA5[] = {
  /* A5 /w */ { 0, BX_IA_REP_MOVSW_YwXw },
  /* A5 /d */ { 0, BX_IA_REP_MOVSD_YdXd },
#if BX_SUPPORT_X86_64
  /* A5 /q */ { 0, BX_IA_REP_MOVSQ_YqXq },
#endif
};

// opcode A6
static const BxOpcodeInfo_t BxOpcodeTableA6[] = {
  /* A6 /w */ { 0, BX_IA_REP_CMPSB_XbYb },
  /* A6 /d */ { 0, BX_IA_REP_CMPSB_XbYb },
#if BX_SUPPORT_X86_64
  /* A6 /q */ { 0, BX_IA_REP_CMPSB_XbYb },
#endif
};

// opcode A7
static const BxOpcodeInfo_t BxOpcodeTableA7[] = {
  /* A7 /w */ { 0, BX_IA_REP_CMPSW_XwYw },
  /* A7 /d */ { 0, BX_IA_REP_CMPSD_XdYd },
#if BX_SUPPORT_X86_64
  /* A7 /q */ { 0, BX_IA_REP_CMPSQ_XqYq },
#endif
};

// opcode A8
static const BxOpcodeInfo_t BxOpcodeTableA8[] = {
  /* A8 /w */ { BxImmediate_Ib, BX_IA_TEST_ALIb },
  /* A8 /d */ { BxImmediate_Ib, BX_IA_TEST_ALIb },
#if BX_SUPPORT_X86_64
  /* A8 /q */ { BxImmediate_Ib, BX_IA_TEST_ALIb },
#endif
};

// opcode A9
static const BxOpcodeInfo_t BxOpcodeTableA9[] = {
  /* A9 /w */ { BxImmediate_Iw, BX_IA_TEST_AXIw },
  /* A9 /d */ { BxImmediate_Id, BX_IA_TEST_EAXId },
#if BX_SUPPORT_X86_64
  /* A9 /q */ { BxImmediate_Id, BX_IA_TEST_RAXId },
#endif
};

// opcode AA
static const BxOpcodeInfo_t BxOpcodeTableAA[] = {
  /* AA /w */ { 0, BX_IA_REP_STOSB_YbAL },
  /* AA /d */ { 0, BX_IA_REP_STOSB_YbAL },
#if BX_SUPPORT_X86_64
  /* AA /q */ { 0, BX_IA_REP_STOSB_YbAL },
#endif
};

// opcode AB
static const BxOpcodeInfo_t BxOpcodeTableAB[] = {
  /* AB /w */ { 0, BX_IA_REP_STOSW_YwAX },
  /* AB /d */ { 0, BX_IA_REP_STOSD_YdEAX },
#if BX_SUPPORT_X86_64
  /* AB /q */ { 0, BX_IA_REP_STOSQ_YqRAX },
#endif
};

// opcode AC
static const BxOpcodeInfo_t BxOpcodeTableAC[] = {
  /* AC /w */ { 0, BX_IA_REP_LODSB_ALXb },
  /* AC /d */ { 0, BX_IA_REP_LODSB_ALXb },
#if BX_SUPPORT_X86_64
  /* AC /q */ { 0, BX_IA_REP_LODSB_ALXb },
#endif
};

// opcode AD
static const BxOpcodeInfo_t BxOpcodeTableAD[] = {
  /* AD /w */ { 0, BX_IA_REP_LODSW_AXXw },
  /* AD /d */ { 0, BX_IA_REP_LODSD_EAXXd },
#if BX_SUPPORT_X86_64
  /* AD /q */ { 0, BX_IA_REP_LODSQ_RAXXq },
#endif
};

// opcode AE
static const BxOpcodeInfo_t BxOpcodeTableAE[] = {
  /* AE /w */ { 0, BX_IA_REP_SCASB_ALYb },
  /* AE /d */ { 0, BX_IA_REP_SCASB_ALYb  },
#if BX_SUPPORT_X86_64
  /* AE /q */ { 0, BX_IA_REP_SCASB_ALYb  },
#endif
};

// opcode AF
static const BxOpcodeInfo_t BxOpcodeTableAF[] = {
  /* AF /w */ { 0, BX_IA_REP_SCASW_AXYw },
  /* AF /d */ { 0, BX_IA_REP_SCASD_EAXYd },
#if BX_SUPPORT_X86_64
  /* AF /q */ { 0, BX_IA_REP_SCASQ_RAXYq },
#endif
};

// opcode B0 - B7
static const BxOpcodeInfo_t BxOpcodeTableB0xB7[] = {
  /* B0 /w */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* B0 /d */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
#if BX_SUPPORT_X86_64
  /* B0 /q */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
#endif
};

// opcode B8 - BF
static const BxOpcodeInfo_t BxOpcodeTableB8xBF[] = {
  /* B8 /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* B8 /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
#if BX_SUPPORT_X86_64
  /* B8 /q */ { BxImmediate_Iq, BX_IA_MOV_RRXIq },
#endif
};

// opcode C0
static const BxOpcodeInfo_t BxOpcodeTableC0_G2EbIb[8] = {
  /* /0 */ { BxImmediate_Ib, BX_IA_ROL_EbIb },
  /* /1 */ { BxImmediate_Ib, BX_IA_ROR_EbIb },
  /* /2 */ { BxImmediate_Ib, BX_IA_RCL_EbIb },
  /* /3 */ { BxImmediate_Ib, BX_IA_RCR_EbIb },
  /* /4 */ { BxImmediate_Ib, BX_IA_SHL_EbIb },
  /* /5 */ { BxImmediate_Ib, BX_IA_SHR_EbIb },
  /* /6 */ { BxImmediate_Ib, BX_IA_SHL_EbIb },
  /* /7 */ { BxImmediate_Ib, BX_IA_SAR_EbIb }
};

// opcode C1
static const BxOpcodeInfo_t BxOpcodeTableC1_G2EvIb[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_Ib, BX_IA_ROL_EwIb },
  /* /1 /w */ { BxImmediate_Ib, BX_IA_ROR_EwIb },
  /* /2 /w */ { BxImmediate_Ib, BX_IA_RCL_EwIb },
  /* /3 /w */ { BxImmediate_Ib, BX_IA_RCR_EwIb },
  /* /4 /w */ { BxImmediate_Ib, BX_IA_SHL_EwIb },
  /* /5 /w */ { BxImmediate_Ib, BX_IA_SHR_EwIb },
  /* /6 /w */ { BxImmediate_Ib, BX_IA_SHL_EwIb },
  /* /7 /w */ { BxImmediate_Ib, BX_IA_SAR_EwIb },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_Ib, BX_IA_ROL_EdIb },
  /* /1 /d */ { BxImmediate_Ib, BX_IA_ROR_EdIb },
  /* /2 /d */ { BxImmediate_Ib, BX_IA_RCL_EdIb },
  /* /3 /d */ { BxImmediate_Ib, BX_IA_RCR_EdIb },
  /* /4 /d */ { BxImmediate_Ib, BX_IA_SHL_EdIb },
  /* /5 /d */ { BxImmediate_Ib, BX_IA_SHR_EdIb },
  /* /6 /d */ { BxImmediate_Ib, BX_IA_SHL_EdIb },
  /* /7 /d */ { BxImmediate_Ib, BX_IA_SAR_EdIb },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { BxImmediate_Ib, BX_IA_ROL_EqIb },
  /* /1 /q */ { BxImmediate_Ib, BX_IA_ROR_EqIb },
  /* /2 /q */ { BxImmediate_Ib, BX_IA_RCL_EqIb },
  /* /3 /q */ { BxImmediate_Ib, BX_IA_RCR_EqIb },
  /* /4 /q */ { BxImmediate_Ib, BX_IA_SHL_EqIb },
  /* /5 /q */ { BxImmediate_Ib, BX_IA_SHR_EqIb },
  /* /6 /q */ { BxImmediate_Ib, BX_IA_SHL_EqIb },
  /* /7 /q */ { BxImmediate_Ib, BX_IA_SAR_EqIb },
#endif
};

// opcode C2
static const BxOpcodeInfo_t BxOpcodeTableC2_32[] = {
  /* C2 /w */ { BxImmediate_Iw, BX_IA_RET_Op16_Iw },
  /* C2 /d */ { BxImmediate_Iw, BX_IA_RET_Op32_Iw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableC2_64[] = {
  /* C2 /w */ { BxImmediate_Iw, BX_IA_RET_Op64_Iw },
  /* C2 /d */ { BxImmediate_Iw, BX_IA_RET_Op64_Iw },
  /* C2 /q */ { BxImmediate_Iw, BX_IA_RET_Op64_Iw },
};
#endif

// opcode C3
static const BxOpcodeInfo_t BxOpcodeTableC3_32[] = {
  /* C3 /w */ { 0, BX_IA_RET_Op16 },
  /* C3 /d */ { 0, BX_IA_RET_Op32 },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableC3_64[] = {
  /* C3 /w */ { 0, BX_IA_RET_Op64 },
  /* C3 /d */ { 0, BX_IA_RET_Op64 },
  /* C3 /q */ { 0, BX_IA_RET_Op64 },
};
#endif

// opcode C4
static const BxOpcodeInfo_t BxOpcodeTableC4_32[] = {
  /* C4 /w */ { 0, BX_IA_LES_GwMp },
  /* C4 /d */ { 0, BX_IA_LES_GdMp },
};

// opcode C5
static const BxOpcodeInfo_t BxOpcodeTableC5_32[] = {
  /* C5 /w */ { 0, BX_IA_LDS_GwMp },
  /* C5 /d */ { 0, BX_IA_LDS_GdMp },
};

// opcode C6
static const BxOpcodeInfo_t BxOpcodeTableC6_G11EbIb[8] = {
  /* /0 */ { BxImmediate_Ib, BX_IA_MOV_EbIb },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_ERROR },
  /* /3 */ { 0, BX_IA_ERROR },
  /* /4 */ { 0, BX_IA_ERROR },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR }
};

// opcode C7
static const BxOpcodeInfo_t BxOpcodeTableC7_G11EvIv[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_Iw, BX_IA_MOV_EwIw },
  /* /1 /w */ { 0, BX_IA_ERROR },
  /* /2 /w */ { 0, BX_IA_ERROR },
  /* /3 /w */ { 0, BX_IA_ERROR },
  /* /4 /w */ { 0, BX_IA_ERROR },
  /* /5 /w */ { 0, BX_IA_ERROR },
  /* /6 /w */ { 0, BX_IA_ERROR },
  /* /7 /w */ { 0, BX_IA_ERROR },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_Id, BX_IA_MOV_EdId },
  /* /1 /d */ { 0, BX_IA_ERROR },
  /* /2 /d */ { 0, BX_IA_ERROR },
  /* /3 /d */ { 0, BX_IA_ERROR },
  /* /4 /d */ { 0, BX_IA_ERROR },
  /* /5 /d */ { 0, BX_IA_ERROR },
  /* /6 /d */ { 0, BX_IA_ERROR },
  /* /7 /d */ { 0, BX_IA_ERROR },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { BxImmediate_Id, BX_IA_MOV_EqId },
  /* /1 /q */ { 0, BX_IA_ERROR },
  /* /2 /q */ { 0, BX_IA_ERROR },
  /* /3 /q */ { 0, BX_IA_ERROR },
  /* /4 /q */ { 0, BX_IA_ERROR },
  /* /5 /q */ { 0, BX_IA_ERROR },
  /* /6 /q */ { 0, BX_IA_ERROR },
  /* /7 /q */ { 0, BX_IA_ERROR },
#endif
};

// opcode C8
static const BxOpcodeInfo_t BxOpcodeTableC8_32[] = {
  /* C8 /w */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op16_IwIb },
  /* C8 /d */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op32_IwIb },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableC8_64[] = {
  /* C8 /w */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op64_IwIb },
  /* C8 /d */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op64_IwIb },
  /* C8 /q */ { BxImmediate_Iw | BxImmediate_Ib2, BX_IA_ENTER_Op64_IwIb },
};
#endif

// opcode C9
static const BxOpcodeInfo_t BxOpcodeTableC9_32[] = {
  /* C9 /w */ { 0, BX_IA_LEAVE_Op16 },
  /* C9 /d */ { 0, BX_IA_LEAVE_Op32 },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableC9_64[] = {
  /* C9 /w */ { 0, BX_IA_LEAVE_Op64 },
  /* C9 /d */ { 0, BX_IA_LEAVE_Op64 },
  /* C9 /q */ { 0, BX_IA_LEAVE_Op64 },
};
#endif

// opcode CA
static const BxOpcodeInfo_t BxOpcodeTableCA[] = {
  /* CA /w */ { BxImmediate_Iw, BX_IA_RETF_Op16_Iw },
  /* CA /d */ { BxImmediate_Iw, BX_IA_RETF_Op32_Iw },
#if BX_SUPPORT_X86_64
  /* CA /q */ { BxImmediate_Iw, BX_IA_RETF_Op64_Iw },
#endif
};

// opcode CB
static const BxOpcodeInfo_t BxOpcodeTableCB[] = {
  /* CB /w */ { 0, BX_IA_RETF_Op16 },
  /* CB /d */ { 0, BX_IA_RETF_Op32 },
#if BX_SUPPORT_X86_64
  /* CB /q */ { 0, BX_IA_RETF_Op64 },
#endif
};

// opcode CC
static const BxOpcodeInfo_t BxOpcodeTableCC[] = {
  /* CC */ { 0, BX_IA_INT3 },
};

// opcode CD
static const BxOpcodeInfo_t BxOpcodeTableCD[] = {
  /* CD /w */ { BxImmediate_Ib, BX_IA_INT_Ib },
  /* CD /d */ { BxImmediate_Ib, BX_IA_INT_Ib },
#if BX_SUPPORT_X86_64
  /* CD /q */ { BxImmediate_Ib, BX_IA_INT_Ib },
#endif
};

// opcode CE
static const BxOpcodeInfo_t BxOpcodeTableCE_32[] = {
  /* CE */ { 0, BX_IA_INTO },
};

// opcode CF
static const BxOpcodeInfo_t BxOpcodeTableCF_32[] = {
  /* CF /w */ { 0, BX_IA_IRET_Op16 },
  /* CF /d */ { 0, BX_IA_IRET_Op32 },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableCF_64[] = {
  /* CF /w */ { 0, BX_IA_IRET_Op64 },
  /* CF /d */ { 0, BX_IA_IRET_Op64 },
  /* CF /q */ { 0, BX_IA_IRET_Op64 },
};
#endif

// opcode D0
static const BxOpcodeInfo_t BxOpcodeTableD0_G2EbI1[8] = {
  /* /0 */ { BxImmediate_I1, BX_IA_ROL_EbIb },
  /* /1 */ { BxImmediate_I1, BX_IA_ROR_EbIb },
  /* /2 */ { BxImmediate_I1, BX_IA_RCL_EbIb },
  /* /3 */ { BxImmediate_I1, BX_IA_RCR_EbIb },
  /* /4 */ { BxImmediate_I1, BX_IA_SHL_EbIb },
  /* /5 */ { BxImmediate_I1, BX_IA_SHR_EbIb },
  /* /6 */ { BxImmediate_I1, BX_IA_SHL_EbIb },
  /* /7 */ { BxImmediate_I1, BX_IA_SAR_EbIb }
};

// opcode D1
static const BxOpcodeInfo_t BxOpcodeTableD1_G2EvI1[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_I1, BX_IA_ROL_EwIb },
  /* /1 /w */ { BxImmediate_I1, BX_IA_ROR_EwIb },
  /* /2 /w */ { BxImmediate_I1, BX_IA_RCL_EwIb },
  /* /3 /w */ { BxImmediate_I1, BX_IA_RCR_EwIb },
  /* /4 /w */ { BxImmediate_I1, BX_IA_SHL_EwIb },
  /* /5 /w */ { BxImmediate_I1, BX_IA_SHR_EwIb },
  /* /6 /w */ { BxImmediate_I1, BX_IA_SHL_EwIb },
  /* /7 /w */ { BxImmediate_I1, BX_IA_SAR_EwIb },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_I1, BX_IA_ROL_EdIb },
  /* /1 /d */ { BxImmediate_I1, BX_IA_ROR_EdIb },
  /* /2 /d */ { BxImmediate_I1, BX_IA_RCL_EdIb },
  /* /3 /d */ { BxImmediate_I1, BX_IA_RCR_EdIb },
  /* /4 /d */ { BxImmediate_I1, BX_IA_SHL_EdIb },
  /* /5 /d */ { BxImmediate_I1, BX_IA_SHR_EdIb },
  /* /6 /d */ { BxImmediate_I1, BX_IA_SHL_EdIb },
  /* /7 /d */ { BxImmediate_I1, BX_IA_SAR_EdIb },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { BxImmediate_I1, BX_IA_ROL_EqIb },
  /* /1 /q */ { BxImmediate_I1, BX_IA_ROR_EqIb },
  /* /2 /q */ { BxImmediate_I1, BX_IA_RCL_EqIb },
  /* /3 /q */ { BxImmediate_I1, BX_IA_RCR_EqIb },
  /* /4 /q */ { BxImmediate_I1, BX_IA_SHL_EqIb },
  /* /5 /q */ { BxImmediate_I1, BX_IA_SHR_EqIb },
  /* /6 /q */ { BxImmediate_I1, BX_IA_SHL_EqIb },
  /* /7 /q */ { BxImmediate_I1, BX_IA_SAR_EqIb },
#endif
};

// opcode D2
static const BxOpcodeInfo_t BxOpcodeTableD2_G2Eb[8] = {
  /* /0 */ { 0, BX_IA_ROL_Eb },
  /* /1 */ { 0, BX_IA_ROR_Eb },
  /* /2 */ { 0, BX_IA_RCL_Eb },
  /* /3 */ { 0, BX_IA_RCR_Eb },
  /* /4 */ { 0, BX_IA_SHL_Eb },
  /* /5 */ { 0, BX_IA_SHR_Eb },
  /* /6 */ { 0, BX_IA_SHL_Eb },
  /* /7 */ { 0, BX_IA_SAR_Eb }
};

// opcode D3
static const BxOpcodeInfo_t BxOpcodeTableD3_G2Ev[] = {
  // osize = 16 bit
  /* /0 /w */ { 0, BX_IA_ROL_Ew },
  /* /1 /w */ { 0, BX_IA_ROR_Ew },
  /* /2 /w */ { 0, BX_IA_RCL_Ew },
  /* /3 /w */ { 0, BX_IA_RCR_Ew },
  /* /4 /w */ { 0, BX_IA_SHL_Ew },
  /* /5 /w */ { 0, BX_IA_SHR_Ew },
  /* /6 /w */ { 0, BX_IA_SHL_Ew },
  /* /7 /w */ { 0, BX_IA_SAR_Ew },

  // osize = 32 bit
  /* /0 /d */ { 0, BX_IA_ROL_Ed },
  /* /1 /d */ { 0, BX_IA_ROR_Ed },
  /* /2 /d */ { 0, BX_IA_RCL_Ed },
  /* /3 /d */ { 0, BX_IA_RCR_Ed },
  /* /4 /d */ { 0, BX_IA_SHL_Ed },
  /* /5 /d */ { 0, BX_IA_SHR_Ed },
  /* /6 /d */ { 0, BX_IA_SHL_Ed },
  /* /7 /d */ { 0, BX_IA_SAR_Ed },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { 0, BX_IA_ROL_Eq },
  /* /1 /q */ { 0, BX_IA_ROR_Eq },
  /* /2 /q */ { 0, BX_IA_RCL_Eq },
  /* /3 /q */ { 0, BX_IA_RCR_Eq },
  /* /4 /q */ { 0, BX_IA_SHL_Eq },
  /* /5 /q */ { 0, BX_IA_SHR_Eq },
  /* /6 /q */ { 0, BX_IA_SHL_Eq },
  /* /7 /q */ { 0, BX_IA_SAR_Eq },
#endif
};

// opcode D4
static const BxOpcodeInfo_t BxOpcodeTableD4_32[] = {
  /* D4 /w */ { BxImmediate_Ib, BX_IA_AAM },
  /* D4 /d */ { BxImmediate_Ib, BX_IA_AAM },
};

// opcode D5
static const BxOpcodeInfo_t BxOpcodeTableD5_32[] = {
  /* D5 /w */ { BxImmediate_Ib, BX_IA_AAD },
  /* D5 /d */ { BxImmediate_Ib, BX_IA_AAD },
};

// opcode D6
static const BxOpcodeInfo_t BxOpcodeTableD6_32[] = {
  /* D6 */ { 0, BX_IA_SALC },
};

// opcode D7
static const BxOpcodeInfo_t BxOpcodeTableD7[] = {
  /* D7 */ { 0, BX_IA_XLAT },
};

// opcode E0
static const BxOpcodeInfo_t BxOpcodeTableE0_32[] = {
  /* E0 /w */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op16_Jb },
  /* E0 /d */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op32_Jb },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE0_64[] = {
  /* E0 /w */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op64_Jb },
  /* E0 /d */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op64_Jb },
  /* E0 /q */ { BxImmediate_BrOff8, BX_IA_LOOPNE_Op64_Jb },
};
#endif

// opcode E1
static const BxOpcodeInfo_t BxOpcodeTableE1_32[] = {
  /* E1 /w */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op16_Jb },
  /* E1 /d */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op32_Jb },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE1_64[] = {
  /* E1 /w */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op64_Jb },
  /* E1 /d */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op64_Jb },
  /* E1 /q */ { BxImmediate_BrOff8, BX_IA_LOOPE_Op64_Jb },
};
#endif

// opcode E2
static const BxOpcodeInfo_t BxOpcodeTableE2_32[] = {
  /* E2 /w */ { BxImmediate_BrOff8, BX_IA_LOOP_Op16_Jb },
  /* E2 /d */ { BxImmediate_BrOff8, BX_IA_LOOP_Op32_Jb },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE2_64[] = {
  /* E2 /w */ { BxImmediate_BrOff8, BX_IA_LOOP_Op64_Jb },
  /* E2 /d */ { BxImmediate_BrOff8, BX_IA_LOOP_Op64_Jb },
  /* E2 /q */ { BxImmediate_BrOff8, BX_IA_LOOP_Op64_Jb },
};
#endif

// opcode E3
static const BxOpcodeInfo_t BxOpcodeTableE3_32[] = {
  /* E3 /w */ { BxImmediate_BrOff8, BX_IA_JCXZ_Jb },
  /* E3 /d */ { BxImmediate_BrOff8, BX_IA_JECXZ_Jb },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE3_64[] = {
  /* E3 /w */ { BxImmediate_BrOff8, BX_IA_JRCXZ_Jb },
  /* E3 /d */ { BxImmediate_BrOff8, BX_IA_JRCXZ_Jb },
  /* E3 /q */ { BxImmediate_BrOff8, BX_IA_JRCXZ_Jb },
};
#endif

// opcode E4
static const BxOpcodeInfo_t BxOpcodeTableE4[] = {
  /* E4 /w */ { BxImmediate_Ib, BX_IA_IN_ALIb },
  /* E4 /d */ { BxImmediate_Ib, BX_IA_IN_ALIb },
#if BX_SUPPORT_X86_64
  /* E4 /q */ { BxImmediate_Ib, BX_IA_IN_ALIb },
#endif
};

// opcode E5
static const BxOpcodeInfo_t BxOpcodeTableE5[] = {
  /* E5 /w */ { BxImmediate_Ib, BX_IA_IN_AXIb },
  /* E5 /d */ { BxImmediate_Ib, BX_IA_IN_EAXIb },
#if BX_SUPPORT_X86_64
  /* E5 /q */ { BxImmediate_Ib, BX_IA_IN_EAXIb },
#endif
};

// opcode E6
static const BxOpcodeInfo_t BxOpcodeTableE6[] = {
  /* E6 /w */ { BxImmediate_Ib, BX_IA_OUT_IbAL },
  /* E6 /d */ { BxImmediate_Ib, BX_IA_OUT_IbAL },
#if BX_SUPPORT_X86_64
  /* E6 /q */ { BxImmediate_Ib, BX_IA_OUT_IbAL },
#endif
};

// opcode E7
static const BxOpcodeInfo_t BxOpcodeTableE7[] = {
  /* E7 /w */ { BxImmediate_Ib, BX_IA_OUT_IbAX },
  /* E7 /d */ { BxImmediate_Ib, BX_IA_OUT_IbEAX },
#if BX_SUPPORT_X86_64
  /* E7 /q */ { BxImmediate_Ib, BX_IA_OUT_IbEAX },
#endif
};

// opcode E8
static const BxOpcodeInfo_t BxOpcodeTableE8_32[] = {
  /* E8 /w */ { BxImmediate_BrOff16, BX_IA_CALL_Jw },
  /* E8 /d */ { BxImmediate_BrOff32, BX_IA_CALL_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE8_64[] = {
  /* E8 /w */ { BxImmediate_BrOff32, BX_IA_CALL_Jq },
  /* E8 /d */ { BxImmediate_BrOff32, BX_IA_CALL_Jq },
  /* E8 /q */ { BxImmediate_BrOff32, BX_IA_CALL_Jq },
};
#endif

// opcode E9
static const BxOpcodeInfo_t BxOpcodeTableE9_32[] = {
  /* E9 /w */ { BxImmediate_BrOff16, BX_IA_JMP_Jw },
  /* E9 /d */ { BxImmediate_BrOff32, BX_IA_JMP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableE9_64[] = {
  /* E9 /w */ { BxImmediate_BrOff32, BX_IA_JMP_Jq },
  /* E9 /d */ { BxImmediate_BrOff32, BX_IA_JMP_Jq },
  /* E9 /q */ { BxImmediate_BrOff32, BX_IA_JMP_Jq },
};
#endif

// opcode EA
static const BxOpcodeInfo_t BxOpcodeTableEA_32[] = {
  /* EA /w */ { BxImmediate_Iw | BxImmediate_Iw2, BX_IA_JMPF_Ap },
  /* EA /d */ { BxImmediate_Id | BxImmediate_Iw2, BX_IA_JMPF_Ap },
};

// opcode EB
static const BxOpcodeInfo_t BxOpcodeTableEB_32[] = {
  /* EB /w */ { BxImmediate_BrOff8, BX_IA_JMP_Jw },
  /* EB /d */ { BxImmediate_BrOff8, BX_IA_JMP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableEB_64[] = {
  /* EB /w */ { BxImmediate_BrOff8, BX_IA_JMP_Jq },
  /* EB /d */ { BxImmediate_BrOff8, BX_IA_JMP_Jq },
  /* EB /q */ { BxImmediate_BrOff8, BX_IA_JMP_Jq },
};
#endif

// opcode EC
static const BxOpcodeInfo_t BxOpcodeTableEC[] = {
  /* EC /w */ { 0, BX_IA_IN_ALDX },
  /* EC /d */ { 0, BX_IA_IN_ALDX },
#if BX_SUPPORT_X86_64
  /* EC /q */ { 0, BX_IA_IN_ALDX },
#endif
};

// opcode ED
static const BxOpcodeInfo_t BxOpcodeTableED[] = {
  /* ED /w */ { 0, BX_IA_IN_AXDX },
  /* ED /d */ { 0, BX_IA_IN_EAXDX },
#if BX_SUPPORT_X86_64
  /* ED /q */ { 0, BX_IA_IN_EAXDX },
#endif
};

// opcode EE
static const BxOpcodeInfo_t BxOpcodeTableEE[] = {
  /* EE /w */ { 0, BX_IA_OUT_DXAL },
  /* EE /d */ { 0, BX_IA_OUT_DXAL },
#if BX_SUPPORT_X86_64
  /* EE /q */ { 0, BX_IA_OUT_DXAL },
#endif
};

// opcode EF
static const BxOpcodeInfo_t BxOpcodeTableEF[] = {
  /* EF /w */ { 0, BX_IA_OUT_DXAX },
  /* EF /d */ { 0, BX_IA_OUT_DXEAX },
#if BX_SUPPORT_X86_64
  /* EF /q */ { 0, BX_IA_OUT_DXEAX },
#endif
};

// opcode F1
static const BxOpcodeInfo_t BxOpcodeTableF1[] = {
  /* F1 */ { 0, BX_IA_INT1 },
};

// opcode F4
static const BxOpcodeInfo_t BxOpcodeTableF4[] = {
  /* F4 */ { 0, BX_IA_HLT },
};

// opcode F5
static const BxOpcodeInfo_t BxOpcodeTableF5[] = {
  /* F5 */ { 0, BX_IA_CMC },
};

// opcode F6
static const BxOpcodeInfo_t BxOpcodeTableF6_G3Eb[8] = {
  /* /0 */ { BxImmediate_Ib, BX_IA_TEST_EbIb },
  /* /1 */ { BxImmediate_Ib, BX_IA_TEST_EbIb },
  /* /2 */ { 0,              BX_IA_NOT_Eb },
  /* /3 */ { 0,              BX_IA_NEG_Eb },
  /* /4 */ { 0,              BX_IA_MUL_ALEb },
  /* /5 */ { 0,              BX_IA_IMUL_ALEb },
  /* /6 */ { 0,              BX_IA_DIV_ALEb },
  /* /7 */ { 0,              BX_IA_IDIV_ALEb }
};

// opcode F7
static const BxOpcodeInfo_t BxOpcodeTableF7_G3Ev[] = {
  // osize = 16 bit
  /* /0 /w */ { BxImmediate_Iw, BX_IA_TEST_EwIw },
  /* /1 /w */ { BxImmediate_Iw, BX_IA_TEST_EwIw },
  /* /2 /w */ { 0,              BX_IA_NOT_Ew },
  /* /3 /w */ { 0,              BX_IA_NEG_Ew },
  /* /4 /w */ { 0,              BX_IA_MUL_AXEw },
  /* /5 /w */ { 0,              BX_IA_IMUL_AXEw },
  /* /6 /w */ { 0,              BX_IA_DIV_AXEw },
  /* /7 /w */ { 0,              BX_IA_IDIV_AXEw },

  // osize = 32 bit
  /* /0 /d */ { BxImmediate_Id, BX_IA_TEST_EdId },
  /* /1 /d */ { BxImmediate_Id, BX_IA_TEST_EdId },
  /* /2 /d */ { 0,              BX_IA_NOT_Ed },
  /* /3 /d */ { 0,              BX_IA_NEG_Ed },
  /* /4 /d */ { 0,              BX_IA_MUL_EAXEd },
  /* /5 /d */ { 0,              BX_IA_IMUL_EAXEd },
  /* /6 /d */ { 0,              BX_IA_DIV_EAXEd },
  /* /7 /d */ { 0,              BX_IA_IDIV_EAXEd },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { BxImmediate_Id, BX_IA_TEST_EqId },
  /* /1 /q */ { BxImmediate_Id, BX_IA_TEST_EqId },
  /* /2 /q */ { 0,              BX_IA_NOT_Eq },
  /* /3 /q */ { 0,              BX_IA_NEG_Eq },
  /* /4 /q */ { 0,              BX_IA_MUL_RAXEq },
  /* /5 /q */ { 0,              BX_IA_IMUL_RAXEq },
  /* /6 /q */ { 0,              BX_IA_DIV_RAXEq },
  /* /7 /q */ { 0,              BX_IA_IDIV_RAXEq },
#endif
};

// opcode F8
static const BxOpcodeInfo_t BxOpcodeTableF8[] = {
  /* F8 */ { 0, BX_IA_CLC },
};

// opcode F9
static const BxOpcodeInfo_t BxOpcodeTableF9[] = {
  /* F9 */ { 0, BX_IA_STC },
};

// opcode FA
static const BxOpcodeInfo_t BxOpcodeTableFA[] = {
  /* FA */ { 0, BX_IA_CLI },
};

// opcode FB
static const BxOpcodeInfo_t BxOpcodeTableFB[] = {
  /* FB */ { 0, BX_IA_STI },
};

// opcode FC
static const BxOpcodeInfo_t BxOpcodeTableFC[] = {
  /* FC */ { 0, BX_IA_CLD },
};

// opcode FD
static const BxOpcodeInfo_t BxOpcodeTableFD[] = {
  /* FD */ { 0, BX_IA_STD },
};

// opcode FE
static const BxOpcodeInfo_t BxOpcodeTableG4[8] = {
  /* /0 */ { 0, BX_IA_INC_Eb },
  /* /1 */ { 0, BX_IA_DEC_Eb },
  /* /2 */ { 0, BX_IA_ERROR },
  /* /3 */ { 0, BX_IA_ERROR },
  /* /4 */ { 0, BX_IA_ERROR },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR }
};

// opcode FF
static const BxOpcodeInfo_t BxOpcodeTableFF_32G5v[8*2] = {
  // osize = 16 bit
  /* /0 /w */ { 0, BX_IA_INC_Ew },
  /* /1 /w */ { 0, BX_IA_DEC_Ew },
  /* /2 /w */ { 0, BX_IA_CALL_Ew },
  /* /3 /w */ { 0, BX_IA_CALLF_Op16_Ep },
  /* /4 /w */ { 0, BX_IA_JMP_Ew },
  /* /5 /w */ { 0, BX_IA_JMPF_Op16_Ep },
  /* /6 /w */ { 0, BX_IA_PUSH_Ew },
  /* /7 /w */ { 0, BX_IA_ERROR },

  // osize = 32 bit
  /* /0 /d */ { 0, BX_IA_INC_Ed },
  /* /1 /d */ { 0, BX_IA_DEC_Ed },
  /* /2 /d */ { 0, BX_IA_CALL_Ed },
  /* /3 /d */ { 0, BX_IA_CALLF_Op32_Ep },
  /* /4 /d */ { 0, BX_IA_JMP_Ed },
  /* /5 /d */ { 0, BX_IA_JMPF_Op32_Ep },
  /* /6 /d */ { 0, BX_IA_PUSH_Ed },
  /* /7 /d */ { 0, BX_IA_ERROR },
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableFF_64G5v[8*3] = {
  // osize = 16 bit
  /* /0 */ { 0, BX_IA_INC_Ew },
  /* /1 */ { 0, BX_IA_DEC_Ew },
  /* /2 */ { 0, BX_IA_CALL_Eq },
  /* /3 */ { 0, BX_IA_CALLF_Op16_Ep },
  /* /4 */ { 0, BX_IA_JMP_Eq },
  /* /5 */ { 0, BX_IA_JMPF_Op16_Ep },
  /* /6 */ { 0, BX_IA_PUSH_Ew },
  /* /7 */ { 0, BX_IA_ERROR },

  // osize = 32 bit
  /* /0 */ { 0, BX_IA_INC_Ed },
  /* /1 */ { 0, BX_IA_DEC_Ed },
  /* /2 */ { 0, BX_IA_CALL_Eq },
  /* /3 */ { 0, BX_IA_CALLF_Op32_Ep },
  /* /4 */ { 0, BX_IA_JMP_Eq },
  /* /5 */ { 0, BX_IA_JMPF_Op32_Ep },
  /* /6 */ { 0, BX_IA_PUSH_Eq },
  /* /7 */ { 0, BX_IA_ERROR },

  // osize = 64 bit
  /* /0 */ { 0, BX_IA_INC_Eq },
  /* /1 */ { 0, BX_IA_DEC_Eq },
  /* /2 */ { 0, BX_IA_CALL_Eq },
  /* /3 */ { 0, BX_IA_CALLF_Op64_Ep }, // TODO: 64-bit offset for Intel
  /* /4 */ { 0, BX_IA_JMP_Eq },
  /* /5 */ { 0, BX_IA_JMPF_Op64_Ep },  // TODO: 64-bit offset for Intel
  /* /6 */ { 0, BX_IA_PUSH_Eq },
  /* /7 */ { 0, BX_IA_ERROR },
};
#endif

// opcode 0F 00
static const BxOpcodeInfo_t BxOpcodeTableG6[8] = {
  /* /0 */ { 0, BX_IA_SLDT_Ew },
  /* /1 */ { 0, BX_IA_STR_Ew },
  /* /2 */ { 0, BX_IA_LLDT_Ew },
  /* /3 */ { 0, BX_IA_LTR_Ew },
  /* /4 */ { 0, BX_IA_VERR_Ew },
  /* /5 */ { 0, BX_IA_VERW_Ew },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR }
};

// opcode 0F 01
static const BxOpcodeInfo_t BxOpcodeTable0F01_32[] = {
  /* /0 */ { 0, BX_IA_SGDT_Ms },
  /* /1 */ { 0, BX_IA_SIDT_Ms },
  /* /2 */ { 0, BX_IA_LGDT_Ms },
  /* /3 */ { 0, BX_IA_LIDT_Ms },
  /* /4 */ { 0, BX_IA_SMSW_Ew },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_LMSW_Ew },
  /* /7 */ { 0, BX_IA_INVLPG },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F01_64[] = {
  /* /0 */ { 0, BX_IA_SGDT_Op64_Ms },
  /* /1 */ { 0, BX_IA_SIDT_Op64_Ms },
  /* /2 */ { 0, BX_IA_LGDT_Op64_Ms },
  /* /3 */ { 0, BX_IA_LIDT_Op64_Ms },
  /* /4 */ { 0, BX_IA_SMSW_Ew },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_LMSW_Ew },
  /* /7 */ { 0, BX_IA_INVLPG },
};
#endif

static const BxOpcodeInfo_t BxOpcodeTable0F01[64] = {
  /* 0F 01 C0 */ { 0, BX_IA_ERROR },
  /* 0F 01 C1 */ { BxNoPrefixSSE, BX_IA_VMCALL },
  /* 0F 01 C2 */ { BxNoPrefixSSE, BX_IA_VMLAUNCH },
  /* 0F 01 C3 */ { BxNoPrefixSSE, BX_IA_VMRESUME },
  /* 0F 01 C4 */ { BxNoPrefixSSE, BX_IA_VMXOFF },
  /* 0F 01 C5 */ { 0, BX_IA_ERROR },
  /* 0F 01 C6 */ { 0, BX_IA_ERROR },
  /* 0F 01 C7 */ { 0, BX_IA_ERROR },
  /* 0F 01 C8 */ { BxNoPrefixSSE, BX_IA_MONITOR },
  /* 0F 01 C9 */ { BxNoPrefixSSE, BX_IA_MWAIT },
  /* 0F 01 CA */ { BxNoPrefixSSE, BX_IA_CLAC },
  /* 0F 01 CB */ { BxNoPrefixSSE, BX_IA_STAC },
  /* 0F 01 CC */ { 0, BX_IA_ERROR },
  /* 0F 01 CD */ { 0, BX_IA_ERROR },
  /* 0F 01 CE */ { 0, BX_IA_ERROR },
  /* 0F 01 CF */ { 0, BX_IA_ERROR },
  /* 0F 01 D0 */ { BxNoPrefixSSE, BX_IA_XGETBV },
  /* 0F 01 D1 */ { BxNoPrefixSSE, BX_IA_XSETBV },
  /* 0F 01 D2 */ { 0, BX_IA_ERROR },
  /* 0F 01 D3 */ { 0, BX_IA_ERROR },
  /* 0F 01 D4 */ { BxNoPrefixSSE, BX_IA_VMFUNC },
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
#if BX_SUPPORT_PKEYS
  /* 0F 01 EE */ { BxNoPrefixSSE, BX_IA_RDPKRU },
  /* 0F 01 EF */ { BxNoPrefixSSE, BX_IA_WRPKRU },
#else
  /* 0F 01 EE */ { 0, BX_IA_ERROR },
  /* 0F 01 EF */ { 0, BX_IA_ERROR },
#endif
  /* 0F 01 F0 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F1 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F2 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F3 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F4 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F5 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F6 */ { 0, BX_IA_LMSW_Ew },
  /* 0F 01 F7 */ { 0, BX_IA_LMSW_Ew },
#if BX_SUPPORT_X86_64
  /* 0F 01 F8 */ { 0, BX_IA_SWAPGS },
#else
  /* 0F 01 F8 */ { 0, BX_IA_ERROR },
#endif
  /* 0F 01 F9 */ { 0, BX_IA_RDTSCP }, // end trace to avoid multiple TSC samples in one cycle
  /* 0F 01 FA */ { 0, BX_IA_MONITORX },
  /* 0F 01 FB */ { 0, BX_IA_MWAITX },
  /* 0F 01 FC */ { 0, BX_IA_CLZERO },
  /* 0F 01 FD */ { 0, BX_IA_ERROR },
  /* 0F 01 FE */ { 0, BX_IA_ERROR },
  /* 0F 01 FF */ { 0, BX_IA_ERROR },
};

// opcode 0F 02
static const BxOpcodeInfo_t BxOpcodeTable0F02[] = {
  /* 0F 02 /w */ { 0, BX_IA_LAR_GwEw },
  /* 0F 02 /d */ { 0, BX_IA_LAR_GdEw },
#if BX_SUPPORT_X86_64
  /* 0F 02 /q */ { 0, BX_IA_LAR_GdEw },
#endif
};

// opcode 0F 03
static const BxOpcodeInfo_t BxOpcodeTable0F03[] = {
  /* 0F 03 /w */ { 0, BX_IA_LSL_GwEw },
  /* 0F 03 /d */ { 0, BX_IA_LSL_GdEw },
#if BX_SUPPORT_X86_64
  /* 0F 03 /q */ { 0, BX_IA_LSL_GdEw },
#endif
};

// opcode 0F 05
static const BxOpcodeInfo_t BxOpcodeTable0F05_32[] = {
  /* 0F 05 */ { 0, BX_IA_SYSCALL_LEGACY },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F05_64[] = {
  /* 0F 05 */ { 0, BX_IA_SYSCALL },
};
#endif

// opcode 0F 06
static const BxOpcodeInfo_t BxOpcodeTable0F06[] = {
  /* 0F 06 */ { 0, BX_IA_CLTS },
};

// opcode 0F 07
static const BxOpcodeInfo_t BxOpcodeTable0F07_32[] = {
  /* 0F 07 */ { 0, BX_IA_SYSRET_LEGACY },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F07_64[] = {
  /* 0F 07 */ { 0, BX_IA_SYSRET },
};
#endif

// opcode 0F 08
static const BxOpcodeInfo_t BxOpcodeTable0F08[] = {
  /* 0F 08 */ { 0, BX_IA_INVD },
};

// opcode 0F 09
static const BxOpcodeInfo_t BxOpcodeTable0F09[] = {
  /* 0F 09 */ { 0, BX_IA_WBINVD },
};

// opcode 0F 0B
static const BxOpcodeInfo_t BxOpcodeTable0F0B[] = {
  /* 0F 0B */ { 0, BX_IA_UD2 },
};

// opcode 0F 0D - 3DNow! PREFETCHW on AMD, NOP on older Intel CPUs
static const BxOpcodeInfo_t BxOpcodeTable0F0D[] = {
  /* 0F 0D /w */ { 0, BX_IA_PREFETCHW_Mb },
  /* 0F 0D /d */ { 0, BX_IA_PREFETCHW_Mb },
#if BX_SUPPORT_X86_64
  /* 0F 0D /q */ { 0, BX_IA_PREFETCHW_Mb },
#endif
};

// opcode 0F 0E - 3DNow! FEMMS
static const BxOpcodeInfo_t BxOpcodeTable0F0E[] = {
  /* 0F 0E */ { 0, BX_IA_FEMMS },
};

// opcode 0F 0F - 3DNow! Opcode Table
static const BxOpcodeInfo_t BxOpcodeTable0F0F[] = {
  /* 0F 0F /w */ { BxImmediate_Ib, BX_IA_ERROR },
  /* 0F 0F /d */ { BxImmediate_Ib, BX_IA_ERROR },
#if BX_SUPPORT_X86_64
  /* 0F 0F /q */ { BxImmediate_Ib, BX_IA_ERROR },
#endif
};

// opcode 0F 10
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F10[4] = {
  /* -- */ { 0, BX_IA_MOVUPS_VpsWps },
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MOVSS_VssWss },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsd },
};

// opcode 0F 11
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F11[4] = {
  /* -- */ { 0, BX_IA_MOVUPS_WpsVps },
  /* 66 */ { 0, BX_IA_MOVUPD_WpdVpd },
  /* F3 */ { 0, BX_IA_MOVSS_WssVss },
  /* F2 */ { 0, BX_IA_MOVSD_WsdVsd },
};

// opcode 0F 12
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F12[4] = {
  /* -- */ { 0, BX_IA_MOVLPS_VpsMq },
  /* 66 */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_MOVDDUP_VpdWq },
};

// opcode 0F 13
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F13[4] = {
  /* -- */ { 0, BX_IA_MOVLPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVLPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 14
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F14[4] = {
  /* -- */ { 0, BX_IA_UNPCKLPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKLPD_VpdWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 15
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F15[4] = {
  /* -- */ { 0, BX_IA_UNPCKHPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKHPD_VpdWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 16
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F16[4] = {
  /* -- */ { 0, BX_IA_MOVHPS_VpsMq },
  /* 66 */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 17
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F17[4] = {
  /* -- */ { 0, BX_IA_MOVHPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVHPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 18 - opcode group G16, PREFETCH hints
#if BX_CPU_LEVEL >= 6
static const BxOpcodeInfo_t BxOpcodeTable0F18_G16[8] = {
  /* /0 */ { 0, BX_IA_PREFETCHNTA_Mb },
  /* /1 */ { 0, BX_IA_PREFETCHT0_Mb },
  /* /2 */ { 0, BX_IA_PREFETCHT1_Mb },
  /* /3 */ { 0, BX_IA_PREFETCHT2_Mb },
  /* /4 */ { 0, BX_IA_PREFETCH_Mb },
  /* /5 */ { 0, BX_IA_PREFETCH_Mb },
  /* /6 */ { 0, BX_IA_PREFETCH_Mb },
  /* /7 */ { 0, BX_IA_PREFETCH_Mb}
};
#endif

// opcode 0F 19 - 0F 1F
static const BxOpcodeInfo_t BxOpcodeTableMultiByteNOP[] = {
  /* 0F 19 /w */ { 0, BX_IA_NOP },      // multi-byte NOP
  /* 0F 19 /d */ { 0, BX_IA_NOP },      // multi-byte NOP
#if BX_SUPPORT_X86_64
  /* 0F 19 /q */ { 0, BX_IA_NOP },      // multi-byte NOP
#endif
};

// opcode 0F 20
static const BxOpcodeInfo_t BxOpcodeTableMOV_RdCd[8] = {
  /* /0 */ { 0, BX_IA_MOV_RdCR0 },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_MOV_RdCR2 },
  /* /3 */ { 0, BX_IA_MOV_RdCR3 },
  /* /4 */ { 0, BX_IA_MOV_RdCR4 },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableMOV_RqCq[8] = {
  /* /0 */ { 0, BX_IA_MOV_RqCR0 },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_MOV_RqCR2 },
  /* /3 */ { 0, BX_IA_MOV_RqCR3 },
  /* /4 */ { 0, BX_IA_MOV_RqCR4 },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR },
};
#endif

// opcode 0F 21
static const BxOpcodeInfo_t BxOpcodeTable0F21_32[] = {
  /* 0F 21 */ { 0, BX_IA_MOV_RdDd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F21_64[] = {
  /* 0F 21 */ { 0, BX_IA_MOV_RqDq },
};
#endif

// opcode 0F 22
static const BxOpcodeInfo_t BxOpcodeTableMOV_CdRd[8] = {
  /* /0 */ { 0, BX_IA_MOV_CR0Rd },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_MOV_CR2Rd },
  /* /3 */ { 0, BX_IA_MOV_CR3Rd },
  /* /4 */ { 0, BX_IA_MOV_CR4Rd },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTableMOV_CqRq[8] = {
  /* /0 */ { 0, BX_IA_MOV_CR0Rq },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_MOV_CR2Rq },
  /* /3 */ { 0, BX_IA_MOV_CR3Rq },
  /* /4 */ { 0, BX_IA_MOV_CR4Rq },
  /* /5 */ { 0, BX_IA_ERROR },
  /* /6 */ { 0, BX_IA_ERROR },
  /* /7 */ { 0, BX_IA_ERROR },
};
#endif

// opcode 0F 23
static const BxOpcodeInfo_t BxOpcodeTable0F23_32[] = {
  /* 0F 23 */ { 0, BX_IA_MOV_DdRd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F23_64[] = {
  /* 0F 23 */ { 0, BX_IA_MOV_DqRq },
};
#endif

// opcode 0F 24
static const BxOpcodeInfo_t BxOpcodeTable0F24_32[] = {
  /* 0F 24 */ { 0, BX_IA_ERROR }, // BX_IA_MOV_RdTd not implemented
};

// opcode 0F 26
static const BxOpcodeInfo_t BxOpcodeTable0F26_32[] = {
  /* 0F 26 */ { 0, BX_IA_ERROR }, // BX_IA_MOV_TdRd not implemented
};

// opcode 0F 28
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F28[4] = {
  /* -- */ { 0, BX_IA_MOVAPS_VpsWps },
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 29
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F29[4] = {
  /* -- */ { 0, BX_IA_MOVAPS_WpsVps },
  /* 66 */ { 0, BX_IA_MOVAPD_WpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 2A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2A[] = {
  /* --    */ { 0, BX_IA_CVTPI2PS_VpsQq },
  /* 66    */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F3    */ { 0, BX_IA_CVTSI2SS_VssEd },
  /* F2    */ { 0, BX_IA_CVTSI2SD_VsdEd },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_CVTPI2PS_VpsQq },
  /* 66 /q */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F3 /q */ { 0, BX_IA_CVTSI2SS_VssEq },
  /* F2 /q */ { 0, BX_IA_CVTSI2SD_VsdEq },
#endif
};

// opcode 0F 2B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2B[4] = {
  /* -- */ { 0, BX_IA_MOVNTPS_MpsVps },
  /* 66 */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F3 */ { 0, BX_IA_MOVNTSS_MssVss },
  /* F2 */ { 0, BX_IA_MOVNTSD_MsdVsd },
};

// opcode 0F 2C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2C[] = {
  /* --    */ { 0, BX_IA_CVTTPS2PI_PqWps },
  /* 66    */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3    */ { 0, BX_IA_CVTTSS2SI_GdWss },
  /* F2    */ { 0, BX_IA_CVTTSD2SI_GdWsd },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_CVTTPS2PI_PqWps },
  /* 66 /q */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3 /q */ { 0, BX_IA_CVTTSS2SI_GqWss },
  /* F2 /q */ { 0, BX_IA_CVTTSD2SI_GqWsd },
#endif
};

// opcode 0F 2D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2D[] = {
  /* --    */ { 0, BX_IA_CVTPS2PI_PqWps },
  /* 66    */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3    */ { 0, BX_IA_CVTSS2SI_GdWss },
  /* F2    */ { 0, BX_IA_CVTSD2SI_GdWsd },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_CVTPS2PI_PqWps },
  /* 66 /q */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3 /q */ { 0, BX_IA_CVTSS2SI_GqWss },
  /* F2 /q */ { 0, BX_IA_CVTSD2SI_GqWsd },
#endif
};

// opcode 0F 2E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2E[4] = {
  /* -- */ { 0, BX_IA_UCOMISS_VssWss },
  /* 66 */ { 0, BX_IA_UCOMISD_VsdWsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 2F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F2F[4] = {
  /* -- */ { 0, BX_IA_COMISS_VssWss },
  /* 66 */ { 0, BX_IA_COMISD_VsdWsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 30
static const BxOpcodeInfo_t BxOpcodeTable0F30[] = {
  /* 0F 30 */ { 0, BX_IA_WRMSR },
};

// opcode 0F 31
static const BxOpcodeInfo_t BxOpcodeTable0F31[] = {
  // end trace to avoid multiple TSC samples in one cycle
  /* 0F 31 */ { 0, BX_IA_RDTSC },
};

// opcode 0F 32
static const BxOpcodeInfo_t BxOpcodeTable0F32[] = {
  // end trace to avoid multiple TSC samples in one cycle
  /* 0F 32 */ { 0, BX_IA_RDMSR },
};

// opcode 0F 33
static const BxOpcodeInfo_t BxOpcodeTable0F33[] = {
  /* 0F 33 */ { 0, BX_IA_RDPMC },
};

// opcode 0F 34
static const BxOpcodeInfo_t BxOpcodeTable0F34[] = {
  /* 0F 34 */ { 0, BX_IA_SYSENTER },
};

// opcode 0F 35
static const BxOpcodeInfo_t BxOpcodeTable0F35[] = {
  /* 0F 35 */ { 0, BX_IA_SYSEXIT },
};

// opcode 0F 37
static const BxOpcodeInfo_t BxOpcodeTable0F37[] = {
  /* 0F 37 */ { 0, BX_IA_GETSEC },
};

// opcode 0F 40
static const BxOpcodeInfo_t BxOpcodeTable0F40[] = {
  /* 0F 40 /w */ { 0, BX_IA_CMOVO_GwEw },
  /* 0F 40 /d */ { 0, BX_IA_CMOVO_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 40 /q */ { 0, BX_IA_CMOVO_GqEq },
#endif
};

// opcode 0F 41
static const BxOpcodeInfo_t BxOpcodeTable0F41[] = {
  /* 0F 41 /w */ { 0, BX_IA_CMOVNO_GwEw },
  /* 0F 41 /d */ { 0, BX_IA_CMOVNO_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 41 /q */ { 0, BX_IA_CMOVNO_GqEq },
#endif
};

// opcode 0F 42
static const BxOpcodeInfo_t BxOpcodeTable0F42[] = {
  /* 0F 42 /w */ { 0, BX_IA_CMOVB_GwEw },
  /* 0F 42 /d */ { 0, BX_IA_CMOVB_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 42 /q */ { 0, BX_IA_CMOVB_GqEq },
#endif
};

// opcode 0F 43
static const BxOpcodeInfo_t BxOpcodeTable0F43[] = {
  /* 0F 43 /w */ { 0, BX_IA_CMOVNB_GwEw },
  /* 0F 43 /d */ { 0, BX_IA_CMOVNB_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 43 /q */ { 0, BX_IA_CMOVNB_GqEq },
#endif
};

// opcode 0F 44
static const BxOpcodeInfo_t BxOpcodeTable0F44[] = {
  /* 0F 44 /w */ { 0, BX_IA_CMOVZ_GwEw },
  /* 0F 44 /d */ { 0, BX_IA_CMOVZ_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 44 /q */ { 0, BX_IA_CMOVZ_GqEq },
#endif
};

// opcode 0F 45
static const BxOpcodeInfo_t BxOpcodeTable0F45[] = {
  /* 0F 45 /w */ { 0, BX_IA_CMOVNZ_GwEw },
  /* 0F 45 /d */ { 0, BX_IA_CMOVNZ_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 45 /q */ { 0, BX_IA_CMOVNZ_GqEq },
#endif
};

// opcode 0F 46
static const BxOpcodeInfo_t BxOpcodeTable0F46[] = {
  /* 0F 46 /w */ { 0, BX_IA_CMOVBE_GwEw },
  /* 0F 46 /d */ { 0, BX_IA_CMOVBE_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 46 /q */ { 0, BX_IA_CMOVBE_GqEq },
#endif
};

// opcode 0F 47
static const BxOpcodeInfo_t BxOpcodeTable0F47[] = {
  /* 0F 47 /w */ { 0, BX_IA_CMOVNBE_GwEw },
  /* 0F 47 /d */ { 0, BX_IA_CMOVNBE_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 47 /q */ { 0, BX_IA_CMOVNBE_GqEq },
#endif
};

// opcode 0F 48
static const BxOpcodeInfo_t BxOpcodeTable0F48[] = {
  /* 0F 48 /w */ { 0, BX_IA_CMOVS_GwEw },
  /* 0F 48 /d */ { 0, BX_IA_CMOVS_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 48 /q */ { 0, BX_IA_CMOVS_GqEq },
#endif
};

// opcode 0F 49
static const BxOpcodeInfo_t BxOpcodeTable0F49[] = {
  /* 0F 49 /w */ { 0, BX_IA_CMOVNS_GwEw },
  /* 0F 49 /d */ { 0, BX_IA_CMOVNS_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 49 /q */ { 0, BX_IA_CMOVNS_GqEq },
#endif
};

// opcode 0F 4A
static const BxOpcodeInfo_t BxOpcodeTable0F4A[] = {
  /* 0F 4A /w */ { 0, BX_IA_CMOVP_GwEw },
  /* 0F 4A /d */ { 0, BX_IA_CMOVP_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4A /q */ { 0, BX_IA_CMOVP_GqEq },
#endif
};

// opcode 0F 4B
static const BxOpcodeInfo_t BxOpcodeTable0F4B[] = {
  /* 0F 4B /w */ { 0, BX_IA_CMOVNP_GwEw },
  /* 0F 4B /d */ { 0, BX_IA_CMOVNP_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4B /q */ { 0, BX_IA_CMOVNP_GqEq },
#endif
};

// opcode 0F 4C
static const BxOpcodeInfo_t BxOpcodeTable0F4C[] = {
  /* 0F 4C /w */ { 0, BX_IA_CMOVL_GwEw },
  /* 0F 4C /d */ { 0, BX_IA_CMOVL_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4C /q */ { 0, BX_IA_CMOVL_GqEq },
#endif
};

// opcode 0F 4D
static const BxOpcodeInfo_t BxOpcodeTable0F4D[] = {
  /* 0F 4D /w */ { 0, BX_IA_CMOVNL_GwEw },
  /* 0F 4D /d */ { 0, BX_IA_CMOVNL_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4D /q */ { 0, BX_IA_CMOVNL_GqEq },
#endif
};

// opcode 0F 4E
static const BxOpcodeInfo_t BxOpcodeTable0F4E[] = {
  /* 0F 4E /w */ { 0, BX_IA_CMOVLE_GwEw },
  /* 0F 4E /d */ { 0, BX_IA_CMOVLE_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4E /q */ { 0, BX_IA_CMOVLE_GqEq },
#endif
};

// opcode 0F 4F
static const BxOpcodeInfo_t BxOpcodeTable0F4F[] = {
  /* 0F 4F /w */ { 0, BX_IA_CMOVNLE_GwEw },
  /* 0F 4F /d */ { 0, BX_IA_CMOVNLE_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F 4F /q */ { 0, BX_IA_CMOVNLE_GqEq },
#endif
};

// opcode 0F 50
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F50[4] = {
  /* -- */ { 0, BX_IA_MOVMSKPS_GdUps },
  /* 66 */ { 0, BX_IA_MOVMSKPD_GdUpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 51
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F51[4] = {
  /* -- */ { 0, BX_IA_SQRTPS_VpsWps },
  /* 66 */ { 0, BX_IA_SQRTPD_VpdWpd },
  /* F3 */ { 0, BX_IA_SQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_SQRTSD_VsdWsd },
};

// opcode 0F 52
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F52[4] = {
  /* -- */ { 0, BX_IA_RSQRTPS_VpsWps },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RSQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 53
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F53[4] = {
  /* -- */ { 0, BX_IA_RCPPS_VpsWps },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RCPSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 54
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F54[4] = {
  /* -- */ { 0, BX_IA_ANDPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 55
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F55[4] = {
  /* -- */ { 0, BX_IA_ANDNPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDNPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 56
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F56[4] = {
  /* -- */ { 0, BX_IA_ORPS_VpsWps },
  /* 66 */ { 0, BX_IA_ORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 57
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F57[4] = {
  /* -- */ { 0, BX_IA_XORPS_VpsWps },
  /* 66 */ { 0, BX_IA_XORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 58
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F58[] = {
  /* -- */ { 0, BX_IA_ADDPS_VpsWps },
  /* 66 */ { 0, BX_IA_ADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ADDSS_VssWss },
  /* F2 */ { 0, BX_IA_ADDSD_VsdWsd },
};

// opcode 0F 59
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F59[] = {
  /* -- */ { 0, BX_IA_MULPS_VpsWps },
  /* 66 */ { 0, BX_IA_MULPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MULSS_VssWss },
  /* F2 */ { 0, BX_IA_MULSD_VsdWsd },
};

// opcode 0F 5A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5A[] = {
  /* -- */ { 0, BX_IA_CVTPS2PD_VpdWps },
  /* 66 */ { 0, BX_IA_CVTPD2PS_VpsWpd },
  /* F3 */ { 0, BX_IA_CVTSS2SD_VsdWss },
  /* F2 */ { 0, BX_IA_CVTSD2SS_VssWsd },
};

// opcode 0F 5B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5B[4] = {
  /* -- */ { 0, BX_IA_CVTDQ2PS_VpsWdq },
  /* 66 */ { 0, BX_IA_CVTPS2DQ_VdqWps },
  /* F3 */ { 0, BX_IA_CVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 5C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5C[4] = {
  /* -- */ { 0, BX_IA_SUBPS_VpsWps },
  /* 66 */ { 0, BX_IA_SUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_SUBSS_VssWss },
  /* F2 */ { 0, BX_IA_SUBSD_VsdWsd },
};

// opcode 0F 5D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5D[4] = {
  /* -- */ { 0, BX_IA_MINPS_VpsWps },
  /* 66 */ { 0, BX_IA_MINPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MINSS_VssWss },
  /* F2 */ { 0, BX_IA_MINSD_VsdWsd },
};

// opcode 0F 5E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5E[4] = {
  /* -- */ { 0, BX_IA_DIVPS_VpsWps },
  /* 66 */ { 0, BX_IA_DIVPD_VpdWpd },
  /* F3 */ { 0, BX_IA_DIVSS_VssWss },
  /* F2 */ { 0, BX_IA_DIVSD_VsdWsd },
};

// opcode 0F 5F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F5F[4] = {
  /* -- */ { 0, BX_IA_MAXPS_VpsWps },
  /* 66 */ { 0, BX_IA_MAXPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MAXSS_VssWss },
  /* F2 */ { 0, BX_IA_MAXSD_VsdWsd },
};

// opcode 0F 60
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F60[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLBW_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 61
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F61[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLWD_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 62
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F62[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLDQ_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 63
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F63[4] = {
  /* -- */ { 0, BX_IA_PACKSSWB_PqQq },
  /* 66 */ { 0, BX_IA_PACKSSWB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F64[4] = {
  /* -- */ { 0, BX_IA_PCMPGTB_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 65
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F65[4] = {
  /* -- */ { 0, BX_IA_PCMPGTW_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 66
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F66[4] = {
  /* -- */ { 0, BX_IA_PCMPGTD_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 67
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F67[4] = {
  /* -- */ { 0, BX_IA_PACKUSWB_PqQq },
  /* 66 */ { 0, BX_IA_PACKUSWB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 68
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F68[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHBW_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 69
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F69[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHWD_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 6A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6A[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHDQ_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 6B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6B[4] = {
  /* -- */ { 0, BX_IA_PACKSSDW_PqQq },
  /* 66 */ { 0, BX_IA_PACKSSDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 6C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6C[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PUNPCKLQDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 6D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6D[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PUNPCKHQDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 6E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6E[] = {
  /* --    */ { 0, BX_IA_MOVD_PqEd },
  /* 66    */ { 0, BX_IA_MOVD_VdqEd },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_MOVQ_PqEq },
  /* 66 /q */ { 0, BX_IA_MOVQ_VdqEq },
  /* F3 /q */ { 0, BX_IA_ERROR },
  /* F2 /q */ { 0, BX_IA_ERROR },
#endif
};

// opcode 0F 6F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F6F[4] = {
  /* -- */ { 0, BX_IA_MOVQ_PqQq },
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdq },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 70
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F70[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PSHUFW_PqQqIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_PSHUFD_VdqWdqIb },
  /* F3 */ { BxImmediate_Ib, BX_IA_PSHUFHW_VdqWdqIb },
  /* F2 */ { BxImmediate_Ib, BX_IA_PSHUFLW_VdqWdqIb },
};

// opcode 0F 71
static const BxOpcodeInfo_t BxOpcodeTable0F71_G12[] = {
  /* -- /0 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /1 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /2 */ { BxImmediate_Ib, BX_IA_PSRLW_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRLW_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /3 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /4 */ { BxImmediate_Ib, BX_IA_PSRAW_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRAW_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /5 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /6 */ { BxImmediate_Ib, BX_IA_PSLLW_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSLLW_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /7 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
};

// opcode 0F 72
static const BxOpcodeInfo_t BxOpcodeTable0F72_G13[] = {
  /* -- /0 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /1 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /2 */ { BxImmediate_Ib, BX_IA_PSRLD_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRLD_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /3 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /4 */ { BxImmediate_Ib, BX_IA_PSRAD_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRAD_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /5 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /6 */ { BxImmediate_Ib, BX_IA_PSLLD_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSLLD_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /7 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
};

// opcode 0F 73
static const BxOpcodeInfo_t BxOpcodeTable0F73_G14[] = {
  /* -- /0 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /1 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /2 */ { BxImmediate_Ib, BX_IA_PSRLQ_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRLQ_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /3 */ { 0, BX_IA_ERROR },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSRLDQ_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /4 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /5 */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /6 */ { BxImmediate_Ib, BX_IA_PSLLQ_NqIb },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSLLQ_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /7 */ { 0, BX_IA_ERROR },
  /* 66    */ { BxImmediate_Ib, BX_IA_PSLLDQ_UdqIb },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
};

// opcode 0F 74
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F74[4] = {
  /* -- */ { 0, BX_IA_PCMPEQB_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 75
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F75[4] = {
  /* -- */ { 0, BX_IA_PCMPEQW_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 76
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F76[4] = {
  /* -- */ { 0, BX_IA_PCMPEQD_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 77
static const BxOpcodeInfo_t BxOpcodeTable0F77[] = {
  /* 0F 77 /w */ { BxNoPrefixSSE, BX_IA_EMMS },
  /* 0F 77 /d */ { BxNoPrefixSSE, BX_IA_EMMS },
#if BX_SUPPORT_X86_64
  /* 0F 77 /q */ { BxNoPrefixSSE, BX_IA_EMMS },
#endif
};

// opcode 0F 78
static const BxOpcodeInfo_t BxOpcodeTable0F78[] = {
  /* 0F 78 /32 */ { 0, BX_IA_VMREAD_EdGd }, // SSE prefix handled inside decode method
#if BX_SUPPORT_X86_64
  /* 0F 78 /64 */ { 0, BX_IA_VMREAD_EqGq }, // SSE prefix handled inside decode method
#endif
};

// opcode 0F 79
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F79_32[4] = {
  /* -- */ { 0, BX_IA_VMWRITE_GdEd },
  /* 66 */ { 0, BX_IA_EXTRQ_VdqUq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_INSERTQ_VdqUdq },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F79_64[4] = {
  /* -- */ { 0, BX_IA_VMWRITE_GqEq },
  /* 66 */ { 0, BX_IA_EXTRQ_VdqUq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_INSERTQ_VdqUdq },
};
#endif

// opcode 0F 7C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F7C[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_HADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HADDPS_VpsWps },
};

// opcode 0F 7D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F7D[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_HSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HSUBPS_VpsWps },
};

// opcode 0F 7E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F7E[] = {
  /* --    */ { 0, BX_IA_MOVD_EdPq },
  /* 66    */ { 0, BX_IA_MOVD_EdVd },
  /* F3    */ { 0, BX_IA_MOVQ_VqWq },
  /* F2    */ { 0, BX_IA_ERROR },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_MOVQ_EqPq },
  /* 66 /q */ { 0, BX_IA_MOVQ_EqVq },
  /* F3 /q */ { 0, BX_IA_MOVQ_VqWq },
  /* F2 /q */ { 0, BX_IA_ERROR },
#endif
};

// opcode 0F 7F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F7F[4] = {
  /* -- */ { 0, BX_IA_MOVQ_QqPq },
  /* 66 */ { 0, BX_IA_MOVDQA_WdqVdq },
  /* F3 */ { 0, BX_IA_MOVDQU_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 80
static const BxOpcodeInfo_t BxOpcodeTable0F80_32[] = {
  /* 0F 80 /w */ { BxImmediate_BrOff16, BX_IA_JO_Jw },
  /* 0F 80 /d */ { BxImmediate_BrOff32, BX_IA_JO_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F80_64[] = {
  /* 0F 80 /w */ { BxImmediate_BrOff32, BX_IA_JO_Jq },
  /* 0F 80 /d */ { BxImmediate_BrOff32, BX_IA_JO_Jq },
  /* 0F 80 /q */ { BxImmediate_BrOff32, BX_IA_JO_Jq },
};
#endif

// opcode 0F 81
static const BxOpcodeInfo_t BxOpcodeTable0F81_32[] = {
  /* 0F 81 /w */ { BxImmediate_BrOff16, BX_IA_JNO_Jw },
  /* 0F 81 /d */ { BxImmediate_BrOff32, BX_IA_JNO_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F81_64[] = {
  /* 0F 81 /w */ { BxImmediate_BrOff32, BX_IA_JNO_Jq },
  /* 0F 81 /d */ { BxImmediate_BrOff32, BX_IA_JNO_Jq },
  /* 0F 81 /q */ { BxImmediate_BrOff32, BX_IA_JNO_Jq },
};
#endif

// opcode 0F 82
static const BxOpcodeInfo_t BxOpcodeTable0F82_32[] = {
  /* 0F 82 /w */ { BxImmediate_BrOff16, BX_IA_JB_Jw },
  /* 0F 82 /d */ { BxImmediate_BrOff32, BX_IA_JB_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F82_64[] = {
  /* 0F 82 /w */ { BxImmediate_BrOff32, BX_IA_JB_Jq },
  /* 0F 82 /d */ { BxImmediate_BrOff32, BX_IA_JB_Jq },
  /* 0F 82 /q */ { BxImmediate_BrOff32, BX_IA_JB_Jq },
};
#endif

// opcode 0F 83
static const BxOpcodeInfo_t BxOpcodeTable0F83_32[] = {
  /* 0F 83 /w */ { BxImmediate_BrOff16, BX_IA_JNB_Jw },
  /* 0F 83 /d */ { BxImmediate_BrOff32, BX_IA_JNB_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F83_64[] = {
  /* 0F 83 /w */ { BxImmediate_BrOff32, BX_IA_JNB_Jq },
  /* 0F 83 /d */ { BxImmediate_BrOff32, BX_IA_JNB_Jq },
  /* 0F 83 /q */ { BxImmediate_BrOff32, BX_IA_JNB_Jq },
};
#endif

// opcode 0F 84
static const BxOpcodeInfo_t BxOpcodeTable0F84_32[] = {
  /* 0F 84 /w */ { BxImmediate_BrOff16, BX_IA_JZ_Jw },
  /* 0F 84 /d */ { BxImmediate_BrOff32, BX_IA_JZ_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F84_64[] = {
  /* 0F 84 /w */ { BxImmediate_BrOff32, BX_IA_JZ_Jq },
  /* 0F 84 /d */ { BxImmediate_BrOff32, BX_IA_JZ_Jq },
  /* 0F 84 /q */ { BxImmediate_BrOff32, BX_IA_JZ_Jq },
};
#endif

// opcode 0F 85
static const BxOpcodeInfo_t BxOpcodeTable0F85_32[] = {
  /* 0F 85 /w */ { BxImmediate_BrOff16, BX_IA_JNZ_Jw },
  /* 0F 85 /d */ { BxImmediate_BrOff32, BX_IA_JNZ_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F85_64[] = {
  /* 0F 85 /w */ { BxImmediate_BrOff32, BX_IA_JNZ_Jq },
  /* 0F 85 /d */ { BxImmediate_BrOff32, BX_IA_JNZ_Jq },
  /* 0F 85 /q */ { BxImmediate_BrOff32, BX_IA_JNZ_Jq },
};
#endif

// opcode 0F 86
static const BxOpcodeInfo_t BxOpcodeTable0F86_32[] = {
  /* 0F 86 /w */ { BxImmediate_BrOff16, BX_IA_JBE_Jw },
  /* 0F 86 /d */ { BxImmediate_BrOff32, BX_IA_JBE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F86_64[] = {
  /* 0F 86 /w */ { BxImmediate_BrOff32, BX_IA_JBE_Jq },
  /* 0F 86 /d */ { BxImmediate_BrOff32, BX_IA_JBE_Jq },
  /* 0F 86 /q */ { BxImmediate_BrOff32, BX_IA_JBE_Jq },
};
#endif

// opcode 0F 87
static const BxOpcodeInfo_t BxOpcodeTable0F87_32[] = {
  /* 0F 87 /w */ { BxImmediate_BrOff16, BX_IA_JNBE_Jw },
  /* 0F 87 /d */ { BxImmediate_BrOff32, BX_IA_JNBE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F87_64[] = {
  /* 0F 87 /w */ { BxImmediate_BrOff32, BX_IA_JNBE_Jq },
  /* 0F 87 /d */ { BxImmediate_BrOff32, BX_IA_JNBE_Jq },
  /* 0F 87 /q */ { BxImmediate_BrOff32, BX_IA_JNBE_Jq },
};
#endif

// opcode 0F 88
static const BxOpcodeInfo_t BxOpcodeTable0F88_32[] = {
  /* 0F 88 /w */ { BxImmediate_BrOff16, BX_IA_JS_Jw },
  /* 0F 88 /d */ { BxImmediate_BrOff32, BX_IA_JS_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F88_64[] = {
  /* 0F 88 /w */ { BxImmediate_BrOff32, BX_IA_JS_Jq },
  /* 0F 88 /d */ { BxImmediate_BrOff32, BX_IA_JS_Jq },
  /* 0F 88 /q */ { BxImmediate_BrOff32, BX_IA_JS_Jq },
};
#endif

// opcode 0F 89
static const BxOpcodeInfo_t BxOpcodeTable0F89_32[] = {
  /* 0F 89 /w */ { BxImmediate_BrOff16, BX_IA_JNS_Jw },
  /* 0F 89 /d */ { BxImmediate_BrOff32, BX_IA_JNS_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F89_64[] = {
  /* 0F 89 /w */ { BxImmediate_BrOff32, BX_IA_JNS_Jq },
  /* 0F 89 /d */ { BxImmediate_BrOff32, BX_IA_JNS_Jq },
  /* 0F 89 /q */ { BxImmediate_BrOff32, BX_IA_JNS_Jq },
};
#endif

// opcode 0F 8A
static const BxOpcodeInfo_t BxOpcodeTable0F8A_32[] = {
  /* 0F 8A /w */ { BxImmediate_BrOff16, BX_IA_JP_Jw },
  /* 0F 8A /d */ { BxImmediate_BrOff32, BX_IA_JP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8A_64[] = {
  /* 0F 8A /w */ { BxImmediate_BrOff32, BX_IA_JP_Jq },
  /* 0F 8A /d */ { BxImmediate_BrOff32, BX_IA_JP_Jq },
  /* 0F 8A /q */ { BxImmediate_BrOff32, BX_IA_JP_Jq },
};
#endif

// opcode 0F 8B
static const BxOpcodeInfo_t BxOpcodeTable0F8B_32[] = {
  /* 0F 8B /w */ { BxImmediate_BrOff16, BX_IA_JNP_Jw },
  /* 0F 8B /d */ { BxImmediate_BrOff32, BX_IA_JNP_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8B_64[] = {
  /* 0F 8B /w */ { BxImmediate_BrOff32, BX_IA_JNP_Jq },
  /* 0F 8B /d */ { BxImmediate_BrOff32, BX_IA_JNP_Jq },
  /* 0F 8B /q */ { BxImmediate_BrOff32, BX_IA_JNP_Jq },
};
#endif

// opcode 0F 8C
static const BxOpcodeInfo_t BxOpcodeTable0F8C_32[] = {
  /* 0F 8C /w */ { BxImmediate_BrOff16, BX_IA_JL_Jw },
  /* 0F 8C /d */ { BxImmediate_BrOff32, BX_IA_JL_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8C_64[] = {
  /* 0F 8C /w */ { BxImmediate_BrOff32, BX_IA_JL_Jq },
  /* 0F 8C /d */ { BxImmediate_BrOff32, BX_IA_JL_Jq },
  /* 0F 8C /q */ { BxImmediate_BrOff32, BX_IA_JL_Jq },
};
#endif

// opcode 0F 8D
static const BxOpcodeInfo_t BxOpcodeTable0F8D_32[] = {
  /* 0F 8D /w */ { BxImmediate_BrOff16, BX_IA_JNL_Jw },
  /* 0F 8D /d */ { BxImmediate_BrOff32, BX_IA_JNL_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8D_64[] = {
  /* 0F 8D /w */ { BxImmediate_BrOff32, BX_IA_JNL_Jq },
  /* 0F 8D /d */ { BxImmediate_BrOff32, BX_IA_JNL_Jq },
  /* 0F 8D /q */ { BxImmediate_BrOff32, BX_IA_JNL_Jq },
};
#endif

// opcode 0F 8E
static const BxOpcodeInfo_t BxOpcodeTable0F8E_32[] = {
  /* 0F 8E /w */ { BxImmediate_BrOff16, BX_IA_JLE_Jw },
  /* 0F 8E /d */ { BxImmediate_BrOff32, BX_IA_JLE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8E_64[] = {
  /* 0F 8E /w */ { BxImmediate_BrOff32, BX_IA_JLE_Jq },
  /* 0F 8E /d */ { BxImmediate_BrOff32, BX_IA_JLE_Jq },
  /* 0F 8E /q */ { BxImmediate_BrOff32, BX_IA_JLE_Jq },
};
#endif

// opcode 0F 8F
static const BxOpcodeInfo_t BxOpcodeTable0F8F_32[] = {
  /* 0F 8F /w */ { BxImmediate_BrOff16, BX_IA_JNLE_Jw },
  /* 0F 8F /d */ { BxImmediate_BrOff32, BX_IA_JNLE_Jd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0F8F_64[] = {
  /* 0F 8F /w */ { BxImmediate_BrOff32, BX_IA_JNLE_Jq },
  /* 0F 8F /d */ { BxImmediate_BrOff32, BX_IA_JNLE_Jq },
  /* 0F 8F /q */ { BxImmediate_BrOff32, BX_IA_JNLE_Jq },
};
#endif

// opcode 0F 90
static const BxOpcodeInfo_t BxOpcodeTable0F90[] = {
  /* 0F 90 /w */ { 0, BX_IA_SETO_Eb },
  /* 0F 90 /d */ { 0, BX_IA_SETO_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 90 /q */ { 0, BX_IA_SETO_Eb },
#endif
};

// opcode 0F 91
static const BxOpcodeInfo_t BxOpcodeTable0F91[] = {
  /* 0F 91 /w */ { 0, BX_IA_SETNO_Eb },
  /* 0F 91 /d */ { 0, BX_IA_SETNO_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 91 /q */ { 0, BX_IA_SETNO_Eb },
#endif
};

// opcode 0F 92
static const BxOpcodeInfo_t BxOpcodeTable0F92[] = {
  /* 0F 92 /w */ { 0, BX_IA_SETB_Eb },
  /* 0F 92 /d */ { 0, BX_IA_SETB_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 92 /q */ { 0, BX_IA_SETB_Eb },
#endif
};

// opcode 0F 93
static const BxOpcodeInfo_t BxOpcodeTable0F93[] = {
  /* 0F 93 /w */ { 0, BX_IA_SETNB_Eb },
  /* 0F 93 /d */ { 0, BX_IA_SETNB_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 93 /q */ { 0, BX_IA_SETNB_Eb },
#endif
};

// opcode 0F 94
static const BxOpcodeInfo_t BxOpcodeTable0F94[] = {
  /* 0F 94 /w */ { 0, BX_IA_SETZ_Eb },
  /* 0F 94 /d */ { 0, BX_IA_SETZ_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 94 /q */ { 0, BX_IA_SETZ_Eb },
#endif
};

// opcode 0F 95
static const BxOpcodeInfo_t BxOpcodeTable0F95[] = {
  /* 0F 95 /w */ { 0, BX_IA_SETNZ_Eb },
  /* 0F 95 /d */ { 0, BX_IA_SETNZ_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 95 /q */ { 0, BX_IA_SETNZ_Eb },
#endif
};

// opcode 0F 96
static const BxOpcodeInfo_t BxOpcodeTable0F96[] = {
  /* 0F 96 /w */ { 0, BX_IA_SETBE_Eb },
  /* 0F 96 /d */ { 0, BX_IA_SETBE_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 96 /q */ { 0, BX_IA_SETBE_Eb },
#endif
};

// opcode 0F 97
static const BxOpcodeInfo_t BxOpcodeTable0F97[] = {
  /* 0F 97 /w */ { 0, BX_IA_SETNBE_Eb },
  /* 0F 97 /d */ { 0, BX_IA_SETNBE_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 97 /q */ { 0, BX_IA_SETNBE_Eb },
#endif
};

// opcode 0F 98
static const BxOpcodeInfo_t BxOpcodeTable0F98[] = {
  /* 0F 98 /w */ { 0, BX_IA_SETS_Eb },
  /* 0F 98 /d */ { 0, BX_IA_SETS_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 98 /q */ { 0, BX_IA_SETS_Eb },
#endif
};

// opcode 0F 99
static const BxOpcodeInfo_t BxOpcodeTable0F99[] = {
  /* 0F 99 /w */ { 0, BX_IA_SETNS_Eb },
  /* 0F 99 /d */ { 0, BX_IA_SETNS_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 99 /q */ { 0, BX_IA_SETNS_Eb },
#endif
};

// opcode 0F 9A
static const BxOpcodeInfo_t BxOpcodeTable0F9A[] = {
  /* 0F 9A /w */ { 0, BX_IA_SETP_Eb },
  /* 0F 9A /d */ { 0, BX_IA_SETP_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9A /q */ { 0, BX_IA_SETP_Eb },
#endif
};

// opcode 0F 9B
static const BxOpcodeInfo_t BxOpcodeTable0F9B[] = {
  /* 0F 9B /w */ { 0, BX_IA_SETNP_Eb },
  /* 0F 9B /d */ { 0, BX_IA_SETNP_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9B /q */ { 0, BX_IA_SETNP_Eb },
#endif
};

// opcode 0F 9C
static const BxOpcodeInfo_t BxOpcodeTable0F9C[] = {
  /* 0F 9C /w */ { 0, BX_IA_SETL_Eb },
  /* 0F 9C /d */ { 0, BX_IA_SETL_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9C /q */ { 0, BX_IA_SETL_Eb },
#endif
};

// opcode 0F 9D
static const BxOpcodeInfo_t BxOpcodeTable0F9D[] = {
  /* 0F 9D /w */ { 0, BX_IA_SETNL_Eb },
  /* 0F 9D /d */ { 0, BX_IA_SETNL_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9D /q */ { 0, BX_IA_SETNL_Eb },
#endif
};

// opcode 0F 9E
static const BxOpcodeInfo_t BxOpcodeTable0F9E[] = {
  /* 0F 9E /w */ { 0, BX_IA_SETLE_Eb },
  /* 0F 9E /d */ { 0, BX_IA_SETLE_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9E /q */ { 0, BX_IA_SETLE_Eb },
#endif
};

// opcode 0F 9F
static const BxOpcodeInfo_t BxOpcodeTable0F9F[] = {
  /* 0F 9F /w */ { 0, BX_IA_SETNLE_Eb },
  /* 0F 9F /d */ { 0, BX_IA_SETNLE_Eb },
#if BX_SUPPORT_X86_64
  /* 0F 9F /q */ { 0, BX_IA_SETNLE_Eb },
#endif
};

// opcode 0F A0
static const BxOpcodeInfo_t BxOpcodeTable0FA0_32[] = {
  /* 0F A0 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A0 /d */ { 0, BX_IA_PUSH_Op32_Sw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FA0_64[] = {
  /* 0F A0 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A0 /d */ { 0, BX_IA_PUSH_Op64_Sw },
  /* 0F A0 /q */ { 0, BX_IA_PUSH_Op64_Sw },
};
#endif

// opcode 0F A1
static const BxOpcodeInfo_t BxOpcodeTable0FA1_32[] = {
  /* 0F A1 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F A1 /d */ { 0, BX_IA_POP_Op32_Sw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FA1_64[] = {
  /* 0F A1 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F A1 /d */ { 0, BX_IA_POP_Op64_Sw },
  /* 0F A1 /q */ { 0, BX_IA_POP_Op64_Sw },
};
#endif

// opcode 0F A2
static const BxOpcodeInfo_t BxOpcodeTable0FA2[] = {
  /* 0F A2 */ { 0, BX_IA_CPUID },
};

// opcode 0F A3
static const BxOpcodeInfo_t BxOpcodeTable0FA3[] = {
  /* 0F A3 /w */ { 0, BX_IA_BT_EwGw },
  /* 0F A3 /d */ { 0, BX_IA_BT_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F A3 /q */ { 0, BX_IA_BT_EqGq },
#endif
};

// opcode 0F A4
static const BxOpcodeInfo_t BxOpcodeTable0FA4[] = {
  /* 0F A4 /w */ { BxImmediate_Ib, BX_IA_SHLD_EwGwIb },
  /* 0F A4 /d */ { BxImmediate_Ib, BX_IA_SHLD_EdGdIb },
#if BX_SUPPORT_X86_64
  /* 0F A4 /q */ { BxImmediate_Ib, BX_IA_SHLD_EqGqIb },
#endif
};

// opcode 0F A5
static const BxOpcodeInfo_t BxOpcodeTable0FA5[] = {
  /* 0F A5 /w */ { 0, BX_IA_SHLD_EwGw },
  /* 0F A5 /d */ { 0, BX_IA_SHLD_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F A5 /q */ { 0, BX_IA_SHLD_EqGq },
#endif
};

// opcode 0F A8
static const BxOpcodeInfo_t BxOpcodeTable0FA8_32[] = {
  /* 0F A8 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A8 /d */ { 0, BX_IA_PUSH_Op32_Sw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FA8_64[] = {
  /* 0F A8 /w */ { 0, BX_IA_PUSH_Op16_Sw },
  /* 0F A8 /d */ { 0, BX_IA_PUSH_Op64_Sw },
  /* 0F A8 /q */ { 0, BX_IA_PUSH_Op64_Sw },
};
#endif

// opcode 0F A9
static const BxOpcodeInfo_t BxOpcodeTable0FA9_32[] = {
  /* 0F A9 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F A9 /d */ { 0, BX_IA_POP_Op32_Sw },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FA9_64[] = {
  /* 0F A9 /w */ { 0, BX_IA_POP_Op16_Sw },
  /* 0F A9 /d */ { 0, BX_IA_POP_Op64_Sw },
  /* 0F A9 /q */ { 0, BX_IA_POP_Op64_Sw },
};
#endif

// opcode 0F AA
static const BxOpcodeInfo_t BxOpcodeTable0FAA[] = {
  /* 0F AA */ { 0, BX_IA_RSM },
};

// opcode 0F AB
static const BxOpcodeInfo_t BxOpcodeTable0FAB[] = {
  /* 0F AB /w */ { 0, BX_IA_BTS_EwGw },
  /* 0F AB /d */ { 0, BX_IA_BTS_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F AB /q */ { 0, BX_IA_BTS_EqGq },
#endif
};

// opcode 0F AC
static const BxOpcodeInfo_t BxOpcodeTable0FAC[] = {
  /* 0F AC /w */ { BxImmediate_Ib, BX_IA_SHRD_EwGwIb },
  /* 0F AC /d */ { BxImmediate_Ib, BX_IA_SHRD_EdGdIb },
#if BX_SUPPORT_X86_64
  /* 0F AC /q */ { BxImmediate_Ib, BX_IA_SHRD_EqGqIb },
#endif
};

// opcode 0F AD
static const BxOpcodeInfo_t BxOpcodeTable0FAD[] = {
  /* 0F AD /w */ { 0, BX_IA_SHRD_EwGw },
  /* 0F AD /d */ { 0, BX_IA_SHRD_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F AD /q */ { 0, BX_IA_SHRD_EqGq },
#endif
};

// opcode 0F AE
static const BxOpcodeInfo_t BxOpcodeTable0FAE_G15_R32[] = {
  /* /r form */
  /* /0 */ { 0, BX_IA_ERROR },
  /* /1 */ { 0, BX_IA_ERROR },
  /* /2 */ { 0, BX_IA_ERROR },
  /* /3 */ { 0, BX_IA_ERROR },
  /* /4 */ { 0, BX_IA_ERROR },
  /* /5 */ { BxNoPrefixSSE, BX_IA_LFENCE },
  /* /6 */ { BxNoPrefixSSE, BX_IA_MFENCE },
  /* /7 */ { BxNoPrefixSSE, BX_IA_SFENCE },
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FAE_G15d_R64[8] = {
  /* /r form */
  /* /0 */ { BxPrefixSSEF3, BX_IA_RDFSBASE_Ed },
  /* /1 */ { BxPrefixSSEF3, BX_IA_RDGSBASE_Ed },
  /* /2 */ { BxPrefixSSEF3, BX_IA_WRFSBASE_Ed },
  /* /3 */ { BxPrefixSSEF3, BX_IA_WRGSBASE_Ed },
  /* /4 */ { 0, BX_IA_ERROR },
  /* /5 */ { BxNoPrefixSSE, BX_IA_LFENCE },
  /* /6 */ { BxNoPrefixSSE, BX_IA_MFENCE },
  /* /7 */ { BxNoPrefixSSE, BX_IA_SFENCE },
};

static const BxOpcodeInfo_t BxOpcodeTable0FAE_G15q_R64[8] = {
  /* /r form */
  /* /0 */ { BxPrefixSSEF3, BX_IA_RDFSBASE_Eq },
  /* /1 */ { BxPrefixSSEF3, BX_IA_RDGSBASE_Eq },
  /* /2 */ { BxPrefixSSEF3, BX_IA_WRFSBASE_Eq },
  /* /3 */ { BxPrefixSSEF3, BX_IA_WRGSBASE_Eq },
  /* /4 */ { 0, BX_IA_ERROR },
  /* /5 */ { BxNoPrefixSSE, BX_IA_LFENCE },
  /* /6 */ { BxNoPrefixSSE, BX_IA_MFENCE },
  /* /7 */ { BxNoPrefixSSE, BX_IA_SFENCE },
};
#endif

static const BxOpcodeInfo_t BxOpcodeTable0FAE_G15M[] = {
  /* /m form */
  /* -- /0 */ { 0, BX_IA_FXSAVE },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /1 */ { 0, BX_IA_FXRSTOR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /2 */ { 0, BX_IA_LDMXCSR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /3 */ { 0, BX_IA_STMXCSR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /4 */ { 0, BX_IA_XSAVE },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /5 */ { 0, BX_IA_XRSTOR },
  /* 66    */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /6 */ { 0, BX_IA_XSAVEOPT },
  /* 66    */ { 0, BX_IA_CLWB },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },

  /* -- /7 */ { 0, BX_IA_CLFLUSH },
  /* 66    */ { 0, BX_IA_CLFLUSHOPT },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
};

// opcode 0F AF
static const BxOpcodeInfo_t BxOpcodeTable0FAF[] = {
  /* 0F AF /w */ { 0, BX_IA_IMUL_GwEw },
  /* 0F AF /d */ { 0, BX_IA_IMUL_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F AF /q */ { 0, BX_IA_IMUL_GqEq },
#endif
};

// opcode 0F B0
static const BxOpcodeInfo_t BxOpcodeTable0FB0[] = {
  /* 0F B0 /w */ { 0, BX_IA_CMPXCHG_EbGb },
  /* 0F B0 /d */ { 0, BX_IA_CMPXCHG_EbGb },
#if BX_SUPPORT_X86_64
  /* 0F B0 /q */ { 0, BX_IA_CMPXCHG_EbGb },
#endif
};

// opcode 0F B1
static const BxOpcodeInfo_t BxOpcodeTable0FB1[] = {
  /* 0F B1 /w */ { 0, BX_IA_CMPXCHG_EwGw },
  /* 0F B1 /d */ { 0, BX_IA_CMPXCHG_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F B1 /q */ { 0, BX_IA_CMPXCHG_EqGq },
#endif
};

// opcode 0F B2
static const BxOpcodeInfo_t BxOpcodeTable0FB2[] = {
  /* 0F B2 /w */ { 0, BX_IA_LSS_GwMp },
  /* 0F B2 /d */ { 0, BX_IA_LSS_GdMp },
#if BX_SUPPORT_X86_64
  /* 0F B2 /q */ { 0, BX_IA_LSS_GqMp }, // TODO: LSS_GdMp for AMD CPU
#endif
};

// opcode 0F B3
static const BxOpcodeInfo_t BxOpcodeTable0FB3[] = {
  /* 0F B3 /w */ { 0, BX_IA_BTR_EwGw },
  /* 0F B3 /d */ { 0, BX_IA_BTR_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F B3 /q */ { 0, BX_IA_BTR_EqGq },
#endif
};

// opcode 0F B4
static const BxOpcodeInfo_t BxOpcodeTable0FB4[] = {
  /* 0F B4 /w */ { 0, BX_IA_LFS_GwMp },
  /* 0F B4 /d */ { 0, BX_IA_LFS_GdMp },
#if BX_SUPPORT_X86_64
  /* 0F B4 /q */ { 0, BX_IA_LFS_GqMp }, // TODO: LFS_GdMp for AMD CPU
#endif
};

// opcode 0F B5
static const BxOpcodeInfo_t BxOpcodeTable0FB5[] = {
  /* 0F B5 /w */ { 0, BX_IA_LGS_GwMp },
  /* 0F B5 /d */ { 0, BX_IA_LGS_GdMp },
#if BX_SUPPORT_X86_64
  /* 0F B5 /q */ { 0, BX_IA_LGS_GqMp }, // TODO: LGS_GdMp for AMD CPU
#endif
};

// opcode 0F B6
static const BxOpcodeInfo_t BxOpcodeTable0FB6[] = {
  /* 0F B6 /w */ { 0, BX_IA_MOVZX_GwEb },
  /* 0F B6 /d */ { 0, BX_IA_MOVZX_GdEb },
#if BX_SUPPORT_X86_64
  /* 0F B6 /q */ { 0, BX_IA_MOVZX_GqEb },
#endif
};

// opcode 0F B7
static const BxOpcodeInfo_t BxOpcodeTable0FB7[] = {
  /* 0F B7 /w */ { 0, BX_IA_MOV_GwEw }, // MOVZX_GwEw
  /* 0F B7 /d */ { 0, BX_IA_MOVZX_GdEw },
#if BX_SUPPORT_X86_64
  /* 0F B7 /q */ { 0, BX_IA_MOVZX_GqEw },
#endif
};

// opcode 0F B8
static const BxOpcodeInfo_t BxOpcodeTable0FB8[] = {
  /* 0F B8 /w */ { BxPrefixSSEF3, BX_IA_POPCNT_GwEw },
  /* 0F B8 /d */ { BxPrefixSSEF3, BX_IA_POPCNT_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F B8 /q */ { BxPrefixSSEF3, BX_IA_POPCNT_GqEq },
#endif
};

// opcode 0F B9
static const BxOpcodeInfo_t BxOpcodeTable0FB9[] = {
  /* 0F B9 /w */ { 0, BX_IA_UD1 },
  /* 0F B9 /d */ { 0, BX_IA_UD1 },
#if BX_SUPPORT_X86_64
  /* 0F B9 /q */ { 0, BX_IA_UD1 },
#endif
};

// opcode 0F BA
static const BxOpcodeInfo_t BxOpcodeTable0FBA_G8EvIv[] = {
  // osize = 16 bit
  /* /0 /w */ { 0, BX_IA_ERROR },
  /* /1 /w */ { 0, BX_IA_ERROR },
  /* /2 /w */ { 0, BX_IA_ERROR },
  /* /3 /w */ { 0, BX_IA_ERROR },
  /* /4 /w */ { BxImmediate_Ib, BX_IA_BT_EwIb },
  /* /5 /w */ { BxImmediate_Ib, BX_IA_BTS_EwIb },
  /* /6 /w */ { BxImmediate_Ib, BX_IA_BTR_EwIb },
  /* /7 /w */ { BxImmediate_Ib, BX_IA_BTC_EwIb },

  // osize = 32 bit
  /* /0 /d */ { 0, BX_IA_ERROR },
  /* /1 /d */ { 0, BX_IA_ERROR },
  /* /2 /d */ { 0, BX_IA_ERROR },
  /* /3 /d */ { 0, BX_IA_ERROR },
  /* /4 /d */ { BxImmediate_Ib, BX_IA_BT_EdIb },
  /* /5 /d */ { BxImmediate_Ib, BX_IA_BTS_EdIb },
  /* /6 /d */ { BxImmediate_Ib, BX_IA_BTR_EdIb },
  /* /7 /d */ { BxImmediate_Ib, BX_IA_BTC_EdIb },

#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* /0 /q */ { 0, BX_IA_ERROR },
  /* /1 /q */ { 0, BX_IA_ERROR },
  /* /2 /q */ { 0, BX_IA_ERROR },
  /* /3 /q */ { 0, BX_IA_ERROR },
  /* /4 /q */ { BxImmediate_Ib, BX_IA_BT_EqIb },
  /* /5 /q */ { BxImmediate_Ib, BX_IA_BTS_EqIb },
  /* /6 /q */ { BxImmediate_Ib, BX_IA_BTR_EqIb },
  /* /7 /q */ { BxImmediate_Ib, BX_IA_BTC_EqIb },
#endif
};

// opcode 0F BB
static const BxOpcodeInfo_t BxOpcodeTable0FBB[] = {
  /* 0F BB /w */ { 0, BX_IA_BTC_EwGw },
  /* 0F BB /d */ { 0, BX_IA_BTC_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F BB /q */ { 0, BX_IA_BTC_EqGq },
#endif
};

// opcode 0F BC
static const BxOpcodeInfo_t BxOpcodeTable0FBC[] = {
  /* 0F BC /w */ { 0, BX_IA_BSF_GwEw },
  /* 0F BC /d */ { 0, BX_IA_BSF_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F BC /q */ { 0, BX_IA_BSF_GqEq },
#endif
};

static const BxOpcodeInfo_t BxOpcodeTable0FBC_TZCNT[] = {
  /* F3 0F BC /w */ { 0, BX_IA_TZCNT_GwEw },
  /* F3 0F BC /d */ { 0, BX_IA_TZCNT_GdEd },
#if BX_SUPPORT_X86_64
  /* F3 0F BC /q */ { 0, BX_IA_TZCNT_GqEq },
#endif
};

// opcode 0F BD
static const BxOpcodeInfo_t BxOpcodeTable0FBD[] = {
  /* 0F BD /w */ { 0, BX_IA_BSR_GwEw },
  /* 0F BD /d */ { 0, BX_IA_BSR_GdEd },
#if BX_SUPPORT_X86_64
  /* 0F BD /q */ { 0, BX_IA_BSR_GqEq },
#endif
};

static const BxOpcodeInfo_t BxOpcodeTable0FBD_LZCNT[] = {
  /* F3 0F BD /w */ { 0, BX_IA_LZCNT_GwEw },
  /* F3 0F BD /d */ { 0, BX_IA_LZCNT_GdEd },
#if BX_SUPPORT_X86_64
  /* F3 0F BD /q */ { 0, BX_IA_LZCNT_GqEq },
#endif
};

// opcode 0F BE
static const BxOpcodeInfo_t BxOpcodeTable0FBE[] = {
  /* 0F BE /w */ { 0, BX_IA_MOVSX_GwEb },
  /* 0F BE /d */ { 0, BX_IA_MOVSX_GdEb },
#if BX_SUPPORT_X86_64
  /* 0F BE /q */ { 0, BX_IA_MOVSX_GqEb },
#endif
};

// opcode 0F BF
static const BxOpcodeInfo_t BxOpcodeTable0FBF[] = {
  /* 0F BF /w */ { 0, BX_IA_MOV_GwEw }, // MOVSX_GwEw
  /* 0F BF /d */ { 0, BX_IA_MOVSX_GdEw },
#if BX_SUPPORT_X86_64
  /* 0F BF /q */ { 0, BX_IA_MOVSX_GqEw },
#endif
};

// opcode 0F C0
static const BxOpcodeInfo_t BxOpcodeTable0FC0[] = {
  /* 0F C0 /w */ { 0, BX_IA_XADD_EbGb },
  /* 0F C0 /d */ { 0, BX_IA_XADD_EbGb },
#if BX_SUPPORT_X86_64
  /* 0F C0 /q */ { 0, BX_IA_XADD_EbGb },
#endif
};

// opcode 0F C1
static const BxOpcodeInfo_t BxOpcodeTable0FC1[] = {
  /* 0F C1 /w */ { 0, BX_IA_XADD_EwGw },
  /* 0F C1 /d */ { 0, BX_IA_XADD_EdGd },
#if BX_SUPPORT_X86_64
  /* 0F C1 /q */ { 0, BX_IA_XADD_EqGq },
#endif
};

// opcode 0F C2
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FC2[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_CMPPS_VpsWpsIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_CMPPD_VpdWpdIb },
  /* F3 */ { BxImmediate_Ib, BX_IA_CMPSS_VssWssIb },
  /* F2 */ { BxImmediate_Ib, BX_IA_CMPSD_VsdWsdIb },
};

// opcode 0F C3
static const BxOpcodeInfo_t BxOpcodeTable0FC3_32[] = {
  /* 0F C3 /w */ { BxNoPrefixSSE, BX_IA_MOVNTI_Op32_MdGd },
  /* 0F C3 /d */ { BxNoPrefixSSE, BX_IA_MOVNTI_Op32_MdGd },
};
#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeTable0FC3_64[] = {
  /* 0F C3 /w */ { BxNoPrefixSSE, BX_IA_MOVNTI_Op64_MdGd },
  /* 0F C3 /d */ { BxNoPrefixSSE, BX_IA_MOVNTI_Op64_MdGd },
  /* 0F C3 /q */ { BxNoPrefixSSE, BX_IA_MOVNTI_MqGq },
};
#endif

// opcode 0F C4
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FC4[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PINSRW_PqEwIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_PINSRW_VdqEwIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F C5
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FC5[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PEXTRW_GdNqIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRW_GdUdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F C6
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FC6[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_SHUFPS_VpsWpsIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_SHUFPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F C7 - handled through separate decode method

// opcode 0F C8 - 0F CF
static const BxOpcodeInfo_t BxOpcodeTable0FC8x0FCF[] = {
  /* 0F C8 /w */ { 0, BX_IA_BSWAP_RX },
  /* 0F C8 /d */ { 0, BX_IA_BSWAP_ERX },
#if BX_SUPPORT_X86_64
  /* 0F C8 /q */ { 0, BX_IA_BSWAP_RRX },
#endif
};

// opcode 0F D0
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD0[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ADDSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ADDSUBPS_VpsWps },
};

// opcode 0F D1
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD1[4] = {
  /* -- */ { 0, BX_IA_PSRLW_PqQq },
  /* 66 */ { 0, BX_IA_PSRLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D2
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD2[4] = {
  /* -- */ { 0, BX_IA_PSRLD_PqQq },
  /* 66 */ { 0, BX_IA_PSRLD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D3
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD3[4] = {
  /* -- */ { 0, BX_IA_PSRLQ_PqQq },
  /* 66 */ { 0, BX_IA_PSRLQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D4
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD4[4] = {
  /* -- */ { 0, BX_IA_PADDQ_PqQq },
  /* 66 */ { 0, BX_IA_PADDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D5
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD5[4] = {
  /* -- */ { 0, BX_IA_PMULLW_PqQq },
  /* 66 */ { 0, BX_IA_PMULLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D6
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD6[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_MOVQ_WqVq },
  /* F3 */ { 0, BX_IA_MOVQ2DQ_VdqQq },
  /* F2 */ { 0, BX_IA_MOVDQ2Q_PqUdq },
};

// opcode 0F D7
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD7[4] = {
  /* -- */ { 0, BX_IA_PMOVMSKB_GdNq },
  /* 66 */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D8
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD8[4] = {
  /* -- */ { 0, BX_IA_PSUBUSB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBUSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F D9
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FD9[4] = {
  /* -- */ { 0, BX_IA_PSUBUSW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBUSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DA
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDA[4] = {
  /* -- */ { 0, BX_IA_PMINUB_PqQq },
  /* 66 */ { 0, BX_IA_PMINUB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DB
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDB[4] = {
  /* -- */ { 0, BX_IA_PAND_PqQq },
  /* 66 */ { 0, BX_IA_PAND_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDC[4] = {
  /* -- */ { 0, BX_IA_PADDUSB_PqQq },
  /* 66 */ { 0, BX_IA_PADDUSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DD
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDD[4] = {
  /* -- */ { 0, BX_IA_PADDUSW_PqQq },
  /* 66 */ { 0, BX_IA_PADDUSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DE
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDE[4] = {
  /* -- */ { 0, BX_IA_PMAXUB_PqQq },
  /* 66 */ { 0, BX_IA_PMAXUB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F DF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FDF[4] = {
  /* -- */ { 0, BX_IA_PANDN_PqQq },
  /* 66 */ { 0, BX_IA_PANDN_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E0
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE0[4] = {
  /* -- */ { 0, BX_IA_PAVGB_PqQq },
  /* 66 */ { 0, BX_IA_PAVGB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E1
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE1[4] = {
  /* -- */ { 0, BX_IA_PSRAW_PqQq },
  /* 66 */ { 0, BX_IA_PSRAW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E2
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE2[4] = {
  /* -- */ { 0, BX_IA_PSRAD_PqQq },
  /* 66 */ { 0, BX_IA_PSRAD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E3
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE3[4] = {
  /* -- */ { 0, BX_IA_PAVGW_PqQq },
  /* 66 */ { 0, BX_IA_PAVGW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E4
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE4[4] = {
  /* -- */ { 0, BX_IA_PMULHUW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHUW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E5
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE5[4] = {
  /* -- */ { 0, BX_IA_PMULHW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E6
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE6[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_CVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_CVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_CVTPD2DQ_VqWpd },
};

// opcode 0F E7
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE7[4] = {
  /* -- */ { 0, BX_IA_MOVNTQ_MqPq },
  /* 66 */ { 0, BX_IA_MOVNTDQ_MdqVdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E8
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE8[4] = {
  /* -- */ { 0, BX_IA_PSUBSB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F E9
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FE9[4] = {
  /* -- */ { 0, BX_IA_PSUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F EA
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FEA[4] = {
  /* -- */ { 0, BX_IA_PMINSW_PqQq },
  /* 66 */ { 0, BX_IA_PMINSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F EB
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FEB[4] = {
  /* -- */ { 0, BX_IA_POR_PqQq },
  /* 66 */ { 0, BX_IA_POR_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F EC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FEC[4] = {
  /* -- */ { 0, BX_IA_PADDSB_PqQq },
  /* 66 */ { 0, BX_IA_PADDSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F ED
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FED[4] = {
  /* -- */ { 0, BX_IA_PADDSW_PqQq },
  /* 66 */ { 0, BX_IA_PADDSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F EE
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FEE[4] = {
  /* -- */ { 0, BX_IA_PMAXSW_PqQq },
  /* 66 */ { 0, BX_IA_PMAXSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F EF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FEF[4] = {
  /* -- */ { 0, BX_IA_PXOR_PqQq },
  /* 66 */ { 0, BX_IA_PXOR_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F0
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF0[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_LDDQU_VdqMdq },
};

// opcode 0F F1
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF1[4] = {
  /* -- */ { 0, BX_IA_PSLLW_PqQq },
  /* 66 */ { 0, BX_IA_PSLLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F2
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF2[4] = {
  /* -- */ { 0, BX_IA_PSLLD_PqQq },
  /* 66 */ { 0, BX_IA_PSLLD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F3
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF3[4] = {
  /* -- */ { 0, BX_IA_PSLLQ_PqQq },
  /* 66 */ { 0, BX_IA_PSLLQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F4
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF4[4] = {
  /* -- */ { 0, BX_IA_PMULUDQ_PqQq },
  /* 66 */ { 0, BX_IA_PMULUDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F5
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF5[4] = {
  /* -- */ { 0, BX_IA_PMADDWD_PqQq },
  /* 66 */ { 0, BX_IA_PMADDWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F6
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF6[4] = {
  /* -- */ { 0, BX_IA_PSADBW_PqQq },
  /* 66 */ { 0, BX_IA_PSADBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F7
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF7[4] = {
  /* -- */ { 0, BX_IA_MASKMOVQ_PqNq },
  /* 66 */ { 0, BX_IA_MASKMOVDQU_VdqUdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F8
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF8[4] = {
  /* -- */ { 0, BX_IA_PSUBB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F F9
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FF9[4] = {
  /* -- */ { 0, BX_IA_PSUBW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FA
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FFA[4] = {
  /* -- */ { 0, BX_IA_PSUBD_PqQq },
  /* 66 */ { 0, BX_IA_PSUBD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FB
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FFB[4] = {
  /* -- */ { 0, BX_IA_PSUBQ_PqQq },
  /* 66 */ { 0, BX_IA_PSUBQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FFC[4] = {
  /* -- */ { 0, BX_IA_PADDB_PqQq },
  /* 66 */ { 0, BX_IA_PADDB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FD
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FFD[4] = {
  /* -- */ { 0, BX_IA_PADDW_PqQq },
  /* 66 */ { 0, BX_IA_PADDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FE
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0FFE[4] = {
  /* -- */ { 0, BX_IA_PADDD_PqQq },
  /* 66 */ { 0, BX_IA_PADDD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F FF
static const BxOpcodeInfo_t BxOpcodeTable0FFF[] = {
  /* 0F FF */ { 0, BX_IA_UD0 },
};

#endif // BX_FETCHDECODE_OPMAP_H
