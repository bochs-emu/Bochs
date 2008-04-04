/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode.h,v 1.58 2008-04-04 22:39:45 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_COMMON_FETCHDECODE_TABLES_H
#define BX_COMMON_FETCHDECODE_TABLES_H

typedef struct BxOpcodeInfo_t {
  Bit16u Attr;
  Bit16u IA;
  const BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;

//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
//

BX_CPP_INLINE Bit16u FetchWORD(const Bit8u *iptr)
{
   Bit16u data;
   ReadHostWordFromLittleEndian(iptr, data);
   return data;
}

BX_CPP_INLINE Bit32u FetchDWORD(const Bit8u *iptr)
{
   Bit32u data;
   ReadHostDWordFromLittleEndian(iptr, data);
   return data;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u FetchQWORD(const Bit8u *iptr)
{
   Bit64u data;
   ReadHostQWordFromLittleEndian(iptr, data);
   return data;
}
#endif

enum {
#define bx_define_opcode(a, b) a,
#include "ia_opcodes.h"
   BX_IA_LAST
};
#undef  bx_define_opcode

//
// Common FetchDecode Opcode Tables
//

#if BX_SUPPORT_FPU

/* ************************************************************************ */
/* FPU Opcodes */

// floating point instructions when mod!=11b.
// the following tables will be accessed like groups using the nnn (reg) field of
// the modrm byte. (the first byte is D8-DF)

  // D8 (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupD8[8] = {
  /* 0 */ { 0, BX_IA_FADD_SINGLE_REAL  },
  /* 1 */ { 0, BX_IA_FMUL_SINGLE_REAL  },
  /* 2 */ { 0, BX_IA_FCOM_SINGLE_REAL  },
  /* 3 */ { 0, BX_IA_FCOM_SINGLE_REAL  },	// FCOMP_SINGLE_REAL
  /* 4 */ { 0, BX_IA_FSUB_SINGLE_REAL  },
  /* 5 */ { 0, BX_IA_FSUBR_SINGLE_REAL },
  /* 6 */ { 0, BX_IA_FDIV_SINGLE_REAL  },
  /* 7 */ { 0, BX_IA_FDIVR_SINGLE_REAL }
};

  // D9 (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupD9[8] = {
  /* 0 */ { 0, BX_IA_FLD_SINGLE_REAL },
  /* 1 */ { 0, BX_IA_ERROR           },
  /* 2 */ { 0, BX_IA_FST_SINGLE_REAL },
  /* 3 */ { 0, BX_IA_FST_SINGLE_REAL },	 	// FSTP_SINGLE_REAL
  /* 4 */ { 0, BX_IA_FLDENV          },
  /* 5 */ { 0, BX_IA_FLDCW           },
  /* 6 */ { 0, BX_IA_FNSTENV         },
  /* 7 */ { 0, BX_IA_FNSTCW          }
};

  // DA (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDA[8] = {
  /* 0 */ { 0, BX_IA_FIADD_DWORD_INTEGER  },
  /* 1 */ { 0, BX_IA_FIMUL_DWORD_INTEGER  },
  /* 2 */ { 0, BX_IA_FICOM_DWORD_INTEGER  },
  /* 3 */ { 0, BX_IA_FICOM_DWORD_INTEGER  },	// FICOMP_DWORD_INTEGER
  /* 4 */ { 0, BX_IA_FISUB_DWORD_INTEGER  },
  /* 5 */ { 0, BX_IA_FISUBR_DWORD_INTEGER },
  /* 6 */ { 0, BX_IA_FIDIV_DWORD_INTEGER  },
  /* 7 */ { 0, BX_IA_FIDIVR_DWORD_INTEGER }
};

  // DB (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDB[8] = {
  /* 0 */ { 0, BX_IA_FILD_DWORD_INTEGER  },
  /* 1 */ { 0, BX_IA_FISTTP32            },
  /* 2 */ { 0, BX_IA_FIST_DWORD_INTEGER  },
  /* 3 */ { 0, BX_IA_FIST_DWORD_INTEGER  },	// FISTP_DWORD_INTEGER
  /* 4 */ { 0, BX_IA_ERROR               },
  /* 5 */ { 0, BX_IA_FLD_EXTENDED_REAL   },
  /* 6 */ { 0, BX_IA_ERROR               },
  /* 7 */ { 0, BX_IA_FSTP_EXTENDED_REAL  }
};

  // DC (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDC[8] = {
  /* 0 */ { 0, BX_IA_FADD_DOUBLE_REAL  },
  /* 1 */ { 0, BX_IA_FMUL_DOUBLE_REAL  },
  /* 2 */ { 0, BX_IA_FCOM_DOUBLE_REAL  },
  /* 3 */ { 0, BX_IA_FCOM_DOUBLE_REAL  },	// FCOMP_DOUBLE_REAL
  /* 4 */ { 0, BX_IA_FSUB_DOUBLE_REAL  },
  /* 5 */ { 0, BX_IA_FSUBR_DOUBLE_REAL },
  /* 6 */ { 0, BX_IA_FDIV_DOUBLE_REAL  },
  /* 7 */ { 0, BX_IA_FDIVR_DOUBLE_REAL }
};

  // DD (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDD[8] = {
  /* 0 */ { 0, BX_IA_FLD_DOUBLE_REAL },
  /* 1 */ { 0, BX_IA_FISTTP64        },
  /* 2 */ { 0, BX_IA_FST_DOUBLE_REAL },
  /* 3 */ { 0, BX_IA_FST_DOUBLE_REAL },		// FSTP_DOUBLE_REAL
  /* 4 */ { 0, BX_IA_FRSTOR          },
  /* 5 */ { 0, BX_IA_ERROR           },
  /* 6 */ { 0, BX_IA_FNSAVE          },
  /* 7 */ { 0, BX_IA_FNSTSW          }
};

  // DE (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDE[8] = {
  /* 0 */ { 0, BX_IA_FIADD_WORD_INTEGER  },
  /* 1 */ { 0, BX_IA_FIMUL_WORD_INTEGER  },
  /* 2 */ { 0, BX_IA_FICOM_WORD_INTEGER  },
  /* 3 */ { 0, BX_IA_FICOM_WORD_INTEGER  },	// FICOMP_WORD_INTEGER
  /* 4 */ { 0, BX_IA_FISUB_WORD_INTEGER  },
  /* 5 */ { 0, BX_IA_FISUBR_WORD_INTEGER },
  /* 6 */ { 0, BX_IA_FIDIV_WORD_INTEGER  },
  /* 7 */ { 0, BX_IA_FIDIVR_WORD_INTEGER }
};

  // DF (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDF[8] = {
  /* 0 */ { 0, BX_IA_FILD_WORD_INTEGER   },
  /* 1 */ { 0, BX_IA_FISTTP16            },
  /* 2 */ { 0, BX_IA_FIST_WORD_INTEGER   },
  /* 3 */ { 0, BX_IA_FIST_WORD_INTEGER   },	// FISTP_WORD_INTEGER
  /* 4 */ { 0, BX_IA_FBLD_PACKED_BCD     },
  /* 5 */ { 0, BX_IA_FILD_QWORD_INTEGER  },
  /* 6 */ { 0, BX_IA_FBSTP_PACKED_BCD    },
  /* 7 */ { 0, BX_IA_FISTP_QWORD_INTEGER }
};

// 512 entries for second byte of floating point instructions. (when mod==11b)
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPoint[512] = {
  // D8 (modrm is outside 00h - BFh) (mod == 11)
  /* D8 C0 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C1 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C2 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C3 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C4 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C5 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C6 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C7 */ { 0, BX_IA_FADD_ST0_STj  },
  /* D8 C8 */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 C9 */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CA */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CB */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CC */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CD */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CE */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 CF */ { 0, BX_IA_FMUL_ST0_STj  },
  /* D8 D0 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D1 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D2 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D3 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D4 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D5 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D6 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D7 */ { 0, BX_IA_FCOM_STi      },
  /* D8 D8 */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 D9 */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DA */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DB */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DC */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DD */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DE */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 DF */ { 0, BX_IA_FCOM_STi      },	// FCOMP
  /* D8 E0 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E1 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E2 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E3 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E4 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E5 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E6 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E7 */ { 0, BX_IA_FSUB_ST0_STj  },
  /* D8 E8 */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 E9 */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 EA */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 EB */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 EC */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 ED */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 EE */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 EF */ { 0, BX_IA_FSUBR_ST0_STj },
  /* D8 F0 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F1 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F2 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F3 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F4 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F5 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F6 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F7 */ { 0, BX_IA_FDIV_ST0_STj  },
  /* D8 F8 */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 F9 */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FA */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FB */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FC */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FD */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FE */ { 0, BX_IA_FDIVR_ST0_STj },
  /* D8 FF */ { 0, BX_IA_FDIVR_ST0_STj },

  // D9 (modrm is outside 00h - BFh) (mod == 11)
  /* D9 C0 */ { 0, BX_IA_FLD_STi  },
  /* D9 C1 */ { 0, BX_IA_FLD_STi  },
  /* D9 C2 */ { 0, BX_IA_FLD_STi  },
  /* D9 C3 */ { 0, BX_IA_FLD_STi  },
  /* D9 C4 */ { 0, BX_IA_FLD_STi  },
  /* D9 C5 */ { 0, BX_IA_FLD_STi  },
  /* D9 C6 */ { 0, BX_IA_FLD_STi  },
  /* D9 C7 */ { 0, BX_IA_FLD_STi  },
  /* D9 C8 */ { 0, BX_IA_FXCH_STi },
  /* D9 C9 */ { 0, BX_IA_FXCH_STi },
  /* D9 CA */ { 0, BX_IA_FXCH_STi },
  /* D9 CB */ { 0, BX_IA_FXCH_STi },
  /* D9 CC */ { 0, BX_IA_FXCH_STi },
  /* D9 CD */ { 0, BX_IA_FXCH_STi },
  /* D9 CE */ { 0, BX_IA_FXCH_STi },
  /* D9 CF */ { 0, BX_IA_FXCH_STi },
  /* D9 D0 */ { 0, BX_IA_FNOP     },
  /* D9 D1 */ { 0, BX_IA_ERROR  },
  /* D9 D2 */ { 0, BX_IA_ERROR  },
  /* D9 D3 */ { 0, BX_IA_ERROR  },
  /* D9 D4 */ { 0, BX_IA_ERROR  },
  /* D9 D5 */ { 0, BX_IA_ERROR  },
  /* D9 D6 */ { 0, BX_IA_ERROR  },
  /* D9 D7 */ { 0, BX_IA_ERROR  },
  /* D9 D8 */ { 0, BX_IA_ERROR  },
  /* D9 D9 */ { 0, BX_IA_ERROR  },
  /* D9 DA */ { 0, BX_IA_ERROR  },
  /* D9 DB */ { 0, BX_IA_ERROR  },
  /* D9 DC */ { 0, BX_IA_ERROR  },
  /* D9 DD */ { 0, BX_IA_ERROR  },
  /* D9 DE */ { 0, BX_IA_ERROR  },
  /* D9 DF */ { 0, BX_IA_ERROR  },
  /* D9 E0 */ { 0, BX_IA_FCHS     },
  /* D9 E1 */ { 0, BX_IA_FABS     },
  /* D9 E2 */ { 0, BX_IA_ERROR  },
  /* D9 E3 */ { 0, BX_IA_ERROR  },
  /* D9 E4 */ { 0, BX_IA_FTST     },
  /* D9 E5 */ { 0, BX_IA_FXAM     },
  /* D9 E6 */ { 0, BX_IA_ERROR    },
  /* D9 E7 */ { 0, BX_IA_ERROR    },
  /* D9 E8 */ { 0, BX_IA_FLD1     },
  /* D9 E9 */ { 0, BX_IA_FLDL2T   },
  /* D9 EA */ { 0, BX_IA_FLDL2E   },
  /* D9 EB */ { 0, BX_IA_FLDPI    },
  /* D9 EC */ { 0, BX_IA_FLDLG2   },
  /* D9 ED */ { 0, BX_IA_FLDLN2   },
  /* D9 EE */ { 0, BX_IA_FLDZ     },
  /* D9 EF */ { 0, BX_IA_ERROR    },
  /* D9 F0 */ { 0, BX_IA_F2XM1    },
  /* D9 F1 */ { 0, BX_IA_FYL2X    },
  /* D9 F2 */ { 0, BX_IA_FPTAN    },
  /* D9 F3 */ { 0, BX_IA_FPATAN   },
  /* D9 F4 */ { 0, BX_IA_FXTRACT  },
  /* D9 F5 */ { 0, BX_IA_FPREM1   },
  /* D9 F6 */ { 0, BX_IA_FDECSTP  },
  /* D9 F7 */ { 0, BX_IA_FINCSTP  },
  /* D9 F8 */ { 0, BX_IA_FPREM    },
  /* D9 F9 */ { 0, BX_IA_FYL2XP1  },
  /* D9 FA */ { 0, BX_IA_FSQRT    },
  /* D9 FB */ { 0, BX_IA_FSINCOS  },
  /* D9 FC */ { 0, BX_IA_FRNDINT  },
  /* D9 FD */ { 0, BX_IA_FSCALE   },
  /* D9 FE */ { 0, BX_IA_FSIN     },
  /* D9 FF */ { 0, BX_IA_FCOS     },

  // DA (modrm is outside 00h - BFh) (mod == 11)
  /* DA C0 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C1 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C2 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C3 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C4 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C5 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C6 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C7 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C8 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA C9 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CA */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CB */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CC */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CD */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CE */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA CF */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D0 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D1 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D2 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D3 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D4 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D5 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D6 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D7 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D8 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA D9 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DA */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DB */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DC */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DD */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DE */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DA DF */ { 0, BX_IA_FCMOV_ST0_STj },
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
  /* DA FF */ { 0, BX_IA_ERROR },

  // DB (modrm is outside 00h - BFh) (mod == 11)
  /* DB C0 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C1 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C2 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C3 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C4 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C5 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C6 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C7 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C8 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB C9 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CA */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CB */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CC */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CD */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CE */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB CF */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D0 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D1 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D2 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D3 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D4 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D5 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D6 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D7 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D8 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB D9 */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DA */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DB */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DC */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DD */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DE */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB DF */ { 0, BX_IA_FCMOV_ST0_STj },
  /* DB E0 */ { 0, BX_IA_FPLEGACY      },     // feni  (287 only)
  /* DB E1 */ { 0, BX_IA_FPLEGACY      },     // fdisi (287 only)
  /* DB E2 */ { 0, BX_IA_FNCLEX        },
  /* DB E3 */ { 0, BX_IA_FNINIT        },
  /* DB E4 */ { 0, BX_IA_FPLEGACY      },     // fsetpm (287 only)
  /* DB E5 */ { 0, BX_IA_ERROR        },
  /* DB E6 */ { 0, BX_IA_ERROR        },
  /* DB E7 */ { 0, BX_IA_ERROR        },
  /* DB E8 */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB E9 */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EA */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EB */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EC */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB ED */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EE */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB EF */ { 0, BX_IA_FUCOMI_ST0_STj },
  /* DB F0 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F1 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F2 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F3 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F4 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F5 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F6 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F7 */ { 0, BX_IA_FCOMI_ST0_STj  },
  /* DB F8 */ { 0, BX_IA_ERROR },
  /* DB F9 */ { 0, BX_IA_ERROR },
  /* DB FA */ { 0, BX_IA_ERROR },
  /* DB FB */ { 0, BX_IA_ERROR },
  /* DB FC */ { 0, BX_IA_ERROR },
  /* DB FD */ { 0, BX_IA_ERROR },
  /* DB FE */ { 0, BX_IA_ERROR },
  /* DB FF */ { 0, BX_IA_ERROR },

  // DC (modrm is outside 00h - BFh) (mod == 11)
  /* DC C0 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C1 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C2 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C3 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C4 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C5 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C6 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C7 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DC C8 */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC C9 */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CA */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CB */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CC */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CD */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CE */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC CF */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DC D0 */ { 0, BX_IA_ERROR },
  /* DC D1 */ { 0, BX_IA_ERROR },
  /* DC D2 */ { 0, BX_IA_ERROR },
  /* DC D3 */ { 0, BX_IA_ERROR },
  /* DC D4 */ { 0, BX_IA_ERROR },
  /* DC D5 */ { 0, BX_IA_ERROR },
  /* DC D6 */ { 0, BX_IA_ERROR },
  /* DC D7 */ { 0, BX_IA_ERROR },
  /* DC D8 */ { 0, BX_IA_ERROR },
  /* DC D9 */ { 0, BX_IA_ERROR },
  /* DC DA */ { 0, BX_IA_ERROR },
  /* DC DB */ { 0, BX_IA_ERROR },
  /* DC DC */ { 0, BX_IA_ERROR },
  /* DC DD */ { 0, BX_IA_ERROR },
  /* DC DE */ { 0, BX_IA_ERROR },
  /* DC DF */ { 0, BX_IA_ERROR },
  /* DC E0 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E1 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E2 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E3 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E4 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E5 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E6 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E7 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DC E8 */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC E9 */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC EA */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC EB */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC EC */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC ED */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC EE */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC EF */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DC F0 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F1 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F2 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F3 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F4 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F5 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F6 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F7 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DC F8 */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC F9 */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FA */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FB */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FC */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FD */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FE */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DC FF */ { 0, BX_IA_FDIV_STi_ST0  },

  // DD (modrm is outside 00h - BFh) (mod == 11)
  /* DD C0 */ { 0, BX_IA_FFREE_STi },
  /* DD C1 */ { 0, BX_IA_FFREE_STi },
  /* DD C2 */ { 0, BX_IA_FFREE_STi },
  /* DD C3 */ { 0, BX_IA_FFREE_STi },
  /* DD C4 */ { 0, BX_IA_FFREE_STi },
  /* DD C5 */ { 0, BX_IA_FFREE_STi },
  /* DD C6 */ { 0, BX_IA_FFREE_STi },
  /* DD C7 */ { 0, BX_IA_FFREE_STi },
  /* DD C8 */ { 0, BX_IA_ERROR   },
  /* DD C9 */ { 0, BX_IA_ERROR   },
  /* DD CA */ { 0, BX_IA_ERROR   },
  /* DD CB */ { 0, BX_IA_ERROR   },
  /* DD CC */ { 0, BX_IA_ERROR   },
  /* DD CD */ { 0, BX_IA_ERROR   },
  /* DD CE */ { 0, BX_IA_ERROR   },
  /* DD CF */ { 0, BX_IA_ERROR   },
  /* DD D0 */ { 0, BX_IA_FST_STi   },
  /* DD D1 */ { 0, BX_IA_FST_STi   },
  /* DD D2 */ { 0, BX_IA_FST_STi   },
  /* DD D3 */ { 0, BX_IA_FST_STi   },
  /* DD D4 */ { 0, BX_IA_FST_STi   },
  /* DD D5 */ { 0, BX_IA_FST_STi   },
  /* DD D6 */ { 0, BX_IA_FST_STi   },
  /* DD D7 */ { 0, BX_IA_FST_STi   },
  /* DD D8 */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD D9 */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DA */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DB */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DC */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DD */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DE */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD DF */ { 0, BX_IA_FST_STi   },	// FSTP_STi
  /* DD E0 */ { 0, BX_IA_FUCOM_STi },
  /* DD E1 */ { 0, BX_IA_FUCOM_STi },
  /* DD E2 */ { 0, BX_IA_FUCOM_STi },
  /* DD E3 */ { 0, BX_IA_FUCOM_STi },
  /* DD E4 */ { 0, BX_IA_FUCOM_STi },
  /* DD E5 */ { 0, BX_IA_FUCOM_STi },
  /* DD E6 */ { 0, BX_IA_FUCOM_STi },
  /* DD E7 */ { 0, BX_IA_FUCOM_STi },
  /* DD E8 */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD E9 */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD EA */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD EB */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD EC */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD ED */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD EE */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD EF */ { 0, BX_IA_FUCOM_STi },	// FUCOMP
  /* DD F0 */ { 0, BX_IA_ERROR },
  /* DD F1 */ { 0, BX_IA_ERROR },
  /* DD F2 */ { 0, BX_IA_ERROR },
  /* DD F3 */ { 0, BX_IA_ERROR },
  /* DD F4 */ { 0, BX_IA_ERROR },
  /* DD F5 */ { 0, BX_IA_ERROR },
  /* DD F6 */ { 0, BX_IA_ERROR },
  /* DD F7 */ { 0, BX_IA_ERROR },
  /* DD F8 */ { 0, BX_IA_ERROR },
  /* DD F9 */ { 0, BX_IA_ERROR },
  /* DD FA */ { 0, BX_IA_ERROR },
  /* DD FB */ { 0, BX_IA_ERROR },
  /* DD FC */ { 0, BX_IA_ERROR },
  /* DD FD */ { 0, BX_IA_ERROR },
  /* DD FE */ { 0, BX_IA_ERROR },
  /* DD FF */ { 0, BX_IA_ERROR },

  // DE (modrm is outside 00h - BFh) (mod == 11)
  // all instructions pop FPU stack
  /* DE C0 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C1 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C2 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C3 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C4 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C5 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C6 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C7 */ { 0, BX_IA_FADD_STi_ST0 },
  /* DE C8 */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE C9 */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CA */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CB */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CC */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CD */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CE */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE CF */ { 0, BX_IA_FMUL_STi_ST0 },
  /* DE D0 */ { 0, BX_IA_ERROR },
  /* DE D1 */ { 0, BX_IA_ERROR },
  /* DE D2 */ { 0, BX_IA_ERROR },
  /* DE D3 */ { 0, BX_IA_ERROR },
  /* DE D4 */ { 0, BX_IA_ERROR },
  /* DE D5 */ { 0, BX_IA_ERROR },
  /* DE D6 */ { 0, BX_IA_ERROR },
  /* DE D7 */ { 0, BX_IA_ERROR },
  /* DE D8 */ { 0, BX_IA_ERROR },
  /* DE D9 */ { 0, BX_IA_FCOMPP  },
  /* DE DA */ { 0, BX_IA_ERROR },
  /* DE DB */ { 0, BX_IA_ERROR },
  /* DE DC */ { 0, BX_IA_ERROR },
  /* DE DD */ { 0, BX_IA_ERROR },
  /* DE DE */ { 0, BX_IA_ERROR },
  /* DE DF */ { 0, BX_IA_ERROR },
  /* DE E0 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E1 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E2 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E3 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E4 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E5 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E6 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E7 */ { 0, BX_IA_FSUBR_STi_ST0 },
  /* DE E8 */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE E9 */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE EA */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE EB */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE EC */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE ED */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE EE */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE EF */ { 0, BX_IA_FSUB_STi_ST0  },
  /* DE F0 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F1 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F2 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F3 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F4 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F5 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F6 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F7 */ { 0, BX_IA_FDIVR_STi_ST0 },
  /* DE F8 */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE F9 */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FA */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FB */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FC */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FD */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FE */ { 0, BX_IA_FDIV_STi_ST0  },
  /* DE FF */ { 0, BX_IA_FDIV_STi_ST0  },

  // DF (modrm is outside 00h - BFh) (mod == 11)
  /* DF C0 */ { 0, BX_IA_FFREEP_STi },   // 287+ compatibility opcode
  /* DF C1 */ { 0, BX_IA_FFREEP_STi },
  /* DF C2 */ { 0, BX_IA_FFREEP_STi },
  /* DF C3 */ { 0, BX_IA_FFREEP_STi },
  /* DF C4 */ { 0, BX_IA_FFREEP_STi },
  /* DF C5 */ { 0, BX_IA_FFREEP_STi },
  /* DF C6 */ { 0, BX_IA_FFREEP_STi },
  /* DF C7 */ { 0, BX_IA_FFREEP_STi },
  /* DF C8 */ { 0, BX_IA_ERROR },
  /* DF C9 */ { 0, BX_IA_ERROR },
  /* DF CA */ { 0, BX_IA_ERROR },
  /* DF CB */ { 0, BX_IA_ERROR },
  /* DF CC */ { 0, BX_IA_ERROR },
  /* DF CD */ { 0, BX_IA_ERROR },
  /* DF CE */ { 0, BX_IA_ERROR },
  /* DF CF */ { 0, BX_IA_ERROR },
  /* DF D0 */ { 0, BX_IA_ERROR },
  /* DF D1 */ { 0, BX_IA_ERROR },
  /* DF D2 */ { 0, BX_IA_ERROR },
  /* DF D3 */ { 0, BX_IA_ERROR },
  /* DF D4 */ { 0, BX_IA_ERROR },
  /* DF D5 */ { 0, BX_IA_ERROR },
  /* DF D6 */ { 0, BX_IA_ERROR },
  /* DF D7 */ { 0, BX_IA_ERROR },
  /* DF D8 */ { 0, BX_IA_ERROR },
  /* DF D9 */ { 0, BX_IA_ERROR },
  /* DF DA */ { 0, BX_IA_ERROR },
  /* DF DB */ { 0, BX_IA_ERROR },
  /* DF DC */ { 0, BX_IA_ERROR },
  /* DF DD */ { 0, BX_IA_ERROR },
  /* DF DE */ { 0, BX_IA_ERROR },
  /* DF DF */ { 0, BX_IA_ERROR },
  /* DF E0 */ { 0, BX_IA_FNSTSW_AX },
  /* DF E1 */ { 0, BX_IA_ERROR },
  /* DF E2 */ { 0, BX_IA_ERROR },
  /* DF E3 */ { 0, BX_IA_ERROR },
  /* DF E4 */ { 0, BX_IA_ERROR },
  /* DF E5 */ { 0, BX_IA_ERROR },
  /* DF E6 */ { 0, BX_IA_ERROR },
  /* DF E7 */ { 0, BX_IA_ERROR },
  /* DF E8 */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF E9 */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EA */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EB */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EC */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF ED */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EE */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EF */ { 0, BX_IA_FUCOMI_ST0_STj },	// FUCOMIP
  /* DF F0 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F1 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F2 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F3 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F4 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F5 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F6 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
  /* DF F7 */ { 0, BX_IA_FCOMI_ST0_STj  },	// FCOMIP
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

static const BxOpcodeInfo_t Bx3DNowOpcodeInfo[256] = {
  // 256 entries for 3DNow opcodes, by suffix
  /* 00 */ { 0, BX_IA_ERROR },
  /* 01 */ { 0, BX_IA_ERROR },
  /* 02 */ { 0, BX_IA_ERROR },
  /* 03 */ { 0, BX_IA_ERROR },
  /* 04 */ { 0, BX_IA_ERROR },
  /* 05 */ { 0, BX_IA_ERROR },
  /* 06 */ { 0, BX_IA_ERROR },
  /* 07 */ { 0, BX_IA_ERROR },
  /* 08 */ { 0, BX_IA_ERROR },
  /* 09 */ { 0, BX_IA_ERROR },
  /* 0A */ { 0, BX_IA_ERROR },
  /* 0B */ { 0, BX_IA_ERROR },
  /* 0C */ { 0, BX_IA_PI2FW_PqQq },
  /* 0D */ { 0, BX_IA_PI2FD_PqQq },
  /* 0E */ { 0, BX_IA_ERROR },
  /* 0F */ { 0, BX_IA_ERROR },
  /* 10 */ { 0, BX_IA_ERROR },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { 0, BX_IA_ERROR },
  /* 15 */ { 0, BX_IA_ERROR },
  /* 16 */ { 0, BX_IA_ERROR },
  /* 17 */ { 0, BX_IA_ERROR },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { 0, BX_IA_PF2IW_PqQq },
  /* 1D */ { 0, BX_IA_PF2ID_PqQq },
  /* 1E */ { 0, BX_IA_ERROR },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { 0, BX_IA_ERROR },
  /* 21 */ { 0, BX_IA_ERROR },
  /* 22 */ { 0, BX_IA_ERROR },
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
  /* 40 */ { 0, BX_IA_ERROR },
  /* 41 */ { 0, BX_IA_ERROR },
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
  /* 8A */ { 0, BX_IA_PFNACC_PqQq },
  /* 8B */ { 0, BX_IA_ERROR },
  /* 8C */ { 0, BX_IA_ERROR },
  /* 8D */ { 0, BX_IA_ERROR },
  /* 8E */ { 0, BX_IA_PFPNACC_PqQq },
  /* 8F */ { 0, BX_IA_ERROR },
  /* 90 */ { 0, BX_IA_PFCMPGE_PqQq },
  /* 91 */ { 0, BX_IA_ERROR },
  /* 92 */ { 0, BX_IA_ERROR },
  /* 93 */ { 0, BX_IA_ERROR },
  /* 94 */ { 0, BX_IA_PFMIN_PqQq },
  /* 95 */ { 0, BX_IA_ERROR },
  /* 96 */ { 0, BX_IA_PFRCP_PqQq },
  /* 97 */ { 0, BX_IA_PFRSQRT_PqQq },
  /* 98 */ { 0, BX_IA_ERROR },
  /* 99 */ { 0, BX_IA_ERROR },
  /* 9A */ { 0, BX_IA_PFSUB_PqQq },
  /* 9B */ { 0, BX_IA_ERROR },
  /* 9C */ { 0, BX_IA_ERROR },
  /* 9D */ { 0, BX_IA_ERROR },
  /* 9E */ { 0, BX_IA_PFADD_PqQq },
  /* 9F */ { 0, BX_IA_ERROR },
  /* A0 */ { 0, BX_IA_PFCMPGT_PqQq },
  /* A1 */ { 0, BX_IA_ERROR },
  /* A2 */ { 0, BX_IA_ERROR },
  /* A3 */ { 0, BX_IA_ERROR },
  /* A4 */ { 0, BX_IA_PFMAX_PqQq },
  /* A5 */ { 0, BX_IA_ERROR },
  /* A6 */ { 0, BX_IA_PFRCPIT1_PqQq },
  /* A7 */ { 0, BX_IA_PFRSQIT1_PqQq },
  /* A8 */ { 0, BX_IA_ERROR },
  /* A9 */ { 0, BX_IA_ERROR },
  /* AA */ { 0, BX_IA_PFSUBR_PqQq },
  /* AB */ { 0, BX_IA_ERROR },
  /* AC */ { 0, BX_IA_ERROR },
  /* AD */ { 0, BX_IA_ERROR },
  /* AE */ { 0, BX_IA_PFACC_PqQq },
  /* AF */ { 0, BX_IA_ERROR },
  /* B0 */ { 0, BX_IA_PFCMPEQ_PqQq },
  /* B1 */ { 0, BX_IA_ERROR },
  /* B2 */ { 0, BX_IA_ERROR },
  /* B3 */ { 0, BX_IA_ERROR },
  /* B4 */ { 0, BX_IA_PFMUL_PqQq },
  /* B5 */ { 0, BX_IA_ERROR },
  /* B6 */ { 0, BX_IA_PFRCPIT2_PqQq },
  /* B7 */ { 0, BX_IA_PMULHRW_PqQq },
  /* B8 */ { 0, BX_IA_ERROR },  	
  /* B9 */ { 0, BX_IA_ERROR },
  /* BA */ { 0, BX_IA_ERROR },
  /* BB */ { 0, BX_IA_PSWAPD_PqQq },
  /* BC */ { 0, BX_IA_ERROR },
  /* BD */ { 0, BX_IA_ERROR },
  /* BE */ { 0, BX_IA_ERROR },
  /* BF */ { 0, BX_IA_PAVGB_PqQq },
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
  /* DF */ { 0, BX_IA_ERROR },
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

#endif


/* ********** */
/* SSE Groups */
/* ********** */

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[4] = {
  /* -- */ { 0, BX_IA_MOVUPS_VpsWps },
  /* 66 */ { 0, BX_IA_MOVUPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MOVSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MOVSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[4] = {
  /* -- */ { 0, BX_IA_MOVUPS_WpsVps },
  /* 66 */ { 0, BX_IA_MOVUPD_WpdVpd },
  /* F2 */ { 0, BX_IA_MOVSD_WsdVsd },
  /* F3 */ { 0, BX_IA_MOVSS_WssVss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[4] = {
  /* -- */ { 0, BX_IA_MOVLPS_VpsMq },
  /* 66 */ { 0, BX_IA_MOVLPD_VsdMq },
  /* F2 */ { 0, BX_IA_MOVDDUP_VpdWq },	// SSE3
  /* F3 */ { 0, BX_IA_MOVSLDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[4] = {
  /* -- */ { 0, BX_IA_MOVLPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVLPD_MqVsd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[4] = {
  /* -- */ { 0, BX_IA_UNPCKLPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKLPD_VpdWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[4] = {
  /* -- */ { 0, BX_IA_UNPCKHPS_VpsWdq },
  /* 66 */ { 0, BX_IA_UNPCKHPD_VpdWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[4] = {
  /* -- */ { 0, BX_IA_MOVHPS_VpsMq },
  /* 66 */ { 0, BX_IA_MOVHPD_VsdMq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVSHDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[4] = {
  /* -- */ { 0, BX_IA_MOVHPS_MqVps },
  /* 66 */ { 0, BX_IA_MOVHPD_MqVsd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[4] = {
  /* -- */ { 0, BX_IA_MOVAPS_VpsWps },
  /* 66 */ { 0, BX_IA_MOVAPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[4] = {
  /* -- */ { 0, BX_IA_MOVAPS_WpsVps },
  /* 66 */ { 0, BX_IA_MOVAPD_WpdVpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[4] = {
  /* -- */ { 0, BX_IA_CVTPI2PS_VpsQq },
  /* 66 */ { 0, BX_IA_CVTPI2PD_VpdQq },
  /* F2 */ { 0, BX_IA_CVTSI2SD_VsdEd },
  /* F3 */ { 0, BX_IA_CVTSI2SS_VssEd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[4] = {
  /* -- */ { 0, BX_IA_MOVNTPS_MpsVps },
  /* 66 */ { 0, BX_IA_MOVNTPD_MpdVpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[4] = {
  /* -- */ { 0, BX_IA_CVTTPS2PI_PqWps },
  /* 66 */ { 0, BX_IA_CVTTPD2PI_PqWpd },
  /* F2 */ { 0, BX_IA_CVTTSD2SI_GdWsd },
  /* F3 */ { 0, BX_IA_CVTTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[4] = {
  /* -- */ { 0, BX_IA_CVTPS2PI_PqWps },
  /* 66 */ { 0, BX_IA_CVTPD2PI_PqWpd },
  /* F2 */ { 0, BX_IA_CVTSD2SI_GdWsd },
  /* F3 */ { 0, BX_IA_CVTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[4] = {
  /* -- */ { 0, BX_IA_UCOMISS_VssWss },
  /* 66 */ { 0, BX_IA_UCOMISD_VsdWsd },            	
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[4] = {
  /* -- */ { 0, BX_IA_COMISS_VpsWps },
  /* 66 */ { 0, BX_IA_COMISD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50[4] = {
  /* -- */ { 0, BX_IA_MOVMSKPS_GdVRps },
  /* 66 */ { 0, BX_IA_MOVMSKPD_GdVRpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[4] = {
  /* -- */ { 0, BX_IA_SQRTPS_VpsWps },
  /* 66 */ { 0, BX_IA_SQRTPD_VpdWpd },
  /* F2 */ { 0, BX_IA_SQRTSD_VsdWsd },
  /* F3 */ { 0, BX_IA_SQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[4] = {
  /* -- */ { 0, BX_IA_RSQRTPS_VpsWps },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RSQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[4] = {
  /* -- */ { 0, BX_IA_RCPPS_VpsWps },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_RCPSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[4] = {
  /* -- */ { 0, BX_IA_ANDPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[4] = {
  /* -- */ { 0, BX_IA_ANDNPS_VpsWps },
  /* 66 */ { 0, BX_IA_ANDNPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[4] = {
  /* -- */ { 0, BX_IA_ORPS_VpsWps },
  /* 66 */ { 0, BX_IA_ORPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[4] = {
  /* -- */ { 0, BX_IA_XORPS_VpsWps },
  /* 66 */ { 0, BX_IA_XORPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[4] = {
  /* -- */ { 0, BX_IA_ADDPS_VpsWps },
  /* 66 */ { 0, BX_IA_ADDPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ADDSD_VsdWsd },
  /* F3 */ { 0, BX_IA_ADDSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[4] = {
  /* -- */ { 0, BX_IA_MULPS_VpsWps },
  /* 66 */ { 0, BX_IA_MULPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MULSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MULSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[4] = {
  /* -- */ { 0, BX_IA_CVTPS2PD_VpsWps },
  /* 66 */ { 0, BX_IA_CVTPD2PS_VpdWpd },
  /* F2 */ { 0, BX_IA_CVTSD2SS_VsdWsd },
  /* F3 */ { 0, BX_IA_CVTSS2SD_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[4] = {
  /* -- */ { 0, BX_IA_CVTDQ2PS_VpsWdq },
  /* 66 */ { 0, BX_IA_CVTPS2DQ_VdqWps },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_CVTTPS2DQ_VdqWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[4] = {
  /* -- */ { 0, BX_IA_SUBPS_VpsWps },
  /* 66 */ { 0, BX_IA_SUBPD_VpdWpd },
  /* F2 */ { 0, BX_IA_SUBSD_VsdWsd },
  /* F3 */ { 0, BX_IA_SUBSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[4] = {
  /* -- */ { 0, BX_IA_MINPS_VpsWps },
  /* 66 */ { 0, BX_IA_MINPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MINSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MINSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[4] = {
  /* -- */ { 0, BX_IA_DIVPS_VpsWps },
  /* 66 */ { 0, BX_IA_DIVPD_VpdWpd },
  /* F2 */ { 0, BX_IA_DIVSD_VsdWsd },
  /* F3 */ { 0, BX_IA_DIVSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[4] = {
  /* -- */ { 0, BX_IA_MAXPS_VpsWps },
  /* 66 */ { 0, BX_IA_MAXPD_VpdWpd },
  /* F2 */ { 0, BX_IA_MAXSD_VsdWsd },
  /* F3 */ { 0, BX_IA_MAXSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLBW_PqQd  },
  /* 66 */ { 0, BX_IA_PUNPCKLBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLWD_PqQd  },
  /* 66 */ { 0, BX_IA_PUNPCKLWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[4] = {
  /* -- */ { 0, BX_IA_PUNPCKLDQ_PqQd  },
  /* 66 */ { 0, BX_IA_PUNPCKLDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[4] = {
  /* -- */ { 0, BX_IA_PACKSSWB_PqQq  },
  /* 66 */ { 0, BX_IA_PACKSSWB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[4] = {
  /* -- */ { 0, BX_IA_PCMPGTB_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPGTB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[4] = {
  /* -- */ { 0, BX_IA_PCMPGTW_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPGTW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[4] = {
  /* -- */ { 0, BX_IA_PCMPGTD_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPGTD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[4] = {
  /* -- */ { 0, BX_IA_PACKUSWB_PqQq   },
  /* 66 */ { 0, BX_IA_PACKUSWB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHBW_PqQq  },
  /* 66 */ { 0, BX_IA_PUNPCKHBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHWD_PqQq  },
  /* 66 */ { 0, BX_IA_PUNPCKHWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[4] = {
  /* -- */ { 0, BX_IA_PUNPCKHDQ_PqQq  },
  /* 66 */ { 0, BX_IA_PUNPCKHDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[4] = {
  /* -- */ { 0, BX_IA_PACKSSDW_PqQq   },
  /* 66 */ { 0, BX_IA_PACKSSDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6c[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PUNPCKLQDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6d[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PUNPCKHQDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[4] = {
  /* -- */ { 0, BX_IA_MOVD_PqEd  },
  /* 66 */ { 0, BX_IA_MOVD_VdqEd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQ[4] = {
  /* -- */ { 0, BX_IA_MOVQ_PqEq  },
  /* 66 */ { 0, BX_IA_MOVQ_VdqEq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[4] = {
  /* -- */ { 0, BX_IA_MOVQ_PqQq     },
  /* 66 */ { 0, BX_IA_MOVDQA_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR       },
  /* F3 */ { 0, BX_IA_MOVDQU_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[4] = {
  /* -- */ { 0, BX_IA_PSHUFW_PqQqIb    },
  /* 66 */ { 0, BX_IA_PSHUFD_VdqWdqIb  },
  /* F2 */ { 0, BX_IA_PSHUFHW_VdqWdqIb },
  /* F3 */ { 0, BX_IA_PSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[4] = {
  /* -- */ { 0, BX_IA_PCMPEQB_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPEQB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[4] = {
  /* -- */ { 0, BX_IA_PCMPEQW_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPEQW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[4] = {
  /* -- */ { 0, BX_IA_PCMPEQD_PqQq   },
  /* 66 */ { 0, BX_IA_PCMPEQD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_HADDPD_VpdWpd },     // SSE3
  /* F2 */ { 0, BX_IA_HADDPS_VpsWps },     // SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_HSUBPD_VpdWpd },     // SSE3
  /* F2 */ { 0, BX_IA_HSUBPS_VpsWps },     // SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[4] = {
  /* -- */ { 0, BX_IA_MOVD_EdPd },
  /* 66 */ { 0, BX_IA_MOVD_EdVd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQ[4] = {
  /* -- */ { 0, BX_IA_MOVQ_EqPq },
  /* 66 */ { 0, BX_IA_MOVQ_EqVq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_MOVQ_VqWq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[4] = {
  /* -- */ { 0, BX_IA_MOVQ_QqPq     },
  /* 66 */ { 0, BX_IA_MOVDQA_WdqVdq },
  /* F2 */ { 0, BX_IA_ERROR       },
  /* F3 */ { 0, BX_IA_MOVDQU_WdqVdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8w[4] = {
  /* -- */ { 0, BX_IA_ERROR     },
  /* 66 */ { 0, BX_IA_ERROR     },
  /* F2 */ { 0, BX_IA_ERROR     },
  /* F3 */ { 0, BX_IA_POPCNT_GwEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8d[4] = {
  /* -- */ { 0, BX_IA_ERROR     },
  /* 66 */ { 0, BX_IA_ERROR     },
  /* F2 */ { 0, BX_IA_ERROR     },
  /* F3 */ { 0, BX_IA_POPCNT_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8q[4] = {
  /* -- */ { 0, BX_IA_ERROR     },
  /* 66 */ { 0, BX_IA_ERROR     },
  /* F2 */ { 0, BX_IA_ERROR     },
  /* F3 */ { 0, BX_IA_POPCNT_GqEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[4] = {
  /* -- */ { 0, BX_IA_CMPPS_VpsWpsIb },
  /* 66 */ { 0, BX_IA_CMPPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_CMPSD_VsdWsdIb },
  /* F3 */ { 0, BX_IA_CMPSS_VssWssIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc3M[4] = {
  /* -- */ { 0, BX_IA_MOVNTI_MdGd },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc3qM[4] = {
  /* -- */ { 0, BX_IA_MOVNTI_MqGq },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PINSRW_PqEwIb  },
  /* 66 */ { BxImmediate_Ib, BX_IA_PINSRW_VdqEwIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_PEXTRW_GdPqIb   },
  /* 66 */ { BxImmediate_Ib, BX_IA_PEXTRW_GdUdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[4] = {
  /* -- */ { BxImmediate_Ib, BX_IA_SHUFPS_VpsWpsIb },
  /* 66 */ { BxImmediate_Ib, BX_IA_SHUFPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ADDSUBPD_VpdWpd },	// SSE3
  /* F2 */ { 0, BX_IA_ADDSUBPS_VpsWps },	// SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[4] = {
  /* -- */ { 0, BX_IA_PSRLW_PqQq   },
  /* 66 */ { 0, BX_IA_PSRLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[4] = {
  /* -- */ { 0, BX_IA_PSRLD_PqQq   },
  /* 66 */ { 0, BX_IA_PSRLD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[4] = {
  /* -- */ { 0, BX_IA_PSRLQ_PqQq   },
  /* 66 */ { 0, BX_IA_PSRLQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[4] = {
  /* -- */ { 0, BX_IA_PADDQ_PqQq   },
  /* 66 */ { 0, BX_IA_PADDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[4] = {
  /* -- */ { 0, BX_IA_PMULLW_PqQq   },
  /* 66 */ { 0, BX_IA_PMULLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_MOVQ_WqVq     },
  /* F2 */ { 0, BX_IA_MOVDQ2Q_PqVRq },
  /* F3 */ { 0, BX_IA_MOVQ2DQ_VdqQq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[4] = {
  /* -- */ { 0, BX_IA_PMOVMSKB_GdPRq  },
  /* 66 */ { 0, BX_IA_PMOVMSKB_GdUdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[4] = {
  /* -- */ { 0, BX_IA_PSUBUSB_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBUSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[4] = {
  /* -- */ { 0, BX_IA_PSUBUSW_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBUSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[4] = {
  /* -- */ { 0, BX_IA_PMINUB_PqQq   },
  /* 66 */ { 0, BX_IA_PMINUB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[4] = {
  /* -- */ { 0, BX_IA_PAND_PqQq   },
  /* 66 */ { 0, BX_IA_PAND_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[4] = {
  /* -- */ { 0, BX_IA_PADDUSB_PqQq   },
  /* 66 */ { 0, BX_IA_PADDUSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[4] = {
  /* -- */ { 0, BX_IA_PADDUSW_PqQq   },
  /* 66 */ { 0, BX_IA_PADDUSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[4] = {
  /* -- */ { 0, BX_IA_PMAXUB_PqQq   },
  /* 66 */ { 0, BX_IA_PMAXUB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[4] = {
  /* -- */ { 0, BX_IA_PANDN_PqQq   },
  /* 66 */ { 0, BX_IA_PANDN_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[4] = {
  /* -- */ { 0, BX_IA_PAVGB_PqQq   },
  /* 66 */ { 0, BX_IA_PAVGB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[4] = {
  /* -- */ { 0, BX_IA_PSRAW_PqQq   },
  /* 66 */ { 0, BX_IA_PSRAW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[4] = {
  /* -- */ { 0, BX_IA_PSRAD_PqQq   },
  /* 66 */ { 0, BX_IA_PSRAD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[4] = {
  /* -- */ { 0, BX_IA_PAVGW_PqQq   },
  /* 66 */ { 0, BX_IA_PAVGW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[4] = {
  /* -- */ { 0, BX_IA_PMULHUW_PqQq   },
  /* 66 */ { 0, BX_IA_PMULHUW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[4] = {
  /* -- */ { 0, BX_IA_PMULHW_PqQq   },
  /* 66 */ { 0, BX_IA_PMULHW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_CVTTPD2DQ_VqWpd },
  /* F2 */ { 0, BX_IA_CVTPD2DQ_VqWpd  },
  /* F3 */ { 0, BX_IA_CVTDQ2PD_VpdWq  }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[4] = {
  /* -- */ { 0, BX_IA_MOVNTQ_MqPq    },
  /* 66 */ { 0, BX_IA_MOVNTDQ_MdqVdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[4] = {
  /* -- */ { 0, BX_IA_PSUBSB_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[4] = {
  /* -- */ { 0, BX_IA_PSUBSW_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[4] = {
  /* -- */ { 0, BX_IA_PMINSW_PqQq   },
  /* 66 */ { 0, BX_IA_PMINSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[4] = {
  /* -- */ { 0, BX_IA_POR_PqQq   },
  /* 66 */ { 0, BX_IA_POR_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[4] = {
  /* -- */ { 0, BX_IA_PADDSB_PqQq   },
  /* 66 */ { 0, BX_IA_PADDSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[4] = {
  /* -- */ { 0, BX_IA_PADDSW_PqQq   },
  /* 66 */ { 0, BX_IA_PADDSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[4] = {
  /* -- */ { 0, BX_IA_PMAXSW_PqQq   },
  /* 66 */ { 0, BX_IA_PMAXSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[4] = {
  /* -- */ { 0, BX_IA_PXOR_PqQq   },
  /* 66 */ { 0, BX_IA_PXOR_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff0M[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_LDDQU_VdqMdq },	// SSE3
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[4] = {
  /* -- */ { 0, BX_IA_PSLLW_PqQq   },
  /* 66 */ { 0, BX_IA_PSLLW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[4] = {
  /* -- */ { 0, BX_IA_PSLLD_PqQq   },
  /* 66 */ { 0, BX_IA_PSLLD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[4] = {
  /* -- */ { 0, BX_IA_PSLLQ_PqQq   },
  /* 66 */ { 0, BX_IA_PSLLQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[4] = {
  /* -- */ { 0, BX_IA_PMULUDQ_PqQq   },
  /* 66 */ { 0, BX_IA_PMULUDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[4] = {
  /* -- */ { 0, BX_IA_PMADDWD_PqQq   },
  /* 66 */ { 0, BX_IA_PMADDWD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[4] = {
  /* -- */ { 0, BX_IA_PSADBW_PqQq   },
  /* 66 */ { 0, BX_IA_PSADBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7M[4] = {
  /* -- */ { 0, BX_IA_MASKMOVQ_PqPRq     },
  /* 66 */ { 0, BX_IA_MASKMOVDQU_VdqUdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[4] = {
  /* -- */ { 0, BX_IA_PSUBB_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[4] = {
  /* -- */ { 0, BX_IA_PSUBW_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[4] = {
  /* -- */ { 0, BX_IA_PSUBD_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[4] = {
  /* -- */ { 0, BX_IA_PSUBQ_PqQq   },
  /* 66 */ { 0, BX_IA_PSUBQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[4] = {
  /* -- */ { 0, BX_IA_PADDB_PqQq   },
  /* 66 */ { 0, BX_IA_PADDB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[4] = {
  /* -- */ { 0, BX_IA_PADDW_PqQq   },
  /* 66 */ { 0, BX_IA_PADDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[4] = {
  /* -- */ { 0, BX_IA_PADDD_PqQq   },
  /* 66 */ { 0, BX_IA_PADDD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[4] = {
  /* -- */ { 0, BX_IA_PSRLW_PqIb  },
  /* 66 */ { 0, BX_IA_PSRLW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[4] = {
  /* -- */ { 0, BX_IA_PSRAW_PqIb  },
  /* 66 */ { 0, BX_IA_PSRAW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[4] = {
  /* -- */ { 0, BX_IA_PSLLW_PqIb  },
  /* 66 */ { 0, BX_IA_PSLLW_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[4] = {
  /* -- */ { 0, BX_IA_PSRLD_PqIb  },
  /* 66 */ { 0, BX_IA_PSRLD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[4] = {
  /* -- */ { 0, BX_IA_PSRAD_PqIb  },
  /* 66 */ { 0, BX_IA_PSRAD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[4] = {
  /* -- */ { 0, BX_IA_PSLLD_PqIb  },
  /* 66 */ { 0, BX_IA_PSLLD_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[4] = {
  /* -- */ { 0, BX_IA_PSRLQ_PqIb  },
  /* 66 */ { 0, BX_IA_PSRLQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1403[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PSRLDQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[4] = {
  /* -- */ { 0, BX_IA_PSLLQ_PqIb  },
  /* 66 */ { 0, BX_IA_PSLLQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1407[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PSLLDQ_UdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[4] = {
  /* -- */ { 0, BX_IA_PSHUFB_PqQq },
  /* 66 */ { 0, BX_IA_PSHUFB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[4] = {
  /* -- */ { 0, BX_IA_PHADDW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[4] = {
  /* -- */ { 0, BX_IA_PHADDD_PqQq },
  /* 66 */ { 0, BX_IA_PHADDD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[4] = {
  /* -- */ { 0, BX_IA_PHADDSW_PqQq },
  /* 66 */ { 0, BX_IA_PHADDSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[4] = {
  /* -- */ { 0, BX_IA_PMADDUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PMADDUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[4] = {
  /* -- */ { 0, BX_IA_PHSUBW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[4] = {
  /* -- */ { 0, BX_IA_PHSUBD_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[4] = {
  /* -- */ { 0, BX_IA_PHSUBSW_PqQq },
  /* 66 */ { 0, BX_IA_PHSUBSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[4] = {
  /* -- */ { 0, BX_IA_PSIGNB_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[4] = {
  /* -- */ { 0, BX_IA_PSIGNW_PqQq },
  /* 66 */ { 0, BX_IA_PSIGNW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3810[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PBLENDVB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3814[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDVPS_VpsWps },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3815[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDVPD_VpdWpd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3817[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PTEST_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3820[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBW_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3821[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBD_VdqWd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3822[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXBQ_VdqWw },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3823[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXWD_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3824[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXWQ_VdqWd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3825[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVSXDQ_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3828[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMULDQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3829[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPEQQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f382a[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_MOVNTDQA_VdqMdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f382b[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PACKUSDW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[4] = {
  /* -- */ { 0, BX_IA_PSIGND_PqQq },
  /* 66 */ { 0, BX_IA_PSIGND_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[4] = {
  /* -- */ { 0, BX_IA_PMULHRSW_PqQq },
  /* 66 */ { 0, BX_IA_PMULHRSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[4] = {
  /* -- */ { 0, BX_IA_PABSB_PqQq },
  /* 66 */ { 0, BX_IA_PABSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[4] = {
  /* -- */ { 0, BX_IA_PABSW_PqQq },
  /* 66 */ { 0, BX_IA_PABSW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[4] = {
  /* -- */ { 0, BX_IA_PABSD_PqQq },
  /* 66 */ { 0, BX_IA_PABSD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3830[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBW_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3831[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBD_VdqWd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3832[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXBQ_VdqWw },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3833[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXWD_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3834[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXWQ_VdqWd },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3835[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMOVZXDQ_VdqWq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3837[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPGTQ_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3838[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3839[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINSD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383a[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINUW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383b[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMINUD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383c[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXSB_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383d[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXSD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383e[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXUW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383f[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMAXUD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3840[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PMULLD_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3841[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PHMINPOSUW_VdqWdq },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEb },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ERROR },
  /* F2 */ { 0, BX_IA_CRC32_GdEv },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a08[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ROUNDPS_VpsWpsIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a09[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ROUNDPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0a[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ROUNDSS_VssWssIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0b[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_ROUNDSD_VsdWsdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0c[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDPS_VpsWpsIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0d[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_BLENDPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0e[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PBLENDW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0f[4] = {
  /* -- */ { 0, BX_IA_PALIGNR_PqQqIb },
  /* 66 */ { 0, BX_IA_PALIGNR_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a14[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PEXTRB_HbdUdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a15[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PEXTRW_HwdUdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a16[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PEXTRD_HdUdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a17[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_EXTRACTPS_HdUpsIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a20[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PINSRB_VdqEbIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a21[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_INSERTPS_VpsWssIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a22[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PINSRD_VdqEdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a40[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_DPPS_VpsWpsIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a41[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_DPPD_VpdWpdIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a42[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_MPSADBW_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a60[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPESTRM_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a61[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPESTRI_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a62[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPISTRM_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a63[4] = {
  /* -- */ { 0, BX_IA_ERROR },
  /* 66 */ { 0, BX_IA_PCMPISTRI_VdqWdqIb },
  /* F2 */ { 0, BX_IA_ERROR },
  /* F3 */ { 0, BX_IA_ERROR }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

static const BxOpcodeInfo_t BxOpcode3ByteTable0f38[256] = {
  /* 00 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3800 },
  /* 01 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3801 },
  /* 02 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3802 },
  /* 03 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3803 },
  /* 04 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3804 },
  /* 05 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3805 },
  /* 06 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3806 },
  /* 07 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3807 },
  /* 08 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3808 },
  /* 09 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3809 },
  /* 0A */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f380a },
  /* 0B */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f380b },
  /* 0C */ { 0, BX_IA_ERROR },
  /* 0D */ { 0, BX_IA_ERROR },
  /* 0E */ { 0, BX_IA_ERROR },
  /* 0F */ { 0, BX_IA_ERROR },
  /* 10 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3810 },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3814 },
  /* 15 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3815 },
  /* 16 */ { 0, BX_IA_ERROR },
  /* 17 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3817 },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f381c },
  /* 1D */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f381d },
  /* 1E */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f381e },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3820 },
  /* 21 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3821 },
  /* 22 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3822 },
  /* 23 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3823 },
  /* 24 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3824 },
  /* 25 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3825 },
  /* 26 */ { 0, BX_IA_ERROR },
  /* 27 */ { 0, BX_IA_ERROR },
  /* 28 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3828 },
  /* 29 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3829 },
  /* 2A */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f382a },
  /* 2B */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f382b },
  /* 2C */ { 0, BX_IA_ERROR },
  /* 2D */ { 0, BX_IA_ERROR },
  /* 2E */ { 0, BX_IA_ERROR },
  /* 2F */ { 0, BX_IA_ERROR },
  /* 30 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3830 },
  /* 31 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3831 },
  /* 32 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3832 },
  /* 33 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3833 },
  /* 34 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3834 },
  /* 35 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3835 },
  /* 36 */ { 0, BX_IA_ERROR },
  /* 37 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3837 },
  /* 38 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3838 },
  /* 39 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3839 },
  /* 3A */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383a },
  /* 3B */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383b },
  /* 3C */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383c },
  /* 3D */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383d },
  /* 3E */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383e },
  /* 3F */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f383f },
  /* 40 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3840 },
  /* 41 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3841 },
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
  /* DB */ { 0, BX_IA_ERROR },
  /* DC */ { 0, BX_IA_ERROR },
  /* DD */ { 0, BX_IA_ERROR },
  /* DE */ { 0, BX_IA_ERROR },
  /* DF */ { 0, BX_IA_ERROR },
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
  /* F0 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f38f0 },
  /* F1 */ { BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f38f1 },
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
  /* 08 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a08 },
  /* 09 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a09 },
  /* 0A */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0a },
  /* 0B */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0b },
  /* 0C */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0c },
  /* 0D */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0d },
  /* 0E */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0e },
  /* 0F */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a0f },
  /* 10 */ { 0, BX_IA_ERROR },
  /* 11 */ { 0, BX_IA_ERROR },
  /* 12 */ { 0, BX_IA_ERROR },
  /* 13 */ { 0, BX_IA_ERROR },
  /* 14 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a14 },
  /* 15 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a15 },
  /* 16 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a16 },
  /* 17 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a17 },
  /* 18 */ { 0, BX_IA_ERROR },
  /* 19 */ { 0, BX_IA_ERROR },
  /* 1A */ { 0, BX_IA_ERROR },
  /* 1B */ { 0, BX_IA_ERROR },
  /* 1C */ { 0, BX_IA_ERROR },
  /* 1D */ { 0, BX_IA_ERROR },
  /* 1E */ { 0, BX_IA_ERROR },
  /* 1F */ { 0, BX_IA_ERROR },
  /* 20 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a20 },
  /* 21 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a21 },
  /* 22 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a22 },
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
  /* 40 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a40 },
  /* 41 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a41 },
  /* 42 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a42 },
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
  /* 60 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a60 },
  /* 61 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a61 },
  /* 62 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a62 },
  /* 63 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_0f3a63 },
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
  /* DF */ { 0, BX_IA_ERROR },
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

#endif // BX_SUPPORT_SSE >= 4 || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

/* ************************************************************************ */
/* Opcode Groups */

/* ******* */
/* Group 1 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EbIbM },
  /* 1 */ { BxLockable, BX_IA_OR_EbIbM  },
  /* 2 */ { BxLockable, BX_IA_ADC_EbIbM },
  /* 3 */ { BxLockable, BX_IA_SBB_EbIbM },
  /* 4 */ { BxLockable, BX_IA_AND_EbIbM },
  /* 5 */ { BxLockable, BX_IA_SUB_EbIbM },
  /* 6 */ { BxLockable, BX_IA_XOR_EbIbM },
  /* 7 */ { 0,          BX_IA_CMP_EbIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EbIbR },
  /* 1 */ { 0, BX_IA_OR_EbIbR  },
  /* 2 */ { 0, BX_IA_ADC_EbIbR },
  /* 3 */ { 0, BX_IA_SBB_EbIbR },
  /* 4 */ { 0, BX_IA_AND_EbIbR },
  /* 5 */ { 0, BX_IA_SUB_EbIbR },
  /* 6 */ { 0, BX_IA_XOR_EbIbR },
  /* 7 */ { 0, BX_IA_CMP_EbIbR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EwM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EwIwM },
  /* 1 */ { BxLockable, BX_IA_OR_EwIwM },
  /* 2 */ { BxLockable, BX_IA_ADC_EwIwM },
  /* 3 */ { BxLockable, BX_IA_SBB_EwIwM },
  /* 4 */ { BxLockable, BX_IA_AND_EwIwM },
  /* 5 */ { BxLockable, BX_IA_SUB_EwIwM },
  /* 6 */ { BxLockable, BX_IA_XOR_EwIwM },
  /* 7 */ { 0,          BX_IA_CMP_EwIwM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EwR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EwIwR },
  /* 1 */ { 0, BX_IA_OR_EwIwR },
  /* 2 */ { 0, BX_IA_ADC_EwIwR },
  /* 3 */ { 0, BX_IA_SBB_EwIwR },
  /* 4 */ { 0, BX_IA_AND_EwIwR },
  /* 5 */ { 0, BX_IA_SUB_EwIwR },
  /* 6 */ { 0, BX_IA_XOR_EwIwR },
  /* 7 */ { 0, BX_IA_CMP_EwIwR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EdM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EdIdM },
  /* 1 */ { BxLockable, BX_IA_OR_EdIdM },
  /* 2 */ { BxLockable, BX_IA_ADC_EdIdM },
  /* 3 */ { BxLockable, BX_IA_SBB_EdIdM },
  /* 4 */ { BxLockable, BX_IA_AND_EdIdM },
  /* 5 */ { BxLockable, BX_IA_SUB_EdIdM },
  /* 6 */ { BxLockable, BX_IA_XOR_EdIdM },
  /* 7 */ { 0,          BX_IA_CMP_EdIdM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG1EdR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EdIdR },
  /* 1 */ { 0, BX_IA_OR_EdIdR },
  /* 2 */ { 0, BX_IA_ADC_EdIdR },
  /* 3 */ { 0, BX_IA_SBB_EdIdR },
  /* 4 */ { 0, BX_IA_AND_EdIdR },
  /* 5 */ { 0, BX_IA_SUB_EdIdR },
  /* 6 */ { 0, BX_IA_XOR_EdIdR },
  /* 7 */ { 0, BX_IA_CMP_EdIdR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1EqM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_ADD_EqIdM },
  /* 1 */ { BxLockable, BX_IA_OR_EqIdM },
  /* 2 */ { BxLockable, BX_IA_ADC_EqIdM },
  /* 3 */ { BxLockable, BX_IA_SBB_EqIdM },
  /* 4 */ { BxLockable, BX_IA_AND_EqIdM },
  /* 5 */ { BxLockable, BX_IA_SUB_EqIdM },
  /* 6 */ { BxLockable, BX_IA_XOR_EqIdM },
  /* 7 */ { 0,          BX_IA_CMP_EqIdM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G1EqR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ADD_EqIdR },
  /* 1 */ { 0, BX_IA_OR_EqIdR },
  /* 2 */ { 0, BX_IA_ADC_EqIdR },
  /* 3 */ { 0, BX_IA_SBB_EqIdR },
  /* 4 */ { 0, BX_IA_AND_EqIdR },
  /* 5 */ { 0, BX_IA_SUB_EqIdR },
  /* 6 */ { 0, BX_IA_XOR_EqIdR },
  /* 7 */ { 0, BX_IA_CMP_EqIdR }
};
#endif

/* ******* */
/* Group 2 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG2Eb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Eb },
  /* 1 */ { 0, BX_IA_ROR_Eb },
  /* 2 */ { 0, BX_IA_RCL_Eb },
  /* 3 */ { 0, BX_IA_RCR_Eb },
  /* 4 */ { 0, BX_IA_SHL_Eb },
  /* 5 */ { 0, BX_IA_SHR_Eb },
  /* 6 */ { 0, BX_IA_SHL_Eb },
  /* 7 */ { 0, BX_IA_SAR_Eb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2Ew[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Ew },
  /* 1 */ { 0, BX_IA_ROR_Ew },
  /* 2 */ { 0, BX_IA_RCL_Ew },
  /* 3 */ { 0, BX_IA_RCR_Ew },
  /* 4 */ { 0, BX_IA_SHL_Ew },
  /* 5 */ { 0, BX_IA_SHR_Ew },
  /* 6 */ { 0, BX_IA_SHL_Ew },
  /* 7 */ { 0, BX_IA_SAR_Ew }
};

static const BxOpcodeInfo_t BxOpcodeInfoG2Ed[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Ed },
  /* 1 */ { 0, BX_IA_ROR_Ed },
  /* 2 */ { 0, BX_IA_RCL_Ed },
  /* 3 */ { 0, BX_IA_RCR_Ed },
  /* 4 */ { 0, BX_IA_SHL_Ed },
  /* 5 */ { 0, BX_IA_SHR_Ed },
  /* 6 */ { 0, BX_IA_SHL_Ed },
  /* 7 */ { 0, BX_IA_SAR_Ed }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G2Eq[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_ROL_Eq },
  /* 1 */ { 0, BX_IA_ROR_Eq },
  /* 2 */ { 0, BX_IA_RCL_Eq },
  /* 3 */ { 0, BX_IA_RCR_Eq },
  /* 4 */ { 0, BX_IA_SHL_Eq },
  /* 5 */ { 0, BX_IA_SHR_Eq },
  /* 6 */ { 0, BX_IA_SHL_Eq },
  /* 7 */ { 0, BX_IA_SAR_Eq }
};
#endif

/* ******* */
/* Group 3 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG3EbM[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_TEST_EbIbM },
  /* 1 */ { BxImmediate_Ib, BX_IA_TEST_EbIbM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EbM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EbM },
  /* 4 */ { 0,              BX_IA_MUL_ALEb },
  /* 5 */ { 0,              BX_IA_IMUL_ALEb },
  /* 6 */ { 0,              BX_IA_DIV_ALEb },
  /* 7 */ { 0,              BX_IA_IDIV_ALEb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EbR[8] = {
  /* 0 */ { BxImmediate_Ib, BX_IA_TEST_EbIbR },
  /* 1 */ { BxImmediate_Ib, BX_IA_TEST_EbIbR },
  /* 2 */ { 0, BX_IA_NOT_EbR },
  /* 3 */ { 0, BX_IA_NEG_EbR },
  /* 4 */ { 0, BX_IA_MUL_ALEb },
  /* 5 */ { 0, BX_IA_IMUL_ALEb },
  /* 6 */ { 0, BX_IA_DIV_ALEb },
  /* 7 */ { 0, BX_IA_IDIV_ALEb }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EwM[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_TEST_EwIwM },
  /* 1 */ { BxImmediate_Iw, BX_IA_TEST_EwIwM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EwM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EwM },
  /* 4 */ { 0,              BX_IA_MUL_AXEw },
  /* 5 */ { 0,              BX_IA_IMUL_AXEw },
  /* 6 */ { 0,              BX_IA_DIV_AXEw },
  /* 7 */ { 0,              BX_IA_IDIV_AXEw }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EwR[8] = {
  /* 0 */ { BxImmediate_Iw, BX_IA_TEST_EwIwR },
  /* 1 */ { BxImmediate_Iw, BX_IA_TEST_EwIwR },
  /* 2 */ { 0, BX_IA_NOT_EwR },
  /* 3 */ { 0, BX_IA_NEG_EwR },
  /* 4 */ { 0, BX_IA_MUL_AXEw },
  /* 5 */ { 0, BX_IA_IMUL_AXEw },
  /* 6 */ { 0, BX_IA_DIV_AXEw },
  /* 7 */ { 0, BX_IA_IDIV_AXEw }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EdM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EdIdM },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EdIdM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EdM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EdM },
  /* 4 */ { 0,              BX_IA_MUL_EAXEd },
  /* 5 */ { 0,              BX_IA_IMUL_EAXEd },
  /* 6 */ { 0,              BX_IA_DIV_EAXEd },
  /* 7 */ { 0,              BX_IA_IDIV_EAXEd }
};

static const BxOpcodeInfo_t BxOpcodeInfoG3EdR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EdIdR },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EdIdR },
  /* 2 */ { 0, BX_IA_NOT_EdR },
  /* 3 */ { 0, BX_IA_NEG_EdR },
  /* 4 */ { 0, BX_IA_MUL_EAXEd },
  /* 5 */ { 0, BX_IA_IMUL_EAXEd },
  /* 6 */ { 0, BX_IA_DIV_EAXEd },
  /* 7 */ { 0, BX_IA_IDIV_EAXEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G3EqM[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EqIdM },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EqIdM },
  /* 2 */ { BxLockable,     BX_IA_NOT_EqM },
  /* 3 */ { BxLockable,     BX_IA_NEG_EqM },
  /* 4 */ { 0,              BX_IA_MUL_RAXEq },
  /* 5 */ { 0,              BX_IA_IMUL_RAXEq },
  /* 6 */ { 0,              BX_IA_DIV_RAXEq },
  /* 7 */ { 0,              BX_IA_IDIV_RAXEq }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G3EqR[8] = {
  /* 0 */ { BxImmediate_Id, BX_IA_TEST_EqIdR },
  /* 1 */ { BxImmediate_Id, BX_IA_TEST_EqIdR },
  /* 2 */ { 0, BX_IA_NOT_EqR },
  /* 3 */ { 0, BX_IA_NEG_EqR },
  /* 4 */ { 0, BX_IA_MUL_RAXEq },
  /* 5 */ { 0, BX_IA_IMUL_RAXEq },
  /* 6 */ { 0, BX_IA_DIV_RAXEq },
  /* 7 */ { 0, BX_IA_IDIV_RAXEq }
};
#endif

/* ******* */
/* Group 4 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG4M[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EbM },
  /* 1 */ { BxLockable, BX_IA_DEC_EbM },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG4R[8] = {
  /* 0 */ { 0, BX_IA_INC_EbR },
  /* 1 */ { 0, BX_IA_DEC_EbR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 5 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG5wM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_EwM },
  /* 1 */ { BxLockable, BX_IA_DEC_EwM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EwM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL16_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EwM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EwM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5wR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_INC_RX },  // DEC_EwR
  /* 1 */ { 0, BX_IA_DEC_RX },  // DEC_EwR
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EwR },
  /* 3 */ { 0, BX_IA_ERROR }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EwR  },
  /* 5 */ { 0, BX_IA_ERROR }, // JMP16_Ep
  /* 6 */ { 0, BX_IA_PUSH_RX }, // PUSH_EwR
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5dM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, BX_IA_INC_EdM },
  /* 1 */ { BxLockable, BX_IA_DEC_EdM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EdM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL32_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EdM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EdM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG5dR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_INC_ERX },  // INC_EdR
  /* 1 */ { 0, BX_IA_DEC_ERX },  // DEC_EdR
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EdR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL32_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EdR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP32_Ep
  /* 6 */ { 0, BX_IA_PUSH_ERX }, // PUSH_EdR
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G5wM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EwM },
  /* 1 */ { BxLockable, BX_IA_DEC_EwM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL16_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP16_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EwM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5wR[8] = {
  /* 0 */ { 0, BX_IA_INC_RX },  // INC_EwR
  /* 1 */ { 0, BX_IA_DEC_RX },  // DEC_EwR
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR }, // JMP16_Ep
  /* 6 */ { 0, BX_IA_PUSH_RX }, // PUSH_EwR
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EdM },
  /* 1 */ { BxLockable, BX_IA_DEC_EdM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL32_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP32_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EqM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dR[8] = {
  /* 0 */ { 0, BX_IA_INC_ERX },  // INC_EdR
  /* 1 */ { 0, BX_IA_DEC_ERX },  // DEC_EdR
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL32_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP32_Ep
  /* 6 */ { 0, BX_IA_PUSH_RRX }, // PUSH_EqR
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qM[8] = {
  /* 0 */ { BxLockable, BX_IA_INC_EqM },
  /* 1 */ { BxLockable, BX_IA_DEC_EqM },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqM },
  /* 3 */ { BxTraceEnd, BX_IA_CALL64_Ep },
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqM },
  /* 5 */ { BxTraceEnd, BX_IA_JMP64_Ep },
  /* 6 */ { 0, BX_IA_PUSH_EqM },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qR[8] = {
  /* 0 */ { 0, BX_IA_INC_EqR },
  /* 1 */ { 0, BX_IA_DEC_EqR },
  /* 2 */ { BxTraceEnd, BX_IA_CALL_EqR },
  /* 3 */ { 0, BX_IA_ERROR },  // CALL64_Ep
  /* 4 */ { BxTraceEnd, BX_IA_JMP_EqR  },
  /* 5 */ { 0, BX_IA_ERROR },  // JMP64_Ep
  /* 6 */ { 0, BX_IA_PUSH_RRX }, // PUSH_EqR
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******* */
/* Group 6 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG6[8] = {
  /* 0 */ { 0, BX_IA_SLDT_Ew },
  /* 1 */ { 0, BX_IA_STR_Ew },
  /* 2 */ { BxTraceEnd, BX_IA_LLDT_Ew },
  /* 3 */ { BxTraceEnd, BX_IA_LTR_Ew },
  /* 4 */ { 0, BX_IA_VERR_Ew },
  /* 5 */ { 0, BX_IA_VERW_Ew },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******* */
/* Group 7 */
/* ******* */

// ----------------------------------------------------
//     MOD    REG  RM  | non 64 bit mode | 64 bit mode
// ----------------------------------------------------
//  MOD <> 11  7   --- |     INVLPG      |   INVLPG
//  MOD == 11  7    0  |      #UD        |   SWAPGS
//  MOD == 11  7    1  |      #UD        |   RDTSCP
//  MOD == 11  7   2-7 |      #UD        |    #UD

static const BxOpcodeInfo_t opcodesGroupRmMONITOR[8] = {
  /* 0 */ { 0, BX_IA_MONITOR },
  /* 1 */ { BxTraceEnd, BX_IA_MWAIT },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t opcodesGroupRmXGETSET[8] = {
  /* 0 */ { 0, BX_IA_XGETBV  },
  /* 1 */ { 0, BX_IA_XSETBV  },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG7R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxRMGroup, BX_IA_ERROR, opcodesGroupRmMONITOR },
  /* 2 */ { BxRMGroup, BX_IA_ERROR, opcodesGroupRmXGETSET },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG7M[8] = {
  // attributes defined in main area
  /* 0 */ { 0, BX_IA_SGDT_Ms },
  /* 1 */ { 0, BX_IA_SIDT_Ms },
  /* 2 */ { BxTraceEnd, BX_IA_LGDT_Ms },
  /* 3 */ { BxTraceEnd, BX_IA_LIDT_Ms },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { BxTraceEnd, BX_IA_INVLPG }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t opcodesGroup64RmINVLPG[8] = {
  /* 0 */ { 0, BX_IA_SWAPGS },
  /* 1 */ { 0, BX_IA_RDTSCP },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G7R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxRMGroup, BX_IA_ERROR, opcodesGroupRmMONITOR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { BxRMGroup, BX_IA_ERROR, opcodesGroup64RmINVLPG }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G7M[8] = {
  /* 0 */ { 0, BX_IA_SGDT64_Ms },
  /* 1 */ { 0, BX_IA_SIDT64_Ms },
  /* 2 */ { BxTraceEnd, BX_IA_LGDT64_Ms },
  /* 3 */ { BxTraceEnd, BX_IA_LIDT64_Ms },
  /* 4 */ { 0, BX_IA_SMSW_Ew },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxTraceEnd, BX_IA_LMSW_Ew },
  /* 7 */ { BxTraceEnd, BX_IA_INVLPG  }
};
#endif


/* ******* */
/* Group 8 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EwIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EwIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EwIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EwIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EwIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EwIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EwIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EwIbR }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EdIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EdIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EdIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EdIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EdIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EdIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EdIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EdIbR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib,              BX_IA_BT_EqIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, BX_IA_BTS_EqIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, BX_IA_BTR_EqIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, BX_IA_BTC_EqIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbR[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib, BX_IA_BT_EqIbR  },
  /* 5 */ { BxImmediate_Ib, BX_IA_BTS_EqIbR },
  /* 6 */ { BxImmediate_Ib, BX_IA_BTR_EqIbR },
  /* 7 */ { BxImmediate_Ib, BX_IA_BTC_EqIbR }
};
#endif

/* ******* */
/* Group 9 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG9M[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG8B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G9qM[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { BxLockable, BX_IA_CMPXCHG16B },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { 0, BX_IA_ERROR },
  /* 7 */ { 0, BX_IA_ERROR }
};
#endif

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG12[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1202 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1204 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1206 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG13[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1302 },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1304 },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1306 },
  /* 7 */ { 0, BX_IA_ERROR }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG14[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1402 },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1403 },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_ERROR },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1406 },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE, BX_IA_ERROR, BxOpcodeGroupSSE_G1407 }
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG15M[8] = {
  /* 0 */ { 0, BX_IA_FXSAVE  },
  /* 1 */ { 0, BX_IA_FXRSTOR },
  /* 2 */ { 0, BX_IA_LDMXCSR },
  /* 3 */ { 0, BX_IA_STMXCSR },
  /* 4 */ { 0, BX_IA_XSAVE   },
  /* 5 */ { 0, BX_IA_XRSTOR  },
  /* 6 */ { 0, BX_IA_ERROR   },
  /* 7 */ { 0, BX_IA_CLFLUSH }
};

static const BxOpcodeInfo_t BxOpcodeInfoG15R[8] = {
  /* 0 */ { 0, BX_IA_ERROR },
  /* 1 */ { 0, BX_IA_ERROR },
  /* 2 */ { 0, BX_IA_ERROR },
  /* 3 */ { 0, BX_IA_ERROR },
  /* 4 */ { 0, BX_IA_ERROR },
  /* 5 */ { 0, BX_IA_NOP }, /* LFENCE */
  /* 6 */ { 0, BX_IA_NOP }, /* MFENCE */
  /* 7 */ { 0, BX_IA_NOP }  /* SFENCE */
};

#endif // BX_COMMON_FETCHDECODE_TABLES_H
