/////////////////////////////////////////////////////////////////////////
// $Id: init.cc,v 1.34 2002-09-29 16:59:28 sshwarts Exp $
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


#if BX_WITH_WX

#if BX_SMP_PROCESSORS!=1
#warning cpu_param_handler only supports parameters for one processor.
// To fix this, I think I will need to change bx_param_num_c::set_handler
// so that I pass in a void* data value.  The void* will be passed to each
// handler.  In this case, I would pass a pointer to the BX_CPU_C object
// in the void*, then in the handler I'd cast it back to BX_CPU_C and call
// BX_CPU_C::cpu_param_handler() which then could be a member function. -BBD
#endif

#define CASE_SEG_REG_GET(x) \
  case BXP_CPU_SEG_##x: \
    return BX_CPU(0)->sregs[BX_SEG_REG_##x].selector.value;
#define CASE_SEG_REG_SET(reg, val) \
  case BXP_CPU_SEG_##reg: \
    BX_CPU(0)->load_seg_reg (&BX_CPU(0)->sregs[BX_SEG_REG_##reg],val); \
    break;
#define CASE_LAZY_EFLAG_GET(flag) \
    case BXP_CPU_EFLAGS_##flag: \
      return BX_CPU(0)->get_##flag ();
#define CASE_LAZY_EFLAG_SET(flag, val) \
    case BXP_CPU_EFLAGS_##flag: \
      BX_CPU(0)->set_##flag(val); \
      break;
#define CASE_EFLAG_GET(flag) \
    case BXP_CPU_EFLAGS_##flag: \
      return BX_CPU(0)->get_##flag ();
#define CASE_EFLAG_SET(flag, val) \
    case BXP_CPU_EFLAGS_##flag: \
      BX_CPU(0)->set_##flag(val); \
      break;


// implement get/set handler for parameters that need unusual set/get
static Bit32s
cpu_param_handler (bx_param_c *param, int set, Bit32s val)
{
  bx_id id = param->get_id ();
  if (set) {
    switch (id) {
      CASE_SEG_REG_SET (CS, val);
      CASE_SEG_REG_SET (DS, val);
      CASE_SEG_REG_SET (SS, val);
      CASE_SEG_REG_SET (ES, val);
      CASE_SEG_REG_SET (FS, val);
      CASE_SEG_REG_SET (GS, val);
      case BXP_CPU_SEG_LDTR:
        BX_CPU(0)->panic("setting LDTR not implemented");
	break;
      case BXP_CPU_SEG_TR:
        BX_CPU(0)->panic ("setting TR not implemented");
	break;
      CASE_LAZY_EFLAG_SET (OF, val);
      CASE_LAZY_EFLAG_SET (SF, val);
      CASE_LAZY_EFLAG_SET (ZF, val);
      CASE_LAZY_EFLAG_SET (AF, val);
      CASE_LAZY_EFLAG_SET (PF, val);
      CASE_LAZY_EFLAG_SET (CF, val);
      CASE_EFLAG_SET (ID,   val);
      //CASE_EFLAG_SET (VIP,  val);
      //CASE_EFLAG_SET (VIF,  val);
      CASE_EFLAG_SET (AC,   val);
      CASE_EFLAG_SET (VM,   val);
      CASE_EFLAG_SET (RF,   val);
      CASE_EFLAG_SET (NT,   val);
      CASE_EFLAG_SET (IOPL, val);
      CASE_EFLAG_SET (DF,   val);
      CASE_EFLAG_SET (IF,   val);
      CASE_EFLAG_SET (TF,   val);
      default:
        BX_CPU(0)->panic ("cpu_param_handler set id %d not handled", id);
    }
  } else {
    switch (id) {
      CASE_SEG_REG_GET (CS);
      CASE_SEG_REG_GET (DS);
      CASE_SEG_REG_GET (SS);
      CASE_SEG_REG_GET (ES);
      CASE_SEG_REG_GET (FS);
      CASE_SEG_REG_GET (GS);
      case BXP_CPU_SEG_LDTR:
        return BX_CPU(0)->ldtr.selector.value;
        break;
      case BXP_CPU_SEG_TR:
        return BX_CPU(0)->tr.selector.value;
        break;
      CASE_LAZY_EFLAG_GET (OF);
      CASE_LAZY_EFLAG_GET (SF);
      CASE_LAZY_EFLAG_GET (ZF);
      CASE_LAZY_EFLAG_GET (AF);
      CASE_LAZY_EFLAG_GET (PF);
      CASE_LAZY_EFLAG_GET (CF);
      CASE_EFLAG_GET (ID);
      //CASE_EFLAG_GET (VIP);
      //CASE_EFLAG_GET (VIF);
      CASE_EFLAG_GET (AC);
      CASE_EFLAG_GET (VM);
      CASE_EFLAG_GET (RF);
      CASE_EFLAG_GET (NT);
      CASE_EFLAG_GET (IOPL);
      CASE_EFLAG_GET (DF);
      CASE_EFLAG_GET (IF);
      CASE_EFLAG_GET (TF);
      default:
        BX_CPU(0)->panic ("cpu_param_handler get id %d ('%s') not handled", id, param->get_name ());
    }
  }
  return val;
}
#undef CASE_SEG_REG_GET
#undef CASE_SEG_REG_SET

#endif

void BX_CPU_C::init(BX_MEM_C *addrspace)
{
  BX_DEBUG(( "Init $Id: init.cc,v 1.34 2002-09-29 16:59:28 sshwarts Exp $"));
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
#if BX_SUPPORT_X86_64
  sreg_mod01_rm32[8] = BX_SEG_REG_DS;
  sreg_mod01_rm32[9] = BX_SEG_REG_DS;
  sreg_mod01_rm32[10] = BX_SEG_REG_DS;
  sreg_mod01_rm32[11] = BX_SEG_REG_DS;
  sreg_mod01_rm32[12] = BX_SEG_REG_DS;
  sreg_mod01_rm32[13] = BX_SEG_REG_DS;
  sreg_mod01_rm32[14] = BX_SEG_REG_DS;
  sreg_mod01_rm32[15] = BX_SEG_REG_DS;
#endif

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
#if BX_SUPPORT_X86_64
  sreg_mod10_rm32[8] = BX_SEG_REG_DS;
  sreg_mod10_rm32[9] = BX_SEG_REG_DS;
  sreg_mod10_rm32[10] = BX_SEG_REG_DS;
  sreg_mod10_rm32[11] = BX_SEG_REG_DS;
  sreg_mod10_rm32[12] = BX_SEG_REG_DS;
  sreg_mod10_rm32[13] = BX_SEG_REG_DS;
  sreg_mod10_rm32[14] = BX_SEG_REG_DS;
  sreg_mod10_rm32[15] = BX_SEG_REG_DS;
#endif


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
#if BX_SUPPORT_X86_64
  sreg_mod0_base32[8] = BX_SEG_REG_DS;
  sreg_mod0_base32[9] = BX_SEG_REG_DS;
  sreg_mod0_base32[10] = BX_SEG_REG_DS;
  sreg_mod0_base32[11] = BX_SEG_REG_DS;
  sreg_mod0_base32[12] = BX_SEG_REG_DS;
  sreg_mod0_base32[13] = BX_SEG_REG_DS;
  sreg_mod0_base32[14] = BX_SEG_REG_DS;
  sreg_mod0_base32[15] = BX_SEG_REG_DS;
#endif

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
#if BX_SUPPORT_X86_64
  sreg_mod1or2_base32[8] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[9] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[10] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[11] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[12] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[13] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[14] = BX_SEG_REG_DS;
  sreg_mod1or2_base32[15] = BX_SEG_REG_DS;
#endif

  mem = addrspace;
  sprintf (name, "CPU %p", this);

#if BX_WITH_WX
  // Register some of the CPUs variables as shadow parameters so that
  // they can be visible in the config interface.
  // (Experimental, obviously not a complete list)
  bx_param_num_c *param;
  const char *fmt16 = "%04X";
  const char *fmt32 = "%08X";
  Bit32u oldbase = bx_param_num_c::set_default_base (16);
  const char *oldfmt = bx_param_num_c::set_default_format (fmt32);
  bx_list_c *list = new bx_list_c (BXP_CPU_PARAMETERS, "CPU State", "", 60);
#define DEFPARAM_NORMAL(name,field) \
  list->add (new bx_shadow_num_c (BXP_CPU_##name, #name, &(field)))


    DEFPARAM_NORMAL (EAX, EAX);
    DEFPARAM_NORMAL (EBX, EBX);
    DEFPARAM_NORMAL (ECX, ECX);
    DEFPARAM_NORMAL (EDX, EDX);
    DEFPARAM_NORMAL (ESP, ESP);
    DEFPARAM_NORMAL (EBP, EBP);
    DEFPARAM_NORMAL (ESI, ESI);
    DEFPARAM_NORMAL (EDI, EDI);
    DEFPARAM_NORMAL (EIP, EIP);
    DEFPARAM_NORMAL (DR0, dr0);
    DEFPARAM_NORMAL (DR1, dr1);
    DEFPARAM_NORMAL (DR2, dr2);
    DEFPARAM_NORMAL (DR3, dr3);
    DEFPARAM_NORMAL (DR6, dr6);
    DEFPARAM_NORMAL (DR7, dr7);
#if BX_SUPPORT_X86_64==0
#if BX_CPU_LEVEL >= 2
    DEFPARAM_NORMAL (CR0, cr0.val32);
    DEFPARAM_NORMAL (CR1, cr1);
    DEFPARAM_NORMAL (CR2, cr2);
    DEFPARAM_NORMAL (CR3, cr3);
#endif
#if BX_CPU_LEVEL >= 4
    DEFPARAM_NORMAL (CR4, cr4.registerValue);
#endif
#endif  // #if BX_SUPPORT_X86_64==0

  // segment registers require a handler function because they have
  // special get/set requirements.
#define DEFPARAM_SEG_REG(x) \
  list->add (param = new bx_param_num_c (BXP_CPU_SEG_##x, \
    #x, "", 0, 0xffff, 0)); \
  param->set_handler (cpu_param_handler); \
  param->set_format (fmt16);
#define DEFPARAM_GLOBAL_SEG_REG(name,field) \
  list->add (param = new bx_shadow_num_c (BXP_CPU_##name##_BASE,  \
        #name" base", \
	& BX_CPU_THIS_PTR field.base)); \
  list->add (param = new bx_shadow_num_c (BXP_CPU_##name##_LIMIT, \
        #name" limit", \
	& BX_CPU_THIS_PTR field.limit));

  DEFPARAM_SEG_REG(CS);
  DEFPARAM_SEG_REG(DS);
  DEFPARAM_SEG_REG(SS);
  DEFPARAM_SEG_REG(ES);
  DEFPARAM_SEG_REG(FS);
  DEFPARAM_SEG_REG(GS);
  DEFPARAM_SEG_REG(LDTR);
  DEFPARAM_SEG_REG(TR);
  DEFPARAM_GLOBAL_SEG_REG(GDTR, gdtr);
  DEFPARAM_GLOBAL_SEG_REG(IDTR, idtr);
#undef DEFPARAM_SEGREG

#if BX_SUPPORT_X86_64==0
  list->add (param = new bx_shadow_num_c (BXP_CPU_EFLAGS, "EFLAGS",
  &BX_CPU_THIS_PTR eflags.val32));
#endif

  // flags implemented in lazy_flags.cc must be done with a handler
  // that calls their get function, to force them to be computed.
#define DEFPARAM_EFLAG(name) \
  list->add ( \
      param = new bx_param_bool_c ( \
	BXP_CPU_EFLAGS_##name, \
        #name, "", get_##name())); \
  param->set_handler (cpu_param_handler);
#define DEFPARAM_LAZY_EFLAG(name) \
  list->add ( \
      param = new bx_param_bool_c ( \
	BXP_CPU_EFLAGS_##name, \
        #name, "", get_##name())); \
  param->set_handler (cpu_param_handler);

#if BX_CPU_LEVEL >= 4
  DEFPARAM_EFLAG(ID);
  //DEFPARAM_EFLAG(VIP);
  //DEFPARAM_EFLAG(VIF);
  DEFPARAM_EFLAG(AC);
#endif
#if BX_CPU_LEVEL >= 3
  DEFPARAM_EFLAG(VM);
  DEFPARAM_EFLAG(RF);
#endif
#if BX_CPU_LEVEL >= 2
  DEFPARAM_EFLAG(NT);
  // IOPL is a special case because it is 2 bits wide.
  list->add (
      param = new bx_shadow_num_c (
	BXP_CPU_EFLAGS_IOPL,
        "IOPL", "", 0, 3, 
	&eflags.val32,
	12, 13));
#if BX_SUPPORT_X86_64==0
  param->set_format ("%d");
#endif
#endif
  DEFPARAM_LAZY_EFLAG(OF);
  DEFPARAM_EFLAG(DF);
  DEFPARAM_EFLAG(IF);
  DEFPARAM_EFLAG(TF);
  DEFPARAM_LAZY_EFLAG(SF);
  DEFPARAM_LAZY_EFLAG(ZF);
  DEFPARAM_LAZY_EFLAG(AF);
  DEFPARAM_LAZY_EFLAG(PF);
  DEFPARAM_LAZY_EFLAG(CF);


  // restore defaults
  bx_param_num_c::set_default_base (oldbase);
  bx_param_num_c::set_default_format (oldfmt);
#endif

#if BX_SupportICache
  iCache.alloc(mem->len);
  iCache.fetchModeMask = 0; // KPL: fixme!!!
#endif
}


BX_CPU_C::~BX_CPU_C(void)
{
  BX_INSTR_SHUTDOWN(CPU_ID);
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

  // status and control flags register set
  BX_CPU_THIS_PTR setEFlags(0x2); // Bit1 is always set
  BX_CPU_THIS_PTR clear_IF ();
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR clear_RF ();
  BX_CPU_THIS_PTR clear_VM ();
#endif
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR clear_AC ();
#endif

  BX_CPU_THIS_PTR inhibit_mask = 0;
  BX_CPU_THIS_PTR debug_trap = 0;

  /* instruction pointer */
#if BX_CPU_LEVEL < 2
  BX_CPU_THIS_PTR prev_eip =
  EIP = 0x00000000;
#else /* from 286 up */
  BX_CPU_THIS_PTR prev_eip =
#if BX_SUPPORT_X86_64
  RIP = 0x0000FFF0;
#else
  EIP = 0x0000FFF0;
#endif
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
  BX_CPU_THIS_PTR cr4.setRegister(0);
#endif

#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32;
#endif


/* initialise MSR registers to defaults */
#if BX_CPU_LEVEL >= 5
  /* APIC Address, APIC enabled and BSP is default, we'll fill in the rest later */
  BX_CPU_THIS_PTR msr.apicbase = (APIC_BASE_ADDR << 12) + 0x900;
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR msr.lme = BX_CPU_THIS_PTR msr.lma = 0;
#endif
#endif

  BX_CPU_THIS_PTR EXT = 0;
  //BX_INTR = 0;

#if BX_SUPPORT_PAGING
#if BX_USE_TLB
  TLB_init();
#endif // BX_USE_TLB
#endif // BX_SUPPORT_PAGING

  BX_CPU_THIS_PTR eipPageBias = 0;
  BX_CPU_THIS_PTR eipPageWindowSize = 0;
  BX_CPU_THIS_PTR eipFetchPtr = NULL;

#if BX_DEBUGGER
#ifdef MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
  BX_CPU_THIS_PTR trace = 0;
#endif

  // Init the Floating Point Unit
  fpu_init();

#if (BX_SMP_PROCESSORS > 1)
  // notice if I'm the bootstrap processor.  If not, do the equivalent of
  // a HALT instruction.
  int apic_id = local_apic.get_id ();
  if (BX_BOOTSTRAP_PROCESSOR == apic_id)
  {
    // boot normally
    BX_CPU_THIS_PTR bsp = 1;
    BX_CPU_THIS_PTR msr.apicbase |= 0x0100;	/* set bit 8 BSP */
    BX_INFO(("CPU[%d] is the bootstrap processor", apic_id));
  } else {
    // it's an application processor, halt until IPI is heard.
    BX_CPU_THIS_PTR bsp = 0;
    BX_CPU_THIS_PTR msr.apicbase &= ~0x0100;	/* clear bit 8 BSP */
    BX_INFO(("CPU[%d] is an application processor. Halting until IPI.", apic_id));
    debug_trap |= 0x80000000;
    async_event = 1;
  }
#else
  BX_CPU_THIS_PTR async_event = 0;
#endif

  BX_INSTR_RESET(CPU_ID);
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
    BX_PANIC(("problems using BX_READ_8BIT_REGx()!"));
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
