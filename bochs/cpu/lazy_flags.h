/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2011  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_LAZY_FLAGS_DEF
#define BX_LAZY_FLAGS_DEF

#if BX_SUPPORT_X86_64
  #define BX_LF_SIGN_BIT  63
#else
  #define BX_LF_SIGN_BIT  31
#endif

typedef struct {
  bx_address result;
  bx_address auxbits;
} bx_lf_flags_entry;

// These are the lazy flags bits in oszapc.auxbits which hold lazy state
// of zero flag, adjust flag, carry flag, and overflow flag.

#define LF_BIT_SD      (0)          /* lazy Sign Flag Delta            */
#define LF_BIT_AF      (3)          /* lazy Adjust flag                */
#define LF_BIT_PDB     (8)          /* lazy Parity Delta Byte (8 bits) */
#define LF_BIT_CF      (31)         /* lazy Carry Flag                 */
#define LF_BIT_PO      (30)         /* lazy Partial Overflow = CF ^ OF */

#define LF_MASK_SD     (0x01 << LF_BIT_SD)
#define LF_MASK_AF     (0x01 << LF_BIT_AF)
#define LF_MASK_PDB    (0xFF << LF_BIT_PDB)
#define LF_MASK_CF     (0x01 << LF_BIT_CF)
#define LF_MASK_PO     (0x01 << LF_BIT_PO)

#define ADD_COUT_VEC(op1, op2, result) \
  (((op1) & (op2)) | (((op1) | (op2)) & (~(result))))

#define SUB_COUT_VEC(op1, op2, result) \
  (((~(op1)) & (op2)) | (((~(op1)) ^ (op2)) & (result)))

#define GET_ADD_OVERFLOW(op1, op2, result, mask) \
  ((((op1) ^ (result)) & ((op2) ^ (result))) & (mask))

// *******************
// OSZAPC
// *******************

/* size, carries, result */
#define SET_FLAGS_OSZAPC_SIZE(size, lf_carries, lf_result) { \
  bx_address temp = ((lf_carries) & (LF_MASK_AF)) | \
        (((lf_carries) >> (size - 2)) << LF_BIT_PO); \
  BX_CPU_THIS_PTR oszapc.result = (bx_address)(Bit##size##s)(lf_result); \
  if ((size) == 32) temp = ((lf_carries) & ~(LF_MASK_PDB | LF_MASK_SD)); \
  if ((size) == 16) temp = ((lf_carries) & (LF_MASK_AF)) | ((lf_carries) << 16); \
  if ((size) == 8)  temp = ((lf_carries) & (LF_MASK_AF)) | ((lf_carries) << 24); \
  BX_CPU_THIS_PTR oszapc.auxbits = (bx_address)(Bit32u)temp; \
}

/* carries, result */
#define SET_FLAGS_OSZAPC_8(carries, result) \
  SET_FLAGS_OSZAPC_SIZE(8, carries, result)
#define SET_FLAGS_OSZAPC_16(carries, result) \
  SET_FLAGS_OSZAPC_SIZE(16, carries, result)
#define SET_FLAGS_OSZAPC_32(carries, result) \
  SET_FLAGS_OSZAPC_SIZE(32, carries, result)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_64(carries, result) \
  SET_FLAGS_OSZAPC_SIZE(64, carries, result)
#endif

/* result */
#define SET_FLAGS_OSZAPC_LOGIC_8(result_8) \
   SET_FLAGS_OSZAPC_8(0, (result_8))
#define SET_FLAGS_OSZAPC_LOGIC_16(result_16) \
   SET_FLAGS_OSZAPC_16(0, (result_16))
#define SET_FLAGS_OSZAPC_LOGIC_32(result_32) \
   SET_FLAGS_OSZAPC_32(0, (result_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_LOGIC_64(result_64) \
   SET_FLAGS_OSZAPC_64(BX_CONST64(0), (result_64))
#endif

/* op1, op2, result */
#define SET_FLAGS_OSZAPC_ADD_8(op1_8, op2_8, sum_8) \
  SET_FLAGS_OSZAPC_8(ADD_COUT_VEC((op1_8), (op2_8), (sum_8)), (sum_8))
#define SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16) \
  SET_FLAGS_OSZAPC_16(ADD_COUT_VEC((op1_16), (op2_16), (sum_16)), (sum_16))
#define SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32) \
  SET_FLAGS_OSZAPC_32(ADD_COUT_VEC((op1_32), (op2_32), (sum_32)), (sum_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64) \
  SET_FLAGS_OSZAPC_64(ADD_COUT_VEC((op1_64), (op2_64), (sum_64)), (sum_64))
#endif

/* op1, op2, result */
#define SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8) \
  SET_FLAGS_OSZAPC_8(SUB_COUT_VEC((op1_8), (op2_8), (diff_8)), (diff_8))
#define SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16) \
  SET_FLAGS_OSZAPC_16(SUB_COUT_VEC((op1_16), (op2_16), (diff_16)), (diff_16))
#define SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32) \
  SET_FLAGS_OSZAPC_32(SUB_COUT_VEC((op1_32), (op2_32), (diff_32)), (diff_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64) \
  SET_FLAGS_OSZAPC_64(SUB_COUT_VEC((op1_64), (op2_64), (diff_64)), (diff_64))
#endif

// *******************
// OSZAP
// *******************

/* size, carries, result */
#define SET_FLAGS_OSZAP_SIZE(size, lf_carries, lf_result) { \
  bx_address temp = ((lf_carries) & (LF_MASK_AF)) | \
        (((lf_carries) >> (size - 2)) << LF_BIT_PO); \
  if ((size) == 32) temp = ((lf_carries) & ~(LF_MASK_PDB | LF_MASK_SD)); \
  if ((size) == 16) temp = ((lf_carries) & (LF_MASK_AF)) | ((lf_carries) << 16); \
  if ((size) == 8)  temp = ((lf_carries) & (LF_MASK_AF)) | ((lf_carries) << 24); \
  BX_CPU_THIS_PTR oszapc.result = (bx_address)(Bit##size##s)(lf_result); \
  bx_address delta_c = (BX_CPU_THIS_PTR oszapc.auxbits ^ temp) & LF_MASK_CF; \
  delta_c ^= (delta_c >> 1); \
  BX_CPU_THIS_PTR oszapc.auxbits = (bx_address)(Bit32u)(temp ^ delta_c); \
}

/* carries, result */
#define SET_FLAGS_OSZAP_8(carries, result) \
  SET_FLAGS_OSZAP_SIZE(8, carries, result)
#define SET_FLAGS_OSZAP_16(carries, result) \
  SET_FLAGS_OSZAP_SIZE(16, carries, result)
#define SET_FLAGS_OSZAP_32(carries, result) \
  SET_FLAGS_OSZAP_SIZE(32, carries, result)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_64(carries, result) \
  SET_FLAGS_OSZAP_SIZE(64, carries, result)
#endif

/* op1, op2, result */
#define SET_FLAGS_OSZAP_ADD_8(op1_8, op2_8, sum_8) \
  SET_FLAGS_OSZAP_8(ADD_COUT_VEC((op1_8), (op2_8), (sum_8)), (sum_8))
#define SET_FLAGS_OSZAP_ADD_16(op1_16, op2_16, sum_16) \
  SET_FLAGS_OSZAP_16(ADD_COUT_VEC((op1_16), (op2_16), (sum_16)), (sum_16))
#define SET_FLAGS_OSZAP_ADD_32(op1_32, op2_32, sum_32) \
  SET_FLAGS_OSZAP_32(ADD_COUT_VEC((op1_32), (op2_32), (sum_32)), (sum_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_ADD_64(op1_64, op2_64, sum_64) \
  SET_FLAGS_OSZAP_64(ADD_COUT_VEC((op1_64), (op2_64), (sum_64)), (sum_64))
#endif

/* op1, op2, result */
#define SET_FLAGS_OSZAP_SUB_8(op1_8, op2_8, diff_8) \
  SET_FLAGS_OSZAP_8(SUB_COUT_VEC((op1_8), (op2_8), (diff_8)), (diff_8))
#define SET_FLAGS_OSZAP_SUB_16(op1_16, op2_16, diff_16) \
  SET_FLAGS_OSZAP_16(SUB_COUT_VEC((op1_16), (op2_16), (diff_16)), (diff_16))
#define SET_FLAGS_OSZAP_SUB_32(op1_32, op2_32, diff_32) \
  SET_FLAGS_OSZAP_32(SUB_COUT_VEC((op1_32), (op2_32), (diff_32)), (diff_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_SUB_64(op1_64, op2_64, diff_64) \
  SET_FLAGS_OSZAP_64(SUB_COUT_VEC((op1_64), (op2_64), (diff_64)), (diff_64))
#endif

// *******************
// OSZAxC
// *******************

/* size, carries, result */
#define SET_FLAGS_OSZAxC_LOGIC_SIZE(size, lf_result) { \
  bx_bool saved_PF = getB_PF(); \
  SET_FLAGS_OSZAPC_SIZE(size, (Bit##size##u)(0), lf_result); \
  set_PF(saved_PF); \
}

/* result */
#define SET_FLAGS_OSZAxC_LOGIC_32(result_32) \
   SET_FLAGS_OSZAxC_LOGIC_SIZE(32, (result_32))
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAxC_LOGIC_64(result_64) \
   SET_FLAGS_OSZAxC_LOGIC_SIZE(64, (result_64))
#endif

#endif // BX_LAZY_FLAGS_DEF
