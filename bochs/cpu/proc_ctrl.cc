/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2019  The Bochs Project
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "cpuid.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#include "pc_system.h"
#include "gui/gui.h"

#include "wide_int.h"
#include "decoder/ia_opcodes.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxError(bxInstruction_c *i)
{
  unsigned ia_opcode = i->getIaOpcode();

  if (ia_opcode == BX_IA_ERROR) {
    BX_DEBUG(("BxError: Encountered an unknown instruction (signalling #UD)"));

    if (LOG_THIS getonoff(LOGLEV_DEBUG))
      debug_disasm_instruction(BX_CPU_THIS_PTR prev_rip);
  }
  else {
    BX_DEBUG(("%s: instruction not supported - signalling #UD", get_bx_opcode_name(ia_opcode)));
    for (unsigned n=0; n<BX_ISA_EXTENSIONS_ARRAY_SIZE; n++)
      BX_DEBUG(("ia_extensions_bitmask[%d]: %08x", n, BX_CPU_THIS_PTR ia_extensions_bitmask[n]));
  }

  exception(BX_UD_EXCEPTION, 0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UndefinedOpcode(bxInstruction_c *i)
{
  BX_DEBUG(("UndefinedOpcode: generate #UD exception"));
  exception(BX_UD_EXCEPTION, 0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOP(bxInstruction_c *i)
{
  // No operation.

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PAUSE(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    VMexit_PAUSE();
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_PAUSE)) SvmInterceptPAUSE();
  }
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PREFETCH(bxInstruction_c *i)
{
#if BX_INSTRUMENTATION
  BX_INSTR_PREFETCH_HINT(BX_CPU_ID, i->src(), i->seg(), BX_CPU_RESOLVE_ADDR(i));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CPUID(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_CPUID, 0);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_CPUID)) Svm_Vmexit(SVM_VMEXIT_CPUID);
  }
#endif

  struct cpuid_function_t leaf;
  BX_CPU_THIS_PTR cpuid->get_cpuid_leaf(EAX, ECX, &leaf);

  RAX = leaf.eax;
  RBX = leaf.ebx;
  RCX = leaf.ecx;
  RDX = leaf.edx;
#endif

  BX_NEXT_INSTR(i);
}

//
// The shutdown state is very similar to the state following the exection
// if HLT instruction. In this mode the processor stops executing
// instructions until #NMI, #SMI, #RESET or #INIT is received. If
// shutdown occurs why in NMI interrupt handler or in SMM, a hardware
// reset must be used to restart the processor execution.
//
void BX_CPU_C::shutdown(void)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_SHUTDOWN)) Svm_Vmexit(SVM_VMEXIT_SHUTDOWN);
  }
#endif

  enter_sleep_state(BX_ACTIVITY_STATE_SHUTDOWN);

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

void BX_CPU_C::enter_sleep_state(unsigned state)
{
  switch(state) {
  case BX_ACTIVITY_STATE_ACTIVE:
    BX_ASSERT(0); // should not be used for entering active CPU state
    break;

  case BX_ACTIVITY_STATE_HLT:
    break;

  case BX_ACTIVITY_STATE_WAIT_FOR_SIPI:
    mask_event(BX_EVENT_INIT | BX_EVENT_SMI | BX_EVENT_NMI); // FIXME: all events should be masked
    // fall through - mask interrupts as well

  case BX_ACTIVITY_STATE_SHUTDOWN:
    BX_CPU_THIS_PTR clear_IF(); // masking interrupts
    break;

  case BX_ACTIVITY_STATE_MWAIT:
  case BX_ACTIVITY_STATE_MWAIT_IF:
    break;

  default:
    BX_PANIC(("enter_sleep_state: unknown state %d", state));
  }

  // artificial trap bit, why use another variable.
  BX_CPU_THIS_PTR activity_state = state;
  BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
  // Execution completes.  The processor will remain in a sleep
  // state until one of the wakeup conditions is met.

  BX_INSTR_HLT(BX_CPU_ID);

#if BX_DEBUGGER
  bx_dbg_halt(BX_CPU_ID);
#endif

#if BX_USE_IDLE_HACK
  bx_gui->sim_is_idle();
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::HLT(bxInstruction_c *i)
{
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_DEBUG(("HLT: %s priveledge check failed, CPL=%d, generate #GP(0)",
        cpu_mode_string(BX_CPU_THIS_PTR cpu_mode), CPL));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (! BX_CPU_THIS_PTR get_IF()) {
    BX_INFO(("WARNING: HLT instruction with IF=0!"));
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.HLT_VMEXIT()) {
      VMexit(VMX_VMEXIT_HLT, 0);
    }
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_HLT)) Svm_Vmexit(SVM_VMEXIT_HLT);
  }
#endif

  // stops instruction execution and places the processor in a
  // HALT state. An enabled interrupt, NMI, or reset will resume
  // execution. If interrupt (including NMI) is used to resume
  // execution after HLT, the saved CS:eIP points to instruction
  // following HLT.
  enter_sleep_state(BX_ACTIVITY_STATE_HLT);

  BX_NEXT_TRACE(i);
}

/* 0F 08 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVD(bxInstruction_c *i)
{
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("%s: priveledge check failed, generate #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_INVD, 0);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_INVD)) Svm_Vmexit(SVM_VMEXIT_INVD);
  }
#endif

  invalidate_prefetch_q();

  BX_DEBUG(("INVD: Flush internal caches !"));
  BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_INVD);

  flushICaches();

  BX_NEXT_TRACE(i);
}

/* 0F 09 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::WBINVD(bxInstruction_c *i)
{
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("%s: priveledge check failed, generate #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.WBINVD_VMEXIT())
      VMexit(VMX_VMEXIT_WBINVD, 0);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_WBINVD)) Svm_Vmexit(SVM_VMEXIT_WBINVD);
  }
#endif

//invalidate_prefetch_q();

  BX_DEBUG(("WBINVD: WB-Invalidate internal caches !"));
  BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_WBINVD);

//flushICaches();

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLFLUSH(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr;

  // CLFLUSH performs all the segmentation and paging checks that a 1-byte read would perform,
  // except that it also allows references to execute-only segments.
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    laddr = get_laddr64(i->seg(), eaddr);
  else
#endif
    laddr = agen_read_execute32(i->seg(), (Bit32u)eaddr, 1);

  tickle_read_linear(i->seg(), laddr);

  BX_INSTR_CLFLUSH(BX_CPU_ID, laddr, BX_CPU_THIS_PTR address_xlation.paddress1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLZERO(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = RAX & ~BX_CONST64(CACHE_LINE_SIZE-1) & i->asize_mask();

  BxPackedZmmRegister zmmzero; // zmm is always made available even if EVEX is not compiled in
  zmmzero.clear();
  for (unsigned n=0; n<CACHE_LINE_SIZE; n += 64) {
    write_virtual_zmmword(i->seg(), eaddr+n, &zmmzero);
  }
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVDIR64B(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6

  BxPackedZmmRegister zmm; // zmm is always made available even if EVEX is not compiled in
  bx_address src_eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_zmmword(i->seg(), src_eaddr, &zmm);

#if BX_SUPPORT_X86_64
  bx_address dst_eaddr = BX_READ_64BIT_REG(i->dst());
#else
  bx_address dst_eaddr = BX_READ_32BIT_REG(i->dst());
#endif
  write_virtual_zmmword_aligned(BX_SEG_REG_ES, dst_eaddr & i->asize_mask(), &zmm);

#endif

  BX_NEXT_INSTR(i);
}

void BX_CPU_C::handleCpuModeChange(void)
{
  unsigned mode = BX_CPU_THIS_PTR cpu_mode;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_LMA()) {
    if (! BX_CPU_THIS_PTR cr0.get_PE()) {
      BX_PANIC(("change_cpu_mode: EFER.LMA is set when CR0.PE=0 !"));
    }
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l) {
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_LONG_64;
    }
    else {
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_LONG_COMPAT;
      // clear upper part of RIP/RSP when leaving 64-bit long mode
      BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RIP);
      BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSP);
    }

    // switching between compatibility and long64 mode also affect SS.BASE
    // which is always zero in long64 mode
    invalidate_stack_cache();
  }
  else
#endif
  {
    if (BX_CPU_THIS_PTR cr0.get_PE()) {
      if (BX_CPU_THIS_PTR get_VM()) {
        BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_V8086;
        CPL = 3;
      }
      else
        BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_PROTECTED;
    }
    else {
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_REAL;

      // CS segment in real mode always allows full access
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type = BX_DATA_READ_WRITE_ACCESSED;

      CPL = 0;
    }
  }

  updateFetchModeMask();

#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_AVX
  handleAvxModeChange(); /* protected mode reloaded */
#endif
#endif

  // re-initialize protection keys
#if BX_SUPPORT_PKEYS
  set_PKeys(BX_CPU_THIS_PTR pkru, BX_CPU_THIS_PTR pkrs);
#endif

  if (mode != BX_CPU_THIS_PTR cpu_mode) {
    BX_DEBUG(("%s activated", cpu_mode_string(BX_CPU_THIS_PTR cpu_mode)));
#if BX_DEBUGGER
    if (BX_CPU_THIS_PTR mode_break) {
      BX_CPU_THIS_PTR stop_reason = STOP_MODE_BREAK_POINT;
      bx_debug_break(); // trap into debugger
    }
#endif
  }
}

#if BX_CPU_LEVEL >= 4
void BX_CPU_C::handleAlignmentCheck(void)
{
  if (CPL == 3 && BX_CPU_THIS_PTR cr0.get_AM() && BX_CPU_THIS_PTR get_AC()) {
#if BX_SUPPORT_ALIGNMENT_CHECK == 0
    BX_PANIC(("WARNING: Alignment check (#AC exception) was not compiled in !"));
#else
    BX_CPU_THIS_PTR alignment_check_mask = 0xF;
#endif
  }
#if BX_SUPPORT_ALIGNMENT_CHECK
  else {
    BX_CPU_THIS_PTR alignment_check_mask = 0;
  }
#endif
}
#endif

void BX_CPU_C::handleFpuMmxModeChange(void)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    clear_fpu_mmx_ok();
  else
    set_fpu_mmx_ok();

  updateFetchModeMask(); /* FPU_MMX_OK changed */
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoFPU(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoMMX(bxInstruction_c *i)
{
  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

#if BX_CPU_LEVEL >= 6
void BX_CPU_C::handleSseModeChange(void)
{
  if(BX_CPU_THIS_PTR cr0.get_TS()) {
    clear_sse_ok();
  }
  else {
    if(BX_CPU_THIS_PTR cr0.get_EM() || !BX_CPU_THIS_PTR cr4.get_OSFXSR())
      clear_sse_ok();
    else
      set_sse_ok();
  }

  updateFetchModeMask(); /* SSE_OK changed */
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoSSE(bxInstruction_c *i)
{
  if(BX_CPU_THIS_PTR cr0.get_EM() || !BX_CPU_THIS_PTR cr4.get_OSFXSR())
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

#if BX_SUPPORT_AVX
void BX_CPU_C::handleAvxModeChange(void)
{
  if (BX_CPU_THIS_PTR cr0.get_TS()) {
    clear_avx_ok();
  }
  else {
    if (! protected_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE() ||
        (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK)) != 0) {
      clear_avx_ok();
    }
    else {
      set_avx_ok();

#if BX_SUPPORT_EVEX
      if ((~BX_CPU_THIS_PTR xcr0.val32 & BX_XCR0_OPMASK_MASK) != 0) {
        clear_opmask_ok();
        clear_evex_ok();
      }
      else {
        set_opmask_ok();

        if ((~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_ZMM_HI256_MASK | BX_XCR0_HI_ZMM_MASK)) != 0)
          clear_evex_ok();
        else
          set_evex_ok();
      }
#endif
    }
  }

#if BX_SUPPORT_AMX
  if (! long64_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE() ||
      (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_XTILECFG_MASK | BX_XCR0_XTILEDATA_MASK)) != 0)
    clear_amx_ok();
  else
    set_amx_ok();
#endif

  updateFetchModeMask(); /* AVX_OK changed */
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoAVX(bxInstruction_c *i)
{
  if (! protected_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK))
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}
#endif

#if BX_SUPPORT_EVEX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoOpMask(bxInstruction_c *i)
{
  if (! protected_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK | BX_XCR0_OPMASK_MASK))
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoEVEX(bxInstruction_c *i)
{
  if (! protected_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK | BX_XCR0_OPMASK_MASK | BX_XCR0_ZMM_HI256_MASK | BX_XCR0_HI_ZMM_MASK))
    exception(BX_UD_EXCEPTION, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}
#endif

#if BX_SUPPORT_AMX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BxNoAMX(bxInstruction_c *i)
{
  if (! long64_mode() || ! BX_CPU_THIS_PTR cr4.get_OSXSAVE())
    exception(BX_UD_EXCEPTION, 0);

  if (~BX_CPU_THIS_PTR xcr0.val32 & (BX_XCR0_XTILECFG_MASK | BX_XCR0_XTILEDATA_MASK))
    exception(BX_UD_EXCEPTION, 0);

  BX_ASSERT(0);

  BX_NEXT_TRACE(i); // keep compiler happy
}
#endif

#endif

void BX_CPU_C::handleCpuContextChange(void)
{
  TLB_flush();

  invalidate_prefetch_q();
  invalidate_stack_cache();

  handleInterruptMaskChange();

#if BX_CPU_LEVEL >= 4
  handleAlignmentCheck();
#endif

  handleCpuModeChange();

  handleFpuMmxModeChange();
#if BX_CPU_LEVEL >= 6
  handleSseModeChange();
#if BX_SUPPORT_AVX
  handleAvxModeChange();
#endif
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDPMC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  // in real mode CPL=0
  if (! BX_CPU_THIS_PTR cr4.get_PCE() && CPL != 0 /* && protected_mode() */) {
    BX_ERROR(("%s: not allowed to use instruction !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)  {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.RDPMC_VMEXIT()) {
      VMexit(VMX_VMEXIT_RDPMC, 0);
    }
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_RDPMC)) Svm_Vmexit(SVM_VMEXIT_RDPMC);
  }
#endif

  /* According to manual, Pentium 4 has 18 counters,
   * previous versions have two.  And the P4 also can do
   * short read-out (EDX always 0).  Otherwise it is
   * limited to 40 bits.
   */

  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_SSE2)) { // Pentium 4 processor (see cpuid.cc)
    if ((ECX & 0x7fffffff) >= 18)
      exception(BX_GP_EXCEPTION, 0);
  }
  else {
    if ((ECX & 0xffffffff) >= 2)
      exception(BX_GP_EXCEPTION, 0);
  }

  // Most counters are for hardware specific details, which
  // we anyhow do not emulate (like pipeline stalls etc)

  // Could be interesting to count number of memory reads,
  // writes.  Misaligned etc...  But to monitor bochs, this
  // is easier done from the host.

  RAX = 0;
  RDX = 0; // if P4 and ECX & 0x10000000, then always 0 (short read 32 bits)

  BX_ERROR(("RDPMC: Performance Counters Support not implemented yet"));
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 5

Bit64u BX_CPU_C::get_TSC(void)
{
  Bit64u tsc = bx_pc_system.time_ticks() + BX_CPU_THIS_PTR tsc_adjust;
  return tsc;
}

Bit64u BX_CPU_C::get_Virtual_TSC()
{
  Bit64u tsc = BX_CPU_THIS_PTR get_TSC();
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.TSC_OFFSET() && BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.TSC_SCALING()) {
      // RDTSC first computes the product of the value of the IA32_TIME_STAMP_COUNTER MSR and
      // the value of the TSC multiplier. It then shifts the value of the product right 48 bits and loads 
      // EAX:EDX with <the sum of that shifted value and the value of the TSC offset>.
      Bit128u product_128;
      long_mul(&product_128,tsc,BX_CPU_THIS_PTR vmcs.tsc_multiplier);
      tsc = (product_128.lo >> 48) | (product_128.hi << 16);   // tsc = (uint64) (long128(tsc_value * tsc_multiplier) >> 48);
    }
  }
#endif
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  tsc += BX_CPU_THIS_PTR tsc_offset;    // BX_CPU_THIS_PTR tsc_offset = 0 if not in VMX or SVM guest
#endif
  return tsc;
}

#if BX_SUPPORT_VMX
Bit64u BX_CPU_C::compute_physical_TSC_delay(Bit64u tsc_delay)
{
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.TSC_OFFSET() && BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.TSC_SCALING()) {
      // The virtual delay is multiplied by 2^48 (using a shift) to produce a 128-bit 
      // integer. That product is then divided by the TSC multiplier to produce a 64-bit integer.
      // The physical delay is that quotient.
      Bit128u product128, quotient;
      product128.hi = tsc_delay >> 16;
      product128.lo = tsc_delay << 48;
      long_div(&quotient, &tsc_delay /*just use it as temp to be destroyed*/, &product128, BX_CPU_THIS_PTR vmcs.tsc_multiplier);
      BX_ASSERT(quotient.hi == 0);
      tsc_delay = quotient.lo;                // tsc = Bit128(tsc_value << 48) / tsc_multiplier
    }
  }
  return tsc_delay;
}
#endif

void BX_CPU_C::set_TSC(Bit64u newval)
{
  // compute the correct setting of tsc_adjust so that a get_TSC()
  // will return newval
  BX_CPU_THIS_PTR tsc_adjust = newval - bx_pc_system.time_ticks();

  // verify
  BX_ASSERT(get_TSC() == newval);
}

#endif // BX_CPU_LEVEL >= 5

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  if (BX_CPU_THIS_PTR cr4.get_TSD() && CPL != 0) {
    BX_ERROR(("%s: not allowed to use instruction !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.RDTSC_VMEXIT()) {
      VMexit(VMX_VMEXIT_RDTSC, 0);
    }
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest)
    if (SVM_INTERCEPT(SVM_INTERCEPT0_RDTSC)) Svm_Vmexit(SVM_VMEXIT_RDTSC);
#endif

  Bit64u ticks = BX_CPU_THIS_PTR get_Virtual_TSC();

  RAX = GET32L(ticks);
  RDX = GET32H(ticks);

  BX_DEBUG(("RDTSC: ticks 0x%08x:%08x", EDX, EAX));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSCP(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64

#if BX_SUPPORT_VMX
  // RDTSCP will always #UD in legacy VMX mode, the #UD takes priority over any other exception the instruction may incur.
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDTSCP()) {
       BX_ERROR(("%s in VMX guest: not allowed to use instruction !", i->getIaOpcodeNameShort()));
       exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

  if (BX_CPU_THIS_PTR cr4.get_TSD() && CPL != 0) {
    BX_ERROR(("%s: not allowed to use instruction !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.RDTSC_VMEXIT()) {
      VMexit(VMX_VMEXIT_RDTSCP, 0);
    }
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest)
    if (SVM_INTERCEPT(SVM_INTERCEPT1_RDTSCP)) Svm_Vmexit(SVM_VMEXIT_RDTSCP);
#endif

  Bit64u ticks = BX_CPU_THIS_PTR get_Virtual_TSC();

  RAX = GET32L(ticks);
  RDX = GET32H(ticks);
  RCX = BX_CPU_THIS_PTR msr.tsc_aux;

#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDPID_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64

#if BX_SUPPORT_VMX
  // RDTSCP will always #UD in legacy VMX mode
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDTSCP()) {
       BX_ERROR(("%s in VMX guest: not allowed to use instruction !", i->getIaOpcodeNameShort()));
       exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

  BX_WRITE_32BIT_REGZ(i->dst(), BX_CPU_THIS_PTR msr.tsc_aux);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSENTER(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (real_mode()) {
    BX_ERROR(("%s: not recognized in real mode !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  if ((BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK) == 0) {
    BX_ERROR(("SYSENTER with zero sysenter_cs_msr !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

  BX_CPU_THIS_PTR clear_VM();       // do this just like the book says to do
  BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_RF();

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    if (!IsCanonical(BX_CPU_THIS_PTR msr.sysenter_eip_msr)) {
      BX_ERROR(("SYSENTER with non-canonical SYSENTER_EIP_MSR !"));
      exception(BX_GP_EXCEPTION, 0);
    }
    if (!IsCanonical(BX_CPU_THIS_PTR msr.sysenter_esp_msr)) {
      BX_ERROR(("SYSENTER with non-canonical SYSENTER_ESP_MSR !"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
#endif

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL))
    BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = SSP;
  if (ShadowStackEnabled(0)) SSP = 0;
  track_indirect(0);
#endif

  parse_selector(BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF; // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;          // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;          // available for use by system
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = !long_mode();
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            =  long_mode();
#endif

#if BX_SUPPORT_X86_64
  handleCpuModeChange(); // mode change could happen only when in long_mode()
#else
  updateFetchModeMask(/* CS reloaded */);
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

  parse_selector((BX_CPU_THIS_PTR msr.sysenter_cs_msr + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF; // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1;          // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1;          // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;          // available for use by system
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
#endif

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    RSP = BX_CPU_THIS_PTR msr.sysenter_esp_msr;
    RIP = BX_CPU_THIS_PTR msr.sysenter_eip_msr;
  }
  else
#endif
  {
    ESP = (Bit32u) BX_CPU_THIS_PTR msr.sysenter_esp_msr;
    EIP = (Bit32u) BX_CPU_THIS_PTR msr.sysenter_eip_msr;
  }

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSENTER,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSEXIT(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (real_mode() || CPL != 0) {
    BX_ERROR(("SYSEXIT from real mode or with CPL<>0 !"));
    exception(BX_GP_EXCEPTION, 0);
  }
  if ((BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK) == 0) {
    BX_ERROR(("SYSEXIT with zero sysenter_cs_msr !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_umonitor();
#endif

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    if (!IsCanonical(RDX)) {
       BX_ERROR(("SYSEXIT with non-canonical RDX (RIP) pointer !"));
       exception(BX_GP_EXCEPTION, 0);
    }
    if (!IsCanonical(RCX)) {
       BX_ERROR(("SYSEXIT with non-canonical RCX (RSP) pointer !"));
       exception(BX_GP_EXCEPTION, 0);
    }

    parse_selector(((BX_CPU_THIS_PTR msr.sysenter_cs_msr + 32) & BX_SELECTOR_RPL_MASK) | 3,
            &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;           // base address
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;           // 4k granularity
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;           // available for use by system
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 1;

    RSP = RCX;
    RIP = RDX;
  }
  else
#endif
  {
    parse_selector(((BX_CPU_THIS_PTR msr.sysenter_cs_msr + 16) & BX_SELECTOR_RPL_MASK) | 3,
            &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;           // base address
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;           // 4k granularity
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;           // available for use by system
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
#if BX_SUPPORT_X86_64
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0;
#endif

    ESP = ECX;
    EIP = EDX;
  }

#if BX_SUPPORT_X86_64
  handleCpuModeChange(); // mode change could happen only when in long_mode()
#else
  updateFetchModeMask(/* CS reloaded */);
#endif

  handleAlignmentCheck(/* CPL change */);

  parse_selector(((BX_CPU_THIS_PTR msr.sysenter_cs_msr + (i->os64L() ? 40:24)) & BX_SELECTOR_RPL_MASK) | 3,
            &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl      = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;           // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1;           // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1;           // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;           // available for use by system
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
#endif

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL))
    SSP = BX_CPU_THIS_PTR msr.ia32_pl_ssp[3];
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSEXIT,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSCALL(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  bx_address temp_RIP;

  BX_DEBUG(("Execute SYSCALL instruction"));

  if (!BX_CPU_THIS_PTR efer.get_SCE()) {
    exception(BX_UD_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_SUPPORT_CET
  unsigned old_CPL = CPL;
#endif

#if BX_SUPPORT_X86_64
  if (long_mode())
  {
    RCX = RIP;
    R11 = read_eflags() & ~(EFlagsRFMask);

    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      temp_RIP = BX_CPU_THIS_PTR msr.lstar;
    }
    else {
      temp_RIP = BX_CPU_THIS_PTR msr.cstar;
    }

    // set up CS segment, flat, 64-bit DPL=0
    parse_selector((BX_CPU_THIS_PTR msr.star >> 32) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 1; /* 64-bit code */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    handleCpuModeChange(); // mode change could only happen when in long_mode()

#if BX_SUPPORT_ALIGNMENT_CHECK
    BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

    // set up SS segment, flat, 64-bit DPL=0
    parse_selector(((BX_CPU_THIS_PTR msr.star >> 32) + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1; /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1; /* 32 bit stack */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0; /* available for use by system */

    writeEFlags(read_eflags() & ~(BX_CPU_THIS_PTR msr.fmask) & ~(EFlagsRFMask), EFlagsValidMask);
    RIP = temp_RIP;
  }
  else
#endif
  {
    // legacy mode

    ECX = EIP;
    temp_RIP = (Bit32u)(BX_CPU_THIS_PTR msr.star);

    // set up CS segment, flat, 32-bit DPL=0
    parse_selector((BX_CPU_THIS_PTR msr.star >> 32) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
#if BX_SUPPORT_X86_64
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
#endif
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    updateFetchModeMask(/* CS reloaded */);

#if BX_SUPPORT_ALIGNMENT_CHECK
    BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

    // set up SS segment, flat, 32-bit DPL=0
    parse_selector(((BX_CPU_THIS_PTR msr.star >> 32) + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1; /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1; /* 32 bit stack */
#if BX_SUPPORT_X86_64
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
#endif
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0; /* available for use by system */

    BX_CPU_THIS_PTR clear_VM();
    BX_CPU_THIS_PTR clear_IF();
    BX_CPU_THIS_PTR clear_RF();
    RIP = temp_RIP;
  }

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(old_CPL))
    BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = SSP;
  if (ShadowStackEnabled(0)) SSP = 0;
  track_indirect(0);
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSCALL,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSRET(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  bx_address temp_RIP;

  BX_DEBUG(("Execute SYSRET instruction"));

  if (!BX_CPU_THIS_PTR efer.get_SCE()) {
    exception(BX_UD_EXCEPTION, 0);
  }

  if(!protected_mode() || CPL != 0) {
    BX_ERROR(("%s: priveledge check failed, generate #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_umonitor();
#endif

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
  {
    if (i->os64L()) {
      if (!IsCanonical(RCX)) {
        BX_ERROR(("SYSRET: canonical failure for RCX (RIP)"));
        exception(BX_GP_EXCEPTION, 0);
      }

      // Return to 64-bit mode, set up CS segment, flat, 64-bit DPL=3
      parse_selector((((BX_CPU_THIS_PTR msr.star >> 48) + 16) & BX_SELECTOR_RPL_MASK) | 3,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 1; /* 64-bit code */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

      temp_RIP = RCX;
    }
    else {
      // Return to 32-bit compatibility mode, set up CS segment, flat, 32-bit DPL=3
      parse_selector((BX_CPU_THIS_PTR msr.star >> 48) | 3,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

      temp_RIP = ECX;
    }

    handleCpuModeChange(); // mode change could only happen when in long64 mode

    handleAlignmentCheck(/* CPL change */);

    // SS base, limit, attributes unchanged
    parse_selector((Bit16u)(((BX_CPU_THIS_PTR msr.star >> 48) + 8) | 3),
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    writeEFlags((Bit32u) R11, EFlagsValidMask);
  }
  else // (!64BIT_MODE)
#endif
  {
    // Return to 32-bit legacy mode, set up CS segment, flat, 32-bit DPL=3
    parse_selector((BX_CPU_THIS_PTR msr.star >> 48) | 3,
                     &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_CODE_EXEC_READ_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
#if BX_SUPPORT_X86_64
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
#endif
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    updateFetchModeMask(/* CS reloaded */);

    handleAlignmentCheck(/* CPL change */);

    // SS base, limit, attributes unchanged
    parse_selector((Bit16u)(((BX_CPU_THIS_PTR msr.star >> 48) + 8) | 3),
                     &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    BX_CPU_THIS_PTR assert_IF();
    temp_RIP = ECX;
  }

  handleCpuModeChange();

  RIP = temp_RIP;

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL))
    SSP = BX_CPU_THIS_PTR msr.ia32_pl_ssp[3];
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSRET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SWAPGS(bxInstruction_c *i)
{
  if(CPL != 0)
    exception(BX_GP_EXCEPTION, 0);

  Bit64u temp_GS_base = MSR_GSBASE;
  MSR_GSBASE = BX_CPU_THIS_PTR msr.kernelgsbase;
  BX_CPU_THIS_PTR msr.kernelgsbase = temp_GS_base;

  BX_NEXT_INSTR(i);
}

/* F3 0F AE /0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDFSBASE_Ed(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) MSR_FSBASE);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDFSBASE_Eq(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  BX_WRITE_64BIT_REG(i->dst(), MSR_FSBASE);
  BX_NEXT_INSTR(i);
}

/* F3 0F AE /1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDGSBASE_Ed(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) MSR_GSBASE);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDGSBASE_Eq(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  BX_WRITE_64BIT_REG(i->dst(), MSR_GSBASE);
  BX_NEXT_INSTR(i);
}

/* F3 0F AE /2 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRFSBASE_Ed(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  // 32-bit value is always canonical
  MSR_FSBASE = BX_READ_32BIT_REG(i->src());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRFSBASE_Eq(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  Bit64u fsbase = BX_READ_64BIT_REG(i->src());
  if (!IsCanonical(fsbase)) {
    BX_ERROR(("%s: canonical failure !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  MSR_FSBASE = fsbase;

  BX_NEXT_INSTR(i);
}

/* F3 0F AE /3 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRGSBASE_Ed(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  // 32-bit value is always canonical
  MSR_GSBASE = BX_READ_32BIT_REG(i->src());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRGSBASE_Eq(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FSGSBASE())
    exception(BX_UD_EXCEPTION, 0);

  Bit64u gsbase = BX_READ_64BIT_REG(i->src());
  if (!IsCanonical(gsbase)) {
    BX_ERROR(("%s: canonical failure !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  MSR_GSBASE = gsbase;

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_X86_64

#if BX_SUPPORT_PKEYS

void BX_CPU_C::set_PKeys(Bit32u pkru_val, Bit32u pkrs_val)
{
  BX_CPU_THIS_PTR pkru = pkru_val;
  BX_CPU_THIS_PTR pkrs = pkrs_val;

  for (unsigned i=0; i<16; i++) {
    BX_CPU_THIS_PTR rd_pkey[i] = BX_CPU_THIS_PTR wr_pkey[i] =
      TLB_SysReadOK | TLB_UserReadOK | TLB_SysWriteOK | TLB_UserWriteOK;

    if (long_mode()) {
      if (BX_CPU_THIS_PTR cr4.get_PKE()) {
        // accessDisable bit set
        if (pkru_val & (1<<(i*2))) {
          BX_CPU_THIS_PTR rd_pkey[i] &= ~(TLB_UserReadOK | TLB_UserWriteOK);
          BX_CPU_THIS_PTR wr_pkey[i] &= ~(TLB_UserReadOK | TLB_UserWriteOK);
        }

        // writeDisable bit set
        if (pkru_val & (1<<(i*2+1))) {
          BX_CPU_THIS_PTR wr_pkey[i] &= ~(TLB_UserWriteOK);
          if (BX_CPU_THIS_PTR cr0.get_WP())
            BX_CPU_THIS_PTR wr_pkey[i] &= ~(TLB_SysWriteOK);
        }
      }

      if (BX_CPU_THIS_PTR cr4.get_PKS()) {
        // accessDisable bit set
        if (pkrs_val & (1<<(i*2))) {
          BX_CPU_THIS_PTR rd_pkey[i] &= ~(TLB_SysReadOK | TLB_SysWriteOK);
          BX_CPU_THIS_PTR wr_pkey[i] &= ~(TLB_SysReadOK | TLB_SysWriteOK);
        }

        // writeDisable bit set
        if (pkrs_val & (1<<(i*2+1))) {
          if (BX_CPU_THIS_PTR cr0.get_WP())
            BX_CPU_THIS_PTR wr_pkey[i] &= ~(TLB_SysWriteOK);
        }
      }
    }

#if BX_SUPPORT_CET
    // replicate pkey access bits for shadow stack checks
    BX_CPU_THIS_PTR rd_pkey[i] |= BX_CPU_THIS_PTR rd_pkey[i]<<4;
    BX_CPU_THIS_PTR wr_pkey[i] |= BX_CPU_THIS_PTR wr_pkey[i]<<4;
#endif
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDPKRU(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_PKE())
    exception(BX_UD_EXCEPTION, 0);

  if (ECX != 0)
    exception(BX_GP_EXCEPTION, 0);

  RAX = BX_CPU_THIS_PTR pkru;
  RDX = 0;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRPKRU(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_PKE())
    exception(BX_UD_EXCEPTION, 0);

  if ((ECX|EDX) != 0)
    exception(BX_GP_EXCEPTION, 0);

  BX_CPU_THIS_PTR set_PKeys(EAX, BX_CPU_THIS_PTR pkrs);

  BX_NEXT_TRACE(i);
}

#endif // BX_SUPPORT_PKEYS
