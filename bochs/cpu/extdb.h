#if BX_SUPPORT_X86_64
// I have no idea why one includes, one doesn't.  Fixme.
#else
#include "bochs.h"
#endif

typedef struct {
#if BX_SUPPORT_X86_64
	Bit64u base;
	Bit16u limit;
	Bit16u fill[3];
#else
	Bit32u base;
	Bit16u limit;
	Bit16u fill;
#endif
	} descbase;

typedef struct {
#if BX_SUPPORT_X86_64
        Bit64u rax,rcx,rdx,rbx,rsp,rbp,rsi,rdi,
               r8,r9,r10,r11,r12,r13,r14,r15,
               rip,rflags;
        Bit32u es,cs,ss,ds,fs,gs,tr,ldt;
        Bit64u cr0,cr1,cr2,cr3,cr4,cr5,cr6,cr7;
        descbase gdt,idt;
        Bit32u efer;
        Bit32u debug_eip,debug_cs,debug_counter;
        Bit8u debug_state;
#else
        Bit32u eax,ecx,edx,ebx,esp,ebp,esi,edi,eip,eflags;
        Bit32u es,cs,ss,ds,fs,gs,tr;
        descbase gdt,idt;
        Bit32u ldt;
        Bit32u cr0,cr1,cr2,cr3;
        Bit32u debug_eip,debug_cs,debug_counter;
        Bit8u debug_state;
#endif
#define debug_step        0
#define debug_run         1
#define debug_count       2
#define debug_skip        3
        } TRegs;

extern TRegs regs;

extern char debug_loaded;

void bx_external_debugger(BX_CPU_C *cpu);

