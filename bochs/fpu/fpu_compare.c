/*---------------------------------------------------------------------------+
 |  fpu_compare.c                                                            |
 |  $Id: fpu_compare.c,v 1.2 2003-11-01 18:45:16 sshwarts Exp $
 |                                                                           |
 | Code to implement FMOVcc and other P6 FPU instructions.                   |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_system.h"
#include "exception.h"
#include "fpu_emu.h"
#include "status_w.h"

  /* EFLAGS: */

  /* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0|ID|VP|VF|AC|VM|RF
   *
   * 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0
   * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
   *  0|NT| IOPL|OF|DF|IF|TF|SF|ZF| 0|AF| 0|PF| 1|CF
   */

#define EFLAGS_CF 0x01
#define EFLAGS_PF 0x04
#define EFLAGS_AF 0x10
#define EFLAGS_ZF 0x40

void FPU_fcmovb()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (eflags & EFLAGS_CF)
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmove()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (eflags & EFLAGS_ZF)
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovbe()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (eflags & (EFLAGS_ZF | EFLAGS_CF))
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovu()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (eflags & EFLAGS_PF)
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovnb()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (!(eflags & EFLAGS_CF))
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovne()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (!(eflags & EFLAGS_ZF))
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovnbe()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (!(eflags & EFLAGS_CF) && !(eflags & EFLAGS_ZF))
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fcmovnu()
{
  int i = FPU_rm;
  FPU_REG *st0_ptr = &st(0), *sti_ptr = &st(i);
  u_char st0_tag = FPU_gettagi(0);
  u_char sti_tag = FPU_gettagi(i);
  u32 eflags = fpu_get_eflags();

  if (st0_tag == TAG_Empty || sti_tag == TAG_Empty) {
        clear_C1();
        FPU_stack_underflow();
  }

  if (!(eflags & EFLAGS_PF))
        FPU_copy_to_reg0(sti_ptr, sti_tag);
}

void FPU_fucomip()
{
    printk("WARNING: FUCOMIP instruction still not implemented");
}

void FPU_fcomip()
{
    printk("WARNING: FCOMIP instruction still not implemented");
}

void FPU_fucomi()
{
    printk("WARNING: FUCOMI instruction still not implemented");
}

void FPU_fcomi()
{
    printk("WARNING: FCOMI instruction still not implemented");
}
