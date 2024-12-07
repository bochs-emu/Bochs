project('bochs', ['c', 'cpp'],
        meson_version: '>=1.1.0',
        default_options : ['cpp_std=c++11'],
        version: '2.7.1')

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

x11_dep         = dependency('x11',          required : get_option('with_x11'))
xrandr_dep      = dependency('xrandr',       required : get_option('with_x11'))
xpm_dep         = dependency('xpm',          required : get_option('with_x11'))
sdl1_dep        = dependency('sdl',          required : get_option('with_sdl1'))
sdl2_dep        = dependency('sdl2',         required : get_option('with_sdl2'))
term_dep        = dependency('curses',       required : get_option('with_term'))
vncsrv_dep      = dependency('libvncserver', required : get_option('with_vncsrv'))
wx_dep          = dependency('wxwidgets',    required : get_option('with_wx'))
readline_dep    = dependency('readline',     required: false)
alsa_dep        = dependency('alsa',         required : false)
oss_dep         = dependency('oss',          required : false)
dl_dep          = dependency('dl',           required : get_option('plugins'))

conf_data = configuration_data()

conf_data.set('BX_CPU_LEVEL', get_option('cpu_level'))
conf_data.set('BX_CPUID_VENDOR_LEN', get_option('cpuid_vendor_len'))
conf_data.set('BX_CPUID_BRAND_LEN', get_option('cpuid_brand_len'))
conf_data.set10('BX_SUPPORT_SMP', get_option('support_smp'))
conf_data.set10('BX_BOOTSTRAP_PROCESSOR', get_option('bootstrap_processor'))
conf_data.set10('BX_MAX_SMP_THREADS_SUPPORTED', get_option('max_smp_threads_supported'))

conf_data.set10('BX_USE_GUI_CONSOLE', x11_dep.found() or sdl1_dep.found() or sdl2_dep.found() or (host_machine.system() == 'windows') or (host_machine.system() == 'darwin') or (host_machine.system() == 'amiga'))
conf_data.set10('BX_USE_TEXTCONFIG', term_dep.found())

conf_data.set10('BX_WITH_RFB', false) # FIX ME
conf_data.set10('BX_WITH_CARBON', false) # FIX ME
conf_data.set10('WX_MSW_UNICODE', 0) # FIX ME
conf_data.set10('WX_GDK_VERSION', 0) # FIX ME
if wx_dep.found()
    conf_data.set('WX_GDK_VERSION', 3) # FIX ME
endif

conf_data.set10('BX_WITH_MACOS', host_machine.system() == 'darwin')
conf_data.set10('BX_WITH_WIN32', host_machine.system() == 'windows')
conf_data.set10('BX_WITH_NOGUI', get_option('with_nogui').enabled())
conf_data.set10('BX_WITH_TERM', term_dep.found())
conf_data.set10('BX_WITH_VNCSRV', vncsrv_dep.found())
conf_data.set10('BX_WITH_AMIGAOS', host_machine.system() == 'amiga')
conf_data.set10('BX_WITH_X11',  x11_dep.found())
conf_data.set10('BX_WITH_SDL',  sdl1_dep.found())
conf_data.set10('BX_WITH_SDL2', sdl2_dep.found())
conf_data.set10('BX_WITH_WX',   wx_dep.found())

conf_data.set_quoted('BX_DEFAULT_CONFIG_INTERFACE', get_option('default_config_interface'))
conf_data.set_quoted('BX_DEFAULT_DISPLAY_LIBRARY', get_option('default_display_library'))
conf_data.set10('WORDS_BIGENDIAN', host_machine.endian() == 'big')

conf_data.set('SIZEOF_UNSIGNED_CHAR', cc.sizeof('unsigned char'))
conf_data.set('SIZEOF_UNSIGNED_SHORT', cc.sizeof('unsigned short'))
conf_data.set('SIZEOF_UNSIGNED_INT', cc.sizeof('unsigned int'))
conf_data.set('SIZEOF_UNSIGNED_LONG', cc.sizeof('unsigned long'))
conf_data.set('SIZEOF_UNSIGNED_LONG_LONG', cc.sizeof('unsigned long long'))
conf_data.set('SIZEOF_INT_P', cc.sizeof('int *'))

if readline_dep.found()
    conf_data.set10('HAVE_LIBREADLINE', 1)
    conf_data.set10('HAVE_READLINE_HISTORY_H',  cc.has_header('readline/history.h', dependencies: [readline_dep], required: false))
endif
conf_data.set10('HAVE_LOCALE_H', cc.has_header('locale.h', required: false))
conf_data.set10('BX_HAVE_SLEEP', cc.has_function('sleep', prefix : '#include <unistd.h>'))
conf_data.set10('BX_HAVE_MSLEEP', cc.has_function('msleep', prefix : '#include <sys/param.h>\n#include <sys/systm.h>\n#include <sys/proc.h>'))
conf_data.set10('BX_HAVE_USLEEP', cc.has_function('usleep', prefix : '#include <unistd.h>'))
conf_data.set10('BX_HAVE_NANOSLEEP', cc.has_function('nanosleep', prefix : '#include <time.h>'))
conf_data.set10('BX_HAVE_ABORT', cc.has_function('abort', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_SOCKLEN_T', cc.has_type('socklen_t', prefix : '#include <sys/types.h>\n#include <sys/socket.h>'))
conf_data.set10('BX_HAVE_SOCKADDR_IN_SIN_LEN', cc.has_member('sockaddr_in', 'sin_len', prefix : '#include <sys/socket.h>\n#include <netinet/in.h>'))
conf_data.set10('BX_HAVE_GETTIMEOFDAY', cc.has_function('gettimeofday', prefix : '#include <sys/time.h>'))
conf_data.set10('BX_HAVE_MKSTEMP', cc.has_function('mkstemp', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_SYS_MMAN_H',  cc.has_header('sys/mman.h'))
conf_data.set10('BX_HAVE_XPM_H',  xpm_dep.found())
conf_data.set10('BX_HAVE_XRANDR_H',  xrandr_dep.found())
conf_data.set10('BX_HAVE_TIMELOCAL', cc.has_function('timelocal', prefix : '#include <time.h>'))
conf_data.set10('BX_HAVE_GMTIME', cc.has_function('gmtime', prefix : '#include <time.h>'))
conf_data.set10('BX_HAVE_MKTIME', cc.has_function('mktime', prefix : '#include <time.h>'))
conf_data.set10('BX_HAVE_TMPFILE64', cc.has_function('tmpfile64', prefix : '#include <stdio.h>'))
conf_data.set10('BX_HAVE_FSEEK64', cc.has_function('fseek64', prefix : '#include <stdio.h>'))
conf_data.set10('BX_HAVE_FSEEKO64', cc.has_function('fseeko64', prefix : '#include <stdio.h>'))
conf_data.set10('BX_HAVE_NET_IF_H',  cc.has_header('net/if.h'))
conf_data.set10('BX_HAVE___BUILTIN_BSWAP32', cc.has_function('__builtin_bswap32'))
conf_data.set10('BX_HAVE___BUILTIN_BSWAP64', cc.has_function('__builtin_bswap64'))
conf_data.set10('BX_HAVE_SSIZE_T', cc.has_type('ssize_t', prefix : '#include <sys/types.h>'))

conf_data.set10('BX_HAVE_LTDL', dl_dep.found())
conf_data.set10('BX_HAVE_DLFCN_H', cc.has_header('dlfcn.h'))
conf_data.set10('HAVE_ASSERT_H', cc.has_header('assert.h'))

# map, set, etc are mandatory C++11, do we still need to check for them ?!?!
conf_data.set10('BX_HAVE_SET', cxx.has_header('set', required: false))
conf_data.set10('BX_HAVE_MAP', cxx.has_header('map', required: false))
conf_data.set10('BX_HAVE_SET_H', cxx.has_header('set.h', required: false))
conf_data.set10('BX_HAVE_MAP_H', cxx.has_header('map.h', required: false))

conf_data.set10('BX_HAVE_GETENV', cc.has_function('getenv', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_SETENV', cc.has_function('setenv', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_SELECT', cc.has_function('select', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_SNPRINTF', cc.has_function('snprintf', prefix : '#include <stdio.h>'))
conf_data.set10('BX_HAVE_VSNPRINTF', cc.has_function('vsnprintf', prefix : '#include <stdio.h>'))
conf_data.set10('BX_HAVE_STRTOULL', cc.has_function('strtoull', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_STRTOUQ', cc.has_function('strtouq', prefix : '#include <stdlib.h>'))
conf_data.set10('BX_HAVE_STRDUP', cc.has_function('strdup', prefix : '#include <string.h>'))
conf_data.set10('BX_HAVE_STRREV', cc.has_function('strrev', prefix : '#include <strings.h>'))
conf_data.set10('BX_HAVE_STRICMP', cc.has_function('stricmp', prefix : '#include <strings.h>'))
conf_data.set10('BX_HAVE_STRCASECMP', cc.has_function('strcasecmp', prefix : '#include <strings.h>'))


conf_data.set10('BX_HAVE_COLOR_SET', term_dep.found())
conf_data.set10('BX_HAVE_MVHLINE', term_dep.found())
conf_data.set10('BX_HAVE_MVVLINE', term_dep.found())


enable_network = get_option('networking')
conf_data.set10('BX_NETWORKING', enable_network)
conf_data.set10('BX_NETMOD_FBSD',  enable_network and cc.has_header('net/bpf.h', required: false))
conf_data.set10('BX_NETMOD_LINUX', enable_network and cc.has_header('netpacket/packet.h', required: false))
conf_data.set10('BX_NETMOD_WIN32', enable_network and host_machine.system() == 'windows')
conf_data.set10('BX_NETMOD_VDE', enable_network and host_machine.system() == 'windows' and get_option('netmod_vde'))
conf_data.set10('BX_NETMOD_TAP', enable_network and get_option('netmod_tap'))
conf_data.set10('BX_NETMOD_TUNTAP', enable_network and get_option('netmod_tuntap'))
iphlpapi_dep = cc.find_library('iphlpapi', required: false)
conf_data.set10('BX_NETMOD_SLIRP', enable_network and host_machine.system() == 'windows' and iphlpapi_dep.found())
conf_data.set10('BX_NETMOD_SOCKET', enable_network and host_machine.system() == 'windows' and iphlpapi_dep.found())


conf_data.set10('BX_HAVE_SOUND_OSX', host_machine.system() == 'darwin')
conf_data.set10('BX_HAVE_SOUND_WIN', host_machine.system() == 'windows')
conf_data.set10('BX_HAVE_SOUND_SDL', sdl1_dep.found() or sdl2_dep.found())
conf_data.set10('BX_HAVE_SOUND_ALSA', alsa_dep.found())
conf_data.set10('BX_HAVE_SOUND_OSS', oss_dep.found())


# keep it in sync with meson.options
options = [
            # various otions
            'debug_linux', 'support_a20', 'dma_floppy_io', 'support_x86_64',  'phy_address_long', 'use_idle_hack', 'plugins', 'use_raw_serial',  'large_ramfile',  'dbg_extensions',

            'use_textconfig',

            'gdbstub', 'debugger',  'debugger_gui', 'support_iodebug', 'instrumentation',

            'no_logging',

            'assert_enable',

            'enable_statistics',

            'support_alignment_check',  'support_fpu', 'support_3dnow', 'support_pkeys', 'support_cet', 'support_monitor_mwait',
            'support_perfmon', 'support_memtype', 'support_svm', 'support_vmx', 'support_avx', 'support_evex',

            'support_repeat_speedups',  'support_handlers_chaining_speedups',  'enable_trace_linking', 'configure_msrs',

            # Soundcard and gameport support
            'support_sb16', 'support_es1370', 'support_gameport', 'support_soundlow',

            # Support x86 hardware debugger registers and facilities.
            'x86_debugger',

            # limited i440FX PCI support
            'support_pci',

            # Experimental host PCI device mapping
            'support_pcidev',

            # CLGD54XX emulation
            'support_clgd54xx',

            # Experimental 3dfx Voodoo (SST-1/2) emulation
            'support_voodoo',

            # USB host controllers
            'support_usb_uhci', 'support_usb_ohci', 'support_usb_ehci', 'support_usb_xhci',

            # MS bus mouse support
            'support_busmouse',

            # CDROM
            'support_cdrom',

            # NE2K network emulation
            'support_ne2k',

            # Pseudo PCI NIC
            'support_pcipnic',

            # Intel(R) Gigabit Ethernet
            'support_e1000',

            #SFM stuff
            'use_cpu_smf',  'use_mem_smf',  'use_hd_smf',  'use_bios_smf', 'use_cmos_smf', 'use_dma_smf',  'use_fd_smf', 'use_key_smf', 'use_par_smf', 'use_pic_smf', 'use_pit_smf',
            'use_ser_smf', 'use_um_smf', 'use_vga_smf', 'use_sb16_smf', 'use_es1370_smf', 'use_dev_smf', 'use_pci_smf', 'use_p2i_smf', 'use_pide_smf', 'use_pcidev_smf', 'use_usb_uhci_smf',
            'use_usb_ohci_smf', 'use_usb_ehci_smf', 'use_usb_xhci_smf', 'use_pcipnic_smf', 'use_efi_smf', 'use_gameport_smf',  'use_cirrus_smf', 'use_busm_smf', 'use_acpi_smf'
]

foreach opt : options
    conf_data.set10('BX_' + opt.to_upper(), get_option(opt))
endforeach

configure_file(input : 'config.h.meson.in', output : 'config.h', configuration : conf_data)

conf_data = configuration_data()
conf_data.set('VERSION', meson.project_version())
conf_data.set('VER_SVN', 1)
configure_file(input : 'bxversion.h.in', output : 'bxversion.h', configuration : conf_data)

top_include = include_directories('.', 'instrument/stubs')

bx_deps = []
if get_option('debugger') or get_option('debugger_gui')
    subdir('bx_debug')
endif

if get_option('support_pcidev')
    # FIX ME https://github.com/mesonbuild/meson/issues/9084
    # subdir('host/linux/pcidev')
endif

subdir('instrument/stubs')
subdir('cpu')
subdir('gui')
subdir('memory')
subdir('iodev')

common_sources = [
    'bxthread.cc',
    'crc.cc',
    'logio.cc',
    'osdep.cc',
    'pc_system.cc',
    'plugin.cc'
]

if get_option('gdbstub')
    common_sources += 'gdbstub.cc'
endif

executable('bochs', [common_sources, 'main.cc', 'config.cc'],
                dependencies: [x11_dep, xrandr_dep, xpm_dep, readline_dep, sdl1_dep, sdl2_dep, term_dep, vncsrv_dep, wx_dep],
                link_with: bx_deps,
                cpp_args: ['-DBX_SHARE_PATH="' + join_paths(get_option('prefix'), get_option('datadir'), 'bochs') + '"'],
                include_directories: [top_include])

#executable('bxdisasm', [common_sources, 'bxdisasm.cc'], dependencies: [x11_dep, xrandr_dep, sdl1_dep, sdl2_dep, term_dep, vncsrv_dep, wx_dep], link_with: bx_deps, include_directories: [top_include])
