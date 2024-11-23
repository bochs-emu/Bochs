/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::configure_tiles(bxInstruction_c *i, const BxPackedAvxRegister &tilecfg)
{
  Bit8u palette_id = tilecfg.vmmubyte(0);
  Bit8u start_row  = tilecfg.vmmubyte(1);

  if (palette_id == 0) {
    BX_CPU_THIS_PTR amx->clear();
    return true;
  }

  if (palette_id == 1) {
    if ((tilecfg.vmm64u(0) >> 16) != 0 || tilecfg.vmm64u(1) || tilecfg.vmm64u(4) || tilecfg.vmm64u(5) || tilecfg.vmm64u(7)) {
      BX_ERROR(("LDTILECFG: reserved bits set for palette_id=%d", palette_id));
      return false;
    }

    AMX::TILECFG tile[8];

    for (unsigned n=0; n < 8; n++) {
      tile[n].bytes_per_row = tilecfg.vmm16u(8+n);
      if (tile[n].bytes_per_row > 64) {
        BX_ERROR(("LDTILECFG: too many bytes_per_row for tile=%d in palette_id=%d", n, palette_id));
        return false;
      }        
      tile[n].rows = tilecfg.vmmubyte(48+n);
      if (tile[n].rows > 16) {
        BX_ERROR(("LDTILECFG: too many rows for tile=%d in palette_id=%d", n, palette_id));
        return false;
      }
      if ((tile[n].rows == 0 && tile[n].bytes_per_row != 0) || (tile[n].rows != 0 && tile[n].bytes_per_row == 0)) {
        BX_ERROR(("LDTILECFG: invalid empty tile=%d in palette_id=%d", n, palette_id));
        return false;
      }
    }

    BX_CPU_THIS_PTR amx->clear();
    BX_CPU_THIS_PTR amx->palette_id = 1;
    BX_CPU_THIS_PTR amx->start_row = start_row;
    for (unsigned n=0; n < 8; n++)
      BX_CPU_THIS_PTR amx->tilecfg[n] = tile[n];
    return true;
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

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STTILECFG(bxInstruction_c *i)
{
  xsave_tilecfg_state(i, BX_CPU_RESOLVE_ADDR_64(i));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILELOADD_TnnnMdq(bxInstruction_c *i)
{
  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("%s: SIB byte required", i->getIaOpcodeNameShort()));
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

  BX_CPU_THIS_PTR amx->tile[tile].clear_upper_rows(BX_CPU_THIS_PTR amx->start_row);

  Bit64u start_eaddr = BX_READ_64BIT_REG(i->sibBase()) + (Bit64s) i->displ32s();
  Bit64u stride = BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale();
  i->setVL(BX_VL512);

  for (unsigned row=BX_CPU_THIS_PTR amx->start_row; row < rows; row++) {
    BxPackedAvxRegister *data = &(BX_CPU_THIS_PTR amx->tile[tile].row[row]);

    Bit64u eaddr = start_eaddr + row * stride;
    if (dword_elements_per_row == 16) {
      read_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), data);
    }
    else {
      avx_masked_load32(i, eaddr, data, mask);

      for (unsigned n=dword_elements_per_row; n < 16; n++)
        data->vmm32u(n) = 0;
    }

    BX_CPU_THIS_PTR amx->start_row++;
  }

  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILESTORED_MdqTnnn(bxInstruction_c *i)
{
  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("%s: SIB byte required", i->getIaOpcodeNameShort()));
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
  Bit64u stride = BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale();

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

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp[2*n]   = f32_mulAdd(convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k)),
                                convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n)),   tmp[2*n],   0, &status);

        tmp[2*n+1] = f32_mulAdd(convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k+1)),
                                convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n+1)), tmp[2*n+1], 0, &status);
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

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        float32 s1r = convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k));                        // real
        float32 s2r = convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n));                        // real
        float32 s1i = convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k+1));                      // imaginary
        float32 s2i = convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n+1));                      // imaginary

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

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        float32 s1r = convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k));       // real
        float32 s2r = convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n));       // real
        float32 s1i = convert_ne_fp16_to_fp32(tsrc1->row[m].vmm16u(2*k+1));     // imaginary
        float32 s2i = convert_ne_fp16_to_fp32(tsrc2->row[k].vmm16u(2*n+1));     // imaginary

        tmp[2*n]   = f32_mulAdd(s1i, s2r, tmp[2*n],   0, &status);
        tmp[2*n+1] = f32_mulAdd(s1r, s2i, tmp[2*n+1], 0, &status);
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

BX_CPP_INLINE float32 f32_silence_snan(float32 a)
{
  if (f32_isNaN(a))
    a = convert_to_QNaN(a);
  return a;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TMMULTF32PS_TnnnTrmTreg(bxInstruction_c *i)
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
    float32 tmp[16]; // new empty array
    for (unsigned n=0; n < 16; n++) tmp[n] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        float32 a = fp32_convert_to_tf32(f32_silence_snan(tsrc1->row[m].vmm32u(k)));
        float32 b = fp32_convert_to_tf32(f32_silence_snan(tsrc2->row[k].vmm32u(n)));
        tmp[n] = f32_mulAdd(a, b, tmp[n], 0, &status);
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      tdst->row[m].vmm32u(n) = f32_add(tdst->row[m].vmm32u(n), tmp[n], &status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::tilemov_row(bxInstruction_c *i, bool immediate_form, BxPackedAvxRegister *dst)
{
  unsigned tile_src = i->src1();
  check_tile(i, tile_src);

  unsigned row, row_chunk;
  if (immediate_form) {
    row = i->Ib() & 0x3f;
    row_chunk = i->Ib() >> 6;
  }
  else {
    row = (unsigned) BX_READ_16BIT_REG(i->src2());
    row_chunk = (unsigned) (BX_READ_32BIT_REG(i->src2()) >> 16);
  }

  unsigned tile_num_rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src);
  if (row > tile_num_rows || row_chunk /* do not support multi-line rows yet */) {
    BX_ERROR(("%s: row=%d:%d out of range for tile %d", i->getIaOpcodeNameShort(), row, row_chunk, tile_src));
    exception(BX_GP_EXCEPTION, 0);
  }

  AMX::TILE *tsrc = &(BX_CPU_THIS_PTR amx->tile[tile_src]);
  *dst = tsrc->row[row];
}

#include "cpu/decoder/ia_opcodes.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILEMOVROW_VdqTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TILEMOVROW_VdqTrmIb, &dst);
  BX_WRITE_AVX_REG(i->dst(), dst)
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWD2PS_VpsTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWD2PS_VpsTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // converting the int32 source elements to fp32
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = i32_to_f32(dst.vmm32u(n), &status);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PHL_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PHL_VphTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert the fp32 source elements to fp16 and place them in low 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = (Bit32u) f32_to_f16(dst.vmm32u(n), &status);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PHH_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PHH_VphTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert the fp32 source elements to fp16 and place them in high 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = ((Bit32u) f32_to_f16(dst.vmm32u(n), &status)) << 16;

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PBF16L_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PBF16L_VphTrmIb, &dst);

  // convert the fp32 source elements to bf16 and place them in low 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = (Bit32u) convert_ne_fp32_to_bfloat16(dst.vmm32u(n));

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PBF16H_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PBF16H_VphTrmIb, &dst);

  // convert the fp32 source elements to bf16 and place them in high 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = ((Bit32u) convert_ne_fp32_to_bfloat16(dst.vmm32u(n))) << 16;

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

#endif // BX_SUPPORT_AMX
