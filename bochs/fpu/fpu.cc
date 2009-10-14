/////////////////////////////////////////////////////////////////////////
// $Id: fpu.cc,v 1.57 2009-10-14 20:45:29 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2009 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

#define CHECK_PENDING_EXCEPTIONS 1

#if BX_SUPPORT_FPU
void BX_CPU_C::prepareFPU(bxInstruction_c *i, bx_bool check_pending_exceptions)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);

  if (check_pending_exceptions)
    BX_CPU_THIS_PTR FPU_check_pending_exceptions();
}

void BX_CPU_C::FPU_update_last_instruction(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR the_i387.foo = (((Bit32u)(i->b1()) << 8) | i->modrm()) & 0x7ff;
  BX_CPU_THIS_PTR the_i387.fcs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  BX_CPU_THIS_PTR the_i387.fip = BX_CPU_THIS_PTR prev_rip;

  if (! i->modC0()) {
     BX_CPU_THIS_PTR the_i387.fds = BX_CPU_THIS_PTR sregs[i->seg()].selector.value;
     BX_CPU_THIS_PTR the_i387.fdp = RMAddr(i);
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

bx_address BX_CPU_C::fpu_save_environment(bxInstruction_c *i)
{
    unsigned offset;

    /* read all registers in stack order and update x87 tag word */
    for(int n=0;n<8;n++) {
       // update tag only if it is not empty
       if (! IS_TAG_EMPTY(n)) {
           int tag = FPU_tagof(BX_READ_FPU_REG(n));
           BX_CPU_THIS_PTR the_i387.FPU_settagi(tag, n);
       }
    }

    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

    if (protected_mode())  /* Protected Mode */
    {
        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;

            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_dword(i->seg(), eaddr, tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_dword(i->seg(), eaddr + 0x04, tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_dword(i->seg(), eaddr + 0x08, tmp);
            tmp = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip);
            write_virtual_dword(i->seg(), eaddr + 0x0c, tmp);
            tmp  = (BX_CPU_THIS_PTR the_i387.fcs & 0xffff) |
                          ((Bit32u)(BX_CPU_THIS_PTR the_i387.foo)) << 16;
            write_virtual_dword(i->seg(), eaddr + 0x10, tmp);
            tmp = (Bit32u)(BX_CPU_THIS_PTR the_i387.fdp);
            write_virtual_dword(i->seg(), eaddr + 0x14, tmp);
            tmp = 0xffff0000 | (BX_CPU_THIS_PTR the_i387.fds);
            write_virtual_dword(i->seg(), eaddr + 0x18, tmp);

            offset = 0x1c;
        }
        else /* Protected Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_word(i->seg(), eaddr, tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_word(i->seg(), eaddr + 0x02, tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_word(i->seg(), eaddr + 0x04, tmp);
            tmp = (Bit16u)(BX_CPU_THIS_PTR the_i387.fip) & 0xffff;
            write_virtual_word(i->seg(), eaddr + 0x06, tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fcs);
            write_virtual_word(i->seg(), eaddr + 0x08, tmp);
            tmp = (Bit16u)(BX_CPU_THIS_PTR the_i387.fdp) & 0xffff;
            write_virtual_word(i->seg(), eaddr + 0x0a, tmp);
            tmp = (BX_CPU_THIS_PTR the_i387.fds);
            write_virtual_word(i->seg(), eaddr + 0x0c, tmp);

            offset = 0x0e;
        }
    }
    else   /* Real or V86 Mode */
    {
        Bit32u fp_ip = ((Bit32u)(BX_CPU_THIS_PTR the_i387.fcs) << 4) +
              (Bit32u)(BX_CPU_THIS_PTR the_i387.fip);
        Bit32u fp_dp = ((Bit32u)(BX_CPU_THIS_PTR the_i387.fds) << 4) +
              (Bit32u)(BX_CPU_THIS_PTR the_i387.fdp);

        if (i->os32L())
        {
            Bit32u tmp;

            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_dword(i->seg(), eaddr, tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_dword(i->seg(), eaddr + 0x04, tmp);
            tmp = 0xffff0000 | BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_dword(i->seg(), eaddr + 0x08, tmp);
            tmp = 0xffff0000 | (fp_ip & 0xffff);
            write_virtual_dword(i->seg(), eaddr + 0x0c, tmp);
            tmp = ((fp_ip & 0xffff0000) >> 4) | BX_CPU_THIS_PTR the_i387.foo;
            write_virtual_dword(i->seg(), eaddr + 0x10, tmp);
            tmp = 0xffff0000 | (fp_dp & 0xffff);
            write_virtual_dword(i->seg(), eaddr + 0x14, tmp);
            tmp = (fp_dp & 0xffff0000) >> 4;
            write_virtual_dword(i->seg(), eaddr + 0x18, tmp);

            offset = 0x1c;
        }
        else  /* Real or V86 Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = BX_CPU_THIS_PTR the_i387.get_control_word();
            write_virtual_word(i->seg(), eaddr, tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_status_word();
            write_virtual_word(i->seg(), eaddr + 0x02, tmp);
            tmp = BX_CPU_THIS_PTR the_i387.get_tag_word();
            write_virtual_word(i->seg(), eaddr + 0x04, tmp);
            tmp = fp_ip & 0xffff;
            write_virtual_word(i->seg(), eaddr + 0x06, tmp);
            tmp = (Bit16u)((fp_ip & 0xf0000) >> 4) | BX_CPU_THIS_PTR the_i387.foo;
            write_virtual_word(i->seg(), eaddr + 0x08, tmp);
            tmp = fp_dp & 0xffff;
            write_virtual_word(i->seg(), eaddr + 0x0a, tmp);
            tmp = (Bit16u)((fp_dp & 0xf0000) >> 4);
            write_virtual_word(i->seg(), eaddr + 0x0c, tmp);

            offset = 0x0e;
        }
    }

    return (eaddr + offset);
}

bx_address BX_CPU_C::fpu_load_environment(bxInstruction_c *i)
{
    unsigned offset;

    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

    if (protected_mode())  /* Protected Mode */
    {
        if (i->os32L() || i->os64L())
        {
            Bit32u tmp;

            tmp = read_virtual_dword(i->seg(), eaddr);
            BX_CPU_THIS_PTR the_i387.cwd = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x04);
            BX_CPU_THIS_PTR the_i387.swd = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x08);
            BX_CPU_THIS_PTR the_i387.twd = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x0c);
            BX_CPU_THIS_PTR the_i387.fip = tmp;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x10);
            BX_CPU_THIS_PTR the_i387.fcs = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.foo = (tmp >> 16) & 0x07ff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x14);
            BX_CPU_THIS_PTR the_i387.fdp = tmp;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x18);
            BX_CPU_THIS_PTR the_i387.fds = tmp & 0xffff;
            offset = 0x1c;
        }
        else /* Protected Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = read_virtual_word(i->seg(), eaddr);
            BX_CPU_THIS_PTR the_i387.cwd = tmp;
            tmp = read_virtual_word(i->seg(), eaddr + 0x2);
            BX_CPU_THIS_PTR the_i387.swd = tmp;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            tmp = read_virtual_word(i->seg(), eaddr + 0x04);
            BX_CPU_THIS_PTR the_i387.twd = tmp;
            tmp = read_virtual_word(i->seg(), eaddr + 0x06);
            BX_CPU_THIS_PTR the_i387.fip = tmp & 0xffff;
            tmp = read_virtual_word(i->seg(), eaddr + 0x08);
            BX_CPU_THIS_PTR the_i387.fcs = tmp;
            tmp = read_virtual_word(i->seg(), eaddr + 0x0a);
            BX_CPU_THIS_PTR the_i387.fdp = tmp & 0xffff;
            tmp = read_virtual_word(i->seg(), eaddr + 0x0c);
            BX_CPU_THIS_PTR the_i387.fds = tmp;
            /* opcode is defined to be zero */
            BX_CPU_THIS_PTR the_i387.foo = 0;
            offset = 0x0e;
        }
    }
    else   /* Real or V86 Mode */
    {
        Bit32u fp_ip = 0, fp_dp = 0;

        if (i->os32L())
        {
            Bit32u tmp;

            tmp = read_virtual_dword(i->seg(), eaddr);
            BX_CPU_THIS_PTR the_i387.cwd = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x04);
            BX_CPU_THIS_PTR the_i387.swd = tmp & 0xffff;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x08);
            BX_CPU_THIS_PTR the_i387.twd = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x0c);
            fp_ip = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x10);
            fp_ip = fp_ip | ((tmp & 0x0ffff000) << 4);
            BX_CPU_THIS_PTR the_i387.fip = fp_ip;
            BX_CPU_THIS_PTR the_i387.foo = tmp & 0x07ff;
            BX_CPU_THIS_PTR the_i387.fcs = 0;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x14);
            fp_dp = tmp & 0xffff;
            tmp = read_virtual_dword(i->seg(), eaddr + 0x18);
            fp_dp = fp_dp | ((tmp & 0x0ffff000) << 4);
            BX_CPU_THIS_PTR the_i387.fdp = fp_dp;
            BX_CPU_THIS_PTR the_i387.fds = 0;
            offset = 0x1c;
        }
        else  /* Real or V86 Mode - 16 bit */
        {
            Bit16u tmp;

            tmp = read_virtual_word(i->seg(), eaddr);
            BX_CPU_THIS_PTR the_i387.cwd = tmp;
            tmp = read_virtual_word(i->seg(), eaddr + 0x2);
            BX_CPU_THIS_PTR the_i387.swd = tmp;
            BX_CPU_THIS_PTR the_i387.tos = (tmp >> 11) & 0x07;
            tmp = read_virtual_word(i->seg(), eaddr + 0x04);
            BX_CPU_THIS_PTR the_i387.twd = tmp;
            tmp = read_virtual_word(i->seg(), eaddr + 0x06);
            fp_ip = tmp & 0xffff;
            tmp = read_virtual_word(i->seg(), eaddr + 0x08);
            fp_ip = fp_ip | ((tmp & 0xf000) << 4);
            BX_CPU_THIS_PTR the_i387.fip = fp_ip;
            BX_CPU_THIS_PTR the_i387.foo = tmp & 0x07ff;
            BX_CPU_THIS_PTR the_i387.fcs = 0;
            tmp = read_virtual_word(i->seg(), eaddr + 0x0a);
            fp_dp = tmp & 0xffff;
            tmp = read_virtual_word(i->seg(), eaddr + 0x0c);
            fp_dp = fp_dp | ((tmp & 0xf000) << 4);
            BX_CPU_THIS_PTR the_i387.fdp = fp_dp;
            BX_CPU_THIS_PTR the_i387.fds = 0;
            offset = 0x0e;
        }
    }

    /* always set bit 6 as '1 */
    BX_CPU_THIS_PTR the_i387.cwd =
       (BX_CPU_THIS_PTR the_i387.cwd & ~FPU_CW_Reserved_Bits) | 0x0040;

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

    return (eaddr + offset);
}

/* D9 /5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLDCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, CHECK_PENDING_EXCEPTIONS);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u cwd = read_virtual_word(i->seg(), eaddr);
  FPU_CONTROL_WORD = (cwd & ~FPU_CW_Reserved_Bits) | 0x0040; // bit 6 is reserved as '1

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
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNSTCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);

  Bit16u cwd = BX_CPU_THIS_PTR the_i387.get_control_word();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_word(i->seg(), eaddr, cwd);
#else
  BX_INFO(("FNSTCW: required FPU, configure --enable-fpu"));
#endif
}

/* DD /7 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNSTSW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);

  Bit16u swd = BX_CPU_THIS_PTR the_i387.get_status_word();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_word(i->seg(), eaddr, swd);
#else
  BX_INFO(("FNSTSW: required FPU, configure --enable-fpu"));
#endif
}

/* DF E0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNSTSW_AX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);
  AX = BX_CPU_THIS_PTR the_i387.get_status_word();
#else
  BX_INFO(("FNSTSW_AX: required FPU, configure --enable-fpu"));
#endif
}

/* DD /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, CHECK_PENDING_EXCEPTIONS);

  bx_address offset = fpu_load_environment(i);
  floatx80 tmp;

  /* read all registers in stack order */
  for(int n=0;n<8;n++)
  {
     tmp.fraction = read_virtual_qword(i->seg(), offset + n*10);
     tmp.exp      = read_virtual_word (i->seg(), offset + n*10 + 8);

     // update tag only if it is not empty
     BX_WRITE_FPU_REGISTER_AND_TAG(tmp,
              IS_TAG_EMPTY(n) ? FPU_Tag_Empty : FPU_tagof(tmp), n);
  }
#else
  BX_INFO(("FRSTOR: required FPU, configure --enable-fpu"));
#endif
}

/* DD /6 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);

  bx_address offset = fpu_save_environment(i);

  /* save all registers in stack order. */
  for(int n=0;n<8;n++)
  {
     floatx80 stn = BX_READ_FPU_REG(n);
     write_virtual_qword(i->seg(), offset + n*10,     stn.fraction);
     write_virtual_word (i->seg(), offset + n*10 + 8, stn.exp);
  }

  BX_CPU_THIS_PTR the_i387.init();
#else
  BX_INFO(("FNSAVE: required FPU, configure --enable-fpu"));
#endif
}

/* 9B E2 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNCLEX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);

  FPU_PARTIAL_STATUS &= ~(FPU_SW_Backward|FPU_SW_Summary|FPU_SW_Stack_Fault|FPU_SW_Precision|
                   FPU_SW_Underflow|FPU_SW_Overflow|FPU_SW_Zero_Div|FPU_SW_Denormal_Op|
                   FPU_SW_Invalid);

  // do not update last fpu instruction pointer
#else
  BX_INFO(("FNCLEX: required FPU, configure --enable-fpu"));
#endif
}

/* DB E3 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNINIT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);
  BX_CPU_THIS_PTR the_i387.init();
#else
  BX_INFO(("FNINIT: required FPU, configure --enable-fpu"));
#endif
}

/* D9 /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLDENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, CHECK_PENDING_EXCEPTIONS);
  fpu_load_environment(i);

  /* read all registers in stack order and update x87 tag word */
  for(int n=0;n<8;n++) {
     // update tag only if it is not empty
     if (! IS_TAG_EMPTY(n)) {
         int tag = FPU_tagof(BX_READ_FPU_REG(n));
         BX_CPU_THIS_PTR the_i387.FPU_settagi(tag, n);
     }
  }
#else
  BX_INFO(("FLDENV: required FPU, configure --enable-fpu"));
#endif
}

/* D9 /6 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNSTENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);
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
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FNOP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, CHECK_PENDING_EXCEPTIONS);
  FPU_update_last_instruction(i);

  // Perform no FPU operation. This instruction takes up space in the
  // instruction stream but does not affect the FPU or machine
  // context, except the EIP register.
#else
  BX_INFO(("FNOP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FPLEGACY(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  prepareFPU(i, !CHECK_PENDING_EXCEPTIONS);

  // FPU performs no specific operation and no internal x87 states
  // are affected
#else
  BX_INFO(("legacy FPU opcodes: required FPU, configure --enable-fpu"));
#endif
}

#endif

#if BX_SUPPORT_FPU

#include "softfloatx80.h"

#include <math.h>

void BX_CPU_C::print_state_FPU(void)
{
  static double scale_factor = pow(2.0, -63.0);
  static const char* cw_round_control[] = {
    "NEAREST", "DOWN", "UP", "CHOP"
  };
  static const char* cw_precision_control[] = {
    "32", "RES", "64", "80"
  };
  static const char* fp_class[] = {
    "ZERO", "xNAN", "-INF", "+INF", "DENORMAL", "NORMAL"
  };

  Bit32u reg;
  reg = BX_CPU_THIS_PTR the_i387.get_status_word();
  fprintf(stderr, "status  word: 0x%04x: ", reg);
  fprintf(stderr, "%s %s TOS%d %s %s %s %s %s %s %s %s %s %s %s\n",
    (reg & FPU_SW_Backward) ? "B" : "b",
    (reg & FPU_SW_C3) ? "C3" : "c3", (FPU_TOS&7),
    (reg & FPU_SW_C2) ? "C2" : "c2",
    (reg & FPU_SW_C1) ? "C1" : "c1",
    (reg & FPU_SW_C0) ? "C0" : "c0",
    (reg & FPU_SW_Summary) ? "ES" : "es",
    (reg & FPU_SW_Stack_Fault) ? "SF" : "sf",
    (reg & FPU_SW_Precision) ? "PE" : "pe",
    (reg & FPU_SW_Underflow) ? "UE" : "ue",
    (reg & FPU_SW_Overflow) ? "OE" : "oe",
    (reg & FPU_SW_Zero_Div) ? "ZE" : "ze",
    (reg & FPU_SW_Denormal_Op) ? "DE" : "de",
    (reg & FPU_SW_Invalid) ? "IE" : "ie");

  reg = BX_CPU_THIS_PTR the_i387.get_control_word();
  fprintf(stderr, "control word: 0x%04x: ", reg);
  fprintf(stderr, "%s RC_%s PC_%s %s %s %s %s %s %s\n",
    (reg & FPU_CW_Inf) ? "INF" : "inf",
    (cw_round_control[(reg & FPU_CW_RC) >> 10]),
    (cw_precision_control[(reg & FPU_CW_PC) >> 8]),
    (reg & FPU_CW_Precision) ? "PM" : "pm",
    (reg & FPU_CW_Underflow) ? "UM" : "um",
    (reg & FPU_CW_Overflow)  ? "OM" : "om",
    (reg & FPU_CW_Zero_Div)  ? "ZM" : "zm",
    (reg & FPU_CW_Denormal)  ? "DM" : "dm",
    (reg & FPU_CW_Invalid)   ? "IM" : "im");

  reg = BX_CPU_THIS_PTR the_i387.get_tag_word();
  fprintf(stderr, "tag word:     0x%04x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.foo;
  fprintf(stderr, "operand:      0x%04x\n", reg);
  reg = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip) & 0xffffffff;
  fprintf(stderr, "fip:          0x%08x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fcs;
  fprintf(stderr, "fcs:          0x%04x\n", reg);
  reg = (Bit32u)(BX_CPU_THIS_PTR the_i387.fdp) & 0xffffffff;
  fprintf(stderr, "fdp:          0x%08x\n", reg);
  reg = BX_CPU_THIS_PTR the_i387.fds;
  fprintf(stderr, "fds:          0x%04x\n", reg);

  // print stack too
  int tos = FPU_TOS & 7;
  for (int i=0; i<8; i++) {
    const floatx80 &fp = BX_FPU_REG(i);
    unsigned tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi((i-tos)&7);
    if (tag != FPU_Tag_Empty) tag = FPU_tagof(fp);
    double f = pow(2.0, ((0x7fff & fp.exp) - 0x3fff));
    if (fp.exp & 0x8000) f = -f;
#ifdef _MSC_VER
    f *= (double)(signed __int64)(fp.fraction>>1) * scale_factor * 2;
#else
    f *= fp.fraction*scale_factor;
#endif
    float_class_t f_class = floatx80_class(fp);
    fprintf(stderr, "%sFP%d ST%d(%c):        raw 0x%04x:%08lx%08lx (%.10f) (%s)\n",
          i==tos?"=>":"  ", i, (i-tos)&7,
          "v0se"[tag],
          fp.exp & 0xffff, GET32H(fp.fraction), GET32L(fp.fraction),
          f, (f_class == float_NaN) ? (floatx80_is_signaling_nan(fp) ? "SNAN" : "QNAN") : fp_class[f_class]);
  }
}

#endif
