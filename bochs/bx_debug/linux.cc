/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
#include <stdio.h>

#include "bochs.h"
#include "debug.h"

#include "cpu/cpu.h"

#if BX_DEBUGGER

#define LOG_THIS genlog->

// Linux segment selectors from <asm/segment.h>, for reference:
//
//   kernel                     KERNEL_CS  KERNEL_DS  USER_CS  USER_DS
//   i386   2.0 - 2.4           0x10       0x18       0x23     0x2B
//   i386   2.6 and later       0x60       0x68       0x73     0x7B
//   x86-64 (all versions)      0x10       0x18       0x33     0x2B
//                                                    0x23 (32-bit compat)
//
// The selectors differ between kernel versions, so the processor mode is
// decided by the privilege level (CPL is the RPL of CS in protected mode):
// the kernel runs at CPL 0 and user processes at CPL 3.
#define KERNEL_CPL 0
#define USER_CPL   3

void bx_dbg_info_linux_command(void)
{
  BX_INFO (("Info linux"));
  bx_dbg_sreg_t cs;
  BX_CPU(dbg_cpu)->dbg_get_sreg(&cs, BX_SEG_REG_CS);
  int cpu_mode = BX_CPU(dbg_cpu)->get_cpu_mode();

  int mode;
  if (cpu_mode >= BX_MODE_IA32_PROTECTED) {
    // protected mode
    if ((cs.sel & 3) == KERNEL_CPL) {
      mode = 'k';
      fprintf (stderr, "Processor mode: kernel\n");
    } else if ((cs.sel & 3) == USER_CPL) {
      fprintf (stderr, "Processor mode: user\n");
      mode = 'u';
    } else {
      mode = '?';
      fprintf (stderr, "Processor mode: ??? protected=1 but CPL is not 0 or 3\n");
    }
  } else {
    mode = 'r';
    fprintf (stderr, "Processor mode: real-address mode, maybe during boot sequence\n");
  }
  if (mode != 'u') return;
  /* user mode, look through registers and memory to find our process ID */
}

class syscall_names_t {
#define MAX_SYSCALLS 200
  const char *syscall_names_linux[MAX_SYSCALLS];
public:
  syscall_names_t() { init (); }
  void init();
  const char *get_name(int num);
};

void syscall_names_t::init()
{
  for (int i=0; i<MAX_SYSCALLS; i++) {
    syscall_names_linux[i] = "<unknown syscall>";
  }
#define DEF_SYSCALL(num,name)  syscall_names_linux[num] = name;
  /* basically every line in the included file is a call to DEF_SYSCALL.
     The preprocessor will turn each DEF_SYSCALL into an assignment
     to syscall_names_linux[num]. */
#include "syscalls-linux.h"
  /* now almost all the name entries have been initialized.  If there
     are any gaps, they still point to "<unknown syscall>". */

#if (N_SYSCALLS > MAX_SYSCALLS)
#error MAX_SYSCALLS must exceed N_SYSCALLS from syscalls-linux.h
#endif
}

const char *syscall_names_t::get_name(int n)
{
  if (n < 0 || n > N_SYSCALLS) return 0;
  return syscall_names_linux[n];
}

syscall_names_t syscall_names;

void bx_dbg_linux_syscall(unsigned which_cpu)
{
  Bit32u eax = BX_CPU(which_cpu)->get_reg32(BX_32BIT_REG_EAX);
  const char *name = syscall_names.get_name(eax);
  if (name)
    fprintf (stderr, "linux system call %s (#%d)\n", name, eax);
  else
    fprintf (stderr, "system call (#%d) is out of range\n", eax);
}

#endif /* if BX_DEBUGGER */
