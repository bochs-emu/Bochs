/////////////////////////////////////////////////////////////////////////
// $Id: segment_ctrl_pro.cc,v 1.76 2007-11-24 14:22:34 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value)
{
  if (protected_mode())
  {
    if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS])
    {
      bx_selector_t ss_selector;
      bx_descriptor_t descriptor;
      Bit32u dword1, dword2;

      parse_selector(new_value, &ss_selector);

      if ((new_value & 0xfffc) == 0) { /* null selector */
#if BX_SUPPORT_X86_64
        // allow SS = 0 in 64 bit mode only with cpl != 3 and rpl=cpl
        if (Is64BitMode() && CPL != 3 && ss_selector.rpl == CPL) {
          seg->selector.index = 0;
          seg->selector.ti    = 0;
          seg->selector.rpl   = 0;
          seg->selector.value = 0;
          seg->cache.valid    = 0; /* invalidate null selector */
          return;
        }
#endif
        BX_ERROR(("load_seg_reg(SS): loading null selector"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

      /* selector's RPL must = CPL, else #GP(selector) */
      if (ss_selector.rpl != CPL) {
        BX_ERROR(("load_seg_reg(SS): rpl != CPL"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        BX_ERROR(("load_seg_reg(SS): valid bit cleared"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* AR byte must indicate a writable data segment else #GP(selector) */
      if (descriptor.segment==0 || IS_CODE_SEGMENT(descriptor.type) ||
          IS_DATA_SEGMENT_WRITEABLE(descriptor.type) == 0)
      {
        BX_ERROR(("load_seg_reg(SS): not writable data segment"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* DPL in the AR byte must equal CPL else #GP(selector) */
      if (descriptor.dpl != CPL) {
        BX_ERROR(("load_seg_reg(SS): dpl != CPL"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* segment must be marked PRESENT else #SS(selector) */
      if (! IS_PRESENT(descriptor)) {
        BX_ERROR(("load_seg_reg(SS): not present"));
        exception(BX_SS_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* load SS with selector, load SS cache with descriptor */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector    = ss_selector;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache       = descriptor;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 1;

      /* now set accessed bit in descriptor */
      dword2 |= 0x0100;
      if (ss_selector.ti == 0) { /* GDT */
        access_linear(BX_CPU_THIS_PTR gdtr.base + ss_selector.index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
      }
      else { /* LDT */
        access_linear(BX_CPU_THIS_PTR ldtr.cache.u.system.base + ss_selector.index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
      }

      return;
    }
    else if ( (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]) ||
              (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES])
#if BX_CPU_LEVEL >= 3
           || (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]) ||
              (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS])
#endif
            )
    {
      bx_descriptor_t descriptor;
      bx_selector_t selector;
      Bit32u dword1, dword2;

      if ((new_value & 0xfffc) == 0) { /* null selector */
        seg->selector.index = 0;
        seg->selector.ti = 0;
        seg->selector.rpl = 0;
        seg->selector.value = 0;
        seg->cache.valid = 0; /* invalidate null selector */
        return;
      }

      parse_selector(new_value, &selector);
      fetch_raw_descriptor(&selector, &dword1, &dword2, BX_GP_EXCEPTION);
      parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        BX_ERROR(("load_seg_reg(%s): valid bit cleared", strseg(seg)));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* AR byte must indicate data or readable code segment else #GP(selector) */
      if (descriptor.segment==0 || (IS_CODE_SEGMENT(descriptor.type) && 
          IS_CODE_SEGMENT_READABLE(descriptor.type) == 0))
      {
        BX_ERROR(("load_seg_reg(%s): not data or readable code", strseg(seg)));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* If data or non-conforming code, then both the RPL and the CPL
       * must be less than or equal to DPL in AR byte else #GP(selector) */
      if (IS_DATA_SEGMENT(descriptor.type) ||
          IS_CODE_SEGMENT_NON_CONFORMING(descriptor.type))
      {
        if ((selector.rpl > descriptor.dpl) || (CPL > descriptor.dpl)) {
          BX_ERROR(("load_seg_reg(%s): RPL & CPL must be <= DPL", strseg(seg)));
          exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
        }
      }

      /* segment must be marked PRESENT else #NP(selector) */
      if (! IS_PRESENT(descriptor)) {
        BX_ERROR(("load_seg_reg(%s): segment not present", strseg(seg)));
        exception(BX_NP_EXCEPTION, new_value & 0xfffc, 0);
      }

      /* load segment register with selector */
      /* load segment register-cache with descriptor */
      seg->selector    = selector;
      seg->cache       = descriptor;
      seg->cache.valid = 1;

      /* now set accessed bit in descriptor                   */
      /* wmr: don't bother if it's already set (thus allowing */ 
      /* GDT to be in read-only pages like real hdwe does)    */

      if (!(dword2 & 0x0100)) {
        dword2 |= 0x0100;
        if (selector.ti == 0) { /* GDT */
          access_linear(BX_CPU_THIS_PTR gdtr.base + selector.index*8 + 4, 4, 0,
            BX_WRITE, &dword2);
        }
        else { /* LDT */
         access_linear(BX_CPU_THIS_PTR ldtr.cache.u.system.base + selector.index*8 + 4, 4, 0,
            BX_WRITE, &dword2);
        }
      }
      return;
    }
    else {
      BX_PANIC(("load_seg_reg(): invalid segment register passed!"));
      return;
    }
  }

  /* real or v8086 mode */

  /* www.x86.org:
    According  to  Intel, each time any segment register is loaded in real
    mode,  the  base  address is calculated as 16 times the segment value,
    while  the  access  rights  and size limit attributes are given fixed,
    "real-mode  compatible" values. This is not true. In fact, only the CS
    descriptor  caches  for  the  286,  386, and 486 get loaded with fixed
    values  each  time  the segment register is loaded. Loading CS, or any
    other segment register in real mode, on later Intel processors doesn't
    change  the  access rights or the segment size limit attributes stored
    in  the  descriptor  cache  registers.  For these segments, the access
    rights and segment size limit attributes from any previous setting are
    honored. */

  seg->selector.value = new_value;
  seg->selector.rpl = real_mode() ? 0 : 3;
  seg->cache.valid = 1;
  seg->cache.u.segment.base = new_value << 4;
  seg->cache.segment = 1; /* regular segment */
  seg->cache.p = 1; /* present */

  if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]) {
    seg->cache.type = BX_CODE_EXEC_READ_ACCESSED;
#if BX_SUPPORT_ICACHE
    BX_CPU_THIS_PTR updateFetchModeMask();
#endif
#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
    handleAlignmentCheck(); // CPL was modified
#endif
    invalidate_prefetch_q();
  }
  else {
    seg->cache.type = BX_DATA_READ_WRITE_ACCESSED;
  }

  /* Do not modify segment limit and AR bytes when in real mode */
  if (real_mode()) return;

  seg->cache.dpl = 3; /* we are in v8086 mode */
  seg->cache.u.segment.limit        = 0xffff;
  seg->cache.u.segment.limit_scaled = 0xffff;
#if BX_CPU_LEVEL >= 3
  seg->cache.u.segment.g     = 0; /* byte granular */
  seg->cache.u.segment.d_b   = 0; /* default 16bit size */
#if BX_SUPPORT_X86_64
  seg->cache.u.segment.l     = 0; /* default 16bit size */
#endif
  seg->cache.u.segment.avl   = 0;
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::loadSRegLMNominal(unsigned segI, unsigned selector, unsigned dpl)
{
  bx_segment_reg_t *seg = & BX_CPU_THIS_PTR sregs[segI];

  // Load a segment register in long-mode with nominal values,
  // so descriptor cache values are compatible with existing checks.
  seg->cache.u.segment.base = 0;
  // I doubt we need limit_scaled.  If we do, it should be
  // of type bx_addr and be maxed to 64bits, not 32.
  seg->cache.u.segment.limit_scaled = 0xffffffff;
  seg->cache.valid = 1;
  seg->cache.dpl = dpl;

  seg->selector.value = selector;
}
#endif

void BX_CPU_C::validate_seg_reg(unsigned seg)
{
  /*
     FOR (seg = ES, DS, FS, GS)
     DO
       IF ((seg.attr.dpl < CPL) && ((seg.attr.type = 'data')
                || (seg.attr.type = 'non-conforming-code')))
       {
          seg = NULL // can't use lower dpl data segment at higher cpl
       }
     END
  */

  bx_segment_reg_t *segment = &BX_CPU_THIS_PTR sregs[seg];

  if (segment->cache.dpl < CPL)
  {
    // invalidate if data or non-conforming code segment
    if (segment->cache.valid==0 || segment->cache.segment==0 ||
        IS_DATA_SEGMENT(segment->cache.type) ||
        IS_CODE_SEGMENT_NON_CONFORMING(segment->cache.type))
    {
      segment->selector.value = 0;
      segment->cache.valid = 0;
    }
  }
}

void BX_CPU_C::validate_seg_regs(void)
{
  validate_seg_reg(BX_SEG_REG_ES);
  validate_seg_reg(BX_SEG_REG_DS);
  validate_seg_reg(BX_SEG_REG_FS);
  validate_seg_reg(BX_SEG_REG_GS);
}

#if BX_CPU_LEVEL >= 2
  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::parse_selector(Bit16u raw_selector, bx_selector_t *selector)
{
  selector->value = raw_selector;
  selector->index = raw_selector >> 3;
  selector->ti    = (raw_selector >> 2) & 0x01;
  selector->rpl   = raw_selector & 0x03;
}
#endif

  Bit8u  BX_CPP_AttrRegparmN(1)
BX_CPU_C::ar_byte(const bx_descriptor_t *d)
{
  if (d->valid == 0) {
    return(0);
  }

  return (d->type) |
         (d->segment << 4) |
         (d->dpl << 5) |
         (d->p << 7);
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::set_ar_byte(bx_descriptor_t *d, Bit8u ar_byte)
{
  d->p        = (ar_byte >> 7) & 0x01;
  d->dpl      = (ar_byte >> 5) & 0x03;
  d->segment  = (ar_byte >> 4) & 0x01;
  d->type     = (ar_byte & 0x0f);
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::get_descriptor_l(const bx_descriptor_t *d)
{
  Bit32u val;

  if (d->valid == 0) {
    return(0);
  }

  if (d->segment) {
    val = ((d->u.segment.base & 0xffff) << 16) | (d->u.segment.limit & 0xffff);
    return(val);
  }
  else {
    switch (d->type) {
      case 0: // Reserved (not defined)
        BX_ERROR(("#get_descriptor_l(): type %d not finished", d->type));
        return(0);

      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        val = ((d->u.system.base & 0xffff) << 16) | (d->u.system.limit & 0xffff);
        return(val);

      default:
        BX_PANIC(("#get_descriptor_l(): type %d not finished", d->type));
        return(0);
    }
  }
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::get_descriptor_h(const bx_descriptor_t *d)
{
  Bit32u val;

  if (d->valid == 0) {
    return(0);
  }

  if (d->segment) {
    val = (d->u.segment.base & 0xff000000) |
          ((d->u.segment.base >> 16) & 0x000000ff) |
          (d->type << 8) |
          (d->segment << 12) |
          (d->dpl << 13) |
          (d->p << 15) |
          (d->u.segment.limit & 0xf0000) |
          (d->u.segment.avl << 20) |
#if BX_SUPPORT_X86_64
          (d->u.segment.l << 21) |
#endif
          (d->u.segment.d_b << 22) |
          (d->u.segment.g << 23);
    return(val);
  }
  else {
    switch (d->type) {
      case 0: // Reserved (not yet defined)
        BX_ERROR(("#get_descriptor_h(): type %d not finished", d->type));
        return(0);

      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
        BX_ASSERT(d->u.system.g   == 0);
        BX_ASSERT(d->u.system.avl == 0);
        // fall through
      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        val = ((d->u.system.base >> 16) & 0xff) |
              (d->type << 8) |
              (d->dpl << 13) |
              (d->p << 15) |
              (d->u.system.limit & 0xf0000) |
              (d->u.system.avl << 20) |
              (d->u.system.g << 23) |
              (d->u.system.base & 0xff000000);
        return(val);

      default:
        BX_PANIC(("#get_descriptor_h(): type %d not finished", d->type));
        return(0);
    }
  }
}

#if BX_CPU_LEVEL >= 3
  Bit16u BX_CPP_AttrRegparmN(1)
BX_CPU_C::get_segment_ar_data(const bx_descriptor_t *d)  // used for SMM
{
  Bit16u val = 0;

  if (d->segment) { /* data/code segment descriptors */
    val = (d->type) |
          (d->segment << 4) |
          (d->dpl << 5) |
          (d->p << 7) |
          (d->u.segment.avl << 12) |
#if BX_SUPPORT_X86_64
          (d->u.segment.l << 13) |
#endif
          (d->u.segment.d_b << 14) |
          (d->u.segment.g << 15);
    return(val);
  }

  switch (d->type) {
    case BX_SYS_SEGMENT_AVAIL_286_TSS:
    case BX_SYS_SEGMENT_BUSY_286_TSS:
        BX_ASSERT(d->u.system.g   == 0);
        BX_ASSERT(d->u.system.avl == 0);
        // fall through
    case BX_SYS_SEGMENT_LDT:
    case BX_SYS_SEGMENT_AVAIL_386_TSS:
    case BX_SYS_SEGMENT_BUSY_386_TSS:
        val = (d->type) |
              (d->dpl << 5) |
              (d->p << 7) |
              (d->u.system.avl << 12) |
              (d->u.system.g << 15);
        return(val);
    default:
        BX_PANIC(("get_segment_ar_data(): case %u unsupported", (unsigned) d->type));
  }

  return val;
}

bx_bool BX_CPU_C::set_segment_ar_data(bx_segment_reg_t *seg, 
            Bit16u raw_selector, bx_address base, Bit32u limit, Bit16u ar_data)
{
  parse_selector(raw_selector, &seg->selector);

  bx_descriptor_t *d = &seg->cache;

  d->p        = (ar_data >> 7) & 0x01;
  d->dpl      = (ar_data >> 5) & 0x03;
  d->segment  = (ar_data >> 4) & 0x01;
  d->type     = (ar_data & 0x0f);

  if (d->segment) { /* data/code segment descriptors */
    d->u.segment.g     = (ar_data >> 15) & 0x01;
    d->u.segment.d_b   = (ar_data >> 14) & 0x01;
#if BX_SUPPORT_X86_64
    d->u.segment.l     = (ar_data >> 13) & 0x01;
#endif
    d->u.segment.avl   = (ar_data >> 12) & 0x01;

    d->u.segment.base  = base;
    d->u.segment.limit = limit;

    if (d->u.segment.g) {
      if (IS_DATA_SEGMENT(d->type) && IS_DATA_SEGMENT_EXPAND_DOWN(d->type))
        d->u.segment.limit_scaled = (d->u.segment.limit << 12);
      else
        d->u.segment.limit_scaled = (d->u.segment.limit << 12) | 0x0fff;
    }
    else
      d->u.segment.limit_scaled = d->u.segment.limit;

    d->valid = 1;
  }
  else {
    switch(d->type) {
      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        d->valid       = 1;
        d->u.system.avl   = (ar_data >> 12) & 0x01;
        d->u.system.g     = (ar_data >> 15) & 0x01;
        d->u.system.base  = base;
        d->u.system.limit = limit;
        if (d->u.system.g)
          d->u.system.limit_scaled = (d->u.system.limit << 12) | 0x0fff;
        else
          d->u.system.limit_scaled = (d->u.system.limit);
        break;

      default:
        BX_PANIC(("set_segment_ar_data(): case %u unsupported", (unsigned) d->type));
    }
  }

  /* invalidate if null selector */
  if ((raw_selector & 0xfffc) == 0) {
     seg->cache.valid = 0;
  }

  return seg->cache.valid;
}
#endif

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp)
{
  Bit8u AR_byte;

  AR_byte        = dword2 >> 8;
  temp->p        = (AR_byte >> 7) & 0x01;
  temp->dpl      = (AR_byte >> 5) & 0x03;
  temp->segment  = (AR_byte >> 4) & 0x01;
  temp->type     = (AR_byte & 0x0f);
  temp->valid    = 0; /* start out invalid */

  if (temp->segment) { /* data/code segment descriptors */
    temp->u.segment.limit      = (dword1 & 0xffff);
    temp->u.segment.base       = (dword1 >> 16) | ((dword2 & 0xFF) << 16);

#if BX_CPU_LEVEL >= 3
    temp->u.segment.limit     |= (dword2 & 0x000F0000);
    temp->u.segment.g          = (dword2 & 0x00800000) > 0;
    temp->u.segment.d_b        = (dword2 & 0x00400000) > 0;
#if BX_SUPPORT_X86_64
    temp->u.segment.l          = (dword2 & 0x00200000) > 0;
#endif
    temp->u.segment.avl        = (dword2 & 0x00100000) > 0;
    temp->u.segment.base      |= (dword2 & 0xFF000000);

    if (temp->u.segment.g) {
      if (IS_DATA_SEGMENT(temp->type) && IS_DATA_SEGMENT_EXPAND_DOWN(temp->type))
        temp->u.segment.limit_scaled = (temp->u.segment.limit << 12);
      else
        temp->u.segment.limit_scaled = (temp->u.segment.limit << 12) | 0x0fff;
      }
    else
#endif
      temp->u.segment.limit_scaled = temp->u.segment.limit;

    temp->valid    = 1;
  }
  else { // system & gate segment descriptors
    switch (temp->type) {
      case  0: // reserved
      case  8: // reserved
      case 10: // reserved
      case 13: // reserved
        temp->valid    = 0;
        break;

      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
        temp->u.system.base  = (dword1 >> 16) | ((dword2 & 0xff) << 16);
        temp->u.system.limit = (dword1 & 0xffff);
        temp->u.system.limit_scaled = temp->u.system.limit;
        temp->u.system.g     = 0;
        temp->u.system.avl   = 0;
        temp->valid       = 1;
        break;

      case BX_286_CALL_GATE:
      case BX_286_INTERRUPT_GATE:
      case BX_286_TRAP_GATE:
        // param count only used for call gate
        temp->u.gate.param_count   = dword2 & 0x1f;
        temp->u.gate.dest_selector = dword1 >> 16;
        temp->u.gate.dest_offset   = dword1 & 0xffff;
        temp->valid = 1;
        break;

      case BX_386_CALL_GATE:
      case BX_386_INTERRUPT_GATE:
      case BX_386_TRAP_GATE:
        // param count only used for call gate
        temp->u.gate.param_count   = dword2 & 0x1f;
        temp->u.gate.dest_selector = dword1 >> 16;
        temp->u.gate.dest_offset   = (dword2 & 0xffff0000) |
                                     (dword1 & 0x0000ffff);
        temp->valid = 1;
        break;

      case BX_TASK_GATE:
        temp->u.taskgate.tss_selector = dword1 >> 16;
        temp->valid = 1;
        break;

      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        temp->u.system.base  = (dword1 >> 16) |
                           ((dword2 & 0xff) << 16) | (dword2 & 0xff000000);
        temp->u.system.limit = (dword1 & 0x0000ffff)  | (dword2 & 0x000f0000);
        temp->u.system.g     = (dword2 & 0x00800000) > 0;
        temp->u.system.avl   = (dword2 & 0x00100000) > 0;
        if (temp->u.system.g)
          temp->u.system.limit_scaled = (temp->u.system.limit << 12) | 0x0fff;
        else
          temp->u.system.limit_scaled = (temp->u.system.limit);
        temp->valid = 1;
        break;

      default: 
        BX_PANIC(("parse_descriptor(): case %u unfinished", (unsigned) temp->type));
        temp->valid = 0;
    }
  }
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl)
{
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector = *selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache = *descriptor;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl = cpl;

  // Add cpl to the selector value.
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value =
    (0xfffc & BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value) | cpl;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    loadSRegLMNominal(BX_SEG_REG_SS, selector->value, cpl);
    return;
  }
#endif
  if ((BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value & 0xfffc) == 0)
    BX_PANIC(("load_ss(): null selector passed"));

  if (!BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid) {
    BX_PANIC(("load_ss(): invalid selector/descriptor passed."));
  }
}

#if BX_CPU_LEVEL >= 2
  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::fetch_raw_descriptor(const bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2, unsigned exception_no)
{
  Bit32u index = selector->index;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 7) > BX_CPU_THIS_PTR gdtr.limit) {
      BX_ERROR(("fetch_raw_descriptor: GDT: index (%x)%x > limit (%x)",
          index*8 + 7, index, BX_CPU_THIS_PTR gdtr.limit));
      exception(exception_no, selector->value & 0xfffc, 0);
    }
    bx_address offset = BX_CPU_THIS_PTR gdtr.base + index*8;
    access_linear(offset,     4, 0, BX_READ, dword1);
    access_linear(offset + 4, 4, 0, BX_READ, dword2);
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_PANIC(("fetch_raw_descriptor: LDTR.valid=0"));
      debug(BX_CPU_THIS_PTR prev_rip);
    }
    if ((index*8 + 7) > BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled) {
      BX_ERROR(("fetch_raw_descriptor: LDT: index (%x)%x > limit (%x)",
          index*8 + 7, index, BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled));
      exception(exception_no, selector->value & 0xfffc, 0);
    }
    bx_address offset = BX_CPU_THIS_PTR ldtr.cache.u.system.base + index*8;
    access_linear(offset,     4, 0, BX_READ, dword1);
    access_linear(offset + 4, 4, 0, BX_READ, dword2);
  }
}

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::fetch_raw_descriptor2(const bx_selector_t *selector, Bit32u *dword1, Bit32u *dword2)
{
  Bit32u index = selector->index;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 7) > BX_CPU_THIS_PTR gdtr.limit)
      return(0);
    bx_address offset = BX_CPU_THIS_PTR gdtr.base + index*8;
    access_linear(offset,     4, 0, BX_READ, dword1);
    access_linear(offset + 4, 4, 0, BX_READ, dword2);
    return(1);
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_PANIC(("fetch_raw_descriptor2: LDTR.valid=0"));
      return(0);
    }
    if ((index*8 + 7) > BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled)
      return(0);
    bx_address offset = BX_CPU_THIS_PTR ldtr.cache.u.system.base + index*8;
    access_linear(offset,     4, 0, BX_READ, dword1);
    access_linear(offset + 4, 4, 0, BX_READ, dword2);
    return(1);
  }
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::fetch_raw_descriptor64(const bx_selector_t *selector,
           Bit32u *dword1, Bit32u *dword2, Bit32u *dword3, unsigned exception_no)
{
  Bit32u index = selector->index;
  Bit32u dword4;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 15) > BX_CPU_THIS_PTR gdtr.limit) {
      BX_ERROR(("fetch_raw_descriptor64: GDT: index (%x)%x > limit (%x)",
          index*8 + 15, index, BX_CPU_THIS_PTR gdtr.limit));
      exception(exception_no, selector->value & 0xfffc, 0);
    }
    bx_address offset = BX_CPU_THIS_PTR gdtr.base + index*8;
    access_linear(offset,      4, 0, BX_READ,  dword1);
    access_linear(offset +  4, 4, 0, BX_READ,  dword2);
    access_linear(offset +  8, 4, 0, BX_READ,  dword3);
    access_linear(offset + 12, 4, 0, BX_READ, &dword4);
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_PANIC(("fetch_raw_descriptor: LDTR.valid=0"));
      debug(BX_CPU_THIS_PTR prev_rip);
    }
    if ((index*8 + 15) > BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled) {
      BX_ERROR(("fetch_raw_descriptor64: LDT: index (%x)%x > limit (%x)",
          index*8 + 15, index, BX_CPU_THIS_PTR ldtr.cache.u.system.limit_scaled));
      exception(exception_no, selector->value & 0xfffc, 0);
    }
    bx_address offset = BX_CPU_THIS_PTR ldtr.cache.u.system.base + index*8;
    access_linear(offset,      4, 0, BX_READ,  dword1);
    access_linear(offset +  4, 4, 0, BX_READ,  dword2);
    access_linear(offset +  8, 4, 0, BX_READ,  dword3);
    access_linear(offset + 12, 4, 0, BX_READ, &dword4);
  }

  if (dword4 != 0) {
    BX_ERROR(("fetch_raw_descriptor64: extended attributes DWORD4 != 0"));
    exception(BX_GP_EXCEPTION, selector->value & 0xfffc, 0);
  }
}
#endif

#endif
