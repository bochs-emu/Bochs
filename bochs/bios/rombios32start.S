.globl _start
.globl smp_ap_boot_code_start
.globl smp_ap_boot_code_end
.global smm_relocation_start
.global smm_relocation_end
.global smm_code_start
.global smm_code_end

#define PM_IO_BASE        0xb000

_start:
  /* clear bss section */
  xor %eax, %eax
  mov $__bss_start, %edi
  mov $_end, %ecx
  sub %edi, %ecx
  rep stosb

  jmp rombios32_init

#define CPU_COUNT 0xf000

  .code16                        
smp_ap_boot_code_start:
  xor %ax, %ax
  mov %ax, %ds
  incw CPU_COUNT
1:
  hlt
  jmp 1b
smp_ap_boot_code_end:

/* code to relocate SMBASE to 0xa0000 */
smm_relocation_start:
  mov $0x38000 + 0x7efc, %ebx
  mov (%ebx), %al  /* revision ID to see if x86_64 or x86 */
  cmp $0x64, %al
  je 1f
  mov $0x38000 + 0x7ef8, %ebx
  jmp 2f
1:
  mov $0x38000 + 0x7f00, %ebx
2:
  movl $0xa0000, %eax
  movl %eax, (%ebx)
  rsm                
smm_relocation_end:
        
/* minimal SMM code to enable or disable ACPI */
smm_code_start:
  movw $0xb2, %dx
  inb %dx, %al
  cmp $0xf0, %al
  jne 1f

  /* ACPI disable */
  mov $PM_IO_BASE + 0x04, %dx /* PMCNTRL */
  inw %dx, %ax
  andw $~1, %ax
  outw %ax, %dx

  jmp 2f

1:
  cmp $0xf1, %al
  jne 2f

  /* ACPI enable */
  mov $PM_IO_BASE + 0x04, %dx /* PMCNTRL */
  inw %dx, %ax
  orw $1, %ax
  outw %ax, %dx
  
2:
  rsm
smm_code_end:
