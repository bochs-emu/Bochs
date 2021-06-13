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

#ifndef BX_FETCHDECODE_OPMAP_0F38_H
#define BX_FETCHDECODE_OPMAP_0F38_H

#if BX_CPU_LEVEL >= 6

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

#endif // BX_CPU_LEVEL >= 6

#endif // BX_FETCHDECODE_OPMAP_0F38_H
