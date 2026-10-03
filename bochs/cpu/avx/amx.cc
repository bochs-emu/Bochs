/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024-2026 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_AMX

#include "amx.h"

// Palette support is determined by XCR0 only and doesn't depend on current cpu mode.
// AMX instructions can be executed only in 64-bit mode (enforced by the decoder) but
// XSAVE feature set (XRSTOR/XRSTORS) can load TILECFG state in any mode
BX_CPP_INLINE bool BX_CPU_C::palette_supported(unsigned palette_id)
{
  Bit32u xcr0 = BX_CPU_THIS_PTR xcr0.get32();

  switch (palette_id) {
  case 0:
    return true;
  case 1:
    return (xcr0 & BX_XCR0_XTILE_BITS_MASK) == BX_XCR0_XTILE_BITS_MASK;
  case 2:
    return (xcr0 & (BX_XCR0_XTILE_BITS_MASK | BX_XCR0_SCALEDATA_MASK)) == (BX_XCR0_XTILE_BITS_MASK | BX_XCR0_SCALEDATA_MASK);
  default:
    return false;
  }
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::configure_tiles(bxInstruction_c *i, const BxPackedAvxRegister &tilecfg)
{
  Bit8u palette_id = tilecfg.vmmubyte(0);
  Bit8u start_row  = tilecfg.vmmubyte(1);

  if (! palette_supported(palette_id)) return false;

  if (palette_id == 0) {
    BX_CPU_THIS_PTR amx->init_tilecfg();
    return true;
  }

  if (palette_id == 1) {
    if ((tilecfg.vmm64u(0) >> 16) != 0 || tilecfg.vmm64u(1) || tilecfg.vmm64u(4) || tilecfg.vmm64u(5) || tilecfg.vmm64u(7)) {
      BX_ERROR(("%s: reserved bits set for palette_id=%d", i->getIaOpcodeNameShort(), palette_id));
      return false;
    }

    AMX::TILECFG tile[8];

    for (unsigned n=0; n < 8; n++) {
      tile[n].bytes_per_row = tilecfg.vmm16u(8+n);
      if (tile[n].bytes_per_row > 64) {
        BX_ERROR(("%s: too many bytes_per_row for tile=%d in palette_id=%d", i->getIaOpcodeNameShort(), n, palette_id));
        return false;
      }        
      tile[n].rows = tilecfg.vmmubyte(48+n);
      if (tile[n].rows > 16) {
        BX_ERROR(("%s: too many rows for tile=%d in palette_id=%d", i->getIaOpcodeNameShort(), n, palette_id));
        return false;
      }
      if ((tile[n].rows == 0 && tile[n].bytes_per_row != 0) || (tile[n].rows != 0 && tile[n].bytes_per_row == 0)) {
        BX_ERROR(("%s: invalid empty tile=%d in palette_id=%d", i->getIaOpcodeNameShort(), n, palette_id));
        return false;
      }
    }

    BX_CPU_THIS_PTR amx->init_tilecfg();
    BX_CPU_THIS_PTR amx->palette_id = 1;
    BX_CPU_THIS_PTR amx->start_row = start_row;
    for (unsigned n=0; n < 8; n++)
      BX_CPU_THIS_PTR amx->tilecfg[n] = tile[n];
    return true;
  }

  if (is_cpu_extension_supported(BX_ISA_ACE)) {
    if (palette_id == 2) {
      if (tilecfg.vmm64u(0) > 255 || tilecfg.vmm64u(1) || ! is_clear(&tilecfg.vmm128(1)) || ! is_clear(&tilecfg.vmm128(2)) || ! is_clear(&tilecfg.vmm128(3))) {
        BX_ERROR(("%s: reserved bits set for palette_id=%d", i->getIaOpcodeNameShort(), palette_id));
        return false;
      }

      BX_CPU_THIS_PTR amx->init_tilecfg();
      BX_CPU_THIS_PTR amx->palette_id = 2;
      BX_CPU_THIS_PTR amx->start_row = 0;

      for (unsigned n=0; n < 8; n++) {
        BX_CPU_THIS_PTR amx->tilecfg[n].bytes_per_row = 64;
        BX_CPU_THIS_PTR amx->tilecfg[n].rows = 16;
      }

      return true;
    }
  }

  return false;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LDTILECFG(bxInstruction_c *i)
{
  BxPackedAvxRegister tilecfg;
  Bit64u eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  read_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), &tilecfg);

  if (!configure_tiles(i, tilecfg))
    exception(BX_GP_EXCEPTION, 0);

  // successful LDTILECFG initializes TILEDATA and SCALEDATA
  BX_CPU_THIS_PTR amx->init_tiledata();
  BX_CPU_THIS_PTR amx->bsr_clear();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STTILECFG(bxInstruction_c *i)
{
  xsave_tilecfg_state(i, BX_CPU_RESOLVE_ADDR_64(i));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILELOADD_TnnnMdq(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 1) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile = i->dst();

  check_tile(i, tile);

  unsigned rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile);
  unsigned dword_elements_per_row = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile);

  if (BX_CPU_THIS_PTR amx->start_row >= rows) {
    BX_ERROR(("%s: invalid tile %d (start_row=%d) >= (rows=%d)", i->getIaOpcodeNameShort(), tile, BX_CPU_THIS_PTR amx->start_row, rows));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit32u mask = (dword_elements_per_row < 16) ? ((1 << dword_elements_per_row) - 1) : 0xFFFF;

  BX_CPU_THIS_PTR amx->set_tile_used(tile);

  BX_CPU_THIS_PTR amx->tile[tile].zero_out_of_range_columns(dword_elements_per_row);

  BX_CPU_THIS_PTR amx->tile[tile].clear_upper_rows(BX_CPU_THIS_PTR amx->start_row);

  Bit64u start_eaddr = BX_READ_64BIT_REG(i->sibBase()) + (Bit64s) i->displ32s();
  // if no index register in the SIB encoding, the value zero is used
  Bit64u stride = (i->sibIndex() != 4) ? (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) : 0;
  i->setVL(BX_VL512);

  for (unsigned row=BX_CPU_THIS_PTR amx->start_row; row < rows; row++) {
    BxPackedAvxRegister *data = &(BX_CPU_THIS_PTR amx->tile[tile].row[row]);

    Bit64u eaddr = start_eaddr + row * stride;
    if (dword_elements_per_row == 16) {
      read_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), data);
    }
    else {
      avx_masked_load32(i, eaddr, data, mask);
    }

    BX_CPU_THIS_PTR amx->start_row++;
  }

  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILESTORED_MdqTnnn(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 1) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile = i->src();

  check_tile(i, tile);

  unsigned rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile);
  unsigned dword_elements_per_row = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile);

  if (BX_CPU_THIS_PTR amx->start_row >= rows) {
    BX_ERROR(("TILESTORED: invalid tile %d (start_row=%d) >= (rows=%d)", tile, BX_CPU_THIS_PTR amx->start_row, rows));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit32u mask = (dword_elements_per_row < 16) ? ((1 << dword_elements_per_row) - 1) : 0xFFFF;
  i->setVL(BX_VL512);

  Bit64u start_eaddr = BX_READ_64BIT_REG(i->sibBase()) + (Bit64s) i->displ32s();
  // if no index register in the SIB encoding, the value zero is used
  Bit64u stride = (i->sibIndex() != 4) ? (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) : 0;

  for (unsigned row=BX_CPU_THIS_PTR amx->start_row; row < rows; row++) {
    BxPackedAvxRegister *data = &(BX_CPU_THIS_PTR amx->tile[tile].row[row]);
    Bit64u eaddr = start_eaddr + row * stride;
    if (dword_elements_per_row == 16)
      write_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), data);
    else
      avx_masked_store32(i, eaddr, data, mask);

    BX_CPU_THIS_PTR amx->start_row++;
  }

  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILEZERO_Tnnn(bxInstruction_c *i)
{
  unsigned tile = i->dst();

  if (tile >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile)) {
    BX_ERROR(("TILEZERO: invalid tile %d", tile));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_CPU_THIS_PTR amx->clear_tile_used(tile);
  BX_CPU_THIS_PTR amx->tile[tile].clear();
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILERELEASE(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR amx->clear();
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::check_tile(bxInstruction_c *i, unsigned tile_num)
{
  // #UD if TILES_CONFIGURED == 0
  // #UD if src are not valid tile
  // #UD if src is >= palette_table[tilecfg.palette_id].max_names
  if (tile_num >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile_num)) {
    BX_ERROR(("%s: invalid tile %d", i->getIaOpcodeNameShort(), tile_num));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned bytes_per_row = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_num);
  if ((bytes_per_row & 0x3) != 0) {
    BX_ERROR(("%s: invalid tile %d bytes_per_row=%d", i->getIaOpcodeNameShort(), tile_num, bytes_per_row));
    exception(BX_UD_EXCEPTION, 0);
  }
}

void BX_CPU_C::check_tiles(bxInstruction_c *i, unsigned tile_dst, unsigned tile_src1, unsigned tile_src2)
{
  // #UD if srcdest == src1 OR src1 == src2 OR srcdest == src2
  if (tile_dst == tile_src1 || tile_dst == tile_src2 || tile_src1 == tile_src2) {
    BX_ERROR(("%s: must use different tiles", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // #UD if TILES_CONFIGURED == 0
  // #UD if srcdest/src1/src2 are not valid tiles
  // #UD if srcdest/src1/src2 are >= palette_table[tilecfg.palette_id].max_names
  check_tile(i, tile_dst);
  check_tile(i, tile_src1);
  check_tile(i, tile_src2);

  unsigned rows[3];
  unsigned dword_elements_per_row[3];

  rows[0] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  dword_elements_per_row[0] = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  rows[1] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src1);
  dword_elements_per_row[1] = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_src1);
  rows[2] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);
  dword_elements_per_row[2] = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned n = dword_elements_per_row[0];
  unsigned m = rows[1];
  unsigned k = rows[2];

  // #UD if srcdest.colbytes != src2.colbytes (n)
  // #UD if srcdest.rows != src1.rows (m)
  // #UD if src1.colbytes / 4 != src2.rows (k)
  if (n != dword_elements_per_row[2] || m != rows[0] || k != dword_elements_per_row[1]) {
    BX_ERROR(("%s: invalid matmul tile dimenstions", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // #UD if srcdest.colbytes > tmul_maxn
  // #UD if src2.colbytes > tmul_maxn
  // #UD if src1.colbytes/4 > tmul_maxk
  // #UD if src2.rows > tmul_maxk
  if (n > 16 || k > 16) {
    BX_ERROR(("%s: unsupported matmul tile dimenstions", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
}

// AMX-INT8 //

BX_CPP_INLINE Bit32u DPBDSS(Bit32u x, Bit32u y)
{
  const Bit8u xbyte[4] = { Bit8u(x & 0xff), Bit8u((x >> 8) & 0xff), Bit8u((x >> 16) & 0xff), Bit8u(x >> 24) };
  const Bit8u ybyte[4] = { Bit8u(y & 0xff), Bit8u((y >> 8) & 0xff), Bit8u((y >> 16) & 0xff), Bit8u(y >> 24) };

  Bit32s p0dword = Bit32s(xbyte[0]) * Bit32s(ybyte[0]);
  Bit32s p1dword = Bit32s(xbyte[1]) * Bit32s(ybyte[1]);
  Bit32s p2dword = Bit32s(xbyte[2]) * Bit32s(ybyte[2]);
  Bit32s p3dword = Bit32s(xbyte[3]) * Bit32s(ybyte[3]);

  return p0dword + p1dword + p2dword + p3dword;
}

BX_CPP_INLINE Bit32u DPBDSU(Bit32u x, Bit32u y)
{
  const Bit8u xbyte[4] = { Bit8u(x & 0xff), Bit8u((x >> 8) & 0xff), Bit8u((x >> 16) & 0xff), Bit8u(x >> 24) };
  const Bit8u ybyte[4] = { Bit8u(y & 0xff), Bit8u((y >> 8) & 0xff), Bit8u((y >> 16) & 0xff), Bit8u(y >> 24) };

  Bit32s p0dword = Bit32s(xbyte[0]) * Bit32u(ybyte[0]);
  Bit32s p1dword = Bit32s(xbyte[1]) * Bit32u(ybyte[1]);
  Bit32s p2dword = Bit32s(xbyte[2]) * Bit32u(ybyte[2]);
  Bit32s p3dword = Bit32s(xbyte[3]) * Bit32u(ybyte[3]);

  return p0dword + p1dword + p2dword + p3dword;
}

BX_CPP_INLINE Bit32u DPBDUS(Bit32u x, Bit32u y)
{
  const Bit8u xbyte[4] = { Bit8u(x & 0xff), Bit8u((x >> 8) & 0xff), Bit8u((x >> 16) & 0xff), Bit8u(x >> 24) };
  const Bit8u ybyte[4] = { Bit8u(y & 0xff), Bit8u((y >> 8) & 0xff), Bit8u((y >> 16) & 0xff), Bit8u(y >> 24) };

  Bit32s p0dword = Bit32u(xbyte[0]) * Bit32s(ybyte[0]);
  Bit32s p1dword = Bit32u(xbyte[1]) * Bit32s(ybyte[1]);
  Bit32s p2dword = Bit32u(xbyte[2]) * Bit32s(ybyte[2]);
  Bit32s p3dword = Bit32u(xbyte[3]) * Bit32s(ybyte[3]);

  return p0dword + p1dword + p2dword + p3dword;
}

BX_CPP_INLINE Bit32u DPBDUU(Bit32u x, Bit32u y)
{
  const Bit8u xbyte[4] = { Bit8u(x & 0xff), Bit8u((x >> 8) & 0xff), Bit8u((x >> 16) & 0xff), Bit8u(x >> 24) };
  const Bit8u ybyte[4] = { Bit8u(y & 0xff), Bit8u((y >> 8) & 0xff), Bit8u((y >> 16) & 0xff), Bit8u(y >> 24) };

  Bit32u p0dword = Bit32u(xbyte[0]) * Bit32u(ybyte[0]);
  Bit32u p1dword = Bit32u(xbyte[1]) * Bit32u(ybyte[1]);
  Bit32u p2dword = Bit32u(xbyte[2]) * Bit32u(ybyte[2]);
  Bit32u p3dword = Bit32u(xbyte[3]) * Bit32u(ybyte[3]);

  return p0dword + p1dword + p2dword + p3dword;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBSSD_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  /*     R   C            */
  /* A = m x k (tsrc1)    */
  /* B = k x n (tsrc2)    */
  /* C = m x n (tsrcdest) */
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  for (unsigned m=0; m < max_m; m++) {
    BxPackedAvxRegister* tmp = &(tdst->row[m]);
    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp->vmm32s(n) += DPBDSS(tsrc1->row[m].vmm32u(k), tsrc2->row[k].vmm32u(n));
      }
    }
    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBSUD_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  /*     R   C            */
  /* A = m x k (tsrc1)    */
  /* B = k x n (tsrc2)    */
  /* C = m x n (tsrcdest) */
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  for (unsigned m=0; m < max_m; m++) {
    BxPackedAvxRegister* tmp = &(tdst->row[m]);
    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp->vmm32s(n) += DPBDSU(tsrc1->row[m].vmm32u(k), tsrc2->row[k].vmm32u(n));
      }
    }
    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBUSD_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  /*     R   C            */
  /* A = m x k (tsrc1)    */
  /* B = k x n (tsrc2)    */
  /* C = m x n (tsrcdest) */
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  for (unsigned m=0; m < max_m; m++) {
    BxPackedAvxRegister* tmp = &(tdst->row[m]);
    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp->vmm32s(n) += DPBDUS(tsrc1->row[m].vmm32u(k), tsrc2->row[k].vmm32u(n));
      }
    }
    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBUUD_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  /*     R   C            */
  /* A = m x k (tsrc1)    */
  /* B = k x n (tsrc2)    */
  /* C = m x n (tsrcdest) */
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  for (unsigned m=0; m < max_m; m++) {
    BxPackedAvxRegister* tmp = &(tdst->row[m]);
    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp->vmm32u(n) += DPBDUU(tsrc1->row[m].vmm32u(k), tsrc2->row[k].vmm32u(n));
      }
    }
    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();
  BX_NEXT_INSTR(i);
}

// AMX-BF16 //

#include "softfloat3e/include/softfloat.h"
#include "bf16.h"

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBF16PS_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp[2*n]   = f32_mulAdd(convert_bfloat16_to_fp32(tsrc1->row[m].vmm16u(2*k)),
                                convert_bfloat16_to_fp32(tsrc2->row[k].vmm16u(2*n)),   tmp[2*n],   0, &status);

        tmp[2*n+1] = f32_mulAdd(convert_bfloat16_to_fp32(tsrc1->row[m].vmm16u(2*k+1)),
                                convert_bfloat16_to_fp32(tsrc2->row[k].vmm16u(2*n+1)), tmp[2*n+1], 0, &status);
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      float32 tmpf32 = f32_add(tmp[2*n], tmp[2*n+1], &status);
      tdst->row[m].vmm32u(n) = f32_add(tdst->row[m].vmm32u(n), tmpf32, &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

// AMX-FP16 //

extern float32 convert_ne_fp16_to_fp32(float16 op);

// convert FP16 elements of the tile to FP32 once, for reuse across the matrix multiplication loops
static void convert_fp16_tile_to_fp32(AMX::TILE *tile, unsigned nrows, unsigned nelements, float32 dst[][32])
{
  for (unsigned row=0; row < nrows; row++)
    for (unsigned e=0; e < nelements; e++)
      dst[row][e] = convert_ne_fp16_to_fp32(tile->row[row].vmm16u(e));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPFP16PS_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output FP32 denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert all FP16 source elements to FP32 once
  float32 src1_fp32[BX_TILE_MAX_ROWS][32], src2_fp32[BX_TILE_MAX_ROWS][32];
  convert_fp16_tile_to_fp32(tsrc1, max_m, 2*max_k, src1_fp32);
  convert_fp16_tile_to_fp32(tsrc2, max_k, 2*max_n, src2_fp32);

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp[2*n]   = f32_mulAdd(src1_fp32[m][2*k],   src2_fp32[k][2*n],   tmp[2*n],   0, &status);

        tmp[2*n+1] = f32_mulAdd(src1_fp32[m][2*k+1], src2_fp32[k][2*n+1], tmp[2*n+1], 0, &status);
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      float32 tmpf32 = f32_add(tmp[2*n], tmp[2*n+1], &status);
      tdst->row[m].vmm32u(n) = f32_add(tdst->row[m].vmm32u(n), tmpf32, &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

// AMX-COMPLEX //

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCMMRLFP16PS_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output FP32 denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert all FP16 source elements to FP32 once
  float32 src1_fp32[BX_TILE_MAX_ROWS][32], src2_fp32[BX_TILE_MAX_ROWS][32];
  convert_fp16_tile_to_fp32(tsrc1, max_m, 2*max_k, src1_fp32);
  convert_fp16_tile_to_fp32(tsrc2, max_k, 2*max_n, src2_fp32);

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        float32 s1r = src1_fp32[m][2*k];     // real
        float32 s2r = src2_fp32[k][2*n];     // real
        float32 s1i = src1_fp32[m][2*k+1];   // imaginary
        float32 s2i = src2_fp32[k][2*n+1];   // imaginary

        tmp[2*n]   = f32_mulAdd(s1r, s2r, tmp[2*n],   0, &status);                               // real
        tmp[2*n+1] = f32_mulAdd(s1i, s2i, tmp[2*n+1], softfloat_muladd_negate_product, &status);     // imaginary, negate for i^2 = -1
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      float32 tmpf32 = f32_add(tmp[2*n], tmp[2*n+1], &status);
      tdst->row[m].vmm32u(n) = f32_add(tdst->row[m].vmm32u(n), tmpf32, &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCMMIMFP16PS_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output FP32 denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert all FP16 source elements to FP32 once
  float32 src1_fp32[BX_TILE_MAX_ROWS][32], src2_fp32[BX_TILE_MAX_ROWS][32];
  convert_fp16_tile_to_fp32(tsrc1, max_m, 2*max_k, src1_fp32);
  convert_fp16_tile_to_fp32(tsrc2, max_k, 2*max_n, src2_fp32);

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        float32 s1r = src1_fp32[m][2*k];     // real
        float32 s2r = src2_fp32[k][2*n];     // real
        float32 s1i = src1_fp32[m][2*k+1];   // imaginary
        float32 s2i = src2_fp32[k][2*n+1];   // imaginary

        // real * imaginary products are accumulated first to get NaN propagation priority
        // matching Intel SDE (SDM pseudocode has the two accumulators in reverse order)
        tmp[2*n]   = f32_mulAdd(s1r, s2i, tmp[2*n],   0, &status);
        tmp[2*n+1] = f32_mulAdd(s1i, s2r, tmp[2*n+1], 0, &status);
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      float32 tmpf32 = f32_add(tmp[2*n], tmp[2*n+1], &status);
      tdst->row[m].vmm32u(n) = f32_add(tdst->row[m].vmm32u(n), tmpf32, &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

// AMX-FP8 //

#include "wide_int.h"
#include "fpu/softfloat-specialize.h"

extern Bit64s convert_bf8_to_fixpoint64(Bit8u fp8_byte);
extern Bit64s convert_hf8_to_fixpoint64(Bit8u fp8_byte);
extern float32 convert_fixpoint128_scaled_to_fp32_ftz_rne(Bit128s x, int adjust);

enum {
  BX_FP8_FINITE = 0,
  BX_FP8_INF = 1,
  BX_FP8_NAN = 2
};

// infinity accumulation state
enum {
  BX_FP8_NO_INF        = 0,
  BX_FP8_POS_INF       = 1,
  BX_FP8_NEG_INF       = 2,
  BX_FP8_CANCELLED_INF = 3
};

// FP8 element converted to 64-bit fixed point (BF8 = 2^16 * value, HF8 = 2^9 * value),
// the fixed point value is meaningful only for finite inputs
struct fp8_fixpoint_t {
  Bit64s value;
  Bit8u fp8_class;
  bool sign;
};

static fp8_fixpoint_t convert_fp8_to_fixpoint64(Bit8u fp8_byte, bool is_bf8)
{
  fp8_fixpoint_t x;
  x.value = 0;
  x.sign = (fp8_byte & 0x80) != 0;

  if (is_bf8) {
    if ((fp8_byte & 0x7C) == 0x7C) {
      x.fp8_class = (fp8_byte & 0x3) ? BX_FP8_NAN : BX_FP8_INF;
    }
    else {
      x.fp8_class = BX_FP8_FINITE;
      x.value = convert_bf8_to_fixpoint64(fp8_byte);
    }
  }
  else {
    // E4M3 has no infinity, S.1111.111 encodes NaN
    if ((fp8_byte & 0x7F) == 0x7F) {
      x.fp8_class = BX_FP8_NAN;
    }
    else {
      x.fp8_class = BX_FP8_FINITE;
      x.value = convert_hf8_to_fixpoint64(fp8_byte);
    }
  }

  return x;
}

BX_CPP_INLINE bool fp8_fixpoint_is_zero(const fp8_fixpoint_t &x)
{
  return x.fp8_class == BX_FP8_FINITE && x.value == 0;
}

// accumulate exact product of two finite FP8 elements converted to fixed point,
// product of two BF8 fixed point values might not fit into 64-bit
BX_CPP_INLINE void fp8_fixpoint_mul_add(Bit64s s1, Bit64s s2, Bit128s *sop)
{
  if (s1 == 0 || s2 == 0) return;

  Bit128s product;
  long_imul(&product, s1, s2);
  long_add((Bit128u*) sop, (Bit128u*) &product);
}

// add infinity of given sign to the infinity accumulation state:
// add(+INF, -INF) cancels the infinity, a following infinity of either sign sets it again
BX_CPP_INLINE int fp8_accumulate_inf(int inf_state, int inf_sign)
{
  if (inf_state == BX_FP8_NO_INF || inf_state == BX_FP8_CANCELLED_INF)
    return inf_sign;

  if (inf_state != inf_sign)
    return BX_FP8_CANCELLED_INF;

  return inf_state;
}

// The FP8 products are accumulated exactly as 128-bit fixed point integer over the whole
// K dimension and converted to FP32 (RNE, FTZ) only once before accumulation into srcdest.
// Any NaN input (src1, src2 or srcdest) results in QNaN Indefinite.
void BX_CPP_AttrRegparmN(3) BX_CPU_C::tdpfp8ps_execute(bxInstruction_c *i, bool a_is_bf8, bool b_is_bf8)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_dword_elements_per_row(tile_dst);
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  // fixed point scaling of the product: BF8 = 2^16, HF8 = 2^9 per element
  int factor = (a_is_bf8 && b_is_bf8) ? 32 : (!a_is_bf8 && !b_is_bf8) ? 18 : 25;

  // convert all src2 elements once
  fp8_fixpoint_t src2_fixpoint[BX_TILE_MAX_ROWS][64];
  for (unsigned k=0; k < max_k; k++)
    for (unsigned b=0; b < 4*max_n; b++)
      src2_fixpoint[k][b] = convert_fp8_to_fixpoint64(tsrc2->row[k].vmmubyte(b), b_is_bf8);

  // the final accumulation into srcdest: RNE, FTZ=1, DAZ=1 (FP8 inputs are not DAZ-ed, see above)
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  for (unsigned m=0; m < max_m; m++) {
    fp8_fixpoint_t src1_fixpoint[64];
    for (unsigned b=0; b < 4*max_k; b++)
      src1_fixpoint[b] = convert_fp8_to_fixpoint64(tsrc1->row[m].vmmubyte(b), a_is_bf8);

    for (unsigned n=0; n < max_n; n++) {
      float32 srcdest = tdst->row[m].vmm32u(n);
      bool nan = f32_isNaN(srcdest);
      int inf_state = BX_FP8_NO_INF;

      Bit128s sop;
      sop.lo = 0;
      sop.hi = 0;

      for (unsigned k=0; k < max_k; k++) {
        for (unsigned e=0; e < 4; e++) {
          const fp8_fixpoint_t &s1 = src1_fixpoint[4*k + e], &s2 = src2_fixpoint[k][4*n + e];

          if (s1.fp8_class == BX_FP8_FINITE && s2.fp8_class == BX_FP8_FINITE) {
            fp8_fixpoint_mul_add(s1.value, s2.value, &sop);
          }
          else if (s1.fp8_class == BX_FP8_NAN || s2.fp8_class == BX_FP8_NAN) {
            nan = true;
          }
          else if (fp8_fixpoint_is_zero(s1) || fp8_fixpoint_is_zero(s2)) {
            nan = true; // mult(INF, ZERO) = QNaN Indefinite
          }
          else {
            // mult(INF, non-zero) = INF
            inf_state = fp8_accumulate_inf(inf_state, (s1.sign != s2.sign) ? BX_FP8_NEG_INF : BX_FP8_POS_INF);
          }
        }

        // infinity cancellation which was not resolved within the same dword results in QNaN Indefinite
        // (behavior matches Intel SDE, the ISE pseudocode only states add(+INF, -INF) = QNaN Indefinite)
        if (inf_state == BX_FP8_CANCELLED_INF)
          nan = true;
      }

      if (nan) {
        tdst->row[m].vmm32u(n) = float32_default_nan;
        continue;
      }

      float32 tmpf32;
      if (inf_state != BX_FP8_NO_INF) {
        tmpf32 = (inf_state == BX_FP8_NEG_INF) ? float32_negative_inf : float32_positive_inf;
      }
      else {
        tmpf32 = convert_fixpoint128_scaled_to_fp32_ftz_rne(sop, -factor);
      }

      tdst->row[m].vmm32u(n) = f32_add(srcdest, tmpf32, &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBF8PS_TnnnTrmTreg(bxInstruction_c *i)  { tdpfp8ps_execute(i, true,  true);  }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPHF8PS_TnnnTrmTreg(bxInstruction_c *i)  { tdpfp8ps_execute(i, false, false); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBHF8PS_TnnnTrmTreg(bxInstruction_c *i) { tdpfp8ps_execute(i, true,  false); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPHBF8PS_TnnnTrmTreg(bxInstruction_c *i) { tdpfp8ps_execute(i, false, true);  }

#endif // BX_SUPPORT_AMX
