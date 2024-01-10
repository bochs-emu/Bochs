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

  if (tile >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile)) {
    BX_ERROR(("%s: invalid tile %d", i->getIaOpcodeNameShort(), tile));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile);
  unsigned bytes_per_row = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile);

  if ((bytes_per_row & 0x3) != 0) {
    BX_ERROR(("%s: invalid tile %d bytes_per_row=%d", i->getIaOpcodeNameShort(), tile, bytes_per_row));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR amx->start_row >= rows) {
    BX_ERROR(("%s: invalid (start_row=%d) >= (rows=%d)", i->getIaOpcodeNameShort(), tile, BX_CPU_THIS_PTR amx->start_row, rows));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned elements_per_row = bytes_per_row / 4;
  Bit32u mask = (elements_per_row < 16) ? (BX_CONST64(1) << elements_per_row) - 1 : BX_CONST64(0xFFFF);

  BX_CPU_THIS_PTR amx->set_tile_used(tile);

  BX_CPU_THIS_PTR amx->tile[tile].clear_upper_rows(BX_CPU_THIS_PTR amx->start_row);

  Bit64u start_eaddr = BX_READ_64BIT_REG(i->sibBase()) + (Bit64s) i->displ32s();
  Bit64u stride = BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale();

  for (unsigned row=BX_CPU_THIS_PTR amx->start_row; row < rows; row++) {
    BxPackedAvxRegister *data = &(BX_CPU_THIS_PTR amx->tile[tile].row[row]);

    Bit64u eaddr = start_eaddr + row * stride;
    if (bytes_per_row == 64) {
      read_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), data);
    }
    else {
      avx_masked_load32(i, eaddr, data, mask);

      for (unsigned n=elements_per_row; n < 16; n++)
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

  if (tile >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile)) {
    BX_ERROR(("TILESTORED: invalid tile %d", tile));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile);
  unsigned bytes_per_row = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile);

  if ((bytes_per_row & 0x3) != 0) {
    BX_ERROR(("TILESTORED: invalid tile %d bytes_per_row=%d", tile, bytes_per_row));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR amx->start_row >= rows) {
    BX_ERROR(("TILESTORED: invalid (start_row=%d) >= (rows=%d)", tile, BX_CPU_THIS_PTR amx->start_row, rows));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned elements_per_row = bytes_per_row / 4;
  Bit32u mask = (elements_per_row < 16) ? (BX_CONST64(1) << elements_per_row) - 1 : BX_CONST64(0xFFFF);

  Bit64u start_eaddr = BX_READ_64BIT_REG(i->sibBase()) + (Bit64s) i->displ32s();
  Bit64u stride = BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale();

  for (unsigned row=BX_CPU_THIS_PTR amx->start_row; row < rows; row++) {
    BxPackedAvxRegister *data = &(BX_CPU_THIS_PTR amx->tile[tile].row[row]);

    Bit64u eaddr = start_eaddr + row * stride;
    if (bytes_per_row == 64)
      write_linear_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), data);
    else
      avx_masked_store32(i, eaddr, data, mask);

    BX_CPU_THIS_PTR amx->start_row++;
  }

  BX_CPU_THIS_PTR amx->restart();

  BX_NEXT_INSTR(i);
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
  if (tile_dst >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile_dst)) {
    BX_ERROR(("%s: invalid tile %d", i->getIaOpcodeNameShort(), tile_dst));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (tile_src1 >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile_src1)) {
    BX_ERROR(("%s: invalid tile %d", i->getIaOpcodeNameShort(), tile_src1));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (tile_src2 >= BX_TILE_REGISTERS || ! BX_CPU_THIS_PTR amx->tile_valid(tile_src2)) {
    BX_ERROR(("%s: invalid tile %d", i->getIaOpcodeNameShort(), tile_src2));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned rows[3];
  unsigned bytes_per_row[3];

  rows[0] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  bytes_per_row[0] = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_dst);
  rows[1] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src1);
  bytes_per_row[1] = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_src1);
  rows[2] = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);
  bytes_per_row[2] = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_src2);

  // #UD if srcdest.colbytes mod 4 != 0
  // #UD if src1.colbytes mod 4 != 0
  // #UD if src2.colbytes mod 4 != 0
  for (unsigned j=0; j<3; j++) {
    if ((bytes_per_row[j] & 0x3) != 0) {
      BX_ERROR(("%s: invalid tile bytes_per_row=%d", i->getIaOpcodeNameShort(), bytes_per_row[j]));
      exception(BX_UD_EXCEPTION, 0);
    }
  }

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned n = bytes_per_row[0] / 4;
  unsigned m = rows[1];
  unsigned k = rows[2];

  // #UD if srcdest.colbytes != src2.colbytes (n)
  // #UD if srcdest.rows != src1.rows (m)
  // #UD if src1.colbytes / 4 != src2.rows (k)
  if (n != (bytes_per_row[2] / 4) || m != rows[0] || k != (bytes_per_row[1] / 4)) {
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

#include "cpu/simd_vnni.h"

#define HANDLE_AMX_INT8_3OP(HANDLER, func)                                             \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                  \
  {                                                                                    \
    unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();        \
    check_tiles(i, tile_dst, tile_src1, tile_src2);                                    \
                                                                                       \
    /*     R   C            */                                                         \
    /* A = m x k (tsrc1)    */                                                         \
    /* B = k x n (tsrc2)    */                                                         \
    /* C = m x n (tsrcdest) */                                                         \
    unsigned max_n = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_dst) / 4;            \
    unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);                     \
    unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);                    \
                                                                                       \
    AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);                         \
    AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);                        \
    AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);                        \
                                                                                       \
    for (unsigned m=0; m < max_m; m++) {                                               \
      BxPackedAvxRegister* tmp = &(tdst->row[m]);                                      \
      for (unsigned k=0; k < max_k; k++) {                                             \
        for (unsigned n=0; n < max_n; n+=4) {                                          \
          (func)(&(tmp->vmm128(n/4)),                                                  \
                 &(tsrc1->row[m].vmm128(n/4)), &(tsrc2->row[m].vmm128(n/4)));          \
        }                                                                              \
      }                                                                                \
      tdst->zero_upper_row_data32(m, max_n);                                           \
    }                                                                                  \
                                                                                       \
    BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);                                      \
    BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);                       \
    BX_CPU_THIS_PTR amx->restart();                                                    \
    BX_NEXT_INSTR(i);                                                                  \
  }

HANDLE_AMX_INT8_3OP(TDPBSSD_TnnnTrmTreg, xmm_pdpbssd)
HANDLE_AMX_INT8_3OP(TDPBSUD_TnnnTrmTreg, xmm_pdpbsud)
HANDLE_AMX_INT8_3OP(TDPBUSD_TnnnTrmTreg, xmm_pdpbusd)
HANDLE_AMX_INT8_3OP(TDPBUUD_TnnnTrmTreg, xmm_pdpbuud)

#include "bf16.h"

extern float_status_t prepare_ne_softfloat_status_helper();

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TDPBF16PS_TnnnTrmTreg(bxInstruction_c *i)
{
  unsigned tile_dst = i->dst(), tile_src1 = i->src1(), tile_src2 = i->src2();
  check_tiles(i, tile_dst, tile_src1, tile_src2);

  //     R   C
  // A = m x k (tsrc1)
  // B = k x n (tsrc2)
  // C = m x n (tsrcdest)
  unsigned max_n = BX_CPU_THIS_PTR amx->tile_bytes_per_row(tile_dst) / 4;
  unsigned max_m = BX_CPU_THIS_PTR amx->tile_num_rows(tile_dst);
  unsigned max_k = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src2);

  AMX::TILE *tdst  = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);
  AMX::TILE *tsrc1 = &(BX_CPU_THIS_PTR amx->tile[tile_src1]);
  AMX::TILE *tsrc2 = &(BX_CPU_THIS_PTR amx->tile[tile_src2]);

  float_status_t status = prepare_ne_softfloat_status_helper();

  for (unsigned m=0; m < max_m; m++) {
    float32 tmp[32]; // new empty array
    for (unsigned n=0; n < 32; n++) tmp[32] = 0;

    for (unsigned k=0; k < max_k; k++) {
      for (unsigned n=0; n < max_n; n++) {
        tmp[2*n]   = float32_fmadd(convert_bfloat16_to_fp32(tsrc1->row[m].vmm16u(2*k)),
                                          convert_bfloat16_to_fp32(tsrc2->row[k].vmm16u(2*n)),   tmp[2*n],   status);

        tmp[2*n+1] = float32_fmadd(convert_bfloat16_to_fp32(tsrc1->row[m].vmm16u(2*k+1)),
                                          convert_bfloat16_to_fp32(tsrc2->row[k].vmm16u(2*n+1)), tmp[2*n+1], status);
      }
    }

    for (unsigned n=0; n < max_n; n++) {
      float32 tmpf32 = float32_add(tmp[2*n], tmp[2*n+1], status);
      tdst->row[m].vmm32u(n) = float32_add(tdst->row[m].vmm32u(n), tmpf32, status);
    }

    tdst->zero_upper_row_data32(m, max_n);
  }

  BX_CPU_THIS_PTR amx->set_tile_used(tile_dst);
  BX_CPU_THIS_PTR amx->tile[tile_dst].clear_upper_rows(max_m);
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

#endif // BX_SUPPORT_AMX
