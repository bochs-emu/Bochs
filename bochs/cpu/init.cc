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


/* the device id and stepping id are loaded into DH & DL upon processor
   startup.  for device id: 3 = 80386, 4 = 80486.  just make up a
   number for the stepping (revision) id. */
#define BX_DEVICE_ID     3
#define BX_STEPPING_ID   0

BX_CPU_C::BX_CPU_C()
#if BX_SUPPORT_APIC
   : local_apic (this)
#endif
{
  // in case of SMF, you cannot reference any member data
  // in the constructor because the only access to it is via
  // global variables which aren't initialized quite yet.
  put("CPU");
  settype (CPU0LOG);
}

void BX_CPU_C::init(BX_MEM_C *addrspace)
{
  BX_DEBUG(( "Init $Id: init.cc,v 1.12 2001-06-27 20:27:48 fries Exp $"));
  // BX_CPU_C constructor
  BX_CPU_THIS_PTR set_INTR (0);
#if BX_SUPPORT_APIC
  local_apic.init ();
#endif
  // in SMP mode, the prefix of the CPU will be changed to [CPUn] in 
  // bx_local_apic_c::set_id as soon as the apic ID is assigned.

  /* hack for the following fields.  Its easier to decode mod-rm bytes if
     you can assume there's always a base & index register used.  For
     modes which don't really use them, point to an empty (zeroed) register.
   */
  empty_register = 0;

  // 16bit address mode base register, used for mod-rm decoding

  _16bit_base_reg[0] = &gen_reg[BX_16BIT_REG_BX].word.rx;
  _16bit_base_reg[1] = &gen_reg[BX_16BIT_REG_BX].word.rx;
  _16bit_base_reg[2] = &gen_reg[BX_16BIT_REG_BP].word.rx;
  _16bit_base_reg[3] = &gen_reg[BX_16BIT_REG_BP].word.rx;
  _16bit_base_reg[4] = (Bit16u*) &empty_register;
  _16bit_base_reg[5] = (Bit16u*) &empty_register;
  _16bit_base_reg[6] = &gen_reg[BX_16BIT_REG_BP].word.rx;
  _16bit_base_reg[7] = &gen_reg[BX_16BIT_REG_BX].word.rx;

  // 16bit address mode index register, used for mod-rm decoding
  _16bit_index_reg[0] = &gen_reg[BX_16BIT_REG_SI].word.rx;
  _16bit_index_reg[1] = &gen_reg[BX_16BIT_REG_DI].word.rx;
  _16bit_index_reg[2] = &gen_reg[BX_16BIT_REG_SI].word.rx;
  _16bit_index_reg[3] = &gen_reg[BX_16BIT_REG_DI].word.rx;
  _16bit_index_reg[4] = &gen_reg[BX_16BIT_REG_SI].word.rx;
  _16bit_index_reg[5] = &gen_reg[BX_16BIT_REG_DI].word.rx;
  _16bit_index_reg[6] = (Bit16u*) &empty_register;
  _16bit_index_reg[7] = (Bit16u*) &empty_register;

  // for decoding instructions: access to seg reg's via index number
  sreg_mod00_rm16[0] = BX_SEG_REG_DS;
  sreg_mod00_rm16[1] = BX_SEG_REG_DS;
  sreg_mod00_rm16[2] = BX_SEG_REG_SS;
  sreg_mod00_rm16[3] = BX_SEG_REG_SS;
  sreg_mod00_rm16[4] = BX_SEG_REG_DS;
  sreg_mod00_rm16[5] = BX_SEG_REG_DS;
  sreg_mod00_rm16[6] = BX_SEG_REG_DS;
  sreg_mod00_rm16[7] = BX_SEG_REG_DS;

  sreg_mod01_rm16[0] = BX_SEG_REG_DS;
  sreg_mod01_rm16[1] = BX_SEG_REG_DS;
  sreg_mod01_rm16[2] = BX_SEG_REG_SS;
  sreg_mod01_rm16[3] = BX_SEG_REG_SS;
  sreg_mod01_rm16[4] = BX_SEG_REG_DS;
  sreg_mod01_rm16[5] = BX_SEG_REG_DS;
  sreg_mod01_rm16[6] = BX_SEG_REG_SS;
  sreg_mod01_rm16[7] = BX_SEG_REG_DS;

  sreg_mod10_rm16[0] = BX_SEG_REG_DS;
  sreg_mod10_rm16[1] = BX_SEG_REG_DS;
  sreg_mod10_rm16[2] = BX_SEG_REG_SS;
  sreg_mod10_rm16[3] = BX_SEG_REG_SS;
  sreg_mod10_rm16[4] = BX_SEG_REG_DS;
  sreg_mod10_rm16[5] = BX_SEG_REG_DS;
  sreg_mod10_rm16[6] = BX_SEG_REG_SS;
  sreg_mod10_rm16[7] = BX_SEG_REG_DS;

  // the default segment to use for a one-byte modrm with mod==01b
  // and rm==i
  //
  sreg_mod01_rm32[0] = BX_SEG_REG_DS;
  sreg_mod01_rm32[1] = BX_SEG_REG_DS;
  sreg_mod01_rm32[2] = BX_SEG_REG_DS;
  sreg_mod01_rm32[3] = BX_SEG_REG_DS;
  sreg_mod01_rm32[4] = BX_SEG_REG_NULL;
    // this entry should never be accessed
    // (escape to 2-byte)
  sreg_mod01_rm32[5] = BX_SEG_REG_SS;
  sreg_mod01_rm32[6] = BX_SEG_REG_DS;
  sreg_mod01_rm32[7] = BX_SEG_REG_DS;

  // the default segment to use for a one-byte modrm with mod==10b
  // and rm==i
  //
  sreg_mod10_rm32[0] = BX_SEG_REG_DS;
  sreg_mod10_rm32[1] = BX_SEG_REG_DS;
  sreg_mod10_rm32[2] = BX_SEG_REG_DS;
  sreg_mod10_rm32[3] = BX_SEG_REG_DS;
  sreg_mod10_rm32[4] = BX_SEG_REG_NULL;
    // this entry should never be accessed
    // (escape to 2-byte)
  sreg_mod10_rm32[5] = BX_SEG_REG_SS;
  sreg_mod10_rm32[6] = BX_SEG_REG_DS;
  sreg_mod10_rm32[7] = BX_SEG_REG_DS;


  // the default segment to use for a two-byte modrm with mod==00b
  // and base==i
  //
  sreg_mod0_base32[0] = BX_SEG_REG_DS;
  sreg_mod0_base32[1] = BX_SEG_REG_DS;
  sreg_mod0_base32[2] = BX_SEG_REG_DS;
  sreg_mod0_base32[3] = BX_SEG_REG_DS;
  sreg_mod0_base32[4] = BX_SEG_REG_SS;
  sreg_mod0_base32[5] = BX_SEG_REG_DS;
  sreg_mod0_base32[6] = BX_SEG_REG_DS;
  sreg_mod0_base32[7] = BX_SEG_REG_DS;

  // the default segment to use for a two-byte modrm with
  // mod==01b or mod==10b and base==i
  sreg_mod1or2_base32[0] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[1] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[2] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[3] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[4] = BX_SEG_REG_SS;
  sreg_mod1or2_base32[5] = BX_SEG_REG_SS;
  sreg_mod1or2_base32[6] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[7] = BX_SEG_REG_DS;

#if BX_DYNAMIC_TRANSLATION
  DTWrite8vShim = NULL;
  DTWrite16vShim = NULL;
  DTWrite32vShim = NULL;
  DTRead8vShim = NULL;
  DTRead16vShim = NULL;
  DTRead32vShim = NULL;
  DTReadRMW8vShim = (BxDTShim_t) DTASReadRMW8vShim;
  BX_DEBUG(( "DTReadRMW8vShim is %x", (unsigned) DTReadRMW8vShim ));
  BX_DEBUG(( "&DTReadRMW8vShim is %x", (unsigned) &DTReadRMW8vShim ));
  DTReadRMW16vShim = NULL;
  DTReadRMW32vShim = NULL;
  DTWriteRMW8vShim = (BxDTShim_t) DTASWriteRMW8vShim;
  DTWriteRMW16vShim = NULL;
  DTWriteRMW32vShim = NULL;
  DTSetFlagsOSZAPCPtr = (BxDTShim_t) DTASSetFlagsOSZAPC;
  DTIndBrHandler = (BxDTShim_t) DTASIndBrHandler;
  DTDirBrHandler = (BxDTShim_t) DTASDirBrHandler;
#endif

  mem = addrspace;
  sprintf (name, "CPU %p", this);

  BX_INSTR_INIT();
}


BX_CPU_C::~BX_CPU_C(void)
{
  BX_INSTR_SHUTDOWN();
  BX_DEBUG(( "Exit."));
}



  void
BX_CPU_C::reset(unsigned source)
{
  UNUSED(source); // either BX_RESET_HARDWARE or BX_RESET_SOFTWARE

  // general registers
  EAX = 0; // processor passed test :-)
  EBX = 0; // undefined
  ECX = 0; // undefined
  EDX = (BX_DEVICE_ID << 8) | BX_STEPPING_ID; // ???
  EBP = 0; // undefined
  ESI = 0; // undefined
  EDI = 0; // undefined
  ESP = 0; // undefined

  // all status flags at known values, use BX_CPU_THIS_PTR eflags structure
  BX_CPU_THIS_PTR lf_flags_status = 0x000000;
  BX_CPU_THIS_PTR lf_pf = 0;

  // status and control flags register set
  BX_CPU_THIS_PTR set_CF(0);
  BX_CPU_THIS_PTR eflags.bit1 = 1;
  BX_CPU_THIS_PTR set_PF(0);
  BX_CPU_THIS_PTR eflags.bit3 = 0;
  BX_CPU_THIS_PTR set_AF(0);
  BX_CPU_THIS_PTR eflags.bit5 = 0;
  BX_CPU_THIS_PTR set_ZF(0);
  BX_CPU_THIS_PTR set_SF(0);
  BX_CPU_THIS_PTR eflags.tf = 0;
  BX_CPU_THIS_PTR eflags.if_ = 0;
  BX_CPU_THIS_PTR eflags.df = 0;
  BX_CPU_THIS_PTR set_OF(0);
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR eflags.iopl = 0;
  BX_CPU_THIS_PTR eflags.nt = 0;
#endif
  BX_CPU_THIS_PTR eflags.bit15 = 0;
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR eflags.rf = 0;
  BX_CPU_THIS_PTR eflags.vm = 0;
#endif
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR eflags.ac = 0;
#endif

  BX_CPU_THIS_PTR inhibit_mask = 0;
  BX_CPU_THIS_PTR debug_trap = 0;

  /* instruction pointer */
#if BX_CPU_LEVEL < 2
  BX_CPU_THIS_PTR prev_eip =
  BX_CPU_THIS_PTR eip = 0x00000000;
#else /* from 286 up */
  BX_CPU_THIS_PTR prev_eip =
  BX_CPU_THIS_PTR eip = 0x0000FFF0;
#endif


  /* CS (Code Segment) and descriptor cache */
  /* Note: on a real cpu, CS initially points to upper memory.  After
   * the 1st jump, the descriptor base is zero'd out.  Since I'm just
   * going to jump to my BIOS, I don't need to do this.
   * For future reference:
   *   processor  cs.selector   cs.base    cs.limit    EIP
   *        8086    FFFF          FFFF0        FFFF   0000
   *        286     F000         FF0000        FFFF   FFF0
   *        386+    F000       FFFF0000        FFFF   FFF0
   */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value =     0xf000;
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable   = 1; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0x000F0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
#endif


  /* SS (Stack Segment) and descriptor cache */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value =     0x0000;
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl = 0;
#endif


  /* DS (Data Segment) and descriptor cache */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value =     0x0000;
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl = 0;
#endif


  /* ES (Extra Segment) and descriptor cache */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value =     0x0000;
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.avl = 0;
#endif


  /* FS and descriptor cache */
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit_scaled =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.avl = 0;
#endif


  /* GS and descriptor cache */
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.index =     0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.ti = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl = 0;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid =     1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.p = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.dpl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.segment = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.type = 3; /* read/write access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit        =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled =     0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b = 0; /* 16bit default size */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.avl = 0;
#endif


  /* GDTR (Global Descriptor Table Register) */
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR gdtr.base         = 0x00000000;  /* undefined */
  BX_CPU_THIS_PTR gdtr.limit        =     0x0000;  /* undefined */
  /* ??? AR=Present, Read/Write */
#endif

  /* IDTR (Interrupt Descriptor Table Register) */
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR idtr.base         = 0x00000000;
  BX_CPU_THIS_PTR idtr.limit        =     0x03FF; /* always byte granular */ /* ??? */
  /* ??? AR=Present, Read/Write */
#endif

  /* LDTR (Local Descriptor Table Register) */
#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR ldtr.selector.value =     0x0000;
  BX_CPU_THIS_PTR ldtr.selector.index =     0x0000;
  BX_CPU_THIS_PTR ldtr.selector.ti = 0;
  BX_CPU_THIS_PTR ldtr.selector.rpl = 0;

  BX_CPU_THIS_PTR ldtr.cache.valid   = 0; /* not valid */
  BX_CPU_THIS_PTR ldtr.cache.p       = 0; /* not present */
  BX_CPU_THIS_PTR ldtr.cache.dpl     = 0; /* field not used */
  BX_CPU_THIS_PTR ldtr.cache.segment = 0; /* system segment */
  BX_CPU_THIS_PTR ldtr.cache.type    = 2; /* LDT descriptor */

  BX_CPU_THIS_PTR ldtr.cache.u.ldt.base      = 0x00000000;
  BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit     =     0xFFFF;
#endif

  /* TR (Task Register) */
#if BX_CPU_LEVEL >= 2
  /* ??? I don't know what state the TR comes up in */
  BX_CPU_THIS_PTR tr.selector.value =     0x0000;
  BX_CPU_THIS_PTR tr.selector.index =     0x0000; /* undefined */
  BX_CPU_THIS_PTR tr.selector.ti    =     0;
  BX_CPU_THIS_PTR tr.selector.rpl   =     0;

  BX_CPU_THIS_PTR tr.cache.valid    = 0;
  BX_CPU_THIS_PTR tr.cache.p        = 0;
  BX_CPU_THIS_PTR tr.cache.dpl      = 0; /* field not used */
  BX_CPU_THIS_PTR tr.cache.segment  = 0;
  BX_CPU_THIS_PTR tr.cache.type     = 0; /* invalid */
  BX_CPU_THIS_PTR tr.cache.u.tss286.base             = 0x00000000; /* undefined */
  BX_CPU_THIS_PTR tr.cache.u.tss286.limit            =     0x0000; /* undefined */
#endif

  // DR0 - DR7 (Debug Registers)
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR dr0 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr1 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr2 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr3 = 0;   /* undefined */
#endif
#if   BX_CPU_LEVEL == 3
  BX_CPU_THIS_PTR dr6 = 0xFFFF1FF0;
  BX_CPU_THIS_PTR dr7 = 0x00000400;
#elif BX_CPU_LEVEL == 4
  BX_CPU_THIS_PTR dr6 = 0xFFFF1FF0;
  BX_CPU_THIS_PTR dr7 = 0x00000400;
#elif BX_CPU_LEVEL == 5
  BX_CPU_THIS_PTR dr6 = 0xFFFF0FF0;
  BX_CPU_THIS_PTR dr7 = 0x00000400;
#elif BX_CPU_LEVEL == 6
  BX_CPU_THIS_PTR dr6 = 0xFFFF0FF0;
  BX_CPU_THIS_PTR dr7 = 0x00000400;
#else
#  error "DR6,7: CPU > 6"
#endif

#if 0
  /* test registers 3-7 (unimplemented) */
  BX_CPU_THIS_PTR tr3 = 0;   /* undefined */
  BX_CPU_THIS_PTR tr4 = 0;   /* undefined */
  BX_CPU_THIS_PTR tr5 = 0;   /* undefined */
  BX_CPU_THIS_PTR tr6 = 0;   /* undefined */
  BX_CPU_THIS_PTR tr7 = 0;   /* undefined */
#endif

#if BX_CPU_LEVEL >= 2
  // MSW (Machine Status Word), so called on 286
  // CR0 (Control Register 0), so called on 386+
  BX_CPU_THIS_PTR cr0.ts = 0; // no task switch
  BX_CPU_THIS_PTR cr0.em = 0; // emulate math coprocessor
  BX_CPU_THIS_PTR cr0.mp = 0; // wait instructions not trapped
  BX_CPU_THIS_PTR cr0.pe = 0; // real mode
  BX_CPU_THIS_PTR cr0.val32 = 0;

#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR cr0.pg = 0; // paging disabled
  // no change to cr0.val32
#endif

#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR cr0.cd = 1; // caching disabled
  BX_CPU_THIS_PTR cr0.nw = 1; // not write-through
  BX_CPU_THIS_PTR cr0.am = 0; // disable alignment check
  BX_CPU_THIS_PTR cr0.wp = 0; // disable write-protect
  BX_CPU_THIS_PTR cr0.ne = 0; // ndp exceptions through int 13H, DOS compat
  BX_CPU_THIS_PTR cr0.val32 |= 0x60000000;
#endif

  // handle reserved bits
#if BX_CPU_LEVEL == 3
  // reserved bits all set to 1 on 386
  BX_CPU_THIS_PTR cr0.val32 |= 0x7ffffff0;
#elif BX_CPU_LEVEL >= 4
  // bit 4 is hardwired to 1 on all x86
  BX_CPU_THIS_PTR cr0.val32 |= 0x00000010;
#endif
#endif // CPU >= 2


#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR cr2 = 0;
  BX_CPU_THIS_PTR cr3 = 0;
#endif
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR cr4 = 0;
#endif



  BX_CPU_THIS_PTR EXT = 0;
  //BX_INTR = 0;

  TLB_init();

  BX_CPU_THIS_PTR bytesleft = 0;
  BX_CPU_THIS_PTR fetch_ptr = NULL;
  BX_CPU_THIS_PTR prev_linear_page = 0;
  BX_CPU_THIS_PTR prev_phy_page = 0;
  BX_CPU_THIS_PTR max_phy_addr = 0;

#if BX_DEBUGGER
#ifdef MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
  BX_CPU_THIS_PTR trace = 0;
#endif

  // Init the Floating Point Unit
  fpu_init();

#if BX_DYNAMIC_TRANSLATION
  dynamic_init();
#endif

#if (BX_SMP_PROCESSORS > 1)
  // notice if I'm the bootstrap processor.  If not, do the equivalent of
  // a HALT instruction.
  int apic_id = local_apic.get_id ();
  if (BX_BOOTSTRAP_PROCESSOR == apic_id)
  {
    // boot normally
    BX_INFO(("CPU[%d] is the bootstrap processor", apic_id));
  } else {
    // it's an application processor, halt until IPI is heard.
    BX_INFO(("CPU[%d] is an application processor. Halting until IPI.", apic_id));
    debug_trap |= 0x80000000;
    async_event = 1;
  }
#endif
}


  void
BX_CPU_C::sanity_checks(void)
{
  Bit8u al, cl, dl, bl, ah, ch, dh, bh;
  Bit16u ax, cx, dx, bx, sp, bp, si, di;
  Bit32u eax, ecx, edx, ebx, esp, ebp, esi, edi;

  EAX = 0xFFEEDDCC;
  ECX = 0xBBAA9988;
  EDX = 0x77665544;
  EBX = 0x332211FF;
  ESP = 0xEEDDCCBB;
  EBP = 0xAA998877;
  ESI = 0x66554433;
  EDI = 0x2211FFEE;

  al = AL;
  cl = CL;
  dl = DL;
  bl = BL;
  ah = AH;
  ch = CH;
  dh = DH;
  bh = BH;

  if ( al != (EAX & 0xFF) ||
       cl != (ECX & 0xFF) ||
       dl != (EDX & 0xFF) ||
       bl != (EBX & 0xFF) ||
       ah != ((EAX >> 8) & 0xFF) ||
       ch != ((ECX >> 8) & 0xFF) ||
       dh != ((EDX >> 8) & 0xFF) ||
       bh != ((EBX >> 8) & 0xFF) ) {
    BX_PANIC(("problems using BX_READ_8BIT_REG()!"));
    }

  ax = AX;
  cx = CX;
  dx = DX;
  bx = BX;
  sp = SP;
  bp = BP;
  si = SI;
  di = DI;

  if ( ax != (EAX & 0xFFFF) ||
       cx != (ECX & 0xFFFF) ||
       dx != (EDX & 0xFFFF) ||
       bx != (EBX & 0xFFFF) ||
       sp != (ESP & 0xFFFF) ||
       bp != (EBP & 0xFFFF) ||
       si != (ESI & 0xFFFF) ||
       di != (EDI & 0xFFFF) ) {
    BX_PANIC(("problems using BX_READ_16BIT_REG()!"));
    }


  eax = EAX;
  ecx = ECX;
  edx = EDX;
  ebx = EBX;
  esp = ESP;
  ebp = EBP;
  esi = ESI;
  edi = EDI;


  if (sizeof(Bit8u)  != 1  ||  sizeof(Bit8s)  != 1)
    BX_PANIC(("data type Bit8u or Bit8s is not of length 1 byte!"));
  if (sizeof(Bit16u) != 2  ||  sizeof(Bit16s) != 2)
    BX_PANIC(("data type Bit16u or Bit16s is not of length 2 bytes!"));
  if (sizeof(Bit32u) != 4  ||  sizeof(Bit32s) != 4)
    BX_PANIC(("data type Bit32u or Bit32s is not of length 4 bytes!"));

  BX_DEBUG(( "#(%u)all sanity checks passed!", BX_SIM_ID ));
}


  void
BX_CPU_C::set_INTR(Boolean value)
{
  BX_CPU_THIS_PTR INTR = value;
  BX_CPU_THIS_PTR async_event = 1;
}
