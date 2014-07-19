/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
        if (long64_mode() && CPL != 3 && ss_selector.rpl == CPL) {
          load_null_selector(seg, new_value);
          return;
        }
#endif
        BX_ERROR(("load_seg_reg(SS): loading null selector"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

      /* selector's RPL must = CPL, else #GP(selector) */
      if (ss_selector.rpl != CPL) {
        BX_ERROR(("load_seg_reg(SS): rpl != CPL"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        BX_ERROR(("load_seg_reg(SS): valid bit cleared"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      /* AR byte must indicate a writable data segment else #GP(selector) */
      if (descriptor.segment==0 || IS_CODE_SEGMENT(descriptor.type) ||
          IS_DATA_SEGMENT_WRITEABLE(descriptor.type) == 0)
      {
        BX_ERROR(("load_seg_reg(SS): not writable data segment"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      /* DPL in the AR byte must equal CPL else #GP(selector) */
      if (descriptor.dpl != CPL) {
        BX_ERROR(("load_seg_reg(SS): dpl != CPL"));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      /* segment must be marked PRESENT else #SS(selector) */
      if (! IS_PRESENT(descriptor)) {
        BX_ERROR(("load_seg_reg(SS): not present"));
        exception(BX_SS_EXCEPTION, new_value & 0xfffc);
      }

      touch_segment(&ss_selector, &descriptor);

      /* load SS with selector, load SS cache with descriptor */
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector    = ss_selector;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache       = descriptor;
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 1;

      invalidate_stack_cache();

      return;
    }
    else if ((seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]) ||
             (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]) ||
             (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]) ||
             (seg==&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS])
            )
    {
      bx_descriptor_t descriptor;
      bx_selector_t selector;
      Bit32u dword1, dword2;

      if ((new_value & 0xfffc) == 0) { /* null selector */
        load_null_selector(seg, new_value);
        return;
      }

      parse_selector(new_value, &selector);
      fetch_raw_descriptor(&selector, &dword1, &dword2, BX_GP_EXCEPTION);
      parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        BX_ERROR(("load_seg_reg(%s, 0x%04x): invalid segment", strseg(seg), new_value));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      /* AR byte must indicate data or readable code segment else #GP(selector) */
      if (descriptor.segment==0 || (IS_CODE_SEGMENT(descriptor.type) &&
          IS_CODE_SEGMENT_READABLE(descriptor.type) == 0))
      {
        BX_ERROR(("load_seg_reg(%s, 0x%04x): not data or readable code", strseg(seg), new_value));
        exception(BX_GP_EXCEPTION, new_value & 0xfffc);
      }

      /* If data or non-conforming code, then both the RPL and the CPL
       * must be less than or equal to DPL in AR byte else #GP(selector) */
      if (IS_DATA_SEGMENT(descriptor.type) ||
          IS_CODE_SEGMENT_NON_CONFORMING(descriptor.type))
      {
        if ((selector.rpl > descriptor.dpl) || (CPL > descriptor.dpl)) {
          BX_ERROR(("load_seg_reg(%s, 0x%04x): RPL & CPL must be <= DPL", strseg(seg), new_value));
          exception(BX_GP_EXCEPTION, new_value & 0xfffc);
        }
      }

      /* segment must be marked PRESENT else #NP(selector) */
      if (! IS_PRESENT(descriptor)) {
        BX_ERROR(("load_seg_reg(%s, 0x%04x): segment not present", strseg(seg), new_value));
        exception(BX_NP_EXCEPTION, new_value & 0xfffc);
      }

      touch_segment(&selector, &descriptor);

      /* load segment register with selector */
      /* load segment register-cache with descriptor */
      seg->selector    = selector;
      seg->cache       = descriptor;
      seg->cache.valid = 1;

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

  /* Do not modify segment limit and AR bytes when in real mode */
  /* Support for big real mode */
  if (!real_mode()) {
    seg->cache.type = BX_DATA_READ_WRITE_ACCESSED;
    seg->cache.dpl = 3; /* we are in v8086 mode */
    seg->cache.u.segment.limit_scaled = 0xffff;
    seg->cache.u.segment.g     = 0; /* byte granular */
    seg->cache.u.segment.d_b   = 0; /* default 16bit size */
#if BX_SUPPORT_X86_64
    seg->cache.u.segment.l     = 0; /* default 16bit size */
#endif
    seg->cache.u.segment.avl   = 0;
  }

  if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]) {
    invalidate_prefetch_q();
    updateFetchModeMask(/* CS reloaded */);
#if BX_CPU_LEVEL >= 4
    handleAlignmentCheck(/* CPL change */);
#endif
  }

  if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS])
    invalidate_stack_cache();
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::load_null_selector(bx_segment_reg_t *seg, unsigned value)
{
  BX_ASSERT((value & 0xfffc) == 0);

  seg->selector.index = 0;
  seg->selector.ti    = 0;
  seg->selector.rpl   = BX_SELECTOR_RPL(value);
  seg->selector.value = value;

  seg->cache.valid    = 0; /* invalidate null selector */
  seg->cache.p        = 0;
  seg->cache.dpl      = 0;
  seg->cache.segment  = 1; /* data/code segment */
  seg->cache.type     = 0;

  seg->cache.u.segment.base         = 0;
  seg->cache.u.segment.limit_scaled = 0;
  seg->cache.u.segment.g            = 0;
  seg->cache.u.segment.d_b          = 0;
  seg->cache.u.segment.avl          = 0;
#if BX_SUPPORT_X86_64
  seg->cache.u.segment.l            = 0;
#endif

  invalidate_stack_cache();
}

BX_CPP_INLINE void BX_CPU_C::validate_seg_reg(unsigned seg)
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

void parse_selector(Bit16u raw_selector, bx_selector_t *selector)
{
  selector->value = raw_selector;
  selector->index = raw_selector >> 3;
  selector->ti    = (raw_selector >> 2) & 0x01;
  selector->rpl   = raw_selector & 0x03;
}

Bit8u get_ar_byte(const bx_descriptor_t *d)
{
  return (d->type) |
         (d->segment << 4) |
         (d->dpl << 5) |
         (d->p << 7);
}

void set_ar_byte(bx_descriptor_t *d, Bit8u ar_byte)
{
  d->p        = (ar_byte >> 7) & 0x01;
  d->dpl      = (ar_byte >> 5) & 0x03;
  d->segment  = (ar_byte >> 4) & 0x01;
  d->type     = (ar_byte & 0x0f);
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::get_descriptor_l(const bx_descriptor_t *d)
{
  Bit32u limit = d->u.segment.limit_scaled;
  if (d->u.segment.g)
    limit >>= 12;

  Bit32u val = ((d->u.segment.base & 0xffff) << 16) | (limit & 0xffff);

  if (d->segment || !d->valid) {
    return(val);
  }
  else {
    switch (d->type) {
      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        return(val);

      default:
        BX_ERROR(("#get_descriptor_l(): type %d not finished", d->type));
        return(0);
    }
  }
}

  Bit32u BX_CPP_AttrRegparmN(1)
BX_CPU_C::get_descriptor_h(const bx_descriptor_t *d)
{
  Bit32u val;

  Bit32u limit = d->u.segment.limit_scaled;
  if (d->u.segment.g)
    limit >>= 12;

  if (d->segment || !d->valid) {
    val = (d->u.segment.base & 0xff000000) |
          ((d->u.segment.base >> 16) & 0x000000ff) |
          (d->type << 8) |
          (d->segment << 12) |
          (d->dpl << 13) |
          (d->p << 15) | (limit & 0xf0000) |
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
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        val = ((d->u.segment.base >> 16) & 0xff) |
              (d->type << 8) |
              (d->dpl << 13) |
              (d->p << 15) | (limit & 0xf0000) |
              (d->u.segment.avl << 20) |
              (d->u.segment.d_b << 22) |
              (d->u.segment.g << 23) |
              (d->u.segment.base & 0xff000000);
        return(val);

      default:
        BX_ERROR(("#get_descriptor_h(): type %d not finished", d->type));
        return(0);
    }
  }
}

bx_bool BX_CPU_C::set_segment_ar_data(bx_segment_reg_t *seg, bx_bool valid,
            Bit16u raw_selector, bx_address base, Bit32u limit_scaled, Bit16u ar_data)
{
  parse_selector(raw_selector, &seg->selector);

  bx_descriptor_t *d = &seg->cache;

  d->p        = (ar_data >> 7) & 0x1;
  d->dpl      = (ar_data >> 5) & 0x3;
  d->segment  = (ar_data >> 4) & 0x1;
  d->type     = (ar_data & 0x0f);

  d->valid    = valid;

  if (d->segment || !valid) { /* data/code segment descriptors */
    d->u.segment.g     = (ar_data >> 15) & 0x1;
    d->u.segment.d_b   = (ar_data >> 14) & 0x1;
#if BX_SUPPORT_X86_64
    d->u.segment.l     = (ar_data >> 13) & 0x1;
#endif
    d->u.segment.avl   = (ar_data >> 12) & 0x1;

    d->u.segment.base  = base;
    d->u.segment.limit_scaled = limit_scaled;
  }
  else {
    switch(d->type) {
      case BX_SYS_SEGMENT_LDT:
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        d->u.segment.avl   = (ar_data >> 12) & 0x1;
        d->u.segment.d_b   = (ar_data >> 14) & 0x1;
        d->u.segment.g     = (ar_data >> 15) & 0x1;
        d->u.segment.base  = base;
        d->u.segment.limit_scaled = limit_scaled;
        break;

      default:
        BX_ERROR(("set_segment_ar_data(): case %u unsupported, valid=%d", (unsigned) d->type, d->valid));
    }
  }

  return d->valid;
}

void parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp)
{
  Bit8u AR_byte;
  Bit32u limit;

  AR_byte        = dword2 >> 8;
  temp->p        = (AR_byte >> 7) & 0x1;
  temp->dpl      = (AR_byte >> 5) & 0x3;
  temp->segment  = (AR_byte >> 4) & 0x1;
  temp->type     = (AR_byte & 0xf);
  temp->valid    = 0; /* start out invalid */

  if (temp->segment) { /* data/code segment descriptors */
    limit = (dword1 & 0xffff) | (dword2 & 0x000F0000);

    temp->u.segment.base       = (dword1 >> 16) | ((dword2 & 0xFF) << 16);
    temp->u.segment.g          = (dword2 & 0x00800000) > 0;
    temp->u.segment.d_b        = (dword2 & 0x00400000) > 0;
#if BX_SUPPORT_X86_64
    temp->u.segment.l          = (dword2 & 0x00200000) > 0;
#endif
    temp->u.segment.avl        = (dword2 & 0x00100000) > 0;
    temp->u.segment.base      |= (dword2 & 0xFF000000);

    if (temp->u.segment.g)
      temp->u.segment.limit_scaled = (limit << 12) | 0xfff;
    else
      temp->u.segment.limit_scaled =  limit;

    temp->valid    = 1;
  }
  else { // system & gate segment descriptors
    switch (temp->type) {
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
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_BUSY_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:
      case BX_SYS_SEGMENT_BUSY_386_TSS:
        limit = (dword1 & 0xffff) | (dword2 & 0x000F0000);
        temp->u.segment.base  = (dword1 >> 16) |
                               ((dword2 & 0xff) << 16) | (dword2 & 0xff000000);
        temp->u.segment.g     = (dword2 & 0x00800000) > 0;
        temp->u.segment.d_b   = (dword2 & 0x00400000) > 0;
        temp->u.segment.avl   = (dword2 & 0x00100000) > 0;
        if (temp->u.segment.g)
          temp->u.segment.limit_scaled = (limit << 12) | 0xfff;
        else
          temp->u.segment.limit_scaled =  limit;
        temp->valid = 1;
        break;

      default: // reserved
        temp->valid    = 0;
        break;
    }
  }
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::touch_segment(bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  if (! IS_SEGMENT_ACCESSED(descriptor->type)) {
    Bit8u AR_byte = get_ar_byte(descriptor);
    AR_byte |= 1;
    descriptor->type |= 1;

    if (selector->ti == 0) { /* GDT */
      system_write_byte(BX_CPU_THIS_PTR gdtr.base + selector->index*8 + 5, AR_byte);
    }
    else { /* LDT */
      system_write_byte(BX_CPU_THIS_PTR ldtr.cache.u.segment.base + selector->index*8 + 5, AR_byte);
    }
  }
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl)
{
  // Add cpl to the selector value.
  selector->value = (BX_SELECTOR_RPL_MASK & selector->value) | cpl;

  if ((selector->value & BX_SELECTOR_RPL_MASK) != 0)
    touch_segment(selector, descriptor);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector = *selector;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache = *descriptor;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl = cpl;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 1;

  invalidate_stack_cache();
}

void BX_CPU_C::fetch_raw_descriptor(const bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2, unsigned exception_no)
{
  Bit32u index = selector->index;
  bx_address offset;
  Bit64u raw_descriptor;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 7) > BX_CPU_THIS_PTR gdtr.limit) {
      BX_ERROR(("fetch_raw_descriptor: GDT: index (%x) %x > limit (%x)",
         index*8 + 7, index, BX_CPU_THIS_PTR gdtr.limit));
      exception(exception_no, selector->value & 0xfffc);
    }
    offset = BX_CPU_THIS_PTR gdtr.base + index*8;
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_ERROR(("fetch_raw_descriptor: LDTR.valid=0"));
      exception(exception_no, selector->value & 0xfffc);
    }
    if ((index*8 + 7) > BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled) {
      BX_ERROR(("fetch_raw_descriptor: LDT: index (%x) %x > limit (%x)",
         index*8 + 7, index, BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled));
      exception(exception_no, selector->value & 0xfffc);
    }
    offset = BX_CPU_THIS_PTR ldtr.cache.u.segment.base + index*8;
  }

  raw_descriptor = system_read_qword(offset);

  *dword1 = GET32L(raw_descriptor);
  *dword2 = GET32H(raw_descriptor);
}

  bx_bool BX_CPP_AttrRegparmN(3)
BX_CPU_C::fetch_raw_descriptor2(const bx_selector_t *selector, Bit32u *dword1, Bit32u *dword2)
{
  Bit32u index = selector->index;
  bx_address offset;
  Bit64u raw_descriptor;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 7) > BX_CPU_THIS_PTR gdtr.limit)
      return 0;
    offset = BX_CPU_THIS_PTR gdtr.base + index*8;
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_ERROR(("fetch_raw_descriptor2: LDTR.valid=0"));
      return 0;
    }
    if ((index*8 + 7) > BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled)
      return 0;
    offset = BX_CPU_THIS_PTR ldtr.cache.u.segment.base + index*8;
  }

  raw_descriptor = system_read_qword(offset);

  *dword1 = GET32L(raw_descriptor);
  *dword2 = GET32H(raw_descriptor);

  return 1;
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::fetch_raw_descriptor_64(const bx_selector_t *selector,
           Bit32u *dword1, Bit32u *dword2, Bit32u *dword3, unsigned exception_no)
{
  Bit32u index = selector->index;
  bx_address offset;
  Bit64u raw_descriptor1, raw_descriptor2;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 15) > BX_CPU_THIS_PTR gdtr.limit) {
      BX_ERROR(("fetch_raw_descriptor64: GDT: index (%x) %x > limit (%x)",
         index*8 + 15, index, BX_CPU_THIS_PTR gdtr.limit));
      exception(exception_no, selector->value & 0xfffc);
    }
    offset = BX_CPU_THIS_PTR gdtr.base + index*8;
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_ERROR(("fetch_raw_descriptor64: LDTR.valid=0"));
      exception(exception_no, selector->value & 0xfffc);
    }
    if ((index*8 + 15) > BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled) {
      BX_ERROR(("fetch_raw_descriptor64: LDT: index (%x) %x > limit (%x)",
         index*8 + 15, index, BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled));
      exception(exception_no, selector->value & 0xfffc);
    }
    offset = BX_CPU_THIS_PTR ldtr.cache.u.segment.base + index*8;
  }

  raw_descriptor1 = system_read_qword(offset);
  raw_descriptor2 = system_read_qword(offset + 8);

  if (raw_descriptor2 & BX_CONST64(0x00001F0000000000)) {
    BX_ERROR(("fetch_raw_descriptor64: extended attributes DWORD4 TYPE != 0"));
    exception(BX_GP_EXCEPTION, selector->value & 0xfffc);
  }

  *dword1 = GET32L(raw_descriptor1);
  *dword2 = GET32H(raw_descriptor1);
  *dword3 = GET32L(raw_descriptor2);
}

bx_bool BX_CPU_C::fetch_raw_descriptor2_64(const bx_selector_t *selector,
           Bit32u *dword1, Bit32u *dword2, Bit32u *dword3)
{
  Bit32u index = selector->index;
  bx_address offset;
  Bit64u raw_descriptor1, raw_descriptor2;

  if (selector->ti == 0) { /* GDT */
    if ((index*8 + 15) > BX_CPU_THIS_PTR gdtr.limit) {
      BX_ERROR(("fetch_raw_descriptor2_64: GDT: index (%x) %x > limit (%x)",
         index*8 + 15, index, BX_CPU_THIS_PTR gdtr.limit));
      return 0;
    }
    offset = BX_CPU_THIS_PTR gdtr.base + index*8;
  }
  else { /* LDT */
    if (BX_CPU_THIS_PTR ldtr.cache.valid==0) {
      BX_ERROR(("fetch_raw_descriptor2_64: LDTR.valid=0"));
      return 0;
    }
    if ((index*8 + 15) > BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled) {
      BX_ERROR(("fetch_raw_descriptor2_64: LDT: index (%x) %x > limit (%x)",
         index*8 + 15, index, BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled));
      return 0;
    }
    offset = BX_CPU_THIS_PTR ldtr.cache.u.segment.base + index*8;
  }

  raw_descriptor1 = system_read_qword(offset);
  raw_descriptor2 = system_read_qword(offset + 8);

  if (raw_descriptor2 & BX_CONST64(0x00001F0000000000)) {
    BX_ERROR(("fetch_raw_descriptor2_64: extended attributes DWORD4 TYPE != 0"));
    return 0;
  }

  *dword1 = GET32L(raw_descriptor1);
  *dword2 = GET32H(raw_descriptor1);
  *dword3 = GET32L(raw_descriptor2);

  return 1;
}
#endif
