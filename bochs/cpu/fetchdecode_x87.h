/////////////////////////////////////////////////////////////////////////
// $Id$
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

#ifndef BX_X87_FETCHDECODE_TABLES_H
#define BX_X87_FETCHDECODE_TABLES_H

//
// Common FetchDecode Opcode Tables - x87 and 3dnow!
//

#if BX_SUPPORT_FPU

/* ************************************************************************ */
/* FPU Opcodes */

// D8 (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupD8[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_FADD_ST0_STj },
  /* 1 */ { 0, BX_IA_FMUL_ST0_STj },
  /* 2 */ { 0, BX_IA_FCOM_STi },
  /* 3 */ { 0, BX_IA_FCOMP_STi },
  /* 4 */ { 0, BX_IA_FSUB_ST0_STj },
  /* 5 */ { 0, BX_IA_FSUBR_ST0_STj },
  /* 6 */ { 0, BX_IA_FDIV_ST0_STj },
  /* 7 */ { 0, BX_IA_FDIVR_ST0_STj },

  /* /m form */
  /* 0 */ { 0, BX_IA_FADD_SINGLE_REAL },
  /* 1 */ { 0, BX_IA_FMUL_SINGLE_REAL },
  /* 2 */ { 0, BX_IA_FCOM_SINGLE_REAL },
  /* 3 */ { 0, BX_IA_FCOMP_SINGLE_REAL },
  /* 4 */ { 0, BX_IA_FSUB_SINGLE_REAL },
  /* 5 */ { 0, BX_IA_FSUBR_SINGLE_REAL },
  /* 6 */ { 0, BX_IA_FDIV_SINGLE_REAL },
  /* 7 */ { 0, BX_IA_FDIVR_SINGLE_REAL }
};

// D9 (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPointD9[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_FLD_SINGLE_REAL },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_FST_SINGLE_REAL },
  /* 3 */ { 0, BX_IA_FSTP_SINGLE_REAL },
  /* 4 */ { 0, BX_IA_FLDENV },
  /* 5 */ { 0, BX_IA_FLDCW },
  /* 6 */ { 0, BX_IA_FNSTENV },
  /* 7 */ { 0, BX_IA_FNSTCW },

  /* /r form */
  /* D9 C0 */ { 0, BX_IA_FLD_STi },
  /* D9 C1 */ { 0, BX_IA_FLD_STi },
  /* D9 C2 */ { 0, BX_IA_FLD_STi },
  /* D9 C3 */ { 0, BX_IA_FLD_STi },
  /* D9 C4 */ { 0, BX_IA_FLD_STi },
  /* D9 C5 */ { 0, BX_IA_FLD_STi },
  /* D9 C6 */ { 0, BX_IA_FLD_STi },
  /* D9 C7 */ { 0, BX_IA_FLD_STi },
  /* D9 C8 */ { 0, BX_IA_FXCH_STi },
  /* D9 C9 */ { 0, BX_IA_FXCH_STi },
  /* D9 CA */ { 0, BX_IA_FXCH_STi },
  /* D9 CB */ { 0, BX_IA_FXCH_STi },
  /* D9 CC */ { 0, BX_IA_FXCH_STi },
  /* D9 CD */ { 0, BX_IA_FXCH_STi },
  /* D9 CE */ { 0, BX_IA_FXCH_STi },
  /* D9 CF */ { 0, BX_IA_FXCH_STi },
  /* D9 D0 */ { 0, BX_IA_FNOP },
  /* D9 D1 */ { 0, BX_IA_ERROR },
  /* D9 D2 */ { 0, BX_IA_ERROR },
  /* D9 D3 */ { 0, BX_IA_ERROR },
  /* D9 D4 */ { 0, BX_IA_ERROR },
  /* D9 D5 */ { 0, BX_IA_ERROR },
  /* D9 D6 */ { 0, BX_IA_ERROR },
  /* D9 D7 */ { 0, BX_IA_ERROR },
  /* D9 D8 */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 D9 */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DA */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DB */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DC */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DD */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DE */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 DF */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* D9 E0 */ { 0, BX_IA_FCHS },
  /* D9 E1 */ { 0, BX_IA_FABS },
  /* D9 E2 */ { 0, BX_IA_ERROR },
  /* D9 E3 */ { 0, BX_IA_ERROR },
  /* D9 E4 */ { 0, BX_IA_FTST },
  /* D9 E5 */ { 0, BX_IA_FXAM },
  /* D9 E6 */ { 0, BX_IA_ERROR },
  /* D9 E7 */ { 0, BX_IA_ERROR },
  /* D9 E8 */ { 0, BX_IA_FLD1 },
  /* D9 E9 */ { 0, BX_IA_FLDL2T },
  /* D9 EA */ { 0, BX_IA_FLDL2E },
  /* D9 EB */ { 0, BX_IA_FLDPI },
  /* D9 EC */ { 0, BX_IA_FLDLG2 },
  /* D9 ED */ { 0, BX_IA_FLDLN2 },
  /* D9 EE */ { 0, BX_IA_FLDZ },
  /* D9 EF */ { 0, BX_IA_ERROR },
  /* D9 F0 */ { 0, BX_IA_F2XM1 },
  /* D9 F1 */ { 0, BX_IA_FYL2X },
  /* D9 F2 */ { 0, BX_IA_FPTAN },
  /* D9 F3 */ { 0, BX_IA_FPATAN },
  /* D9 F4 */ { 0, BX_IA_FXTRACT },
  /* D9 F5 */ { 0, BX_IA_FPREM1 },
  /* D9 F6 */ { 0, BX_IA_FDECSTP },
  /* D9 F7 */ { 0, BX_IA_FINCSTP },
  /* D9 F8 */ { 0, BX_IA_FPREM },
  /* D9 F9 */ { 0, BX_IA_FYL2XP1 },
  /* D9 FA */ { 0, BX_IA_FSQRT },
  /* D9 FB */ { 0, BX_IA_FSINCOS },
  /* D9 FC */ { 0, BX_IA_FRNDINT },
  /* D9 FD */ { 0, BX_IA_FSCALE },
  /* D9 FE */ { 0, BX_IA_FSIN },
  /* D9 FF */ { 0, BX_IA_FCOS }
};

// DA (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPointDA[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_FIADD_DWORD_INTEGER },
  /* 1 */ { 0, BX_IA_FIMUL_DWORD_INTEGER },
  /* 2 */ { 0, BX_IA_FICOM_DWORD_INTEGER },
  /* 3 */ { 0, BX_IA_FICOMP_DWORD_INTEGER },
  /* 4 */ { 0, BX_IA_FISUB_DWORD_INTEGER },
  /* 5 */ { 0, BX_IA_FISUBR_DWORD_INTEGER },
  /* 6 */ { 0, BX_IA_FIDIV_DWORD_INTEGER },
  /* 7 */ { 0, BX_IA_FIDIVR_DWORD_INTEGER },

  /* /r form */
  /* DA C0 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C1 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C2 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C3 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C4 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C5 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C6 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C7 */ { 0, BX_IA_FCMOVB_ST0_STj },
  /* DA C8 */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA C9 */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CA */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CB */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CC */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CD */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CE */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA CF */ { 0, BX_IA_FCMOVE_ST0_STj },
  /* DA D0 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D1 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D2 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D3 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D4 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D5 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D6 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D7 */ { 0, BX_IA_FCMOVBE_ST0_STj },
  /* DA D8 */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA D9 */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DA */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DB */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DC */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DD */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DE */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA DF */ { 0, BX_IA_FCMOVU_ST0_STj },
  /* DA E0 */ { 0, BX_IA_ERROR },
  /* DA E1 */ { 0, BX_IA_ERROR },
  /* DA E2 */ { 0, BX_IA_ERROR },
  /* DA E3 */ { 0, BX_IA_ERROR },
  /* DA E4 */ { 0, BX_IA_ERROR },
  /* DA E5 */ { 0, BX_IA_ERROR },
  /* DA E6 */ { 0, BX_IA_ERROR },
  /* DA E7 */ { 0, BX_IA_ERROR },
  /* DA E8 */ { 0, BX_IA_ERROR },
  /* DA E9 */ { 0, BX_IA_FUCOMPP },
  /* DA EA */ { 0, BX_IA_ERROR },
  /* DA EB */ { 0, BX_IA_ERROR },
  /* DA EC */ { 0, BX_IA_ERROR },
  /* DA ED */ { 0, BX_IA_ERROR },
  /* DA EE */ { 0, BX_IA_ERROR },
  /* DA EF */ { 0, BX_IA_ERROR },
  /* DA F0 */ { 0, BX_IA_ERROR },
  /* DA F1 */ { 0, BX_IA_ERROR },
  /* DA F2 */ { 0, BX_IA_ERROR },
  /* DA F3 */ { 0, BX_IA_ERROR },
  /* DA F4 */ { 0, BX_IA_ERROR },
  /* DA F5 */ { 0, BX_IA_ERROR },
  /* DA F6 */ { 0, BX_IA_ERROR },
  /* DA F7 */ { 0, BX_IA_ERROR },
  /* DA F8 */ { 0, BX_IA_ERROR },
  /* DA F9 */ { 0, BX_IA_ERROR },
  /* DA FA */ { 0, BX_IA_ERROR },
  /* DA FB */ { 0, BX_IA_ERROR },
  /* DA FC */ { 0, BX_IA_ERROR },
  /* DA FD */ { 0, BX_IA_ERROR },
  /* DA FE */ { 0, BX_IA_ERROR },
  /* DA FF */ { 0, BX_IA_ERROR }
};

// DB (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPointDB[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_FILD_DWORD_INTEGER },
  /* 1 */ { 0, BX_IA_FISTTP32 },
  /* 2 */ { 0, BX_IA_FIST_DWORD_INTEGER },
  /* 3 */ { 0, BX_IA_FISTP_DWORD_INTEGER },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_FLD_EXTENDED_REAL },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_FSTP_EXTENDED_REAL },

  /* /r form */
  /* DB C0 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C1 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C2 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C3 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C4 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C5 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C6 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C7 */ { 0, BX_IA_FCMOVNB_ST0_STj },
  /* DB C8 */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB C9 */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CA */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CB */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CC */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CD */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CE */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB CF */ { 0, BX_IA_FCMOVNE_ST0_STj },
  /* DB D0 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D1 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D2 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D3 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D4 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D5 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D6 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D7 */ { 0, BX_IA_FCMOVNBE_ST0_STj },
  /* DB D8 */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB D9 */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DA */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DB */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DC */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DD */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DE */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB DF */ { 0, BX_IA_FCMOVNU_ST0_STj },
  /* DB E0 */ { 0, BX_IA_FPLEGACY },      // feni  (287 only)
  /* DB E1 */ { 0, BX_IA_FPLEGACY },      // fdisi (287 only)
  /* DB E2 */ { 0, BX_IA_FNCLEX },
  /* DB E3 */ { 0, BX_IA_FNINIT },
  /* DB E4 */ { 0, BX_IA_FPLEGACY },      // fsetpm (287 only)
  /* DB E5 */ { 0, BX_IA_ERROR },
  /* DB E6 */ { 0, BX_IA_ERROR },
  /* DB E7 */ { 0, BX_IA_ERROR },
  /* DB E8 */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB E9 */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EA */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EB */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EC */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB ED */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EE */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EF */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB F0 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F1 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F2 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F3 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F4 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F5 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F6 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F7 */ { 0, BX_IA_FCOMI_ST0_STj },
  /* DB F8 */ { 0, BX_IA_ERROR },
  /* DB F9 */ { 0, BX_IA_ERROR },
  /* DB FA */ { 0, BX_IA_ERROR },
  /* DB FB */ { 0, BX_IA_ERROR },
  /* DB FC */ { 0, BX_IA_ERROR },
  /* DB FD */ { 0, BX_IA_ERROR },
  /* DB FE */ { 0, BX_IA_ERROR },
  /* DB FF */ { 0, BX_IA_ERROR }
};

// DC (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDC[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_FADD_STi_ST0 },
  /* 1 */ { 0, BX_IA_FMUL_STi_ST0 },
  /* 2 */ { 0, BX_IA_FCOM_STi },                 // undocumented
  /* 3 */ { 0, BX_IA_FCOMP_STi },                // undocumented
  /* 4 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* 5 */ { 0, BX_IA_FSUB_STi_ST0 },
  /* 6 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* 7 */ { 0, BX_IA_FDIV_STi_ST0 },

  /* /m form */
  /* 0 */ { 0, BX_IA_FADD_DOUBLE_REAL },
  /* 1 */ { 0, BX_IA_FMUL_DOUBLE_REAL },
  /* 2 */ { 0, BX_IA_FCOM_DOUBLE_REAL },
  /* 3 */ { 0, BX_IA_FCOMP_DOUBLE_REAL },
  /* 4 */ { 0, BX_IA_FSUB_DOUBLE_REAL },
  /* 5 */ { 0, BX_IA_FSUBR_DOUBLE_REAL },
  /* 6 */ { 0, BX_IA_FDIV_DOUBLE_REAL },
  /* 7 */ { 0, BX_IA_FDIVR_DOUBLE_REAL }
};

// DD (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDD[8*2] = {
  /* /r form */
  /* 0 */ { 0, BX_IA_FFREE_STi },
  /* 1 */ { 0, BX_IA_FXCH_STi },               // undocumented
  /* 2 */ { 0, BX_IA_FST_STi },
  /* 3 */ { 0, BX_IA_FSTP_STi },
  /* 4 */ { 0, BX_IA_FUCOM_STi },
  /* 5 */ { 0, BX_IA_FUCOMP_STi },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR },

  /* /m form */
  /* 0 */ { 0, BX_IA_FLD_DOUBLE_REAL },
  /* 1 */ { 0, BX_IA_FISTTP64 },
  /* 2 */ { 0, BX_IA_FST_DOUBLE_REAL },
  /* 3 */ { 0, BX_IA_FSTP_DOUBLE_REAL },
  /* 4 */ { 0, BX_IA_FRSTOR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_FNSAVE },
  /* 7 */ { 0, BX_IA_FNSTSW }
};

// DE (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPointDE[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_FIADD_WORD_INTEGER },
  /* 1 */ { 0, BX_IA_FIMUL_WORD_INTEGER },
  /* 2 */ { 0, BX_IA_FICOM_WORD_INTEGER },
  /* 3 */ { 0, BX_IA_FICOMP_WORD_INTEGER },
  /* 4 */ { 0, BX_IA_FISUB_WORD_INTEGER },
  /* 5 */ { 0, BX_IA_FISUBR_WORD_INTEGER },
  /* 6 */ { 0, BX_IA_FIDIV_WORD_INTEGER },
  /* 7 */ { 0, BX_IA_FIDIVR_WORD_INTEGER },

  /* /r form */ // all instructions pop FPU stack
  /* DE C0 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C1 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C2 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C3 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C4 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C5 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C6 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C7 */ { 0, BX_IA_FADDP_STi_ST0 },
  /* DE C8 */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE C9 */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CA */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CB */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CC */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CD */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CE */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE CF */ { 0, BX_IA_FMULP_STi_ST0 },
  /* DE D0 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D1 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D2 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D3 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D4 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D5 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D6 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D7 */ { 0, BX_IA_FCOMP_STi },           // undocumented, special FPSTACK pop case
  /* DE D8 */ { 0, BX_IA_ERROR  },
  /* DE D9 */ { 0, BX_IA_FCOMPP },
  /* DE DA */ { 0, BX_IA_ERROR  },
  /* DE DB */ { 0, BX_IA_ERROR  },
  /* DE DC */ { 0, BX_IA_ERROR  },
  /* DE DD */ { 0, BX_IA_ERROR  },
  /* DE DE */ { 0, BX_IA_ERROR  },
  /* DE DF */ { 0, BX_IA_ERROR  },
  /* DE E0 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E1 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E2 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E3 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E4 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E5 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E6 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E7 */ { 0, BX_IA_FSUBRP_STi_ST0 },
  /* DE E8 */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE E9 */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE EA */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE EB */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE EC */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE ED */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE EE */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE EF */ { 0, BX_IA_FSUBP_STi_ST0  },
  /* DE F0 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F1 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F2 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F3 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F4 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F5 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F6 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F7 */ { 0, BX_IA_FDIVRP_STi_ST0 },
  /* DE F8 */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE F9 */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FA */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FB */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FC */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FD */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FE */ { 0, BX_IA_FDIVP_STi_ST0  },
  /* DE FF */ { 0, BX_IA_FDIVP_STi_ST0  }
};

// DF (modrm is outside 00h - BFh)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPointDF[64+8] = {
  /* /m form */
  /* 0 */ { 0, BX_IA_FILD_WORD_INTEGER },
  /* 1 */ { 0, BX_IA_FISTTP16 },
  /* 2 */ { 0, BX_IA_FIST_WORD_INTEGER },
  /* 3 */ { 0, BX_IA_FISTP_WORD_INTEGER },
  /* 4 */ { 0, BX_IA_FBLD_PACKED_BCD },
  /* 5 */ { 0, BX_IA_FILD_QWORD_INTEGER },
  /* 6 */ { 0, BX_IA_FBSTP_PACKED_BCD },
  /* 7 */ { 0, BX_IA_FISTP_QWORD_INTEGER },

  /* /r form */
  /* DF C0 */ { 0, BX_IA_FFREEP_STi },   // 287+ compatibility opcode
  /* DF C1 */ { 0, BX_IA_FFREEP_STi },
  /* DF C2 */ { 0, BX_IA_FFREEP_STi },
  /* DF C3 */ { 0, BX_IA_FFREEP_STi },
  /* DF C4 */ { 0, BX_IA_FFREEP_STi },
  /* DF C5 */ { 0, BX_IA_FFREEP_STi },
  /* DF C6 */ { 0, BX_IA_FFREEP_STi },
  /* DF C7 */ { 0, BX_IA_FFREEP_STi },
  /* DF C8 */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF C9 */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CA */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CB */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CC */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CD */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CE */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF CF */ { 0, BX_IA_FXCH_STi },    // undocumented
  /* DF D0 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D1 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D2 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D3 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D4 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D5 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D6 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D7 */ { 0, BX_IA_FSTP_STi },	// undocumented, special FPSTACK pop case
  /* DF D8 */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF D9 */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DA */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DB */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DC */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DD */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DE */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF DF */ { 0, BX_IA_FSTP_STi },	// undocumented
  /* DF E0 */ { 0, BX_IA_FNSTSW_AX },
  /* DF E1 */ { 0, BX_IA_ERROR },
  /* DF E2 */ { 0, BX_IA_ERROR },
  /* DF E3 */ { 0, BX_IA_ERROR },
  /* DF E4 */ { 0, BX_IA_ERROR },
  /* DF E5 */ { 0, BX_IA_ERROR },
  /* DF E6 */ { 0, BX_IA_ERROR },
  /* DF E7 */ { 0, BX_IA_ERROR },
  /* DF E8 */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF E9 */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF EA */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF EB */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF EC */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF ED */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF EE */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF EF */ { 0, BX_IA_FUCOMIP_ST0_STj },
  /* DF F0 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F1 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F2 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F3 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F4 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F5 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F6 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F7 */ { 0, BX_IA_FCOMIP_ST0_STj },
  /* DF F8 */ { 0, BX_IA_ERROR },
  /* DF F9 */ { 0, BX_IA_ERROR },
  /* DF FA */ { 0, BX_IA_ERROR },
  /* DF FB */ { 0, BX_IA_ERROR },
  /* DF FC */ { 0, BX_IA_ERROR },
  /* DF FD */ { 0, BX_IA_ERROR },
  /* DF FE */ { 0, BX_IA_ERROR },
  /* DF FF */ { 0, BX_IA_ERROR },
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
