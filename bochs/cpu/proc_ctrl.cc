/////////////////////////////////////////////////////////////////////////
// $Id: proc_ctrl.cc,v 1.69 2003-05-02 12:22:48 vruppert Exp $
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


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif


#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#endif



  void
BX_CPU_C::UndefinedOpcode(bxInstruction_c *i)
{
  BX_DEBUG(("UndefinedOpcode: %02x causes exception 6", (unsigned) i->b1()));
  exception(BX_UD_EXCEPTION, 0, 0);
}

  void
BX_CPU_C::NOP(bxInstruction_c *i)
{
}

void BX_CPU_C::PREFETCH(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE != 0
  BX_INSTR_PREFETCH_HINT(BX_CPU_ID, i->nnn(), i->seg(), RMAddr(i));
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::HLT(bxInstruction_c *i)
{
  // hack to panic if HLT comes from BIOS
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value == 0xf000 )
    BX_PANIC(("HALT instruction encountered in the BIOS ROM"));

  if (CPL!=0) {
    BX_INFO(("HLT(): CPL!=0"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if ( ! BX_CPU_THIS_PTR get_IF () ) {
    BX_INFO(("WARNING: HLT instruction with IF=0!"));
    }

  // stops instruction execution and places the processor in a
  // HALT state.  An enabled interrupt, NMI, or reset will resume
  // execution.  If interrupt (including NMI) is used to resume
  // execution after HLT, the saved CS:eIP points to instruction
  // following HLT.

  // artificial trap bit, why use another variable.
  BX_CPU_THIS_PTR debug_trap |= 0x80000000; // artificial trap
  BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
  // Execution of this instruction completes.  The processor
  // will remain in a halt state until one of the above conditions
  // is met.

#if BX_USE_IDLE_HACK  
  bx_gui->sim_is_idle ();
#endif /* BX_USE_IDLE_HACK */  
}


  void
BX_CPU_C::CLTS(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("CLTS: not implemented for < 286"));
#else

  if (v8086_mode()) BX_PANIC(("clts: v8086 mode unsupported"));

  /* read errata file */
  // does CLTS also clear NT flag???

  // #GP(0) if CPL is not 0
  if (CPL!=0) {
    BX_INFO(("CLTS(): CPL!=0"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  BX_CPU_THIS_PTR cr0.ts = 0;
  BX_CPU_THIS_PTR cr0.val32 &= ~0x08;
#endif
}

  void
BX_CPU_C::INVD(bxInstruction_c *i)
{
  BX_INFO(("---------------"));
  BX_INFO(("- INVD called -"));
  BX_INFO(("---------------"));

#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      BX_INFO(("INVD: CPL!=0"));
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
  BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_INVD);
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::WBINVD(bxInstruction_c *i)
{
  BX_INFO(("WBINVD: (ignoring)"));

#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      BX_INFO(("WBINVD: CPL!=0"));
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
  BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_WBINVD);
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::MOV_DdRd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_DdRd: not supported on < 386"));
#else
  Bit32u val_32;

  if (v8086_mode()) BX_PANIC(("MOV_DdRd: v8086 mode unsupported"));

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_DdRd(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_DdRd: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  val_32 = BX_READ_32BIT_REG(i->rm());
  if (bx_dbg.dreg)
    BX_INFO(("MOV_DdRd: DR[%u]=%08xh unhandled",
      (unsigned) i->nnn(), (unsigned) val_32));

  switch (i->nnn()) {
    case 0: // DR0
      BX_CPU_THIS_PTR dr0 = val_32;
      break;
    case 1: // DR1
      BX_CPU_THIS_PTR dr1 = val_32;
      break;
    case 2: // DR2
      BX_CPU_THIS_PTR dr2 = val_32;
      break;
    case 3: // DR3
      BX_CPU_THIS_PTR dr3 = val_32;
      break;

    case 4: // DR4
    case 6: // DR6
      // DR4 aliased to DR6 by default.  With Debug Extensions on,
      // access to DR4 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 4) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_DdRd: access to DR4 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
#if BX_CPU_LEVEL <= 4
      // On 386/486 bit12 is settable
      BX_CPU_THIS_PTR dr6 = (BX_CPU_THIS_PTR dr6 & 0xffff0ff0) |
                            (val_32 & 0x0000f00f);
#else
      // On Pentium+, bit12 is always zero
      BX_CPU_THIS_PTR dr6 = (BX_CPU_THIS_PTR dr6 & 0xffff0ff0) |
                            (val_32 & 0x0000e00f);
#endif
      break;

    case 5: // DR5
    case 7: // DR7
      // Note: 486+ ignore GE and LE flags.  On the 386, exact
      // data breakpoint matching does not occur unless it is enabled
      // by setting the LE and/or GE flags.

      // DR5 aliased to DR7 by default.  With Debug Extensions on,
      // access to DR5 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 5) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions (CR4.DE) on
        BX_INFO(("MOV_DdRd: access to DR5 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      // Some sanity checks...
      if ( val_32 & 0x00002000 ) {
        BX_PANIC(("MOV_DdRd: GD bit not supported yet"));
        // Note: processor clears GD upon entering debug exception
        // handler, to allow access to the debug registers
        }
      if ( (((val_32>>16) & 3)==2) ||
           (((val_32>>20) & 3)==2) ||
           (((val_32>>24) & 3)==2) ||
           (((val_32>>28) & 3)==2) ) {
        // IO breakpoints (10b) are not yet supported.
        BX_PANIC(("MOV_DdRd: write of %08x contains IO breakpoint",
          val_32));
        }
      if ( (((val_32>>18) & 3)==2) ||
           (((val_32>>22) & 3)==2) ||
           (((val_32>>26) & 3)==2) ||
           (((val_32>>30) & 3)==2) ) {
        // LEN0..3 contains undefined length specifier (10b)
        BX_PANIC(("MOV_DdRd: write of %08x contains undefined LENx",
          val_32));
        }
      if ( ((((val_32>>16) & 3)==0) && (((val_32>>18) & 3)!=0)) ||
           ((((val_32>>20) & 3)==0) && (((val_32>>22) & 3)!=0)) ||
           ((((val_32>>24) & 3)==0) && (((val_32>>26) & 3)!=0)) ||
           ((((val_32>>28) & 3)==0) && (((val_32>>30) & 3)!=0)) ) {
        // Instruction breakpoint with LENx not 00b (1-byte length)
        BX_PANIC(("MOV_DdRd: write of %08x, R/W=00b LEN!=00b",
          val_32));
        }
#if BX_CPU_LEVEL <= 4
      // 386/486: you can play with all the bits except b10 is always 1
      BX_CPU_THIS_PTR dr7 = val_32 | 0x00000400;
#else
      // Pentium+: bits15,14,12 are hardwired to 0, rest are settable.
      // Even bits 11,10 are changeable though reserved.
      BX_CPU_THIS_PTR dr7 = (val_32 & 0xffff2fff) | 0x00000400;
#endif
      break;
    default:
      BX_PANIC(("MOV_DdRd: control register index out of range"));
      break;
    }
#endif
}

  void
BX_CPU_C::MOV_RdDd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RdDd: not supported on < 386"));
#else
  Bit32u val_32;

  if (v8086_mode()) {
    BX_INFO(("MOV_RdDd: v8086 mode causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  if (!i->modC0()) {
    BX_PANIC(("MOV_RdDd(): rm field not a register!"));
    UndefinedOpcode(i);
    }

  if (protected_mode() && (CPL!=0)) {
    BX_INFO(("MOV_RdDd: CPL!=0 causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if (bx_dbg.dreg)
    BX_INFO(("MOV_RdDd: DR%u not implemented yet", i->nnn()));

  switch (i->nnn()) {
    case 0: // DR0
      val_32 = BX_CPU_THIS_PTR dr0;
      break;
    case 1: // DR1
      val_32 = BX_CPU_THIS_PTR dr1;
      break;
    case 2: // DR2
      val_32 = BX_CPU_THIS_PTR dr2;
      break;
    case 3: // DR3
      val_32 = BX_CPU_THIS_PTR dr3;
      break;

    case 4: // DR4
    case 6: // DR6
      // DR4 aliased to DR6 by default.  With Debug Extensions on,
      // access to DR4 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 4) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_RdDd: access to DR4 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      val_32 = BX_CPU_THIS_PTR dr6;
      break;

    case 5: // DR5
    case 7: // DR7
      // DR5 aliased to DR7 by default.  With Debug Extensions on,
      // access to DR5 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 5) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_RdDd: access to DR5 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      val_32 = BX_CPU_THIS_PTR dr7;
      break;

    default:
      BX_PANIC(("MOV_RdDd: control register index out of range"));
      val_32 = 0;
    }
  BX_WRITE_32BIT_REGZ(i->rm(), val_32);
#endif
}

#if BX_SUPPORT_X86_64
  void
BX_CPU_C::MOV_DqRq(bxInstruction_c *i)
{
  Bit64u val_64;

  if (v8086_mode()) BX_PANIC(("MOV_DqRq: v8086 mode unsupported"));

  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_DqRq(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_DqRq: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  val_64 = BX_READ_64BIT_REG(i->rm());
  if (bx_dbg.dreg)
    BX_INFO(("MOV_DqRq: DR[%u]=%08xh unhandled",
      (unsigned) i->nnn(), (unsigned) val_64));

  switch (i->nnn()) {
    case 0: // DR0
      BX_CPU_THIS_PTR dr0 = val_64;
      break;
    case 1: // DR1
      BX_CPU_THIS_PTR dr1 = val_64;
      break;
    case 2: // DR2
      BX_CPU_THIS_PTR dr2 = val_64;
      break;
    case 3: // DR3
      BX_CPU_THIS_PTR dr3 = val_64;
      break;

    case 4: // DR4
    case 6: // DR6
      // DR4 aliased to DR6 by default.  With Debug Extensions on,
      // access to DR4 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 4) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_DqRq: access to DR4 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
#if BX_CPU_LEVEL <= 4
      // On 386/486 bit12 is settable
      BX_CPU_THIS_PTR dr6 = (BX_CPU_THIS_PTR dr6 & 0xffff0ff0) |
                            (val_64 & 0x0000f00f);
#else
      // On Pentium+, bit12 is always zero
      BX_CPU_THIS_PTR dr6 = (BX_CPU_THIS_PTR dr6 & 0xffff0ff0) |
                            (val_64 & 0x0000e00f);
#endif
      break;

    case 5: // DR5
    case 7: // DR7
      // Note: 486+ ignore GE and LE flags.  On the 386, exact
      // data breakpoint matching does not occur unless it is enabled
      // by setting the LE and/or GE flags.

      // DR5 aliased to DR7 by default.  With Debug Extensions on,
      // access to DR5 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 5) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions (CR4.DE) on
        BX_INFO(("MOV_DqRq: access to DR5 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      // Some sanity checks...
      if ( val_64 & 0x00002000 ) {
        BX_PANIC(("MOV_DqRq: GD bit not supported yet"));
        // Note: processor clears GD upon entering debug exception
        // handler, to allow access to the debug registers
        }
      if ( (((val_64>>16) & 3)==2) ||
           (((val_64>>20) & 3)==2) ||
           (((val_64>>24) & 3)==2) ||
           (((val_64>>28) & 3)==2) ) {
        // IO breakpoints (10b) are not yet supported.
        BX_PANIC(("MOV_DqRq: write of %08x contains IO breakpoint",
          val_64));
        }
      if ( (((val_64>>18) & 3)==2) ||
           (((val_64>>22) & 3)==2) ||
           (((val_64>>26) & 3)==2) ||
           (((val_64>>30) & 3)==2) ) {
        // LEN0..3 contains undefined length specifier (10b)
        BX_PANIC(("MOV_DqRq: write of %08x contains undefined LENx",
          val_64));
        }
      if ( ((((val_64>>16) & 3)==0) && (((val_64>>18) & 3)!=0)) ||
           ((((val_64>>20) & 3)==0) && (((val_64>>22) & 3)!=0)) ||
           ((((val_64>>24) & 3)==0) && (((val_64>>26) & 3)!=0)) ||
           ((((val_64>>28) & 3)==0) && (((val_64>>30) & 3)!=0)) ) {
        // Instruction breakpoint with LENx not 00b (1-byte length)
        BX_PANIC(("MOV_DqRq: write of %08x, R/W=00b LEN!=00b",
          val_64));
        }
#if BX_CPU_LEVEL <= 4
      // 386/486: you can play with all the bits except b10 is always 1
      BX_CPU_THIS_PTR dr7 = val_64 | 0x00000400;
#else
      // Pentium+: bits15,14,12 are hardwired to 0, rest are settable.
      // Even bits 11,10 are changeable though reserved.
      BX_CPU_THIS_PTR dr7 = (val_64 & 0xffff2fff) | 0x00000400;
#endif
      break;
    default:
      BX_PANIC(("MOV_DqRq: control register index out of range"));
      break;
    }
}
#endif  // #if BX_SUPPORT_X86_64

#if BX_SUPPORT_X86_64
  void
BX_CPU_C::MOV_RqDq(bxInstruction_c *i)
{
  Bit64u val_64;

  if (v8086_mode()) {
    BX_INFO(("MOV_RqDq: v8086 mode causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  if (!i->modC0()) {
    BX_PANIC(("MOV_RqDq(): rm field not a register!"));
    UndefinedOpcode(i);
    }

  if (protected_mode() && (CPL!=0)) {
    BX_INFO(("MOV_RqDq: CPL!=0 causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if (bx_dbg.dreg)
    BX_INFO(("MOV_RqDq: DR%u not implemented yet", i->nnn()));

  switch (i->nnn()) {
    case 0: // DR0
      val_64 = BX_CPU_THIS_PTR dr0;
      break;
    case 1: // DR1
      val_64 = BX_CPU_THIS_PTR dr1;
      break;
    case 2: // DR2
      val_64 = BX_CPU_THIS_PTR dr2;
      break;
    case 3: // DR3
      val_64 = BX_CPU_THIS_PTR dr3;
      break;

    case 4: // DR4
    case 6: // DR6
      // DR4 aliased to DR6 by default.  With Debug Extensions on,
      // access to DR4 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 4) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_RqDq: access to DR4 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      val_64 = BX_CPU_THIS_PTR dr6;
      break;

    case 5: // DR5
    case 7: // DR7
      // DR5 aliased to DR7 by default.  With Debug Extensions on,
      // access to DR5 causes #UD
#if BX_CPU_LEVEL >= 4
      if ( (i->nnn() == 5) && (BX_CPU_THIS_PTR cr4.get_DE()) ) {
        // Debug extensions on
        BX_INFO(("MOV_RqDq: access to DR5 causes #UD"));
        UndefinedOpcode(i);
        }
#endif
      val_64 = BX_CPU_THIS_PTR dr7;
      break;

    default:
      BX_PANIC(("MOV_RqDq: control register index out of range"));
      val_64 = 0;
    }
  BX_WRITE_64BIT_REG(i->rm(), val_64);
}
#endif  // #if BX_SUPPORT_X86_64



  void
BX_CPU_C::LMSW_Ew(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("LMSW_Ew(): not supported on 8086!"));
#else
  Bit16u msw;
  Bit32u cr0;

  invalidate_prefetch_q();

  if (v8086_mode()) BX_PANIC(("proc_ctrl: LMSW in v8086 mode unsupported"));

  if ( protected_mode() ) {
    if ( CPL != 0 ) {
      BX_INFO(("LMSW: CPL != 0, CPL=%u", (unsigned) CPL));
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  if (i->modC0()) {
    msw = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &msw);
    }

  // LMSW does not affect PG,CD,NW,AM,WP,NE,ET bits, and cannot clear PE

  // LMSW cannot clear PE
  if ( ((msw & 0x0001)==0) && BX_CPU_THIS_PTR cr0.pe ) {
    msw |= 0x0001; // adjust PE bit to current value of 1
    }

  msw &= 0x000f; // LMSW only affects last 4 flags
  cr0 = (BX_CPU_THIS_PTR cr0.val32 & 0xfffffff0) | msw;
  SetCR0(cr0);
#endif /* BX_CPU_LEVEL < 2 */
}

  void
BX_CPU_C::SMSW_Ew(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("SMSW_Ew: not supported yet!"));
#else
  Bit16u msw;


#if BX_CPU_LEVEL == 2
  msw = 0xfff0; /* 80286 init value */
  msw |= (BX_CPU_THIS_PTR cr0.ts << 3) |
         (BX_CPU_THIS_PTR cr0.em << 2) |
         (BX_CPU_THIS_PTR cr0.mp << 1) |
         BX_CPU_THIS_PTR cr0.pe;
#else /* 386+ */
  /* reserved bits 0 ??? */
  /* should NE bit be included here ??? */
  // should ET bit be included here (AW)
  msw =  (BX_CPU_THIS_PTR cr0.ts << 3) |
         (BX_CPU_THIS_PTR cr0.em << 2) |
         (BX_CPU_THIS_PTR cr0.mp << 1) |
         BX_CPU_THIS_PTR cr0.pe;
#endif


  if (i->modC0()) {
    if (i->os32L()) {
      BX_WRITE_32BIT_REGZ(i->rm(), msw);  // zeros out high 16bits
      }
    else {
      BX_WRITE_16BIT_REG(i->rm(), msw);
      }
    }
  else {
    write_virtual_word(i->seg(), RMAddr(i), &msw);
    }

#endif
}


  void
BX_CPU_C::MOV_CdRd(bxInstruction_c *i)
{
  // mov general register data to control register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_CdRd: not supported on < 386"));
#else
  Bit32u val_32;


/*  if (v8086_mode()) BX_PANIC(("proc_ctrl: MOV_CdRd in v8086 mode unsupported"));*/

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_CdRd(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if ((protected_mode() || v8086_mode()) && CPL!=0) {
    BX_PANIC(("MOV_CdRd: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_32 = BX_READ_32BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      // BX_INFO(("MOV_CdRd:CR0: R32 = %08x @CS:EIP %04x:%04x ",
      //   (unsigned) val_32,
      //   (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
      //   (unsigned) EIP));
      SetCR0(val_32);
      break;

    case 1: /* CR1 */
      BX_PANIC(("MOV_CdRd: CR1 not implemented yet"));
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_CdRd: CR2 not implemented yet"));
      BX_DEBUG(("MOV_CdRd: CR2 = reg"));
      BX_CPU_THIS_PTR cr2 = val_32;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        BX_INFO(("MOV_CdRd:CR3 = %08x", (unsigned) val_32));
      // Reserved bits take on value of MOV instruction
      CR3_change(val_32);
      BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_MOV_CR3, val_32);
      // Reload of CR3 always serializes.
      // invalidate_prefetch_q(); // Already done.
      break;
    case 4: // CR4
      {
#if BX_CPU_LEVEL == 3
      BX_PANIC(("MOV_CdRd: write to CR4 of 0x%08x on 386",
        val_32));
      UndefinedOpcode(i);
#else
      //  Protected mode: #GP(0) if attempt to write a 1 to
      //  any reserved bit of CR4

      SetCR4(val_32);
#endif
      }
      break;
    default:
      BX_PANIC(("MOV_CdRd: control register index out of range"));
      break;
    }
#endif
}

  void
BX_CPU_C::MOV_RdCd(bxInstruction_c *i)
{
  // mov control register data to register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RdCd: not supported on < 386"));
#else
  Bit32u val_32;

/*  if (v8086_mode()) BX_PANIC(("proc_ctrl: MOV_RdCd in v8086 mode unsupported"));*/

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_RdCd(): rm field not a register!"));
    }

  if ((protected_mode() || v8086_mode()) && CPL!=0) {
    BX_INFO(("MOV_RdCd: CPL!=0"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      val_32 = BX_CPU_THIS_PTR cr0.val32;
#if 0
      BX_INFO(("MOV_RdCd:CR0: R32 = %08x @CS:EIP %04x:%04x",
        (unsigned) val_32,
        (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
        (unsigned) EIP));
#endif
      break;
    case 1: /* CR1 */
      BX_PANIC(("MOV_RdCd: CR1 not implemented yet"));
      val_32 = 0;
      break;
    case 2: /* CR2 */
      if (bx_dbg.creg)
        BX_INFO(("MOV_RdCd: CR2"));
      val_32 = BX_CPU_THIS_PTR cr2;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        BX_INFO(("MOV_RdCd: reading CR3"));
      val_32 = BX_CPU_THIS_PTR cr3;
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      val_32 = 0;
      BX_INFO(("MOV_RdCd: read of CR4 causes #UD"));
      UndefinedOpcode(i);
#else
      BX_INFO(("MOV_RdCd: read of CR4"));
      val_32 = BX_CPU_THIS_PTR cr4.getRegister();
#endif
      break;
    default:
      BX_PANIC(("MOV_RdCd: control register index out of range"));
      val_32 = 0;
    }
  BX_WRITE_32BIT_REGZ(i->rm(), val_32);
#endif
}


#if BX_SUPPORT_X86_64
  void
BX_CPU_C::MOV_CqRq(bxInstruction_c *i)
{
  // mov general register data to control register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_CqRq: not supported on < 386"));
#else
  Bit64u val_64;


  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_CqRq(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_CqRq: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_64 = BX_READ_64BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      // BX_INFO(("MOV_CqRq:CR0: R64 = %08x @CS:EIP %04x:%04x ",
      //   (unsigned) val_64,
      //   (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
      //   (unsigned) EIP));
      SetCR0(val_64);
      break;

    case 1: /* CR1 */
      BX_PANIC(("MOV_CqRq: CR1 not implemented yet"));
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_CqRq: CR2 not implemented yet"));
      BX_DEBUG(("MOV_CqRq: CR2 = reg"));
      BX_CPU_THIS_PTR cr2 = val_64;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        BX_INFO(("MOV_CqRq:CR3 = %08x", (unsigned) val_64));
      // Reserved bits take on value of MOV instruction
      CR3_change(val_64);
      BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_MOV_CR3, val_64);
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      BX_PANIC(("MOV_CqRq: write to CR4 of 0x%08x on 386",
        val_64));
      UndefinedOpcode(i);
#else
      //  Protected mode: #GP(0) if attempt to write a 1 to
      //  any reserved bit of CR4

      BX_INFO(("MOV_CqRq: ignoring write to CR4 of 0x%08x",
        val_64));
      if (val_64) {
        BX_INFO(("MOV_CqRq: (CR4) write of 0x%08x not supported!",
          val_64));
        }
      // Only allow writes of 0 to CR4 for now.
      // Writes to bits in CR4 should not be 1s as CPUID
      // returns not-supported for all of these features.
      SetCR4(val_64);
#endif
      break;
    default:
      BX_PANIC(("MOV_CqRq: control register index out of range"));
      break;
    }
#endif
}
#endif  // #if BX_SUPPORT_X86_64


#if BX_SUPPORT_X86_64
  void
BX_CPU_C::MOV_RqCq(bxInstruction_c *i)
{
  // mov control register data to register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RqCq: not supported on < 386"));
#else
  Bit64u val_64;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (!i->modC0()) {
    BX_PANIC(("MOV_RqC(): rm field not a register!"));
    }

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_RqCq: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      val_64 = BX_CPU_THIS_PTR cr0.val32;
#if 0
      BX_INFO(("MOV_RqCq:CR0: R64 = %08x @CS:EIP %04x:%04x",
        (unsigned) val_64,
        (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
        (unsigned) EIP));
#endif
      break;
    case 1: /* CR1 */
      BX_PANIC(("MOV_RqCq: CR1 not implemented yet"));
      val_64 = 0;
      break;
    case 2: /* CR2 */
      if (bx_dbg.creg)
        BX_INFO(("MOV_RqCq: CR2"));
      val_64 = BX_CPU_THIS_PTR cr2;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        BX_INFO(("MOV_RqCq: reading CR3"));
      val_64 = BX_CPU_THIS_PTR cr3;
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      val_64 = 0;
      BX_INFO(("MOV_RqCq: read of CR4 causes #UD"));
      UndefinedOpcode(i);
#else
      BX_INFO(("MOV_RqCq: read of CR4"));
      val_64 = BX_CPU_THIS_PTR cr4.getRegister();
#endif
      break;
    default:
      BX_PANIC(("MOV_RqCq: control register index out of range"));
      val_64 = 0;
    }
  BX_WRITE_64BIT_REG(i->rm(), val_64);
#endif
}
#endif  // #if BX_SUPPORT_X86_64

  void
BX_CPU_C::MOV_TdRd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_TdRd:"));
#elif BX_CPU_LEVEL <= 4
  BX_PANIC(("MOV_TdRd:"));
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  BX_INFO(("MOV_TdRd: causes #UD"));
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::MOV_RdTd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RdTd:"));
#elif BX_CPU_LEVEL <= 4
  BX_PANIC(("MOV_RdTd:"));
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  BX_INFO(("MOV_RdTd: causes #UD"));
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::LOADALL(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("undocumented LOADALL instruction not supported on 8086"));
#else
  Bit16u msw, tr, flags, ip, ldtr;
  Bit16u ds_raw, ss_raw, cs_raw, es_raw;
  Bit16u di, si, bp, sp, bx, dx, cx, ax;
  Bit16u base_15_0, limit;
  Bit8u  base_23_16, access;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: LOADALL in v8086 mode unsupported"));

#if BX_CPU_LEVEL > 2
  BX_PANIC(("loadall: not implemented for 386"));
  /* ??? need to set G and other bits, and compute .limit_scaled also */
  /* for all segments CS,DS,SS,... */
#endif

  if (BX_CPU_THIS_PTR cr0.pe) {
    BX_PANIC((
      "LOADALL not yet supported for protected mode"));
    }

BX_PANIC(("LOADALL: handle CR0.val32"));
  /* MSW */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x806, 2, &msw);
  BX_CPU_THIS_PTR cr0.pe = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.mp = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.em = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.ts = (msw & 0x01);

  //BX_INFO(("LOADALL: pe=%u, mp=%u, em=%u, ts=%u",
  //  (unsigned) BX_CPU_THIS_PTR cr0.pe, (unsigned) BX_CPU_THIS_PTR cr0.mp,
  //  (unsigned) BX_CPU_THIS_PTR cr0.em, (unsigned) BX_CPU_THIS_PTR cr0.ts));

  if (BX_CPU_THIS_PTR cr0.pe || BX_CPU_THIS_PTR cr0.mp || BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts)
    BX_PANIC(("LOADALL set PE, MP, EM or TS bits in MSW!"));

  /* TR */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x816, 2, &tr);
  BX_CPU_THIS_PTR tr.selector.value = tr;
  BX_CPU_THIS_PTR tr.selector.rpl   = (tr & 0x03);  tr >>= 2;
  BX_CPU_THIS_PTR tr.selector.ti    = (tr & 0x01);  tr >>= 1;
  BX_CPU_THIS_PTR tr.selector.index = tr;
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x860, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x862, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x863, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x864, 2, &limit);


  BX_CPU_THIS_PTR tr.cache.valid =
  BX_CPU_THIS_PTR tr.cache.p           = (access & 0x80) >> 7;
  BX_CPU_THIS_PTR tr.cache.dpl         = (access & 0x60) >> 5;
  BX_CPU_THIS_PTR tr.cache.segment     = (access & 0x10) >> 4;
  // don't allow busy bit in tr.cache.type, so bit 2 is masked away too.
  BX_CPU_THIS_PTR tr.cache.type        = (access & 0x0d);
  BX_CPU_THIS_PTR tr.cache.u.tss286.base  = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR tr.cache.u.tss286.limit = limit;

  if ( (BX_CPU_THIS_PTR tr.selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
    }
  if ( BX_CPU_THIS_PTR tr.cache.valid == 0 ) {
    }
  if ( BX_CPU_THIS_PTR tr.cache.u.tss286.limit < 43 ) {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
    }
  if ( BX_CPU_THIS_PTR tr.cache.type != 1 ) {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
    }
  if ( BX_CPU_THIS_PTR tr.cache.segment ) {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR tr.cache.valid==0) {
    BX_CPU_THIS_PTR tr.cache.u.tss286.base   = 0;
    BX_CPU_THIS_PTR tr.cache.u.tss286.limit  = 0;
    BX_CPU_THIS_PTR tr.cache.p            = 0;
    BX_CPU_THIS_PTR tr.selector.value     = 0;
    BX_CPU_THIS_PTR tr.selector.index     = 0;
    BX_CPU_THIS_PTR tr.selector.ti        = 0;
    BX_CPU_THIS_PTR tr.selector.rpl       = 0;
    }


  /* FLAGS */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x818, 2, &flags);
  write_flags(flags, 1, 1);

  /* IP */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x81a, 2, &ip);
  IP = ip;

  /* LDTR */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x81c, 2, &ldtr);
  BX_CPU_THIS_PTR ldtr.selector.value = ldtr;
  BX_CPU_THIS_PTR ldtr.selector.rpl   = (ldtr & 0x03);  ldtr >>= 2;
  BX_CPU_THIS_PTR ldtr.selector.ti    = (ldtr & 0x01);  ldtr >>= 1;
  BX_CPU_THIS_PTR ldtr.selector.index = ldtr;
  if ( (BX_CPU_THIS_PTR ldtr.selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR ldtr.cache.valid   = 0;
    BX_CPU_THIS_PTR ldtr.cache.p       = 0;
    BX_CPU_THIS_PTR ldtr.cache.segment = 0;
    BX_CPU_THIS_PTR ldtr.cache.type    = 0;
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.base = 0;
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit = 0;
    BX_CPU_THIS_PTR ldtr.selector.value = 0;
    BX_CPU_THIS_PTR ldtr.selector.index = 0;
    BX_CPU_THIS_PTR ldtr.selector.ti    = 0;
    }
  else {
    BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x854, 2, &base_15_0);
    BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x856, 1, &base_23_16);
    BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x857, 1, &access);
    BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x858, 2, &limit);
    BX_CPU_THIS_PTR ldtr.cache.valid      =
    BX_CPU_THIS_PTR ldtr.cache.p          = access >> 7;
    BX_CPU_THIS_PTR ldtr.cache.dpl        = (access >> 5) & 0x03;
    BX_CPU_THIS_PTR ldtr.cache.segment    = (access >> 4) & 0x01;
    BX_CPU_THIS_PTR ldtr.cache.type       = (access & 0x0f);
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.base = (base_23_16 << 16) | base_15_0;
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit = limit;

    if (access == 0) {
      BX_PANIC(("loadall: LDTR case access byte=0."));
      }
    if ( BX_CPU_THIS_PTR ldtr.cache.valid==0 ) {
      BX_PANIC(("loadall: ldtr.valid=0"));
      }
    if (BX_CPU_THIS_PTR ldtr.cache.segment) { /* not a system segment */
      BX_INFO(("         AR byte = %02x", (unsigned) access));
      BX_PANIC(("loadall: LDTR descriptor cache loaded with non system segment"));
      }
    if ( BX_CPU_THIS_PTR ldtr.cache.type != 2 ) {
      BX_PANIC(("loadall: LDTR.type(%u) != 2", (unsigned) (access & 0x0f)));
      }
    }

  /* DS */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x81e, 2, &ds_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = ds_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl   = (ds_raw & 0x03);  ds_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti    = (ds_raw & 0x01);  ds_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index = ds_raw;
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x848, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x84a, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x84b, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x84c, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit = limit;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.a          = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.executable = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment    = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl        = (access & 0x03); access >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid      =
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p          = (access & 0x01);

  if ( (BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid==0  ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment==0) {
    BX_PANIC(("loadall: DS invalid"));
    }

  /* SS */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x820, 2, &ss_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = ss_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = (ss_raw & 0x03); ss_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (ss_raw & 0x01); ss_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = ss_raw;
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x842, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x844, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x845, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x846, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit = limit;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a          = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment    = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl        = (access & 0x03); access >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p          = (access & 0x01);

  if ( (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid==0  ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment==0) {
    BX_PANIC(("loadall: SS invalid"));
    }


  /* CS */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x822, 2, &cs_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = cs_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = (cs_raw & 0x03); cs_raw >>= 2;

  //BX_INFO(("LOADALL: setting cs.selector.rpl to %u",
  //  (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl));

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (cs_raw & 0x01); cs_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = cs_raw;
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x83c, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x83e, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x83f, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x840, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit = limit;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a          = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment    = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl        = (access & 0x03); access >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p          = (access & 0x01);

  if ( (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid==0  ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment==0) {
    BX_PANIC(("loadall: CS invalid"));
    }

  /* ES */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x824, 2, &es_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = es_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl   = (es_raw & 0x03); es_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti    = (es_raw & 0x01); es_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index = es_raw;
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x836, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x838, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x839, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x83a, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit = limit;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.a          = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.executable = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment    = (access & 0x01); access >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.dpl        = (access & 0x03); access >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.p          = (access & 0x01);

#if 0
    BX_INFO(("cs.dpl = %02x", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl));
    BX_INFO(("ss.dpl = %02x", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl));
    BX_INFO(("BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].dpl = %02x", (unsigned) BX_CPU_THIS_PTR ds.cache.dpl));
    BX_INFO(("BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].dpl = %02x", (unsigned) BX_CPU_THIS_PTR es.cache.dpl));
    BX_INFO(("LOADALL: setting cs.selector.rpl to %u",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl));
    BX_INFO(("LOADALL: setting ss.selector.rpl to %u",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl));
    BX_INFO(("LOADALL: setting ds.selector.rpl to %u",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl));
    BX_INFO(("LOADALL: setting es.selector.rpl to %u",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl));
#endif

  if ( (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid==0  ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment==0) {
    BX_PANIC(("loadall: ES invalid"));
    }

  /* DI */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x826, 2, &di);
  DI = di;

  /* SI */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x828, 2, &si);
  SI = si;

  /* BP */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x82a, 2, &bp);
  BP = bp;

  /* SP */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x82c, 2, &sp);
  SP = sp;

  /* BX */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x82e, 2, &bx);
  BX = bx;

  /* DX */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x830, 2, &dx);
  DX = dx;

  /* CX */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x832, 2, &cx);
  CX = cx;

  /* AX */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x834, 2, &ax);
  AX = ax;

  /* GDTR */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x84e, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x850, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x851, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x852, 2, &limit);
  BX_CPU_THIS_PTR gdtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR gdtr.limit = limit;

#if 0
  if (access)
      BX_INFO(("LOADALL: GDTR access bits not 0 (%02x).",
        (unsigned) access));
#endif

  /* IDTR */
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x85a, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x85c, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x85d, 1, &access);
  BX_CPU_THIS_PTR mem->readPhysicalPage(this, 0x85e, 2, &limit);
  BX_CPU_THIS_PTR idtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR idtr.limit = limit;
#endif
}

/* Get CPU feature flags. Returned by CPUID functions 1 and 80000001.  */
  static inline Bit32u
get_std_cpuid_features()
{
  unsigned features;

      // EAX[3:0]   Stepping ID
      // EAX[7:4]   Model: starts at 1
      // EAX[11:8]  Family: 4=486, 5=Pentium, 6=PPro
      // EAX[13:12] Type: 0=OEM,1=overdrive,2=dual cpu,3=reserved
      // EAX[31:14] Reserved
      // EBX:       Reserved (0)
      // ECX:       Reserved (0)
      // EDX:       Feature Flags
      //   [0:0]   FPU on chip
      //   [1:1]   VME: Virtual-8086 Mode enhancements
      //   [2:2]   DE: Debug Extensions (I/O breakpoints)
      //   [3:3]   PSE: Page Size Extensions
      //   [4:4]   TSC: Time Stamp Counter
      //   [5:5]   MSR: RDMSR and WRMSR support
      //   [6:6]   PAE: Physical Address Extensions
      //   [7:7]   MCE: Machine Check Exception
      //   [8:8]   CXS: CMPXCHG8B instruction
      //   [9:9]   APIC: APIC on Chip
      //   [10:10] Reserved
      //   [11:11] SYSENTER/SYSEXIT support
      //   [12:12] MTRR: Memory Type Range Reg
      //   [13:13] PGE/PTE Global Bit
      //   [14:14] MCA: Machine Check Architecture
      //   [15:15] CMOV: Cond Mov/Cmp Instructions
      //   [22:16] Reserved
      //   [23:23] MMX Technology
      //   [24]    FXSR: FSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
      //   [25]    SSE: SSE Extensions
      //   [26]    SSE2: SSE2 Extensions
      //   [28]    Reserved
      //   [29]    TM: Therm Monitor
      //   [31:30] Reserved

      features = 0; // start with none

#  if BX_SUPPORT_FPU
      features |= 0x01;
#  endif

#if (BX_CPU_LEVEL >= 5)
  features |= (1<<8);   // Support CMPXCHG8B instruction
      features |= (1<<4);   // implement TSC
#  if BX_SUPPORT_MMX
      features |= (1<<23);  // support MMX
#  endif
#endif

#if BX_CPU_LEVEL == 6
      features |= (1<<15);  // Implement CMOV instructions.
#  if BX_SUPPORT_APIC
      features |= (1<<9);   // APIC on chip
#  endif
#  if BX_SUPPORT_SSE >= 1
      features |= (1<<24);  // support FSAVE/FXRSTOR
      features |= (1<<25);  // support SSE
#  endif
#  if BX_SUPPORT_SSE >= 2
      features |= (1<<26);  // support SSE2
#  endif
#endif

#if BX_SUPPORT_4MEG_PAGES
      features |= (1<<3);   // Support Page-Size Extension (4M pages)
#endif

#if BX_SupportGlobalPages
      features |= (1<<13);  // Support Global pages.
#endif

#if BX_SupportPAE
      features |= (1<<6);   // Support PAE.
#endif

#if BX_SUPPORT_X86_64
  features |= (1<<5);       // AMD specific MSR's
#endif

#if BX_SUPPORT_SEP
  features |= (1<<11);      // SYSENTER/SYSEXIT
#endif

  return features;
}

  void
BX_CPU_C::CPUID(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  unsigned family, model, stepping;
#if BX_SUPPORT_X86_64
  unsigned features;
#endif
#endif

  invalidate_prefetch_q();

#if BX_CPU_LEVEL >= 4
  switch (EAX) {
    case 0:
      // EAX: highest input value understood by CPUID
      // EBX: vendor ID string
      // EDX: vendor ID string
      // ECX: vendor ID string
      RAX = 1; // 486 or pentium
#if BX_SUPPORT_X86_64
      RBX = 0x68747541; // "Auth"
      RDX = 0x69746e65; // "enti"
      RCX = 0x444d4163; // "cAMD"
#else
      RBX = 0x756e6547; // "Genu"
      RDX = 0x49656e69; // "ineI"
      RCX = 0x6c65746e; // "ntel"
#endif
      break;

    case 1:

#if BX_CPU_LEVEL == 4
  family = 4;
#  if BX_SUPPORT_FPU
  // 486dx
  model = 1;
  stepping = 3;
#  else
  // 486sx
  model = 2;
  stepping = 3;
#  endif

#elif BX_CPU_LEVEL == 5
  family = 5;
  model = 1; // Pentium (60,66)
  stepping = 3; // ???

#elif BX_CPU_LEVEL == 6
  family = 6;
#if BX_SUPPORT_X86_64
  model = 2; // Hammer returns what?
#else
  model = 1; // Pentium Pro
#endif
  stepping = 3; // ???
#else
  BX_PANIC(("CPUID: not implemented for > 6"));
#endif

      RAX = (family <<8) | (model<<4) | stepping;
      RBX = RCX = 0; // reserved
      RDX = get_std_cpuid_features ();
      break;

#if BX_SUPPORT_X86_64
    // x86-64 functions
    case 0x80000000:
      // max function supported.
      RAX = 0x80000008;
      RBX = 0x68747541; // "Auth"
      RDX = 0x69746e65; // "enti"
      RCX = 0x444d4163; // "cAMD"
      break;

    case 0x80000001:
      // long mode supported.
      features = get_std_cpuid_features ();
      RAX = features;
      // Many of the bits in EDX are the same as EAX
      // [10:10] Reserved
      // [11:11] SYSCALL and SYSRET instructions
      // [18:19] Reserved
      // [20:20] No-Execute page protection
      // [21:21] Reserved
      // [22:22] AMD MMX Extensions
      // [25:28] Reserved
      // [29:29] Long Mode
      // [30:30] AMD 3DNow Extensions
      // [31:31] AMD 3DNow Intructions
      features = features & 0x0183F3FF;
      RDX = features | (1 << 29) | (1 << 11);
      RAX = RBX = RCX = 0;
      break;

    case 0x80000008:
      // virtual & phys address size in low 2 bytes.
      RAX = 0x00003028;
      RBX = RCX = RDX = 0;
      break;
#endif  // #if BX_SUPPORT_X86_64

    default:
      RAX = RBX = RCX = RDX = 0; // Reserved, undefined
      break;
    }
#else
  BX_PANIC(("CPUID: not available on < late 486"));
#endif
}

  void
BX_CPU_C::SetCR0(Bit32u val_32)
{
  // from either MOV_CdRd() or debug functions
  // protection checks made already or forcing from debug
  bx_bool prev_pe, prev_pg;
  Bit32u oldCR0 = BX_CPU_THIS_PTR cr0.val32, newCR0;

  prev_pe = BX_CPU_THIS_PTR cr0.pe;
  prev_pg = BX_CPU_THIS_PTR cr0.pg;

  BX_CPU_THIS_PTR cr0.pe = val_32 & 0x01;
  BX_CPU_THIS_PTR cr0.mp = (val_32 >> 1) & 0x01;
  BX_CPU_THIS_PTR cr0.em = (val_32 >> 2) & 0x01;
  BX_CPU_THIS_PTR cr0.ts = (val_32 >> 3) & 0x01;
  // cr0.et is hardwired to 1
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR cr0.ne = (val_32 >> 5)  & 0x01;
  BX_CPU_THIS_PTR cr0.wp = (val_32 >> 16) & 0x01;
  BX_CPU_THIS_PTR cr0.am = (val_32 >> 18) & 0x01;
  BX_CPU_THIS_PTR cr0.nw = (val_32 >> 29) & 0x01;
  BX_CPU_THIS_PTR cr0.cd = (val_32 >> 30) & 0x01;
#endif
  BX_CPU_THIS_PTR cr0.pg = (val_32 >> 31) & 0x01;

  // handle reserved bits behaviour
#if BX_CPU_LEVEL == 3
  newCR0 = val_32 | 0x7ffffff0;
#elif BX_CPU_LEVEL == 4
  newCR0 = (val_32 | 0x00000010) & 0xe005003f;
#elif BX_CPU_LEVEL == 5
  newCR0 = val_32 | 0x00000010;
#elif BX_CPU_LEVEL == 6
  newCR0 = (val_32 | 0x00000010) & 0xe005003f;
#else
#error "MOV_CdRd: implement reserved bits behaviour for this CPU_LEVEL"
#endif
  BX_CPU_THIS_PTR cr0.val32 = newCR0;

  //if (BX_CPU_THIS_PTR cr0.ts)
  //  BX_INFO(("MOV_CdRd:CR0.TS set 0x%x", (unsigned) val_32));

  if (prev_pe==0 && BX_CPU_THIS_PTR cr0.pe) {
    enter_protected_mode();
    if (BX_CPU_THIS_PTR get_VM()) BX_PANIC(("EFLAGS.VM=1, enter_PM"));
    BX_CPU_THIS_PTR protectedMode = 1;
    BX_CPU_THIS_PTR v8086Mode = 0;
    BX_CPU_THIS_PTR realMode = 0;
    }
  else if (prev_pe==1 && BX_CPU_THIS_PTR cr0.pe==0) {
    enter_real_mode();
    BX_CPU_THIS_PTR protectedMode = 0;
    BX_CPU_THIS_PTR v8086Mode = 0;
    BX_CPU_THIS_PTR realMode = 1;
    }

#if BX_SUPPORT_X86_64
  if (prev_pg==0 && BX_CPU_THIS_PTR cr0.pg) {
    if (BX_CPU_THIS_PTR msr.lme) {
      if (!BX_CPU_THIS_PTR cr4.get_PAE()) {
        BX_PANIC(("SetCR0: attempt to enter x86-64 LONG mode without enabling CR4.PAE !!!"));
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      BX_CPU_THIS_PTR msr.lma = 1;
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_LONG_COMPAT;
#if BX_EXTERNAL_DEBUGGER
      //trap_debugger(0);
#endif
      }
    }
  else if (prev_pg==1 && BX_CPU_THIS_PTR cr0.pg==0) {
    if (BX_CPU_THIS_PTR msr.lma) {
      if (BX_CPU_THIS_PTR dword.rip_upper != 0) {
        BX_PANIC(("SetCR0: attempt to leave x86-64 LONG mode with RIP upper != 0 !!!"));
        }
      BX_CPU_THIS_PTR msr.lma = 0;
      BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32;
#if BX_EXTERNAL_DEBUGGER
      //trap_debugger(0);
#endif
      }
    }
#endif  // #if BX_SUPPORT_X86_64

  // Give the paging unit a chance to look for changes in bits
  // it cares about, like {PG,PE}, so it can flush cache entries etc.
  pagingCR0Changed(oldCR0, newCR0);
}

#if BX_CPU_LEVEL >= 4
  void
BX_CPU_C::SetCR4(Bit32u val_32)
{

  // CR4 bit definitions from AMD Hammer manual:
  //   [63-11] Reserved, Must be Zero
  //   [10]    OSXMMEXCPT: Operating System Unmasked Exception Support R/W
  //   [9]     OSFXSR: Operating System FXSAVE/FXRSTOR Support R/W
  //   [8]     PCE: Performance-Monitoring Counter Enable R/W
  //   [7]     PGE: Page-Global Enable R/W
  //   [6]     MCE: Machine Check Enable R/W
  //   [5]     PAE: Physical-Address Extension R/W
  //   [4]     PSE: Page Size Extensions R/W
  //   [3]     DE: Debugging Extensions R/W
  //   [2]     TSD: Time Stamp Disable R/W
  //   [1]     PVI: Protected-Mode Virtual Interrupts R/W
  //   [0]     VME: Virtual-8086 Mode Extensions R/W

  Bit32u allowMask = 0;
  Bit32u oldCR4 = BX_CPU_THIS_PTR cr4.getRegister();

#if BX_SUPPORT_4MEG_PAGES
  allowMask |= (1<<4);
#endif

#if BX_SupportGlobalPages
  allowMask |= (1<<7);
#endif

#if BX_SupportPAE
  allowMask |= (1<<5);
#endif

#if BX_SUPPORT_SSE != 0
  allowMask |= (1<<9);   /* OSFXSR */
#endif

#if BX_SUPPORT_X86_64
  allowMask |= (1<<9);   /* OSFXSR */
  allowMask |= (1<<10);
  //  need to GPF #0 if LME=1 and PAE=0
  if (   (BX_CPU_THIS_PTR msr.lme)
      && (!(val_32 >> 5) & 1)
      && (BX_CPU_THIS_PTR cr4.get_PAE())
      ) {
    exception(BX_GP_EXCEPTION, 0, 0);
    }
#endif

  // Need to GPF if trying to set undefined bits.
  if (val_32 & ~allowMask) {
    BX_INFO(("#GP(0): SetCR4: Write of 0x%08x not supported (allowMask=0x%x).",
             val_32, allowMask));
    exception(BX_GP_EXCEPTION, 0, 0);
    }
  val_32 &= allowMask; // Screen out unsupported bits. (not needed, for good measure)
  BX_CPU_THIS_PTR cr4.setRegister(val_32);
  pagingCR4Changed(oldCR4, BX_CPU_THIS_PTR cr4.getRegister());
}
#endif


  void
BX_CPU_C::RSM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  BX_PANIC(("RSM: System Management Mode not implemented yet"));
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::RDTSC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  bx_bool tsd = BX_CPU_THIS_PTR cr4.get_TSD();
  bx_bool cpl = CPL;
  if ((tsd==0) || (tsd==1 && cpl==0)) {
    // return ticks
    Bit64u ticks = bx_pc_system.time_ticks ();
    RAX = (Bit32u) (ticks & 0xffffffff);
    RDX = (Bit32u) ((ticks >> 32) & 0xffffffff);
    //BX_INFO(("RDTSC: returning EDX:EAX = %08x:%08x", EDX, EAX));
  } else {
    // not allowed to use RDTSC!
    exception (BX_GP_EXCEPTION, 0, 0);
  }
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::RDMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  invalidate_prefetch_q();

  if (v8086_mode()) {
    BX_INFO(("RDMSR: Invalid whilst in virtual 8086 mode"));
    goto do_exception;
    }

  if (CPL!= 0) {
    BX_INFO(("RDMSR: CPL!= 0"));
    goto do_exception;
    }

  /* We have the requested MSR register in ECX */
  switch(ECX) {

#if BX_SUPPORT_SEP
    case BX_MSR_SYSENTER_CS:  { EAX = BX_CPU_THIS_PTR sysenter_cs_msr;  EDX = 0; return; }
    case BX_MSR_SYSENTER_ESP: { EAX = BX_CPU_THIS_PTR sysenter_esp_msr; EDX = 0; return; }
    case BX_MSR_SYSENTER_EIP: { EAX = BX_CPU_THIS_PTR sysenter_eip_msr; EDX = 0; return; }
#endif 

#if BX_CPU_LEVEL == 5
    /* The following registers are defined for Pentium only */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
      /* TODO */
      return;

    case BX_MSR_TSC:
      RDTSC(i);
      return;

    case BX_MSR_CESR:
      /* TODO */
      return;
#else
    /* These are noops on i686... */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
      /* do nothing */
      return;

    case BX_MSR_TSC:
      RDTSC(i);
      return;

    /* ... And these cause an exception on i686 */
    case BX_MSR_CESR:
    case BX_MSR_CTR0:
    case BX_MSR_CTR1:
      goto do_exception;
#endif  /* BX_CPU_LEVEL == 5 */

    /* MSR_APICBASE
       0:7    Reserved
       8    This is set if its the BSP
       9:10    Reserved
       11    APIC Global Enable bit (1=enabled 0=disabled)
       12:35  APIC Base Address
       36:63  Reserved
    */
    case BX_MSR_APICBASE:
      /* we return low 32 bits in EAX, and high in EDX */
      RAX = Bit32u(BX_CPU_THIS_PTR msr.apicbase & 0xffffffff);
      RDX = Bit32u(BX_CPU_THIS_PTR msr.apicbase >> 32);
      BX_INFO(("RDMSR: Read %08x:%08x from MSR_APICBASE", EDX, EAX));
      return;

#if BX_SUPPORT_X86_64
                case BX_MSR_EFER:
                        RAX = (BX_CPU_THIS_PTR msr.sce << 0)
                            | (BX_CPU_THIS_PTR msr.lme << 8)
                            | (BX_CPU_THIS_PTR msr.lma << 10);
                        RDX = 0;
                        return;

                case BX_MSR_STAR:
                        RAX = MSR_STAR;
                        RDX = MSR_STAR >> 32;
                        return;

                case BX_MSR_LSTAR:
                        RAX = MSR_LSTAR;
                        RDX = MSR_LSTAR >> 32;
                        return;

                case BX_MSR_CSTAR:
                        RAX = MSR_CSTAR;
                        RDX = MSR_CSTAR >> 32;
                        return;

                case BX_MSR_FMASK:
                        RAX = MSR_FMASK;
                        RDX = MSR_FMASK >> 32;
                        return;

                case BX_MSR_FSBASE:
                        RAX = MSR_FSBASE;
                        RDX = MSR_FSBASE >> 32;
                        return;

                case BX_MSR_GSBASE:
                        RAX = MSR_GSBASE;
                        RDX = MSR_GSBASE >> 32;
                        return;

                case BX_MSR_KERNELGSBASE:
                        RAX = MSR_KERNELGSBASE;
                        RDX = MSR_KERNELGSBASE >> 32;
                        return;
#endif  // #if BX_SUPPORT_X86_64

    default:
#if BX_IGNORE_BAD_MSR
                        BX_ERROR(("RDMSR: Unknown register %#x", ECX));
                        return;
#else
      BX_PANIC(("RDMSR: Unknown register %#x", ECX));
#endif
      goto do_exception;

  }
#endif  /* BX_CPU_LEVEL >= 5 */

do_exception:
  exception(BX_GP_EXCEPTION, 0, 0);
}

  void
BX_CPU_C::WRMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  invalidate_prefetch_q();

  if (v8086_mode()) {
    BX_INFO(("WRMSR: Invalid whilst in virtual 8086 mode"));
    goto do_exception;
    }

  if (CPL!= 0) {
    BX_INFO(("WDMSR: CPL!= 0"));
    goto do_exception;
    }

  /* ECX has the MSR to write to */
  switch(ECX) {

#if BX_SUPPORT_SEP
    case BX_MSR_SYSENTER_CS:  {
      if (EAX & 3) BX_PANIC (("writing sysenter_cs_msr with non-kernel mode selector %X", EAX));  // not a bug according to book
      BX_CPU_THIS_PTR sysenter_cs_msr  = EAX;                                                     // ... but very stOOpid
      return;
    }
    case BX_MSR_SYSENTER_ESP: { BX_CPU_THIS_PTR sysenter_esp_msr = EAX; return; }
    case BX_MSR_SYSENTER_EIP: { BX_CPU_THIS_PTR sysenter_eip_msr = EAX; return; }
#endif

#if BX_CPU_LEVEL == 5
    /* The following registers are defined for Pentium only */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
    case BX_MSR_TSC:
    case BX_MSR_CESR:
      /* TODO */
      return;
#else
    /* These are noops on i686... */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
    case BX_MSR_TSC:
      /* do nothing */
      return;

    /* ... And these cause an exception on i686 */
    case BX_MSR_CESR:
    case BX_MSR_CTR0:
    case BX_MSR_CTR1:
      goto do_exception;
#endif  /* BX_CPU_LEVEL == 5 */

    /* MSR_APICBASE
       0:7    Reserved
       8    This is set if its the BSP
       9:10    Reserved
       11    APIC Global Enable bit (1=enabled 0=disabled)
       12:35  APIC Base Address
       36:63  Reserved
    */

    case BX_MSR_APICBASE:
      BX_CPU_THIS_PTR msr.apicbase = ((Bit64u) EDX << 32) + EAX;
      BX_INFO(("WRMSR: wrote %08x:%08x to MSR_APICBASE", EDX, EAX));
      return;

#if BX_SUPPORT_X86_64
                case BX_MSR_EFER:
                        // GPF #0 if lme 0->1 and cr0.pg = 1
                        // GPF #0 if lme 1->0 and cr0.pg = 1
                        if (  (BX_CPU_THIS_PTR msr.lme != (EAX >> 8) & 1)
                           && (BX_CPU_THIS_PTR cr0.pg == 1)) {
                          exception(BX_GP_EXCEPTION, 0, 0);
                          }
                        BX_CPU_THIS_PTR msr.sce = (EAX >> 0) & 1;
                        BX_CPU_THIS_PTR msr.lme = (EAX >> 8) & 1;
                        return;
                case BX_MSR_STAR:
                        MSR_STAR = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_LSTAR:
                        MSR_LSTAR = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_CSTAR:
                        MSR_CSTAR = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_FMASK:
                        MSR_FMASK = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_FSBASE:
                        MSR_FSBASE = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_GSBASE:
                        MSR_GSBASE = ((Bit64u) EDX << 32) + EAX;
                        return;
                case BX_MSR_KERNELGSBASE:
                        MSR_KERNELGSBASE = ((Bit64u) EDX << 32) + EAX;
                        return;
#endif  // #if BX_SUPPORT_X86_64

    default:
#if BX_IGNORE_BAD_MSR
                        BX_ERROR(("WRMSR: Unknown register %#x", ECX));
                        return;
#else
      BX_PANIC(("WRMSR: Unknown register %#x", ECX));
#endif
      goto do_exception;

  }
#endif  /* BX_CPU_LEVEL >= 5 */

do_exception:
  exception(BX_GP_EXCEPTION, 0, 0);

}

  void 
BX_CPU_C::SYSENTER (bxInstruction_c *i)
{
#if BX_SUPPORT_SEP
  if (!protected_mode ()) {
    BX_INFO (("sysenter not from protected mode"));
    exception (BX_GP_EXCEPTION, 0, 0);
    return;
  }
  if (BX_CPU_THIS_PTR sysenter_cs_msr == 0) {
    BX_INFO (("sysenter with zero sysenter_cs_msr"));
    exception (BX_GP_EXCEPTION, 0, 0);
    return;
  }

  invalidate_prefetch_q ();

  BX_CPU_THIS_PTR set_VM(0);           // do this just like the book says to do
  BX_CPU_THIS_PTR set_IF(0);
  BX_CPU_THIS_PTR set_RF(0);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = BX_CPU_THIS_PTR sysenter_cs_msr;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = BX_CPU_THIS_PTR sysenter_cs_msr >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (BX_CPU_THIS_PTR sysenter_cs_msr >> 2) & 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable   = 1;          // code segment
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed         = 0;          // non-conforming
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w          = 1;          // readable
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a            = 1;          // accessed
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;     // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF; // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;          // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;          // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;          // available for use by system

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = BX_CPU_THIS_PTR sysenter_cs_msr + 8;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = (BX_CPU_THIS_PTR sysenter_cs_msr + 8) >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (BX_CPU_THIS_PTR sysenter_cs_msr >> 2) & 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable   = 0;          // data segment
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed         = 0;          // expand-up
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w          = 1;          // writeable
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a            = 1;          // accessed
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;     // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF; // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1;          // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1;          // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;          // available for use by system

  // BX_INFO (("sysenter: old eip %X, esp %x, new eip %x, esp %X, edx %X", BX_CPU_THIS_PTR prev_eip, ESP, BX_CPU_THIS_PTR sysenter_eip_msr, BX_CPU_THIS_PTR sysenter_esp_msr, EDX));

  ESP = BX_CPU_THIS_PTR sysenter_esp_msr;
  EIP = BX_CPU_THIS_PTR sysenter_eip_msr;
#else
  UndefinedOpcode (i);
#endif
}

  void 
BX_CPU_C::SYSEXIT (bxInstruction_c *i)
{
#if BX_SUPPORT_SEP
  if (!protected_mode ()) {
    BX_INFO (("sysexit not from protected mode"));
    exception (BX_GP_EXCEPTION, 0, 0);
    return;
  }
  if (BX_CPU_THIS_PTR sysenter_cs_msr == 0) {
    BX_INFO (("sysexit with zero sysenter_cs_msr"));
    exception (BX_GP_EXCEPTION, 0, 0);
    return;
  }
  if (CPL != 0) {
    BX_INFO (("sysexit at non-zero cpl %u", CPL));
    exception (BX_GP_EXCEPTION, 0, 0);
    return;
  }

  invalidate_prefetch_q ();

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = (BX_CPU_THIS_PTR sysenter_cs_msr + 16) | 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = (BX_CPU_THIS_PTR sysenter_cs_msr + 16) >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (BX_CPU_THIS_PTR sysenter_cs_msr >> 2) & 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable   = 1;           // code segment
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed         = 0;           // non-conforming
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w          = 1;           // readable
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a            = 1;           // accessed
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;           // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;           // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;           // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;           // available for use by system

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = (BX_CPU_THIS_PTR sysenter_cs_msr + 24) | 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = (BX_CPU_THIS_PTR sysenter_cs_msr + 24) >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (BX_CPU_THIS_PTR sysenter_cs_msr >> 2) & 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable   = 0;           // data segment
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed         = 0;           // expand-up
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w          = 1;           // writeable
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a            = 1;           // accessed
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;           // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;      // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1;           // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1;           // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;           // available for use by system

  // BX_INFO (("sysexit: old eip %X, esp %x, new eip %x, esp %X, eax %X", BX_CPU_THIS_PTR prev_eip, ESP, EDX, ECX, EAX));

  ESP = ECX;
  EIP = EDX;
#else
  UndefinedOpcode (i);
#endif
}

#if BX_SUPPORT_X86_64
  void
BX_CPU_C::SWAPGS(bxInstruction_c *i)
{
  Bit64u temp_GS_base;

  if(CPL != 0) {
    exception(BX_GP_EXCEPTION, 0, 0);
    }
  temp_GS_base = MSR_GSBASE;
  MSR_GSBASE = MSR_KERNELGSBASE;
  MSR_KERNELGSBASE = temp_GS_base;

}
#endif

#if BX_X86_DEBUGGER
  Bit32u
BX_CPU_C::hwdebug_compare(Bit32u laddr_0, unsigned size,
                          unsigned opa, unsigned opb)
{
  // Support x86 hardware debug facilities (DR0..DR7)
  Bit32u dr7 = BX_CPU_THIS_PTR dr7;

  bx_bool ibpoint_found = 0;
  Bit32u  laddr_n = laddr_0 + (size - 1);
  Bit32u  dr0, dr1, dr2, dr3;
  Bit32u  dr0_n, dr1_n, dr2_n, dr3_n;
  Bit32u  len0, len1, len2, len3;
  static  unsigned alignment_mask[4] =
    //    00b=1      01b=2     10b=undef     11b=4
    { 0xffffffff, 0xfffffffe, 0xffffffff, 0xfffffffc };
  Bit32u dr0_op, dr1_op, dr2_op, dr3_op;

  len0 = (dr7>>18) & 3;
  len1 = (dr7>>22) & 3;
  len2 = (dr7>>26) & 3;
  len3 = (dr7>>30) & 3;

  dr0 = BX_CPU_THIS_PTR dr0 & alignment_mask[len0];
  dr1 = BX_CPU_THIS_PTR dr1 & alignment_mask[len1];
  dr2 = BX_CPU_THIS_PTR dr2 & alignment_mask[len2];
  dr3 = BX_CPU_THIS_PTR dr3 & alignment_mask[len3];

  dr0_n = dr0 + len0;
  dr1_n = dr1 + len1;
  dr2_n = dr2 + len2;
  dr3_n = dr3 + len3;

  dr0_op = (dr7>>16) & 3;
  dr1_op = (dr7>>20) & 3;
  dr2_op = (dr7>>24) & 3;
  dr3_op = (dr7>>28) & 3;

  // See if this instruction address matches any breakpoints
  if ( (dr7 & 0x00000003) ) {
    if ( (dr0_op==opa || dr0_op==opb) &&
         (laddr_0 <= dr0_n) &&
         (laddr_n >= dr0) )
      ibpoint_found = 1;
    }
  if ( (dr7 & 0x0000000c) ) {
    if ( (dr1_op==opa || dr1_op==opb) &&
         (laddr_0 <= dr1_n) &&
         (laddr_n >= dr1) )
      ibpoint_found = 1;
    }
  if ( (dr7 & 0x00000030) ) {
    if ( (dr2_op==opa || dr2_op==opb) &&
         (laddr_0 <= dr2_n) &&
         (laddr_n >= dr2) )
      ibpoint_found = 1;
    }
  if ( (dr7 & 0x000000c0) ) {
    if ( (dr3_op==opa || dr3_op==opb) &&
         (laddr_0 <= dr3_n) &&
         (laddr_n >= dr3) )
      ibpoint_found = 1;
    }

  // If *any* enabled breakpoints matched, then we need to
  // set status bits for *all* breakpoints, even disabled ones,
  // as long as they meet the other breakpoint criteria.
  // This code is similar to that above, only without the
  // breakpoint enabled check.  Seems weird to duplicate effort,
  // but its more efficient to do it this way.
  if (ibpoint_found) {
    // dr6_mask is the return value.  These bits represent the bits to
    // be OR'd into DR6 as a result of the debug event.
    Bit32u  dr6_mask=0;
    if ( (dr0_op==opa || dr0_op==opb) &&
         (laddr_0 <= dr0_n) &&
         (laddr_n >= dr0) )
      dr6_mask |= 0x01;
    if ( (dr1_op==opa || dr1_op==opb) &&
         (laddr_0 <= dr1_n) &&
         (laddr_n >= dr1) )
      dr6_mask |= 0x02;
    if ( (dr2_op==opa || dr2_op==opb) &&
         (laddr_0 <= dr2_n) &&
         (laddr_n >= dr2) )
      dr6_mask |= 0x04;
    if ( (dr3_op==opa || dr3_op==opb) &&
         (laddr_0 <= dr3_n) &&
         (laddr_n >= dr3) )
      dr6_mask |= 0x08;
    return(dr6_mask);
    }
  return(0);
}
#endif
