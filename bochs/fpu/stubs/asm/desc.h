#ifndef __ARCH_DESC_H
#define __ARCH_DESC_H

struct desc_struct {
	unsigned long a,b;
};

extern struct desc_struct gdt_table[];
extern struct desc_struct *idt, *gdt;

struct Xgt_desc_struct {
	unsigned short size;
	unsigned long address GCC_ATTRIBUTE((packed));
};

#define idt_descr (*(struct Xgt_desc_struct *)((char *)&idt - 2))
#define gdt_descr (*(struct Xgt_desc_struct *)((char *)&gdt - 2))

/*
 * Entry into gdt where to find first TSS. GDT layout:
 *   0 - null
 *   1 - not used
 *   2 - kernel code segment
 *   3 - kernel data segment
 *   4 - user code segment
 *   5 - user data segment
 *   6 - not used
 *   7 - not used
 *   8 - APM BIOS support
 *   9 - APM BIOS support
 *  10 - APM BIOS support
 *  11 - APM BIOS support
 *  12 - TSS #0
 *  13 - LDT #0
 *  14 - TSS #1
 *  15 - LDT #1
 */
#define FIRST_TSS_ENTRY 12
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define load_TR(n) __asm__ __volatile__("ltr %%ax": /* no output */ :"a" (_TSS(n)))
#define load_ldt(n) __asm__ __volatile__("lldt %%ax": /* no output */ :"a" (_LDT(n)))
#define store_TR(n) \
__asm__("str %%ax\n\t" \
	"subl %2,%%eax\n\t" \
	"shrl $4,%%eax" \
	:"=a" (n) \
	:"0" (0),"i" (FIRST_TSS_ENTRY<<3))

extern void set_intr_gate(unsigned int irq, void * addr);
extern void set_ldt_desc(unsigned int n, void *addr, unsigned int size);
extern void set_tss_desc(unsigned int n, void *addr);

/*
 * This is the ldt that every process will get unless we need
 * something other than this.
 */
extern struct desc_struct default_ldt;

#endif
