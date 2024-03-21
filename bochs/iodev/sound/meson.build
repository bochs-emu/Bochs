sources = [
    'es1370.cc',
    'opl.cc',
    'sb16.cc',
    'soundalsa.cc',
    'sounddummy.cc',
    'soundfile.cc',
    'soundlow.cc',
    'soundmod.cc',
    'soundoss.cc',
    'soundosx.cc',
    'soundsdl.cc',
    'soundwin.cc'
]
bx_deps += static_library('sound', sources, dependencies: [sdl1_dep, sdl2_dep, alsa_dep, oss_dep], include_directories: [top_include, '..'])
