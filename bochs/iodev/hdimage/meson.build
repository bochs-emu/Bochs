sources = [
    'cdrom.cc',
    'hdimage.cc',
    'vbox.cc',
    'vmware3.cc',
    'vmware4.cc',
    'vpc.cc',
    'vvfat.cc',
]

if host_machine.system() == 'amiga'
    sources += 'cdrom_amigaos.cc'
elif host_machine.system() == 'windows'
    sources += 'cdrom_win32.cc'
elif host_machine.system() == 'darwin'
    sources += 'cdrom_osx.cc'
else
    sources += 'cdrom_misc.cc'
endif

bx_deps += static_library('hdimage', sources, include_directories: [top_include, '..'])
