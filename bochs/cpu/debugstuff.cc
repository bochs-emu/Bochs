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




#include "bochs.h"


  void
BX_CPU_C::debug(Bit32u offset)
{
  bx_printf("| EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x\n",
          (unsigned) EAX, (unsigned) EBX, (unsigned) ECX, (unsigned) EDX);
  bx_printf("| ESP=%08x  EBP=%08x  ESI=%08x  EDI=%08x\n",
          (unsigned) ESP, (unsigned) EBP, (unsigned) ESI, (unsigned) EDI);
  bx_printf("| IOPL=%1u %s %s %s %s %s %s %s %s\n",
    BX_CPU_THIS_PTR eflags.iopl,
    BX_CPU_THIS_PTR get_OF()       ? "OV" : "NV",
    BX_CPU_THIS_PTR eflags.df  ? "DW" : "UP",
    BX_CPU_THIS_PTR eflags.if_ ? "EI" : "DI",
    BX_CPU_THIS_PTR get_SF()       ? "NG" : "PL",
    BX_CPU_THIS_PTR get_ZF()       ? "ZR" : "NZ",
    BX_CPU_THIS_PTR get_AF()       ? "AC" : "NA",
    BX_CPU_THIS_PTR get_PF()       ? "PE" : "PO",
    BX_CPU_THIS_PTR get_CF()       ? "CY" : "NC");
  bx_printf("| SEG selector     base    limit G D\n");
  bx_printf("| SEG sltr(index|ti|rpl)     base    limit G D\n");
  bx_printf("|  DS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b);
  bx_printf("|  ES:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b);
  bx_printf("|  FS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b);
  bx_printf("|  GS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b);
  bx_printf("|  SS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b);
  bx_printf("|  CS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g,
    (unsigned) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b);
  bx_printf("| EIP=%08x (%08x)\n", (unsigned) BX_CPU_THIS_PTR eip,
    (unsigned) BX_CPU_THIS_PTR prev_eip);

#if 0
  /* (mch) Hack to display the area round EIP and prev_EIP */
  char buf[100];
  sprintf(buf, "%04x:%08x  ", BX_CPU_THIS_PTR sregs[BX_SREG_CS].selector.value, BX_CPU_THIS_PTR eip);
  for (int i = 0; i < 8; i++) {
    Bit8u data;
    BX_CPU_THIS_PTR read_virtual_byte(BX_SREG_CS, BX_CPU_THIS_PTR eip + i, &data);
    sprintf(buf+strlen(buf), "%02x ", data);
    }
  sprintf(buf+strlen(buf), "\n");
  bx_printf(buf);

  sprintf(buf, "%04x:%08x  ", BX_CPU_THIS_PTR sregs[BX_SREG_CS].selector.value, BX_CPU_THIS_PTR prev_eip);
  for (int i = 0; i < 8; i++) {
    Bit8u data;
    BX_CPU_THIS_PTR read_virtual_byte(BX_SREG_CS, BX_CPU_THIS_PTR prev_eip + i, &data);
    sprintf(buf+strlen(buf), "%02x ", data);
    }
  sprintf(buf+strlen(buf), "\n");
  bx_printf(buf);
#endif

  bx_printf(">> ");


#if BX_DISASM
  Boolean valid;
  Bit32u  phy_addr;
  Bit8u   instr_buf[32];
  char    char_buf[256];
  unsigned isize;

  dbg_xlate_linear2phy(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + offset,
                       &phy_addr, &valid);
  if (valid) {
    BX_MEM.dbg_fetch_mem(phy_addr, 16, instr_buf);
    isize = bx_disassemble.disasm(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b,
                        instr_buf, char_buf);
    for (unsigned j=0; j<isize; j++)
      bx_printf("%02x", (unsigned) instr_buf[j]);
    bx_printf(": %s\n", char_buf);
    }
  else {
    bx_printf("(instruction unavailable) page not present\n");
    }
#else
  UNUSED(offset);
#endif  // #if BX_DISASM
}


#if BX_DEBUGGER
  Bit32u
BX_CPU_C::dbg_get_reg(unsigned reg)
{
  Bit32u return_val32;

  switch (reg) {
    case BX_DBG_REG_EAX: return(EAX);
    case BX_DBG_REG_ECX: return(ECX);
    case BX_DBG_REG_EDX: return(EDX);
    case BX_DBG_REG_EBX: return(EBX);
    case BX_DBG_REG_ESP: return(ESP);
    case BX_DBG_REG_EBP: return(EBP);
    case BX_DBG_REG_ESI: return(ESI);
    case BX_DBG_REG_EDI: return(EDI);
    case BX_DBG_REG_EIP: return(EIP);
    case BX_DBG_REG_EFLAGS:
      return_val32 = dbg_get_eflags();
      return(return_val32);
    case BX_DBG_REG_CS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
    case BX_DBG_REG_SS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
    case BX_DBG_REG_DS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
    case BX_DBG_REG_ES: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
    case BX_DBG_REG_FS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
    case BX_DBG_REG_GS: return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
    default:
      bx_panic("get_reg: request for unknown register\n");
      return(0);
    }
}

  Boolean
BX_CPU_C::dbg_set_reg(unsigned reg, Bit32u val)
{
  // returns 1=OK, 0=can't change
  bx_segment_reg_t *seg;
  Bit32u current_sys_bits;

  switch (reg) {
    case BX_DBG_REG_EAX: EAX = val; return(1);
    case BX_DBG_REG_ECX: ECX = val; return(1);
    case BX_DBG_REG_EDX: EDX = val; return(1);
    case BX_DBG_REG_EBX: EBX = val; return(1);
    case BX_DBG_REG_ESP: ESP = val; return(1);
    case BX_DBG_REG_EBP: EBP = val; return(1);
    case BX_DBG_REG_ESI: ESI = val; return(1);
    case BX_DBG_REG_EDI: EDI = val; return(1);
    case BX_DBG_REG_EIP: EIP = val; return(1);
    case BX_DBG_REG_EFLAGS:
      bx_printf("dbg_set_reg: can not handle eflags yet.\n");
      if ( val & 0xffff0000 ) {
        bx_printf("dbg_set_reg: can not set upper 16 bits of eflags.\n");
        return(0);
        }
      // make sure none of the system bits are being changed
      current_sys_bits = (BX_CPU_THIS_PTR eflags.nt << 14) |
                         (BX_CPU_THIS_PTR eflags.iopl << 12) |
                         (BX_CPU_THIS_PTR eflags.tf << 8);
      if ( current_sys_bits != (val & 0x0000f100) ) {
        bx_printf("dbg_set_reg: can not modify NT, IOPL, or TF.\n");
        return(0);
        }
      BX_CPU_THIS_PTR set_CF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_PF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_AF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR set_ZF(val & 0x01); val >>= 1;
      BX_CPU_THIS_PTR set_SF(val & 0x01); val >>= 2;
      BX_CPU_THIS_PTR eflags.if_ = val & 0x01; val >>= 1;
      BX_CPU_THIS_PTR eflags.df  = val & 0x01; val >>= 1;
      BX_CPU_THIS_PTR set_OF(val & 0x01);
      if (BX_CPU_THIS_PTR eflags.if_)
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
      bx_panic("dbg_set_reg: unrecognized register ID (%u)\n", reg);
      return(0);
    }

  if (BX_CPU_THIS_PTR real_mode()) {
    seg->selector.value = val;
    seg->cache.valid = 1;
    seg->cache.p = 1;
    seg->cache.dpl = 0;
    seg->cache.segment = 1; // regular segment
    if (reg == BX_DBG_REG_CS) {
      seg->cache.u.segment.executable = 1; // code segment
      }
    else {
      seg->cache.u.segment.executable = 0; // data segment
      }
    seg->cache.u.segment.c_ed = 0;       // expand up/non-conforming
    seg->cache.u.segment.r_w = 1;        // writeable
    seg->cache.u.segment.a = 1;          // accessed
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

  unsigned
BX_CPU_C::dbg_query_pending(void)
{
  unsigned ret = 0;

  if ( BX_HRQ ) {  // DMA Hold Request
    ret |= BX_DBG_PENDING_DMA;
    }

  if ( BX_INTR && BX_CPU_THIS_PTR eflags.if_ ) {
    ret |= BX_DBG_PENDING_IRQ;
    }

  return(ret);
}



  Bit32u
BX_CPU_C::dbg_get_eflags(void)
{
  Bit32u val32;

  val32 =
    (BX_CPU_THIS_PTR get_CF()) |
    (BX_CPU_THIS_PTR eflags.bit1 << 1) |
    ((BX_CPU_THIS_PTR get_PF()) << 2) |
    (BX_CPU_THIS_PTR eflags.bit3 << 3) |
    ((BX_CPU_THIS_PTR get_AF()>0) << 4) |
    (BX_CPU_THIS_PTR eflags.bit5 << 5) |
    ((BX_CPU_THIS_PTR get_ZF()>0) << 6) |
    ((BX_CPU_THIS_PTR get_SF()>0) << 7) |
    (BX_CPU_THIS_PTR eflags.tf << 8) |
    (BX_CPU_THIS_PTR eflags.if_ << 9) |
    (BX_CPU_THIS_PTR eflags.df << 10) |
    ((BX_CPU_THIS_PTR get_OF()>0) << 11) |
    (BX_CPU_THIS_PTR eflags.iopl << 12) |
    (BX_CPU_THIS_PTR eflags.nt << 14) |
    (BX_CPU_THIS_PTR eflags.bit15 << 15) |
    (BX_CPU_THIS_PTR eflags.rf << 16) |
    (BX_CPU_THIS_PTR eflags.vm << 17);
#if BX_CPU_LEVEL >= 4
  val32 |= (BX_CPU_THIS_PTR eflags.ac << 18);
  //val32 |= (BX_CPU_THIS_PTR eflags.vif << 19);
  //val32 |= (BX_CPU_THIS_PTR eflags.vip << 20);
  val32 |= (BX_CPU_THIS_PTR eflags.id << 21);
#endif
  return(val32);
}


  Bit32u
BX_CPU_C::dbg_get_descriptor_l(bx_descriptor_t *d)
{
  Bit32u val;

  if (d->valid == 0) {
    return(0);
    }

  if (d->segment) {
    val = ((d->u.segment.base & 0xffff) << 16) |
          (d->u.segment.limit & 0xffff);
    return(val);
    }
  else {
    switch (d->type) {
      case 0: // Reserved (not yet defined)
        fprintf(stderr, "#get_descriptor_l(): type %d not finished\n",
          d->type);
        return(0);

      case 1: // available 16bit TSS
        val = ((d->u.tss286.base & 0xffff) << 16) |
               (d->u.tss286.limit & 0xffff);
        return(val);

      case 2: // LDT
        val = ((d->u.ldt.base & 0xffff) << 16) |
              d->u.ldt.limit;
        return(val);

      case 9: // available 32bit TSS
        val = ((d->u.tss386.base & 0xffff) << 16) |
               (d->u.tss386.limit & 0xffff);
        return(val);

      default:
        fprintf(stderr, "#get_descriptor_l(): type %d not finished\n",
          d->type);
        return(0);
      }
    }
}

  Bit32u
BX_CPU_C::dbg_get_descriptor_h(bx_descriptor_t *d)
{
  Bit32u val;

  if (d->valid == 0) {
    return(0);
    }

  if (d->segment) {
    val = (d->u.segment.base & 0xff000000) |
          ((d->u.segment.base >> 16) & 0x000000ff) |
          (d->u.segment.executable << 11) |
          (d->u.segment.c_ed << 10) |
          (d->u.segment.r_w << 9) |
          (d->u.segment.a << 8) |
          (d->segment << 12) |
          (d->dpl << 13) |
          (d->p << 15) |
          (d->u.segment.limit & 0xf0000) |
          (d->u.segment.avl << 20) |
          (d->u.segment.d_b << 22) |
          (d->u.segment.g << 23);
    return(val);
    }
  else {
    switch (d->type) {
      case 0: // Reserved (not yet defined)
        fprintf(stderr, "#get_descriptor_h(): type %d not finished\n",
          d->type);
        return(0);

      case 1: // available 16bit TSS
        val = ((d->u.tss286.base >> 16) & 0xff) |
              (d->type << 8) |
              (d->dpl << 13) |
              (d->p << 15);
        return(val);

      case 2: // LDT
        val = ((d->u.ldt.base >> 16) & 0xff) |
              (d->type << 8) |
              (d->dpl << 13) |
              (d->p << 15) |
              (d->u.ldt.base & 0xff000000);
        return(val);

      case 9: // available 32bit TSS
        val = ((d->u.tss386.base >> 16) & 0xff) |
              (d->type << 8) |
              (d->dpl << 13) |
              (d->p << 15) |
              (d->u.tss386.limit & 0xf0000) |
              (d->u.tss386.avl << 20) |
              (d->u.tss386.g << 23) |
              (d->u.tss386.base & 0xff000000);
        return(val);

      default:
        fprintf(stderr, "#get_descriptor_h(): type %d not finished\n",
          d->type);
        return(0);
      }
    }
}

  Boolean
BX_CPU_C::dbg_get_sreg(bx_dbg_sreg_t *sreg, unsigned sreg_no)
{
  if (sreg_no > 5)
    return(0);
  sreg->sel   = BX_CPU_THIS_PTR sregs[sreg_no].selector.value;
  sreg->des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
  sreg->des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[sreg_no].cache);
  sreg->valid = BX_CPU_THIS_PTR sregs[sreg_no].cache.valid;
  return(1);
}

  Boolean
BX_CPU_C::dbg_get_cpu(bx_dbg_cpu_t *cpu)
{
  cpu->eax = EAX;
  cpu->ebx = EBX;
  cpu->ecx = ECX;
  cpu->edx = EDX;

  cpu->ebp = EBP;
  cpu->esi = ESI;
  cpu->edi = EDI;
  cpu->esp = ESP;

  cpu->eflags = dbg_get_eflags();
  cpu->eip    = BX_CPU_THIS_PTR eip;

  cpu->cs.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  cpu->cs.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache);
  cpu->cs.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache);
  cpu->cs.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid;

  cpu->ss.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  cpu->ss.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache);
  cpu->ss.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache);
  cpu->ss.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid;

  cpu->ds.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
  cpu->ds.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache);
  cpu->ds.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache);
  cpu->ds.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid;

  cpu->es.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
  cpu->es.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache);
  cpu->es.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache);
  cpu->es.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid;

  cpu->fs.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
  cpu->fs.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache);
  cpu->fs.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache);
  cpu->fs.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid;

  cpu->gs.sel   = BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
  cpu->gs.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache);
  cpu->gs.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache);
  cpu->gs.valid = BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid;


  cpu->ldtr.sel   = BX_CPU_THIS_PTR ldtr.selector.value;
  cpu->ldtr.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR ldtr.cache);
  cpu->ldtr.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache);
  cpu->ldtr.valid = BX_CPU_THIS_PTR ldtr.cache.valid;

  cpu->tr.sel   = BX_CPU_THIS_PTR tr.selector.value;
  cpu->tr.des_l = dbg_get_descriptor_l(&BX_CPU_THIS_PTR tr.cache);
  cpu->tr.des_h = dbg_get_descriptor_h(&BX_CPU_THIS_PTR tr.cache);
  cpu->tr.valid = BX_CPU_THIS_PTR tr.cache.valid;

  cpu->gdtr.base  = BX_CPU_THIS_PTR gdtr.base;
  cpu->gdtr.limit = BX_CPU_THIS_PTR gdtr.limit;

  cpu->idtr.base  = BX_CPU_THIS_PTR idtr.base;
  cpu->idtr.limit = BX_CPU_THIS_PTR idtr.limit;

  cpu->dr0 = BX_CPU_THIS_PTR dr0;
  cpu->dr1 = BX_CPU_THIS_PTR dr1;
  cpu->dr2 = BX_CPU_THIS_PTR dr2;
  cpu->dr3 = BX_CPU_THIS_PTR dr3;
  cpu->dr6 = BX_CPU_THIS_PTR dr6;
  cpu->dr7 = BX_CPU_THIS_PTR dr7;

  cpu->tr3 = 0;
  cpu->tr4 = 0;
  cpu->tr5 = 0;
  cpu->tr6 = 0;
  cpu->tr7 = 0;

  // cr0:32=pg,cd,nw,am,wp,ne,ts,em,mp,pe
  cpu->cr0 = BX_CPU_THIS_PTR cr0.val32;
  cpu->cr1 = 0;
  cpu->cr2 = BX_CPU_THIS_PTR cr2;
  cpu->cr3 = BX_CPU_THIS_PTR cr3;
  cpu->cr4 = 0;

  cpu->inhibit_mask = BX_CPU_THIS_PTR inhibit_mask;

  return(1);
}

  Boolean
BX_CPU_C::dbg_set_cpu(bx_dbg_cpu_t *cpu)
{
  // returns 1=OK, 0=Error
  Bit32u val;
  Bit32u type;

  // =================================================
  // Do checks first, before setting any CPU registers
  // =================================================

  // CS, SS, DS, ES, FS, GS descriptor checks
  if (!cpu->cs.valid) {
    fprintf(stderr, "Error: CS not valid\n");
    return(0); // error
    }
  if ( (cpu->cs.des_h & 0x1000) == 0 ) {
    fprintf(stderr, "Error: CS not application type\n");
    return(0); // error
    }
  if ( (cpu->cs.des_h & 0x0800) == 0 ) {
    fprintf(stderr, "Error: CS not executable\n");
    return(0); // error
    }

  if (!cpu->ss.valid) {
    fprintf(stderr, "Error: SS not valid\n");
    return(0); // error
    }
  if ( (cpu->ss.des_h & 0x1000) == 0 ) {
    fprintf(stderr, "Error: SS not application type\n");
    return(0); // error
    }

  if (cpu->ds.valid) {
    if ( (cpu->ds.des_h & 0x1000) == 0 ) {
      fprintf(stderr, "Error: DS not application type\n");
      return(0); // error
      }
    }

  if (cpu->es.valid) {
    if ( (cpu->es.des_h & 0x1000) == 0 ) {
      fprintf(stderr, "Error: ES not application type\n");
      return(0); // error
      }
    }

  if (cpu->fs.valid) {
    if ( (cpu->fs.des_h & 0x1000) == 0 ) {
      fprintf(stderr, "Error: FS not application type\n");
      return(0); // error
      }
    }

  if (cpu->gs.valid) {
    if ( (cpu->gs.des_h & 0x1000) == 0 ) {
      fprintf(stderr, "Error: GS not application type\n");
      return(0); // error
      }
    }

  if (cpu->ldtr.valid) {
    if ( cpu->ldtr.des_h & 0x1000 ) {
      fprintf(stderr, "Error: LDTR not system type\n");
      return(0); // error
      }
    if ( ((cpu->ldtr.des_h >> 8) & 0x0f) != 2 ) {
      fprintf(stderr, "Error: LDTR descriptor type not LDT\n");
      return(0); // error
      }
    }

  if (cpu->tr.valid) {
    if ( cpu->tr.des_h & 0x1000 ) {
      fprintf(stderr, "Error: TR not system type\n");
      return(0); // error
      }
    type = (cpu->tr.des_h >> 8) & 0x0f;

    if ( (type != 1) && (type != 9) ) {
      fprintf(stderr, "Error: TR descriptor type not TSS\n");
      return(0); // error
      }
    }

  // =============
  // end of checks
  // =============

  EAX = cpu->eax;
  EBX = cpu->ebx;
  ECX = cpu->ecx;
  EDX = cpu->edx;
  EBP = cpu->ebp;
  ESI = cpu->esi;
  EDI = cpu->edi;
  ESP = cpu->esp;

  // eflags
  val = cpu->eflags;
  BX_CPU_THIS_PTR set_CF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_PF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_AF(val & 0x01); val >>= 2;
  BX_CPU_THIS_PTR set_ZF(val & 0x01); val >>= 1;
  BX_CPU_THIS_PTR set_SF(val & 0x01); val >>= 1;
  BX_CPU_THIS_PTR eflags.tf  = val & 0x01; val >>= 1;
  BX_CPU_THIS_PTR eflags.if_ = val & 0x01; val >>= 1;
  BX_CPU_THIS_PTR eflags.df  = val & 0x01; val >>= 1;
  BX_CPU_THIS_PTR set_OF(val & 0x01); val >>= 1;
  BX_CPU_THIS_PTR eflags.iopl = val & 0x03; val >>= 2;
  BX_CPU_THIS_PTR eflags.nt   = val & 0x01; val >>= 2;
  BX_CPU_THIS_PTR eflags.rf   = val & 0x01; val >>= 1;
  BX_CPU_THIS_PTR eflags.vm   = val & 0x01; val >>= 1;
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR eflags.ac   = val & 0x01; val >>= 1;
  //BX_CPU_THIS_PTR eflags.vif  = val & 0x01;
  val >>= 1;
  //BX_CPU_THIS_PTR eflags.vip  = val & 0x01;
  val >>= 1;
  BX_CPU_THIS_PTR eflags.id   = val & 0x01;
#endif

  BX_CPU_THIS_PTR eip = cpu->eip;



  // CS:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = cpu->cs.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = cpu->cs.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = (cpu->cs.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = cpu->cs.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid            = cpu->cs.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p                = (cpu->cs.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl              = (cpu->cs.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment          = (cpu->cs.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type             = (cpu->cs.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable = (cpu->cs.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed   = (cpu->cs.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w    = (cpu->cs.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a      = (cpu->cs.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base   = (cpu->cs.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base  |= (cpu->cs.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base  |= (cpu->cs.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit  = (cpu->cs.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit |= (cpu->cs.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g      = (cpu->cs.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b    = (cpu->cs.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl    = (cpu->cs.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit;


  // SS:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value = cpu->ss.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.index = cpu->ss.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.ti    = (cpu->ss.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl   = cpu->ss.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid            = cpu->ss.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p                = (cpu->ss.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl              = (cpu->ss.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment          = (cpu->ss.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type             = (cpu->ss.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.executable = (cpu->ss.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.c_ed   = (cpu->ss.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.r_w    = (cpu->ss.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.a      = (cpu->ss.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base   = (cpu->ss.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base  |= (cpu->ss.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base  |= (cpu->ss.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit  = (cpu->ss.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit |= (cpu->ss.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g      = (cpu->ss.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b    = (cpu->ss.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.avl    = (cpu->ss.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit;


  // DS:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = cpu->ds.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index = cpu->ds.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti    = (cpu->ds.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl   = cpu->ds.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid            = cpu->ds.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p                = (cpu->ds.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl              = (cpu->ds.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment          = (cpu->ds.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type             = (cpu->ds.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.executable = (cpu->ds.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.c_ed   = (cpu->ds.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.r_w    = (cpu->ds.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.a      = (cpu->ds.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base   = (cpu->ds.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base  |= (cpu->ds.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base  |= (cpu->ds.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit  = (cpu->ds.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit |= (cpu->ds.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g      = (cpu->ds.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b    = (cpu->ds.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl    = (cpu->ds.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit;



  // ES:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = cpu->es.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.index = cpu->es.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.ti    = (cpu->es.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.rpl   = cpu->es.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid            = cpu->es.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.p                = (cpu->es.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.dpl              = (cpu->es.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment          = (cpu->es.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.type             = (cpu->es.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.executable = (cpu->es.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.c_ed   = (cpu->es.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.r_w    = (cpu->es.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.a      = (cpu->es.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base   = (cpu->es.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base  |= (cpu->es.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.base  |= (cpu->es.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit  = (cpu->es.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit |= (cpu->es.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g      = (cpu->es.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.d_b    = (cpu->es.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.avl    = (cpu->es.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.u.segment.limit;


  // FS:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = cpu->fs.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.index = cpu->fs.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.ti    = (cpu->fs.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.rpl   = cpu->fs.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid            = cpu->fs.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.p                = (cpu->fs.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.dpl              = (cpu->fs.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.segment          = (cpu->fs.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.type             = (cpu->fs.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.executable = (cpu->fs.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.c_ed   = (cpu->fs.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.r_w    = (cpu->fs.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.a      = (cpu->fs.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base   = (cpu->fs.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base  |= (cpu->fs.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base  |= (cpu->fs.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit  = (cpu->fs.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit |= (cpu->fs.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g      = (cpu->fs.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.d_b    = (cpu->fs.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.avl    = (cpu->fs.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.limit;


  // GS:
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = cpu->gs.sel;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.index = cpu->gs.sel >> 3;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.ti    = (cpu->gs.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.rpl   = cpu->gs.sel & 0x03;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid            = cpu->gs.valid;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.p                = (cpu->gs.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.dpl              = (cpu->gs.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.segment          = (cpu->gs.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.type             = (cpu->gs.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.executable = (cpu->gs.des_h >> 11) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.c_ed   = (cpu->gs.des_h >> 10) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.r_w    = (cpu->gs.des_h >> 9) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.a      = (cpu->gs.des_h >> 8) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base   = (cpu->gs.des_l >> 16);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base  |= (cpu->gs.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base  |= (cpu->gs.des_h & 0xff000000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit  = (cpu->gs.des_l & 0xffff);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit |= (cpu->gs.des_h & 0x000f0000);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g      = (cpu->gs.des_h >> 23) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.d_b    = (cpu->gs.des_h >> 22) & 0x01;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.avl    = (cpu->gs.des_h >> 20) & 0x01;
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.g)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled =
      (BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit << 12) | 0x0fff;
  else
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit_scaled =
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.limit;

  // LDTR:
  BX_CPU_THIS_PTR ldtr.selector.value = cpu->ldtr.sel;
  BX_CPU_THIS_PTR ldtr.selector.index = cpu->ldtr.sel >> 3;
  BX_CPU_THIS_PTR ldtr.selector.ti    = (cpu->ldtr.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR ldtr.selector.rpl   = cpu->ldtr.sel & 0x03;

  BX_CPU_THIS_PTR ldtr.cache.valid            = cpu->ldtr.valid;
  BX_CPU_THIS_PTR ldtr.cache.p                = (cpu->ldtr.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR ldtr.cache.dpl              = (cpu->ldtr.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR ldtr.cache.segment          = (cpu->ldtr.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR ldtr.cache.type             = (cpu->ldtr.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR ldtr.cache.u.ldt.base       = (cpu->ldtr.des_l >> 16);
  BX_CPU_THIS_PTR ldtr.cache.u.ldt.base      |= (cpu->ldtr.des_h & 0xff) << 16;
  BX_CPU_THIS_PTR ldtr.cache.u.ldt.base      |= (cpu->ldtr.des_h & 0xff000000);
  BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit      = (cpu->ldtr.des_l & 0xffff);

  // TR
  type = (cpu->tr.des_h >> 8) & 0x0f;
  BX_CPU_THIS_PTR tr.selector.value = cpu->tr.sel;
  BX_CPU_THIS_PTR tr.selector.index = cpu->tr.sel >> 3;
  BX_CPU_THIS_PTR tr.selector.ti    = (cpu->tr.sel >> 2) & 0x01;
  BX_CPU_THIS_PTR tr.selector.rpl   = cpu->tr.sel & 0x03;

  BX_CPU_THIS_PTR tr.cache.valid            = cpu->tr.valid;
  BX_CPU_THIS_PTR tr.cache.p                = (cpu->tr.des_h >> 15) & 0x01;
  BX_CPU_THIS_PTR tr.cache.dpl              = (cpu->tr.des_h >> 13) & 0x03;
  BX_CPU_THIS_PTR tr.cache.segment          = (cpu->tr.des_h >> 12) & 0x01;
  BX_CPU_THIS_PTR tr.cache.type             = type;
  if (type == 1) { // 286 TSS
    BX_CPU_THIS_PTR tr.cache.u.tss286.base   = (cpu->tr.des_l >> 16);
    BX_CPU_THIS_PTR tr.cache.u.tss286.base  |= (cpu->tr.des_h & 0xff) << 16;
    BX_CPU_THIS_PTR tr.cache.u.tss286.limit  = (cpu->tr.des_l & 0xffff);
    }
  else { // type == 9, 386 TSS
    BX_CPU_THIS_PTR tr.cache.u.tss386.base   = (cpu->tr.des_l >> 16);
    BX_CPU_THIS_PTR tr.cache.u.tss386.base  |= (cpu->tr.des_h & 0xff) << 16;
    BX_CPU_THIS_PTR tr.cache.u.tss386.base  |= (cpu->tr.des_h & 0xff000000);
    BX_CPU_THIS_PTR tr.cache.u.tss386.limit  = (cpu->tr.des_l & 0xffff);
    BX_CPU_THIS_PTR tr.cache.u.tss386.limit |= (cpu->tr.des_h & 0x000f0000);
    BX_CPU_THIS_PTR tr.cache.u.tss386.g      = (cpu->tr.des_h >> 23) & 0x01;
    BX_CPU_THIS_PTR tr.cache.u.tss386.avl    = (cpu->tr.des_h >> 20) & 0x01;
    }


  // gdtr
  BX_CPU_THIS_PTR gdtr.base  = cpu->gdtr.base;
  BX_CPU_THIS_PTR gdtr.limit = cpu->gdtr.limit;

  // idtr
  BX_CPU_THIS_PTR idtr.base  = cpu->idtr.base;
  BX_CPU_THIS_PTR idtr.limit = cpu->idtr.limit;


  BX_CPU_THIS_PTR dr0 = cpu->dr0;
  BX_CPU_THIS_PTR dr1 = cpu->dr1;
  BX_CPU_THIS_PTR dr2 = cpu->dr2;
  BX_CPU_THIS_PTR dr3 = cpu->dr3;
  BX_CPU_THIS_PTR dr6 = cpu->dr6;
  BX_CPU_THIS_PTR dr7 = cpu->dr7;

  // BX_CPU_THIS_PTR tr3 = cpu->tr3;
  // BX_CPU_THIS_PTR tr4 = cpu->tr4;
  // BX_CPU_THIS_PTR tr5 = cpu->tr5;
  // BX_CPU_THIS_PTR tr6 = cpu->tr6;
  // BX_CPU_THIS_PTR tr7 = cpu->tr7;


  // cr0, cr1, cr2, cr3, cr4
  SetCR0(cpu->cr0);
  BX_CPU_THIS_PTR cr1 = cpu->cr1;
  BX_CPU_THIS_PTR cr2 = cpu->cr2;
  BX_CPU_THIS_PTR cr3 = cpu->cr3;
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR cr4 = cpu->cr4;
#endif

  BX_CPU_THIS_PTR inhibit_mask = cpu->inhibit_mask;

  //
  // flush cached items, prefetch, paging, etc
  //
  BX_CPU_THIS_PTR CR3_change(cpu->cr3);
  BX_CPU_THIS_PTR invalidate_prefetch_q();
  BX_CPU_THIS_PTR async_event = 1;

  return(1);
}

#if BX_SIM_ID == 0
#  define BX_DBG_NULL_CALLBACK bx_dbg_null_callback0
#else
#  define BX_DBG_NULL_CALLBACK bx_dbg_null_callback1
#endif
  void
BX_DBG_NULL_CALLBACK(unsigned val)
{
  // bochs uses the pc_system variables, so this function is
  // a stub for notification by the debugger, that a change
  // occurred.
  UNUSED(val);
}

  void
#if BX_SIM_ID == 0
bx_dbg_init_cpu_mem_env0(bx_dbg_callback_t *callback, int argc, char *argv[])
#else
bx_dbg_init_cpu_mem_env1(bx_dbg_callback_t *callback, int argc, char *argv[])
#endif
{
  UNUSED(argc);
  UNUSED(argv);

  callback->setphymem           = BX_MEM.dbg_set_mem;
  callback->getphymem           = BX_MEM.dbg_fetch_mem;
  callback->xlate_linear2phy    = BX_CPU.dbg_xlate_linear2phy;
  callback->set_reg             = BX_CPU.dbg_set_reg;
  callback->get_reg             = BX_CPU.dbg_get_reg;
  callback->get_sreg            = BX_CPU.dbg_get_sreg;
  callback->get_cpu             = BX_CPU.dbg_get_cpu;
  callback->set_cpu             = BX_CPU.dbg_set_cpu;
  callback->dirty_page_tbl_size = sizeof(BX_MEM.dbg_dirty_pages);
  callback->dirty_page_tbl      = BX_MEM.dbg_dirty_pages;
  callback->atexit              = BX_CPU.atexit;
  callback->query_pending       = BX_CPU.dbg_query_pending;
  callback->execute             = BX_CPU.cpu_loop;
  callback->take_irq            = BX_CPU.dbg_take_irq;
  callback->take_dma            = BX_CPU.dbg_take_dma;
  callback->reset_cpu           = BX_CPU.reset;
  callback->init_mem            = BX_MEM.init_memory;
  callback->load_ROM            = BX_MEM.load_ROM;
  callback->set_A20             = NULL;
  callback->set_NMI             = BX_DBG_NULL_CALLBACK;
  callback->set_RESET           = BX_DBG_NULL_CALLBACK;
  callback->set_INTR            = BX_CPU.set_INTR;
  callback->force_interrupt     = BX_CPU.dbg_force_interrupt;

#if BX_INSTRUMENTATION
  callback->instr_start         = bx_instr_start;
  callback->instr_stop          = bx_instr_stop;
  callback->instr_reset         = bx_instr_reset;
  callback->instr_print         = bx_instr_print;
#endif
#if BX_USE_LOADER
  callback->loader              = bx_dbg_loader;
#endif
  callback->crc32               = BX_MEM.dbg_crc32;
}

#endif  // #if BX_DEBUGGER

  void
BX_CPU_C::atexit(void)
{
  bx_printf("\nCPU:%u\n", BX_SIM_ID);
  if (BX_CPU.protected_mode()) bx_printf("protected mode\n");
  else if (BX_CPU.v8086_mode()) bx_printf("v8086 mode\n");
  else bx_printf("real mode\n");
  bx_printf("CS.d_b = %u bit\n",
    BX_CPU.sregs[BX_SREG_CS].cache.u.segment.d_b ? 32 : 16);
  bx_printf("SS.d_b = %u bit\n",
    BX_CPU.sregs[BX_SREG_SS].cache.u.segment.d_b ? 32 : 16);

  BX_CPU.debug(BX_CPU.prev_eip);
}
