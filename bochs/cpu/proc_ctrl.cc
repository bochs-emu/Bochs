/////////////////////////////////////////////////////////////////////////
// $Id: proc_ctrl.cc,v 1.24 2002-08-01 07:23:11 cbothamy Exp $
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

  void
BX_CPU_C::UndefinedOpcode(BxInstruction_t *i)
{
  BX_DEBUG(("UndefinedOpcode: %02x causes exception 6",
              (unsigned) i->b1));
  exception(BX_UD_EXCEPTION, 0, 0);
}

  void
BX_CPU_C::NOP(BxInstruction_t *i)
{
}

  void
BX_CPU_C::HLT(BxInstruction_t *i)
{
  // hack to panic if HLT comes from BIOS
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value == 0xf000 )
    BX_PANIC(("HALT instruction encountered in the BIOS ROM"));

  if (CPL!=0) {
    BX_INFO(("HLT(): CPL!=0"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if ( ! BX_CPU_THIS_PTR eflags.if_ ) {
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
  bx_gui.sim_is_idle ();
#endif /* BX_USE_IDLE_HACK */  
}


  void
BX_CPU_C::CLTS(BxInstruction_t *i)
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
BX_CPU_C::INVD(BxInstruction_t *i)
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
  BX_INSTR_CACHE_CNTRL(BX_INSTR_INVD);
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::WBINVD(BxInstruction_t *i)
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
  BX_INSTR_CACHE_CNTRL(BX_INSTR_WBINVD);
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::MOV_DdRd(BxInstruction_t *i)
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

  if (i->mod != 0xc0) {
    BX_PANIC(("MOV_DdRd(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_DdRd: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  val_32 = BX_READ_32BIT_REG(i->rm);
  if (bx_dbg.dreg)
    BX_INFO(("MOV_DdRd: DR[%u]=%08xh unhandled",
      (unsigned) i->nnn, (unsigned) val_32));

  switch (i->nnn) {
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
      if ( (i->nnn == 4) && (BX_CPU_THIS_PTR cr4 & 0x00000008) ) {
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
      if ( (i->nnn == 5) && (BX_CPU_THIS_PTR cr4 & 0x00000008) ) {
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
BX_CPU_C::MOV_RdDd(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RdDd: not supported on < 386"));
#else
  Bit32u val_32;

  if (v8086_mode()) {
    BX_INFO(("MOV_RdDd: v8086 mode causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  if (i->mod != 0xc0) {
    BX_PANIC(("MOV_RdDd(): rm field not a register!"));
    UndefinedOpcode(i);
    }

  if (protected_mode() && (CPL!=0)) {
    BX_INFO(("MOV_RdDd: CPL!=0 causes #GP"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if (bx_dbg.dreg)
    BX_INFO(("MOV_RdDd: DR%u not implemented yet", i->nnn));

  switch (i->nnn) {
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
      if ( (i->nnn == 4) && (BX_CPU_THIS_PTR cr4 & 0x00000008) ) {
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
      if ( (i->nnn == 5) && (BX_CPU_THIS_PTR cr4 & 0x00000008) ) {
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
  BX_WRITE_32BIT_REG(i->rm, val_32);
#endif
}


  void
BX_CPU_C::LMSW_Ew(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("LMSW_Ew(): not supported on 8086!"));
#else
  Bit16u msw;
  Bit32u cr0;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

  if ( protected_mode() ) {
    if ( CPL != 0 ) {
      BX_INFO(("LMSW: CPL != 0, CPL=%u", (unsigned) CPL));
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  if (i->mod == 0xc0) {
    msw = BX_READ_16BIT_REG(i->rm);
    }
  else {
    read_virtual_word(i->seg, i->rm_addr, &msw);
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
BX_CPU_C::SMSW_Ew(BxInstruction_t *i)
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


  if (i->mod == 0xc0) {
    if (i->os_32) {
      BX_WRITE_32BIT_REG(i->rm, msw);  // zeros out high 16bits
      }
    else {
      BX_WRITE_16BIT_REG(i->rm, msw);
      }
    }
  else {
    write_virtual_word(i->seg, i->rm_addr, &msw);
    }

#endif
}


  void
BX_CPU_C::MOV_CdRd(BxInstruction_t *i)
{
  // mov general register data to control register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_CdRd: not supported on < 386"));
#else
  Bit32u val_32;


  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (i->mod != 0xc0) {
    BX_PANIC(("MOV_CdRd(): rm field not a register!"));
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    BX_PANIC(("MOV_CdRd: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_32 = BX_READ_32BIT_REG(i->rm);

  switch (i->nnn) {
    case 0: // CR0 (MSW)
      // BX_INFO(("MOV_CdRd:CR0: R32 = %08x\n @CS:EIP %04x:%04x ",
      //   (unsigned) val_32,
      //   (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
      //   (unsigned) BX_CPU_THIS_PTR eip));
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
      BX_INSTR_TLB_CNTRL(BX_INSTR_MOV_CR3, val_32);
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      BX_PANIC(("MOV_CdRd: write to CR4 of 0x%08x on 386",
        val_32));
      UndefinedOpcode(i);
#else
      //  Protected mode: #GP(0) if attempt to write a 1 to
      //  any reserved bit of CR4

      if (val_32 & ~ 0x10) { // support CR4<PSE> (to allow 4M pages)
        BX_INFO(("MOV_CdRd: (CR4) write of 0x%08x not supported!",
          val_32));
        }
      // Only allow writes of 0 to CR4 for now.
      // Writes to bits in CR4 should not be 1s as CPUID
      // returns not-supported for all of these features.
      BX_CPU_THIS_PTR cr4 = val_32 & 0x10;
#endif
      break;
    default:
      BX_PANIC(("MOV_CdRd: control register index out of range"));
      break;
    }
#endif
}

  void
BX_CPU_C::MOV_RdCd(BxInstruction_t *i)
{
  // mov control register data to register
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_RdCd: not supported on < 386"));
#else
  Bit32u val_32;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (i->mod != 0xc0) {
    BX_PANIC(("MOV_RdCd(): rm field not a register!"));
    }

  if (protected_mode() && CPL!=0) {
//    BX_PANIC(("MOV_RdCd: CPL!=0"));
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (i->nnn) {
    case 0: // CR0 (MSW)
      val_32 = BX_CPU_THIS_PTR cr0.val32;
#if 0
      BX_INFO(("MOV_RdCd:CR0: R32 = %08x\n @CS:EIP %04x:%04x",
        (unsigned) val_32,
        (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
        (unsigned) BX_CPU_THIS_PTR eip));
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
      val_32 = BX_CPU_THIS_PTR cr4;
#endif
      break;
    default:
      BX_PANIC(("MOV_RdCd: control register index out of range"));
      val_32 = 0;
    }
  BX_WRITE_32BIT_REG(i->rm, val_32);
#endif
}

  void
BX_CPU_C::MOV_TdRd(BxInstruction_t *i)
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
BX_CPU_C::MOV_RdTd(BxInstruction_t *i)
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
BX_CPU_C::LOADALL(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("undocumented LOADALL instruction not supported on 8086"));
#else
  Bit16u msw, tr, flags, ip, ldtr;
  Bit16u ds_raw, ss_raw, cs_raw, es_raw;
  Bit16u di, si, bp, sp, bx, dx, cx, ax;
  Bit16u base_15_0, limit;
  Bit8u  base_23_16, access;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: v8086 mode unsupported"));

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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x806, 2, &msw);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x816, 2, &tr);
  BX_CPU_THIS_PTR tr.selector.value = tr;
  BX_CPU_THIS_PTR tr.selector.rpl   = (tr & 0x03);  tr >>= 2;
  BX_CPU_THIS_PTR tr.selector.ti    = (tr & 0x01);  tr >>= 1;
  BX_CPU_THIS_PTR tr.selector.index = tr;
  BX_CPU_THIS_PTR mem->read_physical(this, 0x860, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x862, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x863, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x864, 2, &limit);


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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x818, 2, &flags);
  write_flags(flags, 1, 1);

  /* IP */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x81a, 2, &ip);
  IP = ip;

  /* LDTR */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x81c, 2, &ldtr);
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
    BX_CPU_THIS_PTR mem->read_physical(this, 0x854, 2, &base_15_0);
    BX_CPU_THIS_PTR mem->read_physical(this, 0x856, 1, &base_23_16);
    BX_CPU_THIS_PTR mem->read_physical(this, 0x857, 1, &access);
    BX_CPU_THIS_PTR mem->read_physical(this, 0x858, 2, &limit);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x81e, 2, &ds_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = ds_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl   = (ds_raw & 0x03);  ds_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti    = (ds_raw & 0x01);  ds_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index = ds_raw;
  BX_CPU_THIS_PTR mem->read_physical(this, 0x848, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x84a, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x84b, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x84c, 2, &limit);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x820, 2, &ss_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = ss_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = (ss_raw & 0x03); ss_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (ss_raw & 0x01); ss_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = ss_raw;
  BX_CPU_THIS_PTR mem->read_physical(this, 0x842, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x844, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x845, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x846, 2, &limit);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x822, 2, &cs_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = cs_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = (cs_raw & 0x03); cs_raw >>= 2;

  //BX_INFO(("LOADALL: setting cs.selector.rpl to %u",
  //  (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl));

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (cs_raw & 0x01); cs_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = cs_raw;
  BX_CPU_THIS_PTR mem->read_physical(this, 0x83c, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x83e, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x83f, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x840, 2, &limit);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x824, 2, &es_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = es_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl   = (es_raw & 0x03); es_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti    = (es_raw & 0x01); es_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index = es_raw;
  BX_CPU_THIS_PTR mem->read_physical(this, 0x836, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x838, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x839, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x83a, 2, &limit);
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
  BX_CPU_THIS_PTR mem->read_physical(this, 0x826, 2, &di);
  DI = di;

  /* SI */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x828, 2, &si);
  SI = si;

  /* BP */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x82a, 2, &bp);
  BP = bp;

  /* SP */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x82c, 2, &sp);
  SP = sp;

  /* BX */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x82e, 2, &bx);
  BX = bx;

  /* DX */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x830, 2, &dx);
  DX = dx;

  /* CX */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x832, 2, &cx);
  CX = cx;

  /* AX */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x834, 2, &ax);
  AX = ax;

  /* GDTR */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x84e, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x850, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x851, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x852, 2, &limit);
  BX_CPU_THIS_PTR gdtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR gdtr.limit = limit;

#if 0
  if (access)
      BX_INFO(("LOADALL: GDTR access bits not 0 (%02x).",
        (unsigned) access));
#endif

  /* IDTR */
  BX_CPU_THIS_PTR mem->read_physical(this, 0x85a, 2, &base_15_0);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x85c, 1, &base_23_16);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x85d, 1, &access);
  BX_CPU_THIS_PTR mem->read_physical(this, 0x85e, 2, &limit);
  BX_CPU_THIS_PTR idtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR idtr.limit = limit;
#endif
}


  void
BX_CPU_C::CPUID(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 4
  unsigned type, family, model, stepping, features;
#endif

  invalidate_prefetch_q();

#if BX_CPU_LEVEL >= 4
  switch (EAX) {
    case 0:
      // EAX: highest input value understood by CPUID
      // EBX: vendor ID string
      // EDX: vendor ID string
      // ECX: vendor ID string
      EAX = 1; // 486 or pentium
      EBX = 0x756e6547; // "Genu"
      EDX = 0x49656e69; // "ineI"
      ECX = 0x6c65746e; // "ntel"
      break;

    case 1:
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
      //   [11:10] Reserved
      //   [12:12] MTRR: Memory Type Range Reg
      //   [13:13] PGE/PTE Global Bit
      //   [14:14] MCA: Machine Check Architecture
      //   [15:15] CMOV: Cond Mov/Cmp Instructions
      //   [22:16] Reserved
      //   [23:23] MMX Technology
      //   [31:24] Reserved

      features = 0; // start with none
      type = 0; // OEM

#if BX_CPU_LEVEL == 4
      family = 4;
#  if BX_SUPPORT_FPU
      // 486dx
      model = 1;
      stepping = 3;
      features |= 0x01;
#  else
      // 486sx
      model = 2;
      stepping = 3;
#  endif

#elif BX_CPU_LEVEL == 5
      family = 5;
      model = 1; // Pentium (60,66)
      stepping = 3; // ???
      features |= (1<<4);   // implement TSC
#  if BX_SUPPORT_FPU
      features |= 0x01;
#  endif

#elif BX_CPU_LEVEL == 6
      family = 6;
      model = 1; // Pentium Pro
      stepping = 3; // ???
      features |= (1<<4);   // implement TSC
#  if BX_SUPPORT_APIC
      features |= (1<<9);   // APIC on chip
#  endif
#  if BX_SUPPORT_FPU
      features |= 0x01;     // has FPU
#  endif
#else
      BX_PANIC(("CPUID: not implemented for > 6"));
#endif
      features |= 8; // support page-size extension (4m pages)

      EAX = (family <<8) | (model<<4) | stepping;
      EBX = ECX = 0; // reserved
      EDX = features;
      break;

    default:
      EAX = EBX = ECX = EDX = 0; // Reserved, undefined
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
  Boolean prev_pe, prev_pg;

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
  BX_CPU_THIS_PTR cr0.val32 = val_32 | 0x7ffffff0;
#elif BX_CPU_LEVEL == 4
  BX_CPU_THIS_PTR cr0.val32 = (val_32 | 0x00000010) & 0xe005003f;
#elif BX_CPU_LEVEL == 5
  BX_CPU_THIS_PTR cr0.val32 = val_32 | 0x00000010;
#elif BX_CPU_LEVEL == 6
  BX_CPU_THIS_PTR cr0.val32 = (val_32 | 0x00000010) & 0xe005003f;
#else
#error "MOV_CdRd: implement reserved bits behaviour for this CPU_LEVEL"
#endif

  //if (BX_CPU_THIS_PTR cr0.ts)
  //  BX_INFO(("MOV_CdRd:CR0.TS set 0x%x", (unsigned) val_32));

  if (prev_pe==0 && BX_CPU_THIS_PTR cr0.pe) {
    enter_protected_mode();
    }
  else if (prev_pe==1 && BX_CPU_THIS_PTR cr0.pe==0) {
    enter_real_mode();
    }

  if (prev_pg==0 && BX_CPU_THIS_PTR cr0.pg)
    enable_paging();
  else if (prev_pg==1 && BX_CPU_THIS_PTR cr0.pg==0)
    disable_paging();
}


  void
BX_CPU_C::RSM(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  BX_PANIC(("RSM: System Management Mode not implemented yet"));
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::RDTSC(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 5
  Boolean tsd = (BX_CPU_THIS_PTR cr4 & 4)? 1 : 0;
  Boolean cpl = CPL;
  if ((tsd==0) || (tsd==1 && cpl==0)) {
    // return ticks
    Bit64u ticks = bx_pc_system.time_ticks ();
    EAX = (Bit32u) (ticks & 0xffffffff);
    EDX = (Bit32u) ((ticks >> 32) & 0xffffffff);
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
BX_CPU_C::RDMSR(BxInstruction_t *i)
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
#endif	/* BX_CPU_LEVEL == 5 */

		/* MSR_APICBASE
		   0:7		Reserved
		   8		This is set if its the BSP
		   9:10		Reserved
		   11		APIC Global Enable bit (1=enabled 0=disabled)
		   12:35	APIC Base Address
		   36:63	Reserved
		*/
		case BX_MSR_APICBASE:
			/* we return low 32 bits in EAX, and high in EDX */
			EAX = Bit32u(BX_CPU_THIS_PTR msr.apicbase & 0xffffffff);
			EDX = Bit32u(BX_CPU_THIS_PTR msr.apicbase >> 32);
			BX_INFO(("RDMSR: Read %08x:%08x from MSR_APICBASE", EDX, EAX));
			return;
			
		default:
			BX_PANIC(("RDMSR: Unknown register %#x", ECX));
			goto do_exception;
			
	}
#endif	/* BX_CPU_LEVEL >= 5 */

do_exception:
	exception(BX_GP_EXCEPTION, 0, 0);
}

  void
BX_CPU_C::WRMSR(BxInstruction_t *i)
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
#endif	/* BX_CPU_LEVEL == 5 */

		/* MSR_APICBASE
		   0:7		Reserved
		   8		This is set if its the BSP
		   9:10		Reserved
		   11		APIC Global Enable bit (1=enabled 0=disabled)
		   12:35	APIC Base Address
		   36:63	Reserved
		*/

		case BX_MSR_APICBASE:
			BX_CPU_THIS_PTR msr.apicbase = ((Bit64u)EDX << 32) + EAX;
			BX_INFO(("WRMSR: wrote %08x:%08x to MSR_APICBASE", EDX, EAX));
			return;
			
		default:
			BX_PANIC(("WRMSR: Unknown register %#x", ECX));
			goto do_exception;
			
	}
#endif	/* BX_CPU_LEVEL >= 5 */

do_exception:
	exception(BX_GP_EXCEPTION, 0, 0);

}

#if BX_X86_DEBUGGER
  Bit32u
BX_CPU_C::hwdebug_compare(Bit32u laddr_0, unsigned size,
                          unsigned opa, unsigned opb)
{
  // Support x86 hardware debug facilities (DR0..DR7)
  Bit32u dr7 = BX_CPU_THIS_PTR dr7;

  Boolean ibpoint_found = 0;
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
