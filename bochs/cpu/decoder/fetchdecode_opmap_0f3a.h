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

#ifndef BX_FETCHDECODE_OPMAP_0F3A_H
#define BX_FETCHDECODE_OPMAP_0F3A_H

#if BX_CPU_LEVEL >= 6

/* ************************************************************************ */
/* 3-byte opcode table (Table A-5, 0F 3A) */

// opcode 0F 3A 08
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A08[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_ROUNDPS_VpsWpsIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 09
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A09[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_ROUNDPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0A[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_ROUNDSS_VssWssIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0B[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_ROUNDSD_VsdWsdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0C[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_BLENDPS_VpsWpsIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0D[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_BLENDPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0E[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PBLENDW_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 0F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A0F[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PALIGNR_PqQqIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_PALIGNR_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 14
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A14[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRB_EbdVdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 15
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A15[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRW_EwdVdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 16
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A16[] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRD_EdVdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
#if BX_SUPPORT_X86_64
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRQ_EqVdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
#endif
};

// opcode 0F 3A 17
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A17[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_EXTRACTPS_EdVpsIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 20
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A20[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PINSRB_VdqEbIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 21
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A21[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_INSERTPS_VpsWssIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 22
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A22[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PINSRD_VdqEdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 40
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A40[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_DPPS_VpsWpsIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 41
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A41[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_DPPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 42
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A42[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_MPSADBW_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 44
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A44[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PCLMULQDQ_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 60
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A60[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PCMPESTRM_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 61
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A61[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PCMPESTRI_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 62
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A62[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PCMPISTRM_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A 63
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3A63[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_PCMPISTRI_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A CC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3ACC[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_SHA1RNDS4_VdqWdqIb },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A CE
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3ACE[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_GF2P8AFFINEQB_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A CF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3ACF[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_GF2P8AFFINEINVQB_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 3A DF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3ADF[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { BxImmediate_Ib, BX_IA_AESKEYGENASSIST_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

#endif

#endif // BX_FETCHDECODE_OPMAP_0F3A_H
