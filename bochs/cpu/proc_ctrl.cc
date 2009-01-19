/////////////////////////////////////////////////////////////////////////
// $Id: proc_ctrl.cc,v 1.274 2009-01-19 17:43:54 sshwarts Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RCX ECX
#define RDX EDX
#define RIP EIP
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UndefinedOpcode(bxInstruction_c *i)
{
  BX_DEBUG(("UndefinedOpcode: b1 = 0x%02x causes #UD exception", i->b1()));
  exception(BX_UD_EXCEPTION, 0, 0);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOP(bxInstruction_c *i)
{
  // No operation.
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PREFETCH(bxInstruction_c *i)
{
#if BX_INSTRUMENTATION
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  BX_INSTR_PREFETCH_HINT(BX_CPU_ID, i->nnn(), i->seg(), eaddr);
#endif
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
  BX_PANIC(("Entering to shutdown state still not implemented"));

  BX_CPU_THIS_PTR clear_IF();

  // artificial trap bit, why use another variable.
  BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_TRAP_SHUTDOWN; // artificial trap
  BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
  // Execution of this instruction completes.  The processor
  // will remain in a halt state until one of the above conditions
  // is met.

  BX_INSTR_HLT(BX_CPU_ID);

#if BX_DEBUGGER
  bx_dbg_halt(BX_CPU_ID);
#endif

#if BX_USE_IDLE_HACK
  bx_gui->sim_is_idle();
#endif

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::HLT(bxInstruction_c *i)
{
  if (!real_mode() && CPL!=0) {
    BX_DEBUG(("HLT: %s priveledge check failed, CPL=%d, generate #GP(0)",
        cpu_mode_string(BX_CPU_THIS_PTR cpu_mode), CPL));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  if (! BX_CPU_THIS_PTR get_IF()) {
    BX_INFO(("WARNING: HLT instruction with IF=0!"));
  }

  // stops instruction execution and places the processor in a
  // HALT state.  An enabled interrupt, NMI, or reset will resume
  // execution.  If interrupt (including NMI) is used to resume
  // execution after HLT, the saved CS:eIP points to instruction
  // following HLT.

  // artificial trap bit, why use another variable.
  BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_TRAP_HALT; // artificial trap
  BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
  // Execution of this instruction completes.  The processor
  // will remain in a halt state until one of the above conditions
  // is met.

  BX_INSTR_HLT(BX_CPU_ID);

#if BX_DEBUGGER
  bx_dbg_halt(BX_CPU_ID);
#endif

#if BX_USE_IDLE_HACK
  bx_gui->sim_is_idle();
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLTS(bxInstruction_c *i)
{
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("CLTS: priveledge check failed, generate #GP(0)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_CPU_THIS_PTR cr0.set_TS(0);
}

/* 0F 08   INVD */
/* 0F 09 WBINVD */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::WBINVD(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("INVD/WBINVD: priveledge check failed, generate #GP(0)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  invalidate_prefetch_q();

  BX_DEBUG(("INVD/WBINVD: Flush internal caches !"));
#if BX_INSTRUMENTATION
  if (i->b1() == 0x08)
    BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_INVD);
  else
    BX_INSTR_CACHE_CNTRL(BX_CPU_ID, BX_INSTR_WBINVD);
#endif

#if BX_SUPPORT_ICACHE
  flushICaches();
#endif

#else
  BX_INFO(("INVD/WBINVD: required 486 support, use --enable-cpu-level=4 option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLFLUSH(bxInstruction_c *i)
{
#if BX_SUPPORT_CLFLUSH
  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[i->seg()];

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = BX_CPU_THIS_PTR get_laddr(i->seg(), eaddr);

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (! IsCanonical(laddr)) {
      BX_ERROR(("CLFLUSH: non-canonical access !"));
      exception(int_number(i->seg()), 0, 0);
    }
  }
  else
#endif
  {
    // check if we could access the memory segment
    if (!(seg->cache.valid & SegAccessROK)) {
      if (! execute_virtual_checks(seg, eaddr, 1))
        exception(int_number(i->seg()), 0, 0);
    }
    else {
      if (eaddr > seg->cache.u.segment.limit_scaled) {
        BX_ERROR(("CLFLUSH: segment limit violation"));
        exception(int_number(i->seg()), 0, 0);
      }
    }
  }

  bx_phy_address paddr;

  if (BX_CPU_THIS_PTR cr0.get_PG()) {
    paddr = dtranslate_linear(laddr, CPL, BX_READ);
    paddr = A20ADDR(paddr);
  }
  else
  {
    paddr = A20ADDR(laddr);
  }

  BX_INSTR_CLFLUSH(BX_CPU_ID, laddr, paddr);

#else
  BX_INFO(("CLFLUSH: not supported, enable with SSE2"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_CPU_LEVEL >= 3
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_DdRd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR cr4.get_DE()) {
    if ((i->nnn() & 0xE) == 4) {
      BX_ERROR(("MOV_DdRd: access to DR4/DR5 causes #UD"));
      exception(BX_UD_EXCEPTION, 0, 0);
    }
  }
#endif

  // Note: processor clears GD upon entering debug exception
  // handler, to allow access to the debug registers
  if (BX_CPU_THIS_PTR dr7 & 0x2000) { // GD bit set
    BX_ERROR(("MOV_DdRd: DR7 GD bit is set"));
    BX_CPU_THIS_PTR dr6 |= 0x2000;
    exception(BX_DB_EXCEPTION, 0, 0);
  }

  if (!real_mode() && CPL!=0) {
    BX_ERROR(("MOV_DdRd: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  invalidate_prefetch_q();

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_DdRd(): rm field not a register!"));

  Bit32u val_32 = BX_READ_32BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // DR0
    case 1: // DR1
    case 2: // DR2
    case 3: // DR3
      TLB_invlpg(val_32);
      BX_CPU_THIS_PTR dr[i->nnn()] = val_32;
      break;

    case 4: // DR4
      // DR4 aliased to DR6 by default. With Debug Extensions on,
      // access to DR4 causes #UD
    case 6: // DR6
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
      // DR5 aliased to DR7 by default. With Debug Extensions on,
      // access to DR5 causes #UD
    case 7: // DR7
      // Note: 486+ ignore GE and LE flags.  On the 386, exact
      // data breakpoint matching does not occur unless it is enabled
      // by setting the LE and/or GE flags.

      // Some sanity checks...
      if (((((val_32>>16) & 3)==0) && (((val_32>>18) & 3)!=0)) ||
          ((((val_32>>20) & 3)==0) && (((val_32>>22) & 3)!=0)) ||
          ((((val_32>>24) & 3)==0) && (((val_32>>26) & 3)!=0)) ||
          ((((val_32>>28) & 3)==0) && (((val_32>>30) & 3)!=0)))
      {
        // Instruction breakpoint with LENx not 00b (1-byte length)
        BX_ERROR(("MOV_DdRd: write of %08x, R/W=00b LEN!=00b", val_32));
      }
#if BX_CPU_LEVEL <= 4
      // 386/486: you can play with all the bits except b10 is always 1
      BX_CPU_THIS_PTR dr7 = val_32 | 0x00000400;
#else
      // Pentium+: bits15,14,12 are hardwired to 0, rest are settable.
      // Even bits 11,10 are changeable though reserved.
      BX_CPU_THIS_PTR dr7 = (val_32 & 0xffff2fff) | 0x00000400;
#endif
      // if we have breakpoints enabled then we must check
      // breakpoints condition in cpu loop
      if(BX_CPU_THIS_PTR dr7 & 0xff)
        BX_CPU_THIS_PTR async_event = 1;
      break;

    default:
      BX_ERROR(("MOV_DdRd: #UD - register index out of range"));
      exception(BX_UD_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdDd(bxInstruction_c *i)
{
  Bit32u val_32;

#if BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR cr4.get_DE()) {
    if ((i->nnn() & 0xE) == 4) {
      BX_ERROR(("MOV_RdDd: access to DR4/DR5 causes #UD"));
      exception(BX_UD_EXCEPTION, 0, 0);
    }
  }
#endif

  // Note: processor clears GD upon entering debug exception
  // handler, to allow access to the debug registers
  if (BX_CPU_THIS_PTR dr7 & 0x2000) { // GD bit set
    BX_ERROR(("MOV_RdDd: DR7 GD bit is set"));
    BX_CPU_THIS_PTR dr6 |= 0x2000;
    exception(BX_DB_EXCEPTION, 0, 0);
  }

  if (!real_mode() && CPL!=0) {
    BX_ERROR(("MOV_RdDd: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_RdDd(): rm field not a register!"));

  switch (i->nnn()) {
    case 0: // DR0
    case 1: // DR1
    case 2: // DR2
    case 3: // DR3
      val_32 = BX_CPU_THIS_PTR dr[i->nnn()];
      break;

    case 4: // DR4
      // DR4 aliased to DR6 by default. With Debug Extensions ON,
      // access to DR4 causes #UD
    case 6: // DR6
      val_32 = BX_CPU_THIS_PTR dr6;
      break;

    case 5: // DR5
      // DR5 aliased to DR7 by default. With Debug Extensions ON,
      // access to DR5 causes #UD
    case 7: // DR7
      val_32 = BX_CPU_THIS_PTR dr7;
      break;

    default:
      BX_ERROR(("MOV_RdDd: #UD - register index out of range"));
      exception(BX_UD_EXCEPTION, 0, 0);
  }

  BX_WRITE_32BIT_REGZ(i->rm(), val_32);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_DqRq(bxInstruction_c *i)
{
  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  if (BX_CPU_THIS_PTR cr4.get_DE()) {
    if ((i->nnn() & 0xE) == 4) {
      BX_ERROR(("MOV_DqRq: access to DR4/DR5 causes #UD"));
      exception(BX_UD_EXCEPTION, 0, 0);
    }
  }

  // Note: processor clears GD upon entering debug exception
  // handler, to allow access to the debug registers
  if (BX_CPU_THIS_PTR dr7 & 0x2000) { // GD bit set
    BX_ERROR(("MOV_DqRq: DR7 GD bit is set"));
    BX_CPU_THIS_PTR dr6 |= 0x2000;
    exception(BX_DB_EXCEPTION, 0, 0);
  }

  /* #GP(0) if CPL is not 0 */
  if (CPL != 0) {
    BX_ERROR(("MOV_DqRq: #GP(0) if CPL is not 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  invalidate_prefetch_q();

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_DqRq(): rm field not a register!"));

  Bit64u val_64 = BX_READ_64BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // DR0
    case 1: // DR1
    case 2: // DR2
    case 3: // DR3
      TLB_invlpg(val_64);
      BX_CPU_THIS_PTR dr[i->nnn()] = val_64;
      break;

    case 4: // DR4
      // DR4 aliased to DR6 by default. With Debug Extensions ON,
      // access to DR4 causes #UD
    case 6: // DR6
      // On Pentium+, bit12 is always zero
      BX_CPU_THIS_PTR dr6 = (BX_CPU_THIS_PTR dr6 & 0xffff0ff0) |
                            (val_64 & 0x0000e00f);
      break;

    case 5: // DR5
      // DR5 aliased to DR7 by default. With Debug Extensions ON,
      // access to DR5 causes #UD
    case 7: // DR7
      // Note: 486+ ignore GE and LE flags.  On the 386, exact
      // data breakpoint matching does not occur unless it is enabled
      // by setting the LE and/or GE flags.

      // Some sanity checks...
      if (((((val_64>>16) & 3)==0) && (((val_64>>18) & 3)!=0)) ||
          ((((val_64>>20) & 3)==0) && (((val_64>>22) & 3)!=0)) ||
          ((((val_64>>24) & 3)==0) && (((val_64>>26) & 3)!=0)) ||
          ((((val_64>>28) & 3)==0) && (((val_64>>30) & 3)!=0)))
      {
        // Instruction breakpoint with LENx not 00b (1-byte length)
        BX_ERROR(("MOV_DqRq: write of %08x:%08x, R/W=00b LEN!=00b",
          (Bit32u)(val_64 >> 32), (Bit32u)(val_64 & 0xFFFFFFFF)));
      }

      // Pentium+: bits15,14,12 are hardwired to 0, rest are settable.
      // Even bits 11,10 are changeable though reserved.
      BX_CPU_THIS_PTR dr7 = (val_64 & 0xffff2fff) | 0x00000400;
      break;

    default:
      BX_ERROR(("MOV_DqRq: #UD - register index out of range"));
      exception(BX_UD_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RqDq(bxInstruction_c *i)
{
  Bit64u val_64;

  if (BX_CPU_THIS_PTR cr4.get_DE()) {
    if ((i->nnn() & 0xE) == 4) {
      BX_ERROR(("MOV_RqDq: access to DR4/DR5 causes #UD"));
      exception(BX_UD_EXCEPTION, 0, 0);
    }
  }

  // Note: processor clears GD upon entering debug exception
  // handler, to allow access to the debug registers
  if (BX_CPU_THIS_PTR dr7 & 0x2000) { // GD bit set
    BX_ERROR(("MOV_RqDq: DR7 GD bit is set"));
    BX_CPU_THIS_PTR dr6 |= 0x2000;
    exception(BX_DB_EXCEPTION, 0, 0);
  }

  /* #GP(0) if CPL is not 0 */
  if (CPL != 0) {
    BX_ERROR(("MOV_RqDq: #GP(0) if CPL is not 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_RqDq(): rm field not a register!"));

  switch (i->nnn()) {
    case 0: // DR0
    case 1: // DR1
    case 2: // DR2
    case 3: // DR3
      val_64 = BX_CPU_THIS_PTR dr[i->nnn()];
      break;

    case 4: // DR4
      // DR4 aliased to DR6 by default. With Debug Extensions ON,
      // access to DR4 causes #UD
    case 6: // DR6
      val_64 = BX_CPU_THIS_PTR dr6;
      break;

    case 5: // DR5
      // DR5 aliased to DR7 by default. With Debug Extensions ON,
      // access to DR5 causes #UD
    case 7: // DR7
      val_64 = BX_CPU_THIS_PTR dr7;
      break;

    default:
      BX_ERROR(("MOV_RqDq: #UD - register index out of range"));
      exception(BX_UD_EXCEPTION, 0, 0);
  }

  BX_WRITE_64BIT_REG(i->rm(), val_64);
}
#endif // #if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_CdRd(bxInstruction_c *i)
{
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("MOV_CdRd: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_CdRd(): rm field not a register!"));

  Bit32u val_32 = BX_READ_32BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      if (! SetCR0(val_32))
        exception(BX_GP_EXCEPTION, 0, 0);
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_CdRd:CR2 = %08x", val_32));
      BX_CPU_THIS_PTR cr2 = val_32;
      break;
    case 3: // CR3
      BX_DEBUG(("MOV_CdRd:CR3 = %08x", val_32));
      // Reserved bits take on value of MOV instruction
      SetCR3(val_32);
      BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_MOV_CR3, val_32);
      break;
#if BX_CPU_LEVEL > 3
    case 4: // CR4
      // Protected mode: #GP(0) if attempt to write a 1 to
      // any reserved bit of CR4
      if (! SetCR4(val_32))
        exception(BX_GP_EXCEPTION, 0, 0);
      break;
#endif
    default:
      BX_ERROR(("MOV_CdRd: #UD - control register %d index out of range", i->nnn()));
      exception(BX_UD_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdCd(bxInstruction_c *i)
{
  // mov control register data to register
  Bit32u val_32 = 0;

  if (!real_mode() && CPL!=0) {
    BX_ERROR(("MOV_RdCd: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_RdCd(): rm field not a register!"));

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      val_32 = BX_CPU_THIS_PTR cr0.get32();
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_RdCd: reading CR2"));
      val_32 = (Bit32u) BX_CPU_THIS_PTR cr2;
      break;
    case 3: // CR3
      BX_DEBUG(("MOV_RdCd: reading CR3"));
      val_32 = (Bit32u) BX_CPU_THIS_PTR cr3;
      break;
    case 4: // CR4
#if BX_CPU_LEVEL > 3
      BX_DEBUG(("MOV_RdCd: read of CR4"));
      val_32 = BX_CPU_THIS_PTR cr4.get32();
#endif
      break;
    default:
      BX_ERROR(("MOV_RdCd: #UD - control register %d index out of range", i->nnn()));
      exception(BX_UD_EXCEPTION, 0, 0);
  }

  BX_WRITE_32BIT_REGZ(i->rm(), val_32);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_CqRq(bxInstruction_c *i)
{
  BX_ASSERT(protected_mode());

  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  /* #GP(0) if CPL is not 0 */
  if (CPL!=0) {
    BX_ERROR(("MOV_CqRq: #GP(0) if CPL is not 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_CqRq(): rm field not a register!"));

  Bit64u val_64 = BX_READ_64BIT_REG(i->rm());

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      if (! SetCR0((Bit32u) val_64))
        exception(BX_GP_EXCEPTION, 0, 0);
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_CqRq: write to CR2 of %08x:%08x", GET32H(val_64), GET32L(val_64)));
      BX_CPU_THIS_PTR cr2 = val_64;
      break;
    case 3: // CR3
      BX_DEBUG(("MOV_CqRq: write to CR3 of %08x:%08x", GET32H(val_64), GET32L(val_64)));
      // Reserved bits take on value of MOV instruction
      SetCR3(val_64);
      BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_MOV_CR3, val_64);
      break;
    case 4: // CR4
      // Protected mode: #GP(0) if attempt to write a 1 to
      // any reserved bit of CR4
      BX_DEBUG(("MOV_CqRq: write to CR4 of %08x:%08x", GET32H(val_64), GET32L(val_64)));
      if (! SetCR4(val_64))
        exception(BX_GP_EXCEPTION, 0, 0);
      break;
    case 8: // CR8
      // CR8 is aliased to APIC->TASK PRIORITY register
      //   APIC.TPR[7:4] = CR8[3:0]
      //   APIC.TPR[3:0] = 0
      // Reads of CR8 return zero extended APIC.TPR[7:4]
      // Write to CR8 update APIC.TPR[7:4]
#if BX_SUPPORT_APIC
      if (val_64 & BX_CONST64(0xfffffffffffffff0)) {
        BX_ERROR(("MOV_CqRq: Attempt to set reserved bits of CR8"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      BX_CPU_THIS_PTR local_apic.set_tpr((val_64 & 0xF) << 0x4);
      break;
#endif
    default:
      BX_ERROR(("MOV_CqRq: #UD - control register %d index out of range", i->nnn()));
      exception(BX_UD_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RqCq(bxInstruction_c *i)
{
  // mov control register data to register
  Bit64u val_64 = 0;

  BX_ASSERT(protected_mode());

  /* NOTES:
   *   64bit operands always used
   *   r/m field specifies general register
   *   reg field specifies which special register
   */

  /* #GP(0) if CPL is not 0 */
  if (CPL!=0) {
    BX_ERROR(("MOV_RqCq: #GP(0) if CPL is not 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* This instruction is always treated as a register-to-register,
   * regardless of the encoding of the MOD field in the MODRM byte.
   */
  if (!i->modC0())
    BX_PANIC(("MOV_RqCq(): rm field not a register!"));

  switch (i->nnn()) {
    case 0: // CR0 (MSW)
      val_64 = BX_CPU_THIS_PTR cr0.get32();
      break;
    case 2: /* CR2 */
      BX_DEBUG(("MOV_RqCq: read of CR2"));
      val_64 = BX_CPU_THIS_PTR cr2;
      break;
    case 3: // CR3
      BX_DEBUG(("MOV_RqCq: read of CR3"));
      val_64 = BX_CPU_THIS_PTR cr3;
      break;
    case 4: // CR4
      BX_DEBUG(("MOV_RqCq: read of CR4"));
      val_64 = BX_CPU_THIS_PTR cr4.get32();
      break;
    case 8: // CR8
      // CR8 is aliased to APIC->TASK PRIORITY register
      //   APIC.TPR[7:4] = CR8[3:0]
      //   APIC.TPR[3:0] = 0
      // Reads of CR8 return zero extended APIC.TPR[7:4]
      // Write to CR8 update APIC.TPR[7:4]
#if BX_SUPPORT_APIC
      val_64 = (BX_CPU_THIS_PTR local_apic.get_tpr() & 0xF) >> 4;
      break;
#endif
    default:
      BX_ERROR(("MOV_RqCq: #UD - control register %d index out of range", i->nnn()));
      exception(BX_UD_EXCEPTION, 0, 0);
  }

  BX_WRITE_64BIT_REG(i->rm(), val_64);
}
#endif // #if BX_SUPPORT_X86_64

#endif // #if BX_CPU_LEVEL >= 3

#if BX_CPU_LEVEL >= 2
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LMSW_Ew(bxInstruction_c *i)
{
  Bit16u msw;

  if (!real_mode() && CPL!=0) {
    BX_ERROR(("LMSW: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  if (i->modC0()) {
    msw = BX_READ_16BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    msw = read_virtual_word(i->seg(), eaddr);
  }

  // LMSW does not affect PG,CD,NW,AM,WP,NE,ET bits, and cannot clear PE

  // LMSW cannot clear PE
  if (BX_CPU_THIS_PTR cr0.get_PE())
    msw |= 0x0001; // adjust PE bit to current value of 1

  msw &= 0xf; // LMSW only affects last 4 flags
  Bit32u cr0 = (BX_CPU_THIS_PTR cr0.get32() & 0xfffffff0) | msw;
  if (! SetCR0(cr0))
    exception(BX_GP_EXCEPTION, 0, 0);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SMSW_EwR(bxInstruction_c *i)
{
  if (i->os32L()) {
    BX_WRITE_32BIT_REGZ(i->rm(), BX_CPU_THIS_PTR cr0.get32());
  }
  else {
    BX_WRITE_16BIT_REG(i->rm(), BX_CPU_THIS_PTR cr0.get32() & 0xffff);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SMSW_EwM(bxInstruction_c *i)
{
  Bit16u msw = BX_CPU_THIS_PTR cr0.get32() & 0xffff;
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_word(i->seg(), eaddr, msw);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_TdRd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL <= 4
  BX_PANIC(("MOV_TdRd: Still not implemented"));
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  BX_INFO(("MOV_TdRd: causes #UD"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdTd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL <= 4
  BX_PANIC(("MOV_RdTd: Still not implemented"));
#else
  // Pentium+ does not have TRx.  They were redesigned using the MSRs.
  BX_INFO(("MOV_RdTd: causes #UD"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_CPU_LEVEL == 2
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOADALL(bxInstruction_c *i)
{
  Bit16u msw, tr, flags, ip, ldtr;
  Bit16u ds_raw, ss_raw, cs_raw, es_raw;
  Bit16u base_15_0, limit;
  Bit8u  base_23_16, access;

  if (v8086_mode()) BX_PANIC(("proc_ctrl: LOADALL in v8086 mode unsupported"));

  if (BX_CPU_THIS_PTR cr0.get_PE())
  {
    BX_PANIC(("LOADALL not yet supported for protected mode"));
  }

  BX_PANIC(("LOADALL: handle CR0.val32"));
  /* MSW */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x806, 2, &msw);
  BX_CPU_THIS_PTR cr0.set_PE(msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.set_MP(msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.set_EM(msw & 0x01); msw >>= 1;
  BX_CPU_THIS_PTR cr0.set_TS(msw & 0x01);

  if (BX_CPU_THIS_PTR cr0.get_PE() || BX_CPU_THIS_PTR cr0.get_MP() || BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    BX_PANIC(("LOADALL set PE, MP, EM or TS bits in MSW!"));

  /* TR */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x816, 2, &tr);
  BX_CPU_THIS_PTR tr.selector.value = tr;
  BX_CPU_THIS_PTR tr.selector.rpl   = (tr & 0x03); tr >>= 2;
  BX_CPU_THIS_PTR tr.selector.ti    = (tr & 0x01); tr >>= 1;
  BX_CPU_THIS_PTR tr.selector.index = tr;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x860, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x862, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x863, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x864, 2, &limit);

  BX_CPU_THIS_PTR tr.cache.valid =
  BX_CPU_THIS_PTR tr.cache.p           = (access & 0x80) >> 7;
  BX_CPU_THIS_PTR tr.cache.dpl         = (access & 0x60) >> 5;
  BX_CPU_THIS_PTR tr.cache.segment     = (access & 0x10) >> 4;
  // don't allow busy bit in tr.cache.type, so bit 2 is masked away too.
  BX_CPU_THIS_PTR tr.cache.type        = (access & 0x0d);
  BX_CPU_THIS_PTR tr.cache.u.system.base  = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR tr.cache.u.system.limit = limit;

  if ((BX_CPU_THIS_PTR tr.selector.value & 0xfffc) == 0) {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
  }
  if (BX_CPU_THIS_PTR tr.cache.u.system.limit < 43 ||
      BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_286_TSS ||
      BX_CPU_THIS_PTR tr.cache.segment)
  {
    BX_CPU_THIS_PTR tr.cache.valid = 0;
  }
  if (BX_CPU_THIS_PTR tr.cache.valid==0)
  {
    BX_CPU_THIS_PTR tr.selector.value    = 0;
    BX_CPU_THIS_PTR tr.selector.index    = 0;
    BX_CPU_THIS_PTR tr.selector.ti       = 0;
    BX_CPU_THIS_PTR tr.selector.rpl      = 0;
    BX_CPU_THIS_PTR tr.cache.u.system.base  = 0;
    BX_CPU_THIS_PTR tr.cache.u.system.limit = 0;
    BX_CPU_THIS_PTR tr.cache.p           = 0;
  }

  /* FLAGS */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x818, 2, &flags);
  write_flags(flags, 1, 1);

  /* IP */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x81a, 2, &IP);

  /* LDTR */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x81c, 2, &ldtr);
  BX_CPU_THIS_PTR ldtr.selector.value = ldtr;
  BX_CPU_THIS_PTR ldtr.selector.rpl   = (ldtr & 0x03); ldtr >>= 2;
  BX_CPU_THIS_PTR ldtr.selector.ti    = (ldtr & 0x01); ldtr >>= 1;
  BX_CPU_THIS_PTR ldtr.selector.index = ldtr;
  if ((BX_CPU_THIS_PTR ldtr.selector.value & 0xfffc) == 0)
  {
    BX_CPU_THIS_PTR ldtr.cache.valid   = 0;
    BX_CPU_THIS_PTR ldtr.cache.p       = 0;
    BX_CPU_THIS_PTR ldtr.cache.segment = 0;
    BX_CPU_THIS_PTR ldtr.cache.type    = 0;
    BX_CPU_THIS_PTR ldtr.cache.u.system.base = 0;
    BX_CPU_THIS_PTR ldtr.cache.u.system.limit = 0;
    BX_CPU_THIS_PTR ldtr.selector.value = 0;
    BX_CPU_THIS_PTR ldtr.selector.index = 0;
    BX_CPU_THIS_PTR ldtr.selector.ti    = 0;
  }
  else {
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x854, 2, &base_15_0);
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x856, 1, &base_23_16);
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x857, 1, &access);
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x858, 2, &limit);
    BX_CPU_THIS_PTR ldtr.cache.valid      =
    BX_CPU_THIS_PTR ldtr.cache.p          = access >> 7;
    BX_CPU_THIS_PTR ldtr.cache.dpl        = (access >> 5) & 0x03;
    BX_CPU_THIS_PTR ldtr.cache.segment    = (access >> 4) & 0x01;
    BX_CPU_THIS_PTR ldtr.cache.type       = (access & 0x0f);
    BX_CPU_THIS_PTR ldtr.cache.u.system.base = (base_23_16 << 16) | base_15_0;
    BX_CPU_THIS_PTR ldtr.cache.u.system.limit = limit;

    if (access == 0) {
      BX_PANIC(("loadall: LDTR case access byte=0"));
    }
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_PANIC(("loadall: ldtr.valid=0"));
    }
    if (BX_CPU_THIS_PTR ldtr.cache.segment) { /* not a system segment */
      BX_INFO(("         AR byte = %02x", (unsigned) access));
      BX_PANIC(("loadall: LDTR descriptor cache loaded with non system segment"));
    }
    if (BX_CPU_THIS_PTR ldtr.cache.type != BX_SYS_SEGMENT_LDT) {
      BX_PANIC(("loadall: LDTR.type(%u) != LDT", (unsigned) (access & 0x0f)));
    }
  }

  /* DS */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x81e, 2, &ds_raw);
  parse_selector(ds_raw, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x848, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x84a, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x84b, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x84c, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit = limit;
  set_ar_byte(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache, access);

  if ((BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value & 0xfffc) == 0) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
  }
  else {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 1;
  }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid==0 ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment==0)
  {
    BX_PANIC(("loadall: DS invalid"));
  }

  /* SS */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x820, 2, &ss_raw);
  parse_selector(ss_raw, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x842, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x844, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x845, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x846, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit = limit;
  set_ar_byte(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, access);

  if ((BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value & 0xfffc) == 0) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 0;
  }
  else {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 1;
  }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid==0 ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment==0)
  {
    BX_PANIC(("loadall: SS invalid"));
  }

  /* CS */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x822, 2, &cs_raw);
  parse_selector(cs_raw, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x83c, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x83e, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x83f, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x840, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit = limit;
  set_ar_byte(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache, access);

  if ((BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value & 0xfffc) == 0) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid = 0;
  }
  else {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid = 1;
  }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid==0 ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment==0)
  {
    BX_PANIC(("loadall: CS invalid"));
  }

  updateFetchModeMask();
  handleCpuModeChange();

  /* ES */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x824, 2, &es_raw);
  parse_selector(es_raw, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x836, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x838, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x839, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x83a, 2, &limit);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit = limit;
  set_ar_byte(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache, access);

  if ((BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value & 0xfffc) == 0) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
  }
  else {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 1;
  }
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid==0 ||
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment==0)
  {
    BX_PANIC(("loadall: ES invalid"));
  }

#if 0
  BX_INFO(("cs.dpl = %02x", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl));
  BX_INFO(("ss.dpl = %02x", (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl));
  BX_INFO(("BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].dpl = 0x%02x", (unsigned) BX_CPU_THIS_PTR ds.cache.dpl));
  BX_INFO(("BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].dpl = 0x%02x", (unsigned) BX_CPU_THIS_PTR es.cache.dpl));
  BX_INFO(("LOADALL: setting cs.selector.rpl to %u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl));
  BX_INFO(("LOADALL: setting ss.selector.rpl to %u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl));
  BX_INFO(("LOADALL: setting ds.selector.rpl to %u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl));
  BX_INFO(("LOADALL: setting es.selector.rpl to %u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl));
#endif

  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x826, 2, &DI);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x828, 2, &SI);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x82a, 2, &BP);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x82c, 2, &SP);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x82e, 2, &BX);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x830, 2, &DX);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x832, 2, &CX);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x834, 2, &AX);

  /* GDTR */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x84e, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x850, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x851, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x852, 2, &limit);
  BX_CPU_THIS_PTR gdtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR gdtr.limit = limit;

  /* IDTR */
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x85a, 2, &base_15_0);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x85c, 1, &base_23_16);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x85d, 1, &access);
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, 0x85e, 2, &limit);
  BX_CPU_THIS_PTR idtr.base = (base_23_16 << 16) | base_15_0;
  BX_CPU_THIS_PTR idtr.limit = limit;
}
#endif

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
  }
  else
#endif
  {
    if (BX_CPU_THIS_PTR cr0.get_PE()) {
      if (BX_CPU_THIS_PTR get_VM()) {
        BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_V8086;
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

      BX_ASSERT(CPL == 0);
    }
  }

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

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
void BX_CPU_C::handleAlignmentCheck(void)
{
  if (CPL == 3 && BX_CPU_THIS_PTR cr0.get_AM() && BX_CPU_THIS_PTR get_AC()) {
    if (BX_CPU_THIS_PTR alignment_check_mask == 0) {
      BX_CPU_THIS_PTR alignment_check_mask = 0xF;
      BX_INFO(("Enable alignment check (#AC exception)"));
#if BX_SUPPORT_ICACHE
      BX_CPU_THIS_PTR iCache.flushICacheEntries();
#endif
    }
  }
  else {
    if (BX_CPU_THIS_PTR alignment_check_mask != 0) {
      BX_CPU_THIS_PTR alignment_check_mask = 0;
      BX_INFO(("Disable alignment check (#AC exception)"));
#if BX_SUPPORT_ICACHE
      BX_CPU_THIS_PTR iCache.flushICacheEntries();
#endif
    }
  }
}
#endif

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::SetCR0(Bit32u val_32)
{
  bx_bool pe = val_32 & 0x01;
  bx_bool nw = (val_32 >> 29) & 0x01;
  bx_bool cd = (val_32 >> 30) & 0x01;
  bx_bool pg = (val_32 >> 31) & 0x01;

  if (pg && !pe) {
    BX_ERROR(("SetCR0: GP(0) when attempt to set CR0.PG with CR0.PE cleared !"));
    return 0;
  }

  if (nw && !cd) {
    BX_ERROR(("SetCR0: GP(0) when attempt to set CR0.NW with CR0.CD cleared !"));
    return 0;
  }

  if (pe && BX_CPU_THIS_PTR get_VM()) BX_PANIC(("EFLAGS.VM=1, enter_PM"));

  // from either MOV_CdRd() or debug functions
  // protection checks made already or forcing from debug
  Bit32u oldCR0 = BX_CPU_THIS_PTR cr0.get32();

#if BX_SUPPORT_X86_64
  bx_bool prev_pg = BX_CPU_THIS_PTR cr0.get_PG();

  if (prev_pg==0 && pg) {
    if (BX_CPU_THIS_PTR efer.get_LME()) {
      if (!BX_CPU_THIS_PTR cr4.get_PAE()) {
        BX_ERROR(("SetCR0: attempt to enter x86-64 long mode without enabling CR4.PAE !"));
        return 0;
      }
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l) {
        BX_ERROR(("SetCR0: attempt to enter x86-64 long mode with CS.L !"));
        return 0;
      }
      if (BX_CPU_THIS_PTR tr.cache.type <= 3) {
        BX_ERROR(("SetCR0: attempt to enter x86-64 long mode with TSS286 in TR !"));
        return 0;
      }
      BX_CPU_THIS_PTR efer.set_LMA(1);
    }
  }
  else if (prev_pg==1 && ! pg) {
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      BX_ERROR(("SetCR0: attempt to leave 64 bit mode directly to legacy mode !"));
      return 0;
    }
    if (BX_CPU_THIS_PTR efer.get_LMA()) {
      if (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_RIP].dword.hrx != 0) {
        BX_PANIC(("SetCR0: attempt to leave x86-64 LONG mode with RIP upper != 0 !!!"));
      }
      BX_CPU_THIS_PTR efer.set_LMA(0);
    }
  }
#endif  // #if BX_SUPPORT_X86_64

  // handle reserved bits behaviour
#if BX_CPU_LEVEL == 3
  val_32 = val_32 | 0x7ffffff0;
#elif BX_CPU_LEVEL == 4
  val_32 = (val_32 | 0x00000010) & 0xe005003f;
#elif BX_CPU_LEVEL == 5
  val_32 = val_32 | 0x00000010;
#elif BX_CPU_LEVEL == 6
  val_32 = (val_32 | 0x00000010) & 0xe005003f;
#else
#error "SetCR0: implement reserved bits behaviour for this CPU_LEVEL"
#endif
  BX_CPU_THIS_PTR cr0.set32(val_32);

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  handleAlignmentCheck();
#endif

  handleCpuModeChange();

  // Give the paging unit a chance to look for changes in bits
  // it cares about, like {PG,PE}, so it can flush cache entries etc.
  pagingCR0Changed(oldCR0, val_32);

  return 1;
}

#if BX_CPU_LEVEL >= 4
bx_address get_cr4_allow_mask(void)
{
  bx_address allowMask = 0;

  // CR4 bits definitions:
  //   [31-19] Reserved, Must be Zero
  //   [18]    OSXSAVE: Operating System XSAVE Support R/W
  //   [17-15] Reserved, Must be Zero
  //   [14]    SMXE: SMX Extensions R/W
  //   [13]    VMXE: VMX Extensions R/W
  //   [12-11] Reserved, Must be Zero
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

#if BX_SUPPORT_VME
  allowMask |= (1<<0) | (1<<1);  /* VME */
#endif

#if BX_CPU_LEVEL >= 5
  allowMask |= (1<<2);   /* TSD */
#endif

  allowMask |= (1<<3);   /* DE  */

#if BX_SUPPORT_LARGE_PAGES
  allowMask |= (1<<4);
#endif

#if BX_SUPPORT_PAE
  allowMask |= (1<<5);
#endif

#if BX_CPU_LEVEL >= 5
  // NOTE: exception 18 (#MC) never appears in Bochs
  allowMask |= (1<<6);   /* MCE */
#endif

#if BX_SUPPORT_GLOBAL_PAGES
  allowMask |= (1<<7);
#endif

#if BX_CPU_LEVEL >= 6
  allowMask |= (1<<8);   /* PCE */
  allowMask |= (1<<9);   /* OSFXSR */
#endif

#if BX_SUPPORT_SSE
  allowMask |= (1<<10);  /* OSXMMECPT */
#endif

#if BX_SUPPORT_XSAVE
  allowMask |= (1<<18);  /* OSXSAVE */
#endif

  return allowMask;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::SetCR4(bx_address val)
{
  Bit32u oldCR4 = BX_CPU_THIS_PTR cr4.get32();
  bx_address allowMask = get_cr4_allow_mask();

#if BX_SUPPORT_X86_64
  // need to GP(0) if LMA=1 and PAE=1->0
  if (BX_CPU_THIS_PTR efer.get_LMA()) {
    if(!(val & (1<<5)) && BX_CPU_THIS_PTR cr4.get_PAE()) {
      BX_ERROR(("SetCR4: attempt to change PAE when EFER.LMA=1"));
      return 0;
    }
  }
#endif

  // Need to GPF if trying to set undefined bits.
  if (val & ~allowMask) {
    BX_ERROR(("#GP(0): SetCR4: Write of 0x%08x not supported (allowMask=0x%x)", (Bit32u) val, (Bit32u) allowMask));
    return 0;
  }

  BX_CPU_THIS_PTR cr4.set32(val);
  pagingCR4Changed(oldCR4, BX_CPU_THIS_PTR cr4.get32());
  return 1;
}
#endif // BX_CPU_LEVEL >= 4

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDPMC(bxInstruction_c *i)
{
/* We need to be Pentium with MMX or later */
#if (BX_CPU_LEVEL >= 6) || (BX_SUPPORT_MMX && BX_CPU_LEVEL == 5)
  bx_bool pce = BX_CPU_THIS_PTR cr4.get_PCE();

  if ((pce==1) || (CPL==0) || real_mode())
  {
    /* According to manual, Pentium 4 has 18 counters,
     * previous versions have two.  And the P4 also can do
     * short read-out (EDX always 0).  Otherwise it is
     * limited to 40 bits.
     */

#if (BX_CPU_LEVEL == 6 && BX_SUPPORT_SSE >= 2) // Pentium 4 processor (see cpuid.cc)
    if ((ECX & 0x7fffffff) >= 18)
      exception(BX_GP_EXCEPTION, 0, 0);
#else //
    if ((ECX & 0xffffffff) >= 2)
      exception(BX_GP_EXCEPTION, 0, 0);
#endif
    // Most counters are for hardware specific details, which
    // we anyhow do not emulate (like pipeline stalls etc)

    // Could be interesting to count number of memory reads,
    // writes.  Misaligned etc...  But to monitor bochs, this
    // is easier done from the host.

    RAX = 0;
    RDX = 0; // if P4 and ECX & 0x10000000, then always 0 (short read 32 bits)

    BX_ERROR(("RDPMC: Performance Counters Support not reasonably implemented yet"));
  } else {
    // not allowed to use RDPMC!
    exception(BX_GP_EXCEPTION, 0, 0);
  }
#else
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_CPU_LEVEL >= 5
Bit64u BX_CPU_C::get_TSC(void)
{
  Bit64u tsc = bx_pc_system.time_ticks() - BX_CPU_THIS_PTR msr.tsc_last_reset;
  return tsc;
}

void BX_CPU_C::set_TSC(Bit64u newval)
{
  // compute the correct setting of tsc_last_reset so that a get_TSC()
  // will return newval
  BX_CPU_THIS_PTR msr.tsc_last_reset = bx_pc_system.time_ticks() - newval;

  // verify
  BX_ASSERT(get_TSC() == newval);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  if (! BX_CPU_THIS_PTR cr4.get_TSD() || CPL==0) {
    // return ticks
    Bit64u ticks = BX_CPU_THIS_PTR get_TSC();
    RAX = GET32L(ticks);
    RDX = GET32H(ticks);
  } else {
    BX_ERROR(("RDTSC: not allowed to use instruction !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }
#else
  BX_INFO(("RDTSC: Pentium CPU required, use --enable-cpu=5"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSCP(bxInstruction_c *i)
{
  RDTSC(i);
  RCX = MSR_TSC_AUX;
}
#endif

#if BX_SUPPORT_MONITOR_MWAIT
bx_bool BX_CPU_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  bx_phy_address end_addr = begin_addr + len;
  if (begin_addr >= BX_CPU_THIS_PTR monitor.monitor_end || end_addr <= BX_CPU_THIS_PTR monitor.monitor_begin)
    return 0;
  else
    return 1;
}

void BX_CPU_C::check_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (is_monitor(begin_addr, len)) {
    // wakeup from MWAIT state
    BX_ASSERT(BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT);
    BX_CPU_THIS_PTR debug_trap &= ~BX_DEBUG_TRAP_SPECIAL;
    // clear monitor
    BX_MEM(0)->clear_monitor(BX_CPU_THIS_PTR bx_cpuid);
    BX_CPU_THIS_PTR monitor.reset_monitor();
 }
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MONITOR(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  // TODO: #UD when CPL > 0 and
  //       MSR 0xC0010015[MONITOR_MWAIT_USER_UNABLE] = 1
  BX_DEBUG(("MONITOR instruction executed EAX = 0x08x", (unsigned) EAX));

  if (RCX != 0) {
    BX_ERROR(("MONITOR: no optional extensions supported"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  bx_segment_reg_t *seg = &BX_CPU_THIS_PTR sregs[i->seg()];

  bx_address offset;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
     offset = RAX;
  }
  else
#endif
  if (i->as32L()) {
     offset = EAX;
  }
  else {
     offset =  AX;
  }

  // set MONITOR
  bx_address laddr = BX_CPU_THIS_PTR get_laddr(i->seg(), offset);

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (! IsCanonical(laddr)) {
      BX_ERROR(("MONITOR: non-canonical access !"));
      exception(int_number(i->seg()), 0, 0);
    }
  }
  else
#endif
  {
    // check if we could access the memory segment
    if (!(seg->cache.valid & SegAccessROK)) {
      if (! read_virtual_checks(seg, offset, 1))
        exception(int_number(i->seg()), 0, 0);
    }
    else {
      if (offset > seg->cache.u.segment.limit_scaled) {
        BX_ERROR(("MONITOR: segment limit violation"));
        exception(int_number(i->seg()), 0, 0);
      }
    }
  }

  bx_phy_address paddr;

  if (BX_CPU_THIS_PTR cr0.get_PG()) {
    paddr = dtranslate_linear(laddr, CPL, BX_READ);
    paddr = A20ADDR(paddr);
  }
  else
  {
    paddr = A20ADDR(laddr);
  }

  BX_CPU_THIS_PTR monitor.monitor_begin = paddr;
  BX_CPU_THIS_PTR monitor.monitor_end   = paddr + CACHE_LINE_SIZE;

  // Set the monitor immediately.  If monitor is still armed when we MWAIT,
  // the processor will stall.
  bx_pc_system.invlpg(BX_CPU_THIS_PTR monitor.monitor_begin);
  if ((BX_CPU_THIS_PTR monitor.monitor_end & ~0xfff) != (BX_CPU_THIS_PTR monitor.monitor_begin & ~0xfff))
    bx_pc_system.invlpg(BX_CPU_THIS_PTR monitor.monitor_end);
  BX_DEBUG(("MONITOR for phys_addr=0x" FMT_PHY_ADDRX, BX_CPU_THIS_PTR monitor.monitor_begin));
  BX_MEM(0)->set_monitor(BX_CPU_THIS_PTR bx_cpuid);
#else
  BX_INFO(("MONITOR: use --enable-monitor-mwait to enable MONITOR/MWAIT support"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MWAIT(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  // TODO: #UD when CPL > 0 and
  //       MSR 0xC0010015[MONITOR_MWAIT_USER_UNABLE] = 1
  BX_DEBUG(("MWAIT instruction executed ECX = 0x%08x", ECX));

  // only one extension is supported
  //   ECX[0] - interrupt MWAIT even if EFLAGS.IF = 0
  if (RCX & ~(BX_CONST64(1))) {
    BX_ERROR(("MWAIT: incorrect optional extensions in RCX"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  // Do not enter optimized state if MONITOR wasn't properly set
  if (BX_CPU_THIS_PTR monitor.monitor_begin == BX_CPU_THIS_PTR monitor.monitor_end) {
    BX_DEBUG(("MWAIT: incorrect MONITOR settings"));
    return;
  }

  // If monitor has already triggered, we just return.
  if (!BX_CPU_THIS_PTR monitor.armed) {
    BX_DEBUG(("MWAIT: the MONITOR was already triggered"));
    return;
  }

  // stops instruction execution and places the processor in a optimized
  // state.  Events that cause exit from MWAIT state are:
  // A store from another processor to monitored range, any unmasked
  // interrupt, including INTR, NMI, SMI, INIT or reset will resume
  // the execution. Any far control transfer between MONITOR and MWAIT
  // resets the monitoring logic.

  // artificial trap bit, why use another variable.
  BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_TRAP_MWAIT; // artificial trap
  if (ECX & 1)
    BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_TRAP_MWAIT_IF;
  BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
  // Execution of this instruction completes.  The processor
  // will remain in a optimized state until one of the above
  // conditions is met.

  BX_INSTR_MWAIT(BX_CPU_ID, BX_CPU_THIS_PTR monitor.monitor_begin, CACHE_LINE_SIZE, ECX);

#if BX_USE_IDLE_HACK
  bx_gui->sim_is_idle();
#endif

#if BX_DEBUGGER
  bx_dbg_halt(BX_CPU_ID);
#endif

#else
  BX_INFO(("MWAIT: use --enable-monitor-mwait to enable MONITOR/MWAIT support"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSENTER(bxInstruction_c *i)
{
#if BX_SUPPORT_SEP
  if (real_mode()) {
    BX_ERROR(("SYSENTER not recognized in real mode !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }
  if ((BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK) == 0) {
    BX_ERROR(("SYSENTER with zero sysenter_cs_msr !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  invalidate_prefetch_q();

  BX_CPU_THIS_PTR clear_VM();       // do this just like the book says to do
  BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_RF();

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    if (!IsCanonical(BX_CPU_THIS_PTR msr.sysenter_eip_msr)) {
      BX_ERROR(("SYSENTER with non-canonical SYSENTER_EIP_MSR !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    if (!IsCanonical(BX_CPU_THIS_PTR msr.sysenter_esp_msr)) {
      BX_ERROR(("SYSENTER with non-canonical SYSENTER_ESP_MSR !"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }
  }
#endif

  parse_selector(BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;     // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF; // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1;          // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;          // available for use by system
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = !long_mode();
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            =  long_mode();
#endif

  updateFetchModeMask();

#if BX_SUPPORT_X86_64
  handleCpuModeChange(); // mode change could happen only when in long_mode()
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

  parse_selector((BX_CPU_THIS_PTR msr.sysenter_cs_msr + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;          // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;     // segment limit
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
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#else
  BX_INFO(("SYSENTER: use --enable-sep to enable SYSENTER/SYSEXIT support"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSEXIT(bxInstruction_c *i)
{
#if BX_SUPPORT_SEP
  if (real_mode() || CPL != 0) {
    BX_ERROR(("SYSEXIT from real mode or with CPL<>0 !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }
  if ((BX_CPU_THIS_PTR msr.sysenter_cs_msr & BX_SELECTOR_RPL_MASK) == 0) {
    BX_ERROR(("SYSEXIT with zero sysenter_cs_msr !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    if (!IsCanonical(RDX)) {
      BX_ERROR(("SYSEXIT with non-canonical RDX (RIP) pointer !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    if (!IsCanonical(RCX)) {
      BX_ERROR(("SYSEXIT with non-canonical RCX (RSP) pointer !"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }
  }
#endif

  invalidate_prefetch_q();

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    parse_selector(((BX_CPU_THIS_PTR msr.sysenter_cs_msr + 32) & BX_SELECTOR_RPL_MASK) | 3,
            &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;           // base address
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      // segment limit
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

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;           // base address
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      // segment limit
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

  updateFetchModeMask();

#if BX_SUPPORT_X86_64
  handleCpuModeChange(); // mode change could happen only when in long_mode()
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  handleAlignmentCheck(); // CPL was modified
#endif

  parse_selector(((BX_CPU_THIS_PTR msr.sysenter_cs_msr + (i->os64L() ? 40:24)) & BX_SELECTOR_RPL_MASK) | 3,
            &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl      = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0;           // base address
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;      // segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  // scaled segment limit
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1;           // 4k granularity
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1;           // 32-bit mode
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;           // available for use by system
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSEXIT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
#else
  BX_INFO(("SYSEXIT: use --enable-sep to enable SYSENTER/SYSEXIT support"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSCALL(bxInstruction_c *i)
{
  bx_address temp_RIP;

  BX_DEBUG(("Execute SYSCALL instruction"));

  if (!BX_CPU_THIS_PTR efer.get_SCE()) {
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  invalidate_prefetch_q();

  if (long_mode())
  {
    RCX = RIP;
    R11 = read_eflags() & ~(EFlagsRFMask);

    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      temp_RIP = MSR_LSTAR;
    }
    else {
      temp_RIP = MSR_CSTAR;
    }

    // set up CS segment, flat, 64-bit DPL=0
    parse_selector((MSR_STAR >> 32) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 1; /* 64-bit code */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    updateFetchModeMask();
    handleCpuModeChange(); // mode change could only happen when in long_mode()

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

    // set up SS segment, flat, 64-bit DPL=0
    parse_selector(((MSR_STAR >> 32) + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1; /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1; /* 32 bit stack */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0; /* available for use by system */

    writeEFlags(read_eflags() & (~MSR_FMASK), EFlagsValidMask);
    BX_CPU_THIS_PTR clear_RF();
    RIP = temp_RIP;
  }
  else {
    // legacy mode

    ECX = EIP;
    temp_RIP = MSR_STAR & 0xFFFFFFFF;

    // set up CS segment, flat, 32-bit DPL=0
    parse_selector((MSR_STAR >> 32) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    updateFetchModeMask();

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

    // set up SS segment, flat, 32-bit DPL=0
    parse_selector(((MSR_STAR >> 32) + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1; /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 1; /* 32 bit stack */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.l            = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0; /* available for use by system */

    BX_CPU_THIS_PTR clear_VM();
    BX_CPU_THIS_PTR clear_IF();
    BX_CPU_THIS_PTR clear_RF();
    RIP = temp_RIP;
  }

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSCALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSRET(bxInstruction_c *i)
{
  bx_address temp_RIP;

  BX_DEBUG(("Execute SYSRET instruction"));

  if (!BX_CPU_THIS_PTR efer.get_SCE()) {
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  if(!protected_mode() || CPL != 0) {
    BX_ERROR(("SYSRET: priveledge check failed, generate #GP(0)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (!IsCanonical(RCX)) {
    BX_ERROR(("SYSRET: canonical failure for RCX (RIP)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }
#endif

  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
  {
    if (i->os64L()) {
      // Return to 64-bit mode, set up CS segment, flat, 64-bit DPL=3
      parse_selector((((MSR_STAR >> 48) + 16) & BX_SELECTOR_RPL_MASK) | 3,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 1; /* 64-bit code */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

      temp_RIP = RCX;
    }
    else {
      // Return to 32-bit compatibility mode, set up CS segment, flat, 32-bit DPL=3
      parse_selector((MSR_STAR >> 48) | 3,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

      temp_RIP = ECX;
    }

    updateFetchModeMask();
    handleCpuModeChange(); // mode change could only happen when in long64 mode

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    handleAlignmentCheck(); // CPL was modified
#endif

    // SS base, limit, attributes unchanged
    parse_selector((Bit16u)((MSR_STAR >> 48) + 8),
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    writeEFlags((Bit32u) R11, EFlagsValidMask);
  }
  else { // (!64BIT_MODE)
    // Return to 32-bit legacy mode, set up CS segment, flat, 32-bit DPL=3
    parse_selector((MSR_STAR >> 48) | 3,
                     &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0; /* base address */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;      /* segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;  /* scaled segment limit */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 1; /* 4k granularity */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l            = 0; /* 32-bit code */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0; /* available for use by system */

    updateFetchModeMask();

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    handleAlignmentCheck(); // CPL was modified
#endif

    // SS base, limit, attributes unchanged
    parse_selector((Bit16u)((MSR_STAR >> 48) + 8),
                     &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1;  /* data/code segment */
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    BX_CPU_THIS_PTR assert_IF();
    temp_RIP = ECX;
  }

  handleCpuModeChange();

  RIP = temp_RIP;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_SYSRET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SWAPGS(bxInstruction_c *i)
{
  Bit64u temp_GS_base;

  BX_ASSERT(protected_mode());

  if(CPL != 0)
    exception(BX_GP_EXCEPTION, 0, 0);

  temp_GS_base = MSR_GSBASE;
  MSR_GSBASE = MSR_KERNELGSBASE;
  MSR_KERNELGSBASE = temp_GS_base;
}
#endif

#if BX_X86_DEBUGGER
bx_bool BX_CPU_C::hwbreakpoint_check(bx_address laddr)
{
  laddr = LPFOf(laddr);

  for (int i=0;i<4;i++) {
    if (laddr == LPFOf(BX_CPU_THIS_PTR dr[i]))
      return 1;
  }

  return 0;
}

void BX_CPU_C::hwbreakpoint_match(bx_address laddr, unsigned len, unsigned rw)
{
  if (BX_CPU_THIS_PTR dr7 & 0x000000ff) {
    // Only compare debug registers if any breakpoints are enabled
    unsigned opa, opb, write = rw & 1;
    opa = BX_HWDebugMemRW; // Read or Write always compares vs 11b
    if (! write) // only compares vs 11b
      opb = opa;
    else // BX_WRITE or BX_RW; also compare vs 01b
      opb = BX_HWDebugMemW;
    Bit32u dr6_bits = hwdebug_compare(laddr, len, opa, opb);
    if (dr6_bits) {
      BX_CPU_THIS_PTR debug_trap |= dr6_bits;
      BX_CPU_THIS_PTR async_event = 1;
    }
  }
}

Bit32u BX_CPU_C::hwdebug_compare(bx_address laddr_0, unsigned size,
                          unsigned opa, unsigned opb)
{
  // Support x86 hardware debug facilities (DR0..DR7)
  Bit32u dr7 = BX_CPU_THIS_PTR dr7;

  static bx_address alignment_mask[4] =
    // 00b=1  01b=2  10b=undef(8)  11b=4
    {  0x0,   0x1,   0x7,          0x3   };

  bx_address laddr_n = laddr_0 + (size - 1);
  bx_address dr[4], dr_n[4];
  Bit32u dr_op[4], len[4];
  bx_bool ibpoint_found_n[4], ibpoint_found = 0;

  len[0] = (dr7>>18) & 3;
  len[1] = (dr7>>22) & 3;
  len[2] = (dr7>>26) & 3;
  len[3] = (dr7>>30) & 3;

  dr_op[0] = (dr7>>16) & 3;
  dr_op[1] = (dr7>>20) & 3;
  dr_op[2] = (dr7>>24) & 3;
  dr_op[3] = (dr7>>28) & 3;

  for (unsigned n=0;n<4;n++) {
    dr[n]   = BX_CPU_THIS_PTR dr[n] & ~alignment_mask[len[n]];
    dr_n[n] = dr[n] + alignment_mask[len[n]];
    ibpoint_found_n[n] = 0;

    // See if this instruction address matches any breakpoints
    if (dr7 & (3 << n*2)) {
      if ((dr_op[n]==opa || dr_op[n]==opb) &&
           (laddr_0 <= dr_n[n]) &&
           (laddr_n >= dr[n])) {
        ibpoint_found_n[n] = 1;
        ibpoint_found = 1;
      }
    }
  }

  // If *any* enabled breakpoints matched, then we need to
  // set status bits for *all* breakpoints, even disabled ones,
  // as long as they meet the other breakpoint criteria.
  // dr6_mask is the return value.  These bits represent the bits
  // to be OR'd into DR6 as a result of the debug event.
  Bit32u dr6_mask = 0;

  if (ibpoint_found) {
    if (ibpoint_found_n[0]) dr6_mask |= 0x1;
    if (ibpoint_found_n[1]) dr6_mask |= 0x2;
    if (ibpoint_found_n[2]) dr6_mask |= 0x4;
    if (ibpoint_found_n[3]) dr6_mask |= 0x8;
  }

  return dr6_mask;
}

void BX_CPU_C::iobreakpoint_match(unsigned port, unsigned len)
{
  // Only compare debug registers if any breakpoints are enabled
  if (BX_CPU_THIS_PTR cr4.get_DE() && (BX_CPU_THIS_PTR dr7 & 0x000000ff))
  {
    Bit32u dr_op[4], dr_len[4];
    Bit32u dr7 = BX_CPU_THIS_PTR dr7;

    dr_len[0] = 1 + (dr7>>18) & 3;
    dr_len[1] = 1 + (dr7>>22) & 3;
    dr_len[2] = 1 + (dr7>>26) & 3;
    dr_len[3] = 1 + (dr7>>30) & 3;

    dr_op[0] = (dr7>>16) & 3;
    dr_op[1] = (dr7>>20) & 3;
    dr_op[2] = (dr7>>24) & 3;
    dr_op[3] = (dr7>>28) & 3;

    for (unsigned n=0;n<4;n++) {
      if (dr_op[n] == 2 && dr_len[n] == len && BX_CPU_THIS_PTR dr[n] == port) {
        BX_CPU_THIS_PTR debug_trap |= (1<<n);
        BX_CPU_THIS_PTR async_event = 1;
      }        
    }
  }
}
#endif
