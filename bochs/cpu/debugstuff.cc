/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "memory/memory-bochs.h"
#include "pc_system.h"

const char *stringify_EFLAGS(Bit32u eflags, char *s)
{
  /* 31|30|29|28| 27|26|25|24| 23|22|21|20| 19|18|17|16
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0| 0| 0| 0|  0| 0| 0| 0|  0| 0|ID|VP| VF|AC|VM|RF
   *
   * 15|14|13|12| 11|10| 9| 8|  7| 6| 5| 4|  3| 2| 1| 0
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0|NT| IOPL| OF|DF|IF|TF| SF|ZF| 0|AF|  0|PF| 1|CF
   */

  sprintf(s, "%s %s %s %s %s %s %s IOPL=%1u %s %s %s %s %s %s %s %s %s",
    (eflags & (1<<21)) ? "ID" : "id",
    (eflags & (1<<20)) ? "VIP" : "vip",
    (eflags & (1<<19)) ? "VIF" : "vif",
    (eflags & (1<<18)) ? "AC" : "ac",
    (eflags & (1<<17)) ? "VM" : "vm",
    (eflags & (1<<16)) ? "RF" : "rf",
    (eflags & (1<<14)) ? "NT" : "nt",
    (eflags >> 12) & 0x3,
    (eflags & (1<<11)) ? "OF" : "of",
    (eflags & (1<<10)) ? "DF" : "df",
    (eflags & (1<<9)) ? "IF" : "if",
    (eflags & (1<<8)) ? "TF" : "tf",
    (eflags & (1<<7)) ? "SF" : "sf",
    (eflags & (1<<6)) ? "ZF" : "zf",
    (eflags & (1<<4)) ? "AF" : "af",
    (eflags & (1<<2)) ? "PF" : "pf",
    (eflags & (1<<0)) ? "CF" : "cf");
  return s;
}

const char* stringify_CR0(Bit32u cr0, char *s)
{
  sprintf(s, "%s %s %s %s %s %s %s %s %s %s %s",
    (cr0 & (1<<31)) ? "PG" : "pg",
    (cr0 & (1<<30)) ? "CD" : "cd",
    (cr0 & (1<<29)) ? "NW" : "nw",
    (cr0 & (1<<18)) ? "AC" : "ac",
    (cr0 & (1<<16)) ? "WP" : "wp",
    (cr0 & (1<<5))  ? "NE" : "ne",
    (cr0 & (1<<4))  ? "ET" : "et",
    (cr0 & (1<<3))  ? "TS" : "ts",
    (cr0 & (1<<2))  ? "EM" : "em",
    (cr0 & (1<<1))  ? "MP" : "mp",
    (cr0 & (1<<0))  ? "PE" : "pe");
  return s;
}

const char* stringify_CR4(Bit32u cr4, char *s)
{
  sprintf(s, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
    (cr4 & (1<<27)) ? "LASS" : "lass",
    (cr4 & (1<<25)) ? "UINTR" : "uintr",
    (cr4 & (1<<24)) ? "PKS" : "pks",
    (cr4 & (1<<23)) ? "CET" : "cet",
    (cr4 & (1<<22)) ? "PKE" : "pke",
    (cr4 & (1<<21)) ? "SMAP" : "smap",
    (cr4 & (1<<20)) ? "SMEP" : "smep",
    (cr4 & (1<<19)) ? "KEYLOCK" : "keylock",
    (cr4 & (1<<18)) ? "OSXSAVE" : "osxsave",
    (cr4 & (1<<17)) ? "PCID" : "pcid",
    (cr4 & (1<<16)) ? "FSGSBASE" : "fsgsbase",
    (cr4 & (1<<14)) ? "SMX" : "smx",
    (cr4 & (1<<13)) ? "VMX" : "vmx",
    (cr4 & (1<<12)) ? "LA57" : "la57",
    (cr4 & (1<<11)) ? "UMIP" : "umip",
    (cr4 & (1<<10)) ? "OSXMMEXCPT" : "osxmmexcpt",
    (cr4 & (1<<9))  ? "OSFXSR" : "osfxsr",
    (cr4 & (1<<8))  ? "PCE" : "pce",
    (cr4 & (1<<7))  ? "PGE" : "pge",
    (cr4 & (1<<6))  ? "MCE" : "mce",
    (cr4 & (1<<5))  ? "PAE" : "pae",
    (cr4 & (1<<4))  ? "PSE" : "pse",
    (cr4 & (1<<3))  ? "DE" : "de",
    (cr4 & (1<<2))  ? "TSD" : "tsd",
    (cr4 & (1<<1))  ? "PVI" : "pvi",
    (cr4 & (1<<0))  ? "VME" : "vme");
  return s;
};

const char* stringify_MSR_EFER(Bit32u efer, char *s)
{
  sprintf(s, "%s %s %s %s %s",
    (efer & (1<<14)) ? "FFXSR" : "ffxsr",
    (efer & (1<<11)) ? "NXE" : "nxe",
    (efer & (1<<10)) ? "LMA" : "lma",
    (efer & (1<<8))  ? "LME" : "lme",
    (efer & (1<<0))  ? "SCE" : "sce");
  return s;
}

const char* stringify_XCR0(Bit32u xcr0, char *s)
{
  sprintf(s, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
    (xcr0 & (1<<18)) ? "TILEDATA" : "tiledata",
    (xcr0 & (1<<17)) ? "TILECFG" : "tilecfg",
    (xcr0 & (1<<16)) ? "HWP" : "hwp",
    (xcr0 & (1<<15)) ? "LBR" : "lbr",
    (xcr0 & (1<<14)) ? "UINTR" : "uintr",
    (xcr0 & (1<<13)) ? "HDC" : "hdc",
    (xcr0 & (1<<12)) ? "CET_S" : "cet_s",
    (xcr0 & (1<<11)) ? "CET_U" : "cet_u",
    (xcr0 & (1<<10)) ? "PASID" : "pasid",
    (xcr0 & (1<<9))  ? "PKRU" : "pkru",
    (xcr0 & (1<<8))  ? "PT" : "pt",
    (xcr0 & (1<<7))  ? "HI_ZMM" : "hi_zmm",
    (xcr0 & (1<<6))  ? "ZMM_HI256" : "zmm_hi256",
    (xcr0 & (1<<5))  ? "OPMASK" : "opmask",
    (xcr0 & (1<<4))  ? "BNDCFG" : "bndcfg",
    (xcr0 & (1<<3))  ? "BNDREGS" : "bndregs",
    (xcr0 & (1<<2))  ? "YMM" : "ymm",
    (xcr0 & (1<<1))  ? "SSE" : "sse",
    (xcr0 & (1<<0))  ? "FPU" : "fpu");
  return s;
}

#include "bx_debug/debug.h"

void BX_CPU_C::debug_disasm_instruction(bx_address offset)
{
#if BX_DEBUGGER
  if (bx_dbg.debugger_active) {
    bx_dbg_disassemble_current(BX_CPU_ID, 1); // only one cpu, print time stamp
    return;
  }
#endif

  bx_phy_address phy_addr;
  Bit8u  instr_buf[16];
  char   char_buf[512];
  size_t i=0;

  static char letters[] = "0123456789ABCDEF";
  unsigned remainsInPage = 0x1000 - PAGE_OFFSET(offset);

  bool valid = dbg_xlate_linear2phy(get_laddr(BX_SEG_REG_CS, offset), &phy_addr);
  if (valid) {
    BX_MEM(0)->dbg_fetch_mem(BX_CPU_THIS, phy_addr, 16, instr_buf);

    bxInstruction_c instr;
    disasm(instr_buf,
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b,
      BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64, char_buf+i, &instr,
      BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS), offset, BX_DISASM_INTEL);
    unsigned isize = instr.ilen();

    if (isize <= remainsInPage) {
      i=strlen(char_buf);
      char_buf[i++] = ' ';
      char_buf[i++] = ':';
      char_buf[i++] = ' ';
      for (unsigned j=0; j<isize; j++) {
        char_buf[i++] = letters[(instr_buf[j] >> 4) & 0xf];
        char_buf[i++] = letters[(instr_buf[j] >> 0) & 0xf];
      }
      char_buf[i] = 0;
      BX_INFO(("0x" FMT_ADDRX ">> %s", offset, char_buf));
    }
    else {
      BX_INFO(("0x" FMT_ADDRX ": (instruction unavailable) page split instruction", offset));
    }
  }
  else {
    BX_INFO(("0x" FMT_ADDRX ": (instruction unavailable) page not present", offset));
  }
}

const char* cpu_mode_string(unsigned cpu_mode)
{
  static const char *cpu_mode_name[] = {
     "real mode",
     "v8086 mode",
     "protected mode",
     "compatibility mode",
     "long mode",
     "unknown mode"
  };

  if(cpu_mode >= 5) cpu_mode = 5;
  return cpu_mode_name[cpu_mode];
}

const char* cpu_state_string(unsigned state)
{
  static const char *cpu_state_name[] = {
     "active",
     "halted",
     "in shutdown",
     "waiting for SIPI",
     "executing mwait",
     "executing mwait inhibit interrupts",
     "unknown state"
  };

  if(state >= 6) state = 6;
  return cpu_state_name[state];
}

void BX_CPU_C::debug(bx_address offset)
{
  char s[256];

#if BX_SUPPORT_VMX
  BX_INFO(("CPU is in %s (%s%s)", cpu_mode_string(BX_CPU_THIS_PTR get_cpu_mode()),
    cpu_state_string(BX_CPU_THIS_PTR activity_state),
    BX_CPU_THIS_PTR in_vmx_guest ? ", vmx guest" : ""));
#else
  BX_INFO(("CPU is in %s (%s)", cpu_mode_string(BX_CPU_THIS_PTR get_cpu_mode()),
    cpu_state_string(BX_CPU_THIS_PTR activity_state)));
#endif
  BX_INFO(("CS.mode = %u bit",
    long64_mode() ? 64 : (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b ? 32 : 16)));
  BX_INFO(("SS.mode = %u bit",
    long64_mode() ? 64 : (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b ? 32 : 16)));
#if BX_CPU_LEVEL >= 5
  BX_INFO(("EFER = 0x%08x: %s", BX_CPU_THIS_PTR efer.get32(), stringify_MSR_EFER(BX_CPU_THIS_PTR efer.get32(), s)));
#endif
#if BX_CPU_LEVEL >= 6
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_XSAVE)) {
    Bit32u xcr0 = BX_CPU_THIS_PTR xcr0.get32();
    BX_INFO(("XCR0=0x%08x: %s", xcr0, stringify_XCR0(xcr0, s)));
  }
#endif
#if BX_SUPPORT_X86_64
  if (long_mode()) {
    BX_INFO(("| RAX=" FMT_ADDRX64 "  RBX=" FMT_ADDRX64 "", RAX, RBX));
    BX_INFO(("| RCX=" FMT_ADDRX64 "  RDX=" FMT_ADDRX64 "", RCX, RDX));
    BX_INFO(("| RSP=" FMT_ADDRX64 "  RBP=" FMT_ADDRX64 "", RSP, RBP));
    BX_INFO(("| RSI=" FMT_ADDRX64 "  RDI=" FMT_ADDRX64 "", RSI, RDI));
    BX_INFO(("|  R8=" FMT_ADDRX64 "   R9=" FMT_ADDRX64 "", R8,  R9));
    BX_INFO(("| R10=" FMT_ADDRX64 "  R11=" FMT_ADDRX64 "", R10, R11));
    BX_INFO(("| R12=" FMT_ADDRX64 "  R13=" FMT_ADDRX64 "", R12, R13));
    BX_INFO(("| R14=" FMT_ADDRX64 "  R15=" FMT_ADDRX64 "", R14, R15));
  }
  else
#endif
  {
    BX_INFO(("| EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x", EAX, EBX, ECX, EDX));
    BX_INFO(("| ESP=%08x  EBP=%08x  ESI=%08x  EDI=%08x", ESP, EBP, ESI, EDI));
  }

  Bit32u eflags = BX_CPU_THIS_PTR read_eflags();
  BX_INFO(("| EFLAGS=%08x: %s", eflags, stringify_EFLAGS(eflags, s)));

  BX_INFO(("| SEG sltr(index|ti|rpl)     base    limit G D"));
  BX_INFO(("|  CS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b));
  BX_INFO(("|  DS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b));
  BX_INFO(("|  SS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b));
  BX_INFO(("|  ES:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b));
  BX_INFO(("|  FS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b));
  BX_INFO(("|  GS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b));

  Bit32u cr0 = BX_CPU_THIS_PTR cr0.get32();
#if BX_CPU_LEVEL >= 5
  Bit32u cr4 = BX_CPU_THIS_PTR cr4.get32();
#endif

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    BX_INFO(("|  MSR_FS_BASE:" FMT_ADDRX64,
      MSR_FSBASE));
    BX_INFO(("|  MSR_GS_BASE:" FMT_ADDRX64,
      MSR_GSBASE));

    BX_INFO(("| RIP=" FMT_ADDRX64 " (" FMT_ADDRX64 ")",
      BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_RIP].rrx,
      BX_CPU_THIS_PTR prev_rip));
    BX_INFO(("| CR0=0x%08x: %s", cr0, stringify_CR0(cr0, s)));
    BX_INFO(("| CR2=0x" FMT_ADDRX64, BX_CPU_THIS_PTR cr2));
    BX_INFO(("| CR3=0x" FMT_ADDRX64, BX_CPU_THIS_PTR cr3));
    BX_INFO(("| CR4=0x%08x: %s", cr4, stringify_CR4(cr4, s)));
  }
  else
#endif // BX_SUPPORT_X86_64
  {
    BX_INFO(("| EIP=%08x (%08x)", (unsigned) EIP,
      (unsigned) BX_CPU_THIS_PTR prev_rip));
    BX_INFO(("| CR0=0x%08x: %s", cr0, stringify_CR0(cr0, s)));
    BX_INFO(("| CR2=0x%08x", (unsigned) BX_CPU_THIS_PTR cr2));
    BX_INFO(("| CR3=0x%08x", (unsigned) BX_CPU_THIS_PTR cr3));
#if BX_CPU_LEVEL >= 5
    BX_INFO(("| CR4=0x%08x: %s", cr4, stringify_CR4(cr4, s)));
#endif
  }

  debug_disasm_instruction(offset);
}

#if BX_DEBUGGER
void BX_CPU_C::dbg_set_eip(bx_address val)
{
  RIP = BX_CPU_THIS_PTR prev_rip = val;
  invalidate_prefetch_q();
}

bool BX_CPU_C::dbg_set_eflags(Bit32u val)
{
  // returns 1=OK, 0=can't change

  if (val & 0xffff0000) {
    BX_INFO(("dbg_set_eflags: can't set upper 16 bits of EFLAGS !"));
    return(0);
  }

  // make sure none of the system bits are being changed
  Bit32u current_sys_bits = ((BX_CPU_THIS_PTR getB_NT()) << 14) |
                             (BX_CPU_THIS_PTR get_IOPL () << 12) |
                            ((BX_CPU_THIS_PTR getB_TF()) << 8);
  if (current_sys_bits != (val & 0x0000f100)) {
    BX_INFO(("dbg_set_eflags: can't modify NT, IOPL, or TF !"));
    return(0);
  }

  BX_CPU_THIS_PTR set_CF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_PF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_AF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_ZF(val & 0x01); val >>= 1;
  BX_CPU_THIS_PTR set_SF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_DF(val & 0x01); val >>= 1;
  BX_CPU_THIS_PTR set_OF(val & 0x01);
  return(1);
}

unsigned BX_CPU_C::dbg_query_pending(void)
{
  unsigned ret = 0;

  if (BX_HRQ) {  // DMA Hold Request
    ret |= BX_DBG_PENDING_DMA;
  }

  if (is_unmasked_event_pending(BX_EVENT_PENDING_INTR)) {
    ret |= BX_DBG_PENDING_IRQ;
  }

  return ret;
}

bool BX_CPU_C::dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no)
{
  if (sreg_no > 5)
    return(0);
  sreg->valid = BX_CPU_THIS_PTR sregs[sreg_no].cache.valid;
  sreg->sel   = BX_CPU_THIS_PTR sregs[sreg_no].selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
#if BX_SUPPORT_X86_64
  sreg->dword3 = BX_CPU_THIS_PTR sregs[sreg_no].cache.u.segment.base >> 32;
#endif
  return(1);
}

bool BX_CPU_C::dbg_set_sreg(unsigned sreg_no, bx_segment_reg_t *sreg)
{
  if (sreg_no < 6) {
    BX_CPU_THIS_PTR sregs[sreg_no] = *sreg;
    if (sreg_no == BX_SEG_REG_CS) {
      handleCpuModeChange();
#if BX_CPU_LEVEL >= 4
      handleAlignmentCheck(/* CPL change */);
#endif
      invalidate_prefetch_q();
      return true;
    }
  }

  return false;
}

void BX_CPU_C::dbg_get_tr(bx_dbg_sreg_t *sreg)
{
  sreg->valid = BX_CPU_THIS_PTR tr.cache.valid;
  sreg->sel   = BX_CPU_THIS_PTR tr.selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR tr.cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR tr.cache);
#if BX_SUPPORT_X86_64
  sreg->dword3 = BX_CPU_THIS_PTR tr.cache.u.segment.base >> 32;
#endif
}

void BX_CPU_C::dbg_get_ldtr(bx_dbg_sreg_t *sreg)
{
  sreg->valid = BX_CPU_THIS_PTR ldtr.cache.valid;
  sreg->sel   = BX_CPU_THIS_PTR ldtr.selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR ldtr.cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache);
#if BX_SUPPORT_X86_64
  sreg->dword3 = BX_CPU_THIS_PTR ldtr.cache.u.segment.base >> 32;
#endif
}

void BX_CPU_C::dbg_get_gdtr(bx_global_segment_reg_t *sreg)
{
  sreg->base  = BX_CPU_THIS_PTR gdtr.base;
  sreg->limit = BX_CPU_THIS_PTR gdtr.limit;
}

void BX_CPU_C::dbg_get_idtr(bx_global_segment_reg_t *sreg)
{
  sreg->base  = BX_CPU_THIS_PTR idtr.base;
  sreg->limit = BX_CPU_THIS_PTR idtr.limit;
}

void BX_CPU_C::dbg_get_guard_state(bx_dbg_guard_state_t *guard_state)
{
  bx_address debug_eip = RIP;
  Bit16u cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

  guard_state->cs  = cs;
  guard_state->eip = debug_eip;
  guard_state->laddr = get_laddr(BX_SEG_REG_CS, debug_eip);
  guard_state->code_32_64 = BX_CPU_THIS_PTR fetchModeMask;
}

#endif  // #if BX_DEBUGGER

void BX_CPU_C::atexit(void)
{
  debug(BX_CPU_THIS_PTR prev_rip);
}
