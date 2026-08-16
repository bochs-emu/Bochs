/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2026 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_FPU

#include "softfloat-specialize.h"

void BX_CPU_C::FPU_stack_overflow(bxInstruction_c *i)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(floatx80_default_nan, 0);
  }
  FPU_exception(i, FPU_EX_Stack_Overflow);
}

void BX_CPU_C::FPU_stack_underflow(bxInstruction_c *i, int stnr, int pop_stack)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
    BX_WRITE_FPU_REG(floatx80_default_nan, stnr);
    if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  FPU_exception(i, FPU_EX_Stack_Underflow);
}

/* Returns unmasked exceptions if occurred */
unsigned BX_CPU_C::FPU_exception(bxInstruction_c *i, unsigned exception, bool is_store)
{
  /* Extract only the bits which we use to set the status word */
  exception &= (FPU_SW_Exceptions_Mask);

  Bit32u status = FPU_PARTIAL_STATUS;

  unsigned unmasked = exception & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask;
  // if IE or DZ exception happen nothing else will be reported
  if (exception & (FPU_EX_Invalid | FPU_EX_Zero_Div))
      unmasked &= (FPU_EX_Invalid | FPU_EX_Zero_Div);

  /* Set summary bits iff exception isn't masked */
  if (unmasked) {
    set_unmasked_fpu_exception();

    // when FOPCODE deprecation is set, FOPCODE is updated only when unmasked x87 exception occurs
    if (is_cpu_extension_supported(BX_ISA_FOPCODE_DEPRECATION))
      BX_CPU_THIS_PTR the_i387.foo = i->foo();

    // when FOPCODE deprecation is set, FCS/FDP are updated only when unmasked x87 exception occurs
    if (is_cpu_extension_supported(BX_ISA_FDP_DEPRECATION)) {
      if (! i->modC0()) {
        BX_CPU_THIS_PTR the_i387.fds = BX_CPU_THIS_PTR sregs[i->seg()].selector.value;
        BX_CPU_THIS_PTR the_i387.fdp = RMAddr(i);
      }
    }
  }

  if (exception & FPU_EX_Invalid) {
     // FPU_EX_Invalid cannot come with any other exception but x87 stack fault
     FPU_PARTIAL_STATUS |= exception;
     if (exception & FPU_SW_Stack_Fault) {
        if (! (exception & FPU_SW_C1)) {
           /* This bit distinguishes over- from underflow for a stack fault,
              and roundup from round-down for precision loss. */
           FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
        }
     }
     return unmasked;
  }

  if (exception & FPU_EX_Zero_Div) {
     FPU_PARTIAL_STATUS |= FPU_EX_Zero_Div;
     return unmasked;
  }

  if (exception & FPU_EX_Denormal) {
     FPU_PARTIAL_STATUS |= FPU_EX_Denormal;
     if (unmasked & FPU_EX_Denormal)
        return unmasked & FPU_EX_Denormal;
  }

  /* Set the corresponding exception bits */
  FPU_PARTIAL_STATUS |= exception;

  if (exception & FPU_EX_Precision)
  {
    if (! (exception & FPU_SW_C1)) {
      /* This bit distinguishes over- from underflow for a stack fault,
           and roundup from round-down for precision loss. */
      FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
    }
  }

  // If #P unmasked exception occurred the result still has to be
  // written to the destination.
  unmasked &= ~FPU_EX_Precision;

  if (unmasked & (FPU_EX_Underflow | FPU_EX_Overflow)) {
    // If unmasked over- or underflow occurs and dest is a memory location:
    //   - the TOS and destination operands remain unchanged
    //   - the inexact-result condition is not reported and C1 flag is cleared
    //   - no result is stored in the memory
    // If the destination is in the register stack, adjusted resulting value
    // is stored in the destination operand.
    if (! is_store) {
       unmasked &= ~(FPU_EX_Underflow | FPU_EX_Overflow);
    }
    else {
       FPU_PARTIAL_STATUS &= ~FPU_SW_C1; // clear C1 flag
       if (! (status & FPU_EX_Precision))
          FPU_PARTIAL_STATUS &= ~FPU_EX_Precision;
    }
  }

  return unmasked;
}

//
// When MS-DOS compatibility mode is enabled for the Intel486 or Pentium processors (NE bit is set to 0) and the
// IGNNE# input pin is de-asserted, the FERR# signal is generated as follows:
//
// 1. When an x87 FPU instruction causes an unmasked x87 FPU exception, the processor (in most cases) uses a
//    "deferred" method of reporting the error. This means that the processor does not respond immediately, but
//    rather freezes just before executing the next WAIT or x87 FPU instruction (except for "no-wait" instructions,
//    which the x87 FPU executes regardless of an error condition).
//
// 2. When the processor freezes, it also asserts the FERR# output.
//
// 3. The frozen processor waits for an external interrupt, which must be supplied by external hardware in response
//    to the FERR# assertion.
//
// 4. In MS-DOS compatibility systems, FERR# is fed to the IRQ13 input in the cascaded PIC. The PIC generates
//    interrupt 75H, which then branches to interrupt 2, as described earlier in this appendix for systems using the
//    Intel 286 and Intel 287 or Intel386 and Intel 387 processors.
//
// The deferred method of error reporting is used for all exceptions caused by the basic arithmetic instructions
// (including FADD, FSUB, FMUL, FDIV, FSQRT, FCOM and FUCOM), for precision exceptions caused by all types of x87
// FPU instructions, and for numeric underflow and overflow exceptions caused by all types of x87 FPU instructions
// except stores to memory.
//
// Some x87 FPU instructions with some x87 FPU exceptions use an "immediate" method of reporting errors. Here,
// the FERR# is asserted immediately, at the time that the exception occurs. The immediate method of error
// reporting is used for x87 FPU stack fault, invalid operation and denormal exceptions caused by all transcendental
// instructions, FSCALE, FXTRACT, FPREM and others, and all exceptions (except precision) when caused by x87 FPU
// store instructions. Like deferred error reporting, immediate error reporting will cause the processor to freeze just
// before executing the next WAIT or x87 FPU instruction if the error condition has not been cleared by that time.
// Note that in general, whether deferred or immediate error reporting is used for an x87 FPU exception depends both
// on which exception occurred and which instruction caused that exception. A complete specification of these cases,
// which applies to both the Pentium and the Intel486 processors, is given in Section 5.1.21 in the Pentium Processor
// Family Developer's Manual: Volume 1
//

#include "iodev/iodev.h"

void BX_CPU_C::FPU_check_pending_exceptions(void)
{
  if(BX_CPU_THIS_PTR the_i387.get_partial_status() & FPU_SW_Summary)
  {
     // NE=1 selects the native or internal mode, which generates #MF,
     // which is an extension introduced with 80486.
     // NE=0 selects the original (backward compatible) FPU error
     // handling, which generates an IRQ 13 via the PIC chip.
#if BX_CPU_LEVEL >= 4
     if (BX_CPU_THIS_PTR cr0.get_NE() != 0) {
         exception(BX_MF_EXCEPTION, 0);
     }
     else
#endif
     {
        // MSDOS compatibility mode
        if (! BX_CPU_THIS_PTR get_IGNNE()) {
          // with "deferred" method of reporting the error here is the time to assert FERR#
          DEV_extfpuirq_set_fpu_error(true);

          enter_sleep_state(BX_ACTIVITY_WAIT_FOR_X87);
          longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
        }
     }
  }
}

// this code uses "immediate" method of reporting errors for simplicity
void BX_CPU_C::set_unmasked_fpu_exception()
{
  /* set the B and ES bits in the status-word */
  FPU_PARTIAL_STATUS |= (FPU_SW_Summary | FPU_SW_Backward);

  BX_DEBUG(("Set FERR# on unmasked x87 exception"));

  // Here we set FERR# (Reset after FSW B/ES are cleared)
  DEV_extfpuirq_set_fpu_error(true);
}

void BX_CPU_C::clear_unmasked_fpu_exception()
{
  /* clear the B and ES bits in the status-word */
  FPU_PARTIAL_STATUS &= ~(FPU_SW_Summary | FPU_SW_Backward);

  BX_DEBUG(("Clear FERR#"));

  // Here we set FERR# (Reset after FSW B/ES are cleared)
  DEV_extfpuirq_set_fpu_error(false);
}

#endif
