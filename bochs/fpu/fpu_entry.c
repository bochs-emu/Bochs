/*---------------------------------------------------------------------------+
 |  fpu_entry.c                                                              |
 |  $Id: fpu_entry.c,v 1.18 2003-11-01 18:36:19 sshwarts Exp $
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

#define __BAD__ FPU_illegal   /* Illegal on an 80486, causes SIGILL */

#if BX_CPU_LEVEL < 6

static FUNC const st_instr_table[64] = {
  fadd__,  fld_i_,    __BAD__, __BAD__, fadd_i,  ffree_,  faddp_,  __BAD__,
  fmul__,  fxch_i,    __BAD__, __BAD__, fmul_i,  __BAD__, fmulp_,  __BAD__,
  fcom_st, fp_nop,    __BAD__, __BAD__, __BAD__, fst_i_,  __BAD__, __BAD__,
  fcompst, __BAD__,   __BAD__, __BAD__, __BAD__, fstp_i,  fcompp,  __BAD__,
  fsub__,  FPU_etc,   __BAD__, finit_,  fsubri,  fucom_,  fsubrp,  fstsw_,
  fsubr_,  fconst,    fucompp, __BAD__, fsub_i,  fucomp,  fsubp_,  __BAD__,
  fdiv__,  FPU_triga, __BAD__, __BAD__, fdivri,  __BAD__, fdivrp,  __BAD__,
  fdivr_,  FPU_trigb, __BAD__, __BAD__, fdiv_i,  __BAD__, fdivp_,  __BAD__,
};

#else

static FUNC const st_instr_table[64] = {
  fadd__,  fld_i_,    FPU_fcmovb,  FPU_fcmovnb,  fadd_i,  ffree_,  faddp_,  __BAD__,
  fmul__,  fxch_i,    FPU_fcmove,  FPU_fcmovne,  fmul_i,  __BAD__, fmulp_,  __BAD__,
  fcom_st, fp_nop,    FPU_fcmovbe, FPU_fcmovnbe, __BAD__, fst_i_,  __BAD__, __BAD__,
  fcompst, __BAD__,   FPU_fcmovu,  FPU_fcmovnu,  __BAD__, fstp_i,  fcompp,  __BAD__,
  fsub__,  FPU_etc,   __BAD__,     finit_,       fsubri,  fucom_,  fsubrp,  fstsw_,
  fsubr_,  fconst,    fucompp,     FPU_fucomi,   fsub_i,  fucomp,  fsubp_,  FPU_fucomip,
  fdiv__,  FPU_triga, __BAD__,     FPU_fcomi,    fdivri,  __BAD__, fdivrp,  FPU_fcomip,
  fdivr_,  FPU_trigb, __BAD__,     __BAD__,      fdiv_i,  __BAD__, fdivp_,  __BAD__,
};

#endif

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

#if BX_CPU_LEVEL < 6

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

#else

static u_char const type_table[64] = {
  _REGI_, _NONE_, _REGIn, _REGIn, _REGIi, _REGi_, _REGIp, _null_,
  _REGI_, _REGIn, _REGIn, _REGIn, _REGIi, _null_, _REGIp, _null_,
  _REGIc, _NONE_, _REGIn, _REGIn, _null_, _REG0_, _null_, _null_,
  _REGIc, _null_, _REGIn, _REGIn, _null_, _REG0_, _REGIc, _null_,
  _REGI_, _NONE_, _null_, _NONE_, _REGIi, _REGIc, _REGIp, _NONE_,
  _REGI_, _NONE_, _REGIc, _REGIc, _REGIi, _REGIc, _REGIp, _REGIc,
  _REGI_, _NONE_, _null_, _REGIc, _REGIi, _null_, _REGIp, _REGIc,
  _REGI_, _NONE_, _null_, _null_, _REGIi, _null_, _REGIp, _null_
};

#endif

/* Note, this is a version of fpu_entry.c, modified to interface
 * to a CPU simulator, rather than a kernel.
 *
 * Ported by Kevin Lawton Sep 20, 1999
 */

  asmlinkage void
math_emulate(fpu_addr_modes addr_modes,
              u_char FPU_modrm,
              u_char byte1,
              bx_address data_address,
              struct address data_sel_off,
              struct address entry_sel_off)
{
  u16 code;
  int unmasked;
  FPU_REG loaded_data;
  FPU_REG *st0_ptr;
  u_char  loaded_tag, st0_tag;

  /* assuming byte is 0xd8..0xdf or 0xdb==FWAIT */

  /* lock is not a valid prefix for FPU instructions, +++
     let the cpu handle it to generate a SIGILL. */

  no_ip_update = 0;

  if (byte1 == FWAIT_OPCODE) {
    if (FPU_partial_status & SW_Summary)
      goto do_the_FPU_interrupt;
    else
      return;
    }

  if (FPU_partial_status & SW_Summary) {
    /* Ignore the error for now if the current instruction is a no-wait
       control instruction */
    /* The 80486 manual contradicts itself on this topic,
       but a real 80486 uses the following instructions:
       fninit, fnstenv, fnsave, fnstsw, fnstenv, fnclex.
       */
    code = (FPU_modrm << 8) | byte1;
    if (! ((((code & 0xf803) == 0xe003) ||    /* fnclex, fninit, fnstsw */
              (((code & 0x3003) == 0x3001) &&   /* fnsave, fnstcw, fnstenv,
                                                   fnstsw */
               ((code & 0xc000) != 0xc000))))) {
      /*
       *  We need to simulate the action of the kernel to FPU
       *  interrupts here.
       */
do_the_FPU_interrupt:

      math_abort(NULL, SIGFPE);
      }
    }

  entry_sel_off.opcode = (byte1 << 8) | FPU_modrm;

  FPU_rm = FPU_modrm & 7;

  if (FPU_modrm < 0300) {
      /* All of these instructions use the mod/rm byte to get a data address */

      if (!(byte1 & 1)) {
          u16 status1 = FPU_partial_status;

          st0_ptr = &st(0);
          st0_tag = FPU_gettag0();

          /* Stack underflow has priority */
          if (NOT_EMPTY_ST0) {
              unmasked = 0;  /* Do this here to stop compiler warnings. */
              switch ((byte1 >> 1) & 3)
                {
                case 0:
                  unmasked = FPU_load_single(data_address, &loaded_data);
                  loaded_tag = unmasked & 0xff;
                  unmasked &= ~0xff;
                  break;
                case 1:
                  loaded_tag = FPU_load_int32(data_address, &loaded_data);
                  break;
                case 2:
                  unmasked = FPU_load_double(data_address, &loaded_data);
                  loaded_tag = unmasked & 0xff;
                  unmasked &= ~0xff;
                  break;
                case 3:
                default:  /* Used here to suppress gcc warnings. */
                  loaded_tag = FPU_load_int16(data_address, &loaded_data);
                  break;
                }

              /* No more access to user memory, it is safe
                 to use static data now */

              /* NaN operands have the next priority. */
              /* We have to delay looking at st(0) until after
                 loading the data, because that data might contain an SNaN */
              if (((st0_tag == TAG_Special) && isNaN(st0_ptr)) ||
                  ((loaded_tag == TAG_Special) && isNaN(&loaded_data)))
                {
                  /* Restore the status word; we might have loaded a
                     denormal. */
                  FPU_partial_status = status1;
                  if ((FPU_modrm & 0x30) == 0x10)
                    {
                      /* fcom or fcomp */
                      EXCEPTION(EX_Invalid);
                      setcc(SW_C3 | SW_C2 | SW_C0);
                      if ((FPU_modrm & 0x08) && (FPU_control_word & CW_Invalid))
                        FPU_pop();             /* fcomp, masked, so we pop. */
                    }
                  else
                    {
                      if (loaded_tag == TAG_Special)
                        loaded_tag = FPU_Special(&loaded_data);
#ifdef PECULIAR_486
                      /* This is not really needed, but gives behaviour
                         identical to an 80486 */
                      if ((FPU_modrm & 0x28) == 0x20)
                        /* fdiv or fsub */
                        real_2op_NaN(&loaded_data, loaded_tag, 0, &loaded_data);
                      else
#endif /* PECULIAR_486 */
                        /* fadd, fdivr, fmul, or fsubr */
                        real_2op_NaN(&loaded_data, loaded_tag, 0, st0_ptr);
                    }
                  goto reg_mem_instr_done;
                }

              if (unmasked && !((FPU_modrm & 0x30) == 0x10))
                {
                  /* Is not a comparison instruction. */
                  if ((FPU_modrm & 0x38) == 0x38)
                    {
                      /* fdivr */
                      if ((st0_tag == TAG_Zero) &&
                           ((loaded_tag == TAG_Valid)
                            || (loaded_tag == TAG_Special
                                && isdenormal(&loaded_data))))
                        {
                          if (FPU_divide_by_zero(0, getsign(&loaded_data)) < 0)
                            {
                              /* We use the fact here that the unmasked
                                 exception in the loaded data was for a
                                 denormal operand */
                              /* Restore the state of the denormal op bit */
                              FPU_partial_status &= ~SW_Denorm_Op;
                              FPU_partial_status |= status1 & SW_Denorm_Op;
                            }
                          else
                            setsign(st0_ptr, getsign(&loaded_data));
                        }
                    }
                  goto reg_mem_instr_done;
                }

              switch ((FPU_modrm >> 3) & 7)
                {
                case 0:         /* fadd */
                  clear_C1();
                  FPU_add(&loaded_data, loaded_tag, 0, FPU_control_word);
                  break;
                case 1:         /* fmul */
                  clear_C1();
                  FPU_mul(&loaded_data, loaded_tag, 0, FPU_control_word);
                  break;
                case 2:         /* fcom */
                  FPU_compare_st_data(&loaded_data, loaded_tag);
                  break;
                case 3:         /* fcomp */
		  /* bbd: used to typecase to int first, but this corrupted the
		     pointer on 64 bit machines. */
                  if (!FPU_compare_st_data(&loaded_data, loaded_tag) && !unmasked)
                    FPU_pop();
                  break;
                case 4:         /* fsub */
                  clear_C1();
                  FPU_sub(LOADED|loaded_tag, &loaded_data, FPU_control_word);
                  break;
                case 5:         /* fsubr */
                  clear_C1();
                  FPU_sub(REV|LOADED|loaded_tag, &loaded_data, FPU_control_word);
                  break;
                case 6:         /* fdiv */
                  clear_C1();
                  FPU_div(LOADED|loaded_tag, &loaded_data, FPU_control_word);
                  break;
                case 7:         /* fdivr */
                  clear_C1();
                  if (st0_tag == TAG_Zero)
                    FPU_partial_status = status1;  /* Undo any denorm tag,
                                                  zero-divide has priority. */
                  FPU_div(REV|LOADED|loaded_tag, &loaded_data, FPU_control_word);
                  break;
                }
            }
          else
            {
              if ((FPU_modrm & 0x30) == 0x10)
                {
                  /* The instruction is fcom or fcomp */
                  EXCEPTION(EX_StackUnder);
                  setcc(SW_C3 | SW_C2 | SW_C0);
                  if ((FPU_modrm & 0x08) && (FPU_control_word & CW_Invalid))
                    FPU_pop();             /* fcomp */
                }
              else
                FPU_stack_underflow();
            }
        reg_mem_instr_done:
          FPU_operand_address = data_sel_off;
        }
      else {
          if (!(no_ip_update =
                 FPU_load_store(((FPU_modrm & 0x38) | (byte1 & 6)) >> 1,
                                addr_modes, data_address)))
            {
              FPU_operand_address = data_sel_off;
            }
        }
    }
  else {
      /* None of these instructions access user memory */
      u_char instr_index = (FPU_modrm & 0x38) | (byte1 & 7);

#ifdef PECULIAR_486
      /* This is supposed to be undefined, but a real 80486 seems
         to do this: */
      FPU_operand_address.offset = 0;
      FPU_operand_address.selector = FPU_DS;
#endif /* PECULIAR_486 */

      st0_ptr = &st(0);
      st0_tag = FPU_gettag0();
      switch (type_table[(int) instr_index])
        {
        case _NONE_:   /* also _REGIc: _REGIn */
          break;
        case _REG0_:
          if (!NOT_EMPTY_ST0)
            {
              FPU_stack_underflow();
              goto FPU_instruction_done;
            }
          break;
        case _REGIi:
          if (!NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm))
            {
              FPU_stack_underflow_i(FPU_rm);
              goto FPU_instruction_done;
            }
          break;
        case _REGIp:
          if (!NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm))
            {
              FPU_stack_underflow_pop(FPU_rm);
              goto FPU_instruction_done;
            }
          break;
        case _REGI_:
          if (!NOT_EMPTY_ST0 || !NOT_EMPTY(FPU_rm))
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
          INTERNAL(0x111);
          goto FPU_instruction_done;
        }
      (*st_instr_table[(int) instr_index])();

FPU_instruction_done:
      ;
    }

  if (! no_ip_update)
    FPU_instruction_address = entry_sel_off;
}
