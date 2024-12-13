sources = [
    # AMD
    'amd/amd_k6_2_chomper.cc',
    'amd/athlon64_clawhammer.cc',
    'amd/athlon64_venice.cc',
    'amd/phenomx3_8650_toliman.cc',
    'amd/ryzen.cc',
    'amd/trinity_apu.cc',
    'amd/turion64_tyler.cc',
    'amd/zambezi.cc',

    # Intel
    'intel/atom_n270.cc',
    'intel/broadwell_ult.cc',
    'intel/core2_penryn_t9600.cc',
    'intel/core_duo_t2400_yonah.cc',
    'intel/corei3_cnl.cc',
    'intel/corei5_arrandale_m520.cc',
    'intel/corei5_lynnfield_750.cc',
    'intel/corei7_haswell_4770.cc',
    'intel/corei7_icelake-u.cc',
    'intel/corei7_ivy_bridge_3770K.cc',
    'intel/corei7_sandy_bridge_2600K.cc',
    'intel/corei7_skylake-x.cc',
    'intel/p2_klamath.cc',
    'intel/p3_katmai.cc',
    'intel/p4_prescott_celeron_336.cc',
    'intel/p4_willamette.cc',
    'intel/pentium.cc',
    'intel/pentium_mmx.cc',
    'intel/tigerlake.cc',
]

bx_deps += static_library('cpudb', sources, include_directories: [top_include, '..', 'amd', 'intel'])
