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





#include "bochs.h"





  void
BX_CPU_C::write_virtual_checks(bx_segment_reg_t *seg, Bit32u offset,
                               unsigned length)
{
  Bit32u upper_limit;


  if ( protected_mode() ) {
    if ( seg->cache.valid==0 ) {
      BX_CPU_THIS_PTR info("seg = %s\n", BX_CPU_THIS_PTR strseg(seg));
      BX_CPU_THIS_PTR info("seg->selector.value = %04x\n", (unsigned) seg->selector.value);
      BX_CPU_THIS_PTR info("write_virtual_checks: valid bit = 0\n");
BX_CPU_THIS_PTR info("CS: %04x\n", (unsigned) BX_CPU_THIS_PTR sregs[1].selector.value);
BX_CPU_THIS_PTR info("IP: %04x\n", (unsigned) BX_CPU_THIS_PTR prev_eip);
debug(BX_CPU_THIS_PTR eip);
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (seg->cache.p == 0) { /* not present */
BX_CPU_THIS_PTR info("write_virtual_checks(): segment not present\n");
      exception(int_number(seg), 0, 0);
      return;
      }

    switch ( seg->cache.type ) {
      case 0: case 1:   // read only
      case 4: case 5:   // read only, expand down
      case 8: case 9:   // execute only
      case 10: case 11: // execute/read
      case 12: case 13: // execute only, conforming
      case 14: case 15: // execute/read-only, conforming
BX_CPU_THIS_PTR info("write_virtual_checks(): no write access to seg\n");
        exception(int_number(seg), 0, 0);
        return;

      case 2: case 3: /* read/write */
        if ( (offset+length-1) > seg->cache.u.segment.limit_scaled ) {
BX_CPU_THIS_PTR info("write_virtual_checks(): write beyond limit, r/w\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 6: case 7: /* read write, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
BX_CPU_THIS_PTR info("write_virtual_checks(): write beyond limit, r/w ED\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;
      }

    return;
    }

  else { /* real mode */
    if ( (offset + length - 1)  >  seg->cache.u.segment.limit_scaled) {
      //BX_CPU_THIS_PTR info("write_virtual_checks() SEG EXCEPTION:  %x:%x + %x\n",
      //  (unsigned) seg->selector.value, (unsigned) offset, (unsigned) length);
      if (seg == & BX_CPU_THIS_PTR sregs[2]) exception(BX_SS_EXCEPTION, 0, 0);
      else exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
}

  void
BX_CPU_C::read_virtual_checks(bx_segment_reg_t *seg, Bit32u offset,
                              unsigned length)
{
  Bit32u upper_limit;


  if ( protected_mode() ) {
    if ( seg->cache.valid==0 ) {
      BX_CPU_THIS_PTR info("seg = %s\n", BX_CPU_THIS_PTR strseg(seg));
      BX_CPU_THIS_PTR info("seg->selector.value = %04x\n", (unsigned) seg->selector.value);
      //BX_CPU_THIS_PTR info("read_virtual_checks: valid bit = 0\n");
      //BX_CPU_THIS_PTR info("CS: %04x\n", (unsigned)
      //   BX_CPU_THIS_PTR sregs[1].selector.value);
      //BX_CPU_THIS_PTR info("IP: %04x\n", (unsigned) BX_CPU_THIS_PTR prev_eip);
      //debug(BX_CPU_THIS_PTR eip);
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (seg->cache.p == 0) { /* not present */
BX_CPU_THIS_PTR info("read_virtual_checks(): segment not present\n");
      exception(int_number(seg), 0, 0);
      return;
      }

    switch ( seg->cache.type ) {
      case 0: case 1: /* read only */
      case 10: case 11: /* execute/read */
      case 14: case 15: /* execute/read-only, conforming */
        if ( (offset+length-1) > seg->cache.u.segment.limit_scaled ) {
BX_CPU_THIS_PTR info("read_virtual_checks(): write beyond limit\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 2: case 3: /* read/write */
        if ( (offset+length-1) > seg->cache.u.segment.limit_scaled ) {
BX_CPU_THIS_PTR info("read_virtual_checks(): write beyond limit\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 4: case 5: /* read only, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
BX_CPU_THIS_PTR info("read_virtual_checks(): write beyond limit\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 6: case 7: /* read write, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
BX_CPU_THIS_PTR info("read_virtual_checks(): write beyond limit\n");
          exception(int_number(seg), 0, 0);
          return;
          }
        break;

      case 8: case 9: /* execute only */
      case 12: case 13: /* execute only, conforming */
        /* can't read or write an execute-only segment */
BX_CPU_THIS_PTR info("read_virtual_checks(): execute only\n");
        exception(int_number(seg), 0, 0);
        return;
        break;
      }
    return;
    }

  else { /* real mode */
    if ( (offset + length - 1)  >  seg->cache.u.segment.limit_scaled) {
      //BX_CPU_THIS_PTR info("read_virtual_checks() SEG EXCEPTION:  %x:%x + %x\n",
      //  (unsigned) seg->selector.value, (unsigned) offset, (unsigned) length);
      if (seg == & BX_CPU_THIS_PTR sregs[2]) exception(BX_SS_EXCEPTION, 0, 0);
      else exception(BX_GP_EXCEPTION, 0, 0);
      }
    return;
    }
}




  char *
BX_CPU_C::strseg(bx_segment_reg_t *seg)
{
  if (seg == &BX_CPU_THIS_PTR sregs[0]) return("ES");
  else if (seg == & BX_CPU_THIS_PTR sregs[1]) return("CS");
  else if (seg == & BX_CPU_THIS_PTR sregs[2]) return("SS");
  else if (seg == &BX_CPU_THIS_PTR sregs[3]) return("DS");
  else if (seg == &BX_CPU_THIS_PTR sregs[4]) return("FS");
  else if (seg == &BX_CPU_THIS_PTR sregs[5]) return("GS");
  else {
    BX_CPU_THIS_PTR info("undefined segment passed to strseg()!\n");
    return("??");
    }
}




  void
BX_CPU_C::write_virtual_byte(unsigned s, Bit32u offset, Bit8u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 1);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 1, BX_WRITE);

  // all checks OK
  access_linear(laddr, 1, CPL==3, BX_WRITE, (void *) data);
}

  void
BX_CPU_C::write_virtual_word(unsigned s, Bit32u offset, Bit16u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 2);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 2, BX_WRITE);

  // all checks OK
  access_linear(laddr, 2, CPL==3, BX_WRITE, (void *) data);
}

  void
BX_CPU_C::write_virtual_dword(unsigned s, Bit32u offset, Bit32u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 4);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 4, BX_WRITE);

  // all checks OK
  access_linear(laddr, 4, CPL==3, BX_WRITE, (void *) data);
}

  void
BX_CPU_C::read_virtual_byte(unsigned s, Bit32u offset, Bit8u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  read_virtual_checks(seg, offset, 1);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 1, BX_READ);

  // all checks OK
  access_linear(laddr, 1, CPL==3, BX_READ, (void *) data);
}


  void
BX_CPU_C::read_virtual_word(unsigned s, Bit32u offset, Bit16u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  read_virtual_checks(seg, offset, 2);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 2, BX_READ);

  // all checks OK
  access_linear(laddr, 2, CPL==3, BX_READ, (void *) data);
}


  void
BX_CPU_C::read_virtual_dword(unsigned s, Bit32u offset, Bit32u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  read_virtual_checks(seg, offset, 4);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 4, BX_READ);

  // all checks OK
  access_linear(laddr, 4, CPL==3, BX_READ, (void *) data);
}

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  void
BX_CPU_C::read_RMW_virtual_byte(unsigned s, Bit32u offset, Bit8u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 1);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 1, BX_READ);

  // all checks OK
#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg)
    access_linear(laddr, 1, CPL==3, BX_RW, (void *) data);
  else
#endif
    {
    BX_CPU_THIS_PTR address_xlation.paddress1 = laddr;
    BX_INSTR_LIN_READ(laddr, laddr, 1);
    BX_INSTR_LIN_WRITE(laddr, laddr, 1);
    BX_MEM.read_physical(laddr, 1, data);
    }
}


  void
BX_CPU_C::read_RMW_virtual_word(unsigned s, Bit32u offset, Bit16u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 2);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 2, BX_READ);

  // all checks OK
#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg)
    access_linear(laddr, 2, CPL==3, BX_RW, (void *) data);
  else
#endif
    {
    BX_CPU_THIS_PTR address_xlation.paddress1 = laddr;
    BX_INSTR_LIN_READ(laddr, laddr, 2);
    BX_INSTR_LIN_WRITE(laddr, laddr, 2);
    BX_MEM.read_physical(laddr, 2, data);
    }
}

  void
BX_CPU_C::read_RMW_virtual_dword(unsigned s, Bit32u offset, Bit32u *data)
{
  Bit32u laddr;
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[s];
  write_virtual_checks(seg, offset, 4);

  laddr = seg->cache.u.segment.base + offset;
  BX_INSTR_MEM_DATA(laddr, 4, BX_READ);

  // all checks OK
#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg)
    access_linear(laddr, 4, CPL==3, BX_RW, (void *) data);
  else
#endif
    {
    BX_CPU_THIS_PTR address_xlation.paddress1 = laddr;
    BX_INSTR_LIN_READ(laddr, laddr, 4);
    BX_INSTR_LIN_WRITE(laddr, laddr, 4);
    BX_MEM.read_physical(laddr, 4, data);
    }
}

  void
BX_CPU_C::write_RMW_virtual_byte(Bit8u val8)
{
  BX_INSTR_MEM_DATA(BX_CPU_THIS_PTR address_xlation.paddress1, 1, BX_WRITE);

#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg) {
    // BX_CPU_THIS_PTR address_xlation.pages must be 1
    BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val8);
    }
  else
#endif
    {
    BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val8);
    }
}

  void
BX_CPU_C::write_RMW_virtual_word(Bit16u val16)
{
  BX_INSTR_MEM_DATA(BX_CPU_THIS_PTR address_xlation.paddress1, 2, BX_WRITE);

#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg) {
    if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 2, &val16);
      }
    else {
#ifdef BX_LITTLE_ENDIAN
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1,
                            &val16);
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1,
                            ((Bit8u *) &val16) + 1);
#else
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1,
                            ((Bit8u *) &val16) + 1);
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1,
                            &val16);
#endif
      }
    }
  else
#endif
    {
    BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 2, &val16);
    }
}

  void
BX_CPU_C::write_RMW_virtual_dword(Bit32u val32)
{
  BX_INSTR_MEM_DATA(BX_CPU_THIS_PTR address_xlation.paddress1, 4, BX_WRITE);

#if BX_CPU_LEVEL >= 3
  if (BX_CPU_THIS_PTR cr0.pg) {
    if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 4, &val32);
      }
    else {
#ifdef BX_LITTLE_ENDIAN
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
                            BX_CPU_THIS_PTR address_xlation.len1,
                            &val32);
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
                            BX_CPU_THIS_PTR address_xlation.len2,
                            ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
#else
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
                            BX_CPU_THIS_PTR address_xlation.len1,
                            ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
      BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
                            BX_CPU_THIS_PTR address_xlation.len2,
                            &val32);
#endif
      }
    }
  else
#endif
    {
    BX_MEM.write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 4, &val32);
    }
}
