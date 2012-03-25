////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2005-2012 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

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
  if (long64_mode()) temp_RSP = RSP;
  else
#endif
  {
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) temp_RSP = ESP;
    else temp_RSP = SP;
  }

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    raw_cs_selector = (Bit16u) stack_read_qword(temp_RSP + 8);
    return_RIP      =          stack_read_qword(temp_RSP);
    stack_param_offset = 16;
  }
  else
#endif
  if (i->os32L()) {
    raw_cs_selector = (Bit16u) stack_read_dword(temp_RSP + 4);
    return_RIP      =          stack_read_dword(temp_RSP);
    stack_param_offset = 8;
  }
  else {
    raw_cs_selector = stack_read_word(temp_RSP + 2);
    return_RIP      = stack_read_word(temp_RSP);
    stack_param_offset = 4;
  }

  // selector must be non-null else #GP(0)
  if ((raw_cs_selector & 0xfffc) == 0) {
    BX_ERROR(("return_protected: CS selector null"));
    exception(BX_GP_EXCEPTION, 0);
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
    exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc);
  }

  // check code-segment descriptor
  check_cs(&cs_descriptor, raw_cs_selector, 0, cs_selector.rpl);

  // if return selector RPL == CPL then
  // RETURN TO SAME PRIVILEGE LEVEL
  if (cs_selector.rpl == CPL)
  {
    BX_DEBUG(("return_protected: return to SAME PRIVILEGE LEVEL"));

    branch_far64(&cs_selector, &cs_descriptor, return_RIP, CPL);

#if BX_SUPPORT_X86_64
    if (long64_mode())
      RSP += stack_param_offset + pop_bytes;
    else
#endif
    {
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
        RSP = ESP + stack_param_offset + pop_bytes;
      else
         SP += stack_param_offset + pop_bytes;
    }
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

    BX_DEBUG(("return_protected: return to OUTER PRIVILEGE LEVEL"));

#if BX_SUPPORT_X86_64
    if (i->os64L()) {
      raw_ss_selector = stack_read_word(temp_RSP + 24 + pop_bytes);
      return_RSP      = stack_read_qword(temp_RSP + 16 + pop_bytes);
    }
    else
#endif
    if (i->os32L()) {
      raw_ss_selector = stack_read_word(temp_RSP + 12 + pop_bytes);
      return_RSP      = stack_read_dword(temp_RSP + 8 + pop_bytes);
    }
    else {
      raw_ss_selector = stack_read_word(temp_RSP + 6 + pop_bytes);
      return_RSP      = stack_read_word(temp_RSP + 4 + pop_bytes);
    }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    parse_selector(raw_ss_selector, &ss_selector);

    if ((raw_ss_selector & 0xfffc) == 0) {
#if BX_SUPPORT_X86_64
      if (long_mode()) {
        if (! IS_LONG64_SEGMENT(cs_descriptor) || (cs_selector.rpl == 3)) {
          BX_ERROR(("return_protected: SS selector null"));
          exception(BX_GP_EXCEPTION, 0);
        }
      }
      else // not in long or compatibility mode
#endif
      {
        BX_ERROR(("return_protected: SS selector null"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
    else {
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
      parse_descriptor(dword1, dword2, &ss_descriptor);

      /* selector RPL must = RPL of the return CS selector,
       * else #GP(selector) */
      if (ss_selector.rpl != cs_selector.rpl) {
        BX_ERROR(("return_protected: ss.rpl != cs.rpl"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc);
      }

      /* descriptor AR byte must indicate a writable data segment,
       * else #GP(selector) */
      if (ss_descriptor.valid==0 || ss_descriptor.segment==0 ||
           IS_CODE_SEGMENT(ss_descriptor.type) ||
          !IS_DATA_SEGMENT_WRITEABLE(ss_descriptor.type))
      {
        BX_ERROR(("return_protected: SS.AR byte not writable data"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc);
      }

      /* descriptor dpl must = RPL of the return CS selector,
       * else #GP(selector) */
      if (ss_descriptor.dpl != cs_selector.rpl) {
        BX_ERROR(("return_protected: SS.dpl != cs.rpl"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc);
      }

      /* segment must be present else #SS(selector) */
      if (! IS_PRESENT(ss_descriptor)) {
        BX_ERROR(("return_protected: ss.present == 0"));
        exception(BX_SS_EXCEPTION, raw_ss_selector & 0xfffc);
      }
    }

    branch_far64(&cs_selector, &cs_descriptor, return_RIP, cs_selector.rpl);

    if ((raw_ss_selector & 0xfffc) != 0) {
      // load SS:RSP from stack
      // load the SS-cache with SS descriptor
      load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
    }
#if BX_SUPPORT_X86_64
    else {
      // we are in 64-bit mode (checked above)
      load_null_selector(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], raw_ss_selector);
    }

    if (long64_mode())
      RSP = return_RSP + pop_bytes;
    else
#endif
    {
      if (ss_descriptor.u.segment.d_b)
        RSP = (Bit32u)(return_RSP + pop_bytes);
      else
        SP  = (Bit16u)(return_RSP + pop_bytes);
    }

    /* check ES, DS, FS, GS for validity */
    validate_seg_regs();
  }
}
