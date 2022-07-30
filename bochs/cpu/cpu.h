/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2020  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_CPU_H
#define BX_CPU_H

#include <setjmp.h>

#include "bx_debug/debug.h"

#include "decoder/decoder.h"

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

#define TMP8L (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].word.byte.rl)

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
#define IP (BX_CPU_THIS_PTR gen_reg[BX_16BIT_REG_IP].word.rx)

#define TMP16 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].word.rx)

// accesss to 32 bit general registers
#define EAX (BX_CPU_THIS_PTR gen_reg[0].dword.erx)
#define ECX (BX_CPU_THIS_PTR gen_reg[1].dword.erx)
#define EDX (BX_CPU_THIS_PTR gen_reg[2].dword.erx)
#define EBX (BX_CPU_THIS_PTR gen_reg[3].dword.erx)
#define ESP (BX_CPU_THIS_PTR gen_reg[4].dword.erx)
#define EBP (BX_CPU_THIS_PTR gen_reg[5].dword.erx)
#define ESI (BX_CPU_THIS_PTR gen_reg[6].dword.erx)
#define EDI (BX_CPU_THIS_PTR gen_reg[7].dword.erx)

// access to 32 bit instruction pointer
#define EIP (BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_EIP].dword.erx)

#define TMP32 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].dword.erx)

#if BX_SUPPORT_X86_64

// accesss to 64 bit general registers
#define RAX (BX_CPU_THIS_PTR gen_reg[0].rrx)
#define RCX (BX_CPU_THIS_PTR gen_reg[1].rrx)
#define RDX (BX_CPU_THIS_PTR gen_reg[2].rrx)
#define RBX (BX_CPU_THIS_PTR gen_reg[3].rrx)
#define RSP (BX_CPU_THIS_PTR gen_reg[4].rrx)
#define RBP (BX_CPU_THIS_PTR gen_reg[5].rrx)
#define RSI (BX_CPU_THIS_PTR gen_reg[6].rrx)
#define RDI (BX_CPU_THIS_PTR gen_reg[7].rrx)
#define R8  (BX_CPU_THIS_PTR gen_reg[8].rrx)
#define R9  (BX_CPU_THIS_PTR gen_reg[9].rrx)
#define R10 (BX_CPU_THIS_PTR gen_reg[10].rrx)
#define R11 (BX_CPU_THIS_PTR gen_reg[11].rrx)
#define R12 (BX_CPU_THIS_PTR gen_reg[12].rrx)
#define R13 (BX_CPU_THIS_PTR gen_reg[13].rrx)
#define R14 (BX_CPU_THIS_PTR gen_reg[14].rrx)
#define R15 (BX_CPU_THIS_PTR gen_reg[15].rrx)

// access to 64 bit instruction pointer
#define RIP (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_RIP].rrx)

#define SSP (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_SSP].rrx)

#define TMP64 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].rrx)

// access to 64 bit MSR registers
#define MSR_FSBASE  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base)
#define MSR_GSBASE  (BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base)

#else // simplify merge between 32-bit and 64-bit mode

#define RAX EAX
#define RCX ECX
#define RDX EDX
#define RBX EBX
#define RSP ESP
#define RBP EBP
#define RSI ESI
#define RDI EDI
#define RIP EIP

#endif // BX_SUPPORT_X86_64 == 0

#define PREV_RIP (BX_CPU_THIS_PTR prev_rip)

#if BX_SUPPORT_X86_64
#define BX_READ_8BIT_REGx(index,extended)  ((((index) & 4) == 0 || (extended)) ? \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl) : \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh))
#define BX_READ_64BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].rrx)
#define BX_READ_64BIT_REG_HIGH(index) (BX_CPU_THIS_PTR gen_reg[index].dword.hrx)
#else
#define BX_READ_8BIT_REG(index)  (((index) & 4) ? \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh) : \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl))
#define BX_READ_8BIT_REGx(index,ext) BX_READ_8BIT_REG(index)
#endif

#define BX_READ_8BIT_REGL(index) (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl)
#define BX_READ_16BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].dword.erx)

#define BX_WRITE_8BIT_REGH(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.byte.rh = val; \
}

#define BX_WRITE_16BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.rx = val; \
}

#if BX_SUPPORT_X86_64

#define BX_WRITE_8BIT_REGx(index, extended, val) {\
  if (((index) & 4) == 0 || (extended)) \
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
  if ((index) & 4) \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
}
#define BX_WRITE_8BIT_REGx(index, ext, val) BX_WRITE_8BIT_REG(index, val)

// For x86-32, I just pretend this one is like the macro above,
// so common code can be used.
#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.erx = (Bit32u) val; \
}

#define BX_CLEAR_64BIT_HIGH(index)

#endif

#define CPL       (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl)

#define USER_PL   (BX_CPU_THIS_PTR user_pl) /* CPL == 3 */

#if BX_SUPPORT_SMP
#define BX_CPU_ID (BX_CPU_THIS_PTR bx_cpuid)
#else
#define BX_CPU_ID (0)
#endif

#if BX_SUPPORT_AVX

#define BX_READ_8BIT_OPMASK(index)  (BX_CPU_THIS_PTR opmask[index].word.byte.rl)
#define BX_READ_16BIT_OPMASK(index) (BX_CPU_THIS_PTR opmask[index].word.rx)
#define BX_READ_32BIT_OPMASK(index) (BX_CPU_THIS_PTR opmask[index].dword.erx)
#define BX_READ_OPMASK(index)       (BX_CPU_THIS_PTR opmask[index].rrx)

#define BX_SCALAR_ELEMENT_MASK(index) ((index) == 0 || (BX_READ_32BIT_OPMASK(index) & 0x1))

#define BX_WRITE_OPMASK(index, val_64) { \
  BX_CPU_THIS_PTR opmask[index].rrx = val_64; \
}

BX_CPP_INLINE Bit64u CUT_OPMASK_TO(unsigned nelements) { return (BX_CONST64(1) << (nelements)) - 1; }

#endif

#endif  // defined(NEED_CPU_REG_SHORTCUTS)

// <TAG-INSTRUMENTATION_COMMON-BEGIN>

// possible types passed to BX_INSTR_TLB_CNTRL()
enum BX_Instr_TLBControl {
  BX_INSTR_MOV_CR0 = 10,
  BX_INSTR_MOV_CR3 = 11,
  BX_INSTR_MOV_CR4 = 12,
  BX_INSTR_TASK_SWITCH = 13,
  BX_INSTR_CONTEXT_SWITCH = 14,
  BX_INSTR_INVLPG = 15,
  BX_INSTR_INVEPT = 16,
  BX_INSTR_INVVPID = 17,
  BX_INSTR_INVPCID = 18
};

// possible types passed to BX_INSTR_CACHE_CNTRL()
enum BX_Instr_CacheControl {
  BX_INSTR_INVD = 10,
  BX_INSTR_WBINVD = 11
};

// possible types passed to BX_INSTR_FAR_BRANCH() and BX_INSTR_UCNEAR_BRANCH()
enum BX_Instr_Branch {
  BX_INSTR_IS_JMP = 10,
  BX_INSTR_IS_JMP_INDIRECT = 11,
  BX_INSTR_IS_CALL = 12,
  BX_INSTR_IS_CALL_INDIRECT = 13,
  BX_INSTR_IS_RET = 14,
  BX_INSTR_IS_IRET = 15,
  BX_INSTR_IS_INT = 16,
  BX_INSTR_IS_SYSCALL = 17,
  BX_INSTR_IS_SYSRET = 18,
  BX_INSTR_IS_SYSENTER = 19,
  BX_INSTR_IS_SYSEXIT = 20
};

// possible types passed to BX_INSTR_PREFETCH_HINT()
enum BX_Instr_PrefetchHINT {
  BX_INSTR_PREFETCH_NTA = 0,
  BX_INSTR_PREFETCH_T0  = 1,
  BX_INSTR_PREFETCH_T1  = 2,
  BX_INSTR_PREFETCH_T2  = 3
};

// <TAG-INSTRUMENTATION_COMMON-END>

// passed to internal debugger together with BX_READ/BX_WRITE/BX_EXECUTE/BX_RW
enum {
  BX_PDPTR0_ACCESS = 1,
  BX_PDPTR1_ACCESS,
  BX_PDPTR2_ACCESS,
  BX_PDPTR3_ACCESS,
  BX_PTE_ACCESS,
  BX_PDE_ACCESS,
  BX_PDTE_ACCESS,
  BX_PML4E_ACCESS,
  BX_EPT_PTE_ACCESS,
  BX_EPT_PDE_ACCESS,
  BX_EPT_PDTE_ACCESS,
  BX_EPT_PML4E_ACCESS,
  BX_EPT_SPP_PTE_ACCESS,
  BX_EPT_SPP_PDE_ACCESS,
  BX_EPT_SPP_PDTE_ACCESS,
  BX_EPT_SPP_PML4E_ACCESS,
  BX_VMCS_ACCESS,
  BX_SHADOW_VMCS_ACCESS,
  BX_MSR_BITMAP_ACCESS,
  BX_IO_BITMAP_ACCESS,
  BX_VMREAD_BITMAP_ACCESS,
  BX_VMWRITE_BITMAP_ACCESS,
  BX_VMX_LOAD_MSR_ACCESS,
  BX_VMX_STORE_MSR_ACCESS,
  BX_VMX_VAPIC_ACCESS,
  BX_VMX_PML_WRITE,
  BX_SMRAM_ACCESS
};

struct BxExceptionInfo {
  unsigned exception_type;
  unsigned exception_class;
  bool push_error;
};

enum BX_Exception {
  BX_DE_EXCEPTION =  0, // Divide Error (fault)
  BX_DB_EXCEPTION =  1, // Debug (fault/trap)
  BX_BP_EXCEPTION =  3, // Breakpoint (trap)
  BX_OF_EXCEPTION =  4, // Overflow (trap)
  BX_BR_EXCEPTION =  5, // BOUND (fault)
  BX_UD_EXCEPTION =  6,
  BX_NM_EXCEPTION =  7,
  BX_DF_EXCEPTION =  8,
  BX_TS_EXCEPTION = 10,
  BX_NP_EXCEPTION = 11,
  BX_SS_EXCEPTION = 12,
  BX_GP_EXCEPTION = 13,
  BX_PF_EXCEPTION = 14,
  BX_MF_EXCEPTION = 16,
  BX_AC_EXCEPTION = 17,
  BX_MC_EXCEPTION = 18,
  BX_XM_EXCEPTION = 19,
  BX_VE_EXCEPTION = 20,
  BX_CP_EXCEPTION = 21  // Control Protection (fault)
};

enum CP_Exception_Error_Code {
  BX_CP_NEAR_RET = 1,
  BX_CP_FAR_RET_IRET = 2,
  BX_CP_ENDBRANCH = 3,
  BX_CP_RSTORSSP = 4,
  BX_CP_SETSSBSY = 5
};

const unsigned BX_CPU_HANDLED_EXCEPTIONS = 32;

enum BxCpuMode {
  BX_MODE_IA32_REAL = 0,        // CR0.PE=0                |
  BX_MODE_IA32_V8086 = 1,       // CR0.PE=1, EFLAGS.VM=1   | EFER.LMA=0
  BX_MODE_IA32_PROTECTED = 2,   // CR0.PE=1, EFLAGS.VM=0   |
  BX_MODE_LONG_COMPAT = 3,      // EFER.LMA = 1, CR0.PE=1, CS.L=0
  BX_MODE_LONG_64 = 4           // EFER.LMA = 1, CR0.PE=1, CS.L=1
};

const unsigned BX_MSR_MAX_INDEX = 0x1000;

extern const char* cpu_mode_string(unsigned cpu_mode);

#if BX_SUPPORT_X86_64
BX_CPP_INLINE bool IsCanonical(bx_address offset)
{
  return ((Bit64u)((((Bit64s)(offset)) >> (BX_LIN_ADDRESS_WIDTH-1)) + 1) < 2);
}
#endif

BX_CPP_INLINE bool IsValidPhyAddr(bx_phy_address addr)
{
  return ((addr & BX_PHY_ADDRESS_RESERVED_BITS) == 0);
}

BX_CPP_INLINE bool IsValidPageAlignedPhyAddr(bx_phy_address addr)
{
  return ((addr & (BX_PHY_ADDRESS_RESERVED_BITS | 0xfff)) == 0);
}

const Bit32u CACHE_LINE_SIZE = 64;

class BX_CPU_C;
class BX_MEM_C;
class bxInstruction_c;

// <TAG-TYPE-EXECUTEPTR-START>
#if BX_USE_CPU_SMF
typedef void (BX_CPP_AttrRegparmN(1) *BxRepIterationPtr_tR)(bxInstruction_c *);
#else
typedef void (BX_CPU_C::*BxRepIterationPtr_tR)(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
// <TAG-TYPE-EXECUTEPTR-END>

#if BX_USE_CPU_SMF == 0
// normal member functions.  This can ONLY be used within BX_CPU_C classes.
// Anyone on the outside should use the BX_CPU macro (defined in bochs.h)
// instead.
#  define BX_CPU_THIS_PTR  this->
#  define BX_CPU_THIS      this
#  define BX_SMF
// with normal member functions, calling a member fn pointer looks like
// object->*(fnptr)(arg, ...);
// Since this is different from when SMF=1, encapsulate it in a macro.
#  define BX_CPU_CALL_METHOD(func, args) \
            (this->*((BxExecutePtr_tR) (func))) args
#  define BX_CPU_CALL_REP_ITERATION(func, args) \
            (this->*((BxRepIterationPtr_tR) (func))) args
#else
// static member functions.  With SMF, there is only one CPU by definition.
#  define BX_CPU_THIS_PTR  BX_CPU(0)->
#  define BX_CPU_THIS      BX_CPU(0)
#  define BX_SMF           static
#  define BX_CPU_CALL_METHOD(func, args) \
            ((BxExecutePtr_tR) (func)) args
#  define BX_CPU_CALL_REP_ITERATION(func, args) \
            ((BxRepIterationPtr_tR) (func)) args
#endif

//
// BX_CPU_RESOLVE_ADDR:
// Resolve virtual address of the instruction's memory reference without any
// assumptions about instruction's operand size, address size or execution
// mode
//
// BX_CPU_RESOLVE_ADDR_64:
// Resolve virtual address of the instruction memory reference assuming
// the instruction is executed in 64-bit long mode with possible 64-bit
// or 32-bit address size.
//
// BX_CPU_RESOLVE_ADDR_32:
// Resolve virtual address of the instruction memory reference assuming
// the instruction is executed in legacy or compatibility mode with
// possible 32-bit or 16-bit address size.
//
//
#if BX_SUPPORT_X86_64
#  define BX_CPU_RESOLVE_ADDR(i) \
            ((i)->as64L() ? BxResolve64(i) : BxResolve32(i))
#  define BX_CPU_RESOLVE_ADDR_64(i) \
            ((i)->as64L() ? BxResolve64(i) : BxResolve32(i))
#else
#  define BX_CPU_RESOLVE_ADDR(i) \
            (BxResolve32(i))
#endif
#  define BX_CPU_RESOLVE_ADDR_32(i) \
            (BxResolve32(i))


#if BX_SUPPORT_SMP
// multiprocessor simulation, we need an array of cpus and memories
BOCHSAPI extern BX_CPU_C **bx_cpu_array;
#else
// single processor simulation, so there's one of everything
BOCHSAPI extern BX_CPU_C   bx_cpu;
#endif

// notify internal debugger/instrumentation about memory access
#define BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, paddr, size, memtype, rw, dataptr) {              \
  BX_INSTR_LIN_ACCESS(BX_CPU_ID, (laddr), (paddr), (size), (memtype), (rw));                 \
  BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, (laddr), (paddr), (size), (memtype), (rw), (dataptr)); \
}

#define BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, size, memtype, rw, why, dataptr) {              \
  BX_INSTR_PHY_ACCESS(BX_CPU_ID, (paddr), (size), (memtype), (rw));                        \
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, (paddr), (size), (memtype), (rw), (why), (dataptr)); \
}

// accessors for all eflags in bx_flags_reg_t
// The macro is used once for each flag bit
// Do not use for arithmetic flags !
#define DECLARE_EFLAG_ACCESSOR(name,bitnum)                     \
  BX_SMF BX_CPP_INLINE unsigned  get_##name ();                 \
  BX_SMF BX_CPP_INLINE unsigned getB_##name ();                 \
  BX_SMF BX_CPP_INLINE void assert_##name ();                   \
  BX_SMF BX_CPP_INLINE void clear_##name ();                    \
  BX_SMF BX_CPP_INLINE void set_##name (bool val);

#define IMPLEMENT_EFLAG_ACCESSOR(name,bitnum)                   \
  BX_CPP_INLINE unsigned BX_CPU_C::getB_##name () {             \
    return 1 & (BX_CPU_THIS_PTR eflags >> bitnum);              \
  }                                                             \
  BX_CPP_INLINE unsigned BX_CPU_C::get_##name () {              \
    return BX_CPU_THIS_PTR eflags & (1 << bitnum);              \
  }

#define IMPLEMENT_EFLAG_SET_ACCESSOR(name,bitnum)                   \
  BX_CPP_INLINE void BX_CPU_C::assert_##name () {                   \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                          \
  }                                                                 \
  BX_CPP_INLINE void BX_CPU_C::clear_##name () {                    \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                         \
  }                                                                 \
  BX_CPP_INLINE void BX_CPU_C::set_##name (bool val) {              \
    BX_CPU_THIS_PTR eflags =                                        \
      (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|(Bit32u(val)<<bitnum);   \
  }

#if BX_CPU_LEVEL >= 4

#define IMPLEMENT_EFLAG_SET_ACCESSOR_AC(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_AC() {                    \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                      \
    handleAlignmentCheck();                                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_AC() {                     \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                     \
    handleAlignmentCheck();                                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_AC(bool val) {                   \
    BX_CPU_THIS_PTR eflags =                                        \
      (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|(Bit32u(val)<<bitnum);  \
    handleAlignmentCheck();                                         \
  }

#endif

#define IMPLEMENT_EFLAG_SET_ACCESSOR_VM(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_VM() {                    \
    set_VM(1);                                                  \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_VM() {                     \
    set_VM(0);                                                  \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_VM(bool val) {               \
    if (!long_mode()) {                                         \
      BX_CPU_THIS_PTR eflags =                                  \
        (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|(Bit32u(val)<<bitnum);  \
      handleCpuModeChange();                                    \
    }                                                           \
  }

// need special handling when IF is set
#define IMPLEMENT_EFLAG_SET_ACCESSOR_IF(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_IF() {                    \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                      \
    handleInterruptMaskChange();                                \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_IF() {                     \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                     \
    handleInterruptMaskChange();                                \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_IF(bool val) {               \
    if (val) assert_IF();                                       \
    else clear_IF();                                            \
  }

// assert async_event when TF is set
#define IMPLEMENT_EFLAG_SET_ACCESSOR_TF(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_TF() {                    \
    BX_CPU_THIS_PTR async_event = 1;                            \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_TF() {                     \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_TF(bool val) {                   \
    if (val) BX_CPU_THIS_PTR async_event = 1;                       \
    BX_CPU_THIS_PTR eflags =                                        \
      (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|(Bit32u(val)<<bitnum);  \
  }

// invalidate prefetch queue and call prefetch() when RF is set
#define IMPLEMENT_EFLAG_SET_ACCESSOR_RF(bitnum)                 \
  BX_CPP_INLINE void BX_CPU_C::assert_RF() {                    \
    invalidate_prefetch_q();                                    \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_RF() {                     \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_RF(bool val) {                   \
    if (val) invalidate_prefetch_q();                               \
    BX_CPU_THIS_PTR eflags =                                        \
      (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|(Bit32u(val)<<bitnum);  \
  }

#define DECLARE_EFLAG_ACCESSOR_IOPL(bitnum)                     \
  BX_SMF BX_CPP_INLINE void set_IOPL(Bit32u val);               \
  BX_SMF BX_CPP_INLINE Bit32u  get_IOPL(void);                  

#define IMPLEMENT_EFLAG_ACCESSOR_IOPL(bitnum)                   \
  BX_CPP_INLINE void BX_CPU_C::set_IOPL(Bit32u val) {           \
    BX_CPU_THIS_PTR eflags &= ~(3<<bitnum);                     \
    BX_CPU_THIS_PTR eflags |= ((3&val) << bitnum);              \
  }                                                             \
  BX_CPP_INLINE Bit32u BX_CPU_C::get_IOPL() {                   \
    return 3 & (BX_CPU_THIS_PTR eflags >> bitnum);              \
  }

const Bit32u EFlagsCFMask   = (1 <<  0);
const Bit32u EFlagsPFMask   = (1 <<  2);
const Bit32u EFlagsAFMask   = (1 <<  4);
const Bit32u EFlagsZFMask   = (1 <<  6);
const Bit32u EFlagsSFMask   = (1 <<  7);
const Bit32u EFlagsTFMask   = (1 <<  8);
const Bit32u EFlagsIFMask   = (1 <<  9);
const Bit32u EFlagsDFMask   = (1 << 10);
const Bit32u EFlagsOFMask   = (1 << 11);
const Bit32u EFlagsIOPLMask = (3 << 12);
const Bit32u EFlagsNTMask   = (1 << 14);
const Bit32u EFlagsRFMask   = (1 << 16);
const Bit32u EFlagsVMMask   = (1 << 17);
const Bit32u EFlagsACMask   = (1 << 18);
const Bit32u EFlagsVIFMask  = (1 << 19);
const Bit32u EFlagsVIPMask  = (1 << 20);
const Bit32u EFlagsIDMask   = (1 << 21);

const Bit32u EFlagsOSZAPCMask = \
    (EFlagsCFMask | EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask);

const Bit32u EFlagsOSZAPMask = \
    (EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask);

const Bit32u EFlagsValidMask = 0x003f7fd5; // only supported bits for EFLAGS

#if BX_SUPPORT_FPU
#include "i387.h"
#endif

#if BX_CPU_LEVEL >= 5
typedef struct
{
#if BX_SUPPORT_APIC
  bx_phy_address apicbase;
#endif

  // SYSCALL/SYSRET instruction msr's
  Bit64u star;
#if BX_SUPPORT_X86_64
  Bit64u lstar;
  Bit64u cstar;
  Bit32u fmask;
  Bit64u kernelgsbase;
  Bit32u tsc_aux;
#endif

#if BX_CPU_LEVEL >= 6
  // SYSENTER/SYSEXIT instruction msr's
  Bit32u sysenter_cs_msr;
  bx_address sysenter_esp_msr;
  bx_address sysenter_eip_msr;

  BxPackedRegister pat;
  Bit64u mtrrphys[16];
  BxPackedRegister mtrrfix64k;
  BxPackedRegister mtrrfix16k[2];
  BxPackedRegister mtrrfix4k[8];
  Bit32u mtrr_deftype;
#endif

#if BX_SUPPORT_VMX
  Bit32u ia32_feature_ctrl;
#endif

#if BX_SUPPORT_SVM
  Bit64u svm_hsave_pa;
#endif

#if BX_CPU_LEVEL >= 6
  Bit64u ia32_xss;
#endif

  // CET
#if BX_SUPPORT_CET
  Bit64u ia32_cet_control[2]; // indexed by CPL==3
  Bit64u ia32_pl_ssp[4];
  Bit64u ia32_interrupt_ssp_table;
#endif

  Bit32u ia32_spec_ctrl; // SCA

  /* TODO finish of the others */
} bx_regs_msr_t;
#endif

#include "crregs.h"
#include "descriptor.h"
#include "decoder/instr.h"
#include "lazy_flags.h"
#include "tlb.h"
#include "icache.h"

// general purpose register
#if BX_SUPPORT_X86_64

#ifdef BX_BIG_ENDIAN
typedef struct {
  union {
    struct {
      Bit32u dword_filler;
      Bit16u  word_filler;
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
      Bit32u erx;  // lo 32 bits
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
      Bit16u  word_filler;
      Bit32u dword_filler;
    } word;
    Bit64u rrx;
    struct {
      Bit32u erx;  // lo 32 bits
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

#if BX_SUPPORT_APIC
#include "apic.h"
#endif

#if BX_SUPPORT_FPU
#include "xmm.h"
#endif

#if BX_SUPPORT_VMX
#include "vmx.h"
#endif

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#if BX_SUPPORT_MONITOR_MWAIT
struct monitor_addr_t {

    bx_phy_address monitor_addr;
    bool armed;

    monitor_addr_t(): monitor_addr(0xffffffff), armed(0) {}

    BX_CPP_INLINE void arm(bx_phy_address addr) {
      // align to cache line
      monitor_addr = addr & ~((bx_phy_address)(CACHE_LINE_SIZE - 1));
      armed = 1;
    }

    BX_CPP_INLINE void reset_monitor(void) { armed = 0; }
};
#endif

struct BX_SMM_State;
struct BxOpcodeInfo_t;
struct bx_cpu_statistics;

#include "cpuid.h"

class BOCHSAPI BX_CPU_C : public logfunctions {

public: // for now...

  unsigned bx_cpuid;

#if BX_CPU_LEVEL >= 4
  bx_cpuid_t *cpuid;
#endif

  Bit32u ia_extensions_bitmask[BX_ISA_EXTENSIONS_ARRAY_SIZE];

#define BX_CPUID_SUPPORT_ISA_EXTENSION(feature) \
   (BX_CPU_THIS_PTR ia_extensions_bitmask[feature/32] & (1<<(feature%32)))

#if BX_SUPPORT_VMX
  Bit32u vmx_extensions_bitmask;
#endif
#if BX_SUPPORT_SVM
  Bit32u svm_extensions_bitmask;
#endif

#define BX_SUPPORT_VMX_EXTENSION(feature_mask) \
   (BX_CPU_THIS_PTR vmx_extensions_bitmask & (feature_mask))

#define BX_SUPPORT_SVM_EXTENSION(feature_mask) \
   (BX_CPU_THIS_PTR svm_extensions_bitmask & (feature_mask))

  // General register set
  // rax: accumulator
  // rbx: base
  // rcx: count
  // rdx: data
  // rbp: base pointer
  // rsi: source index
  // rdi: destination index
  // esp: stack pointer
  // r8..r15 x86-64 extended registers
  // rip: instruction pointer
  // ssp: shadow stack pointer
  // tmp: temp register
  // nil: null register
  bx_gen_reg_t gen_reg[BX_GENERAL_REGISTERS+4];

  /* 31|30|29|28| 27|26|25|24| 23|22|21|20| 19|18|17|16
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0| 0| 0| 0|  0| 0| 0| 0|  0| 0|ID|VP| VF|AC|VM|RF
   *
   * 15|14|13|12| 11|10| 9| 8|  7| 6| 5| 4|  3| 2| 1| 0
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0|NT| IOPL| OF|DF|IF|TF| SF|ZF| 0|AF|  0|PF| 1|CF
   */
  Bit32u eflags; // Raw 32-bit value in x86 bit position.

  // lazy arithmetic flags state
  bx_lazyflags_entry oszapc;

  // so that we can back up when handling faults, exceptions, etc.
  // we need to store the value of the instruction pointer, before
  // each fetch/execute cycle.
  bx_address prev_rip;
  bx_address prev_rsp;
#if BX_SUPPORT_CET
  bx_address prev_ssp;
#endif
  bool    speculative_rsp;

  Bit64u icount;
  Bit64u icount_last_sync;

#define BX_INHIBIT_INTERRUPTS        0x01
#define BX_INHIBIT_DEBUG             0x02

#define BX_INHIBIT_INTERRUPTS_BY_MOVSS        \
    (BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG)

  // What events to inhibit at any given time.  Certain instructions
  // inhibit interrupts, some debug exceptions and single-step traps.
  unsigned inhibit_mask;
  Bit64u inhibit_icount;

  /* user segment register set */
  bx_segment_reg_t  sregs[6];

  /* system segment registers */
  bx_global_segment_reg_t gdtr; /* global descriptor table register */
  bx_global_segment_reg_t idtr; /* interrupt descriptor table register */
  bx_segment_reg_t        ldtr; /* local descriptor table register */
  bx_segment_reg_t        tr;   /* task register */

  /* debug registers DR0-DR7 */
  bx_address dr[4]; /* DR0-DR3 */
  bx_dr6_t   dr6;
  bx_dr7_t   dr7;

  Bit32u debug_trap; // holds DR6 value (16bit) to be set

  /* Control registers */
  bx_cr0_t   cr0;
  bx_address cr2;
  bx_address cr3;
#if BX_CPU_LEVEL >= 5
  bx_cr4_t   cr4;
  Bit32u cr4_suppmask;

  bx_efer_t efer;
  Bit32u efer_suppmask;
#endif

#if BX_CPU_LEVEL >= 5
  // TSC: Time Stamp Counter
  // Instead of storing a counter and incrementing it every instruction, we
  // remember the time in ticks that it was reset to zero.  With a little
  // algebra, we can also support setting it to something other than zero.
  // Don't read this directly; use get_TSC and set_TSC to access the TSC.
  Bit64s tsc_adjust;
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  Bit64s tsc_offset;
#endif
#endif

#if BX_CPU_LEVEL >= 6
  xcr0_t xcr0;
  Bit32u xcr0_suppmask;
#endif

#if BX_SUPPORT_PKEYS
  // protection keys
  Bit32u pkru;
  Bit32u pkrs;

  // unpacked protection keys to be tested together with accessBits from TLB
  // the unpacked key is stored in the accessBits format:
  //     bit 5: Execute from User   privilege is OK
  //     bit 4: Execute from System privilege is OK
  //     bit 3: Write   from User   privilege is OK
  //     bit 2: Write   from System privilege is OK
  //     bit 1: Read    from User   privilege is OK
  //     bit 0: Read    from System privilege is OK
  // But only bits 1 and 3 are relevant, all others should be set to '1
  // When protection key prevents all accesses to the page both bits 1 and 3 are cleared
  // When protection key prevents writes to the page bit 1 will be set and 3 cleared
  // When no protection keys are enabled all bits should be set for all keys
  Bit32u rd_pkey[16];
  Bit32u wr_pkey[16];
#endif

#if BX_SUPPORT_FPU
  i387_t the_i387;
#endif

#if BX_CPU_LEVEL >= 6

  // Vector register set
  // vmm0-vmmN: up to 32 vector registers
  // vtmp: temp register
#if BX_SUPPORT_EVEX
  bx_zmm_reg_t vmm[BX_XMM_REGISTERS+1] BX_CPP_AlignN(64);
#else
#if BX_SUPPORT_AVX
  bx_ymm_reg_t vmm[BX_XMM_REGISTERS+1] BX_CPP_AlignN(32);
#else
  bx_xmm_reg_t vmm[BX_XMM_REGISTERS+1] BX_CPP_AlignN(16);
#endif
#endif

  bx_mxcsr_t mxcsr;
  Bit32u mxcsr_mask;

#if BX_SUPPORT_EVEX
  bx_gen_reg_t opmask[8];
#endif

#endif

#if BX_SUPPORT_MONITOR_MWAIT
  monitor_addr_t monitor;
#endif

#if BX_SUPPORT_APIC
  bx_local_apic_c lapic;
#endif

  /* SMM base register */
  Bit32u smbase;

#if BX_CPU_LEVEL >= 5
  bx_regs_msr_t msr;
#endif

#if BX_CONFIGURE_MSRS
  MSR *msrs[BX_MSR_MAX_INDEX];
#endif

#if BX_SUPPORT_VMX
  bool in_vmx;
  bool in_vmx_guest;
  bool in_smm_vmx; // save in_vmx and in_vmx_guest flags when in SMM mode
  bool in_smm_vmx_guest;
  Bit64u  vmcsptr;
  bx_hostpageaddr_t vmcshostptr;
#if BX_SUPPORT_MEMTYPE
  BxMemtype vmcs_memtype;
#endif
  Bit64u  vmxonptr;
  
  VMCS_CACHE vmcs;
  VMX_CAP vmx_cap;
  VMCS_Mapping *vmcs_map;
#endif

#if BX_SUPPORT_SVM
  bool in_svm_guest;
  bool svm_gif; /* global interrupt enable flag, when zero all external interrupt disabled */
  bx_phy_address  vmcbptr;
  bx_hostpageaddr_t vmcbhostptr;
#if BX_SUPPORT_MEMTYPE
  BxMemtype vmcb_memtype;
#endif
  VMCB_CACHE vmcb;

// make SVM integration easier
#define SVM_GIF (BX_CPU_THIS_PTR svm_gif)

#else

#define SVM_GIF (1)

#endif

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  bool in_event;
#endif

#if BX_SUPPORT_VMX
  bool nmi_unblocking_iret;
#endif

  bool EXT; /* 1 if processing external interrupt or exception
                * or if not related to current instruction,
                * 0 if current CS:IP caused exception */

  enum CPU_Activity_State {
    BX_ACTIVITY_STATE_ACTIVE = 0,
    BX_ACTIVITY_STATE_HLT,
    BX_ACTIVITY_STATE_SHUTDOWN,
    BX_ACTIVITY_STATE_WAIT_FOR_SIPI,
    BX_ACTIVITY_STATE_MWAIT,
    BX_ACTIVITY_STATE_MWAIT_IF
  };

#define BX_VMX_LAST_ACTIVITY_STATE (BX_ACTIVITY_STATE_WAIT_FOR_SIPI)

  unsigned activity_state;

#define BX_EVENT_NMI                          (1 <<  0)
#define BX_EVENT_SMI                          (1 <<  1)
#define BX_EVENT_INIT                         (1 <<  2)
#define BX_EVENT_CODE_BREAKPOINT_ASSIST       (1 <<  3)
#define BX_EVENT_VMX_MONITOR_TRAP_FLAG        (1 <<  4)
#define BX_EVENT_VMX_PREEMPTION_TIMER_EXPIRED (1 <<  5)
#define BX_EVENT_VMX_INTERRUPT_WINDOW_EXITING (1 <<  6)
#define BX_EVENT_VMX_VIRTUAL_NMI              (1 <<  7)
#define BX_EVENT_SVM_VIRQ_PENDING             (1 <<  8)
#define BX_EVENT_PENDING_VMX_VIRTUAL_INTR     (1 <<  9)
#define BX_EVENT_PENDING_INTR                 (1 << 10)
#define BX_EVENT_PENDING_LAPIC_INTR           (1 << 11)
#define BX_EVENT_VMX_VTPR_UPDATE              (1 << 12)
#define BX_EVENT_VMX_VEOI_UPDATE              (1 << 13)
#define BX_EVENT_VMX_VIRTUAL_APIC_WRITE       (1 << 14)
  Bit32u  pending_event;
  Bit32u  event_mask;
  Bit32u  async_event;

  BX_SMF BX_CPP_INLINE void signal_event(Bit32u event) {
    BX_CPU_THIS_PTR pending_event |= event;
    if (! is_masked_event(event)) BX_CPU_THIS_PTR async_event = 1;
  }

  BX_SMF BX_CPP_INLINE void clear_event(Bit32u event) {
    BX_CPU_THIS_PTR pending_event &= ~event;
  }

  BX_SMF BX_CPP_INLINE void mask_event(Bit32u event) {
    BX_CPU_THIS_PTR event_mask |= event;
  }
  BX_SMF BX_CPP_INLINE void unmask_event(Bit32u event) {
    BX_CPU_THIS_PTR event_mask &= ~event;
    if (is_pending(event)) BX_CPU_THIS_PTR async_event = 1;
  }

  BX_SMF BX_CPP_INLINE bool is_masked_event(Bit32u event) {
    return (BX_CPU_THIS_PTR event_mask & event) != 0;
  }

  BX_SMF BX_CPP_INLINE bool is_pending(Bit32u event) {
    return (BX_CPU_THIS_PTR pending_event & event) != 0;
  }
  BX_SMF BX_CPP_INLINE bool is_unmasked_event_pending(Bit32u event) {
    return (BX_CPU_THIS_PTR pending_event & ~BX_CPU_THIS_PTR event_mask & event) != 0;
  }

  BX_SMF BX_CPP_INLINE Bit32u unmasked_events_pending(void) {
    return (BX_CPU_THIS_PTR pending_event & ~BX_CPU_THIS_PTR event_mask);
  }

#define BX_ASYNC_EVENT_STOP_TRACE (1<<31)

#if BX_X86_DEBUGGER
  bool  in_repeat;
#endif
  bool  in_smm;
  unsigned cpu_mode;
  bool  user_pl;
#if BX_CPU_LEVEL >= 5
  bool  ignore_bad_msrs;
#endif
#if BX_CPU_LEVEL >= 6
  unsigned sse_ok;
#if BX_SUPPORT_AVX
  unsigned avx_ok;
#endif
#if BX_SUPPORT_EVEX
  unsigned opmask_ok;
  unsigned evex_ok;
#endif
#endif

  // for exceptions
  static jmp_buf jmp_buf_env;
  unsigned last_exception_type;

  // Boundaries of current code page, based on EIP
  bx_address eipPageBias;
  Bit32u     eipPageWindowSize;
  const Bit8u *eipFetchPtr;
  bx_phy_address pAddrFetchPage; // Guest physical address of current instruction page

  // Boundaries of current stack page, based on ESP
  bx_address espPageBias;        // Linear address of current stack page
  Bit32u     espPageWindowSize;
  const Bit8u *espHostPtr;
  bx_phy_address pAddrStackPage; // Guest physical address of current stack page
#if BX_SUPPORT_MEMTYPE
  BxMemtype espPageMemtype;
#endif
#if BX_SUPPORT_SMP == 0
  Bit32u espPageFineGranularityMapping;
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned alignment_check_mask;
#endif

  // statistics
  bx_cpu_statistics *stats;

#if BX_DEBUGGER
  bx_phy_address watchpoint;
  Bit8u break_point;
  Bit8u magic_break;
  Bit8u stop_reason;
  bool trace;
  bool trace_reg;
  bool trace_mem;
  bool mode_break;
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  bool vmexit_break;
#endif
  unsigned show_flag;
  bx_guard_found_t guard_found;
#endif

#if BX_INSTRUMENTATION
  // store far branch CS:EIP pair for instrumentation purposes
  // unfortunatelly prev_rip CPU field cannot be used as is because it
  // could be overwritten by task switch which could happen as result
  // of the far branch
  struct {
    Bit16u prev_cs;
    bx_address prev_rip;
  } far_branch;

#define FAR_BRANCH_PREV_CS (BX_CPU_THIS_PTR far_branch.prev_cs)
#define FAR_BRANCH_PREV_RIP (BX_CPU_THIS_PTR far_branch.prev_rip)

#define BX_INSTR_FAR_BRANCH_ORIGIN() { \
  BX_CPU_THIS_PTR far_branch.prev_cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value; \
  BX_CPU_THIS_PTR far_branch.prev_rip = PREV_RIP; \
}

#else
#define BX_INSTR_FAR_BRANCH_ORIGIN()
#endif

#define BX_DTLB_SIZE 2048
#define BX_ITLB_SIZE 1024
  TLB<BX_DTLB_SIZE> DTLB BX_CPP_AlignN(32);
  TLB<BX_ITLB_SIZE> ITLB BX_CPP_AlignN(32);

#if BX_CPU_LEVEL >= 6
  struct {
    Bit64u entry[4];
  } PDPTR_CACHE;
#endif

  // An instruction cache.  Each entry should be exactly 32 bytes, and
  // this structure should be aligned on a 32-byte boundary to be friendly
  // with the host cache lines.
  bxICache_c iCache BX_CPP_AlignN(32);
  Bit32u fetchModeMask;

  struct {
    bx_address rm_addr;       // The address offset after resolution
    bx_phy_address paddress1; // physical address after translation of 1st len1 bytes of data
    bx_phy_address paddress2; // physical address after translation of 2nd len2 bytes of data
    Bit32u len1;              // Number of bytes in page 1
    Bit32u len2;              // Number of bytes in page 2
    bx_ptr_equiv_t pages;     // Number of pages access spans (1 or 2).  Also used
                              // for the case when a native host pointer is
                              // available for the R-M-W instructions.  The host
                              // pointer is stuffed here.  Since this field has
                              // to be checked anyways (and thus cached), if it
                              // is greated than 2 (the maximum possible for
                              // normal cases) it is a native pointer and is used
                              // for a direct write access.
#if BX_SUPPORT_MEMTYPE
    BxMemtype memtype1;       // memory type of the page 1
    BxMemtype memtype2;       // memory type of the page 2
#endif
  } address_xlation;

  BX_SMF void setEFlags(Bit32u val) BX_CPP_AttrRegparmN(1);

  BX_SMF BX_CPP_INLINE void setEFlagsOSZAPC(Bit32u flags32) {
    set_OF(1 & ((flags32) >> 11));
    set_SF(1 & ((flags32) >> 7));
    set_ZF(1 & ((flags32) >> 6));
    set_AF(1 & ((flags32) >> 4));
    set_PF(1 & ((flags32) >> 2));
    set_CF(1 & ((flags32) >> 0));
  }

  BX_SMF BX_CPP_INLINE void clearEFlagsOSZAPC(void) {
    SET_FLAGS_OSZAPC_LOGIC_32(1);
  }

  BX_SMF BX_CPP_INLINE unsigned getB_OF(void) { return BX_CPU_THIS_PTR oszapc.getB_OF(); }
  BX_SMF BX_CPP_INLINE unsigned get_OF(void) { return BX_CPU_THIS_PTR oszapc.get_OF(); }
  BX_SMF BX_CPP_INLINE void set_OF(bool val) { BX_CPU_THIS_PTR oszapc.set_OF(val); }
  BX_SMF BX_CPP_INLINE void clear_OF(void) { BX_CPU_THIS_PTR oszapc.clear_OF(); }
  BX_SMF BX_CPP_INLINE void assert_OF(void) { BX_CPU_THIS_PTR oszapc.assert_OF(); }

  BX_SMF BX_CPP_INLINE unsigned getB_SF(void) { return BX_CPU_THIS_PTR oszapc.getB_SF(); }
  BX_SMF BX_CPP_INLINE unsigned get_SF(void) { return BX_CPU_THIS_PTR oszapc.get_SF(); }
  BX_SMF BX_CPP_INLINE void set_SF(bool val) { BX_CPU_THIS_PTR oszapc.set_SF(val); }
  BX_SMF BX_CPP_INLINE void clear_SF(void) { BX_CPU_THIS_PTR oszapc.clear_SF(); }
  BX_SMF BX_CPP_INLINE void assert_SF(void) { BX_CPU_THIS_PTR oszapc.assert_SF(); }

  BX_SMF BX_CPP_INLINE unsigned getB_ZF(void) { return BX_CPU_THIS_PTR oszapc.getB_ZF(); }
  BX_SMF BX_CPP_INLINE unsigned get_ZF(void) { return BX_CPU_THIS_PTR oszapc.get_ZF(); }
  BX_SMF BX_CPP_INLINE void set_ZF(bool val) { BX_CPU_THIS_PTR oszapc.set_ZF(val); }
  BX_SMF BX_CPP_INLINE void clear_ZF(void) { BX_CPU_THIS_PTR oszapc.clear_ZF(); }
  BX_SMF BX_CPP_INLINE void assert_ZF(void) { BX_CPU_THIS_PTR oszapc.assert_ZF(); }

  BX_SMF BX_CPP_INLINE unsigned getB_AF(void) { return BX_CPU_THIS_PTR oszapc.getB_AF(); }
  BX_SMF BX_CPP_INLINE unsigned get_AF(void) { return BX_CPU_THIS_PTR oszapc.get_AF(); }
  BX_SMF BX_CPP_INLINE void set_AF(bool val) { BX_CPU_THIS_PTR oszapc.set_AF(val); }
  BX_SMF BX_CPP_INLINE void clear_AF(void) { BX_CPU_THIS_PTR oszapc.clear_AF(); }
  BX_SMF BX_CPP_INLINE void assert_AF(void) { BX_CPU_THIS_PTR oszapc.assert_AF(); }

  BX_SMF BX_CPP_INLINE unsigned getB_PF(void) { return BX_CPU_THIS_PTR oszapc.getB_PF(); }
  BX_SMF BX_CPP_INLINE unsigned get_PF(void) { return BX_CPU_THIS_PTR oszapc.get_PF(); }
  BX_SMF BX_CPP_INLINE void set_PF(bool val) { BX_CPU_THIS_PTR oszapc.set_PF(val); }
  BX_SMF BX_CPP_INLINE void clear_PF(void) { BX_CPU_THIS_PTR oszapc.clear_PF(); }
  BX_SMF BX_CPP_INLINE void assert_PF(void) { BX_CPU_THIS_PTR oszapc.assert_PF(); }

  BX_SMF BX_CPP_INLINE unsigned getB_CF(void) { return BX_CPU_THIS_PTR oszapc.getB_CF(); }
  BX_SMF BX_CPP_INLINE unsigned get_CF(void) { return BX_CPU_THIS_PTR oszapc.get_CF(); }
  BX_SMF BX_CPP_INLINE void set_CF(bool val) { BX_CPU_THIS_PTR oszapc.set_CF(val); }
  BX_SMF BX_CPP_INLINE void clear_CF(void) { BX_CPU_THIS_PTR oszapc.clear_CF(); }
  BX_SMF BX_CPP_INLINE void assert_CF(void) { BX_CPU_THIS_PTR oszapc.assert_CF(); }
 
  // constructors & destructors...
  BX_CPU_C(unsigned id = 0);
 ~BX_CPU_C();

  void initialize(void);
  void init_statistics(void);
  void after_restore_state(void);
  void register_state(void);
  static Bit64s param_save_handler(void *devptr, bx_param_c *param);
  static void param_restore_handler(void *devptr, bx_param_c *param, Bit64s val);
#if !BX_USE_CPU_SMF
  Bit64s param_save(bx_param_c *param);
  void param_restore(bx_param_c *param, Bit64s val);
#endif

// <TAG-CLASS-CPU-START>
  // prototypes for CPU instructions...
  BX_SMF void PUSH16_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void DAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSHA32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHA16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPA32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPA16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ARPL_EwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_Id(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INSB32_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSB16_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW32_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW16_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD32_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD16_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSB32_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSB16_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW32_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW16_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD32_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD16_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void REP_INSB_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_INSW_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_INSD_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSB_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSW_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSD_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BOUND_GwMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BOUND_GdMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV32_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV32_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV32S_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV32S_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EwSwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwSwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_SwEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LEA_GdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEA_GwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CBW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CWD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL32_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL16_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_ALOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EAXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_AXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // repeatable instructions
  BX_SMF void REP_MOVSB_YbXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_MOVSW_YwXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_MOVSD_YdXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSB_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSW_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSD_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSB_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSB_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASB_ALYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSW_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSW_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASW_AXYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSD_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSD_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASD_EAXYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // qualified by address size
  BX_SMF void CMPSB16_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW16_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD16_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSB32_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW32_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD32_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SCASB16_ALYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW16_AXYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD16_EAXYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASB32_ALYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW32_AXYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD32_EAXYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LODSB16_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW16_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD16_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSB32_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW32_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD32_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void STOSB16_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW16_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD16_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSB32_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW32_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD32_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVSB16_YbXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW16_YwXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD16_YdXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSB32_YbXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW32_YwXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD32_YdXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ENTER16_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ENTER32_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INT1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INT3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INT_Ib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INTO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SALC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XLAT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LOOPNE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPNE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JCXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JECXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_AXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_EAXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_ALDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_AXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_EAXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void HLT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LAR_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSL_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLTS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WBINVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLFLUSH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLZERO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_CR0Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR2Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR3Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR4Rd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdCR0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdCR2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdCR3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdCR4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_DdRd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdDd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SETO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SETO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CPUID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SHRD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSF_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSF_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LES_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LES_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSWAP_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSWAP_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ZERO_IDIOM_GwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ZERO_IDIOM_GdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NEG_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NEG_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void IMUL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GdEdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JMP_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LTR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VERR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VERW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SMSW_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SMSW_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LMSW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // LOAD methods
  BX_SMF void LOAD_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void LOAD_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void LOADU_Wdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Wdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Wss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Wsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Ww(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Wb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_AVX
  BX_SMF void LOAD_Vector(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Half_Vector(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Quarter_Vector(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Eighth_Vector(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_EVEX
  BX_SMF void LOAD_MASK_Wss(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_MASK_Wsd(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_MASK_VectorB(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_MASK_VectorW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_MASK_VectorD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_MASK_VectorQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_VectorD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_MASK_VectorD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_VectorQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_MASK_VectorQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_Half_VectorD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_BROADCAST_MASK_Half_VectorD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_FPU == 0	// if FPU is disabled
  BX_SMF void FPU_ESC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void FWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_FPU
  // load/store
  BX_SMF void FLD_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FBLD_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FST_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FST_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FST_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSTP_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIST_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIST_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISTP_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FBSTP_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FISTTP16(bxInstruction_c *) BX_CPP_AttrRegparmN(1); // SSE3
  BX_SMF void FISTTP32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISTTP64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // control
  BX_SMF void FNINIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNCLEX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FLDCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTSW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTSW_AX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // const
  BX_SMF void FLD1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDL2T(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDL2E(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDPI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDLG2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDLN2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDZ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // add
  BX_SMF void FADD_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIADD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIADD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // mul
  BX_SMF void FMUL_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIMUL_WORD_INTEGER (bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIMUL_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // sub
  BX_SMF void FSUB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FISUB_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUBR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUB_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUBR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // div
  BX_SMF void FDIV_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FIDIV_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIVR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIV_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIVR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // compare
  BX_SMF void FCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOM_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOM_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FICOM_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FICOM_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FCMOVB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVBE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVU_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVNB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVNE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVNBE_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOVNU_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // misc
  BX_SMF void FXCH_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPLEGACY(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCHS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FABS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FTST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDECSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FINCSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FFREE_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FFREEP_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void F2XM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FYL2X(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPTAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPATAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXTRACT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPREM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPREM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FYL2XP1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSQRT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSINCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FRNDINT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#undef FSCALE            // <sys/param.h> is #included on Mac OS X from bochs.h
  BX_SMF void FSCALE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSIN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* MMX */
  BX_SMF void PUNPCKLBW_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLWD_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLDQ_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_PqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_PqEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_PqQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_PqQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void EMMS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_EdPqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_EdPqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_QqPqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PANDN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PXOR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_NqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* MMX */

#if BX_SUPPORT_3DNOW
  BX_SMF void PFPNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PI2FW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PI2FD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PF2IW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PF2ID_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPGE_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMIN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCP_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRSQRT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFSUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFADD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPGT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMAX_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCPIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRSQIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFSUBR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPEQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMUL_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCPIT2_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSWAPD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void SYSCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSRET(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  /* SSE */
  BX_SMF void FXSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PREFETCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE */
  BX_SMF void ANDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ORPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XORPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ANDNPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVUPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVUPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSS_VssWssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSS_WssVssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD_VsdWsdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD_WsdVsdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHLPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLHPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPS_MqVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PS_VpsQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PS_VpsQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSI2SS_VssEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSS2SI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSS2SI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UCOMISS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void COMISS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVMSKPS_GdUps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSQRTSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCPPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCPSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFW_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFLW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSS_VssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRW_PqEwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_GdNqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHUFPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVMSKB_GdNq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHUW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSADBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MASKMOVQ_PqNq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE2 */
  BX_SMF void MOVSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PD_VpdQqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PD_VpdQqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSI2SD_VsdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSD2SI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSD2SI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UCOMISD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void COMISD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVMSKPD_GdUpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2PD_VpdWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2PS_VpsWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSD2SS_VssWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSS2SD_VsdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTDQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKLPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSWB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSWB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKHPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLQDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHQDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_VdqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFD_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFHW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_EdVdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_VqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD_VsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRW_VdqEwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRW_VdqEwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_GdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHUFPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQ2Q_PqUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ2DQ_VdqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVMSKB_GdUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2DQ_VqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTDQ2PD_VpdWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULUDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULUDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSADBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MASKMOVDQU_VdqUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE2 */

  /* SSE3 */
  BX_SMF void MOVDDUP_VpdWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSLDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSHDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HADDPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HADDPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HSUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HSUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSUBPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSUBPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE3 */

#if BX_CPU_LEVEL >= 6
  /* SSSE3 */
  BX_SMF void PSHUFB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PALIGNR_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PSHUFB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGND_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PALIGNR_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSSE3 */

  /* SSE4.1 */
  BX_SMF void PBLENDVB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDVPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDVPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PTEST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSDW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBW_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBD_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBQ_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXWD_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXWQ_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXDQ_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBW_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBD_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBQ_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXWD_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXWQ_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXDQ_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHMINPOSUW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDSS_VssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDSD_VsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PBLENDW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRB_EbdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRB_EbdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_EwdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_EwdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRD_EdVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRD_EdVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void PEXTRQ_EqVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRQ_EqVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void PINSRB_VdqEbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRB_VdqEbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRD_VdqEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRD_VdqEdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void PINSRQ_VdqEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRQ_VdqEqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void DPPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DPPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MPSADBW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INSERTPS_VpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSERTPS_VpsWssIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.1 */

  /* SSE4.2 */
  BX_SMF void CRC32_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CRC32_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CRC32_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void CRC32_GdEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void PCMPGTQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPESTRM_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPESTRI_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPISTRM_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPISTRI_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.2 */

  /* MOVBE Intel Atom(R) instruction */
  BX_SMF void MOVBE_GwMw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_GdMd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_MwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_MdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void MOVBE_GqMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_MqGq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  /* MOVBE Intel Atom(R) instruction */
#endif

  /* XSAVE/XRSTOR extensions */
  BX_SMF void XSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XSAVEC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XGETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XSETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* XSAVE/XRSTOR extensions */

#if BX_CPU_LEVEL >= 6
  /* AES instructions */
  BX_SMF void AESIMC_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESKEYGENASSIST_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESENC_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESENCLAST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESDEC_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESDECLAST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCLMULQDQ_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AES instructions */

  /* SHA instructions */
  BX_SMF void SHA1NEXTE_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA1MSG1_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA1MSG2_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA256RNDS2_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA256MSG1_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA256MSG2_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHA1RNDS4_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SHA instructions */

  /* GFNI instructions */
  BX_SMF void GF2P8AFFINEINVQB_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void GF2P8AFFINEQB_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void GF2P8MULB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* GFNI instructions */
#endif

  /* VMX instructions */
  BX_SMF void VMXON(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMXOFF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMLAUNCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMCLEAR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMPTRLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMPTRST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMREAD_EdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMWRITE_GdEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void VMREAD_EqGq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMWRITE_GqEq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void VMFUNC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* VMX instructions */

  /* SVM instructions */
  BX_SMF void VMRUN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMMCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMLOAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SKINIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLGI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STGI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INVLPGA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SVM instructions */

  /* SMX instructions */
  BX_SMF void GETSEC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SMX instructions */

#if BX_CPU_LEVEL >= 6
  /* VMXx2 */
  BX_SMF void INVEPT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INVVPID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* VMXx2 */

  BX_SMF void INVPCID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* CET instructions */
#if BX_SUPPORT_CET
  BX_SMF void INCSSPD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INCSSPQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDSSPD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDSSPQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAVEPREVSSP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSTORSSP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRSSD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRUSSD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRSSQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRUSSQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETSSBSY(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLRSSBSY(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ENDBRANCH32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ENDBRANCH64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  /* CET instructions */

#if BX_SUPPORT_AVX
  /* AVX */
  BX_SMF void VZEROUPPER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VZEROALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVSS_VssHpsWssR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSD_VsdHpdWsdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPS_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPS_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVLPD_VpdHpdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVHPD_VpdHpdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVLHPS_VpsHpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVHLPS_VpsHpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSHDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSLDUP_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDDUP_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKLPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKHPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKLPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKHPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVMSKPS_GdUps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVMSKPD_GdUpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVMSKB_GdUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VHADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VHADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VHSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VHSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSS2SD_VsdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSD2SS_VssWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTDQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2DQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2PD_VpdWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2PS_VpsWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2DQ_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTDQ2PD_VpdWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2DQ_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSI2SD_VsdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSI2SS_VssEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSI2SD_VsdEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSI2SS_VssEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPSS_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPSD_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VROUNDPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VROUNDPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VROUNDSS_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VROUNDSD_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDPPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRSQRTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRSQRTSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRCPPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRCPSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSHUFPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSHUFPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBLENDPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBLENDPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBLENDVB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTEST_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VTESTPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VTESTPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VANDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VANDNPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VORPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VXORPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF128_VdqMdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBLENDVPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBLENDVPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTF128_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF128_WdqVdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF128_WdqVdqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPS_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPD_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERM2F128_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMASKMOVPS_VpsHpsMps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMASKMOVPD_VpdHpdMpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMASKMOVPS_MpsHpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMASKMOVPD_MpdHpdVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRB_VdqHdqEbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRB_VdqHdqEbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRW_VdqHdqEwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRW_VdqHdqEwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRD_VdqHdqEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRD_VdqHdqEdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRQ_VdqHdqEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPINSRQ_VdqHdqEqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTPS_VpsHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTPS_VpsHpsWssIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPH2PS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2PH_WpsVpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX */

  /* AVX2 */
  BX_SMF void VPCMPEQB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSIGNB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSIGNW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSIGND_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPADDB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSB_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBUSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBUSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDUSB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDUSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPAVGB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPAVGW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHUFHW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHUFLW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKUSWB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKSSWB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKUSDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKSSDW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKLBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKHBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKLWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKHWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHUW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULUDQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHRSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADDUBSW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADDWD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMPSADBW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBLENDW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSADBW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHUFB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPALIGNR_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVSXBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVZXBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMQ_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSRAVW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVW_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORVD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORVQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPBROADCASTB_VdqWbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTW_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTD_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTQ_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VGATHERDPS_VpsHps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERQPS_VpsHps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERDPD_VpdHpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERQPD_VpdHpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX2 */

  /* AVX2 FMA */
  BX_SMF void VFMADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSD_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSS_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AVX2 FMA */

  /* BMI */
  BX_SMF void ANDN_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULX_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSI_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSR_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RORX_GdEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SARX_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BEXTR_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BZHI_GdEdBdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXT_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PDEP_GdBdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ANDN_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULX_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSI_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSR_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RORX_GqEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SARX_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BEXTR_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BZHI_GqEqBqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXT_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PDEP_GqBqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* BMI */

  /* FMA4 specific handlers (AMD) */
  BX_SMF void VFMADDSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSS_VssHssWssVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSD_VsdHsdWsdVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* FMA4 specific handlers (AMD) */

  /* XOP (AMD) */
  BX_SMF void VPCMOV_VdqHdqWdqVIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPPERM_VdqHdqWdqVIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHAB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHAW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHAD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHAQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLB_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLW_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLD_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLQ_VdqWdqHdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSSWW_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSSDQL_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSSDD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSSDQH_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSWW_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSDQL_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSDD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMACSDQH_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADCSSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADCSWD_VdqHdqWdqVIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTB_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTW_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTD_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROTQ_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMUB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMUW_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMUD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMUQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFRCZPS_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFRCZPD_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFRCZSS_VssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFRCZSD_VsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUBD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUBQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUWQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHADDUDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBBW_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBWD_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPHSUBDQ_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMIL2PS_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMIL2PD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* XOP (AMD) */

  /* TBM (AMD) */
  BX_SMF void BEXTR_GdEdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCFILL_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCI_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCIC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCS_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSFILL_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSIC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void T1MSKC_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TZMSK_BdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BEXTR_GqEqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCFILL_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCI_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCIC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLCS_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSFILL_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLSIC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void T1MSKC_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TZMSK_BqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* TBM (AMD) */
#endif

#if BX_SUPPORT_AVX
  // VAES: VEX extended AES instructions
  BX_SMF void VAESENC_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VAESENCLAST_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VAESDEC_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VAESDECLAST_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCLMULQDQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  /* GFNI instructions: VEX extended form */
  BX_SMF void VGF2P8AFFINEINVQB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGF2P8AFFINEQB_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGF2P8MULB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  /* AVX encoded VNNI instructions */
  BX_SMF void VPDPBUSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPBUSDS_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPWSSD_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPWSSDS_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // AVX512 OPMASK instructions (VEX encoded)
  BX_SMF void KADDB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDNB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVB_KGbKEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVB_KGbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVB_KEbKGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVB_KGbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVB_GdKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KNOTB_KGbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORTESTB_KGbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTLB_KGbKEbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTRB_KGbKEbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXNORB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXORB_KGbKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KTESTB_KGbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void KADDW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDNW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVW_KGwKEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVW_KGwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVW_KEwKGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVW_KGwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVW_GdKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KUNPCKBW_KGwKHbKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KNOTW_KGwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORTESTW_KGwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTLW_KGwKEwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTRW_KGwKEwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXNORW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXORW_KGwKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KTESTW_KGwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void KADDD_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDD_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDND_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVD_KGdKEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVD_KGdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVD_KEdKGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVD_KGdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVD_GdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KUNPCKWD_KGdKHwKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KNOTD_KGdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORD_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORTESTD_KGdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTLD_KGdKEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTRD_KGdKEdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXNORD_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXORD_KGdKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KTESTD_KGdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void KADDQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KANDNQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVQ_KGqKEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVQ_KGqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVQ_KEqKGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVQ_KGqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KMOVQ_GqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KUNPCKDQ_KGqKHdKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KNOTQ_KGqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KORTESTQ_KGqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTLQ_KGqKEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KSHIFTRQ_KGqKEqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXNORQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KXORQ_KGqKHqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void KTESTQ_KGqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  // AVX512 OPMASK instructions (VEX encoded)
#endif

#if BX_SUPPORT_EVEX
  BX_SMF void VADDPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VADDSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSUBSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMULSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDIVSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMINSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMAXSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTPS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTPD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSQRTSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VFPCLASSPS_MASK_KGwWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFPCLASSPD_MASK_KGbWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFPCLASSSS_MASK_KGbWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFPCLASSSD_MASK_KGbWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VGETEXPPS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETEXPPD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETEXPSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETEXPSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VGETMANTPS_MASK_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETMANTPD_MASK_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETMANTSS_MASK_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGETMANTSD_MASK_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VRNDSCALEPS_MASK_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRNDSCALEPD_MASK_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRNDSCALESS_MASK_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRNDSCALESD_MASK_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VREDUCEPS_MASK_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VREDUCEPD_MASK_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VREDUCESS_MASK_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VREDUCESD_MASK_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VSCALEFPS_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFPD_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFSS_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFSD_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VSCALEFPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCALEFSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VRANGEPS_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRANGEPD_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRANGESS_MASK_VssHpsWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRANGESD_MASK_VsdHpdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VRCP14PS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRCP14PD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRCP14SS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRCP14SD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VRSQRT14PS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRSQRT14PD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRSQRT14SS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VRSQRT14SD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTSS2USI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSS2USI_GqWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSD2USI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSD2USI_GqWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTTSS2USI_GdWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTSS2USI_GqWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTSD2USI_GdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTSD2USI_GqWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTUSI2SD_VsdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUSI2SS_VssEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUSI2SD_VsdEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUSI2SS_VssEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTTPS2UDQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2UDQ_MASK_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2UDQ_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2UDQ_MASK_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPS2UDQ_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2UDQ_MASK_VdqWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2UDQ_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2UDQ_MASK_VdqWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTUDQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUDQ2PS_MASK_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUDQ2PD_VpdWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUDQ2PD_MASK_VpdWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTQQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTQQ2PS_MASK_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUQQ2PS_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUQQ2PS_MASK_VpsWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTQQ2PD_VpdWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTQQ2PD_MASK_VpdWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUQQ2PD_VpdWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTUQQ2PD_MASK_VpdWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPS2QQ_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2QQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2QQ_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2QQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2UQQ_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2UQQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2UQQ_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2UQQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPD2QQ_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2QQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2QQ_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2QQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2UQQ_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPD2UQQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2UQQ_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2UQQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPD2PS_MASK_VpsWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2PD_MASK_VpdWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSS2SD_MASK_VsdWssR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTSD2SS_MASK_VssWsdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPS2DQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPS2DQ_MASK_VdqWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTDQ2PS_MASK_VpsWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPD2DQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTTPD2DQ_MASK_VdqWpdR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTDQ2PD_MASK_VpdWdqR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCVTPH2PS_MASK_VpsWpsR(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2PH_MASK_WpsVpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCVTPS2PH_MASK_WpsVpsIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPABSB_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSW_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPABSQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPADDD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPANDD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPANDND_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPORD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPXORD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKLPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKHPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADDWD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPADDQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPANDQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPANDNQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPORQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPXORQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKLPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VUNPCKHPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULDQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULUDQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPROLD_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPROLQ_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORD_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPRORQ_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLW_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLD_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLQ_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAW_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAD_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAQ_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLW_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLD_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLQ_MASK_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSUBB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBUSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSUBUSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDUSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPADDUSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMINSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMINUW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMAXUW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSRLW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRAW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSRAVW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSRLVW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSLLVW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPAVGB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPAVGW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADDUBSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULLW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHUW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULHRSW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPACKSSWB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKUSWB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKSSDW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPACKUSDW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPUNPCKLBW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKHBW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKLWD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPUNPCKHWD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVAPS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPS_MASK_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPS_MASK_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPD_MASK_VpdWpdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVAPD_MASK_WpdVpdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVUPS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPS_MASK_VpsWpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPS_MASK_WpsVpsM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPD_MASK_VpdWpdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVUPD_MASK_WpdVpdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVDQU8_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDQU8_MASK_VdqWdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDQU8_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVDQU16_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDQU16_MASK_VdqWdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDQU16_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVSD_MASK_VsdWsdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSS_MASK_VssWssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSD_MASK_WsdVsdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSS_MASK_WssVssM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSD_MASK_VsdHpdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSS_MASK_VssHpsWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VMOVSHDUP_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVSLDUP_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMOVDDUP_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VFMADDPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSD_MASK_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSS_MASK_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSUBPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMADDSUBPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBADDPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBADDPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSD_MASK_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFMSUBSS_MASK_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSD_MASK_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMADDSS_MASK_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSD_MASK_VpdHsdWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFNMSUBSS_MASK_VpsHssWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VFIXUPIMMSS_MASK_VssHssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFIXUPIMMSD_MASK_VsdHsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFIXUPIMMPS_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFIXUPIMMPD_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFIXUPIMMPS_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VFIXUPIMMPD_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VBLENDMPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBLENDMPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPBLENDMB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBLENDMW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPCMPB_MASK_KGqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPUB_MASK_KGqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPW_MASK_KGdHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPUW_MASK_KGdHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPD_MASK_KGwHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPUD_MASK_KGwHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPQ_MASK_KGbHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPUQ_MASK_KGbHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPCMPEQB_MASK_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTB_MASK_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQW_MASK_KGdHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTW_MASK_KGdHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQD_MASK_KGwHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTD_MASK_KGwHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPEQQ_MASK_KGbHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCMPGTQ_MASK_KGbHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPTESTMB_MASK_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTNMB_MASK_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTMW_MASK_KGdHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTNMW_MASK_KGdHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTMD_MASK_KGwHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTNMD_MASK_KGwHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTMQ_MASK_KGbHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTESTNMQ_MASK_KGbHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCMPPS_MASK_KGwHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPPD_MASK_KGbHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPSS_MASK_KGbHssWssIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCMPSD_MASK_KGbHsdWsdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSHUFB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMQ_MASK_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSHUFPS_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSHUFPD_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHUFLW_MASK_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHUFHW_MASK_VdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMILPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPS_MASK_VpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMILPD_MASK_VpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VSHUFF32x4_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSHUFF64x2_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VALIGND_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VALIGNQ_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPALIGNR_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VDBPSADBW_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMI2B_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMI2W_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMT2B_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMT2W_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMI2PS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMI2PD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMT2PS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMT2PD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPERMPS_MASK_VpsHpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPERMPD_MASK_VpdHpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VINSERTF32x4_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTF64x2_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTF64x4_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTF64x4_MASK_VpdHpdWpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VINSERTF32x8_MASK_VpsHpsWpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VEXTRACTF32x4_MASK_WpsVpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF32x4_MASK_WpsVpsIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VEXTRACTF64x4_WpdVpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF64x4_WpdVpdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF64x4_MASK_WpdVpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF64x4_MASK_WpdVpdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VEXTRACTF32x8_MASK_WpsVpsIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF32x8_MASK_WpsVpsIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF64x2_MASK_WpdVpdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXTRACTF64x2_MASK_WpdVpdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPBROADCASTB_MASK_VdqWbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTW_MASK_VdqWwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTD_MASK_VdqWdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTQ_MASK_VdqWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTB_MASK_VdqWbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTW_MASK_VdqWwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTD_MASK_VdqWdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTQ_MASK_VdqWqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPBROADCASTB_VdqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTW_VdqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTD_VdqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTQ_VdqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTB_MASK_VdqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTW_MASK_VdqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTD_MASK_VdqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTQ_MASK_VdqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VBROADCASTF32x2_MASK_VpsWqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF32x2_MASK_VpsWqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VBROADCASTF64x2_MASK_VpdMpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF32x4_MASK_VpsMps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF64x4_VpdMpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF32x8_MASK_VpsMps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VBROADCASTF64x4_MASK_VpdMpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPTERNLOGD_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTERNLOGQ_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTERNLOGD_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPTERNLOGQ_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VGATHERDPS_MASK_VpsVSib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERQPS_MASK_VpsVSib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERDPD_MASK_VpdVSib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VGATHERQPD_MASK_VpdVSib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VSCATTERDPS_MASK_VSibVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCATTERQPS_MASK_VSibVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCATTERDPD_MASK_VSibVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VSCATTERQPD_MASK_VSibVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VCOMPRESSPS_MASK_WpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VCOMPRESSPD_MASK_WpdVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXPANDPS_MASK_VpsWpsR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VEXPANDPD_MASK_VpdWpdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPCOMPRESSB_MASK_WdqVdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCOMPRESSW_MASK_WdqVdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPEXPANDB_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPEXPANDW_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVQB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVWB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQD_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVQB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVWB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQD_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVQB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVWB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVDW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQD_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVUSQB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSWB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQD_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVUSQB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSWB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQD_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVUSQB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSWB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSDW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVUSQD_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVSQB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSWB_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQW_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQD_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVSQB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSWB_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQW_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQD_MASK_WdqVdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVSQB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSWB_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSDW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQW_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSQD_MASK_WdqVdqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVSXBW_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXBD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXBQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXWD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXWQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVSXDQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVZXBW_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXBD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXBQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXWD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXWQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVZXDQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPCONFLICTD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPCONFLICTQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPLZCNTD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPLZCNTQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPOPCNTB_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPOPCNTW_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPOPCNTD_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPOPCNTQ_MASK_VdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSHUFBITQMB_MASK_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VP2INTERSECTD_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VP2INTERSECTQ_KGqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPBROADCASTMB2Q_VdqKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPBROADCASTMW2D_VdqKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVM2B_VdqKEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVM2W_VdqKEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVM2D_VdqKEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVM2Q_VdqKEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMOVB2M_KGqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVW2M_KGdWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVD2M_KGwWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMOVQ2M_KGbWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMADD52LUQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADD52LUQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADD52HUQ_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMADD52HUQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPMULTISHIFTQB_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPMULTISHIFTQB_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPDPBUSD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPBUSDS_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPWSSD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPDPWSSDS_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSHLDW_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLDVW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLDD_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLDVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLDQ_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHLDVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void VPSHRDW_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHRDVW_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHRDD_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHRDVD_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHRDQ_MASK_VdqHdqWdqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VPSHRDVQ_MASK_VdqHdqWdqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void LZCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LZCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void LZCNT_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* BMI - TZCNT */
  BX_SMF void TZCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TZCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void TZCNT_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  /* BMI - TZCNT */

  /* SSE4A */
  BX_SMF void EXTRQ_UdqIbIb(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void EXTRQ_VdqUq(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSERTQ_VdqUqIbIb(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSERTQ_VdqUdq(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  /* SSE4A */

  BX_SMF void CMPXCHG8B(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMOVO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMOVO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CWDE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CDQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GwEwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAUSE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void POP_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void POPCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void POPCNT_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void ADCX_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADOX_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void ADCX_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADOX_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  // SMAP
  BX_SMF void CLAC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STAC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  // SMAP

  // RDRAND/RDSEED
  BX_SMF void RDRAND_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDRAND_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void RDRAND_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void RDSEED_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDSEED_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void RDSEED_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_X86_64
  // 64 bit extensions
  BX_SMF void ADD_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LEA_GqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_RAXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EAXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_AXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_ALOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV64S_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV64S_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // repeatable instructions
  BX_SMF void REP_MOVSQ_YqXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSQ_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSQ_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSQ_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASQ_RAXYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // qualified by address size
  BX_SMF void CMPSB64_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW64_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD64_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASB64_ALYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW64_AXYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD64_EAXYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSB64_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW64_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD64_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSB64_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW64_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD64_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSB64_YbXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW64_YwXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD64_YdXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPSQ32_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSQ64_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASQ32_RAXYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASQ64_RAXYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSQ32_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSQ64_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSQ32_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSQ64_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSQ32_YqXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSQ64_YqXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INSB64_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW64_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD64_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void OUTSB64_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW64_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD64_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ENTER64_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_CR0Rq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR2Rq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR3Rq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_CR4Rq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqCR0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqCR2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqCR3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqCR4(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_DqRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqDq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SHLD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV64_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV64_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GqEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GqEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSF_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSWAP_RRX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GqEqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL64_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP64_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CDQE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CQO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void RETnear64_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar64_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMOVO_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_RRXIq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH64_Id(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH64_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP64_Sw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LSS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SGDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SIDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LIDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG16B(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SWAPGS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDFSBASE_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDGSBASE_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDFSBASE_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDGSBASE_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRFSBASE_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRGSBASE_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRFSBASE_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRGSBASE_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LOOPNE64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JRCXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVQ_EqPqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_EqVqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_PqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_VdqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CVTSI2SS_VssEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSI2SD_VsdEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSD2SI_GqWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSS2SI_GqWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSD2SI_GqWsdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSS2SI_GqWssR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif  // #if BX_SUPPORT_X86_64

  BX_SMF void RDTSCP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INVLPG(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void WRMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDTSC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDPMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSENTER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSEXIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MONITOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_PKEYS
  BX_SMF void RDPKRU(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WRPKRU(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void RDPID_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void UndefinedOpcode(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BxError(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  BX_SMF void BxEndTrace(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_CPU_LEVEL >= 6
  BX_SMF void BxNoSSE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_AVX
  BX_SMF void BxNoAVX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_EVEX
  BX_SMF void BxNoOpMask(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BxNoEVEX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#endif

  BX_CPP_INLINE BX_SMF Bit32u BxResolve32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_CPP_INLINE BX_SMF Bit64u BxResolve64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_AVX
  BX_SMF bx_address BxResolveGatherD(bxInstruction_c *, unsigned) BX_CPP_AttrRegparmN(2);
  BX_SMF bx_address BxResolveGatherQ(bxInstruction_c *, unsigned) BX_CPP_AttrRegparmN(2);
#endif
// <TAG-CLASS-CPU-END>

#if BX_DEBUGGER
  BX_SMF void       dbg_take_dma(void);
  BX_SMF bool       dbg_set_eflags(Bit32u val);
  BX_SMF void       dbg_set_eip(bx_address val);
  BX_SMF bool       dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no);
  BX_SMF bool       dbg_set_sreg(unsigned sreg_no, bx_segment_reg_t *sreg);
  BX_SMF void       dbg_get_tr(bx_dbg_sreg_t *sreg);
  BX_SMF void       dbg_get_ldtr(bx_dbg_sreg_t *sreg);
  BX_SMF void       dbg_get_gdtr(bx_dbg_global_sreg_t *sreg);
  BX_SMF void       dbg_get_idtr(bx_dbg_global_sreg_t *sreg);
  BX_SMF unsigned   dbg_query_pending(void);
#endif
#if BX_DEBUGGER || BX_GDBSTUB
  BX_SMF bool  dbg_instruction_epilog(void);
#endif
  BX_SMF bool  dbg_xlate_linear2phy(bx_address linear, bx_phy_address *phy, bx_address *lpf_mask = 0, bool verbose = 0);
#if BX_SUPPORT_VMX >= 2
  BX_SMF bool dbg_translate_guest_physical(bx_phy_address guest_paddr, bx_phy_address *phy, bool verbose = 0);
#endif
#if BX_LARGE_RAMFILE
  BX_SMF bool check_addr_in_tlb_buffers(const Bit8u *addr, const Bit8u *end);
#endif
  BX_SMF void atexit(void);

  // now for some ancillary functions...
  BX_SMF void cpu_loop(void);
#if BX_SUPPORT_SMP
  BX_SMF void cpu_run_trace(void);
#endif
  BX_SMF bool handleAsyncEvent(void);
  BX_SMF bool handleWaitForEvent(void);
  BX_SMF void InterruptAcknowledge(void);

  BX_SMF void boundaryFetch(const Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *);

  BX_SMF bxICacheEntry_c *serveICacheMiss(Bit32u eipBiased, bx_phy_address pAddr);
  BX_SMF bxICacheEntry_c* getICacheEntry(void);
  BX_SMF bool mergeTraces(bxICacheEntry_c *entry, bxInstruction_c *i, bx_phy_address pAddr);
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS && BX_ENABLE_TRACE_LINKING
  BX_SMF void linkTrace(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void prefetch(void);
  BX_SMF void updateFetchModeMask(void);
  BX_SMF BX_CPP_INLINE void invalidate_prefetch_q(void)
  {
    BX_CPU_THIS_PTR eipPageWindowSize = 0;
  }

  BX_SMF BX_CPP_INLINE void invalidate_stack_cache(void)
  {
    BX_CPU_THIS_PTR espPageWindowSize = 0;
  }

  BX_SMF bool write_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len, bool align = false) BX_CPP_AttrRegparmN(4);
  BX_SMF bool read_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len, bool align = false) BX_CPP_AttrRegparmN(4);
  BX_SMF bool execute_virtual_checks(bx_segment_reg_t *seg, Bit32u offset, unsigned len) BX_CPP_AttrRegparmN(3);

  BX_SMF Bit8u read_linear_byte(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_linear_word(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_linear_dword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_linear_qword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
#if BX_CPU_LEVEL >= 6
  BX_SMF void read_linear_xmmword(unsigned seg, bx_address off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_linear_xmmword_aligned(unsigned seg, bx_address off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_linear_ymmword(unsigned seg, bx_address off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_linear_ymmword_aligned(unsigned seg, bx_address off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_linear_zmmword(unsigned seg, bx_address off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_linear_zmmword_aligned(unsigned seg, bx_address off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF void write_linear_byte(unsigned seg, bx_address offset, Bit8u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_word(unsigned seg, bx_address offset, Bit16u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_dword(unsigned seg, bx_address offset, Bit32u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_qword(unsigned seg, bx_address offset, Bit64u data) BX_CPP_AttrRegparmN(3);
#if BX_CPU_LEVEL >= 6
  BX_SMF void write_linear_xmmword(unsigned seg, bx_address offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_xmmword_aligned(unsigned seg, bx_address offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_ymmword(unsigned seg, bx_address off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_ymmword_aligned(unsigned seg, bx_address off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_zmmword(unsigned seg, bx_address off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_linear_zmmword_aligned(unsigned seg, bx_address off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF void tickle_read_linear(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF void tickle_read_virtual_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF void tickle_read_virtual(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit8u read_virtual_byte_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_virtual_word_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_virtual_dword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_virtual_qword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
#if BX_CPU_LEVEL >= 6
  BX_SMF void read_virtual_xmmword_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_xmmword_aligned_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_ymmword_32(unsigned seg, Bit32u off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_ymmword_aligned_32(unsigned seg, Bit32u off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_zmmword_32(unsigned seg, Bit32u off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_zmmword_aligned_32(unsigned seg, Bit32u off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF void write_virtual_byte_32(unsigned seg, Bit32u offset, Bit8u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_word_32(unsigned seg, Bit32u offset, Bit16u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dword_32(unsigned seg, Bit32u offset, Bit32u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_qword_32(unsigned seg, Bit32u offset, Bit64u data) BX_CPP_AttrRegparmN(3);
#if BX_CPU_LEVEL >= 6
  BX_SMF void write_virtual_xmmword_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_xmmword_aligned_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_ymmword_32(unsigned seg, Bit32u off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_ymmword_aligned_32(unsigned seg, Bit32u off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_zmmword_32(unsigned seg, Bit32u off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_zmmword_aligned_32(unsigned seg, Bit32u off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF Bit8u read_virtual_byte(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_virtual_word(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_virtual_dword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_virtual_qword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
#if BX_CPU_LEVEL >= 6
  BX_SMF void read_virtual_xmmword(unsigned seg, bx_address off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_xmmword_aligned(unsigned seg, bx_address off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_ymmword(unsigned seg, bx_address off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_ymmword_aligned(unsigned seg, bx_address off, BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_zmmword(unsigned seg, bx_address off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_zmmword_aligned(unsigned seg, bx_address off, BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF void write_virtual_byte(unsigned seg, bx_address offset, Bit8u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_word(unsigned seg, bx_address offset, Bit16u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dword(unsigned seg, bx_address offset, Bit32u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_qword(unsigned seg, bx_address offset, Bit64u data) BX_CPP_AttrRegparmN(3);
#if BX_CPU_LEVEL >= 6
  BX_SMF void write_virtual_xmmword(unsigned seg, bx_address offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_xmmword_aligned(unsigned seg, bx_address offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_ymmword(unsigned seg, bx_address off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_ymmword_aligned(unsigned seg, bx_address off, const BxPackedYmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_zmmword(unsigned seg, bx_address off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_zmmword_aligned(unsigned seg, bx_address off, const BxPackedZmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF Bit8u read_RMW_linear_byte(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_RMW_linear_word(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_RMW_linear_dword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_RMW_linear_qword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit8u read_RMW_virtual_byte_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_RMW_virtual_word_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_RMW_virtual_dword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_RMW_virtual_qword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit8u read_RMW_virtual_byte(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_RMW_virtual_word(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_RMW_virtual_dword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_RMW_virtual_qword(unsigned seg, bx_address offset) BX_CPP_AttrRegparmN(2);

  BX_SMF void write_RMW_linear_byte(Bit8u val8) BX_CPP_AttrRegparmN(1);
  BX_SMF void write_RMW_linear_word(Bit16u val16) BX_CPP_AttrRegparmN(1);
  BX_SMF void write_RMW_linear_dword(Bit32u val32) BX_CPP_AttrRegparmN(1);
  BX_SMF void write_RMW_linear_qword(Bit64u val64) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_X86_64
  BX_SMF void read_RMW_linear_dqword_aligned_64(unsigned seg, bx_address laddr, Bit64u *hi, Bit64u *lo);
  BX_SMF void write_RMW_linear_dqword(Bit64u hi, Bit64u lo);
#endif

  // write of word/dword to new stack could happen only in legacy mode
  BX_SMF void write_new_stack_word(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit16u data);
  BX_SMF void write_new_stack_dword(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit32u data);
  BX_SMF void write_new_stack_qword(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit64u data);

  BX_SMF void write_new_stack_word(bx_address laddr, unsigned curr_pl, Bit16u data);
  BX_SMF void write_new_stack_dword(bx_address laddr, unsigned curr_pl, Bit32u data);
  BX_SMF void write_new_stack_qword(bx_address laddr, unsigned curr_pl, Bit64u data);

  // dedicated optimized stack access methods
  BX_SMF void stack_write_byte(bx_address offset, Bit8u data) BX_CPP_AttrRegparmN(2);
  BX_SMF void stack_write_word(bx_address offset, Bit16u data) BX_CPP_AttrRegparmN(2);
  BX_SMF void stack_write_dword(bx_address offset, Bit32u data) BX_CPP_AttrRegparmN(2);
  BX_SMF void stack_write_qword(bx_address offset, Bit64u data) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit8u stack_read_byte(bx_address offset) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit16u stack_read_word(bx_address offset) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u stack_read_dword(bx_address offset) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u stack_read_qword(bx_address offset) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_CET
  BX_SMF void shadow_stack_write_dword(bx_address offset, unsigned curr_pl, Bit32u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void shadow_stack_write_qword(bx_address offset, unsigned curr_pl, Bit64u data) BX_CPP_AttrRegparmN(3);

  BX_SMF Bit32u shadow_stack_read_dword(bx_address offset, unsigned curr_pl) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u shadow_stack_read_qword(bx_address offset, unsigned curr_pl) BX_CPP_AttrRegparmN(2);

  BX_SMF bool shadow_stack_lock_cmpxchg8b(bx_address offset, unsigned curr_pl, Bit64u data, Bit64u expected_data) BX_CPP_AttrRegparmN(4);
  BX_SMF bool shadow_stack_atomic_set_busy(bx_address offset, unsigned curr_pl) BX_CPP_AttrRegparmN(2);
  BX_SMF bool shadow_stack_atomic_clear_busy(bx_address offset, unsigned curr_pl) BX_CPP_AttrRegparmN(2);
#endif

  BX_SMF void stackPrefetch(bx_address offset, unsigned len) BX_CPP_AttrRegparmN(2);

  // dedicated system linear read/write methods with no segment
  BX_SMF Bit8u  system_read_byte(bx_address laddr) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit16u system_read_word(bx_address laddr) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u system_read_dword(bx_address laddr) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u system_read_qword(bx_address laddr) BX_CPP_AttrRegparmN(1);

  BX_SMF void system_write_byte(bx_address laddr, Bit8u data) BX_CPP_AttrRegparmN(2);
  BX_SMF void system_write_word(bx_address laddr, Bit16u data) BX_CPP_AttrRegparmN(2);
  BX_SMF void system_write_dword(bx_address laddr, Bit32u data) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit8u* v2h_read_byte(bx_address laddr, bool user) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit8u* v2h_write_byte(bx_address laddr, bool user) BX_CPP_AttrRegparmN(2);

  BX_SMF void branch_near16(Bit16u new_IP) BX_CPP_AttrRegparmN(1);
  BX_SMF void branch_near32(Bit32u new_EIP) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void branch_near64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void branch_far(bx_selector_t *selector,
       bx_descriptor_t *descriptor, bx_address rip, unsigned cpl);

#if BX_SUPPORT_REPEAT_SPEEDUPS
  BX_SMF Bit32u FastRepMOVSB(unsigned srcSeg, Bit32u srcOff, unsigned dstSeg, Bit32u dstOff, Bit32u byteCount, Bit32u granularity);
  BX_SMF Bit32u FastRepMOVSB(bx_address laddrSrc, bx_address laddrDst, Bit64u byteCount, Bit32u granularity);

  BX_SMF Bit32u FastRepSTOSB(unsigned dstSeg, Bit32u dstOff, Bit8u  val, Bit32u  byteCount);
  BX_SMF Bit32u FastRepSTOSW(unsigned dstSeg, Bit32u dstOff, Bit16u val, Bit32u  wordCount);
  BX_SMF Bit32u FastRepSTOSD(unsigned dstSeg, Bit32u dstOff, Bit32u val, Bit32u dwordCount);

  BX_SMF Bit32u FastRepSTOSB(bx_address laddrDst, Bit8u  val, Bit32u  byteCount);
  BX_SMF Bit32u FastRepSTOSW(bx_address laddrDst, Bit16u val, Bit32u  wordCount);
  BX_SMF Bit32u FastRepSTOSD(bx_address laddrDst, Bit32u val, Bit32u dwordCount);

  BX_SMF Bit32u FastRepINSW(Bit32u dstOff, Bit16u port, Bit32u wordCount);
  BX_SMF Bit32u FastRepOUTSW(unsigned srcSeg, Bit32u srcOff, Bit16u port, Bit32u wordCount);
#endif

  BX_SMF void repeat(bxInstruction_c *i, BxRepIterationPtr_tR execute) BX_CPP_AttrRegparmN(2);
  BX_SMF void repeat_ZF(bxInstruction_c *i, BxRepIterationPtr_tR execute) BX_CPP_AttrRegparmN(2);

  // linear address for access_linear expected to be canonical !
  BX_SMF int access_read_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned xlate_rw, Bit32u ac_mask, void *data);
  BX_SMF int access_write_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned xlate_rw, Bit32u ac_mask, void *data);
  BX_SMF void page_fault(unsigned fault, bx_address laddr, unsigned user, unsigned rw);

  BX_SMF void access_read_physical(bx_phy_address paddr, unsigned len, void *data);
  BX_SMF void access_write_physical(bx_phy_address paddr, unsigned len, void *data);

  BX_SMF bx_hostpageaddr_t getHostMemAddr(bx_phy_address addr, unsigned rw);

  // linear address for translate_linear expected to be canonical !
  BX_SMF bx_phy_address translate_linear(bx_TLB_entry *entry, bx_address laddr, unsigned user, unsigned rw);
  BX_SMF bx_phy_address translate_linear_legacy(bx_address laddr, Bit32u &lpf_mask, unsigned user, unsigned rw);
  BX_SMF void update_access_dirty(bx_phy_address *entry_addr, Bit32u *entry, BxMemtype *entry_memtype, unsigned leaf, unsigned write);
#if BX_CPU_LEVEL >= 6
  BX_SMF bx_phy_address translate_linear_load_PDPTR(bx_address laddr, unsigned user, unsigned rw);
  BX_SMF bx_phy_address translate_linear_PAE(bx_address laddr, Bit32u &lpf_mask, unsigned user, unsigned rw);
  BX_SMF int check_entry_PAE(const char *s, Bit64u entry, Bit64u reserved, unsigned rw, bool *nx_fault);
  BX_SMF void update_access_dirty_PAE(bx_phy_address *entry_addr, Bit64u *entry, BxMemtype *entry_memtype, unsigned max_level, unsigned leaf, unsigned write);
#endif
#if BX_SUPPORT_X86_64
  BX_SMF bx_phy_address translate_linear_long_mode(bx_address laddr, Bit32u &lpf_mask, Bit32u &pkey, unsigned user, unsigned rw);
#endif
#if BX_SUPPORT_VMX >= 2
  BX_SMF bx_phy_address translate_guest_physical(bx_phy_address guest_paddr, bx_address guest_laddr, bool guest_laddr_valid, bool is_page_walk, unsigned rw, bool supervisor_shadow_stack = false);
  BX_SMF void update_ept_access_dirty(bx_phy_address *entry_addr, Bit64u *entry, BxMemtype eptptr_memtype, unsigned leaf, unsigned write);
  BX_SMF bool is_eptptr_valid(Bit64u eptptr);
  BX_SMF bool spp_walk(bx_phy_address guest_paddr, bx_address guest_laddr, BxMemtype memtype);
#endif
#if BX_SUPPORT_SVM
  BX_SMF void nested_page_fault(unsigned fault, bx_phy_address guest_paddr, unsigned rw, unsigned is_page_walk);
  BX_SMF bx_phy_address nested_walk_long_mode(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk);
  BX_SMF bx_phy_address nested_walk_PAE(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk);
  BX_SMF bx_phy_address nested_walk_legacy(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk);
  BX_SMF bx_phy_address nested_walk(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk);
#endif
#if BX_SUPPORT_MEMTYPE
  BX_SMF BxMemtype memtype_by_mtrr(bx_phy_address paddr) BX_CPP_AttrRegparmN(1);
  BX_SMF BxMemtype memtype_by_pat(unsigned pat) BX_CPP_AttrRegparmN(1);
  BX_SMF BxMemtype resolve_memtype(BxMemtype mtrr_memtype, BxMemtype pat_memtype = BX_MEMTYPE_WB) BX_CPP_AttrRegparmN(2);
#endif

#if BX_CPU_LEVEL >= 6
  BX_SMF void TLB_flushNonGlobal(void);
#endif
  BX_SMF void TLB_flush(void);
  BX_SMF void TLB_invlpg(bx_address laddr);
  BX_SMF void inhibit_interrupts(unsigned mask);
  BX_SMF bool interrupts_inhibited(unsigned mask);
  BX_SMF const char *strseg(bx_segment_reg_t *seg);
  BX_SMF void interrupt(Bit8u vector, unsigned type, bool push_error, Bit16u error_code);
  BX_SMF void real_mode_int(Bit8u vector, bool push_error, Bit16u error_code);
  BX_SMF void protected_mode_int(Bit8u vector, unsigned soft_int, bool push_error, Bit16u error_code);
#if BX_SUPPORT_X86_64
  BX_SMF void long_mode_int(Bit8u vector, unsigned soft_int, bool push_error, Bit16u error_code);
#endif
  BX_SMF void exception(unsigned vector, Bit16u error_code)
                  BX_CPP_AttrNoReturn();
  BX_SMF void init_SMRAM(void);
  BX_SMF int  int_number(unsigned s);

  BX_SMF bool SetCR0(bxInstruction_c *i, bx_address val);
  BX_SMF bool check_CR0(bx_address val) BX_CPP_AttrRegparmN(1);
  BX_SMF bool SetCR3(bx_address val) BX_CPP_AttrRegparmN(1);
#if BX_CPU_LEVEL >= 5
  BX_SMF bool SetCR4(bxInstruction_c *i, bx_address val);
  BX_SMF bool check_CR4(bx_address val) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u get_cr4_allow_mask(void);
#endif
#if BX_CPU_LEVEL >= 6
  BX_SMF bool CheckPDPTR(bx_phy_address cr3_val) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_VMX >= 2
  BX_SMF bool CheckPDPTR(Bit64u *pdptr) BX_CPP_AttrRegparmN(1);
#endif
#if BX_CPU_LEVEL >= 5
  BX_SMF bool SetEFER(bx_address val) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF bx_address read_CR0(void);
#if BX_CPU_LEVEL >= 5
  BX_SMF bx_address read_CR4(void);
#endif
#if BX_CPU_LEVEL >= 6
  BX_SMF Bit32u ReadCR8(bxInstruction_c *i);
  BX_SMF void WriteCR8(bxInstruction_c *i, bx_address val);
#endif

  BX_SMF void reset(unsigned source);
  BX_SMF void shutdown(void);
  BX_SMF void enter_sleep_state(unsigned state);
  BX_SMF void handleCpuModeChange(void);
  BX_SMF void handleCpuContextChange(void);
  BX_SMF void handleInterruptMaskChange(void);
#if BX_CPU_LEVEL >= 4
  BX_SMF void handleAlignmentCheck(void);
#endif
#if BX_CPU_LEVEL >= 6
  BX_SMF void handleSseModeChange(void);
  BX_SMF void handleAvxModeChange(void);
#endif

#if BX_SUPPORT_AVX
  BX_SMF void avx_masked_load8(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *dst, Bit64u mask);
  BX_SMF void avx_masked_load16(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *dst, Bit32u mask);
  BX_SMF void avx_masked_load32(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *dst, Bit32u mask);
  BX_SMF void avx_masked_load64(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *dst, Bit32u mask);
  BX_SMF void avx_masked_store8(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit64u mask);
  BX_SMF void avx_masked_store16(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask);
  BX_SMF void avx_masked_store32(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask);
  BX_SMF void avx_masked_store64(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask);
#endif

#if BX_SUPPORT_EVEX
  BX_SMF void avx512_write_regb_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned vlen, Bit64u mask);
  BX_SMF void avx512_write_regw_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned vlen, Bit32u mask);
  BX_SMF void avx512_write_regd_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned vlen, Bit32u mask);
  BX_SMF void avx512_write_regq_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned vlen, Bit32u mask);
#endif

#if BX_CPU_LEVEL >= 5
  BX_SMF bool rdmsr(Bit32u index, Bit64u *val_64) BX_CPP_AttrRegparmN(2);
  BX_SMF bool handle_unknown_rdmsr(Bit32u index, Bit64u *val_64) BX_CPP_AttrRegparmN(2);
  BX_SMF bool wrmsr(Bit32u index, Bit64u  val_64) BX_CPP_AttrRegparmN(2);
  BX_SMF bool handle_unknown_wrmsr(Bit32u index, Bit64u  val_64) BX_CPP_AttrRegparmN(2);
#endif

#if BX_SUPPORT_APIC
  BX_SMF bool relocate_apic(Bit64u val_64);
#endif

  BX_SMF void load_segw(bxInstruction_c *i, unsigned seg) BX_CPP_AttrRegparmN(2);
  BX_SMF void load_segd(bxInstruction_c *i, unsigned seg) BX_CPP_AttrRegparmN(2);
  BX_SMF void load_segq(bxInstruction_c *i, unsigned seg) BX_CPP_AttrRegparmN(2);

  BX_SMF void jmp_far16(bxInstruction_c *i, Bit16u cs_raw, Bit16u disp16);
  BX_SMF void jmp_far32(bxInstruction_c *i, Bit16u cs_raw, Bit32u disp32);
  BX_SMF void call_far16(bxInstruction_c *i, Bit16u cs_raw, Bit16u disp16);
  BX_SMF void call_far32(bxInstruction_c *i, Bit16u cs_raw, Bit32u disp32);
  BX_SMF void task_gate(bxInstruction_c *i, bx_selector_t *selector, bx_descriptor_t *gate_descriptor, unsigned source);
  BX_SMF void jump_protected(bxInstruction_c *i, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);
  BX_SMF void jmp_call_gate(bx_selector_t *selector, bx_descriptor_t *gate_descriptor) BX_CPP_AttrRegparmN(2);
  BX_SMF void call_gate(bx_descriptor_t *gate_descriptor) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void jmp_call_gate64(bx_selector_t *selector) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void call_protected(bxInstruction_c *i, Bit16u cs, bx_address disp) BX_CPP_AttrRegparmN(3);
#if BX_SUPPORT_X86_64
  BX_SMF void call_gate64(bx_selector_t *selector) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void return_protected(bxInstruction_c *i, Bit16u pop_bytes) BX_CPP_AttrRegparmN(2);
  BX_SMF void iret_protected(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void long_iret(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
#if BX_SUPPORT_CET
  BX_SMF void shadow_stack_switch(bx_address new_SSP) BX_CPP_AttrRegparmN(1);
  BX_SMF void call_far_shadow_stack_push(Bit16u cs, bx_address lip, bx_address old_ssp) BX_CPP_AttrRegparmN(3);
  BX_SMF bx_address shadow_stack_restore(Bit16u raw_cs_selector, const bx_descriptor_t &cs_descriptor, bx_address return_rip) BX_CPP_AttrRegparmN(3);
#endif
  BX_SMF void validate_seg_reg(unsigned seg);
  BX_SMF void validate_seg_regs(void);
  BX_SMF void stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32);
  BX_SMF void iret16_stack_return_from_v86(bxInstruction_c *);
  BX_SMF void iret32_stack_return_from_v86(bxInstruction_c *);
  BX_SMF int  v86_redirect_interrupt(Bit8u vector);
  BX_SMF void init_v8086_mode(void);
  BX_SMF void task_switch_load_selector(bx_segment_reg_t *seg,
                 bx_selector_t *selector, Bit16u raw_selector, Bit8u cs_rpl);
  BX_SMF void task_switch(bxInstruction_c *i, bx_selector_t *selector, bx_descriptor_t *descriptor,
                 unsigned source, Bit32u dword1, Bit32u dword2, bool push_error = 0, Bit32u error_code = 0);
  BX_SMF void get_SS_ESP_from_TSS(unsigned pl, Bit16u *ss, Bit32u *esp);
#if BX_SUPPORT_X86_64
  BX_SMF Bit64u get_RSP_from_TSS(unsigned pl);
#endif
  BX_SMF void write_flags(Bit16u flags, bool change_IOPL, bool change_IF) BX_CPP_AttrRegparmN(3);
  BX_SMF void writeEFlags(Bit32u eflags, Bit32u changeMask) BX_CPP_AttrRegparmN(2); // Newer variant.
  BX_SMF void write_eflags_fpu_compare(int float_relation);
  BX_SMF Bit32u force_flags(void);
  BX_SMF Bit32u read_eflags(void) { return BX_CPU_THIS_PTR force_flags(); }

  BX_SMF bool allow_io(bxInstruction_c *i, Bit16u addr, unsigned len) BX_CPP_AttrRegparmN(3);
  BX_SMF Bit32u  get_descriptor_l(const bx_descriptor_t *) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u  get_descriptor_h(const bx_descriptor_t *) BX_CPP_AttrRegparmN(1);
  BX_SMF bool set_segment_ar_data(bx_segment_reg_t *seg, bool valid, Bit16u raw_selector,
                         bx_address base, Bit32u limit_scaled, Bit16u ar_data);
  BX_SMF void    check_cs(bx_descriptor_t *descriptor, Bit16u cs_raw, Bit8u check_rpl, Bit8u check_cpl);
  // the basic assumption of the code that load_cs and load_ss cannot fail !
  BX_SMF void    load_cs(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  BX_SMF void    load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl) BX_CPP_AttrRegparmN(3);
  BX_SMF void    touch_segment(bx_selector_t *selector, bx_descriptor_t *descriptor) BX_CPP_AttrRegparmN(2);
  BX_SMF void    fetch_raw_descriptor(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2, unsigned exception_no);
  BX_SMF bool fetch_raw_descriptor2(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2) BX_CPP_AttrRegparmN(3);
  BX_SMF void    load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value) BX_CPP_AttrRegparmN(2);
  BX_SMF void    load_null_selector(bx_segment_reg_t *seg, unsigned value) BX_CPP_AttrRegparmN(2);
#if BX_SUPPORT_X86_64
  BX_SMF void    fetch_raw_descriptor_64(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2, Bit32u *dword3, unsigned exception_no);
  BX_SMF bool fetch_raw_descriptor2_64(const bx_selector_t *selector,
                         Bit32u *dword1, Bit32u *dword2, Bit32u *dword3);
#endif
  BX_SMF void    push_16(Bit16u value16) BX_CPP_AttrRegparmN(1);
  BX_SMF void    push_32(Bit32u value32) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit16u  pop_16(void);
  BX_SMF Bit32u  pop_32(void);
#if BX_SUPPORT_X86_64
  BX_SMF void    push_64(Bit64u value64) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u  pop_64(void);
#endif
#if BX_SUPPORT_CET
  BX_SMF void    shadow_stack_push_32(Bit32u value32) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u  shadow_stack_pop_32(void);
  BX_SMF void    shadow_stack_push_64(Bit64u value64) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u  shadow_stack_pop_64(void);
#endif
  BX_SMF void    sanity_checks(void);
  BX_SMF void    assert_checks(void);

  BX_SMF void    enter_system_management_mode(void);
  BX_SMF bool    resume_from_system_management_mode(BX_SMM_State *smm_state);
  BX_SMF void    smram_save_state(Bit32u *smm_saved_state);
  BX_SMF bool    smram_restore_state(const Bit32u *smm_saved_state);

  BX_SMF void    raise_INTR(void);
  BX_SMF void    clear_INTR(void);

  BX_SMF void    deliver_INIT(void);
  BX_SMF void    deliver_NMI(void);
  BX_SMF void    deliver_SMI(void);
  BX_SMF void    deliver_SIPI(unsigned vector);
  BX_SMF void    debug(bx_address offset);
  BX_SMF void    debug_disasm_instruction(bx_address offset);

#if BX_X86_DEBUGGER
  // x86 hardware debug support
  BX_SMF bool hwbreakpoint_check(bx_address laddr, unsigned opa, unsigned opb);
#if BX_CPU_LEVEL >= 5
  BX_SMF void    iobreakpoint_match(unsigned port, unsigned len);
#endif
  BX_SMF Bit32u  code_breakpoint_match(bx_address laddr);
  BX_SMF void    hwbreakpoint_match(bx_address laddr, unsigned len, unsigned rw);
  BX_SMF Bit32u  hwdebug_compare(bx_address laddr, unsigned len, unsigned opa, unsigned opb);
#endif

  BX_SMF void init_FetchDecodeTables(void);

#if BX_SUPPORT_APIC
  BX_SMF BX_CPP_INLINE Bit8u get_apic_id(void) { return BX_CPU_THIS_PTR bx_cpuid; }
#endif

  BX_SMF BX_CPP_INLINE bool is_cpu_extension_supported(unsigned extension) {
    assert(extension < BX_ISA_EXTENSION_LAST);
    return BX_CPU_THIS_PTR ia_extensions_bitmask[extension / 32] & (1 << (extension % 32));
  }

  BX_SMF BX_CPP_INLINE unsigned which_cpu(void) { return BX_CPU_THIS_PTR bx_cpuid; }
  BX_SMF BX_CPP_INLINE const bx_gen_reg_t *get_gen_regfile() { return BX_CPU_THIS_PTR gen_reg; }

  BX_SMF BX_CPP_INLINE Bit64u get_icount(void) { return BX_CPU_THIS_PTR icount; }
  BX_SMF BX_CPP_INLINE void sync_icount(void) { BX_CPU_THIS_PTR icount_last_sync = BX_CPU_THIS_PTR icount; }
  BX_SMF BX_CPP_INLINE Bit64u get_icount_last_sync(void) { return BX_CPU_THIS_PTR icount_last_sync; }

  BX_SMF BX_CPP_INLINE bx_address get_instruction_pointer(void);

  BX_SMF BX_CPP_INLINE Bit32u get_eip(void) { return (BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_EIP].dword.erx); }
  BX_SMF BX_CPP_INLINE Bit16u get_ip (void) { return (BX_CPU_THIS_PTR gen_reg[BX_16BIT_REG_IP].word.rx); }
#if BX_SUPPORT_X86_64
  BX_SMF BX_CPP_INLINE Bit64u get_rip(void) { return (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_RIP].rrx); }
#endif

  BX_SMF BX_CPP_INLINE Bit32u get_cpl(void) { return (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl); }

#if BX_SUPPORT_CET
  BX_SMF BX_CPP_INLINE bx_address get_ssp(void) { return (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_SSP].rrx); }
#endif

  BX_SMF BX_CPP_INLINE Bit8u get_reg8l(unsigned reg);
  BX_SMF BX_CPP_INLINE Bit8u get_reg8h(unsigned reg);
  BX_SMF BX_CPP_INLINE void  set_reg8l(unsigned reg, Bit8u val);
  BX_SMF BX_CPP_INLINE void  set_reg8h(unsigned reg, Bit8u val);

  BX_SMF BX_CPP_INLINE Bit16u get_reg16(unsigned reg);
  BX_SMF BX_CPP_INLINE void   set_reg16(unsigned reg, Bit16u val);
  BX_SMF BX_CPP_INLINE Bit32u get_reg32(unsigned reg);
  BX_SMF BX_CPP_INLINE void   set_reg32(unsigned reg, Bit32u val);
#if BX_SUPPORT_X86_64
  BX_SMF BX_CPP_INLINE Bit64u get_reg64(unsigned reg);
  BX_SMF BX_CPP_INLINE void   set_reg64(unsigned reg, Bit64u val);
#endif
#if BX_SUPPORT_EVEX
  BX_SMF BX_CPP_INLINE Bit64u get_opmask(unsigned reg);
  BX_SMF BX_CPP_INLINE void set_opmask(unsigned reg, Bit64u val);
#endif

#if BX_CPU_LEVEL >= 6
  BX_SMF BX_CPP_INLINE unsigned get_cr8();
#endif

  BX_SMF bx_address get_segment_base(unsigned seg);

  // The linear address must be truncated to the 32-bit when CPU is not
  // executing in long64 mode.  The function  must  be used  to compute
  // linear address everywhere when a code is shared between long64 and
  // legacy mode. For legacy mode only  just use Bit32u to store linear 
  // address value.
  BX_SMF bx_address get_laddr(unsigned seg, bx_address offset);

  BX_SMF Bit32u get_laddr32(unsigned seg, Bit32u offset);
#if BX_SUPPORT_X86_64
  BX_SMF Bit64u get_laddr64(unsigned seg, Bit64u offset);
#endif

  BX_SMF bx_address agen_read(unsigned seg, bx_address offset, unsigned len);
  BX_SMF Bit32u agen_read32(unsigned seg, Bit32u offset, unsigned len);
  BX_SMF Bit32u agen_read_execute32(unsigned seg, Bit32u offset, unsigned len);
  BX_SMF bx_address agen_read_aligned(unsigned seg, bx_address offset, unsigned len);
  BX_SMF Bit32u agen_read_aligned32(unsigned seg, Bit32u offset, unsigned len);

  BX_SMF bx_address agen_write(unsigned seg, bx_address offset, unsigned len);
  BX_SMF Bit32u agen_write32(unsigned seg, Bit32u offset, unsigned len);
  BX_SMF bx_address agen_write_aligned(unsigned seg, bx_address offset, unsigned len);
  BX_SMF Bit32u agen_write_aligned32(unsigned seg, Bit32u offset, unsigned len);

  DECLARE_EFLAG_ACCESSOR   (ID,  21)
  DECLARE_EFLAG_ACCESSOR   (VIP, 20)
  DECLARE_EFLAG_ACCESSOR   (VIF, 19)
  DECLARE_EFLAG_ACCESSOR   (AC,  18)
  DECLARE_EFLAG_ACCESSOR   (VM,  17)
  DECLARE_EFLAG_ACCESSOR   (RF,  16)
  DECLARE_EFLAG_ACCESSOR   (NT,  14)
  DECLARE_EFLAG_ACCESSOR_IOPL(   12)
  DECLARE_EFLAG_ACCESSOR   (DF,  10)
  DECLARE_EFLAG_ACCESSOR   (IF,   9)
  DECLARE_EFLAG_ACCESSOR   (TF,   8)

  BX_SMF BX_CPP_INLINE bool real_mode(void);
  BX_SMF BX_CPP_INLINE bool smm_mode(void);
  BX_SMF BX_CPP_INLINE bool protected_mode(void);
  BX_SMF BX_CPP_INLINE bool v8086_mode(void);
  BX_SMF BX_CPP_INLINE bool long_mode(void);
  BX_SMF BX_CPP_INLINE bool long64_mode(void);
  BX_SMF BX_CPP_INLINE unsigned get_cpu_mode(void);

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  BX_SMF BX_CPP_INLINE bool alignment_check(void);
#endif

#if BX_CPU_LEVEL >= 5
  BX_SMF Bit64u get_TSC();
  BX_SMF void   set_TSC(Bit64u tsc);
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_SMF Bit64u get_TSC_VMXAdjust(Bit64u tsc);
#endif
#endif

#if BX_SUPPORT_PKEYS
  BX_SMF void set_PKeys(Bit32u pkru, Bit32u pkrs);
#endif

#if BX_SUPPORT_FPU
  BX_SMF void print_state_FPU(void);
  BX_SMF void prepareFPU(bxInstruction_c *i, bool = 1);
  BX_SMF void FPU_check_pending_exceptions(void);
  BX_SMF void FPU_update_last_instruction(bxInstruction_c *i);
  BX_SMF void FPU_stack_underflow(bxInstruction_c *i, int stnr, int pop_stack = 0);
  BX_SMF void FPU_stack_overflow(bxInstruction_c *i);
  BX_SMF unsigned FPU_exception(bxInstruction_c *i, unsigned exception, bool = 0);
  BX_SMF bx_address fpu_save_environment(bxInstruction_c *i);
  BX_SMF bx_address fpu_load_environment(bxInstruction_c *i);
  BX_SMF Bit8u pack_FPU_TW(Bit16u tag_word);
  BX_SMF Bit16u unpack_FPU_TW(Bit16u tag_byte);
  BX_SMF Bit16u x87_get_FCS(void);
  BX_SMF Bit16u x87_get_FDS(void);
#endif

#if BX_CPU_LEVEL >= 5
  BX_SMF void prepareMMX(void);
  BX_SMF void prepareFPU2MMX(void); /* cause transition from FPU to MMX technology state */
  BX_SMF void print_state_MMX(void);
#endif

#if BX_CPU_LEVEL >= 6
  BX_SMF void check_exceptionsSSE(int);
  BX_SMF void print_state_SSE(void);

  BX_SMF void prepareXSAVE(void);
  BX_SMF void print_state_AVX(void);
#endif

#if BX_CPU_LEVEL >= 6
  BX_SMF void xsave_xrestor_init(void);
  BX_SMF Bit32u get_xcr0_allow_mask(void);
  BX_SMF Bit32u get_ia32_xss_allow_mask(void);
  BX_SMF Bit32u get_xinuse_vector(Bit32u requested_feature_bitmap);

  BX_SMF bool xsave_x87_state_xinuse(void);
  BX_SMF void xsave_x87_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_x87_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_x87_state(void);

  BX_SMF bool xsave_sse_state_xinuse(void);
  BX_SMF void xsave_sse_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_sse_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_sse_state(void);

#if BX_SUPPORT_AVX
  BX_SMF bool xsave_ymm_state_xinuse(void);
  BX_SMF void xsave_ymm_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_ymm_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_ymm_state(void);
#if BX_SUPPORT_EVEX
  BX_SMF bool xsave_opmask_state_xinuse(void);
  BX_SMF void xsave_opmask_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_opmask_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_opmask_state(void);

  BX_SMF bool xsave_zmm_hi256_state_xinuse(void);
  BX_SMF void xsave_zmm_hi256_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_zmm_hi256_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_zmm_hi256_state(void);

  BX_SMF bool xsave_hi_zmm_state_xinuse(void);
  BX_SMF void xsave_hi_zmm_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_hi_zmm_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_hi_zmm_state(void);
#endif
#endif

#if BX_SUPPORT_PKEYS
  BX_SMF bool xsave_pkru_state_xinuse(void);
  BX_SMF void xsave_pkru_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_pkru_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_pkru_state(void);
#endif

#if BX_SUPPORT_CET
  BX_SMF bool xsave_cet_u_state_xinuse(void);
  BX_SMF void xsave_cet_u_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_cet_u_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_cet_u_state(void);

  BX_SMF bool xsave_cet_s_state_xinuse(void);
  BX_SMF void xsave_cet_s_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_cet_s_state(bxInstruction_c *i, bx_address offset);
  BX_SMF void xrstor_init_cet_s_state(void);
#endif
#endif

#if BX_SUPPORT_CET
 BX_SMF bool ShadowStackEnabled(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF bool ShadowStackWriteEnabled(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF bool EndbranchEnabled(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF bool EndbranchEnabledAndNotSuppressed(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF bool WaitingForEndbranch(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF bool LegacyEndbranchTreatment(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF void track_indirect(unsigned cpl) BX_CPP_AttrRegparmN(1);
 BX_SMF void track_indirect_if_not_suppressed(bxInstruction_c *i, unsigned cpl) BX_CPP_AttrRegparmN(2);
 BX_SMF void reset_endbranch_tracker(unsigned cpl, bool suppress=false) BX_CPP_AttrRegparmN(2);
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  BX_SMF bool   is_monitor(bx_phy_address addr, unsigned len);
  BX_SMF void   check_monitor(bx_phy_address addr, unsigned len);
  BX_SMF void   wakeup_monitor(void);
#endif

#if BX_SUPPORT_VMX
  BX_SMF Bit16u VMread16(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u VMread32(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u VMread64(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF bx_address VMread_natural(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMwrite16(unsigned encoding, Bit16u val_16) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMwrite32(unsigned encoding, Bit32u val_32) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMwrite64(unsigned encoding, Bit64u val_64) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMwrite_natural(unsigned encoding, bx_address val) BX_CPP_AttrRegparmN(2);

  BX_SMF Bit64u vmread(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF void vmwrite(unsigned encoding, Bit64u val_64) BX_CPP_AttrRegparmN(2);
#if BX_SUPPORT_VMX >= 2
  BX_SMF Bit64u vmread_shadow(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF void vmwrite_shadow(unsigned encoding, Bit64u val_64) BX_CPP_AttrRegparmN(2);
#endif

  BX_SMF BX_CPP_INLINE void VMsucceed(void) { clearEFlagsOSZAPC(); }
  BX_SMF BX_CPP_INLINE void VMfailInvalid(void) { setEFlagsOSZAPC(EFlagsCFMask); }
  BX_SMF void VMfail(Bit32u error_code);
  BX_SMF void VMabort(VMX_vmabort_code error_code);

  BX_SMF Bit32u LoadMSRs(Bit32u msr_cnt, bx_phy_address pAddr);
  BX_SMF Bit32u StoreMSRs(Bit32u msr_cnt, bx_phy_address pAddr);
  BX_SMF Bit32u VMXReadRevisionID(bx_phy_address pAddr);
  BX_SMF VMX_error_code VMenterLoadCheckVmControls(void);
  BX_SMF VMX_error_code VMenterLoadCheckHostState(void);
  BX_SMF Bit32u VMenterLoadCheckGuestState(Bit64u *qualification);
  BX_SMF void VMenterInjectEvents(void);
  BX_SMF void VMexit(Bit32u reason, Bit64u qualification);
  BX_SMF void VMexitSaveGuestState(void);
  BX_SMF void VMexitSaveGuestMSRs(void);
  BX_SMF void VMexitLoadHostState(void);
  BX_SMF void set_VMCSPTR(Bit64u vmxptr);
  BX_SMF void init_vmx_capabilities(void);
#if BX_SUPPORT_VMX >= 2
  BX_SMF void init_ept_vpid_capabilities(void);
  BX_SMF void init_vmfunc_capabilities(void);
#endif
  BX_SMF void init_pin_based_vmexec_ctrls(void);
  BX_SMF void init_secondary_proc_based_vmexec_ctrls(void);
  BX_SMF void init_primary_proc_based_vmexec_ctrls(void);
  BX_SMF void init_vmexit_ctrls(void);
  BX_SMF void init_vmentry_ctrls(void);
  BX_SMF void init_VMCS(void);
  BX_SMF bool vmcs_field_supported(Bit32u encoding);
  BX_SMF void register_vmx_state(bx_param_c *parent);
#if BX_SUPPORT_VMX >= 2
  BX_SMF Bit16u VMX_Get_Current_VPID(void);
#endif
#if BX_SUPPORT_X86_64
  BX_SMF bool is_virtual_apic_page(bx_phy_address paddr) BX_CPP_AttrRegparmN(1);
  BX_SMF bool virtual_apic_access_vmexit(unsigned offset, unsigned len) BX_CPP_AttrRegparmN(2);
  BX_SMF bx_phy_address VMX_Virtual_Apic_Read(bx_phy_address paddr, unsigned len, void *data);
  BX_SMF void VMX_Virtual_Apic_Write(bx_phy_address paddr, unsigned len, void *data);
  BX_SMF Bit32u VMX_Read_Virtual_APIC(unsigned offset);
  BX_SMF void VMX_Write_Virtual_APIC(unsigned offset, Bit32u val32);
  BX_SMF void VMX_TPR_Virtualization(void);
  BX_SMF bool Virtualize_X2APIC_Write(unsigned msr, Bit64u val_64);
  BX_SMF void VMX_Virtual_Apic_Access_Trap(void);
#if BX_SUPPORT_VMX >= 2
  BX_SMF void vapic_set_vector(unsigned apic_arrbase, Bit8u vector);
  BX_SMF Bit8u vapic_clear_and_find_highest_priority_int(unsigned apic_arrbase, Bit8u vector);
  BX_SMF void VMX_Write_VICR(void);
  BX_SMF void VMX_PPR_Virtualization(void);
  BX_SMF void VMX_EOI_Virtualization(void);
  BX_SMF void VMX_Self_IPI_Virtualization(Bit8u vector);
  BX_SMF void VMX_Evaluate_Pending_Virtual_Interrupts(void);
  BX_SMF void VMX_Deliver_Virtual_Interrupt(void);
  BX_SMF void vmx_page_modification_logging(Bit64u guest_addr, unsigned dirty_update);
#endif
#if BX_SUPPORT_VMX >= 2
  BX_SMF Bit16u VMread16_Shadow(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit32u VMread32_Shadow(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u VMread64_Shadow(unsigned encoding) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMwrite16_Shadow(unsigned encoding, Bit16u val_16) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMwrite32_Shadow(unsigned encoding, Bit32u val_32) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMwrite64_Shadow(unsigned encoding, Bit64u val_64) BX_CPP_AttrRegparmN(2);
  BX_SMF bool Vmexit_Vmread(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF bool Vmexit_Vmwrite(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
#endif
#endif
  // vmexit reasons
  BX_SMF void VMexit_Instruction(bxInstruction_c *i, Bit32u reason, bool rw = BX_READ) BX_CPP_AttrRegparmN(3);
  BX_SMF void VMexit_Event(unsigned type, unsigned vector,
       Bit16u errcode, bool errcode_valid, Bit64u qualification = 0);
  BX_SMF void VMexit_TripleFault(void);
  BX_SMF void VMexit_ExtInterrupt(void);
  BX_SMF void VMexit_TaskSwitch(Bit16u tss_selector, unsigned source) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMexit_PAUSE(void);
  BX_SMF bool VMexit_CLTS(void);
  BX_SMF void VMexit_MSR(unsigned op, Bit32u msr) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMexit_IO(bxInstruction_c *i, unsigned port, unsigned len) BX_CPP_AttrRegparmN(3);
  BX_SMF Bit32u VMexit_LMSW(bxInstruction_c *i, Bit32u msw) BX_CPP_AttrRegparmN(2);
  BX_SMF bx_address VMexit_CR0_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMexit_CR3_Read(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMexit_CR3_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  BX_SMF bx_address VMexit_CR4_Write(bxInstruction_c *i, bx_address) BX_CPP_AttrRegparmN(2);
  BX_SMF void VMexit_CR8_Read(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMexit_CR8_Write(bxInstruction_c *i) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMexit_DR_Access(unsigned read, unsigned dr, unsigned reg);
#if BX_SUPPORT_VMX >= 2
  BX_SMF void Virtualization_Exception(Bit64u qualification, Bit64u guest_physical, Bit64u guest_linear);
  BX_SMF void vmfunc_eptp_switching(void);
#endif
#endif

#if BX_SUPPORT_SVM
  BX_SMF void set_VMCBPTR(Bit64u vmcbptr);
  BX_SMF void SvmEnterSaveHostState(SVM_HOST_STATE *host);
  BX_SMF bool SvmEnterLoadCheckControls(SVM_CONTROLS *ctrls);
  BX_SMF bool SvmEnterLoadCheckGuestState(void);
  BX_SMF bool SvmInjectEvents(void);
  BX_SMF void Svm_Vmexit(int reason, Bit64u exitinfo1 = 0, Bit64u exitinfo2 = 0);
  BX_SMF void SvmExitSaveGuestState(void);
  BX_SMF void SvmExitLoadHostState(SVM_HOST_STATE *host);
  BX_SMF Bit8u vmcb_read8(unsigned offset);
  BX_SMF Bit16u vmcb_read16(unsigned offset);
  BX_SMF Bit32u vmcb_read32(unsigned offset);
  BX_SMF Bit64u vmcb_read64(unsigned offset);
  BX_SMF void vmcb_write8(unsigned offset, Bit8u val_8);
  BX_SMF void vmcb_write16(unsigned offset, Bit16u val_16);
  BX_SMF void vmcb_write32(unsigned offset, Bit32u val_32);
  BX_SMF void vmcb_write64(unsigned offset, Bit64u val_64);
  BX_SMF void svm_segment_read(bx_segment_reg_t *seg, unsigned offset);
  BX_SMF void svm_segment_write(bx_segment_reg_t *seg, unsigned offset);
  BX_SMF void SvmInterceptException(unsigned type, unsigned vector,
       Bit16u errcode, bool errcode_valid, Bit64u qualification = 0);
  BX_SMF void SvmInterceptIO(bxInstruction_c *i, unsigned port, unsigned len);
  BX_SMF void SvmInterceptMSR(unsigned op, Bit32u msr);
  BX_SMF void SvmInterceptTaskSwitch(Bit16u tss_selector, unsigned source, bool push_error, Bit32u error_code);
  BX_SMF void SvmInterceptPAUSE(void);
  BX_SMF void VirtualInterruptAcknowledge(void);
  BX_SMF void register_svm_state(bx_param_c *parent);
#endif

#if BX_CONFIGURE_MSRS
  int load_MSRs(const char *file);
#endif
};

#if BX_CPU_LEVEL >= 5
BX_CPP_INLINE void BX_CPU_C::prepareMMX(void)
{
  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();
}

BX_CPP_INLINE void BX_CPU_C::prepareFPU2MMX(void)
{
  BX_CPU_THIS_PTR the_i387.twd = 0;
  BX_CPU_THIS_PTR the_i387.tos = 0; /* reset FPU Top-Of-Stack */
}
#endif

#if BX_CPU_LEVEL >= 6
BX_CPP_INLINE void BX_CPU_C::prepareXSAVE(void)
{
  if(! BX_CPU_THIS_PTR cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);
}
#endif

// Can be used as LHS or RHS.
#define RMAddr(i)  (BX_CPU_THIS_PTR address_xlation.rm_addr)

#if defined(NEED_CPU_REG_SHORTCUTS)

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::BxResolve64(bxInstruction_c *i)
{
  Bit64u eaddr = (Bit64u) (BX_READ_64BIT_REG(i->sibBase()) + i->displ32s());
  if (i->sibIndex() != 4)
    eaddr += BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale();
  return eaddr;
}
#endif

BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(1) BX_CPU_C::BxResolve32(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + i->displ32s());
  if (i->sibIndex() != 4)
    eaddr += BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  return eaddr & i->asize_mask();
}

#include "stack.h"

#define PRESERVE_RSP { BX_CPU_THIS_PTR prev_rsp = RSP; }
#if BX_SUPPORT_CET
#define PRESERVE_SSP { BX_CPU_THIS_PTR prev_ssp = SSP; }
#else
#define PRESERVE_SSP 
#endif

#define RSP_SPECULATIVE {              \
  BX_CPU_THIS_PTR speculative_rsp = 1; \
  PRESERVE_RSP;                        \
  PRESERVE_SSP;                        \
}

#define RSP_COMMIT { BX_CPU_THIS_PTR speculative_rsp = 0; }

#endif // defined(NEED_CPU_REG_SHORTCUTS)

//
// bit 0 - CS.D_B
// bit 1 - long64 mode (CS.L)
// bit 2 - SSE_OK
// bit 3 - AVX_OK
// bit 4 - OPMASK_OK
// bit 5 - EVEX_OK
//

enum {
  BX_FETCH_MODE_IS32_MASK = (1 << 0),
  BX_FETCH_MODE_IS64_MASK = (1 << 1),
  BX_FETCH_MODE_SSE_OK    = (1 << 2),
  BX_FETCH_MODE_AVX_OK    = (1 << 3),
  BX_FETCH_MODE_OPMASK_OK = (1 << 4),
  BX_FETCH_MODE_EVEX_OK   = (1 << 5)
};

//
// updateFetchModeMask - has to be called everytime 
//   CS.L / CS.D_B / CR0.PE, CR0.TS or CR0.EM / CR4.OSFXSR / CR4.OSXSAVE changes
//
BX_CPP_INLINE void BX_CPU_C::updateFetchModeMask(void)
{
  BX_CPU_THIS_PTR fetchModeMask =
#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_EVEX
     (BX_CPU_THIS_PTR evex_ok << 5) | (BX_CPU_THIS_PTR opmask_ok << 4) |
#endif
#if BX_SUPPORT_AVX
     (BX_CPU_THIS_PTR avx_ok << 3) |
#endif
     (BX_CPU_THIS_PTR sse_ok << 2) |
#endif
#if BX_SUPPORT_X86_64
    ((BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)<<1) |
#endif
     unsigned(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b);    // typecast to keep MSVC warnings silent

  BX_CPU_THIS_PTR user_pl = // CPL == 3
     (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl == 3);
}

#if BX_X86_DEBUGGER
enum {
  BX_HWDebugInstruction = 0x00,
  BX_HWDebugMemW        = 0x01,
  BX_HWDebugIO          = 0x02,
  BX_HWDebugMemRW       = 0x03
};
#endif

BX_CPP_INLINE bx_address BX_CPU_C::get_segment_base(unsigned seg)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (seg < BX_SEG_REG_FS) return 0;
  }
#endif
  return BX_CPU_THIS_PTR sregs[seg].cache.u.segment.base;
}

BX_CPP_INLINE Bit32u BX_CPU_C::get_laddr32(unsigned seg, Bit32u offset)
{
  return (Bit32u) BX_CPU_THIS_PTR sregs[seg].cache.u.segment.base + offset;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u BX_CPU_C::get_laddr64(unsigned seg, Bit64u offset)
{
  if (seg < BX_SEG_REG_FS)
    return offset;
  else
    return BX_CPU_THIS_PTR sregs[seg].cache.u.segment.base + offset;
}
#endif

BX_CPP_INLINE bx_address BX_CPU_C::get_laddr(unsigned seg, bx_address offset)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return get_laddr64(seg, offset);
  }
#endif
  return get_laddr32(seg, (Bit32u) offset);
}

// same as agen_read32 but also allow access to execute only segments
BX_CPP_INLINE Bit32u BX_CPU_C::agen_read_execute32(unsigned s, Bit32u offset, unsigned len)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  if (seg->cache.valid & SegAccessROK4G) {
    return offset;
  }

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
      return get_laddr32(s, offset);
    }
  }

  if (!execute_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);

  return get_laddr32(s, offset);
}

BX_CPP_INLINE Bit32u BX_CPU_C::agen_read32(unsigned s, Bit32u offset, unsigned len)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  if (seg->cache.valid & SegAccessROK4G) {
    return offset;
  }

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
      return get_laddr32(s, offset);
    }
  }

  if (!read_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);

  return get_laddr32(s, offset);
}

BX_CPP_INLINE Bit32u BX_CPU_C::agen_read_aligned32(unsigned s, Bit32u offset, unsigned len)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  if (seg->cache.valid & SegAccessROK4G) {
    return offset;
  }

  if (seg->cache.valid & SegAccessROK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
      return get_laddr32(s, offset);
    }
  }

  if (!read_virtual_checks(seg, offset, len, true /* aligned */))
    exception(int_number(s), 0);

  return get_laddr32(s, offset);
}

BX_CPP_INLINE Bit32u BX_CPU_C::agen_write32(unsigned s, Bit32u offset, unsigned len)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  if (seg->cache.valid & SegAccessWOK4G) {
    return offset;
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
      return get_laddr32(s, offset);
    }
  }

  if (!write_virtual_checks(seg, offset, len))
    exception(int_number(s), 0);

  return get_laddr32(s, offset);
}

BX_CPP_INLINE Bit32u BX_CPU_C::agen_write_aligned32(unsigned s, Bit32u offset, unsigned len)
{
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[s];

  if (seg->cache.valid & SegAccessWOK4G) {
    return offset;
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-len+1)) {
      return get_laddr32(s, offset);
    }
  }

  if (!write_virtual_checks(seg, offset, len, true /* aligned */))
    exception(int_number(s), 0);

  return get_laddr32(s, offset);
}

BX_CPP_INLINE bx_address BX_CPU_C::agen_read(unsigned s, bx_address offset, unsigned len)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return get_laddr64(s, offset);
  }
#endif
  return agen_read32(s, (Bit32u)offset, len);
}

BX_CPP_INLINE bx_address BX_CPU_C::agen_read_aligned(unsigned s, bx_address offset, unsigned len)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return get_laddr64(s, offset);
  }
#endif
  return agen_read_aligned32(s, (Bit32u)offset, len);
}

BX_CPP_INLINE bx_address BX_CPU_C::agen_write(unsigned s, bx_address offset, unsigned len)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return get_laddr64(s, offset);
  }
#endif
  return agen_write32(s, (Bit32u)offset, len);
}

BX_CPP_INLINE bx_address BX_CPU_C::agen_write_aligned(unsigned s, bx_address offset, unsigned len)
{
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return get_laddr64(s, offset);
  }
#endif
  return agen_write_aligned32(s, (Bit32u)offset, len);
}

#include "access.h"

BX_CPP_INLINE Bit8u BX_CPU_C::get_reg8l(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (BX_CPU_THIS_PTR gen_reg[reg].word.byte.rl);
}

BX_CPP_INLINE void BX_CPU_C::set_reg8l(unsigned reg, Bit8u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  BX_CPU_THIS_PTR gen_reg[reg].word.byte.rl = val;
}

BX_CPP_INLINE Bit8u BX_CPU_C::get_reg8h(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (BX_CPU_THIS_PTR gen_reg[reg].word.byte.rh);
}

BX_CPP_INLINE void BX_CPU_C::set_reg8h(unsigned reg, Bit8u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  BX_CPU_THIS_PTR gen_reg[reg].word.byte.rh = val;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE bx_address BX_CPU_C::get_instruction_pointer(void)
{
  return BX_CPU_THIS_PTR get_rip();
}
#else
BX_CPP_INLINE bx_address BX_CPU_C::get_instruction_pointer(void)
{
  return BX_CPU_THIS_PTR get_eip();
}
#endif

BX_CPP_INLINE Bit16u BX_CPU_C::get_reg16(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (BX_CPU_THIS_PTR gen_reg[reg].word.rx);
}

BX_CPP_INLINE void BX_CPU_C::set_reg16(unsigned reg, Bit16u val)
{
  assert(reg < BX_GENERAL_REGISTERS);
  BX_CPU_THIS_PTR gen_reg[reg].word.rx = val;
}

BX_CPP_INLINE Bit32u BX_CPU_C::get_reg32(unsigned reg)
{
  assert(reg < BX_GENERAL_REGISTERS);
  return (BX_CPU_THIS_PTR gen_reg[reg].dword.erx);
}

BX_CPP_INLINE void BX_CPU_C::set_reg32(unsigned reg, Bit32u val)
{
   assert(reg < BX_GENERAL_REGISTERS);
   BX_CPU_THIS_PTR gen_reg[reg].dword.erx = val;
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE Bit64u BX_CPU_C::get_reg64(unsigned reg)
{
   assert(reg < BX_GENERAL_REGISTERS);
   return (BX_CPU_THIS_PTR gen_reg[reg].rrx);
}

BX_CPP_INLINE void BX_CPU_C::set_reg64(unsigned reg, Bit64u val)
{
   assert(reg < BX_GENERAL_REGISTERS);
   BX_CPU_THIS_PTR gen_reg[reg].rrx = val;
}
#endif

#if BX_SUPPORT_EVEX
BX_CPP_INLINE Bit64u BX_CPU_C::get_opmask(unsigned reg)
{
   assert(reg < 8);
   return (BX_CPU_THIS_PTR opmask[reg].rrx);
}

BX_CPP_INLINE void BX_CPU_C::set_opmask(unsigned reg, Bit64u val)
{
   assert(reg < 8);
   BX_CPU_THIS_PTR opmask[reg].rrx = val;
}
#endif

#if BX_CPU_LEVEL >= 6
// CR8 is aliased to APIC->TASK PRIORITY register
//   APIC.TPR[7:4] = CR8[3:0]
//   APIC.TPR[3:0] = 0
// Reads of CR8 return zero extended APIC.TPR[7:4]
BX_CPP_INLINE unsigned BX_CPU_C::get_cr8(void)
{
   return (BX_CPU_THIS_PTR lapic.get_tpr() >> 4) & 0xf;
}
#endif

BX_CPP_INLINE bool BX_CPU_C::real_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode == BX_MODE_IA32_REAL);
}

BX_CPP_INLINE bool BX_CPU_C::smm_mode(void)
{
  return (BX_CPU_THIS_PTR in_smm);
}

BX_CPP_INLINE bool BX_CPU_C::v8086_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode == BX_MODE_IA32_V8086);
}

BX_CPP_INLINE bool BX_CPU_C::protected_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode >= BX_MODE_IA32_PROTECTED);
}

BX_CPP_INLINE bool BX_CPU_C::long_mode(void)
{
#if BX_SUPPORT_X86_64
  return BX_CPU_THIS_PTR efer.get_LMA();
#else
  return 0;
#endif
}

BX_CPP_INLINE bool BX_CPU_C::long64_mode(void)
{
#if BX_SUPPORT_X86_64
  return (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
#else
  return 0;
#endif
}

BX_CPP_INLINE unsigned BX_CPU_C::get_cpu_mode(void)
{
  return (BX_CPU_THIS_PTR cpu_mode);
}

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
BX_CPP_INLINE bool BX_CPU_C::alignment_check(void)
{
  return BX_CPU_THIS_PTR alignment_check_mask;
}
#endif

IMPLEMENT_EFLAG_ACCESSOR   (ID,  21)
IMPLEMENT_EFLAG_ACCESSOR   (VIP, 20)
IMPLEMENT_EFLAG_ACCESSOR   (VIF, 19)
IMPLEMENT_EFLAG_ACCESSOR   (AC,  18)
IMPLEMENT_EFLAG_ACCESSOR   (VM,  17)
IMPLEMENT_EFLAG_ACCESSOR   (RF,  16)
IMPLEMENT_EFLAG_ACCESSOR   (NT,  14)
IMPLEMENT_EFLAG_ACCESSOR_IOPL(   12)
IMPLEMENT_EFLAG_ACCESSOR   (DF,  10)
IMPLEMENT_EFLAG_ACCESSOR   (IF,   9)
IMPLEMENT_EFLAG_ACCESSOR   (TF,   8)

IMPLEMENT_EFLAG_SET_ACCESSOR   (ID,  21)
IMPLEMENT_EFLAG_SET_ACCESSOR   (VIP, 20)
IMPLEMENT_EFLAG_SET_ACCESSOR   (VIF, 19)
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
IMPLEMENT_EFLAG_SET_ACCESSOR_AC(     18)
#else
IMPLEMENT_EFLAG_SET_ACCESSOR   (AC,  18)
#endif
IMPLEMENT_EFLAG_SET_ACCESSOR_VM(     17)
IMPLEMENT_EFLAG_SET_ACCESSOR_RF(     16)
IMPLEMENT_EFLAG_SET_ACCESSOR   (NT,  14)
IMPLEMENT_EFLAG_SET_ACCESSOR   (DF,  10)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF(      9)
IMPLEMENT_EFLAG_SET_ACCESSOR_TF(      8)
                               
// hardware task switching
enum {
  BX_TASK_FROM_CALL = 0,
  BX_TASK_FROM_IRET = 1,
  BX_TASK_FROM_JUMP = 2,
  BX_TASK_FROM_INT  = 3
};

// exception types for interrupt method
enum {
  BX_EXTERNAL_INTERRUPT = 0,
  BX_NMI = 2,
  BX_HARDWARE_EXCEPTION = 3,  // all exceptions except #BP and #OF
  BX_SOFTWARE_INTERRUPT = 4,
  BX_PRIVILEGED_SOFTWARE_INTERRUPT = 5,
  BX_SOFTWARE_EXCEPTION = 6
};

#if BX_CPU_LEVEL >= 6
enum {
  BX_INVPCID_INDIVIDUAL_ADDRESS_NON_GLOBAL_INVALIDATION,
  BX_INVPCID_SINGLE_CONTEXT_NON_GLOBAL_INVALIDATION,
  BX_INVPCID_ALL_CONTEXT_INVALIDATION,
  BX_INVPCID_ALL_CONTEXT_NON_GLOBAL_INVALIDATION
};
#endif

class bxInstruction_c;

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS

#define BX_COMMIT_INSTRUCTION(i) {                     \
  BX_CPU_THIS_PTR prev_rip = RIP; /* commit new RIP */ \
  BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, (i));            \
  BX_CPU_THIS_PTR icount++;                            \
}

#define BX_EXECUTE_INSTRUCTION(i) {                    \
  BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, (i));           \
  RIP += (i)->ilen();                                  \
  return BX_CPU_CALL_METHOD(i->execute1, (i));         \
}

#define BX_NEXT_TRACE(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  return;                                              \
}

#if BX_ENABLE_TRACE_LINKING == 0
#define linkTrace(i)
#endif

#define BX_LINK_TRACE(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  linkTrace(i);                                        \
  return;                                              \
}

#define BX_NEXT_INSTR(i) {                             \
  BX_COMMIT_INSTRUCTION(i);                            \
  if (BX_CPU_THIS_PTR async_event) return;             \
  ++i;                                                 \
  BX_EXECUTE_INSTRUCTION(i);                           \
}

#else // BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS

#define BX_NEXT_TRACE(i) { return; }
#define BX_NEXT_INSTR(i) { return; }
#define BX_LINK_TRACE(i) { return; }

#endif

#endif  // #ifndef BX_CPU_H
