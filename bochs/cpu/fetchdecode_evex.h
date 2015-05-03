/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2014 Stanislav Shwartsman
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

#ifndef BX_EVEX_FETCHDECODE_TABLES_H
#define BX_EVEX_FETCHDECODE_TABLES_H

#if BX_SUPPORT_EVEX

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10R[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_VpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_VpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10M[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_VpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_VpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_VssWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10[2] = {
  /* R */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10R },
  /* M */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10M },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10R_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_VpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_VpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10M_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_VpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_VpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_VssWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_VsdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f10_Mask[2] = {
  /* R */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10R_Mask },
  /* M */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10M_Mask },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11R[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_WpsVps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_WpdVpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_WssHpsVss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_WsdHpdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11M[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_WpsVps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_WpdVpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_WssVss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_WsdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11[2] = {
  /* R */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11R },
  /* M */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11M },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11R_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_WpsVps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_WpdVpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_WssHpsVss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_WsdHpdVsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11M_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVUPS_WpsVps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVUPD_WpdVpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSS_WssVss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVSD_WsdVsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f11_Mask[2] = {
  /* R */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11R_Mask },
  /* M */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11M_Mask },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f12[4] = {
  /* -- */ { BxVexW0 | BxVexL0, BX_IA_V512_VMOVLPS_VpsHpsMq },
  /* 66 */ { BxVexW1 | BxVexL0, BX_IA_V512_VMOVLPD_VpdHpdMq },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSLDUP_VpsWps },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVDDUP_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f12_Mask[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSLDUP_VpsWps_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMOVDDUP_VpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f13M[2] = {
  /* -- */ { BxVexW0 | BxVexL0, BX_IA_V512_VMOVLPS_MqVps },
  /* 66 */ { BxVexW1 | BxVexL0, BX_IA_V512_VMOVLPD_MqVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f14[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VUNPCKLPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VUNPCKLPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f14_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VUNPCKLPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VUNPCKLPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f15[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VUNPCKHPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VUNPCKHPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f15_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VUNPCKHPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VUNPCKHPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f16[4] = {
  /* -- */ { BxVexW0 | BxVexL0, BX_IA_V512_VMOVHPS_VpsHpsMq },
  /* 66 */ { BxVexW1 | BxVexL0, BX_IA_V512_VMOVHPD_VpdHpdMq },
  /* F3 */ { BxVexW0, BX_IA_V512_VMOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f17M[2] = {
  /* -- */ { BxVexW0 | BxVexL0, BX_IA_V512_VMOVHPS_MqVps },
  /* 66 */ { BxVexW1 | BxVexL0, BX_IA_V512_VMOVHPD_MqVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f28[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVAPS_VpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVAPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f28_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVAPS_VpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVAPD_VpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f29[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVAPS_WpsVps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVAPD_WpdVpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f29_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVAPS_WpsVps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVAPD_WpdVpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2a[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTSI2SS_VssEd },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTSI2SD_VsdEd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2b[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMOVNTPS_MpsVps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMOVNTPD_MpdVpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2c[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTTSS2SI_GdWss },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTTSD2SI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2d[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTSS2SI_GdWss },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTSD2SI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2e[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VUCOMISS_VssWss },
  /* 66 */ { BxVexW1, BX_IA_V512_VUCOMISD_VsdWsd }            	
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f2f[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VCOMISS_VssWss },
  /* 66 */ { BxVexW1, BX_IA_V512_VCOMISD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f51[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VSQRTPS_VpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VSQRTPD_VpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VSQRTSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VSQRTSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f51_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VSQRTPS_VpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VSQRTPD_VpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VSQRTSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VSQRTSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f54[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VANDPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VANDPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f54_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VANDPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VANDPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f55[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VANDNPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VANDNPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f55_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VANDNPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VANDNPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f56[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VORPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VORPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f56_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VORPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VORPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f57[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VXORPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VXORPD_VpdHpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f57_Mask[2] = {
  /* -- */ { BxVexW0, BX_IA_V512_VXORPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VXORPD_VpdHpdWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f58[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VADDPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VADDPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VADDSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VADDSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f58_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VADDPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VADDPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VADDSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VADDSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f59[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMULPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMULPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMULSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMULSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f59_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMULPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMULPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMULSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMULSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5a[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VCVTPS2PD_VpdWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VCVTPD2PS_VpsWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VCVTSS2SD_VsdWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VCVTSD2SS_VssWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5a_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VCVTPS2PD_VpdWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VCVTPD2PS_VpsWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VCVTSS2SD_VsdWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VCVTSD2SS_VssWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5b[4] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTDQ2PS_VpsWdq },
  /* 66 */ { BxVexW0, BX_IA_V512_VCVTPS2DQ_VdqWps },
  /* F3 */ { BxVexW0, BX_IA_V512_VCVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5b_Mask[4] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTDQ2PS_VpsWdq_Kmask },
  /* 66 */ { BxVexW0, BX_IA_V512_VCVTPS2DQ_VdqWps_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VCVTTPS2DQ_VdqWps_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5c[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VSUBPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VSUBPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VSUBSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VSUBSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5c_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VSUBPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VSUBPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VSUBSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VSUBSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5d[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMINPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMINPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMINSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMINSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5d_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMINPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMINPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMINSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMINSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5e[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VDIVPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VDIVPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VDIVSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VDIVSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5e_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VDIVPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VDIVPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VDIVSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VDIVSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5f[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMAXPS_VpsHpsWps },
  /* 66 */ { BxVexW1, BX_IA_V512_VMAXPD_VpdHpdWpd },
  /* F3 */ { BxVexW0, BX_IA_V512_VMAXSS_VssHpsWss },
  /* F2 */ { BxVexW1, BX_IA_V512_VMAXSD_VsdHpdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f5f_Mask[4] = {
  /* -- */ { BxVexW0, BX_IA_V512_VMAXPS_VpsHpsWps_Kmask },
  /* 66 */ { BxVexW1, BX_IA_V512_VMAXPD_VpdHpdWpd_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VMAXSS_VssHpsWss_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VMAXSD_VsdHpdWsd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f6f[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VMOVDQA32_VdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VMOVDQU32_VdqWdq },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VMOVDQU8_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f6f_Mask[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VMOVDQA32_VdqWdq_Kmask },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VMOVDQU32_VdqWdq_Kmask },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VMOVDQU8_VdqWdq_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f70[3] = {
  /* 66 */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VPSHUFD_VdqWdqIb },
  /* F3 */ { BxImmediate_Ib, BX_IA_V512_VPSHUFHW_VdqWdqIb },
  /* F2 */ { BxImmediate_Ib, BX_IA_V512_VPSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f70_Mask[3] = {
  /* 66 */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VPSHUFD_VdqWdqIb_Kmask },
  /* F3 */ { BxImmediate_Ib, BX_IA_V512_VPSHUFHW_VdqWdqIb_Kmask },
  /* F2 */ { BxImmediate_Ib, BX_IA_V512_VPSHUFLW_VdqWdqIb_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f78[4] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTTPS2UDQ_VdqWps },
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTTPS2UQQ_VdqWps },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTTSS2USI_GdWss },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTTSD2USI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f78_Mask[2] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTTPS2UDQ_VdqWps_Kmask },
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTTPS2UQQ_VdqWps_Kmask },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f79[4] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTPS2UDQ_VdqWps },
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTPS2UQQ_VdqWps },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTSS2USI_GdWss },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTSD2USI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f79_Mask[2] = {
  /* -- */ { BxAliasVexW, BX_IA_V512_VCVTPS2UDQ_VdqWps_Kmask },
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTPS2UQQ_VdqWps_Kmask },
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7a[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTTPS2QQ_VdqWps },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VCVTUDQ2PD_VpdWdq },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VCVTUDQ2PS_VpsWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7a_Mask[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTTPS2QQ_VdqWps_Kmask },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VCVTUDQ2PD_VpdWdq_Kmask },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VCVTUDQ2PS_VpsWdq_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7b[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VCVTPS2QQ_VdqWps },
  /* F3 */ { BxAliasVexW64, BX_IA_V512_VCVTUSI2SS_VssEd },
  /* F2 */ { BxAliasVexW64, BX_IA_V512_VCVTUSI2SD_VsdEd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7e[3] = {
  /* 66 */ { BxAliasVexW64, BX_IA_V512_VMOVD_EdVd },
  /* F3 */ {       BxVexW1, BX_IA_V512_VMOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7f[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VMOVDQA32_WdqVdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VMOVDQU32_WdqVdq },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VMOVDQU8_WdqVdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f7f_Mask[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VMOVDQA32_WdqVdq_Kmask },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VMOVDQU32_WdqVdq_Kmask },
  /* F2 */ { BxAliasVexW, BX_IA_V512_VMOVDQU8_WdqVdq_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0fc2[4] = {
  /* -- */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VCMPPS_KGwHpsWpsIb },
  /* 66 */ { BxVexW1 | BxImmediate_Ib, BX_IA_V512_VCMPPD_KGbHpdWpdIb },
  /* F3 */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VCMPSS_KGbHssWssIb },
  /* F2 */ { BxVexW1 | BxImmediate_Ib, BX_IA_V512_VCMPSD_KGbHsdWsdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0fc6[2] = {
  /* -- */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VSHUFPS_VpsHpsWpsIb },
  /* 66 */ { BxVexW1 | BxImmediate_Ib, BX_IA_V512_VSHUFPD_VpdHpdWpdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0fc6_Mask[2] = {
  /* -- */ { BxVexW0 | BxImmediate_Ib, BX_IA_V512_VSHUFPS_VpsHpsWpsIb_Kmask },
  /* 66 */ { BxVexW1 | BxImmediate_Ib, BX_IA_V512_VSHUFPD_VpdHpdWpdIb_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0fe6[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VCVTTPD2DQ_VdqWpd },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VCVTDQ2PD_VpdWdq },
  /* F2 */ { BxVexW1, BX_IA_V512_VCVTPD2DQ_VdqWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0fe6_Mask[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VCVTTPD2DQ_VdqWpd_Kmask },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VCVTDQ2PD_VpdWdq_Kmask },
  /* F2 */ { BxVexW1, BX_IA_V512_VCVTPD2DQ_VdqWpd_Kmask }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3810[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSRLVW_VdqHdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSWB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3810_Mask[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSRLVW_VdqHdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSWB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3811[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSRAVW_VdqHdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSDB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3811_Mask[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSRAVW_VdqHdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSDB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3812[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSLLVW_VdqHdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3812_Mask[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPSLLVW_VdqHdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3813[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VCVTPH2PS_VpsWps },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSDW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3813_Mask[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VCVTPH2PS_VpsWps_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSDW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3814[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPRORVD_VdqHdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3814_Mask[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPRORVD_VdqHdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3815[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPROLVD_VdqHdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQD_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3815_Mask[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPROLVD_VdqHdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVUSQD_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3820[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBW_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSWB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3820_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBW_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSWB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3821[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBD_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSDB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3821_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBD_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSDB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3822[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3822_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXBQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3823[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXWD_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSDW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3823_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXWQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSDW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3824[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXWQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3824_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVSXWD_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3825[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VPMOVSXDQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQD_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3825_Mask[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VPMOVSXDQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVSQD_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3826[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPTESTMB_KGqHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPTESTNMB_KGqHdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3827[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPTESTMD_KGwHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPTESTNMD_KGwHdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3828[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPMULDQ_VdqHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPMOVM2B_VdqKEq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3829[3] = {
  /* 66 */ { BxVexW1, BX_IA_V512_VPCMPEQQ_KGbHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPMOVB2M_KGqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f382a[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VMOVNTDQA_VdqMdq },
  /* F3 */ { BxVexW1, BX_IA_V512_VPBROADCASTMB2Q_VdqKEb },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3830[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBW_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVWB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3830_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBW_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVWB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3831[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBD_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVDB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3831_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBD_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVDB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3832[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQB_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3832_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXBQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQB_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3833[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXWD_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVDW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3833_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXWQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVDW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3834[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXWQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQW_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3834_Mask[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMOVZXWD_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQW_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3835[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VPMOVZXDQ_VdqWdq },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQD_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3835_Mask[3] = {
  /* 66 */ { BxVexW0, BX_IA_V512_VPMOVZXDQ_VdqWdq_Kmask },
  /* F3 */ { BxVexW0, BX_IA_V512_VPMOVQD_WdqVdq_Kmask },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3838[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMINSB_VdqHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPMOVM2D_VdqKEw },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f3839[3] = {
  /* 66 */ { BxAliasVexW, BX_IA_V512_VPMINSD_VdqHdqWdq },
  /* F3 */ { BxAliasVexW, BX_IA_V512_VPMOVD2M_KGwWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupEVEX_0f383a[3] = {
  /* 66 */ { 0, BX_IA_V512_VPMINUW_VdqHdqWdq },
  /* F3 */ { BxVexW1, BX_IA_V512_VPBROADCASTMW2D_VdqKEw },
  /* F2 */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G12R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSRLW_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSRAW_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSLLW_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G12R_Mask[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSRLW_UdqIb_Kmask },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSRAW_UdqIb_Kmask },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSLLW_UdqIb_Kmask },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G13R[8] = {
  /* 0 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPRORD_UdqIb },
  /* 1 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPROLD_UdqIb },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW0,     BX_IA_V512_VPSRLD_UdqIb },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPSRAD_UdqIb },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW0,     BX_IA_V512_VPSLLD_UdqIb },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G13R_Mask[8] = {
  /* 0 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPRORD_UdqIb_Kmask },
  /* 1 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPROLD_UdqIb_Kmask },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW0,     BX_IA_V512_VPSRLD_UdqIb_Kmask },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPSRAD_UdqIb_Kmask },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW0,     BX_IA_V512_VPSLLD_UdqIb_Kmask },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G14R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPSRLQ_UdqIb },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSRLDQ_UdqIb },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPSLLQ_UdqIb },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE66, BX_IA_V512_VPSLLDQ_UdqIb },
};

static const BxOpcodeInfo_t BxOpcodeInfoEVEX_G14R_Mask[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPSRLQ_UdqIb_Kmask },
  /* 3 */ { 0, BX_IA_ERROR }, // #UD
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPSLLQ_UdqIb_Kmask },
  /* 7 */ { 0, BX_IA_ERROR }  // #UD
};

/* ************************************************************************ */

static const BxOpcodeInfo_t BxOpcodeTableEVEX[256*3*2] = {
  // 256 entries for EVEX-encoded 0x0F opcodes
  /* 00 k0 */ { 0, BX_IA_ERROR },
  /* 00    */ { 0, BX_IA_ERROR },
  /* 01 k0 */ { 0, BX_IA_ERROR },
  /* 01    */ { 0, BX_IA_ERROR },
  /* 02 k0 */ { 0, BX_IA_ERROR },
  /* 02    */ { 0, BX_IA_ERROR },
  /* 03 k0 */ { 0, BX_IA_ERROR },
  /* 03    */ { 0, BX_IA_ERROR },
  /* 04 k0 */ { 0, BX_IA_ERROR },
  /* 04    */ { 0, BX_IA_ERROR },
  /* 05 k0 */ { 0, BX_IA_ERROR },
  /* 05    */ { 0, BX_IA_ERROR },
  /* 06 k0 */ { 0, BX_IA_ERROR },
  /* 06    */ { 0, BX_IA_ERROR },
  /* 07 k0 */ { 0, BX_IA_ERROR },
  /* 07    */ { 0, BX_IA_ERROR },
  /* 08 k0 */ { 0, BX_IA_ERROR },
  /* 08    */ { 0, BX_IA_ERROR },
  /* 09 k0 */ { 0, BX_IA_ERROR },
  /* 09    */ { 0, BX_IA_ERROR },
  /* 0A k0 */ { 0, BX_IA_ERROR },
  /* 0A    */ { 0, BX_IA_ERROR },
  /* 0B k0 */ { 0, BX_IA_ERROR },
  /* 0B    */ { 0, BX_IA_ERROR },
  /* 0C k0 */ { 0, BX_IA_ERROR },
  /* 0C    */ { 0, BX_IA_ERROR },
  /* 0D k0 */ { 0, BX_IA_ERROR },
  /* 0D    */ { 0, BX_IA_ERROR },
  /* 0E k0 */ { 0, BX_IA_ERROR },
  /* 0E    */ { 0, BX_IA_ERROR },
  /* 0F k0 */ { 0, BX_IA_ERROR },
  /* 0F    */ { 0, BX_IA_ERROR },
  /* 10 k0 */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10 },      // split because of VMOVSS/VMOVSD encoding issue
  /* 10    */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupEVEX_0f10_Mask }, // split because of VMOVSS/VMOVSD encoding issue
  /* 11 k0 */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11 },      // split because of VMOVSS/VMOVSD encoding issue
  /* 11    */ { BxSplitMod11B, BX_IA_ERROR, BxOpcodeGroupEVEX_0f11_Mask }, // split because of VMOVSS/VMOVSD encoding issue
  /* 12 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f12 },
  /* 12    */ { BxPrefixSSE,  BX_IA_ERROR, BxOpcodeGroupEVEX_0f12_Mask },
  /* 13 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f13M },
  /* 13    */ { 0, BX_IA_ERROR }, // #UD
  /* 14 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f14 },
  /* 14    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f14_Mask },
  /* 15 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f15 },
  /* 15    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f15_Mask },
  /* 16 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f16 },
  /* 16    */ { BxVexW0 | BxPrefixSSEF3, BX_IA_V512_VMOVSHDUP_VpsWps_Kmask },
  /* 17 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f17M },
  /* 17    */ { 0, BX_IA_ERROR }, // #UD
  /* 18 k0 */ { 0, BX_IA_ERROR },
  /* 18    */ { 0, BX_IA_ERROR },
  /* 19 k0 */ { 0, BX_IA_ERROR },
  /* 19    */ { 0, BX_IA_ERROR },
  /* 1A k0 */ { 0, BX_IA_ERROR },
  /* 1A    */ { 0, BX_IA_ERROR },
  /* 1B k0 */ { 0, BX_IA_ERROR },
  /* 1B    */ { 0, BX_IA_ERROR },
  /* 1C k0 */ { 0, BX_IA_ERROR },
  /* 1C    */ { 0, BX_IA_ERROR },
  /* 1D k0 */ { 0, BX_IA_ERROR },
  /* 1D    */ { 0, BX_IA_ERROR },
  /* 1E k0 */ { 0, BX_IA_ERROR },
  /* 1E    */ { 0, BX_IA_ERROR },
  /* 1F k0 */ { 0, BX_IA_ERROR },
  /* 1F    */ { 0, BX_IA_ERROR },
  /* 20 k0 */ { 0, BX_IA_ERROR },
  /* 20    */ { 0, BX_IA_ERROR },
  /* 21 k0 */ { 0, BX_IA_ERROR },
  /* 21    */ { 0, BX_IA_ERROR },
  /* 22 k0 */ { 0, BX_IA_ERROR },
  /* 22    */ { 0, BX_IA_ERROR },
  /* 23 k0 */ { 0, BX_IA_ERROR },
  /* 23    */ { 0, BX_IA_ERROR },
  /* 24 k0 */ { 0, BX_IA_ERROR },
  /* 24    */ { 0, BX_IA_ERROR },
  /* 25 k0 */ { 0, BX_IA_ERROR },
  /* 25    */ { 0, BX_IA_ERROR },
  /* 26 k0 */ { 0, BX_IA_ERROR },
  /* 26    */ { 0, BX_IA_ERROR },
  /* 27 k0 */ { 0, BX_IA_ERROR },
  /* 27    */ { 0, BX_IA_ERROR },
  /* 28 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f28 },
  /* 28    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f28_Mask },
  /* 29 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f29 },
  /* 29    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f29_Mask },
  /* 2A k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2a },
  /* 2A    */ { 0, BX_IA_ERROR }, // #UD
  /* 2B k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2b },
  /* 2B    */ { 0, BX_IA_ERROR }, // #UD
  /* 2C k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2c },
  /* 2C    */ { 0, BX_IA_ERROR }, // #UD
  /* 2D k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2d },
  /* 2D    */ { 0, BX_IA_ERROR }, // #UD
  /* 2E k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2e },
  /* 2E    */ { 0, BX_IA_ERROR }, // #UD
  /* 2F k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f2f },
  /* 2F    */ { 0, BX_IA_ERROR }, // #UD
  /* 30 k0 */ { 0, BX_IA_ERROR },
  /* 30    */ { 0, BX_IA_ERROR },
  /* 31 k0 */ { 0, BX_IA_ERROR },
  /* 31    */ { 0, BX_IA_ERROR },
  /* 32 k0 */ { 0, BX_IA_ERROR },
  /* 32    */ { 0, BX_IA_ERROR },
  /* 33 k0 */ { 0, BX_IA_ERROR },
  /* 33    */ { 0, BX_IA_ERROR },
  /* 34 k0 */ { 0, BX_IA_ERROR },
  /* 34    */ { 0, BX_IA_ERROR },
  /* 35 k0 */ { 0, BX_IA_ERROR },
  /* 35    */ { 0, BX_IA_ERROR },
  /* 36 k0 */ { 0, BX_IA_ERROR },
  /* 36    */ { 0, BX_IA_ERROR },
  /* 37 k0 */ { 0, BX_IA_ERROR },
  /* 37    */ { 0, BX_IA_ERROR },
  /* 38 k0 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 38    */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 39 k0 */ { 0, BX_IA_ERROR },
  /* 39    */ { 0, BX_IA_ERROR },
  /* 3A k0 */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 3A    */ { 0, BX_IA_ERROR }, // 3-Byte Escape
  /* 3B k0 */ { 0, BX_IA_ERROR },
  /* 3B    */ { 0, BX_IA_ERROR },
  /* 3C k0 */ { 0, BX_IA_ERROR },
  /* 3C    */ { 0, BX_IA_ERROR },
  /* 3D k0 */ { 0, BX_IA_ERROR },
  /* 3D    */ { 0, BX_IA_ERROR },
  /* 3E k0 */ { 0, BX_IA_ERROR },
  /* 3E    */ { 0, BX_IA_ERROR },
  /* 3F k0 */ { 0, BX_IA_ERROR },
  /* 3F    */ { 0, BX_IA_ERROR },
  /* 40 k0 */ { 0, BX_IA_ERROR },
  /* 40    */ { 0, BX_IA_ERROR },
  /* 41 k0 */ { 0, BX_IA_ERROR },
  /* 41    */ { 0, BX_IA_ERROR },
  /* 42 k0 */ { 0, BX_IA_ERROR },
  /* 42    */ { 0, BX_IA_ERROR },
  /* 43 k0 */ { 0, BX_IA_ERROR },
  /* 43    */ { 0, BX_IA_ERROR },
  /* 44 k0 */ { 0, BX_IA_ERROR },
  /* 44    */ { 0, BX_IA_ERROR },
  /* 45 k0 */ { 0, BX_IA_ERROR },
  /* 45    */ { 0, BX_IA_ERROR },
  /* 46 k0 */ { 0, BX_IA_ERROR },
  /* 46    */ { 0, BX_IA_ERROR },
  /* 47 k0 */ { 0, BX_IA_ERROR },
  /* 47    */ { 0, BX_IA_ERROR },
  /* 48 k0 */ { 0, BX_IA_ERROR },
  /* 48    */ { 0, BX_IA_ERROR },
  /* 49 k0 */ { 0, BX_IA_ERROR },
  /* 49    */ { 0, BX_IA_ERROR },
  /* 4A k0 */ { 0, BX_IA_ERROR },
  /* 4A    */ { 0, BX_IA_ERROR },
  /* 4B k0 */ { 0, BX_IA_ERROR },
  /* 4B    */ { 0, BX_IA_ERROR },
  /* 4C k0 */ { 0, BX_IA_ERROR },
  /* 4C    */ { 0, BX_IA_ERROR },
  /* 4D k0 */ { 0, BX_IA_ERROR },
  /* 4D    */ { 0, BX_IA_ERROR },
  /* 4E k0 */ { 0, BX_IA_ERROR },
  /* 4E    */ { 0, BX_IA_ERROR },
  /* 4F k0 */ { 0, BX_IA_ERROR },
  /* 4F    */ { 0, BX_IA_ERROR },
  /* 50 k0 */ { 0, BX_IA_ERROR },
  /* 50    */ { 0, BX_IA_ERROR },
  /* 51 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f51 },
  /* 51    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f51_Mask },
  /* 52 k0 */ { 0, BX_IA_ERROR },
  /* 52    */ { 0, BX_IA_ERROR },
  /* 53 k0 */ { 0, BX_IA_ERROR },
  /* 53    */ { 0, BX_IA_ERROR },
  /* 54 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f54 },
  /* 54    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f54_Mask },
  /* 55 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f55 },
  /* 55    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f55_Mask },
  /* 56 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f56 },
  /* 56    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f56_Mask },
  /* 57 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f57 },
  /* 57    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f57_Mask },
  /* 58 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f58 },
  /* 58    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f58_Mask },
  /* 59 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f59 },
  /* 59    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f59_Mask },
  /* 5A k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5a },
  /* 5A    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5a_Mask },
  /* 5B k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5b },
  /* 5B    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5b_Mask },
  /* 5C k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5c },
  /* 5C    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5c_Mask },
  /* 5D k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5d },
  /* 5D    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5d_Mask },
  /* 5E k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5e },
  /* 5E    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5e_Mask },
  /* 5F k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5f },
  /* 5F    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f5f_Mask },
  /* 60 k0 */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKLBW_VdqHdqWdq },
  /* 60    */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKLBW_VdqHdqWdq_Kmask },
  /* 61 k0 */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKLWD_VdqHdqWdq },
  /* 61    */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKLWD_VdqHdqWdq_Kmask },
  /* 62 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPUNPCKLDQ_VdqHdqWdq },
  /* 62    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPUNPCKLDQ_VdqHdqWdq_Kmask },
  /* 63 k0 */ { BxPrefixSSE66, BX_IA_V512_VPACKSSWB_VdqHdqWdq },
  /* 63    */ { BxPrefixSSE66, BX_IA_V512_VPACKSSWB_VdqHdqWdq_Kmask },
  /* 64 k0 */ { BxPrefixSSE66, BX_IA_V512_VPCMPGTB_KGqHdqWdq },
  /* 64    */ { BxPrefixSSE66, BX_IA_V512_VPCMPGTB_KGqHdqWdq },
  /* 65 k0 */ { BxPrefixSSE66, BX_IA_V512_VPCMPGTW_KGdHdqWdq },
  /* 65    */ { BxPrefixSSE66, BX_IA_V512_VPCMPGTW_KGdHdqWdq },
  /* 66 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPCMPGTD_KGwHdqWdq },
  /* 66    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPCMPGTD_KGwHdqWdq },
  /* 67 k0 */ { BxPrefixSSE66, BX_IA_V512_VPACKUSWB_VdqHdqWdq },
  /* 67    */ { BxPrefixSSE66, BX_IA_V512_VPACKUSWB_VdqHdqWdq_Kmask },
  /* 68 k0 */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKHBW_VdqHdqWdq },
  /* 68    */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKHBW_VdqHdqWdq_Kmask },
  /* 69 k0 */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKHWD_VdqHdqWdq },
  /* 69    */ { BxPrefixSSE66, BX_IA_V512_VPUNPCKHWD_VdqHdqWdq_Kmask },
  /* 6A k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPUNPCKHDQ_VdqHdqWdq },
  /* 6A    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPUNPCKHDQ_VdqHdqWdq_Kmask },
  /* 6B k0 */ { BxPrefixSSE66, BX_IA_V512_VPACKSSDW_VdqHdqWdq },
  /* 6B    */ { BxPrefixSSE66, BX_IA_V512_VPACKSSDW_VdqHdqWdq_Kmask },
  /* 6C k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPUNPCKLQDQ_VdqHdqWdq },
  /* 6C    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPUNPCKLQDQ_VdqHdqWdq_Kmask },
  /* 6D k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPUNPCKHQDQ_VdqHdqWdq },
  /* 6D    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPUNPCKHQDQ_VdqHdqWdq_Kmask },
  /* 6E k0 */ { BxVexL0 | BxPrefixSSE66 | BxAliasVexW64, BX_IA_V512_VMOVD_VdqEd },
  /* 6E    */ { 0, BX_IA_ERROR }, // #UD
  /* 6F k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f6f },
  /* 6F    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f6f_Mask },
  /* 70 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f70 },
  /* 70    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f70_Mask },
  /* 71 k0 */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoEVEX_G12R },
  /* 71    */ { BxGroup12, BX_IA_ERROR, BxOpcodeInfoEVEX_G12R_Mask },
  /* 72 k0 */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoEVEX_G13R },
  /* 72    */ { BxGroup13, BX_IA_ERROR, BxOpcodeInfoEVEX_G13R_Mask },
  /* 73 k0 */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoEVEX_G14R },
  /* 73    */ { BxGroup14, BX_IA_ERROR, BxOpcodeInfoEVEX_G14R_Mask },
  /* 74 k0 */ { BxPrefixSSE66, BX_IA_V512_VPCMPEQB_KGqHdqWdq },
  /* 74    */ { BxPrefixSSE66, BX_IA_V512_VPCMPEQB_KGqHdqWdq },
  /* 75 k0 */ { BxPrefixSSE66, BX_IA_V512_VPCMPEQW_KGdHdqWdq },
  /* 75    */ { BxPrefixSSE66, BX_IA_V512_VPCMPEQW_KGdHdqWdq },
  /* 76 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPCMPEQD_KGwHdqWdq },
  /* 76    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPCMPEQD_KGwHdqWdq },
  /* 77 k0 */ { 0, BX_IA_ERROR },
  /* 77    */ { 0, BX_IA_ERROR },
  /* 78 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f78 },
  /* 78    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f78_Mask },
  /* 79 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0f79 },
  /* 79    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0f79_Mask },
  /* 7A k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7a },
  /* 7A    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7a_Mask },
  /* 7B k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7b },
  /* 7B    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VCVTPS2QQ_VdqWps_Kmask },
  /* 7C k0 */ { 0, BX_IA_ERROR },
  /* 7C    */ { 0, BX_IA_ERROR },
  /* 7D k0 */ { 0, BX_IA_ERROR },
  /* 7D    */ { 0, BX_IA_ERROR },
  /* 7E k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7e },
  /* 7E    */ { 0, BX_IA_ERROR }, // #UD
  /* 7F k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7f },
  /* 7F    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f7f_Mask },
  /* 80 k0 */ { 0, BX_IA_ERROR },
  /* 80    */ { 0, BX_IA_ERROR },
  /* 81 k0 */ { 0, BX_IA_ERROR },
  /* 81    */ { 0, BX_IA_ERROR },
  /* 82 k0 */ { 0, BX_IA_ERROR },
  /* 82    */ { 0, BX_IA_ERROR },
  /* 83 k0 */ { 0, BX_IA_ERROR },
  /* 83    */ { 0, BX_IA_ERROR },
  /* 84 k0 */ { 0, BX_IA_ERROR },
  /* 84    */ { 0, BX_IA_ERROR },
  /* 85 k0 */ { 0, BX_IA_ERROR },
  /* 85    */ { 0, BX_IA_ERROR },
  /* 86 k0 */ { 0, BX_IA_ERROR },
  /* 86    */ { 0, BX_IA_ERROR },
  /* 87 k0 */ { 0, BX_IA_ERROR },
  /* 87    */ { 0, BX_IA_ERROR },
  /* 88 k0 */ { 0, BX_IA_ERROR },
  /* 88    */ { 0, BX_IA_ERROR },
  /* 89 k0 */ { 0, BX_IA_ERROR },
  /* 89    */ { 0, BX_IA_ERROR },
  /* 8A k0 */ { 0, BX_IA_ERROR },
  /* 8A    */ { 0, BX_IA_ERROR },
  /* 8B k0 */ { 0, BX_IA_ERROR },
  /* 8B    */ { 0, BX_IA_ERROR },
  /* 8C k0 */ { 0, BX_IA_ERROR },
  /* 8C    */ { 0, BX_IA_ERROR },
  /* 8D k0 */ { 0, BX_IA_ERROR },
  /* 8D    */ { 0, BX_IA_ERROR },
  /* 8E k0 */ { 0, BX_IA_ERROR },
  /* 8E    */ { 0, BX_IA_ERROR },
  /* 8F k0 */ { 0, BX_IA_ERROR },
  /* 8F    */ { 0, BX_IA_ERROR },
  /* 90 k0 */ { 0, BX_IA_ERROR },
  /* 90    */ { 0, BX_IA_ERROR },
  /* 91 k0 */ { 0, BX_IA_ERROR },
  /* 91    */ { 0, BX_IA_ERROR },
  /* 92 k0 */ { 0, BX_IA_ERROR },
  /* 92    */ { 0, BX_IA_ERROR },
  /* 93 k0 */ { 0, BX_IA_ERROR },
  /* 93    */ { 0, BX_IA_ERROR },
  /* 94 k0 */ { 0, BX_IA_ERROR },
  /* 94    */ { 0, BX_IA_ERROR },
  /* 95 k0 */ { 0, BX_IA_ERROR },
  /* 95    */ { 0, BX_IA_ERROR },
  /* 96 k0 */ { 0, BX_IA_ERROR },
  /* 96    */ { 0, BX_IA_ERROR },
  /* 97 k0 */ { 0, BX_IA_ERROR },
  /* 97    */ { 0, BX_IA_ERROR },
  /* 98 k0 */ { 0, BX_IA_ERROR },
  /* 98    */ { 0, BX_IA_ERROR },
  /* 99 k0 */ { 0, BX_IA_ERROR },
  /* 99    */ { 0, BX_IA_ERROR },
  /* 9A k0 */ { 0, BX_IA_ERROR },
  /* 9A    */ { 0, BX_IA_ERROR },
  /* 9B k0 */ { 0, BX_IA_ERROR },
  /* 9B    */ { 0, BX_IA_ERROR },
  /* 9C k0 */ { 0, BX_IA_ERROR },
  /* 9C    */ { 0, BX_IA_ERROR },
  /* 9D k0 */ { 0, BX_IA_ERROR },
  /* 9D    */ { 0, BX_IA_ERROR },
  /* 9E k0 */ { 0, BX_IA_ERROR },
  /* 9E    */ { 0, BX_IA_ERROR },
  /* 9F k0 */ { 0, BX_IA_ERROR },
  /* 9F    */ { 0, BX_IA_ERROR },
  /* A0 k0 */ { 0, BX_IA_ERROR },
  /* A0    */ { 0, BX_IA_ERROR },
  /* A1 k0 */ { 0, BX_IA_ERROR },
  /* A1    */ { 0, BX_IA_ERROR },
  /* A2 k0 */ { 0, BX_IA_ERROR },
  /* A2    */ { 0, BX_IA_ERROR },
  /* A3 k0 */ { 0, BX_IA_ERROR },
  /* A3    */ { 0, BX_IA_ERROR },
  /* A4 k0 */ { 0, BX_IA_ERROR },
  /* A4    */ { 0, BX_IA_ERROR },
  /* A5 k0 */ { 0, BX_IA_ERROR },
  /* A5    */ { 0, BX_IA_ERROR },
  /* A6 k0 */ { 0, BX_IA_ERROR },
  /* A6    */ { 0, BX_IA_ERROR },
  /* A7 k0 */ { 0, BX_IA_ERROR },
  /* A7    */ { 0, BX_IA_ERROR },
  /* A8 k0 */ { 0, BX_IA_ERROR },
  /* A8    */ { 0, BX_IA_ERROR },
  /* A9 k0 */ { 0, BX_IA_ERROR },
  /* A9    */ { 0, BX_IA_ERROR },
  /* AA k0 */ { 0, BX_IA_ERROR },
  /* AA    */ { 0, BX_IA_ERROR },
  /* AB k0 */ { 0, BX_IA_ERROR },
  /* AB    */ { 0, BX_IA_ERROR },
  /* AC k0 */ { 0, BX_IA_ERROR },
  /* AC    */ { 0, BX_IA_ERROR },
  /* AD k0 */ { 0, BX_IA_ERROR },
  /* AD    */ { 0, BX_IA_ERROR },
  /* AE k0 */ { 0, BX_IA_ERROR },
  /* AE    */ { 0, BX_IA_ERROR },
  /* AF k0 */ { 0, BX_IA_ERROR },
  /* AF    */ { 0, BX_IA_ERROR },
  /* B0 k0 */ { 0, BX_IA_ERROR },
  /* B0    */ { 0, BX_IA_ERROR },
  /* B1 k0 */ { 0, BX_IA_ERROR },
  /* B1    */ { 0, BX_IA_ERROR },
  /* B2 k0 */ { 0, BX_IA_ERROR },
  /* B2    */ { 0, BX_IA_ERROR },
  /* B3 k0 */ { 0, BX_IA_ERROR },
  /* B3    */ { 0, BX_IA_ERROR },
  /* B4 k0 */ { 0, BX_IA_ERROR },
  /* B4    */ { 0, BX_IA_ERROR },
  /* B5 k0 */ { 0, BX_IA_ERROR },
  /* B5    */ { 0, BX_IA_ERROR },
  /* B6 k0 */ { 0, BX_IA_ERROR },
  /* B6    */ { 0, BX_IA_ERROR },
  /* B7 k0 */ { 0, BX_IA_ERROR },
  /* B7    */ { 0, BX_IA_ERROR },
  /* B8 k0 */ { 0, BX_IA_ERROR },
  /* B8    */ { 0, BX_IA_ERROR },
  /* B9 k0 */ { 0, BX_IA_ERROR },
  /* B9    */ { 0, BX_IA_ERROR },
  /* BA k0 */ { 0, BX_IA_ERROR },
  /* BA    */ { 0, BX_IA_ERROR },
  /* BB k0 */ { 0, BX_IA_ERROR },
  /* BB    */ { 0, BX_IA_ERROR },
  /* BC k0 */ { 0, BX_IA_ERROR },
  /* BC    */ { 0, BX_IA_ERROR },
  /* BD k0 */ { 0, BX_IA_ERROR },
  /* BD    */ { 0, BX_IA_ERROR },
  /* BE k0 */ { 0, BX_IA_ERROR },
  /* BE    */ { 0, BX_IA_ERROR },
  /* BF k0 */ { 0, BX_IA_ERROR },
  /* BF    */ { 0, BX_IA_ERROR },
  /* C0 k0 */ { 0, BX_IA_ERROR },
  /* C0    */ { 0, BX_IA_ERROR },
  /* C1 k0 */ { 0, BX_IA_ERROR },
  /* C1    */ { 0, BX_IA_ERROR },
  /* C2 k0 */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0fc2 },
  /* C2    */ { BxPrefixSSE4, BX_IA_ERROR, BxOpcodeGroupEVEX_0fc2 },
  /* C3 k0 */ { 0, BX_IA_ERROR },
  /* C3    */ { 0, BX_IA_ERROR },
  /* C4 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPINSRW_VdqEwIb },
  /* C4    */ { 0, BX_IA_ERROR }, // #UD
  /* C5 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPEXTRW_GdUdqIb },
  /* C5    */ { 0, BX_IA_ERROR }, // #UD
  /* C6 k0 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0fc6 },
  /* C6    */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupEVEX_0fc6_Mask },
  /* C7 k0 */ { 0, BX_IA_ERROR },
  /* C7    */ { 0, BX_IA_ERROR },
  /* C8 k0 */ { 0, BX_IA_ERROR },
  /* C8    */ { 0, BX_IA_ERROR },
  /* C9 k0 */ { 0, BX_IA_ERROR },
  /* C9    */ { 0, BX_IA_ERROR },
  /* CA k0 */ { 0, BX_IA_ERROR },
  /* CA    */ { 0, BX_IA_ERROR },
  /* CB k0 */ { 0, BX_IA_ERROR },
  /* CB    */ { 0, BX_IA_ERROR },
  /* CC k0 */ { 0, BX_IA_ERROR },
  /* CC    */ { 0, BX_IA_ERROR },
  /* CD k0 */ { 0, BX_IA_ERROR },
  /* CD    */ { 0, BX_IA_ERROR },
  /* CE k0 */ { 0, BX_IA_ERROR },
  /* CE    */ { 0, BX_IA_ERROR },
  /* CF k0 */ { 0, BX_IA_ERROR },
  /* CF    */ { 0, BX_IA_ERROR },
  /* D0 k0 */ { 0, BX_IA_ERROR },
  /* D0    */ { 0, BX_IA_ERROR },
  /* D1 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSRLW_VdqHdqWdq },
  /* D1    */ { BxPrefixSSE66, BX_IA_V512_VPSRLW_VdqHdqWdq_Kmask },
  /* D2 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSRLD_VdqHdqWdq },
  /* D2    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSRLD_VdqHdqWdq_Kmask },
  /* D3 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSRLQ_VdqHdqWdq },
  /* D3    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSRLQ_VdqHdqWdq_Kmask },
  /* D4 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPADDQ_VdqHdqWdq },
  /* D4    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPADDQ_VdqHdqWdq_Kmask },
  /* D5 k0 */ { BxPrefixSSE66, BX_IA_V512_VPMULLW_VdqHdqWdq },
  /* D5    */ { BxPrefixSSE66, BX_IA_V512_VPMULLW_VdqHdqWdq_Kmask },
  /* D6 k0 */ { BxVexL0 | BxVexW1 | BxPrefixSSE66, BX_IA_V512_VMOVQ_WqVq },
  /* D6    */ { 0, BX_IA_ERROR }, // #UD
  /* D7 k0 */ { 0, BX_IA_ERROR },
  /* D7    */ { 0, BX_IA_ERROR },
  /* D8 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBUSB_VdqHdqWdq },
  /* D8    */ { BxPrefixSSE66, BX_IA_V512_VPSUBUSB_VdqHdqWdq_Kmask },
  /* D9 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBUSW_VdqHdqWdq },
  /* D9    */ { BxPrefixSSE66, BX_IA_V512_VPSUBUSW_VdqHdqWdq_Kmask },
  /* DA k0 */ { BxPrefixSSE66, BX_IA_V512_VPMINUB_VdqHdqWdq },
  /* DA    */ { BxPrefixSSE66, BX_IA_V512_VPMINUB_VdqHdqWdq_Kmask },
  /* DB k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPANDD_VdqHdqWdq },
  /* DB    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPANDD_VdqHdqWdq_Kmask },
  /* EC k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDUSB_VdqHdqWdq },
  /* EC    */ { BxPrefixSSE66, BX_IA_V512_VPADDUSB_VdqHdqWdq_Kmask },
  /* ED k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDUSW_VdqHdqWdq },
  /* ED    */ { BxPrefixSSE66, BX_IA_V512_VPADDUSW_VdqHdqWdq_Kmask },
  /* DE k0 */ { BxPrefixSSE66, BX_IA_V512_VPMAXUB_VdqHdqWdq },
  /* DE    */ { BxPrefixSSE66, BX_IA_V512_VPMAXUB_VdqHdqWdq_Kmask },
  /* DF k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPANDND_VdqHdqWdq },
  /* DF    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPANDND_VdqHdqWdq_Kmask },
  /* E0 k0 */ { BxPrefixSSE66, BX_IA_V512_VPAVGB_VdqHdqWdq },
  /* E0    */ { BxPrefixSSE66, BX_IA_V512_VPAVGB_VdqHdqWdq_Kmask },
  /* E1 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSRAW_VdqHdqWdq },
  /* E1    */ { BxPrefixSSE66, BX_IA_V512_VPSRAW_VdqHdqWdq_Kmask },
  /* E2 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRAD_VdqHdqWdq },
  /* E2    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRAD_VdqHdqWdq_Kmask },
  /* E3 k0 */ { BxPrefixSSE66, BX_IA_V512_VPAVGW_VdqHdqWdq },
  /* E3    */ { BxPrefixSSE66, BX_IA_V512_VPAVGW_VdqHdqWdq_Kmask },
  /* E4 k0 */ { BxPrefixSSE66, BX_IA_V512_VPMULHUW_VdqHdqWdq },
  /* E4    */ { BxPrefixSSE66, BX_IA_V512_VPMULHUW_VdqHdqWdq_Kmask },
  /* E5 k0 */ { BxPrefixSSE66, BX_IA_V512_VPMULHW_VdqHdqWdq },
  /* E5    */ { BxPrefixSSE66, BX_IA_V512_VPMULHW_VdqHdqWdq_Kmask },
  /* E6 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0fe6 },
  /* E6    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0fe6_Mask },
  /* E7 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VMOVNTDQ_MdqVdq },
  /* E7    */ { 0, BX_IA_ERROR }, // #UD
  /* E8 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBSB_VdqHdqWdq },
  /* E8    */ { BxPrefixSSE66, BX_IA_V512_VPSUBSB_VdqHdqWdq_Kmask },
  /* E9 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBSW_VdqHdqWdq },
  /* E9    */ { BxPrefixSSE66, BX_IA_V512_VPSUBSW_VdqHdqWdq_Kmask },
  /* EA k0 */ { BxPrefixSSE66, BX_IA_V512_VPMINSW_VdqHdqWdq },
  /* EA    */ { BxPrefixSSE66, BX_IA_V512_VPMINSW_VdqHdqWdq_Kmask },
  /* EB k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPORD_VdqHdqWdq },
  /* EB    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPORD_VdqHdqWdq_Kmask },
  /* EC k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDSB_VdqHdqWdq },
  /* EC    */ { BxPrefixSSE66, BX_IA_V512_VPADDSB_VdqHdqWdq_Kmask },
  /* ED k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDSW_VdqHdqWdq },
  /* ED    */ { BxPrefixSSE66, BX_IA_V512_VPADDSW_VdqHdqWdq_Kmask },
  /* EE k0 */ { BxPrefixSSE66, BX_IA_V512_VPMAXSW_VdqHdqWdq },
  /* EE    */ { BxPrefixSSE66, BX_IA_V512_VPMAXSW_VdqHdqWdq_Kmask },
  /* EF k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPXORD_VdqHdqWdq },
  /* EF    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPXORD_VdqHdqWdq_Kmask },
  /* F0 k0 */ { 0, BX_IA_ERROR },
  /* F0    */ { 0, BX_IA_ERROR },
  /* F1 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSLLW_VdqHdqWdq },
  /* F1    */ { BxPrefixSSE66, BX_IA_V512_VPSLLW_VdqHdqWdq_Kmask },
  /* F2 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSLLD_VdqHdqWdq },
  /* F2    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSLLD_VdqHdqWdq_Kmask },
  /* F3 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSLLQ_VdqHdqWdq },
  /* F3    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSLLQ_VdqHdqWdq_Kmask },
  /* F4 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMULUDQ_VdqHdqWdq },
  /* F4    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMULUDQ_VdqHdqWdq_Kmask },
  /* F5 k0 */ { BxPrefixSSE66, BX_IA_V512_VPMADDWD_VdqHdqWdq },
  /* F5    */ { BxPrefixSSE66, BX_IA_V512_VPMADDWD_VdqHdqWdq_Kmask },
  /* F6 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSADBW_VdqHdqWdq },
  /* F6    */ { 0, BX_IA_ERROR }, // UD
  /* F7 k0 */ { 0, BX_IA_ERROR },
  /* F7    */ { 0, BX_IA_ERROR },
  /* F8 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBB_VdqHdqWdq },
  /* F8    */ { BxPrefixSSE66, BX_IA_V512_VPSUBB_VdqHdqWdq_Kmask },
  /* F9 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSUBW_VdqHdqWdq },
  /* F9    */ { BxPrefixSSE66, BX_IA_V512_VPSUBW_VdqHdqWdq_Kmask },
  /* FA k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSUBD_VdqHdqWdq },
  /* FA    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPSUBD_VdqHdqWdq_Kmask },
  /* FB k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSUBQ_VdqHdqWdq },
  /* FB    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPSUBQ_VdqHdqWdq_Kmask },
  /* FC k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDB_VdqHdqWdq },
  /* FC    */ { BxPrefixSSE66, BX_IA_V512_VPADDB_VdqHdqWdq_Kmask },
  /* FD k0 */ { BxPrefixSSE66, BX_IA_V512_VPADDW_VdqHdqWdq },
  /* FD    */ { BxPrefixSSE66, BX_IA_V512_VPADDW_VdqHdqWdq_Kmask },
  /* FE k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPADDD_VdqHdqWdq },
  /* FE    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPADDD_VdqHdqWdq_Kmask },
  /* FF k0 */ { 0, BX_IA_ERROR },
  /* FF    */ { 0, BX_IA_ERROR },

  // 256 entries for EVEX-encoded 0x0F 0x38 opcodes
  /* 00 k0 */ { BxPrefixSSE66, BX_IA_V512_VPSHUFB_VdqHdqWdq },
  /* 00    */ { BxPrefixSSE66, BX_IA_V512_VPSHUFB_VdqHdqWdq_Kmask },
  /* 01 k0 */ { 0, BX_IA_ERROR },
  /* 01    */ { 0, BX_IA_ERROR },
  /* 02 k0 */ { 0, BX_IA_ERROR },
  /* 02    */ { 0, BX_IA_ERROR },
  /* 03 k0 */ { 0, BX_IA_ERROR },
  /* 03    */ { 0, BX_IA_ERROR },
  /* 04 k0 */ { BxPrefixSSE66, BX_IA_V512_VPMADDUBSW_VdqHdqWdq },
  /* 04    */ { BxPrefixSSE66, BX_IA_V512_VPMADDUBSW_VdqHdqWdq_Kmask },
  /* 05 k0 */ { 0, BX_IA_ERROR },
  /* 05    */ { 0, BX_IA_ERROR },
  /* 06 k0 */ { 0, BX_IA_ERROR },
  /* 06    */ { 0, BX_IA_ERROR },
  /* 07 k0 */ { 0, BX_IA_ERROR },
  /* 07    */ { 0, BX_IA_ERROR },
  /* 08 k0 */ { 0, BX_IA_ERROR },
  /* 08    */ { 0, BX_IA_ERROR },
  /* 09 k0 */ { 0, BX_IA_ERROR },
  /* 09    */ { 0, BX_IA_ERROR },
  /* 0A k0 */ { 0, BX_IA_ERROR },
  /* 0A    */ { 0, BX_IA_ERROR },
  /* 0B k0 */ { BxPrefixSSE66, BX_IA_V512_VPMULHRSW_VdqHdqWdq },
  /* 0B    */ { BxPrefixSSE66, BX_IA_V512_VPMULHRSW_VdqHdqWdq_Kmask },
  /* 0C k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPERMILPS_VpsHpsWps },
  /* 0C    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPERMILPS_VpsHpsWps_Kmask },
  /* 0D k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPERMILPD_VpdHpdWpd },
  /* 0D    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPERMILPD_VpdHpdWpd_Kmask },
  /* 0E k0 */ { 0, BX_IA_ERROR },
  /* 0E    */ { 0, BX_IA_ERROR },
  /* 0F k0 */ { 0, BX_IA_ERROR },
  /* 0F    */ { 0, BX_IA_ERROR },
  /* 10 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3810 },
  /* 10    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3810_Mask },
  /* 11 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3811 },
  /* 11    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3811_Mask },
  /* 12 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3812 },
  /* 12    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3812_Mask },
  /* 13 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3813 },
  /* 13    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3813_Mask },
  /* 14 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3814 },
  /* 14    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3814_Mask },
  /* 15 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3815 },
  /* 15    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3815_Mask },
  /* 16 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxVexL1, BX_IA_V512_VPERMPS_VpsHpsWps_Kmask },
  /* 16    */ { BxAliasVexW | BxPrefixSSE66 | BxVexL1, BX_IA_V512_VPERMPS_VpsHpsWps_Kmask },
  /* 17 k0 */ { 0, BX_IA_ERROR },
  /* 17    */ { 0, BX_IA_ERROR },
  /* 18 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VBROADCASTSS_VpsWss },
  /* 18    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VBROADCASTSS_VpsWss_Kmask },
  /* 19 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x2_VpsWq },
  /* 19    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x2_VpsWq_Kmask },
  /* 1A k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x4_VpsWps },
  /* 1A    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x4_VpsWps_Kmask },
  /* 1B k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x8_VpsWps },
  /* 1B    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTF32x8_VpsWps_Kmask },
  /* 1C k0 */ { BxPrefixSSE66, BX_IA_V512_VPABSB_VdqWdq },
  /* 1C    */ { BxPrefixSSE66, BX_IA_V512_VPABSB_VdqWdq_Kmask },
  /* 1D k0 */ { BxPrefixSSE66, BX_IA_V512_VPABSW_VdqWdq },
  /* 1D    */ { BxPrefixSSE66, BX_IA_V512_VPABSW_VdqWdq_Kmask },
  /* 1E k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPABSD_VdqWdq },
  /* 1E    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPABSD_VdqWdq_Kmask },
  /* 1F k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPABSQ_VdqWdq },
  /* 1F    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPABSQ_VdqWdq_Kmask },
  /* 20 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3820 },
  /* 20    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3820_Mask },
  /* 21 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3821 },
  /* 21    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3821_Mask },
  /* 22 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3822 },
  /* 22    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3822_Mask },
  /* 23 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3823 },
  /* 23    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3823_Mask },
  /* 24 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3824 },
  /* 24    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3824_Mask },
  /* 25 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3825 },
  /* 25    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3825_Mask },
  /* 26 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3826 },
  /* 26    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3826 },
  /* 27 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3827 },
  /* 27    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3827 },
  /* 28 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3828 },
  /* 28    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMULDQ_VdqHdqWdq_Kmask },
  /* 29 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3829 },
  /* 29    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPCMPEQQ_KGbHdqWdq },
  /* 2A k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f382a },
  /* 2A    */ { 0, BX_IA_ERROR }, // #UD
  /* 2B k0 */ { BxPrefixSSE66, BX_IA_V512_VPACKUSDW_VdqHdqWdq },
  /* 2B    */ { BxPrefixSSE66, BX_IA_V512_VPACKUSDW_VdqHdqWdq_Kmask },
  /* 2C k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCALEFPS_VpsHpsWps },
  /* 2C    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCALEFPS_VpsHpsWps_Kmask },
  /* 2D k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCALEFSS_VssHpsWss },
  /* 2D    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCALEFSS_VssHpsWss_Kmask },
  /* 2E k0 */ { 0, BX_IA_ERROR },
  /* 2E    */ { 0, BX_IA_ERROR },
  /* 2F k0 */ { 0, BX_IA_ERROR },
  /* 2F    */ { 0, BX_IA_ERROR },
  /* 30 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3830 },
  /* 30    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3830_Mask },
  /* 31 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3831 },
  /* 31    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3831_Mask },
  /* 32 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3832 },
  /* 32    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3832_Mask },
  /* 33 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3833 },
  /* 33    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3833_Mask },
  /* 34 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3834 },
  /* 34    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3834_Mask },
  /* 35 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3835 },
  /* 35    */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3835_Mask },
  /* 36 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxVexL1, BX_IA_V512_VPERMD_VdqHdqWdq_Kmask },
  /* 36    */ { BxAliasVexW | BxPrefixSSE66 | BxVexL1, BX_IA_V512_VPERMD_VdqHdqWdq_Kmask },
  /* 37 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPCMPGTQ_KGbHdqWdq },
  /* 37    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPCMPGTQ_KGbHdqWdq },
  /* 38 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3838 },
  /* 38    */ { BxPrefixSSE66, BX_IA_V512_VPMINSB_VdqHdqWdq_Kmask },
  /* 39 k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f3839 },
  /* 39    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMINSD_VdqHdqWdq_Kmask },
  /* 3A k0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupEVEX_0f383a },
  /* 3A    */ { BxPrefixSSE66, BX_IA_V512_VPMINUW_VdqHdqWdq_Kmask },
  /* 3B k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMINUD_VdqHdqWdq },
  /* 3B    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMINUD_VdqHdqWdq_Kmask },
  /* 3C k0 */ { BxPrefixSSE66, BX_IA_V512_VPMAXSB_VdqHdqWdq },
  /* 3C    */ { BxPrefixSSE66, BX_IA_V512_VPMAXSB_VdqHdqWdq_Kmask },
  /* 3D k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMAXSD_VdqHdqWdq },
  /* 3D    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMAXSD_VdqHdqWdq_Kmask },
  /* 3E k0 */ { BxPrefixSSE66, BX_IA_V512_VPMAXUW_VdqHdqWdq },
  /* 3E    */ { BxPrefixSSE66, BX_IA_V512_VPMAXUW_VdqHdqWdq_Kmask },
  /* 3F k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMAXUD_VdqHdqWdq },
  /* 3F    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMAXUD_VdqHdqWdq_Kmask },
  /* 40 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMULLD_VdqHdqWdq },
  /* 40    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPMULLD_VdqHdqWdq_Kmask },
  /* 41 k0 */ { 0, BX_IA_ERROR },
  /* 41    */ { 0, BX_IA_ERROR },
  /* 42 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGETEXPPS_VpsWps_Kmask },
  /* 42    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGETEXPPS_VpsWps_Kmask },
  /* 43 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGETEXPSS_VssHpsWss_Kmask },
  /* 43    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGETEXPSS_VssHpsWss_Kmask },
  /* 44 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPLZCNTD_VdqWdq_Kmask },
  /* 44    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPLZCNTD_VdqWdq_Kmask },
  /* 45 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRLVD_VdqHdqWdq },
  /* 45    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRLVD_VdqHdqWdq_Kmask },
  /* 46 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRAVD_VdqHdqWdq },
  /* 46    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSRAVD_VdqHdqWdq_Kmask },
  /* 47 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSLLVD_VdqHdqWdq },
  /* 47    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPSLLVD_VdqHdqWdq_Kmask },
  /* 48 k0 */ { 0, BX_IA_ERROR },
  /* 48    */ { 0, BX_IA_ERROR },
  /* 49 k0 */ { 0, BX_IA_ERROR },
  /* 49    */ { 0, BX_IA_ERROR },
  /* 4A k0 */ { 0, BX_IA_ERROR },
  /* 4A    */ { 0, BX_IA_ERROR },
  /* 4B k0 */ { 0, BX_IA_ERROR },
  /* 4B    */ { 0, BX_IA_ERROR },
  /* 4C k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRCP14PS_VpsWps_Kmask },
  /* 4C    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRCP14PS_VpsWps_Kmask },
  /* 4D k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRCP14SS_VssHpsWss_Kmask },
  /* 4D    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRCP14SS_VssHpsWss_Kmask },
  /* 4E k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRSQRT14PS_VpsWps_Kmask },
  /* 4E    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRSQRT14PS_VpsWps_Kmask },
  /* 4F k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRSQRT14SS_VssHpsWss_Kmask },
  /* 4F    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VRSQRT14SS_VssHpsWss_Kmask },
  /* 50 k0 */ { 0, BX_IA_ERROR },
  /* 50    */ { 0, BX_IA_ERROR },
  /* 51 k0 */ { 0, BX_IA_ERROR },
  /* 51    */ { 0, BX_IA_ERROR },
  /* 52 k0 */ { 0, BX_IA_ERROR },
  /* 52    */ { 0, BX_IA_ERROR },
  /* 53 k0 */ { 0, BX_IA_ERROR },
  /* 53    */ { 0, BX_IA_ERROR },
  /* 54 k0 */ { 0, BX_IA_ERROR },
  /* 54    */ { 0, BX_IA_ERROR },
  /* 55 k0 */ { 0, BX_IA_ERROR },
  /* 55    */ { 0, BX_IA_ERROR },
  /* 56 k0 */ { 0, BX_IA_ERROR },
  /* 56    */ { 0, BX_IA_ERROR },
  /* 57 k0 */ { 0, BX_IA_ERROR },
  /* 57    */ { 0, BX_IA_ERROR },
  /* 58 k0 */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPBROADCASTD_VdqWd },
  /* 58    */ { BxVexW0 | BxPrefixSSE66, BX_IA_V512_VPBROADCASTD_VdqWd_Kmask },
  /* 59 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x2_VdqWq },
  /* 59    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x2_VdqWq_Kmask },
  /* 5A k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x4_VdqWdq },
  /* 5A    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x4_VdqWdq_Kmask },
  /* 5B k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x8_VdqWdq },
  /* 5B    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66, BX_IA_V512_VBROADCASTI32x8_VdqWdq_Kmask },
  /* 5C k0 */ { 0, BX_IA_ERROR },
  /* 5C    */ { 0, BX_IA_ERROR },
  /* 5D k0 */ { 0, BX_IA_ERROR },
  /* 5D    */ { 0, BX_IA_ERROR },
  /* 5E k0 */ { 0, BX_IA_ERROR },
  /* 5E    */ { 0, BX_IA_ERROR },
  /* 5F k0 */ { 0, BX_IA_ERROR },
  /* 5F    */ { 0, BX_IA_ERROR },
  /* 60 k0 */ { 0, BX_IA_ERROR },
  /* 60    */ { 0, BX_IA_ERROR },
  /* 61 k0 */ { 0, BX_IA_ERROR },
  /* 61    */ { 0, BX_IA_ERROR },
  /* 62 k0 */ { 0, BX_IA_ERROR },
  /* 62    */ { 0, BX_IA_ERROR },
  /* 63 k0 */ { 0, BX_IA_ERROR },
  /* 63    */ { 0, BX_IA_ERROR },
  /* 64 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPBLENDMD_VdqHdqWdq },
  /* 64    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPBLENDMD_VdqHdqWdq },
  /* 65 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBLENDMPS_VpsHpsWps },
  /* 65    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VBLENDMPS_VpsHpsWps },
  /* 66 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPBLENDMB_VdqHdqWdq },
  /* 66    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPBLENDMB_VdqHdqWdq },
  /* 67 k0 */ { 0, BX_IA_ERROR },
  /* 67    */ { 0, BX_IA_ERROR },
  /* 68 k0 */ { 0, BX_IA_ERROR },
  /* 68    */ { 0, BX_IA_ERROR },
  /* 69 k0 */ { 0, BX_IA_ERROR },
  /* 69    */ { 0, BX_IA_ERROR },
  /* 6A k0 */ { 0, BX_IA_ERROR },
  /* 6A    */ { 0, BX_IA_ERROR },
  /* 6B k0 */ { 0, BX_IA_ERROR },
  /* 6B    */ { 0, BX_IA_ERROR },
  /* 6C k0 */ { 0, BX_IA_ERROR },
  /* 6C    */ { 0, BX_IA_ERROR },
  /* 6D k0 */ { 0, BX_IA_ERROR },
  /* 6D    */ { 0, BX_IA_ERROR },
  /* 6E k0 */ { 0, BX_IA_ERROR },
  /* 6E    */ { 0, BX_IA_ERROR },
  /* 6F k0 */ { 0, BX_IA_ERROR },
  /* 6F    */ { 0, BX_IA_ERROR },
  /* 70 k0 */ { 0, BX_IA_ERROR },
  /* 70    */ { 0, BX_IA_ERROR },
  /* 71 k0 */ { 0, BX_IA_ERROR },
  /* 71    */ { 0, BX_IA_ERROR },
  /* 72 k0 */ { 0, BX_IA_ERROR },
  /* 72    */ { 0, BX_IA_ERROR },
  /* 73 k0 */ { 0, BX_IA_ERROR },
  /* 73    */ { 0, BX_IA_ERROR },
  /* 74 k0 */ { 0, BX_IA_ERROR },
  /* 74    */ { 0, BX_IA_ERROR },
  /* 75 k0 */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMI2W_VdqHdqWdq_Kmask },
  /* 75    */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMI2W_VdqHdqWdq_Kmask },
  /* 76 k0 */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMI2D_VdqHdqWdq_Kmask },
  /* 76    */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMI2D_VdqHdqWdq_Kmask },
  /* 77 k0 */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMI2PS_VpsHpsWps_Kmask },
  /* 77    */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMI2PS_VpsHpsWps_Kmask },
  /* 78 k0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTB_VdqWb },
  /* 78    */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTB_VdqWb_Kmask },
  /* 79 k0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTW_VdqWw },
  /* 79    */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTW_VdqWw_Kmask },
  /* 7A k0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTB_VdqEb },
  /* 7A    */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTB_VdqEb_Kmask },
  /* 7B k0 */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTW_VdqEw },
  /* 7B    */ { BxPrefixSSE66 | BxVexW0, BX_IA_V512_VPBROADCASTW_VdqEw_Kmask },
  /* 7C k0 */ { BxPrefixSSE66 | BxAliasVexW64, BX_IA_V512_VPBROADCASTD_VdqEd },
  /* 7C    */ { BxPrefixSSE66 | BxAliasVexW64, BX_IA_V512_VPBROADCASTD_VdqEd_Kmask },
  /* 7D k0 */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMT2W_VdqHdqWdq_Kmask },
  /* 7D    */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMT2W_VdqHdqWdq_Kmask },
  /* 7E k0 */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMT2D_VdqHdqWdq_Kmask },
  /* 7E    */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMT2D_VdqHdqWdq_Kmask },
  /* 7F k0 */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMT2PS_VpsHpsWps_Kmask },
  /* 7F    */ { BxPrefixSSE66 | BxAliasVexW, BX_IA_V512_VPERMT2PS_VpsHpsWps_Kmask },
  /* 80 k0 */ { 0, BX_IA_ERROR },
  /* 80    */ { 0, BX_IA_ERROR },
  /* 81 k0 */ { 0, BX_IA_ERROR },
  /* 81    */ { 0, BX_IA_ERROR },
  /* 82 k0 */ { 0, BX_IA_ERROR },
  /* 82    */ { 0, BX_IA_ERROR },
  /* 83 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMULTISHIFTQB_VdqHdqWdq },
  /* 83    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMULTISHIFTQB_VdqHdqWdq_Kmask },
  /* 84 k0 */ { 0, BX_IA_ERROR },
  /* 84    */ { 0, BX_IA_ERROR },
  /* 85 k0 */ { 0, BX_IA_ERROR },
  /* 85    */ { 0, BX_IA_ERROR },
  /* 86 k0 */ { 0, BX_IA_ERROR },
  /* 86    */ { 0, BX_IA_ERROR },
  /* 87 k0 */ { 0, BX_IA_ERROR },
  /* 87    */ { 0, BX_IA_ERROR },
  /* 88 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VEXPANDPS_VpsWps },
  /* 88    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VEXPANDPS_VpsWps_Kmask },
  /* 89 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VEXPANDD_VdqWdq },
  /* 89    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VEXPANDD_VdqWdq_Kmask },
  /* 8A k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VCOMPRESSPS_WpsVps },
  /* 8A    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VCOMPRESSPS_WpsVps_Kmask },
  /* 8B k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VCOMPRESSD_WdqVdq },
  /* 8B    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VCOMPRESSD_WdqVdq_Kmask },
  /* 8C k0 */ { 0, BX_IA_ERROR },
  /* 8C    */ { 0, BX_IA_ERROR },
  /* 8D k0 */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMW_VdqHdqWdq_Kmask },
  /* 8D    */ { BxPrefixSSE66 | BxVexW1, BX_IA_V512_VPERMW_VdqHdqWdq_Kmask },
  /* 8E k0 */ { 0, BX_IA_ERROR },
  /* 8E    */ { 0, BX_IA_ERROR },
  /* 8F k0 */ { 0, BX_IA_ERROR },
  /* 8F    */ { 0, BX_IA_ERROR },
  /* 90 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* 90    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGATHERDD_VdqVSib },
  /* 91 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* 91    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGATHERQD_VdqVSib },
  /* 92 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* 92    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGATHERDPS_VpsVSib },
  /* 93 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* 93    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VGATHERQPS_VpsVSib },
  /* 94 k0 */ { 0, BX_IA_ERROR },
  /* 94    */ { 0, BX_IA_ERROR },
  /* 95 k0 */ { 0, BX_IA_ERROR },
  /* 95    */ { 0, BX_IA_ERROR },
  /* 96 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB132PS_VpsHpsWps },
  /* 96    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB132PS_VpsHpsWps_Kmask },
  /* 97 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD132PS_VpsHpsWps },
  /* 97    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD132PS_VpsHpsWps_Kmask },
  /* 98 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD132PS_VpsHpsWps },
  /* 98    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD132PS_VpsHpsWps_Kmask },
  /* 99 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD132SS_VpsHssWss },
  /* 99    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD132SS_VpsHssWss_Kmask },
  /* 9A k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB132PS_VpsHpsWps },
  /* 9A    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB132PS_VpsHpsWps_Kmask },
  /* 9B k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB132SS_VpsHssWss },
  /* 9B    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB132SS_VpsHssWss_Kmask },
  /* 9C k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD132PS_VpsHpsWps },
  /* 9C    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD132PS_VpsHpsWps_Kmask },
  /* 9D k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD132SS_VpsHssWss },
  /* 9D    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD132SS_VpsHssWss_Kmask },
  /* 9E k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB132PS_VpsHpsWps },
  /* 9E    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB132PS_VpsHpsWps_Kmask },
  /* 9F k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB132SS_VpsHssWss },
  /* 9F    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB132SS_VpsHssWss_Kmask },
  /* A0 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* A0    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCATTERDD_VSibVdq },
  /* A1 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* A1    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCATTERQD_VSibVdq },
  /* A2 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* A2    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCATTERDPS_VSibVps },
  /* A3 k0 */ { 0, BX_IA_ERROR }, // #UD
  /* A3    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VSCATTERQPS_VSibVps },
  /* A4 k0 */ { 0, BX_IA_ERROR },
  /* A4    */ { 0, BX_IA_ERROR },
  /* A5 k0 */ { 0, BX_IA_ERROR },
  /* A5    */ { 0, BX_IA_ERROR },
  /* A6 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB213PS_VpsHpsWps },
  /* A6    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB213PS_VpsHpsWps_Kmask },
  /* A7 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD213PS_VpsHpsWps },
  /* A7    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD213PS_VpsHpsWps_Kmask },
  /* A8 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD213PS_VpsHpsWps },
  /* A8    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD213PS_VpsHpsWps_Kmask },
  /* A9 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD213SS_VpsHssWss },
  /* A9    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD213SS_VpsHssWss_Kmask },
  /* AA k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB213PS_VpsHpsWps },
  /* AA    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB213PS_VpsHpsWps_Kmask },
  /* AB k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB213SS_VpsHssWss },
  /* AB    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB213SS_VpsHssWss_Kmask },
  /* AC k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD213PS_VpsHpsWps },
  /* AC    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD213PS_VpsHpsWps_Kmask },
  /* AD k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD213SS_VpsHssWss },
  /* AD    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD213SS_VpsHssWss_Kmask },
  /* AE k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB213PS_VpsHpsWps },
  /* AE    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB213PS_VpsHpsWps_Kmask },
  /* AF k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB213SS_VpsHssWss },
  /* AF    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB213SS_VpsHssWss_Kmask },
  /* B0 k0 */ { 0, BX_IA_ERROR },
  /* B0    */ { 0, BX_IA_ERROR },
  /* B1 k0 */ { 0, BX_IA_ERROR },
  /* B1    */ { 0, BX_IA_ERROR },
  /* B2 k0 */ { 0, BX_IA_ERROR },
  /* B2    */ { 0, BX_IA_ERROR },
  /* B3 k0 */ { 0, BX_IA_ERROR },
  /* B3    */ { 0, BX_IA_ERROR },
  /* B4 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMADD52LUQ_VdqHdqWdq },
  /* B4    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMADD52LUQ_VdqHdqWdq_Kmask },
  /* B5 k0 */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMADD52HUQ_VdqHdqWdq },
  /* B5    */ { BxVexW1 | BxPrefixSSE66, BX_IA_V512_VPMADD52HUQ_VdqHdqWdq_Kmask },
  /* B6 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB231PS_VpsHpsWps },
  /* B6    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADDSUB231PS_VpsHpsWps_Kmask },
  /* B7 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD231PS_VpsHpsWps },
  /* B7    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUBADD231PS_VpsHpsWps_Kmask },
  /* B8 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD231PS_VpsHpsWps },
  /* B8    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD231PS_VpsHpsWps_Kmask },
  /* B9 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD231SS_VpsHssWss },
  /* B9    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMADD231SS_VpsHssWss_Kmask },
  /* BA k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB231PS_VpsHpsWps },
  /* BA    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB231PS_VpsHpsWps_Kmask },
  /* BB k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB231SS_VpsHssWss },
  /* BB    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFMSUB231SS_VpsHssWss_Kmask },
  /* BC k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD231PS_VpsHpsWps },
  /* BC    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD231PS_VpsHpsWps_Kmask },
  /* BD k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD231SS_VpsHssWss },
  /* BD    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMADD231SS_VpsHssWss_Kmask },
  /* BE k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB231PS_VpsHpsWps },
  /* BE    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB231PS_VpsHpsWps_Kmask },
  /* BF k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB231SS_VpsHssWss },
  /* BF    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VFNMSUB231SS_VpsHssWss_Kmask },
  /* C0 k0 */ { 0, BX_IA_ERROR },
  /* C0    */ { 0, BX_IA_ERROR },
  /* C1 k0 */ { 0, BX_IA_ERROR },
  /* C1    */ { 0, BX_IA_ERROR },
  /* C2 k0 */ { 0, BX_IA_ERROR },
  /* C2    */ { 0, BX_IA_ERROR },
  /* C3 k0 */ { 0, BX_IA_ERROR },
  /* C3    */ { 0, BX_IA_ERROR },
  /* C4 k0 */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPCONFLICTD_VdqWdq_Kmask },
  /* C4    */ { BxAliasVexW | BxPrefixSSE66, BX_IA_V512_VPCONFLICTD_VdqWdq_Kmask },
  /* C5 k0 */ { 0, BX_IA_ERROR },
  /* C5    */ { 0, BX_IA_ERROR },
  /* C6 k0 */ { 0, BX_IA_ERROR },
  /* C6    */ { 0, BX_IA_ERROR },
  /* C7 k0 */ { 0, BX_IA_ERROR },
  /* C7    */ { 0, BX_IA_ERROR },
  /* C8 k0 */ { 0, BX_IA_ERROR },
  /* C8    */ { 0, BX_IA_ERROR },
  /* C9 k0 */ { 0, BX_IA_ERROR },
  /* C9    */ { 0, BX_IA_ERROR },
  /* CA k0 */ { 0, BX_IA_ERROR },
  /* CA    */ { 0, BX_IA_ERROR },
  /* CB k0 */ { 0, BX_IA_ERROR },
  /* CB    */ { 0, BX_IA_ERROR },
  /* CC k0 */ { 0, BX_IA_ERROR },
  /* CC    */ { 0, BX_IA_ERROR },
  /* CD k0 */ { 0, BX_IA_ERROR },
  /* CD    */ { 0, BX_IA_ERROR },
  /* CE k0 */ { 0, BX_IA_ERROR },
  /* CE    */ { 0, BX_IA_ERROR },
  /* CF k0 */ { 0, BX_IA_ERROR },
  /* CF    */ { 0, BX_IA_ERROR },
  /* D0 k0 */ { 0, BX_IA_ERROR },
  /* D0    */ { 0, BX_IA_ERROR },
  /* D1 k0 */ { 0, BX_IA_ERROR },
  /* D1    */ { 0, BX_IA_ERROR },
  /* D2 k0 */ { 0, BX_IA_ERROR },
  /* D2    */ { 0, BX_IA_ERROR },
  /* D3 k0 */ { 0, BX_IA_ERROR },
  /* D3    */ { 0, BX_IA_ERROR },
  /* D4 k0 */ { 0, BX_IA_ERROR },
  /* D4    */ { 0, BX_IA_ERROR },
  /* D5 k0 */ { 0, BX_IA_ERROR },
  /* D5    */ { 0, BX_IA_ERROR },
  /* D6 k0 */ { 0, BX_IA_ERROR },
  /* D6    */ { 0, BX_IA_ERROR },
  /* D7 k0 */ { 0, BX_IA_ERROR },
  /* D7    */ { 0, BX_IA_ERROR },
  /* D8 k0 */ { 0, BX_IA_ERROR },
  /* D8    */ { 0, BX_IA_ERROR },
  /* D9 k0 */ { 0, BX_IA_ERROR },
  /* D9    */ { 0, BX_IA_ERROR },
  /* DA k0 */ { 0, BX_IA_ERROR },
  /* DA    */ { 0, BX_IA_ERROR },
  /* DB k0 */ { 0, BX_IA_ERROR },
  /* DB    */ { 0, BX_IA_ERROR },
  /* DC k0 */ { 0, BX_IA_ERROR },
  /* DC    */ { 0, BX_IA_ERROR },
  /* DD k0 */ { 0, BX_IA_ERROR },
  /* DD    */ { 0, BX_IA_ERROR },
  /* DE k0 */ { 0, BX_IA_ERROR },
  /* DE    */ { 0, BX_IA_ERROR },
  /* DF k0 */ { 0, BX_IA_ERROR },
  /* DF    */ { 0, BX_IA_ERROR },
  /* E0 k0 */ { 0, BX_IA_ERROR },
  /* E0    */ { 0, BX_IA_ERROR },
  /* E1 k0 */ { 0, BX_IA_ERROR },
  /* E1    */ { 0, BX_IA_ERROR },
  /* E2 k0 */ { 0, BX_IA_ERROR },
  /* E2    */ { 0, BX_IA_ERROR },
  /* E3 k0 */ { 0, BX_IA_ERROR },
  /* E3    */ { 0, BX_IA_ERROR },
  /* E4 k0 */ { 0, BX_IA_ERROR },
  /* E4    */ { 0, BX_IA_ERROR },
  /* E5 k0 */ { 0, BX_IA_ERROR },
  /* E5    */ { 0, BX_IA_ERROR },
  /* E6 k0 */ { 0, BX_IA_ERROR },
  /* E6    */ { 0, BX_IA_ERROR },
  /* E7 k0 */ { 0, BX_IA_ERROR },
  /* E7    */ { 0, BX_IA_ERROR },
  /* E8 k0 */ { 0, BX_IA_ERROR },
  /* E8    */ { 0, BX_IA_ERROR },
  /* E9 k0 */ { 0, BX_IA_ERROR },
  /* E9    */ { 0, BX_IA_ERROR },
  /* EA k0 */ { 0, BX_IA_ERROR },
  /* EA    */ { 0, BX_IA_ERROR },
  /* EB k0 */ { 0, BX_IA_ERROR },
  /* EB    */ { 0, BX_IA_ERROR },
  /* EC k0 */ { 0, BX_IA_ERROR },
  /* EC    */ { 0, BX_IA_ERROR },
  /* ED k0 */ { 0, BX_IA_ERROR },
  /* ED    */ { 0, BX_IA_ERROR },
  /* EE k0 */ { 0, BX_IA_ERROR },
  /* EE    */ { 0, BX_IA_ERROR },
  /* EF k0 */ { 0, BX_IA_ERROR },
  /* EF    */ { 0, BX_IA_ERROR },
  /* F0 k0 */ { 0, BX_IA_ERROR },
  /* F0    */ { 0, BX_IA_ERROR },
  /* F1 k0 */ { 0, BX_IA_ERROR },
  /* F1    */ { 0, BX_IA_ERROR },
  /* F2 k0 */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
  /* F3 k0 */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F4 k0 */ { 0, BX_IA_ERROR },
  /* F4    */ { 0, BX_IA_ERROR },
  /* F5 k0 */ { 0, BX_IA_ERROR },
  /* F5    */ { 0, BX_IA_ERROR },
  /* F6 k0 */ { 0, BX_IA_ERROR },
  /* F6    */ { 0, BX_IA_ERROR },
  /* F7 k0 */ { 0, BX_IA_ERROR },
  /* F7    */ { 0, BX_IA_ERROR },
  /* F8 k0 */ { 0, BX_IA_ERROR },
  /* F8    */ { 0, BX_IA_ERROR },
  /* F9 k0 */ { 0, BX_IA_ERROR },
  /* F9    */ { 0, BX_IA_ERROR },
  /* FA k0 */ { 0, BX_IA_ERROR },
  /* FA    */ { 0, BX_IA_ERROR },
  /* FB k0 */ { 0, BX_IA_ERROR },
  /* FB    */ { 0, BX_IA_ERROR },
  /* FC k0 */ { 0, BX_IA_ERROR },
  /* FC    */ { 0, BX_IA_ERROR },
  /* FD k0 */ { 0, BX_IA_ERROR },
  /* FD    */ { 0, BX_IA_ERROR },
  /* FE k0 */ { 0, BX_IA_ERROR },
  /* FE    */ { 0, BX_IA_ERROR },
  /* FF k0 */ { 0, BX_IA_ERROR },
  /* FF    */ { 0, BX_IA_ERROR },

  // 256 entries for EVEX-encoded 0x0F 0x3A opcodes
  /* 00 k0 */ { BxVexW1 | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMQ_VdqWdqIb_Kmask },
  /* 00    */ { BxVexW1 | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMQ_VdqWdqIb_Kmask },
  /* 01 k0 */ { BxVexW1 | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMPD_VpdWpdIb_Kmask },
  /* 01    */ { BxVexW1 | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMPD_VpdWpdIb_Kmask },
  /* 02 k0 */ { 0, BX_IA_ERROR },
  /* 02    */ { 0, BX_IA_ERROR },
  /* 03 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VALIGND_VdqHdqWdqIb_Kmask },
  /* 03    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VALIGND_VdqHdqWdqIb_Kmask },
  /* 04 k0 */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMILPS_VpsWpsIb },
  /* 04    */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMILPS_VpsWpsIb_Kmask },
  /* 05 k0 */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMILPD_VpdWpdIb },
  /* 05    */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPERMILPD_VpdWpdIb_Kmask },
  /* 06 k0 */ { 0, BX_IA_ERROR },
  /* 06    */ { 0, BX_IA_ERROR },
  /* 07 k0 */ { 0, BX_IA_ERROR },
  /* 07    */ { 0, BX_IA_ERROR },
  /* 08 k0 */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALEPS_VpsWpsIb_Kmask },
  /* 08    */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALEPS_VpsWpsIb_Kmask },
  /* 09 k0 */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALEPD_VpdWpdIb_Kmask },
  /* 09    */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALEPD_VpdWpdIb_Kmask },
  /* 0A k0 */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALESS_VssHpsWssIb_Kmask },
  /* 0A    */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALESS_VssHpsWssIb_Kmask },
  /* 0B k0 */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALESD_VsdHpdWsdIb_Kmask },
  /* 0B    */ { BxVexW1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRNDSCALESD_VsdHpdWsdIb_Kmask },
  /* 0C k0 */ { 0, BX_IA_ERROR },
  /* 0C    */ { 0, BX_IA_ERROR },
  /* 0D k0 */ { 0, BX_IA_ERROR },
  /* 0D    */ { 0, BX_IA_ERROR },
  /* 0E k0 */ { 0, BX_IA_ERROR },
  /* 0E    */ { 0, BX_IA_ERROR },
  /* 0F k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPALIGNR_VdqHdqWdqIb },
  /* 0F    */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPALIGNR_VdqHdqWdqIb_Kmask },
  /* 10 k0 */ { 0, BX_IA_ERROR },
  /* 10    */ { 0, BX_IA_ERROR },
  /* 11 k0 */ { 0, BX_IA_ERROR },
  /* 11    */ { 0, BX_IA_ERROR },
  /* 12 k0 */ { 0, BX_IA_ERROR },
  /* 12    */ { 0, BX_IA_ERROR },
  /* 13 k0 */ { 0, BX_IA_ERROR },
  /* 13    */ { 0, BX_IA_ERROR },
  /* 14 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPEXTRB_EbdVdqIb },
  /* 14    */ { 0, BX_IA_ERROR }, // #UD
  /* 15 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPEXTRW_EwdVdqIb },
  /* 15    */ { 0, BX_IA_ERROR }, // #UD
  /* 16 k0 */ { BxPrefixSSE66 | BxAliasVexW64 | BxImmediate_Ib, BX_IA_V512_VPEXTRD_EdVdqIb },
  /* 16    */ { 0, BX_IA_ERROR }, // #UD
  /* 17 k0 */ { BxPrefixSSE66 | BxVexW0 | BxImmediate_Ib, BX_IA_V512_VEXTRACTPS_EdVpsIb },
  /* 17    */ { 0, BX_IA_ERROR }, // #UD
  /* 18 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTF32x4_VpsHpsWpsIb },
  /* 18    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTF32x4_VpsHpsWpsIb_Kmask },
  /* 19 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTF32x4_WpsVpsIb },
  /* 19    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTF32x4_WpsVpsIb_Kmask },
  /* 1A k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTF32x8_VpsHpsWpsIb },
  /* 1A    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTF32x8_VpsHpsWpsIb_Kmask },
  /* 1B k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTF32x8_WpsVpsIb },
  /* 1B    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTF32x8_WpsVpsIb_Kmask },
  /* 1C k0 */ { 0, BX_IA_ERROR },
  /* 1C    */ { 0, BX_IA_ERROR },
  /* 1D k0 */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VCVTPS2PH_WpsVpsIb },
  /* 1D    */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VCVTPS2PH_WpsVpsIb_Kmask },
  /* 1E k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPUD_KGwHdqWdqIb },
  /* 1E    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPUD_KGwHdqWdqIb },
  /* 1F k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPD_KGwHdqWdqIb },
  /* 1F    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPD_KGwHdqWdqIb },
  /* 20 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPINSRB_VdqEbIb },
  /* 20    */ { 0, BX_IA_ERROR }, // #UD
  /* 21 k0 */ { BxVexW0 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTPS_VpsWssIb },
  /* 21    */ { 0, BX_IA_ERROR }, // #UD
  /* 22 k0 */ { BxPrefixSSE66 | BxAliasVexW64 | BxImmediate_Ib, BX_IA_V512_VPINSRD_VdqEdIb },
  /* 22    */ { 0, BX_IA_ERROR }, // #UD
  /* 23 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VSHUFF32x4_VpsHpsWpsIb_Kmask },
  /* 23    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VSHUFF32x4_VpsHpsWpsIb_Kmask },
  /* 24 k0 */ { 0, BX_IA_ERROR },
  /* 24    */ { 0, BX_IA_ERROR },
  /* 25 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPTERNLOGD_VdqHdqWdqIb },
  /* 25    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPTERNLOGD_VdqHdqWdqIb_Kmask },
  /* 26 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VGETMANTPS_VpsWpsIb_Kmask },
  /* 26    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VGETMANTPS_VpsWpsIb_Kmask },
  /* 27 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VGETMANTSS_VssHpsWssIb_Kmask },
  /* 27    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VGETMANTSS_VssHpsWssIb_Kmask },
  /* 28 k0 */ { 0, BX_IA_ERROR },
  /* 28    */ { 0, BX_IA_ERROR },
  /* 29 k0 */ { 0, BX_IA_ERROR },
  /* 29    */ { 0, BX_IA_ERROR },
  /* 2A k0 */ { 0, BX_IA_ERROR },
  /* 2A    */ { 0, BX_IA_ERROR },
  /* 2B k0 */ { 0, BX_IA_ERROR },
  /* 2B    */ { 0, BX_IA_ERROR },
  /* 2C k0 */ { 0, BX_IA_ERROR },
  /* 2C    */ { 0, BX_IA_ERROR },
  /* 2D k0 */ { 0, BX_IA_ERROR },
  /* 2D    */ { 0, BX_IA_ERROR },
  /* 2E k0 */ { 0, BX_IA_ERROR },
  /* 2E    */ { 0, BX_IA_ERROR },
  /* 2F k0 */ { 0, BX_IA_ERROR },
  /* 2F    */ { 0, BX_IA_ERROR },
  /* 30 k0 */ { 0, BX_IA_ERROR },
  /* 30    */ { 0, BX_IA_ERROR },
  /* 31 k0 */ { 0, BX_IA_ERROR },
  /* 31    */ { 0, BX_IA_ERROR },
  /* 32 k0 */ { 0, BX_IA_ERROR },
  /* 32    */ { 0, BX_IA_ERROR },
  /* 33 k0 */ { 0, BX_IA_ERROR },
  /* 33    */ { 0, BX_IA_ERROR },
  /* 34 k0 */ { 0, BX_IA_ERROR },
  /* 34    */ { 0, BX_IA_ERROR },
  /* 35 k0 */ { 0, BX_IA_ERROR },
  /* 35    */ { 0, BX_IA_ERROR },
  /* 36 k0 */ { 0, BX_IA_ERROR },
  /* 36    */ { 0, BX_IA_ERROR },
  /* 37 k0 */ { 0, BX_IA_ERROR },
  /* 37    */ { 0, BX_IA_ERROR },
  /* 38 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTI32x4_VdqHdqWdqIb },
  /* 38    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTI32x4_VdqHdqWdqIb_Kmask },
  /* 39 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTI32x4_WdqVdqIb },
  /* 39    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTI32x4_WdqVdqIb_Kmask },
  /* 3A k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTI32x8_VdqHdqWdqIb },
  /* 3A    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VINSERTI32x8_VdqHdqWdqIb_Kmask },
  /* 3B k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTI32x8_WdqVdqIb },
  /* 3B    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VEXTRACTI32x8_WdqVdqIb_Kmask },
  /* 3C k0 */ { 0, BX_IA_ERROR },
  /* 3C    */ { 0, BX_IA_ERROR },
  /* 3D k0 */ { 0, BX_IA_ERROR },
  /* 3D    */ { 0, BX_IA_ERROR },
  /* 3E k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPUB_KGqHdqWdqIb },
  /* 3E    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPUB_KGqHdqWdqIb },
  /* 3F k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPB_KGqHdqWdqIb },
  /* 3F    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VPCMPB_KGqHdqWdqIb },
  /* 40 k0 */ { 0, BX_IA_ERROR },
  /* 40    */ { 0, BX_IA_ERROR },
  /* 41 k0 */ { 0, BX_IA_ERROR },
  /* 41    */ { 0, BX_IA_ERROR },
  /* 42 k0 */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VDBPSADBW_VdqHdqWdqIb_Kmask },
  /* 42    */ { BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VDBPSADBW_VdqHdqWdqIb_Kmask },
  /* 43 k0 */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VSHUFI32x4_VdqHdqWdqIb_Kmask },
  /* 43    */ { BxAliasVexW | BxVexL1 | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VSHUFI32x4_VdqHdqWdqIb_Kmask },
  /* 44 k0 */ { 0, BX_IA_ERROR },
  /* 44    */ { 0, BX_IA_ERROR },
  /* 45 k0 */ { 0, BX_IA_ERROR },
  /* 45    */ { 0, BX_IA_ERROR },
  /* 46 k0 */ { 0, BX_IA_ERROR },
  /* 46    */ { 0, BX_IA_ERROR },
  /* 47 k0 */ { 0, BX_IA_ERROR },
  /* 47    */ { 0, BX_IA_ERROR },
  /* 48 k0 */ { 0, BX_IA_ERROR },
  /* 48    */ { 0, BX_IA_ERROR },
  /* 49 k0 */ { 0, BX_IA_ERROR },
  /* 49    */ { 0, BX_IA_ERROR },
  /* 4A k0 */ { 0, BX_IA_ERROR },
  /* 4A    */ { 0, BX_IA_ERROR },
  /* 4B k0 */ { 0, BX_IA_ERROR },
  /* 4B    */ { 0, BX_IA_ERROR },
  /* 4C k0 */ { 0, BX_IA_ERROR },
  /* 4C    */ { 0, BX_IA_ERROR },
  /* 4D k0 */ { 0, BX_IA_ERROR },
  /* 4D    */ { 0, BX_IA_ERROR },
  /* 4E k0 */ { 0, BX_IA_ERROR },
  /* 4E    */ { 0, BX_IA_ERROR },
  /* 4F k0 */ { 0, BX_IA_ERROR },
  /* 4F    */ { 0, BX_IA_ERROR },
  /* 50 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRANGEPS_VpsHpsWpsIb_Kmask },
  /* 50    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRANGEPS_VpsHpsWpsIb_Kmask },
  /* 51 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRANGESS_VssHpsWssIb_Kmask },
  /* 51    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VRANGESS_VssHpsWssIb_Kmask },
  /* 52 k0 */ { 0, BX_IA_ERROR },
  /* 52    */ { 0, BX_IA_ERROR },
  /* 53 k0 */ { 0, BX_IA_ERROR },
  /* 53    */ { 0, BX_IA_ERROR },
  /* 54 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFIXUPIMMPS_VpsHpsWpsIb },
  /* 54    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFIXUPIMMPS_VpsHpsWpsIb_Kmask },
  /* 55 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFIXUPIMMSS_VssHssWssIb_Kmask },
  /* 55    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFIXUPIMMSS_VssHssWssIb_Kmask },
  /* 56 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VREDUCEPS_VpsWpsIb_Kmask },
  /* 56    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VREDUCEPS_VpsWpsIb_Kmask },
  /* 57 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VREDUCESS_VssHpsWssIb_Kmask },
  /* 57    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VREDUCESS_VssHpsWssIb_Kmask },
  /* 58 k0 */ { 0, BX_IA_ERROR },
  /* 58    */ { 0, BX_IA_ERROR },
  /* 59 k0 */ { 0, BX_IA_ERROR },
  /* 59    */ { 0, BX_IA_ERROR },
  /* 5A k0 */ { 0, BX_IA_ERROR },
  /* 5A    */ { 0, BX_IA_ERROR },
  /* 5B k0 */ { 0, BX_IA_ERROR },
  /* 5B    */ { 0, BX_IA_ERROR },
  /* 5C k0 */ { 0, BX_IA_ERROR },
  /* 5C    */ { 0, BX_IA_ERROR },
  /* 5D k0 */ { 0, BX_IA_ERROR },
  /* 5D    */ { 0, BX_IA_ERROR },
  /* 5E k0 */ { 0, BX_IA_ERROR },
  /* 5E    */ { 0, BX_IA_ERROR },
  /* 5F k0 */ { 0, BX_IA_ERROR },
  /* 5F    */ { 0, BX_IA_ERROR },
  /* 60 k0 */ { 0, BX_IA_ERROR },
  /* 60    */ { 0, BX_IA_ERROR },
  /* 61 k0 */ { 0, BX_IA_ERROR },
  /* 61    */ { 0, BX_IA_ERROR },
  /* 62 k0 */ { 0, BX_IA_ERROR },
  /* 62    */ { 0, BX_IA_ERROR },
  /* 63 k0 */ { 0, BX_IA_ERROR },
  /* 63    */ { 0, BX_IA_ERROR },
  /* 64 k0 */ { 0, BX_IA_ERROR },
  /* 64    */ { 0, BX_IA_ERROR },
  /* 65 k0 */ { 0, BX_IA_ERROR },
  /* 65    */ { 0, BX_IA_ERROR },
  /* 66 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFPCLASSPS_KGwWpsIb_Kmask },
  /* 66    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFPCLASSPS_KGwWpsIb_Kmask },
  /* 67 k0 */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFPCLASSSS_KGbWssIb_Kmask },
  /* 67    */ { BxAliasVexW | BxPrefixSSE66 | BxImmediate_Ib, BX_IA_V512_VFPCLASSSS_KGbWssIb_Kmask },
  /* 68 k0 */ { 0, BX_IA_ERROR },
  /* 68    */ { 0, BX_IA_ERROR },
  /* 69 k0 */ { 0, BX_IA_ERROR },
  /* 69    */ { 0, BX_IA_ERROR },
  /* 6A k0 */ { 0, BX_IA_ERROR },
  /* 6A    */ { 0, BX_IA_ERROR },
  /* 6B k0 */ { 0, BX_IA_ERROR },
  /* 6B    */ { 0, BX_IA_ERROR },
  /* 6C k0 */ { 0, BX_IA_ERROR },
  /* 6C    */ { 0, BX_IA_ERROR },
  /* 6D k0 */ { 0, BX_IA_ERROR },
  /* 6D    */ { 0, BX_IA_ERROR },
  /* 6E k0 */ { 0, BX_IA_ERROR },
  /* 6E    */ { 0, BX_IA_ERROR },
  /* 6F k0 */ { 0, BX_IA_ERROR },
  /* 6F    */ { 0, BX_IA_ERROR },
  /* 70 k0 */ { 0, BX_IA_ERROR },
  /* 70    */ { 0, BX_IA_ERROR },
  /* 71 k0 */ { 0, BX_IA_ERROR },
  /* 71    */ { 0, BX_IA_ERROR },
  /* 72 k0 */ { 0, BX_IA_ERROR },
  /* 72    */ { 0, BX_IA_ERROR },
  /* 73 k0 */ { 0, BX_IA_ERROR },
  /* 73    */ { 0, BX_IA_ERROR },
  /* 74 k0 */ { 0, BX_IA_ERROR },
  /* 74    */ { 0, BX_IA_ERROR },
  /* 75 k0 */ { 0, BX_IA_ERROR },
  /* 75    */ { 0, BX_IA_ERROR },
  /* 76 k0 */ { 0, BX_IA_ERROR },
  /* 76    */ { 0, BX_IA_ERROR },
  /* 77 k0 */ { 0, BX_IA_ERROR },
  /* 77    */ { 0, BX_IA_ERROR },
  /* 78 k0 */ { 0, BX_IA_ERROR },
  /* 78    */ { 0, BX_IA_ERROR },
  /* 79 k0 */ { 0, BX_IA_ERROR },
  /* 79    */ { 0, BX_IA_ERROR },
  /* 7A k0 */ { 0, BX_IA_ERROR },
  /* 7A    */ { 0, BX_IA_ERROR },
  /* 7B k0 */ { 0, BX_IA_ERROR },
  /* 7B    */ { 0, BX_IA_ERROR },
  /* 7C k0 */ { 0, BX_IA_ERROR },
  /* 7C    */ { 0, BX_IA_ERROR },
  /* 7D k0 */ { 0, BX_IA_ERROR },
  /* 7D    */ { 0, BX_IA_ERROR },
  /* 7E k0 */ { 0, BX_IA_ERROR },
  /* 7E    */ { 0, BX_IA_ERROR },
  /* 7F k0 */ { 0, BX_IA_ERROR },
  /* 7F    */ { 0, BX_IA_ERROR },
  /* 80 k0 */ { 0, BX_IA_ERROR },
  /* 80    */ { 0, BX_IA_ERROR },
  /* 81 k0 */ { 0, BX_IA_ERROR },
  /* 81    */ { 0, BX_IA_ERROR },
  /* 82 k0 */ { 0, BX_IA_ERROR },
  /* 82    */ { 0, BX_IA_ERROR },
  /* 83 k0 */ { 0, BX_IA_ERROR },
  /* 83    */ { 0, BX_IA_ERROR },
  /* 84 k0 */ { 0, BX_IA_ERROR },
  /* 84    */ { 0, BX_IA_ERROR },
  /* 85 k0 */ { 0, BX_IA_ERROR },
  /* 85    */ { 0, BX_IA_ERROR },
  /* 86 k0 */ { 0, BX_IA_ERROR },
  /* 86    */ { 0, BX_IA_ERROR },
  /* 87 k0 */ { 0, BX_IA_ERROR },
  /* 87    */ { 0, BX_IA_ERROR },
  /* 88 k0 */ { 0, BX_IA_ERROR },
  /* 88    */ { 0, BX_IA_ERROR },
  /* 89 k0 */ { 0, BX_IA_ERROR },
  /* 89    */ { 0, BX_IA_ERROR },
  /* 8A k0 */ { 0, BX_IA_ERROR },
  /* 8A    */ { 0, BX_IA_ERROR },
  /* 8B k0 */ { 0, BX_IA_ERROR },
  /* 8B    */ { 0, BX_IA_ERROR },
  /* 8C k0 */ { 0, BX_IA_ERROR },
  /* 8C    */ { 0, BX_IA_ERROR },
  /* 8D k0 */ { 0, BX_IA_ERROR },
  /* 8D    */ { 0, BX_IA_ERROR },
  /* 8E k0 */ { 0, BX_IA_ERROR },
  /* 8E    */ { 0, BX_IA_ERROR },
  /* 8F k0 */ { 0, BX_IA_ERROR },
  /* 8F    */ { 0, BX_IA_ERROR },
  /* 90 k0 */ { 0, BX_IA_ERROR },
  /* 90    */ { 0, BX_IA_ERROR },
  /* 91 k0 */ { 0, BX_IA_ERROR },
  /* 91    */ { 0, BX_IA_ERROR },
  /* 92 k0 */ { 0, BX_IA_ERROR },
  /* 92    */ { 0, BX_IA_ERROR },
  /* 93 k0 */ { 0, BX_IA_ERROR },
  /* 93    */ { 0, BX_IA_ERROR },
  /* 94 k0 */ { 0, BX_IA_ERROR },
  /* 94    */ { 0, BX_IA_ERROR },
  /* 95 k0 */ { 0, BX_IA_ERROR },
  /* 95    */ { 0, BX_IA_ERROR },
  /* 96 k0 */ { 0, BX_IA_ERROR },
  /* 96    */ { 0, BX_IA_ERROR },
  /* 97 k0 */ { 0, BX_IA_ERROR },
  /* 97    */ { 0, BX_IA_ERROR },
  /* 98 k0 */ { 0, BX_IA_ERROR },
  /* 98    */ { 0, BX_IA_ERROR },
  /* 99 k0 */ { 0, BX_IA_ERROR },
  /* 99    */ { 0, BX_IA_ERROR },
  /* 9A k0 */ { 0, BX_IA_ERROR },
  /* 9A    */ { 0, BX_IA_ERROR },
  /* 9B k0 */ { 0, BX_IA_ERROR },
  /* 9B    */ { 0, BX_IA_ERROR },
  /* 9C k0 */ { 0, BX_IA_ERROR },
  /* 9C    */ { 0, BX_IA_ERROR },
  /* 9D k0 */ { 0, BX_IA_ERROR },
  /* 9D    */ { 0, BX_IA_ERROR },
  /* 9E k0 */ { 0, BX_IA_ERROR },
  /* 9E    */ { 0, BX_IA_ERROR },
  /* 9F k0 */ { 0, BX_IA_ERROR },
  /* 9F    */ { 0, BX_IA_ERROR },
  /* A0 k0 */ { 0, BX_IA_ERROR },
  /* A0    */ { 0, BX_IA_ERROR },
  /* A1 k0 */ { 0, BX_IA_ERROR },
  /* A1    */ { 0, BX_IA_ERROR },
  /* A2 k0 */ { 0, BX_IA_ERROR },
  /* A2    */ { 0, BX_IA_ERROR },
  /* A3 k0 */ { 0, BX_IA_ERROR },
  /* A3    */ { 0, BX_IA_ERROR },
  /* A4 k0 */ { 0, BX_IA_ERROR },
  /* A4    */ { 0, BX_IA_ERROR },
  /* A5 k0 */ { 0, BX_IA_ERROR },
  /* A5    */ { 0, BX_IA_ERROR },
  /* A6 k0 */ { 0, BX_IA_ERROR },
  /* A6    */ { 0, BX_IA_ERROR },
  /* A7 k0 */ { 0, BX_IA_ERROR },
  /* A7    */ { 0, BX_IA_ERROR },
  /* A8 k0 */ { 0, BX_IA_ERROR },
  /* A8    */ { 0, BX_IA_ERROR },
  /* A9 k0 */ { 0, BX_IA_ERROR },
  /* A9    */ { 0, BX_IA_ERROR },
  /* AA k0 */ { 0, BX_IA_ERROR },
  /* AA    */ { 0, BX_IA_ERROR },
  /* AB k0 */ { 0, BX_IA_ERROR },
  /* AB    */ { 0, BX_IA_ERROR },
  /* AC k0 */ { 0, BX_IA_ERROR },
  /* AC    */ { 0, BX_IA_ERROR },
  /* AD k0 */ { 0, BX_IA_ERROR },
  /* AD    */ { 0, BX_IA_ERROR },
  /* AE k0 */ { 0, BX_IA_ERROR },
  /* AE    */ { 0, BX_IA_ERROR },
  /* AF k0 */ { 0, BX_IA_ERROR },
  /* AF    */ { 0, BX_IA_ERROR },
  /* B0 k0 */ { 0, BX_IA_ERROR },
  /* B0    */ { 0, BX_IA_ERROR },
  /* B1 k0 */ { 0, BX_IA_ERROR },
  /* B1    */ { 0, BX_IA_ERROR },
  /* B2 k0 */ { 0, BX_IA_ERROR },
  /* B2    */ { 0, BX_IA_ERROR },
  /* B3 k0 */ { 0, BX_IA_ERROR },
  /* B3    */ { 0, BX_IA_ERROR },
  /* B4 k0 */ { 0, BX_IA_ERROR },
  /* B4    */ { 0, BX_IA_ERROR },
  /* B5 k0 */ { 0, BX_IA_ERROR },
  /* B5    */ { 0, BX_IA_ERROR },
  /* B6 k0 */ { 0, BX_IA_ERROR },
  /* B6    */ { 0, BX_IA_ERROR },
  /* B7 k0 */ { 0, BX_IA_ERROR },
  /* B7    */ { 0, BX_IA_ERROR },
  /* B8 k0 */ { 0, BX_IA_ERROR },
  /* B8    */ { 0, BX_IA_ERROR },
  /* B9 k0 */ { 0, BX_IA_ERROR },
  /* B9    */ { 0, BX_IA_ERROR },
  /* BA k0 */ { 0, BX_IA_ERROR },
  /* BA    */ { 0, BX_IA_ERROR },
  /* BB k0 */ { 0, BX_IA_ERROR },
  /* BB    */ { 0, BX_IA_ERROR },
  /* BC k0 */ { 0, BX_IA_ERROR },
  /* BC    */ { 0, BX_IA_ERROR },
  /* BD k0 */ { 0, BX_IA_ERROR },
  /* BD    */ { 0, BX_IA_ERROR },
  /* BE k0 */ { 0, BX_IA_ERROR },
  /* BE    */ { 0, BX_IA_ERROR },
  /* BF k0 */ { 0, BX_IA_ERROR },
  /* BF    */ { 0, BX_IA_ERROR },
  /* C0 k0 */ { 0, BX_IA_ERROR },
  /* C0    */ { 0, BX_IA_ERROR },
  /* C1 k0 */ { 0, BX_IA_ERROR },
  /* C1    */ { 0, BX_IA_ERROR },
  /* C2 k0 */ { 0, BX_IA_ERROR },
  /* C2    */ { 0, BX_IA_ERROR },
  /* C3 k0 */ { 0, BX_IA_ERROR },
  /* C3    */ { 0, BX_IA_ERROR },
  /* C4 k0 */ { 0, BX_IA_ERROR },
  /* C4    */ { 0, BX_IA_ERROR },
  /* C5 k0 */ { 0, BX_IA_ERROR },
  /* C5    */ { 0, BX_IA_ERROR },
  /* C6 k0 */ { 0, BX_IA_ERROR },
  /* C6    */ { 0, BX_IA_ERROR },
  /* C7 k0 */ { 0, BX_IA_ERROR },
  /* C7    */ { 0, BX_IA_ERROR },
  /* C8 k0 */ { 0, BX_IA_ERROR },
  /* C8    */ { 0, BX_IA_ERROR },
  /* C9 k0 */ { 0, BX_IA_ERROR },
  /* C9    */ { 0, BX_IA_ERROR },
  /* CA k0 */ { 0, BX_IA_ERROR },
  /* CA    */ { 0, BX_IA_ERROR },
  /* CB k0 */ { 0, BX_IA_ERROR },
  /* CB    */ { 0, BX_IA_ERROR },
  /* CC k0 */ { 0, BX_IA_ERROR },
  /* CC    */ { 0, BX_IA_ERROR },
  /* CD k0 */ { 0, BX_IA_ERROR },
  /* CD    */ { 0, BX_IA_ERROR },
  /* CE k0 */ { 0, BX_IA_ERROR },
  /* CE    */ { 0, BX_IA_ERROR },
  /* CF k0 */ { 0, BX_IA_ERROR },
  /* CF    */ { 0, BX_IA_ERROR },
  /* D0 k0 */ { 0, BX_IA_ERROR },
  /* D0    */ { 0, BX_IA_ERROR },
  /* D1 k0 */ { 0, BX_IA_ERROR },
  /* D1    */ { 0, BX_IA_ERROR },
  /* D2 k0 */ { 0, BX_IA_ERROR },
  /* D2    */ { 0, BX_IA_ERROR },
  /* D3 k0 */ { 0, BX_IA_ERROR },
  /* D3    */ { 0, BX_IA_ERROR },
  /* D4 k0 */ { 0, BX_IA_ERROR },
  /* D4    */ { 0, BX_IA_ERROR },
  /* D5 k0 */ { 0, BX_IA_ERROR },
  /* D5    */ { 0, BX_IA_ERROR },
  /* D6 k0 */ { 0, BX_IA_ERROR },
  /* D6    */ { 0, BX_IA_ERROR },
  /* D7 k0 */ { 0, BX_IA_ERROR },
  /* D7    */ { 0, BX_IA_ERROR },
  /* D8 k0 */ { 0, BX_IA_ERROR },
  /* D8    */ { 0, BX_IA_ERROR },
  /* D9 k0 */ { 0, BX_IA_ERROR },
  /* D9    */ { 0, BX_IA_ERROR },
  /* DA k0 */ { 0, BX_IA_ERROR },
  /* DA    */ { 0, BX_IA_ERROR },
  /* DB k0 */ { 0, BX_IA_ERROR },
  /* DB    */ { 0, BX_IA_ERROR },
  /* DC k0 */ { 0, BX_IA_ERROR },
  /* DC    */ { 0, BX_IA_ERROR },
  /* DD k0 */ { 0, BX_IA_ERROR },
  /* DD    */ { 0, BX_IA_ERROR },
  /* DE k0 */ { 0, BX_IA_ERROR },
  /* DE    */ { 0, BX_IA_ERROR },
  /* DF k0 */ { 0, BX_IA_ERROR },
  /* DF    */ { 0, BX_IA_ERROR },
  /* E0 k0 */ { 0, BX_IA_ERROR },
  /* E0    */ { 0, BX_IA_ERROR },
  /* E1 k0 */ { 0, BX_IA_ERROR },
  /* E1    */ { 0, BX_IA_ERROR },
  /* E2 k0 */ { 0, BX_IA_ERROR },
  /* E2    */ { 0, BX_IA_ERROR },
  /* E3 k0 */ { 0, BX_IA_ERROR },
  /* E3    */ { 0, BX_IA_ERROR },
  /* E4 k0 */ { 0, BX_IA_ERROR },
  /* E4    */ { 0, BX_IA_ERROR },
  /* E5 k0 */ { 0, BX_IA_ERROR },
  /* E5    */ { 0, BX_IA_ERROR },
  /* E6 k0 */ { 0, BX_IA_ERROR },
  /* E6    */ { 0, BX_IA_ERROR },
  /* E7 k0 */ { 0, BX_IA_ERROR },
  /* E7    */ { 0, BX_IA_ERROR },
  /* E8 k0 */ { 0, BX_IA_ERROR },
  /* E8    */ { 0, BX_IA_ERROR },
  /* E9 k0 */ { 0, BX_IA_ERROR },
  /* E9    */ { 0, BX_IA_ERROR },
  /* EA k0 */ { 0, BX_IA_ERROR },
  /* EA    */ { 0, BX_IA_ERROR },
  /* EB k0 */ { 0, BX_IA_ERROR },
  /* EB    */ { 0, BX_IA_ERROR },
  /* EC k0 */ { 0, BX_IA_ERROR },
  /* EC    */ { 0, BX_IA_ERROR },
  /* ED k0 */ { 0, BX_IA_ERROR },
  /* ED    */ { 0, BX_IA_ERROR },
  /* EE k0 */ { 0, BX_IA_ERROR },
  /* EE    */ { 0, BX_IA_ERROR },
  /* EF k0 */ { 0, BX_IA_ERROR },
  /* EF    */ { 0, BX_IA_ERROR },
  /* F0 k0 */ { 0, BX_IA_ERROR },
  /* F0    */ { 0, BX_IA_ERROR },
  /* F1 k0 */ { 0, BX_IA_ERROR },
  /* F1    */ { 0, BX_IA_ERROR },
  /* F2 k0 */ { 0, BX_IA_ERROR },
  /* F2    */ { 0, BX_IA_ERROR },
  /* F3 k0 */ { 0, BX_IA_ERROR },
  /* F3    */ { 0, BX_IA_ERROR },
  /* F4 k0 */ { 0, BX_IA_ERROR },
  /* F4    */ { 0, BX_IA_ERROR },
  /* F5 k0 */ { 0, BX_IA_ERROR },
  /* F5    */ { 0, BX_IA_ERROR },
  /* F6 k0 */ { 0, BX_IA_ERROR },
  /* F6    */ { 0, BX_IA_ERROR },
  /* F7 k0 */ { 0, BX_IA_ERROR },
  /* F7    */ { 0, BX_IA_ERROR },
  /* F8 k0 */ { 0, BX_IA_ERROR },
  /* F8    */ { 0, BX_IA_ERROR },
  /* F9 k0 */ { 0, BX_IA_ERROR },
  /* F9    */ { 0, BX_IA_ERROR },
  /* FA k0 */ { 0, BX_IA_ERROR },
  /* FA    */ { 0, BX_IA_ERROR },
  /* FB k0 */ { 0, BX_IA_ERROR },
  /* FB    */ { 0, BX_IA_ERROR },
  /* FC k0 */ { 0, BX_IA_ERROR },
  /* FC    */ { 0, BX_IA_ERROR },
  /* FD k0 */ { 0, BX_IA_ERROR },
  /* FD    */ { 0, BX_IA_ERROR },
  /* FE k0 */ { 0, BX_IA_ERROR },
  /* FE    */ { 0, BX_IA_ERROR },
  /* FF k0 */ { 0, BX_IA_ERROR },
  /* FF    */ { 0, BX_IA_ERROR }
};

#endif // BX_SUPPORT_EVEX

#endif // BX_AVX_FETCHDECODE_TABLES_H
