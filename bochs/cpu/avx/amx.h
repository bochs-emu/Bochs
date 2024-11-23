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

#ifndef BX_AMX_EXTENSIONS_H
#define BX_AMX_EXTENSIONS_H

#if BX_SUPPORT_AMX

#include "cpu/xmm.h"

#define BX_TILE_REGISTERS 8

struct AMX {
  AMX(): palette_id(0), start_row(0) {}

  unsigned palette_id; // 0 if tiles are not configured
  unsigned start_row;  // used to restart tile operations

  struct TILECFG {
    unsigned rows, bytes_per_row;
    TILECFG() { clear(); }
    void clear() { rows = bytes_per_row = 0; }
  } tilecfg[BX_TILE_REGISTERS];

  bool tiles_configured() const { return palette_id != 0; }

  void clear_tilecfg() {
    for (int i=0;i<BX_TILE_REGISTERS;i++) tilecfg[i].clear();
  }

  bool tile_valid(unsigned tile_num) const { return tilecfg[tile_num].rows != 0; }
  unsigned tile_num_rows(unsigned tile_num) const { return tilecfg[tile_num].rows; }
  unsigned tile_bytes_per_row(unsigned tile_num) const { return tilecfg[tile_num].bytes_per_row; }
  unsigned tile_dword_elements_per_row(unsigned tile_num) const { return tilecfg[tile_num].bytes_per_row / 4; }

  bool is_tile_used(unsigned tile_num) const { return tile_use_tracker & (1 << tile_num); }
  void set_tile_used(unsigned tile_num) { tile_use_tracker |= (1 << tile_num); }
  void clear_tile_used(unsigned tile_num) { tile_use_tracker &= ~(1 << tile_num); }

  void restart() { start_row = 0; }

  struct TILE {
#define BX_TILE_MAX_ROWS (16)
    bx_zmm_reg_t row[BX_TILE_MAX_ROWS];

    TILE() { clear(); }

    // clear upper part of a row (clears dwords limit..16)
    void zero_upper_row_data32(unsigned nrow, unsigned limit)
    {
      for (unsigned i=limit; i < 16; i++)
        row[nrow].vmm32u(i) = 0;
    }

    // clear 0..nrows
    void clear_rows(unsigned nrows) {
      for (unsigned i=0; i < nrows; i++)
        row[i].clear();
    }

    // clear nrows..MAX_ROWS
    void clear_upper_rows(unsigned nrows) {
      for (unsigned i=nrows; i < BX_TILE_MAX_ROWS; i++)
        row[i].clear();
    }

    void clear() { clear_rows(BX_TILE_MAX_ROWS); }
  } tile[BX_TILE_REGISTERS] BX_CPP_AlignN(64);

  unsigned tile_use_tracker;

  void clear_tiles() {
    for (int i=0;i<BX_TILE_REGISTERS;i++) tile[i].clear();
  }

  void clear() {
    palette_id = 0;
    start_row = 0;
    tile_use_tracker = 0;

    clear_tiles();
    clear_tilecfg();
  }
};

#endif // BX_SUPPORT_AMX

#endif

