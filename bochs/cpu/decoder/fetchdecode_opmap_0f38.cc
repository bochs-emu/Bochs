/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2024 Stanislav Shwartsman
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

#include "config.h"

#if BX_CPU_LEVEL >= 6

#ifndef BX_STANDALONE_DECODER
#define BX_STANDALONE_DECODER
#endif

#include "fetchdecode.h"

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

// opcode 0F 38 00
static const Bit64u BxOpcodeTable0F3800[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSHUFB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSHUFB_VdqWdq),
};

// opcode 0F 38 01
static const Bit64u BxOpcodeTable0F3801[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHADDW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHADDW_VdqWdq),
};

// opcode 0F 38 02
static const Bit64u BxOpcodeTable0F3802[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHADDD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHADDD_VdqWdq),
};

// opcode 0F 38 03
static const Bit64u BxOpcodeTable0F3803[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHADDSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHADDSW_VdqWdq),
};

// opcode 0F 38 04
static const Bit64u BxOpcodeTable0F3804[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMADDUBSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMADDUBSW_VdqWdq),
};

// opcode 0F 38 05
static const Bit64u BxOpcodeTable0F3805[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHSUBW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHSUBW_VdqWdq),
};

// opcode 0F 38 06
static const Bit64u BxOpcodeTable0F3806[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHSUBD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHSUBD_VdqWdq),
};

// opcode 0F 38 07
static const Bit64u BxOpcodeTable0F3807[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PHSUBSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHSUBSW_VdqWdq),
};

// opcode 0F 38 08
static const Bit64u BxOpcodeTable0F3808[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSIGNB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSIGNB_VdqWdq),
};

// opcode 0F 38 09
static const Bit64u BxOpcodeTable0F3809[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSIGNW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSIGNW_VdqWdq),
};

// opcode 0F 38 0A
static const Bit64u BxOpcodeTable0F380A[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PSIGND_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PSIGND_VdqWdq),
};

// opcode 0F 38 0B
static const Bit64u BxOpcodeTable0F380B[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PMULHRSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULHRSW_VdqWdq),
};

static const Bit64u BxOpcodeTable0F3810[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PBLENDVB_VdqWdq) };
static const Bit64u BxOpcodeTable0F3814[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_BLENDVPS_VpsWps) };
static const Bit64u BxOpcodeTable0F3815[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_BLENDVPD_VpdWpd) };
static const Bit64u BxOpcodeTable0F3817[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PTEST_VdqWdq) };

// opcode 0F 38 1C
static const Bit64u BxOpcodeTable0F381C[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PABSB_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PABSB_VdqWdq),
};

// opcode 0F 38 1D
static const Bit64u BxOpcodeTable0F381D[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PABSW_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PABSW_VdqWdq),
};

// opcode 0F 38 1E
static const Bit64u BxOpcodeTable0F381E[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PABSD_PqQq),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PABSD_VdqWdq),
};

static const Bit64u BxOpcodeTable0F3820[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXBW_VdqWq) };
static const Bit64u BxOpcodeTable0F3821[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXBD_VdqWd) };
static const Bit64u BxOpcodeTable0F3822[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXBQ_VdqWw) };
static const Bit64u BxOpcodeTable0F3823[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXWD_VdqWq) };
static const Bit64u BxOpcodeTable0F3824[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXWQ_VdqWd) };
static const Bit64u BxOpcodeTable0F3825[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVSXDQ_VdqWq) };
static const Bit64u BxOpcodeTable0F3828[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULDQ_VdqWdq) };
static const Bit64u BxOpcodeTable0F3829[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPEQQ_VdqWdq) };
static const Bit64u BxOpcodeTable0F382A[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_MOVNTDQA_VdqMdq) };
static const Bit64u BxOpcodeTable0F382B[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PACKUSDW_VdqWdq) };
static const Bit64u BxOpcodeTable0F3830[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXBW_VdqWq) };
static const Bit64u BxOpcodeTable0F3831[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXBD_VdqWd) };
static const Bit64u BxOpcodeTable0F3832[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXBQ_VdqWw) };
static const Bit64u BxOpcodeTable0F3833[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXWD_VdqWq) };
static const Bit64u BxOpcodeTable0F3834[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXWQ_VdqWd) };
static const Bit64u BxOpcodeTable0F3835[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMOVZXDQ_VdqWq) };
static const Bit64u BxOpcodeTable0F3837[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPGTQ_VdqWdq) };
static const Bit64u BxOpcodeTable0F3838[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINSB_VdqWdq) };
static const Bit64u BxOpcodeTable0F3839[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINSD_VdqWdq) };
static const Bit64u BxOpcodeTable0F383A[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINUW_VdqWdq) };
static const Bit64u BxOpcodeTable0F383B[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMINUD_VdqWdq) };
static const Bit64u BxOpcodeTable0F383C[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXSB_VdqWdq) };
static const Bit64u BxOpcodeTable0F383D[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXSD_VdqWdq) };
static const Bit64u BxOpcodeTable0F383E[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXUW_VdqWdq) };
static const Bit64u BxOpcodeTable0F383F[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMAXUD_VdqWdq) };
static const Bit64u BxOpcodeTable0F3840[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PMULLD_VdqWdq) };
static const Bit64u BxOpcodeTable0F3841[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PHMINPOSUW_VdqWdq) };
static const Bit64u BxOpcodeTable0F3880[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_INVEPT) };
static const Bit64u BxOpcodeTable0F3881[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_INVVPID) };
static const Bit64u BxOpcodeTable0F3882[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_INVPCID) };

#if BX_SUPPORT_X86_64
static const Bit64u BxOpcodeTable0F388A[] = { last_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_IS64, BX_IA_MOVRS_GbEb) };
static const Bit64u BxOpcodeTable0F388B[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_OS64 | ATTR_IS64, BX_IA_MOVRS_GqEq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_OS32 | ATTR_IS64, BX_IA_MOVRS_GdEd),
  last_opcode(ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM | ATTR_OS16 | ATTR_IS64, BX_IA_MOVRS_GwEw)
};
#endif

static const Bit64u BxOpcodeTable0F38C8[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA1NEXTE_VdqWdq) };
static const Bit64u BxOpcodeTable0F38C9[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA1MSG1_VdqWdq) };
static const Bit64u BxOpcodeTable0F38CA[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA1MSG2_VdqWdq) };
static const Bit64u BxOpcodeTable0F38CB[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA256RNDS2_VdqWdq) };
static const Bit64u BxOpcodeTable0F38CC[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA256MSG1_VdqWdq) };
static const Bit64u BxOpcodeTable0F38CD[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA256MSG2_VdqWdq) };
static const Bit64u BxOpcodeTable0F38CF[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_GF2P8MULB_VdqWdq) };
static const Bit64u BxOpcodeTable0F38DB[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESIMC_VdqWdq) };
static const Bit64u BxOpcodeTable0F38DC[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESENC_VdqWdq) };
static const Bit64u BxOpcodeTable0F38DD[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESENCLAST_VdqWdq) };
static const Bit64u BxOpcodeTable0F38DE[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESDEC_VdqWdq) };
static const Bit64u BxOpcodeTable0F38DF[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESDECLAST_VdqWdq) };

// opcode 0F 38 F0
static const Bit64u BxOpcodeTable0F38F0[] = {
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS16 | ATTR_MOD_MEM, BX_IA_MOVBE_GwMw),
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS32 | ATTR_MOD_MEM, BX_IA_MOVBE_GdMd),
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS64 | ATTR_MOD_MEM, BX_IA_MOVBE_GqMq),
#endif
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_CRC32_GdEb)
};

// opcode 0F 38 F1
static const Bit64u BxOpcodeTable0F38F1[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS64 | ATTR_MOD_MEM, BX_IA_MOVBE_MqGq),
#endif
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS32 | ATTR_MOD_MEM, BX_IA_MOVBE_MdGd),
  form_opcode(ATTR_NO_SSE_PREFIX_F2_F3 | ATTR_OS16 | ATTR_MOD_MEM, BX_IA_MOVBE_MwGw),

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS64, BX_IA_CRC32_GdEq),
#endif
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS32, BX_IA_CRC32_GdEd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_OS16, BX_IA_CRC32_GdEw)
};

// opcode 0F 38 F6
#if BX_SUPPORT_CET
static const Bit64u BxOpcodeTable0F38F5[] = {
  form_opcode(ATTR_OS64    | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_WRUSSQ),
  last_opcode(ATTR_OS16_32 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_WRUSSD),
};
#endif

// opcode 0F 38 F6
static const Bit64u BxOpcodeTable0F38F6[] = {
#if BX_SUPPORT_CET
  form_opcode(ATTR_OS64    | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_WRSSQ),
  form_opcode(ATTR_OS16_32 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_WRSSD),
#endif

#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_OS64, BX_IA_ADCX_GqEq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_OS64, BX_IA_ADOX_GqEq),
#endif
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_ADCX_GdEd),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_ADOX_GdEd)
};

// opcode 0F 38 F8
static const Bit64u BxOpcodeTable0F38F8[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_MOVDIR64B_GqMdq),
#endif
  last_opcode(            ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_MOVDIR64B_GdMdq)
};

// opcode 0F 38 F9
static const Bit64u BxOpcodeTable0F38F9[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_MOVDIRI_MqGq),
#endif
  last_opcode(            ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_MOVDIRI_MdGd)
};

// opcode 0F 38 FC
static const Bit64u BxOpcodeTable0F38FC[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_AADD_EqGq),
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_AAND_EqGq),
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_AOR_EqGq),
  form_opcode(ATTR_OS64 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_AXOR_EqGq),
#endif
  form_opcode(            ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_AADD_EdGd),
  form_opcode(            ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_AAND_EdGd),
  form_opcode(            ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_AOR_EdGd),
  last_opcode(            ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_AXOR_EdGd)
};

/* ************************************************************************ */

const Bit64u *BxOpcodeTable0F38[256] = {
  // 3-byte opcode 0x0F 0x38
  /* 0F 38 00 */ BxOpcodeTable0F3800,
  /* 0F 38 01 */ BxOpcodeTable0F3801,
  /* 0F 38 02 */ BxOpcodeTable0F3802,
  /* 0F 38 03 */ BxOpcodeTable0F3803,
  /* 0F 38 04 */ BxOpcodeTable0F3804,
  /* 0F 38 05 */ BxOpcodeTable0F3805,
  /* 0F 38 06 */ BxOpcodeTable0F3806,
  /* 0F 38 07 */ BxOpcodeTable0F3807,
  /* 0F 38 08 */ BxOpcodeTable0F3808,
  /* 0F 38 09 */ BxOpcodeTable0F3809,
  /* 0F 38 0A */ BxOpcodeTable0F380A,
  /* 0F 38 0B */ BxOpcodeTable0F380B,
  /* 0F 38 0C */ BxOpcodeGroup_ERR,
  /* 0F 38 0D */ BxOpcodeGroup_ERR,
  /* 0F 38 0E */ BxOpcodeGroup_ERR,
  /* 0F 38 0F */ BxOpcodeGroup_ERR,
  /* 0F 38 10 */ BxOpcodeTable0F3810,
  /* 0F 38 11 */ BxOpcodeGroup_ERR,
  /* 0F 38 12 */ BxOpcodeGroup_ERR,
  /* 0F 38 13 */ BxOpcodeGroup_ERR,
  /* 0F 38 14 */ BxOpcodeTable0F3814,
  /* 0F 38 15 */ BxOpcodeTable0F3815,
  /* 0F 38 16 */ BxOpcodeGroup_ERR,
  /* 0F 38 17 */ BxOpcodeTable0F3817,
  /* 0F 38 18 */ BxOpcodeGroup_ERR,
  /* 0F 38 19 */ BxOpcodeGroup_ERR,
  /* 0F 38 1A */ BxOpcodeGroup_ERR,
  /* 0F 38 1B */ BxOpcodeGroup_ERR,
  /* 0F 38 1C */ BxOpcodeTable0F381C,
  /* 0F 38 1D */ BxOpcodeTable0F381D,
  /* 0F 38 1E */ BxOpcodeTable0F381E,
  /* 0F 38 1F */ BxOpcodeGroup_ERR,
  /* 0F 38 20 */ BxOpcodeTable0F3820,
  /* 0F 38 21 */ BxOpcodeTable0F3821,
  /* 0F 38 22 */ BxOpcodeTable0F3822,
  /* 0F 38 23 */ BxOpcodeTable0F3823,
  /* 0F 38 24 */ BxOpcodeTable0F3824,
  /* 0F 38 25 */ BxOpcodeTable0F3825,
  /* 0F 38 26 */ BxOpcodeGroup_ERR,
  /* 0F 38 27 */ BxOpcodeGroup_ERR,
  /* 0F 38 28 */ BxOpcodeTable0F3828,
  /* 0F 38 29 */ BxOpcodeTable0F3829,
  /* 0F 38 2A */ BxOpcodeTable0F382A,
  /* 0F 38 2B */ BxOpcodeTable0F382B,
  /* 0F 38 2C */ BxOpcodeGroup_ERR,
  /* 0F 38 2D */ BxOpcodeGroup_ERR,
  /* 0F 38 2E */ BxOpcodeGroup_ERR,
  /* 0F 38 2F */ BxOpcodeGroup_ERR,
  /* 0F 38 30 */ BxOpcodeTable0F3830,
  /* 0F 38 31 */ BxOpcodeTable0F3831,
  /* 0F 38 32 */ BxOpcodeTable0F3832,
  /* 0F 38 33 */ BxOpcodeTable0F3833,
  /* 0F 38 34 */ BxOpcodeTable0F3834,
  /* 0F 38 35 */ BxOpcodeTable0F3835,
  /* 0F 38 36 */ BxOpcodeGroup_ERR,
  /* 0F 38 37 */ BxOpcodeTable0F3837,
  /* 0F 38 38 */ BxOpcodeTable0F3838,
  /* 0F 38 39 */ BxOpcodeTable0F3839,
  /* 0F 38 3A */ BxOpcodeTable0F383A,
  /* 0F 38 3B */ BxOpcodeTable0F383B,
  /* 0F 38 3C */ BxOpcodeTable0F383C,
  /* 0F 38 3D */ BxOpcodeTable0F383D,
  /* 0F 38 3E */ BxOpcodeTable0F383E,
  /* 0F 38 3F */ BxOpcodeTable0F383F,
  /* 0F 38 40 */ BxOpcodeTable0F3840,
  /* 0F 38 41 */ BxOpcodeTable0F3841,
  /* 0F 38 42 */ BxOpcodeGroup_ERR,
  /* 0F 38 43 */ BxOpcodeGroup_ERR,
  /* 0F 38 44 */ BxOpcodeGroup_ERR,
  /* 0F 38 45 */ BxOpcodeGroup_ERR,
  /* 0F 38 46 */ BxOpcodeGroup_ERR,
  /* 0F 38 47 */ BxOpcodeGroup_ERR,
  /* 0F 38 48 */ BxOpcodeGroup_ERR,
  /* 0F 38 49 */ BxOpcodeGroup_ERR,
  /* 0F 38 4A */ BxOpcodeGroup_ERR,
  /* 0F 38 4B */ BxOpcodeGroup_ERR,
  /* 0F 38 4C */ BxOpcodeGroup_ERR,
  /* 0F 38 4D */ BxOpcodeGroup_ERR,
  /* 0F 38 4E */ BxOpcodeGroup_ERR,
  /* 0F 38 4F */ BxOpcodeGroup_ERR,
  /* 0F 38 50 */ BxOpcodeGroup_ERR,
  /* 0F 38 51 */ BxOpcodeGroup_ERR,
  /* 0F 38 52 */ BxOpcodeGroup_ERR,
  /* 0F 38 53 */ BxOpcodeGroup_ERR,
  /* 0F 38 54 */ BxOpcodeGroup_ERR,
  /* 0F 38 55 */ BxOpcodeGroup_ERR,
  /* 0F 38 56 */ BxOpcodeGroup_ERR,
  /* 0F 38 57 */ BxOpcodeGroup_ERR,
  /* 0F 38 58 */ BxOpcodeGroup_ERR,
  /* 0F 38 59 */ BxOpcodeGroup_ERR,
  /* 0F 38 5A */ BxOpcodeGroup_ERR,
  /* 0F 38 5B */ BxOpcodeGroup_ERR,
  /* 0F 38 5C */ BxOpcodeGroup_ERR,
  /* 0F 38 5D */ BxOpcodeGroup_ERR,
  /* 0F 38 5E */ BxOpcodeGroup_ERR,
  /* 0F 38 5F */ BxOpcodeGroup_ERR,
  /* 0F 38 60 */ BxOpcodeGroup_ERR,
  /* 0F 38 61 */ BxOpcodeGroup_ERR,
  /* 0F 38 62 */ BxOpcodeGroup_ERR,
  /* 0F 38 63 */ BxOpcodeGroup_ERR,
  /* 0F 38 64 */ BxOpcodeGroup_ERR,
  /* 0F 38 65 */ BxOpcodeGroup_ERR,
  /* 0F 38 66 */ BxOpcodeGroup_ERR,
  /* 0F 38 67 */ BxOpcodeGroup_ERR,
  /* 0F 38 68 */ BxOpcodeGroup_ERR,
  /* 0F 38 69 */ BxOpcodeGroup_ERR,
  /* 0F 38 6A */ BxOpcodeGroup_ERR,
  /* 0F 38 6B */ BxOpcodeGroup_ERR,
  /* 0F 38 6C */ BxOpcodeGroup_ERR,
  /* 0F 38 6D */ BxOpcodeGroup_ERR,
  /* 0F 38 6E */ BxOpcodeGroup_ERR,
  /* 0F 38 6F */ BxOpcodeGroup_ERR,
  /* 0F 38 70 */ BxOpcodeGroup_ERR,
  /* 0F 38 71 */ BxOpcodeGroup_ERR,
  /* 0F 38 72 */ BxOpcodeGroup_ERR,
  /* 0F 38 73 */ BxOpcodeGroup_ERR,
  /* 0F 38 74 */ BxOpcodeGroup_ERR,
  /* 0F 38 75 */ BxOpcodeGroup_ERR,
  /* 0F 38 76 */ BxOpcodeGroup_ERR,
  /* 0F 38 77 */ BxOpcodeGroup_ERR,
  /* 0F 38 78 */ BxOpcodeGroup_ERR,
  /* 0F 38 79 */ BxOpcodeGroup_ERR,
  /* 0F 38 7A */ BxOpcodeGroup_ERR,
  /* 0F 38 7B */ BxOpcodeGroup_ERR,
  /* 0F 38 7C */ BxOpcodeGroup_ERR,
  /* 0F 38 7D */ BxOpcodeGroup_ERR,
  /* 0F 38 7E */ BxOpcodeGroup_ERR,
  /* 0F 38 7F */ BxOpcodeGroup_ERR,
  /* 0F 38 80 */ BxOpcodeTable0F3880,
  /* 0F 38 81 */ BxOpcodeTable0F3881,
  /* 0F 38 82 */ BxOpcodeTable0F3882,
  /* 0F 38 83 */ BxOpcodeGroup_ERR,
  /* 0F 38 84 */ BxOpcodeGroup_ERR,
  /* 0F 38 85 */ BxOpcodeGroup_ERR,
  /* 0F 38 86 */ BxOpcodeGroup_ERR,
  /* 0F 38 87 */ BxOpcodeGroup_ERR,
  /* 0F 38 88 */ BxOpcodeGroup_ERR,
  /* 0F 38 89 */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_X86_64
  /* 0F 38 8A */ BxOpcodeTable0F388A,
  /* 0F 38 8B */ BxOpcodeTable0F388B,
#else
  /* 0F 38 8A */ BxOpcodeGroup_ERR,
  /* 0F 38 8B */ BxOpcodeGroup_ERR,
#endif
  /* 0F 38 8C */ BxOpcodeGroup_ERR,
  /* 0F 38 8D */ BxOpcodeGroup_ERR,
  /* 0F 38 8E */ BxOpcodeGroup_ERR,
  /* 0F 38 8F */ BxOpcodeGroup_ERR,
  /* 0F 38 90 */ BxOpcodeGroup_ERR,
  /* 0F 38 91 */ BxOpcodeGroup_ERR,
  /* 0F 38 92 */ BxOpcodeGroup_ERR,
  /* 0F 38 93 */ BxOpcodeGroup_ERR,
  /* 0F 38 94 */ BxOpcodeGroup_ERR,
  /* 0F 38 95 */ BxOpcodeGroup_ERR,
  /* 0F 38 96 */ BxOpcodeGroup_ERR,
  /* 0F 38 97 */ BxOpcodeGroup_ERR,
  /* 0F 38 98 */ BxOpcodeGroup_ERR,
  /* 0F 38 99 */ BxOpcodeGroup_ERR,
  /* 0F 38 9A */ BxOpcodeGroup_ERR,
  /* 0F 38 9B */ BxOpcodeGroup_ERR,
  /* 0F 38 9C */ BxOpcodeGroup_ERR,
  /* 0F 38 9D */ BxOpcodeGroup_ERR,
  /* 0F 38 9E */ BxOpcodeGroup_ERR,
  /* 0F 38 9F */ BxOpcodeGroup_ERR,
  /* 0F 38 A0 */ BxOpcodeGroup_ERR,
  /* 0F 38 A1 */ BxOpcodeGroup_ERR,
  /* 0F 38 A2 */ BxOpcodeGroup_ERR,
  /* 0F 38 A3 */ BxOpcodeGroup_ERR,
  /* 0F 38 A4 */ BxOpcodeGroup_ERR,
  /* 0F 38 A5 */ BxOpcodeGroup_ERR,
  /* 0F 38 A6 */ BxOpcodeGroup_ERR,
  /* 0F 38 A7 */ BxOpcodeGroup_ERR,
  /* 0F 38 A8 */ BxOpcodeGroup_ERR,
  /* 0F 38 A9 */ BxOpcodeGroup_ERR,
  /* 0F 38 AA */ BxOpcodeGroup_ERR,
  /* 0F 38 AB */ BxOpcodeGroup_ERR,
  /* 0F 38 AC */ BxOpcodeGroup_ERR,
  /* 0F 38 AD */ BxOpcodeGroup_ERR,
  /* 0F 38 AE */ BxOpcodeGroup_ERR,
  /* 0F 38 AF */ BxOpcodeGroup_ERR,
  /* 0F 38 B0 */ BxOpcodeGroup_ERR,
  /* 0F 38 B1 */ BxOpcodeGroup_ERR,
  /* 0F 38 B2 */ BxOpcodeGroup_ERR,
  /* 0F 38 B3 */ BxOpcodeGroup_ERR,
  /* 0F 38 B4 */ BxOpcodeGroup_ERR,
  /* 0F 38 B5 */ BxOpcodeGroup_ERR,
  /* 0F 38 B6 */ BxOpcodeGroup_ERR,
  /* 0F 38 B7 */ BxOpcodeGroup_ERR,
  /* 0F 38 B8 */ BxOpcodeGroup_ERR,
  /* 0F 38 B9 */ BxOpcodeGroup_ERR,
  /* 0F 38 BA */ BxOpcodeGroup_ERR,
  /* 0F 38 BB */ BxOpcodeGroup_ERR,
  /* 0F 38 BC */ BxOpcodeGroup_ERR,
  /* 0F 38 BD */ BxOpcodeGroup_ERR,
  /* 0F 38 BE */ BxOpcodeGroup_ERR,
  /* 0F 38 BF */ BxOpcodeGroup_ERR,
  /* 0F 38 C0 */ BxOpcodeGroup_ERR,
  /* 0F 38 C1 */ BxOpcodeGroup_ERR,
  /* 0F 38 C2 */ BxOpcodeGroup_ERR,
  /* 0F 38 C3 */ BxOpcodeGroup_ERR,
  /* 0F 38 C4 */ BxOpcodeGroup_ERR,
  /* 0F 38 C5 */ BxOpcodeGroup_ERR,
  /* 0F 38 C6 */ BxOpcodeGroup_ERR,
  /* 0F 38 C7 */ BxOpcodeGroup_ERR,
  /* 0F 38 C8 */ BxOpcodeTable0F38C8,
  /* 0F 38 C9 */ BxOpcodeTable0F38C9,
  /* 0F 38 CA */ BxOpcodeTable0F38CA,
  /* 0F 38 CB */ BxOpcodeTable0F38CB,
  /* 0F 38 CC */ BxOpcodeTable0F38CC,
  /* 0F 38 CD */ BxOpcodeTable0F38CD,
  /* 0F 38 CE */ BxOpcodeGroup_ERR,
  /* 0F 38 CF */ BxOpcodeTable0F38CF,
  /* 0F 38 D0 */ BxOpcodeGroup_ERR,
  /* 0F 38 D1 */ BxOpcodeGroup_ERR,
  /* 0F 38 D2 */ BxOpcodeGroup_ERR,
  /* 0F 38 D3 */ BxOpcodeGroup_ERR,
  /* 0F 38 D4 */ BxOpcodeGroup_ERR,
  /* 0F 38 D5 */ BxOpcodeGroup_ERR,
  /* 0F 38 D6 */ BxOpcodeGroup_ERR,
  /* 0F 38 D7 */ BxOpcodeGroup_ERR,
  /* 0F 38 D8 */ BxOpcodeGroup_ERR,
  /* 0F 38 D9 */ BxOpcodeGroup_ERR,
  /* 0F 38 DA */ BxOpcodeGroup_ERR,
  /* 0F 38 DB */ BxOpcodeTable0F38DB,
  /* 0F 38 DC */ BxOpcodeTable0F38DC,
  /* 0F 38 DD */ BxOpcodeTable0F38DD,
  /* 0F 38 DE */ BxOpcodeTable0F38DE,
  /* 0F 38 DF */ BxOpcodeTable0F38DF,
  /* 0F 38 E0 */ BxOpcodeGroup_ERR,
  /* 0F 38 E1 */ BxOpcodeGroup_ERR,
  /* 0F 38 E2 */ BxOpcodeGroup_ERR,
  /* 0F 38 E3 */ BxOpcodeGroup_ERR,
  /* 0F 38 E4 */ BxOpcodeGroup_ERR,
  /* 0F 38 E5 */ BxOpcodeGroup_ERR,
  /* 0F 38 E6 */ BxOpcodeGroup_ERR,
  /* 0F 38 E7 */ BxOpcodeGroup_ERR,
  /* 0F 38 E8 */ BxOpcodeGroup_ERR,
  /* 0F 38 E9 */ BxOpcodeGroup_ERR,
  /* 0F 38 EA */ BxOpcodeGroup_ERR,
  /* 0F 38 EB */ BxOpcodeGroup_ERR,
  /* 0F 38 EC */ BxOpcodeGroup_ERR,
  /* 0F 38 ED */ BxOpcodeGroup_ERR,
  /* 0F 38 EE */ BxOpcodeGroup_ERR,
  /* 0F 38 EF */ BxOpcodeGroup_ERR,
  /* 0F 38 F0 */ BxOpcodeTable0F38F0,
  /* 0F 38 F1 */ BxOpcodeTable0F38F1,
  /* 0F 38 F2 */ BxOpcodeGroup_ERR,
  /* 0F 38 F3 */ BxOpcodeGroup_ERR,
  /* 0F 38 F4 */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_CET
  /* 0F 38 F5 */ BxOpcodeTable0F38F5,
#else
  /* 0F 38 F5 */ BxOpcodeGroup_ERR,
#endif
  /* 0F 38 F6 */ BxOpcodeTable0F38F6,
  /* 0F 38 F7 */ BxOpcodeGroup_ERR,
  /* 0F 38 F8 */ BxOpcodeTable0F38F8,
  /* 0F 38 F9 */ BxOpcodeTable0F38F9,
  /* 0F 38 FA */ BxOpcodeGroup_ERR,
  /* 0F 38 FB */ BxOpcodeGroup_ERR,
  /* 0F 38 FC */ BxOpcodeTable0F38FC,
  /* 0F 38 FD */ BxOpcodeGroup_ERR,
  /* 0F 38 FE */ BxOpcodeGroup_ERR,
  /* 0F 38 FF */ BxOpcodeGroup_ERR
};

#endif // BX_CPU_LEVEL >= 6
