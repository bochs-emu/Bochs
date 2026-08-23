/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025  The Bochs Project
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

#if BX_SUPPORT_GEFORCE

#if BX_USE_GEFORCE_SMF
#  define BX_GEFORCE_SMF  static
#  define BX_GEFORCE_THIS theSvga->
#  define BX_GEFORCE_THIS_PTR theSvga
#else
#  define BX_GEFORCE_SMF
#  define BX_GEFORCE_THIS this->
#  define BX_GEFORCE_THIS_PTR this
#endif // BX_USE_GEFORCE_SMF

// 0x3b4,0x3d4
#define VGA_CRTC_MAX 0x18
#define GEFORCE_CRTC_MAX 0xF0

#define GEFORCE_CHANNEL_COUNT 32
#define GEFORCE_SUBCHANNEL_COUNT 8
#define GEFORCE_CACHE1_SIZE 64

#define GEFORCE_CLASS_COUNT 0x10000
#define GEFORCE_METHOD_COUNT 0x800

#define BX_ROP_PATTERN 0x01

struct gf_texture
{
  Bit32u offset;
  Bit32u dma_obj;
  Bit32u format;
  bool cubemap;
  bool linear;
  bool unnormalized;
  bool compressed;
  bool dxt_alpha_data;
  bool dxt_alpha_explicit;
  Bit32u color_bytes;
  Bit32u levels;
  Bit32u base_size[3];
  Bit32u size[3];
  Bit32u face_bytes;
  Bit32u wrap[3];
  Bit32u control0;
  bool enabled;
  Bit32u control1;
  bool signed_any;
  bool signed_comp[4];
  Bit32u image_rect;
  Bit32u pal_dma_obj;
  Bit32u pal_ofs;
  Bit32u control3;
  Bit32u key_color;
  float offset_matrix[4];
};

struct gf_light
{
  float ambient_color[3];
  float diffuse_color[3];
  float specular_color[3];
  float inf_half_vector[3];
  float inf_direction[3];
  float spot_direction[4];
  float local_position[3];
  float local_attenuation[3];
};

struct gf_channel
{
  Bit32u subr_return;
  bool subr_active;
  struct {
    Bit32u mthd;
    Bit32u subc;
    Bit32u mcnt;
    bool ni;
  } dma_state;
  struct {
    Bit32u object;
    Bit8u engine;
    Bit32u notifier;
  } schs[GEFORCE_SUBCHANNEL_COUNT];

  bool notify_pending;
  Bit32u notify_type;

  bool s2d_locked;
  Bit32u s2d_img_src;
  Bit32u s2d_img_dst;
  Bit32u s2d_color_fmt;
  Bit32u s2d_color_bytes;
  Bit32u s2d_pitch_src;
  Bit32u s2d_pitch_dst;
  Bit32u s2d_ofs_src;
  Bit32u s2d_ofs_dst;

  Bit32u swzs_img_obj;
  Bit32u swzs_fmt;
  Bit32u swzs_color_bytes;
  Bit32u swzs_width;
  Bit32u swzs_height;
  Bit32u swzs_ofs;

  bool ifc_color_key_enable;
  bool ifc_clip_enable;
  Bit32u ifc_operation;
  Bit32u ifc_color_fmt;
  Bit32u ifc_color_bytes;
  Bit32u ifc_pixels_per_word;
  Bit32u ifc_x;
  Bit32u ifc_y;
  Bit32u ifc_ofs_x;
  Bit32u ifc_ofs_y;
  Bit32u ifc_draw_offset;
  Bit32u ifc_redraw_offset;
  Bit32u ifc_dst_width;
  Bit32u ifc_dst_height;
  Bit32u ifc_src_width;
  Bit32u ifc_src_height;
  Bit32u ifc_clip_x0;
  Bit32u ifc_clip_y0;
  Bit32u ifc_clip_x1;
  Bit32u ifc_clip_y1;

  Bit32u iifc_palette;
  Bit32u iifc_palette_ofs;
  Bit32u iifc_operation;
  Bit32u iifc_color_fmt;
  Bit32u iifc_color_bytes;
  Bit32u iifc_bpp4;
  Bit32u iifc_yx;
  Bit32u iifc_dhw;
  Bit32u iifc_shw;
  Bit32u iifc_words_ptr;
  Bit32u iifc_words_left;
  Bit32u* iifc_words;

  Bit32u sifc_operation;
  Bit32u sifc_color_fmt;
  Bit32u sifc_color_bytes;
  Bit32u sifc_shw;
  Bit32u sifc_dxds;
  Bit32u sifc_dydt;
  Bit32u sifc_clip_yx;
  Bit32u sifc_clip_hw;
  Bit32u sifc_syx;
  Bit32u sifc_words_ptr;
  Bit32u sifc_words_left;
  Bit32u* sifc_words;

  bool blit_color_key_enable;
  Bit32u blit_operation;
  Bit32u blit_syx;
  Bit32u blit_dyx;
  Bit32u blit_hw;

  bool tfc_swizzled;
  Bit32u tfc_color_fmt;
  Bit32u tfc_color_bytes;
  Bit32u tfc_yx;
  Bit32u tfc_hw;
  Bit32u tfc_clip_wx;
  Bit32u tfc_clip_hy;
  Bit32u tfc_words_ptr;
  Bit32u tfc_words_left;
  Bit32u* tfc_words;
  bool tfc_upload;
  Bit32u tfc_upload_offset;

  Bit32u sifm_src;
  bool sifm_swizzled;
  bool sifm_swizzled_0389;
  Bit32u sifm_operation;
  Bit32u sifm_color_fmt;
  Bit32u sifm_color_bytes;
  Bit32u sifm_syx;
  Bit32u sifm_dyx;
  Bit32u sifm_shw;
  Bit32u sifm_dhw;
  Bit32s sifm_dudx;
  Bit32s sifm_dvdy;
  Bit32u sifm_sfmt;
  Bit32u sifm_sofs;

  Bit32u m2mf_src;
  Bit32u m2mf_dst;
  Bit32u m2mf_src_offset;
  Bit32u m2mf_dst_offset;
  Bit32u m2mf_src_pitch;
  Bit32u m2mf_dst_pitch;
  Bit32u m2mf_line_length;
  Bit32u m2mf_line_count;
  Bit32u m2mf_format;
  Bit32u m2mf_buffer_notify;

  Bit32u d3d_a_obj;
  Bit32u d3d_b_obj;
  Bit32u d3d_color_obj;
  Bit32u d3d_zeta_obj;
  Bit32u d3d_vertex_a_obj;
  Bit32u d3d_vertex_b_obj;
  Bit32u d3d_report_obj;
  Bit32u d3d_clip_horizontal;
  Bit32u d3d_clip_vertical;
  Bit32u d3d_surface_format;
  Bit32u d3d_color_bytes;
  Bit32u d3d_depth_bytes;
  Bit32u d3d_surface_pitch_a;
  Bit32u d3d_surface_pitch_z;
  bool d3d_local_viewer;
  Bit32u d3d_color_material_emission;
  Bit32u d3d_color_material_ambient;
  Bit32u d3d_color_material_diffuse;
  Bit32u d3d_color_material_specular;
  Bit32u d3d_fog_mode;
  Bit32u d3d_fog_gen_mode;
  float d3d_fog_params[3];
  Bit32u d3d_fog_enable;
  float d3d_fog_color[4];
  Bit16s d3d_window_offset_x;
  Bit16s d3d_window_offset_y;
  Bit32u d3d_window_clip_x1[8];
  Bit32u d3d_window_clip_x2[8];
  Bit32u d3d_window_clip_y1[8];
  Bit32u d3d_window_clip_y2[8];
  Bit32u d3d_surface_color_offset;
  Bit32u d3d_surface_zeta_offset;
  Bit32u d3d_combiner_alpha_icw[8];
  Bit32u d3d_combiner_final[2];
  Bit32u d3d_alpha_test_enable;
  Bit32u d3d_alpha_func;
  Bit32u d3d_alpha_ref;
  Bit32u d3d_blend_enable;
  Bit16u d3d_blend_sfactor_rgb;
  Bit16u d3d_blend_sfactor_alpha;
  Bit16u d3d_blend_dfactor_rgb;
  Bit16u d3d_blend_dfactor_alpha;
  Bit16u d3d_blend_equation_rgb;
  Bit16u d3d_blend_equation_alpha;
  float d3d_blend_color[4];
  Bit32u d3d_cull_face_enable;
  Bit32u d3d_depth_test_enable;
  Bit32u d3d_depth_write_enable;
  Bit32u d3d_stencil_mask;
  Bit32u d3d_stencil_func;
  Bit32u d3d_stencil_func_ref;
  Bit32u d3d_stencil_func_mask;
  Bit32u d3d_stencil_op_sfail;
  Bit32u d3d_stencil_op_dpfail;
  Bit32u d3d_stencil_op_dppass;
  Bit32u d3d_lighting_enable;
  Bit32u d3d_stencil_test_enable;
  Bit32u d3d_depth_func;
  Bit32u d3d_color_mask;
  Bit32u d3d_color_mask_565;
  Bit32u d3d_color_mask_8888;
  Bit32u d3d_shade_mode;
  float d3d_clip_min;
  float d3d_clip_max;
  Bit32u d3d_cull_face;
  Bit32u d3d_front_face;
  Bit32u d3d_normalize_enable;
  float d3d_material_factor[4];
  Bit32u d3d_separate_specular;
  Bit32u d3d_light_enable_mask;
  Bit32u d3d_texgen[8][4];
  Bit32u d3d_texture_matrix_enable[16];
  Bit32u d3d_view_matrix_enable;
  float d3d_model_view_matrix[2][16];
  float d3d_inverse_model_view_matrix[12];
  float d3d_composite_matrix[16];
  float d3d_texture_matrix[8][16];
  float d3d_texgen_plane[8][4][4];
  Bit32u d3d_scissor_x;
  Bit32u d3d_scissor_width;
  Bit32u d3d_scissor_y;
  Bit32u d3d_scissor_height;
  Bit32u d3d_shader_program;
  Bit32u d3d_shader_obj;
  Bit32u d3d_shader_offset;
  float d3d_specular_params[6];
  float d3d_specular_power;
  float d3d_scene_ambient_color[4];
  Bit32u d3d_viewport_x;
  Bit32u d3d_viewport_width;
  Bit32u d3d_viewport_y;
  Bit32u d3d_viewport_height;
  float d3d_viewport_offset[4];
  float d3d_eye_position[4];
  float d3d_combiner_const_color[8][2][4];
  Bit32u d3d_combiner_alpha_ocw[8];
  Bit32u d3d_combiner_color_icw[8];
  float d3d_viewport_scale[4];
  Bit32u d3d_transform_program[544][4];
  float d3d_transform_constant[512][4];
  gf_light d3d_light[8];
  Bit32u d3d_attrib_count;
  Bit32u d3d_vertex_data_base_index;
  Bit32u d3d_vertex_data_array_offset[16];
  Bit32u d3d_vertex_data_array_format_type[16];
  Bit32u d3d_vertex_data_array_format_size[16];
  Bit32u d3d_vertex_data_array_format_stride[16];
  bool d3d_vertex_data_array_format_dx[16];
  bool d3d_vertex_data_array_format_homogeneous[16];
  Bit32u d3d_begin_end;
  bool d3d_primitive_done;
  bool d3d_triangle_flip;
  Bit32u d3d_vertex_index;
  Bit32u d3d_attrib_index;
  Bit32u d3d_comp_index;
  float d3d_vertex_data[4][16][4];
  float d3d_vertex_data_imm[16][4];
  Bit32u d3d_index_array_offset;
  bool d3d_index_array_dma;
  bool d3d_index_array_type_16;
  gf_texture d3d_texture[16];
  Bit32u d3d_shader_control;
  Bit32u d3d_semaphore_obj;
  Bit32u d3d_semaphore_offset;
  Bit32u d3d_zstencil_clear_value;
  Bit32u d3d_color_clear_value;
  Bit32u d3d_clear_surface;
  Bit32u d3d_combiner_color_ocw[8];
  Bit32u d3d_combiner_control;
  Bit32u d3d_combiner_control_num_stages;
  Bit32u d3d_tex_shader_op[4];
  Bit32u d3d_tex_shader_dotmapping[4];
  Bit32u d3d_tex_shader_previous[4];
  Bit32u d3d_transform_execution_mode;
  Bit32u d3d_transform_program_load;
  Bit32u d3d_transform_program_start;
  Bit32u d3d_transform_constant_load;
  Bit32u d3d_attrib_in_normal;
  Bit32u d3d_attrib_in_color[2];
  Bit32u d3d_attrib_out_color[2];
  Bit32u d3d_attrib_out_fogc;
  Bit32u d3d_attrib_in_tex_coord[16];
  Bit32u d3d_attrib_out_tex_coord[16];
  bool d3d_attrib_out_enable[32];
  Bit32u d3d_vs_temp_regs_count;
  Bit32u d3d_tex_coord_count;

  Bit8u  rop;

  Bit32u beta;

  Bit16u clip_x;
  Bit16u clip_y;
  Bit16u clip_width;
  Bit16u clip_height;

  Bit32u chroma_color_fmt;
  Bit32u chroma_color;

  Bit32u patt_shape;
  bool patt_type_color;
  Bit32u patt_bg_color;
  Bit32u patt_fg_color;
  bool patt_data_mono[64];
  Bit32u patt_data_color[64];

  Bit32u gdi_operation;
  Bit32u gdi_color_fmt;
  Bit32u gdi_mono_fmt;
  Bit32u gdi_clip_yx0;
  Bit32u gdi_clip_yx1;
  Bit32u gdi_rect_color;
  Bit32u gdi_rect_xy;
  Bit32u gdi_rect_yx0;
  Bit32u gdi_rect_yx1;
  Bit32u gdi_rect_wh;
  Bit32u gdi_bg_color;
  Bit32u gdi_fg_color;
  Bit32u gdi_image_swh;
  Bit32u gdi_image_dwh;
  Bit32u gdi_image_xy;
  Bit32u gdi_words_ptr;
  Bit32u gdi_words_left;
  Bit32u* gdi_words;

  Bit32u rect_operation;
  Bit32u rect_color_fmt;
  Bit32u rect_color;
  Bit32u rect_yx;
  Bit32u rect_hw;
};

class bx_geforce_c : public bx_vgacore_c
{
public:
  bx_geforce_c();
  virtual ~bx_geforce_c();

  virtual bool init_vga_extension(void);
  Bit16u get_crtc_vtotal();
  virtual void get_crtc_params(bx_crtc_params_t* crtcp, Bit32u* vclock);
  virtual void reset(unsigned type);
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height);
  // doubled coordinates in low resolution modes
  void redraw_area_d(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height);
  // not doubled coordinates in low resolution modes
  void redraw_area_nd(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height);
  void redraw_area_nd(Bit32u offset, Bit32u width, Bit32u height);
  virtual Bit8u mem_read(bx_phy_address addr);
  virtual void mem_write(bx_phy_address addr, Bit8u value);
  virtual void get_text_snapshot(Bit8u **text_snapshot,
                                 unsigned *txHeight, unsigned *txWidth);
  virtual void register_state(void);
  virtual void after_restore_state(void);

#if BX_SUPPORT_PCI
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#endif
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

protected:
  virtual void update(void);

private:
#if BX_USE_GEFORCE_SMF
  typedef void (*gf_method_handler)(
    gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
#else
  typedef void (bx_geforce_c::*gf_method_handler)(
    gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
#endif

  static Bit32u svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_GEFORCE_SMF
  Bit32u svga_read(Bit32u address, unsigned io_len);
  void   svga_write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_GEFORCE_SMF void   svga_init_members();
  BX_GEFORCE_SMF void   bitblt_init();
  BX_GEFORCE_SMF void   init_method_handlers();
  BX_GEFORCE_SMF void   set_method_handler(Bit32u cls, Bit32u method, gf_method_handler handler);
  BX_GEFORCE_SMF void   set_method_handler(Bit32u cls, Bit32u method_start, Bit32u method_end, gf_method_handler handler);
  BX_GEFORCE_SMF void   set_d3d_method_handler(Bit32u cl0096, Bit32u cl0097,
    Bit32u cl0497, Bit32u cl4097, Bit32u method_start, Bit32u method_end, gf_method_handler handler);
  BX_GEFORCE_SMF void   set_d3d_method_handler(Bit32u cl0096, Bit32u cl0097,
    Bit32u cl0497, Bit32u cl4097, Bit32u method, gf_method_handler handler);

  BX_GEFORCE_SMF Bit16u cursor_read16(Bit32u address);
  BX_GEFORCE_SMF Bit32u cursor_read32(Bit32u address);
  BX_GEFORCE_SMF void draw_hardware_cursor(unsigned, unsigned, bx_svga_tileinfo_t *);

  // 0x3b4-0x3b5,0x3d4-0x3d5
  BX_GEFORCE_SMF Bit8u svga_read_crtc(Bit32u address, unsigned index);
  BX_GEFORCE_SMF void  svga_write_crtc(Bit32u address, unsigned index, Bit8u value);

  BX_GEFORCE_SMF void set_irq_level(bool level);
  BX_GEFORCE_SMF Bit32u get_mc_intr();
  BX_GEFORCE_SMF void update_irq_level();

  BX_GEFORCE_SMF Bit8u register_read8(Bit32u address);
  BX_GEFORCE_SMF void  register_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF Bit32u register_read32(Bit32u address);
  BX_GEFORCE_SMF void  register_write32(Bit32u address, Bit32u value);

  BX_GEFORCE_SMF Bit8u vram_read8(Bit32u address);
  BX_GEFORCE_SMF Bit16u vram_read16(Bit32u address);
  BX_GEFORCE_SMF Bit32u vram_read32(Bit32u address);
  BX_GEFORCE_SMF Bit64u vram_read64(Bit32u address);
  BX_GEFORCE_SMF void vram_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void vram_write16(Bit32u address, Bit16u value);
  BX_GEFORCE_SMF void vram_write32(Bit32u address, Bit32u value);
  BX_GEFORCE_SMF void vram_write64(Bit32u address, Bit64u value);
  BX_GEFORCE_SMF Bit8u ramin_read8(Bit32u address);
  BX_GEFORCE_SMF Bit16u ramin_read16(Bit32u address);
  BX_GEFORCE_SMF Bit32u ramin_read32(Bit32u address);
  BX_GEFORCE_SMF void ramin_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void ramin_write32(Bit32u address, Bit32u value);
  BX_GEFORCE_SMF Bit8u physical_read8(Bit32u address);
  BX_GEFORCE_SMF Bit16u physical_read16(Bit32u address);
  BX_GEFORCE_SMF Bit32u physical_read32(Bit32u address);
  BX_GEFORCE_SMF Bit64u physical_read64(Bit32u address);
  BX_GEFORCE_SMF void physical_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void physical_write16(Bit32u address, Bit16u value);
  BX_GEFORCE_SMF void physical_write32(Bit32u address, Bit32u value);
  BX_GEFORCE_SMF void physical_write64(Bit32u address, Bit64u value);
  BX_GEFORCE_SMF Bit8u dma_read8(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit16u dma_read16(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit32u dma_read32(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit64u dma_read64(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF void dma_write8(Bit32u object, Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void dma_write16(Bit32u object, Bit32u address, Bit16u value);
  BX_GEFORCE_SMF void dma_write32(Bit32u object, Bit32u address, Bit32u value);
  BX_GEFORCE_SMF void dma_write64(Bit32u object, Bit32u address, Bit64u value);
  BX_GEFORCE_SMF void dma_copy(Bit32u dst_obj, Bit32u dst_addr,
    Bit32u src_obj, Bit32u src_addr, Bit32u byte_count);
  BX_GEFORCE_SMF Bit32u dma_pt_lookup(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit32u dma_lin_lookup(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit32u ramfc_address(Bit32u chid, Bit32u offset);
  BX_GEFORCE_SMF void ramfc_write32(Bit32u chid, Bit32u offset, Bit32u value);
  BX_GEFORCE_SMF Bit32u ramfc_read32(Bit32u chid, Bit32u offset);

  BX_GEFORCE_SMF Bit64u get_current_time();

  BX_GEFORCE_SMF void ramht_lookup(Bit32u handle, Bit32u chid, Bit32u* object, Bit8u* engine);

  BX_GEFORCE_SMF void update_fifo_wait();
  BX_GEFORCE_SMF void fifo_process();
  BX_GEFORCE_SMF void fifo_process(Bit32u chid);
  BX_GEFORCE_SMF int execute_command(Bit32u chid, Bit32u subc, Bit32u method, Bit32u param);

  BX_GEFORCE_SMF void update_color_bytes_s2d(gf_channel* ch);
  BX_GEFORCE_SMF void update_color_bytes_ifc(gf_channel* ch);
  BX_GEFORCE_SMF void update_color_bytes_sifc(gf_channel* ch);
  BX_GEFORCE_SMF void update_color_bytes_tfc(gf_channel* ch);
  BX_GEFORCE_SMF void update_color_bytes_iifc(gf_channel* ch);
  BX_GEFORCE_SMF void update_color_bytes(Bit32u s2d_color_fmt, Bit32u color_fmt, Bit32u* color_bytes);

  BX_GEFORCE_SMF void execute_clip(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_m2mf(gf_channel* ch, Bit32u subc, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_rop(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_patt(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_gdi(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_swzsurf(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_chroma(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_rect(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_imageblit(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_ifc(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_surf2d(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_iifc(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_sifc(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_beta(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_tfc(gf_channel* ch, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void execute_sifm(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);

  BX_GEFORCE_SMF void d3d_mh_object(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_flip_read(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_flip_write(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_flip_modulo(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_flip_incr(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fifo_wait(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_a_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_b_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_vertex_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_color_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_zeta_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_vertex_a_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_vertex_b_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_semaphore_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_report_obj(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_clip_horizontal(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_clip_vertical(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_surface_format(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_surface_pitch_a(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_surface_color_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_surface_zeta_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_surface_pitch_z(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_alpha_icw(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_final(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_0a5(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_0a6(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fog_mode(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fog_gen_mode(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fog_params(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fog_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_fog_color(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_window_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_window_clip(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_alpha_test_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_alpha_func(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_alpha_ref(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_cull_face_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_depth_test_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_lighting_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_test_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_sfactor_0096(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_dfactor_0096(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_equation_0096(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_sfactor_0497(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_dfactor_0497(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_equation_0497(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_blend_color(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_depth_func(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_color_mask(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_depth_write_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_mask(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_func(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_func_ref(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_func_mask(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_0dc(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_op_dpfail(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_stencil_op_dppass(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_shade_mode(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_clip_min(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_clip_max(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_cull_face(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_front_face(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_normalize_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_material_factor(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_separate_specular(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_light_enable_mask(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texgen(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texture_matrix_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_view_matrix_enable(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_model_view_matrix(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_inverse_model_view_matrix(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_composite_matrix(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texture_matrix(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texgen_plane(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_scissor_x_width(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_scissor_y_height(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_shader_program(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0497_240(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_viewport_x_width(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_viewport_y_height(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_specular_params(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_scene_ambient_color(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_viewport_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_eye_position(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_09c(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_298(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_alpha_ocw(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_color_icw(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texture_key_color(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_viewport_scale(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_program(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_constant(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_light(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_300(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0497_540(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_306(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_30c(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_314(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_318(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0096_31b(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texcoord(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_5c8(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_vertex_data_base_index(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_vertex_data_array_format(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_get_report(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_begin_end(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_array_element16(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_array_element32(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_draw_arrays(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_inline_array(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_index_array_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_index_array_dma(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0497_609(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_60a(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0497_610(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_620(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_640(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_650(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_0097_680(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_texture(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_shader_control(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_semaphore_offset(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_75c(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_75d(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_zstencil_clear_value(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_color_clear_value(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_clear_surface(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_color_ocw(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_combiner_control(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_tex_shader_op(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_tex_shader_dotmapping(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_tex_shader_previous(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_execution_mode(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_program_load(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_program_start(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_transform_constant_load(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_4097_7f1(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_4097_7f2(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_4097_7f3(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void d3d_mh_4097_7fd(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF void empty_method_handler(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);

  BX_GEFORCE_SMF void execute_d3d(gf_channel* ch, Bit32u cls, Bit32u method, Bit32u param);

  BX_GEFORCE_SMF Bit32u get_pixel(Bit32u obj, Bit32u ofs, Bit32u x, Bit32u cb);
  BX_GEFORCE_SMF void put_pixel(gf_channel* ch, Bit32u ofs, Bit32u x, Bit32u value);
  BX_GEFORCE_SMF void put_pixel_swzs(gf_channel* ch, Bit32u ofs, Bit32u value);
  BX_GEFORCE_SMF void pixel_operation(gf_channel* ch, Bit32u op,
    Bit32u* dstcolor, const Bit32u* srccolor, Bit32u cb, Bit32u px, Bit32u py);

  BX_GEFORCE_SMF void gdi_fillrect(gf_channel* ch, bool clipped);
  BX_GEFORCE_SMF void gdi_blit(gf_channel* ch, Bit32u type);
  BX_GEFORCE_SMF void rect(gf_channel* ch);
  BX_GEFORCE_SMF void ifc(gf_channel* ch, Bit32u word);
  BX_GEFORCE_SMF void iifc(gf_channel* ch);
  BX_GEFORCE_SMF void sifc(gf_channel* ch);
  BX_GEFORCE_SMF void copyarea(gf_channel* ch);
  BX_GEFORCE_SMF void tfc(gf_channel* ch);
  BX_GEFORCE_SMF void m2mf(gf_channel* ch);
  BX_GEFORCE_SMF void sifm(gf_channel* ch, bool swizzled);

  BX_GEFORCE_SMF bool d3d_scissor_clip(gf_channel* ch, Bit32u* x, Bit32u* y, Bit32u* width, Bit32u* height);
  BX_GEFORCE_SMF bool d3d_viewport_clip(gf_channel* ch, Bit32u* x, Bit32u* y, Bit32u* width, Bit32u* height);
  BX_GEFORCE_SMF bool d3d_window_clip(gf_channel* ch, Bit32u* x, Bit32u* y, Bit32u* width, Bit32u* height);
  BX_GEFORCE_SMF void d3d_clear_surface(gf_channel* ch);
  BX_GEFORCE_SMF void d3d_texture_process_format(gf_texture* tex);
  BX_GEFORCE_SMF void d3d_sample_texture(gf_channel* ch,
    gf_texture* tex, float coords_in[3], float color[4]);
  BX_GEFORCE_SMF void d3d_vertex_shader(gf_channel* ch, float in[16][4], float out[16][4]);
  BX_GEFORCE_SMF void d3d_register_combiners(gf_channel* ch, float regs[16][4], float out[4]);
  BX_GEFORCE_SMF bool d3d_pixel_shader(gf_channel* ch, float in[16][4], float tmp_regs16[64][4], float tmp_regs32[64][4]);
  BX_GEFORCE_SMF void d3d_triangle(gf_channel* ch, Bit32u base);
  BX_GEFORCE_SMF void d3d_triangle_clipped(gf_channel* ch, float v0[16][4], float v1[16][4], float v2[16][4]);
  BX_GEFORCE_SMF void d3d_clip_to_screen(gf_channel* ch, float pos_clip[4], float pos_screen[4]);
  BX_GEFORCE_SMF void d3d_process_vertex(gf_channel* ch, bool immediate);
  BX_GEFORCE_SMF void d3d_load_vertex(gf_channel* ch, Bit32u index);
  BX_GEFORCE_SMF Bit32u d3d_get_surface_pitch_z(gf_channel* ch);

  struct {
    Bit8u index;
    Bit8u reg[GEFORCE_CRTC_MAX+1];
  } crtc; // 0x3b4-5/0x3d4-5

  bool mc_soft_intr;
  Bit32u mc_intr_en;
  Bit32u mc_enable;
  Bit32u bus_intr;
  Bit32u bus_intr_en;
  bool fifo_wait;
  bool fifo_wait_soft;
  bool fifo_wait_notify;
  bool fifo_wait_flip;
  bool fifo_wait_acquire;
  Bit32u fifo_intr;
  Bit32u fifo_intr_en;
  Bit32u fifo_ramht;
  Bit32u fifo_ramfc;
  Bit32u fifo_ramro;
  Bit32u fifo_mode;
  Bit32u fifo_cache1_push0;
  Bit32u fifo_cache1_push1;
  Bit32u fifo_cache1_put;
  Bit32u fifo_cache1_dma_push;
  Bit32u fifo_cache1_dma_instance;
  Bit32u fifo_cache1_dma_put;
  Bit32u fifo_cache1_dma_get;
  Bit32u fifo_cache1_ref_cnt;
  Bit32u fifo_cache1_pull0;
  Bit32u fifo_cache1_semaphore;
  Bit32u fifo_cache1_get;
  Bit32u fifo_grctx_instance;
  Bit32u fifo_cache1_method[GEFORCE_CACHE1_SIZE];
  Bit32u fifo_cache1_data[GEFORCE_CACHE1_SIZE];
  Bit32u rma_addr;
  Bit32u timer_intr;
  Bit32u timer_intr_en;
  Bit32u timer_num;
  Bit32u timer_den;
  Bit64u timer_inittime1;
  Bit64u timer_inittime2;
  Bit32u timer_alarm;
  Bit32u straps0_primary;
  Bit32u straps0_primary_original;
  Bit32u graph_intr;
  Bit32u graph_nsource;
  Bit32u graph_intr_en;
  Bit32u graph_ctx_switch1;
  Bit32u graph_ctx_switch2;
  Bit32u graph_ctx_switch4;
  Bit32u graph_ctxctl_cur;
  Bit32u graph_status;
  Bit32u graph_trapped_addr;
  Bit32u graph_trapped_data;
  Bit32u graph_flip_read;
  Bit32u graph_flip_write;
  Bit32u graph_flip_modulo;
  Bit32u graph_notify;
  Bit32u graph_fifo;
  Bit32u graph_bpixel;
  Bit32u graph_channel_ctx_table;
  Bit32u graph_offset0;
  Bit32u graph_pitch0;
  Bit32u crtc_intr;
  Bit32u crtc_intr_en;
  Bit32u crtc_start;
  Bit32u crtc_config;
  Bit32u crtc_raster_pos;
  Bit32u crtc_cursor_offset;
  Bit32u crtc_cursor_config;
  Bit32u crtc_gpio_ext;
  Bit32u ramdac_cu_start_pos;
  Bit32u ramdac_vpll;
  Bit32u ramdac_vpll_b;
  Bit32u ramdac_pll_select;
  Bit32u ramdac_general_control;

  gf_method_handler empty_method_handlers[GEFORCE_METHOD_COUNT];
  gf_method_handler cl0096_method_handlers[GEFORCE_METHOD_COUNT];
  gf_method_handler cl0097_method_handlers[GEFORCE_METHOD_COUNT];
  gf_method_handler cl0497_method_handlers[GEFORCE_METHOD_COUNT];
  gf_method_handler cl4097_method_handlers[GEFORCE_METHOD_COUNT];
  gf_method_handler* class_method_handlers[GEFORCE_CLASS_COUNT];
  bx_bitblt_rop_t rop_handler[0x100];
  Bit8u rop_flags[0x100];

  gf_channel chs[GEFORCE_CHANNEL_COUNT];

  Bit32u unk_regs[4*1024*1024]; // temporary

  bool svga_unlock_special;
  bool svga_needs_update_tile;
  bool svga_needs_update_dispentire;
  bool svga_needs_update_mode;
  bool svga_double_width;

  Bit8u devfunc;
  unsigned svga_xres;
  unsigned svga_yres;
  unsigned svga_pitch;
  unsigned svga_bpp;
  unsigned svga_dispbpp;

  Bit32u card_type;
  Bit32u memsize_mask;
  Bit32u bar2_size;
  Bit32u ramin_flip;
  Bit32u class_mask;

  Bit8u *disp_ptr;
  Bit32u disp_offset;
  Bit32u disp_end_offset;
  Bit32u bank_base[2];

  struct {
    bool vram;
    Bit32u offset;
    Bit16s x, y;
    Bit8u size;
    bool bpp32;
    bool enabled;
  } hw_cursor;

  bx_ddc_c ddc;

  bool is_unlocked() { return svga_unlock_special; }

  BX_GEFORCE_SMF void svga_init_pcihandlers(void);

  void vertical_timer();

  static bool geforce_mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool geforce_mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
};

#endif // BX_SUPPORT_GEFORCE
