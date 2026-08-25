/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation
//
//  The device model follows the structure of the 86Box Rage 128 Pro
//  emulation: a VGA-compatible core with an extended CRTC / PLL / DAC
//  display block, a synchronous 2D GUI engine, a PM4/CCE command
//  processor whose packets execute on a worker thread (the same pattern
//  the Voodoo FIFO thread uses), a software 3D rasterizer with optional
//  scanline-interleaved render workers, and the OV0 video overlay.
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

#ifndef BX_IODEV_RAGE128_H
#define BX_IODEV_RAGE128_H

#if BX_SUPPORT_RAGE128

#include "bxthread.h"

// The Rage 128 model never uses the "static member functions" trick: the
// engines run on worker threads and every module needs the object pointer.
#define BX_RAGE128_THIS this->
#define BX_RAGE128_THIS_PTR this

#define RAGE128_VRAM_MAX (32 * 1024 * 1024)

// CCE (command processor) FIFO between the CPU-thread ring pump and the
// executor thread: 2^19 dwords = 2 MB.
#define RAGE128_CCE_FIFO_DWORDS (1u << 19)
#define RAGE128_CCE_FIFO_MASK   (RAGE128_CCE_FIFO_DWORDS - 1)
#define RAGE128_CCE_TAG_IB      1
#define RAGE128_PM4_MAX_PAYLOAD 16384

#define RAGE128_RASTER_MAX_WORKERS 8

// Card address space: bit 25 splits local VRAM (below) from the AGP system
// memory image (above). Shared by every engine.
static inline bool r128_card_is_agp(Bit32u card_addr)
{
  return (card_addr & R128_CARD_AGP_HALF) != 0;
}

// 64 byte x 16 line tile transform shared by every tiled consumer.
static inline Bit32u r128_tile_off(Bit32u xb, Bit32u y, Bit32u pitch_b)
{
  return 16u * pitch_b * (y >> 4) + ((xb >> 6) << 10) + ((y & 15u) << 6) + (xb & 63u);
}

static inline Bit32u r128_tile_x(Bit32u xb)
{
  return ((xb >> 6) << 10) + (xb & 63u);
}

static inline bool r128_tiled_ok(Bit32u tile_bit, Bit32u pitch_b)
{
  return tile_bit && pitch_b && !(pitch_b & 63u);
}

// 14-bit signed coordinate field (S.14.0)
static inline int rage128_sx14(Bit32u v)
{
  return ((Bit32s)((v & 0x3fff) << 18)) >> 18;
}

// One decoded 3D vertex in the fixed VC_FORMAT component order.
typedef struct {
  float  x, y, z, rhw;
  Bit32u diffuse;   // ARGB
  Bit32u spec;      // spec B,G,R bytes + fog factor byte
  float  s, t;
  float  s2, t2;
  float  rhw2;
} r3d_vtx_t;

// Cracked TEX_COMBINE_CNTL fields, decoded once per draw.
typedef struct {
  Bit32u comb, fmsb, cfac, ifac;
  Bit32u comba, afac, ifaca;
} r3d_comb_desc_t;

// Immutable per-stage sampler constants.
typedef struct {
  Bit32u tsp;
  Bit32u clamp_s, clamp_t;
  Bit32u dt, s3tc;
  Bit32u border;
  Bit32u minb, mag;
  int    mipdis, top;
} r3d_stage_hdr_t;

// Flat per-draw state: every register field the rasterizer consumes.
typedef struct {
  int    draw_ok;
  Bit32u dst_dt;
  int    bpp;
  Bit32u wmask;
  int    dither;
  int    stip_en;
  int    c_tiled;
  int    aux_on;
  Bit32u aux_cntl;
  Bit32s aux_x0[3], aux_x1[3], aux_y0[3], aux_y1[3];
  int    sx0, sy0, sx1, sy1;
  int    sub;
  float  subf;
  int    rnd;
  Bit32s slim;
  Bit32s woxi, woyi;
  int    z_en, z_wr;
  Bit32u zfn;
  int    zbpp;
  Bit32u zmax;
  int    zshift;
  Bit32u zrowpx;
  int    z_tiled;
  int    sten_on;
  Bit32u sfn, sfail_op, zpass_op, zfail_op;
  Bit32u sref, svmask, swmask;
  int    sshift;
  int    flat_on;
  int    flat_src;
  int    tex_en, sec_en;
  int    premult, do_persp;
  int    need_lod, need_lod2;
  float  lod_bias;
  float  texw0, texh0, texw1, texh1;
  int    sec_sel;
  r3d_stage_hdr_t sh[2];
  r3d_comb_desc_t comb[2];
  int    need_ck, ck3d_on, ckc_on;
  Bit32u ckfn;
  Bit32u ck3d_clr, ck3d_msk, ckc_clr, ckc_msk;
  float  cc[4];
  int    spec_en;
  int    fog_en, fog_table_en;
  float  fogr, fogg, fogb;
  int    atest_en;
  Bit32u atest_fn, atest_ref;
  int    alpha_en;
  Bit32u bsrc, bdst, bfcn;
} rage128_draw_state_t;

// 3D engine CCE context (register images)
typedef struct {
  Bit32u fpu_setup;
  Bit32u scale_3d_cntl;
  Bit32u scale_3d_datatype;
  Bit32u composite_shadow_id;
  Bit32u clr_cmp_clr_3d;
  Bit32u clr_cmp_msk_3d;
  Bit32u setup_cntl;
  Bit32u setup_cntl_pm4;
  Bit32u window_xy_offset;
  Bit32u z_offset;
  Bit32u z_pitch;
  Bit32u z_sten_cntl;
  Bit32u tex_cntl;
  Bit32u misc_3d_state_cntl;
  Bit32u tex_clr_cmp_clr;
  Bit32u tex_clr_cmp_msk;
  Bit32u fog_color;
  Bit32u prim_tex_cntl;
  Bit32u prim_tex_combine_cntl;
  Bit32u tex_size_pitch;
  Bit32u prim_tex_offset[11];
  Bit32u sec_tex_cntl;
  Bit32u sec_tex_combine_cntl;
  Bit32u sec_tex_offset[11];
  Bit32u constant_color;
  Bit32u prim_tex_border_color;
  Bit32u sec_tex_border_color;
  Bit32u sten_ref_mask;
  Bit32u plane_3d_mask;
  Bit32u tex_palette[256];
  Bit8u  fog_table[256];
} rage128_t3d_ctx_t;

// Per-draw rasterizer state snapshot: a primitive is rasterized under the
// engine state latched when it was submitted.
typedef struct {
  rage128_t3d_ctx_t t3d;
  Bit32u dst_offset;
  Bit32u dst_pitch;
  Bit32u dst_tiled;
  Bit32u dp_datatype;
  Bit32u dp_write_mask;
  Bit32u sc_top_left;
  Bit32u sc_bottom_right;
  Bit32u aux_sc_cntl;
  Bit32u aux_sc_rect[3][4];
  Bit32u stipple[32];
  Bit32u dst_bpp;
  Bit32u tex_lo[2];
  Bit32u tex_hi[2];
  // Per mip slot staging arena byte offset (0xffffffff = VRAM resident)
  Bit32u prim_stage_off[11];
  Bit32u sec_stage_off[11];
  int    stage_dead;
  rage128_draw_state_t d;
} rage128_raster_state_t;

#define R128_TEX_STAGE_NONE 0xffffffffu

// Per-triangle resolved sampler state (one per stage)
typedef struct {
  Bit32u tsp;
  Bit32u clamp_s, clamp_t, dt, s3tc;
  Bit32u border;
  Bit32u minb, mag;
  int    mipdis, top;
  const Bit32u *pal;
  Bit16u slot_valid;
  Bit16u slot_tiled;
  struct {
    Bit32u lw, lh;
    const Bit8u *texbase;
    Bit32u base, mask;
  } slot[11];
} r3d_stage_desc_t;

// Per-triangle texture stage context
typedef struct {
  const rage128_raster_state_t *rs;
  float sta, stb, stc, tta, ttb, ttc;
  float s2a, s2b, s2c, t2a, t2b, t2c;
  float arhw, brhw, crhw;
  float dSdx, dSdy, dTdx, dTdy;
  float dWdx, dWdy;
  float dS2dx, dS2dy, dT2dx, dT2dy;
  r3d_stage_desc_t sd0, sd1;
} r3d_texctx_t;

// One deferred primitive in the parallel raster batch
#define RB_TRI   0
#define RB_LINE  1
#define RB_POINT 2

typedef struct {
  Bit32u    state_idx;
  Bit32u    kind;
  Bit32s    py0, py1;
  r3d_vtx_t v[3];
} rb_cmd_t;

class bx_rage128_c;

typedef struct {
  bx_rage128_c *dev;
  int  id;
  int  mask;
  BX_THREAD_VAR(thread);
  bx_thread_sem_t wake;
  bx_thread_sem_t done;
} rb_worker_t;

// Staging of an AGP-resident (or tiled) surface span
typedef struct {
  Bit8u *arena;
  Bit32u cap;
  Bit32u vm;
  Bit32u len;
  bool   active;
  bool   tiled;
  Bit32u tbase, tpitch;
} rage128_span_stage_t;

// Surface window for the synchronous 2D executors: local VRAM or a staged
// copy of an AGP-resident / tiled span.
typedef struct {
  Bit8u *base;
  Bit32u rel;
  Bit32u mask;
  rage128_span_stage_t *st;
} r128_surf_t;

static inline Bit8u *r128_surf_at(const r128_surf_t *s, Bit32u card_addr)
{
  return &s->base[(card_addr - s->rel) & s->mask];
}

static inline Bit8u *r128_surf_run(const r128_surf_t *s, Bit32u card_addr, Bit32u len)
{
  Bit32u idx = (card_addr - s->rel) & s->mask;
  if ((Bit64u)idx + len > (Bit64u)s->mask + 1u)
    return NULL;
  return &s->base[idx];
}

// Auxiliary scissor tests (rage128_2d.cc): 3D lane semantics and the
// 2D twin (only subtractive rects carve).
bool rage128_aux_sc_pass(Bit32u cntl, const Bit32u rect[3][4], int x, int y);
bool rage128_aux_sc_pass_2d(Bit32u cntl, const Bit32u rect[3][4], int x, int y);
Bit32u rage128_rop3(Bit8u rop, Bit32u p, Bit32u s, Bit32u d);

// Colour-compare predicate state
struct r128_ccmp {
  int    src_on, dst_on;
  int    fn_src, fn_dst;
  Bit32u key_src, key_dst;
  Bit32u smask, dmask;
  Bit32u flip;
};

class bx_rage128_c : public bx_vgacore_c {
public:
  bx_rage128_c();
  virtual ~bx_rage128_c();

  virtual bool init_vga_extension(void);
  virtual void get_crtc_params(bx_crtc_params_t *crtcp, Bit32u *vclock);
  virtual void reset(unsigned type);
  virtual void redraw_area(unsigned x0, unsigned y0, unsigned width, unsigned height);
  virtual Bit8u mem_read(bx_phy_address addr);
  virtual void mem_write(bx_phy_address addr, Bit8u value);
  virtual void get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight, unsigned *txWidth);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual void vertical_timer(void);
#if BX_SUPPORT_PCI
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
#endif
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

  // Config option support (rage128.cc)
  static Bit32s options_parser(const char *context, int num_params, char *params[]);
  static Bit32s options_save(FILE *fp);

  // ---- entry points used by the worker threads (public on purpose) ----
  void cce_thread_main(void);
  void raster_worker_main(rb_worker_t *w);
  bool cce_thread_run;
  bool raster_run;

protected:
  virtual void update(void);

private:
  // ---- rage128.cc: plugin glue, PCI, MMIO/IO dispatch, chip core ----
  static Bit32u svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u svga_read(Bit32u address, unsigned io_len);
  void   svga_write(Bit32u address, Bit32u value, unsigned io_len);
  static Bit32u io_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   io_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool lfb_read(bx_phy_address addr, unsigned len, Bit8u *data);
  bool lfb_write(bx_phy_address addr, unsigned len, Bit8u *data);
  void mmio_read(Bit32u off, unsigned len, Bit8u *data);
  void mmio_write(Bit32u off, unsigned len, Bit8u *data);
  static void vline_timer_handler(void *);
  static void pump_timer_handler(void *);

  void init_members(void);
  void init_pcihandlers(void);
  void chip_reset(void);
  void set_irq_level(bool level);
  void gen_int_update(void);
  void gen_int_fold_gui_idle(void);
  void fold_deferred(void);
  void timing_update(void);
  void update_vga_decode(void);
  void update_banking(void);

  Bit32u reg_read32(Bit32u off);
  void   reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   reg_poke(Bit32u off, Bit32u val) { reg_write(off, val, 0xffffffff); }
  bool   agp_image_read(Bit32u off, Bit8u *dst, Bit32u len);
  bool   agp_image_write(Bit32u off, const Bit8u *src, Bit32u len);
  Bit32u surf_xlate(Bit32u addr);
  void   vram_dirty(Bit32u addr, Bit32u len);

  // ---- rage128.cc: display block (PLL / CRTC / DAC / cursor / scanout) ----
  bool   display_reg_read(Bit32u off, Bit32u *val);
  bool   display_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   display_reset(void);
  void   ppll_commit(void);
  double ppll_vco_hz(int sel);
  double dot_clock_hz(void);
  double xpll_hz(void);
  double mpll_hz(void);
  double test_clock_hz(void);
  Bit32u pll_read(void);
  void   pll_write(Bit32u val, Bit32u mask);
  void   palette_data_write(Bit32u val, Bit32u mask);
  Bit32u palette_data_read(void);
  void   update_mode(void);
  void   latch_crtc_offset(void);
  void   cursor_publish(void);
  void   cursor_frame_latch(void);
  void   timing_recalc(void);
  void   cursor_rect(Bit32u posn, Bit32u hvoff, int *x0, int *y0, int *x1, int *y1);
  void   paint_tile(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info, Bit8u *pel8);
  void   paint_tiles_in(int x0, int y0, int x1, int y1);
  void   draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info);
  Bit32u get_display_usec(void);
  Bit32u get_current_line(void);
  bool   in_vblank(void);
  void   snapshot_take(void);
  void   redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height);
  bool   ext_mode(void) { return (crtc_gen_cntl & 0x01000000) != 0; }

  // ---- rage128_2d.cc: card-space resolver (local VRAM / AGP / tiled) ----
  bool   card_read_block(Bit32u vm, Bit8u *dst, Bit32u len);
  bool   card_write_block(Bit32u vm, const Bit8u *src, Bit32u len);
  bool   card_copy_tiled(Bit32u tbase, Bit32u tpitch, Bit32u lin, Bit8u *buf, Bit32u len, bool dir);
  bool   span_stage_grow(rage128_span_stage_t *st, Bit32u need);
  bool   span_stage_acquire(rage128_span_stage_t *st, Bit32u vm, Bit32u extent);
  bool   span_stage_acquire_tiled(rage128_span_stage_t *st, Bit32u tbase, Bit32u tpitch, Bit32u extent);
  void   span_stage_writeback(rage128_span_stage_t *st);
  bool   surf_map(r128_surf_t *s, rage128_span_stage_t *st, Bit32u lo, Bit32u len);
  bool   surf_map_tiled(r128_surf_t *s, rage128_span_stage_t *st, Bit32u tbase, Bit32u tpitch, Bit32u lo, Bit32u len);

  // ---- rage128_2d.cc: 2D GUI engine ----
  void   r2d_span_rect(int y, int h, int xl, int xr, const Bit8u *mono8x8, const Bit8u *col8x8, bool mono_la);
  bool   r2d_reg_read(Bit32u off, Bit32u *val);
  bool   r2d_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   r2d_reset(void);
  void   r2d_apply_gmc(Bit32u gmc);
  void   r2d_set_pitch_offset(bool is_dst, Bit32u val);
  void   r2d_clip(int *x0, int *y0, int *x1, int *y1);
  bool   r2d_map_span(r128_surf_t *s, rage128_span_stage_t *st, Bit32u surf_base,
                      Bit32u pitch_px, int bpp, int y0, int y1, int x1, bool is_dst);
  void   r2d_surf_commit(r128_surf_t *s);
  void   r2d_surf_release(r128_surf_t *s);
  void   r2d_mark_dirty(const r128_surf_t *s, Bit32u addr, Bit32u len);
  int    r2d_ccmp_setup(bool has_src, int sbpp, int dbpp, struct r128_ccmp *cc);
  Bit32u r2d_dst_addr(int x, int y, int bpp);
  Bit32u r2d_src_addr(int x, int y, int bpp);
  void   r2d_paint_rect(int x, int y, int w, int h, const Bit8u *mono8x8, const Bit8u *col8x8, bool mono_la);
  void   r2d_line(int x0, int y0, int x1, int y1, Bit32u pat32, bool pat_en, bool pat_la, int *phase);
  void   r2d_bres_line(Bit32u dir);
  void   r2d_blit_rect_pat(int sx, int sy, int dx, int dy, int w, int h, const Bit8u *pat8x8, const Bit8u *col8x8, bool mono_la);
  void   r2d_blit_rect(int sx, int sy, int dx, int dy, int w, int h);
  void   r2d_blit_rect_key(int sx, int sy, int dx, int dy, int w, int h, Bit32u key, Bit32u kmask, bool key_eq_skip);
  Bit32u r2d_stretch_texel(const r128_surf_t *s, Bit32u addr, int sdt, int ddt, bool usepal);
  bool   r2d_stretch_resolve(const r128_surf_t *ssrc, int sbpp, int sdt, int ddt, bool usepal, bool blend,
                             int sx0, Bit32u xinc, int xskip, int col, Bit32u saddr, Bit32u sbddr,
                             Bit32u wy, int sxlim, Bit32u key, Bit32u kmask, int key_skip, Bit32u *out);
  void   r2d_stretch_rect(Bit32u soff, Bit32u spitch_px, int sx0, int sy0, Bit32u xinc, Bit32u yinc,
                          int dx, int dy, int w, int h, int sdt, bool blend);
  bool   r2d_scale_block(const Bit32u *b);
  void   r2d_mono_rect(int x, int y, int w, int h, const Bit8u *bits, Bit32u bitpitch, const Bit8u *pat8x8, const Bit8u *patcol);
  void   r2d_host_color_rect(int x, int y, int w, int h, const Bit8u *px, Bit32u avail);
  bool   r2d_brush_block(Bit32u g, const Bit32u *pl, Bit32u count, Bit32u *pp, const Bit8u **mono,
                         const Bit8u **col, bool *mono_la, Bit32u *linepat, bool *line_en);
  void   r2d_packet3(Bit32u hdr, const Bit32u *pl, Bit32u count);
  void   r2d_fill_solid(int x, int y, int w, int h, Bit32u color);
  Bit32u r2d_grad_px(int kx, int ky);
  void   r2d_fill_gradient(int x, int y, int w, int h);
  void   r2d_hostdata_paint(void);
  void   r2d_hostdata_word(Bit32u off, Bit32u val);
  void   r2d_gui_op_run(int w, int h);
  void   r2d_gui_op(int w, int h);
  Bit32u r2d_pack_rgb(int r, int g, int b);

  // ---- rage128_pm4.cc: PM4/CCE command processor ----
  bool   pm4_reg_read(Bit32u off, Bit32u *val);
  bool   pm4_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   pm4_reset(void);
  void   pm4_thread_init(void);
  void   pm4_thread_close(void);
  bool   pm4_bus_master_ok(void);
  bool   pm4_bus_read(Bit32u bus_addr, Bit32u *val);
  bool   pm4_bus_read_block(Bit32u vm, Bit8u *dst, Bit32u len);
  bool   pm4_bus_write(Bit32u bus_addr, Bit32u val);
  bool   pm4_bus_write_block(Bit32u vm, const Bit8u *src, Bit32u len);
  Bit32u pm4_vm_addr(Bit32u vm);
  Bit32u pm4_ring_mask(void);
  bool   pm4_cce_running(void);
  bool   pm4_ring_bm(void);
  bool   pm4_active(void);
  int    pm4_pump(void);
  void   pm4_kick(void);
  void   pm4_gui_reset(void);
  void   pm4_drain_wait(void);
  void   pm4_flip_notify(void);
  bool   pm4_enqueue_write(Bit32u off, Bit32u val);
  Bit32u pm4_splice_ib(Bit32u wr, Bit32u off, Bit32u n, Bit32u rptr);
  void   cce_fifo_reserve(Bit32u wr, Bit32u need);
  bool   cce_get(Bit32u *val);
  bool   cce_get_ib(Bit32u *val);
  void   cce_packet(Bit32u hdr);
  void   pm4_run_indirect(void);
  bool   pm4_reg_in_fetch_block(Bit32u reg);
  void   pm4_exec_packet3(Bit32u hdr, Bit32u *pl, Bit32u n);
  void   pm4_wait_until(Bit32u val);

  // ---- rage128_3d.cc: 3D engine ----
  bool   r3d_reg_read(Bit32u off, Bit32u *val);
  bool   r3d_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   r3d_reset(void);
  bool   r3d_packet3(Bit32u hdr, const Bit32u *pl, Bit32u count);
  void   r3d_draw_packet(const Bit32u *pl, Bit32u count);
  void   r3d_draw_inline(const Bit32u *pl, Bit32u count);
  void   r3d_buffer_draw(const Bit32u *pl, Bit32u count, Bit32u base, Bit32u fmt, Bit32u stride,
                         Bit32u prim, Bit32u num, bool indexed);
  bool   r3d_fetch_vertex(Bit32u base, Bit32u stride, Bit32u idx, Bit32u fmt, r3d_vtx_t *out);
  void   raster_state_capture(rage128_raster_state_t *rs);
  void   r3d_stage_textures(rage128_raster_state_t *rs);
  Bit32u r3d_stage_level(Bit32u key, Bit32u vm, Bit32u len);
  void   r3d_draw_state_derive(rage128_raster_state_t *rs);
  void   r3d_tri(const rage128_raster_state_t *rs, int thr_id, int thr_mask,
                 const r3d_vtx_t *a, const r3d_vtx_t *b, const r3d_vtx_t *c);
  void   r3d_line(const rage128_raster_state_t *rs, int thr_id, int thr_mask,
                  const r3d_vtx_t *a, const r3d_vtx_t *b);
  void   r3d_point(const rage128_raster_state_t *rs, int thr_id, int thr_mask, const r3d_vtx_t *v);
  int    r3d_texstage_run(r3d_texctx_t *tc, float w0, float w1, float w2, float *col);
  Bit32u r3d_tex_sample(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d,
                        float s, float t, float lod, int has_lod, Bit32u *nearest);
  Bit32u r3d_tex_level(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d, int slot,
                       float s, float t, int linear, Bit32u *nearest);
  void   r3d_stage_slot(const rage128_raster_state_t *rs, int st, r3d_stage_desc_t *d, int cslot);
  void   r3d_mark_dirty(Bit32u addr, Bit32u len);
  // raster batch / worker pool
  void   raster_init(void);
  void   raster_close(void);
  void   raster_flush(void);
  void   raster_abandon(void);
  void   raster_submit_tri(const rage128_raster_state_t *rs, const r3d_vtx_t *a, const r3d_vtx_t *b, const r3d_vtx_t *c);
  void   raster_submit_line(const rage128_raster_state_t *rs, const r3d_vtx_t *a, const r3d_vtx_t *b);
  void   raster_submit_point(const rage128_raster_state_t *rs, const r3d_vtx_t *v);
  void   rb_enqueue(const rage128_raster_state_t *rs, int kind, const r3d_vtx_t *v, int n);
  Bit32u rb_intern_state(const rage128_raster_state_t *rs);
  void   rb_run_parallel(void);
  void   rb_guard_rt(const rage128_raster_state_t *rs);
  void   rb_reset_written(void);
  void   shadow_id_advance(void);

  // ---- rage128_ov0.cc: video overlay + subpicture ----
  bool   ov0_reg_read(Bit32u off, Bit32u *val);
  bool   ov0_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   ov0_reset(void);
  void   ov0_apply(void);
  void   ov0_update(void);
  bool   subpic_reg_read(Bit32u off, Bit32u *val);
  bool   subpic_reg_write(Bit32u off, Bit32u val, Bit32u mask);
  void   ov0_frame_latch(void);
  void   draw_overlay(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info);
  bool   ov0_gfx_key_match(int x, int y);
  void   subpic_blend(int out_x, int out_row, int *y, int *cb, int *cr);

  // =====================================================================
  // State
  // =====================================================================
  Bit8u  devfunc;
  bool   is_agp;
  Bit16u pci_device_id;
  Bit16u pci_subsys_id;
  double ref_freq_hz;
  Bit32u vram_size;
  Bit32u vram_mask;
  int    render_threads;
  int    vline_timer_id;
  int    pump_timer_id;
  bool   vga_disabled;

  Bit32u mm_index;
  Bit32u bios_scratch[4];
  Bit32u bus_cntl;
  Bit32u bus_cntl1;
  Bit32u config_cntl;
  Bit32u gen_reset_cntl;
  Bit32u config_memsize;
  Bit32u test_debug_cntl;
  Bit32u test_debug_mux;
  Bit32u hw_debug;
  Bit32u host_path_cntl;
  Bit32u mem_cntl;
  Bit32u ext_mem_cntl;
  Bit32u mem_addr_config;
  Bit32u mem_intf_cntl;
  Bit32u mem_str_cntl;
  Bit32u mem_init_lat_timer;
  Bit32u mem_sdram_mode_reg;
  Bit32u pad_ctlr_strength;
  Bit32u pc_misc_ctl;
  Bit32u videomux_cntl;
  Bit32u surface_delay;
  Bit32u surf_lower[4];
  Bit32u surf_upper[4];
  Bit32u surf_info[4];
  bool   surf_xlate_on;
  Bit32u agp_cntl_b;
  Bit32u agp_base;
  Bit32u agp_cntl;
  Bit32u gui_debug0;
  Bit32u pc_gui_mode;
  Bit32u bm_chunk_val[2];
  Bit32u gpio_monid;
  Bit32u gen_int_cntl;
  Bit32u gen_int_status;
  volatile int gui_idle_event;
  volatile bool irq_dirty;
  volatile bool timing_dirty;
  // Last CRTC timing handed to the VGA core (a recalculation restarts the
  // core's vertical timer, so it is only done when the timing changed)
  bool   timing_valid;
  bx_crtc_params_t timing_last;
  Bit32u timing_vclock;
  // Geometry last reported to the GUI through dimension_update()
  unsigned gui_xres, gui_yres, gui_bpp;
  volatile bool palette_dirty;

  // Display block
  Bit32u pll_regs[RAGE128_PLL_REGS];
  Bit32u clock_cntl_index;
  bool   ppll_update_pending;
  Bit32u ppll_work[5];
  Bit8u  pll_test_count_base;
  Bit64u pll_test_zero_usec;
  double pll_test_acc;
  Bit32u crtc_gen_cntl;
  Bit32u crtc_ext_cntl;
  Bit32u bank_w[2];
  Bit32u bank_r[2];
  Bit32u dac_cntl;
  Bit8u  dac_mask_prog;
  Bit32u crtc_h_total_disp;
  Bit32u crtc_h_sync_strt_wid;
  Bit32u crtc_v_total_disp;
  Bit32u crtc_v_disp_active;
  Bit32u crtc_v_sync_strt_wid;
  Bit32u crtc_vline;
  Bit32u crtc_gui_trig_vline;
  Bit32u crtc_debug;
  Bit32u crtc_offset;
  Bit32u crtc_offset_latched;
  Bit32u crtc_offset_cntl;
  Bit32u crtc_pitch;
  Bit32u cur_offset;
  Bit32u cur_horz_vert_posn;
  Bit32u cur_horz_vert_off;
  Bit32u cur_offset_act;
  Bit32u cur_posn_act;
  Bit32u cur_hvoff_act;
  bool   cur_lock;
  Bit32u cur_clr0;
  Bit32u cur_clr1;
  // Cursor tuple as latched at the last vertical blank (what scanout
  // composites this frame) and the "bitmap rewritten in VRAM" flag
  bool   cur_lat_en;
  Bit32u cur_lat_offset;
  Bit32u cur_lat_posn;
  Bit32u cur_lat_hvoff;
  Bit32u cur_lat_clr0;
  Bit32u cur_lat_clr1;
  volatile bool cur_bitmap_dirty;
  Bit32u ovr_clr;
  Bit32u ovr_wid_left_right;
  Bit32u ovr_wid_top_bottom;
  Bit32u dac_ext_cntl;
  Bit32u dac_crc_sig;
  Bit32u dda_config;
  Bit32u dda_on_off;
  Bit32u vga_dda_config;
  Bit32u vga_dda_on_off;
  Bit32u mem_vga_wp_sel;
  Bit32u mem_vga_rp_sel;
  Bit32u palette_index;
  Bit32u frame_count;
  Bit32u snapshot_vh_counts;
  Bit32u snapshot_f_count;
  Bit32u n_vif_count;
  Bit32u snapshot_vif_cntl;
  bool   crtc_offset_pending;
  bool   crtc_offset_lock;
  bool   vblank_save;
  Bit8u  ext_crtc[0x40];   // extended CRTC index shadow (0x19-0x3f)

  // Derived scanout state
  bool     disp_ext;
  unsigned disp_xres, disp_yres, disp_bpp, disp_pitch;
  Bit32u   disp_base;
  bool     disp_dblscan;
  bool     disp_blank;
  bool     disp_dac_const;
  Bit32u   disp_dac_const_color;
  bool     needs_update_mode;
  bool     needs_update_tile;
  bool     needs_update_dispentire;
  bool     ext_last;

  // 2D GUI engine context
  Bit32u dp_gui_master_cntl;
  Bit32u dp_brush_frgd_clr;
  Bit32u dp_brush_bkgd_clr;
  Bit32u brush_yx;
  Bit32u brush_data[32];
  Bit32u dp_src_frgd_clr;
  Bit32u dp_src_bkgd_clr;
  Bit32u dp_cntl;
  Bit32u dp_datatype;
  Bit32u dp_mix;
  Bit32u dp_write_mask;
  Bit32u clr_cmp_clr_src;
  Bit32u clr_cmp_clr_dst;
  Bit32u clr_cmp_cntl;
  Bit32u clr_cmp_mask;
  Bit32u aux_sc_cntl;
  Bit32u aux_sc_rect[3][4];
  Bit32u default_offset;
  Bit32u default_pitch;
  Bit32u default_sc_bottom_right;
  Bit32u sc_top_left;
  Bit32u sc_bottom_right;
  Bit32u src_offset;
  Bit32u src_pitch;
  Bit32u dst_offset;
  Bit32u dst_pitch;
  Bit32u src_pitch_reg;
  Bit32u dst_pitch_reg;
  Bit32u src_sc_right;
  Bit32u src_sc_bottom;
  Bit32s gui_dst_x, gui_dst_y, gui_src_x, gui_src_y;
  Bit32u gui_dst_w, gui_dst_h;
  Bit32u bres_err, bres_inc, bres_dec, bres_lnth;
  Bit32u dp_cntl_line;
  Bit32u grad_start[3];
  Bit32s grad_slope_x[3];
  Bit32s grad_slope_y[3];
  bool   grad_valid;
  Bit32u gui_scratch[6];
  Bit32u scale_scr_height_width;
  Bit32u scl_palette[256];
  bool   hostdata_active;
  int    hostdata_x, hostdata_y, hostdata_w, hostdata_h;
  int    hostdata_srcdt;
  Bit32u hostdata_ndw;
  Bit32u hostdata_buf[1024];
  rage128_span_stage_t s2d_dst;
  rage128_span_stage_t s2d_src;

  // PM4 / CCE
  Bit32u pm4_buffer_offset;
  Bit32u pm4_buffer_cntl;
  Bit32u pm4_wm_cntl;
  Bit32u pm4_rptr_addr;
  Bit32u pm4_rptr;
  Bit32u pm4_wptr;
  Bit32u pm4_wptr_delay;
  Bit32u pm4_vc_debug_config;
  Bit32u pm4_microcode_addr;
  Bit32u pm4_micro_cntl;
  Bit32u pm4_iw_indoff;
  Bit32u pm4_iw_indsize;
  Bit32u pci_gart_page;
  bool   pm4_ind_busy;
  bool   pm4_ind_pending;
  Bit32u pump_frame_rem;
  Bit32u pump_ib_state;
  Bit32u pump_ib_addr;
  Bit32u *cce_fifo;
  Bit32u *cce_fifo_rptr;
  Bit8u  *cce_fifo_tag;
  volatile Bit32u cce_fifo_rd;
  volatile Bit32u cce_fifo_wr;
  volatile Bit32u cce_retire_rptr;
  Bit32u cce_retire_pending;
  Bit32u cce_shadow_last;
  Bit32u *cce_pl;
  Bit32u *ind_pl;
  volatile int cce_executing;
  volatile int cce_batch_pending;
  volatile int cce_drain_req;
  volatile int cce_abort;
  bool   cce_thread_started;
  BX_THREAD_VAR(cce_thread_var);
  bx_thread_sem_t cce_wake_sem;
  bx_thread_sem_t cce_idle_sem;
  bx_thread_sem_t cce_flip_sem;
  BX_MUTEX(cce_mutex);
  bool   on_cce_thread(void);
  bool   in_indirect;

  // 3D engine
  rage128_t3d_ctx_t t3d;
  Bit32u fog_table_wr_index;
  Bit32u tex_pal_wr_index;
  // texture staging arena for AGP-resident levels
  struct {
    Bit8u *arena;
    Bit32u cap;
    Bit32u used;
    Bit32u ent_count;
    struct {
      Bit32u vm_base, len, arena_off;
    } ent[256];
  } tex_stage;
  rage128_span_stage_t z_stage;
  rage128_span_stage_t c_stage;
  // vertex cache for indexed walks
  struct {
    r3d_vtx_t *v;
    Bit32u *gen;
    Bit32u cap;
    Bit32u cur;
  } vtx_cache;
  // parallel raster batch
  struct {
    int nthreads;
    int mask;
    rb_cmd_t *cmds;
    Bit32u cmd_count, cmd_cap;
    rage128_raster_state_t *states;
    Bit32u state_count, state_cap;
    bool   rt_valid;
    Bit32u rt_dst_offset, rt_dst_pitch, rt_dst_bpp, rt_dst_tiled;
    bool   rt_z_valid;
    Bit32u rt_z_offset, rt_z_stride, rt_z_tiled;
    bool   wr_valid;
    Bit32u wr_c_lo, wr_c_hi, wr_z_lo, wr_z_hi;
    rb_worker_t workers[RAGE128_RASTER_MAX_WORKERS];
  } rb;

  // OV0 overlay + subpicture
  struct {
    Bit32u shadow[64];
    Bit32u active[64];
  } ov0;
  Bit32u ov0_reg_load_cntl;
  bool   ov0_enabled;
  int    ov0_x1, ov0_y1, ov0_x2, ov0_y2;
  struct {
    Bit32u regs[19];
    Bit32u pal[16];
    Bit32u active[19];
    Bit32u apal[16];
    Bit32u frame_stamp;
  } subpic;

  // DDC / I2C
  bx_ddc_c ddc;
};

#endif // BX_SUPPORT_RAGE128

#endif // BX_IODEV_RAGE128_H
