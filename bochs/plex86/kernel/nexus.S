/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2001  Kevin P. Lawton
 *
 *  nexus.S: code to transition between host and monitor/guest
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */



.text


/* This module consists of relocatable code and data necessary to
 * effect transitions between the host <--> guest.  This information
 * is purposely stored in this single page, so that we have access
 * to it during our transitions between the monitor interrupt handler,
 * and our host.
 *
 * I coded the relevant parts to use completely relocatable
 * accesses to the following fields.  This is necessary, so that
 * we can float this code page anywhere in the monitor's linear
 * address space.
 */

/* ===============================================================
 * NOTE: If you modify ANY of the following fields, you must also
 * update the corresponding entries in the C typedef 'nexus_t'.
 * That construct is used from C land to access values in this
 * relocatable page.
 */

.globl __nexus_start
__nexus_start:

__vm:              ;.skip 4, 0

__host_gdt_info:   ;.skip 6, 0
__host_idt_info:   ;.skip 6, 0
__host_jmp_info:   ;.skip 6, 0
__host_stack_info: ;.skip 6, 0
__host_ldt_sel:    ;.skip 2, 0
__host_tss_sel:    ;.skip 2, 0
__host_cr0:        ;.skip 4, 0
__host_cr2:        ;.skip 4, 0
__host_cr3:        ;.skip 4, 0
__host_cr4:        ;.skip 4, 0

__mon_gdt_info:    ;.skip 6, 0
__mon_idt_info:    ;.skip 6, 0
__mon_jmp_info:    ;.skip 6, 0
__mon_stack_info:  ;.skip 6, 0
__mon_ldt_sel:     ;.skip 2, 0
__mon_tss_sel:     ;.skip 2, 0
__mon_base:        ;.skip 4, 0
__mon_cr0:         ;.skip 4, 0
__mon_cr3:         ;.skip 4, 0
__mon_cr4:         ;.skip 4, 0
__mon_eflags:      ;.skip 4, 0

__transition_pde:        ;.skip 4, 0
__transition_pde_p_host: ;.skip 4, 0
__transition_pde_p_mon:  ;.skip 4, 0
__transition_laddr:      ;.skip 4, 0

/* ===============================================================
 * End NOTE.
 */

#define OFFSET_OF(field)  [field - __nexus_start]

/* These are the offsets of the structures above, from the */
/* beginning of this section. */
#define HOST_GDT_INFO       OFFSET_OF(__host_gdt_info)
#define HOST_IDT_INFO       OFFSET_OF(__host_idt_info)
#define HOST_JMP_INFO       OFFSET_OF(__host_jmp_info)
#define HOST_STACK_INFO     OFFSET_OF(__host_stack_info)
#define HOST_LDT_SEL        OFFSET_OF(__host_ldt_sel)
#define HOST_TSS_SEL        OFFSET_OF(__host_tss_sel)
#define HOST_CR0            OFFSET_OF(__host_cr0)
#define HOST_CR2            OFFSET_OF(__host_cr2)
#define HOST_CR3            OFFSET_OF(__host_cr3)
#define HOST_CR4            OFFSET_OF(__host_cr4)

#define MON_GDT_INFO        OFFSET_OF(__mon_gdt_info)
#define MON_IDT_INFO        OFFSET_OF(__mon_idt_info)
#define MON_JMP_INFO        OFFSET_OF(__mon_jmp_info)
#define MON_STACK_INFO      OFFSET_OF(__mon_stack_info)
#define MON_LDT_SEL         OFFSET_OF(__mon_ldt_sel)
#define MON_TSS_SEL         OFFSET_OF(__mon_tss_sel)
#define MON_CR0             OFFSET_OF(__mon_cr0)
#define MON_CR3             OFFSET_OF(__mon_cr3)
#define MON_CR4             OFFSET_OF(__mon_cr4)
#define MON_BASE            OFFSET_OF(__mon_base)

#define TRANSITION_PDE        OFFSET_OF(__transition_pde)
#define TRANSITION_PDE_P_HOST OFFSET_OF(__transition_pde_p_host)
#define TRANSITION_PDE_P_MON  OFFSET_OF(__transition_pde_p_mon)
#define TRANSITION_LADDR      OFFSET_OF(__transition_laddr)


/* To make this code page and data accesses to the fields above */
/* relocatable, I use the following conventions.  I load EBX with */
/* a pointer to the beginning of this page, to be used with an */
/* access through the CS: segment.  We can easily get the */
/* current EIP with a call/pop EBX, so the combination of CS:EBX, */
/* accesses this page no matter where it is located. */


/* ================================================================== */
.globl __host2mon      /* Start function __host2mon() */
__host2mon:
  /* Save host context first, so it can be restored later */
  pushfl               /* Save host flags */
  pushal               /* Save host general regs */
  pushl %es            /* Save host segments */
  pushl %ds
  pushl %fs
  pushl %gs

  /* Put EIP of beginning of this section in EBX to be used to */
  /* access data. */
  call null_call
null_call:
  popl %ebx
  subl $(OFFSET_OF(null_call)), %ebx

  /* Create identity mapping of this page into the monitor context */
  movl  (TRANSITION_PDE_P_HOST)(%ebx), %eax
  movl  (TRANSITION_PDE)(%ebx), %ebp
  xchgl %ebp, (%eax)  /* old PDE saved in %ebp to be restored below */

  /* Save host GDT, LDT, IDT, and TSS */
  sgdt  (HOST_GDT_INFO)(%ebx)
  sidt  (HOST_IDT_INFO)(%ebx)
  sldt  (HOST_LDT_SEL)(%ebx)
  str   (HOST_TSS_SEL)(%ebx)
  
  movl  %esp, (HOST_STACK_INFO)(%ebx)    /* Save host SS:ESP */
  movw  %ss,  (4+HOST_STACK_INFO)(%ebx)  /* for later restore */

  leal  (OFFSET_OF(__host_cs))(%ebx), %eax  /* Save the CS:EIP for monitor to */
  movl  %eax, (HOST_JMP_INFO)(%ebx)         /* jump to when reloading host CS. */
  movw  %cs,  (4+HOST_JMP_INFO)(%ebx)       /* See __guest2host below. */

  /* Save host CRx values */
  movl  %cr0, %eax
  movl  %cr2, %ecx
  movl  %cr4, %edx
  movl  %cr3, %esi

  movl  %eax, (HOST_CR0)(%ebx)
  movl  %ecx, (HOST_CR2)(%ebx)
  movl  %edx, (HOST_CR4)(%ebx)
  movl  %esi, (HOST_CR3)(%ebx)

  /* Compute monitor CRx values */
  movl (MON_CR0)(%ebx), %eax
  movl (MON_CR4)(%ebx), %edx
  movl (MON_CR3)(%ebx), %esi

  /* Before changing the PSE bit in CR4, we have to switch over */
  /* to the new CR3 (this page identity mapped anyways).  Otherwise */
  /* the processor could flush the TLB, and reload the entry for */
  /* this page, only to find it's marked with a 4Meg Page, but we */
  /* have that support turned off,  before we actually */
  /* reloaded CR3! */
  movl %esi, %cr3  /* Set monitor CR3 */
  movl %eax, %cr0  /* Set monitor CR0 */
  movl %edx, %cr4  /* Set monitor CR4 */
  movl %esi, %cr3  /* Set monitor CR3 */

jmp null_jmp0
null_jmp0:

  /* Switch to monitor GDT, LDT, and IDT */
  lgdt  (MON_GDT_INFO)(%ebx)
  lidt  (MON_IDT_INFO)(%ebx)
  lldt  (MON_LDT_SEL)(%ebx)

  /* Switch to monitor stack and CS */
  /* and jump to the monitor-side nexus page */
  lss   (MON_STACK_INFO)(%ebx), %esp
  ljmp  (MON_JMP_INFO)(%ebx)
.globl __mon_cs
__mon_cs:

  /* Reset DS:EBX to point to the monitor-side nexus page */
  movw  %ss, %ax
  movw  %ax, %ds /* copy SS to DS */
  movw  %ax, %es /* copy SS to ES */
  movl  %esp, %ebx
  andl  $0xfffff000, %ebx

  /* Clear busy bit of the monitor TSS and switch to it */
  movzwl (MON_TSS_SEL)(%ebx), %eax
  andl   $0xfffffff8, %eax
  addl   (MON_GDT_INFO+2)(%ebx), %eax
  subl   (MON_BASE)(%ebx), %eax
  andl   $0xfffffdff, 4(%eax)
  ltr    (MON_TSS_SEL)(%ebx)

  /* We no longer need the nexus page identity mapped.  Fix the mapping */
  /* back to the way it should be, in case guest code uses it. */
  movl   (TRANSITION_PDE_P_MON)(%ebx), %eax
  movl   %ebp, (%eax)  /* %ebp still contains the original value */
  movl   (TRANSITION_LADDR)(%ebx), %eax
  invlpg (%eax) /* Tell TLB about the change */
/* +++ xxx fix this, need to convert pure laddr to offset */
movl %cr3, %eax /* +++ xxx */
movl %eax, %cr3 /* +++ xxx */

  /* */
  /* We can now restore the monitor context from it's stack. */
  /* */
  popl %gs
  popl %fs
  popal          /* Restore mon general registers */
  popfl          /* Restore mon eflags */
  ret /* Resume execution in monitor exception handler code. */



/* ================================================================== */
.globl __mon2host    /* Start function __mon2host() */
__mon2host:
  pushfl             /* Save mon flags */
  pushal             /* Save mon general registers */
  pushl %fs
  pushl %gs

  /* Set EBX to point to this nexus page */
  movl  %esp, %ebx
  andl  $0xfffff000, %ebx

  movl  %esp, (MON_STACK_INFO)(%ebx)    /* Save mon ESP */

  /* Identity map this code page to host address space. */
  movl   (TRANSITION_PDE_P_MON)(%ebx), %eax
  movl   (TRANSITION_PDE)(%ebx), %ebp
  xchgl  %ebp, (%eax)  /* old PDE saved in %ebp to be restored below */
  movl   (TRANSITION_LADDR)(%ebx), %eax

  /* Switch EBX to point to the identity mapped copy of  */
  /* the nexus page, and jump to the copy of this code there. */
  subl (MON_BASE)(%ebx), %eax
  invlpg (%eax)        /* Tell TLB about the change */
  movl %eax, %ebx
  leal (OFFSET_OF(__mon_nexus_jmp))(%ebx), %eax
  jmp  *%eax
__mon_nexus_jmp:

  /* We are still in the monitor context, but are running at the */
  /* same CS.base+EIP location in either host or monitor context, */
  /* and this page is identity mapped between the 2 contexts. */
  /* We can now switch to the host CR3, and be sure that execution */
  /* will resume at the next instruction. */

  /* NOTE: Don't try to access the stack after CR3 was reloaded */
  /*       but before we switched back to the host stack! */

  /* Restore host CRx values */
  movl  (HOST_CR0)(%ebx), %eax
  movl  (HOST_CR2)(%ebx), %ecx
  movl  (HOST_CR4)(%ebx), %edx
  movl  (HOST_CR3)(%ebx), %esi

  movl  %eax, %cr0
  movl  %ecx, %cr2
  movl  %edx, %cr4
  movl  %esi, %cr3

jmp null_jmp1
null_jmp1:

  /* Switch to host GDT, LDT, and IDT */
  lgdt   (HOST_GDT_INFO)(%ebx)
  lidt   (HOST_IDT_INFO)(%ebx)
  lldt   (HOST_LDT_SEL)(%ebx)

  /* Restore host stack and CS */
  lss    (HOST_STACK_INFO)(%ebx), %esp
  ljmp   (HOST_JMP_INFO)(%ebx)
__host_cs:

  /* Clear busy bit of the host TSS and switch to it */
  /* Note that DS is still the monitor segment with base (MON_BASE). */
  movzwl (HOST_TSS_SEL)(%ebx), %eax
  andl   $0xfffffff8, %eax
  addl   (HOST_GDT_INFO+2)(%ebx), %eax
  subl   (MON_BASE)(%ebx), %eax
  andl   $0xfffffdff, 4(%eax)
  ltr    (HOST_TSS_SEL)(%ebx)

  /* We no longer need the nexus page identity mapped, so we clean */
  /* up the monitor page directory in case the host looks at it. */
  /* Note that SS is already the host segment. */
  movl   (TRANSITION_PDE_P_HOST)(%ebx), %eax
  ss; movl %ebp, (%eax)  /* %ebp still contains the original value */

  /* Now we can restore the rest of */
  /* the host context from the host stack.  Look at __host2guest */
  /* for the format of the values stored on the host stack. */
  popl %gs
  popl %fs
  popl %ds
  popl %es
  popal
  popfl
  ret 

.globl __nexus_end
__nexus_end:
