/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2014 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

bx_define_cpudb(bx_generic)
#if BX_CPU_LEVEL >= 5
bx_define_cpudb(pentium_mmx)
bx_define_cpudb(amd_k6_2_chomper)
#if BX_CPU_LEVEL >= 6
bx_define_cpudb(p2_klamath)
bx_define_cpudb(p3_katmai)
bx_define_cpudb(p4_willamette)
bx_define_cpudb(core_duo_t2400_yonah)
bx_define_cpudb(atom_n270)
#if BX_SUPPORT_X86_64
bx_define_cpudb(p4_prescott_celeron_336)
bx_define_cpudb(athlon64_clawhammer)
bx_define_cpudb(athlon64_venice)
bx_define_cpudb(turion64_tyler)
bx_define_cpudb(phenom_8650_toliman)
bx_define_cpudb(core2_penryn_t9600)
bx_define_cpudb(corei5_lynnfield_750)
bx_define_cpudb(corei5_arrandale_m520)
bx_define_cpudb(corei7_sandy_bridge_2600k)
#if BX_SUPPORT_AVX
bx_define_cpudb(zambezi)
bx_define_cpudb(trinity_apu)
bx_define_cpudb(corei7_ivy_bridge_3770k)
bx_define_cpudb(corei7_haswell_4770)
#endif
#endif
#endif
#endif
