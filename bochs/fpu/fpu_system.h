/*---------------------------------------------------------------------------+
 |  fpu_system.h                                                             |
 |                                                                           |
 | Copyright (C) 1992,1994,1997                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@suburbia.net             |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _FPU_SYSTEM_H
#define _FPU_SYSTEM_H

#ifndef USE_WITH_CPU_SIM

/* system dependent definitions */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>

/* This sets the pointer FPU_info to point to the argument part
   of the stack frame of math_emulate() */
#define SETUP_DATA_AREA(arg)	FPU_info = (struct info *) &arg

#define LDT_DESCRIPTOR(s)	(((struct desc_struct *)current->mm->segments)[(s) >> 3])
#define SEG_D_SIZE(x)		((x).b & (3 << 21))
#define SEG_G_BIT(x)		((x).b & (1 << 23))
#define SEG_GRANULARITY(x)	(((x).b & (1 << 23)) ? 4096 : 1)
#define SEG_286_MODE(x)		((x).b & ( 0xff000000 | 0xf0000 | (1 << 23)))
#define SEG_BASE_ADDR(s)	(((s).b & 0xff000000) \
				 | (((s).b & 0xff) << 16) | ((s).a >> 16))
#define SEG_LIMIT(s)		(((s).b & 0xff0000) | ((s).a & 0xffff))
#define SEG_EXECUTE_ONLY(s)	(((s).b & ((1 << 11) | (1 << 9))) == (1 << 11))
#define SEG_WRITE_PERM(s)	(((s).b & ((1 << 11) | (1 << 9))) == (1 << 9))
#define SEG_EXPAND_DOWN(s)	(((s).b & ((1 << 11) | (1 << 10))) \
				 == (1 << 10))

#define I387			(current->tss.i387)
#define FPU_info		(I387.soft.info)

#define FPU_CS			(*(u16 *) &(FPU_info->___cs))
#define FPU_SS			(*(u16 *) &(FPU_info->___ss))
#define FPU_DS			(*(u16 *) &(FPU_info->___ds))
#define FPU_EAX			(FPU_info->___eax)
#define FPU_EFLAGS		(FPU_info->___eflags)
#define FPU_EIP			(FPU_info->___eip)
#define FPU_ORIG_EIP		(FPU_info->___orig_eip)

#define FPU_lookahead           (I387.soft.lookahead)

#define SET_AX(val16)           *(s16 *) &FPU_EAX = val16

/* nz if ip_offset and cs_selector are not to be set for the current
   instruction. */
#define no_ip_update		(*(u_char *)&(I387.soft.no_update))
#define FPU_rm			(*(u_char *)&(I387.soft.rm))

/* Number of bytes of data which can be legally accessed by the current
   instruction. This only needs to hold a number <= 108, so a byte will do. */
#define access_limit		(*(u_char *)&(I387.soft.alimit))

#define partial_status		(I387.soft.swd)
#define control_word		(I387.soft.cwd)
#define fpu_tag_word		(I387.soft.twd)
#define registers		(I387.soft.st_space)
#define top			(I387.soft.ftop)

#define instruction_address	(*(struct address *)&I387.soft.fip)
#define operand_address		(*(struct address *)&I387.soft.foo)

#define FPU_verify_area(x,y,z)	if ( verify_area(x,y,z) ) \
				math_abort(FPU_info,SIGSEGV)

#undef FPU_IGNORE_CODE_SEGV
#ifdef FPU_IGNORE_CODE_SEGV
/* verify_area() is very expensive, and causes the emulator to run
   about 20% slower if applied to the code. Anyway, errors due to bad
   code addresses should be much rarer than errors due to bad data
   addresses. */
#define	FPU_code_verify_area(z)
#else
/* A simpler test than verify_area() can probably be done for
   FPU_code_verify_area() because the only possible error is to step
   past the upper boundary of a legal code area. */
#define	FPU_code_verify_area(z) FPU_verify_area(VERIFY_READ,(void *)FPU_EIP,z)
#endif

#define FPU_get_user(x,y)       get_user((x),(y))
#define FPU_put_user(x,y)       put_user((x),(y))

#else  /* USE_WITH_CPU_SIM */

/* -----------------------------------------------------------
 * Slimmed down version used to compile against a CPU simulator
 * rather than a kernel (ported by Kevin Lawton)
 * ------------------------------------------------------------ */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/math_emu.h>
#include <linux/types.h>

/* Get data sizes from config.h generated from simulator's
 * configure script
 */
#include "config.h"
typedef Bit8u  u8;
typedef Bit8s  s8;
typedef Bit16u u16;
typedef Bit16s s16;
typedef Bit32u u32;
typedef Bit32s s32;
typedef Bit64u u64;
typedef Bit64s s64;

#ifndef WORDS_BIGENDIAN
#error "WORDS_BIGENDIAN not defined in config.h"
#elif WORDS_BIGENDIAN == 1
#define EMU_BIG_ENDIAN 1
#else
/* Nothing needed.  Lack of defining EMU_BIG_ENDIAN means
 * small endian
 */
#endif


extern unsigned fpu_get_user(void *ptr, unsigned len);
extern void fpu_put_user(unsigned val, void *ptr, unsigned len);

extern void fpu_verify_area(unsigned what, void *ptr, unsigned n);
extern void math_emulate_init(void);
extern unsigned fpu_get_ds(void);
extern void fpu_set_ax(u16);

#ifndef __ASSEMBLY__

struct info {
  };

#define FPU_info ((struct info *) NULL)


//
// Minimal i387 structure, pruned from the linux headers.  Only
// the fields which were necessary are included.
//
typedef struct {
  struct {
    s32      cwd;
    s32      swd;
    s32      twd;
    s32      fip;
    s32      fcs;
    s32      foo;
    s32      fos;
    u32      fill0; /* to make sure the following aligns on an 8byte boundary */
    u64      st_space[16];   /* 8*16 bytes per FP-reg (aligned) = 128 bytes */
    unsigned char   ftop;
    unsigned char   no_update;
    unsigned char   rm;
    unsigned char   alimit;
    } __attribute__ ((aligned(16), packed)) soft;
  } i387_t;

extern i387_t i387;


#endif

#define SIGSEGV  11

#define I387     i387


#define SET_AX(val16)           fpu_set_ax(val16);

#define no_ip_update            (*(u_char *)&(I387.soft.no_update))
#define FPU_rm                  (*(u_char *)&(I387.soft.rm))


/* Number of bytes of data which can be legally accessed by the current
   instruction. This only needs to hold a number <= 108, so a byte will do. */
#define access_limit            (*(u_char *)&(I387.soft.alimit))

#define partial_status          (I387.soft.swd)
#define control_word            (I387.soft.cwd)
#define fpu_tag_word            (I387.soft.twd)
#define registers               (I387.soft.st_space)
#define top                     (I387.soft.ftop)

#define instruction_address     (*(struct address *)&I387.soft.fip)
#define operand_address         (*(struct address *)&I387.soft.foo)

#define FPU_verify_area(x,y,z)	fpu_verify_area(x,y,z)
#define FPU_get_user(x,y)       ((x) = fpu_get_user((y), sizeof(*(y))))
#define FPU_put_user(val,ptr)   fpu_put_user((val),(ptr),sizeof(*(ptr)))

#define FPU_DS  (fpu_get_ds())

#endif  /* USE_WITH_CPU_SIM */

#define REGNO2PTR(n)		((FPU_REG*)((long)(n)))

#endif
