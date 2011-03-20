/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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

#ifndef BX_AVX_FETCHDECODE_TABLES_H
#define BX_AVX_FETCHDECODE_TABLES_H

#if BX_SUPPORT_AVX

/* ************************************************************************ */

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2a[2] = {
  /* 0 */ { 0, BX_IA_VCVTSI2SS_VssEd },
  /* 1 */ { 0, BX_IA_VCVTSI2SS_VssEq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2a[2] = {
  /* 0 */ { 0, BX_IA_VCVTSI2SD_VsdEd },
  /* 1 */ { 0, BX_IA_VCVTSI2SD_VsdEq }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2c[2] = {
  /* 0 */ { 0, BX_IA_VCVTTSS2SI_GdWss },
  /* 1 */ { 0, BX_IA_VCVTTSS2SI_GqWss }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2c[2] = {
  /* 0 */ { 0, BX_IA_VCVTTSD2SI_GdWsd },
  /* 1 */ { 0, BX_IA_VCVTTSD2SI_GqWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f30f2d[2] = {
  /* 0 */ { 0, BX_IA_VCVTSS2SI_GdWss },
  /* 1 */ { 0, BX_IA_VCVTSS2SI_GqWss }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_f20f2d[2] = {
  /* 0 */ { 0, BX_IA_VCVTSD2SI_GdWsd },
  /* 1 */ { 0, BX_IA_VCVTSD2SI_GqWsd }
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f6e[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VMOVD_VdqEd },
  /* 1 */ { BxPrefixSSE66, BX_IA_VMOVQ_VdqEq },
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_660f7e[2] = {
  /* 0 */ { 0, BX_IA_VMOVD_EdVd },
  /* 1 */ { 0, BX_IA_VMOVQ_EqVq },
};

static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3a16[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VPEXTRD_EdVdqIb },
  /* 1 */ { BxPrefixSSE66, BX_IA_VPEXTRQ_EqVdqIb }
};


static const BxOpcodeInfo_t BxOpcodeInfoAVX_VexW_0f3a22[2] = {
  /* 0 */ { BxPrefixSSE66, BX_IA_VPINSRD_VdqEdIb },
  /* 1 */ { BxPrefixSSE66, BX_IA_VPINSRQ_VdqEqIb }
};

/* ************************************************************************ */

/* ********** */
/* SSE Groups */
/* ********** */

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f10[6] = {
  /* 66 */ { 0, BX_IA_VMOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VMOVSS_VssWss },
  /* F2 */ { 0, BX_IA_VMOVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f11[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_VMOVUPD_WpdVpd },
  /* F3 */ { BxArithDstRM, BX_IA_VMOVSS_WssVss },
  /* F2 */ { BxArithDstRM, BX_IA_VMOVSD_WsdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f12[6] = {
  /* 66 */ { 0, BX_IA_VMOVLPD_VpdMq },
  /* F3 */ { 0, BX_IA_VMOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_VMOVDDUP_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f13M[6] = {
  /* 66 */ { 0, BX_IA_VMOVLPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f14[6] = {
  /* 66 */ { 0, BX_IA_VUNPCKLPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f15[6] = {
  /* 66 */ { 0, BX_IA_VUNPCKHPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f16[6] = {
  /* 66 */ { 0, BX_IA_VMOVHPD_VpdMq },
  /* F3 */ { 0, BX_IA_VMOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f17M[6] = {
  /* 66 */ { 0, BX_IA_VMOVHPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f28[6] = {
  /* 66 */ { 0, BX_IA_VMOVAPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f29[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_VMOVAPD_WpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2a[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2a },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2a }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2bM[6] = {
  /* 66 */ { 0, BX_IA_VMOVNTPD_MpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2c[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2c },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2c }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2d[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f30f2d },
  /* F2 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_f20f2d }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2e[6] = {
  /* 66 */ { 0, BX_IA_VUCOMISD_VsdWsd },            	
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f2f[6] = {
  /* 66 */ { 0, BX_IA_VCOMISD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f50R[6] = {
  /* 66 */ { 0, BX_IA_VMOVMSKPD_GdVRpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f51[6] = {
  /* 66 */ { 0, BX_IA_VSQRTPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VSQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_VSQRTSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f52[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VRSQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f53[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VRCPSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f54[6] = {
  /* 66 */ { 0, BX_IA_VANDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f55[6] = {
  /* 66 */ { 0, BX_IA_VANDNPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f56[6] = {
  /* 66 */ { 0, BX_IA_VORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f57[6] = {
  /* 66 */ { 0, BX_IA_VXORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f58[6] = {
  /* 66 */ { 0, BX_IA_VADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VADDSS_VssWss },
  /* F2 */ { 0, BX_IA_VADDSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f59[6] = {
  /* 66 */ { 0, BX_IA_VMULPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VMULSS_VssWss },
  /* F2 */ { 0, BX_IA_VMULSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5a[6] = {
  /* 66 */ { 0, BX_IA_VCVTPD2PS_VpsWpd },
  /* F3 */ { 0, BX_IA_VCVTSS2SD_VsdWss },
  /* F2 */ { 0, BX_IA_VCVTSD2SS_VssWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5b[6] = {
  /* 66 */ { 0, BX_IA_VCVTPS2DQ_VdqWps },
  /* F3 */ { 0, BX_IA_VCVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5c[6] = {
  /* 66 */ { 0, BX_IA_VSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VSUBSS_VssWss },
  /* F2 */ { 0, BX_IA_VSUBSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5d[6] = {
  /* 66 */ { 0, BX_IA_VMINPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VMINSS_VssWss },
  /* F2 */ { 0, BX_IA_VMINSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5e[6] = {
  /* 66 */ { 0, BX_IA_VDIVPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VDIVSS_VssWss },
  /* F2 */ { 0, BX_IA_VDIVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f5f[6] = {
  /* 66 */ { 0, BX_IA_VMAXPD_VpdWpd },
  /* F3 */ { 0, BX_IA_VMAXSS_VssWss },
  /* F2 */ { 0, BX_IA_VMAXSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f6f[6] = {
  /* 66 */ { 0, BX_IA_VMOVDQA_VdqWdq },
  /* F3 */ { 0, BX_IA_VMOVDQU_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f70[6] = {
  /* 66 */ { 0, BX_IA_VPSHUFD_VdqWdqIb  },
  /* F3 */ { 0, BX_IA_VPSHUFHW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_VPSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7c[6] = {
  /* 66 */ { 0, BX_IA_VHADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VHADDPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7d[6] = {
  /* 66 */ { 0, BX_IA_VHSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VHSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7e[6] = {
  /* 66 */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_660f7e },
  /* F3 */ { 0, BX_IA_VMOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0f7f[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_VMOVDQA_WdqVdq },
  /* F3 */ { BxArithDstRM, BX_IA_VMOVDQU_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fc2[6] = {
  /* 66 */ { 0, BX_IA_VCMPPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_VCMPSS_VssWssIb },
  /* F2 */ { 0, BX_IA_VCMPSD_VsdWsdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fc6[6] = {
  /* 66 */ { 0, BX_IA_VSHUFPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fd0[6] = {
  /* 66 */ { 0, BX_IA_VADDSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_VADDSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupAVX_0fe6[6] = {
  /* 66 */ { 0, BX_IA_VCVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_VCVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_VCVTPD2DQ_VqWpd }
};

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVXG12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRLW_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRAW_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSLLW_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVXG13R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRLD_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRAD_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSLLD_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVXG14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRLQ_UdqIb },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSLLQ_UdqIb },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_VPSLLDQ_UdqIb }
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoAVXG15M[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxPrefixSSE, BX_IA_VLDMXCSR, BxOpcodeGroupSSE_ERR },
  /* 3 */ { BxPrefixSSE, BX_IA_VSTMXCSR, BxOpcodeGroupSSE_ERR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */

static const BxOpcodeInfo_t BxOpcodeTableAVX[256*3] = {
  // 256 entries for VEX-encoded 0x0F opcodes
  /* 00 */ { 0, BX_IA_ERROR },
  /* 01 */ { 0, BX_IA_ERROR },
  /* 02 */ { 0, BX_IA_ERROR },
  /* 03 */ { 0, BX_IA_ERROR },
  /* 04 */ { 0, BX_IA_ERROR },
  /* 05 */ { 0, BX_IA_ERROR },
  /* 06 */ { 0, BX_IA_ERROR },
  /* 07 */ { 0, BX_IA_ERROR },
  /* 08 */ { 0, BX_IA_ERROR },
  /* 09 */ { 0, BX_IA_ERROR },
  /* 0A */ { 0, BX_IA_ERROR },
  /* 0B */ { 0, BX_IA_ERROR },
  /* 0C */ { 0, BX_IA_ERROR },
  /* 0D */ { 0, BX_IA_ERROR },
  /* 0E */ { 0, BX_IA_ERROR },
  /* 0F */ { 0, BX_IA_ERROR },
  /* 10 */ { BxPrefixSSE, BX_IA_VMOVUPS_VpsWps, BxOpcodeGroupAVX_0f10 },
  /* 11 */ { BxPrefixSSE | BxArithDstRM, BX_IA_VMOVUPS_WpsVps, BxOpcodeGroupAVX_0f11 },
  /* 12 */ { BxPrefixSSE, BX_IA_VMOVLPS_VpsMq, BxOpcodeGroupAVX_0f12 },
  /* 13 */ { BxPrefixSSE, BX_IA_VMOVLPS_MqVps, BxOpcodeGroupAVX_0f13M },
  /* 14 */ { BxPrefixSSE, BX_IA_VUNPCKLPS_VpsWps, BxOpcodeGroupAVX_0f14 },
  /* 15 */ { BxPrefixSSE, BX_IA_VUNPCKHPS_VpsWps, BxOpcodeGroupAVX_0f15 },
  /* 16 */ { BxPrefixSSE, BX_IA_VMOVHPS_VpsMq, BxOpcodeGroupAVX_0f16 },
  /* 17 */ { BxPrefixSSE, BX_IA_VMOVHPS_MqVps, BxOpcodeGroupAVX_0f17M },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { 0, BX_IA_ERROR },
  /* 1D */ { 0, BX_IA_ERROR },
  /* 1E */ { 0, BX_IA_ERROR },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { 0, BX_IA_ERROR },
  /* 21 */ { 0, BX_IA_ERROR },
  /* 22 */ { 0, BX_IA_ERROR },
  /* 23 */ { 0, BX_IA_ERROR },
  /* 24 */ { 0, BX_IA_ERROR },
  /* 25 */ { 0, BX_IA_ERROR },
  /* 26 */ { 0, BX_IA_ERROR },
  /* 27 */ { 0, BX_IA_ERROR },
  /* 28 */ { BxPrefixSSE, BX_IA_VMOVAPS_VpsWps, BxOpcodeGroupAVX_0f28 },
  /* 29 */ { BxPrefixSSE | BxArithDstRM, BX_IA_VMOVAPS_WpsVps, BxOpcodeGroupAVX_0f29 },
  /* 2A */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2a },
  /* 2B */ { BxPrefixSSE, BX_IA_VMOVNTPS_MpsVps, BxOpcodeGroupAVX_0f2bM },
  /* 2C */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2c },
  /* 2D */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f2d },
  /* 2E */ { BxPrefixSSE, BX_IA_VUCOMISS_VssWss, BxOpcodeGroupAVX_0f2e },
  /* 2F */ { BxPrefixSSE, BX_IA_VCOMISS_VpsWps, BxOpcodeGroupAVX_0f2f },
  /* 30 */ { 0, BX_IA_ERROR },
  /* 31 */ { 0, BX_IA_ERROR },
  /* 32 */ { 0, BX_IA_ERROR },
  /* 33 */ { 0, BX_IA_ERROR },
  /* 34 */ { 0, BX_IA_ERROR },
  /* 35 */ { 0, BX_IA_ERROR },
  /* 36 */ { 0, BX_IA_ERROR },
  /* 37 */ { 0, BX_IA_ERROR },
  /* 38 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 39 */ { 0, BX_IA_ERROR },
  /* 3A */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 3B */ { 0, BX_IA_ERROR },
  /* 3C */ { 0, BX_IA_ERROR },
  /* 3D */ { 0, BX_IA_ERROR },
  /* 3E */ { 0, BX_IA_ERROR },
  /* 3F */ { 0, BX_IA_ERROR },
  /* 40 */ { 0, BX_IA_ERROR },
  /* 41 */ { 0, BX_IA_ERROR },
  /* 42 */ { 0, BX_IA_ERROR },
  /* 43 */ { 0, BX_IA_ERROR },
  /* 44 */ { 0, BX_IA_ERROR },
  /* 45 */ { 0, BX_IA_ERROR },
  /* 46 */ { 0, BX_IA_ERROR },
  /* 47 */ { 0, BX_IA_ERROR },
  /* 48 */ { 0, BX_IA_ERROR },
  /* 49 */ { 0, BX_IA_ERROR },
  /* 4A */ { 0, BX_IA_ERROR },
  /* 4B */ { 0, BX_IA_ERROR },
  /* 4C */ { 0, BX_IA_ERROR },
  /* 4D */ { 0, BX_IA_ERROR },
  /* 4E */ { 0, BX_IA_ERROR },
  /* 4F */ { 0, BX_IA_ERROR },
  /* 50 */ { BxPrefixSSE, BX_IA_VMOVMSKPS_GdVRps, BxOpcodeGroupAVX_0f50R },
  /* 51 */ { BxPrefixSSE, BX_IA_VSQRTPS_VpsWps, BxOpcodeGroupAVX_0f51 },
  /* 52 */ { BxPrefixSSE, BX_IA_VRSQRTPS_VpsWps, BxOpcodeGroupAVX_0f52 },
  /* 53 */ { BxPrefixSSE, BX_IA_VRCPPS_VpsWps, BxOpcodeGroupAVX_0f53 },
  /* 54 */ { BxPrefixSSE, BX_IA_VANDPS_VpsWps, BxOpcodeGroupAVX_0f54 },
  /* 55 */ { BxPrefixSSE, BX_IA_VANDNPS_VpsWps, BxOpcodeGroupAVX_0f55 },
  /* 56 */ { BxPrefixSSE, BX_IA_VORPS_VpsWps, BxOpcodeGroupAVX_0f56 },
  /* 57 */ { BxPrefixSSE, BX_IA_VXORPS_VpsWps, BxOpcodeGroupAVX_0f57 },
  /* 58 */ { BxPrefixSSE, BX_IA_VADDPS_VpsWps, BxOpcodeGroupAVX_0f58 },
  /* 59 */ { BxPrefixSSE, BX_IA_VMULPS_VpsWps, BxOpcodeGroupAVX_0f59 },
  /* 5A */ { BxPrefixSSE, BX_IA_VCVTPS2PD_VpdWps, BxOpcodeGroupAVX_0f5a },
  /* 5B */ { BxPrefixSSE, BX_IA_VCVTDQ2PS_VpsWdq, BxOpcodeGroupAVX_0f5b },
  /* 5C */ { BxPrefixSSE, BX_IA_VSUBPS_VpsWps, BxOpcodeGroupAVX_0f5c },
  /* 5D */ { BxPrefixSSE, BX_IA_VMINPS_VpsWps, BxOpcodeGroupAVX_0f5d },
  /* 5E */ { BxPrefixSSE, BX_IA_VDIVPS_VpsWps, BxOpcodeGroupAVX_0f5e },
  /* 5F */ { BxPrefixSSE, BX_IA_VMAXPS_VpsWps, BxOpcodeGroupAVX_0f5f },
  /* 60 */ { BxPrefixSSE66, BX_IA_VPUNPCKLBW_VdqWdq },
  /* 61 */ { BxPrefixSSE66, BX_IA_VPUNPCKLWD_VdqWdq },
  /* 62 */ { BxPrefixSSE66, BX_IA_VPUNPCKLDQ_VdqWdq },
  /* 63 */ { BxPrefixSSE66, BX_IA_VPACKSSWB_VdqWdq },
  /* 64 */ { BxPrefixSSE66, BX_IA_VPCMPGTB_VdqWdq },
  /* 65 */ { BxPrefixSSE66, BX_IA_VPCMPGTW_VdqWdq },
  /* 66 */ { BxPrefixSSE66, BX_IA_VPCMPGTD_VdqWdq },
  /* 67 */ { BxPrefixSSE66, BX_IA_VPACKUSWB_VdqWdq },
  /* 68 */ { BxPrefixSSE66, BX_IA_VPUNPCKHBW_VdqWdq },
  /* 69 */ { BxPrefixSSE66, BX_IA_VPUNPCKHWD_VdqWdq },
  /* 6A */ { BxPrefixSSE66, BX_IA_VPUNPCKHDQ_VdqWdq },
  /* 6B */ { BxPrefixSSE66, BX_IA_VPACKSSDW_VdqWdq },
  /* 6C */ { BxPrefixSSE66, BX_IA_VPUNPCKLQDQ_VdqWdq },
  /* 6D */ { BxPrefixSSE66, BX_IA_VPUNPCKHQDQ_VdqWdq },
  /* 6E */ { BxSplitVexW, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f6e },
  /* 6F */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f6f },
  /* 70 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeGroupAVX_0f70 },
  /* 71 */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoAVXG12R },
  /* 72 */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoAVXG13R },
  /* 73 */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoAVXG14R },
  /* 74 */ { BxPrefixSSE66, BX_IA_VPCMPEQB_VdqWdq },
  /* 75 */ { BxPrefixSSE66, BX_IA_VPCMPEQW_VdqWdq },
  /* 76 */ { BxPrefixSSE66, BX_IA_VPCMPEQD_VdqWdq },
  /* 77 */ { BxPrefixSSE, BX_IA_VZEROUPPER, BxOpcodeGroupSSE_ERR },
  /* 78 */ { 0, BX_IA_ERROR },
  /* 79 */ { 0, BX_IA_ERROR },
  /* 7A */ { 0, BX_IA_ERROR },
  /* 7B */ { 0, BX_IA_ERROR },
  /* 7C */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7c },
  /* 7D */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7d },
  /* 7E */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7e },
  /* 7F */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0f7f },
  /* 80 */ { 0, BX_IA_ERROR },
  /* 81 */ { 0, BX_IA_ERROR },
  /* 82 */ { 0, BX_IA_ERROR },
  /* 83 */ { 0, BX_IA_ERROR },
  /* 84 */ { 0, BX_IA_ERROR },
  /* 85 */ { 0, BX_IA_ERROR },
  /* 86 */ { 0, BX_IA_ERROR },
  /* 87 */ { 0, BX_IA_ERROR },
  /* 88 */ { 0, BX_IA_ERROR },
  /* 89 */ { 0, BX_IA_ERROR },
  /* 8A */ { 0, BX_IA_ERROR },
  /* 8B */ { 0, BX_IA_ERROR },
  /* 8C */ { 0, BX_IA_ERROR },
  /* 8D */ { 0, BX_IA_ERROR },
  /* 8E */ { 0, BX_IA_ERROR },
  /* 8F */ { 0, BX_IA_ERROR },
  /* 90 */ { 0, BX_IA_ERROR },
  /* 91 */ { 0, BX_IA_ERROR },
  /* 92 */ { 0, BX_IA_ERROR },
  /* 93 */ { 0, BX_IA_ERROR },
  /* 94 */ { 0, BX_IA_ERROR },
  /* 95 */ { 0, BX_IA_ERROR },
  /* 96 */ { 0, BX_IA_ERROR },
  /* 97 */ { 0, BX_IA_ERROR },
  /* 98 */ { 0, BX_IA_ERROR },
  /* 99 */ { 0, BX_IA_ERROR },
  /* 9A */ { 0, BX_IA_ERROR },
  /* 9B */ { 0, BX_IA_ERROR },
  /* 9C */ { 0, BX_IA_ERROR },
  /* 9D */ { 0, BX_IA_ERROR },
  /* 9E */ { 0, BX_IA_ERROR },
  /* 9F */ { 0, BX_IA_ERROR },
  /* A0 */ { 0, BX_IA_ERROR },
  /* A1 */ { 0, BX_IA_ERROR },
  /* A2 */ { 0, BX_IA_ERROR },
  /* A3 */ { 0, BX_IA_ERROR },
  /* A4 */ { 0, BX_IA_ERROR },
  /* A5 */ { 0, BX_IA_ERROR },
  /* A6 */ { 0, BX_IA_ERROR },
  /* A7 */ { 0, BX_IA_ERROR },
  /* A8 */ { 0, BX_IA_ERROR },
  /* A9 */ { 0, BX_IA_ERROR },
  /* AA */ { 0, BX_IA_ERROR },
  /* AB */ { 0, BX_IA_ERROR },
  /* AC */ { 0, BX_IA_ERROR },
  /* AD */ { 0, BX_IA_ERROR },
  /* AE */ { BxGroupN, BX_IA_ERROR, BxOpcodeInfoAVXG15M },
  /* AF */ { 0, BX_IA_ERROR },
  /* B0 */ { 0, BX_IA_ERROR },
  /* B1 */ { 0, BX_IA_ERROR },
  /* B2 */ { 0, BX_IA_ERROR },
  /* B3 */ { 0, BX_IA_ERROR },
  /* B4 */ { 0, BX_IA_ERROR },
  /* B5 */ { 0, BX_IA_ERROR },
  /* B6 */ { 0, BX_IA_ERROR },
  /* B7 */ { 0, BX_IA_ERROR },
  /* B8 */ { 0, BX_IA_ERROR },
  /* B9 */ { 0, BX_IA_ERROR },
  /* BA */ { 0, BX_IA_ERROR },
  /* BB */ { 0, BX_IA_ERROR },
  /* BC */ { 0, BX_IA_ERROR },
  /* BD */ { 0, BX_IA_ERROR },
  /* BE */ { 0, BX_IA_ERROR },
  /* BF */ { 0, BX_IA_ERROR },
  /* C0 */ { 0, BX_IA_ERROR },
  /* C1 */ { 0, BX_IA_ERROR },
  /* C2 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VCMPPS_VpsWpsIb, BxOpcodeGroupAVX_0fc2 },
  /* C3 */ { 0, BX_IA_ERROR },
  /* C4 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPINSRW_VdqEwIb },
  /* C5 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPEXTRW_GdUdqIb },
  /* C6 */ { BxPrefixSSE | BxImmediate_Ib, BX_IA_VSHUFPS_VpsWpsIb, BxOpcodeGroupAVX_0fc6 },
  /* C7 */ { 0, BX_IA_ERROR },
  /* C8 */ { 0, BX_IA_ERROR },
  /* C9 */ { 0, BX_IA_ERROR },
  /* CA */ { 0, BX_IA_ERROR },
  /* CB */ { 0, BX_IA_ERROR },
  /* CC */ { 0, BX_IA_ERROR },
  /* CD */ { 0, BX_IA_ERROR },
  /* CE */ { 0, BX_IA_ERROR },
  /* CF */ { 0, BX_IA_ERROR },
  /* D0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0fd0 },
  /* D1 */ { BxPrefixSSE66, BX_IA_VPSRLW_VdqWdq },
  /* D2 */ { BxPrefixSSE66, BX_IA_VPSRLD_VdqWdq },
  /* D3 */ { BxPrefixSSE66, BX_IA_VPSRLQ_VdqWdq },
  /* D4 */ { BxPrefixSSE66, BX_IA_VPADDQ_VdqWdq },
  /* D5 */ { BxPrefixSSE66, BX_IA_VPMULLW_VdqWdq },
  /* D6 */ { BxPrefixSSE66 | BxArithDstRM, BX_IA_VMOVQ_WqVq },
  /* D7 */ { BxPrefixSSE66, BX_IA_VPMOVMSKB_GdUdq },
  /* D8 */ { BxPrefixSSE66, BX_IA_VPSUBUSB_VdqWdq },
  /* D9 */ { BxPrefixSSE66, BX_IA_VPSUBUSW_VdqWdq },
  /* DA */ { BxPrefixSSE66, BX_IA_VPMINUB_VdqWdq },
  /* DB */ { BxPrefixSSE66, BX_IA_VPAND_VdqWdq },
  /* DC */ { BxPrefixSSE66, BX_IA_VPADDUSB_VdqWdq },
  /* DD */ { BxPrefixSSE66, BX_IA_VPADDUSW_VdqWdq },
  /* DE */ { BxPrefixSSE66, BX_IA_VPMAXUB_VdqWdq },
  /* DF */ { BxPrefixSSE66, BX_IA_VPANDN_VdqWdq },
  /* E0 */ { BxPrefixSSE66, BX_IA_VPAVGB_VdqWdq },
  /* E1 */ { BxPrefixSSE66, BX_IA_VPSRAW_VdqWdq },
  /* E2 */ { BxPrefixSSE66, BX_IA_VPSRAD_VdqWdq },
  /* E3 */ { BxPrefixSSE66, BX_IA_VPAVGW_VdqWdq },
  /* E4 */ { BxPrefixSSE66, BX_IA_VPMULHUW_VdqWdq },
  /* E5 */ { BxPrefixSSE66, BX_IA_VPMULHW_VdqWdq },
  /* E6 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupAVX_0fe6 },
  /* E7 */ { BxPrefixSSE66, BX_IA_VMOVNTDQ_MdqVdq },
  /* E8 */ { BxPrefixSSE66, BX_IA_VPSUBSB_VdqWdq },
  /* E9 */ { BxPrefixSSE66, BX_IA_VPSUBSW_VdqWdq },
  /* EA */ { BxPrefixSSE66, BX_IA_VPMINSW_VdqWdq },
  /* EB */ { BxPrefixSSE66, BX_IA_VPOR_VdqWdq },
  /* EC */ { BxPrefixSSE66, BX_IA_VPADDSB_VdqWdq },
  /* ED */ { BxPrefixSSE66, BX_IA_VPADDSW_VdqWdq },
  /* EE */ { BxPrefixSSE66, BX_IA_VPMAXSW_VdqWdq },
  /* EF */ { BxPrefixSSE66, BX_IA_VPXOR_VdqWdq },
  /* F0 */ { BxPrefixSSEF2, BX_IA_VLDDQU_VdqMdq },
  /* F1 */ { BxPrefixSSE66, BX_IA_VPSLLW_VdqWdq },
  /* F2 */ { BxPrefixSSE66, BX_IA_VPSLLD_VdqWdq },
  /* F3 */ { BxPrefixSSE66, BX_IA_VPSLLQ_VdqWdq },
  /* F4 */ { BxPrefixSSE66, BX_IA_VPMULUDQ_VdqWdq },
  /* F5 */ { BxPrefixSSE66, BX_IA_VPMADDWD_VdqWdq },
  /* F6 */ { BxPrefixSSE66, BX_IA_VPSADBW_VdqWdq },
  /* F7 */ { BxPrefixSSE66, BX_IA_VMASKMOVDQU_VdqUdq },
  /* F8 */ { BxPrefixSSE66, BX_IA_VPSUBB_VdqWdq },
  /* F9 */ { BxPrefixSSE66, BX_IA_VPSUBW_VdqWdq },
  /* FA */ { BxPrefixSSE66, BX_IA_VPSUBD_VdqWdq },
  /* FB */ { BxPrefixSSE66, BX_IA_VPSUBQ_VdqWdq },
  /* FC */ { BxPrefixSSE66, BX_IA_VPADDB_VdqWdq },
  /* FD */ { BxPrefixSSE66, BX_IA_VPADDW_VdqWdq },
  /* FE */ { BxPrefixSSE66, BX_IA_VPADDD_VdqWdq },
  /* FF */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x38 opcodes
  /* 00 */ { BxPrefixSSE66, BX_IA_VPSHUFB_VdqWdq },
  /* 01 */ { BxPrefixSSE66, BX_IA_VPHADDW_VdqWdq },
  /* 02 */ { BxPrefixSSE66, BX_IA_VPHADDD_VdqWdq },
  /* 03 */ { BxPrefixSSE66, BX_IA_VPHADDSW_VdqWdq },
  /* 04 */ { BxPrefixSSE66, BX_IA_VPMADDUBSW_VdqWdq },
  /* 05 */ { BxPrefixSSE66, BX_IA_VPHSUBW_VdqWdq },
  /* 06 */ { BxPrefixSSE66, BX_IA_VPHSUBD_VdqWdq },
  /* 07 */ { BxPrefixSSE66, BX_IA_VPHSUBSW_VdqWdq },
  /* 08 */ { BxPrefixSSE66, BX_IA_VPSIGNB_VdqWdq },
  /* 09 */ { BxPrefixSSE66, BX_IA_VPSIGNW_VdqWdq },
  /* 0A */ { BxPrefixSSE66, BX_IA_VPSIGND_VdqWdq },
  /* 0B */ { BxPrefixSSE66, BX_IA_VPMULHRSW_VdqWdq },
  /* 0C */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPS_VpsWps },
  /* 0D */ { BxPrefixSSE66 | BxVexW0, BX_IA_VPERMILPD_VpdWpd },
  /* 0E */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPS_VpsWps },
  /* 0F */ { BxPrefixSSE66 | BxVexW0, BX_IA_VTESTPD_VpdWpd },
  /* 10 */ { 0, BX_IA_ERROR },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { 0, BX_IA_ERROR },
  /* 15 */ { 0, BX_IA_ERROR },
  /* 16 */ { 0, BX_IA_ERROR },
  /* 17 */ { BxPrefixSSE66, BX_IA_VPTEST_VdqWdq },
  /* 18 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VBROADCASTSS_VpsMss },
  /* 19 */ { BxPrefixSSE66 | BxVexW0, BX_IA_VBROADCASTSD_VpdMsd },
  /* 1A */ { BxPrefixSSE66 | BxVexW0, BX_IA_VBROADCASTF128_VdqMdq },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { BxPrefixSSE66, BX_IA_VPABSB_VdqWdq },
  /* 1D */ { BxPrefixSSE66, BX_IA_VPABSW_VdqWdq },
  /* 1E */ { BxPrefixSSE66, BX_IA_VPABSD_VdqWdq },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxPrefixSSE66, BX_IA_VPMOVSXBW_VdqWq },
  /* 21 */ { BxPrefixSSE66, BX_IA_VPMOVSXBD_VdqWd },
  /* 22 */ { BxPrefixSSE66, BX_IA_VPMOVSXBQ_VdqWw },
  /* 23 */ { BxPrefixSSE66, BX_IA_VPMOVSXWD_VdqWq },
  /* 24 */ { BxPrefixSSE66, BX_IA_VPMOVSXWQ_VdqWd },
  /* 25 */ { BxPrefixSSE66, BX_IA_VPMOVSXDQ_VdqWq },
  /* 26 */ { 0, BX_IA_ERROR },
  /* 27 */ { 0, BX_IA_ERROR },
  /* 28 */ { BxPrefixSSE66, BX_IA_VPMULDQ_VdqWdq },
  /* 29 */ { BxPrefixSSE66, BX_IA_VPCMPEQQ_VdqWdq },
  /* 2A */ { BxPrefixSSE66, BX_IA_VMOVNTDQA_VdqMdq },
  /* 2B */ { BxPrefixSSE66, BX_IA_VPACKUSDW_VdqWdq },
  /* 2C */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_VpsMps },
  /* 2D */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_VpdMpd },
  /* 2E */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPS_MpsVps },
  /* 2F */ { BxPrefixSSE66 | BxVexW0, BX_IA_VMASKMOVPD_MpdVpd },
  /* 30 */ { BxPrefixSSE66, BX_IA_VPMOVZXBW_VdqWq },
  /* 31 */ { BxPrefixSSE66, BX_IA_VPMOVZXBD_VdqWd },
  /* 32 */ { BxPrefixSSE66, BX_IA_VPMOVZXBQ_VdqWw },
  /* 33 */ { BxPrefixSSE66, BX_IA_VPMOVZXWD_VdqWq },
  /* 34 */ { BxPrefixSSE66, BX_IA_VPMOVZXWQ_VdqWd },
  /* 35 */ { BxPrefixSSE66, BX_IA_VPMOVZXDQ_VdqWq },
  /* 36 */ { 0, BX_IA_ERROR },
  /* 37 */ { BxPrefixSSE66, BX_IA_VPCMPGTQ_VdqWdq },
  /* 38 */ { BxPrefixSSE66, BX_IA_VPMINSB_VdqWdq },
  /* 39 */ { BxPrefixSSE66, BX_IA_VPMINSD_VdqWdq },
  /* 3A */ { BxPrefixSSE66, BX_IA_VPMINUW_VdqWdq },
  /* 3B */ { BxPrefixSSE66, BX_IA_VPMINUD_VdqWdq },
  /* 3C */ { BxPrefixSSE66, BX_IA_VPMAXSB_VdqWdq },
  /* 3D */ { BxPrefixSSE66, BX_IA_VPMAXSD_VdqWdq },
  /* 3E */ { BxPrefixSSE66, BX_IA_VPMAXUW_VdqWdq },
  /* 3F */ { BxPrefixSSE66, BX_IA_VPMAXUD_VdqWdq },
  /* 40 */ { BxPrefixSSE66, BX_IA_VPMULLD_VdqWdq },
  /* 41 */ { BxPrefixSSE66, BX_IA_VPHMINPOSUW_VdqWdq },
  /* 42 */ { 0, BX_IA_ERROR },
  /* 43 */ { 0, BX_IA_ERROR },
  /* 44 */ { 0, BX_IA_ERROR },
  /* 45 */ { 0, BX_IA_ERROR },
  /* 46 */ { 0, BX_IA_ERROR },
  /* 47 */ { 0, BX_IA_ERROR },
  /* 48 */ { 0, BX_IA_ERROR },
  /* 49 */ { 0, BX_IA_ERROR },
  /* 4A */ { 0, BX_IA_ERROR },
  /* 4B */ { 0, BX_IA_ERROR },
  /* 4C */ { 0, BX_IA_ERROR },
  /* 4D */ { 0, BX_IA_ERROR },
  /* 4E */ { 0, BX_IA_ERROR },
  /* 4F */ { 0, BX_IA_ERROR },
  /* 50 */ { 0, BX_IA_ERROR },
  /* 51 */ { 0, BX_IA_ERROR },
  /* 52 */ { 0, BX_IA_ERROR },
  /* 53 */ { 0, BX_IA_ERROR },
  /* 54 */ { 0, BX_IA_ERROR },
  /* 55 */ { 0, BX_IA_ERROR },
  /* 56 */ { 0, BX_IA_ERROR },
  /* 57 */ { 0, BX_IA_ERROR },
  /* 58 */ { 0, BX_IA_ERROR },
  /* 59 */ { 0, BX_IA_ERROR },
  /* 5A */ { 0, BX_IA_ERROR },
  /* 5B */ { 0, BX_IA_ERROR },
  /* 5C */ { 0, BX_IA_ERROR },
  /* 5D */ { 0, BX_IA_ERROR },
  /* 5E */ { 0, BX_IA_ERROR },
  /* 5F */ { 0, BX_IA_ERROR },
  /* 60 */ { 0, BX_IA_ERROR },
  /* 61 */ { 0, BX_IA_ERROR },
  /* 62 */ { 0, BX_IA_ERROR },
  /* 63 */ { 0, BX_IA_ERROR },
  /* 64 */ { 0, BX_IA_ERROR },
  /* 65 */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* 67 */ { 0, BX_IA_ERROR },
  /* 68 */ { 0, BX_IA_ERROR },
  /* 69 */ { 0, BX_IA_ERROR },
  /* 6A */ { 0, BX_IA_ERROR },
  /* 6B */ { 0, BX_IA_ERROR },
  /* 6C */ { 0, BX_IA_ERROR },
  /* 6D */ { 0, BX_IA_ERROR },
  /* 6E */ { 0, BX_IA_ERROR },
  /* 6F */ { 0, BX_IA_ERROR },
  /* 70 */ { 0, BX_IA_ERROR },
  /* 71 */ { 0, BX_IA_ERROR },
  /* 72 */ { 0, BX_IA_ERROR },
  /* 73 */ { 0, BX_IA_ERROR },
  /* 74 */ { 0, BX_IA_ERROR },
  /* 75 */ { 0, BX_IA_ERROR },
  /* 76 */ { 0, BX_IA_ERROR },
  /* 77 */ { 0, BX_IA_ERROR },
  /* 78 */ { 0, BX_IA_ERROR },
  /* 79 */ { 0, BX_IA_ERROR },
  /* 7A */ { 0, BX_IA_ERROR },
  /* 7B */ { 0, BX_IA_ERROR },
  /* 7C */ { 0, BX_IA_ERROR },
  /* 7D */ { 0, BX_IA_ERROR },
  /* 7E */ { 0, BX_IA_ERROR },
  /* 7F */ { 0, BX_IA_ERROR },
  /* 80 */ { 0, BX_IA_ERROR },
  /* 81 */ { 0, BX_IA_ERROR },
  /* 82 */ { 0, BX_IA_ERROR },
  /* 83 */ { 0, BX_IA_ERROR },
  /* 84 */ { 0, BX_IA_ERROR },
  /* 85 */ { 0, BX_IA_ERROR },
  /* 86 */ { 0, BX_IA_ERROR },
  /* 87 */ { 0, BX_IA_ERROR },
  /* 88 */ { 0, BX_IA_ERROR },
  /* 89 */ { 0, BX_IA_ERROR },
  /* 8A */ { 0, BX_IA_ERROR },
  /* 8B */ { 0, BX_IA_ERROR },
  /* 8C */ { 0, BX_IA_ERROR },
  /* 8D */ { 0, BX_IA_ERROR },
  /* 8E */ { 0, BX_IA_ERROR },
  /* 8F */ { 0, BX_IA_ERROR },
  /* 90 */ { 0, BX_IA_ERROR },
  /* 91 */ { 0, BX_IA_ERROR },
  /* 92 */ { 0, BX_IA_ERROR },
  /* 93 */ { 0, BX_IA_ERROR },
  /* 94 */ { 0, BX_IA_ERROR },
  /* 95 */ { 0, BX_IA_ERROR },
  /* 96 */ { 0, BX_IA_ERROR },
  /* 97 */ { 0, BX_IA_ERROR },
  /* 98 */ { 0, BX_IA_ERROR },
  /* 99 */ { 0, BX_IA_ERROR },
  /* 9A */ { 0, BX_IA_ERROR },
  /* 9B */ { 0, BX_IA_ERROR },
  /* 9C */ { 0, BX_IA_ERROR },
  /* 9D */ { 0, BX_IA_ERROR },
  /* 9E */ { 0, BX_IA_ERROR },
  /* 9F */ { 0, BX_IA_ERROR },
  /* A0 */ { 0, BX_IA_ERROR },
  /* A1 */ { 0, BX_IA_ERROR },
  /* A2 */ { 0, BX_IA_ERROR },
  /* A3 */ { 0, BX_IA_ERROR },
  /* A4 */ { 0, BX_IA_ERROR },
  /* A5 */ { 0, BX_IA_ERROR },
  /* A6 */ { 0, BX_IA_ERROR },
  /* A7 */ { 0, BX_IA_ERROR },
  /* A8 */ { 0, BX_IA_ERROR },
  /* A9 */ { 0, BX_IA_ERROR },
  /* AA */ { 0, BX_IA_ERROR },
  /* AB */ { 0, BX_IA_ERROR },
  /* AC */ { 0, BX_IA_ERROR },
  /* AD */ { 0, BX_IA_ERROR },
  /* AE */ { 0, BX_IA_ERROR },
  /* AF */ { 0, BX_IA_ERROR },
  /* B0 */ { 0, BX_IA_ERROR },
  /* B1 */ { 0, BX_IA_ERROR },
  /* B2 */ { 0, BX_IA_ERROR },
  /* B3 */ { 0, BX_IA_ERROR },
  /* B4 */ { 0, BX_IA_ERROR },
  /* B5 */ { 0, BX_IA_ERROR },
  /* B6 */ { 0, BX_IA_ERROR },
  /* B7 */ { 0, BX_IA_ERROR },
  /* B8 */ { 0, BX_IA_ERROR },
  /* B9 */ { 0, BX_IA_ERROR },
  /* BA */ { 0, BX_IA_ERROR },
  /* BB */ { 0, BX_IA_ERROR },
  /* BC */ { 0, BX_IA_ERROR },
  /* BD */ { 0, BX_IA_ERROR },
  /* BE */ { 0, BX_IA_ERROR },
  /* BF */ { 0, BX_IA_ERROR },
  /* C0 */ { 0, BX_IA_ERROR },
  /* C1 */ { 0, BX_IA_ERROR },
  /* C2 */ { 0, BX_IA_ERROR },
  /* C3 */ { 0, BX_IA_ERROR },
  /* C4 */ { 0, BX_IA_ERROR },
  /* C5 */ { 0, BX_IA_ERROR },
  /* C6 */ { 0, BX_IA_ERROR },
  /* C7 */ { 0, BX_IA_ERROR },
  /* C8 */ { 0, BX_IA_ERROR },
  /* C9 */ { 0, BX_IA_ERROR },
  /* CA */ { 0, BX_IA_ERROR },
  /* CB */ { 0, BX_IA_ERROR },
  /* CC */ { 0, BX_IA_ERROR },
  /* CD */ { 0, BX_IA_ERROR },
  /* CE */ { 0, BX_IA_ERROR },
  /* CF */ { 0, BX_IA_ERROR },
  /* D0 */ { 0, BX_IA_ERROR },
  /* D1 */ { 0, BX_IA_ERROR },
  /* D2 */ { 0, BX_IA_ERROR },
  /* D3 */ { 0, BX_IA_ERROR },
  /* D4 */ { 0, BX_IA_ERROR },
  /* D5 */ { 0, BX_IA_ERROR },
  /* D6 */ { 0, BX_IA_ERROR },
  /* D7 */ { 0, BX_IA_ERROR },
  /* D8 */ { 0, BX_IA_ERROR },
  /* D9 */ { 0, BX_IA_ERROR },
  /* DA */ { 0, BX_IA_ERROR },
  /* DB */ { BxPrefixSSE66, BX_IA_VAESIMC_VdqWdq },
  /* DC */ { BxPrefixSSE66, BX_IA_VAESENC_VdqWdq },
  /* DD */ { BxPrefixSSE66, BX_IA_VAESENCLAST_VdqWdq },
  /* DE */ { BxPrefixSSE66, BX_IA_VAESDEC_VdqWdq },
  /* DF */ { BxPrefixSSE66, BX_IA_VAESDECLAST_VdqWdq },
  /* E0 */ { 0, BX_IA_ERROR },
  /* E1 */ { 0, BX_IA_ERROR },
  /* E2 */ { 0, BX_IA_ERROR },
  /* E3 */ { 0, BX_IA_ERROR },
  /* E4 */ { 0, BX_IA_ERROR },
  /* E5 */ { 0, BX_IA_ERROR },
  /* E6 */ { 0, BX_IA_ERROR },
  /* E7 */ { 0, BX_IA_ERROR },
  /* E8 */ { 0, BX_IA_ERROR },
  /* E9 */ { 0, BX_IA_ERROR },
  /* EA */ { 0, BX_IA_ERROR },
  /* EB */ { 0, BX_IA_ERROR },
  /* EC */ { 0, BX_IA_ERROR },
  /* ED */ { 0, BX_IA_ERROR },
  /* EE */ { 0, BX_IA_ERROR },
  /* EF */ { 0, BX_IA_ERROR },
  /* F0 */ { 0, BX_IA_ERROR },
  /* F1 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F4 */ { 0, BX_IA_ERROR },
  /* F5 */ { 0, BX_IA_ERROR },
  /* F6 */ { 0, BX_IA_ERROR },
  /* F7 */ { 0, BX_IA_ERROR },
  /* F8 */ { 0, BX_IA_ERROR },
  /* F9 */ { 0, BX_IA_ERROR },
  /* FA */ { 0, BX_IA_ERROR },
  /* FB */ { 0, BX_IA_ERROR },
  /* FC */ { 0, BX_IA_ERROR },
  /* FD */ { 0, BX_IA_ERROR },
  /* FE */ { 0, BX_IA_ERROR },
  /* FF */ { 0, BX_IA_ERROR },

  // 256 entries for VEX-encoded 0x0F 0x3A opcodes
  /* 00 */ { 0, BX_IA_ERROR },
  /* 01 */ { 0, BX_IA_ERROR },
  /* 02 */ { 0, BX_IA_ERROR },
  /* 03 */ { 0, BX_IA_ERROR },
  /* 04 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPS_VpsWpsIb },
  /* 05 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERMILPD_VpdWpdIb },
  /* 06 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VPERM2F128_VdqWdqIb },
  /* 07 */ { 0, BX_IA_ERROR },
  /* 08 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPS_VpsWpsIb },
  /* 09 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDPD_VpdWpdIb },
  /* 0A */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSS_VssWssIb },
  /* 0B */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VROUNDSD_VsdWsdIb },
  /* 0C */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPS_VpsWpsIb },
  /* 0D */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VBLENDPD_VpdWpdIb },
  /* 0E */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPBLENDW_VdqWdqIb },
  /* 0F */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPALIGNR_VdqWdqIb },
  /* 10 */ { 0, BX_IA_ERROR },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPEXTRB_EbdVdqIb },
  /* 15 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPEXTRW_EwdVdqIb },
  /* 16 */ { BxSplitVexW | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3a16 },
  /* 17 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VEXTRACTPS_EdVpsIb },
  /* 18 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VINSERTF128_VdqWdqIb },
  /* 19 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_VEXTRACTF128_WdqVdqIb },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { 0, BX_IA_ERROR },
  /* 1D */ { 0, BX_IA_ERROR },
  /* 1E */ { 0, BX_IA_ERROR },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPINSRB_VdqEbIb },
  /* 21 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VINSERTPS_VpsWssIb },
  /* 22 */ { BxSplitVexW | BxImmediate_Ib, BX_IA_ERROR, BxOpcodeInfoAVX_VexW_0f3a22 },
  /* 23 */ { 0, BX_IA_ERROR },
  /* 24 */ { 0, BX_IA_ERROR },
  /* 25 */ { 0, BX_IA_ERROR },
  /* 26 */ { 0, BX_IA_ERROR },
  /* 27 */ { 0, BX_IA_ERROR },
  /* 28 */ { 0, BX_IA_ERROR },
  /* 29 */ { 0, BX_IA_ERROR },
  /* 2A */ { 0, BX_IA_ERROR },
  /* 2B */ { 0, BX_IA_ERROR },
  /* 2C */ { 0, BX_IA_ERROR },
  /* 2D */ { 0, BX_IA_ERROR },
  /* 2E */ { 0, BX_IA_ERROR },
  /* 2F */ { 0, BX_IA_ERROR },
  /* 30 */ { 0, BX_IA_ERROR },
  /* 31 */ { 0, BX_IA_ERROR },
  /* 32 */ { 0, BX_IA_ERROR },
  /* 33 */ { 0, BX_IA_ERROR },
  /* 34 */ { 0, BX_IA_ERROR },
  /* 35 */ { 0, BX_IA_ERROR },
  /* 36 */ { 0, BX_IA_ERROR },
  /* 37 */ { 0, BX_IA_ERROR },
  /* 38 */ { 0, BX_IA_ERROR },
  /* 39 */ { 0, BX_IA_ERROR },
  /* 3A */ { 0, BX_IA_ERROR },
  /* 3B */ { 0, BX_IA_ERROR },
  /* 3C */ { 0, BX_IA_ERROR },
  /* 3D */ { 0, BX_IA_ERROR },
  /* 3E */ { 0, BX_IA_ERROR },
  /* 3F */ { 0, BX_IA_ERROR },
  /* 40 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VDPPS_VpsWpsIb },
  /* 41 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VDPPD_VpdWpdIb },
  /* 42 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VMPSADBW_VdqWdqIb },
  /* 43 */ { 0, BX_IA_ERROR },
  /* 44 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPCLMULQDQ_VdqWdqIb },
  /* 45 */ { 0, BX_IA_ERROR },
  /* 46 */ { 0, BX_IA_ERROR },
  /* 47 */ { 0, BX_IA_ERROR },
  /* 48 */ { 0, BX_IA_ERROR },
  /* 49 */ { 0, BX_IA_ERROR },
  /* 4A */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPS_VpsWpsIb },
  /* 4B */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VBLENDVPD_VpdWpdIb },
  /* 4C */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib4, BX_IA_VPBLENDVB_VdqWdqIb },
  /* 4D */ { 0, BX_IA_ERROR },
  /* 4E */ { 0, BX_IA_ERROR },
  /* 4F */ { 0, BX_IA_ERROR },
  /* 50 */ { 0, BX_IA_ERROR },
  /* 51 */ { 0, BX_IA_ERROR },
  /* 52 */ { 0, BX_IA_ERROR },
  /* 53 */ { 0, BX_IA_ERROR },
  /* 54 */ { 0, BX_IA_ERROR },
  /* 55 */ { 0, BX_IA_ERROR },
  /* 56 */ { 0, BX_IA_ERROR },
  /* 57 */ { 0, BX_IA_ERROR },
  /* 58 */ { 0, BX_IA_ERROR },
  /* 59 */ { 0, BX_IA_ERROR },
  /* 5A */ { 0, BX_IA_ERROR },
  /* 5B */ { 0, BX_IA_ERROR },
  /* 5C */ { 0, BX_IA_ERROR },
  /* 5D */ { 0, BX_IA_ERROR },
  /* 5E */ { 0, BX_IA_ERROR },
  /* 5F */ { 0, BX_IA_ERROR },
  /* 60 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPCMPESTRM_VdqWdqIb },
  /* 61 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPCMPESTRI_VdqWdqIb },
  /* 62 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPCMPISTRM_VdqWdqIb },
  /* 63 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VPCMPISTRI_VdqWdqIb },
  /* 64 */ { 0, BX_IA_ERROR },
  /* 65 */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* 67 */ { 0, BX_IA_ERROR },
  /* 68 */ { 0, BX_IA_ERROR },
  /* 69 */ { 0, BX_IA_ERROR },
  /* 6A */ { 0, BX_IA_ERROR },
  /* 6B */ { 0, BX_IA_ERROR },
  /* 6C */ { 0, BX_IA_ERROR },
  /* 6D */ { 0, BX_IA_ERROR },
  /* 6E */ { 0, BX_IA_ERROR },
  /* 6F */ { 0, BX_IA_ERROR },
  /* 70 */ { 0, BX_IA_ERROR },
  /* 71 */ { 0, BX_IA_ERROR },
  /* 72 */ { 0, BX_IA_ERROR },
  /* 73 */ { 0, BX_IA_ERROR },
  /* 74 */ { 0, BX_IA_ERROR },
  /* 75 */ { 0, BX_IA_ERROR },
  /* 76 */ { 0, BX_IA_ERROR },
  /* 77 */ { 0, BX_IA_ERROR },
  /* 78 */ { 0, BX_IA_ERROR },
  /* 79 */ { 0, BX_IA_ERROR },
  /* 7A */ { 0, BX_IA_ERROR },
  /* 7B */ { 0, BX_IA_ERROR },
  /* 7C */ { 0, BX_IA_ERROR },
  /* 7D */ { 0, BX_IA_ERROR },
  /* 7E */ { 0, BX_IA_ERROR },
  /* 7F */ { 0, BX_IA_ERROR },
  /* 80 */ { 0, BX_IA_ERROR },
  /* 81 */ { 0, BX_IA_ERROR },
  /* 82 */ { 0, BX_IA_ERROR },
  /* 83 */ { 0, BX_IA_ERROR },
  /* 84 */ { 0, BX_IA_ERROR },
  /* 85 */ { 0, BX_IA_ERROR },
  /* 86 */ { 0, BX_IA_ERROR },
  /* 87 */ { 0, BX_IA_ERROR },
  /* 88 */ { 0, BX_IA_ERROR },
  /* 89 */ { 0, BX_IA_ERROR },
  /* 8A */ { 0, BX_IA_ERROR },
  /* 8B */ { 0, BX_IA_ERROR },
  /* 8C */ { 0, BX_IA_ERROR },
  /* 8D */ { 0, BX_IA_ERROR },
  /* 8E */ { 0, BX_IA_ERROR },
  /* 8F */ { 0, BX_IA_ERROR },
  /* 90 */ { 0, BX_IA_ERROR },
  /* 91 */ { 0, BX_IA_ERROR },
  /* 92 */ { 0, BX_IA_ERROR },
  /* 93 */ { 0, BX_IA_ERROR },
  /* 94 */ { 0, BX_IA_ERROR },
  /* 95 */ { 0, BX_IA_ERROR },
  /* 96 */ { 0, BX_IA_ERROR },
  /* 97 */ { 0, BX_IA_ERROR },
  /* 98 */ { 0, BX_IA_ERROR },
  /* 99 */ { 0, BX_IA_ERROR },
  /* 9A */ { 0, BX_IA_ERROR },
  /* 9B */ { 0, BX_IA_ERROR },
  /* 9C */ { 0, BX_IA_ERROR },
  /* 9D */ { 0, BX_IA_ERROR },
  /* 9E */ { 0, BX_IA_ERROR },
  /* 9F */ { 0, BX_IA_ERROR },
  /* A0 */ { 0, BX_IA_ERROR },
  /* A1 */ { 0, BX_IA_ERROR },
  /* A2 */ { 0, BX_IA_ERROR },
  /* A3 */ { 0, BX_IA_ERROR },
  /* A4 */ { 0, BX_IA_ERROR },
  /* A5 */ { 0, BX_IA_ERROR },
  /* A6 */ { 0, BX_IA_ERROR },
  /* A7 */ { 0, BX_IA_ERROR },
  /* A8 */ { 0, BX_IA_ERROR },
  /* A9 */ { 0, BX_IA_ERROR },
  /* AA */ { 0, BX_IA_ERROR },
  /* AB */ { 0, BX_IA_ERROR },
  /* AC */ { 0, BX_IA_ERROR },
  /* AD */ { 0, BX_IA_ERROR },
  /* AE */ { 0, BX_IA_ERROR },
  /* AF */ { 0, BX_IA_ERROR },
  /* B0 */ { 0, BX_IA_ERROR },
  /* B1 */ { 0, BX_IA_ERROR },
  /* B2 */ { 0, BX_IA_ERROR },
  /* B3 */ { 0, BX_IA_ERROR },
  /* B4 */ { 0, BX_IA_ERROR },
  /* B5 */ { 0, BX_IA_ERROR },
  /* B6 */ { 0, BX_IA_ERROR },
  /* B7 */ { 0, BX_IA_ERROR },
  /* B8 */ { 0, BX_IA_ERROR },
  /* B9 */ { 0, BX_IA_ERROR },
  /* BA */ { 0, BX_IA_ERROR },
  /* BB */ { 0, BX_IA_ERROR },
  /* BC */ { 0, BX_IA_ERROR },
  /* BD */ { 0, BX_IA_ERROR },
  /* BE */ { 0, BX_IA_ERROR },
  /* BF */ { 0, BX_IA_ERROR },
  /* C0 */ { 0, BX_IA_ERROR },
  /* C1 */ { 0, BX_IA_ERROR },
  /* C2 */ { 0, BX_IA_ERROR },
  /* C3 */ { 0, BX_IA_ERROR },
  /* C4 */ { 0, BX_IA_ERROR },
  /* C5 */ { 0, BX_IA_ERROR },
  /* C6 */ { 0, BX_IA_ERROR },
  /* C7 */ { 0, BX_IA_ERROR },
  /* C8 */ { 0, BX_IA_ERROR },
  /* C9 */ { 0, BX_IA_ERROR },
  /* CA */ { 0, BX_IA_ERROR },
  /* CB */ { 0, BX_IA_ERROR },
  /* CC */ { 0, BX_IA_ERROR },
  /* CD */ { 0, BX_IA_ERROR },
  /* CE */ { 0, BX_IA_ERROR },
  /* CF */ { 0, BX_IA_ERROR },
  /* D0 */ { 0, BX_IA_ERROR },
  /* D1 */ { 0, BX_IA_ERROR },
  /* D2 */ { 0, BX_IA_ERROR },
  /* D3 */ { 0, BX_IA_ERROR },
  /* D4 */ { 0, BX_IA_ERROR },
  /* D5 */ { 0, BX_IA_ERROR },
  /* D6 */ { 0, BX_IA_ERROR },
  /* D7 */ { 0, BX_IA_ERROR },
  /* D8 */ { 0, BX_IA_ERROR },
  /* D9 */ { 0, BX_IA_ERROR },
  /* DA */ { 0, BX_IA_ERROR },
  /* DB */ { 0, BX_IA_ERROR },
  /* DC */ { 0, BX_IA_ERROR },
  /* DD */ { 0, BX_IA_ERROR },
  /* DE */ { 0, BX_IA_ERROR },
  /* DF */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_VAESKEYGENASSIST_VdqWdqIb },
  /* E0 */ { 0, BX_IA_ERROR },
  /* E1 */ { 0, BX_IA_ERROR },
  /* E2 */ { 0, BX_IA_ERROR },
  /* E3 */ { 0, BX_IA_ERROR },
  /* E4 */ { 0, BX_IA_ERROR },
  /* E5 */ { 0, BX_IA_ERROR },
  /* E6 */ { 0, BX_IA_ERROR },
  /* E7 */ { 0, BX_IA_ERROR },
  /* E8 */ { 0, BX_IA_ERROR },
  /* E9 */ { 0, BX_IA_ERROR },
  /* EA */ { 0, BX_IA_ERROR },
  /* EB */ { 0, BX_IA_ERROR },
  /* EC */ { 0, BX_IA_ERROR },
  /* ED */ { 0, BX_IA_ERROR },
  /* EE */ { 0, BX_IA_ERROR },
  /* EF */ { 0, BX_IA_ERROR },
  /* F0 */ { 0, BX_IA_ERROR },
  /* F1 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F4 */ { 0, BX_IA_ERROR },
  /* F5 */ { 0, BX_IA_ERROR },
  /* F6 */ { 0, BX_IA_ERROR },
  /* F7 */ { 0, BX_IA_ERROR },
  /* F8 */ { 0, BX_IA_ERROR },
  /* F9 */ { 0, BX_IA_ERROR },
  /* FA */ { 0, BX_IA_ERROR },
  /* FB */ { 0, BX_IA_ERROR },
  /* FC */ { 0, BX_IA_ERROR },
  /* FD */ { 0, BX_IA_ERROR },
  /* FE */ { 0, BX_IA_ERROR },
  /* FF */ { 0, BX_IA_ERROR }
};

#endif

#endif // BX_AVX_FETCHDECODE_TABLES_H
