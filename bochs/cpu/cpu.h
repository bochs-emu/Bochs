/////////////////////////////////////////////////////////////////////////
// $Id: cpu.h,v 1.217 2005-04-13 17:13:01 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#ifndef BX_CPU_H
#  define BX_CPU_H 1

#include <setjmp.h>

#include "cpu/lazy_flags.h"
#include "cpu/hostasm.h"

// <TAG-DEFINES-DECODE-START>
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
// <TAG-DEFINES-DECODE-END>


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

#define BX_64BIT_REG_RAX 0
#define BX_64BIT_REG_RCX 1
#define BX_64BIT_REG_RDX 2
#define BX_64BIT_REG_RBX 3
#define BX_64BIT_REG_RSP 4
#define BX_64BIT_REG_RBP 5
#define BX_64BIT_REG_RSI 6
#define BX_64BIT_REG_RDI 7

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
#define BX_READ_64BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].rrx)
#else
#define BX_READ_8BIT_REG(index)  (((index) < 4) ? \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl) : \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh))
#define BX_READ_8BIT_REGx(index,ext) BX_READ_8BIT_REG(index)
#endif

#define BX_READ_16BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].dword.erx)

#define BX_READ_16BIT_BASE_REG(var, index) {\
  var = *BX_CPU_THIS_PTR _16bit_base_reg[index];\
  }

#define BX_READ_16BIT_INDEX_REG(var, index) {\
  var = *BX_CPU_THIS_PTR _16bit_index_reg[index];\
  }

#define BX_WRITE_16BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.rx = val; \
  }

#define BX_WRITE_32BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = val; \
  }

#if BX_SUPPORT_X86_64

#define BX_WRITE_8BIT_REGx(index, extended, val) {\
  if (((index) < 4) || (extended)) \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
  }

#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = (Bit32u) val; \
  }

#define BX_WRITE_64BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = val; \
  }
#define BX_CLEAR_64BIT_HIGH(index) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.hrx = 0; \
  }

#else

#define BX_WRITE_8BIT_REG(index, val) {\
  if ((index) < 4) \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
  }
#define BX_WRITE_8BIT_REGx(index, ext, val) BX_WRITE_8BIT_REG(index, val)

// For x86-32, I just pretend this one is like the macro above,
// so common code can be used.
#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = (Bit32u) val; \
  }

#define BX_CLEAR_64BIT_HIGH(index)

#endif

#define CPL  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl)

#if BX_SMP_PROCESSORS==1
#define BX_CPU_ID 0
#else
#define BX_CPU_ID (BX_CPU_THIS_PTR bx_cpuid)
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
#define BX_XM_EXCEPTION  19

/* MSR registers */
#define BX_MSR_P5_MC_ADDR        0x0000
#define BX_MSR_MC_TYPE           0x0001
#define BX_MSR_TSC               0x0010
#define BX_MSR_CESR              0x0011
#define BX_MSR_CTR0              0x0012
#define BX_MSR_CTR1              0x0013
#define BX_MSR_APICBASE          0x001b
#define BX_MSR_EBL_CR_POWERON    0x002a
#define BX_MSR_TEST_CTL          0x0033
#define BX_MSR_BIOS_UPDT_TRIG    0x0079
#define BX_MSR_BBL_CR_D0         0x0088
#define BX_MSR_BBL_CR_D1         0x0089
#define BX_MSR_BBL_CR_D2         0x008a
#define BX_MSR_BBL_CR_D3         0x008b /* = BIOS_SIGN */
#define BX_MSR_PERFCTR0          0x00c1
#define BX_MSR_PERFCTR1          0x00c2
#define BX_MSR_MTRRCAP           0x00fe
#define BX_MSR_BBL_CR_ADDR       0x0116
#define BX_MSR_BBL_DECC          0x0118
#define BX_MSR_BBL_CR_CTL        0x0119
#define BX_MSR_BBL_CR_TRIG       0x011a
#define BX_MSR_BBL_CR_BUSY       0x011b
#define BX_MSR_BBL_CR_CTL3       0x011e
#if BX_SUPPORT_SEP
#  define BX_MSR_SYSENTER_CS  0x0174
#  define BX_MSR_SYSENTER_ESP 0x0175
#  define BX_MSR_SYSENTER_EIP 0x0176
#endif
#define BX_MSR_MCG_CAP           0x0179
#define BX_MSR_MCG_STATUS        0x017a
#define BX_MSR_MCG_CTL           0x017b
#define BX_MSR_EVNTSEL0          0x0186
#define BX_MSR_EVNTSEL1          0x0187
#define BX_MSR_DEBUGCTLMSR       0x01d9
#define BX_MSR_LASTBRANCHFROMIP  0x01db
#define BX_MSR_LASTBRANCHTOIP    0x01dc
#define BX_MSR_LASTINTOIP        0x01dd
#define BX_MSR_ROB_CR_BKUPTMPDR6 0x01e0
#define BX_MSR_MTRRPHYSBASE0     0x0200
#define BX_MSR_MTRRPHYSMASK0     0x0201
#define BX_MSR_MTRRPHYSBASE1     0x0202

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

#define BX_MODE_IA32_REAL       0x0   // CR0.PE=0
#define BX_MODE_IA32_V8086      0x1   // CR0.PE=1, EFLAGS.VM=1
#define BX_MODE_IA32_PROTECTED  0x2   // CR0.PE=1, EFLAGS.VM=0
#define BX_MODE_LONG_COMPAT     0x3   // EFER.LMA = 0, EFER.LME = 1, CR0.PE=1
#define BX_MODE_LONG_64         0x4   // EFER.LMA = 1, EFER.LME = 1, CR0.PE=1

#if BX_SUPPORT_X86_64
#define IsLongMode() (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
#else
#define IsLongMode() (0)
#endif

#if BX_SUPPORT_APIC
#define BX_CPU_INTR  (BX_CPU_THIS_PTR INTR || BX_CPU_THIS_PTR local_apic.INTR)
#else
#define BX_CPU_INTR  (BX_CPU_THIS_PTR INTR)
#endif

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
#  define BX_CPU_CALL_METHODR(func, args) \
            (this->*((BxExecutePtr_tR) (func))) args
#else
// static member functions.  With SMF, there is only one CPU by definition.
#  define BX_CPU_THIS_PTR  BX_CPU(0)->
#  define BX_CPU_THIS      BX_CPU(0)
#  define BX_SMF           static
#  define BX_CPU_C_PREFIX
#  define BX_CPU_CALL_METHOD(func, args) \
            ((BxExecutePtr_t) (func)) args
#  define BX_CPU_CALL_METHODR(func, args) \
            ((BxExecutePtr_tR) (func)) args
#endif

#if BX_SMP_PROCESSORS==1
// single processor simulation, so there's one of everything
BOCHSAPI extern BX_CPU_C       bx_cpu;
#else
// multiprocessor simulation, we need an array of cpus and memories
BOCHSAPI extern BX_CPU_C       *bx_cpu_array[BX_SMP_PROCESSORS];
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
  BX_CPP_INLINE void setEFlags(Bit32u val);

#define IMPLEMENT_EFLAGS_ACCESSORS()                                         \
  BX_CPP_INLINE void BX_CPU_C::setEFlags(Bit32u val) {                       \
    BX_CPU_THIS_PTR eflags.val32 = val;                                      \
    BX_CPU_THIS_PTR eflags.VM_cached = val & (1<<17);                        \
    if (BX_CPU_THIS_PTR cr0.pe && BX_CPU_THIS_PTR eflags.VM_cached) {        \
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_V8086;                         \
    }                                                                        \
  }

  // accessors for all eflags in bx_flags_reg_t
  // The macro is used once for each flag bit.
#define DECLARE_EFLAG_ACCESSOR(name,bitnum)                                  \
  BX_CPP_INLINE void    assert_##name ();                                    \
  BX_CPP_INLINE void    clear_##name ();                                     \
  BX_CPP_INLINE Bit32u  get_##name ();                                       \
  BX_CPP_INLINE bx_bool getB_##name ();                                      \
  BX_CPP_INLINE void    set_##name (Bit8u val);

#define IMPLEMENT_EFLAG_ACCESSOR(name,bitnum)                                \
  BX_CPP_INLINE void BX_CPU_C::assert_##name () {                            \
    BX_CPU_THIS_PTR eflags.val32 |= (1<<bitnum);                             \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::clear_##name () {                             \
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<bitnum);                            \
  }                                                                          \
  BX_CPP_INLINE bx_bool BX_CPU_C::getB_##name () {                           \
    return 1 & (BX_CPU_THIS_PTR eflags.val32 >> bitnum);                     \
  }                                                                          \
  BX_CPP_INLINE Bit32u  BX_CPU_C::get_##name () {                            \
    return BX_CPU_THIS_PTR eflags.val32 & (1 << bitnum);                     \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::set_##name (Bit8u val) {                      \
    BX_CPU_THIS_PTR eflags.val32 =                                           \
      (BX_CPU_THIS_PTR eflags.val32&~(1<<bitnum)) | ((!!val)<<bitnum);       \
  }

#define DECLARE_EFLAG_ACCESSOR_VM(bitnum)                                    \
  BX_CPP_INLINE void    assert_VM();                                         \
  BX_CPP_INLINE void    clear_VM();                                          \
  BX_CPP_INLINE Bit32u  get_VM();                                            \
  BX_CPP_INLINE bx_bool getB_VM();                                           \
  BX_CPP_INLINE void    set_VM(Bit32u val);

#define IMPLEMENT_EFLAG_ACCESSOR_VM(bitnum)                                  \
  BX_CPP_INLINE void BX_CPU_C::assert_VM() {                                 \
    BX_CPU_THIS_PTR eflags.val32 |= (1<<bitnum);                             \
    BX_CPU_THIS_PTR eflags.VM_cached = 1;                                    \
    if (BX_CPU_THIS_PTR cr0.pe) {                                            \
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_V8086;                         \
    }                                                                        \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::clear_VM() {                                  \
    if (BX_CPU_THIS_PTR eflags.VM_cached) {                                  \
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<bitnum);                          \
      BX_CPU_THIS_PTR eflags.VM_cached = 0;                                  \
      if (BX_CPU_THIS_PTR cr0.pe) {                                          \
        BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_PROTECTED;                   \
      }                                                                      \
    }                                                                        \
  }                                                                          \
  BX_CPP_INLINE Bit32u  BX_CPU_C::get_VM() {                                 \
    return BX_CPU_THIS_PTR eflags.VM_cached;                                 \
  }                                                                          \
  BX_CPP_INLINE bx_bool BX_CPU_C::getB_VM() {                                \
    return (BX_CPU_THIS_PTR eflags.VM_cached>0);                             \
  }                                                                          \
  BX_CPP_INLINE void BX_CPU_C::set_VM(Bit32u val) {                          \
    BX_CPU_THIS_PTR eflags.val32 =                                           \
      (BX_CPU_THIS_PTR eflags.val32&~(1<<bitnum)) | (val ? (1<<bitnum) : 0); \
    BX_CPU_THIS_PTR eflags.VM_cached = val;                                  \
    if (BX_CPU_THIS_PTR cr0.pe && BX_CPU_THIS_PTR eflags.VM_cached) {        \
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_V8086;                         \
    }                                                                        \
  }

#define DECLARE_EFLAG_ACCESSOR_IOPL(bitnum)                                  \
  BX_CPP_INLINE void set_IOPL(Bit32u val);                                   \
  BX_CPP_INLINE Bit32u  get_IOPL(void);

#define IMPLEMENT_EFLAG_ACCESSOR_IOPL(bitnum)                                \
  BX_CPP_INLINE void BX_CPU_C::set_IOPL(Bit32u val) {                        \
    BX_CPU_THIS_PTR eflags.val32 &= ~(3<<12);                                \
    BX_CPU_THIS_PTR eflags.val32 |= ((3&val) << 12);                         \
  }                                                                          \
  BX_CPP_INLINE Bit32u BX_CPU_C::get_IOPL() {                                \
    return 3 & (BX_CPU_THIS_PTR eflags.val32 >> 12);                         \
  }

#define EFlagsCFMask     0x00000001
#define EFlagsPFMask     0x00000004
#define EFlagsAFMask     0x00000010
#define EFlagsZFMask     0x00000040
#define EFlagsSFMask     0x00000080
#define EFlagsOFMask     0x00000800

#define EFlagsOSZAPCMask \
    (EFlagsCFMask | EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

#define EFlagsOSZAPMask  \
    (EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

  } bx_flags_reg_t;


#define DECLARE_8BIT_REGISTER_ACCESSORS(name)                                \
  BX_SMF BX_CPP_INLINE Bit8u get_##name(void);                               \
  BX_SMF BX_CPP_INLINE void  set_##name(Bit8u val)

#define DECLARE_16BIT_REGISTER_ACCESSORS(name)                               \
  BX_SMF BX_CPP_INLINE Bit16u get_##name(void);                              \
  BX_SMF BX_CPP_INLINE void   set_##name(Bit16u val)

#define DECLARE_32BIT_REGISTER_ACCESSORS(name)                               \
  BX_SMF BX_CPP_INLINE Bit32u get_##name(void);                              \
  BX_SMF BX_CPP_INLINE void   set_##name(Bit32u val)

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
  bx_bool pg; // paging
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
  bx_bool cd; // cache disable
  bx_bool nw; // no write-through
  bx_bool am; // alignment mask
  bx_bool wp; // write-protect
  bx_bool ne; // numerics exception
#endif

  bx_bool ts; // task switched
  bx_bool em; // emulate math coprocessor
  bx_bool mp; // monitor coprocessor
  bx_bool pe; // protected mode enable
  } bx_cr0_t;
#endif

#if BX_CPU_LEVEL >= 4
typedef struct {
  Bit32u  registerValue; // 32bit value of register

  // Accessors for all cr4 bitfields.
#define IMPLEMENT_CR4_ACCESSORS(name,bitnum)                                 \
  BX_CPP_INLINE bx_bool get_##name () {                                      \
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
  BX_CPP_INLINE Bit32u  getRegister() { return registerValue; }
  BX_CPP_INLINE void    setRegister(Bit32u r) { registerValue = r; }
  } bx_cr4_t;
#endif  // #if BX_CPU_LEVEL >= 4

#if BX_CPU_LEVEL >= 5
typedef struct {
  Bit64u apicbase;

#if BX_SUPPORT_X86_64
  // x86-64 EFER bits
  bx_bool sce;		// system call extensions
  bx_bool lme;		// long mode enable
  bx_bool lma;		// long mode active
  bx_bool nxe;		// no-execute enable
  bx_bool ffxsr;	// fast FXSAVE/FXRSTOR

  Bit64u star;
  Bit64u lstar;
  Bit64u cstar;
  Bit64u fmask;
  Bit64u kernelgsbase;
#endif

  /* TODO finish of the others */
} bx_regs_msr_t;
#endif

#include "descriptor.h"

typedef void * (*BxVoidFPtr_t)(void);

// <TAG-CLASS-INSTRUCTION-START>
class bxInstruction_c {
public:
  // Function pointers; a function to resolve the modRM address
  // given the current state of the CPU and the instruction data,
  // and a function to execute the instruction after resolving
  // the memory address (if any).
#if BX_USE_CPU_SMF
  void  ( BX_CPP_AttrRegparmN(1) *ResolveModrm )(bxInstruction_c *);
  void (*execute)(bxInstruction_c *);
#else
  void (BX_CPU_C::*ResolveModrm)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
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
  // want a bx_bool value, use os32B() etc.  This makes for smaller
  // code, when a strict 0 or 1 is not necessary.
  BX_CPP_INLINE void initMetaInfo(unsigned seg,
                                  unsigned os32, unsigned as32,
                                  unsigned os64, unsigned as64)
  {
    metaInfo = seg | (os32<<4) | (as32<<5) | (os64<<6) | (as64<<7);
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
// <TAG-CLASS-INSTRUCTION-END>


// <TAG-TYPE-EXECUTEPTR-START>
#if BX_USE_CPU_SMF
typedef void (*BxExecutePtr_t)(bxInstruction_c *);
typedef void (BX_CPP_AttrRegparmN(1) *BxExecutePtr_tR)(bxInstruction_c *);
#else
typedef void (BX_CPU_C::*BxExecutePtr_t)(bxInstruction_c *);
typedef void (BX_CPU_C::*BxExecutePtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
// <TAG-TYPE-EXECUTEPTR-END>


// ========== iCache =============================================
#if BX_SUPPORT_ICACHE
#include "icache.h"
#endif

#if BX_USE_TLB
typedef bx_ptr_equiv_t bx_hostpageaddr_t;

  typedef struct {
    bx_address lpf; // linear page frame
    Bit32u ppf; // physical page frame
    Bit32u accessBits; // Page Table Address for updating A & D bits
    bx_hostpageaddr_t hostPageAddr;
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

#include "cpu/apic.h"

class BX_MEM_C;

#if BX_SUPPORT_X86_64
# define BX_GENERAL_REGISTERS 16
#else
# define BX_GENERAL_REGISTERS 8
#endif

#if BX_SUPPORT_FPU
#include "cpu/i387.h"
#include "cpu/xmm.h"
#endif

class BOCHSAPI BX_CPU_C : public logfunctions {

public: // for now...

  char name[64];

  unsigned bx_cpuid;

  // General register set
  // eax: accumulator
  // ebx: base
  // ecx: count
  // edx: data
  // ebp: base pointer
  // esi: source index
  // edi: destination index
  // esp: stack pointer
  bx_gen_reg_t  gen_reg[BX_GENERAL_REGISTERS];

  // instruction pointer
#if BX_SUPPORT_X86_64
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
    union {
      Bit32u eip;    // instruction pointer
    } dword;
#endif

  // so that we can back up when handling faults, exceptions, etc.
  // we need to store the value of the instruction pointer, before
  // each fetch/execute cycle.
  bx_address prev_eip;
  bx_address prev_esp;

  // status and control flags register set
  Bit32u lf_flags_status;
  bx_flags_reg_t eflags;

  bx_lf_flags_entry oszapc;
  bx_lf_flags_entry oszap;

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
  bx_segment_reg_t        ldtr; /* local descriptor table register */
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
  bx_cr0_t      cr0;

  Bit32u        cr1;
  bx_address    cr2;
  bx_address    cr3;
  bx_address    cr3_masked;
#endif

#if BX_CPU_LEVEL >= 4
  bx_cr4_t cr4;
#endif

#if BX_CPU_LEVEL >= 5
  bx_regs_msr_t msr;  
#endif

#if BX_SUPPORT_FPU || BX_SUPPORT_MMX
  i387_t the_i387;
#endif

#if BX_SUPPORT_SSE
  bx_xmm_reg_t xmm[BX_XMM_REGISTERS];
  bx_mxcsr_t mxcsr;
#endif

  // pointer to the address space that this processor uses.
  BX_MEM_C *mem;

  bx_bool EXT; /* 1 if processing external interrupt or exception
                * or if not related to current instruction,
                * 0 if current CS:IP caused exception */
  unsigned errorno;   /* signal exception during instruction emulation */

  Bit32u   debug_trap; // holds DR6 value to be set as well
  volatile bx_bool async_event;
  volatile bx_bool INTR;
  volatile bx_bool kill_bochs_request;

  /* wether this CPU is the BSP always set for UP */
  bx_bool bsp;
  // for accessing registers by index number
  Bit16u *_16bit_base_reg[8];
  Bit16u *_16bit_index_reg[8];
  Bit32u empty_register;

// <TAG-CLASS-CPU-START>
  // for decoding instructions; accessing seg reg's by index
  unsigned sreg_mod00_rm16[8];
  unsigned sreg_mod01or10_rm16[8];
#if BX_SUPPORT_X86_64
  unsigned sreg_mod01or10_rm32[16];
  unsigned sreg_mod0_base32[16];
  unsigned sreg_mod1or2_base32[16];
#else
  unsigned sreg_mod01or10_rm32[8];
  unsigned sreg_mod0_base32[8];
  unsigned sreg_mod1or2_base32[8];
#endif
// <TAG-CLASS-CPU-END>

  // for exceptions
  jmp_buf jmp_buf_env;
  Bit8u curr_exception[2];

  bx_segment_reg_t save_cs;
  bx_segment_reg_t save_ss;
  Bit32u           save_eip;
  Bit32u           save_esp;
  // This help for OS/2
  bx_bool          except_chk;
  Bit16u           except_cs;
  Bit16u           except_ss;

  // Boundaries of current page, based on EIP
  bx_address eipPageBias;
  bx_address eipPageWindowSize;
  Bit8u     *eipFetchPtr;
  Bit32u     pAddrA20Page; // Guest physical address of current instruction
                           // page with A20() already applied.
  unsigned cpu_mode;

#if BX_DEBUGGER
  Bit32u watchpoint;
  Bit8u break_point;
#if BX_MAGIC_BREAKPOINT
  Bit8u magic_break;
#endif
  Bit8u stop_reason;
  Bit8u trace;
  Bit8u trace_reg;
  Bit8u mode_break; /* BW */
  bx_bool debug_vm; /* BW contains current mode*/
  Bit8u show_eip;   /* BW record eip at special instr f.ex eip */
  Bit8u show_flag;  /* BW shows instr class executed */
  bx_guard_found_t guard_found;
#endif

#if BX_GDBSTUB
  Bit8u ispanic;
#endif

#if BX_SUPPORT_X86_64
#define TLB_GENERATION_MAX (BX_TLB_SIZE-1)
#endif

  // SYSENTER/SYSEXIT instruction msr's
#if BX_SUPPORT_SEP
  Bit32u sysenter_cs_msr;
  Bit32u sysenter_esp_msr;
  Bit32u sysenter_eip_msr;
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
#if BX_SUPPORT_ICACHE
  bxICache_c iCache  BX_CPP_AlignN(32);
#endif


  struct {
    bx_address  rm_addr; // The address offset after resolution.
    Bit32u  paddress1;  // physical address after translation of 1st len1 bytes of data
    Bit32u  paddress2;  // physical address after translation of 2nd len2 bytes of data
    Bit32u  len1;       // Number of bytes in page 1
    Bit32u  len2;       // Number of bytes in page 2
    bx_ptr_equiv_t pages;      // Number of pages access spans (1 or 2).  Also used
                        //   for the case when a native host pointer is
                        //   available for the R-M-W instructions.  The host
                        //   pointer is stuffed here.  Since this field has
                        //   to be checked anyways (and thus cached), if it
                        //   is greated than 2 (the maximum possible for
                        //   normal cases) it is a native pointer and is used
                        //   for a direct write access.
    } address_xlation;

#if BX_EXTERNAL_DEBUGGER
  virtual void ask (int level, const char *prefix, const char *fmt, va_list ap);
#endif

#define ArithmeticalFlag(flag, lfMaskShift, eflagsBitShift) \
  BX_SMF bx_bool get_##flag##Lazy(void); \
  BX_SMF bx_bool getB_##flag(void) { \
    if ( (BX_CPU_THIS_PTR lf_flags_status & (0xf<<lfMaskShift)) == 0) \
      return (BX_CPU_THIS_PTR eflags.val32 >> eflagsBitShift) & 1; \
    else \
      return get_##flag##Lazy(); \
    } \
  BX_SMF bx_bool get_##flag(void) { \
    if ( (BX_CPU_THIS_PTR lf_flags_status & (0xf<<lfMaskShift)) == 0) \
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

  void set_cpu_id(unsigned id);

  // constructors & destructors...
  BX_CPU_C();
#if BX_EXTERNAL_DEBUGGER
  virtual ~BX_CPU_C();
#else
  ~BX_CPU_C();
#endif
  void init (BX_MEM_C *addrspace);

// <TAG-CLASS-CPU-START>
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
  BX_SMF void ARPL_EwGw(bxInstruction_c *);
  BX_SMF void PUSH_Id(bxInstruction_c *);
  BX_SMF void PUSH_Iw(bxInstruction_c *);
  BX_SMF void INSB_YbDX(bxInstruction_c *);
  BX_SMF void INSW_YvDX(bxInstruction_c *);
  BX_SMF void OUTSB_DXXb(bxInstruction_c *);
  BX_SMF void OUTSW_DXXv(bxInstruction_c *);

  BX_SMF void BOUND_GwMa(bxInstruction_c *);
  BX_SMF void BOUND_GdMa(bxInstruction_c *);

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

  BX_SMF void CBW(bxInstruction_c *);
  BX_SMF void CWD(bxInstruction_c *);
  BX_SMF void CALL32_Ap(bxInstruction_c *);
  BX_SMF void CALL16_Ap(bxInstruction_c *);
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
  BX_SMF void TEST_ALIb(bxInstruction_c *);
  BX_SMF void TEST_EAXId(bxInstruction_c *);
  BX_SMF void TEST_AXIw(bxInstruction_c *);

  // repeatable instructions
  BX_SMF void MOVSB_XbYb(bxInstruction_c *);
  BX_SMF void MOVSW_XwYw(bxInstruction_c *);
  BX_SMF void MOVSD_XdYd(bxInstruction_c *);
  BX_SMF void CMPSB_XbYb(bxInstruction_c *);
  BX_SMF void CMPSW_XwYw(bxInstruction_c *);
  BX_SMF void CMPSD_XdYd(bxInstruction_c *);
  BX_SMF void STOSB_YbAL(bxInstruction_c *);
  BX_SMF void LODSB_ALXb(bxInstruction_c *);
  BX_SMF void SCASB_ALXb(bxInstruction_c *);
  BX_SMF void STOSW_YwAX(bxInstruction_c *);
  BX_SMF void LODSW_AXXw(bxInstruction_c *);
  BX_SMF void SCASW_AXXw(bxInstruction_c *);
  BX_SMF void STOSD_YdEAX(bxInstruction_c *);
  BX_SMF void LODSD_EAXXd(bxInstruction_c *);
  BX_SMF void SCASD_EAXXd(bxInstruction_c *);

  BX_SMF void RETnear32(bxInstruction_c *);
  BX_SMF void RETnear16(bxInstruction_c *);
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

  BX_SMF void SHRD_EwGw(bxInstruction_c *);
  BX_SMF void SHRD_EdGd(bxInstruction_c *);
  BX_SMF void SHLD_EdGd(bxInstruction_c *);
  BX_SMF void SHLD_EwGw(bxInstruction_c *);

  BX_SMF void BSF_GwEw(bxInstruction_c *);
  BX_SMF void BSF_GdEd(bxInstruction_c *);
  BX_SMF void BSR_GwEw(bxInstruction_c *);
  BX_SMF void BSR_GdEd(bxInstruction_c *);

  BX_SMF void BT_EwGw(bxInstruction_c *);
  BX_SMF void BT_EdGd(bxInstruction_c *);
  BX_SMF void BTS_EwGw(bxInstruction_c *);
  BX_SMF void BTS_EdGd(bxInstruction_c *);
  BX_SMF void BTR_EwGw(bxInstruction_c *);
  BX_SMF void BTR_EdGd(bxInstruction_c *);
  BX_SMF void BTC_EwGw(bxInstruction_c *);
  BX_SMF void BTC_EdGd(bxInstruction_c *);

  BX_SMF void LES_GvMp(bxInstruction_c *);
  BX_SMF void LDS_GvMp(bxInstruction_c *);
  BX_SMF void LSS_GvMp(bxInstruction_c *);
  BX_SMF void LFS_GvMp(bxInstruction_c *);
  BX_SMF void LGS_GvMp(bxInstruction_c *);

  BX_SMF void MOVZX_GdEb(bxInstruction_c *);
  BX_SMF void MOVZX_GwEb(bxInstruction_c *);
  BX_SMF void MOVZX_GdEw(bxInstruction_c *);
  BX_SMF void MOVZX_GwEw(bxInstruction_c *);
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
  BX_SMF void IMUL_GdEd(bxInstruction_c *);
  BX_SMF void IMUL_ALEb(bxInstruction_c *);
  BX_SMF void IMUL_GdEdId(bxInstruction_c *);
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

#if BX_SUPPORT_FPU == 0	// if FPU is disabled
  BX_SMF void FPU_ESC(bxInstruction_c *);
#endif

  BX_SMF void FWAIT(bxInstruction_c *);

#if BX_SUPPORT_FPU
  // load/store
  BX_SMF void FLD_STi(bxInstruction_c *);  
  BX_SMF void FLD_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FLD_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FLD_EXTENDED_REAL(bxInstruction_c *);
  BX_SMF void FILD_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FILD_DWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FILD_QWORD_INTEGER(bxInstruction_c *);  
  BX_SMF void FBLD_PACKED_BCD(bxInstruction_c *);

  BX_SMF void FST_STi(bxInstruction_c *);
  BX_SMF void FST_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FST_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FSTP_EXTENDED_REAL(bxInstruction_c *);
  BX_SMF void FIST_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FIST_DWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FISTP_QWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FBSTP_PACKED_BCD(bxInstruction_c *);

  BX_SMF void FISTTP16(bxInstruction_c *);
  BX_SMF void FISTTP32(bxInstruction_c *);
  BX_SMF void FISTTP64(bxInstruction_c *);

  // control
  BX_SMF void FNINIT(bxInstruction_c *);
  BX_SMF void FNCLEX(bxInstruction_c *);

  BX_SMF void FRSTOR(bxInstruction_c *);
  BX_SMF void FNSAVE(bxInstruction_c *);
  BX_SMF void FLDENV(bxInstruction_c *);
  BX_SMF void FNSTENV(bxInstruction_c *);

  BX_SMF void FLDCW(bxInstruction_c *);
  BX_SMF void FNSTCW(bxInstruction_c *);
  BX_SMF void FNSTSW(bxInstruction_c *);
  BX_SMF void FNSTSW_AX(bxInstruction_c *);

  // const
  BX_SMF void FLD1(bxInstruction_c *); 
  BX_SMF void FLDL2T(bxInstruction_c *);
  BX_SMF void FLDL2E(bxInstruction_c *);
  BX_SMF void FLDPI(bxInstruction_c *);
  BX_SMF void FLDLG2(bxInstruction_c *);
  BX_SMF void FLDLN2(bxInstruction_c *);
  BX_SMF void FLDZ(bxInstruction_c *);                        

  // add
  BX_SMF void FADD_ST0_STj(bxInstruction_c *);
  BX_SMF void FADD_STi_ST0(bxInstruction_c *);
  BX_SMF void FADD_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FADD_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FIADD_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FIADD_DWORD_INTEGER(bxInstruction_c *);

  // mul
  BX_SMF void FMUL_ST0_STj(bxInstruction_c *);
  BX_SMF void FMUL_STi_ST0(bxInstruction_c *);
  BX_SMF void FMUL_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FMUL_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FIMUL_WORD_INTEGER (bxInstruction_c *);
  BX_SMF void FIMUL_DWORD_INTEGER(bxInstruction_c *);

  // sub
  BX_SMF void FSUB_ST0_STj(bxInstruction_c *);
  BX_SMF void FSUBR_ST0_STj(bxInstruction_c *);
  BX_SMF void FSUB_STi_ST0(bxInstruction_c *);
  BX_SMF void FSUBR_STi_ST0(bxInstruction_c *);
  BX_SMF void FSUB_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FSUBR_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FSUB_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FSUBR_DOUBLE_REAL(bxInstruction_c *);

  BX_SMF void FISUB_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FISUBR_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FISUB_DWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FISUBR_DWORD_INTEGER(bxInstruction_c *);

  // div
  BX_SMF void FDIV_ST0_STj(bxInstruction_c *);
  BX_SMF void FDIVR_ST0_STj(bxInstruction_c *);
  BX_SMF void FDIV_STi_ST0(bxInstruction_c *);
  BX_SMF void FDIVR_STi_ST0(bxInstruction_c *);
  BX_SMF void FDIV_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FDIVR_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FDIV_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FDIVR_DOUBLE_REAL(bxInstruction_c *);

  BX_SMF void FIDIV_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FIDIVR_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FIDIV_DWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FIDIVR_DWORD_INTEGER(bxInstruction_c *);

  // compare
  BX_SMF void FCOM_STi(bxInstruction_c *);
  BX_SMF void FUCOM_STi(bxInstruction_c *);
  BX_SMF void FCOMI_ST0_STj(bxInstruction_c *);
  BX_SMF void FUCOMI_ST0_STj(bxInstruction_c *);
  BX_SMF void FCOM_SINGLE_REAL(bxInstruction_c *);
  BX_SMF void FCOM_DOUBLE_REAL(bxInstruction_c *);
  BX_SMF void FICOM_WORD_INTEGER(bxInstruction_c *);
  BX_SMF void FICOM_DWORD_INTEGER(bxInstruction_c *);
  BX_SMF void FCMOV_ST0_STj(bxInstruction_c *);

  BX_SMF void FCOMPP(bxInstruction_c *);  
  BX_SMF void FUCOMPP(bxInstruction_c *);

  // misc
  BX_SMF void FXCH_STi(bxInstruction_c *);
  BX_SMF void FNOP(bxInstruction_c *);
  BX_SMF void FPLEGACY(bxInstruction_c *);
  BX_SMF void FCHS(bxInstruction_c *);
  BX_SMF void FABS(bxInstruction_c *);
  BX_SMF void FTST(bxInstruction_c *);
  BX_SMF void FXAM(bxInstruction_c *);
  BX_SMF void FDECSTP(bxInstruction_c *);
  BX_SMF void FINCSTP(bxInstruction_c *);
  BX_SMF void FFREE_STi(bxInstruction_c *);
  BX_SMF void FFREEP_STi(bxInstruction_c *);

  BX_SMF void F2XM1(bxInstruction_c *);
  BX_SMF void FYL2X(bxInstruction_c *);
  BX_SMF void FPTAN(bxInstruction_c *);
  BX_SMF void FPATAN(bxInstruction_c *);
  BX_SMF void FXTRACT(bxInstruction_c *);
  BX_SMF void FPREM1(bxInstruction_c *);
  BX_SMF void FPREM(bxInstruction_c *);
  BX_SMF void FYL2XP1(bxInstruction_c *);
  BX_SMF void FSQRT(bxInstruction_c *);
  BX_SMF void FSINCOS(bxInstruction_c *);
  BX_SMF void FRNDINT(bxInstruction_c *);
#undef FSCALE            // <sys/param.h> is #included on Mac OS X from bochs.h
  BX_SMF void FSCALE(bxInstruction_c *);
  BX_SMF void FSIN(bxInstruction_c *);
  BX_SMF void FCOS(bxInstruction_c *);
#endif

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

#if BX_SUPPORT_3DNOW
  BX_SMF void PFPNACC_PqQq(bxInstruction_c *i);
  BX_SMF void PI2FW_PqQq(bxInstruction_c *i);
  BX_SMF void PI2FD_PqQq(bxInstruction_c *i);
  BX_SMF void PF2IW_PqQq(bxInstruction_c *i);
  BX_SMF void PF2ID_PqQq(bxInstruction_c *i);
  BX_SMF void PFNACC_PqQq(bxInstruction_c *i);
  BX_SMF void PFCMPGE_PqQq(bxInstruction_c *i);
  BX_SMF void PFMIN_PqQq(bxInstruction_c *i);
  BX_SMF void PFRCP_PqQq(bxInstruction_c *i);
  BX_SMF void PFRSQRT_PqQq(bxInstruction_c *i);
  BX_SMF void PFSUB_PqQq(bxInstruction_c *i);
  BX_SMF void PFADD_PqQq(bxInstruction_c *i);
  BX_SMF void PFCMPGT_PqQq(bxInstruction_c *i);
  BX_SMF void PFMAX_PqQq(bxInstruction_c *i);
  BX_SMF void PFRCPIT1_PqQq(bxInstruction_c *i);
  BX_SMF void PFRSQIT1_PqQq(bxInstruction_c *i);
  BX_SMF void PFSUBR_PqQq(bxInstruction_c *i);
  BX_SMF void PFACC_PqQq(bxInstruction_c *i);
  BX_SMF void PFCMPEQ_PqQq(bxInstruction_c *i);
  BX_SMF void PFMUL_PqQq(bxInstruction_c *i);
  BX_SMF void PFRCPIT2_PqQq(bxInstruction_c *i);
  BX_SMF void PMULHRW_PqQq(bxInstruction_c *i);
  BX_SMF void PSWAPD_PqQq(bxInstruction_c *i);
#endif

  /* SSE */
  BX_SMF void FXSAVE(bxInstruction_c *i);
  BX_SMF void FXRSTOR(bxInstruction_c *i);
  BX_SMF void LDMXCSR(bxInstruction_c *i);
  BX_SMF void STMXCSR(bxInstruction_c *i);
  BX_SMF void PREFETCH(bxInstruction_c *i);
  /* SSE */

  /* SSE */
  BX_SMF void MOVUPS_VpsWps(bxInstruction_c *i);
  BX_SMF void MOVSS_VssWss(bxInstruction_c *i);
  BX_SMF void MOVUPS_WpsVps(bxInstruction_c *i);
  BX_SMF void MOVSS_WssVss(bxInstruction_c *i);
  BX_SMF void MOVLPS_VpsMq(bxInstruction_c *i);
  BX_SMF void MOVLPS_MqVps(bxInstruction_c *i);
  BX_SMF void MOVHPS_VpsMq(bxInstruction_c *i);
  BX_SMF void MOVHPS_MqVps(bxInstruction_c *i);
  BX_SMF void MOVAPS_VpsWps(bxInstruction_c *i);
  BX_SMF void MOVAPS_WpsVps(bxInstruction_c *i);
  BX_SMF void CVTPI2PS_VpsQq(bxInstruction_c *i);
  BX_SMF void CVTSI2SS_VssEd(bxInstruction_c *i);
  BX_SMF void MOVNTPS_MdqVps(bxInstruction_c *i);
  BX_SMF void CVTTPS2PI_PqWps(bxInstruction_c *i);
  BX_SMF void CVTTSS2SI_GdWss(bxInstruction_c *i);
  BX_SMF void CVTPS2PI_PqWps(bxInstruction_c *i);
  BX_SMF void CVTSS2SI_GdWss(bxInstruction_c *i);
  BX_SMF void UCOMISS_VssWss(bxInstruction_c *i);
  BX_SMF void COMISS_VpsWps(bxInstruction_c *i);
  BX_SMF void MOVMSKPS_GdVRps(bxInstruction_c *i);
  BX_SMF void SQRTPS_VpsWps(bxInstruction_c *i);
  BX_SMF void SQRTSS_VssWss(bxInstruction_c *i);
  BX_SMF void RSQRTPS_VpsWps(bxInstruction_c *i);
  BX_SMF void RSQRTSS_VssWss(bxInstruction_c *i);
  BX_SMF void RCPPS_VpsWps(bxInstruction_c *i);
  BX_SMF void RCPSS_VssWss(bxInstruction_c *i);
  BX_SMF void ADDPS_VpsWps(bxInstruction_c *i);
  BX_SMF void ADDSS_VssWss(bxInstruction_c *i);
  BX_SMF void MULPS_VpsWps(bxInstruction_c *i);
  BX_SMF void MULSS_VssWss(bxInstruction_c *i);
  BX_SMF void SUBPS_VpsWps(bxInstruction_c *i);
  BX_SMF void SUBSS_VssWss(bxInstruction_c *i);
  BX_SMF void MINPS_VpsWps(bxInstruction_c *i);
  BX_SMF void MINSS_VssWss(bxInstruction_c *i);
  BX_SMF void DIVPS_VpsWps(bxInstruction_c *i);
  BX_SMF void DIVSS_VssWss(bxInstruction_c *i);
  BX_SMF void MAXPS_VpsWps(bxInstruction_c *i);
  BX_SMF void MAXSS_VssWss(bxInstruction_c *i);
  BX_SMF void PSHUFW_PqQqIb(bxInstruction_c *i);
  BX_SMF void PSHUFLW_VqWqIb(bxInstruction_c *i);
  BX_SMF void CMPPS_VpsWpsIb(bxInstruction_c *i);
  BX_SMF void CMPSS_VssWssIb(bxInstruction_c *i);
  BX_SMF void PINSRW_PqEdIb(bxInstruction_c *i);
  BX_SMF void PEXTRW_PqEdIb(bxInstruction_c *i);
  BX_SMF void SHUFPS_VpsWpsIb(bxInstruction_c *i);
  BX_SMF void PMOVMSKB_GdPRq(bxInstruction_c *i);
  BX_SMF void PMINUB_PqQq(bxInstruction_c *i);
  BX_SMF void PMAXUB_PqQq(bxInstruction_c *i);
  BX_SMF void PAVGB_PqQq(bxInstruction_c *i);
  BX_SMF void PAVGW_PqQq(bxInstruction_c *i);
  BX_SMF void PMULHUW_PqQq(bxInstruction_c *i);
  BX_SMF void MOVNTQ_MqPq(bxInstruction_c *i);
  BX_SMF void PMINSW_PqQq(bxInstruction_c *i);
  BX_SMF void PMAXSW_PqQq(bxInstruction_c *i);
  BX_SMF void PSADBW_PqQq(bxInstruction_c *i);
  BX_SMF void MASKMOVQ_PqPRq(bxInstruction_c *i);
  /* SSE */

  /* SSE2 */
  BX_SMF void MOVSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void MOVSD_WsdVsd(bxInstruction_c *i);
  BX_SMF void CVTPI2PD_VpdQd(bxInstruction_c *i);
  BX_SMF void CVTSI2SD_VsdEd(bxInstruction_c *i);
  BX_SMF void CVTTPD2PI_PqWpd(bxInstruction_c *i);
  BX_SMF void CVTTSD2SI_GdWsd(bxInstruction_c *i);
  BX_SMF void CVTPD2PI_PqWpd(bxInstruction_c *i);
  BX_SMF void CVTSD2SI_GdWsd(bxInstruction_c *i);
  BX_SMF void UCOMISD_VsdWsd(bxInstruction_c *i);            	
  BX_SMF void COMISD_VpdWpd(bxInstruction_c *i);   
  BX_SMF void MOVMSKPD_EdVRpd(bxInstruction_c *i);
  BX_SMF void SQRTPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void SQRTSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void ADDPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void ADDSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void MULPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void MULSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void CVTPS2PD_VpsWps(bxInstruction_c *i);
  BX_SMF void CVTPD2PS_VpdWpd(bxInstruction_c *i);
  BX_SMF void CVTSD2SS_VsdWsd(bxInstruction_c *i);
  BX_SMF void CVTSS2SD_VssWss(bxInstruction_c *i);
  BX_SMF void CVTDQ2PS_VpsWdq(bxInstruction_c *i);
  BX_SMF void CVTPS2DQ_VdqWps(bxInstruction_c *i);
  BX_SMF void CVTTPS2DQ_VdqWps(bxInstruction_c *i);
  BX_SMF void SUBPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void SUBSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void MINPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void MINSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void DIVPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void DIVSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void MAXPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void MAXSD_VsdWsd(bxInstruction_c *i);
  BX_SMF void PUNPCKLBW_VdqWq(bxInstruction_c *i);
  BX_SMF void PUNPCKLWD_VdqWq(bxInstruction_c *i);
  BX_SMF void PUNPCKLDQ_VdqWq(bxInstruction_c *i);
  BX_SMF void PACKSSWB_VdqWq(bxInstruction_c *i);
  BX_SMF void PCMPGTB_VdqWq(bxInstruction_c *i);
  BX_SMF void PCMPGTW_VdqWq(bxInstruction_c *i);
  BX_SMF void PCMPGTD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PACKUSWB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PUNPCKHBW_VdqWq(bxInstruction_c *i);
  BX_SMF void PUNPCKHWD_VdqWq(bxInstruction_c *i);
  BX_SMF void PUNPCKHDQ_VdqWq(bxInstruction_c *i);
  BX_SMF void PACKSSDW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PUNPCKLQDQ_VdqWq(bxInstruction_c *i);
  BX_SMF void PUNPCKHQDQ_VdqWq(bxInstruction_c *i);
  BX_SMF void MOVD_VdqEd(bxInstruction_c *i);
  BX_SMF void PSHUFD_VdqWdqIb(bxInstruction_c *i);
  BX_SMF void PSHUFHW_VqWqIb(bxInstruction_c *i);
  BX_SMF void PCMPEQB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PCMPEQW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PCMPEQD_VdqWdq(bxInstruction_c *i);
  BX_SMF void MOVD_EdVd(bxInstruction_c *i);
  BX_SMF void MOVQ_VqWq(bxInstruction_c *i);
  BX_SMF void CMPPD_VpdWpdIb(bxInstruction_c *i);
  BX_SMF void CMPSD_VsdWsdIb(bxInstruction_c *i);
  BX_SMF void MOVNTI_MdGd(bxInstruction_c *i);
  BX_SMF void PINSRW_VdqEdIb(bxInstruction_c *i);
  BX_SMF void PEXTRW_VdqEdIb(bxInstruction_c *i);
  BX_SMF void SHUFPD_VpdWpdIb(bxInstruction_c *i);
  BX_SMF void PSRLW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSRLD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSRLQ_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDQ_PqQq(bxInstruction_c *i);
  BX_SMF void PADDQ_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMULLW_VdqWdq(bxInstruction_c *i);
  BX_SMF void MOVQ_WqVq(bxInstruction_c *i);
  BX_SMF void MOVDQ2Q_PqVRq(bxInstruction_c *i);
  BX_SMF void MOVQ2DQ_VdqQq(bxInstruction_c *i);
  BX_SMF void PMOVMSKB_GdVRdq(bxInstruction_c *i);
  BX_SMF void PSUBUSB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSUBUSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMINUB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PAND_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDUSB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDUSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMAXUB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PANDN_VdqWdq(bxInstruction_c *i);
  BX_SMF void PAVGB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSRAW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSRAD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PAVGW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMULHUW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMULHW_VdqWdq(bxInstruction_c *i);
  BX_SMF void CVTTPD2DQ_VqWpd(bxInstruction_c *i);
  BX_SMF void CVTPD2DQ_VqWpd(bxInstruction_c *i);
  BX_SMF void CVTDQ2PD_VpdWq(bxInstruction_c *i);
  BX_SMF void PSUBSB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSUBSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMINSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void POR_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDSB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMAXSW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PXOR_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSLLW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSLLD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSLLQ_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMULUDQ_PqQq(bxInstruction_c *i);
  BX_SMF void PMULUDQ_VdqWdq(bxInstruction_c *i);
  BX_SMF void PMADDWD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSADBW_VdqWdq(bxInstruction_c *i);
  BX_SMF void MASKMOVDQU_VdqVRdq(bxInstruction_c *i);
  BX_SMF void PSUBB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSUBW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSUBD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSUBQ_PqQq(bxInstruction_c *i);
  BX_SMF void PSUBQ_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDB_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDW_VdqWdq(bxInstruction_c *i);
  BX_SMF void PADDD_VdqWdq(bxInstruction_c *i);
  BX_SMF void PSRLW_PdqIb(bxInstruction_c *i);
  BX_SMF void PSRAW_PdqIb(bxInstruction_c *i);
  BX_SMF void PSLLW_PdqIb(bxInstruction_c *i);
  BX_SMF void PSRLD_PdqIb(bxInstruction_c *i);
  BX_SMF void PSRAD_PdqIb(bxInstruction_c *i);
  BX_SMF void PSLLD_PdqIb(bxInstruction_c *i);
  BX_SMF void PSRLQ_PdqIb(bxInstruction_c *i);
  BX_SMF void PSRLDQ_WdqIb(bxInstruction_c *i);
  BX_SMF void PSLLQ_PdqIb(bxInstruction_c *i);
  BX_SMF void PSLLDQ_WdqIb(bxInstruction_c *i);
  /* SSE2 */

  /*** Duplicate SSE/SSE2 instructions ***/
  // Although in implementation, these instructions are aliased to the
  // another function, it's nice to have them call a seperate function when
  // the decoder is being tested in stand-alone mode.
#ifdef StandAloneDecoder
  BX_SMF void MOVUPD_VpdWpd(bxInstruction_c *);
  BX_SMF void MOVUPD_WpdVpd(bxInstruction_c *);
  BX_SMF void MOVAPD_VpdWpd(bxInstruction_c *);
  BX_SMF void MOVAPD_WpdVpd(bxInstruction_c *);
  BX_SMF void MOVDQU_VdqWdq(bxInstruction_c *);
  BX_SMF void MOVDQU_WdqVdq(bxInstruction_c *);
  BX_SMF void MOVDQA_VdqWdq(bxInstruction_c *);
  BX_SMF void MOVDQA_WdqVdq(bxInstruction_c *);
  BX_SMF void UNPCKHPS_VpsWq(bxInstruction_c *);
  BX_SMF void UNPCKLPS_VpsWq(bxInstruction_c *);
  BX_SMF void ANDPS_VpsWps(bxInstruction_c *);
  BX_SMF void ANDPD_VpdWpd(bxInstruction_c *);
  BX_SMF void ANDNPS_VpsWps(bxInstruction_c *);
  BX_SMF void ANDNPD_VpdWpd(bxInstruction_c *);
  BX_SMF void ORPS_VpsWps(bxInstruction_c *);
  BX_SMF void ORPD_VpdWpd(bxInstruction_c *);
  BX_SMF void XORPS_VpsWps(bxInstruction_c *);
  BX_SMF void XORPD_VpdWpd(bxInstruction_c *);
  BX_SMF void UNPCKHPD_VpdWq(bxInstruction_c *);
  BX_SMF void UNPCKLPD_VpdWq(bxInstruction_c *);
  BX_SMF void MOVLPD_VsdMq(bxInstruction_c *);
  BX_SMF void MOVLPD_MqVsd(bxInstruction_c *);
  BX_SMF void MOVHPD_VsdMq(bxInstruction_c *);
  BX_SMF void MOVHPD_MqVsd(bxInstruction_c *);
  BX_SMF void MOVNTPD_MdqVpd(bxInstruction_c *);
  BX_SMF void MOVNTDQ_MdqVdq(bxInstruction_c *);
#else
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
#endif  // #ifdef StandAloneDecoder

  /* PNI */
  BX_SMF void MOVDDUP_VpdWq(bxInstruction_c *i);
  BX_SMF void MOVSLDUP_VpsWps(bxInstruction_c *i);
  BX_SMF void MOVSHDUP_VpsWps(bxInstruction_c *i);
  BX_SMF void HADDPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void HADDPS_VpsWps(bxInstruction_c *i);
  BX_SMF void HSUBPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void HSUBPS_VpsWps(bxInstruction_c *i);
  BX_SMF void ADDSUBPD_VpdWpd(bxInstruction_c *i);
  BX_SMF void ADDSUBPS_VpsWps(bxInstruction_c *i);
  BX_SMF void LDDQU_VdqMdq(bxInstruction_c *i);
  /* PNI */

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

#if BX_CPU_LEVEL == 2
  BX_SMF void LOADALL(bxInstruction_c *);
#endif

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
  BX_SMF void ADC_EqGq(bxInstruction_c *);
  BX_SMF void ADC_GqEq(bxInstruction_c *);
  BX_SMF void ADC_RAXId(bxInstruction_c *);
  BX_SMF void SUB_EqGq(bxInstruction_c *);
  BX_SMF void SUB_GqEq(bxInstruction_c *);
  BX_SMF void SUB_RAXId(bxInstruction_c *);
  BX_SMF void SBB_EqGq(bxInstruction_c *);
  BX_SMF void SBB_GqEq(bxInstruction_c *);
  BX_SMF void SBB_RAXId(bxInstruction_c *);

  BX_SMF void OR_EqGq(bxInstruction_c *);
  BX_SMF void OR_GqEq(bxInstruction_c *);
  BX_SMF void OR_RAXId(bxInstruction_c *);

  BX_SMF void AND_EqGq(bxInstruction_c *);
  BX_SMF void AND_GqEq(bxInstruction_c *);
  BX_SMF void AND_RAXId(bxInstruction_c *);

  BX_SMF void XOR_EqGq(bxInstruction_c *);
  BX_SMF void XOR_GqEq(bxInstruction_c *);
  BX_SMF void XOR_RAXId(bxInstruction_c *);

  BX_SMF void CMP_EqGq(bxInstruction_c *);
  BX_SMF void CMP_GqEq(bxInstruction_c *);
  BX_SMF void CMP_RAXId(bxInstruction_c *);

  BX_SMF void PUSH64_Id(bxInstruction_c *);

  BX_SMF void TEST_RAXId(bxInstruction_c *);
  BX_SMF void TEST_EqGq(bxInstruction_c *);
  BX_SMF void XCHG_EqGq(bxInstruction_c *);
  BX_SMF void LEA_GqM(bxInstruction_c *);

  BX_SMF void MOV_RAXOq(bxInstruction_c *);
  BX_SMF void MOV_OqRAX(bxInstruction_c *);
  BX_SMF void MOV_EAXOq(bxInstruction_c *);
  BX_SMF void MOV_OqEAX(bxInstruction_c *);
  BX_SMF void MOV_AXOq(bxInstruction_c *);
  BX_SMF void MOV_OqAX(bxInstruction_c *);
  BX_SMF void MOV_ALOq(bxInstruction_c *);
  BX_SMF void MOV_OqAL(bxInstruction_c *);
  BX_SMF void MOV_EqGq(bxInstruction_c *);
  BX_SMF void MOV_GqEq(bxInstruction_c *);
  BX_SMF void MOV_EqId(bxInstruction_c *);

  BX_SMF void MOVSQ_XqYq(bxInstruction_c *);
  BX_SMF void CMPSQ_XqYq(bxInstruction_c *);

  BX_SMF void STOSQ_YqRAX(bxInstruction_c *);
  BX_SMF void LODSQ_RAXXq(bxInstruction_c *);
  BX_SMF void SCASQ_RAXXq(bxInstruction_c *);

  BX_SMF void RETnear64(bxInstruction_c *);
  BX_SMF void ENTER64_IwIb(bxInstruction_c *);
  BX_SMF void LEAVE64(bxInstruction_c *);
  BX_SMF void RETfar64(bxInstruction_c *);

  BX_SMF void IRET64(bxInstruction_c *);

  BX_SMF void CALL_Aq(bxInstruction_c *);
  BX_SMF void JMP_Jq(bxInstruction_c *);

  BX_SMF void MOV_CqRq(bxInstruction_c *);
  BX_SMF void MOV_DqRq(bxInstruction_c *);
  BX_SMF void MOV_RqCq(bxInstruction_c *);
  BX_SMF void MOV_RqDq(bxInstruction_c *);

  BX_SMF void JCC_Jq(bxInstruction_c *);

  BX_SMF void SHLD_EqGq(bxInstruction_c *);
  BX_SMF void SHRD_EqGq(bxInstruction_c *);
  BX_SMF void IMUL_GqEq(bxInstruction_c *);
  BX_SMF void IMUL_GqEqId(bxInstruction_c *);

  BX_SMF void MOVZX_GqEb(bxInstruction_c *);
  BX_SMF void MOVZX_GqEw(bxInstruction_c *);
  BX_SMF void MOVSX_GqEb(bxInstruction_c *);
  BX_SMF void MOVSX_GqEw(bxInstruction_c *);
  BX_SMF void MOVSX_GqEd(bxInstruction_c *);

  BX_SMF void BSF_GqEq(bxInstruction_c *);
  BX_SMF void BSR_GqEq(bxInstruction_c *);

  BX_SMF void BT_EqGq(bxInstruction_c *);
  BX_SMF void BTS_EqGq(bxInstruction_c *);
  BX_SMF void BTR_EqGq(bxInstruction_c *);
  BX_SMF void BTC_EqGq(bxInstruction_c *);

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
  BX_SMF void PUSH_RRX(bxInstruction_c *);
  BX_SMF void POP_RRX(bxInstruction_c *);
  BX_SMF void POP_Eq(bxInstruction_c *);
  BX_SMF void XCHG_RRXRAX(bxInstruction_c *);

  BX_SMF void PUSH64_FS(bxInstruction_c *);
  BX_SMF void POP64_FS(bxInstruction_c *);
  BX_SMF void PUSH64_GS(bxInstruction_c *);
  BX_SMF void POP64_GS(bxInstruction_c *);

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
  BX_SMF void RSM(bxInstruction_c *);

  BX_SMF void WRMSR(bxInstruction_c *);
  BX_SMF void RDTSC(bxInstruction_c *);
  BX_SMF void RDPMC(bxInstruction_c *);
  BX_SMF void RDMSR(bxInstruction_c *);
  BX_SMF void SYSENTER(bxInstruction_c *);
  BX_SMF void SYSEXIT(bxInstruction_c *);

  BX_SMF unsigned fetchDecode(Bit8u *, bxInstruction_c *, unsigned);
#if BX_SUPPORT_X86_64
  BX_SMF unsigned fetchDecode64(Bit8u *, bxInstruction_c *, unsigned);
#endif
  BX_SMF void UndefinedOpcode(bxInstruction_c *);
  BX_SMF void BxError(bxInstruction_c *i);

  BX_SMF void Resolve16Mod0Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod0Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve16Mod1or2Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve16Mod1or2Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve32Mod0Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve32Mod1or2Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve32Mod0Base0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod0Base7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve32Mod1or2Base0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve32Mod1or2Base7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_X86_64
  // 64 bit addressing
  BX_SMF void Resolve64Mod0Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm8(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm9(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm10(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm11(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm12(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm13(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm14(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Rm15(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve64Mod1or2Rm0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm8(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm9(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm10(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm11(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm12(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm13(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm14(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Rm15(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve64Mod0Base0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base8(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base9(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base10(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base11(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base12(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base13(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base14(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod0Base15(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void Resolve64Mod1or2Base0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base5(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base6(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base7(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base8(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base9(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base10(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base11(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base12(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base13(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base14(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void Resolve64Mod1or2Base15(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif  // #if BX_SUPPORT_X86_64
  BX_SMF void BxResolveError(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
// <TAG-CLASS-CPU-END>

#if BX_DEBUGGER
  BX_SMF void     dbg_take_irq(void);
  BX_SMF void     dbg_force_interrupt(unsigned vector);
  BX_SMF void     dbg_take_dma(void);
  BX_SMF bx_bool  dbg_get_cpu(bx_dbg_cpu_t *cpu);
  BX_SMF bx_bool  dbg_set_cpu(bx_dbg_cpu_t *cpu);
  BX_SMF bx_bool  dbg_set_reg(unsigned reg, Bit32u val);
  BX_SMF Bit32u   dbg_get_reg(unsigned reg);
  BX_SMF bx_bool  dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no);
  BX_SMF unsigned dbg_query_pending(void);
  BX_SMF Bit32u   dbg_get_descriptor_l(bx_descriptor_t *);
  BX_SMF Bit32u   dbg_get_descriptor_h(bx_descriptor_t *);
  BX_SMF Bit32u   dbg_get_eflags(void);
  BX_SMF bx_bool  dbg_is_begin_instr_bpoint(Bit32u cs, Bit32u eip, Bit32u laddr,
                                            Bit32u is_32);
  BX_SMF bx_bool  dbg_is_end_instr_bpoint(Bit32u cs, Bit32u eip,
                                          Bit32u laddr, Bit32u is_32);
#endif
#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION || BX_GDBSTUB
  BX_SMF void     dbg_xlate_linear2phy(bx_address linear, Bit32u *phy, bx_bool *valid);
#endif
  BX_SMF void     atexit(void);
  
  // now for some ancillary functions...
  BX_SMF void cpu_loop(Bit32s max_instr_count);
  BX_SMF unsigned handleAsyncEvent(void);
  BX_SMF void boundaryFetch(Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *i);
  BX_SMF void branch_near32(Bit32u new_eip) BX_CPP_AttrRegparmN(1);

  BX_SMF void prefetch(void);
  // revalidate_prefetch_q is now a no-op, due to the newer EIP window
  // technique.
  BX_SMF BX_CPP_INLINE void revalidate_prefetch_q(void) { }
  BX_SMF BX_CPP_INLINE void invalidate_prefetch_q(void) 
  {
    BX_CPU_THIS_PTR eipPageWindowSize = 0;
  }

  BX_SMF void write_virtual_checks(bx_segment_reg_t *seg, bx_address offset, unsigned length) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_checks(bx_segment_reg_t *seg, bx_address offset, unsigned length) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_byte(unsigned seg, bx_address offset, Bit8u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_word(unsigned seg, bx_address offset, Bit16u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dword(unsigned seg, bx_address offset, Bit32u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_qword(unsigned seg, bx_address offset, Bit64u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dqword(unsigned s, bx_address off, Bit8u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dqword_aligned(unsigned s, bx_address off, Bit8u *data) BX_CPP_AttrRegparmN(3);
#if BX_SUPPORT_FPU
  BX_SMF void write_virtual_tword(unsigned seg, bx_address offset, floatx80 *data) BX_CPP_AttrRegparmN(3);
#endif
  BX_SMF void read_virtual_byte(unsigned seg, bx_address offset, Bit8u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_word(unsigned seg, bx_address offset, Bit16u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_dword(unsigned seg, bx_address offset, Bit32u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_qword(unsigned seg, bx_address offset, Bit64u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_dqword(unsigned s, bx_address off, Bit8u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_dqword_aligned(unsigned s, bx_address off, Bit8u *data) BX_CPP_AttrRegparmN(3);
#if BX_SUPPORT_FPU
  BX_SMF void read_virtual_tword(unsigned seg, bx_address offset, floatx80 *data) BX_CPP_AttrRegparmN(3);
#endif

#define readVirtualDQword(s, off, data) read_virtual_dqword(s, off, data)
#define readVirtualDQwordAligned(s, off, data) read_virtual_dqword_aligned(s, off, data)
#define writeVirtualDQword(s, off, data) write_virtual_dqword(s, off, data)
#define writeVirtualDQwordAligned(s, off, data) write_virtual_dqword_aligned(s, off, data)

  BX_SMF void read_RMW_virtual_byte(unsigned seg, bx_address offset, Bit8u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_RMW_virtual_word(unsigned seg, bx_address offset, Bit16u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_RMW_virtual_dword(unsigned seg, bx_address offset, Bit32u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_RMW_virtual_qword(unsigned seg, bx_address offset, Bit64u *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_RMW_virtual_byte(Bit8u val8) BX_CPP_AttrRegparmN(1);
  BX_SMF void write_RMW_virtual_word(Bit16u val16) BX_CPP_AttrRegparmN(1);
  BX_SMF void write_RMW_virtual_dword(Bit32u val32);
  BX_SMF void write_RMW_virtual_qword(Bit64u val64);

#define Write_RMW_virtual_byte(val8)   write_RMW_virtual_byte(val8)
#define Write_RMW_virtual_word(val16)  write_RMW_virtual_word(val16)
#define Write_RMW_virtual_dword(val32) write_RMW_virtual_dword(val32)
#define Write_RMW_virtual_qword(val64) write_RMW_virtual_qword(val64)

  BX_SMF void access_linear(bx_address address, unsigned length, unsigned pl,
                     unsigned rw, void *data) BX_CPP_AttrRegparmN(3);
  BX_SMF Bit32u  translate_linear(bx_address laddr, 
     unsigned pl, unsigned rw, unsigned access_type) BX_CPP_AttrRegparmN(3);
  BX_SMF Bit32u itranslate_linear(bx_address laddr, unsigned pl) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u dtranslate_linear(bx_address laddr, unsigned pl, unsigned rw) BX_CPP_AttrRegparmN(3);
  BX_SMF void TLB_flush(bx_bool invalidateGlobal);
  BX_SMF void TLB_init(void);
  BX_SMF void set_INTR(bx_bool value);
  BX_SMF char *strseg(bx_segment_reg_t *seg) BX_CPP_AttrRegparmN(1);
  BX_SMF void interrupt(Bit8u vector, bx_bool is_INT, bx_bool is_error_code,
                 Bit16u error_code);
  BX_SMF void real_mode_int(Bit8u vector, bx_bool is_INT, bx_bool is_error_code,
                 Bit16u error_code);
  BX_SMF void protected_mode_int(Bit8u vector, bx_bool is_INT, bx_bool is_error_code,
                 Bit16u error_code);
#if BX_SUPPORT_X86_64
  BX_SMF void long_mode_int(Bit8u vector, bx_bool is_INT, bx_bool is_error_code,
                 Bit16u error_code);
#endif
#if BX_CPU_LEVEL >= 2
  BX_SMF void exception(unsigned vector, Bit16u error_code, bx_bool is_INT)
                  BX_CPP_AttrNoReturn();
#endif
  BX_SMF int  int_number(bx_segment_reg_t *seg);
  BX_SMF void CR3_change(bx_address value) BX_CPP_AttrRegparmN(1);
  BX_SMF void pagingCR0Changed(Bit32u oldCR0, Bit32u newCR0) BX_CPP_AttrRegparmN(2);
  BX_SMF void pagingCR4Changed(Bit32u oldCR4, Bit32u newCR4) BX_CPP_AttrRegparmN(2);
  BX_SMF void pagingA20Changed(void);

  BX_SMF void reset(unsigned source);

  BX_SMF void jump_protected(bxInstruction_c *, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);
  BX_SMF void call_protected(bxInstruction_c *, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);
  BX_SMF void return_protected(bxInstruction_c *, Bit16u pop_bytes) BX_CPP_AttrRegparmN(2);
  BX_SMF void iret_protected(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void validate_seg_regs(void);
  BX_SMF void stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector,
                                     Bit32u flags32);
  BX_SMF void stack_return_from_v86(bxInstruction_c *);
#if BX_SUPPORT_VME
  BX_SMF void v86_redirect_interrupt(Bit32u vector);
#endif
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
  BX_SMF void write_flags(Bit16u flags, bx_bool change_IOPL, bx_bool change_IF) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_eflags(Bit32u eflags, bx_bool change_IOPL, bx_bool change_IF,
                    bx_bool change_VM, bx_bool change_RF);
  BX_SMF void writeEFlags(Bit32u eflags, Bit32u changeMask) BX_CPP_AttrRegparmN(2); // Newer variant.
#if BX_SUPPORT_FPU || BX_SUPPORT_SSE >= 1
  BX_SMF void write_eflags_fpu_compare(int float_relation);
#endif
  BX_SMF Bit16u read_flags(void);
  BX_SMF Bit32u read_eflags(void);

  BX_SMF Bit8u   inp8(Bit16u addr) BX_CPP_AttrRegparmN(1);
  BX_SMF void    outp8(Bit16u addr, Bit8u value) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u  inp16(Bit16u addr) BX_CPP_AttrRegparmN(1);
  BX_SMF void    outp16(Bit16u addr, Bit16u value) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u  inp32(Bit16u addr) BX_CPP_AttrRegparmN(1);
  BX_SMF void    outp32(Bit16u addr, Bit32u value) BX_CPP_AttrRegparmN(2);
  BX_SMF bx_bool allow_io(Bit16u addr, unsigned len);
  BX_SMF void    enter_protected_mode(void);
  BX_SMF void    enter_real_mode(void);
  BX_SMF void    parse_selector(Bit16u raw_selector, bx_selector_t *selector) BX_CPP_AttrRegparmN(2);
  BX_SMF void    parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp) BX_CPP_AttrRegparmN(3);
  BX_SMF void    load_ldtr(bx_selector_t *selector, bx_descriptor_t *descriptor);
  BX_SMF void    load_cs(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  BX_SMF void    load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  BX_SMF void    fetch_raw_descriptor(bx_selector_t *selector,
                               Bit32u *dword1, Bit32u *dword2, Bit8u exception) BX_CPP_AttrRegparmN(3);
  BX_SMF void    load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value) BX_CPP_AttrRegparmN(2);
#if BX_SUPPORT_X86_64
  BX_SMF void    load_ss64(Bit8u cpl) BX_CPP_AttrRegparmN(1);
  BX_SMF void    loadSRegLMNominal(unsigned seg, unsigned selector,
                                   bx_address base, unsigned dpl);
#endif
  BX_SMF bx_bool fetch_raw_descriptor2(bx_selector_t *selector,
                                Bit32u *dword1, Bit32u *dword2) BX_CPP_AttrRegparmN(3);
  BX_SMF void    push_16(Bit16u value16) BX_CPP_AttrRegparmN(1);
  BX_SMF void    push_32(Bit32u value32);
#if BX_SUPPORT_X86_64
  BX_SMF void    push_64(Bit64u value64);
#endif
  BX_SMF void    pop_16(Bit16u *value16_ptr);
  BX_SMF void    pop_32(Bit32u *value32_ptr);
#if BX_SUPPORT_X86_64
  BX_SMF void    pop_64(Bit64u *value64_ptr);
#endif
  BX_SMF bx_bool can_push(bx_descriptor_t *descriptor, Bit32u esp, Bit32u bytes) BX_CPP_AttrRegparmN(3);
  BX_SMF bx_bool can_pop(Bit32u bytes);
  BX_SMF void    decrementESPForPush(unsigned nBytes, Bit32u *eSP);
  BX_SMF void    sanity_checks(void);

  BX_SMF void    debug(bx_address offset);

#if BX_EXTERNAL_DEBUGGER
  BX_SMF void    trap_debugger(bx_bool callnow);
#endif

#if BX_X86_DEBUGGER
  // x86 hardware debug support
  BX_SMF Bit32u  hwdebug_compare(Bit32u laddr, unsigned size,
                                 unsigned opa, unsigned opb);
#endif

  BX_SMF Bit32u get_cpu_version_information(void);
  BX_SMF Bit32u get_extended_cpuid_features(void);
  BX_SMF Bit32u get_std_cpuid_features(void);

  BX_CPP_INLINE unsigned which_cpu(void) { return bx_cpuid; }
  BX_CPP_INLINE const bx_gen_reg_t *get_gen_reg() { return gen_reg; }

  DECLARE_EFLAGS_ACCESSORS()
  DECLARE_EFLAG_ACCESSOR   (DF,  10)
  DECLARE_EFLAG_ACCESSOR   (ID,  21)
  DECLARE_EFLAG_ACCESSOR   (VIP, 20)
  DECLARE_EFLAG_ACCESSOR   (VIF, 19)
  DECLARE_EFLAG_ACCESSOR   (AC,  18)
  DECLARE_EFLAG_ACCESSOR_VM(     17)
  DECLARE_EFLAG_ACCESSOR   (RF,  16)
  DECLARE_EFLAG_ACCESSOR   (NT,  14)
  DECLARE_EFLAG_ACCESSOR_IOPL(   12)
  DECLARE_EFLAG_ACCESSOR   (IF,   9)
  DECLARE_EFLAG_ACCESSOR   (TF,   8)

  BX_SMF BX_CPP_INLINE void set_CF(bx_bool val);
  BX_SMF BX_CPP_INLINE void set_AF(bx_bool val);
  BX_SMF BX_CPP_INLINE void set_ZF(bx_bool val);
  BX_SMF BX_CPP_INLINE void set_SF(bx_bool val);
  BX_SMF BX_CPP_INLINE void set_OF(bx_bool val);
  BX_SMF BX_CPP_INLINE void set_PF(bx_bool val);
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

  BX_SMF BX_CPP_INLINE bx_address get_segment_base(unsigned seg);

  BX_SMF BX_CPP_INLINE bx_bool real_mode(void);
  BX_SMF BX_CPP_INLINE bx_bool protected_mode(void);
  BX_SMF BX_CPP_INLINE bx_bool v8086_mode(void);

#if BX_SUPPORT_FPU
  BX_SMF void print_state_FPU(void);
  BX_SMF void prepareFPU(bxInstruction_c *i, bx_bool = 1, bx_bool = 1);
  BX_SMF void FPU_check_pending_exceptions(void);
  BX_SMF void FPU_stack_underflow(int stnr, int pop_stack = 0);
  BX_SMF void FPU_stack_overflow(void);
  BX_SMF int  FPU_exception(int exception);
#endif

#if BX_SUPPORT_MMX || BX_SUPPORT_SSE
  BX_SMF void prepareMMX(void);
  BX_SMF void prepareFPU2MMX(void); /* cause transition from FPU to MMX technology state */
  BX_SMF void print_state_MMX(void);
#endif

#if BX_SUPPORT_SSE
  BX_SMF void prepareSSE(void);
  BX_SMF void check_exceptionsSSE(int);
#endif

#if BX_SUPPORT_FPU
  BX_SMF int  fpu_save_environment(bxInstruction_c *i);
  BX_SMF int  fpu_load_environment(bxInstruction_c *i);
#endif

  BX_SMF void wait_for_interrupt();

  BX_SMF void SetCR0(Bit32u val_32);
#if BX_CPU_LEVEL >= 4
  BX_SMF void SetCR4(Bit32u val_32);
#endif

#if BX_SUPPORT_APIC
  bx_local_apic_c local_apic;
#endif

};

#if BX_SUPPORT_ICACHE

BX_CPP_INLINE Bit32u createFetchModeMask(BX_CPU_C *cpu)
{
  return (cpu->sregs[BX_SEG_REG_CS].cache.u.segment.d_b << 31)
#if BX_SUPPORT_X86_64
         | ((cpu->cpu_mode == BX_MODE_LONG_64)<<30)
#endif
         | (1<<29); // iCache code.
}

#endif


#if BX_X86_DEBUGGER
#define BX_HWDebugInstruction   0x00
#define BX_HWDebugMemW          0x01
#define BX_HWDebugIO            0x02
#define BX_HWDebugMemRW         0x03
#endif

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

BX_CPP_INLINE void BX_CPU_C::set_cpu_id(unsigned id)
{
   BX_CPU_THIS_PTR bx_cpuid = id;
}

BX_SMF BX_CPP_INLINE Bit8u BX_CPU_C_PREFIX get_CPL(void)
{ 
   return (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl); 
} 

BX_CPP_INLINE Bit32u BX_CPU_C::get_EIP(void)
{
   return (BX_CPU_THIS_PTR dword.eip); 
} 

BX_CPP_INLINE bx_address BX_CPU_C::get_segment_base(unsigned seg)
{
#if BX_SUPPORT_X86_64
   if (IsLongMode() && seg != BX_SEG_REG_FS && seg != BX_SEG_REG_GS)
       return 0;
#endif
   return (BX_CPU_THIS_PTR sregs[seg].cache.u.segment.base);
}

BX_CPP_INLINE bx_bool BX_CPU_C::real_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode == BX_MODE_IA32_REAL);
}

BX_CPP_INLINE bx_bool BX_CPU_C::v8086_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode == BX_MODE_IA32_V8086);
}

BX_CPP_INLINE bx_bool BX_CPU_C::protected_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode >= BX_MODE_IA32_PROTECTED);
}

    BX_CPP_INLINE void
BX_CPU_C::set_CF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xfffff0;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<0);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val);
    }

    BX_CPP_INLINE void
BX_CPU_C::set_AF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xfff0ff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<4);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<4;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_ZF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xff0fff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<6);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<6;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_SF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xf0ffff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<7);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<7;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_OF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0x0fffff;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<11);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<11;
    }

    BX_CPP_INLINE void
BX_CPU_C::set_PF(bx_bool val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
    BX_CPU_THIS_PTR eflags.val32 |= (!!val)<<2;
    }

BOCHSAPI extern const bx_bool bx_parity_lookup[256];

    BX_CPP_INLINE void
BX_CPU_C::set_PF_base(Bit8u val) {
    BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
    val = bx_parity_lookup[val]; // Always returns 0 or 1.
    BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
    BX_CPU_THIS_PTR eflags.val32 |= val<<2;
    }

// *******************
// OSZAPC
// *******************

/* op1, op2, result */
#define SET_FLAGS_OSZAPC_SIZE(size, lf_op1, lf_op2, lf_result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1##size = lf_op1; \
    BX_CPU_THIS_PTR oszapc.op2##size = lf_op2; \
    BX_CPU_THIS_PTR oszapc.result##size = lf_result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
}

#define SET_FLAGS_OSZAPC_8(op1, op2, result, ins) \
    SET_FLAGS_OSZAPC_SIZE(_8, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_16(op1, op2, result, ins) \
    SET_FLAGS_OSZAPC_SIZE(_16, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_32(op1, op2, result, ins) \
    SET_FLAGS_OSZAPC_SIZE(_32, op1, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_64(op1, op2, result, ins) \
    SET_FLAGS_OSZAPC_SIZE(_64, op1, op2, result, ins)
#endif

/* op1 and result only */
#define SET_FLAGS_OSZAPC_S1_SIZE(size, lf_op1, lf_result, ins) { \
    BX_CPU_THIS_PTR oszapc.op1##size = lf_op1; \
    BX_CPU_THIS_PTR oszapc.result##size = lf_result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
}

#define SET_FLAGS_OSZAPC_S1_8(op1, result, ins) \
    SET_FLAGS_OSZAPC_S1_SIZE(_8, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_16(op1, result, ins) \
    SET_FLAGS_OSZAPC_S1_SIZE(_16, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_32(op1, result, ins) \
    SET_FLAGS_OSZAPC_S1_SIZE(_32, op1, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S1_64(op1, result, ins) \
    SET_FLAGS_OSZAPC_S1_SIZE(_64, op1, result, ins)
#endif

/* op2 and result only */
#define SET_FLAGS_OSZAPC_S2_SIZE(size, lf_op2, lf_result, ins) { \
    BX_CPU_THIS_PTR oszapc.op2##size = lf_op2; \
    BX_CPU_THIS_PTR oszapc.result##size = lf_result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
}

#define SET_FLAGS_OSZAPC_S2_8(op2, result, ins) \
    SET_FLAGS_OSZAPC_S2_SIZE(_8, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_16(op2, result, ins) \
    SET_FLAGS_OSZAPC_S2_SIZE(_16, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_32(op2, result, ins) \
    SET_FLAGS_OSZAPC_S2_SIZE(_32, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S2_64(op2, result, ins) \
    SET_FLAGS_OSZAPC_S2_SIZE(_64, op2, result, ins)
#endif

/* op1 and op2 only */
#define SET_FLAGS_OSZAPC_S1S2_SIZE(size, lf_op1, lf_op2, ins) { \
    BX_CPU_THIS_PTR oszapc.op1##size = lf_op1; \
    BX_CPU_THIS_PTR oszapc.op2##size = lf_op2; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
}

#define SET_FLAGS_OSZAPC_S1S2_8(op1, op2, ins) \
    SET_FLAGS_OSZAPC_S1S2_SIZE(_8, op1, op2, ins)
#define SET_FLAGS_OSZAPC_S1S2_16(op1, op2, ins) \
    SET_FLAGS_OSZAPC_S1S2_SIZE(_16, op1, op2, ins)
#define SET_FLAGS_OSZAPC_S1S2_32(op1, op2, ins) \
    SET_FLAGS_OSZAPC_S1S2_SIZE(_32, op1, op2, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S1S2_64(op1, op2, ins) \
    SET_FLAGS_OSZAPC_S1S2_SIZE(_64, op1, op2, ins)
#endif

/* result only */
#define SET_FLAGS_OSZAPC_RESULT_SIZE(size, lf_result, ins) { \
    BX_CPU_THIS_PTR oszapc.result##size = lf_result; \
    BX_CPU_THIS_PTR oszapc.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = BX_LF_MASK_OSZAPC; \
}

#define SET_FLAGS_OSZAPC_RESULT_8(result, ins) \
    SET_FLAGS_OSZAPC_RESULT_SIZE(_8, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_16(result, ins) \
    SET_FLAGS_OSZAPC_RESULT_SIZE(_16, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_32(result, ins) \
    SET_FLAGS_OSZAPC_RESULT_SIZE(_32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_RESULT_64(result, ins) \
    SET_FLAGS_OSZAPC_RESULT_SIZE(_64, result, ins)
#endif

// *******************
// OSZAP
// *******************

/* op1, op2, result */
#define SET_FLAGS_OSZAP_SIZE(size, lf_op1, lf_op2, lf_result, ins) { \
    BX_CPU_THIS_PTR oszap.op1##size = lf_op1; \
    BX_CPU_THIS_PTR oszap.op2##size = lf_op2; \
    BX_CPU_THIS_PTR oszap.result##size = lf_result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
}

#define SET_FLAGS_OSZAP_8(op1, op2, result, ins) \
    SET_FLAGS_OSZAP_SIZE(_8, op1, op2, result, ins)
#define SET_FLAGS_OSZAP_16(op1, op2, result, ins) \
    SET_FLAGS_OSZAP_SIZE(_16, op1, op2, result, ins)
#define SET_FLAGS_OSZAP_32(op1, op2, result, ins) \
    SET_FLAGS_OSZAP_SIZE(_32, op1, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_64(op1, op2, result, ins) \
    SET_FLAGS_OSZAP_SIZE(_64, op1, op2, result, ins)
#endif

/* result only */
#define SET_FLAGS_OSZAP_RESULT_SIZE(size, lf_result, ins) { \
    BX_CPU_THIS_PTR oszap.result##size = lf_result; \
    BX_CPU_THIS_PTR oszap.instr = ins; \
    BX_CPU_THIS_PTR lf_flags_status = (BX_CPU_THIS_PTR lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP; \
}

#define SET_FLAGS_OSZAP_RESULT_8(result, ins) \
    SET_FLAGS_OSZAP_RESULT_SIZE(_8, result, ins)
#define SET_FLAGS_OSZAP_RESULT_16(result, ins) \
    SET_FLAGS_OSZAP_RESULT_SIZE(_16, result, ins)
#define SET_FLAGS_OSZAP_RESULT_32(result, ins) \
    SET_FLAGS_OSZAP_RESULT_SIZE(_32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_RESULT_64(result, ins) \
    SET_FLAGS_OSZAP_RESULT_SIZE(_64, result, ins)
#endif

IMPLEMENT_EFLAGS_ACCESSORS()
IMPLEMENT_EFLAG_ACCESSOR   (DF,  10)
IMPLEMENT_EFLAG_ACCESSOR   (ID,  21)
IMPLEMENT_EFLAG_ACCESSOR   (VIP, 20)
IMPLEMENT_EFLAG_ACCESSOR   (VIF, 19)
IMPLEMENT_EFLAG_ACCESSOR   (AC,  18)
IMPLEMENT_EFLAG_ACCESSOR_VM(     17)
IMPLEMENT_EFLAG_ACCESSOR   (RF,  16)
IMPLEMENT_EFLAG_ACCESSOR   (NT,  14)
IMPLEMENT_EFLAG_ACCESSOR_IOPL(   12)
IMPLEMENT_EFLAG_ACCESSOR   (IF,   9)
IMPLEMENT_EFLAG_ACCESSOR   (TF,   8)

// <TAG-DEFINES-DECODE-START>
//
// For decoding...
//

#define BX_REPE_PREFIX  10
#define BX_REPNE_PREFIX 11

#define BX_TASK_FROM_JUMP         10
#define BX_TASK_FROM_CALL_OR_INT  11
#define BX_TASK_FROM_IRET         12

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

// Lookup for opcode and attributes in another opcode tables
// Totally 7 opcode groups supported
#define BxGroupX            0x0070 // bits 6..4: opcode groups definition
#define BxGroupN            0x0010 // Group encoding: 001
#define BxPrefixSSE         0x0020 // Group encoding: 010
#define BxSplitMod11b       0x0030 // Group encoding: 011
#define BxFPGroup           0x0040 // Group encoding: 100

#define BxPrefix            0x0080 // bit  7
#define BxAnother           0x0100 // bit  8
#define BxLockable          0x0200 // bit  9

#define BxRepeatable        0x0800 // bit 11 (pass through to metaInfo field)
#define BxRepeatableZF      0x1000 // bit 12 (pass through to metaInfo field)

#define BxGroup1          BxGroupN
#define BxGroup2          BxGroupN
#define BxGroup3          BxGroupN
#define BxGroup4          BxGroupN
#define BxGroup5          BxGroupN
#define BxGroup6          BxGroupN
#define BxGroup7          BxGroupN
#define BxGroup8          BxGroupN
#define BxGroup9          BxGroupN

#define BxGroup12         BxGroupN
#define BxGroup13         BxGroupN
#define BxGroup14         BxGroupN
#define BxGroup15         BxGroupN
#define BxGroup16         BxGroupN
// <TAG-DEFINES-DECODE-END>

#if BX_DEBUGGER
typedef enum _show_flags {
      Flag_call   = 0x1,
      Flag_ret    = 0x2,
      Flag_int    = 0x4,
      Flag_iret   = 0x8,
      Flag_intsig = 0x10
} show_flags_t;
#endif

// Can be used as LHS or RHS.
#define RMAddr(i)  (BX_CPU_THIS_PTR address_xlation.rm_addr)

#define setEFlagsOSZAPC(flags32) {                       \
  BX_CPU_THIS_PTR eflags.val32 =                         \
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPCMask) | \
    (flags32 & EFlagsOSZAPCMask);                        \
  BX_CPU_THIS_PTR lf_flags_status = 0;                   \
  }

#define setEFlagsOSZAP(flags32) {                        \
  BX_CPU_THIS_PTR eflags.val32 =                         \
    (BX_CPU_THIS_PTR eflags.val32 & ~EFlagsOSZAPMask) |  \
    (flags32 & EFlagsOSZAPMask);                         \
  BX_CPU_THIS_PTR lf_flags_status &= 0x00000f;           \
  }

#define SET_FLAGS_OxxxxC(new_of, new_cf) { \
    BX_CPU_THIS_PTR eflags.val32 &= ~((1<<11) | (1<<0)); \
    BX_CPU_THIS_PTR eflags.val32 |= ((!!new_of)<<11) | ((!!new_cf)<<0); \
    BX_CPU_THIS_PTR lf_flags_status &= 0x0ffff0; \
    }

#endif  // #ifndef BX_CPU_H
