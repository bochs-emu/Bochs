/*---------------------------------------------------------------------------+
 |  fpu_entry.c                                                              |
 |  $Id: fpu_entry.c,v 1.8 2003-04-22 20:21:33 sshwarts Exp $
 |                                                                           |
 | The entry functions for wm-FPU-emu                                        |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1996,1997                                    |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@suburbia.net                              |
 |                                                                           |
 | See the files "README" and "COPYING" for further copyright and warranty   |
 | information.                                                              |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 | Note:                                                                     |
 |    The file contains code which accesses user memory.                     |
 |    Emulator static data may change when user memory is accessed, due to   |
 |    other processes using the emulator while swapping is in progress.      |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 | math_emulate(), restore_i387_soft() and save_i387_soft() are the only     |
 | entry points for wm-FPU-emu.                                              |
 +---------------------------------------------------------------------------*/

#include "fpu_system.h"
#include "fpu_emu.h"
#include "exception.h"
#include "control_w.h"
#include "status_w.h"

#include <linux/signal.h>
#include <asm/desc.h>


#define __BAD__ FPU_illegal   /* Illegal on an 80486, causes SIGILL */

#ifndef NO_UNDOC_CODE    /* Un-documented FPU op-codes supported by default. */

/* WARNING: These codes are not documented by Intel in their 80486 manual
   and may not work on FPU clones or later Intel FPUs. */

/* Changes to support the un-doc codes provided by Linus Torvalds. */

#define _d9_d8_ fstp_i    /* unofficial code (19) */
#define _dc_d0_ fcom_st   /* unofficial code (14) */
#define _dc_d8_ fcompst   /* unofficial code (1c) */
#define _dd_c8_ fxch_i    /* unofficial code (0d) */
#define _de_d0_ fcompst   /* unofficial code (16) */
#define _df_c0_ ffreep    /* unofficial code (07) ffree + pop */
#define _df_c8_ fxch_i    /* unofficial code (0f) */
#define _df_d0_ fstp_i    /* unofficial code (17) */
#define _df_d8_ fstp_i    /* unofficial code (1f) */

static FUNC const st_instr_table[64] = {
  fadd__,   fld_i_,     __BAD__, __BAD__, fadd_i,  ffree_,  faddp_,  _df_c0_,
  fmul__,   fxch_i,     __BAD__, __BAD__, fmul_i,  _dd_c8_, fmulp_,  _df_c8_,
  fcom_st,  fp_nop,     __BAD__, __BAD__, _dc_d0_, fst_i_,  _de_d0_, _df_d0_,
  fcompst,  _d9_d8_,    __BAD__, __BAD__, _dc_d8_, fstp_i,  fcompp,  _df_d8_,
  fsub__,   FPU_etc,    __BAD__, finit_,  fsubri,  fucom_,  fsubrp,  fstsw_,
  fsubr_,   fconst,     fucompp, __BAD__, fsub_i,  fucomp,  fsubp_,  __BAD__,
  fdiv__,   FPU_triga,  __BAD__, __BAD__, fdivri,  __BAD__, fdivrp,  __BAD__,
  fdivr_,   FPU_trigb,  __BAD__, __BAD__, fdiv_i,  __BAD__, fdivp_,  __BAD__,
};

#else     /* Support only documented FPU op-codes */

static FUNC const st_instr_table[64] = {
  fadd__,   fld_i_,     __BAD__, __BAD__, fadd_i,  ffree_,  faddp_,  __BAD__,
  fmul__,   fxch_i,     __BAD__, __BAD__, fmul_i,  __BAD__, fmulp_,  __BAD__,
  fcom_st,  fp_nop,     __BAD__, __BAD__, __BAD__, fst_i_,  __BAD__, __BAD__,
  fcompst,  __BAD__,    __BAD__, __BAD__, __BAD__, fstp_i,  fcompp,  __BAD__,
  fsub__,   FPU_etc,    __BAD__, finit_,  fsubri,  fucom_,  fsubrp,  fstsw_,
  fsubr_,   fconst,     fucompp, __BAD__, fsub_i,  fucomp,  fsubp_,  __BAD__,
  fdiv__,   FPU_triga,  __BAD__, __BAD__, fdivri,  __BAD__, fdivrp,  __BAD__,
  fdivr_,   FPU_trigb,  __BAD__, __BAD__, fdiv_i,  __BAD__, fdivp_,  __BAD__,
};

#endif /* NO_UNDOC_CODE */


#define _NONE_ 0   /* Take no special action */
#define _REG0_ 1   /* Need to check for not empty st(0) */
#define _REGI_ 2   /* Need to check for not empty st(0) and st(rm) */
#define _REGi_ 0   /* Uses st(rm) */
#define _PUSH_ 3   /* Need to check for space to push onto stack */
#define _null_ 4   /* Function illegal or not implemented */
#define _REGIi 5   /* Uses st(0) and st(rm), result to st(rm) */
#define _REGIp 6   /* Uses st(0) and st(rm), result to st(rm) then pop */
#define _REGIc 0   /* Compare st(0) and st(rm) */
#define _REGIn 0   /* Uses st(0) and st(rm), but handle checks later */

#ifndef NO_UNDOC_CODE

/* Un-documented FPU op-codes supported by default. (see above) */

static u_char const type_table[64] = {
  _REGI_, _NONE_, _null_, _null_, _REGIi, _REGi_, _REGIp, _REGi_,
  _REGI_, _REGIn, _null_, _null_, _REGIi, _REGI_, _REGIp, _REGI_,
  _REGIc, _NONE_, _null_, _null_, _REGIc, _REG0_, _REGIc, _REG0_,
  _REGIc, _REG0_, _null_, _null_, _REGIc, _REG0_, _REGIc, _REG0_,
  _REGI_, _NONE_, _null_, _NONE_, _REGIi, _REGIc, _REGIp, _NONE_,
  _REGI_, _NONE_, _REGIc, _null_, _REGIi, _REGIc, _REGIp, _null_,
  _REGI_, _NONE_, _null_, _null_, _REGIi, _null_, _REGIp, _null_,
  _REGI_, _NONE_, _null_, _null_, _REGIi, _null_, _REGIp, _null_
};

#else     /* Support only documented FPU op-codes */

static u_char const type_table[64] = {
  _REGI_, _NONE_, _null_, _null_, _REGIi, _REGi_, _REGIp, _null_,
  _REGI_, _REGIn, _null_, _null_, _REGIi, _null_, _REGIp, _null_,
  _REGIc, _NONE_, _null_, _null_, _null_, _REG0_, _null_, _null_,
  _REGIc, _null_, _null_, _null_, _null_, _REG0_, _REGIc, _null_,
  _REGI_, _NONE_, _null_, _NONE_, _REGIi, _REGIc, _REGIp, _NONE_,
  _REGI_, _NONE_, _REGIc, _null_, _REGIi, _REGIc, _REGIp, _null_,
  _REGI_, _NONE_, _null_, _null_, _REGIi, _null_, _REGIp, _null_,
  _REGI_, _NONE_, _null_, _null_, _REGIi, _null_, _REGIp, _null_
};

#endif /* NO_UNDOC_CODE */


/* Note, this is a version of fpu_entry.c, modified to interface
 * to a CPU simulator, rather than a kernel.
 *
 * Ported by Kevin Lawton Sep 20, 1999
 */


  asmlinkage void
math_emulate(fpu_addr_modes addr_modes,
              u_char  FPU_modrm,
              u_char byte1,
              void *data_address,
              struct address data_sel_off,
              struct address entry_sel_off)
{
  u16 code;
  int unmasked;
  FPU_REG loaded_data;
  FPU_REG *st0_ptr;
  u_char    loaded_tag, st0_tag;


  // assuming byte is 0xd8..0xdf or 0xdb==FWAIT

  // lock is not a valid prefix for FPU instructions, +++
  // let the cpu handle it to generate a SIGILL.


  no_ip_update = 0;

  if ( byte1 == FWAIT_OPCODE ) {
    if (partial_status & SW_Summary)
      goto do_the_FPU_interrupt;
    else
      goto FPU_fwait_done;
    }

  if (partial_status & SW_Summary) {
    /* Ignore the error for now if the current instruction is a no-wait
       control instruction */
    /* The 80486 manual contradicts itself on this topic,
       but a real 80486 uses the following instructions:
       fninit, fnstenv, fnsave, fnstsw, fnstenv, fnclex.
       */
    code = (FPU_modrm << 8) | byte1;
    if ( ! ( (((code & 0xf803) == 0xe003) ||    /* fnclex, fninit, fnstsw */
              (((code & 0x3003) == 0x3001) &&   /* fnsave, fnstcw, fnstenv,
                                                   fnstsw */
               ((code & 0xc000) != 0xc000))) ) ) {
      /*
       *  We need to simulate the action of the kernel to FPU
       *  interrupts here.
       */
do_the_FPU_interrupt:

      math_abort(FPU_info, SIGFPE);
      }
    }

  entry_sel_off.opcode = (byte1 << 8) | FPU_modrm;

  FPU_rm = FPU_modrm & 7;

  if ( FPU_modrm < 0300 ) {
      /* All of these instructions use the mod/rm byte to get a data address */

      if ( !(byte1 & 1) ) {
          u16 status1 = partial_status;

          st0_ptr = &st(0);
          st0_tag = FPU_gettag0();

          /* Stack underflow has priority */
          if ( NOT_EMPTY_ST0 ) {

#ifndef USE_WITH_CPU_SIM
              /* memory access limits checked in FPU_verify_area */
              if ( addr_modes.default_mode & PROTECTED )
                {
                  /* This table works for 16 and 32 bit protected mode */
                  if ( access_limit < data_sizes_16[(byte1 >> 1) & 3] )
                    math_abort(FPU_info, SIGSEGV);
                }
#endif
              unmasked = 0;  /* Do this here to stop compiler warnings. */
              switch ( (byte1 >> 1) & 3 )
                {
                case 0:
                  unmasked = FPU_load_single((float *)data_address,
                                             &loaded_data);
                  loaded_tag = unmasked & 0xff;
                  unmasked &= ~0xff;
                  break;
                case 1:
                  loaded_tag = FPU_load_int32((s32 *)data_address, &loaded_data);  // bbd: was (u32 *)
                  break;
                case 2:
                  unmasked = FPU_load_double((double *)data_address,
                                             &loaded_data);
                  loaded_tag = unmasked & 0xff;
                  unmasked &= ~0xff;
                  break;
                case 3:
                default:  /* Used here to suppress gcc warnings. */
                  loaded_tag = FPU_load_int16((s16 *)data_address, &loaded_data);
                  break;
                }

              /* No more access to user memory, it is safe
                 to use static data now */

              /* NaN operands have the next priority. */
              /* We have to delay looking at st(0) until after
                 loading the data, because that data might contain an SNaN */
              if ( ((st0_tag == TAG_Special) && isNaN(st0_ptr)) ||
                  ((loaded_tag == TAG_Special) && isNaN(&loaded_data)) )
                {
                  /* Restore the status word; we might have loaded a
                     denormal. */
                  partial_status = status1;
                  if ( (FPU_modrm & 0x30) == 0x10 )
                    {
                      /* fcom or fcomp */
                      EXCEPTION(EX_Invalid);
                      setcc(SW_C3 | SW_C2 | SW_C0);
                      if ( (FPU_modrm & 0x08) && (control_word & CW_Invalid) )
                        FPU_pop();             /* fcomp, masked, so we pop. */
                    }
                  else
                    {
                      if ( loaded_tag == TAG_Special )
                        loaded_tag = FPU_Special(&loaded_data);
#ifdef PECULIAR_486
                      /* This is not really needed, but gives behaviour
                         identical to an 80486 */
                      if ( (FPU_modrm & 0x28) == 0x20 )
                        /* fdiv or fsub */
                        real_2op_NaN(&loaded_data, loaded_tag, 0, &loaded_data);
                      else
#endif /* PECULIAR_486 */
                        /* fadd, fdivr, fmul, or fsubr */
                        real_2op_NaN(&loaded_data, loaded_tag, 0, st0_ptr);
                    }
                  goto reg_mem_instr_done;
                }

              if ( unmasked && !((FPU_modrm & 0x30) == 0x10) )
                {
                  /* Is not a comparison instruction. */
                  if ( (FPU_modrm & 0x38) == 0x38 )
                    {
                      /* fdivr */
                      if ( (st0_tag == TAG_Zero) &&
                           ((loaded_tag == TAG_Valid)
                            || (loaded_tag == TAG_Special
                                && isdenormal(&loaded_data))) )
                        {
                          if ( FPU_divide_by_zero(0, getsign(&loaded_data))
                               < 0 )
                            {
                              /* We use the fact here that the unmasked
                                 exception in the loaded data was for a
                                 denormal operand */
                              /* Restore the state of the denormal op bit */
                              partial_status &= ~SW_Denorm_Op;
                              partial_status |= status1 & SW_Denorm_Op;
                            }
                          else
                            setsign(st0_ptr, getsign(&loaded_data));
                        }
                    }
                  goto reg_mem_instr_done;
                }

              switch ( (FPU_modrm >> 3) & 7 )
                {
                case 0:         /* fadd */
                  clear_C1();
                  FPU_add(&loaded_data, loaded_tag, 0, control_word);
                  break;
                case 1:         /* fmul */
                  clear_C1();
                  FPU_mul(&loaded_data, loaded_tag, 0, control_word);
                  break;
                case 2:         /* fcom */
                  FPU_compare_st_data(&loaded_data, loaded_tag);
                  break;
                case 3:         /* fcomp */
		  // bbd: used to typecase to int first, but this corrupted the
		  // pointer on 64 bit machines.
                  if ( !FPU_compare_st_data(&loaded_data, loaded_tag)
                       && !unmasked )
                    FPU_pop();
                  break;
                case 4:         /* fsub */
                  clear_C1();
                  FPU_sub(LOADED|loaded_tag, &loaded_data, control_word);
                  break;
                case 5:         /* fsubr */
                  clear_C1();
                  FPU_sub(REV|LOADED|loaded_tag, &loaded_data, control_word);
                  break;
                case 6:         /* fdiv */
                  clear_C1();
                  FPU_div(LOADED|loaded_tag, &loaded_data, control_word);
                  break;
                case 7:         /* fdivr */
                  clear_C1();
                  if ( st0_tag == TAG_Zero )
                    partial_status = status1;  /* Undo any denorm tag,
                                                  zero-divide has priority. */
                  FPU_div(REV|LOADED|loaded_tag, &loaded_data, control_word);
                  break;
                }
            }
          else
            {
              if ( (FPU_modrm & 0x30) == 0x10 )
                {
                  /* The instruction is fcom or fcomp */
                  EXCEPTION(EX_StackUnder);
                  setcc(SW_C3 | SW_C2 | SW_C0);
                  if ( (FPU_modrm & 0x08) && (control_word & CW_Invalid) )
                    FPU_pop();             /* fcomp */
                }
              else
                FPU_stack_underflow();
            }
        reg_mem_instr_done:
          operand_address = data_sel_off;
        }
      else {
          if ( !(no_ip_update =
                 FPU_load_store(((FPU_modrm & 0x38) | (byte1 & 6)) >> 1,
                                addr_modes, data_address)) )
            {
              operand_address = data_sel_off;
            }
        }
    }
  else {
      /* None of these instructions access user memory */
      u_char instr_index = (FPU_modrm & 0x38) | (byte1 & 7);

#ifdef PECULIAR_486
      /* This is supposed to be undefined, but a real 80486 seems
         to do this: */
      operand_address.offset = 0;
      operand_address.selector = FPU_DS;
#endif /* PECULIAR_486 */

      st0_ptr = &st(0);
      st0_tag = FPU_gettag0();
      switch ( type_table[(int) instr_index] )
        {
        case _NONE_:   /* also _REGIc: _REGIn */
          break;
        case _REG0_:
          if ( !NOT_EMPTY_ST0 )
            {
              FPU_stack_underflow();
              goto FPU_instruction_done;
            }
          break;
        case _REGIi:
          if ( !NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm) )
            {
              FPU_stack_underflow_i(FPU_rm);
              goto FPU_instruction_done;
            }
          break;
        case _REGIp:
          if ( !NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm) )
            {
              FPU_stack_underflow_pop(FPU_rm);
              goto FPU_instruction_done;
            }
          break;
        case _REGI_:
          if ( !NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm) )
            {
              FPU_stack_underflow();
              goto FPU_instruction_done;
            }
          break;
        case _PUSH_:     /* Only used by the fld st(i) instruction */
          break;
        case _null_:
          FPU_illegal();
          goto FPU_instruction_done;
        default:
          EXCEPTION(EX_INTERNAL|0x111);
          goto FPU_instruction_done;
        }
      (*st_instr_table[(int) instr_index])();

FPU_instruction_done:
      ;
    }

  if ( ! no_ip_update )
    instruction_address = entry_sel_off;

FPU_fwait_done:

#ifdef DEBUG
  FPU_printall();
#endif /* DEBUG */
#ifdef BX_NO_BLANK_LABELS
  if(0);
#endif
}
