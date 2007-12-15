/////////////////////////////////////////////////////////////////////////
// $Id: fetchdecode.h,v 1.47 2007-12-15 17:42:20 sshwarts Exp $
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
  Bit16u         Attr;
  BxExecutePtr_t ExecutePtr;
  const BxOpcodeInfo_t *AnotherArray;
} BxOpcodeInfo_t;


//
// This file contains common IA-32/X86-64 opcode tables, like FPU opcode
// table, 3DNow! opcode table or SSE opcode groups (choose the opcode
// according to instruction prefixes)
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

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u FetchQWORD(Bit8u *iptr)
{
   Bit64u data;
   ReadHostQWordFromLittleEndian(iptr, data);
   return data;
}
#endif

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
  /* 0 */ { 0,  &BX_CPU_C::FADD_SINGLE_REAL  },
  /* 1 */ { 0,  &BX_CPU_C::FMUL_SINGLE_REAL  },
  /* 2 */ { 0,  &BX_CPU_C::FCOM_SINGLE_REAL  },
  /* 3 */ { 0,  &BX_CPU_C::FCOM_SINGLE_REAL  },	        // FCOMP_SINGLE_REAL
  /* 4 */ { 0,  &BX_CPU_C::FSUB_SINGLE_REAL  },
  /* 5 */ { 0,  &BX_CPU_C::FSUBR_SINGLE_REAL },
  /* 6 */ { 0,  &BX_CPU_C::FDIV_SINGLE_REAL  },
  /* 7 */ { 0,  &BX_CPU_C::FDIVR_SINGLE_REAL }
};

  // D9 (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupD9[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FLD_SINGLE_REAL  },
  /* 1 */ { 0,  &BX_CPU_C::BxError          },
  /* 2 */ { 0,  &BX_CPU_C::FST_SINGLE_REAL  },
  /* 3 */ { 0,  &BX_CPU_C::FST_SINGLE_REAL  },	 	// FSTP_SINGLE_REAL
  /* 4 */ { 0,  &BX_CPU_C::FLDENV           },
  /* 5 */ { 0,  &BX_CPU_C::FLDCW            },
  /* 6 */ { 0,  &BX_CPU_C::FNSTENV          },
  /* 7 */ { 0,  &BX_CPU_C::FNSTCW           }
};

  // DA (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDA[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FIADD_DWORD_INTEGER  },
  /* 1 */ { 0,  &BX_CPU_C::FIMUL_DWORD_INTEGER  },
  /* 2 */ { 0,  &BX_CPU_C::FICOM_DWORD_INTEGER  },
  /* 3 */ { 0,  &BX_CPU_C::FICOM_DWORD_INTEGER  },	// FICOMP_DWORD_INTEGER
  /* 4 */ { 0,  &BX_CPU_C::FISUB_DWORD_INTEGER  },
  /* 5 */ { 0,  &BX_CPU_C::FISUBR_DWORD_INTEGER },
  /* 6 */ { 0,  &BX_CPU_C::FIDIV_DWORD_INTEGER  },
  /* 7 */ { 0,  &BX_CPU_C::FIDIVR_DWORD_INTEGER }
};

  // DB (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDB[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FILD_DWORD_INTEGER  },
  /* 1 */ { 0,  &BX_CPU_C::FISTTP32            },
  /* 2 */ { 0,  &BX_CPU_C::FIST_DWORD_INTEGER  },
  /* 3 */ { 0,  &BX_CPU_C::FIST_DWORD_INTEGER  },	// FISTP_DWORD_INTEGER
  /* 4 */ { 0,  &BX_CPU_C::BxError             },
  /* 5 */ { 0,  &BX_CPU_C::FLD_EXTENDED_REAL   },
  /* 6 */ { 0,  &BX_CPU_C::BxError             },
  /* 7 */ { 0,  &BX_CPU_C::FSTP_EXTENDED_REAL  }
};

  // DC (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDC[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FADD_DOUBLE_REAL  },
  /* 1 */ { 0,  &BX_CPU_C::FMUL_DOUBLE_REAL  },
  /* 2 */ { 0,  &BX_CPU_C::FCOM_DOUBLE_REAL  },
  /* 3 */ { 0,  &BX_CPU_C::FCOM_DOUBLE_REAL  },	        // FCOMP_DOUBLE_REAL
  /* 4 */ { 0,  &BX_CPU_C::FSUB_DOUBLE_REAL  },
  /* 5 */ { 0,  &BX_CPU_C::FSUBR_DOUBLE_REAL },
  /* 6 */ { 0,  &BX_CPU_C::FDIV_DOUBLE_REAL  },
  /* 7 */ { 0,  &BX_CPU_C::FDIVR_DOUBLE_REAL }
};

  // DD (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDD[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FLD_DOUBLE_REAL },
  /* 1 */ { 0,  &BX_CPU_C::FISTTP64        },
  /* 2 */ { 0,  &BX_CPU_C::FST_DOUBLE_REAL },
  /* 3 */ { 0,  &BX_CPU_C::FST_DOUBLE_REAL },		// FSTP_DOUBLE_REAL
  /* 4 */ { 0,  &BX_CPU_C::FRSTOR          },
  /* 5 */ { 0,  &BX_CPU_C::BxError         },
  /* 6 */ { 0,  &BX_CPU_C::FNSAVE          },
  /* 7 */ { 0,  &BX_CPU_C::FNSTSW          }
};

  // DE (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDE[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FIADD_WORD_INTEGER  },
  /* 1 */ { 0,  &BX_CPU_C::FIMUL_WORD_INTEGER  },
  /* 2 */ { 0,  &BX_CPU_C::FICOM_WORD_INTEGER  },
  /* 3 */ { 0,  &BX_CPU_C::FICOM_WORD_INTEGER  },	// FICOMP_WORD_INTEGER
  /* 4 */ { 0,  &BX_CPU_C::FISUB_WORD_INTEGER  },
  /* 5 */ { 0,  &BX_CPU_C::FISUBR_WORD_INTEGER },
  /* 6 */ { 0,  &BX_CPU_C::FIDIV_WORD_INTEGER  },
  /* 7 */ { 0,  &BX_CPU_C::FIDIVR_WORD_INTEGER }
};

  // DF (modrm is outside 00h - BFh) (mod != 11)
static const BxOpcodeInfo_t BxOpcodeInfo_FPGroupDF[8] = { 
  /* 0 */ { 0,  &BX_CPU_C::FILD_WORD_INTEGER   },
  /* 1 */ { 0,  &BX_CPU_C::FISTTP16            },
  /* 2 */ { 0,  &BX_CPU_C::FIST_WORD_INTEGER   },
  /* 3 */ { 0,  &BX_CPU_C::FIST_WORD_INTEGER  	},	// FISTP_WORD_INTEGER
  /* 4 */ { 0,  &BX_CPU_C::FBLD_PACKED_BCD     },
  /* 5 */ { 0,  &BX_CPU_C::FILD_QWORD_INTEGER  },
  /* 6 */ { 0,  &BX_CPU_C::FBSTP_PACKED_BCD    },
  /* 7 */ { 0,  &BX_CPU_C::FISTP_QWORD_INTEGER }
};

// 512 entries for second byte of floating point instructions. (when mod==11b) 
static const BxOpcodeInfo_t BxOpcodeInfo_FloatingPoint[512] = { 
  // D8 (modrm is outside 00h - BFh) (mod == 11)
  /* D8 C0 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C1 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C2 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C3 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C4 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C5 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C6 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C7 */ { 0,  &BX_CPU_C::FADD_ST0_STj  },
  /* D8 C8 */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 C9 */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CA */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CB */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CC */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CD */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CE */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 CF */ { 0,  &BX_CPU_C::FMUL_ST0_STj  },
  /* D8 D0 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D1 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D2 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D3 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D4 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D5 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D6 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D7 */ { 0,  &BX_CPU_C::FCOM_STi      },
  /* D8 D8 */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 D9 */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DA */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DB */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DC */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DD */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DE */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 DF */ { 0,  &BX_CPU_C::FCOM_STi      },	// FCOMP
  /* D8 E0 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E1 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E2 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E3 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E4 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E5 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E6 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E7 */ { 0,  &BX_CPU_C::FSUB_ST0_STj  },
  /* D8 E8 */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 E9 */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 EA */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 EB */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 EC */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 ED */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 EE */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 EF */ { 0,  &BX_CPU_C::FSUBR_ST0_STj },
  /* D8 F0 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F1 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F2 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F3 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F4 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F5 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F6 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F7 */ { 0,  &BX_CPU_C::FDIV_ST0_STj  },
  /* D8 F8 */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 F9 */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FA */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FB */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FC */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FD */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FE */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  /* D8 FF */ { 0,  &BX_CPU_C::FDIVR_ST0_STj },
  
  // D9 (modrm is outside 00h - BFh) (mod == 11)
  /* D9 C0 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C1 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C2 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C3 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C4 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C5 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C6 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C7 */ { 0,  &BX_CPU_C::FLD_STi  },
  /* D9 C8 */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 C9 */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CA */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CB */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CC */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CD */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CE */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 CF */ { 0,  &BX_CPU_C::FXCH_STi },
  /* D9 D0 */ { 0,  &BX_CPU_C::FNOP     },
  /* D9 D1 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D2 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D3 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D4 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D5 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D6 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D7 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D8 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 D9 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DA */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DB */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DC */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DD */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DE */ { 0,  &BX_CPU_C::BxError  },
  /* D9 DF */ { 0,  &BX_CPU_C::BxError  },
  /* D9 E0 */ { 0,  &BX_CPU_C::FCHS     },
  /* D9 E1 */ { 0,  &BX_CPU_C::FABS     },
  /* D9 E2 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 E3 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 E4 */ { 0,  &BX_CPU_C::FTST     },
  /* D9 E5 */ { 0,  &BX_CPU_C::FXAM     },
  /* D9 E6 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 E7 */ { 0,  &BX_CPU_C::BxError  },
  /* D9 E8 */ { 0,  &BX_CPU_C::FLD1     },
  /* D9 E9 */ { 0,  &BX_CPU_C::FLDL2T   },
  /* D9 EA */ { 0,  &BX_CPU_C::FLDL2E   },
  /* D9 EB */ { 0,  &BX_CPU_C::FLDPI    },
  /* D9 EC */ { 0,  &BX_CPU_C::FLDLG2   },
  /* D9 ED */ { 0,  &BX_CPU_C::FLDLN2   },
  /* D9 EE */ { 0,  &BX_CPU_C::FLDZ     },
  /* D9 EF */ { 0,  &BX_CPU_C::BxError  },
  /* D9 F0 */ { 0,  &BX_CPU_C::F2XM1    },
  /* D9 F1 */ { 0,  &BX_CPU_C::FYL2X    },
  /* D9 F2 */ { 0,  &BX_CPU_C::FPTAN    },
  /* D9 F3 */ { 0,  &BX_CPU_C::FPATAN   },
  /* D9 F4 */ { 0,  &BX_CPU_C::FXTRACT  },
  /* D9 F5 */ { 0,  &BX_CPU_C::FPREM1   },
  /* D9 F6 */ { 0,  &BX_CPU_C::FDECSTP  },
  /* D9 F7 */ { 0,  &BX_CPU_C::FINCSTP  },
  /* D9 F8 */ { 0,  &BX_CPU_C::FPREM    },
  /* D9 F9 */ { 0,  &BX_CPU_C::FYL2XP1  },
  /* D9 FA */ { 0,  &BX_CPU_C::FSQRT    },
  /* D9 FB */ { 0,  &BX_CPU_C::FSINCOS  },
  /* D9 FC */ { 0,  &BX_CPU_C::FRNDINT  },
  /* D9 FD */ { 0,  &BX_CPU_C::FSCALE   },
  /* D9 FE */ { 0,  &BX_CPU_C::FSIN     },
  /* D9 FF */ { 0,  &BX_CPU_C::FCOS     },
  
  // DA (modrm is outside 00h - BFh) (mod == 11)
  /* DA C0 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C1 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C2 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C3 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C4 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C5 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C6 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C7 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C8 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA C9 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CA */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CB */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CC */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CD */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CE */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA CF */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D0 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D1 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D2 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D3 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D4 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D5 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D6 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D7 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D8 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA D9 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DA */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DB */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DC */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DD */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DE */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA DF */ { 0,  &BX_CPU_C::FCMOV_ST0_STj },
  /* DA E0 */ { 0,  &BX_CPU_C::BxError },
  /* DA E1 */ { 0,  &BX_CPU_C::BxError },
  /* DA E2 */ { 0,  &BX_CPU_C::BxError },
  /* DA E3 */ { 0,  &BX_CPU_C::BxError },
  /* DA E4 */ { 0,  &BX_CPU_C::BxError },
  /* DA E5 */ { 0,  &BX_CPU_C::BxError },
  /* DA E6 */ { 0,  &BX_CPU_C::BxError },
  /* DA E7 */ { 0,  &BX_CPU_C::BxError },
  /* DA E8 */ { 0,  &BX_CPU_C::BxError },
  /* DA E9 */ { 0,  &BX_CPU_C::FUCOMPP },
  /* DA EA */ { 0,  &BX_CPU_C::BxError },
  /* DA EB */ { 0,  &BX_CPU_C::BxError },
  /* DA EC */ { 0,  &BX_CPU_C::BxError },
  /* DA ED */ { 0,  &BX_CPU_C::BxError },
  /* DA EE */ { 0,  &BX_CPU_C::BxError },
  /* DA EF */ { 0,  &BX_CPU_C::BxError },
  /* DA F0 */ { 0,  &BX_CPU_C::BxError },
  /* DA F1 */ { 0,  &BX_CPU_C::BxError },
  /* DA F2 */ { 0,  &BX_CPU_C::BxError },
  /* DA F3 */ { 0,  &BX_CPU_C::BxError },
  /* DA F4 */ { 0,  &BX_CPU_C::BxError },
  /* DA F5 */ { 0,  &BX_CPU_C::BxError },
  /* DA F6 */ { 0,  &BX_CPU_C::BxError },
  /* DA F7 */ { 0,  &BX_CPU_C::BxError },
  /* DA F8 */ { 0,  &BX_CPU_C::BxError },
  /* DA F9 */ { 0,  &BX_CPU_C::BxError },
  /* DA FA */ { 0,  &BX_CPU_C::BxError },
  /* DA FB */ { 0,  &BX_CPU_C::BxError },
  /* DA FC */ { 0,  &BX_CPU_C::BxError },
  /* DA FD */ { 0,  &BX_CPU_C::BxError },
  /* DA FE */ { 0,  &BX_CPU_C::BxError },
  /* DA FF */ { 0,  &BX_CPU_C::BxError },

  // DB (modrm is outside 00h - BFh) (mod == 11)
  /* DB C0 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C1 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C2 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C3 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C4 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C5 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C6 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C7 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C8 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB C9 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CA */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CB */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CC */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CD */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CE */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB CF */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D0 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D1 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D2 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D3 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D4 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D5 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D6 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D7 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D8 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB D9 */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DA */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DB */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DC */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DD */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DE */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB DF */ { 0,  &BX_CPU_C::FCMOV_ST0_STj  },
  /* DB E0 */ { 0,  &BX_CPU_C::FPLEGACY       },     // feni  (287 only)
  /* DB E1 */ { 0,  &BX_CPU_C::FPLEGACY       },     // fdisi (287 only)
  /* DB E2 */ { 0,  &BX_CPU_C::FNCLEX         },
  /* DB E3 */ { 0,  &BX_CPU_C::FNINIT         },
  /* DB E4 */ { 0,  &BX_CPU_C::FPLEGACY       },     // fsetpm (287 only)
  /* DB E5 */ { 0,  &BX_CPU_C::BxError        },
  /* DB E6 */ { 0,  &BX_CPU_C::BxError        },
  /* DB E7 */ { 0,  &BX_CPU_C::BxError        },
  /* DB E8 */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB E9 */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB EA */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB EB */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB EC */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB ED */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB EE */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB EF */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },
  /* DB F0 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F1 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F2 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F3 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F4 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F5 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F6 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F7 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },
  /* DB F8 */ { 0,  &BX_CPU_C::BxError },
  /* DB F9 */ { 0,  &BX_CPU_C::BxError },
  /* DB FA */ { 0,  &BX_CPU_C::BxError },
  /* DB FB */ { 0,  &BX_CPU_C::BxError },
  /* DB FC */ { 0,  &BX_CPU_C::BxError },
  /* DB FD */ { 0,  &BX_CPU_C::BxError },
  /* DB FE */ { 0,  &BX_CPU_C::BxError },
  /* DB FF */ { 0,  &BX_CPU_C::BxError },

  // DC (modrm is outside 00h - BFh) (mod == 11)
  /* DC C0 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C1 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C2 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C3 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C4 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C5 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C6 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C7 */ { 0,  &BX_CPU_C::FADD_STi_ST0 },
  /* DC C8 */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC C9 */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CA */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CB */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CC */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CD */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CE */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC CF */ { 0,  &BX_CPU_C::FMUL_STi_ST0 },
  /* DC D0 */ { 0,  &BX_CPU_C::BxError },
  /* DC D1 */ { 0,  &BX_CPU_C::BxError },
  /* DC D2 */ { 0,  &BX_CPU_C::BxError },
  /* DC D3 */ { 0,  &BX_CPU_C::BxError },
  /* DC D4 */ { 0,  &BX_CPU_C::BxError },
  /* DC D5 */ { 0,  &BX_CPU_C::BxError },
  /* DC D6 */ { 0,  &BX_CPU_C::BxError },
  /* DC D7 */ { 0,  &BX_CPU_C::BxError },
  /* DC D8 */ { 0,  &BX_CPU_C::BxError },
  /* DC D9 */ { 0,  &BX_CPU_C::BxError },
  /* DC DA */ { 0,  &BX_CPU_C::BxError },
  /* DC DB */ { 0,  &BX_CPU_C::BxError },
  /* DC DC */ { 0,  &BX_CPU_C::BxError },
  /* DC DD */ { 0,  &BX_CPU_C::BxError },
  /* DC DE */ { 0,  &BX_CPU_C::BxError },
  /* DC DF */ { 0,  &BX_CPU_C::BxError },
  /* DC E0 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E1 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E2 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E3 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E4 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E5 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E6 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E7 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DC E8 */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC E9 */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC EA */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC EB */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC EC */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC ED */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC EE */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC EF */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DC F0 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F1 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F2 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F3 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F4 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F5 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F6 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F7 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DC F8 */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC F9 */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FA */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FB */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FC */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FD */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FE */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DC FF */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },

  // DD (modrm is outside 00h - BFh) (mod == 11)
  /* DD C0 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C1 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C2 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C3 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C4 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C5 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C6 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C7 */ { 0,  &BX_CPU_C::FFREE_STi },
  /* DD C8 */ { 0,  &BX_CPU_C::BxError   },
  /* DD C9 */ { 0,  &BX_CPU_C::BxError   },
  /* DD CA */ { 0,  &BX_CPU_C::BxError   },
  /* DD CB */ { 0,  &BX_CPU_C::BxError   },
  /* DD CC */ { 0,  &BX_CPU_C::BxError   },
  /* DD CD */ { 0,  &BX_CPU_C::BxError   },
  /* DD CE */ { 0,  &BX_CPU_C::BxError   },
  /* DD CF */ { 0,  &BX_CPU_C::BxError   },
  /* DD D0 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D1 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D2 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D3 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D4 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D5 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D6 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D7 */ { 0,  &BX_CPU_C::FST_STi   },
  /* DD D8 */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD D9 */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DA */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DB */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DC */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DD */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DE */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD DF */ { 0,  &BX_CPU_C::FST_STi   },	// FSTP_STi
  /* DD E0 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E1 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E2 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E3 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E4 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E5 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E6 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E7 */ { 0,  &BX_CPU_C::FUCOM_STi },
  /* DD E8 */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD E9 */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD EA */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD EB */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD EC */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD ED */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD EE */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD EF */ { 0,  &BX_CPU_C::FUCOM_STi },	// FUCOMP
  /* DD F0 */ { 0,  &BX_CPU_C::BxError },
  /* DD F1 */ { 0,  &BX_CPU_C::BxError },
  /* DD F2 */ { 0,  &BX_CPU_C::BxError },
  /* DD F3 */ { 0,  &BX_CPU_C::BxError },
  /* DD F4 */ { 0,  &BX_CPU_C::BxError },
  /* DD F5 */ { 0,  &BX_CPU_C::BxError },
  /* DD F6 */ { 0,  &BX_CPU_C::BxError },
  /* DD F7 */ { 0,  &BX_CPU_C::BxError },
  /* DD F8 */ { 0,  &BX_CPU_C::BxError },
  /* DD F9 */ { 0,  &BX_CPU_C::BxError },
  /* DD FA */ { 0,  &BX_CPU_C::BxError },
  /* DD FB */ { 0,  &BX_CPU_C::BxError },
  /* DD FC */ { 0,  &BX_CPU_C::BxError },
  /* DD FD */ { 0,  &BX_CPU_C::BxError },
  /* DD FE */ { 0,  &BX_CPU_C::BxError },
  /* DD FF */ { 0,  &BX_CPU_C::BxError },

  // DE (modrm is outside 00h - BFh) (mod == 11)
  // all instructions pop FPU stack
  /* DE C0 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C1 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C2 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C3 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C4 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C5 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C6 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C7 */ { 0,  &BX_CPU_C::FADD_STi_ST0  },
  /* DE C8 */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE C9 */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CA */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CB */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CC */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CD */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CE */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE CF */ { 0,  &BX_CPU_C::FMUL_STi_ST0  },
  /* DE D0 */ { 0,  &BX_CPU_C::BxError },
  /* DE D1 */ { 0,  &BX_CPU_C::BxError },
  /* DE D2 */ { 0,  &BX_CPU_C::BxError },
  /* DE D3 */ { 0,  &BX_CPU_C::BxError },
  /* DE D4 */ { 0,  &BX_CPU_C::BxError },
  /* DE D5 */ { 0,  &BX_CPU_C::BxError },
  /* DE D6 */ { 0,  &BX_CPU_C::BxError },
  /* DE D7 */ { 0,  &BX_CPU_C::BxError },
  /* DE D8 */ { 0,  &BX_CPU_C::BxError },
  /* DE D9 */ { 0,  &BX_CPU_C::FCOMPP  },
  /* DE DA */ { 0,  &BX_CPU_C::BxError },
  /* DE DB */ { 0,  &BX_CPU_C::BxError },
  /* DE DC */ { 0,  &BX_CPU_C::BxError },
  /* DE DD */ { 0,  &BX_CPU_C::BxError },
  /* DE DE */ { 0,  &BX_CPU_C::BxError },
  /* DE DF */ { 0,  &BX_CPU_C::BxError },
  /* DE E0 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E1 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E2 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E3 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E4 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E5 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E6 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E7 */ { 0,  &BX_CPU_C::FSUBR_STi_ST0 },
  /* DE E8 */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE E9 */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE EA */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE EB */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE EC */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE ED */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE EE */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE EF */ { 0,  &BX_CPU_C::FSUB_STi_ST0  },
  /* DE F0 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F1 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F2 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F3 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F4 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F5 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F6 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F7 */ { 0,  &BX_CPU_C::FDIVR_STi_ST0 },
  /* DE F8 */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE F9 */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FA */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FB */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FC */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FD */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FE */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },
  /* DE FF */ { 0,  &BX_CPU_C::FDIV_STi_ST0  },

  // DF (modrm is outside 00h - BFh) (mod == 11)
  /* DF C0 */ { 0,  &BX_CPU_C::FFREEP_STi },   // 287+ compatibility opcode
  /* DF C1 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C2 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C3 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C4 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C5 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C6 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C7 */ { 0,  &BX_CPU_C::FFREEP_STi },
  /* DF C8 */ { 0,  &BX_CPU_C::BxError },
  /* DF C9 */ { 0,  &BX_CPU_C::BxError },
  /* DF CA */ { 0,  &BX_CPU_C::BxError },
  /* DF CB */ { 0,  &BX_CPU_C::BxError },
  /* DF CC */ { 0,  &BX_CPU_C::BxError },
  /* DF CD */ { 0,  &BX_CPU_C::BxError },
  /* DF CE */ { 0,  &BX_CPU_C::BxError },
  /* DF CF */ { 0,  &BX_CPU_C::BxError },
  /* DF D0 */ { 0,  &BX_CPU_C::BxError },
  /* DF D1 */ { 0,  &BX_CPU_C::BxError },
  /* DF D2 */ { 0,  &BX_CPU_C::BxError },
  /* DF D3 */ { 0,  &BX_CPU_C::BxError },
  /* DF D4 */ { 0,  &BX_CPU_C::BxError },
  /* DF D5 */ { 0,  &BX_CPU_C::BxError },
  /* DF D6 */ { 0,  &BX_CPU_C::BxError },
  /* DF D7 */ { 0,  &BX_CPU_C::BxError },
  /* DF D8 */ { 0,  &BX_CPU_C::BxError },
  /* DF D9 */ { 0,  &BX_CPU_C::BxError },
  /* DF DA */ { 0,  &BX_CPU_C::BxError },
  /* DF DB */ { 0,  &BX_CPU_C::BxError },
  /* DF DC */ { 0,  &BX_CPU_C::BxError },
  /* DF DD */ { 0,  &BX_CPU_C::BxError },
  /* DF DE */ { 0,  &BX_CPU_C::BxError },
  /* DF DF */ { 0,  &BX_CPU_C::BxError },
  /* DF E0 */ { 0,  &BX_CPU_C::FNSTSW_AX },
  /* DF E1 */ { 0,  &BX_CPU_C::BxError },
  /* DF E2 */ { 0,  &BX_CPU_C::BxError },
  /* DF E3 */ { 0,  &BX_CPU_C::BxError },
  /* DF E4 */ { 0,  &BX_CPU_C::BxError },
  /* DF E5 */ { 0,  &BX_CPU_C::BxError },
  /* DF E6 */ { 0,  &BX_CPU_C::BxError },
  /* DF E7 */ { 0,  &BX_CPU_C::BxError },
  /* DF E8 */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF E9 */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EA */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EB */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EC */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF ED */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EE */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF EF */ { 0,  &BX_CPU_C::FUCOMI_ST0_STj },	// FUCOMIP
  /* DF F0 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F1 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F2 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F3 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F4 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F5 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F6 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F7 */ { 0,  &BX_CPU_C::FCOMI_ST0_STj  },	// FCOMIP
  /* DF F8 */ { 0,  &BX_CPU_C::BxError },
  /* DF F9 */ { 0,  &BX_CPU_C::BxError },
  /* DF FA */ { 0,  &BX_CPU_C::BxError },
  /* DF FB */ { 0,  &BX_CPU_C::BxError },
  /* DF FC */ { 0,  &BX_CPU_C::BxError },
  /* DF FD */ { 0,  &BX_CPU_C::BxError },
  /* DF FE */ { 0,  &BX_CPU_C::BxError },
  /* DF FF */ { 0,  &BX_CPU_C::BxError },
};

#endif

/* ************************************************************************ */
/* 3DNow! Opcodes */

#if BX_SUPPORT_3DNOW

static const BxOpcodeInfo_t Bx3DNowOpcodeInfo[256] = {
  // 256 entries for 3DNow opcodes, by suffix
  /* 00 */ { 0, &BX_CPU_C::BxError },
  /* 01 */ { 0, &BX_CPU_C::BxError },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::PI2FW_PqQq },
  /* 0D */ { 0, &BX_CPU_C::PI2FD_PqQq },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError },
  /* 10 */ { 0, &BX_CPU_C::BxError },
  /* 11 */ { 0, &BX_CPU_C::BxError },
  /* 12 */ { 0, &BX_CPU_C::BxError },
  /* 13 */ { 0, &BX_CPU_C::BxError },
  /* 14 */ { 0, &BX_CPU_C::BxError },
  /* 15 */ { 0, &BX_CPU_C::BxError },
  /* 16 */ { 0, &BX_CPU_C::BxError },
  /* 17 */ { 0, &BX_CPU_C::BxError },
  /* 18 */ { 0, &BX_CPU_C::BxError },
  /* 19 */ { 0, &BX_CPU_C::BxError },
  /* 1A */ { 0, &BX_CPU_C::BxError },
  /* 1B */ { 0, &BX_CPU_C::BxError },
  /* 1C */ { 0, &BX_CPU_C::PF2IW_PqQq },
  /* 1D */ { 0, &BX_CPU_C::PF2ID_PqQq },
  /* 1E */ { 0, &BX_CPU_C::BxError },
  /* 1F */ { 0, &BX_CPU_C::BxError },
  /* 20 */ { 0, &BX_CPU_C::BxError },
  /* 21 */ { 0, &BX_CPU_C::BxError },
  /* 22 */ { 0, &BX_CPU_C::BxError },
  /* 23 */ { 0, &BX_CPU_C::BxError },
  /* 24 */ { 0, &BX_CPU_C::BxError },
  /* 25 */ { 0, &BX_CPU_C::BxError },
  /* 26 */ { 0, &BX_CPU_C::BxError },
  /* 27 */ { 0, &BX_CPU_C::BxError },
  /* 28 */ { 0, &BX_CPU_C::BxError },
  /* 29 */ { 0, &BX_CPU_C::BxError },
  /* 2A */ { 0, &BX_CPU_C::BxError },
  /* 2B */ { 0, &BX_CPU_C::BxError },
  /* 2C */ { 0, &BX_CPU_C::BxError },
  /* 2D */ { 0, &BX_CPU_C::BxError },
  /* 2E */ { 0, &BX_CPU_C::BxError },
  /* 2F */ { 0, &BX_CPU_C::BxError },
  /* 30 */ { 0, &BX_CPU_C::BxError },
  /* 31 */ { 0, &BX_CPU_C::BxError },
  /* 32 */ { 0, &BX_CPU_C::BxError },
  /* 33 */ { 0, &BX_CPU_C::BxError },
  /* 34 */ { 0, &BX_CPU_C::BxError },
  /* 35 */ { 0, &BX_CPU_C::BxError },
  /* 36 */ { 0, &BX_CPU_C::BxError },
  /* 37 */ { 0, &BX_CPU_C::BxError },
  /* 38 */ { 0, &BX_CPU_C::BxError },
  /* 39 */ { 0, &BX_CPU_C::BxError },
  /* 3A */ { 0, &BX_CPU_C::BxError },
  /* 3B */ { 0, &BX_CPU_C::BxError },
  /* 3C */ { 0, &BX_CPU_C::BxError },
  /* 3D */ { 0, &BX_CPU_C::BxError },
  /* 3E */ { 0, &BX_CPU_C::BxError },
  /* 3F */ { 0, &BX_CPU_C::BxError },
  /* 40 */ { 0, &BX_CPU_C::BxError },
  /* 41 */ { 0, &BX_CPU_C::BxError },
  /* 42 */ { 0, &BX_CPU_C::BxError },
  /* 43 */ { 0, &BX_CPU_C::BxError },
  /* 44 */ { 0, &BX_CPU_C::BxError },
  /* 45 */ { 0, &BX_CPU_C::BxError },
  /* 46 */ { 0, &BX_CPU_C::BxError },
  /* 47 */ { 0, &BX_CPU_C::BxError },
  /* 48 */ { 0, &BX_CPU_C::BxError },
  /* 49 */ { 0, &BX_CPU_C::BxError },
  /* 4A */ { 0, &BX_CPU_C::BxError },
  /* 4B */ { 0, &BX_CPU_C::BxError },
  /* 4C */ { 0, &BX_CPU_C::BxError },
  /* 4D */ { 0, &BX_CPU_C::BxError },
  /* 4E */ { 0, &BX_CPU_C::BxError },
  /* 4F */ { 0, &BX_CPU_C::BxError },
  /* 50 */ { 0, &BX_CPU_C::BxError },
  /* 51 */ { 0, &BX_CPU_C::BxError },
  /* 52 */ { 0, &BX_CPU_C::BxError },
  /* 53 */ { 0, &BX_CPU_C::BxError },
  /* 54 */ { 0, &BX_CPU_C::BxError },
  /* 55 */ { 0, &BX_CPU_C::BxError },
  /* 56 */ { 0, &BX_CPU_C::BxError },
  /* 57 */ { 0, &BX_CPU_C::BxError },
  /* 58 */ { 0, &BX_CPU_C::BxError },
  /* 59 */ { 0, &BX_CPU_C::BxError },
  /* 5A */ { 0, &BX_CPU_C::BxError },
  /* 5B */ { 0, &BX_CPU_C::BxError },
  /* 5C */ { 0, &BX_CPU_C::BxError },
  /* 5D */ { 0, &BX_CPU_C::BxError },
  /* 5E */ { 0, &BX_CPU_C::BxError },
  /* 5F */ { 0, &BX_CPU_C::BxError },
  /* 60 */ { 0, &BX_CPU_C::BxError },
  /* 61 */ { 0, &BX_CPU_C::BxError },
  /* 62 */ { 0, &BX_CPU_C::BxError },
  /* 63 */ { 0, &BX_CPU_C::BxError },
  /* 64 */ { 0, &BX_CPU_C::BxError },
  /* 65 */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* 67 */ { 0, &BX_CPU_C::BxError },
  /* 68 */ { 0, &BX_CPU_C::BxError },
  /* 69 */ { 0, &BX_CPU_C::BxError },
  /* 6A */ { 0, &BX_CPU_C::BxError },
  /* 6B */ { 0, &BX_CPU_C::BxError },
  /* 6C */ { 0, &BX_CPU_C::BxError },
  /* 6D */ { 0, &BX_CPU_C::BxError },
  /* 6E */ { 0, &BX_CPU_C::BxError },
  /* 6F */ { 0, &BX_CPU_C::BxError },
  /* 70 */ { 0, &BX_CPU_C::BxError },
  /* 71 */ { 0, &BX_CPU_C::BxError },
  /* 72 */ { 0, &BX_CPU_C::BxError },
  /* 73 */ { 0, &BX_CPU_C::BxError },
  /* 74 */ { 0, &BX_CPU_C::BxError },
  /* 75 */ { 0, &BX_CPU_C::BxError },
  /* 76 */ { 0, &BX_CPU_C::BxError },
  /* 77 */ { 0, &BX_CPU_C::BxError },
  /* 78 */ { 0, &BX_CPU_C::BxError },
  /* 79 */ { 0, &BX_CPU_C::BxError },
  /* 7A */ { 0, &BX_CPU_C::BxError },
  /* 7B */ { 0, &BX_CPU_C::BxError },
  /* 7C */ { 0, &BX_CPU_C::BxError },
  /* 7D */ { 0, &BX_CPU_C::BxError },
  /* 7E */ { 0, &BX_CPU_C::BxError },
  /* 7F */ { 0, &BX_CPU_C::BxError },
  /* 80 */ { 0, &BX_CPU_C::BxError },
  /* 81 */ { 0, &BX_CPU_C::BxError },
  /* 82 */ { 0, &BX_CPU_C::BxError },
  /* 83 */ { 0, &BX_CPU_C::BxError },
  /* 84 */ { 0, &BX_CPU_C::BxError },
  /* 85 */ { 0, &BX_CPU_C::BxError },
  /* 86 */ { 0, &BX_CPU_C::BxError },
  /* 87 */ { 0, &BX_CPU_C::BxError },
  /* 88 */ { 0, &BX_CPU_C::BxError },
  /* 89 */ { 0, &BX_CPU_C::BxError },
  /* 8A */ { 0, &BX_CPU_C::PFNACC_PqQq },
  /* 8B */ { 0, &BX_CPU_C::BxError },
  /* 8C */ { 0, &BX_CPU_C::BxError },
  /* 8D */ { 0, &BX_CPU_C::BxError },
  /* 8E */ { 0, &BX_CPU_C::PFPNACC_PqQq },
  /* 8F */ { 0, &BX_CPU_C::BxError },
  /* 90 */ { 0, &BX_CPU_C::PFCMPGE_PqQq },
  /* 91 */ { 0, &BX_CPU_C::BxError },
  /* 92 */ { 0, &BX_CPU_C::BxError },
  /* 93 */ { 0, &BX_CPU_C::BxError },
  /* 94 */ { 0, &BX_CPU_C::PFMIN_PqQq },
  /* 95 */ { 0, &BX_CPU_C::BxError },
  /* 96 */ { 0, &BX_CPU_C::PFRCP_PqQq },
  /* 97 */ { 0, &BX_CPU_C::PFRSQRT_PqQq },
  /* 98 */ { 0, &BX_CPU_C::BxError },
  /* 99 */ { 0, &BX_CPU_C::BxError },
  /* 9A */ { 0, &BX_CPU_C::PFSUB_PqQq },
  /* 9B */ { 0, &BX_CPU_C::BxError },
  /* 9C */ { 0, &BX_CPU_C::BxError },
  /* 9D */ { 0, &BX_CPU_C::BxError },
  /* 9E */ { 0, &BX_CPU_C::PFADD_PqQq },
  /* 9F */ { 0, &BX_CPU_C::BxError },
  /* A0 */ { 0, &BX_CPU_C::PFCMPGT_PqQq },
  /* A1 */ { 0, &BX_CPU_C::BxError },
  /* A2 */ { 0, &BX_CPU_C::BxError },
  /* A3 */ { 0, &BX_CPU_C::BxError },
  /* A4 */ { 0, &BX_CPU_C::PFMAX_PqQq },
  /* A5 */ { 0, &BX_CPU_C::BxError },
  /* A6 */ { 0, &BX_CPU_C::PFRCPIT1_PqQq },
  /* A7 */ { 0, &BX_CPU_C::PFRSQIT1_PqQq },
  /* A8 */ { 0, &BX_CPU_C::BxError },
  /* A9 */ { 0, &BX_CPU_C::BxError },
  /* AA */ { 0, &BX_CPU_C::PFSUBR_PqQq },
  /* AB */ { 0, &BX_CPU_C::BxError },
  /* AC */ { 0, &BX_CPU_C::BxError },
  /* AD */ { 0, &BX_CPU_C::BxError },
  /* AE */ { 0, &BX_CPU_C::PFACC_PqQq },
  /* AF */ { 0, &BX_CPU_C::BxError },
  /* B0 */ { 0, &BX_CPU_C::PFCMPEQ_PqQq },
  /* B1 */ { 0, &BX_CPU_C::BxError },
  /* B2 */ { 0, &BX_CPU_C::BxError },
  /* B3 */ { 0, &BX_CPU_C::BxError },
  /* B4 */ { 0, &BX_CPU_C::PFMUL_PqQq },
  /* B5 */ { 0, &BX_CPU_C::BxError },
  /* B6 */ { 0, &BX_CPU_C::PFRCPIT2_PqQq },
  /* B7 */ { 0, &BX_CPU_C::PMULHRW_PqQq },
  /* B8 */ { 0, &BX_CPU_C::BxError },  	
  /* B9 */ { 0, &BX_CPU_C::BxError },
  /* BA */ { 0, &BX_CPU_C::BxError },
  /* BB */ { 0, &BX_CPU_C::PSWAPD_PqQq },
  /* BC */ { 0, &BX_CPU_C::BxError },
  /* BD */ { 0, &BX_CPU_C::BxError },
  /* BE */ { 0, &BX_CPU_C::BxError },
  /* BF */ { 0, &BX_CPU_C::PAVGB_PqQq },
  /* C0 */ { 0, &BX_CPU_C::BxError },
  /* C1 */ { 0, &BX_CPU_C::BxError },
  /* C2 */ { 0, &BX_CPU_C::BxError },
  /* C3 */ { 0, &BX_CPU_C::BxError },
  /* C4 */ { 0, &BX_CPU_C::BxError },
  /* C5 */ { 0, &BX_CPU_C::BxError },
  /* C6 */ { 0, &BX_CPU_C::BxError },
  /* C7 */ { 0, &BX_CPU_C::BxError },
  /* C8 */ { 0, &BX_CPU_C::BxError },
  /* C9 */ { 0, &BX_CPU_C::BxError },
  /* CA */ { 0, &BX_CPU_C::BxError },
  /* CB */ { 0, &BX_CPU_C::BxError },
  /* CC */ { 0, &BX_CPU_C::BxError },
  /* CD */ { 0, &BX_CPU_C::BxError },
  /* CE */ { 0, &BX_CPU_C::BxError },
  /* CF */ { 0, &BX_CPU_C::BxError },
  /* D0 */ { 0, &BX_CPU_C::BxError },
  /* D1 */ { 0, &BX_CPU_C::BxError },
  /* D2 */ { 0, &BX_CPU_C::BxError },
  /* D3 */ { 0, &BX_CPU_C::BxError },
  /* D4 */ { 0, &BX_CPU_C::BxError },
  /* D5 */ { 0, &BX_CPU_C::BxError },
  /* D6 */ { 0, &BX_CPU_C::BxError },
  /* D7 */ { 0, &BX_CPU_C::BxError },
  /* D8 */ { 0, &BX_CPU_C::BxError },
  /* D9 */ { 0, &BX_CPU_C::BxError },
  /* DA */ { 0, &BX_CPU_C::BxError },
  /* DB */ { 0, &BX_CPU_C::BxError },
  /* DC */ { 0, &BX_CPU_C::BxError },
  /* DD */ { 0, &BX_CPU_C::BxError },
  /* DE */ { 0, &BX_CPU_C::BxError },
  /* DF */ { 0, &BX_CPU_C::BxError },
  /* E0 */ { 0, &BX_CPU_C::BxError },
  /* E1 */ { 0, &BX_CPU_C::BxError },
  /* E2 */ { 0, &BX_CPU_C::BxError },
  /* E3 */ { 0, &BX_CPU_C::BxError },
  /* E4 */ { 0, &BX_CPU_C::BxError },
  /* E5 */ { 0, &BX_CPU_C::BxError },
  /* E6 */ { 0, &BX_CPU_C::BxError },
  /* E7 */ { 0, &BX_CPU_C::BxError },
  /* E8 */ { 0, &BX_CPU_C::BxError },
  /* E9 */ { 0, &BX_CPU_C::BxError },
  /* EA */ { 0, &BX_CPU_C::BxError },
  /* EB */ { 0, &BX_CPU_C::BxError },
  /* EC */ { 0, &BX_CPU_C::BxError },
  /* ED */ { 0, &BX_CPU_C::BxError },
  /* EE */ { 0, &BX_CPU_C::BxError },
  /* EF */ { 0, &BX_CPU_C::BxError },
  /* F0 */ { 0, &BX_CPU_C::BxError },
  /* F1 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError },
  /* F4 */ { 0, &BX_CPU_C::BxError },
  /* F5 */ { 0, &BX_CPU_C::BxError },
  /* F6 */ { 0, &BX_CPU_C::BxError },
  /* F7 */ { 0, &BX_CPU_C::BxError },
  /* F8 */ { 0, &BX_CPU_C::BxError },
  /* F9 */ { 0, &BX_CPU_C::BxError },
  /* FA */ { 0, &BX_CPU_C::BxError },
  /* FB */ { 0, &BX_CPU_C::BxError },
  /* FC */ { 0, &BX_CPU_C::BxError },
  /* FD */ { 0, &BX_CPU_C::BxError },
  /* FE */ { 0, &BX_CPU_C::BxError },
  /* FF */ { 0, &BX_CPU_C::BxError }
};

#endif


/* ********** */
/* SSE Groups */
/* ********** */

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f10[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVUPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::MOVUPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::MOVSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::MOVSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f11[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVUPS_WpsVps },
  /* 66 */ { 0, &BX_CPU_C::MOVUPD_WpdVpd },
  /* F2 */ { 0, &BX_CPU_C::MOVSD_WsdVsd },
  /* F3 */ { 0, &BX_CPU_C::MOVSS_WssVss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f12[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVLPS_VpsMq },
  /* 66 */ { 0, &BX_CPU_C::MOVLPD_VsdMq },
  /* F2 */ { 0, &BX_CPU_C::MOVDDUP_VpdWq },	// SSE3
  /* F3 */ { 0, &BX_CPU_C::MOVSLDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f13M[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVLPS_MqVps },
  /* 66 */ { 0, &BX_CPU_C::MOVLPD_MqVsd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f14[4] = {
  /* -- */ { 0, &BX_CPU_C::UNPCKLPS_VpsWdq },
  /* 66 */ { 0, &BX_CPU_C::UNPCKLPD_VpdWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f15[4] = {
  /* -- */ { 0, &BX_CPU_C::UNPCKHPS_VpsWdq },
  /* 66 */ { 0, &BX_CPU_C::UNPCKHPD_VpdWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f16[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVHPS_VpsMq },
  /* 66 */ { 0, &BX_CPU_C::MOVHPD_VsdMq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::MOVSHDUP_VpsWps }	// SSE3
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f17M[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVHPS_MqVps },
  /* 66 */ { 0, &BX_CPU_C::MOVHPD_MqVsd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f28[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVAPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::MOVAPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f29[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVAPS_WpsVps },
  /* 66 */ { 0, &BX_CPU_C::MOVAPD_WpdVpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2a[4] = {
  /* -- */ { 0, &BX_CPU_C::CVTPI2PS_VpsQq },
  /* 66 */ { 0, &BX_CPU_C::CVTPI2PD_VpdQq },
  /* F2 */ { 0, &BX_CPU_C::CVTSI2SD_VsdEd },
  /* F3 */ { 0, &BX_CPU_C::CVTSI2SS_VssEd }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2bM[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVNTPS_MpsVps },
  /* 66 */ { 0, &BX_CPU_C::MOVNTPD_MpdVpd },
  /* F2 */ { 0, &BX_CPU_C::MOVNTSD_MsdVsd },    // SSE4A
  /* F3 */ { 0, &BX_CPU_C::MOVNTSS_MssVss }     // SSE4A
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2c[4] = {
  /* -- */ { 0, &BX_CPU_C::CVTTPS2PI_PqWps },
  /* 66 */ { 0, &BX_CPU_C::CVTTPD2PI_PqWpd },
  /* F2 */ { 0, &BX_CPU_C::CVTTSD2SI_GdWsd },
  /* F3 */ { 0, &BX_CPU_C::CVTTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2d[4] = {
  /* -- */ { 0, &BX_CPU_C::CVTPS2PI_PqWps },
  /* 66 */ { 0, &BX_CPU_C::CVTPD2PI_PqWpd },
  /* F2 */ { 0, &BX_CPU_C::CVTSD2SI_GdWsd },
  /* F3 */ { 0, &BX_CPU_C::CVTSS2SI_GdWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2e[4] = {
  /* -- */ { 0, &BX_CPU_C::UCOMISS_VssWss },
  /* 66 */ { 0, &BX_CPU_C::UCOMISD_VsdWsd },            	
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f2f[4] = {
  /* -- */ { 0, &BX_CPU_C::COMISS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::COMISD_VpdWpd },   
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f50[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVMSKPS_GdVRps },
  /* 66 */ { 0, &BX_CPU_C::MOVMSKPD_GdVRpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f51[4] = {
  /* -- */ { 0, &BX_CPU_C::SQRTPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::SQRTPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::SQRTSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::SQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f52[4] = {
  /* -- */ { 0, &BX_CPU_C::RSQRTPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::RSQRTSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f53[4] = {
  /* -- */ { 0, &BX_CPU_C::RCPPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::RCPSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f54[4] = {
  /* -- */ { 0, &BX_CPU_C::ANDPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::ANDPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f55[4] = {
  /* -- */ { 0, &BX_CPU_C::ANDNPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::ANDNPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f56[4] = {
  /* -- */ { 0, &BX_CPU_C::ORPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::ORPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f57[4] = {
  /* -- */ { 0, &BX_CPU_C::XORPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::XORPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f58[4] = {
  /* -- */ { 0, &BX_CPU_C::ADDPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::ADDPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::ADDSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::ADDSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f59[4] = {
  /* -- */ { 0, &BX_CPU_C::MULPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::MULPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::MULSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::MULSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5a[4] = {
  /* -- */ { 0, &BX_CPU_C::CVTPS2PD_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::CVTPD2PS_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::CVTSD2SS_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::CVTSS2SD_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5b[4] = {
  /* -- */ { 0, &BX_CPU_C::CVTDQ2PS_VpsWdq },
  /* 66 */ { 0, &BX_CPU_C::CVTPS2DQ_VdqWps },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::CVTTPS2DQ_VdqWps }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5c[4] = {
  /* -- */ { 0, &BX_CPU_C::SUBPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::SUBPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::SUBSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::SUBSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5d[4] = {
  /* -- */ { 0, &BX_CPU_C::MINPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::MINPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::MINSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::MINSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5e[4] = {
  /* -- */ { 0, &BX_CPU_C::DIVPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::DIVPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::DIVSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::DIVSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f5f[4] = {
  /* -- */ { 0, &BX_CPU_C::MAXPS_VpsWps },
  /* 66 */ { 0, &BX_CPU_C::MAXPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::MAXSD_VsdWsd },
  /* F3 */ { 0, &BX_CPU_C::MAXSS_VssWss }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f60[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKLBW_PqQd  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKLBW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f61[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKLWD_PqQd  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKLWD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f62[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKLDQ_PqQd  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKLDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f63[4] = {
  /* -- */ { 0, &BX_CPU_C::PACKSSWB_PqQq  },
  /* 66 */ { 0, &BX_CPU_C::PACKSSWB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f64[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPGTB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPGTB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f65[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPGTW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPGTW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f66[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPGTD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPGTD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f67[4] = {
  /* -- */ { 0, &BX_CPU_C::PACKUSWB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PACKUSWB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f68[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKHBW_PqQq  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKHBW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f69[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKHWD_PqQq  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKHWD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6a[4] = {
  /* -- */ { 0, &BX_CPU_C::PUNPCKHDQ_PqQq  },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKHDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6b[4] = {
  /* -- */ { 0, &BX_CPU_C::PACKSSDW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PACKSSDW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6c[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKLQDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6d[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PUNPCKHQDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6e[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVD_PqEd  },
  /* 66 */ { 0, &BX_CPU_C::MOVD_VdqEd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6eQ[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVQ_PqEq  },
  /* 66 */ { 0, &BX_CPU_C::MOVQ_VdqEq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f6f[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVQ_PqQq     },
  /* 66 */ { 0, &BX_CPU_C::MOVDQA_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError       },
  /* F3 */ { 0, &BX_CPU_C::MOVDQU_VdqWdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f70[4] = {
  /* -- */ { 0, &BX_CPU_C::PSHUFW_PqQqIb    },
  /* 66 */ { 0, &BX_CPU_C::PSHUFD_VdqWdqIb  },
  /* F2 */ { 0, &BX_CPU_C::PSHUFHW_VdqWdqIb },
  /* F3 */ { 0, &BX_CPU_C::PSHUFLW_VdqWdqIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f74[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPEQB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPEQB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f75[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPEQW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPEQW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f76[4] = {
  /* -- */ { 0, &BX_CPU_C::PCMPEQD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PCMPEQD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t opcodesGroupRmEXTRQ[8] = {
  /* 0 */ { BxImmediate_IbIb, &BX_CPU_C::EXTRQ_VdqIbIb },       // SSE4A
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f78[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { BxRMGroup, NULL, opcodesGroupRmEXTRQ },            // SSE4A
  /* F2 */ { BxImmediate_IbIb, &BX_CPU_C::INSERTQ_VdqUdqIbIb }, // SSE4A
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f79[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::EXTRQ_VdqUdq },      // SSE4A
  /* F2 */ { 0, &BX_CPU_C::INSERTQ_VdqUdq },    // SSE4A
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7c[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::HADDPD_VpdWpd },     // SSE3
  /* F2 */ { 0, &BX_CPU_C::HADDPS_VpsWps },     // SSE3
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7d[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::HSUBPD_VpdWpd },     // SSE3
  /* F2 */ { 0, &BX_CPU_C::HSUBPS_VpsWps },     // SSE3
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7e[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVD_EdPd },
  /* 66 */ { 0, &BX_CPU_C::MOVD_EdVd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::MOVQ_VqWq }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7eQ[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVQ_EqPq },
  /* 66 */ { 0, &BX_CPU_C::MOVQ_EqVq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::MOVQ_VqWq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f7f[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVQ_QqPq     },
  /* 66 */ { 0, &BX_CPU_C::MOVDQA_WdqVdq },
  /* F2 */ { 0, &BX_CPU_C::BxError       },
  /* F3 */ { 0, &BX_CPU_C::MOVDQU_WdqVdq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8w[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError     },
  /* 66 */ { 0, &BX_CPU_C::BxError     },
  /* F2 */ { 0, &BX_CPU_C::BxError     },
  /* F3 */ { 0, &BX_CPU_C::POPCNT_GwEw }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8d[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError     },
  /* 66 */ { 0, &BX_CPU_C::BxError     },
  /* F2 */ { 0, &BX_CPU_C::BxError     },
  /* F3 */ { 0, &BX_CPU_C::POPCNT_GdEd }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fb8q[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError     },
  /* 66 */ { 0, &BX_CPU_C::BxError     },
  /* F2 */ { 0, &BX_CPU_C::BxError     },
  /* F3 */ { 0, &BX_CPU_C::POPCNT_GqEq }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc2[4] = {
  /* -- */ { 0, &BX_CPU_C::CMPPS_VpsWpsIb },
  /* 66 */ { 0, &BX_CPU_C::CMPPD_VpdWpdIb },
  /* F2 */ { 0, &BX_CPU_C::CMPSD_VsdWsdIb },
  /* F3 */ { 0, &BX_CPU_C::CMPSS_VssWssIb }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc3M[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVNTI_MdGd },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc3qM[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVNTI_MqGq },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};
#endif

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc4[4] = {
  /* -- */ { BxImmediate_Ib, &BX_CPU_C::PINSRW_PqEwIb  },
  /* 66 */ { BxImmediate_Ib, &BX_CPU_C::PINSRW_VdqEwIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc5[4] = {
  /* -- */ { BxImmediate_Ib, &BX_CPU_C::PEXTRW_GdPqIb   },
  /* 66 */ { BxImmediate_Ib, &BX_CPU_C::PEXTRW_GdUdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fc6[4] = {
  /* -- */ { BxImmediate_Ib, &BX_CPU_C::SHUFPS_VpsWpsIb },
  /* 66 */ { BxImmediate_Ib, &BX_CPU_C::SHUFPD_VpdWpdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd0[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::ADDSUBPD_VpdWpd },	// SSE3
  /* F2 */ { 0, &BX_CPU_C::ADDSUBPS_VpsWps },	// SSE3
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd1[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSRLW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd2[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSRLD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd3[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLQ_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSRLQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd4[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDQ_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd5[4] = {
  /* -- */ { 0, &BX_CPU_C::PMULLW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMULLW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd6[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::MOVQ_WqVq     },
  /* F2 */ { 0, &BX_CPU_C::MOVDQ2Q_PqVRq },
  /* F3 */ { 0, &BX_CPU_C::MOVQ2DQ_VdqQq }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd7[4] = {
  /* -- */ { 0, &BX_CPU_C::PMOVMSKB_GdPRq  },
  /* 66 */ { 0, &BX_CPU_C::PMOVMSKB_GdUdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd8[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBUSB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBUSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fd9[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBUSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBUSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fda[4] = {
  /* -- */ { 0, &BX_CPU_C::PMINUB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMINUB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdb[4] = {
  /* -- */ { 0, &BX_CPU_C::PAND_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PAND_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdc[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDUSB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDUSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdd[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDUSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDUSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fde[4] = {
  /* -- */ { 0, &BX_CPU_C::PMAXUB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMAXUB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fdf[4] = {
  /* -- */ { 0, &BX_CPU_C::PANDN_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PANDN_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe0[4] = {
  /* -- */ { 0, &BX_CPU_C::PAVGB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PAVGB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe1[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRAW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSRAW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe2[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRAD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSRAD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe3[4] = {
  /* -- */ { 0, &BX_CPU_C::PAVGW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PAVGW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe4[4] = {
  /* -- */ { 0, &BX_CPU_C::PMULHUW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMULHUW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe5[4] = {
  /* -- */ { 0, &BX_CPU_C::PMULHW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMULHW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe6[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::CVTTPD2DQ_VqWpd },
  /* F2 */ { 0, &BX_CPU_C::CVTPD2DQ_VqWpd  },
  /* F3 */ { 0, &BX_CPU_C::CVTDQ2PD_VpdWq  }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe7M[4] = {
  /* -- */ { 0, &BX_CPU_C::MOVNTQ_MqPq    },
  /* 66 */ { 0, &BX_CPU_C::MOVNTDQ_MdqVdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe8[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBSB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fe9[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fea[4] = {
  /* -- */ { 0, &BX_CPU_C::PMINSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMINSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0feb[4] = {
  /* -- */ { 0, &BX_CPU_C::POR_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::POR_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fec[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDSB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fed[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fee[4] = {
  /* -- */ { 0, &BX_CPU_C::PMAXSW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMAXSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0fef[4] = {
  /* -- */ { 0, &BX_CPU_C::PXOR_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PXOR_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff0M[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::LDDQU_VdqMdq },	// SSE3
  /* F3 */ { 0, &BX_CPU_C::BxError }
};                        

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff1[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSLLW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff2[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSLLD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff3[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLQ_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSLLQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff4[4] = {
  /* -- */ { 0, &BX_CPU_C::PMULUDQ_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMULUDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff5[4] = {
  /* -- */ { 0, &BX_CPU_C::PMADDWD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PMADDWD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff6[4] = {
  /* -- */ { 0, &BX_CPU_C::PSADBW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSADBW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff7[4] = {
  /* -- */ { 0, &BX_CPU_C::MASKMOVQ_PqPRq     },
  /* 66 */ { 0, &BX_CPU_C::MASKMOVDQU_VdqUdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff8[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ff9[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffa[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffb[4] = {
  /* -- */ { 0, &BX_CPU_C::PSUBQ_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PSUBQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffc[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDB_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffd[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDW_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0ffe[4] = {
  /* -- */ { 0, &BX_CPU_C::PADDD_PqQq   },
  /* 66 */ { 0, &BX_CPU_C::PADDD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1202[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLW_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSRLW_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1204[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRAW_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSRAW_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1206[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLW_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSLLW_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1302[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLD_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSRLD_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1304[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRAD_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSRAD_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1306[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLD_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSLLD_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1402[4] = {
  /* -- */ { 0, &BX_CPU_C::PSRLQ_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSRLQ_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1403[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PSRLDQ_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1406[4] = {
  /* -- */ { 0, &BX_CPU_C::PSLLQ_PqIb  },
  /* 66 */ { 0, &BX_CPU_C::PSLLQ_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_G1407[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PSLLDQ_UdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3800[4] = {
  /* -- */ { 0, &BX_CPU_C::PSHUFB_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PSHUFB_VdqWdq },   
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3801[4] = {
  /* -- */ { 0, &BX_CPU_C::PHADDW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHADDW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3802[4] = {
  /* -- */ { 0, &BX_CPU_C::PHADDD_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHADDD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3803[4] = {
  /* -- */ { 0, &BX_CPU_C::PHADDSW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHADDSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3804[4] = {
  /* -- */ { 0, &BX_CPU_C::PMADDUBSW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PMADDUBSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3805[4] = {
  /* -- */ { 0, &BX_CPU_C::PHSUBW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHSUBW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3806[4] = {
  /* -- */ { 0, &BX_CPU_C::PHSUBD_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHSUBD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3807[4] = {
  /* -- */ { 0, &BX_CPU_C::PHSUBSW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PHSUBSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3808[4] = {
  /* -- */ { 0, &BX_CPU_C::PSIGNB_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PSIGNB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3809[4] = {
  /* -- */ { 0, &BX_CPU_C::PSIGNW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PSIGNW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3810[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PBLENDVB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3814[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BLENDVPS_VpsWps },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3815[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BLENDVPD_VpdWpd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3817[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PTEST_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3820[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXBW_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3821[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXBD_VdqWd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3822[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXBQ_VdqWw },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3823[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXWD_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3824[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXWQ_VdqWd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3825[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVSXDQ_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3828[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMULDQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3829[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPEQQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f382a[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::MOVNTDQA_VdqMdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f382b[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PACKUSDW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380a[4] = {
  /* -- */ { 0, &BX_CPU_C::PSIGND_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PSIGND_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f380b[4] = {
  /* -- */ { 0, &BX_CPU_C::PMULHRSW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PMULHRSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381c[4] = {
  /* -- */ { 0, &BX_CPU_C::PABSB_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PABSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381d[4] = {
  /* -- */ { 0, &BX_CPU_C::PABSW_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PABSW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f381e[4] = {
  /* -- */ { 0, &BX_CPU_C::PABSD_PqQq },
  /* 66 */ { 0, &BX_CPU_C::PABSD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3830[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXBW_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3831[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXBD_VdqWd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3832[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXBQ_VdqWw },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3833[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXWD_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3834[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXWQ_VdqWd },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3835[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMOVZXDQ_VdqWq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3837[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPGTQ_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3838[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMINSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3839[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMINSD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383a[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMINUW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383b[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMINUD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383c[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMAXSB_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383d[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMAXSD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383e[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMAXUW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f383f[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMAXUD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3840[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PMULLD_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3841[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PHMINPOSUW_VdqWdq },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f0[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::CRC32_GdEb },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f38f1[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BxError },
  /* F2 */ { 0, &BX_CPU_C::CRC32_GdEv },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a08[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::ROUNDPS_VpsWpsIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a09[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::ROUNDPD_VpdWpdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0a[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::ROUNDSS_VssWssIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0b[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::ROUNDSD_VsdWsdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0c[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BLENDPS_VpsWpsIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0d[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::BLENDPD_VpdWpdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0e[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PBLENDW_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a0f[4] = {
  /* -- */ { 0, &BX_CPU_C::PALIGNR_PqQqIb },
  /* 66 */ { 0, &BX_CPU_C::PALIGNR_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a14[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PEXTRB_HbdUdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a15[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PEXTRW_HwdUdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a16[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PEXTRD_HdUdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a17[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::EXTRACTPS_HdUpsIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a20[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PINSRB_VdqEbIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a21[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::INSERTPS_VpsWssIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a22[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PINSRD_VdqEdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a40[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::DPPS_VpsWpsIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a41[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::DPPD_VpdWpdIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a42[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::MPSADBW_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a60[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPESTRM_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a61[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPESTRI_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a62[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPISTRM_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeGroupSSE_0f3a63[4] = {
  /* -- */ { 0, &BX_CPU_C::BxError },
  /* 66 */ { 0, &BX_CPU_C::PCMPISTRI_VdqWdqIb },
  /* F2 */ { 0, &BX_CPU_C::BxError },
  /* F3 */ { 0, &BX_CPU_C::BxError }
};

/* ************************************************************************ */
/* 3-byte opcode table (Table A-4, 0F 38) */

static const BxOpcodeInfo_t BxOpcode3ByteOp0f380x[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3800 },
  /* 01 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3801 },
  /* 02 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3802 },
  /* 03 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3803 },
  /* 04 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3804 },
  /* 05 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3805 },
  /* 06 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3806 },
  /* 07 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3807 },
  /* 08 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3808 },
  /* 09 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3809 },
  /* 0A */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f380a },
  /* 0B */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f380b },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f381x[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3810 },
  /* 01 */ { 0, &BX_CPU_C::BxError },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3814 },
  /* 05 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3815 },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3817 },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f381c },
  /* 0D */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f381d },
  /* 0E */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f381e },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f382x[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3820 },
  /* 01 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3821 },
  /* 02 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3822 },
  /* 03 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3823 },
  /* 04 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3824 },
  /* 05 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3825 },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3828 },
  /* 09 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3829 },
  /* 0A */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f382a },
  /* 0B */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f382b },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f383x[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3830 },
  /* 01 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3831 },
  /* 02 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3832 },
  /* 03 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3833 },
  /* 04 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3834 },
  /* 05 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3835 },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3837 },
  /* 08 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3838 },
  /* 09 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3839 },
  /* 0A */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383a },
  /* 0B */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383b },
  /* 0C */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383c },
  /* 0D */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383d },
  /* 0E */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383e },
  /* 0F */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f383f }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f384x[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3840 },
  /* 01 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3841 },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f38fx[16] = {
  /* 00 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f38f0 },
  /* 01 */ { BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f38f1 },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteTableA4[16] = {
  /* 00 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f380x },
  /* 01 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f381x },
  /* 02 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f382x },
  /* 03 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f383x },
  /* 04 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f384x },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f38fx }
};         

/* ************************************************************************ */
/* 3-byte opcode table (Table A-5, 0F 3A) */

static const BxOpcodeInfo_t BxOpcode3ByteOp0f3a0x[16] = {
  /* 00 */ { 0, &BX_CPU_C::BxError },
  /* 01 */ { 0, &BX_CPU_C::BxError },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a08 },
  /* 09 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a09 },
  /* 0A */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0a },
  /* 0B */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0b },
  /* 0C */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0c },
  /* 0D */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0d },
  /* 0E */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0e },
  /* 0F */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a0f }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f3a1x[16] = {
  /* 00 */ { 0, &BX_CPU_C::BxError },
  /* 01 */ { 0, &BX_CPU_C::BxError },
  /* 02 */ { 0, &BX_CPU_C::BxError },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a14 },
  /* 05 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a15 },
  /* 06 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a16 },
  /* 07 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a17 },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f3a2x[16] = {
  /* 00 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a20 },
  /* 01 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a21 },
  /* 02 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a22 },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f3a4x[16] = {
  /* 00 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a40 },
  /* 01 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a41 },
  /* 02 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a42 },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteOp0f3a6x[16] = {
  /* 00 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a60 },
  /* 01 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a61 },
  /* 02 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a62 },
  /* 03 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_0f3a63 },
  /* 04 */ { 0, &BX_CPU_C::BxError },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { 0, &BX_CPU_C::BxError },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcode3ByteTableA5[16] = {
  /* 00 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f3a0x },
  /* 01 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f3a1x },
  /* 02 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f3a2x },
  /* 03 */ { 0, &BX_CPU_C::BxError },
  /* 04 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f3a4x },
  /* 05 */ { 0, &BX_CPU_C::BxError },
  /* 06 */ { Bx3ByteOpIndex, NULL, BxOpcode3ByteOp0f3a6x },
  /* 07 */ { 0, &BX_CPU_C::BxError },
  /* 08 */ { 0, &BX_CPU_C::BxError },
  /* 09 */ { 0, &BX_CPU_C::BxError },
  /* 0A */ { 0, &BX_CPU_C::BxError },
  /* 0B */ { 0, &BX_CPU_C::BxError },
  /* 0C */ { 0, &BX_CPU_C::BxError },
  /* 0D */ { 0, &BX_CPU_C::BxError },
  /* 0E */ { 0, &BX_CPU_C::BxError },
  /* 0F */ { 0, &BX_CPU_C::BxError }
};

#endif // BX_SUPPORT_SSE >= 4 || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

/* ************************************************************************ */
/* Opcode Groups */

/* ******* */
/* Group 1 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::ADD_EbIbM },
  /* 1 */ { BxLockable, &BX_CPU_C::OR_EbIbM  },
  /* 2 */ { BxLockable, &BX_CPU_C::ADC_EbIbM },
  /* 3 */ { BxLockable, &BX_CPU_C::SBB_EbIbM },
  /* 4 */ { BxLockable, &BX_CPU_C::AND_EbIbM },
  /* 5 */ { BxLockable, &BX_CPU_C::SUB_EbIbM },
  /* 6 */ { BxLockable, &BX_CPU_C::XOR_EbIbM },
  /* 7 */ { 0,          &BX_CPU_C::CMP_EbIbM }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG1EbIbR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ADD_EbIbR },
  /* 1 */ { 0, &BX_CPU_C::OR_EbIbR  },
  /* 2 */ { 0, &BX_CPU_C::ADC_EbIbR },
  /* 3 */ { 0, &BX_CPU_C::SBB_EbIbR },
  /* 4 */ { 0, &BX_CPU_C::AND_EbIbR },
  /* 5 */ { 0, &BX_CPU_C::SUB_EbIbR },
  /* 6 */ { 0, &BX_CPU_C::XOR_EbIbR },
  /* 7 */ { 0, &BX_CPU_C::CMP_EbIbR }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG1EwM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::ADD_EwIwM },
  /* 1 */ { BxLockable, &BX_CPU_C::OR_EwIwM },
  /* 2 */ { BxLockable, &BX_CPU_C::ADC_EwIwM },
  /* 3 */ { BxLockable, &BX_CPU_C::SBB_EwIwM },
  /* 4 */ { BxLockable, &BX_CPU_C::AND_EwIwM },
  /* 5 */ { BxLockable, &BX_CPU_C::SUB_EwIwM },
  /* 6 */ { BxLockable, &BX_CPU_C::XOR_EwIwM },
  /* 7 */ { 0,          &BX_CPU_C::CMP_EwIwM }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG1EwR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ADD_EwIwR },
  /* 1 */ { 0, &BX_CPU_C::OR_EwIwR },
  /* 2 */ { 0, &BX_CPU_C::ADC_EwIwR },
  /* 3 */ { 0, &BX_CPU_C::SBB_EwIwR },
  /* 4 */ { 0, &BX_CPU_C::AND_EwIwR },
  /* 5 */ { 0, &BX_CPU_C::SUB_EwIwR },
  /* 6 */ { 0, &BX_CPU_C::XOR_EwIwR },
  /* 7 */ { 0, &BX_CPU_C::CMP_EwIwR }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG1EdM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::ADD_EdIdM },
  /* 1 */ { BxLockable, &BX_CPU_C::OR_EdIdM },
  /* 2 */ { BxLockable, &BX_CPU_C::ADC_EdIdM },
  /* 3 */ { BxLockable, &BX_CPU_C::SBB_EdIdM },
  /* 4 */ { BxLockable, &BX_CPU_C::AND_EdIdM },
  /* 5 */ { BxLockable, &BX_CPU_C::SUB_EdIdM },
  /* 6 */ { BxLockable, &BX_CPU_C::XOR_EdIdM },
  /* 7 */ { 0,          &BX_CPU_C::CMP_EdIdM }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG1EdR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ADD_EdIdR },
  /* 1 */ { 0, &BX_CPU_C::OR_EdIdR },
  /* 2 */ { 0, &BX_CPU_C::ADC_EdIdR },
  /* 3 */ { 0, &BX_CPU_C::SBB_EdIdR },
  /* 4 */ { 0, &BX_CPU_C::AND_EdIdR },
  /* 5 */ { 0, &BX_CPU_C::SUB_EdIdR },
  /* 6 */ { 0, &BX_CPU_C::XOR_EdIdR },
  /* 7 */ { 0, &BX_CPU_C::CMP_EdIdR }
}; 

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G1EqM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::ADD_EqIdM },
  /* 1 */ { BxLockable, &BX_CPU_C::OR_EqIdM },
  /* 2 */ { BxLockable, &BX_CPU_C::ADC_EqIdM },
  /* 3 */ { BxLockable, &BX_CPU_C::SBB_EqIdM },
  /* 4 */ { BxLockable, &BX_CPU_C::AND_EqIdM },
  /* 5 */ { BxLockable, &BX_CPU_C::SUB_EqIdM },
  /* 6 */ { BxLockable, &BX_CPU_C::XOR_EqIdM },
  /* 7 */ { 0,          &BX_CPU_C::CMP_EqIdM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G1EqR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ADD_EqIdR },
  /* 1 */ { 0, &BX_CPU_C::OR_EqIdR },
  /* 2 */ { 0, &BX_CPU_C::ADC_EqIdR },
  /* 3 */ { 0, &BX_CPU_C::SBB_EqIdR },
  /* 4 */ { 0, &BX_CPU_C::AND_EqIdR },
  /* 5 */ { 0, &BX_CPU_C::SUB_EqIdR },
  /* 6 */ { 0, &BX_CPU_C::XOR_EqIdR },
  /* 7 */ { 0, &BX_CPU_C::CMP_EqIdR }
};
#endif

/* ******* */
/* Group 2 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG2Eb[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ROL_Eb },
  /* 1 */ { 0, &BX_CPU_C::ROR_Eb },
  /* 2 */ { 0, &BX_CPU_C::RCL_Eb },
  /* 3 */ { 0, &BX_CPU_C::RCR_Eb },
  /* 4 */ { 0, &BX_CPU_C::SHL_Eb },
  /* 5 */ { 0, &BX_CPU_C::SHR_Eb },
  /* 6 */ { 0, &BX_CPU_C::SHL_Eb },
  /* 7 */ { 0, &BX_CPU_C::SAR_Eb }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG2Ew[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ROL_Ew },
  /* 1 */ { 0, &BX_CPU_C::ROR_Ew },
  /* 2 */ { 0, &BX_CPU_C::RCL_Ew },
  /* 3 */ { 0, &BX_CPU_C::RCR_Ew },
  /* 4 */ { 0, &BX_CPU_C::SHL_Ew },
  /* 5 */ { 0, &BX_CPU_C::SHR_Ew },
  /* 6 */ { 0, &BX_CPU_C::SHL_Ew },
  /* 7 */ { 0, &BX_CPU_C::SAR_Ew }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG2Ed[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ROL_Ed },
  /* 1 */ { 0, &BX_CPU_C::ROR_Ed },
  /* 2 */ { 0, &BX_CPU_C::RCL_Ed },
  /* 3 */ { 0, &BX_CPU_C::RCR_Ed },
  /* 4 */ { 0, &BX_CPU_C::SHL_Ed },
  /* 5 */ { 0, &BX_CPU_C::SHR_Ed },
  /* 6 */ { 0, &BX_CPU_C::SHL_Ed },
  /* 7 */ { 0, &BX_CPU_C::SAR_Ed }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G2Eq[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::ROL_Eq },
  /* 1 */ { 0, &BX_CPU_C::ROR_Eq },
  /* 2 */ { 0, &BX_CPU_C::RCL_Eq },
  /* 3 */ { 0, &BX_CPU_C::RCR_Eq },
  /* 4 */ { 0, &BX_CPU_C::SHL_Eq },
  /* 5 */ { 0, &BX_CPU_C::SHR_Eq },
  /* 6 */ { 0, &BX_CPU_C::SHL_Eq },
  /* 7 */ { 0, &BX_CPU_C::SAR_Eq }
};
#endif

/* ******* */
/* Group 3 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG3EbM[8] = {
  /* 0 */ { BxImmediate_Ib, &BX_CPU_C::TEST_EbIbM },
  /* 1 */ { BxImmediate_Ib, &BX_CPU_C::TEST_EbIbM },
  /* 2 */ { BxLockable,     &BX_CPU_C::NOT_EbM },
  /* 3 */ { BxLockable,     &BX_CPU_C::NEG_EbM },
  /* 4 */ { 0,              &BX_CPU_C::MUL_ALEb },
  /* 5 */ { 0,              &BX_CPU_C::IMUL_ALEb },
  /* 6 */ { 0,              &BX_CPU_C::DIV_ALEb },
  /* 7 */ { 0,              &BX_CPU_C::IDIV_ALEb }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG3EbR[8] = {
  /* 0 */ { BxImmediate_Ib, &BX_CPU_C::TEST_EbIbR },
  /* 1 */ { BxImmediate_Ib, &BX_CPU_C::TEST_EbIbR },
  /* 2 */ { 0, &BX_CPU_C::NOT_EbR },
  /* 3 */ { 0, &BX_CPU_C::NEG_EbR },
  /* 4 */ { 0, &BX_CPU_C::MUL_ALEb },
  /* 5 */ { 0, &BX_CPU_C::IMUL_ALEb },
  /* 6 */ { 0, &BX_CPU_C::DIV_ALEb },
  /* 7 */ { 0, &BX_CPU_C::IDIV_ALEb }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG3EwM[8] = {
  /* 0 */ { BxImmediate_Iw, &BX_CPU_C::TEST_EwIwM },
  /* 1 */ { BxImmediate_Iw, &BX_CPU_C::TEST_EwIwM },
  /* 2 */ { BxLockable,     &BX_CPU_C::NOT_EwM },
  /* 3 */ { BxLockable,     &BX_CPU_C::NEG_EwM },
  /* 4 */ { 0,              &BX_CPU_C::MUL_AXEw },
  /* 5 */ { 0,              &BX_CPU_C::IMUL_AXEw },
  /* 6 */ { 0,              &BX_CPU_C::DIV_AXEw },
  /* 7 */ { 0,              &BX_CPU_C::IDIV_AXEw }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG3EwR[8] = {
  /* 0 */ { BxImmediate_Iw, &BX_CPU_C::TEST_EwIwR },
  /* 1 */ { BxImmediate_Iw, &BX_CPU_C::TEST_EwIwR },
  /* 2 */ { 0, &BX_CPU_C::NOT_EwR },
  /* 3 */ { 0, &BX_CPU_C::NEG_EwR },
  /* 4 */ { 0, &BX_CPU_C::MUL_AXEw },
  /* 5 */ { 0, &BX_CPU_C::IMUL_AXEw },
  /* 6 */ { 0, &BX_CPU_C::DIV_AXEw },
  /* 7 */ { 0, &BX_CPU_C::IDIV_AXEw }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG3EdM[8] = {
  /* 0 */ { BxImmediate_Id, &BX_CPU_C::TEST_EdIdM },
  /* 1 */ { BxImmediate_Id, &BX_CPU_C::TEST_EdIdM },
  /* 2 */ { BxLockable,     &BX_CPU_C::NOT_EdM },
  /* 3 */ { BxLockable,     &BX_CPU_C::NEG_EdM },
  /* 4 */ { 0,              &BX_CPU_C::MUL_EAXEd },
  /* 5 */ { 0,              &BX_CPU_C::IMUL_EAXEd },
  /* 6 */ { 0,              &BX_CPU_C::DIV_EAXEd },
  /* 7 */ { 0,              &BX_CPU_C::IDIV_EAXEd }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG3EdR[8] = {
  /* 0 */ { BxImmediate_Id, &BX_CPU_C::TEST_EdIdR },
  /* 1 */ { BxImmediate_Id, &BX_CPU_C::TEST_EdIdR },
  /* 2 */ { 0, &BX_CPU_C::NOT_EdR },
  /* 3 */ { 0, &BX_CPU_C::NEG_EdR },
  /* 4 */ { 0, &BX_CPU_C::MUL_EAXEd },
  /* 5 */ { 0, &BX_CPU_C::IMUL_EAXEd },
  /* 6 */ { 0, &BX_CPU_C::DIV_EAXEd },
  /* 7 */ { 0, &BX_CPU_C::IDIV_EAXEd }
}; 

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G3EqM[8] = {
  /* 0 */ { BxImmediate_Id, &BX_CPU_C::TEST_EqIdM },
  /* 1 */ { BxImmediate_Id, &BX_CPU_C::TEST_EqIdM },
  /* 2 */ { BxLockable,     &BX_CPU_C::NOT_EqM },
  /* 3 */ { BxLockable,     &BX_CPU_C::NEG_EqM },
  /* 4 */ { 0,              &BX_CPU_C::MUL_RAXEq },
  /* 5 */ { 0,              &BX_CPU_C::IMUL_RAXEq },
  /* 6 */ { 0,              &BX_CPU_C::DIV_RAXEq },
  /* 7 */ { 0,              &BX_CPU_C::IDIV_RAXEq }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G3EqR[8] = {
  /* 0 */ { BxImmediate_Id, &BX_CPU_C::TEST_EqIdR },
  /* 1 */ { BxImmediate_Id, &BX_CPU_C::TEST_EqIdR },
  /* 2 */ { 0, &BX_CPU_C::NOT_EqR },
  /* 3 */ { 0, &BX_CPU_C::NEG_EqR },
  /* 4 */ { 0, &BX_CPU_C::MUL_RAXEq },
  /* 5 */ { 0, &BX_CPU_C::IMUL_RAXEq },
  /* 6 */ { 0, &BX_CPU_C::DIV_RAXEq },
  /* 7 */ { 0, &BX_CPU_C::IDIV_RAXEq }
};
#endif

/* ******* */
/* Group 4 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG4M[8] = {
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EbM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EbM },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG4R[8] = {
  /* 0 */ { 0, &BX_CPU_C::INC_EbR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EbR },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

/* ******* */
/* Group 5 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG5wM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EwM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EwM },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Ew },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::CALL16_Ep }, 
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_EwM },
  /* 5 */ { BxTraceEnd, &BX_CPU_C::JMP16_Ep },
  /* 6 */ { 0, &BX_CPU_C::PUSH_EwM },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG5wR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::INC_EwR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EwR },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Ew },
  /* 3 */ { 0, &BX_CPU_C::BxError }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_EwR  },
  /* 5 */ { 0, &BX_CPU_C::BxError }, // JMP16_Ep
  /* 6 */ { 0, &BX_CPU_C::PUSH_EwR },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG5dM[8] = {
  // attributes defined in main area
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EdM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EdM },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Ed },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::CALL32_Ep },
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_EdM },
  /* 5 */ { BxTraceEnd, &BX_CPU_C::JMP32_Ep },
  /* 6 */ { 0, &BX_CPU_C::PUSH_EdM },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG5dR[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::INC_EdR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EdR },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Ed },
  /* 3 */ { 0, &BX_CPU_C::BxError }, // CALL32_Ep
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_EdR  },
  /* 5 */ { 0, &BX_CPU_C::BxError }, // JMP32_Ep
  /* 6 */ { 0, &BX_CPU_C::PUSH_EdR },
  /* 7 */ { 0, &BX_CPU_C::BxError }
}; 

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G5wM[8] = {
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EwM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EwM },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::CALL16_Ep },
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq },
  /* 5 */ { BxTraceEnd, &BX_CPU_C::JMP16_Ep },
  /* 6 */ { 0, &BX_CPU_C::PUSH_EwM },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5wR[8] = {
  /* 0 */ { 0, &BX_CPU_C::INC_EwR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EwR },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { 0, &BX_CPU_C::BxError }, // CALL16_Ep
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq  },
  /* 5 */ { 0, &BX_CPU_C::BxError }, // JMP16_Ep
  /* 6 */ { 0, &BX_CPU_C::PUSH_EwR },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dM[8] = {
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EdM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EdM },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::CALL32_Ep },
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq },
  /* 5 */ { BxTraceEnd, &BX_CPU_C::JMP32_Ep },
  /* 6 */ { 0, &BX_CPU_C::PUSH_EqM },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5dR[8] = {
  /* 0 */ { 0, &BX_CPU_C::INC_EdR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EdR },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { 0, &BX_CPU_C::BxError }, // CALL32_Ep
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq  },
  /* 5 */ { 0, &BX_CPU_C::BxError }, // JMP32_Ep
  /* 6 */ { 0, &BX_CPU_C::PUSH_EqR },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qM[8] = {
  /* 0 */ { BxLockable, &BX_CPU_C::INC_EqM },
  /* 1 */ { BxLockable, &BX_CPU_C::DEC_EqM },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::CALL64_Ep },
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq },
  /* 5 */ { BxTraceEnd, &BX_CPU_C::JMP64_Ep },
  /* 6 */ { 0, &BX_CPU_C::PUSH_EqM },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G5qR[8] = {
  /* 0 */ { 0, &BX_CPU_C::INC_EqR },
  /* 1 */ { 0, &BX_CPU_C::DEC_EqR },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::CALL_Eq },
  /* 3 */ { 0, &BX_CPU_C::BxError }, // CALL64_Ep
  /* 4 */ { BxTraceEnd, &BX_CPU_C::JMP_Eq  },
  /* 5 */ { 0, &BX_CPU_C::BxError }, // JMP64_Ep
  /* 6 */ { 0, &BX_CPU_C::PUSH_EqR },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};
#endif

/* ******* */
/* Group 6 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG6[8] = {
  /* 0 */ { 0, &BX_CPU_C::SLDT_Ew },
  /* 1 */ { 0, &BX_CPU_C::STR_Ew },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::LLDT_Ew },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::LTR_Ew },
  /* 4 */ { 0, &BX_CPU_C::VERR_Ew },
  /* 5 */ { 0, &BX_CPU_C::VERW_Ew },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
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
  /* 0 */ { 0, &BX_CPU_C::MONITOR },
  /* 1 */ { BxTraceEnd, &BX_CPU_C::MWAIT },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfoG7R[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { BxRMGroup, NULL, opcodesGroupRmMONITOR },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::SMSW_Ew },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxTraceEnd, &BX_CPU_C::LMSW_Ew },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfoG7M[8] = {
  // attributes defined in main area
  /* 0 */ { 0, &BX_CPU_C::SGDT_Ms },
  /* 1 */ { 0, &BX_CPU_C::SIDT_Ms },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::LGDT_Ms },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::LIDT_Ms },
  /* 4 */ { 0, &BX_CPU_C::SMSW_Ew },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxTraceEnd, &BX_CPU_C::LMSW_Ew },
  /* 7 */ { BxTraceEnd, &BX_CPU_C::INVLPG }
}; 

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t opcodesGroup64RmINVLPG[8] = {
  /* 0 */ { 0, &BX_CPU_C::SWAPGS  },
  /* 1 */ { 0, &BX_CPU_C::RDTSCP  },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G7R[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { BxRMGroup, NULL, opcodesGroupRmMONITOR },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::SMSW_Ew },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxTraceEnd, &BX_CPU_C::LMSW_Ew },
  /* 7 */ { BxRMGroup, NULL, opcodesGroup64RmINVLPG }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G7M[8] = {
  /* 0 */ { 0, &BX_CPU_C::SGDT64_Ms },
  /* 1 */ { 0, &BX_CPU_C::SIDT64_Ms },
  /* 2 */ { BxTraceEnd, &BX_CPU_C::LGDT64_Ms },
  /* 3 */ { BxTraceEnd, &BX_CPU_C::LIDT64_Ms },
  /* 4 */ { 0, &BX_CPU_C::SMSW_Ew },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxTraceEnd, &BX_CPU_C::LMSW_Ew },
  /* 7 */ { BxTraceEnd, &BX_CPU_C::INVLPG  }
};
#endif


/* ******* */
/* Group 8 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbM[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib,              &BX_CPU_C::BT_EwIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTS_EwIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTR_EwIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTC_EwIbM }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG8EwIbR[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib, &BX_CPU_C::BT_EwIbR  },
  /* 5 */ { BxImmediate_Ib, &BX_CPU_C::BTS_EwIbR },
  /* 6 */ { BxImmediate_Ib, &BX_CPU_C::BTR_EwIbR },
  /* 7 */ { BxImmediate_Ib, &BX_CPU_C::BTC_EwIbR }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbM[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib,              &BX_CPU_C::BT_EdIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTS_EdIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTR_EdIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTC_EdIbM }
}; 

static const BxOpcodeInfo_t BxOpcodeInfoG8EdIbR[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib, &BX_CPU_C::BT_EdIbR  },
  /* 5 */ { BxImmediate_Ib, &BX_CPU_C::BTS_EdIbR },
  /* 6 */ { BxImmediate_Ib, &BX_CPU_C::BTR_EdIbR },
  /* 7 */ { BxImmediate_Ib, &BX_CPU_C::BTC_EdIbR }
}; 

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbM[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib,              &BX_CPU_C::BT_EqIbM  },
  /* 5 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTS_EqIbM },
  /* 6 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTR_EqIbM },
  /* 7 */ { BxImmediate_Ib | BxLockable, &BX_CPU_C::BTC_EqIbM }
};

static const BxOpcodeInfo_t BxOpcodeInfo64G8EqIbR[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib, &BX_CPU_C::BT_EqIbR  },
  /* 5 */ { BxImmediate_Ib, &BX_CPU_C::BTS_EqIbR },
  /* 6 */ { BxImmediate_Ib, &BX_CPU_C::BTR_EqIbR },
  /* 7 */ { BxImmediate_Ib, &BX_CPU_C::BTC_EqIbR }
};
#endif

/* ******* */
/* Group 9 */
/* ******* */

static const BxOpcodeInfo_t BxOpcodeInfoG9M[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { BxLockable, &BX_CPU_C::CMPXCHG8B },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

#if BX_SUPPORT_X86_64
static const BxOpcodeInfo_t BxOpcodeInfo64G9qM[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { BxLockable, &BX_CPU_C::CMPXCHG16B },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};
#endif

/* ******** */
/* Group 12 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG12[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1202 },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1204 },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1206 },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

/* ******** */
/* Group 13 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG13[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1302 },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1304 },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1306 },
  /* 7 */ { 0, &BX_CPU_C::BxError }
};

/* ******** */
/* Group 14 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG14[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1402 },
  /* 3 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1403 },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1406 },
  /* 7 */ { BxImmediate_Ib | BxPrefixSSE, NULL, BxOpcodeGroupSSE_G1407 } 
};

/* ******** */
/* Group 15 */
/* ******** */

static const BxOpcodeInfo_t BxOpcodeInfoG15M[8] = {
  /* 0 */ { 0, &BX_CPU_C::FXSAVE  },
  /* 1 */ { 0, &BX_CPU_C::FXRSTOR },
  /* 2 */ { 0, &BX_CPU_C::LDMXCSR },
  /* 3 */ { 0, &BX_CPU_C::STMXCSR },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::BxError },
  /* 6 */ { 0, &BX_CPU_C::BxError },
  /* 7 */ { 0, &BX_CPU_C::CLFLUSH }
};

static const BxOpcodeInfo_t BxOpcodeInfoG15R[8] = {
  /* 0 */ { 0, &BX_CPU_C::BxError },
  /* 1 */ { 0, &BX_CPU_C::BxError },
  /* 2 */ { 0, &BX_CPU_C::BxError },
  /* 3 */ { 0, &BX_CPU_C::BxError },
  /* 4 */ { 0, &BX_CPU_C::BxError },
  /* 5 */ { 0, &BX_CPU_C::NOP }, /* LFENCE */
  /* 6 */ { 0, &BX_CPU_C::NOP }, /* MFENCE */
  /* 7 */ { 0, &BX_CPU_C::NOP }  /* SFENCE */
};

#endif // BX_COMMON_FETCHDECODE_TABLES_H
