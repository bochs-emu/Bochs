#include "bochs.h"

typedef struct {
	Bit32u base;
	Bit16u limit;
	Bit16u fill;
	} descbase;

typedef struct {
        Bit32u eax,ecx,edx,ebx,esp,ebp,esi,edi,eip,eflags;
        Bit32u es,cs,ss,ds,fs,gs,tr;
        descbase gdt,idt;
        Bit32u ldt;
        Bit32u cr0,cr1,cr2,cr3;
        Bit32u debug_eip,debug_cs,debug_counter;
        Bit8u debug_state;
#define debug_step        0
#define debug_run         1
#define debug_count       2
#define debug_skip        3
        } TRegs;

extern TRegs regs;

extern char debug_loaded;

void bx_external_debugger(BX_CPU_C *cpu);

