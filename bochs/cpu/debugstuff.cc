/////////////////////////////////////////////////////////////////////////
// $Id: debugstuff.cc,v 1.84 2007-11-05 16:36:37 sshwarts Exp $
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


#if BX_DISASM
void BX_CPU_C::debug_disasm_instruction(bx_address offset)
{
  bx_phy_address phy_addr;
  Bit8u   instr_buf[16];
  char    char_buf[512];
  unsigned i=0;

  static char letters[] = "0123456789ABCDEF";
  static disassembler bx_disassemble;
  unsigned remainsInPage = 0x1000 - (offset & 0xfff);

  bx_bool valid = dbg_xlate_linear2phy(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS) + offset, &phy_addr);
  if (valid && BX_CPU_THIS_PTR mem!=NULL) {
    BX_CPU_THIS_PTR mem->dbg_fetch_mem(BX_CPU_THIS, phy_addr, 16, instr_buf);
    char_buf[i++] = '>';
    char_buf[i++] = '>';
    char_buf[i++] = ' ';
    unsigned isize = bx_disassemble.disasm(
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b,
        BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64,
        BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS), offset,
        instr_buf, char_buf+i);
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
      BX_INFO(("%s", char_buf));
    }
    else {
      BX_INFO(("(instruction unavailable) page split instruction"));
    }
  }
  else {
    BX_INFO(("(instruction unavailable) page not present"));
  }
}
#endif  // #if BX_DISASM

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

const char* cpu_state_string()
{
  unsigned cpu_state = 5; // unknown state

  static const char *cpu_state_name[] = {
     "active",
     "executing mwait",
     "waiting for SIPI",
     "in shutdown",
     "halted",
     "unknown state"
  };

  if(BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_HALT) cpu_state = 4;
  else if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_SHUTDOWN) cpu_state = 3;
  else if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_WAIT_FOR_SIPI) cpu_state = 2;
  else if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT) cpu_state = 1;
  else if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_SPECIAL) cpu_state = 5;
  else cpu_state = 0;
  return cpu_state_name[cpu_state];
}

void BX_CPU_C::debug(bx_address offset)
{
  BX_INFO(("CPU is in %s (%s)", cpu_mode_string(BX_CPU_THIS_PTR get_cpu_mode()), 
    cpu_state_string()));
  BX_INFO(("CS.d_b = %u bit",
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b ? 32 : 16));
  BX_INFO(("SS.d_b = %u bit",
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b ? 32 : 16));
#if BX_SUPPORT_X86_64
  BX_INFO(("EFER   = 0x%08x", BX_CPU_THIS_PTR get_EFER()));
  BX_INFO(("| RAX=%08x%08x  RBX=%08x%08x",
          (unsigned) (RAX >> 32), (unsigned) EAX,
          (unsigned) (RBX >> 32), (unsigned) EBX));
  BX_INFO(("| RCX=%08x%08x  RDX=%08x%08x",
          (unsigned) (RCX >> 32), (unsigned) ECX,
          (unsigned) (RDX >> 32), (unsigned) EDX));
  BX_INFO(("| RSP=%08x%08x  RBP=%08x%08x",
          (unsigned) (RSP >> 32), (unsigned) ESP,
          (unsigned) (RBP >> 32), (unsigned) EBP));
  BX_INFO(("| RSI=%08x%08x  RDI=%08x%08x",
          (unsigned) (RSI >> 32), (unsigned) ESI,
          (unsigned) (RDI >> 32), (unsigned) EDI));
  BX_INFO(("|  R8=%08x%08x   R9=%08x%08x",
          (unsigned) (R8  >> 32), (unsigned) (R8  & 0xFFFFFFFF),
          (unsigned) (R9  >> 32), (unsigned) (R9  & 0xFFFFFFFF)));
  BX_INFO(("| R10=%08x%08x  R11=%08x%08x",
          (unsigned) (R10 >> 32), (unsigned) (R10 & 0xFFFFFFFF),
          (unsigned) (R11 >> 32), (unsigned) (R11 & 0xFFFFFFFF)));
  BX_INFO(("| R12=%08x%08x  R13=%08x%08x",
          (unsigned) (R12 >> 32), (unsigned) (R12 & 0xFFFFFFFF),
          (unsigned) (R13 >> 32), (unsigned) (R13 & 0xFFFFFFFF)));
  BX_INFO(("| R14=%08x%08x  R15=%08x%08x",
          (unsigned) (R14 >> 32), (unsigned) (R14 & 0xFFFFFFFF),
          (unsigned) (R15 >> 32), (unsigned) (R15 & 0xFFFFFFFF)));
#else
  BX_INFO(("| EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x",
          (unsigned) EAX, (unsigned) EBX, (unsigned) ECX, (unsigned) EDX));
  BX_INFO(("| ESP=%08x  EBP=%08x  ESI=%08x  EDI=%08x",
          (unsigned) ESP, (unsigned) EBP, (unsigned) ESI, (unsigned) EDI));
#endif
  BX_INFO(("| IOPL=%1u %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
    BX_CPU_THIS_PTR get_IOPL(),
    BX_CPU_THIS_PTR get_ID() ? "ID" : "id",
    BX_CPU_THIS_PTR get_VIP() ? "VIP" : "vip",
    BX_CPU_THIS_PTR get_VIF() ? "VIF" : "vif",
    BX_CPU_THIS_PTR get_AC() ? "AC" : "ac",
    BX_CPU_THIS_PTR get_VM() ? "VM" : "vm",
    BX_CPU_THIS_PTR get_RF() ? "RF" : "rf",
    BX_CPU_THIS_PTR get_NT() ? "NT" : "nt",
    BX_CPU_THIS_PTR get_OF() ? "OF" : "of",
    BX_CPU_THIS_PTR get_DF() ? "DF" : "df",
    BX_CPU_THIS_PTR get_IF() ? "IF" : "if",
    BX_CPU_THIS_PTR get_TF() ? "TF" : "tf",
    BX_CPU_THIS_PTR get_SF() ? "SF" : "sf",
    BX_CPU_THIS_PTR get_ZF() ? "ZF" : "zf",
    BX_CPU_THIS_PTR get_AF() ? "AF" : "af",
    BX_CPU_THIS_PTR get_PF() ? "PF" : "pf",
    BX_CPU_THIS_PTR get_CF() ? "CF" : "cf"));

  BX_INFO(("| SEG selector     base    limit G D"));
  BX_INFO(("| SEG sltr(index|ti|rpl)     base    limit G D"));
  BX_INFO(("|  CS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b));
  BX_INFO(("|  DS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b));
  BX_INFO(("|  SS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b));
  BX_INFO(("|  ES:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b));
  BX_INFO(("|  FS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b));
  BX_INFO(("|  GS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b));
#if BX_SUPPORT_X86_64
  BX_INFO(("|  MSR_FS_BASE:%08x%08x",
    (unsigned) (MSR_FSBASE >> 32), (unsigned) (MSR_FSBASE & 0xFFFFFFFF)));
  BX_INFO(("|  MSR_GS_BASE:%08x%08x",
    (unsigned) (MSR_GSBASE >> 32), (unsigned) (MSR_GSBASE & 0xFFFFFFFF)));
#endif

#if BX_SUPPORT_X86_64
  BX_INFO(("| RIP=%08x%08x (%08x%08x)", 
    (unsigned) BX_CPU_THIS_PTR eip_reg.dword.rip_upper, (unsigned) EIP,
    (unsigned) (BX_CPU_THIS_PTR prev_eip >> 32), 
    (unsigned) (BX_CPU_THIS_PTR prev_eip & 0xffffffff)));
  BX_INFO(("| CR0=0x%08x CR1=0x%x CR2=0x%08x%08x",
    (unsigned) (BX_CPU_THIS_PTR cr0.val32), 0,
    (unsigned) (BX_CPU_THIS_PTR cr2 >> 32),
    (unsigned) (BX_CPU_THIS_PTR cr2 & 0xffffffff)));
  BX_INFO(("| CR3=0x%08x CR4=0x%08x",
    (unsigned) BX_CPU_THIS_PTR cr3, BX_CPU_THIS_PTR cr4.getRegister()));
#else
  BX_INFO(("| EIP=%08x (%08x)", (unsigned) EIP,
    (unsigned) BX_CPU_THIS_PTR prev_eip));

#if BX_CPU_LEVEL >= 2 && BX_CPU_LEVEL < 4
  BX_INFO(("| CR0=0x%08x CR1=%x CR2=0x%08x CR3=0x%08x",
    BX_CPU_THIS_PTR cr0.val32, 0,
    BX_CPU_THIS_PTR cr2,
    BX_CPU_THIS_PTR cr3));
#elif BX_CPU_LEVEL >= 4
  BX_INFO(("| CR0=0x%08x CR1=%x CR2=0x%08x",
    BX_CPU_THIS_PTR cr0.val32, 0,
    BX_CPU_THIS_PTR cr2));
  BX_INFO(("| CR3=0x%08x CR4=0x%08x",
    BX_CPU_THIS_PTR cr3,
    BX_CPU_THIS_PTR cr4.getRegister()));
#endif

#endif // BX_SUPPORT_X86_64

#if BX_DISASM
  debug_disasm_instruction(offset);
#endif  // #if BX_DISASM
}


#if BX_DEBUGGER
Bit32u BX_CPU_C::dbg_get_reg(unsigned reg)
{
  Bit32u return_val32;

  switch (reg) {
    case BX_DBG_REG_EIP: return(EIP);
    case BX_DBG_REG_EFLAGS:
      return_val32 = BX_CPU_THIS_PTR read_eflags();
      return(return_val32);
    case BX_DBG_REG_CS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
    case BX_DBG_REG_SS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
    case BX_DBG_REG_DS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
    case BX_DBG_REG_ES: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
    case BX_DBG_REG_FS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
    case BX_DBG_REG_GS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
    case BX_DBG_REG_CR0:
      return BX_CPU_THIS_PTR cr0.val32;
    case BX_DBG_REG_CR2:
      return BX_CPU_THIS_PTR cr2;
    case BX_DBG_REG_CR3:
      return BX_CPU_THIS_PTR cr3;
#if BX_CPU_LEVEL >= 4
    case BX_DBG_REG_CR4:
      return BX_CPU_THIS_PTR cr4.getRegister();
#endif
    default:
      BX_PANIC(("get_reg: request for unknown register"));
      return(0);
  }
}

bx_bool BX_CPU_C::dbg_set_reg(unsigned reg, Bit32u val)
{
  // returns 1=OK, 0=can't change
  bx_segment_reg_t *seg;
  Bit32u current_sys_bits;

  switch (reg) {
    case BX_DBG_REG_EIP: EIP = val; return(1);
    case BX_DBG_REG_EFLAGS:
      BX_INFO(("dbg_set_reg: can not handle eflags yet."));
      if (val & 0xffff0000) {
        BX_INFO(("dbg_set_reg: can not set upper 16 bits of eflags."));
        return(0);
      }
      // make sure none of the system bits are being changed
      current_sys_bits = ((BX_CPU_THIS_PTR getB_NT()) << 14) |
                         (BX_CPU_THIS_PTR get_IOPL () << 12) |
                         ((BX_CPU_THIS_PTR getB_TF()) << 8);
      if (current_sys_bits != (val & 0x0000f100)) {
        BX_INFO(("dbg_set_reg: can not modify NT, IOPL, or TF."));
        return(0);
      }
      BX_CPU_THIS_PTR set_CF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_PF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_AF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_ZF(val & 0x01); val >>= 1;
      BX_CPU_THIS_PTR set_SF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_IF(val & 0x01); val >>= 1;
      BX_CPU_THIS_PTR set_DF(val & 0x01); val >>= 1;
      BX_CPU_THIS_PTR set_OF(val & 0x01);
      if (BX_CPU_THIS_PTR get_IF())
        BX_CPU_THIS_PTR async_event = 1;
      return(1);
    case BX_DBG_REG_CS:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS];
      break;
    case BX_DBG_REG_SS:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS];
      break;
    case BX_DBG_REG_DS:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
      break;
    case BX_DBG_REG_ES:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES];
      break;
    case BX_DBG_REG_FS:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS];
      break;
    case BX_DBG_REG_GS:
      seg = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS];
      break;
    default:
      BX_PANIC(("dbg_set_reg: unrecognized register ID (%u)", reg));
      return(0);
  }

  if (real_mode()) {
    seg->selector.value = val;
    seg->cache.valid = 1;
    seg->cache.p = 1;
    seg->cache.dpl = 0;
    seg->cache.segment = 1; // regular segment
    if (reg == BX_DBG_REG_CS)
      seg->cache.type = BX_CODE_EXEC_READ_ACCESSED;
    else
      seg->cache.type = BX_DATA_READ_WRITE_ACCESSED;
    seg->cache.u.segment.base = val << 4;
    seg->cache.u.segment.limit        = 0xffff;
    seg->cache.u.segment.limit_scaled = 0xffff;
    seg->cache.u.segment.g     = 0;      // byte granular
    seg->cache.u.segment.d_b   = 0;      // default 16bit size
    seg->cache.u.segment.avl   = 0;
    return(1); // ok
  }

  return(0); // can't change when not in real mode
}

unsigned BX_CPU_C::dbg_query_pending(void)
{
  unsigned ret = 0;

  if (BX_HRQ) {  // DMA Hold Request
    ret |= BX_DBG_PENDING_DMA;
  }

  if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR get_IF()) {
    ret |= BX_DBG_PENDING_IRQ;
  }

  return(ret);
}

bx_bool BX_CPU_C::dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no)
{
  if (sreg_no > 5)
    return(0);
  sreg->sel   = BX_CPU_THIS_PTR sregs[sreg_no].selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
  sreg->valid = BX_CPU_THIS_PTR sregs[sreg_no].cache.valid;
  return(1);
}

void BX_CPU_C::dbg_get_tr(bx_dbg_sreg_t *sreg)
{
  sreg->sel   = BX_CPU_THIS_PTR tr.selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR tr.cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR tr.cache);
  sreg->valid = BX_CPU_THIS_PTR tr.cache.valid;
}

void BX_CPU_C::dbg_get_ldtr(bx_dbg_sreg_t *sreg)
{
  sreg->sel   = BX_CPU_THIS_PTR ldtr.selector.value;
  sreg->des_l = get_descriptor_l(&BX_CPU_THIS_PTR ldtr.cache);
  sreg->des_h = get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache);
  sreg->valid = BX_CPU_THIS_PTR ldtr.cache.valid;
}

void BX_CPU_C::dbg_get_gdtr(bx_dbg_global_sreg_t *sreg)
{
  sreg->base  = BX_CPU_THIS_PTR gdtr.base;
  sreg->limit = BX_CPU_THIS_PTR gdtr.limit;
}

void BX_CPU_C::dbg_get_idtr(bx_dbg_global_sreg_t *sreg)
{
  sreg->base  = BX_CPU_THIS_PTR idtr.base;
  sreg->limit = BX_CPU_THIS_PTR idtr.limit;
}

#endif  // #if BX_DEBUGGER

void BX_CPU_C::atexit(void)
{
  debug(BX_CPU_THIS_PTR prev_eip);
}
