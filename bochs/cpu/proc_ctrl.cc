//  Copyright (C) 2000  MandrakeSoft S.A.
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







#include "bochs.h"


  void
BX_CPU_C::UndefinedOpcode(BxInstruction_t *i)
{
  if (i->b1 != 0x63) {
    // Windows hits the ARPL command a bunch of times.
    // Too much spew...
    bx_printf("UndefinedOpcode: %02x causes exception 6\n",
              (unsigned) i->b1);
    }
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
    bx_panic("HALT instruction encountered\n");

  if (CPL!=0) {
    bx_printf("HLT(): CPL!=0\n");
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if ( ! BX_CPU_THIS_PTR eflags.if_ ) {
    fprintf(stderr, "WARNING: HLT instruction with IF=0!\n");
    bx_printf("WARNING: HLT instruction with IF=0!\n");
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
}


#if 0
// (kpl) used to be called by ::HLT()
// (mch) Wait for an interrupt
  void
BX_CPU_C::wait_for_interrupt()
{ 
  
  while ( !(BX_INTR) ) {
#if MCH_WAIT_EVENT_HACK
    bx_pc_system.wait_for_event();
#else
    BX_TICK1();
#endif
    }
}
#endif


  void
BX_CPU_C::CLTS(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("CLTS: not implemented for < 286\n");
#else

  if (v8086_mode()) bx_panic("clts: v8086 mode unsupported\n");

  /* read errata file */
  // does CLTS also clear NT flag???

  // #GP(0) if CPL is not 0
  if (CPL!=0) {
    bx_printf("CLTS(): CPL!=0\n");
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
  bx_printf("---------------\n");
  bx_printf("- INVD called -\n");
  bx_printf("---------------\n");

#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      bx_printf("INVD: CPL!=0\n");
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
  bx_printf("WBINVD: (ignoring)\n");

#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      bx_printf("WBINVD: CPL!=0\n");
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
  bx_panic("MOV_DdRd: not supported on < 386\n");
#else
  Bit32u val_32;

  if (v8086_mode()) bx_panic("MOV_DdRd: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (i->mod != 0xc0) {
    bx_panic("MOV_DdRd(): rm field not a register!\n");
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    bx_panic("MOV_DdRd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  val_32 = BX_READ_32BIT_REG(i->rm);
  if (bx_dbg.dreg)
    bx_printf("MOV_DdRd: DR[%u]=%08xh unhandled\n",
      (unsigned) i->nnn, (unsigned) val_32);

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
        bx_printf("MOV_DdRd: access to DR4 causes #UD\n");
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
        bx_printf("MOV_DdRd: access to DR5 causes #UD\n");
        UndefinedOpcode(i);
        }
#endif
      // Some sanity checks...
      if ( val_32 & 0x00002000 ) {
        bx_panic("MOV_DdRd: GD bit not supported yet\n");
        // Note: processor clears GD upon entering debug exception
        // handler, to allow access to the debug registers
        }
      if ( (((val_32>>16) & 3)==2) ||
           (((val_32>>20) & 3)==2) ||
           (((val_32>>24) & 3)==2) ||
           (((val_32>>28) & 3)==2) ) {
        // IO breakpoints (10b) are not yet supported.
        bx_panic("MOV_DdRd: write of %08x contains IO breakpoint\n",
          val_32);
        }
      if ( (((val_32>>18) & 3)==2) ||
           (((val_32>>22) & 3)==2) ||
           (((val_32>>26) & 3)==2) ||
           (((val_32>>30) & 3)==2) ) {
        // LEN0..3 contains undefined length specifier (10b)
        bx_panic("MOV_DdRd: write of %08x contains undefined LENx\n",
          val_32);
        }
      if ( ((((val_32>>16) & 3)==0) && (((val_32>>18) & 3)!=0)) ||
           ((((val_32>>20) & 3)==0) && (((val_32>>22) & 3)!=0)) ||
           ((((val_32>>24) & 3)==0) && (((val_32>>26) & 3)!=0)) ||
           ((((val_32>>28) & 3)==0) && (((val_32>>30) & 3)!=0)) ) {
        // Instruction breakpoint with LENx not 00b (1-byte length)
        bx_panic("MOV_DdRd: write of %08x, R/W=00b LEN!=00b\n",
          val_32);
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
      bx_panic("MOV_DdRd: control register index out of range\n");
      break;
    }
#endif
}

  void
BX_CPU_C::MOV_RdDd(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOV_RdDd: not supported on < 386\n");
#else
  Bit32u val_32;

  if (v8086_mode()) {
    bx_printf("MOV_RdDd: v8086 mode causes #GP\n");
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  if (i->mod != 0xc0) {
    bx_panic("MOV_RdDd(): rm field not a register!\n");
    UndefinedOpcode(i);
    }

  if (protected_mode() && (CPL!=0)) {
    bx_printf("MOV_RdDd: CPL!=0 causes #GP\n");
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if (bx_dbg.dreg)
    bx_printf("MOV_RdDd: DR%u not implemented yet\n", i->nnn);

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
        bx_printf("MOV_RdDd: access to DR4 causes #UD\n");
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
        bx_printf("MOV_RdDd: access to DR5 causes #UD\n");
        UndefinedOpcode(i);
        }
#endif
      val_32 = BX_CPU_THIS_PTR dr7;
      break;

    default:
      bx_panic("MOV_RdDd: control register index out of range\n");
      val_32 = 0;
    }
  BX_WRITE_32BIT_REG(i->rm, val_32);
#endif
}


  void
BX_CPU_C::LMSW_Ew(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("LMSW_Ew(): not supported on 8086!\n");
#else
  Bit16u msw;
  Bit32u cr0;

  if (v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  if ( protected_mode() ) {
    if ( CPL != 0 ) {
      bx_printf("LMSW: CPL != 0, CPL=%u\n", (unsigned) CPL);
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
  bx_panic("SMSW_Ew: not supported yet!\n");
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
  bx_panic("MOV_CdRd: not supported on < 386\n");
#else
  Bit32u val_32;


  if (v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (i->mod != 0xc0) {
    bx_panic("MOV_CdRd(): rm field not a register!\n");
    }

  invalidate_prefetch_q();

  if (protected_mode() && CPL!=0) {
    bx_panic("MOV_CdRd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_32 = BX_READ_32BIT_REG(i->rm);

  switch (i->nnn) {
    case 0: // CR0 (MSW)
      // bx_printf("MOV_CdRd:CR0: R32 = %08x\n @CS:EIP %04x:%04x ",
      //   (unsigned) val_32,
      //   (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
      //   (unsigned) BX_CPU_THIS_PTR eip);
      SetCR0(val_32);
      break;

    case 1: /* CR1 */
      bx_panic("MOV_CdRd: CR1 not implemented yet\n");
      break;
    case 2: /* CR2 */
      bx_printf("MOV_CdRd: CR2 not implemented yet\n");
      if (bx_dbg.creg)
        bx_printf("MOV_CdRd: CR2 = reg\n");
      BX_CPU_THIS_PTR cr2 = val_32;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        bx_printf("MOV_CdRd:CR3 = %08x\n", (unsigned) val_32);
      // Reserved bits take on value of MOV instruction
      CR3_change(val_32);
      BX_INSTR_TLB_CNTRL(BX_INSTR_MOV_CR3, val_32);
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      bx_panic("MOV_CdRd: write to CR4 of 0x%08x on 386\n",
        val_32);
      UndefinedOpcode(i);
#else
      //  Protected mode: #GP(0) if attempt to write a 1 to
      //  any reserved bit of CR4

      bx_printf("MOV_CdRd: ignoring write to CR4 of 0x%08x\n",
        val_32);
      if (val_32) {
        bx_panic("MOV_CdRd: (CR4) write of 0x%08x\n",
          val_32);
        }
      // Only allow writes of 0 to CR4 for now.
      // Writes to bits in CR4 should not be 1s as CPUID
      // returns not-supported for all of these features.
      BX_CPU_THIS_PTR cr4 = 0;
#endif
      break;
    default:
      bx_panic("MOV_CdRd: control register index out of range\n");
      break;
    }
#endif
}

  void
BX_CPU_C::MOV_RdCd(BxInstruction_t *i)
{
  // mov control register data to register
#if BX_CPU_LEVEL < 3
  bx_panic("MOV_RdCd: not supported on < 386\n");
#else
  Bit32u val_32;

  if (v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  if (i->mod != 0xc0) {
    bx_panic("MOV_RdCd(): rm field not a register!\n");
    }

  if (protected_mode() && CPL!=0) {
    bx_panic("MOV_RdCd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (i->nnn) {
    case 0: // CR0 (MSW)
      val_32 = BX_CPU_THIS_PTR cr0.val32;
#if 0
      bx_printf("MOV_RdCd:CR0: R32 = %08x\n @CS:EIP %04x:%04x\n",
        (unsigned) val_32,
        (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
        (unsigned) BX_CPU_THIS_PTR eip);
#endif
      break;
    case 1: /* CR1 */
      bx_panic("MOV_RdCd: CR1 not implemented yet\n");
      val_32 = 0;
      break;
    case 2: /* CR2 */
      if (bx_dbg.creg)
        bx_printf("MOV_RdCd: CR2\n");
      val_32 = BX_CPU_THIS_PTR cr2;
      break;
    case 3: // CR3
      if (bx_dbg.creg)
        bx_printf("MOV_RdCd: reading CR3\n");
      val_32 = BX_CPU_THIS_PTR cr3;
      break;
    case 4: // CR4
#if BX_CPU_LEVEL == 3
      val_32 = 0;
      bx_printf("MOV_RdCd: read of CR4 causes #UD\n");
      UndefinedOpcode(i);
#else
      bx_printf("MOV_RdCd: read of CR4\n");
      val_32 = BX_CPU_THIS_PTR cr4;
#endif
      break;
    default:
      bx_panic("MOV_RdCd: control register index out of range\n");
      val_32 = 0;
    }
  BX_WRITE_32BIT_REG(i->rm, val_32);
#endif
}

  void
BX_CPU_C::MOV_TdRd(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOV_TdRd:\n");
#elif BX_CPU_LEVEL <= 4
  bx_panic("MOV_TdRd:\n");
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  bx_printf("MOV_TdRd: causes #UD\n");
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::MOV_RdTd(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOV_RdTd:\n");
#elif BX_CPU_LEVEL <= 4
  bx_panic("MOV_RdTd:\n");
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  bx_printf("MOV_RdTd: causes #UD\n");
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::LOADALL(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("undocumented LOADALL instruction not supported on 8086\n");
#else
  Bit16u msw, tr, flags, ip, ldtr;
  Bit16u ds_raw, ss_raw, cs_raw, es_raw;
  Bit16u di, si, bp, sp, bx, dx, cx, ax;
  Bit16u base_15_0, limit;
  Bit8u  base_23_16, access;

  if (v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

#if BX_CPU_LEVEL > 2
  bx_panic("loadall: not implemented for 386\n");
  /* ??? need to set G and other bits, and compute .limit_scaled also */
  /* for all segments CS,DS,SS,... */
#endif

  if (BX_CPU_THIS_PTR cr0.pe) {
    bx_panic(
      "LOADALL not yet supported for protected mode\n");
    }

bx_panic("LOADALL: handle CR0.val32\n");
  /* MSW */
  BX_MEM.read_physical(0x806, 2, &msw);
  BX_CPU_THIS_PTR cr0.pe = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.mp = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.em = (msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.ts = (msw & 0x01);

  //bx_printf("LOADALL: pe=%u, mp=%u, em=%u, ts=%u\n",
  //  (unsigned) BX_CPU_THIS_PTR cr0.pe, (unsigned) BX_CPU_THIS_PTR cr0.mp,
  //  (unsigned) BX_CPU_THIS_PTR cr0.em, (unsigned) BX_CPU_THIS_PTR cr0.ts);

  if (BX_CPU_THIS_PTR cr0.pe || BX_CPU_THIS_PTR cr0.mp || BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts)
    bx_panic("LOADALL set PE, MP, EM or TS bits in MSW!\n");

  /* TR */
  BX_MEM.read_physical(0x816, 2, &tr);
  BX_CPU_THIS_PTR tr.selector.value = tr;
  BX_CPU_THIS_PTR tr.selector.rpl   = (tr & 0x03);  tr >>= 2;
  BX_CPU_THIS_PTR tr.selector.ti    = (tr & 0x01);  tr >>= 1;
  BX_CPU_THIS_PTR tr.selector.index = tr;
  BX_MEM.read_physical(0x860, 2, &base_15_0);
  BX_MEM.read_physical(0x862, 1, &base_23_16);
  BX_MEM.read_physical(0x863, 1, &access);
  BX_MEM.read_physical(0x864, 2, &limit);


  BX_CPU_THIS_PTR tr.cache.valid =
  BX_CPU_THIS_PTR tr.cache.p           = (access & 0x80) >> 7;
  BX_CPU_THIS_PTR tr.cache.dpl         = (access & 0x60) >> 5;
  BX_CPU_THIS_PTR tr.cache.segment     = (access & 0x10) >> 4;
  BX_CPU_THIS_PTR tr.cache.type        = (access & 0x0f);
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
  BX_MEM.read_physical(0x818, 2, &flags);
  write_flags(flags, 1, 1);

  /* IP */
  BX_MEM.read_physical(0x81a, 2, &ip);
  IP = ip;

  /* LDTR */
  BX_MEM.read_physical(0x81c, 2, &ldtr);
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
    BX_MEM.read_physical(0x854, 2, &base_15_0);
    BX_MEM.read_physical(0x856, 1, &base_23_16);
    BX_MEM.read_physical(0x857, 1, &access);
    BX_MEM.read_physical(0x858, 2, &limit);
    BX_CPU_THIS_PTR ldtr.cache.valid      =
    BX_CPU_THIS_PTR ldtr.cache.p          = access >> 7;
    BX_CPU_THIS_PTR ldtr.cache.dpl        = (access >> 5) & 0x03;
    BX_CPU_THIS_PTR ldtr.cache.segment    = (access >> 4) & 0x01;
    BX_CPU_THIS_PTR ldtr.cache.type       = (access & 0x0f);
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.base = (base_23_16 << 16) | base_15_0;
    BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit = limit;

    if (access == 0) {
      bx_panic("loadall: LDTR case access byte=0.\n");
      }
    if ( BX_CPU_THIS_PTR ldtr.cache.valid==0 ) {
      bx_panic("loadall: ldtr.valid=0\n");
      }
    if (BX_CPU_THIS_PTR ldtr.cache.segment) { /* not a system segment */
      bx_printf("         AR byte = %02x\n", (unsigned) access);
      bx_panic("loadall: LDTR descriptor cache loaded with non system segment\n");
      }
    if ( BX_CPU_THIS_PTR ldtr.cache.type != 2 ) {
      bx_panic("loadall: LDTR.type(%u) != 2\n", (unsigned) (access & 0x0f));
      }
    }

  /* DS */
  BX_MEM.read_physical(0x81e, 2, &ds_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = ds_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl   = (ds_raw & 0x03);  ds_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti    = (ds_raw & 0x01);  ds_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index = ds_raw;
  BX_MEM.read_physical(0x848, 2, &base_15_0);
  BX_MEM.read_physical(0x84a, 1, &base_23_16);
  BX_MEM.read_physical(0x84b, 1, &access);
  BX_MEM.read_physical(0x84c, 2, &limit);
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
    bx_panic("loadall: DS invalid\n");
    }

  /* SS */
  BX_MEM.read_physical(0x820, 2, &ss_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = ss_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = (ss_raw & 0x03); ss_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (ss_raw & 0x01); ss_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = ss_raw;
  BX_MEM.read_physical(0x842, 2, &base_15_0);
  BX_MEM.read_physical(0x844, 1, &base_23_16);
  BX_MEM.read_physical(0x845, 1, &access);
  BX_MEM.read_physical(0x846, 2, &limit);
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
    bx_panic("loadall: SS invalid\n");
    }


  /* CS */
  BX_MEM.read_physical(0x822, 2, &cs_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = cs_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = (cs_raw & 0x03); cs_raw >>= 2;

  //bx_printf("LOADALL: setting cs.selector.rpl to %u\n",
  //  (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (cs_raw & 0x01); cs_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = cs_raw;
  BX_MEM.read_physical(0x83c, 2, &base_15_0);
  BX_MEM.read_physical(0x83e, 1, &base_23_16);
  BX_MEM.read_physical(0x83f, 1, &access);
  BX_MEM.read_physical(0x840, 2, &limit);
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
    bx_panic("loadall: CS invalid\n");
    }

  /* ES */
  BX_MEM.read_physical(0x824, 2, &es_raw);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = es_raw;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl   = (es_raw & 0x03); es_raw >>= 2;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti    = (es_raw & 0x01); es_raw >>= 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index = es_raw;
  BX_MEM.read_physical(0x836, 2, &base_15_0);
  BX_MEM.read_physical(0x838, 1, &base_23_16);
  BX_MEM.read_physical(0x839, 1, &access);
  BX_MEM.read_physical(0x83a, 2, &limit);
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
    bx_printf("cs.dpl = %02x\n", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl);
    bx_printf("ss.dpl = %02x\n", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl);
    bx_printf("BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].dpl = %02x\n", (unsigned) BX_CPU_THIS_PTR ds.cache.dpl);
    bx_printf("BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].dpl = %02x\n", (unsigned) BX_CPU_THIS_PTR es.cache.dpl);
    bx_printf("LOADALL: setting cs.selector.rpl to %u\n",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl);
    bx_printf("LOADALL: setting ss.selector.rpl to %u\n",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl);
    bx_printf("LOADALL: setting ds.selector.rpl to %u\n",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl);
    bx_printf("LOADALL: setting es.selector.rpl to %u\n",
      (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl);
#endif

  if ( (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value & 0xfffc) == 0 ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
    }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid==0  ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment==0) {
    bx_panic("loadall: ES invalid\n");
    }

  /* DI */
  BX_MEM.read_physical(0x826, 2, &di);
  DI = di;

  /* SI */
  BX_MEM.read_physical(0x828, 2, &si);
  SI = si;

  /* BP */
  BX_MEM.read_physical(0x82a, 2, &bp);
  BP = bp;

  /* SP */
  BX_MEM.read_physical(0x82c, 2, &sp);
  SP = sp;

  /* BX */
  BX_MEM.read_physical(0x82e, 2, &bx);
  BX = bx;

  /* DX */
  BX_MEM.read_physical(0x830, 2, &dx);
  DX = dx;

  /* CX */
  BX_MEM.read_physical(0x832, 2, &cx);
  CX = cx;

  /* AX */
  BX_MEM.read_physical(0x834, 2, &ax);
  AX = ax;

  /* GDTR */
  BX_MEM.read_physical(0x84e, 2, &base_15_0);
  BX_MEM.read_physical(0x850, 1, &base_23_16);
  BX_MEM.read_physical(0x851, 1, &access);
  BX_MEM.read_physical(0x852, 2, &limit);
  BX_CPU_THIS_PTR gdtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR gdtr.limit = limit;

#if 0
  if (access)
      bx_printf("LOADALL: GDTR access bits not 0 (%02x).\n",
        (unsigned) access);
#endif

  /* IDTR */
  BX_MEM.read_physical(0x85a, 2, &base_15_0);
  BX_MEM.read_physical(0x85c, 1, &base_23_16);
  BX_MEM.read_physical(0x85d, 1, &access);
  BX_MEM.read_physical(0x85e, 2, &limit);
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

#else
      family = 6;
      bx_panic("CPUID: not implemented for > 5\n");
#endif

      EAX = (family <<8) | (model<<4) | stepping;
      EBX = ECX = 0; // reserved
      EDX = features;
      break;

    default:
      EAX = EBX = ECX = EDX = 0; // Reserved, undefined
      break;
    }
#else
  bx_panic("CPUID: not available on < late 486\n");
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
  BX_CPU_THIS_PTR cr0.val32 = val_32 & 0xe005002f;
#elif BX_CPU_LEVEL == 5
  BX_CPU_THIS_PTR cr0.val32 = val_32 | 0x00000010;
#elif BX_CPU_LEVEL == 6
  BX_CPU_THIS_PTR cr0.val32 = (val_32 | 0x00000010) & 0xe005003f;
#else
#error "MOV_CdRd: implement reserved bits behaviour for this CPU_LEVEL"
#endif

  //if (BX_CPU_THIS_PTR cr0.ts)
  //  bx_printf("MOV_CdRd:CR0.TS set 0x%x\n", (unsigned) val_32);

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

  bx_panic("RSM: System Management Mode not implemented yet\n");
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::RDTSC(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 5
  Boolean tsd = BX_CPU_THIS_PTR cr4 & 4;
  Boolean cpl = CPL;
  if ((tsd==0) || (tsd==1 && cpl==0)) {
    // return ticks
    Bit64u ticks = bx_pc_system.time_ticks ();
    EAX = (Bit32u) (ticks & 0xffffffff);
    EDX = (Bit32u) ((ticks >> 32) & 0xffffffff);
    bx_printf ("RDTSC: returning EDX:EAX = %08x:%08x\n", EDX, EAX);
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
  bx_panic("RDMSR: not implemented yet\n");
#else
  UndefinedOpcode(i);
#endif
}

  void
BX_CPU_C::WRMSR(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 5
  invalidate_prefetch_q();

  bx_panic("WRMSR: not implemented yet\n");
#else
  UndefinedOpcode(i);
#endif
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
