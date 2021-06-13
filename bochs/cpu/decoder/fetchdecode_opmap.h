/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2019 Stanislav Shwartsman
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
static const Bit64u BxOpcodeTable00[] = { last_opcode_lockable(0, BX_IA_ADD_EbGb) };

// opcode 01
static const Bit64u BxOpcodeTable01[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_ADD_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_ADD_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_ADD_EwGw)
};

// opcode 02
static const Bit64u BxOpcodeTable02[] = { last_opcode(0, BX_IA_ADD_GbEb) };

// opcode 03
static const Bit64u BxOpcodeTable03[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_ADD_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_ADD_GdEd),
  last_opcode(ATTR_OS16, BX_IA_ADD_GwEw)
};

// opcode 04
static const Bit64u BxOpcodeTable04[] = { last_opcode(0, BX_IA_ADD_ALIb) };

// opcode 05
static const Bit64u BxOpcodeTable05[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_ADD_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_ADD_EAXId),
  last_opcode(ATTR_OS16, BX_IA_ADD_AXIw)
};

// opcode 06
static const Bit64u BxOpcodeTable06[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_PUSH_Op16_Sw)
};

// opcode 07
static const Bit64u BxOpcodeTable07[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_POP_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_POP_Op16_Sw)
};

// opcode 08
static const Bit64u BxOpcodeTable08[] = { last_opcode_lockable(0, BX_IA_OR_EbGb) };

// opcode 09
static const Bit64u BxOpcodeTable09[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_OR_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_OR_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_OR_EwGw)
};

// opcode 0A
static const Bit64u BxOpcodeTable0A[] = { last_opcode(0, BX_IA_OR_GbEb) };

// opcode 0B
static const Bit64u BxOpcodeTable0B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_OR_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_OR_GdEd),
  last_opcode(ATTR_OS16, BX_IA_OR_GwEw)
};

// opcode 0C
static const Bit64u BxOpcodeTable0C[] = { last_opcode(0, BX_IA_OR_ALIb) };

// opcode 0D
static const Bit64u BxOpcodeTable0D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_OR_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_OR_EAXId),
  last_opcode(ATTR_OS16, BX_IA_OR_AXIw)
};

// opcode 0E
static const Bit64u BxOpcodeTable0E[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_PUSH_Op16_Sw)
};

// opcode 10
static const Bit64u BxOpcodeTable10[] = { last_opcode_lockable(0, BX_IA_ADC_EbGb) };

// opcode 11
static const Bit64u BxOpcodeTable11[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_ADC_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_ADC_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_ADC_EwGw)
};

// opcode 12
static const Bit64u BxOpcodeTable12[] = { last_opcode(0, BX_IA_ADC_GbEb) };

// opcode 13
static const Bit64u BxOpcodeTable13[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_ADC_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_ADC_GdEd),
  last_opcode(ATTR_OS16, BX_IA_ADC_GwEw)
};

// opcode 14
static const Bit64u BxOpcodeTable14[] = { last_opcode(0, BX_IA_ADC_ALIb) };

// opcode 15
static const Bit64u BxOpcodeTable15[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_ADC_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_ADC_EAXId),
  last_opcode(ATTR_OS16, BX_IA_ADC_AXIw)
};

// opcode 16
static const Bit64u BxOpcodeTable16[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_PUSH_Op16_Sw)
};

// opcode 17
static const Bit64u BxOpcodeTable17[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_POP_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_POP_Op16_Sw)
};

// opcode 18
static const Bit64u BxOpcodeTable18[] = { last_opcode_lockable(0, BX_IA_SBB_EbGb) };

// opcode 19
static const Bit64u BxOpcodeTable19[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_SBB_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_SBB_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_SBB_EwGw)
};

// opcode 1A
static const Bit64u BxOpcodeTable1A[] = { last_opcode(0, BX_IA_SBB_GbEb) };

// opcode 1B
static const Bit64u BxOpcodeTable1B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SBB_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_SBB_GdEd),
  last_opcode(ATTR_OS16, BX_IA_SBB_GwEw)
};

// opcode 1C
static const Bit64u BxOpcodeTable1C[] = { last_opcode(0, BX_IA_SBB_ALIb) };

// opcode 1D
static const Bit64u BxOpcodeTable1D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SBB_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_SBB_EAXId),
  last_opcode(ATTR_OS16, BX_IA_SBB_AXIw)
};

// opcode 1E
static const Bit64u BxOpcodeTable1E[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_PUSH_Op16_Sw)
};

// opcode 1F
static const Bit64u BxOpcodeTable1F[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_POP_Op32_Sw),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_POP_Op16_Sw)
};

// opcode 20
static const Bit64u BxOpcodeTable20[] = { last_opcode_lockable(0, BX_IA_AND_EbGb) };

// opcode 21
static const Bit64u BxOpcodeTable21[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_AND_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_AND_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_AND_EwGw)
};

// opcode 22
static const Bit64u BxOpcodeTable22[] = { last_opcode(0, BX_IA_AND_GbEb) };

// opcode 23
static const Bit64u BxOpcodeTable23[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_AND_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_AND_GdEd),
  last_opcode(ATTR_OS16, BX_IA_AND_GwEw)
};

// opcode 24
static const Bit64u BxOpcodeTable24[] = { last_opcode(0, BX_IA_AND_ALIb) };

// opcode 25
static const Bit64u BxOpcodeTable25[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_AND_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_AND_EAXId),
  last_opcode(ATTR_OS16, BX_IA_AND_AXIw)
};

// opcode 27
static const Bit64u BxOpcodeTable27[] = { last_opcode(0, BX_IA_DAA) };

// opcode 28
static const Bit64u BxOpcodeTable28[] = { last_opcode_lockable(0, BX_IA_SUB_EbGb) };

// opcode 29
static const Bit64u BxOpcodeTable29[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SRC_EQ_DST, BX_IA_SUB_EqGq_ZERO_IDIOM),
#endif
  form_opcode(ATTR_OS32 | ATTR_SRC_EQ_DST, BX_IA_SUB_EdGd_ZERO_IDIOM),
  form_opcode(ATTR_OS16 | ATTR_SRC_EQ_DST, BX_IA_SUB_EwGw_ZERO_IDIOM),

#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_SUB_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_SUB_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_SUB_EwGw)
};

// opcode 2A
static const Bit64u BxOpcodeTable2A[] = { last_opcode(0, BX_IA_SUB_GbEb) };

// opcode 2B
static const Bit64u BxOpcodeTable2B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SRC_EQ_DST, BX_IA_SUB_GqEq_ZERO_IDIOM),
#endif
  form_opcode(ATTR_OS32 | ATTR_SRC_EQ_DST, BX_IA_SUB_GdEd_ZERO_IDIOM),
  form_opcode(ATTR_OS16 | ATTR_SRC_EQ_DST, BX_IA_SUB_GwEw_ZERO_IDIOM),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SUB_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_SUB_GdEd),
  last_opcode(ATTR_OS16, BX_IA_SUB_GwEw)
};

// opcode 2C
static const Bit64u BxOpcodeTable2C[] = { last_opcode(0, BX_IA_SUB_ALIb) };

// opcode 2D
static const Bit64u BxOpcodeTable2D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SUB_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_SUB_EAXId),
  last_opcode(ATTR_OS16, BX_IA_SUB_AXIw)
};

// opcode 2F
static const Bit64u BxOpcodeTable2F[] = { last_opcode(0, BX_IA_DAS) };

// opcode 30
static const Bit64u BxOpcodeTable30[] = { last_opcode_lockable(0, BX_IA_XOR_EbGb) };

// opcode 31
static const Bit64u BxOpcodeTable31[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SRC_EQ_DST, BX_IA_XOR_EqGq_ZERO_IDIOM),
#endif
  form_opcode(ATTR_OS32 | ATTR_SRC_EQ_DST, BX_IA_XOR_EdGd_ZERO_IDIOM),
  form_opcode(ATTR_OS16 | ATTR_SRC_EQ_DST, BX_IA_XOR_EwGw_ZERO_IDIOM),

#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_XOR_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_XOR_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_XOR_EwGw)
};

// opcode 32
static const Bit64u BxOpcodeTable32[] = { last_opcode(0, BX_IA_XOR_GbEb) };

// opcode 33
static const Bit64u BxOpcodeTable33[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SRC_EQ_DST, BX_IA_XOR_GqEq_ZERO_IDIOM),
#endif
  form_opcode(ATTR_OS32 | ATTR_SRC_EQ_DST, BX_IA_XOR_GdEd_ZERO_IDIOM),
  form_opcode(ATTR_OS16 | ATTR_SRC_EQ_DST, BX_IA_XOR_GwEw_ZERO_IDIOM),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_XOR_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_XOR_GdEd),
  last_opcode(ATTR_OS16, BX_IA_XOR_GwEw)
};

// opcode 34
static const Bit64u BxOpcodeTable34[] = { last_opcode(0, BX_IA_XOR_ALIb) };

// opcode 35
static const Bit64u BxOpcodeTable35[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_XOR_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_XOR_EAXId),
  last_opcode(ATTR_OS16, BX_IA_XOR_AXIw)
};

// opcode 37
static const Bit64u BxOpcodeTable37[] = { last_opcode(0, BX_IA_AAA) };

// opcode 38
static const Bit64u BxOpcodeTable38[] = { last_opcode(0, BX_IA_CMP_EbGb) };

// opcode 39
static const Bit64u BxOpcodeTable39[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMP_EqGq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMP_EdGd),
  last_opcode(ATTR_OS16, BX_IA_CMP_EwGw)
};

// opcode 3A
static const Bit64u BxOpcodeTable3A[] = { last_opcode(0, BX_IA_CMP_GbEb) };

// opcode 3B
static const Bit64u BxOpcodeTable3B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMP_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMP_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMP_GwEw)
};

// opcode 3C
static const Bit64u BxOpcodeTable3C[] = { last_opcode(0, BX_IA_CMP_ALIb) };

// opcode 3D
static const Bit64u BxOpcodeTable3D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMP_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMP_EAXId),
  last_opcode(ATTR_OS16, BX_IA_CMP_AXIw)
};

// opcode 3F
static const Bit64u BxOpcodeTable3F[] = { last_opcode(0, BX_IA_AAS) };

// opcode 40 - 47
static const Bit64u BxOpcodeTable40x47[] = { 
  form_opcode_lockable(ATTR_OS32 | ATTR_IS32, BX_IA_INC_Ed),
  last_opcode_lockable(ATTR_OS16 | ATTR_IS32, BX_IA_INC_Ew)
};

// opcode 48 - 4F
static const Bit64u BxOpcodeTable48x4F[] = { 
  form_opcode_lockable(ATTR_OS32 | ATTR_IS32, BX_IA_DEC_Ed),
  last_opcode_lockable(ATTR_OS16 | ATTR_IS32, BX_IA_DEC_Ew)
};

// opcode 50 - 57
static const Bit64u BxOpcodeTable50x57[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_PUSH_Eq),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_PUSH_Ed),
  last_opcode(ATTR_OS16,                BX_IA_PUSH_Ew)
};

// opcode 58 - 5F
static const Bit64u BxOpcodeTable58x5F[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_POP_Eq),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_POP_Ed),
  last_opcode(ATTR_OS16,                BX_IA_POP_Ew)
};

// opcode 60
static const Bit64u BxOpcodeTable60[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_PUSHA_Op32),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_PUSHA_Op16)
};

// opcode 61
static const Bit64u BxOpcodeTable61[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_POPA_Op32),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_POPA_Op16)
};

// opcode 62 - EVEX prefix
static const Bit64u BxOpcodeTable62[] = {
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_BOUND_GdMa),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_BOUND_GwMa)
};

// opcode 63
static const Bit64u BxOpcodeTable63_32[] = { last_opcode(ATTR_OS16_32, BX_IA_ARPL_EwGw) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable63_64[] = { 
  form_opcode(ATTR_OS64, BX_IA_MOVSXD_GqEd),
  form_opcode(ATTR_OS32, BX_IA_MOV_Op64_GdEd),  // MOVSX_GdEd
  last_opcode(ATTR_OS16, BX_IA_MOV_GwEw)        // MOVSX_GwEw
};
#endif

// opcode 68
static const Bit64u BxOpcodeTable68[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_PUSH_Op64_Id),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_PUSH_Id),
  last_opcode(ATTR_OS16,                BX_IA_PUSH_Iw)
};

// opcode 69
static const Bit64u BxOpcodeTable69[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_IMUL_GqEqId),
#endif
  form_opcode(ATTR_OS32, BX_IA_IMUL_GdEdId),
  last_opcode(ATTR_OS16, BX_IA_IMUL_GwEwIw)
};

// opcode 6A
static const Bit64u BxOpcodeTable6A[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_PUSH_Op64_sIb),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_PUSH_sIb32),    // imm8 sign extended to 32-bit
  last_opcode(ATTR_OS16,                BX_IA_PUSH_sIb16)     // imm8 sign extended to 16-bit
};

// opcode 6B
static const Bit64u BxOpcodeTable6B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_IMUL_GqEqsIb),
#endif
  form_opcode(ATTR_OS32, BX_IA_IMUL_GdEdsIb),
  last_opcode(ATTR_OS16, BX_IA_IMUL_GwEwsIb)
};

// opcode 6C
static const Bit64u BxOpcodeTable6C[] = { last_opcode(0, BX_IA_REP_INSB_YbDX) };

// opcode 6D
static const Bit64u BxOpcodeTable6D[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_REP_INSD_YdDX),
  last_opcode(ATTR_OS16,    BX_IA_REP_INSW_YwDX)
};

// opcode 6E
static const Bit64u BxOpcodeTable6E[] = { last_opcode(0, BX_IA_REP_OUTSB_DXXb) };

// opcode 6F
static const Bit64u BxOpcodeTable6F[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_REP_OUTSD_DXXd),
  last_opcode(ATTR_OS16,    BX_IA_REP_OUTSW_DXXw)
};

// opcode 70
static const Bit64u BxOpcodeTable70_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JO_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JO_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable70_64[] = { last_opcode(0, BX_IA_JO_Jbq) };
#endif

// opcode 71
static const Bit64u BxOpcodeTable71_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNO_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNO_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable71_64[] = { last_opcode(0, BX_IA_JNO_Jbq) };
#endif

// opcode 72
static const Bit64u BxOpcodeTable72_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JB_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JB_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable72_64[] = { last_opcode(0, BX_IA_JB_Jbq) };
#endif

// opcode 73
static const Bit64u BxOpcodeTable73_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNB_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNB_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable73_64[] = { last_opcode(0, BX_IA_JNB_Jbq) };
#endif

// opcode 74
static const Bit64u BxOpcodeTable74_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JZ_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JZ_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable74_64[] = { last_opcode(0, BX_IA_JZ_Jbq) };
#endif

// opcode 75
static const Bit64u BxOpcodeTable75_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNZ_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNZ_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable75_64[] = { last_opcode(0, BX_IA_JNZ_Jbq) };
#endif

// opcode 76
static const Bit64u BxOpcodeTable76_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JBE_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JBE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable76_64[] = { last_opcode(0, BX_IA_JBE_Jbq) };
#endif

// opcode 77
static const Bit64u BxOpcodeTable77_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNBE_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNBE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable77_64[] = { last_opcode(0, BX_IA_JNBE_Jbq) };
#endif

// opcode 78
static const Bit64u BxOpcodeTable78_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JS_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JS_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable78_64[] = { last_opcode(0, BX_IA_JS_Jbq) };
#endif

// opcode 79
static const Bit64u BxOpcodeTable79_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNS_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNS_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable79_64[] = { last_opcode(0, BX_IA_JNS_Jbq) };
#endif

// opcode 7A
static const Bit64u BxOpcodeTable7A_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JP_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JP_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7A_64[] = { last_opcode(0, BX_IA_JP_Jbq) };
#endif

// opcode 7B
static const Bit64u BxOpcodeTable7B_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNP_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNP_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7B_64[] = { last_opcode(0, BX_IA_JNP_Jbq) };
#endif

// opcode 7C
static const Bit64u BxOpcodeTable7C_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JL_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JL_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7C_64[] = { last_opcode(0, BX_IA_JL_Jbq) };
#endif

// opcode 7D
static const Bit64u BxOpcodeTable7D_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNL_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNL_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7D_64[] = { last_opcode(0, BX_IA_JNL_Jbq) };
#endif

// opcode 7E
static const Bit64u BxOpcodeTable7E_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JLE_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JLE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7E_64[] = { last_opcode(0, BX_IA_JLE_Jbq) };
#endif

// opcode 7F
static const Bit64u BxOpcodeTable7F_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNLE_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JNLE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable7F_64[] = { last_opcode(0, BX_IA_JNLE_Jbq) };
#endif

// opcode 80
static const Bit64u BxOpcodeTable80[] = {
  form_opcode_lockable(ATTR_NNN0, BX_IA_ADD_EbIb),
  form_opcode_lockable(ATTR_NNN1, BX_IA_OR_EbIb),
  form_opcode_lockable(ATTR_NNN2, BX_IA_ADC_EbIb),
  form_opcode_lockable(ATTR_NNN3, BX_IA_SBB_EbIb),
  form_opcode_lockable(ATTR_NNN4, BX_IA_AND_EbIb),
  form_opcode_lockable(ATTR_NNN5, BX_IA_SUB_EbIb),
  form_opcode_lockable(ATTR_NNN6, BX_IA_XOR_EbIb),
  last_opcode(ATTR_NNN7, BX_IA_CMP_EbIb)
};

// opcode 81
static const Bit64u BxOpcodeTable81[] = {
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_NNN0 | ATTR_OS64, BX_IA_ADD_EqId),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS64, BX_IA_OR_EqId),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS64, BX_IA_ADC_EqId),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS64, BX_IA_SBB_EqId),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS64, BX_IA_AND_EqId),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS64, BX_IA_SUB_EqId),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS64, BX_IA_XOR_EqId),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_CMP_EqId),
#endif

  form_opcode_lockable(ATTR_NNN0 | ATTR_OS32, BX_IA_ADD_EdId),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS32, BX_IA_OR_EdId),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS32, BX_IA_ADC_EdId),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS32, BX_IA_SBB_EdId),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS32, BX_IA_AND_EdId),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS32, BX_IA_SUB_EdId),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS32, BX_IA_XOR_EdId),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_CMP_EdId),

  form_opcode_lockable(ATTR_NNN0 | ATTR_OS16, BX_IA_ADD_EwIw),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS16, BX_IA_OR_EwIw),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS16, BX_IA_ADC_EwIw),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS16, BX_IA_SBB_EwIw),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS16, BX_IA_AND_EwIw),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS16, BX_IA_SUB_EwIw),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS16, BX_IA_XOR_EwIw),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_CMP_EwIw)
};

// opcode 83
static const Bit64u BxOpcodeTable83[] = {
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_NNN0 | ATTR_OS64, BX_IA_ADD_EqsIb),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS64, BX_IA_OR_EqsIb),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS64, BX_IA_ADC_EqsIb),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS64, BX_IA_SBB_EqsIb),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS64, BX_IA_AND_EqsIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS64, BX_IA_SUB_EqsIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS64, BX_IA_XOR_EqsIb),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_CMP_EqsIb),
#endif

  form_opcode_lockable(ATTR_NNN0 | ATTR_OS32, BX_IA_ADD_EdsIb),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS32, BX_IA_OR_EdsIb),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS32, BX_IA_ADC_EdsIb),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS32, BX_IA_SBB_EdsIb),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS32, BX_IA_AND_EdsIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS32, BX_IA_SUB_EdsIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS32, BX_IA_XOR_EdsIb),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_CMP_EdsIb),

  form_opcode_lockable(ATTR_NNN0 | ATTR_OS16, BX_IA_ADD_EwsIb),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS16, BX_IA_OR_EwsIb),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS16, BX_IA_ADC_EwsIb),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS16, BX_IA_SBB_EwsIb),
  form_opcode_lockable(ATTR_NNN4 | ATTR_OS16, BX_IA_AND_EwsIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS16, BX_IA_SUB_EwsIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS16, BX_IA_XOR_EwsIb),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_CMP_EwsIb)
};

// opcode 84
static const Bit64u BxOpcodeTable84[] = { last_opcode(0, BX_IA_TEST_EbGb) };

// opcode 85
static const Bit64u BxOpcodeTable85[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_TEST_EqGq),
#endif
  form_opcode(ATTR_OS32, BX_IA_TEST_EdGd),
  last_opcode(ATTR_OS16, BX_IA_TEST_EwGw)
};

// opcode 86
static const Bit64u BxOpcodeTable86[] = { last_opcode_lockable(0, BX_IA_XCHG_EbGb) };

// opcode 87
static const Bit64u BxOpcodeTable87[] = {
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_XCHG_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_XCHG_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_XCHG_EwGw)
};

// opcode 88
static const Bit64u BxOpcodeTable88[] = { last_opcode(0, BX_IA_MOV_EbGb) };

// opcode 89 - split for better emulation performance
static const Bit64u BxOpcodeTable89[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOV_EqGq),
  form_opcode(ATTR_OS32 | ATTR_IS64, BX_IA_MOV_Op64_EdGd),
#endif
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_MOV_Op32_EdGd),
  last_opcode(ATTR_OS16, BX_IA_MOV_EwGw)
};

// opcode 8A
static const Bit64u BxOpcodeTable8A[] = { last_opcode(0, BX_IA_MOV_GbEb) };

// opcode 8B - split for better emulation performance
static const Bit64u BxOpcodeTable8B[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOV_GqEq),
  form_opcode(ATTR_OS32 | ATTR_IS64, BX_IA_MOV_Op64_GdEd),
#endif
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_MOV_Op32_GdEd),
  last_opcode(ATTR_OS16, BX_IA_MOV_GwEw)
};

// opcode 8C
static const Bit64u BxOpcodeTable8C[] = { last_opcode(0, BX_IA_MOV_EwSw) };

// opcode 8D
static const Bit64u BxOpcodeTable8D[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM, BX_IA_LEA_GqM),
#endif
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM, BX_IA_LEA_GdM),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM, BX_IA_LEA_GwM)
};

// opcode 8E
static const Bit64u BxOpcodeTable8E[] = { last_opcode(0, BX_IA_MOV_SwEw) };

// opcode 8F - XOP prefix
static const Bit64u BxOpcodeTable8F[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_OS32_64 | ATTR_NNN0, BX_IA_POP_Eq),
#endif
  form_opcode(ATTR_IS32 | ATTR_OS32    | ATTR_NNN0, BX_IA_POP_Ed),
  last_opcode(ATTR_OS16                | ATTR_NNN0, BX_IA_POP_Ew)
};

// opcode 90 - 97
static const Bit64u BxOpcodeTable90x97[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_XCHG_RRXRAX),    // handles also XCHG R8, RAX
#endif
  form_opcode(ATTR_OS32, BX_IA_XCHG_ERXEAX),    // handles also XCHG R8d, EAX
  last_opcode(ATTR_OS16, BX_IA_XCHG_RXAX)       // handles also XCHG R8w, AX
};

// opcode 98
static const Bit64u BxOpcodeTable98[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CDQE),
#endif
  form_opcode(ATTR_OS32, BX_IA_CWDE),
  last_opcode(ATTR_OS16, BX_IA_CBW)
};

// opcode 99
static const Bit64u BxOpcodeTable99[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CQO),
#endif
  form_opcode(ATTR_OS32, BX_IA_CDQ),
  last_opcode(ATTR_OS16, BX_IA_CWD)
};

// opcode 9A
static const Bit64u BxOpcodeTable9A[] = { 
  form_opcode(ATTR_OS32 | ATTR_IS32, BX_IA_CALLF_Op32_Ap),
  last_opcode(ATTR_OS16 | ATTR_IS32, BX_IA_CALLF_Op16_Ap)
};

// opcode 9B
static const Bit64u BxOpcodeTable9B[] = { last_opcode(0, BX_IA_FWAIT) };

// opcode 9C
static const Bit64u BxOpcodeTable9C[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_PUSHF_Fq),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_PUSHF_Fd),
  last_opcode(ATTR_OS16,                BX_IA_PUSHF_Fw)
};

// opcode 9D
static const Bit64u BxOpcodeTable9D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS32_64 | ATTR_IS64, BX_IA_POPF_Fq),
#endif
  form_opcode(ATTR_OS32    | ATTR_IS32, BX_IA_POPF_Fd),
  last_opcode(ATTR_OS16,                BX_IA_POPF_Fw)
};

// opcode 9E
static const Bit64u BxOpcodeTable9E_32[] = { last_opcode(0, BX_IA_SAHF) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable9E_64[] = { last_opcode(0, BX_IA_SAHF_LM) };
#endif

// opcode 9F
static const Bit64u BxOpcodeTable9F_32[] = { last_opcode(0, BX_IA_LAHF) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable9F_64[] = { last_opcode(0, BX_IA_LAHF_LM) };
#endif

// opcode A0
static const Bit64u BxOpcodeTableA0_32[] = { last_opcode(0, BX_IA_MOV_ALOd) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableA0_64[] = { last_opcode(0, BX_IA_MOV_ALOq) };
#endif

// opcode A1
static const Bit64u BxOpcodeTableA1_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_MOV_EAXOd),
  last_opcode(ATTR_OS16, BX_IA_MOV_AXOd)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableA1_64[] = { 
  form_opcode(ATTR_OS64, BX_IA_MOV_RAXOq),
  form_opcode(ATTR_OS32, BX_IA_MOV_EAXOq),
  last_opcode(ATTR_OS16, BX_IA_MOV_AXOq)
};
#endif

// opcode A2
static const Bit64u BxOpcodeTableA2_32[] = { last_opcode(0, BX_IA_MOV_OdAL) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableA2_64[] = { last_opcode(0, BX_IA_MOV_OqAL) };
#endif

// opcode A3
static const Bit64u BxOpcodeTableA3_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_MOV_OdEAX),
  last_opcode(ATTR_OS16, BX_IA_MOV_OdAX)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableA3_64[] = { 
  form_opcode(ATTR_OS64, BX_IA_MOV_OqRAX),
  form_opcode(ATTR_OS32, BX_IA_MOV_OqEAX),
  last_opcode(ATTR_OS16, BX_IA_MOV_OqAX)
};
#endif

// opcode A4
static const Bit64u BxOpcodeTableA4[] = { last_opcode(0, BX_IA_REP_MOVSB_YbXb) };

// opcode A5
static const Bit64u BxOpcodeTableA5[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_REP_MOVSQ_YqXq),
#endif
  form_opcode(ATTR_OS32, BX_IA_REP_MOVSD_YdXd),
  last_opcode(ATTR_OS16, BX_IA_REP_MOVSW_YwXw)
};

// opcode A6
static const Bit64u BxOpcodeTableA6[] = { last_opcode(0, BX_IA_REP_CMPSB_XbYb) };

// opcode A7
static const Bit64u BxOpcodeTableA7[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_REP_CMPSQ_XqYq),
#endif
  form_opcode(ATTR_OS32, BX_IA_REP_CMPSD_XdYd),
  last_opcode(ATTR_OS16, BX_IA_REP_CMPSW_XwYw)
};

// opcode A8
static const Bit64u BxOpcodeTableA8[] = { last_opcode(0, BX_IA_TEST_ALIb) };

// opcode A9
static const Bit64u BxOpcodeTableA9[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_TEST_RAXId),
#endif
  form_opcode(ATTR_OS32, BX_IA_TEST_EAXId),
  last_opcode(ATTR_OS16, BX_IA_TEST_AXIw)
};

// opcode AA
static const Bit64u BxOpcodeTableAA[] = { last_opcode(0, BX_IA_REP_STOSB_YbAL) };

// opcode AB
static const Bit64u BxOpcodeTableAB[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_REP_STOSQ_YqRAX),
#endif
  form_opcode(ATTR_OS32, BX_IA_REP_STOSD_YdEAX),
  last_opcode(ATTR_OS16, BX_IA_REP_STOSW_YwAX)
};

// opcode AC
static const Bit64u BxOpcodeTableAC[] = { last_opcode(0, BX_IA_REP_LODSB_ALXb) };

// opcode AD
static const Bit64u BxOpcodeTableAD[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_REP_LODSQ_RAXXq),
#endif
  form_opcode(ATTR_OS32, BX_IA_REP_LODSD_EAXXd),
  last_opcode(ATTR_OS16, BX_IA_REP_LODSW_AXXw)
};

// opcode AE
static const Bit64u BxOpcodeTableAE[] = { last_opcode(0, BX_IA_REP_SCASB_ALYb) };

// opcode AF
static const Bit64u BxOpcodeTableAF[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_REP_SCASQ_RAXYq),
#endif
  form_opcode(ATTR_OS32, BX_IA_REP_SCASD_EAXYd),
  last_opcode(ATTR_OS16, BX_IA_REP_SCASW_AXYw)
};

// opcode B0 - B7
static const Bit64u BxOpcodeTableB0xB7[] = { last_opcode(0, BX_IA_MOV_EbIb) };

// opcode B8 - BF
static const Bit64u BxOpcodeTableB8xBF[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOV_RRXIq),
#endif
  form_opcode(ATTR_OS32, BX_IA_MOV_EdId),
  last_opcode(ATTR_OS16, BX_IA_MOV_EwIw)
};

// opcode D0
static const Bit64u BxOpcodeTableC0[] = {
  form_opcode(ATTR_NNN0, BX_IA_ROL_EbIb),
  form_opcode(ATTR_NNN1, BX_IA_ROR_EbIb),
  form_opcode(ATTR_NNN2, BX_IA_RCL_EbIb),
  form_opcode(ATTR_NNN3, BX_IA_RCR_EbIb),
  form_opcode(ATTR_NNN4, BX_IA_SHL_EbIb),
  form_opcode(ATTR_NNN5, BX_IA_SHR_EbIb),
  form_opcode(ATTR_NNN6, BX_IA_SHL_EbIb),
  last_opcode(ATTR_NNN7, BX_IA_SAR_EbIb)
};

// opcode C1
static const Bit64u BxOpcodeTableC1[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN0 | ATTR_OS64, BX_IA_ROL_EqIb),
  form_opcode(ATTR_NNN1 | ATTR_OS64, BX_IA_ROR_EqIb),
  form_opcode(ATTR_NNN2 | ATTR_OS64, BX_IA_RCL_EqIb),
  form_opcode(ATTR_NNN3 | ATTR_OS64, BX_IA_RCR_EqIb),
  form_opcode(ATTR_NNN4 | ATTR_OS64, BX_IA_SHL_EqIb),
  form_opcode(ATTR_NNN5 | ATTR_OS64, BX_IA_SHR_EqIb),
  form_opcode(ATTR_NNN6 | ATTR_OS64, BX_IA_SHL_EqIb),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_SAR_EqIb),
#endif

  form_opcode(ATTR_NNN0 | ATTR_OS32, BX_IA_ROL_EdIb),
  form_opcode(ATTR_NNN1 | ATTR_OS32, BX_IA_ROR_EdIb),
  form_opcode(ATTR_NNN2 | ATTR_OS32, BX_IA_RCL_EdIb),
  form_opcode(ATTR_NNN3 | ATTR_OS32, BX_IA_RCR_EdIb),
  form_opcode(ATTR_NNN4 | ATTR_OS32, BX_IA_SHL_EdIb),
  form_opcode(ATTR_NNN5 | ATTR_OS32, BX_IA_SHR_EdIb),
  form_opcode(ATTR_NNN6 | ATTR_OS32, BX_IA_SHL_EdIb),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_SAR_EdIb),

  form_opcode(ATTR_NNN0 | ATTR_OS16, BX_IA_ROL_EwIb),
  form_opcode(ATTR_NNN1 | ATTR_OS16, BX_IA_ROR_EwIb),
  form_opcode(ATTR_NNN2 | ATTR_OS16, BX_IA_RCL_EwIb),
  form_opcode(ATTR_NNN3 | ATTR_OS16, BX_IA_RCR_EwIb),
  form_opcode(ATTR_NNN4 | ATTR_OS16, BX_IA_SHL_EwIb),
  form_opcode(ATTR_NNN5 | ATTR_OS16, BX_IA_SHR_EwIb),
  form_opcode(ATTR_NNN6 | ATTR_OS16, BX_IA_SHL_EwIb),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_SAR_EwIb)
};

// opcode C2
static const Bit64u BxOpcodeTableC2_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_RET_Op32_Iw),
  last_opcode(ATTR_OS16, BX_IA_RET_Op16_Iw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableC2_64[] = { last_opcode(0, BX_IA_RET_Op64_Iw) };
#endif

// opcode C3
static const Bit64u BxOpcodeTableC3_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_RET_Op32),
  last_opcode(ATTR_OS16, BX_IA_RET_Op16)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableC3_64[] = { last_opcode(0, BX_IA_RET_Op64) };
#endif

// opcode C4 - VEX prefix
static const Bit64u BxOpcodeTableC4_32[] = {
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_LES_GdMp),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_LES_GwMp)
};

// opcode C5 - VEX prefix
static const Bit64u BxOpcodeTableC5_32[] = {
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_LDS_GdMp),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM | ATTR_IS32, BX_IA_LDS_GwMp)
};

// opcode C6
static const Bit64u BxOpcodeTableC6[] = { last_opcode(ATTR_NNN0, BX_IA_MOV_EbIb) };

// opcode C7
static const Bit64u BxOpcodeTableC7[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN0 | ATTR_OS64, BX_IA_MOV_EqId),
#endif
  form_opcode(ATTR_NNN0 | ATTR_OS32, BX_IA_MOV_EdId),
  last_opcode(ATTR_NNN0 | ATTR_OS16, BX_IA_MOV_EwIw)
};

// opcode C8
static const Bit64u BxOpcodeTableC8_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_ENTER_Op32_IwIb),
  last_opcode(ATTR_OS16, BX_IA_ENTER_Op16_IwIb)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableC8_64[] = { last_opcode(0, BX_IA_ENTER_Op64_IwIb) };
#endif

// opcode C9
static const Bit64u BxOpcodeTableC9_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_LEAVE_Op32),
  last_opcode(ATTR_OS16, BX_IA_LEAVE_Op16)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableC9_64[] = { last_opcode(0, BX_IA_LEAVE_Op64) };
#endif

// opcode CA
static const Bit64u BxOpcodeTableCA[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_RETF_Op64_Iw),
#endif
  form_opcode(ATTR_OS32, BX_IA_RETF_Op32_Iw),
  last_opcode(ATTR_OS16, BX_IA_RETF_Op16_Iw)
};

// opcode CB
static const Bit64u BxOpcodeTableCB[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_RETF_Op64),
#endif
  form_opcode(ATTR_OS32, BX_IA_RETF_Op32),
  last_opcode(ATTR_OS16, BX_IA_RETF_Op16)
};

// opcode CC
static const Bit64u BxOpcodeTableCC[] = { last_opcode(0, BX_IA_INT3) };

// opcode CD
static const Bit64u BxOpcodeTableCD[] = { last_opcode_lockable(0, BX_IA_INT_Ib) };

// opcode CE
static const Bit64u BxOpcodeTableCE[] = { last_opcode(0, BX_IA_INTO) };

// opcode CF
static const Bit64u BxOpcodeTableCF_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_IRET_Op32),
  last_opcode(ATTR_OS16, BX_IA_IRET_Op16)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableCF_64[] = { last_opcode(0, BX_IA_IRET_Op64) };
#endif

// opcode D0
static const Bit64u BxOpcodeTableD0[] = {
  form_opcode(ATTR_NNN0, BX_IA_ROL_EbI1),
  form_opcode(ATTR_NNN1, BX_IA_ROR_EbI1),
  form_opcode(ATTR_NNN2, BX_IA_RCL_EbI1),
  form_opcode(ATTR_NNN3, BX_IA_RCR_EbI1),
  form_opcode(ATTR_NNN4, BX_IA_SHL_EbI1),
  form_opcode(ATTR_NNN5, BX_IA_SHR_EbI1),
  form_opcode(ATTR_NNN6, BX_IA_SHL_EbI1),
  last_opcode(ATTR_NNN7, BX_IA_SAR_EbI1)
};

// opcode D1
static const Bit64u BxOpcodeTableD1[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN0 | ATTR_OS64, BX_IA_ROL_EqI1),
  form_opcode(ATTR_NNN1 | ATTR_OS64, BX_IA_ROR_EqI1),
  form_opcode(ATTR_NNN2 | ATTR_OS64, BX_IA_RCL_EqI1),
  form_opcode(ATTR_NNN3 | ATTR_OS64, BX_IA_RCR_EqI1),
  form_opcode(ATTR_NNN4 | ATTR_OS64, BX_IA_SHL_EqI1),
  form_opcode(ATTR_NNN5 | ATTR_OS64, BX_IA_SHR_EqI1),
  form_opcode(ATTR_NNN6 | ATTR_OS64, BX_IA_SHL_EqI1),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_SAR_EqI1),
#endif

  form_opcode(ATTR_NNN0 | ATTR_OS32, BX_IA_ROL_EdI1),
  form_opcode(ATTR_NNN1 | ATTR_OS32, BX_IA_ROR_EdI1),
  form_opcode(ATTR_NNN2 | ATTR_OS32, BX_IA_RCL_EdI1),
  form_opcode(ATTR_NNN3 | ATTR_OS32, BX_IA_RCR_EdI1),
  form_opcode(ATTR_NNN4 | ATTR_OS32, BX_IA_SHL_EdI1),
  form_opcode(ATTR_NNN5 | ATTR_OS32, BX_IA_SHR_EdI1),
  form_opcode(ATTR_NNN6 | ATTR_OS32, BX_IA_SHL_EdI1),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_SAR_EdI1),

  form_opcode(ATTR_NNN0 | ATTR_OS16, BX_IA_ROL_EwI1),
  form_opcode(ATTR_NNN1 | ATTR_OS16, BX_IA_ROR_EwI1),
  form_opcode(ATTR_NNN2 | ATTR_OS16, BX_IA_RCL_EwI1),
  form_opcode(ATTR_NNN3 | ATTR_OS16, BX_IA_RCR_EwI1),
  form_opcode(ATTR_NNN4 | ATTR_OS16, BX_IA_SHL_EwI1),
  form_opcode(ATTR_NNN5 | ATTR_OS16, BX_IA_SHR_EwI1),
  form_opcode(ATTR_NNN6 | ATTR_OS16, BX_IA_SHL_EwI1),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_SAR_EwI1)
};

// opcode D2
static const Bit64u BxOpcodeTableD2[] = {
  form_opcode(ATTR_NNN0, BX_IA_ROL_Eb),
  form_opcode(ATTR_NNN1, BX_IA_ROR_Eb),
  form_opcode(ATTR_NNN2, BX_IA_RCL_Eb),
  form_opcode(ATTR_NNN3, BX_IA_RCR_Eb),
  form_opcode(ATTR_NNN4, BX_IA_SHL_Eb),
  form_opcode(ATTR_NNN5, BX_IA_SHR_Eb),
  form_opcode(ATTR_NNN6, BX_IA_SHL_Eb),
  last_opcode(ATTR_NNN7, BX_IA_SAR_Eb)
};

// opcode D3
static const Bit64u BxOpcodeTableD3[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN0 | ATTR_OS64, BX_IA_ROL_Eq),
  form_opcode(ATTR_NNN1 | ATTR_OS64, BX_IA_ROR_Eq),
  form_opcode(ATTR_NNN2 | ATTR_OS64, BX_IA_RCL_Eq),
  form_opcode(ATTR_NNN3 | ATTR_OS64, BX_IA_RCR_Eq),
  form_opcode(ATTR_NNN4 | ATTR_OS64, BX_IA_SHL_Eq),
  form_opcode(ATTR_NNN5 | ATTR_OS64, BX_IA_SHR_Eq),
  form_opcode(ATTR_NNN6 | ATTR_OS64, BX_IA_SHL_Eq),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_SAR_Eq),
#endif

  form_opcode(ATTR_NNN0 | ATTR_OS32, BX_IA_ROL_Ed),
  form_opcode(ATTR_NNN1 | ATTR_OS32, BX_IA_ROR_Ed),
  form_opcode(ATTR_NNN2 | ATTR_OS32, BX_IA_RCL_Ed),
  form_opcode(ATTR_NNN3 | ATTR_OS32, BX_IA_RCR_Ed),
  form_opcode(ATTR_NNN4 | ATTR_OS32, BX_IA_SHL_Ed),
  form_opcode(ATTR_NNN5 | ATTR_OS32, BX_IA_SHR_Ed),
  form_opcode(ATTR_NNN6 | ATTR_OS32, BX_IA_SHL_Ed),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_SAR_Ed),

  form_opcode(ATTR_NNN0 | ATTR_OS16, BX_IA_ROL_Ew),
  form_opcode(ATTR_NNN1 | ATTR_OS16, BX_IA_ROR_Ew),
  form_opcode(ATTR_NNN2 | ATTR_OS16, BX_IA_RCL_Ew),
  form_opcode(ATTR_NNN3 | ATTR_OS16, BX_IA_RCR_Ew),
  form_opcode(ATTR_NNN4 | ATTR_OS16, BX_IA_SHL_Ew),
  form_opcode(ATTR_NNN5 | ATTR_OS16, BX_IA_SHR_Ew),
  form_opcode(ATTR_NNN6 | ATTR_OS16, BX_IA_SHL_Ew),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_SAR_Ew)
};

// opcode D4
static const Bit64u BxOpcodeTableD4[] = { last_opcode(ATTR_IS32, BX_IA_AAM) };

// opcode D5
static const Bit64u BxOpcodeTableD5[] = { last_opcode(ATTR_IS32, BX_IA_AAD) };

// opcode D6
static const Bit64u BxOpcodeTableD6[] = { last_opcode(0, BX_IA_SALC) };

// opcode D7
static const Bit64u BxOpcodeTableD7[] = { last_opcode(0, BX_IA_XLAT) };

// opcode E0
static const Bit64u BxOpcodeTableE0_32[] = { 
  form_opcode(ATTR_IS32 | ATTR_OS32, BX_IA_LOOPNE_Jbd),
  last_opcode(ATTR_IS32 | ATTR_OS16, BX_IA_LOOPNE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE0_64[] = { last_opcode(ATTR_IS64, BX_IA_LOOPNE_Jbq) };
#endif

// opcode E1
static const Bit64u BxOpcodeTableE1_32[] = { 
  form_opcode(ATTR_IS32 | ATTR_OS32, BX_IA_LOOPE_Jbd),
  last_opcode(ATTR_IS32 | ATTR_OS16, BX_IA_LOOPE_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE1_64[] = { last_opcode(ATTR_IS64, BX_IA_LOOPE_Jbq) };
#endif

// opcode E2
static const Bit64u BxOpcodeTableE2_32[] = { 
  form_opcode(ATTR_IS32 | ATTR_OS32, BX_IA_LOOP_Jbd),
  last_opcode(ATTR_IS32 | ATTR_OS16, BX_IA_LOOP_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE2_64[] = { last_opcode(ATTR_IS64, BX_IA_LOOP_Jbq) };
#endif

// opcode E3
static const Bit64u BxOpcodeTableE3_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JECXZ_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JCXZ_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE3_64[] = { last_opcode(ATTR_IS64, BX_IA_JRCXZ_Jbq) };
#endif

// opcode E4
static const Bit64u BxOpcodeTableE4[] = { last_opcode(0, BX_IA_IN_ALIb) };

// opcode E5
static const Bit64u BxOpcodeTableE5[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_IN_EAXIb),
  last_opcode(ATTR_OS16,    BX_IA_IN_AXIb)
};

// opcode E6
static const Bit64u BxOpcodeTableE6[] = { last_opcode(0, BX_IA_OUT_IbAL) };

// opcode E7
static const Bit64u BxOpcodeTableE7[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_OUT_IbEAX),
  last_opcode(ATTR_OS16,    BX_IA_OUT_IbAX)
};

// opcode E8
static const Bit64u BxOpcodeTableE8_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_CALL_Jd),
  last_opcode(ATTR_OS16, BX_IA_CALL_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE8_64[] = { last_opcode(0, BX_IA_CALL_Jq) };
#endif

// opcode E9
static const Bit64u BxOpcodeTableE9_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JMP_Jd),
  last_opcode(ATTR_OS16, BX_IA_JMP_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableE9_64[] = { last_opcode(0, BX_IA_JMP_Jq) };
#endif

// opcode EA
static const Bit64u BxOpcodeTableEA_32[] = { last_opcode(0, BX_IA_JMPF_Ap) };

// opcode EB
static const Bit64u BxOpcodeTableEB_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JMP_Jbd),
  last_opcode(ATTR_OS16, BX_IA_JMP_Jbw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTableEB_64[] = { last_opcode(0, BX_IA_JMP_Jbq) };
#endif

// opcode EC
static const Bit64u BxOpcodeTableEC[] = { last_opcode(0, BX_IA_IN_ALDX) };

// opcode ED
static const Bit64u BxOpcodeTableED[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_IN_EAXDX),
  last_opcode(ATTR_OS16,    BX_IA_IN_AXDX)
};

// opcode EE
static const Bit64u BxOpcodeTableEE[] = { last_opcode(0, BX_IA_OUT_DXAL) };

// opcode EF
static const Bit64u BxOpcodeTableEF[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_OUT_DXEAX),
  last_opcode(ATTR_OS16,    BX_IA_OUT_DXAX)
};

// opcode F1
static const Bit64u BxOpcodeTableF1[] = { last_opcode(0, BX_IA_INT1) };

// opcode F4
static const Bit64u BxOpcodeTableF4[] = { last_opcode(0, BX_IA_HLT) };

// opcode F5
static const Bit64u BxOpcodeTableF5[] = { last_opcode(0, BX_IA_CMC) };

// opcode F6
static const Bit64u BxOpcodeTableF6[] = {
  form_opcode(ATTR_NNN0, BX_IA_TEST_EbIb),
  form_opcode(ATTR_NNN1, BX_IA_TEST_EbIb),
  form_opcode_lockable(ATTR_NNN2, BX_IA_NOT_Eb),
  form_opcode_lockable(ATTR_NNN3, BX_IA_NEG_Eb),
  form_opcode(ATTR_NNN4, BX_IA_MUL_ALEb),
  form_opcode(ATTR_NNN5, BX_IA_IMUL_ALEb),
  form_opcode(ATTR_NNN6, BX_IA_DIV_ALEb),
  last_opcode(ATTR_NNN7, BX_IA_IDIV_ALEb)
};

// opcode F7
static const Bit64u BxOpcodeTableF7[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN0 | ATTR_OS64, BX_IA_TEST_EqId),
  form_opcode(ATTR_NNN1 | ATTR_OS64, BX_IA_TEST_EqId),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS64, BX_IA_NOT_Eq),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS64, BX_IA_NEG_Eq),
  form_opcode(ATTR_NNN4 | ATTR_OS64, BX_IA_MUL_RAXEq),
  form_opcode(ATTR_NNN5 | ATTR_OS64, BX_IA_IMUL_RAXEq),
  form_opcode(ATTR_NNN6 | ATTR_OS64, BX_IA_DIV_RAXEq),
  form_opcode(ATTR_NNN7 | ATTR_OS64, BX_IA_IDIV_RAXEq),
#endif

  form_opcode(ATTR_NNN0 | ATTR_OS32, BX_IA_TEST_EdId),
  form_opcode(ATTR_NNN1 | ATTR_OS32, BX_IA_TEST_EdId),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS32, BX_IA_NOT_Ed),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS32, BX_IA_NEG_Ed),
  form_opcode(ATTR_NNN4 | ATTR_OS32, BX_IA_MUL_EAXEd),
  form_opcode(ATTR_NNN5 | ATTR_OS32, BX_IA_IMUL_EAXEd),
  form_opcode(ATTR_NNN6 | ATTR_OS32, BX_IA_DIV_EAXEd),
  form_opcode(ATTR_NNN7 | ATTR_OS32, BX_IA_IDIV_EAXEd),

  form_opcode(ATTR_NNN0 | ATTR_OS16, BX_IA_TEST_EwIw),
  form_opcode(ATTR_NNN1 | ATTR_OS16, BX_IA_TEST_EwIw),
  form_opcode_lockable(ATTR_NNN2 | ATTR_OS16, BX_IA_NOT_Ew),
  form_opcode_lockable(ATTR_NNN3 | ATTR_OS16, BX_IA_NEG_Ew),
  form_opcode(ATTR_NNN4 | ATTR_OS16, BX_IA_MUL_AXEw),
  form_opcode(ATTR_NNN5 | ATTR_OS16, BX_IA_IMUL_AXEw),
  form_opcode(ATTR_NNN6 | ATTR_OS16, BX_IA_DIV_AXEw),
  last_opcode(ATTR_NNN7 | ATTR_OS16, BX_IA_IDIV_AXEw)
};

// opcode F8
static const Bit64u BxOpcodeTableF8[] = { last_opcode(0, BX_IA_CLC) };

// opcode F9
static const Bit64u BxOpcodeTableF9[] = { last_opcode(0, BX_IA_STC) };

// opcode FA
static const Bit64u BxOpcodeTableFA[] = { last_opcode(0, BX_IA_CLI) };

// opcode FB
static const Bit64u BxOpcodeTableFB[] = { last_opcode(0, BX_IA_STI) };

// opcode FC
static const Bit64u BxOpcodeTableFC[] = { last_opcode(0, BX_IA_CLD) };

// opcode FD
static const Bit64u BxOpcodeTableFD[] = { last_opcode(0, BX_IA_STD) };

// opcode FE
static const Bit64u BxOpcodeTableFE[] = {
  form_opcode_lockable(ATTR_NNN0, BX_IA_INC_Eb),
  last_opcode_lockable(ATTR_NNN1, BX_IA_DEC_Eb)
};

// opcode FF
static const Bit64u BxOpcodeTableFF[] = {
  //0
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_NNN0 | ATTR_OS64, BX_IA_INC_Eq),
#endif
  form_opcode_lockable(ATTR_NNN0 | ATTR_OS32, BX_IA_INC_Ed),
  form_opcode_lockable(ATTR_NNN0 | ATTR_OS16, BX_IA_INC_Ew),

  //1
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS64, BX_IA_DEC_Eq),
#endif
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS32, BX_IA_DEC_Ed),
  form_opcode_lockable(ATTR_NNN1 | ATTR_OS16, BX_IA_DEC_Ew),

  //2
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN2 | ATTR_IS64, BX_IA_CALL_Eq), // regardless of Osize
#endif
  form_opcode(ATTR_NNN2 | ATTR_IS32 | ATTR_OS16, BX_IA_CALL_Ew),
  form_opcode(ATTR_NNN2 | ATTR_IS32 | ATTR_OS32, BX_IA_CALL_Ed),

  //3
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN3 | ATTR_OS64 | ATTR_MOD_MEM, BX_IA_CALLF_Op64_Ep),
#endif
  form_opcode(ATTR_NNN3 | ATTR_OS32 | ATTR_MOD_MEM, BX_IA_CALLF_Op32_Ep),
  form_opcode(ATTR_NNN3 | ATTR_OS16 | ATTR_MOD_MEM, BX_IA_CALLF_Op16_Ep),

  //4
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN4 | ATTR_IS64, BX_IA_JMP_Eq), // regardless of Osize
#endif
  form_opcode(ATTR_NNN4 | ATTR_IS32 | ATTR_OS16, BX_IA_JMP_Ew),
  form_opcode(ATTR_NNN4 | ATTR_IS32 | ATTR_OS32, BX_IA_JMP_Ed),

  //5
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN5 | ATTR_OS64 | ATTR_MOD_MEM, BX_IA_JMPF_Op64_Ep),
#endif
  form_opcode(ATTR_NNN5 | ATTR_OS32 | ATTR_MOD_MEM, BX_IA_JMPF_Op32_Ep),
  form_opcode(ATTR_NNN5 | ATTR_OS16 | ATTR_MOD_MEM, BX_IA_JMPF_Op16_Ep),

  //6
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN6 | ATTR_IS64 | ATTR_OS32_64, BX_IA_PUSH_Eq),
#endif
  form_opcode(ATTR_NNN6 | ATTR_OS32, BX_IA_PUSH_Ed),
  last_opcode(ATTR_NNN6 | ATTR_OS16, BX_IA_PUSH_Ew)
};

// opcode 0F 00
static const Bit64u BxOpcodeTable0F00[] = {
  form_opcode(ATTR_NNN0, BX_IA_SLDT_Ew),
  form_opcode(ATTR_NNN1, BX_IA_STR_Ew),
  form_opcode(ATTR_NNN2, BX_IA_LLDT_Ew),
  form_opcode(ATTR_NNN3, BX_IA_LTR_Ew),
  form_opcode(ATTR_NNN4, BX_IA_VERR_Ew),
  last_opcode(ATTR_NNN5, BX_IA_VERW_Ew)
};

// opcode 0F 01
static const Bit64u BxOpcodeTable0F01[] = {
  form_opcode(ATTR_IS32 | ATTR_MOD_MEM | ATTR_NNN0, BX_IA_SGDT_Ms),
  form_opcode(ATTR_IS32 | ATTR_MOD_MEM | ATTR_NNN1, BX_IA_SIDT_Ms),
  form_opcode(ATTR_IS32 | ATTR_MOD_MEM | ATTR_NNN2, BX_IA_LGDT_Ms),
  form_opcode(ATTR_IS32 | ATTR_MOD_MEM | ATTR_NNN3, BX_IA_LIDT_Ms),
  
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_MOD_MEM | ATTR_NNN0, BX_IA_SGDT_Op64_Ms),
  form_opcode(ATTR_IS64 | ATTR_MOD_MEM | ATTR_NNN1, BX_IA_SIDT_Op64_Ms),
  form_opcode(ATTR_IS64 | ATTR_MOD_MEM | ATTR_NNN2, BX_IA_LGDT_Op64_Ms),
  form_opcode(ATTR_IS64 | ATTR_MOD_MEM | ATTR_NNN3, BX_IA_LIDT_Op64_Ms),
#endif
  
  form_opcode(ATTR_NNN4, BX_IA_SMSW_Ew),
  form_opcode(ATTR_NNN6, BX_IA_LMSW_Ew),
  
  form_opcode(ATTR_NNN7 | ATTR_MOD_MEM, BX_IA_INVLPG),

  form_opcode(ATTR_NNN0 | ATTR_RRR1 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_VMCALL),
  form_opcode(ATTR_NNN0 | ATTR_RRR2 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_VMLAUNCH),
  form_opcode(ATTR_NNN0 | ATTR_RRR3 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_VMRESUME),
  form_opcode(ATTR_NNN0 | ATTR_RRR4 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_VMXOFF),
  
  form_opcode(ATTR_NNN1 | ATTR_RRR0 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_MONITOR),
  form_opcode(ATTR_NNN1 | ATTR_RRR1 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_MWAIT),
  form_opcode(ATTR_NNN1 | ATTR_RRR2 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_CLAC),
  form_opcode(ATTR_NNN1 | ATTR_RRR3 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_STAC),

  form_opcode(ATTR_NNN2 | ATTR_RRR0 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_XGETBV),
  form_opcode(ATTR_NNN2 | ATTR_RRR1 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_XSETBV),
  form_opcode(ATTR_NNN2 | ATTR_RRR4 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_VMFUNC),
  
  form_opcode(ATTR_NNN3 | ATTR_RRR0 | ATTR_MODC0, BX_IA_VMRUN),
  form_opcode(ATTR_NNN3 | ATTR_RRR1 | ATTR_MODC0, BX_IA_VMMCALL),
  form_opcode(ATTR_NNN3 | ATTR_RRR2 | ATTR_MODC0, BX_IA_VMLOAD),
  form_opcode(ATTR_NNN3 | ATTR_RRR3 | ATTR_MODC0, BX_IA_VMSAVE),
  form_opcode(ATTR_NNN3 | ATTR_RRR4 | ATTR_MODC0, BX_IA_STGI),
  form_opcode(ATTR_NNN3 | ATTR_RRR5 | ATTR_MODC0, BX_IA_CLGI),
  form_opcode(ATTR_NNN3 | ATTR_RRR6 | ATTR_MODC0, BX_IA_SKINIT),
  form_opcode(ATTR_NNN3 | ATTR_RRR7 | ATTR_MODC0, BX_IA_INVLPGA),

#if BX_SUPPORT_CET
  form_opcode(ATTR_NNN5 | ATTR_RRR0 | ATTR_MODC0   | ATTR_SSE_PREFIX_F3, BX_IA_SETSSBSY),
  form_opcode(ATTR_NNN5 | ATTR_RRR2 | ATTR_MODC0   | ATTR_SSE_PREFIX_F3, BX_IA_SAVEPREVSSP),
  form_opcode(ATTR_NNN5 |             ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_RSTORSSP),
#endif
  
#if BX_SUPPORT_PKEYS
  form_opcode(ATTR_NNN5 | ATTR_RRR6 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_RDPKRU),
  form_opcode(ATTR_NNN5 | ATTR_RRR7 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_WRPKRU),
#endif

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN7 | ATTR_RRR0 | ATTR_MODC0 | ATTR_IS64, BX_IA_SWAPGS),
#endif
  form_opcode(ATTR_NNN7 | ATTR_RRR1 | ATTR_MODC0, BX_IA_RDTSCP),
  form_opcode(ATTR_NNN7 | ATTR_RRR2 | ATTR_MODC0, BX_IA_MONITORX),
  form_opcode(ATTR_NNN7 | ATTR_RRR3 | ATTR_MODC0, BX_IA_MWAITX),
  last_opcode(ATTR_NNN7 | ATTR_RRR4 | ATTR_MODC0, BX_IA_CLZERO)
};

// opcode 0F 02
static const Bit64u BxOpcodeTable0F02[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_LAR_GdEw),
  last_opcode(ATTR_OS16,    BX_IA_LAR_GwEw)
};

// opcode 0F 03
static const Bit64u BxOpcodeTable0F03[] = { 
  form_opcode(ATTR_OS32_64, BX_IA_LSL_GdEw),
  last_opcode(ATTR_OS16,    BX_IA_LSL_GwEw)
};

// opcode 0F 05
static const Bit64u BxOpcodeTable0F05_32[] = { last_opcode(0, BX_IA_SYSCALL_LEGACY) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F05_64[] = { last_opcode(0, BX_IA_SYSCALL) };
#endif

// opcode 0F 06
static const Bit64u BxOpcodeTable0F06[] = { last_opcode(0, BX_IA_CLTS) };

// opcode 0F 07
static const Bit64u BxOpcodeTable0F07_32[] = { last_opcode(0, BX_IA_SYSRET_LEGACY) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F07_64[] = { last_opcode(0, BX_IA_SYSRET) };
#endif

// opcode 0F 08
static const Bit64u BxOpcodeTable0F08[] = { last_opcode(0, BX_IA_INVD) };

// opcode 0F 09
static const Bit64u BxOpcodeTable0F09[] = { last_opcode(0, BX_IA_WBINVD) };

// opcode 0F 0B
static const Bit64u BxOpcodeTable0F0B[] = { last_opcode(0, BX_IA_UD2) };

// opcode 0F 0D - 3DNow! PREFETCHW on AMD, NOP on older Intel CPUs
static const Bit64u BxOpcodeTable0F0D[] = { last_opcode(0, BX_IA_PREFETCHW_Mb) };

// opcode 0F 0E - 3DNow! FEMMS
static const Bit64u BxOpcodeTable0F0E[] = { last_opcode(0, BX_IA_FEMMS) };

// opcode 0F 0F - 3DNow! Opcode Table

// opcode 0F 10
static const Bit64u BxOpcodeTable0F10[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVUPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVUPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MOVSD_VsdWsd)
};

// opcode 0F 11
static const Bit64u BxOpcodeTable0F11[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVUPS_WpsVps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVUPD_WpdVpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVSS_WssVss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MOVSD_WsdVsd)
};

// opcode 0F 12
static const Bit64u BxOpcodeTable0F12[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVLPS_VpsMq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_MOVHLPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVLPD_VsdMq),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVSLDUP_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MOVDDUP_VpdWq)
};

// opcode 0F 13
static const Bit64u BxOpcodeTable0F13[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVLPS_MqVps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVLPD_MqVsd)
};

// opcode 0F 14
static const Bit64u BxOpcodeTable0F14[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_UNPCKLPS_VpsWdq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_UNPCKLPD_VpdWdq)
};

// opcode 0F 15
static const Bit64u BxOpcodeTable0F15[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_UNPCKHPS_VpsWdq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_UNPCKHPD_VpdWdq)
};

// opcode 0F 16
static const Bit64u BxOpcodeTable0F16[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVHPS_VpsMq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_MOVLHPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVHPD_VsdMq),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVSHDUP_VpsWps)
};

// opcode 0F 17
static const Bit64u BxOpcodeTable0F17[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVHPS_MqVps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVHPD_MqVsd)
};

// opcode 0F 18 - opcode group G16, PREFETCH hints
#if BX_CPU_LEVEL >= 6
static const Bit64u BxOpcodeTable0F18[] = {
  form_opcode(ATTR_NNN0, BX_IA_PREFETCHNTA_Mb),
  form_opcode(ATTR_NNN1, BX_IA_PREFETCHT0_Mb),
  form_opcode(ATTR_NNN2, BX_IA_PREFETCHT1_Mb),
  form_opcode(ATTR_NNN3, BX_IA_PREFETCHT2_Mb),
  last_opcode(0,         BX_IA_PREFETCH_Mb)
};
#endif

// opcode 0F 1E
#if BX_SUPPORT_CET
static const Bit64u BxOpcodeTable0F1E[] = {
  form_opcode(ATTR_OS16_32 | ATTR_NNN1 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_RDSSPD),
  form_opcode(ATTR_OS64    | ATTR_NNN1 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_RDSSPQ),

  form_opcode(ATTR_NNN7 | ATTR_RRR2 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_ENDBRANCH64),
  form_opcode(ATTR_NNN7 | ATTR_RRR3 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_ENDBRANCH32),

  last_opcode(0, BX_IA_NOP) // multi byte-NOP
};
#endif

// opcode 0F 19 - 0F 1F: multi-byte NOP
static const Bit64u BxOpcodeTableMultiByteNOP[] = { last_opcode(0, BX_IA_NOP) };

// opcode 0F 20
static const Bit64u BxOpcodeTable0F20_32[] = {
  form_opcode(ATTR_NNN0, BX_IA_MOV_RdCR0),
  form_opcode(ATTR_NNN2, BX_IA_MOV_RdCR2),
  form_opcode(ATTR_NNN3, BX_IA_MOV_RdCR3),
  last_opcode(ATTR_NNN4, BX_IA_MOV_RdCR4)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F20_64[] = {
  form_opcode(ATTR_NNN0, BX_IA_MOV_RqCR0),
  form_opcode(ATTR_NNN2, BX_IA_MOV_RqCR2),
  form_opcode(ATTR_NNN3, BX_IA_MOV_RqCR3),
  last_opcode(ATTR_NNN4, BX_IA_MOV_RqCR4)
};
#endif

// opcode 0F 21
static const Bit64u BxOpcodeTable0F21_32[] = { last_opcode(0, BX_IA_MOV_RdDd) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F21_64[] = { last_opcode(0, BX_IA_MOV_RqDq) };
#endif

// opcode 0F 22
static const Bit64u BxOpcodeTable0F22_32[] = {
  form_opcode(ATTR_NNN0, BX_IA_MOV_CR0Rd),
  form_opcode(ATTR_NNN2, BX_IA_MOV_CR2Rd),
  form_opcode(ATTR_NNN3, BX_IA_MOV_CR3Rd),
  last_opcode(ATTR_NNN4, BX_IA_MOV_CR4Rd)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F22_64[] = {
  form_opcode(ATTR_NNN0, BX_IA_MOV_CR0Rq),
  form_opcode(ATTR_NNN2, BX_IA_MOV_CR2Rq),
  form_opcode(ATTR_NNN3, BX_IA_MOV_CR3Rq),
  last_opcode(ATTR_NNN4, BX_IA_MOV_CR4Rq)
};
#endif

// opcode 0F 23
static const Bit64u BxOpcodeTable0F23_32[] = { last_opcode(0, BX_IA_MOV_DdRd) };
#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F23_64[] = { last_opcode(0, BX_IA_MOV_DqRq) };
#endif

// opcode 0F 24
static const Bit64u BxOpcodeTable0F24[] = { last_opcode(0, BX_IA_ERROR) }; // BX_IA_MOV_RdTd not implemented
// opcode 0F 26
static const Bit64u BxOpcodeTable0F26[] = { last_opcode(0, BX_IA_ERROR) }; // BX_IA_MOV_TdRd not implemented

// opcode 0F 28
static const Bit64u BxOpcodeTable0F28[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVAPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVAPD_VpdWpd)
};

// opcode 0F 29
static const Bit64u BxOpcodeTable0F29[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVAPS_WpsVps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVAPD_WpdVpd)
};

// opcode 0F 2A
static const Bit64u BxOpcodeTable0F2A[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CVTPI2PS_VpsQq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTPI2PD_VpdQq),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_OS64, BX_IA_CVTSI2SS_VssEq),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS64, BX_IA_CVTSI2SD_VsdEq),
#endif

  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTSI2SS_VssEd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CVTSI2SD_VsdEd)
};

// opcode 0F 2B
static const Bit64u BxOpcodeTable0F2B[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVNTPS_MpsVps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVNTPD_MpdVpd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MOD_MEM, BX_IA_MOVNTSS_MssVss),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MOD_MEM, BX_IA_MOVNTSD_MsdVsd)
};

// opcode 0F 2C
static const Bit64u BxOpcodeTable0F2C[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CVTTPS2PI_PqWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTTPD2PI_PqWpd),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_OS64, BX_IA_CVTTSS2SI_GqWss),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS64, BX_IA_CVTTSD2SI_GqWsd),
#endif

  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTTSS2SI_GdWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CVTTSD2SI_GdWsd)
};

// opcode 0F 2D
static const Bit64u BxOpcodeTable0F2D[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CVTPS2PI_PqWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTPD2PI_PqWpd),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_OS64, BX_IA_CVTSS2SI_GqWss),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS64, BX_IA_CVTSD2SI_GqWsd),
#endif

  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTSS2SI_GdWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CVTSD2SI_GdWsd)
};

// opcode 0F 2E
static const Bit64u BxOpcodeTable0F2E[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_UCOMISS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_UCOMISD_VsdWsd)
};

// opcode 0F 2F
static const Bit64u BxOpcodeTable0F2F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_COMISS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_COMISD_VsdWsd)
};

// opcode 0F 30
static const Bit64u BxOpcodeTable0F30[] = { last_opcode(0, BX_IA_WRMSR) };

// opcode 0F 31 - end trace to avoid multiple TSC samples in one cycle
static const Bit64u BxOpcodeTable0F31[] = { last_opcode(0, BX_IA_RDTSC) };

// opcode 0F 32 - end trace to avoid multiple TSC samples in one cycle
static const Bit64u BxOpcodeTable0F32[] = { last_opcode(0, BX_IA_RDMSR) };

// opcode 0F 33
static const Bit64u BxOpcodeTable0F33[] = { last_opcode(0, BX_IA_RDPMC) };

// opcode 0F 34
static const Bit64u BxOpcodeTable0F34[] = { last_opcode(0, BX_IA_SYSENTER) };

// opcode 0F 35
static const Bit64u BxOpcodeTable0F35[] = { last_opcode(0, BX_IA_SYSEXIT) };

// opcode 0F 37
static const Bit64u BxOpcodeTable0F37[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_GETSEC) };

// opcode 0F 40
static const Bit64u BxOpcodeTable0F40[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVO_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVO_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVO_GwEw)
};

// opcode 0F 41
static const Bit64u BxOpcodeTable0F41[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNO_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNO_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNO_GwEw)
};

// opcode 0F 42
static const Bit64u BxOpcodeTable0F42[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVB_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVB_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVB_GwEw)
};

// opcode 0F 43
static const Bit64u BxOpcodeTable0F43[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNB_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNB_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNB_GwEw)
};

// opcode 0F 44
static const Bit64u BxOpcodeTable0F44[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVZ_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVZ_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVZ_GwEw)
};

// opcode 0F 45
static const Bit64u BxOpcodeTable0F45[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNZ_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNZ_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNZ_GwEw)
};

// opcode 0F 46
static const Bit64u BxOpcodeTable0F46[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVBE_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVBE_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVBE_GwEw)
};

// opcode 0F 47
static const Bit64u BxOpcodeTable0F47[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNBE_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNBE_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNBE_GwEw)
};

// opcode 0F 48
static const Bit64u BxOpcodeTable0F48[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVS_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVS_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVS_GwEw)
};

// opcode 0F 49
static const Bit64u BxOpcodeTable0F49[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNS_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNS_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNS_GwEw)
};

// opcode 0F 4A
static const Bit64u BxOpcodeTable0F4A[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVP_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVP_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVP_GwEw)
};

// opcode 0F 4B
static const Bit64u BxOpcodeTable0F4B[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNP_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNP_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNP_GwEw)
};

// opcode 0F 4C
static const Bit64u BxOpcodeTable0F4C[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVL_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVL_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVL_GwEw)
};

// opcode 0F 4D
static const Bit64u BxOpcodeTable0F4D[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNL_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNL_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNL_GwEw)
};

// opcode 0F 4E
static const Bit64u BxOpcodeTable0F4E[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVLE_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVLE_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVLE_GwEw)
};

// opcode 0F 4F
static const Bit64u BxOpcodeTable0F4F[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_CMOVNLE_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_CMOVNLE_GdEd),
  last_opcode(ATTR_OS16, BX_IA_CMOVNLE_GwEw)
};

// opcode 0F 50
static const Bit64u BxOpcodeTable0F50[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_MOVMSKPS_GdUps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_MOVMSKPD_GdUpd)
};

// opcode 0F 51
static const Bit64u BxOpcodeTable0F51[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SQRTPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_SQRTPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_SQRTSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_SQRTSD_VsdWsd)
};

// opcode 0F 52
static const Bit64u BxOpcodeTable0F52[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_RSQRTPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_RSQRTSS_VssWss)
};

// opcode 0F 53
static const Bit64u BxOpcodeTable0F53[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_RCPPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_RCPSS_VssWss)
};

// opcode 0F 54
static const Bit64u BxOpcodeTable0F54[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_ANDPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ANDPD_VpdWpd)
};

// opcode 0F 55
static const Bit64u BxOpcodeTable0F55[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_ANDNPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ANDNPD_VpdWpd)
};

// opcode 0F 56
static const Bit64u BxOpcodeTable0F56[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_ORPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ORPD_VpdWpd)
};

// opcode 0F 57
static const Bit64u BxOpcodeTable0F57[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_XORPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_XORPD_VpdWpd)
};

// opcode 0F 58
static const Bit64u BxOpcodeTable0F58[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_ADDPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_ADDPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_ADDSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_ADDSD_VsdWsd)
};

// opcode 0F 59
static const Bit64u BxOpcodeTable0F59[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MULPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MULPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MULSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MULSD_VsdWsd)
};

// opcode 0F 5A
static const Bit64u BxOpcodeTable0F5A[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CVTPS2PD_VpdWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTPD2PS_VpsWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTSS2SD_VsdWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CVTSD2SS_VssWsd)
};

// opcode 0F 5B
static const Bit64u BxOpcodeTable0F5B[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CVTDQ2PS_VpsWdq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTPS2DQ_VdqWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTTPS2DQ_VdqWps)
};

// opcode 0F 5C
static const Bit64u BxOpcodeTable0F5C[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SUBPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_SUBPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_SUBSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_SUBSD_VsdWsd)
};

// opcode 0F 5D
static const Bit64u BxOpcodeTable0F5D[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MINPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MINPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MINSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MINSD_VsdWsd)
};

// opcode 0F 5E
static const Bit64u BxOpcodeTable0F5E[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_DIVPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_DIVPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_DIVSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_DIVSD_VsdWsd)
};

// opcode 0F 5F
static const Bit64u BxOpcodeTable0F5F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MAXPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MAXPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MAXSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_MAXSD_VsdWsd)
};

// opcode 0F 60
static const Bit64u BxOpcodeTable0F60[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKLBW_PqQd),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKLBW_VdqWdq)
};

// opcode 0F 61
static const Bit64u BxOpcodeTable0F61[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKLWD_PqQd),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKLWD_VdqWdq)
};

// opcode 0F 62
static const Bit64u BxOpcodeTable0F62[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKLDQ_PqQd),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKLDQ_VdqWdq)
};

// opcode 0F 63
static const Bit64u BxOpcodeTable0F63[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PACKSSWB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PACKSSWB_VdqWdq)
};

// opcode 0F 64
static const Bit64u BxOpcodeTable0F64[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPGTB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPGTB_VdqWdq)
};

// opcode 0F 65
static const Bit64u BxOpcodeTable0F65[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPGTW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPGTW_VdqWdq)
};

// opcode 0F 66
static const Bit64u BxOpcodeTable0F66[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPGTD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPGTD_VdqWdq)
};

// opcode 0F 67
static const Bit64u BxOpcodeTable0F67[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PACKUSWB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PACKUSWB_VdqWdq)
};

// opcode 0F 68
static const Bit64u BxOpcodeTable0F68[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKHBW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKHBW_VdqWdq)
};

// opcode 0F 69
static const Bit64u BxOpcodeTable0F69[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKHWD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKHWD_VdqWdq)
};

// opcode 0F 6A
static const Bit64u BxOpcodeTable0F6A[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PUNPCKHDQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKHDQ_VdqWdq)
};

// opcode 0F 6B
static const Bit64u BxOpcodeTable0F6B[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PACKSSDW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PACKSSDW_VdqWdq)
};

// opcode 0F 6C - 0F 6D
static const Bit64u BxOpcodeTable0F6C[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKLQDQ_VdqWdq) };
static const Bit64u BxOpcodeTable0F6D[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PUNPCKHQDQ_VdqWdq) };

// opcode 0F 6E
static const Bit64u BxOpcodeTable0F6E[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_OS64, BX_IA_MOVQ_PqEq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_OS64, BX_IA_MOVQ_VdqEq),
#endif
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVD_PqEd),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVD_VdqEd)
};

// opcode 0F 6F
static const Bit64u BxOpcodeTable0F6F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVQ_PqQq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVDQA_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVDQU_VdqWdq)
};

// opcode 0F 70
static const Bit64u BxOpcodeTable0F70[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSHUFW_PqQqIb),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSHUFD_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_PSHUFHW_VdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_PSHUFLW_VdqWdqIb),
};

// opcode 0F 71
static const Bit64u BxOpcodeTable0F71[] = {
  form_opcode(ATTR_NNN2 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSRLW_NqIb),
  form_opcode(ATTR_NNN2 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRLW_UdqIb),

  form_opcode(ATTR_NNN4 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSRAW_NqIb),
  form_opcode(ATTR_NNN4 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRAW_UdqIb),

  form_opcode(ATTR_NNN6 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSLLW_NqIb),
  last_opcode(ATTR_NNN6 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSLLW_UdqIb)
};

// opcode 0F 72
static const Bit64u BxOpcodeTable0F72[] = {
  form_opcode(ATTR_NNN2 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSRLD_NqIb),
  form_opcode(ATTR_NNN2 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRLD_UdqIb),

  form_opcode(ATTR_NNN4 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSRAD_NqIb),
  form_opcode(ATTR_NNN4 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRAD_UdqIb),

  form_opcode(ATTR_NNN6 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSLLD_NqIb),
  last_opcode(ATTR_NNN6 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSLLD_UdqIb)
};

// opcode 0F 73
static const Bit64u BxOpcodeTable0F73[] = {
  form_opcode(ATTR_NNN2 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSRLQ_NqIb),
  form_opcode(ATTR_NNN2 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRLQ_UdqIb),

  form_opcode(ATTR_NNN3 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSRLDQ_UdqIb),

  form_opcode(ATTR_NNN6 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PSLLQ_NqIb),
  form_opcode(ATTR_NNN6 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSLLQ_UdqIb),

  last_opcode(ATTR_NNN7 | ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PSLLDQ_UdqIb)
};

// opcode 0F 74
static const Bit64u BxOpcodeTable0F74[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPEQB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPEQB_VdqWdq)
};

// opcode 0F 75
static const Bit64u BxOpcodeTable0F75[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPEQW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPEQW_VdqWdq)
};

// opcode 0F 76
static const Bit64u BxOpcodeTable0F76[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PCMPEQD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPEQD_VdqWdq)
};

// opcode 0F 77
static const Bit64u BxOpcodeTable0F77[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_EMMS) };

// opcode 0F 78
static const Bit64u BxOpcodeTable0F78[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_IS32, BX_IA_VMREAD_EdGd),
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_IS64, BX_IA_VMREAD_EqGq),
#endif
  // SSE4A by AMD
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0 | ATTR_NNN0, BX_IA_EXTRQ_UdqIbIb),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MODC0,             BX_IA_INSERTQ_VdqUqIbIb)
};

// opcode 0F 79
static const Bit64u BxOpcodeTable0F79[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_IS32, BX_IA_VMWRITE_GdEd),
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_IS64, BX_IA_VMWRITE_GqEq),
#endif
  // SSE4A by AMD
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_EXTRQ_VdqUq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MODC0, BX_IA_INSERTQ_VdqUdq)
};

// opcode 0F 7C
static const Bit64u BxOpcodeTable0F7C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_HADDPD_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_HADDPS_VpsWps)
};

// opcode 0F 7D
static const Bit64u BxOpcodeTable0F7D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_HSUBPD_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_HSUBPS_VpsWps)
};

// opcode 0F 7E
static const Bit64u BxOpcodeTable0F7E[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_OS64, BX_IA_MOVQ_EqPq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_OS64, BX_IA_MOVQ_EqVq),
#endif
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVD_EdPq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVD_EdVd),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVQ_VqWq)
};

// opcode 0F 7F
static const Bit64u BxOpcodeTable0F7F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_MOVQ_QqPq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVDQA_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_MOVDQU_WdqVdq)
};

// opcode 0F 80
static const Bit64u BxOpcodeTable0F80_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JO_Jd),
  last_opcode(ATTR_OS16, BX_IA_JO_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F80_64[] = { last_opcode(0, BX_IA_JO_Jq) };
#endif

// opcode 0F 81
static const Bit64u BxOpcodeTable0F81_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNO_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNO_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F81_64[] = { last_opcode(0, BX_IA_JNO_Jq) };
#endif

// opcode 0F 82
static const Bit64u BxOpcodeTable0F82_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JB_Jd),
  last_opcode(ATTR_OS16, BX_IA_JB_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F82_64[] = { last_opcode(0, BX_IA_JB_Jq) };
#endif

// opcode 0F 83
static const Bit64u BxOpcodeTable0F83_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNB_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNB_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F83_64[] = { last_opcode(0, BX_IA_JNB_Jq) };
#endif

// opcode 0F 84
static const Bit64u BxOpcodeTable0F84_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JZ_Jd),
  last_opcode(ATTR_OS16, BX_IA_JZ_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F84_64[] = { last_opcode(0, BX_IA_JZ_Jq) };
#endif

// opcode 0F 85
static const Bit64u BxOpcodeTable0F85_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNZ_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNZ_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F85_64[] = { last_opcode(0, BX_IA_JNZ_Jq) };
#endif

// opcode 0F 86
static const Bit64u BxOpcodeTable0F86_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JBE_Jd),
  last_opcode(ATTR_OS16, BX_IA_JBE_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F86_64[] = { last_opcode(0, BX_IA_JBE_Jq) };
#endif

// opcode 0F 87
static const Bit64u BxOpcodeTable0F87_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNBE_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNBE_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F87_64[] = { last_opcode(0, BX_IA_JNBE_Jq) };
#endif

// opcode 0F 88
static const Bit64u BxOpcodeTable0F88_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JS_Jd),
  last_opcode(ATTR_OS16, BX_IA_JS_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F88_64[] = { last_opcode(0, BX_IA_JS_Jq) };
#endif

// opcode 0F 89
static const Bit64u BxOpcodeTable0F89_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNS_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNS_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F89_64[] = { last_opcode(0, BX_IA_JNS_Jq) };
#endif

// opcode 0F 8A
static const Bit64u BxOpcodeTable0F8A_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JP_Jd),
  last_opcode(ATTR_OS16, BX_IA_JP_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8A_64[] = { last_opcode(0, BX_IA_JP_Jq) };
#endif

// opcode 0F 8B
static const Bit64u BxOpcodeTable0F8B_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNP_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNP_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8B_64[] = { last_opcode(0, BX_IA_JNP_Jq) };
#endif

// opcode 0F 8C
static const Bit64u BxOpcodeTable0F8C_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JL_Jd),
  last_opcode(ATTR_OS16, BX_IA_JL_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8C_64[] = { last_opcode(0, BX_IA_JL_Jq) };
#endif

// opcode 0F 8D
static const Bit64u BxOpcodeTable0F8D_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNL_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNL_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8D_64[] = { last_opcode(0, BX_IA_JNL_Jq) };
#endif

// opcode 0F 8E
static const Bit64u BxOpcodeTable0F8E_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JLE_Jd),
  last_opcode(ATTR_OS16, BX_IA_JLE_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8E_64[] = { last_opcode(0, BX_IA_JLE_Jq) };
#endif

// opcode 0F 8F
static const Bit64u BxOpcodeTable0F8F_32[] = { 
  form_opcode(ATTR_OS32, BX_IA_JNLE_Jd),
  last_opcode(ATTR_OS16, BX_IA_JNLE_Jw)
};

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F8F_64[] = { last_opcode(0, BX_IA_JNLE_Jq) };
#endif

// opcode 0F 90 - 0F 9F
static const Bit64u BxOpcodeTable0F90[] = { last_opcode(0, BX_IA_SETO_Eb) };
static const Bit64u BxOpcodeTable0F91[] = { last_opcode(0, BX_IA_SETNO_Eb) };
static const Bit64u BxOpcodeTable0F92[] = { last_opcode(0, BX_IA_SETB_Eb) };
static const Bit64u BxOpcodeTable0F93[] = { last_opcode(0, BX_IA_SETNB_Eb) };
static const Bit64u BxOpcodeTable0F94[] = { last_opcode(0, BX_IA_SETZ_Eb) };
static const Bit64u BxOpcodeTable0F95[] = { last_opcode(0, BX_IA_SETNZ_Eb) };
static const Bit64u BxOpcodeTable0F96[] = { last_opcode(0, BX_IA_SETBE_Eb) };
static const Bit64u BxOpcodeTable0F97[] = { last_opcode(0, BX_IA_SETNBE_Eb) };
static const Bit64u BxOpcodeTable0F98[] = { last_opcode(0, BX_IA_SETS_Eb) };
static const Bit64u BxOpcodeTable0F99[] = { last_opcode(0, BX_IA_SETNS_Eb) };
static const Bit64u BxOpcodeTable0F9A[] = { last_opcode(0, BX_IA_SETP_Eb) };
static const Bit64u BxOpcodeTable0F9B[] = { last_opcode(0, BX_IA_SETNP_Eb) };
static const Bit64u BxOpcodeTable0F9C[] = { last_opcode(0, BX_IA_SETL_Eb) };
static const Bit64u BxOpcodeTable0F9D[] = { last_opcode(0, BX_IA_SETNL_Eb) };
static const Bit64u BxOpcodeTable0F9E[] = { last_opcode(0, BX_IA_SETLE_Eb) };
static const Bit64u BxOpcodeTable0F9F[] = { last_opcode(0, BX_IA_SETNLE_Eb) };

// opcode 0F A0
static const Bit64u BxOpcodeTable0FA0[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_OS32_64, BX_IA_PUSH_Op64_Sw),
#endif
  form_opcode(ATTR_IS32 | ATTR_OS32,    BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16,                BX_IA_PUSH_Op16_Sw)
};

// opcode 0F A1
static const Bit64u BxOpcodeTable0FA1[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_OS32_64, BX_IA_POP_Op64_Sw),
#endif
  form_opcode(ATTR_IS32 | ATTR_OS32,    BX_IA_POP_Op32_Sw),
  last_opcode(ATTR_OS16,                BX_IA_POP_Op16_Sw)
};

// opcode 0F A2
static const Bit64u BxOpcodeTable0FA2[] = { last_opcode(0, BX_IA_CPUID) };

// opcode 0F A3
static const Bit64u BxOpcodeTable0FA3[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_BT_EqGq),
#endif
  form_opcode(ATTR_OS32, BX_IA_BT_EdGd),
  last_opcode(ATTR_OS16, BX_IA_BT_EwGw)
};

// opcode 0F A4
static const Bit64u BxOpcodeTable0FA4[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SHLD_EqGqIb),
#endif
  form_opcode(ATTR_OS32, BX_IA_SHLD_EdGdIb),
  last_opcode(ATTR_OS16, BX_IA_SHLD_EwGwIb)
};

// opcode 0F A5
static const Bit64u BxOpcodeTable0FA5[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SHLD_EqGq),
#endif
  form_opcode(ATTR_OS32, BX_IA_SHLD_EdGd),
  last_opcode(ATTR_OS16, BX_IA_SHLD_EwGw)
};

// opcode 0F A8
static const Bit64u BxOpcodeTable0FA8[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_OS32_64, BX_IA_PUSH_Op64_Sw),
#endif
  form_opcode(ATTR_IS32 | ATTR_OS32,    BX_IA_PUSH_Op32_Sw),
  last_opcode(ATTR_OS16,                BX_IA_PUSH_Op16_Sw)
};

// opcode 0F A9
static const Bit64u BxOpcodeTable0FA9[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_IS64 | ATTR_OS32_64, BX_IA_POP_Op64_Sw),
#endif
  form_opcode(ATTR_IS32 | ATTR_OS32,    BX_IA_POP_Op32_Sw),
  last_opcode(ATTR_OS16,                BX_IA_POP_Op16_Sw)
};

// opcode 0F AA
static const Bit64u BxOpcodeTable0FAA[] = { last_opcode(0, BX_IA_RSM) };

// opcode 0F AB
static const Bit64u BxOpcodeTable0FAB[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_BTS_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_BTS_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_BTS_EwGw)
};

// opcode 0F AC
static const Bit64u BxOpcodeTable0FAC[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SHRD_EqGqIb),
#endif
  form_opcode(ATTR_OS32, BX_IA_SHRD_EdGdIb),
  last_opcode(ATTR_OS16, BX_IA_SHRD_EwGwIb)
};

// opcode 0F AD
static const Bit64u BxOpcodeTable0FAD[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_SHRD_EqGq),
#endif
  form_opcode(ATTR_OS32, BX_IA_SHRD_EdGd),
  last_opcode(ATTR_OS16, BX_IA_SHRD_EwGw)
};

// opcode 0F AE
static const Bit64u BxOpcodeTable0FAE[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS16_32 | ATTR_IS64 | ATTR_MODC0 | ATTR_NNN0 | ATTR_SSE_PREFIX_F3, BX_IA_RDFSBASE_Ed),
  form_opcode(ATTR_OS16_32 | ATTR_IS64 | ATTR_MODC0 | ATTR_NNN1 | ATTR_SSE_PREFIX_F3, BX_IA_RDGSBASE_Ed),
  form_opcode(ATTR_OS16_32 | ATTR_IS64 | ATTR_MODC0 | ATTR_NNN2 | ATTR_SSE_PREFIX_F3, BX_IA_WRFSBASE_Ed),
  form_opcode(ATTR_OS16_32 | ATTR_IS64 | ATTR_MODC0 | ATTR_NNN3 | ATTR_SSE_PREFIX_F3, BX_IA_WRGSBASE_Ed),
  
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN0 | ATTR_SSE_PREFIX_F3, BX_IA_RDFSBASE_Eq),
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN1 | ATTR_SSE_PREFIX_F3, BX_IA_RDGSBASE_Eq),
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN2 | ATTR_SSE_PREFIX_F3, BX_IA_WRFSBASE_Eq),
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN3 | ATTR_SSE_PREFIX_F3, BX_IA_WRGSBASE_Eq),
#endif

#if BX_SUPPORT_CET
  form_opcode(ATTR_OS16_32 | ATTR_NNN5 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_INCSSPD),
  form_opcode(ATTR_OS64    | ATTR_NNN5 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_INCSSPQ),
#endif

  form_opcode(ATTR_NNN5 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_LFENCE),
  form_opcode(ATTR_NNN6 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_MFENCE),
  form_opcode(ATTR_NNN7 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_SFENCE),

  form_opcode(ATTR_NNN0 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_FXSAVE),
  form_opcode(ATTR_NNN1 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_FXRSTOR),
  form_opcode(ATTR_NNN2 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_LDMXCSR),
  form_opcode(ATTR_NNN3 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_STMXCSR),
  form_opcode(ATTR_NNN4 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XSAVE),
  form_opcode(ATTR_NNN5 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XRSTOR),
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XSAVEOPT),
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_CLWB),
#if BX_SUPPORT_CET
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_CLRSSBSY),
#endif
  form_opcode(ATTR_NNN7 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_CLFLUSH),
  last_opcode(ATTR_NNN7 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_CLFLUSHOPT)
};

// opcode 0F AF
static const Bit64u BxOpcodeTable0FAF[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_IMUL_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_IMUL_GdEd),
  last_opcode(ATTR_OS16, BX_IA_IMUL_GwEw)
};

// opcode 0F B0
static const Bit64u BxOpcodeTable0FB0[] = { last_opcode_lockable(0, BX_IA_CMPXCHG_EbGb) };

// opcode 0F B1
static const Bit64u BxOpcodeTable0FB1[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_CMPXCHG_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_CMPXCHG_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_CMPXCHG_EwGw)
};

// opcode 0F B2
static const Bit64u BxOpcodeTable0FB2[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM, BX_IA_LSS_GqMp), // TODO: LSS_GdMp for AMD CPU
#endif
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM, BX_IA_LSS_GdMp),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM, BX_IA_LSS_GwMp)
};

// opcode 0F B3
static const Bit64u BxOpcodeTable0FB3[] = { 
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_BTR_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_BTR_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_BTR_EwGw)
};

// opcode 0F B4
static const Bit64u BxOpcodeTable0FB4[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM, BX_IA_LFS_GqMp), // TODO: LFS_GdMp for AMD CPU
#endif
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM, BX_IA_LFS_GdMp),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM, BX_IA_LFS_GwMp)
};

// opcode 0F B5
static const Bit64u BxOpcodeTable0FB5[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM, BX_IA_LGS_GqMp), // TODO: LGS_GdMp for AMD CPU
#endif
  form_opcode(ATTR_OS32 | ATTR_MOD_MEM, BX_IA_LGS_GdMp),
  last_opcode(ATTR_OS16 | ATTR_MOD_MEM, BX_IA_LGS_GwMp)
};

// opcode 0F B6
static const Bit64u BxOpcodeTable0FB6[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOVZX_GqEb),
#endif
  form_opcode(ATTR_OS32, BX_IA_MOVZX_GdEb),
  last_opcode(ATTR_OS16, BX_IA_MOVZX_GwEb)
};

// opcode 0F B7
static const Bit64u BxOpcodeTable0FB7[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOVZX_GqEw),
#endif
  form_opcode(ATTR_OS32, BX_IA_MOVZX_GdEw),
  last_opcode(ATTR_OS16, BX_IA_MOV_GwEw)                 // MOVZX_GwEw
};

// opcode 0F B8
static const Bit64u BxOpcodeTable0FB8[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SSE_PREFIX_F3, BX_IA_POPCNT_GqEq),
#endif
  form_opcode(ATTR_OS32 | ATTR_SSE_PREFIX_F3, BX_IA_POPCNT_GdEd),
  last_opcode(ATTR_OS16 | ATTR_SSE_PREFIX_F3, BX_IA_POPCNT_GwEw)
};

// opcode 0F B9
static const Bit64u BxOpcodeTable0FB9[] = { last_opcode(0, BX_IA_UD1) };

// opcode 0F BA
static const Bit64u BxOpcodeTable0FBA[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NNN4 | ATTR_OS64, BX_IA_BT_EqIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS64, BX_IA_BTS_EqIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS64, BX_IA_BTR_EqIb),
  form_opcode_lockable(ATTR_NNN7 | ATTR_OS64, BX_IA_BTC_EqIb),
#endif

  form_opcode(ATTR_NNN4 | ATTR_OS32, BX_IA_BT_EdIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS32, BX_IA_BTS_EdIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS32, BX_IA_BTR_EdIb),
  form_opcode_lockable(ATTR_NNN7 | ATTR_OS32, BX_IA_BTC_EdIb),

  form_opcode(ATTR_NNN4 | ATTR_OS16, BX_IA_BT_EwIb),
  form_opcode_lockable(ATTR_NNN5 | ATTR_OS16, BX_IA_BTS_EwIb),
  form_opcode_lockable(ATTR_NNN6 | ATTR_OS16, BX_IA_BTR_EwIb),
  last_opcode_lockable(ATTR_NNN7 | ATTR_OS16, BX_IA_BTC_EwIb)
};

// opcode 0F BB
static const Bit64u BxOpcodeTable0FBB[] = {
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_BTC_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_BTC_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_BTC_EwGw)
};

// opcode 0F BC
static const Bit64u BxOpcodeTable0FBC[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SSE_PREFIX_F3, BX_IA_TZCNT_GqEq),
#endif
  form_opcode(ATTR_OS32 | ATTR_SSE_PREFIX_F3, BX_IA_TZCNT_GdEd),
  form_opcode(ATTR_OS16 | ATTR_SSE_PREFIX_F3, BX_IA_TZCNT_GwEw),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_BSF_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_BSF_GdEd),
  last_opcode(ATTR_OS16, BX_IA_BSF_GwEw)
};

// opcode 0F BD
static const Bit64u BxOpcodeTable0FBD[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_SSE_PREFIX_F3, BX_IA_LZCNT_GqEq),
#endif
  form_opcode(ATTR_OS32 | ATTR_SSE_PREFIX_F3, BX_IA_LZCNT_GdEd),
  form_opcode(ATTR_OS16 | ATTR_SSE_PREFIX_F3, BX_IA_LZCNT_GwEw),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_BSR_GqEq),
#endif
  form_opcode(ATTR_OS32, BX_IA_BSR_GdEd),
  last_opcode(ATTR_OS16, BX_IA_BSR_GwEw)
};

// opcode 0F BE
static const Bit64u BxOpcodeTable0FBE[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOVSX_GqEb),
#endif
  form_opcode(ATTR_OS32, BX_IA_MOVSX_GdEb),
  last_opcode(ATTR_OS16, BX_IA_MOVSX_GwEb)
};

// opcode 0F BF
static const Bit64u BxOpcodeTable0FBF[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_MOVSX_GqEw),
#endif
  form_opcode(ATTR_OS32, BX_IA_MOVSX_GdEw),
  last_opcode(ATTR_OS16, BX_IA_MOV_GwEw)            // MOVSX_GwEw
};

// opcode 0F C0
static const Bit64u BxOpcodeTable0FC0[] = { last_opcode_lockable(0, BX_IA_XADD_EbGb) };

// opcode 0F C1
static const Bit64u BxOpcodeTable0FC1[] = {
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64, BX_IA_XADD_EqGq),
#endif
  form_opcode_lockable(ATTR_OS32, BX_IA_XADD_EdGd),
  last_opcode_lockable(ATTR_OS16, BX_IA_XADD_EwGw)
};

// opcode 0F C2
static const Bit64u BxOpcodeTable0FC2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_CMPPS_VpsWpsIb),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CMPPD_VpdWpdIb),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CMPSS_VssWssIb),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CMPSD_VsdWsdIb)
};

static const Bit64u BxOpcodeTable0FC3[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_IS64 | ATTR_OS16_32, BX_IA_MOVNTI_Op64_MdGd),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_IS64 | ATTR_OS64,    BX_IA_MOVNTI_MqGq),
#endif
  last_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_IS32, BX_IA_MOVNTI_Op32_MdGd)
};

// opcode 0F C4
static const Bit64u BxOpcodeTable0FC4[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PINSRW_PqEwIb),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PINSRW_VdqEwIb)
};

// opcode 0F C5
static const Bit64u BxOpcodeTable0FC5[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PEXTRW_GdNqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PEXTRW_GdUdqIb)
};

// opcode 0F C6
static const Bit64u BxOpcodeTable0FC6[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHUFPS_VpsWpsIb),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_SHUFPD_VpdWpdIb)
};

// opcode 0F C7
static const Bit64u BxOpcodeTable0FC7[] = {
  form_opcode(ATTR_OS16 | ATTR_MODC0 | ATTR_NNN6 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDRAND_Ew),
  form_opcode(ATTR_OS16 | ATTR_MODC0 | ATTR_NNN7 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDSEED_Ew),
  form_opcode(ATTR_OS32 | ATTR_MODC0 | ATTR_NNN6 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDRAND_Ed),
  form_opcode(ATTR_OS32 | ATTR_MODC0 | ATTR_NNN7 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDSEED_Ed),
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN6 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDRAND_Eq),
  form_opcode(ATTR_OS64 | ATTR_MODC0 | ATTR_NNN7 | ATTR_NO_SSE_PREFIX_F2_F3, BX_IA_RDSEED_Eq),
#endif
  form_opcode(ATTR_NNN7 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_RDPID_Ed),
  form_opcode_lockable(ATTR_OS16_32 | ATTR_NNN1 | ATTR_MOD_MEM, BX_IA_CMPXCHG8B),
#if BX_SUPPORT_X86_64
  form_opcode_lockable(ATTR_OS64    | ATTR_NNN1 | ATTR_MOD_MEM, BX_IA_CMPXCHG16B),
#endif
  form_opcode(ATTR_NNN3 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XRSTORS),
  form_opcode(ATTR_NNN4 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XSAVEC),
  form_opcode(ATTR_NNN5 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_XSAVES),
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_VMPTRLD_Mq),
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_VMCLEAR_Mq),
  form_opcode(ATTR_NNN6 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_VMXON_Mq),
  last_opcode(ATTR_NNN7 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_VMPTRST_Mq)
};

// opcode 0F C8 - 0F CF
static const Bit64u BxOpcodeTable0FC8x0FCF[] = { 
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64, BX_IA_BSWAP_RRX),
#endif
  form_opcode(ATTR_OS32, BX_IA_BSWAP_ERX),
  last_opcode(ATTR_OS16, BX_IA_BSWAP_RX)
};

// opcode 0F D0
static const Bit64u BxOpcodeTable0FD0[] = {
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_ADDSUBPD_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_ADDSUBPS_VpsWps)
};

// opcode 0F D1
static const Bit64u BxOpcodeTable0FD1[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSRLW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSRLW_VdqWdq)
};

// opcode 0F D2
static const Bit64u BxOpcodeTable0FD2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSRLD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSRLD_VdqWdq)
};

// opcode 0F D3
static const Bit64u BxOpcodeTable0FD3[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSRLQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSRLQ_VdqWdq)
};

// opcode 0F D4
static const Bit64u BxOpcodeTable0FD4[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDQ_VdqWdq)
};

// opcode 0F D5
static const Bit64u BxOpcodeTable0FD5[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMULLW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULLW_VdqWdq)
};

// opcode 0F D6
static const Bit64u BxOpcodeTable0FD6[] = {
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_MOVQ_WqVq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0, BX_IA_MOVQ2DQ_VdqQq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MODC0, BX_IA_MOVDQ2Q_PqUdq)
};

// opcode 0F D7
static const Bit64u BxOpcodeTable0FD7[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_PMOVMSKB_GdNq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_PMOVMSKB_GdUdq)
};

// opcode 0F D8
static const Bit64u BxOpcodeTable0FD8[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBUSB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBUSB_VdqWdq)
};

// opcode 0F D9
static const Bit64u BxOpcodeTable0FD9[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBUSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBUSW_VdqWdq)
};

// opcode 0F DA
static const Bit64u BxOpcodeTable0FDA[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMINUB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINUB_VdqWdq)
};

// opcode 0F DB
static const Bit64u BxOpcodeTable0FDB[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PAND_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PAND_VdqWdq)
};

// opcode 0F DC
static const Bit64u BxOpcodeTable0FDC[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDUSB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDUSB_VdqWdq)
};

// opcode 0F DD
static const Bit64u BxOpcodeTable0FDD[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDUSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDUSW_VdqWdq)
};

// opcode 0F DE
static const Bit64u BxOpcodeTable0FDE[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMAXUB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXUB_VdqWdq)
};

// opcode 0F DF
static const Bit64u BxOpcodeTable0FDF[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PANDN_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PANDN_VdqWdq)
};

// opcode 0F E0
static const Bit64u BxOpcodeTable0FE0[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PAVGB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PAVGB_VdqWdq)
};

// opcode 0F E1
static const Bit64u BxOpcodeTable0FE1[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSRAW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSRAW_VdqWdq)
};

// opcode 0F E2
static const Bit64u BxOpcodeTable0FE2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSRAD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSRAD_VdqWdq)
};

// opcode 0F E3
static const Bit64u BxOpcodeTable0FE3[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PAVGW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PAVGW_VdqWdq)
};

// opcode 0F E4
static const Bit64u BxOpcodeTable0FE4[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMULHUW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULHUW_VdqWdq)
};

// opcode 0F E5
static const Bit64u BxOpcodeTable0FE5[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMULHW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULHW_VdqWdq)
};

// opcode 0F E6
static const Bit64u BxOpcodeTable0FE6[] = {
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_CVTTPD2DQ_VqWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_CVTDQ2PD_VpdWq),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CVTPD2DQ_VqWpd)
};

// opcode 0F E7
static const Bit64u BxOpcodeTable0FE7[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_MOVNTQ_MqPq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVNTDQ_MdqVdq)
};

// opcode 0F E8
static const Bit64u BxOpcodeTable0FE8[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBSB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBSB_VdqWdq)
};

// opcode 0F E9
static const Bit64u BxOpcodeTable0FE9[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBSW_VdqWdq)
};

// opcode 0F EA
static const Bit64u BxOpcodeTable0FEA[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMINSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINSW_VdqWdq)
};

// opcode 0F EB
static const Bit64u BxOpcodeTable0FEB[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_POR_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_POR_VdqWdq)
};

// opcode 0F EC
static const Bit64u BxOpcodeTable0FEC[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDSB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDSB_VdqWdq)
};

// opcode 0F ED
static const Bit64u BxOpcodeTable0FED[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDSW_VdqWdq)
};

// opcode 0F EE
static const Bit64u BxOpcodeTable0FEE[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMAXSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXSW_VdqWdq)
};

// opcode 0F EF
static const Bit64u BxOpcodeTable0FEF[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PXOR_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PXOR_VdqWdq)
};

// opcode 0F F0
static const Bit64u BxOpcodeTable0FF0[4] = { last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MOD_MEM, BX_IA_LDDQU_VdqMdq) };

// opcode 0F F1
static const Bit64u BxOpcodeTable0FF1[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSLLW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSLLW_VdqWdq)
};

// opcode 0F F2
static const Bit64u BxOpcodeTable0FF2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSLLD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSLLD_VdqWdq)
};

// opcode 0F F3
static const Bit64u BxOpcodeTable0FF3[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSLLQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSLLQ_VdqWdq)
};

// opcode 0F F4
static const Bit64u BxOpcodeTable0FF4[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMULUDQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULUDQ_VdqWdq)
};

// opcode 0F F5
static const Bit64u BxOpcodeTable0FF5[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMADDWD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMADDWD_VdqWdq)
};

// opcode 0F F6
static const Bit64u BxOpcodeTable0FF6[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSADBW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSADBW_VdqWdq)
};

// opcode 0F F7
static const Bit64u BxOpcodeTable0FF7[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_MASKMOVQ_PqNq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_MASKMOVDQU_VdqUdq)
};

// opcode 0F F8
static const Bit64u BxOpcodeTable0FF8[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBB_VdqWdq)
};

// opcode 0F F9
static const Bit64u BxOpcodeTable0FF9[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBW_VdqWdq)
};

// opcode 0F FA
static const Bit64u BxOpcodeTable0FFA[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBD_VdqWdq)
};

// opcode 0F FB
static const Bit64u BxOpcodeTable0FFB[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSUBQ_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSUBQ_VdqWdq)
};

// opcode 0F FC
static const Bit64u BxOpcodeTable0FFC[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDB_VdqWdq)
};

// opcode 0F FD
static const Bit64u BxOpcodeTable0FFD[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDW_VdqWdq)
};

// opcode 0F FE
static const Bit64u BxOpcodeTable0FFE[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PADDD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PADDD_VdqWdq)
};

// opcode 0F FF
static const Bit64u BxOpcodeTable0FFF[] = { last_opcode(0, BX_IA_UD0) };

#endif // BX_FETCHDECODE_OPMAP_H
