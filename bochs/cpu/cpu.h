/////////////////////////////////////////////////////////////////////////
// $Id: cpu.h,v 1.88 2002-09-29 19:21:36 kevinlawton Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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


#ifndef BX_CPU_H
#  define BX_CPU_H 1

#include <setjmp.h>

#include "cpu/lazy_flags.h"

#if BX_SUPPORT_X86_64
typedef Bit64u bx_address;
#else
typedef Bit32u bx_address;
#endif


#define BX_SREG_ES    0
#define BX_SREG_CS    1
#define BX_SREG_SS    2
#define BX_SREG_DS    3
#define BX_SREG_FS    4
#define BX_SREG_GS    5

// segment register encoding
#define BX_SEG_REG_ES    0
#define BX_SEG_REG_CS    1
#define BX_SEG_REG_SS    2
#define BX_SEG_REG_DS    3
#define BX_SEG_REG_FS    4
#define BX_SEG_REG_GS    5
// NULL now has to fit in 3 bits.
#define BX_SEG_REG_NULL  7
#define BX_NULL_SEG_REG(seg) ((seg) == BX_SEG_REG_NULL)


#ifdef BX_LITTLE_ENDIAN
#define BX_REG8L_OFFSET 0
#define BX_REG8H_OFFSET 1
#define BX_REG16_OFFSET 0
#else // BX_BIG_ENDIAN
#define BX_REG8L_OFFSET 3
#define BX_REG8H_OFFSET 2
#define BX_REG16_OFFSET 2
#endif // ifdef BX_LITTLE_ENDIAN

#define BX_8BIT_REG_AL  0
#define BX_8BIT_REG_CL  1
#define BX_8BIT_REG_DL  2
#define BX_8BIT_REG_BL  3
#define BX_8BIT_REG_AH  4
#define BX_8BIT_REG_CH  5
#define BX_8BIT_REG_DH  6
#define BX_8BIT_REG_BH  7

#define BX_16BIT_REG_AX 0
#define BX_16BIT_REG_CX 1
#define BX_16BIT_REG_DX 2
#define BX_16BIT_REG_BX 3
#define BX_16BIT_REG_SP 4
#define BX_16BIT_REG_BP 5
#define BX_16BIT_REG_SI 6
#define BX_16BIT_REG_DI 7

#define BX_32BIT_REG_EAX 0
#define BX_32BIT_REG_ECX 1
#define BX_32BIT_REG_EDX 2
#define BX_32BIT_REG_EBX 3
#define BX_32BIT_REG_ESP 4
#define BX_32BIT_REG_EBP 5
#define BX_32BIT_REG_ESI 6
#define BX_32BIT_REG_EDI 7


#if defined(NEED_CPU_REG_SHORTCUTS)

/* WARNING: 
   Only BX_CPU_C member functions can use these shortcuts safely!
   Functions that use the shortcuts outside of BX_CPU_C might work 
   when BX_USE_CPU_SMF=1 but will fail when BX_USE_CPU_SMF=0
   (for example in SMP mode).
*/

// access to 8 bit general registers
#define AL (BX_CPU_THIS_PTR gen_reg[0].word.byte.rl)
#define CL (BX_CPU_THIS_PTR gen_reg[1].word.byte.rl)
#define DL (BX_CPU_THIS_PTR gen_reg[2].word.byte.rl)
#define BL (BX_CPU_THIS_PTR gen_reg[3].word.byte.rl)
#define AH (BX_CPU_THIS_PTR gen_reg[0].word.byte.rh)
#define CH (BX_CPU_THIS_PTR gen_reg[1].word.byte.rh)
#define DH (BX_CPU_THIS_PTR gen_reg[2].word.byte.rh)
#define BH (BX_CPU_THIS_PTR gen_reg[3].word.byte.rh)


// access to 16 bit general registers
#define AX (BX_CPU_THIS_PTR gen_reg[0].word.rx)
#define CX (BX_CPU_THIS_PTR gen_reg[1].word.rx)
#define DX (BX_CPU_THIS_PTR gen_reg[2].word.rx)
#define BX (BX_CPU_THIS_PTR gen_reg[3].word.rx)
#define SP (BX_CPU_THIS_PTR gen_reg[4].word.rx)
#define BP (BX_CPU_THIS_PTR gen_reg[5].word.rx)
#define SI (BX_CPU_THIS_PTR gen_reg[6].word.rx)
#define DI (BX_CPU_THIS_PTR gen_reg[7].word.rx)

// access to 16 bit instruction pointer
#define IP (* (Bit16u *) (((Bit8u *) &BX_CPU_THIS_PTR dword.eip) + BX_REG16_OFFSET))


// accesss to 32 bit general registers
#define EAX BX_CPU_THIS_PTR gen_reg[0].dword.erx
#define ECX BX_CPU_THIS_PTR gen_reg[1].dword.erx
#define EDX BX_CPU_THIS_PTR gen_reg[2].dword.erx
#define EBX BX_CPU_THIS_PTR gen_reg[3].dword.erx
#define ESP BX_CPU_THIS_PTR gen_reg[4].dword.erx
#define EBP BX_CPU_THIS_PTR gen_reg[5].dword.erx
#define ESI BX_CPU_THIS_PTR gen_reg[6].dword.erx
#define EDI BX_CPU_THIS_PTR gen_reg[7].dword.erx

#if BX_SUPPORT_X86_64
// accesss to 64 bit general registers
#define RAX BX_CPU_THIS_PTR gen_reg[0].rrx
#define RCX BX_CPU_THIS_PTR gen_reg[1].rrx
#define RDX BX_CPU_THIS_PTR gen_reg[2].rrx
#define RBX BX_CPU_THIS_PTR gen_reg[3].rrx
#define RSP BX_CPU_THIS_PTR gen_reg[4].rrx
#define RBP BX_CPU_THIS_PTR gen_reg[5].rrx
#define RSI BX_CPU_THIS_PTR gen_reg[6].rrx
#define RDI BX_CPU_THIS_PTR gen_reg[7].rrx
#define R8  BX_CPU_THIS_PTR gen_reg[8].rrx
#define R9  BX_CPU_THIS_PTR gen_reg[9].rrx
#define R10 BX_CPU_THIS_PTR gen_reg[10].rrx
#define R11 BX_CPU_THIS_PTR gen_reg[11].rrx
#define R12 BX_CPU_THIS_PTR gen_reg[12].rrx
#define R13 BX_CPU_THIS_PTR gen_reg[13].rrx
#define R14 BX_CPU_THIS_PTR gen_reg[14].rrx
#define R15 BX_CPU_THIS_PTR gen_reg[15].rrx
#endif

// access to 32 bit instruction pointer
#define EIP BX_CPU_THIS_PTR dword.eip

#if BX_SUPPORT_X86_64
// access to 64 bit instruction pointer
#define RIP BX_CPU_THIS_PTR rip

// access to 64 bit MSR registers
#define MSR_FSBASE  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base)
#define MSR_GSBASE  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base)
#define MSR_STAR    (BX_CPU_THIS_PTR msr.star)
#define MSR_LSTAR   (BX_CPU_THIS_PTR msr.lstar)
#define MSR_CSTAR   (BX_CPU_THIS_PTR msr.cstar)
#define MSR_FMASK   (BX_CPU_THIS_PTR msr.fmask)
#define MSR_KERNELGSBASE   (BX_CPU_THIS_PTR msr.kernelgsbase)

#endif

#if BX_SUPPORT_X86_64
#define BX_READ_8BIT_REGx(index,extended)  ((((index) < 4) || (extended)) ? \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl) : \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh))
#define BX_READ_16BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].dword.erx)
#define BX_READ_64BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].rrx)

#else

#define BX_READ_8BIT_REG(index)  (((index) < 4) ? \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl) : \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh))
#define BX_READ_8BIT_REGx(index,ext) BX_READ_8BIT_REG(index)
#define BX_READ_16BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].dword.erx)
#endif

#define BX_READ_16BIT_BASE_REG(var, index) {\
  var = *BX_CPU_THIS_PTR _16bit_base_reg[index];\
  }

#define BX_READ_16BIT_INDEX_REG(var, index) {\
  var = *BX_CPU_THIS_PTR _16bit_index_reg[index];\
  }

#if BX_SUPPORT_X86_64
#define BX_WRITE_8BIT_REGx(index, extended, val) {\
  if (((index) < 4) || (extended)) \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
  }
#define BX_WRITE_16BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.rx = val; \
  }
#define BX_WRITE_32BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = val; \
  }
#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = (Bit32u) val; \
  }

#define BX_WRITE_64BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = val; \
  }

#else

#define BX_WRITE_8BIT_REG(index, val) {\
  if ((index) < 4) \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
  }
#define BX_WRITE_8BIT_REGx(index, ext, val) BX_WRITE_8BIT_REG(index, val)
#define BX_WRITE_16BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.rx = val; \
  }
#define BX_WRITE_32BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = val; \
  }
// For x86-32, I just pretend this one is like the macro above,
// so common code can be used.
#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = (Bit32u) val; \
  }
#endif


#define CPU_ID (BX_CPU_THIS_PTR which_cpu())

#ifndef CPL
#define CPL  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl)
#endif


#endif  // defined(NEED_CPU_REG_SHORTCUTS)

#define BX_DE_EXCEPTION   0 // Divide Error (fault)
#define BX_DB_EXCEPTION   1 // Debug (fault/trap)
#define BX_BP_EXCEPTION   3 // Breakpoint (trap)
#define BX_OF_EXCEPTION   4 // Overflow (trap)
#define BX_BR_EXCEPTION   5 // BOUND (fault)
#define BX_UD_EXCEPTION   6
#define BX_NM_EXCEPTION   7
#define BX_DF_EXCEPTION   8
#define BX_TS_EXCEPTION  10
#define BX_NP_EXCEPTION  11
#define BX_SS_EXCEPTION  12
#define BX_GP_EXCEPTION  13
#define BX_PF_EXCEPTION  14
#define BX_MF_EXCEPTION  16
#define BX_AC_EXCEPTION  17
#define BX_MC_EXCEPTION  18
#define BX_XF_EXCEPTION  19

/* MSR registers */
#define BX_MSR_P5_MC_ADDR	0x0000
#define BX_MSR_MC_TYPE		0x0001
#define BX_MSR_TSC		0x0010
#define BX_MSR_CESR		0x0011
#define BX_MSR_CTR0		0x0012
#define BX_MSR_CTR1		0x0013
#define BX_MSR_APICBASE		0x001b
#define BX_MSR_EBL_CR_POWERON	0x002a
#define BX_MSR_TEST_CTL		0x0033
#define BX_MSR_BIOS_UPDT_TRIG	0x0079
#define BX_MSR_BBL_CR_D0	0x0088
#define BX_MSR_BBL_CR_D1	0x0089
#define BX_MSR_BBL_CR_D2	0x008a
#define BX_MSR_BBL_CR_D3	0x008b	/* = BIOS_SIGN */
#define BX_MSR_PERFCTR0		0x00c1
#define BX_MSR_PERFCTR1		0x00c2
#define BX_MSR_MTRRCAP		0x00fe
#define BX_MSR_BBL_CR_ADDR	0x0116
#define BX_MSR_BBL_DECC		0x0118
#define BX_MSR_BBL_CR_CTL	0x0119
#define BX_MSR_BBL_CR_TRIG	0x011a
#define BX_MSR_BBL_CR_BUSY	0x011b
#define BX_MSR_BBL_CR_CTL3	0x011e
#define BX_MSR_MCG_CAP		0x0179
#define BX_MSR_MCG_STATUS	0x017a
#define BX_MSR_MCG_CTL		0x017b
#define BX_MSR_EVNTSEL0		0x0186
#define BX_MSR_EVNTSEL1		0x0187
#define BX_MSR_DEBUGCTLMSR	0x01d9
#define BX_MSR_LASTBRANCHFROMIP	0x01db
#define BX_MSR_LASTBRANCHTOIP	0x01dc
#define BX_MSR_LASTINTOIP	0x01dd
#define BX_MSR_ROB_CR_BKUPTMPDR6	0x01e0
#define BX_MSR_MTRRPHYSBASE0	0x0200
#define BX_MSR_MTRRPHYSMASK0	0x0201
#define BX_MSR_MTRRPHYSBASE1	0x0202

#if BX_SUPPORT_X86_64
#define BX_MSR_EFER             0xc0000080
#define BX_MSR_STAR             0xc0000081
#define BX_MSR_LSTAR            0xc0000082
#define BX_MSR_CSTAR            0xc0000083
#define BX_MSR_FMASK            0xc0000084
#define BX_MSR_FSBASE           0xc0000100
#define BX_MSR_GSBASE           0xc0000101
#define BX_MSR_KERNELGSBASE     0xc0000102
#endif

#define BX_MODE_IA32            0x0
#define BX_MODE_LONG_COMPAT     0x1
#define BX_MODE_LONG_64         0x2

class BX_CPU_C;

#if BX_USE_CPU_SMF == 0
// normal member functions.  This can ONLY be used within BX_CPU_C classes.
// Anyone on the outside should use the BX_CPU macro (defined in bochs.h) 
// instead.
#  define BX_CPU_THIS_PTR  this->
#  define BX_CPU_THIS      this
#  define BX_SMF
#  define BX_CPU_C_PREFIX  BX_CPU_C::
// with normal member functions, calling a member fn pointer looks like
// object->*(fnptr)(arg, ...);
// Since this is different from when SMF=1, encapsulate it in a macro.
#  define BX_CPU_CALL_METHOD(func, args) \
            (this->*((BxExecutePtr_t) (func))) args
#else
// static member functions.  With SMF, there is only one CPU by definition.
#  define BX_CPU_THIS_PTR  BX_CPU(0)->
#  define BX_CPU_THIS      BX_CPU(0)
#  define BX_SMF           static
#  define BX_CPU_C_PREFIX
#  define BX_CPU_CALL_METHOD(func, args) \
            ((BxExecutePtr_t) (func)) args
#endif

#if BX_SMP_PROCESSORS==1
// single processor simulation, so there's one of everything
extern BX_CPU_C       bx_cpu;
#else
// multiprocessor simulation, we need an array of cpus and memories
extern BX_CPU_C       *bx_cpu_array[BX_SMP_PROCESSORS];
#endif



typedef struct {
  /* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0|ID|VP|VF|AC|VM|RF
   *
   * 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0|NT| IOPL|OF|DF|IF|TF|SF|ZF| 0|AF| 0|PF| 1|CF
   */
  Bit32u val32; // Raw 32-bit value in x86 bit position.  Used to store
                //   some eflags which are not cached in separate fields.
  Bit32u VM_cached;

#define DECLARE_EFLAGS_ACCESSORS()                                           \
  BX_CPP_INLINE void BX_CPU_C::setEFlags(Bit32u val);
#define IMPLEMENT_EFLAGS_ACCESSORS()                                         \
  BX_CPP_INLINE void BX_CPU_C::setEFlags(Bit32u val) {                       \
    BX_CPU_THIS_PTR eflags.val32 = val;                                      \
    BX_CPU_THIS_PTR eflags.VM_cached = val & (1<<17);                        \
    }

  // accessors for all eflags in bx_flags_reg_t
  // The macro is used once for each flag bit.
#define DECLARE_EFLAG_ACCESSOR(name,bitnum)                                  \
  BX_CPP_INLINE void    assert_##name ();                                    \
  BX_CPP_INLINE void    clear_##name ();                                     \
  BX_CPP_INLINE Boolean get_##name ();                                       \
  BX_CPP_INLINE Boolean getB_##name ();                                      \
  BX_CPP_INLINE void    set_##name (Bit8u val);

#define IMPLEMENT_EFLAG_ACCESSOR(name,bitnum)                                \
  BX_CPP_INLINE void BX_CPU_C::assert_##name () {                            \
    BX_CPU_THIS_PTR eflags.val32 |= (1<<bitnum);                             \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::clear_##name () {                             \
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<bitnum);                            \
  }                                                                          \
  BX_CPP_INLINE Boolean BX_CPU_C::getB_##name () {                           \
    return 1 & (BX_CPU_THIS_PTR eflags.val32 >> bitnum);                     \
  }                                                                          \
  BX_CPP_INLINE Boolean BX_CPU_C::get_##name () {                            \
    return BX_CPU_THIS_PTR eflags.val32 & (1 << bitnum);                     \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::set_##name (Bit8u val) {                      \
    BX_CPU_THIS_PTR eflags.val32 =                                           \
      (BX_CPU_THIS_PTR eflags.val32&~(1<<bitnum)) | ((!!val)<<bitnum);       \
  }

#define DECLARE_EFLAG_ACCESSOR_VM(bitnum)                                    \
  BX_CPP_INLINE void    assert_VM();                                         \
  BX_CPP_INLINE void    clear_VM();                                          \
  BX_CPP_INLINE Boolean get_VM();                                            \
  BX_CPP_INLINE Boolean getB_VM();                                           \
  BX_CPP_INLINE void    set_VM(Bit32u val);

#define IMPLEMENT_EFLAG_ACCESSOR_VM(bitnum)                                  \
  BX_CPP_INLINE void BX_CPU_C::assert_VM() {                                 \
    BX_CPU_THIS_PTR eflags.val32 |= (1<<bitnum);                             \
    BX_CPU_THIS_PTR eflags.VM_cached = 1;                                    \
    }                                                                        \
  BX_CPP_INLINE void BX_CPU_C::clear_VM() {                                  \
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<bitnum);                            \
    BX_CPU_THIS_PTR eflags.VM_cached = 0;                                    \
    }                                                                        \
  BX_CPP_INLINE Boolean BX_CPU_C::get_VM() {                                 \
    return BX_CPU_THIS_PTR eflags.VM_cached;                                 \
    }                                                                        \
  BX_CPP_INLINE Boolean BX_CPU_C::getB_VM() {                                \
    return (BX_CPU_THIS_PTR eflags.VM_cached>0);                             \
    }                                                                        \
  BX_CPP_INLINE void BX_CPU_C::set_VM(Bit32u val) {                          \
    BX_CPU_THIS_PTR eflags.val32 =                                           \
      (BX_CPU_THIS_PTR eflags.val32&~(1<<bitnum)) | (val ? (1<<bitnum) : 0); \
    BX_CPU_THIS_PTR eflags.VM_cached = val;                                  \
    }

#define DECLARE_EFLAG_ACCESSOR_IOPL(bitnum)                                  \
  BX_CPP_INLINE void set_IOPL(Bit32u val);                                   \
  BX_CPP_INLINE Boolean get_IOPL(void);

#define IMPLEMENT_EFLAG_ACCESSOR_IOPL(bitnum)                                \
  BX_CPP_INLINE void BX_CPU_C::set_IOPL(Bit32u val) {                        \
    BX_CPU_THIS_PTR eflags.val32 &= ~(3<<12);                                \
    BX_CPU_THIS_PTR eflags.val32 |= ((3&val) << 12);                         \
    }                                                                        \
  BX_CPP_INLINE Boolean BX_CPU_C::get_IOPL() {                               \
    return 3 & (BX_CPU_THIS_PTR eflags.val32 >> 12);                         \
    }

#define EFlagsOSZAPCMask 0x000008d5
#define EFlagsOSZAPMask  0x000008d4

  } bx_flags_reg_t;


#define DECLARE_8BIT_REGISTER_ACCESSORS(name)                                \
  BX_SMF BX_CPP_INLINE Bit8u get_##name(void);                               \
  BX_SMF BX_CPP_INLINE void  set_##name(Bit8u val);

#define DECLARE_16BIT_REGISTER_ACCESSORS(name)                               \
  BX_SMF BX_CPP_INLINE Bit16u get_##name(void);                              \
  BX_SMF BX_CPP_INLINE void   set_##name(Bit16u val);

#define DECLARE_32BIT_REGISTER_ACCESSORS(name)                               \
  BX_SMF BX_CPP_INLINE Bit32u get_##name(void);                              \
  BX_SMF BX_CPP_INLINE void   set_##name(Bit32u val);

#define IMPLEMENT_8LBIT_REGISTER_ACCESSORS(name)                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(Bit8u val) {                \
    BX_CPU_THIS_PTR gen_reg[BX_8BIT_REG_##name].word.byte.rl = val;          \
  }                                                                          \
  BX_CPP_INLINE Bit8u BX_CPU_C::get_##name(void) {                    \
    return (BX_CPU_THIS_PTR gen_reg[BX_8BIT_REG_##name].word.byte.rl);       \
  }

#define IMPLEMENT_8HBIT_REGISTER_ACCESSORS(name)                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(Bit8u val) {                       \
    BX_CPU_THIS_PTR gen_reg[BX_8BIT_REG_##name-4].word.byte.rh = val;        \
  }                                                                          \
  BX_CPP_INLINE Bit8u BX_CPU_C::get_##name(void) {                           \
    return (BX_CPU_THIS_PTR gen_reg[BX_8BIT_REG_##name-4].word.byte.rh);     \
  }                                                                          

#define IMPLEMENT_16BIT_REGISTER_ACCESSORS(name)                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(Bit16u val) {                      \
    BX_CPU_THIS_PTR gen_reg[BX_16BIT_REG_##name].word.rx = val;              \
  }                                                                          \
  BX_CPP_INLINE Bit16u BX_CPU_C::get_##name(void) {                          \
    return (BX_CPU_THIS_PTR gen_reg[BX_16BIT_REG_##name].word.rx);           \
  }                                                                          

#define IMPLEMENT_32BIT_REGISTER_ACCESSORS(name)                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name(Bit32u val) {                      \
    BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_##name].dword.erx = val;            \
  }                                                                          \
  BX_CPP_INLINE Bit32u BX_CPU_C::get_##name(void) {                          \
    return (BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_##name].dword.erx);         \
  }                                                                          

#if BX_CPU_LEVEL >= 2
typedef struct {
  Bit32u  val32; // 32bit value of register

  // bitfields broken out for efficient access
#if BX_CPU_LEVEL >= 3
  Boolean pg; // paging
#endif

// CR0 notes:
//   Each x86 level has its own quirks regarding how it handles
//   reserved bits.  I used DOS DEBUG.EXE in real mode on the
//   following processors, tried to clear bits 1..30, then tried
//   to set bits 1..30, to see how these bits are handled.
//   I found the following:
//
//   Processor    try to clear bits 1..30    try to set bits 1..30
//   386          7FFFFFF0                   7FFFFFFE
//   486DX2       00000010                   6005003E
//   Pentium      00000010                   7FFFFFFE
//   Pentium-II   00000010                   6005003E
//
// My assumptions:
//   All processors: bit 4 is hardwired to 1 (not true on all clones)
//   386: bits 5..30 of CR0 are also hardwired to 1
//   Pentium: reserved bits retain value set using mov cr0, reg32
//   486DX2/Pentium-II: reserved bits are hardwired to 0

#if BX_CPU_LEVEL >= 4
  Boolean cd; // cache disable
  Boolean nw; // no write-through
  Boolean am; // alignment mask
  Boolean wp; // write-protect
  Boolean ne; // numerics exception
#endif

  Boolean ts; // task switched
  Boolean em; // emulate math coprocessor
  Boolean mp; // monitor coprocessor
  Boolean pe; // protected mode enable
  } bx_cr0_t;
#endif

#if BX_CPU_LEVEL >= 4
typedef struct {
  Bit32u  registerValue; // 32bit value of register

  // Accessors for all cr4 bitfields.
#define IMPLEMENT_CR4_ACCESSORS(name,bitnum)                                 \
  BX_CPP_INLINE Boolean get_##name () {                                      \
    return 1 & (registerValue >> bitnum);                                    \
    }                                                                        \
  BX_CPP_INLINE void set_##name (Bit8u val) {                                \
    registerValue = (registerValue&~(1<<bitnum)) | (val ? (1<<bitnum) : 0);  \
    }
  IMPLEMENT_CR4_ACCESSORS(VME, 0);
  IMPLEMENT_CR4_ACCESSORS(PVI, 1);
  IMPLEMENT_CR4_ACCESSORS(TSD, 2);
  IMPLEMENT_CR4_ACCESSORS(DE,  3);
  IMPLEMENT_CR4_ACCESSORS(PSE, 4);
  IMPLEMENT_CR4_ACCESSORS(PAE, 5);
  IMPLEMENT_CR4_ACCESSORS(MCE, 6);
  IMPLEMENT_CR4_ACCESSORS(PGE, 7);
  IMPLEMENT_CR4_ACCESSORS(PCE, 8);
  IMPLEMENT_CR4_ACCESSORS(OSFXSR, 9);
  IMPLEMENT_CR4_ACCESSORS(OSXMMEXCPT, 10);
  BX_CPP_INLINE Boolean getRegister() { return registerValue; }
  BX_CPP_INLINE void    setRegister(Bit32u r) { registerValue = r; }
  } bx_cr4_t;
#endif  // #if BX_CPU_LEVEL >= 4

#if BX_CPU_LEVEL >= 5
typedef struct {
  Bit64u apicbase;

#if BX_SUPPORT_X86_64
  // x86-64 EFER bits
  Boolean sce;
  Boolean lme;
  Boolean lma;

  Bit64u star;
  Bit64u lstar;
  Bit64u cstar;
  Bit64u fmask;
  Bit64u kernelgsbase;
#endif

  /* TODO finish of the others */
  } bx_regs_msr_t;
#endif

typedef struct { /* bx_selector_t */
  Bit16u value;   /* the 16bit value of the selector */
#if BX_CPU_LEVEL >= 2
    /* the following fields are extracted from the value field in protected
       mode only.  They're used for sake of efficiency */
  Bit16u index;   /* 13bit index extracted from value in protected mode */
  Bit8u  ti;      /* table indicator bit extracted from value */
  Bit8u  rpl;     /* RPL extracted from value */
#endif
  } bx_selector_t;



typedef struct {

#define SegValidCache 0x1
#define SegAccessROK  0x2
#define SegAccessWOK  0x4
  Boolean valid;         // Holds above values, Or'd together.  Used to
                         // hold only 0 or 1.

  Boolean p;             /* present */
  Bit8u   dpl;           /* descriptor privilege level 0..3 */
  Boolean segment;       /* 0 = system/gate, 1 = data/code segment */
  Bit8u   type;          /* For system & gate descriptors, only
                          *  0 = invalid descriptor (reserved)
                          *  1 = 286 available Task State Segment (TSS)
                          *  2 = LDT descriptor
                          *  3 = 286 busy Task State Segment (TSS)
                          *  4 = 286 call gate
                          *  5 = task gate
                          *  6 = 286 interrupt gate
                          *  7 = 286 trap gate
                          *  8 = (reserved)
                          *  9 = 386 available TSS
                          * 10 = (reserved)
                          * 11 = 386 busy TSS
                          * 12 = 386 call gate
                          * 13 = (reserved)
                          * 14 = 386 interrupt gate
                          * 15 = 386 trap gate */
  union {
  struct {
    Boolean executable;    /* 1=code, 0=data or stack segment */
    Boolean c_ed;          /* for code: 1=conforming,
                              for data/stack: 1=expand down */
    Boolean r_w;           /* for code: readable?, for data/stack: writeable? */
    Boolean a;             /* accessed? */
    bx_address  base;      /* base address: 286=24bits, 386=32bits, long=64 */
    Bit32u  limit;         /* limit: 286=16bits, 386=20bits */
    Bit32u  limit_scaled;  /* for efficiency, this contrived field is set to
                            * limit for byte granular, and
                            * (limit << 12) | 0xfff for page granular seg's
                            */
#if BX_CPU_LEVEL >= 3
    Boolean g;             /* granularity: 0=byte, 1=4K (page) */
    Boolean d_b;           /* default size: 0=16bit, 1=32bit */
#if BX_SUPPORT_X86_64
    Boolean l;             /* long mode: 0=compat, 1=64 bit */
#endif
    Boolean avl;           /* available for use by system */
#endif
    } segment;
  struct {
    Bit8u   word_count;    /* 5bits (0..31) #words to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit16u  dest_offset;
    } gate286;
  struct {                 // type 5: Task Gate Descriptor
    Bit16u  tss_selector;  // TSS segment selector
    } taskgate;
#if BX_CPU_LEVEL >= 3
  struct {
    Bit8u   dword_count;   /* 5bits (0..31) #dwords to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit32u  dest_offset;
    } gate386;
#endif
  struct {
    Bit32u  base;          /* 24 bit 286 TSS base  */
    Bit16u  limit;         /* 16 bit 286 TSS limit */
    } tss286;
#if BX_CPU_LEVEL >= 3
  struct {
    bx_address  base;      /* 32/64 bit 386 TSS base */
    Bit32u  limit;         /* 20 bit 386 TSS limit */
    Bit32u  limit_scaled;  // Same notes as for 'segment' field
    Boolean g;             /* granularity: 0=byte, 1=4K (page) */
    Boolean avl;           /* available for use by system */
    } tss386;
#endif
  struct {
    bx_address  base;  /* 286=24 386+ =32/64 bit LDT base */
    Bit16u  limit; /* 286+ =16 bit LDT limit */
    } ldt;
    } u;

  } bx_descriptor_t;

typedef struct {
  bx_selector_t          selector;
  bx_descriptor_t  cache;
  } bx_segment_reg_t;

typedef void * (*BxVoidFPtr_t)(void);
class BX_CPU_C;


class bxInstruction_c {
public:
  // Function pointers; a function to resolve the modRM address
  // given the current state of the CPU and the instruction data,
  // and a function to execute the instruction after resolving
  // the memory address (if any).
#if BX_USE_CPU_SMF
  void (*ResolveModrm)(bxInstruction_c *);
  void (*execute)(bxInstruction_c *);
#else
  void (BX_CPU_C::*ResolveModrm)(bxInstruction_c *);
  void (BX_CPU_C::*execute)(bxInstruction_c *);
#endif

  // 26..23  ilen (0..15).  Leave this one on top so no mask is needed.
  // 22..22  mod==c0 (modrm)
  // 21..13  b1 (9bits of opcode; 1byte-op=0..255, 2byte-op=256..511.
  // 12..12  BxRepeatableZF (pass-thru from fetchdecode attributes)
  // 11..11  BxRepeatable   (pass-thru from fetchdecode attributes)
  // 10...9  repUsed (0=none, 2=0xF2, 3=0xF3).
  //  8...8  extend8bit
  //  7...7  as64
  //  6...6  os64
  //  5...5  as32
  //  4...4  os32
  //  3...3  (unused)
  //  2...0  seg
  unsigned metaInfo;

  union {
    // Form (longest case): [opcode+modrm+sib/displacement32/immediate32]
    struct {
      //  Note: if you add more bits, mask the previously upper field,
      //        in the accessor.
      //  27..20 modRM   (modrm)
      //  19..16 index           (sib)
      //  15..12 base            (sib)
      //  11...8 nnn     (modrm)
      //   7...6 mod     (modrm)
      //   5...4 scale           (sib)
      //   3...0 rm      (modrm)
      Bit32u modRMData;

      union {
        Bit32u   Id;
        Bit16u   Iw;
        Bit8u    Ib;
        };

      union {
        Bit16u displ16u; // for 16-bit modrm forms
        Bit32u displ32u; // for 32-bit modrm forms
        };
      } modRMForm;

    struct {
      Bit32u dummy;
      union {
        Bit32u   Id;
        Bit16u   Iw;
        Bit8u    Ib;
        };
      union {
        Bit32u   Id2; // Not used (for alignment)
        Bit16u   Iw2;
        Bit8u    Ib2;
        };
      } IxIxForm;

    struct {
      // For opcodes which don't use modRM, but which encode the
      // register in the low 3 bits of the opcode, extended by the
      // REX.B bit on x86-64, the register value is cached in opcodeReg.
      Bit32u opcodeReg;
      union {
        Bit32u   Id;
        Bit16u   Iw;
        Bit8u    Ib;
        };
      Bit32u dummy;
      } IxForm;

#if BX_SUPPORT_X86_64
    // Form: [opcode/Iq].  These opcode never use a modrm sequence.
    struct {
      Bit32u opcodeReg;
      Bit64u   Iq;  // for MOV Rx,imm64
      } IqForm;
#endif
    };

  BX_CPP_INLINE unsigned opcodeReg() {
    // The opcodeReg form (low 3 bits of the opcode byte (extended
    // by REX.B on x86-64) can be accessed by IxForm or IqForm.  They
    // are aligned in the same place, so it doesn't matter.
    return IxForm.opcodeReg;
    }
  BX_CPP_INLINE unsigned modrm() { return (modRMForm.modRMData>>20) & 0xff; }
  BX_CPP_INLINE unsigned mod() { return modRMForm.modRMData & 0xc0; }
  BX_CPP_INLINE unsigned modC0()
    {
    // This is a cheaper way to test for modRM instructions where
    // the mod field is 0xc0.  FetchDecode flags this condition since
    // it is quite common to be tested for.
    return metaInfo & (1<<22);
    }
  BX_CPP_INLINE unsigned nnn() {
      return (modRMForm.modRMData >> 8) & 0xf;
      }
  BX_CPP_INLINE unsigned rm()  { return modRMForm.modRMData & 0xf; }
  BX_CPP_INLINE unsigned sibScale()  {
      return (modRMForm.modRMData >> 4) & 0x3;
      }
  BX_CPP_INLINE unsigned sibIndex() {
      return (modRMForm.modRMData >> 16) & 0xf;
      }
  BX_CPP_INLINE unsigned sibBase()  {
      return (modRMForm.modRMData >> 12) & 0xf;
      }
  BX_CPP_INLINE Bit32u   displ32u() { return modRMForm.displ32u; }
  BX_CPP_INLINE Bit16u   displ16u() { return modRMForm.displ16u; }
  BX_CPP_INLINE Bit32u   Id()  { return modRMForm.Id; }
  BX_CPP_INLINE Bit16u   Iw()  { return modRMForm.Iw; }
  BX_CPP_INLINE Bit8u    Ib()  { return modRMForm.Ib; }
  BX_CPP_INLINE Bit16u   Iw2() { return IxIxForm.Iw2; } // Legacy
  BX_CPP_INLINE Bit8u    Ib2() { return IxIxForm.Ib2; } // Legacy
#if BX_SUPPORT_X86_64
  BX_CPP_INLINE Bit64u   Iq()  { return IqForm.Iq; }
#endif

  // Info in the metaInfo field.
  // Note: the 'L' at the end of certain flags, means the value returned
  // is for Logical comparisons, eg if (i->os32L() && i->as32L()).  If you
  // want a Boolean value, use os32B() etc.  This makes for smaller
  // code, when a strict 0 or 1 is not necessary.
  BX_CPP_INLINE void initMetaInfo(unsigned seg,
                                  unsigned os32, unsigned as32,
                                  unsigned os64, unsigned as64,
                                  unsigned extend8bit, unsigned repUsed) {
      metaInfo = seg | (os32<<4) | (as32<<5) |
                 (os64<<6) | (as64<<7) | (extend8bit<<8) | (repUsed<<9);
      }
  BX_CPP_INLINE unsigned seg(void) {
      return metaInfo & 7;
      }
  BX_CPP_INLINE void setSeg(unsigned val) {
      metaInfo = (metaInfo & ~7) | val;
      }

  BX_CPP_INLINE unsigned os32L(void) {
      return metaInfo & (1<<4);
      }
  BX_CPP_INLINE unsigned os32B(void) {
      return (metaInfo >> 4) & 1;
      }
  BX_CPP_INLINE void setOs32B(unsigned bit) {
      metaInfo = (metaInfo & ~(1<<4)) | (bit<<4);
      }
  BX_CPP_INLINE void assertOs32(void) {
      metaInfo |= (1<<4);
      }

  BX_CPP_INLINE unsigned as32L(void) {
      return metaInfo & (1<<5);
      }
  BX_CPP_INLINE unsigned as32B(void) {
      return (metaInfo >> 5) & 1;
      }
  BX_CPP_INLINE void setAs32B(unsigned bit) {
      metaInfo = (metaInfo & ~(1<<5)) | (bit<<5);
      }

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned os64L(void) {
      return metaInfo & (1<<6);
      }
  BX_CPP_INLINE void setOs64B(unsigned bit) {
      metaInfo = (metaInfo & ~(1<<6)) | (bit<<6);
      }
  BX_CPP_INLINE void assertOs64(void) {
      metaInfo |= (1<<6);
      }
#else
  BX_CPP_INLINE unsigned os64L(void) { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned as64L(void) {
      return metaInfo & (1<<7);
      }
  BX_CPP_INLINE void setAs64B(unsigned bit) {
      metaInfo = (metaInfo & ~(1<<7)) | (bit<<7);
      }
#else
  BX_CPP_INLINE unsigned as64L(void) { return 0; }
#endif

#if BX_SUPPORT_X86_64
  BX_CPP_INLINE unsigned extend8bitL(void) {
      return metaInfo & (1<<8);
      }
  BX_CPP_INLINE void assertExtend8bit(void) {
      metaInfo |= (1<<8);
      }
#endif

  BX_CPP_INLINE unsigned repUsedL(void) {
      return metaInfo & (3<<9);
      }
  BX_CPP_INLINE unsigned repUsedValue(void) {
      return (metaInfo >> 9) & 3;
      }
  BX_CPP_INLINE void setRepUsed(unsigned value) {
      metaInfo = (metaInfo & ~(3<<9)) | (value<<9);
      }

  BX_CPP_INLINE void setRepAttr(unsigned value) {
      // value is expected to be masked, and only contain bits
      // for BxRepeatable and BxRepeatableZF.  We don't need to
      // keep masking out these bits before we add in new ones,
      // since the fetch process won't start with repeatable attributes
      // and then delete them.
      metaInfo |= value;
      }
  BX_CPP_INLINE unsigned repeatableL(void) {
      return metaInfo & (1<<11);
      }
  BX_CPP_INLINE unsigned repeatableZFL(void) {
      return metaInfo & (1<<12);
      }

  BX_CPP_INLINE unsigned b1(void) {
      return (metaInfo >> 13) & 0x1ff;
      }
  BX_CPP_INLINE void setB1(unsigned b1) {
      metaInfo = (metaInfo & ~(0x1ff<<13)) | (b1<<13);
      }

  // Note this is the highest field, and thus needs no masking.
  // DON'T PUT ANY FIELDS HIGHER THAN THIS ONE WITHOUT ADDING A MASK.
  BX_CPP_INLINE unsigned ilen(void) {
      return metaInfo >> 23;
      }
  BX_CPP_INLINE void setILen(unsigned ilen) {
      metaInfo |= (ilen<<23);
      }
  };


#if BX_USE_CPU_SMF
typedef void (*BxExecutePtr_t)(bxInstruction_c *);
#else
typedef void (BX_CPU_C::*BxExecutePtr_t)(bxInstruction_c *);
#endif


// ========== iCache =============================================

#if BX_SupportICache

#define BxICacheEntries (32 * 1024)  // Must be a power of 2.
  // bit31: 1=CS is 32/64-bit, 0=CS is 16-bit.
  // bit30: 1=Long Mode, 0=not Long Mode.
  // bit29: 1=iCache page, 0=Data.
#define ICacheWriteStampInvalid   0x1fffffff
#define ICacheWriteStampMax       0x1fffffff // Decrements from here.
#define ICacheWriteStampMask      0x1fffffff

class bxICacheEntry_c {
  public:

  Bit32u pAddr;       // Physical address of the instruction.
  Bit32u writeStamp;  // Generation ID.  Each write to a physical page
                      // decrements this value.
  bxInstruction_c i;  // The instruction decode information.
  };

class bxICache_c {
  public:

  bxICacheEntry_c entry[BxICacheEntries];

  // A table (dynamically allocated) to store write-stamp
  // generation IDs.  Each time a write occurs to a physical page,
  // a generation ID is decremented.  Only iCache entries which have
  // write stamps matching the physical page write stamp are valid.
  Bit32u *pageWriteStampTable; // Allocated later.

  Bit32u  fetchModeMask;

  bxICache_c::bxICache_c() {
    // Initially clear the iCache;
    memset(this, 0, sizeof(*this));
    pageWriteStampTable = NULL;
    for (unsigned i=0; i<BxICacheEntries; i++) {
      entry[i].writeStamp = ICacheWriteStampInvalid;
      }
    }

  BX_CPP_INLINE void alloc(unsigned memSizeInBytes) {
    pageWriteStampTable =
        (Bit32u*) malloc(sizeof(Bit32u) * (memSizeInBytes>>12));
    for (unsigned i=0; i<(memSizeInBytes>>12); i++) {
      pageWriteStampTable[i] = ICacheWriteStampInvalid;
      }
    }

  BX_CPP_INLINE void decWriteStamp(BX_CPU_C *cpu, Bit32u a20Addr);

  BX_CPP_INLINE void clear(void) {
    memset(this, 0, sizeof(*this));
    }
  BX_CPP_INLINE unsigned hash(Bit32u pAddr) {
    // A pretty dumb hash function for now.
    return pAddr & (BxICacheEntries-1);
    }
  BX_CPP_INLINE Bit32u createFetchModeMask(BX_CPU_C *cpu);
  };
#endif
// ===============================================================


#if BX_CPU_LEVEL < 2
  /* no GDTR or IDTR register in an 8086 */
#else
typedef struct {
  bx_address             base;      /* base address: 24bits=286,32bits=386,64bits=x86-64 */
  Bit16u                 limit;     /* limit, 16bits */
  } bx_global_segment_reg_t;
#endif




#if BX_USE_TLB
  typedef struct {
    bx_address lpf; // linear page frame
    Bit32u ppf; // physical page frame
    Bit32u accessBits; // Page Table Address for updating A & D bits
    Bit32u hostPageAddr;
    } bx_TLB_entry;
#endif  // #if BX_USE_TLB


#if BX_SUPPORT_X86_64

#ifdef BX_BIG_ENDIAN
 typedef struct {
   union {
     struct {
       Bit32u dword_filler;
       Bit16u word_filler;
       union {
         Bit16u rx;
         struct {
           Bit8u rh;
           Bit8u rl;
           } byte;
         };
       } word;
     Bit64u rrx;
     struct {
       Bit32u hrx;  // hi 32 bits
       Bit32u erx;  // low 32 bits
       } dword;
     };
   } bx_gen_reg_t;
#else

 typedef struct {
   union {
     struct {
       union {
         Bit16u rx;
         struct {
           Bit8u rl;
           Bit8u rh;
           } byte;
         };
       Bit16u word_filler;
       Bit32u dword_filler;
       } word;
     Bit64u rrx;
     struct {
       Bit32u erx;  // low 32 bits
       Bit32u hrx;  // hi 32 bits
       } dword;
     };
   } bx_gen_reg_t;

#endif

#else  // #if BX_SUPPORT_X86_64

#ifdef BX_BIG_ENDIAN
typedef struct {
  union {
    struct {
      Bit32u erx;
      } dword;
    struct {
      Bit16u word_filler;
      union {
        Bit16u rx;
        struct {
          Bit8u rh;
          Bit8u rl;
          } byte;
        };
      } word;
    };
  } bx_gen_reg_t;

#else

typedef struct {
  union {
    struct {
      Bit32u erx;
      } dword;
    struct {
      union {
        Bit16u rx;
        struct {
          Bit8u rl;
          Bit8u rh;
          } byte;
        };
      Bit16u word_filler;
      } word;
    };
  } bx_gen_reg_t;
#endif

#endif  // #if BX_SUPPORT_X86_64

typedef enum {
  APIC_TYPE_NONE,
  APIC_TYPE_IOAPIC,
  APIC_TYPE_LOCAL_APIC
} bx_apic_type_t;

#define APIC_BASE_ADDR	0xfee00000	// default APIC address

#if BX_SUPPORT_APIC
class bx_generic_apic_c : public logfunctions {
protected:
  Bit32u base_addr;
  Bit8u id;
#define APIC_UNKNOWN_ID 0xff
#define APIC_VERSION_ID 0x00170011  // same version as 82093 IOAPIC
public:
  bx_generic_apic_c ();
  virtual ~bx_generic_apic_c ();
  virtual void init ();
  virtual void hwreset () { }
  Bit32u get_base (void) { return base_addr; }
  void set_base (Bit32u newbase);
  void set_id (Bit8u newid);
  Bit8u get_id () { return id; }
  virtual char *get_name();
  Boolean is_selected (Bit32u addr, Bit32u len);
  void read (Bit32u addr, void *data, unsigned len);
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void write(Bit32u address, Bit32u *value, unsigned len);
  virtual void startup_msg (Bit32u vector);
  // on local APIC, trigger means deliver to the CPU.
  // on I/O APIC, trigger means direct to another APIC according to table.
  virtual void trigger_irq (unsigned num, unsigned from);
  virtual void untrigger_irq (unsigned num, unsigned from);
  virtual Bit32u get_delivery_bitmask (Bit8u dest, Bit8u dest_mode);
  virtual Boolean deliver (Bit8u destination, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode);
  virtual Boolean match_logical_addr (Bit8u address);
  virtual bx_apic_type_t get_type ();
  virtual void set_arb_id (int newid);  // only implemented on local apics
};

class bx_local_apic_c : public bx_generic_apic_c {
#define BX_LOCAL_APIC_MAX_INTS 256
  // TMR=trigger mode register.  Cleared for edge-triggered interrupts
  // and set for level-triggered interrupts.  If set, local APIC must send
  // EOI message to all other APICs.  EOI's are not implemented.
  Bit8u tmr[BX_LOCAL_APIC_MAX_INTS];
  // IRR=interrupt request register.  When an interrupt is triggered by
  // the I/O APIC or another processor, it sets a bit in irr.  The bit is
  // cleared when the interrupt is acknowledged by the processor.
  Bit8u irr[BX_LOCAL_APIC_MAX_INTS];
  // ISR=in-service register.  When an IRR bit is cleared, the corresponding
  // bit in ISR is set.  The ISR bit is cleared when 
  Bit8u isr[BX_LOCAL_APIC_MAX_INTS];
  Bit32u arb_id, arb_priority, task_priority, log_dest, dest_format, spurious_vec;
  Bit32u lvt[6];
#define APIC_LVT_TIMER   0
#define APIC_LVT_THERMAL 1
#define APIC_LVT_PERFORM 2
#define APIC_LVT_LINT0   3
#define APIC_LVT_LINT1   4
#define APIC_LVT_ERROR   5
  Bit32u timer_initial, timer_current, timer_divconf;
  Boolean timer_active;  // internal state, not accessible from bus
  Bit32u timer_divide_counter, timer_divide_factor;
  Bit32u icr_high, icr_low;
  Bit32u err_status;
#define APIC_ERR_ILLEGAL_ADDR    0x80
#define APIC_ERR_RX_ILLEGAL_VEC  0x40
#define APIC_ERR_TX_ILLEGAL_VEC  0x20
#define APIC_ERR_RX_ACCEPT_ERR   0x08
#define APIC_ERR_TX_ACCEPT_ERR   0x04
#define APIC_ERR_RX_CHECKSUM     0x02
#define APIC_ERR_TX_CHECKSUM     0x01
public:
  bx_local_apic_c(BX_CPU_C *mycpu);
  virtual ~bx_local_apic_c(void);
  BX_CPU_C *cpu;
  virtual void hwreset ();
  virtual void init ();
  BX_CPU_C *get_cpu (Bit8u id);
  void set_id (Bit8u newid);   // redefine to set cpu->name
  virtual char *get_name();
  virtual void write (Bit32u addr, Bit32u *data, unsigned len);
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void startup_msg (Bit32u vector);
  // on local APIC, trigger means raise the CPU's INTR line.  For now
  // I also have to raise pc_system.INTR but that should be replaced
  // with the cpu-specific INTR signals.
  virtual void trigger_irq (unsigned num, unsigned from);
  virtual void untrigger_irq (unsigned num, unsigned from);
  Bit8u acknowledge_int ();  // only the local CPU should call this
  int highest_priority_int (Bit8u *array);
  void service_local_apic ();
  void print_status ();
  virtual Boolean match_logical_addr (Bit8u address);
  virtual Boolean is_local_apic () { return true; }
  virtual bx_apic_type_t get_type () { return APIC_TYPE_LOCAL_APIC; }
  virtual Bit32u get_delivery_bitmask (Bit8u dest, Bit8u dest_mode);
  virtual Boolean deliver (Bit8u destination, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode);
  Bit8u get_ppr ();
  Bit8u get_apr ();
  void periodic (Bit32u usec_delta);
  void set_divide_configuration (Bit32u value);
  virtual void update_msr_apicbase(Bit32u newaddr);
  virtual void set_arb_id (int newid);
  };

#define APIC_MAX_ID 16
extern bx_generic_apic_c *apic_index[APIC_MAX_ID];
#endif // if BX_SUPPORT_APIC


typedef void (*BxDTShim_t)(void);

class BX_MEM_C;

#include "cpu/i387.h"

class BX_CPU_C : public logfunctions {

public: // for now...

  char name[64];

  // General register set
  // eax: accumulator
  // ebx: base
  // ecx: count
  // edx: data
  // ebp: base pointer
  // esi: source index
  // edi: destination index
  // esp: stack pointer
#if BX_SUPPORT_X86_64
  bx_gen_reg_t  gen_reg[16];

union {
#ifdef BX_BIG_ENDIAN
  struct {
    Bit32u rip_upper;
    Bit32u eip;
    } dword;
#else
  struct {
    Bit32u eip;
    Bit32u rip_upper;
    } dword;
#endif
  Bit64u rip;
  };

#else

  bx_gen_reg_t  gen_reg[8];

  union {
    Bit32u eip;    // instruction pointer
    } dword;
#endif

#if BX_CPU_LEVEL > 0
  // so that we can back up when handling faults, exceptions, etc.
  // we need to store the value of the instruction pointer, before
  // each fetch/execute cycle.
  bx_address prev_eip;
#endif
  // A few pointer to functions for use by the dynamic translation
  // code.  Keep them close to the gen_reg declaration, so I can
  // use an 8bit offset to access them.

#if BX_DYNAMIC_TRANSLATION
  BxDTShim_t DTWrite8vShim;
  BxDTShim_t DTWrite16vShim;
  BxDTShim_t DTWrite32vShim;
  BxDTShim_t DTRead8vShim;
  BxDTShim_t DTRead16vShim;
  BxDTShim_t DTRead32vShim;
  BxDTShim_t DTReadRMW8vShim;
  BxDTShim_t DTReadRMW16vShim;
  BxDTShim_t DTReadRMW32vShim;
  BxDTShim_t DTWriteRMW8vShim;
  BxDTShim_t DTWriteRMW16vShim;
  BxDTShim_t DTWriteRMW32vShim;
  BxDTShim_t DTSetFlagsOSZAPCPtr;
  BxDTShim_t DTIndBrHandler;
  BxDTShim_t DTDirBrHandler;
#endif

  // status and control flags register set
  Bit32u   lf_flags_status;
  bx_flags_reg_t eflags;

  bx_lf_flags_entry oszapc;
  bx_lf_flags_entry oszap;

  bx_address prev_esp;

#define BX_INHIBIT_INTERRUPTS 0x01
#define BX_INHIBIT_DEBUG      0x02
  // What events to inhibit at any given time.  Certain instructions
  // inhibit interrupts, some debug exceptions and single-step traps.
  unsigned inhibit_mask;

  /* user segment register set */
  bx_segment_reg_t  sregs[6];

  /* system segment registers */
#if BX_CPU_LEVEL >= 2
  bx_global_segment_reg_t gdtr; /* global descriptor table register */
  bx_global_segment_reg_t idtr; /* interrupt descriptor table register */
#endif
  bx_segment_reg_t        ldtr; /* interrupt descriptor table register */
  bx_segment_reg_t        tr;   /* task register */


  /* debug registers 0-7 (unimplemented) */
#if BX_CPU_LEVEL >= 3
  Bit32u dr0;
  Bit32u dr1;
  Bit32u dr2;
  Bit32u dr3;
  Bit32u dr6;
  Bit32u dr7;
#endif

  /* TR3 - TR7 (Test Register 3-7), unimplemented */

  /* Control registers */
#if BX_CPU_LEVEL >= 2
  bx_cr0_t  cr0;
  Bit32u    cr1;
  bx_address    cr2;
  bx_address    cr3;
#endif

#if BX_CPU_LEVEL >= 4
  bx_cr4_t cr4;
#endif

#if BX_CPU_LEVEL >= 5
  bx_regs_msr_t	msr;  
#endif

  i387_t the_i387;

  // pointer to the address space that this processor uses.
  BX_MEM_C *mem;

  Boolean EXT; /* 1 if processing external interrupt or exception
                * or if not related to current instruction,
                * 0 if current CS:IP caused exception */
  unsigned errorno;   /* signal exception during instruction emulation */

  Bit32u   debug_trap; // holds DR6 value to be set as well
  volatile Boolean async_event;
  volatile Boolean INTR;
  volatile Boolean kill_bochs_request;

  /* wether this CPU is the BSP always set for UP */
  Boolean bsp;
  // for accessing registers by index number
  Bit16u *_16bit_base_reg[8];
  Bit16u *_16bit_index_reg[8];
  Bit32u empty_register;

  // for decoding instructions; accessing seg reg's by index
  unsigned sreg_mod00_rm16[8];
  unsigned sreg_mod01_rm16[8];
  unsigned sreg_mod10_rm16[8];
#if BX_SUPPORT_X86_64
  unsigned sreg_mod01_rm32[16];
  unsigned sreg_mod10_rm32[16];
  unsigned sreg_mod0_base32[16];
  unsigned sreg_mod1or2_base32[16];
#else
  unsigned sreg_mod01_rm32[8];
  unsigned sreg_mod10_rm32[8];
  unsigned sreg_mod0_base32[8];
  unsigned sreg_mod1or2_base32[8];
#endif

  // for exceptions
  jmp_buf jmp_buf_env;
  Bit8u curr_exception[2];

  static const Boolean is_exception_OK[3][3];

  bx_segment_reg_t save_cs;
  bx_segment_reg_t save_ss;
  Bit32u           save_eip;
  Bit32u           save_esp;


  // Boundaries of current page, based on EIP
  bx_address eipPageBias;
  bx_address eipPageWindowSize;
  Bit8u     *eipFetchPtr;
  Bit32u     pAddrA20Page; // Guest physical address of current instruction
                           // page with A20() already applied.

#if BX_SUPPORT_X86_64
  // for x86-64  (MODE_IA32,MODE_LONG,MODE_64)
  unsigned cpu_mode;
#else
  // x86-32 is always in IA32 mode.
  enum { cpu_mode = BX_MODE_IA32 };
#endif

#if BX_DEBUGGER
  Bit32u watchpoint;
  Bit8u break_point;
#ifdef MAGIC_BREAKPOINT
  Bit8u magic_break;
#endif
  Bit8u stop_reason;
  Bit8u trace;
  Bit8u trace_reg;
  Bit8u mode_break;		/* BW */
  Boolean debug_vm;		/* BW contains current mode*/
  Bit8u show_eip;		/* BW record eip at special instr f.ex eip */
  Bit8u show_flag;		/* BW shows instr class executed */
  bx_guard_found_t guard_found;
#endif

#if BX_SUPPORT_X86_64
#define TLB_GENERATION_MAX (BX_TLB_SIZE-1)
#endif

  // for paging
#if BX_USE_TLB
  struct {
    bx_TLB_entry entry[BX_TLB_SIZE]  BX_CPP_AlignN(16);

#if BX_USE_QUICK_TLB_INVALIDATE
#  define BX_TLB_LPF_VALUE(lpf) (lpf | BX_CPU_THIS_PTR TLB.tlb_invalidate)
    Bit32u tlb_invalidate;
#else
#  define BX_TLB_LPF_VALUE(lpf) (lpf)
#endif
    } TLB;
#endif  // #if BX_USE_TLB


  // An instruction cache.  Each entry should be exactly 32 bytes, and
  // this structure should be aligned on a 32-byte boundary to be friendly
  // with the host cache lines.
#if BX_SupportICache
  bxICache_c iCache  BX_CPP_AlignN(32);
#endif


  struct {
    bx_address  rm_addr; // The address offset after resolution.
    Bit32u  paddress1;  // physical address after translation of 1st len1 bytes of data
    Bit32u  paddress2;  // physical address after translation of 2nd len2 bytes of data
    Bit32u  len1;       // Number of bytes in page 1
    Bit32u  len2;       // Number of bytes in page 2
    Bit32u  pages;      // Number of pages access spans (1 or 2).  Also used
                        //   for the case when a native host pointer is
                        //   available for the R-M-W instructions.  The host
                        //   pointer is stuffed here.  Since this field has
                        //   to be checked anyways (and thus cached), if it
                        //   is greated than 2 (the maximum possible for
                        //   normal cases) it is a native pointer and is used
                        //   for a direct write access.
    } address_xlation;

#if BX_SUPPORT_X86_64
  // data upper 32 bits - not used any longer
  //Bit32s daddr_upper;    // upper bits must be canonical  (-virtmax --> + virtmax)
  // instruction upper 32 bits - not used any longer
  //Bit32s iaddr_upper;    // upper bits must be canonical  (-virtmax --> + virtmax)

  void ask (int level, const char *prefix, const char *fmt, va_list ap);
#endif

#define ArithmeticalFlag(flag, lfMaskShift, eflagsBitShift) \
  BX_SMF Boolean get_##flag##Lazy(void); \
  BX_SMF Boolean getB_##flag(void) { \
    if ( (BX_CPU_THIS_PTR lf_flags_status & (0xf<<lfMaskShift)) == \
         ((Bit32u) (BX_LF_INDEX_KNOWN<<lfMaskShift)) ) \
      return (BX_CPU_THIS_PTR eflags.val32 >> eflagsBitShift) & 1; \
    else \
      return get_##flag##Lazy(); \
    } \
  BX_SMF Boolean get_##flag(void) { \
    if ( (BX_CPU_THIS_PTR lf_flags_status & (0xf<<lfMaskShift)) == \
         ((Bit32u) (BX_LF_INDEX_KNOWN<<lfMaskShift)) ) \
      return BX_CPU_THIS_PTR eflags.val32 & (1<<eflagsBitShift); \
    else \
      return get_##flag##Lazy(); \
    }

  ArithmeticalFlag(OF, 20, 11);
  ArithmeticalFlag(SF, 16,  7);
  ArithmeticalFlag(ZF, 12,  6);
  ArithmeticalFlag(AF,  8,  4);
  ArithmeticalFlag(PF,  4,  2);
  ArithmeticalFlag(CF,  0,  0);


  // constructors & destructors...
  BX_CPU_C();
  ~BX_CPU_C(void);
  void init (BX_MEM_C *addrspace);

  // prototypes for CPU instructions...
  BX_SMF void ADD_EbGb(bxInstruction_c *);
  BX_SMF void ADD_EdGd(bxInstruction_c *);
  BX_SMF void ADD_GbEb(bxInstruction_c *);

  BX_SMF void ADD_GdEEd(bxInstruction_c *);
  BX_SMF void ADD_GdEGd(bxInstruction_c *);

  BX_SMF void ADD_ALIb(bxInstruction_c *);
  BX_SMF void ADD_EAXId(bxInstruction_c *);
  BX_SMF void OR_EbGb(bxInstruction_c *);
  BX_SMF void OR_EdGd(bxInstruction_c *);
  BX_SMF void OR_EwGw(bxInstruction_c *);
  BX_SMF void OR_GbEb(bxInstruction_c *);
  BX_SMF void OR_GdEd(bxInstruction_c *);
  BX_SMF void OR_GwEw(bxInstruction_c *);
  BX_SMF void OR_ALIb(bxInstruction_c *);
  BX_SMF void OR_EAXId(bxInstruction_c *);
  BX_SMF void OR_AXIw(bxInstruction_c *);

  BX_SMF void PUSH_CS(bxInstruction_c *);
  BX_SMF void PUSH_DS(bxInstruction_c *);
  BX_SMF void POP_DS(bxInstruction_c *);
  BX_SMF void PUSH_ES(bxInstruction_c *);
  BX_SMF void POP_ES(bxInstruction_c *);
  BX_SMF void PUSH_FS(bxInstruction_c *);
  BX_SMF void POP_FS(bxInstruction_c *);
  BX_SMF void PUSH_GS(bxInstruction_c *);
  BX_SMF void POP_GS(bxInstruction_c *);
  BX_SMF void PUSH_SS(bxInstruction_c *);
  BX_SMF void POP_SS(bxInstruction_c *);

  BX_SMF void ADC_EbGb(bxInstruction_c *);
  BX_SMF void ADC_EdGd(bxInstruction_c *);
  BX_SMF void ADC_GbEb(bxInstruction_c *);
  BX_SMF void ADC_GdEd(bxInstruction_c *);
  BX_SMF void ADC_ALIb(bxInstruction_c *);
  BX_SMF void ADC_EAXId(bxInstruction_c *);
  BX_SMF void SBB_EbGb(bxInstruction_c *);
  BX_SMF void SBB_EdGd(bxInstruction_c *);
  BX_SMF void SBB_GbEb(bxInstruction_c *);
  BX_SMF void SBB_GdEd(bxInstruction_c *);
  BX_SMF void SBB_ALIb(bxInstruction_c *);
  BX_SMF void SBB_EAXId(bxInstruction_c *);

  BX_SMF void AND_EbGb(bxInstruction_c *);
  BX_SMF void AND_EdGd(bxInstruction_c *);
  BX_SMF void AND_EwGw(bxInstruction_c *);
  BX_SMF void AND_GbEb(bxInstruction_c *);
  BX_SMF void AND_GdEd(bxInstruction_c *);
  BX_SMF void AND_GwEw(bxInstruction_c *);
  BX_SMF void AND_ALIb(bxInstruction_c *);
  BX_SMF void AND_EAXId(bxInstruction_c *);
  BX_SMF void AND_AXIw(bxInstruction_c *);
  BX_SMF void DAA(bxInstruction_c *);
  BX_SMF void SUB_EbGb(bxInstruction_c *);
  BX_SMF void SUB_EdGd(bxInstruction_c *);
  BX_SMF void SUB_GbEb(bxInstruction_c *);
  BX_SMF void SUB_GdEd(bxInstruction_c *);
  BX_SMF void SUB_ALIb(bxInstruction_c *);
  BX_SMF void SUB_EAXId(bxInstruction_c *);
  BX_SMF void DAS(bxInstruction_c *);

  BX_SMF void XOR_EbGb(bxInstruction_c *);
  BX_SMF void XOR_EdGd(bxInstruction_c *);
  BX_SMF void XOR_EwGw(bxInstruction_c *);
  BX_SMF void XOR_GbEb(bxInstruction_c *);
  BX_SMF void XOR_GdEd(bxInstruction_c *);
  BX_SMF void XOR_GwEw(bxInstruction_c *);
  BX_SMF void XOR_ALIb(bxInstruction_c *);
  BX_SMF void XOR_EAXId(bxInstruction_c *);
  BX_SMF void XOR_AXIw(bxInstruction_c *);
  BX_SMF void AAA(bxInstruction_c *);
  BX_SMF void CMP_EbGb(bxInstruction_c *);
  BX_SMF void CMP_EdGd(bxInstruction_c *);
  BX_SMF void CMP_GbEb(bxInstruction_c *);
  BX_SMF void CMP_GdEd(bxInstruction_c *);
  BX_SMF void CMP_ALIb(bxInstruction_c *);
  BX_SMF void CMP_EAXId(bxInstruction_c *);
  BX_SMF void AAS(bxInstruction_c *);

  BX_SMF void PUSHAD32(bxInstruction_c *);
  BX_SMF void PUSHAD16(bxInstruction_c *);
  BX_SMF void POPAD32(bxInstruction_c *);
  BX_SMF void POPAD16(bxInstruction_c *);
  BX_SMF void BOUND_GvMa(bxInstruction_c *);
  BX_SMF void ARPL_EwGw(bxInstruction_c *);
  BX_SMF void PUSH_Id(bxInstruction_c *);
  BX_SMF void PUSH_Iw(bxInstruction_c *);
  BX_SMF void IMUL_GdEdId(bxInstruction_c *);
  BX_SMF void INSB_YbDX(bxInstruction_c *);
  BX_SMF void INSW_YvDX(bxInstruction_c *);
  BX_SMF void OUTSB_DXXb(bxInstruction_c *);
  BX_SMF void OUTSW_DXXv(bxInstruction_c *);

  BX_SMF void TEST_EbGb(bxInstruction_c *);
  BX_SMF void TEST_EdGd(bxInstruction_c *);
  BX_SMF void TEST_EwGw(bxInstruction_c *);
  BX_SMF void XCHG_EbGb(bxInstruction_c *);
  BX_SMF void XCHG_EdGd(bxInstruction_c *);
  BX_SMF void XCHG_EwGw(bxInstruction_c *);

  BX_SMF void MOV_EEbGb(bxInstruction_c *);
  BX_SMF void MOV_EGbGb(bxInstruction_c *);

  BX_SMF void MOV_EEdGd(bxInstruction_c *);
  BX_SMF void MOV_EGdGd(bxInstruction_c *);

  BX_SMF void MOV_EEwGw(bxInstruction_c *);
  BX_SMF void MOV_EGwGw(bxInstruction_c *);

  BX_SMF void MOV_GbEEb(bxInstruction_c *);
  BX_SMF void MOV_GbEGb(bxInstruction_c *);

  BX_SMF void MOV_GdEEd(bxInstruction_c *);
  BX_SMF void MOV_GdEGd(bxInstruction_c *);

  BX_SMF void MOV_GwEEw(bxInstruction_c *);
  BX_SMF void MOV_GwEGw(bxInstruction_c *);

  BX_SMF void MOV_EwSw(bxInstruction_c *);
  BX_SMF void LEA_GdM(bxInstruction_c *);
  BX_SMF void LEA_GwM(bxInstruction_c *);
  BX_SMF void MOV_SwEw(bxInstruction_c *);
  BX_SMF void POP_Ev(bxInstruction_c *);

  BX_SMF void CBW(bxInstruction_c *);
  BX_SMF void CWD(bxInstruction_c *);
  BX_SMF void CALL32_Ap(bxInstruction_c *);
  BX_SMF void CALL16_Ap(bxInstruction_c *);
  BX_SMF void FWAIT(bxInstruction_c *);
  BX_SMF void PUSHF_Fv(bxInstruction_c *);
  BX_SMF void POPF_Fv(bxInstruction_c *);
  BX_SMF void SAHF(bxInstruction_c *);
  BX_SMF void LAHF(bxInstruction_c *);

  BX_SMF void MOV_ALOb(bxInstruction_c *);
  BX_SMF void MOV_EAXOd(bxInstruction_c *);
  BX_SMF void MOV_AXOw(bxInstruction_c *);
  BX_SMF void MOV_ObAL(bxInstruction_c *);
  BX_SMF void MOV_OdEAX(bxInstruction_c *);
  BX_SMF void MOV_OwAX(bxInstruction_c *);
  BX_SMF void MOVSB_XbYb(bxInstruction_c *);
  BX_SMF void MOVSW_XvYv(bxInstruction_c *);
  BX_SMF void CMPSB_XbYb(bxInstruction_c *);
  BX_SMF void CMPSW_XvYv(bxInstruction_c *);
  BX_SMF void TEST_ALIb(bxInstruction_c *);
  BX_SMF void TEST_EAXId(bxInstruction_c *);
  BX_SMF void TEST_AXIw(bxInstruction_c *);
  BX_SMF void STOSB_YbAL(bxInstruction_c *);
  BX_SMF void STOSW_YveAX(bxInstruction_c *);
  BX_SMF void LODSB_ALXb(bxInstruction_c *);
  BX_SMF void LODSW_eAXXv(bxInstruction_c *);
  BX_SMF void SCASB_ALXb(bxInstruction_c *);
  BX_SMF void SCASW_eAXXv(bxInstruction_c *);

  BX_SMF void RETnear32(bxInstruction_c *);
  BX_SMF void RETnear16(bxInstruction_c *);
  BX_SMF void LES_GvMp(bxInstruction_c *);
  BX_SMF void LDS_GvMp(bxInstruction_c *);
  BX_SMF void MOV_EbIb(bxInstruction_c *);
  BX_SMF void MOV_EdId(bxInstruction_c *);
  BX_SMF void MOV_EwIw(bxInstruction_c *);
  BX_SMF void ENTER_IwIb(bxInstruction_c *);
  BX_SMF void LEAVE(bxInstruction_c *);
  BX_SMF void RETfar32(bxInstruction_c *);
  BX_SMF void RETfar16(bxInstruction_c *);

  BX_SMF void INT1(bxInstruction_c *);
  BX_SMF void INT3(bxInstruction_c *);
  BX_SMF void INT_Ib(bxInstruction_c *);
  BX_SMF void INTO(bxInstruction_c *);
  BX_SMF void IRET32(bxInstruction_c *);
  BX_SMF void IRET16(bxInstruction_c *);

  BX_SMF void AAM(bxInstruction_c *);
  BX_SMF void AAD(bxInstruction_c *);
  BX_SMF void SALC(bxInstruction_c *);
  BX_SMF void XLAT(bxInstruction_c *);

  BX_SMF void LOOPNE_Jb(bxInstruction_c *);
  BX_SMF void LOOPE_Jb(bxInstruction_c *);
  BX_SMF void LOOP_Jb(bxInstruction_c *);
  BX_SMF void JCXZ_Jb(bxInstruction_c *);
  BX_SMF void IN_ALIb(bxInstruction_c *);
  BX_SMF void IN_eAXIb(bxInstruction_c *);
  BX_SMF void OUT_IbAL(bxInstruction_c *);
  BX_SMF void OUT_IbeAX(bxInstruction_c *);
  BX_SMF void CALL_Aw(bxInstruction_c *);
  BX_SMF void CALL_Ad(bxInstruction_c *);
  BX_SMF void JMP_Jd(bxInstruction_c *);
  BX_SMF void JMP_Jw(bxInstruction_c *);
  BX_SMF void JMP_Ap(bxInstruction_c *);
  BX_SMF void IN_ALDX(bxInstruction_c *);
  BX_SMF void IN_eAXDX(bxInstruction_c *);
  BX_SMF void OUT_DXAL(bxInstruction_c *);
  BX_SMF void OUT_DXeAX(bxInstruction_c *);

  BX_SMF void HLT(bxInstruction_c *);
  BX_SMF void CMC(bxInstruction_c *);
  BX_SMF void CLC(bxInstruction_c *);
  BX_SMF void STC(bxInstruction_c *);
  BX_SMF void CLI(bxInstruction_c *);
  BX_SMF void STI(bxInstruction_c *);
  BX_SMF void CLD(bxInstruction_c *);
  BX_SMF void STD(bxInstruction_c *);


  BX_SMF void LAR_GvEw(bxInstruction_c *);
  BX_SMF void LSL_GvEw(bxInstruction_c *);
  BX_SMF void CLTS(bxInstruction_c *);
  BX_SMF void INVD(bxInstruction_c *);
  BX_SMF void WBINVD(bxInstruction_c *);

  BX_SMF void MOV_CdRd(bxInstruction_c *);
  BX_SMF void MOV_DdRd(bxInstruction_c *);
  BX_SMF void MOV_RdCd(bxInstruction_c *);
  BX_SMF void MOV_RdDd(bxInstruction_c *);
  BX_SMF void MOV_TdRd(bxInstruction_c *);
  BX_SMF void MOV_RdTd(bxInstruction_c *);

  BX_SMF void JCC_Jd(bxInstruction_c *);
  BX_SMF void JCC_Jw(bxInstruction_c *);
  BX_SMF void JZ_Jd(bxInstruction_c *);
  BX_SMF void JZ_Jw(bxInstruction_c *);
  BX_SMF void JNZ_Jd(bxInstruction_c *);
  BX_SMF void JNZ_Jw(bxInstruction_c *);

  BX_SMF void SETO_Eb(bxInstruction_c *);
  BX_SMF void SETNO_Eb(bxInstruction_c *);
  BX_SMF void SETB_Eb(bxInstruction_c *);
  BX_SMF void SETNB_Eb(bxInstruction_c *);
  BX_SMF void SETZ_Eb(bxInstruction_c *);
  BX_SMF void SETNZ_Eb(bxInstruction_c *);
  BX_SMF void SETBE_Eb(bxInstruction_c *);
  BX_SMF void SETNBE_Eb(bxInstruction_c *);
  BX_SMF void SETS_Eb(bxInstruction_c *);
  BX_SMF void SETNS_Eb(bxInstruction_c *);
  BX_SMF void SETP_Eb(bxInstruction_c *);
  BX_SMF void SETNP_Eb(bxInstruction_c *);
  BX_SMF void SETL_Eb(bxInstruction_c *);
  BX_SMF void SETNL_Eb(bxInstruction_c *);
  BX_SMF void SETLE_Eb(bxInstruction_c *);
  BX_SMF void SETNLE_Eb(bxInstruction_c *);

  BX_SMF void CPUID(bxInstruction_c *);
  BX_SMF void BT_EvGv(bxInstruction_c *);
  BX_SMF void SHLD_EdGd(bxInstruction_c *);
  BX_SMF void SHLD_EwGw(bxInstruction_c *);


  BX_SMF void BTS_EvGv(bxInstruction_c *);

  BX_SMF void SHRD_EwGw(bxInstruction_c *);
  BX_SMF void SHRD_EdGd(bxInstruction_c *);

  BX_SMF void IMUL_GdEd(bxInstruction_c *);

  BX_SMF void LSS_GvMp(bxInstruction_c *);
  BX_SMF void BTR_EvGv(bxInstruction_c *);
  BX_SMF void LFS_GvMp(bxInstruction_c *);
  BX_SMF void LGS_GvMp(bxInstruction_c *);
  BX_SMF void MOVZX_GdEb(bxInstruction_c *);
  BX_SMF void MOVZX_GwEb(bxInstruction_c *);
  BX_SMF void MOVZX_GdEw(bxInstruction_c *);
  BX_SMF void MOVZX_GwEw(bxInstruction_c *);
  BX_SMF void BTC_EvGv(bxInstruction_c *);
  BX_SMF void BSF_GvEv(bxInstruction_c *);
  BX_SMF void BSR_GvEv(bxInstruction_c *);
  BX_SMF void MOVSX_GdEb(bxInstruction_c *);
  BX_SMF void MOVSX_GwEb(bxInstruction_c *);
  BX_SMF void MOVSX_GdEw(bxInstruction_c *);
  BX_SMF void MOVSX_GwEw(bxInstruction_c *);

  BX_SMF void BSWAP_EAX(bxInstruction_c *);
  BX_SMF void BSWAP_ECX(bxInstruction_c *);
  BX_SMF void BSWAP_EDX(bxInstruction_c *);
  BX_SMF void BSWAP_EBX(bxInstruction_c *);
  BX_SMF void BSWAP_ESP(bxInstruction_c *);
  BX_SMF void BSWAP_EBP(bxInstruction_c *);
  BX_SMF void BSWAP_ESI(bxInstruction_c *);
  BX_SMF void BSWAP_EDI(bxInstruction_c *);

  BX_SMF void ADD_EbIb(bxInstruction_c *);
  BX_SMF void ADC_EbIb(bxInstruction_c *);
  BX_SMF void SBB_EbIb(bxInstruction_c *);
  BX_SMF void SUB_EbIb(bxInstruction_c *);
  BX_SMF void CMP_EbIb(bxInstruction_c *);

  BX_SMF void XOR_EbIb(bxInstruction_c *);
  BX_SMF void OR_EbIb(bxInstruction_c *);
  BX_SMF void AND_EbIb(bxInstruction_c *);

  BX_SMF void ADD_EEdId(bxInstruction_c *);
  BX_SMF void ADD_EGdId(bxInstruction_c *);

  BX_SMF void OR_EdId(bxInstruction_c *);
  BX_SMF void OR_EwIw(bxInstruction_c *);
  BX_SMF void ADC_EdId(bxInstruction_c *);
  BX_SMF void SBB_EdId(bxInstruction_c *);
  BX_SMF void AND_EdId(bxInstruction_c *);
  BX_SMF void AND_EwIw(bxInstruction_c *);
  BX_SMF void SUB_EdId(bxInstruction_c *);
  BX_SMF void XOR_EdId(bxInstruction_c *);
  BX_SMF void XOR_EwIw(bxInstruction_c *);
  BX_SMF void CMP_EdId(bxInstruction_c *);

  BX_SMF void ROL_Eb(bxInstruction_c *);
  BX_SMF void ROR_Eb(bxInstruction_c *);
  BX_SMF void RCL_Eb(bxInstruction_c *);
  BX_SMF void RCR_Eb(bxInstruction_c *);
  BX_SMF void SHL_Eb(bxInstruction_c *);
  BX_SMF void SHR_Eb(bxInstruction_c *);
  BX_SMF void SAR_Eb(bxInstruction_c *);

  BX_SMF void ROL_Ed(bxInstruction_c *);
  BX_SMF void ROL_Ew(bxInstruction_c *);
  BX_SMF void ROR_Ed(bxInstruction_c *);
  BX_SMF void ROR_Ew(bxInstruction_c *);
  BX_SMF void RCL_Ed(bxInstruction_c *);
  BX_SMF void RCL_Ew(bxInstruction_c *);
  BX_SMF void RCR_Ed(bxInstruction_c *);
  BX_SMF void RCR_Ew(bxInstruction_c *);
  BX_SMF void SHL_Ed(bxInstruction_c *);
  BX_SMF void SHL_Ew(bxInstruction_c *);
  BX_SMF void SHR_Ed(bxInstruction_c *);
  BX_SMF void SHR_Ew(bxInstruction_c *);
  BX_SMF void SAR_Ed(bxInstruction_c *);
  BX_SMF void SAR_Ew(bxInstruction_c *);

  BX_SMF void TEST_EbIb(bxInstruction_c *);
  BX_SMF void NOT_Eb(bxInstruction_c *);
  BX_SMF void NEG_Eb(bxInstruction_c *);
  BX_SMF void MUL_ALEb(bxInstruction_c *);
  BX_SMF void IMUL_ALEb(bxInstruction_c *);
  BX_SMF void DIV_ALEb(bxInstruction_c *);
  BX_SMF void IDIV_ALEb(bxInstruction_c *);

  BX_SMF void TEST_EdId(bxInstruction_c *);
  BX_SMF void TEST_EwIw(bxInstruction_c *);
  BX_SMF void NOT_Ed(bxInstruction_c *);
  BX_SMF void NOT_Ew(bxInstruction_c *);
  BX_SMF void NEG_Ed(bxInstruction_c *);
  BX_SMF void MUL_EAXEd(bxInstruction_c *);
  BX_SMF void IMUL_EAXEd(bxInstruction_c *);
  BX_SMF void DIV_EAXEd(bxInstruction_c *);
  BX_SMF void IDIV_EAXEd(bxInstruction_c *);

  BX_SMF void INC_Eb(bxInstruction_c *);
  BX_SMF void DEC_Eb(bxInstruction_c *);

  BX_SMF void INC_Ed(bxInstruction_c *);
  BX_SMF void DEC_Ed(bxInstruction_c *);
  BX_SMF void CALL_Ed(bxInstruction_c *);
  BX_SMF void CALL_Ew(bxInstruction_c *);
  BX_SMF void CALL32_Ep(bxInstruction_c *);
  BX_SMF void CALL16_Ep(bxInstruction_c *);
  BX_SMF void JMP_Ed(bxInstruction_c *);
  BX_SMF void JMP_Ew(bxInstruction_c *);
  BX_SMF void JMP32_Ep(bxInstruction_c *);
  BX_SMF void JMP16_Ep(bxInstruction_c *);
  BX_SMF void PUSH_Ed(bxInstruction_c *);
  BX_SMF void PUSH_Ew(bxInstruction_c *);

  BX_SMF void SLDT_Ew(bxInstruction_c *);
  BX_SMF void STR_Ew(bxInstruction_c *);
  BX_SMF void LLDT_Ew(bxInstruction_c *);
  BX_SMF void LTR_Ew(bxInstruction_c *);
  BX_SMF void VERR_Ew(bxInstruction_c *);
  BX_SMF void VERW_Ew(bxInstruction_c *);

  BX_SMF void SGDT_Ms(bxInstruction_c *);
  BX_SMF void SIDT_Ms(bxInstruction_c *);
  BX_SMF void LGDT_Ms(bxInstruction_c *);
  BX_SMF void LIDT_Ms(bxInstruction_c *);
  BX_SMF void SMSW_Ew(bxInstruction_c *);
  BX_SMF void LMSW_Ew(bxInstruction_c *);


  BX_SMF void BT_EvIb(bxInstruction_c *);
  BX_SMF void BTS_EvIb(bxInstruction_c *);
  BX_SMF void BTR_EvIb(bxInstruction_c *);
  BX_SMF void BTC_EvIb(bxInstruction_c *);

  BX_SMF void ESC0(bxInstruction_c *);
  BX_SMF void ESC1(bxInstruction_c *);
  BX_SMF void ESC2(bxInstruction_c *);
  BX_SMF void ESC3(bxInstruction_c *);
  BX_SMF void ESC4(bxInstruction_c *);
  BX_SMF void ESC5(bxInstruction_c *);
  BX_SMF void ESC6(bxInstruction_c *);
  BX_SMF void ESC7(bxInstruction_c *);

  /* MMX */
  BX_SMF void PUNPCKLBW_PqQd(bxInstruction_c *i);
  BX_SMF void PUNPCKLWD_PqQd(bxInstruction_c *i);
  BX_SMF void PUNPCKLDQ_PqQd(bxInstruction_c *i);
  BX_SMF void PACKSSWB_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPGTB_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPGTW_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPGTD_PqQq(bxInstruction_c *i);
  BX_SMF void PACKUSWB_PqQq(bxInstruction_c *i);
  BX_SMF void PUNPCKHBW_PqQq(bxInstruction_c *i);
  BX_SMF void PUNPCKHWD_PqQq(bxInstruction_c *i);
  BX_SMF void PUNPCKHDQ_PqQq(bxInstruction_c *i);
  BX_SMF void PACKSSDW_PqQq(bxInstruction_c *i);
  BX_SMF void MOVD_PqEd(bxInstruction_c *i);
  BX_SMF void MOVQ_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPEQB_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPEQW_PqQq(bxInstruction_c *i);
  BX_SMF void PCMPEQD_PqQq(bxInstruction_c *i);
  BX_SMF void EMMS(bxInstruction_c *i);
  BX_SMF void MOVD_EdPd(bxInstruction_c *i);
  BX_SMF void MOVQ_QqPq(bxInstruction_c *i);
  BX_SMF void PSRLW_PqQq(bxInstruction_c *i);
  BX_SMF void PSRLD_PqQq(bxInstruction_c *i);
  BX_SMF void PSRLQ_PqQq(bxInstruction_c *i);
  BX_SMF void PMULLW_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBUSB_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBUSW_PqQq(bxInstruction_c *i);
  BX_SMF void PAND_PqQq(bxInstruction_c *i);
  BX_SMF void PADDUSB_PqQq(bxInstruction_c *i);
  BX_SMF void PADDUSW_PqQq(bxInstruction_c *i);
  BX_SMF void PANDN_PqQq(bxInstruction_c *i);
  BX_SMF void PSRAW_PqQq(bxInstruction_c *i);
  BX_SMF void PSRAD_PqQq(bxInstruction_c *i);
  BX_SMF void PMULHW_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBSB_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBSW_PqQq(bxInstruction_c *i);
  BX_SMF void POR_PqQq(bxInstruction_c *i);
  BX_SMF void PADDSB_PqQq(bxInstruction_c *i);
  BX_SMF void PADDSW_PqQq(bxInstruction_c *i);
  BX_SMF void PXOR_PqQq(bxInstruction_c *i);
  BX_SMF void PSLLW_PqQq(bxInstruction_c *i);
  BX_SMF void PSLLD_PqQq(bxInstruction_c *i);
  BX_SMF void PSLLQ_PqQq(bxInstruction_c *i);
  BX_SMF void PMADDWD_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBB_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBW_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBD_PqQq(bxInstruction_c *i);
  BX_SMF void PADDB_PqQq(bxInstruction_c *i);
  BX_SMF void PADDW_PqQq(bxInstruction_c *i);
  BX_SMF void PADDD_PqQq(bxInstruction_c *i);
  BX_SMF void PSRLW_PqIb(bxInstruction_c *i);
  BX_SMF void PSRAW_PqIb(bxInstruction_c *i);
  BX_SMF void PSLLW_PqIb(bxInstruction_c *i);
  BX_SMF void PSRLD_PqIb(bxInstruction_c *i);
  BX_SMF void PSRAD_PqIb(bxInstruction_c *i);
  BX_SMF void PSLLD_PqIb(bxInstruction_c *i);
  BX_SMF void PSRLQ_PqIb(bxInstruction_c *i);
  BX_SMF void PSLLQ_PqIb(bxInstruction_c *i);
  /* MMX */

#if BX_SUPPORT_MMX
  BX_SMF void PrepareMmxInstruction(void);
  BX_SMF void PrintMmxRegisters(void);
#endif

  BX_SMF void fpu_execute(bxInstruction_c *i);
  BX_SMF void fpu_init(void);
  BX_SMF void fpu_print_regs (void);

  BX_SMF void CMPXCHG_XBTS(bxInstruction_c *);
  BX_SMF void CMPXCHG_IBTS(bxInstruction_c *);
  BX_SMF void CMPXCHG_EbGb(bxInstruction_c *);
  BX_SMF void CMPXCHG_EdGd(bxInstruction_c *);
  BX_SMF void CMPXCHG8B(bxInstruction_c *);
  BX_SMF void XADD_EbGb(bxInstruction_c *);
  BX_SMF void XADD_EdGd(bxInstruction_c *);
  BX_SMF void RETnear32_Iw(bxInstruction_c *);
  BX_SMF void RETnear16_Iw(bxInstruction_c *);
  BX_SMF void RETfar32_Iw(bxInstruction_c *);
  BX_SMF void RETfar16_Iw(bxInstruction_c *);

  BX_SMF void LOADALL(bxInstruction_c *);
  BX_SMF void CMOV_GdEd(bxInstruction_c *);
  BX_SMF void CMOV_GwEw(bxInstruction_c *);

  BX_SMF void ADD_EwGw(bxInstruction_c *);

  BX_SMF void ADD_GwEEw(bxInstruction_c *);
  BX_SMF void ADD_GwEGw(bxInstruction_c *);

  BX_SMF void ADD_AXIw(bxInstruction_c *);
  BX_SMF void ADC_EwGw(bxInstruction_c *);
  BX_SMF void ADC_GwEw(bxInstruction_c *);
  BX_SMF void ADC_AXIw(bxInstruction_c *);
  BX_SMF void SBB_EwGw(bxInstruction_c *);
  BX_SMF void SBB_GwEw(bxInstruction_c *);
  BX_SMF void SBB_AXIw(bxInstruction_c *);
  BX_SMF void SBB_EwIw(bxInstruction_c *);
  BX_SMF void SUB_EwGw(bxInstruction_c *);
  BX_SMF void SUB_GwEw(bxInstruction_c *);
  BX_SMF void SUB_AXIw(bxInstruction_c *);
  BX_SMF void CMP_EwGw(bxInstruction_c *);
  BX_SMF void CMP_GwEw(bxInstruction_c *);
  BX_SMF void CMP_AXIw(bxInstruction_c *);
  BX_SMF void CWDE(bxInstruction_c *);
  BX_SMF void CDQ(bxInstruction_c *);
  BX_SMF void XADD_EwGw(bxInstruction_c *);

  BX_SMF void ADD_EEwIw(bxInstruction_c *);
  BX_SMF void ADD_EGwIw(bxInstruction_c *);

  BX_SMF void ADC_EwIw(bxInstruction_c *);
  BX_SMF void SUB_EwIw(bxInstruction_c *);
  BX_SMF void CMP_EwIw(bxInstruction_c *);
  BX_SMF void NEG_Ew(bxInstruction_c *);
  BX_SMF void INC_Ew(bxInstruction_c *);
  BX_SMF void DEC_Ew(bxInstruction_c *);
  BX_SMF void CMPXCHG_EwGw(bxInstruction_c *);
  BX_SMF void MUL_AXEw(bxInstruction_c *);
  BX_SMF void IMUL_AXEw(bxInstruction_c *);
  BX_SMF void DIV_AXEw(bxInstruction_c *);
  BX_SMF void IDIV_AXEw(bxInstruction_c *);
  BX_SMF void IMUL_GwEwIw(bxInstruction_c *);
  BX_SMF void IMUL_GwEw(bxInstruction_c *);
  BX_SMF void NOP(bxInstruction_c *);
  BX_SMF void MOV_RLIb(bxInstruction_c *);
  BX_SMF void MOV_RHIb(bxInstruction_c *);
  BX_SMF void MOV_RXIw(bxInstruction_c *);
  BX_SMF void MOV_ERXId(bxInstruction_c *);
  BX_SMF void INC_RX(bxInstruction_c *);
  BX_SMF void DEC_RX(bxInstruction_c *);
  BX_SMF void INC_ERX(bxInstruction_c *);
  BX_SMF void DEC_ERX(bxInstruction_c *);
  BX_SMF void PUSH_RX(bxInstruction_c *);
  BX_SMF void POP_RX(bxInstruction_c *);
  BX_SMF void PUSH_ERX(bxInstruction_c *);
  BX_SMF void POP_ERX(bxInstruction_c *);
  BX_SMF void POP_Ew(bxInstruction_c *);
  BX_SMF void POP_Ed(bxInstruction_c *);
  BX_SMF void XCHG_RXAX(bxInstruction_c *);
  BX_SMF void XCHG_ERXEAX(bxInstruction_c *);

#if BX_SUPPORT_X86_64
  // 64 bit extensions
  BX_SMF void ADD_EqGq(bxInstruction_c *);
  BX_SMF void ADD_GqEq(bxInstruction_c *);
  BX_SMF void ADD_RAXId(bxInstruction_c *);
  BX_SMF void OR_EqGq(bxInstruction_c *);
  BX_SMF void OR_GqEq(bxInstruction_c *);
  BX_SMF void OR_RAXId(bxInstruction_c *);


  BX_SMF void ADC_EqGq(bxInstruction_c *);
  BX_SMF void ADC_GqEq(bxInstruction_c *);
  BX_SMF void ADC_RAXId(bxInstruction_c *);
  BX_SMF void SBB_EqGq(bxInstruction_c *);
  BX_SMF void SBB_GqEq(bxInstruction_c *);
  BX_SMF void SBB_RAXId(bxInstruction_c *);

  BX_SMF void AND_EqGq(bxInstruction_c *);
  BX_SMF void AND_GqEq(bxInstruction_c *);
  BX_SMF void AND_RAXId(bxInstruction_c *);
  BX_SMF void SUB_EqGq(bxInstruction_c *);
  BX_SMF void SUB_GqEq(bxInstruction_c *);
  BX_SMF void SUB_RAXId(bxInstruction_c *);

  BX_SMF void XOR_EqGq(bxInstruction_c *);
  BX_SMF void XOR_GqEq(bxInstruction_c *);
  BX_SMF void XOR_RAXId(bxInstruction_c *);
  BX_SMF void CMP_EqGq(bxInstruction_c *);
  BX_SMF void CMP_GqEq(bxInstruction_c *);
  BX_SMF void CMP_RAXId(bxInstruction_c *);

  BX_SMF void PUSHAD64(bxInstruction_c *);
  BX_SMF void POPAD64(bxInstruction_c *);
  BX_SMF void PUSH64_Id(bxInstruction_c *);
  BX_SMF void IMUL_GqEqId(bxInstruction_c *);

  BX_SMF void TEST_EqGq(bxInstruction_c *);
  BX_SMF void XCHG_EqGq(bxInstruction_c *);
  BX_SMF void MOV_EqGq(bxInstruction_c *);
  BX_SMF void MOV_GqEq(bxInstruction_c *);
  BX_SMF void LEA_GqM(bxInstruction_c *);

  BX_SMF void CALL64_Ap(bxInstruction_c *);

  BX_SMF void MOV_RAXOq(bxInstruction_c *);
  BX_SMF void MOV_OqRAX(bxInstruction_c *);
  BX_SMF void MOV_EAXOq(bxInstruction_c *);
  BX_SMF void MOV_OqEAX(bxInstruction_c *);
  BX_SMF void MOV_AXOq(bxInstruction_c *);
  BX_SMF void MOV_OqAX(bxInstruction_c *);
  BX_SMF void MOV_ALOq(bxInstruction_c *);
  BX_SMF void MOV_OqAL(bxInstruction_c *);

  BX_SMF void TEST_RAXId(bxInstruction_c *);

  BX_SMF void RETnear64(bxInstruction_c *);
  BX_SMF void MOV_EqId(bxInstruction_c *);
  BX_SMF void ENTER64_IwIb(bxInstruction_c *);
  BX_SMF void LEAVE64(bxInstruction_c *);
  BX_SMF void RETfar64(bxInstruction_c *);

  BX_SMF void IRET64(bxInstruction_c *);

  //BX_SMF void IN_eAXIb(bxInstruction_c *);
  //BX_SMF void OUT_IbeAX(bxInstruction_c *);
  BX_SMF void CALL_Aq(bxInstruction_c *);
  BX_SMF void JMP_Jq(bxInstruction_c *);
  //BX_SMF void IN_eAXDX(bxInstruction_c *);
  //BX_SMF void OUT_DXeAX(bxInstruction_c *);



  BX_SMF void MOV_CqRq(bxInstruction_c *);
  BX_SMF void MOV_DqRq(bxInstruction_c *);
  BX_SMF void MOV_RqCq(bxInstruction_c *);
  BX_SMF void MOV_RqDq(bxInstruction_c *);
  BX_SMF void MOV_TqRq(bxInstruction_c *);
  BX_SMF void MOV_RqTq(bxInstruction_c *);

  BX_SMF void JCC_Jq(bxInstruction_c *);

  BX_SMF void SHLD_EqGq(bxInstruction_c *);

  BX_SMF void SHRD_EqGq(bxInstruction_c *);

  BX_SMF void IMUL_GqEq(bxInstruction_c *);

  BX_SMF void MOVZX_GqEb(bxInstruction_c *);
  BX_SMF void MOVZX_GqEw(bxInstruction_c *);
  BX_SMF void MOVSX_GqEb(bxInstruction_c *);
  BX_SMF void MOVSX_GqEw(bxInstruction_c *);
  BX_SMF void MOVSX_GqEd(bxInstruction_c *);

  BX_SMF void BSWAP_RAX(bxInstruction_c *);
  BX_SMF void BSWAP_RCX(bxInstruction_c *);
  BX_SMF void BSWAP_RDX(bxInstruction_c *);
  BX_SMF void BSWAP_RBX(bxInstruction_c *);
  BX_SMF void BSWAP_RSP(bxInstruction_c *);
  BX_SMF void BSWAP_RBP(bxInstruction_c *);
  BX_SMF void BSWAP_RSI(bxInstruction_c *);
  BX_SMF void BSWAP_RDI(bxInstruction_c *);

  BX_SMF void ADD_EqId(bxInstruction_c *);
  BX_SMF void OR_EqId(bxInstruction_c *);
  BX_SMF void ADC_EqId(bxInstruction_c *);
  BX_SMF void SBB_EqId(bxInstruction_c *);
  BX_SMF void AND_EqId(bxInstruction_c *);
  BX_SMF void SUB_EqId(bxInstruction_c *);
  BX_SMF void XOR_EqId(bxInstruction_c *);
  BX_SMF void CMP_EqId(bxInstruction_c *);

  BX_SMF void ROL_Eq(bxInstruction_c *);
  BX_SMF void ROR_Eq(bxInstruction_c *);
  BX_SMF void RCL_Eq(bxInstruction_c *);
  BX_SMF void RCR_Eq(bxInstruction_c *);
  BX_SMF void SHL_Eq(bxInstruction_c *);
  BX_SMF void SHR_Eq(bxInstruction_c *);
  BX_SMF void SAR_Eq(bxInstruction_c *);

  BX_SMF void TEST_EqId(bxInstruction_c *);
  BX_SMF void NOT_Eq(bxInstruction_c *);
  BX_SMF void NEG_Eq(bxInstruction_c *);
  BX_SMF void MUL_RAXEq(bxInstruction_c *);
  BX_SMF void IMUL_RAXEq(bxInstruction_c *);
  BX_SMF void DIV_RAXEq(bxInstruction_c *);
  BX_SMF void IDIV_RAXEq(bxInstruction_c *);

  BX_SMF void INC_Eq(bxInstruction_c *);
  BX_SMF void DEC_Eq(bxInstruction_c *);
  BX_SMF void CALL_Eq(bxInstruction_c *);
  BX_SMF void CALL64_Ep(bxInstruction_c *);
  BX_SMF void JMP_Eq(bxInstruction_c *);
  BX_SMF void JMP64_Ep(bxInstruction_c *);
  BX_SMF void PUSH_Eq(bxInstruction_c *);


  BX_SMF void CMPXCHG_EqGq(bxInstruction_c *);
  BX_SMF void CDQE(bxInstruction_c *);
  BX_SMF void CQO(bxInstruction_c *);
  BX_SMF void XADD_EqGq(bxInstruction_c *);
  BX_SMF void RETnear64_Iw(bxInstruction_c *);
  BX_SMF void RETfar64_Iw(bxInstruction_c *);

  BX_SMF void CMOV_GqEq(bxInstruction_c *);

  BX_SMF void MOV_RRXIq(bxInstruction_c *);
  BX_SMF void INC_RRX(bxInstruction_c *);
  BX_SMF void DEC_RRX(bxInstruction_c *);
  BX_SMF void PUSH_RRX(bxInstruction_c *);
  BX_SMF void POP_RRX(bxInstruction_c *);
  BX_SMF void POP_Eq(bxInstruction_c *);
  BX_SMF void XCHG_RRXRAX(bxInstruction_c *);

  BX_SMF void PUSH64_CS(bxInstruction_c *);
  BX_SMF void PUSH64_DS(bxInstruction_c *);
  BX_SMF void POP64_DS(bxInstruction_c *);
  BX_SMF void PUSH64_ES(bxInstruction_c *);
  BX_SMF void POP64_ES(bxInstruction_c *);
  BX_SMF void PUSH64_FS(bxInstruction_c *);
  BX_SMF void POP64_FS(bxInstruction_c *);
  BX_SMF void PUSH64_GS(bxInstruction_c *);
  BX_SMF void POP64_GS(bxInstruction_c *);
  BX_SMF void PUSH64_SS(bxInstruction_c *);
  BX_SMF void POP64_SS(bxInstruction_c *);

  BX_SMF void SYSCALL(bxInstruction_c *i);
  BX_SMF void SYSRET(bxInstruction_c *i);

  BX_SMF void SWAPGS(bxInstruction_c *i);

  BX_SMF void LOOPNE64_Jb(bxInstruction_c *);
  BX_SMF void LOOPE64_Jb(bxInstruction_c *);
  BX_SMF void LOOP64_Jb(bxInstruction_c *);
  BX_SMF void JCXZ64_Jb(bxInstruction_c *);
#endif  // #if BX_SUPPORT_X86_64

  // mch added
  BX_SMF void INVLPG(bxInstruction_c *);
  BX_SMF void wait_for_interrupt();
  BX_SMF void RSM(bxInstruction_c *);

  BX_SMF void WRMSR(bxInstruction_c *);
  BX_SMF void RDTSC(bxInstruction_c *);
  BX_SMF void RDMSR(bxInstruction_c *);
  BX_SMF void SetCR0(Bit32u val_32);
#if BX_CPU_LEVEL >= 4
  BX_SMF void SetCR4(Bit32u val_32);
#endif
  BX_SMF void dynamic_translate(void);
  BX_SMF void dynamic_init(void);
  BX_SMF unsigned fetchDecode(Bit8u *, bxInstruction_c *, unsigned);
#if BX_SUPPORT_X86_64
  BX_SMF unsigned fetchDecode64(Bit8u *, bxInstruction_c *, unsigned);
#endif
  BX_SMF void UndefinedOpcode(bxInstruction_c *);
  BX_SMF void BxError(bxInstruction_c *i);
  BX_SMF void BxResolveError(bxInstruction_c *i);

  BX_SMF void Resolve16Mod0Rm0(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm1(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm2(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm3(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm4(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm5(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm6(bxInstruction_c *);
  BX_SMF void Resolve16Mod0Rm7(bxInstruction_c *);

  BX_SMF void Resolve16Mod1or2Rm0(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm1(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm2(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm3(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm4(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm5(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm6(bxInstruction_c *);
  BX_SMF void Resolve16Mod1or2Rm7(bxInstruction_c *);

  BX_SMF void Resolve32Mod0Rm0(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm1(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm2(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm3(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm5(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm6(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Rm7(bxInstruction_c *);

  BX_SMF void Resolve32Mod1or2Rm0(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm1(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm2(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm3(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm5(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm6(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Rm7(bxInstruction_c *);

  BX_SMF void Resolve32Mod0Base0(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base1(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base2(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base3(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base4(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base5(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base6(bxInstruction_c *);
  BX_SMF void Resolve32Mod0Base7(bxInstruction_c *);

  BX_SMF void Resolve32Mod1or2Base0(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base1(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base2(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base3(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base4(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base5(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base6(bxInstruction_c *);
  BX_SMF void Resolve32Mod1or2Base7(bxInstruction_c *);

#if BX_SUPPORT_X86_64
  // 64 bit addressing

  BX_SMF void Resolve64Mod0Rm0(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm1(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm2(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm3(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm5(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm6(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm7(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm8(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm9(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm10(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm11(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm12(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm13(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm14(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Rm15(bxInstruction_c *);

  BX_SMF void Resolve64Mod1or2Rm0(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm1(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm2(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm3(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm5(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm6(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm7(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm8(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm9(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm10(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm11(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm12(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm13(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm14(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Rm15(bxInstruction_c *);

  BX_SMF void Resolve64Mod0Base0(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base1(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base2(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base3(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base4(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base5(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base6(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base7(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base8(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base9(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base10(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base11(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base12(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base13(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base14(bxInstruction_c *);
  BX_SMF void Resolve64Mod0Base15(bxInstruction_c *);

  BX_SMF void Resolve64Mod1or2Base0(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base1(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base2(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base3(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base4(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base5(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base6(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base7(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base8(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base9(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base10(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base11(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base12(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base13(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base14(bxInstruction_c *);
  BX_SMF void Resolve64Mod1or2Base15(bxInstruction_c *);
#endif  // #if BX_SUPPORT_X86_64


  BX_SMF void REP(void (*)(void));
  BX_SMF void REP_ZF(void (*)(void), unsigned rep_prefix);
#if BX_DEBUGGER
  BX_SMF void     dbg_take_irq(void);
  BX_SMF void     dbg_force_interrupt(unsigned vector);
  BX_SMF void     dbg_take_dma(void);
  BX_SMF Boolean  dbg_get_cpu(bx_dbg_cpu_t *cpu);
  BX_SMF Boolean  dbg_set_cpu(bx_dbg_cpu_t *cpu);
  BX_SMF Boolean  dbg_set_reg(unsigned reg, Bit32u val);
  BX_SMF Bit32u   dbg_get_reg(unsigned reg);
  BX_SMF Boolean  dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no);
  BX_SMF unsigned dbg_query_pending(void);
  BX_SMF Bit32u   dbg_get_descriptor_l(bx_descriptor_t *);
  BX_SMF Bit32u   dbg_get_descriptor_h(bx_descriptor_t *);
  BX_SMF Bit32u   dbg_get_eflags(void);
  BX_SMF Boolean  dbg_is_begin_instr_bpoint(Bit32u cs, Bit32u eip, Bit32u laddr,
                                            Bit32u is_32);
  BX_SMF Boolean  dbg_is_end_instr_bpoint(Bit32u cs, Bit32u eip,
                                          Bit32u laddr, Bit32u is_32);
#endif
#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION
  BX_SMF void     dbg_xlate_linear2phy(Bit32u linear, Bit32u *phy, Boolean *valid);
#endif
  BX_SMF void     atexit(void);

  // now for some ancillary functions...
  BX_SMF void cpu_loop(Bit32s max_instr_count);
  BX_SMF void boundaryFetch(bxInstruction_c *i);
  BX_SMF void decode_exgx16(unsigned need_fetch);
  BX_SMF void decode_exgx32(unsigned need_fetch);

  BX_SMF void prefetch(void);
  // revalidate_prefetch_q is now a no-op, due to the newer EIP window
  // technique.
  BX_SMF BX_CPP_INLINE void revalidate_prefetch_q(void) { }
  BX_SMF BX_CPP_INLINE void invalidate_prefetch_q(void) {
    BX_CPU_THIS_PTR eipPageWindowSize = 0;
    }

  BX_SMF void write_virtual_checks(bx_segment_reg_t *seg, bx_address offset, unsigned length);
  BX_SMF void read_virtual_checks(bx_segment_reg_t *seg, bx_address offset, unsigned length);
  BX_SMF void write_virtual_byte(unsigned seg, bx_address offset, Bit8u *data);
  BX_SMF void write_virtual_word(unsigned seg, bx_address offset, Bit16u *data);
  BX_SMF void write_virtual_dword(unsigned seg, bx_address offset, Bit32u *data);
  BX_SMF void write_virtual_qword(unsigned seg, bx_address offset, Bit64u *data);
  BX_SMF void read_virtual_byte(unsigned seg, bx_address offset, Bit8u *data);
  BX_SMF void read_virtual_word(unsigned seg, bx_address offset, Bit16u *data);
  BX_SMF void read_virtual_dword(unsigned seg, bx_address offset, Bit32u *data);
  BX_SMF void read_virtual_qword(unsigned seg, bx_address offset, Bit64u *data);

  BX_SMF void read_RMW_virtual_byte(unsigned seg, bx_address offset, Bit8u *data);
  BX_SMF void read_RMW_virtual_word(unsigned seg, bx_address offset, Bit16u *data);
  BX_SMF void read_RMW_virtual_dword(unsigned seg, bx_address offset, Bit32u *data);
  BX_SMF void read_RMW_virtual_qword(unsigned seg, bx_address offset, Bit64u *data);
  BX_SMF void write_RMW_virtual_byte(Bit8u val8);
  BX_SMF void write_RMW_virtual_word(Bit16u val16);
  BX_SMF void write_RMW_virtual_dword(Bit32u val32);
  BX_SMF void write_RMW_virtual_qword(Bit64u val64);

#define Write_RMW_virtual_byte(val8)   write_RMW_virtual_byte(val8)
#define Write_RMW_virtual_word(val16)  write_RMW_virtual_word(val16)
#define Write_RMW_virtual_dword(val32) write_RMW_virtual_dword(val32)
#define Write_RMW_virtual_qword(val32) write_RMW_virtual_qword(val32)

  BX_SMF void access_linear(bx_address address, unsigned length, unsigned pl,
                     unsigned rw, void *data);
  BX_SMF Bit32u itranslate_linear(bx_address laddr, unsigned pl);
  BX_SMF Bit32u dtranslate_linear(bx_address laddr, unsigned pl, unsigned rw);
  BX_SMF void TLB_flush(Boolean invalidateGlobal);
  BX_SMF void TLB_init(void);
  BX_SMF void set_INTR(Boolean value);
  BX_SMF char *strseg(bx_segment_reg_t *seg);
  BX_SMF void interrupt(Bit8u vector, Boolean is_INT, Boolean is_error_code,
                 Bit16u error_code);
#if BX_CPU_LEVEL >= 2
  BX_SMF void exception(unsigned vector, Bit16u error_code, Boolean is_INT);
#endif
  BX_SMF int  int_number(bx_segment_reg_t *seg);
  BX_SMF void shutdown_cpu(void);
  BX_SMF void CR3_change(bx_address value);
  BX_SMF void pagingCR0Changed(Bit32u oldCR0, Bit32u newCR0);
  BX_SMF void pagingCR4Changed(Bit32u oldCR4, Bit32u newCR4);
  BX_SMF void pagingA20Changed(void);

  BX_SMF void reset(unsigned source);

  BX_SMF void jump_protected(bxInstruction_c *, Bit16u cs, bx_address disp);
  BX_SMF void call_protected(bxInstruction_c *, Bit16u cs, bx_address disp);
  BX_SMF void return_protected(bxInstruction_c *, Bit16u pop_bytes);
  BX_SMF void iret_protected(bxInstruction_c *);
  BX_SMF void validate_seg_regs(void);
  BX_SMF void stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector,
                                     Bit32u flags32);
  BX_SMF void stack_return_from_v86(bxInstruction_c *);
  BX_SMF void init_v8086_mode(void);
  BX_SMF void v8086_message(void);
  BX_SMF void task_switch(bx_selector_t *selector,
                     bx_descriptor_t *descriptor,
                     unsigned source,
                     Bit32u dword1, Bit32u dword2);
  BX_SMF void get_SS_ESP_from_TSS(unsigned pl, Bit16u *ss, Bit32u *esp);
#if BX_SUPPORT_X86_64
  BX_SMF void get_RSP_from_TSS(unsigned pl, Bit64u *rsp);
#endif
  BX_SMF void write_flags(Bit16u flags, Boolean change_IOPL, Boolean change_IF);
  BX_SMF void write_eflags(Bit32u eflags, Boolean change_IOPL, Boolean change_IF,
                    Boolean change_VM, Boolean change_RF);
  BX_SMF Bit16u read_flags(void);
  BX_SMF Bit32u read_eflags(void);
  BX_SMF Bit32u get_segment_base(unsigned seg);

  BX_SMF Bit8u   inp8(Bit16u addr);
  BX_SMF void    outp8(Bit16u addr, Bit8u value);
  BX_SMF Bit16u  inp16(Bit16u addr);
  BX_SMF void    outp16(Bit16u addr, Bit16u value);
  BX_SMF Bit32u  inp32(Bit16u addr);
  BX_SMF void    outp32(Bit16u addr, Bit32u value);
  BX_SMF Boolean allow_io(Bit16u addr, unsigned len);
  BX_SMF void    enter_protected_mode(void);
  BX_SMF void    enter_real_mode(void);
  BX_SMF void    parse_selector(Bit16u raw_selector, bx_selector_t *selector);
  BX_SMF void    parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp);
  BX_SMF void    load_ldtr(bx_selector_t *selector, bx_descriptor_t *descriptor);
  BX_SMF void    load_cs(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl);
  BX_SMF void    load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl);
  BX_SMF void    fetch_raw_descriptor(bx_selector_t *selector,
                               Bit32u *dword1, Bit32u *dword2, Bit8u exception);
  BX_SMF void    load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value);
#if BX_SUPPORT_X86_64
  BX_SMF void    loadSRegLMNominal(unsigned seg, unsigned selector,
                                   bx_address base, unsigned dpl);
#endif
  BX_SMF Boolean fetch_raw_descriptor2(bx_selector_t *selector,
                                Bit32u *dword1, Bit32u *dword2);
  BX_SMF void    push_16(Bit16u value16);
  BX_SMF void    push_32(Bit32u value32);
#if BX_SUPPORT_X86_64
  BX_SMF void    push_64(Bit64u value64);
#endif
  BX_SMF void    pop_16(Bit16u *value16_ptr);
  BX_SMF void    pop_32(Bit32u *value32_ptr);
#if BX_SUPPORT_X86_64
  BX_SMF void    pop_64(Bit64u *value64_ptr);
#endif
  BX_SMF Boolean can_push(bx_descriptor_t *descriptor, Bit32u esp, Bit32u bytes);
  BX_SMF Boolean can_pop(Bit32u bytes);
  BX_SMF void    sanity_checks(void);

  BX_SMF void    debug(Bit32u offset);

#if BX_EXTERNAL_DEBUGGER
  BX_SMF void    trap_debugger(Boolean callnow);
#endif

#if BX_X86_DEBUGGER
  // x86 hardware debug support
  BX_SMF Bit32u  hwdebug_compare(Bit32u laddr, unsigned size,
                                 unsigned opa, unsigned opb);
#endif

  BX_CPP_INLINE const bx_gen_reg_t *get_gen_reg() { return gen_reg; }

  DECLARE_EFLAGS_ACCESSORS()
  DECLARE_EFLAG_ACCESSOR   (DF,  10)
  DECLARE_EFLAG_ACCESSOR   (ID,  21)
  DECLARE_EFLAG_ACCESSOR   (VP,  20)
  DECLARE_EFLAG_ACCESSOR   (VF,  19)
  DECLARE_EFLAG_ACCESSOR   (AC,  18)
  DECLARE_EFLAG_ACCESSOR_VM(     17)
  DECLARE_EFLAG_ACCESSOR   (RF,  16)
  DECLARE_EFLAG_ACCESSOR   (NT,  14)
  DECLARE_EFLAG_ACCESSOR_IOPL(   12)
  DECLARE_EFLAG_ACCESSOR   (IF,   9)
  DECLARE_EFLAG_ACCESSOR   (TF,   8)

  BX_SMF BX_CPP_INLINE void set_CF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_AF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_ZF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_SF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_OF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_PF(Boolean val);
  BX_SMF BX_CPP_INLINE void set_PF_base(Bit8u val);

  DECLARE_8BIT_REGISTER_ACCESSORS(AL);
  DECLARE_8BIT_REGISTER_ACCESSORS(AH);
  DECLARE_8BIT_REGISTER_ACCESSORS(BL);
  DECLARE_8BIT_REGISTER_ACCESSORS(BH);
  DECLARE_8BIT_REGISTER_ACCESSORS(CL);
  DECLARE_8BIT_REGISTER_ACCESSORS(CH);
  DECLARE_8BIT_REGISTER_ACCESSORS(DL);
  DECLARE_8BIT_REGISTER_ACCESSORS(DH);

  DECLARE_16BIT_REGISTER_ACCESSORS(AX);
  DECLARE_16BIT_REGISTER_ACCESSORS(BX);
  DECLARE_16BIT_REGISTER_ACCESSORS(CX);
  DECLARE_16BIT_REGISTER_ACCESSORS(DX);
  DECLARE_16BIT_REGISTER_ACCESSORS(SP);
  DECLARE_16BIT_REGISTER_ACCESSORS(BP);
  DECLARE_16BIT_REGISTER_ACCESSORS(SI);
  DECLARE_16BIT_REGISTER_ACCESSORS(DI);

  DECLARE_32BIT_REGISTER_ACCESSORS(EAX);
  DECLARE_32BIT_REGISTER_ACCESSORS(EBX);
  DECLARE_32BIT_REGISTER_ACCESSORS(ECX);
  DECLARE_32BIT_REGISTER_ACCESSORS(EDX);
  DECLARE_32BIT_REGISTER_ACCESSORS(ESP);
  DECLARE_32BIT_REGISTER_ACCESSORS(EBP);
  DECLARE_32BIT_REGISTER_ACCESSORS(ESI);
  DECLARE_32BIT_REGISTER_ACCESSORS(EDI);

  BX_CPP_INLINE Bit8u  get_CPL(void);
  BX_CPP_INLINE Bit32u get_EIP(void);

  BX_SMF BX_CPP_INLINE int which_cpu(void);

#if BX_CPU_LEVEL >= 2
  BX_SMF BX_CPP_INLINE Boolean real_mode(void);
#endif
#if BX_CPU_LEVEL >= 3
  BX_SMF BX_CPP_INLINE Boolean protected_mode(void);
  BX_SMF BX_CPP_INLINE Boolean v8086_mode(void);
#endif
#if BX_SUPPORT_APIC
  bx_local_apic_c local_apic;
  Boolean int_from_local_apic;
#endif
  };


#if BX_SupportICache

BX_CPP_INLINE void bxICache_c::decWriteStamp(BX_CPU_C *cpu, Bit32u a20Addr) {
  // Increment page write stamp, so iCache entries with older stamps
  // are effectively invalidated.
  Bit32u pageIndex = a20Addr >> 12;
  Bit32u writeStamp = cpu->iCache.pageWriteStampTable[pageIndex];
  if ( writeStamp & 0x20000000 ) {
    // Page possibly contains iCache code.
    if ( writeStamp & ICacheWriteStampMask ) {
      // Short case: there is room to decrement the generation counter.
      cpu->iCache.pageWriteStampTable[pageIndex]--;
      }
    else {
      // Long case: there is no more room to decrement.  We have dump
      // all iCache entries which can possibly hash to this page since
      // we don't keep track of individual entries.

      // Take the hash of the 0th page offset.
      unsigned iCacheHash = cpu->iCache.hash(a20Addr & 0xfffff000);
      for (unsigned o=0; o<4096; o++) {
        cpu->iCache.entry[iCacheHash].writeStamp = ICacheWriteStampInvalid;
        iCacheHash = (iCacheHash + 1) % BxICacheEntries;
        }
      // Reset write stamp to highest value to begin the decrementing process
      // again.
      cpu->iCache.pageWriteStampTable[pageIndex] = ICacheWriteStampInvalid;
      }
    }
  }

BX_CPP_INLINE Bit32u bxICache_c::createFetchModeMask(BX_CPU_C *cpu) {
  return (cpu->sregs[BX_SEG_REG_CS].cache.u.segment.d_b << 31)
#if BX_SUPPORT_X86_64
         | ((cpu->cpu_mode == BX_MODE_LONG_64)<<30)
#endif
         | (1<<29) // iCache code.
         ;
  }

#endif


#if BX_X86_DEBUGGER
#define BX_HWDebugInstruction   0x00
#define BX_HWDebugMemW          0x01
#define BX_HWDebugIO            0x02
#define BX_HWDebugMemRW         0x03
#endif

BX_SMF BX_CPP_INLINE int BX_CPU_C_PREFIX which_cpu(void)
{ 
#if (BX_SMP_PROCESSORS==1)
   return 0;
#else
   return local_apic.get_id();
#endif
}

IMPLEMENT_8LBIT_REGISTER_ACCESSORS(AL);
IMPLEMENT_8HBIT_REGISTER_ACCESSORS(AH);
IMPLEMENT_8LBIT_REGISTER_ACCESSORS(BL);
IMPLEMENT_8HBIT_REGISTER_ACCESSORS(BH);
IMPLEMENT_8LBIT_REGISTER_ACCESSORS(CL);
IMPLEMENT_8HBIT_REGISTER_ACCESSORS(CH);
IMPLEMENT_8LBIT_REGISTER_ACCESSORS(DL);
IMPLEMENT_8HBIT_REGISTER_ACCESSORS(DH);

IMPLEMENT_16BIT_REGISTER_ACCESSORS(AX);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(BX);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(CX);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(DX);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(SP);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(BP);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(SI);
IMPLEMENT_16BIT_REGISTER_ACCESSORS(DI);

IMPLEMENT_32BIT_REGISTER_ACCESSORS(EAX);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(EBX);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(ECX);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(EDX);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(ESP);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(EBP);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(ESI);
IMPLEMENT_32BIT_REGISTER_ACCESSORS(EDI);

BX_SMF BX_CPP_INLINE Bit8u  BX_CPU_C_PREFIX get_CPL(void) { 
   return (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl); 
} 

BX_CPP_INLINE Bit32u BX_CPU_C::get_EIP(void) {
   return (BX_CPU_THIS_PTR dword.eip); 
} 

BX_SMF BX_CPP_INLINE Bit32u BX_CPU_C_PREFIX get_segment_base(unsigned seg) {
   return (BX_CPU_THIS_PTR sregs[seg].cache.u.segment.base);
}


#if BX_CPU_LEVEL >= 2
  BX_CPP_INLINE Boolean BX_CPU_C::real_mode(void) { return( !BX_CPU_THIS_PTR cr0.pe ); };
#endif

#if BX_CPU_LEVEL == 2
  BX_CPP_INLINE Boolean BX_CPU_C::protected_mode(void) { return( BX_CPU_THIS_PTR cr0.pe ); };
#endif


#if BX_CPU_LEVEL >= 3
#  if BX_SUPPORT_V8086_MODE
  BX_CPP_INLINE Boolean
BX_CPU_C::v8086_mode(void) {
  return (BX_CPU_THIS_PTR get_VM ());
  }

  BX_CPP_INLINE Boolean
BX_CPU_C::protected_mode(void) {
  return(BX_CPU_THIS_PTR cr0.pe && !BX_CPU_THIS_PTR get_VM ());
  }
#  else
  BX_CPP_INLINE Boolean
BX_CPU_C::v8086_mode(void) {
  return(0);
  }

  BX_CPP_INLINE Boolean
BX_CPU_C::protected_mode(void) {
  return(BX_CPU_THIS_PTR cr0.pe);
  }
#  endif
#endif

    BX_CPP_INLINE void
BX_CPU_C::set_CF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xfffff0;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<0);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val);
    }

    BX_CPP_INLINE void
BX_CPU_C::set_AF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xfff0ff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<4);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<4;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_ZF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xff0fff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<6);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<6;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_SF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xf0ffff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<7);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<7;
    }


    BX_CPP_INLINE void
BX_CPU_C::set_OF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0x0fffff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<11);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<11;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_PF(Boolean val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<2;
    }

extern const Boolean bx_parity_lookup[256];

    BX_CPP_INLINE void
BX_CPU_C::set_PF_base(Bit8u val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
    val = bx_parity_lookup[val]; // Always returns 0 or 1.
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
    BX_CPU_THIS_PTR eflags.val32 |= val<<2;
    }


#define SET_FLAGS_OSZAPC_8(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1_8 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_8 = op2; \
    BX_CPU_THIS_PTR oszapc.result_8 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_8_CF(op1, op2, result, ins, last_CF) { \
    BX_CPU_THIS_PTR oszapc.op1_8 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_8 = op2; \
    BX_CPU_THIS_PTR oszapc.result_8 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR oszapc.prev_CF = last_CF; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_16(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1_16 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_16 = op2; \
    BX_CPU_THIS_PTR oszapc.result_16 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_16_CF(op1, op2, result, ins, last_CF) { \
    BX_CPU_THIS_PTR oszapc.op1_16 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_16 = op2; \
    BX_CPU_THIS_PTR oszapc.result_16 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR oszapc.prev_CF = last_CF; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_32(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1_32 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_32 = op2; \
    BX_CPU_THIS_PTR oszapc.result_32 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_32_CF(op1, op2, result, ins, last_CF) { \
    BX_CPU_THIS_PTR oszapc.op1_32 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_32 = op2; \
    BX_CPU_THIS_PTR oszapc.result_32 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR oszapc.prev_CF = last_CF; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_64(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1_64 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_64 = op2; \
    BX_CPU_THIS_PTR oszapc.result_64 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }

#define SET_FLAGS_OSZAPC_64_CF(op1, op2, result, ins, last_CF) { \
    BX_CPU_THIS_PTR oszapc.op1_64 = op1; \
    BX_CPU_THIS_PTR oszapc.op2_64 = op2; \
    BX_CPU_THIS_PTR oszapc.result_64 = result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR oszapc.prev_CF = last_CF; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
    }


#define SET_FLAGS_OSZAP_8(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszap.op1_8 = op1; \
    BX_CPU_THIS_PTR oszap.op2_8 = op2; \
    BX_CPU_THIS_PTR oszap.result_8 = result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
    }

#define SET_FLAGS_OSZAP_16(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszap.op1_16 = op1; \
    BX_CPU_THIS_PTR oszap.op2_16 = op2; \
    BX_CPU_THIS_PTR oszap.result_16 = result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
    }

#define SET_FLAGS_OSZAP_32(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszap.op1_32 = op1; \
    BX_CPU_THIS_PTR oszap.op2_32 = op2; \
    BX_CPU_THIS_PTR oszap.result_32 = result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
    }

#define SET_FLAGS_OSZAP_64(op1, op2, result, ins) { \
    BX_CPU_THIS_PTR oszap.op1_64 = op1; \
    BX_CPU_THIS_PTR oszap.op2_64 = op2; \
    BX_CPU_THIS_PTR oszap.result_64 = result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
    }

#define SET_FLAGS_OxxxxC(new_of, new_cf) { \
    BX_CPU_THIS_PTR eflags.val32 &= ~((1<<11) | (1<<0)); \
    BX_CPU_THIS_PTR eflags.val32 |= ((!!new_of)<<11) | ((!!new_cf)<<0); \
    BX_CPU_THIS_PTR lf_flags_status &= 0x0ffff0; \
    /* ??? could also mark other bits undefined here */ \
    }


IMPLEMENT_EFLAGS_ACCESSORS()
IMPLEMENT_EFLAG_ACCESSOR   (DF, 10)
IMPLEMENT_EFLAG_ACCESSOR   (ID, 21)
IMPLEMENT_EFLAG_ACCESSOR   (VP, 20)
IMPLEMENT_EFLAG_ACCESSOR   (VF, 19)
IMPLEMENT_EFLAG_ACCESSOR   (AC, 18)
IMPLEMENT_EFLAG_ACCESSOR_VM(    17)
IMPLEMENT_EFLAG_ACCESSOR   (RF, 16)
IMPLEMENT_EFLAG_ACCESSOR   (NT, 14)
IMPLEMENT_EFLAG_ACCESSOR_IOPL(  12)
IMPLEMENT_EFLAG_ACCESSOR   (IF,  9)
IMPLEMENT_EFLAG_ACCESSOR   (TF,  8)







#define BX_REPE_PREFIX  10
#define BX_REPNE_PREFIX 11



#define BX_TASK_FROM_JUMP         10
#define BX_TASK_FROM_CALL_OR_INT  11
#define BX_TASK_FROM_IRET         12


//
// For decoding...
//

// If the Immediate bit is set, the lowest 3 bits of the attribute
// specify which kinds of immediate data a required by instruction.

#define BxImmediate         0x000f // bits 3..0: any immediate
#define BxImmediate_Ib      0x0001 // 8 bits regardless
#define BxImmediate_Ib_SE   0x0002 // sign extend to OS size
#define BxImmediate_Iv      0x0003 // 16 or 32 depending on OS size
#define BxImmediate_Iw      0x0004 // 16 bits regardless
#define BxImmediate_IvIw    0x0005 // call_Ap
#define BxImmediate_IwIb    0x0006 // enter_IwIb
#define BxImmediate_O       0x0007 // mov_ALOb, mov_ObAL, mov_eAXOv, mov_OveAX
#define BxImmediate_BrOff8  0x0008 // Relative branch offset byte
#define BxImmediate_BrOff16 0x0009 // Relative branch offset word
#define BxImmediate_BrOff32 BxImmediate_Iv
#if BX_SUPPORT_X86_64
#define BxImmediate_Iq      0x000A // 64 bit override
#endif

#define BxPrefix          0x0010 // bit  4
#define BxAnother         0x0020 // bit  5
#define BxSplitMod11b     0x0040 // bit  6
#define BxRepeatable      0x0800 // bit 11 (pass through to metaInfo field)
#define BxRepeatableZF    0x1000 // bit 12 (pass through to metaInfo field)
#define BxGroupN          0x0100 // bits 8
#define BxGroup1          BxGroupN
#define BxGroup2          BxGroupN
#define BxGroup3          BxGroupN
#define BxGroup4          BxGroupN
#define BxGroup5          BxGroupN
#define BxGroup6          BxGroupN
#define BxGroup7          BxGroupN
#define BxGroup8          BxGroupN
#define BxGroup9          BxGroupN
#define BxGroupA          BxGroupN

#if BX_SUPPORT_FPU
#define BxAnotherFPU      BxAnother
#else
#define BxAnotherFPU      (0)
#endif

#if BX_SUPPORT_MMX
#define BxAnotherMMX      BxAnother
#else
#define BxAnotherMMX      (0)
#endif

#define BxGroup15         BxGroupN

#if BX_DEBUGGER
typedef enum _show_flags {
      Flag_call = 0x1,
      Flag_ret = 0x2,
      Flag_int = 0x4,
      Flag_iret = 0x8,
      Flag_intsig = 0x10
} show_flags_t;
#endif

// Can be used as LHS or RHS.
#define RMAddr(i)  (BX_CPU_THIS_PTR address_xlation.rm_addr)


#endif  // #ifndef BX_CPU_H
