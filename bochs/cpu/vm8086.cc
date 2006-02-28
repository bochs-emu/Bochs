/////////////////////////////////////////////////////////////////////////
// $Id: vm8086.cc,v 1.24 2006-02-28 17:47:33 sshwarts Exp $
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging 64&32-bit code.
#define RIP EIP
#define RSP ESP
#endif


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


#if BX_SUPPORT_V8086_MODE

#if BX_CPU_LEVEL >= 3

void BX_CPU_C::stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector,
                              Bit32u flags32)
{
  Bit32u temp_ESP, new_esp, esp_laddr;
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

  // top 36 bytes of stack must be within stack limits, else #SS(0)
  if ( !can_pop(36) ) {
    BX_INFO(("iret: VM: top 36 bytes not within limits"));
    exception(BX_SS_EXCEPTION, 0, 0);
  }

  esp_laddr = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP;

  // load SS:ESP from stack
  access_linear(esp_laddr + 12, 4, 0, BX_READ, &new_esp);
  access_linear(esp_laddr + 16, 2, 0, BX_READ, &raw_ss_selector);

  // load ES,DS,FS,GS from stack
  access_linear(esp_laddr + 20, 2, 0, BX_READ, &raw_es_selector);
  access_linear(esp_laddr + 24, 2, 0, BX_READ, &raw_ds_selector);
  access_linear(esp_laddr + 28, 2, 0, BX_READ, &raw_fs_selector);
  access_linear(esp_laddr + 32, 2, 0, BX_READ, &raw_gs_selector);

  writeEFlags(flags32, EFlagsValidMask);

  // load CS:IP from stack; already read and passed as args
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = raw_cs_selector;
  EIP = new_eip & 0xffff;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = raw_es_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = raw_ds_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = raw_fs_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = raw_gs_selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = raw_ss_selector;
  RSP = new_esp;	// full 32 bit are loaded

  init_v8086_mode();
}

void BX_CPU_C::iret16_stack_return_from_v86(bxInstruction_c *i)
{
  if ((BX_CPU_THIS_PTR get_IOPL() < 3) && (CR4_VME_ENABLED == 0)) {
    // trap to virtual 8086 monitor
    BX_DEBUG(("IRET in vm86 with IOPL != 3, VME = 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit16u ip, cs_raw, flags16;

  if( !can_pop(6) )
  {
    exception(BX_SS_EXCEPTION, 0, 0);
    return;
  }

  pop_16(&ip);
  pop_16(&cs_raw);
  pop_16(&flags16);

#if BX_SUPPORT_VME
  if (CR4_VME_ENABLED && BX_CPU_THIS_PTR get_IOPL() < 3)
  {
    if (((flags16 & EFlagsIFMask) && BX_CPU_THIS_PTR get_VIP()) || 
         (flags16 & EFlagsTFMask))
    {
      BX_DEBUG(("iret16_stack_return_from_v86: #GP(0) in VME mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    RIP = (Bit32u) ip;

    // IF, IOPL unchanged, EFLAGS.VIF = TMP_FLAGS.IF
    Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | 
                            EFlagsDFMask | EFlagsNTMask | EFlagsVIFMask;
    Bit32u flags32 = (Bit32u) flags16;
    if (BX_CPU_THIS_PTR get_IF()) flags32 |= EFlagsVIFMask;
    writeEFlags(flags32, changeMask);

    return;
  }
#endif

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
  RIP = (Bit32u) ip;
  write_flags(flags16, /*IOPL*/ 0, /*IF*/ 1);
}

void BX_CPU_C::iret32_stack_return_from_v86(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR get_IOPL() < 3) {
    // trap to virtual 8086 monitor
    BX_DEBUG(("IRET in vm86 with IOPL != 3, VME = 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit32u eip, cs_raw, flags32;
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u change_mask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsIFMask 
                         | EFlagsDFMask | EFlagsNTMask | EFlagsRFMask;

#if BX_CPU_LEVEL >= 4
  change_mask |= (EFlagsIDMask | EFlagsACMask);  // ID/AC
#endif

  if( !can_pop(12) )
  {
    exception(BX_SS_EXCEPTION, 0, 0);
    return;
  }
  
  pop_32(&eip);
  pop_32(&cs_raw);
  pop_32(&flags32);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) cs_raw);
  RIP = eip;
  // VIF, VIP, VM, IOPL unchanged
  writeEFlags(flags32, change_mask);
}

#if BX_SUPPORT_VME
void BX_CPU_C::v86_redirect_interrupt(Bit32u vector)
{
  Bit16u temp_IP, temp_CS, temp_flags = read_flags();

  access_linear(vector*4,     2, 0, BX_READ, &temp_IP);
  access_linear(vector*4 + 2, 2, 0, BX_READ, &temp_CS);

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
  RIP = temp_IP;

  BX_CPU_THIS_PTR clear_TF ();
  BX_CPU_THIS_PTR clear_RF ();
  if (BX_CPU_THIS_PTR get_IOPL() == 3)
    BX_CPU_THIS_PTR clear_IF ();
  else
    BX_CPU_THIS_PTR clear_VIF();
}
#endif

void BX_CPU_C::init_v8086_mode(void)
{
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable   = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl                 = 3;

#if BX_SUPPORT_ICACHE  // update instruction cache
  BX_CPU_THIS_PTR fetchModeMask = createFetchModeMask(BX_CPU_THIS);
#endif

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl                 = 3;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.executable   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl                 = 3;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.executable   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl                 = 3;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.executable   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl                 = 3;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid                  = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.p                      = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.dpl                    = 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.segment                = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.executable   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.c_ed         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.r_w          = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.a            = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base         =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g            = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.avl          = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl                 = 3;
}

#endif /* BX_CPU_LEVEL >= 3 */

#else  // BX_SUPPORT_V8086_MODE

// compiled without v8086 mode support

void BX_CPU_C::stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32)
{
  BX_PANIC(("stack_return_to_v86: virtual 8086 mode not supported !"));
}

void BX_CPU_C::stack_return_from_v86(void)
{
  BX_PANIC(("stack_return_from_v86: virtual 8086 mode not supported !"));
}

#endif // BX_SUPPORT_V8086_MODE
