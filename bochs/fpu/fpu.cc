/////////////////////////////////////////////////////////////////////////
// $Id: fpu.cc,v 1.26 2007-10-30 18:52:25 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

#define UPDATE_LAST_OPCODE       1
#define CHECK_PENDING_EXCEPTIONS 1


#if BX_SUPPORT_FPU
void BX_CPU_C::prepareFPU(bxInstruction_c *i, 
	bx_bool check_pending_exceptions, bx_bool update_last_instruction)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);

  if (check_pending_exceptions)
    BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  if (update_last_instruction)
  {
    BX_CPU_THIS_PTR the_i387.foo = ((Bit32u)(i->b1()) << 8) | (Bit32u)(i->modrm()) & 0x7ff;
    BX_CPU_THIS_PTR the_i387.fcs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
    BX_CPU_THIS_PTR the_i387.fip = BX_CPU_THIS_PTR prev_eip;

    if (! i->modC0()) {
         BX_CPU_THIS_PTR the_i387.fds = BX_CPU_THIS_PTR sregs[i->seg()].selector.value;
         BX_CPU_THIS_PTR the_i387.fdp = RMAddr(i);
    } else {
         BX_CPU_THIS_PTR the_i387.fds = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
         BX_CPU_THIS_PTR the_i387.fdp = 0;
    }
  }
}

void BX_CPU_C::FPU_check_pending_exceptions(void)
{
  if(BX_CPU_THIS_PTR the_i387.get_partial_status() & FPU_SW_Summary)
  {
     // NE=1 selects the native or internal mode, which generates #MF,
     // which is an extension introduced with 80486.
     // NE=0 selects the original (backward compatible) FPU error
     // handling, which generates an IRQ 13 via the PIC chip.
#if BX_CPU_LEVEL >= 4
     if (BX_CPU_THIS_PTR cr0.get_NE() != 0) {
         exception(BX_MF_EXCEPTION, 0, 0);
     }
     else
#endif
     {
        // MSDOS compatibility external interrupt (IRQ13)
        BX_INFO (("math_abort: MSDOS compatibility FPU exception"));
        DEV_pic_raise_irq(13);
     }
  }
}

int BX_CPU_C::fpu_save_environment(bxInstruction_c *i)
{
    if (protected_mode())  /* Protected Mode */
    {
        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;

            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_dword(i->seg(), RMAddr(i), &tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_dword(i->seg(), RMAddr(i) + 0x04, &tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_dword(i->seg(), RMAddr(i) + 0x08, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fip) & 0xffffffff;
            write_virtual_dword(i->seg(), RMAddr(i) + 0x0c, &tmp);
            tmp  = (BX_CPU_THIS_PTR the_i387.fcs & 0xffff) |
                          ((Bit32u)(BX_CPU_THIS_PTR the_i387.foo)) << 16;
            write_virtual_dword(i->seg(), RMAddr(i) + 0x10, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fdp) & 0xffffffff;
            write_virtual_dword(i->seg(), RMAddr(i) + 0x14, &tmp);
            tmp = 0xffff0000 | (BX_CPU_THIS_PTR the_i387.fds);
            write_virtual_dword(i->seg(), RMAddr(i) + 0x18, &tmp);

            return 0x1c;
        }
        else /* Protected Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_word(i->seg(), RMAddr(i), &tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_word(i->seg(), RMAddr(i) + 0x02, &tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_word(i->seg(), RMAddr(i) + 0x04, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fip) & 0xffff;
            write_virtual_word(i->seg(), RMAddr(i) + 0x06, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fcs);
            write_virtual_word(i->seg(), RMAddr(i) + 0x08, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fdp) & 0xffff;
            write_virtual_word(i->seg(), RMAddr(i) + 0x0a, &tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fds);
            write_virtual_word(i->seg(), RMAddr(i) + 0x0c, &tmp);

            return 0x0e;
        }
    }
    else   /* Real or V86 Mode */
    {
        Bit32u fp_ip = ((Bit32u)(BX_CPU_THIS_PTR the_i387.fcs) << 4) +
              (BX_CPU_THIS_PTR the_i387.fip);
        Bit32u fp_dp = ((Bit32u)(BX_CPU_THIS_PTR the_i387.fds) << 4) +
              (BX_CPU_THIS_PTR the_i387.fdp);

        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;
        
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_dword(i->seg(), RMAddr(i), &tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_dword(i->seg(), RMAddr(i) + 0x04, &tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_dword(i->seg(), RMAddr(i) + 0x08, &tmp);
            tmp = 0xffff0000 | (fp_ip & 0xffff);
            write_virtual_dword(i->seg(), RMAddr(i) + 0x0c, &tmp);
            tmp = ((fp_ip & 0xffff0000) >> 4) |
                          (BX_CPU_THIS_PTR the_i387.foo & 0x7ff);
            write_virtual_dword(i->seg(), RMAddr(i) + 0x10, &tmp);
            tmp = 0xffff0000 | (fp_dp & 0xffff);
            write_virtual_dword(i->seg(), RMAddr(i) + 0x14, &tmp);
            tmp = (fp_dp & 0xffff0000) >> 4;
            write_virtual_dword(i->seg(), RMAddr(i) + 0x18, &tmp);

            return 0x1c;
        }
        else  /* Real or V86 Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_word(i->seg(), RMAddr(i), &tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_word(i->seg(), RMAddr(i) + 0x02, &tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_word(i->seg(), RMAddr(i) + 0x04, &tmp);
            tmp = fp_ip & 0xffff;
            write_virtual_word(i->seg(), RMAddr(i) + 0x06, &tmp);
            tmp = (Bit16u)(((fp_ip & 0xf0000) >> 4) |
                          (BX_CPU_THIS_PTR the_i387.foo & 0x7ff));
            write_virtual_word(i->seg(), RMAddr(i) + 0x08, &tmp);
            tmp = fp_dp & 0xffff;
            write_virtual_word(i->seg(), RMAddr(i) + 0x0a, &tmp);
            tmp = (Bit16u)((fp_dp & 0xf0000) >> 4);
            write_virtual_word(i->seg(), RMAddr(i) + 0x0c, &tmp);

            return 0x0e;
        }       
    }   
}

int BX_CPU_C::fpu_load_environment(bxInstruction_c *i)
{
    int offset;

    if (protected_mode())  /* Protected Mode */
    {
        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;

            read_virtual_dword(i->seg(), RMAddr(i), &tmp);
            BX_CPU_THIS_PTR the_i387.cwd = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x04, &tmp);
            BX_CPU_THIS_PTR the_i387.swd = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x08, &tmp);
            BX_CPU_THIS_PTR the_i387.twd = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x0c, &tmp);
            BX_CPU_THIS_PTR the_i387.fip = tmp;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x10, &tmp);
            BX_CPU_THIS_PTR the_i387.fcs = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.foo = (tmp >> 16) & 0x07ff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x14, &tmp);
            BX_CPU_THIS_PTR the_i387.fdp = tmp;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x18, &tmp);
            BX_CPU_THIS_PTR the_i387.fds = tmp & 0xffff;
            offset = 0x1c;
        }
        else /* Protected Mode - 16 bit */
        {
            Bit16u tmp;

            read_virtual_word(i->seg(), RMAddr(i), &tmp);
            BX_CPU_THIS_PTR the_i387.cwd = tmp;
            read_virtual_word(i->seg(), RMAddr(i) + 0x2, &tmp);
            BX_CPU_THIS_PTR the_i387.swd = tmp;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            read_virtual_word(i->seg(), RMAddr(i) + 0x04, &tmp);
            BX_CPU_THIS_PTR the_i387.twd = tmp;
            read_virtual_word(i->seg(), RMAddr(i) + 0x06, &tmp);
            BX_CPU_THIS_PTR the_i387.fip = tmp & 0xffff;
            read_virtual_word(i->seg(), RMAddr(i) + 0x08, &tmp);
            BX_CPU_THIS_PTR the_i387.fcs = tmp;
            read_virtual_word(i->seg(), RMAddr(i) + 0x0a, &tmp);
            BX_CPU_THIS_PTR the_i387.fdp = tmp & 0xffff;
            read_virtual_word(i->seg(), RMAddr(i) + 0x0c, &tmp);
            BX_CPU_THIS_PTR the_i387.fds = tmp;
            /* opcode is defined to be zero */
            BX_CPU_THIS_PTR the_i387.foo = 0;
            offset = 0x0e;
        }
    }
    else   /* Real or V86 Mode */
    {
        Bit32u fp_ip = 0, fp_dp = 0;

        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;

            read_virtual_dword(i->seg(), RMAddr(i), &tmp);
            BX_CPU_THIS_PTR the_i387.cwd = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x04, &tmp);
            BX_CPU_THIS_PTR the_i387.swd = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x08, &tmp);
            BX_CPU_THIS_PTR the_i387.twd = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x0c, &tmp);
            fp_ip = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x10, &tmp);
            fp_ip = fp_ip | ((tmp & 0x0ffff000) << 4);
            BX_CPU_THIS_PTR the_i387.fip = fp_ip;
            BX_CPU_THIS_PTR the_i387.foo = tmp & 0x07ff;
            BX_CPU_THIS_PTR the_i387.fcs = 0;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x14, &tmp);
            fp_dp = tmp & 0xffff;
            read_virtual_dword(i->seg(), RMAddr(i) + 0x18, &tmp);
            fp_dp = fp_dp | ((tmp & 0x0ffff000) << 4);
            BX_CPU_THIS_PTR the_i387.fdp = fp_dp;
            BX_CPU_THIS_PTR the_i387.fds = 0;
            offset = 0x1c;
        }
        else  /* Real or V86 Mode - 16 bit */
        {
            Bit16u tmp;

            read_virtual_word(i->seg(), RMAddr(i), &tmp);
            BX_CPU_THIS_PTR the_i387.cwd = tmp;
            read_virtual_word(i->seg(), RMAddr(i) + 0x2, &tmp);
            BX_CPU_THIS_PTR the_i387.swd = tmp;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            read_virtual_word(i->seg(), RMAddr(i) + 0x04, &tmp);
            BX_CPU_THIS_PTR the_i387.twd = tmp;
            read_virtual_word(i->seg(), RMAddr(i) + 0x06, &tmp);
            fp_ip = tmp & 0xffff;
            read_virtual_word(i->seg(), RMAddr(i) + 0x08, &tmp);
            fp_ip = fp_ip | ((tmp & 0xf000) << 4);
            BX_CPU_THIS_PTR the_i387.fip = fp_ip;
            BX_CPU_THIS_PTR the_i387.foo = tmp & 0x07ff;
            BX_CPU_THIS_PTR the_i387.fcs = 0;
            read_virtual_word(i->seg(), RMAddr(i) + 0x0a, &tmp);
            fp_dp = tmp & 0xffff;
            read_virtual_word(i->seg(), RMAddr(i) + 0x0c, &tmp);
            fp_dp = fp_dp | ((tmp & 0xf000) << 4);
            BX_CPU_THIS_PTR the_i387.fdp = fp_dp;
            BX_CPU_THIS_PTR the_i387.fds = 0;
            offset = 0x0e;
        }
    }

    /* check for unmasked exceptions */
    if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask)
    {
        /* set the B and ES bits in the status-word */
        FPU_PARTIAL_STATUS |= FPU_SW_Summary | FPU_SW_Backward;
    }
    else
    {
        /* clear the B and ES bits in the status-word */
        FPU_PARTIAL_STATUS &= ~(FPU_SW_Summary | FPU_SW_Backward);
    }

    return offset;
}
#endif

/* D9 /5 */
void BX_CPU_C::FLDCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  Bit16u cwd;
  read_virtual_word(i->seg(), RMAddr(i), &cwd);
  FPU_CONTROL_WORD = cwd;

  /* check for unmasked exceptions */
  if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask)
  {
      /* set the B and ES bits in the status-word */
      FPU_PARTIAL_STATUS |= FPU_SW_Summary | FPU_SW_Backward;
  }
  else
  {
      /* clear the B and ES bits in the status-word */
      FPU_PARTIAL_STATUS &= ~(FPU_SW_Summary | FPU_SW_Backward);
  }
#else
  BX_INFO(("FLDCW: required FPU, configure --enable-fpu"));
#endif
}

/* D9 /7 */
void BX_CPU_C::FNSTCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  Bit16u cwd = BX_CPU_THIS_PTR the_i387.get_control_word();
  write_virtual_word(i->seg(), RMAddr(i), &cwd);
#else
  BX_INFO(("FNSTCW: required FPU, configure --enable-fpu"));
#endif
}

/* DD /7 */
void BX_CPU_C::FNSTSW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  Bit16u swd = BX_CPU_THIS_PTR the_i387.get_status_word();
  write_virtual_word(i->seg(), RMAddr(i), &swd);
#else
  BX_INFO(("FNSTSW: required FPU, configure --enable-fpu"));
#endif
}

/* DF E0 */
void BX_CPU_C::FNSTSW_AX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  AX = BX_CPU_THIS_PTR the_i387.get_status_word();
#else
  BX_INFO(("FNSTSW_AX: required FPU, configure --enable-fpu"));
#endif
}

/* DD /4 */
void BX_CPU_C::FRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  int offset = fpu_load_environment(i);

  /* read all registers in stack order. */
  for(int n=0;n<8;n++)
  {
     floatx80 tmp;

     // read register only if its tag is not empty
     if (! IS_TAG_EMPTY(n))
     {
         read_virtual_tword(i->seg(), RMAddr(i) + offset + n*10, &tmp);
         BX_WRITE_FPU_REG(tmp, n);
     }
  }
#else
  BX_INFO(("FRSTOR: required FPU, configure --enable-fpu"));
#endif
}

/* DD /6 */
void BX_CPU_C::FNSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);

  int offset = fpu_save_environment(i);

  /* save all registers in stack order. */
  for(int n=0;n<8;n++)
  {
     floatx80 stn = BX_READ_FPU_REG(n);
     write_virtual_tword(i->seg(), RMAddr(i) + offset + n*10, &stn);
  }

  BX_CPU_THIS_PTR the_i387.init();
#else
  BX_INFO(("FNSAVE: required FPU, configure --enable-fpu"));
#endif
}

/* 9B E2 */
void BX_CPU_C::FNCLEX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);

  FPU_PARTIAL_STATUS &= ~(FPU_SW_Backward|FPU_SW_Summary|FPU_SW_Stack_Fault|FPU_SW_Precision|
		   FPU_SW_Underflow|FPU_SW_Overflow|FPU_SW_Zero_Div|FPU_SW_Denormal_Op|
		   FPU_SW_Invalid);

  // do not update last fpu instruction pointer
#else
  BX_INFO(("FNCLEX: required FPU, configure --enable-fpu"));
#endif
}

/* DB E3 */
void BX_CPU_C::FNINIT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  BX_CPU_THIS_PTR the_i387.init();
#else
  BX_INFO(("FNINIT: required FPU, configure --enable-fpu"));
#endif
}

/* D9 /4 */
void BX_CPU_C::FLDENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  fpu_load_environment(i);
#else
  BX_INFO(("FLDENV: required FPU, configure --enable-fpu"));
#endif
}

/* D9 /6 */
void BX_CPU_C::FNSTENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);
  fpu_save_environment(i);
  /* mask all floating point exceptions */
  FPU_CONTROL_WORD |= FPU_CW_Exceptions_Mask;
  /* clear the B and ES bits in the status word */
  FPU_PARTIAL_STATUS &= ~(FPU_SW_Backward|FPU_SW_Summary);
#else
  BX_INFO(("FNSTENV: required FPU, configure --enable-fpu"));
#endif
}

/* D9 D0 */
void BX_CPU_C::FNOP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);

  // Perform no FPU operation. This instruction takes up space in the
  // instruction stream but does not affect the FPU or machine
  // context, except the EIP register.
#else
  BX_INFO(("FNOP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPLEGACY(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i, !CHECK_PENDING_EXCEPTIONS, !UPDATE_LAST_OPCODE);

  // FPU performs no specific operation and no internal x87 states
  // are affected
#else
  BX_INFO(("legacy FPU opcodes: required FPU, configure --enable-fpu"));
#endif
}


#if BX_SUPPORT_FPU

#include <math.h>

void BX_CPU_C::print_state_FPU()
{
  static double scale_factor = pow(2.0, -63.0);

  Bit32u reg;
  reg = BX_CPU_THIS_PTR the_i387.get_control_word();
  fprintf(stderr, "control word: 0x%04x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.get_status_word();
  fprintf(stderr, "status  word: 0x%04x\n", reg);
  fprintf(stderr, "        TOS : %d\n", FPU_TOS&7);
  reg = BX_CPU_THIS_PTR the_i387.get_tag_word();
  fprintf(stderr, "tag word:     0x%04x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.foo;
  fprintf(stderr, "operand:      0x%04x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fip & 0xffffffff;
  fprintf(stderr, "fip:          0x%08x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fcs;
  fprintf(stderr, "fcs:          0x%04x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fdp & 0xffffffff;
  fprintf(stderr, "fdp:          0x%08x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fds;
  fprintf(stderr, "fds:          0x%04x\n", reg);

  // print stack too
  int tos = FPU_TOS & 7;
  for (int i=0; i<8; i++) {
    const floatx80 &fp = BX_FPU_REG(i);
    double f = pow(2.0, ((0x7fff & fp.exp) - 0x3fff));
    if (fp.exp & 0x8000) f = -f;
#ifdef _MSC_VER
    f *= (double)(signed __int64)(fp.fraction>>1) * scale_factor * 2;
#else
    f *= fp.fraction*scale_factor;
#endif
    fprintf(stderr, "%sFPR%d(%c):        %.10f (raw 0x%04x:%08lx%08lx)\n",
          i==tos?"=>":"  ",
          i, 
          "v0s?"[BX_CPU_THIS_PTR the_i387.FPU_gettagi((i-tos)&7)],
          f, fp.exp & 0xffff, fp.fraction >> 32, fp.fraction & 0xffffffff);
  }
}
#endif
