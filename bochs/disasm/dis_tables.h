#ifndef _BX_DISASM_TABLES_
#define _BX_DISASM_TABLES_

#define BX_DISASM_NEED_SHORTCUTS

#define _PREFIX        1
#define _GROUPN        2
#define _SPLIT11B      3
#define _GRPFP         4
#define _GRP3DNOW      5
#define _GRPSSE        6

struct BxDisasmOpcodeInfo_t
{
    const char *Opcode;
    Bit32u Attr;
    BxDisasmPtr_t Operand1;
    unsigned  Op1Attr;
    BxDisasmPtr_t Operand2;
    unsigned  Op2Attr;
    BxDisasmPtr_t Operand3;
    unsigned  Op3Attr;
    struct BxDisasmOpcodeInfo_t *AnotherArray;
};

#define GRPSSE(n)       "(error)",  _GRPSSE, NULL, 0, NULL, 0, NULL, 0, BxDisasmGroupSSE_##n
#define GRPN(n)         "(error)",  _GROUPN, NULL, 0, NULL, 0, NULL, 0, BxDisasmGroup##n
#define GRPFP(n)        "(error)",  _GRPFP,  NULL, 0, NULL, 0, NULL, 0, BxDisasmFPGroup##n

#define GRP3DNOW        "(error)",  _GRP3DNOW, NULL, 0, NULL, 0, NULL, 0, BxDisasm3DNowGroup

#define PREFIX_CS       "cs",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_DS       "ds",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_ES       "es",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_SS       "ss",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_FS       "fs",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_GS       "gs",       _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_OPSIZE   "opsize",   _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_ADDRSIZE "addrsize", _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_LOCK     "lock",     _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_REP      "rep",      _PREFIX, NULL, 0, NULL, 0, NULL, 0
#define PREFIX_REPNE    "repne",    _PREFIX, NULL, 0, NULL, 0, NULL, 0

#ifdef BX_DISASM_NEED_SHORTCUTS

 // general
#define XX  &disassembler::XX,  0

 // fpu
#define ST0 &disassembler::ST0, 0
#define STj &disassembler::STj, 0

 // control/debug register
#define Cd  &disassembler::Cd,  0
#define Dd  &disassembler::Dd,  0
#define Td  &disassembler::Td,  0

#define Sw  &disassembler::Sw,  0
#define Rw  &disassembler::Rw,  0
#define Rd  &disassembler::Rd,  0

 // segment register
#define CS  &disassembler::OP_SEG, CS_REG
#define DS  &disassembler::OP_SEG, DS_REG
#define ES  &disassembler::OP_SEG, ES_REG
#define SS  &disassembler::OP_SEG, SS_REG
#define GS  &disassembler::OP_SEG, GS_REG
#define FS  &disassembler::OP_SEG, FS_REG

 // 16/32-bit general purpose register 
#define eAX &disassembler::reg32, eAX_REG
#define eCX &disassembler::reg32, eCX_REG
#define eDX &disassembler::reg32, eDX_REG
#define eBX &disassembler::reg32, eBX_REG
#define eSI &disassembler::reg32, eSI_REG
#define eDI &disassembler::reg32, eDI_REG
#define eBP &disassembler::reg32, eBP_REG
#define eSP &disassembler::reg32, eSP_REG

 // 8-bit general purpose register 
#define AL  &disassembler::reg8,  AL_REG
#define AH  &disassembler::reg8,  AH_REG
#define BL  &disassembler::reg8,  BL_REG
#define BH  &disassembler::reg8,  BH_REG
#define CL  &disassembler::reg8,  CL_REG
#define CH  &disassembler::reg8,  CH_REG
#define DL  &disassembler::reg8,  DL_REG
#define DH  &disassembler::reg8,  DH_REG

 // 8-bit general purpose register 
#define AX  &disassembler::reg16, AX_REG
#define DX  &disassembler::reg16, DX_REG

 // mmx
#define Pd  &disassembler::OP_P,  D_MODE
#define Pq  &disassembler::OP_P,  Q_MODE
#define Qd  &disassembler::OP_Q,  D_MODE
#define Qq  &disassembler::OP_Q,  Q_MODE

 // xmm
#define Vd  &disassembler::OP_V,  D_MODE
#define Vq  &disassembler::OP_V,  Q_MODE
#define Vdq &disassembler::OP_V,  O_MODE
#define Vss &disassembler::OP_V,  D_MODE
#define Vsd &disassembler::OP_V,  Q_MODE
#define Vps &disassembler::OP_V,  O_MODE
#define Vpd &disassembler::OP_V,  O_MODE

#define Wq  &disassembler::OP_W,  Q_MODE
#define Wdq &disassembler::OP_W,  O_MODE
#define Wss &disassembler::OP_W,  D_MODE
#define Wsd &disassembler::OP_W,  Q_MODE
#define Wps &disassembler::OP_W,  O_MODE
#define Wpd &disassembler::OP_W,  O_MODE

#define Xb  &disassembler::OP_X,  B_MODE
#define Yb  &disassembler::OP_Y,  B_MODE
#define Xv  &disassembler::OP_X,  V_MODE
#define Xve &disassembler::OP_X,  V_MODE|0x80
#define Yv  &disassembler::OP_Y,  V_MODE
#define Yve &disassembler::OP_Y,  V_MODE|0x80

 // immediate
#define I1 &disassembler::I1, 0
#define Ib &disassembler::Ib, 0
#define Iw &disassembler::Iw, 0
#define Id &disassembler::Id, 0
#define Iv &disassembler::Iv, 0

 // sign-extended immediate
#define sIb &disassembler::sIb, 0

 // memory only
#define Mb  &disassembler::OP_MEM, B_MODE
#define Mw  &disassembler::OP_MEM, W_MODE
#define Mv  &disassembler::OP_MEM, V_MODE
#define Md  &disassembler::OP_MEM, D_MODE
#define Mp  &disassembler::OP_MEM, P_MODE
#define Mq  &disassembler::OP_MEM, Q_MODE
#define Mt  &disassembler::OP_MEM, T_MODE
#define Mx  &disassembler::OP_MEM, X_MODE
#define Mdq &disassembler::OP_MEM, O_MODE
#define Mps &disassembler::OP_MEM, O_MODE
#define Mpd &disassembler::OP_MEM, O_MODE

 // still unsorted
#define Gb  &disassembler::Gb, 0
#define Gw  &disassembler::Gw, 0
#define Gv  &disassembler::Gv, 0
#define Gd  &disassembler::Gd, 0

#define Eb  &disassembler::Eb, 0
#define Ew  &disassembler::Ew, 0
#define Ed  &disassembler::Ed, 0
#define Ev  &disassembler::Ev, 0
#define Ea  &disassembler::Ea, 0
#define Ep  &disassembler::Ep, 0

 // jump
#define Jb  &disassembler::Jb, 0
#define Jv  &disassembler::Jv, 0

#define Ap  &disassembler::Ap, 0

#define Ob  &disassembler::Ob, 0
#define Ov  &disassembler::Ov, 0

#endif

/* ************************************************************************ */
/* SSE opcodes */

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f10[4] = {
  /* -- */  { "movups",     0, Vps, Wps, XX },
  /* 66 */  { "movupd",     0, Vpd, Wpd, XX },
  /* F2 */  { "movdq",      0, Vdq, Wdq, XX },
  /* F3 */  { "movdq",      0, Vdq, Wdq, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f11[4] = {
  /* -- */  { "movups",     0, Wps, Vps, XX },
  /* 66 */  { "movupd",     0, Wpd, Vpd, XX },
  /* F2 */  { "movdq",      0, Wdq, Vdq, XX },
  /* F3 */  { "movdq",      0, Wdq, Vdq, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f12[4] = {
  /* -- */  { "movlps",     0, Vps,  Mq, XX },
  /* 66 */  { "movlpd",     0, Vpd,  Mq, XX },
  /* F2 */  { "movddup",    0, Vdq,  Wq, XX },
  /* F3 */  { "movsldup",   0, Vdq, Wdq, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f13[4] = {
  /* -- */  { "movlps",     0,  Mq, Vps, XX },
  /* 66 */  { "movlpd",     0,  Mq, Vpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };                        

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f14[4] = {
  /* -- */  { "unpcklps",   0, Vps, Wq, XX },
  /* 66 */  { "unpcklpd",   0, Vpd, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f15[4] = {
  /* -- */  { "unpckhps",   0, Vps, Wq, XX },
  /* 66 */  { "unpckhpd",   0, Vpd, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f16[4] = {
  /* -- */  { "movhps",     0, Vps,  Mq, XX  },
  /* 66 */  { "movhpd",     0, Vpd,  Mq, XX  },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "movshdup",   0, Vdq, Wdq, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f17[4] = {
  /* -- */  { "movhps",     0, Mq, Vps, XX },
  /* 66 */  { "mpvhpd",     0, Mq, Vpd, XX },
  /* F2 */  { "(invalid)",  0, XX,  XX, XX },
  /* F3 */  { "(invalid)",  0, XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f28[4] = {
  /* -- */  { "movaps",     0, Vps, Wps, XX },
  /* 66 */  { "movapd",     0, Vpd, Wpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f29[4] = {
  /* -- */  { "movaps",     0, Wps, Vps, XX },
  /* 66 */  { "movapd",     0, Wpd, Vpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2a[4] = {
  /* -- */  { "cvtpi2ps",   0, Vps,  Qq, XX },
  /* 66 */  { "cvtpi2pd",   0, Vpd,  Qd, XX },
  /* F2 */  { "cvtsi2sd",   0, Vsd,  Ed, XX },
  /* F3 */  { "cvtsi2ss",   0, Vss,  Ed, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2b[4] = {
  /* -- */  { "movntps",    0, Mps, Vps, XX },
  /* 66 */  { "movntpd",    0, Mpd, Vpd, XX },                  
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2c[4] = {
  /* -- */  { "cvtps2pi",   0,  Pq, Wps, XX },
  /* 66 */  { "cvtpd2pi",   0,  Pq, Wpd, XX },
  /* F2 */  { "cvtsd2si",   0,  Gd, Wsd, XX },
  /* F3 */  { "cvtss2ss",   0,  Gd, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2d[4] = {
  /* -- */  { "cvtps2pi",   0,  Pq, Wps, XX },
  /* 66 */  { "cvtpd2pi",   0,  Pq, Wpd, XX },
  /* F2 */  { "cvtsd2si",   0,  Gd, Wsd, XX },
  /* F3 */  { "cvtss2si",   0,  Gd, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2e[4] = {
  /* -- */  { "ucomiss",    0, Vss, Wss, XX },
  /* 66 */  { "ucomisd",    0, Vsd, Wss, XX },                  
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f2f[4] = {
  /* -- */  { "comiss",     0, Vss, Wss, XX },
  /* 66 */  { "comisd",     0, Vsd, Wsd, XX },   
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f50[4] = {
  /* -- */  { "movmskps",   0,  Gd, Vps, XX },
  /* 66 */  { "movmskpd",   0,  Ed, Vpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f51[4] = {
  /* -- */  { "sqrtps",     0, Vps, Wps, XX },
  /* 66 */  { "sqrtpd",     0, Vpd, Wpd, XX },
  /* F2 */  { "sqrtsd",     0, Vsd, Wsd, XX },
  /* F3 */  { "sqrtss",     0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f52[4] = {
  /* -- */  { "rsqrtps",    0, Vps, Wps, XX },
  /* 66 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "rsqrtss",    0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f53[4] = {
  /* -- */  { "rcpps",      0, Vps, Wps, XX },
  /* 66 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "rcpss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f54[4] = {
  /* -- */  { "andps",      0, Vps, Wps, XX },
  /* 66 */  { "andpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f55[4] = {
  /* -- */  { "andnps",     0, Vps, Wps, XX },
  /* 66 */  { "andnpd",     0, Vpd, Wpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f56[4] = {
  /* -- */  { "orps",       0, Vps, Wps, XX },
  /* 66 */  { "orpd",       0, Vpd, Wpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f57[4] = {
  /* -- */  { "xorps",      0, Vps, Wps, XX },
  /* 66 */  { "xorpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f58[4] = {
  /* -- */  { "addps",      0, Vps, Wps, XX },
  /* 66 */  { "addpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "addsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "addss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f59[4] = {
  /* -- */  { "mulps",      0, Vps, Wps, XX },
  /* 66 */  { "mulpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "mulsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "mulss",      0, Vss, Wss, XX }
  };                   

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5a[4] = {
  /* -- */  { "cvtps2pd",   0, Vpd, Wps, XX },
  /* 66 */  { "cvtpd2ps",   0, Vps, Wpd, XX },
  /* F2 */  { "cvtsd2ss",   0, Vss, Wsd, XX },
  /* F3 */  { "cvtss2sd",   0, Vsd, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5b[4] = {
  /* -- */  { "cvtdq2ps",   0, Vps, Wdq, XX },
  /* 66 */  { "cvtps2dq",   0, Vdq, Wps, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "cvttps2dq",  0, Vdq, Wps, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5c[4] = {
  /* -- */  { "subps",      0, Vps, Wps, XX },
  /* 66 */  { "subpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "subsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "subss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5d[4] = {
  /* -- */  { "minps",      0, Vps, Wps, XX },
  /* 66 */  { "minpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "minsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "minss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5e[4] = {
  /* -- */  { "divps",      0, Vps, Wps, XX },
  /* 66 */  { "divpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "divsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "divss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f5f[4] = {
  /* -- */  { "maxps",      0, Vps, Wps, XX },
  /* 66 */  { "maxpd",      0, Vpd, Wpd, XX },
  /* F2 */  { "maxsd",      0, Vsd, Wsd, XX },
  /* F3 */  { "maxss",      0, Vss, Wss, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f60[4] = {
  /* -- */  { "punpcklbw",  0,  Pq, Qd, XX },
  /* 66 */  { "punpcklbw",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f61[4] = {
  /* -- */  { "punpcklwd",  0,  Pq, Qd, XX },
  /* 66 */  { "punpcklwd",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f62[4] = {
  /* -- */  { "punpckldq",  0,  Pq, Qd, XX },
  /* 66 */  { "punpckldq",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f63[4] = {
  /* -- */  { "packsswb",   0,  Pq, Qq, XX },
  /* 66 */  { "packsswb",   0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f64[4] = {
  /* -- */  { "pcmpgtb",    0,  Pq, Qq, XX },
  /* 66 */  { "pcmpgtb",    0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f65[4] = {
  /* -- */  { "pcmpgtw",    0,  Pq, Qq, XX },
  /* 66 */  { "pcmpgtw",    0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f66[4] = {
  /* -- */  { "pcmpgtd",    0,  Pq,  Qq, XX },
  /* 66 */  { "pcmpgtd",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f67[4] = {
  /* -- */  { "packuswb",   0,  Pq,  Qq, XX },
  /* 66 */  { "packuswb",   0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f68[4] = {
  /* -- */  { "punpckhbw",  0,  Pq, Qq, XX },
  /* 66 */  { "punpckhbw",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f69[4] = {
  /* -- */  { "punpckhwd",  0,  Pq, Qq, XX },
  /* 66 */  { "punpckhwd",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6a[4] = {
  /* -- */  { "punpckhdq",  0,  Pq, Qq, XX },
  /* 66 */  { "punpckhdq",  0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6b[4] = {
  /* -- */  { "packssdw",   0,  Pq,  Qq, XX },
  /* 66 */  { "packssdw",   0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6c[4] = {
  /* -- */  { "(invalid)",  0,  XX, XX, XX },
  /* 66 */  { "punpcklqdq", 0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6d[4] = {
  /* -- */  { "(invalid)",  0,  XX, XX, XX },
  /* 66 */  { "punpckhqdq", 0, Vdq, Wq, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6e[4] = {
  /* -- */  { "movd",       0,  Pq, Ed, XX },
  /* 66 */  { "movd",       0, Vdq, Ed, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f6f[4] = {
  /* -- */  { "movq",       0,  Pq,  Qq, XX },
  /* 66 */  { "movdqa",     0, Vdq, Wdq, XX },
  /* F2 */  { "movdqa",     0,  XX,  XX, XX },
  /* F3 */  { "movdqu",     0, Vdq, Wdq, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f70[4] = {
  /* -- */  { "pshufw",     0,  Pq,  Qq, Ib },
  /* 66 */  { "pshufd",     0, Vdq, Wdq, Ib },
  /* F2 */  { "pshufhw",    0,  Vq,  Wq, Ib },
  /* F3 */  { "pshuflw",    0,  Vq,  Wq, Ib }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f74[4] = {
  /* -- */  { "pcmpeqb",    0,  Pq,  Qq, XX },
  /* 66 */  { "pcmpeqb",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f75[4] = {
  /* -- */  { "pcmpeqw",    0,  Pq,  Qq, XX },
  /* 66 */  { "pcmpeqw",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f76[4] = {
  /* -- */  { "pcmpeqd",    0,  Pq,  Qq, XX },
  /* 66 */  { "pcmpeqd",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f7c[4] = {
  /* -- */  { "(invalid)",  0,  XX,  XX, XX },
  /* 66 */  { "haddpd",     0, Vpd, Wpd, XX },
  /* F2 */  { "haddps",     0, Vps, Wps, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f7d[4] = {
  /* -- */  { "(invalid)",  0,  XX,  XX, XX },
  /* 66 */  { "hsubpd",     0, Vpd, Wpd, XX },
  /* F2 */  { "hsubps",     0, Vps, Wps, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f7e[4] = {
  /* -- */  { "movd",       0,  Ed, Pq, XX },
  /* 66 */  { "movd",       0,  Ed, Vd, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "movq",       0,  Vq, Wq, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0f7f[4] = {
  /* -- */  { "movq",       0,  Qq,  Pq, XX },
  /* 66 */  { "movdqa",     0, Wdq, Vdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "movdqu",     0, Wdq, Vdq, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fc2[4] = {
  /* -- */  { "cmpps",      0, Vps, Wps, Ib },
  /* 66 */  { "cmppd",      0, Vpd, Wpd, Ib },
  /* F2 */  { "cmpsd",      0, Vsd, Wsd, Ib },
  /* F3 */  { "cmpss",      0, Vss, Wss, Ib }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fc3[4] = {
  /* -- */  { "movnti",     0, Md, Gd, XX },
  /* 66 */  { "(invalid)",  0, XX, XX, XX },
  /* F2 */  { "(invalid)",  0, XX, XX, XX },
  /* F3 */  { "(invalid)",  0, XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fc4[4] = {
  /* -- */  { "pinsrw",     0,  Pq, Ed, Ib },
  /* 66 */  { "pinsrw",     0, Vdq, Ed, Ib },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fc5[4] = {
  /* -- */  { "pextrw",     0,  Pq, Ed, Ib },
  /* 66 */  { "pextrw",     0, Vdq, Ed, Ib },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fc6[4] = {
  /* -- */  { "shufps",     0, Vps, Wps, Ib },
  /* 66 */  { "shufpd",     0, Vpd, Wpd, Ib },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd0[4] = {
  /* -- */  { "(invalid)",  0,  XX,  XX, XX },
  /* 66 */  { "addsubpd",   0, Vpd, Wpd, XX },
  /* F2 */  { "addsubps",   0, Vps, Wps, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd1[4] = {
  /* -- */  { "psrlw",      0,  Pq,  Qq, XX },
  /* 66 */  { "psrlw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd2[4] = {
  /* -- */  { "psrld",      0,  Pq,  Qq, XX },
  /* 66 */  { "psrld",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd3[4] = {
  /* -- */  { "psrlq",      0,  Pq,  Qq, XX },
  /* 66 */  { "psrlq",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd4[4] = {
  /* -- */  { "paddq",      0,  Pq,  Qq, XX },
  /* 66 */  { "paddq",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd5[4] = {
  /* -- */  { "pmullw",     0,  Pq,  Qq, XX },
  /* 66 */  { "pmullw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd6[4] = {
  /* -- */  { "(invalid)",  0,  XX, XX, XX },
  /* 66 */  { "movq",       0,  Wq, Vq, XX },
  /* F2 */  { "movdq2q",    0,  Pq, Vq, XX },
  /* F3 */  { "movq2dq",    0, Vdq, Qq, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd7[4] = {
  /* -- */  { "pmovmskb",   0, Gd,  Pq, XX },
  /* 66 */  { "pmovmskb",   0, Gd, Vdq, XX },
  /* F2 */  { "(invalid)",  0, XX,  XX, XX },
  /* F3 */  { "(invalid)",  0, XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd8[4] = {
  /* -- */  { "psubusb",    0,  Pq,  Qq, XX },
  /* 66 */  { "psubusb",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fd9[4] = {
  /* -- */  { "psubusw",    0, Pq, Qq, XX   },
  /* 66 */  { "psubusw",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fda[4] = {
  /* -- */  { "pminub",     0,  Pq,  Qq, XX },
  /* 66 */  { "pminub",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fdb[4] = {
  /* -- */  { "pand",       0,  Pq,  Qq, XX },
  /* 66 */  { "pand",       0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fdc[4] = {
  /* -- */  { "paddusb",    0,  Pq,  Qq, XX },
  /* 66 */  { "paddusb",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fdd[4] = {
  /* -- */  { "paddusw",    0,  Pq,  Qq, XX },
  /* 66 */  { "paddusw",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fde[4] = {
  /* -- */  { "pmaxub",     0,  Pq,  Qq, XX },
  /* 66 */  { "pmaxub",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fdf[4] = {
  /* -- */  { "pandn",      0,  Pq,  Qq, XX },
  /* 66 */  { "pandn",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe0[4] = {
  /* -- */  { "pavgb",      0,  Pq,  Qq, XX },
  /* 66 */  { "pavgb",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe1[4] = {
  /* -- */  { "psraw",      0,  Pq,  Qq, XX },
  /* 66 */  { "psraw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe2[4] = {
  /* -- */  { "psrad",      0,  Pq,  Qq, XX },
  /* 66 */  { "psrad",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe3[4] = {
  /* -- */  { "pavgw",      0,  Pq,  Qq, XX },
  /* 66 */  { "pavgw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe4[4] = {
  /* -- */  { "pmulhuw",    0,  Pq,  Qq, XX },
  /* 66 */  { "pmulhuw",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe5[4] = {
  /* -- */  { "pmulhw",     0,  Pq,  Qq, XX },
  /* 66 */  { "pmulhw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe6[4] = {
  /* -- */  { "(invalid)",  0,  XX,  XX, XX },
  /* 66 */  { "cvttpd2dq",  0,  Vq, Wpd, XX },
  /* F2 */  { "cvtpd2dq",   0,  Vq, Wpd, XX },
  /* F3 */  { "cvtdq2pd",   0, Vpd,  Wq, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe7[4] = {
  /* -- */  { "movntq",     0,  Mq,  Pq, XX },
  /* 66 */  { "movntdq",    0, Mdq, Vdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe8[4] = {
  /* -- */  { "psubsb",     0,  Pq,  Qq, XX },
  /* 66 */  { "psubsb",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fe9[4] = {
  /* -- */  { "psubsw",     0,  Pq,  Qq, XX },
  /* 66 */  { "psubsw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fea[4] = {
  /* -- */  { "pminsw",     0,  Pq,  Qq, XX },
  /* 66 */  { "pminsw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0feb[4] = {
  /* -- */  { "por",        0,  Pq,  Qq, XX },
  /* 66 */  { "por",        0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fec[4] = {
  /* -- */  { "paddsb",     0,  Pq,  Qq, XX },
  /* 66 */  { "paddsb",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fed[4] = {
  /* -- */  { "paddsw",     0,  Pq,  Qq, XX },
  /* 66 */  { "paddsw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fee[4] = {
  /* -- */  { "pmaxuw",     0,  Pq,  Qq, XX },
  /* 66 */  { "pmaxuw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0fef[4] = {
  /* -- */  { "pxor",       0,  Pq,  Qq, XX },
  /* 66 */  { "pxor",       0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff0[4] = {
  /* -- */  { "(invalid)",  0,  XX,  XX, XX },
  /* 66 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "lddqu",      0, Vdq, Mdq, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };                        

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff1[4] = {
  /* -- */  { "psllw",      0,  Pq,  Qq, XX },
  /* 66 */  { "psllw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff2[4] = {
  /* -- */  { "pslld",      0,  Pq,  Qq, XX },
  /* 66 */  { "pslld",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff3[4] = {
  /* -- */  { "psllq",      0,  Pq,  Qq, XX },
  /* 66 */  { "psllq",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff4[4] = {
  /* -- */  { "pmuludq",    0,  Pq,  Qq, XX },
  /* 66 */  { "pmuludq",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff5[4] = {
  /* -- */  { "pmaddwd",    0,  Pq,  Qq, XX },
  /* 66 */  { "pmaddwd",    0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff6[4] = {
  /* -- */  { "psadbw",     0,  Pq,  Qq, XX },
  /* 66 */  { "psadbw",     0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff7[4] = {
  /* -- */  { "maskmovq",   0,  Pq,  Qq, XX },
  /* 66 */  { "maskmovdqu", 0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff8[4] = {
  /* -- */  { "psubb",      0,  Pq,  Qq, XX },
  /* 66 */  { "psubb",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ff9[4] = {
  /* -- */  { "psubw",      0,  Pq,  Qq, XX },
  /* 66 */  { "psubw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ffa[4] = {
  /* -- */  { "psubd",      0,  Pq,  Qq, XX },
  /* 66 */  { "psubd",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ffb[4] = {
  /* -- */  { "psubq",      0,  Pq,  Qq, XX },
  /* 66 */  { "psubq",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ffc[4] = {
  /* -- */  { "paddb",      0,  Pq,  Qq, XX },
  /* 66 */  { "paddb",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ffd[4] = {
  /* -- */  { "paddw",      0,  Pq,  Qq, XX },
  /* 66 */  { "paddw",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_0ffe[4] = {
  /* -- */  { "paddd",      0,  Pq,  Qq, XX },
  /* 66 */  { "paddd",      0, Vdq, Wdq, XX },
  /* F2 */  { "(invalid)",  0,  XX,  XX, XX },
  /* F3 */  { "(invalid)",  0,  XX,  XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1202[4] = {
  /* -- */  { "psrlw",      0,  Pq, Ib, XX },
  /* 66 */  { "psrlw",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1204[4] = {
  /* -- */  { "psraw",      0,  Pq, Ib, XX },
  /* 66 */  { "psraw",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1206[4] = {
  /* -- */  { "psllw",      0,  Pq, Ib, XX },
  /* 66 */  { "psllw",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1302[4] = {
  /* -- */  { "psrld",      0,  Pq, Ib, XX },
  /* 66 */  { "psrld",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1304[4] = {
  /* -- */  { "psrad",      0,  Pq, Ib, XX },
  /* 66 */  { "psrad",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1306[4] = {
  /* -- */  { "pslld",      0,  Pq, Ib, XX },
  /* 66 */  { "pslld",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1402[4] = {
  /* -- */  { "psrlq",      0,  Pq, Ib, XX },
  /* 66 */  { "psrlq",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1403[4] = {
  /* -- */  { "(invalid)",  0,  XX, XX, XX },
  /* 66 */  { "psrldq",     0, Wdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1406[4] = {
  /* -- */  { "psllq",      0,  Pq, Ib, XX },
  /* 66 */  { "psllq",      0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupSSE_G1407[4] = {
  /* -- */  { "(invalid)",  0,  XX, XX, XX },
  /* 66 */  { "pslldq",     0, Vdq, Ib, XX },
  /* F2 */  { "(invalid)",  0,  XX, XX, XX },
  /* F3 */  { "(invalid)",  0,  XX, XX, XX }
  };


/* ************************************************************************ */
/* Opcode GroupN */

static BxDisasmOpcodeInfo_t BxDisasmGroupG1EbIb[8] = {
  /* 0 */  { "add",         0, Eb, Ib, XX },
  /* 1 */  { "or",          0, Eb, Ib, XX },
  /* 2 */  { "adc",         0, Eb, Ib, XX },
  /* 3 */  { "sbb",         0, Eb, Ib, XX },
  /* 4 */  { "and",         0, Eb, Ib, XX },
  /* 5 */  { "sub",         0, Eb, Ib, XX },
  /* 6 */  { "xor",         0, Eb, Ib, XX },
  /* 7 */  { "cmp",         0, Eb, Ib, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG1EvIv[8] = {
  /* 0 */  { "add",         0, Ev, Iv, XX },
  /* 1 */  { "or",          0, Ev, Iv, XX },
  /* 2 */  { "adc",         0, Ev, Iv, XX },
  /* 3 */  { "sbb",         0, Ev, Iv, XX },
  /* 4 */  { "and",         0, Ev, Iv, XX },
  /* 5 */  { "sub",         0, Ev, Iv, XX },
  /* 6 */  { "xor",         0, Ev, Iv, XX },
  /* 7 */  { "cmp",         0, Ev, Iv, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG1EvIb[8] = {
  /* 0 */  { "add",         0, Ev, sIb, XX },	// sign-extend byte 
  /* 1 */  { "or",          0, Ev, sIb, XX },
  /* 2 */  { "adc",         0, Ev, sIb, XX },
  /* 3 */  { "sbb",         0, Ev, sIb, XX },
  /* 4 */  { "and",         0, Ev, sIb, XX },
  /* 5 */  { "sub",         0, Ev, sIb, XX },
  /* 6 */  { "xor",         0, Ev, sIb, XX },
  /* 7 */  { "cmp",         0, Ev, sIb, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2Eb[8] = {
  /* 0 */  { "rol",         0, Eb, Ib, XX },
  /* 1 */  { "ror",         0, Eb, Ib, XX },
  /* 2 */  { "rcl",         0, Eb, Ib, XX },
  /* 3 */  { "rcr",         0, Eb, Ib, XX },
  /* 4 */  { "shl",         0, Eb, Ib, XX },
  /* 5 */  { "shr",         0, Eb, Ib, XX },
  /* 6 */  { "shl",         0, Eb, Ib, XX },
  /* 7 */  { "sar",         0, Eb, Ib, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2Eb1[8] = {
  /* 0 */  { "rol",         0, Eb, I1, XX },
  /* 1 */  { "ror",         0, Eb, I1, XX },
  /* 2 */  { "rcl",         0, Eb, I1, XX },
  /* 3 */  { "rcr",         0, Eb, I1, XX },
  /* 4 */  { "shl",         0, Eb, I1, XX },
  /* 5 */  { "shr",         0, Eb, I1, XX },
  /* 6 */  { "shl",         0, Eb, I1, XX },
  /* 7 */  { "sar",         0, Eb, I1, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2EbCL[8] = {
  /* 0 */  { "rol",         0, Eb, CL, XX },
  /* 1 */  { "ror",         0, Eb, CL, XX },
  /* 2 */  { "rcl",         0, Eb, CL, XX },
  /* 3 */  { "rcr",         0, Eb, CL, XX },
  /* 4 */  { "shl",         0, Eb, CL, XX },
  /* 5 */  { "shr",         0, Eb, CL, XX },
  /* 6 */  { "shl",         0, Eb, CL, XX },
  /* 7 */  { "sar",         0, Eb, CL, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2Ev[8] = {
  /* 0 */  { "rol",         0, Ev, Ib, XX },
  /* 1 */  { "ror",         0, Ev, Ib, XX },
  /* 2 */  { "rcl",         0, Ev, Ib, XX },
  /* 3 */  { "rcr",         0, Ev, Ib, XX },
  /* 4 */  { "shl",         0, Ev, Ib, XX },
  /* 5 */  { "shr",         0, Ev, Ib, XX },
  /* 6 */  { "shl",         0, Ev, Ib, XX },
  /* 7 */  { "sar",         0, Ev, Ib, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2Ev1[8] = {
  /* 0 */  { "rol",         0, Ev, I1, XX },
  /* 1 */  { "ror",         0, Ev, I1, XX },
  /* 2 */  { "rcl",         0, Ev, I1, XX },
  /* 3 */  { "rcr",         0, Ev, I1, XX },
  /* 4 */  { "shl",         0, Ev, I1, XX },
  /* 5 */  { "shr",         0, Ev, I1, XX },
  /* 6 */  { "shl",         0, Ev, I1, XX },
  /* 7 */  { "sar",         0, Ev, I1, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG2EvCL[8] = {
  /* 0 */  { "rol",         0, Ev, CL, XX },
  /* 1 */  { "ror",         0, Ev, CL, XX },
  /* 2 */  { "rcl",         0, Ev, CL, XX },
  /* 3 */  { "rcr",         0, Ev, CL, XX },
  /* 4 */  { "shl",         0, Ev, CL, XX },
  /* 5 */  { "shr",         0, Ev, CL, XX },
  /* 6 */  { "shl",         0, Ev, CL, XX },
  /* 7 */  { "sar",         0, Ev, CL, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG3Eb[8] = {
  /* 0 */  { "test",        0, Eb, Ib, XX },
  /* 1 */  { "test",        0, Eb, Ib, XX },
  /* 2 */  { "not",         0, Eb, XX, XX },
  /* 3 */  { "beg",         0, Eb, XX, XX },
  /* 4 */  { "mul",         0, AL, Eb, XX },
  /* 5 */  { "imul",        0, AL, Eb, XX },
  /* 6 */  { "div",         0, AL, Eb, XX },
  /* 7 */  { "idiv",        0, AL, Eb, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG3Ev[8] = {
  /* 0 */  { "test",        0,  Ev, Iv, XX },
  /* 1 */  { "test",        0,  Ev, Iv, XX },
  /* 2 */  { "not",         0,  Ev, XX, XX },
  /* 3 */  { "neg",         0,  Ev, XX, XX },    
  /* 4 */  { "mul",         0, eAX, Ev, XX },
  /* 5 */  { "imul",        0, eAX, Ev, XX },
  /* 6 */  { "div",         0, eAX, Ev, XX },
  /* 7 */  { "idiv",        0, eAX, Ev, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG4[8] = {
  /* 0 */  { "inc",         0, Eb, XX, XX },
  /* 1 */  { "dec",         0, Eb, XX, XX },
  /* 2 */  { "(invalid)",   0, XX, XX, XX },
  /* 3 */  { "(invalid)",   0, XX, XX, XX },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { "(invalid)",   0, XX, XX, XX },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG5[8] = {
  /* 0 */  { "inc",         0, Ev, XX, XX },
  /* 1 */  { "dec",         0, Ev, XX, XX },
  /* 2 */  { "call",        0, Ev, XX, XX },
  /* 3 */  { "callw",       0, Ep, XX, XX },
  /* 4 */  { "jmp",         0, Ev, XX, XX },
  /* 5 */  { "jmpw",        0, Ep, XX, XX },
  /* 6 */  { "push",        0, Ev, XX, XX },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG6[8] = {
  /* 0 */  { "sldt",        0, Ew, XX, XX },
  /* 1 */  { "str",         0, Ew, XX, XX },
  /* 2 */  { "lldt",        0, Ew, XX, XX },
  /* 3 */  { "ltr",         0, Ew, XX, XX },
  /* 4 */  { "verr",        0, Ew, XX, XX },
  /* 5 */  { "verw",        0, Ew, XX, XX },
  /* 6 */  { "(invalid)",   0, XX, XX, XX },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG7[8] = {
  /* 0 */  { "sgdt",        0, Mx, XX, XX },
  /* 1 */  { "sidt",        0, Mx, XX, XX },
  /* 2 */  { "lgdt",        0, Mx, XX, XX },
  /* 3 */  { "lidt",        0, Mx, XX, XX },
  /* 4 */  { "smsw",        0, Ew, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { "lmsw",        0, Ew, XX, XX },
  /* 7 */  { "invlpg",      0, XX, XX, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG8EvIb[8] = {
  /* 0 */  { "(invalid)",   0, XX, XX, XX },
  /* 1 */  { "(invalid)",   0, XX, XX, XX },
  /* 2 */  { "(invalid)",   0, XX, XX, XX },
  /* 3 */  { "(invalid)",   0, XX, XX, XX },
  /* 4 */  { "bt",          0, Ev, Ib, XX },
  /* 5 */  { "bts",         0, Ev, Ib, XX },
  /* 6 */  { "btr",         0, Ev, Ib, XX },
  /* 7 */  { "btc",         0, Ev, Ib, XX }
  }; 

static BxDisasmOpcodeInfo_t BxDisasmGroupG9[8] = {
  /* 0 */  { "(invalid)",   0, XX, XX, XX },
  /* 1 */  { "cmpxchg8b",   0, Ev, XX, XX },
  /* 2 */  { "(invalid)",   0, XX, XX, XX },
  /* 3 */  { "(invalid)",   0, XX, XX, XX },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { "(invalid)",   0, XX, XX, XX },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG12[8] = {
  /* 0 */  { "(invalid)",   0, XX, XX, XX },
  /* 1 */  { "(invalid)",   0, XX, XX, XX },
  /* 2 */  { GRPSSE(G1202) },
  /* 3 */  { "(invalid)",   0, XX, XX, XX },
  /* 4 */  { GRPSSE(G1204) },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { GRPSSE(G1206) },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG13[8] = {
  /* 0 */  { "(invalid)",   0, XX, XX, XX },
  /* 1 */  { "(invalid)",   0, XX, XX, XX },
  /* 2 */  { GRPSSE(G1302) },
  /* 3 */  { "(invalid)",   0, XX, XX, XX },
  /* 4 */  { GRPSSE(G1304) },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { GRPSSE(G1306) },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG14[8] = {
  /* 0 */  { "(invalid)",   0, XX, XX, XX },
  /* 1 */  { "(invalid)",   0, XX, XX, XX },
  /* 2 */  { GRPSSE(G1402) },
  /* 3 */  { GRPSSE(G1403) },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { GRPSSE(G1406) },
  /* 7 */  { GRPSSE(G1407) } 
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG15[8] = {
  /* 0 */  { "fxsave",      0, Mx, XX, XX },
  /* 1 */  { "fxrstor",     0, Mx, XX, XX },
  /* 2 */  { "ldmxcsr",     0, Md, XX, XX },
  /* 3 */  { "stmxcsr",     0, Md, XX, XX },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "lfence",      0, Mx, XX, XX },
  /* 6 */  { "mfence",      0, Mx, XX, XX },
  /* 7 */  { "sfence",      0, Mx, XX, XX }       /* SFENCE/CFLUSH */
  };

static BxDisasmOpcodeInfo_t BxDisasmGroupG16[8] = {
  /* 0 */  { "prefetchnta", 0, Mb, XX, XX },
  /* 1 */  { "prefetcht0",  0, Mb, XX, XX },
  /* 2 */  { "prefetcht1",  0, Mb, XX, XX },
  /* 3 */  { "prefetcht2",  0, Mb, XX, XX },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { "(invalid)",   0, XX, XX, XX },
  /* 7 */  { "(invalid)",   0, XX, XX, XX }
  };

/* ************************************************************************ */
/* 3DNow! opcodes */

static BxDisasmOpcodeInfo_t BxDisasm3DNowGroup[256] = {
  // 256 entries for 3DNow opcodes, by suffix
  /* 00 */  { "(invalid)",  0, XX, XX, XX },
  /* 01 */  { "(invalid)",  0, XX, XX, XX },
  /* 02 */  { "(invalid)",  0, XX, XX, XX },
  /* 03 */  { "(invalid)",  0, XX, XX, XX },
  /* 04 */  { "(invalid)",  0, XX, XX, XX },
  /* 05 */  { "(invalid)",  0, XX, XX, XX },
  /* 06 */  { "(invalid)",  0, XX, XX, XX },
  /* 07 */  { "(invalid)",  0, XX, XX, XX },
  /* 08 */  { "(invalid)",  0, XX, XX, XX },
  /* 09 */  { "(invalid)",  0, XX, XX, XX },
  /* 0A */  { "(invalid)",  0, XX, XX, XX },
  /* 0B */  { "(invalid)",  0, XX, XX, XX },
  /* 0C */  { "pi2fw",      0, Pq, Qq, XX },
  /* 0D */  { "pi2fd",      0, Pq, Qq, XX },
  /* 0E */  { "(invalid)",  0, XX, XX, XX },
  /* 0F */  { "(invalid)",  0, XX, XX, XX },
  /* 10 */  { "(invalid)",  0, XX, XX, XX },
  /* 11 */  { "(invalid)",  0, XX, XX, XX },
  /* 12 */  { "(invalid)",  0, XX, XX, XX },
  /* 13 */  { "(invalid)",  0, XX, XX, XX },
  /* 14 */  { "(invalid)",  0, XX, XX, XX },
  /* 15 */  { "(invalid)",  0, XX, XX, XX },
  /* 16 */  { "(invalid)",  0, XX, XX, XX },
  /* 17 */  { "(invalid)",  0, XX, XX, XX },
  /* 18 */  { "(invalid)",  0, XX, XX, XX },
  /* 19 */  { "(invalid)",  0, XX, XX, XX },
  /* 1A */  { "(invalid)",  0, XX, XX, XX },
  /* 1B */  { "(invalid)",  0, XX, XX, XX },
  /* 1C */  { "pf2iw",      0, Pq, Qq, XX },
  /* 1D */  { "pf2id",      0, Pq, Qq, XX },
  /* 1E */  { "(invalid)",  0, XX, XX, XX },
  /* 1F */  { "(invalid)",  0, XX, XX, XX },
  /* 20 */  { "(invalid)",  0, XX, XX, XX },
  /* 21 */  { "(invalid)",  0, XX, XX, XX },
  /* 22 */  { "(invalid)",  0, XX, XX, XX },
  /* 23 */  { "(invalid)",  0, XX, XX, XX },
  /* 24 */  { "(invalid)",  0, XX, XX, XX },
  /* 25 */  { "(invalid)",  0, XX, XX, XX },
  /* 26 */  { "(invalid)",  0, XX, XX, XX },
  /* 27 */  { "(invalid)",  0, XX, XX, XX },
  /* 28 */  { "(invalid)",  0, XX, XX, XX },
  /* 29 */  { "(invalid)",  0, XX, XX, XX },
  /* 2A */  { "(invalid)",  0, XX, XX, XX },
  /* 2B */  { "(invalid)",  0, XX, XX, XX },
  /* 2C */  { "(invalid)",  0, XX, XX, XX },
  /* 2D */  { "(invalid)",  0, XX, XX, XX },
  /* 2E */  { "(invalid)",  0, XX, XX, XX },
  /* 2F */  { "(invalid)",  0, XX, XX, XX },
  /* 30 */  { "(invalid)",  0, XX, XX, XX },
  /* 31 */  { "(invalid)",  0, XX, XX, XX },
  /* 32 */  { "(invalid)",  0, XX, XX, XX },
  /* 33 */  { "(invalid)",  0, XX, XX, XX },
  /* 34 */  { "(invalid)",  0, XX, XX, XX },
  /* 35 */  { "(invalid)",  0, XX, XX, XX },
  /* 36 */  { "(invalid)",  0, XX, XX, XX },
  /* 37 */  { "(invalid)",  0, XX, XX, XX },
  /* 38 */  { "(invalid)",  0, XX, XX, XX },
  /* 39 */  { "(invalid)",  0, XX, XX, XX },
  /* 3A */  { "(invalid)",  0, XX, XX, XX },
  /* 3B */  { "(invalid)",  0, XX, XX, XX },
  /* 3C */  { "(invalid)",  0, XX, XX, XX },
  /* 3D */  { "(invalid)",  0, XX, XX, XX },
  /* 3E */  { "(invalid)",  0, XX, XX, XX },
  /* 3F */  { "(invalid)",  0, XX, XX, XX },
  /* 40 */  { "(invalid)",  0, XX, XX, XX },
  /* 41 */  { "(invalid)",  0, XX, XX, XX },
  /* 42 */  { "(invalid)",  0, XX, XX, XX },
  /* 43 */  { "(invalid)",  0, XX, XX, XX },
  /* 44 */  { "(invalid)",  0, XX, XX, XX },
  /* 45 */  { "(invalid)",  0, XX, XX, XX },
  /* 46 */  { "(invalid)",  0, XX, XX, XX },
  /* 47 */  { "(invalid)",  0, XX, XX, XX },
  /* 48 */  { "(invalid)",  0, XX, XX, XX },
  /* 49 */  { "(invalid)",  0, XX, XX, XX },
  /* 4A */  { "(invalid)",  0, XX, XX, XX },
  /* 4B */  { "(invalid)",  0, XX, XX, XX },
  /* 4C */  { "(invalid)",  0, XX, XX, XX },
  /* 4D */  { "(invalid)",  0, XX, XX, XX },
  /* 4E */  { "(invalid)",  0, XX, XX, XX },
  /* 4F */  { "(invalid)",  0, XX, XX, XX },
  /* 50 */  { "(invalid)",  0, XX, XX, XX },
  /* 51 */  { "(invalid)",  0, XX, XX, XX },
  /* 52 */  { "(invalid)",  0, XX, XX, XX },
  /* 53 */  { "(invalid)",  0, XX, XX, XX },
  /* 54 */  { "(invalid)",  0, XX, XX, XX },
  /* 55 */  { "(invalid)",  0, XX, XX, XX },
  /* 56 */  { "(invalid)",  0, XX, XX, XX },
  /* 57 */  { "(invalid)",  0, XX, XX, XX },
  /* 58 */  { "(invalid)",  0, XX, XX, XX },
  /* 59 */  { "(invalid)",  0, XX, XX, XX },
  /* 5A */  { "(invalid)",  0, XX, XX, XX },
  /* 5B */  { "(invalid)",  0, XX, XX, XX },
  /* 5C */  { "(invalid)",  0, XX, XX, XX },
  /* 5D */  { "(invalid)",  0, XX, XX, XX },
  /* 5E */  { "(invalid)",  0, XX, XX, XX },
  /* 5F */  { "(invalid)",  0, XX, XX, XX },
  /* 60 */  { "(invalid)",  0, XX, XX, XX },
  /* 61 */  { "(invalid)",  0, XX, XX, XX },
  /* 62 */  { "(invalid)",  0, XX, XX, XX },
  /* 63 */  { "(invalid)",  0, XX, XX, XX },
  /* 64 */  { "(invalid)",  0, XX, XX, XX },
  /* 65 */  { "(invalid)",  0, XX, XX, XX },
  /* 66 */  { "(invalid)",  0, XX, XX, XX },
  /* 67 */  { "(invalid)",  0, XX, XX, XX },
  /* 68 */  { "(invalid)",  0, XX, XX, XX },
  /* 69 */  { "(invalid)",  0, XX, XX, XX },
  /* 6A */  { "(invalid)",  0, XX, XX, XX },
  /* 6B */  { "(invalid)",  0, XX, XX, XX },
  /* 6C */  { "(invalid)",  0, XX, XX, XX },
  /* 6D */  { "(invalid)",  0, XX, XX, XX },
  /* 6E */  { "(invalid)",  0, XX, XX, XX },
  /* 6F */  { "(invalid)",  0, XX, XX, XX },
  /* 70 */  { "(invalid)",  0, XX, XX, XX },
  /* 71 */  { "(invalid)",  0, XX, XX, XX },
  /* 72 */  { "(invalid)",  0, XX, XX, XX },
  /* 73 */  { "(invalid)",  0, XX, XX, XX },
  /* 74 */  { "(invalid)",  0, XX, XX, XX },
  /* 75 */  { "(invalid)",  0, XX, XX, XX },
  /* 76 */  { "(invalid)",  0, XX, XX, XX },
  /* 77 */  { "(invalid)",  0, XX, XX, XX },
  /* 78 */  { "(invalid)",  0, XX, XX, XX },
  /* 79 */  { "(invalid)",  0, XX, XX, XX },
  /* 7A */  { "(invalid)",  0, XX, XX, XX },
  /* 7B */  { "(invalid)",  0, XX, XX, XX },
  /* 7C */  { "(invalid)",  0, XX, XX, XX },
  /* 7D */  { "(invalid)",  0, XX, XX, XX },
  /* 7E */  { "(invalid)",  0, XX, XX, XX },
  /* 7F */  { "(invalid)",  0, XX, XX, XX },
  /* 80 */  { "(invalid)",  0, XX, XX, XX },
  /* 81 */  { "(invalid)",  0, XX, XX, XX },
  /* 82 */  { "(invalid)",  0, XX, XX, XX },
  /* 83 */  { "(invalid)",  0, XX, XX, XX },
  /* 84 */  { "(invalid)",  0, XX, XX, XX },
  /* 85 */  { "(invalid)",  0, XX, XX, XX },
  /* 86 */  { "(invalid)",  0, XX, XX, XX },
  /* 87 */  { "(invalid)",  0, XX, XX, XX },
  /* 88 */  { "(invalid)",  0, XX, XX, XX },
  /* 89 */  { "(invalid)",  0, XX, XX, XX },
  /* 8A */  { "pfnacc",     0, Pq, Qq, XX },
  /* 8B */  { "(invalid)",  0, XX, XX, XX },
  /* 8C */  { "(invalid)",  0, XX, XX, XX },
  /* 8D */  { "(invalid)",  0, XX, XX, XX },
  /* 8E */  { "pfpnacc",    0, Pq, Qq, XX },
  /* 8F */  { "(invalid)",  0, XX, XX, XX },
  /* 90 */  { "pfcmpge",    0, Pq, Qq, XX },
  /* 91 */  { "(invalid)",  0, XX, XX, XX },
  /* 92 */  { "(invalid)",  0, XX, XX, XX },
  /* 93 */  { "(invalid)",  0, XX, XX, XX },
  /* 94 */  { "pfmin",      0, Pq, Qq, XX },
  /* 95 */  { "(invalid)",  0, XX, XX, XX },
  /* 96 */  { "pfrcp",      0, Pq, Qq, XX },
  /* 97 */  { "pfrsqrt",    0, Pq, Qq, XX },
  /* 98 */  { "(invalid)",  0, XX, XX, XX },
  /* 99 */  { "(invalid)",  0, XX, XX, XX },
  /* 9A */  { "pfsub",      0, Pq, Qq, XX },
  /* 9B */  { "(invalid)",  0, XX, XX, XX },
  /* 9C */  { "(invalid)",  0, XX, XX, XX },
  /* 9D */  { "(invalid)",  0, XX, XX, XX },
  /* 9E */  { "pfadd",      0, Pq, Qq, XX },
  /* 9F */  { "(invalid)",  0, XX, XX, XX },
  /* A0 */  { "pfcmpgt",    0, Pq, Qq, XX },
  /* A1 */  { "(invalid)",  0, XX, XX, XX },
  /* A2 */  { "(invalid)",  0, XX, XX, XX },
  /* A3 */  { "(invalid)",  0, XX, XX, XX },
  /* A4 */  { "pfmax",      0, Pq, Qq, XX },
  /* A5 */  { "(invalid)",  0, XX, XX, XX },
  /* A6 */  { "pfrcpit1",   0, Pq, Qq, XX },
  /* A7 */  { "pfrsqit1",   0, Pq, Qq, XX },
  /* A8 */  { "(invalid)",  0, XX, XX, XX },
  /* A9 */  { "(invalid)",  0, XX, XX, XX },
  /* AA */  { "pfsubr",     0, Pq, Qq, XX },
  /* AB */  { "(invalid)",  0, XX, XX, XX },
  /* AC */  { "(invalid)",  0, XX, XX, XX },
  /* AD */  { "(invalid)",  0, XX, XX, XX },
  /* AE */  { "pfacc",      0, Pq, Qq, XX },
  /* AF */  { "(invalid)",  0, XX, XX, XX },
  /* B0 */  { "pfcmpeq",    0, Pq, Qq, XX },
  /* B1 */  { "(invalid)",  0, XX, XX, XX },
  /* B2 */  { "(invalid)",  0, XX, XX, XX },
  /* B3 */  { "(invalid)",  0, XX, XX, XX },
  /* B4 */  { "pfmul",      0, Pq, Qq, XX },
  /* B5 */  { "(invalid)",  0, XX, XX, XX },
  /* B6 */  { "pfrcpit2",   0, Pq, Qq, XX },
  /* B7 */  { "pmulhrw",    0, Pq, Qq, XX },
  /* B8 */  { "(invalid)",  0, XX, XX, XX },    
  /* B9 */  { "(invalid)",  0, XX, XX, XX },
  /* BA */  { "(invalid)",  0, XX, XX, XX },
  /* BB */  { "pswapd",     0, Pq, Qq, XX },
  /* BC */  { "(invalid)",  0, XX, XX, XX },
  /* BD */  { "(invalid)",  0, XX, XX, XX },
  /* BE */  { "(invalid)",  0, XX, XX, XX },
  /* BF */  { "pavgb",      0, Pq, Qq, XX },
  /* C0 */  { "(invalid)",  0, XX, XX, XX },
  /* C1 */  { "(invalid)",  0, XX, XX, XX },
  /* C2 */  { "(invalid)",  0, XX, XX, XX },
  /* C3 */  { "(invalid)",  0, XX, XX, XX },
  /* C4 */  { "(invalid)",  0, XX, XX, XX },
  /* C5 */  { "(invalid)",  0, XX, XX, XX },
  /* C6 */  { "(invalid)",  0, XX, XX, XX },
  /* C7 */  { "(invalid)",  0, XX, XX, XX },
  /* C8 */  { "(invalid)",  0, XX, XX, XX },
  /* C9 */  { "(invalid)",  0, XX, XX, XX },
  /* CA */  { "(invalid)",  0, XX, XX, XX },
  /* CB */  { "(invalid)",  0, XX, XX, XX },
  /* CC */  { "(invalid)",  0, XX, XX, XX },
  /* CD */  { "(invalid)",  0, XX, XX, XX },
  /* CE */  { "(invalid)",  0, XX, XX, XX },
  /* CF */  { "(invalid)",  0, XX, XX, XX },
  /* D0 */  { "(invalid)",  0, XX, XX, XX },
  /* D1 */  { "(invalid)",  0, XX, XX, XX },
  /* D2 */  { "(invalid)",  0, XX, XX, XX },
  /* D3 */  { "(invalid)",  0, XX, XX, XX },
  /* D4 */  { "(invalid)",  0, XX, XX, XX },
  /* D5 */  { "(invalid)",  0, XX, XX, XX },
  /* D6 */  { "(invalid)",  0, XX, XX, XX },
  /* D7 */  { "(invalid)",  0, XX, XX, XX },
  /* D8 */  { "(invalid)",  0, XX, XX, XX },
  /* D9 */  { "(invalid)",  0, XX, XX, XX },
  /* DA */  { "(invalid)",  0, XX, XX, XX },
  /* DB */  { "(invalid)",  0, XX, XX, XX },
  /* DC */  { "(invalid)",  0, XX, XX, XX },
  /* DD */  { "(invalid)",  0, XX, XX, XX },
  /* DE */  { "(invalid)",  0, XX, XX, XX },
  /* DF */  { "(invalid)",  0, XX, XX, XX },
  /* E0 */  { "(invalid)",  0, XX, XX, XX },
  /* E1 */  { "(invalid)",  0, XX, XX, XX },
  /* E2 */  { "(invalid)",  0, XX, XX, XX },
  /* E3 */  { "(invalid)",  0, XX, XX, XX },
  /* E4 */  { "(invalid)",  0, XX, XX, XX },
  /* E5 */  { "(invalid)",  0, XX, XX, XX },
  /* E6 */  { "(invalid)",  0, XX, XX, XX },
  /* E7 */  { "(invalid)",  0, XX, XX, XX },
  /* E8 */  { "(invalid)",  0, XX, XX, XX },
  /* E9 */  { "(invalid)",  0, XX, XX, XX },
  /* EA */  { "(invalid)",  0, XX, XX, XX },
  /* EB */  { "(invalid)",  0, XX, XX, XX },
  /* EC */  { "(invalid)",  0, XX, XX, XX },
  /* ED */  { "(invalid)",  0, XX, XX, XX },
  /* EE */  { "(invalid)",  0, XX, XX, XX },
  /* EF */  { "(invalid)",  0, XX, XX, XX },
  /* F0 */  { "(invalid)",  0, XX, XX, XX },
  /* F1 */  { "(invalid)",  0, XX, XX, XX },
  /* F2 */  { "(invalid)",  0, XX, XX, XX },
  /* F3 */  { "(invalid)",  0, XX, XX, XX },
  /* F4 */  { "(invalid)",  0, XX, XX, XX },
  /* F5 */  { "(invalid)",  0, XX, XX, XX },
  /* F6 */  { "(invalid)",  0, XX, XX, XX },
  /* F7 */  { "(invalid)",  0, XX, XX, XX },
  /* F8 */  { "(invalid)",  0, XX, XX, XX },
  /* F9 */  { "(invalid)",  0, XX, XX, XX },
  /* FA */  { "(invalid)",  0, XX, XX, XX },
  /* FB */  { "(invalid)",  0, XX, XX, XX },
  /* FC */  { "(invalid)",  0, XX, XX, XX },
  /* FD */  { "(invalid)",  0, XX, XX, XX },
  /* FE */  { "(invalid)",  0, XX, XX, XX },
  /* FF */  { "(invalid)",  0, XX, XX, XX }
};                                

/* ************************************************************************ */
/* FPU Opcodes */

// floating point instructions when mod!=11b.
// the following tables will be accessed like groups using the nnn (reg) field of
// the modrm byte. (the first byte is D8-DF)

  // D8 (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupD8[8] = { 
  /* 0 */  { "fadd",        0, Md, XX, XX },
  /* 1 */  { "fmul",        0, Md, XX, XX },
  /* 2 */  { "fcom",        0, Md, XX, XX },
  /* 3 */  { "fcomp",       0, Md, XX, XX },
  /* 4 */  { "fsub",        0, Md, XX, XX },
  /* 5 */  { "fsubr",       0, Md, XX, XX },
  /* 6 */  { "fdiv",        0, Md, XX, XX },
  /* 7 */  { "fdivpr",      0, Md, XX, XX }
  };

  // D9 (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupD9[8] = { 
  /* 0 */  { "fld",         0, Md, XX, XX },
  /* 1 */  { "(invalid)",   0, XX, XX, XX },
  /* 2 */  { "fst",         0, Md, XX, XX },
  /* 3 */  { "fstp",        0, Md, XX, XX },
  /* 4 */  { "fldenv",      0, Mx, XX, XX },
  /* 5 */  { "fldcw",       0, Ew, XX, XX },
  /* 6 */  { "fnstenv",     0, Mx, XX, XX },
  /* 7 */  { "fnstcw",      0, Mw, XX, XX }
  };

  // DA (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDA[8] = { 
  /* 0 */  { "fiadd",       0, Md, XX, XX },
  /* 1 */  { "fimul",       0, Md, XX, XX },
  /* 2 */  { "ficom",       0, Md, XX, XX },
  /* 3 */  { "ficomp",      0, Md, XX, XX },
  /* 4 */  { "fisub",       0, Md, XX, XX },
  /* 5 */  { "fisubr",      0, Md, XX, XX },
  /* 6 */  { "fidiv",       0, Md, XX, XX },
  /* 7 */  { "fidivr",      0, Md, XX, XX }
  };

  // DB (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDB[8] = { 
  /* 0 */  { "fild",        0, Md, XX, XX },
  /* 1 */  { "fisttp",      0, Md, XX, XX },
  /* 2 */  { "fist",        0, Md, XX, XX },
  /* 3 */  { "fistp",       0, Md, XX, XX },
  /* 4 */  { "(invalid)",   0, XX, XX, XX },
  /* 5 */  { "fld",         0, Mt, XX, XX },
  /* 6 */  { "(invalid)",   0, XX, XX, XX },
  /* 7 */  { "fstp",        0, Mt, XX, XX }
  };

  // DC (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDC[8] = { 
  /* 0 */  { "fadd",        0, Mq, XX, XX },
  /* 1 */  { "fmul",        0, Mq, XX, XX },
  /* 2 */  { "fcom",        0, Mq, XX, XX },
  /* 3 */  { "fcomp",       0, Mq, XX, XX },
  /* 4 */  { "fsub",        0, Mq, XX, XX },
  /* 5 */  { "fsubr",       0, Mq, XX, XX },
  /* 6 */  { "fdiv",        0, Mq, XX, XX },
  /* 7 */  { "fdivr",       0, Mq, XX, XX }
  };

  // DD (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDD[8] = { 
  /* 0 */  { "fld",         0, Mq, XX, XX },
  /* 1 */  { "fisttp",      0, Mq, XX, XX },
  /* 2 */  { "fst",         0, Mq, XX, XX },
  /* 3 */  { "fstp",        0, Mq, XX, XX },
  /* 4 */  { "frstor",      0, Mx, XX, XX },
  /* 5 */  { "(invalid)",   0, XX, XX, XX },
  /* 6 */  { "fnsave",      0, Mx, XX, XX },
  /* 7 */  { "fnstsw",      0, Mw, XX, XX }
  };

  // DE (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDE[8] = { 
  /* 0 */  { "fiadd",       0, Mw, XX, XX },
  /* 1 */  { "fimul",       0, Mw, XX, XX },
  /* 2 */  { "ficom",       0, Mw, XX, XX },
  /* 3 */  { "ficomp",      0, Mw, XX, XX },
  /* 4 */  { "fisub",       0, Mw, XX, XX },
  /* 5 */  { "fisubr",      0, Mw, XX, XX },
  /* 6 */  { "fidiv",       0, Mw, XX, XX },
  /* 7 */  { "fidivr",      0, Mw, XX, XX }
  };

  // DF (modrm is outside 00h - BFh) (mod != 11)
static BxDisasmOpcodeInfo_t BxDisasmFPGroupDF[8] = { 
  /* 0 */  { "fild",        0, Mw, XX, XX },
  /* 1 */  { "fisttp",      0, Mw, XX, XX },
  /* 2 */  { "fist",        0, Mw, XX, XX },
  /* 3 */  { "fistp",       0, Mw, XX, XX },
  /* 4 */  { "fbld",        0, Mb, XX, XX },
  /* 5 */  { "fild",        0, Mq, XX, XX },
  /* 6 */  { "fbstp",       0, Mb, XX, XX },
  /* 7 */  { "fistp",       0, Mq, XX, XX }
  };

// 512 entries for second byte of floating point instructions. (when mod==11b) 
static BxDisasmOpcodeInfo_t BxDisasmOpcodeInfoFP[512] = { 
  // D8 (modrm is outside 00h - BFh) (mod == 11)
  /* D8 C0 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C1 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C2 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C3 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C4 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C5 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C6 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C7 */  { "fadd",      0, ST0, STj, XX },
  /* D8 C8 */  { "fmul",      0, ST0, STj, XX },
  /* D8 C9 */  { "fmul",      0, ST0, STj, XX },
  /* D8 CA */  { "fmul",      0, ST0, STj, XX },
  /* D8 CB */  { "fmul",      0, ST0, STj, XX },
  /* D8 CC */  { "fmul",      0, ST0, STj, XX },
  /* D8 CD */  { "fmul",      0, ST0, STj, XX },
  /* D8 CE */  { "fmul",      0, ST0, STj, XX },
  /* D8 CF */  { "fmul",      0, ST0, STj, XX },
  /* D8 D0 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D1 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D2 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D3 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D4 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D5 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D6 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D7 */  { "fcom",      0, STj,  XX, XX },
  /* D8 D8 */  { "fcomp",     0, STj,  XX, XX },
  /* D8 D9 */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DA */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DB */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DC */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DD */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DE */  { "fcomp",     0, STj,  XX, XX },
  /* D8 DF */  { "fcomp",     0, STj,  XX, XX },
  /* D8 E0 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E1 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E2 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E3 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E4 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E5 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E6 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E7 */  { "fsub",      0, ST0, STj, XX },
  /* D8 E8 */  { "fsubr",     0, ST0, STj, XX },
  /* D8 E9 */  { "fsubr",     0, ST0, STj, XX },
  /* D8 EA */  { "fsubr",     0, ST0, STj, XX },
  /* D8 EB */  { "fsubr",     0, ST0, STj, XX },
  /* D8 EC */  { "fsubr",     0, ST0, STj, XX },
  /* D8 ED */  { "fsubr",     0, ST0, STj, XX },
  /* D8 EE */  { "fsubr",     0, ST0, STj, XX },
  /* D8 EF */  { "fsubr",     0, ST0, STj, XX },
  /* D8 F0 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F1 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F2 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F3 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F4 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F5 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F6 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F7 */  { "fdiv",      0, ST0, STj, XX },
  /* D8 F8 */  { "fsubr",     0, ST0, STj, XX },
  /* D8 F9 */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FA */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FB */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FC */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FD */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FE */  { "fsubr",     0, ST0, STj, XX },
  /* D8 FF */  { "fsubr",     0, ST0, STj, XX },
  
  // D9 (modrm is outside 00h - BFh) (mod == 11)
  /* D9 C0 */  { "fld",       0, STj,  XX, XX },
  /* D9 C1 */  { "fld",       0, STj,  XX, XX },
  /* D9 C2 */  { "fld",       0, STj,  XX, XX },
  /* D9 C3 */  { "fld",       0, STj,  XX, XX },
  /* D9 C4 */  { "fld",       0, STj,  XX, XX },
  /* D9 C5 */  { "fld",       0, STj,  XX, XX },
  /* D9 C6 */  { "fld",       0, STj,  XX, XX },
  /* D9 C7 */  { "fld",       0, STj,  XX, XX },
  /* D9 C8 */  { "fxch",      0, STj,  XX, XX },
  /* D9 C9 */  { "fxch",      0, STj,  XX, XX },
  /* D9 CA */  { "fxch",      0, STj,  XX, XX },
  /* D9 CB */  { "fxch",      0, STj,  XX, XX },
  /* D9 CC */  { "fxch",      0, STj,  XX, XX },
  /* D9 CD */  { "fxch",      0, STj,  XX, XX },
  /* D9 CE */  { "fxch",      0, STj,  XX, XX },
  /* D9 CF */  { "fxch",      0, STj,  XX, XX },
  /* D9 D0 */  { "fnop",      0,  XX,  XX, XX },
  /* D9 D1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 D9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DA */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DB */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DC */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DD */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DE */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 DF */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 E0 */  { "fchs",      0,  XX,  XX, XX },
  /* D9 E1 */  { "fabs",      0,  XX,  XX, XX },
  /* D9 E2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 E3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 E4 */  { "ftst",      0,  XX,  XX, XX },
  /* D9 E5 */  { "fxam",      0,  XX,  XX, XX },
  /* D9 E6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 E7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 E8 */  { "fld1",      0,  XX,  XX, XX },
  /* D9 E9 */  { "fldl2t",    0,  XX,  XX, XX },
  /* D9 EA */  { "fldl2e",    0,  XX,  XX, XX },
  /* D9 EB */  { "fldpi",     0,  XX,  XX, XX },
  /* D9 EC */  { "fldlg2",    0,  XX,  XX, XX },
  /* D9 ED */  { "fldln2",    0,  XX,  XX, XX },
  /* D9 EE */  { "fldz",      0,  XX,  XX, XX },
  /* D9 EF */  { "(invalid)", 0,  XX,  XX, XX },
  /* D9 F0 */  { "f2xm1",     0,  XX,  XX, XX },
  /* D9 F1 */  { "fyl2x",     0,  XX,  XX, XX },
  /* D9 F2 */  { "fptan",     0,  XX,  XX, XX },
  /* D9 F3 */  { "fpatan",    0,  XX,  XX, XX },
  /* D9 F4 */  { "fxtract",   0,  XX,  XX, XX },
  /* D9 F5 */  { "fprem1",    0,  XX,  XX, XX },
  /* D9 F6 */  { "fdecstp",   0,  XX,  XX, XX },
  /* D9 F7 */  { "fincstp",   0,  XX,  XX, XX },
  /* D9 F8 */  { "fprem",     0,  XX,  XX, XX },
  /* D9 F9 */  { "fyl2xp1",   0,  XX,  XX, XX },
  /* D9 FA */  { "fsqrt",     0,  XX,  XX, XX },
  /* D9 FB */  { "fsincos",   0,  XX,  XX, XX },
  /* D9 FC */  { "frndint",   0,  XX,  XX, XX },
  /* D9 FD */  { "fscale",    0,  XX,  XX, XX },
  /* D9 FE */  { "fsin",      0,  XX,  XX, XX },
  /* D9 FF */  { "fcos",      0,  XX,  XX, XX },
                                  
  // DA (modrm is outside 00h - BFh) (mod == 11)
  /* DA C0 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C1 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C2 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C3 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C4 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C5 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C6 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C7 */  { "fcmovb",    0, STj, STj, XX },
  /* DA C8 */  { "fcmove",    0, STj, STj, XX },
  /* DA C9 */  { "fcmove",    0, STj, STj, XX },
  /* DA CA */  { "fcmove",    0, STj, STj, XX },
  /* DA CB */  { "fcmove",    0, STj, STj, XX },
  /* DA CC */  { "fcmove",    0, STj, STj, XX },
  /* DA CD */  { "fcmove",    0, STj, STj, XX },
  /* DA CE */  { "fcmove",    0, STj, STj, XX },
  /* DA CF */  { "fcmove",    0, STj, STj, XX },
  /* DA D0 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D1 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D2 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D3 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D4 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D5 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D6 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D7 */  { "fcmovbe",   0, STj, STj, XX },
  /* DA D8 */  { "fcmovu",    0, STj, STj, XX },
  /* DA D9 */  { "fcmovu",    0, STj, STj, XX },
  /* DA DA */  { "fcmovu",    0, STj, STj, XX },
  /* DA DB */  { "fcmovu",    0, STj, STj, XX },
  /* DA DC */  { "fcmovu",    0, STj, STj, XX },
  /* DA DD */  { "fcmovu",    0, STj, STj, XX },
  /* DA DE */  { "fcmovu",    0, STj, STj, XX },
  /* DA DF */  { "fcmovu",    0, STj, STj, XX },
  /* DA E0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA E9 */  { "fucompp",   0,  XX,  XX, XX },
  /* DA EA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA EB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA EC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA ED */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA EE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA EF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA F9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DA FF */  { "(invalid)", 0,  XX,  XX, XX },

  // DB (modrm is outside 00h - BFh) (mod == 11)
  /* DB C0 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C1 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C2 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C3 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C4 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C5 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C6 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C7 */  { "fcmovnb",   0, STj, STj, XX },
  /* DB C8 */  { "fcmovne",   0, STj, STj, XX },
  /* DB C9 */  { "fcmovne",   0, STj, STj, XX },
  /* DB CA */  { "fcmovne",   0, STj, STj, XX },
  /* DB CB */  { "fcmovne",   0, STj, STj, XX },
  /* DB CC */  { "fcmovne",   0, STj, STj, XX },
  /* DB CD */  { "fcmovne",   0, STj, STj, XX },
  /* DB CE */  { "fcmovne",   0, STj, STj, XX },
  /* DB CF */  { "fcmovne",   0, STj, STj, XX },
  /* DB D0 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D1 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D2 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D3 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D4 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D5 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D6 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D7 */  { "fcmovnbe",  0, STj, STj, XX },
  /* DB D8 */  { "fcmovnu",   0, STj, STj, XX },
  /* DB D9 */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DA */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DB */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DC */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DD */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DE */  { "fcmovnu",   0, STj, STj, XX },
  /* DB DF */  { "fcmovnu",   0, STj, STj, XX },
  /* DB E0 */  { "feni   (287 only)", 0, XX, XX, XX },
  /* DB E1 */  { "fdisi  (287 only)", 0, XX, XX, XX },
  /* DB E2 */  { "fnclex",    0,  XX,  XX, XX },
  /* DB E3 */  { "fninit",    0,  XX,  XX, XX },
  /* DB E4 */  { "fsetpm (287 only)", 0, XX, XX, XX },
  /* DB E5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB E6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB E7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB E8 */  { "fucomi",    0, STj, STj, XX },
  /* DB E9 */  { "fucomi",    0, STj, STj, XX },
  /* DB EA */  { "fucomi",    0, STj, STj, XX },
  /* DB EB */  { "fucomi",    0, STj, STj, XX },
  /* DB EC */  { "fucomi",    0, STj, STj, XX },
  /* DB ED */  { "fucomi",    0, STj, STj, XX },
  /* DB EE */  { "fucomi",    0, STj, STj, XX },
  /* DB EF */  { "fucomi",    0, STj, STj, XX },
  /* DB F0 */  { "fcomi",     0, STj, STj, XX },
  /* DB F1 */  { "fcomi",     0, STj, STj, XX },
  /* DB F2 */  { "fcomi",     0, STj, STj, XX },
  /* DB F3 */  { "fcomi",     0, STj, STj, XX },
  /* DB F4 */  { "fcomi",     0, STj, STj, XX },
  /* DB F5 */  { "fcomi",     0, STj, STj, XX },
  /* DB F6 */  { "fcomi",     0, STj, STj, XX },
  /* DB F7 */  { "fcomi",     0, STj, STj, XX },
  /* DB F8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB F9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DB FF */  { "(invalid)", 0,  XX,  XX, XX },

  // DC (modrm is outside 00h - BFh) (mod == 11)
  /* DC C0 */  { "fadd",      0, STj, STj, XX },
  /* DC C1 */  { "fadd",      0, STj, STj, XX },
  /* DC C2 */  { "fadd",      0, STj, STj, XX },
  /* DC C3 */  { "fadd",      0, STj, STj, XX },
  /* DC C4 */  { "fadd",      0, STj, STj, XX },
  /* DC C5 */  { "fadd",      0, STj, STj, XX },
  /* DC C6 */  { "fadd",      0, STj, STj, XX },
  /* DC C7 */  { "fadd",      0, STj, STj, XX },
  /* DC C8 */  { "fmul",      0, STj, STj, XX },
  /* DC C9 */  { "fmul",      0, STj, STj, XX },
  /* DC CA */  { "fmul",      0, STj, STj, XX },
  /* DC CB */  { "fmul",      0, STj, STj, XX },
  /* DC CC */  { "fmul",      0, STj, STj, XX },
  /* DC CD */  { "fmul",      0, STj, STj, XX },
  /* DC CE */  { "fmul",      0, STj, STj, XX },
  /* DC CF */  { "fmul",      0, STj, STj, XX },
  /* DC D0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC D9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC DF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DC E0 */  { "fsubr",     0, STj, STj, XX },
  /* DC E1 */  { "fsubr",     0, STj, STj, XX },
  /* DC E2 */  { "fsubr",     0, STj, STj, XX },
  /* DC E3 */  { "fsubr",     0, STj, STj, XX },
  /* DC E4 */  { "fsubr",     0, STj, STj, XX },
  /* DC E5 */  { "fsubr",     0, STj, STj, XX },
  /* DC E6 */  { "fsubr",     0, STj, STj, XX },
  /* DC E7 */  { "fsubr",     0, STj, STj, XX },
  /* DC E8 */  { "fsub",      0, STj, STj, XX },
  /* DC E9 */  { "fsub",      0, STj, STj, XX },
  /* DC EA */  { "fsub",      0, STj, STj, XX },
  /* DC EB */  { "fsub",      0, STj, STj, XX },
  /* DC EC */  { "fsub",      0, STj, STj, XX },
  /* DC ED */  { "fsub",      0, STj, STj, XX },
  /* DC EE */  { "fsub",      0, STj, STj, XX },
  /* DC EF */  { "fsub",      0, STj, STj, XX },
  /* DC F0 */  { "fdivr",     0, STj, STj, XX },
  /* DC F1 */  { "fdivr",     0, STj, STj, XX },
  /* DC F2 */  { "fdivr",     0, STj, STj, XX },
  /* DC F3 */  { "fdivr",     0, STj, STj, XX },
  /* DC F4 */  { "fdivr",     0, STj, STj, XX },
  /* DC F5 */  { "fdivr",     0, STj, STj, XX },
  /* DC F6 */  { "fdivr",     0, STj, STj, XX },
  /* DC F7 */  { "fdivr",     0, STj, STj, XX },
  /* DC F8 */  { "fdiv",      0, STj, STj, XX },
  /* DC F9 */  { "fdiv",      0, STj, STj, XX },
  /* DC FA */  { "fdiv",      0, STj, STj, XX },
  /* DC FB */  { "fdiv",      0, STj, STj, XX },
  /* DC FC */  { "fdiv",      0, STj, STj, XX },
  /* DC FD */  { "fdiv",      0, STj, STj, XX },
  /* DC FE */  { "fdiv",      0, STj, STj, XX },
  /* DC FF */  { "fdiv",      0, STj, STj, XX },

  // DD (modrm is outside 00h - BFh) (mod == 11)
  /* DD C0 */  { "ffree",     0, STj,  XX, XX },
  /* DD C1 */  { "ffree",     0, STj,  XX, XX },
  /* DD C2 */  { "ffree",     0, STj,  XX, XX },
  /* DD C3 */  { "ffree",     0, STj,  XX, XX },
  /* DD C4 */  { "ffree",     0, STj,  XX, XX },
  /* DD C5 */  { "ffree",     0, STj,  XX, XX },
  /* DD C6 */  { "ffree",     0, STj,  XX, XX },
  /* DD C7 */  { "ffree",     0, STj,  XX, XX },
  /* DD C8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD C9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD CF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD D0 */  { "fst",       0, STj,  XX, XX },
  /* DD D1 */  { "fst",       0, STj,  XX, XX },
  /* DD D2 */  { "fst",       0, STj,  XX, XX },
  /* DD D3 */  { "fst",       0, STj,  XX, XX },
  /* DD D4 */  { "fst",       0, STj,  XX, XX },
  /* DD D5 */  { "fst",       0, STj,  XX, XX },
  /* DD D6 */  { "fst",       0, STj,  XX, XX },
  /* DD D7 */  { "fst",       0, STj,  XX, XX },
  /* DD D8 */  { "fstp",      0, STj,  XX, XX },
  /* DD D9 */  { "fstp",      0, STj,  XX, XX },
  /* DD DA */  { "fstp",      0, STj,  XX, XX },
  /* DD DB */  { "fstp",      0, STj,  XX, XX },
  /* DD DC */  { "fstp",      0, STj,  XX, XX },
  /* DD DD */  { "fstp",      0, STj,  XX, XX },
  /* DD DE */  { "fstp",      0, STj,  XX, XX },
  /* DD DF */  { "fstp",      0, STj,  XX, XX },
  /* DD E0 */  { "fucom",     0, STj,  XX, XX },
  /* DD E1 */  { "fucom",     0, STj,  XX, XX },
  /* DD E2 */  { "fucom",     0, STj,  XX, XX },
  /* DD E3 */  { "fucom",     0, STj,  XX, XX },
  /* DD E4 */  { "fucom",     0, STj,  XX, XX },
  /* DD E5 */  { "fucom",     0, STj,  XX, XX },
  /* DD E6 */  { "fucom",     0, STj,  XX, XX },
  /* DD E7 */  { "fucom",     0, STj,  XX, XX },
  /* DD E8 */  { "fucomp",    0, STj,  XX, XX },
  /* DD E9 */  { "fucomp",    0, STj,  XX, XX },
  /* DD EA */  { "fucomp",    0, STj,  XX, XX },
  /* DD EB */  { "fucomp",    0, STj,  XX, XX },
  /* DD EC */  { "fucomp",    0, STj,  XX, XX },
  /* DD ED */  { "fucomp",    0, STj,  XX, XX },
  /* DD EE */  { "fucomp",    0, STj,  XX, XX },
  /* DD EF */  { "fucomp",    0, STj,  XX, XX },
  /* DD F0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD F9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DD FF */  { "(invalid)", 0,  XX,  XX, XX },

  // DE (modrm is outside 00h - BFh) (mod == 11)
  /* DE C0 */  { "faddp",     0, STj, STj, XX },
  /* DE C1 */  { "faddp",     0, STj, STj, XX },
  /* DE C2 */  { "faddp",     0, STj, STj, XX },
  /* DE C3 */  { "faddp",     0, STj, STj, XX },
  /* DE C4 */  { "faddp",     0, STj, STj, XX },
  /* DE C5 */  { "faddp",     0, STj, STj, XX },
  /* DE C6 */  { "faddp",     0, STj, STj, XX },
  /* DE C7 */  { "faddp",     0, STj, STj, XX },
  /* DE C8 */  { "fmulp",     0, STj, STj, XX },
  /* DE C9 */  { "fmulp",     0, STj, STj, XX },
  /* DE CA */  { "fmulp",     0, STj, STj, XX },
  /* DE CB */  { "fmulp",     0, STj, STj, XX },
  /* DE CC */  { "fmulp",     0, STj, STj, XX },
  /* DE CD */  { "fmulp",     0, STj, STj, XX },
  /* DE CE */  { "fmulp",     0, STj, STj, XX },
  /* DE CF */  { "fmulp",     0, STj, STj, XX },
  /* DE D0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE D9 */  { "fcompp",    0,  XX,  XX, XX },
  /* DE DA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE DB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE DC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE DD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE DE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE DF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DE E0 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E1 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E2 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E3 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E4 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E5 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E6 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E7 */  { "fsubrp",    0, STj, STj, XX },
  /* DE E8 */  { "fsubp",     0, STj, STj, XX },
  /* DE E9 */  { "fsubp",     0, STj, STj, XX },
  /* DE EA */  { "fsubp",     0, STj, STj, XX },
  /* DE EB */  { "fsubp",     0, STj, STj, XX },
  /* DE EC */  { "fsubp",     0, STj, STj, XX },
  /* DE ED */  { "fsubp",     0, STj, STj, XX },
  /* DE EE */  { "fsubp",     0, STj, STj, XX },
  /* DE EF */  { "fsubp",     0, STj, STj, XX },
  /* DE F0 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F1 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F2 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F3 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F4 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F5 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F6 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F7 */  { "fdivrp",    0, STj, STj, XX },
  /* DE F8 */  { "fdivp",     0, STj, STj, XX },
  /* DE F9 */  { "fdivp",     0, STj, STj, XX },
  /* DE FA */  { "fdivp",     0, STj, STj, XX },
  /* DE FB */  { "fdivp",     0, STj, STj, XX },
  /* DE FC */  { "fdivp",     0, STj, STj, XX },
  /* DE FD */  { "fdivp",     0, STj, STj, XX },
  /* DE FE */  { "fdivp",     0, STj, STj, XX },
  /* DE FF */  { "fdivp",     0, STj, STj, XX },

  // DF (modrm is outside 00h - BFh) (mod == 11)
  /* DF C0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF C9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF CF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D0 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF D9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF DF */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E0 */  { "fnstsw",    0,  AX,  XX, XX },
  /* DF E1 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E2 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E3 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E4 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E5 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E6 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E7 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF E8 */  { "fucomip",   0, STj, STj, XX },
  /* DF E9 */  { "fucomip",   0, STj, STj, XX },
  /* DF EA */  { "fucomip",   0, STj, STj, XX },
  /* DF EB */  { "fucomip",   0, STj, STj, XX },
  /* DF EC */  { "fucomip",   0, STj, STj, XX },
  /* DF ED */  { "fucomip",   0, STj, STj, XX },
  /* DF EE */  { "fucomip",   0, STj, STj, XX },
  /* DF EF */  { "fucomip",   0, STj, STj, XX },
  /* DF F0 */  { "fcomip",    0, STj, STj, XX },
  /* DF F1 */  { "fcomip",    0, STj, STj, XX },
  /* DF F2 */  { "fcomip",    0, STj, STj, XX },
  /* DF F3 */  { "fcomip",    0, STj, STj, XX },
  /* DF F4 */  { "fcomip",    0, STj, STj, XX },
  /* DF F5 */  { "fcomip",    0, STj, STj, XX },
  /* DF F6 */  { "fcomip",    0, STj, STj, XX },
  /* DF F7 */  { "fcomip",    0, STj, STj, XX },
  /* DF F8 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF F9 */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FA */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FB */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FC */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FD */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FE */  { "(invalid)", 0,  XX,  XX, XX },
  /* DF FF */  { "(invalid)", 0,  XX,  XX, XX },
  };

static BxDisasmOpcodeInfo_t BxDisasmOpcodes[256*2] = {
  // 256 entries for single byte opcodes
  /* 00 */  { "add",       0,  Eb,  Gb, XX },
  /* 01 */  { "add",       0,  Ev,  Gv, XX },
  /* 02 */  { "add",       0,  Gb,  Eb, XX },
  /* 03 */  { "add",       0,  Gv,  Ev, XX },
  /* 04 */  { "add",       0,  AL,  Ib, XX },
  /* 05 */  { "add",       0, eAX,  Iv, XX },
  /* 06 */  { "push",      0,  ES,  XX, XX },
  /* 07 */  { "pop",       0,  ES,  XX, XX },
  /* 08 */  { "or",        0,  Eb,  Gb, XX },
  /* 09 */  { "or",        0,  Ev,  Gv, XX },
  /* 0A */  { "or",        0,  Gb,  Eb, XX },
  /* 0B */  { "or",        0,  Gv,  Ev, XX },
  /* 0C */  { "or",        0,  AL,  Ib, XX },
  /* 0D */  { "or",        0, eAX,  Iv, XX },
  /* 0E */  { "push",      0,  CS,  XX, XX },
  /* 0F */  { "(error)",   0,  XX,  XX, XX },   // 2 byte escape
  /* 10 */  { "adc",       0,  Eb,  Gb, XX },
  /* 11 */  { "adc",       0,  Ev,  Gv, XX },
  /* 12 */  { "adc",       0,  Gb,  Eb, XX },
  /* 13 */  { "adc",       0,  Gv,  Ev, XX },
  /* 14 */  { "adc",       0,  AL,  Ib, XX },
  /* 15 */  { "adc",       0, eAX,  Iv, XX },
  /* 16 */  { "push",      0,  SS,  XX, XX },
  /* 17 */  { "pop",       0,  SS,  XX, XX },
  /* 18 */  { "sbb",       0,  Eb,  Gb, XX },
  /* 19 */  { "sbb",       0,  Ev,  Gv, XX },
  /* 1A */  { "sbb",       0,  Gb,  Eb, XX },
  /* 1B */  { "sbb",       0,  Gv,  Ev, XX },
  /* 1C */  { "sbb",       0,  AL,  Ib, XX },
  /* 1D */  { "sbb",       0, eAX,  Iv, XX },
  /* 1E */  { "push",      0,  DS,  XX, XX },
  /* 1F */  { "pop",       0,  DS,  XX, XX },
  /* 20 */  { "and",       0,  Eb,  Gb, XX },
  /* 21 */  { "and",       0,  Ev,  Gv, XX },
  /* 22 */  { "and",       0,  Gb,  Eb, XX },
  /* 23 */  { "and",       0,  Gv,  Ev, XX },
  /* 24 */  { "and",       0,  AL,  Ib, XX },
  /* 25 */  { "and",       0, eAX,  Iv, XX },
  /* 26 */  { PREFIX_ES },      // ES:
  /* 27 */  { "daa",       0,  XX,  XX, XX },
  /* 20 */  { "sub",       0,  Eb,  Gb, XX },
  /* 21 */  { "sub",       0,  Ev,  Gv, XX },
  /* 22 */  { "sub",       0,  Gb,  Eb, XX },
  /* 23 */  { "sub",       0,  Gv,  Ev, XX },
  /* 24 */  { "sub",       0,  AL,  Ib, XX },
  /* 25 */  { "sub",       0, eAX,  Iv, XX },
  /* 2E */  { PREFIX_CS },      // CS:
  /* 2F */  { "das",       0,  XX,  XX, XX },
  /* 30 */  { "xor",       0,  Eb,  Gb, XX },
  /* 31 */  { "xor",       0,  Ev,  Gv, XX },
  /* 32 */  { "xor",       0,  Gb,  Eb, XX },
  /* 33 */  { "xor",       0,  Gv,  Ev, XX },
  /* 34 */  { "xor",       0,  AL,  Ib, XX },
  /* 35 */  { "xor",       0, eAX,  Iv, XX },
  /* 36 */  { PREFIX_SS },      // SS:
  /* 37 */  { "aaa",       0,  XX,  XX, XX },
  /* 30 */  { "cmp",       0,  Eb,  Gb, XX },
  /* 31 */  { "cmp",       0,  Ev,  Gv, XX },
  /* 32 */  { "cmp",       0,  Gb,  Eb, XX },
  /* 33 */  { "cmp",       0,  Gv,  Ev, XX },
  /* 34 */  { "cmp",       0,  AL,  Ib, XX },
  /* 35 */  { "cmp",       0, eAX,  Iv, XX },
  /* 3E */  { PREFIX_DS },      // DS:
  /* 3F */  { "aas",       0,  XX,  XX, XX },
  /* 40 */  { "inc",       0, eAX,  XX, XX },
  /* 41 */  { "inc",       0, eCX,  XX, XX },
  /* 42 */  { "inc",       0, eDX,  XX, XX },
  /* 43 */  { "inc",       0, eBX,  XX, XX },
  /* 44 */  { "inc",       0, eSP,  XX, XX },
  /* 45 */  { "inc",       0, eBP,  XX, XX },
  /* 46 */  { "inc",       0, eSI,  XX, XX },
  /* 47 */  { "inc",       0, eDI,  XX, XX },
  /* 48 */  { "dec",       0, eAX,  XX, XX },
  /* 49 */  { "dec",       0, eCX,  XX, XX },
  /* 4A */  { "dec",       0, eDX,  XX, XX },
  /* 4B */  { "dec",       0, eBX,  XX, XX },
  /* 4C */  { "dec",       0, eSP,  XX, XX },
  /* 4D */  { "dec",       0, eBP,  XX, XX },
  /* 4E */  { "dec",       0, eSI,  XX, XX },
  /* 4F */  { "dec",       0, eDI,  XX, XX },
  /* 50 */  { "push",      0, eAX,  XX, XX },
  /* 51 */  { "push",      0, eCX,  XX, XX },
  /* 52 */  { "push",      0, eDX,  XX, XX },
  /* 53 */  { "push",      0, eBX,  XX, XX },
  /* 54 */  { "push",      0, eSP,  XX, XX },
  /* 55 */  { "push",      0, eBP,  XX, XX },
  /* 56 */  { "push",      0, eSI,  XX, XX },
  /* 57 */  { "push",      0, eDI,  XX, XX },
  /* 58 */  { "pop",       0, eAX,  XX, XX },
  /* 59 */  { "pop",       0, eCX,  XX, XX },
  /* 5A */  { "pop",       0, eDX,  XX, XX },
  /* 5B */  { "pop",       0, eBX,  XX, XX },
  /* 5C */  { "pop",       0, eSP,  XX, XX },
  /* 5D */  { "pop",       0, eBP,  XX, XX },
  /* 5E */  { "pop",       0, eSI,  XX, XX },
  /* 5F */  { "pop",       0, eDI,  XX, XX },
  /* 60 */  { "pushad",    0,  XX,  XX, XX },
  /* 61 */  { "popad",     0,  XX,  XX, XX },
  /* 62 */  { "bound",     0,  Gv,  Ea, XX },
  /* 63 */  { "arpl",      0,  Ew,  Rw, XX },
  /* 64 */  { PREFIX_FS },      // FS:
  /* 65 */  { PREFIX_GS },      // GS:
  /* 66 */  { PREFIX_OPSIZE },  
  /* 67 */  { PREFIX_ADDRSIZE },
  /* 68 */  { "push",      0,  Iv,  XX, XX },
  /* 69 */  { "imul",      0,  Gv,  Ev, Iv },
  /* 6A */  { "push",      0, sIb,  XX, XX },   // sign extended immediate
  /* 6B */  { "imul",      0,  Gv,  Ev, Ib },   
  /* 6C */  { "insb",      0,  Yb,  DX, XX },
  /* 6D */  { "ins",       0,  Yb,  DX, XX },
  /* 6E */  { "outsb",     0,  DX,  Xb, XX },
  /* 6F */  { "outs",      0,  DX,  Xv, XX },
  /* 70 */  { "jo",        0,  Jb,  XX, XX },
  /* 71 */  { "jno",       0,  Jb,  XX, XX },
  /* 72 */  { "jb",        0,  Jb,  XX, XX },
  /* 73 */  { "jnb",       0,  Jb,  XX, XX },
  /* 74 */  { "jz",        0,  Jb,  XX, XX },
  /* 75 */  { "jnz",       0,  Jb,  XX, XX },
  /* 76 */  { "jbe",       0,  Jb,  XX, XX },
  /* 77 */  { "jnbe",      0,  Jb,  XX, XX },
  /* 78 */  { "js",        0,  Jb,  XX, XX },
  /* 79 */  { "jns",       0,  Jb,  XX, XX },
  /* 7A */  { "jp",        0,  Jb,  XX, XX },
  /* 7B */  { "jnp",       0,  Jb,  XX, XX },
  /* 7C */  { "jl",        0,  Jb,  XX, XX },
  /* 7D */  { "jnl",       0,  Jb,  XX, XX },
  /* 7E */  { "jle",       0,  Jb,  XX, XX },
  /* 7F */  { "jnle",      0,  Jb,  XX, XX },
  /* 80 */  { GRPN(G1EbIb) },
  /* 81 */  { GRPN(G1EvIv) },
  /* 82 */  { "(invalid)", 0,  XX,  XX, XX },
  /* 83 */  { GRPN(G1EvIb) },
  /* 84 */  { "test",      0,  Eb,  Gb, XX },
  /* 85 */  { "test",      0,  Ev,  Gv, XX },
  /* 86 */  { "xchg",      0,  Eb,  Gb, XX },
  /* 87 */  { "xchg",      0,  Ev,  Gv, XX },
  /* 88 */  { "mov",       0,  Gb,  Eb, XX },
  /* 89 */  { "mov",       0,  Ev,  Gv, XX },
  /* 8A */  { "mov",       0,  Gb,  Eb, XX },
  /* 8B */  { "mov",       0,  Gv,  Ev, XX },
  /* 8C */  { "mov",       0,  Ew,  Sw, XX },
  /* 8D */  { "lea",       0,  Gv,  Mv, XX },
  /* 8E */  { "mov",       0,  Sw,  Ew, XX },
  /* 8F */  { "pop",       0,  Ev,  XX, XX },
  /* 90 */  { "nop",       0,  XX,  XX, XX },
  /* 91 */  { "xchg",      0, eCX, eAX, XX },
  /* 92 */  { "xchg",      0, eDX, eAX, XX },
  /* 93 */  { "xchg",      0, eBX, eAX, XX },
  /* 94 */  { "xchg",      0, eSP, eAX, XX },
  /* 95 */  { "xchg",      0, eBP, eAX, XX },
  /* 96 */  { "xchg",      0, eSI, eAX, XX },
  /* 97 */  { "xchg",      0, eDI, eAX, XX },
  /* 98 */  { "cbw",       0,  XX,  XX, XX },
  /* 99 */  { "cwd|cdq",   0,  XX,  XX, XX },
  /* 9A */  { "call",      0,  Ap,  XX, XX },
  /* 9B */  { "wait",      0,  XX,  XX, XX },
  /* 9C */  { "pushf",     0,  XX,  XX, XX },
  /* 9D */  { "popf",      0,  XX,  XX, XX },
  /* 9E */  { "sahf",      0,  XX,  XX, XX },
  /* 9F */  { "lahf",      0,  XX,  XX, XX },
  /* A0 */  { "mov",       0,  AL,  Ob, XX },
  /* A1 */  { "mov",       0, eAX,  Ov, XX },
  /* A2 */  { "mov",       0,  Ob,  AL, XX },
  /* A3 */  { "mov",       0,  Ov,  AX, XX },
  /* A4 */  { "movsb",     0,  Yb,  Xb, XX },
  /* A5 */  { "movs",      0,  Yv,  Xv, XX },
  /* A6 */  { "cmpsb",     0,  Xb,  Yb, XX },
  /* A7 */  { "cmps",      0,  Xv,  Yv, XX },
  /* A8 */  { "test",      0,  AL,  Ib, XX },
  /* A9 */  { "test",      0, eAX,  Iv, XX },
  /* AA */  { "stosb",     0,  Yb,  AL, XX },
  /* AB */  { "stos",      0,  Yv, eAX, XX },
  /* AC */  { "lodsb",     0,  AL,  Xb, XX },
  /* AD */  { "lods",      0, eAX,  Xv, XX },
  /* AE */  { "scasb",     0,  Yb,  AL, XX },
  /* AF */  { "scas",      0,  Yv, eAX, XX },
  /* B0 */  { "mov",       0,  AL,  Ib, XX },
  /* B1 */  { "mov",       0,  CL,  Ib, XX },
  /* B2 */  { "mov",       0,  DL,  Ib, XX },
  /* B3 */  { "mov",       0,  BL,  Ib, XX },
  /* B4 */  { "mov",       0,  AH,  Ib, XX },
  /* B5 */  { "mov",       0,  CH,  Ib, XX },
  /* B6 */  { "mov",       0,  DH,  Ib, XX },
  /* B7 */  { "mov",       0,  BH,  Ib, XX },
  /* B8 */  { "mov",       0, eAX,  Iv, XX },   
  /* B9 */  { "mov",       0, eCX,  Iv, XX },
  /* BA */  { "mov",       0, eDX,  Iv, XX },
  /* BB */  { "mov",       0, eBX,  Iv, XX },
  /* BC */  { "mov",       0, eSP,  Iv, XX },
  /* BD */  { "mov",       0, eBP,  Iv, XX },
  /* BE */  { "mov",       0, eSI,  Iv, XX },
  /* BF */  { "mov",       0, eDI,  Iv, XX },
  /* C0 */  { GRPN(G2Eb) },
  /* C1 */  { GRPN(G2Ev) },
  /* C2 */  { "retn",      0,  Iw,  XX, XX },
  /* C3 */  { "retn",      0,  XX,  XX, XX },
  /* C4 */  { "les",       0,  Gv,  Mp, XX },
  /* C5 */  { "lds",       0,  Gv,  Mp, XX },
  /* C6 */  { "mov",       0,  Eb,  Ib, XX },
  /* C7 */  { "mov",       0,  Ev,  Iv, XX },
  /* C8 */  { "enter",     0,  Iw,  Ib, XX },
  /* C9 */  { "leave",     0,  XX,  XX, XX },
  /* CA */  { "retf",      0,  Iw,  XX, XX },
  /* CB */  { "retf",      0,  XX,  XX, XX },
  /* CC */  { "int3",      0,  XX,  XX, XX },
  /* CD */  { "int",       0,  Ib,  XX, XX },
  /* CE */  { "into",      0,  XX,  XX, XX },
  /* CF */  { "iret",      0,  XX,  XX, XX },
  /* D0 */  { GRPN(G2Eb1) },
  /* D1 */  { GRPN(G2Ev1) },
  /* D2 */  { GRPN(G2EbCL) },
  /* D3 */  { GRPN(G2EvCL) },
  /* D4 */  { "aam",       0, sIb,  XX, XX },
  /* D5 */  { "aad",       0, sIb,  XX, XX },
  /* D6 */  { "salc",      0,  XX,  XX, XX },
  /* D7 */  { "xlat",      0,  XX,  XX, XX },
  /* D8 */  { GRPFP(D8) },
  /* D9 */  { GRPFP(D9) },
  /* DA */  { GRPFP(DA) },
  /* DB */  { GRPFP(DB) },
  /* DC */  { GRPFP(DC) },
  /* DD */  { GRPFP(DD) },
  /* DE */  { GRPFP(DE) },
  /* DF */  { GRPFP(DF) },
  /* E0 */  { "loopne",    0,  Jb,  XX, XX },
  /* E1 */  { "loope",     0,  Jb,  XX, XX },
  /* E2 */  { "loop",      0,  Jb,  XX, XX },
  /* E3 */  { "jcxz",      0,  Jb,  XX, XX },
  /* E4 */  { "in",        0,  AL,  Ib, XX },
  /* E5 */  { "in",        0, eAX,  Ib, XX },
  /* E6 */  { "out",       0,  Ib,  AL, XX },
  /* E7 */  { "out",       0,  Ib, eAX, XX },
  /* E8 */  { "call",      0,  Jv,  XX, XX },
  /* E9 */  { "jmp",       0,  Jv,  XX, XX },
  /* EA */  { "jmp",       0,  Ap,  XX, XX },
  /* EB */  { "jmp",       0,  Jb,  XX, XX },
  /* EC */  { "in",        0,  AL,  DX, XX },
  /* ED */  { "in",        0, eAX,  DX, XX },
  /* EE */  { "out",       0,  DX,  AL, XX },
  /* EF */  { "out",       0,  DX, eAX, XX },
  /* F0 */  { PREFIX_LOCK },    // LOCK:
  /* F1 */  { "int1",      0,  XX,  XX, XX },
  /* F2 */  { PREFIX_REPNE },   // REPNE:
  /* F3 */  { PREFIX_REP },     // REP:
  /* F4 */  { "hlt",       0,  XX,  XX, XX },
  /* F5 */  { "cmc",       0,  XX,  XX, XX },
  /* F6 */  { GRPN(G3Eb) },
  /* F7 */  { GRPN(G3Ev) },
  /* F8 */  { "clc",       0,  XX,  XX, XX },
  /* F9 */  { "stc",       0,  XX,  XX, XX },
  /* FA */  { "cli",       0,  XX,  XX, XX },
  /* FB */  { "sti",       0,  XX,  XX, XX },
  /* FC */  { "cld",       0,  XX,  XX, XX },
  /* FD */  { "std",       0,  XX,  XX, XX },
  /* FE */  { GRPN(G4) },
  /* FF */  { GRPN(G5) },

  // 256 entries for two byte opcodes
  /* 0F 00 */  { GRPN(G6) },
  /* 0F 01 */  { GRPN(G7) },
  /* 0F 02 */  { "lar",       0,  Gv,  Ew, XX },
  /* 0F 03 */  { "lsl",       0,  Gv,  Ew, XX },
  /* 0F 04 */  { "(invalid)", 0,  XX,  XX, XX },
  /* 0F 05 */  { "syscall",   0,  XX,  XX, XX },
  /* 0F 06 */  { "clts",      0,  XX,  XX, XX },
  /* 0F 07 */  { "sysret",    0,  XX,  XX, XX },
  /* 0F 08 */  { "invd",      0,  XX,  XX, XX },
  /* 0F 09 */  { "wbinvd",    0,  XX,  XX, XX },
  /* 0F 0A */  { "(invalid)", 0,  XX,  XX, XX },
  /* 0F 0B */  { "ud2a",      0,  XX,  XX, XX },
  /* 0F 0C */  { "(invalid)", 0,  XX,  XX, XX },
  /* 0F 0D */  { "(invalid)", 0,  XX,  XX, XX },        // 3DNow!
  /* 0F 0E */  { "femms",     0,  XX,  XX, XX },        // 3DNow!
  /* 0F 0F */  { GRP3DNOW     },
  /* 0F 10 */  { GRPSSE(0f10) },
  /* 0F 11 */  { GRPSSE(0f11) },
  /* 0F 12 */  { GRPSSE(0f12) },
  /* 0F 13 */  { GRPSSE(0f13) },
  /* 0F 14 */  { GRPSSE(0f14) },
  /* 0F 15 */  { GRPSSE(0f15) },
  /* 0F 16 */  { GRPSSE(0f16) },
  /* 0F 17 */  { GRPSSE(0f17) },
  /* 0F 18 */  { GRPN(G16) },
  /* 0F 19 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1A */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1B */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1C */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1D */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1E */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 1F */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 20 */  { "mov",        0,  Rd,  Cd, XX },
  /* 0F 21 */  { "mov",        0,  Rd,  Dd, XX },
  /* 0F 22 */  { "mov",        0,  Cd,  Rd, XX },
  /* 0F 23 */  { "mov",        0,  Dd,  Rd, XX },
  /* 0F 24 */  { "mov",        0,  Rd,  Td, XX },
  /* 0F 25 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 26 */  { "mov",        0,  Td,  Rd, XX },
  /* 0F 27 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 28 */  { GRPSSE(0f28) },
  /* 0F 29 */  { GRPSSE(0f29) },
  /* 0F 2A */  { GRPSSE(0f2a) },
  /* 0F 2B */  { GRPSSE(0f2b) },
  /* 0F 2C */  { GRPSSE(0f2c) },
  /* 0F 2D */  { GRPSSE(0f2d) },
  /* 0F 2E */  { GRPSSE(0f2e) },
  /* 0F 2F */  { GRPSSE(0f2f) },
  /* 0F 30 */  { "wrmsr",      0,  XX,  XX, XX },
  /* 0F 31 */  { "rdtsc",      0,  XX,  XX, XX },
  /* 0F 32 */  { "rdmsr",      0,  XX,  XX, XX },
  /* 0F 33 */  { "rdpmc",      0,  XX,  XX, XX },
  /* 0F 34 */  { "sysenter",   0,  XX,  XX, XX },
  /* 0F 35 */  { "sysexit",    0,  XX,  XX, XX },
  /* 0F 36 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 37 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 38 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 39 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3A */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3B */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3C */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3D */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3E */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 3F */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 40 */  { "cmovo",      0,  Gv,  Ev, XX },
  /* 0F 41 */  { "cmovno",     0,  Gv,  Ev, XX },
  /* 0F 42 */  { "cmovc",      0,  Gv,  Ev, XX },
  /* 0F 43 */  { "cmovnc",     0,  Gv,  Ev, XX },
  /* 0F 44 */  { "cmovz",      0,  Gv,  Ev, XX },
  /* 0F 45 */  { "cmovnz",     0,  Gv,  Ev, XX },
  /* 0F 46 */  { "cmovna",     0,  Gv,  Ev, XX },
  /* 0F 47 */  { "cmova",      0,  Gv,  Ev, XX },
  /* 0F 48 */  { "cmovs",      0,  Gv,  Ev, XX },
  /* 0F 49 */  { "cmovns",     0,  Gv,  Ev, XX },
  /* 0F 4A */  { "cmovp",      0,  Gv,  Ev, XX },
  /* 0F 4B */  { "cmocnp",     0,  Gv,  Ev, XX },
  /* 0F 4C */  { "cmovl",      0,  Gv,  Ev, XX },
  /* 0F 4D */  { "cmovnl",     0,  Gv,  Ev, XX },
  /* 0F 4E */  { "cmovng",     0,  Gv,  Ev, XX },
  /* 0F 4F */  { "cmovg",      0,  Gv,  Ev, XX },
  /* 0F 50 */  { GRPSSE(0f50) },
  /* 0F 51 */  { GRPSSE(0f51) },
  /* 0F 52 */  { GRPSSE(0f52) },
  /* 0F 53 */  { GRPSSE(0f53) },
  /* 0F 54 */  { GRPSSE(0f54) },
  /* 0F 55 */  { GRPSSE(0f55) },
  /* 0F 56 */  { GRPSSE(0f56) },
  /* 0F 57 */  { GRPSSE(0f57) },
  /* 0F 58 */  { GRPSSE(0f58) },
  /* 0F 59 */  { GRPSSE(0f59) },
  /* 0F 5A */  { GRPSSE(0f5a) },
  /* 0F 5B */  { GRPSSE(0f5b) },
  /* 0F 5C */  { GRPSSE(0f5c) },
  /* 0F 5D */  { GRPSSE(0f5d) },
  /* 0F 5E */  { GRPSSE(0f5e) },
  /* 0F 5F */  { GRPSSE(0f5f) },
  /* 0F 60 */  { GRPSSE(0f60) },
  /* 0F 61 */  { GRPSSE(0f61) },
  /* 0F 62 */  { GRPSSE(0f62) },
  /* 0F 63 */  { GRPSSE(0f63) },
  /* 0F 64 */  { GRPSSE(0f64) },
  /* 0F 65 */  { GRPSSE(0f65) },
  /* 0F 66 */  { GRPSSE(0f66) },
  /* 0F 67 */  { GRPSSE(0f67) },
  /* 0F 68 */  { GRPSSE(0f68) },
  /* 0F 69 */  { GRPSSE(0f69) },
  /* 0F 6A */  { GRPSSE(0f6a) },
  /* 0F 6B */  { GRPSSE(0f6b) },
  /* 0F 6C */  { GRPSSE(0f6c) },
  /* 0F 6D */  { GRPSSE(0f6d) },
  /* 0F 6E */  { GRPSSE(0f6e) },
  /* 0F 6F */  { GRPSSE(0f6f) },
  /* 0F 70 */  { GRPSSE(0f70) },
  /* 0F 71 */  { GRPN(G12) },
  /* 0F 72 */  { GRPN(G13) },
  /* 0F 73 */  { GRPN(G14) },
  /* 0F 74 */  { GRPSSE(0f74) },
  /* 0F 75 */  { GRPSSE(0f75) },
  /* 0F 76 */  { GRPSSE(0f76) },
  /* 0F 77 */  { "emms",       0,  XX,  XX, XX },
  /* 0F 78 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 79 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 7A */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 7B */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F 7C */  { GRPSSE(0f7c) },
  /* 0F 7D */  { GRPSSE(0f7d) },
  /* 0F 7E */  { GRPSSE(0f7e) },
  /* 0F 7F */  { GRPSSE(0f7f) },
  /* 0F 80 */  { "jo",         0,  Jv,  XX, XX },
  /* 0F 81 */  { "jno",        0,  Jv,  XX, XX },
  /* 0F 82 */  { "jb",         0,  Jv,  XX, XX },
  /* 0F 83 */  { "jnb",        0,  Jv,  XX, XX },
  /* 0F 84 */  { "jz",         0,  Jv,  XX, XX },
  /* 0F 85 */  { "jnz",        0,  Jv,  XX, XX },
  /* 0F 86 */  { "jbe",        0,  Jv,  XX, XX },
  /* 0F 87 */  { "jnbe",       0,  Jv,  XX, XX },
  /* 0F 88 */  { "js",         0,  Jv,  XX, XX },
  /* 0F 89 */  { "jns",        0,  Jv,  XX, XX },
  /* 0F 8A */  { "jp",         0,  Jv,  XX, XX },
  /* 0F 8B */  { "jnp",        0,  Jv,  XX, XX },
  /* 0F 8C */  { "jl",         0,  Jv,  XX, XX },
  /* 0F 8D */  { "jnl",        0,  Jv,  XX, XX },
  /* 0F 8E */  { "jle",        0,  Jv,  XX, XX },
  /* 0F 8F */  { "jnle",       0,  Jv,  XX, XX },
  /* 0F 90 */  { "seto",       0,  Eb,  XX, XX },
  /* 0F 91 */  { "setno",      0,  Eb,  XX, XX },
  /* 0F 92 */  { "setb",       0,  Eb,  XX, XX },
  /* 0F 93 */  { "setnb",      0,  Eb,  XX, XX },
  /* 0F 94 */  { "setz",       0,  Eb,  XX, XX },
  /* 0F 95 */  { "setnz",      0,  Eb,  XX, XX },
  /* 0F 96 */  { "setbe",      0,  Eb,  XX, XX },
  /* 0F 97 */  { "setnbe",     0,  Eb,  XX, XX },
  /* 0F 98 */  { "sets",       0,  Eb,  XX, XX },
  /* 0F 99 */  { "setns",      0,  Eb,  XX, XX },
  /* 0F 9A */  { "setp",       0,  Eb,  XX, XX },
  /* 0F 9B */  { "setnp",      0,  Eb,  XX, XX },
  /* 0F 9C */  { "setl",       0,  Eb,  XX, XX },
  /* 0F 9D */  { "setnl",      0,  Eb,  XX, XX },
  /* 0F 9E */  { "setle",      0,  Eb,  XX, XX },
  /* 0F 9F */  { "setnle",     0,  Eb,  XX, XX },
  /* 0F A0 */  { "push",       0,  FS,  XX, XX },
  /* 0F A1 */  { "pop",        0,  FS,  XX, XX },
  /* 0F A2 */  { "cpuid",      0,  XX,  XX, XX },
  /* 0F A3 */  { "bt",         0,  Ev,  Gv, XX },
  /* 0F A4 */  { "shld",       0,  Ev,  Gv, Ib },
  /* 0F A5 */  { "shld",       0,  Ev,  Gv, CL },
  /* 0F A6 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F A7 */  { "(invalid)",  0,  XX,  XX, XX },
  /* 0F A8 */  { "push",       0,  GS,  XX, XX },
  /* 0F A9 */  { "pop",        0,  GS,  XX, XX },
  /* 0F AA */  { "rsm",        0,  XX,  XX, XX },
  /* 0F AB */  { "bts",        0,  Ev,  Gv, XX },
  /* 0F AC */  { "shrd",       0,  Ev,  Gv, Ib },
  /* 0F AD */  { "shrd",       0,  Ev,  Gv, CL },
  /* 0F AE */  { GRPN(G15) },
  /* 0F AF */  { "imul",       0,  Gv,  Ev, XX },
  /* 0F B0 */  { "cmpxchg",    0,  Eb,  Gb, XX },
  /* 0F B1 */  { "cmpxchg",    0,  Ev,  Gv, XX },
  /* 0F B2 */  { "lss",        0,  Mp,  XX, XX },
  /* 0F B3 */  { "btr",        0,  Ev,  Gv, XX },
  /* 0F B4 */  { "lfs",        0,  Mp,  XX, XX },
  /* 0F B5 */  { "lgs",        0,  Mp,  XX, XX },
  /* 0F B6 */  { "movzx",      0,  Gv,  Eb, XX },
  /* 0F B7 */  { "movzx",      0,  Gv,  Ew, XX },
  /* 0F B8 */  { "(invalid)",  0,  XX,  XX, XX },       
  /* 0F B9 */  { "ud2b",       0,  XX,  XX, XX },
  /* 0F BA */  { GRPN(G8EvIb) },
  /* 0F BB */  { "btc",        0,  Ev,  Gv, XX },
  /* 0F BC */  { "bsf",        0,  Gv,  Ev, XX },
  /* 0F BD */  { "bsr",        0,  Gv,  Ev, XX },
  /* 0F BE */  { "movsx",      0,  Gv,  Eb, XX },
  /* 0F BF */  { "movsx",      0,  Gv,  Ew, XX },
  /* 0F C0 */  { "xadd",       0,  Eb,  Gb, XX },
  /* 0F C1 */  { "xadd",       0,  Ev,  Gv, XX },
  /* 0F C2 */  { GRPSSE(0fc2) },
  /* 0F C3 */  { GRPSSE(0fc3) },
  /* 0F C4 */  { GRPSSE(0fc4) },
  /* 0F C5 */  { GRPSSE(0fc5) },
  /* 0F C6 */  { GRPSSE(0fc6) },
  /* 0F C7 */  { GRPN(G9) },
  /* 0F C8 */  { "bswap",      0, eAX,  XX, XX },
  /* 0F C9 */  { "bswap",      0, eCX,  XX, XX },
  /* 0F CA */  { "bswap",      0, eDX,  XX, XX },
  /* 0F CB */  { "bswap",      0, eBX,  XX, XX },
  /* 0F CC */  { "bswap",      0, eSP,  XX, XX },
  /* 0F CD */  { "bswap",      0, eBP,  XX, XX },
  /* 0F CE */  { "bswap",      0, eSI,  XX, XX },
  /* 0F CF */  { "bswap",      0, eDI,  XX, XX },
  /* 0F D0 */  { GRPSSE(0fd0) },
  /* 0F D1 */  { GRPSSE(0fd1) },
  /* 0F D2 */  { GRPSSE(0fd2) },
  /* 0F D3 */  { GRPSSE(0fd3) },
  /* 0F D4 */  { GRPSSE(0fd4) },
  /* 0F D5 */  { GRPSSE(0fd5) },
  /* 0F D6 */  { GRPSSE(0fd6) },
  /* 0F D7 */  { GRPSSE(0fd7) },
  /* 0F D8 */  { GRPSSE(0fd8) },
  /* 0F D9 */  { GRPSSE(0fd9) },
  /* 0F DA */  { GRPSSE(0fda) },
  /* 0F DB */  { GRPSSE(0fdb) },
  /* 0F DC */  { GRPSSE(0fdc) },
  /* 0F DD */  { GRPSSE(0fdd) },
  /* 0F DE */  { GRPSSE(0fde) },
  /* 0F DF */  { GRPSSE(0fdf) },
  /* 0F E0 */  { GRPSSE(0fe0) },
  /* 0F E1 */  { GRPSSE(0fe1) },
  /* 0F E2 */  { GRPSSE(0fe2) },
  /* 0F E3 */  { GRPSSE(0fe3) },
  /* 0F E4 */  { GRPSSE(0fe4) },
  /* 0F E5 */  { GRPSSE(0fe5) },
  /* 0F E6 */  { GRPSSE(0fe6) },
  /* 0F E7 */  { GRPSSE(0fe7) },
  /* 0F E8 */  { GRPSSE(0fe8) },
  /* 0F E9 */  { GRPSSE(0fe9) },
  /* 0F EA */  { GRPSSE(0fea) },
  /* 0F EB */  { GRPSSE(0feb) },
  /* 0F EC */  { GRPSSE(0fec) },
  /* 0F ED */  { GRPSSE(0fed) },
  /* 0F EE */  { GRPSSE(0fee) },
  /* 0F EF */  { GRPSSE(0fef) },
  /* 0F F0 */  { GRPSSE(0ff0) },
  /* 0F F1 */  { GRPSSE(0ff1) },
  /* 0F F2 */  { GRPSSE(0ff2) },
  /* 0F F3 */  { GRPSSE(0ff3) },
  /* 0F F4 */  { GRPSSE(0ff4) },
  /* 0F F5 */  { GRPSSE(0ff5) },
  /* 0F F6 */  { GRPSSE(0ff6) },
  /* 0F F7 */  { GRPSSE(0ff7) },
  /* 0F F8 */  { GRPSSE(0ff8) },
  /* 0F F9 */  { GRPSSE(0ff9) },
  /* 0F FA */  { GRPSSE(0ffa) },
  /* 0F FB */  { GRPSSE(0ffb) },
  /* 0F FC */  { GRPSSE(0ffc) },
  /* 0F FD */  { GRPSSE(0ffd) },
  /* 0F FE */  { GRPSSE(0ffe) },
  /* 0F FF */  { "(invalid)",  0,  XX,  XX, XX }
};

#undef XX

#endif
