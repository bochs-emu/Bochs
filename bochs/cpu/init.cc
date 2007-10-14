/////////////////////////////////////////////////////////////////////////
// $Id: init.cc,v 1.137 2007-10-14 00:22:07 sshwarts Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#endif


BX_CPU_C::BX_CPU_C(unsigned id): bx_cpuid(id)
#if BX_SUPPORT_APIC
   ,local_apic (this)
#endif
{
  // in case of SMF, you cannot reference any member data
  // in the constructor because the only access to it is via
  // global variables which aren't initialized quite yet.
  put("CPU");
  settype (CPU0LOG);
}

#if BX_WITH_WX

#define IF_SEG_REG_GET(x) \
  if (!strcmp(param->get_name(), #x)) { \
    return BX_CPU(cpu)->sregs[BX_SEG_REG_##x].selector.value; \
  }
#define IF_SEG_REG_SET(reg, val) \
  if (!strcmp(param->get_name(), #reg)) { \
    BX_CPU(cpu)->load_seg_reg(&BX_CPU(cpu)->sregs[BX_SEG_REG_##reg],val); \
  }
#define IF_LAZY_EFLAG_GET(flag) \
    if (!strcmp(param->get_name(), #flag)) { \
      return BX_CPU(cpu)->get_##flag(); \
    }
#define IF_LAZY_EFLAG_SET(flag, val) \
    if (!strcmp(param->get_name(), #flag)) { \
      BX_CPU(cpu)->set_##flag(val); \
    }
#define IF_EFLAG_GET(flag) \
    if (!strcmp(param->get_name(), #flag)) { \
      return BX_CPU(cpu)->get_##flag(); \
    }
#define IF_EFLAG_SET(flag, val) \
    if (!strcmp(param->get_name(), #flag)) { \
      BX_CPU(cpu)->set_##flag(val); \
    }


// implement get/set handler for parameters that need unusual set/get
static Bit64s cpu_param_handler(bx_param_c *param, int set, Bit64s val)
{
#if BX_SUPPORT_SMP
  int cpu = atoi(param->get_parent()->get_name());
#endif
  if (set) {
    if (!strcmp(param->get_name(), "LDTR")) {
      BX_CPU(cpu)->panic("setting LDTR not implemented");
    }
    if (!strcmp(param->get_name(), "TR")) {
      BX_CPU(cpu)->panic("setting LDTR not implemented");
    }
    IF_SEG_REG_SET(CS, val);
    IF_SEG_REG_SET(DS, val);
    IF_SEG_REG_SET(SS, val);
    IF_SEG_REG_SET(ES, val);
    IF_SEG_REG_SET(FS, val);
    IF_SEG_REG_SET(GS, val);
    IF_LAZY_EFLAG_SET(OF, val);
    IF_LAZY_EFLAG_SET(SF, val);
    IF_LAZY_EFLAG_SET(ZF, val);
    IF_LAZY_EFLAG_SET(AF, val);
    IF_LAZY_EFLAG_SET(PF, val);
    IF_LAZY_EFLAG_SET(CF, val);
    IF_EFLAG_SET(ID,   val);
    IF_EFLAG_SET(VIP,  val);
    IF_EFLAG_SET(VIF,  val);
    IF_EFLAG_SET(AC,   val);
    IF_EFLAG_SET(VM,   val);
    IF_EFLAG_SET(RF,   val);
    IF_EFLAG_SET(NT,   val);
    IF_EFLAG_SET(IOPL, val);
    IF_EFLAG_SET(DF,   val);
    IF_EFLAG_SET(IF,   val);
    IF_EFLAG_SET(TF,   val);
  } else {
    if (!strcmp(param->get_name(), "LDTR")) {
      return BX_CPU(cpu)->ldtr.selector.value;
    }
    if (!strcmp(param->get_name(), "TR")) {
      return BX_CPU(cpu)->tr.selector.value;
    }
    IF_SEG_REG_GET (CS);
    IF_SEG_REG_GET (DS);
    IF_SEG_REG_GET (SS);
    IF_SEG_REG_GET (ES);
    IF_SEG_REG_GET (FS);
    IF_SEG_REG_GET (GS);
    IF_LAZY_EFLAG_GET(OF);
    IF_LAZY_EFLAG_GET(SF);
    IF_LAZY_EFLAG_GET(ZF);
    IF_LAZY_EFLAG_GET(AF);
    IF_LAZY_EFLAG_GET(PF);
    IF_LAZY_EFLAG_GET(CF);
    IF_EFLAG_GET(ID);
    IF_EFLAG_GET(VIP);
    IF_EFLAG_GET(VIF);
    IF_EFLAG_GET(AC);
    IF_EFLAG_GET(VM);
    IF_EFLAG_GET(RF);
    IF_EFLAG_GET(NT);
    IF_EFLAG_GET(IOPL);
    IF_EFLAG_GET(DF);
    IF_EFLAG_GET(IF);
    IF_EFLAG_GET(TF);
  }
  return val;
}
#undef IF_SEG_REG_GET
#undef IF_SEG_REG_SET

#endif

void BX_CPU_C::initialize(BX_MEM_C *addrspace)
{
  // BX_CPU_C constructor
  BX_CPU_THIS_PTR set_INTR (0);

#if BX_SUPPORT_APIC
  BX_CPU_THIS_PTR local_apic.set_id(BX_CPU_ID);
  BX_CPU_THIS_PTR local_apic.init();
#endif

  // in SMP mode, the prefix of the CPU will be changed to [CPUn] in 
  // bx_local_apic_c::set_id as soon as the apic ID is assigned.

// <TAG-INIT-CPU-START>
  // for decoding instructions: access to seg reg's via index number
  sreg_mod00_rm16[0] = BX_SEG_REG_DS;
  sreg_mod00_rm16[1] = BX_SEG_REG_DS;
  sreg_mod00_rm16[2] = BX_SEG_REG_SS;
  sreg_mod00_rm16[3] = BX_SEG_REG_SS;
  sreg_mod00_rm16[4] = BX_SEG_REG_DS;
  sreg_mod00_rm16[5] = BX_SEG_REG_DS;
  sreg_mod00_rm16[6] = BX_SEG_REG_DS;
  sreg_mod00_rm16[7] = BX_SEG_REG_DS;

  sreg_mod01or10_rm16[0] = BX_SEG_REG_DS;
  sreg_mod01or10_rm16[1] = BX_SEG_REG_DS;
  sreg_mod01or10_rm16[2] = BX_SEG_REG_SS;
  sreg_mod01or10_rm16[3] = BX_SEG_REG_SS;
  sreg_mod01or10_rm16[4] = BX_SEG_REG_DS;
  sreg_mod01or10_rm16[5] = BX_SEG_REG_DS;
  sreg_mod01or10_rm16[6] = BX_SEG_REG_SS;
  sreg_mod01or10_rm16[7] = BX_SEG_REG_DS;

  // the default segment to use for a one-byte modrm with 
  // mod==01b or mod==10b and rm == i
  sreg_mod01or10_rm32[0] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[1] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[2] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[3] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[4] = BX_SEG_REG_NULL;
    // this entry should never be accessed
    // (escape to 2-byte)
  sreg_mod01or10_rm32[5] = BX_SEG_REG_SS;
  sreg_mod01or10_rm32[6] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[7] = BX_SEG_REG_DS;
#if BX_SUPPORT_X86_64
  sreg_mod01or10_rm32[8] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[9] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[10] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[11] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[12] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[13] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[14] = BX_SEG_REG_DS;
  sreg_mod01or10_rm32[15] = BX_SEG_REG_DS;
#endif

  // the default segment to use for a two-byte modrm with
  // mod==00b and base == i
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
  // mod==01b or mod==10b and base == i
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
// <TAG-INIT-CPU-END>

  mem = addrspace;
  sprintf(name, "CPU %d", BX_CPU_ID);

#if BX_WITH_WX
  if (SIM->get_param(BXPN_WX_CPU_STATE) != NULL) {
    // Register some of the CPUs variables as shadow parameters so that
    // they can be visible in the config interface.
    // (Experimental, obviously not a complete list)
    bx_param_num_c *param;
    char cpu_name[10], cpu_title[10], cpu_pname[16];
    const char *fmt16 = "%04X";
    const char *fmt32 = "%08X";
    Bit32u oldbase = bx_param_num_c::set_default_base(16);
    const char *oldfmt = bx_param_num_c::set_default_format(fmt32);
    sprintf(cpu_name, "%d", BX_CPU_ID);
    sprintf(cpu_title, "CPU %d", BX_CPU_ID);
    sprintf(cpu_pname, "%s.%d", BXPN_WX_CPU_STATE, BX_CPU_ID);
    if (SIM->get_param(cpu_pname) == NULL) {
      bx_list_c *list = new bx_list_c(SIM->get_param(BXPN_WX_CPU_STATE), 
         cpu_name, cpu_title, 60);

#define DEFPARAM_NORMAL(name,field) \
    new bx_shadow_num_c(list, #name, &(field))

      DEFPARAM_NORMAL(EAX, EAX);
      DEFPARAM_NORMAL(EBX, EBX);
      DEFPARAM_NORMAL(ECX, ECX);
      DEFPARAM_NORMAL(EDX, EDX);
      DEFPARAM_NORMAL(ESP, ESP);
      DEFPARAM_NORMAL(EBP, EBP);
      DEFPARAM_NORMAL(ESI, ESI);
      DEFPARAM_NORMAL(EDI, EDI);
      DEFPARAM_NORMAL(EIP, EIP);
      DEFPARAM_NORMAL(DR0, dr0);
      DEFPARAM_NORMAL(DR1, dr1);
      DEFPARAM_NORMAL(DR2, dr2);
      DEFPARAM_NORMAL(DR3, dr3);
      DEFPARAM_NORMAL(DR6, dr6);
      DEFPARAM_NORMAL(DR7, dr7);
      DEFPARAM_NORMAL(CR0, cr0.val32);
      DEFPARAM_NORMAL(CR1, cr1);
      DEFPARAM_NORMAL(CR2, cr2);
      DEFPARAM_NORMAL(CR3, cr3);
#if BX_CPU_LEVEL >= 4
      DEFPARAM_NORMAL(CR4, cr4.val32);
#endif

      // segment registers require a handler function because they have
      // special get/set requirements.
#define DEFPARAM_SEG_REG(x) \
    param = new bx_param_num_c(list, \
      #x, #x, "", 0, 0xffff, 0); \
    param->set_handler(cpu_param_handler); \
    param->set_format(fmt16);
#define DEFPARAM_GLOBAL_SEG_REG(name,field) \
    param = new bx_shadow_num_c(list, \
        #name"_base", \
        & BX_CPU_THIS_PTR field.base); \
    param = new bx_shadow_num_c(list, \
        #name"_limit", \
        & BX_CPU_THIS_PTR field.limit);

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
#undef DEFPARAM_NORMAL
#undef DEFPARAM_SEG_REG
#undef DEFPARAM_GLOBAL_SEG_REG

      param = new bx_shadow_num_c(list, "EFLAGS",
          &BX_CPU_THIS_PTR eflags.val32);

      // flags implemented in lazy_flags.cc must be done with a handler
      // that calls their get function, to force them to be computed.
#define DEFPARAM_EFLAG(name) \
    param = new bx_param_bool_c(list, \
            #name, #name, "", get_##name()); \
    param->set_handler(cpu_param_handler);
#define DEFPARAM_LAZY_EFLAG(name) \
    param = new bx_param_bool_c(list, \
            #name, #name, "", get_##name()); \
    param->set_handler(cpu_param_handler);

#if BX_CPU_LEVEL >= 4
      DEFPARAM_EFLAG(ID);
      DEFPARAM_EFLAG(VIP);
      DEFPARAM_EFLAG(VIF);
      DEFPARAM_EFLAG(AC);
#endif
#if BX_CPU_LEVEL >= 3
      DEFPARAM_EFLAG(VM);
      DEFPARAM_EFLAG(RF);
#endif
#if BX_CPU_LEVEL >= 2
      DEFPARAM_EFLAG(NT);
      // IOPL is a special case because it is 2 bits wide.
      param = new bx_shadow_num_c(
              list,
              "IOPL",
              &eflags.val32, 10,
              12, 13);
      param->set_range(0, 3);
      param->set_format("%d");
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
      bx_param_num_c::set_default_base(oldbase);
      bx_param_num_c::set_default_format(oldfmt);
    }
  }
#endif
}

// save/restore functionality
void BX_CPU_C::register_state(void)
{
  unsigned i;
  char cpu_name[10], cpu_title[10], name[10];

  sprintf(cpu_name, "%d", BX_CPU_ID);
  sprintf(cpu_title, "CPU %d", BX_CPU_ID);
  bx_list_c *cpu = new bx_list_c(SIM->get_param("bochs.cpu"), 
           cpu_name, cpu_title, 60);

  BXRS_PARAM_SPECIAL32(cpu, cpu_version, param_save_handler, param_restore_handler);
  BXRS_PARAM_SPECIAL32(cpu, cpuid_std,   param_save_handler, param_restore_handler);
  BXRS_PARAM_SPECIAL32(cpu, cpuid_ext,   param_save_handler, param_restore_handler);
  BXRS_DEC_PARAM_SIMPLE(cpu, cpu_mode);
  BXRS_DEC_PARAM_SIMPLE(cpu, cpu_state);
  BXRS_HEX_PARAM_SIMPLE(cpu, inhibit_mask);
#if BX_SUPPORT_X86_64
  BXRS_HEX_PARAM_SIMPLE(cpu, RAX);
  BXRS_HEX_PARAM_SIMPLE(cpu, RBX);
  BXRS_HEX_PARAM_SIMPLE(cpu, RCX);
  BXRS_HEX_PARAM_SIMPLE(cpu, RDX);
  BXRS_HEX_PARAM_SIMPLE(cpu, RSP);
  BXRS_HEX_PARAM_SIMPLE(cpu, RBP);
  BXRS_HEX_PARAM_SIMPLE(cpu, RSI);
  BXRS_HEX_PARAM_SIMPLE(cpu, RDI);
  BXRS_HEX_PARAM_SIMPLE(cpu, R8);
  BXRS_HEX_PARAM_SIMPLE(cpu, R9);
  BXRS_HEX_PARAM_SIMPLE(cpu, R10);
  BXRS_HEX_PARAM_SIMPLE(cpu, R11);
  BXRS_HEX_PARAM_SIMPLE(cpu, R12);
  BXRS_HEX_PARAM_SIMPLE(cpu, R13);
  BXRS_HEX_PARAM_SIMPLE(cpu, R14);
  BXRS_HEX_PARAM_SIMPLE(cpu, R15);
  BXRS_HEX_PARAM_SIMPLE(cpu, RIP);
#else
  BXRS_HEX_PARAM_SIMPLE(cpu, EAX);
  BXRS_HEX_PARAM_SIMPLE(cpu, EBX);
  BXRS_HEX_PARAM_SIMPLE(cpu, ECX);
  BXRS_HEX_PARAM_SIMPLE(cpu, EDX);
  BXRS_HEX_PARAM_SIMPLE(cpu, ESP);
  BXRS_HEX_PARAM_SIMPLE(cpu, EBP);
  BXRS_HEX_PARAM_SIMPLE(cpu, ESI);
  BXRS_HEX_PARAM_SIMPLE(cpu, EDI);
  BXRS_HEX_PARAM_SIMPLE(cpu, EIP);
#endif
  BXRS_PARAM_SPECIAL32(cpu, EFLAGS, 
         param_save_handler, param_restore_handler);
#if BX_CPU_LEVEL >= 3
  BXRS_HEX_PARAM_FIELD(cpu, DR0, dr0);
  BXRS_HEX_PARAM_FIELD(cpu, DR1, dr1);
  BXRS_HEX_PARAM_FIELD(cpu, DR2, dr2);
  BXRS_HEX_PARAM_FIELD(cpu, DR3, dr3);
  BXRS_HEX_PARAM_FIELD(cpu, DR6, dr6);
  BXRS_HEX_PARAM_FIELD(cpu, DR7, dr7);
#endif
  BXRS_HEX_PARAM_FIELD(cpu, CR0, cr0.val32);
  BXRS_HEX_PARAM_FIELD(cpu, CR2, cr2);
  BXRS_HEX_PARAM_FIELD(cpu, CR3, cr3);
#if BX_CPU_LEVEL >= 4
  BXRS_HEX_PARAM_FIELD(cpu, CR4, cr4.val32);
#endif

  for(i=0; i<6; i++) {
    bx_segment_reg_t *segment = &BX_CPU_THIS_PTR sregs[i];
    bx_list_c *sreg = new bx_list_c(cpu, strseg(segment), 9);
    BXRS_PARAM_SPECIAL16(sreg, selector, 
           param_save_handler, param_restore_handler);
    BXRS_HEX_PARAM_FIELD(sreg, base, segment->cache.u.segment.base);
    BXRS_HEX_PARAM_FIELD(sreg, limit, segment->cache.u.segment.limit);
    BXRS_HEX_PARAM_FIELD(sreg, limit_scaled, segment->cache.u.segment.limit_scaled);
    BXRS_PARAM_SPECIAL8 (sreg, ar_byte, 
           param_save_handler, param_restore_handler);
    BXRS_PARAM_BOOL(sreg, granularity, segment->cache.u.segment.g);
    BXRS_PARAM_BOOL(sreg, d_b, segment->cache.u.segment.d_b);
#if BX_SUPPORT_X86_64
    BXRS_PARAM_BOOL(sreg, l, segment->cache.u.segment.l);
#endif
    BXRS_PARAM_BOOL(sreg, avl, segment->cache.u.segment.avl);
  }

#if BX_CPU_LEVEL >= 2
  BXRS_HEX_PARAM_FIELD(cpu, gdtr_base, BX_CPU_THIS_PTR gdtr.base);
  BXRS_HEX_PARAM_FIELD(cpu, gdtr_limit, BX_CPU_THIS_PTR gdtr.limit);
  BXRS_HEX_PARAM_FIELD(cpu, idtr_base, BX_CPU_THIS_PTR idtr.base);
  BXRS_HEX_PARAM_FIELD(cpu, idtr_limit, BX_CPU_THIS_PTR idtr.limit);
#endif

  bx_list_c *LDTR = new bx_list_c (cpu, "LDTR", 7);
  BXRS_PARAM_SPECIAL16(LDTR, selector, param_save_handler, param_restore_handler);
  BXRS_HEX_PARAM_FIELD(LDTR, base,  ldtr.cache.u.system.base);
  BXRS_HEX_PARAM_FIELD(LDTR, limit, ldtr.cache.u.system.limit);
  BXRS_HEX_PARAM_FIELD(LDTR, limit_scaled, ldtr.cache.u.system.limit);
  BXRS_PARAM_SPECIAL8 (LDTR, ar_byte, param_save_handler, param_restore_handler);
  BXRS_PARAM_BOOL(LDTR, granularity, ldtr.cache.u.system.g);
  BXRS_PARAM_BOOL(LDTR, avl, ldtr.cache.u.system.avl);

  bx_list_c *TR = new bx_list_c (cpu, "TR", 7);
  BXRS_PARAM_SPECIAL16(TR, selector, param_save_handler, param_restore_handler);
  BXRS_HEX_PARAM_FIELD(TR, base,  tr.cache.u.system.base);
  BXRS_HEX_PARAM_FIELD(TR, limit, tr.cache.u.system.limit);
  BXRS_HEX_PARAM_FIELD(TR, limit_scaled, tr.cache.u.system.limit_scaled);
  BXRS_PARAM_SPECIAL8 (TR, ar_byte, param_save_handler, param_restore_handler);
  BXRS_PARAM_BOOL(TR, granularity, tr.cache.u.system.g);
  BXRS_PARAM_BOOL(TR, avl, tr.cache.u.system.avl);

  BXRS_HEX_PARAM_SIMPLE(cpu, smbase);

#if BX_CPU_LEVEL >= 5
  bx_list_c *MSR = new bx_list_c(cpu, "MSR", 45);

#if BX_SUPPORT_APIC
  BXRS_HEX_PARAM_FIELD(MSR, apicbase, msr.apicbase);
#endif
#if BX_SUPPORT_X86_64
  BXRS_PARAM_SPECIAL32(MSR, EFER, param_save_handler, param_restore_handler);
  BXRS_HEX_PARAM_FIELD(MSR,  star, msr.star);
  BXRS_HEX_PARAM_FIELD(MSR, lstar, msr.lstar);
  BXRS_HEX_PARAM_FIELD(MSR, cstar, msr.cstar);
  BXRS_HEX_PARAM_FIELD(MSR, fmask, msr.fmask);
  BXRS_HEX_PARAM_FIELD(MSR, kernelgsbase, msr.kernelgsbase);
  BXRS_HEX_PARAM_FIELD(MSR, tsc_aux, msr.tsc_aux);
#endif
  BXRS_HEX_PARAM_FIELD(MSR, tsc_last_reset, msr.tsc_last_reset);
#if BX_SUPPORT_SEP
  BXRS_HEX_PARAM_FIELD(MSR, sysenter_cs_msr,  msr.sysenter_cs_msr);
  BXRS_HEX_PARAM_FIELD(MSR, sysenter_esp_msr, msr.sysenter_esp_msr);
  BXRS_HEX_PARAM_FIELD(MSR, sysenter_eip_msr, msr.sysenter_eip_msr);
#endif
#if BX_SUPPORT_MTRR
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase0, msr.mtrrphys[0]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask0, msr.mtrrphys[1]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase1, msr.mtrrphys[2]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask1, msr.mtrrphys[3]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase2, msr.mtrrphys[4]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask2, msr.mtrrphys[5]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase3, msr.mtrrphys[6]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask3, msr.mtrrphys[7]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase4, msr.mtrrphys[8]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask4, msr.mtrrphys[9]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase5, msr.mtrrphys[10]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask5, msr.mtrrphys[11]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase6, msr.mtrrphys[12]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask6, msr.mtrrphys[13]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysbase7, msr.mtrrphys[14]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrphysmask7, msr.mtrrphys[15]);

  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix64k_00000, msr.mtrrfix64k_00000);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix16k_80000, msr.mtrrfix16k_80000);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix16k_a0000, msr.mtrrfix16k_a0000);

  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_c0000, msr.mtrrfix4k[0]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_c8000, msr.mtrrfix4k[1]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_d0000, msr.mtrrfix4k[2]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_d8000, msr.mtrrfix4k[3]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_e0000, msr.mtrrfix4k[4]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_e8000, msr.mtrrfix4k[5]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_f0000, msr.mtrrfix4k[6]);
  BXRS_HEX_PARAM_FIELD(MSR, mtrrfix4k_f8000, msr.mtrrfix4k[7]);

  BXRS_HEX_PARAM_FIELD(MSR, pat, msr.pat);
  BXRS_HEX_PARAM_FIELD(MSR, mtrr_deftype, msr.mtrr_deftype);
#endif
#endif

#if BX_SUPPORT_FPU || BX_SUPPORT_MMX
  bx_list_c *fpu = new bx_list_c(cpu, "fpu", 17);
  BXRS_HEX_PARAM_FIELD(fpu, cwd, the_i387.cwd);
  BXRS_HEX_PARAM_FIELD(fpu, swd, the_i387.swd);
  BXRS_HEX_PARAM_FIELD(fpu, twd, the_i387.twd);
  BXRS_HEX_PARAM_FIELD(fpu, foo, the_i387.foo);
  BXRS_HEX_PARAM_FIELD(fpu, fcs, the_i387.fcs);
  BXRS_HEX_PARAM_FIELD(fpu, fip, the_i387.fip);
  BXRS_HEX_PARAM_FIELD(fpu, fds, the_i387.fds);
  BXRS_HEX_PARAM_FIELD(fpu, fdp, the_i387.fdp);
  for (i=0; i<8; i++) {
    sprintf(name, "st%d", i);
    bx_list_c *STx = new bx_list_c(fpu, name, 8);
    BXRS_HEX_PARAM_FIELD(STx, exp,      the_i387.st_space[i].exp);
    BXRS_HEX_PARAM_FIELD(STx, fraction, the_i387.st_space[i].fraction);
  }
  BXRS_DEC_PARAM_FIELD(fpu, tos, the_i387.tos);
#endif

#if BX_SUPPORT_SSE
  bx_list_c *sse = new bx_list_c(cpu, "sse", 2*BX_XMM_REGISTERS+1);
  BXRS_HEX_PARAM_FIELD(sse, mxcsr, mxcsr.mxcsr);
  for (i=0; i<BX_XMM_REGISTERS; i++) {
    sprintf(name, "xmm%02d_hi", i);
    new bx_shadow_num_c(sse, name, &BX_CPU_THIS_PTR xmm[i].xmm64u(1), BASE_HEX);
    sprintf(name, "xmm%02d_lo", i);
    new bx_shadow_num_c(sse, name, &BX_CPU_THIS_PTR xmm[i].xmm64u(0), BASE_HEX);
  }
#endif

#if BX_SUPPORT_APIC
  local_apic.register_state(cpu);
#endif

  BXRS_PARAM_BOOL(cpu, EXT, EXT);
  BXRS_PARAM_BOOL(cpu, async_event, async_event);
  BXRS_PARAM_BOOL(cpu, INTR, INTR);
  BXRS_PARAM_BOOL(cpu, smi_pending, smi_pending);
  BXRS_PARAM_BOOL(cpu, nmi_pending, nmi_pending);
  BXRS_PARAM_BOOL(cpu, in_smm, in_smm);
  BXRS_PARAM_BOOL(cpu, nmi_disable, nmi_disable);
  BXRS_PARAM_BOOL(cpu, trace, trace);
}

Bit64s BX_CPU_C::param_save_handler(void *devptr, bx_param_c *param, Bit64s val)
{
#if !BX_USE_CPU_SMF
  BX_CPU_C *class_ptr = (BX_CPU_C *) devptr;
  return class_ptr->param_save(param, val);
}

Bit64s BX_CPU_C::param_save(bx_param_c *param, Bit64s val)
{
#else
  UNUSED(devptr);
#endif // !BX_USE_CPU_SMF
  const char *pname, *segname;
  bx_segment_reg_t *segment = NULL;

  pname = param->get_name();
  if (!strcmp(pname, "cpu_version")) {
    val = get_cpu_version_information();
  } else if (!strcmp(pname, "cpuid_std")) {
    val = get_std_cpuid_features();
  } else if (!strcmp(pname, "cpuid_ext")) {
    val = get_extended_cpuid_features();
  } else if (!strcmp(pname, "EFLAGS")) {
    val = BX_CPU_THIS_PTR read_eflags();
#if BX_SUPPORT_X86_64
  } else if (!strcmp(pname, "EFER")) {
    val = BX_CPU_THIS_PTR get_EFER();
#endif
  } else if (!strcmp(pname, "ar_byte") || !strcmp(pname, "selector")) {
    segname = param->get_parent()->get_name();
    if (!strcmp(segname, "CS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS];
    } else if (!strcmp(segname, "DS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
    } else if (!strcmp(segname, "SS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS];
    } else if (!strcmp(segname, "ES")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES];
    } else if (!strcmp(segname, "FS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS];
    } else if (!strcmp(segname, "GS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS];
    } else if (!strcmp(segname, "LDTR")) {
      segment = &BX_CPU_THIS_PTR ldtr;
    } else if (!strcmp(segname, "TR")) {
      segment = &BX_CPU_THIS_PTR tr;
    }
    if (segment != NULL) {
      if (!strcmp(pname, "ar_byte")) {
        val = ar_byte(&(segment->cache));
      }
      else if (!strcmp(pname, "selector")) {
        val = segment->selector.value;
      }
    }
  }
  else {
    BX_PANIC(("Unknown param %s in param_save handler !", pname));
  }
  return val;
}

Bit64s BX_CPU_C::param_restore_handler(void *devptr, bx_param_c *param, Bit64s val)
{
#if !BX_USE_CPU_SMF
  BX_CPU_C *class_ptr = (BX_CPU_C *) devptr;
  return class_ptr->param_restore(param, val);
}

Bit64s BX_CPU_C::param_restore(bx_param_c *param, Bit64s val)
{
#else
  UNUSED(devptr);
#endif // !BX_USE_CPU_SMF
  const char *pname, *segname;
  bx_segment_reg_t *segment = NULL;

  pname = param->get_name();
  if (!strcmp(pname, "cpu_version")) {
    if (val != get_cpu_version_information()) {
      BX_PANIC(("save/restore: CPU version mismatch"));
    }
  } else if (!strcmp(pname, "cpuid_std")) {
    if (val != get_std_cpuid_features()) {
      BX_PANIC(("save/restore: CPUID mismatch"));
    }
  } else if (!strcmp(pname, "cpuid_ext")) {
    if (val != get_extended_cpuid_features()) {
      BX_PANIC(("save/restore: CPUID mismatch"));
    }
  } else if (!strcmp(pname, "EFLAGS")) {
    BX_CPU_THIS_PTR setEFlags((Bit32u)val);
#if BX_SUPPORT_X86_64
  } else if (!strcmp(pname, "EFER")) {
    BX_CPU_THIS_PTR efer.sce   = (val >> 0)  & 1;
    BX_CPU_THIS_PTR efer.lme   = (val >> 8)  & 1;
    BX_CPU_THIS_PTR efer.lma   = (val >> 10) & 1;
    BX_CPU_THIS_PTR efer.nxe   = (val >> 11) & 1;
    BX_CPU_THIS_PTR efer.ffxsr = (val >> 14) & 1;
#endif
  } else if (!strcmp(pname, "ar_byte") || !strcmp(pname, "selector")) {
    segname = param->get_parent()->get_name();
    if (!strcmp(segname, "CS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS];
    } else if (!strcmp(segname, "DS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
    } else if (!strcmp(segname, "SS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS];
    } else if (!strcmp(segname, "ES")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES];
    } else if (!strcmp(segname, "FS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS];
    } else if (!strcmp(segname, "GS")) {
      segment = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS];
    } else if (!strcmp(segname, "LDTR")) {
      segment = &BX_CPU_THIS_PTR ldtr;
    } else if (!strcmp(segname, "TR")) {
      segment = &BX_CPU_THIS_PTR tr;
    }
    if (segment != NULL) {
      bx_descriptor_t *d = &(segment->cache);
      bx_selector_t *selector = &(segment->selector);
      if (!strcmp(pname, "ar_byte")) {
        set_ar_byte(d, (Bit8u)val);
      }
      else if (!strcmp(pname, "selector")) {
        parse_selector((Bit16u)val, selector);
        // validate the selector
        if ((selector->value & 0xfffc) != 0) d->valid = 1;
        else d->valid = 0;
      }
    }
  }
  else {
    BX_PANIC(("Unknown param %s in param_restore handler !", pname));
  }
  return val;
}

void BX_CPU_C::after_restore_state(void)
{
  SetCR0(cr0.val32);
  CR3_change(cr3);
  TLB_flush(1);
  assert_checks();
  invalidate_prefetch_q();
  debug(RIP);
}
// end of save/restore functionality

BX_CPU_C::~BX_CPU_C()
{
  BX_INSTR_SHUTDOWN(BX_CPU_ID);
  BX_DEBUG(("Exit."));
}

void BX_CPU_C::reset(unsigned source)
{
  unsigned i;

  UNUSED(source); // either BX_RESET_HARDWARE or BX_RESET_SOFTWARE

  // initialize CPUID values
  set_cpuid_defaults();

#if BX_SUPPORT_X86_64
  RAX = 0; // processor passed test :-)
  RBX = 0;
  RCX = 0;
  RDX = get_cpu_version_information();
  RBP = 0;
  RSI = 0;
  RDI = 0;
  RSP = 0;
  R8  = 0;
  R9  = 0;
  R10 = 0;
  R11 = 0;
  R12 = 0;
  R13 = 0;
  R14 = 0;
  R15 = 0;
#else
  // general registers
  EAX = 0; // processor passed test :-)
  EBX = 0;
  ECX = 0;
  EDX = get_cpu_version_information();
  EBP = 0;
  ESI = 0;
  EDI = 0;
  ESP = 0;
#endif

  // status and control flags register set
  BX_CPU_THIS_PTR setEFlags(0x2); // Bit1 is always set

  BX_CPU_THIS_PTR inhibit_mask = 0;
  BX_CPU_THIS_PTR debug_trap = 0;

  /* instruction pointer */
#if BX_CPU_LEVEL < 2
  BX_CPU_THIS_PTR prev_eip = EIP = 0x00000000;
#else /* from 286 up */
  BX_CPU_THIS_PTR prev_eip = RIP = 0x0000FFF0;
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

  parse_selector(0xf000, 
          &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type     = BX_CODE_EXEC_READ_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0xFFFF0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l   = 0; /* 16bit default size */
#endif
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
#endif

#if BX_SUPPORT_ICACHE
  BX_CPU_THIS_PTR updateFetchModeMask();
#endif

  /* DS (Data Segment) and descriptor cache */

  parse_selector(0x0000, 
          &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector);

#if BX_CPU_LEVEL >= 2
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit        = 0xFFFF;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xFFFF;
#endif
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.l   = 0; /* 16bit default size */
#endif
#endif

  // use DS segment as template for the others
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
#endif

#if BX_CPU_LEVEL >= 2
  /* GDTR (Global Descriptor Table Register) */
  BX_CPU_THIS_PTR gdtr.base  = 0x00000000;
  BX_CPU_THIS_PTR gdtr.limit =     0xFFFF;

  /* IDTR (Interrupt Descriptor Table Register) */
  BX_CPU_THIS_PTR idtr.base  = 0x00000000;
  BX_CPU_THIS_PTR idtr.limit =     0xFFFF; /* always byte granular */

  /* LDTR (Local Descriptor Table Register) */
  BX_CPU_THIS_PTR ldtr.selector.value = 0x0000;
  BX_CPU_THIS_PTR ldtr.selector.index = 0x0000;
  BX_CPU_THIS_PTR ldtr.selector.ti    = 0;
  BX_CPU_THIS_PTR ldtr.selector.rpl   = 0;

  BX_CPU_THIS_PTR ldtr.cache.valid    = 1; /* valid */
  BX_CPU_THIS_PTR ldtr.cache.p        = 1; /* present */
  BX_CPU_THIS_PTR ldtr.cache.dpl      = 0; /* field not used */
  BX_CPU_THIS_PTR ldtr.cache.segment  = 0; /* system segment */
  BX_CPU_THIS_PTR ldtr.cache.type     = BX_SYS_SEGMENT_LDT;
  BX_CPU_THIS_PTR ldtr.cache.u.system.base       = 0x00000000;
  BX_CPU_THIS_PTR ldtr.cache.u.system.limit      =     0xFFFF;
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled =   0xFFFF;
  BX_CPU_THIS_PTR ldtr.cache.u.system.avl = 0;
  BX_CPU_THIS_PTR ldtr.cache.u.system.g   = 0;  /* byte granular */
#endif

  /* TR (Task Register) */
  BX_CPU_THIS_PTR tr.selector.value = 0x0000;
  BX_CPU_THIS_PTR tr.selector.index = 0x0000; /* undefined */
  BX_CPU_THIS_PTR tr.selector.ti    = 0;
  BX_CPU_THIS_PTR tr.selector.rpl   = 0;

  BX_CPU_THIS_PTR tr.cache.valid    = 1; /* valid */
  BX_CPU_THIS_PTR tr.cache.p        = 1; /* present */
  BX_CPU_THIS_PTR tr.cache.dpl      = 0; /* field not used */
  BX_CPU_THIS_PTR tr.cache.segment  = 0; /* system segment */
  BX_CPU_THIS_PTR tr.cache.type     = BX_SYS_SEGMENT_BUSY_286_TSS;
  BX_CPU_THIS_PTR tr.cache.u.system.base         = 0x00000000;
  BX_CPU_THIS_PTR tr.cache.u.system.limit        =     0xFFFF;
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR tr.cache.u.system.limit_scaled =     0xFFFF;
  BX_CPU_THIS_PTR tr.cache.u.system.avl = 0;
  BX_CPU_THIS_PTR tr.cache.u.system.g   = 0;  /* byte granular */
#endif
#endif

  // DR0 - DR7 (Debug Registers)
#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR dr0 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr1 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr2 = 0;   /* undefined */
  BX_CPU_THIS_PTR dr3 = 0;   /* undefined */
#endif

  BX_CPU_THIS_PTR dr7 = 0x00000400;
#if   BX_CPU_LEVEL == 3
  BX_CPU_THIS_PTR dr6 = 0xFFFF1FF0;
#elif BX_CPU_LEVEL == 4
  BX_CPU_THIS_PTR dr6 = 0xFFFF1FF0;
#elif BX_CPU_LEVEL == 5
  BX_CPU_THIS_PTR dr6 = 0xFFFF0FF0;
#elif BX_CPU_LEVEL == 6
  BX_CPU_THIS_PTR dr6 = 0xFFFF0FF0;
#else
#  error "DR6: CPU > 6"
#endif

  BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_REAL;
  BX_CPU_THIS_PTR cpu_state = BX_CPU_STATE_ACTIVE;

  BX_CPU_THIS_PTR smi_pending = 0;
  BX_CPU_THIS_PTR nmi_pending = 0;
  BX_CPU_THIS_PTR in_smm = 0;
  BX_CPU_THIS_PTR nmi_disable = 0;
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check = 0;
#endif

  BX_CPU_THIS_PTR smbase = 0x30000;

  BX_CPU_THIS_PTR cr0.setRegister(0);

  // handle reserved bits
#if BX_CPU_LEVEL == 3
  // reserved bits all set to 1 on 386
  BX_CPU_THIS_PTR cr0.val32 |= 0x7ffffff0;
#elif BX_CPU_LEVEL >= 4
  // bit 4 is hardwired to 1 on all x86
  BX_CPU_THIS_PTR cr0.val32 |= 0x00000010;
#endif

#if BX_CPU_LEVEL >= 3
  BX_CPU_THIS_PTR cr1 = 0;
  BX_CPU_THIS_PTR cr2 = 0;
  BX_CPU_THIS_PTR cr3 = 0;
  BX_CPU_THIS_PTR cr3_masked = 0;
#endif

#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR cr4.setRegister(0);
#endif

  // CR0/CR4 paging might be modified
  TLB_flush(1);

/* initialise MSR registers to defaults */
#if BX_CPU_LEVEL >= 5
#if BX_SUPPORT_APIC
  /* APIC Address, APIC enabled and BSP is default, we'll fill in the rest later */
  BX_CPU_THIS_PTR msr.apicbase = BX_LAPIC_BASE_ADDR;
  BX_CPU_THIS_PTR local_apic.init();
  BX_CPU_THIS_PTR msr.apicbase |= 0x900;
#endif
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR efer.lme = BX_CPU_THIS_PTR efer.lma = 0;
  BX_CPU_THIS_PTR efer.sce = BX_CPU_THIS_PTR efer.nxe = 0;
  BX_CPU_THIS_PTR efer.ffxsr = 0;

  BX_CPU_THIS_PTR msr.star  = 0;
  BX_CPU_THIS_PTR msr.lstar = 0;
  BX_CPU_THIS_PTR msr.cstar = 0;
  BX_CPU_THIS_PTR msr.fmask = 0;
  BX_CPU_THIS_PTR msr.kernelgsbase = 0;
  BX_CPU_THIS_PTR msr.tsc_aux = 0;
#endif
  BX_CPU_THIS_PTR set_TSC(0);
#endif

#if BX_SUPPORT_SEP
  BX_CPU_THIS_PTR msr.sysenter_cs_msr  = 0;
  BX_CPU_THIS_PTR msr.sysenter_esp_msr = 0;
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = 0;
#endif

#if BX_SUPPORT_MTRR
  for (int i=0;i<16;i++)
    BX_CPU_THIS_PTR msr.mtrrphys[i] = 0;

  BX_CPU_THIS_PTR msr.mtrrfix64k_00000 = 0; // all fix range MTRRs undefined according to manual
  BX_CPU_THIS_PTR msr.mtrrfix16k_80000 = 0;
  BX_CPU_THIS_PTR msr.mtrrfix16k_a0000 = 0;

  for (int i=0;i<8;i++)
    BX_CPU_THIS_PTR msr.mtrrfix4k[i] = 0;

  BX_CPU_THIS_PTR msr.pat = BX_CONST64(0x0007040600070406);
  BX_CPU_THIS_PTR msr.mtrr_deftype = 0;
#endif

  BX_CPU_THIS_PTR EXT = 0;

#if BX_USE_TLB
  TLB_init();
#endif

  // invalidate the prefetch queue
  BX_CPU_THIS_PTR eipPageBias = 0;
  BX_CPU_THIS_PTR eipPageWindowSize = 0;
  BX_CPU_THIS_PTR eipFetchPtr = NULL;

#if BX_DEBUGGER
#if BX_MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
  BX_CPU_THIS_PTR trace_reg = 0;
  BX_CPU_THIS_PTR dbg_cpu_mode = BX_CPU_THIS_PTR cpu_mode;
#endif

  BX_CPU_THIS_PTR trace = 0;

  // Reset the Floating Point Unit
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR the_i387.reset(); // unchanged on #INIT
#endif

  // Reset XMM state
#if BX_SUPPORT_SSE >= 1  // unchanged on #INIT
  for(i=0; i < BX_XMM_REGISTERS; i++)
  {
    BX_CPU_THIS_PTR xmm[i].xmm64u(0) = 0;
    BX_CPU_THIS_PTR xmm[i].xmm64u(1) = 0;
  }

  BX_CPU_THIS_PTR mxcsr.mxcsr = MXCSR_RESET;
#endif

#if BX_SUPPORT_SMP
  // notice if I'm the bootstrap processor.  If not, do the equivalent of
  // a HALT instruction.
  int apic_id = local_apic.get_id();
  if (BX_BOOTSTRAP_PROCESSOR == apic_id) {
    // boot normally
    BX_CPU_THIS_PTR msr.apicbase |= 0x0100; /* set bit 8 BSP */
    BX_INFO(("CPU[%d] is the bootstrap processor", apic_id));
  } else {
    // it's an application processor, halt until IPI is heard.
    BX_CPU_THIS_PTR msr.apicbase &= ~0x0100; /* clear bit 8 BSP */
    BX_INFO(("CPU[%d] is an application processor. Halting until IPI.", apic_id));
    BX_CPU_THIS_PTR cpu_state = BX_CPU_STATE_WAIT_FOR_SIPI;
    debug_trap |= BX_DEBUG_TRAP_HALT_STATE;
    async_event = 1;
  }
#endif

  BX_INSTR_RESET(BX_CPU_ID);
}

void BX_CPU_C::sanity_checks(void)
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
       bh != ((EBX >> 8) & 0xFF) )
  {
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
       di != (EDI & 0xFFFF) )
  {
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
  if (sizeof(Bit64u) != 8  ||  sizeof(Bit64s) != 8)
    BX_PANIC(("data type Bit64u or Bit64u is not of length 8 bytes!"));

  BX_DEBUG(("#(%u)all sanity checks passed!", BX_CPU_ID));
}

void BX_CPU_C::assert_checks(void)
{
  // check CPU mode consistency
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.lma) {
    if (! BX_CPU_THIS_PTR cr0.get_PE()) {
      BX_PANIC(("assert_checks: EFER.LMA is set when CR0.PE=0 !"));
    }
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l) {
      if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64)
        BX_PANIC(("assert_checks: unconsistent cpu_mode BX_MODE_LONG_64 !"));
    }
    else {
      if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_COMPAT)
        BX_PANIC(("assert_checks: unconsistent cpu_mode BX_MODE_LONG_COMPAT !"));
    }
  }
  else 
#endif
  {
    if (BX_CPU_THIS_PTR cr0.get_PE()) {
      if (BX_CPU_THIS_PTR get_VM()) {
        if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_IA32_V8086)
          BX_PANIC(("assert_checks: unconsistent cpu_mode BX_MODE_IA32_V8086 !"));
      }
      else {
        if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_IA32_PROTECTED)
          BX_PANIC(("assert_checks: unconsistent cpu_mode BX_MODE_IA32_PROTECTED !"));
      }
    }
    else {
      if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_IA32_REAL)
        BX_PANIC(("assert_checks: unconsistent cpu_mode BX_MODE_IA32_REAL !"));
    }
  }

  // check CR0 consistency
  if (BX_CPU_THIS_PTR cr0.get_PG() && ! BX_CPU_THIS_PTR cr0.get_PE())
    BX_PANIC(("assert_checks: CR0.PG=1 with CR0.PE=0 !"));
#if BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR cr0.get_NW() && ! BX_CPU_THIS_PTR cr0.get_CD())
    BX_PANIC(("assert_checks: CR0.NW=1 with CR0.CD=0 !"));
#endif


#if BX_SUPPORT_X86_64
  // VM should be OFF in long mode
  if (long_mode()) {
    if (BX_CPU_THIS_PTR get_VM()) BX_PANIC(("assert_checks: VM is set in long mode !"));
  }

  // CS.L and CS.D_B are mutualy exclusive
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l &&
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
  {
    BX_PANIC(("assert_checks: CS.l and CS.d_b set together !"));
  }
#endif

  // check LDTR type
  if (BX_CPU_THIS_PTR ldtr.cache.valid)
  {
    if (BX_CPU_THIS_PTR ldtr.cache.type != BX_SYS_SEGMENT_LDT)
    {
      BX_PANIC(("assert_checks: LDTR is not LDT type !"));
    }
  }

  // check Task Register type
  if(BX_CPU_THIS_PTR tr.cache.valid)
  {
    switch(BX_CPU_THIS_PTR tr.cache.type)
    {
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
#if BX_CPU_LEVEL >= 3
        if (BX_CPU_THIS_PTR tr.cache.u.system.g != 0)
          BX_PANIC(("assert_checks: tss286.g != 0 !"));
        if (BX_CPU_THIS_PTR tr.cache.u.system.avl != 0)
          BX_PANIC(("assert_checks: tss286.avl != 0 !"));
#endif
        break;
      case BX_SYS_SEGMENT_BUSY_386_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
        break;
      default:    
        BX_PANIC(("assert_checks: TR is not TSS type !"));
    }
  }
}

void BX_CPU_C::set_INTR(bx_bool value)
{
  BX_CPU_THIS_PTR INTR = value;
  BX_CPU_THIS_PTR async_event = 1;
}
