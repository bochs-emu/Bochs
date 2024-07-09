subdir('display')
subdir('hdimage')
subdir('network')
subdir('sound')
subdir('usb')

sources = [
    'acpi.cc',
    'biosdev.cc',
    'busmouse.cc',
    'cmos.cc',
    'devices.cc',
    'dma.cc',
    'extfpuirq.cc',
    'floppy.cc',
    'harddrv.cc',
    'hpet.cc',
    'ioapic.cc',
    'iodebug.cc',
    'keyboard.cc',
    'parallel.cc',
    'pci2isa.cc',
    'pci.cc',
    'pcidev.cc',
    'pci_ide.cc',
    'pic.cc',
    'pit82c54.cc',
    'pit.cc',
    'scancodes.cc',
    'serial.cc',
    'serial_raw.cc',
    'slowdown_timer.cc',
    'speaker.cc',
    'unmapped.cc',
    'virt_timer.cc'
]

if get_option('support_gameport')
    sources += 'gameport.cc'
endif

bx_deps += static_library('iodev', sources, include_directories: [top_include])
