OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start);
SECTIONS
{
        . = 0x000e0000;
        .text     : { *(.text)    }
        .rodata    : { *(.rodata*) }
        _end = . ;
        .data 0x700 : AT (_end) { __data_start = .; *(.data); __data_end = .;}
        .bss      : { __bss_start = .; *(.bss) *(COMMON); __bss_end = .;}
        /DISCARD/ : { *(.stab)
                     *(.stabstr)
                     *(.comment)
                     *(.eh_frame)
                     *(.note)
                  }
}
