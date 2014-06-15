/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004 Makoto Suzuki (suzu)
//                     Volker Ruppert (vruppert)
//                     Robin Kay (komadori)
//  Copyright (C) 2004-2013  The Bochs Project
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

#if BX_SUPPORT_CLGD54XX

#if BX_USE_CIRRUS_SMF
#  define BX_CIRRUS_SMF  static
#  define BX_CIRRUS_THIS theSvga->
#  define BX_CIRRUS_THIS_PTR theSvga
#else
#  define BX_CIRRUS_SMF
#  define BX_CIRRUS_THIS this->
#  define BX_CIRRUS_THIS_PTR this
#endif // BX_USE_CIRRUS_SMF

// 0x3b4,0x3d4
#define VGA_CRTC_MAX 0x18
#define CIRRUS_CRTC_MAX 0x27
// 0x3c4
#define VGA_SEQENCER_MAX 0x04
#define CIRRUS_SEQENCER_MAX 0x1f
// 0x3ce
#define VGA_CONTROL_MAX 0x08
#define CIRRUS_CONTROL_MAX 0x39

// Size of internal cache memory for bitblt. (must be >= 256 and 4-byte aligned)
#define CIRRUS_BLT_CACHESIZE (2048 * 4)

#if BX_SUPPORT_PCI
#define CIRRUS_VIDEO_MEMORY_MB    4
#else
#define CIRRUS_VIDEO_MEMORY_MB    2
#endif

#define CIRRUS_VIDEO_MEMORY_KB    (CIRRUS_VIDEO_MEMORY_MB * 1024)
#define CIRRUS_VIDEO_MEMORY_BYTES (CIRRUS_VIDEO_MEMORY_KB * 1024)

typedef void (*bx_cirrus_bitblt_rop_t)(
    Bit8u *dst,const Bit8u *src,
    int dstpitch,int srcpitch,
    int bltwidth,int bltheight);

class bx_svga_cirrus_c : public bx_vgacore_c
{
public:
  bx_svga_cirrus_c();
  virtual ~bx_svga_cirrus_c();

  virtual void init_vga_extension(void);
  virtual void reset(unsigned type);
  virtual void refresh_display(void *this_ptr, bx_bool redraw);
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height);
  virtual Bit8u mem_read(bx_phy_address addr);
  virtual void mem_write(bx_phy_address addr, Bit8u value);
  virtual void get_text_snapshot(Bit8u **text_snapshot,
                                 unsigned *txHeight, unsigned *txWidth);
  virtual void register_state(void);
  virtual void after_restore_state(void);

#if BX_SUPPORT_PCI
  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#endif
#if BX_DEBUGGER
  virtual void   debug_dump(int argc, char **argv);
#endif

private:
  static Bit32u svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_CIRRUS_SMF
  Bit32u svga_read(Bit32u address, unsigned io_len);
  void   svga_write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_CIRRUS_SMF void mem_write_mode4and5_8bpp(Bit8u mode, Bit32u offset, Bit8u value);
  BX_CIRRUS_SMF void mem_write_mode4and5_16bpp(Bit8u mode, Bit32u offset, Bit8u value);

  static void   svga_timer_handler(void *);
  static Bit64s svga_param_handler(bx_param_c *param, int set, Bit64s val);
  BX_CIRRUS_SMF void   svga_timer(void);
  BX_CIRRUS_SMF void   svga_modeupdate(void);
  BX_CIRRUS_SMF void   svga_update(void);

  BX_CIRRUS_SMF void   svga_init_members();

  BX_CIRRUS_SMF void draw_hardware_cursor(unsigned, unsigned, bx_svga_tileinfo_t *);

  // bank memory
  BX_CIRRUS_SMF void update_bank_ptr(Bit8u bank_index);
  // 0x3b4-0x3b5,0x3d4-0x3d5
  BX_CIRRUS_SMF Bit8u svga_read_crtc(Bit32u address, unsigned index);
  BX_CIRRUS_SMF void  svga_write_crtc(Bit32u address, unsigned index, Bit8u value);
  // 0x3c4-0x3c5
  BX_CIRRUS_SMF Bit8u svga_read_sequencer(Bit32u address, unsigned index);
  BX_CIRRUS_SMF void  svga_write_sequencer(Bit32u address, unsigned index, Bit8u value);
  // 0x3ce-0x3cf
  BX_CIRRUS_SMF Bit8u svga_read_control(Bit32u address, unsigned index);
  BX_CIRRUS_SMF void  svga_write_control(Bit32u address, unsigned index, Bit8u value);
  // memory-mapped I/O
  BX_CIRRUS_SMF Bit8u svga_mmio_vga_read(Bit32u address);
  BX_CIRRUS_SMF void  svga_mmio_vga_write(Bit32u address,Bit8u value);
  BX_CIRRUS_SMF Bit8u svga_mmio_blt_read(Bit32u address);
  BX_CIRRUS_SMF void  svga_mmio_blt_write(Bit32u address,Bit8u value);

  BX_CIRRUS_SMF void  svga_reset_bitblt(void);
  BX_CIRRUS_SMF void  svga_bitblt();

  BX_CIRRUS_SMF void  svga_colorexpand(Bit8u *dst,const Bit8u *src,int count,int pixelwidth);

#if BX_USE_CIRRUS_SMF
  #define svga_colorexpand_8_static svga_colorexpand_8
  #define svga_colorexpand_16_static svga_colorexpand_16
  #define svga_colorexpand_24_static svga_colorexpand_24
  #define svga_colorexpand_32_static svga_colorexpand_32
#else
  static void svga_colorexpand_8_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count);
  static void svga_colorexpand_16_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count);
  static void svga_colorexpand_24_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count);
  static void svga_colorexpand_32_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count);
#endif

  BX_CIRRUS_SMF void svga_colorexpand_8(Bit8u *dst,const Bit8u *src,int count);
  BX_CIRRUS_SMF void svga_colorexpand_16(Bit8u *dst,const Bit8u *src,int count);
  BX_CIRRUS_SMF void svga_colorexpand_24(Bit8u *dst,const Bit8u *src,int count);
  BX_CIRRUS_SMF void svga_colorexpand_32(Bit8u *dst,const Bit8u *src,int count);

  BX_CIRRUS_SMF void svga_setup_bitblt_cputovideo(Bit32u dstaddr,Bit32u srcaddr);
  BX_CIRRUS_SMF void svga_setup_bitblt_videotocpu(Bit32u dstaddr,Bit32u srcaddr);
  BX_CIRRUS_SMF void svga_setup_bitblt_videotovideo(Bit32u dstaddr,Bit32u srcaddr);

#if BX_USE_CIRRUS_SMF == 0
  static void svga_patterncopy_static(void *this_ptr);
  static void svga_simplebitblt_static(void *this_ptr);
  static void svga_solidfill_static(void *this_ptr);
  static void svga_patterncopy_memsrc_static(void *this_ptr);
  static void svga_simplebitblt_memsrc_static(void *this_ptr);
  static void svga_colorexpand_transp_memsrc_static(void *this_ptr);
#else
  #define svga_patterncopy_static svga_patterncopy
  #define svga_simplebitblt_static svga_simplebitblt
  #define svga_solidfill_static svga_solidfill
  #define svga_patterncopy_memsrc_static svga_patterncopy_memsrc
  #define svga_simplebitblt_memsrc_static svga_simplebitblt_memsrc
  #define svga_colorexpand_transp_memsrc_static svga_colorexpand_transp_memsrc
#endif

  BX_CIRRUS_SMF void svga_patterncopy();
  BX_CIRRUS_SMF void svga_simplebitblt();
  BX_CIRRUS_SMF void svga_solidfill();
  BX_CIRRUS_SMF void svga_patterncopy_memsrc();
  BX_CIRRUS_SMF void svga_simplebitblt_memsrc();
  BX_CIRRUS_SMF void svga_colorexpand_transp_memsrc();

  BX_CIRRUS_SMF bx_bool svga_asyncbitblt_next();
  BX_CIRRUS_SMF bx_cirrus_bitblt_rop_t svga_get_fwd_rop_handler(Bit8u rop);
  BX_CIRRUS_SMF bx_cirrus_bitblt_rop_t svga_get_bkwd_rop_handler(Bit8u rop);

  struct {
    Bit8u index;
    Bit8u reg[CIRRUS_CRTC_MAX+1];
  } crtc; // 0x3b4-5/0x3d4-5
  struct {
    Bit8u index;
    Bit8u reg[CIRRUS_SEQENCER_MAX+1];
  } sequencer; // 0x3c4-5
  struct {
    Bit8u index;
    Bit8u reg[CIRRUS_CONTROL_MAX+1];
    Bit8u shadow_reg0;
    Bit8u shadow_reg1;
  } control; // 0x3ce-f
  struct {
    unsigned lockindex;
    Bit8u data;
    Bit8u palette[48];
  } hidden_dac; // 0x3c6

  bx_bool svga_unlock_special;
  bx_bool svga_needs_update_tile;
  bx_bool svga_needs_update_dispentire;
  bx_bool svga_needs_update_mode;

  unsigned svga_xres;
  unsigned svga_yres;
  unsigned svga_pitch;
  unsigned svga_bpp;
  unsigned svga_dispbpp;

  Bit32u bank_base[2];
  Bit32u bank_limit[2];
  Bit32u memsize_mask;
  Bit8u *disp_ptr;

  struct {
    bx_cirrus_bitblt_rop_t rop_handler;
    int pixelwidth;
    int bltwidth;
    int bltheight;
    int dstpitch;
    int srcpitch;
    Bit8u bltmode;
    Bit8u bltmodeext;
    Bit8u bltrop;
    Bit8u *dst;
    const Bit8u *src;
    Bit32u srcaddr;
#if BX_USE_CIRRUS_SMF
    void (*bitblt_ptr)();
#else
    void (*bitblt_ptr)(void *this_ptr);
#endif
    Bit8u *memsrc_ptr; // CPU -> video
    Bit8u *memsrc_endptr;
    int memsrc_needed;
    Bit8u *memdst_ptr; // video -> CPU
    Bit8u *memdst_endptr;
    int memdst_bytesperline;
    int memdst_needed;
    Bit8u memsrc[CIRRUS_BLT_CACHESIZE];
    Bit8u memdst[CIRRUS_BLT_CACHESIZE];
  } bitblt;

  struct {
    Bit16u x, y, size;
  } hw_cursor;

  struct {
    Bit16u x, y, w, h;
  } redraw;

  bx_bool is_unlocked() { return svga_unlock_special; }

  bx_bool banking_granularity_is_16k() { return !!(control.reg[0x0B] & 0x20); }
  bx_bool banking_is_dual() { return !!(control.reg[0x0B] & 0x01); }

#if BX_SUPPORT_PCI
  BX_CIRRUS_SMF void svga_init_pcihandlers(void);

  BX_CIRRUS_SMF bx_bool cirrus_mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_CIRRUS_SMF bx_bool cirrus_mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif
};

#endif // BX_SUPPORT_CLGD54XX
