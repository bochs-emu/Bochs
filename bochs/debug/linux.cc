#include <stdio.h>
#include "bochs.h"

// for Linux segment numbers
// these numbers are from <asm/segment.h>
#define KERNEL_CS 0x10
#define USER_CS 0x18

  void
bx_dbg_info_linux_command (void)
{
  bx_printf ("Info linux\n");
  bx_dbg_cpu_t cpu;
  bx_dbg_callback[0].get_cpu(&cpu);

  int mode;
  if (cpu.cr0 & 1) {
    // protected mode
    if (cpu.cs.sel == KERNEL_CS) {
      mode = 'k';
      fprintf (stderr, "Processor mode: kernel\n");
    } else if (cpu.cs.sel == USER_CS) {
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

#if 0
  fprintf(stderr, "eax:0x%x\n", (unsigned) cpu.eax);
  fprintf(stderr, "ebx:0x%x\n", (unsigned) cpu.ebx);
  fprintf(stderr, "ecx:0x%x\n", (unsigned) cpu.ecx);
  fprintf(stderr, "edx:0x%x\n", (unsigned) cpu.edx);
  fprintf(stderr, "ebp:0x%x\n", (unsigned) cpu.ebp);
  fprintf(stderr, "esi:0x%x\n", (unsigned) cpu.esi);
  fprintf(stderr, "edi:0x%x\n", (unsigned) cpu.edi);
  fprintf(stderr, "esp:0x%x\n", (unsigned) cpu.esp);
  fprintf(stderr, "eflags:0x%x\n", (unsigned) cpu.eflags);
  fprintf(stderr, "eip:0x%x\n", (unsigned) cpu.eip);

  fprintf(stderr, "cs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.cs.sel, (unsigned) cpu.cs.des_l,
    (unsigned) cpu.cs.des_h, (unsigned) cpu.cs.valid);

  fprintf(stderr, "ss:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.ss.sel, (unsigned) cpu.ss.des_l,
    (unsigned) cpu.ss.des_h, (unsigned) cpu.ss.valid);

  fprintf(stderr, "ds:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.ds.sel, (unsigned) cpu.ds.des_l,
    (unsigned) cpu.ds.des_h, (unsigned) cpu.ds.valid);

  fprintf(stderr, "es:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.es.sel, (unsigned) cpu.es.des_l,
    (unsigned) cpu.es.des_h, (unsigned) cpu.es.valid);

  fprintf(stderr, "fs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.fs.sel, (unsigned) cpu.fs.des_l,
    (unsigned) cpu.fs.des_h, (unsigned) cpu.fs.valid);

  fprintf(stderr, "gs:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.gs.sel, (unsigned) cpu.gs.des_l,
    (unsigned) cpu.gs.des_h, (unsigned) cpu.gs.valid);

  fprintf(stderr, "ldtr:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.ldtr.sel, (unsigned) cpu.ldtr.des_l,
    (unsigned) cpu.ldtr.des_h, (unsigned) cpu.ldtr.valid);

  fprintf(stderr, "tr:s=0x%x, dl=0x%x, dh=0x%x, valid=%u\n",
    (unsigned) cpu.tr.sel, (unsigned) cpu.tr.des_l,
    (unsigned) cpu.tr.des_h, (unsigned) cpu.tr.valid);

  fprintf(stderr, "gdtr:base=0x%x, limit=0x%x\n",
    (unsigned) cpu.gdtr.base, (unsigned) cpu.gdtr.limit);

  fprintf(stderr, "idtr:base=0x%x, limit=0x%x\n",
    (unsigned) cpu.idtr.base, (unsigned) cpu.idtr.limit);

  fprintf(stderr, "dr0:0x%x\n", (unsigned) cpu.dr0);
  fprintf(stderr, "dr1:0x%x\n", (unsigned) cpu.dr1);
  fprintf(stderr, "dr2:0x%x\n", (unsigned) cpu.dr2);
  fprintf(stderr, "dr3:0x%x\n", (unsigned) cpu.dr3);
  fprintf(stderr, "dr6:0x%x\n", (unsigned) cpu.dr6);
  fprintf(stderr, "dr7:0x%x\n", (unsigned) cpu.dr7);

  fprintf(stderr, "tr3:0x%x\n", (unsigned) cpu.tr3);
  fprintf(stderr, "tr4:0x%x\n", (unsigned) cpu.tr4);
  fprintf(stderr, "tr5:0x%x\n", (unsigned) cpu.tr5);
  fprintf(stderr, "tr6:0x%x\n", (unsigned) cpu.tr6);
  fprintf(stderr, "tr7:0x%x\n", (unsigned) cpu.tr7);

  fprintf(stderr, "cr0:0x%x\n", (unsigned) cpu.cr0);
  fprintf(stderr, "cr1:0x%x\n", (unsigned) cpu.cr1);
  fprintf(stderr, "cr2:0x%x\n", (unsigned) cpu.cr2);
  fprintf(stderr, "cr3:0x%x\n", (unsigned) cpu.cr3);
  fprintf(stderr, "cr4:0x%x\n", (unsigned) cpu.cr4);

  fprintf(stderr, "inhibit_mask:%u\n", cpu.inhibit_mask);
#endif

}

class syscall_names_t {
#define MAX_SYSCALLS 200
  char *syscall_names_linux[MAX_SYSCALLS];
public:
  syscall_names_t () { init (); }
  void init ();
  char *get_name (int num);
};

void
syscall_names_t::init ()
{
  for (int i; i<MAX_SYSCALLS; i++) {
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

char *
syscall_names_t::get_name (int n) 
{
  static char buf[64];
  if (n < 0 || n > N_SYSCALLS) {
    sprintf (buf, "syscall %d out of range", n);
    return buf;
  }
  return syscall_names_linux[n];
}

syscall_names_t syscall_names;

void bx_dbg_linux_syscall () {
  bx_dbg_cpu_t cpu;
  bx_dbg_callback[0].get_cpu(&cpu);
  char *name = syscall_names.get_name (cpu.eax);
  fprintf (stderr, "linux system call %s (#%d)\n", name, cpu.eax);
}
