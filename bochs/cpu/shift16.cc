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






#define BX_IN_CPU_METHOD 1
#include "bochs.h"



  void
BX_CPU_C::SHLD_EwGw(BxInstruction_t *i)
{
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;

  /* op1:op2 << count.  result stored in op1 */
  if (i->b1 == 0x1a4)
    count = i->Ib;
  else // 0x1a5
    count = CL;

  count &= 0x1f; // use only 5 LSB's


    if (!count) return; /* NOP */
    // count is 1..31

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    temp_32 = (op1_16 << 16) | (op2_16); // double formed by op1:op2
    result_32 = temp_32 << count;
    if (count > 16) {
      // hack to act like x86 SHLD when count > 16
      // actually shifting op1:op2:op2 << count
      result_32 |= (op2_16 << (count - 16));
      }
    result_16 = result_32 >> 16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    /* set eflags:
     * SHLD count affects the following flags: S,Z,P,C,O
     */
    set_CF( (temp_32 >> (32 - count)) & 0x01 );
    if (count == 1)
      set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    set_ZF(result_16 == 0);
    set_SF(result_16 >> 15);
    set_PF_base((Bit8u) result_16);
}


  void
BX_CPU_C::SHRD_EwGw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("shrd_evgvib: not supported on < 386\n");
#else
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;

  if (i->b1 == 0x1ac)
    count = i->Ib;
  else // 0x1ad
    count = CL;
  count &= 0x1F; /* use only 5 LSB's */

  if (!count) return; /* NOP */

    // count is 1..31

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
    result_32 = temp_32 >> count;
    if (count > 16) {
      // hack to act like x86 SHLD when count > 16
      // actually shifting op2:op2:op1 >> count
      result_32 |= (op2_16 << (32 - count));
      }
    result_16 = result_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    /* set eflags:
     * SHRD count affects the following flags: S,Z,P,C,O
     */

    set_CF((temp_32 >> (count - 1)) & 0x01);
    set_ZF(result_16 == 0);
    set_SF(result_16 >> 15);
    /* for shift of 1, OF set if sign change occurred. */
    if (count == 1)
      set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    set_PF_base((Bit8u) result_16);
#endif /* BX_CPU_LEVEL >= 3 */
}



  void
BX_CPU_C::ROL_Ew(BxInstruction_t *i)
{

    Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

    count &= 0x0f; // only use bottom 4 bits

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    if (count) {
      result_16 = (op1_16 << count) | (op1_16 >> (16 - count));

      /* now write result back to destination */
      if (i->mod == 0xc0) {
        BX_WRITE_16BIT_REG(i->rm, result_16);
        }
      else {
        write_RMW_virtual_word(result_16);
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      set_CF(result_16 & 0x01);
      if (count == 1)
        set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
}




  void
BX_CPU_C::ROR_Ew(BxInstruction_t *i)
{
    Bit16u op1_16, result_16, result_b15;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

    count &= 0x0f;  // use only 4 LSB's

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    if (count) {
      result_16 = (op1_16 >> count) | (op1_16 << (16 - count));

      /* now write result back to destination */
      if (i->mod == 0xc0) {
        BX_WRITE_16BIT_REG(i->rm, result_16);
        }
      else {
        write_RMW_virtual_word(result_16);
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b15 = result_16 & 0x8000;

      set_CF(result_b15 != 0);
      if (count == 1)
        set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
}



  void
BX_CPU_C::RCL_Ew(BxInstruction_t *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1F;

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    count %= 17;

    if (!count) return;

    if (count==1) {
      result_16 = (op1_16 << 1) | get_CF();
      }
    else if (count==16) {
      result_16 = (get_CF() << 15) |
                  (op1_16 >> 1);
      }
    else { // 2..15
      result_16 = (op1_16 << count) |
                  (get_CF() << (count - 1)) |
                  (op1_16 >> (17 - count));
      }

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    /* set eflags:
     * RCL count affects the following flags: C
     */

    if (count == 1)
      set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    set_CF((op1_16 >> (16 - count)) & 0x01);
}



  void
BX_CPU_C::RCR_Ew(BxInstruction_t *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  count = count & 0x1F;

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    count %= 17;
    if (count) {
      result_16 = (op1_16 >> count) |
		(get_CF() << (16 - count)) |
		(op1_16 << (17 - count));

      /* now write result back to destination */
      if (i->mod == 0xc0) {
	BX_WRITE_16BIT_REG(i->rm, result_16);
	}
      else {
	write_RMW_virtual_word(result_16);
        }

      /* set eflags:
       * RCR count affects the following flags: C
       */

      set_CF((op1_16 >> (count - 1)) & 0x01);
      if (count == 1)
        set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
}




  void
BX_CPU_C::SHL_Ew(BxInstruction_t *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1F; /* use only 5 LSB's */

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    if (!count) return;

    result_16 = (op1_16 << count);

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SHL16);
}




  void
BX_CPU_C::SHR_Ew(BxInstruction_t *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1F; /* use only 5 LSB's */

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    if (!count) return;

    result_16 = (op1_16 >> count);


    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SHR16);
}



  void
BX_CPU_C::SAR_Ew(BxInstruction_t *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1 == 0xc1 )
    count = i->Ib;
  else if ( i->b1 == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1F;  /* use only 5 LSB's */

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    if (!count) return;

    if (count < 16) {
      if (op1_16 & 0x8000) {
	result_16 = (op1_16 >> count) | (0xffff << (16 - count));
	}
      else {
	result_16 = (op1_16 >> count);
	}
      }
    else {
      if (op1_16 & 0x8000) {
	result_16 = 0xffff;
	}
      else {
	result_16 = 0;
	}
      }



    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */
    if (count < 16) {
      set_CF((op1_16 >> (count - 1)) & 0x01);
      }
    else {
      if (op1_16 & 0x8000) {
	set_CF(1);
	}
      else {
	set_CF(0);
	}
      }

    set_ZF(result_16 == 0);
    set_SF(result_16 >> 15);
    if (count == 1)
      set_OF(0);
    set_PF_base((Bit8u) result_16);
}
