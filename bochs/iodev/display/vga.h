/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
//  PCI VGA dummy adapter Copyright (C) 2002,2003  Mike Nordell
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

#ifndef BX_IODEV_VGA_H
#define BX_IODEV_VGA_H

// Bochs VBE definitions

#define VBE_DISPI_TOTAL_VIDEO_MEMORY_MB  16
#define VBE_DISPI_4BPP_PLANE_SHIFT       22

#define VBE_DISPI_BANK_ADDRESS           0xA0000
#define VBE_DISPI_BANK_SIZE_KB           64

#define VBE_DISPI_MAX_XRES               2560
#define VBE_DISPI_MAX_YRES               1600
#define VBE_DISPI_MAX_BPP                32

#define VBE_DISPI_IOPORT_INDEX           0x01CE
#define VBE_DISPI_IOPORT_DATA            0x01CF

#define VBE_DISPI_INDEX_ID               0x0
#define VBE_DISPI_INDEX_XRES             0x1
#define VBE_DISPI_INDEX_YRES             0x2
#define VBE_DISPI_INDEX_BPP              0x3
#define VBE_DISPI_INDEX_ENABLE           0x4
#define VBE_DISPI_INDEX_BANK             0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH       0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT      0x7
#define VBE_DISPI_INDEX_X_OFFSET         0x8
#define VBE_DISPI_INDEX_Y_OFFSET         0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define VBE_DISPI_ID0                    0xB0C0
#define VBE_DISPI_ID1                    0xB0C1
#define VBE_DISPI_ID2                    0xB0C2
#define VBE_DISPI_ID3                    0xB0C3
#define VBE_DISPI_ID4                    0xB0C4
#define VBE_DISPI_ID5                    0xB0C5

#define VBE_DISPI_BPP_4                  0x04
#define VBE_DISPI_BPP_8                  0x08
#define VBE_DISPI_BPP_15                 0x0F
#define VBE_DISPI_BPP_16                 0x10
#define VBE_DISPI_BPP_24                 0x18
#define VBE_DISPI_BPP_32                 0x20

#define VBE_DISPI_DISABLED               0x00
#define VBE_DISPI_ENABLED                0x01
#define VBE_DISPI_GETCAPS                0x02
#define VBE_DISPI_8BIT_DAC               0x20
#define VBE_DISPI_LFB_ENABLED            0x40
#define VBE_DISPI_NOCLEARMEM             0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS   0xE0000000

#define VBE_DISPI_TOTAL_VIDEO_MEMORY_KB  (VBE_DISPI_TOTAL_VIDEO_MEMORY_MB * 1024)
#define VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB * 1024)

// End Bochs VBE definitions

#if BX_USE_VGA_SMF
#  define BX_VGA_SMF  static
#  define BX_VGA_THIS theVga->
#  define BX_VGA_THIS_PTR theVga
#else
#  define BX_VGA_SMF
#  define BX_VGA_THIS this->
#  define BX_VGA_THIS_PTR this
#endif

class bx_vga_c : public bx_vgacore_c {
public:
  bx_vga_c();
  virtual ~bx_vga_c();
  virtual void   reset(unsigned type);
  BX_VGA_SMF bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_VGA_SMF bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  virtual Bit8u  mem_read(bx_phy_address addr);
  virtual void   mem_write(bx_phy_address addr, Bit8u value);
  virtual void   register_state(void);
  virtual void   after_restore_state(void);

  virtual void   redraw_area(unsigned x0, unsigned y0,
                             unsigned width, unsigned height);

  virtual void   init_vga_extension(void);
  virtual void   refresh_display(void *this_ptr, bx_bool redraw);

  static void     timer_handler(void *);
#if BX_USE_VGA_SMF == 0
  BX_VGA_SMF void timer(void);
#endif

#if BX_SUPPORT_PCI
  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#endif
#if BX_DEBUGGER
  virtual void   debug_dump(int argc, char **argv);
#endif

  static Bit64s   vga_param_handler(bx_param_c *param, int set, Bit64s val);

protected:
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if BX_USE_VGA_SMF
  static void   write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#endif
  void  write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log);

  BX_VGA_SMF void update(void);

  // Bochs VBE section
#if BX_SUPPORT_PCI
  virtual bx_bool vbe_set_base_addr(Bit32u *addr, Bit8u *pci_conf);
#endif

  BX_VGA_SMF Bit8u vbe_mem_read(bx_phy_address addr) BX_CPP_AttrRegparmN(1);
  BX_VGA_SMF void  vbe_mem_write(bx_phy_address addr, Bit8u value) BX_CPP_AttrRegparmN(2);

  static Bit32u vbe_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   vbe_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

#if BX_USE_VGA_SMF == 0
  Bit32u vbe_read(Bit32u address, unsigned io_len);
  void  vbe_write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log);
#endif

private:
  bx_bool vbe_present;
  struct {
    Bit16u  cur_dispi;
    Bit32u  base_address;
    Bit16u  xres;
    Bit16u  yres;
    Bit16u  bpp;
    Bit16u  max_xres;
    Bit16u  max_yres;
    Bit16u  max_bpp;
    Bit16u  bank;
    bx_bool enabled;
    Bit16u  curindex;
    Bit32u  visible_screen_size; /**< in bytes */
    Bit16u  offset_x;            /**< Virtual screen x start (in pixels) */
    Bit16u  offset_y;            /**< Virtual screen y start (in pixels) */
    Bit16u  virtual_xres;
    Bit16u  virtual_yres;
    Bit32u  virtual_start;   /**< For dealing with bpp>8, this is where the virtual screen starts. */
    Bit8u   bpp_multiplier;  /**< We have to save this b/c sometimes we need to recalculate stuff with it. */
    bx_bool lfb_enabled;
    bx_bool get_capabilities;
    bx_bool dac_8bit;
  } vbe;  // VBE state information
};

#endif
