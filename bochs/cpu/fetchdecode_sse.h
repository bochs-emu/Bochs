/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2013 Stanislav Shwartsman
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
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_PAUSE[3] = {
  /* 66 */ { 0, BX_IA_NOP },
  /* F3 */ { 0, BX_IA_PAUSE },
  /* F2 */ { 0, BX_IA_NOP }
};

static const BxOpcodeInfo_t BxOpcodeInfoG17A[8] = {
  /* 0 */ { BxImmediate_Ib | BxImmediate_Ib2, BX_IA_EXTRQ_UdqIbIb },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE4A_0f78[3] = {
  /* 66 */ { BxGroup17A, BX_IA_ERROR, BxOpcodeInfoG17A },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { BxImmediate_Ib | BxImmediate_Ib2, BX_IA_INSERTQ_VdqUqIbIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE4A_0f79[3] = {
  /* 66 */ { 0, BX_IA_EXTRQ_VdqUq },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_INSERTQ_VdqUdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_TZCNT16[3] = {
  /* 66 */ { 0, BX_IA_BSF_GwEw },
  /* F3 */ { 0, BX_IA_TZCNT_GwEw },
  /* F2 */ { 0, BX_IA_BSF_GwEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_TZCNT32[3] = {
  /* 66 */ { 0, BX_IA_BSF_GdEd },
  /* F3 */ { 0, BX_IA_TZCNT_GdEd },
  /* F2 */ { 0, BX_IA_BSF_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_TZCNT64[3] = {
  /* 66 */ { 0, BX_IA_BSF_GqEq },
  /* F3 */ { 0, BX_IA_TZCNT_GqEq },
  /* F2 */ { 0, BX_IA_BSF_GqEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_LZCNT16[3] = {
  /* 66 */ { 0, BX_IA_BSR_GwEw },
  /* F3 */ { 0, BX_IA_LZCNT_GwEw },
  /* F2 */ { 0, BX_IA_BSR_GwEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_LZCNT32[3] = {
  /* 66 */ { 0, BX_IA_BSR_GdEd },
  /* F3 */ { 0, BX_IA_LZCNT_GdEd },
  /* F2 */ { 0, BX_IA_BSR_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_LZCNT64[3] = {
  /* 66 */ { 0, BX_IA_BSR_GqEq },
  /* F3 */ { 0, BX_IA_LZCNT_GqEq },
  /* F2 */ { 0, BX_IA_BSR_GqEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpd },
  /* F3 */ { 0, BX_IA_MOVSS_VssWss },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[3] = {
  /* 66 */ { 0, BX_IA_MOVUPD_WpdVpd },
  /* F3 */ { 0, BX_IA_MOVSS_WssVss },
  /* F2 */ { 0, BX_IA_MOVSD_WsdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[3] = {
  /* 66 */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSLDUP_VpsWps },
  /* F2 */ { 0, BX_IA_MOVDDUP_VpdWq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[2] = {
  /* -- */ { 0, BX_IA_MOVLPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVLPD_MqVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[2] = {
  /* -- */ { 0, BX_IA_UNPCKLPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKLPD_VpdWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[2] = {
  /* -- */ { 0, BX_IA_UNPCKHPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKHPD_VpdWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[3] = {
  /* 66 */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F3 */ { 0, BX_IA_MOVSHDUP_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[2] = {
  /* -- */ { 0, BX_IA_MOVHPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVHPD_MqVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[2] = {
  /* -- */ { 0, BX_IA_MOVAPS_VpsWps },
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[2] = {
  /* -- */ { 0, BX_IA_MOVAPS_WpsVps },
  /* 66 */ { 0, BX_IA_MOVAPD_WpdVpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[3] = {
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEd },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2aQ[3] = {
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEq },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[3] = {
  /* 66 */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F3 */ { 0, BX_IA_MOVNTSS_MssVss },
  /* F2 */ { 0, BX_IA_MOVNTSD_MsdVsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[3] = {
  /* 66 */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTTSS2SI_GdWss },
  /* F2 */ { 0, BX_IA_CVTTSD2SI_GdWsd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2cQ[3] = {
  /* 66 */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTTSS2SI_GqWss },
  /* F2 */ { 0, BX_IA_CVTTSD2SI_GqWsd }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[3] = {
  /* 66 */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTSS2SI_GdWss },
  /* F2 */ { 0, BX_IA_CVTSD2SI_GdWsd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2dQ[3] = {
  /* 66 */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F3 */ { 0, BX_IA_CVTSS2SI_GqWss },
  /* F2 */ { 0, BX_IA_CVTSD2SI_GqWsd }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[2] = {
  /* -- */ { 0, BX_IA_UCOMISS_VssWss },
  /* 66 */ { 0, BX_IA_UCOMISD_VsdWsd }            	
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[2] = {
  /* -- */ { 0, BX_IA_COMISS_VssWss },
  /* 66 */ { 0, BX_IA_COMISD_VsdWsd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50R[2] = {
  /* -- */ { 0, BX_IA_MOVMSKPS_GdUps },
  /* 66 */ { 0, BX_IA_MOVMSKPD_GdUpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RSQRTSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RCPSS_VssWss },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[2] = {
  /* -- */ { 0, BX_IA_ANDPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[2] = {
  /* -- */ { 0, BX_IA_ANDNPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDNPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[2] = {
  /* -- */ { 0, BX_IA_ORPS_VpsWps },
  /* 66 */ { 0, BX_IA_ORPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[2] = {
  /* -- */ { 0, BX_IA_XORPS_VpsWps },
  /* 66 */ { 0, BX_IA_XORPD_VpdWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[3] = {
  /* 66 */ { 0, BX_IA_CVTPS2DQ_VdqWps },
  /* F3 */ { 0, BX_IA_CVTTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[2] = {
  /* -- */ { 0, BX_IA_PUNPCKLBW_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLBW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[2] = {
  /* -- */ { 0, BX_IA_PUNPCKLWD_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLWD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[2] = {
  /* -- */ { 0, BX_IA_PUNPCKLDQ_PqQd },
  /* 66 */ { 0, BX_IA_PUNPCKLDQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[2] = {
  /* -- */ { 0, BX_IA_PACKSSWB_PqQq },
  /* 66 */ { 0, BX_IA_PACKSSWB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[2] = {
  /* -- */ { 0, BX_IA_PCMPGTB_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[2] = {
  /* -- */ { 0, BX_IA_PCMPGTW_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[2] = {
  /* -- */ { 0, BX_IA_PCMPGTD_PqQq },
  /* 66 */ { 0, BX_IA_PCMPGTD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[2] = {
  /* -- */ { 0, BX_IA_PACKUSWB_PqQq },
  /* 66 */ { 0, BX_IA_PACKUSWB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[2] = {
  /* -- */ { 0, BX_IA_PUNPCKHBW_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHBW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[2] = {
  /* -- */ { 0, BX_IA_PUNPCKHWD_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHWD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[2] = {
  /* -- */ { 0, BX_IA_PUNPCKHDQ_PqQq },
  /* 66 */ { 0, BX_IA_PUNPCKHDQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[2] = {
  /* -- */ { 0, BX_IA_PACKSSDW_PqQq },
  /* 66 */ { 0, BX_IA_PACKSSDW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[2] = {
  /* -- */ { 0, BX_IA_MOVD_PqEd },
  /* 66 */ { 0, BX_IA_MOVD_VdqEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQ[2] = {
  /* -- */ { 0, BX_IA_MOVQ_PqEq },
  /* 66 */ { 0, BX_IA_MOVQ_VdqEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdq },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[3] = {
  /* 66 */ { 0, BX_IA_PSHUFD_VdqWdqIb },
  /* F3 */ { 0, BX_IA_PSHUFHW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_PSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[2] = {
  /* -- */ { 0, BX_IA_PCMPEQB_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[2] = {
  /* -- */ { 0, BX_IA_PCMPEQW_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[2] = {
  /* -- */ { 0, BX_IA_PCMPEQD_PqQq },
  /* 66 */ { 0, BX_IA_PCMPEQD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[3] = {
  /* 66 */ { 0, BX_IA_HADDPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HADDPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[3] = {
  /* 66 */ { 0, BX_IA_HSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_HSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[3] = {
  /* 66 */ { 0, BX_IA_MOVD_EdVd },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQ[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_EqVq },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq },
  /* F2 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[3] = {
  /* 66 */ { 0, BX_IA_MOVDQA_WdqVdq },
  /* F3 */ { 0, BX_IA_MOVDQU_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[2] = {
  /* -- */ { 0, BX_IA_PINSRW_PqEwIb },
  /* 66 */ { 0, BX_IA_PINSRW_VdqEwIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5R[3] = {
  /* -- */ { 0, BX_IA_PEXTRW_GdNqIb },
  /* 66 */ { 0, BX_IA_PEXTRW_GdUdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[2] = {
  /* -- */ { 0, BX_IA_SHUFPS_VpsWpsIb },
  /* 66 */ { 0, BX_IA_SHUFPD_VpdWpdIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[3] = {
  /* 66 */ { 0, BX_IA_ADDSUBPD_VpdWpd },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ADDSUBPS_VpsWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[2] = {
  /* -- */ { 0, BX_IA_PSRLW_PqQq },
  /* 66 */ { 0, BX_IA_PSRLW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[2] = {
  /* -- */ { 0, BX_IA_PSRLD_PqQq },
  /* 66 */ { 0, BX_IA_PSRLD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[2] = {
  /* -- */ { 0, BX_IA_PSRLQ_PqQq },
  /* 66 */ { 0, BX_IA_PSRLQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[2] = {
  /* -- */ { 0, BX_IA_PADDQ_PqQq },
  /* 66 */ { 0, BX_IA_PADDQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[2] = {
  /* -- */ { 0, BX_IA_PMULLW_PqQq },
  /* 66 */ { 0, BX_IA_PMULLW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[3] = {
  /* 66 */ { 0, BX_IA_MOVQ_WqVq },
  /* F3 */ { 0, BX_IA_MOVQ2DQ_VdqQq },
  /* F2 */ { 0, BX_IA_MOVDQ2Q_PqUdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7R[2] = {
  /* -- */ { 0, BX_IA_PMOVMSKB_GdNq },
  /* 66 */ { 0, BX_IA_PMOVMSKB_GdUdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[2] = {
  /* -- */ { 0, BX_IA_PSUBUSB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBUSB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[2] = {
  /* -- */ { 0, BX_IA_PSUBUSW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBUSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[2] = {
  /* -- */ { 0, BX_IA_PMINUB_PqQq },
  /* 66 */ { 0, BX_IA_PMINUB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[2] = {
  /* -- */ { 0, BX_IA_PAND_PqQq },
  /* 66 */ { 0, BX_IA_PAND_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[2] = {
  /* -- */ { 0, BX_IA_PADDUSB_PqQq },
  /* 66 */ { 0, BX_IA_PADDUSB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[2] = {
  /* -- */ { 0, BX_IA_PADDUSW_PqQq },
  /* 66 */ { 0, BX_IA_PADDUSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[2] = {
  /* -- */ { 0, BX_IA_PMAXUB_PqQq },
  /* 66 */ { 0, BX_IA_PMAXUB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[2] = {
  /* -- */ { 0, BX_IA_PANDN_PqQq },
  /* 66 */ { 0, BX_IA_PANDN_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[2] = {
  /* -- */ { 0, BX_IA_PAVGB_PqQq },
  /* 66 */ { 0, BX_IA_PAVGB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[2] = {
  /* -- */ { 0, BX_IA_PSRAW_PqQq },
  /* 66 */ { 0, BX_IA_PSRAW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[2] = {
  /* -- */ { 0, BX_IA_PSRAD_PqQq },
  /* 66 */ { 0, BX_IA_PSRAD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[2] = {
  /* -- */ { 0, BX_IA_PAVGW_PqQq },
  /* 66 */ { 0, BX_IA_PAVGW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[2] = {
  /* -- */ { 0, BX_IA_PMULHUW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHUW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[2] = {
  /* -- */ { 0, BX_IA_PMULHW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[3] = {
  /* 66 */ { 0, BX_IA_CVTTPD2DQ_VqWpd },
  /* F3 */ { 0, BX_IA_CVTDQ2PD_VpdWq },
  /* F2 */ { 0, BX_IA_CVTPD2DQ_VqWpd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[2] = {
  /* -- */ { 0, BX_IA_MOVNTQ_MqPq },
  /* 66 */ { 0, BX_IA_MOVNTDQ_MdqVdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[2] = {
  /* -- */ { 0, BX_IA_PSUBSB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBSB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[2] = {
  /* -- */ { 0, BX_IA_PSUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[2] = {
  /* -- */ { 0, BX_IA_PMINSW_PqQq },
  /* 66 */ { 0, BX_IA_PMINSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[2] = {
  /* -- */ { 0, BX_IA_POR_PqQq },
  /* 66 */ { 0, BX_IA_POR_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[2] = {
  /* -- */ { 0, BX_IA_PADDSB_PqQq },
  /* 66 */ { 0, BX_IA_PADDSB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[2] = {
  /* -- */ { 0, BX_IA_PADDSW_PqQq },
  /* 66 */ { 0, BX_IA_PADDSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[2] = {
  /* -- */ { 0, BX_IA_PMAXSW_PqQq },
  /* 66 */ { 0, BX_IA_PMAXSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[2] = {
  /* -- */ { 0, BX_IA_PXOR_PqQq },
  /* 66 */ { 0, BX_IA_PXOR_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[2] = {
  /* -- */ { 0, BX_IA_PSLLW_PqQq },
  /* 66 */ { 0, BX_IA_PSLLW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[2] = {
  /* -- */ { 0, BX_IA_PSLLD_PqQq },
  /* 66 */ { 0, BX_IA_PSLLD_VdqWdq },
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[2] = {
  /* -- */ { 0, BX_IA_PSLLQ_PqQq },
  /* 66 */ { 0, BX_IA_PSLLQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[2] = {
  /* -- */ { 0, BX_IA_PMULUDQ_PqQq },
  /* 66 */ { 0, BX_IA_PMULUDQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[2] = {
  /* -- */ { 0, BX_IA_PMADDWD_PqQq },
  /* 66 */ { 0, BX_IA_PMADDWD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[2] = {
  /* -- */ { 0, BX_IA_PSADBW_PqQq },
  /* 66 */ { 0, BX_IA_PSADBW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7R[2] = {
  /* -- */ { 0, BX_IA_MASKMOVQ_PqNq },
  /* 66 */ { 0, BX_IA_MASKMOVDQU_VdqUdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[2] = {
  /* -- */ { 0, BX_IA_PSUBB_PqQq },
  /* 66 */ { 0, BX_IA_PSUBB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[2] = {
  /* -- */ { 0, BX_IA_PSUBW_PqQq },
  /* 66 */ { 0, BX_IA_PSUBW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[2] = {
  /* -- */ { 0, BX_IA_PSUBD_PqQq },
  /* 66 */ { 0, BX_IA_PSUBD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[2] = {
  /* -- */ { 0, BX_IA_PSUBQ_PqQq },
  /* 66 */ { 0, BX_IA_PSUBQ_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[2] = {
  /* -- */ { 0, BX_IA_PADDB_PqQq },
  /* 66 */ { 0, BX_IA_PADDB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[2] = {
  /* -- */ { 0, BX_IA_PADDW_PqQq },
  /* 66 */ { 0, BX_IA_PADDW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[2] = {
  /* -- */ { 0, BX_IA_PADDD_PqQq },
  /* 66 */ { 0, BX_IA_PADDD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[2] = {
  /* -- */ { 0, BX_IA_PSRLW_NqIb },
  /* 66 */ { 0, BX_IA_PSRLW_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[2] = {
  /* -- */ { 0, BX_IA_PSRAW_NqIb },
  /* 66 */ { 0, BX_IA_PSRAW_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[2] = {
  /* -- */ { 0, BX_IA_PSLLW_NqIb },
  /* 66 */ { 0, BX_IA_PSLLW_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[2] = {
  /* -- */ { 0, BX_IA_PSRLD_NqIb },
  /* 66 */ { 0, BX_IA_PSRLD_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[2] = {
  /* -- */ { 0, BX_IA_PSRAD_NqIb },
  /* 66 */ { 0, BX_IA_PSRAD_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[2] = {
  /* -- */ { 0, BX_IA_PSLLD_NqIb },
  /* 66 */ { 0, BX_IA_PSLLD_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[2] = {
  /* -- */ { 0, BX_IA_PSRLQ_NqIb },
  /* 66 */ { 0, BX_IA_PSRLQ_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[2] = {
  /* -- */ { 0, BX_IA_PSLLQ_NqIb },
  /* 66 */ { 0, BX_IA_PSLLQ_UdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G9VMX6[3] = {
  /* 66 */ { 0, BX_IA_VMCLEAR_Mq },
  /* F3 */ { 0, BX_IA_VMXON_Mq },
  /* F2 */ { 0, BX_IA_ERROR }
};

#if BX_CPU_LEVEL >= 6

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[2] = {
  /* -- */ { 0, BX_IA_PSHUFB_PqQq },
  /* 66 */ { 0, BX_IA_PSHUFB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[2] = {
  /* -- */ { 0, BX_IA_PHADDW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[2] = {
  /* -- */ { 0, BX_IA_PHADDD_PqQq },
  /* 66 */ { 0, BX_IA_PHADDD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[2] = {
  /* -- */ { 0, BX_IA_PHADDSW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[2] = {
  /* -- */ { 0, BX_IA_PMADDUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PMADDUBSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[2] = {
  /* -- */ { 0, BX_IA_PHSUBW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[2] = {
  /* -- */ { 0, BX_IA_PHSUBD_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[2] = {
  /* -- */ { 0, BX_IA_PHSUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[2] = {
  /* -- */ { 0, BX_IA_PSIGNB_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[2] = {
  /* -- */ { 0, BX_IA_PSIGNW_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[2] = {
  /* -- */ { 0, BX_IA_PSIGND_PqQq },
  /* 66 */ { 0, BX_IA_PSIGND_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[2] = {
  /* -- */ { 0, BX_IA_PMULHRSW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHRSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[2] = {
  /* -- */ { 0, BX_IA_PABSB_PqQq },
  /* 66 */ { 0, BX_IA_PABSB_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[2] = {
  /* -- */ { 0, BX_IA_PABSW_PqQq },
  /* 66 */ { 0, BX_IA_PABSW_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[2] = {
  /* -- */ { 0, BX_IA_PABSD_PqQq },
  /* 66 */ { 0, BX_IA_PABSD_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEb }
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f0[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_GwMw, BxOpcodeGroupSSE_0f38f0 },
  /* 32 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_GdMd, BxOpcodeGroupSSE_0f38f0 },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_GqMq, BxOpcodeGroupSSE_0f38f0 },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1w[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1d[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1q[3] = {
  /* 66 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupOsize0f38f1[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_MwGw, BxOpcodeGroupSSE_0f38f1w },
  /* 32 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_MdGd, BxOpcodeGroupSSE_0f38f1d },
#if BX_SUPPORT_X86_64
  /* 64 */ { BxPrefixSSEF2F3, BX_IA_MOVBE_MqGq, BxOpcodeGroupSSE_0f38f1q },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize_660f38f6[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { 0, BX_IA_ADCX_GdEd },
  /* 32 */ { 0, BX_IA_ADCX_GdEd },
#if BX_SUPPORT_X86_64
  /* 64 */ { 0, BX_IA_ADCX_GqEq },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupOsize_f20f38f6[BX_SUPPORT_X86_64 + 2] = {
  /* 16 */ { 0, BX_IA_ADOX_GdEd },
  /* 32 */ { 0, BX_IA_ADOX_GdEd },
#if BX_SUPPORT_X86_64
  /* 64 */ { 0, BX_IA_ADOX_GqEq },
#endif
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f6[3] = {
  /* 66 */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize_660f38f6 },
  /* F2 */ { BxOSizeGrp, BX_IA_ERROR, BxOpcodeGroupOsize_f20f38f6 },
  /* F3 */ { 0, BX_IA_ERROR },
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0f[2] = {
  /* -- */ { 0, BX_IA_PALIGNR_PqQqIb },
  /* 66 */ { 0, BX_IA_PALIGNR_VdqWdqIb }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f38[256] = {
  /* 00 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3800 },
  /* 01 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3801 },
  /* 02 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3802 },
  /* 03 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3803 },
  /* 04 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3804 },
  /* 05 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3805 },
  /* 06 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3806 },
  /* 07 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3807 },
  /* 08 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3808 },
  /* 09 */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3809 },
  /* 0A */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f380a },
  /* 0B */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f380b },
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
  /* 1C */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f381c },
  /* 1D */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f381d },
  /* 1E */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f381e },
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
  /* 80 */ { BxPrefixSSE66, BX_IA_INVEPT },
  /* 81 */ { BxPrefixSSE66, BX_IA_INVVPID },
  /* 82 */ { BxPrefixSSE66, BX_IA_INVPCID },
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
  /* C8 */ { BxPrefixSSE, BX_IA_SHA1NEXTE_VdqWdq, BxOpcodeGroupSSE_ERR },
  /* C9 */ { BxPrefixSSE, BX_IA_SHA1MSG1_VdqWdq, BxOpcodeGroupSSE_ERR },
  /* CA */ { BxPrefixSSE, BX_IA_SHA1MSG2_VdqWdq, BxOpcodeGroupSSE_ERR },
  /* CB */ { BxPrefixSSE, BX_IA_SHA256RNDS2_VdqWdq, BxOpcodeGroupSSE_ERR },
  /* CC */ { BxPrefixSSE, BX_IA_SHA256MSG1_VdqWdq, BxOpcodeGroupSSE_ERR },
  /* CD */ { BxPrefixSSE, BX_IA_SHA256MSG2_VdqWdq, BxOpcodeGroupSSE_ERR },
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
  /* F6 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f38f6 },
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
  /* 0F */ { BxPrefixSSE2, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0f },
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
  /* CC */ { BxPrefixSSE, BX_IA_SHA1RNDS4_VdqWdqIb, BxOpcodeGroupSSE_ERR },
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

#endif // BX_CPU_LEVEL >= 6

#endif // BX_SSE_FETCHDECODE_TABLES_H
