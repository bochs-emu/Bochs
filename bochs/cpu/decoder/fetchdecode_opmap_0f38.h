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

#ifndef BX_FETCHDECODE_OPMAP_0F38_H
#define BX_FETCHDECODE_OPMAP_0F38_H

#if BX_CPU_LEVEL >= 6

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

// opcode 0F 38 00
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3800[4] = {
  /* -- */ { 0, BX_IA_PSHUFB_PqQq },
  /* 66 */ { 0, BX_IA_PSHUFB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 01
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3801[4] = {
  /* -- */ { 0, BX_IA_PHADDW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 02
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3802[4] = {
  /* -- */ { 0, BX_IA_PHADDD_PqQq },
  /* 66 */ { 0, BX_IA_PHADDD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 03
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3803[4] = {
  /* -- */ { 0, BX_IA_PHADDSW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 04
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3804[4] = {
  /* -- */ { 0, BX_IA_PMADDUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PMADDUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 05
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3805[4] = {
  /* -- */ { 0, BX_IA_PHSUBW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 06
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3806[4] = {
  /* -- */ { 0, BX_IA_PHSUBD_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 07
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3807[4] = {
  /* -- */ { 0, BX_IA_PHSUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 08
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3808[4] = {
  /* -- */ { 0, BX_IA_PSIGNB_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 09
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3809[4] = {
  /* -- */ { 0, BX_IA_PSIGNW_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 0A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F380A[4] = {
  /* -- */ { 0, BX_IA_PSIGND_PqQq },
  /* 66 */ { 0, BX_IA_PSIGND_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 0B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F380B[4] = {
  /* -- */ { 0, BX_IA_PMULHRSW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHRSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 10
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3810[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PBLENDVB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 14
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3814[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDVPS_VpsWps },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 15
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3815[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDVPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 17
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3817[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PTEST_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 1C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F381C[4] = {
  /* -- */ { 0, BX_IA_PABSB_PqQq },
  /* 66 */ { 0, BX_IA_PABSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 1D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F381D[4] = {
  /* -- */ { 0, BX_IA_PABSW_PqQq },
  /* 66 */ { 0, BX_IA_PABSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 1E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F381E[4] = {
  /* -- */ { 0, BX_IA_PABSD_PqQq },
  /* 66 */ { 0, BX_IA_PABSD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 20
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3820[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBW_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 21
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3821[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBD_VdqWd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 22
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3822[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBQ_VdqWw },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 23
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3823[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXWD_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 24
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3824[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXWQ_VdqWd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 25
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3825[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXDQ_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 28
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3828[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMULDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 29
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3829[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPEQQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 2A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F382A[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_MOVNTDQA_VdqMdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 2B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F382B[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PACKUSDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 30
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3830[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBW_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 31
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3831[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBD_VdqWd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 32
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3832[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBQ_VdqWw },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 33
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3833[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXWD_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 34
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3834[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXWQ_VdqWd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 35
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3835[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXDQ_VdqWq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 37
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3837[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPGTQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 38
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3838[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 39
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3839[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINSD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3A
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383A[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINUW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3B
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383B[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINUD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3C
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383C[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3D
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383D[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXSD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3E
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383E[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXUW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 3F
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F383F[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXUD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 40
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3840[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMULLD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 41
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3841[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PHMINPOSUW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 80
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3880[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_INVEPT },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 81
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3881[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_INVVPID },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 82
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F3882[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_INVPCID },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 C8
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38C8[4] = {
  /* -- */ { 0, BX_IA_SHA1NEXTE_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 C9
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38C9[4] = {
  /* -- */ { 0, BX_IA_SHA1MSG1_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 CA
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38CA[4] = {
  /* -- */ { 0, BX_IA_SHA1MSG2_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 CB
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38CB[4] = {
  /* -- */ { 0, BX_IA_SHA256RNDS2_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 CC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38CC[4] = {
  /* -- */ { 0, BX_IA_SHA256MSG1_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 CD
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38CD[4] = {
  /* -- */ { 0, BX_IA_SHA256MSG2_VdqWdq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 CF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38CF[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_GF2P8MULB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 DB
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38DB[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_AESIMC_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 DC
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38DC[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_AESENC_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 DD
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38DD[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_AESENCLAST_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 DE
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38DE[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_AESDEC_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 DF
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38DF[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_AESDECLAST_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

// opcode 0F 38 F0
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38F0[] = {
  // osize = 16 bit
  /* -- /w */ { 0, BX_IA_MOVBE_GwMw },
  /* 66 /w */ { 0, BX_IA_MOVBE_GwMw },
  /* F3 /w */ { 0, BX_IA_ERROR },
  /* F2 /w */ { 0, BX_IA_CRC32_GdEb },
  // osize = 32 bit
  /* -- /d */ { 0, BX_IA_MOVBE_GdMd },
  /* 66 /d */ { 0, BX_IA_MOVBE_GdMd },
  /* F3 /d */ { 0, BX_IA_ERROR },
  /* F2 /d */ { 0, BX_IA_CRC32_GdEb },
#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* -- /q */ { 0, BX_IA_MOVBE_GqMq },
  /* 66 /q */ { 0, BX_IA_MOVBE_GqMq },
  /* F3 /q */ { 0, BX_IA_ERROR },
  /* F2 /q */ { 0, BX_IA_CRC32_GdEb },
#endif
};

// opcode 0F 38 F1
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38F1[] = {
  // osize = 16 bit
  /* -- /w */ { 0, BX_IA_MOVBE_MwGw },
  /* 66 /w */ { 0, BX_IA_MOVBE_MwGw },
  /* F3 /w */ { 0, BX_IA_ERROR },
  /* F2 /w */ { 0, BX_IA_CRC32_GdEw },
  // osize = 32 bit
  /* -- /d */ { 0, BX_IA_MOVBE_MdGd },
  /* 66 /d */ { 0, BX_IA_MOVBE_MdGd },
  /* F3 /d */ { 0, BX_IA_ERROR },
  /* F2 /d */ { 0, BX_IA_CRC32_GdEd },
#if BX_SUPPORT_X86_64
  // osize = 64 bit
  /* -- /q */ { 0, BX_IA_MOVBE_MqGq },
  /* 66 /q */ { 0, BX_IA_MOVBE_GqMq },
  /* F3 /q */ { 0, BX_IA_ERROR },
  /* F2 /q */ { 0, BX_IA_CRC32_GdEq },
#endif
};

// opcode 0F 38 F6
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0F38F6[] = {
  /* --    */ { 0, BX_IA_ERROR },
  /* 66    */ { 0, BX_IA_ADCX_GdEd },
  /* F3    */ { 0, BX_IA_ADOX_GdEd },
  /* F2    */ { 0, BX_IA_ERROR },
#if BX_SUPPORT_X86_64
  /* -- /q */ { 0, BX_IA_ERROR },
  /* 66 /q */ { 0, BX_IA_ADCX_GqEq },
  /* F3 /q */ { 0, BX_IA_ADOX_GqEq },
  /* F2 /q */ { 0, BX_IA_ERROR },
#endif
};

#endif

#endif // BX_FETCHDECODE_OPMAP_0F38_H
