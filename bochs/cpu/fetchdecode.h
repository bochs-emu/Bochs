/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate@fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA



#ifndef BX_COMMON_FETCHDECODE_TABLES_H
#define BX_COMMON_FETCHDECODE_TABLES_H

//
// Common FetchDecode Opcode Tables
//

BX_CPP_INLINE Bit16u FetchWORD(Bit8u *iptr)
{
   Bit16u data;
   ReadHostWordFromLittleEndian(iptr, data);
   return data;
}

BX_CPP_INLINE Bit32u FetchDWORD(Bit8u *iptr)
{
   Bit32u data;
   ReadHostDWordFromLittleEndian(iptr, data);
   return data;
}

BX_CPP_INLINE Bit64u FetchQWORD(Bit8u *iptr)
{
   Bit64u data;
   ReadHostDWordFromLittleEndian(iptr, data);
   return data;
}

//
// This file contains common IA-32/IA-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

/* ************************************************************************ */
/* 3DNow! Opcodes */

#if BX_SUPPORT_3DNOW

static BxOpcodeInfo_t Bx3DNowOpcodeInfo[256] = {
  // 256 entries for 3DNow opcodes, by suffix
  /* 00 */  { 0, &BX_CPU_C::BxError },
  /* 01 */  { 0, &BX_CPU_C::BxError },
  /* 02 */  { 0, &BX_CPU_C::BxError },
  /* 03 */  { 0, &BX_CPU_C::BxError },
  /* 04 */  { 0, &BX_CPU_C::BxError },
  /* 05 */  { 0, &BX_CPU_C::BxError },
  /* 06 */  { 0, &BX_CPU_C::BxError },
  /* 07 */  { 0, &BX_CPU_C::BxError },
  /* 08 */  { 0, &BX_CPU_C::BxError },
  /* 09 */  { 0, &BX_CPU_C::BxError },
  /* 0A */  { 0, &BX_CPU_C::BxError },
  /* 0B */  { 0, &BX_CPU_C::BxError },
  /* 0C */  { 0, &BX_CPU_C::PI2FW_PqQq },
  /* 0D */  { 0, &BX_CPU_C::PI2FD_PqQq },
  /* 0E */  { 0, &BX_CPU_C::BxError },
  /* 0F */  { 0, &BX_CPU_C::BxError },
  /* 10 */  { 0, &BX_CPU_C::BxError },
  /* 11 */  { 0, &BX_CPU_C::BxError },
  /* 12 */  { 0, &BX_CPU_C::BxError },
  /* 13 */  { 0, &BX_CPU_C::BxError },
  /* 14 */  { 0, &BX_CPU_C::BxError },
  /* 15 */  { 0, &BX_CPU_C::BxError },
  /* 16 */  { 0, &BX_CPU_C::BxError },
  /* 17 */  { 0, &BX_CPU_C::BxError },
  /* 18 */  { 0, &BX_CPU_C::BxError },
  /* 19 */  { 0, &BX_CPU_C::BxError },
  /* 1A */  { 0, &BX_CPU_C::BxError },
  /* 1B */  { 0, &BX_CPU_C::BxError },
  /* 1C */  { 0, &BX_CPU_C::PF2IW_PqQq },
  /* 1D */  { 0, &BX_CPU_C::PF2ID_PqQq },
  /* 1E */  { 0, &BX_CPU_C::BxError },
  /* 1F */  { 0, &BX_CPU_C::BxError },
  /* 20 */  { 0, &BX_CPU_C::BxError },
  /* 21 */  { 0, &BX_CPU_C::BxError },
  /* 22 */  { 0, &BX_CPU_C::BxError },
  /* 23 */  { 0, &BX_CPU_C::BxError },
  /* 24 */  { 0, &BX_CPU_C::BxError },
  /* 25 */  { 0, &BX_CPU_C::BxError },
  /* 26 */  { 0, &BX_CPU_C::BxError },
  /* 27 */  { 0, &BX_CPU_C::BxError },
  /* 28 */  { 0, &BX_CPU_C::BxError },
  /* 29 */  { 0, &BX_CPU_C::BxError },
  /* 2A */  { 0, &BX_CPU_C::BxError },
  /* 2B */  { 0, &BX_CPU_C::BxError },
  /* 2C */  { 0, &BX_CPU_C::BxError },
  /* 2D */  { 0, &BX_CPU_C::BxError },
  /* 2E */  { 0, &BX_CPU_C::BxError },
  /* 2F */  { 0, &BX_CPU_C::BxError },
  /* 30 */  { 0, &BX_CPU_C::BxError },
  /* 31 */  { 0, &BX_CPU_C::BxError },
  /* 32 */  { 0, &BX_CPU_C::BxError },
  /* 33 */  { 0, &BX_CPU_C::BxError },
  /* 34 */  { 0, &BX_CPU_C::BxError },
  /* 35 */  { 0, &BX_CPU_C::BxError },
  /* 36 */  { 0, &BX_CPU_C::BxError },
  /* 37 */  { 0, &BX_CPU_C::BxError },
  /* 38 */  { 0, &BX_CPU_C::BxError },
  /* 39 */  { 0, &BX_CPU_C::BxError },
  /* 3A */  { 0, &BX_CPU_C::BxError },
  /* 3B */  { 0, &BX_CPU_C::BxError },
  /* 3C */  { 0, &BX_CPU_C::BxError },
  /* 3D */  { 0, &BX_CPU_C::BxError },
  /* 3E */  { 0, &BX_CPU_C::BxError },
  /* 3F */  { 0, &BX_CPU_C::BxError },
  /* 40 */  { 0, &BX_CPU_C::BxError },
  /* 41 */  { 0, &BX_CPU_C::BxError },
  /* 42 */  { 0, &BX_CPU_C::BxError },
  /* 43 */  { 0, &BX_CPU_C::BxError },
  /* 44 */  { 0, &BX_CPU_C::BxError },
  /* 45 */  { 0, &BX_CPU_C::BxError },
  /* 46 */  { 0, &BX_CPU_C::BxError },
  /* 47 */  { 0, &BX_CPU_C::BxError },
  /* 48 */  { 0, &BX_CPU_C::BxError },
  /* 49 */  { 0, &BX_CPU_C::BxError },
  /* 4A */  { 0, &BX_CPU_C::BxError },
  /* 4B */  { 0, &BX_CPU_C::BxError },
  /* 4C */  { 0, &BX_CPU_C::BxError },
  /* 4D */  { 0, &BX_CPU_C::BxError },
  /* 4E */  { 0, &BX_CPU_C::BxError },
  /* 4F */  { 0, &BX_CPU_C::BxError },
  /* 50 */  { 0, &BX_CPU_C::BxError },
  /* 51 */  { 0, &BX_CPU_C::BxError },
  /* 52 */  { 0, &BX_CPU_C::BxError },
  /* 53 */  { 0, &BX_CPU_C::BxError },
  /* 54 */  { 0, &BX_CPU_C::BxError },
  /* 55 */  { 0, &BX_CPU_C::BxError },
  /* 56 */  { 0, &BX_CPU_C::BxError },
  /* 57 */  { 0, &BX_CPU_C::BxError },
  /* 58 */  { 0, &BX_CPU_C::BxError },
  /* 59 */  { 0, &BX_CPU_C::BxError },
  /* 5A */  { 0, &BX_CPU_C::BxError },
  /* 5B */  { 0, &BX_CPU_C::BxError },
  /* 5C */  { 0, &BX_CPU_C::BxError },
  /* 5D */  { 0, &BX_CPU_C::BxError },
  /* 5E */  { 0, &BX_CPU_C::BxError },
  /* 5F */  { 0, &BX_CPU_C::BxError },
  /* 60 */  { 0, &BX_CPU_C::BxError },
  /* 61 */  { 0, &BX_CPU_C::BxError },
  /* 62 */  { 0, &BX_CPU_C::BxError },
  /* 63 */  { 0, &BX_CPU_C::BxError },
  /* 64 */  { 0, &BX_CPU_C::BxError },
  /* 65 */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::BxError },
  /* 67 */  { 0, &BX_CPU_C::BxError },
  /* 68 */  { 0, &BX_CPU_C::BxError },
  /* 69 */  { 0, &BX_CPU_C::BxError },
  /* 6A */  { 0, &BX_CPU_C::BxError },
  /* 6B */  { 0, &BX_CPU_C::BxError },
  /* 6C */  { 0, &BX_CPU_C::BxError },
  /* 6D */  { 0, &BX_CPU_C::BxError },
  /* 6E */  { 0, &BX_CPU_C::BxError },
  /* 6F */  { 0, &BX_CPU_C::BxError },
  /* 70 */  { 0, &BX_CPU_C::BxError },
  /* 71 */  { 0, &BX_CPU_C::BxError },
  /* 72 */  { 0, &BX_CPU_C::BxError },
  /* 73 */  { 0, &BX_CPU_C::BxError },
  /* 74 */  { 0, &BX_CPU_C::BxError },
  /* 75 */  { 0, &BX_CPU_C::BxError },
  /* 76 */  { 0, &BX_CPU_C::BxError },
  /* 77 */  { 0, &BX_CPU_C::BxError },
  /* 78 */  { 0, &BX_CPU_C::BxError },
  /* 79 */  { 0, &BX_CPU_C::BxError },
  /* 7A */  { 0, &BX_CPU_C::BxError },
  /* 7B */  { 0, &BX_CPU_C::BxError },
  /* 7C */  { 0, &BX_CPU_C::BxError },
  /* 7D */  { 0, &BX_CPU_C::BxError },
  /* 7E */  { 0, &BX_CPU_C::BxError },
  /* 7F */  { 0, &BX_CPU_C::BxError },
  /* 80 */  { 0, &BX_CPU_C::BxError },
  /* 81 */  { 0, &BX_CPU_C::BxError },
  /* 82 */  { 0, &BX_CPU_C::BxError },
  /* 83 */  { 0, &BX_CPU_C::BxError },
  /* 84 */  { 0, &BX_CPU_C::BxError },
  /* 85 */  { 0, &BX_CPU_C::BxError },
  /* 86 */  { 0, &BX_CPU_C::BxError },
  /* 87 */  { 0, &BX_CPU_C::BxError },
  /* 88 */  { 0, &BX_CPU_C::BxError },
  /* 89 */  { 0, &BX_CPU_C::BxError },
  /* 8A */  { 0, &BX_CPU_C::PFNACC_PqQq },
  /* 8B */  { 0, &BX_CPU_C::BxError },
  /* 8C */  { 0, &BX_CPU_C::BxError },
  /* 8D */  { 0, &BX_CPU_C::BxError },
  /* 8E */  { 0, &BX_CPU_C::PFPNACC_PqQq },
  /* 8F */  { 0, &BX_CPU_C::BxError },
  /* 90 */  { 0, &BX_CPU_C::PFCMPGE_PqQq },
  /* 91 */  { 0, &BX_CPU_C::BxError },
  /* 92 */  { 0, &BX_CPU_C::BxError },
  /* 93 */  { 0, &BX_CPU_C::BxError },
  /* 94 */  { 0, &BX_CPU_C::PFMIN_PqQq },
  /* 95 */  { 0, &BX_CPU_C::BxError },
  /* 96 */  { 0, &BX_CPU_C::PFRCP_PqQq },
  /* 97 */  { 0, &BX_CPU_C::PFRSQRT_PqQq },
  /* 98 */  { 0, &BX_CPU_C::BxError },
  /* 99 */  { 0, &BX_CPU_C::BxError },
  /* 9A */  { 0, &BX_CPU_C::PFSUB_PqQq },
  /* 9B */  { 0, &BX_CPU_C::BxError },
  /* 9C */  { 0, &BX_CPU_C::BxError },
  /* 9D */  { 0, &BX_CPU_C::BxError },
  /* 9E */  { 0, &BX_CPU_C::PFADD_PqQq },
  /* 9F */  { 0, &BX_CPU_C::BxError },
  /* A0 */  { 0, &BX_CPU_C::PFCMPGT_PqQq },
  /* A1 */  { 0, &BX_CPU_C::BxError },
  /* A2 */  { 0, &BX_CPU_C::BxError },
  /* A3 */  { 0, &BX_CPU_C::BxError },
  /* A4 */  { 0, &BX_CPU_C::PFMAX_PqQq },
  /* A5 */  { 0, &BX_CPU_C::BxError },
  /* A6 */  { 0, &BX_CPU_C::PFRCPIT1_PqQq },
  /* A7 */  { 0, &BX_CPU_C::PFRSQIT1_PqQq },
  /* A8 */  { 0, &BX_CPU_C::BxError },
  /* A9 */  { 0, &BX_CPU_C::BxError },
  /* AA */  { 0, &BX_CPU_C::PFSUBR_PqQq },
  /* AB */  { 0, &BX_CPU_C::BxError },
  /* AC */  { 0, &BX_CPU_C::BxError },
  /* AD */  { 0, &BX_CPU_C::BxError },
  /* AE */  { 0, &BX_CPU_C::PFACC_PqQq },
  /* AF */  { 0, &BX_CPU_C::BxError },
  /* B0 */  { 0, &BX_CPU_C::PFCMPEQ_PqQq },
  /* B1 */  { 0, &BX_CPU_C::BxError },
  /* B2 */  { 0, &BX_CPU_C::BxError },
  /* B3 */  { 0, &BX_CPU_C::BxError },
  /* B4 */  { 0, &BX_CPU_C::PFMUL_PqQq },
  /* B5 */  { 0, &BX_CPU_C::BxError },
  /* B6 */  { 0, &BX_CPU_C::PFRCPIT2_PqQq },
  /* B7 */  { 0, &BX_CPU_C::PMULHRW_PqQq },
  /* B8 */  { 0, &BX_CPU_C::BxError },  	
  /* B9 */  { 0, &BX_CPU_C::BxError },
  /* BA */  { 0, &BX_CPU_C::BxError },
  /* BB */  { 0, &BX_CPU_C::PSWAPD_PqQq },
  /* BC */  { 0, &BX_CPU_C::BxError },
  /* BD */  { 0, &BX_CPU_C::BxError },
  /* BE */  { 0, &BX_CPU_C::BxError },
  /* BF */  { 0, &BX_CPU_C::PAVGB_PqQq },
  /* C0 */  { 0, &BX_CPU_C::BxError },
  /* C1 */  { 0, &BX_CPU_C::BxError },
  /* C2 */  { 0, &BX_CPU_C::BxError },
  /* C3 */  { 0, &BX_CPU_C::BxError },
  /* C4 */  { 0, &BX_CPU_C::BxError },
  /* C5 */  { 0, &BX_CPU_C::BxError },
  /* C6 */  { 0, &BX_CPU_C::BxError },
  /* C7 */  { 0, &BX_CPU_C::BxError },
  /* C8 */  { 0, &BX_CPU_C::BxError },
  /* C9 */  { 0, &BX_CPU_C::BxError },
  /* CA */  { 0, &BX_CPU_C::BxError },
  /* CB */  { 0, &BX_CPU_C::BxError },
  /* CC */  { 0, &BX_CPU_C::BxError },
  /* CD */  { 0, &BX_CPU_C::BxError },
  /* CE */  { 0, &BX_CPU_C::BxError },
  /* CF */  { 0, &BX_CPU_C::BxError },
  /* D0 */  { 0, &BX_CPU_C::BxError },
  /* D1 */  { 0, &BX_CPU_C::BxError },
  /* D2 */  { 0, &BX_CPU_C::BxError },
  /* D3 */  { 0, &BX_CPU_C::BxError },
  /* D4 */  { 0, &BX_CPU_C::BxError },
  /* D5 */  { 0, &BX_CPU_C::BxError },
  /* D6 */  { 0, &BX_CPU_C::BxError },
  /* D7 */  { 0, &BX_CPU_C::BxError },
  /* D8 */  { 0, &BX_CPU_C::BxError },
  /* D9 */  { 0, &BX_CPU_C::BxError },
  /* DA */  { 0, &BX_CPU_C::BxError },
  /* DB */  { 0, &BX_CPU_C::BxError },
  /* DC */  { 0, &BX_CPU_C::BxError },
  /* DD */  { 0, &BX_CPU_C::BxError },
  /* DE */  { 0, &BX_CPU_C::BxError },
  /* DF */  { 0, &BX_CPU_C::BxError },
  /* E0 */  { 0, &BX_CPU_C::BxError },
  /* E1 */  { 0, &BX_CPU_C::BxError },
  /* E2 */  { 0, &BX_CPU_C::BxError },
  /* E3 */  { 0, &BX_CPU_C::BxError },
  /* E4 */  { 0, &BX_CPU_C::BxError },
  /* E5 */  { 0, &BX_CPU_C::BxError },
  /* E6 */  { 0, &BX_CPU_C::BxError },
  /* E7 */  { 0, &BX_CPU_C::BxError },
  /* E8 */  { 0, &BX_CPU_C::BxError },
  /* E9 */  { 0, &BX_CPU_C::BxError },
  /* EA */  { 0, &BX_CPU_C::BxError },
  /* EB */  { 0, &BX_CPU_C::BxError },
  /* EC */  { 0, &BX_CPU_C::BxError },
  /* ED */  { 0, &BX_CPU_C::BxError },
  /* EE */  { 0, &BX_CPU_C::BxError },
  /* EF */  { 0, &BX_CPU_C::BxError },
  /* F0 */  { 0, &BX_CPU_C::BxError },
  /* F1 */  { 0, &BX_CPU_C::BxError },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError },
  /* F4 */  { 0, &BX_CPU_C::BxError },
  /* F5 */  { 0, &BX_CPU_C::BxError },
  /* F6 */  { 0, &BX_CPU_C::BxError },
  /* F7 */  { 0, &BX_CPU_C::BxError },
  /* F8 */  { 0, &BX_CPU_C::BxError },
  /* F9 */  { 0, &BX_CPU_C::BxError },
  /* FA */  { 0, &BX_CPU_C::BxError },
  /* FB */  { 0, &BX_CPU_C::BxError },
  /* FC */  { 0, &BX_CPU_C::BxError },
  /* FD */  { 0, &BX_CPU_C::BxError },
  /* FE */  { 0, &BX_CPU_C::BxError },
  /* FF */  { 0, &BX_CPU_C::BxError }
};

#endif


/* ************************************************************************ */
/* SSE Opcode Groups */


/*** Duplicate SSE/SSE2 instructions ***/

#define MOVUPD_VpdWpd  /* 66 0f 10 */ MOVUPS_VpsWps    /*    0f 10 */
#define MOVUPD_WpdVpd  /* 66 0f 11 */ MOVUPS_WpsVps    /*    0f 11 */
#define MOVAPD_VpdWpd  /* 66 0f 28 */ MOVAPS_VpsWps    /*    0f 28 */
#define MOVAPD_WpdVpd  /* 66 0f 29 */ MOVAPS_WpsVps    /*    0f 29 */
#define MOVDQU_VdqWdq  /* f3 0f 6f */ MOVUPS_VpsWps    /*    0f 10 */
#define MOVDQU_WdqVdq  /* f3 0f 7f */ MOVUPS_WpsVps    /*    0f 11 */
#define MOVDQA_VdqWdq  /* 66 0f 6f */ MOVAPS_VpsWps    /*    0f 28 */
#define MOVDQA_WdqVdq  /* 66 0f 7f */ MOVAPS_WpsVps    /*    0f 29 */

#define UNPCKHPS_VpsWq /*    0f 15 */ PUNPCKHDQ_VdqWq  /* 66 0f 6a */
#define UNPCKLPS_VpsWq /*    0f 14 */ PUNPCKLDQ_VdqWq  /* 66 0f 62 */

#define ANDPS_VpsWps   /*    0f 54 */ PAND_VdqWdq      /* 66 0f db */
#define ANDPD_VpdWpd   /* 66 0f 54 */ PAND_VdqWdq      /* 66 0f db */
#define ANDNPS_VpsWps  /*    0f 55 */ PANDN_VdqWdq     /* 66 0f df */
#define ANDNPD_VpdWpd  /* 66 0f 55 */ PANDN_VdqWdq     /* 66 0f df */
#define ORPS_VpsWps    /*    0f 56 */ POR_VdqWdq       /* 66 0f eb */
#define ORPD_VpdWpd    /* 66 0f 56 */ POR_VdqWdq       /* 66 0f eb */
#define XORPS_VpsWps   /*    0f 57 */ PXOR_VdqWdq      /* 66 0f ef */
#define XORPD_VpdWpd   /* 66 0f 57 */ PXOR_VdqWdq      /* 66 0f ef */

#define UNPCKHPD_VpdWq /* 66 0f 15 */ PUNPCKHQDQ_VdqWq /* 66 0f 6d */
#define UNPCKLPD_VpdWq /* 66 0f 14 */ PUNPCKLQDQ_VdqWq /* 66 0f 6c */

#define MOVLPD_VsdMq   /* 66 0f 12 */ MOVLPS_VpsMq     /*    0f 12 */
#define MOVLPD_MqVsd   /* 66 0f 13 */ MOVLPS_MqVps     /*    0f 13 */
#define MOVHPD_VsdMq   /* 66 0f 16 */ MOVHPS_VpsMq     /*    0f 16 */
#define MOVHPD_MqVsd   /* 66 0f 17 */ MOVHPS_MqVps     /*    0f 17 */

#define MOVNTPD_MdqVpd /* 66 0f 2b */ MOVNTPS_MdqVps   /*    0f 2b */
#define MOVNTDQ_MdqVdq /* 66 0f e7 */ MOVNTPD_MdqVpd   /* 66 0f 2b */


/* ********** */
/* SSE Groups */
/* ********** */

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVUPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::MOVUPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::MOVSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::MOVSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVUPS_WpsVps },
  /* 66 */  { 0, &BX_CPU_C::MOVUPD_WpdVpd },
  /* F2 */  { 0, &BX_CPU_C::MOVSD_WsdVsd },
  /* F3 */  { 0, &BX_CPU_C::MOVSS_WssVss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVLPS_VpsMq },
  /* 66 */  { 0, &BX_CPU_C::MOVLPD_VsdMq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f13[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVLPS_MqVps },
  /* 66 */  { 0, &BX_CPU_C::MOVLPD_MqVsd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[4] = {
  /* -- */  { 0, &BX_CPU_C::UNPCKLPS_VpsWq },
  /* 66 */  { 0, &BX_CPU_C::UNPCKLPD_VpdWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[4] = {
  /* -- */  { 0, &BX_CPU_C::UNPCKHPS_VpsWq },
  /* 66 */  { 0, &BX_CPU_C::UNPCKHPD_VpdWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVHPS_VpsMq },
  /* 66 */  { 0, &BX_CPU_C::MOVHPD_VsdMq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f17[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVHPS_MqVps },
  /* 66 */  { 0, &BX_CPU_C::MOVHPD_MqVsd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVAPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::MOVAPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVAPS_WpsVps },
  /* 66 */  { 0, &BX_CPU_C::MOVAPD_WpdVpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[4] = {
  /* -- */  { 0, &BX_CPU_C::CVTPI2PS_VpsQq },
  /* 66 */  { 0, &BX_CPU_C::CVTPI2PD_VpdQd },
  /* F2 */  { 0, &BX_CPU_C::CVTSI2SD_VsdEd },
  /* F3 */  { 0, &BX_CPU_C::CVTSI2SS_VssEd }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2b[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVNTPS_MdqVps },
  /* 66 */  { 0, &BX_CPU_C::MOVNTPD_MdqVpd },            	
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[4] = {
  /* -- */  { 0, &BX_CPU_C::CVTTPS2PI_PqWps },
  /* 66 */  { 0, &BX_CPU_C::CVTTPD2PI_PqWpd },
  /* F2 */  { 0, &BX_CPU_C::CVTTSD2SI_GdWsd },
  /* F3 */  { 0, &BX_CPU_C::CVTTSS2SI_GdWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[4] = {
  /* -- */  { 0, &BX_CPU_C::CVTPS2PI_PqWps },
  /* 66 */  { 0, &BX_CPU_C::CVTPD2PI_PqWpd },
  /* F2 */  { 0, &BX_CPU_C::CVTSD2SI_GdWsd },
  /* F3 */  { 0, &BX_CPU_C::CVTSS2SI_GdWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[4] = {
  /* -- */  { 0, &BX_CPU_C::UCOMISS_VssWss },
  /* 66 */  { 0, &BX_CPU_C::UCOMISD_VsdWsd },            	
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[4] = {
  /* -- */  { 0, &BX_CPU_C::COMISS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::COMISD_VpdWpd },   
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f50[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVMSKPS_GdVRps },
  /* 66 */  { 0, &BX_CPU_C::MOVMSKPD_EdVRpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[4] = {
  /* -- */  { 0, &BX_CPU_C::SQRTPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::SQRTPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::SQRTSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::SQRTSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[4] = {
  /* -- */  { 0, &BX_CPU_C::RSQRTPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::BxError },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::RSQRTSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[4] = {
  /* -- */  { 0, &BX_CPU_C::RCPPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::BxError },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::RCPSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[4] = {
  /* -- */  { 0, &BX_CPU_C::ANDPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::ANDPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[4] = {
  /* -- */  { 0, &BX_CPU_C::ANDNPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::ANDNPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[4] = {
  /* -- */  { 0, &BX_CPU_C::ORPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::ORPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[4] = {
  /* -- */  { 0, &BX_CPU_C::XORPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::XORPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[4] = {
  /* -- */  { 0, &BX_CPU_C::ADDPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::ADDPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::ADDSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::ADDSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[4] = {
  /* -- */  { 0, &BX_CPU_C::MULPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::MULPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::MULSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::MULSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[4] = {
  /* -- */  { 0, &BX_CPU_C::CVTPS2PD_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::CVTPD2PS_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::CVTSD2SS_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::CVTSS2SD_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[4] = {
  /* -- */  { 0, &BX_CPU_C::CVTDQ2PS_VpsWdq  },
  /* 66 */  { 0, &BX_CPU_C::CVTPS2DQ_VdqWps  },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::CVTTPS2DQ_VdqWps }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[4] = {
  /* -- */  { 0, &BX_CPU_C::SUBPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::SUBPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::SUBSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::SUBSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[4] = {
  /* -- */  { 0, &BX_CPU_C::MINPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::MINPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::MINSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::MINSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[4] = {
  /* -- */  { 0, &BX_CPU_C::DIVPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::DIVPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::DIVSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::DIVSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[4] = {
  /* -- */  { 0, &BX_CPU_C::MAXPS_VpsWps },
  /* 66 */  { 0, &BX_CPU_C::MAXPD_VpdWpd },
  /* F2 */  { 0, &BX_CPU_C::MAXSD_VsdWsd },
  /* F3 */  { 0, &BX_CPU_C::MAXSS_VssWss }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKLBW_PqQd  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKLBW_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKLWD_PqQd  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKLWD_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKLDQ_PqQd  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKLDQ_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[4] = {
  /* -- */  { 0, &BX_CPU_C::PACKSSWB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PACKSSWB_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPGTB_PqQq  },
  /* 66 */  { 0, &BX_CPU_C::PCMPGTB_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPGTW_PqQq  },
  /* 66 */  { 0, &BX_CPU_C::PCMPGTW_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPGTD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PCMPGTD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[4] = {
  /* -- */  { 0, &BX_CPU_C::PACKUSWB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PACKUSWB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKHBW_PqQq  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKHBW_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKHWD_PqQq  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKHWD_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[4] = {
  /* -- */  { 0, &BX_CPU_C::PUNPCKHDQ_PqQq  },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKHDQ_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[4] = {
  /* -- */  { 0, &BX_CPU_C::PACKSSDW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PACKSSDW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6c[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKLQDQ_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6d[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::PUNPCKHQDQ_VdqWq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVD_PqEd  },
  /* 66 */  { 0, &BX_CPU_C::MOVD_VdqEd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVQ_PqQq     },
  /* 66 */  { 0, &BX_CPU_C::MOVDQA_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError       },
  /* F3 */  { 0, &BX_CPU_C::MOVDQU_VdqWdq },
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[4] = {
  /* -- */  { 0, &BX_CPU_C::PSHUFW_PqQqIb   },
  /* 66 */  { 0, &BX_CPU_C::PSHUFD_VdqWdqIb },
  /* F2 */  { 0, &BX_CPU_C::PSHUFHW_VqWqIb  },
  /* F3 */  { 0, &BX_CPU_C::PSHUFLW_VqWqIb  }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPEQB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PCMPEQB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPEQW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PCMPEQW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[4] = {
  /* -- */  { 0, &BX_CPU_C::PCMPEQD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PCMPEQD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVD_EdPd },
  /* 66 */  { 0, &BX_CPU_C::MOVD_EdVd },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::MOVQ_VqWq },
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVQ_QqPq     },
  /* 66 */  { 0, &BX_CPU_C::MOVDQA_WdqVdq },
  /* F2 */  { 0, &BX_CPU_C::BxError       },
  /* F3 */  { 0, &BX_CPU_C::MOVDQU_WdqVdq },
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[4] = {
  /* -- */  { 0, &BX_CPU_C::CMPPS_VpsWpsIb },
  /* 66 */  { 0, &BX_CPU_C::CMPPD_VpdWpdIb },
  /* F2 */  { 0, &BX_CPU_C::CMPSD_VsdWsdIb },
  /* F3 */  { 0, &BX_CPU_C::CMPSS_VssWssIb }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fc3[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVNTI_MdGd },
  /* 66 */  { 0, &BX_CPU_C::BxError },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[4] = {
  /* -- */  { BxImmediate_Ib, &BX_CPU_C::PINSRW_PqEdIb  },
  /* 66 */  { BxImmediate_Ib, &BX_CPU_C::PINSRW_VdqEdIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5[4] = {
  /* -- */  { BxImmediate_Ib, &BX_CPU_C::PEXTRW_PqEdIb  },
  /* 66 */  { BxImmediate_Ib, &BX_CPU_C::PEXTRW_VdqEdIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[4] = {
  /* -- */  { BxImmediate_Ib, &BX_CPU_C::SHUFPS_VpsWpsIb },
  /* 66 */  { BxImmediate_Ib, &BX_CPU_C::SHUFPD_VpdWpdIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSRLW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSRLD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLQ_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSRLQ_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDQ_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDQ_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[4] = {
  /* -- */  { 0, &BX_CPU_C::PMULLW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMULLW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::MOVQ_WqVq     },
  /* F2 */  { 0, &BX_CPU_C::MOVDQ2Q_PqVRq },
  /* F3 */  { 0, &BX_CPU_C::MOVQ2DQ_VdqQq },
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[4] = {
  /* -- */  { 0, &BX_CPU_C::PMOVMSKB_GdPRq  },
  /* 66 */  { 0, &BX_CPU_C::PMOVMSKB_GdVRdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBUSB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBUSB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBUSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBUSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[4] = {
  /* -- */  { 0, &BX_CPU_C::PMINUB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMINUB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[4] = {
  /* -- */  { 0, &BX_CPU_C::PAND_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PAND_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDUSB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDUSB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDUSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDUSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[4] = {
  /* -- */  { 0, &BX_CPU_C::PMAXUB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMAXUB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[4] = {
  /* -- */  { 0, &BX_CPU_C::PANDN_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PANDN_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[4] = {
  /* -- */  { 0, &BX_CPU_C::PAVGB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PAVGB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRAW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSRAW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRAD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSRAD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[4] = {
  /* -- */  { 0, &BX_CPU_C::PAVGW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PAVGW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[4] = {
  /* -- */  { 0, &BX_CPU_C::PMULHUW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMULHUW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[4] = {
  /* -- */  { 0, &BX_CPU_C::PMULHW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMULHW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::CVTTPD2DQ_VqWpd },
  /* F2 */  { 0, &BX_CPU_C::CVTPD2DQ_VqWpd  },
  /* F3 */  { 0, &BX_CPU_C::CVTDQ2PD_VpdWq  }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7[4] = {
  /* -- */  { 0, &BX_CPU_C::MOVNTQ_MqPq    },
  /* 66 */  { 0, &BX_CPU_C::MOVNTDQ_MdqVdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBSB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBSB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[4] = {
  /* -- */  { 0, &BX_CPU_C::PMINSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMINSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[4] = {
  /* -- */  { 0, &BX_CPU_C::POR_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::POR_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDSB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDSB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[4] = {
  /* -- */  { 0, &BX_CPU_C::PMAXSW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMAXSW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[4] = {
  /* -- */  { 0, &BX_CPU_C::PXOR_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PXOR_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSLLW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSLLD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLQ_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSLLQ_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[4] = {
  /* -- */  { 0, &BX_CPU_C::PMULUDQ_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMULUDQ_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[4] = {
  /* -- */  { 0, &BX_CPU_C::PMADDWD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PMADDWD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[4] = {
  /* -- */  { 0, &BX_CPU_C::PSADBW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSADBW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7[4] = {
  /* -- */  { 0, &BX_CPU_C::MASKMOVQ_PqPRq     },
  /* 66 */  { 0, &BX_CPU_C::MASKMOVDQU_VdqVRdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[4] = {
  /* -- */  { 0, &BX_CPU_C::PSUBQ_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PSUBQ_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDB_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDB_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDW_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDW_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[4] = {
  /* -- */  { 0, &BX_CPU_C::PADDD_PqQq   },
  /* 66 */  { 0, &BX_CPU_C::PADDD_VdqWdq },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLW_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSRLW_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRAW_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSRAW_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLW_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSLLW_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLD_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSRLD_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRAD_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSRAD_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLD_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSLLD_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[4] = {
  /* -- */  { 0, &BX_CPU_C::PSRLQ_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSRLQ_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1403[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::PSRLDQ_WdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[4] = {
  /* -- */  { 0, &BX_CPU_C::PSLLQ_PqIb  },
  /* 66 */  { 0, &BX_CPU_C::PSLLQ_PdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

static BxOpcodeInfo_t BxOpcodeGroupSSE_G1407[4] = {
  /* -- */  { 0, &BX_CPU_C::BxError },
  /* 66 */  { 0, &BX_CPU_C::PSLLDQ_WdqIb },
  /* F2 */  { 0, &BX_CPU_C::BxError },
  /* F3 */  { 0, &BX_CPU_C::BxError }
  };

#endif
