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





#define BX_IN_CPU_METHOD 1
#include "bochs.h"



  void
BX_CPU_C::POP_Ed(BxInstruction_t *i)
{
  Bit32u val32;

  pop_32(&val32);

  if (i->mod == 0xc0) {
    BX_WRITE_32BIT_REG(i->rm, val32);
    }
  else {
    // Note: there is one little weirdism here.  When 32bit addressing
    // is used, it is possible to use ESP in the modrm addressing.
    // If used, the value of ESP after the pop is used to calculate
    // the address.
    if (i->as_32 && (i->mod!=0xc0) && (i->rm==4) && (i->base==4)) {
      // call method on BX_CPU_C object
      BxExecutePtr_t func = i->ResolveModrm;
      (this->*func) (i);
      }
    write_virtual_dword(i->seg, i->rm_addr, &val32);
    }
}

  void
BX_CPU_C::PUSH_ERX(BxInstruction_t *i)
{
  push_32(BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].erx);
}

  void
BX_CPU_C::POP_ERX(BxInstruction_t *i)
{
  Bit32u erx;

  pop_32(&erx);
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].erx = erx;
}


  void
BX_CPU_C::PUSH_CS(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
}
  void
BX_CPU_C::PUSH_DS(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
}
  void
BX_CPU_C::PUSH_ES(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
}
  void
BX_CPU_C::PUSH_FS(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}
  void
BX_CPU_C::PUSH_GS(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}
  void
BX_CPU_C::PUSH_SS(BxInstruction_t *i)
{
  if (i->os_32)
    push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
  else
    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
}


  void
BX_CPU_C::POP_DS(BxInstruction_t *i)
{
  if (i->os_32) {
    Bit32u ds;
    pop_32(&ds);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], (Bit16u) ds);
    }
  else {
    Bit16u ds;
    pop_16(&ds);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);
    }
}
  void
BX_CPU_C::POP_ES(BxInstruction_t *i)
{
  if (i->os_32) {
    Bit32u es;
    pop_32(&es);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], (Bit16u) es);
    }
  else {
    Bit16u es;
    pop_16(&es);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);
    }
}
  void
BX_CPU_C::POP_FS(BxInstruction_t *i)
{
  if (i->os_32) {
    Bit32u fs;
    pop_32(&fs);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], (Bit16u) fs);
    }
  else {
    Bit16u fs;
    pop_16(&fs);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);
    }
}
  void
BX_CPU_C::POP_GS(BxInstruction_t *i)
{
  if (i->os_32) {
    Bit32u gs;
    pop_32(&gs);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], (Bit16u) gs);
    }
  else {
    Bit16u gs;
    pop_16(&gs);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);
    }
}
  void
BX_CPU_C::POP_SS(BxInstruction_t *i)
{
  if (i->os_32) {
    Bit32u ss;
    pop_32(&ss);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], (Bit16u) ss);
    }
  else {
    Bit16u ss;
    pop_16(&ss);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);
    }

  // POP SS inhibits interrupts, debug exceptions and single-step
  // trap exceptions until the execution boundary following the
  // next instruction is reached.
  // Same code as MOV_SwEw()
  BX_CPU_THIS_PTR inhibit_mask |=
    BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
  BX_CPU_THIS_PTR async_event = 1;
}


  void
BX_CPU_C::PUSHAD32(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("PUSHAD: not supported on an 8086\n");
#else
  Bit32u temp_ESP;
  Bit32u esp;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;


    if (protected_mode()) {
      if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 32) ) {
        bx_panic("PUSHAD(): stack doesn't have enough room!\n");
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else {
      if (temp_ESP < 32)
        bx_panic("pushad: eSP < 32\n");
      }

    esp = ESP;

    /* ??? optimize this by using virtual write, all checks passed */
    push_32(EAX);
    push_32(ECX);
    push_32(EDX);
    push_32(EBX);
    push_32(esp);
    push_32(EBP);
    push_32(ESI);
    push_32(EDI);
#endif
}

  void
BX_CPU_C::POPAD32(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("POPAD not supported on an 8086\n");
#else /* 286+ */
    Bit32u edi, esi, ebp, etmp, ebx, edx, ecx, eax;

    if (protected_mode()) {
      if ( !can_pop(32) ) {
        bx_panic("pop_ad: not enough bytes on stack\n");
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    /* ??? optimize this */
    pop_32(&edi);
    pop_32(&esi);
    pop_32(&ebp);
    pop_32(&etmp); /* value for ESP discarded */
    pop_32(&ebx);
    pop_32(&edx);
    pop_32(&ecx);
    pop_32(&eax);

    EDI = edi;
    ESI = esi;
    EBP = ebp;
    EBX = ebx;
    EDX = edx;
    ECX = ecx;
    EAX = eax;
#endif
}

  void
BX_CPU_C::PUSH_Id(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("PUSH_Iv: not supported on 8086!\n");
#else

    Bit32u imm32;

    imm32 = i->Id;

    push_32(imm32);
#endif
}

  void
BX_CPU_C::PUSH_Ed(BxInstruction_t *i)
{
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    push_32(op1_32);
}


  void
BX_CPU_C::ENTER_IwIb(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("ENTER_IwIb: not supported by 8086!\n");
#else
  Bit32u frame_ptr32;
  Bit16u frame_ptr16;
  Bit8u level;

  level = i->Ib2;

  invalidate_prefetch_q();

  level %= 32;
/* ??? */
if (level) bx_panic("enter(): level > 0\n");
//if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b && i->os_32==0) {
//  bx_printf("enter(): stacksize!=opsize: I'm unsure of the code for this\n");
//  bx_panic("         The Intel manuals are a mess on this one!\n");
//  }

  if ( protected_mode() ) {
    Bit32u bytes_to_push, temp_ESP;

    if (level == 0) {
      if (i->os_32)
        bytes_to_push = 4 + i->Iw;
      else
        bytes_to_push = 2 + i->Iw;
      }
    else { /* level > 0 */
      if (i->os_32)
        bytes_to_push = 4 + (level-1)*4 + 4 + i->Iw;
      else
        bytes_to_push = 2 + (level-1)*2 + 2 + i->Iw;
      }
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      temp_ESP = ESP;
    else
      temp_ESP = SP;
    if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, bytes_to_push) ) {
      bx_panic("ENTER: not enough room on stack!\n");
      exception(BX_SS_EXCEPTION, 0, 0);
      }
    }

  if (i->os_32)
    push_32(EBP);
  else
    push_16(BP);

  // can just do frame_ptr32 = ESP for either case ???
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    frame_ptr32 = ESP;
  else
    frame_ptr32 = SP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      if (i->os_32) {
        Bit32u temp32;

        if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* 32bit stacksize */
          EBP -= 4;
          read_virtual_dword(BX_SEG_REG_SS, EBP, &temp32);
          ESP -= 4;
          write_virtual_dword(BX_SEG_REG_SS, ESP, &temp32);
          }
        else { /* 16bit stacksize */
          BP -= 4;
          read_virtual_dword(BX_SEG_REG_SS, BP, &temp32);
          SP -= 4;
          write_virtual_dword(BX_SEG_REG_SS, SP, &temp32);
          }
        }
      else { /* 16bit opsize */
        Bit16u temp16;

        if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* 32bit stacksize */
          EBP -= 2;
          read_virtual_word(BX_SEG_REG_SS, EBP, &temp16);
          ESP -= 2;
          write_virtual_word(BX_SEG_REG_SS, ESP, &temp16);
          }
        else { /* 16bit stacksize */
          BP -= 2;
          read_virtual_word(BX_SEG_REG_SS, BP, &temp16);
          SP -= 2;
          write_virtual_word(BX_SEG_REG_SS, SP, &temp16);
          }
        }
      } /* while (--level) */

    /* push(frame pointer) */
    if (i->os_32) {
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* 32bit stacksize */
        ESP -= 4;
        write_virtual_dword(BX_SEG_REG_SS, ESP, &frame_ptr32);
        }
      else {
        SP -= 4;
        write_virtual_dword(BX_SEG_REG_SS, SP, &frame_ptr32);
        }
      }
    else { /* 16bit opsize */
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* 32bit stacksize */
        frame_ptr16 = frame_ptr32;
        ESP -= 2;
        write_virtual_word(BX_SEG_REG_SS, ESP, &frame_ptr16);
        }
      else {
        frame_ptr16 = frame_ptr32;
        SP -= 2;
        write_virtual_word(BX_SEG_REG_SS, SP, &frame_ptr16);
        }
      }
    } /* if (level > 0) ... */

  if (i->os_32)
    EBP = frame_ptr32;
  else
    BP = frame_ptr32;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* 32bit stacksize */
    ESP = ESP - i->Iw;
    }
  else { /* 16bit stack */
    SP = SP - i->Iw;
    }
#endif
}

  void
BX_CPU_C::LEAVE(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  bx_panic("LEAVE: not supported by 8086!\n");
#else
  Bit32u temp_EBP;


  invalidate_prefetch_q();

#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_EBP = EBP;
  else
#endif
    temp_EBP = BP;

  if ( protected_mode() ) {
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed) { /* expand up */
      if (temp_EBP <= BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled) {
        bx_panic("LEAVE: BP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].limit\n");
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else { /* normal */
      if (temp_EBP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled) {
        bx_panic("LEAVE: BP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].limit\n");
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    }


  // delete frame
#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    ESP = EBP;
  else
#endif
    SP = BP;

  // restore frame pointer
#if BX_CPU_LEVEL >= 3
  if (i->os_32) {
    Bit32u temp32;

    pop_32(&temp32);
    EBP = temp32;
    }
  else
#endif
    {
    Bit16u temp16;

    pop_16(&temp16);
    BP = temp16;
    }
#endif
}
