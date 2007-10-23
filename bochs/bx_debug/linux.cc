/////////////////////////////////////////////////////////////////////////
// $Id: linux.cc,v 1.8 2007-10-23 21:51:43 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
#include <stdio.h>

#include "bochs.h"
#include "cpu/cpu.h"

#if BX_DEBUGGER

#define LOG_THIS genlog->

// for Linux segment numbers
// these numbers are from <asm/segment.h>
#define KERNEL_CS 0x10
#define USER_CS 0x18

void bx_dbg_info_linux_command (void)
{
  BX_INFO (("Info linux"));
  bx_dbg_sreg_t cs;
  Bit32u cr0 = BX_CPU(dbg_cpu)->dbg_get_reg(BX_DBG_REG_CR0);
  BX_CPU(dbg_cpu)->dbg_get_sreg(&cs, BX_DBG_SREG_CS);

  int mode;
  if (cr0 & 1) {
    // protected mode
    if (cs.sel == KERNEL_CS) {
      mode = 'k';
      fprintf (stderr, "Processor mode: kernel\n");
    } else if (cs.sel == USER_CS) {
      fprintf (stderr, "Processor mode: user\n");
      mode = 'u';
    } else {
      mode = '?';
      fprintf (stderr, "Processor mode: ??? protected=1 but unrecognized CS\n");
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
  char *syscall_names_linux[MAX_SYSCALLS];
public:
  syscall_names_t () { init (); }
  void init ();
  char *get_name (int num);
};

void syscall_names_t::init ()
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

char *syscall_names_t::get_name (int n) 
{
  static char buf[64];
  if (n < 0 || n > N_SYSCALLS) {
    sprintf (buf, "syscall %d out of range", n);
    return buf;
  }
  return syscall_names_linux[n];
}

syscall_names_t syscall_names;

void bx_dbg_linux_syscall (unsigned which_cpu)
{
  Bit32u eax = BX_CPU(which_cpu)->get_reg32(BX_32BIT_REG_EAX);
  char *name = syscall_names.get_name (eax);
  fprintf (stderr, "linux system call %s (#%d)\n", name, eax);
}

#endif /* if BX_DEBUGGER */
