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
extern "C" {
#include "fpu_emu.h"
#include "linux/signal.h"
}

#define LOG_THIS genlog->

// Use this to hold a pointer to the instruction since
// we can't pass this to the FPU emulation routines, which
// will ultimately call routines here.
static BxInstruction_t *fpu_iptr = NULL;

i387_t i387;

extern "C" void
math_emulate2(fpu_addr_modes addr_modes,
              u_char  FPU_modrm,
              u_char byte1,
              void *data_address,
              struct address data_sel_off,
              struct address entry_sel_off);

extern "C" void printfp(char *s, FPU_REG *r);


  // This is called by bochs upon reset
  void
BX_CPU_C::fpu_init(void)
{
  finit();
}

  void
BX_CPU_C::fpu_execute(BxInstruction_t *i)
{
  fpu_addr_modes addr_modes;
  void *data_address;
  struct address data_sel_off;
  struct address entry_sel_off;
  Boolean is_32;

  fpu_iptr = i;

#if 0
  addr_modes.default_mode = VM86;
  addr_modes.default_mode = 0; // FPU_CS == __USER_CS && FPU_DS == __USER_DS
  addr_modes.default_mode = SEG32;
  addr_modes.default_mode = PM16;
#endif
  if (protected_mode()) {
    addr_modes.default_mode = SEG32;
    }
  else if (v8086_mode()) {
    addr_modes.default_mode = VM86;
    }
  else {
    // real mode, use vm86 for now
    addr_modes.default_mode = VM86;
    }


  // Mark if instruction used opsize or addrsize prefixes
  // Actually, addr_modes.override.address_size is not used,
  // could delete that code.
  is_32 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  if (i->as_32 == is_32)
    addr_modes.override.address_size = 0;
  else
    addr_modes.override.address_size = ADDR_SIZE_PREFIX;
  if (i->os_32 == is_32)
    addr_modes.override.operand_size = 0;
  else
    addr_modes.override.operand_size = OP_SIZE_PREFIX;

  // For now set access_limit to max.  It seems to be
  // a number from 0..255 denoting how many bytes the
  // current instruction can access according to its
  // memory operand.  255 means >= 255.
access_limit = 0xff;

  // fill in orig eip here in offset
  // fill in CS in selector
  entry_sel_off.offset = BX_CPU_THIS_PTR prev_eip;
  entry_sel_off.selector =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

// should set these fields to 0 if mem operand not used
  data_address = (void *) i->rm_addr;
  data_sel_off.offset = i->rm_addr;
  data_sel_off.selector = BX_CPU_THIS_PTR sregs[i->seg].selector.value;

  math_emulate2(addr_modes, i->modrm, i->b1, data_address,
                data_sel_off, entry_sel_off);
}


  unsigned
fpu_get_ds(void)
{
  return(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
}

  void
fpu_set_ax(unsigned short val16)
{
  AX = val16;
//fprintf(stderr, "fpu_set_ax(0x%04x)\n", (unsigned) val16);
}

  void
fpu_verify_area(unsigned what, void *ptr, unsigned n)
{
  bx_segment_reg_t *seg;

  seg = &BX_CPU_THIS_PTR sregs[fpu_iptr->seg];

  if (what == VERIFY_READ) {
    BX_CPU.read_virtual_checks(seg, PTR2INT(ptr), n);
    }
  else {  // VERIFY_WRITE
    BX_CPU.write_virtual_checks(seg, PTR2INT(ptr), n);
    }
//fprintf(stderr, "verify_area: 0x%x\n", PTR2INT(ptr));
}


  void
FPU_printall(void)
{
  BX_PANIC(("FPU_printall\n"));
}


  unsigned
fpu_get_user(void *ptr, unsigned len)
{
  Bit32u val32;
  Bit16u val16;
  Bit8u  val8;

  switch (len) {
    case 1:
      BX_CPU.read_virtual_byte(fpu_iptr->seg, PTR2INT(ptr), &val8);
      val32 = val8;
      break;
    case 2:
      BX_CPU.read_virtual_word(fpu_iptr->seg, PTR2INT(ptr), &val16);
      val32 = val16;
      break;
    case 4:
      BX_CPU.read_virtual_dword(fpu_iptr->seg, PTR2INT(ptr), &val32);
      break;
    default:
      BX_PANIC(("fpu_get_user: len=%u\n", len));
    }
  return(val32);
}

  void
fpu_put_user(unsigned val, void *ptr, unsigned len)
{
  Bit32u val32;
  Bit16u val16;
  Bit8u  val8;

  switch (len) {
    case 1:
      val8 = val;
      BX_CPU.write_virtual_byte(fpu_iptr->seg, PTR2INT(ptr), &val8);
      break;
    case 2:
      val16 = val;
      BX_CPU.write_virtual_word(fpu_iptr->seg, PTR2INT(ptr), &val16);
      break;
    case 4:
      val32 = val;
      BX_CPU.write_virtual_dword(fpu_iptr->seg, PTR2INT(ptr), &val32);
      break;
    default:
      BX_PANIC(("fpu_put_user: len=%u\n", len));
    }
}

  void
math_abort(struct info *info, unsigned int signal)
{
  UNUSED(info); // info is always passed NULL
#if BX_CPU_LEVEL >= 4

// values of signal:
//   SIGILL  : opcodes which are illegal
//   SIGFPE  : unmasked FP exception before WAIT or non-control instruction
//   SIGSEGV : access data beyond segment violation
  switch (signal) {
    case SIGFPE:
      if (BX_CPU.cr0.ne == 0) {
        // MSDOS compatibility external interrupt (IRQ13)
        BX_PANIC (("math_abort: MSDOS compatibility not supported yet\n"));
        }
      BX_CPU.exception(BX_MF_EXCEPTION, 0, 0);
      // execution does not reach here

    case SIGILL:
      BX_PANIC (("math_abort: SIGILL not implemented yet.\n"));
      break;
    case SIGSEGV:
      BX_PANIC (("math_abort: SIGSEGV not implemented yet.\n"));
      break;
    }

#else
  UNUSED(signal);
  BX_INFO("math_abort: CPU<4 not supported yet\n");
#endif
}

  int
printk(const char * fmt, ...)
{
  BX_INFO(("printk not complete: %s\n", fmt));
  return(0); // for now
}
