/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode_sse.h,v 1.7 2010-03-30 15:01:09 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2010 Stanislav Shwartsman
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

#ifndef BX_SSE_FETCHDECODE_TABLES_H
#define BX_SSE_FETCHDECODE_TABLES_H

/* ********** */
/* SSE Groups */
/* ********** */

static const BxOpcodeInfo_t BxOpcodeGroupSSE_ERR[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_PAUSE[3] = {
  /* 66 */ { 0, BX_IA_NOP },
  /* F2 */ { 0, BX_IA_NOP },
  /* F3 */ { 0, BX_IA_PAUSE }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10R[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpdR },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsdR },
  /* F3 */ { 0, BX_IA_MOVSS_VssWssR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10M[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpdM },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsdM },
  /* F3 */ { 0, BX_IA_MOVSS_VssWssM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11R[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_WpdVpdR },
  /* F2 */ { BxArithDstRM, BX_IA_MOVSD_VsdWsdR },
  /* F3 */ { BxArithDstRM, BX_IA_MOVSS_VssWssR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11M[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_WpdVpdM },
  /* F2 */ { 0, BX_IA_MOVSD_WsdVsdM },
  /* F3 */ { 0, BX_IA_MOVSS_WssVssM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[3] = {
  /* 66 */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F2 */ { 0, BX_IA_MOVDDUP_VpdWq },	// SSE3
  /* F3 */ { 0, BX_IA_MOVSLDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[3] = {
  /* 66 */ { 0, BX_IA_MOVLPD_MqVsd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[3] = {
  /* 66 */ { 0, BX_IA_UNPCKLPD_VpdWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[3] = {
  /* 66 */ { 0, BX_IA_UNPCKHPD_VpdWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[3] = {
  /* 66 */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVSHDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[3] = {
  /* 66 */ { 0, BX_IA_MOVHPD_MqVsd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28R[3] = {
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpdR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28M[3] = {
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpdM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29R[3] = {
  /* 66 */ { 0, BX_IA_MOVAPD_WpdVpdR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29M[3] = {
  /* 66 */ { 0, BX_IA_MOVAPD_WpdVpdM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2aR[3] = {
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQqR },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEdR },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEdR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2aM[3] = {
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQqM },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEdM },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[3] = {
  /* 66 */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[3] = {
  /* 66 */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F2 */ { 0, BX_IA_CVTTSD2SI_GdWsd },
  /* F3 */ { 0, BX_IA_CVTTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[3] = {
  /* 66 */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F2 */ { 0, BX_IA_CVTSD2SI_GdWsd },
  /* F3 */ { 0, BX_IA_CVTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[3] = {
  /* 66 */ { 0, BX_IA_UCOMISD_VsdWsd },            	
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[3] = {
  /* 66 */ { 0, BX_IA_COMISD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50[3] = {
  /* 66 */ { 0, BX_IA_MOVMSKPD_GdVRpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[3] = {
  /* 66 */ { 0, BX_IA_SQRTPD_VpdWpd },
  /* F2 */ { 0, BX_IA_SQRTSD_VsdWsd },
  /* F3 */ { 0, BX_IA_SQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RSQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RCPSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[3] = {
  /* 66 */ { 0, BX_IA_ANDPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[3] = {
  /* 66 */ { 0, BX_IA_ANDNPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[3] = {
  /* 66 */ { 0, BX_IA_ORPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[3] = {
  /* 66 */ { 0, BX_IA_XORPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[3] = {
  /* 66 */ { 0, BX_IA_ADDPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ADDSD_VsdWsd },
  /* F3 */ { 0, BX_IA_ADDSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[3] = {
  /* 66 */ { 0, BX_IA_MULPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MULSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MULSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[3] = {
  /* 66 */ { 0, BX_IA_CVTPD2PS_VpdWpd },
  /* F2 */ { 0, BX_IA_CVTSD2SS_VsdWsd },
  /* F3 */ { 0, BX_IA_CVTSS2SD_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[3] = {
  /* 66 */ { 0, BX_IA_CVTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_CVTTPS2DQ_VdqWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[3] = {
  /* 66 */ { 0, BX_IA_SUBPD_VpdWpd },
  /* F2 */ { 0, BX_IA_SUBSD_VsdWsd },
  /* F3 */ { 0, BX_IA_SUBSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[3] = {
  /* 66 */ { 0, BX_IA_MINPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MINSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MINSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[3] = {
  /* 66 */ { 0, BX_IA_DIVPD_VpdWpd },
  /* F2 */ { 0, BX_IA_DIVSD_VsdWsd },
  /* F3 */ { 0, BX_IA_DIVSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[3] = {
  /* 66 */ { 0, BX_IA_MAXPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MAXSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MAXSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKLBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKLWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKLDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[3] = {
  /* 66 */ { 0, BX_IA_PACKSSWB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[3] = {
  /* 66 */ { 0, BX_IA_PCMPGTB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[3] = {
  /* 66 */ { 0, BX_IA_PCMPGTW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[3] = {
  /* 66 */ { 0, BX_IA_PCMPGTD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[3] = {
  /* 66 */ { 0, BX_IA_PACKUSWB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKHBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKHWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[3] = {
  /* 66 */ { 0, BX_IA_PUNPCKHDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[3] = {
  /* 66 */ { 0, BX_IA_PACKSSDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eR[3] = {
  /* 66 */ { 0, BX_IA_MOVD_VdqEdR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eM[3] = {
  /* 66 */ { 0, BX_IA_MOVD_VdqEdM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQR[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_VdqEqR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQM[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_VdqEqM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6fR[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdqR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdqR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6fM[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdqM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdqM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[3] = {
  /* 66 */ { 0, BX_IA_PSHUFD_VdqWdqIb  },
  /* F2 */ { 0, BX_IA_PSHUFLW_VdqWdqIb },
  /* F3 */ { 0, BX_IA_PSHUFHW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[3] = {
  /* 66 */ { 0, BX_IA_PCMPEQB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[3] = {
  /* 66 */ { 0, BX_IA_PCMPEQW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[3] = {
  /* 66 */ { 0, BX_IA_PCMPEQD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[3] = {
  /* 66 */ { 0, BX_IA_HADDPD_VpdWpd },     // SSE3
  /* F2 */ { 0, BX_IA_HADDPS_VpsWps },     // SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[3] = {
  /* 66 */ { 0, BX_IA_HSUBPD_VpdWpd },     // SSE3
  /* F2 */ { 0, BX_IA_HSUBPS_VpsWps },     // SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eR[3] = {
  /* 66 */ { 0, BX_IA_MOVD_EdVdR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWqR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eM[3] = {
  /* 66 */ { 0, BX_IA_MOVD_EdVdM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWqM }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQR[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_EqVqR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWqR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQM[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_EqVqM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWqM }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7fR[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_WdqVdqR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVDQU_WdqVdqR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7fM[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_WdqVdqM },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVDQU_WdqVdqM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[3] = {
  /* 66 */ { 0, BX_IA_CMPPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_CMPSD_VsdWsdIb },
  /* F3 */ { 0, BX_IA_CMPSS_VssWssIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[3] = {
  /* 66 */ { 0, BX_IA_PINSRW_VdqEwIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5R[3] = {
  /* 66 */ { 0, BX_IA_PEXTRW_GdUdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[3] = {
  /* 66 */ { 0, BX_IA_SHUFPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[3] = {
  /* 66 */ { 0, BX_IA_ADDSUBPD_VpdWpd },	// SSE3
  /* F2 */ { 0, BX_IA_ADDSUBPS_VpsWps },	// SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[3] = {
  /* 66 */ { 0, BX_IA_PSRLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[3] = {
  /* 66 */ { 0, BX_IA_PSRLD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[3] = {
  /* 66 */ { 0, BX_IA_PSRLQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[3] = {
  /* 66 */ { 0, BX_IA_PADDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[3] = {
  /* 66 */ { 0, BX_IA_PMULLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6R[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_WqVqR    },
  /* F2 */ { 0, BX_IA_MOVDQ2Q_PqVRq },
  /* F3 */ { 0, BX_IA_MOVQ2DQ_VdqQq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[3] = {
  /* 66 */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[3] = {
  /* 66 */ { 0, BX_IA_PSUBUSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[3] = {
  /* 66 */ { 0, BX_IA_PSUBUSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[3] = {
  /* 66 */ { 0, BX_IA_PMINUB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[3] = {
  /* 66 */ { 0, BX_IA_PAND_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[3] = {
  /* 66 */ { 0, BX_IA_PADDUSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[3] = {
  /* 66 */ { 0, BX_IA_PADDUSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[3] = {
  /* 66 */ { 0, BX_IA_PMAXUB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[3] = {
  /* 66 */ { 0, BX_IA_PANDN_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[3] = {
  /* 66 */ { 0, BX_IA_PAVGB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[3] = {
  /* 66 */ { 0, BX_IA_PSRAW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[3] = {
  /* 66 */ { 0, BX_IA_PSRAD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[3] = {
  /* 66 */ { 0, BX_IA_PAVGW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[3] = {
  /* 66 */ { 0, BX_IA_PMULHUW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[3] = {
  /* 66 */ { 0, BX_IA_PMULHW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[3] = {
  /* 66 */ { 0, BX_IA_CVTTPD2DQ_VqWpd },
  /* F2 */ { 0, BX_IA_CVTPD2DQ_VqWpd  },
  /* F3 */ { 0, BX_IA_CVTDQ2PD_VpdWq  }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[3] = {
  /* 66 */ { 0, BX_IA_MOVNTDQ_MdqVdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[3] = {
  /* 66 */ { 0, BX_IA_PSUBSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[3] = {
  /* 66 */ { 0, BX_IA_PSUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[3] = {
  /* 66 */ { 0, BX_IA_PMINSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[3] = {
  /* 66 */ { 0, BX_IA_POR_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[3] = {
  /* 66 */ { 0, BX_IA_PADDSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[3] = {
  /* 66 */ { 0, BX_IA_PADDSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[3] = {
  /* 66 */ { 0, BX_IA_PMAXSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[3] = {
  /* 66 */ { 0, BX_IA_PXOR_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[3] = {
  /* 66 */ { 0, BX_IA_PSLLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[3] = {
  /* 66 */ { 0, BX_IA_PSLLD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[3] = {
  /* 66 */ { 0, BX_IA_PSLLQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[3] = {
  /* 66 */ { 0, BX_IA_PMULUDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[3] = {
  /* 66 */ { 0, BX_IA_PMADDWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[3] = {
  /* 66 */ { 0, BX_IA_PSADBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7R[3] = {
  /* 66 */ { 0, BX_IA_MASKMOVDQU_VdqUdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[3] = {
  /* 66 */ { 0, BX_IA_PSUBB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[3] = {
  /* 66 */ { 0, BX_IA_PSUBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[3] = {
  /* 66 */ { 0, BX_IA_PSUBD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[3] = {
  /* 66 */ { 0, BX_IA_PSUBQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[3] = {
  /* 66 */ { 0, BX_IA_PADDB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[3] = {
  /* 66 */ { 0, BX_IA_PADDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[3] = {
  /* 66 */ { 0, BX_IA_PADDD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[3] = {
  /* 66 */ { 0, BX_IA_PSRLW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[3] = {
  /* 66 */ { 0, BX_IA_PSRAW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[3] = {
  /* 66 */ { 0, BX_IA_PSLLW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[3] = {
  /* 66 */ { 0, BX_IA_PSRLD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[3] = {
  /* 66 */ { 0, BX_IA_PSRAD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[3] = {
  /* 66 */ { 0, BX_IA_PSLLD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[3] = {
  /* 66 */ { 0, BX_IA_PSRLQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[3] = {
  /* 66 */ { 0, BX_IA_PSLLQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G9VMX6[3] = {
  /* 66 */ { 0, BX_IA_VMCLEAR_Mq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_VMXON_Mq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[3] = {
  /* 66 */ { 0, BX_IA_PSHUFB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[3] = {
  /* 66 */ { 0, BX_IA_PHADDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[3] = {
  /* 66 */ { 0, BX_IA_PHADDD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[3] = {
  /* 66 */ { 0, BX_IA_PHADDSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[3] = {
  /* 66 */ { 0, BX_IA_PMADDUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[3] = {
  /* 66 */ { 0, BX_IA_PHSUBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[3] = {
  /* 66 */ { 0, BX_IA_PHSUBD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[3] = {
  /* 66 */ { 0, BX_IA_PHSUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[3] = {
  /* 66 */ { 0, BX_IA_PSIGNB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[3] = {
  /* 66 */ { 0, BX_IA_PSIGNW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[3] = {
  /* 66 */ { 0, BX_IA_PSIGND_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[3] = {
  /* 66 */ { 0, BX_IA_PMULHRSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[3] = {
  /* 66 */ { 0, BX_IA_PABSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[3] = {
  /* 66 */ { 0, BX_IA_PABSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[3] = {
  /* 66 */ { 0, BX_IA_PABSD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEb },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f0[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_GwEw, BxOpcodeGroupSSE_0f38f0 },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_GdEd, BxOpcodeGroupSSE_0f38f0 },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_GqEq, BxOpcodeGroupSSE_0f38f0 },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1w[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEw },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1d[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEd },
  /* F3 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1q[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEq },
  /* F3 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f1[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_EwGw, BxOpcodeGroupSSE_0f38f1w },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_EdGd, BxOpcodeGroupSSE_0f38f1d },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_EqGq, BxOpcodeGroupSSE_0f38f1q },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0f[3] = {
  /* 66 */ { 0, BX_IA_PALIGNR_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f38[256] = {
  /* 00 */ { BxPrefixSSE, BX_IA_PSHUFB_PqQq, BxOpcodeGroupSSE_0f3800 },
  /* 01 */ { BxPrefixSSE, BX_IA_PHADDW_PqQq, BxOpcodeGroupSSE_0f3801 },
  /* 02 */ { BxPrefixSSE, BX_IA_PHADDD_PqQq, BxOpcodeGroupSSE_0f3802 },
  /* 03 */ { BxPrefixSSE, BX_IA_PHADDSW_PqQq, BxOpcodeGroupSSE_0f3803 },
  /* 04 */ { BxPrefixSSE, BX_IA_PMADDUBSW_PqQq, BxOpcodeGroupSSE_0f3804 },
  /* 05 */ { BxPrefixSSE, BX_IA_PHSUBW_PqQq, BxOpcodeGroupSSE_0f3805 },
  /* 06 */ { BxPrefixSSE, BX_IA_PHSUBD_PqQq, BxOpcodeGroupSSE_0f3806 },
  /* 07 */ { BxPrefixSSE, BX_IA_PHSUBSW_PqQq, BxOpcodeGroupSSE_0f3807 },
  /* 08 */ { BxPrefixSSE, BX_IA_PSIGNB_PqQq, BxOpcodeGroupSSE_0f3808 },
  /* 09 */ { BxPrefixSSE, BX_IA_PSIGNW_PqQq, BxOpcodeGroupSSE_0f3809 },
  /* 0A */ { BxPrefixSSE, BX_IA_PSIGND_PqQq, BxOpcodeGroupSSE_0f380a },
  /* 0B */ { BxPrefixSSE, BX_IA_PMULHRSW_PqQq, BxOpcodeGroupSSE_0f380b },
  /* 0C */ { 0, BX_IA_ERROR },
  /* 0D */ { 0, BX_IA_ERROR },
  /* 0E */ { 0, BX_IA_ERROR },
  /* 0F */ { 0, BX_IA_ERROR },
  /* 10 */ { BxPrefixSSE66, BX_IA_PBLENDVB_VdqWdq },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { BxPrefixSSE66, BX_IA_BLENDVPS_VpsWps },
  /* 15 */ { BxPrefixSSE66, BX_IA_BLENDVPD_VpdWpd },
  /* 16 */ { 0, BX_IA_ERROR },
  /* 17 */ { BxPrefixSSE66, BX_IA_PTEST_VdqWdq },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { BxPrefixSSE, BX_IA_PABSB_PqQq, BxOpcodeGroupSSE_0f381c },
  /* 1D */ { BxPrefixSSE, BX_IA_PABSW_PqQq, BxOpcodeGroupSSE_0f381d },
  /* 1E */ { BxPrefixSSE, BX_IA_PABSD_PqQq, BxOpcodeGroupSSE_0f381e },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxPrefixSSE66, BX_IA_PMOVSXBW_VdqWq },
  /* 21 */ { BxPrefixSSE66, BX_IA_PMOVSXBD_VdqWd },
  /* 22 */ { BxPrefixSSE66, BX_IA_PMOVSXBQ_VdqWw },
  /* 23 */ { BxPrefixSSE66, BX_IA_PMOVSXWD_VdqWq },
  /* 24 */ { BxPrefixSSE66, BX_IA_PMOVSXWQ_VdqWd },
  /* 25 */ { BxPrefixSSE66, BX_IA_PMOVSXDQ_VdqWq },
  /* 26 */ { 0, BX_IA_ERROR },
  /* 27 */ { 0, BX_IA_ERROR },
  /* 28 */ { BxPrefixSSE66, BX_IA_PMULDQ_VdqWdq },
  /* 29 */ { BxPrefixSSE66, BX_IA_PCMPEQQ_VdqWdq },
  /* 2A */ { BxPrefixSSE66, BX_IA_MOVNTDQA_VdqMdq },
  /* 2B */ { BxPrefixSSE66, BX_IA_PACKUSDW_VdqWdq },
  /* 2C */ { 0, BX_IA_ERROR },
  /* 2D */ { 0, BX_IA_ERROR },
  /* 2E */ { 0, BX_IA_ERROR },
  /* 2F */ { 0, BX_IA_ERROR },
  /* 30 */ { BxPrefixSSE66, BX_IA_PMOVZXBW_VdqWq },
  /* 31 */ { BxPrefixSSE66, BX_IA_PMOVZXBD_VdqWd },
  /* 32 */ { BxPrefixSSE66, BX_IA_PMOVZXBQ_VdqWw },
  /* 33 */ { BxPrefixSSE66, BX_IA_PMOVZXWD_VdqWq },
  /* 34 */ { BxPrefixSSE66, BX_IA_PMOVZXWQ_VdqWd },
  /* 35 */ { BxPrefixSSE66, BX_IA_PMOVZXDQ_VdqWq },
  /* 36 */ { 0, BX_IA_ERROR },
  /* 37 */ { BxPrefixSSE66, BX_IA_PCMPGTQ_VdqWdq },
  /* 38 */ { BxPrefixSSE66, BX_IA_PMINSB_VdqWdq },
  /* 39 */ { BxPrefixSSE66, BX_IA_PMINSD_VdqWdq },
  /* 3A */ { BxPrefixSSE66, BX_IA_PMINUW_VdqWdq },
  /* 3B */ { BxPrefixSSE66, BX_IA_PMINUD_VdqWdq },
  /* 3C */ { BxPrefixSSE66, BX_IA_PMAXSB_VdqWdq },
  /* 3D */ { BxPrefixSSE66, BX_IA_PMAXSD_VdqWdq },
  /* 3E */ { BxPrefixSSE66, BX_IA_PMAXUW_VdqWdq },
  /* 3F */ { BxPrefixSSE66, BX_IA_PMAXUD_VdqWdq },
  /* 40 */ { BxPrefixSSE66, BX_IA_PMULLD_VdqWdq },
  /* 41 */ { BxPrefixSSE66, BX_IA_PHMINPOSUW_VdqWdq },
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
  /* DB */ { BxPrefixSSE66, BX_IA_AESIMC_VdqWdq },      // AES
  /* DC */ { BxPrefixSSE66, BX_IA_AESENC_VdqWdq },      // AES
  /* DD */ { BxPrefixSSE66, BX_IA_AESENCLAST_VdqWdq },  // AES
  /* DE */ { BxPrefixSSE66, BX_IA_AESDEC_VdqWdq },      // AES
  /* DF */ { BxPrefixSSE66, BX_IA_AESDECLAST_VdqWdq },  // AES
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
  /* F0 */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f0 },
  /* F1 */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f1 },
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

/* ************************************************************************ */
/* 3-byte opcode table (Table A-5, 0F 3A) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f3a[256] = {
  /* 00 */ { 0, BX_IA_ERROR },
  /* 01 */ { 0, BX_IA_ERROR },
  /* 02 */ { 0, BX_IA_ERROR },
  /* 03 */ { 0, BX_IA_ERROR },
  /* 04 */ { 0, BX_IA_ERROR },
  /* 05 */ { 0, BX_IA_ERROR },
  /* 06 */ { 0, BX_IA_ERROR },
  /* 07 */ { 0, BX_IA_ERROR },
  /* 08 */ { BxPrefixSSE66, BX_IA_ROUNDPS_VpsWpsIb },
  /* 09 */ { BxPrefixSSE66, BX_IA_ROUNDPD_VpdWpdIb },
  /* 0A */ { BxPrefixSSE66, BX_IA_ROUNDSS_VssWssIb },
  /* 0B */ { BxPrefixSSE66, BX_IA_ROUNDSD_VsdWsdIb },
  /* 0C */ { BxPrefixSSE66, BX_IA_BLENDPS_VpsWpsIb },
  /* 0D */ { BxPrefixSSE66, BX_IA_BLENDPD_VpdWpdIb },
  /* 0E */ { BxPrefixSSE66, BX_IA_PBLENDW_VdqWdqIb },
  /* 0F */ { BxPrefixSSE, BX_IA_PALIGNR_PqQqIb, BxOpcodeGroupSSE_0f3a0f },
  /* 10 */ { 0, BX_IA_ERROR },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { BxPrefixSSE66, BX_IA_PEXTRB_HbdUdqIb },
  /* 15 */ { BxPrefixSSE66, BX_IA_PEXTRW_HwdUdqIb },
  /* 16 */ { BxPrefixSSE66, BX_IA_PEXTRD_HdUdqIb },
  /* 17 */ { BxPrefixSSE66, BX_IA_EXTRACTPS_HdUpsIb },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { 0, BX_IA_ERROR },
  /* 1D */ { 0, BX_IA_ERROR },
  /* 1E */ { 0, BX_IA_ERROR },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxPrefixSSE66, BX_IA_PINSRB_VdqEbIb },
  /* 21 */ { BxPrefixSSE66, BX_IA_INSERTPS_VpsWssIb },
  /* 22 */ { BxPrefixSSE66, BX_IA_PINSRD_VdqEdIb },
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
  /* 40 */ { BxPrefixSSE66, BX_IA_DPPS_VpsWpsIb },
  /* 41 */ { BxPrefixSSE66, BX_IA_DPPD_VpdWpdIb },
  /* 42 */ { BxPrefixSSE66, BX_IA_MPSADBW_VdqWdqIb },
  /* 43 */ { 0, BX_IA_ERROR },
  /* 44 */ { BxPrefixSSE66, BX_IA_PCLMULQDQ_VdqWdqIb },
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
  /* 60 */ { BxPrefixSSE66, BX_IA_PCMPESTRM_VdqWdqIb },
  /* 61 */ { BxPrefixSSE66, BX_IA_PCMPESTRI_VdqWdqIb },
  /* 62 */ { BxPrefixSSE66, BX_IA_PCMPISTRM_VdqWdqIb },
  /* 63 */ { BxPrefixSSE66, BX_IA_PCMPISTRI_VdqWdqIb },
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
  /* DF */ { BxPrefixSSE66, BX_IA_AESKEYGENASSIST_VdqWdqIb }, // AES
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

#endif // BX_SSE_FETCHDECODE_TABLES_H
