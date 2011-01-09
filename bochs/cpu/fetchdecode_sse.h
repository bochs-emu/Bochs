/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode_sse.h,v 1.24 2011-01-09 20:18:02 sshwarts Exp $
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
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_PAUSE[6] = {
  /* 66 /r */ { 0, BX_IA_NOP },
  /* 66 /m */ { 0, BX_IA_NOP },
  /* F3 /r */ { 0, BX_IA_PAUSE },
  /* F3 /m */ { 0, BX_IA_PAUSE },
  /* F2 /r */ { 0, BX_IA_NOP },
  /* F2 /m */ { 0, BX_IA_NOP }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[6] = {
  /* 66 /r */ { 0, BX_IA_MOVUPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MOVUPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_MOVSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_MOVSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_MOVSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_MOVSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[6] = {
  /* 66 /r */ { BxArithDstRM, BX_IA_MOVUPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MOVUPD_WpdVpdM },
  /* F3 /r */ { BxArithDstRM, BX_IA_MOVSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_MOVSS_WssVssM },
  /* F2 /r */ { BxArithDstRM, BX_IA_MOVSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_MOVSD_WsdVsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F3 /r */ { 0, BX_IA_MOVSLDUP_VpsWpsR },
  /* F3 /m */ { 0, BX_IA_MOVSLDUP_VpsWpsM },
  /* F2 /r */ { 0, BX_IA_MOVDDUP_VpdWqR },
  /* F2 /m */ { 0, BX_IA_MOVDDUP_VpdWqM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVLPD_MqVsd },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[6] = {
  /* 66 /r */ { 0, BX_IA_UNPCKLPD_VpdWdqR },
  /* 66 /m */ { 0, BX_IA_UNPCKLPD_VpdWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[6] = {
  /* 66 /r */ { 0, BX_IA_UNPCKHPD_VpdWdqR },
  /* 66 /m */ { 0, BX_IA_UNPCKHPD_VpdWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F3 /r */ { 0, BX_IA_MOVSHDUP_VpsWpsR },
  /* F3 /m */ { 0, BX_IA_MOVSHDUP_VpsWpsM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVHPD_MqVsd },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[6] = {
  /* 66 /r */ { 0, BX_IA_MOVAPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MOVAPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[6] = {
  /* 66 /r */ { 0, BX_IA_MOVAPD_WpdVpdR },
  /* 66 /m */ { 0, BX_IA_MOVAPD_WpdVpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[6] = {
  /* 66 /r */ { 0, BX_IA_CVTPI2PD_VpdQqR },
  /* 66 /m */ { 0, BX_IA_CVTPI2PD_VpdQqM },
  /* F3 /r */ { 0, BX_IA_CVTSI2SS_VssEdR },
  /* F3 /m */ { 0, BX_IA_CVTSI2SS_VssEdM },
  /* F2 /r */ { 0, BX_IA_CVTSI2SD_VsdEdR },
  /* F2 /m */ { 0, BX_IA_CVTSI2SD_VsdEdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[6] = {
  /* 66 /r */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* 66 /m */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3 /r */ { 0, BX_IA_CVTTSS2SI_GdWssR },
  /* F3 /m */ { 0, BX_IA_CVTTSS2SI_GdWssM },
  /* F2 /r */ { 0, BX_IA_CVTTSD2SI_GdWsdR },
  /* F2 /m */ { 0, BX_IA_CVTTSD2SI_GdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[6] = {
  /* 66 /r */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* 66 /m */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3 /r */ { 0, BX_IA_CVTSS2SI_GdWssR },
  /* F3 /m */ { 0, BX_IA_CVTSS2SI_GdWssM },
  /* F2 /r */ { 0, BX_IA_CVTSD2SI_GdWsdR },
  /* F2 /m */ { 0, BX_IA_CVTSD2SI_GdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[6] = {
  /* 66 /r */ { 0, BX_IA_UCOMISD_VsdWsdR },            	
  /* 66 /m */ { 0, BX_IA_UCOMISD_VsdWsdM },            	
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[6] = {
  /* 66 /r */ { 0, BX_IA_COMISD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_COMISD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50R[6] = {
  /* 66 /r */ { 0, BX_IA_MOVMSKPD_GdVRpd },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[6] = {
  /* 66 /r */ { 0, BX_IA_SQRTPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_SQRTPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_SQRTSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_SQRTSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_SQRTSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_SQRTSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_RSQRTSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_RSQRTSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_RCPSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_RCPSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[6] = {
  /* 66 /r */ { 0, BX_IA_ANDPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_ANDPD_VpdWpdR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[6] = {
  /* 66 /r */ { 0, BX_IA_ANDNPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_ANDNPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[6] = {
  /* 66 /r */ { 0, BX_IA_ORPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_ORPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[6] = {
  /* 66 /r */ { 0, BX_IA_XORPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_XORPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[6] = {
  /* 66 /r */ { 0, BX_IA_ADDPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_ADDPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ADDSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_ADDSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_ADDSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_ADDSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[6] = {
  /* 66 /r */ { 0, BX_IA_MULPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MULPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_MULSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_MULSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_MULSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_MULSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[6] = {
  /* 66 /r */ { 0, BX_IA_CVTPD2PS_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_CVTPD2PS_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_CVTSS2SD_VssWssR },
  /* F3 /m */ { 0, BX_IA_CVTSS2SD_VssWssM },
  /* F2 /r */ { 0, BX_IA_CVTSD2SS_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_CVTSD2SS_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[6] = {
  /* 66 /r */ { 0, BX_IA_CVTPS2DQ_VdqWpsR },
  /* 66 /m */ { 0, BX_IA_CVTPS2DQ_VdqWpsM },
  /* F3 /r */ { 0, BX_IA_CVTTPS2DQ_VdqWpsR },
  /* F3 /m */ { 0, BX_IA_CVTTPS2DQ_VdqWpsM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[6] = {
  /* 66 /r */ { 0, BX_IA_SUBPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_SUBPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_SUBSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_SUBSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_SUBSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_SUBSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[6] = {
  /* 66 /r */ { 0, BX_IA_MINPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MINPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_MINSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_MINSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_MINSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_MINSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[6] = {
  /* 66 /r */ { 0, BX_IA_DIVPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_DIVPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_DIVSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_DIVSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_DIVSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_DIVSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[6] = {
  /* 66 /r */ { 0, BX_IA_MAXPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_MAXPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_MAXSS_VssWssR },
  /* F3 /m */ { 0, BX_IA_MAXSS_VssWssM },
  /* F2 /r */ { 0, BX_IA_MAXSD_VsdWsdR },
  /* F2 /m */ { 0, BX_IA_MAXSD_VsdWsdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKLBW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKLBW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKLWD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKLWD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKLDQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKLDQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[6] = {
  /* 66 /r */ { 0, BX_IA_PACKSSWB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PACKSSWB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPGTB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPGTB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPGTW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPGTW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPGTD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPGTD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[6] = {
  /* 66 /r */ { 0, BX_IA_PACKUSWB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PACKUSWB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKHBW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKHBW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKHWD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKHWD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[6] = {
  /* 66 /r */ { 0, BX_IA_PUNPCKHDQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PUNPCKHDQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[6] = {
  /* 66 /r */ { 0, BX_IA_PACKSSDW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PACKSSDW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[6] = {
  /* 66 /r */ { 0, BX_IA_MOVD_VdqEdR },
  /* 66 /m */ { 0, BX_IA_MOVD_VdqEdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQ[6] = {
  /* 66 /r */ { 0, BX_IA_MOVQ_VdqEqR },
  /* 66 /m */ { 0, BX_IA_MOVQ_VdqEqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[6] = {
  /* 66 /r */ { 0, BX_IA_MOVDQA_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_MOVDQA_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_MOVDQU_VdqWdqR },
  /* F3 /m */ { 0, BX_IA_MOVDQU_VdqWdqM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[6] = {
  /* 66 /r */ { 0, BX_IA_PSHUFD_VdqWdqIbR  },
  /* 66 /m */ { 0, BX_IA_PSHUFD_VdqWdqIbM  },
  /* F3 /r */ { 0, BX_IA_PSHUFHW_VdqWdqIbR },
  /* F3 /m */ { 0, BX_IA_PSHUFHW_VdqWdqIbM },
  /* F2 /r */ { 0, BX_IA_PSHUFLW_VdqWdqIbR },
  /* F2 /m */ { 0, BX_IA_PSHUFLW_VdqWdqIbM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPEQB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPEQB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPEQW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPEQW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[6] = {
  /* 66 /r */ { 0, BX_IA_PCMPEQD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PCMPEQD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[6] = {
  /* 66 /r */ { 0, BX_IA_HADDPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_HADDPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_HADDPS_VpsWpsR },
  /* F2 /m */ { 0, BX_IA_HADDPS_VpsWpsM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[6] = {
  /* 66 /r */ { 0, BX_IA_HSUBPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_HSUBPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_HSUBPS_VpsWpsR },
  /* F2 /m */ { 0, BX_IA_HSUBPS_VpsWpsM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[6] = {
  /* 66 /r */ { 0, BX_IA_MOVD_EdVdR },
  /* 66 /m */ { 0, BX_IA_MOVD_EdVdM },
  /* F3 /r */ { 0, BX_IA_MOVQ_VqWqR },
  /* F3 /m */ { 0, BX_IA_MOVQ_VqWqM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQ[6] = {
  /* 66 /r */ { 0, BX_IA_MOVQ_EqVqR },
  /* 66 /m */ { 0, BX_IA_MOVQ_EqVqM },
  /* F3 /r */ { 0, BX_IA_MOVQ_VqWqR },
  /* F3 /m */ { 0, BX_IA_MOVQ_VqWqM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[6] = {
  /* 66 /r */ { 0, BX_IA_MOVDQA_WdqVdqR },
  /* 66 /m */ { 0, BX_IA_MOVDQA_WdqVdqM },
  /* F3 /r */ { 0, BX_IA_MOVDQU_WdqVdqR },
  /* F3 /m */ { 0, BX_IA_MOVDQU_WdqVdqM },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[6] = {
  /* 66 /r */ { 0, BX_IA_CMPPD_VpdWpdIbR },
  /* 66 /m */ { 0, BX_IA_CMPPD_VpdWpdIbM },
  /* F3 /r */ { 0, BX_IA_CMPSS_VssWssIbR },
  /* F3 /m */ { 0, BX_IA_CMPSS_VssWssIbM },
  /* F2 /r */ { 0, BX_IA_CMPSD_VsdWsdIbR },
  /* F2 /m */ { 0, BX_IA_CMPSD_VsdWsdIbM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[6] = {
  /* 66 /r */ { 0, BX_IA_PINSRW_VdqEwIbR },
  /* 66 /m */ { 0, BX_IA_PINSRW_VdqEwIbM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5R[6] = {
  /* 66 /r */ { 0, BX_IA_PEXTRW_GdUdqIb },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[6] = {
  /* 66 /r */ { 0, BX_IA_SHUFPD_VpdWpdIbR },
  /* 66 /m */ { 0, BX_IA_SHUFPD_VpdWpdIbM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[6] = {
  /* 66 /r */ { 0, BX_IA_ADDSUBPD_VpdWpdR },
  /* 66 /m */ { 0, BX_IA_ADDSUBPD_VpdWpdM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ADDSUBPS_VpsWpsR },
  /* F2 /m */ { 0, BX_IA_ADDSUBPS_VpsWpsM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSRLW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSRLD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSRLQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[6] = {
  /* 66 /r */ { 0, BX_IA_PADDQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[6] = {
  /* 66 /r */ { 0, BX_IA_PMULLW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMULLW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[6] = {
  /* 66 /r */ { 0, BX_IA_MOVQ_WqVqR },
  /* 66 /m */ { 0, BX_IA_MOVQ_WqVqM },
  /* F3 /r */ { 0, BX_IA_MOVQ2DQ_VdqQq },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_MOVDQ2Q_PqVRq },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[6] = {
  /* 66 /r */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* 66 /m */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBUSB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBUSB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBUSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBUSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[6] = {
  /* 66 /r */ { 0, BX_IA_PMINUB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMINUB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[6] = {
  /* 66 /r */ { 0, BX_IA_PAND_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PAND_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[6] = {
  /* 66 /r */ { 0, BX_IA_PADDUSB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDUSB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[6] = {
  /* 66 /r */ { 0, BX_IA_PADDUSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDUSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[6] = {
  /* 66 /r */ { 0, BX_IA_PMAXUB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMAXUB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[6] = {
  /* 66 /r */ { 0, BX_IA_PANDN_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PANDN_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[6] = {
  /* 66 /r */ { 0, BX_IA_PAVGB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PAVGB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[6] = {
  /* 66 /r */ { 0, BX_IA_PSRAW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSRAW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[6] = {
  /* 66 /r */ { 0, BX_IA_PSRAD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSRAD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[6] = {
  /* 66 /r */ { 0, BX_IA_PAVGW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PAVGW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[6] = {
  /* 66 /r */ { 0, BX_IA_PMULHUW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMULHUW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[6] = {
  /* 66 /r */ { 0, BX_IA_PMULHW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMULHW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[6] = {
  /* 66 /r */ { 0, BX_IA_CVTTPD2DQ_VqWpdR },
  /* 66 /m */ { 0, BX_IA_CVTTPD2DQ_VqWpdM },
  /* F3 /r */ { 0, BX_IA_CVTDQ2PD_VpdWqR },
  /* F3 /m */ { 0, BX_IA_CVTDQ2PD_VpdWqM },
  /* F2 /r */ { 0, BX_IA_CVTPD2DQ_VqWpdR },
  /* F2 /m */ { 0, BX_IA_CVTPD2DQ_VqWpdM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_MOVNTDQ_MdqVdq },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBSB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBSB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[6] = {
  /* 66 /r */ { 0, BX_IA_PMINSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMINSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[6] = {
  /* 66 /r */ { 0, BX_IA_POR_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_POR_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[6] = {
  /* 66 /r */ { 0, BX_IA_PADDSB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDSB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[6] = {
  /* 66 /r */ { 0, BX_IA_PADDSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[6] = {
  /* 66 /r */ { 0, BX_IA_PMAXSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMAXSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[6] = {
  /* 66 /r */ { 0, BX_IA_PXOR_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PXOR_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSLLW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSLLD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSLLQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[6] = {
  /* 66 /r */ { 0, BX_IA_PMULUDQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMULUDQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[6] = {
  /* 66 /r */ { 0, BX_IA_PMADDWD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMADDWD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[6] = {
  /* 66 /r */ { 0, BX_IA_PSADBW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSADBW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7R[6] = {
  /* 66 /r */ { 0, BX_IA_MASKMOVDQU_VdqUdq },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[6] = {
  /* 66 /r */ { 0, BX_IA_PSUBQ_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSUBQ_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[6] = {
  /* 66 /r */ { 0, BX_IA_PADDB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[6] = {
  /* 66 /r */ { 0, BX_IA_PADDW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[6] = {
  /* 66 /r */ { 0, BX_IA_PADDD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PADDD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLW_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSRLW_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[6] = {
  /* 66 /r */ { 0, BX_IA_PSRAW_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSRAW_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLW_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSLLW_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLD_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSRLD_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[6] = {
  /* 66 /r */ { 0, BX_IA_PSRAD_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSRAD_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLD_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSLLD_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[6] = {
  /* 66 /r */ { 0, BX_IA_PSRLQ_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSRLQ_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[6] = {
  /* 66 /r */ { 0, BX_IA_PSLLQ_UdqIb },
  /* 66 /m */ { 0, BX_IA_PSLLQ_UdqIb },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G9VMX6[6] = {
  /* 66 /r */ { 0, BX_IA_VMCLEAR_Mq },
  /* 66 /m */ { 0, BX_IA_VMCLEAR_Mq },
  /* F3 /r */ { 0, BX_IA_VMXON_Mq },
  /* F3 /m */ { 0, BX_IA_VMXON_Mq },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[6] = {
  /* 66 /r */ { 0, BX_IA_PSHUFB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSHUFB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[6] = {
  /* 66 /r */ { 0, BX_IA_PHADDW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHADDW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[6] = {
  /* 66 /r */ { 0, BX_IA_PHADDD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHADDD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[6] = {
  /* 66 /r */ { 0, BX_IA_PHADDSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHADDSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[6] = {
  /* 66 /r */ { 0, BX_IA_PMADDUBSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMADDUBSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[6] = {
  /* 66 /r */ { 0, BX_IA_PHSUBW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHSUBW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[6] = {
  /* 66 /r */ { 0, BX_IA_PHSUBD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHSUBD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[6] = {
  /* 66 /r */ { 0, BX_IA_PHSUBSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PHSUBSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[6] = {
  /* 66 /r */ { 0, BX_IA_PSIGNB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSIGNB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[6] = {
  /* 66 /r */ { 0, BX_IA_PSIGNW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSIGNW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[6] = {
  /* 66 /r */ { 0, BX_IA_PSIGND_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PSIGND_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[6] = {
  /* 66 /r */ { 0, BX_IA_PMULHRSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PMULHRSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[6] = {
  /* 66 /r */ { 0, BX_IA_PABSB_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PABSB_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[6] = {
  /* 66 /r */ { 0, BX_IA_PABSW_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PABSW_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[6] = {
  /* 66 /r */ { 0, BX_IA_PABSD_VdqWdqR },
  /* 66 /m */ { 0, BX_IA_PABSD_VdqWdqM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_CRC32_GdEbR },
  /* F2 /m */ { 0, BX_IA_CRC32_GdEbM }
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f0R[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_GwEwR, BxOpcodeGroupSSE_0f38f0 },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_GdEdR, BxOpcodeGroupSSE_0f38f0 },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_GqEqR, BxOpcodeGroupSSE_0f38f0 },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f0M[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSE, BX_IA_MOVBE_GwEwM, BxOpcodeGroupSSE_0f38f0 },
  /* 32 */ { BxPrefixSSE, BX_IA_MOVBE_GdEdM, BxOpcodeGroupSSE_0f38f0 },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSE, BX_IA_MOVBE_GqEqM, BxOpcodeGroupSSE_0f38f0 },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1w[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_CRC32_GdEwR },
  /* F2 /m */ { 0, BX_IA_CRC32_GdEwM }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1d[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_CRC32_GdEdR },
  /* F2 /m */ { 0, BX_IA_CRC32_GdEdM }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1q[6] = {
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_CRC32_GdEqR },
  /* F2 /m */ { 0, BX_IA_CRC32_GdEqM }
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
  /* 66 /r */ { 0, BX_IA_PALIGNR_VdqWdqIbR },
  /* 66 /m */ { 0, BX_IA_PALIGNR_VdqWdqIbM },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f38[256*2] = {
  /* 00 /r */ { BxPrefixSSE, BX_IA_PSHUFB_PqQq, BxOpcodeGroupSSE_0f3800 },
  /* 00 /m */ { BxPrefixSSE, BX_IA_PSHUFB_PqQq, BxOpcodeGroupSSE_0f3800 },
  /* 01 /r */ { BxPrefixSSE, BX_IA_PHADDW_PqQq, BxOpcodeGroupSSE_0f3801 },
  /* 01 /m */ { BxPrefixSSE, BX_IA_PHADDW_PqQq, BxOpcodeGroupSSE_0f3801 },
  /* 02 /r */ { BxPrefixSSE, BX_IA_PHADDD_PqQq, BxOpcodeGroupSSE_0f3802 },
  /* 02 /m */ { BxPrefixSSE, BX_IA_PHADDD_PqQq, BxOpcodeGroupSSE_0f3802 },
  /* 03 /r */ { BxPrefixSSE, BX_IA_PHADDSW_PqQq, BxOpcodeGroupSSE_0f3803 },
  /* 03 /m */ { BxPrefixSSE, BX_IA_PHADDSW_PqQq, BxOpcodeGroupSSE_0f3803 },
  /* 04 /r */ { BxPrefixSSE, BX_IA_PMADDUBSW_PqQq, BxOpcodeGroupSSE_0f3804 },
  /* 04 /m */ { BxPrefixSSE, BX_IA_PMADDUBSW_PqQq, BxOpcodeGroupSSE_0f3804 },
  /* 05 /r */ { BxPrefixSSE, BX_IA_PHSUBW_PqQq, BxOpcodeGroupSSE_0f3805 },
  /* 05 /m */ { BxPrefixSSE, BX_IA_PHSUBW_PqQq, BxOpcodeGroupSSE_0f3805 },
  /* 06 /r */ { BxPrefixSSE, BX_IA_PHSUBD_PqQq, BxOpcodeGroupSSE_0f3806 },
  /* 06 /m */ { BxPrefixSSE, BX_IA_PHSUBD_PqQq, BxOpcodeGroupSSE_0f3806 },
  /* 07 /r */ { BxPrefixSSE, BX_IA_PHSUBSW_PqQq, BxOpcodeGroupSSE_0f3807 },
  /* 07 /m */ { BxPrefixSSE, BX_IA_PHSUBSW_PqQq, BxOpcodeGroupSSE_0f3807 },
  /* 08 /r */ { BxPrefixSSE, BX_IA_PSIGNB_PqQq, BxOpcodeGroupSSE_0f3808 },
  /* 08 /m */ { BxPrefixSSE, BX_IA_PSIGNB_PqQq, BxOpcodeGroupSSE_0f3808 },
  /* 09 /r */ { BxPrefixSSE, BX_IA_PSIGNW_PqQq, BxOpcodeGroupSSE_0f3809 },
  /* 09 /m */ { BxPrefixSSE, BX_IA_PSIGNW_PqQq, BxOpcodeGroupSSE_0f3809 },
  /* 0A /r */ { BxPrefixSSE, BX_IA_PSIGND_PqQq, BxOpcodeGroupSSE_0f380a },
  /* 0A /m */ { BxPrefixSSE, BX_IA_PSIGND_PqQq, BxOpcodeGroupSSE_0f380a },
  /* 0B /r */ { BxPrefixSSE, BX_IA_PMULHRSW_PqQq, BxOpcodeGroupSSE_0f380b },
  /* 0B /m */ { BxPrefixSSE, BX_IA_PMULHRSW_PqQq, BxOpcodeGroupSSE_0f380b },
  /* 0C /r */ { 0, BX_IA_ERROR },
  /* 0C /m */ { 0, BX_IA_ERROR },
  /* 0D /r */ { 0, BX_IA_ERROR },
  /* 0D /m */ { 0, BX_IA_ERROR },
  /* 0E /r */ { 0, BX_IA_ERROR },
  /* 0E /m */ { 0, BX_IA_ERROR },
  /* 0F /r */ { 0, BX_IA_ERROR },
  /* 0F /m */ { 0, BX_IA_ERROR },
  /* 10 /r */ { BxPrefixSSE66, BX_IA_PBLENDVB_VdqWdqR },
  /* 10 /m */ { BxPrefixSSE66, BX_IA_PBLENDVB_VdqWdqM },
  /* 11 /r */ { 0, BX_IA_ERROR },
  /* 11 /m */ { 0, BX_IA_ERROR },
  /* 12 /r */ { 0, BX_IA_ERROR },
  /* 12 /m */ { 0, BX_IA_ERROR },
  /* 13 /r */ { 0, BX_IA_ERROR },
  /* 13 /m */ { 0, BX_IA_ERROR },
  /* 14 /r */ { BxPrefixSSE66, BX_IA_BLENDVPS_VpsWpsR },
  /* 14 /m */ { BxPrefixSSE66, BX_IA_BLENDVPS_VpsWpsM },
  /* 15 /r */ { BxPrefixSSE66, BX_IA_BLENDVPD_VpdWpdR },
  /* 15 /m */ { BxPrefixSSE66, BX_IA_BLENDVPD_VpdWpdM },
  /* 16 /r */ { 0, BX_IA_ERROR },
  /* 16 /m */ { 0, BX_IA_ERROR },
  /* 17 /r */ { BxPrefixSSE66, BX_IA_PTEST_VdqWdqR },
  /* 17 /m */ { BxPrefixSSE66, BX_IA_PTEST_VdqWdqM },
  /* 18 /r */ { 0, BX_IA_ERROR },
  /* 18 /m */ { 0, BX_IA_ERROR },
  /* 19 /r */ { 0, BX_IA_ERROR },
  /* 19 /m */ { 0, BX_IA_ERROR },
  /* 1A /r */ { 0, BX_IA_ERROR },
  /* 1A /m */ { 0, BX_IA_ERROR },
  /* 1B /r */ { 0, BX_IA_ERROR },
  /* 1B /m */ { 0, BX_IA_ERROR },
  /* 1C /r */ { BxPrefixSSE, BX_IA_PABSB_PqQq, BxOpcodeGroupSSE_0f381c },
  /* 1C /m */ { BxPrefixSSE, BX_IA_PABSB_PqQq, BxOpcodeGroupSSE_0f381c },
  /* 1D /r */ { BxPrefixSSE, BX_IA_PABSW_PqQq, BxOpcodeGroupSSE_0f381d },
  /* 1D /m */ { BxPrefixSSE, BX_IA_PABSW_PqQq, BxOpcodeGroupSSE_0f381d },
  /* 1E /r */ { BxPrefixSSE, BX_IA_PABSD_PqQq, BxOpcodeGroupSSE_0f381e },
  /* 1E /m */ { BxPrefixSSE, BX_IA_PABSD_PqQq, BxOpcodeGroupSSE_0f381e },
  /* 1F /r */ { 0, BX_IA_ERROR },
  /* 1F /m */ { 0, BX_IA_ERROR },
  /* 20 /r */ { BxPrefixSSE66, BX_IA_PMOVSXBW_VdqWqR },
  /* 20 /m */ { BxPrefixSSE66, BX_IA_PMOVSXBW_VdqWqM },
  /* 21 /r */ { BxPrefixSSE66, BX_IA_PMOVSXBD_VdqWdR },
  /* 21 /m */ { BxPrefixSSE66, BX_IA_PMOVSXBD_VdqWdM },
  /* 22 /r */ { BxPrefixSSE66, BX_IA_PMOVSXBQ_VdqWwR },
  /* 22 /m */ { BxPrefixSSE66, BX_IA_PMOVSXBQ_VdqWwM },
  /* 23 /r */ { BxPrefixSSE66, BX_IA_PMOVSXWD_VdqWqR },
  /* 23 /m */ { BxPrefixSSE66, BX_IA_PMOVSXWD_VdqWqM },
  /* 24 /r */ { BxPrefixSSE66, BX_IA_PMOVSXWQ_VdqWdR },
  /* 24 /m */ { BxPrefixSSE66, BX_IA_PMOVSXWQ_VdqWdM },
  /* 25 /r */ { BxPrefixSSE66, BX_IA_PMOVSXDQ_VdqWqR },
  /* 25 /m */ { BxPrefixSSE66, BX_IA_PMOVSXDQ_VdqWqM },
  /* 26 /r */ { 0, BX_IA_ERROR },
  /* 26 /m */ { 0, BX_IA_ERROR },
  /* 27 /r */ { 0, BX_IA_ERROR },
  /* 27 /m */ { 0, BX_IA_ERROR },
  /* 28 /r */ { BxPrefixSSE66, BX_IA_PMULDQ_VdqWdqR },
  /* 28 /m */ { BxPrefixSSE66, BX_IA_PMULDQ_VdqWdqM },
  /* 29 /r */ { BxPrefixSSE66, BX_IA_PCMPEQQ_VdqWdqR },
  /* 29 /m */ { BxPrefixSSE66, BX_IA_PCMPEQQ_VdqWdqM },
  /* 2A /r */ { 0, BX_IA_ERROR },
  /* 2A /m */ { BxPrefixSSE66, BX_IA_MOVNTDQA_VdqMdq },
  /* 2B /r */ { BxPrefixSSE66, BX_IA_PACKUSDW_VdqWdqR },
  /* 2B /m */ { BxPrefixSSE66, BX_IA_PACKUSDW_VdqWdqM },
  /* 2C /r */ { 0, BX_IA_ERROR },
  /* 2C /m */ { 0, BX_IA_ERROR },
  /* 2D /r */ { 0, BX_IA_ERROR },
  /* 2D /m */ { 0, BX_IA_ERROR },
  /* 2E /r */ { 0, BX_IA_ERROR },
  /* 2E /m */ { 0, BX_IA_ERROR },
  /* 2F /r */ { 0, BX_IA_ERROR },
  /* 2F /m */ { 0, BX_IA_ERROR },
  /* 30 /r */ { BxPrefixSSE66, BX_IA_PMOVZXBW_VdqWqR },
  /* 30 /m */ { BxPrefixSSE66, BX_IA_PMOVZXBW_VdqWqM },
  /* 31 /r */ { BxPrefixSSE66, BX_IA_PMOVZXBD_VdqWdR },
  /* 31 /m */ { BxPrefixSSE66, BX_IA_PMOVZXBD_VdqWdM },
  /* 32 /r */ { BxPrefixSSE66, BX_IA_PMOVZXBQ_VdqWwR },
  /* 32 /m */ { BxPrefixSSE66, BX_IA_PMOVZXBQ_VdqWwM },
  /* 33 /r */ { BxPrefixSSE66, BX_IA_PMOVZXWD_VdqWqR },
  /* 33 /m */ { BxPrefixSSE66, BX_IA_PMOVZXWD_VdqWqM },
  /* 34 /r */ { BxPrefixSSE66, BX_IA_PMOVZXWQ_VdqWdR },
  /* 34 /m */ { BxPrefixSSE66, BX_IA_PMOVZXWQ_VdqWdM },
  /* 35 /r */ { BxPrefixSSE66, BX_IA_PMOVZXDQ_VdqWqR },
  /* 35 /m */ { BxPrefixSSE66, BX_IA_PMOVZXDQ_VdqWqM },
  /* 36 /r */ { 0, BX_IA_ERROR },
  /* 36 /m */ { 0, BX_IA_ERROR },
  /* 37 /r */ { BxPrefixSSE66, BX_IA_PCMPGTQ_VdqWdqR },
  /* 37 /m */ { BxPrefixSSE66, BX_IA_PCMPGTQ_VdqWdqM },
  /* 38 /r */ { BxPrefixSSE66, BX_IA_PMINSB_VdqWdqR },
  /* 38 /m */ { BxPrefixSSE66, BX_IA_PMINSB_VdqWdqM },
  /* 39 /r */ { BxPrefixSSE66, BX_IA_PMINSD_VdqWdqR },
  /* 39 /m */ { BxPrefixSSE66, BX_IA_PMINSD_VdqWdqM },
  /* 3A /r */ { BxPrefixSSE66, BX_IA_PMINUW_VdqWdqR },
  /* 3A /m */ { BxPrefixSSE66, BX_IA_PMINUW_VdqWdqM },
  /* 3B /r */ { BxPrefixSSE66, BX_IA_PMINUD_VdqWdqR },
  /* 3B /m */ { BxPrefixSSE66, BX_IA_PMINUD_VdqWdqM },
  /* 3C /r */ { BxPrefixSSE66, BX_IA_PMAXSB_VdqWdqR },
  /* 3C /m */ { BxPrefixSSE66, BX_IA_PMAXSB_VdqWdqM },
  /* 3D /r */ { BxPrefixSSE66, BX_IA_PMAXSD_VdqWdqR },
  /* 3D /m */ { BxPrefixSSE66, BX_IA_PMAXSD_VdqWdqM },
  /* 3E /r */ { BxPrefixSSE66, BX_IA_PMAXUW_VdqWdqR },
  /* 3E /m */ { BxPrefixSSE66, BX_IA_PMAXUW_VdqWdqM },
  /* 3F /r */ { BxPrefixSSE66, BX_IA_PMAXUD_VdqWdqR },
  /* 3F /m */ { BxPrefixSSE66, BX_IA_PMAXUD_VdqWdqM },
  /* 40 /r */ { BxPrefixSSE66, BX_IA_PMULLD_VdqWdqR },
  /* 40 /m */ { BxPrefixSSE66, BX_IA_PMULLD_VdqWdqM },
  /* 41 /r */ { BxPrefixSSE66, BX_IA_PHMINPOSUW_VdqWdqR },
  /* 41 /m */ { BxPrefixSSE66, BX_IA_PHMINPOSUW_VdqWdqM },
  /* 42 /r */ { 0, BX_IA_ERROR },
  /* 42 /m */ { 0, BX_IA_ERROR },
  /* 43 /r */ { 0, BX_IA_ERROR },
  /* 43 /m */ { 0, BX_IA_ERROR },
  /* 44 /r */ { 0, BX_IA_ERROR },
  /* 44 /m */ { 0, BX_IA_ERROR },
  /* 45 /r */ { 0, BX_IA_ERROR },
  /* 45 /m */ { 0, BX_IA_ERROR },
  /* 46 /r */ { 0, BX_IA_ERROR },
  /* 46 /m */ { 0, BX_IA_ERROR },
  /* 47 /r */ { 0, BX_IA_ERROR },
  /* 47 /m */ { 0, BX_IA_ERROR },
  /* 48 /r */ { 0, BX_IA_ERROR },
  /* 48 /m */ { 0, BX_IA_ERROR },
  /* 49 /r */ { 0, BX_IA_ERROR },
  /* 49 /m */ { 0, BX_IA_ERROR },
  /* 4A /r */ { 0, BX_IA_ERROR },
  /* 4A /m */ { 0, BX_IA_ERROR },
  /* 4B /r */ { 0, BX_IA_ERROR },
  /* 4B /m */ { 0, BX_IA_ERROR },
  /* 4C /r */ { 0, BX_IA_ERROR },
  /* 4C /m */ { 0, BX_IA_ERROR },
  /* 4D /r */ { 0, BX_IA_ERROR },
  /* 4D /m */ { 0, BX_IA_ERROR },
  /* 4E /r */ { 0, BX_IA_ERROR },
  /* 4E /m */ { 0, BX_IA_ERROR },
  /* 4F /r */ { 0, BX_IA_ERROR },
  /* 4F /m */ { 0, BX_IA_ERROR },
  /* 50 /r */ { 0, BX_IA_ERROR },
  /* 50 /m */ { 0, BX_IA_ERROR },
  /* 51 /r */ { 0, BX_IA_ERROR },
  /* 51 /m */ { 0, BX_IA_ERROR },
  /* 52 /r */ { 0, BX_IA_ERROR },
  /* 52 /m */ { 0, BX_IA_ERROR },
  /* 53 /r */ { 0, BX_IA_ERROR },
  /* 53 /m */ { 0, BX_IA_ERROR },
  /* 54 /r */ { 0, BX_IA_ERROR },
  /* 54 /m */ { 0, BX_IA_ERROR },
  /* 55 /r */ { 0, BX_IA_ERROR },
  /* 55 /m */ { 0, BX_IA_ERROR },
  /* 56 /r */ { 0, BX_IA_ERROR },
  /* 56 /m */ { 0, BX_IA_ERROR },
  /* 57 /r */ { 0, BX_IA_ERROR },
  /* 57 /m */ { 0, BX_IA_ERROR },
  /* 58 /r */ { 0, BX_IA_ERROR },
  /* 58 /m */ { 0, BX_IA_ERROR },
  /* 59 /r */ { 0, BX_IA_ERROR },
  /* 59 /m */ { 0, BX_IA_ERROR },
  /* 5A /r */ { 0, BX_IA_ERROR },
  /* 5A /m */ { 0, BX_IA_ERROR },
  /* 5B /r */ { 0, BX_IA_ERROR },
  /* 5B /m */ { 0, BX_IA_ERROR },
  /* 5C /r */ { 0, BX_IA_ERROR },
  /* 5C /m */ { 0, BX_IA_ERROR },
  /* 5D /r */ { 0, BX_IA_ERROR },
  /* 5D /m */ { 0, BX_IA_ERROR },
  /* 5E /r */ { 0, BX_IA_ERROR },
  /* 5E /m */ { 0, BX_IA_ERROR },
  /* 5F /r */ { 0, BX_IA_ERROR },
  /* 5F /m */ { 0, BX_IA_ERROR },
  /* 60 /r */ { 0, BX_IA_ERROR },
  /* 60 /m */ { 0, BX_IA_ERROR },
  /* 61 /r */ { 0, BX_IA_ERROR },
  /* 61 /m */ { 0, BX_IA_ERROR },
  /* 62 /r */ { 0, BX_IA_ERROR },
  /* 62 /m */ { 0, BX_IA_ERROR },
  /* 63 /r */ { 0, BX_IA_ERROR },
  /* 63 /m */ { 0, BX_IA_ERROR },
  /* 64 /r */ { 0, BX_IA_ERROR },
  /* 64 /m */ { 0, BX_IA_ERROR },
  /* 65 /r */ { 0, BX_IA_ERROR },
  /* 65 /m */ { 0, BX_IA_ERROR },
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* 67 /r */ { 0, BX_IA_ERROR },
  /* 67 /m */ { 0, BX_IA_ERROR },
  /* 68 /r */ { 0, BX_IA_ERROR },
  /* 68 /m */ { 0, BX_IA_ERROR },
  /* 69 /r */ { 0, BX_IA_ERROR },
  /* 69 /m */ { 0, BX_IA_ERROR },
  /* 6A /r */ { 0, BX_IA_ERROR },
  /* 6A /m */ { 0, BX_IA_ERROR },
  /* 6B /r */ { 0, BX_IA_ERROR },
  /* 6B /m */ { 0, BX_IA_ERROR },
  /* 6C /r */ { 0, BX_IA_ERROR },
  /* 6C /m */ { 0, BX_IA_ERROR },
  /* 6D /r */ { 0, BX_IA_ERROR },
  /* 6D /m */ { 0, BX_IA_ERROR },
  /* 6E /r */ { 0, BX_IA_ERROR },
  /* 6E /m */ { 0, BX_IA_ERROR },
  /* 6F /r */ { 0, BX_IA_ERROR },
  /* 6F /m */ { 0, BX_IA_ERROR },
  /* 70 /r */ { 0, BX_IA_ERROR },
  /* 70 /m */ { 0, BX_IA_ERROR },
  /* 71 /r */ { 0, BX_IA_ERROR },
  /* 71 /m */ { 0, BX_IA_ERROR },
  /* 72 /r */ { 0, BX_IA_ERROR },
  /* 72 /m */ { 0, BX_IA_ERROR },
  /* 73 /r */ { 0, BX_IA_ERROR },
  /* 73 /m */ { 0, BX_IA_ERROR },
  /* 74 /r */ { 0, BX_IA_ERROR },
  /* 74 /m */ { 0, BX_IA_ERROR },
  /* 75 /r */ { 0, BX_IA_ERROR },
  /* 75 /m */ { 0, BX_IA_ERROR },
  /* 76 /r */ { 0, BX_IA_ERROR },
  /* 76 /m */ { 0, BX_IA_ERROR },
  /* 77 /r */ { 0, BX_IA_ERROR },
  /* 77 /m */ { 0, BX_IA_ERROR },
  /* 78 /r */ { 0, BX_IA_ERROR },
  /* 78 /m */ { 0, BX_IA_ERROR },
  /* 79 /r */ { 0, BX_IA_ERROR },
  /* 79 /m */ { 0, BX_IA_ERROR },
  /* 7A /r */ { 0, BX_IA_ERROR },
  /* 7A /m */ { 0, BX_IA_ERROR },
  /* 7B /r */ { 0, BX_IA_ERROR },
  /* 7B /m */ { 0, BX_IA_ERROR },
  /* 7C /r */ { 0, BX_IA_ERROR },
  /* 7C /m */ { 0, BX_IA_ERROR },
  /* 7D /r */ { 0, BX_IA_ERROR },
  /* 7D /m */ { 0, BX_IA_ERROR },
  /* 7E /r */ { 0, BX_IA_ERROR },
  /* 7E /m */ { 0, BX_IA_ERROR },
  /* 7F /r */ { 0, BX_IA_ERROR },
  /* 7F /m */ { 0, BX_IA_ERROR },
  /* 80 /r */ { 0, BX_IA_ERROR },
  /* 80 /m */ { BxPrefixSSE66 | BxTraceEnd, BX_IA_INVEPT },
  /* 81 /r */ { 0, BX_IA_ERROR },
  /* 81 /m */ { BxPrefixSSE66 | BxTraceEnd, BX_IA_INVVPID },
  /* 82 /r */ { 0, BX_IA_ERROR },
  /* 82 /m */ { 0, BX_IA_ERROR },
  /* 83 /r */ { 0, BX_IA_ERROR },
  /* 83 /m */ { 0, BX_IA_ERROR },
  /* 84 /r */ { 0, BX_IA_ERROR },
  /* 84 /m */ { 0, BX_IA_ERROR },
  /* 85 /r */ { 0, BX_IA_ERROR },
  /* 85 /m */ { 0, BX_IA_ERROR },
  /* 86 /r */ { 0, BX_IA_ERROR },
  /* 86 /m */ { 0, BX_IA_ERROR },
  /* 87 /r */ { 0, BX_IA_ERROR },
  /* 87 /m */ { 0, BX_IA_ERROR },
  /* 88 /r */ { 0, BX_IA_ERROR },
  /* 88 /m */ { 0, BX_IA_ERROR },
  /* 89 /r */ { 0, BX_IA_ERROR },
  /* 89 /m */ { 0, BX_IA_ERROR },
  /* 8A /r */ { 0, BX_IA_ERROR },
  /* 8A /m */ { 0, BX_IA_ERROR },
  /* 8B /r */ { 0, BX_IA_ERROR },
  /* 8B /m */ { 0, BX_IA_ERROR },
  /* 8C /r */ { 0, BX_IA_ERROR },
  /* 8C /m */ { 0, BX_IA_ERROR },
  /* 8D /r */ { 0, BX_IA_ERROR },
  /* 8D /m */ { 0, BX_IA_ERROR },
  /* 8E /r */ { 0, BX_IA_ERROR },
  /* 8E /m */ { 0, BX_IA_ERROR },
  /* 8F /r */ { 0, BX_IA_ERROR },
  /* 8F /m */ { 0, BX_IA_ERROR },
  /* 90 /r */ { 0, BX_IA_ERROR },
  /* 90 /m */ { 0, BX_IA_ERROR },
  /* 91 /r */ { 0, BX_IA_ERROR },
  /* 91 /m */ { 0, BX_IA_ERROR },
  /* 92 /r */ { 0, BX_IA_ERROR },
  /* 92 /m */ { 0, BX_IA_ERROR },
  /* 93 /r */ { 0, BX_IA_ERROR },
  /* 93 /m */ { 0, BX_IA_ERROR },
  /* 94 /r */ { 0, BX_IA_ERROR },
  /* 94 /m */ { 0, BX_IA_ERROR },
  /* 95 /r */ { 0, BX_IA_ERROR },
  /* 95 /m */ { 0, BX_IA_ERROR },
  /* 96 /r */ { 0, BX_IA_ERROR },
  /* 96 /m */ { 0, BX_IA_ERROR },
  /* 97 /r */ { 0, BX_IA_ERROR },
  /* 97 /m */ { 0, BX_IA_ERROR },
  /* 98 /r */ { 0, BX_IA_ERROR },
  /* 98 /m */ { 0, BX_IA_ERROR },
  /* 99 /r */ { 0, BX_IA_ERROR },
  /* 99 /m */ { 0, BX_IA_ERROR },
  /* 9A /r */ { 0, BX_IA_ERROR },
  /* 9A /m */ { 0, BX_IA_ERROR },
  /* 9B /r */ { 0, BX_IA_ERROR },
  /* 9B /m */ { 0, BX_IA_ERROR },
  /* 9C /r */ { 0, BX_IA_ERROR },
  /* 9C /m */ { 0, BX_IA_ERROR },
  /* 9D /r */ { 0, BX_IA_ERROR },
  /* 9D /m */ { 0, BX_IA_ERROR },
  /* 9E /r */ { 0, BX_IA_ERROR },
  /* 9E /m */ { 0, BX_IA_ERROR },
  /* 9F /r */ { 0, BX_IA_ERROR },
  /* 9F /m */ { 0, BX_IA_ERROR },
  /* A0 /r */ { 0, BX_IA_ERROR },
  /* A0 /m */ { 0, BX_IA_ERROR },
  /* A1 /r */ { 0, BX_IA_ERROR },
  /* A1 /m */ { 0, BX_IA_ERROR },
  /* A2 /r */ { 0, BX_IA_ERROR },
  /* A2 /m */ { 0, BX_IA_ERROR },
  /* A3 /r */ { 0, BX_IA_ERROR },
  /* A3 /m */ { 0, BX_IA_ERROR },
  /* A4 /r */ { 0, BX_IA_ERROR },
  /* A4 /m */ { 0, BX_IA_ERROR },
  /* A5 /r */ { 0, BX_IA_ERROR },
  /* A5 /m */ { 0, BX_IA_ERROR },
  /* A6 /r */ { 0, BX_IA_ERROR },
  /* A6 /m */ { 0, BX_IA_ERROR },
  /* A7 /r */ { 0, BX_IA_ERROR },
  /* A7 /m */ { 0, BX_IA_ERROR },
  /* A8 /r */ { 0, BX_IA_ERROR },
  /* A8 /m */ { 0, BX_IA_ERROR },
  /* A9 /r */ { 0, BX_IA_ERROR },
  /* A9 /m */ { 0, BX_IA_ERROR },
  /* AA /r */ { 0, BX_IA_ERROR },
  /* AA /m */ { 0, BX_IA_ERROR },
  /* AB /r */ { 0, BX_IA_ERROR },
  /* AB /m */ { 0, BX_IA_ERROR },
  /* AC /r */ { 0, BX_IA_ERROR },
  /* AC /m */ { 0, BX_IA_ERROR },
  /* AD /r */ { 0, BX_IA_ERROR },
  /* AD /m */ { 0, BX_IA_ERROR },
  /* AE /r */ { 0, BX_IA_ERROR },
  /* AE /m */ { 0, BX_IA_ERROR },
  /* AF /r */ { 0, BX_IA_ERROR },
  /* AF /m */ { 0, BX_IA_ERROR },
  /* B0 /r */ { 0, BX_IA_ERROR },
  /* B0 /m */ { 0, BX_IA_ERROR },
  /* B1 /r */ { 0, BX_IA_ERROR },
  /* B1 /m */ { 0, BX_IA_ERROR },
  /* B2 /r */ { 0, BX_IA_ERROR },
  /* B2 /m */ { 0, BX_IA_ERROR },
  /* B3 /r */ { 0, BX_IA_ERROR },
  /* B3 /m */ { 0, BX_IA_ERROR },
  /* B4 /r */ { 0, BX_IA_ERROR },
  /* B4 /m */ { 0, BX_IA_ERROR },
  /* B5 /r */ { 0, BX_IA_ERROR },
  /* B5 /m */ { 0, BX_IA_ERROR },
  /* B6 /r */ { 0, BX_IA_ERROR },
  /* B6 /m */ { 0, BX_IA_ERROR },
  /* B7 /r */ { 0, BX_IA_ERROR },
  /* B7 /m */ { 0, BX_IA_ERROR },
  /* B8 /r */ { 0, BX_IA_ERROR },
  /* B8 /m */ { 0, BX_IA_ERROR },
  /* B9 /r */ { 0, BX_IA_ERROR },
  /* B9 /m */ { 0, BX_IA_ERROR },
  /* BA /r */ { 0, BX_IA_ERROR },
  /* BA /m */ { 0, BX_IA_ERROR },
  /* BB /r */ { 0, BX_IA_ERROR },
  /* BB /m */ { 0, BX_IA_ERROR },
  /* BC /r */ { 0, BX_IA_ERROR },
  /* BC /m */ { 0, BX_IA_ERROR },
  /* BD /r */ { 0, BX_IA_ERROR },
  /* BD /m */ { 0, BX_IA_ERROR },
  /* BE /r */ { 0, BX_IA_ERROR },
  /* BE /m */ { 0, BX_IA_ERROR },
  /* BF /r */ { 0, BX_IA_ERROR },
  /* BF /m */ { 0, BX_IA_ERROR },
  /* C0 /r */ { 0, BX_IA_ERROR },
  /* C0 /m */ { 0, BX_IA_ERROR },
  /* C1 /r */ { 0, BX_IA_ERROR },
  /* C1 /m */ { 0, BX_IA_ERROR },
  /* C2 /r */ { 0, BX_IA_ERROR },
  /* C2 /m */ { 0, BX_IA_ERROR },
  /* C3 /r */ { 0, BX_IA_ERROR },
  /* C3 /m */ { 0, BX_IA_ERROR },
  /* C4 /r */ { 0, BX_IA_ERROR },
  /* C4 /m */ { 0, BX_IA_ERROR },
  /* C5 /r */ { 0, BX_IA_ERROR },
  /* C5 /m */ { 0, BX_IA_ERROR },
  /* C6 /r */ { 0, BX_IA_ERROR },
  /* C6 /m */ { 0, BX_IA_ERROR },
  /* C7 /r */ { 0, BX_IA_ERROR },
  /* C7 /m */ { 0, BX_IA_ERROR },
  /* C8 /r */ { 0, BX_IA_ERROR },
  /* C8 /m */ { 0, BX_IA_ERROR },
  /* C9 /r */ { 0, BX_IA_ERROR },
  /* C9 /m */ { 0, BX_IA_ERROR },
  /* CA /r */ { 0, BX_IA_ERROR },
  /* CA /m */ { 0, BX_IA_ERROR },
  /* CB /r */ { 0, BX_IA_ERROR },
  /* CB /m */ { 0, BX_IA_ERROR },
  /* CC /r */ { 0, BX_IA_ERROR },
  /* CC /m */ { 0, BX_IA_ERROR },
  /* CD /r */ { 0, BX_IA_ERROR },
  /* CD /m */ { 0, BX_IA_ERROR },
  /* CE /r */ { 0, BX_IA_ERROR },
  /* CE /m */ { 0, BX_IA_ERROR },
  /* CF /r */ { 0, BX_IA_ERROR },
  /* CF /m */ { 0, BX_IA_ERROR },
  /* D0 /r */ { 0, BX_IA_ERROR },
  /* D0 /m */ { 0, BX_IA_ERROR },
  /* D1 /r */ { 0, BX_IA_ERROR },
  /* D1 /m */ { 0, BX_IA_ERROR },
  /* D2 /r */ { 0, BX_IA_ERROR },
  /* D2 /m */ { 0, BX_IA_ERROR },
  /* D3 /r */ { 0, BX_IA_ERROR },
  /* D3 /m */ { 0, BX_IA_ERROR },
  /* D4 /r */ { 0, BX_IA_ERROR },
  /* D4 /m */ { 0, BX_IA_ERROR },
  /* D5 /r */ { 0, BX_IA_ERROR },
  /* D5 /m */ { 0, BX_IA_ERROR },
  /* D6 /r */ { 0, BX_IA_ERROR },
  /* D6 /m */ { 0, BX_IA_ERROR },
  /* D7 /r */ { 0, BX_IA_ERROR },
  /* D7 /m */ { 0, BX_IA_ERROR },
  /* D8 /r */ { 0, BX_IA_ERROR },
  /* D8 /m */ { 0, BX_IA_ERROR },
  /* D9 /r */ { 0, BX_IA_ERROR },
  /* D9 /m */ { 0, BX_IA_ERROR },
  /* DA /r */ { 0, BX_IA_ERROR },
  /* DA /m */ { 0, BX_IA_ERROR },
  /* DB /r */ { BxPrefixSSE66, BX_IA_AESIMC_VdqWdqR },      // AES
  /* DB /m */ { BxPrefixSSE66, BX_IA_AESIMC_VdqWdqM },      // AES
  /* DC /r */ { BxPrefixSSE66, BX_IA_AESENC_VdqWdqR },      // AES
  /* DC /m */ { BxPrefixSSE66, BX_IA_AESENC_VdqWdqM },      // AES
  /* DD /r */ { BxPrefixSSE66, BX_IA_AESENCLAST_VdqWdqR },  // AES
  /* DD /m */ { BxPrefixSSE66, BX_IA_AESENCLAST_VdqWdqM },  // AES
  /* DE /r */ { BxPrefixSSE66, BX_IA_AESDEC_VdqWdqR },      // AES
  /* DE /m */ { BxPrefixSSE66, BX_IA_AESDEC_VdqWdqM },      // AES
  /* DF /r */ { BxPrefixSSE66, BX_IA_AESDECLAST_VdqWdqR },  // AES
  /* DF /m */ { BxPrefixSSE66, BX_IA_AESDECLAST_VdqWdqM },  // AES
  /* E0 /r */ { 0, BX_IA_ERROR },
  /* E0 /m */ { 0, BX_IA_ERROR },
  /* E1 /r */ { 0, BX_IA_ERROR },
  /* E1 /m */ { 0, BX_IA_ERROR },
  /* E2 /r */ { 0, BX_IA_ERROR },
  /* E2 /m */ { 0, BX_IA_ERROR },
  /* E3 /r */ { 0, BX_IA_ERROR },
  /* E3 /m */ { 0, BX_IA_ERROR },
  /* E4 /r */ { 0, BX_IA_ERROR },
  /* E4 /m */ { 0, BX_IA_ERROR },
  /* E5 /r */ { 0, BX_IA_ERROR },
  /* E5 /m */ { 0, BX_IA_ERROR },
  /* E6 /r */ { 0, BX_IA_ERROR },
  /* E6 /m */ { 0, BX_IA_ERROR },
  /* E7 /r */ { 0, BX_IA_ERROR },
  /* E7 /m */ { 0, BX_IA_ERROR },
  /* E8 /r */ { 0, BX_IA_ERROR },
  /* E8 /m */ { 0, BX_IA_ERROR },
  /* E9 /r */ { 0, BX_IA_ERROR },
  /* E9 /m */ { 0, BX_IA_ERROR },
  /* EA /r */ { 0, BX_IA_ERROR },
  /* EA /m */ { 0, BX_IA_ERROR },
  /* EB /r */ { 0, BX_IA_ERROR },
  /* EB /m */ { 0, BX_IA_ERROR },
  /* EC /r */ { 0, BX_IA_ERROR },
  /* EC /m */ { 0, BX_IA_ERROR },
  /* ED /r */ { 0, BX_IA_ERROR },
  /* ED /m */ { 0, BX_IA_ERROR },
  /* EE /r */ { 0, BX_IA_ERROR },
  /* EE /m */ { 0, BX_IA_ERROR },
  /* EF /r */ { 0, BX_IA_ERROR },
  /* EF /m */ { 0, BX_IA_ERROR },
  /* F0 /r */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f0R },
  /* F0 /m */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f0M },
  /* F1 /r */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f1 },
  /* F1 /m */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize0f38f1 },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F4 /r */ { 0, BX_IA_ERROR },
  /* F4 /m */ { 0, BX_IA_ERROR },
  /* F5 /r */ { 0, BX_IA_ERROR },
  /* F5 /m */ { 0, BX_IA_ERROR },
  /* F6 /r */ { 0, BX_IA_ERROR },
  /* F6 /m */ { 0, BX_IA_ERROR },
  /* F7 /r */ { 0, BX_IA_ERROR },
  /* F7 /m */ { 0, BX_IA_ERROR },
  /* F8 /r */ { 0, BX_IA_ERROR },
  /* F8 /m */ { 0, BX_IA_ERROR },
  /* F9 /r */ { 0, BX_IA_ERROR },
  /* F9 /m */ { 0, BX_IA_ERROR },
  /* FA /r */ { 0, BX_IA_ERROR },
  /* FA /m */ { 0, BX_IA_ERROR },
  /* FB /r */ { 0, BX_IA_ERROR },
  /* FB /m */ { 0, BX_IA_ERROR },
  /* FC /r */ { 0, BX_IA_ERROR },
  /* FC /m */ { 0, BX_IA_ERROR },
  /* FD /r */ { 0, BX_IA_ERROR },
  /* FD /m */ { 0, BX_IA_ERROR },
  /* FE /r */ { 0, BX_IA_ERROR },
  /* FE /m */ { 0, BX_IA_ERROR },
  /* FF /r */ { 0, BX_IA_ERROR },
  /* FF /m */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-5, 0F 3A) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f3a[256*2] = {
  /* 00 /r */ { 0, BX_IA_ERROR },
  /* 00 /m */ { 0, BX_IA_ERROR },
  /* 01 /r */ { 0, BX_IA_ERROR },
  /* 01 /m */ { 0, BX_IA_ERROR },
  /* 02 /r */ { 0, BX_IA_ERROR },
  /* 02 /m */ { 0, BX_IA_ERROR },
  /* 03 /r */ { 0, BX_IA_ERROR },
  /* 03 /m */ { 0, BX_IA_ERROR },
  /* 04 /r */ { 0, BX_IA_ERROR },
  /* 04 /m */ { 0, BX_IA_ERROR },
  /* 05 /r */ { 0, BX_IA_ERROR },
  /* 05 /m */ { 0, BX_IA_ERROR },
  /* 06 /r */ { 0, BX_IA_ERROR },
  /* 06 /m */ { 0, BX_IA_ERROR },
  /* 07 /r */ { 0, BX_IA_ERROR },
  /* 07 /m */ { 0, BX_IA_ERROR },
  /* 08 /r */ { BxPrefixSSE66, BX_IA_ROUNDPS_VpsWpsIbR },
  /* 08 /m */ { BxPrefixSSE66, BX_IA_ROUNDPS_VpsWpsIbM },
  /* 09 /r */ { BxPrefixSSE66, BX_IA_ROUNDPD_VpdWpdIbR },
  /* 09 /m */ { BxPrefixSSE66, BX_IA_ROUNDPD_VpdWpdIbM },
  /* 0A /r */ { BxPrefixSSE66, BX_IA_ROUNDSS_VssWssIbR },
  /* 0A /m */ { BxPrefixSSE66, BX_IA_ROUNDSS_VssWssIbM },
  /* 0B /r */ { BxPrefixSSE66, BX_IA_ROUNDSD_VsdWsdIbR },
  /* 0B /m */ { BxPrefixSSE66, BX_IA_ROUNDSD_VsdWsdIbM },
  /* 0C /r */ { BxPrefixSSE66, BX_IA_BLENDPS_VpsWpsIbR },
  /* 0C /m */ { BxPrefixSSE66, BX_IA_BLENDPS_VpsWpsIbM },
  /* 0D /r */ { BxPrefixSSE66, BX_IA_BLENDPD_VpdWpdIbR },
  /* 0D /m */ { BxPrefixSSE66, BX_IA_BLENDPD_VpdWpdIbM },
  /* 0E /r */ { BxPrefixSSE66, BX_IA_PBLENDW_VdqWdqIbR },
  /* 0E /m */ { BxPrefixSSE66, BX_IA_PBLENDW_VdqWdqIbM },
  /* 0F /r */ { BxPrefixSSE, BX_IA_PALIGNR_PqQqIb, BxOpcodeGroupSSE_0f3a0f },
  /* 0F /m */ { BxPrefixSSE, BX_IA_PALIGNR_PqQqIb, BxOpcodeGroupSSE_0f3a0f },
  /* 10 /r */ { 0, BX_IA_ERROR },
  /* 10 /m */ { 0, BX_IA_ERROR },
  /* 11 /r */ { 0, BX_IA_ERROR },
  /* 11 /m */ { 0, BX_IA_ERROR },
  /* 12 /r */ { 0, BX_IA_ERROR },
  /* 12 /m */ { 0, BX_IA_ERROR },
  /* 13 /r */ { 0, BX_IA_ERROR },
  /* 13 /m */ { 0, BX_IA_ERROR },
  /* 14 /r */ { BxPrefixSSE66, BX_IA_PEXTRB_EbdVdqIbR },
  /* 14 /m */ { BxPrefixSSE66, BX_IA_PEXTRB_EbdVdqIbM },
  /* 15 /r */ { BxPrefixSSE66, BX_IA_PEXTRW_EwdVdqIbR },
  /* 15 /m */ { BxPrefixSSE66, BX_IA_PEXTRW_EwdVdqIbM },
  /* 16 /r */ { BxPrefixSSE66, BX_IA_PEXTRD_EdVdqIbR },
  /* 16 /m */ { BxPrefixSSE66, BX_IA_PEXTRD_EdVdqIbM },
  /* 17 /r */ { BxPrefixSSE66, BX_IA_EXTRACTPS_EdVpsIbR },
  /* 17 /m */ { BxPrefixSSE66, BX_IA_EXTRACTPS_EdVpsIbM },
  /* 18 /r */ { 0, BX_IA_ERROR },
  /* 18 /m */ { 0, BX_IA_ERROR },
  /* 19 /r */ { 0, BX_IA_ERROR },
  /* 19 /m */ { 0, BX_IA_ERROR },
  /* 1A /r */ { 0, BX_IA_ERROR },
  /* 1A /m */ { 0, BX_IA_ERROR },
  /* 1B /r */ { 0, BX_IA_ERROR },
  /* 1B /m */ { 0, BX_IA_ERROR },
  /* 1C /r */ { 0, BX_IA_ERROR },
  /* 1C /m */ { 0, BX_IA_ERROR },
  /* 1D /r */ { 0, BX_IA_ERROR },
  /* 1D /m */ { 0, BX_IA_ERROR },
  /* 1E /r */ { 0, BX_IA_ERROR },
  /* 1E /m */ { 0, BX_IA_ERROR },
  /* 1F /r */ { 0, BX_IA_ERROR },
  /* 1F /m */ { 0, BX_IA_ERROR },
  /* 20 /r */ { BxPrefixSSE66, BX_IA_PINSRB_VdqEbIb },
  /* 20 /m */ { BxPrefixSSE66, BX_IA_PINSRB_VdqEbIb },
  /* 21 /r */ { BxPrefixSSE66, BX_IA_INSERTPS_VpsWssIb },
  /* 21 /m */ { BxPrefixSSE66, BX_IA_INSERTPS_VpsWssIb },
  /* 22 /r */ { BxPrefixSSE66, BX_IA_PINSRD_VdqEdIbR },
  /* 22 /m */ { BxPrefixSSE66, BX_IA_PINSRD_VdqEdIbM },
  /* 23 /r */ { 0, BX_IA_ERROR },
  /* 23 /m */ { 0, BX_IA_ERROR },
  /* 24 /r */ { 0, BX_IA_ERROR },
  /* 24 /m */ { 0, BX_IA_ERROR },
  /* 25 /r */ { 0, BX_IA_ERROR },
  /* 25 /m */ { 0, BX_IA_ERROR },
  /* 26 /r */ { 0, BX_IA_ERROR },
  /* 26 /m */ { 0, BX_IA_ERROR },
  /* 27 /r */ { 0, BX_IA_ERROR },
  /* 27 /m */ { 0, BX_IA_ERROR },
  /* 28 /r */ { 0, BX_IA_ERROR },
  /* 28 /m */ { 0, BX_IA_ERROR },
  /* 29 /r */ { 0, BX_IA_ERROR },
  /* 29 /m */ { 0, BX_IA_ERROR },
  /* 2A /r */ { 0, BX_IA_ERROR },
  /* 2A /m */ { 0, BX_IA_ERROR },
  /* 2B /r */ { 0, BX_IA_ERROR },
  /* 2B /m */ { 0, BX_IA_ERROR },
  /* 2C /r */ { 0, BX_IA_ERROR },
  /* 2C /m */ { 0, BX_IA_ERROR },
  /* 2D /r */ { 0, BX_IA_ERROR },
  /* 2D /m */ { 0, BX_IA_ERROR },
  /* 2E /r */ { 0, BX_IA_ERROR },
  /* 2E /m */ { 0, BX_IA_ERROR },
  /* 2F /r */ { 0, BX_IA_ERROR },
  /* 2F /m */ { 0, BX_IA_ERROR },
  /* 30 /r */ { 0, BX_IA_ERROR },
  /* 30 /m */ { 0, BX_IA_ERROR },
  /* 31 /r */ { 0, BX_IA_ERROR },
  /* 31 /m */ { 0, BX_IA_ERROR },
  /* 32 /r */ { 0, BX_IA_ERROR },
  /* 32 /m */ { 0, BX_IA_ERROR },
  /* 33 /r */ { 0, BX_IA_ERROR },
  /* 33 /m */ { 0, BX_IA_ERROR },
  /* 34 /r */ { 0, BX_IA_ERROR },
  /* 34 /m */ { 0, BX_IA_ERROR },
  /* 35 /r */ { 0, BX_IA_ERROR },
  /* 35 /m */ { 0, BX_IA_ERROR },
  /* 36 /r */ { 0, BX_IA_ERROR },
  /* 36 /m */ { 0, BX_IA_ERROR },
  /* 37 /r */ { 0, BX_IA_ERROR },
  /* 37 /m */ { 0, BX_IA_ERROR },
  /* 38 /r */ { 0, BX_IA_ERROR },
  /* 38 /m */ { 0, BX_IA_ERROR },
  /* 39 /r */ { 0, BX_IA_ERROR },
  /* 39 /m */ { 0, BX_IA_ERROR },
  /* 3A /r */ { 0, BX_IA_ERROR },
  /* 3A /m */ { 0, BX_IA_ERROR },
  /* 3B /r */ { 0, BX_IA_ERROR },
  /* 3B /m */ { 0, BX_IA_ERROR },
  /* 3C /r */ { 0, BX_IA_ERROR },
  /* 3C /m */ { 0, BX_IA_ERROR },
  /* 3D /r */ { 0, BX_IA_ERROR },
  /* 3D /m */ { 0, BX_IA_ERROR },
  /* 3E /r */ { 0, BX_IA_ERROR },
  /* 3E /m */ { 0, BX_IA_ERROR },
  /* 3F /r */ { 0, BX_IA_ERROR },
  /* 3F /m */ { 0, BX_IA_ERROR },
  /* 40 /r */ { BxPrefixSSE66, BX_IA_DPPS_VpsWpsIbR },
  /* 40 /m */ { BxPrefixSSE66, BX_IA_DPPS_VpsWpsIbM },
  /* 41 /r */ { BxPrefixSSE66, BX_IA_DPPD_VpdWpdIbR },
  /* 41 /m */ { BxPrefixSSE66, BX_IA_DPPD_VpdWpdIbM },
  /* 42 /r */ { BxPrefixSSE66, BX_IA_MPSADBW_VdqWdqIbR },
  /* 42 /m */ { BxPrefixSSE66, BX_IA_MPSADBW_VdqWdqIbM },
  /* 43 /r */ { 0, BX_IA_ERROR },
  /* 43 /m */ { 0, BX_IA_ERROR },
  /* 44 /r */ { BxPrefixSSE66, BX_IA_PCLMULQDQ_VdqWdqIbR },
  /* 44 /m */ { BxPrefixSSE66, BX_IA_PCLMULQDQ_VdqWdqIbM },
  /* 45 /r */ { 0, BX_IA_ERROR },
  /* 45 /m */ { 0, BX_IA_ERROR },
  /* 46 /r */ { 0, BX_IA_ERROR },
  /* 46 /m */ { 0, BX_IA_ERROR },
  /* 47 /r */ { 0, BX_IA_ERROR },
  /* 47 /m */ { 0, BX_IA_ERROR },
  /* 48 /r */ { 0, BX_IA_ERROR },
  /* 48 /m */ { 0, BX_IA_ERROR },
  /* 49 /r */ { 0, BX_IA_ERROR },
  /* 49 /m */ { 0, BX_IA_ERROR },
  /* 4A /r */ { 0, BX_IA_ERROR },
  /* 4A /m */ { 0, BX_IA_ERROR },
  /* 4B /r */ { 0, BX_IA_ERROR },
  /* 4B /m */ { 0, BX_IA_ERROR },
  /* 4C /r */ { 0, BX_IA_ERROR },
  /* 4C /m */ { 0, BX_IA_ERROR },
  /* 4D /r */ { 0, BX_IA_ERROR },
  /* 4D /m */ { 0, BX_IA_ERROR },
  /* 4E /r */ { 0, BX_IA_ERROR },
  /* 4E /m */ { 0, BX_IA_ERROR },
  /* 4F /r */ { 0, BX_IA_ERROR },
  /* 4F /m */ { 0, BX_IA_ERROR },
  /* 50 /r */ { 0, BX_IA_ERROR },
  /* 50 /m */ { 0, BX_IA_ERROR },
  /* 51 /r */ { 0, BX_IA_ERROR },
  /* 51 /m */ { 0, BX_IA_ERROR },
  /* 52 /r */ { 0, BX_IA_ERROR },
  /* 52 /m */ { 0, BX_IA_ERROR },
  /* 53 /r */ { 0, BX_IA_ERROR },
  /* 53 /m */ { 0, BX_IA_ERROR },
  /* 54 /r */ { 0, BX_IA_ERROR },
  /* 54 /m */ { 0, BX_IA_ERROR },
  /* 55 /r */ { 0, BX_IA_ERROR },
  /* 55 /m */ { 0, BX_IA_ERROR },
  /* 56 /r */ { 0, BX_IA_ERROR },
  /* 56 /m */ { 0, BX_IA_ERROR },
  /* 57 /r */ { 0, BX_IA_ERROR },
  /* 57 /m */ { 0, BX_IA_ERROR },
  /* 58 /r */ { 0, BX_IA_ERROR },
  /* 58 /m */ { 0, BX_IA_ERROR },
  /* 59 /r */ { 0, BX_IA_ERROR },
  /* 59 /m */ { 0, BX_IA_ERROR },
  /* 5A /r */ { 0, BX_IA_ERROR },
  /* 5A /m */ { 0, BX_IA_ERROR },
  /* 5B /r */ { 0, BX_IA_ERROR },
  /* 5B /m */ { 0, BX_IA_ERROR },
  /* 5C /r */ { 0, BX_IA_ERROR },
  /* 5C /m */ { 0, BX_IA_ERROR },
  /* 5D /r */ { 0, BX_IA_ERROR },
  /* 5D /m */ { 0, BX_IA_ERROR },
  /* 5E /r */ { 0, BX_IA_ERROR },
  /* 5E /m */ { 0, BX_IA_ERROR },
  /* 5F /r */ { 0, BX_IA_ERROR },
  /* 5F /m */ { 0, BX_IA_ERROR },
  /* 60 /r */ { BxPrefixSSE66, BX_IA_PCMPESTRM_VdqWdqIbR },
  /* 60 /m */ { BxPrefixSSE66, BX_IA_PCMPESTRM_VdqWdqIbM },
  /* 61 /r */ { BxPrefixSSE66, BX_IA_PCMPESTRI_VdqWdqIbR },
  /* 61 /m */ { BxPrefixSSE66, BX_IA_PCMPESTRI_VdqWdqIbM },
  /* 62 /r */ { BxPrefixSSE66, BX_IA_PCMPISTRM_VdqWdqIbR },
  /* 62 /m */ { BxPrefixSSE66, BX_IA_PCMPISTRM_VdqWdqIbM },
  /* 63 /r */ { BxPrefixSSE66, BX_IA_PCMPISTRI_VdqWdqIbR },
  /* 63 /m */ { BxPrefixSSE66, BX_IA_PCMPISTRI_VdqWdqIbM },
  /* 64 /r */ { 0, BX_IA_ERROR },
  /* 64 /m */ { 0, BX_IA_ERROR },
  /* 65 /r */ { 0, BX_IA_ERROR },
  /* 65 /m */ { 0, BX_IA_ERROR },
  /* 66 /r */ { 0, BX_IA_ERROR },
  /* 66 /m */ { 0, BX_IA_ERROR },
  /* 67 /r */ { 0, BX_IA_ERROR },
  /* 67 /m */ { 0, BX_IA_ERROR },
  /* 68 /r */ { 0, BX_IA_ERROR },
  /* 68 /m */ { 0, BX_IA_ERROR },
  /* 69 /r */ { 0, BX_IA_ERROR },
  /* 69 /m */ { 0, BX_IA_ERROR },
  /* 6A /r */ { 0, BX_IA_ERROR },
  /* 6A /m */ { 0, BX_IA_ERROR },
  /* 6B /r */ { 0, BX_IA_ERROR },
  /* 6B /m */ { 0, BX_IA_ERROR },
  /* 6C /r */ { 0, BX_IA_ERROR },
  /* 6C /m */ { 0, BX_IA_ERROR },
  /* 6D /r */ { 0, BX_IA_ERROR },
  /* 6D /m */ { 0, BX_IA_ERROR },
  /* 6E /r */ { 0, BX_IA_ERROR },
  /* 6E /m */ { 0, BX_IA_ERROR },
  /* 6F /r */ { 0, BX_IA_ERROR },
  /* 6F /m */ { 0, BX_IA_ERROR },
  /* 70 /r */ { 0, BX_IA_ERROR },
  /* 70 /m */ { 0, BX_IA_ERROR },
  /* 71 /r */ { 0, BX_IA_ERROR },
  /* 71 /m */ { 0, BX_IA_ERROR },
  /* 72 /r */ { 0, BX_IA_ERROR },
  /* 72 /m */ { 0, BX_IA_ERROR },
  /* 73 /r */ { 0, BX_IA_ERROR },
  /* 73 /m */ { 0, BX_IA_ERROR },
  /* 74 /r */ { 0, BX_IA_ERROR },
  /* 74 /m */ { 0, BX_IA_ERROR },
  /* 75 /r */ { 0, BX_IA_ERROR },
  /* 75 /m */ { 0, BX_IA_ERROR },
  /* 76 /r */ { 0, BX_IA_ERROR },
  /* 76 /m */ { 0, BX_IA_ERROR },
  /* 77 /r */ { 0, BX_IA_ERROR },
  /* 77 /m */ { 0, BX_IA_ERROR },
  /* 78 /r */ { 0, BX_IA_ERROR },
  /* 78 /m */ { 0, BX_IA_ERROR },
  /* 79 /r */ { 0, BX_IA_ERROR },
  /* 79 /m */ { 0, BX_IA_ERROR },
  /* 7A /r */ { 0, BX_IA_ERROR },
  /* 7A /m */ { 0, BX_IA_ERROR },
  /* 7B /r */ { 0, BX_IA_ERROR },
  /* 7B /m */ { 0, BX_IA_ERROR },
  /* 7C /r */ { 0, BX_IA_ERROR },
  /* 7C /m */ { 0, BX_IA_ERROR },
  /* 7D /r */ { 0, BX_IA_ERROR },
  /* 7D /m */ { 0, BX_IA_ERROR },
  /* 7E /r */ { 0, BX_IA_ERROR },
  /* 7E /m */ { 0, BX_IA_ERROR },
  /* 7F /r */ { 0, BX_IA_ERROR },
  /* 7F /m */ { 0, BX_IA_ERROR },
  /* 80 /r */ { 0, BX_IA_ERROR },
  /* 80 /m */ { 0, BX_IA_ERROR },
  /* 81 /r */ { 0, BX_IA_ERROR },
  /* 81 /m */ { 0, BX_IA_ERROR },
  /* 82 /r */ { 0, BX_IA_ERROR },
  /* 82 /m */ { 0, BX_IA_ERROR },
  /* 83 /r */ { 0, BX_IA_ERROR },
  /* 83 /m */ { 0, BX_IA_ERROR },
  /* 84 /r */ { 0, BX_IA_ERROR },
  /* 84 /m */ { 0, BX_IA_ERROR },
  /* 85 /r */ { 0, BX_IA_ERROR },
  /* 85 /m */ { 0, BX_IA_ERROR },
  /* 86 /r */ { 0, BX_IA_ERROR },
  /* 86 /m */ { 0, BX_IA_ERROR },
  /* 87 /r */ { 0, BX_IA_ERROR },
  /* 87 /m */ { 0, BX_IA_ERROR },
  /* 88 /r */ { 0, BX_IA_ERROR },
  /* 88 /m */ { 0, BX_IA_ERROR },
  /* 89 /r */ { 0, BX_IA_ERROR },
  /* 89 /m */ { 0, BX_IA_ERROR },
  /* 8A /r */ { 0, BX_IA_ERROR },
  /* 8A /m */ { 0, BX_IA_ERROR },
  /* 8B /r */ { 0, BX_IA_ERROR },
  /* 8B /m */ { 0, BX_IA_ERROR },
  /* 8C /r */ { 0, BX_IA_ERROR },
  /* 8C /m */ { 0, BX_IA_ERROR },
  /* 8D /r */ { 0, BX_IA_ERROR },
  /* 8D /m */ { 0, BX_IA_ERROR },
  /* 8E /r */ { 0, BX_IA_ERROR },
  /* 8E /m */ { 0, BX_IA_ERROR },
  /* 8F /r */ { 0, BX_IA_ERROR },
  /* 8F /m */ { 0, BX_IA_ERROR },
  /* 90 /r */ { 0, BX_IA_ERROR },
  /* 90 /m */ { 0, BX_IA_ERROR },
  /* 91 /r */ { 0, BX_IA_ERROR },
  /* 91 /m */ { 0, BX_IA_ERROR },
  /* 92 /r */ { 0, BX_IA_ERROR },
  /* 92 /m */ { 0, BX_IA_ERROR },
  /* 93 /r */ { 0, BX_IA_ERROR },
  /* 93 /m */ { 0, BX_IA_ERROR },
  /* 94 /r */ { 0, BX_IA_ERROR },
  /* 94 /m */ { 0, BX_IA_ERROR },
  /* 95 /r */ { 0, BX_IA_ERROR },
  /* 95 /m */ { 0, BX_IA_ERROR },
  /* 96 /r */ { 0, BX_IA_ERROR },
  /* 96 /m */ { 0, BX_IA_ERROR },
  /* 97 /r */ { 0, BX_IA_ERROR },
  /* 97 /m */ { 0, BX_IA_ERROR },
  /* 98 /r */ { 0, BX_IA_ERROR },
  /* 98 /m */ { 0, BX_IA_ERROR },
  /* 99 /r */ { 0, BX_IA_ERROR },
  /* 99 /m */ { 0, BX_IA_ERROR },
  /* 9A /r */ { 0, BX_IA_ERROR },
  /* 9A /m */ { 0, BX_IA_ERROR },
  /* 9B /r */ { 0, BX_IA_ERROR },
  /* 9B /m */ { 0, BX_IA_ERROR },
  /* 9C /r */ { 0, BX_IA_ERROR },
  /* 9C /m */ { 0, BX_IA_ERROR },
  /* 9D /r */ { 0, BX_IA_ERROR },
  /* 9D /m */ { 0, BX_IA_ERROR },
  /* 9E /r */ { 0, BX_IA_ERROR },
  /* 9E /m */ { 0, BX_IA_ERROR },
  /* 9F /r */ { 0, BX_IA_ERROR },
  /* 9F /m */ { 0, BX_IA_ERROR },
  /* A0 /r */ { 0, BX_IA_ERROR },
  /* A0 /m */ { 0, BX_IA_ERROR },
  /* A1 /r */ { 0, BX_IA_ERROR },
  /* A1 /m */ { 0, BX_IA_ERROR },
  /* A2 /r */ { 0, BX_IA_ERROR },
  /* A2 /m */ { 0, BX_IA_ERROR },
  /* A3 /r */ { 0, BX_IA_ERROR },
  /* A3 /m */ { 0, BX_IA_ERROR },
  /* A4 /r */ { 0, BX_IA_ERROR },
  /* A4 /m */ { 0, BX_IA_ERROR },
  /* A5 /r */ { 0, BX_IA_ERROR },
  /* A5 /m */ { 0, BX_IA_ERROR },
  /* A6 /r */ { 0, BX_IA_ERROR },
  /* A6 /m */ { 0, BX_IA_ERROR },
  /* A7 /r */ { 0, BX_IA_ERROR },
  /* A7 /m */ { 0, BX_IA_ERROR },
  /* A8 /r */ { 0, BX_IA_ERROR },
  /* A8 /m */ { 0, BX_IA_ERROR },
  /* A9 /r */ { 0, BX_IA_ERROR },
  /* A9 /m */ { 0, BX_IA_ERROR },
  /* AA /r */ { 0, BX_IA_ERROR },
  /* AA /m */ { 0, BX_IA_ERROR },
  /* AB /r */ { 0, BX_IA_ERROR },
  /* AB /m */ { 0, BX_IA_ERROR },
  /* AC /r */ { 0, BX_IA_ERROR },
  /* AC /m */ { 0, BX_IA_ERROR },
  /* AD /r */ { 0, BX_IA_ERROR },
  /* AD /m */ { 0, BX_IA_ERROR },
  /* AE /r */ { 0, BX_IA_ERROR },
  /* AE /m */ { 0, BX_IA_ERROR },
  /* AF /r */ { 0, BX_IA_ERROR },
  /* AF /m */ { 0, BX_IA_ERROR },
  /* B0 /r */ { 0, BX_IA_ERROR },
  /* B0 /m */ { 0, BX_IA_ERROR },
  /* B1 /r */ { 0, BX_IA_ERROR },
  /* B1 /m */ { 0, BX_IA_ERROR },
  /* B2 /r */ { 0, BX_IA_ERROR },
  /* B2 /m */ { 0, BX_IA_ERROR },
  /* B3 /r */ { 0, BX_IA_ERROR },
  /* B3 /m */ { 0, BX_IA_ERROR },
  /* B4 /r */ { 0, BX_IA_ERROR },
  /* B4 /m */ { 0, BX_IA_ERROR },
  /* B5 /r */ { 0, BX_IA_ERROR },
  /* B5 /m */ { 0, BX_IA_ERROR },
  /* B6 /r */ { 0, BX_IA_ERROR },
  /* B6 /m */ { 0, BX_IA_ERROR },
  /* B7 /r */ { 0, BX_IA_ERROR },
  /* B7 /m */ { 0, BX_IA_ERROR },
  /* B8 /r */ { 0, BX_IA_ERROR },
  /* B8 /m */ { 0, BX_IA_ERROR },
  /* B9 /r */ { 0, BX_IA_ERROR },
  /* B9 /m */ { 0, BX_IA_ERROR },
  /* BA /r */ { 0, BX_IA_ERROR },
  /* BA /m */ { 0, BX_IA_ERROR },
  /* BB /r */ { 0, BX_IA_ERROR },
  /* BB /m */ { 0, BX_IA_ERROR },
  /* BC /r */ { 0, BX_IA_ERROR },
  /* BC /m */ { 0, BX_IA_ERROR },
  /* BD /r */ { 0, BX_IA_ERROR },
  /* BD /m */ { 0, BX_IA_ERROR },
  /* BE /r */ { 0, BX_IA_ERROR },
  /* BE /m */ { 0, BX_IA_ERROR },
  /* BF /r */ { 0, BX_IA_ERROR },
  /* BF /m */ { 0, BX_IA_ERROR },
  /* C0 /r */ { 0, BX_IA_ERROR },
  /* C0 /m */ { 0, BX_IA_ERROR },
  /* C1 /r */ { 0, BX_IA_ERROR },
  /* C1 /m */ { 0, BX_IA_ERROR },
  /* C2 /r */ { 0, BX_IA_ERROR },
  /* C2 /m */ { 0, BX_IA_ERROR },
  /* C3 /r */ { 0, BX_IA_ERROR },
  /* C3 /m */ { 0, BX_IA_ERROR },
  /* C4 /r */ { 0, BX_IA_ERROR },
  /* C4 /m */ { 0, BX_IA_ERROR },
  /* C5 /r */ { 0, BX_IA_ERROR },
  /* C5 /m */ { 0, BX_IA_ERROR },
  /* C6 /r */ { 0, BX_IA_ERROR },
  /* C6 /m */ { 0, BX_IA_ERROR },
  /* C7 /r */ { 0, BX_IA_ERROR },
  /* C7 /m */ { 0, BX_IA_ERROR },
  /* C8 /r */ { 0, BX_IA_ERROR },
  /* C8 /m */ { 0, BX_IA_ERROR },
  /* C9 /r */ { 0, BX_IA_ERROR },
  /* C9 /m */ { 0, BX_IA_ERROR },
  /* CA /r */ { 0, BX_IA_ERROR },
  /* CA /m */ { 0, BX_IA_ERROR },
  /* CB /r */ { 0, BX_IA_ERROR },
  /* CB /m */ { 0, BX_IA_ERROR },
  /* CC /r */ { 0, BX_IA_ERROR },
  /* CC /m */ { 0, BX_IA_ERROR },
  /* CD /r */ { 0, BX_IA_ERROR },
  /* CD /m */ { 0, BX_IA_ERROR },
  /* CE /r */ { 0, BX_IA_ERROR },
  /* CE /m */ { 0, BX_IA_ERROR },
  /* CF /r */ { 0, BX_IA_ERROR },
  /* CF /m */ { 0, BX_IA_ERROR },
  /* D0 /r */ { 0, BX_IA_ERROR },
  /* D0 /m */ { 0, BX_IA_ERROR },
  /* D1 /r */ { 0, BX_IA_ERROR },
  /* D1 /m */ { 0, BX_IA_ERROR },
  /* D2 /r */ { 0, BX_IA_ERROR },
  /* D2 /m */ { 0, BX_IA_ERROR },
  /* D3 /r */ { 0, BX_IA_ERROR },
  /* D3 /m */ { 0, BX_IA_ERROR },
  /* D4 /r */ { 0, BX_IA_ERROR },
  /* D4 /m */ { 0, BX_IA_ERROR },
  /* D5 /r */ { 0, BX_IA_ERROR },
  /* D5 /m */ { 0, BX_IA_ERROR },
  /* D6 /r */ { 0, BX_IA_ERROR },
  /* D6 /m */ { 0, BX_IA_ERROR },
  /* D7 /r */ { 0, BX_IA_ERROR },
  /* D7 /m */ { 0, BX_IA_ERROR },
  /* D8 /r */ { 0, BX_IA_ERROR },
  /* D8 /m */ { 0, BX_IA_ERROR },
  /* D9 /r */ { 0, BX_IA_ERROR },
  /* D9 /m */ { 0, BX_IA_ERROR },
  /* DA /r */ { 0, BX_IA_ERROR },
  /* DA /m */ { 0, BX_IA_ERROR },
  /* DB /r */ { 0, BX_IA_ERROR },
  /* DB /m */ { 0, BX_IA_ERROR },
  /* DC /r */ { 0, BX_IA_ERROR },
  /* DC /m */ { 0, BX_IA_ERROR },
  /* DD /r */ { 0, BX_IA_ERROR },
  /* DD /m */ { 0, BX_IA_ERROR },
  /* DE /r */ { 0, BX_IA_ERROR },
  /* DE /m */ { 0, BX_IA_ERROR },
  /* DF /r */ { BxPrefixSSE66, BX_IA_AESKEYGENASSIST_VdqWdqIbR }, // AES
  /* DF /m */ { BxPrefixSSE66, BX_IA_AESKEYGENASSIST_VdqWdqIbM }, // AES
  /* E0 /r */ { 0, BX_IA_ERROR },
  /* E0 /m */ { 0, BX_IA_ERROR },
  /* E1 /r */ { 0, BX_IA_ERROR },
  /* E1 /m */ { 0, BX_IA_ERROR },
  /* E2 /r */ { 0, BX_IA_ERROR },
  /* E2 /m */ { 0, BX_IA_ERROR },
  /* E3 /r */ { 0, BX_IA_ERROR },
  /* E3 /m */ { 0, BX_IA_ERROR },
  /* E4 /r */ { 0, BX_IA_ERROR },
  /* E4 /m */ { 0, BX_IA_ERROR },
  /* E5 /r */ { 0, BX_IA_ERROR },
  /* E5 /m */ { 0, BX_IA_ERROR },
  /* E6 /r */ { 0, BX_IA_ERROR },
  /* E6 /m */ { 0, BX_IA_ERROR },
  /* E7 /r */ { 0, BX_IA_ERROR },
  /* E7 /m */ { 0, BX_IA_ERROR },
  /* E8 /r */ { 0, BX_IA_ERROR },
  /* E8 /m */ { 0, BX_IA_ERROR },
  /* E9 /r */ { 0, BX_IA_ERROR },
  /* E9 /m */ { 0, BX_IA_ERROR },
  /* EA /r */ { 0, BX_IA_ERROR },
  /* EA /m */ { 0, BX_IA_ERROR },
  /* EB /r */ { 0, BX_IA_ERROR },
  /* EB /m */ { 0, BX_IA_ERROR },
  /* EC /r */ { 0, BX_IA_ERROR },
  /* EC /m */ { 0, BX_IA_ERROR },
  /* ED /r */ { 0, BX_IA_ERROR },
  /* ED /m */ { 0, BX_IA_ERROR },
  /* EE /r */ { 0, BX_IA_ERROR },
  /* EE /m */ { 0, BX_IA_ERROR },
  /* EF /r */ { 0, BX_IA_ERROR },
  /* EF /m */ { 0, BX_IA_ERROR },
  /* F0 /r */ { 0, BX_IA_ERROR },
  /* F0 /m */ { 0, BX_IA_ERROR },
  /* F1 /r */ { 0, BX_IA_ERROR },
  /* F1 /m */ { 0, BX_IA_ERROR },
  /* F2 /r */ { 0, BX_IA_ERROR },
  /* F2 /m */ { 0, BX_IA_ERROR },
  /* F3 /r */ { 0, BX_IA_ERROR },
  /* F3 /m */ { 0, BX_IA_ERROR },
  /* F4 /r */ { 0, BX_IA_ERROR },
  /* F4 /m */ { 0, BX_IA_ERROR },
  /* F5 /r */ { 0, BX_IA_ERROR },
  /* F5 /m */ { 0, BX_IA_ERROR },
  /* F6 /r */ { 0, BX_IA_ERROR },
  /* F6 /m */ { 0, BX_IA_ERROR },
  /* F7 /r */ { 0, BX_IA_ERROR },
  /* F7 /m */ { 0, BX_IA_ERROR },
  /* F8 /r */ { 0, BX_IA_ERROR },
  /* F8 /m */ { 0, BX_IA_ERROR },
  /* F9 /r */ { 0, BX_IA_ERROR },
  /* F9 /m */ { 0, BX_IA_ERROR },
  /* FA /r */ { 0, BX_IA_ERROR },
  /* FA /m */ { 0, BX_IA_ERROR },
  /* FB /r */ { 0, BX_IA_ERROR },
  /* FB /m */ { 0, BX_IA_ERROR },
  /* FC /r */ { 0, BX_IA_ERROR },
  /* FC /m */ { 0, BX_IA_ERROR },
  /* FD /r */ { 0, BX_IA_ERROR },
  /* FD /m */ { 0, BX_IA_ERROR },
  /* FE /r */ { 0, BX_IA_ERROR },
  /* FE /m */ { 0, BX_IA_ERROR },
  /* FF /r */ { 0, BX_IA_ERROR },
  /* FF /m */ { 0, BX_IA_ERROR }
};

#endif // BX_SSE_FETCHDECODE_TABLES_H
