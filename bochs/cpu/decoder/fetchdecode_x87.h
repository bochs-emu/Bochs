/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2016 Stanislav Shwartsman
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

#ifndef BX_X87_FETCHDECODE_TABLES_H
#define BX_X87_FETCHDECODE_TABLES_H

//
// Common FetchDecode Opcode Tables - x87 and 3dnow!
//

#if BX_SUPPORT_FPU

/* ************************************************************************ */
/* FPU Opcodes */


// For compatibility with prior generations there are a few reserved x87 opcodes which do not result in an invalid
// opcode (#UD) exception, but rather result in the same behavior as existing defined x87 instructions. In the interest
// of standardization, it is recommended that the opcodes defined in the Intel 64 and IA-32 Architectures Software
// Developers Manual, Volumes 2A, 2B, 2C, & 2D, be used for these operations for standardization.

//  * DCD0H .. DCD7H - Behaves the same as FCOM, D8D0H through D8D7H.
//  * DCD8H .. DCDFH - Behaves the same as FCOMP, D8D8H through D8DFH.
//  * DDC8H .. DDCFH - Behaves the same as FXCH, D9C8H through D9CFH.
//  * DED0H .. DED7H - Behaves the same as FCOMP, D8D8H through D8DFH.
//  * DFD0H .. DFD7H - Behaves the same as FSTP, DDD8H through DDDFH.
//  * DFC8H .. DFCFH - Behaves the same as FXCH, D9C8H through D9CFH.
//  * DFD8H .. DFDFH - Behaves the same as FSTP, DDD8H through DDDFH.

// There are a few reserved x87 opcodes which provide unique behavior but do not provide capabilities which are not
// already available in the main instructions:
//  * D9D8H through D9DFH - Behaves the same as FSTP (DDD8H through DDDFH) but won't cause a stack underflow exception.
//  * DFC0H through DFC7H - Behaves the same as FFREE (DDC0H through DDD7H) with the addition of an x87 stack POP.

// D8 (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointD8[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FADD_SINGLE_REAL,
  /* 1 */ BX_IA_FMUL_SINGLE_REAL,
  /* 2 */ BX_IA_FCOM_SINGLE_REAL,
  /* 3 */ BX_IA_FCOMP_SINGLE_REAL,
  /* 4 */ BX_IA_FSUB_SINGLE_REAL,
  /* 5 */ BX_IA_FSUBR_SINGLE_REAL,
  /* 6 */ BX_IA_FDIV_SINGLE_REAL,
  /* 7 */ BX_IA_FDIVR_SINGLE_REAL,

  /* /r form */
  /* D8 C0 */ BX_IA_FADD_ST0_STj,
  /* D8 C1 */ BX_IA_FADD_ST0_STj,
  /* D8 C2 */ BX_IA_FADD_ST0_STj,
  /* D8 C3 */ BX_IA_FADD_ST0_STj,
  /* D8 C4 */ BX_IA_FADD_ST0_STj,
  /* D8 C5 */ BX_IA_FADD_ST0_STj,
  /* D8 C6 */ BX_IA_FADD_ST0_STj,
  /* D8 C7 */ BX_IA_FADD_ST0_STj,
  /* D8 C8 */ BX_IA_FMUL_ST0_STj,
  /* D8 C9 */ BX_IA_FMUL_ST0_STj,
  /* D8 CA */ BX_IA_FMUL_ST0_STj,
  /* D8 CB */ BX_IA_FMUL_ST0_STj,
  /* D8 CC */ BX_IA_FMUL_ST0_STj,
  /* D8 CD */ BX_IA_FMUL_ST0_STj,
  /* D8 CE */ BX_IA_FMUL_ST0_STj,
  /* D8 CF */ BX_IA_FMUL_ST0_STj,
  /* D8 D0 */ BX_IA_FCOM_STi,
  /* D8 D1 */ BX_IA_FCOM_STi,
  /* D8 D2 */ BX_IA_FCOM_STi,
  /* D8 D3 */ BX_IA_FCOM_STi,
  /* D8 D4 */ BX_IA_FCOM_STi,
  /* D8 D5 */ BX_IA_FCOM_STi,
  /* D8 D6 */ BX_IA_FCOM_STi,
  /* D8 D7 */ BX_IA_FCOM_STi,
  /* D8 D8 */ BX_IA_FCOMP_STi,
  /* D8 D9 */ BX_IA_FCOMP_STi,
  /* D8 DA */ BX_IA_FCOMP_STi,
  /* D8 DB */ BX_IA_FCOMP_STi,
  /* D8 DC */ BX_IA_FCOMP_STi,
  /* D8 DD */ BX_IA_FCOMP_STi,
  /* D8 DE */ BX_IA_FCOMP_STi,
  /* D8 DF */ BX_IA_FCOMP_STi,
  /* D8 E0 */ BX_IA_FSUB_ST0_STj,
  /* D8 E1 */ BX_IA_FSUB_ST0_STj,
  /* D8 E2 */ BX_IA_FSUB_ST0_STj,
  /* D8 E3 */ BX_IA_FSUB_ST0_STj,
  /* D8 E4 */ BX_IA_FSUB_ST0_STj,
  /* D8 E5 */ BX_IA_FSUB_ST0_STj,
  /* D8 E6 */ BX_IA_FSUB_ST0_STj,
  /* D8 E7 */ BX_IA_FSUB_ST0_STj,
  /* D8 E8 */ BX_IA_FSUBR_ST0_STj,
  /* D8 E9 */ BX_IA_FSUBR_ST0_STj,
  /* D8 EA */ BX_IA_FSUBR_ST0_STj,
  /* D8 EB */ BX_IA_FSUBR_ST0_STj,
  /* D8 EC */ BX_IA_FSUBR_ST0_STj,
  /* D8 ED */ BX_IA_FSUBR_ST0_STj,
  /* D8 EE */ BX_IA_FSUBR_ST0_STj,
  /* D8 EF */ BX_IA_FSUBR_ST0_STj,
  /* D8 F0 */ BX_IA_FDIV_ST0_STj,
  /* D8 F1 */ BX_IA_FDIV_ST0_STj,
  /* D8 F2 */ BX_IA_FDIV_ST0_STj,
  /* D8 F3 */ BX_IA_FDIV_ST0_STj,
  /* D8 F4 */ BX_IA_FDIV_ST0_STj,
  /* D8 F5 */ BX_IA_FDIV_ST0_STj,
  /* D8 F6 */ BX_IA_FDIV_ST0_STj,
  /* D8 F7 */ BX_IA_FDIV_ST0_STj,
  /* D8 F8 */ BX_IA_FDIVR_ST0_STj,
  /* D8 F9 */ BX_IA_FDIVR_ST0_STj,
  /* D8 FA */ BX_IA_FDIVR_ST0_STj,
  /* D8 FB */ BX_IA_FDIVR_ST0_STj,
  /* D8 FC */ BX_IA_FDIVR_ST0_STj,
  /* D8 FD */ BX_IA_FDIVR_ST0_STj,
  /* D8 FE */ BX_IA_FDIVR_ST0_STj,
  /* D8 FF */ BX_IA_FDIVR_ST0_STj
};

// D9 (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointD9[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FLD_SINGLE_REAL,
  /* 1 */ BX_IA_ERROR,
  /* 2 */ BX_IA_FST_SINGLE_REAL,
  /* 3 */ BX_IA_FSTP_SINGLE_REAL,
  /* 4 */ BX_IA_FLDENV,
  /* 5 */ BX_IA_FLDCW,
  /* 6 */ BX_IA_FNSTENV,
  /* 7 */ BX_IA_FNSTCW,

  /* /r form */
  /* D9 C0 */ BX_IA_FLD_STi,
  /* D9 C1 */ BX_IA_FLD_STi,
  /* D9 C2 */ BX_IA_FLD_STi,
  /* D9 C3 */ BX_IA_FLD_STi,
  /* D9 C4 */ BX_IA_FLD_STi,
  /* D9 C5 */ BX_IA_FLD_STi,
  /* D9 C6 */ BX_IA_FLD_STi,
  /* D9 C7 */ BX_IA_FLD_STi,
  /* D9 C8 */ BX_IA_FXCH_STi,
  /* D9 C9 */ BX_IA_FXCH_STi,
  /* D9 CA */ BX_IA_FXCH_STi,
  /* D9 CB */ BX_IA_FXCH_STi,
  /* D9 CC */ BX_IA_FXCH_STi,
  /* D9 CD */ BX_IA_FXCH_STi,
  /* D9 CE */ BX_IA_FXCH_STi,
  /* D9 CF */ BX_IA_FXCH_STi,
  /* D9 D0 */ BX_IA_FNOP,
  /* D9 D1 */ BX_IA_ERROR,
  /* D9 D2 */ BX_IA_ERROR,
  /* D9 D3 */ BX_IA_ERROR,
  /* D9 D4 */ BX_IA_ERROR,
  /* D9 D5 */ BX_IA_ERROR,
  /* D9 D6 */ BX_IA_ERROR,
  /* D9 D7 */ BX_IA_ERROR,
  /* D9 D8 */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 D9 */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DA */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DB */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DC */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DD */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DE */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 DF */ BX_IA_FSTP_SPECIAL_STi,	// undocumented
  /* D9 E0 */ BX_IA_FCHS,
  /* D9 E1 */ BX_IA_FABS,
  /* D9 E2 */ BX_IA_ERROR,
  /* D9 E3 */ BX_IA_ERROR,
  /* D9 E4 */ BX_IA_FTST,
  /* D9 E5 */ BX_IA_FXAM,
  /* D9 E6 */ BX_IA_ERROR,
  /* D9 E7 */ BX_IA_ERROR,
  /* D9 E8 */ BX_IA_FLD1,
  /* D9 E9 */ BX_IA_FLDL2T,
  /* D9 EA */ BX_IA_FLDL2E,
  /* D9 EB */ BX_IA_FLDPI,
  /* D9 EC */ BX_IA_FLDLG2,
  /* D9 ED */ BX_IA_FLDLN2,
  /* D9 EE */ BX_IA_FLDZ,
  /* D9 EF */ BX_IA_ERROR,
  /* D9 F0 */ BX_IA_F2XM1,
  /* D9 F1 */ BX_IA_FYL2X,
  /* D9 F2 */ BX_IA_FPTAN,
  /* D9 F3 */ BX_IA_FPATAN,
  /* D9 F4 */ BX_IA_FXTRACT,
  /* D9 F5 */ BX_IA_FPREM1,
  /* D9 F6 */ BX_IA_FDECSTP,
  /* D9 F7 */ BX_IA_FINCSTP,
  /* D9 F8 */ BX_IA_FPREM,
  /* D9 F9 */ BX_IA_FYL2XP1,
  /* D9 FA */ BX_IA_FSQRT,
  /* D9 FB */ BX_IA_FSINCOS,
  /* D9 FC */ BX_IA_FRNDINT,
  /* D9 FD */ BX_IA_FSCALE,
  /* D9 FE */ BX_IA_FSIN,
  /* D9 FF */ BX_IA_FCOS
};

// DA (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDA[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FIADD_DWORD_INTEGER,
  /* 1 */ BX_IA_FIMUL_DWORD_INTEGER,
  /* 2 */ BX_IA_FICOM_DWORD_INTEGER,
  /* 3 */ BX_IA_FICOMP_DWORD_INTEGER,
  /* 4 */ BX_IA_FISUB_DWORD_INTEGER,
  /* 5 */ BX_IA_FISUBR_DWORD_INTEGER,
  /* 6 */ BX_IA_FIDIV_DWORD_INTEGER,
  /* 7 */ BX_IA_FIDIVR_DWORD_INTEGER,

  /* /r form */
  /* DA C0 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C1 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C2 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C3 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C4 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C5 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C6 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C7 */ BX_IA_FCMOVB_ST0_STj,
  /* DA C8 */ BX_IA_FCMOVE_ST0_STj,
  /* DA C9 */ BX_IA_FCMOVE_ST0_STj,
  /* DA CA */ BX_IA_FCMOVE_ST0_STj,
  /* DA CB */ BX_IA_FCMOVE_ST0_STj,
  /* DA CC */ BX_IA_FCMOVE_ST0_STj,
  /* DA CD */ BX_IA_FCMOVE_ST0_STj,
  /* DA CE */ BX_IA_FCMOVE_ST0_STj,
  /* DA CF */ BX_IA_FCMOVE_ST0_STj,
  /* DA D0 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D1 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D2 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D3 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D4 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D5 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D6 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D7 */ BX_IA_FCMOVBE_ST0_STj,
  /* DA D8 */ BX_IA_FCMOVU_ST0_STj,
  /* DA D9 */ BX_IA_FCMOVU_ST0_STj,
  /* DA DA */ BX_IA_FCMOVU_ST0_STj,
  /* DA DB */ BX_IA_FCMOVU_ST0_STj,
  /* DA DC */ BX_IA_FCMOVU_ST0_STj,
  /* DA DD */ BX_IA_FCMOVU_ST0_STj,
  /* DA DE */ BX_IA_FCMOVU_ST0_STj,
  /* DA DF */ BX_IA_FCMOVU_ST0_STj,
  /* DA E0 */ BX_IA_ERROR,
  /* DA E1 */ BX_IA_ERROR,
  /* DA E2 */ BX_IA_ERROR,
  /* DA E3 */ BX_IA_ERROR,
  /* DA E4 */ BX_IA_ERROR,
  /* DA E5 */ BX_IA_ERROR,
  /* DA E6 */ BX_IA_ERROR,
  /* DA E7 */ BX_IA_ERROR,
  /* DA E8 */ BX_IA_ERROR,
  /* DA E9 */ BX_IA_FUCOMPP,
  /* DA EA */ BX_IA_ERROR,
  /* DA EB */ BX_IA_ERROR,
  /* DA EC */ BX_IA_ERROR,
  /* DA ED */ BX_IA_ERROR,
  /* DA EE */ BX_IA_ERROR,
  /* DA EF */ BX_IA_ERROR,
  /* DA F0 */ BX_IA_ERROR,
  /* DA F1 */ BX_IA_ERROR,
  /* DA F2 */ BX_IA_ERROR,
  /* DA F3 */ BX_IA_ERROR,
  /* DA F4 */ BX_IA_ERROR,
  /* DA F5 */ BX_IA_ERROR,
  /* DA F6 */ BX_IA_ERROR,
  /* DA F7 */ BX_IA_ERROR,
  /* DA F8 */ BX_IA_ERROR,
  /* DA F9 */ BX_IA_ERROR,
  /* DA FA */ BX_IA_ERROR,
  /* DA FB */ BX_IA_ERROR,
  /* DA FC */ BX_IA_ERROR,
  /* DA FD */ BX_IA_ERROR,
  /* DA FE */ BX_IA_ERROR,
  /* DA FF */ BX_IA_ERROR
};

// DB (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDB[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FILD_DWORD_INTEGER,
  /* 1 */ BX_IA_FISTTP_Md,
  /* 2 */ BX_IA_FIST_DWORD_INTEGER,
  /* 3 */ BX_IA_FISTP_DWORD_INTEGER,
  /* 4 */ BX_IA_ERROR,
  /* 5 */ BX_IA_FLD_EXTENDED_REAL,
  /* 6 */ BX_IA_ERROR,
  /* 7 */ BX_IA_FSTP_EXTENDED_REAL,

  /* /r form */
  /* DB C0 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C1 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C2 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C3 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C4 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C5 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C6 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C7 */ BX_IA_FCMOVNB_ST0_STj,
  /* DB C8 */ BX_IA_FCMOVNE_ST0_STj,
  /* DB C9 */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CA */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CB */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CC */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CD */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CE */ BX_IA_FCMOVNE_ST0_STj,
  /* DB CF */ BX_IA_FCMOVNE_ST0_STj,
  /* DB D0 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D1 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D2 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D3 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D4 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D5 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D6 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D7 */ BX_IA_FCMOVNBE_ST0_STj,
  /* DB D8 */ BX_IA_FCMOVNU_ST0_STj,
  /* DB D9 */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DA */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DB */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DC */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DD */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DE */ BX_IA_FCMOVNU_ST0_STj,
  /* DB DF */ BX_IA_FCMOVNU_ST0_STj,
  /* DB E0 */ BX_IA_FPLEGACY,      // feni  (287 only)
  /* DB E1 */ BX_IA_FPLEGACY,      // fdisi (287 only)
  /* DB E2 */ BX_IA_FNCLEX,
  /* DB E3 */ BX_IA_FNINIT,
  /* DB E4 */ BX_IA_FPLEGACY,      // fsetpm (287 only)
  /* DB E5 */ BX_IA_ERROR,
  /* DB E6 */ BX_IA_ERROR,
  /* DB E7 */ BX_IA_ERROR,
  /* DB E8 */ BX_IA_FUCOMI_ST0_STj,
  /* DB E9 */ BX_IA_FUCOMI_ST0_STj,
  /* DB EA */ BX_IA_FUCOMI_ST0_STj,
  /* DB EB */ BX_IA_FUCOMI_ST0_STj,
  /* DB EC */ BX_IA_FUCOMI_ST0_STj,
  /* DB ED */ BX_IA_FUCOMI_ST0_STj,
  /* DB EE */ BX_IA_FUCOMI_ST0_STj,
  /* DB EF */ BX_IA_FUCOMI_ST0_STj,
  /* DB F0 */ BX_IA_FCOMI_ST0_STj,
  /* DB F1 */ BX_IA_FCOMI_ST0_STj,
  /* DB F2 */ BX_IA_FCOMI_ST0_STj,
  /* DB F3 */ BX_IA_FCOMI_ST0_STj,
  /* DB F4 */ BX_IA_FCOMI_ST0_STj,
  /* DB F5 */ BX_IA_FCOMI_ST0_STj,
  /* DB F6 */ BX_IA_FCOMI_ST0_STj,
  /* DB F7 */ BX_IA_FCOMI_ST0_STj,
  /* DB F8 */ BX_IA_ERROR,
  /* DB F9 */ BX_IA_ERROR,
  /* DB FA */ BX_IA_ERROR,
  /* DB FB */ BX_IA_ERROR,
  /* DB FC */ BX_IA_ERROR,
  /* DB FD */ BX_IA_ERROR,
  /* DB FE */ BX_IA_ERROR,
  /* DB FF */ BX_IA_ERROR
};

// DC (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDC[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FADD_DOUBLE_REAL,
  /* 1 */ BX_IA_FMUL_DOUBLE_REAL,
  /* 2 */ BX_IA_FCOM_DOUBLE_REAL,
  /* 3 */ BX_IA_FCOMP_DOUBLE_REAL,
  /* 4 */ BX_IA_FSUB_DOUBLE_REAL,
  /* 5 */ BX_IA_FSUBR_DOUBLE_REAL,
  /* 6 */ BX_IA_FDIV_DOUBLE_REAL,
  /* 7 */ BX_IA_FDIVR_DOUBLE_REAL,

  /* /r form */
  /* DC C0 */ BX_IA_FADD_STi_ST0,
  /* DC C1 */ BX_IA_FADD_STi_ST0,
  /* DC C2 */ BX_IA_FADD_STi_ST0,
  /* DC C3 */ BX_IA_FADD_STi_ST0,
  /* DC C4 */ BX_IA_FADD_STi_ST0,
  /* DC C5 */ BX_IA_FADD_STi_ST0,
  /* DC C6 */ BX_IA_FADD_STi_ST0,
  /* DC C7 */ BX_IA_FADD_STi_ST0,
  /* DC C8 */ BX_IA_FMUL_STi_ST0,
  /* DC C9 */ BX_IA_FMUL_STi_ST0,
  /* DC CA */ BX_IA_FMUL_STi_ST0,
  /* DC CB */ BX_IA_FMUL_STi_ST0,
  /* DC CC */ BX_IA_FMUL_STi_ST0,
  /* DC CD */ BX_IA_FMUL_STi_ST0,
  /* DC CE */ BX_IA_FMUL_STi_ST0,
  /* DC CF */ BX_IA_FMUL_STi_ST0,
  /* DC D0 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D1 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D2 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D3 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D4 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D5 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D6 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D7 */ BX_IA_FCOM_STi,                 // undocumented
  /* DC D8 */ BX_IA_FCOMP_STi,                // undocumented
  /* DC D9 */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DA */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DB */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DC */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DD */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DE */ BX_IA_FCOMP_STi,                // undocumented
  /* DC DF */ BX_IA_FCOMP_STi,                // undocumented
  /* DC E0 */ BX_IA_FSUBR_STi_ST0,
  /* DC E1 */ BX_IA_FSUBR_STi_ST0,
  /* DC E2 */ BX_IA_FSUBR_STi_ST0,
  /* DC E3 */ BX_IA_FSUBR_STi_ST0,
  /* DC E4 */ BX_IA_FSUBR_STi_ST0,
  /* DC E5 */ BX_IA_FSUBR_STi_ST0,
  /* DC E6 */ BX_IA_FSUBR_STi_ST0,
  /* DC E7 */ BX_IA_FSUBR_STi_ST0,
  /* DC E8 */ BX_IA_FSUB_STi_ST0,
  /* DC E9 */ BX_IA_FSUB_STi_ST0,
  /* DC EA */ BX_IA_FSUB_STi_ST0,
  /* DC EB */ BX_IA_FSUB_STi_ST0,
  /* DC EC */ BX_IA_FSUB_STi_ST0,
  /* DC ED */ BX_IA_FSUB_STi_ST0,
  /* DC EE */ BX_IA_FSUB_STi_ST0,
  /* DC EF */ BX_IA_FSUB_STi_ST0,
  /* DC F0 */ BX_IA_FDIVR_STi_ST0,
  /* DC F1 */ BX_IA_FDIVR_STi_ST0,
  /* DC F2 */ BX_IA_FDIVR_STi_ST0,
  /* DC F3 */ BX_IA_FDIVR_STi_ST0,
  /* DC F4 */ BX_IA_FDIVR_STi_ST0,
  /* DC F5 */ BX_IA_FDIVR_STi_ST0,
  /* DC F6 */ BX_IA_FDIVR_STi_ST0,
  /* DC F7 */ BX_IA_FDIVR_STi_ST0,
  /* DC F8 */ BX_IA_FDIV_STi_ST0,
  /* DC F9 */ BX_IA_FDIV_STi_ST0,
  /* DC FA */ BX_IA_FDIV_STi_ST0,
  /* DC FB */ BX_IA_FDIV_STi_ST0,
  /* DC FC */ BX_IA_FDIV_STi_ST0,
  /* DC FD */ BX_IA_FDIV_STi_ST0,
  /* DC FE */ BX_IA_FDIV_STi_ST0,
  /* DC FF */ BX_IA_FDIV_STi_ST0
};

// DD (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDD[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FLD_DOUBLE_REAL,
  /* 1 */ BX_IA_FISTTP_Mq,
  /* 2 */ BX_IA_FST_DOUBLE_REAL,
  /* 3 */ BX_IA_FSTP_DOUBLE_REAL,
  /* 4 */ BX_IA_FRSTOR,
  /* 5 */ BX_IA_ERROR,
  /* 6 */ BX_IA_FNSAVE,
  /* 7 */ BX_IA_FNSTSW,

  /* /r form */
  /* DD C0 */ BX_IA_FFREE_STi,
  /* DD C1 */ BX_IA_FFREE_STi,
  /* DD C2 */ BX_IA_FFREE_STi,
  /* DD C3 */ BX_IA_FFREE_STi,
  /* DD C4 */ BX_IA_FFREE_STi,
  /* DD C5 */ BX_IA_FFREE_STi,
  /* DD C6 */ BX_IA_FFREE_STi,
  /* DD C7 */ BX_IA_FFREE_STi,
  /* DD C8 */ BX_IA_FXCH_STi,               // undocumented
  /* DD C9 */ BX_IA_FXCH_STi,               // undocumented
  /* DD CA */ BX_IA_FXCH_STi,               // undocumented
  /* DD CB */ BX_IA_FXCH_STi,               // undocumented
  /* DD CC */ BX_IA_FXCH_STi,               // undocumented
  /* DD CD */ BX_IA_FXCH_STi,               // undocumented
  /* DD CE */ BX_IA_FXCH_STi,               // undocumented
  /* DD CF */ BX_IA_FXCH_STi,               // undocumented
  /* DD D0 */ BX_IA_FST_STi,
  /* DD D1 */ BX_IA_FST_STi,
  /* DD D2 */ BX_IA_FST_STi,
  /* DD D3 */ BX_IA_FST_STi,
  /* DD D4 */ BX_IA_FST_STi,
  /* DD D5 */ BX_IA_FST_STi,
  /* DD D6 */ BX_IA_FST_STi,
  /* DD D7 */ BX_IA_FST_STi,
  /* DD D8 */ BX_IA_FSTP_STi,
  /* DD D9 */ BX_IA_FSTP_STi,
  /* DD DA */ BX_IA_FSTP_STi,
  /* DD DB */ BX_IA_FSTP_STi,
  /* DD DC */ BX_IA_FSTP_STi,
  /* DD DD */ BX_IA_FSTP_STi,
  /* DD DE */ BX_IA_FSTP_STi,
  /* DD DF */ BX_IA_FSTP_STi,
  /* DD E0 */ BX_IA_FUCOM_STi,
  /* DD E1 */ BX_IA_FUCOM_STi,
  /* DD E2 */ BX_IA_FUCOM_STi,
  /* DD E3 */ BX_IA_FUCOM_STi,
  /* DD E4 */ BX_IA_FUCOM_STi,
  /* DD E5 */ BX_IA_FUCOM_STi,
  /* DD E6 */ BX_IA_FUCOM_STi,
  /* DD E7 */ BX_IA_FUCOM_STi,
  /* DD E8 */ BX_IA_FUCOMP_STi,
  /* DD E9 */ BX_IA_FUCOMP_STi,
  /* DD EA */ BX_IA_FUCOMP_STi,
  /* DD EB */ BX_IA_FUCOMP_STi,
  /* DD EC */ BX_IA_FUCOMP_STi,
  /* DD ED */ BX_IA_FUCOMP_STi,
  /* DD EE */ BX_IA_FUCOMP_STi,
  /* DD EF */ BX_IA_FUCOMP_STi,
  /* DD F0 */ BX_IA_ERROR,
  /* DD F1 */ BX_IA_ERROR,
  /* DD F2 */ BX_IA_ERROR,
  /* DD F3 */ BX_IA_ERROR,
  /* DD F4 */ BX_IA_ERROR,
  /* DD F5 */ BX_IA_ERROR,
  /* DD F6 */ BX_IA_ERROR,
  /* DD F7 */ BX_IA_ERROR,
  /* DD F8 */ BX_IA_ERROR,
  /* DD F9 */ BX_IA_ERROR,
  /* DD FA */ BX_IA_ERROR,
  /* DD FB */ BX_IA_ERROR,
  /* DD FC */ BX_IA_ERROR,
  /* DD FD */ BX_IA_ERROR,
  /* DD FE */ BX_IA_ERROR,
  /* DD FF */ BX_IA_ERROR
};

// DE (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDE[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FIADD_WORD_INTEGER,
  /* 1 */ BX_IA_FIMUL_WORD_INTEGER,
  /* 2 */ BX_IA_FICOM_WORD_INTEGER,
  /* 3 */ BX_IA_FICOMP_WORD_INTEGER,
  /* 4 */ BX_IA_FISUB_WORD_INTEGER,
  /* 5 */ BX_IA_FISUBR_WORD_INTEGER,
  /* 6 */ BX_IA_FIDIV_WORD_INTEGER,
  /* 7 */ BX_IA_FIDIVR_WORD_INTEGER,

  /* /r form */ // all instructions pop FPU stack
  /* DE C0 */ BX_IA_FADDP_STi_ST0,
  /* DE C1 */ BX_IA_FADDP_STi_ST0,
  /* DE C2 */ BX_IA_FADDP_STi_ST0,
  /* DE C3 */ BX_IA_FADDP_STi_ST0,
  /* DE C4 */ BX_IA_FADDP_STi_ST0,
  /* DE C5 */ BX_IA_FADDP_STi_ST0,
  /* DE C6 */ BX_IA_FADDP_STi_ST0,
  /* DE C7 */ BX_IA_FADDP_STi_ST0,
  /* DE C8 */ BX_IA_FMULP_STi_ST0,
  /* DE C9 */ BX_IA_FMULP_STi_ST0,
  /* DE CA */ BX_IA_FMULP_STi_ST0,
  /* DE CB */ BX_IA_FMULP_STi_ST0,
  /* DE CC */ BX_IA_FMULP_STi_ST0,
  /* DE CD */ BX_IA_FMULP_STi_ST0,
  /* DE CE */ BX_IA_FMULP_STi_ST0,
  /* DE CF */ BX_IA_FMULP_STi_ST0,
  /* DE D0 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D1 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D2 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D3 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D4 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D5 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D6 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D7 */ BX_IA_FCOMP_STi,           // undocumented, special FPSTACK pop case
  /* DE D8 */ BX_IA_ERROR,
  /* DE D9 */ BX_IA_FCOMPP,
  /* DE DA */ BX_IA_ERROR,
  /* DE DB */ BX_IA_ERROR,
  /* DE DC */ BX_IA_ERROR,
  /* DE DD */ BX_IA_ERROR,
  /* DE DE */ BX_IA_ERROR,
  /* DE DF */ BX_IA_ERROR,
  /* DE E0 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E1 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E2 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E3 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E4 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E5 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E6 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E7 */ BX_IA_FSUBRP_STi_ST0,
  /* DE E8 */ BX_IA_FSUBP_STi_ST0,
  /* DE E9 */ BX_IA_FSUBP_STi_ST0,
  /* DE EA */ BX_IA_FSUBP_STi_ST0,
  /* DE EB */ BX_IA_FSUBP_STi_ST0,
  /* DE EC */ BX_IA_FSUBP_STi_ST0,
  /* DE ED */ BX_IA_FSUBP_STi_ST0,
  /* DE EE */ BX_IA_FSUBP_STi_ST0,
  /* DE EF */ BX_IA_FSUBP_STi_ST0,
  /* DE F0 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F1 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F2 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F3 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F4 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F5 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F6 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F7 */ BX_IA_FDIVRP_STi_ST0,
  /* DE F8 */ BX_IA_FDIVP_STi_ST0,
  /* DE F9 */ BX_IA_FDIVP_STi_ST0,
  /* DE FA */ BX_IA_FDIVP_STi_ST0,
  /* DE FB */ BX_IA_FDIVP_STi_ST0,
  /* DE FC */ BX_IA_FDIVP_STi_ST0,
  /* DE FD */ BX_IA_FDIVP_STi_ST0,
  /* DE FE */ BX_IA_FDIVP_STi_ST0,
  /* DE FF */ BX_IA_FDIVP_STi_ST0
};

// DF (modrm is outside 00h - BFh)
static const Bit16u BxOpcodeInfo_FloatingPointDF[64+8] = {
  /* /m form */
  /* 0 */ BX_IA_FILD_WORD_INTEGER,
  /* 1 */ BX_IA_FISTTP_Mw,
  /* 2 */ BX_IA_FIST_WORD_INTEGER,
  /* 3 */ BX_IA_FISTP_WORD_INTEGER,
  /* 4 */ BX_IA_FBLD_PACKED_BCD,
  /* 5 */ BX_IA_FILD_QWORD_INTEGER,
  /* 6 */ BX_IA_FBSTP_PACKED_BCD,
  /* 7 */ BX_IA_FISTP_QWORD_INTEGER,

  /* /r form */
  /* DF C0 */ BX_IA_FFREEP_STi,  // 287+ compatibility opcode
  /* DF C1 */ BX_IA_FFREEP_STi,
  /* DF C2 */ BX_IA_FFREEP_STi,
  /* DF C3 */ BX_IA_FFREEP_STi,
  /* DF C4 */ BX_IA_FFREEP_STi,
  /* DF C5 */ BX_IA_FFREEP_STi,
  /* DF C6 */ BX_IA_FFREEP_STi,
  /* DF C7 */ BX_IA_FFREEP_STi,
  /* DF C8 */ BX_IA_FXCH_STi,    // undocumented
  /* DF C9 */ BX_IA_FXCH_STi,    // undocumented
  /* DF CA */ BX_IA_FXCH_STi,    // undocumented
  /* DF CB */ BX_IA_FXCH_STi,    // undocumented
  /* DF CC */ BX_IA_FXCH_STi,    // undocumented
  /* DF CD */ BX_IA_FXCH_STi,    // undocumented
  /* DF CE */ BX_IA_FXCH_STi,    // undocumented
  /* DF CF */ BX_IA_FXCH_STi,    // undocumented
  /* DF D0 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D1 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D2 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D3 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D4 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D5 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D6 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D7 */ BX_IA_FSTP_STi,	 // undocumented, special FPSTACK pop case
  /* DF D8 */ BX_IA_FSTP_STi,	 // undocumented
  /* DF D9 */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DA */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DB */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DC */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DD */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DE */ BX_IA_FSTP_STi,	 // undocumented
  /* DF DF */ BX_IA_FSTP_STi,	 // undocumented
  /* DF E0 */ BX_IA_FNSTSW_AX,
  /* DF E1 */ BX_IA_ERROR,
  /* DF E2 */ BX_IA_ERROR,
  /* DF E3 */ BX_IA_ERROR,
  /* DF E4 */ BX_IA_ERROR,
  /* DF E5 */ BX_IA_ERROR,
  /* DF E6 */ BX_IA_ERROR,
  /* DF E7 */ BX_IA_ERROR,
  /* DF E8 */ BX_IA_FUCOMIP_ST0_STj,
  /* DF E9 */ BX_IA_FUCOMIP_ST0_STj,
  /* DF EA */ BX_IA_FUCOMIP_ST0_STj,
  /* DF EB */ BX_IA_FUCOMIP_ST0_STj,
  /* DF EC */ BX_IA_FUCOMIP_ST0_STj,
  /* DF ED */ BX_IA_FUCOMIP_ST0_STj,
  /* DF EE */ BX_IA_FUCOMIP_ST0_STj,
  /* DF EF */ BX_IA_FUCOMIP_ST0_STj,
  /* DF F0 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F1 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F2 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F3 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F4 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F5 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F6 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F7 */ BX_IA_FCOMIP_ST0_STj,
  /* DF F8 */ BX_IA_ERROR,
  /* DF F9 */ BX_IA_ERROR,
  /* DF FA */ BX_IA_ERROR,
  /* DF FB */ BX_IA_ERROR,
  /* DF FC */ BX_IA_ERROR,
  /* DF FD */ BX_IA_ERROR,
  /* DF FE */ BX_IA_ERROR,
  /* DF FF */ BX_IA_ERROR,
};

#endif

/* ************************************************************************ */
/* 3DNow! Opcodes */

#if BX_SUPPORT_3DNOW

static Bit16u Bx3DNowOpcode[256] = {
  // 256 entries for 3DNow opcodes, by suffix
  /* 00 */ BX_IA_ERROR,
  /* 01 */ BX_IA_ERROR,
  /* 02 */ BX_IA_ERROR,
  /* 03 */ BX_IA_ERROR,
  /* 04 */ BX_IA_ERROR,
  /* 05 */ BX_IA_ERROR,
  /* 06 */ BX_IA_ERROR,
  /* 07 */ BX_IA_ERROR,
  /* 08 */ BX_IA_ERROR,
  /* 09 */ BX_IA_ERROR,
  /* 0A */ BX_IA_ERROR,
  /* 0B */ BX_IA_ERROR,
  /* 0C */ BX_IA_PI2FW_PqQq,
  /* 0D */ BX_IA_PI2FD_PqQq,
  /* 0E */ BX_IA_ERROR,
  /* 0F */ BX_IA_ERROR,
  /* 10 */ BX_IA_ERROR,
  /* 11 */ BX_IA_ERROR,
  /* 12 */ BX_IA_ERROR,
  /* 13 */ BX_IA_ERROR,
  /* 14 */ BX_IA_ERROR,
  /* 15 */ BX_IA_ERROR,
  /* 16 */ BX_IA_ERROR,
  /* 17 */ BX_IA_ERROR,
  /* 18 */ BX_IA_ERROR,
  /* 19 */ BX_IA_ERROR,
  /* 1A */ BX_IA_ERROR,
  /* 1B */ BX_IA_ERROR,
  /* 1C */ BX_IA_PF2IW_PqQq,
  /* 1D */ BX_IA_PF2ID_PqQq,
  /* 1E */ BX_IA_ERROR,
  /* 1F */ BX_IA_ERROR,
  /* 20 */ BX_IA_ERROR,
  /* 21 */ BX_IA_ERROR,
  /* 22 */ BX_IA_ERROR,
  /* 23 */ BX_IA_ERROR,
  /* 24 */ BX_IA_ERROR,
  /* 25 */ BX_IA_ERROR,
  /* 26 */ BX_IA_ERROR,
  /* 27 */ BX_IA_ERROR,
  /* 28 */ BX_IA_ERROR,
  /* 29 */ BX_IA_ERROR,
  /* 2A */ BX_IA_ERROR,
  /* 2B */ BX_IA_ERROR,
  /* 2C */ BX_IA_ERROR,
  /* 2D */ BX_IA_ERROR,
  /* 2E */ BX_IA_ERROR,
  /* 2F */ BX_IA_ERROR,
  /* 30 */ BX_IA_ERROR,
  /* 31 */ BX_IA_ERROR,
  /* 32 */ BX_IA_ERROR,
  /* 33 */ BX_IA_ERROR,
  /* 34 */ BX_IA_ERROR,
  /* 35 */ BX_IA_ERROR,
  /* 36 */ BX_IA_ERROR,
  /* 37 */ BX_IA_ERROR,
  /* 38 */ BX_IA_ERROR,
  /* 39 */ BX_IA_ERROR,
  /* 3A */ BX_IA_ERROR,
  /* 3B */ BX_IA_ERROR,
  /* 3C */ BX_IA_ERROR,
  /* 3D */ BX_IA_ERROR,
  /* 3E */ BX_IA_ERROR,
  /* 3F */ BX_IA_ERROR,
  /* 40 */ BX_IA_ERROR,
  /* 41 */ BX_IA_ERROR,
  /* 42 */ BX_IA_ERROR,
  /* 43 */ BX_IA_ERROR,
  /* 44 */ BX_IA_ERROR,
  /* 45 */ BX_IA_ERROR,
  /* 46 */ BX_IA_ERROR,
  /* 47 */ BX_IA_ERROR,
  /* 48 */ BX_IA_ERROR,
  /* 49 */ BX_IA_ERROR,
  /* 4A */ BX_IA_ERROR,
  /* 4B */ BX_IA_ERROR,
  /* 4C */ BX_IA_ERROR,
  /* 4D */ BX_IA_ERROR,
  /* 4E */ BX_IA_ERROR,
  /* 4F */ BX_IA_ERROR,
  /* 50 */ BX_IA_ERROR,
  /* 51 */ BX_IA_ERROR,
  /* 52 */ BX_IA_ERROR,
  /* 53 */ BX_IA_ERROR,
  /* 54 */ BX_IA_ERROR,
  /* 55 */ BX_IA_ERROR,
  /* 56 */ BX_IA_ERROR,
  /* 57 */ BX_IA_ERROR,
  /* 58 */ BX_IA_ERROR,
  /* 59 */ BX_IA_ERROR,
  /* 5A */ BX_IA_ERROR,
  /* 5B */ BX_IA_ERROR,
  /* 5C */ BX_IA_ERROR,
  /* 5D */ BX_IA_ERROR,
  /* 5E */ BX_IA_ERROR,
  /* 5F */ BX_IA_ERROR,
  /* 60 */ BX_IA_ERROR,
  /* 61 */ BX_IA_ERROR,
  /* 62 */ BX_IA_ERROR,
  /* 63 */ BX_IA_ERROR,
  /* 64 */ BX_IA_ERROR,
  /* 65 */ BX_IA_ERROR,
  /* 66 */ BX_IA_ERROR,
  /* 67 */ BX_IA_ERROR,
  /* 68 */ BX_IA_ERROR,
  /* 69 */ BX_IA_ERROR,
  /* 6A */ BX_IA_ERROR,
  /* 6B */ BX_IA_ERROR,
  /* 6C */ BX_IA_ERROR,
  /* 6D */ BX_IA_ERROR,
  /* 6E */ BX_IA_ERROR,
  /* 6F */ BX_IA_ERROR,
  /* 70 */ BX_IA_ERROR,
  /* 71 */ BX_IA_ERROR,
  /* 72 */ BX_IA_ERROR,
  /* 73 */ BX_IA_ERROR,
  /* 74 */ BX_IA_ERROR,
  /* 75 */ BX_IA_ERROR,
  /* 76 */ BX_IA_ERROR,
  /* 77 */ BX_IA_ERROR,
  /* 78 */ BX_IA_ERROR,
  /* 79 */ BX_IA_ERROR,
  /* 7A */ BX_IA_ERROR,
  /* 7B */ BX_IA_ERROR,
  /* 7C */ BX_IA_ERROR,
  /* 7D */ BX_IA_ERROR,
  /* 7E */ BX_IA_ERROR,
  /* 7F */ BX_IA_ERROR,
  /* 80 */ BX_IA_ERROR,
  /* 81 */ BX_IA_ERROR,
  /* 82 */ BX_IA_ERROR,
  /* 83 */ BX_IA_ERROR,
  /* 84 */ BX_IA_ERROR,
  /* 85 */ BX_IA_ERROR,
  /* 86 */ BX_IA_ERROR,
  /* 87 */ BX_IA_ERROR,
  /* 88 */ BX_IA_ERROR,
  /* 89 */ BX_IA_ERROR,
  /* 8A */ BX_IA_PFNACC_PqQq,
  /* 8B */ BX_IA_ERROR,
  /* 8C */ BX_IA_ERROR,
  /* 8D */ BX_IA_ERROR,
  /* 8E */ BX_IA_PFPNACC_PqQq,
  /* 8F */ BX_IA_ERROR,
  /* 90 */ BX_IA_PFCMPGE_PqQq,
  /* 91 */ BX_IA_ERROR,
  /* 92 */ BX_IA_ERROR,
  /* 93 */ BX_IA_ERROR,
  /* 94 */ BX_IA_PFMIN_PqQq,
  /* 95 */ BX_IA_ERROR,
  /* 96 */ BX_IA_PFRCP_PqQq,
  /* 97 */ BX_IA_PFRSQRT_PqQq,
  /* 98 */ BX_IA_ERROR,
  /* 99 */ BX_IA_ERROR,
  /* 9A */ BX_IA_PFSUB_PqQq,
  /* 9B */ BX_IA_ERROR,
  /* 9C */ BX_IA_ERROR,
  /* 9D */ BX_IA_ERROR,
  /* 9E */ BX_IA_PFADD_PqQq,
  /* 9F */ BX_IA_ERROR,
  /* A0 */ BX_IA_PFCMPGT_PqQq,
  /* A1 */ BX_IA_ERROR,
  /* A2 */ BX_IA_ERROR,
  /* A3 */ BX_IA_ERROR,
  /* A4 */ BX_IA_PFMAX_PqQq,
  /* A5 */ BX_IA_ERROR,
  /* A6 */ BX_IA_PFRCPIT1_PqQq,
  /* A7 */ BX_IA_PFRSQIT1_PqQq,
  /* A8 */ BX_IA_ERROR,
  /* A9 */ BX_IA_ERROR,
  /* AA */ BX_IA_PFSUBR_PqQq,
  /* AB */ BX_IA_ERROR,
  /* AC */ BX_IA_ERROR,
  /* AD */ BX_IA_ERROR,
  /* AE */ BX_IA_PFACC_PqQq,
  /* AF */ BX_IA_ERROR,
  /* B0 */ BX_IA_PFCMPEQ_PqQq,
  /* B1 */ BX_IA_ERROR,
  /* B2 */ BX_IA_ERROR,
  /* B3 */ BX_IA_ERROR,
  /* B4 */ BX_IA_PFMUL_PqQq,
  /* B5 */ BX_IA_ERROR,
  /* B6 */ BX_IA_PFRCPIT2_PqQq,
  /* B7 */ BX_IA_PMULHRW_PqQq,
  /* B8 */ BX_IA_ERROR,
  /* B9 */ BX_IA_ERROR,
  /* BA */ BX_IA_ERROR,
  /* BB */ BX_IA_PSWAPD_PqQq,
  /* BC */ BX_IA_ERROR,
  /* BD */ BX_IA_ERROR,
  /* BE */ BX_IA_ERROR,
  /* BF */ BX_IA_PAVGB_PqQq,
  /* C0 */ BX_IA_ERROR,
  /* C1 */ BX_IA_ERROR,
  /* C2 */ BX_IA_ERROR,
  /* C3 */ BX_IA_ERROR,
  /* C4 */ BX_IA_ERROR,
  /* C5 */ BX_IA_ERROR,
  /* C6 */ BX_IA_ERROR,
  /* C7 */ BX_IA_ERROR,
  /* C8 */ BX_IA_ERROR,
  /* C9 */ BX_IA_ERROR,
  /* CA */ BX_IA_ERROR,
  /* CB */ BX_IA_ERROR,
  /* CC */ BX_IA_ERROR,
  /* CD */ BX_IA_ERROR,
  /* CE */ BX_IA_ERROR,
  /* CF */ BX_IA_ERROR,
  /* D0 */ BX_IA_ERROR,
  /* D1 */ BX_IA_ERROR,
  /* D2 */ BX_IA_ERROR,
  /* D3 */ BX_IA_ERROR,
  /* D4 */ BX_IA_ERROR,
  /* D5 */ BX_IA_ERROR,
  /* D6 */ BX_IA_ERROR,
  /* D7 */ BX_IA_ERROR,
  /* D8 */ BX_IA_ERROR,
  /* D9 */ BX_IA_ERROR,
  /* DA */ BX_IA_ERROR,
  /* DB */ BX_IA_ERROR,
  /* DC */ BX_IA_ERROR,
  /* DD */ BX_IA_ERROR,
  /* DE */ BX_IA_ERROR,
  /* DF */ BX_IA_ERROR,
  /* E0 */ BX_IA_ERROR,
  /* E1 */ BX_IA_ERROR,
  /* E2 */ BX_IA_ERROR,
  /* E3 */ BX_IA_ERROR,
  /* E4 */ BX_IA_ERROR,
  /* E5 */ BX_IA_ERROR,
  /* E6 */ BX_IA_ERROR,
  /* E7 */ BX_IA_ERROR,
  /* E8 */ BX_IA_ERROR,
  /* E9 */ BX_IA_ERROR,
  /* EA */ BX_IA_ERROR,
  /* EB */ BX_IA_ERROR,
  /* EC */ BX_IA_ERROR,
  /* ED */ BX_IA_ERROR,
  /* EE */ BX_IA_ERROR,
  /* EF */ BX_IA_ERROR,
  /* F0 */ BX_IA_ERROR,
  /* F1 */ BX_IA_ERROR,
  /* F2 */ BX_IA_ERROR,
  /* F3 */ BX_IA_ERROR,
  /* F4 */ BX_IA_ERROR,
  /* F5 */ BX_IA_ERROR,
  /* F6 */ BX_IA_ERROR,
  /* F7 */ BX_IA_ERROR,
  /* F8 */ BX_IA_ERROR,
  /* F9 */ BX_IA_ERROR,
  /* FA */ BX_IA_ERROR,
  /* FB */ BX_IA_ERROR,
  /* FC */ BX_IA_ERROR,
  /* FD */ BX_IA_ERROR,
  /* FE */ BX_IA_ERROR,
  /* FF */ BX_IA_ERROR
};

#endif

#endif // BX_X87_FETCHDECODE_TABLES_H
