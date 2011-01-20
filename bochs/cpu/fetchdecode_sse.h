/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode_sse.h,v 1.28 2011-01-20 16:24:42 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2011 Stanislav Shwartsman
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

static const BxOpcodeInfo_t BxOpcodeGroupSSE_ERR[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_PAUSE[6] = {
  /* 66 */ { 0, BX_IA_NOP },
  /* F3 */ { 0, BX_IA_PAUSE },
  /* F2 */ { 0, BX_IA_NOP }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[6] = {
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MOVSS_VssWss },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_MOVUPD_WpdVpd },
  /* F3 */ { BxArithDstRM, BX_IA_MOVSS_WssVss },
  /* F2 */ { BxArithDstRM, BX_IA_MOVSD_WsdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[6] = {
  /* 66 */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_MOVDDUP_VpdWq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[6] = {
  /* 66 */ { 0, BX_IA_MOVLPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[6] = {
  /* 66 */ { 0, BX_IA_UNPCKLPD_VpdWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[6] = {
  /* 66 */ { 0, BX_IA_UNPCKHPD_VpdWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[6] = {
  /* 66 */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[6] = {
  /* 66 */ { 0, BX_IA_MOVHPD_MqVsd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[6] = {
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_MOVAPD_WpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[6] = {
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEd },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[6] = {
  /* 66 */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[6] = {
  /* 66 */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTTSS2SI_GdWss },
  /* F2 */ { 0, BX_IA_CVTTSD2SI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[6] = {
  /* 66 */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTSS2SI_GdWss },
  /* F2 */ { 0, BX_IA_CVTSD2SI_GdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[6] = {
  /* 66 */ { 0, BX_IA_UCOMISD_VsdWsd },            	
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[6] = {
  /* 66 */ { 0, BX_IA_COMISD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50R[6] = {
  /* 66 */ { 0, BX_IA_MOVMSKPD_GdVRpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[6] = {
  /* 66 */ { 0, BX_IA_SQRTPD_VpdWpd },
  /* F3 */ { 0, BX_IA_SQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_SQRTSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RSQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RCPSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[6] = {
  /* 66 */ { 0, BX_IA_ANDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[6] = {
  /* 66 */ { 0, BX_IA_ANDNPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[6] = {
  /* 66 */ { 0, BX_IA_ORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[6] = {
  /* 66 */ { 0, BX_IA_XORPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[6] = {
  /* 66 */ { 0, BX_IA_ADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ADDSS_VssWss },
  /* F2 */ { 0, BX_IA_ADDSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[6] = {
  /* 66 */ { 0, BX_IA_MULPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MULSS_VssWss },
  /* F2 */ { 0, BX_IA_MULSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[6] = {
  /* 66 */ { 0, BX_IA_CVTPD2PS_VpdWpd },
  /* F3 */ { 0, BX_IA_CVTSS2SD_VssWss },
  /* F2 */ { 0, BX_IA_CVTSD2SS_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[6] = {
  /* 66 */ { 0, BX_IA_CVTPS2DQ_VdqWps },
  /* F3 */ { 0, BX_IA_CVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[6] = {
  /* 66 */ { 0, BX_IA_SUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_SUBSS_VssWss },
  /* F2 */ { 0, BX_IA_SUBSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[6] = {
  /* 66 */ { 0, BX_IA_MINPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MINSS_VssWss },
  /* F2 */ { 0, BX_IA_MINSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[6] = {
  /* 66 */ { 0, BX_IA_DIVPD_VpdWpd },
  /* F3 */ { 0, BX_IA_DIVSS_VssWss },
  /* F2 */ { 0, BX_IA_DIVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[6] = {
  /* 66 */ { 0, BX_IA_MAXPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MAXSS_VssWss },
  /* F2 */ { 0, BX_IA_MAXSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKLBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKLWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKLDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[6] = {
  /* 66 */ { 0, BX_IA_PACKSSWB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[6] = {
  /* 66 */ { 0, BX_IA_PCMPGTB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[6] = {
  /* 66 */ { 0, BX_IA_PCMPGTW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[6] = {
  /* 66 */ { 0, BX_IA_PCMPGTD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[6] = {
  /* 66 */ { 0, BX_IA_PACKUSWB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKHBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKHWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[6] = {
  /* 66 */ { 0, BX_IA_PUNPCKHDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[6] = {
  /* 66 */ { 0, BX_IA_PACKSSDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[6] = {
  /* 66 */ { 0, BX_IA_MOVD_VdqEd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQ[6] = {
  /* 66 */ { 0, BX_IA_MOVQ_VdqEq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[6] = {
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdq },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[6] = {
  /* 66 */ { 0, BX_IA_PSHUFD_VdqWdqIb },
  /* F3 */ { 0, BX_IA_PSHUFHW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_PSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[6] = {
  /* 66 */ { 0, BX_IA_PCMPEQB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[6] = {
  /* 66 */ { 0, BX_IA_PCMPEQW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[6] = {
  /* 66 */ { 0, BX_IA_PCMPEQD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[6] = {
  /* 66 */ { 0, BX_IA_HADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HADDPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[6] = {
  /* 66 */ { 0, BX_IA_HSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[6] = {
  /* 66 */ { 0, BX_IA_MOVD_EdVd },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQ[6] = {
  /* 66 */ { 0, BX_IA_MOVQ_EqVq },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_MOVDQA_WdqVdq },
  /* F3 */ { BxArithDstRM, BX_IA_MOVDQU_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[6] = {
  /* 66 */ { 0, BX_IA_CMPPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_CMPSS_VssWssIb },
  /* F2 */ { 0, BX_IA_CMPSD_VsdWsdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[6] = {
  /* 66 */ { 0, BX_IA_PINSRW_VdqEwIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5R[6] = {
  /* 66 */ { 0, BX_IA_PEXTRW_GdUdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[6] = {
  /* 66 */ { 0, BX_IA_SHUFPD_VpdWpdIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[6] = {
  /* 66 */ { 0, BX_IA_ADDSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ADDSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[6] = {
  /* 66 */ { 0, BX_IA_PSRLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[6] = {
  /* 66 */ { 0, BX_IA_PSRLD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[6] = {
  /* 66 */ { 0, BX_IA_PSRLQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[6] = {
  /* 66 */ { 0, BX_IA_PADDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[6] = {
  /* 66 */ { 0, BX_IA_PMULLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[6] = {
  /* 66 */ { BxArithDstRM, BX_IA_MOVQ_WqVq },
  /* F3 */ { 0,            BX_IA_MOVQ2DQ_VdqQq },
  /* F2 */ { 0,            BX_IA_MOVDQ2Q_PqVRq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[6] = {
  /* 66 */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[6] = {
  /* 66 */ { 0, BX_IA_PSUBUSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[6] = {
  /* 66 */ { 0, BX_IA_PSUBUSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[6] = {
  /* 66 */ { 0, BX_IA_PMINUB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[6] = {
  /* 66 */ { 0, BX_IA_PAND_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[6] = {
  /* 66 */ { 0, BX_IA_PADDUSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[6] = {
  /* 66 */ { 0, BX_IA_PADDUSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[6] = {
  /* 66 */ { 0, BX_IA_PMAXUB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[6] = {
  /* 66 */ { 0, BX_IA_PANDN_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[6] = {
  /* 66 */ { 0, BX_IA_PAVGB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[6] = {
  /* 66 */ { 0, BX_IA_PSRAW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[6] = {
  /* 66 */ { 0, BX_IA_PSRAD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[6] = {
  /* 66 */ { 0, BX_IA_PAVGW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[6] = {
  /* 66 */ { 0, BX_IA_PMULHUW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[6] = {
  /* 66 */ { 0, BX_IA_PMULHW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[6] = {
  /* 66 */ { 0, BX_IA_CVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_CVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_CVTPD2DQ_VqWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[6] = {
  /* 66 */ { 0, BX_IA_MOVNTDQ_MdqVdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[6] = {
  /* 66 */ { 0, BX_IA_PSUBSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[6] = {
  /* 66 */ { 0, BX_IA_PSUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[6] = {
  /* 66 */ { 0, BX_IA_PMINSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[6] = {
  /* 66 */ { 0, BX_IA_POR_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[6] = {
  /* 66 */ { 0, BX_IA_PADDSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[6] = {
  /* 66 */ { 0, BX_IA_PADDSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[6] = {
  /* 66 */ { 0, BX_IA_PMAXSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[6] = {
  /* 66 */ { 0, BX_IA_PXOR_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[6] = {
  /* 66 */ { 0, BX_IA_PSLLW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[6] = {
  /* 66 */ { 0, BX_IA_PSLLD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[6] = {
  /* 66 */ { 0, BX_IA_PSLLQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[6] = {
  /* 66 */ { 0, BX_IA_PMULUDQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[6] = {
  /* 66 */ { 0, BX_IA_PMADDWD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[6] = {
  /* 66 */ { 0, BX_IA_PSADBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7R[6] = {
  /* 66 */ { 0, BX_IA_MASKMOVDQU_VdqUdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[6] = {
  /* 66 */ { 0, BX_IA_PSUBB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[6] = {
  /* 66 */ { 0, BX_IA_PSUBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[6] = {
  /* 66 */ { 0, BX_IA_PSUBD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[6] = {
  /* 66 */ { 0, BX_IA_PSUBQ_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[6] = {
  /* 66 */ { 0, BX_IA_PADDB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[6] = {
  /* 66 */ { 0, BX_IA_PADDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[6] = {
  /* 66 */ { 0, BX_IA_PADDD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[6] = {
  /* 66 */ { 0, BX_IA_PSRLW_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[6] = {
  /* 66 */ { 0, BX_IA_PSRAW_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[6] = {
  /* 66 */ { 0, BX_IA_PSLLW_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[6] = {
  /* 66 */ { 0, BX_IA_PSRLD_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[6] = {
  /* 66 */ { 0, BX_IA_PSRAD_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[6] = {
  /* 66 */ { 0, BX_IA_PSLLD_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[6] = {
  /* 66 */ { 0, BX_IA_PSRLQ_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[6] = {
  /* 66 */ { 0, BX_IA_PSLLQ_UdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G9VMX6[6] = {
  /* 66 */ { 0, BX_IA_VMCLEAR_Mq },
  /* F3 */ { 0, BX_IA_VMXON_Mq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[6] = {
  /* 66 */ { 0, BX_IA_PSHUFB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[6] = {
  /* 66 */ { 0, BX_IA_PHADDW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[6] = {
  /* 66 */ { 0, BX_IA_PHADDD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[6] = {
  /* 66 */ { 0, BX_IA_PHADDSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[6] = {
  /* 66 */ { 0, BX_IA_PMADDUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[6] = {
  /* 66 */ { 0, BX_IA_PHSUBW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[6] = {
  /* 66 */ { 0, BX_IA_PHSUBD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[6] = {
  /* 66 */ { 0, BX_IA_PHSUBSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[6] = {
  /* 66 */ { 0, BX_IA_PSIGNB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[6] = {
  /* 66 */ { 0, BX_IA_PSIGNW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[6] = {
  /* 66 */ { 0, BX_IA_PSIGND_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[6] = {
  /* 66 */ { 0, BX_IA_PMULHRSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[6] = {
  /* 66 */ { 0, BX_IA_PABSB_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[6] = {
  /* 66 */ { 0, BX_IA_PABSW_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[6] = {
  /* 66 */ { 0, BX_IA_PABSD_VdqWdq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEb }
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f0[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_GwEw, BxOpcodeGroupSSE_0f38f0 },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_GdEd, BxOpcodeGroupSSE_0f38f0 },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_GqEq, BxOpcodeGroupSSE_0f38f0 },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1w[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1d[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1q[6] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f1[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_EwGw, BxOpcodeGroupSSE_0f38f1w },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_EdGd, BxOpcodeGroupSSE_0f38f1d },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_EqGq, BxOpcodeGroupSSE_0f38f1q },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0f[6] = {
  /* 66 */ { 0, BX_IA_PALIGNR_VdqWdqIb },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
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
  /* 80 */ { BxPrefixSSE66 | BxTraceEnd, BX_IA_INVEPT },
  /* 81 */ { BxPrefixSSE66 | BxTraceEnd, BX_IA_INVVPID },
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
  /* DB */ { BxPrefixSSE66, BX_IA_AESIMC_VdqWdq },
  /* DC */ { BxPrefixSSE66, BX_IA_AESENC_VdqWdq },
  /* DD */ { BxPrefixSSE66, BX_IA_AESENCLAST_VdqWdq },
  /* DE */ { BxPrefixSSE66, BX_IA_AESDEC_VdqWdq },
  /* DF */ { BxPrefixSSE66, BX_IA_AESDECLAST_VdqWdq },
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
  /* 14 */ { BxPrefixSSE66, BX_IA_PEXTRB_EbdVdqIb },
  /* 15 */ { BxPrefixSSE66, BX_IA_PEXTRW_EwdVdqIb },
  /* 16 */ { BxPrefixSSE66, BX_IA_PEXTRD_EdVdqIb },
  /* 17 */ { BxPrefixSSE66, BX_IA_EXTRACTPS_EdVpsIb },
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
  /* DF */ { BxPrefixSSE66, BX_IA_AESKEYGENASSIST_VdqWdqIb },
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
