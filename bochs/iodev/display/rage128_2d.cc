/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation: 2D GUI engine.
//
//  Datapath interpreter: solid / pattern paint, screen-to-screen blit
//  (plain, keyed, stretched), Bresenham lines, mono expansion and host
//  data, driven either by direct register writes or by PM4 type-3 packets.
//  Ported from the 86Box Rage 128 Pro emulation (vid_ati_rage128_2d.c and
//  vid_ati_rage128_mem.c).
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_RAGE128

#include "vgacore.h"
#include "pci.h"
#include "ddc.h"
#include "rage128_regs.h"
#include "rage128.h"

#define LOG_THIS BX_RAGE128_THIS

// ---------------------------------------------------------------------
// Datapath register accessors. The GMC is a write-through alias of
// DP_MIX / DP_DATATYPE; executors consume the active registers.
// ---------------------------------------------------------------------

#define DP_DST_DT()   ((dp_datatype >> 8) & 0xf)
#define DP_BRUSH_DT() (dp_datatype & 0xf)
#define DP_SRC_DT()   ((dp_datatype >> 16) & 3)
#define DP_LSB_FIRST() ((dp_datatype >> 30) & 1)
#define DP_ROP3()     ((Bit8u)((dp_mix >> 16) & 0xff))
#define DP_SRC_SOURCE() ((dp_mix >> 8) & 7)

// Destination datatype -> bytes per pixel
static int r2d_bpp(Bit32u dst_datatype)
{
  switch (dst_datatype) {
    case 3: case 4: case 15: return 2;
    case 5: return 3;
    case 6: case 14: return 4;
    case 11: case 12: return 2;
    default: return 1;
  }
}

// Row byte stride from a latched pitch (pixels/8 units, bytes at 24 bpp)
static BX_CPP_INLINE Bit32u r2d_stride(Bit32u pitch, int bpp)
{
  return (bpp == 3) ? pitch : pitch * (Bit32u)bpp;
}

// ROP3: evaluate any of the 256 ternary raster ops bitwise
Bit32u rage128_rop3(Bit8u rop, Bit32u p, Bit32u s, Bit32u d)
{
  Bit32u r = 0;
  if (rop & 0x01) r |= ~p & ~s & ~d;
  if (rop & 0x02) r |= ~p & ~s &  d;
  if (rop & 0x04) r |= ~p &  s & ~d;
  if (rop & 0x08) r |= ~p &  s &  d;
  if (rop & 0x10) r |=  p & ~s & ~d;
  if (rop & 0x20) r |=  p & ~s &  d;
  if (rop & 0x40) r |=  p &  s & ~d;
  if (rop & 0x80) r |=  p &  s &  d;
  return r;
}

static BX_CPP_INLINE void r2d_row_fill(Bit8u *dst, const Bit8u *px, int bpp, int count)
{
  Bit32u total = (Bit32u)count * (Bit32u)bpp;
  Bit32u done;
  if (bpp == 1) {
    memset(dst, px[0], (size_t)count);
    return;
  }
  memcpy(dst, px, (size_t)bpp);
  for (done = (Bit32u)bpp; done < total;) {
    Bit32u chunk = done < total - done ? done : total - done;
    memcpy(dst + done, dst, (size_t)chunk);
    done += chunk;
  }
}

// DP_WRITE_MASK gates the dst bitplanes a 2D op may modify (per pixel byte lane)
static BX_CPP_INLINE void r2d_store(Bit8u *dp, Bit8u v, Bit8u mb)
{
  *dp = (Bit8u)((v & mb) | (*dp & ~mb));
}

// Auxiliary scissor test, 3D lane semantics
bool rage128_aux_sc_pass(Bit32u cntl, const Bit32u rect[3][4], int x, int y)
{
  bool have_add = false, in_add = false;
  for (int i = 0; i < 3; i++) {
    if (!(cntl & (1u << (i * 2))))
      continue;
    bool in = (x >= rage128_sx14(rect[i][0])) && (x <= rage128_sx14(rect[i][1])) &&
              (y >= rage128_sx14(rect[i][2])) && (y <= rage128_sx14(rect[i][3]));
    if (cntl & (2u << (i * 2))) {
      if (in) return false;
    } else {
      have_add = true;
      if (in) in_add = true;
    }
  }
  return have_add ? in_add : true;
}

// 2D engine twin: only subtractive rects carve
bool rage128_aux_sc_pass_2d(Bit32u cntl, const Bit32u rect[3][4], int x, int y)
{
  for (int i = 0; i < 3; i++) {
    if (!(cntl & (1u << (i * 2))) || !(cntl & (2u << (i * 2))))
      continue;
    if ((x >= rage128_sx14(rect[i][0])) && (x <= rage128_sx14(rect[i][1])) &&
        (y >= rage128_sx14(rect[i][2])) && (y <= rage128_sx14(rect[i][3])))
      return false;
  }
  return true;
}

static bool r2d_aux_accepts_rect(Bit32u cntl, const Bit32u rect[3][4], int x0, int y0, int x1, int y1)
{
  for (int i = 0; i < 3; i++) {
    if (!(cntl & (1u << (i * 2))) || !(cntl & (2u << (i * 2))))
      continue;
    if ((x0 <= rage128_sx14(rect[i][1])) && (x1 >= rage128_sx14(rect[i][0])) &&
        (y0 <= rage128_sx14(rect[i][3])) && (y1 >= rage128_sx14(rect[i][2])))
      return false;
  }
  return true;
}

// ---------------------------------------------------------------------
// Colour compare (CLR_CMP_*) evaluated per pixel at the output stage
// ---------------------------------------------------------------------

int bx_rage128_c::r2d_ccmp_setup(bool has_src, int sbpp, int dbpp, struct r128_ccmp *cc)
{
  int sel = (clr_cmp_cntl >> 24) & 3;
  int fs = clr_cmp_cntl & 7;
  int fd = (clr_cmp_cntl >> 8) & 7;
  Bit32u spm = (sbpp >= 4) ? 0xffffffff : ((1u << (sbpp * 8)) - 1u);
  Bit32u dpm = (dbpp >= 4) ? 0xffffffff : ((1u << (dbpp * 8)) - 1u);

  cc->fn_src = ((fs == 1) || (fs == 4) || (fs == 5) || (fs == 7)) ? fs : 0;
  cc->fn_dst = ((fd == 1) || (fd == 4) || (fd == 5)) ? fd : 0;
  cc->src_on = has_src && ((sel == 1) || (sel == 2));
  cc->dst_on = ((sel == 0) || (sel == 2));
  cc->smask = clr_cmp_mask & spm;
  cc->dmask = clr_cmp_mask & dpm;
  cc->key_src = clr_cmp_clr_src & cc->smask;
  cc->key_dst = clr_cmp_clr_dst & cc->dmask;
  cc->flip = dp_src_frgd_clr & dpm;
  return (cc->src_on && (cc->fn_src != 0)) || (cc->dst_on && (cc->fn_dst != 0));
}

// 0 = suppress the store, 1 = store the datapath result, 2 = store d ^ flip
static BX_CPP_INLINE int r2d_ccmp_px(const struct r128_ccmp *cc, Bit32u s, Bit32u d)
{
  int flip = 0;
  if (cc->src_on) {
    bool eq = ((s & cc->smask) == cc->key_src);
    if ((cc->fn_src == 1) || ((cc->fn_src == 4) && !eq) || ((cc->fn_src == 5) && eq))
      return 0;
    if (cc->fn_src == 7) {
      if (!eq) return 0;
      flip = 1;
    }
  }
  if (cc->dst_on) {
    bool eq = ((d & cc->dmask) == cc->key_dst);
    if ((cc->fn_dst == 1) || ((cc->fn_dst == 4) && !eq) || ((cc->fn_dst == 5) && eq))
      return 0;
  }
  return flip ? 2 : 1;
}

// ---------------------------------------------------------------------
// Card-space resolver: local VRAM below 32 MB, AGP system memory above
// ---------------------------------------------------------------------

bool bx_rage128_c::card_read_block(Bit32u vm, Bit8u *dst, Bit32u len)
{
  if (!r128_card_is_agp(vm)) {
    Bit32u loc = len;
    if ((Bit64u)(vm & (R128_CARD_AGP_HALF - 1u)) + len > R128_CARD_AGP_HALF)
      loc = R128_CARD_AGP_HALF - (vm & (R128_CARD_AGP_HALF - 1u));
    for (Bit32u o = 0; o < loc; o++)
      dst[o] = BX_RAGE128_THIS s.memory[(vm + o) & vram_mask];
    if (loc == len)
      return true;
    return pm4_bus_read_block(R128_CARD_AGP_HALF, dst + loc, len - loc);
  }
  return pm4_bus_read_block(vm, dst, len);
}

bool bx_rage128_c::card_write_block(Bit32u vm, const Bit8u *src, Bit32u len)
{
  if (!len)
    return true;
  if (!r128_card_is_agp(vm)) {
    Bit32u loc = len;
    if ((Bit64u)(vm & (R128_CARD_AGP_HALF - 1u)) + len > R128_CARD_AGP_HALF)
      loc = R128_CARD_AGP_HALF - (vm & (R128_CARD_AGP_HALF - 1u));
    for (Bit32u o = 0; o < loc; o++)
      BX_RAGE128_THIS s.memory[(vm + o) & vram_mask] = src[o];
    vram_dirty(vm & vram_mask, loc);
    if (loc == len)
      return true;
    return pm4_bus_write_block(R128_CARD_AGP_HALF, src + loc, len - loc);
  }
  return pm4_bus_write_block(vm, src, len);
}

// Copy between a tiled card surface and a linear buffer (dir false = gather)
bool bx_rage128_c::card_copy_tiled(Bit32u tbase, Bit32u tpitch, Bit32u lin, Bit8u *buf, Bit32u len, bool dir)
{
  while (len) {
    Bit32u y = lin / tpitch;
    Bit32u xb = lin % tpitch;
    Bit32u run = 64u - (xb & 63u);
    if (run > tpitch - xb) run = tpitch - xb;
    if (run > len) run = len;
    Bit32u ca = tbase + r128_tile_off(xb, y, tpitch);
    if (dir ? !card_write_block(ca, buf, run) : !card_read_block(ca, buf, run))
      return false;
    buf += run;
    lin += run;
    len -= run;
  }
  return true;
}

bool bx_rage128_c::span_stage_grow(rage128_span_stage_t *st, Bit32u need)
{
  if (need <= st->cap)
    return true;
  Bit32u ncap = st->cap ? st->cap : (1u << 20);
  while (ncap < need) ncap <<= 1;
  Bit8u *na = (Bit8u*)realloc(st->arena, ncap);
  if (na == NULL)
    return false;
  st->arena = na;
  st->cap = ncap;
  return true;
}

bool bx_rage128_c::span_stage_acquire(rage128_span_stage_t *st, Bit32u vm, Bit32u extent)
{
  if (!extent)
    return false;
  if (st->active && st->tiled)
    span_stage_writeback(st);
  if (st->active && (st->vm == vm) && (st->len >= extent))
    return true;
  if (st->active && (st->vm == vm)) {
    Bit32u old_len = st->len;
    if (!span_stage_grow(st, extent))
      return false;
    if (!card_read_block(vm + old_len, st->arena + old_len, extent - old_len))
      return false;
    st->len = extent;
    return true;
  }
  if (st->active)
    span_stage_writeback(st);
  if (!span_stage_grow(st, extent))
    return false;
  if (!card_read_block(vm, st->arena, extent))
    return false;
  st->vm = vm;
  st->len = extent;
  st->active = true;
  st->tiled = false;
  return true;
}

bool bx_rage128_c::span_stage_acquire_tiled(rage128_span_stage_t *st, Bit32u tbase, Bit32u tpitch, Bit32u extent)
{
  if (!extent)
    return false;
  if (st->active && (!st->tiled || (st->tbase != tbase) || (st->tpitch != tpitch)))
    span_stage_writeback(st);
  if (st->active && (st->len >= extent))
    return true;
  if (st->active) {
    Bit32u old_len = st->len;
    if (!span_stage_grow(st, extent))
      return false;
    if (!card_copy_tiled(tbase, tpitch, old_len, st->arena + old_len, extent - old_len, false))
      return false;
    st->len = extent;
    return true;
  }
  if (!span_stage_grow(st, extent))
    return false;
  if (!card_copy_tiled(tbase, tpitch, 0, st->arena, extent, false))
    return false;
  st->vm = tbase;
  st->len = extent;
  st->active = true;
  st->tiled = true;
  st->tbase = tbase;
  st->tpitch = tpitch;
  return true;
}

void bx_rage128_c::span_stage_writeback(rage128_span_stage_t *st)
{
  if (!st->active)
    return;
  if (st->arena && st->len) {
    if (st->tiled)
      card_copy_tiled(st->tbase, st->tpitch, st->vm - st->tbase, st->arena, st->len, true);
    else
      card_write_block(st->vm, st->arena, st->len);
  }
  st->active = false;
  st->tiled = false;
}

bool bx_rage128_c::surf_map(r128_surf_t *s, rage128_span_stage_t *st, Bit32u lo, Bit32u len)
{
  Bit32u m;
  if (!len) len = 1;
  if (!r128_card_is_agp(lo) && !r128_card_is_agp(lo + len - 1)) {
    s->base = BX_RAGE128_THIS s.memory;
    s->rel = 0;
    s->mask = vram_mask;
    s->st = NULL;
    return true;
  }
  if ((Bit64u)(lo & (2u * R128_CARD_AGP_HALF - 1u)) + len > 2u * R128_CARD_AGP_HALF)
    len = 2u * R128_CARD_AGP_HALF - (lo & (2u * R128_CARD_AGP_HALF - 1u));
  if (!span_stage_acquire(st, lo, len))
    return false;
  m = 1;
  while (m < len) m <<= 1;
  s->base = st->arena;
  s->rel = lo;
  s->mask = m - 1u;
  s->st = st;
  return true;
}

bool bx_rage128_c::surf_map_tiled(r128_surf_t *s, rage128_span_stage_t *st, Bit32u tbase, Bit32u tpitch, Bit32u lo, Bit32u len)
{
  Bit32u m;
  if (!len) len = 1;
  if ((Bit64u)((lo - tbase) & (2u * R128_CARD_AGP_HALF - 1u)) + len > 2u * R128_CARD_AGP_HALF)
    len = 2u * R128_CARD_AGP_HALF - ((lo - tbase) & (2u * R128_CARD_AGP_HALF - 1u));
  if (st->active)
    span_stage_writeback(st);
  if (!span_stage_grow(st, len))
    return false;
  if (!card_copy_tiled(tbase, tpitch, lo - tbase, st->arena, len, false))
    return false;
  st->vm = lo;
  st->len = len;
  st->active = true;
  st->tiled = true;
  st->tbase = tbase;
  st->tpitch = tpitch;
  m = 1;
  while (m < len) m <<= 1;
  s->base = st->arena;
  s->rel = lo;
  s->mask = m - 1u;
  s->st = st;
  return true;
}

void bx_rage128_c::r2d_surf_commit(r128_surf_t *s)
{
  if (s->st) {
    span_stage_writeback(s->st);
    s->st = NULL;
  }
}

void bx_rage128_c::r2d_surf_release(r128_surf_t *s)
{
  if (s->st) {
    s->st->active = false;
    s->st->tiled = false;
    s->st = NULL;
  }
}

void bx_rage128_c::r2d_mark_dirty(const r128_surf_t *s, Bit32u addr, Bit32u len)
{
  if (s->st)
    return;
  vram_dirty(addr & vram_mask, len);
}

// ---------------------------------------------------------------------
// Engine state
// ---------------------------------------------------------------------

void bx_rage128_c::r2d_reset(void)
{
  dp_gui_master_cntl = 0;
  dp_brush_frgd_clr = 0;
  dp_brush_bkgd_clr = 0;
  brush_yx = 0;
  dp_src_frgd_clr = 0;
  dp_src_bkgd_clr = 0;
  dp_cntl = 0;
  dp_datatype = 0;
  dp_mix = 0;
  sc_top_left = 0;
  sc_bottom_right = 0x1fff1fff;
  default_sc_bottom_right = 0x1fff1fff;
  default_offset = 0;
  default_pitch = 0;
  dp_write_mask = 0xffffffff;
  clr_cmp_cntl = 0;
  clr_cmp_clr_src = 0;
  clr_cmp_clr_dst = 0;
  clr_cmp_mask = 0;
  aux_sc_cntl = 0;
  memset(aux_sc_rect, 0, sizeof(aux_sc_rect));
  src_offset = dst_offset = 0;
  src_pitch = dst_pitch = 0;
  src_pitch_reg = dst_pitch_reg = 0;
  src_sc_right = src_sc_bottom = 0;
  gui_dst_x = gui_dst_y = gui_src_x = gui_src_y = 0;
  gui_dst_w = gui_dst_h = 0;
  bres_err = bres_inc = bres_dec = bres_lnth = 0;
  dp_cntl_line = 0;
  memset(grad_start, 0, sizeof(grad_start));
  memset(grad_slope_x, 0, sizeof(grad_slope_x));
  memset(grad_slope_y, 0, sizeof(grad_slope_y));
  grad_valid = false;
  scale_scr_height_width = 0;
  hostdata_active = false;
  hostdata_ndw = 0;
}

// Apply a DP_GUI_MASTER_CNTL image
void bx_rage128_c::r2d_apply_gmc(Bit32u gmc)
{
  dp_gui_master_cntl = gmc;
  if (!(gmc & RAGE128_GMC_3D_FCN_EN))
    grad_valid = false;
  if (!(gmc & RAGE128_GMC_SRC_PITCH_OFFSET_LEAVE)) {
    src_offset = (default_offset & 0x03fffff0);
    src_pitch = (default_pitch & 0x3ff) * 8;
    src_pitch_reg = (default_pitch & 0x000103ff);
  }
  if (!(gmc & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE)) {
    dst_offset = (default_offset & 0x03fffff0);
    dst_pitch = (default_pitch & 0x3ff) * 8;
    dst_pitch_reg = (default_pitch & 0x000103ff) | (dst_pitch_reg & 0x00060000);
  }
  if (!(gmc & RAGE128_GMC_SRC_CLIP_LEAVE)) {
    src_sc_right = default_sc_bottom_right & 0x3fff;
    src_sc_bottom = (default_sc_bottom_right >> 16) & 0x3fff;
  }
  if (!(gmc & RAGE128_GMC_DST_CLIP_LEAVE)) {
    sc_top_left = 0;
    sc_bottom_right = default_sc_bottom_right;
  }
  if (gmc & RAGE128_GMC_WR_MSK_DIS) {
    dp_write_mask = 0xffffffff;
    clr_cmp_mask = 0xffffffff;
  }
  if (gmc & RAGE128_GMC_CLR_CMP_CNTL_DIS)
    clr_cmp_cntl &= ~0x00000707;
  if (gmc & RAGE128_GMC_AUX_CLIP_DIS)
    aux_sc_cntl &= ~RAGE128_AUX_SC_ENB_MASK;

  dp_mix = (dp_mix & ~0x00ff0700) | (gmc & 0x00ff0000) | ((Bit32u)RAGE128_GMC_SRC_SOURCE(gmc) << 8);
  dp_datatype = (dp_datatype & ~0xc0030f0f) |
                ((Bit32u)RAGE128_GMC_DST_DATATYPE(gmc) << 8) |
                (Bit32u)RAGE128_GMC_BRUSH_TYPE(gmc) |
                ((Bit32u)RAGE128_GMC_SRC_DATATYPE(gmc) << 16) |
                (((gmc >> 14) & 1u) << 30) | (((gmc >> 15) & 1u) << 31);
  dp_cntl |= RAGE128_DP_CNTL_DST_X_DIR | RAGE128_DP_CNTL_DST_Y_DIR | RAGE128_DP_CNTL_POLY_LINE;
}

// SRC/DST_PITCH_OFFSET packing: offset[20:0] in 32-byte units, pitch[30:21] in 8-pixel units
void bx_rage128_c::r2d_set_pitch_offset(bool is_dst, Bit32u val)
{
  Bit32u off = (val & 0x001fffff) << 5;
  Bit32u pitch = ((val >> 21) & 0x3ff) * 8;
  if (is_dst) {
    dst_offset = off;
    dst_pitch = pitch;
    dst_pitch_reg = ((val >> 21) & 0x3ff) | (((val >> 31) & 1u) << 16) | (dst_pitch_reg & 0x00060000);
  } else {
    src_offset = off;
    src_pitch = pitch;
    src_pitch_reg = ((val >> 21) & 0x3ff) | (((val >> 31) & 1u) << 16);
  }
}

void bx_rage128_c::r2d_clip(int *x0, int *y0, int *x1, int *y1)
{
  *x0 = sc_top_left & 0x3fff;
  *y0 = (sc_top_left >> 16) & 0x3fff;
  *x1 = sc_bottom_right & 0x3fff;
  *y1 = (sc_bottom_right >> 16) & 0x3fff;
}

Bit32u bx_rage128_c::r2d_dst_addr(int x, int y, int bpp)
{
  return dst_offset + ((Bit32u)y * r2d_stride(dst_pitch, bpp) + (Bit32u)x * (Bit32u)bpp);
}

Bit32u bx_rage128_c::r2d_src_addr(int x, int y, int bpp)
{
  return src_offset + ((Bit32u)y * r2d_stride(src_pitch, bpp) + (Bit32u)x * (Bit32u)bpp);
}

// Resolve the dst/src window for a 2D op touching rows [y0..y1] up to column x1
bool bx_rage128_c::r2d_map_span(r128_surf_t *s, rage128_span_stage_t *st, Bit32u surf_base,
                                Bit32u pitch_px, int bpp, int y0, int y1, int x1, bool is_dst)
{
  Bit32u lo, len;
  bool tiled = is_dst
    ? ((surf_base == dst_offset) && (pitch_px == dst_pitch) &&
       r128_tiled_ok((dst_pitch_reg >> 16) & 1u, r2d_stride(dst_pitch, bpp)))
    : ((surf_base == src_offset) && (pitch_px == src_pitch) &&
       r128_tiled_ok((src_pitch_reg >> 16) & 1u, r2d_stride(src_pitch, bpp)));

  if (y0 < 0) y0 = 0;
  if ((y1 < y0) || (x1 < 0)) {
    s->base = BX_RAGE128_THIS s.memory;
    s->rel = 0;
    s->mask = vram_mask;
    s->st = NULL;
    return true;
  }
  lo = surf_base + (Bit32u)y0 * r2d_stride(pitch_px, bpp);
  len = (Bit32u)(y1 - y0) * r2d_stride(pitch_px, bpp) + ((Bit32u)x1 + 1u) * (Bit32u)bpp;
  if (tiled)
    return surf_map_tiled(s, st, surf_base, r2d_stride(pitch_px, bpp), lo, len);
  return surf_map(s, st, lo, len);
}

// Pack an 8-bit-per-channel RGB triple into the current DST pixel format
Bit32u bx_rage128_c::r2d_pack_rgb(int r, int g, int b)
{
  switch (DP_DST_DT()) {
    case 4:  return ((Bit32u)(r >> 3) << 11) | ((Bit32u)(g >> 2) << 5) | (Bit32u)(b >> 3);
    case 3:
    case 15: return 0x8000u | ((Bit32u)(r >> 3) << 10) | ((Bit32u)(g >> 3) << 5) | (Bit32u)(b >> 3);
    case 5:  return ((Bit32u)r << 16) | ((Bit32u)g << 8) | (Bit32u)b;
    case 6:  return 0xff000000u | ((Bit32u)r << 16) | ((Bit32u)g << 8) | (Bit32u)b;
    default: return dp_brush_frgd_clr;
  }
}

// ---------------------------------------------------------------------
// Executors
// ---------------------------------------------------------------------

// Fill one rectangle through the current ROP3. mono8x8 = 8x8 mono pattern
// (byte n = row n, MSB first), col8x8 = 64 dst-format pixels; both NULL
// for solid. Patterns are screen-aligned.
void bx_rage128_c::r2d_paint_rect(int x, int y, int w, int h, const Bit8u *mono8x8, const Bit8u *col8x8, bool mono_la)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca;
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (x < cx0) { w -= cx0 - x; x = cx0; }
  if (y < cy0) { h -= cy0 - y; y = cy0; }
  if (x + w - 1 > cx1) w = cx1 - x + 1;
  if (y + h - 1 > cy1) h = cy1 - y + 1;
  if ((w <= 0) || (h <= 0))
    return;
  cca = r2d_ccmp_setup(false, bpp, bpp, &cc);

  bool aux_ok = !aux_on || r2d_aux_accepts_rect(aux_sc_cntl, aux_sc_rect, x, y, x + w - 1, y + h - 1);
  bool fast = aux_ok && (wmask == 0xffffffff) && (rop == 0xf0) && !mono8x8 && !col8x8 && !cca;
  Bit8u fpx[4];
  if (fast)
    for (int b = 0; b < bpp; b++)
      fpx[b] = (Bit8u)((dp_brush_frgd_clr >> ((b & 3) * 8)) & 0xff);

  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, y, y + h - 1, x + w - 1, true))
    return;

  for (int row = 0; row < h; row++) {
    Bit32u addr = r2d_dst_addr(x, y + row, bpp);
    if (fast) {
      Bit8u *run = r128_surf_run(&sd, addr, (Bit32u)w * bpp);
      if (run) {
        r2d_row_fill(run, fpx, bpp, w);
        r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
        continue;
      }
    }
    for (int col = 0; col < w; col++) {
      Bit32u pat = dp_brush_frgd_clr;
      const Bit8u *patpx = NULL;
      Bit32u a = addr + (Bit32u)col * bpp;

      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x + col, y + row))
        continue;
      if (mono8x8) {
        int set = (mono8x8[(y + row) & 7] >> (7 - ((x + col) & 7))) & 1;
        if (!set && mono_la)
          continue;
        pat = set ? dp_brush_frgd_clr : dp_brush_bkgd_clr;
      } else if (col8x8) {
        patpx = &col8x8[(((y + row) & 7) * 8 + ((x + col) & 7)) * bpp];
      }
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        if (!r2d_ccmp_px(&cc, 0, dpx))
          continue;
      }
      for (int b = 0; b < bpp; b++) {
        Bit8u *dp = r128_surf_at(&sd, a + (Bit32u)b);
        Bit8u pb = patpx ? patpx[b] : ((pat >> ((b & 3) * 8)) & 0xff);
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, 0, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
  }
  r2d_surf_commit(&sd);
}

// Line segment (Bresenham) through the ROP3 P operand and the scissor.
// Brush types 6/7 carry a 32x1 line pattern.
void bx_rage128_c::r2d_line(int x0, int y0, int x1, int y1, Bit32u pat32, bool pat_en, bool pat_la, int *phase)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(false, bpp, bpp, &cc);
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;
  int dx = x1 > x0 ? x1 - x0 : x0 - x1;
  int dy = y1 > y0 ? y1 - y0 : y0 - y1;
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;
  int bx1, by0, by1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  by0 = (y0 < y1 ? y0 : y1); if (by0 < cy0) by0 = cy0;
  by1 = (y0 > y1 ? y0 : y1); if (by1 > cy1) by1 = cy1;
  bx1 = (x0 > x1 ? x0 : x1); if (bx1 > cx1) bx1 = cx1;

  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, by0, by1, bx1, true))
    return;

  for (;;) {
    int set = 1;
    bool at_end = (x0 == x1) && (y0 == y1);

    if (at_end && !(dp_cntl & RAGE128_DP_CNTL_DST_LAST_PEL))
      break;
    if (pat_en) {
      set = (pat32 >> (31 - (*phase & 31))) & 1;
      (*phase)++;
    }
    if ((set || !pat_la) && (x0 >= cx0) && (x0 <= cx1) && (y0 >= cy0) && (y0 <= cy1) &&
        (!aux_on || rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x0, y0))) {
      Bit32u a = r2d_dst_addr(x0, y0, bpp);
      Bit32u pat = set ? dp_brush_frgd_clr : dp_brush_bkgd_clr;
      int cok = 1;
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        cok = r2d_ccmp_px(&cc, 0, dpx);
      }
      for (int b = 0; cok && (b < bpp); b++) {
        Bit8u *dp = r128_surf_at(&sd, a + (Bit32u)b);
        Bit8u pb = (pat >> ((b & 3) * 8)) & 0xff;
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, 0, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
      r2d_mark_dirty(&sd, a, bpp);
    }
    if (at_end)
      break;
    int e2 = err * 2;
    if (e2 > -dy) { err -= dy; x0 += sx; }
    if (e2 <  dx) { err += dx; y0 += sy; }
  }
  r2d_surf_commit(&sd);
}

// Register-triggered Bresenham line (XFree86 / Linux 2D path)
void bx_rage128_c::r2d_bres_line(Bit32u dir)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  bool ymajor = (dir & RAGE128_DP_LINE_Y_MAJOR) != 0;
  int xstep = (dir & RAGE128_DP_LINE_X_DIR) ? 1 : -1;
  int ystep = (dir & RAGE128_DP_LINE_Y_DIR) ? 1 : -1;
  bool zero_pos = (!ymajor && (ystep < 0)) || (ymajor && (xstep < 0));
  int n = (int)(bres_lnth & 0x3fff);
  Bit32s inc = (Bit32s)(bres_inc << 12) >> 12;
  Bit32s dec = (Bit32s)(bres_dec << 12) >> 12;
  Bit32s e = (Bit32s)(bres_err << 12) >> 12;
  int x = gui_dst_x, y = gui_dst_y;
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(false, bpp, bpp, &cc);
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;
  int xa, ya, xb, yb;

  if (n <= 0)
    return;
  r2d_clip(&cx0, &cy0, &cx1, &cy1);

  // Pass 1: bounding box of the walk
  {
    int px = x, py = y;
    Bit32s pe = e;
    xa = xb = px;
    ya = yb = py;
    for (int i = 0; i < n; i++) {
      if (px < xa) xa = px;
      if (px > xb) xb = px;
      if (py < ya) ya = py;
      if (py > yb) yb = py;
      if (zero_pos ? (pe >= 0) : (pe > 0)) { if (ymajor) px += xstep; else py += ystep; pe += dec; }
      pe += inc;
      if (ymajor) py += ystep; else px += xstep;
    }
  }
  if (ya < cy0) ya = cy0;
  if (yb > cy1) yb = cy1;
  if (xb > cx1) xb = cx1;
  if (xb < 0) xb = 0;
  if (ya > yb)
    return;
  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, ya, yb, xb, true))
    return;

  // Pass 2: plot LNTH pixels
  for (int i = 0; i < n; i++) {
    if ((x >= cx0) && (x <= cx1) && (y >= cy0) && (y <= cy1) &&
        (!aux_on || rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x, y))) {
      Bit32u a = r2d_dst_addr(x, y, bpp);
      Bit32u pat = dp_brush_frgd_clr;
      int cok = 1;
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        cok = r2d_ccmp_px(&cc, 0, dpx);
      }
      for (int b = 0; cok && (b < bpp); b++) {
        Bit8u *dp = r128_surf_at(&sd, a + (Bit32u)b);
        Bit8u pb = (pat >> ((b & 3) * 8)) & 0xff;
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, 0, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
      r2d_mark_dirty(&sd, a, bpp);
    }
    if (zero_pos ? (e >= 0) : (e > 0)) { if (ymajor) x += xstep; else y += ystep; e += dec; }
    e += inc;
    if (ymajor) y += ystep; else x += xstep;
  }
  r2d_surf_commit(&sd);
}

// Screen-to-screen blit through the ROP3 with an optional pattern brush
void bx_rage128_c::r2d_blit_rect_pat(int sx, int sy, int dx, int dy, int w, int h,
                                     const Bit8u *pat8x8, const Bit8u *col8x8, bool mono_la)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u pat = dp_brush_frgd_clr;
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca;
  r128_surf_t sdst, ssrc;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (dx < cx0) { w -= cx0 - dx; sx += cx0 - dx; dx = cx0; }
  if (dy < cy0) { h -= cy0 - dy; sy += cy0 - dy; dy = cy0; }
  if (dx + w - 1 > cx1) w = cx1 - dx + 1;
  if (dy + h - 1 > cy1) h = cy1 - dy + 1;
  if ((w <= 0) || (h <= 0))
    return;
  cca = r2d_ccmp_setup(true, bpp, bpp, &cc);

  Bit8u *rowbuf = new Bit8u[(size_t)w * bpp];
  if (!r2d_map_span(&sdst, &s2d_dst, dst_offset, dst_pitch, bpp, dy, dy + h - 1, dx + w - 1, true)) {
    delete [] rowbuf;
    return;
  }
  if (!r2d_map_span(&ssrc, &s2d_src, src_offset, src_pitch, bpp, sy < 0 ? 0 : sy,
                    sy + h - 1 < 0 ? 0 : sy + h - 1, sx + w - 1 < 0 ? 0 : sx + w - 1, false)) {
    r2d_surf_release(&sdst);
    delete [] rowbuf;
    return;
  }

  bool top_down = !(dy > sy);
  bool fast = !aux_on && (wmask == 0xffffffff) && (rop == 0xcc) && !cca && !(pat8x8 && mono_la);

  for (int i = 0; i < h; i++) {
    int row = top_down ? i : (h - 1 - i);
    Bit32u saddr = r2d_src_addr(sx, sy + row, bpp);
    Bit32u daddr = r2d_dst_addr(dx, dy + row, bpp);

    for (int b = 0; b < w * bpp; b++)
      rowbuf[b] = *r128_surf_at(&ssrc, saddr + (Bit32u)b);
    if (fast) {
      Bit8u *run = r128_surf_run(&sdst, daddr, (Bit32u)w * bpp);
      if (run) {
        memcpy(run, rowbuf, (size_t)w * bpp);
        r2d_mark_dirty(&sdst, daddr, (Bit32u)w * bpp);
        continue;
      }
    }
    for (int col = 0; col < w; col++) {
      Bit32u pcol = 0;
      const Bit8u *patpx = NULL;

      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, dx + col, dy + row))
        continue;
      if (pat8x8) {
        int pset = (pat8x8[(dy + row) & 7] >> (7 - ((dx + col) & 7))) & 1;
        if (!pset && mono_la)
          continue;
        pcol = pset ? dp_brush_frgd_clr : dp_brush_bkgd_clr;
      } else if (col8x8) {
        patpx = &col8x8[(((dy + row) & 7) * 8 + ((dx + col) & 7)) * bpp];
      }
      if (cca) {
        Bit32u rb0 = (Bit32u)col * bpp;
        Bit32u spx = 0, dpx = 0;
        for (int b = 0; b < bpp; b++) {
          spx |= (Bit32u)rowbuf[rb0 + (Bit32u)b] << (b * 8);
          dpx |= (Bit32u)*r128_surf_at(&sdst, daddr + rb0 + (Bit32u)b) << (b * 8);
        }
        int cr = r2d_ccmp_px(&cc, spx, dpx);
        if (!cr)
          continue;
        if (cr == 2) {
          for (int b = 0; b < bpp; b++)
            r2d_store(r128_surf_at(&sdst, daddr + rb0 + (Bit32u)b), (Bit8u)((dpx ^ cc.flip) >> (b * 8)),
                      (Bit8u)(wmask >> ((b & 3) * 8)));
          continue;
        }
      }
      for (int b = 0; b < bpp; b++) {
        Bit32u rbi = (Bit32u)col * bpp + (Bit32u)b;
        Bit8u *dp = r128_surf_at(&sdst, daddr + rbi);
        Bit8u pb = patpx ? patpx[b] : pat8x8 ? (Bit8u)((pcol >> ((b & 3) * 8)) & 0xff) : (Bit8u)((pat >> ((b & 3) * 8)) & 0xff);
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, rowbuf[rbi], *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sdst, daddr, (Bit32u)w * bpp);
  }
  r2d_surf_release(&ssrc);
  r2d_surf_commit(&sdst);
  delete [] rowbuf;
}

void bx_rage128_c::r2d_blit_rect(int sx, int sy, int dx, int dy, int w, int h)
{
  r2d_blit_rect_pat(sx, sy, dx, dy, w, h, NULL, NULL, false);
}

// Transparent (colour keyed) screen-to-screen blit
void bx_rage128_c::r2d_blit_rect_key(int sx, int sy, int dx, int dy, int w, int h,
                                     Bit32u key, Bit32u kmask, bool key_eq_skip)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u pat = dp_brush_frgd_clr;
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  Bit32u pmask = (bpp >= 4) ? 0xffffffff : ((1u << (bpp * 8)) - 1);
  r128_surf_t sdst, ssrc;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (dx < cx0) { w -= cx0 - dx; sx += cx0 - dx; dx = cx0; }
  if (dy < cy0) { h -= cy0 - dy; sy += cy0 - dy; dy = cy0; }
  if (dx + w - 1 > cx1) w = cx1 - dx + 1;
  if (dy + h - 1 > cy1) h = cy1 - dy + 1;
  if ((w <= 0) || (h <= 0))
    return;
  kmask &= pmask;
  key &= kmask;

  if (!r2d_map_span(&sdst, &s2d_dst, dst_offset, dst_pitch, bpp, dy, dy + h - 1, dx + w - 1, true))
    return;
  if (!r2d_map_span(&ssrc, &s2d_src, src_offset, src_pitch, bpp, sy < 0 ? 0 : sy,
                    sy + h - 1 < 0 ? 0 : sy + h - 1, sx + w - 1 < 0 ? 0 : sx + w - 1, false)) {
    r2d_surf_release(&sdst);
    return;
  }
  for (int row = 0; row < h; row++) {
    Bit32u saddr = r2d_src_addr(sx, sy + row, bpp);
    Bit32u daddr = r2d_dst_addr(dx, dy + row, bpp);
    for (int col = 0; col < w; col++) {
      Bit32u sa = saddr + (Bit32u)col * bpp;
      Bit32u da = daddr + (Bit32u)col * bpp;
      Bit32u sv = 0;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, dx + col, dy + row))
        continue;
      for (int b = 0; b < bpp; b++)
        sv |= (Bit32u)*r128_surf_at(&ssrc, sa + (Bit32u)b) << (b * 8);
      if (((sv & kmask) == key) == key_eq_skip)
        continue;
      for (int b = 0; b < bpp; b++) {
        Bit8u *dp = r128_surf_at(&sdst, da + (Bit32u)b);
        Bit8u pb = (pat >> ((b & 3) * 8)) & 0xff;
        Bit8u sb = (sv >> (b * 8)) & 0xff;
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, sb, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sdst, daddr, (Bit32u)w * bpp);
  }
  r2d_surf_release(&ssrc);
  r2d_surf_commit(&sdst);
}

// ---------------------------------------------------------------------
// Stretch blit (scaler)
// ---------------------------------------------------------------------

static Bit32u r2d_texel_argb(const r128_surf_t *s, Bit32u addr, int dt)
{
  Bit32u v, r, g, b;
  switch (dt) {
    case 3: case 15:
      v = *r128_surf_at(s, addr) | ((Bit32u)*r128_surf_at(s, addr + 1) << 8);
      r = (v >> 10) & 0x1f; g = (v >> 5) & 0x1f; b = v & 0x1f;
      return ((v & 0x8000) ? 0xff000000u : 0) | (((r << 3) | (r >> 2)) << 16) | (((g << 3) | (g >> 2)) << 8) | ((b << 3) | (b >> 2));
    case 4:
      v = *r128_surf_at(s, addr) | ((Bit32u)*r128_surf_at(s, addr + 1) << 8);
      r = (v >> 11) & 0x1f; g = (v >> 5) & 0x3f; b = v & 0x1f;
      return 0xff000000u | (((r << 3) | (r >> 2)) << 16) | (((g << 2) | (g >> 4)) << 8) | ((b << 3) | (b >> 2));
    case 5:
      return 0xff000000u | ((Bit32u)*r128_surf_at(s, addr + 2) << 16) | ((Bit32u)*r128_surf_at(s, addr + 1) << 8) | *r128_surf_at(s, addr);
    case 6: case 14:
      return *r128_surf_at(s, addr) | ((Bit32u)*r128_surf_at(s, addr + 1) << 8) |
             ((Bit32u)*r128_surf_at(s, addr + 2) << 16) | ((Bit32u)*r128_surf_at(s, addr + 3) << 24);
    case 11: case 12:
      return *r128_surf_at(s, addr) | ((Bit32u)*r128_surf_at(s, addr + 1) << 8);
    default:
      v = *r128_surf_at(s, addr);
      return (v << 24) | (v << 16) | (v << 8) | v;
  }
}

static Bit32u r2d_argb_pack(Bit32u argb, int dt)
{
  switch (dt) {
    case 3: case 15:
      return ((argb >> 16) & 0x8000) | ((argb >> 9) & 0x7c00) | ((argb >> 6) & 0x03e0) | ((argb >> 3) & 0x001f);
    case 4:
      return ((argb >> 8) & 0xf800) | ((argb >> 5) & 0x07e0) | ((argb >> 3) & 0x001f);
    case 5:
      return argb & 0x00ffffff;
    case 6: case 14:
      return argb;
    case 11: case 12:
      return argb & 0xffff;
    default:
      return argb & 0xff;
  }
}

static Bit32u r2d_val_argb(Bit32u v, int dt)
{
  Bit32u r, g, b;
  switch (dt) {
    case 3: case 15:
      r = (v >> 10) & 0x1f; g = (v >> 5) & 0x1f; b = v & 0x1f;
      return ((v & 0x8000) ? 0xff000000u : 0) | (((r << 3) | (r >> 2)) << 16) | (((g << 3) | (g >> 2)) << 8) | ((b << 3) | (b >> 2));
    case 4:
      r = (v >> 11) & 0x1f; g = (v >> 5) & 0x3f; b = v & 0x1f;
      return 0xff000000u | (((r << 3) | (r >> 2)) << 16) | (((g << 2) | (g >> 4)) << 8) | ((b << 3) | (b >> 2));
    case 5:
      return 0xff000000u | (v & 0x00ffffff);
    case 6:
      return v;
    default:
      v &= 0xff;
      return (v << 24) | (v << 16) | (v << 8) | v;
  }
}

// YUV422 texel through the datapath Y2R converter
static Bit32u r2d_yuv_texel_argb(Bit32u dp_datatype, const r128_surf_t *s, Bit32u addr, int sdt)
{
  Bit32u pa = addr & ~3u;
  int odd = (int)(addr >> 1) & 1;
  Bit8u p0 = *r128_surf_at(s, pa), p1 = *r128_surf_at(s, pa + 1);
  Bit8u p2 = *r128_surf_at(s, pa + 2), p3 = *r128_surf_at(s, pa + 3);
  int y, u, v, r, g, b;
  if (sdt == 11) { y = odd ? p2 : p0; u = p1; v = p3; }
  else { y = odd ? p3 : p1; u = p0; v = p2; }
  if (dp_datatype & 0x80000000u)
    r = (144 * y + 200 * v - 27904) >> 7;
  else
    r = (144 * y + 400 * v - 53504) >> 7;
  g = (144 * y - 104 * v - 50 * u + 17408) >> 7;
  b = (144 * y + 256 * u - 35072) >> 7;
  if (r < 0) r = 0; else if (r > 255) r = 255;
  if (g < 0) g = 0; else if (g > 255) g = 255;
  if (b < 0) b = 0; else if (b > 255) b = 255;
  return 0xff000000u | ((Bit32u)r << 16) | ((Bit32u)g << 8) | (Bit32u)b;
}

Bit32u bx_rage128_c::r2d_stretch_texel(const r128_surf_t *s, Bit32u addr, int sdt, int ddt, bool usepal)
{
  if (usepal) {
    Bit32u idx = *r128_surf_at(s, addr);
    return r2d_val_argb(scl_palette[idx], ddt);
  }
  if (((sdt == 11) || (sdt == 12)) && (sdt != ddt))
    return r2d_yuv_texel_argb(dp_datatype, s, addr, sdt);
  return r2d_texel_argb(s, addr, sdt);
}

static Bit32u r2d_lerp_argb(Bit32u c0, Bit32u c1, Bit32u w)
{
  Bit32u iw = 256 - w;
  Bit32u rb = (((c0 & 0x00ff00ffu) * iw + (c1 & 0x00ff00ffu) * w) >> 8) & 0x00ff00ffu;
  Bit32u ag = ((((c0 >> 8) & 0x00ff00ffu) * iw + ((c1 >> 8) & 0x00ff00ffu) * w)) & 0xff00ff00u;
  return rb | ag;
}

bool bx_rage128_c::r2d_stretch_resolve(const r128_surf_t *ssrc, int sbpp, int sdt, int ddt, bool usepal,
                                       bool blend, int sx0, Bit32u xinc, int xskip, int col, Bit32u saddr,
                                       Bit32u sbddr, Bit32u wy, int sxlim, Bit32u key, Bit32u kmask,
                                       int key_skip, Bit32u *out)
{
  Bit32u xacc = (Bit32u)(col + xskip) * xinc;
  int sx = sx0 + (int)(xacc >> 16);
  Bit32u a = saddr + (Bit32u)sx * (Bit32u)sbpp;
  Bit32u c;

  if (key_skip >= 0) {
    Bit32u raw = 0;
    for (int b = 0; b < sbpp; b++)
      raw |= (Bit32u)*r128_surf_at(ssrc, a + (Bit32u)b) << (b * 8);
    if ((int)((raw & kmask) == key) == key_skip)
      return false;
  }
  c = r2d_stretch_texel(ssrc, a, sdt, ddt, usepal);
  if (blend) {
    int sxb = (sx < sxlim) ? sx + 1 : sx;
    Bit32u wx = (xacc & 0xffff) >> 8;
    Bit32u xb = (Bit32u)sxb * (Bit32u)sbpp;
    Bit32u top = r2d_lerp_argb(c, r2d_stretch_texel(ssrc, saddr + xb, sdt, ddt, usepal), wx);
    Bit32u bot = r2d_lerp_argb(r2d_stretch_texel(ssrc, sbddr + (Bit32u)sx * (Bit32u)sbpp, sdt, ddt, usepal),
                               r2d_stretch_texel(ssrc, sbddr + xb, sdt, ddt, usepal), wx);
    c = r2d_lerp_argb(top, bot, wy);
  }
  *out = r2d_argb_pack(c, ddt);
  return true;
}

void bx_rage128_c::r2d_stretch_rect(Bit32u soff, Bit32u spitch_px, int sx0, int sy0, Bit32u xinc, Bit32u yinc,
                                    int dx, int dy, int w, int h, int sdt, bool blend)
{
  int ddt = DP_DST_DT();
  int bpp = r2d_bpp(ddt);
  int sbpp = r2d_bpp(sdt);
  Bit8u rop = DP_ROP3();
  Bit32u pat = dp_brush_frgd_clr;
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  r128_surf_t sdst, ssrc;
  int cx0, cy0, cx1, cy1;
  int xskip = 0, yskip = 0;
  int symax, sxmax, sxlim, sylim;
  bool usepal = (sdt == 2) && (bpp != 1);
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(true, sbpp, bpp, &cc);
  bool keyq = cca && cc.src_on && ((cc.fn_src == 4) || (cc.fn_src == 5)) && (!cc.dst_on || (cc.fn_dst == 0));
  bool cgen = cca && !keyq;
  int key_skip = keyq ? (cc.fn_src == 5) : -1;
  Bit32u key = keyq ? cc.key_src : 0;
  Bit32u kmask = keyq ? cc.smask : 0;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (dx < cx0) { xskip = cx0 - dx; w -= xskip; dx = cx0; }
  if (dy < cy0) { yskip = cy0 - dy; h -= yskip; dy = cy0; }
  if (dx + w - 1 > cx1) w = cx1 - dx + 1;
  if (dy + h - 1 > cy1) h = cy1 - dy + 1;
  if ((w <= 0) || (h <= 0))
    return;
  symax = sy0 + (int)(((Bit32u)(h - 1 + yskip) * yinc) >> 16);
  sxmax = sx0 + (int)(((Bit32u)(w - 1 + xskip) * xinc) >> 16);
  sxlim = (scale_scr_height_width & 0xffff) ? (int)(scale_scr_height_width & 0xffff) - 1 : sxmax;
  sylim = (scale_scr_height_width >> 16) ? (int)(scale_scr_height_width >> 16) - 1 : symax;

  if (!r2d_map_span(&sdst, &s2d_dst, dst_offset, dst_pitch, bpp, dy, dy + h - 1, dx + w - 1, true))
    return;
  {
    int sxmap = (blend && (sxmax < sxlim)) ? sxmax + 1 : (sxmax < 0 ? 0 : sxmax);
    if (((sdt == 11) || (sdt == 12)) && (sdt != ddt))
      sxmap |= 1;
    if (!r2d_map_span(&ssrc, &s2d_src, soff, spitch_px, sbpp, sy0 < 0 ? 0 : sy0,
                      (blend && (symax < sylim)) ? symax + 1 : (symax < 0 ? 0 : symax), sxmap, false)) {
      r2d_surf_release(&sdst);
      return;
    }
  }

  for (int row = 0; row < h; row++) {
    Bit32u yacc = (Bit32u)(row + yskip) * yinc;
    int sy = sy0 + (int)(yacc >> 16);
    int syb = (sy < sylim) ? sy + 1 : sy;
    Bit32u wy = (yacc & 0xffff) >> 8;
    Bit32u saddr = soff + (Bit32u)sy * r2d_stride(spitch_px, sbpp);
    Bit32u sbddr = soff + (Bit32u)syb * r2d_stride(spitch_px, sbpp);
    Bit32u daddr = r2d_dst_addr(dx, dy + row, bpp);

    for (int col = 0; col < w; col++) {
      Bit32u da = daddr + (Bit32u)col * (Bit32u)bpp;
      Bit32u spx;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, dx + col, dy + row))
        continue;
      if (cgen) {
        Bit32u xacc = (Bit32u)(col + xskip) * xinc;
        int rsx = sx0 + (int)(xacc >> 16);
        Bit32u ra = saddr + (Bit32u)rsx * (Bit32u)sbpp;
        Bit32u raw = 0, dpx = 0;
        for (int b = 0; b < sbpp; b++)
          raw |= (Bit32u)*r128_surf_at(&ssrc, ra + (Bit32u)b) << (b * 8);
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sdst, da + (Bit32u)b) << (b * 8);
        int cr = r2d_ccmp_px(&cc, raw, dpx);
        if (!cr)
          continue;
        if (cr == 2) {
          for (int b = 0; b < bpp; b++)
            r2d_store(r128_surf_at(&sdst, da + (Bit32u)b), (Bit8u)((dpx ^ cc.flip) >> (b * 8)), (Bit8u)(wmask >> ((b & 3) * 8)));
          continue;
        }
      }
      if (!r2d_stretch_resolve(&ssrc, sbpp, sdt, ddt, usepal, blend, sx0, xinc, xskip, col, saddr, sbddr,
                               wy, sxlim, key, kmask, key_skip, &spx))
        continue;
      for (int b = 0; b < bpp; b++) {
        Bit8u *dp = r128_surf_at(&sdst, da + (Bit32u)b);
        Bit8u sb = (Bit8u)(spx >> (b * 8));
        Bit8u pb = (pat >> ((b & 3) * 8)) & 0xff;
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, sb, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sdst, daddr, (Bit32u)w * bpp);
  }
  r2d_surf_release(&ssrc);
  r2d_surf_commit(&sdst);
}

// Decode and run the 11-dword scaler parameter block (0x96 / 0x97)
bool bx_rage128_c::r2d_scale_block(const Bit32u *b)
{
  Bit32u depth = b[3];
  Bit32u spitch = (b[5] & 0x3ff) * 8;
  if ((b[5] >> 30) == 1) spitch *= 2;
  else if ((b[5] >> 30) == 2) spitch *= 4;
  switch (DP_DST_DT()) {
    case 11: case 12: case 14:
      if (depth != DP_DST_DT()) return false;
      break;
    default: break;
  }
  switch (depth) {
    case 2: break;
    case 7: case 8: case 9:
      if (r2d_bpp(DP_DST_DT()) != 1) return false;
      break;
    case 3: case 4: case 5: case 6: case 15:
      if (r2d_bpp(DP_DST_DT()) == 1) return false;
      break;
    case 11: case 12:
      if (b[4] & 3) return false;
      if ((depth != DP_DST_DT()) && (r2d_bpp(DP_DST_DT()) == 1)) return false;
      break;
    case 14:
      if (depth != DP_DST_DT()) return false;
      break;
    default:
      return false;
  }
  r2d_stretch_rect(b[4], spitch, rage128_sx14(b[6]), rage128_sx14(b[6] >> 16), b[7], b[8],
                   rage128_sx14(b[9] >> 16), rage128_sx14(b[9]), (int)(b[10] & 0x3fff),
                   (int)((b[10] >> 16) & 0x3fff), (int)depth, !((b[0] >> 10) & 1));
  return true;
}

// Mono expand: set bits paint the foreground; clear bits write the
// background or are left alone per the source datatype. bitpitch in bits.
void bx_rage128_c::r2d_mono_rect(int x, int y, int w, int h, const Bit8u *bits, Bit32u bitpitch,
                                 const Bit8u *pat8x8, const Bit8u *patcol)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  bool use_bkgd = (DP_SRC_DT() == 0);
  bool lsb_first = DP_LSB_FIRST() != 0;
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(true, bpp, bpp, &cc);
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, y < cy0 ? cy0 : y,
                    y + h - 1 > cy1 ? cy1 : y + h - 1, x + w - 1 > cx1 ? cx1 : x + w - 1, true))
    return;

  for (int row = 0; row < h; row++) {
    if ((y + row < cy0) || (y + row > cy1))
      continue;
    Bit32u addr = r2d_dst_addr(x, y + row, bpp);
    for (int col = 0; col < w; col++) {
      Bit32u bit = (Bit32u)row * bitpitch + col;
      int set;
      if ((x + col < cx0) || (x + col > cx1))
        continue;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x + col, y + row))
        continue;
      if (lsb_first)
        set = (bits[bit >> 3] >> (bit & 7)) & 1;
      else
        set = (bits[bit >> 3] >> (7 - (bit & 7))) & 1;
      if (!set && !use_bkgd)
        continue;
      Bit32u fg = set ? dp_src_frgd_clr : dp_src_bkgd_clr;
      Bit32u a = addr + (Bit32u)col * bpp;
      Bit32u pat = dp_brush_frgd_clr;
      const Bit8u *patpx = NULL;
      if (pat8x8) {
        int pset = (pat8x8[(y + row) & 7] >> (7 - ((x + col) & 7))) & 1;
        pat = pset ? dp_brush_frgd_clr : dp_brush_bkgd_clr;
      } else if (patcol) {
        patpx = &patcol[(((y + row) & 7) * 8 + ((x + col) & 7)) * bpp];
      }
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        int cr = r2d_ccmp_px(&cc, fg, dpx);
        if (!cr)
          continue;
        if (cr == 2) {
          for (int b = 0; b < bpp; b++)
            r2d_store(r128_surf_at(&sd, a + (Bit32u)b), (Bit8u)((dpx ^ cc.flip) >> (b * 8)), (Bit8u)(wmask >> ((b & 3) * 8)));
          continue;
        }
      }
      for (int b = 0; b < bpp; b++) {
        Bit8u *dp = r128_surf_at(&sd, a + (Bit32u)b);
        Bit8u sb = (fg >> ((b & 3) * 8)) & 0xff;
        Bit8u pb = patpx ? patpx[b] : ((pat >> ((b & 3) * 8)) & 0xff);
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, sb, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
  }
  r2d_surf_commit(&sd);
}

// Colour host data: pixels in the dst format, rows back-to-back
void bx_rage128_c::r2d_host_color_rect(int x, int y, int w, int h, const Bit8u *px, Bit32u avail)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit8u rop = DP_ROP3();
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  Bit32u stride = (Bit32u)w * (Bit32u)bpp;
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(true, bpp, bpp, &cc);
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, y < cy0 ? cy0 : y,
                    y + h - 1 > cy1 ? cy1 : y + h - 1, x + w - 1 > cx1 ? cx1 : x + w - 1, true))
    return;

  for (int row = 0; row < h; row++) {
    if ((y + row < cy0) || (y + row > cy1))
      continue;
    Bit32u addr = r2d_dst_addr(x, y + row, bpp);
    const Bit8u *srow = px + (Bit32u)row * stride;
    for (int col = 0; col < w; col++) {
      if ((x + col < cx0) || (x + col > cx1))
        continue;
      if ((Bit32u)row * stride + ((Bit32u)col + 1) * bpp > avail)
        goto done;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x + col, y + row))
        continue;
      Bit32u a = addr + (Bit32u)col * bpp;
      if (cca) {
        Bit32u spx = 0, dpx = 0;
        for (int b = 0; b < bpp; b++) {
          spx |= (Bit32u)srow[(Bit32u)col * bpp + b] << (b * 8);
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        }
        int cr = r2d_ccmp_px(&cc, spx, dpx);
        if (!cr)
          continue;
        if (cr == 2) {
          for (int b = 0; b < bpp; b++)
            r2d_store(r128_surf_at(&sd, a + (Bit32u)b), (Bit8u)((dpx ^ cc.flip) >> (b * 8)), (Bit8u)(wmask >> ((b & 3) * 8)));
          continue;
        }
      }
      for (int b = 0; b < bpp; b++) {
        Bit8u *dp = r128_surf_at(&sd, a + (Bit32u)b);
        Bit8u sv = srow[(Bit32u)col * bpp + b];
        Bit8u pb = (dp_brush_frgd_clr >> ((b & 3) * 8)) & 0xff;
        r2d_store(dp, (Bit8u)rage128_rop3(rop, pb, sv, *dp), (Bit8u)(wmask >> ((b & 3) * 8)));
      }
    }
    r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
  }
done:
  r2d_surf_commit(&sd);
}

void bx_rage128_c::r2d_span_rect(int y, int h, int xl, int xr, const Bit8u *mono8x8, const Bit8u *col8x8, bool mono_la)
{
  if ((xr <= xl) || (h <= 0))
    return;
  r2d_paint_rect(xl, y, xr - xl, h, mono8x8, col8x8, mono_la);
}

// Parse the brush data block riding after the slot dwords
bool bx_rage128_c::r2d_brush_block(Bit32u g, const Bit32u *pl, Bit32u count, Bit32u *pp, const Bit8u **mono,
                                   const Bit8u **col, bool *mono_la, Bit32u *linepat, bool *line_en)
{
  Bit32u p = *pp;

  *mono = NULL; *col = NULL; *mono_la = false; *linepat = 0; *line_en = false;
  switch (RAGE128_GMC_BRUSH_TYPE(g)) {
    case 15:
      break;
    case 13:
      if (p >= count) return false;
      dp_brush_frgd_clr = pl[p++];
      break;
    case 0:
      if (p + 3 >= count) return false;
      dp_brush_bkgd_clr = pl[p++];
      dp_brush_frgd_clr = pl[p++];
      brush_data[0] = pl[p];
      brush_data[1] = pl[p + 1];
      *mono = (const Bit8u*)&pl[p];
      p += 2;
      break;
    case 1:
      if (p + 2 >= count) return false;
      dp_brush_frgd_clr = pl[p++];
      brush_data[0] = pl[p];
      brush_data[1] = pl[p + 1];
      *mono = (const Bit8u*)&pl[p];
      *mono_la = true;
      p += 2;
      break;
    case 10: {
      Bit32u nb = (64u * (Bit32u)r2d_bpp(RAGE128_GMC_DST_DATATYPE(g)) + 3) / 4;
      if (p + nb > count) return false;
      *col = (const Bit8u*)&pl[p];
      p += nb;
      break;
    }
    case 6:
      if (p + 2 >= count) return false;
      dp_brush_bkgd_clr = pl[p++];
      dp_brush_frgd_clr = pl[p++];
      *linepat = pl[p++];
      *line_en = true;
      break;
    case 7:
      if (p + 1 >= count) return false;
      dp_brush_frgd_clr = pl[p++];
      *linepat = pl[p++];
      *line_en = true;
      *mono_la = true;
      break;
    default:
      return false;
  }
  if (g & RAGE128_GMC_LD_BRUSH_Y_X) {
    if (p >= count) return false;
    p++;
  }
  *pp = p;
  return true;
}

// ---------------------------------------------------------------------
// PM4 type-3 dispatch
// ---------------------------------------------------------------------

void bx_rage128_c::r2d_packet3(Bit32u hdr, const Bit32u *pl, Bit32u count)
{
  Bit32u op = RAGE128_PM4_T3_OPCODE(hdr);
  Bit32u g;
  Bit32u p = 1;

  if (op == RAGE128_PM4_OP_NOP)
    return;
  if (count < 1)
    goto unhandled;
  g = pl[0];

  switch (op) {
    case RAGE128_PM4_OP_PAINT:
    case RAGE128_PM4_OP_PAINT_MULTI:
    case RAGE128_PM4_OP_POLYLINE:
    case RAGE128_PM4_OP_SPANLIST:
    {
      const Bit8u *pat = NULL;
      const Bit8u *col = NULL;
      bool mono_la, line_en;
      Bit32u linepat;

      r2d_apply_gmc(g);
      if (g & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(true, pl[p++]);
      }
      if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
        if (p + 1 >= count) goto unhandled;
        sc_top_left = pl[p++];
        sc_bottom_right = pl[p++];
      }
      if (!r2d_brush_block(g, pl, count, &p, &pat, &col, &mono_la, &linepat, &line_en))
        goto unhandled;
      if (op == RAGE128_PM4_OP_POLYLINE) {
        int phase = 0;
        for (; p + 1 < count; p++)
          r2d_line(rage128_sx14(pl[p]), rage128_sx14(pl[p] >> 16),
                   rage128_sx14(pl[p + 1]), rage128_sx14(pl[p + 1] >> 16),
                   linepat, line_en, mono_la, &phase);
        return;
      }
      if (op == RAGE128_PM4_OP_SPANLIST) {
        Bit32u nent;
        if (p >= count)
          return; // setup-only header (DrvFillPath)
        nent = pl[p++];
        for (Bit32u e = 0; (e < nent) && (p + 1 < count); e++) {
          Bit32u nspan = pl[p++];
          int yy = rage128_sx14(pl[p]);
          int hh = (pl[p] >> 16) & 0x3fff;
          p++;
          for (Bit32u sp = 0; (sp < nspan) && (p < count); sp++, p++)
            r2d_span_rect(yy, hh, rage128_sx14(pl[p]), rage128_sx14(pl[p] >> 16), pat, col, mono_la);
        }
        return;
      }
      for (; p + 1 < count; p += 2) {
        int x, y, w, h;
        if (op == RAGE128_PM4_OP_PAINT) {
          x = rage128_sx14(pl[p]);
          y = rage128_sx14(pl[p] >> 16);
          w = rage128_sx14(pl[p + 1]) - x + 1;
          h = rage128_sx14(pl[p + 1] >> 16) - y + 1;
        } else {
          x = rage128_sx14(pl[p] >> 16);
          y = rage128_sx14(pl[p]);
          w = (pl[p + 1] >> 16) & 0x3fff;
          h = pl[p + 1] & 0x3fff;
        }
        r2d_paint_rect(x, y, w, h, pat, col, mono_la);
      }
      return;
    }

    case RAGE128_PM4_OP_PAINT_NC:
    {
      Bit32u bt = DP_BRUSH_DT();
      bool pat_mono = (bt == 0) || (bt == 1);
      for (Bit32u pp = 0; pp + 1 < count; pp += 2) {
        int x = rage128_sx14(pl[pp]);
        int y = rage128_sx14(pl[pp] >> 16);
        int w = rage128_sx14(pl[pp + 1]) - x + 1;
        int h = rage128_sx14(pl[pp + 1] >> 16) - y + 1;
        r2d_paint_rect(x, y, w, h, pat_mono ? (const Bit8u*)brush_data : NULL, NULL, bt == 1);
      }
      return;
    }

    case RAGE128_PM4_OP_BITBLT_NC:
    {
      Bit32u q = 0;
      if (count % 3)
        goto unhandled;
      for (; q + 2 < count; q += 3) {
        int sx = rage128_sx14(pl[q] >> 16), sy = rage128_sx14(pl[q]);
        int dx = rage128_sx14(pl[q + 1] >> 16), dy = rage128_sx14(pl[q + 1]);
        int w = (pl[q + 2] >> 16) & 0x3fff, h = pl[q + 2] & 0x3fff;
        r2d_blit_rect(sx, sy, dx, dy, w, h);
      }
      return;
    }

    case RAGE128_PM4_OP_CNTL_BITBLT:
    case RAGE128_PM4_OP_BITBLT_MULTI:
    case RAGE128_PM4_OP_BLIT_MULTI:
    {
      const Bit8u *pat = NULL;
      const Bit8u *col = NULL;
      bool mono_la, line_en;
      Bit32u linepat;

      r2d_apply_gmc(g);
      if (g & RAGE128_GMC_SRC_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(false, pl[p++]);
      }
      if (g & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(true, pl[p++]);
      }
      if (g & RAGE128_GMC_SRC_CLIP_LEAVE) {
        if (p >= count) goto unhandled;
        src_sc_right = pl[p] & 0x3fff;
        src_sc_bottom = (pl[p] >> 16) & 0x3fff;
        p++;
      }
      if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
        if (p + 1 >= count) goto unhandled;
        sc_top_left = pl[p++];
        sc_bottom_right = pl[p++];
      }
      if (!r2d_brush_block(g, pl, count, &p, &pat, &col, &mono_la, &linepat, &line_en))
        goto unhandled;
      if ((count - p) % 3)
        goto unhandled;
      for (; p + 2 < count; p += 3) {
        int sx = rage128_sx14(pl[p] >> 16), sy = rage128_sx14(pl[p]);
        int dx = rage128_sx14(pl[p + 1] >> 16), dy = rage128_sx14(pl[p + 1]);
        int w = (pl[p + 2] >> 16) & 0x3fff, h = pl[p + 2] & 0x3fff;
        r2d_blit_rect_pat(sx, sy, dx, dy, w, h, pat, col, mono_la);
      }
      return;
    }

    case RAGE128_PM4_OP_TRANS_BITBLT:
    {
      int sx, sy, dx, dy, w, h;

      if ((count < 12) && (count < ((g & RAGE128_GMC_DST_CLIP_LEAVE) ? 11u : 9u)))
        goto unhandled;
      r2d_apply_gmc(g);
      if (count == 12) {
        r2d_set_pitch_offset(false, pl[1]);
        r2d_set_pitch_offset(true, pl[2]);
        sc_top_left = pl[4];
        sc_bottom_right = pl[5];
        sx = rage128_sx14(pl[9] >> 16);  sy = rage128_sx14(pl[9]);
        dx = rage128_sx14(pl[10] >> 16); dy = rage128_sx14(pl[10]);
        w = (pl[11] >> 16) & 0x3fff; h = pl[11] & 0x3fff;
        clr_cmp_cntl = pl[6];
        clr_cmp_clr_src = pl[7];
      } else if (count > 12) {
        r2d_set_pitch_offset(true, pl[1]);
        r2d_set_pitch_offset(false, pl[2]);
        sx = rage128_sx14(pl[5] >> 16); sy = rage128_sx14(pl[5]);
        dx = rage128_sx14(pl[3] >> 16); dy = rage128_sx14(pl[3]);
        w = (pl[11] >> 16) & 0x3fff; h = pl[11] & 0x3fff;
        clr_cmp_cntl = pl[6];
        clr_cmp_clr_src = pl[7];
      } else {
        int q = 3;
        r2d_set_pitch_offset(false, pl[1]);
        r2d_set_pitch_offset(true, pl[2]);
        if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
          sc_top_left = pl[3];
          sc_bottom_right = pl[4];
          q = 5;
        }
        clr_cmp_cntl = pl[q];
        clr_cmp_clr_src = pl[q + 1];
        sx = rage128_sx14(pl[q + 3] >> 16); sy = rage128_sx14(pl[q + 3]);
        dx = rage128_sx14(pl[q + 4] >> 16); dy = rage128_sx14(pl[q + 4]);
        w = (pl[q + 5] >> 16) & 0x3fff; h = pl[q + 5] & 0x3fff;
      }
      if ((w <= 0) || (h <= 0))
        return;
      {
        int kb = r2d_bpp(DP_DST_DT());
        struct r128_ccmp cc;
        int cca = r2d_ccmp_setup(true, kb, kb, &cc);
        if (cca && cc.src_on && ((cc.fn_src == 4) || (cc.fn_src == 5)) && (!cc.dst_on || (cc.fn_dst == 0)))
          r2d_blit_rect_key(sx, sy, dx, dy, w, h, cc.key_src, cc.smask, cc.fn_src == 5);
        else
          r2d_blit_rect(sx, sy, dx, dy, w, h);
      }
      return;
    }

    case RAGE128_PM4_OP_SMALLTEXT:
    {
      int x, y;
      r2d_apply_gmc(g);
      if (g & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(true, pl[p++]);
      }
      if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
        if (p + 1 >= count) goto unhandled;
        sc_top_left = pl[p++];
        sc_bottom_right = pl[p++];
      }
      if (p + 1 >= count)
        goto unhandled;
      dp_src_frgd_clr = pl[p++];
      x = rage128_sx14(pl[p]);
      y = rage128_sx14(pl[p] >> 16);
      p++;
      while (p < count) {
        Bit32u geom = pl[p++];
        int adv = (Bit8s)(geom & 0xff);
        int ascent = (Bit8s)((geom >> 8) & 0xff);
        int w = (geom >> 16) & 0xff;
        int h = (geom >> 24) & 0xff;
        Bit32u nd = ((Bit32u)w * h + 31) / 32;
        x += adv;
        if ((w == 0) || (h == 0))
          continue;
        if (count - p < nd)
          break;
        r2d_mono_rect(x, y - ascent, w, h, (const Bit8u*)&pl[p], (Bit32u)w, NULL, NULL);
        p += nd;
      }
      return;
    }

    case RAGE128_PM4_OP_HOSTDATA_BLT:
    {
      int x, y, w, h;
      Bit32u nd, srcdt;
      const Bit8u *bpat = NULL;
      const Bit8u *bcol = NULL;
      bool bmono_la, bline_en;
      Bit32u blinepat;

      r2d_apply_gmc(g);
      if (g & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(true, pl[p++]);
      }
      if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
        if (p + 1 >= count) goto unhandled;
        sc_top_left = pl[p++];
        sc_bottom_right = pl[p++];
      }
      srcdt = RAGE128_GMC_SRC_DATATYPE(g);
      if ((RAGE128_GMC_SRC_SOURCE(g) == 3) && (srcdt == 1)) {
        // ExtTextOut overflow form: per-glyph records
        if (p + 1 >= count)
          goto unhandled;
        dp_src_frgd_clr = pl[p++];
        p++;
        while (p + 2 < count) {
          x = rage128_sx14(pl[p]);
          y = rage128_sx14(pl[p] >> 16);
          h = (pl[p + 1] >> 16) & 0x3fff;
          w = pl[p + 1] & 0x3fff;
          nd = pl[p + 2];
          p += 3;
          if ((w <= 0) || (h <= 0) || (count - p < nd))
            goto unhandled;
          r2d_mono_rect(x, y, w, h, (const Bit8u*)&pl[p], (Bit32u)w, NULL, NULL);
          p += nd;
        }
        return;
      }
      if (!r2d_brush_block(g, pl, count, &p, &bpat, &bcol, &bmono_la, &blinepat, &bline_en))
        goto unhandled;
      if (p + 4 >= count)
        goto unhandled;
      dp_src_frgd_clr = pl[p++];
      dp_src_bkgd_clr = pl[p++];
      x = rage128_sx14(pl[p]);
      y = rage128_sx14(pl[p] >> 16);
      h = (pl[p + 1] >> 16) & 0x3fff;
      w = pl[p + 1] & 0x3fff;
      nd = pl[p + 2];
      p += 3;
      if ((w <= 0) || (h <= 0))
        goto unhandled;
      if (srcdt == 3) {
        Bit32u sc_r = sc_bottom_right & 0x3fff;
        Bit32u sc_b = (sc_bottom_right >> 16) & 0x3fff;
        Bit32u last_x = (Bit32u)x + (Bit32u)w - 1;
        Bit32u last_y = (Bit32u)y + (Bit32u)h - 1;
        if (last_x > sc_r) last_x = sc_r;
        if (last_y > sc_b) last_y = sc_b;
        if (((Bit32s)last_x < x) || ((Bit32s)last_y < y))
          return;
        r2d_host_color_rect(x, y, w, h, (const Bit8u*)&pl[p], (count - p) * 4);
      } else {
        if ((count - p < nd) || ((Bit64u)nd * 4 < (Bit64u)((((Bit32u)w + 7) & ~7u) / 8) * (Bit32u)h))
          goto unhandled;
        r2d_mono_rect(x, y, w, h, (const Bit8u*)&pl[p], ((Bit32u)w + 7) & ~7u, bpat, bcol);
      }
      return;
    }

    case RAGE128_PM4_OP_PURGE:
    case RAGE128_PM4_OP_SCALER_CFG:
      return;

    case RAGE128_PM4_OP_SCALING:
    {
      r2d_apply_gmc(g);
      if (g & RAGE128_GMC_SRC_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(false, pl[p++]);
      }
      if (g & RAGE128_GMC_DST_PITCH_OFFSET_LEAVE) {
        if (p >= count) goto unhandled;
        r2d_set_pitch_offset(true, pl[p++]);
      }
      if (g & RAGE128_GMC_SRC_CLIP_LEAVE) {
        if (p >= count) goto unhandled;
        p++;
      }
      if (g & RAGE128_GMC_DST_CLIP_LEAVE) {
        if (p + 1 >= count) goto unhandled;
        sc_top_left = pl[p++];
        sc_bottom_right = pl[p++];
      }
      if (count - p < 11)
        goto unhandled;
      if (!r2d_scale_block(&pl[p]))
        goto unhandled;
      return;
    }

    case RAGE128_PM4_OP_TRANS_SCALING:
    {
      if (count < 20)
        goto unhandled;
      r2d_apply_gmc(g);
      r2d_set_pitch_offset(true, pl[2]);
      sc_top_left = pl[4];
      sc_bottom_right = pl[5];
      clr_cmp_cntl = pl[6];
      clr_cmp_clr_src = pl[7];
      clr_cmp_mask = pl[8];
      if (!r2d_scale_block(&pl[9]))
        goto unhandled;
      return;
    }

    case RAGE128_PM4_OP_HOSTROW:
    {
      int x, y, w, h;
      Bit32u nd, srcdt;
      if (count < 2)
        goto unhandled;
      srcdt = DP_SRC_DT();
      x = rage128_sx14(pl[0]);
      y = rage128_sx14(pl[0] >> 16);
      h = (pl[1] >> 16) & 0x3fff;
      w = pl[1] & 0x3fff;
      nd = count - 2;
      if ((w <= 0) || (h <= 0))
        goto unhandled;
      if (srcdt == 3) {
        if (nd * 4 < (Bit32u)w * (Bit32u)h * (Bit32u)r2d_bpp(DP_DST_DT()))
          goto unhandled;
        r2d_host_color_rect(x, y, w, h, (const Bit8u*)&pl[2], (count - 2) * 4);
      } else {
        if ((Bit64u)nd * 32 < (Bit64u)w * (Bit64u)h)
          goto unhandled;
        r2d_mono_rect(x, y, w, h, (const Bit8u*)&pl[2], (Bit32u)w, NULL, NULL);
      }
      return;
    }

    case RAGE128_PM4_OP_PLY_NEXTSCAN:
    {
      int y, xl, xr;
      if (count < 2)
        goto unhandled;
      y = (int)(pl[0] & 0xffff);
      xl = (int)(pl[1] & 0xffff);
      xr = (int)((pl[1] >> 16) & 0xffff);
      if (xr <= xl)
        return;
      {
        Bit32u bt = DP_BRUSH_DT();
        bool pat_mono = (bt == 0) || (bt == 1);
        r2d_paint_rect(xl, y, xr - xl, 1, pat_mono ? (const Bit8u*)brush_data : NULL, NULL, bt == 1);
      }
      return;
    }

    case RAGE128_PM4_OP_SET_SCISSORS:
      if (count < 2)
        goto unhandled;
      sc_top_left = pl[0];
      sc_bottom_right = pl[1];
      return;

    default:
      break;
  }

unhandled:
  BX_DEBUG(("unhandled 2D packet op=0x%02x count=%u g=0x%08x", op, count, (count >= 1) ? pl[0] : 0));
}

// ---------------------------------------------------------------------
// Register-triggered ops (direct 2D path)
// ---------------------------------------------------------------------

void bx_rage128_c::r2d_fill_solid(int x, int y, int w, int h, Bit32u color)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca;
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (x < cx0) { w -= cx0 - x; x = cx0; }
  if (y < cy0) { h -= cy0 - y; y = cy0; }
  if (x + w - 1 > cx1) w = cx1 - x + 1;
  if (y + h - 1 > cy1) h = cy1 - y + 1;
  if ((w <= 0) || (h <= 0))
    return;
  cca = r2d_ccmp_setup(false, bpp, bpp, &cc);
  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, y, y + h - 1, x + w - 1, true))
    return;
  for (int row = 0; row < h; row++) {
    Bit32u addr = r2d_dst_addr(x, y + row, bpp);
    for (int col = 0; col < w; col++) {
      Bit32u a = addr + (Bit32u)col * bpp;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x + col, y + row))
        continue;
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        if (!r2d_ccmp_px(&cc, 0, dpx))
          continue;
      }
      for (int b = 0; b < bpp; b++)
        r2d_store(r128_surf_at(&sd, a + (Bit32u)b), (Bit8u)(color >> ((b & 3) * 8)), (Bit8u)(wmask >> ((b & 3) * 8)));
    }
    r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
  }
  r2d_surf_commit(&sd);
}

Bit32u bx_rage128_c::r2d_grad_px(int kx, int ky)
{
  int ch[3];
  for (int c = 0; c < 3; c++) {
    Bit64s acc = (Bit64s)grad_start[c] + (Bit64s)grad_slope_x[c] * kx + (Bit64s)grad_slope_y[c] * ky;
    int v8 = (int)(acc >> 16);
    ch[c] = v8 < 0 ? 0 : (v8 > 255 ? 255 : v8);
  }
  return r2d_pack_rgb(ch[0], ch[1], ch[2]);
}

void bx_rage128_c::r2d_fill_gradient(int x, int y, int w, int h)
{
  int bpp = r2d_bpp(DP_DST_DT());
  Bit32u wmask = dp_write_mask;
  bool aux_on = (aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  struct r128_ccmp cc;
  int cca = r2d_ccmp_setup(true, bpp, bpp, &cc);
  r128_surf_t sd;
  int cx0, cy0, cx1, cy1;
  int kx0 = 0, ky0 = 0;

  r2d_clip(&cx0, &cy0, &cx1, &cy1);
  if (x < cx0) { kx0 = cx0 - x; w -= cx0 - x; x = cx0; }
  if (y < cy0) { ky0 = cy0 - y; h -= cy0 - y; y = cy0; }
  if (x + w - 1 > cx1) w = cx1 - x + 1;
  if (y + h - 1 > cy1) h = cy1 - y + 1;
  if ((w <= 0) || (h <= 0))
    return;
  if (!r2d_map_span(&sd, &s2d_dst, dst_offset, dst_pitch, bpp, y, y + h - 1, x + w - 1, true))
    return;
  for (int row = 0; row < h; row++) {
    Bit32u addr = r2d_dst_addr(x, y + row, bpp);
    for (int col = 0; col < w; col++) {
      Bit32u a = addr + (Bit32u)col * bpp;
      Bit32u pix;
      if (aux_on && !rage128_aux_sc_pass_2d(aux_sc_cntl, aux_sc_rect, x + col, y + row))
        continue;
      pix = r2d_grad_px(kx0 + col, ky0 + row);
      if (cca) {
        Bit32u dpx = 0;
        for (int b = 0; b < bpp; b++)
          dpx |= (Bit32u)*r128_surf_at(&sd, a + (Bit32u)b) << (b * 8);
        int cr = r2d_ccmp_px(&cc, pix, dpx);
        if (!cr)
          continue;
        if (cr == 2)
          pix = dpx ^ cc.flip;
      }
      for (int b = 0; b < bpp; b++)
        r2d_store(r128_surf_at(&sd, a + (Bit32u)b), (Bit8u)(pix >> ((b & 3) * 8)), (Bit8u)(wmask >> ((b & 3) * 8)));
    }
    r2d_mark_dirty(&sd, addr, (Bit32u)w * bpp);
  }
  r2d_surf_commit(&sd);
}

// Paint the accumulated register-triggered HOST_DATA stream
void bx_rage128_c::r2d_hostdata_paint(void)
{
  int w = hostdata_w, h = hostdata_h;
  Bit32u have = hostdata_ndw;

  if ((w <= 0) || (h <= 0) || (have == 0))
    return;
  if (hostdata_srcdt == 3) {
    r2d_host_color_rect(hostdata_x, hostdata_y, w, h, (const Bit8u*)hostdata_buf, have * 4);
  } else {
    Bit32u dpw = ((Bit32u)w + 31) >> 5;
    Bit32u bitpitch = dpw * 32;
    if (have < dpw * (Bit32u)h)
      h = (int)(have / dpw);
    if (h > 0)
      r2d_mono_rect(hostdata_x, hostdata_y, w, h, (const Bit8u*)hostdata_buf, bitpitch, NULL, NULL);
  }
}

void bx_rage128_c::r2d_hostdata_word(Bit32u off, Bit32u val)
{
  if (!hostdata_active)
    return;
  if (hostdata_ndw < (Bit32u)(sizeof(hostdata_buf) / sizeof(hostdata_buf[0])))
    hostdata_buf[hostdata_ndw++] = val;
  if (off == RAGE128_HOST_DATA_LAST) {
    r2d_hostdata_paint();
    hostdata_ndw = 0;
    hostdata_active = false;
    gui_idle_event = 1;
  }
}

void bx_rage128_c::r2d_gui_op_run(int w, int h)
{
  Bit32u gmc = dp_gui_master_cntl;
  int src = DP_SRC_SOURCE();

  if ((w <= 0) || (h <= 0))
    return;
  if (gmc & RAGE128_GMC_3D_FCN_EN) {
    Bit32u gnz = grad_start[0] | grad_start[1] | grad_start[2] |
                 (Bit32u)grad_slope_x[0] | (Bit32u)grad_slope_x[1] | (Bit32u)grad_slope_x[2] |
                 (Bit32u)grad_slope_y[0] | (Bit32u)grad_slope_y[1] | (Bit32u)grad_slope_y[2];
    if (grad_valid && gnz)
      r2d_fill_gradient(gui_dst_x, gui_dst_y, w, h);
    else
      r2d_fill_solid(gui_dst_x, gui_dst_y, w, h, dp_brush_frgd_clr);
    return;
  }
  if (src == 2) {
    int sx = gui_src_x, sy = gui_src_y;
    int dx = gui_dst_x, dy = gui_dst_y;
    if (!(dp_cntl & RAGE128_DP_CNTL_DST_X_DIR)) { sx -= w - 1; dx -= w - 1; }
    if (!(dp_cntl & RAGE128_DP_CNTL_DST_Y_DIR)) { sy -= h - 1; dy -= h - 1; }
    Bit32u bt = DP_BRUSH_DT();
    bool pat_mono = (bt == 0) || (bt == 1);
    r2d_blit_rect_pat(sx, sy, dx, dy, w, h, pat_mono ? (const Bit8u*)brush_data : NULL, NULL, bt == 1);
  } else if (src >= 3) {
    hostdata_active = true;
    hostdata_x = gui_dst_x;
    hostdata_y = gui_dst_y;
    hostdata_w = w;
    hostdata_h = h;
    hostdata_srcdt = DP_SRC_DT();
    hostdata_ndw = 0;
  } else {
    Bit32u bt = DP_BRUSH_DT();
    bool pat_mono = (bt == 0) || (bt == 1);
    r2d_paint_rect(gui_dst_x, gui_dst_y, w, h, pat_mono ? (const Bit8u*)brush_data : NULL, NULL, bt == 1);
  }
}

void bx_rage128_c::r2d_gui_op(int w, int h)
{
  if (on_cce_thread())
    raster_flush();
  r2d_gui_op_run(w, h);
  gui_idle_event = 1;
}

// ---------------------------------------------------------------------
// Register file interface
// ---------------------------------------------------------------------

bool bx_rage128_c::r2d_reg_read(Bit32u off, Bit32u *val)
{
  if ((off >= RAGE128_BRUSH_DATA0) && (off < RAGE128_BRUSH_DATA0 + 32 * 4)) {
    *val = brush_data[(off - RAGE128_BRUSH_DATA0) >> 2];
    return true;
  }
  if ((off >= RAGE128_AUX1_SC_LEFT) && (off <= RAGE128_AUX3_SC_BOTTOM)) {
    Bit32u idx = (off - RAGE128_AUX1_SC_LEFT) >> 2;
    *val = aux_sc_rect[idx >> 2][idx & 3];
    return true;
  }
  if ((off >= RAGE128_GUI_SCRATCH_REG0) && (off < RAGE128_GUI_SCRATCH_REG0 + 6 * 4)) {
    *val = gui_scratch[(off - RAGE128_GUI_SCRATCH_REG0) >> 2];
    return true;
  }
  switch (off) {
    case RAGE128_DP_GUI_MASTER_CNTL: *val = dp_gui_master_cntl; return true;
    case RAGE128_DP_BRUSH_BKGD_CLR:  *val = dp_brush_bkgd_clr; return true;
    case RAGE128_DP_BRUSH_FRGD_CLR:  *val = dp_brush_frgd_clr; return true;
    case RAGE128_BRUSH_Y_X:          *val = brush_yx; return true;
    case RAGE128_DP_SRC_FRGD_CLR:    *val = dp_src_frgd_clr; return true;
    case RAGE128_DP_SRC_BKGD_CLR:    *val = dp_src_bkgd_clr; return true;
    case RAGE128_CLR_CMP_CLR_SRC:    *val = clr_cmp_clr_src; return true;
    case RAGE128_CLR_CMP_CLR_DST:    *val = clr_cmp_clr_dst; return true;
    case RAGE128_CLR_CMP_CNTL:       *val = clr_cmp_cntl; return true;
    case RAGE128_CLR_CMP_MASK:       *val = clr_cmp_mask; return true;
    case RAGE128_AUX_SC_CNTL:        *val = aux_sc_cntl; return true;
    case RAGE128_DP_CNTL:            *val = dp_cntl; return true;
    case RAGE128_DP_DATATYPE:        *val = dp_datatype; return true;
    case RAGE128_DP_MIX:             *val = dp_mix; return true;
    case RAGE128_DP_WRITE_MASK:      *val = dp_write_mask; return true;
    case RAGE128_DEFAULT_OFFSET:     *val = default_offset; return true;
    case RAGE128_DEFAULT_PITCH:      *val = default_pitch; return true;
    case RAGE128_DEFAULT_SC_BOTTOM_RIGHT: *val = default_sc_bottom_right; return true;
    case RAGE128_SC_TOP_LEFT:        *val = sc_top_left; return true;
    case RAGE128_SC_BOTTOM_RIGHT:    *val = sc_bottom_right; return true;
    case RAGE128_DST_OFFSET:  *val = dst_offset & 0x03fffff0; return true;
    case RAGE128_SRC_OFFSET:  *val = src_offset & 0x03fffff0; return true;
    case RAGE128_DST_PITCH:   *val = dst_pitch_reg; return true;
    case RAGE128_SRC_PITCH:   *val = src_pitch_reg; return true;
    case RAGE128_DST_WIDTH:   *val = gui_dst_w; return true;
    case RAGE128_DST_HEIGHT:  *val = gui_dst_h; return true;
    case RAGE128_DST_X:       *val = (Bit32u)gui_dst_x & 0x3fff; return true;
    case RAGE128_DST_Y:       *val = (Bit32u)gui_dst_y & 0x3fff; return true;
    case RAGE128_SRC_X:       *val = (Bit32u)gui_src_x & 0x3fff; return true;
    case RAGE128_SRC_Y:       *val = (Bit32u)gui_src_y & 0x3fff; return true;
    case RAGE128_SC_LEFT:     *val = sc_top_left & 0x3fff; return true;
    case RAGE128_SC_TOP:      *val = (sc_top_left >> 16) & 0x3fff; return true;
    case RAGE128_SC_RIGHT:    *val = sc_bottom_right & 0x3fff; return true;
    case RAGE128_SC_BOTTOM:   *val = (sc_bottom_right >> 16) & 0x3fff; return true;
    case RAGE128_SRC_SC_RIGHT:  *val = src_sc_right; return true;
    case RAGE128_SRC_SC_BOTTOM: *val = src_sc_bottom; return true;
    case RAGE128_PC_GUI_CTLSTAT: *val = pm4_active() ? 0xa0000000 : 0; return true;
    case RAGE128_SCALE_SCR_HEIGHT_WIDTH: *val = scale_scr_height_width; return true;
    case 0x1a40: *val = (Bit32u)grad_slope_x[0]; return true;
    case 0x1a44: *val = (Bit32u)grad_slope_y[0]; return true;
    case 0x1a48: *val = grad_start[0]; return true;
    case 0x1a4c: *val = (Bit32u)grad_slope_x[1]; return true;
    case 0x1a50: *val = (Bit32u)grad_slope_y[1]; return true;
    case 0x1a54: *val = grad_start[1]; return true;
    case 0x1a58: *val = (Bit32u)grad_slope_x[2]; return true;
    case 0x1a5c: *val = (Bit32u)grad_slope_y[2]; return true;
    case 0x1a60: *val = grad_start[2]; return true;
    default:
      break;
  }
  return false;
}

bool bx_rage128_c::r2d_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
#define MERGE(field) ((field) = ((field) & ~mask) | (val & mask))
  if ((off >= RAGE128_BRUSH_DATA0) && (off < RAGE128_BRUSH_DATA0 + 32 * 4)) {
    MERGE(brush_data[(off - RAGE128_BRUSH_DATA0) >> 2]);
    return true;
  }
  if ((off >= RAGE128_AUX1_SC_LEFT) && (off <= RAGE128_AUX3_SC_BOTTOM)) {
    Bit32u idx = (off - RAGE128_AUX1_SC_LEFT) >> 2;
    Bit32u *r = &aux_sc_rect[idx >> 2][idx & 3];
    *r = ((*r & ~mask) | (val & mask)) & 0x3fff;
    return true;
  }
  if ((off >= RAGE128_GUI_SCRATCH_REG0) && (off < RAGE128_GUI_SCRATCH_REG0 + 6 * 4)) {
    MERGE(gui_scratch[(off - RAGE128_GUI_SCRATCH_REG0) >> 2]);
    return true;
  }
  if ((off >= RAGE128_HOST_DATA0) && (off <= RAGE128_HOST_DATA_LAST)) {
    r2d_hostdata_word(off, val & mask);
    return true;
  }
  switch (off) {
    case RAGE128_DP_GUI_MASTER_CNTL:
      r2d_apply_gmc((dp_gui_master_cntl & ~mask) | (val & mask));
      return true;
    case RAGE128_SRC_PITCH_OFFSET:
      r2d_set_pitch_offset(false, val & mask);
      return true;
    case RAGE128_DST_PITCH_OFFSET:
      r2d_set_pitch_offset(true, val & mask);
      return true;
    case 0x1a40: grad_slope_x[0] = (Bit32s)(((Bit32u)grad_slope_x[0] & ~mask) | (val & mask)); grad_valid = true; return true;
    case 0x1a44: grad_slope_y[0] = (Bit32s)(((Bit32u)grad_slope_y[0] & ~mask) | (val & mask)); return true;
    case 0x1a48: MERGE(grad_start[0]); return true;
    case 0x1a4c: grad_slope_x[1] = (Bit32s)(((Bit32u)grad_slope_x[1] & ~mask) | (val & mask)); grad_valid = true; return true;
    case 0x1a50: grad_slope_y[1] = (Bit32s)(((Bit32u)grad_slope_y[1] & ~mask) | (val & mask)); return true;
    case 0x1a54: MERGE(grad_start[1]); return true;
    case 0x1a58: grad_slope_x[2] = (Bit32s)(((Bit32u)grad_slope_x[2] & ~mask) | (val & mask)); grad_valid = true; return true;
    case 0x1a5c: grad_slope_y[2] = (Bit32s)(((Bit32u)grad_slope_y[2] & ~mask) | (val & mask)); return true;
    case 0x1a60: MERGE(grad_start[2]); return true;
    case RAGE128_SRC_Y_X:
      gui_src_x = rage128_sx14(val & mask);
      gui_src_y = rage128_sx14((val & mask) >> 16);
      return true;
    case RAGE128_DST_Y_X:
      gui_dst_x = rage128_sx14(val & mask);
      gui_dst_y = rage128_sx14((val & mask) >> 16);
      return true;
    case RAGE128_SRC_X_Y:
      gui_src_x = rage128_sx14((val & mask) >> 16);
      gui_src_y = rage128_sx14(val & mask);
      return true;
    case RAGE128_DST_X_Y:
      gui_dst_x = rage128_sx14((val & mask) >> 16);
      gui_dst_y = rage128_sx14(val & mask);
      return true;
    case RAGE128_DST_WIDTH_HEIGHT:
      gui_dst_w = ((val & mask) >> 16) & 0x3fff;
      gui_dst_h = (val & mask) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      return true;
    case RAGE128_DST_HEIGHT_WIDTH:
      gui_dst_w = (val & mask) & 0x3fff;
      gui_dst_h = ((val & mask) >> 16) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      return true;
    case RAGE128_DST_OFFSET:
      dst_offset = (val & mask) & 0x03fffff0;
      return true;
    case RAGE128_SRC_OFFSET:
      src_offset = (val & mask) & 0x03fffff0;
      return true;
    case RAGE128_DST_PITCH: {
      Bit32u v = (val & mask) & 0x000703ff;
      Bit32u adj = (v >> 17) & 3;
      dst_pitch_reg = v;
      dst_pitch = (v & 0x3ff) * 8u * ((adj == 1) ? 2u : (adj == 2) ? 4u : 1u);
      return true;
    }
    case RAGE128_SRC_PITCH: {
      Bit32u v = (val & mask) & 0x000103ff;
      src_pitch_reg = v;
      src_pitch = (v & 0x3ff) * 8u;
      return true;
    }
    case RAGE128_DST_WIDTH:  gui_dst_w = (val & mask) & 0x3fff; return true;
    case RAGE128_DST_HEIGHT: gui_dst_h = (val & mask) & 0x3fff; return true;
    case RAGE128_DST_X: gui_dst_x = rage128_sx14(val & mask); return true;
    case RAGE128_DST_Y: gui_dst_y = rage128_sx14(val & mask); return true;
    case RAGE128_SRC_X: gui_src_x = rage128_sx14(val & mask); return true;
    case RAGE128_SRC_Y: gui_src_y = rage128_sx14(val & mask); return true;
    case RAGE128_SC_LEFT:
      sc_top_left = (sc_top_left & ~0x3fffu) | ((val & mask) & 0x3fff);
      return true;
    case RAGE128_SC_TOP:
      sc_top_left = (sc_top_left & ~(0x3fffu << 16)) | (((val & mask) & 0x3fff) << 16);
      return true;
    case RAGE128_SC_RIGHT:
      sc_bottom_right = (sc_bottom_right & ~0x3fffu) | ((val & mask) & 0x3fff);
      return true;
    case RAGE128_SC_BOTTOM:
      sc_bottom_right = (sc_bottom_right & ~(0x3fffu << 16)) | (((val & mask) & 0x3fff) << 16);
      return true;
    case RAGE128_SRC_SC_RIGHT:  src_sc_right = (val & mask) & 0x3fff; return true;
    case RAGE128_SRC_SC_BOTTOM: src_sc_bottom = (val & mask) & 0x3fff; return true;
    case RAGE128_SRC_SC_BOTTOM_RIGHT:
      src_sc_right = (val & mask) & 0x3fff;
      src_sc_bottom = ((val & mask) >> 16) & 0x3fff;
      return true;
    case RAGE128_DST_WIDTH_X:
      gui_dst_x = rage128_sx14(val & mask);
      gui_dst_w = ((val & mask) >> 16) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      return true;
    case RAGE128_DST_WIDTH_X_INCY:
      gui_dst_x = rage128_sx14(val & mask);
      gui_dst_w = ((val & mask) >> 16) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      gui_dst_y += (Bit32s)gui_dst_h;
      return true;
    case RAGE128_DST_HEIGHT_Y:
      gui_dst_y = rage128_sx14(val & mask);
      gui_dst_h = ((val & mask) >> 16) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      return true;
    case RAGE128_DST_WIDTH_BW:
      gui_dst_w = (val & mask) & 0x3fff;
      r2d_gui_op((int)gui_dst_w, (int)gui_dst_h);
      return true;
    case RAGE128_DST_BRES_ERR:  MERGE(bres_err);  return true;
    case RAGE128_DST_BRES_INC:  MERGE(bres_inc);  return true;
    case RAGE128_DST_BRES_DEC:  MERGE(bres_dec);  return true;
    case RAGE128_DP_CNTL_XDIR_YDIR_YMAJOR:
      MERGE(dp_cntl_line);
      return true;
    case RAGE128_DST_BRES_LNTH:
      MERGE(bres_lnth);
      if (on_cce_thread())
        raster_flush();
      r2d_bres_line(dp_cntl_line);
      gui_idle_event = 1;
      return true;
    case RAGE128_DP_BRUSH_BKGD_CLR: MERGE(dp_brush_bkgd_clr); return true;
    case RAGE128_DP_BRUSH_FRGD_CLR: MERGE(dp_brush_frgd_clr); return true;
    case RAGE128_BRUSH_Y_X:         MERGE(brush_yx); return true;
    case RAGE128_DP_SRC_FRGD_CLR:   MERGE(dp_src_frgd_clr); return true;
    case RAGE128_DP_SRC_BKGD_CLR:   MERGE(dp_src_bkgd_clr); return true;
    case RAGE128_CLR_CMP_CLR_SRC:   MERGE(clr_cmp_clr_src); return true;
    case RAGE128_CLR_CMP_CLR_DST:   MERGE(clr_cmp_clr_dst); return true;
    case RAGE128_CLR_CMP_CNTL:      MERGE(clr_cmp_cntl); return true;
    case RAGE128_CLR_CMP_MASK:      MERGE(clr_cmp_mask); return true;
    case RAGE128_AUX_SC_CNTL:       MERGE(aux_sc_cntl); return true;
    case RAGE128_DP_CNTL:           MERGE(dp_cntl); return true;
    case RAGE128_DP_DATATYPE:       MERGE(dp_datatype); return true;
    case RAGE128_DP_MIX:            MERGE(dp_mix); return true;
    case RAGE128_DP_WRITE_MASK:     MERGE(dp_write_mask); return true;
    case RAGE128_DEFAULT_OFFSET:    MERGE(default_offset); return true;
    case RAGE128_DEFAULT_PITCH:     MERGE(default_pitch); return true;
    case RAGE128_DEFAULT_SC_BOTTOM_RIGHT: MERGE(default_sc_bottom_right); return true;
    case RAGE128_SC_TOP_LEFT:       MERGE(sc_top_left); return true;
    case RAGE128_SC_BOTTOM_RIGHT:   MERGE(sc_bottom_right); return true;
    case RAGE128_PC_GUI_CTLSTAT:    return true;
    case RAGE128_SCALE_SCR_HEIGHT_WIDTH: MERGE(scale_scr_height_width); return true;
    default:
      break;
  }
#undef MERGE
  return false;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_RAGE128
