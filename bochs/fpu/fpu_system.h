/*---------------------------------------------------------------------------+
 |  fpu_system.h                                                             |
 |  $Id: fpu_system.h,v 1.20 2003-11-01 18:36:19 sshwarts Exp $
 |                                                                           |
 | Copyright (C) 1992,1994,1997                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@suburbia.net             |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _FPU_SYSTEM_H
#define _FPU_SYSTEM_H

#include <stdio.h>

/* Get data sizes from config.h generated from simulator's
 * configure script
 */
#include "config.h"
typedef Bit8u  u8; /* for FPU only */
typedef Bit8s  s8;
typedef Bit16u u16;
typedef Bit16s s16;
typedef Bit32u u32;
typedef Bit32s s32;
typedef Bit64u u64;
typedef Bit64s s64;

typedef Bit8u u_char;

/* -----------------------------------------------------------
 * Slimmed down version used to compile against a CPU simulator
 * rather than a kernel (ported by Kevin Lawton)
 * ------------------------------------------------------------ */

#include <cpu/i387.h>

#define VERIFY_READ     0
#define VERIFY_WRITE    1

#ifndef WORDS_BIGENDIAN
#error "WORDS_BIGENDIAN not defined in config.h"
#elif WORDS_BIGENDIAN == 1
#define EMU_BIG_ENDIAN 1
#else
/* Nothing needed.  Lack of defining EMU_BIG_ENDIAN means
 * little endian.
 */
#endif

extern u32 fpu_get_user(bx_address ptr, unsigned len) BX_CPP_AttrRegparmN(2);
extern void fpu_put_user(u32 val, bx_address ptr, unsigned len) BX_CPP_AttrRegparmN(3);
extern void fpu_verify_area(unsigned what, bx_address ptr, unsigned n) BX_CPP_AttrRegparmN(3);
extern unsigned fpu_get_ds(void);
extern void fpu_set_ax(u16 ax);
extern u32 fpu_get_eflags();
extern void fpu_set_eflags(u32 eflags);

#define SIGSEGV  11

extern struct i387_t *current_i387;

#define i387     (*current_i387)

#define no_ip_update            (*(u_char *)&(i387.no_update))
#define FPU_rm                  (*(u_char *)&(i387.rm))

#define FPU_partial_status      (i387.swd)
#define FPU_control_word        (i387.cwd)
#define FPU_tag_word            (i387.twd)
#define FPU_registers           (i387.st_space)
#define FPU_tos                 (i387.tos)

#define FPU_register_base       ((u_char *) FPU_registers)

#define FPU_instruction_address (*(struct address *)&i387.fip)
#define FPU_operand_address     (*(struct address *)&i387.foo)

#define FPU_verify_area(x,y,z)	fpu_verify_area(x,(bx_address)(y),z)
#define FPU_get_user(val,ptr,len)       ((val) = fpu_get_user((ptr), (len)))
#define FPU_put_user(val,ptr,len)   fpu_put_user((val),(ptr),(len))

#define FPU_DS  (fpu_get_ds())

/*
 * Change a pointer to an int, with type conversions that make it legal.
 * First make it a void pointer, then convert to an integer of the same
 * size as the pointer.  Otherwise, on machines with 64-bit pointers, 
 * compilers complain when you typecast a 64-bit pointer into a 32-bit integer.
 */
#define PTR2INT(x)   ((bx_ptr_equiv_t)(void *)(x))

/*
 * Change an int to a pointer, with type conversions that make it legal.
 * Same strategy as PTR2INT: change to bx_ptr_equiv_t which is an integer
 * type of the same size as FPU_REG*.  Then the conversion to pointer
 * is legal.
 */
#define REGNO2PTR(x) ((FPU_REG*)((bx_ptr_equiv_t)(x)))

#endif
