#include "bochs.h"
#include "extdb.h"
#ifdef WIN32
#  include <windows.h>
#else
#  error "extdb.cc only supported in win32 environment"
#endif

TRegs regs;

char debug_loaded = 0;

void (*call_debugger)(TRegs *,Bit8u *, Bit32u);

void bx_external_debugger(BX_CPU_C *cpu)
{
     switch (regs.debug_state) {
     case debug_run:
       return;
     case debug_count:
       if (--regs.debug_counter) return;
       regs.debug_state = debug_step;
       break;
     case debug_skip:
       if (cpu->eip != regs.debug_eip ||
           cpu->sregs[1].selector.value != regs.debug_cs) return;
       regs.debug_state = debug_step;
       break;
     }

     regs.eax = cpu->gen_reg[0].erx;
     regs.ecx = cpu->gen_reg[1].erx;
     regs.edx = cpu->gen_reg[2].erx;
     regs.ebx = cpu->gen_reg[3].erx;
     regs.esp = cpu->gen_reg[4].erx;
     regs.ebp = cpu->gen_reg[5].erx;
     regs.esi = cpu->gen_reg[6].erx;
     regs.edi = cpu->gen_reg[7].erx;
     regs.eip = cpu->eip;
     regs.eflags = cpu->read_eflags();
     regs.es = cpu->sregs[0].selector.value;
     regs.cs = cpu->sregs[1].selector.value;
     regs.ss = cpu->sregs[2].selector.value;
     regs.ds = cpu->sregs[3].selector.value;
     regs.fs = cpu->sregs[4].selector.value;
     regs.gs = cpu->sregs[5].selector.value;
     regs.gdt.base = cpu->gdtr.base;
     regs.gdt.limit = cpu->gdtr.limit;
     regs.idt.base = cpu->idtr.base;
     regs.idt.limit = cpu->idtr.limit;
     regs.ldt = cpu->ldtr.selector.value;
     regs.cr0 = cpu->cr0.val32;
     regs.cr1 = cpu->cr1;
     regs.cr2 = cpu->cr2;
     regs.cr3 = cpu->cr3;

     if (debug_loaded == 0) {
       HINSTANCE hdbg;

       debug_loaded = 1;
       hdbg = LoadLibrary("debug.dll");
       call_debugger = (void (*)(TRegs *,Bit8u *, Bit32u)) GetProcAddress(hdbg,"call_debugger");

       if (call_debugger != NULL) debug_loaded = 2;
     }
     if (debug_loaded == 2) call_debugger(&regs,cpu->mem->vector,cpu->mem->len);
}

