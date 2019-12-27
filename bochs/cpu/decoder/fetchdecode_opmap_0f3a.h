/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2019 Stanislav Shwartsman
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

#ifndef BX_FETCHDECODE_OPMAP_0F3A_H
#define BX_FETCHDECODE_OPMAP_0F3A_H

#if BX_CPU_LEVEL >= 6

/* ************************************************************************ */
/* 3-byte opcode table (Table A-5, 0F 3A) */

static const Bit64u BxOpcodeTable0F3A08[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ROUNDPS_VpsWpsIb) };
static const Bit64u BxOpcodeTable0F3A09[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ROUNDPD_VpdWpdIb) };
static const Bit64u BxOpcodeTable0F3A0A[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ROUNDSS_VssWssIb) };
static const Bit64u BxOpcodeTable0F3A0B[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_ROUNDSD_VsdWsdIb) };
static const Bit64u BxOpcodeTable0F3A0C[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_BLENDPS_VpsWpsIb) };
static const Bit64u BxOpcodeTable0F3A0D[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_BLENDPD_VpdWpdIb) };
static const Bit64u BxOpcodeTable0F3A0E[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PBLENDW_VdqWdqIb) };

static const Bit64u BxOpcodeTable0F3A0F[] = {
  form_opcode(ATTR_SSE_NO_PREFIX, BX_IA_PALIGNR_PqQqIb),
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PALIGNR_VdqWdqIb),
};

static const Bit64u BxOpcodeTable0F3A14[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PEXTRB_EbdVdqIb) };
static const Bit64u BxOpcodeTable0F3A15[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PEXTRW_EwdVdqIb) };

// opcode 0F 3A 16
static const Bit64u BxOpcodeTable0F3A16[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_OS64, BX_IA_PEXTRQ_EqVdqIb),
#endif
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PEXTRD_EdVdqIb)
};

static const Bit64u BxOpcodeTable0F3A17[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_EXTRACTPS_EdVpsIb) };
static const Bit64u BxOpcodeTable0F3A20[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PINSRB_VdqEbIb) };
static const Bit64u BxOpcodeTable0F3A21[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_INSERTPS_VpsWssIb) };

// opcode 0F 3A 22
static const Bit64u BxOpcodeTable0F3A22[] = {
#if BX_SUPPORT_X86_64
  form_opcode(ATTR_SSE_PREFIX_66 | ATTR_OS64, BX_IA_PINSRQ_VdqEqIb),
#endif
  last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PINSRD_VdqEdIb)
};

static const Bit64u BxOpcodeTable0F3A40[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_DPPS_VpsWpsIb) };
static const Bit64u BxOpcodeTable0F3A41[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_DPPD_VpdWpdIb) };
static const Bit64u BxOpcodeTable0F3A42[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_MPSADBW_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3A44[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCLMULQDQ_VdqWdqIb) };

static const Bit64u BxOpcodeTable0F3A60[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPESTRM_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3A61[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPESTRI_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3A62[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPISTRM_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3A63[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_PCMPISTRI_VdqWdqIb) };

static const Bit64u BxOpcodeTable0F3ACC[] = { last_opcode(ATTR_SSE_NO_PREFIX, BX_IA_SHA1RNDS4_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3ACE[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_GF2P8AFFINEQB_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3ACF[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_GF2P8AFFINEINVQB_VdqWdqIb) };
static const Bit64u BxOpcodeTable0F3ADF[] = { last_opcode(ATTR_SSE_PREFIX_66, BX_IA_AESKEYGENASSIST_VdqWdqIb) };

#endif

#endif // BX_FETCHDECODE_OPMAP_0F3A_H
