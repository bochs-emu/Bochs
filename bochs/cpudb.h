/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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
#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_X86_64 == 0
bx_define_cpudb(p3_katmai)
bx_define_cpudb(p4_willamette)
bx_define_cpudb(atom_n270)
#else
bx_define_cpudb(p4_prescott_celeron_336)
bx_define_cpudb(athlon64_clawhammer)
bx_define_cpudb(core2_extreme_x9770)
#if BX_SUPPORT_AVX
bx_define_cpudb(corei7_sandy_bridge_2600k)
#endif
#endif
#endif
