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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

BX_CPP_INLINE void BX_CPP_AttrRegparmN(1) BX_CPU_C::branch_near64(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("branch_near64: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = new_RIP;

  if (bx_dbg.debugger_active || BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS == 0) {
    // assert magic async_event to stop trace execution
    BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear64_Iw(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  RSP_SPECULATIVE;

  Bit64u return_RIP = pop_64();
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL)) {
    Bit64u shadow_RIP = shadow_stack_pop_64();
    if (shadow_RIP != return_RIP)
      exception(BX_CP_EXCEPTION, BX_CP_NEAR_RET);
  }
#endif

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = return_RIP;
  RSP += i->Iw();

  RSP_COMMIT;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, PREV_RIP, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar64_Iw(bxInstruction_c *i)
{
  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_ASSERT(protected_mode());

  RSP_SPECULATIVE;

  // return_protected is RSP safe
  return_protected(i, i->Iw());

  RSP_COMMIT;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  RSP_SPECULATIVE;

  /* push 64 bit EA of next instruction */
  push_64(RIP);
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL) && i->Id())
    shadow_stack_push_64(RIP);
#endif

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = new_RIP;

  RSP_COMMIT;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, PREV_RIP, RIP);

  BX_LINK_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_EqR(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  Bit64u new_RIP = BX_READ_64BIT_REG(i->dst());

  RSP_SPECULATIVE;

  /* push 64 bit EA of next instruction */
  push_64(RIP);
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL))
    shadow_stack_push_64(RIP);
#endif

  if (! IsCanonical(new_RIP))
  {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = new_RIP;

  RSP_COMMIT;

#if BX_SUPPORT_CET
  track_indirect_if_not_suppressed(i, CPL);
#endif

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL_INDIRECT, PREV_RIP, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL64_Ep(bxInstruction_c *i)
{
  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  /* pointer, segment address pair */
  Bit64u op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  Bit16u cs_raw = read_linear_word(i->seg(), get_laddr64(i->seg(), (eaddr+8) & i->asize_mask()));

  BX_ASSERT(protected_mode());

  RSP_SPECULATIVE;

  call_protected(i, cs_raw, op1_64);

  RSP_COMMIT;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL_INDIRECT,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = new_RIP;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, PREV_RIP, RIP);

  BX_LINK_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JO_Jq(bxInstruction_c *i)
{
  if (get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNO_Jq(bxInstruction_c *i)
{
  if (! get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JB_Jq(bxInstruction_c *i)
{
  if (get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNB_Jq(bxInstruction_c *i)
{
  if (! get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JZ_Jq(bxInstruction_c *i)
{
  if (get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNZ_Jq(bxInstruction_c *i)
{
  if (! get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JBE_Jq(bxInstruction_c *i)
{
  if (get_CF() || get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNBE_Jq(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JS_Jq(bxInstruction_c *i)
{
  if (get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNS_Jq(bxInstruction_c *i)
{
  if (! get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JP_Jq(bxInstruction_c *i)
{
  if (get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNP_Jq(bxInstruction_c *i)
{
  if (! get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JL_Jq(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNL_Jq(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JLE_Jq(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNLE_Jq(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());

  if (! IsCanonical(op1_64)) {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = op1_64;
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP_INDIRECT, PREV_RIP, RIP);

#if BX_SUPPORT_CET
  track_indirect_if_not_suppressed(i, CPL);
#endif

  BX_NEXT_TRACE(i);
}

/* Far indirect jump */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP64_Ep(bxInstruction_c *i)
{
  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit64u op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  Bit16u cs_raw = read_linear_word(i->seg(), get_laddr64(i->seg(), (eaddr+8) & i->asize_mask()));

  BX_ASSERT(protected_mode());

  jump_protected(i, cs_raw, op1_64);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP_INDIRECT,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IRET64(bxInstruction_c *i)
{
  invalidate_prefetch_q();

  BX_INSTR_FAR_BRANCH_ORIGIN();

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_IRET)) Svm_Vmexit(SVM_VMEXIT_IRET);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    if (is_masked_event(BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI() ? BX_EVENT_VMX_VIRTUAL_NMI : BX_EVENT_NMI))
      BX_CPU_THIS_PTR nmi_unblocking_iret = true;

  if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.NMI_EXITING()) {
    if (BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI()) unmask_event(BX_EVENT_VMX_VIRTUAL_NMI);
  }
  else
#endif
    unmask_event(BX_EVENT_NMI);

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_umonitor();
#endif

  BX_ASSERT(long_mode());

  RSP_SPECULATIVE;

  long_iret(i);

  RSP_COMMIT;

#if BX_SUPPORT_VMX
  BX_CPU_THIS_PTR nmi_unblocking_iret = false;
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_IRET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JRCXZ_Jb(bxInstruction_c *i)
{
  Bit64u temp_RCX;

  if (i->as64L())
    temp_RCX = RCX;
  else
    temp_RCX = ECX;

  if (temp_RCX == 0) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_TRACE(i);
}

//
// There is some weirdness in LOOP instructions definition. If an exception
// was generated during the instruction execution (for example #GP fault
// because EIP was beyond CS segment limits) CPU state should restore the
// state prior to instruction execution.
//
// The final point that we are not allowed to decrement RCX register before
// it is known that no exceptions can happen.
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPNE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if (((--count) != 0) && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if (((--count) != 0) && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if (((--count) != 0) && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if (((--count) != 0) && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOP64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if ((--count) != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if ((--count) != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    RCX = count;
  }

  BX_NEXT_TRACE(i);
}

#endif /* if BX_SUPPORT_X86_64 */
