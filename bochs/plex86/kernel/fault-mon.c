/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  fault-mon.c:  fault/int handlers for VM monitor - monitor space.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
#define IN_MONITOR_SPACE
#include "monitor.h"





/* The monitor stack frame.  When an exception or interrupt occurrs
 * during the execution of either guest or monitor code, the following
 * values are pushed.
 * 
 * ss
 * esp
 * eflags    Values pushed by the CPU and interrupt stub.  To simplify
 * cs        things, the stub pushes an error of zero for those
 * eip       events which don't naturally cause an error push, and
 * error     also pushes the vector of the exception/interrupt.
 * vector
 *
 * eax
 * ecx
 * edx       General registers, pushed with a PUSHA instruction,
 * ebx       by code below.
 * <esp>
 * ebp
 * esi
 * edi
 *
 * es
 * ds        Segment selectors, pushed by code below.
 * fs
 * gs
 */

void handleMonFault(guest_context_t *monContext);

  static inline
Bit32u readCR2(void)
{
  Bit32u cr2;
  asm volatile ("movl %%cr2, %0" : "=r" (cr2));
  return( cr2 );
}


asm (
".text                  \n\t"

/* __handle_fault:  This is called by all of the monitor's fault handler
 *     stubs.  A fault could have originated from execution of the guest
 *     (due to virtualization conditions or natural fault generation) or
 *     from the monitor (currently only due to bugs in the monitor).
 */
".globl __handle_fault  \n\t"
"__handle_fault:        \n\t"
"  pushal               \n\t" /* Save general registers */
"  pushl %es            \n\t" /* Save segment registers */
"  pushl %ds            \n\t"
"  pushl %fs            \n\t"
"  pushl %gs            \n\t"
"  movl  60(%esp), %eax     \n\t" /* CS pushed by CPU from fault */
"  andl  $3, %eax           \n\t" /* Check CS.RPL bits */
"  jz    __fault_from_mon   \n\t" /* RPL0 means from monitor */

/* We have determined that the fault was from guest code.  Prepare
 * to call the monitor C code to do most of the fault handling.
 */
"__fault_from_guest:    \n\t"
"  movl  %ss, %eax      \n\t" /* Copy SS into DS/ES */
"  movl  %eax, %ds      \n\t"
"  movl  %eax, %es      \n\t"
"  cld                  \n\t" /* gcc-compiled code needs this */
"  pushl %esp           \n\t" /* Push pointer to saved guest context for C call.*/
"  call handleGuestFault\n\t" /* Call the C monitor fault handler. */
"  addl $4, %esp        \n\t" /* Remove arg from stack. */
".globl __ret_to_guest  \n\t" /* Fault handled, work back to guest. */
"__ret_to_guest:        \n\t"
/* Return to the guest.  Restore registers from the monitor stack. */
"  popl  %gs            \n\t" /* Restore guest segments */
"  popl  %fs            \n\t"
"  popl  %ds            \n\t"
"  popl  %es            \n\t"
"  popal                \n\t" /* Restore guest general registers */
"  addl  $8, %esp       \n\t" /* Ignore vector and error dwords */
"  iret                 \n\t" /* Resume execution of guest */


"__fault_from_mon:               \n\t"
"  cld                           \n\t" /* gcc-compiled code needs this */
"  pushl %esp                    \n\t" /* Push pointer to context. */
"  call handleMonFault           \n\t" /* Call C code for real work */
"  addl $4, %esp                 \n\t"
/* Return to monitor.  Restore state from the monitor stack. */
"__ret_to_monitor:      \n\t"
"  popl  %gs            \n\t" /* Restore monitor segments */
"  popl  %fs            \n\t"
"  popl  %ds            \n\t"
"  popl  %es            \n\t"
"  popal                \n\t" /* Restore monitor general registers */
"  addl  $8, %esp       \n\t" /* ignore vector and error dwords */
"  iret                 \n\t" /* Resume execution of monitor */


/*
 * Hardware interrupt handler stub
 */
".globl __handle_int    \n\t" /* Return to monitor code */
"__handle_int:          \n\t"
"  pushal               \n\t" /* Save guest general registers */
"  pushl %es            \n\t" /* Save guest segment registers */
"  pushl %ds            \n\t"
"  pushl %fs            \n\t"
"  pushl %gs            \n\t"

"  movl  %ss, %eax      \n\t" /* Copy SS into DS/ES */
"  movl  %eax, %ds      \n\t"
"  movl  %eax, %es      \n\t"
"  cld                  \n\t" /* gcc-compiled code needs this */
"  pushl %esp           \n\t"
"  call handleInt       \n\t" /* monitor interrupt handler */
"  addl $4, %esp        \n\t"
"  cmpl $0x1, %eax      \n\t" /* Was interrupt generated from monitor code? */
"  je   __ret_to_monitor\n\t" /* Yes, so return to monitor code */
"  jmp  __ret_to_guest  \n\t" /* No, so return to guest code */
);



  unsigned
handleInt(guest_context_t *context)
/*
 * handleInt(): Redirect a hardware interrupt back to the host
 */
{
  nexus_t *nexus = (nexus_t *) (((Bit32u) context) & 0xfffff000);
  vm_t    *vm    = (vm_t *) nexus->vm;
  unsigned from_monitor;
  Bit64u t1;

  t1 = vm_rdtsc();

  if ( (context->cs & 0x0003) == 0x0003 ) {
    /* End of elapsed guest execution duration.  Add elapsed */
    /* cycles to time framework. */
    vm->system.cyclesElapsed += (t1 - vm->system.t0);

    from_monitor = 0; /* Event from guest code */
    }
  else {
    from_monitor = 1; /* Event from monitor code */
    }

  /* Interrupts are off naturally here. */
  vm->mon_request = MonReqRedirect;
  vm->redirect_vector = context->vector;
  vm->guest.__mon2host();
  return(from_monitor);
}


  void
handleGuestFault(guest_context_t *context)
/*  Handle a fault from the guest.  Called from the assembly stub
 *  __handle_fault.
 */
{
  nexus_t *nexus = (nexus_t *) (((Bit32u) context) & 0xfffff000);
  vm_t    *vm    = (vm_t *) nexus->vm;
  Bit32u  cr2    = readCR2();
  Bit64u  t1;

  /* End of elapsed guest execution duration */
  t1 = vm_rdtsc();
  vm->system.cyclesElapsed += (t1 - vm->system.t0);

#warning "Delete these checks"
#if ANAL_CHECKS
  if ( !context->eflags.fields.if_ )
    monpanic(vm, "handleGuestFault: guest IF=0.\n");
  if ( context->eflags.fields.vm )
    monpanic(vm, "handleGuestFault: eflags.VM=1.\n");
#endif

  STI();

  switch ( context->vector ) {
    case ExceptionDB: /* 1 */
      monpanic(vm, "handleGuestFault: #DB, method=%u not coded\n",
        vm->executeMethod);
#if 0
      if (vm->executeMethod == RunGuestNMethodBreakpoint) {
        /* Breakpoint generated because we requested it via TF=1 */
        }
      else {
        monpanic(vm, "handleGuestFault: #DB, method=%u not coded\n",
          vm->executeMethod);
        }
#endif
      break;

    case ExceptionBR: /* 5 */
monpanic(vm, "handleGuestFault: BR unfinished.\n");
      /* BOUND instruction fault; array index not in bounds */
monpanic(vm, "handleGuestFault: emulate_exception was here.\n");
      /*emulate_exception(vm, context->vector, 0);*/
      break;

    case ExceptionDE: /* 0 */
    case ExceptionBP: /* 3 */
    case ExceptionOF: /* 4 */
    case ExceptionNM: /* 7 */
    case ExceptionMF: /* 16 */
      toHostGuestFault(vm, context->vector);
      /*monpanic(vm, "handleGuestFault: DE/BP/OF/NM/MF unfinished.\n");*/
      /*monpanic(vm, "handleGuestFault: %u\n", context->vector);*/
      /* emulate_interrupt(vm, context->vector); */
      break;

    case ExceptionNP: /* 11 */
    case ExceptionSS: /* 12 */
    case ExceptionAC: /* 17 */
monpanic(vm, "handleGuestFault: NP/SS/AC unfinished.\n");
      /* use emulate_xyz() */
      /*interrupt(vm, context->vector, 0, 1, context->error); */
      monpanic(vm, "handleGuestFault: %u\n", context->vector);
      break;

    case ExceptionUD: /* 6 */
    case ExceptionGP: /* 13 */
      toHostGuestFault(vm, context->vector);
      break;

    case ExceptionPF: /* 14 */
      guestPageFault(vm, context, cr2);
      break;

    default:
      monpanic(vm, "handleGuestFault: Unhandled Fault: %u\n", context->vector);
      break;
    }
}

  void
handleMonFault(guest_context_t *monContext)
{
  nexus_t *nexus = (nexus_t *) (((Bit32u) monContext) & 0xfffff000);
  vm_t    *vm    = (vm_t *) nexus->vm;

  if (vm->inMonFault)
    monpanic(vm, "handleMonFault called recursively.\n");
  vm->inMonFault = 1;
monpanic(vm, "handleMonFault: vector=%u\n", monContext->vector);

  /* Fault occurred inside monitor code. */

  switch ( monContext->vector ) {
    case ExceptionPF:
    case ExceptionGP:
      {
      Bit32u cr2;
      /*unsigned us, rw;*/

      cr2 = readCR2();
      STI();

      if (monContext->error & 0x8) /* If RSVD bits used in PDir */
        monpanic(vm, "handleMF: RSVD\n");
      /*us = G_GetCPL(vm)==3;*/
      /*rw = (monContext->error >> 1) & 1;*/
      monpanic(vm, "handleMF: \n");
      break;
      }

    default:
      monpanic(vm, "hMF: vector=%u\n", monContext->vector);
      break;
    }

  /*vm->abort_code = 1;*/
  /*monpanic_nomess(vm);*/
  CLI();
  vm->inMonFault = 0;
}
