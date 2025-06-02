/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2024 Stanislav Shwartsman
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

#if BX_SUPPORT_EVEX

#ifndef BX_STANDALONE_DECODER
#define BX_STANDALONE_DECODER
#endif

#include "fetchdecode.h"

// EVEX-encoded 0x0F opcodes
static const Bit64u BxOpcodeGroup_EVEX_0F10[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVUPS_VpsWps),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVUPD_VpdWpd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_VssHpsWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_VsdHpdWsd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_VssWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_VsdWsd),

  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVUPS_VpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVUPD_VpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_VsdHpdWsd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_VssWss_Kmask),
  last_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_VsdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F11[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVUPS_WpsVps),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVUPD_WpdVpd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_WssHpsVss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_WsdHpdVsd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_WssVss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_WsdVsd),

  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVUPS_WpsVps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVUPD_WpdVpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MODC0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_WssHpsVss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MODC0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_WsdHpdVsd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSS_WssVss_Kmask),
  last_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVSD_WsdVsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F12[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_EVEX_VMOVLPS_VpsHpsMq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_EVEX_VMOVHLPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_EVEX_VMOVLPD_VpdHpdMq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSLDUP_VpsWps),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDDUP_VpdWpd),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSLDUP_VpsWps_Kmask),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDDUP_VpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F13[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVLPS_MqVps),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVLPD_MqVsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F14[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUNPCKLPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUNPCKLPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VUNPCKLPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VUNPCKLPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F15[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUNPCKHPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUNPCKHPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VUNPCKHPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VUNPCKHPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F16[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_NO_PREFIX | ATTR_MOD_MEM, BX_IA_EVEX_VMOVHPS_VpsHpsMq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_NO_PREFIX | ATTR_MODC0, BX_IA_EVEX_VMOVLHPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_VL128 | ATTR_SSE_PREFIX_66 | ATTR_MOD_MEM, BX_IA_EVEX_VMOVHPD_VpdHpdMq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSHDUP_VpsWps),
  last_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVSHDUP_VpsWps_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F17[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_VL128 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVHPS_MqVps),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_VL128 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVHPD_MqVsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F28[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVAPS_VpsWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVAPS_VpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVAPD_VpdWpd),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVAPD_VpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F29[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVAPS_WpsVps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVAPS_WpsVps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVAPD_WpdVpd),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVAPD_WpdVpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F2A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSI2SS_VssEd),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSI2SS_VssEq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSI2SD_VsdEd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTSI2SD_VsdEq),
};

static const Bit64u BxOpcodeGroup_EVEX_0F2B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMOVNTPS_MpsVps),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVNTPD_MpdVpd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F2C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTSS2SI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSS2SI_GqWss),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTTSD2SI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTTSD2SI_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F2D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSS2SI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSS2SI_GqWss),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSD2SI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTSD2SI_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F2E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUCOMISS_VssWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VUCOMISD_VsdWsd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VUCOMXSD_VsdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VUCOMXSS_VssWss)
};

static const Bit64u BxOpcodeGroup_EVEX_0F2F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCOMISS_VssWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCOMISD_VsdWsd),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCOMXSD_VsdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCOMXSS_VssWss)
};

static const Bit64u BxOpcodeGroup_EVEX_0F51[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSQRTPS_VpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSQRTPS_VpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSQRTPD_VpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSQRTPD_VpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSQRTSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSQRTSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VSQRTSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VSQRTSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F54[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VANDPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VANDPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VANDPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VANDPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F55[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VANDNPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VANDNPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VANDNPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VANDNPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F56[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VORPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VORPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VORPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VORPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F57[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VXORPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VXORPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VXORPD_VpdHpdWpd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VXORPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F58[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VADDPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VADDPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VADDPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VADDPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VADDSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VADDSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VADDSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VADDSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F59[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMULPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMULPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMULPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMULPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMULSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMULSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMULSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMULSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPS2PD_VpdWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPS2PD_VpdWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2PS_VpsWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2PS_VpsWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSS2SD_VsdWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSS2SD_VsdWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSD2SS_VssWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSD2SS_VssWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTDQ2PS_VpsWdq),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTDQ2PS_VpsWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTQQ2PS_VpsWdq),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTQQ2PS_VpsWdq_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2DQ_VdqWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2DQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTPS2DQ_VdqWps),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTPS2DQ_VdqWps_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSUBPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSUBPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSUBPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSUBPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSUBSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSUBSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VSUBSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VSUBSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMINPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMINPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMINPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMINPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMINSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMINSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMINSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMINSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VDIVPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VDIVPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VDIVPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VDIVPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VDIVSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VDIVSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VDIVSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VDIVSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F5F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMAXPS_VpsHpsWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMAXPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMAXPD_VpdHpdWpd),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMAXPD_VpdHpdWpd_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMAXSS_VssHpsWss),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMAXSS_VssHpsWss_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMAXSD_VsdHpdWsd),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMAXSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F60[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKLBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKLBW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F61[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKLWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKLWD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F62[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKLDQ_VdqHdqWdq),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKLDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F63[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPACKSSWB_VdqHdqWdq),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPACKSSWB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F64[] = { last_opcode(ATTR_SSE_PREFIX_66,               BX_IA_EVEX_VPCMPGTB_KGqHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F65[] = { last_opcode(ATTR_SSE_PREFIX_66,               BX_IA_EVEX_VPCMPGTW_KGdHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F66[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPGTD_KGwHdqWdq) };

static const Bit64u BxOpcodeGroup_EVEX_0F67[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPACKUSWB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPACKUSWB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F68[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKHBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKHBW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F69[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKHWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKHWD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKHDQ_VdqHdqWdq),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKHDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPACKSSDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPACKSSDW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6C[] = {
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKLQDQ_VdqHdqWdq),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKLQDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6D[] = {
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPUNPCKHQDQ_VdqHdqWdq),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPUNPCKHQDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_EVEX_VMOVD_VdqEd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_IS64, BX_IA_EVEX_VMOVQ_VdqEq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F6F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA32_VdqWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA32_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA64_VdqWdq),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA64_VdqWdq_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU32_VdqWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU32_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU64_VdqWdq),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU64_VdqWdq_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU8_VdqWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU8_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU16_VdqWdq),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU16_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F70[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSHUFD_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSHUFD_VdqWdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MASK_K0, BX_IA_EVEX_VPSHUFHW_VdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_F3,                BX_IA_EVEX_VPSHUFHW_VdqWdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_MASK_K0, BX_IA_EVEX_VPSHUFLW_VdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_F2,                BX_IA_EVEX_VPSHUFLW_VdqWdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F71[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLW_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN2,                BX_IA_EVEX_VPSRLW_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAW_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN4,                BX_IA_EVEX_VPSRAW_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLW_UdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN6,                BX_IA_EVEX_VPSLLW_UdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F72[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN0 | ATTR_MASK_K0, BX_IA_EVEX_VPRORD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN0,                BX_IA_EVEX_VPRORD_UdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN0 | ATTR_MASK_K0, BX_IA_EVEX_VPRORQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN0,                BX_IA_EVEX_VPRORQ_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN1 | ATTR_MASK_K0, BX_IA_EVEX_VPROLD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN1,                BX_IA_EVEX_VPROLD_UdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN1 | ATTR_MASK_K0, BX_IA_EVEX_VPROLQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN1,                BX_IA_EVEX_VPROLQ_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN2 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN2,                BX_IA_EVEX_VPSRLD_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN4 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAD_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN4,                BX_IA_EVEX_VPSRAD_UdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN4 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN4,                BX_IA_EVEX_VPSRAQ_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN6 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLD_UdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_NNN6,                BX_IA_EVEX_VPSLLD_UdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F73[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN2 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN2,                BX_IA_EVEX_VPSRLQ_UdqIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN3 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLDQ_UdqIb),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN6 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLQ_UdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_NNN6,                BX_IA_EVEX_VPSLLQ_UdqIb_Kmask),

  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_NNN7 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLDQ_UdqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F74[] = { last_opcode(ATTR_SSE_PREFIX_66,               BX_IA_EVEX_VPCMPEQB_KGqHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F75[] = { last_opcode(ATTR_SSE_PREFIX_66,               BX_IA_EVEX_VPCMPEQW_KGdHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F76[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPEQD_KGwHdqWdq) };

static const Bit64u BxOpcodeGroup_EVEX_0F78[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2UDQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2UDQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2UDQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2UDQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2UQQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2UQQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2UQQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2UQQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTTSS2USI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSS2USI_GqWss),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2,             BX_IA_EVEX_VCVTTSD2USI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTTSD2USI_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F79[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPS2UDQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPS2UDQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPD2UDQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPD2UDQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2UQQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2UQQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2UQQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2UQQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTSS2USI_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSS2USI_GqWss),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2,             BX_IA_EVEX_VCVTSD2USI_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTSD2USI_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F7A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2QQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2QQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2QQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2QQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUDQ2PD_VpdWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUDQ2PD_VpdWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUQQ2PD_VpdWdq),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUQQ2PD_VpdWdq_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUDQ2PS_VpsWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUDQ2PS_VpsWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUQQ2PS_VpsWdq),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUQQ2PS_VpsWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F7B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2QQ_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2QQ_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2QQ_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2QQ_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUSI2SS_VssEd),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTUSI2SS_VssEq),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUSI2SD_VsdEd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTUSI2SD_VsdEq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F7E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_EVEX_VMOVD_EdVd),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_IS64, BX_IA_EVEX_VMOVQ_EqVq),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_VL128, BX_IA_EVEX_VMOVD_VdWd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_VL128, BX_IA_EVEX_VMOVQ_VqWq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F7F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA32_WdqVdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA32_WdqVdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA64_WdqVdq),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVDQA64_WdqVdq_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU32_WdqVdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU32_WdqVdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU64_WdqVdq),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMOVDQU64_WdqVdq_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU8_WdqVdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU8_WdqVdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU16_WdqVdq),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VMOVDQU16_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FC2[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCMPPS_KGwHpsWpsIb),
  form_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCMPPD_KGbHpdWpdIb),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCMPSS_KGbHssWssIb),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCMPSD_KGbHsdWsdIb),
};

static const Bit64u BxOpcodeGroup_EVEX_0FC4[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0,              BX_IA_EVEX_VPINSRW_VdqEwIb) };
static const Bit64u BxOpcodeGroup_EVEX_0FC5[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0 | ATTR_MODC0, BX_IA_EVEX_VPEXTRW_GdUdqIb) };

static const Bit64u BxOpcodeGroup_EVEX_0FC6[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VSHUFPS_VpsHpsWpsIb),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VSHUFPS_VpsHpsWpsIb_Kmask),

  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VSHUFPD_VpdHpdWpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VSHUFPD_VpdHpdWpdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD1[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSRLW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD2[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSRLD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD3[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRLQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD4[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPADDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPADDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD5[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMULLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMULLW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VMOVD_WdVd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VMOVQ_WqVq)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBUSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBUSB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FD9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBUSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBUSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMINUB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMINUB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPANDD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPANDD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPANDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPANDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDUSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDUSB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDUSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDUSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXUB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMAXUB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FDF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPANDND_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPANDND_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPANDNQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPANDNQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE0[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPAVGB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPAVGB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE1[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSRAW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE2[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSRAD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRAQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE3[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPAVGW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPAVGW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE4[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMULHUW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMULHUW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE5[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMULHW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMULHW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VCVTTPD2DQ_VdqWpd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VCVTTPD2DQ_VdqWpd_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VCVTDQ2PD_VpdWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VCVTDQ2PD_VpdWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VCVTQQ2PD_VpdWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W1,                BX_IA_EVEX_VCVTQQ2PD_VpdWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VCVTPD2DQ_VdqWpd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W1,                BX_IA_EVEX_VCVTPD2DQ_VdqWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE7[] = { last_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMOVNTDQ_MdqVdq) };

static const Bit64u BxOpcodeGroup_EVEX_0FE8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBSB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FE9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FEA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMINSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMINSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FEB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPORD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPORD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPORQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPORQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FEC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDSB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FED[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FEE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMAXSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FEF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPXORD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPXORD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPXORQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPXORQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF1[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSLLW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF2[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSLLD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF3[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSLLQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF4[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMULUDQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMULUDQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF5[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMADDWD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMADDWD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF6[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSADBW_VdqHdqWdq) };

static const Bit64u BxOpcodeGroup_EVEX_0FF8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FF9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSUBW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FFA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSUBD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FFB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSUBQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSUBQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FFC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FFD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPADDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPADDW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0FFE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPADDD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPADDD_VdqHdqWdq_Kmask)
};

// EVEX-encoded 0x0F 0x38 opcodes
static const Bit64u BxOpcodeGroup_EVEX_0F3800[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPSHUFB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPSHUFB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3804[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMADDUBSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMADDUBSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F380B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMULHRSW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMULHRSW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F380C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPERMILPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPERMILPS_VpsHpsWps_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F380D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPERMILPD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPERMILPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3810[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLVW_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRLVW_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSWB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSWB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3811[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAVW_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRAVW_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSDB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSDB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3812[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLVW_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSLLVW_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSQB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSQB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3813[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VCVTPH2PS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VCVTPH2PS_VpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSDW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSDW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3814[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPRORVD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPRORVD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPRORVQ_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPRORVQ_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSQW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSQW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3815[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPROLVD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPROLVD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPROLVQ_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPROLVQ_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVUSQD_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVUSQD_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3816[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256_512, BX_IA_EVEX_VPERMPS_VpsHpsWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256_512, BX_IA_EVEX_VPERMPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3818[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTSS_VpsWss),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VBROADCASTSS_VpsWss_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3819[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTF32x2_VpsWq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VBROADCASTF32x2_VpsWq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTSD_VpdWsd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VBROADCASTSD_VpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F381A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_VL256_512 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTF32x4_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_VL256_512,                BX_IA_EVEX_VBROADCASTF32x4_VpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_VL256_512 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTF64x2_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_VL256_512,                BX_IA_EVEX_VBROADCASTF64x2_VpdWpd_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_0F381B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_VL512 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTF32x8_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_VL512,                BX_IA_EVEX_VBROADCASTF32x8_VpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_VL512 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTF64x4_VpdWpd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_VL512,                BX_IA_EVEX_VBROADCASTF64x4_VpdWpd_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_0F381C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPABSB_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPABSB_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F381D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPABSW_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPABSW_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F381E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPABSD_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPABSD_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F381F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPABSQ_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPABSQ_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3820[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXBW_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVSXBW_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSWB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSWB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3821[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXBD_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVSXBD_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSDB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSDB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3822[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXBQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVSXBQ_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSQB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSQB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3823[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXWD_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVSXWD_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSDW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSDW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3824[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXWQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVSXWQ_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSQW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSQW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3825[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSXDQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSXDQ_VdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVSQD_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVSQD_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3826[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPTESTMB_KGqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPTESTMW_KGdHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VPTESTNMB_KGqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W1, BX_IA_EVEX_VPTESTNMW_KGdHdqWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3827[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPTESTMD_KGwHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPTESTMQ_KGbHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VPTESTNMD_KGwHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W1, BX_IA_EVEX_VPTESTNMQ_KGbHdqWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3828[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMULDQ_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMULDQ_VdqHdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVM2B_VdqKEq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVM2W_VdqKEd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3829[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPEQQ_KGbHdqWdq),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVB2M_KGqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVW2M_KGdWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F382A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_K0, BX_IA_EVEX_VMOVNTDQA_VdqMdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W1 | ATTR_MODC0   | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTMB2Q_VdqKEb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F382B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPACKUSDW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPACKUSDW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F382C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VSCALEFPS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VSCALEFPS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VSCALEFPD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VSCALEFPD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F382D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VSCALEFSS_VssHpsWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VSCALEFSS_VssHpsWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VSCALEFSD_VsdHpdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VSCALEFSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3830[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXBW_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVZXBW_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVWB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVWB_WdqVdq_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_0F3831[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXBD_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVZXBD_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVDB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVDB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3832[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXBQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVZXBQ_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVQB_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVQB_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3833[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXWD_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVZXWD_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVDW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVDW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3834[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXWQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMOVZXWQ_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVQW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVQW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3835[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVZXDQ_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVZXDQ_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMOVQD_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPMOVQD_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3836[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL256_512, BX_IA_EVEX_VPERMD_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256_512, BX_IA_EVEX_VPERMQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3837[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPGTQ_KGbHdqWdq) };

static const Bit64u BxOpcodeGroup_EVEX_0F3838[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMINSB_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMINSB_VdqHdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_VEX_W0, BX_IA_EVEX_VPMOVM2D_VdqKEw),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_VEX_W1, BX_IA_EVEX_VPMOVM2Q_VdqKEb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3839[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMINSD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMINSD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMINSQ_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMINSQ_VdqHdqWdq_Kmask),

  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_VEX_W0, BX_IA_EVEX_VPMOVD2M_KGwWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_VEX_W1, BX_IA_EVEX_VPMOVQ2M_KGbWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMINUW_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMINUW_VdqHdqWdq_Kmask),

  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_VEX_W0, BX_IA_EVEX_VPBROADCASTMW2D_VdqKEw)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMINUD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMINUD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMINUQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMINUQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXSB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMAXSB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXSD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMAXSD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXSQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMAXSQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXUW_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPMAXUW_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F383F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXUD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMAXUD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMAXUQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMAXUQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3840[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPMULLD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPMULLD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMULLQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMULLQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3842[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VGETEXPPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VGETEXPPS_VpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VGETEXPPD_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VGETEXPPD_VpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3843[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VGETEXPSS_VssHpsWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VGETEXPSS_VssHpsWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VGETEXPSD_VsdHpdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VGETEXPSD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3844[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPLZCNTD_VdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPLZCNTQ_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3845[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLVD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSRLVD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRLVQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRLVQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3846[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAVD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSRAVD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSRAVQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSRAVQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3847[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLVD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPSLLVD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPSLLVQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPSLLVQ_VdqHdqWdq_Kmask)
};

#if BX_SUPPORT_AMX
static const Bit64u BxOpcodeGroup_EVEX_0F384A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TILEMOVROW_VdqTrmBd),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWD2PS_VpsTrmBd) 
};
#endif

static const Bit64u BxOpcodeGroup_EVEX_0F384C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRCP14PS_VpsWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRCP14PD_VpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F384D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRCP14SS_VssHpsWss_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRCP14SD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F384E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRSQRT14PS_VpsWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRSQRT14PD_VpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F384F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRSQRT14SS_VssHpsWss_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRSQRT14SD_VsdHpdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3850[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBUUD_VdqHdqWdq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBUUD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBUSD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBUSD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBSUD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBSUD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBSSD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBSSD_VdqHdqWdq_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3851[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBUUDS_VdqHdqWdq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBUUDS_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBUSDS_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBUSDS_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBSUDS_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBSUDS_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPBSSDS_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPBSSDS_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3852[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VDPPHPS_VpsHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWSSD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWSSD_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VDPBF16PS_VpsHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3853[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWSSDS_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWSSDS_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3854[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPOPCNTB_VdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPOPCNTW_VdqWdq_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3855[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPOPCNTD_VdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPOPCNTQ_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3858[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTD_VdqWd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPBROADCASTD_VdqWd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3859[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTI32x2_VdqWq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VBROADCASTI32x2_VdqWq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTQ_VdqWq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPBROADCASTQ_VdqWq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F385A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_MOD_MEM | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTI32x4_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_MOD_MEM | ATTR_VEX_W0,                BX_IA_EVEX_VBROADCASTI32x4_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_MOD_MEM | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTI64x2_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_MOD_MEM | ATTR_VEX_W1,                BX_IA_EVEX_VBROADCASTI64x2_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F385B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_MOD_MEM | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTI32x8_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_MOD_MEM | ATTR_VEX_W0,                BX_IA_EVEX_VBROADCASTI32x8_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_MOD_MEM | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VBROADCASTI64x4_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_MOD_MEM | ATTR_VEX_W1,                BX_IA_EVEX_VBROADCASTI64x4_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3862[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPEXPANDB_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPEXPANDB_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPEXPANDW_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPEXPANDW_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3863[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPCOMPRESSB_WdqVdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPCOMPRESSB_WdqVdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPCOMPRESSW_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPCOMPRESSW_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3864[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPBLENDMD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPBLENDMQ_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3865[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VBLENDMPS_VpsHpsWps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VBLENDMPD_VpdHpdWpd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3866[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPBLENDMB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPBLENDMW_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3867[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VCVT2PS2PHX_VphHpsWps_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_0F3868[] = {
  form_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VP2INTERSECTD_KGqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VP2INTERSECTQ_KGqHdqWdq),
};

#if BX_SUPPORT_AMX
static const Bit64u BxOpcodeGroup_EVEX_0F386D[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2PHH_VphTrmBd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2PHL_VphTrmBd),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2BF16L_VphTrmBd),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2BF16H_VphTrmBd)
};
#endif

static const Bit64u BxOpcodeGroup_EVEX_0F3870[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHLDVW_VdqHdqWdq_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3871[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPSHLDVD_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHLDVQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3872[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHRDVW_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VCVTNEPS2BF16_VphWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VCVTNE2PS2BF16_VphHpsWps_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3873[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPSHRDVD_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHRDVQ_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3874[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VCVTBIASPH2BF8_Vf8HdqWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VCVTPH2BF8_Vf8HdqWph_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VCVT2PH2BF8_Vf8HdqWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3875[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMI2B_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMI2W_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3876[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMI2D_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMI2Q_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3877[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMI2PS_VpsHpsWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMI2PD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3878[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTB_VdqWb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPBROADCASTB_VdqWb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3879[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTW_VdqWw),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPBROADCASTW_VdqWw_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F387A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTB_VdqEb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0,                BX_IA_EVEX_VPBROADCASTB_VdqEb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F387B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0 | ATTR_MASK_K0, BX_IA_EVEX_VPBROADCASTW_VdqEw),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0,                BX_IA_EVEX_VPBROADCASTW_VdqEw_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F387C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0 | ATTR_MASK_K0,             BX_IA_EVEX_VPBROADCASTD_VdqEd),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MODC0,                            BX_IA_EVEX_VPBROADCASTD_VdqEd_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MODC0 | ATTR_MASK_K0 | ATTR_IS64, BX_IA_EVEX_VPBROADCASTQ_VdqEq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MODC0 |                ATTR_IS64, BX_IA_EVEX_VPBROADCASTQ_VdqEq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F387D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMT2B_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMT2W_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F387E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMT2D_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMT2Q_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F387F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMT2PS_VpsHpsWps_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMT2PD_VpdHpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3883[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMULTISHIFTQB_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMULTISHIFTQB_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3888[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXPANDPS_VpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VEXPANDPS_VpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VEXPANDPD_VpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VEXPANDPD_VpdWpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3889[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPEXPANDD_VdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPEXPANDD_VdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPEXPANDQ_VdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPEXPANDQ_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F388A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VCOMPRESSPS_WpsVps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VCOMPRESSPS_WpsVps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VCOMPRESSPD_WpdVpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VCOMPRESSPD_WpdVpd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F388B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPCOMPRESSD_WdqVdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPCOMPRESSD_WdqVdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPCOMPRESSQ_WdqVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPCOMPRESSQ_WdqVdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F388D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPERMB_VdqHdqWdq_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPERMW_VdqHdqWdq_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F388F[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPSHUFBITQMB_KGqHdqWdq_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_0F3890[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERDD_VdqVSib),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERDQ_VdqVSib)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3891[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERQD_VdqVSib),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERQQ_VdqVSib)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3892[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERDPS_VpsVSib),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERDPD_VpdVSib)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3893[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERQPS_VpsVSib),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VGATHERQPD_VpdVSib)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3896[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADDSUB132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3897[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUBADD132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3898[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3899[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD132SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD132SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB132SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB132SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389C[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD132SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD132SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB132PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB132PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F389F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB132SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB132SD_VpdHsdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38A0[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERDD_VSibVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERDQ_VSibVdq)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A1[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERQD_VSibVdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERQQ_VSibVdq)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A2[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERDPS_VSibVps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERDPD_VSibVpd)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A3[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERQPS_VSibVps),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_MASK_REQUIRED, BX_IA_EVEX_VSCATTERQPD_VSibVpd)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38A6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADDSUB213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUBADD213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38A9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD213SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD213SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB213SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB213SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD213SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD213SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB213PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB213PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38AF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB213SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB213SD_VpdHsdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38B4[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMADD52LUQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMADD52LUQ_VdqHdqWdq_Kmask),
};
static const Bit64u BxOpcodeGroup_EVEX_0F38B5[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPMADD52HUQ_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPMADD52HUQ_VdqHdqWdq_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_0F38B6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADDSUB231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38B7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUBADD231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38B8[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38B9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD231SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMADD231SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BA[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB231SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFMSUB231SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BC[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD231SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMADD231SD_VpdHsdWsd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BE[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231PS_VpsHpsWps),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB231PS_VpsHpsWps_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231PD_VpdHpdWpd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB231PD_VpdHpdWpd_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F38BF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231SS_VpsHssWss),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB231SS_VpsHssWss_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231SD_VpdHsdWsd),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFNMSUB231SD_VpdHsdWsd_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38C4[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VPCONFLICTD_VdqWdq_Kmask),
  last_opcode(ATTR_VEX_W1 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VPCONFLICTQ_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38CF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VGF2P8MULB_VdqHdqWdq_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_0F38D2[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWUUD_VdqHdqWdq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWUUD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWUSD_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWUSD_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWSUD_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWSUD_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38D3[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWUUDS_VdqHdqWdq),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWUUDS_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWUSDS_VdqHdqWdq),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWUSDS_VdqHdqWdq_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPDPWSUDS_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VPDPWSUDS_VdqHdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38DA[] = {
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VSM4KEY4_VdqHdqWdq),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VSM4RNDS4_VdqHdqWdq)
};

static const Bit64u BxOpcodeGroup_EVEX_0F38DC[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VAESENC_VdqHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F38DD[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VAESENCLAST_VdqHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F38DE[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VAESDEC_VdqHdqWdq) };
static const Bit64u BxOpcodeGroup_EVEX_0F38DF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VAESDECLAST_VdqHdqWdq) };

// EVEX-encoded 0x0F 0x3A opcodes
static const Bit64u BxOpcodeGroup_EVEX_0F3A00[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256_512, BX_IA_EVEX_VPERMQ_VdqWdqIb_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3A01[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_VL256_512, BX_IA_EVEX_VPERMPD_VpdWpdIb_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_0F3A03[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VALIGND_VdqHdqWdqIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VALIGNQ_VdqHdqWdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A04[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPERMILPS_VpsWpsIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPERMILPS_VpsWpsIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A05[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPERMILPD_VpdWpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPERMILPD_VpdWpdIb_Kmask)
};

#if BX_SUPPORT_AMX
static const Bit64u BxOpcodeGroup_EVEX_0F3A07[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TILEMOVROW_VdqTrmIb),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWD2PS_VpsTrmIb) 
};
#endif

static const Bit64u BxOpcodeGroup_EVEX_0F3A08[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VRNDSCALEPH_VphWphIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRNDSCALEPS_VpsWpsIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VRNDSCALEBF16_VphWphIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A09[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRNDSCALEPD_VpdWpdIb_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3A0A[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VRNDSCALESH_VshHphWshIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRNDSCALESS_VssHpsWssIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A0B[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRNDSCALESD_VsdHpdWsdIb_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_0F3A0F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPALIGNR_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66,                BX_IA_EVEX_VPALIGNR_VdqHdqWdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A14[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0 | ATTR_MODC0,   BX_IA_EVEX_VPEXTRB_EdVdqIbR),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0 | ATTR_MOD_MEM, BX_IA_EVEX_VPEXTRB_MbVdqIbM)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A15[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0 | ATTR_MODC0,   BX_IA_EVEX_VPEXTRW_EdVdqIbR),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_MASK_K0 | ATTR_MOD_MEM, BX_IA_EVEX_VPEXTRW_MwVdqIbM)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A16[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0 | ATTR_MASK_K0,             BX_IA_EVEX_VPEXTRD_EdVdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_IS64, BX_IA_EVEX_VPEXTRQ_EqVdqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A17[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTPS_EdVpsIb) };

static const Bit64u BxOpcodeGroup_EVEX_0F3A18[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTF32x4_VpsHpsWpsIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0,                BX_IA_EVEX_VINSERTF32x4_VpsHpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTF64x2_VpdHpdWpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1,                BX_IA_EVEX_VINSERTF64x2_VpdHpdWpdIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A19[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTF32x4_WpsVpsIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0,                BX_IA_EVEX_VEXTRACTF32x4_WpsVpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTF64x2_WpdVpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1,                BX_IA_EVEX_VEXTRACTF64x2_WpdVpdIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A1A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTF32x8_VpsHpsWpsIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0,                BX_IA_EVEX_VINSERTF32x8_VpsHpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTF64x4_VpdHpdWpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1,                BX_IA_EVEX_VINSERTF64x4_VpdHpdWpdIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A1B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTF32x8_WpsVpsIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0,                BX_IA_EVEX_VEXTRACTF32x8_WpsVpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTF64x4_WpdVpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1,                BX_IA_EVEX_VEXTRACTF64x4_WpdVpdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A1D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VCVTPS2PH_WpsVpsIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VCVTPS2PH_WpsVpsIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A1E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPUD_KGwHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPUQ_KGbHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A1F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPD_KGwHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPQ_KGbHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A20[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 |               ATTR_MASK_K0, BX_IA_EVEX_VPINSRB_VdqEbIb) };
static const Bit64u BxOpcodeGroup_EVEX_0F3A21[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTPS_VpsWssIb) };

static const Bit64u BxOpcodeGroup_EVEX_0F3A22[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W0 | ATTR_MASK_K0,             BX_IA_EVEX_VPINSRD_VdqEdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL128 | ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_IS64, BX_IA_EVEX_VPINSRQ_VdqEqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A23[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0, BX_IA_EVEX_VSHUFF32x4_VpsHpsWpsIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1, BX_IA_EVEX_VSHUFF64x2_VpdHpdWpdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A25[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VPTERNLOGD_VdqHdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VPTERNLOGD_VdqHdqWdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VPTERNLOGQ_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VPTERNLOGQ_VdqHdqWdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A26[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VGETMANTPH_VphWphIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VGETMANTPS_VpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VGETMANTPD_VpdWpdIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VGETMANTPBF16_VphWphIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A27[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VGETMANTSH_VshHphWshIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VGETMANTSS_VssHpsWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VGETMANTSD_VsdHpdWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A38[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTI32x4_VdqHdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0,                BX_IA_EVEX_VINSERTI32x4_VdqHdqWdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTI64x2_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1,                BX_IA_EVEX_VINSERTI64x2_VdqHdqWdqIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A39[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTI32x4_WdqVdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0,                BX_IA_EVEX_VEXTRACTI32x4_WdqVdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTI64x2_WdqVdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1,                BX_IA_EVEX_VEXTRACTI64x2_WdqVdqIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A3A[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTI32x8_VdqHdqWdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0,                BX_IA_EVEX_VINSERTI32x8_VdqHdqWdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VINSERTI64x4_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1,                BX_IA_EVEX_VINSERTI64x4_VdqHdqWdqIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A3B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTI32x8_WdqVdqIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W0,                BX_IA_EVEX_VEXTRACTI32x8_WdqVdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VEXTRACTI64x4_WdqVdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL512 | ATTR_VEX_W1,                BX_IA_EVEX_VEXTRACTI64x4_WdqVdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A3E[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPUB_KGqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPUW_KGdHdqWdqIb)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A3F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPCMPB_KGqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPCMPW_KGdHdqWdqIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A42[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VDBPSADBW_VdqHdqWdqIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VMPSADBW_VdqHdqWdqIb),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0,                BX_IA_EVEX_VMPSADBW_VdqHdqWdqIb_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A43[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W0, BX_IA_EVEX_VSHUFI32x4_VdqHdqWdqIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VL256_512 | ATTR_VEX_W1, BX_IA_EVEX_VSHUFI64x2_VdqHdqWdqIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A44[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_MASK_K0, BX_IA_EVEX_VPCLMULQDQ_VdqHdqWdqIb) };

static const Bit64u BxOpcodeGroup_EVEX_0F3A50[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRANGEPS_VpsHpsWpsIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRANGEPD_VpdHpdWpdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A51[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VRANGESS_VssHpsWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VRANGESD_VsdHpdWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A52[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VMINMAXPH_VphHphWphIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VMINMAXPS_VpsHpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VMINMAXPD_VpdHpdWpdIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VMINMAXBF16_VphHphWphIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A53[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VMINMAXSH_VshHphWshIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VMINMAXSS_VssHpsWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VMINMAXSD_VsdHpdWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A54[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFIXUPIMMPS_VpsHpsWpsIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFIXUPIMMPS_VpsHpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1 | ATTR_MASK_K0, BX_IA_EVEX_VFIXUPIMMPD_VpdHpdWpdIb),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1,                BX_IA_EVEX_VFIXUPIMMPD_VpdHpdWpdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A55[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VFIXUPIMMSS_VssHssWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VFIXUPIMMSD_VsdHsdWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A56[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VREDUCEPH_VphWphIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VREDUCEPS_VpsWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VREDUCEPD_VpdWpdIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VREDUCEBF16_VphWphIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A57[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VREDUCESH_VshHphWshIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VREDUCESS_VssHpsWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VREDUCESD_VsdHpdWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A66[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VFPCLASSPH_KGdWphIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VFPCLASSPS_KGwWpsIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VFPCLASSPD_KGbWpdIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VFPCLASSPBF16_KGdWphIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A67[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VFPCLASSSH_KGbWshIb_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VFPCLASSSS_KGbWssIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VFPCLASSSD_KGbWsdIb_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3A70[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHLDW_VdqHdqWdqIb_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3A71[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPSHLDD_VdqHdqWdqIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHLDQ_VdqHdqWdqIb_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_0F3A72[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHRDW_VdqHdqWdqIb_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3A73[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0, BX_IA_EVEX_VPSHRDD_VdqHdqWdqIb_Kmask),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VPSHRDQ_VdqHdqWdqIb_Kmask)
};

#if BX_SUPPORT_AMX
static const Bit64u BxOpcodeGroup_EVEX_0F3A77[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2PHH_VphTrmIb),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2PHL_VphTrmIb),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2BF16L_VphTrmIb),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0 | ATTR_VL512 | ATTR_MASK_K0 | ATTR_MODC0 | ATTR_IS64, BX_IA_EVEX_TCVTROWPS2BF16H_VphTrmIb)
};
#endif

static const Bit64u BxOpcodeGroup_EVEX_0F3AC2[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCMPPH_KGdHphWphIb),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCMPSH_KGbHshWshIb),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCMPPBF16_KGdHphWphIb)
};

static const Bit64u BxOpcodeGroup_EVEX_0F3ACE[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VGF2P8AFFINEQB_VdqHdqWdqIb_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_0F3ACF[] = { last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W1, BX_IA_EVEX_VGF2P8AFFINEINVQB_VdqHdqWdqIb_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_MAP5_10[] = {
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MODC0, BX_IA_EVEX_VMOVSH_VshHphWsh),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM, BX_IA_EVEX_VMOVSH_VshWsh),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 |                ATTR_MODC0, BX_IA_EVEX_VMOVSH_VshHphWsh_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 |                ATTR_MOD_MEM, BX_IA_EVEX_VMOVSH_VshWsh_Kmask),
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_11[] = {
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MODC0, BX_IA_EVEX_VMOVSH_WshHphVsh),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_MOD_MEM, BX_IA_EVEX_VMOVSH_WshVsh),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 |                ATTR_MODC0, BX_IA_EVEX_VMOVSH_WshHphVsh_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0 |                ATTR_MOD_MEM, BX_IA_EVEX_VMOVSH_WshVsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_18[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VCVTBIASPH2HF8_Vf8HdqWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VCVTPH2HF8_Vf8HdqWph_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VCVT2PH2HF8_Vf8HdqWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_1B[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VCVTBIASPH2HF8S_Vf8HdqWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VCVTPH2HF8S_Vf8HdqWph_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VCVT2PH2HF8S_Vf8HdqWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_1D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTSS2SH_VssWsh),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTSS2SH_VssWsh_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2PHX_VphWdq),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2PHX_VphWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_1E[] = { last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTHF82PH_VphWf8_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_MAP5_2A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSI2SH_VshEd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSI2SH_VshEq)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_2C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTSH2SI_GdWss),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSH2SI_GqWss)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_2D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSH2SI_GdWss),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSH2SI_GqWss)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_2E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VUCOMISH_VshWsh),
  last_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VUCOMXSH_VshWsh)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_2F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCOMISH_VshWsh),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCOMISBF16_VshWsh),
  last_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCOMXSH_VshWsh)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_51[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSQRTPH_VphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSQRTPH_VphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSQRTBF16_VphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSQRTBF16_VphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSQRTSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSQRTSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_58[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VADDPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VADDPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VADDBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VADDBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VADDSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VADDSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_59[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMULPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMULPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMULBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMULBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMULSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMULSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2PD_VpdWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2PD_VpdWph_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2PH_VphWdq),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPD2PH_VphWdq_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSH2SD_VsdWsh),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTSH2SD_VsdWsh_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSD2SH_VssWsh),
  last_opcode(ATTR_VEX_W1                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTSD2SH_VssWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTDQ2PH_VphWdq),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTDQ2PH_VphWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTQQ2PH_VphWdq),
  form_opcode(ATTR_VEX_W1                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTQQ2PH_VphWdq_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2DQ_VdqWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2DQ_VdqWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTPH2DQ_VdqWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTTPH2DQ_VdqWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSUBPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSUBPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSUBBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSUBBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSUBSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VSUBSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMINPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMINPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMINPBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMINPBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMINSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMINSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VDIVPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VDIVPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VDIVBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VDIVBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VDIVSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VDIVSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_5F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMAXPH_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VMAXPH_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMAXPBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VMAXPBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMAXSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VMAXSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_68[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2IBS_V8bWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2IBS_V8bWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2IBS_V8bWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2IBS_V8bWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTTBF162IBS_V8bWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTTBF162IBS_V8bWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_69[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2IBS_V8bWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2IBS_V8bWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2IBS_V8bWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2IBS_V8bWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTBF162IBS_V8bWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTBF162IBS_V8bWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2IUBS_V8bWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2IUBS_V8bWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2IUBS_V8bWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2IUBS_V8bWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTTBF162IUBS_V8bWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTTBF162IUBS_V8bWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2IUBS_V8bWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2IUBS_V8bWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2IUBS_V8bWps),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPS2IUBS_V8bWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTBF162IUBS_V8bWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTBF162IUBS_V8bWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2UDQS_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2UDQS_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2UDQS_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2UDQS_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2UQQS_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2UQQS_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2UQQS_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2UQQS_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTTSS2USIS_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSS2USIS_GqWss),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2,             BX_IA_EVEX_VCVTTSD2USIS_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTTSD2USIS_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2DQS_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPS2DQS_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2DQS_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPD2DQS_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2QQS_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPS2QQS_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2QQS_VdqWpd),
  form_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPD2QQS_VdqWpd_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTTSS2SIS_GdWss),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSS2SIS_GqWss),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2,             BX_IA_EVEX_VCVTTSD2SIS_GdWsd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2 | ATTR_IS64, BX_IA_EVEX_VCVTTSD2SIS_GqWsd)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6E[] = {
  form_opcode(ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_EVEX_VMOVW_VshEw),
  last_opcode(ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_EVEX_VMOVW_VshWsh)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_6F[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3,                BX_IA_EVEX_VMOVRSD_VdqWdq),
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3 | ATTR_MASK_K0, BX_IA_EVEX_VMOVRSD_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3,                BX_IA_EVEX_VMOVRSQ_VdqWdq),
  form_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F3 | ATTR_MASK_K0, BX_IA_EVEX_VMOVRSQ_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2,                BX_IA_EVEX_VMOVRSB_VdqWdq),
  form_opcode(ATTR_VEX_W0 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2 | ATTR_MASK_K0, BX_IA_EVEX_VMOVRSB_VdqWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2,                BX_IA_EVEX_VMOVRSW_VdqWdq),
  last_opcode(ATTR_VEX_W1 | ATTR_MOD_MEM | ATTR_SSE_PREFIX_F2 | ATTR_MASK_K0, BX_IA_EVEX_VMOVRSW_VdqWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_74[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0, BX_IA_EVEX_VCVTBIASPH2BF8S_Vf8HdqWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_F3 | ATTR_VEX_W0, BX_IA_EVEX_VCVTPH2BF8S_Vf8HdqWph_Kmask),
  last_opcode(ATTR_SSE_PREFIX_F2 | ATTR_VEX_W0, BX_IA_EVEX_VCVT2PH2BF8S_Vf8HdqWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_78[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2UDQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2UDQ_VdqWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2UQQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2UQQ_VdqWph_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTTSH2USI_GdWss),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTTSH2USI_GqWss)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_79[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2UDQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2UDQ_VdqWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2UQQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2UQQ_VdqWph_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3,             BX_IA_EVEX_VCVTSH2USI_GdWss),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTSH2USI_GqWss)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_7A[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2QQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2QQ_VdqWph_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUDQ2PH_VphWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUDQ2PH_VphWdq_Kmask),
  form_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUQQ2PH_VphWdq),
  last_opcode(ATTR_VEX_W1 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUQQ2PH_VphWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_7B[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2QQ_VdqWph),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2QQ_VdqWph_Kmask),

  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTUSI2SH_VshEd),
  last_opcode(ATTR_VEX_W1 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_IS64, BX_IA_EVEX_VCVTUSI2SH_VshEq)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_7C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2UW_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTTPH2UW_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2W_VdqWps),
  last_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTTPH2W_VdqWps_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_7D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2UW_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTPH2UW_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2W_VdqWps),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2W_VdqWps_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTW2PH_VphWdq),
  form_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VCVTW2PH_VphWdq_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUW2PH_VphWdq),
  last_opcode(ATTR_VEX_W0 |                ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VCVTUW2PH_VphWdq_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP5_7E[] = {
  form_opcode(ATTR_MASK_K0 | ATTR_SSE_PREFIX_66 | ATTR_VL128, BX_IA_EVEX_VMOVW_EdVsh),
  last_opcode(ATTR_MASK_K0 | ATTR_SSE_PREFIX_F3 | ATTR_VL128 | ATTR_VEX_W0, BX_IA_EVEX_VMOVW_WshVsh)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_13[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTSH2SS_VssWsh),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VCVTSH2SS_VssWsh_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2PSX_VpsWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VCVTPH2PSX_VpsWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_2C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSCALEFPBF16_VphHphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VSCALEFPBF16_VphHphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSCALEFPH_VphHphWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSCALEFPH_VphHphWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_2D[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSCALEFSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VSCALEFSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_42[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VGETEXPPBF16_VphWph),
  form_opcode(ATTR_VEX_W0                | ATTR_SSE_NO_PREFIX, BX_IA_EVEX_VGETEXPPBF16_VphWph_Kmask),
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VGETEXPPH_VphWph),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VGETEXPPH_VphWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_43[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_MASK_K0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VGETEXPSH_VshHphWsh),
  last_opcode(ATTR_VEX_W0                | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VGETEXPSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_4C[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX | ATTR_MASK_K0, BX_IA_EVEX_VRCPPBF16_VphWph),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX,                BX_IA_EVEX_VRCPPBF16_VphWph_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VRCPPH_VphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_4D[] = { last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VRCPSH_VshHphWsh_Kmask) };
static const Bit64u BxOpcodeGroup_EVEX_MAP6_4E[] = {
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX | ATTR_MASK_K0, BX_IA_EVEX_VRSQRTPBF16_VphWph),
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_NO_PREFIX,                BX_IA_EVEX_VRSQRTPBF16_VphWph_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VRSQRTPH_VphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_4F[] = { last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_66, BX_IA_EVEX_VRSQRTSH_VshHphWsh_Kmask) };

static const Bit64u BxOpcodeGroup_EVEX_MAP6_56[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VFMADDCPH_VphHphWph_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VFCMADDCPH_VphHphWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_57[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VFMADDCSH_VshHphWsh_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VFCMADDCSH_VshHphWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_96[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_97[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_98[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD132BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_99[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD132SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD132SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9A[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB132BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9B[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB132SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB132SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9C[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD132BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9D[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD132SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD132SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9E[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB132BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB132PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_9F[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB132SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB132SH_VphHshWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_A6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_A7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_A8[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD213BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_A9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD213SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD213SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AA[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB213BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB213SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB213SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AC[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD213BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD213SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD213SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AE[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB213BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB213PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_AF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB213SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB213SH_VphHshWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_B6[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADDSUB231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADDSUB231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_B7[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUBADD231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUBADD231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_B8[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD231BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_B9[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMADD231SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMADD231SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BA[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB231BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BB[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFMSUB231SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFMSUB231SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BC[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD231BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BD[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMADD231SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMADD231SH_VphHshWsh_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BE[] = {
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231BF16_VphHphWph),
  form_opcode(ATTR_SSE_NO_PREFIX | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB231BF16_VphHphWph_Kmask),
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231PH_VphHphWph),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB231PH_VphHphWph_Kmask)
};
static const Bit64u BxOpcodeGroup_EVEX_MAP6_BF[] = {
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0 | ATTR_MASK_K0, BX_IA_EVEX_VFNMSUB231SH_VphHshWsh),
  last_opcode(ATTR_SSE_PREFIX_66 | ATTR_VEX_W0,                BX_IA_EVEX_VFNMSUB231SH_VphHshWsh_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_D6[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VFMULCPH_VphHphWph_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VFCMULCPH_VphHphWph_Kmask)
};

static const Bit64u BxOpcodeGroup_EVEX_MAP6_D7[] = { 
  form_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F3, BX_IA_EVEX_VFMULCSH_VshHphWsh_Kmask),
  last_opcode(ATTR_VEX_W0 | ATTR_SSE_PREFIX_F2, BX_IA_EVEX_VFCMULCSH_VshHphWsh_Kmask)
};

/* ************************************************************************ */

const Bit64u *BxOpcodeTableEVEX[256*5] = {
  // 256 entries for EVEX-encoded 0x0F opcodes
  /* 00 */ BxOpcodeGroup_ERR,
  /* 01 */ BxOpcodeGroup_ERR,
  /* 02 */ BxOpcodeGroup_ERR,
  /* 03 */ BxOpcodeGroup_ERR,
  /* 04 */ BxOpcodeGroup_ERR,
  /* 05 */ BxOpcodeGroup_ERR,
  /* 06 */ BxOpcodeGroup_ERR,
  /* 07 */ BxOpcodeGroup_ERR,
  /* 08 */ BxOpcodeGroup_ERR,
  /* 09 */ BxOpcodeGroup_ERR,
  /* 0A */ BxOpcodeGroup_ERR,
  /* 0B */ BxOpcodeGroup_ERR,
  /* 0C */ BxOpcodeGroup_ERR,
  /* 0D */ BxOpcodeGroup_ERR,
  /* 0E */ BxOpcodeGroup_ERR,
  /* 0F */ BxOpcodeGroup_ERR,
  /* 10 */ BxOpcodeGroup_EVEX_0F10,
  /* 11 */ BxOpcodeGroup_EVEX_0F11,
  /* 12 */ BxOpcodeGroup_EVEX_0F12,
  /* 13 */ BxOpcodeGroup_EVEX_0F13,
  /* 14 */ BxOpcodeGroup_EVEX_0F14,
  /* 15 */ BxOpcodeGroup_EVEX_0F15,
  /* 16 */ BxOpcodeGroup_EVEX_0F16,
  /* 17 */ BxOpcodeGroup_EVEX_0F17,
  /* 18 */ BxOpcodeGroup_ERR,
  /* 19 */ BxOpcodeGroup_ERR,
  /* 1A */ BxOpcodeGroup_ERR,
  /* 1B */ BxOpcodeGroup_ERR,
  /* 1C */ BxOpcodeGroup_ERR,
  /* 1D */ BxOpcodeGroup_ERR,
  /* 1E */ BxOpcodeGroup_ERR,
  /* 1F */ BxOpcodeGroup_ERR,
  /* 20 */ BxOpcodeGroup_ERR,
  /* 21 */ BxOpcodeGroup_ERR,
  /* 22 */ BxOpcodeGroup_ERR,
  /* 23 */ BxOpcodeGroup_ERR,
  /* 24 */ BxOpcodeGroup_ERR,
  /* 25 */ BxOpcodeGroup_ERR,
  /* 26 */ BxOpcodeGroup_ERR,
  /* 27 */ BxOpcodeGroup_ERR,
  /* 28 */ BxOpcodeGroup_EVEX_0F28,
  /* 29 */ BxOpcodeGroup_EVEX_0F29,
  /* 2A */ BxOpcodeGroup_EVEX_0F2A,
  /* 2B */ BxOpcodeGroup_EVEX_0F2B,
  /* 2C */ BxOpcodeGroup_EVEX_0F2C,
  /* 2D */ BxOpcodeGroup_EVEX_0F2D,
  /* 2E */ BxOpcodeGroup_EVEX_0F2E,
  /* 2F */ BxOpcodeGroup_EVEX_0F2F,
  /* 30 */ BxOpcodeGroup_ERR,
  /* 31 */ BxOpcodeGroup_ERR,
  /* 32 */ BxOpcodeGroup_ERR,
  /* 33 */ BxOpcodeGroup_ERR,
  /* 34 */ BxOpcodeGroup_ERR,
  /* 35 */ BxOpcodeGroup_ERR,
  /* 36 */ BxOpcodeGroup_ERR,
  /* 37 */ BxOpcodeGroup_ERR,
  /* 38 */ BxOpcodeGroup_ERR, // 3-Byte Escape
  /* 39 */ BxOpcodeGroup_ERR,
  /* 3A */ BxOpcodeGroup_ERR, // 3-Byte Escape
  /* 3B */ BxOpcodeGroup_ERR,
  /* 3C */ BxOpcodeGroup_ERR,
  /* 3D */ BxOpcodeGroup_ERR,
  /* 3E */ BxOpcodeGroup_ERR,
  /* 3F */ BxOpcodeGroup_ERR,
  /* 40 */ BxOpcodeGroup_ERR,
  /* 41 */ BxOpcodeGroup_ERR,
  /* 42 */ BxOpcodeGroup_ERR,
  /* 43 */ BxOpcodeGroup_ERR,
  /* 44 */ BxOpcodeGroup_ERR,
  /* 45 */ BxOpcodeGroup_ERR,
  /* 46 */ BxOpcodeGroup_ERR,
  /* 47 */ BxOpcodeGroup_ERR,
  /* 48 */ BxOpcodeGroup_ERR,
  /* 49 */ BxOpcodeGroup_ERR,
  /* 4A */ BxOpcodeGroup_ERR,
  /* 4B */ BxOpcodeGroup_ERR,
  /* 4C */ BxOpcodeGroup_ERR,
  /* 4D */ BxOpcodeGroup_ERR,
  /* 4E */ BxOpcodeGroup_ERR,
  /* 4F */ BxOpcodeGroup_ERR,
  /* 50 */ BxOpcodeGroup_ERR,
  /* 51 */ BxOpcodeGroup_EVEX_0F51,
  /* 52 */ BxOpcodeGroup_ERR,
  /* 53 */ BxOpcodeGroup_ERR,
  /* 54 */ BxOpcodeGroup_EVEX_0F54,
  /* 55 */ BxOpcodeGroup_EVEX_0F55,
  /* 56 */ BxOpcodeGroup_EVEX_0F56,
  /* 57 */ BxOpcodeGroup_EVEX_0F57,
  /* 58 */ BxOpcodeGroup_EVEX_0F58,
  /* 59 */ BxOpcodeGroup_EVEX_0F59,
  /* 5A */ BxOpcodeGroup_EVEX_0F5A,
  /* 5B */ BxOpcodeGroup_EVEX_0F5B,
  /* 5C */ BxOpcodeGroup_EVEX_0F5C,
  /* 5D */ BxOpcodeGroup_EVEX_0F5D,
  /* 5E */ BxOpcodeGroup_EVEX_0F5E,
  /* 5F */ BxOpcodeGroup_EVEX_0F5F,
  /* 60 */ BxOpcodeGroup_EVEX_0F60,
  /* 61 */ BxOpcodeGroup_EVEX_0F61,
  /* 62 */ BxOpcodeGroup_EVEX_0F62,
  /* 63 */ BxOpcodeGroup_EVEX_0F63,
  /* 64 */ BxOpcodeGroup_EVEX_0F64,
  /* 65 */ BxOpcodeGroup_EVEX_0F65,
  /* 66 */ BxOpcodeGroup_EVEX_0F66,
  /* 67 */ BxOpcodeGroup_EVEX_0F67,
  /* 68 */ BxOpcodeGroup_EVEX_0F68,
  /* 69 */ BxOpcodeGroup_EVEX_0F69,
  /* 6A */ BxOpcodeGroup_EVEX_0F6A,
  /* 6B */ BxOpcodeGroup_EVEX_0F6B,
  /* 6C */ BxOpcodeGroup_EVEX_0F6C,
  /* 6D */ BxOpcodeGroup_EVEX_0F6D,
  /* 6E */ BxOpcodeGroup_EVEX_0F6E,
  /* 6F */ BxOpcodeGroup_EVEX_0F6F,
  /* 70 */ BxOpcodeGroup_EVEX_0F70,
  /* 71 */ BxOpcodeGroup_EVEX_0F71,
  /* 72 */ BxOpcodeGroup_EVEX_0F72,
  /* 73 */ BxOpcodeGroup_EVEX_0F73,
  /* 74 */ BxOpcodeGroup_EVEX_0F74,
  /* 75 */ BxOpcodeGroup_EVEX_0F75,
  /* 76 */ BxOpcodeGroup_EVEX_0F76,
  /* 77 */ BxOpcodeGroup_ERR,
  /* 78 */ BxOpcodeGroup_EVEX_0F78,
  /* 79 */ BxOpcodeGroup_EVEX_0F79,
  /* 7A */ BxOpcodeGroup_EVEX_0F7A,
  /* 7B */ BxOpcodeGroup_EVEX_0F7B,
  /* 7C */ BxOpcodeGroup_ERR,
  /* 7D */ BxOpcodeGroup_ERR,
  /* 7E */ BxOpcodeGroup_EVEX_0F7E,
  /* 7F */ BxOpcodeGroup_EVEX_0F7F,
  /* 80 */ BxOpcodeGroup_ERR,
  /* 81 */ BxOpcodeGroup_ERR,
  /* 82 */ BxOpcodeGroup_ERR,
  /* 83 */ BxOpcodeGroup_ERR,
  /* 84 */ BxOpcodeGroup_ERR,
  /* 85 */ BxOpcodeGroup_ERR,
  /* 86 */ BxOpcodeGroup_ERR,
  /* 87 */ BxOpcodeGroup_ERR,
  /* 88 */ BxOpcodeGroup_ERR,
  /* 89 */ BxOpcodeGroup_ERR,
  /* 8A */ BxOpcodeGroup_ERR,
  /* 8B */ BxOpcodeGroup_ERR,
  /* 8C */ BxOpcodeGroup_ERR,
  /* 8D */ BxOpcodeGroup_ERR,
  /* 8E */ BxOpcodeGroup_ERR,
  /* 8F */ BxOpcodeGroup_ERR,
  /* 90 */ BxOpcodeGroup_ERR,
  /* 91 */ BxOpcodeGroup_ERR,
  /* 92 */ BxOpcodeGroup_ERR,
  /* 93 */ BxOpcodeGroup_ERR,
  /* 94 */ BxOpcodeGroup_ERR,
  /* 95 */ BxOpcodeGroup_ERR,
  /* 96 */ BxOpcodeGroup_ERR,
  /* 97 */ BxOpcodeGroup_ERR,
  /* 98 */ BxOpcodeGroup_ERR,
  /* 99 */ BxOpcodeGroup_ERR,
  /* 9A */ BxOpcodeGroup_ERR,
  /* 9B */ BxOpcodeGroup_ERR,
  /* 9C */ BxOpcodeGroup_ERR,
  /* 9D */ BxOpcodeGroup_ERR,
  /* 9E */ BxOpcodeGroup_ERR,
  /* 9F */ BxOpcodeGroup_ERR,
  /* A0 */ BxOpcodeGroup_ERR,
  /* A1 */ BxOpcodeGroup_ERR,
  /* A2 */ BxOpcodeGroup_ERR,
  /* A3 */ BxOpcodeGroup_ERR,
  /* A4 */ BxOpcodeGroup_ERR,
  /* A5 */ BxOpcodeGroup_ERR,
  /* A6 */ BxOpcodeGroup_ERR,
  /* A7 */ BxOpcodeGroup_ERR,
  /* A8 */ BxOpcodeGroup_ERR,
  /* A9 */ BxOpcodeGroup_ERR,
  /* AA */ BxOpcodeGroup_ERR,
  /* AB */ BxOpcodeGroup_ERR,
  /* AC */ BxOpcodeGroup_ERR,
  /* AD */ BxOpcodeGroup_ERR,
  /* AE */ BxOpcodeGroup_ERR,
  /* AF */ BxOpcodeGroup_ERR,
  /* B0 */ BxOpcodeGroup_ERR,
  /* B1 */ BxOpcodeGroup_ERR,
  /* B2 */ BxOpcodeGroup_ERR,
  /* B3 */ BxOpcodeGroup_ERR,
  /* B4 */ BxOpcodeGroup_ERR,
  /* B5 */ BxOpcodeGroup_ERR,
  /* B6 */ BxOpcodeGroup_ERR,
  /* B7 */ BxOpcodeGroup_ERR,
  /* B8 */ BxOpcodeGroup_ERR,
  /* B9 */ BxOpcodeGroup_ERR,
  /* BA */ BxOpcodeGroup_ERR,
  /* BB */ BxOpcodeGroup_ERR,
  /* BC */ BxOpcodeGroup_ERR,
  /* BD */ BxOpcodeGroup_ERR,
  /* BE */ BxOpcodeGroup_ERR,
  /* BF */ BxOpcodeGroup_ERR,
  /* C0 */ BxOpcodeGroup_ERR,
  /* C1 */ BxOpcodeGroup_ERR,
  /* C2 */ BxOpcodeGroup_EVEX_0FC2,
  /* C3 */ BxOpcodeGroup_ERR,
  /* C4 */ BxOpcodeGroup_EVEX_0FC4,
  /* C5 */ BxOpcodeGroup_EVEX_0FC5,
  /* C6 */ BxOpcodeGroup_EVEX_0FC6,
  /* C7 */ BxOpcodeGroup_ERR,
  /* C8 */ BxOpcodeGroup_ERR,
  /* C9 */ BxOpcodeGroup_ERR,
  /* CA */ BxOpcodeGroup_ERR,
  /* CB */ BxOpcodeGroup_ERR,
  /* CC */ BxOpcodeGroup_ERR,
  /* CD */ BxOpcodeGroup_ERR,
  /* CE */ BxOpcodeGroup_ERR,
  /* CF */ BxOpcodeGroup_ERR,
  /* D0 */ BxOpcodeGroup_ERR,
  /* D1 */ BxOpcodeGroup_EVEX_0FD1,
  /* D2 */ BxOpcodeGroup_EVEX_0FD2,
  /* D3 */ BxOpcodeGroup_EVEX_0FD3,
  /* D4 */ BxOpcodeGroup_EVEX_0FD4,
  /* D5 */ BxOpcodeGroup_EVEX_0FD5,
  /* D6 */ BxOpcodeGroup_EVEX_0FD6,
  /* D7 */ BxOpcodeGroup_ERR,
  /* D8 */ BxOpcodeGroup_EVEX_0FD8,
  /* D9 */ BxOpcodeGroup_EVEX_0FD9,
  /* DA */ BxOpcodeGroup_EVEX_0FDA,
  /* DB */ BxOpcodeGroup_EVEX_0FDB,
  /* DC */ BxOpcodeGroup_EVEX_0FDC,
  /* DD */ BxOpcodeGroup_EVEX_0FDD,
  /* DE */ BxOpcodeGroup_EVEX_0FDE,
  /* DF */ BxOpcodeGroup_EVEX_0FDF,
  /* E0 */ BxOpcodeGroup_EVEX_0FE0,
  /* E1 */ BxOpcodeGroup_EVEX_0FE1,
  /* E2 */ BxOpcodeGroup_EVEX_0FE2,
  /* E3 */ BxOpcodeGroup_EVEX_0FE3,
  /* E4 */ BxOpcodeGroup_EVEX_0FE4,
  /* E5 */ BxOpcodeGroup_EVEX_0FE5,
  /* E6 */ BxOpcodeGroup_EVEX_0FE6,
  /* E7 */ BxOpcodeGroup_EVEX_0FE7,
  /* E8 */ BxOpcodeGroup_EVEX_0FE8,
  /* E9 */ BxOpcodeGroup_EVEX_0FE9,
  /* EA */ BxOpcodeGroup_EVEX_0FEA,
  /* EB */ BxOpcodeGroup_EVEX_0FEB,
  /* EC */ BxOpcodeGroup_EVEX_0FEC,
  /* ED */ BxOpcodeGroup_EVEX_0FED,
  /* EE */ BxOpcodeGroup_EVEX_0FEE,
  /* EF */ BxOpcodeGroup_EVEX_0FEF,
  /* F0 */ BxOpcodeGroup_ERR,
  /* F1 */ BxOpcodeGroup_EVEX_0FF1,
  /* F2 */ BxOpcodeGroup_EVEX_0FF2,
  /* F3 */ BxOpcodeGroup_EVEX_0FF3,
  /* F4 */ BxOpcodeGroup_EVEX_0FF4,
  /* F5 */ BxOpcodeGroup_EVEX_0FF5,
  /* F6 */ BxOpcodeGroup_EVEX_0FF6,
  /* F7 */ BxOpcodeGroup_ERR,
  /* F8 */ BxOpcodeGroup_EVEX_0FF8,
  /* F9 */ BxOpcodeGroup_EVEX_0FF9,
  /* FA */ BxOpcodeGroup_EVEX_0FFA,
  /* FB */ BxOpcodeGroup_EVEX_0FFB,
  /* FC */ BxOpcodeGroup_EVEX_0FFC,
  /* FD */ BxOpcodeGroup_EVEX_0FFD,
  /* FE */ BxOpcodeGroup_EVEX_0FFE,
  /* FF */ BxOpcodeGroup_ERR,

  // 256 entries for EVEX-encoded 0x0F 0x38 opcodes
  /* 00 */ BxOpcodeGroup_EVEX_0F3800,
  /* 01 */ BxOpcodeGroup_ERR,
  /* 02 */ BxOpcodeGroup_ERR,
  /* 03 */ BxOpcodeGroup_ERR,
  /* 04 */ BxOpcodeGroup_EVEX_0F3804,
  /* 05 */ BxOpcodeGroup_ERR,
  /* 06 */ BxOpcodeGroup_ERR,
  /* 07 */ BxOpcodeGroup_ERR,
  /* 08 */ BxOpcodeGroup_ERR,
  /* 09 */ BxOpcodeGroup_ERR,
  /* 0A */ BxOpcodeGroup_ERR,
  /* 0B */ BxOpcodeGroup_EVEX_0F380B,
  /* 0C */ BxOpcodeGroup_EVEX_0F380C,
  /* 0D */ BxOpcodeGroup_EVEX_0F380D,
  /* 0E */ BxOpcodeGroup_ERR,
  /* 0F */ BxOpcodeGroup_ERR,
  /* 10 */ BxOpcodeGroup_EVEX_0F3810,
  /* 11 */ BxOpcodeGroup_EVEX_0F3811,
  /* 12 */ BxOpcodeGroup_EVEX_0F3812,
  /* 13 */ BxOpcodeGroup_EVEX_0F3813,
  /* 14 */ BxOpcodeGroup_EVEX_0F3814,
  /* 15 */ BxOpcodeGroup_EVEX_0F3815,
  /* 16 */ BxOpcodeGroup_EVEX_0F3816,
  /* 17 */ BxOpcodeGroup_ERR,
  /* 18 */ BxOpcodeGroup_EVEX_0F3818,
  /* 19 */ BxOpcodeGroup_EVEX_0F3819,
  /* 1A */ BxOpcodeGroup_EVEX_0F381A,
  /* 1B */ BxOpcodeGroup_EVEX_0F381B,
  /* 1C */ BxOpcodeGroup_EVEX_0F381C,
  /* 1D */ BxOpcodeGroup_EVEX_0F381D,
  /* 1E */ BxOpcodeGroup_EVEX_0F381E,
  /* 1F */ BxOpcodeGroup_EVEX_0F381F,
  /* 20 */ BxOpcodeGroup_EVEX_0F3820,
  /* 21 */ BxOpcodeGroup_EVEX_0F3821,
  /* 22 */ BxOpcodeGroup_EVEX_0F3822,
  /* 23 */ BxOpcodeGroup_EVEX_0F3823,
  /* 24 */ BxOpcodeGroup_EVEX_0F3824,
  /* 25 */ BxOpcodeGroup_EVEX_0F3825,
  /* 26 */ BxOpcodeGroup_EVEX_0F3826,
  /* 27 */ BxOpcodeGroup_EVEX_0F3827,
  /* 28 */ BxOpcodeGroup_EVEX_0F3828,
  /* 29 */ BxOpcodeGroup_EVEX_0F3829,
  /* 2A */ BxOpcodeGroup_EVEX_0F382A,
  /* 2B */ BxOpcodeGroup_EVEX_0F382B,
  /* 2C */ BxOpcodeGroup_EVEX_0F382C,
  /* 2D */ BxOpcodeGroup_EVEX_0F382D,
  /* 2E */ BxOpcodeGroup_ERR,
  /* 2F */ BxOpcodeGroup_ERR,
  /* 30 */ BxOpcodeGroup_EVEX_0F3830,
  /* 31 */ BxOpcodeGroup_EVEX_0F3831,
  /* 32 */ BxOpcodeGroup_EVEX_0F3832,
  /* 33 */ BxOpcodeGroup_EVEX_0F3833,
  /* 34 */ BxOpcodeGroup_EVEX_0F3834,
  /* 35 */ BxOpcodeGroup_EVEX_0F3835,
  /* 36 */ BxOpcodeGroup_EVEX_0F3836,
  /* 37 */ BxOpcodeGroup_EVEX_0F3837,
  /* 38 */ BxOpcodeGroup_EVEX_0F3838,
  /* 39 */ BxOpcodeGroup_EVEX_0F3839,
  /* 3A */ BxOpcodeGroup_EVEX_0F383A,
  /* 3B */ BxOpcodeGroup_EVEX_0F383B,
  /* 3C */ BxOpcodeGroup_EVEX_0F383C,
  /* 3D */ BxOpcodeGroup_EVEX_0F383D,
  /* 3E */ BxOpcodeGroup_EVEX_0F383E,
  /* 3F */ BxOpcodeGroup_EVEX_0F383F,
  /* 40 */ BxOpcodeGroup_EVEX_0F3840,
  /* 41 */ BxOpcodeGroup_ERR,
  /* 42 */ BxOpcodeGroup_EVEX_0F3842,
  /* 43 */ BxOpcodeGroup_EVEX_0F3843,
  /* 44 */ BxOpcodeGroup_EVEX_0F3844,
  /* 45 */ BxOpcodeGroup_EVEX_0F3845,
  /* 46 */ BxOpcodeGroup_EVEX_0F3846,
  /* 47 */ BxOpcodeGroup_EVEX_0F3847,
  /* 48 */ BxOpcodeGroup_ERR,
  /* 49 */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_AMX
  /* 4A */ BxOpcodeGroup_EVEX_0F384A,
#else
  /* 4A */ BxOpcodeGroup_ERR,
#endif
  /* 4B */ BxOpcodeGroup_ERR,
  /* 4C */ BxOpcodeGroup_EVEX_0F384C,
  /* 4D */ BxOpcodeGroup_EVEX_0F384D,
  /* 4E */ BxOpcodeGroup_EVEX_0F384E,
  /* 4F */ BxOpcodeGroup_EVEX_0F384F,
  /* 50 */ BxOpcodeGroup_EVEX_0F3850,
  /* 51 */ BxOpcodeGroup_EVEX_0F3851,
  /* 52 */ BxOpcodeGroup_EVEX_0F3852,
  /* 53 */ BxOpcodeGroup_EVEX_0F3853,
  /* 54 */ BxOpcodeGroup_EVEX_0F3854,
  /* 55 */ BxOpcodeGroup_EVEX_0F3855,
  /* 56 */ BxOpcodeGroup_ERR,
  /* 57 */ BxOpcodeGroup_ERR,
  /* 58 */ BxOpcodeGroup_EVEX_0F3858,
  /* 59 */ BxOpcodeGroup_EVEX_0F3859,
  /* 5A */ BxOpcodeGroup_EVEX_0F385A,
  /* 5B */ BxOpcodeGroup_EVEX_0F385B,
  /* 5C */ BxOpcodeGroup_ERR,
  /* 5D */ BxOpcodeGroup_ERR,
  /* 5E */ BxOpcodeGroup_ERR,
  /* 5F */ BxOpcodeGroup_ERR,
  /* 60 */ BxOpcodeGroup_ERR,
  /* 61 */ BxOpcodeGroup_ERR,
  /* 62 */ BxOpcodeGroup_EVEX_0F3862,
  /* 63 */ BxOpcodeGroup_EVEX_0F3863,
  /* 64 */ BxOpcodeGroup_EVEX_0F3864,
  /* 65 */ BxOpcodeGroup_EVEX_0F3865,
  /* 66 */ BxOpcodeGroup_EVEX_0F3866,
  /* 67 */ BxOpcodeGroup_EVEX_0F3867,
  /* 66 */ BxOpcodeGroup_EVEX_0F3868,
  /* 69 */ BxOpcodeGroup_ERR,
  /* 6A */ BxOpcodeGroup_ERR,
  /* 6B */ BxOpcodeGroup_ERR,
  /* 6C */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_AMX
  /* 6D */ BxOpcodeGroup_EVEX_0F386D,
#else
  /* 6D */ BxOpcodeGroup_ERR,
#endif
  /* 6E */ BxOpcodeGroup_ERR,
  /* 6F */ BxOpcodeGroup_ERR,
  /* 70 */ BxOpcodeGroup_EVEX_0F3870,
  /* 71 */ BxOpcodeGroup_EVEX_0F3871,
  /* 72 */ BxOpcodeGroup_EVEX_0F3872,
  /* 73 */ BxOpcodeGroup_EVEX_0F3873,
  /* 74 */ BxOpcodeGroup_EVEX_0F3874,
  /* 75 */ BxOpcodeGroup_EVEX_0F3875,
  /* 76 */ BxOpcodeGroup_EVEX_0F3876,
  /* 77 */ BxOpcodeGroup_EVEX_0F3877,
  /* 78 */ BxOpcodeGroup_EVEX_0F3878,
  /* 79 */ BxOpcodeGroup_EVEX_0F3879,
  /* 7A */ BxOpcodeGroup_EVEX_0F387A,
  /* 7B */ BxOpcodeGroup_EVEX_0F387B,
  /* 7C */ BxOpcodeGroup_EVEX_0F387C,
  /* 7D */ BxOpcodeGroup_EVEX_0F387D,
  /* 7E */ BxOpcodeGroup_EVEX_0F387E,
  /* 7F */ BxOpcodeGroup_EVEX_0F387F,
  /* 80 */ BxOpcodeGroup_ERR,
  /* 81 */ BxOpcodeGroup_ERR,
  /* 82 */ BxOpcodeGroup_ERR,
  /* 83 */ BxOpcodeGroup_EVEX_0F3883,
  /* 84 */ BxOpcodeGroup_ERR,
  /* 85 */ BxOpcodeGroup_ERR,
  /* 86 */ BxOpcodeGroup_ERR,
  /* 87 */ BxOpcodeGroup_ERR,
  /* 88 */ BxOpcodeGroup_EVEX_0F3888,
  /* 89 */ BxOpcodeGroup_EVEX_0F3889,
  /* 8A */ BxOpcodeGroup_EVEX_0F388A,
  /* 8B */ BxOpcodeGroup_EVEX_0F388B,
  /* 8C */ BxOpcodeGroup_ERR,
  /* 8D */ BxOpcodeGroup_EVEX_0F388D,
  /* 8E */ BxOpcodeGroup_ERR,
  /* 8F */ BxOpcodeGroup_EVEX_0F388F,
  /* 90 */ BxOpcodeGroup_EVEX_0F3890,
  /* 91 */ BxOpcodeGroup_EVEX_0F3891,
  /* 92 */ BxOpcodeGroup_EVEX_0F3892,
  /* 93 */ BxOpcodeGroup_EVEX_0F3893,
  /* 94 */ BxOpcodeGroup_ERR,
  /* 95 */ BxOpcodeGroup_ERR,
  /* 96 */ BxOpcodeGroup_EVEX_0F3896,
  /* 97 */ BxOpcodeGroup_EVEX_0F3897,
  /* 98 */ BxOpcodeGroup_EVEX_0F3898,
  /* 99 */ BxOpcodeGroup_EVEX_0F3899,
  /* 9A */ BxOpcodeGroup_EVEX_0F389A,
  /* 9B */ BxOpcodeGroup_EVEX_0F389B,
  /* 9C */ BxOpcodeGroup_EVEX_0F389C,
  /* 9D */ BxOpcodeGroup_EVEX_0F389D,
  /* 9E */ BxOpcodeGroup_EVEX_0F389E,
  /* 9F */ BxOpcodeGroup_EVEX_0F389F,
  /* A0 */ BxOpcodeGroup_EVEX_0F38A0,
  /* A1 */ BxOpcodeGroup_EVEX_0F38A1,
  /* A2 */ BxOpcodeGroup_EVEX_0F38A2,
  /* A3 */ BxOpcodeGroup_EVEX_0F38A3,
  /* A4 */ BxOpcodeGroup_ERR,
  /* A5 */ BxOpcodeGroup_ERR,
  /* A6 */ BxOpcodeGroup_EVEX_0F38A6,
  /* A7 */ BxOpcodeGroup_EVEX_0F38A7,
  /* A8 */ BxOpcodeGroup_EVEX_0F38A8,
  /* A9 */ BxOpcodeGroup_EVEX_0F38A9,
  /* AA */ BxOpcodeGroup_EVEX_0F38AA,
  /* AB */ BxOpcodeGroup_EVEX_0F38AB,
  /* AC */ BxOpcodeGroup_EVEX_0F38AC,
  /* AD */ BxOpcodeGroup_EVEX_0F38AD,
  /* AE */ BxOpcodeGroup_EVEX_0F38AE,
  /* AF */ BxOpcodeGroup_EVEX_0F38AF,
  /* B0 */ BxOpcodeGroup_ERR,
  /* B1 */ BxOpcodeGroup_ERR,
  /* B2 */ BxOpcodeGroup_ERR,
  /* B3 */ BxOpcodeGroup_ERR,
  /* B4 */ BxOpcodeGroup_EVEX_0F38B4,
  /* B5 */ BxOpcodeGroup_EVEX_0F38B5,
  /* B6 */ BxOpcodeGroup_EVEX_0F38B6,
  /* B7 */ BxOpcodeGroup_EVEX_0F38B7,
  /* B8 */ BxOpcodeGroup_EVEX_0F38B8,
  /* B9 */ BxOpcodeGroup_EVEX_0F38B9,
  /* BA */ BxOpcodeGroup_EVEX_0F38BA,
  /* BB */ BxOpcodeGroup_EVEX_0F38BB,
  /* BC */ BxOpcodeGroup_EVEX_0F38BC,
  /* BD */ BxOpcodeGroup_EVEX_0F38BD,
  /* BE */ BxOpcodeGroup_EVEX_0F38BE,
  /* BF */ BxOpcodeGroup_EVEX_0F38BF,
  /* C0 */ BxOpcodeGroup_ERR,
  /* C1 */ BxOpcodeGroup_ERR,
  /* C2 */ BxOpcodeGroup_ERR,
  /* C3 */ BxOpcodeGroup_ERR,
  /* C4 */ BxOpcodeGroup_EVEX_0F38C4,
  /* C5 */ BxOpcodeGroup_ERR,
  /* C6 */ BxOpcodeGroup_ERR,
  /* C7 */ BxOpcodeGroup_ERR,
  /* C8 */ BxOpcodeGroup_ERR,
  /* C9 */ BxOpcodeGroup_ERR,
  /* CA */ BxOpcodeGroup_ERR,
  /* CB */ BxOpcodeGroup_ERR,
  /* CC */ BxOpcodeGroup_ERR,
  /* CD */ BxOpcodeGroup_ERR,
  /* CE */ BxOpcodeGroup_ERR,
  /* CF */ BxOpcodeGroup_EVEX_0F38CF,
  /* D0 */ BxOpcodeGroup_ERR,
  /* D1 */ BxOpcodeGroup_ERR,
  /* D2 */ BxOpcodeGroup_ERR,
  /* D3 */ BxOpcodeGroup_ERR,
  /* D4 */ BxOpcodeGroup_ERR,
  /* D5 */ BxOpcodeGroup_ERR,
  /* D6 */ BxOpcodeGroup_ERR,
  /* D7 */ BxOpcodeGroup_ERR,
  /* D8 */ BxOpcodeGroup_ERR,
  /* D9 */ BxOpcodeGroup_ERR,
  /* DA */ BxOpcodeGroup_ERR,
  /* DB */ BxOpcodeGroup_ERR,
  /* DC */ BxOpcodeGroup_EVEX_0F38DC,
  /* DD */ BxOpcodeGroup_EVEX_0F38DD,
  /* DF */ BxOpcodeGroup_EVEX_0F38DE,
  /* DE */ BxOpcodeGroup_EVEX_0F38DF,
  /* E0 */ BxOpcodeGroup_ERR,
  /* E1 */ BxOpcodeGroup_ERR,
  /* E2 */ BxOpcodeGroup_ERR,
  /* E3 */ BxOpcodeGroup_ERR,
  /* E4 */ BxOpcodeGroup_ERR,
  /* E5 */ BxOpcodeGroup_ERR,
  /* E6 */ BxOpcodeGroup_ERR,
  /* E7 */ BxOpcodeGroup_ERR,
  /* E8 */ BxOpcodeGroup_ERR,
  /* E9 */ BxOpcodeGroup_ERR,
  /* EA */ BxOpcodeGroup_ERR,
  /* EB */ BxOpcodeGroup_ERR,
  /* EC */ BxOpcodeGroup_ERR,
  /* ED */ BxOpcodeGroup_ERR,
  /* EE */ BxOpcodeGroup_ERR,
  /* EF */ BxOpcodeGroup_ERR,
  /* F0 */ BxOpcodeGroup_ERR,
  /* F1 */ BxOpcodeGroup_ERR,
  /* F2 */ BxOpcodeGroup_ERR,
  /* F3 */ BxOpcodeGroup_ERR,
  /* F4 */ BxOpcodeGroup_ERR,
  /* F5 */ BxOpcodeGroup_ERR,
  /* F6 */ BxOpcodeGroup_ERR,
  /* F7 */ BxOpcodeGroup_ERR,
  /* F8 */ BxOpcodeGroup_ERR,
  /* F9 */ BxOpcodeGroup_ERR,
  /* FA */ BxOpcodeGroup_ERR,
  /* FB */ BxOpcodeGroup_ERR,
  /* FC */ BxOpcodeGroup_ERR,
  /* FD */ BxOpcodeGroup_ERR,
  /* FE */ BxOpcodeGroup_ERR,
  /* FF */ BxOpcodeGroup_ERR,

  // 256 entries for EVEX-encoded 0x0F 0x3A opcodes
  /* 00 */ BxOpcodeGroup_EVEX_0F3A00,
  /* 01 */ BxOpcodeGroup_EVEX_0F3A01,
  /* 02 */ BxOpcodeGroup_ERR,
  /* 03 */ BxOpcodeGroup_EVEX_0F3A03,
  /* 04 */ BxOpcodeGroup_EVEX_0F3A04,
  /* 05 */ BxOpcodeGroup_EVEX_0F3A05,
  /* 06 */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_AMX
  /* 07 */ BxOpcodeGroup_EVEX_0F3A07,
#else
  /* 07 */ BxOpcodeGroup_ERR,
#endif
  /* 08 */ BxOpcodeGroup_EVEX_0F3A08,
  /* 09 */ BxOpcodeGroup_EVEX_0F3A09,
  /* 0A */ BxOpcodeGroup_EVEX_0F3A0A,
  /* 0B */ BxOpcodeGroup_EVEX_0F3A0B,
  /* 0C */ BxOpcodeGroup_ERR,
  /* 0D */ BxOpcodeGroup_ERR,
  /* 0E */ BxOpcodeGroup_ERR,
  /* 0F */ BxOpcodeGroup_EVEX_0F3A0F,
  /* 10 */ BxOpcodeGroup_ERR,
  /* 11 */ BxOpcodeGroup_ERR,
  /* 12 */ BxOpcodeGroup_ERR,
  /* 13 */ BxOpcodeGroup_ERR,
  /* 14 */ BxOpcodeGroup_EVEX_0F3A14,
  /* 15 */ BxOpcodeGroup_EVEX_0F3A15,
  /* 16 */ BxOpcodeGroup_EVEX_0F3A16,
  /* 17 */ BxOpcodeGroup_EVEX_0F3A17,
  /* 18 */ BxOpcodeGroup_EVEX_0F3A18,
  /* 19 */ BxOpcodeGroup_EVEX_0F3A19,
  /* 1A */ BxOpcodeGroup_EVEX_0F3A1A,
  /* 1B */ BxOpcodeGroup_EVEX_0F3A1B,
  /* 1C */ BxOpcodeGroup_ERR,
  /* 1D */ BxOpcodeGroup_EVEX_0F3A1D,
  /* 1E */ BxOpcodeGroup_EVEX_0F3A1E,
  /* 1F */ BxOpcodeGroup_EVEX_0F3A1F,
  /* 20 */ BxOpcodeGroup_EVEX_0F3A20,
  /* 21 */ BxOpcodeGroup_EVEX_0F3A21,
  /* 22 */ BxOpcodeGroup_EVEX_0F3A22,
  /* 23 */ BxOpcodeGroup_EVEX_0F3A23,
  /* 24 */ BxOpcodeGroup_ERR,
  /* 25 */ BxOpcodeGroup_EVEX_0F3A25,
  /* 26 */ BxOpcodeGroup_EVEX_0F3A26,
  /* 27 */ BxOpcodeGroup_EVEX_0F3A27,
  /* 28 */ BxOpcodeGroup_ERR,
  /* 29 */ BxOpcodeGroup_ERR,
  /* 2A */ BxOpcodeGroup_ERR,
  /* 2B */ BxOpcodeGroup_ERR,
  /* 2C */ BxOpcodeGroup_ERR,
  /* 2D */ BxOpcodeGroup_ERR,
  /* 2E */ BxOpcodeGroup_ERR,
  /* 2F */ BxOpcodeGroup_ERR,
  /* 30 */ BxOpcodeGroup_ERR,
  /* 31 */ BxOpcodeGroup_ERR,
  /* 32 */ BxOpcodeGroup_ERR,
  /* 33 */ BxOpcodeGroup_ERR,
  /* 34 */ BxOpcodeGroup_ERR,
  /* 35 */ BxOpcodeGroup_ERR,
  /* 36 */ BxOpcodeGroup_ERR,
  /* 37 */ BxOpcodeGroup_ERR,
  /* 38 */ BxOpcodeGroup_EVEX_0F3A38,
  /* 39 */ BxOpcodeGroup_EVEX_0F3A39,
  /* 3A */ BxOpcodeGroup_EVEX_0F3A3A,
  /* 3B */ BxOpcodeGroup_EVEX_0F3A3B,
  /* 3C */ BxOpcodeGroup_ERR,
  /* 3D */ BxOpcodeGroup_ERR,
  /* 3E */ BxOpcodeGroup_EVEX_0F3A3E,
  /* 3F */ BxOpcodeGroup_EVEX_0F3A3F,
  /* 40 */ BxOpcodeGroup_ERR,
  /* 41 */ BxOpcodeGroup_ERR,
  /* 42 */ BxOpcodeGroup_EVEX_0F3A42,
  /* 43 */ BxOpcodeGroup_EVEX_0F3A43,
  /* 44 */ BxOpcodeGroup_EVEX_0F3A44,
  /* 45 */ BxOpcodeGroup_ERR,
  /* 46 */ BxOpcodeGroup_ERR,
  /* 47 */ BxOpcodeGroup_ERR,
  /* 48 */ BxOpcodeGroup_ERR,
  /* 49 */ BxOpcodeGroup_ERR,
  /* 4A */ BxOpcodeGroup_ERR,
  /* 4B */ BxOpcodeGroup_ERR,
  /* 4C */ BxOpcodeGroup_ERR,
  /* 4D */ BxOpcodeGroup_ERR,
  /* 4E */ BxOpcodeGroup_ERR,
  /* 4F */ BxOpcodeGroup_ERR,
  /* 50 */ BxOpcodeGroup_EVEX_0F3A50,
  /* 51 */ BxOpcodeGroup_EVEX_0F3A51,
  /* 52 */ BxOpcodeGroup_EVEX_0F3A52,
  /* 53 */ BxOpcodeGroup_EVEX_0F3A53,
  /* 54 */ BxOpcodeGroup_EVEX_0F3A54,
  /* 55 */ BxOpcodeGroup_EVEX_0F3A55,
  /* 56 */ BxOpcodeGroup_EVEX_0F3A56,
  /* 57 */ BxOpcodeGroup_EVEX_0F3A57,
  /* 58 */ BxOpcodeGroup_ERR,
  /* 59 */ BxOpcodeGroup_ERR,
  /* 5A */ BxOpcodeGroup_ERR,
  /* 5B */ BxOpcodeGroup_ERR,
  /* 5C */ BxOpcodeGroup_ERR,
  /* 5D */ BxOpcodeGroup_ERR,
  /* 5E */ BxOpcodeGroup_ERR,
  /* 5F */ BxOpcodeGroup_ERR,
  /* 60 */ BxOpcodeGroup_ERR,
  /* 61 */ BxOpcodeGroup_ERR,
  /* 62 */ BxOpcodeGroup_ERR,
  /* 63 */ BxOpcodeGroup_ERR,
  /* 64 */ BxOpcodeGroup_ERR,
  /* 65 */ BxOpcodeGroup_ERR,
  /* 66 */ BxOpcodeGroup_EVEX_0F3A66,
  /* 67 */ BxOpcodeGroup_EVEX_0F3A67,
  /* 68 */ BxOpcodeGroup_ERR,
  /* 69 */ BxOpcodeGroup_ERR,
  /* 6A */ BxOpcodeGroup_ERR,
  /* 6B */ BxOpcodeGroup_ERR,
  /* 6C */ BxOpcodeGroup_ERR,
  /* 6D */ BxOpcodeGroup_ERR,
  /* 6E */ BxOpcodeGroup_ERR,
  /* 6F */ BxOpcodeGroup_ERR,
  /* 70 */ BxOpcodeGroup_EVEX_0F3A70,
  /* 71 */ BxOpcodeGroup_EVEX_0F3A71,
  /* 72 */ BxOpcodeGroup_EVEX_0F3A72,
  /* 73 */ BxOpcodeGroup_EVEX_0F3A73,
  /* 74 */ BxOpcodeGroup_ERR,
  /* 75 */ BxOpcodeGroup_ERR,
  /* 76 */ BxOpcodeGroup_ERR,
#if BX_SUPPORT_AMX
  /* 77 */ BxOpcodeGroup_EVEX_0F3A77,
#else
  /* 77 */ BxOpcodeGroup_ERR,
#endif
  /* 78 */ BxOpcodeGroup_ERR,
  /* 79 */ BxOpcodeGroup_ERR,
  /* 7A */ BxOpcodeGroup_ERR,
  /* 7B */ BxOpcodeGroup_ERR,
  /* 7C */ BxOpcodeGroup_ERR,
  /* 7D */ BxOpcodeGroup_ERR,
  /* 7E */ BxOpcodeGroup_ERR,
  /* 7F */ BxOpcodeGroup_ERR,
  /* 80 */ BxOpcodeGroup_ERR,
  /* 81 */ BxOpcodeGroup_ERR,
  /* 82 */ BxOpcodeGroup_ERR,
  /* 83 */ BxOpcodeGroup_ERR,
  /* 84 */ BxOpcodeGroup_ERR,
  /* 85 */ BxOpcodeGroup_ERR,
  /* 86 */ BxOpcodeGroup_ERR,
  /* 87 */ BxOpcodeGroup_ERR,
  /* 88 */ BxOpcodeGroup_ERR,
  /* 89 */ BxOpcodeGroup_ERR,
  /* 8A */ BxOpcodeGroup_ERR,
  /* 8B */ BxOpcodeGroup_ERR,
  /* 8C */ BxOpcodeGroup_ERR,
  /* 8D */ BxOpcodeGroup_ERR,
  /* 8E */ BxOpcodeGroup_ERR,
  /* 8F */ BxOpcodeGroup_ERR,
  /* 90 */ BxOpcodeGroup_ERR,
  /* 91 */ BxOpcodeGroup_ERR,
  /* 92 */ BxOpcodeGroup_ERR,
  /* 93 */ BxOpcodeGroup_ERR,
  /* 94 */ BxOpcodeGroup_ERR,
  /* 95 */ BxOpcodeGroup_ERR,
  /* 96 */ BxOpcodeGroup_ERR,
  /* 97 */ BxOpcodeGroup_ERR,
  /* 98 */ BxOpcodeGroup_ERR,
  /* 99 */ BxOpcodeGroup_ERR,
  /* 9A */ BxOpcodeGroup_ERR,
  /* 9B */ BxOpcodeGroup_ERR,
  /* 9C */ BxOpcodeGroup_ERR,
  /* 9D */ BxOpcodeGroup_ERR,
  /* 9E */ BxOpcodeGroup_ERR,
  /* 9F */ BxOpcodeGroup_ERR,
  /* A0 */ BxOpcodeGroup_ERR,
  /* A1 */ BxOpcodeGroup_ERR,
  /* A2 */ BxOpcodeGroup_ERR,
  /* A3 */ BxOpcodeGroup_ERR,
  /* A4 */ BxOpcodeGroup_ERR,
  /* A5 */ BxOpcodeGroup_ERR,
  /* A6 */ BxOpcodeGroup_ERR,
  /* A7 */ BxOpcodeGroup_ERR,
  /* A8 */ BxOpcodeGroup_ERR,
  /* A9 */ BxOpcodeGroup_ERR,
  /* AA */ BxOpcodeGroup_ERR,
  /* AB */ BxOpcodeGroup_ERR,
  /* AC */ BxOpcodeGroup_ERR,
  /* AD */ BxOpcodeGroup_ERR,
  /* AE */ BxOpcodeGroup_ERR,
  /* AF */ BxOpcodeGroup_ERR,
  /* B0 */ BxOpcodeGroup_ERR,
  /* B1 */ BxOpcodeGroup_ERR,
  /* B2 */ BxOpcodeGroup_ERR,
  /* B3 */ BxOpcodeGroup_ERR,
  /* B4 */ BxOpcodeGroup_ERR,
  /* B5 */ BxOpcodeGroup_ERR,
  /* B6 */ BxOpcodeGroup_ERR,
  /* B7 */ BxOpcodeGroup_ERR,
  /* B8 */ BxOpcodeGroup_ERR,
  /* B9 */ BxOpcodeGroup_ERR,
  /* BA */ BxOpcodeGroup_ERR,
  /* BB */ BxOpcodeGroup_ERR,
  /* BC */ BxOpcodeGroup_ERR,
  /* BD */ BxOpcodeGroup_ERR,
  /* BE */ BxOpcodeGroup_ERR,
  /* BF */ BxOpcodeGroup_ERR,
  /* C0 */ BxOpcodeGroup_ERR,
  /* C1 */ BxOpcodeGroup_ERR,
  /* C2 */ BxOpcodeGroup_EVEX_0F3AC2,
  /* C3 */ BxOpcodeGroup_ERR,
  /* C4 */ BxOpcodeGroup_ERR,
  /* C5 */ BxOpcodeGroup_ERR,
  /* C6 */ BxOpcodeGroup_ERR,
  /* C7 */ BxOpcodeGroup_ERR,
  /* C8 */ BxOpcodeGroup_ERR,
  /* C9 */ BxOpcodeGroup_ERR,
  /* CA */ BxOpcodeGroup_ERR,
  /* CB */ BxOpcodeGroup_ERR,
  /* CC */ BxOpcodeGroup_ERR,
  /* CD */ BxOpcodeGroup_ERR,
  /* CE */ BxOpcodeGroup_EVEX_0F3ACE,
  /* CF */ BxOpcodeGroup_EVEX_0F3ACF,
  /* D0 */ BxOpcodeGroup_ERR,
  /* D1 */ BxOpcodeGroup_ERR,
  /* D2 */ BxOpcodeGroup_ERR,
  /* D3 */ BxOpcodeGroup_ERR,
  /* D4 */ BxOpcodeGroup_ERR,
  /* D5 */ BxOpcodeGroup_ERR,
  /* D6 */ BxOpcodeGroup_ERR,
  /* D7 */ BxOpcodeGroup_ERR,
  /* D8 */ BxOpcodeGroup_ERR,
  /* D9 */ BxOpcodeGroup_ERR,
  /* DA */ BxOpcodeGroup_ERR,
  /* DB */ BxOpcodeGroup_ERR,
  /* DC */ BxOpcodeGroup_ERR,
  /* DD */ BxOpcodeGroup_ERR,
  /* DE */ BxOpcodeGroup_ERR,
  /* DF */ BxOpcodeGroup_ERR,
  /* E0 */ BxOpcodeGroup_ERR,
  /* E1 */ BxOpcodeGroup_ERR,
  /* E2 */ BxOpcodeGroup_ERR,
  /* E3 */ BxOpcodeGroup_ERR,
  /* E4 */ BxOpcodeGroup_ERR,
  /* E5 */ BxOpcodeGroup_ERR,
  /* E6 */ BxOpcodeGroup_ERR,
  /* E7 */ BxOpcodeGroup_ERR,
  /* E8 */ BxOpcodeGroup_ERR,
  /* E9 */ BxOpcodeGroup_ERR,
  /* EA */ BxOpcodeGroup_ERR,
  /* EB */ BxOpcodeGroup_ERR,
  /* EC */ BxOpcodeGroup_ERR,
  /* ED */ BxOpcodeGroup_ERR,
  /* EE */ BxOpcodeGroup_ERR,
  /* EF */ BxOpcodeGroup_ERR,
  /* F0 */ BxOpcodeGroup_ERR,
  /* F1 */ BxOpcodeGroup_ERR,
  /* F2 */ BxOpcodeGroup_ERR,
  /* F3 */ BxOpcodeGroup_ERR,
  /* F4 */ BxOpcodeGroup_ERR,
  /* F5 */ BxOpcodeGroup_ERR,
  /* F6 */ BxOpcodeGroup_ERR,
  /* F7 */ BxOpcodeGroup_ERR,
  /* F8 */ BxOpcodeGroup_ERR,
  /* F9 */ BxOpcodeGroup_ERR,
  /* FA */ BxOpcodeGroup_ERR,
  /* FB */ BxOpcodeGroup_ERR,
  /* FC */ BxOpcodeGroup_ERR,
  /* FD */ BxOpcodeGroup_ERR,
  /* FE */ BxOpcodeGroup_ERR,
  /* FF */ BxOpcodeGroup_ERR,

  // 256 entries for EVEX-encoded map5 opcodes
  /* 00 */ BxOpcodeGroup_ERR,
  /* 01 */ BxOpcodeGroup_ERR,
  /* 02 */ BxOpcodeGroup_ERR,
  /* 03 */ BxOpcodeGroup_ERR,
  /* 04 */ BxOpcodeGroup_ERR,
  /* 05 */ BxOpcodeGroup_ERR,
  /* 06 */ BxOpcodeGroup_ERR,
  /* 07 */ BxOpcodeGroup_ERR,
  /* 08 */ BxOpcodeGroup_ERR,
  /* 09 */ BxOpcodeGroup_ERR,
  /* 0A */ BxOpcodeGroup_ERR,
  /* 0B */ BxOpcodeGroup_ERR,
  /* 0C */ BxOpcodeGroup_ERR,
  /* 0D */ BxOpcodeGroup_ERR,
  /* 0E */ BxOpcodeGroup_ERR,
  /* 0F */ BxOpcodeGroup_ERR,
  /* 10 */ BxOpcodeGroup_EVEX_MAP5_10,
  /* 11 */ BxOpcodeGroup_EVEX_MAP5_11,
  /* 12 */ BxOpcodeGroup_ERR,
  /* 13 */ BxOpcodeGroup_ERR,
  /* 14 */ BxOpcodeGroup_ERR,
  /* 15 */ BxOpcodeGroup_ERR,
  /* 16 */ BxOpcodeGroup_ERR,
  /* 17 */ BxOpcodeGroup_ERR,
  /* 18 */ BxOpcodeGroup_EVEX_MAP5_18,
  /* 19 */ BxOpcodeGroup_ERR,
  /* 1A */ BxOpcodeGroup_ERR,
  /* 1B */ BxOpcodeGroup_EVEX_MAP5_1B,
  /* 1C */ BxOpcodeGroup_ERR,
  /* 1D */ BxOpcodeGroup_EVEX_MAP5_1D,
  /* 1E */ BxOpcodeGroup_EVEX_MAP5_1E,
  /* 1F */ BxOpcodeGroup_ERR,
  /* 20 */ BxOpcodeGroup_ERR,
  /* 21 */ BxOpcodeGroup_ERR,
  /* 22 */ BxOpcodeGroup_ERR,
  /* 23 */ BxOpcodeGroup_ERR,
  /* 24 */ BxOpcodeGroup_ERR,
  /* 25 */ BxOpcodeGroup_ERR,
  /* 26 */ BxOpcodeGroup_ERR,
  /* 27 */ BxOpcodeGroup_ERR,
  /* 28 */ BxOpcodeGroup_ERR,
  /* 29 */ BxOpcodeGroup_ERR,
  /* 2A */ BxOpcodeGroup_EVEX_MAP5_2A,
  /* 2B */ BxOpcodeGroup_ERR,
  /* 2C */ BxOpcodeGroup_EVEX_MAP5_2C,
  /* 2D */ BxOpcodeGroup_EVEX_MAP5_2D,
  /* 2E */ BxOpcodeGroup_EVEX_MAP5_2E,
  /* 2F */ BxOpcodeGroup_EVEX_MAP5_2F,
  /* 30 */ BxOpcodeGroup_ERR,
  /* 31 */ BxOpcodeGroup_ERR,
  /* 32 */ BxOpcodeGroup_ERR,
  /* 33 */ BxOpcodeGroup_ERR,
  /* 34 */ BxOpcodeGroup_ERR,
  /* 35 */ BxOpcodeGroup_ERR,
  /* 36 */ BxOpcodeGroup_ERR,
  /* 37 */ BxOpcodeGroup_ERR,
  /* 38 */ BxOpcodeGroup_ERR,
  /* 39 */ BxOpcodeGroup_ERR,
  /* 3A */ BxOpcodeGroup_ERR,
  /* 3B */ BxOpcodeGroup_ERR,
  /* 3C */ BxOpcodeGroup_ERR,
  /* 3D */ BxOpcodeGroup_ERR,
  /* 3E */ BxOpcodeGroup_ERR,
  /* 3F */ BxOpcodeGroup_ERR,
  /* 40 */ BxOpcodeGroup_ERR,
  /* 41 */ BxOpcodeGroup_ERR,
  /* 42 */ BxOpcodeGroup_ERR,
  /* 43 */ BxOpcodeGroup_ERR,
  /* 44 */ BxOpcodeGroup_ERR,
  /* 45 */ BxOpcodeGroup_ERR,
  /* 46 */ BxOpcodeGroup_ERR,
  /* 47 */ BxOpcodeGroup_ERR,
  /* 48 */ BxOpcodeGroup_ERR,
  /* 49 */ BxOpcodeGroup_ERR,
  /* 4A */ BxOpcodeGroup_ERR,
  /* 4B */ BxOpcodeGroup_ERR,
  /* 4C */ BxOpcodeGroup_ERR,
  /* 4D */ BxOpcodeGroup_ERR,
  /* 4E */ BxOpcodeGroup_ERR,
  /* 4F */ BxOpcodeGroup_ERR,
  /* 50 */ BxOpcodeGroup_ERR,
  /* 51 */ BxOpcodeGroup_EVEX_MAP5_51,
  /* 52 */ BxOpcodeGroup_ERR,
  /* 53 */ BxOpcodeGroup_ERR,
  /* 54 */ BxOpcodeGroup_ERR,
  /* 55 */ BxOpcodeGroup_ERR,
  /* 56 */ BxOpcodeGroup_ERR,
  /* 57 */ BxOpcodeGroup_ERR,
  /* 58 */ BxOpcodeGroup_EVEX_MAP5_58,
  /* 59 */ BxOpcodeGroup_EVEX_MAP5_59,
  /* 5A */ BxOpcodeGroup_EVEX_MAP5_5A,
  /* 5B */ BxOpcodeGroup_EVEX_MAP5_5B,
  /* 5C */ BxOpcodeGroup_EVEX_MAP5_5C,
  /* 5D */ BxOpcodeGroup_EVEX_MAP5_5D,
  /* 5E */ BxOpcodeGroup_EVEX_MAP5_5E,
  /* 5F */ BxOpcodeGroup_EVEX_MAP5_5F,
  /* 60 */ BxOpcodeGroup_ERR,
  /* 61 */ BxOpcodeGroup_ERR,
  /* 62 */ BxOpcodeGroup_ERR,
  /* 63 */ BxOpcodeGroup_ERR,
  /* 64 */ BxOpcodeGroup_ERR,
  /* 65 */ BxOpcodeGroup_ERR,
  /* 66 */ BxOpcodeGroup_ERR,
  /* 67 */ BxOpcodeGroup_ERR,
  /* 68 */ BxOpcodeGroup_EVEX_MAP5_68,
  /* 69 */ BxOpcodeGroup_EVEX_MAP5_69,
  /* 6A */ BxOpcodeGroup_EVEX_MAP5_6A,
  /* 6B */ BxOpcodeGroup_EVEX_MAP5_6B,
  /* 6C */ BxOpcodeGroup_EVEX_MAP5_6C,
  /* 6D */ BxOpcodeGroup_EVEX_MAP5_6D,
  /* 6E */ BxOpcodeGroup_EVEX_MAP5_6E,
  /* 6F */ BxOpcodeGroup_EVEX_MAP5_6F,
  /* 70 */ BxOpcodeGroup_ERR,
  /* 71 */ BxOpcodeGroup_ERR,
  /* 72 */ BxOpcodeGroup_ERR,
  /* 73 */ BxOpcodeGroup_ERR,
  /* 74 */ BxOpcodeGroup_EVEX_MAP5_74,
  /* 75 */ BxOpcodeGroup_ERR,
  /* 76 */ BxOpcodeGroup_ERR,
  /* 77 */ BxOpcodeGroup_ERR,
  /* 78 */ BxOpcodeGroup_EVEX_MAP5_78,
  /* 79 */ BxOpcodeGroup_EVEX_MAP5_79,
  /* 7A */ BxOpcodeGroup_EVEX_MAP5_7A,
  /* 7B */ BxOpcodeGroup_EVEX_MAP5_7B,
  /* 7C */ BxOpcodeGroup_EVEX_MAP5_7C,
  /* 7D */ BxOpcodeGroup_EVEX_MAP5_7D,
  /* 7E */ BxOpcodeGroup_EVEX_MAP5_7E,
  /* 7F */ BxOpcodeGroup_ERR,
  /* 80 */ BxOpcodeGroup_ERR,
  /* 81 */ BxOpcodeGroup_ERR,
  /* 82 */ BxOpcodeGroup_ERR,
  /* 83 */ BxOpcodeGroup_ERR,
  /* 84 */ BxOpcodeGroup_ERR,
  /* 85 */ BxOpcodeGroup_ERR,
  /* 86 */ BxOpcodeGroup_ERR,
  /* 87 */ BxOpcodeGroup_ERR,
  /* 88 */ BxOpcodeGroup_ERR,
  /* 89 */ BxOpcodeGroup_ERR,
  /* 8A */ BxOpcodeGroup_ERR,
  /* 8B */ BxOpcodeGroup_ERR,
  /* 8C */ BxOpcodeGroup_ERR,
  /* 8D */ BxOpcodeGroup_ERR,
  /* 8E */ BxOpcodeGroup_ERR,
  /* 8F */ BxOpcodeGroup_ERR,
  /* 90 */ BxOpcodeGroup_ERR,
  /* 91 */ BxOpcodeGroup_ERR,
  /* 92 */ BxOpcodeGroup_ERR,
  /* 93 */ BxOpcodeGroup_ERR,
  /* 94 */ BxOpcodeGroup_ERR,
  /* 95 */ BxOpcodeGroup_ERR,
  /* 96 */ BxOpcodeGroup_ERR,
  /* 97 */ BxOpcodeGroup_ERR,
  /* 98 */ BxOpcodeGroup_ERR,
  /* 99 */ BxOpcodeGroup_ERR,
  /* 9A */ BxOpcodeGroup_ERR,
  /* 9B */ BxOpcodeGroup_ERR,
  /* 9C */ BxOpcodeGroup_ERR,
  /* 9D */ BxOpcodeGroup_ERR,
  /* 9E */ BxOpcodeGroup_ERR,
  /* 9F */ BxOpcodeGroup_ERR,
  /* A0 */ BxOpcodeGroup_ERR,
  /* A1 */ BxOpcodeGroup_ERR,
  /* A2 */ BxOpcodeGroup_ERR,
  /* A3 */ BxOpcodeGroup_ERR,
  /* A4 */ BxOpcodeGroup_ERR,
  /* A5 */ BxOpcodeGroup_ERR,
  /* A6 */ BxOpcodeGroup_ERR,
  /* A7 */ BxOpcodeGroup_ERR,
  /* A8 */ BxOpcodeGroup_ERR,
  /* A9 */ BxOpcodeGroup_ERR,
  /* AA */ BxOpcodeGroup_ERR,
  /* AB */ BxOpcodeGroup_ERR,
  /* AC */ BxOpcodeGroup_ERR,
  /* AD */ BxOpcodeGroup_ERR,
  /* AE */ BxOpcodeGroup_ERR,
  /* AF */ BxOpcodeGroup_ERR,
  /* B0 */ BxOpcodeGroup_ERR,
  /* B1 */ BxOpcodeGroup_ERR,
  /* B2 */ BxOpcodeGroup_ERR,
  /* B3 */ BxOpcodeGroup_ERR,
  /* B4 */ BxOpcodeGroup_ERR,
  /* B5 */ BxOpcodeGroup_ERR,
  /* B6 */ BxOpcodeGroup_ERR,
  /* B7 */ BxOpcodeGroup_ERR,
  /* B8 */ BxOpcodeGroup_ERR,
  /* B9 */ BxOpcodeGroup_ERR,
  /* BA */ BxOpcodeGroup_ERR,
  /* BB */ BxOpcodeGroup_ERR,
  /* BC */ BxOpcodeGroup_ERR,
  /* BD */ BxOpcodeGroup_ERR,
  /* BE */ BxOpcodeGroup_ERR,
  /* BF */ BxOpcodeGroup_ERR,
  /* C0 */ BxOpcodeGroup_ERR,
  /* C1 */ BxOpcodeGroup_ERR,
  /* C2 */ BxOpcodeGroup_ERR,
  /* C3 */ BxOpcodeGroup_ERR,
  /* C4 */ BxOpcodeGroup_ERR,
  /* C5 */ BxOpcodeGroup_ERR,
  /* C6 */ BxOpcodeGroup_ERR,
  /* C7 */ BxOpcodeGroup_ERR,
  /* C8 */ BxOpcodeGroup_ERR,
  /* C9 */ BxOpcodeGroup_ERR,
  /* CA */ BxOpcodeGroup_ERR,
  /* CB */ BxOpcodeGroup_ERR,
  /* CC */ BxOpcodeGroup_ERR,
  /* CD */ BxOpcodeGroup_ERR,
  /* CE */ BxOpcodeGroup_ERR,
  /* CF */ BxOpcodeGroup_ERR,
  /* D0 */ BxOpcodeGroup_ERR,
  /* D1 */ BxOpcodeGroup_ERR,
  /* D2 */ BxOpcodeGroup_ERR,
  /* D3 */ BxOpcodeGroup_ERR,
  /* D4 */ BxOpcodeGroup_ERR,
  /* D5 */ BxOpcodeGroup_ERR,
  /* D6 */ BxOpcodeGroup_ERR,
  /* D7 */ BxOpcodeGroup_ERR,
  /* D8 */ BxOpcodeGroup_ERR,
  /* D9 */ BxOpcodeGroup_ERR,
  /* DA */ BxOpcodeGroup_ERR,
  /* DB */ BxOpcodeGroup_ERR,
  /* DC */ BxOpcodeGroup_ERR,
  /* DD */ BxOpcodeGroup_ERR,
  /* DE */ BxOpcodeGroup_ERR,
  /* DF */ BxOpcodeGroup_ERR,
  /* E0 */ BxOpcodeGroup_ERR,
  /* E1 */ BxOpcodeGroup_ERR,
  /* E2 */ BxOpcodeGroup_ERR,
  /* E3 */ BxOpcodeGroup_ERR,
  /* E4 */ BxOpcodeGroup_ERR,
  /* E5 */ BxOpcodeGroup_ERR,
  /* E6 */ BxOpcodeGroup_ERR,
  /* E7 */ BxOpcodeGroup_ERR,
  /* E8 */ BxOpcodeGroup_ERR,
  /* E9 */ BxOpcodeGroup_ERR,
  /* EA */ BxOpcodeGroup_ERR,
  /* EB */ BxOpcodeGroup_ERR,
  /* EC */ BxOpcodeGroup_ERR,
  /* ED */ BxOpcodeGroup_ERR,
  /* EE */ BxOpcodeGroup_ERR,
  /* EF */ BxOpcodeGroup_ERR,
  /* F0 */ BxOpcodeGroup_ERR,
  /* F1 */ BxOpcodeGroup_ERR,
  /* F2 */ BxOpcodeGroup_ERR,
  /* F3 */ BxOpcodeGroup_ERR,
  /* F4 */ BxOpcodeGroup_ERR,
  /* F5 */ BxOpcodeGroup_ERR,
  /* F6 */ BxOpcodeGroup_ERR,
  /* F7 */ BxOpcodeGroup_ERR,
  /* F8 */ BxOpcodeGroup_ERR,
  /* F9 */ BxOpcodeGroup_ERR,
  /* FA */ BxOpcodeGroup_ERR,
  /* FB */ BxOpcodeGroup_ERR,
  /* FC */ BxOpcodeGroup_ERR,
  /* FD */ BxOpcodeGroup_ERR,
  /* FE */ BxOpcodeGroup_ERR,
  /* FF */ BxOpcodeGroup_ERR,

  // 256 entries for EVEX-encoded map6 opcodes
  /* 00 */ BxOpcodeGroup_ERR,
  /* 01 */ BxOpcodeGroup_ERR,
  /* 02 */ BxOpcodeGroup_ERR,
  /* 03 */ BxOpcodeGroup_ERR,
  /* 04 */ BxOpcodeGroup_ERR,
  /* 05 */ BxOpcodeGroup_ERR,
  /* 06 */ BxOpcodeGroup_ERR,
  /* 07 */ BxOpcodeGroup_ERR,
  /* 08 */ BxOpcodeGroup_ERR,
  /* 09 */ BxOpcodeGroup_ERR,
  /* 0A */ BxOpcodeGroup_ERR,
  /* 0B */ BxOpcodeGroup_ERR,
  /* 0C */ BxOpcodeGroup_ERR,
  /* 0D */ BxOpcodeGroup_ERR,
  /* 0E */ BxOpcodeGroup_ERR,
  /* 0F */ BxOpcodeGroup_ERR,
  /* 10 */ BxOpcodeGroup_ERR,
  /* 11 */ BxOpcodeGroup_ERR,
  /* 12 */ BxOpcodeGroup_ERR,
  /* 13 */ BxOpcodeGroup_EVEX_MAP6_13,
  /* 14 */ BxOpcodeGroup_ERR,
  /* 15 */ BxOpcodeGroup_ERR,
  /* 16 */ BxOpcodeGroup_ERR,
  /* 17 */ BxOpcodeGroup_ERR,
  /* 18 */ BxOpcodeGroup_ERR,
  /* 19 */ BxOpcodeGroup_ERR,
  /* 1A */ BxOpcodeGroup_ERR,
  /* 1B */ BxOpcodeGroup_ERR,
  /* 1C */ BxOpcodeGroup_ERR,
  /* 1D */ BxOpcodeGroup_ERR,
  /* 1E */ BxOpcodeGroup_ERR,
  /* 1F */ BxOpcodeGroup_ERR,
  /* 20 */ BxOpcodeGroup_ERR,
  /* 21 */ BxOpcodeGroup_ERR,
  /* 22 */ BxOpcodeGroup_ERR,
  /* 23 */ BxOpcodeGroup_ERR,
  /* 24 */ BxOpcodeGroup_ERR,
  /* 25 */ BxOpcodeGroup_ERR,
  /* 26 */ BxOpcodeGroup_ERR,
  /* 27 */ BxOpcodeGroup_ERR,
  /* 28 */ BxOpcodeGroup_ERR,
  /* 29 */ BxOpcodeGroup_ERR,
  /* 2A */ BxOpcodeGroup_ERR,
  /* 2B */ BxOpcodeGroup_ERR,
  /* 2C */ BxOpcodeGroup_EVEX_MAP6_2C,
  /* 2D */ BxOpcodeGroup_EVEX_MAP6_2D,
  /* 2E */ BxOpcodeGroup_ERR,
  /* 2F */ BxOpcodeGroup_ERR,
  /* 30 */ BxOpcodeGroup_ERR,
  /* 31 */ BxOpcodeGroup_ERR,
  /* 32 */ BxOpcodeGroup_ERR,
  /* 33 */ BxOpcodeGroup_ERR,
  /* 34 */ BxOpcodeGroup_ERR,
  /* 35 */ BxOpcodeGroup_ERR,
  /* 36 */ BxOpcodeGroup_ERR,
  /* 37 */ BxOpcodeGroup_ERR,
  /* 38 */ BxOpcodeGroup_ERR,
  /* 39 */ BxOpcodeGroup_ERR,
  /* 3A */ BxOpcodeGroup_ERR,
  /* 3B */ BxOpcodeGroup_ERR,
  /* 3C */ BxOpcodeGroup_ERR,
  /* 3D */ BxOpcodeGroup_ERR,
  /* 3E */ BxOpcodeGroup_ERR,
  /* 3F */ BxOpcodeGroup_ERR,
  /* 40 */ BxOpcodeGroup_ERR,
  /* 41 */ BxOpcodeGroup_ERR,
  /* 42 */ BxOpcodeGroup_EVEX_MAP6_42,
  /* 43 */ BxOpcodeGroup_EVEX_MAP6_43,
  /* 44 */ BxOpcodeGroup_ERR,
  /* 45 */ BxOpcodeGroup_ERR,
  /* 46 */ BxOpcodeGroup_ERR,
  /* 47 */ BxOpcodeGroup_ERR,
  /* 48 */ BxOpcodeGroup_ERR,
  /* 49 */ BxOpcodeGroup_ERR,
  /* 4A */ BxOpcodeGroup_ERR,
  /* 4B */ BxOpcodeGroup_ERR,
  /* 4C */ BxOpcodeGroup_EVEX_MAP6_4C,
  /* 4D */ BxOpcodeGroup_EVEX_MAP6_4D,
  /* 4E */ BxOpcodeGroup_EVEX_MAP6_4E,
  /* 4F */ BxOpcodeGroup_EVEX_MAP6_4F,
  /* 50 */ BxOpcodeGroup_ERR,
  /* 51 */ BxOpcodeGroup_ERR,
  /* 52 */ BxOpcodeGroup_ERR,
  /* 53 */ BxOpcodeGroup_ERR,
  /* 54 */ BxOpcodeGroup_ERR,
  /* 55 */ BxOpcodeGroup_ERR,
  /* 56 */ BxOpcodeGroup_EVEX_MAP6_56,
  /* 57 */ BxOpcodeGroup_EVEX_MAP6_57,
  /* 58 */ BxOpcodeGroup_ERR,
  /* 59 */ BxOpcodeGroup_ERR,
  /* 5A */ BxOpcodeGroup_ERR,
  /* 5B */ BxOpcodeGroup_ERR,
  /* 5C */ BxOpcodeGroup_ERR,
  /* 5D */ BxOpcodeGroup_ERR,
  /* 5E */ BxOpcodeGroup_ERR,
  /* 5F */ BxOpcodeGroup_ERR,
  /* 60 */ BxOpcodeGroup_ERR,
  /* 61 */ BxOpcodeGroup_ERR,
  /* 62 */ BxOpcodeGroup_ERR,
  /* 63 */ BxOpcodeGroup_ERR,
  /* 64 */ BxOpcodeGroup_ERR,
  /* 65 */ BxOpcodeGroup_ERR,
  /* 66 */ BxOpcodeGroup_ERR,
  /* 67 */ BxOpcodeGroup_ERR,
  /* 68 */ BxOpcodeGroup_ERR,
  /* 69 */ BxOpcodeGroup_ERR,
  /* 6A */ BxOpcodeGroup_ERR,
  /* 6B */ BxOpcodeGroup_ERR,
  /* 6C */ BxOpcodeGroup_ERR,
  /* 6D */ BxOpcodeGroup_ERR,
  /* 6E */ BxOpcodeGroup_ERR,
  /* 6F */ BxOpcodeGroup_ERR,
  /* 70 */ BxOpcodeGroup_ERR,
  /* 71 */ BxOpcodeGroup_ERR,
  /* 72 */ BxOpcodeGroup_ERR,
  /* 73 */ BxOpcodeGroup_ERR,
  /* 74 */ BxOpcodeGroup_ERR,
  /* 75 */ BxOpcodeGroup_ERR,
  /* 76 */ BxOpcodeGroup_ERR,
  /* 77 */ BxOpcodeGroup_ERR,
  /* 78 */ BxOpcodeGroup_ERR,
  /* 79 */ BxOpcodeGroup_ERR,
  /* 7A */ BxOpcodeGroup_ERR,
  /* 7B */ BxOpcodeGroup_ERR,
  /* 7C */ BxOpcodeGroup_ERR,
  /* 7D */ BxOpcodeGroup_ERR,
  /* 7E */ BxOpcodeGroup_ERR,
  /* 7F */ BxOpcodeGroup_ERR,
  /* 80 */ BxOpcodeGroup_ERR,
  /* 81 */ BxOpcodeGroup_ERR,
  /* 82 */ BxOpcodeGroup_ERR,
  /* 83 */ BxOpcodeGroup_ERR,
  /* 84 */ BxOpcodeGroup_ERR,
  /* 85 */ BxOpcodeGroup_ERR,
  /* 86 */ BxOpcodeGroup_ERR,
  /* 87 */ BxOpcodeGroup_ERR,
  /* 88 */ BxOpcodeGroup_ERR,
  /* 89 */ BxOpcodeGroup_ERR,
  /* 8A */ BxOpcodeGroup_ERR,
  /* 8B */ BxOpcodeGroup_ERR,
  /* 8C */ BxOpcodeGroup_ERR,
  /* 8D */ BxOpcodeGroup_ERR,
  /* 8E */ BxOpcodeGroup_ERR,
  /* 8F */ BxOpcodeGroup_ERR,
  /* 90 */ BxOpcodeGroup_ERR,
  /* 91 */ BxOpcodeGroup_ERR,
  /* 92 */ BxOpcodeGroup_ERR,
  /* 93 */ BxOpcodeGroup_ERR,
  /* 94 */ BxOpcodeGroup_ERR,
  /* 95 */ BxOpcodeGroup_ERR,
  /* 96 */ BxOpcodeGroup_EVEX_MAP6_96,
  /* 97 */ BxOpcodeGroup_EVEX_MAP6_97,
  /* 98 */ BxOpcodeGroup_EVEX_MAP6_98,
  /* 99 */ BxOpcodeGroup_EVEX_MAP6_99,
  /* 9A */ BxOpcodeGroup_EVEX_MAP6_9A,
  /* 9B */ BxOpcodeGroup_EVEX_MAP6_9B,
  /* 9C */ BxOpcodeGroup_EVEX_MAP6_9C,
  /* 9D */ BxOpcodeGroup_EVEX_MAP6_9D,
  /* 9E */ BxOpcodeGroup_EVEX_MAP6_9E,
  /* 9F */ BxOpcodeGroup_EVEX_MAP6_9F,
  /* A0 */ BxOpcodeGroup_ERR,
  /* A1 */ BxOpcodeGroup_ERR,
  /* A2 */ BxOpcodeGroup_ERR,
  /* A3 */ BxOpcodeGroup_ERR,
  /* A4 */ BxOpcodeGroup_ERR,
  /* A5 */ BxOpcodeGroup_ERR,
  /* A6 */ BxOpcodeGroup_EVEX_MAP6_A6,
  /* A7 */ BxOpcodeGroup_EVEX_MAP6_A7,
  /* A8 */ BxOpcodeGroup_EVEX_MAP6_A8,
  /* A9 */ BxOpcodeGroup_EVEX_MAP6_A9,
  /* AA */ BxOpcodeGroup_EVEX_MAP6_AA,
  /* AB */ BxOpcodeGroup_EVEX_MAP6_AB,
  /* AC */ BxOpcodeGroup_EVEX_MAP6_AC,
  /* AD */ BxOpcodeGroup_EVEX_MAP6_AD,
  /* AE */ BxOpcodeGroup_EVEX_MAP6_AE,
  /* AF */ BxOpcodeGroup_EVEX_MAP6_AF,
  /* B0 */ BxOpcodeGroup_ERR,
  /* B1 */ BxOpcodeGroup_ERR,
  /* B2 */ BxOpcodeGroup_ERR,
  /* B3 */ BxOpcodeGroup_ERR,
  /* B4 */ BxOpcodeGroup_ERR,
  /* B5 */ BxOpcodeGroup_ERR,
  /* B6 */ BxOpcodeGroup_EVEX_MAP6_B6,
  /* B7 */ BxOpcodeGroup_EVEX_MAP6_B7,
  /* B8 */ BxOpcodeGroup_EVEX_MAP6_B8,
  /* B9 */ BxOpcodeGroup_EVEX_MAP6_B9,
  /* BA */ BxOpcodeGroup_EVEX_MAP6_BA,
  /* BB */ BxOpcodeGroup_EVEX_MAP6_BB,
  /* BC */ BxOpcodeGroup_EVEX_MAP6_BC,
  /* BD */ BxOpcodeGroup_EVEX_MAP6_BD,
  /* BE */ BxOpcodeGroup_EVEX_MAP6_BE,
  /* BF */ BxOpcodeGroup_EVEX_MAP6_BF,
  /* C0 */ BxOpcodeGroup_ERR,
  /* C1 */ BxOpcodeGroup_ERR,
  /* C2 */ BxOpcodeGroup_ERR,
  /* C3 */ BxOpcodeGroup_ERR,
  /* C4 */ BxOpcodeGroup_ERR,
  /* C5 */ BxOpcodeGroup_ERR,
  /* C6 */ BxOpcodeGroup_ERR,
  /* C7 */ BxOpcodeGroup_ERR,
  /* C8 */ BxOpcodeGroup_ERR,
  /* C9 */ BxOpcodeGroup_ERR,
  /* CA */ BxOpcodeGroup_ERR,
  /* CB */ BxOpcodeGroup_ERR,
  /* CC */ BxOpcodeGroup_ERR,
  /* CD */ BxOpcodeGroup_ERR,
  /* CE */ BxOpcodeGroup_ERR,
  /* CF */ BxOpcodeGroup_ERR,
  /* D0 */ BxOpcodeGroup_ERR,
  /* D1 */ BxOpcodeGroup_ERR,
  /* D2 */ BxOpcodeGroup_ERR,
  /* D3 */ BxOpcodeGroup_ERR,
  /* D4 */ BxOpcodeGroup_ERR,
  /* D5 */ BxOpcodeGroup_ERR,
  /* D6 */ BxOpcodeGroup_EVEX_MAP6_D6,
  /* D7 */ BxOpcodeGroup_EVEX_MAP6_D7,
  /* D8 */ BxOpcodeGroup_ERR,
  /* D9 */ BxOpcodeGroup_ERR,
  /* DA */ BxOpcodeGroup_ERR,
  /* DB */ BxOpcodeGroup_ERR,
  /* DC */ BxOpcodeGroup_ERR,
  /* DD */ BxOpcodeGroup_ERR,
  /* DE */ BxOpcodeGroup_ERR,
  /* DF */ BxOpcodeGroup_ERR,
  /* E0 */ BxOpcodeGroup_ERR,
  /* E1 */ BxOpcodeGroup_ERR,
  /* E2 */ BxOpcodeGroup_ERR,
  /* E3 */ BxOpcodeGroup_ERR,
  /* E4 */ BxOpcodeGroup_ERR,
  /* E5 */ BxOpcodeGroup_ERR,
  /* E6 */ BxOpcodeGroup_ERR,
  /* E7 */ BxOpcodeGroup_ERR,
  /* E8 */ BxOpcodeGroup_ERR,
  /* E9 */ BxOpcodeGroup_ERR,
  /* EA */ BxOpcodeGroup_ERR,
  /* EB */ BxOpcodeGroup_ERR,
  /* EC */ BxOpcodeGroup_ERR,
  /* ED */ BxOpcodeGroup_ERR,
  /* EE */ BxOpcodeGroup_ERR,
  /* EF */ BxOpcodeGroup_ERR,
  /* F0 */ BxOpcodeGroup_ERR,
  /* F1 */ BxOpcodeGroup_ERR,
  /* F2 */ BxOpcodeGroup_ERR,
  /* F3 */ BxOpcodeGroup_ERR,
  /* F4 */ BxOpcodeGroup_ERR,
  /* F5 */ BxOpcodeGroup_ERR,
  /* F6 */ BxOpcodeGroup_ERR,
  /* F7 */ BxOpcodeGroup_ERR,
  /* F8 */ BxOpcodeGroup_ERR,
  /* F9 */ BxOpcodeGroup_ERR,
  /* FA */ BxOpcodeGroup_ERR,
  /* FB */ BxOpcodeGroup_ERR,
  /* FC */ BxOpcodeGroup_ERR,
  /* FD */ BxOpcodeGroup_ERR,
  /* FE */ BxOpcodeGroup_ERR,
  /* FF */ BxOpcodeGroup_ERR
};

#endif // BX_SUPPORT_EVEX
