/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode.h,v 1.100 2010-04-04 18:46:02 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2010 Stanislav Shwartsman
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

#ifndef BX_COMMON_FETCHDECODE_TABLES_H
#define BX_COMMON_FETCHDECODE_TABLES_H

typedef struct BxOpcodeInfo_t {
  Bit16u Attr;
  Bit16u IA;
  const BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;

//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

BX_CPP_INLINE Bit16u FetchWORD(const Bit8u *iptr)
{
   Bit16u data;
   ReadHostWordFromLittleEndian(iptr, data);
   return data;
}

BX_CPP_INLINE Bit32u FetchDWORD(const Bit8u *iptr)
{
   Bit32u data;
   ReadHostDWordFromLittleEndian(iptr, data);
   return data;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u FetchQWORD(const Bit8u *iptr)
{
   Bit64u data;
   ReadHostQWordFromLittleEndian(iptr, data);
   return data;
}
#endif

struct bxIAOpcodeTable {
  BxExecutePtr_tR execute1;
  BxExecutePtr_tR execute2;
};

//
// Common FetchDecode Opcode Tables
//

#include "fetchdecode_x87.h"
#include "fetchdecode_sse.h"

/* ************************************************************************ */
/* Opcode Groups */

/* ******* */
/* Group 1 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EbIbM },
  /* 1 */ { BxLockable, BX_IA_OR_EbIbM  },
  /* 2 */ { BxLockable, BX_IA_ADC_EbIbM },
  /* 3 */ { BxLockable, BX_IA_SBB_EbIbM },
  /* 4 */ { BxLockable, BX_IA_AND_EbIbM },
  /* 5 */ { BxLockable, BX_IA_SUB_EbIbM },
  /* 6 */ { BxLockable, BX_IA_XOR_EbIbM },
  /* 7 */ { 0,          BX_IA_CMP_EbIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EbIbR },
  /* 1 */ { 0, BX_IA_OR_EbIbR  },
  /* 2 */ { 0, BX_IA_ADC_EbIbR },
  /* 3 */ { 0, BX_IA_SBB_EbIbR },
  /* 4 */ { 0, BX_IA_AND_EbIbR },
  /* 5 */ { 0, BX_IA_SUB_EbIbR },
  /* 6 */ { 0, BX_IA_XOR_EbIbR },
  /* 7 */ { 0, BX_IA_CMP_EbIbR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EwM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EwIwM },
  /* 1 */ { BxLockable, BX_IA_OR_EwIwM },
  /* 2 */ { BxLockable, BX_IA_ADC_EwIwM },
  /* 3 */ { BxLockable, BX_IA_SBB_EwIwM },
  /* 4 */ { BxLockable, BX_IA_AND_EwIwM },
  /* 5 */ { BxLockable, BX_IA_SUB_EwIwM },
  /* 6 */ { BxLockable, BX_IA_XOR_EwIwM },
  /* 7 */ { 0,          BX_IA_CMP_EwIwM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EwR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EwIwR },
  /* 1 */ { 0, BX_IA_OR_EwIwR },
  /* 2 */ { 0, BX_IA_ADC_EwIwR },
  /* 3 */ { 0, BX_IA_SBB_EwIwR },
  /* 4 */ { 0, BX_IA_AND_EwIwR },
  /* 5 */ { 0, BX_IA_SUB_EwIwR },
  /* 6 */ { 0, BX_IA_XOR_EwIwR },
  /* 7 */ { 0, BX_IA_CMP_EwIwR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EdM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EdIdM },
  /* 1 */ { BxLockable, BX_IA_OR_EdIdM },
  /* 2 */ { BxLockable, BX_IA_ADC_EdIdM },
  /* 3 */ { BxLockable, BX_IA_SBB_EdIdM },
  /* 4 */ { BxLockable, BX_IA_AND_EdIdM },
  /* 5 */ { BxLockable, BX_IA_SUB_EdIdM },
  /* 6 */ { BxLockable, BX_IA_XOR_EdIdM },
  /* 7 */ { 0,          BX_IA_CMP_EdIdM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EdR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EdIdR },
  /* 1 */ { 0, BX_IA_OR_EdIdR },
  /* 2 */ { 0, BX_IA_ADC_EdIdR },
  /* 3 */ { 0, BX_IA_SBB_EdIdR },
  /* 4 */ { 0, BX_IA_AND_EdIdR },
  /* 5 */ { 0, BX_IA_SUB_EdIdR },
  /* 6 */ { 0, BX_IA_XOR_EdIdR },
  /* 7 */ { 0, BX_IA_CMP_EdIdR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1EqM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EqIdM },
  /* 1 */ { BxLockable, BX_IA_OR_EqIdM },
  /* 2 */ { BxLockable, BX_IA_ADC_EqIdM },
  /* 3 */ { BxLockable, BX_IA_SBB_EqIdM },
  /* 4 */ { BxLockable, BX_IA_AND_EqIdM },
  /* 5 */ { BxLockable, BX_IA_SUB_EqIdM },
  /* 6 */ { BxLockable, BX_IA_XOR_EqIdM },
  /* 7 */ { 0,          BX_IA_CMP_EqIdM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G1EqR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EqIdR },
  /* 1 */ { 0, BX_IA_OR_EqIdR },
  /* 2 */ { 0, BX_IA_ADC_EqIdR },
  /* 3 */ { 0, BX_IA_SBB_EqIdR },
  /* 4 */ { 0, BX_IA_AND_EqIdR },
  /* 5 */ { 0, BX_IA_SUB_EqIdR },
  /* 6 */ { 0, BX_IA_XOR_EqIdR },
  /* 7 */ { 0, BX_IA_CMP_EqIdR }
};
#endif

/* ******** */
/* Group 1A */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG1AEwR[8] = {
  /* 0 */ { 0, BX_IA_POP_EwR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1AEwM[8] = {
  /* 0 */ { 0, BX_IA_POP_EwM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1AEdR[8] = {
  /* 0 */ { 0, BX_IA_POP_EdR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1AEdM[8] = {
  /* 0 */ { 0, BX_IA_POP_EdM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1AEqR[8] = {
  /* 0 */ { 0, BX_IA_POP_EqR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G1AEqM[8] = {
  /* 0 */ { 0, BX_IA_POP_EqM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 2 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG2Eb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Eb },
  /* 1 */ { 0, BX_IA_ROR_Eb },
  /* 2 */ { 0, BX_IA_RCL_Eb },
  /* 3 */ { 0, BX_IA_RCR_Eb },
  /* 4 */ { 0, BX_IA_SHL_Eb },
  /* 5 */ { 0, BX_IA_SHR_Eb },
  /* 6 */ { 0, BX_IA_SHL_Eb },
  /* 7 */ { 0, BX_IA_SAR_Eb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2Ew[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Ew },
  /* 1 */ { 0, BX_IA_ROR_Ew },
  /* 2 */ { 0, BX_IA_RCL_Ew },
  /* 3 */ { 0, BX_IA_RCR_Ew },
  /* 4 */ { 0, BX_IA_SHL_Ew },
  /* 5 */ { 0, BX_IA_SHR_Ew },
  /* 6 */ { 0, BX_IA_SHL_Ew },
  /* 7 */ { 0, BX_IA_SAR_Ew }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2EdM[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EdM },
  /* 1 */ { 0, BX_IA_ROR_EdM },
  /* 2 */ { 0, BX_IA_RCL_EdM },
  /* 3 */ { 0, BX_IA_RCR_EdM },
  /* 4 */ { 0, BX_IA_SHL_EdM },
  /* 5 */ { 0, BX_IA_SHR_EdM },
  /* 6 */ { 0, BX_IA_SHL_EdM },
  /* 7 */ { 0, BX_IA_SAR_EdM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2EdR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EdR },
  /* 1 */ { 0, BX_IA_ROR_EdR },
  /* 2 */ { 0, BX_IA_RCL_EdR },
  /* 3 */ { 0, BX_IA_RCR_EdR },
  /* 4 */ { 0, BX_IA_SHL_EdR },
  /* 5 */ { 0, BX_IA_SHR_EdR },
  /* 6 */ { 0, BX_IA_SHL_EdR },
  /* 7 */ { 0, BX_IA_SAR_EdR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G2EqM[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EqM },
  /* 1 */ { 0, BX_IA_ROR_EqM },
  /* 2 */ { 0, BX_IA_RCL_EqM },
  /* 3 */ { 0, BX_IA_RCR_EqM },
  /* 4 */ { 0, BX_IA_SHL_EqM },
  /* 5 */ { 0, BX_IA_SHR_EqM },
  /* 6 */ { 0, BX_IA_SHL_EqM },
  /* 7 */ { 0, BX_IA_SAR_EqM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G2EqR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_EqR },
  /* 1 */ { 0, BX_IA_ROR_EqR },
  /* 2 */ { 0, BX_IA_RCL_EqR },
  /* 3 */ { 0, BX_IA_RCR_EqR },
  /* 4 */ { 0, BX_IA_SHL_EqR },
  /* 5 */ { 0, BX_IA_SHR_EqR },
  /* 6 */ { 0, BX_IA_SHL_EqR },
  /* 7 */ { 0, BX_IA_SAR_EqR }
};
#endif

/* ******* */
/* Group 3 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG3EbM[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_TEST_EbIbM },
  /* 1 */ { BxImmediate_Ib, BX_IA_TEST_EbIbM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EbM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EbM },
  /* 4 */ { 0,              BX_IA_MUL_ALEbM },
  /* 5 */ { 0,              BX_IA_IMUL_ALEbM },
  /* 6 */ { 0,              BX_IA_DIV_ALEbM },
  /* 7 */ { 0,              BX_IA_IDIV_ALEbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EbR[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_TEST_EbIbR },
  /* 1 */ { BxImmediate_Ib, BX_IA_TEST_EbIbR },
  /* 2 */ { 0, BX_IA_NOT_EbR },
  /* 3 */ { 0, BX_IA_NEG_EbR },
  /* 4 */ { 0, BX_IA_MUL_ALEbR },
  /* 5 */ { 0, BX_IA_IMUL_ALEbR },
  /* 6 */ { 0, BX_IA_DIV_ALEbR },
  /* 7 */ { 0, BX_IA_IDIV_ALEbR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EwM[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_TEST_EwIwM },
  /* 1 */ { BxImmediate_Iw, BX_IA_TEST_EwIwM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EwM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EwM },
  /* 4 */ { 0,              BX_IA_MUL_AXEwM },
  /* 5 */ { 0,              BX_IA_IMUL_AXEwM },
  /* 6 */ { 0,              BX_IA_DIV_AXEwM },
  /* 7 */ { 0,              BX_IA_IDIV_AXEwM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EwR[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_TEST_EwIwR },
  /* 1 */ { BxImmediate_Iw, BX_IA_TEST_EwIwR },
  /* 2 */ { 0, BX_IA_NOT_EwR },
  /* 3 */ { 0, BX_IA_NEG_EwR },
  /* 4 */ { 0, BX_IA_MUL_AXEwR },
  /* 5 */ { 0, BX_IA_IMUL_AXEwR },
  /* 6 */ { 0, BX_IA_DIV_AXEwR },
  /* 7 */ { 0, BX_IA_IDIV_AXEwR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EdM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EdIdM },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EdIdM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EdM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EdM },
  /* 4 */ { 0,              BX_IA_MUL_EAXEdM },
  /* 5 */ { 0,              BX_IA_IMUL_EAXEdM },
  /* 6 */ { 0,              BX_IA_DIV_EAXEdM },
  /* 7 */ { 0,              BX_IA_IDIV_EAXEdM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EdR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EdIdR },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EdIdR },
  /* 2 */ { 0, BX_IA_NOT_EdR },
  /* 3 */ { 0, BX_IA_NEG_EdR },
  /* 4 */ { 0, BX_IA_MUL_EAXEdR },
  /* 5 */ { 0, BX_IA_IMUL_EAXEdR },
  /* 6 */ { 0, BX_IA_DIV_EAXEdR },
  /* 7 */ { 0, BX_IA_IDIV_EAXEdR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G3EqM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EqIdM },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EqIdM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EqM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EqM },
  /* 4 */ { 0,              BX_IA_MUL_RAXEqM },
  /* 5 */ { 0,              BX_IA_IMUL_RAXEqM },
  /* 6 */ { 0,              BX_IA_DIV_RAXEqM },
  /* 7 */ { 0,              BX_IA_IDIV_RAXEqM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G3EqR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EqIdR },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EqIdR },
  /* 2 */ { 0, BX_IA_NOT_EqR },
  /* 3 */ { 0, BX_IA_NEG_EqR },
  /* 4 */ { 0, BX_IA_MUL_RAXEqR },
  /* 5 */ { 0, BX_IA_IMUL_RAXEqR },
  /* 6 */ { 0, BX_IA_DIV_RAXEqR },
  /* 7 */ { 0, BX_IA_IDIV_RAXEqR }
};
#endif

/* ******* */
/* Group 4 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG4M[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EbM },
  /* 1 */ { BxLockable, BX_IA_DEC_EbM },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG4R[8] = {
  /* 0 */ { 0, BX_IA_INC_EbR },
  /* 1 */ { 0, BX_IA_DEC_EbR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 5 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG5wM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_EwM },
  /* 1 */ { BxLockable, BX_IA_DEC_EwM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EwM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL16_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EwM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EwM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5wR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_INC_EwR },
  /* 1 */ { 0, BX_IA_DEC_EwR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EwR },
  /* 3 */ { 0, BX_IA_ERROR }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EwR  },
  /* 5 */ { 0, BX_IA_ERROR }, // JMP16_Ep
  /* 6 */ { 0, BX_IA_PUSH_EwR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5dM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_EdM },
  /* 1 */ { BxLockable, BX_IA_DEC_EdM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EdM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL32_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EdM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EdM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5dR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_INC_EdR },
  /* 1 */ { 0, BX_IA_DEC_EdR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EdR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL32_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EdR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP32_Ep
  /* 6 */ { 0, BX_IA_PUSH_EdR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G5wM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EwM },
  /* 1 */ { BxLockable, BX_IA_DEC_EwM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL16_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EwM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5wR[8] = {
  /* 0 */ { 0, BX_IA_INC_EwR },
  /* 1 */ { 0, BX_IA_DEC_EwR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR }, // JMP16_Ep
  /* 6 */ { 0, BX_IA_PUSH_EwR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EdM },
  /* 1 */ { BxLockable, BX_IA_DEC_EdM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL32_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EqM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dR[8] = {
  /* 0 */ { 0, BX_IA_INC_EdR },
  /* 1 */ { 0, BX_IA_DEC_EdR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL32_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP32_Ep
  /* 6 */ { 0, BX_IA_PUSH_EqR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EqM },
  /* 1 */ { BxLockable, BX_IA_DEC_EqM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL64_Ep }, // TODO: 64-bit offset for Intel
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP64_Ep },  // TODO: 64-bit offset for Intel
  /* 6 */ { 0, BX_IA_PUSH_EqM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qR[8] = {
  /* 0 */ { 0, BX_IA_INC_EqR },
  /* 1 */ { 0, BX_IA_DEC_EqR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL64_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP64_Ep
  /* 6 */ { 0, BX_IA_PUSH_EqR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 6 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG6[8] = {
  /* 0 */ { 0, BX_IA_SLDT_Ew },
  /* 1 */ { 0, BX_IA_STR_Ew },
  /* 2 */ { 0, BX_IA_LLDT_Ew },
  /* 3 */ { 0, BX_IA_LTR_Ew },
  /* 4 */ { 0, BX_IA_VERR_Ew },
  /* 5 */ { 0, BX_IA_VERW_Ew },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 7 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG7R[64] = {
  /* 0F 01 C0 */ { 0, BX_IA_ERROR },
  /* 0F 01 C1 */ { BxTraceEnd | BxPrefixSSE, BX_IA_VMCALL, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C2 */ { BxTraceEnd | BxPrefixSSE, BX_IA_VMLAUNCH, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C3 */ { BxTraceEnd | BxPrefixSSE, BX_IA_VMRESUME, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C4 */ { BxTraceEnd | BxPrefixSSE, BX_IA_VMXOFF, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C5 */ { 0, BX_IA_ERROR },
  /* 0F 01 C6 */ { 0, BX_IA_ERROR },
  /* 0F 01 C7 */ { 0, BX_IA_ERROR },
  /* 0F 01 C8 */ { BxPrefixSSE, BX_IA_MONITOR, BxOpcodeGroupSSE_ERR },
  /* 0F 01 C9 */ { BxPrefixSSE | BxTraceEnd, BX_IA_MWAIT, BxOpcodeGroupSSE_ERR },
  /* 0F 01 CA */ { 0, BX_IA_ERROR },
  /* 0F 01 CB */ { 0, BX_IA_ERROR },
  /* 0F 01 CC */ { 0, BX_IA_ERROR },
  /* 0F 01 CD */ { 0, BX_IA_ERROR },
  /* 0F 01 CE */ { 0, BX_IA_ERROR },
  /* 0F 01 CF */ { 0, BX_IA_ERROR },
  /* 0F 01 D0 */ { BxPrefixSSE, BX_IA_XGETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D1 */ { BxPrefixSSE, BX_IA_XSETBV, BxOpcodeGroupSSE_ERR },
  /* 0F 01 D2 */ { 0, BX_IA_ERROR },
  /* 0F 01 D3 */ { 0, BX_IA_ERROR },
  /* 0F 01 D4 */ { 0, BX_IA_ERROR },
  /* 0F 01 D5 */ { 0, BX_IA_ERROR },
  /* 0F 01 D6 */ { 0, BX_IA_ERROR },
  /* 0F 01 D7 */ { 0, BX_IA_ERROR },
  /* 0F 01 D8 */ { 0, BX_IA_ERROR },
  /* 0F 01 D9 */ { 0, BX_IA_ERROR },
  /* 0F 01 DA */ { 0, BX_IA_ERROR },
  /* 0F 01 DB */ { 0, BX_IA_ERROR },
  /* 0F 01 DC */ { 0, BX_IA_ERROR },
  /* 0F 01 DD */ { 0, BX_IA_ERROR },
  /* 0F 01 DE */ { 0, BX_IA_ERROR },
  /* 0F 01 DF */ { 0, BX_IA_ERROR },
  /* 0F 01 E0 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E1 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E2 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E3 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E4 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E5 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E6 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E7 */ { 0, BX_IA_SMSW_EwR },
  /* 0F 01 E8 */ { 0, BX_IA_ERROR },
  /* 0F 01 E9 */ { 0, BX_IA_ERROR },
  /* 0F 01 EA */ { 0, BX_IA_ERROR },
  /* 0F 01 EB */ { 0, BX_IA_ERROR },
  /* 0F 01 EC */ { 0, BX_IA_ERROR },
  /* 0F 01 ED */ { 0, BX_IA_ERROR },
  /* 0F 01 EE */ { 0, BX_IA_ERROR },
  /* 0F 01 EF */ { 0, BX_IA_ERROR },
  /* 0F 01 F0 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F1 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F2 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F3 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F4 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F5 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F7 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 0F 01 F8 */ { 0, BX_IA_SWAPGS },
  /* 0F 01 F9 */ { 0, BX_IA_RDTSCP },
  /* 0F 01 FA */ { 0, BX_IA_ERROR },
  /* 0F 01 FB */ { 0, BX_IA_ERROR },
  /* 0F 01 FC */ { 0, BX_IA_ERROR },
  /* 0F 01 FD */ { 0, BX_IA_ERROR },
  /* 0F 01 FE */ { 0, BX_IA_ERROR },
  /* 0F 01 FF */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG7M[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_SGDT_Ms },
  /* 1 */ { 0, BX_IA_SIDT_Ms },
  /* 2 */ { 0, BX_IA_LGDT_Ms },
  /* 3 */ { 0, BX_IA_LIDT_Ms },
  /* 4 */ { 0, BX_IA_SMSW_EwM },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { BxTraceEnd, BX_IA_INVLPG }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G7M[8] = {
  /* 0 */ { 0, BX_IA_SGDT64_Ms },
  /* 1 */ { 0, BX_IA_SIDT64_Ms },
  /* 2 */ { 0, BX_IA_LGDT64_Ms },
  /* 3 */ { 0, BX_IA_LIDT64_Ms },
  /* 4 */ { 0, BX_IA_SMSW_EwM },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { BxTraceEnd, BX_IA_INVLPG  }
};
#endif

/* ******* */
/* Group 8 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EwIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EwIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EwIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EwIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EwIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EwIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EwIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EwIbR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EdIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EdIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EdIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EdIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EdIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EdIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EdIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EdIbR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EqIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EqIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EqIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EqIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EqIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EqIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EqIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EqIbR }
};
#endif

/* ******* */
/* Group 9 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG9M[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G9qM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG16B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxPrefixSSE, BX_IA_VMPTRLD_Mq, BxOpcodeGroupSSE_G9VMX6 },
  /* 7 */ { BxPrefixSSE, BX_IA_VMPTRST_Mq, BxOpcodeGroupSSE_ERR }
};
#endif

/* ******** */
/* Group 11 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG11EbR[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_MOV_EbIbR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11EbM[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_MOV_EbIbM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11EwR[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_MOV_EwIwR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11EwM[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_MOV_EwIwM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11EdR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EdIdR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG11EdM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EdIdM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G11EqR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EqIdR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G11EqM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_MOV_EqIdM },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSRLW_PqIb, BxOpcodeGroupSSE_G1202 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSRAW_PqIb, BxOpcodeGroupSSE_G1204 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSLLW_PqIb, BxOpcodeGroupSSE_G1206 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG13R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSRLD_PqIb, BxOpcodeGroupSSE_G1302 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSRAD_PqIb, BxOpcodeGroupSSE_G1304 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSLLD_PqIb, BxOpcodeGroupSSE_G1306 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSRLQ_PqIb, BxOpcodeGroupSSE_G1402 },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_PSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_PSLLQ_PqIb, BxOpcodeGroupSSE_G1406 },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_PSLLDQ_UdqIb }
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG15M[8] = {
  /* 0 */ { BxPrefixSSE, BX_IA_FXSAVE,  BxOpcodeGroupSSE_ERR },
  /* 1 */ { BxPrefixSSE, BX_IA_FXRSTOR, BxOpcodeGroupSSE_ERR },
  /* 2 */ { BxPrefixSSE, BX_IA_LDMXCSR, BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_STMXCSR, BxOpcodeGroupSSE_ERR },
  /* 4 */ { BxPrefixSSE, BX_IA_XSAVE,   BxOpcodeGroupSSE_ERR },
  /* 5 */ { BxPrefixSSE, BX_IA_XRSTOR,  BxOpcodeGroupSSE_ERR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { BxPrefixSSE, BX_IA_CLFLUSH, BxOpcodeGroupSSE_ERR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG15R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { BxPrefixSSE, BX_IA_NOP, BxOpcodeGroupSSE_ERR }, /* LFENCE */
  /* 6 */ { BxPrefixSSE, BX_IA_NOP, BxOpcodeGroupSSE_ERR }, /* MFENCE */
  /* 7 */ { BxPrefixSSE, BX_IA_NOP, BxOpcodeGroupSSE_ERR }  /* SFENCE */
};

#endif // BX_COMMON_FETCHDECODE_TABLES_H
