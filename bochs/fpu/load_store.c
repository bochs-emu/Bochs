/*---------------------------------------------------------------------------+
 |  load_store.c                                                             |
 |  $Id: load_store.c,v 1.12 2003-10-04 12:32:56 sshwarts Exp $
 |                                                                           |
 | This file contains most of the code to interpret the FPU instructions     |
 | which load and store from user memory.                                    |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1997                                         |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@suburbia.net             |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 | Note:                                                                     |
 |    The file contains code which accesses user memory.                     |
 |    Emulator static data may change when user memory is accessed, due to   |
 |    other processes using the emulator while swapping is in progress.      |
 +---------------------------------------------------------------------------*/

#include "fpu_system.h"
#include "exception.h"
#include "fpu_emu.h"
#include "status_w.h"
#include "control_w.h"


#define _NONE_ 0   /* st0_ptr etc not needed */
#define _REG0_ 1   /* Will be storing st(0) */
#define _PUSH_ 3   /* Need to check for space to push onto stack */
#define _null_ 4   /* Function illegal or not implemented */

#define pop_0()	{ FPU_settag0(TAG_Empty); FPU_tos++; }


static u_char const type_table[32] = {
  _PUSH_, _PUSH_, _PUSH_, _PUSH_,
  _null_, _null_, _null_, _null_,
  _REG0_, _REG0_, _REG0_, _REG0_,
  _REG0_, _REG0_, _REG0_, _REG0_,
  _NONE_, _null_, _NONE_, _PUSH_,
  _NONE_, _PUSH_, _null_, _PUSH_,
  _NONE_, _null_, _NONE_, _REG0_,
  _NONE_, _REG0_, _NONE_, _REG0_
  };

int FPU_load_store(u_char type, fpu_addr_modes addr_modes,
		     bx_address data_address)
{
  FPU_REG loaded_data;
  FPU_REG *st0_ptr;
  u_char st0_tag = TAG_Empty;  /* This is just to stop a gcc warning. */
  u_char loaded_tag;

  st0_ptr = (FPU_REG*) NULL;   /* Initialized just to stop compiler warnings. */

  switch (type_table[type])
    {
    case _NONE_:
      break;
    case _REG0_:
      st0_ptr = &st(0);       /* Some of these instructions pop after
				 storing */
      st0_tag = FPU_gettag0();
      break;
    case _PUSH_:
      {
	if ( FPU_gettagi(-1) != TAG_Empty )
	  { FPU_stack_overflow(); return 0; }
	FPU_tos--;
	st0_ptr = &st(0);
      }
      break;
    case _null_:
      FPU_illegal();
      return 0;
#ifdef PARANOID
    default:
      INTERNAL(0x141);
      return 0;
#endif /* PARANOID */
    }

  switch (type)
    {
    case 000:       /* fld m32real */
      clear_C1();
      loaded_tag = FPU_load_single(data_address, &loaded_data);
      if ( (loaded_tag == TAG_Special)
	   && isNaN(&loaded_data)
	   && (real_1op_NaN(&loaded_data) < 0) )
	{
	  FPU_tos++;
	  break;
	}
      FPU_copy_to_reg0(&loaded_data, loaded_tag);
      break;
    case 001:      /* fild m32int */
      clear_C1();
      loaded_tag = FPU_load_int32(data_address, &loaded_data);
      FPU_copy_to_reg0(&loaded_data, loaded_tag);
      break;
    case 002:      /* fld m64real */
      clear_C1();
      loaded_tag = FPU_load_double(data_address, &loaded_data);
      if ((loaded_tag == TAG_Special)
	   && isNaN(&loaded_data)
	   && (real_1op_NaN(&loaded_data) < 0))
	{
	  FPU_tos++;
	  break;
	}
      FPU_copy_to_reg0(&loaded_data, loaded_tag);
      break;
    case 003:      /* fild m16int */
      clear_C1();
      loaded_tag = FPU_load_int16(data_address, &loaded_data);
      FPU_copy_to_reg0(&loaded_data, loaded_tag);
      break;
    case 010:      /* fst m32real */
      clear_C1();
      FPU_store_single(st0_ptr, st0_tag, data_address);
      break;
    case 011:      /* fist m32int */
      clear_C1();
      FPU_store_int32(st0_ptr, st0_tag, data_address);
      break;
    case 012:     /* fst m64real */
      clear_C1();
      FPU_store_double(st0_ptr, st0_tag, data_address);
      break;
    case 013:     /* fist m16int */
      clear_C1();
      FPU_store_int16(st0_ptr, st0_tag, data_address);
      break;
    case 014:     /* fstp m32real */
      clear_C1();
      if ( FPU_store_single(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 015:     /* fistp m32int */
      clear_C1();
      if ( FPU_store_int32(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 016:     /* fstp m64real */
      clear_C1();
      if ( FPU_store_double(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 017:     /* fistp m16int */
      clear_C1();
      if ( FPU_store_int16(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 020:     /* fldenv  m14/28byte */
      fldenv(addr_modes, data_address);
      /* Ensure that the values just loaded are not changed by
	 fix-up operations. */
      return 1;
    case 022:     /* frstor m94/108byte */
      frstor(addr_modes, data_address);
      /* Ensure that the values just loaded are not changed by
	 fix-up operations. */
      return 1;
    case 023:     /* fbld m80dec */
      clear_C1();
      loaded_tag = FPU_load_bcd(data_address);
      FPU_settag0(loaded_tag);
      break;
    case 024:     /* fldcw */
      RE_ENTRANT_CHECK_OFF;
      FPU_verify_area(VERIFY_READ, data_address, 2);
      FPU_get_user(FPU_control_word, data_address, 2);
      RE_ENTRANT_CHECK_ON;
      if ( FPU_partial_status & ~FPU_control_word & CW_Exceptions )
	FPU_partial_status |= (SW_Summary | SW_Backward);
      else
	FPU_partial_status &= ~(SW_Summary | SW_Backward);
#ifdef PECULIAR_486
      FPU_control_word |= 0x40;  /* An 80486 appears to always set this bit */
#endif /* PECULIAR_486 */
      return 1;
    case 025:      /* fld m80real */
      clear_C1();
      loaded_tag = FPU_load_extended(data_address, 0);
      FPU_settag0(loaded_tag);
      break;
    case 027:      /* fild m64int */
      clear_C1();
      loaded_tag = FPU_load_int64(data_address);
      FPU_settag0(loaded_tag);
      break;
    case 030:     /* fstenv  m14/28byte */
      fstenv(addr_modes, data_address);
      return 1;
    case 032:      /* fsave */
      fsave(addr_modes, data_address);
      return 1;
    case 033:      /* fbstp m80dec */
      clear_C1();
      if ( FPU_store_bcd(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 034:      /* fstcw m16int */
      RE_ENTRANT_CHECK_OFF;
      FPU_verify_area(VERIFY_WRITE,data_address,2);
      FPU_put_user(FPU_control_word, data_address, 2);
      RE_ENTRANT_CHECK_ON;
      return 1;
    case 035:      /* fstp m80real */
      clear_C1();
      if ( FPU_store_extended(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    case 036:      /* fstsw m2byte */
      RE_ENTRANT_CHECK_OFF;
      FPU_verify_area(VERIFY_WRITE,data_address,2);
      FPU_put_user(status_word(),data_address, 2);
      RE_ENTRANT_CHECK_ON;
      return 1;
    case 037:      /* fistp m64int */
      clear_C1();
      if ( FPU_store_int64(st0_ptr, st0_tag, data_address) )
	pop_0();  /* pop only if the number was actually stored
		     (see the 80486 manual p16-28) */
      break;
    }
  return 0;
}
