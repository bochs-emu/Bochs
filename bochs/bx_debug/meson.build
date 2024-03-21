flex = find_program('flex', required: false)
yacc = find_program('yacc', required: false)

sources = [
    'dbg_breakpoints.cc',
    'dbg_main.cc',
    'linux.cc',
    'symbols.cc'
]

if flex.found() and yacc.found()
    lgen = generator(flex, output : '@PLAINNAME@.c', arguments : ['-Pbx', '-o', '@OUTPUT@', '@INPUT@'])
    ygen = generator(yacc, output : '@PLAINNAME@.cc', arguments : ['-p', 'bx', '-d', '-o', '@OUTPUT@', '@INPUT@'])
    sources += [lgen.process('lexer.l'), ygen.process('parser.y')]
else
    sources += ['parser.cc', 'lexer.c']
endif

bx_deps += static_library('debug', sources, include_directories: [top_include])
