/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

//
// Notes:
//
// The high bits of the 32bit eip image are ignored by
// the IRET to VM.  The high bits of the 32bit esp image
// are loaded into ESP.  A subsequent push uses
// only the low 16bits since it's in VM.  In neither case
// did a protection fault occur during actual tests.  This
// is contrary to the Intel docs which claim a #GP for
// eIP out of code limits.
//
// IRET to VM does affect IOPL, IF, VM, and RF
//

#if BX_CPU_LEVEL >= 3

void BX_CPU_C::stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32)
{
  Bit32u temp_ESP, new_esp;
  Bit16u raw_es_selector, raw_ds_selector, raw_fs_selector,
         raw_gs_selector, raw_ss_selector;

  // Must be 32bit effective opsize, VM is set in upper 16bits of eFLAGS
  // and CPL = 0 to get here

  // ----------------
  // |     | OLD GS | eSP+32
  // |     | OLD FS | eSP+28
  // |     | OLD DS | eSP+24
  // |     | OLD ES | eSP+20
  // |     | OLD SS | eSP+16
  // |  OLD ESP     | eSP+12
  // | OLD EFLAGS   | eSP+8
  // |     | OLD CS | eSP+4
  // |  OLD EIP     | eSP+0
  // ----------------

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;

  // load SS:ESP from stack
  new_esp         =          stack_read_dword(temp_ESP+12);
  raw_ss_selector = (Bit16u) stack_read_dword(temp_ESP+16);

  // load ES,DS,FS,GS from stack
  raw_es_selector = (Bit16u) stack_read_dword(temp_ESP+20);
  raw_ds_selector = (Bit16u) stack_read_dword(temp_ESP+24);
  raw_fs_selector = (Bit16u) stack_read_dword(temp_ESP+28);
  raw_gs_selector = (Bit16u) stack_read_dword(temp_ESP+32);

  writeEFlags(flags32, EFlagsValidMask);

  // load CS:IP from stack; already read and passed as args
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = raw_cs_selector;
  EIP = new_eip & 0xffff;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = raw_es_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = raw_ds_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = raw_fs_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = raw_gs_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = raw_ss_selector;
  ESP = new_esp;	// full 32 bit are loaded

  init_v8086_mode();
}

#if BX_CPU_LEVEL >= 5
  #define BX_CR4_VME_ENABLED (BX_CPU_THIS_PTR cr4.get_VME())
#else
  #define BX_CR4_VME_ENABLED (0)
#endif

void BX_CPU_C::iret16_stack_return_from_v86(bxInstruction_c *i)
{
  if ((BX_CPU_THIS_PTR get_IOPL() < 3) && (BX_CR4_VME_ENABLED == 0)) {
    // trap to virtual 8086 monitor
    BX_DEBUG(("IRET in vm86 with IOPL != 3, VME = 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit16u ip, cs_raw, flags16;

  ip      = pop_16();
  cs_raw  = pop_16();
  flags16 = pop_16();

#if BX_CPU_LEVEL >= 5
  if (BX_CPU_THIS_PTR cr4.get_VME() && BX_CPU_THIS_PTR get_IOPL() < 3)
  {
    if (((flags16 & EFlagsIFMask) && BX_CPU_THIS_PTR get_VIP()) ||
         (flags16 & EFlagsTFMask))
    {
      BX_DEBUG(("iret16_stack_return_from_v86(): #GP(0) in VME mode"));
      exception(BX_GP_EXCEPTION, 0);
    }

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = (Bit32u) ip;

    // IF, IOPL unchanged, EFLAGS.VIF = TMP_FLAGS.IF
    Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask |
                            EFlagsDFMask | EFlagsNTMask | EFlagsVIFMask;
    Bit32u flags32 = (Bit32u) flags16;
    if (flags16 & EFlagsIFMask) flags32 |= EFlagsVIFMask;
    writeEFlags(flags32, changeMask);

    return;
  }
#endif

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
  EIP = (Bit32u) ip;
  write_flags(flags16, /*IOPL*/ 0, /*IF*/ 1);
}

void BX_CPU_C::iret32_stack_return_from_v86(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR get_IOPL() < 3) {
    // trap to virtual 8086 monitor
    BX_DEBUG(("IRET in vm86 with IOPL != 3, VME = 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u eip, cs_raw, flags32;
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u change_mask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsIFMask
                         | EFlagsDFMask | EFlagsNTMask | EFlagsRFMask;

#if BX_CPU_LEVEL >= 4
  change_mask |= (EFlagsIDMask | EFlagsACMask);  // ID/AC
#endif

  eip     = pop_32();
  cs_raw  = pop_32();
  flags32 = pop_32();

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) cs_raw);
  EIP = eip;
  // VIF, VIP, VM, IOPL unchanged
  writeEFlags(flags32, change_mask);
}

int BX_CPU_C::v86_redirect_interrupt(Bit8u vector)
{
#if BX_CPU_LEVEL >= 5
  if (BX_CPU_THIS_PTR cr4.get_VME())
  {
    bx_address tr_base = BX_CPU_THIS_PTR tr.cache.u.segment.base;
    if (BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled < 103) {
      BX_ERROR(("v86_redirect_interrupt(): TR.limit < 103 in VME"));
      exception(BX_GP_EXCEPTION, 0);
    }

    Bit32u io_base = system_read_word(tr_base + 102), offset = io_base - 32 + (vector >> 3);
    if (offset > BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled) {
      BX_ERROR(("v86_redirect_interrupt(): failed to fetch VME redirection bitmap"));
      exception(BX_GP_EXCEPTION, 0);
    }

    Bit8u vme_redirection_bitmap = system_read_byte(tr_base + offset);
    if (!(vme_redirection_bitmap & (1 << (vector & 7))))
    {
      // redirect interrupt through virtual-mode idt
      Bit16u temp_flags = (Bit16u) read_eflags();

      Bit16u temp_CS = system_read_word(vector*4 + 2);
      Bit16u temp_IP = system_read_word(vector*4);

      if (BX_CPU_THIS_PTR get_IOPL() < 3) {
        temp_flags |= EFlagsIOPLMask;
        if (BX_CPU_THIS_PTR get_VIF())
          temp_flags |=  EFlagsIFMask;
        else
          temp_flags &= ~EFlagsIFMask;
      }

      Bit16u old_IP = IP;
      Bit16u old_CS = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

      push_16(temp_flags);
      // push return address onto new stack
      push_16(old_CS);
      push_16(old_IP);

      load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) temp_CS);
      EIP = temp_IP;

      BX_CPU_THIS_PTR clear_TF();
      BX_CPU_THIS_PTR clear_RF();
      if (BX_CPU_THIS_PTR get_IOPL() == 3)
        BX_CPU_THIS_PTR clear_IF();
      else
        BX_CPU_THIS_PTR clear_VIF();

      return 1;
    }
  }
#endif
  // interrupt is not redirected or VME is OFF
  if (BX_CPU_THIS_PTR get_IOPL() < 3)
  {
    BX_DEBUG(("v86_redirect_interrupt(): interrupt cannot be redirected, generate #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  return 0;
}

void BX_CPU_C::init_v8086_mode(void)
{
  for(unsigned sreg = 0; sreg < 6; sreg++) {
    BX_CPU_THIS_PTR sregs[sreg].cache.valid   = SegValidCache | SegAccessROK | SegAccessWOK;
    BX_CPU_THIS_PTR sregs[sreg].cache.p       = 1;
    BX_CPU_THIS_PTR sregs[sreg].cache.dpl     = 3;
    BX_CPU_THIS_PTR sregs[sreg].cache.segment = 1;
    BX_CPU_THIS_PTR sregs[sreg].cache.type    = BX_DATA_READ_WRITE_ACCESSED;

    BX_CPU_THIS_PTR sregs[sreg].cache.u.segment.base =
        BX_CPU_THIS_PTR sregs[sreg].selector.value << 4;
    BX_CPU_THIS_PTR sregs[sreg].cache.u.segment.limit_scaled = 0xffff;
    BX_CPU_THIS_PTR sregs[sreg].cache.u.segment.g            = 0;
    BX_CPU_THIS_PTR sregs[sreg].cache.u.segment.d_b          = 0;
    BX_CPU_THIS_PTR sregs[sreg].cache.u.segment.avl          = 0;
    BX_CPU_THIS_PTR sregs[sreg].selector.rpl                 = 3;
  }

  handleCpuModeChange();

#if BX_CPU_LEVEL >= 4
  handleAlignmentCheck(/* CPL change */);
#endif

  invalidate_stack_cache();
}

#endif /* BX_CPU_LEVEL >= 3 */
