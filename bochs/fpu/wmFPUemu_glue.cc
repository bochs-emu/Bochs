//  Copyright (C) 2001  MandrakeSoft S.A.
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


//
// This is the glue logic needed to connect the wm-FPU-emu
// FPU emulator written by Bill Metzenthen to bochs.
//


#include "bochs.h"
#include <math.h>

#if !BX_WITH_MACOS
extern "C" {
#endif

#include "fpu_emu.h"
#include "linux/signal.h"

#if !BX_WITH_MACOS
}
#endif

#define LOG_THIS genlog->
#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif

// Use this to hold a pointer to the instruction since
// we can't pass this to the FPU emulation routines, which
// will ultimately call routines here.
static bxInstruction_c *fpu_iptr = NULL;
static BX_CPU_C *fpu_cpu_ptr = NULL;

i387_t *current_i387;

extern "C" void
math_emulate(fpu_addr_modes addr_modes,
              u_char  FPU_modrm,
              u_char byte1,
              bx_address data_address,
              struct address data_sel_off,
              struct address entry_sel_off);


  // This is called by bochs upon reset
  void
BX_CPU_C::fpu_init(void)
{
  current_i387 = &(BX_CPU_THIS_PTR the_i387);
  finit();
}

  void
BX_CPU_C::fpu_execute(bxInstruction_c *i)
{
  fpu_addr_modes addr_modes;
  bx_address data_address;
  struct address data_sel_off;
  struct address entry_sel_off;
  bx_bool is_32;

  fpu_iptr = i;
  fpu_cpu_ptr = this;
  current_i387 = &(BX_CPU_THIS_PTR the_i387);

  is_32 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;

  if (protected_mode()) {
    if (is_32) addr_modes.default_mode = SEG32;
    else addr_modes.default_mode = PM16;
    }
  else if (v8086_mode()) {
    addr_modes.default_mode = VM86;
    }
  else {
    // real mode, use vm86 for now
    addr_modes.default_mode = VM86;
    }

  // Mark if instruction used opsize or addrsize prefixes
  if (i->as32B() == is_32)
    addr_modes.override.address_size = 0;
  else
    addr_modes.override.address_size = ADDR_SIZE_PREFIX;

  if (i->os32B() == is_32)
    addr_modes.override.operand_size = 0;
  else
    addr_modes.override.operand_size = OP_SIZE_PREFIX;

  // fill in orig eip here in offset
  // fill in CS in selector
  entry_sel_off.offset = BX_CPU_THIS_PTR prev_eip;
  entry_sel_off.selector = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

// should set these fields to 0 if mem operand not used
  data_address = RMAddr(i);
  data_sel_off.offset = RMAddr(i);
  data_sel_off.selector = BX_CPU_THIS_PTR sregs[i->seg()].selector.value;

  math_emulate(addr_modes, i->modrm(), i->b1(), data_address,
                data_sel_off, entry_sel_off);
}

void BX_CPU_C::print_state_FPU()
{
  static double sigh_scale_factor = pow(2.0, -31.0);
  static double sigl_scale_factor = pow(2.0, -63.0);

  Bit32u reg;
  reg = i387.cwd;
  fprintf(stderr, "cwd            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.swd;
  fprintf(stderr, "swd            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.twd;
  fprintf(stderr, "twd            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.fip;
  fprintf(stderr, "fip            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.fcs;
  fprintf(stderr, "fcs            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.foo;
  fprintf(stderr, "foo            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  reg = i387.fos;
  fprintf(stderr, "fos            0x%-8x\t%d\n", (unsigned) reg, (int) reg);
  // print stack too
  for (int i=0; i<8; i++) {
    FPU_REG *fpr = &st(i);
    double f1 = pow(2.0, ((0x7fff&fpr->exp) - EXTENDED_Ebias));
    if (fpr->exp & SIGN_Negative) f1 = -f1;
    double f2 = ((double)fpr->sigh * sigh_scale_factor);
    double f3 = ((double)fpr->sigl * sigl_scale_factor);
    double f = f1*(f2+f3);
    fprintf(stderr, "st%d            %.10f (raw 0x%04x%08x%08x)\n", i, f, 0xffff&fpr->exp, fpr->sigh, fpr->sigl);
  }
}

  unsigned
fpu_get_ds(void)
{
  return(fpu_cpu_ptr->sregs[BX_SEG_REG_DS].selector.value);
}

  void
fpu_set_ax(Bit16u val16)
{
  fpu_cpu_ptr->set_AX(val16);
}

  void
fpu_set_eflags(Bit32u val32)
{
  fpu_cpu_ptr->writeEFlags(val32, 0xFFFFFFFF);
}

  Bit32u 
fpu_get_eflags(void)
{
  return fpu_cpu_ptr->read_eflags();
}

  void BX_CPP_AttrRegparmN(3)
fpu_verify_area(unsigned what, bx_address ptr, unsigned n)
{
  bx_segment_reg_t *seg;

  seg = &fpu_cpu_ptr->sregs[fpu_iptr->seg()];

  if (what == VERIFY_READ) {
    fpu_cpu_ptr->read_virtual_checks(seg, ptr, n);
    }
  else {  // VERIFY_WRITE
    fpu_cpu_ptr->write_virtual_checks(seg, ptr, n);
    }
}

  Bit32u BX_CPP_AttrRegparmN(2)
fpu_get_user(bx_address ptr, unsigned len)
{
  Bit32u val32;
  Bit16u val16;
  Bit8u  val8;

  switch (len) {
    case 1:
      fpu_cpu_ptr->read_virtual_byte(fpu_iptr->seg(), ptr, &val8);
      val32 = val8;
      break;
    case 2:
      fpu_cpu_ptr->read_virtual_word(fpu_iptr->seg(), ptr, &val16);
      val32 = val16;
      break;
    case 4:
      fpu_cpu_ptr->read_virtual_dword(fpu_iptr->seg(), ptr, &val32);
      break;
    default:
      BX_PANIC(("fpu_get_user: len=%u", len));
    }
  return(val32);
}

  void BX_CPP_AttrRegparmN(3)
fpu_put_user(Bit32u val, bx_address ptr, unsigned len)
{
  Bit32u val32;
  Bit16u val16;
  Bit8u  val8;

  switch (len) {
    case 1:
      val8 = val;
      fpu_cpu_ptr->write_virtual_byte(fpu_iptr->seg(), ptr, &val8);
      break;
    case 2:
      val16 = val;
      fpu_cpu_ptr->write_virtual_word(fpu_iptr->seg(), ptr, &val16);
      break;
    case 4:
      val32 = val;
      fpu_cpu_ptr->write_virtual_dword(fpu_iptr->seg(), ptr, &val32);
      break;
    default:
      BX_PANIC(("fpu_put_user: len=%u", len));
    }
}

  void
math_abort(void *info, unsigned int signal)
{
  UNUSED(info); // info is always passed NULL
#if BX_CPU_LEVEL >= 4

// values of signal:
//   SIGILL  : opcodes which are illegal
//   SIGFPE  : unmasked FP exception before WAIT or non-control instruction
//   SIGSEGV : access data beyond segment violation
  switch (signal) {
    case SIGFPE:
      if (fpu_cpu_ptr->cr0.ne == 0) {
        // MSDOS compatibility external interrupt (IRQ13)
        BX_INFO (("math_abort: MSDOS compatibility FPU exception"));

        DEV_pic_raise_irq(13);
        return;
        }
      fpu_cpu_ptr->exception(BX_MF_EXCEPTION, 0, 0);
      // execution does not reach here

    case SIGILL:
      BX_PANIC (("math_abort: SIGILL not implemented yet."));
      fpu_cpu_ptr->UndefinedOpcode(fpu_iptr);
      break;

    case SIGSEGV:
      BX_PANIC (("math_abort: SIGSEGV not implemented yet."));
      break;
    }

#else
  UNUSED(signal);
  BX_INFO(("math_abort: CPU<4 not supported yet"));
#endif
}

extern "C" int printk(const char * fmt, ...)
{
  BX_INFO(("math abort: %s", fmt));
  return 0;
}
