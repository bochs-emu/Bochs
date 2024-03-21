sources = [
    'enh_dbg.cc',
    'gui.cc',
    'keymap.cc',
    'nogui.cc',
    'paramtree.cc',
    'scrollwin.cc',
    'sdl2.cc',
    'sdl.cc',
    'siminterface.cc',
    'term.cc',
    'textconfig.cc',
    'x.cc'
]

if host_machine.system() == 'amiga'
    sources += 'amigaos.cc'
endif

if host_machine.system() == 'darwin'
    sources += ['macintosh.cc', 'carbon.cc']
endif


if host_machine.system() == 'windows'
    sources += [
#         'rfb.cc',
        'win32.cc',
        'win32config.cc',
        'win32_enh_dbg_osdep.cc',
        'win32paramdlg.cc'
    ]
endif

if vncsrv_dep.found()
    sources += 'vncsrv.cc'
endif

gui_deps = [x11_dep, sdl1_dep, sdl2_dep, term_dep, vncsrv_dep, wx_dep]

if wx_dep.found()
    sources += ['wx.cc', 'wxdialog.cc', 'wxmain.cc']
    gtk_dep = dependency('gtk+-3.0', required : false)
else
    gtk_dep = dependency('gtk+-2.0', required : false)
endif

if gtk_dep.found()
    gui_deps += gtk_dep
    sources += 'gtk_enh_dbg_osdep.cc'
endif

bx_deps += static_library('gui', sources, dependencies: gui_deps, include_directories: [top_include, '../iodev'])
