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


#define BX_IN_CPU_METHOD 1
#include "bochs.h"


///////////////////////////
// prefix bytes
// opcode bytes
// modrm/sib
// address displacement
// immediate constant
///////////////////////////



// sign extended to osize:
//   6a push ib
//   6b imul gvevib
//   70..7f jo..jnle
//   83 G1 0..7 ADD..CMP Evib

// is 6b imul_gvevib sign extended?  don't think
//   I'm sign extending it properly in old decode/execute

//check all the groups.  Make sure to add duplicates rather
// than error.

// mark instructions as changing control transfer, then
// don't always load from fetch_ptr, etc.

// cant use immediate as another because of Group3 where
// some have immediate and some don't, and those won't
// be picked up by logic until indirection.

// get attr and execute ptr at same time

// maybe move 16bit only i's like  MOV_EwSw, MOV_SwEw
// to 32 bit modules.

// use 0F as a prefix too?




void BxResolveError(BxInstruction_t *);

#if BX_DYNAMIC_TRANSLATION
// For 16-bit address mode, this matrix describes the registers
// used to formulate the offset, indexed by the RM field.
// This info is needed by the dynamic translation code for dataflow.
static unsigned BxMemRegsUsed16[8] = {
  (1<<3) | (1<<6), // BX + SI
  (1<<3) | (1<<7), // BX + DI
  (1<<5) | (1<<6), // BP + SI
  (1<<5) | (1<<7), // BP + DI
  (1<<6),          // SI
  (1<<7),          // DI
  (1<<5),          // BP
  (1<<3)           // BX
  };
#endif

static BxExecutePtr_t BxResolve16Mod0[8] = {
  &BX_CPU_C::Resolve16Mod0Rm0,
  &BX_CPU_C::Resolve16Mod0Rm1,
  &BX_CPU_C::Resolve16Mod0Rm2,
  &BX_CPU_C::Resolve16Mod0Rm3,
  &BX_CPU_C::Resolve16Mod0Rm4,
  &BX_CPU_C::Resolve16Mod0Rm5,
  NULL, // d16, no registers used
  &BX_CPU_C::Resolve16Mod0Rm7
  };

static BxExecutePtr_t BxResolve16Mod1or2[8] = {
  &BX_CPU_C::Resolve16Mod1or2Rm0,
  &BX_CPU_C::Resolve16Mod1or2Rm1,
  &BX_CPU_C::Resolve16Mod1or2Rm2,
  &BX_CPU_C::Resolve16Mod1or2Rm3,
  &BX_CPU_C::Resolve16Mod1or2Rm4,
  &BX_CPU_C::Resolve16Mod1or2Rm5,
  &BX_CPU_C::Resolve16Mod1or2Rm6,
  &BX_CPU_C::Resolve16Mod1or2Rm7
  };

static BxExecutePtr_t BxResolve32Mod0[8] = {
  &BX_CPU_C::Resolve32Mod0Rm0,
  &BX_CPU_C::Resolve32Mod0Rm1,
  &BX_CPU_C::Resolve32Mod0Rm2,
  &BX_CPU_C::Resolve32Mod0Rm3,
  NULL, // escape to 2-byte
  NULL, // d32, no registers used
  &BX_CPU_C::Resolve32Mod0Rm6,
  &BX_CPU_C::Resolve32Mod0Rm7
  };

static BxExecutePtr_t BxResolve32Mod1or2[8] = {
  &BX_CPU_C::Resolve32Mod1or2Rm0,
  &BX_CPU_C::Resolve32Mod1or2Rm1,
  &BX_CPU_C::Resolve32Mod1or2Rm2,
  &BX_CPU_C::Resolve32Mod1or2Rm3,
  NULL, // escape to 2-byte
  &BX_CPU_C::Resolve32Mod1or2Rm5,
  &BX_CPU_C::Resolve32Mod1or2Rm6,
  &BX_CPU_C::Resolve32Mod1or2Rm7
  };

static BxExecutePtr_t BxResolve32Mod0Base[8] = {
  &BX_CPU_C::Resolve32Mod0Base0,
  &BX_CPU_C::Resolve32Mod0Base1,
  &BX_CPU_C::Resolve32Mod0Base2,
  &BX_CPU_C::Resolve32Mod0Base3,
  &BX_CPU_C::Resolve32Mod0Base4,
  &BX_CPU_C::Resolve32Mod0Base5,
  &BX_CPU_C::Resolve32Mod0Base6,
  &BX_CPU_C::Resolve32Mod0Base7,
  };

static BxExecutePtr_t BxResolve32Mod1or2Base[8] = {
  &BX_CPU_C::Resolve32Mod1or2Base0,
  &BX_CPU_C::Resolve32Mod1or2Base1,
  &BX_CPU_C::Resolve32Mod1or2Base2,
  &BX_CPU_C::Resolve32Mod1or2Base3,
  &BX_CPU_C::Resolve32Mod1or2Base4,
  &BX_CPU_C::Resolve32Mod1or2Base5,
  &BX_CPU_C::Resolve32Mod1or2Base6,
  &BX_CPU_C::Resolve32Mod1or2Base7,
  };

typedef struct BxOpcodeInfo_t {
  Bit16u         Attr;
  BxExecutePtr_t ExecutePtr;
  struct BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;

static BxOpcodeInfo_t BxOpcodeInfoG1EbIb[8] = {
  /* 0 */  { BxImmediate_Ib,  &BX_CPU_C::ADD_EbIb },
  /* 1 */  { BxImmediate_Ib,  &BX_CPU_C::OR_EbIb },
  /* 2 */  { BxImmediate_Ib,  &BX_CPU_C::ADC_EbIb },
  /* 3 */  { BxImmediate_Ib,  &BX_CPU_C::SBB_EbIb },
  /* 4 */  { BxImmediate_Ib,  &BX_CPU_C::AND_EbIb },
  /* 5 */  { BxImmediate_Ib,  &BX_CPU_C::SUB_EbIb },
  /* 6 */  { BxImmediate_Ib,  &BX_CPU_C::XOR_EbIb },
  /* 7 */  { BxImmediate_Ib,  &BX_CPU_C::CMP_EbIb }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG1Ew[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::ADD_EwIw },
  /* 1 */  { 0,  &BX_CPU_C::OR_EwIw },
  /* 2 */  { 0,  &BX_CPU_C::ADC_EwIw },
  /* 3 */  { 0,  &BX_CPU_C::SBB_EwIw },
  /* 4 */  { 0,  &BX_CPU_C::AND_EwIw },
  /* 5 */  { 0,  &BX_CPU_C::SUB_EwIw },
  /* 6 */  { 0,  &BX_CPU_C::XOR_EwIw },
  /* 7 */  { 0,  &BX_CPU_C::CMP_EwIw }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG1Ed[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::ADD_EdId },
  /* 1 */  { 0,  &BX_CPU_C::OR_EdId },
  /* 2 */  { 0,  &BX_CPU_C::ADC_EdId },
  /* 3 */  { 0,  &BX_CPU_C::SBB_EdId },
  /* 4 */  { 0,  &BX_CPU_C::AND_EdId },
  /* 5 */  { 0,  &BX_CPU_C::SUB_EdId },
  /* 6 */  { 0,  &BX_CPU_C::XOR_EdId },
  /* 7 */  { 0,  &BX_CPU_C::CMP_EdId }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG2Eb[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::ROL_Eb },
  /* 1 */  { 0,  &BX_CPU_C::ROR_Eb },
  /* 2 */  { 0,  &BX_CPU_C::RCL_Eb },
  /* 3 */  { 0,  &BX_CPU_C::RCR_Eb },
  /* 4 */  { 0,  &BX_CPU_C::SHL_Eb },
  /* 5 */  { 0,  &BX_CPU_C::SHR_Eb },
  /* 6 */  { 0,  &BX_CPU_C::SHL_Eb },
  /* 7 */  { 0,  &BX_CPU_C::SAR_Eb }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG2Ew[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::ROL_Ew },
  /* 1 */  { 0,  &BX_CPU_C::ROR_Ew },
  /* 2 */  { 0,  &BX_CPU_C::RCL_Ew },
  /* 3 */  { 0,  &BX_CPU_C::RCR_Ew },
  /* 4 */  { 0,  &BX_CPU_C::SHL_Ew },
  /* 5 */  { 0,  &BX_CPU_C::SHR_Ew },
  /* 6 */  { 0,  &BX_CPU_C::SHL_Ew },
  /* 7 */  { 0,  &BX_CPU_C::SAR_Ew }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG2Ed[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::ROL_Ed },
  /* 1 */  { 0,  &BX_CPU_C::ROR_Ed },
  /* 2 */  { 0,  &BX_CPU_C::RCL_Ed },
  /* 3 */  { 0,  &BX_CPU_C::RCR_Ed },
  /* 4 */  { 0,  &BX_CPU_C::SHL_Ed },
  /* 5 */  { 0,  &BX_CPU_C::SHR_Ed },
  /* 6 */  { 0,  &BX_CPU_C::SHL_Ed },
  /* 7 */  { 0,  &BX_CPU_C::SAR_Ed }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG3Eb[8] = {
  /* 0 */  { BxImmediate_Ib,  &BX_CPU_C::TEST_EbIb },
  /* 1 */  { BxImmediate_Ib,  &BX_CPU_C::TEST_EbIb },
  /* 2 */  { 0,             &BX_CPU_C::NOT_Eb },
  /* 3 */  { 0,             &BX_CPU_C::NEG_Eb },
  /* 4 */  { 0,             &BX_CPU_C::MUL_ALEb },
  /* 5 */  { 0,             &BX_CPU_C::IMUL_ALEb },
  /* 6 */  { 0,             &BX_CPU_C::DIV_ALEb },
  /* 7 */  { 0,             &BX_CPU_C::IDIV_ALEb }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG3Ew[8] = {
  /* 0 */  { BxImmediate_Iw,  &BX_CPU_C::TEST_EwIw },
  /* 1 */  { BxImmediate_Iw,  &BX_CPU_C::TEST_EwIw },
  /* 2 */  { 0,             &BX_CPU_C::NOT_Ew },
  /* 3 */  { 0,             &BX_CPU_C::NEG_Ew },
  /* 4 */  { 0,             &BX_CPU_C::MUL_AXEw },
  /* 5 */  { 0,             &BX_CPU_C::IMUL_AXEw },
  /* 6 */  { 0,             &BX_CPU_C::DIV_AXEw },
  /* 7 */  { 0,             &BX_CPU_C::IDIV_AXEw }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG3Ed[8] = {
  /* 0 */  { BxImmediate_Iv,  &BX_CPU_C::TEST_EdId },
  /* 1 */  { BxImmediate_Iv,  &BX_CPU_C::TEST_EdId },
  /* 2 */  { 0,             &BX_CPU_C::NOT_Ed },
  /* 3 */  { 0,             &BX_CPU_C::NEG_Ed },
  /* 4 */  { 0,             &BX_CPU_C::MUL_EAXEd },
  /* 5 */  { 0,             &BX_CPU_C::IMUL_EAXEd },
  /* 6 */  { 0,             &BX_CPU_C::DIV_EAXEd },
  /* 7 */  { 0,             &BX_CPU_C::IDIV_EAXEd }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG4[8] = {
  /* 0 */  { 0,  &BX_CPU_C::INC_Eb },
  /* 1 */  { 0,  &BX_CPU_C::DEC_Eb },
  /* 2 */  { 0,  &BX_CPU_C::BxError },
  /* 3 */  { 0,  &BX_CPU_C::BxError },
  /* 4 */  { 0,  &BX_CPU_C::BxError },
  /* 5 */  { 0,  &BX_CPU_C::BxError },
  /* 6 */  { 0,  &BX_CPU_C::BxError },
  /* 7 */  { 0,  &BX_CPU_C::BxError }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG5w[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::INC_Ew },
  /* 1 */  { 0,  &BX_CPU_C::DEC_Ew },
  /* 2 */  { 0,  &BX_CPU_C::CALL_Ew },
  /* 3 */  { 0,  &BX_CPU_C::CALL16_Ep },
  /* 4 */  { 0,  &BX_CPU_C::JMP_Ew },
  /* 5 */  { 0,  &BX_CPU_C::JMP16_Ep },
  /* 6 */  { 0,  &BX_CPU_C::PUSH_Ew },
  /* 7 */  { 0,  &BX_CPU_C::BxError }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG5d[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::INC_Ed },
  /* 1 */  { 0,  &BX_CPU_C::DEC_Ed },
  /* 2 */  { 0,  &BX_CPU_C::CALL_Ed },
  /* 3 */  { 0,  &BX_CPU_C::CALL32_Ep },
  /* 4 */  { 0,  &BX_CPU_C::JMP_Ed },
  /* 5 */  { 0,  &BX_CPU_C::JMP32_Ep },
  /* 6 */  { 0,  &BX_CPU_C::PUSH_Ed },
  /* 7 */  { 0,  &BX_CPU_C::BxError }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG6[8] = {
  // attributes defined in main area
  /* 0 */  { 0,  &BX_CPU_C::SLDT_Ew },
  /* 1 */  { 0,  &BX_CPU_C::STR_Ew },
  /* 2 */  { 0,  &BX_CPU_C::LLDT_Ew },
  /* 3 */  { 0,  &BX_CPU_C::LTR_Ew },
  /* 4 */  { 0,  &BX_CPU_C::VERR_Ew },
  /* 5 */  { 0,  &BX_CPU_C::VERW_Ew },
  /* 6 */  { 0,  &BX_CPU_C::BxError },
  /* 7 */  { 0,  &BX_CPU_C::BxError }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG7[8] = {
  /* 0 */  { 0,  &BX_CPU_C::SGDT_Ms },
  /* 1 */  { 0,  &BX_CPU_C::SIDT_Ms },
  /* 2 */  { 0,  &BX_CPU_C::LGDT_Ms },
  /* 3 */  { 0,  &BX_CPU_C::LIDT_Ms },
  /* 4 */  { 0,  &BX_CPU_C::SMSW_Ew },
  /* 5 */  { 0,  &BX_CPU_C::BxError },
  /* 6 */  { 0,  &BX_CPU_C::LMSW_Ew },
  /* 7 */  { 0,  &BX_CPU_C::INVLPG }
  }; 


static BxOpcodeInfo_t BxOpcodeInfoG8EvIb[8] = {
  /* 0 */  { 0,  &BX_CPU_C::BxError },
  /* 1 */  { 0,  &BX_CPU_C::BxError },
  /* 2 */  { 0,  &BX_CPU_C::BxError },
  /* 3 */  { 0,  &BX_CPU_C::BxError },
  /* 4 */  { BxImmediate_Ib,  &BX_CPU_C::BT_EvIb },
  /* 5 */  { BxImmediate_Ib,  &BX_CPU_C::BTS_EvIb },
  /* 6 */  { BxImmediate_Ib,  &BX_CPU_C::BTR_EvIb },
  /* 7 */  { BxImmediate_Ib,  &BX_CPU_C::BTC_EvIb }
  }; 

static BxOpcodeInfo_t BxOpcodeInfoG9[8] = {
  /* 0 */  { 0,  &BX_CPU_C::BxError },
  /* 1 */  { 0,  &BX_CPU_C::CMPXCHG8B },
  /* 2 */  { 0,  &BX_CPU_C::BxError },
  /* 3 */  { 0,  &BX_CPU_C::BxError },
  /* 4 */  { 0,  &BX_CPU_C::BxError },
  /* 5 */  { 0,  &BX_CPU_C::BxError },
  /* 6 */  { 0,  &BX_CPU_C::BxError },
  /* 7 */  { 0,  &BX_CPU_C::BxError }
  };


// 512 entries for 16bit mode
// 512 entries for 32bit mode

static BxOpcodeInfo_t BxOpcodeInfo[512*2] = {
  // 512 entries for 16bit mode
  /* 00 */  { BxAnother,  &BX_CPU_C::ADD_EbGb },
  /* 01 */  { BxAnother,  &BX_CPU_C::ADD_EwGw },
  /* 02 */  { BxAnother,  &BX_CPU_C::ADD_GbEb },
  /* 03 */  { BxAnother,  &BX_CPU_C::ADD_GwEw },
  /* 04 */  { BxImmediate_Ib,  &BX_CPU_C::ADD_ALIb },
  /* 05 */  { BxImmediate_Iv,  &BX_CPU_C::ADD_AXIw },
  /* 06 */  { 0,  &BX_CPU_C::PUSH_ES },
  /* 07 */  { 0,  &BX_CPU_C::POP_ES },
  /* 08 */  { BxAnother,  &BX_CPU_C::OR_EbGb },
  /* 09 */  { BxAnother,  &BX_CPU_C::OR_EwGw },
  /* 0A */  { BxAnother,  &BX_CPU_C::OR_GbEb },
  /* 0B */  { BxAnother,  &BX_CPU_C::OR_GwEw },
  /* 0C */  { BxImmediate_Ib,  &BX_CPU_C::OR_ALIb },
  /* 0D */  { BxImmediate_Iv,  &BX_CPU_C::OR_AXIw },
  /* 0E */  { 0,  &BX_CPU_C::PUSH_CS },
  /* 0F */  { BxAnother,  &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 */  { BxAnother,  &BX_CPU_C::ADC_EbGb },
  /* 11 */  { BxAnother,  &BX_CPU_C::ADC_EwGw },
  /* 12 */  { BxAnother,  &BX_CPU_C::ADC_GbEb },
  /* 13 */  { BxAnother,  &BX_CPU_C::ADC_GwEw },
  /* 14 */  { BxImmediate_Ib,  &BX_CPU_C::ADC_ALIb },
  /* 15 */  { BxImmediate_Iv,  &BX_CPU_C::ADC_AXIw },
  /* 16 */  { 0,  &BX_CPU_C::PUSH_SS },
  /* 17 */  { 0,  &BX_CPU_C::POP_SS },
  /* 18 */  { BxAnother,  &BX_CPU_C::SBB_EbGb },
  /* 19 */  { BxAnother,  &BX_CPU_C::SBB_EwGw },
  /* 1A */  { BxAnother,  &BX_CPU_C::SBB_GbEb },
  /* 1B */  { BxAnother,  &BX_CPU_C::SBB_GwEw },
  /* 1C */  { BxImmediate_Ib,  &BX_CPU_C::SBB_ALIb },
  /* 1D */  { BxImmediate_Iv,  &BX_CPU_C::SBB_AXIw },
  /* 1E */  { 0,  &BX_CPU_C::PUSH_DS },
  /* 1F */  { 0,  &BX_CPU_C::POP_DS },
  /* 20 */  { BxAnother,  &BX_CPU_C::AND_EbGb },
  /* 21 */  { BxAnother,  &BX_CPU_C::AND_EwGw },
  /* 22 */  { BxAnother,  &BX_CPU_C::AND_GbEb },
  /* 23 */  { BxAnother,  &BX_CPU_C::AND_GwEw },
  /* 24 */  { BxImmediate_Ib,  &BX_CPU_C::AND_ALIb },
  /* 25 */  { BxImmediate_Iv,  &BX_CPU_C::AND_AXIw },
  /* 26 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // ES:
  /* 27 */  { 0,  &BX_CPU_C::DAA },
  /* 28 */  { BxAnother,  &BX_CPU_C::SUB_EbGb },
  /* 29 */  { BxAnother,  &BX_CPU_C::SUB_EwGw },
  /* 2A */  { BxAnother,  &BX_CPU_C::SUB_GbEb },
  /* 2B */  { BxAnother,  &BX_CPU_C::SUB_GwEw },
  /* 2C */  { BxImmediate_Ib,  &BX_CPU_C::SUB_ALIb },
  /* 2D */  { BxImmediate_Iv,  &BX_CPU_C::SUB_AXIw },
  /* 2E */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // CS:
  /* 2F */  { 0,  &BX_CPU_C::DAS },
  /* 30 */  { BxAnother,  &BX_CPU_C::XOR_EbGb },
  /* 31 */  { BxAnother,  &BX_CPU_C::XOR_EwGw },
  /* 32 */  { BxAnother,  &BX_CPU_C::XOR_GbEb },
  /* 33 */  { BxAnother,  &BX_CPU_C::XOR_GwEw },
  /* 34 */  { BxImmediate_Ib,  &BX_CPU_C::XOR_ALIb },
  /* 35 */  { BxImmediate_Iv,  &BX_CPU_C::XOR_AXIw },
  /* 36 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // SS:
  /* 37 */  { 0,  &BX_CPU_C::AAA },
  /* 38 */  { BxAnother,  &BX_CPU_C::CMP_EbGb },
  /* 39 */  { BxAnother,  &BX_CPU_C::CMP_EwGw },
  /* 3A */  { BxAnother,  &BX_CPU_C::CMP_GbEb },
  /* 3B */  { BxAnother,  &BX_CPU_C::CMP_GwEw },
  /* 3C */  { BxImmediate_Ib,  &BX_CPU_C::CMP_ALIb },
  /* 3D */  { BxImmediate_Iv,  &BX_CPU_C::CMP_AXIw },
  /* 3E */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // DS:
  /* 3F */  { 0,  &BX_CPU_C::AAS },
  /* 40 */  { 0,  &BX_CPU_C::INC_RX },
  /* 41 */  { 0,  &BX_CPU_C::INC_RX },
  /* 42 */  { 0,  &BX_CPU_C::INC_RX },
  /* 43 */  { 0,  &BX_CPU_C::INC_RX },
  /* 44 */  { 0,  &BX_CPU_C::INC_RX },
  /* 45 */  { 0,  &BX_CPU_C::INC_RX },
  /* 46 */  { 0,  &BX_CPU_C::INC_RX },
  /* 47 */  { 0,  &BX_CPU_C::INC_RX },
  /* 48 */  { 0,  &BX_CPU_C::DEC_RX },
  /* 49 */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4A */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4B */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4C */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4D */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4E */  { 0,  &BX_CPU_C::DEC_RX },
  /* 4F */  { 0,  &BX_CPU_C::DEC_RX },
  /* 50 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 51 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 52 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 53 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 54 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 55 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 56 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 57 */  { 0,  &BX_CPU_C::PUSH_RX },
  /* 58 */  { 0,  &BX_CPU_C::POP_RX },
  /* 59 */  { 0,  &BX_CPU_C::POP_RX },
  /* 5A */  { 0,  &BX_CPU_C::POP_RX },
  /* 5B */  { 0,  &BX_CPU_C::POP_RX },
  /* 5C */  { 0,  &BX_CPU_C::POP_RX },
  /* 5D */  { 0,  &BX_CPU_C::POP_RX },
  /* 5E */  { 0,  &BX_CPU_C::POP_RX },
  /* 5F */  { 0,  &BX_CPU_C::POP_RX },
  /* 60 */  { 0,  &BX_CPU_C::PUSHAD16 },
  /* 61 */  { 0,  &BX_CPU_C::POPAD16 },
  /* 62 */  { BxAnother,  &BX_CPU_C::BOUND_GvMa },
  /* 63 */  { BxAnother,  &BX_CPU_C::ARPL_EwGw },
  /* 64 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // FS:
  /* 65 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // GS:
  /* 66 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // OS:
  /* 67 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // AS:
  /* 68 */  { BxImmediate_Iv,  &BX_CPU_C::PUSH_Iw },
  /* 69 */  { BxAnother | BxImmediate_Iv,  &BX_CPU_C::IMUL_GwEwIw },
  /* 6A */  { BxImmediate_Ib_SE,  &BX_CPU_C::PUSH_Iw },
  /* 6B */  { BxAnother | BxImmediate_Ib_SE,  &BX_CPU_C::IMUL_GwEwIw },
  /* 6C */  { BxRepeatable,  &BX_CPU_C::INSB_YbDX },
  /* 6D */  { BxRepeatable,  &BX_CPU_C::INSW_YvDX },
  /* 6E */  { BxRepeatable,  &BX_CPU_C::OUTSB_DXXb },
  /* 6F */  { BxRepeatable,  &BX_CPU_C::OUTSW_DXXv },
  /* 70 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 71 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 72 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 73 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 74 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 75 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 76 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 77 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 78 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 79 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7A */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7B */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7C */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7D */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7E */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 7F */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jw },
  /* 80 */  { BxAnother | BxGroup1, NULL, BxOpcodeInfoG1EbIb },
  /* 81 */  { BxAnother | BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfoG1Ew },
  /* 82 */  { BxAnother | BxGroup1,  NULL, BxOpcodeInfoG1EbIb },
  /* 83 */  { BxAnother | BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1Ew },
  /* 84 */  { BxAnother,  &BX_CPU_C::TEST_EbGb },
  /* 85 */  { BxAnother,  &BX_CPU_C::TEST_EwGw },
  /* 86 */  { BxAnother,  &BX_CPU_C::XCHG_EbGb },
  /* 87 */  { BxAnother,  &BX_CPU_C::XCHG_EwGw },
  /* 88 */  { BxAnother,  &BX_CPU_C::MOV_EbGb },
  /* 89 */  { BxAnother,  &BX_CPU_C::MOV_EwGw },
  /* 8A */  { BxAnother,  &BX_CPU_C::MOV_GbEb },
  /* 8B */  { BxAnother,  &BX_CPU_C::MOV_GwEw },
  /* 8C */  { BxAnother,  &BX_CPU_C::MOV_EwSw },
  /* 8D */  { BxAnother,  &BX_CPU_C::LEA_GwM },
  /* 8E */  { BxAnother,  &BX_CPU_C::MOV_SwEw },
  /* 8F */  { BxAnother,  &BX_CPU_C::POP_Ew },
  /* 90 */  { 0,  &BX_CPU_C::NOP },
  /* 91 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 92 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 93 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 94 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 95 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 96 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 97 */  { 0,  &BX_CPU_C::XCHG_RXAX },
  /* 98 */  { 0,  &BX_CPU_C::CBW },
  /* 99 */  { 0,  &BX_CPU_C::CWD },
  /* 9A */  { BxImmediate_IvIw,  &BX_CPU_C::CALL16_Ap },
  /* 9B */  { 0,  &BX_CPU_C::FWAIT },
  /* 9C */  { 0,  &BX_CPU_C::PUSHF_Fv },
  /* 9D */  { 0,  &BX_CPU_C::POPF_Fv },
  /* 9E */  { 0,  &BX_CPU_C::SAHF },
  /* 9F */  { 0,  &BX_CPU_C::LAHF },
  /* A0 */  { BxImmediate_O,  &BX_CPU_C::MOV_ALOb },
  /* A1 */  { BxImmediate_O,  &BX_CPU_C::MOV_AXOw },
  /* A2 */  { BxImmediate_O,  &BX_CPU_C::MOV_ObAL },
  /* A3 */  { BxImmediate_O,  &BX_CPU_C::MOV_OwAX },
  /* A4 */  { BxRepeatable,  &BX_CPU_C::MOVSB_XbYb },
  /* A5 */  { BxRepeatable,  &BX_CPU_C::MOVSW_XvYv },
  /* A6 */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::CMPSB_XbYb },
  /* A7 */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::CMPSW_XvYv },
  /* A8 */  { BxImmediate_Ib,  &BX_CPU_C::TEST_ALIb },
  /* A9 */  { BxImmediate_Iv,  &BX_CPU_C::TEST_AXIw },
  /* AA */  { BxRepeatable,  &BX_CPU_C::STOSB_YbAL },
  /* AB */  { BxRepeatable,  &BX_CPU_C::STOSW_YveAX },
  /* AC */  { BxRepeatable,  &BX_CPU_C::LODSB_ALXb },
  /* AD */  { BxRepeatable,  &BX_CPU_C::LODSW_eAXXv },
  /* AE */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::SCASB_ALXb },
  /* AF */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::SCASW_eAXXv },
  /* B0 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B1 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B2 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B3 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B4 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B5 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B6 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B7 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B8 */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* B9 */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BA */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BB */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BC */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BD */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BE */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* BF */  { BxImmediate_Iv,  &BX_CPU_C::MOV_RXIw },
  /* C0 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ew },
  /* C2 */  { BxImmediate_Iw,  &BX_CPU_C::RETnear16_Iw },
  /* C3 */  { 0,             &BX_CPU_C::RETnear16 },
  /* C4 */  { BxAnother,  &BX_CPU_C::LES_GvMp },
  /* C5 */  { BxAnother,  &BX_CPU_C::LDS_GvMp },
  /* C6 */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::MOV_EbIb },
  /* C7 */  { BxAnother | BxImmediate_Iv,  &BX_CPU_C::MOV_EwIw },
  /* C8 */  { BxImmediate_IwIb,  &BX_CPU_C::ENTER_IwIb },
  /* C9 */  { 0,  &BX_CPU_C::LEAVE },
  /* CA */  { BxImmediate_Iw,  &BX_CPU_C::RETfar16_Iw },
  /* CB */  { 0,  &BX_CPU_C::RETfar16 },
  /* CC */  { 0,  &BX_CPU_C::INT3 },
  /* CD */  { BxImmediate_Ib,  &BX_CPU_C::INT_Ib },
  /* CE */  { 0,  &BX_CPU_C::INTO },
  /* CF */  { 0,  &BX_CPU_C::IRET16 },
  /* D0 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Eb },
  /* D1 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Ew },
  /* D2 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Eb },
  /* D3 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Ew },
  /* D4 */  { BxImmediate_Ib,  &BX_CPU_C::AAM },
  /* D5 */  { BxImmediate_Ib,  &BX_CPU_C::AAD },
  /* D6 */  { 0,  &BX_CPU_C::SALC },
  /* D7 */  { 0,  &BX_CPU_C::XLAT },
  /* D8 */  { BxAnother,  &BX_CPU_C::ESC0 },
  /* D9 */  { BxAnother,  &BX_CPU_C::ESC1 },
  /* DA */  { BxAnother,  &BX_CPU_C::ESC2 },
  /* DB */  { BxAnother,  &BX_CPU_C::ESC3 },
  /* DC */  { BxAnother,  &BX_CPU_C::ESC4 },
  /* DD */  { BxAnother,  &BX_CPU_C::ESC5 },
  /* DE */  { BxAnother,  &BX_CPU_C::ESC6 },
  /* DF */  { BxAnother,  &BX_CPU_C::ESC7 },
  /* E0 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOPNE_Jb },
  /* E1 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOPE_Jb },
  /* E2 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOP_Jb },
  /* E3 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCXZ_Jb },
  /* E4 */  { BxImmediate_Ib,  &BX_CPU_C::IN_ALIb },
  /* E5 */  { BxImmediate_Ib,  &BX_CPU_C::IN_eAXIb },
  /* E6 */  { BxImmediate_Ib,  &BX_CPU_C::OUT_IbAL },
  /* E7 */  { BxImmediate_Ib,  &BX_CPU_C::OUT_IbeAX },
  /* E8 */  { BxImmediate_BrOff16,  &BX_CPU_C::CALL_Aw },
  /* E9 */  { BxImmediate_BrOff16,  &BX_CPU_C::JMP_Jw },
  /* EA */  { BxImmediate_IvIw,  &BX_CPU_C::JMP_Ap },
  /* EB */  { BxImmediate_BrOff8,  &BX_CPU_C::JMP_Jw },
  /* EC */  { 0,  &BX_CPU_C::IN_ALDX },
  /* ED */  { 0,  &BX_CPU_C::IN_eAXDX },
  /* EE */  { 0,  &BX_CPU_C::OUT_DXAL },
  /* EF */  { 0,  &BX_CPU_C::OUT_DXeAX },
  /* F0 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // LOCK
  /* F1 */  { 0,  &BX_CPU_C::INT1 },
  /* F2 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // REP, REPE/REPZ
  /* F4 */  { 0,  &BX_CPU_C::HLT },
  /* F5 */  { 0,  &BX_CPU_C::CMC },
  /* F6 */  { BxAnother | BxGroup3,  NULL, BxOpcodeInfoG3Eb },
  /* F7 */  { BxAnother | BxGroup3,  NULL, BxOpcodeInfoG3Ew },
  /* F8 */  { 0,  &BX_CPU_C::CLC },
  /* F9 */  { 0,  &BX_CPU_C::STC },
  /* FA */  { 0,  &BX_CPU_C::CLI },
  /* FB */  { 0,  &BX_CPU_C::STI },
  /* FC */  { 0,  &BX_CPU_C::CLD },
  /* FD */  { 0,  &BX_CPU_C::STD },
  /* FE */  { BxAnother | BxGroup4,  NULL, BxOpcodeInfoG4 },
  /* FF */  { BxAnother | BxGroup5,  NULL, BxOpcodeInfoG5w },

  /* 0F 00 */  { BxAnother | BxGroup6,  NULL, BxOpcodeInfoG6 },
  /* 0F 01 */  { BxAnother | BxGroup7,  NULL, BxOpcodeInfoG7 },
  /* 0F 02 */  { BxAnother,  &BX_CPU_C::LAR_GvEw },
  /* 0F 03 */  { BxAnother,  &BX_CPU_C::LSL_GvEw },
  /* 0F 04 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 05 */  { 0,  &BX_CPU_C::LOADALL },
  /* 0F 06 */  { 0,  &BX_CPU_C::CLTS },
  /* 0F 07 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 08 */  { 0,  &BX_CPU_C::INVD },
  /* 0F 09 */  { 0,  &BX_CPU_C::WBINVD },
  /* 0F 0A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 10 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 11 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 12 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 13 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 14 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 15 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 16 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 17 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 18 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 19 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 20 */  { BxAnother,  &BX_CPU_C::MOV_RdCd },
  /* 0F 21 */  { BxAnother,  &BX_CPU_C::MOV_RdDd },
  /* 0F 22 */  { BxAnother,  &BX_CPU_C::MOV_CdRd },
  /* 0F 23 */  { BxAnother,  &BX_CPU_C::MOV_DdRd },
  /* 0F 24 */  { BxAnother,  &BX_CPU_C::MOV_RdTd },
  /* 0F 25 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 26 */  { BxAnother,  &BX_CPU_C::MOV_TdRd },
  /* 0F 27 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 28 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 29 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 30 */  { 0,  &BX_CPU_C::WRMSR },
  /* 0F 31 */  { 0,  &BX_CPU_C::RDTSC },
  /* 0F 32 */  { 0,  &BX_CPU_C::RDMSR },
  /* 0F 33 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 34 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 35 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 36 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 37 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 38 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 39 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 40 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 41 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 42 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 43 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 44 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 45 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 46 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 47 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 48 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 49 */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4A */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4B */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4C */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4D */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4E */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 4F */  { BxAnother,  &BX_CPU_C::CMOV_GwEw },
  /* 0F 50 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 51 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 52 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 53 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 54 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 55 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 56 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 57 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 58 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 59 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 60 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 61 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 62 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 63 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 64 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 65 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 66 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 67 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 68 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 69 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 70 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 71 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 72 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 73 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 74 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 75 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 76 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 77 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 78 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 79 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 80 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 81 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 82 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 83 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 84 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 85 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 86 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 87 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 88 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 89 */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8A */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8B */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8C */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8D */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8E */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 8F */  { BxImmediate_BrOff16,  &BX_CPU_C::JCC_Jw },
  /* 0F 90 */  { BxAnother,  &BX_CPU_C::SETO_Eb },
  /* 0F 91 */  { BxAnother,  &BX_CPU_C::SETNO_Eb },
  /* 0F 92 */  { BxAnother,  &BX_CPU_C::SETB_Eb },
  /* 0F 93 */  { BxAnother,  &BX_CPU_C::SETNB_Eb },
  /* 0F 94 */  { BxAnother,  &BX_CPU_C::SETZ_Eb },
  /* 0F 95 */  { BxAnother,  &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 */  { BxAnother,  &BX_CPU_C::SETBE_Eb },
  /* 0F 97 */  { BxAnother,  &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 */  { BxAnother,  &BX_CPU_C::SETS_Eb },
  /* 0F 99 */  { BxAnother,  &BX_CPU_C::SETNS_Eb },
  /* 0F 9A */  { BxAnother,  &BX_CPU_C::SETP_Eb },
  /* 0F 9B */  { BxAnother,  &BX_CPU_C::SETNP_Eb },
  /* 0F 9C */  { BxAnother,  &BX_CPU_C::SETL_Eb },
  /* 0F 9D */  { BxAnother,  &BX_CPU_C::SETNL_Eb },
  /* 0F 9E */  { BxAnother,  &BX_CPU_C::SETLE_Eb },
  /* 0F 9F */  { BxAnother,  &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 */  { 0,  &BX_CPU_C::PUSH_FS },
  /* 0F A1 */  { 0,  &BX_CPU_C::POP_FS },
  /* 0F A2 */  { 0,  &BX_CPU_C::CPUID },
  /* 0F A3 */  { BxAnother,  &BX_CPU_C::BT_EvGv },
  /* 0F A4 */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::SHLD_EwGw },
  /* 0F A5 */  { BxAnother,                 &BX_CPU_C::SHLD_EwGw },
  /* 0F A6 */  { 0,  &BX_CPU_C::CMPXCHG_XBTS },
  /* 0F A7 */  { 0,  &BX_CPU_C::CMPXCHG_IBTS },
  /* 0F A8 */  { 0,  &BX_CPU_C::PUSH_GS },
  /* 0F A9 */  { 0,  &BX_CPU_C::POP_GS },
  /* 0F AA */  { 0,  &BX_CPU_C::RSM },
  /* 0F AB */  { BxAnother,  &BX_CPU_C::BTS_EvGv },
  /* 0F AC */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::SHRD_EwGw },
  /* 0F AD */  { BxAnother,                 &BX_CPU_C::SHRD_EwGw },
  /* 0F AE */  { 0,  &BX_CPU_C::BxError },
  /* 0F AF */  { BxAnother,  &BX_CPU_C::IMUL_GwEw },
  /* 0F B0 */  { BxAnother,  &BX_CPU_C::CMPXCHG_EbGb },
  /* 0F B1 */  { BxAnother,  &BX_CPU_C::CMPXCHG_EwGw },
  /* 0F B2 */  { BxAnother,  &BX_CPU_C::LSS_GvMp },
  /* 0F B3 */  { BxAnother,  &BX_CPU_C::BTR_EvGv },
  /* 0F B4 */  { BxAnother,  &BX_CPU_C::LFS_GvMp },
  /* 0F B5 */  { BxAnother,  &BX_CPU_C::LGS_GvMp },
  /* 0F B6 */  { BxAnother,  &BX_CPU_C::MOVZX_GwEb },
  /* 0F B7 */  { BxAnother,  &BX_CPU_C::MOVZX_GwEw },
  /* 0F B8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F B9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F BA */  { BxAnother | BxGroup8, NULL, BxOpcodeInfoG8EvIb },
  /* 0F BB */  { BxAnother,  &BX_CPU_C::BTC_EvGv },
  /* 0F BC */  { BxAnother,  &BX_CPU_C::BSF_GvEv },
  /* 0F BD */  { BxAnother,  &BX_CPU_C::BSR_GvEv },
  /* 0F BE */  { BxAnother,  &BX_CPU_C::MOVSX_GwEb },
  /* 0F BF */  { BxAnother,  &BX_CPU_C::MOVSX_GwEw },
  /* 0F C0 */  { BxAnother,  &BX_CPU_C::XADD_EbGb },
  /* 0F C1 */  { BxAnother,  &BX_CPU_C::XADD_EwGw },
  /* 0F C2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C7 */  { BxAnother | BxGroup9,  NULL, BxOpcodeInfoG9 },
  /* 0F C8 */  { 0,  &BX_CPU_C::BSWAP_EAX },
  /* 0F C9 */  { 0,  &BX_CPU_C::BSWAP_ECX },
  /* 0F CA */  { 0,  &BX_CPU_C::BSWAP_EDX },
  /* 0F CB */  { 0,  &BX_CPU_C::BSWAP_EBX },
  /* 0F CC */  { 0,  &BX_CPU_C::BSWAP_ESP },
  /* 0F CD */  { 0,  &BX_CPU_C::BSWAP_EBP },
  /* 0F CE */  { 0,  &BX_CPU_C::BSWAP_ESI },
  /* 0F CF */  { 0,  &BX_CPU_C::BSWAP_EDI },
  /* 0F D0 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D1 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D7 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F DA */  { 0,  &BX_CPU_C::BxError },
  /* 0F DB */  { 0,  &BX_CPU_C::BxError },
  /* 0F DC */  { 0,  &BX_CPU_C::BxError },
  /* 0F DD */  { 0,  &BX_CPU_C::BxError },
  /* 0F DE */  { 0,  &BX_CPU_C::BxError },
  /* 0F DF */  { 0,  &BX_CPU_C::BxError },
  /* 0F E0 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E1 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E7 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F EA */  { 0,  &BX_CPU_C::BxError },
  /* 0F EB */  { 0,  &BX_CPU_C::BxError },
  /* 0F EC */  { 0,  &BX_CPU_C::BxError },
  /* 0F ED */  { 0,  &BX_CPU_C::BxError },
  /* 0F EE */  { 0,  &BX_CPU_C::BxError },
  /* 0F EF */  { 0,  &BX_CPU_C::BxError },
  /* 0F F0 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F1 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F2 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F3 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F4 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F5 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F6 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F7 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F8 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F9 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FA */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FB */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FC */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FD */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FE */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FF */  { 0,  &BX_CPU_C::UndefinedOpcode },

  // 512 entries for 32bit mod
  /* 00 */  { BxAnother,  &BX_CPU_C::ADD_EbGb },
  /* 01 */  { BxAnother,  &BX_CPU_C::ADD_EdGd },
  /* 02 */  { BxAnother,  &BX_CPU_C::ADD_GbEb },
  /* 03 */  { BxAnother,  &BX_CPU_C::ADD_GdEd },
  /* 04 */  { BxImmediate_Ib,  &BX_CPU_C::ADD_ALIb },
  /* 05 */  { BxImmediate_Iv,  &BX_CPU_C::ADD_EAXId },
  /* 06 */  { 0,  &BX_CPU_C::PUSH_ES },
  /* 07 */  { 0,  &BX_CPU_C::POP_ES },
  /* 08 */  { BxAnother,  &BX_CPU_C::OR_EbGb },
  /* 09 */  { BxAnother,  &BX_CPU_C::OR_EdGd },
  /* 0A */  { BxAnother,  &BX_CPU_C::OR_GbEb },
  /* 0B */  { BxAnother,  &BX_CPU_C::OR_GdEd },
  /* 0C */  { BxImmediate_Ib,  &BX_CPU_C::OR_ALIb },
  /* 0D */  { BxImmediate_Iv,  &BX_CPU_C::OR_EAXId },
  /* 0E */  { 0,  &BX_CPU_C::PUSH_CS },
  /* 0F */  { BxAnother,  &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 */  { BxAnother,  &BX_CPU_C::ADC_EbGb },
  /* 11 */  { BxAnother,  &BX_CPU_C::ADC_EdGd },
  /* 12 */  { BxAnother,  &BX_CPU_C::ADC_GbEb },
  /* 13 */  { BxAnother,  &BX_CPU_C::ADC_GdEd },
  /* 14 */  { BxImmediate_Ib,  &BX_CPU_C::ADC_ALIb },
  /* 15 */  { BxImmediate_Iv,  &BX_CPU_C::ADC_EAXId },
  /* 16 */  { 0,  &BX_CPU_C::PUSH_SS },
  /* 17 */  { 0,  &BX_CPU_C::POP_SS },
  /* 18 */  { BxAnother,  &BX_CPU_C::SBB_EbGb },
  /* 19 */  { BxAnother,  &BX_CPU_C::SBB_EdGd },
  /* 1A */  { BxAnother,  &BX_CPU_C::SBB_GbEb },
  /* 1B */  { BxAnother,  &BX_CPU_C::SBB_GdEd },
  /* 1C */  { BxImmediate_Ib,  &BX_CPU_C::SBB_ALIb },
  /* 1D */  { BxImmediate_Iv,  &BX_CPU_C::SBB_EAXId },
  /* 1E */  { 0,  &BX_CPU_C::PUSH_DS },
  /* 1F */  { 0,  &BX_CPU_C::POP_DS },
  /* 20 */  { BxAnother,  &BX_CPU_C::AND_EbGb },
  /* 21 */  { BxAnother,  &BX_CPU_C::AND_EdGd },
  /* 22 */  { BxAnother,  &BX_CPU_C::AND_GbEb },
  /* 23 */  { BxAnother,  &BX_CPU_C::AND_GdEd },
  /* 24 */  { BxImmediate_Ib,  &BX_CPU_C::AND_ALIb },
  /* 25 */  { BxImmediate_Iv,  &BX_CPU_C::AND_EAXId },
  /* 26 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // ES:
  /* 27 */  { 0,  &BX_CPU_C::DAA },
  /* 28 */  { BxAnother,  &BX_CPU_C::SUB_EbGb },
  /* 29 */  { BxAnother,  &BX_CPU_C::SUB_EdGd },
  /* 2A */  { BxAnother,  &BX_CPU_C::SUB_GbEb },
  /* 2B */  { BxAnother,  &BX_CPU_C::SUB_GdEd },
  /* 2C */  { BxImmediate_Ib,  &BX_CPU_C::SUB_ALIb },
  /* 2D */  { BxImmediate_Iv,  &BX_CPU_C::SUB_EAXId },
  /* 2E */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // CS:
  /* 2F */  { 0,  &BX_CPU_C::DAS },
  /* 30 */  { BxAnother,  &BX_CPU_C::XOR_EbGb },
  /* 31 */  { BxAnother,  &BX_CPU_C::XOR_EdGd },
  /* 32 */  { BxAnother,  &BX_CPU_C::XOR_GbEb },
  /* 33 */  { BxAnother,  &BX_CPU_C::XOR_GdEd },
  /* 34 */  { BxImmediate_Ib,  &BX_CPU_C::XOR_ALIb },
  /* 35 */  { BxImmediate_Iv,  &BX_CPU_C::XOR_EAXId },
  /* 36 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // SS:
  /* 37 */  { 0,  &BX_CPU_C::AAA },
  /* 38 */  { BxAnother,  &BX_CPU_C::CMP_EbGb },
  /* 39 */  { BxAnother,  &BX_CPU_C::CMP_EdGd },
  /* 3A */  { BxAnother,  &BX_CPU_C::CMP_GbEb },
  /* 3B */  { BxAnother,  &BX_CPU_C::CMP_GdEd },
  /* 3C */  { BxImmediate_Ib,  &BX_CPU_C::CMP_ALIb },
  /* 3D */  { BxImmediate_Iv,  &BX_CPU_C::CMP_EAXId },
  /* 3E */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // DS:
  /* 3F */  { 0,  &BX_CPU_C::AAS },
  /* 40 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 41 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 42 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 43 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 44 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 45 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 46 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 47 */  { 0,  &BX_CPU_C::INC_ERX },
  /* 48 */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 49 */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4A */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4B */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4C */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4D */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4E */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 4F */  { 0,  &BX_CPU_C::DEC_ERX },
  /* 50 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 51 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 52 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 53 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 54 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 55 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 56 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 57 */  { 0,  &BX_CPU_C::PUSH_ERX },
  /* 58 */  { 0,  &BX_CPU_C::POP_ERX },
  /* 59 */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5A */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5B */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5C */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5D */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5E */  { 0,  &BX_CPU_C::POP_ERX },
  /* 5F */  { 0,  &BX_CPU_C::POP_ERX },
  /* 60 */  { 0,  &BX_CPU_C::PUSHAD32 },
  /* 61 */  { 0,  &BX_CPU_C::POPAD32 },
  /* 62 */  { BxAnother,  &BX_CPU_C::BOUND_GvMa },
  /* 63 */  { BxAnother,  &BX_CPU_C::ARPL_EwGw },
  /* 64 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // FS:
  /* 65 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // GS:
  /* 66 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // OS:
  /* 67 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // AS:
  /* 68 */  { BxImmediate_Iv,  &BX_CPU_C::PUSH_Id },
  /* 69 */  { BxAnother | BxImmediate_Iv,  &BX_CPU_C::IMUL_GdEdId },
  /* 6A */  { BxImmediate_Ib_SE,  &BX_CPU_C::PUSH_Id },
  /* 6B */  { BxAnother | BxImmediate_Ib_SE,  &BX_CPU_C::IMUL_GdEdId },
  /* 6C */  { BxRepeatable,  &BX_CPU_C::INSB_YbDX },
  /* 6D */  { BxRepeatable,  &BX_CPU_C::INSW_YvDX },
  /* 6E */  { BxRepeatable,  &BX_CPU_C::OUTSB_DXXb },
  /* 6F */  { BxRepeatable,  &BX_CPU_C::OUTSW_DXXv },
  /* 70 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 71 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 72 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 73 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 74 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 75 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 76 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 77 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 78 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 79 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7A */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7B */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7C */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7D */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7E */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 7F */  { BxImmediate_BrOff8,  &BX_CPU_C::JCC_Jd },
  /* 80 */  { BxAnother | BxGroup1,  NULL, BxOpcodeInfoG1EbIb },
  /* 81 */  { BxAnother | BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfoG1Ed },
  /* 82 */  { BxAnother | BxGroup1,  NULL, BxOpcodeInfoG1EbIb },
  /* 83 */  { BxAnother | BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfoG1Ed },
  /* 84 */  { BxAnother,  &BX_CPU_C::TEST_EbGb },
  /* 85 */  { BxAnother,  &BX_CPU_C::TEST_EdGd },
  /* 86 */  { BxAnother,  &BX_CPU_C::XCHG_EbGb },
  /* 87 */  { BxAnother,  &BX_CPU_C::XCHG_EdGd },
  /* 88 */  { BxAnother,  &BX_CPU_C::MOV_EbGb },
  /* 89 */  { BxAnother,  &BX_CPU_C::MOV_EdGd },
  /* 8A */  { BxAnother,  &BX_CPU_C::MOV_GbEb },
  /* 8B */  { BxAnother,  &BX_CPU_C::MOV_GdEd },
  /* 8C */  { BxAnother,  &BX_CPU_C::MOV_EwSw },
  /* 8D */  { BxAnother,  &BX_CPU_C::LEA_GdM },
  /* 8E */  { BxAnother,  &BX_CPU_C::MOV_SwEw },
  /* 8F */  { BxAnother,  &BX_CPU_C::POP_Ed },
  /* 90 */  { 0,  &BX_CPU_C::NOP },
  /* 91 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 92 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 93 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 94 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 95 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 96 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 97 */  { 0,  &BX_CPU_C::XCHG_ERXEAX },
  /* 98 */  { 0,  &BX_CPU_C::CWDE },
  /* 99 */  { 0,  &BX_CPU_C::CDQ },
  /* 9A */  { BxImmediate_IvIw,  &BX_CPU_C::CALL32_Ap },
  /* 9B */  { 0,  &BX_CPU_C::FWAIT },
  /* 9C */  { 0,  &BX_CPU_C::PUSHF_Fv },
  /* 9D */  { 0,  &BX_CPU_C::POPF_Fv },
  /* 9E */  { 0,  &BX_CPU_C::SAHF },
  /* 9F */  { 0,  &BX_CPU_C::LAHF },
  /* A0 */  { BxImmediate_O,  &BX_CPU_C::MOV_ALOb },
  /* A1 */  { BxImmediate_O,  &BX_CPU_C::MOV_EAXOd },
  /* A2 */  { BxImmediate_O,  &BX_CPU_C::MOV_ObAL },
  /* A3 */  { BxImmediate_O,  &BX_CPU_C::MOV_OdEAX },
  /* A4 */  { BxRepeatable,  &BX_CPU_C::MOVSB_XbYb },
  /* A5 */  { BxRepeatable,  &BX_CPU_C::MOVSW_XvYv },
  /* A6 */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::CMPSB_XbYb },
  /* A7 */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::CMPSW_XvYv },
  /* A8 */  { BxImmediate_Ib,  &BX_CPU_C::TEST_ALIb },
  /* A9 */  { BxImmediate_Iv,  &BX_CPU_C::TEST_EAXId },
  /* AA */  { BxRepeatable,  &BX_CPU_C::STOSB_YbAL },
  /* AB */  { BxRepeatable,  &BX_CPU_C::STOSW_YveAX },
  /* AC */  { BxRepeatable,  &BX_CPU_C::LODSB_ALXb },
  /* AD */  { BxRepeatable,  &BX_CPU_C::LODSW_eAXXv },
  /* AE */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::SCASB_ALXb },
  /* AF */  { BxRepeatable | BxRepeatableZF,  &BX_CPU_C::SCASW_eAXXv },
  /* B0 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B1 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B2 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B3 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RLIb },
  /* B4 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B5 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B6 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B7 */  { BxImmediate_Ib,  &BX_CPU_C::MOV_RHIb },
  /* B8 */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* B9 */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BA */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BB */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BC */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BD */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BE */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* BF */  { BxImmediate_Iv,  &BX_CPU_C::MOV_ERXId },
  /* C0 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Eb },
  /* C1 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfoG2Ed },
  /* C2 */  { BxImmediate_Iw,  &BX_CPU_C::RETnear32_Iw },
  /* C3 */  { 0,             &BX_CPU_C::RETnear32 },
  /* C4 */  { BxAnother,  &BX_CPU_C::LES_GvMp },
  /* C5 */  { BxAnother,  &BX_CPU_C::LDS_GvMp },
  /* C6 */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::MOV_EbIb },
  /* C7 */  { BxAnother | BxImmediate_Iv,  &BX_CPU_C::MOV_EdId },
  /* C8 */  { BxImmediate_IwIb,  &BX_CPU_C::ENTER_IwIb },
  /* C9 */  { 0,  &BX_CPU_C::LEAVE },
  /* CA */  { BxImmediate_Iw,  &BX_CPU_C::RETfar32_Iw },
  /* CB */  { 0,  &BX_CPU_C::RETfar32 },
  /* CC */  { 0,  &BX_CPU_C::INT3 },
  /* CD */  { BxImmediate_Ib,  &BX_CPU_C::INT_Ib },
  /* CE */  { 0,  &BX_CPU_C::INTO },
  /* CF */  { 0,  &BX_CPU_C::IRET32 },
  /* D0 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Eb },
  /* D1 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Ed },
  /* D2 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Eb },
  /* D3 */  { BxAnother | BxGroup2,  NULL, BxOpcodeInfoG2Ed },
  /* D4 */  { BxImmediate_Ib,  &BX_CPU_C::AAM },
  /* D5 */  { BxImmediate_Ib,  &BX_CPU_C::AAD },
  /* D6 */  { 0,  &BX_CPU_C::SALC },
  /* D7 */  { 0,  &BX_CPU_C::XLAT },
  /* D8 */  { BxAnother,  &BX_CPU_C::ESC0 },
  /* D9 */  { BxAnother,  &BX_CPU_C::ESC1 },
  /* DA */  { BxAnother,  &BX_CPU_C::ESC2 },
  /* DB */  { BxAnother,  &BX_CPU_C::ESC3 },
  /* DC */  { BxAnother,  &BX_CPU_C::ESC4 },
  /* DD */  { BxAnother,  &BX_CPU_C::ESC5 },
  /* DE */  { BxAnother,  &BX_CPU_C::ESC6 },
  /* DF */  { BxAnother,  &BX_CPU_C::ESC7 },
  /* E0 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOPNE_Jb },
  /* E1 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOPE_Jb },
  /* E2 */  { BxImmediate_BrOff8,  &BX_CPU_C::LOOP_Jb },
  /* E3 */  { BxImmediate_BrOff8,  &BX_CPU_C::JCXZ_Jb },
  /* E4 */  { BxImmediate_Ib,  &BX_CPU_C::IN_ALIb },
  /* E5 */  { BxImmediate_Ib,  &BX_CPU_C::IN_eAXIb },
  /* E6 */  { BxImmediate_Ib,  &BX_CPU_C::OUT_IbAL },
  /* E7 */  { BxImmediate_Ib,  &BX_CPU_C::OUT_IbeAX },
  /* E8 */  { BxImmediate_BrOff32,  &BX_CPU_C::CALL_Ad },
  /* E9 */  { BxImmediate_BrOff32,  &BX_CPU_C::JMP_Jd },
  /* EA */  { BxImmediate_IvIw,  &BX_CPU_C::JMP_Ap },
  /* EB */  { BxImmediate_BrOff8,  &BX_CPU_C::JMP_Jd },
  /* EC */  { 0,  &BX_CPU_C::IN_ALDX },
  /* ED */  { 0,  &BX_CPU_C::IN_eAXDX },
  /* EE */  { 0,  &BX_CPU_C::OUT_DXAL },
  /* EF */  { 0,  &BX_CPU_C::OUT_DXeAX },
  /* F0 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // LOCK:
  /* F1 */  { 0,  &BX_CPU_C::INT1 },
  /* F2 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 */  { BxPrefix | BxAnother,  &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 */  { 0,  &BX_CPU_C::HLT },
  /* F5 */  { 0,  &BX_CPU_C::CMC },
  /* F6 */  { BxAnother | BxGroup3,  NULL, BxOpcodeInfoG3Eb },
  /* F7 */  { BxAnother | BxGroup3,  NULL, BxOpcodeInfoG3Ed },
  /* F8 */  { 0,  &BX_CPU_C::CLC },
  /* F9 */  { 0,  &BX_CPU_C::STC },
  /* FA */  { 0,  &BX_CPU_C::CLI },
  /* FB */  { 0,  &BX_CPU_C::STI },
  /* FC */  { 0,  &BX_CPU_C::CLD },
  /* FD */  { 0,  &BX_CPU_C::STD },
  /* FE */  { BxAnother | BxGroup4,  NULL, BxOpcodeInfoG4 },
  /* FF */  { BxAnother | BxGroup5,  NULL, BxOpcodeInfoG5d },

  /* 0F 00 */  { BxAnother | BxGroup6,  NULL, BxOpcodeInfoG6 },
  /* 0F 01 */  { BxAnother | BxGroup7,  NULL, BxOpcodeInfoG7 },
  /* 0F 02 */  { BxAnother,  &BX_CPU_C::LAR_GvEw },
  /* 0F 03 */  { BxAnother,  &BX_CPU_C::LSL_GvEw },
  /* 0F 04 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 05 */  { 0,  &BX_CPU_C::LOADALL },
  /* 0F 06 */  { 0,  &BX_CPU_C::CLTS },
  /* 0F 07 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 08 */  { 0,  &BX_CPU_C::INVD },
  /* 0F 09 */  { 0,  &BX_CPU_C::WBINVD },
  /* 0F 0A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 0F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 10 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 11 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 12 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 13 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 14 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 15 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 16 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 17 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 18 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 19 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 1F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 20 */  { BxAnother,  &BX_CPU_C::MOV_RdCd },
  /* 0F 21 */  { BxAnother,  &BX_CPU_C::MOV_RdDd },
  /* 0F 22 */  { BxAnother,  &BX_CPU_C::MOV_CdRd },
  /* 0F 23 */  { BxAnother,  &BX_CPU_C::MOV_DdRd },
  /* 0F 24 */  { BxAnother,  &BX_CPU_C::MOV_RdTd },
  /* 0F 25 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 26 */  { BxAnother,  &BX_CPU_C::MOV_TdRd },
  /* 0F 27 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 28 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 29 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 2F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 30 */  { 0,  &BX_CPU_C::WRMSR },
  /* 0F 31 */  { 0,  &BX_CPU_C::RDTSC },
  /* 0F 32 */  { 0,  &BX_CPU_C::RDMSR },
  /* 0F 33 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 34 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 35 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 36 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 37 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 38 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 39 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 3F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 40 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 41 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 42 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 43 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 44 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 45 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 46 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 47 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 48 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 49 */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4A */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4B */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4C */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4D */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4E */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 4F */  { BxAnother,  &BX_CPU_C::CMOV_GdEd },
  /* 0F 50 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 51 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 52 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 53 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 54 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 55 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 56 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 57 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 58 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 59 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 5F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 60 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 61 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 62 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 63 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 64 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 65 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 66 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 67 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 68 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 69 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 6F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 70 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 71 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 72 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 73 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 74 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 75 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 76 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 77 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 78 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 79 */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7A */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7B */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7C */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7D */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7E */  { 0,  &BX_CPU_C::BxError },
  /* 0F 7F */  { 0,  &BX_CPU_C::BxError },
  /* 0F 80 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 81 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 82 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 83 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 84 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 85 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 86 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 87 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 88 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 89 */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8A */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8B */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8C */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8D */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8E */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 8F */  { BxImmediate_BrOff32,  &BX_CPU_C::JCC_Jd },
  /* 0F 90 */  { BxAnother,  &BX_CPU_C::SETO_Eb },
  /* 0F 91 */  { BxAnother,  &BX_CPU_C::SETNO_Eb },
  /* 0F 92 */  { BxAnother,  &BX_CPU_C::SETB_Eb },
  /* 0F 93 */  { BxAnother,  &BX_CPU_C::SETNB_Eb },
  /* 0F 94 */  { BxAnother,  &BX_CPU_C::SETZ_Eb },
  /* 0F 95 */  { BxAnother,  &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 */  { BxAnother,  &BX_CPU_C::SETBE_Eb },
  /* 0F 97 */  { BxAnother,  &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 */  { BxAnother,  &BX_CPU_C::SETS_Eb },
  /* 0F 99 */  { BxAnother,  &BX_CPU_C::SETNS_Eb },
  /* 0F 9A */  { BxAnother,  &BX_CPU_C::SETP_Eb },
  /* 0F 9B */  { BxAnother,  &BX_CPU_C::SETNP_Eb },
  /* 0F 9C */  { BxAnother,  &BX_CPU_C::SETL_Eb },
  /* 0F 9D */  { BxAnother,  &BX_CPU_C::SETNL_Eb },
  /* 0F 9E */  { BxAnother,  &BX_CPU_C::SETLE_Eb },
  /* 0F 9F */  { BxAnother,  &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 */  { 0,  &BX_CPU_C::PUSH_FS },
  /* 0F A1 */  { 0,  &BX_CPU_C::POP_FS },
  /* 0F A2 */  { 0,  &BX_CPU_C::CPUID },
  /* 0F A3 */  { BxAnother,  &BX_CPU_C::BT_EvGv },
  /* 0F A4 */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::SHLD_EdGd },
  /* 0F A5 */  { BxAnother,                 &BX_CPU_C::SHLD_EdGd },
  /* 0F A6 */  { 0,  &BX_CPU_C::CMPXCHG_XBTS },
  /* 0F A7 */  { 0,  &BX_CPU_C::CMPXCHG_IBTS },
  /* 0F A8 */  { 0,  &BX_CPU_C::PUSH_GS },
  /* 0F A9 */  { 0,  &BX_CPU_C::POP_GS },
  /* 0F AA */  { 0,  &BX_CPU_C::RSM },
  /* 0F AB */  { BxAnother,  &BX_CPU_C::BTS_EvGv },
  /* 0F AC */  { BxAnother | BxImmediate_Ib,  &BX_CPU_C::SHRD_EdGd },
  /* 0F AD */  { BxAnother,                 &BX_CPU_C::SHRD_EdGd },
  /* 0F AE */  { 0,  &BX_CPU_C::BxError },
  /* 0F AF */  { BxAnother,  &BX_CPU_C::IMUL_GdEd },
  /* 0F B0 */  { BxAnother,  &BX_CPU_C::CMPXCHG_EbGb },
  /* 0F B1 */  { BxAnother,  &BX_CPU_C::CMPXCHG_EdGd },
  /* 0F B2 */  { BxAnother,  &BX_CPU_C::LSS_GvMp },
  /* 0F B3 */  { BxAnother,  &BX_CPU_C::BTR_EvGv },
  /* 0F B4 */  { BxAnother,  &BX_CPU_C::LFS_GvMp },
  /* 0F B5 */  { BxAnother,  &BX_CPU_C::LGS_GvMp },
  /* 0F B6 */  { BxAnother,  &BX_CPU_C::MOVZX_GdEb },
  /* 0F B7 */  { BxAnother,  &BX_CPU_C::MOVZX_GdEw },
  /* 0F B8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F B9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F BA */  { BxAnother | BxGroup8,  NULL, BxOpcodeInfoG8EvIb },
  /* 0F BB */  { BxAnother,  &BX_CPU_C::BTC_EvGv },
  /* 0F BC */  { BxAnother,  &BX_CPU_C::BSF_GvEv },
  /* 0F BD */  { BxAnother,  &BX_CPU_C::BSR_GvEv },
  /* 0F BE */  { BxAnother,  &BX_CPU_C::MOVSX_GdEb },
  /* 0F BF */  { BxAnother,  &BX_CPU_C::MOVSX_GdEw },
  /* 0F C0 */  { BxAnother,  &BX_CPU_C::XADD_EbGb },
  /* 0F C1 */  { BxAnother,  &BX_CPU_C::XADD_EdGd },
  /* 0F C2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F C7 */  { BxAnother | BxGroup9,  NULL, BxOpcodeInfoG9 },
  /* 0F C8 */  { 0,  &BX_CPU_C::BSWAP_EAX },
  /* 0F C9 */  { 0,  &BX_CPU_C::BSWAP_ECX },
  /* 0F CA */  { 0,  &BX_CPU_C::BSWAP_EDX },
  /* 0F CB */  { 0,  &BX_CPU_C::BSWAP_EBX },
  /* 0F CC */  { 0,  &BX_CPU_C::BSWAP_ESP },
  /* 0F CD */  { 0,  &BX_CPU_C::BSWAP_EBP },
  /* 0F CE */  { 0,  &BX_CPU_C::BSWAP_ESI },
  /* 0F CF */  { 0,  &BX_CPU_C::BSWAP_EDI },
  /* 0F D0 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D1 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D7 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F D9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F DA */  { 0,  &BX_CPU_C::BxError },
  /* 0F DB */  { 0,  &BX_CPU_C::BxError },
  /* 0F DC */  { 0,  &BX_CPU_C::BxError },
  /* 0F DD */  { 0,  &BX_CPU_C::BxError },
  /* 0F DE */  { 0,  &BX_CPU_C::BxError },
  /* 0F DF */  { 0,  &BX_CPU_C::BxError },
  /* 0F E0 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E1 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E2 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E3 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E4 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E5 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E6 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E7 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E8 */  { 0,  &BX_CPU_C::BxError },
  /* 0F E9 */  { 0,  &BX_CPU_C::BxError },
  /* 0F EA */  { 0,  &BX_CPU_C::BxError },
  /* 0F EB */  { 0,  &BX_CPU_C::BxError },
  /* 0F EC */  { 0,  &BX_CPU_C::BxError },
  /* 0F ED */  { 0,  &BX_CPU_C::BxError },
  /* 0F EE */  { 0,  &BX_CPU_C::BxError },
  /* 0F EF */  { 0,  &BX_CPU_C::BxError },
  /* 0F F0 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F1 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F2 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F3 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F4 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F5 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F6 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F7 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F8 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F F9 */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FA */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FB */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FC */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FD */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FE */  { 0,  &BX_CPU_C::UndefinedOpcode },
  /* 0F FF */  { 0,  &BX_CPU_C::UndefinedOpcode },
  };




  unsigned
BX_CPU_C::FetchDecode(Bit8u *iptr, BxInstruction_t *instruction,
                      unsigned remain, Boolean is_32)
{
  // remain must be at least 1

  unsigned b1, b2, ilen=1, attr;
  unsigned imm_mode, offset;

  instruction->os_32 = instruction->as_32 = is_32;
  instruction->ResolveModrm = NULL;
  instruction->seg = BX_SEG_REG_NULL;
  instruction->rep_used = 0;


fetch_b1:
  b1 = *iptr++;

another_byte:
  offset = instruction->os_32 << 9; // * 512
  instruction->attr = attr = BxOpcodeInfo[b1+offset].Attr;

  if (attr & BxAnother) {
    if (attr & BxPrefix) {
      switch (b1) {
        case 0x66: // OpSize
          instruction->os_32 = !is_32;
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0x67: // AddrSize
          instruction->as_32 = !is_32;
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0xf2: // REPNE/REPNZ
        case 0xf3: // REP/REPE/REPZ
          instruction->rep_used = b1;
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
          break;

        case 0x2e: // CS:
          instruction->seg = BX_SEG_REG_CS;
          ilen++; goto fetch_b1;
          break;
        case 0x26: // ES:
          instruction->seg = BX_SEG_REG_ES;
          ilen++; goto fetch_b1;
          break;
        case 0x36: // SS:
          instruction->seg = BX_SEG_REG_SS;
          ilen++; goto fetch_b1;
          break;
        case 0x3e: // DS:
          instruction->seg = BX_SEG_REG_DS;
          ilen++; goto fetch_b1;
          break;
        case 0x64: // FS:
          instruction->seg = BX_SEG_REG_FS;
          ilen++; goto fetch_b1;
          break;
        case 0x65: // GS:
          instruction->seg = BX_SEG_REG_GS;
          ilen++; goto fetch_b1;
          break;
        case 0xf0: // LOCK:
          ilen++; goto fetch_b1;
          break;

        default:
BX_CPU_THIS_PTR panic("fetch_decode: prefix default = 0x%02x\n", b1);
        }
      }
    // opcode requires another byte
    if (ilen < remain) {
      ilen++;
      b2 = *iptr++;
      if (b1 == 0x0f) {
        // 2-byte prefix
        b1 = 0x100 | b2;
        goto another_byte;
        }
      }
    else
      return(0);

    // Parse mod-nnn-rm and related bytes
    unsigned rm;
    instruction->modrm = b2;
    rm =
    instruction->rm    = b2 & 0x07;
    instruction->mod   = b2 & 0xc0; // leave unshifted
    instruction->nnn   = (b2 >> 3) & 0x07;
    if (instruction->mod == 0xc0) { // mod == 11b
      goto modrm_done;
      }
    if (instruction->as_32) {
      // 32-bit addressing modes; note that mod==11b handled above
      if (rm != 4) { // no s-i-b byte
#if BX_DYNAMIC_TRANSLATION
        instruction->DTMemRegsUsed = 1<<rm; // except for mod=00b rm=100b
#endif
        if (instruction->mod == 0x00) { // mod == 00b
          instruction->ResolveModrm = BxResolve32Mod0[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg))
            instruction->seg = BX_SEG_REG_DS;
          if (rm == 5) {
            if ((ilen+3) < remain) {
              Bit32u imm32u;
              imm32u = *iptr++;
              imm32u |= (*iptr++) << 8;
              imm32u |= (*iptr++) << 16;
              imm32u |= (*iptr++) << 24;
              instruction->rm_addr = imm32u;
              ilen += 4;
#if BX_DYNAMIC_TRANSLATION
              instruction->DTMemRegsUsed = 0;
#endif
              goto modrm_done;
              }
            else {
              return(0);
              }
            }
          // mod==00b, rm!=4, rm!=5
          goto modrm_done;
          }
        if (instruction->mod == 0x40) { // mod == 01b
          instruction->ResolveModrm = BxResolve32Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg))
            instruction->seg = BX_CPU_THIS_PTR sreg_mod01_rm32[rm];
get_8bit_displ:
          if (ilen < remain) {
            // 8 sign extended to 32
            instruction->displ32u = (Bit8s) *iptr++;
            ilen++;
            goto modrm_done;
            }
          else {
            return(0);
            }
          }
        // (mod == 0x80) mod == 10b
        instruction->ResolveModrm = BxResolve32Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
        if (BX_NULL_SEG_REG(instruction->seg))
          instruction->seg = BX_CPU_THIS_PTR sreg_mod10_rm32[rm];
get_32bit_displ:
        if ((ilen+3) < remain) {
          Bit32u imm32u;
          imm32u = *iptr++;
          imm32u |= (*iptr++) << 8;
          imm32u |= (*iptr++) << 16;
          imm32u |= (*iptr++) << 24;
          instruction->displ32u = imm32u;
          ilen += 4;
          goto modrm_done;
          }
        else {
          return(0);
          }
        }
      else { // mod!=11b, rm==4, s-i-b byte follows
        unsigned sib, base;
        if (ilen < remain) {
          sib = *iptr++;
          ilen++;
          }
        else {
          return(0);
          }
        instruction->sib   = sib;
        base =
        instruction->base  = sib & 0x07; sib >>= 3;
        instruction->index = sib & 0x07; sib >>= 3;
        instruction->scale = sib;
#if BX_DYNAMIC_TRANSLATION
        if (instruction->index == 0x04) // 100b
          instruction->DTMemRegsUsed = 0;
        else
          instruction->DTMemRegsUsed = 1<<instruction->index;
#endif
        if (instruction->mod == 0x00) { // mod==00b, rm==4
          instruction->ResolveModrm = BxResolve32Mod0Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg))
            instruction->seg = BX_CPU_THIS_PTR sreg_mod0_base32[base];
          if (instruction->base == 0x05) {
            goto get_32bit_displ;
            }
          // mod==00b, rm==4, base!=5
#if BX_DYNAMIC_TRANSLATION
          instruction->DTMemRegsUsed |= 1<<base;
#endif
          goto modrm_done;
          }
#if BX_DYNAMIC_TRANSLATION
        // for remaining 32bit cases
        instruction->DTMemRegsUsed |= 1<<base;
#endif
        if (instruction->mod == 0x40) { // mod==01b, rm==4
          instruction->ResolveModrm = BxResolve32Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg))
            instruction->seg = BX_CPU_THIS_PTR sreg_mod1or2_base32[base];
          goto get_8bit_displ;
          }
        // (instruction->mod == 0x80),  mod==10b, rm==4
        instruction->ResolveModrm = BxResolve32Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
        if (BX_NULL_SEG_REG(instruction->seg))
          instruction->seg = BX_CPU_THIS_PTR sreg_mod1or2_base32[base];
        goto get_32bit_displ;
        }
      }
    else {
      // 16-bit addressing modes, mod==11b handled above
      if (instruction->mod == 0x40) { // mod == 01b
        instruction->ResolveModrm = BxResolve16Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve16Mod1or2[rm];
#endif
        if (BX_NULL_SEG_REG(instruction->seg))
          instruction->seg = BX_CPU_THIS_PTR sreg_mod01_rm16[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTMemRegsUsed = BxMemRegsUsed16[rm];
#endif
        if (ilen < remain) {
          // 8 sign extended to 16
          instruction->displ16u = (Bit8s) *iptr++;
          ilen++;
          goto modrm_done;
          }
        else {
          return(0);
          }
        }
      if (instruction->mod == 0x80) { // mod == 10b
        instruction->ResolveModrm = BxResolve16Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve16Mod1or2[rm];
#endif
        if (BX_NULL_SEG_REG(instruction->seg))
          instruction->seg = BX_CPU_THIS_PTR sreg_mod10_rm16[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTMemRegsUsed = BxMemRegsUsed16[rm];
#endif
        if ((ilen+1) < remain) {
          Bit16u displ16u;
          displ16u = *iptr++;
          displ16u |= (*iptr++) << 8;
          instruction->displ16u = displ16u;
          ilen += 2;
          goto modrm_done;
          }
        else {
          return(0);
          }
        }
      // mod must be 00b at this point
      instruction->ResolveModrm = BxResolve16Mod0[rm];
#if BX_DYNAMIC_TRANSLATION
      instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve16Mod0[rm];
#endif
      if (BX_NULL_SEG_REG(instruction->seg))
        instruction->seg = BX_CPU_THIS_PTR sreg_mod00_rm16[rm];
      if (rm == 0x06) {
        if ((ilen+1) < remain) {
          Bit16u displ16u;
          displ16u = *iptr++;
          displ16u |= (*iptr++) << 8;
          instruction->rm_addr = displ16u;
          ilen += 2;
          goto modrm_done;
          }
        else {
          return(0);
          }
        }
      // mod=00b rm!=6
#if BX_DYNAMIC_TRANSLATION
      instruction->DTMemRegsUsed = BxMemRegsUsed16[rm];
#endif
      }

modrm_done:
    if (attr & BxGroupN) {
      BxOpcodeInfo_t *OpcodeInfoPtr;

      OpcodeInfoPtr = BxOpcodeInfo[b1+offset].AnotherArray;
      instruction->execute = OpcodeInfoPtr[instruction->nnn].ExecutePtr;
      // get additional attributes from group table
      attr |= OpcodeInfoPtr[instruction->nnn].Attr;
      instruction->attr = attr;
#if BX_DYNAMIC_TRANSLATION
      instruction->DTAttr = 0; // for now
#endif
      }
    else {
      instruction->execute = BxOpcodeInfo[b1+offset].ExecutePtr;
#if BX_DYNAMIC_TRANSLATION
      instruction->DTAttr = BxDTOpcodeInfo[b1+offset].DTAttr;
      instruction->DTFPtr = BxDTOpcodeInfo[b1+offset].DTASFPtr;
#endif
      }
    }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.
    instruction->execute = BxOpcodeInfo[b1+offset].ExecutePtr;
#if BX_DYNAMIC_TRANSLATION
    instruction->DTAttr = BxDTOpcodeInfo[b1+offset].DTAttr;
    instruction->DTFPtr = BxDTOpcodeInfo[b1+offset].DTASFPtr;
#endif
    }


  imm_mode = attr & BxImmediate;

  if (imm_mode) {
    switch (imm_mode) {
      case BxImmediate_Ib:
        if (ilen < remain) {
          instruction->Ib = *iptr;
          ilen++;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_Ib_SE: // Sign extend to OS size
        if (ilen < remain) {
          Bit8s temp8s;
          temp8s = *iptr;
          if (instruction->os_32)
            instruction->Id = (Bit32s) temp8s;
          else
            instruction->Iw = (Bit16s) temp8s;
          ilen++;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_Iv: // same as BxImmediate_BrOff32
      case BxImmediate_IvIw: // CALL_Ap
        if (instruction->os_32) {
          if ((ilen+3) < remain) {
            Bit32u imm32u;
            imm32u = *iptr++;
            imm32u |= (*iptr++) << 8;
            imm32u |= (*iptr++) << 16;
            imm32u |= (*iptr) << 24;
            instruction->Id = imm32u;
            ilen += 4;
            }
          else {
            return(0);
            }
          }
        else {
          if ((ilen+1) < remain) {
            Bit16u imm16u;
            imm16u = *iptr++;
            imm16u |= (*iptr) << 8;
            instruction->Iw = imm16u;
            ilen += 2;
            }
          else {
            return(0);
            }
          }
        if (imm_mode != BxImmediate_IvIw)
          break;
        iptr++;
        // Get Iw for BxImmediate_IvIw
        if ((ilen+1) < remain) {
          Bit16u imm16u;
          imm16u = *iptr++;
          imm16u |= (*iptr) << 8;
          instruction->Iw2 = imm16u;
          ilen += 2;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_O:
        if (instruction->as_32) {
          // fetch 32bit address into Id
          if ((ilen+3) < remain) {
            Bit32u imm32u;
            imm32u = *iptr++;
            imm32u |= (*iptr++) << 8;
            imm32u |= (*iptr++) << 16;
            imm32u |= (*iptr) << 24;
            instruction->Id = imm32u;
            ilen += 4;
            }
          else {
            return(0);
            }
          }
        else {
          // fetch 16bit address into Id
          if ((ilen+1) < remain) {
            Bit32u imm32u;
            imm32u = *iptr++;
            imm32u |= (*iptr) << 8;
            instruction->Id = imm32u;
            ilen += 2;
            }
          else {
            return(0);
            }
          }
        break;
      case BxImmediate_Iw:
      case BxImmediate_IwIb:
        if ((ilen+1) < remain) {
          Bit16u imm16u;
          imm16u = *iptr++;
          imm16u |= (*iptr) << 8;
          instruction->Iw = imm16u;
          ilen += 2;
          }
        else {
          return(0);
          }
        if (imm_mode == BxImmediate_Iw) break;
        iptr++;
        if (ilen < remain) {
          instruction->Ib2 = *iptr;
          ilen++;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_BrOff8:
        if (ilen < remain) {
          Bit8s temp8s;
          temp8s = *iptr;
          instruction->Id = temp8s;
          ilen++;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_BrOff16:
        if ((ilen+1) < remain) {
          Bit16u imm16u;
          imm16u = *iptr++;
          imm16u |= (*iptr) << 8;
          instruction->Id = (Bit16s) imm16u;
          ilen += 2;
          }
        else {
          return(0);
          }
        break;
      default:
BX_CPU_THIS_PTR info("b1 was %x\n", b1);
        BX_CPU_THIS_PTR panic("fetchdecode: imm_mode = %u\n", imm_mode);
      }
    }


  instruction->b1 = b1;
  instruction->ilen = ilen;
  //instruction->flags_in  = 0; // for now
  //instruction->flags_out = 0; // for now
  return(1);
}

  void
BX_CPU_C::BxError(BxInstruction_t *i)
{
  // extern void dump_core();
  BX_CPU_THIS_PTR info("BxError: instruction with op1=0x%x\n", i->b1);
  BX_CPU_THIS_PTR info("nnn was %u\n", i->nnn);

  BX_CPU_THIS_PTR info("WARNING: Encountered an unknown instruction (signalling illegal instruction):\n");
  // dump_core();

  BX_CPU_THIS_PTR UndefinedOpcode(i);
}

  void
BX_CPU_C::BxResolveError(BxInstruction_t *i)
{
  BX_CPU_THIS_PTR panic("BxResolveError: instruction with op1=0x%x\n", i->b1);
}
