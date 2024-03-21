sources = [
    'avx.cc',
    'avx2.cc',
    'avx512.cc',
    'avx512_bitalg.cc',
    'avx512_broadcast.cc',
    'avx512_cvt.cc',
    'avx512_fma.cc',
    'avx512_mask16.cc',
    'avx512_mask32.cc',
    'avx512_mask64.cc',
    'avx512_mask8.cc',
    'avx512_move.cc',
    'avx512_pfp.cc',
    'avx512_rcp14.cc',
    'avx512_rsqrt14.cc',
    'avx_cvt.cc',
    'avx_fma.cc',
    'avx_ifma52.cc',
    'avx_pfp.cc',
    'gather.cc',
    'tbm32.cc',
    'tbm64.cc',
    'vnni.cc',
    'xop.cc'
]

bx_deps += static_library('avx', sources, include_directories: [top_include, '..'])
