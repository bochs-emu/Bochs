/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation: 3D engine.
//
//  CCE 3D state registers, draw packet decode, texture sampling and the
//  edge-function rasterizer, plus the parallel scanline-interleaved
//  render worker pool. Ported from the 86Box Rage 128 Pro emulation
//  (vid_ati_rage128_3d.c and vid_ati_rage128_raster.c).
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

#include <math.h>
#include "vgacore.h"
#include "pci.h"
#include "ddc.h"
#include "rage128_regs.h"
#include "rage128.h"

#define LOG_THIS BX_RAGE128_THIS

// ---------------------------------------------------------------------
// Register file
// ---------------------------------------------------------------------

void bx_rage128_c::r3d_reset(void)
{
  memset(&t3d, 0, sizeof(t3d));
  memset(t3d.fog_table, 0xff, sizeof(t3d.fog_table));
  // FPU_SETUP all-zero decodes as cull-both-faces: default to both faces
  // solid + Gouraud (the values the DDX composite path programs)
  t3d.fpu_setup = RAGE128_FPU_BACKFACE_SOLID | RAGE128_FPU_FRONTFACE_SOLID | RAGE128_FPU_COLOR_GOURAUD;
  t3d.plane_3d_mask = 0xffffffff;
  fog_table_wr_index = 0;
  tex_pal_wr_index = 0;
}

bool bx_rage128_c::r3d_reg_read(Bit32u off, Bit32u *val)
{
  if ((off >= RAGE128_PRIM_TEX_OFFSET_C(0)) && (off <= RAGE128_PRIM_TEX_OFFSET_C(10))) {
    *val = t3d.prim_tex_offset[(off - RAGE128_PRIM_TEX_OFFSET_C(0)) >> 2];
    return true;
  }
  if ((off >= RAGE128_SEC_TEX_OFFSET_C(0)) && (off <= RAGE128_SEC_TEX_OFFSET_C(10))) {
    *val = t3d.sec_tex_offset[(off - RAGE128_SEC_TEX_OFFSET_C(0)) >> 2];
    return true;
  }
  switch (off) {
    case RAGE128_SCALE_3D_CNTL:        *val = t3d.scale_3d_cntl; return true;
    case RAGE128_SCALE_3D_DATATYPE:    *val = t3d.scale_3d_datatype; return true;
    case RAGE128_COMPOSITE_SHADOW_ID:  *val = t3d.composite_shadow_id; return true;
    case RAGE128_CLR_CMP_CLR_3D:       *val = t3d.clr_cmp_clr_3d; return true;
    case RAGE128_CLR_CMP_MSK_3D:       *val = t3d.clr_cmp_msk_3d; return true;
    case RAGE128_SETUP_CNTL:           *val = t3d.setup_cntl; return true;
    case RAGE128_WINDOW_XY_OFFSET:     *val = t3d.window_xy_offset; return true;
    case RAGE128_SETUP_CNTL_PM4:       *val = t3d.setup_cntl_pm4; return true;
    case RAGE128_Z_OFFSET_C:           *val = t3d.z_offset; return true;
    case RAGE128_Z_PITCH_C:            *val = t3d.z_pitch; return true;
    case RAGE128_Z_STEN_CNTL_C:        *val = t3d.z_sten_cntl; return true;
    case RAGE128_TEX_CNTL_C:           *val = t3d.tex_cntl; return true;
    case RAGE128_MISC_3D_STATE_CNTL:   *val = t3d.misc_3d_state_cntl; return true;
    case RAGE128_TEX_CLR_CMP_CLR_C:    *val = t3d.tex_clr_cmp_clr; return true;
    case RAGE128_TEX_CLR_CMP_MSK_C:    *val = t3d.tex_clr_cmp_msk; return true;
    case RAGE128_FOG_COLOR_C:          *val = t3d.fog_color; return true;
    case RAGE128_PRIM_TEX_CNTL_C:      *val = t3d.prim_tex_cntl; return true;
    case RAGE128_PRIM_TEX_COMBINE_CNTL_C: *val = t3d.prim_tex_combine_cntl; return true;
    case RAGE128_TEX_SIZE_PITCH_C:     *val = t3d.tex_size_pitch; return true;
    case RAGE128_SEC_TEX_CNTL_C:       *val = t3d.sec_tex_cntl; return true;
    case RAGE128_SEC_TEX_COMBINE_CNTL_C: *val = t3d.sec_tex_combine_cntl; return true;
    case RAGE128_CONSTANT_COLOR_C:     *val = t3d.constant_color; return true;
    case RAGE128_PRIM_TEX_BORDER_COLOR_C: *val = t3d.prim_tex_border_color; return true;
    case RAGE128_SEC_TEX_BORDER_COLOR_C:  *val = t3d.sec_tex_border_color; return true;
    case RAGE128_STEN_REF_MASK_C:
    case RAGE128_STEN_REF_MASK_LEGACY: *val = t3d.sten_ref_mask; return true;
    case RAGE128_PLANE_3D_MASK_C:      *val = t3d.plane_3d_mask; return true;
    case RAGE128_DST_PITCH_OFFSET_C:   return r2d_reg_read(RAGE128_DST_PITCH_OFFSET, val);
    case RAGE128_DP_GUI_MASTER_CNTL_C: return r2d_reg_read(RAGE128_DP_GUI_MASTER_CNTL, val);
    case RAGE128_SC_TOP_LEFT_C:        return r2d_reg_read(RAGE128_SC_TOP_LEFT, val);
    case RAGE128_SC_BOTTOM_RIGHT_C:    return r2d_reg_read(RAGE128_SC_BOTTOM_RIGHT, val);
    default:
      break;
  }
  return false;
}

bool bx_rage128_c::r3d_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
#define MERGE(field) ((field) = ((field) & ~mask) | (val & mask))
  if ((off >= RAGE128_PRIM_TEX_OFFSET_C(0)) && (off <= RAGE128_PRIM_TEX_OFFSET_C(10))) {
    MERGE(t3d.prim_tex_offset[(off - RAGE128_PRIM_TEX_OFFSET_C(0)) >> 2]);
    return true;
  }
  if ((off >= RAGE128_SEC_TEX_OFFSET_C(0)) && (off <= RAGE128_SEC_TEX_OFFSET_C(10))) {
    MERGE(t3d.sec_tex_offset[(off - RAGE128_SEC_TEX_OFFSET_C(0)) >> 2]);
    return true;
  }
  switch (off) {
    case RAGE128_SCALE_3D_CNTL:        MERGE(t3d.scale_3d_cntl); return true;
    case RAGE128_SCALE_3D_DATATYPE:    MERGE(t3d.scale_3d_datatype); return true;
    case RAGE128_COMPOSITE_SHADOW_ID:
      MERGE(t3d.composite_shadow_id);
      t3d.composite_shadow_id &= RAGE128_SHADOW_ID_WMASK;
      return true;
    case RAGE128_CLR_CMP_CLR_3D:       MERGE(t3d.clr_cmp_clr_3d); return true;
    case RAGE128_CLR_CMP_MSK_3D:       MERGE(t3d.clr_cmp_msk_3d); return true;
    case RAGE128_SETUP_CNTL:           MERGE(t3d.setup_cntl); return true;
    case RAGE128_WINDOW_XY_OFFSET:     MERGE(t3d.window_xy_offset); return true;
    case RAGE128_DRAW_LINE_POINT:      return true;
    case RAGE128_SETUP_CNTL_PM4:       MERGE(t3d.setup_cntl_pm4); return true;
    case RAGE128_Z_OFFSET_C:           MERGE(t3d.z_offset); return true;
    case RAGE128_Z_PITCH_C:            MERGE(t3d.z_pitch); return true;
    case RAGE128_Z_STEN_CNTL_C:        MERGE(t3d.z_sten_cntl); return true;
    case RAGE128_TEX_CNTL_C:           MERGE(t3d.tex_cntl); return true;
    case RAGE128_MISC_3D_STATE_CNTL:   MERGE(t3d.misc_3d_state_cntl); return true;
    case RAGE128_TEX_CLR_CMP_CLR_C:    MERGE(t3d.tex_clr_cmp_clr); return true;
    case RAGE128_TEX_CLR_CMP_MSK_C:    MERGE(t3d.tex_clr_cmp_msk); return true;
    case RAGE128_FOG_COLOR_C:          MERGE(t3d.fog_color); return true;
    case RAGE128_PRIM_TEX_CNTL_C:      MERGE(t3d.prim_tex_cntl); return true;
    case RAGE128_PRIM_TEX_COMBINE_CNTL_C: MERGE(t3d.prim_tex_combine_cntl); return true;
    case RAGE128_TEX_SIZE_PITCH_C:     MERGE(t3d.tex_size_pitch); return true;
    case RAGE128_SEC_TEX_CNTL_C:       MERGE(t3d.sec_tex_cntl); return true;
    case RAGE128_SEC_TEX_COMBINE_CNTL_C:
    case RAGE128_SECONDARY_TEXTURE_COMBINE_CNTL:
      MERGE(t3d.sec_tex_combine_cntl); return true;
    case RAGE128_CONSTANT_COLOR_C:     MERGE(t3d.constant_color); return true;
    case RAGE128_PRIM_TEX_BORDER_COLOR_C: MERGE(t3d.prim_tex_border_color); return true;
    case RAGE128_SEC_TEX_BORDER_COLOR_C:  MERGE(t3d.sec_tex_border_color); return true;
    case RAGE128_STEN_REF_MASK_C:
    case RAGE128_STEN_REF_MASK_LEGACY: MERGE(t3d.sten_ref_mask); return true;
    case RAGE128_PLANE_3D_MASK_C:      MERGE(t3d.plane_3d_mask); return true;
    case RAGE128_DST_PITCH_OFFSET_C:
      r2d_set_pitch_offset(true, val);
      return true;
    case RAGE128_TEX_PALETTE_INDEX:
      tex_pal_wr_index = val & 0xff;
      return true;
    case RAGE128_TEX_PALETTE_DATA:
      t3d.tex_palette[tex_pal_wr_index & 0xff] = 0xff000000u | (val & 0x00ffffff);
      tex_pal_wr_index++;
      return true;
    case RAGE128_FOG_TABLE_INDEX:
      fog_table_wr_index = val & 0xff;
      return true;
    case RAGE128_FOG_TABLE_DATA:
      t3d.fog_table[fog_table_wr_index & 0xff] = val & 0xff;
      fog_table_wr_index++;
      return true;
    case RAGE128_DP_GUI_MASTER_CNTL_C: return r2d_reg_write(RAGE128_DP_GUI_MASTER_CNTL, val, mask);
    case RAGE128_SC_TOP_LEFT_C:        return r2d_reg_write(RAGE128_SC_TOP_LEFT, val, mask);
    case RAGE128_SC_BOTTOM_RIGHT_C:    return r2d_reg_write(RAGE128_SC_BOTTOM_RIGHT, val, mask);
    default:
      break;
  }
#undef MERGE
  return false;
}

// ---------------------------------------------------------------------
// Vertex decode
// ---------------------------------------------------------------------

static float rage128_u32_to_float(Bit32u u)
{
  float f;
  memcpy(&f, &u, 4);
  return f;
}

static BX_CPP_INLINE float r3d_clamp01(float v)
{
  return (v > 0.0f) ? (v > 1.0f ? 1.0f : v) : 0.0f;
}

static void r3d_decode_vertex(Bit32u fmt, const Bit32u *d, r3d_vtx_t *v)
{
  Bit32u i = 0;

  v->x = rage128_u32_to_float(d[i++]);
  v->y = rage128_u32_to_float(d[i++]);
  v->z = rage128_u32_to_float(d[i++]);
  v->rhw = 1.0f;
  v->diffuse = 0xffffffff;
  v->spec = 0xff000000;
  v->s = v->t = v->s2 = v->t2 = 0.0f;
  if (fmt & RAGE128_VCF_RHW)
    v->rhw = rage128_u32_to_float(d[i++]);
  if (fmt & (RAGE128_VCF_DIFFUSE_BGR | RAGE128_VCF_DIFFUSE_A)) {
    float fb = 1.0f, fg = 1.0f, fr = 1.0f, fa = 1.0f;
    if (fmt & RAGE128_VCF_DIFFUSE_BGR) {
      fb = rage128_u32_to_float(d[i++]);
      fg = rage128_u32_to_float(d[i++]);
      fr = rage128_u32_to_float(d[i++]);
    }
    if (fmt & RAGE128_VCF_DIFFUSE_A)
      fa = rage128_u32_to_float(d[i++]);
    Bit32u cb = (Bit32u)(r3d_clamp01(fb) * 255.0f + 0.5f);
    Bit32u cg = (Bit32u)(r3d_clamp01(fg) * 255.0f + 0.5f);
    Bit32u cr = (Bit32u)(r3d_clamp01(fr) * 255.0f + 0.5f);
    Bit32u ca = (Bit32u)(r3d_clamp01(fa) * 255.0f + 0.5f);
    v->diffuse = (ca << 24) | (cr << 16) | (cg << 8) | cb;
  }
  if (fmt & RAGE128_VCF_DIFFUSE_ARGB)
    v->diffuse = d[i++];
  if (fmt & RAGE128_VCF_SPEC_BGR) {
    float sb = rage128_u32_to_float(d[i++]);
    float sg = rage128_u32_to_float(d[i++]);
    float sr = rage128_u32_to_float(d[i++]);
    Bit32u cb = (Bit32u)(r3d_clamp01(sb) * 255.0f + 0.5f);
    Bit32u cg = (Bit32u)(r3d_clamp01(sg) * 255.0f + 0.5f);
    Bit32u cr = (Bit32u)(r3d_clamp01(sr) * 255.0f + 0.5f);
    v->spec = (v->spec & 0xff000000u) | (cr << 16) | (cg << 8) | cb;
  }
  if (fmt & RAGE128_VCF_SPEC_F) {
    float ff = rage128_u32_to_float(d[i++]);
    v->spec = (v->spec & 0x00ffffffu) | ((Bit32u)(r3d_clamp01(ff) * 255.0f + 0.5f) << 24);
  }
  if (fmt & RAGE128_VCF_SPEC_FRGB)
    v->spec = d[i++];
  if (fmt & RAGE128_VCF_S_T) {
    v->s = rage128_u32_to_float(d[i++]);
    v->t = rage128_u32_to_float(d[i++]);
  }
  if (fmt & RAGE128_VCF_S2_T2) {
    v->s2 = rage128_u32_to_float(d[i++]);
    v->t2 = rage128_u32_to_float(d[i++]);
  }
  if (fmt & RAGE128_VCF_RHW2)
    v->rhw2 = rage128_u32_to_float(d[i++]);
  else
    v->rhw2 = v->rhw;
}

static Bit32u rage128_3d_vertex_dwords(Bit32u fmt)
{
  Bit32u n = 3;
  if (fmt & RAGE128_VCF_RHW) n += 1;
  if (fmt & RAGE128_VCF_DIFFUSE_BGR) n += 3;
  if (fmt & RAGE128_VCF_DIFFUSE_A) n += 1;
  if (fmt & RAGE128_VCF_DIFFUSE_ARGB) n += 1;
  if (fmt & RAGE128_VCF_SPEC_BGR) n += 3;
  if (fmt & RAGE128_VCF_SPEC_F) n += 1;
  if (fmt & RAGE128_VCF_SPEC_FRGB) n += 1;
  if (fmt & RAGE128_VCF_S_T) n += 2;
  if (fmt & RAGE128_VCF_S2_T2) n += 2;
  if (fmt & RAGE128_VCF_RHW2) n += 1;
  return n;
}

// ---------------------------------------------------------------------
// Pixel helpers
// ---------------------------------------------------------------------

void bx_rage128_c::r3d_mark_dirty(Bit32u addr, Bit32u len)
{
  vram_dirty(addr & vram_mask, len);
}

static BX_CPP_INLINE int r3d_cmp(Bit32u fn, Bit32u a, Bit32u b)
{
  switch (fn & 7) {
    case 0: return 0;
    case 1: return a < b;
    case 2: return a <= b;
    case 3: return a == b;
    case 4: return a >= b;
    case 5: return a > b;
    case 6: return a != b;
    default: return 1;
  }
}

static BX_CPP_INLINE Bit32u r3d_dst_read(const Bit8u *p, Bit32u dt)
{
  Bit32u v;
  switch (dt) {
    case 3:
      v = p[0] | (p[1] << 8);
      return ((v & 0x8000) ? 0xff000000 : 0) | ((v & 0x7c00) << 9) | ((v & 0x03e0) << 6) | ((v & 0x001f) << 3);
    case 4:
      v = p[0] | (p[1] << 8);
      return 0xff000000 | ((v & 0xf800) << 8) | ((v & 0x07e0) << 5) | ((v & 0x001f) << 3);
    case 15:
      v = p[0] | (p[1] << 8);
      return ((v & 0xf000) << 16) | ((v & 0x0f00) << 12) | ((v & 0x00f0) << 8) | ((v & 0x000f) << 4);
    default:
      return p[0] | (p[1] << 8) | (p[2] << 16) | ((Bit32u)p[3] << 24);
  }
}

static const Bit8u r3d_bayer4[4][4] = {
  {  0,  8,  2, 10 },
  { 12,  4, 14,  6 },
  {  3, 11,  1,  9 },
  { 15,  7, 13,  5 },
};

static BX_CPP_INLINE Bit32u r3d_dq(Bit32u v8, Bit32u add)
{
  v8 += add;
  return v8 > 255 ? 255 : v8;
}

static void r3d_dst_write(Bit8u *p, Bit32u dt, Bit32u argb, int bay, Bit32u pmask)
{
  Bit32u a = (argb >> 24) & 0xff;
  Bit32u r = (argb >> 16) & 0xff;
  Bit32u g = (argb >> 8) & 0xff;
  Bit32u b = argb & 0xff;
  Bit32u raw, m, cur;

  switch (dt) {
    case 3:
      if (bay >= 0) {
        r = r3d_dq(r, (Bit32u)bay >> 1);
        g = r3d_dq(g, (Bit32u)bay >> 1);
        b = r3d_dq(b, (Bit32u)bay >> 1);
      }
      raw = ((a << 8) & 0x8000) | ((r << 7) & 0x7c00) | ((g << 2) & 0x03e0) | (b >> 3);
      goto raw16;
    case 4:
      if (bay >= 0) {
        r = r3d_dq(r, (Bit32u)bay >> 1);
        g = r3d_dq(g, (Bit32u)bay >> 2);
        b = r3d_dq(b, (Bit32u)bay >> 1);
      }
      raw = ((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | (b >> 3);
      goto raw16;
    case 15:
      if (bay >= 0) {
        a = r3d_dq(a, (Bit32u)bay);
        r = r3d_dq(r, (Bit32u)bay);
        g = r3d_dq(g, (Bit32u)bay);
        b = r3d_dq(b, (Bit32u)bay);
      }
      raw = ((a << 8) & 0xf000) | ((r << 4) & 0x0f00) | (g & 0x00f0) | (b >> 4);
    raw16:
      m = pmask & 0xffff;
      if (m != 0xffff) {
        cur = p[0] | (p[1] << 8);
        raw = (raw & m) | (cur & ~m);
      }
      p[0] = (Bit8u)raw;
      p[1] = (Bit8u)(raw >> 8);
      break;
    default:
      if (pmask != 0xffffffff) {
        cur = p[0] | (p[1] << 8) | (p[2] << 16) | ((Bit32u)p[3] << 24);
        argb = (argb & pmask) | (cur & ~pmask);
      }
      p[0] = (Bit8u)argb;
      p[1] = (Bit8u)(argb >> 8);
      p[2] = (Bit8u)(argb >> 16);
      p[3] = (Bit8u)(argb >> 24);
      break;
  }
}

static BX_CPP_INLINE int r3d_dst_bpp(Bit32u dt)
{
  return (dt == 6) ? 4 : 2;
}

// ---------------------------------------------------------------------
// Texture sampling
// ---------------------------------------------------------------------

static BX_CPP_INLINE int r3d_tex_wrap(int c, int n, Bit32u mode)
{
  int m;
  switch (mode & 3) {
    case 0: return c & (n - 1);
    case 1:
      m = c & (2 * n - 1);
      return (m < n) ? m : (2 * n - 1 - m);
    case 2: return (c < 0) ? 0 : (c >= n) ? n - 1 : c;
    default: return ((c < 0) || (c >= n)) ? -1 : c;
  }
}

#define R3D_TSP_HALF(rs, st) ((st) ? ((rs)->t3d.tex_size_pitch >> 16) : ((rs)->t3d.tex_size_pitch & 0xffff))
#define R3D_TOPSLOT(tsp) ((int)(((tsp) >> 4) & 0xf) - (int)(((tsp) >> 12) & 0xf))
#define R3D_S3TC_CLASS(cntl) ((((cntl) >> 16) & 0xf) ? 0u : (((cntl) >> 26) & 3u))

static void r3d_level_dims(Bit32u tsp, int slot, Bit32u *lw, Bit32u *lh)
{
  int shift = R3D_TOPSLOT(tsp) - slot;
  Bit32u w = 1u << (tsp & 0xf);
  Bit32u h = 1u << ((tsp >> 8) & 0xf);
  if (shift < 0) shift = 0;
  if (shift > 31) shift = 31;
  w >>= shift; h >>= shift;
  *lw = w ? w : 1;
  *lh = h ? h : 1;
}

static Bit32u r3d_level_bytes(Bit32u dt, Bit32u s3tc, Bit32u lw, Bit32u lh)
{
  if (dt == 0)
    return ((lw + 3u) >> 2) * ((lh + 3u) >> 2) * ((s3tc >= 2u) ? 16u : 8u);
  if ((dt == 1) || (dt == 2) || (dt == 7) || (dt == 8) || (dt == 9))
    return lw * lh;
  return lw * lh * (((dt == 6) || (dt == 5) || (dt == 14)) ? 4u : 2u);
}

static BX_CPP_INLINE Bit32u r3d_level_pitch_b(Bit32u dt, Bit32u lw)
{
  if ((dt == 0) || (dt == 5))
    return 0;
  if ((dt == 1) || (dt == 2) || (dt == 7) || (dt == 8) || (dt == 9))
    return lw;
  return lw * (((dt == 6) || (dt == 14)) ? 4u : 2u);
}

static BX_CPP_INLINE Bit32u r3d_yuv_to_argb(Bit32u a, int y, int cb, int cr)
{
  int c = y - 16, d = cb - 128, e = cr - 128;
  int r = (298 * c + 409 * e + 128) >> 8;
  int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
  int b = (298 * c + 516 * d + 128) >> 8;
  if (r < 0) r = 0; else if (r > 255) r = 255;
  if (g < 0) g = 0; else if (g > 255) g = 255;
  if (b < 0) b = 0; else if (b > 255) b = 255;
  return (a << 24) | ((Bit32u)r << 16) | ((Bit32u)g << 8) | (Bit32u)b;
}

static BX_CPP_INLINE Bit16u r3d_rd16(const Bit8u *base, Bit32u off, Bit32u mask)
{
  return base[off & mask] | ((Bit16u)base[(off + 1) & mask] << 8);
}

static BX_CPP_INLINE Bit32u r3d_rd32(const Bit8u *base, Bit32u off, Bit32u mask)
{
  return base[off & mask] | ((Bit32u)base[(off + 1) & mask] << 8) |
         ((Bit32u)base[(off + 2) & mask] << 16) | ((Bit32u)base[(off + 3) & mask] << 24);
}

// Texel fetch from precomputed per-level constants; ARGB8888
static Bit32u r3d_texel(const Bit8u *texbase, Bit32u base, Bit32u mask, Bit32u lw, Bit32u tiled,
                        Bit32u dt, Bit32u s3tc, const Bit32u *pal, int u, int v)
{
  Bit32u off = (Bit32u)v * lw + u;
  Bit32u texel;

#define R3D_TOFF(xb, pb) (tiled ? r128_tile_off((xb), (Bit32u)v, (pb)) : (Bit32u)v * (pb) + (xb))

  switch (dt) {
    case 0: {
      Bit32u bpitch = (lw + 3u) >> 2;
      Bit32u boff = ((Bit32u)(v >> 2) * bpitch + (Bit32u)(u >> 2)) * ((s3tc >= 2u) ? 16u : 8u);
      Bit32u coff = boff + ((s3tc >= 2u) ? 8u : 0u);
      Bit32u texidx = ((Bit32u)(v & 3) << 2) | (Bit32u)(u & 3);
      Bit32u a = 0xff;
      Bit32u c0, c1, bits, sel, r0, g0, b0, r1, g1, b1;

      if (s3tc == 2u) {
        Bit32u an = texbase[(base + boff + (texidx >> 1)) & mask];
        a = (texidx & 1u) ? (an >> 4) : (an & 0xfu);
        a *= 0x11u;
      } else if (s3tc == 3u) {
        Bit32u a0 = texbase[(base + boff) & mask];
        Bit32u a1 = texbase[(base + boff + 1) & mask];
        Bit32u bp = texidx * 3u;
        Bit32u aw = r3d_rd32(texbase, base + boff + 2 + (bp >> 3), mask);
        Bit32u ac = (aw >> (bp & 7u)) & 7u;
        if (ac == 0) a = a0;
        else if (ac == 1) a = a1;
        else if (a0 > a1) a = ((8u - ac) * a0 + (ac - 1u) * a1) / 7u;
        else if (ac == 6) a = 0;
        else if (ac == 7) a = 255;
        else a = ((6u - ac) * a0 + (ac - 1u) * a1) / 5u;
      }
      c0 = r3d_rd16(texbase, base + coff, mask);
      c1 = r3d_rd16(texbase, base + coff + 2, mask);
      bits = r3d_rd32(texbase, base + coff + 4, mask);
      sel = (bits >> (texidx * 2u)) & 3;
      r0 = ((c0 >> 11) & 0x1f) * 255 / 31; g0 = ((c0 >> 5) & 0x3f) * 255 / 63; b0 = (c0 & 0x1f) * 255 / 31;
      r1 = ((c1 >> 11) & 0x1f) * 255 / 31; g1 = ((c1 >> 5) & 0x3f) * 255 / 63; b1 = (c1 & 0x1f) * 255 / 31;
      switch (sel) {
        case 0: return (a << 24) | (r0 << 16) | (g0 << 8) | b0;
        case 1: return (a << 24) | (r1 << 16) | (g1 << 8) | b1;
        case 2:
          if ((c0 > c1) || (s3tc >= 2u))
            return (a << 24) | (((2 * r0 + r1) / 3) << 16) | (((2 * g0 + g1) / 3) << 8) | ((2 * b0 + b1) / 3);
          return (a << 24) | (((r0 + r1) >> 1) << 16) | (((g0 + g1) >> 1) << 8) | ((b0 + b1) >> 1);
        default:
          if ((c0 > c1) || (s3tc >= 2u))
            return (a << 24) | (((r0 + 2 * r1) / 3) << 16) | (((g0 + 2 * g1) / 3) << 8) | ((b0 + 2 * b1) / 3);
          return 0;
      }
    }
    case 1:
      return pal[texbase[(base + R3D_TOFF((Bit32u)u, lw)) & mask] & 0xf];
    case 2:
      return pal[texbase[(base + R3D_TOFF((Bit32u)u, lw)) & mask]];
    case 3:
      texel = r3d_rd16(texbase, base + R3D_TOFF((Bit32u)u * 2u, lw * 2u), mask);
      return ((texel & 0x8000) ? 0xff000000 : 0) | ((texel & 0x7c00) << 9) | ((texel & 0x03e0) << 6) | ((texel & 0x001f) << 3);
    case 4:
      texel = r3d_rd16(texbase, base + R3D_TOFF((Bit32u)u * 2u, lw * 2u), mask);
      return 0xff000000 | ((texel & 0xf800) << 8) | ((texel & 0x07e0) << 5) | ((texel & 0x001f) << 3);
    case 15:
      texel = r3d_rd16(texbase, base + R3D_TOFF((Bit32u)u * 2u, lw * 2u), mask);
      return ((texel & 0xf000) << 16) | ((texel & 0x0f00) << 12) | ((texel & 0x00f0) << 8) | ((texel & 0x000f) << 4);
    case 6:
      return r3d_rd32(texbase, base + R3D_TOFF((Bit32u)u * 4u, lw * 4u), mask);
    case 5: {
      Bit32u o = base + off * 3;
      return 0xff000000 | ((Bit32u)texbase[(o + 2) & mask] << 16) | ((Bit32u)texbase[(o + 1) & mask] << 8) | texbase[o & mask];
    }
    case 7:
      texel = texbase[(base + R3D_TOFF((Bit32u)u, lw)) & mask];
      return 0xff000000 | ((texel & 0xe0) << 16) | ((texel & 0x1c) << 11) | ((texel & 0x03) << 6);
    case 8:
      texel = texbase[(base + R3D_TOFF((Bit32u)u, lw)) & mask];
      return 0xff000000 | (texel << 16) | (texel << 8) | texel;
    case 9:
      texel = texbase[(base + R3D_TOFF((Bit32u)u, lw)) & mask];
      return (texel << 24) | (texel << 16) | (texel << 8) | texel;
    case 11:
    case 12: {
      Bit32u poff = (base + R3D_TOFF((Bit32u)(u & ~1) * 2u, lw * 2u)) & mask;
      int yb, cb, cr;
      if (dt == 11) {
        yb = texbase[(poff + ((Bit32u)(u & 1) * 2u)) & mask];
        cb = texbase[(poff + 1u) & mask];
        cr = texbase[(poff + 3u) & mask];
      } else {
        yb = texbase[(poff + 1u + ((Bit32u)(u & 1) * 2u)) & mask];
        cb = texbase[poff];
        cr = texbase[(poff + 2u) & mask];
      }
      return r3d_yuv_to_argb(0xffu, yb, cb, cr);
    }
    case 14:
      texel = r3d_rd32(texbase, base + R3D_TOFF((Bit32u)u * 4u, lw * 4u), mask);
      return r3d_yuv_to_argb(texel >> 24, (int)((texel >> 16) & 0xff), (int)((texel >> 8) & 0xff), (int)(texel & 0xff));
    default:
      return 0xffffffff;
  }
#undef R3D_TOFF
}

static BX_CPP_INLINE Bit32u r3d_texel_ci_raw(const Bit8u *texbase, Bit32u base, Bit32u mask, Bit32u lw,
                                             Bit32u tiled, Bit32u dt, int u, int v)
{
  Bit32u off = tiled ? r128_tile_off((Bit32u)u, (Bit32u)v, lw) : (Bit32u)v * lw + (Bit32u)u;
  Bit32u idx = texbase[(base + off) & mask];
  return (dt == 1) ? (idx & 0xfu) : idx;
}

// Chroma key registers hold raw texel values: permute through the texel conversion
static Bit32u r3d_ck_to_argb(Bit32u dt, Bit32u v)
{
  switch (dt) {
    case 3: return ((v & 0x8000) ? 0xff000000 : 0) | ((v & 0x7c00) << 9) | ((v & 0x03e0) << 6) | ((v & 0x001f) << 3);
    case 4: return ((v & 0xf800) << 8) | ((v & 0x07e0) << 5) | ((v & 0x001f) << 3);
    case 15: return ((v & 0xf000) << 16) | ((v & 0x0f00) << 12) | ((v & 0x00f0) << 8) | ((v & 0x000f) << 4);
    case 5: return 0xff000000 | (v & 0x00ffffff);
    case 7: return 0xff000000 | ((v & 0xe0) << 16) | ((v & 0x1c) << 11) | ((v & 0x03) << 6);
    case 8: return 0xff000000 | ((v & 0xff) * 0x00010101u);
    case 9: return (v & 0xff) * 0x01010101u;
    case 11:
    case 12: return r3d_yuv_to_argb(0xffu, (int)(v & 0xff), (int)((v >> 8) & 0xff), (int)((v >> 24) & 0xff));
    case 14: return r3d_yuv_to_argb(v >> 24, (int)((v >> 16) & 0xff), (int)((v >> 8) & 0xff), (int)(v & 0xff));
    default: return v;
  }
}

static BX_CPP_INLINE float r3d_texcoord_fx(float f)
{
  return (float)floor(f * 4096.0f + 0.5f) * (1.0f / 4096.0f);
}

static void r3d_stage_desc_init(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d)
{
  const r3d_stage_hdr_t *h = &rs->d.sh[st];
  d->tsp = h->tsp;
  d->clamp_s = h->clamp_s;
  d->clamp_t = h->clamp_t;
  d->dt = h->dt;
  d->s3tc = h->s3tc;
  d->border = h->border;
  d->minb = h->minb;
  d->mag = h->mag;
  d->mipdis = h->mipdis;
  d->top = h->top;
  d->pal = rs->t3d.tex_palette;
  d->slot_valid = 0;
  d->slot_tiled = 0;
}

// Resolve one mip slot's level constants into the descriptor
void bx_rage128_c::r3d_stage_slot(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d, int cslot)
{
  if (d->slot_valid & (1u << cslot))
    return;
  Bit32u so = st ? rs->sec_stage_off[cslot] : rs->prim_stage_off[cslot];
  Bit32u raw = st ? rs->t3d.sec_tex_offset[cslot] : rs->t3d.prim_tex_offset[cslot];

  d->slot_valid |= (Bit16u)(1u << cslot);
  r3d_level_dims(d->tsp, cslot, &d->slot[cslot].lw, &d->slot[cslot].lh);
  d->slot_tiled &= (Bit16u)~(1u << cslot);
  if (raw >> 30) {
    Bit32u pb = r3d_level_pitch_b(d->dt, d->slot[cslot].lw);
    if (r128_tiled_ok(1u, pb) && (so == R128_TEX_STAGE_NONE))
      d->slot_tiled |= (Bit16u)(1u << cslot);
  }
  if (so != R128_TEX_STAGE_NONE) {
    Bit32u slice = r3d_level_bytes(d->dt, d->s3tc, d->slot[cslot].lw, d->slot[cslot].lh);
    d->slot[cslot].texbase = tex_stage.arena + so;
    d->slot[cslot].base = 0;
    d->slot[cslot].mask = slice ? (slice - 1u) : 0u;
  } else {
    d->slot[cslot].texbase = BX_RAGE128_THIS s.memory;
    d->slot[cslot].base = raw & 0x3fffffff;
    d->slot[cslot].mask = vram_mask;
  }
}

static BX_CPP_INLINE Bit32u r3d_lerp_packed(Bit32u x, Bit32u y, Bit32u w)
{
  Bit32u iw = 256u - w;
  Bit32u rb = ((x & 0x00ff00ffu) * iw + (y & 0x00ff00ffu) * w + 0x00800080u) >> 8;
  Bit32u ag = (((x >> 8) & 0x00ff00ffu) * iw + ((y >> 8) & 0x00ff00ffu) * w + 0x00800080u) >> 8;
  return ((ag & 0x00ff00ffu) << 8) | (rb & 0x00ff00ffu);
}

Bit32u bx_rage128_c::r3d_tex_level(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d, int slot,
                                   float s, float t, int linear, Bit32u *nearest)
{
  int cslot = slot < 0 ? 0 : (slot > 10 ? 10 : slot);
  Bit32u border = d->border;
  int u, v;
  Bit32u nearv;

  r3d_stage_slot(rs, st, d, cslot);
  const Bit8u *texbase = d->slot[cslot].texbase;
  Bit32u basesel = d->slot[cslot].base;
  Bit32u masksel = d->slot[cslot].mask;
  Bit32u tilesel = (d->slot_tiled >> cslot) & 1u;
  Bit32u lw = d->slot[cslot].lw, lh = d->slot[cslot].lh;

  if (!linear || nearest) {
    u = r3d_tex_wrap((int)floor(r3d_texcoord_fx(s * (float)lw)), lw, d->clamp_s);
    v = r3d_tex_wrap((int)floor(r3d_texcoord_fx(t * (float)lh)), lh, d->clamp_t);
    nearv = ((u < 0) || (v < 0)) ? border : r3d_texel(texbase, basesel, masksel, lw, tilesel, d->dt, d->s3tc, d->pal, u, v);
    if (nearest)
      *nearest = (((d->dt == 1) || (d->dt == 2)) && (u >= 0) && (v >= 0))
        ? r3d_texel_ci_raw(texbase, basesel, masksel, lw, tilesel, d->dt, u, v) : nearv;
    if (!linear)
      return nearv;
  }
  {
    float fu = r3d_texcoord_fx(s * (float)lw) - 0.5f;
    float fv = r3d_texcoord_fx(t * (float)lh) - 0.5f;
    int u0 = (int)floor(fu);
    int v0 = (int)floor(fv);
    Bit32u wu = (Bit32u)((fu - (float)u0) * 256.0f + 0.5f);
    Bit32u wv = (Bit32u)((fv - (float)v0) * 256.0f + 0.5f);
    Bit32u c[4];
    int uu0 = r3d_tex_wrap(u0, lw, d->clamp_s), uu1 = r3d_tex_wrap(u0 + 1, lw, d->clamp_s);
    int vv0 = r3d_tex_wrap(v0, lh, d->clamp_t), vv1 = r3d_tex_wrap(v0 + 1, lh, d->clamp_t);

    c[0] = ((uu0 < 0) || (vv0 < 0)) ? border : r3d_texel(texbase, basesel, masksel, lw, tilesel, d->dt, d->s3tc, d->pal, uu0, vv0);
    c[1] = ((uu1 < 0) || (vv0 < 0)) ? border : r3d_texel(texbase, basesel, masksel, lw, tilesel, d->dt, d->s3tc, d->pal, uu1, vv0);
    c[2] = ((uu0 < 0) || (vv1 < 0)) ? border : r3d_texel(texbase, basesel, masksel, lw, tilesel, d->dt, d->s3tc, d->pal, uu0, vv1);
    c[3] = ((uu1 < 0) || (vv1 < 0)) ? border : r3d_texel(texbase, basesel, masksel, lw, tilesel, d->dt, d->s3tc, d->pal, uu1, vv1);
    return r3d_lerp_packed(r3d_lerp_packed(c[0], c[1], wu), r3d_lerp_packed(c[2], c[3], wu), wv);
  }
}

Bit32u bx_rage128_c::r3d_tex_sample(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d,
                                    float s, float t, float lod, int has_lod, Bit32u *nearest)
{
  Bit32u minb = d->minb;
  Bit32u mag = d->mag;
  int mipdis = d->mipdis;
  int top = d->top;

  if (!has_lod || mipdis || (minb < 2)) {
    int linear = has_lod ? ((lod > 0.0f) ? (int)(minb & 1) : (mag == 1)) : !((minb == 0) && (mag == 0));
    return r3d_tex_level(rs, st, d, top, s, t, linear, nearest);
  }
  if (lod <= 0.0f)
    return r3d_tex_level(rs, st, d, top, s, t, mag == 1, nearest);
  {
    float lvl = lod > (float)top ? (float)top : lod;
    int texel_linear = (minb == 3) || (minb == 5);
    int mip_linear = (minb == 4) || (minb == 5);
    if (mip_linear) {
      int l0 = (int)floor(lvl);
      float f = lvl - (float)l0;
      int slotA = top - l0;
      int slotB = top - (l0 + 1);
      if (slotB < 0) slotB = 0;
      Bit32u ca = r3d_tex_level(rs, st, d, slotA, s, t, texel_linear, nearest);
      Bit32u cb = r3d_tex_level(rs, st, d, slotB, s, t, texel_linear, NULL);
      return r3d_lerp_packed(ca, cb, (Bit32u)(f * 256.0f + 0.5f));
    }
    return r3d_tex_level(rs, st, d, top - (int)(lvl + 0.5f), s, t, texel_linear, nearest);
  }
}

static BX_CPP_INLINE float r3d_log2f_fast(float x)
{
  union { float f; Bit32u u; } v;
  v.f = x;
  float e = (float)(int)((v.u >> 23) & 0xff) - 127.0f;
  v.u = (v.u & 0x007fffffu) | 0x3f800000u;
  float m = v.f;
  return e + (-2.133847707f + m * (3.010783972f + m * (-1.029521946f + m * 0.153918478f)));
}

// Blend factor code book; RGBA per channel
static void r3d_blend_factor(Bit32u code, const float sc[4], const float dc[4], float out[4])
{
  float f;
  switch (code & 0xf) {
    case 0x0: out[0] = out[1] = out[2] = out[3] = 0.0f; return;
    case 0x1: out[0] = out[1] = out[2] = out[3] = 1.0f; return;
    case 0x2: out[0] = sc[0]; out[1] = sc[1]; out[2] = sc[2]; out[3] = sc[3]; return;
    case 0x3: out[0] = 1.0f - sc[0]; out[1] = 1.0f - sc[1]; out[2] = 1.0f - sc[2]; out[3] = 1.0f - sc[3]; return;
    case 0x4: out[0] = out[1] = out[2] = out[3] = sc[3]; return;
    case 0x5: out[0] = out[1] = out[2] = out[3] = 1.0f - sc[3]; return;
    case 0x6: out[0] = out[1] = out[2] = out[3] = dc[3]; return;
    case 0x7: out[0] = out[1] = out[2] = out[3] = 1.0f - dc[3]; return;
    case 0x8: out[0] = dc[0]; out[1] = dc[1]; out[2] = dc[2]; out[3] = dc[3]; return;
    case 0x9: out[0] = 1.0f - dc[0]; out[1] = 1.0f - dc[1]; out[2] = 1.0f - dc[2]; out[3] = 1.0f - dc[3]; return;
    case 0xa:
      f = sc[3] < 1.0f - dc[3] ? sc[3] : 1.0f - dc[3];
      out[0] = out[1] = out[2] = f;
      out[3] = 1.0f;
      return;
    default: out[0] = out[1] = out[2] = out[3] = 1.0f; return;
  }
}

// Float window coordinate -> fixed point on the subpixel grid
static BX_CPP_INLINE Bit32s r3d_snap(float v, float subf, int rnd, Bit32s lim)
{
  float s = v * subf;
  if (!(s >= (float)-lim))
    return -lim;
  if (s >= (float)lim)
    return lim - 1;
  return rnd ? (Bit32s)floor(s + 0.5f) : (Bit32s)floor(s);
}

// Apply one stage's texture combine to the running pixel colour
static void r3d_tex_combine(const r3d_comb_desc_t *cd, float col[4], Bit32u tx, const float int_color[4], const float cc[4])
{
  Bit32u comb = cd->comb, fmsb = cd->fmsb, cfac = cd->cfac, ifac = cd->ifac;
  Bit32u comba = cd->comba, afac = cd->afac, ifaca = cd->ifaca;
  float tr = ((tx >> 16) & 0xff) / 255.0f;
  float tg = ((tx >> 8) & 0xff) / 255.0f;
  float tb = (tx & 0xff) / 255.0f;
  float ta = (tx >> 24) / 255.0f;
  float prev[4], fc[3], ci[3], fa, ia;
  int i;

  prev[0] = col[0]; prev[1] = col[1]; prev[2] = col[2]; prev[3] = col[3];

  switch (cfac) {
    case 0: fc[0] = cc[0]; fc[1] = cc[1]; fc[2] = cc[2]; break;
    case 1: fc[0] = 1.0f - cc[0]; fc[1] = 1.0f - cc[1]; fc[2] = 1.0f - cc[2]; break;
    case 5: fc[0] = 1.0f - tr; fc[1] = 1.0f - tg; fc[2] = 1.0f - tb; break;
    case 6: fc[0] = fc[1] = fc[2] = ta; break;
    case 7: fc[0] = fc[1] = fc[2] = 1.0f - ta; break;
    case 8: fc[0] = prev[0]; fc[1] = prev[1]; fc[2] = prev[2]; break;
    default: fc[0] = tr; fc[1] = tg; fc[2] = tb; break;
  }
  switch (ifac) {
    case 2: ci[0] = cc[0]; ci[1] = cc[1]; ci[2] = cc[2]; break;
    case 3: ci[0] = ci[1] = ci[2] = cc[3]; break;
    case 5: ci[0] = ci[1] = ci[2] = int_color[3]; break;
    case 8: ci[0] = prev[0]; ci[1] = prev[1]; ci[2] = prev[2]; break;
    case 9: ci[0] = ci[1] = ci[2] = prev[3]; break;
    default: ci[0] = int_color[0]; ci[1] = int_color[1]; ci[2] = int_color[2]; break;
  }

  switch (comb) {
    case 0:
      if (fmsb) {
        for (i = 0; i < 3; i++) { col[i] = fc[i] - ci[i]; if (col[i] < 0.0f) col[i] = 0.0f; }
        break;
      }
      // fall through
    case 1:
      col[0] = fc[0]; col[1] = fc[1]; col[2] = fc[2];
      break;
    case 2:
      col[0] = ci[0]; col[1] = ci[1]; col[2] = ci[2];
      break;
    case 4:
      if (fmsb) {
        col[0] = ci[0] * (1.0f - tr) + fc[0] * tr;
        col[1] = ci[1] * (1.0f - tg) + fc[1] * tg;
        col[2] = ci[2] * (1.0f - tb) + fc[2] * tb;
      } else {
        for (i = 0; i < 3; i++) { col[i] = ci[i] * fc[i] * 2.0f; if (col[i] > 1.0f) col[i] = 1.0f; }
      }
      break;
    case 5:
      if (fmsb) {
        float t[3] = { tr, tg, tb };
        for (i = 0; i < 3; i++) { col[i] = fc[i] + ci[i] * (1.0f - t[i]); if (col[i] > 1.0f) col[i] = 1.0f; }
      } else {
        for (i = 0; i < 3; i++) { col[i] = ci[i] * fc[i] * 4.0f; if (col[i] > 1.0f) col[i] = 1.0f; }
      }
      break;
    case 6:
      if (fmsb) {
        float t[3] = { tr, tg, tb };
        for (i = 0; i < 3; i++) { col[i] = fc[i] + ci[i] * t[i]; if (col[i] > 1.0f) col[i] = 1.0f; }
      } else {
        for (i = 0; i < 3; i++) { col[i] = ci[i] + fc[i]; if (col[i] > 1.0f) col[i] = 1.0f; }
      }
      break;
    case 7:
      for (i = 0; i < 3; i++) { col[i] = ci[i] + fc[i] - 0.5f; col[i] = col[i] < 0.0f ? 0.0f : (col[i] > 1.0f ? 1.0f : col[i]); }
      break;
    case 8: {
      float f = int_color[3];
      for (i = 0; i < 3; i++) col[i] = ci[i] * (1.0f - f) + fc[i] * f;
      break;
    }
    case 9:
      for (i = 0; i < 3; i++) col[i] = ci[i] * (1.0f - ta) + fc[i] * ta;
      break;
    case 10:
      for (i = 0; i < 3; i++) col[i] = ci[i] * (1.0f - cc[3]) + fc[i] * cc[3];
      break;
    case 11:
      for (i = 0; i < 3; i++) { col[i] = fc[i] + ci[i] * (1.0f - ta); if (col[i] > 1.0f) col[i] = 1.0f; }
      break;
    case 12:
      for (i = 0; i < 3; i++) col[i] = ci[i] * (1.0f - prev[3]) + fc[i] * prev[3];
      break;
    case 13:
      for (i = 0; i < 3; i++) { col[i] = fc[i] + ci[i] * ta; if (col[i] > 1.0f) col[i] = 1.0f; }
      break;
    case 14:
      for (i = 0; i < 3; i++) { col[i] = (ci[i] + fc[i] - 0.5f) * 2.0f; col[i] = col[i] < 0.0f ? 0.0f : (col[i] > 1.0f ? 1.0f : col[i]); }
      break;
    case 15:
      for (i = 0; i < 3; i++) col[i] = ci[i] * (1.0f - cc[i]) + fc[i] * cc[i];
      break;
    default:
    case 3:
      col[0] = ci[0] * fc[0]; col[1] = ci[1] * fc[1]; col[2] = ci[2] * fc[2];
      break;
  }

  fa = (afac == 7) ? (1.0f - ta) : ta;
  switch (ifaca) {
    case 1: ia = cc[3]; break;
    case 2: ia = int_color[3]; break;
    default: ia = prev[3]; break;
  }
  switch (comba) {
    case 0:
    case 1: col[3] = fa; break;
    case 2: col[3] = ia; break;
    case 4: col[3] = ia * fa * 2.0f; if (col[3] > 1.0f) col[3] = 1.0f; break;
    case 6: col[3] = ia + fa; if (col[3] > 1.0f) col[3] = 1.0f; break;
    case 7: col[3] = ia + fa - 0.5f; col[3] = col[3] < 0.0f ? 0.0f : (col[3] > 1.0f ? 1.0f : col[3]); break;
    case 5: col[3] = ia * fa * 4.0f; if (col[3] > 1.0f) col[3] = 1.0f; break;
    case 14: col[3] = (ia + fa - 0.5f) * 2.0f; col[3] = col[3] < 0.0f ? 0.0f : (col[3] > 1.0f ? 1.0f : col[3]); break;
    default:
    case 3: col[3] = ia * fa; break;
  }
}

// Per-pixel texture stage block: both stages, chroma keys, combine.
// Returns 0 when a chroma key discards the pixel.
int bx_rage128_c::r3d_texstage_run(r3d_texctx_t *tc, float w0, float w1, float w2, float *col)
{
  const rage128_raster_state_t *rs = tc->rs;
  const rage128_draw_state_t *ds = &rs->d;
  float int_color[4];
  float ir = 1.0f;
  float rhw = 0.0f;

  int_color[0] = col[0]; int_color[1] = col[1]; int_color[2] = col[2]; int_color[3] = col[3];

  if (ds->do_persp) {
    rhw = w0 * tc->arhw + w1 * tc->brhw + w2 * tc->crhw;
    if (rhw != 0.0f)
      ir = 1.0f / rhw;
  }
  if (ds->tex_en) {
    float sp = w0 * tc->sta + w1 * tc->stb + w2 * tc->stc;
    float tp = w0 * tc->tta + w1 * tc->ttb + w2 * tc->ttc;
    float s = sp * ir, t = tp * ir;
    float lod = 0.0f;
    Bit32u tx, tnear = 0;

    if (ds->need_lod) {
      float dsx, dsy, dtx, dty, ax2, ay2, rho2;
      if (ds->do_persp) {
        float wp = rhw;
        float iw2 = (wp != 0.0f) ? 1.0f / (wp * wp) : 0.0f;
        dsx = (tc->dSdx * wp - sp * tc->dWdx) * iw2;
        dsy = (tc->dSdy * wp - sp * tc->dWdy) * iw2;
        dtx = (tc->dTdx * wp - tp * tc->dWdx) * iw2;
        dty = (tc->dTdy * wp - tp * tc->dWdy) * iw2;
      } else {
        dsx = tc->dSdx; dsy = tc->dSdy; dtx = tc->dTdx; dty = tc->dTdy;
      }
      dsx *= ds->texw0; dsy *= ds->texw0;
      dtx *= ds->texh0; dty *= ds->texh0;
      ax2 = dsx * dsx + dtx * dtx;
      ay2 = dsy * dsy + dty * dty;
      rho2 = ax2 > ay2 ? ax2 : ay2;
      lod = rho2 > 0.0f ? 0.5f * r3d_log2f_fast(rho2) + ds->lod_bias : -1000.0f;
    }
    tx = r3d_tex_sample(rs, 0, &tc->sd0, s, t, lod, ds->need_lod, ds->need_ck ? &tnear : NULL);
    if (ds->ck3d_on) {
      int eq = (tnear & ds->ck3d_msk) == (ds->ck3d_clr & ds->ck3d_msk);
      if ((ds->ckfn == 3) ? eq : !eq)
        return 0;
    }
    if (ds->ckc_on) {
      if ((tnear & ds->ckc_msk) == (ds->ckc_clr & ds->ckc_msk))
        return 0;
    }
    r3d_tex_combine(&ds->comb[0], col, tx, int_color, ds->cc);
  }
  if (ds->sec_en) {
    int sel = ds->sec_sel;
    float sp2 = sel ? (w0 * tc->s2a + w1 * tc->s2b + w2 * tc->s2c) : (w0 * tc->sta + w1 * tc->stb + w2 * tc->stc);
    float tp2 = sel ? (w0 * tc->t2a + w1 * tc->t2b + w2 * tc->t2c) : (w0 * tc->tta + w1 * tc->ttb + w2 * tc->ttc);
    float s = sp2 * ir, t = tp2 * ir;
    float lod2 = 0.0f;
    Bit32u tx;

    if (ds->need_lod2) {
      float gsx = sel ? tc->dS2dx : tc->dSdx, gsy = sel ? tc->dS2dy : tc->dSdy;
      float gtx = sel ? tc->dT2dx : tc->dTdx, gty = sel ? tc->dT2dy : tc->dTdy;
      float dsx, dsy, dtx, dty, ax2, ay2, rho2;
      if (ds->do_persp) {
        float wp = rhw;
        float iw2 = (wp != 0.0f) ? 1.0f / (wp * wp) : 0.0f;
        dsx = (gsx * wp - sp2 * tc->dWdx) * iw2;
        dsy = (gsy * wp - sp2 * tc->dWdy) * iw2;
        dtx = (gtx * wp - tp2 * tc->dWdx) * iw2;
        dty = (gty * wp - tp2 * tc->dWdy) * iw2;
      } else {
        dsx = gsx; dsy = gsy; dtx = gtx; dty = gty;
      }
      dsx *= ds->texw1; dsy *= ds->texw1;
      dtx *= ds->texh1; dty *= ds->texh1;
      ax2 = dsx * dsx + dtx * dtx;
      ay2 = dsy * dsy + dty * dty;
      rho2 = ax2 > ay2 ? ax2 : ay2;
      lod2 = rho2 > 0.0f ? 0.5f * r3d_log2f_fast(rho2) + ds->lod_bias : -1000.0f;
    }
    tx = r3d_tex_sample(rs, 1, &tc->sd1, s, t, lod2, ds->need_lod2, NULL);
    r3d_tex_combine(&ds->comb[1], col, tx, int_color, ds->cc);
  }
  return 1;
}

// ---------------------------------------------------------------------
// Triangle rasterizer: snapped integer edge functions with a top-left
// fill rule, evaluated at pixel centres. thr_mask selects the scanlines
// this call owns (0 = every row).
// ---------------------------------------------------------------------

static BX_CPP_INLINE bool r128_row_owned(Bit32s y, int thr_id, int thr_mask)
{
  if (thr_mask > 0)
    return (y & thr_mask) == thr_id;
  if (thr_mask == 0)
    return true;
  return ((Bit32u)y % (Bit32u)-thr_mask) == (Bit32u)thr_id;
}

void bx_rage128_c::r3d_tri(const rage128_raster_state_t *rs, int thr_id, int thr_mask,
                           const r3d_vtx_t *a, const r3d_vtx_t *b, const r3d_vtx_t *c)
{
  const rage128_draw_state_t *ds = &rs->d;
  Bit32u dst_dt = ds->dst_dt;
  int bpp = ds->bpp;
  Bit32u wmask = ds->wmask;
  int aux_on = ds->aux_on;
  Bit32u zfn = ds->zfn;
  int zbpp = ds->zbpp;
  Bit32u zmax = ds->zmax;
  int zshift = ds->zshift;
  int sten_on = ds->sten_on;
  Bit32u sfn = ds->sfn, sfail_op = ds->sfail_op, zpass_op = ds->zpass_op, zfail_op = ds->zfail_op;
  Bit32u sref = ds->sref, svmask = ds->svmask, swmask = ds->swmask;
  int sshift = ds->sshift;
  Bit32u zrowpx = ds->zrowpx;
  int z_en = ds->z_en;
  int tex_en = ds->tex_en;
  int sec_en = ds->sec_en;
  int alpha_en = ds->alpha_en;
  int c_tld = ds->c_tiled;
  int z_tld = ds->z_tiled;
  Bit32u cpb = rs->dst_pitch * (Bit32u)ds->bpp;
  Bit32u zpb = zrowpx * (Bit32u)ds->zbpp;
  int sub = ds->sub;
  float subf = ds->subf;
  int rnd = ds->rnd;
  Bit32s slim = ds->slim;
  Bit32s axi, ayi, bxi, byi, cxi, cyi;
  Bit64s area2i;
  float ax, ay, bx, by, cx, cy;
  float area2, inv, invs;
  int x0, y0, x1, y1;
  Bit8u *vram = BX_RAGE128_THIS s.memory;

  if (!ds->draw_ok)
    return;

  axi = r3d_snap(a->x, subf, rnd, slim) + ds->woxi;
  ayi = r3d_snap(a->y, subf, rnd, slim) + ds->woyi;
  bxi = r3d_snap(b->x, subf, rnd, slim) + ds->woxi;
  byi = r3d_snap(b->y, subf, rnd, slim) + ds->woyi;
  cxi = r3d_snap(c->x, subf, rnd, slim) + ds->woxi;
  cyi = r3d_snap(c->y, subf, rnd, slim) + ds->woyi;

  area2i = (Bit64s)(cxi - axi) * (byi - ayi) - (Bit64s)(cyi - ayi) * (bxi - axi);
  if (area2i == 0)
    return;
  ax = (float)axi / subf; ay = (float)ayi / subf;
  bx = (float)bxi / subf; by = (float)byi / subf;
  cx = (float)cxi / subf; cy = (float)cyi / subf;
  area2 = (float)area2i / (subf * subf);
  inv = 1.0f / area2;
  invs = 1.0f / (float)area2i;

  x0 = (int)floor(ax < bx ? (ax < cx ? ax : cx) : (bx < cx ? bx : cx));
  y0 = (int)floor(ay < by ? (ay < cy ? ay : cy) : (by < cy ? by : cy));
  x1 = (int)ceil(ax > bx ? (ax > cx ? ax : cx) : (bx > cx ? bx : cx));
  y1 = (int)ceil(ay > by ? (ay > cy ? ay : cy) : (by > cy ? by : cy));
  if (x0 < ds->sx0) x0 = ds->sx0;
  if (y0 < ds->sy0) y0 = ds->sy0;
  if (x1 > ds->sx1) x1 = ds->sx1;
  if (y1 > ds->sy1) y1 = ds->sy1;
  if ((x0 > x1) || (y0 > y1))
    return;

  float e0dx = cy - by, e0dy = bx - cx;
  float e1dx = ay - cy, e1dy = cx - ax;
  float e2dx = by - ay, e2dy = ax - bx;
  Bit64s px0s = (Bit64s)x0 * sub + (sub >> 1);
  Bit64s py0s = (Bit64s)y0 * sub + (sub >> 1);
  Bit64s e0dxi = (Bit64s)sub * (cyi - byi), e0dyi = (Bit64s)sub * (bxi - cxi);
  Bit64s e1dxi = (Bit64s)sub * (ayi - cyi), e1dyi = (Bit64s)sub * (cxi - axi);
  Bit64s e2dxi = (Bit64s)sub * (byi - ayi), e2dyi = (Bit64s)sub * (axi - bxi);
  Bit64s e0ri = (px0s - bxi) * (cyi - byi) - (py0s - byi) * (cxi - bxi);
  Bit64s e1ri = (px0s - cxi) * (ayi - cyi) - (py0s - cyi) * (axi - cxi);
  Bit64s e2ri = (px0s - axi) * (byi - ayi) - (py0s - ayi) * (bxi - axi);
  double invd = 1.0 / (double)area2i;
  double azd = (double)a->z, bzd = (double)b->z, czd = (double)c->z;
  double zr_d = ((double)e0ri * azd + (double)e1ri * bzd + (double)e2ri * czd) * invd;
#define R3D_TL(dx, dy) (area2i > 0 ? ((dy) > 0 || ((dy) == 0 && (dx) < 0)) : ((dy) < 0 || ((dy) == 0 && (dx) > 0)))
  if (!R3D_TL(cxi - bxi, cyi - byi)) e0ri += (area2i > 0) ? -1 : 1;
  if (!R3D_TL(axi - cxi, ayi - cyi)) e1ri += (area2i > 0) ? -1 : 1;
  if (!R3D_TL(bxi - axi, byi - ayi)) e2ri += (area2i > 0) ? -1 : 1;
#undef R3D_TL
  float vca[4], vcb[4], vcc[4];
  float spa[3], spb[3], spc[3];
  float fga, fgb, fgc;
  float fogr = ds->fogr, fogg = ds->fogg, fogb = ds->fogb;
  r3d_texctx_t tctx;
  int premult = ds->premult;
  int dither = ds->dither;

  {
    const r3d_vtx_t *vv[3] = { a, b, c };
    float *cols[3] = { vca, vcb, vcc };
    float *sps[3] = { spa, spb, spc };
    float *fgs[3] = { &fga, &fgb, &fgc };
    for (int i = 0; i < 3; i++) {
      Bit32u d = vv[i]->diffuse;
      Bit32u sv = vv[i]->spec;
      cols[i][0] = ((d >> 16) & 0xff) / 255.0f;
      cols[i][1] = ((d >> 8) & 0xff) / 255.0f;
      cols[i][2] = (d & 0xff) / 255.0f;
      cols[i][3] = (d >> 24) / 255.0f;
      sps[i][0] = ((sv >> 16) & 0xff) / 255.0f;
      sps[i][1] = ((sv >> 8) & 0xff) / 255.0f;
      sps[i][2] = (sv & 0xff) / 255.0f;
      *fgs[i] = (sv >> 24) / 255.0f;
    }
    if (ds->flat_on) {
      float fl[4];
      memcpy(fl, cols[ds->flat_src], sizeof(fl));
      memcpy(vca, fl, sizeof(fl));
      memcpy(vcb, fl, sizeof(fl));
      memcpy(vcc, fl, sizeof(fl));
    }
  }
  tctx.rs = rs;
  tctx.sta = premult ? a->s * a->rhw : a->s;
  tctx.stb = premult ? b->s * b->rhw : b->s;
  tctx.stc = premult ? c->s * c->rhw : c->s;
  tctx.tta = premult ? a->t * a->rhw : a->t;
  tctx.ttb = premult ? b->t * b->rhw : b->t;
  tctx.ttc = premult ? c->t * c->rhw : c->t;
  tctx.s2a = premult ? a->s2 * a->rhw : a->s2;
  tctx.s2b = premult ? b->s2 * b->rhw : b->s2;
  tctx.s2c = premult ? c->s2 * c->rhw : c->s2;
  tctx.t2a = premult ? a->t2 * a->rhw : a->t2;
  tctx.t2b = premult ? b->t2 * b->rhw : b->t2;
  tctx.t2c = premult ? c->t2 * c->rhw : c->t2;
  tctx.arhw = a->rhw;
  tctx.brhw = b->rhw;
  tctx.crhw = c->rhw;
  tctx.dSdx = inv * (tctx.sta * e0dx + tctx.stb * e1dx + tctx.stc * e2dx);
  tctx.dSdy = inv * (tctx.sta * e0dy + tctx.stb * e1dy + tctx.stc * e2dy);
  tctx.dTdx = inv * (tctx.tta * e0dx + tctx.ttb * e1dx + tctx.ttc * e2dx);
  tctx.dTdy = inv * (tctx.tta * e0dy + tctx.ttb * e1dy + tctx.ttc * e2dy);
  tctx.dWdx = inv * (a->rhw * e0dx + b->rhw * e1dx + c->rhw * e2dx);
  tctx.dWdy = inv * (a->rhw * e0dy + b->rhw * e1dy + c->rhw * e2dy);
  double dZdx = ((double)e0dxi * azd + (double)e1dxi * bzd + (double)e2dxi * czd) * invd;
  double dZdy = ((double)e0dyi * azd + (double)e1dyi * bzd + (double)e2dyi * czd) * invd;
  tctx.dS2dx = 0.0f; tctx.dS2dy = 0.0f; tctx.dT2dx = 0.0f; tctx.dT2dy = 0.0f;
  if (ds->need_lod2 && ds->sec_sel) {
    tctx.dS2dx = inv * (tctx.s2a * e0dx + tctx.s2b * e1dx + tctx.s2c * e2dx);
    tctx.dS2dy = inv * (tctx.s2a * e0dy + tctx.s2b * e1dy + tctx.s2c * e2dy);
    tctx.dT2dx = inv * (tctx.t2a * e0dx + tctx.t2b * e1dx + tctx.t2c * e2dx);
    tctx.dT2dy = inv * (tctx.t2a * e0dy + tctx.t2b * e1dy + tctx.t2c * e2dy);
  }
  if (tex_en)
    r3d_stage_desc_init(rs, 0, &tctx.sd0);
  if (sec_en)
    r3d_stage_desc_init(rs, 1, &tctx.sd1);

  for (int py = y0; py <= y1; py++, e0ri += e0dyi, e1ri += e1dyi, e2ri += e2dyi, zr_d += dZdy) {
    if (!r128_row_owned(py, thr_id, thr_mask))
      continue;
    Bit64s e0 = e0ri, e1 = e1ri, e2 = e2ri;
    double zline = zr_d;
    Bit32u drow = rs->dst_offset + (c_tld ? 16u * cpb * ((Bit32u)py >> 4) + (((Bit32u)py & 15u) << 6) : (Bit32u)py * cpb);
    Bit32u zrow = rs->t3d.z_offset + (z_tld ? 16u * zpb * ((Bit32u)py >> 4) + (((Bit32u)py & 15u) << 6) : (Bit32u)py * zpb);
    int rx0 = -1, rx1 = -1;

    for (int px = x0; px <= x1; px++, e0 += e0dxi, e1 += e1dxi, e2 += e2dxi, zline += dZdx) {
      float w0, w1, w2;
      float col[4];
      Bit32u zaddr = 0, zi = 0, zbuf;
      Bit8u *zcell = NULL, *dcell;
      Bit32u daddr, dst = 0, out;

      if (area2i > 0) {
        if ((e0 < 0) || (e1 < 0) || (e2 < 0)) continue;
      } else {
        if ((e0 > 0) || (e1 > 0) || (e2 > 0)) continue;
      }
      if (aux_on && !rage128_aux_sc_pass(rs->aux_sc_cntl, rs->aux_sc_rect, px, py))
        continue;
      if (ds->stip_en && !((rs->stipple[py & 31] >> (31 - (px & 31))) & 1))
        continue;
      w0 = (float)e0 * invs;
      w1 = (float)e1 * invs;
      w2 = (float)e2 * invs;

      double zc = zline;
      if (!(zc > 0.0)) zc = 0.0;
      if (zc > 1.0) zc = 1.0;

      int zres = 1, sres = 1;
      Bit32u sbuf = 0;
      if (z_en || sten_on) {
        double zq = zc * (double)zmax + 0.5;
        if (zq > (double)zmax) zq = (double)zmax;
        zi = (Bit32u)zq;
        zaddr = zrow + (z_tld ? r128_tile_x((Bit32u)px * zbpp) : (Bit32u)px * zbpp);
        zcell = &vram[zaddr & vram_mask];
        if (zbpp == 2)
          zbuf = (zcell[0] | (zcell[1] << 8)) & zmax;
        else
          zbuf = ((zcell[0] | (zcell[1] << 8) | (zcell[2] << 16) | ((Bit32u)zcell[3] << 24)) >> zshift) & zmax;
        if (z_en)
          zres = r3d_cmp(zfn, zi, zbuf);
        if (sten_on) {
          Bit32u zw = zcell[0] | (zcell[1] << 8) | (zcell[2] << 16) | ((Bit32u)zcell[3] << 24);
          sbuf = (zw >> sshift) & 0xff;
          sres = r3d_cmp(sfn, sref & svmask, sbuf & svmask);
        } else if (!zres) {
          continue;
        }
      }

      col[0] = w0 * vca[0] + w1 * vcb[0] + w2 * vcc[0];
      col[1] = w0 * vca[1] + w1 * vcb[1] + w2 * vcc[1];
      col[2] = w0 * vca[2] + w1 * vcb[2] + w2 * vcc[2];
      col[3] = w0 * vca[3] + w1 * vcb[3] + w2 * vcc[3];

      if (tex_en || sec_en) {
        if (!r3d_texstage_run(&tctx, w0, w1, w2, col))
          continue;
      }
      if (ds->spec_en) {
        col[0] += w0 * spa[0] + w1 * spb[0] + w2 * spc[0];
        col[1] += w0 * spa[1] + w1 * spb[1] + w2 * spc[1];
        col[2] += w0 * spa[2] + w1 * spb[2] + w2 * spc[2];
        if (col[0] > 1.0f) col[0] = 1.0f;
        if (col[1] > 1.0f) col[1] = 1.0f;
        if (col[2] > 1.0f) col[2] = 1.0f;
      }
      if (ds->fog_en) {
        float f;
        if (ds->fog_table_en)
          f = rs->t3d.fog_table[(int)(zc * 255.0 + 0.5)] / 255.0f;
        else
          f = w0 * fga + w1 * fgb + w2 * fgc;
        if (f < 0.0f) f = 0.0f;
        if (f > 1.0f) f = 1.0f;
        col[0] = col[0] * f + fogr * (1.0f - f);
        col[1] = col[1] * f + fogg * (1.0f - f);
        col[2] = col[2] * f + fogb * (1.0f - f);
      }
      if (ds->atest_en && !r3d_cmp(ds->atest_fn, (Bit32u)(col[3] * 255.0f + 0.5f), ds->atest_ref))
        continue;

      if (sten_on) {
        Bit32u sop = !sres ? sfail_op : (zres ? zpass_op : zfail_op);
        Bit32u snew = sbuf;
        switch (sop) {
          case 0: break;
          case 1: snew = 0; break;
          case 2: snew = sref; break;
          case 3: snew = (sbuf == 0xff) ? 0xff : sbuf + 1; break;
          case 4: snew = (sbuf == 0) ? 0 : sbuf - 1; break;
          case 5: snew = ~sbuf & 0xff; break;
          case 6: snew = (sbuf + 1) & 0xff; break;
          default: snew = (sbuf - 1) & 0xff; break;
        }
        snew = (sbuf & ~swmask) | (snew & swmask);
        if (snew != sbuf) {
          Bit32u zw = zcell[0] | (zcell[1] << 8) | (zcell[2] << 16) | ((Bit32u)zcell[3] << 24);
          zw = (zw & ~(0xffu << sshift)) | (snew << sshift);
          zcell[0] = (Bit8u)zw; zcell[1] = (Bit8u)(zw >> 8); zcell[2] = (Bit8u)(zw >> 16); zcell[3] = (Bit8u)(zw >> 24);
        }
        if (!sres || !zres)
          continue;
      }

      daddr = drow + (c_tld ? r128_tile_x((Bit32u)px * bpp) : (Bit32u)px * bpp);
      dcell = &vram[daddr & vram_mask];
      if (alpha_en)
        dst = r3d_dst_read(dcell, dst_dt);

      if (alpha_en) {
        float dc[4], fs[4], fd[4];
        Bit32u bsrc = ds->bsrc, bdst = ds->bdst, bfcn = ds->bfcn;
        dc[0] = ((dst >> 16) & 0xff) / 255.0f;
        dc[1] = ((dst >> 8) & 0xff) / 255.0f;
        dc[2] = (dst & 0xff) / 255.0f;
        dc[3] = (dst >> 24) / 255.0f;
        if ((bsrc == 0xb) || (bsrc == 0xc)) {
          float sa = (bsrc == 0xb) ? col[3] : 1.0f - col[3];
          fs[0] = fs[1] = fs[2] = fs[3] = sa;
          fd[0] = fd[1] = fd[2] = fd[3] = 1.0f - sa;
        } else {
          r3d_blend_factor(bsrc, col, dc, fs);
          r3d_blend_factor(bdst, col, dc, fd);
        }
        for (int ch = 0; ch < 4; ch++) {
          float v = (bfcn & 2) ? col[ch] * fs[ch] - dc[ch] * fd[ch] : col[ch] * fs[ch] + dc[ch] * fd[ch];
          if (bfcn & 1)
            v = (float)(((int)floor(v * 255.0f + 0.5f)) & 0xff) / 255.0f;
          else
            v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
          col[ch] = v;
        }
      }

      out = ((Bit32u)(col[3] * 255.0f + 0.5f) << 24) | ((Bit32u)(col[0] * 255.0f + 0.5f) << 16) |
            ((Bit32u)(col[1] * 255.0f + 0.5f) << 8) | (Bit32u)(col[2] * 255.0f + 0.5f);
      r3d_dst_write(dcell, dst_dt, out, dither ? r3d_bayer4[py & 3][px & 3] : -1, wmask);
      if (rx0 < 0) rx0 = px;
      rx1 = px;

      if (z_en && ds->z_wr) {
        if (zbpp == 2) {
          zcell[0] = (Bit8u)zi;
          zcell[1] = (Bit8u)(zi >> 8);
        } else {
          Bit32u zw = zcell[0] | (zcell[1] << 8) | (zcell[2] << 16) | ((Bit32u)zcell[3] << 24);
          zw = (zw & ~(zmax << zshift)) | (zi << zshift);
          zcell[0] = (Bit8u)zw; zcell[1] = (Bit8u)(zw >> 8); zcell[2] = (Bit8u)(zw >> 16); zcell[3] = (Bit8u)(zw >> 24);
        }
      }
    }
    if (rx0 >= 0) {
      if (c_tld) {
        Bit32u trb = rs->dst_offset + 16u * cpb * ((Bit32u)py >> 4);
        Bit32u cl = ((Bit32u)rx0 * bpp) >> 6;
        Bit32u ch = ((Bit32u)rx1 * bpp) >> 6;
        r3d_mark_dirty(trb + (cl << 10), (ch - cl + 1u) << 10);
      } else {
        r3d_mark_dirty(drow + (Bit32u)rx0 * bpp, (Bit32u)(rx1 - rx0 + 1) * bpp);
      }
    }
  }
}

// Lines and points render as 1 px wide quads through the triangle path
static bool r3d_line_tris(const rage128_raster_state_t *rs, const r3d_vtx_t *a, const r3d_vtx_t *b, r3d_vtx_t v[6])
{
  float dx = b->x - a->x, dy = b->y - a->y;
  float len = (float)sqrt(dx * dx + dy * dy);
  float hx, hy, px, py;
  if (len == 0.0f)
    return false;
  hx = dx / len * 0.5f;
  hy = dy / len * 0.5f;
  px = -hy;
  py = hx;
  v[0] = *a; v[1] = *a; v[2] = *b; v[4] = *b;
  v[0].x += px; v[0].y += py;
  v[1].x -= px; v[1].y -= py;
  v[2].x += px; v[2].y += py;
  v[4].x -= px; v[4].y -= py;
  if (rs->t3d.setup_cntl & (1 << 18)) {
    v[0].x -= hx; v[0].y -= hy;
    v[1].x -= hx; v[1].y -= hy;
    v[2].x -= hx; v[2].y -= hy;
    v[4].x -= hx; v[4].y -= hy;
  }
  v[3] = v[1];
  v[5] = v[2];
  return true;
}

void bx_rage128_c::r3d_line(const rage128_raster_state_t *rs, int thr_id, int thr_mask,
                            const r3d_vtx_t *a, const r3d_vtx_t *b)
{
  r3d_vtx_t v[6];
  if (!r3d_line_tris(rs, a, b, v))
    return;
  r3d_tri(rs, thr_id, thr_mask, &v[0], &v[1], &v[2]);
  r3d_tri(rs, thr_id, thr_mask, &v[3], &v[4], &v[5]);
}

void bx_rage128_c::r3d_point(const rage128_raster_state_t *rs, int thr_id, int thr_mask, const r3d_vtx_t *p)
{
  r3d_vtx_t p0 = *p, p1 = *p, p2 = *p, p3 = *p;
  p0.x -= 0.5f; p0.y -= 0.5f;
  p1.x += 0.5f; p1.y -= 0.5f;
  p2.x += 0.5f; p2.y += 0.5f;
  p3.x -= 0.5f; p3.y += 0.5f;
  r3d_tri(rs, thr_id, thr_mask, &p0, &p1, &p2);
  r3d_tri(rs, thr_id, thr_mask, &p0, &p2, &p3);
}

// ---------------------------------------------------------------------
// Draw state capture
// ---------------------------------------------------------------------

// Stage one mip level from card space into the arena; returns the arena
// offset or R128_TEX_STAGE_NONE.
Bit32u bx_rage128_c::r3d_stage_level(Bit32u key, Bit32u vm, Bit32u len)
{
  for (Bit32u i = 0; i < tex_stage.ent_count; i++)
    if ((tex_stage.ent[i].vm_base == key) && (tex_stage.ent[i].len == len))
      return tex_stage.ent[i].arena_off;
  Bit32u off = (tex_stage.used + 15u) & ~15u;
  if ((tex_stage.arena == NULL) || (off + len > tex_stage.cap) || (off < tex_stage.used)) {
    Bit32u ncap = tex_stage.cap ? tex_stage.cap : (1u << 20);
    while (ncap < off + len) ncap <<= 1;
    Bit8u *na = (Bit8u*)realloc(tex_stage.arena, ncap);
    if (na == NULL)
      return R128_TEX_STAGE_NONE;
    tex_stage.arena = na;
    tex_stage.cap = ncap;
  }
  if (!pm4_bus_read_block(vm, tex_stage.arena + off, len))
    memset(tex_stage.arena + off, 0xff, len);
  tex_stage.used = off + len;
  if (tex_stage.ent_count < 256) {
    tex_stage.ent[tex_stage.ent_count].vm_base = key;
    tex_stage.ent[tex_stage.ent_count].len = len;
    tex_stage.ent[tex_stage.ent_count].arena_off = off;
    tex_stage.ent_count++;
  }
  return off;
}

// Stage every enabled-stage mip level whose offset lies in the AGP half
void bx_rage128_c::r3d_stage_textures(rage128_raster_state_t *rs)
{
  Bit32u tc = rs->t3d.tex_cntl;

  for (int st = 0; st < 2; st++) {
    Bit32u *stage = st ? rs->sec_stage_off : rs->prim_stage_off;
    bool en = st ? ((tc & R3D_TC_SEC_EN) != 0) : ((tc & R3D_TC_TEX_EN) != 0);
    Bit32u tsp, cntl;
    int top;

    for (int sl = 0; sl < 11; sl++)
      stage[sl] = R128_TEX_STAGE_NONE;
    if (!en)
      continue;
    tsp = R3D_TSP_HALF(rs, st);
    top = R3D_TOPSLOT(tsp);
    if (top < 0) top = 0;
    if (top > 10) top = 10;
    cntl = st ? rs->t3d.sec_tex_cntl : rs->t3d.prim_tex_cntl;
    for (int sl = 0; sl <= top; sl++) {
      Bit32u raw = st ? rs->t3d.sec_tex_offset[sl] : rs->t3d.prim_tex_offset[sl];
      Bit32u vm = raw & 0x3fffffff;
      Bit32u llw, llh, llen;
      r3d_level_dims(tsp, sl, &llw, &llh);
      llen = r3d_level_bytes((cntl >> 16) & 0xf, R3D_S3TC_CLASS(cntl), llw, llh);
      if (r128_card_is_agp(vm) || (llen && r128_card_is_agp(vm + llen - 1u))) {
        stage[sl] = r3d_stage_level(raw, vm, llen);
        if (stage[sl] == R128_TEX_STAGE_NONE)
          rs->stage_dead = 1;
      }
    }
  }
}

// Crack every register field the rasterizer consumes into rs->d
void bx_rage128_c::r3d_draw_state_derive(rage128_raster_state_t *rs)
{
  rage128_draw_state_t *ds = &rs->d;
  Bit32u tc = rs->t3d.tex_cntl;
  Bit32u misc = rs->t3d.misc_3d_state_cntl;
  Bit32u zwidth = (rs->t3d.z_sten_cntl >> 1) & 3;
  Bit32u colmode, fpucol;

  memset(ds, 0, sizeof(*ds));
  ds->dst_dt = (rs->dp_datatype >> 8) & 0xf;
  ds->bpp = r3d_dst_bpp(ds->dst_dt);
  ds->draw_ok = (ds->dst_dt == 3) || (ds->dst_dt == 4) || (ds->dst_dt == 6) || (ds->dst_dt == 15);
  ds->wmask = rs->t3d.plane_3d_mask;
  ds->dither = ((tc >> 8) & 1) && (ds->dst_dt != 6);
  ds->stip_en = (rs->dp_datatype & 0xf) == 9;
  ds->aux_on = (rs->aux_sc_cntl & RAGE128_AUX_SC_ENB_MASK) != 0;
  if (ds->aux_on) {
    for (int i = 0; i < 3; i++) {
      if (!(rs->aux_sc_cntl & (1u << (i * 2))))
        continue;
      ds->aux_cntl |= ((rs->aux_sc_cntl >> (i * 2)) & 3u) << (i * 2);
      ds->aux_x0[i] = rage128_sx14(rs->aux_sc_rect[i][0]);
      ds->aux_x1[i] = rage128_sx14(rs->aux_sc_rect[i][1]);
      ds->aux_y0[i] = rage128_sx14(rs->aux_sc_rect[i][2]);
      ds->aux_y1[i] = rage128_sx14(rs->aux_sc_rect[i][3]);
    }
  }
  ds->sx0 = rs->sc_top_left & 0x3fff;
  ds->sy0 = (rs->sc_top_left >> 16) & 0x3fff;
  ds->sx1 = rs->sc_bottom_right & 0x3fff;
  ds->sy1 = (rs->sc_bottom_right >> 16) & 0x3fff;

  ds->sub = (rs->t3d.setup_cntl & (1 << 19)) ? 16 : 4;
  ds->subf = (float)ds->sub;
  ds->rnd = (rs->t3d.fpu_setup >> 15) & 1;
  ds->slim = (ds->sub == 16) ? (1 << 15) : (1 << 14);
  if (rs->t3d.window_xy_offset) {
    ds->woyi = (Bit16s)(rs->t3d.window_xy_offset & 0xffff);
    ds->woxi = (Bit16s)(rs->t3d.window_xy_offset >> 16);
  }

  ds->z_en = (tc & R3D_TC_Z_EN) != 0;
  ds->z_wr = (tc & R3D_TC_Z_WR) != 0;
  ds->zfn = (rs->t3d.z_sten_cntl >> 4) & 7;
  ds->zbpp = zwidth ? 4 : 2;
  ds->zmax = (zwidth == 0) ? 0xffff : 0xffffff;
  ds->zshift = (zwidth == 2) ? 8 : 0;
  ds->zrowpx = (rs->t3d.z_pitch & 0xfffe) * 8;
  ds->z_tiled = r128_tiled_ok((rs->t3d.z_pitch >> 16) & 1u, ds->zrowpx * (Bit32u)ds->zbpp);
  ds->c_tiled = r128_tiled_ok(rs->dst_tiled, rs->dst_pitch * (Bit32u)ds->bpp);
  ds->sten_on = (tc & R3D_TC_STEN_EN) && (zwidth != 0);
  ds->sfn = (rs->t3d.z_sten_cntl >> 12) & 7;
  ds->sfail_op = (rs->t3d.z_sten_cntl >> 16) & 7;
  ds->zpass_op = (rs->t3d.z_sten_cntl >> 20) & 7;
  ds->zfail_op = (rs->t3d.z_sten_cntl >> 24) & 7;
  ds->sref = rs->t3d.sten_ref_mask & 0xff;
  ds->svmask = (rs->t3d.sten_ref_mask >> 16) & 0xff;
  ds->swmask = rs->t3d.sten_ref_mask >> 24;
  ds->sshift = ds->zshift ? 0 : 24;

  colmode = (rs->t3d.setup_cntl >> 3) & 7;
  fpucol = (rs->t3d.fpu_setup >> RAGE128_FPU_COLOR_SHIFT) & RAGE128_FPU_COLOR_MASK;
  ds->flat_on = (fpucol < 2) || ((colmode >= 1) && (colmode <= 3));
  if (fpucol < 2)
    ds->flat_src = (rs->t3d.fpu_setup & RAGE128_FPU_FLAT_VERTEX_OGL) ? 2 : 0;
  else
    ds->flat_src = (colmode == 2) ? 1 : (colmode == 3) ? 2 : 0;

  ds->tex_en = (tc & R3D_TC_TEX_EN) != 0;
  ds->sec_en = (tc & R3D_TC_SEC_EN) != 0;
  ds->premult = !(rs->t3d.setup_cntl & (1 << 9));
  ds->do_persp = !(rs->t3d.prim_tex_cntl & (1 << 14));
  ds->need_lod = (!((rs->t3d.prim_tex_cntl >> 7) & 1) && (((rs->t3d.prim_tex_cntl >> 1) & 7) >= 2)) ||
                 (((rs->t3d.prim_tex_cntl >> 1) & 1) != ((((rs->t3d.prim_tex_cntl >> 4) & 7) == 1)));
  ds->need_lod2 = (tc & R3D_TC_SEC_EN) &&
                  ((!((rs->t3d.sec_tex_cntl >> 7) & 1) && (((rs->t3d.sec_tex_cntl >> 1) & 7) >= 2)) ||
                   (((rs->t3d.sec_tex_cntl >> 1) & 1) != ((((rs->t3d.sec_tex_cntl >> 4) & 7) == 1))));
  ds->lod_bias = -(float)(Bit8s)((tc >> 24) & 0xff) / 128.0f;
  ds->sec_sel = rs->t3d.sec_tex_cntl & 1;

  for (int st = 0; st < 2; st++) {
    Bit32u cntl = st ? rs->t3d.sec_tex_cntl : rs->t3d.prim_tex_cntl;
    r3d_stage_hdr_t *h = &ds->sh[st];
    h->tsp = R3D_TSP_HALF(rs, st);
    h->clamp_s = (cntl >> 8) & 3;
    h->clamp_t = (cntl >> 11) & 3;
    h->dt = (cntl >> 16) & 0xf;
    h->s3tc = R3D_S3TC_CLASS(cntl);
    h->border = st ? rs->t3d.sec_tex_border_color : rs->t3d.prim_tex_border_color;
    h->minb = (cntl >> 1) & 7;
    h->mag = (cntl >> 4) & 7;
    h->mipdis = (cntl >> 7) & 1;
    h->top = R3D_TOPSLOT(h->tsp);
    if (h->top < 0) h->top = 0;
    if (h->top > 10) h->top = 10;
  }
  ds->texw0 = (float)(1u << (ds->sh[0].tsp & 0xf));
  ds->texh0 = (float)(1u << ((ds->sh[0].tsp >> 8) & 0xf));
  ds->texw1 = (float)(1u << (ds->sh[1].tsp & 0xf));
  ds->texh1 = (float)(1u << ((ds->sh[1].tsp >> 8) & 0xf));

  for (int st = 0; st < 2; st++) {
    Bit32u cc = st ? rs->t3d.sec_tex_combine_cntl : rs->t3d.prim_tex_combine_cntl;
    ds->comb[st].comb = cc & 0xf;
    ds->comb[st].fmsb = (cc >> 8) & 1;
    ds->comb[st].cfac = (cc >> 4) & 0xf;
    ds->comb[st].ifac = (cc >> 10) & 0xf;
    ds->comb[st].comba = (cc >> 14) & 0xf;
    ds->comb[st].afac = (cc >> 18) & 0xf;
    ds->comb[st].ifaca = (cc >> 25) & 0x7;
  }

  ds->ckfn = (misc >> 30) & 3;
  ds->ck3d_on = (ds->ckfn >= 2) && (rs->t3d.clr_cmp_msk_3d != 0);
  ds->ckc_on = ((tc >> 12) & 1) && (rs->t3d.tex_clr_cmp_msk != 0);
  ds->need_ck = ds->ck3d_on || ds->ckc_on;
  if (ds->tex_en) {
    ds->ck3d_clr = r3d_ck_to_argb(ds->sh[0].dt, rs->t3d.clr_cmp_clr_3d);
    ds->ck3d_msk = r3d_ck_to_argb(ds->sh[0].dt, rs->t3d.clr_cmp_msk_3d);
    ds->ckc_clr = r3d_ck_to_argb(ds->sh[0].dt, rs->t3d.tex_clr_cmp_clr);
    ds->ckc_msk = r3d_ck_to_argb(ds->sh[0].dt, rs->t3d.tex_clr_cmp_msk);
  }
  ds->cc[0] = ((rs->t3d.constant_color >> 16) & 0xff) / 255.0f;
  ds->cc[1] = ((rs->t3d.constant_color >> 8) & 0xff) / 255.0f;
  ds->cc[2] = (rs->t3d.constant_color & 0xff) / 255.0f;
  ds->cc[3] = (rs->t3d.constant_color >> 24) / 255.0f;

  ds->spec_en = (tc & R3D_TC_SPEC_EN) != 0;
  ds->fog_en = (tc & R3D_TC_FOG_EN) != 0;
  ds->fog_table_en = (misc & (1u << 14)) != 0;
  if (ds->fog_en) {
    ds->fogr = ((rs->t3d.fog_color >> 16) & 0xff) / 255.0f;
    ds->fogg = ((rs->t3d.fog_color >> 8) & 0xff) / 255.0f;
    ds->fogb = (rs->t3d.fog_color & 0xff) / 255.0f;
  }
  ds->atest_en = (tc & R3D_TC_ATEST_EN) != 0;
  ds->atest_fn = (misc >> 24) & 7;
  ds->atest_ref = misc & 0xff;
  ds->alpha_en = (tc & R3D_TC_ALPHA_EN) != 0;
  ds->bsrc = (misc >> 16) & 0xf;
  ds->bdst = (misc >> 20) & 0xf;
  ds->bfcn = (misc >> 12) & 3;
}

void bx_rage128_c::raster_state_capture(rage128_raster_state_t *rs)
{
  Bit32u tc;

  // Serial mode never flushes: recycle the staging arena per draw
  if (rb.nthreads <= 1) {
    tex_stage.used = 0;
    tex_stage.ent_count = 0;
  }

  rs->t3d = t3d;
  rs->dst_offset = dst_offset;
  rs->dst_pitch = dst_pitch;
  rs->dst_tiled = (dst_pitch_reg >> 16) & 1u;
  rs->dp_datatype = dp_datatype;
  rs->dp_write_mask = dp_write_mask;
  rs->sc_top_left = sc_top_left;
  rs->sc_bottom_right = sc_bottom_right;
  rs->aux_sc_cntl = aux_sc_cntl;
  memcpy(rs->aux_sc_rect, aux_sc_rect, sizeof(rs->aux_sc_rect));
  rs->dst_bpp = (Bit32u)r3d_dst_bpp((rs->dp_datatype >> 8) & 0xf);
  if ((rs->dp_datatype & 0xf) == 9)
    memcpy(rs->stipple, brush_data, sizeof(rs->stipple));
  else
    memset(rs->stipple, 0, sizeof(rs->stipple));
  rs->stage_dead = 0;

  // AGP-resident render targets / depth buffers are not supported: a
  // draw whose colour or Z surface lives in the AGP half is dropped
  // rather than aliased onto local VRAM.
  if (r128_card_is_agp(rs->dst_offset))
    rs->stage_dead = 1;
  if ((t3d.tex_cntl & (R3D_TC_Z_EN | R3D_TC_STEN_EN)) && r128_card_is_agp(t3d.z_offset & 0x3fffffff))
    rs->stage_dead = 1;

  r3d_stage_textures(rs);
  r3d_draw_state_derive(rs);

  // Byte span each enabled stage can sample (render-to-texture hazards)
  tc = rs->t3d.tex_cntl;
  for (int st = 0; st < 2; st++) {
    bool en = st ? ((tc & R3D_TC_SEC_EN) != 0) : ((tc & R3D_TC_TEX_EN) != 0);
    rs->tex_lo[st] = rs->tex_hi[st] = 0;
    if (en) {
      const r3d_stage_hdr_t *h = &rs->d.sh[st];
      Bit32u tsp = R3D_TSP_HALF(rs, st);
      Bit32u cntl = st ? rs->t3d.sec_tex_cntl : rs->t3d.prim_tex_cntl;
      int top = h->top;
      int low = ((st ? rs->d.need_lod2 : rs->d.need_lod) && !h->mipdis && (h->minb >= 2)) ? 0 : top;
      Bit32u lo = 0xffffffff, hi = 0;
      for (int sl = low; sl <= top; sl++) {
        Bit32u raw = st ? rs->t3d.sec_tex_offset[sl] : rs->t3d.prim_tex_offset[sl];
        Bit32u lw, lh, base, end;
        r3d_level_dims(tsp, sl, &lw, &lh);
        base = raw & 0x3fffffff;
        end = base + r3d_level_bytes((cntl >> 16) & 0xf, R3D_S3TC_CLASS(cntl), lw, lh);
        if (end < base) end = 0xffffffff;
        if (base < lo) lo = base;
        if (end > hi) hi = end;
      }
      if (hi > lo) {
        rs->tex_lo[st] = lo;
        rs->tex_hi[st] = hi;
      }
    }
  }
}

// ---------------------------------------------------------------------
// Draw packets
// ---------------------------------------------------------------------

bool bx_rage128_c::r3d_fetch_vertex(Bit32u base, Bit32u stride, Bit32u idx, Bit32u fmt, r3d_vtx_t *out)
{
  Bit32u vdw[20];
  if ((stride == 0) || (stride > 20))
    return false;
  // per-draw vertex cache for the indexed walk
  if (vtx_cache.cur && vtx_cache.cap && (idx < vtx_cache.cap) && (vtx_cache.gen[idx] == vtx_cache.cur)) {
    *out = vtx_cache.v[idx];
    return true;
  }
  for (Bit32u j = 0; j < stride; j++) {
    if (!pm4_bus_read(base + (idx * stride + j) * 4, &vdw[j]))
      return false;
  }
  r3d_decode_vertex(fmt, vdw, out);
  if (vtx_cache.cur) {
    if (idx >= vtx_cache.cap) {
      Bit32u ncap = vtx_cache.cap ? vtx_cache.cap : 1024;
      while (ncap <= idx) ncap <<= 1;
      r3d_vtx_t *nv = (r3d_vtx_t*)realloc(vtx_cache.v, ncap * sizeof(r3d_vtx_t));
      Bit32u *ng = (Bit32u*)realloc(vtx_cache.gen, ncap * sizeof(Bit32u));
      if (nv) vtx_cache.v = nv;
      if (ng) vtx_cache.gen = ng;
      if (!nv || !ng)
        return true;
      memset(vtx_cache.gen + vtx_cache.cap, 0, (ncap - vtx_cache.cap) * sizeof(Bit32u));
      vtx_cache.cap = ncap;
    }
    vtx_cache.v[idx] = *out;
    vtx_cache.gen[idx] = vtx_cache.cur;
  }
  return true;
}

// FPU_SETUP face cull (buffer walks only)
static bool r3d_vc_face_culled(Bit32u fs, const r3d_vtx_t *a, const r3d_vtx_t *b, const r3d_vtx_t *c)
{
  float area2 = (b->x - a->x) * (c->y - a->y) - (c->x - a->x) * (b->y - a->y);
  if (fs & (1u << 18))
    return false;
  bool front = (fs & RAGE128_FPU_FRONT_DIR_CCW) ? (area2 < 0.0f) : (area2 > 0.0f);
  Bit32u mode = (fs >> (front ? RAGE128_FPU_FRONTFACE_SHIFT : RAGE128_FPU_BACKFACE_SHIFT)) & RAGE128_FPU_FACE_MODE_MASK;
  return mode == RAGE128_FPU_FACE_CULL;
}

void bx_rage128_c::r3d_buffer_draw(const Bit32u *pl, Bit32u count, Bit32u base, Bit32u fmt, Bit32u stride,
                                   Bit32u prim, Bit32u num, bool indexed)
{
  r3d_vtx_t va, vb, vc;
  Bit32u i;
  Bit32u fs = t3d.fpu_setup;
  rage128_raster_state_t *rs = new rage128_raster_state_t;

  raster_state_capture(rs);
  if (indexed) {
    if (++vtx_cache.cur == 0) {
      if (vtx_cache.cap)
        memset(vtx_cache.gen, 0, vtx_cache.cap * sizeof(Bit32u));
      vtx_cache.cur = 1;
    }
  } else {
    vtx_cache.cur = 0;
  }

#define SRC(p) (indexed ? (((4u + (p) / 2u) < count) ? ((pl[4u + (p) / 2u] >> (((p) & 1u) ? 16u : 0u)) & 0xffffu) : 0xffffffffu) : (p))
#define GET(dst, p) \
  do { \
    Bit32u _s = SRC(p); \
    if ((_s == 0xffffffffu) || !r3d_fetch_vertex(base, stride, _s, fmt, &(dst))) { \
      vtx_cache.cur = 0; delete rs; return; \
    } \
  } while (0)

  switch (prim) {
    case 4:
      for (i = 0; i + 2 < num; i += 3) {
        GET(va, i); GET(vb, i + 1); GET(vc, i + 2);
        if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
          raster_submit_tri(rs, &va, &vb, &vc);
      }
      break;
    case 5:
      if (num < 3) break;
      GET(va, 0); GET(vb, 1);
      for (i = 2; i < num; i++) {
        GET(vc, i);
        if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
          raster_submit_tri(rs, &va, &vb, &vc);
        vb = vc;
      }
      break;
    case 6:
      if (num < 3) break;
      GET(va, 0); GET(vb, 1);
      for (i = 2; i < num; i++) {
        GET(vc, i);
        if (i & 1) {
          if (!r3d_vc_face_culled(fs, &vb, &va, &vc))
            raster_submit_tri(rs, &vb, &va, &vc);
        } else {
          if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
            raster_submit_tri(rs, &va, &vb, &vc);
        }
        va = vb;
        vb = vc;
      }
      break;
    case 2:
      for (i = 0; i + 1 < num; i += 2) {
        GET(va, i); GET(vb, i + 1);
        raster_submit_line(rs, &va, &vb);
      }
      break;
    case 3:
      if (num < 2) break;
      GET(va, 0);
      for (i = 1; i < num; i++) {
        GET(vb, i);
        raster_submit_line(rs, &va, &vb);
        va = vb;
      }
      break;
    case 1:
      for (i = 0; i < num; i++) {
        GET(va, i);
        raster_submit_point(rs, &va);
      }
      break;
    default:
      BX_DEBUG(("unhandled 3D prim type %u", prim));
      break;
  }
#undef GET
#undef SRC
  vtx_cache.cur = 0;
  delete rs;
}

static BX_CPP_INLINE bool r3d_vc_inhibited(Bit32u vc_debug_config)
{
  return (vc_debug_config & (RAGE128_VC_DEBUG_DONT_START | RAGE128_VC_DEBUG_NO_OUTPUT)) != 0;
}

// 3D_RNDR_GEN_INDX_PRIM: { vertex buffer address, count, VC_FORMAT, VC_CNTL, [u16 index pairs] }
void bx_rage128_c::r3d_draw_packet(const Bit32u *pl, Bit32u count)
{
  if ((count < 4) || r3d_vc_inhibited(pm4_vc_debug_config))
    return;
  Bit32u addr = pm4_vm_addr(pl[0]);
  Bit32u fmt = pl[2];
  Bit32u cntl = pl[3];
  Bit32u prim = RAGE128_VC_PRIM_TYPE(cntl);
  Bit32u walk = RAGE128_VC_PRIM_WALK(cntl);
  Bit32u num = RAGE128_VC_NUM(cntl);
  Bit32u stride = rage128_3d_vertex_dwords(fmt);

  if (stride == 0)
    return;
  switch (walk) {
    case RAGE128_VC_WALK_LIST:
      r3d_buffer_draw(pl, count, addr, fmt, stride, prim, num, false);
      break;
    case RAGE128_VC_WALK_IND:
      if (4 + (num + 1) / 2 > count)
        break;
      r3d_buffer_draw(pl, count, addr, fmt, stride, prim, num, true);
      break;
    default:
      break;
  }
}

// 3D_RNDR_GEN_PRIM: { VC_FORMAT, VC_CNTL, inline vertices }
void bx_rage128_c::r3d_draw_inline(const Bit32u *pl, Bit32u count)
{
  if ((count < 2) || r3d_vc_inhibited(pm4_vc_debug_config))
    return;
  Bit32u fmt = pl[0];
  Bit32u cntl = pl[1];
  Bit32u prim = RAGE128_VC_PRIM_TYPE(cntl);
  Bit32u walk = RAGE128_VC_PRIM_WALK(cntl);
  Bit32u num = RAGE128_VC_NUM(cntl);
  Bit32u stride = rage128_3d_vertex_dwords(fmt);

  if ((stride == 0) || (walk != RAGE128_VC_WALK_RING) || ((Bit64u)num * stride != count - 2))
    return;

  const Bit32u *vd = &pl[2];
  r3d_vtx_t va, vb, vc;
  Bit32u fs = t3d.fpu_setup;
  rage128_raster_state_t *rs = new rage128_raster_state_t;

  raster_state_capture(rs);
  switch (prim) {
    case 4:
      for (Bit32u i = 0; i + 2 < num; i += 3) {
        r3d_decode_vertex(fmt, &vd[i * stride], &va);
        r3d_decode_vertex(fmt, &vd[(i + 1) * stride], &vb);
        r3d_decode_vertex(fmt, &vd[(i + 2) * stride], &vc);
        if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
          raster_submit_tri(rs, &va, &vb, &vc);
      }
      break;
    case 5:
      if (num < 3) break;
      r3d_decode_vertex(fmt, &vd[0], &va);
      r3d_decode_vertex(fmt, &vd[stride], &vb);
      for (Bit32u i = 2; i < num; i++) {
        r3d_decode_vertex(fmt, &vd[i * stride], &vc);
        if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
          raster_submit_tri(rs, &va, &vb, &vc);
        vb = vc;
      }
      break;
    case 6:
      if (num < 3) break;
      r3d_decode_vertex(fmt, &vd[0], &va);
      r3d_decode_vertex(fmt, &vd[stride], &vb);
      for (Bit32u i = 2; i < num; i++) {
        r3d_decode_vertex(fmt, &vd[i * stride], &vc);
        if (i & 1) {
          if (!r3d_vc_face_culled(fs, &vb, &va, &vc))
            raster_submit_tri(rs, &vb, &va, &vc);
        } else {
          if (!r3d_vc_face_culled(fs, &va, &vb, &vc))
            raster_submit_tri(rs, &va, &vb, &vc);
        }
        va = vb;
        vb = vc;
      }
      break;
    case 2:
      for (Bit32u i = 0; i + 1 < num; i += 2) {
        r3d_decode_vertex(fmt, &vd[i * stride], &va);
        r3d_decode_vertex(fmt, &vd[(i + 1) * stride], &vb);
        raster_submit_line(rs, &va, &vb);
      }
      break;
    case 3:
      if (num < 2) break;
      r3d_decode_vertex(fmt, &vd[0], &va);
      for (Bit32u i = 1; i < num; i++) {
        r3d_decode_vertex(fmt, &vd[i * stride], &vb);
        raster_submit_line(rs, &va, &vb);
        va = vb;
      }
      break;
    case 1:
      for (Bit32u i = 0; i < num; i++) {
        r3d_decode_vertex(fmt, &vd[i * stride], &va);
        raster_submit_point(rs, &va);
      }
      break;
    default:
      BX_DEBUG(("unhandled 3D prim type %u", prim));
      break;
  }
  delete rs;
}

bool bx_rage128_c::r3d_packet3(Bit32u hdr, const Bit32u *pl, Bit32u count)
{
  Bit32u op = RAGE128_PM4_T3_OPCODE(hdr);
  switch (op) {
    case RAGE128_PM4_OP_3D_RNDR_GEN_INDX_PRIM:
      r3d_draw_packet(pl, count);
      return true;
    case RAGE128_PM4_OP_PURGE:
      return true;
    case RAGE128_PM4_OP_3D_RNDR_GEN_PRIM:
      r3d_draw_inline(pl, count);
      return true;
    case RAGE128_PM4_OP_LOAD_PALETTE:
      if (count >= 2) {
        Bit32u n = count - 1;
        if (n > 256) n = 256;
        for (Bit32u k = 0; k < n; k++)
          scl_palette[k] = pl[k + 1];
      }
      return true;
    case RAGE128_PM4_OP_3D_SAVE_CONTEXT:
    case RAGE128_PM4_OP_3D_PLAY_CONTEXT:
    case RAGE128_PM4_OP_NEXT_VERTEX_BUNDLE:
      return true;
    default:
      return false;
  }
}

// ---------------------------------------------------------------------
// Parallel rasterizer: deferred batch fanned out across persistent,
// scanline-interleaved worker threads
// ---------------------------------------------------------------------

BX_THREAD_FUNC(rage128_raster_worker_func, indata)
{
  rb_worker_t *w = (rb_worker_t *)indata;
  w->dev->raster_worker_main(w);
  BX_THREAD_EXIT;
}

void bx_rage128_c::raster_worker_main(rb_worker_t *w)
{
  for (;;) {
    bx_wait_sem(&w->wake);
    if (!raster_run)
      break;
    for (Bit32u i = 0; i < rb.cmd_count; i++) {
      const rb_cmd_t *c = &rb.cmds[i];
      const rage128_raster_state_t *rs = &rb.states[c->state_idx];
      if (c->py1 - c->py0 + 1 < rb.nthreads) {
        bool own = false;
        for (Bit32s y = c->py0; y <= c->py1; y++)
          if (r128_row_owned(y, w->id, w->mask)) { own = true; break; }
        if (!own)
          continue;
      }
      if (c->kind == RB_TRI)
        r3d_tri(rs, w->id, w->mask, &c->v[0], &c->v[1], &c->v[2]);
      else if (c->kind == RB_LINE)
        r3d_line(rs, w->id, w->mask, &c->v[0], &c->v[1]);
      else
        r3d_point(rs, w->id, w->mask, &c->v[0]);
    }
    bx_set_sem(&w->done);
  }
}

void bx_rage128_c::raster_init(void)
{
  int n = render_threads;
  if (n < 1) n = 1;
  if (n > RAGE128_RASTER_MAX_WORKERS) n = RAGE128_RASTER_MAX_WORKERS;
  memset(&rb, 0, sizeof(rb));
  rb.nthreads = n;
  rb.mask = (n & (n - 1)) ? -n : n - 1;
  rb_reset_written();
  raster_run = true;
  if (n <= 1)
    return;
  for (int i = 0; i < n; i++) {
    rb_worker_t *w = &rb.workers[i];
    w->dev = this;
    w->id = i;
    w->mask = rb.mask;
    bx_create_sem(&w->wake);
    bx_create_sem(&w->done);
    BX_THREAD_CREATE(rage128_raster_worker_func, w, w->thread);
  }
}

void bx_rage128_c::raster_close(void)
{
  if (!raster_run)
    return;
  raster_run = false;
  if (rb.nthreads > 1) {
    for (int i = 0; i < rb.nthreads; i++)
      bx_set_sem(&rb.workers[i].wake);
    for (int i = 0; i < rb.nthreads; i++) {
      BX_THREAD_JOIN(rb.workers[i].thread);
      bx_destroy_sem(&rb.workers[i].wake);
      bx_destroy_sem(&rb.workers[i].done);
    }
  }
  if (rb.cmds) free(rb.cmds);
  if (rb.states) free(rb.states);
  rb.cmds = NULL;
  rb.states = NULL;
  rb.nthreads = 1;
}

void bx_rage128_c::rb_run_parallel(void)
{
  for (int i = 0; i < rb.nthreads; i++)
    bx_set_sem(&rb.workers[i].wake);
  for (int i = 0; i < rb.nthreads; i++)
    bx_wait_sem(&rb.workers[i].done);
}

// Render the current batch and reset it
void bx_rage128_c::raster_flush(void)
{
  if ((rb.nthreads > 1) && (rb.cmd_count != 0)) {
    rb_run_parallel();
    rb.cmd_count = 0;
    rb.state_count = 0;
    rb.rt_valid = false;
    rb.rt_z_valid = false;
    rb_reset_written();
    cce_batch_pending = 0;
  }
  // recycle the texture staging arena (workers are idle)
  tex_stage.used = 0;
  tex_stage.ent_count = 0;
}

void bx_rage128_c::raster_abandon(void)
{
  rb.cmd_count = 0;
  rb.state_count = 0;
  rb.rt_valid = false;
  rb.rt_z_valid = false;
  rb_reset_written();
  cce_batch_pending = 0;
}

void bx_rage128_c::rb_reset_written(void)
{
  rb.wr_valid = false;
  rb.wr_c_lo = rb.wr_z_lo = 0xffffffff;
  rb.wr_c_hi = rb.wr_z_hi = 0;
}

static void rb_rows_to_bytes(Bit32u base, Bit32u stride, int tiled, Bit32s y0, Bit32s y1, Bit32u *lo, Bit32u *hi)
{
  Bit64u a, b;
  if (!stride || (y1 < y0))
    return;
  if (y0 < 0) y0 = 0;
  if (tiled) {
    a = (Bit64u)base + 16ull * ((Bit32u)y0 >> 4) * stride;
    b = (Bit64u)base + 16ull * (((Bit32u)y1 >> 4) + 1u) * stride;
  } else {
    a = (Bit64u)base + (Bit64u)(Bit32u)y0 * stride;
    b = (Bit64u)base + ((Bit64u)(Bit32u)y1 + 1u) * stride;
  }
  if (a > 0xffffffffull) a = 0xffffffffull;
  if (b > 0xffffffffull) b = 0xffffffffull;
  if (b <= a)
    return;
  *lo = (Bit32u)a;
  *hi = (Bit32u)b;
}

static BX_CPP_INLINE bool rb_rng_hit(Bit32u alo, Bit32u ahi, Bit32u blo, Bit32u bhi)
{
  return (ahi > alo) && (bhi > blo) && (alo < bhi) && (ahi > blo);
}

static bool rb_tex_hits(const rage128_raster_state_t *rs, Bit32u lo, Bit32u hi)
{
  for (int st = 0; st < 2; st++)
    if (rb_rng_hit(rs->tex_lo[st], rs->tex_hi[st], lo, hi))
      return true;
  return false;
}

static void rb_prim_rows(const rage128_raster_state_t *rs, const r3d_vtx_t *v, int n, Bit32s *y0, Bit32s *y1)
{
  float ymin = v[0].y, ymax = v[0].y;
  for (int k = 1; k < n; k++) {
    if (v[k].y < ymin) ymin = v[k].y;
    if (v[k].y > ymax) ymax = v[k].y;
  }
  if (!(ymin <= ymax) || (ymin < -65536.0f) || (ymax > 65536.0f)) {
    *y0 = -0x3fffffff;
    *y1 = 0x3fffffff;
    return;
  }
  int sub = (rs->t3d.setup_cntl & (1 << 19)) ? 16 : 4;
  float wof = 0.0f;
  if (rs->t3d.window_xy_offset)
    wof = (float)(Bit16s)(rs->t3d.window_xy_offset & 0xffff) / (float)sub;
  *y0 = (Bit32s)floor(ymin + wof) - 2;
  *y1 = (Bit32s)ceil(ymax + wof) + 2;
}

static void rb_prim_ranges(const rage128_raster_state_t *rs, Bit32s y0, Bit32s y1,
                           Bit32u *clo, Bit32u *chi, Bit32u *zlo, Bit32u *zhi)
{
  Bit32s sy0 = (Bit32s)((rs->sc_top_left >> 16) & 0x3fff);
  Bit32s sy1 = (Bit32s)((rs->sc_bottom_right >> 16) & 0x3fff);
  *clo = *zlo = 0xffffffff;
  *chi = *zhi = 0;
  if (y0 < sy0) y0 = sy0;
  if (y1 > sy1) y1 = sy1;
  if (y1 < y0)
    return;
  rb_rows_to_bytes(rs->dst_offset, rs->dst_pitch * rs->dst_bpp, rs->d.c_tiled, y0, y1, clo, chi);
  if (rs->d.z_en || rs->d.sten_on)
    rb_rows_to_bytes(rs->t3d.z_offset, rs->d.zrowpx * (Bit32u)rs->d.zbpp, rs->d.z_tiled, y0, y1, zlo, zhi);
}

Bit32u bx_rage128_c::rb_intern_state(const rage128_raster_state_t *rs)
{
  if ((rb.state_count > 0) && (memcmp(&rb.states[rb.state_count - 1], rs, sizeof(*rs)) == 0))
    return rb.state_count - 1;
  if (rb.state_count == rb.state_cap) {
    rb.state_cap = rb.state_cap ? rb.state_cap * 2u : 256u;
    rb.states = (rage128_raster_state_t*)realloc(rb.states, rb.state_cap * sizeof(*rb.states));
  }
  rb.states[rb.state_count] = *rs;
  return rb.state_count++;
}

void bx_rage128_c::rb_guard_rt(const rage128_raster_state_t *rs)
{
  bool zt = rs->d.z_en || rs->d.sten_on;
  Bit32u zs = rs->d.zrowpx * (Bit32u)rs->d.zbpp;
  if (rb.rt_valid &&
      (((rs->dst_offset != rb.rt_dst_offset) || (rs->dst_pitch != rb.rt_dst_pitch) ||
        (rs->dst_bpp != rb.rt_dst_bpp) || ((Bit32u)rs->d.c_tiled != rb.rt_dst_tiled)) ||
       (zt && rb.rt_z_valid && ((rs->t3d.z_offset != rb.rt_z_offset) || (zs != rb.rt_z_stride) ||
                                ((Bit32u)rs->d.z_tiled != rb.rt_z_tiled)))))
    raster_flush();
  rb.rt_valid = true;
  rb.rt_dst_offset = rs->dst_offset;
  rb.rt_dst_pitch = rs->dst_pitch;
  rb.rt_dst_bpp = rs->dst_bpp;
  rb.rt_dst_tiled = (Bit32u)rs->d.c_tiled;
  if (zt) {
    rb.rt_z_valid = true;
    rb.rt_z_offset = rs->t3d.z_offset;
    rb.rt_z_stride = zs;
    rb.rt_z_tiled = (Bit32u)rs->d.z_tiled;
  }
}

void bx_rage128_c::rb_enqueue(const rage128_raster_state_t *rs, int kind, const r3d_vtx_t *v, int n)
{
  rb_cmd_t *cmd;
  Bit32u clo, chi, zlo, zhi;
  Bit32s y0, y1;

  rb_prim_rows(rs, v, n, &y0, &y1);
  rb_prim_ranges(rs, y0, y1, &clo, &chi, &zlo, &zhi);

  // self-feedback: render inline
  if (rb_tex_hits(rs, clo, chi) || rb_tex_hits(rs, zlo, zhi)) {
    raster_flush();
    if (kind == RB_TRI)
      r3d_tri(rs, 0, 0, &v[0], &v[1], &v[2]);
    else if (kind == RB_LINE)
      r3d_line(rs, 0, 0, &v[0], &v[1]);
    else
      r3d_point(rs, 0, 0, &v[0]);
    return;
  }
  if (rb.wr_valid && (rb_tex_hits(rs, rb.wr_c_lo, rb.wr_c_hi) || rb_tex_hits(rs, rb.wr_z_lo, rb.wr_z_hi)))
    raster_flush();
  rb_guard_rt(rs);

  if (rb.cmd_count == rb.cmd_cap) {
    rb.cmd_cap = rb.cmd_cap ? rb.cmd_cap * 2u : 4096u;
    rb.cmds = (rb_cmd_t*)realloc(rb.cmds, rb.cmd_cap * sizeof(*rb.cmds));
  }
  cmd = &rb.cmds[rb.cmd_count++];
  cmd->state_idx = rb_intern_state(rs);
  cmd->kind = (Bit32u)kind;
  cmd->py0 = y0;
  cmd->py1 = y1;
  for (int k = 0; k < n; k++)
    cmd->v[k] = v[k];
  if (chi > clo) {
    if (clo < rb.wr_c_lo) rb.wr_c_lo = clo;
    if (chi > rb.wr_c_hi) rb.wr_c_hi = chi;
    rb.wr_valid = true;
  }
  if (zhi > zlo) {
    if (zlo < rb.wr_z_lo) rb.wr_z_lo = zlo;
    if (zhi > rb.wr_z_hi) rb.wr_z_hi = zhi;
    rb.wr_valid = true;
  }
  cce_batch_pending = 1;
}

// COMPOSITE_SHADOW_ID [23:0] counts executed 3D primitives
void bx_rage128_c::shadow_id_advance(void)
{
  Bit32u id = t3d.composite_shadow_id;
  if (id & RAGE128_SHADOW_AUTO_INC_DIS)
    return;
  t3d.composite_shadow_id = (id & ~RAGE128_SHADOW_ID_MASK) | ((id + 1) & RAGE128_SHADOW_ID_MASK);
}

void bx_rage128_c::raster_submit_tri(const rage128_raster_state_t *rs, const r3d_vtx_t *a, const r3d_vtx_t *b, const r3d_vtx_t *c)
{
  r3d_vtx_t v[3];
  shadow_id_advance();
  if (rs->stage_dead)
    return;
  if (rb.nthreads <= 1) {
    r3d_tri(rs, 0, 0, a, b, c);
    return;
  }
  v[0] = *a; v[1] = *b; v[2] = *c;
  rb_enqueue(rs, RB_TRI, v, 3);
}

void bx_rage128_c::raster_submit_line(const rage128_raster_state_t *rs, const r3d_vtx_t *a, const r3d_vtx_t *b)
{
  r3d_vtx_t v[2];
  shadow_id_advance();
  if (rs->stage_dead)
    return;
  if (rb.nthreads <= 1) {
    r3d_line(rs, 0, 0, a, b);
    return;
  }
  v[0] = *a; v[1] = *b;
  rb_enqueue(rs, RB_LINE, v, 2);
}

void bx_rage128_c::raster_submit_point(const rage128_raster_state_t *rs, const r3d_vtx_t *v)
{
  shadow_id_advance();
  if (rs->stage_dead)
    return;
  if (rb.nthreads <= 1) {
    r3d_point(rs, 0, 0, v);
    return;
  }
  rb_enqueue(rs, RB_POINT, v, 1);
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_RAGE128
