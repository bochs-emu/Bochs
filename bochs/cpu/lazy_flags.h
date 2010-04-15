/////////////////////////////////////////////////////////////////////////
// $Id: lazy_flags.h,v 1.38 2010-04-15 05:51:00 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

#ifndef BX_LAZY_FLAGS_DEF
#define BX_LAZY_FLAGS_DEF

#define BX_LF_INSTR_ADD8           1
#define BX_LF_INSTR_ADC8           2

#define BX_LF_INSTR_ADD16          3
#define BX_LF_INSTR_ADC16          4

#define BX_LF_INSTR_ADD32          5
#define BX_LF_INSTR_ADC32          6

#define BX_LF_INSTR_ADD64          7
#define BX_LF_INSTR_ADC64          8

#define BX_LF_INSTR_ADD_ADC8(cf)  (1 + (cf))
#define BX_LF_INSTR_ADD_ADC16(cf) (3 + (cf))
#define BX_LF_INSTR_ADD_ADC32(cf) (5 + (cf))
#define BX_LF_INSTR_ADD_ADC64(cf) (7 + (cf))

#define BX_LF_INSTR_SUB8           9
#define BX_LF_INSTR_SBB8           10

#define BX_LF_INSTR_SUB16          11
#define BX_LF_INSTR_SBB16          12

#define BX_LF_INSTR_SUB32          13
#define BX_LF_INSTR_SBB32          14

#define BX_LF_INSTR_SUB64          15
#define BX_LF_INSTR_SBB64          16

#define BX_LF_INSTR_SUB_SBB8(cf)  (9  + (cf))
#define BX_LF_INSTR_SUB_SBB16(cf) (11 + (cf))
#define BX_LF_INSTR_SUB_SBB32(cf) (13 + (cf))
#define BX_LF_INSTR_SUB_SBB64(cf) (15 + (cf))

#define BX_LF_INSTR_INC8           17
#define BX_LF_INSTR_INC16          18
#define BX_LF_INSTR_INC32          19
#define BX_LF_INSTR_INC64          20

#define BX_LF_INSTR_DEC8           21
#define BX_LF_INSTR_DEC16          22
#define BX_LF_INSTR_DEC32          23
#define BX_LF_INSTR_DEC64          24

#define BX_LF_INSTR_NEG8           25
#define BX_LF_INSTR_NEG16          26
#define BX_LF_INSTR_NEG32          27
#define BX_LF_INSTR_NEG64          28

#define BX_LF_INSTR_LOGIC8         29
#define BX_LF_INSTR_LOGIC16        30
#define BX_LF_INSTR_LOGIC32        31
#define BX_LF_INSTR_LOGIC64        32

#if BX_SUPPORT_X86_64
  #define BX_LF_SIGN_BIT  63
#else
  #define BX_LF_SIGN_BIT  31
#endif

typedef struct {
  bx_address op1;
  bx_address op2;
  bx_address result;
  unsigned instr;
} bx_lf_flags_entry;

// *******************
// OSZAPC
// *******************

/* op1, op2, result */
#define SET_FLAGS_OSZAPC_SIZE(size, lf_op1, lf_op2, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op1    = (bx_address)(Bit##size##s)(lf_op1); \
  BX_CPU_THIS_PTR oszapc.op2    = (bx_address)(Bit##size##s)(lf_op2); \
  BX_CPU_THIS_PTR oszapc.result = (bx_address)(Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_8(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(8, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_16(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(16, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_32(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(32, op1, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_64(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(64, op1, op2, result, ins)
#endif

/* op1 and result only */
#define SET_FLAGS_OSZAPC_S1_SIZE(size, lf_op1, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op1    = (bx_address)(Bit##size##s)(lf_op1); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_S1_8(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(8, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_16(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(16, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_32(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(32, op1, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S1_64(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(64, op1, result, ins)
#endif

/* op2 and result only */
#define SET_FLAGS_OSZAPC_S2_SIZE(size, lf_op2, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op2    = (bx_address)(Bit##size##s)(lf_op2); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_S2_8(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(8, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_16(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(16, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_32(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(32, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S2_64(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(64, op2, result, ins)
#endif

/* result only */
#define SET_FLAGS_OSZAPC_RESULT_SIZE(size, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_RESULT_8(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(8, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_16(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(16, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_32(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_RESULT_64(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(64, result, ins)
#endif

// *******************
// OSZAP
// *******************

/* result only */
#define SET_FLAGS_OSZAP_RESULT_SIZE(size, lf_result, ins) { \
  force_CF(); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPMask; \
}

#define SET_FLAGS_OSZAP_RESULT_8(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, result, ins)
#define SET_FLAGS_OSZAP_RESULT_16(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, result, ins)
#define SET_FLAGS_OSZAP_RESULT_32(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_RESULT_64(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, result, ins)
#endif

// transition to new lazy flags code
#define SET_FLAGS_OSZAPC_LOGIC_8(result_8) \
   SET_FLAGS_OSZAPC_RESULT_8((result_8), BX_LF_INSTR_LOGIC8)
#define SET_FLAGS_OSZAPC_LOGIC_16(result_16) \
   SET_FLAGS_OSZAPC_RESULT_16((result_16), BX_LF_INSTR_LOGIC16)
#define SET_FLAGS_OSZAPC_LOGIC_32(result_32) \
   SET_FLAGS_OSZAPC_RESULT_32((result_32), BX_LF_INSTR_LOGIC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_LOGIC_64(result_64) \
   SET_FLAGS_OSZAPC_RESULT_64((result_64), BX_LF_INSTR_LOGIC64)
#endif

#define SET_FLAGS_OSZAPC_ADD_8(op1_8, op2_8, sum_8) \
  SET_FLAGS_OSZAPC_8((op1_8), (op2_8), (sum_8), BX_LF_INSTR_ADD8)
#define SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16) \
  SET_FLAGS_OSZAPC_16((op1_16), (op2_16), (sum_16), BX_LF_INSTR_ADD16)
#define SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32) \
  SET_FLAGS_OSZAPC_32((op1_32), (op2_32), (sum_32), BX_LF_INSTR_ADD32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64) \
  SET_FLAGS_OSZAPC_64((op1_64), (op2_64), (sum_64), BX_LF_INSTR_ADD64)
#endif

#define SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8) \
  SET_FLAGS_OSZAPC_8((op1_8), (op2_8), (diff_8), BX_LF_INSTR_SUB8)
#define SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16) \
  SET_FLAGS_OSZAPC_16((op1_16), (op2_16), (diff_16), BX_LF_INSTR_SUB16)
#define SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32) \
  SET_FLAGS_OSZAPC_32((op1_32), (op2_32), (diff_32), BX_LF_INSTR_SUB32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64) \
  SET_FLAGS_OSZAPC_64((op1_64), (op2_64), (diff_64), BX_LF_INSTR_SUB64)
#endif

#define SET_FLAGS_OSZAPC_INC_8(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, (result), BX_LF_INSTR_INC8)
#define SET_FLAGS_OSZAPC_INC_16(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, (result), BX_LF_INSTR_INC16)
#define SET_FLAGS_OSZAPC_INC_32(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, (result), BX_LF_INSTR_INC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_INC_64(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, (result), BX_LF_INSTR_INC64)
#endif

#define SET_FLAGS_OSZAPC_DEC_8(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, (result), BX_LF_INSTR_DEC8)
#define SET_FLAGS_OSZAPC_DEC_16(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, (result), BX_LF_INSTR_DEC16)
#define SET_FLAGS_OSZAPC_DEC_32(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, (result), BX_LF_INSTR_DEC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_DEC_64(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, (result), BX_LF_INSTR_DEC64)
#endif

#endif // BX_LAZY_FLAGS_DEF
