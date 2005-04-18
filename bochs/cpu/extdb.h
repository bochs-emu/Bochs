#ifndef EXTDB_H
#define EXTDB_H

typedef struct {
  Bit64u base;
  Bit16u limit;
  Bit16u fill[3];
} descbase;

typedef struct {
        Bit64u rax,rcx,rdx,rbx,rsp,rbp,rsi,rdi,
               r8,r9,r10,r11,r12,r13,r14,r15,
               rip,rflags;
        Bit32u es,cs,ss,ds,fs,gs,tr,ldt;
        Bit64u cr0,cr1,cr2,cr3,cr4,cr5,cr6,cr7;
        Bit64u fsbase,gsbase;
        descbase gdt,idt;
        Bit32u efer;
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

#endif
