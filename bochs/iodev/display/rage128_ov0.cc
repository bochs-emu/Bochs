/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation: OV0 hardware video overlay window and
//  front-end scaler (register latch model, double-buffered through the
//  OV0_REG_LOAD_CNTL lock) plus the DVD subpicture block. The compositor
//  scales the YUV/RGB source window into the display tiles.
//  Ported from the 86Box Rage 128 Pro emulation (vid_ati_rage128_ov0.c).
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

#define OV0_ACT(off) (ov0.active[RAGE128_OV0_REG(off)])

// ---------------------------------------------------------------------
// Latch model
// ---------------------------------------------------------------------

void bx_rage128_c::ov0_apply(void)
{
  memcpy(ov0.active, ov0.shadow, sizeof(ov0.active));
  ov0_update();
}

void bx_rage128_c::ov0_update(void)
{
  Bit32u scale = OV0_ACT(RAGE128_OV0_SCALE_CNTL);
  Bit32u strt = OV0_ACT(RAGE128_OV0_Y_X_START);
  Bit32u end = OV0_ACT(RAGE128_OV0_Y_X_END);
  bool was = ov0_enabled;

  ov0_x1 = strt & 0xfff;
  ov0_y1 = (strt >> 16) & 0xfff;
  ov0_x2 = end & 0xfff;
  ov0_y2 = (end >> 16) & 0xfff;
  ov0_enabled = (scale & RAGE128_OV0_SCALER_ENABLE) && !(scale & RAGE128_OV0_SCALER_SOFT_RESET) &&
                disp_ext && (ov0_y2 > ov0_y1) && (ov0_x2 > ov0_x1);
  if (was || ov0_enabled)
    needs_update_dispentire = true;
}

bool bx_rage128_c::ov0_reg_read(Bit32u off, Bit32u *val)
{
  if ((off < RAGE128_OV0_BLOCK_BASE) || (off > RAGE128_OV0_BLOCK_END))
    return false;
  if (off == RAGE128_OV0_REG_LOAD_CNTL) {
    *val = (ov0_reg_load_cntl & RAGE128_OV0_REG_LD_CTL_LOCK)
      ? (ov0_reg_load_cntl | RAGE128_OV0_REG_LD_CTL_LOCK_READBACK)
      : (ov0_reg_load_cntl & ~RAGE128_OV0_REG_LD_CTL_LOCK_READBACK);
    return true;
  }
  *val = ov0.shadow[RAGE128_OV0_REG(off)];
  return true;
}

bool bx_rage128_c::ov0_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
  int idx;
  if ((off < RAGE128_OV0_BLOCK_BASE) || (off > RAGE128_OV0_BLOCK_END))
    return false;
  if (off == RAGE128_OV0_REG_LOAD_CNTL) {
    bool was_locked = (ov0_reg_load_cntl & RAGE128_OV0_REG_LD_CTL_LOCK) != 0;
    ov0_reg_load_cntl = ((ov0_reg_load_cntl & ~mask) | (val & mask)) & ~RAGE128_OV0_REG_LD_CTL_LOCK_READBACK;
    if (was_locked && !(ov0_reg_load_cntl & RAGE128_OV0_REG_LD_CTL_LOCK))
      ov0_apply();
    return true;
  }
  idx = RAGE128_OV0_REG(off);
  ov0.shadow[idx] = (ov0.shadow[idx] & ~mask) | (val & mask);
  if (!(ov0_reg_load_cntl & RAGE128_OV0_REG_LD_CTL_LOCK)) {
    ov0.active[idx] = ov0.shadow[idx];
    if ((off == RAGE128_OV0_SCALE_CNTL) || (off == RAGE128_OV0_Y_X_START) || (off == RAGE128_OV0_Y_X_END))
      ov0_update();
    else if (ov0_enabled)
      needs_update_dispentire = true;
  }
  return true;
}

void bx_rage128_c::ov0_reset(void)
{
  memset(&ov0, 0, sizeof(ov0));
  memset(&subpic, 0, sizeof(subpic));
  ov0_reg_load_cntl = 0;
  ov0_enabled = false;
  ov0_x1 = ov0_y1 = ov0_x2 = ov0_y2 = 0;
}

// Once per scanout frame: commit the subpicture shadow set and repaint
// a live overlay window (its source buffers change without VRAM tracking)
void bx_rage128_c::ov0_frame_latch(void)
{
  if (subpic.frame_stamp != frame_count) {
    subpic.frame_stamp = frame_count;
    memcpy(subpic.active, subpic.regs, sizeof(subpic.active));
    memcpy(subpic.apal, subpic.pal, sizeof(subpic.apal));
  }
  if (ov0_enabled && disp_ext) {
    int x1 = ov0_x1, y1 = ov0_y1;
    int x2 = ov0_x2, y2 = ov0_y2;
    if (x2 > (int)disp_xres) x2 = disp_xres;
    if (y2 > (int)disp_yres) y2 = disp_yres;
    if ((x2 > x1) && (y2 > y1))
      redraw_area((Bit32s)x1, (Bit32s)y1, (Bit32u)(x2 - x1), (Bit32u)(y2 - y1));
  }
}

// ---------------------------------------------------------------------
// DVD subpicture block (0x0540-0x0588)
// ---------------------------------------------------------------------

bool bx_rage128_c::subpic_reg_read(Bit32u off, Bit32u *val)
{
  if ((off < RAGE128_SUBPIC_BLOCK_BASE) || (off > RAGE128_SUBPIC_BLOCK_END))
    return false;
  if (off == RAGE128_SUBPIC_PALETTE_DATA)
    *val = subpic.pal[subpic.regs[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_PALETTE_INDEX)] & 0xf];
  else if (off == RAGE128_SUBPIC_CNTL)
    *val = subpic.regs[0] & ~RAGE128_SUBPIC_CNTL_NEW_FRAME;
  else
    *val = subpic.regs[RAGE128_SUBPIC_REG(off)];
  return true;
}

bool bx_rage128_c::subpic_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
  Bit32u idx;
  if ((off < RAGE128_SUBPIC_BLOCK_BASE) || (off > RAGE128_SUBPIC_BLOCK_END))
    return false;
  idx = RAGE128_SUBPIC_REG(off);
  subpic.regs[idx] = (subpic.regs[idx] & ~mask) | (val & mask);
  if (off == RAGE128_SUBPIC_PALETTE_DATA)
    subpic.pal[subpic.regs[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_PALETTE_INDEX)] & 0xf] = subpic.regs[idx];
  return true;
}

// Blend the subpicture into one video pixel (YCbCr, before CSC)
void bx_rage128_c::subpic_blend(int out_x, int out_row, int *y, int *cb, int *cr)
{
  Bit32u *r = subpic.active;

  if (!(r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_CNTL)] & RAGE128_SUBPIC_CNTL_DISPLAY_EN))
    return;
  Bit32u pitch = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_PITCH_LENGTH)] & 0xffff;
  if (!pitch)
    return;
  int su = (int)((r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_H_ACC)] + (Bit32u)out_x * r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_H_STEP)]) >> 16);
  int sv = (int)((r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_V_ACC)] + (Bit32u)out_row * r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_V_STEP)]) >> 16);
  Bit32u das = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_DAREA_START)];
  Bit32u dae = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_DAREA_END)];
  int x0 = das & 0x3ff, y0 = (das >> 16) & 0x3ff;
  int x1 = dae & 0x3ff, y1 = (dae >> 16) & 0x3ff;
  if ((su < x0) || (su > x1) || (sv < y0) || (sv > y1))
    return;
  Bit32u a = (r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_PXD_A)] + (Bit32u)sv * pitch + ((Bit32u)su >> 2)) & vram_mask;
  int cl = (BX_RAGE128_THIS s.memory[a] >> ((su & 3) * 2)) & 3;
  Bit32u cc = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_COLOR_CONTRAST)];
  Bit32u hlt = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_HL_TOP)];
  Bit32u hlb = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_HL_BOTTOM)];
  if (((hlb & 0x3ff) > (hlt & 0x3ff)) && (((hlb >> 16) & 0x3ff) > ((hlt >> 16) & 0x3ff)) &&
      (su >= (int)(hlt & 0x3ff)) && (su <= (int)(hlb & 0x3ff)) &&
      (sv >= (int)((hlt >> 16) & 0x3ff)) && (sv <= (int)((hlb >> 16) & 0x3ff)))
    cc = r[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_HL_COLOR_CONTRAST)];
  int k = (cc >> (cl * 4)) & 0xf;
  if (!k)
    return;
  Bit32u pe = subpic.apal[(cc >> (16 + cl * 4)) & 0xf];
  int sy = (pe >> 16) & 0xff, scb = (pe >> 8) & 0xff, scr = pe & 0xff;
  *y = (*y * (15 - k) + sy * k + 7) / 15;
  *cb = (*cb * (15 - k) + scb * k + 7) / 15;
  *cr = (*cr * (15 - k) + scr * k + 7) / 15;
}

// ---------------------------------------------------------------------
// Compositor
// ---------------------------------------------------------------------

static BX_CPP_INLINE Bit32u ov0_h_accum_init(Bit32u reg, Bit32u int_mask)
{
  return (((reg >> 28) & int_mask) << 20) | (reg & 0x000f8000);
}

static BX_CPP_INLINE Bit32u ov0_yuv_to_rgb(int y, int cb, int cr)
{
  int c = y - 16, d = cb - 128, e = cr - 128;
  int r = (298 * c + 409 * e + 128) >> 8;
  int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
  int b = (298 * c + 516 * d + 128) >> 8;
  if (r < 0) r = 0; else if (r > 255) r = 255;
  if (g < 0) g = 0; else if (g > 255) g = 255;
  if (b < 0) b = 0; else if (b > 255) b = 255;
  return ((Bit32u)r << 16) | ((Bit32u)g << 8) | (Bit32u)b;
}

static BX_CPP_INLINE int ov0_key_fn(Bit32u fn, int eq)
{
  switch (fn & 7) {
    case 1: return 1;
    case 4: return eq;
    case 5: return !eq;
    default: return 0;
  }
}

// Graphics key compares the raw framebuffer pixel, before the LUT/DAC
bool bx_rage128_c::ov0_gfx_key_match(int x, int y)
{
  Bit32u clr = OV0_ACT(RAGE128_OV0_GRAPHICS_KEY_CLR);
  Bit32u msk = OV0_ACT(RAGE128_OV0_GRAPHICS_KEY_MSK);
  int bpb = (disp_bpp == 15) ? 2 : ((disp_bpp + 7) >> 3);
  unsigned sy = disp_dblscan ? (y >> 1) : y;
  Bit32u ma = disp_base + sy * disp_pitch + (Bit32u)x * (Bit32u)bpb;
  Bit32u px;
  Bit8u *vram = BX_RAGE128_THIS s.memory;
  switch (bpb) {
    case 1:
      px = vram[ma & vram_mask];
      break;
    case 2:
      px = vram[ma & vram_mask] | ((Bit32u)vram[(ma + 1) & vram_mask] << 8);
      break;
    default:
      px = vram[ma & vram_mask] | ((Bit32u)vram[(ma + 1) & vram_mask] << 8) | ((Bit32u)vram[(ma + 2) & vram_mask] << 16);
      break;
  }
  return ((px ^ clr) & msk) == 0;
}

// Composite the overlay window into one display tile (or the snapshot)
void bx_rage128_c::draw_overlay(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  if (!ov0_enabled || disp_dac_const || disp_blank)
    return;

  Bit32u scale = OV0_ACT(RAGE128_OV0_SCALE_CNTL);
  int fmt = (scale & RAGE128_OV0_SCALER_SURFAC_FORMAT) >> RAGE128_OV0_SCALER_FORMAT_SHIFT;
  Bit8u *vram = BX_RAGE128_THIS s.memory;
  Bit32u vmask = vram_mask;

  switch (fmt) {
    case RAGE128_OV0_SCALER_SOURCE_15BPP:
    case RAGE128_OV0_SCALER_SOURCE_16BPP:
    case RAGE128_OV0_SCALER_SOURCE_32BPP:
    case RAGE128_OV0_SCALER_SOURCE_YUV12:
    case RAGE128_OV0_SCALER_SOURCE_VYUY422:
    case RAGE128_OV0_SCALER_SOURCE_YVYU422:
      break;
    default:
      return;
  }

  unsigned w, h;
  Bit8u *tile_ptr;
  if (info->snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    w = disp_xres;
    h = disp_yres;
  } else {
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  }
  if (tile_ptr == NULL)
    return;
  if (info->bpp == 15) info->bpp = 16;
  unsigned tbytes = info->bpp / 8;

  int x1 = ov0_x1, x2 = ov0_x2, y1 = ov0_y1, y2 = ov0_y2;
  if (x2 > (int)disp_xres) x2 = disp_xres;
  if (y2 > (int)disp_yres) y2 = disp_yres;
  int tx0 = (int)xc, ty0 = (int)yc, tx1 = (int)(xc + w), ty1 = (int)(yc + h);
  int cx0 = x1 > tx0 ? x1 : tx0;
  int cy0 = y1 > ty0 ? y1 : ty0;
  int cx1 = x2 < tx1 ? x2 : tx1;
  int cy1 = y2 < ty1 ? y2 : ty1;
  if ((cx0 >= cx1) || (cy0 >= cy1))
    return;

  // Scale factors
  Bit32u h_inc_reg = OV0_ACT(RAGE128_OV0_H_INC);
  Bit32u step_reg = OV0_ACT(RAGE128_OV0_STEP_BY);
  Bit32u v_inc = OV0_ACT(RAGE128_OV0_V_INC);
  int ecp_div = (pll_regs[RAGE128_PLL_VCLK_ECP_CNTL] >> RAGE128_ECP_DIV_SHIFT) & 3;
  int step_y = step_reg & 0xff;
  int step_c = (step_reg >> 8) & 0xff;
  if (step_y < 1) step_y = 1;
  if (step_c < 1) step_c = 1;
  Bit32u h_step_y = ((h_inc_reg & 0xffff) << (step_y - 1)) << (8 - ecp_div);
  Bit32u h_step_c = ((h_inc_reg >> 16) << (step_c - 1)) << (8 - ecp_div);
  Bit32u h_acc0_y = ov0_h_accum_init(OV0_ACT(RAGE128_OV0_P1_H_ACCUM_INIT), 0xf);
  Bit32u h_acc0_c = ov0_h_accum_init(OV0_ACT(RAGE128_OV0_P23_H_ACCUM_INIT), 0x7);
  Bit32u h_bias_y = ((0x28000 + ((h_inc_reg & 0xffff) << 3)) & 0xff800) << 4;
  Bit32u h_bias_c = ((0x28000 + (((h_inc_reg >> 16) & 0xffff) << 3)) & 0xff800) << 4;
  Bit32u h_rel0_y = (h_acc0_y > h_bias_y) ? h_acc0_y - h_bias_y : 0;
  Bit32u h_rel0_c = (h_acc0_c > h_bias_c) ? h_acc0_c - h_bias_c : 0;
  Bit32u p1_se = OV0_ACT(RAGE128_OV0_P1_X_START_END);
  Bit32u p2_se = OV0_ACT(RAGE128_OV0_P2_X_START_END);
  int p1_x0 = (p1_se >> 16) & 0x7f;
  int p1_xe = p1_se & 0xfff;
  int p2_x0 = (p2_se >> 16) & 0x7f;
  int p2_xe = p2_se & 0xfff;
  Bit32u v_acc0_y = OV0_ACT(RAGE128_OV0_P1_V_ACCUM_INIT) & 0x03ff8000;
  Bit32u v_acc0_c = OV0_ACT(RAGE128_OV0_P23_V_ACCUM_INIT) & 0x01ff8000;
  int src_h_y = ((OV0_ACT(RAGE128_OV0_P1_BLANK_LINES_AT_TOP) >> 16) & 0xfff) + 1;
  int src_h_c = ((OV0_ACT(RAGE128_OV0_P23_BLANK_LINES_AT_TOP) >> 16) & 0x7ff) + 1;

  int buf_n = OV0_ACT(RAGE128_OV0_AUTO_FLIP_CNTL) & 7;
  if (buf_n > 5) buf_n = 5;
  Bit32u buf0 = ov0.active[RAGE128_OV0_REG(RAGE128_OV0_VID_BUF0_BASE_ADRS) + buf_n];
  Bit32u buf1 = ov0.active[RAGE128_OV0_REG(RAGE128_OV0_VID_BUF0_BASE_ADRS) + ((buf_n < 4) ? buf_n + 1 : 5)];
  Bit32u buf2 = ov0.active[RAGE128_OV0_REG(RAGE128_OV0_VID_BUF0_BASE_ADRS) + ((buf_n < 3) ? buf_n + 2 : 5)];
  Bit32u pitch0 = OV0_ACT(RAGE128_OV0_VID_BUF_PITCH0_VALUE) & 0xfffff;
  Bit32u pitch1 = OV0_ACT(RAGE128_OV0_VID_BUF_PITCH1_VALUE) & 0xfffff;

  Bit32u colour_cntl = OV0_ACT(RAGE128_OV0_COLOUR_CNTL);
  int bright = (int)((Bit8s)((colour_cntl & 0x7f) << 1)) >> 1;
  int sat_u = (colour_cntl >> 8) & 0x1f;
  int sat_v = (colour_cntl >> 16) & 0x1f;
  Bit32u key_cntl = OV0_ACT(RAGE128_OV0_KEY_CNTL);
  Bit32u vkey_fn = key_cntl & RAGE128_OV0_VIDEO_KEY_FN_MASK;
  Bit32u gkey_fn = (key_cntl & RAGE128_OV0_GRAPHIC_KEY_FN_MASK) >> RAGE128_OV0_GRAPHIC_KEY_FN_SHIFT;
  Bit32u vkey_clr = OV0_ACT(RAGE128_OV0_VIDEO_KEY_CLR);
  Bit32u vkey_msk = OV0_ACT(RAGE128_OV0_VIDEO_KEY_MSK);
  bool mix_and = (key_cntl & RAGE128_OV0_CMP_MIX_AND) != 0;
  bool sp_on = (subpic.active[RAGE128_SUBPIC_REG(RAGE128_SUBPIC_CNTL)] & RAGE128_SUBPIC_CNTL_DISPLAY_EN) != 0;

  for (int y = cy0; y < cy1; y++) {
    int out_row = y - y1;
    int src_y = (int)((v_acc0_y + (Bit32u)out_row * v_inc - 0x180000) >> 20);
    if (src_y < 0) src_y = 0;
    if (src_y > src_h_y - 1) src_y = src_h_y - 1;
    int src_yc = (int)((v_acc0_c + (Bit32u)out_row * (v_inc >> 1) - 0x180000) >> 20);
    if (src_yc < 0) src_yc = 0;
    if (src_yc > src_h_c - 1) src_yc = src_h_c - 1;
    Bit32u base_y = (buf0 & 0xfffffff0) + (Bit32u)src_y * ((buf0 & 1) ? pitch1 : pitch0);
    Bit32u base_u = (buf1 & 0xfffffff0) + (Bit32u)src_yc * ((buf1 & 1) ? pitch1 : pitch0);
    Bit32u base_v = (buf2 & 0xfffffff0) + (Bit32u)src_yc * ((buf2 & 1) ? pitch1 : pitch0);
    Bit8u *tp = tile_ptr + info->pitch * (y - ty0) + tbytes * (cx0 - tx0);

    for (int x = cx0; x < cx1; x++) {
      int i = x - x1;
      int sx = p1_x0 + (int)((h_rel0_y + (Bit32u)i * h_step_y) >> 20);
      int sxc;
      Bit32u rgb, vpx = 0;
      int yy = 0, cb = 0, cr = 0;

      if (sx < 0) sx = 0;
      if (sx > p1_xe) sx = p1_xe;
      switch (fmt) {
        case RAGE128_OV0_SCALER_SOURCE_YUV12:
          sxc = p2_x0 + (int)((h_rel0_c + (Bit32u)i * h_step_c) >> 20);
          if (sxc < 0) sxc = 0;
          if (sxc > p2_xe) sxc = p2_xe;
          yy = vram[(base_y + sx) & vmask];
          cb = vram[(base_u + sxc) & vmask];
          cr = vram[(base_v + sxc) & vmask];
          vpx = ((Bit32u)cr << 16) | ((Bit32u)cb << 8) | yy;
          break;
        case RAGE128_OV0_SCALER_SOURCE_VYUY422:
          yy = vram[(base_y + sx * 2) & vmask];
          cb = vram[(base_y + (sx & ~1) * 2 + 1) & vmask];
          cr = vram[(base_y + (sx & ~1) * 2 + 3) & vmask];
          vpx = ((Bit32u)cr << 16) | ((Bit32u)cb << 8) | yy;
          break;
        case RAGE128_OV0_SCALER_SOURCE_YVYU422:
          yy = vram[(base_y + sx * 2 + 1) & vmask];
          cb = vram[(base_y + (sx & ~1) * 2) & vmask];
          cr = vram[(base_y + (sx & ~1) * 2 + 2) & vmask];
          vpx = ((Bit32u)cr << 16) | ((Bit32u)cb << 8) | yy;
          break;
        case RAGE128_OV0_SCALER_SOURCE_15BPP:
        case RAGE128_OV0_SCALER_SOURCE_16BPP:
          vpx = vram[(base_y + sx * 2) & vmask] | ((Bit32u)vram[(base_y + sx * 2 + 1) & vmask] << 8);
          break;
        default: {
          Bit32u a = (base_y + sx * 4) & vmask;
          vpx = vram[a] | ((Bit32u)vram[(a + 1) & vmask] << 8) | ((Bit32u)vram[(a + 2) & vmask] << 16) | ((Bit32u)vram[(a + 3) & vmask] << 24);
          break;
        }
      }
      switch (fmt) {
        case RAGE128_OV0_SCALER_SOURCE_15BPP:
          rgb = (((vpx >> 10) & 0x1f) << 19) | (((vpx >> 10) & 0x1c) << 14) |
                (((vpx >> 5) & 0x1f) << 11) | (((vpx >> 5) & 0x1c) << 6) |
                ((vpx & 0x1f) << 3) | ((vpx & 0x1c) >> 2);
          break;
        case RAGE128_OV0_SCALER_SOURCE_16BPP:
          rgb = (((vpx >> 11) & 0x1f) << 19) | (((vpx >> 11) & 0x1c) << 14) |
                (((vpx >> 5) & 0x3f) << 10) | (((vpx >> 5) & 0x30) << 4) |
                ((vpx & 0x1f) << 3) | ((vpx & 0x1c) >> 2);
          break;
        case RAGE128_OV0_SCALER_SOURCE_32BPP:
          rgb = vpx & 0xffffff;
          break;
        default:
          yy = yy + bright;
          if (yy < 0) yy = 0; else if (yy > 255) yy = 255;
          cb = 128 + (((cb - 128) * sat_u) >> 4);
          cr = 128 + (((cr - 128) * sat_v) >> 4);
          if (sp_on)
            subpic_blend(i, out_row, &yy, &cb, &cr);
          rgb = ov0_yuv_to_rgb(yy, cb, cr);
          break;
      }
      int gcmp = ov0_key_fn(gkey_fn, ov0_gfx_key_match(x, y) ? 1 : 0);
      int vcmp = ov0_key_fn(vkey_fn, (((vpx ^ vkey_clr) & vkey_msk) == 0) ? 1 : 0);
      bool show_gfx = mix_and ? (gcmp && vcmp) : (gcmp || vcmp);
      if (!show_gfx) {
        if (info->is_indexed) {
          tp[0] = (Bit8u)rgb;
        } else {
          Bit32u colour = MAKE_COLOUR(rgb, 24, info->red_shift, info->red_mask,
                                      rgb, 16, info->green_shift, info->green_mask,
                                      rgb, 8, info->blue_shift, info->blue_mask);
          if (info->is_little_endian) {
            for (int k = 0; k < info->bpp; k += 8) *(tp + k / 8) = (Bit8u)(colour >> k);
          } else {
            for (int k = info->bpp - 8, j = 0; k > -8; k -= 8, j++) *(tp + j) = (Bit8u)(colour >> k);
          }
        }
      }
      tp += tbytes;
    }
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_RAGE128
