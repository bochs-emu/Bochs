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



#include "bochs.h"






  void
BX_CPU_C::RETnear32_Iw(BxInstruction_t *i)
{
  Bit16u imm16;
  Bit32u temp_ESP;
  Bit32u return_EIP;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
    temp_ESP = ESP;
  else
    temp_ESP = SP;

  imm16 = i->Iw;

  invalidate_prefetch_q();


    if (protected_mode()) {
      if ( !can_pop(4) ) {
        bx_panic("retnear_iw: can't pop EIP\n");
        /* ??? #SS(0) -or #GP(0) */
        }

      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);

      if (protected_mode() &&
          (return_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) ) {
        BX_CPU_THIS_PTR ldebug("retnear_iw: EIP > limit\n");
        exception(BX_GP_EXCEPTION, 0, 0);
        }

      /* Pentium book says imm16 is number of words ??? */
      if ( !can_pop(4 + imm16) ) {
        bx_panic("retnear_iw: can't release bytes from stack\n");
        /* #GP(0) -or #SS(0) ??? */
        }

      BX_CPU_THIS_PTR eip = return_EIP;
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
        ESP += 4 + imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += 4 + imm16;
      }
    else {
      pop_32(&return_EIP);
      BX_CPU_THIS_PTR eip = return_EIP;
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
        ESP += imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += imm16;
      }

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_RET, BX_CPU_THIS_PTR eip);
}

  void
BX_CPU_C::RETnear32(BxInstruction_t *i)
{
  Bit32u temp_ESP;
  Bit32u return_EIP;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  invalidate_prefetch_q();

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
    temp_ESP = ESP;
  else
    temp_ESP = SP;


    if (protected_mode()) {
      if ( !can_pop(4) ) {
        bx_panic("retnear: can't pop EIP\n");
        /* ??? #SS(0) -or #GP(0) */
        }

      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);

      if ( return_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled ) {
        bx_panic("retnear: EIP > limit\n");
        //exception(BX_GP_EXCEPTION, 0, 0);
        }
      BX_CPU_THIS_PTR eip = return_EIP;
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
        ESP += 4;
      else
        SP  += 4;
      }
    else {
      pop_32(&return_EIP);
      BX_CPU_THIS_PTR eip = return_EIP;
      }

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_RET, BX_CPU_THIS_PTR eip);
}

  void
BX_CPU_C::RETfar32_Iw(BxInstruction_t *i)
{
  Bit32u eip, ecs_raw;
  Bit16s imm16;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif
  /* ??? is imm16, number of bytes/words depending on operandsize ? */

  imm16 = i->Iw;

  invalidate_prefetch_q();

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    BX_CPU_THIS_PTR return_protected(i, imm16);
    goto done;
    }
#endif


    pop_32(&eip);
    pop_32(&ecs_raw);
    BX_CPU_THIS_PTR eip = eip;
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) ecs_raw);
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      ESP += imm16;
    else
      SP  += imm16;

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}

  void
BX_CPU_C::RETfar32(BxInstruction_t *i)
{
  Bit32u eip, ecs_raw;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  invalidate_prefetch_q();

#if BX_CPU_LEVEL >= 2
  if ( protected_mode() ) {
    BX_CPU_THIS_PTR return_protected(i, 0);
    goto done;
    }
#endif


    pop_32(&eip);
    pop_32(&ecs_raw); /* 32bit pop, MSW discarded */
    BX_CPU_THIS_PTR eip = eip;
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) ecs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}



  void
BX_CPU_C::CALL_Ad(BxInstruction_t *i)
{
  Bit32u new_EIP;
  Bit32s disp32;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  disp32 = i->Id;
  invalidate_prefetch_q();

  new_EIP = EIP + disp32;

  if ( protected_mode() ) {
    if ( new_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled ) {
      bx_panic("call_av: offset outside of CS limits\n");
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }

  /* push 32 bit EA of next instruction */
  push_32(BX_CPU_THIS_PTR eip);
  BX_CPU_THIS_PTR eip = new_EIP;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_CALL, BX_CPU_THIS_PTR eip);
}

  void
BX_CPU_C::CALL32_Ap(BxInstruction_t *i)
{
  Bit16u cs_raw;
  Bit32u disp32;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  disp32 = i->Id;
  cs_raw = i->Iw2;
  invalidate_prefetch_q();

  if (protected_mode()) {
    BX_CPU_THIS_PTR call_protected(i, cs_raw, disp32);
    goto done;
    }
  push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_32(BX_CPU_THIS_PTR eip);
  BX_CPU_THIS_PTR eip = disp32;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}

  void
BX_CPU_C::CALL_Ed(BxInstruction_t *i)
{
  Bit32u temp_ESP;
  Bit32u op1_32;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;


    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      read_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }
    invalidate_prefetch_q();

    if (protected_mode()) {
      if (op1_32 > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
        BX_CPU_THIS_PTR ldebug("call_ev: EIP out of CS limits! at %s:%d\n",__FILE__,
__LINE__);
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 4) ) {
        bx_panic("call_ev: can't push EIP\n");
        }
      }

    push_32(BX_CPU_THIS_PTR eip);

    BX_CPU_THIS_PTR eip = op1_32;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_CALL, BX_CPU_THIS_PTR eip);
}

  void
BX_CPU_C::CALL32_Ep(BxInstruction_t *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      bx_panic("CALL_Ep: op1 is a register\n");
      }

    /* pointer, segment address pair */
    read_virtual_dword(i->seg, i->rm_addr, &op1_32);
    read_virtual_word(i->seg, i->rm_addr+4, &cs_raw);
    invalidate_prefetch_q();

    if ( protected_mode() ) {
      BX_CPU_THIS_PTR call_protected(i, cs_raw, op1_32);
      goto done;
      }

    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
    push_32(BX_CPU_THIS_PTR eip);

    BX_CPU_THIS_PTR eip = op1_32;
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}


  void
BX_CPU_C::JMP_Jd(BxInstruction_t *i)
{
  Bit32u new_EIP;

    invalidate_prefetch_q();

    new_EIP = EIP + (Bit32s) i->Id;

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    if ( new_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled ) {
      bx_panic("jmp_jv: offset outside of CS limits\n");
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
#endif

  BX_CPU_THIS_PTR eip = new_EIP;
  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_JMP, new_EIP);
}

  void
BX_CPU_C::JCC_Jd(BxInstruction_t *i)
{
  Boolean condition = 0;

  switch (i->b1 & 0x0f) {
    case 0x00: /* JO */ condition = get_OF(); break;
    case 0x01: /* JNO */ condition = !get_OF(); break;
    case 0x02: /* JB */ condition = get_CF(); break;
    case 0x03: /* JNB */ condition = !get_CF(); break;
    case 0x04: /* JZ */ condition = get_ZF(); break;
    case 0x05: /* JNZ */ condition = !get_ZF(); break;
    case 0x06: /* JBE */ condition = get_CF() || get_ZF(); break;
    case 0x07: /* JNBE */ condition = !get_CF() && !get_ZF(); break;
    case 0x08: /* JS */ condition = get_SF(); break;
    case 0x09: /* JNS */ condition = !get_SF(); break;
    case 0x0A: /* JP */ condition = get_PF(); break;
    case 0x0B: /* JNP */ condition = !get_PF(); break;
    case 0x0C: /* JL */ condition = get_SF() != get_OF(); break;
    case 0x0D: /* JNL */ condition = get_SF() == get_OF(); break;
    case 0x0E: /* JLE */ condition = get_ZF() || (get_SF() != get_OF());
      break;
    case 0x0F: /* JNLE */ condition = (get_SF() == get_OF()) &&
                            !get_ZF();
      break;
    }

  if (condition) {
    Bit32u new_EIP;

    new_EIP = EIP + (Bit32s) i->Id;
#if BX_CPU_LEVEL >= 2
    if (protected_mode()) {
      if ( new_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled ) {
        bx_panic("jo_routine: offset outside of CS limits\n");
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      }
#endif
    EIP = new_EIP;
    BX_INSTR_CNEAR_BRANCH_TAKEN(new_EIP);
    revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN();
    }
#endif
}

  void
BX_CPU_C::JMP_Ap(BxInstruction_t *i)
{
  Bit32u disp32;
  Bit16u cs_raw;

  invalidate_prefetch_q();

  if (i->os_32) {
    disp32 = i->Id;
    }
  else {
    disp32 = i->Iw;
    }
  cs_raw = i->Iw2;

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    BX_CPU_THIS_PTR jump_protected(i, cs_raw, disp32);
    goto done;
    }
#endif

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
  BX_CPU_THIS_PTR eip = disp32;

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}




  void
BX_CPU_C::JMP_Ed(BxInstruction_t *i)
{
  Bit32u new_EIP;
  Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    invalidate_prefetch_q();
    new_EIP = op1_32;

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    if (new_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      bx_panic("jmp_ev: IP out of CS limits!\n");
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
#endif

  BX_CPU_THIS_PTR eip = new_EIP;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_JMP, new_EIP);
}

  /* Far indirect jump */

  void
BX_CPU_C::JMP32_Ep(BxInstruction_t *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      /* far indirect must specify a memory address */
      bx_panic("JMP_Ep(): op1 is a register\n");
      }

    /* pointer, segment address pair */
    read_virtual_dword(i->seg, i->rm_addr, &op1_32);
    read_virtual_word(i->seg, i->rm_addr+4, &cs_raw);
    invalidate_prefetch_q();

    if ( protected_mode() ) {
      BX_CPU_THIS_PTR jump_protected(i, cs_raw, op1_32);
      goto done;
      }

    BX_CPU_THIS_PTR eip = op1_32;
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}

  void
BX_CPU_C::IRET32(BxInstruction_t *i)
{
  Bit32u eip, ecs_raw, eflags;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
  BX_CPU_THIS_PTR show_eip = BX_CPU_THIS_PTR eip;
#endif

  invalidate_prefetch_q();

  if (v8086_mode()) {
    // IOPL check in stack_return_from_v86()
    stack_return_from_v86(i);
    goto done;
    }

#if BX_CPU_LEVEL >= 2
  if (BX_CPU_THIS_PTR cr0.pe) {
    iret_protected(i);
    goto done;
    }
#endif


    pop_32(&eip);
    pop_32(&ecs_raw);
    pop_32(&eflags);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) ecs_raw);
    BX_CPU_THIS_PTR eip = eip;
    write_eflags(eflags, /* change IOPL? */ 1, /* change IF? */ 1, 0, 1);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_IRET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
  return;
}
