/////////////////////////////////////////////////////////////////////////
// $Id: vga.cc,v 1.126 2006-02-27 19:04:01 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#define LOG_THIS theVga->

// (mch)
#define VGA_TRACE_FEATURE

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, do nothing.
#define SET_TILE_UPDATED(xtile,ytile,value)                              \
  do {                                                                   \
    if (((xtile) < BX_NUM_X_TILES) && ((ytile) < BX_NUM_Y_TILES))        \
      BX_VGA_THIS s.vga_tile_updated[(xtile)][(ytile)] = value;          \
  } while (0)

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, return 0.
#define GET_TILE_UPDATED(xtile,ytile)                                    \
  ((((xtile) < BX_NUM_X_TILES) && ((ytile) < BX_NUM_Y_TILES))?           \
     BX_VGA_THIS s.vga_tile_updated[(xtile)][(ytile)]                    \
     : 0)

static const Bit16u charmap_offset[8] = {
  0x0000, 0x4000, 0x8000, 0xc000,
  0x2000, 0x6000, 0xa000, 0xe000
};

static const Bit8u ccdat[16][4] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0xff, 0x00, 0x00, 0x00 },
  { 0x00, 0xff, 0x00, 0x00 },
  { 0xff, 0xff, 0x00, 0x00 },
  { 0x00, 0x00, 0xff, 0x00 },
  { 0xff, 0x00, 0xff, 0x00 },
  { 0x00, 0xff, 0xff, 0x00 },
  { 0xff, 0xff, 0xff, 0x00 },
  { 0x00, 0x00, 0x00, 0xff },
  { 0xff, 0x00, 0x00, 0xff },
  { 0x00, 0xff, 0x00, 0xff },
  { 0xff, 0xff, 0x00, 0xff },
  { 0x00, 0x00, 0xff, 0xff },
  { 0xff, 0x00, 0xff, 0xff },
  { 0x00, 0xff, 0xff, 0xff },
  { 0xff, 0xff, 0xff, 0xff },
};

bx_vga_c *theVga = NULL;

unsigned old_iHeight = 0, old_iWidth = 0, old_MSL = 0;

#if BX_SUPPORT_CLGD54XX
  void
libvga_set_smf_pointer(bx_vga_c *theVga_ptr)
{
  theVga = theVga_ptr;
}
#else // BX_SUPPORT_CLGD54XX
  int
libvga_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theVga = new bx_vga_c ();
  bx_devices.pluginVgaDevice = theVga;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theVga, BX_PLUGIN_VGA);
  return(0); // Success
}

  void
libvga_LTX_plugin_fini(void)
{
}
#endif // BX_SUPPORT_CLGD54XX

bx_vga_c::bx_vga_c(void)
{
  put("VGA");
  s.vga_mem_updated = 0;
  s.x_tilesize = X_TILESIZE;
  s.y_tilesize = Y_TILESIZE;
  timer_id = BX_NULL_TIMER_HANDLE;
#if BX_SUPPORT_VBE
  s.vbe_memory = NULL;
#endif
}


bx_vga_c::~bx_vga_c(void)
{
#if BX_SUPPORT_VBE
  if (s.vbe_memory != NULL) {
    delete [] s.vbe_memory;
  }
#endif
}


  void
bx_vga_c::init(void)
{
  unsigned i,string_i;
  unsigned x,y;
#if BX_SUPPORT_VBE  
  Bit16u max_xres, max_yres, max_bpp;
#endif
  int argc;
  char *argv[16];
  char *ptr;
  char string[512];
#if BX_SUPPORT_VBE  
  unsigned addr;
#endif

  BX_VGA_THIS extension_init = 0;
  BX_VGA_THIS extension_checked = 0;
#if !BX_SUPPORT_CLGD54XX
  BX_VGA_THIS init_iohandlers(read_handler,write_handler);
#endif // !BX_SUPPORT_CLGD54XX

  DEV_register_memory_handlers(theVga, mem_read_handler, mem_write_handler,
                               0xa0000, 0xbffff);

  BX_VGA_THIS s.vga_enabled = 1;
  BX_VGA_THIS s.misc_output.color_emulation  = 1;
  BX_VGA_THIS s.misc_output.enable_ram  = 1;
  BX_VGA_THIS s.misc_output.clock_select     = 0;
  BX_VGA_THIS s.misc_output.select_high_bank = 0;
  BX_VGA_THIS s.misc_output.horiz_sync_pol   = 1;
  BX_VGA_THIS s.misc_output.vert_sync_pol    = 1;

  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha = 0;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type = 0;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics = 1;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity = 0;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat = 0;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select = 0;
  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size = 0;

  BX_VGA_THIS s.line_offset=80;
  BX_VGA_THIS s.line_compare=1023;
  BX_VGA_THIS s.vertical_display_end=399;

  for (i=0; i<=0x18; i++)
    BX_VGA_THIS s.CRTC.reg[i] = 0;
  BX_VGA_THIS s.CRTC.address = 0;
  BX_VGA_THIS s.CRTC.write_protect = 0;

  BX_VGA_THIS s.attribute_ctrl.flip_flop = 0;
  BX_VGA_THIS s.attribute_ctrl.address = 0;
  BX_VGA_THIS s.attribute_ctrl.video_enabled = 1;
  for (i=0; i<16; i++)
    BX_VGA_THIS s.attribute_ctrl.palette_reg[i] = 0;
  BX_VGA_THIS s.attribute_ctrl.overscan_color = 0;
  BX_VGA_THIS s.attribute_ctrl.color_plane_enable = 0x0f;
  BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning = 0;
  BX_VGA_THIS s.attribute_ctrl.color_select = 0;

  for (i=0; i<256; i++) {
    BX_VGA_THIS s.pel.data[i].red = 0;
    BX_VGA_THIS s.pel.data[i].green = 0;
    BX_VGA_THIS s.pel.data[i].blue = 0;
    }
  BX_VGA_THIS s.pel.write_data_register = 0;
  BX_VGA_THIS s.pel.write_data_cycle = 0;
  BX_VGA_THIS s.pel.read_data_register = 0;
  BX_VGA_THIS s.pel.read_data_cycle = 0;
  BX_VGA_THIS s.pel.dac_state = 0x01;
  BX_VGA_THIS s.pel.mask = 0xff;

  BX_VGA_THIS s.graphics_ctrl.index = 0;
  BX_VGA_THIS s.graphics_ctrl.set_reset = 0;
  BX_VGA_THIS s.graphics_ctrl.enable_set_reset = 0;
  BX_VGA_THIS s.graphics_ctrl.color_compare = 0;
  BX_VGA_THIS s.graphics_ctrl.data_rotate = 0;
  BX_VGA_THIS s.graphics_ctrl.raster_op    = 0;
  BX_VGA_THIS s.graphics_ctrl.read_map_select = 0;
  BX_VGA_THIS s.graphics_ctrl.write_mode = 0;
  BX_VGA_THIS s.graphics_ctrl.read_mode  = 0;
  BX_VGA_THIS s.graphics_ctrl.odd_even = 0;
  BX_VGA_THIS s.graphics_ctrl.chain_odd_even = 0;
  BX_VGA_THIS s.graphics_ctrl.shift_reg = 0;
  BX_VGA_THIS s.graphics_ctrl.graphics_alpha = 0;
  BX_VGA_THIS s.graphics_ctrl.memory_mapping = 2; // monochrome text mode
  BX_VGA_THIS s.graphics_ctrl.color_dont_care = 0;
  BX_VGA_THIS s.graphics_ctrl.bitmask = 0;
  for (i=0; i<4; i++) {
    BX_VGA_THIS s.graphics_ctrl.latch[i] = 0;
    }

  BX_VGA_THIS s.sequencer.index = 0;
  BX_VGA_THIS s.sequencer.map_mask = 0;
  for (i=0; i<4; i++) {
    BX_VGA_THIS s.sequencer.map_mask_bit[i] = 0;
    }
  BX_VGA_THIS s.sequencer.reset1 = 1;
  BX_VGA_THIS s.sequencer.reset2 = 1;
  BX_VGA_THIS s.sequencer.reg1 = 0;
  BX_VGA_THIS s.sequencer.char_map_select = 0;
  BX_VGA_THIS s.sequencer.extended_mem = 1; // display mem greater than 64K
  BX_VGA_THIS s.sequencer.odd_even = 1; // use sequential addressing mode
  BX_VGA_THIS s.sequencer.chain_four = 0; // use map mask & read map select

  memset(BX_VGA_THIS s.vga_memory, 0, sizeof(BX_VGA_THIS s.vga_memory));

  BX_VGA_THIS s.vga_mem_updated = 0;
  for (y=0; y<480/Y_TILESIZE; y++)
    for (x=0; x<640/X_TILESIZE; x++)
      SET_TILE_UPDATED (x, y, 0);

  memset(argv, 0, sizeof(argv));
  argc = 1;
  argv[0] = "bochs";
  if (strlen(SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->getptr())) {
    ptr = strtok(SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->getptr(), ",");
    while (ptr) {
      string_i = 0;
      for (i=0; i<strlen(ptr); i++) {
        if (!isspace(ptr[i])) string[string_i++] = ptr[i];
      }
      string[string_i] = '\0';
      if (argv[argc] != NULL) {
        free(argv[argc]);
        argv[argc] = NULL;
      }
      if (argc < 16) {
        argv[argc++] = strdup(string);
      } else {
        BX_PANIC (("too many parameters, max is 16\n"));
      }
      ptr = strtok(NULL, ",");
    }
  }
  bx_gui->init(argc, argv, BX_VGA_THIS s.x_tilesize, BX_VGA_THIS s.y_tilesize);
  for (i = 1; i < 16; i++)
  {
    if ( argv[i] != NULL )
    {
        free(argv[i]);
        argv[i] = NULL;
    }
  }

#if !BX_SUPPORT_CLGD54XX
  BX_VGA_THIS init_systemtimer(timer_handler, vga_param_handler);
#endif // !BX_SUPPORT_CLGD54XX

  /* video card with BIOS ROM */
  DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0xcf) | 0x00); 

  BX_VGA_THIS s.charmap_address = 0;
  BX_VGA_THIS s.x_dotclockdiv2 = 0;
  BX_VGA_THIS s.y_doublescan = 0;
  BX_VGA_THIS s.last_bpp = 8;

#if BX_SUPPORT_VBE  
    // The following is for the vbe display extension

  BX_VGA_THIS s.vbe_enabled=0;
  BX_VGA_THIS s.vbe_8bit_dac=0;
  if (!strcmp(SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr(), "vbe")) {
    for (addr=VBE_DISPI_IOPORT_INDEX; addr<=VBE_DISPI_IOPORT_DATA; addr++) {
      DEV_register_ioread_handler(this, vbe_read_handler, addr, "vga video", 7);
      DEV_register_iowrite_handler(this, vbe_write_handler, addr, "vga video", 7);
    }    
    if (!BX_SUPPORT_PCIUSB || !bx_options.usb[0].Oenabled->get()) {
      for (addr=VBE_DISPI_IOPORT_INDEX_OLD; addr<=VBE_DISPI_IOPORT_DATA_OLD; addr++) {
        DEV_register_ioread_handler(this, vbe_read_handler, addr, "vga video", 7);
        DEV_register_iowrite_handler(this, vbe_write_handler, addr, "vga video", 7);
      }    
    }
    DEV_register_memory_handlers(theVga, mem_read_handler, mem_write_handler,
                                 VBE_DISPI_LFB_PHYSICAL_ADDRESS,
                                 VBE_DISPI_LFB_PHYSICAL_ADDRESS + VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES - 1);

    if (BX_VGA_THIS s.vbe_memory == NULL)
      BX_VGA_THIS s.vbe_memory = new Bit8u[VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES];
    memset(BX_VGA_THIS s.vbe_memory, 0, VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES);
    BX_VGA_THIS s.vbe_cur_dispi=VBE_DISPI_ID0;
    BX_VGA_THIS s.vbe_xres=640;
    BX_VGA_THIS s.vbe_yres=480;
    BX_VGA_THIS s.vbe_bpp=8;
    BX_VGA_THIS s.vbe_bank=0;
    BX_VGA_THIS s.vbe_curindex=0;
    BX_VGA_THIS s.vbe_offset_x=0;
    BX_VGA_THIS s.vbe_offset_y=0;
    BX_VGA_THIS s.vbe_virtual_xres=640;
    BX_VGA_THIS s.vbe_virtual_yres=480;
    BX_VGA_THIS s.vbe_bpp_multiplier=1;
    BX_VGA_THIS s.vbe_virtual_start=0;
    BX_VGA_THIS s.vbe_lfb_enabled=0;
    BX_VGA_THIS s.vbe_get_capabilities=0;
    bx_gui->get_capabilities(&max_xres, &max_yres,
                             &max_bpp);
    if (max_xres > VBE_DISPI_MAX_XRES) {
      BX_VGA_THIS s.vbe_max_xres=VBE_DISPI_MAX_XRES;
    } else {
      BX_VGA_THIS s.vbe_max_xres=max_xres;
    }
    if (max_yres > VBE_DISPI_MAX_YRES) {
      BX_VGA_THIS s.vbe_max_yres=VBE_DISPI_MAX_YRES;
    } else {
      BX_VGA_THIS s.vbe_max_yres=max_yres;
    }
    if (max_bpp > VBE_DISPI_MAX_BPP) {
      BX_VGA_THIS s.vbe_max_bpp=VBE_DISPI_MAX_BPP;
    } else {
      BX_VGA_THIS s.vbe_max_bpp=max_bpp;
    }
    BX_VGA_THIS extension_init = 1;
  
    BX_INFO(("VBE Bochs Display Extension Enabled"));
  }
#endif  
}

  void
bx_vga_c::init_iohandlers(bx_read_handler_t f_read, bx_write_handler_t f_write)
{
  unsigned addr, i;
  Bit8u io_mask[16] = {3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1};
  for (addr=0x03B4; addr<=0x03B5; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 1);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
    }

  for (addr=0x03BA; addr<=0x03BA; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 1);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
    }

  i = 0;
  for (addr=0x03C0; addr<=0x03CF; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", io_mask[i++]);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
    }

  for (addr=0x03D4; addr<=0x03D5; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 3);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
    }

  for (addr=0x03DA; addr<=0x03DA; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 1);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
    }

}

  void
bx_vga_c::init_systemtimer(bx_timer_handler_t f_timer, param_event_handler f_param)
{
  bx_param_num_c *vga_update_interval = SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL);
  BX_INFO(("interval=%u", vga_update_interval->get()));
  if (BX_VGA_THIS timer_id == BX_NULL_TIMER_HANDLE) {
    BX_VGA_THIS timer_id = bx_pc_system.register_timer(this, f_timer,
       vga_update_interval->get (), 1, 1, "vga");
    vga_update_interval->set_handler (f_param);
    vga_update_interval->set_runtime_param (1);
  }
}

  void
bx_vga_c::reset(unsigned type)
{
  if (!BX_VGA_THIS extension_checked) {
    char *strptr = SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr();
    if (!BX_VGA_THIS extension_init &&
        (strlen(strptr) > 0) &&
        strcmp(strptr, "none")) {
      BX_PANIC(("unknown display extension: %s", strptr));
    }
    BX_VGA_THIS extension_checked = 1;
  }
}


  void
bx_vga_c::determine_screen_dimensions(unsigned *piHeight, unsigned *piWidth)
{
  int ai[0x20];
  int i,h,v;
  for ( i = 0 ; i < 0x20 ; i++ )
   ai[i] = BX_VGA_THIS s.CRTC.reg[i];

  h = (ai[1] + 1) * 8;
  v = (ai[18] | ((ai[7] & 0x02) << 7) | ((ai[7] & 0x40) << 3)) + 1;

  if ( BX_VGA_THIS s.graphics_ctrl.shift_reg == 0 )
    {
    *piWidth = 640;
    *piHeight = 480;

    if ( BX_VGA_THIS s.CRTC.reg[6] == 0xBF )
      {
      if (BX_VGA_THIS s.CRTC.reg[23] == 0xA3 &&
         BX_VGA_THIS s.CRTC.reg[20] == 0x40 &&
         BX_VGA_THIS s.CRTC.reg[9] == 0x41)
        {
        *piWidth = 320;
        *piHeight = 240;
        }
      else {
        if (BX_VGA_THIS s.x_dotclockdiv2) h <<= 1;
        *piWidth = h;
        *piHeight = v;
        }
      }
    else if ((h >= 640) && (v >= 480)) {
      *piWidth = h;
      *piHeight = v;
      }
    }
  else if ( BX_VGA_THIS s.graphics_ctrl.shift_reg == 2 )
    {

    if ( BX_VGA_THIS s.sequencer.chain_four )
      {
      *piWidth = h;
      *piHeight = v;
      }
    else
      {
      *piWidth = h;
      *piHeight = v;
      }
    }
  else
    {
    if (BX_VGA_THIS s.x_dotclockdiv2) h <<= 1;
    *piWidth = h;
    *piHeight = v;
    }
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_vga_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_VGA_SMF
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_vga_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_VGA_SMF
  bx_bool  horiz_retrace = 0, vert_retrace = 0;
  Bit64u usec;
  Bit16u ret16, vertres;
  Bit8u retval;

#if defined(VGA_TRACE_FEATURE)
  Bit32u ret = 0;
#define RETURN(x) do { ret = (x); goto read_return; } while (0)
#else
#define RETURN return
#endif

  if (io_len == 2) {
#if BX_USE_VGA_SMF
    ret16 = bx_vga_c::read_handler(0, address, 1);
    ret16 |= (bx_vga_c::read_handler(0, address+1, 1)) << 8;
#else
    ret16 = bx_vga_c::read(address, 1);
    ret16 |= (bx_vga_c::read(address+1, 1) << 8;
#endif
    RETURN(ret16);
    }

#ifdef __OS2__
  if ( bx_options.videomode == BX_VIDEO_DIRECT )
     {
     return _inp(address);
     }
#endif

#if !defined(VGA_TRACE_FEATURE)
    BX_DEBUG(("io read from 0x%04x", (unsigned) address));
#endif

  if ( (address >= 0x03b0) && (address <= 0x03bf) &&
       (BX_VGA_THIS s.misc_output.color_emulation) ) {
	RETURN(0xff);
  }
  if ( (address >= 0x03d0) && (address <= 0x03df) &&
       (BX_VGA_THIS s.misc_output.color_emulation==0) ) {
	RETURN(0xff);
  }

  switch (address) {
    case 0x03ba: /* Input Status 1 (monochrome emulation modes) */
    case 0x03ca: /* Feature Control ??? */
    case 0x03da: /* Input Status 1 (color emulation modes) */
      // bit3: Vertical Retrace
      //       0 = display is in the display mode
      //       1 = display is in the vertical retrace mode
      // bit0: Display Enable
      //       0 = display is in the display mode
      //       1 = display is not in the display mode; either the
      //           horizontal or vertical retrace period is active

      // using 72 Hz vertical frequency
      usec = bx_pc_system.time_usec();
      switch ( ( BX_VGA_THIS s.misc_output.vert_sync_pol << 1) | BX_VGA_THIS s.misc_output.horiz_sync_pol )
      {
        case 0: vertres = 200; break;
        case 1: vertres = 400; break;
        case 2: vertres = 350; break;
        default: vertres = 480; break;
      }
      if ((usec % 13888) < 70) {
        vert_retrace = 1;
      }
      if ((usec % (13888 / vertres)) == 0) {
        horiz_retrace = 1;
      }

      retval = 0;
      if (horiz_retrace || vert_retrace)
        retval = 0x01;
      if (vert_retrace)
        retval |= 0x08;

      /* reading this port resets the flip-flop to address mode */
      BX_VGA_THIS s.attribute_ctrl.flip_flop = 0;
      RETURN(retval);
      break;


    case 0x03c0: /* */
      if (BX_VGA_THIS s.attribute_ctrl.flip_flop == 0) {
        //BX_INFO(("io read: 0x3c0: flip_flop = 0"));
        retval =
          (BX_VGA_THIS s.attribute_ctrl.video_enabled << 5) |
          BX_VGA_THIS s.attribute_ctrl.address;
        RETURN(retval);
        }
      else {
        BX_ERROR(("io read: 0x3c0: flip_flop != 0"));
        return(0);
        }
      break;

    case 0x03c1: /* */
      switch (BX_VGA_THIS s.attribute_ctrl.address) {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b:
        case 0x0c: case 0x0d: case 0x0e: case 0x0f:
          retval = BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address];
	  RETURN(retval);
          break;
        case 0x10: /* mode control register */
          retval =
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha << 0) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type << 1) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics << 2) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity << 3) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat << 5) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select << 6) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size << 7);
	  RETURN(retval);
          break;
	case 0x11: /* overscan color register */
	  RETURN(BX_VGA_THIS s.attribute_ctrl.overscan_color);
          break;
	case 0x12: /* color plane enable */
	  RETURN(BX_VGA_THIS s.attribute_ctrl.color_plane_enable);
          break;
        case 0x13: /* horizontal PEL panning register */
          RETURN(BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning);
          break;
        case 0x14: /* color select register */
          RETURN(BX_VGA_THIS s.attribute_ctrl.color_select);
          break;
        default:
          BX_INFO(("io read: 0x3c1: unknown register 0x%02x",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.address));
          RETURN(0);
        }
      break;

    case 0x03c2: /* Input Status 0 */
      BX_DEBUG(("io read 0x3c2: input status #0: ignoring"));
      RETURN(0);
      break;

    case 0x03c3: /* VGA Enable Register */
      RETURN(BX_VGA_THIS s.vga_enabled);
      break;

    case 0x03c4: /* Sequencer Index Register */
      RETURN(BX_VGA_THIS s.sequencer.index);
      break;

    case 0x03c5: /* Sequencer Registers 00..04 */
      switch (BX_VGA_THIS s.sequencer.index) {
        case 0: /* sequencer: reset */
          BX_DEBUG(("io read 0x3c5: sequencer reset"));
          RETURN(BX_VGA_THIS s.sequencer.reset1 | (BX_VGA_THIS s.sequencer.reset2<<1));
          break;
        case 1: /* sequencer: clocking mode */
          BX_DEBUG(("io read 0x3c5: sequencer clocking mode"));
          RETURN(BX_VGA_THIS s.sequencer.reg1);
          break;
        case 2: /* sequencer: map mask register */
          RETURN(BX_VGA_THIS s.sequencer.map_mask);
          break;
        case 3: /* sequencer: character map select register */
          RETURN(BX_VGA_THIS s.sequencer.char_map_select);
          break;
        case 4: /* sequencer: memory mode register */
          retval =
            (BX_VGA_THIS s.sequencer.extended_mem   << 1) |
            (BX_VGA_THIS s.sequencer.odd_even       << 2) |
            (BX_VGA_THIS s.sequencer.chain_four     << 3);
          RETURN(retval);
          break;

        default:
          BX_DEBUG(("io read 0x3c5: index %u unhandled",
            (unsigned) BX_VGA_THIS s.sequencer.index));
          RETURN(0);
        }
      break;

    case 0x03c6: /* PEL mask ??? */
      RETURN(BX_VGA_THIS s.pel.mask);
      break;

    case 0x03c7: /* DAC state, read = 11b, write = 00b */
      RETURN(BX_VGA_THIS s.pel.dac_state);
      break;

    case 0x03c8: /* PEL address write mode */
      RETURN(BX_VGA_THIS s.pel.write_data_register);
      break;

    case 0x03c9: /* PEL Data Register, colors 00..FF */
      if (BX_VGA_THIS s.pel.dac_state == 0x03) {
        switch (BX_VGA_THIS s.pel.read_data_cycle) {
          case 0:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].red;
            break;
          case 1:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].green;
            break;
          case 2:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].blue;
            break;
          default:
            retval = 0; // keep compiler happy
          }
        BX_VGA_THIS s.pel.read_data_cycle++;
        if (BX_VGA_THIS s.pel.read_data_cycle >= 3) {
          BX_VGA_THIS s.pel.read_data_cycle = 0;
          BX_VGA_THIS s.pel.read_data_register++;
          }
	}
      else {
        retval = 0x3f;
        }
      RETURN(retval);
      break;

    case 0x03cc: /* Miscellaneous Output / Graphics 1 Position ??? */
      retval =
        ((BX_VGA_THIS s.misc_output.color_emulation  & 0x01) << 0) |
        ((BX_VGA_THIS s.misc_output.enable_ram       & 0x01) << 1) |
        ((BX_VGA_THIS s.misc_output.clock_select     & 0x03) << 2) |
        ((BX_VGA_THIS s.misc_output.select_high_bank & 0x01) << 5) |
        ((BX_VGA_THIS s.misc_output.horiz_sync_pol   & 0x01) << 6) |
        ((BX_VGA_THIS s.misc_output.vert_sync_pol    & 0x01) << 7);
      RETURN(retval);
      break;

    case 0x03ce: /* Graphics Controller Index Register */
      RETURN(BX_VGA_THIS s.graphics_ctrl.index);
      break;

    case 0x03cd: /* ??? */
      BX_DEBUG(("io read from 03cd"));
      RETURN(0x00);
      break;

    case 0x03cf: /* Graphics Controller Registers 00..08 */
      switch (BX_VGA_THIS s.graphics_ctrl.index) {
        case 0: /* Set/Reset */
          RETURN(BX_VGA_THIS s.graphics_ctrl.set_reset);
          break;
        case 1: /* Enable Set/Reset */
          RETURN(BX_VGA_THIS s.graphics_ctrl.enable_set_reset);
          break;
        case 2: /* Color Compare */
          RETURN(BX_VGA_THIS s.graphics_ctrl.color_compare);
          break;
        case 3: /* Data Rotate */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.raster_op & 0x03) << 3) |
            ((BX_VGA_THIS s.graphics_ctrl.data_rotate & 0x07) << 0);
          RETURN(retval);
          break;
        case 4: /* Read Map Select */
          RETURN(BX_VGA_THIS s.graphics_ctrl.read_map_select);
          break;
        case 5: /* Mode */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.shift_reg & 0x03) << 5) |
            ((BX_VGA_THIS s.graphics_ctrl.odd_even & 0x01 ) << 4) |
            ((BX_VGA_THIS s.graphics_ctrl.read_mode & 0x01) << 3) |
            ((BX_VGA_THIS s.graphics_ctrl.write_mode & 0x03) << 0);

          if (BX_VGA_THIS s.graphics_ctrl.odd_even ||
              BX_VGA_THIS s.graphics_ctrl.shift_reg)
            BX_DEBUG(("io read 0x3cf: reg 05 = 0x%02x", (unsigned) retval));
          RETURN(retval);
          break;
        case 6: /* Miscellaneous */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.memory_mapping & 0x03 ) << 2) |
            ((BX_VGA_THIS s.graphics_ctrl.odd_even & 0x01) << 1) |
            ((BX_VGA_THIS s.graphics_ctrl.graphics_alpha & 0x01) << 0);
          RETURN(retval);
          break;
        case 7: /* Color Don't Care */
          RETURN(BX_VGA_THIS s.graphics_ctrl.color_dont_care);
          break;
        case 8: /* Bit Mask */
          RETURN(BX_VGA_THIS s.graphics_ctrl.bitmask);
          break;
        default:
          /* ??? */
          BX_DEBUG(("io read: 0x3cf: index %u unhandled",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.index));
          RETURN(0);
        }
      break;

    case 0x03d4: /* CRTC Index Register (color emulation modes) */
      RETURN(BX_VGA_THIS s.CRTC.address);
      break;

    case 0x03b5: /* CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* CRTC Registers (color emulation modes) */
      if (BX_VGA_THIS s.CRTC.address > 0x18) {
        BX_DEBUG(("io read: invalid CRTC register 0x%02x",
          (unsigned) BX_VGA_THIS s.CRTC.address));
        RETURN(0);
      }
      RETURN(BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address]);
      break;

    case 0x03b4: /* CRTC Index Register (monochrome emulation modes) */
    case 0x03cb: /* not sure but OpenBSD reads it a lot */
    default:
      BX_INFO(("io read from vga port 0x%04x", (unsigned) address));
      RETURN(0); /* keep compiler happy */
    }

#if defined(VGA_TRACE_FEATURE)
  read_return:
    if (io_len == 1) {
      BX_DEBUG(("8-bit read from 0x%04x = 0x%02x", (unsigned) address, ret));
    } else {
      BX_DEBUG(("16-bit read from 0x%04x = 0x%04x", (unsigned) address, ret));
    }
    return ret;
#endif
}
#if defined(VGA_TRACE_FEATURE)
#undef RETURN
#endif

  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_vga_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_VGA_SMF
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  class_ptr->write(address, value, io_len, 0);
#else
  UNUSED(this_ptr);
  theVga->write(address, value, io_len, 0);
#endif
}

  void
bx_vga_c::write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_VGA_SMF
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  class_ptr->write(address, value, io_len, 1);
#else
  UNUSED(this_ptr);
  theVga->write(address, value, io_len, 1);
#endif
}

  void
bx_vga_c::write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log)
{
  unsigned i;
  Bit8u charmap1, charmap2, prev_memory_mapping;
  bx_bool prev_video_enabled, prev_line_graphics, prev_int_pal_size;
  bx_bool prev_graphics_alpha, prev_chain_odd_even;
  bx_bool needs_update = 0;

#if defined(VGA_TRACE_FEATURE)
  if (!no_log)
	switch (io_len) {
	      case 1:
		    BX_DEBUG(("8-bit write to %04x = %02x", (unsigned)address, (unsigned)value));
		    break;
	      case 2:
		    BX_DEBUG(("16-bit write to %04x = %04x", (unsigned)address, (unsigned)value));
		    break;
	      default:
		    BX_PANIC(("Weird VGA write size"));
	}
#else
  if (io_len == 1) {
    BX_DEBUG(("io write to 0x%04x = 0x%02x", (unsigned) address,
      (unsigned) value));
  }
#endif

  if (io_len == 2) {
#if BX_USE_VGA_SMF
    bx_vga_c::write_handler_no_log(0, address, value & 0xff, 1);
    bx_vga_c::write_handler_no_log(0, address+1, (value >> 8) & 0xff, 1);
#else
    bx_vga_c::write(address, value & 0xff, 1, 1);
    bx_vga_c::write(address+1, (value >> 8) & 0xff, 1, 1);
#endif
    return;
    }

#ifdef __OS2__
  if ( bx_options.videomode == BX_VIDEO_DIRECT )
     {
     _outp(address,value);
     return;
     }
#endif

  if ( (address >= 0x03b0) && (address <= 0x03bf) &&
       (BX_VGA_THIS s.misc_output.color_emulation) )
    return;
  if ( (address >= 0x03d0) && (address <= 0x03df) &&
       (BX_VGA_THIS s.misc_output.color_emulation==0) )
    return;

  switch (address) {
    case 0x03ba: /* Feature Control (monochrome emulation modes) */
#if !defined(VGA_TRACE_FEATURE)
      BX_DEBUG(("io write 3ba: feature control: ignoring"));
#endif
      break;

    case 0x03c0: /* Attribute Controller */
      if (BX_VGA_THIS s.attribute_ctrl.flip_flop == 0) { /* address mode */
        prev_video_enabled = BX_VGA_THIS s.attribute_ctrl.video_enabled;
        BX_VGA_THIS s.attribute_ctrl.video_enabled = (value >> 5) & 0x01;
#if !defined(VGA_TRACE_FEATURE)
        BX_DEBUG(("io write 3c0: video_enabled = %u",
                  (unsigned) BX_VGA_THIS s.attribute_ctrl.video_enabled));
#endif
        if (BX_VGA_THIS s.attribute_ctrl.video_enabled == 0)
          bx_gui->clear_screen();
        else if (!prev_video_enabled) {
#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("found enable transition"));
#endif
          needs_update = 1;
        }
        value &= 0x1f; /* address = bits 0..4 */
        BX_VGA_THIS s.attribute_ctrl.address = value;
        switch (value) {
          case 0x00: case 0x01: case 0x02: case 0x03:
          case 0x04: case 0x05: case 0x06: case 0x07:
          case 0x08: case 0x09: case 0x0a: case 0x0b:
          case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            break;

          default:
            BX_DEBUG(("io write 0x3c0: address mode reg=0x%02x",
              (unsigned) value));
          }
        }
      else { /* data-write mode */
        switch (BX_VGA_THIS s.attribute_ctrl.address) {
          case 0x00: case 0x01: case 0x02: case 0x03:
          case 0x04: case 0x05: case 0x06: case 0x07:
          case 0x08: case 0x09: case 0x0a: case 0x0b:
          case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            if (value != BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address]) {
              BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address] =
                value;
              needs_update = 1;
            }
            break;
          case 0x10: // mode control register
            prev_line_graphics = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics;
            prev_int_pal_size = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha =
              (value >> 0) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type =
              (value >> 1) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics =
              (value >> 2) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity =
              (value >> 3) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat =
              (value >> 5) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select =
              (value >> 6) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size =
              (value >> 7) & 0x01;
            if (((value >> 2) & 0x01) != prev_line_graphics) {
              bx_gui->set_text_charmap(
                & BX_VGA_THIS s.vga_memory[0x20000 + BX_VGA_THIS s.charmap_address]);
              BX_VGA_THIS s.vga_mem_updated = 1;
            }
            if (((value >> 7) & 0x01) != prev_int_pal_size) {
              needs_update = 1;
            }
#if !defined(VGA_TRACE_FEATURE)
            BX_DEBUG(("io write 0x3c0: mode control: 0x%02x",
                (unsigned) value));
#endif
            break;
          case 0x11: // Overscan Color Register
            BX_VGA_THIS s.attribute_ctrl.overscan_color = (value & 0x3f);
#if !defined(VGA_TRACE_FEATURE)
            BX_DEBUG(("io write 0x3c0: overscan color = 0x%02x",
                        (unsigned) value));
#endif
            break;
          case 0x12: // Color Plane Enable Register
            BX_VGA_THIS s.attribute_ctrl.color_plane_enable = (value & 0x0f);
            needs_update = 1;
#if !defined(VGA_TRACE_FEATURE)
            BX_DEBUG(("io write 0x3c0: color plane enable = 0x%02x",
                        (unsigned) value));
#endif
            break;
          case 0x13: // Horizontal Pixel Panning Register
            BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning = (value & 0x0f);
            needs_update = 1;
#if !defined(VGA_TRACE_FEATURE)
            BX_DEBUG(("io write 0x3c0: horiz pel panning = 0x%02x",
                        (unsigned) value));
#endif
            break;
          case 0x14: // Color Select Register
            BX_VGA_THIS s.attribute_ctrl.color_select = (value & 0x0f);
            needs_update = 1;
#if !defined(VGA_TRACE_FEATURE)
            BX_DEBUG(("io write 0x3c0: color select = 0x%02x",
                        (unsigned) BX_VGA_THIS s.attribute_ctrl.color_select));
#endif
            break;
          default:
            BX_DEBUG(("io write 0x3c0: data-write mode 0x%02x",
              (unsigned) BX_VGA_THIS s.attribute_ctrl.address));
          }
        }
      BX_VGA_THIS s.attribute_ctrl.flip_flop = !BX_VGA_THIS s.attribute_ctrl.flip_flop;
      break;

    case 0x03c2: // Miscellaneous Output Register
      BX_VGA_THIS s.misc_output.color_emulation  = (value >> 0) & 0x01;
      BX_VGA_THIS s.misc_output.enable_ram       = (value >> 1) & 0x01;
      BX_VGA_THIS s.misc_output.clock_select     = (value >> 2) & 0x03;
      BX_VGA_THIS s.misc_output.select_high_bank = (value >> 5) & 0x01;
      BX_VGA_THIS s.misc_output.horiz_sync_pol   = (value >> 6) & 0x01;
      BX_VGA_THIS s.misc_output.vert_sync_pol    = (value >> 7) & 0x01;
#if !defined(VGA_TRACE_FEATURE)
        BX_DEBUG(("io write 3c2:"));
        BX_DEBUG(("  color_emulation (attempted) = %u",
                  (value >> 0) & 0x01));
        BX_DEBUG(("  enable_ram = %u",
                  (unsigned) BX_VGA_THIS s.misc_output.enable_ram));
        BX_DEBUG(("  clock_select = %u",
                  (unsigned) BX_VGA_THIS s.misc_output.clock_select));
        BX_DEBUG(("  select_high_bank = %u",
                  (unsigned) BX_VGA_THIS s.misc_output.select_high_bank));
        BX_DEBUG(("  horiz_sync_pol = %u",
                  (unsigned) BX_VGA_THIS s.misc_output.horiz_sync_pol));
        BX_DEBUG(("  vert_sync_pol = %u",
                  (unsigned) BX_VGA_THIS s.misc_output.vert_sync_pol));
#endif
      break;

    case 0x03c3: // VGA enable
      // bit0: enables VGA display if set
      BX_VGA_THIS s.vga_enabled = value & 0x01;
#if !defined(VGA_TRACE_FEATURE)
      BX_DEBUG(("io write 0x03c3: VGA enable = %u", BX_VGA_THIS s.vga_enabled));
#endif
      break;

    case 0x03c4: /* Sequencer Index Register */
      if (value > 4) {
        BX_DEBUG(("io write 3c4: value > 4"));
        }
      BX_VGA_THIS s.sequencer.index = value;
      break;

    case 0x03c5: /* Sequencer Registers 00..04 */
      switch (BX_VGA_THIS s.sequencer.index) {
        case 0: /* sequencer: reset */
#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("write 0x3c5: sequencer reset: value=0x%02x",
                      (unsigned) value));
#endif
          if (BX_VGA_THIS s.sequencer.reset1 && ((value & 0x01) == 0)) {
            BX_VGA_THIS s.sequencer.char_map_select = 0;
            BX_VGA_THIS s.charmap_address = 0;
            bx_gui->set_text_charmap(
              & BX_VGA_THIS s.vga_memory[0x20000 + BX_VGA_THIS s.charmap_address]);
            BX_VGA_THIS s.vga_mem_updated = 1;
          }
          BX_VGA_THIS s.sequencer.reset1 = (value >> 0) & 0x01;
          BX_VGA_THIS s.sequencer.reset2 = (value >> 1) & 0x01;
          break;
        case 1: /* sequencer: clocking mode */
#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("io write 0x3c5=0x%02x: clocking mode reg: ignoring",
                      (unsigned) value));
#endif
          BX_VGA_THIS s.sequencer.reg1 = value & 0x3d;
          BX_VGA_THIS s.x_dotclockdiv2 = ((value & 0x08) > 0);
          break;
        case 2: /* sequencer: map mask register */
          BX_VGA_THIS s.sequencer.map_mask = (value & 0x0f);
          for (i=0; i<4; i++)
            BX_VGA_THIS s.sequencer.map_mask_bit[i] = (value >> i) & 0x01;
          break;
        case 3: /* sequencer: character map select register */
          BX_VGA_THIS s.sequencer.char_map_select = value & 0x3f;
          charmap1 = value & 0x13;
          if (charmap1 > 3) charmap1 = (charmap1 & 3) + 4;
          charmap2 = (value & 0x2C) >> 2;
          if (charmap2 > 3) charmap2 = (charmap2 & 3) + 4;
	  if (BX_VGA_THIS s.CRTC.reg[0x09] > 0) {
            BX_VGA_THIS s.charmap_address = charmap_offset[charmap1];
            bx_gui->set_text_charmap(
              & BX_VGA_THIS s.vga_memory[0x20000 + BX_VGA_THIS s.charmap_address]);
            BX_VGA_THIS s.vga_mem_updated = 1;
            }
          if (charmap2 != charmap1)
            BX_INFO(("char map select: map #2 in block #%d unused", charmap2));
          break;
        case 4: /* sequencer: memory mode register */
          BX_VGA_THIS s.sequencer.extended_mem   = (value >> 1) & 0x01;
          BX_VGA_THIS s.sequencer.odd_even       = (value >> 2) & 0x01;
          BX_VGA_THIS s.sequencer.chain_four     = (value >> 3) & 0x01;

#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("io write 0x3c5: memory mode:"));
          BX_DEBUG(("  extended_mem = %u",
              (unsigned) BX_VGA_THIS s.sequencer.extended_mem));
          BX_DEBUG(("  odd_even     = %u",
              (unsigned) BX_VGA_THIS s.sequencer.odd_even));
          BX_DEBUG(("  chain_four   = %u",
              (unsigned) BX_VGA_THIS s.sequencer.chain_four));
#endif
          break;
        default:
          BX_DEBUG(("io write 0x3c5: index 0x%02x unhandled",
            (unsigned) BX_VGA_THIS s.sequencer.index));
        }
      break;

    case 0x03c6: /* PEL mask */
      BX_VGA_THIS s.pel.mask = value;
      if (BX_VGA_THIS s.pel.mask != 0xff)
        BX_DEBUG(("io write 0x3c6: PEL mask=0x%02x != 0xFF", value));
      // BX_VGA_THIS s.pel.mask should be and'd with final value before
      // indexing into color register BX_VGA_THIS s.pel.data[]
      break;

    case 0x03c7: // PEL address, read mode
      BX_VGA_THIS s.pel.read_data_register = value;
      BX_VGA_THIS s.pel.read_data_cycle    = 0;
      BX_VGA_THIS s.pel.dac_state = 0x03;
      break;

    case 0x03c8: /* PEL address write mode */
      BX_VGA_THIS s.pel.write_data_register = value;
      BX_VGA_THIS s.pel.write_data_cycle    = 0;
      BX_VGA_THIS s.pel.dac_state = 0x00;
      break;

    case 0x03c9: /* PEL Data Register, colors 00..FF */
      switch (BX_VGA_THIS s.pel.write_data_cycle) {
        case 0:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red = value;
          break;
        case 1:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green = value;
          break;
        case 2:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue = value;

#if BX_SUPPORT_VBE
          if (BX_VGA_THIS s.vbe_8bit_dac) {
            needs_update |= bx_gui->palette_change(BX_VGA_THIS s.pel.write_data_register,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue);
          } else {
#endif
            needs_update |= bx_gui->palette_change(BX_VGA_THIS s.pel.write_data_register,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red<<2,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green<<2,
              BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue<<2);
#if BX_SUPPORT_VBE
          }
#endif
          break;
        }

      BX_VGA_THIS s.pel.write_data_cycle++;
      if (BX_VGA_THIS s.pel.write_data_cycle >= 3) {
        //BX_INFO(("BX_VGA_THIS s.pel.data[%u] {r=%u, g=%u, b=%u}",
        //  (unsigned) BX_VGA_THIS s.pel.write_data_register,
        //  (unsigned) BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red,
        //  (unsigned) BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green,
        //  (unsigned) BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue);
        BX_VGA_THIS s.pel.write_data_cycle = 0;
        BX_VGA_THIS s.pel.write_data_register++;
        }
      break;

    case 0x03ca: /* Graphics 2 Position (EGA) */
      // ignore, EGA only???
      break;

    case 0x03cc: /* Graphics 1 Position (EGA) */
      // ignore, EGA only???
      break;

    case 0x03cd: /* ??? */
      BX_DEBUG(("io write to 0x3cd = 0x%02x", (unsigned) value));
      break;

    case 0x03ce: /* Graphics Controller Index Register */
      if (value > 0x08) /* ??? */
        BX_DEBUG(("io write: 0x3ce: value > 8"));
      BX_VGA_THIS s.graphics_ctrl.index = value;
      break;

    case 0x03cf: /* Graphics Controller Registers 00..08 */
      switch (BX_VGA_THIS s.graphics_ctrl.index) {
        case 0: /* Set/Reset */
          BX_VGA_THIS s.graphics_ctrl.set_reset = value & 0x0f;
          break;
        case 1: /* Enable Set/Reset */
          BX_VGA_THIS s.graphics_ctrl.enable_set_reset = value & 0x0f;
          break;
        case 2: /* Color Compare */
          BX_VGA_THIS s.graphics_ctrl.color_compare = value & 0x0f;
          break;
        case 3: /* Data Rotate */
          BX_VGA_THIS s.graphics_ctrl.data_rotate = value & 0x07;
          BX_VGA_THIS s.graphics_ctrl.raster_op    = (value >> 3) & 0x03;
          break;
        case 4: /* Read Map Select */
          BX_VGA_THIS s.graphics_ctrl.read_map_select = value & 0x03;
#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("io write to 0x3cf = 0x%02x (RMS)", (unsigned) value));
#endif
          break;
        case 5: /* Mode */
          BX_VGA_THIS s.graphics_ctrl.write_mode        = value & 0x03;
          BX_VGA_THIS s.graphics_ctrl.read_mode         = (value >> 3) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.odd_even          = (value >> 4) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.shift_reg         = (value >> 5) & 0x03;

          if (BX_VGA_THIS s.graphics_ctrl.odd_even)
            BX_DEBUG(("io write: 0x3cf: mode reg: value = 0x%02x",
              (unsigned) value));
          if (BX_VGA_THIS s.graphics_ctrl.shift_reg)
            BX_DEBUG(("io write: 0x3cf: mode reg: value = 0x%02x",
              (unsigned) value));
          break;
        case 6: /* Miscellaneous */
          prev_graphics_alpha = BX_VGA_THIS s.graphics_ctrl.graphics_alpha;
          prev_chain_odd_even = BX_VGA_THIS s.graphics_ctrl.chain_odd_even;
          prev_memory_mapping = BX_VGA_THIS s.graphics_ctrl.memory_mapping;

          BX_VGA_THIS s.graphics_ctrl.graphics_alpha = value & 0x01;
          BX_VGA_THIS s.graphics_ctrl.chain_odd_even = (value >> 1) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.memory_mapping = (value >> 2) & 0x03;
#if !defined(VGA_TRACE_FEATURE)
          BX_DEBUG(("memory_mapping set to %u",
              (unsigned) BX_VGA_THIS s.graphics_ctrl.memory_mapping));
          BX_DEBUG(("graphics mode set to %u",
              (unsigned) BX_VGA_THIS s.graphics_ctrl.graphics_alpha));
          BX_DEBUG(("odd_even mode set to %u",
              (unsigned) BX_VGA_THIS s.graphics_ctrl.odd_even));
          BX_DEBUG(("io write: 0x3cf: misc reg: value = 0x%02x",
                (unsigned) value));
#endif
          if (prev_memory_mapping != BX_VGA_THIS s.graphics_ctrl.memory_mapping)
            needs_update = 1;
          if (prev_graphics_alpha != BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
            needs_update = 1;
            old_iHeight = 0;
          }
          break;
        case 7: /* Color Don't Care */
          BX_VGA_THIS s.graphics_ctrl.color_dont_care = value & 0x0f;
          break;
        case 8: /* Bit Mask */
          BX_VGA_THIS s.graphics_ctrl.bitmask = value;
          break;
        default:
          /* ??? */
          BX_DEBUG(("io write: 0x3cf: index %u unhandled",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.index));
        }
      break;

    case 0x03b4: /* CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* CRTC Index Register (color emulation modes) */
      BX_VGA_THIS s.CRTC.address = value & 0x7f;
      if (BX_VGA_THIS s.CRTC.address > 0x18)
        BX_DEBUG(("write: invalid CRTC register 0x%02x selected",
          (unsigned) BX_VGA_THIS s.CRTC.address));
      break;

    case 0x03b5: /* CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* CRTC Registers (color emulation modes) */
      if (BX_VGA_THIS s.CRTC.address > 0x18) {
        BX_DEBUG(("write: invalid CRTC register 0x%02x ignored",
          (unsigned) BX_VGA_THIS s.CRTC.address));
        return;
      }
      if (BX_VGA_THIS s.CRTC.write_protect && (BX_VGA_THIS s.CRTC.address < 0x08)) {
        if (BX_VGA_THIS s.CRTC.address == 0x07) {
          BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] &= ~0x10;
          BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] |= (value & 0x10);
          BX_VGA_THIS s.line_compare &= 0x2ff;
          if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x10) BX_VGA_THIS s.line_compare |= 0x100;
          needs_update = 1;
          break;
        } else {
          return;
        }
      }
      if (value != BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address]) {
        BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] = value;
        switch (BX_VGA_THIS s.CRTC.address) {
          case 0x07:
            BX_VGA_THIS s.vertical_display_end &= 0xff;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x02) BX_VGA_THIS s.vertical_display_end |= 0x100;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x40) BX_VGA_THIS s.vertical_display_end |= 0x200;
            BX_VGA_THIS s.line_compare &= 0x2ff;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x10) BX_VGA_THIS s.line_compare |= 0x100;
            needs_update = 1;
            break;
          case 0x08:
            // Vertical pel panning change
            needs_update = 1;
            break;
          case 0x09:
            BX_VGA_THIS s.y_doublescan = ((value & 0x9f) > 0);
            BX_VGA_THIS s.line_compare &= 0x1ff;
            if (BX_VGA_THIS s.CRTC.reg[0x09] & 0x40) BX_VGA_THIS s.line_compare |= 0x200;
            needs_update = 1;
            break;
          case 0x0A:
          case 0x0B:
          case 0x0E:
          case 0x0F:
            // Cursor size / location change
            BX_VGA_THIS s.vga_mem_updated = 1;
            break;
          case 0x0C:
          case 0x0D:
            // Start address change
            if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
              needs_update = 1;
            } else {
              BX_VGA_THIS s.vga_mem_updated = 1;
            }
            break;
          case 0x11:
            BX_VGA_THIS s.CRTC.write_protect = ((BX_VGA_THIS s.CRTC.reg[0x11] & 0x80) > 0);
            break;
          case 0x12:
            BX_VGA_THIS s.vertical_display_end &= 0x300;
            BX_VGA_THIS s.vertical_display_end |= BX_VGA_THIS s.CRTC.reg[0x12];
            break;
          case 0x13:
          case 0x14:
          case 0x17:
#if BX_SUPPORT_VBE
            if (!BX_VGA_THIS s.vbe_enabled)
#endif
            {
              // Line offset change
              BX_VGA_THIS s.line_offset = BX_VGA_THIS s.CRTC.reg[0x13] << 1;
              if (BX_VGA_THIS s.CRTC.reg[0x14] & 0x40) BX_VGA_THIS s.line_offset <<= 2;
              else if ((BX_VGA_THIS s.CRTC.reg[0x17] & 0x40) == 0) BX_VGA_THIS s.line_offset <<= 1;
              needs_update = 1;
            }
            break;
          case 0x18:
            BX_VGA_THIS s.line_compare &= 0x300;
            BX_VGA_THIS s.line_compare |= BX_VGA_THIS s.CRTC.reg[0x18];
            needs_update = 1;
            break;
        }

      }
      break;

    case 0x03da: /* Feature Control (color emulation modes) */
      BX_DEBUG(("io write: 3da: ignoring: feature ctrl & vert sync"));
      break;

    case 0x03c1: /* */
    default:
      BX_ERROR(("unsupported io write to port 0x%04x, val=0x%02x",
        (unsigned) address, (unsigned) value));
    }
  if (needs_update) {
    // Mark all video as updated so the changes will go through
    BX_VGA_THIS redraw_area(0, 0, old_iWidth, old_iHeight);
  }
}

Bit64s bx_vga_c::vga_param_handler(bx_param_c *param, int set, Bit64s val)
{
  // handler for runtime parameter 'vga_update_interval'
  if (set) {
    BX_INFO (("Changing timer interval to %d", (Bit32u)val));
    BX_VGA_THIS timer_handler (theVga);
    bx_pc_system.activate_timer (BX_VGA_THIS timer_id, (Bit32u)val, 1);
  }
  return val;
}

  void
bx_vga_c::trigger_timer(void *this_ptr)
{
  timer_handler(this_ptr);
}

  void
bx_vga_c::timer_handler(void *this_ptr)
{
#if !BX_USE_VGA_SMF
  
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  class_ptr->timer();
}

  void
bx_vga_c::timer(void)
{
#else
  UNUSED(this_ptr);
#endif

  update();
  bx_gui->flush();

}


  void
bx_vga_c::update(void)
{
  unsigned iHeight, iWidth;

  /* no screen update necessary */
  if (BX_VGA_THIS s.vga_mem_updated==0)
    return;

  /* skip screen update when vga/video is disabled or the sequencer is in reset mode */
  if (!BX_VGA_THIS s.vga_enabled || !BX_VGA_THIS s.attribute_ctrl.video_enabled
      || !BX_VGA_THIS s.sequencer.reset2 || !BX_VGA_THIS s.sequencer.reset1)
    return;

  /* skip screen update if the vertical retrace is in progress
     (using 72 Hz vertical frequency) */
  if ((bx_pc_system.time_usec() % 13888) < 70)
    return;

#if BX_SUPPORT_VBE  
  if ((BX_VGA_THIS s.vbe_enabled) && (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4))
  {
    // specific VBE code display update code
    unsigned pitch;
    unsigned xc, yc, xti, yti;
    unsigned r, c, w, h;
    int i;
    unsigned long red, green, blue, colour;
    Bit8u * vid_ptr, * vid_ptr2;
    Bit8u * tile_ptr, * tile_ptr2;
    bx_svga_tileinfo_t info;
    Bit8u dac_size = BX_VGA_THIS s.vbe_8bit_dac ? 8 : 6;

    iWidth=BX_VGA_THIS s.vbe_xres;
    iHeight=BX_VGA_THIS s.vbe_yres;
    pitch = BX_VGA_THIS s.line_offset;
    Bit8u *disp_ptr = &BX_VGA_THIS s.vbe_memory[BX_VGA_THIS s.vbe_virtual_start]; 

    if (bx_gui->graphics_tile_info(&info)) {
      if (info.is_indexed) {
        switch (BX_VGA_THIS s.vbe_bpp) {
          case 4:
          case 15:
          case 16:
          case 24:
          case 32:
            BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays"));
            break;
          case 8:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + xc);
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      colour = 0;
                      for (i=0; i<(int)BX_VGA_THIS s.vbe_bpp; i+=8) {
                        colour |= *(vid_ptr2++) << i;
                      }
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch;
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
        }
      }
      else {
        switch (BX_VGA_THIS s.vbe_bpp) {
          case 4:
            BX_ERROR(("cannot draw 4bpp SVGA"));
            break;
          case 8:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + xc);
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      colour = *(vid_ptr2++);
                      colour = MAKE_COLOUR(
                        BX_VGA_THIS s.pel.data[colour].red, dac_size, info.red_shift, info.red_mask,
                        BX_VGA_THIS s.pel.data[colour].green, dac_size, info.green_shift, info.green_mask,
                        BX_VGA_THIS s.pel.data[colour].blue, dac_size, info.blue_shift, info.blue_mask);
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch;
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
          case 15:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + (xc<<1));
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      colour = *(vid_ptr2++);
                      colour |= *(vid_ptr2++) << 8;
                      colour = MAKE_COLOUR(
                        colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                        colour & 0x03e0, 10, info.green_shift, info.green_mask,
                        colour & 0x7c00, 15, info.red_shift, info.red_mask);
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch; 
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
          case 16:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + (xc<<1));
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      colour = *(vid_ptr2++);
                      colour |= *(vid_ptr2++) << 8;
                      colour = MAKE_COLOUR(
                        colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                        colour & 0x07e0, 11, info.green_shift, info.green_mask,
                        colour & 0xf800, 16, info.red_shift, info.red_mask);
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch; 
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
          case 24:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + 3*xc);
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      blue = *(vid_ptr2++);
                      green = *(vid_ptr2++);
                      red = *(vid_ptr2++);
                      colour = MAKE_COLOUR(
                        red, 8, info.red_shift, info.red_mask,
                        green, 8, info.green_shift, info.green_mask,
                        blue, 8, info.blue_shift, info.blue_mask);
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch; 
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
          case 32:
            for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
              for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                if (GET_TILE_UPDATED (xti, yti)) {
                  vid_ptr = disp_ptr + (yc * pitch + (xc<<2));
                  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                  for (r=0; r<h; r++) {
                    vid_ptr2  = vid_ptr;
                    tile_ptr2 = tile_ptr;
                    for (c=0; c<w; c++) {
                      blue = *(vid_ptr2++);
                      green = *(vid_ptr2++);
                      red = *(vid_ptr2++);
                      vid_ptr2++;
                      colour = MAKE_COLOUR(
                        red, 8, info.red_shift, info.red_mask,
                        green, 8, info.green_shift, info.green_mask,
                        blue, 8, info.blue_shift, info.blue_mask);
                      if (info.is_little_endian) {
                        for (i=0; i<info.bpp; i+=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                      else {
                        for (i=info.bpp-8; i>-8; i-=8) {
                          *(tile_ptr2++) = (Bit8u)(colour >> i);
                        }
                      }
                    }
                    vid_ptr  += pitch;
                    tile_ptr += info.pitch;
                  }
                  bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                  SET_TILE_UPDATED (xti, yti, 0);
                }
              }
            }
            break;
        }
      }
      old_iWidth = iWidth;
      old_iHeight = iHeight;
      BX_VGA_THIS s.vga_mem_updated = 0;
    }
    else {
      BX_PANIC(("cannot get svga tile info"));
    }

    // after a vbe display update, don't try to do any 'normal vga' updates anymore
    return;
  }
#endif  
  // fields that effect the way video memory is serialized into screen output:
  // GRAPHICS CONTROLLER:
  //   BX_VGA_THIS s.graphics_ctrl.shift_reg:
  //     0: output data in standard VGA format or CGA-compatible 640x200 2 color
  //        graphics mode (mode 6)
  //     1: output data in CGA-compatible 320x200 4 color graphics mode
  //        (modes 4 & 5)
  //     2: output data 8 bits at a time from the 4 bit planes
  //        (mode 13 and variants like modeX)

  // if (BX_VGA_THIS s.vga_mem_updated==0 || BX_VGA_THIS s.attribute_ctrl.video_enabled == 0)

  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    Bit8u color;
    unsigned bit_no, r, c, x, y;
    unsigned long byte_offset, start_addr;
    unsigned xc, yc, xti, yti;

    start_addr = (BX_VGA_THIS s.CRTC.reg[0x0c] << 8) | BX_VGA_THIS s.CRTC.reg[0x0d];

//BX_DEBUG(("update: shiftreg=%u, chain4=%u, mapping=%u",
//  (unsigned) BX_VGA_THIS s.graphics_ctrl.shift_reg,
//  (unsigned) BX_VGA_THIS s.sequencer.chain_four,
//  (unsigned) BX_VGA_THIS s.graphics_ctrl.memory_mapping);

    determine_screen_dimensions(&iHeight, &iWidth);
    if((iWidth != old_iWidth) || (iHeight != old_iHeight) ||
        (BX_VGA_THIS s.last_bpp > 8))
    {
      bx_gui->dimension_update(iWidth, iHeight);
      old_iWidth = iWidth;
      old_iHeight = iHeight;
      BX_VGA_THIS s.last_bpp = 8;
    }

    switch ( BX_VGA_THIS s.graphics_ctrl.shift_reg ) {

      case 0:
        Bit8u attribute, palette_reg_val, DAC_regno;
        unsigned long line_compare;
        Bit8u *plane0, *plane1, *plane2, *plane3;

        if (BX_VGA_THIS s.graphics_ctrl.memory_mapping == 3) { // CGA 640x200x2

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  for (c=0; c<X_TILESIZE; c++) {

                    x = xc + c;
                    /* 0 or 0x2000 */
                    byte_offset = start_addr + ((y & 1) << 13);
                    /* to the start of the line */
                    byte_offset += (320 / 4) * (y / 2);
                    /* to the byte start */
                    byte_offset += (x / 8);

                    bit_no = 7 - (x % 8);
                    palette_reg_val = (((BX_VGA_THIS s.vga_memory[byte_offset]) >> bit_no) & 1);
                    DAC_regno = BX_VGA_THIS s.attribute_ctrl.palette_reg[palette_reg_val];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = DAC_regno;
                  }
                }
                SET_TILE_UPDATED (xti, yti, 0);
                bx_gui->graphics_tile_update(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        } else { // output data in serial fashion with each display plane
                 // output on its associated serial output.  Standard EGA/VGA format

#if BX_SUPPORT_VBE  
          if (BX_VGA_THIS s.vbe_enabled)
          {
            plane0 = &BX_VGA_THIS s.vbe_memory[0<<20];
            plane1 = &BX_VGA_THIS s.vbe_memory[1<<20];
            plane2 = &BX_VGA_THIS s.vbe_memory[2<<20];
            plane3 = &BX_VGA_THIS s.vbe_memory[3<<20];
            start_addr = BX_VGA_THIS s.vbe_virtual_start;
            line_compare = 0xffff;
          }
          else
#endif
          {
            plane0 = &BX_VGA_THIS s.vga_memory[0<<16];
            plane1 = &BX_VGA_THIS s.vga_memory[1<<16];
            plane2 = &BX_VGA_THIS s.vga_memory[2<<16];
            plane3 = &BX_VGA_THIS s.vga_memory[3<<16];
            line_compare = BX_VGA_THIS s.line_compare;
            if (BX_VGA_THIS s.y_doublescan) line_compare >>= 1;
          }

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    if (BX_VGA_THIS s.x_dotclockdiv2) x >>= 1;
                    bit_no = 7 - (x % 8);
                    if (y > line_compare) {
                      byte_offset = x / 8 +
                        ((y - line_compare - 1) * BX_VGA_THIS s.line_offset);
                    } else {
                      byte_offset = start_addr + x / 8 +
                        (y * BX_VGA_THIS s.line_offset);
                    }
                    attribute =
                      (((plane0[byte_offset] >> bit_no) & 0x01) << 0) |
                      (((plane1[byte_offset] >> bit_no) & 0x01) << 1) |
                      (((plane2[byte_offset] >> bit_no) & 0x01) << 2) |
                      (((plane3[byte_offset] >> bit_no) & 0x01) << 3);

                    attribute &= BX_VGA_THIS s.attribute_ctrl.color_plane_enable;
                    // undocumented feature ???: colors 0..7 high intensity, colors 8..15 blinking
                    // using low/high intensity. Blinking is not implemented yet.
                    if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity) attribute ^= 0x08;
                    palette_reg_val = BX_VGA_THIS s.attribute_ctrl.palette_reg[attribute];
                    if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size) {
                      // use 4 lower bits from palette register
                      // use 4 higher bits from color select register
                      // 16 banks of 16-color registers
                      DAC_regno = (palette_reg_val & 0x0f) |
                                  (BX_VGA_THIS s.attribute_ctrl.color_select << 4);
                      }
                    else {
                      // use 6 lower bits from palette register
                      // use 2 higher bits from color select register
                      // 4 banks of 64-color registers
                      DAC_regno = (palette_reg_val & 0x3f) |
                                  ((BX_VGA_THIS s.attribute_ctrl.color_select & 0x0c) << 4);
                      }
                    // DAC_regno &= video DAC mask register ???

                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = DAC_regno;
                    }
                  }
                SET_TILE_UPDATED (xti, yti, 0);
                bx_gui->graphics_tile_update(BX_VGA_THIS s.tile, xc, yc);
                }
              }
            }
          }
        break; // case 0

      case 1: // output the data in a CGA-compatible 320x200 4 color graphics
              // mode.  (modes 4 & 5)

        /* CGA 320x200x4 start */

        for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
          for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
            if (GET_TILE_UPDATED (xti, yti)) {
              for (r=0; r<Y_TILESIZE; r++) {
                y = yc + r;
                if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                for (c=0; c<X_TILESIZE; c++) {

                  x = xc + c;
                  if (BX_VGA_THIS s.x_dotclockdiv2) x >>= 1;
                  /* 0 or 0x2000 */
                  byte_offset = start_addr + ((y & 1) << 13);
                  /* to the start of the line */
                  byte_offset += (320 / 4) * (y / 2);
                  /* to the byte start */
                  byte_offset += (x / 4);

                  attribute = 6 - 2*(x % 4);
                  palette_reg_val = (BX_VGA_THIS s.vga_memory[byte_offset]) >> attribute;
                  palette_reg_val &= 3;
                  DAC_regno = BX_VGA_THIS s.attribute_ctrl.palette_reg[palette_reg_val];
                  BX_VGA_THIS s.tile[r*X_TILESIZE + c] = DAC_regno;
                }
              }
              SET_TILE_UPDATED (xti, yti, 0);
              bx_gui->graphics_tile_update(BX_VGA_THIS s.tile, xc, yc);
            }
          }
        }
        /* CGA 320x200x4 end */

        break; // case 1

      case 2: // output the data eight bits at a time from the 4 bit plane
              // (format for VGA mode 13 hex)
      case 3: // FIXME: is this really the same ???

        if ( BX_VGA_THIS s.sequencer.chain_four ) {
          unsigned long pixely, pixelx, plane;

          if (BX_VGA_THIS s.misc_output.select_high_bank != 1)
            BX_PANIC(("update: select_high_bank != 1"));

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  pixely = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) pixely >>= 1;
                  for (c=0; c<X_TILESIZE; c++) {
                    pixelx = (xc + c) >> 1;
                    plane  = (pixelx % 4);
                    byte_offset = start_addr + (plane * 65536) +
                                  (pixely * BX_VGA_THIS s.line_offset) + (pixelx & ~0x03);
                    color = BX_VGA_THIS s.vga_memory[byte_offset];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = color;
                    }
                  }
                SET_TILE_UPDATED (xti, yti, 0);
                bx_gui->graphics_tile_update(BX_VGA_THIS s.tile, xc, yc);
                }
              }
            }
          }

        else { // chain_four == 0, modeX
          unsigned long pixely, pixelx, plane;

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  pixely = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) pixely >>= 1;
                  for (c=0; c<X_TILESIZE; c++) {
                    pixelx = (xc + c) >> 1;
                    plane  = (pixelx % 4);
                    byte_offset = (plane * 65536) +
                                  (pixely * BX_VGA_THIS s.line_offset)
                                  + (pixelx >> 2);
                    color = BX_VGA_THIS s.vga_memory[start_addr + byte_offset];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = color;
                    }
                  }
                SET_TILE_UPDATED (xti, yti, 0);
                bx_gui->graphics_tile_update(BX_VGA_THIS s.tile, xc, yc);
                }
	      }
	    }
          }
        break; // case 2

      default:
        BX_PANIC(("update: shift_reg == %u", (unsigned)
          BX_VGA_THIS s.graphics_ctrl.shift_reg ));
      }

    BX_VGA_THIS s.vga_mem_updated = 0;
    return;
    }

  else { // text mode
    unsigned long start_address;
    unsigned long cursor_address, cursor_x, cursor_y;
    bx_vga_tminfo_t tm_info;
    unsigned VDE, MSL, cols, rows, cWidth;

    tm_info.start_address = 2*((BX_VGA_THIS s.CRTC.reg[12] << 8) +
                            BX_VGA_THIS s.CRTC.reg[13]);
    tm_info.cs_start = BX_VGA_THIS s.CRTC.reg[0x0a] & 0x3f;
    tm_info.cs_end = BX_VGA_THIS s.CRTC.reg[0x0b] & 0x1f;
    tm_info.line_offset = BX_VGA_THIS s.CRTC.reg[0x13] << 2;
    tm_info.line_compare = BX_VGA_THIS s.line_compare;
    tm_info.h_panning = BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning & 0x0f;
    tm_info.v_panning = BX_VGA_THIS s.CRTC.reg[0x08] & 0x1f;
    tm_info.line_graphics = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics;
    tm_info.split_hpanning =  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat;
    if ((BX_VGA_THIS s.sequencer.reg1 & 0x01) == 0) {
      if (tm_info.h_panning >= 8)
        tm_info.h_panning = 0;
      else
        tm_info.h_panning++;
    } else {
      tm_info.h_panning &= 0x07;
    }

    // Verticle Display End: find out how many lines are displayed
    VDE = BX_VGA_THIS s.vertical_display_end;
    // Maximum Scan Line: height of character cell
    MSL = BX_VGA_THIS s.CRTC.reg[0x09] & 0x1f;
    if (MSL == 0) {
      BX_ERROR(("character height = 1, skipping text update"));
      return;
    }
    cols = BX_VGA_THIS s.CRTC.reg[1] + 1;
    if ((MSL == 1) && (VDE == 399)) {
      // emulated CGA graphics mode 160x100x16 colors
      MSL = 3;
    }
    rows = (VDE+1)/(MSL+1);
    if (rows > BX_MAX_TEXT_LINES) {
      BX_PANIC(("text rows>%d: %d",BX_MAX_TEXT_LINES,rows));
      return;
    }
    cWidth = ((BX_VGA_THIS s.sequencer.reg1 & 0x01) == 1) ? 8 : 9;
    iWidth = cWidth * cols;
    iHeight = VDE+1;
    if ((iWidth != old_iWidth) || (iHeight != old_iHeight) || (MSL != old_MSL) ||
        (BX_VGA_THIS s.last_bpp > 8))
    {
      bx_gui->dimension_update(iWidth, iHeight, MSL+1, cWidth);
      old_iWidth = iWidth;
      old_iHeight = iHeight;
      old_MSL = MSL;
      BX_VGA_THIS s.last_bpp = 8;
    }
    // pass old text snapshot & new VGA memory contents
    start_address = 2*((BX_VGA_THIS s.CRTC.reg[12] << 8) +
                    BX_VGA_THIS s.CRTC.reg[13]);
    cursor_address = 2*((BX_VGA_THIS s.CRTC.reg[0x0e] << 8) +
                     BX_VGA_THIS s.CRTC.reg[0x0f]);
    if (cursor_address < start_address) {
      cursor_x = 0xffff;
      cursor_y = 0xffff;
    } else {
      cursor_x = ((cursor_address - start_address)/2) % (iWidth/cWidth);
      cursor_y = ((cursor_address - start_address)/2) / (iWidth/cWidth);
    }
    bx_gui->text_update(BX_VGA_THIS s.text_snapshot,
                        &BX_VGA_THIS s.vga_memory[start_address],
                        cursor_x, cursor_y, tm_info, rows);
    // screen updated, copy new VGA memory contents into text snapshot
    memcpy(BX_VGA_THIS s.text_snapshot,
           &BX_VGA_THIS s.vga_memory[start_address],
           2*cols*rows);
    BX_VGA_THIS s.vga_mem_updated = 0;
  }
}


  bx_bool
bx_vga_c::mem_read_handler(unsigned long addr, unsigned long len,
                           void *data, void *param)
{
  Bit8u *data_ptr;

#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = theVga->mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

  Bit8u
bx_vga_c::mem_read(Bit32u addr)
{
  Bit32u offset;
  Bit8u *plane0, *plane1, *plane2, *plane3;

#if BX_SUPPORT_VBE  
  // if in a vbe enabled mode, read from the vbe_memory
  if ((BX_VGA_THIS s.vbe_enabled) && (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4))
  {
        return vbe_mem_read(addr);
  }
  else if (addr >= VBE_DISPI_LFB_PHYSICAL_ADDRESS)
  {
        return 0xff;
  }
#endif  

#if defined(VGA_TRACE_FEATURE)
//	BX_DEBUG(("8-bit memory read from %08x", addr));
#endif

#ifdef __OS2__

#if BX_PLUGINS
#error Fix the code for plugins
#endif

  if ( bx_options.videomode == BX_VIDEO_DIRECT )
     {
     char value;

     value = devices->mem->video[addr-0xA0000];

     return value;
     }
#endif

  switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
    case 1: // 0xA0000 .. 0xAFFFF
      if (addr > 0xAFFFF) return 0xff;
      offset = addr - 0xA0000;
      break;
    case 2: // 0xB0000 .. 0xB7FFF
      if ((addr < 0xB0000) || (addr > 0xB7FFF)) return 0xff;
      return BX_VGA_THIS s.vga_memory[addr - 0xB0000];
      break;
    case 3: // 0xB8000 .. 0xBFFFF
      if (addr < 0xB8000) return 0xff;
      return BX_VGA_THIS s.vga_memory[addr - 0xB8000];
      break;
    default: // 0xA0000 .. 0xBFFFF
      return BX_VGA_THIS s.vga_memory[addr - 0xA0000];
    }

  // addr between 0xA0000 and 0xAFFFF
  if ( BX_VGA_THIS s.sequencer.chain_four ) {

    // Mode 13h: 320 x 200 256 color mode: chained pixel representation
    return BX_VGA_THIS s.vga_memory[(offset & ~0x03) + (offset % 4)*65536];
    }

#if BX_SUPPORT_VBE  
  if (BX_VGA_THIS s.vbe_enabled)
  {
    plane0 = &BX_VGA_THIS s.vbe_memory[(0<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane1 = &BX_VGA_THIS s.vbe_memory[(1<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane2 = &BX_VGA_THIS s.vbe_memory[(2<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane3 = &BX_VGA_THIS s.vbe_memory[(3<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
  }
  else
#endif
  {
    plane0 = &BX_VGA_THIS s.vga_memory[0<<16];
    plane1 = &BX_VGA_THIS s.vga_memory[1<<16];
    plane2 = &BX_VGA_THIS s.vga_memory[2<<16];
    plane3 = &BX_VGA_THIS s.vga_memory[3<<16];
  }

  /* addr between 0xA0000 and 0xAFFFF */
  switch (BX_VGA_THIS s.graphics_ctrl.read_mode) {
    case 0: /* read mode 0 */
      BX_VGA_THIS s.graphics_ctrl.latch[0] = plane0[offset];
      BX_VGA_THIS s.graphics_ctrl.latch[1] = plane1[offset];
      BX_VGA_THIS s.graphics_ctrl.latch[2] = plane2[offset];
      BX_VGA_THIS s.graphics_ctrl.latch[3] = plane3[offset];
      return(BX_VGA_THIS s.graphics_ctrl.latch[BX_VGA_THIS s.graphics_ctrl.read_map_select]);
      break;

    case 1: /* read mode 1 */
      {
      Bit8u color_compare, color_dont_care;
      Bit8u latch0, latch1, latch2, latch3, retval;

      color_compare   = BX_VGA_THIS s.graphics_ctrl.color_compare & 0x0f;
      color_dont_care = BX_VGA_THIS s.graphics_ctrl.color_dont_care & 0x0f;
      latch0 = BX_VGA_THIS s.graphics_ctrl.latch[0] = plane0[offset];
      latch1 = BX_VGA_THIS s.graphics_ctrl.latch[1] = plane1[offset];
      latch2 = BX_VGA_THIS s.graphics_ctrl.latch[2] = plane2[offset];
      latch3 = BX_VGA_THIS s.graphics_ctrl.latch[3] = plane3[offset];

      latch0 ^= ccdat[color_compare][0];
      latch1 ^= ccdat[color_compare][1];
      latch2 ^= ccdat[color_compare][2];
      latch3 ^= ccdat[color_compare][3];

      latch0 &= ccdat[color_dont_care][0];
      latch1 &= ccdat[color_dont_care][1];
      latch2 &= ccdat[color_dont_care][2];
      latch3 &= ccdat[color_dont_care][3];

      retval = ~(latch0 | latch1 | latch2 | latch3);

      return retval;
      }
      break;
    default:
      return 0;
    }
}

  bx_bool
bx_vga_c::mem_write_handler(unsigned long addr, unsigned long len,
                            void *data, void *param)
{
  Bit8u *data_ptr;

#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    theVga->mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

  void
bx_vga_c::mem_write(Bit32u addr, Bit8u value)
{
  Bit32u offset;
  Bit8u new_val[4];
  unsigned start_addr;
  Bit8u *plane0, *plane1, *plane2, *plane3;

#if BX_SUPPORT_VBE
  // if in a vbe enabled mode, write to the vbe_memory
  if ((BX_VGA_THIS s.vbe_enabled) && (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4))
  {
        vbe_mem_write(addr,value);
        return;
  }
  else if (addr >= VBE_DISPI_LFB_PHYSICAL_ADDRESS)
  {
        return;
  }
#endif

#if defined(VGA_TRACE_FEATURE)
//	BX_DEBUG(("8-bit memory write to %08x = %02x", addr, value));
#endif

#ifdef __OS2__

#if BX_PLUGINS
#error Fix the code for plugins
#endif

  if ( bx_options.videomode == BX_VIDEO_DIRECT )
    {
    devices->mem->video[addr-0xA0000] = value;

    return;
    }
#endif

  switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
    case 1: // 0xA0000 .. 0xAFFFF
      if (addr > 0xAFFFF) return;
      offset = addr - 0xA0000;
      break;
    case 2: // 0xB0000 .. 0xB7FFF
      if ((addr < 0xB0000) || (addr > 0xB7FFF)) return;
      offset = addr - 0xB0000;
      break;
    case 3: // 0xB8000 .. 0xBFFFF
      if (addr < 0xB8000) return;
      offset = addr - 0xB8000;
      break;
    default: // 0xA0000 .. 0xBFFFF
      offset = addr - 0xA0000;
    }

  start_addr = (BX_VGA_THIS s.CRTC.reg[0x0c] << 8) | BX_VGA_THIS s.CRTC.reg[0x0d];

  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    if (BX_VGA_THIS s.graphics_ctrl.memory_mapping == 3) { // 0xB8000 .. 0xBFFFF
      unsigned x_tileno, x_tileno2, y_tileno;

      /* CGA 320x200x4 / 640x200x2 start */
      BX_VGA_THIS s.vga_memory[offset] = value;
      offset -= start_addr;
      if (offset>=0x2000) {
        y_tileno = offset - 0x2000;
        y_tileno /= (320/4);
        y_tileno <<= 1; //2 * y_tileno;
        y_tileno++;
        x_tileno = (offset - 0x2000) % (320/4);
        x_tileno <<= 2; //*= 4;
      } else {
        y_tileno = offset / (320/4);
        y_tileno <<= 1; //2 * y_tileno;
        x_tileno = offset % (320/4);
        x_tileno <<= 2; //*=4;
      }
      x_tileno2=x_tileno;
      if (BX_VGA_THIS s.graphics_ctrl.shift_reg==0) {
        x_tileno*=2;
        x_tileno2+=7;
      } else {
        x_tileno2+=3;
      }
      if (BX_VGA_THIS s.x_dotclockdiv2) {
        x_tileno/=(X_TILESIZE/2);
        x_tileno2/=(X_TILESIZE/2);
      } else {
        x_tileno/=X_TILESIZE;
        x_tileno2/=X_TILESIZE;
      }
      if (BX_VGA_THIS s.y_doublescan) {
        y_tileno/=(Y_TILESIZE/2);
      } else {
        y_tileno/=Y_TILESIZE;
      }
      BX_VGA_THIS s.vga_mem_updated = 1;
      SET_TILE_UPDATED (x_tileno, y_tileno, 1);
      if (x_tileno2!=x_tileno) {
        SET_TILE_UPDATED (x_tileno2, y_tileno, 1);
      }
      return;
      /* CGA 320x200x4 / 640x200x2 end */
      }
    else if (BX_VGA_THIS s.graphics_ctrl.memory_mapping != 1) {

      BX_PANIC(("mem_write: graphics: mapping = %u",
               (unsigned) BX_VGA_THIS s.graphics_ctrl.memory_mapping));
      return;
      }

    if ( BX_VGA_THIS s.sequencer.chain_four ) {
      unsigned x_tileno, y_tileno;

      // 320 x 200 256 color mode: chained pixel representation
      BX_VGA_THIS s.vga_memory[(offset & ~0x03) + (offset % 4)*65536] = value;
      if (BX_VGA_THIS s.line_offset > 0) {
        offset -= start_addr;
        x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE/2);
        if (BX_VGA_THIS s.y_doublescan) {
          y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE/2);
        } else {
          y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
        }
        BX_VGA_THIS s.vga_mem_updated = 1;
        SET_TILE_UPDATED (x_tileno, y_tileno, 1);
      }
      return;
    }
  }

  /* addr between 0xA0000 and 0xAFFFF */

#if BX_SUPPORT_VBE
  if (BX_VGA_THIS s.vbe_enabled)
  {
    plane0 = &BX_VGA_THIS s.vbe_memory[(0<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane1 = &BX_VGA_THIS s.vbe_memory[(1<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane2 = &BX_VGA_THIS s.vbe_memory[(2<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
    plane3 = &BX_VGA_THIS s.vbe_memory[(3<<20) + (BX_VGA_THIS s.vbe_bank<<16)];
  }
  else
#endif
  {
    plane0 = &BX_VGA_THIS s.vga_memory[0<<16];
    plane1 = &BX_VGA_THIS s.vga_memory[1<<16];
    plane2 = &BX_VGA_THIS s.vga_memory[2<<16];
    plane3 = &BX_VGA_THIS s.vga_memory[3<<16];
  }

  switch (BX_VGA_THIS s.graphics_ctrl.write_mode) {
    unsigned i;

    case 0: /* write mode 0 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask;
        const Bit8u set_reset = BX_VGA_THIS s.graphics_ctrl.set_reset;
        const Bit8u enable_set_reset = BX_VGA_THIS s.graphics_ctrl.enable_set_reset;
        /* perform rotate on CPU data in case its needed */
        if (BX_VGA_THIS s.graphics_ctrl.data_rotate) {
          value = (value >> BX_VGA_THIS s.graphics_ctrl.data_rotate) |
                  (value << (8 - BX_VGA_THIS s.graphics_ctrl.data_rotate));
        }
        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;
        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // replace
            new_val[0] |= ((enable_set_reset & 1)
                           ? ((set_reset & 1) ? bitmask : 0)
                           : (value & bitmask));
            new_val[1] |= ((enable_set_reset & 2)
                           ? ((set_reset & 2) ? bitmask : 0)
                           : (value & bitmask));
            new_val[2] |= ((enable_set_reset & 4)
                           ? ((set_reset & 4) ? bitmask : 0)
                           : (value & bitmask));
            new_val[3] |= ((enable_set_reset & 8)
                           ? ((set_reset & 8) ? bitmask : 0)
                           : (value & bitmask));
            break;
          case 1: // AND
            new_val[0] |= ((enable_set_reset & 1)
                           ? ((set_reset & 1)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask);
            new_val[1] |= ((enable_set_reset & 2)
                           ? ((set_reset & 2)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask);
            new_val[2] |= ((enable_set_reset & 4)
                           ? ((set_reset & 4)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask);
            new_val[3] |= ((enable_set_reset & 8)
                           ? ((set_reset & 8)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask);
            break;
          case 2: // OR
            new_val[0]
              |= ((enable_set_reset & 1)
                  ? ((set_reset & 1)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask));
            new_val[1]
              |= ((enable_set_reset & 2)
                  ? ((set_reset & 2)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask));
            new_val[2]
              |= ((enable_set_reset & 4)
                  ? ((set_reset & 4)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask));
            new_val[3]
              |= ((enable_set_reset & 8)
                  ? ((set_reset & 8)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask));
            break;
          case 3: // XOR
            new_val[0]
              |= ((enable_set_reset & 1)
                 ? ((set_reset & 1)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask);
            new_val[1]
              |= ((enable_set_reset & 2)
                 ? ((set_reset & 2)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask);
            new_val[2]
              |= ((enable_set_reset & 4)
                 ? ((set_reset & 4)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask);
            new_val[3]
              |= ((enable_set_reset & 8)
                 ? ((set_reset & 8)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask);
            break;
          default:
            BX_PANIC(("vga_mem_write: write mode 0: op = %u",
                      (unsigned) BX_VGA_THIS s.graphics_ctrl.raster_op));
        }
      }
      break;

    case 1: /* write mode 1 */
      for (i=0; i<4; i++ ) {
        new_val[i] = BX_VGA_THIS s.graphics_ctrl.latch[i];
        }
      break;

    case 2: /* write mode 2 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask;

        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;
        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // write
            new_val[0] |= (value & 1) ? bitmask : 0;
            new_val[1] |= (value & 2) ? bitmask : 0;
            new_val[2] |= (value & 4) ? bitmask : 0;
            new_val[3] |= (value & 8) ? bitmask : 0;
            break;
          case 1: // AND
            new_val[0] |= (value & 1)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
              : 0;
            new_val[1] |= (value & 2)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
              : 0;
            new_val[2] |= (value & 4)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
              : 0;
            new_val[3] |= (value & 8)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
              : 0;
            break;
          case 2: // OR
            new_val[0] |= (value & 1)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask);
            new_val[1] |= (value & 2)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask);
            new_val[2] |= (value & 4)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask);
            new_val[3] |= (value & 8)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask);
            break;
          case 3: // XOR
            new_val[0] |= (value & 1)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask);
            new_val[1] |= (value & 2)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask);
            new_val[2] |= (value & 4)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask);
            new_val[3] |= (value & 8)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask);
            break;
        }
      }
      break;

    case 3: /* write mode 3 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask & value;
        const Bit8u set_reset = BX_VGA_THIS s.graphics_ctrl.set_reset;

        /* perform rotate on CPU data */
        if (BX_VGA_THIS s.graphics_ctrl.data_rotate) {
          value = (value >> BX_VGA_THIS s.graphics_ctrl.data_rotate) |
                  (value << (8 - BX_VGA_THIS s.graphics_ctrl.data_rotate));
        }
        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;

        value &= bitmask;

        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // write
            new_val[0] |= (set_reset & 1) ? value : 0;
            new_val[1] |= (set_reset & 2) ? value : 0;
            new_val[2] |= (set_reset & 4) ? value : 0;
            new_val[3] |= (set_reset & 8) ? value : 0;
            break;
          case 1: // AND
            new_val[0] |= ((set_reset & 1) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
          case 2: // OR
            new_val[0] |= ((set_reset & 1) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
          case 3: // XOR
            new_val[0] |= ((set_reset & 1) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
        }
      }
      break;

    default:
      BX_PANIC(("vga_mem_write: write mode %u ?",
        (unsigned) BX_VGA_THIS s.graphics_ctrl.write_mode));
  }

  if (BX_VGA_THIS s.sequencer.map_mask & 0x0f) {
    BX_VGA_THIS s.vga_mem_updated = 1;
    if (BX_VGA_THIS s.sequencer.map_mask_bit[0])
      plane0[offset] = new_val[0];
    if (BX_VGA_THIS s.sequencer.map_mask_bit[1])
      plane1[offset] = new_val[1];
    if (BX_VGA_THIS s.sequencer.map_mask_bit[2]) {
      if ((offset & 0xe000) == BX_VGA_THIS s.charmap_address) {
        bx_gui->set_text_charbyte((offset & 0x1fff), new_val[2]);
      }
      plane2[offset] = new_val[2];
    }
    if (BX_VGA_THIS s.sequencer.map_mask_bit[3])
      plane3[offset] = new_val[3];

    unsigned x_tileno, y_tileno;

    if (BX_VGA_THIS s.graphics_ctrl.shift_reg == 2) {
      offset -= start_addr;
      x_tileno = (offset % BX_VGA_THIS s.line_offset) * 4 / (X_TILESIZE / 2);
      if (BX_VGA_THIS s.y_doublescan) {
        y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE / 2);
      } else {
        y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
      }
      SET_TILE_UPDATED (x_tileno, y_tileno, 1);
    } else {
      if (BX_VGA_THIS s.line_compare < BX_VGA_THIS s.vertical_display_end) {
        if (BX_VGA_THIS s.line_offset > 0) {
          if (BX_VGA_THIS s.x_dotclockdiv2) {
            x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 16);
          } else {
            x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 8);
          }
          if (BX_VGA_THIS s.y_doublescan) {
            y_tileno = ((offset / BX_VGA_THIS s.line_offset) * 2 + BX_VGA_THIS s.line_compare + 1) / Y_TILESIZE;
          } else {
            y_tileno = ((offset / BX_VGA_THIS s.line_offset) + BX_VGA_THIS s.line_compare + 1) / Y_TILESIZE;
          }
          SET_TILE_UPDATED (x_tileno, y_tileno, 1);
        }
      }
      if (offset >= start_addr) {
        offset -= start_addr;
        if (BX_VGA_THIS s.line_offset > 0) {
          if (BX_VGA_THIS s.x_dotclockdiv2) {
            x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 16);
          } else {
            x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 8);
          }
          if (BX_VGA_THIS s.y_doublescan) {
            y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE / 2);
          } else {
            y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
          }
          SET_TILE_UPDATED (x_tileno, y_tileno, 1);
        }
      }
    }
  }
}

  void
bx_vga_c::get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight,
                                                   unsigned *txWidth)
{
  unsigned VDE, MSL;

  if (!BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    *text_snapshot = &BX_VGA_THIS s.text_snapshot[0];
    VDE = BX_VGA_THIS s.vertical_display_end;
    MSL = BX_VGA_THIS s.CRTC.reg[0x09] & 0x1f;
    *txHeight = (VDE+1)/(MSL+1);
    *txWidth = BX_VGA_THIS s.CRTC.reg[1] + 1;
  } else {
    *txHeight = 0;
    *txWidth = 0;
  }
}

  Bit8u
bx_vga_c::get_actl_palette_idx(Bit8u index)
{
  return BX_VGA_THIS s.attribute_ctrl.palette_reg[index];
}

  void
bx_vga_c::dump_status(void)
{
#if BX_DEBUGGER
  dbg_printf("s.misc_output.color_emulation = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.color_emulation);
  dbg_printf("s.misc_output.enable_ram = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.enable_ram);
  dbg_printf("s.misc_output.clock_select = %u ",
            (unsigned) BX_VGA_THIS s.misc_output.clock_select);
  if (BX_VGA_THIS s.misc_output.clock_select == 0)
    dbg_printf("(25Mhz 640 horiz pixel clock)\n");
  else
    dbg_printf("(28Mhz 720 horiz pixel clock)\n");
  dbg_printf("s.misc_output.select_high_bank = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.select_high_bank);
  dbg_printf("s.misc_output.horiz_sync_pol = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.horiz_sync_pol);
  dbg_printf("s.misc_output.vert_sync_pol = %u ",
            (unsigned) BX_VGA_THIS s.misc_output.vert_sync_pol);
  switch ( (BX_VGA_THIS s.misc_output.vert_sync_pol << 1) |
           BX_VGA_THIS s.misc_output.horiz_sync_pol ) {
    case 1: dbg_printf("(400 lines)\n"); break;
    case 2: dbg_printf("(350 lines)\n"); break;
    case 3: dbg_printf("(480 lines)\n"); break;
    default: dbg_printf("(reserved)\n");
    }

  dbg_printf("s.graphics_ctrl.odd_even = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.odd_even);
  dbg_printf("s.graphics_ctrl.chain_odd_even = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.chain_odd_even);
  dbg_printf("s.graphics_ctrl.shift_reg = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.shift_reg);
  dbg_printf("s.graphics_ctrl.graphics_alpha = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.graphics_alpha);
  dbg_printf("s.graphics_ctrl.memory_mapping = %u ",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.memory_mapping);
  switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
    case 1: dbg_printf("(A0000-AFFFF)\n"); break;
    case 2: dbg_printf("(B0000-B7FFF)\n"); break;
    case 3: dbg_printf("(B8000-BFFFF)\n"); break;
    default: dbg_printf("(A0000-BFFFF)\n"); break;
    }

  dbg_printf("s.sequencer.extended_mem = %u\n",
            (unsigned) BX_VGA_THIS s.sequencer.extended_mem);
  dbg_printf("s.sequencer.odd_even = %u (inverted)\n",
            (unsigned) BX_VGA_THIS s.sequencer.odd_even);
  dbg_printf("s.sequencer.chain_four = %u\n",
            (unsigned) BX_VGA_THIS s.sequencer.chain_four);

  dbg_printf("s.attribute_ctrl.video_enabled = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.video_enabled);
  dbg_printf("s.attribute_ctrl.mode_ctrl.graphics_alpha = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha);
  dbg_printf("s.attribute_ctrl.mode_ctrl.display_type = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type);
  dbg_printf("s.attribute_ctrl.mode_ctrl.internal_palette_size = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size);
  dbg_printf("s.attribute_ctrl.mode_ctrl.pixel_clock_select = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select);
#endif
}


  void
bx_vga_c::redraw_area(unsigned x0, unsigned y0, unsigned width,
                      unsigned height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1, xmax, ymax;

  if ((width == 0) || (height == 0)) {
    return;
  }

  BX_VGA_THIS s.vga_mem_updated = 1;

#if BX_SUPPORT_VBE
  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha || BX_VGA_THIS s.vbe_enabled) {
#else
  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
#endif
    // graphics mode
    xmax = old_iWidth;
    ymax = old_iHeight;
#if BX_SUPPORT_VBE
    if (BX_VGA_THIS s.vbe_enabled) {
      xmax = BX_VGA_THIS s.vbe_xres;
      ymax = BX_VGA_THIS s.vbe_yres;
    }
#endif
    xt0 = x0 / X_TILESIZE;
    yt0 = y0 / Y_TILESIZE;
    if (x0 < xmax) {
      xt1 = (x0 + width  - 1) / X_TILESIZE;
    } else {
      xt1 = (xmax - 1) / X_TILESIZE;
    }
    if (y0 < ymax) {
      yt1 = (y0 + height - 1) / Y_TILESIZE;
    } else {
      yt1 = (ymax - 1) / Y_TILESIZE;
    }
    for (yti=yt0; yti<=yt1; yti++) {
      for (xti=xt0; xti<=xt1; xti++) {
        SET_TILE_UPDATED (xti, yti, 1);
      }
    }

  } else {
    // text mode
    memset(BX_VGA_THIS s.text_snapshot, 0,
           sizeof(BX_VGA_THIS s.text_snapshot));
  }
}


#if BX_SUPPORT_VBE
  Bit8u  BX_CPP_AttrRegparmN(1)
bx_vga_c::vbe_mem_read(Bit32u addr)
{
  Bit32u offset;

  if (addr >= VBE_DISPI_LFB_PHYSICAL_ADDRESS)
  {
    // LFB read
    offset = addr - VBE_DISPI_LFB_PHYSICAL_ADDRESS;
  }
  else
  {
    // banked mode read
    offset = BX_VGA_THIS s.vbe_bank*65536 + addr - 0xA0000;
  }

  // check for out of memory read
  if (offset > VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)
    return 0;

  return (BX_VGA_THIS s.vbe_memory[offset]);
}

  void BX_CPP_AttrRegparmN(2)
bx_vga_c::vbe_mem_write(Bit32u addr, Bit8u value)
{
  Bit32u offset;
  unsigned x_tileno, y_tileno;

  if (BX_VGA_THIS s.vbe_lfb_enabled)
  {
    if (addr >= VBE_DISPI_LFB_PHYSICAL_ADDRESS)
    {
      // LFB write
      offset = addr - VBE_DISPI_LFB_PHYSICAL_ADDRESS;
    }
    else
    {
      // banked mode write while in LFB mode -> ignore
      return;
    }
  }
  else
  {
    if (addr < VBE_DISPI_LFB_PHYSICAL_ADDRESS)
    {
      // banked mode write
      offset = (BX_VGA_THIS s.vbe_bank*65536) + (addr - 0xA0000);
    }
    else
    {
      // LFB write while in banked mode -> ignore
      return;
    }
  }

  // check for out of memory write
  if (offset < VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)
  {
    BX_VGA_THIS s.vbe_memory[offset]=value;
  }
  else
  {
    // make sure we don't flood the logfile
    static int count=0;
    if (count<100)
    {
      count ++;
      BX_INFO(("VBE_mem_write out of video memory write at %x",offset));
    }
  }

  offset-=BX_VGA_THIS s.vbe_virtual_start;

  // only update the UI when writing 'onscreen'
  if (offset < BX_VGA_THIS s.vbe_visible_screen_size)
  {
    y_tileno = ((offset / BX_VGA_THIS s.vbe_bpp_multiplier) / BX_VGA_THIS s.vbe_virtual_xres) / Y_TILESIZE;
    x_tileno = ((offset / BX_VGA_THIS s.vbe_bpp_multiplier) % BX_VGA_THIS s.vbe_virtual_xres) / X_TILESIZE;

    if ((y_tileno < BX_NUM_Y_TILES) && (x_tileno < BX_NUM_X_TILES))
    {
      BX_VGA_THIS s.vga_mem_updated = 1;
      SET_TILE_UPDATED (x_tileno, y_tileno, 1);
    }
  }  
}

  Bit32u
bx_vga_c::vbe_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_VGA_SMF
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  return( class_ptr->vbe_read(address, io_len) );
}


  Bit32u
bx_vga_c::vbe_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_VGA_SMF
  Bit16u retval;

//  BX_INFO(("VBE_read %x (len %x)", address, io_len));

  if ((address==VBE_DISPI_IOPORT_INDEX) ||
      (address==VBE_DISPI_IOPORT_INDEX_OLD))
  {
    // index register
    return (Bit32u) BX_VGA_THIS s.vbe_curindex;
  }
  else
  {
    // data register read
    
    switch (BX_VGA_THIS s.vbe_curindex)
    {
      case VBE_DISPI_INDEX_ID: // Display Interface ID check
      {
        return BX_VGA_THIS s.vbe_cur_dispi;
      } break;
      
      case VBE_DISPI_INDEX_XRES: // x resolution
      {
        if (BX_VGA_THIS s.vbe_get_capabilities) {
          return BX_VGA_THIS s.vbe_max_xres;
        } else {
          return BX_VGA_THIS s.vbe_xres;
        }
      } break;

      case VBE_DISPI_INDEX_YRES: // y resolution
      {
        if (BX_VGA_THIS s.vbe_get_capabilities) {
          return BX_VGA_THIS s.vbe_max_yres;
        } else {
          return BX_VGA_THIS s.vbe_yres;
        }
      } break;
      
      case VBE_DISPI_INDEX_BPP: // bpp
      {
        if (BX_VGA_THIS s.vbe_get_capabilities) {
          return BX_VGA_THIS s.vbe_max_bpp;
        } else {
          return BX_VGA_THIS s.vbe_bpp;
        }
      } break;

      case VBE_DISPI_INDEX_ENABLE: // vbe enabled
      {
        retval = BX_VGA_THIS s.vbe_enabled;
	if (BX_VGA_THIS s.vbe_get_capabilities)
          retval |= VBE_DISPI_GETCAPS;
	if (BX_VGA_THIS s.vbe_8bit_dac)
          retval |= VBE_DISPI_8BIT_DAC;
        return retval;
      } break;
      
      case VBE_DISPI_INDEX_BANK: // current bank
      {
        return BX_VGA_THIS s.vbe_bank;
      } break;

      case VBE_DISPI_INDEX_X_OFFSET:
      {
      	return BX_VGA_THIS s.vbe_offset_x;
      } break;

      case VBE_DISPI_INDEX_Y_OFFSET:
      {
      	return BX_VGA_THIS s.vbe_offset_y;
      } break;

      case VBE_DISPI_INDEX_VIRT_WIDTH:
      {
      	return BX_VGA_THIS s.vbe_virtual_xres;
      	
      } break;
      
      case VBE_DISPI_INDEX_VIRT_HEIGHT:
      {
      	return BX_VGA_THIS s.vbe_virtual_yres;      	
      } break;
        

      default:
      {
        BX_PANIC(("VBE unknown data read index 0x%x",BX_VGA_THIS s.vbe_curindex));
      } break;
    }      
  }
  BX_PANIC(("VBE_read shouldn't reach this"));
  return 0; /* keep compiler happy */
}

  void
bx_vga_c::vbe_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_VGA_SMF
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;

  class_ptr->vbe_write(address, value, io_len);
}

  Bit32u
bx_vga_c::vbe_write(Bit32u address, Bit32u value, unsigned io_len)
{ 
#else
  UNUSED(this_ptr);
#endif  
  bx_bool new_vbe_8bit_dac;
  bx_bool needs_update = 0;
  unsigned i;

//  BX_INFO(("VBE_write %x = %x (len %x)", address, value, io_len));
  
  switch(address)
  {
    // index register    
    case VBE_DISPI_IOPORT_INDEX:
    // legacy index register    
    case VBE_DISPI_IOPORT_INDEX_OLD:

      BX_VGA_THIS s.vbe_curindex = (Bit16u) value;
      break;

    // data register
    // FIXME: maybe do some 'sanity' checks on received data?
    case VBE_DISPI_IOPORT_DATA:
    // legacy data register
    case VBE_DISPI_IOPORT_DATA_OLD:
      switch (BX_VGA_THIS s.vbe_curindex)
      {
        case VBE_DISPI_INDEX_ID: // Display Interface ID check
        {
          if ( (value == VBE_DISPI_ID0) ||
               (value == VBE_DISPI_ID1) ||
               (value == VBE_DISPI_ID2) ||
               (value == VBE_DISPI_ID3) )
          {
            // allow backwards compatible with previous dispi bioses
            BX_VGA_THIS s.vbe_cur_dispi=value;
          }
          else
          {
            BX_PANIC(("VBE unknown Display Interface %x",value));
          }

          // make sure we don't flood the logfile
          static int count=0;
          if (count < 100)
          {
            count++;
            BX_INFO(("VBE known Display Interface %x",value));
          }
        } break;

        case VBE_DISPI_INDEX_XRES: // set xres
        {
          // check that we don't set xres during vbe enabled
          if (!BX_VGA_THIS s.vbe_enabled)
          {
            // check for within max xres range
            if (value <= VBE_DISPI_MAX_XRES)
            {
              BX_VGA_THIS s.vbe_xres=(Bit16u) value;
              BX_INFO(("VBE set xres (%d)",value));
            }
            else
            {
              BX_INFO(("VBE set xres more then max xres (%d)",value));
            }
          }
          else
          {
            BX_INFO(("VBE set xres during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_YRES: // set yres
        {
          // check that we don't set yres during vbe enabled
          if (!BX_VGA_THIS s.vbe_enabled)
          {
            // check for within max yres range
            if (value <= VBE_DISPI_MAX_YRES)
            {
              BX_VGA_THIS s.vbe_yres=(Bit16u) value;
              BX_INFO(("VBE set yres (%d)",value));
            }
            else
            {
              BX_INFO(("VBE set yres more then max yres (%d)",value));
            }
          }
          else
          {
            BX_INFO(("VBE set yres during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_BPP: // set bpp
        {
          // check that we don't set bpp during vbe enabled
          if (!BX_VGA_THIS s.vbe_enabled)
          {
            // for backward compatiblity
            if (value == 0) value = VBE_DISPI_BPP_8;
            // check for correct bpp range
            if ((value == VBE_DISPI_BPP_4) || (value == VBE_DISPI_BPP_8) || (value == VBE_DISPI_BPP_15) ||
                (value == VBE_DISPI_BPP_16) || (value == VBE_DISPI_BPP_24) || (value == VBE_DISPI_BPP_32))
            {
              BX_VGA_THIS s.vbe_bpp=(Bit16u) value;
              BX_INFO(("VBE set bpp (%d)",value));
            }
            else
            {
              BX_INFO(("VBE set bpp with unknown bpp (%d)",value));
            }
          }
          else
          {
            BX_INFO(("VBE set bpp during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_BANK: // set bank
        {
          value=value & 0xff ; // FIXME lobyte = vbe bank A?
          unsigned divider = (BX_VGA_THIS s.vbe_bpp!=VBE_DISPI_BPP_4)?64:256;
          // check for max bank nr
          if (value < (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB / divider))
          {
            if (!BX_VGA_THIS s.vbe_lfb_enabled)
            {
              BX_DEBUG(("VBE set bank to %d", value));
              BX_VGA_THIS s.vbe_bank=value;
            }
            else
            {
              BX_ERROR(("VBE set bank in LFB mode ignored"));
            }
          }
          else
          {
            BX_INFO(("VBE set invalid bank (%d)",value));
          }
        } break;

        case VBE_DISPI_INDEX_ENABLE: // enable video
        {
          if ((value & VBE_DISPI_ENABLED) && !BX_VGA_THIS s.vbe_enabled)
          {
            unsigned depth=0;

            // setup virtual resolution to be the same as current reso      
            BX_VGA_THIS s.vbe_virtual_yres=BX_VGA_THIS s.vbe_yres;
            BX_VGA_THIS s.vbe_virtual_xres=BX_VGA_THIS s.vbe_xres;

            // reset offset
            BX_VGA_THIS s.vbe_offset_x=0;
            BX_VGA_THIS s.vbe_offset_y=0;
            BX_VGA_THIS s.vbe_virtual_start=0;

            switch((BX_VGA_THIS s.vbe_bpp))
            {
              // Default pixel sizes
              case VBE_DISPI_BPP_8: 
                BX_VGA_THIS s.vbe_bpp_multiplier = 1;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres;
                depth=8;
                break;

              case VBE_DISPI_BPP_4: 
                BX_VGA_THIS s.vbe_bpp_multiplier = 1;
                BX_VGA_THIS s.line_offset = (BX_VGA_THIS s.vbe_virtual_xres >> 3);
                depth=4;
                break;

              case VBE_DISPI_BPP_15:
                BX_VGA_THIS s.vbe_bpp_multiplier = 2; 
                BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres * 2;
                depth=15;
                break;        	          

              case VBE_DISPI_BPP_16:
                BX_VGA_THIS s.vbe_bpp_multiplier = 2; 
                BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres * 2;
                depth=16;
                break;        	          

              case VBE_DISPI_BPP_24: 
                BX_VGA_THIS s.vbe_bpp_multiplier = 3; 
                BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres * 3;
                depth=24;
                break;        	          

              case VBE_DISPI_BPP_32: 
                BX_VGA_THIS s.vbe_bpp_multiplier = 4; 
                BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres << 2;
                depth=32;
                break;        	          
            }
            BX_VGA_THIS s.vbe_visible_screen_size = BX_VGA_THIS s.line_offset * BX_VGA_THIS s.vbe_yres;

            BX_INFO(("VBE enabling x %d, y %d, bpp %d, %u bytes visible", BX_VGA_THIS s.vbe_xres, BX_VGA_THIS s.vbe_yres, BX_VGA_THIS s.vbe_bpp, BX_VGA_THIS s.vbe_visible_screen_size));

            if (depth > 4)
            {
              BX_VGA_THIS s.vbe_lfb_enabled=(bx_bool)(value & VBE_DISPI_LFB_ENABLED);
              if ((value & VBE_DISPI_NOCLEARMEM) == 0)
              {
                memset(BX_VGA_THIS s.vbe_memory, 0, BX_VGA_THIS s.vbe_visible_screen_size);
              }
              bx_gui->dimension_update(BX_VGA_THIS s.vbe_xres, BX_VGA_THIS s.vbe_yres, 0, 0, depth);
              BX_VGA_THIS s.last_bpp = depth;
              // some test applications expect these standard VGA settings
              BX_VGA_THIS s.CRTC.reg[9] = 0x00;
              BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha = 1;
              BX_VGA_THIS s.graphics_ctrl.memory_mapping = 1;
              BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select = 1;
              BX_VGA_THIS s.CRTC.reg[18] = (BX_VGA_THIS s.vbe_yres - 1) & 0xff;
              BX_VGA_THIS s.CRTC.reg[7] &= ~0x42;
              if ((BX_VGA_THIS s.vbe_yres - 1) & 0x0100) {
                BX_VGA_THIS s.CRTC.reg[7] |= 0x02;
              }
              if ((BX_VGA_THIS s.vbe_yres - 1) & 0x0200) {
                BX_VGA_THIS s.CRTC.reg[7] |= 0x40;
              }
              BX_VGA_THIS s.sequencer.map_mask = 0x0f;
              BX_VGA_THIS s.sequencer.chain_four = 1;
            }
          }
          else if (((value & VBE_DISPI_ENABLED) == 0) && BX_VGA_THIS s.vbe_enabled)
          {
            BX_INFO(("VBE disabling"));
            BX_VGA_THIS s.vbe_lfb_enabled=0;
          }     
          BX_VGA_THIS s.vbe_enabled=(bx_bool)(value & VBE_DISPI_ENABLED);
          BX_VGA_THIS s.vbe_get_capabilities=(bx_bool)((value & VBE_DISPI_GETCAPS) != 0);
          new_vbe_8bit_dac=(bx_bool)((value & VBE_DISPI_8BIT_DAC) != 0);
          if (new_vbe_8bit_dac != BX_VGA_THIS s.vbe_8bit_dac)
          {
            if (new_vbe_8bit_dac)
            {
              for (i=0; i<256; i++)
              {
                BX_VGA_THIS s.pel.data[i].red <<= 2;
                BX_VGA_THIS s.pel.data[i].green <<= 2;
                BX_VGA_THIS s.pel.data[i].blue <<= 2;
              }
              BX_INFO(("DAC in 8 bit mode"));
            }
            else
            {
              for (i=0; i<256; i++)
              {
                BX_VGA_THIS s.pel.data[i].red >>= 2;
                BX_VGA_THIS s.pel.data[i].green >>= 2;
                BX_VGA_THIS s.pel.data[i].blue >>= 2;
              }
              BX_INFO(("DAC in standard mode"));
            }
            BX_VGA_THIS s.vbe_8bit_dac=new_vbe_8bit_dac;
            needs_update = 1;
          }
        } break;

        case VBE_DISPI_INDEX_X_OFFSET:
        {
          // BX_INFO(("VBE offset x %x",value));
          BX_VGA_THIS s.vbe_offset_x=(Bit16u)value;

          BX_VGA_THIS s.vbe_virtual_start = BX_VGA_THIS s.vbe_offset_y * BX_VGA_THIS s.line_offset;
          if (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4) {
            BX_VGA_THIS s.vbe_virtual_start += (BX_VGA_THIS s.vbe_offset_x * BX_VGA_THIS s.vbe_bpp_multiplier);
          } else {
            BX_VGA_THIS s.vbe_virtual_start += (BX_VGA_THIS s.vbe_offset_x >> 3);
          }
          needs_update = 1;
        } break;

        case VBE_DISPI_INDEX_Y_OFFSET:
        {
          // BX_INFO(("VBE offset y %x",value));
          BX_VGA_THIS s.vbe_offset_y=(Bit16u)value;
          BX_VGA_THIS s.vbe_virtual_start = BX_VGA_THIS s.vbe_offset_y * BX_VGA_THIS s.line_offset;
          if (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4) {
            BX_VGA_THIS s.vbe_virtual_start += (BX_VGA_THIS s.vbe_offset_x * BX_VGA_THIS s.vbe_bpp_multiplier);
          } else {
            BX_VGA_THIS s.vbe_virtual_start += (BX_VGA_THIS s.vbe_offset_x >> 3);
          }
          needs_update = 1;
        } break;

        case VBE_DISPI_INDEX_VIRT_WIDTH:
        {
          BX_INFO(("VBE requested virtual width %d",value));

          // calculate virtual width & height dimensions
          // req:
          //   virt_width > xres
          //   virt_height >=yres
          //   virt_width*virt_height < MAX_VIDEO_MEMORY

          // basicly 2 situations

          // situation 1: 
          //   MAX_VIDEO_MEMORY / virt_width >= yres
          //        adjust result height
          //   else
          //        adjust result width based upon virt_height=yres
          Bit16u new_width=value;
          Bit16u new_height;
          if (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4) {
            new_height=(sizeof(BX_VGA_THIS s.vbe_memory) / BX_VGA_THIS s.vbe_bpp_multiplier) / new_width;
          } else {
            new_height=(sizeof(BX_VGA_THIS s.vbe_memory) * 2) / new_width;
          }
          if (new_height >=BX_VGA_THIS s.vbe_yres)
          {
            // we have a decent virtual width & new_height
            BX_INFO(("VBE decent virtual height %d",new_height));
          }
          else
          {
            // no decent virtual height: adjust width & height
            new_height=BX_VGA_THIS s.vbe_yres;
            if (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4) {
              new_width=(sizeof(BX_VGA_THIS s.vbe_memory) / BX_VGA_THIS s.vbe_bpp_multiplier) / new_height;
            } else {
              new_width=(sizeof(BX_VGA_THIS s.vbe_memory) * 2) / new_height;
            }

            BX_INFO(("VBE recalc virtual width %d height %d",new_width, new_height));
          }

          BX_VGA_THIS s.vbe_virtual_xres=new_width;
          BX_VGA_THIS s.vbe_virtual_yres=new_height;
          if (BX_VGA_THIS s.vbe_bpp != VBE_DISPI_BPP_4) {
            BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres * BX_VGA_THIS s.vbe_bpp_multiplier;
          } else {
            BX_VGA_THIS s.line_offset = BX_VGA_THIS s.vbe_virtual_xres >> 3;
          }
          BX_VGA_THIS s.vbe_visible_screen_size = BX_VGA_THIS s.line_offset * BX_VGA_THIS s.vbe_yres;

        } break;
	/*      
        case VBE_DISPI_INDEX_VIRT_HEIGHT:
        {
          BX_INFO(("VBE virtual height %x",value));

        } break;
	*/  
        default:
        {
          BX_PANIC(("VBE unknown data write index 0x%x",BX_VGA_THIS s.vbe_curindex));
        } break;      
      }        
      if (needs_update) {
        BX_VGA_THIS s.vga_mem_updated = 1;
        for (unsigned xti = 0; xti < BX_NUM_X_TILES; xti++) {
          for (unsigned yti = 0; yti < BX_NUM_Y_TILES; yti++) {
            SET_TILE_UPDATED (xti, yti, 1);
          }
        }
      }
      break;
	  
  } // end switch address
}

#endif
