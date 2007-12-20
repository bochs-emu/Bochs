////////////////////////////////////////////////////////////////////////
// $Id: ret_far.cc,v 1.12 2007-12-20 20:58:37 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#define RSP ESP
#endif


  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::return_protected(bxInstruction_c *i, Bit16u pop_bytes)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  bx_descriptor_t cs_descriptor, ss_descriptor;
  Bit32u stack_param_offset;
  bx_address return_RIP, return_RSP, temp_RSP;
  Bit32u dword1, dword2;

  /* + 6+N*2: SS      | +12+N*4:     SS | +24+N*8      SS */
  /* + 4+N*2: SP      | + 8+N*4:    ESP | +16+N*8     RSP */
  /*          parm N  | +        parm N | +        parm N */
  /*          parm 3  | +        parm 3 | +        parm 3 */
  /*          parm 2  | +        parm 2 | +        parm 2 */
  /* + 4:     parm 1  | + 8:     parm 1 | +16:     parm 1 */
  /* + 2:     CS      | + 4:         CS | + 8:         CS */
  /* + 0:     IP      | + 0:        EIP | + 0:        RIP */

#if BX_SUPPORT_X86_64
  if (StackAddrSize64()) temp_RSP = RSP;
  else 
#endif
  {
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) temp_RSP = ESP;
    else temp_RSP = SP;
  }

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    /* operand size=64: in long mode 1st and 2nd quadword on the stack 
       must be in canonical address space */

    raw_cs_selector = read_virtual_word (BX_SEG_REG_SS, temp_RSP + 8);
    return_RIP      = read_virtual_qword(BX_SEG_REG_SS, temp_RSP);

    stack_param_offset = 16;
  } 
  else
#endif
  if (i->os32L()) {
    /* operand size=32: 2nd dword on stack must be within stack limits,
     *   else #SS(0); */
    if (! can_pop(8))
    {
      BX_ERROR(("return_protected: 2rd dword not in stack limits"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }

    raw_cs_selector = read_virtual_word (BX_SEG_REG_SS, temp_RSP + 4);
    return_RIP      = read_virtual_dword(BX_SEG_REG_SS, temp_RSP);

    stack_param_offset = 8;
  }
  else {
    /* operand size=16: second word on stack must be within stack limits,
     *   else #SS(0); */
    if (! can_pop(4))
    {
      BX_ERROR(("return_protected: 2nd word not in stack limits"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }

    raw_cs_selector = read_virtual_word(BX_SEG_REG_SS, temp_RSP + 2);
    return_RIP      = read_virtual_word(BX_SEG_REG_SS, temp_RSP);

    stack_param_offset = 4;
  }

  // selector must be non-null else #GP(0)
  if ((raw_cs_selector & 0xfffc) == 0) {
    BX_ERROR(("return_protected: CS selector null"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  parse_selector(raw_cs_selector, &cs_selector);

  // selector index must be within its descriptor table limits,
  // else #GP(selector)
  fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);

  // descriptor AR byte must indicate code segment, else #GP(selector)
  parse_descriptor(dword1, dword2, &cs_descriptor);

  // return selector RPL must be >= CPL, else #GP(return selector)
  if (cs_selector.rpl < CPL) {
    BX_ERROR(("return_protected: CS.rpl < CPL"));
    exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
  }

  // check code-segment descriptor
  check_cs(&cs_descriptor, raw_cs_selector, 0, cs_selector.rpl);

  // if return selector RPL == CPL then
  // RETURN TO SAME PRIVILEGE LEVEL
  if (cs_selector.rpl == CPL)
  {
    BX_DEBUG(("return_protected: return to SAME PRIVILEGE LEVEL"));

    // top word on stack must be within stack limits, else #SS(0)
    if (! can_pop(stack_param_offset + pop_bytes)) {
      BX_ERROR(("return_protected: top word not in stack limits"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }

    branch_far64(&cs_selector, &cs_descriptor, return_RIP, CPL);

#if BX_SUPPORT_X86_64
    if (StackAddrSize64()) 
      RSP += stack_param_offset + pop_bytes;
    else 
#endif
    {
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
        ESP += stack_param_offset + pop_bytes;
      else
         SP += stack_param_offset + pop_bytes;
    }
    return;
  }
  /* RETURN TO OUTER PRIVILEGE LEVEL */
  else {
    /* + 6+N*2: SS      | +12+N*4:     SS | +24+N*8      SS */
    /* + 4+N*2: SP      | + 8+N*4:    ESP | +16+N*8     RSP */
    /*          parm N  | +        parm N | +        parm N */
    /*          parm 3  | +        parm 3 | +        parm 3 */
    /*          parm 2  | +        parm 2 | +        parm 2 */
    /* + 4:     parm 1  | + 8:     parm 1 | +16:     parm 1 */
    /* + 2:     CS      | + 4:         CS | + 8:         CS */
    /* + 0:     IP      | + 0:        EIP | + 0:        RIP */

#if BX_SUPPORT_X86_64
    if (i->os64L()) {
      /* top 32+immediate bytes on stack must be within stack limits, else #SS(0) */
      if (! can_pop(32 + pop_bytes)) {
        BX_ERROR(("return_protected: 32 bytes not within stack limits"));
        exception(BX_SS_EXCEPTION, 0, 0);
      }

      raw_ss_selector = read_virtual_word (BX_SEG_REG_SS, temp_RSP + 24 + pop_bytes);
      return_RSP      = read_virtual_qword(BX_SEG_REG_SS, temp_RSP + 16 + pop_bytes);
    }
    else
#endif
    if (i->os32L()) {
      /* top 16+immediate bytes on stack must be within stack limits, else #SS(0) */
      if (! can_pop(16 + pop_bytes)) {
        BX_ERROR(("return_protected: 16 bytes not within stack limits"));
        exception(BX_SS_EXCEPTION, 0, 0);
      }

      raw_ss_selector = read_virtual_word (BX_SEG_REG_SS, temp_RSP + 12 + pop_bytes);
      return_RSP      = read_virtual_dword(BX_SEG_REG_SS, temp_RSP +  8 + pop_bytes);
    }
    else {
      /* top 8+immediate bytes on stack must be within stack limits, else #SS(0) */
      if (! can_pop(8 + pop_bytes)) {
        BX_ERROR(("return_protected: 8 bytes not within stack limits"));
        exception(BX_SS_EXCEPTION, 0, 0);
      }

      raw_ss_selector = read_virtual_word(BX_SEG_REG_SS, temp_RSP + 6 + pop_bytes);
      return_RSP      = read_virtual_word(BX_SEG_REG_SS, temp_RSP + 4 + pop_bytes);
    }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    parse_selector(raw_ss_selector, &ss_selector);

    if ((raw_ss_selector & 0xfffc) == 0) {
      if (long_mode()) {
        if (! IS_LONG64_SEGMENT(cs_descriptor) || (cs_selector.rpl == 3)) {
          BX_ERROR(("return_protected: SS selector null"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }
      }
      else // not in long or compatibility mode
      {
        BX_ERROR(("return_protected: SS selector null"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
    }

    fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);

    /* selector RPL must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_selector.rpl != cs_selector.rpl) {
      BX_ERROR(("return_protected: ss.rpl != cs.rpl"));
      exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
    }

    /* descriptor AR byte must indicate a writable data segment,
     * else #GP(selector) */
    if (ss_descriptor.valid==0 || ss_descriptor.segment==0 ||
         IS_CODE_SEGMENT(ss_descriptor.type) ||
        !IS_DATA_SEGMENT_WRITEABLE(ss_descriptor.type))
    {
      BX_ERROR(("return_protected: SS.AR byte not writable data"));
      exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
    }

    /* descriptor dpl must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_descriptor.dpl != cs_selector.rpl) {
      BX_ERROR(("return_protected: SS.dpl != cs.rpl"));
      exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
    }

    /* segment must be present else #SS(selector) */
    if (! IS_PRESENT(ss_descriptor)) {
      BX_ERROR(("return_protected: ss.present == 0"));
      exception(BX_SS_EXCEPTION, raw_ss_selector & 0xfffc, 0);
    }

    branch_far64(&cs_selector, &cs_descriptor, return_RIP, cs_selector.rpl);

    /* load SS:SP from stack */
    /* load SS-cache with return SS descriptor */
    load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);

#if BX_SUPPORT_X86_64
    if (StackAddrSize64()) RSP = return_RSP + pop_bytes;
    else 
#endif
    if (ss_descriptor.u.segment.d_b)
      ESP = (Bit32u) return_RSP + pop_bytes;
    else
      SP  = (Bit16u) return_RSP + pop_bytes;

    /* check ES, DS, FS, GS for validity */
    validate_seg_regs();
  }
}
