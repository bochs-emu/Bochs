/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2019 Stanislav Shwartsman
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

// VEX-encoded 0x0F opcodes
static const Bit64u BxOpcodeGroup_VEX_0F10[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128_256, BX_IA_VMOVUPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128_256, BX_IA_VMOVUPD_VpdWpd),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0, BX_IA_V128_VMOVSS_VssHpsWss),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MODC0, BX_IA_V128_VMOVSD_VsdHpdWsd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MOD_MEM, BX_IA_V128_VMOVSS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MOD_MEM, BX_IA_V128_VMOVSD_VsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F11[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128, BX_IA_V128_VMOVUPS_WpsVps),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL256, BX_IA_V256_VMOVUPS_WpsVps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VMOVUPD_WpdVpd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VMOVUPD_WpdVpd),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0, BX_IA_V128_VMOVSS_WssHpsVss),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MODC0, BX_IA_V128_VMOVSD_WsdHpdVsd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MOD_MEM, BX_IA_V128_VMOVSS_WssVss),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MOD_MEM, BX_IA_V128_VMOVSD_WsdVsd),
};

static const Bit64u BxOpcodeGroup_VEX_0F12[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVLPS_VpsHpsMq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_MODC0, BX_IA_V128_VMOVHLPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVLPD_VpdHpdMq),

  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VMOVSLDUP_VpsWps),

  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128, BX_IA_V128_VMOVDDUP_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL256, BX_IA_V256_VMOVDDUP_VpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F13[] = { 
  form_opcode(ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_V128_VMOVLPS_MqVps),
  last_opcode(ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_V128_VMOVLPD_MqVsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F14[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VUNPCKLPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VUNPCKLPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F15[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VUNPCKHPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VUNPCKHPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F16[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVHPS_VpsHpsMq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_MODC0, BX_IA_V128_VMOVLHPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVHPD_VpdHpdMq),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VMOVSHDUP_VpsWps)
};

static const Bit64u BxOpcodeGroup_VEX_0F17[] = {
  form_opcode(ATTR_MOD_MEM | ATTR_VL128 | ATTR_SSE_NO_PREFIX, BX_IA_V128_VMOVHPS_MqVps),
  last_opcode(ATTR_MOD_MEM | ATTR_VL128 | ATTR_SSE_PREFIX_66, BX_IA_V128_VMOVHPD_MqVsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F28[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128_256, BX_IA_VMOVAPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128_256, BX_IA_VMOVAPD_VpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F29[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128, BX_IA_V128_VMOVAPS_WpsVps),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL256, BX_IA_V256_VMOVAPS_WpsVps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VMOVAPD_WpdVpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VMOVAPD_WpdVpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F2A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_VCVTSI2SS_VssEd),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_VCVTSI2SS_VssEq),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_VCVTSI2SD_VsdEd),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_VCVTSI2SD_VsdEq),
};

static const Bit64u BxOpcodeGroup_VEX_0F2B[] = {
  form_opcode(ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_V128_VMOVNTPS_MpsVps),
  form_opcode(ATTR_VL256 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_V256_VMOVNTPS_MpsVps),
  form_opcode(ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_V128_VMOVNTPD_MpdVpd),
  last_opcode(ATTR_VL256 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_V256_VMOVNTPD_MpdVpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F2C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_VCVTTSS2SI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_VCVTTSS2SI_GqWss),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_VCVTTSD2SI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_VCVTTSD2SI_GqWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F2D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_VCVTSS2SI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_VCVTSS2SI_GqWss),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_VCVTSD2SI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_VCVTSD2SI_GqWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F2E[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VUCOMISS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VUCOMISD_VsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F2F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VCOMISS_VssWss),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VCOMISD_VsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F41[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KANDW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KANDQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KANDB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KANDD_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F42[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KANDNW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KANDNQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KANDNB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KANDND_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F44[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KNOTW_KGwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KNOTQ_KGqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KNOTB_KGbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KNOTD_KGdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F45[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KORW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KORQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KORB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KORD_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F46[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KXNORW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KXNORQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KXNORB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KXNORD_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F47[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KXORW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KXORQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KXORB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KXORD_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F4A[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KADDW_KGwKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KADDQ_KGqKHqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KADDB_KGbKHbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KADDD_KGdKHdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F4B[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KUNPCKWD_KGdKHwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KUNPCKDQ_KGqKHdKEd),
  last_opcode(ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KUNPCKBW_KGwKHbKEb)
};

static const Bit64u BxOpcodeGroup_VEX_0F50[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_VMOVMSKPS_GdUps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0, BX_IA_VMOVMSKPD_GdUpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F51[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VSQRTPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VSQRTPD_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VSQRTSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VSQRTSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F52[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VRSQRTPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VRSQRTSS_VssHpsWss)
};

static const Bit64u BxOpcodeGroup_VEX_0F53[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VRCPPS_VpsWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VRCPSS_VssHpsWss)
};

static const Bit64u BxOpcodeGroup_VEX_0F54[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VANDPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VANDPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F55[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VANDNPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VANDNPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F56[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VORPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VORPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F57[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VXORPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VXORPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F58[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VADDPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VADDPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VADDSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VADDSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F59[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VMULPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VMULPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VMULSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VMULSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F5A[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VCVTPS2PD_VpdWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VCVTPD2PS_VpsWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VCVTSS2SD_VsdWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VCVTSD2SS_VssWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F5B[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VCVTDQ2PS_VpsWdq),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VCVTPS2DQ_VdqWps),
  last_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VCVTTPS2DQ_VdqWps)
};

static const Bit64u BxOpcodeGroup_VEX_0F5C[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VSUBPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VSUBPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VSUBSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VSUBSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F5D[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VMINPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VMINPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VMINSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VMINSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F5E[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VDIVPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VDIVPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VDIVSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VDIVSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F5F[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VMAXPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VMAXPD_VpdHpdWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VMAXSS_VssHpsWss),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VMAXSD_VsdHpdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F60[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKLBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKLBW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F61[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKLWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKLWD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F62[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKLDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKLDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F63[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPACKSSWB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPACKSSWB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F64[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPGTB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPGTB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F65[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPGTW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPGTW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F66[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPGTD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPGTD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F67[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPACKUSWB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPACKUSWB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F68[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKHBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKHBW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F69[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKHWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKHWD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKHDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKHDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPACKSSDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPACKSSDW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKLQDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKLQDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPUNPCKHQDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPUNPCKHQDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_V128_VMOVD_VdqEd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_V128_VMOVQ_VdqEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F6F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128_256, BX_IA_VMOVDQA_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128_256, BX_IA_VMOVDQU_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F70[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSHUFD_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSHUFD_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128, BX_IA_V128_VPSHUFHW_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL256, BX_IA_V256_VPSHUFHW_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128, BX_IA_V128_VPSHUFLW_VdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL256, BX_IA_V256_VPSHUFLW_VdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F71[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL128, BX_IA_V128_VPSRLW_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL256, BX_IA_V256_VPSRLW_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4 | ATTR_VL128, BX_IA_V128_VPSRAW_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4 | ATTR_VL256, BX_IA_V256_VPSRAW_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL128, BX_IA_V128_VPSLLW_UdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL256, BX_IA_V256_VPSLLW_UdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F72[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL128, BX_IA_V128_VPSRLD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL256, BX_IA_V256_VPSRLD_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4 | ATTR_VL128, BX_IA_V128_VPSRAD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4 | ATTR_VL256, BX_IA_V256_VPSRAD_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL128, BX_IA_V128_VPSLLD_UdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL256, BX_IA_V256_VPSLLD_UdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F73[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL128, BX_IA_V128_VPSRLQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_VL256, BX_IA_V256_VPSRLQ_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN3 | ATTR_VL128, BX_IA_V128_VPSRLDQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN3 | ATTR_VL256, BX_IA_V256_VPSRLDQ_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL128, BX_IA_V128_VPSLLQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_VL256, BX_IA_V256_VPSLLQ_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN7 | ATTR_VL128, BX_IA_V128_VPSLLDQ_UdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN7 | ATTR_VL256, BX_IA_V256_VPSLLDQ_UdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F74[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPEQB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPEQB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F75[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPEQW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPEQW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F76[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPEQD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPEQD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F77[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_MODC0, BX_IA_VZEROUPPER),
  last_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL256 | ATTR_MODC0, BX_IA_VZEROALL)
};

static const Bit64u BxOpcodeGroup_VEX_0F7C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VHADDPD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VHADDPS_VpsHpsWps)
};

static const Bit64u BxOpcodeGroup_VEX_0F7D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VHSUBPD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VHSUBPS_VpsHpsWps)
};

static const Bit64u BxOpcodeGroup_VEX_0F7E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_V128_VMOVD_EdVd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_V128_VMOVQ_EqVq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128, BX_IA_VMOVQ_VqWq)
};

static const Bit64u BxOpcodeGroup_VEX_0F7F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VMOVDQA_WdqVdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VMOVDQA_WdqVdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128, BX_IA_V128_VMOVDQU_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL256, BX_IA_V256_VMOVDQU_WdqVdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F90[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_SSE_NO_PREFIX, BX_IA_KMOVW_KGwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_SSE_NO_PREFIX, BX_IA_KMOVQ_KGqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_SSE_PREFIX_66, BX_IA_KMOVB_KGbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_SSE_PREFIX_66, BX_IA_KMOVD_KGdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F91[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_KMOVW_KEwKGw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_KMOVQ_KEqKGq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_KMOVB_KEbKGb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_KMOVD_KEdKGd)
};

static const Bit64u BxOpcodeGroup_VEX_0F92[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KMOVW_KGwEw),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KMOVB_KGbEb),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2,             BX_IA_KMOVD_KGdEd),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_KMOVQ_KGqEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F93[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KMOVW_GdKEw),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KMOVB_GdKEb),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2,             BX_IA_KMOVD_GdKEd),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_KMOVQ_GqKEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F98[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KORTESTW_KGwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KORTESTQ_KGqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KORTESTB_KGbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KORTESTD_KGdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0F99[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KTESTW_KGwKEw),
  form_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_NO_PREFIX, BX_IA_KTESTQ_KGqKEq),
  form_opcode(ATTR_VEX_W0 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KTESTB_KGbKEb),
  last_opcode(ATTR_VEX_W1 | ATTR_VL128 | ATTR_MODC0 | ATTR_SSE_PREFIX_66, BX_IA_KTESTD_KGdKEd)
};

static const Bit64u BxOpcodeGroup_VEX_0FAE[] = {
  form_opcode(ATTR_VL128 | ATTR_NNN2 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_VLDMXCSR),
  last_opcode(ATTR_VL128 | ATTR_NNN3 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_VSTMXCSR)
};

static const Bit64u BxOpcodeGroup_VEX_0FC2[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VCMPPS_VpsHpsWpsIb),
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VCMPPD_VpdHpdWpdIb),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VCMPSS_VssHpsWssIb),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VCMPSD_VsdHpdWsdIb)
};

static const Bit64u BxOpcodeGroup_VEX_0FC4[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128,              BX_IA_V128_VPINSRW_VdqEwIb) };
static const Bit64u BxOpcodeGroup_VEX_0FC5[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MODC0, BX_IA_V128_VPEXTRW_GdUdqIb) };

static const Bit64u BxOpcodeGroup_VEX_0FC6[] = { 
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_VSHUFPS_VpsHpsWpsIb),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VSHUFPD_VpdHpdWpdIb)
};

static const Bit64u BxOpcodeGroup_VEX_0FD0[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VADDSUBPD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VADDSUBPS_VpsHpsWps)
};

static const Bit64u BxOpcodeGroup_VEX_0FD1[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSRLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSRLW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD2[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSRLD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSRLD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD3[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSRLQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSRLQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD4[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD5[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULLW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD6[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_VMOVQ_WqVq) };

static const Bit64u BxOpcodeGroup_VEX_0FD7[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MODC0, BX_IA_V128_VPMOVMSKB_GdUdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_MODC0, BX_IA_V256_VPMOVMSKB_GdUdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD8[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBUSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBUSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FD9[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBUSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBUSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDA[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINUB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINUB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDB[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPAND_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPAND_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDC[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDUSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDUSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDD[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDUSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDUSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDE[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXUB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXUB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FDF[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPANDN_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPANDN_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE0[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPAVGB_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPAVGB_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE1[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSRAW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSRAW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE2[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSRAD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSRAD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE3[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPAVGW_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPAVGW_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE4[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULHUW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULHUW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE5[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULHW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULHW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE6[] = { 
  form_opcode(ATTR_SSE_PREFIX_66, BX_IA_VCVTTPD2DQ_VdqWpd),
  form_opcode(ATTR_SSE_PREFIX_F3, BX_IA_VCVTDQ2PD_VpdWdq),
  last_opcode(ATTR_SSE_PREFIX_F2, BX_IA_VCVTPD2DQ_VdqWpd)
};

static const Bit64u BxOpcodeGroup_VEX_0FE7[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVNTDQ_MdqVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_MOD_MEM, BX_IA_V256_VMOVNTDQ_MdqVdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE8[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FE9[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FEA[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FEB[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPOR_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPOR_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FEC[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FED[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FEE[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FEF[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPXOR_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPXOR_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF0[] = { last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MOD_MEM, BX_IA_VLDDQU_VdqMdq) };

static const Bit64u BxOpcodeGroup_VEX_0FF1[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSLLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSLLW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF2[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSLLD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSLLD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF3[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSLLQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSLLQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF4[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULUDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULUDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF5[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMADDWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMADDWD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF6[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSADBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSADBW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF7[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MODC0 | ATTR_VL128, BX_IA_V128_VMASKMOVDQU_VdqUdq) };

static const Bit64u BxOpcodeGroup_VEX_0FF8[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FF9[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FFA[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FFB[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSUBQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSUBQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FFC[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FFD[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0FFE[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPADDD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPADDD_VdqHdqWdq)
};

// VEX-encoded 0x0F 0x38 opcodes

static const Bit64u BxOpcodeGroup_VEX_0F3800[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSHUFB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSHUFB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3801[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHADDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHADDW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3802[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHADDD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHADDD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3803[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHADDSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHADDSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3804[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMADDUBSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMADDUBSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3805[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHSUBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHSUBW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3806[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHSUBD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHSUBD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3807[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHSUBSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPHSUBSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3808[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSIGNB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSIGNB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3809[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSIGNW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSIGNW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F380A[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPSIGND_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPSIGND_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F380B[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULHRSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULHRSW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F380C[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMILPS_VpsHpsWps) };
static const Bit64u BxOpcodeGroup_VEX_0F380D[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMILPD_VpdHpdWpd) };
static const Bit64u BxOpcodeGroup_VEX_0F380E[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VTESTPS_VpsWps) };
static const Bit64u BxOpcodeGroup_VEX_0F380F[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VTESTPD_VpdWpd) };
static const Bit64u BxOpcodeGroup_VEX_0F3813[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VCVTPH2PS_VpsWps) };
static const Bit64u BxOpcodeGroup_VEX_0F3816[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256, BX_IA_V256_VPERMPS_VpsHpsWps) };
static const Bit64u BxOpcodeGroup_VEX_0F3817[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VPTEST_VdqWdq) };

static const Bit64u BxOpcodeGroup_VEX_0F3818[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0,   BX_IA_VBROADCASTSS_VpsWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VBROADCASTSS_VpsMss),
};

static const Bit64u BxOpcodeGroup_VEX_0F3819[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256 | ATTR_MODC0,   BX_IA_V256_VBROADCASTSD_VpdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256 | ATTR_MOD_MEM, BX_IA_V256_VBROADCASTSD_VpdMsd),
};

static const Bit64u BxOpcodeGroup_VEX_0F381A[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256 | ATTR_MOD_MEM, BX_IA_V256_VBROADCASTF128_VdqMdq) };

static const Bit64u BxOpcodeGroup_VEX_0F381C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPABSB_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPABSB_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F381D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPABSW_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPABSW_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F381E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPABSD_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPABSD_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3820[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXBW_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXBW_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3821[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXBD_VdqWd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXBD_VdqWq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3822[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXBQ_VdqWw),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXBQ_VdqWd)
};

static const Bit64u BxOpcodeGroup_VEX_0F3823[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXWD_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXWD_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3824[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXWQ_VdqWd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXWQ_VdqWq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3825[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVSXDQ_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVSXDQ_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3828[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULDQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3829[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPEQQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPEQQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F382A[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MOD_MEM, BX_IA_V128_VMOVNTDQA_VdqMdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_MOD_MEM, BX_IA_V256_VMOVNTDQA_VdqMdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F382B[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPACKUSDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPACKUSDW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F382C[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VMASKMOVPS_VpsHpsMps) };
static const Bit64u BxOpcodeGroup_VEX_0F382D[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VMASKMOVPD_VpdHpdMpd) };
static const Bit64u BxOpcodeGroup_VEX_0F382E[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VMASKMOVPS_MpsHpsVps) };
static const Bit64u BxOpcodeGroup_VEX_0F382F[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VMASKMOVPD_MpdHpdVpd) };

static const Bit64u BxOpcodeGroup_VEX_0F3830[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXBW_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXBW_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3831[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXBD_VdqWd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXBD_VdqWq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3832[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXBQ_VdqWw),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXBQ_VdqWd)
};

static const Bit64u BxOpcodeGroup_VEX_0F3833[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXWD_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXWD_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3834[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXWQ_VdqWd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXWQ_VdqWq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3835[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMOVZXDQ_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMOVZXDQ_VdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3836[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256, BX_IA_V256_VPERMD_VdqHdqWdq) };

static const Bit64u BxOpcodeGroup_VEX_0F3837[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPGTQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCMPGTQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3838[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3839[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINSD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINSD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383A[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINUW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINUW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383B[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMINUD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMINUD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXSB_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXSD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXSD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXUW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXUW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F383F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMAXUD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMAXUD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3840[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPMULLD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPMULLD_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3841[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPHMINPOSUW_VdqWdq) };

static const Bit64u BxOpcodeGroup_VEX_0F3845[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPSRLVD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VPSRLVQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3846[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPSRAVD_VdqHdqWdq) };

static const Bit64u BxOpcodeGroup_VEX_0F3847[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPSLLVD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VPSLLVQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3858[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPBROADCASTD_VdqWd) };
static const Bit64u BxOpcodeGroup_VEX_0F3859[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPBROADCASTQ_VdqWq) };
static const Bit64u BxOpcodeGroup_VEX_0F385A[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256 | ATTR_MOD_MEM, BX_IA_V256_VBROADCASTI128_VdqMdq) };
static const Bit64u BxOpcodeGroup_VEX_0F3878[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPBROADCASTB_VdqWb) };
static const Bit64u BxOpcodeGroup_VEX_0F3879[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPBROADCASTW_VdqWw) };

static const Bit64u BxOpcodeGroup_VEX_0F388C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VMASKMOVD_VdqHdqMdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VMASKMOVQ_VdqHdqMdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F388E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VMASKMOVD_MdqHdqVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VMASKMOVQ_MdqHdqVdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F3890[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VGATHERDD_VdqHdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VGATHERDQ_VdqHdq)
};
static const Bit64u BxOpcodeGroup_VEX_0F3891[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VGATHERQD_VdqHdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VGATHERQQ_VdqHdq)
};
static const Bit64u BxOpcodeGroup_VEX_0F3892[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VGATHERDPS_VpsHps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VGATHERDPD_VpdHpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F3893[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM, BX_IA_VGATHERQPS_VpsHps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM, BX_IA_VGATHERQPD_VpdHpd)
};

static const Bit64u BxOpcodeGroup_VEX_0F3896[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSUB132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSUB132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F3897[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBADD132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBADD132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F3898[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F3899[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD132SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD132SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB132SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB132SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD132SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD132SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB132PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB132PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F389F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB132SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB132SD_VpdHsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F38A6[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSUB213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSUB213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38A7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBADD213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBADD213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38A8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38A9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD213SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD213SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB213SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB213SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD213SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD213SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB213PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB213PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38AF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB213SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB213SD_VpdHsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F38B6[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSUB231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSUB231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38B7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBADD231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBADD231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38B8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38B9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADD231SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADD231SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUB231SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUB231SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADD231SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADD231SD_VpdHsdWsd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB231PS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB231PD_VpdHpdWpd)
};
static const Bit64u BxOpcodeGroup_VEX_0F38BF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUB231SS_VpsHssWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUB231SD_VpdHsdWsd)
};

static const Bit64u BxOpcodeGroup_VEX_0F38CF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VGF2P8MULB_VdqHdqWdq) };

static const Bit64u BxOpcodeGroup_VEX_0F38DB[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128,  BX_IA_V128_VAESIMC_VdqWdq) };
static const Bit64u BxOpcodeGroup_VEX_0F38DC[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VAESENC_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VAESENC_VdqHdqWdq)
};
static const Bit64u BxOpcodeGroup_VEX_0F38DD[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VAESENCLAST_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VAESENCLAST_VdqHdqWdq)
};
static const Bit64u BxOpcodeGroup_VEX_0F38DE[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VAESDEC_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VAESDEC_VdqHdqWdq)
};
static const Bit64u BxOpcodeGroup_VEX_0F38DF[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VAESDECLAST_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VAESDECLAST_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_VEX_0F38F2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_ANDN_GdBdEd),
  last_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_ANDN_GqBqEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F38F3[] = {
  form_opcode(ATTR_NNN1 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_BLSR_BdEd),
  form_opcode(ATTR_NNN1 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_BLSR_BqEq),

  form_opcode(ATTR_NNN2 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_BLSMSK_BdEd),
  form_opcode(ATTR_NNN2 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_BLSMSK_BqEq),

  form_opcode(ATTR_NNN3 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_BLSI_BdEd),
  last_opcode(ATTR_NNN3 | ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_BLSI_BqEq),
};

static const Bit64u BxOpcodeGroup_VEX_0F38F5[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_BZHI_GdBdEd),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_BZHI_GqBqEq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_PEXT_GdBdEd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_PEXT_GqBqEq),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_PDEP_GdBdEd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_PDEP_GqBqEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F38F6[] = {
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_MULX_GdBdEd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_MULX_GqBqEq)
};

static const Bit64u BxOpcodeGroup_VEX_0F38F7[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_BEXTR_GdEdBd),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_BEXTR_GqEqBq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_SHLX_GdEdBd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_SHLX_GqEqBq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_SARX_GdEdBd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_SARX_GqEqBq),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_SHRX_GdEdBd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_SHRX_GqEqBq)
};

// VEX-encoded 0x0F 0x3A opcodes

static const Bit64u BxOpcodeGroup_VEX_0F3A00[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256, BX_IA_V256_VPERMQ_VdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A01[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256, BX_IA_V256_VPERMPD_VpdWpdIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A02[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPBLENDD_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A04[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMILPS_VpsWpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A05[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMILPD_VpdWpdIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A06[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256, BX_IA_V256_VPERM2F128_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A08[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VROUNDPS_VpsWpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A09[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VROUNDPD_VpdWpdIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A0A[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VROUNDSS_VssHpsWssIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A0B[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VROUNDSD_VsdHpdWsdIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A0C[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VBLENDPS_VpsHpsWpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A0D[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_VBLENDPD_VpdHpdWpdIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A0E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPBLENDW_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPBLENDW_VdqHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A0F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPALIGNR_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPALIGNR_VdqHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A14[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPEXTRB_EbdVdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A15[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPEXTRW_EwdVdqIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A16[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_V128_VPEXTRD_EdVdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_V128_VPEXTRQ_EqVdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A17[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VEXTRACTPS_EdVpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A18[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_VEX_W0, BX_IA_V256_VINSERTF128_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A19[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_VEX_W0, BX_IA_V256_VEXTRACTF128_WdqVdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A1D[] = { last_opcode(ATTR_SSE_PREFIX_66 |              ATTR_VEX_W0, BX_IA_VCVTPS2PH_WpsVpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A20[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPINSRB_VdqEbIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A21[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VINSERTPS_VpsWssIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A22[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_V128_VPINSRD_VdqEdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_V128_VPINSRQ_VdqEqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A30[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_KSHIFTRB_KGbKEbIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1, BX_IA_KSHIFTRW_KGwKEwIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A31[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_KSHIFTRD_KGdKEdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1, BX_IA_KSHIFTRQ_KGqKEqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A32[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_KSHIFTLB_KGbKEbIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1, BX_IA_KSHIFTLW_KGwKEwIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A33[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_KSHIFTLD_KGdKEdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1, BX_IA_KSHIFTLQ_KGqKEqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A38[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_VEX_W0, BX_IA_V256_VINSERTI128_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A39[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_VEX_W0, BX_IA_V256_VEXTRACTI128_WdqVdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A40[] = { last_opcode(ATTR_SSE_PREFIX_66,              BX_IA_VDPPS_VpsHpsWpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A41[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_VDPPD_VpdHpdWpdIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A42[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VMPSADBW_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VMPSADBW_VdqHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A44[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCLMULQDQ_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPCLMULQDQ_VdqHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A46[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256 | ATTR_VEX_W0, BX_IA_V256_VPERM2I128_VdqHdqWdqIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A48[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMIL2PS_VdqHdqVIbWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VPERMIL2PS_VdqHdqWdqVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A49[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VPERMIL2PD_VdqHdqVIbWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VPERMIL2PD_VdqHdqWdqVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A4A[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VBLENDVPS_VpsHpsWpsIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A4B[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VBLENDVPD_VpdHpdWpdIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A4C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPBLENDVB_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256, BX_IA_V256_VPBLENDVB_VdqHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A5C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSUBPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSUBPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A5D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSUBPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSUBPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A5E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBADDPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBADDPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A5F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBADDPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBADDPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A60[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPESTRM_VdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A61[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPESTRI_VdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A62[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPISTRM_VdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3A63[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VPCMPISTRI_VdqWdqIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3A68[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A69[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6A[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSS_VssHssVIbWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSS_VssHssWssVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6B[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMADDSD_VsdHsdVIbWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMADDSD_VsdHsdWsdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBSS_VssHssVIbWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBSS_VssHssWssVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A6F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFMSUBSD_VsdHsdVIbWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFMSUBSD_VsdHsdWsdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A78[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADDPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADDPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A79[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADDPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADDPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7A[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADDSS_VssHssVIbWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADDSS_VssHssWssVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7B[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMADDSD_VsdHsdVIbWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMADDSD_VsdHsdWsdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7C[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUBPS_VpsHpsVIbWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUBPS_VpsHpsWpsVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7D[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUBPD_VpdHpdVIbWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUBPD_VpdHpdWpdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7E[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUBSS_VssHssVIbWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUBSS_VssHssWssVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3A7F[] = { 
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_VFNMSUBSD_VsdHsdVIbWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VFNMSUBSD_VsdHsdWsdVIb)
};

static const Bit64u BxOpcodeGroup_VEX_0F3ACE[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VGF2P8AFFINEQB_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3ACF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_VGF2P8AFFINEINVQB_VdqHdqWdqIb) };
static const Bit64u BxOpcodeGroup_VEX_0F3ADF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_V128_VAESKEYGENASSIST_VdqWdqIb) };

static const Bit64u BxOpcodeGroup_VEX_0F3AF0[] = {
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W0,             BX_IA_RORX_GdEdIb),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_IS64, BX_IA_RORX_GqEqIb)
};

/* ************************************************************************ */

static const Bit64u *BxOpcodeTableVEX[256*3] = {
  // 256 entries for VEX-encoded 0x0F opcodes
  /* 00  */ ( BxOpcodeGroup_ERR ),
  /* 01  */ ( BxOpcodeGroup_ERR ),
  /* 02  */ ( BxOpcodeGroup_ERR ),
  /* 03  */ ( BxOpcodeGroup_ERR ),
  /* 04  */ ( BxOpcodeGroup_ERR ),
  /* 05  */ ( BxOpcodeGroup_ERR ),
  /* 06  */ ( BxOpcodeGroup_ERR ),
  /* 07  */ ( BxOpcodeGroup_ERR ),
  /* 08  */ ( BxOpcodeGroup_ERR ),
  /* 09  */ ( BxOpcodeGroup_ERR ),
  /* 0A  */ ( BxOpcodeGroup_ERR ),
  /* 0B  */ ( BxOpcodeGroup_ERR ),
  /* 0C  */ ( BxOpcodeGroup_ERR ),
  /* 0D  */ ( BxOpcodeGroup_ERR ),
  /* 0E  */ ( BxOpcodeGroup_ERR ),
  /* 0F  */ ( BxOpcodeGroup_ERR ),
  /* 10  */ ( BxOpcodeGroup_VEX_0F10 ),
  /* 11  */ ( BxOpcodeGroup_VEX_0F11 ),
  /* 12  */ ( BxOpcodeGroup_VEX_0F12 ),
  /* 13  */ ( BxOpcodeGroup_VEX_0F13 ),
  /* 14  */ ( BxOpcodeGroup_VEX_0F14 ),
  /* 15  */ ( BxOpcodeGroup_VEX_0F15 ),
  /* 16  */ ( BxOpcodeGroup_VEX_0F16 ),
  /* 17  */ ( BxOpcodeGroup_VEX_0F17 ),
  /* 18  */ ( BxOpcodeGroup_ERR ),
  /* 19  */ ( BxOpcodeGroup_ERR ),
  /* 1A  */ ( BxOpcodeGroup_ERR ),
  /* 1B  */ ( BxOpcodeGroup_ERR ),
  /* 1C  */ ( BxOpcodeGroup_ERR ),
  /* 1D  */ ( BxOpcodeGroup_ERR ),
  /* 1E  */ ( BxOpcodeGroup_ERR ),
  /* 1F  */ ( BxOpcodeGroup_ERR ),
  /* 20  */ ( BxOpcodeGroup_ERR ),
  /* 21  */ ( BxOpcodeGroup_ERR ),
  /* 22  */ ( BxOpcodeGroup_ERR ),
  /* 23  */ ( BxOpcodeGroup_ERR ),
  /* 24  */ ( BxOpcodeGroup_ERR ),
  /* 25  */ ( BxOpcodeGroup_ERR ),
  /* 26  */ ( BxOpcodeGroup_ERR ),
  /* 27  */ ( BxOpcodeGroup_ERR ),
  /* 28  */ ( BxOpcodeGroup_VEX_0F28 ),
  /* 29  */ ( BxOpcodeGroup_VEX_0F29 ),
  /* 2A  */ ( BxOpcodeGroup_VEX_0F2A ),
  /* 2B  */ ( BxOpcodeGroup_VEX_0F2B ),
  /* 2C  */ ( BxOpcodeGroup_VEX_0F2C ),
  /* 2D  */ ( BxOpcodeGroup_VEX_0F2D ),
  /* 2E  */ ( BxOpcodeGroup_VEX_0F2E ),
  /* 2F  */ ( BxOpcodeGroup_VEX_0F2F ),
  /* 30  */ ( BxOpcodeGroup_ERR ),
  /* 31  */ ( BxOpcodeGroup_ERR ),
  /* 32  */ ( BxOpcodeGroup_ERR ),
  /* 33  */ ( BxOpcodeGroup_ERR ),
  /* 34  */ ( BxOpcodeGroup_ERR ),
  /* 35  */ ( BxOpcodeGroup_ERR ),
  /* 36  */ ( BxOpcodeGroup_ERR ),
  /* 37  */ ( BxOpcodeGroup_ERR ),
  /* 38  */ ( BxOpcodeGroup_ERR ), // 3-Byte Escape
  /* 39  */ ( BxOpcodeGroup_ERR ),
  /* 3A  */ ( BxOpcodeGroup_ERR ), // 3-Byte Escape
  /* 3B  */ ( BxOpcodeGroup_ERR ),
  /* 3C  */ ( BxOpcodeGroup_ERR ),
  /* 3D  */ ( BxOpcodeGroup_ERR ),
  /* 3E  */ ( BxOpcodeGroup_ERR ),
  /* 3F  */ ( BxOpcodeGroup_ERR ),
  /* 40  */ ( BxOpcodeGroup_ERR ),
  /* 41  */ ( BxOpcodeGroup_VEX_0F41 ),
  /* 42  */ ( BxOpcodeGroup_VEX_0F42 ),
  /* 43  */ ( BxOpcodeGroup_ERR ),
  /* 44  */ ( BxOpcodeGroup_VEX_0F44 ),
  /* 45  */ ( BxOpcodeGroup_VEX_0F45 ),
  /* 46  */ ( BxOpcodeGroup_VEX_0F46 ),
  /* 47  */ ( BxOpcodeGroup_VEX_0F47 ),
  /* 48  */ ( BxOpcodeGroup_ERR ),
  /* 49  */ ( BxOpcodeGroup_ERR ),
  /* 4A  */ ( BxOpcodeGroup_VEX_0F4A ),
  /* 4B  */ ( BxOpcodeGroup_VEX_0F4B ),
  /* 4C  */ ( BxOpcodeGroup_ERR ),
  /* 4D  */ ( BxOpcodeGroup_ERR ),
  /* 4E  */ ( BxOpcodeGroup_ERR ),
  /* 4F  */ ( BxOpcodeGroup_ERR ),
  /* 50  */ ( BxOpcodeGroup_VEX_0F50 ),
  /* 51  */ ( BxOpcodeGroup_VEX_0F51 ),
  /* 52  */ ( BxOpcodeGroup_VEX_0F52 ),
  /* 53  */ ( BxOpcodeGroup_VEX_0F53 ),
  /* 54  */ ( BxOpcodeGroup_VEX_0F54 ),
  /* 55  */ ( BxOpcodeGroup_VEX_0F55 ),
  /* 56  */ ( BxOpcodeGroup_VEX_0F56 ),
  /* 57  */ ( BxOpcodeGroup_VEX_0F57 ),
  /* 58  */ ( BxOpcodeGroup_VEX_0F58 ),
  /* 59  */ ( BxOpcodeGroup_VEX_0F59 ),
  /* 5A  */ ( BxOpcodeGroup_VEX_0F5A ),
  /* 5B  */ ( BxOpcodeGroup_VEX_0F5B ),
  /* 5C  */ ( BxOpcodeGroup_VEX_0F5C ),
  /* 5D  */ ( BxOpcodeGroup_VEX_0F5D ),
  /* 5E  */ ( BxOpcodeGroup_VEX_0F5E ),
  /* 5F  */ ( BxOpcodeGroup_VEX_0F5F ),
  /* 60  */ ( BxOpcodeGroup_VEX_0F60 ),
  /* 61  */ ( BxOpcodeGroup_VEX_0F61 ),
  /* 62  */ ( BxOpcodeGroup_VEX_0F62 ),
  /* 63  */ ( BxOpcodeGroup_VEX_0F63 ),
  /* 64  */ ( BxOpcodeGroup_VEX_0F64 ),
  /* 65  */ ( BxOpcodeGroup_VEX_0F65 ),
  /* 66  */ ( BxOpcodeGroup_VEX_0F66 ),
  /* 67  */ ( BxOpcodeGroup_VEX_0F67 ),
  /* 68  */ ( BxOpcodeGroup_VEX_0F68 ),
  /* 69  */ ( BxOpcodeGroup_VEX_0F69 ),
  /* 6A  */ ( BxOpcodeGroup_VEX_0F6A ),
  /* 6B  */ ( BxOpcodeGroup_VEX_0F6B ),
  /* 6C  */ ( BxOpcodeGroup_VEX_0F6C ),
  /* 6D  */ ( BxOpcodeGroup_VEX_0F6D ),
  /* 6E  */ ( BxOpcodeGroup_VEX_0F6E ),
  /* 6F  */ ( BxOpcodeGroup_VEX_0F6F ),
  /* 70  */ ( BxOpcodeGroup_VEX_0F70 ),
  /* 71  */ ( BxOpcodeGroup_VEX_0F71 ),
  /* 72  */ ( BxOpcodeGroup_VEX_0F72 ),
  /* 73  */ ( BxOpcodeGroup_VEX_0F73 ),
  /* 74  */ ( BxOpcodeGroup_VEX_0F74 ),
  /* 75  */ ( BxOpcodeGroup_VEX_0F75 ),
  /* 76  */ ( BxOpcodeGroup_VEX_0F76 ),
  /* 77  */ ( BxOpcodeGroup_VEX_0F77 ),
  /* 78  */ ( BxOpcodeGroup_ERR ),
  /* 79  */ ( BxOpcodeGroup_ERR ),
  /* 7A  */ ( BxOpcodeGroup_ERR ),
  /* 7B  */ ( BxOpcodeGroup_ERR ),
  /* 7C  */ ( BxOpcodeGroup_VEX_0F7C ),
  /* 7D  */ ( BxOpcodeGroup_VEX_0F7D ),
  /* 7E  */ ( BxOpcodeGroup_VEX_0F7E ),
  /* 7F  */ ( BxOpcodeGroup_VEX_0F7F ),
  /* 80  */ ( BxOpcodeGroup_ERR ),
  /* 81  */ ( BxOpcodeGroup_ERR ),
  /* 82  */ ( BxOpcodeGroup_ERR ),
  /* 83  */ ( BxOpcodeGroup_ERR ),
  /* 84  */ ( BxOpcodeGroup_ERR ),
  /* 85  */ ( BxOpcodeGroup_ERR ),
  /* 86  */ ( BxOpcodeGroup_ERR ),
  /* 87  */ ( BxOpcodeGroup_ERR ),
  /* 88  */ ( BxOpcodeGroup_ERR ),
  /* 89  */ ( BxOpcodeGroup_ERR ),
  /* 8A  */ ( BxOpcodeGroup_ERR ),
  /* 8B  */ ( BxOpcodeGroup_ERR ),
  /* 8C  */ ( BxOpcodeGroup_ERR ),
  /* 8D  */ ( BxOpcodeGroup_ERR ),
  /* 8E  */ ( BxOpcodeGroup_ERR ),
  /* 8F  */ ( BxOpcodeGroup_ERR ),
  /* 90  */ ( BxOpcodeGroup_VEX_0F90 ),
  /* 91  */ ( BxOpcodeGroup_VEX_0F91 ),
  /* 92  */ ( BxOpcodeGroup_VEX_0F92 ),
  /* 93  */ ( BxOpcodeGroup_VEX_0F93 ),
  /* 94  */ ( BxOpcodeGroup_ERR ),
  /* 95  */ ( BxOpcodeGroup_ERR ),
  /* 96  */ ( BxOpcodeGroup_ERR ),
  /* 97  */ ( BxOpcodeGroup_ERR ),
  /* 98  */ ( BxOpcodeGroup_VEX_0F98 ),
  /* 99  */ ( BxOpcodeGroup_VEX_0F99 ),
  /* 9A  */ ( BxOpcodeGroup_ERR ),
  /* 9B  */ ( BxOpcodeGroup_ERR ),
  /* 9C  */ ( BxOpcodeGroup_ERR ),
  /* 9D  */ ( BxOpcodeGroup_ERR ),
  /* 9E  */ ( BxOpcodeGroup_ERR ),
  /* 9F  */ ( BxOpcodeGroup_ERR ),
  /* A0  */ ( BxOpcodeGroup_ERR ),
  /* A1  */ ( BxOpcodeGroup_ERR ),
  /* A2  */ ( BxOpcodeGroup_ERR ),
  /* A3  */ ( BxOpcodeGroup_ERR ),
  /* A4  */ ( BxOpcodeGroup_ERR ),
  /* A5  */ ( BxOpcodeGroup_ERR ),
  /* A6  */ ( BxOpcodeGroup_ERR ),
  /* A7  */ ( BxOpcodeGroup_ERR ),
  /* A8  */ ( BxOpcodeGroup_ERR ),
  /* A9  */ ( BxOpcodeGroup_ERR ),
  /* AA  */ ( BxOpcodeGroup_ERR ),
  /* AB  */ ( BxOpcodeGroup_ERR ),
  /* AC  */ ( BxOpcodeGroup_ERR ),
  /* AD  */ ( BxOpcodeGroup_ERR ),
  /* AE  */ ( BxOpcodeGroup_VEX_0FAE ),
  /* AF  */ ( BxOpcodeGroup_ERR ),
  /* B0  */ ( BxOpcodeGroup_ERR ),
  /* B1  */ ( BxOpcodeGroup_ERR ),
  /* B2  */ ( BxOpcodeGroup_ERR ),
  /* B3  */ ( BxOpcodeGroup_ERR ),
  /* B4  */ ( BxOpcodeGroup_ERR ),
  /* B5  */ ( BxOpcodeGroup_ERR ),
  /* B6  */ ( BxOpcodeGroup_ERR ),
  /* B7  */ ( BxOpcodeGroup_ERR ),
  /* B8  */ ( BxOpcodeGroup_ERR ),
  /* B9  */ ( BxOpcodeGroup_ERR ),
  /* BA  */ ( BxOpcodeGroup_ERR ),
  /* BB  */ ( BxOpcodeGroup_ERR ),
  /* BC  */ ( BxOpcodeGroup_ERR ),
  /* BD  */ ( BxOpcodeGroup_ERR ),
  /* BE  */ ( BxOpcodeGroup_ERR ),
  /* BF  */ ( BxOpcodeGroup_ERR ),
  /* C0  */ ( BxOpcodeGroup_ERR ),
  /* C1  */ ( BxOpcodeGroup_ERR ),
  /* C2  */ ( BxOpcodeGroup_VEX_0FC2 ),
  /* C3  */ ( BxOpcodeGroup_ERR ),
  /* C4  */ ( BxOpcodeGroup_VEX_0FC4 ),
  /* C5  */ ( BxOpcodeGroup_VEX_0FC5 ),
  /* C6  */ ( BxOpcodeGroup_VEX_0FC6 ),
  /* C7  */ ( BxOpcodeGroup_ERR ),
  /* C8  */ ( BxOpcodeGroup_ERR ),
  /* C9  */ ( BxOpcodeGroup_ERR ),
  /* CA  */ ( BxOpcodeGroup_ERR ),
  /* CB  */ ( BxOpcodeGroup_ERR ),
  /* CC  */ ( BxOpcodeGroup_ERR ),
  /* CD  */ ( BxOpcodeGroup_ERR ),
  /* CE  */ ( BxOpcodeGroup_ERR ),
  /* CF  */ ( BxOpcodeGroup_ERR ),
  /* D0  */ ( BxOpcodeGroup_VEX_0FD0 ),
  /* D1  */ ( BxOpcodeGroup_VEX_0FD1 ),
  /* D2  */ ( BxOpcodeGroup_VEX_0FD2 ),
  /* D3  */ ( BxOpcodeGroup_VEX_0FD3 ),
  /* D4  */ ( BxOpcodeGroup_VEX_0FD4 ),
  /* D5  */ ( BxOpcodeGroup_VEX_0FD5 ),
  /* D6  */ ( BxOpcodeGroup_VEX_0FD6 ),
  /* D7  */ ( BxOpcodeGroup_VEX_0FD7 ),
  /* D8  */ ( BxOpcodeGroup_VEX_0FD8 ),
  /* D9  */ ( BxOpcodeGroup_VEX_0FD9 ),
  /* DA  */ ( BxOpcodeGroup_VEX_0FDA ),
  /* DB  */ ( BxOpcodeGroup_VEX_0FDB ),
  /* DC  */ ( BxOpcodeGroup_VEX_0FDC ),
  /* DD  */ ( BxOpcodeGroup_VEX_0FDD ),
  /* DE  */ ( BxOpcodeGroup_VEX_0FDE ),
  /* DF  */ ( BxOpcodeGroup_VEX_0FDF ),
  /* E0  */ ( BxOpcodeGroup_VEX_0FE0 ),
  /* E1  */ ( BxOpcodeGroup_VEX_0FE1 ),
  /* E2  */ ( BxOpcodeGroup_VEX_0FE2 ),
  /* E3  */ ( BxOpcodeGroup_VEX_0FE3 ),
  /* E4  */ ( BxOpcodeGroup_VEX_0FE4 ),
  /* E5  */ ( BxOpcodeGroup_VEX_0FE5 ),
  /* E6  */ ( BxOpcodeGroup_VEX_0FE6 ),
  /* E7  */ ( BxOpcodeGroup_VEX_0FE7 ),
  /* E8  */ ( BxOpcodeGroup_VEX_0FE8 ),
  /* E9  */ ( BxOpcodeGroup_VEX_0FE9 ),
  /* EA  */ ( BxOpcodeGroup_VEX_0FEA ),
  /* EB  */ ( BxOpcodeGroup_VEX_0FEB ),
  /* EC  */ ( BxOpcodeGroup_VEX_0FEC ),
  /* ED  */ ( BxOpcodeGroup_VEX_0FED ),
  /* EE  */ ( BxOpcodeGroup_VEX_0FEE ),
  /* EF  */ ( BxOpcodeGroup_VEX_0FEF ),
  /* F0  */ ( BxOpcodeGroup_VEX_0FF0 ),
  /* F1  */ ( BxOpcodeGroup_VEX_0FF1 ),
  /* F2  */ ( BxOpcodeGroup_VEX_0FF2 ),
  /* F3  */ ( BxOpcodeGroup_VEX_0FF3 ),
  /* F4  */ ( BxOpcodeGroup_VEX_0FF4 ),
  /* F5  */ ( BxOpcodeGroup_VEX_0FF5 ),
  /* F6  */ ( BxOpcodeGroup_VEX_0FF6 ),
  /* F7  */ ( BxOpcodeGroup_VEX_0FF7 ),
  /* F8  */ ( BxOpcodeGroup_VEX_0FF8 ),
  /* F9  */ ( BxOpcodeGroup_VEX_0FF9 ),
  /* FA  */ ( BxOpcodeGroup_VEX_0FFA ),
  /* FB  */ ( BxOpcodeGroup_VEX_0FFB ),
  /* FC  */ ( BxOpcodeGroup_VEX_0FFC ),
  /* FD  */ ( BxOpcodeGroup_VEX_0FFD ),
  /* FE  */ ( BxOpcodeGroup_VEX_0FFE ),
  /* FF  */ ( BxOpcodeGroup_ERR ),

  // 256 entries for VEX-encoded 0x0F 0x38 opcodes
  /* 00  */ ( BxOpcodeGroup_VEX_0F3800 ),
  /* 01  */ ( BxOpcodeGroup_VEX_0F3801 ),
  /* 02  */ ( BxOpcodeGroup_VEX_0F3802 ),
  /* 03  */ ( BxOpcodeGroup_VEX_0F3803 ),
  /* 04  */ ( BxOpcodeGroup_VEX_0F3804 ),
  /* 05  */ ( BxOpcodeGroup_VEX_0F3805 ),
  /* 06  */ ( BxOpcodeGroup_VEX_0F3806 ),
  /* 07  */ ( BxOpcodeGroup_VEX_0F3807 ),
  /* 08  */ ( BxOpcodeGroup_VEX_0F3808 ),
  /* 09  */ ( BxOpcodeGroup_VEX_0F3809 ),
  /* 0A  */ ( BxOpcodeGroup_VEX_0F380A ),
  /* 0B  */ ( BxOpcodeGroup_VEX_0F380B ),
  /* 0C  */ ( BxOpcodeGroup_VEX_0F380C ),
  /* 0D  */ ( BxOpcodeGroup_VEX_0F380D ),
  /* 0E  */ ( BxOpcodeGroup_VEX_0F380E ),
  /* 0F  */ ( BxOpcodeGroup_VEX_0F380F ),
  /* 10  */ ( BxOpcodeGroup_ERR ),
  /* 11  */ ( BxOpcodeGroup_ERR ),
  /* 12  */ ( BxOpcodeGroup_ERR ),
  /* 13  */ ( BxOpcodeGroup_VEX_0F3813 ),
  /* 14  */ ( BxOpcodeGroup_ERR ),
  /* 15  */ ( BxOpcodeGroup_ERR ),
  /* 16  */ ( BxOpcodeGroup_VEX_0F3816 ),
  /* 17  */ ( BxOpcodeGroup_VEX_0F3817 ),
  /* 18  */ ( BxOpcodeGroup_VEX_0F3818 ),
  /* 19  */ ( BxOpcodeGroup_VEX_0F3819 ),
  /* 1A  */ ( BxOpcodeGroup_VEX_0F381A ),
  /* 1B  */ ( BxOpcodeGroup_ERR ),
  /* 1C  */ ( BxOpcodeGroup_VEX_0F381C ),
  /* 1D  */ ( BxOpcodeGroup_VEX_0F381D ),
  /* 1E  */ ( BxOpcodeGroup_VEX_0F381E ),
  /* 1F  */ ( BxOpcodeGroup_ERR ),
  /* 20  */ ( BxOpcodeGroup_VEX_0F3820 ),
  /* 21  */ ( BxOpcodeGroup_VEX_0F3821 ),
  /* 22  */ ( BxOpcodeGroup_VEX_0F3822 ),
  /* 23  */ ( BxOpcodeGroup_VEX_0F3823 ),
  /* 24  */ ( BxOpcodeGroup_VEX_0F3824 ),
  /* 25  */ ( BxOpcodeGroup_VEX_0F3825 ),
  /* 26  */ ( BxOpcodeGroup_ERR ),
  /* 27  */ ( BxOpcodeGroup_ERR ),
  /* 28  */ ( BxOpcodeGroup_VEX_0F3828 ),
  /* 29  */ ( BxOpcodeGroup_VEX_0F3829 ),
  /* 2A  */ ( BxOpcodeGroup_VEX_0F382A ),
  /* 2B  */ ( BxOpcodeGroup_VEX_0F382B ),
  /* 2C  */ ( BxOpcodeGroup_VEX_0F382C ),
  /* 2D  */ ( BxOpcodeGroup_VEX_0F382D ),
  /* 2E  */ ( BxOpcodeGroup_VEX_0F382E ),
  /* 2F  */ ( BxOpcodeGroup_VEX_0F382F ),
  /* 30  */ ( BxOpcodeGroup_VEX_0F3830 ),
  /* 31  */ ( BxOpcodeGroup_VEX_0F3831 ),
  /* 32  */ ( BxOpcodeGroup_VEX_0F3832 ),
  /* 33  */ ( BxOpcodeGroup_VEX_0F3833 ),
  /* 34  */ ( BxOpcodeGroup_VEX_0F3834 ),
  /* 35  */ ( BxOpcodeGroup_VEX_0F3835 ),
  /* 36  */ ( BxOpcodeGroup_VEX_0F3836 ),
  /* 37  */ ( BxOpcodeGroup_VEX_0F3837 ),
  /* 38  */ ( BxOpcodeGroup_VEX_0F3838 ),
  /* 39  */ ( BxOpcodeGroup_VEX_0F3839 ),
  /* 3A  */ ( BxOpcodeGroup_VEX_0F383A ),
  /* 3B  */ ( BxOpcodeGroup_VEX_0F383B ),
  /* 3C  */ ( BxOpcodeGroup_VEX_0F383C ),
  /* 3D  */ ( BxOpcodeGroup_VEX_0F383D ),
  /* 3E  */ ( BxOpcodeGroup_VEX_0F383E ),
  /* 3F  */ ( BxOpcodeGroup_VEX_0F383F ),
  /* 40  */ ( BxOpcodeGroup_VEX_0F3840 ),
  /* 41  */ ( BxOpcodeGroup_VEX_0F3841 ),
  /* 42  */ ( BxOpcodeGroup_ERR ),
  /* 43  */ ( BxOpcodeGroup_ERR ),
  /* 44  */ ( BxOpcodeGroup_ERR ),
  /* 45  */ ( BxOpcodeGroup_VEX_0F3845 ),
  /* 46  */ ( BxOpcodeGroup_VEX_0F3846 ),
  /* 47  */ ( BxOpcodeGroup_VEX_0F3847 ),
  /* 48  */ ( BxOpcodeGroup_ERR ),
  /* 49  */ ( BxOpcodeGroup_ERR ),
  /* 4A  */ ( BxOpcodeGroup_ERR ),
  /* 4B  */ ( BxOpcodeGroup_ERR ),
  /* 4C  */ ( BxOpcodeGroup_ERR ),
  /* 4D  */ ( BxOpcodeGroup_ERR ),
  /* 4E  */ ( BxOpcodeGroup_ERR ),
  /* 4F  */ ( BxOpcodeGroup_ERR ),
  /* 50  */ ( BxOpcodeGroup_ERR ),
  /* 51  */ ( BxOpcodeGroup_ERR ),
  /* 52  */ ( BxOpcodeGroup_ERR ),
  /* 53  */ ( BxOpcodeGroup_ERR ),
  /* 54  */ ( BxOpcodeGroup_ERR ),
  /* 55  */ ( BxOpcodeGroup_ERR ),
  /* 56  */ ( BxOpcodeGroup_ERR ),
  /* 57  */ ( BxOpcodeGroup_ERR ),
  /* 58  */ ( BxOpcodeGroup_VEX_0F3858 ),
  /* 59  */ ( BxOpcodeGroup_VEX_0F3859 ),
  /* 5A  */ ( BxOpcodeGroup_VEX_0F385A ),
  /* 5B  */ ( BxOpcodeGroup_ERR ),
  /* 5C  */ ( BxOpcodeGroup_ERR ),
  /* 5D  */ ( BxOpcodeGroup_ERR ),
  /* 5E  */ ( BxOpcodeGroup_ERR ),
  /* 5F  */ ( BxOpcodeGroup_ERR ),
  /* 60  */ ( BxOpcodeGroup_ERR ),
  /* 61  */ ( BxOpcodeGroup_ERR ),
  /* 62  */ ( BxOpcodeGroup_ERR ),
  /* 63  */ ( BxOpcodeGroup_ERR ),
  /* 64  */ ( BxOpcodeGroup_ERR ),
  /* 65  */ ( BxOpcodeGroup_ERR ),
  /* 66  */ ( BxOpcodeGroup_ERR ),
  /* 67  */ ( BxOpcodeGroup_ERR ),
  /* 68  */ ( BxOpcodeGroup_ERR ),
  /* 69  */ ( BxOpcodeGroup_ERR ),
  /* 6A  */ ( BxOpcodeGroup_ERR ),
  /* 6B  */ ( BxOpcodeGroup_ERR ),
  /* 6C  */ ( BxOpcodeGroup_ERR ),
  /* 6D  */ ( BxOpcodeGroup_ERR ),
  /* 6E  */ ( BxOpcodeGroup_ERR ),
  /* 6F  */ ( BxOpcodeGroup_ERR ),
  /* 70  */ ( BxOpcodeGroup_ERR ),
  /* 71  */ ( BxOpcodeGroup_ERR ),
  /* 72  */ ( BxOpcodeGroup_ERR ),
  /* 73  */ ( BxOpcodeGroup_ERR ),
  /* 74  */ ( BxOpcodeGroup_ERR ),
  /* 75  */ ( BxOpcodeGroup_ERR ),
  /* 76  */ ( BxOpcodeGroup_ERR ),
  /* 77  */ ( BxOpcodeGroup_ERR ),
  /* 78  */ ( BxOpcodeGroup_VEX_0F3878 ),
  /* 79  */ ( BxOpcodeGroup_VEX_0F3879 ),
  /* 7A  */ ( BxOpcodeGroup_ERR ),
  /* 7B  */ ( BxOpcodeGroup_ERR ),
  /* 7C  */ ( BxOpcodeGroup_ERR ),
  /* 7D  */ ( BxOpcodeGroup_ERR ),
  /* 7E  */ ( BxOpcodeGroup_ERR ),
  /* 7F  */ ( BxOpcodeGroup_ERR ),
  /* 80  */ ( BxOpcodeGroup_ERR ),
  /* 81  */ ( BxOpcodeGroup_ERR ),
  /* 82  */ ( BxOpcodeGroup_ERR ),
  /* 83  */ ( BxOpcodeGroup_ERR ),
  /* 84  */ ( BxOpcodeGroup_ERR ),
  /* 85  */ ( BxOpcodeGroup_ERR ),
  /* 86  */ ( BxOpcodeGroup_ERR ),
  /* 87  */ ( BxOpcodeGroup_ERR ),
  /* 88  */ ( BxOpcodeGroup_ERR ),
  /* 89  */ ( BxOpcodeGroup_ERR ),
  /* 8A  */ ( BxOpcodeGroup_ERR ),
  /* 8B  */ ( BxOpcodeGroup_ERR ),
  /* 8C  */ ( BxOpcodeGroup_VEX_0F388C ),
  /* 8D  */ ( BxOpcodeGroup_ERR ),
  /* 8E  */ ( BxOpcodeGroup_VEX_0F388E ),
  /* 8F  */ ( BxOpcodeGroup_ERR ),
  /* 90  */ ( BxOpcodeGroup_VEX_0F3890 ),
  /* 91  */ ( BxOpcodeGroup_VEX_0F3891 ),
  /* 92  */ ( BxOpcodeGroup_VEX_0F3892 ),
  /* 93  */ ( BxOpcodeGroup_VEX_0F3893 ),
  /* 94  */ ( BxOpcodeGroup_ERR ),
  /* 95  */ ( BxOpcodeGroup_ERR ),
  /* 96  */ ( BxOpcodeGroup_VEX_0F3896 ),
  /* 97  */ ( BxOpcodeGroup_VEX_0F3897 ),
  /* 98  */ ( BxOpcodeGroup_VEX_0F3898 ),
  /* 99  */ ( BxOpcodeGroup_VEX_0F3899 ),
  /* 9A  */ ( BxOpcodeGroup_VEX_0F389A ),
  /* 9B  */ ( BxOpcodeGroup_VEX_0F389B ),
  /* 9C  */ ( BxOpcodeGroup_VEX_0F389C ),
  /* 9D  */ ( BxOpcodeGroup_VEX_0F389D ),
  /* 9E  */ ( BxOpcodeGroup_VEX_0F389E ),
  /* 9F  */ ( BxOpcodeGroup_VEX_0F389F ),
  /* A0  */ ( BxOpcodeGroup_ERR ),
  /* A1  */ ( BxOpcodeGroup_ERR ),
  /* A2  */ ( BxOpcodeGroup_ERR ),
  /* A3  */ ( BxOpcodeGroup_ERR ),
  /* A4  */ ( BxOpcodeGroup_ERR ),
  /* A5  */ ( BxOpcodeGroup_ERR ),
  /* A6  */ ( BxOpcodeGroup_VEX_0F38A6 ),
  /* A7  */ ( BxOpcodeGroup_VEX_0F38A7 ),
  /* A8  */ ( BxOpcodeGroup_VEX_0F38A8 ),
  /* A9  */ ( BxOpcodeGroup_VEX_0F38A9 ),
  /* AA  */ ( BxOpcodeGroup_VEX_0F38AA ),
  /* AB  */ ( BxOpcodeGroup_VEX_0F38AB ),
  /* AC  */ ( BxOpcodeGroup_VEX_0F38AC ),
  /* AD  */ ( BxOpcodeGroup_VEX_0F38AD ),
  /* AE  */ ( BxOpcodeGroup_VEX_0F38AE ),
  /* AF  */ ( BxOpcodeGroup_VEX_0F38AF ),
  /* B0  */ ( BxOpcodeGroup_ERR ),
  /* B1  */ ( BxOpcodeGroup_ERR ),
  /* B2  */ ( BxOpcodeGroup_ERR ),
  /* B3  */ ( BxOpcodeGroup_ERR ),
  /* B4  */ ( BxOpcodeGroup_ERR ),
  /* B5  */ ( BxOpcodeGroup_ERR ),
  /* B6  */ ( BxOpcodeGroup_VEX_0F38B6 ),
  /* B7  */ ( BxOpcodeGroup_VEX_0F38B7 ),
  /* B8  */ ( BxOpcodeGroup_VEX_0F38B8 ),
  /* B9  */ ( BxOpcodeGroup_VEX_0F38B9 ),
  /* BA  */ ( BxOpcodeGroup_VEX_0F38BA ),
  /* BB  */ ( BxOpcodeGroup_VEX_0F38BB ),
  /* BC  */ ( BxOpcodeGroup_VEX_0F38BC ),
  /* BD  */ ( BxOpcodeGroup_VEX_0F38BD ),
  /* BE  */ ( BxOpcodeGroup_VEX_0F38BE ),
  /* BF  */ ( BxOpcodeGroup_VEX_0F38BF ),
  /* C0  */ ( BxOpcodeGroup_ERR ),
  /* C1  */ ( BxOpcodeGroup_ERR ),
  /* C2  */ ( BxOpcodeGroup_ERR ),
  /* C3  */ ( BxOpcodeGroup_ERR ),
  /* C4  */ ( BxOpcodeGroup_ERR ),
  /* C5  */ ( BxOpcodeGroup_ERR ),
  /* C6  */ ( BxOpcodeGroup_ERR ),
  /* C7  */ ( BxOpcodeGroup_ERR ),
  /* C8  */ ( BxOpcodeGroup_ERR ),
  /* C9  */ ( BxOpcodeGroup_ERR ),
  /* CA  */ ( BxOpcodeGroup_ERR ),
  /* CB  */ ( BxOpcodeGroup_ERR ),
  /* CC  */ ( BxOpcodeGroup_ERR ),
  /* CD  */ ( BxOpcodeGroup_ERR ),
  /* CE  */ ( BxOpcodeGroup_ERR ),
  /* CF  */ ( BxOpcodeGroup_VEX_0F38CF ),
  /* D0  */ ( BxOpcodeGroup_ERR ),
  /* D1  */ ( BxOpcodeGroup_ERR ),
  /* D2  */ ( BxOpcodeGroup_ERR ),
  /* D3  */ ( BxOpcodeGroup_ERR ),
  /* D4  */ ( BxOpcodeGroup_ERR ),
  /* D5  */ ( BxOpcodeGroup_ERR ),
  /* D6  */ ( BxOpcodeGroup_ERR ),
  /* D7  */ ( BxOpcodeGroup_ERR ),
  /* D8  */ ( BxOpcodeGroup_ERR ),
  /* D9  */ ( BxOpcodeGroup_ERR ),
  /* DA  */ ( BxOpcodeGroup_ERR ),
  /* DB  */ ( BxOpcodeGroup_VEX_0F38DB ),
  /* DC  */ ( BxOpcodeGroup_VEX_0F38DC ),
  /* DD  */ ( BxOpcodeGroup_VEX_0F38DD ),
  /* DF  */ ( BxOpcodeGroup_VEX_0F38DE ),
  /* DE  */ ( BxOpcodeGroup_VEX_0F38DF ),
  /* E0  */ ( BxOpcodeGroup_ERR ),
  /* E1  */ ( BxOpcodeGroup_ERR ),
  /* E2  */ ( BxOpcodeGroup_ERR ),
  /* E3  */ ( BxOpcodeGroup_ERR ),
  /* E4  */ ( BxOpcodeGroup_ERR ),
  /* E5  */ ( BxOpcodeGroup_ERR ),
  /* E6  */ ( BxOpcodeGroup_ERR ),
  /* E7  */ ( BxOpcodeGroup_ERR ),
  /* E8  */ ( BxOpcodeGroup_ERR ),
  /* E9  */ ( BxOpcodeGroup_ERR ),
  /* EA  */ ( BxOpcodeGroup_ERR ),
  /* EB  */ ( BxOpcodeGroup_ERR ),
  /* EC  */ ( BxOpcodeGroup_ERR ),
  /* ED  */ ( BxOpcodeGroup_ERR ),
  /* EE  */ ( BxOpcodeGroup_ERR ),
  /* EF  */ ( BxOpcodeGroup_ERR ),
  /* F0  */ ( BxOpcodeGroup_ERR ),
  /* F1  */ ( BxOpcodeGroup_ERR ),
  /* F2  */ ( BxOpcodeGroup_VEX_0F38F2 ),
  /* F3  */ ( BxOpcodeGroup_VEX_0F38F3 ),
  /* F4  */ ( BxOpcodeGroup_ERR ),
  /* F5  */ ( BxOpcodeGroup_VEX_0F38F5 ),
  /* F6  */ ( BxOpcodeGroup_VEX_0F38F6 ),
  /* F7  */ ( BxOpcodeGroup_VEX_0F38F7 ),
  /* F8  */ ( BxOpcodeGroup_ERR ),
  /* F9  */ ( BxOpcodeGroup_ERR ),
  /* FA  */ ( BxOpcodeGroup_ERR ),
  /* FB  */ ( BxOpcodeGroup_ERR ),
  /* FC  */ ( BxOpcodeGroup_ERR ),
  /* FD  */ ( BxOpcodeGroup_ERR ),
  /* FE  */ ( BxOpcodeGroup_ERR ),
  /* FF  */ ( BxOpcodeGroup_ERR ),

  // 256 entries for VEX-encoded 0x0F 0x3A opcodes
  /* 00  */ ( BxOpcodeGroup_VEX_0F3A00 ),
  /* 01  */ ( BxOpcodeGroup_VEX_0F3A01 ),
  /* 02  */ ( BxOpcodeGroup_VEX_0F3A02 ),
  /* 03  */ ( BxOpcodeGroup_ERR ),
  /* 04  */ ( BxOpcodeGroup_VEX_0F3A04 ),
  /* 05  */ ( BxOpcodeGroup_VEX_0F3A05 ),
  /* 06  */ ( BxOpcodeGroup_VEX_0F3A06 ),
  /* 07  */ ( BxOpcodeGroup_ERR ),
  /* 08  */ ( BxOpcodeGroup_VEX_0F3A08 ),
  /* 09  */ ( BxOpcodeGroup_VEX_0F3A09 ),
  /* 0A  */ ( BxOpcodeGroup_VEX_0F3A0A ),
  /* 0B  */ ( BxOpcodeGroup_VEX_0F3A0B ),
  /* 0C  */ ( BxOpcodeGroup_VEX_0F3A0C ),
  /* 0D  */ ( BxOpcodeGroup_VEX_0F3A0D ),
  /* 0E  */ ( BxOpcodeGroup_VEX_0F3A0E ),
  /* 0F  */ ( BxOpcodeGroup_VEX_0F3A0F ),
  /* 10  */ ( BxOpcodeGroup_ERR ),
  /* 11  */ ( BxOpcodeGroup_ERR ),
  /* 12  */ ( BxOpcodeGroup_ERR ),
  /* 13  */ ( BxOpcodeGroup_ERR ),
  /* 14  */ ( BxOpcodeGroup_VEX_0F3A14 ),
  /* 15  */ ( BxOpcodeGroup_VEX_0F3A15 ),
  /* 16  */ ( BxOpcodeGroup_VEX_0F3A16 ),
  /* 17  */ ( BxOpcodeGroup_VEX_0F3A17 ),
  /* 18  */ ( BxOpcodeGroup_VEX_0F3A18 ),
  /* 19  */ ( BxOpcodeGroup_VEX_0F3A19 ),
  /* 1A  */ ( BxOpcodeGroup_ERR ),
  /* 1B  */ ( BxOpcodeGroup_ERR ),
  /* 1C  */ ( BxOpcodeGroup_ERR ),
  /* 1D  */ ( BxOpcodeGroup_VEX_0F3A1D ),
  /* 1E  */ ( BxOpcodeGroup_ERR ),
  /* 1F  */ ( BxOpcodeGroup_ERR ),
  /* 20  */ ( BxOpcodeGroup_VEX_0F3A20 ),
  /* 21  */ ( BxOpcodeGroup_VEX_0F3A21 ),
  /* 22  */ ( BxOpcodeGroup_VEX_0F3A22 ),
  /* 23  */ ( BxOpcodeGroup_ERR ),
  /* 24  */ ( BxOpcodeGroup_ERR ),
  /* 25  */ ( BxOpcodeGroup_ERR ),
  /* 26  */ ( BxOpcodeGroup_ERR ),
  /* 27  */ ( BxOpcodeGroup_ERR ),
  /* 28  */ ( BxOpcodeGroup_ERR ),
  /* 29  */ ( BxOpcodeGroup_ERR ),
  /* 2A  */ ( BxOpcodeGroup_ERR ),
  /* 2B  */ ( BxOpcodeGroup_ERR ),
  /* 2C  */ ( BxOpcodeGroup_ERR ),
  /* 2D  */ ( BxOpcodeGroup_ERR ),
  /* 2E  */ ( BxOpcodeGroup_ERR ),
  /* 2F  */ ( BxOpcodeGroup_ERR ),
  /* 30  */ ( BxOpcodeGroup_VEX_0F3A30 ),
  /* 31  */ ( BxOpcodeGroup_VEX_0F3A31 ),
  /* 32  */ ( BxOpcodeGroup_VEX_0F3A32 ),
  /* 33  */ ( BxOpcodeGroup_VEX_0F3A33 ),
  /* 34  */ ( BxOpcodeGroup_ERR ),
  /* 35  */ ( BxOpcodeGroup_ERR ),
  /* 36  */ ( BxOpcodeGroup_ERR ),
  /* 37  */ ( BxOpcodeGroup_ERR ),
  /* 38  */ ( BxOpcodeGroup_VEX_0F3A38 ),
  /* 39  */ ( BxOpcodeGroup_VEX_0F3A39 ),
  /* 3A  */ ( BxOpcodeGroup_ERR ),
  /* 3B  */ ( BxOpcodeGroup_ERR ),
  /* 3C  */ ( BxOpcodeGroup_ERR ),
  /* 3D  */ ( BxOpcodeGroup_ERR ),
  /* 3E  */ ( BxOpcodeGroup_ERR ),
  /* 3F  */ ( BxOpcodeGroup_ERR ),
  /* 40  */ ( BxOpcodeGroup_VEX_0F3A40 ),
  /* 41  */ ( BxOpcodeGroup_VEX_0F3A41 ),
  /* 42  */ ( BxOpcodeGroup_VEX_0F3A42 ),
  /* 43  */ ( BxOpcodeGroup_ERR ),
  /* 44  */ ( BxOpcodeGroup_VEX_0F3A44 ),
  /* 45  */ ( BxOpcodeGroup_ERR ),
  /* 46  */ ( BxOpcodeGroup_VEX_0F3A46 ),
  /* 47  */ ( BxOpcodeGroup_ERR ),
  /* 48  */ ( BxOpcodeGroup_VEX_0F3A48 ),
  /* 49  */ ( BxOpcodeGroup_VEX_0F3A49 ),
  /* 4A  */ ( BxOpcodeGroup_VEX_0F3A4A ),
  /* 4B  */ ( BxOpcodeGroup_VEX_0F3A4B ),
  /* 4C  */ ( BxOpcodeGroup_VEX_0F3A4C ),
  /* 4D  */ ( BxOpcodeGroup_ERR ),
  /* 4E  */ ( BxOpcodeGroup_ERR ),
  /* 4F  */ ( BxOpcodeGroup_ERR ),
  /* 50  */ ( BxOpcodeGroup_ERR ),
  /* 51  */ ( BxOpcodeGroup_ERR ),
  /* 52  */ ( BxOpcodeGroup_ERR ),
  /* 53  */ ( BxOpcodeGroup_ERR ),
  /* 54  */ ( BxOpcodeGroup_ERR ),
  /* 55  */ ( BxOpcodeGroup_ERR ),
  /* 56  */ ( BxOpcodeGroup_ERR ),
  /* 57  */ ( BxOpcodeGroup_ERR ),
  /* 58  */ ( BxOpcodeGroup_ERR ),
  /* 59  */ ( BxOpcodeGroup_ERR ),
  /* 5A  */ ( BxOpcodeGroup_ERR ),
  /* 5B  */ ( BxOpcodeGroup_ERR ),
  /* 5C  */ ( BxOpcodeGroup_VEX_0F3A5C ),
  /* 5D  */ ( BxOpcodeGroup_VEX_0F3A5D ),
  /* 5E  */ ( BxOpcodeGroup_VEX_0F3A5E ),
  /* 5F  */ ( BxOpcodeGroup_VEX_0F3A5F ),
  /* 60  */ ( BxOpcodeGroup_VEX_0F3A60 ),
  /* 61  */ ( BxOpcodeGroup_VEX_0F3A61 ),
  /* 62  */ ( BxOpcodeGroup_VEX_0F3A62 ),
  /* 63  */ ( BxOpcodeGroup_VEX_0F3A63 ),
  /* 64  */ ( BxOpcodeGroup_ERR ),
  /* 65  */ ( BxOpcodeGroup_ERR ),
  /* 66  */ ( BxOpcodeGroup_ERR ),
  /* 67  */ ( BxOpcodeGroup_ERR ),
  /* 68  */ ( BxOpcodeGroup_VEX_0F3A68 ),
  /* 69  */ ( BxOpcodeGroup_VEX_0F3A69 ),
  /* 6A  */ ( BxOpcodeGroup_VEX_0F3A6A ),
  /* 6B  */ ( BxOpcodeGroup_VEX_0F3A6B ),
  /* 6C  */ ( BxOpcodeGroup_VEX_0F3A6C ),
  /* 6D  */ ( BxOpcodeGroup_VEX_0F3A6D ),
  /* 6E  */ ( BxOpcodeGroup_VEX_0F3A6E ),
  /* 6F  */ ( BxOpcodeGroup_VEX_0F3A6F ),
  /* 70  */ ( BxOpcodeGroup_ERR ),
  /* 71  */ ( BxOpcodeGroup_ERR ),
  /* 72  */ ( BxOpcodeGroup_ERR ),
  /* 73  */ ( BxOpcodeGroup_ERR ),
  /* 74  */ ( BxOpcodeGroup_ERR ),
  /* 75  */ ( BxOpcodeGroup_ERR ),
  /* 76  */ ( BxOpcodeGroup_ERR ),
  /* 77  */ ( BxOpcodeGroup_ERR ),
  /* 78  */ ( BxOpcodeGroup_VEX_0F3A78 ),
  /* 79  */ ( BxOpcodeGroup_VEX_0F3A79 ),
  /* 7A  */ ( BxOpcodeGroup_VEX_0F3A7A ),
  /* 7B  */ ( BxOpcodeGroup_VEX_0F3A7B ),
  /* 7C  */ ( BxOpcodeGroup_VEX_0F3A7C ),
  /* 7D  */ ( BxOpcodeGroup_VEX_0F3A7D ),
  /* 7E  */ ( BxOpcodeGroup_VEX_0F3A7E ),
  /* 7F  */ ( BxOpcodeGroup_VEX_0F3A7F ),
  /* 80  */ ( BxOpcodeGroup_ERR ),
  /* 81  */ ( BxOpcodeGroup_ERR ),
  /* 82  */ ( BxOpcodeGroup_ERR ),
  /* 83  */ ( BxOpcodeGroup_ERR ),
  /* 84  */ ( BxOpcodeGroup_ERR ),
  /* 85  */ ( BxOpcodeGroup_ERR ),
  /* 86  */ ( BxOpcodeGroup_ERR ),
  /* 87  */ ( BxOpcodeGroup_ERR ),
  /* 88  */ ( BxOpcodeGroup_ERR ),
  /* 89  */ ( BxOpcodeGroup_ERR ),
  /* 8A  */ ( BxOpcodeGroup_ERR ),
  /* 8B  */ ( BxOpcodeGroup_ERR ),
  /* 8C  */ ( BxOpcodeGroup_ERR ),
  /* 8D  */ ( BxOpcodeGroup_ERR ),
  /* 8E  */ ( BxOpcodeGroup_ERR ),
  /* 8F  */ ( BxOpcodeGroup_ERR ),
  /* 90  */ ( BxOpcodeGroup_ERR ),
  /* 91  */ ( BxOpcodeGroup_ERR ),
  /* 92  */ ( BxOpcodeGroup_ERR ),
  /* 93  */ ( BxOpcodeGroup_ERR ),
  /* 94  */ ( BxOpcodeGroup_ERR ),
  /* 95  */ ( BxOpcodeGroup_ERR ),
  /* 96  */ ( BxOpcodeGroup_ERR ),
  /* 97  */ ( BxOpcodeGroup_ERR ),
  /* 98  */ ( BxOpcodeGroup_ERR ),
  /* 99  */ ( BxOpcodeGroup_ERR ),
  /* 9A  */ ( BxOpcodeGroup_ERR ),
  /* 9B  */ ( BxOpcodeGroup_ERR ),
  /* 9C  */ ( BxOpcodeGroup_ERR ),
  /* 9D  */ ( BxOpcodeGroup_ERR ),
  /* 9E  */ ( BxOpcodeGroup_ERR ),
  /* 9F  */ ( BxOpcodeGroup_ERR ),
  /* A0  */ ( BxOpcodeGroup_ERR ),
  /* A1  */ ( BxOpcodeGroup_ERR ),
  /* A2  */ ( BxOpcodeGroup_ERR ),
  /* A3  */ ( BxOpcodeGroup_ERR ),
  /* A4  */ ( BxOpcodeGroup_ERR ),
  /* A5  */ ( BxOpcodeGroup_ERR ),
  /* A6  */ ( BxOpcodeGroup_ERR ),
  /* A7  */ ( BxOpcodeGroup_ERR ),
  /* A8  */ ( BxOpcodeGroup_ERR ),
  /* A9  */ ( BxOpcodeGroup_ERR ),
  /* AA  */ ( BxOpcodeGroup_ERR ),
  /* AB  */ ( BxOpcodeGroup_ERR ),
  /* AC  */ ( BxOpcodeGroup_ERR ),
  /* AD  */ ( BxOpcodeGroup_ERR ),
  /* AE  */ ( BxOpcodeGroup_ERR ),
  /* AF  */ ( BxOpcodeGroup_ERR ),
  /* B0  */ ( BxOpcodeGroup_ERR ),
  /* B1  */ ( BxOpcodeGroup_ERR ),
  /* B2  */ ( BxOpcodeGroup_ERR ),
  /* B3  */ ( BxOpcodeGroup_ERR ),
  /* B4  */ ( BxOpcodeGroup_ERR ),
  /* B5  */ ( BxOpcodeGroup_ERR ),
  /* B6  */ ( BxOpcodeGroup_ERR ),
  /* B7  */ ( BxOpcodeGroup_ERR ),
  /* B8  */ ( BxOpcodeGroup_ERR ),
  /* B9  */ ( BxOpcodeGroup_ERR ),
  /* BA  */ ( BxOpcodeGroup_ERR ),
  /* BB  */ ( BxOpcodeGroup_ERR ),
  /* BC  */ ( BxOpcodeGroup_ERR ),
  /* BD  */ ( BxOpcodeGroup_ERR ),
  /* BE  */ ( BxOpcodeGroup_ERR ),
  /* BF  */ ( BxOpcodeGroup_ERR ),
  /* C0  */ ( BxOpcodeGroup_ERR ),
  /* C1  */ ( BxOpcodeGroup_ERR ),
  /* C2  */ ( BxOpcodeGroup_ERR ),
  /* C3  */ ( BxOpcodeGroup_ERR ),
  /* C4  */ ( BxOpcodeGroup_ERR ),
  /* C5  */ ( BxOpcodeGroup_ERR ),
  /* C6  */ ( BxOpcodeGroup_ERR ),
  /* C7  */ ( BxOpcodeGroup_ERR ),
  /* C8  */ ( BxOpcodeGroup_ERR ),
  /* C9  */ ( BxOpcodeGroup_ERR ),
  /* CA  */ ( BxOpcodeGroup_ERR ),
  /* CB  */ ( BxOpcodeGroup_ERR ),
  /* CC  */ ( BxOpcodeGroup_ERR ),
  /* CD  */ ( BxOpcodeGroup_ERR ),
  /* CE  */ ( BxOpcodeGroup_VEX_0F3ACE ),
  /* CF  */ ( BxOpcodeGroup_VEX_0F3ACF ),
  /* D0  */ ( BxOpcodeGroup_ERR ),
  /* D1  */ ( BxOpcodeGroup_ERR ),
  /* D2  */ ( BxOpcodeGroup_ERR ),
  /* D3  */ ( BxOpcodeGroup_ERR ),
  /* D4  */ ( BxOpcodeGroup_ERR ),
  /* D5  */ ( BxOpcodeGroup_ERR ),
  /* D6  */ ( BxOpcodeGroup_ERR ),
  /* D7  */ ( BxOpcodeGroup_ERR ),
  /* D8  */ ( BxOpcodeGroup_ERR ),
  /* D9  */ ( BxOpcodeGroup_ERR ),
  /* DA  */ ( BxOpcodeGroup_ERR ),
  /* DB  */ ( BxOpcodeGroup_ERR ),
  /* DC  */ ( BxOpcodeGroup_ERR ),
  /* DD  */ ( BxOpcodeGroup_ERR ),
  /* DE  */ ( BxOpcodeGroup_ERR ),
  /* DF  */ ( BxOpcodeGroup_VEX_0F3ADF ),
  /* E0  */ ( BxOpcodeGroup_ERR ),
  /* E1  */ ( BxOpcodeGroup_ERR ),
  /* E2  */ ( BxOpcodeGroup_ERR ),
  /* E3  */ ( BxOpcodeGroup_ERR ),
  /* E4  */ ( BxOpcodeGroup_ERR ),
  /* E5  */ ( BxOpcodeGroup_ERR ),
  /* E6  */ ( BxOpcodeGroup_ERR ),
  /* E7  */ ( BxOpcodeGroup_ERR ),
  /* E8  */ ( BxOpcodeGroup_ERR ),
  /* E9  */ ( BxOpcodeGroup_ERR ),
  /* EA  */ ( BxOpcodeGroup_ERR ),
  /* EB  */ ( BxOpcodeGroup_ERR ),
  /* EC  */ ( BxOpcodeGroup_ERR ),
  /* ED  */ ( BxOpcodeGroup_ERR ),
  /* EE  */ ( BxOpcodeGroup_ERR ),
  /* EF  */ ( BxOpcodeGroup_ERR ),
  /* F0  */ ( BxOpcodeGroup_VEX_0F3AF0 ),
  /* F1  */ ( BxOpcodeGroup_ERR ),
  /* F2  */ ( BxOpcodeGroup_ERR ),
  /* F3  */ ( BxOpcodeGroup_ERR ),
  /* F4  */ ( BxOpcodeGroup_ERR ),
  /* F5  */ ( BxOpcodeGroup_ERR ),
  /* F6  */ ( BxOpcodeGroup_ERR ),
  /* F7  */ ( BxOpcodeGroup_ERR ),
  /* F8  */ ( BxOpcodeGroup_ERR ),
  /* F9  */ ( BxOpcodeGroup_ERR ),
  /* FA  */ ( BxOpcodeGroup_ERR ),
  /* FB  */ ( BxOpcodeGroup_ERR ),
  /* FC  */ ( BxOpcodeGroup_ERR ),
  /* FD  */ ( BxOpcodeGroup_ERR ),
  /* FE  */ ( BxOpcodeGroup_ERR ),
  /* FF  */ ( BxOpcodeGroup_ERR )
};

#endif // BX_SUPPORT_AVX

#endif // BX_AVX_FETCHDECODE_TABLES_H
