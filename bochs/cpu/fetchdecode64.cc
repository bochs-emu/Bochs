/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode64.cc,v 1.53 2003-05-29 19:44:59 sshwarts Exp $
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
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

#if 0
// KPL.  A number of cases are testing field values after they have
// been 16-register extended, but should be testing the pre-extension
// value.  Or, are causing use of extended values on 32-bit case arrays
// which are only 8-registers wide.
#define ExtendedFieldCheck(panicCondition) { \
  if ( panicCondition ) \
    BX_PANIC(("Extended Field Check macro, extended field found.")); \
  }
#else
#define ExtendedFieldCheck(panicCondition) 
#endif

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




void BxResolveError(bxInstruction_c *);

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

static BxExecutePtr_tR BxResolve32Mod0[8] = {
  &BX_CPU_C::Resolve32Mod0Rm0,
  &BX_CPU_C::Resolve32Mod0Rm1,
  &BX_CPU_C::Resolve32Mod0Rm2,
  &BX_CPU_C::Resolve32Mod0Rm3,
  NULL, // escape to 2-byte
  &BX_CPU_C::Resolve32Mod0Rm5,
  &BX_CPU_C::Resolve32Mod0Rm6,
  &BX_CPU_C::Resolve32Mod0Rm7
  };

static BxExecutePtr_tR BxResolve32Mod1or2[8] = {
  &BX_CPU_C::Resolve32Mod1or2Rm0,
  &BX_CPU_C::Resolve32Mod1or2Rm1,
  &BX_CPU_C::Resolve32Mod1or2Rm2,
  &BX_CPU_C::Resolve32Mod1or2Rm3,
  NULL, // escape to 2-byte
  &BX_CPU_C::Resolve32Mod1or2Rm5,
  &BX_CPU_C::Resolve32Mod1or2Rm6,
  &BX_CPU_C::Resolve32Mod1or2Rm7
  };

static BxExecutePtr_tR BxResolve32Mod0Base[8] = {
  &BX_CPU_C::Resolve32Mod0Base0,
  &BX_CPU_C::Resolve32Mod0Base1,
  &BX_CPU_C::Resolve32Mod0Base2,
  &BX_CPU_C::Resolve32Mod0Base3,
  &BX_CPU_C::Resolve32Mod0Base4,
  &BX_CPU_C::Resolve32Mod0Base5,
  &BX_CPU_C::Resolve32Mod0Base6,
  &BX_CPU_C::Resolve32Mod0Base7,
  };

static BxExecutePtr_tR BxResolve32Mod1or2Base[8] = {
  &BX_CPU_C::Resolve32Mod1or2Base0,
  &BX_CPU_C::Resolve32Mod1or2Base1,
  &BX_CPU_C::Resolve32Mod1or2Base2,
  &BX_CPU_C::Resolve32Mod1or2Base3,
  &BX_CPU_C::Resolve32Mod1or2Base4,
  &BX_CPU_C::Resolve32Mod1or2Base5,
  &BX_CPU_C::Resolve32Mod1or2Base6,
  &BX_CPU_C::Resolve32Mod1or2Base7,
  };

static BxExecutePtr_tR BxResolve64Mod0[16] = {
  &BX_CPU_C::Resolve64Mod0Rm0,
  &BX_CPU_C::Resolve64Mod0Rm1,
  &BX_CPU_C::Resolve64Mod0Rm2,
  &BX_CPU_C::Resolve64Mod0Rm3,
  NULL, // escape to 2-byte
  &BX_CPU_C::Resolve64Mod0Rm5,
  &BX_CPU_C::Resolve64Mod0Rm6,
  &BX_CPU_C::Resolve64Mod0Rm7,
  &BX_CPU_C::Resolve64Mod0Rm8,
  &BX_CPU_C::Resolve64Mod0Rm9,
  &BX_CPU_C::Resolve64Mod0Rm10,
  &BX_CPU_C::Resolve64Mod0Rm11,
  &BX_CPU_C::Resolve64Mod0Rm12,
  &BX_CPU_C::Resolve64Mod0Rm13,
  &BX_CPU_C::Resolve64Mod0Rm14,
  &BX_CPU_C::Resolve64Mod0Rm15
  };

static BxExecutePtr_tR BxResolve64Mod1or2[16] = {
  &BX_CPU_C::Resolve64Mod1or2Rm0,
  &BX_CPU_C::Resolve64Mod1or2Rm1,
  &BX_CPU_C::Resolve64Mod1or2Rm2,
  &BX_CPU_C::Resolve64Mod1or2Rm3,
  NULL, // escape to 2-byte
  &BX_CPU_C::Resolve64Mod1or2Rm5,
  &BX_CPU_C::Resolve64Mod1or2Rm6,
  &BX_CPU_C::Resolve64Mod1or2Rm7,
  &BX_CPU_C::Resolve64Mod1or2Rm8,
  &BX_CPU_C::Resolve64Mod1or2Rm9,
  &BX_CPU_C::Resolve64Mod1or2Rm10,
  &BX_CPU_C::Resolve64Mod1or2Rm11,
  &BX_CPU_C::Resolve64Mod1or2Rm12,
  &BX_CPU_C::Resolve64Mod1or2Rm13,
  &BX_CPU_C::Resolve64Mod1or2Rm14,
  &BX_CPU_C::Resolve64Mod1or2Rm15
  };

static BxExecutePtr_tR BxResolve64Mod0Base[16] = {
  &BX_CPU_C::Resolve64Mod0Base0,
  &BX_CPU_C::Resolve64Mod0Base1,
  &BX_CPU_C::Resolve64Mod0Base2,
  &BX_CPU_C::Resolve64Mod0Base3,
  &BX_CPU_C::Resolve64Mod0Base4,
  &BX_CPU_C::Resolve64Mod0Base5,
  &BX_CPU_C::Resolve64Mod0Base6,
  &BX_CPU_C::Resolve64Mod0Base7,
  &BX_CPU_C::Resolve64Mod0Base8,
  &BX_CPU_C::Resolve64Mod0Base9,
  &BX_CPU_C::Resolve64Mod0Base10,
  &BX_CPU_C::Resolve64Mod0Base11,
  &BX_CPU_C::Resolve64Mod0Base12,
  &BX_CPU_C::Resolve64Mod0Base13,
  &BX_CPU_C::Resolve64Mod0Base14,
  &BX_CPU_C::Resolve64Mod0Base15,
  };

static BxExecutePtr_tR BxResolve64Mod1or2Base[16] = {
  &BX_CPU_C::Resolve64Mod1or2Base0,
  &BX_CPU_C::Resolve64Mod1or2Base1,
  &BX_CPU_C::Resolve64Mod1or2Base2,
  &BX_CPU_C::Resolve64Mod1or2Base3,
  &BX_CPU_C::Resolve64Mod1or2Base4,
  &BX_CPU_C::Resolve64Mod1or2Base5,
  &BX_CPU_C::Resolve64Mod1or2Base6,
  &BX_CPU_C::Resolve64Mod1or2Base7,
  &BX_CPU_C::Resolve64Mod1or2Base8,
  &BX_CPU_C::Resolve64Mod1or2Base9,
  &BX_CPU_C::Resolve64Mod1or2Base10,
  &BX_CPU_C::Resolve64Mod1or2Base11,
  &BX_CPU_C::Resolve64Mod1or2Base12,
  &BX_CPU_C::Resolve64Mod1or2Base13,
  &BX_CPU_C::Resolve64Mod1or2Base14,
  &BX_CPU_C::Resolve64Mod1or2Base15,
  };

typedef struct BxOpcodeInfo_t {
  Bit16u         Attr;
  BxExecutePtr_t ExecutePtr;
  struct BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;


// common fetchdecode32/64 opcode tables
#include "fetchdecode.h"


static BxOpcodeInfo_t opcodesADD_EwIw[2] = {
  { BxLockable, &BX_CPU_C::ADD_EEwIw },
  { 0,          &BX_CPU_C::ADD_EGwIw }
  };

static BxOpcodeInfo_t opcodesADD_EdId[2] = {
  { BxLockable, &BX_CPU_C::ADD_EEdId },
  { 0,          &BX_CPU_C::ADD_EGdId }
  };

static BxOpcodeInfo_t opcodesADD_GwEw[2] = {
  { 0,          &BX_CPU_C::ADD_GwEEw },
  { 0,          &BX_CPU_C::ADD_GwEGw }
  };

static BxOpcodeInfo_t opcodesADD_GdEd[2] = {
  { 0,          &BX_CPU_C::ADD_GdEEd },
  { 0,          &BX_CPU_C::ADD_GdEGd }
  };

static BxOpcodeInfo_t opcodesMOV_GbEb[2] = {
  { 0,          &BX_CPU_C::MOV_GbEEb },
  { 0,          &BX_CPU_C::MOV_GbEGb }
  };

static BxOpcodeInfo_t opcodesMOV_GwEw[2] = {
  { 0,          &BX_CPU_C::MOV_GwEEw },
  { 0,          &BX_CPU_C::MOV_GwEGw }
  };

static BxOpcodeInfo_t opcodesMOV_GdEd[2] = {
  { 0,          &BX_CPU_C::MOV_GdEEd },
  { 0,          &BX_CPU_C::MOV_GdEGd }
  };

static BxOpcodeInfo_t opcodesMOV_EbGb[2] = {
  { 0,          &BX_CPU_C::MOV_EEbGb },
  { 0,          &BX_CPU_C::MOV_EGbGb }
  };

static BxOpcodeInfo_t opcodesMOV_EwGw[2] = {
  { 0,          &BX_CPU_C::MOV_EEwGw },
  { 0,          &BX_CPU_C::MOV_EGwGw }
  };

static BxOpcodeInfo_t opcodesMOV_EdGd[2] = {
  { 0,          &BX_CPU_C::MOV_EEdGd },
  { 0,          &BX_CPU_C::MOV_EGdGd }
  };


/* ************* */
/* Opcode Groups */
/* ************* */

static BxOpcodeInfo_t BxOpcodeInfo64G1EbIb[8] = {
  /* 0 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::ADD_EbIb },
  /* 1 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::OR_EbIb },
  /* 2 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::ADC_EbIb },
  /* 3 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::SBB_EbIb },
  /* 4 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::AND_EbIb },
  /* 5 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::SUB_EbIb },
  /* 6 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::XOR_EbIb },
  /* 7 */  { BxImmediate_Ib,              &BX_CPU_C::CMP_EbIb }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G1Ew[8] = {
  // attributes defined in main area
  /* 0 */  { BxSplitMod11b, NULL, opcodesADD_EwIw },
  /* 1 */  { BxLockable, &BX_CPU_C::OR_EwIw },
  /* 2 */  { BxLockable, &BX_CPU_C::ADC_EwIw },
  /* 3 */  { BxLockable, &BX_CPU_C::SBB_EwIw },
  /* 4 */  { BxLockable, &BX_CPU_C::AND_EwIw },
  /* 5 */  { BxLockable, &BX_CPU_C::SUB_EwIw },
  /* 6 */  { BxLockable, &BX_CPU_C::XOR_EwIw },
  /* 7 */  { 0,          &BX_CPU_C::CMP_EwIw }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G1Ed[8] = {
  // attributes defined in main area
  /* 0 */  { BxSplitMod11b, NULL, opcodesADD_EdId },
  /* 1 */  { BxLockable, &BX_CPU_C::OR_EdId },
  /* 2 */  { BxLockable, &BX_CPU_C::ADC_EdId },
  /* 3 */  { BxLockable, &BX_CPU_C::SBB_EdId },
  /* 4 */  { BxLockable, &BX_CPU_C::AND_EdId },
  /* 5 */  { BxLockable, &BX_CPU_C::SUB_EdId },
  /* 6 */  { BxLockable, &BX_CPU_C::XOR_EdId },
  /* 7 */  { 0,          &BX_CPU_C::CMP_EdId }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G1Eq[8] = {
  // attributes defined in main area
  /* 0 */  { BxLockable, &BX_CPU_C::ADD_EqId },
  /* 1 */  { BxLockable, &BX_CPU_C::OR_EqId },
  /* 2 */  { BxLockable, &BX_CPU_C::ADC_EqId },
  /* 3 */  { BxLockable, &BX_CPU_C::SBB_EqId },
  /* 4 */  { BxLockable, &BX_CPU_C::AND_EqId },
  /* 5 */  { BxLockable, &BX_CPU_C::SUB_EqId },
  /* 6 */  { BxLockable, &BX_CPU_C::XOR_EqId },
  /* 7 */  { 0,          &BX_CPU_C::CMP_EqId }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G2Eb[8] = {
  // attributes defined in main area
  /* 0 */  { 0, &BX_CPU_C::ROL_Eb },
  /* 1 */  { 0, &BX_CPU_C::ROR_Eb },
  /* 2 */  { 0, &BX_CPU_C::RCL_Eb },
  /* 3 */  { 0, &BX_CPU_C::RCR_Eb },
  /* 4 */  { 0, &BX_CPU_C::SHL_Eb },
  /* 5 */  { 0, &BX_CPU_C::SHR_Eb },
  /* 6 */  { 0, &BX_CPU_C::SHL_Eb },
  /* 7 */  { 0, &BX_CPU_C::SAR_Eb }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G2Ew[8] = {
  // attributes defined in main area
  /* 0 */  { 0, &BX_CPU_C::ROL_Ew },
  /* 1 */  { 0, &BX_CPU_C::ROR_Ew },
  /* 2 */  { 0, &BX_CPU_C::RCL_Ew },
  /* 3 */  { 0, &BX_CPU_C::RCR_Ew },
  /* 4 */  { 0, &BX_CPU_C::SHL_Ew },
  /* 5 */  { 0, &BX_CPU_C::SHR_Ew },
  /* 6 */  { 0, &BX_CPU_C::SHL_Ew },
  /* 7 */  { 0, &BX_CPU_C::SAR_Ew }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G2Ed[8] = {
  // attributes defined in main area
  /* 0 */  { 0, &BX_CPU_C::ROL_Ed },
  /* 1 */  { 0, &BX_CPU_C::ROR_Ed },
  /* 2 */  { 0, &BX_CPU_C::RCL_Ed },
  /* 3 */  { 0, &BX_CPU_C::RCR_Ed },
  /* 4 */  { 0, &BX_CPU_C::SHL_Ed },
  /* 5 */  { 0, &BX_CPU_C::SHR_Ed },
  /* 6 */  { 0, &BX_CPU_C::SHL_Ed },
  /* 7 */  { 0, &BX_CPU_C::SAR_Ed }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G2Eq[8] = {
  // attributes defined in main area
  /* 0 */  { 0, &BX_CPU_C::ROL_Eq },
  /* 1 */  { 0, &BX_CPU_C::ROR_Eq },
  /* 2 */  { 0, &BX_CPU_C::RCL_Eq },
  /* 3 */  { 0, &BX_CPU_C::RCR_Eq },
  /* 4 */  { 0, &BX_CPU_C::SHL_Eq },
  /* 5 */  { 0, &BX_CPU_C::SHR_Eq },
  /* 6 */  { 0, &BX_CPU_C::SHL_Eq },
  /* 7 */  { 0, &BX_CPU_C::SAR_Eq }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G3Eb[8] = {
  /* 0 */  { BxImmediate_Ib, &BX_CPU_C::TEST_EbIb },
  /* 1 */  { BxImmediate_Ib, &BX_CPU_C::TEST_EbIb },
  /* 2 */  { BxLockable,     &BX_CPU_C::NOT_Eb },
  /* 3 */  { BxLockable,     &BX_CPU_C::NEG_Eb },
  /* 4 */  { 0,              &BX_CPU_C::MUL_ALEb },
  /* 5 */  { 0,              &BX_CPU_C::IMUL_ALEb },
  /* 6 */  { 0,              &BX_CPU_C::DIV_ALEb },
  /* 7 */  { 0,              &BX_CPU_C::IDIV_ALEb }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G3Ew[8] = {
  /* 0 */  { BxImmediate_Iw, &BX_CPU_C::TEST_EwIw },
  /* 1 */  { BxImmediate_Iw, &BX_CPU_C::TEST_EwIw },
  /* 2 */  { BxLockable,     &BX_CPU_C::NOT_Ew },
  /* 3 */  { BxLockable,     &BX_CPU_C::NEG_Ew },
  /* 4 */  { 0,              &BX_CPU_C::MUL_AXEw },
  /* 5 */  { 0,              &BX_CPU_C::IMUL_AXEw },
  /* 6 */  { 0,              &BX_CPU_C::DIV_AXEw },
  /* 7 */  { 0,              &BX_CPU_C::IDIV_AXEw }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G3Ed[8] = {
  /* 0 */  { BxImmediate_Iv, &BX_CPU_C::TEST_EdId },
  /* 1 */  { BxImmediate_Iv, &BX_CPU_C::TEST_EdId },
  /* 2 */  { BxLockable,     &BX_CPU_C::NOT_Ed },
  /* 3 */  { BxLockable,     &BX_CPU_C::NEG_Ed },
  /* 4 */  { 0,              &BX_CPU_C::MUL_EAXEd },
  /* 5 */  { 0,              &BX_CPU_C::IMUL_EAXEd },
  /* 6 */  { 0,              &BX_CPU_C::DIV_EAXEd },
  /* 7 */  { 0,              &BX_CPU_C::IDIV_EAXEd }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G3Eq[8] = {
  /* 0 */  { BxImmediate_Iv, &BX_CPU_C::TEST_EqId },
  /* 1 */  { BxImmediate_Iv, &BX_CPU_C::TEST_EqId },
  /* 2 */  { BxLockable,     &BX_CPU_C::NOT_Eq },
  /* 3 */  { BxLockable,     &BX_CPU_C::NEG_Eq },
  /* 4 */  { 0,              &BX_CPU_C::MUL_RAXEq },
  /* 5 */  { 0,              &BX_CPU_C::IMUL_RAXEq },
  /* 6 */  { 0,              &BX_CPU_C::DIV_RAXEq },
  /* 7 */  { 0,              &BX_CPU_C::IDIV_RAXEq }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G4[8] = {
  /* 0 */  { BxLockable, &BX_CPU_C::INC_Eb },
  /* 1 */  { BxLockable, &BX_CPU_C::DEC_Eb },
  /* 2 */  { 0, &BX_CPU_C::BxError },
  /* 3 */  { 0, &BX_CPU_C::BxError },
  /* 4 */  { 0, &BX_CPU_C::BxError },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { 0, &BX_CPU_C::BxError },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  }; 

static BxOpcodeInfo_t BxOpcodeInfo64G5w[8] = {
  // attributes defined in main area
  /* 0 */  { BxLockable, &BX_CPU_C::INC_Ew },
  /* 1 */  { BxLockable, &BX_CPU_C::DEC_Ew },
  /* 2 */  { 0, &BX_CPU_C::CALL_Ew },   // invalid??
  /* 3 */  { 0, &BX_CPU_C::CALL16_Ep }, // invalid??
  /* 4 */  { 0, &BX_CPU_C::JMP_Eq },    // invalid??
  /* 5 */  { 0, &BX_CPU_C::JMP16_Ep },  // invalid??
  /* 6 */  { 0, &BX_CPU_C::PUSH_Ew },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G5d[8] = {
  // attributes defined in main area
  /* 0 */  { BxLockable, &BX_CPU_C::INC_Ed },
  /* 1 */  { BxLockable, &BX_CPU_C::DEC_Ed },
  /* 2 */  { 0, &BX_CPU_C::CALL_Eq },
  /* 3 */  { 0, &BX_CPU_C::CALL32_Ep },
  /* 4 */  { 0, &BX_CPU_C::JMP_Eq },
  /* 5 */  { 0, &BX_CPU_C::JMP32_Ep },
  /* 6 */  { 0, &BX_CPU_C::PUSH_Eq },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G5q[8] = {
  // attributes defined in main area
  /* 0 */  { BxLockable, &BX_CPU_C::INC_Eq },
  /* 1 */  { BxLockable, &BX_CPU_C::DEC_Eq },
  /* 2 */  { 0, &BX_CPU_C::CALL_Eq },
  /* 3 */  { 0, &BX_CPU_C::CALL64_Ep },
  /* 4 */  { 0, &BX_CPU_C::JMP_Eq },
  /* 5 */  { 0, &BX_CPU_C::JMP64_Ep },
  /* 6 */  { 0, &BX_CPU_C::PUSH_Eq },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G6[8] = {
  // attributes defined in main area
  /* 0 */  { 0, &BX_CPU_C::SLDT_Ew },
  /* 1 */  { 0, &BX_CPU_C::STR_Ew },
  /* 2 */  { 0, &BX_CPU_C::LLDT_Ew },
  /* 3 */  { 0, &BX_CPU_C::LTR_Ew },
  /* 4 */  { 0, &BX_CPU_C::VERR_Ew },
  /* 5 */  { 0, &BX_CPU_C::VERW_Ew },
  /* 6 */  { 0, &BX_CPU_C::BxError },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G7[8] = {
  /* 0 */  { 0, &BX_CPU_C::SGDT_Ms },
  /* 1 */  { 0, &BX_CPU_C::SIDT_Ms },
  /* 2 */  { 0, &BX_CPU_C::LGDT_Ms },
  /* 3 */  { 0, &BX_CPU_C::LIDT_Ms },
  /* 4 */  { 0, &BX_CPU_C::SMSW_Ew },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { 0, &BX_CPU_C::LMSW_Ew },
  /* 7 */  { 0, &BX_CPU_C::INVLPG }
  };


static BxOpcodeInfo_t BxOpcodeInfo64G8EvIb[8] = {
  /* 0 */  { 0, &BX_CPU_C::BxError },
  /* 1 */  { 0, &BX_CPU_C::BxError },
  /* 2 */  { 0, &BX_CPU_C::BxError },
  /* 3 */  { 0, &BX_CPU_C::BxError },
  /* 4 */  { BxImmediate_Ib,              &BX_CPU_C::BT_EvIb },
  /* 5 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTS_EvIb },
  /* 6 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTR_EvIb },
  /* 7 */  { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTC_EvIb }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G9[8] = {
  /* 0 */  { 0, &BX_CPU_C::BxError },
  /* 1 */  { BxLockable, &BX_CPU_C::CMPXCHG8B },
  /* 2 */  { 0, &BX_CPU_C::BxError },
  /* 3 */  { 0, &BX_CPU_C::BxError },
  /* 4 */  { 0, &BX_CPU_C::BxError },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { 0, &BX_CPU_C::BxError },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G12[8] = {
  /* 0 */  { 0, &BX_CPU_C::BxError },
  /* 1 */  { 0, &BX_CPU_C::BxError },
  /* 2 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1202 },
  /* 3 */  { 0, &BX_CPU_C::BxError },
  /* 4 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1204 },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1206 },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G13[8] = {
  /* 0 */  { 0, &BX_CPU_C::BxError },
  /* 1 */  { 0, &BX_CPU_C::BxError },
  /* 2 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1302 },
  /* 3 */  { 0, &BX_CPU_C::BxError },
  /* 4 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1304 },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1306 },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeInfo64G14[8] = {
  /* 0 */  { 0, &BX_CPU_C::BxError },
  /* 1 */  { 0, &BX_CPU_C::BxError },
  /* 2 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1402 },
  /* 3 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1403 },
  /* 4 */  { 0, &BX_CPU_C::BxError },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1406 },
  /* 7 */  { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1407 } 
  };

static BxOpcodeInfo_t BxOpcodeInfo64G15[8] = {
  /* 0 */  { 0, &BX_CPU_C::FXSAVE  },
  /* 1 */  { 0, &BX_CPU_C::FXRSTOR },
  /* 2 */  { 0, &BX_CPU_C::LDMXCSR },
  /* 3 */  { 0, &BX_CPU_C::STMXCSR },
  /* 4 */  { 0, &BX_CPU_C::BxError },
  /* 5 */  { 0, &BX_CPU_C::NOP },      /* LFENCE */
  /* 6 */  { 0, &BX_CPU_C::NOP },      /* MFENCE */
  /* 7 */  { 0, &BX_CPU_C::NOP }       /* SFENCE/CFLUSH */
  };

static BxOpcodeInfo_t BxOpcodeInfo64G16[8] = {
  /* 0 */  { 0, &BX_CPU_C::PREFETCH },           /* PREFETCH_NTA */
  /* 1 */  { 0, &BX_CPU_C::PREFETCH },           /* PREFETCH_T0  */
  /* 2 */  { 0, &BX_CPU_C::PREFETCH },           /* PREFETCH_T1  */
  /* 3 */  { 0, &BX_CPU_C::PREFETCH },           /* PREFETCH_T2  */
  /* 4 */  { 0, &BX_CPU_C::BxError },
  /* 5 */  { 0, &BX_CPU_C::BxError },
  /* 6 */  { 0, &BX_CPU_C::BxError },
  /* 7 */  { 0, &BX_CPU_C::BxError }
  };

// 512 entries for 16bit mode
// 512 entries for 32bit mode
// 512 entries for 64bit mode

static BxOpcodeInfo_t BxOpcodeInfo64[512*3] = {
  // 512 entries for 16bit mode
  /* 00 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EbGb },
  /* 01 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EwGw },
  /* 02 */  { BxAnother, &BX_CPU_C::ADD_GbEb },
  /* 03 */  { BxAnother | BxSplitMod11b, NULL, opcodesADD_GwEw },
  /* 04 */  { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 */  { BxImmediate_Iv, &BX_CPU_C::ADD_AXIw },
  /* 06 */  { 0, &BX_CPU_C::BxError },
  /* 07 */  { 0, &BX_CPU_C::BxError },
  /* 08 */  { BxAnother | BxLockable, &BX_CPU_C::OR_EbGb },
  /* 09 */  { BxAnother | BxLockable, &BX_CPU_C::OR_EwGw },
  /* 0A */  { BxAnother, &BX_CPU_C::OR_GbEb },
  /* 0B */  { BxAnother, &BX_CPU_C::OR_GwEw },
  /* 0C */  { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D */  { BxImmediate_Iv, &BX_CPU_C::OR_AXIw },
  /* 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F */  { BxAnother, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EbGb },
  /* 11 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EwGw },
  /* 12 */  { BxAnother, &BX_CPU_C::ADC_GbEb },
  /* 13 */  { BxAnother, &BX_CPU_C::ADC_GwEw },
  /* 14 */  { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 */  { BxImmediate_Iv, &BX_CPU_C::ADC_AXIw },
  /* 16 */  { 0, &BX_CPU_C::BxError },
  /* 17 */  { 0, &BX_CPU_C::BxError },
  /* 18 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EbGb },
  /* 19 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EwGw },
  /* 1A */  { BxAnother, &BX_CPU_C::SBB_GbEb },
  /* 1B */  { BxAnother, &BX_CPU_C::SBB_GwEw },
  /* 1C */  { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D */  { BxImmediate_Iv, &BX_CPU_C::SBB_AXIw },
  /* 1E */  { 0, &BX_CPU_C::BxError },
  /* 1F */  { 0, &BX_CPU_C::BxError },
  /* 20 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EbGb },
  /* 21 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EwGw },
  /* 22 */  { BxAnother, &BX_CPU_C::AND_GbEb },
  /* 23 */  { BxAnother, &BX_CPU_C::AND_GwEw },
  /* 24 */  { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 */  { BxImmediate_Iv, &BX_CPU_C::AND_AXIw },
  /* 26 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // ES:
  /* 27 */  { 0, &BX_CPU_C::BxError },
  /* 28 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EbGb },
  /* 29 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EwGw },
  /* 2A */  { BxAnother, &BX_CPU_C::SUB_GbEb },
  /* 2B */  { BxAnother, &BX_CPU_C::SUB_GwEw },
  /* 2C */  { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D */  { BxImmediate_Iv, &BX_CPU_C::SUB_AXIw },
  /* 2E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // CS:
  /* 2F */  { 0, &BX_CPU_C::BxError },
  /* 30 */  { BxAnother, &BX_CPU_C::XOR_EbGb },
  /* 31 */  { BxAnother, &BX_CPU_C::XOR_EwGw },
  /* 32 */  { BxAnother, &BX_CPU_C::XOR_GbEb },
  /* 33 */  { BxAnother, &BX_CPU_C::XOR_GwEw },
  /* 34 */  { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 */  { BxImmediate_Iv, &BX_CPU_C::XOR_AXIw },
  /* 36 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // SS:
  /* 37 */  { 0, &BX_CPU_C::BxError },
  /* 38 */  { BxAnother, &BX_CPU_C::CMP_EbGb },
  /* 39 */  { BxAnother, &BX_CPU_C::CMP_EwGw },
  /* 3A */  { BxAnother, &BX_CPU_C::CMP_GbEb },
  /* 3B */  { BxAnother, &BX_CPU_C::CMP_GwEw },
  /* 3C */  { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D */  { BxImmediate_Iv, &BX_CPU_C::CMP_AXIw },
  /* 3E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // DS:
  /* 3F */  { 0, &BX_CPU_C::BxError },
  /* 40 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 41 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 42 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 43 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 44 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 45 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 46 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 47 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 48 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 49 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4A */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4B */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4C */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4D */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4F */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 50 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 51 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 52 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 53 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 54 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 55 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 56 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 57 */  { 0, &BX_CPU_C::PUSH_RX },
  /* 58 */  { 0, &BX_CPU_C::POP_RX },
  /* 59 */  { 0, &BX_CPU_C::POP_RX },
  /* 5A */  { 0, &BX_CPU_C::POP_RX },
  /* 5B */  { 0, &BX_CPU_C::POP_RX },
  /* 5C */  { 0, &BX_CPU_C::POP_RX },
  /* 5D */  { 0, &BX_CPU_C::POP_RX },
  /* 5E */  { 0, &BX_CPU_C::POP_RX },
  /* 5F */  { 0, &BX_CPU_C::POP_RX },
  /* 60 */  { 0, &BX_CPU_C::BxError },
  /* 61 */  { 0, &BX_CPU_C::BxError },
  /* 62 */  { 0, &BX_CPU_C::BxError },
#ifdef __GNUC__
#warning PRT: op=63 This needs checking on real hardware.  Manual says 16 bit version leaves upper 48 bits unchanged
#endif
  /* 63 */  { BxAnother, &BX_CPU_C::MOVSX_GwEw }, // 
  /* 64 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // FS:
  /* 65 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // GS:
  /* 66 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // OS:
  /* 67 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // AS:
  /* 68 */  { BxImmediate_Iv, &BX_CPU_C::PUSH_Iw },
  /* 69 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::IMUL_GwEwIw },
  /* 6A */  { BxImmediate_Ib_SE, &BX_CPU_C::PUSH_Iw },
  /* 6B */  { BxAnother | BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GwEwIw },
  /* 6C */  { BxRepeatable, &BX_CPU_C::INSB_YbDX },
  /* 6D */  { BxRepeatable, &BX_CPU_C::INSW_YvDX },
  /* 6E */  { BxRepeatable, &BX_CPU_C::OUTSB_DXXb },
  /* 6F */  { BxRepeatable, &BX_CPU_C::OUTSW_DXXv },
  /* 70 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 71 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 72 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 73 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 74 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 75 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 76 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 77 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 78 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 79 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7A */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7B */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7C */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7D */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7E */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7F */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 80 */  { BxAnother | BxGroup1, NULL, BxOpcodeInfo64G1EbIb },
  /* 81 */  { BxAnother | BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfo64G1Ew },
  /* 82 */  { 0, &BX_CPU_C::BxError },
  /* 83 */  { BxAnother | BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfo64G1Ew },
  /* 84 */  { BxAnother, &BX_CPU_C::TEST_EbGb },
  /* 85 */  { BxAnother, &BX_CPU_C::TEST_EwGw },
  /* 86 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EbGb },
  /* 87 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EwGw },
  /* 88 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_EbGb },
  /* 89 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_EwGw },
  /* 8A */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GbEb },
  /* 8B */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GwEw },
  /* 8C */  { BxAnother, &BX_CPU_C::MOV_EwSw },
  /* 8D */  { BxAnother, &BX_CPU_C::LEA_GwM },
  /* 8E */  { BxAnother, &BX_CPU_C::MOV_SwEw },
  /* 8F */  { BxAnother, &BX_CPU_C::POP_Ew },
  /* 90 */  { 0, &BX_CPU_C::NOP },
  /* 91 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 92 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 93 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 94 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 95 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 96 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 97 */  { 0, &BX_CPU_C::XCHG_RXAX },
  /* 98 */  { 0, &BX_CPU_C::CBW },
  /* 99 */  { 0, &BX_CPU_C::CWD },
  /* 9A */  { 0, &BX_CPU_C::BxError },
  /* 9B */  { 0, &BX_CPU_C::FWAIT },
  /* 9C */  { 0, &BX_CPU_C::PUSHF_Fv },
  /* 9D */  { 0, &BX_CPU_C::POPF_Fv },
  /* 9E */  { 0, &BX_CPU_C::BxError },
  /* 9F */  { 0, &BX_CPU_C::BxError },
  /* A0 */  { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 */  { BxImmediate_O, &BX_CPU_C::MOV_AXOq },
  /* A2 */  { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 */  { BxImmediate_O, &BX_CPU_C::MOV_OqAX },
  /* A4 */  { BxRepeatable, &BX_CPU_C::MOVSB_XbYb },
  /* A5 */  { BxRepeatable, &BX_CPU_C::MOVSW_XvYv },
  /* A6 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSB_XbYb },
  /* A7 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSW_XvYv },
  /* A8 */  { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 */  { BxImmediate_Iv, &BX_CPU_C::TEST_AXIw },
  /* AA */  { BxRepeatable, &BX_CPU_C::STOSB_YbAL },
  /* AB */  { BxRepeatable, &BX_CPU_C::STOSW_YveAX },
  /* AC */  { BxRepeatable, &BX_CPU_C::LODSB_ALXb },
  /* AD */  { BxRepeatable, &BX_CPU_C::LODSW_eAXXv },
  /* AE */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASB_ALXb },
  /* AF */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASW_eAXXv },
  /* B0 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* B9 */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BA */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BB */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BC */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BD */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BE */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* BF */  { BxImmediate_Iv, &BX_CPU_C::MOV_RXIw },
  /* C0 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eb },
  /* C1 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Ew },
  /* C2 */  { BxImmediate_Iw, &BX_CPU_C::RETnear16_Iw },
  /* C3 */  { 0,              &BX_CPU_C::RETnear16 },
  /* C4 */  { 0, &BX_CPU_C::BxError },
  /* C5 */  { 0, &BX_CPU_C::BxError },
  /* C6 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::MOV_EbIb },
  /* C7 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::MOV_EwIw },
  /* C8 */  { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 */  { 0, &BX_CPU_C::LEAVE64 },
  /* CA */  { BxImmediate_Iw, &BX_CPU_C::RETfar16_Iw },
  /* CB */  { 0, &BX_CPU_C::RETfar16 },
  /* CC */  { 0, &BX_CPU_C::INT3 },
  /* CD */  { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE */  { 0, &BX_CPU_C::BxError },
  /* CF */  { 0, &BX_CPU_C::IRET16 },
  /* D0 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D1 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Ew },
  /* D2 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D3 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Ew },
  /* D4 */  { 0, &BX_CPU_C::BxError },
  /* D5 */  { 0, &BX_CPU_C::BxError },
  /* D6 */  { 0, &BX_CPU_C::BxError },
  /* D7 */  { 0, &BX_CPU_C::XLAT },
  /* D8 */  { BxAnother, &BX_CPU_C::ESC0 },
  /* D9 */  { BxAnother, &BX_CPU_C::ESC1 },
  /* DA */  { BxAnother, &BX_CPU_C::ESC2 },
  /* DB */  { BxAnother, &BX_CPU_C::ESC3 },
  /* DC */  { BxAnother, &BX_CPU_C::ESC4 },
  /* DD */  { BxAnother, &BX_CPU_C::ESC5 },
  /* DE */  { BxAnother, &BX_CPU_C::ESC6 },
  /* DF */  { BxAnother, &BX_CPU_C::ESC7 },
  /* E0 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 */  { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 */  { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 */  { BxImmediate_Ib, &BX_CPU_C::IN_eAXIb },
  /* E6 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbeAX },
  /* E8 */  { BxImmediate_BrOff16, &BX_CPU_C::CALL_Aw },
  /* E9 */  { BxImmediate_BrOff16, &BX_CPU_C::JMP_Jq },
  /* EA */  { 0, &BX_CPU_C::BxError },
  /* EB */  { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC */  { 0, &BX_CPU_C::IN_ALDX },
  /* ED */  { 0, &BX_CPU_C::IN_eAXDX },
  /* EE */  { 0, &BX_CPU_C::OUT_DXAL },
  /* EF */  { 0, &BX_CPU_C::OUT_DXeAX },
  /* F0 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // LOCK
  /* F1 */  { 0, &BX_CPU_C::INT1 },
  /* F2 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REP, REPE/REPZ
  /* F4 */  { 0, &BX_CPU_C::HLT },
  /* F5 */  { 0, &BX_CPU_C::CMC },
  /* F6 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Eb },
  /* F7 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Ew },
  /* F8 */  { 0, &BX_CPU_C::CLC },
  /* F9 */  { 0, &BX_CPU_C::STC },
  /* FA */  { 0, &BX_CPU_C::CLI },
  /* FB */  { 0, &BX_CPU_C::STI },
  /* FC */  { 0, &BX_CPU_C::CLD },
  /* FD */  { 0, &BX_CPU_C::STD },
  /* FE */  { BxAnother | BxGroup4, NULL, BxOpcodeInfo64G4 },
  /* FF */  { BxAnother | BxGroup5, NULL, BxOpcodeInfo64G5w },

  /* 0F 00 */  { BxAnother | BxGroup6, NULL, BxOpcodeInfo64G6 },
  /* 0F 01 */  { BxAnother | BxGroup7, NULL, BxOpcodeInfo64G7 },
  /* 0F 02 */  { BxAnother, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 */  { BxAnother, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 */  { 0, &BX_CPU_C::BxError },
  /* 0F 05 */  { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 */  { 0, &BX_CPU_C::CLTS },
  /* 0F 07 */  { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 */  { 0, &BX_CPU_C::INVD },
  /* 0F 09 */  { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A */  { 0, &BX_CPU_C::BxError },
  /* 0F 0B */  { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C */  { 0, &BX_CPU_C::BxError },
#if BX_SUPPORT_3DNOW
  /* 0F 0D */  { 0, &BX_CPU_C::NOP   },           // 3DNow! PREFETCH
  /* 0F 0E */  { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F */  { BxAnother | BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0D */  { 0, &BX_CPU_C::BxError },
  /* 0F 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F 0F */  { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 */  { BxAnother | BxGroup16, NULL, BxOpcodeInfo64G16 },
  /* 0F 19 */  { 0, &BX_CPU_C::BxError },
  /* 0F 1A */  { 0, &BX_CPU_C::BxError },
  /* 0F 1B */  { 0, &BX_CPU_C::BxError },
  /* 0F 1C */  { 0, &BX_CPU_C::BxError },
  /* 0F 1D */  { 0, &BX_CPU_C::BxError },
  /* 0F 1E */  { 0, &BX_CPU_C::BxError },
  /* 0F 1F */  { 0, &BX_CPU_C::BxError },
  /* 0F 20 */  { BxAnother, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 */  { BxAnother, &BX_CPU_C::MOV_RdDd },
  /* 0F 22 */  { BxAnother, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 */  { BxAnother, &BX_CPU_C::MOV_DdRd },
  /* 0F 24 */  { BxAnother, &BX_CPU_C::MOV_RdTd },
  /* 0F 25 */  { 0, &BX_CPU_C::BxError },
  /* 0F 26 */  { BxAnother, &BX_CPU_C::MOV_TdRd },
  /* 0F 27 */  { 0, &BX_CPU_C::BxError },
  /* 0F 28 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2b },
  /* 0F 2C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 */  { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 */  { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 */  { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 */  { 0, &BX_CPU_C::BxError },
  /* 0F 34 */  { 0, &BX_CPU_C::BxError },
  /* 0F 35 */  { 0, &BX_CPU_C::BxError },
  /* 0F 36 */  { 0, &BX_CPU_C::BxError },
  /* 0F 37 */  { 0, &BX_CPU_C::BxError },
  /* 0F 38 */  { 0, &BX_CPU_C::BxError },
  /* 0F 39 */  { 0, &BX_CPU_C::BxError },
  /* 0F 3A */  { 0, &BX_CPU_C::BxError },
  /* 0F 3B */  { 0, &BX_CPU_C::BxError },
  /* 0F 3C */  { 0, &BX_CPU_C::BxError },
  /* 0F 3D */  { 0, &BX_CPU_C::BxError },
  /* 0F 3E */  { 0, &BX_CPU_C::BxError },
  /* 0F 3F */  { 0, &BX_CPU_C::BxError },
  /* 0F 40 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 41 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 42 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 43 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 44 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 45 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 46 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 47 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 48 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 49 */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4A */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4B */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4C */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4D */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4E */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 4F */  { BxAnother, &BX_CPU_C::CMOV_GwEw },
  /* 0F 50 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 */  { BxAnother | BxGroup12, NULL, BxOpcodeInfo64G12 },
  /* 0F 72 */  { BxAnother | BxGroup13, NULL, BxOpcodeInfo64G13 },
  /* 0F 73 */  { BxAnother | BxGroup14, NULL, BxOpcodeInfo64G14 },
  /* 0F 74 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 */  { 0, &BX_CPU_C::EMMS }, 
  /* 0F 78 */  { 0, &BX_CPU_C::BxError },
  /* 0F 79 */  { 0, &BX_CPU_C::BxError },
  /* 0F 7A */  { 0, &BX_CPU_C::BxError },
  /* 0F 7B */  { 0, &BX_CPU_C::BxError },
  /* 0F 7C */  { 0, &BX_CPU_C::BxError },
  /* 0F 7D */  { 0, &BX_CPU_C::BxError },
  /* 0F 7E */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 81 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 82 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 83 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 84 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 85 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 86 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 87 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 88 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 89 */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8A */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8B */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8C */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8D */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8E */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 8F */  { BxImmediate_BrOff16, &BX_CPU_C::JCC_Jq },
  /* 0F 90 */  { BxAnother, &BX_CPU_C::SETO_Eb },
  /* 0F 91 */  { BxAnother, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 */  { BxAnother, &BX_CPU_C::SETB_Eb },
  /* 0F 93 */  { BxAnother, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 */  { BxAnother, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 */  { BxAnother, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 */  { BxAnother, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 */  { BxAnother, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 */  { BxAnother, &BX_CPU_C::SETS_Eb },
  /* 0F 99 */  { BxAnother, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A */  { BxAnother, &BX_CPU_C::SETP_Eb },
  /* 0F 9B */  { BxAnother, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C */  { BxAnother, &BX_CPU_C::SETL_Eb },
  /* 0F 9D */  { BxAnother, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E */  { BxAnother, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F */  { BxAnother, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 */  { 0, &BX_CPU_C::PUSH_FS },
  /* 0F A1 */  { 0, &BX_CPU_C::POP_FS },
  /* 0F A2 */  { 0, &BX_CPU_C::CPUID },
  /* 0F A3 */  { BxAnother, &BX_CPU_C::BT_EvGv },
  /* 0F A4 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHLD_EwGw },
  /* 0F A5 */  { BxAnother,                  &BX_CPU_C::SHLD_EwGw },
  /* 0F A6 */  { 0, &BX_CPU_C::CMPXCHG_XBTS },
  /* 0F A7 */  { 0, &BX_CPU_C::CMPXCHG_IBTS },
  /* 0F A8 */  { 0, &BX_CPU_C::PUSH_GS },
  /* 0F A9 */  { 0, &BX_CPU_C::POP_GS },
  /* 0F AA */  { 0, &BX_CPU_C::RSM },
  /* 0F AB */  { BxAnother | BxLockable, &BX_CPU_C::BTS_EvGv },
  /* 0F AC */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHRD_EwGw },
  /* 0F AD */  { BxAnother,                  &BX_CPU_C::SHRD_EwGw },
  /* 0F AE */  { BxAnother | BxGroup15, NULL, BxOpcodeInfo64G15 },
  /* 0F AF */  { BxAnother, &BX_CPU_C::IMUL_GwEw },
  /* 0F B0 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EbGb },
  /* 0F B1 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EwGw },
  /* 0F B2 */  { BxAnother, &BX_CPU_C::LSS_GvMp },
  /* 0F B3 */  { BxAnother | BxLockable, &BX_CPU_C::BTR_EvGv },
  /* 0F B4 */  { BxAnother, &BX_CPU_C::LFS_GvMp },
  /* 0F B5 */  { BxAnother, &BX_CPU_C::LGS_GvMp },
  /* 0F B6 */  { BxAnother, &BX_CPU_C::MOVZX_GwEb },
  /* 0F B7 */  { BxAnother, &BX_CPU_C::MOVZX_GwEw },
  /* 0F B8 */  { 0, &BX_CPU_C::BxError },
  /* 0F B9 */  { 0, &BX_CPU_C::BxError },
  /* 0F BA */  { BxAnother | BxGroup8, NULL, BxOpcodeInfo64G8EvIb },
  /* 0F BB */  { BxAnother | BxLockable, &BX_CPU_C::BTC_EvGv },
  /* 0F BC */  { BxAnother, &BX_CPU_C::BSF_GvEv },
  /* 0F BD */  { BxAnother, &BX_CPU_C::BSR_GvEv },
  /* 0F BE */  { BxAnother, &BX_CPU_C::MOVSX_GwEb },
  /* 0F BF */  { BxAnother, &BX_CPU_C::MOVSX_GwEw },
  /* 0F C0 */  { BxAnother | BxLockable, &BX_CPU_C::XADD_EbGb },
  /* 0F C1 */  { BxAnother | BxLockable, &BX_CPU_C::XADD_EwGw },
  /* 0F C2 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3 },
  /* 0F C4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 */  { BxAnother | BxGroup9, NULL, BxOpcodeInfo64G9 },
  /* 0F C8 */  { 0, &BX_CPU_C::BSWAP_EAX },
  /* 0F C9 */  { 0, &BX_CPU_C::BSWAP_ECX },
  /* 0F CA */  { 0, &BX_CPU_C::BSWAP_EDX },
  /* 0F CB */  { 0, &BX_CPU_C::BSWAP_EBX },
  /* 0F CC */  { 0, &BX_CPU_C::BSWAP_ESP },
  /* 0F CD */  { 0, &BX_CPU_C::BSWAP_EBP },
  /* 0F CE */  { 0, &BX_CPU_C::BSWAP_ESI },
  /* 0F CF */  { 0, &BX_CPU_C::BSWAP_EDI },
  /* 0F D0 */  { 0, &BX_CPU_C::BxError },
  /* 0F D1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7 },
  /* 0F E8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 */  { 0, &BX_CPU_C::BxError },
  /* 0F F1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF */  { 0, &BX_CPU_C::BxError },

  // 512 entries for 32bit mod
  /* 00 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EbGb },
  /* 01 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EdGd },
  /* 02 */  { BxAnother, &BX_CPU_C::ADD_GbEb },
  /* 03 */  { BxAnother | BxSplitMod11b, NULL, opcodesADD_GdEd },
  /* 04 */  { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 */  { BxImmediate_Iv, &BX_CPU_C::ADD_EAXId },
  /* 06 */  { 0, &BX_CPU_C::BxError },
  /* 07 */  { 0, &BX_CPU_C::BxError },
  /* 08 */  { BxAnother | BxLockable, &BX_CPU_C::OR_EbGb },
  /* 09 */  { BxAnother | BxLockable, &BX_CPU_C::OR_EdGd },
  /* 0A */  { BxAnother, &BX_CPU_C::OR_GbEb },
  /* 0B */  { BxAnother, &BX_CPU_C::OR_GdEd },
  /* 0C */  { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D */  { BxImmediate_Iv, &BX_CPU_C::OR_EAXId },
  /* 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F */  { BxAnother, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EbGb },
  /* 11 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EdGd },
  /* 12 */  { BxAnother, &BX_CPU_C::ADC_GbEb },
  /* 13 */  { BxAnother, &BX_CPU_C::ADC_GdEd },
  /* 14 */  { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 */  { BxImmediate_Iv, &BX_CPU_C::ADC_EAXId },
  /* 16 */  { 0, &BX_CPU_C::BxError },
  /* 17 */  { 0, &BX_CPU_C::BxError },
  /* 18 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EbGb },
  /* 19 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EdGd },
  /* 1A */  { BxAnother, &BX_CPU_C::SBB_GbEb },
  /* 1B */  { BxAnother, &BX_CPU_C::SBB_GdEd },
  /* 1C */  { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D */  { BxImmediate_Iv, &BX_CPU_C::SBB_EAXId },
  /* 1E */  { 0, &BX_CPU_C::BxError },
  /* 1F */  { 0, &BX_CPU_C::BxError },
  /* 20 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EbGb },
  /* 21 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EdGd },
  /* 22 */  { BxAnother, &BX_CPU_C::AND_GbEb },
  /* 23 */  { BxAnother, &BX_CPU_C::AND_GdEd },
  /* 24 */  { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 */  { BxImmediate_Iv, &BX_CPU_C::AND_EAXId },
  /* 26 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // ES:
  /* 27 */  { 0, &BX_CPU_C::BxError },
  /* 28 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EbGb },
  /* 29 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EdGd },
  /* 2A */  { BxAnother, &BX_CPU_C::SUB_GbEb },
  /* 2B */  { BxAnother, &BX_CPU_C::SUB_GdEd },
  /* 2C */  { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D */  { BxImmediate_Iv, &BX_CPU_C::SUB_EAXId },
  /* 2E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // CS:
  /* 2F */  { 0, &BX_CPU_C::BxError },
  /* 30 */  { BxAnother | BxLockable, &BX_CPU_C::XOR_EbGb },
  /* 31 */  { BxAnother | BxLockable, &BX_CPU_C::XOR_EdGd },
  /* 32 */  { BxAnother, &BX_CPU_C::XOR_GbEb },
  /* 33 */  { BxAnother, &BX_CPU_C::XOR_GdEd },
  /* 34 */  { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 */  { BxImmediate_Iv, &BX_CPU_C::XOR_EAXId },
  /* 36 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // SS:
  /* 37 */  { 0, &BX_CPU_C::BxError },
  /* 38 */  { BxAnother, &BX_CPU_C::CMP_EbGb },
  /* 39 */  { BxAnother, &BX_CPU_C::CMP_EdGd },
  /* 3A */  { BxAnother, &BX_CPU_C::CMP_GbEb },
  /* 3B */  { BxAnother, &BX_CPU_C::CMP_GdEd },
  /* 3C */  { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D */  { BxImmediate_Iv, &BX_CPU_C::CMP_EAXId },
  /* 3E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // DS:
  /* 3F */  { 0, &BX_CPU_C::BxError },
  /* 40 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 41 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 42 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 43 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 44 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 45 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 46 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 47 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 48 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 49 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4A */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4B */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4C */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4D */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4F */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 50 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 */  { 0, &BX_CPU_C::POP_RRX },
  /* 59 */  { 0, &BX_CPU_C::POP_RRX },
  /* 5A */  { 0, &BX_CPU_C::POP_RRX },
  /* 5B */  { 0, &BX_CPU_C::POP_RRX },
  /* 5C */  { 0, &BX_CPU_C::POP_RRX },
  /* 5D */  { 0, &BX_CPU_C::POP_RRX },
  /* 5E */  { 0, &BX_CPU_C::POP_RRX },
  /* 5F */  { 0, &BX_CPU_C::POP_RRX },
  /* 60 */  { 0, &BX_CPU_C::BxError },
  /* 61 */  { 0, &BX_CPU_C::BxError },
  /* 62 */  { 0, &BX_CPU_C::BxError },
#ifdef __GNUC__
#warning PRT: This needs checking on real hardware.  Manual says 32 bit version zero extends result
#endif
  /* 63 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GdEd },
  /* 64 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // FS:
  /* 65 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // GS:
  /* 66 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // OS:
  /* 67 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // AS:
  /* 68 */  { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::IMUL_GdEdId },
  /* 6A */  { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B */  { BxAnother | BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GdEdId },
  /* 6C */  { BxRepeatable, &BX_CPU_C::INSB_YbDX },
  /* 6D */  { BxRepeatable, &BX_CPU_C::INSW_YvDX },
  /* 6E */  { BxRepeatable, &BX_CPU_C::OUTSB_DXXb },
  /* 6F */  { BxRepeatable, &BX_CPU_C::OUTSW_DXXv },
  /* 70 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 71 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 72 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 73 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 74 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 75 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 76 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 77 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 78 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 79 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7A */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7B */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7C */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7D */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7E */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7F */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 80 */  { BxAnother | BxGroup1, NULL, BxOpcodeInfo64G1EbIb },
  /* 81 */  { BxAnother | BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfo64G1Ed },
  /* 82 */  { 0, &BX_CPU_C::BxError },
  /* 83 */  { BxAnother | BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfo64G1Ed },
  /* 84 */  { BxAnother, &BX_CPU_C::TEST_EbGb },
  /* 85 */  { BxAnother, &BX_CPU_C::TEST_EdGd },
  /* 86 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EbGb },
  /* 87 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EdGd },
  /* 88 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_EbGb },
  /* 89 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_EdGd },
  /* 8A */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GbEb },
  /* 8B */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GdEd },
  /* 8C */  { BxAnother, &BX_CPU_C::MOV_EwSw },
  /* 8D */  { BxAnother, &BX_CPU_C::LEA_GdM },
  /* 8E */  { BxAnother, &BX_CPU_C::MOV_SwEw },
  /* 8F */  { BxAnother, &BX_CPU_C::POP_Eq },
  /* 90 */  { 0, &BX_CPU_C::NOP },
  /* 91 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 92 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 93 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 94 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 95 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 96 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 97 */  { 0, &BX_CPU_C::XCHG_ERXEAX },
  /* 98 */  { 0, &BX_CPU_C::CWDE },
  /* 99 */  { 0, &BX_CPU_C::CDQ },
  /* 9A */  { 0, &BX_CPU_C::BxError },
  /* 9B */  { 0, &BX_CPU_C::FWAIT },
  /* 9C */  { 0, &BX_CPU_C::PUSHF_Fv },
  /* 9D */  { 0, &BX_CPU_C::POPF_Fv },
  /* 9E */  { 0, &BX_CPU_C::BxError },
  /* 9F */  { 0, &BX_CPU_C::BxError },
  /* A0 */  { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 */  { BxImmediate_O, &BX_CPU_C::MOV_EAXOq },
  /* A2 */  { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 */  { BxImmediate_O, &BX_CPU_C::MOV_OqEAX },
  /* A4 */  { BxRepeatable, &BX_CPU_C::MOVSB_XbYb },
  /* A5 */  { BxRepeatable, &BX_CPU_C::MOVSW_XvYv },
  /* A6 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSB_XbYb },
  /* A7 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSW_XvYv },
  /* A8 */  { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 */  { BxImmediate_Iv, &BX_CPU_C::TEST_EAXId },
  /* AA */  { BxRepeatable, &BX_CPU_C::STOSB_YbAL },
  /* AB */  { BxRepeatable, &BX_CPU_C::STOSW_YveAX },
  /* AC */  { BxRepeatable, &BX_CPU_C::LODSB_ALXb },
  /* AD */  { BxRepeatable, &BX_CPU_C::LODSW_eAXXv },
  /* AE */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASB_ALXb },
  /* AF */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASW_eAXXv },
  /* B0 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* B9 */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BA */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BB */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BC */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BD */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BE */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* BF */  { BxImmediate_Iv, &BX_CPU_C::MOV_ERXId },
  /* C0 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eb },
  /* C1 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Ed },
  /* C2 */  { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 */  { 0,             &BX_CPU_C::RETnear64 },
  /* C4 */  { 0, &BX_CPU_C::BxError },
  /* C5 */  { 0, &BX_CPU_C::BxError },
  /* C6 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::MOV_EbIb },
  /* C7 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::MOV_EdId },
  /* C8 */  { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 */  { 0, &BX_CPU_C::LEAVE64 },
  /* CA */  { BxImmediate_Iw, &BX_CPU_C::RETfar32_Iw },
  /* CB */  { 0, &BX_CPU_C::RETfar32 },
  /* CC */  { 0, &BX_CPU_C::INT3 },
  /* CD */  { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE */  { 0, &BX_CPU_C::BxError },
  /* CF */  { 0, &BX_CPU_C::IRET32 },
  /* D0 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D1 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Ed },
  /* D2 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D3 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Ed },
  /* D4 */  { 0, &BX_CPU_C::BxError },
  /* D5 */  { 0, &BX_CPU_C::BxError },
  /* D6 */  { 0, &BX_CPU_C::BxError },
  /* D7 */  { 0, &BX_CPU_C::XLAT },
  /* D8 */  { BxAnother, &BX_CPU_C::ESC0 },
  /* D9 */  { BxAnother, &BX_CPU_C::ESC1 },
  /* DA */  { BxAnother, &BX_CPU_C::ESC2 },
  /* DB */  { BxAnother, &BX_CPU_C::ESC3 },
  /* DC */  { BxAnother, &BX_CPU_C::ESC4 },
  /* DD */  { BxAnother, &BX_CPU_C::ESC5 },
  /* DE */  { BxAnother, &BX_CPU_C::ESC6 },
  /* DF */  { BxAnother, &BX_CPU_C::ESC7 },
  /* E0 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 */  { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 */  { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 */  { BxImmediate_Ib, &BX_CPU_C::IN_eAXIb },
  /* E6 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbeAX },
  /* E8 */  { BxImmediate_BrOff32, &BX_CPU_C::CALL_Aq },
  /* E9 */  { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA */  { 0, &BX_CPU_C::BxError },
  /* EB */  { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC */  { 0, &BX_CPU_C::IN_ALDX },
  /* ED */  { 0, &BX_CPU_C::IN_eAXDX },
  /* EE */  { 0, &BX_CPU_C::OUT_DXAL },
  /* EF */  { 0, &BX_CPU_C::OUT_DXeAX },
  /* F0 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // LOCK:
  /* F1 */  { 0, &BX_CPU_C::INT1 },
  /* F2 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 */  { 0, &BX_CPU_C::HLT },
  /* F5 */  { 0, &BX_CPU_C::CMC },
  /* F6 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Eb },
  /* F7 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Ed },
  /* F8 */  { 0, &BX_CPU_C::CLC },
  /* F9 */  { 0, &BX_CPU_C::STC },
  /* FA */  { 0, &BX_CPU_C::CLI },
  /* FB */  { 0, &BX_CPU_C::STI },
  /* FC */  { 0, &BX_CPU_C::CLD },
  /* FD */  { 0, &BX_CPU_C::STD },
  /* FE */  { BxAnother | BxGroup4, NULL, BxOpcodeInfo64G4 },
  /* FF */  { BxAnother | BxGroup5, NULL, BxOpcodeInfo64G5d },

  /* 0F 00 */  { BxAnother | BxGroup6, NULL, BxOpcodeInfo64G6 },
  /* 0F 01 */  { BxAnother | BxGroup7, NULL, BxOpcodeInfo64G7 },
  /* 0F 02 */  { BxAnother, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 */  { BxAnother, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 */  { 0, &BX_CPU_C::BxError },
  /* 0F 05 */  { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 */  { 0, &BX_CPU_C::CLTS },
  /* 0F 07 */  { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 */  { 0, &BX_CPU_C::INVD },
  /* 0F 09 */  { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A */  { 0, &BX_CPU_C::BxError },
  /* 0F 0B */  { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C */  { 0, &BX_CPU_C::BxError },
#if BX_SUPPORT_3DNOW
  /* 0F 0D */  { 0, &BX_CPU_C::NOP   },           // 3DNow! PREFETCH
  /* 0F 0E */  { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F */  { BxAnother | BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0D */  { 0, &BX_CPU_C::BxError },
  /* 0F 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F 0F */  { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 */  { BxAnother | BxGroup16, NULL, BxOpcodeInfo64G16 },
  /* 0F 19 */  { 0, &BX_CPU_C::BxError },
  /* 0F 1A */  { 0, &BX_CPU_C::BxError },
  /* 0F 1B */  { 0, &BX_CPU_C::BxError },
  /* 0F 1C */  { 0, &BX_CPU_C::BxError },
  /* 0F 1D */  { 0, &BX_CPU_C::BxError },
  /* 0F 1E */  { 0, &BX_CPU_C::BxError },
  /* 0F 1F */  { 0, &BX_CPU_C::BxError },
  /* 0F 20 */  { BxAnother, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 */  { BxAnother, &BX_CPU_C::MOV_RdDd },
  /* 0F 22 */  { BxAnother, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 */  { BxAnother, &BX_CPU_C::MOV_DdRd },
  /* 0F 24 */  { BxAnother, &BX_CPU_C::MOV_RdTd },
  /* 0F 25 */  { 0, &BX_CPU_C::BxError },
  /* 0F 26 */  { BxAnother, &BX_CPU_C::MOV_TdRd },
  /* 0F 27 */  { 0, &BX_CPU_C::BxError },
  /* 0F 28 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2b },
  /* 0F 2C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 */  { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 */  { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 */  { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 */  { 0, &BX_CPU_C::BxError },
  /* 0F 34 */  { 0, &BX_CPU_C::BxError },
  /* 0F 35 */  { 0, &BX_CPU_C::BxError },
  /* 0F 36 */  { 0, &BX_CPU_C::BxError },
  /* 0F 37 */  { 0, &BX_CPU_C::BxError },
  /* 0F 38 */  { 0, &BX_CPU_C::BxError },
  /* 0F 39 */  { 0, &BX_CPU_C::BxError },
  /* 0F 3A */  { 0, &BX_CPU_C::BxError },
  /* 0F 3B */  { 0, &BX_CPU_C::BxError },
  /* 0F 3C */  { 0, &BX_CPU_C::BxError },
  /* 0F 3D */  { 0, &BX_CPU_C::BxError },
  /* 0F 3E */  { 0, &BX_CPU_C::BxError },
  /* 0F 3F */  { 0, &BX_CPU_C::BxError },
  /* 0F 40 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 41 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 42 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 43 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 44 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 45 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 46 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 47 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 48 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 49 */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4A */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4B */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4C */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4D */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4E */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 4F */  { BxAnother, &BX_CPU_C::CMOV_GdEd },
  /* 0F 50 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 */  { BxAnother | BxGroup12, NULL, BxOpcodeInfo64G12 },
  /* 0F 72 */  { BxAnother | BxGroup13, NULL, BxOpcodeInfo64G13 },
  /* 0F 73 */  { BxAnother | BxGroup14, NULL, BxOpcodeInfo64G14 },
  /* 0F 74 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 */  { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 */  { 0, &BX_CPU_C::BxError },
  /* 0F 79 */  { 0, &BX_CPU_C::BxError },
  /* 0F 7A */  { 0, &BX_CPU_C::BxError },
  /* 0F 7B */  { 0, &BX_CPU_C::BxError },
  /* 0F 7C */  { 0, &BX_CPU_C::BxError },
  /* 0F 7D */  { 0, &BX_CPU_C::BxError },
  /* 0F 7E */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 81 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 82 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 83 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 84 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 85 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 86 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 87 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 88 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 89 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8A */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8B */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8C */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8D */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8E */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8F */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 90 */  { BxAnother, &BX_CPU_C::SETO_Eb },
  /* 0F 91 */  { BxAnother, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 */  { BxAnother, &BX_CPU_C::SETB_Eb },
  /* 0F 93 */  { BxAnother, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 */  { BxAnother, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 */  { BxAnother, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 */  { BxAnother, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 */  { BxAnother, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 */  { BxAnother, &BX_CPU_C::SETS_Eb },
  /* 0F 99 */  { BxAnother, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A */  { BxAnother, &BX_CPU_C::SETP_Eb },
  /* 0F 9B */  { BxAnother, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C */  { BxAnother, &BX_CPU_C::SETL_Eb },
  /* 0F 9D */  { BxAnother, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E */  { BxAnother, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F */  { BxAnother, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 */  { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 */  { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 */  { 0, &BX_CPU_C::CPUID },
  /* 0F A3 */  { BxAnother, &BX_CPU_C::BT_EvGv },
  /* 0F A4 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHLD_EdGd },
  /* 0F A5 */  { BxAnother,                  &BX_CPU_C::SHLD_EdGd },
  /* 0F A6 */  { 0, &BX_CPU_C::CMPXCHG_XBTS },
  /* 0F A7 */  { 0, &BX_CPU_C::CMPXCHG_IBTS },
  /* 0F A8 */  { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 */  { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA */  { 0, &BX_CPU_C::RSM },
  /* 0F AB */  { BxAnother | BxLockable, &BX_CPU_C::BTS_EvGv },
  /* 0F AC */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHRD_EdGd },
  /* 0F AD */  { BxAnother,                  &BX_CPU_C::SHRD_EdGd },
  /* 0F AE */  { BxAnother | BxGroup15, NULL, BxOpcodeInfo64G15 },
  /* 0F AF */  { BxAnother, &BX_CPU_C::IMUL_GdEd },
  /* 0F B0 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EbGb },
  /* 0F B1 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EdGd },
  /* 0F B2 */  { BxAnother, &BX_CPU_C::LSS_GvMp },
  /* 0F B3 */  { BxAnother | BxLockable, &BX_CPU_C::BTR_EvGv },
  /* 0F B4 */  { BxAnother, &BX_CPU_C::LFS_GvMp },
  /* 0F B5 */  { BxAnother, &BX_CPU_C::LGS_GvMp },
  /* 0F B6 */  { BxAnother, &BX_CPU_C::MOVZX_GdEb },
  /* 0F B7 */  { BxAnother, &BX_CPU_C::MOVZX_GdEw },
  /* 0F B8 */  { 0, &BX_CPU_C::BxError },
  /* 0F B9 */  { 0, &BX_CPU_C::BxError },
  /* 0F BA */  { BxAnother | BxGroup8, NULL, BxOpcodeInfo64G8EvIb },
  /* 0F BB */  { BxAnother | BxLockable, &BX_CPU_C::BTC_EvGv },
  /* 0F BC */  { BxAnother, &BX_CPU_C::BSF_GvEv },
  /* 0F BD */  { BxAnother, &BX_CPU_C::BSR_GvEv },
  /* 0F BE */  { BxAnother, &BX_CPU_C::MOVSX_GdEb },
  /* 0F BF */  { BxAnother, &BX_CPU_C::MOVSX_GdEw },
  /* 0F C0 */  { BxAnother | BxLockable, &BX_CPU_C::XADD_EbGb },
  /* 0F C1 */  { BxAnother | BxLockable, &BX_CPU_C::XADD_EdGd },
  /* 0F C2 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3 },
  /* 0F C4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 */  { BxAnother | BxGroup9, NULL, BxOpcodeInfo64G9 },
  /* 0F C8 */  { 0, &BX_CPU_C::BSWAP_EAX },
  /* 0F C9 */  { 0, &BX_CPU_C::BSWAP_ECX },
  /* 0F CA */  { 0, &BX_CPU_C::BSWAP_EDX },
  /* 0F CB */  { 0, &BX_CPU_C::BSWAP_EBX },
  /* 0F CC */  { 0, &BX_CPU_C::BSWAP_ESP },
  /* 0F CD */  { 0, &BX_CPU_C::BSWAP_EBP },
  /* 0F CE */  { 0, &BX_CPU_C::BSWAP_ESI },
  /* 0F CF */  { 0, &BX_CPU_C::BSWAP_EDI },
  /* 0F D0 */  { 0, &BX_CPU_C::BxError },
  /* 0F D1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7 },
  /* 0F E8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 */  { 0, &BX_CPU_C::BxError },
  /* 0F F1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF */  { 0, &BX_CPU_C::BxError },

  // 512 entries for 64bit mod
  /* 00 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EbGb },
  /* 01 */  { BxAnother | BxLockable, &BX_CPU_C::ADD_EqGq },
  /* 02 */  { BxAnother, &BX_CPU_C::ADD_GbEb },
  /* 03 */  { BxAnother, &BX_CPU_C::ADD_GqEq },
  /* 04 */  { BxImmediate_Ib, &BX_CPU_C::ADD_ALIb },
  /* 05 */  { BxImmediate_Iv, &BX_CPU_C::ADD_RAXId },
  /* 06 */  { 0, &BX_CPU_C::BxError },
  /* 07 */  { 0, &BX_CPU_C::BxError },
  /* 08 */  { BxAnother, &BX_CPU_C::OR_EbGb },
  /* 09 */  { BxAnother | BxLockable, &BX_CPU_C::OR_EqGq },
  /* 0A */  { BxAnother | BxLockable, &BX_CPU_C::OR_GbEb },
  /* 0B */  { BxAnother, &BX_CPU_C::OR_GqEq },
  /* 0C */  { BxImmediate_Ib, &BX_CPU_C::OR_ALIb },
  /* 0D */  { BxImmediate_Iv, &BX_CPU_C::OR_RAXId },
  /* 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F */  { BxAnother, &BX_CPU_C::BxError }, // 2-byte escape
  /* 10 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EbGb },
  /* 11 */  { BxAnother | BxLockable, &BX_CPU_C::ADC_EqGq },
  /* 12 */  { BxAnother, &BX_CPU_C::ADC_GbEb },
  /* 13 */  { BxAnother, &BX_CPU_C::ADC_GqEq },
  /* 14 */  { BxImmediate_Ib, &BX_CPU_C::ADC_ALIb },
  /* 15 */  { BxImmediate_Iv, &BX_CPU_C::ADC_RAXId },
  /* 16 */  { 0, &BX_CPU_C::BxError },
  /* 17 */  { 0, &BX_CPU_C::BxError },
  /* 18 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EbGb },
  /* 19 */  { BxAnother | BxLockable, &BX_CPU_C::SBB_EqGq },
  /* 1A */  { BxAnother, &BX_CPU_C::SBB_GbEb },
  /* 1B */  { BxAnother, &BX_CPU_C::SBB_GqEq },
  /* 1C */  { BxImmediate_Ib, &BX_CPU_C::SBB_ALIb },
  /* 1D */  { BxImmediate_Iv, &BX_CPU_C::SBB_RAXId },
  /* 1E */  { 0, &BX_CPU_C::BxError },
  /* 1F */  { 0, &BX_CPU_C::BxError },
  /* 20 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EbGb },
  /* 21 */  { BxAnother | BxLockable, &BX_CPU_C::AND_EqGq },
  /* 22 */  { BxAnother, &BX_CPU_C::AND_GbEb },
  /* 23 */  { BxAnother, &BX_CPU_C::AND_GqEq },
  /* 24 */  { BxImmediate_Ib, &BX_CPU_C::AND_ALIb },
  /* 25 */  { BxImmediate_Iv, &BX_CPU_C::AND_RAXId },
  /* 26 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // ES:
  /* 27 */  { 0, &BX_CPU_C::BxError },
  /* 28 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EbGb },
  /* 29 */  { BxAnother | BxLockable, &BX_CPU_C::SUB_EqGq },
  /* 2A */  { BxAnother, &BX_CPU_C::SUB_GbEb },
  /* 2B */  { BxAnother, &BX_CPU_C::SUB_GqEq },
  /* 2C */  { BxImmediate_Ib, &BX_CPU_C::SUB_ALIb },
  /* 2D */  { BxImmediate_Iv, &BX_CPU_C::SUB_RAXId },
  /* 2E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // CS:
  /* 2F */  { 0, &BX_CPU_C::BxError },
  /* 30 */  { BxAnother | BxLockable, &BX_CPU_C::XOR_EbGb },
  /* 31 */  { BxAnother | BxLockable, &BX_CPU_C::XOR_EqGq },
  /* 32 */  { BxAnother, &BX_CPU_C::XOR_GbEb },
  /* 33 */  { BxAnother, &BX_CPU_C::XOR_GqEq },
  /* 34 */  { BxImmediate_Ib, &BX_CPU_C::XOR_ALIb },
  /* 35 */  { BxImmediate_Iv, &BX_CPU_C::XOR_RAXId },
  /* 36 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // SS:
  /* 37 */  { 0, &BX_CPU_C::BxError },
  /* 38 */  { BxAnother, &BX_CPU_C::CMP_EbGb },
  /* 39 */  { BxAnother, &BX_CPU_C::CMP_EqGq },
  /* 3A */  { BxAnother, &BX_CPU_C::CMP_GbEb },
  /* 3B */  { BxAnother, &BX_CPU_C::CMP_GqEq },
  /* 3C */  { BxImmediate_Ib, &BX_CPU_C::CMP_ALIb },
  /* 3D */  { BxImmediate_Iv, &BX_CPU_C::CMP_RAXId },
  /* 3E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // DS:
  /* 3F */  { 0, &BX_CPU_C::BxError },
  /* 40 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 41 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 42 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 43 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 44 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 45 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 46 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 47 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 48 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 49 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4A */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4B */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4C */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4D */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4E */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 4F */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REX:
  /* 50 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 51 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 52 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 53 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 54 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 55 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 56 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 57 */  { 0, &BX_CPU_C::PUSH_RRX },
  /* 58 */  { 0, &BX_CPU_C::POP_RRX },
  /* 59 */  { 0, &BX_CPU_C::POP_RRX },
  /* 5A */  { 0, &BX_CPU_C::POP_RRX },
  /* 5B */  { 0, &BX_CPU_C::POP_RRX },
  /* 5C */  { 0, &BX_CPU_C::POP_RRX },
  /* 5D */  { 0, &BX_CPU_C::POP_RRX },
  /* 5E */  { 0, &BX_CPU_C::POP_RRX },
  /* 5F */  { 0, &BX_CPU_C::POP_RRX },
  /* 60 */  { 0, &BX_CPU_C::BxError },
  /* 61 */  { 0, &BX_CPU_C::BxError },
  /* 62 */  { 0, &BX_CPU_C::BxError },
  /* 63 */  { BxAnother, &BX_CPU_C::MOVSX_GqEd },
  /* 64 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // FS:
  /* 65 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // GS:
  /* 66 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // OS:
  /* 67 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // AS:
  /* 68 */  { BxImmediate_Iv, &BX_CPU_C::PUSH64_Id },
  /* 69 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::IMUL_GqEqId },
  /* 6A */  { BxImmediate_Ib_SE, &BX_CPU_C::PUSH64_Id },
  /* 6B */  { BxAnother | BxImmediate_Ib_SE, &BX_CPU_C::IMUL_GqEqId },
  /* 6C */  { BxRepeatable, &BX_CPU_C::INSB_YbDX },
  /* 6D */  { BxRepeatable, &BX_CPU_C::INSW_YvDX },
  /* 6E */  { BxRepeatable, &BX_CPU_C::OUTSB_DXXb },
  /* 6F */  { BxRepeatable, &BX_CPU_C::OUTSW_DXXv },
  /* 70 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 71 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 72 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 73 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 74 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 75 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 76 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 77 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 78 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 79 */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7A */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7B */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7C */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7D */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7E */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 7F */  { BxImmediate_BrOff8, &BX_CPU_C::JCC_Jq },
  /* 80 */  { BxAnother | BxGroup1, NULL, BxOpcodeInfo64G1EbIb },
  /* 81 */  { BxAnother | BxGroup1 | BxImmediate_Iv, NULL, BxOpcodeInfo64G1Eq },
  /* 82 */  { 0, &BX_CPU_C::BxError },
  /* 83 */  { BxAnother | BxGroup1 | BxImmediate_Ib_SE, NULL, BxOpcodeInfo64G1Eq },
  /* 84 */  { BxAnother, &BX_CPU_C::TEST_EbGb },
  /* 85 */  { BxAnother, &BX_CPU_C::TEST_EqGq },
  /* 86 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EbGb },
  /* 87 */  { BxAnother | BxLockable, &BX_CPU_C::XCHG_EqGq },
  /* 88 */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_EbGb },
  /* 89 */  { BxAnother, &BX_CPU_C::MOV_EqGq },
  /* 8A */  { BxAnother | BxSplitMod11b, NULL, opcodesMOV_GbEb },
  /* 8B */  { BxAnother, &BX_CPU_C::MOV_GqEq },
  /* 8C */  { BxAnother, &BX_CPU_C::MOV_EwSw },
  /* 8D */  { BxAnother, &BX_CPU_C::LEA_GqM },
  /* 8E */  { BxAnother, &BX_CPU_C::MOV_SwEw },
  /* 8F */  { BxAnother, &BX_CPU_C::POP_Eq },
  /* 90 */  { 0, &BX_CPU_C::NOP },
  /* 91 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 92 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 93 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 94 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 95 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 96 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 97 */  { 0, &BX_CPU_C::XCHG_RRXRAX },
  /* 98 */  { 0, &BX_CPU_C::CDQE },
  /* 99 */  { 0, &BX_CPU_C::CQO },
  /* 9A */  { 0, &BX_CPU_C::BxError },
  /* 9B */  { 0, &BX_CPU_C::FWAIT },
  /* 9C */  { 0, &BX_CPU_C::PUSHF_Fv },
  /* 9D */  { 0, &BX_CPU_C::POPF_Fv },
  /* 9E */  { 0, &BX_CPU_C::BxError },
  /* 9F */  { 0, &BX_CPU_C::BxError },
  /* A0 */  { BxImmediate_O, &BX_CPU_C::MOV_ALOq },
  /* A1 */  { BxImmediate_O, &BX_CPU_C::MOV_RAXOq },
  /* A2 */  { BxImmediate_O, &BX_CPU_C::MOV_OqAL },
  /* A3 */  { BxImmediate_O, &BX_CPU_C::MOV_OqRAX },
  /* A4 */  { BxRepeatable, &BX_CPU_C::MOVSB_XbYb },
  /* A5 */  { BxRepeatable, &BX_CPU_C::MOVSW_XvYv },
  /* A6 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSB_XbYb },
  /* A7 */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::CMPSW_XvYv },
  /* A8 */  { BxImmediate_Ib, &BX_CPU_C::TEST_ALIb },
  /* A9 */  { BxImmediate_Iv, &BX_CPU_C::TEST_RAXId },
  /* AA */  { BxRepeatable, &BX_CPU_C::STOSB_YbAL },
  /* AB */  { BxRepeatable, &BX_CPU_C::STOSW_YveAX },
  /* AC */  { BxRepeatable, &BX_CPU_C::LODSB_ALXb },
  /* AD */  { BxRepeatable, &BX_CPU_C::LODSW_eAXXv },
  /* AE */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASB_ALXb },
  /* AF */  { BxRepeatable | BxRepeatableZF, &BX_CPU_C::SCASW_eAXXv },
  /* B0 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B1 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B2 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B3 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B4 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B5 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B6 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B7 */  { BxImmediate_Ib, &BX_CPU_C::MOV_RLIb },
  /* B8 */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* B9 */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BA */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BB */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BC */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BD */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BE */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* BF */  { BxImmediate_Iq, &BX_CPU_C::MOV_RRXIq },
  /* C0 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eb },
  /* C1 */  { BxAnother | BxGroup2 | BxImmediate_Ib, NULL, BxOpcodeInfo64G2Eq },
  /* C2 */  { BxImmediate_Iw, &BX_CPU_C::RETnear64_Iw },
  /* C3 */  { 0,              &BX_CPU_C::RETnear64 },
  /* C4 */  { 0, &BX_CPU_C::BxError },
  /* C5 */  { 0, &BX_CPU_C::BxError },
  /* C6 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::MOV_EbIb },
  /* C7 */  { BxAnother | BxImmediate_Iv, &BX_CPU_C::MOV_EqId },
  /* C8 */  { BxImmediate_IwIb, &BX_CPU_C::ENTER64_IwIb },
  /* C9 */  { 0, &BX_CPU_C::LEAVE64 },
  /* CA */  { BxImmediate_Iw, &BX_CPU_C::RETfar64_Iw },
  /* CB */  { 0, &BX_CPU_C::RETfar64 },
  /* CC */  { 0, &BX_CPU_C::INT3 },
  /* CD */  { BxImmediate_Ib, &BX_CPU_C::INT_Ib },
  /* CE */  { 0, &BX_CPU_C::BxError },
  /* CF */  { 0, &BX_CPU_C::IRET64 },
  /* D0 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D1 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D2 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eb },
  /* D3 */  { BxAnother | BxGroup2, NULL, BxOpcodeInfo64G2Eq },
  /* D4 */  { 0, &BX_CPU_C::BxError },
  /* D5 */  { 0, &BX_CPU_C::BxError },
  /* D6 */  { 0, &BX_CPU_C::BxError },
  /* D7 */  { 0, &BX_CPU_C::XLAT },
  /* D8 */  { BxAnother, &BX_CPU_C::ESC0 },
  /* D9 */  { BxAnother, &BX_CPU_C::ESC1 },
  /* DA */  { BxAnother, &BX_CPU_C::ESC2 },
  /* DB */  { BxAnother, &BX_CPU_C::ESC3 },
  /* DC */  { BxAnother, &BX_CPU_C::ESC4 },
  /* DD */  { BxAnother, &BX_CPU_C::ESC5 },
  /* DE */  { BxAnother, &BX_CPU_C::ESC6 },
  /* DF */  { BxAnother, &BX_CPU_C::ESC7 },
  /* E0 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPNE64_Jb },
  /* E1 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOPE64_Jb },
  /* E2 */  { BxImmediate_BrOff8, &BX_CPU_C::LOOP64_Jb },
  /* E3 */  { BxImmediate_BrOff8, &BX_CPU_C::JCXZ64_Jb },
  /* E4 */  { BxImmediate_Ib, &BX_CPU_C::IN_ALIb },
  /* E5 */  { BxImmediate_Ib, &BX_CPU_C::IN_eAXIb },
  /* E6 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbAL },
  /* E7 */  { BxImmediate_Ib, &BX_CPU_C::OUT_IbeAX },
  /* E8 */  { BxImmediate_BrOff32, &BX_CPU_C::CALL_Aq },
  /* E9 */  { BxImmediate_BrOff32, &BX_CPU_C::JMP_Jq },
  /* EA */  { 0, &BX_CPU_C::BxError },
  /* EB */  { BxImmediate_BrOff8, &BX_CPU_C::JMP_Jq },
  /* EC */  { 0, &BX_CPU_C::IN_ALDX },
  /* ED */  { 0, &BX_CPU_C::IN_eAXDX },
  /* EE */  { 0, &BX_CPU_C::OUT_DXAL },
  /* EF */  { 0, &BX_CPU_C::OUT_DXeAX },
  /* F0 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // LOCK:
  /* F1 */  { 0, &BX_CPU_C::INT1 },
  /* F2 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REPNE/REPNZ
  /* F3 */  { BxPrefix | BxAnother, &BX_CPU_C::BxError }, // REP,REPE/REPZ
  /* F4 */  { 0, &BX_CPU_C::HLT },
  /* F5 */  { 0, &BX_CPU_C::CMC },
  /* F6 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Eb },
  /* F7 */  { BxAnother | BxGroup3, NULL, BxOpcodeInfo64G3Eq },
  /* F8 */  { 0, &BX_CPU_C::CLC },
  /* F9 */  { 0, &BX_CPU_C::STC },
  /* FA */  { 0, &BX_CPU_C::CLI },
  /* FB */  { 0, &BX_CPU_C::STI },
  /* FC */  { 0, &BX_CPU_C::CLD },
  /* FD */  { 0, &BX_CPU_C::STD },
  /* FE */  { BxAnother | BxGroup4, NULL, BxOpcodeInfo64G4 },
  /* FF */  { BxAnother | BxGroup5, NULL, BxOpcodeInfo64G5q },

  /* 0F 00 */  { BxAnother | BxGroup6, NULL, BxOpcodeInfo64G6 },
  /* 0F 01 */  { BxAnother | BxGroup7, NULL, BxOpcodeInfo64G7 },
  /* 0F 02 */  { BxAnother, &BX_CPU_C::LAR_GvEw },
  /* 0F 03 */  { BxAnother, &BX_CPU_C::LSL_GvEw },
  /* 0F 04 */  { 0, &BX_CPU_C::BxError },
  /* 0F 05 */  { 0, &BX_CPU_C::SYSCALL },
  /* 0F 06 */  { 0, &BX_CPU_C::CLTS },
  /* 0F 07 */  { 0, &BX_CPU_C::SYSRET },
  /* 0F 08 */  { 0, &BX_CPU_C::INVD },
  /* 0F 09 */  { 0, &BX_CPU_C::WBINVD },
  /* 0F 0A */  { 0, &BX_CPU_C::BxError },
  /* 0F 0B */  { 0, &BX_CPU_C::UndefinedOpcode }, // UD2 opcode
  /* 0F 0C */  { 0, &BX_CPU_C::BxError },
#if BX_SUPPORT_3DNOW
  /* 0F 0D */  { 0, &BX_CPU_C::NOP   },           // 3DNow! PREFETCH
  /* 0F 0E */  { 0, &BX_CPU_C::EMMS },            // 3DNow! FEMMS
  /* 0F 0F */  { BxAnother | BxImmediate_Ib, NULL, Bx3DNowOpcodeInfo },
#else
  /* 0F 0D */  { 0, &BX_CPU_C::BxError },
  /* 0F 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F 0F */  { 0, &BX_CPU_C::BxError },
#endif
  /* 0F 10 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f10 },
  /* 0F 11 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f11 },
  /* 0F 12 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f12 },
  /* 0F 13 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f13 },
  /* 0F 14 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f14 },
  /* 0F 15 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f15 },
  /* 0F 16 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f16 },
  /* 0F 17 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f17 },
  /* 0F 18 */  { BxAnother | BxGroup16, NULL, BxOpcodeInfo64G16 },
  /* 0F 19 */  { 0, &BX_CPU_C::BxError },
  /* 0F 1A */  { 0, &BX_CPU_C::BxError },
  /* 0F 1B */  { 0, &BX_CPU_C::BxError },
  /* 0F 1C */  { 0, &BX_CPU_C::BxError },
  /* 0F 1D */  { 0, &BX_CPU_C::BxError },
  /* 0F 1E */  { 0, &BX_CPU_C::BxError },
  /* 0F 1F */  { 0, &BX_CPU_C::BxError },
  /* 0F 20 */  { BxAnother, &BX_CPU_C::MOV_RqCq },
  /* 0F 21 */  { BxAnother, &BX_CPU_C::MOV_RqDq },
  /* 0F 22 */  { BxAnother, &BX_CPU_C::MOV_CqRq },
  /* 0F 23 */  { BxAnother, &BX_CPU_C::MOV_DqRq },
  /* 0F 24 */  { 0, &BX_CPU_C::BxError },
  /* 0F 25 */  { 0, &BX_CPU_C::BxError },
  /* 0F 26 */  { 0, &BX_CPU_C::BxError },
  /* 0F 27 */  { 0, &BX_CPU_C::BxError },
  /* 0F 28 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f28 },
  /* 0F 29 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f29 },
  /* 0F 2A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2a },
  /* 0F 2B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2b },
  /* 0F 2C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2c },
  /* 0F 2D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2d },
  /* 0F 2E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2e },
  /* 0F 2F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f2f },
  /* 0F 30 */  { 0, &BX_CPU_C::WRMSR },
  /* 0F 31 */  { 0, &BX_CPU_C::RDTSC },
  /* 0F 32 */  { 0, &BX_CPU_C::RDMSR },
  /* 0F 33 */  { 0, &BX_CPU_C::BxError },
  /* 0F 34 */  { 0, &BX_CPU_C::BxError },
  /* 0F 35 */  { 0, &BX_CPU_C::BxError },
  /* 0F 36 */  { 0, &BX_CPU_C::BxError },
  /* 0F 37 */  { 0, &BX_CPU_C::BxError },
  /* 0F 38 */  { 0, &BX_CPU_C::BxError },
  /* 0F 39 */  { 0, &BX_CPU_C::BxError },
  /* 0F 3A */  { 0, &BX_CPU_C::BxError },
  /* 0F 3B */  { 0, &BX_CPU_C::BxError },
  /* 0F 3C */  { 0, &BX_CPU_C::BxError },
  /* 0F 3D */  { 0, &BX_CPU_C::BxError },
  /* 0F 3E */  { 0, &BX_CPU_C::BxError },
  /* 0F 3F */  { 0, &BX_CPU_C::BxError },
  /* 0F 40 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 41 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 42 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 43 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 44 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 45 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 46 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 47 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 48 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 49 */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4A */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4B */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4C */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4D */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4E */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 4F */  { BxAnother, &BX_CPU_C::CMOV_GqEq },
  /* 0F 50 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f50 },
  /* 0F 51 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f51 },
  /* 0F 52 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f52 },
  /* 0F 53 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f53 },
  /* 0F 54 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f54 },
  /* 0F 55 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f55 },
  /* 0F 56 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f56 },
  /* 0F 57 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f57 },
  /* 0F 58 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f58 },
  /* 0F 59 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f59 },
  /* 0F 5A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5a },
  /* 0F 5B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5b },
  /* 0F 5C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5c },
  /* 0F 5D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5d },
  /* 0F 5E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5e },
  /* 0F 5F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f5f },
  /* 0F 60 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f60 },
  /* 0F 61 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f61 }, 
  /* 0F 62 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f62 }, 
  /* 0F 63 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f63 }, 
  /* 0F 64 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f64 }, 
  /* 0F 65 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f65 }, 
  /* 0F 66 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f66 }, 
  /* 0F 67 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f67 }, 
  /* 0F 68 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f68 }, 
  /* 0F 69 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f69 }, 
  /* 0F 6A */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6a }, 
  /* 0F 6B */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6b }, 
  /* 0F 6C */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6c },
  /* 0F 6D */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6d },
  /* 0F 6E */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6e }, 
  /* 0F 6F */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f6f }, 
  /* 0F 70 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f70 },
  /* 0F 71 */  { BxAnother | BxGroup12, NULL, BxOpcodeInfo64G12 },
  /* 0F 72 */  { BxAnother | BxGroup13, NULL, BxOpcodeInfo64G13 },
  /* 0F 73 */  { BxAnother | BxGroup14, NULL, BxOpcodeInfo64G14 },
  /* 0F 74 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f74 }, 
  /* 0F 75 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f75 }, 
  /* 0F 76 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f76 }, 
  /* 0F 77 */  { 0, &BX_CPU_C::EMMS },     
  /* 0F 78 */  { 0, &BX_CPU_C::BxError },
  /* 0F 79 */  { 0, &BX_CPU_C::BxError },
  /* 0F 7A */  { 0, &BX_CPU_C::BxError },
  /* 0F 7B */  { 0, &BX_CPU_C::BxError },
  /* 0F 7C */  { 0, &BX_CPU_C::BxError },
  /* 0F 7D */  { 0, &BX_CPU_C::BxError },
  /* 0F 7E */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7e }, 
  /* 0F 7F */  { BxAnother | BxPrefixSSE, NULL,  BxOpcodeGroupSSE_0f7f }, 
  /* 0F 80 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 81 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 82 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 83 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 84 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 85 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 86 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 87 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 88 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 89 */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8A */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8B */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8C */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8D */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8E */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 8F */  { BxImmediate_BrOff32, &BX_CPU_C::JCC_Jq },
  /* 0F 90 */  { BxAnother, &BX_CPU_C::SETO_Eb },
  /* 0F 91 */  { BxAnother, &BX_CPU_C::SETNO_Eb },
  /* 0F 92 */  { BxAnother, &BX_CPU_C::SETB_Eb },
  /* 0F 93 */  { BxAnother, &BX_CPU_C::SETNB_Eb },
  /* 0F 94 */  { BxAnother, &BX_CPU_C::SETZ_Eb },
  /* 0F 95 */  { BxAnother, &BX_CPU_C::SETNZ_Eb },
  /* 0F 96 */  { BxAnother, &BX_CPU_C::SETBE_Eb },
  /* 0F 97 */  { BxAnother, &BX_CPU_C::SETNBE_Eb },
  /* 0F 98 */  { BxAnother, &BX_CPU_C::SETS_Eb },
  /* 0F 99 */  { BxAnother, &BX_CPU_C::SETNS_Eb },
  /* 0F 9A */  { BxAnother, &BX_CPU_C::SETP_Eb },
  /* 0F 9B */  { BxAnother, &BX_CPU_C::SETNP_Eb },
  /* 0F 9C */  { BxAnother, &BX_CPU_C::SETL_Eb },
  /* 0F 9D */  { BxAnother, &BX_CPU_C::SETNL_Eb },
  /* 0F 9E */  { BxAnother, &BX_CPU_C::SETLE_Eb },
  /* 0F 9F */  { BxAnother, &BX_CPU_C::SETNLE_Eb },
  /* 0F A0 */  { 0, &BX_CPU_C::PUSH64_FS },
  /* 0F A1 */  { 0, &BX_CPU_C::POP64_FS },
  /* 0F A2 */  { 0, &BX_CPU_C::CPUID },
  /* 0F A3 */  { BxAnother, &BX_CPU_C::BT_EvGv },
  /* 0F A4 */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHLD_EqGq },
  /* 0F A5 */  { BxAnother,                  &BX_CPU_C::SHLD_EqGq },
  /* 0F A6 */  { 0, &BX_CPU_C::CMPXCHG_XBTS },
  /* 0F A7 */  { 0, &BX_CPU_C::CMPXCHG_IBTS },
  /* 0F A8 */  { 0, &BX_CPU_C::PUSH64_GS },
  /* 0F A9 */  { 0, &BX_CPU_C::POP64_GS },
  /* 0F AA */  { 0, &BX_CPU_C::RSM },
  /* 0F AB */  { BxAnother, &BX_CPU_C::BTS_EvGv },
  /* 0F AC */  { BxAnother | BxImmediate_Ib, &BX_CPU_C::SHRD_EqGq },
  /* 0F AD */  { BxAnother,                  &BX_CPU_C::SHRD_EqGq },
  /* 0F AE */  { BxAnother | BxGroup15, NULL, BxOpcodeInfo64G15 },
  /* 0F AF */  { BxAnother, &BX_CPU_C::IMUL_GqEq },
  /* 0F B0 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EbGb },
  /* 0F B1 */  { BxAnother | BxLockable, &BX_CPU_C::CMPXCHG_EqGq },
  /* 0F B2 */  { BxAnother, &BX_CPU_C::LSS_GvMp },
  /* 0F B3 */  { BxAnother, &BX_CPU_C::BTR_EvGv },
  /* 0F B4 */  { BxAnother, &BX_CPU_C::LFS_GvMp },
  /* 0F B5 */  { BxAnother, &BX_CPU_C::LGS_GvMp },
  /* 0F B6 */  { BxAnother, &BX_CPU_C::MOVZX_GqEb },
  /* 0F B7 */  { BxAnother, &BX_CPU_C::MOVZX_GqEw },
  /* 0F B8 */  { 0, &BX_CPU_C::BxError },
  /* 0F B9 */  { 0, &BX_CPU_C::BxError },
  /* 0F BA */  { BxAnother | BxGroup8, NULL, BxOpcodeInfo64G8EvIb },
  /* 0F BB */  { BxAnother, &BX_CPU_C::BTC_EvGv },
  /* 0F BC */  { BxAnother, &BX_CPU_C::BSF_GvEv },
  /* 0F BD */  { BxAnother, &BX_CPU_C::BSR_GvEv },
  /* 0F BE */  { BxAnother, &BX_CPU_C::MOVSX_GqEb },
  /* 0F BF */  { BxAnother, &BX_CPU_C::MOVSX_GqEw },
  /* 0F C0 */  { BxAnother, &BX_CPU_C::XADD_EbGb },
  /* 0F C1 */  { BxAnother | BxLockable, &BX_CPU_C::XADD_EqGq },
  /* 0F C2 */  { BxAnother | BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc2 },
  /* 0F C3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc3 },
  /* 0F C4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc4 },
  /* 0F C5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc5 },
  /* 0F C6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fc6 },
  /* 0F C7 */  { BxAnother | BxGroup9, NULL, BxOpcodeInfo64G9 },
  /* 0F C8 */  { 0, &BX_CPU_C::BSWAP_RAX },
  /* 0F C9 */  { 0, &BX_CPU_C::BSWAP_RCX },
  /* 0F CA */  { 0, &BX_CPU_C::BSWAP_RDX },
  /* 0F CB */  { 0, &BX_CPU_C::BSWAP_RBX },
  /* 0F CC */  { 0, &BX_CPU_C::BSWAP_RSP },
  /* 0F CD */  { 0, &BX_CPU_C::BSWAP_RBP },
  /* 0F CE */  { 0, &BX_CPU_C::BSWAP_RSI },
  /* 0F CF */  { 0, &BX_CPU_C::BSWAP_RDI },
  /* 0F D0 */  { 0, &BX_CPU_C::BxError },
  /* 0F D1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd1 },
  /* 0F D2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd2 },
  /* 0F D3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd3 },
  /* 0F D4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd4 },
  /* 0F D5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd5 }, 
  /* 0F D6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd6 },
  /* 0F D7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd7 },
  /* 0F D8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd8 },
  /* 0F D9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fd9 },
  /* 0F DA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fda },
  /* 0F DB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdb },
  /* 0F DC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdc },
  /* 0F DD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdd },
  /* 0F DE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fde },
  /* 0F DF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fdf },
  /* 0F E0 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe0 },
  /* 0F E1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe1 }, 
  /* 0F E2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe2 }, 
  /* 0F E3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe3 },
  /* 0F E4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe4 },
  /* 0F E5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe5 },
  /* 0F E6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe6 },
  /* 0F E7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe7 },
  /* 0F E8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe8 },
  /* 0F E9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fe9 },
  /* 0F EA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fea },
  /* 0F EB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0feb },
  /* 0F EC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fec },
  /* 0F ED */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fed },
  /* 0F EE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fee },
  /* 0F EF */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0fef },
  /* 0F F0 */  { 0, &BX_CPU_C::BxError },
  /* 0F F1 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff1 }, 
  /* 0F F2 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff2 }, 
  /* 0F F3 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff3 }, 
  /* 0F F4 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff4 }, 
  /* 0F F5 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff5 }, 
  /* 0F F6 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff6 }, 
  /* 0F F7 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff7 }, 
  /* 0F F8 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff8 }, 
  /* 0F F9 */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ff9 }, 
  /* 0F FA */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffa }, 
  /* 0F FB */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffb }, 
  /* 0F FC */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffc }, 
  /* 0F FD */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffd }, 
  /* 0F FE */  { BxAnother | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0ffe }, 
  /* 0F FF */  { 0, &BX_CPU_C::BxError }
  };




  unsigned
BX_CPU_C::fetchDecode64(Bit8u *iptr, bxInstruction_c *instruction,
                      unsigned remain)
{
  // remain must be at least 1

  unsigned b1, b2, ilen=1, attr, lock=0;
  unsigned imm_mode, offset, rex_r,rex_x,rex_b;
  unsigned rm, mod, nnn;
  unsigned sse_prefix;
#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F2   2
#define SSE_PREFIX_F3   4      /* only one SSE prefix could be used */
  static int sse_prefix_index[8] = { 0, 1, 2, -1, 3, -1, -1, -1 };

  offset = 512*1;
  rex_r = 0;
  rex_x = 0;
  rex_b = 0;
  instruction->ResolveModrm = NULL;
  instruction->initMetaInfo(
                  BX_SEG_REG_NULL,
                  /*os32*/       1, // operand size 32 override defaults to 1
                  /*as32*/       1, // address size 32 override defaults to 1
                  /*os64*/       0, // operand size 64 override defaults to 0
                  /*as64*/       1, // address size 64 override defaults to 1
                  /*extend8bit*/ 0,
                  /*repUsed*/    0);

  sse_prefix = SSE_PREFIX_NONE;

fetch_b1:
  b1 = *iptr++;

another_byte:
  //offset = (instruction->os32L()+instruction->os_64 <<1) << 9; // * 512
  //BX_DEBUG (("op: 0x%02x offset: 0x%04x",b1,offset));

  attr = BxOpcodeInfo64[b1+offset].Attr;
  instruction->setRepAttr(attr & (BxRepeatable | BxRepeatableZF));

  if (attr & BxAnother) {
    if (attr & BxPrefix) {
      switch (b1) {
        case 0x66: // OpSize
          BX_INSTR_PREFIX_OS(BX_CPU_ID);
          sse_prefix |= SSE_PREFIX_66;
          if (!instruction->os64L()) {
            instruction->setOs32B(0);
            offset = 0;
            }
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0x67: // AddrSize
          BX_INSTR_PREFIX_AS(BX_CPU_ID);
          instruction->setAs64B(0);
          if (ilen < remain) {
            ilen++;
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
          BX_INSTR_PREFIX_EXTEND8B(BX_CPU_ID);

          instruction->assertExtend8bit();
          //BX_DEBUG (("REX byte = %02x",b1));
          if (b1 & 0x8) {
            instruction->assertOs64();
            instruction->assertOs32();
            offset = 512*2;
            //BX_DEBUG ((" 64bit")):
            }
          if (b1 & 0x4) {
            rex_r = 8;
            //BX_DEBUG((" reg+8"));
            }
          if (b1 & 0x2) {
            rex_x = 8;
            //BX_DEBUG((" index+8"));
            }
          if (b1 & 0x1) {
            rex_b = 8;
            //BX_DEBUG((" base+8"));
            }
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0xf2: // REPNE/REPNZ
          BX_INSTR_PREFIX_REPNE(BX_CPU_ID);
          sse_prefix |= SSE_PREFIX_F2;
          instruction->setRepUsed(b1 & 3);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0xf3: // REP/REPE/REPZ
          BX_INSTR_PREFIX_REP(BX_CPU_ID);
          sse_prefix |= SSE_PREFIX_F3;
          instruction->setRepUsed(b1 & 3);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        case 0x2e: // CS:
          BX_INSTR_PREFIX_CS(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_CS);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0x26: // ES:
          BX_INSTR_PREFIX_ES(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_ES);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0x36: // SS:
          BX_INSTR_PREFIX_SS(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_SS);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0x3e: // DS:
          BX_INSTR_PREFIX_DS(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_DS);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0x64: // FS:
          BX_INSTR_PREFIX_FS(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_FS);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0x65: // GS:
          BX_INSTR_PREFIX_GS(BX_CPU_ID);
          instruction->setSeg(BX_SEG_REG_GS);
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);
        case 0xf0: // LOCK:
          BX_INSTR_PREFIX_LOCK(BX_CPU_ID);
          lock = 1;
          if (ilen < remain) {
            ilen++;
            goto fetch_b1;
            }
          return(0);

        default:
BX_PANIC(("fetch_decode: prefix default = 0x%02x", b1));
          return(0);
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
    mod = b2 & 0xc0;
    nnn   = ((b2 >> 3) & 0x07) | rex_r;
    rm  = b2 & 0x07;
    instruction->modRMForm.modRMData = (b2<<20);
    instruction->modRMForm.modRMData |= mod;
    instruction->modRMForm.modRMData |= (nnn<<8);

    if (mod == 0xc0) { // mod == 11b
      rm |= rex_b;
      instruction->modRMForm.modRMData |= rm;
      instruction->metaInfo |= (1<<22); // (modC0)
      goto modrm_done;
      }
    if (rm != 4) {
      rm |= rex_b;
      }
    instruction->modRMForm.modRMData |= rm;
    if (instruction->as64L()) {
      // 64-bit addressing modes; note that mod==11b handled above
      if (rm != 4) { // no s-i-b byte
#if BX_DYNAMIC_TRANSLATION
        instruction->DTMemRegsUsed = 1<<rm; // except for mod=00b rm=100b
#endif
        if (mod == 0x00) { // mod == 00b
          instruction->ResolveModrm = BxResolve64Mod0[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_SEG_REG_DS);
ExtendedFieldCheck((rm&8) && ((rm&7)==5)); // KPL
          if (rm == 5) {
            if ((ilen+3) < remain) {
              Bit32u imm32u;
              imm32u = *iptr++;
              imm32u |= (*iptr++) << 8;
              imm32u |= (*iptr++) << 16;
              imm32u |= (*iptr++) << 24;
              ilen += 4;
              instruction->modRMForm.displ32u = imm32u;
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
        if (mod == 0x40) { // mod == 01b
          instruction->ResolveModrm = BxResolve64Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod01_rm32[rm]);
get_8bit_displ_1:
          if (ilen < remain) {
            // 8 sign extended to 32
            instruction->modRMForm.displ32u = (Bit8s) *iptr++;
            ilen++;
            goto modrm_done;
            }
          else {
            return(0);
            }
          }
        // (mod == 0x80) mod == 10b
        instruction->ResolveModrm = BxResolve64Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(BX_CPU_THIS_PTR sreg_mod10_rm32[rm]);
get_32bit_displ_1:
        if ((ilen+3) < remain) {
          Bit32u imm32u;
          imm32u = *iptr++;
          imm32u |= (*iptr++) << 8;
          imm32u |= (*iptr++) << 16;
          imm32u |= (*iptr++) << 24;
          instruction->modRMForm.displ32u = imm32u;
          ilen += 4;
          goto modrm_done;
          }
        else {
          return(0);
          }
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
        base = (sib & 0x07) | rex_b; sib >>= 3;
        index = (sib & 0x07) | rex_x; sib >>= 3;
        scale = sib;
        instruction->modRMForm.modRMData |= (base<<12);
        instruction->modRMForm.modRMData |= (index<<16);
        instruction->modRMForm.modRMData |= (scale<<4);
#if BX_DYNAMIC_TRANSLATION
        if (instruction->modRMForm.index == 0x04) // 100b
          instruction->DTMemRegsUsed = 0;
        else
          instruction->DTMemRegsUsed = 1<<instruction->modRMForm.index;
#endif
        if (mod == 0x00) { // mod==00b, rm==4
          instruction->ResolveModrm = BxResolve64Mod0Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod0_base32[base]);
ExtendedFieldCheck((base&8) && ((base&7)==5)); // KPL
          if (base == 0x05) {
            goto get_32bit_displ_1;
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
        if (mod == 0x40) { // mod==01b, rm==4
          instruction->ResolveModrm = BxResolve64Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod1or2_base32[base]);
          goto get_8bit_displ_1;
          }
        // (mod == 0x80),  mod==10b, rm==4
        instruction->ResolveModrm = BxResolve64Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(BX_CPU_THIS_PTR sreg_mod1or2_base32[base]);
        goto get_32bit_displ_1;
        }
      }
    else {
     // 32-bit addressing modes; note that mod==11b handled above
      if (rm != 4) { // no s-i-b byte
#if BX_DYNAMIC_TRANSLATION
        instruction->DTMemRegsUsed = 1<<rm; // except for mod=00b rm=100b
#endif
        if (mod == 0x00) { // mod == 00b
ExtendedFieldCheck(rm&8); // KPL
          instruction->ResolveModrm = BxResolve32Mod0[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_SEG_REG_DS);
ExtendedFieldCheck((rm&8) && ((rm&7)==5)); // KPL
          if (rm == 5) {
            if ((ilen+3) < remain) {
              Bit32u imm32u;
              imm32u = *iptr++;
              imm32u |= (*iptr++) << 8;
              imm32u |= (*iptr++) << 16;
              imm32u |= (*iptr++) << 24;
              //RMAddr(instruction) = imm32u;
              instruction->modRMForm.displ32u = imm32u;
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
        if (mod == 0x40) { // mod == 01b
ExtendedFieldCheck(rm&8); // KPL
          instruction->ResolveModrm = BxResolve32Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod01_rm32[rm]);
get_8bit_displ:
          if (ilen < remain) {
            // 8 sign extended to 32
            instruction->modRMForm.displ32u = (Bit8s) *iptr++;
            ilen++;
            goto modrm_done;
            }
          else {
            return(0);
            }
          }
        // (mod == 0x80) mod == 10b
ExtendedFieldCheck(rm&8); // KPL
        instruction->ResolveModrm = BxResolve32Mod1or2[rm];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2[rm];
#endif
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(BX_CPU_THIS_PTR sreg_mod10_rm32[rm]);
get_32bit_displ:
        if ((ilen+3) < remain) {
          Bit32u imm32u;
          imm32u = *iptr++;
          imm32u |= (*iptr++) << 8;
          imm32u |= (*iptr++) << 16;
          imm32u |= (*iptr++) << 24;
          instruction->modRMForm.displ32u = imm32u;
          ilen += 4;
          goto modrm_done;
          }
        else {
          return(0);
          }
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
        scale = sib;
        instruction->modRMForm.modRMData |= (base<<12);
        instruction->modRMForm.modRMData |= (index<<16);
        instruction->modRMForm.modRMData |= (scale<<4);
#if BX_DYNAMIC_TRANSLATION
        if (instruction->modRMForm.index == 0x04) // 100b
          instruction->DTMemRegsUsed = 0;
        else
          instruction->DTMemRegsUsed = 1<<instruction->modRMForm.index;
#endif
        if (mod == 0x00) { // mod==00b, rm==4
ExtendedFieldCheck(base&8); // KPL
          instruction->ResolveModrm = BxResolve32Mod0Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod0Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod0_base32[base]);
ExtendedFieldCheck((base&8) && ((base&7)==5)); // KPL
          if (base == 0x05) {
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
        if (mod == 0x40) { // mod==01b, rm==4
ExtendedFieldCheck(base&8); // KPL
          instruction->ResolveModrm = BxResolve32Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
          instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
          if (BX_NULL_SEG_REG(instruction->seg()))
            instruction->setSeg(BX_CPU_THIS_PTR sreg_mod1or2_base32[base]);
          goto get_8bit_displ;
          }
        // (mod == 0x80),  mod==10b, rm==4
ExtendedFieldCheck(base&8); // KPL
        instruction->ResolveModrm = BxResolve32Mod1or2Base[base];
#if BX_DYNAMIC_TRANSLATION
        instruction->DTResolveModrm = (BxVoidFPtr_t) BxDTResolve32Mod1or2Base[base];
#endif
        if (BX_NULL_SEG_REG(instruction->seg()))
          instruction->setSeg(BX_CPU_THIS_PTR sreg_mod1or2_base32[base]);
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
                     nnn
                     ));
    */
    if (attr & BxGroupN) {
      BxOpcodeInfo_t *OpcodeInfoPtr = BxOpcodeInfo64[b1+offset].AnotherArray;
ExtendedFieldCheck(nnn&8); // KPL
      // get additional attributes from group table
      attr |= OpcodeInfoPtr[nnn].Attr;
      instruction->setRepAttr(attr & (BxRepeatable | BxRepeatableZF));
#if BX_DYNAMIC_TRANSLATION
      instruction->DTAttr = 0; // for now
#endif
      /* For SSE opcodes, look into another 4 entries table 
                        with the opcode prefixes (NONE, 0x66, 0xF2, 0xF3 */
      if (attr & BxPrefixSSE) {
        int op = sse_prefix_index[sse_prefix];
        if(op < 0) BX_PANIC(("fetchdecode: SSE opcode with two or more prefixes"));
        else {
           OpcodeInfoPtr = OpcodeInfoPtr[nnn].AnotherArray;
           instruction->execute = OpcodeInfoPtr[op].ExecutePtr;
           attr |= OpcodeInfoPtr[op].Attr; 
        }
      }
      // For high frequency opcodes, two variants of the instruction are
      // implemented; one for the mod=11b case (Reg-Reg), and one for
      // the other cases (Reg-Mem).  If this is one of those cases,
      // we need to dereference to get to the execute pointer.
      else {
        if (attr & BxSplitMod11b) {
          OpcodeInfoPtr = OpcodeInfoPtr[nnn].AnotherArray;
          instruction->execute = OpcodeInfoPtr[mod==0xc0].ExecutePtr;
          attr |= OpcodeInfoPtr[mod==0xc0].Attr;
        }
        else {
          instruction->execute = OpcodeInfoPtr[nnn].ExecutePtr;
          attr |= OpcodeInfoPtr[nnn].Attr;
        }
      }
    }
    else {
#if BX_DYNAMIC_TRANSLATION
      instruction->DTAttr = BxDTOpcodeInfo[b1+offset].DTAttr;
      instruction->DTFPtr = BxDTOpcodeInfo[b1+offset].DTASFPtr;
#endif
      if (attr & BxPrefixSSE) {
        int op = sse_prefix_index[sse_prefix];
        if(op < 0) BX_PANIC(("fetchdecode: SSE opcode with two or more prefixes"));
        else {
           BxOpcodeInfo_t *OpcodeInfoPtr = BxOpcodeInfo64[b1+offset].AnotherArray;
           instruction->execute = OpcodeInfoPtr[op].ExecutePtr;
           attr |= OpcodeInfoPtr[op].Attr; 
        }
      }
      // (See note immediately above for comment)
      else {
        if (attr & BxSplitMod11b) {
          BxOpcodeInfo_t *OpcodeInfoPtr = BxOpcodeInfo64[b1+offset].AnotherArray;
          instruction->execute = OpcodeInfoPtr[mod==0xc0].ExecutePtr;
          attr |= OpcodeInfoPtr[mod==0xc0].Attr;
        }
        else {
          instruction->execute = BxOpcodeInfo64[b1+offset].ExecutePtr;
          attr |= BxOpcodeInfo64[b1+offset].Attr;
        }
      }
    }
  }
  else {
    // Opcode does not require a MODRM byte.
    // Note that a 2-byte opcode (0F XX) will jump to before
    // the if() above after fetching the 2nd byte, so this path is
    // taken in all cases if a modrm byte is NOT required.
    instruction->execute = BxOpcodeInfo64[b1+offset].ExecutePtr;
    instruction->IxForm.opcodeReg = (b1 & 7) | rex_b;
#if BX_DYNAMIC_TRANSLATION
    instruction->DTAttr = BxDTOpcodeInfo[b1+offset].DTAttr;
    instruction->DTFPtr = BxDTOpcodeInfo[b1+offset].DTASFPtr;
#endif
    }

  if (lock) { // lock prefix invalid opcode
      // lock prefix not allowed or destination operand is not memory
      if ((mod == 0xc0) || !(attr & BxLockable)) {
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
          Bit8s temp8s;
          temp8s = *iptr;
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
      case BxImmediate_Iv: // same as BxImmediate_BrOff32
      case BxImmediate_IvIw: // CALL_Ap
        if (instruction->os32L()) {
          if ((ilen+3) < remain) {
            Bit32u imm32u;
            ReadHostDWordFromLittleEndian(iptr, imm32u);
            instruction->modRMForm.Id = imm32u;
            ilen += 4;
            }
          else {
            return(0);
            }
          }
        else {
          if ((ilen+1) < remain) {
            Bit16u imm16u;
            ReadHostWordFromLittleEndian(iptr, imm16u);
            instruction->modRMForm.Iw = imm16u;
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
          ReadHostWordFromLittleEndian(iptr, imm16u);
          instruction->IxIxForm.Iw2 = imm16u;
          ilen += 2;
          }
        else {
          return(0);
          }
        break;
      case BxImmediate_Iq: // MOV Rx,imm64
        if ((ilen+7) < remain) {
          Bit64u imm64u;
          ReadHostQWordFromLittleEndian(iptr, imm64u);
          instruction->IqForm.Iq = imm64u;
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
            Bit64u imm64u;
            ReadHostQWordFromLittleEndian(iptr, imm64u);
            instruction->IqForm.Iq = imm64u;
            ilen += 8;
            }
          else {
            return(0);
            }
          }
        else {
          if ((ilen+3) < remain) {
            Bit32u imm32u;
            ReadHostDWordFromLittleEndian(iptr, imm32u);
            // Sign extend???
            instruction->IqForm.Iq = imm32u;
            ilen += 4;
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
          instruction->modRMForm.Iw = imm16u;
          ilen += 2;
          }
        else {
          return(0);
          }
        if (imm_mode == BxImmediate_Iw) break;
        iptr++;
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
          Bit8s temp8s;
          temp8s = *iptr;
          instruction->modRMForm.Id = temp8s;
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
          instruction->modRMForm.Id = (Bit16s) imm16u;
          ilen += 2;
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
  if(b1 == 0x10f) {		// 3DNow! instruction set
     instruction->execute = Bx3DNowOpcodeInfo[instruction->modRMForm.Ib].ExecutePtr;
    }
#endif

  instruction->setB1(b1);
  instruction->setILen(ilen);
  return(1);
}

#endif /* if BX_SUPPORT_X86_64 */
