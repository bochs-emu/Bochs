/*---------------------------------------------------------------------------+
 |  errors.c                                                                 |
 |  $Id: errors.c,v 1.16 2004-02-06 22:27:59 danielg4 Exp $
 |                                                                           |
 |  The error handling functions for wm-FPU-emu                              |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1996                                         |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@jacobi.maths.monash.edu.au                |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 | Note:                                                                     |
 |    The file contains code which accesses user memory.                     |
 |    Emulator static data may change when user memory is accessed, due to   |
 |    other processes using the emulator while swapping is in progress.      |
 +---------------------------------------------------------------------------*/

#include <linux/signal.h>
#include <stdio.h>

#include "fpu_emu.h"
#include "fpu_system.h"
#include "exception.h"
#include "status_w.h"
#include "control_w.h"
#include "reg_constant.h"
#include "version.h"
#ifdef __cplusplus
extern "C"
#endif
int printk(const char * fmt, ...);

/*
   Called for opcodes which are illegal and which are known to result in a
   SIGILL with a real 80486.
   */
void FPU_illegal(void)
{
  math_abort(NULL, SIGILL);
}

#ifndef USE_WITH_CPU_SIM
void FPU_printall(void)
{
  int i;
  static const char *tag_desc[] = { "Valid", "Zero", "ERROR", "Empty",
                              "DeNorm", "Inf", "NaN" };

  FPU_partial_status = status_word();

  printk(" SW: b=%d st=%ld es=%d sf=%d cc=%d%d%d%d ef=%d%d%d%d%d%d\n",
	 FPU_partial_status & 0x8000 ? 1 : 0,   /* busy */
	 (FPU_partial_status & 0x3800) >> 11,   /* stack top pointer */
	 FPU_partial_status & 0x80 ? 1 : 0,     /* Error summary status */
	 FPU_partial_status & 0x40 ? 1 : 0,     /* Stack flag */
	 FPU_partial_status & SW_C3?1:0, FPU_partial_status & SW_C2?1:0, /* cc */
	 FPU_partial_status & SW_C1?1:0, FPU_partial_status & SW_C0?1:0, /* cc */
	 FPU_partial_status & SW_Precision?1:0, FPU_partial_status & SW_Underflow?1:0,
	 FPU_partial_status & SW_Overflow?1:0, FPU_partial_status & SW_Zero_Div?1:0,
	 FPU_partial_status & SW_Denorm_Op?1:0, FPU_partial_status & SW_Invalid?1:0);
  
printk(" CW: ic=%d rc=%ld%ld pc=%ld%ld iem=%d     ef=%d%d%d%d%d%d\n",
	 FPU_control_word & 0x1000 ? 1 : 0,
	 (FPU_control_word & 0x800) >> 11, (FPU_control_word & 0x400) >> 10,
	 (FPU_control_word & 0x200) >> 9, (FPU_control_word & 0x100) >> 8,
	 FPU_control_word & 0x80 ? 1 : 0,
	 FPU_control_word & SW_Precision?1:0, FPU_control_word & SW_Underflow?1:0,
	 FPU_control_word & SW_Overflow?1:0, FPU_control_word & SW_Zero_Div?1:0,
	 FPU_control_word & SW_Denorm_Op?1:0, FPU_control_word & SW_Invalid?1:0);

  for (i = 0; i < 8; i++)
    {
      FPU_REG *r = &st(i);
      u_char tagi = FPU_gettagi(i);
      switch (tagi)
	{
	case TAG_Empty:
	  continue;
	  break;
	case TAG_Zero:
	case TAG_Special:
	  tagi = FPU_Special(r);
	case TAG_Valid:
	  printk("st(%d)  %c .%04lx %04lx %04lx %04lx e%+-6d ", i,
		 getsign(r) ? '-' : '+',
		 (s32)(r->sigh >> 16),
		 (s32)(r->sigh & 0xFFFF),
		 (s32)(r->sigl >> 16),
		 (s32)(r->sigl & 0xFFFF),
		 exponent(r) - EXP_BIAS + 1);
	  break;
	default:
	  printk("Whoops! Error in errors.c: tag%d is %d ", i, tagi);
	  continue;
	  break;
	}
      printk("%s\n", tag_desc[(int) (unsigned) tagi]);
    }

  RE_ENTRANT_CHECK_ON;

}
#endif

static struct {
  int type;
  const char *name;
} exception_names[] = {
  { EX_StackOver, "stack overflow" },
  { EX_StackUnder, "stack underflow" },
  { EX_Precision, "loss of precision" },
  { EX_Underflow, "underflow" },
  { EX_Overflow, "overflow" },
  { EX_ZeroDiv, "divide by zero" },
  { EX_Denormal, "denormalized operand" },
  { EX_Invalid, "invalid operation" },
  { 0, NULL }
};

/*
 EX_INTERNAL is always given with a code which indicates where the
 error was detected.

 Internal error types:
       0x14   in fpu_etc.c
       0x1nn  in a *.c file:
              0x101  in reg_add_sub.c
              0x102  in reg_mul.c
              0x104  in poly_atan.c
              0x105  in reg_mul.c
              0x107  in fpu_trig.c
	      0x108  in reg_compare.c
	      0x109  in reg_compare.c
	      0x110  in reg_add_sub.c
	      0x111  in fpe_entry.c
	      0x112  in fpu_trig.c
	      0x113  in errors.c
	      0x115  in fpu_trig.c
	      0x116  in fpu_trig.c
	      0x117  in fpu_trig.c
	      0x118  in fpu_trig.c
	      0x119  in fpu_trig.c
	      0x120  in poly_atan.c
	      0x121  in reg_compare.c
	      0x122  in reg_compare.c
	      0x123  in reg_compare.c
	      0x125  in fpu_trig.c
	      0x126  in fpu_entry.c
	      0x127  in poly_2xm1.c
	      0x128  in fpu_entry.c
	      0x129  in fpu_entry.c
	      0x130  in get_address.c
	      0x131  in get_address.c
	      0x132  in get_address.c
	      0x133  in get_address.c
	      0x140  in load_store.c
	      0x141  in load_store.c
              0x150  in poly_sin.c
              0x151  in poly_sin.c
	      0x160  in reg_ld_str.c
	      0x161  in reg_ld_str.c
	      0x162  in reg_ld_str.c
	      0x163  in reg_ld_str.c
	      0x164  in reg_ld_str.c
	      0x170  in fpu_tags.c
	      0x171  in fpu_tags.c
	      0x172  in fpu_tags.c
	      0x180  in reg_convert.c
*/

void FPU_internal(int type)
{
   printk("FPU emulator: Internal error type 0x%04x\n", type);
}

void FPU_exception(int exception)
{
  /* Extract only the bits which we use to set the status word */
  exception &= (SW_Exc_Mask);
  /* Set the corresponding exception bit */
  FPU_partial_status |= exception;
  /* Set summary bits iff exception isn't masked */
  if (FPU_partial_status & ~FPU_control_word & CW_Exceptions)
	FPU_partial_status |= (SW_Summary | SW_Backward);
  if (exception & (SW_Stack_Fault | EX_Precision))
  {
      if (!(exception & SW_C1))
        /* This bit distinguishes over- from underflow for a stack fault,
             and roundup from round-down for precision loss. */
        FPU_partial_status &= ~SW_C1;
  }
}

/* Real operation attempted on a NaN. */
/* Returns < 0 if the exception is unmasked */
int real_1op_NaN(FPU_REG *a)
{
  int signalling, isNaN;

  isNaN = (exponent(a) == EXP_OVER) && (a->sigh & 0x80000000);

  /* The default result for the case of two "equal" NaNs (signs may
     differ) is chosen to reproduce 80486 behaviour */
  signalling = isNaN && !(a->sigh & 0x40000000);

  if (!signalling)
    {
      if (!isNaN)  /* pseudo-NaN, or other unsupported? */
	{
	  if (FPU_control_word & CW_Invalid)
	    {
	      /* Masked response */
	      reg_copy(&CONST_QNaN, a);
	    }
	  EXCEPTION(EX_Invalid);
	  return (!(FPU_control_word & CW_Invalid) ? FPU_Exception : 0) | TAG_Special;
	}
      return TAG_Special;
    }

  if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      if (!(a->sigh & 0x80000000))  /* pseudo-NaN ? */
	{
	  reg_copy(&CONST_QNaN, a);
	}
      /* ensure a Quiet NaN */
      a->sigh |= 0x40000000;
    }

  EXCEPTION(EX_Invalid);

  return (!(FPU_control_word & CW_Invalid) ? FPU_Exception : 0) | TAG_Special;
}


/* Real operation attempted on two operands, one a NaN. */
/* Returns < 0 if the exception is unmasked */
int real_2op_NaN(FPU_REG const *b, u_char tagb,
		 int deststnr,
		 FPU_REG const *defaultNaN)
{
  FPU_REG *dest = &st(deststnr);
  FPU_REG const *a = dest;
  u_char taga = FPU_gettagi(deststnr);
  FPU_REG const *x;
  int signalling, unsupported;

  if (taga == TAG_Special)
    taga = FPU_Special(a);
  if (tagb == TAG_Special)
    tagb = FPU_Special(b);

  /* TW_NaN is also used for unsupported data types. */
  unsupported = ((taga == TW_NaN)
		 && !((exponent(a) == EXP_OVER) && (a->sigh & 0x80000000)))
    || ((tagb == TW_NaN)
	&& !((exponent(b) == EXP_OVER) && (b->sigh & 0x80000000)));
  if (unsupported)
    {
      if (FPU_control_word & CW_Invalid)
	{
	  /* Masked response */
	  FPU_copy_to_regi(&CONST_QNaN, TAG_Special, deststnr);
	}
      EXCEPTION(EX_Invalid);
      return (!(FPU_control_word & CW_Invalid) ? FPU_Exception : 0) | TAG_Special;
    }

  if (taga == TW_NaN)
    {
      x = a;
      if (tagb == TW_NaN)
	{
	  signalling = !(a->sigh & b->sigh & 0x40000000);
	  if (significand(b) > significand(a))
	    x = b;
	  else if (significand(b) == significand(a))
	    {
	      /* The default result for the case of two "equal" NaNs (signs may
		 differ) is chosen to reproduce 80486 behaviour */
	      x = defaultNaN;
	    }
	}
      else
	{
	  /* return the quiet version of the NaN in a */
	  signalling = !(a->sigh & 0x40000000);
	}
    }
  else
#ifdef PARANOID
    if (tagb == TW_NaN)
#endif /* PARANOID */
    {
      signalling = !(b->sigh & 0x40000000);
      x = b;
    }
#ifdef PARANOID
  else
    {
      signalling = 0;
      INTERNAL(0x113);
      x = &CONST_QNaN;
    }
#endif /* PARANOID */

  if ((!signalling) || (FPU_control_word & CW_Invalid))
    {
      if (! x)
	x = b;

      if (!(x->sigh & 0x80000000))  /* pseudo-NaN ? */
	x = &CONST_QNaN;

      FPU_copy_to_regi(x, TAG_Special, deststnr);

      if (!signalling)
	return TAG_Special;

      /* ensure a Quiet NaN */
      dest->sigh |= 0x40000000;
    }

  EXCEPTION(EX_Invalid);

  return (!(FPU_control_word & CW_Invalid) ? FPU_Exception : 0) | TAG_Special;
}


/* Invalid arith operation on Valid registers */
/* Returns < 0 if the exception is unmasked */
asmlinkage int arith_invalid(int deststnr)
{
  EXCEPTION(EX_Invalid);
  
  if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      FPU_copy_to_regi(&CONST_QNaN, TAG_Special, deststnr);
    }
  
  return (!(FPU_control_word & CW_Invalid) ? FPU_Exception : 0) | TAG_Valid;
}


/* Divide a finite number by zero */
asmlinkage int FPU_divide_by_zero(int deststnr, u_char sign)
{
  FPU_REG *dest = &st(deststnr);
  int tag = TAG_Valid;

  if (FPU_control_word & CW_ZeroDiv)
    {
      /* The masked response */
      FPU_copy_to_regi(&CONST_INF, TAG_Special, deststnr);
      setsign(dest, sign);
      tag = TAG_Special;
    }
 
  EXCEPTION(EX_ZeroDiv);

  return (!(FPU_control_word & CW_ZeroDiv) ? FPU_Exception : 0) | tag;
}


/* This may be called often, so keep it lean */
int set_precision_flag(int flags)
{
  if (FPU_control_word & CW_Precision)
    {
      FPU_partial_status &= ~(SW_C1 & flags);
      FPU_partial_status |= flags;   /* The masked response */
      return 0;
    }
  else
    {
      EXCEPTION(flags);
      return 1;
    }
}


/* This may be called often, so keep it lean */
asmlinkage void set_precision_flag_up(void)
{
  if (FPU_control_word & CW_Precision)
    FPU_partial_status |= (SW_Precision | SW_C1);   /* The masked response */
  else
    EXCEPTION(EX_Precision | SW_C1);
}


/* This may be called often, so keep it lean */
asmlinkage void set_precision_flag_down(void)
{
  if (FPU_control_word & CW_Precision)
    {   /* The masked response */
      FPU_partial_status &= ~SW_C1;
      FPU_partial_status |= SW_Precision;
    }
  else
    EXCEPTION(EX_Precision);
}


asmlinkage int denormal_operand(void)
{
  if (FPU_control_word & CW_Denormal)
    {   /* The masked response */
      FPU_partial_status |= SW_Denorm_Op;
      return TAG_Special;
    }
  else
    {
      EXCEPTION(EX_Denormal);
      return TAG_Special | FPU_Exception;
    }
}


asmlinkage int arith_overflow(FPU_REG *dest)
{
  int tag = TAG_Valid;

  if (FPU_control_word & CW_Overflow)
    {
      /* The masked response */
      reg_copy(&CONST_INF, dest);
      tag = TAG_Special;
    }
  else
    {
      /* Subtract the magic number from the exponent */
      addexponent(dest, (-3 * (1 << 13)));
    }

  EXCEPTION(EX_Overflow);
  if (FPU_control_word & CW_Overflow)
    {
      /* The overflow exception is masked. */
      /* By definition, precision is lost.
	 The roundup bit (C1) is also set because we have
	 "rounded" upwards to Infinity. */
      EXCEPTION(EX_Precision | SW_C1);
      return tag;
    }

  return tag;
}


asmlinkage int arith_round_overflow(FPU_REG *dest, u8 sign)
{
  int tag = TAG_Valid;
  int largest;

  if (FPU_control_word & CW_Overflow)
    {
      /* The masked response */
      /* The response here depends upon the rounding mode */
      switch (FPU_control_word & CW_RC)
	{
	case RC_CHOP:		/* Truncate */
	  largest = 1;
	  break;
	case RC_UP:		/* Towards +infinity */
	  largest = (sign == SIGN_NEG);
	  break;
	case RC_DOWN:		/* Towards -infinity */
	  largest = (sign == SIGN_POS);
	  break;
	default:
	  largest = 0;
	  break;
	}
      if (! largest)
	{
	  reg_copy(&CONST_INF, dest);
	  tag = TAG_Special;
	}
      else
	{
	  dest->exp = EXTENDED_Ebias+EXP_OVER-1;
	  switch (FPU_control_word & CW_PC)
	    {
	    case 01:
	    case PR_64_BITS:
	      significand(dest) = BX_CONST64(0xffffffffffffffff);
	      break;
	    case PR_53_BITS:
	      significand(dest) = BX_CONST64(0xfffffffffffff800);
	      break;
	    case PR_24_BITS:
	      significand(dest) = BX_CONST64(0xffffff0000000000);
	      break;
	    }
	}
    }
  else
    {
      /* Subtract the magic number from the exponent */
      addexponent(dest, (-3 * (1 << 13)));
      largest = 0;
    }

  EXCEPTION(EX_Overflow);
  if (FPU_control_word & CW_Overflow)
    {
      /* The overflow exception is masked. */
      if (largest)
	{
	  EXCEPTION(EX_Precision);
	}
      else
	{
	  /* By definition, precision is lost.
	     The roundup bit (C1) is also set because we have
	     "rounded" upwards to Infinity. */
	  EXCEPTION(EX_Precision | SW_C1);
	}
      return tag;
    }

  return tag;
}


asmlinkage int arith_underflow(FPU_REG *dest)
{
  int tag = TAG_Valid;

  if (FPU_control_word & CW_Underflow)
    {
      /* The masked response */
      if (exponent16(dest) <= EXP_UNDER - 63)
	{
	  reg_copy(&CONST_Z, dest);
	  FPU_partial_status &= ~SW_C1;       /* Round down. */
	  tag = TAG_Zero;
	}
      else
	{
	  stdexp(dest);
	}
    }
  else
    {
      /* Add the magic number to the exponent. */
      addexponent(dest, (3 * (1 << 13)) + EXTENDED_Ebias);
    }

  EXCEPTION(EX_Underflow);
  if (FPU_control_word & CW_Underflow)
    {
      /* The underflow exception is masked. */
      EXCEPTION(EX_Precision);
      return tag;
    }

  return tag;
}


void FPU_stack_overflow(void)
{
 if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      FPU_tos--;
      FPU_copy_to_reg0(&CONST_QNaN, TAG_Special);
    }

  EXCEPTION(EX_StackOver);
}


void FPU_stack_underflow(void)
{
 if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      FPU_copy_to_reg0(&CONST_QNaN, TAG_Special);
    }

  EXCEPTION(EX_StackUnder);
}


void FPU_stack_underflow_i(int i)
{
 if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      FPU_copy_to_regi(&CONST_QNaN, TAG_Special, i);
    }

  EXCEPTION(EX_StackUnder);
}


void FPU_stack_underflow_pop(int i)
{
 if (FPU_control_word & CW_Invalid)
    {
      /* The masked response */
      FPU_copy_to_regi(&CONST_QNaN, TAG_Special, i);
      FPU_pop();
    }

  EXCEPTION(EX_StackUnder);
}
