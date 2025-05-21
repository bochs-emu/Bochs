/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2018-2025  The Bochs Project
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

// DDC support (returns the VESA EDID for the Bochs plug&play monitor)

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "gui/siminterface.h"
#include "gui/gui.h"
#include "ddc.h"
#include "param_names.h"

#define LOG_THIS

enum {
  DDC_STAGE_START,
  DDC_STAGE_ADDRESS,
  DDC_STAGE_RW,
  DDC_STAGE_DATA_IN,
  DDC_STAGE_DATA_OUT,
  DDC_STAGE_ACK_IN,
  DDC_STAGE_ACK_OUT,
  DDC_STAGE_STOP
};

const Bit8u vesa_EDID[128] = {
  0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
                                   /* 0x0000 8-byte header */
  0x04,0x21,                       /* 0x0008 Vendor ID ("AAA") */
  0xAB,0xCD,                       /* 0x000A Product ID */
  0x00,0x00,0x00,0x00,             /* 0x000C Serial number (none) */
  12, 11,                          /* 0x0010 Week of manufacture (12) and year of manufacture (2001) */
  0x01, 0x03,                      /* 0x0012 EDID version number (1.3) */
  0x0F,                            /* 0x0014 Video signal interface (analogue, 0.700 : 0.300 : 1.000 V p-p,
                                             Video Setup: Blank Level = Black Level, Separate Sync H & V Signals are
                                             supported, Composite Sync Signal on Horizontal is supported, Composite
                                             Sync Signal on Green Video is supported, Serration on the Vertical Sync
                                             is supported) */
  0x21,0x19,                       /* 0x0015 Scren size (330 mm * 250 mm) */
  0x78,                            /* 0x0017 Display gamma (2.2) */
  0x0F,                            /* 0x0018 Feature flags (no DMPS states, RGB, preferred timing mode, display is continuous frequency) */
  0x78,0xF5,                       /* 0x0019 Least significant bits for chromaticity and default white point */
  0xA6,0x55,0x48,0x9B,0x26,0x12,0x50,0x54,
                                   /* 0x001B Most significant bits for chromaticity and default white point */

  0xFF,                            /* 0x0023 Established timings 1 (720 x 400 @ 70Hz, 720 x 400 @ 88Hz,
                                             640 x 480 @ 60Hz, 640 x 480 @ 67Hz, 640 x 480 @ 72Hz, 640 x 480 @ 75Hz,
                                             800 x 600 @ 56Hz, 800 x 600 @ 60Hz) - historical resolutions */
  0xEF,                            /* 0x0024 Established timings 2 (800 x 600 @ 72Hz, 800 x 600 @ 75Hz, 832 x 624 @ 75Hz
                                             not 1024 x 768 @ 87Hz(I), 1024 x 768 @ 60Hz, 1024 x 768 @ 70Hz,
                                             1024 x 768 @ 75Hz, 1280 x 1024 @ 75Hz) - historical resolutions */
  0x80,                            /* 0x0025 Established timings 2 (1152 x 870 @ 75Hz and no manufacturer timings) */

                                   /* Standard timing */
                                   /* First byte: X resolution, divided by 8, less 31 (256–2288 pixels) */
                                   /* bit 7-6, X:Y pixel ratio: 00=16:10; 01=4:3; 10=5:4; 11=16:9 */
                                   /* bit 5-0, Vertical frequency, less 60 (60–123 Hz), nop 01 01 */
  0x31, 0x59,                      /* 0x0026 Standard timing #1 (640 x 480 @ 85 Hz) */
  0x45, 0x59,                      /* 0x0028 Standard timing #2 (800 x 600 @ 85 Hz) */
  0x61, 0x59,                      /* 0x002A Standard timing #3 (1024 x 768 @ 85 Hz) */
  0x81, 0xCA,                      /* 0x002C Standard timing #4 (1280 x 720 @ 70 Hz) */
  0x81, 0x0A,                      /* 0x002E Standard timing #5 (1280 x 800 @ 70 Hz) */
  0xA9, 0xC0,                      /* 0x0030 Standard timing #6 (1600 x 900 @ 60 Hz) */
  0xA9, 0x40,                      /* 0x0032 Standard timing #7 (1600 x 1200 @ 60 Hz) */
  0xD1, 0x00,                      /* 0x0034 Standard timing #8 (1920 x 1080 @ 60 Hz) */

                                   /* 0x0036 First 18-byte descriptor (1920 x 1200) */
  0x28, 0x3C,                      /*        Pixel clock = 154000000 Hz */
  0x80,                            /* 0x0038 Horizontal addressable pixels low byte (0x0780 & 0xFF) */
  0xA0,                            /* 0x0039 Horizontal blanking low byte (0x00A0 & 0xFF) */
  0x70,                            /* 0x003A Horizontal addressable pixels high 4 bits ((0x0780 & 0x0F00) >> 4), and */
                                   /*        Horizontal blanking high 4 bits ((0x00A0 & 0x0F00 ) >> 8) as low bits */
  0xB0,                            /* 0x003B Vertical addressable pixels low byte (0x04B0 & 0xFF) */
  0x23,                            /* 0x003C Vertical blanking low byte (0x0023 & 0xFF) */
  0x40,                            /* 0x003D Vertical addressable pixels high 4 bits ((0x04B0 & 0x0F00) >> 4), and */
                                   /*        Vertical blanking high 4 bits ((0x0024 & x0F00) >> 8) */
  0x30,                            /* 0x003E Horizontal front porch in pixels low byte (0x0030 & 0xFF) */
  0x20,                            /* 0x003F Horizontal sync pulse width in pixels low byte (0x0020 & 0xFF) */
  0x36,                            /* 0x0040 Vertical front porch in lines low 4 bits ((0x0003 & 0x0F) << 4), and */
                                   /*        Vertical sync pulse width in lines low 4 bits (0x0006 & 0x0F) */
  0x00,                            /* 0x0041 Horizontal front porch pixels high 2 bits (0x0030 >> 8), and */
                                   /*        Horizontal sync pulse width in pixels high 2 bits (0x0020 >> 8), and */
                                   /*        Vertical front porch in lines high 2 bits (0x0003 >> 4), and */
                                   /*        Vertical sync pulse width in lines high 2 bits (0x0006 >> 4) */
  0x06,                            /* 0x0042 Horizontal addressable video image size in mm low 8 bits (0x0206 & 0xFF) */
  0x44,                            /* 0x0043 Vertical addressable video image size in mm low 8 bits (0x0144 & 0xFF) */
  0x21,                            /* 0x0044 Horizontal addressable video image size in mm high 8 bits (0x0206 >> 8), and */
                                   /*        Vertical addressable video image size in mm high 8 bits (0x0144 >> 8) */
  0x00,                            /* 0x0045 Left and right border size in pixels (0x00) */
  0x00,                            /* 0x0046 Top and bottom border size in lines (0x00) */
  0x1E,                            /* 0x0047 Flags (non-interlaced, no stereo, analog composite sync, sync on */
                                   /*        all three (RGB) video signals) */

  0x00,0x00,0x00,0xFF,0x00,        /* 0x0048 Second 18-byte descriptor - display product serial number */
  '0','1','2','3','4','5','6','7','8','9',
  0x0A,0x20,0x20,

  0x00,0x00,0x00,0xFC,0x00,        /* 0x005A Third 18-byte descriptor - display product name  */
  'B','o','c','h','s',' ','S','c','r','e','e','n',
  0x0A,

  0x00,0x00,0x00,0x10,0x00,0x00,   /* 0x006C Fourth 18-byte descriptor (unused) */
  0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,                            /* 0x007E Extension block count (none)  */
  0x00,                            /* 0x007F Checksum (set by constructor) */
};

const Bit8u desc_1280x1024[18] = {
                                   /* 0x0036 First 18-byte descriptor (1280 x 1024) */
  0x30, 0x2a,                      /*        Pixel clock = 108000000 Hz */
  0x00,                            /* 0x0038 Horizontal addressable pixels low byte (0x0500 & 0xFF) */
  0x98,                            /* 0x0039 Horizontal blanking low byte (0x0198 & 0xFF) */
  0x51,                            /* 0x003A Horizontal addressable pixels high 4 bits (0x0500 >> 8), and */
                                   /*        Horizontal blanking high 4 bits (0x0198 >> 8) */
  0x00,                            /* 0x003B Vertical addressable pixels low byte (0x0400 & 0xFF) */
  0x2A,                            /* 0x003C Vertical blanking low byte (0x002A & 0xFF) */
  0x40,                            /* 0x003D Vertical addressable pixels high 4 bits (0x0400 >> 8), and */
                                   /*        Vertical blanking high 4 bits (0x002A >> 8) */
  0x30,                            /* 0x003E Horizontal front porch in pixels low byte (0x0030 & 0xFF) */
  0x70,                            /* 0x003F Horizontal sync pulse width in pixels low byte (0x0070 & 0xFF) */
  0x13,                            /* 0x0040 Vertical front porch in lines low 4 bits (0x0001 & 0x0F), and */
                                   /*        Vertical sync pulse width in lines low 4 bits (0x0003 & 0x0F) */
  0x00,                            /* 0x0041 Horizontal front porch pixels high 2 bits (0x0030 >> 8), and */
                                   /*        Horizontal sync pulse width in pixels high 2 bits (0x0070 >> 8), and */
                                   /*        Vertical front porch in lines high 2 bits (0x0001 >> 4), and */
                                   /*        Vertical sync pulse width in lines high 2 bits (0x0003 >> 4) */
  0x2C,                            /* 0x0042 Horizontal addressable video image size in mm low 8 bits (0x012C & 0xFF) */
  0xE1,                            /* 0x0043 Vertical addressable video image size in mm low 8 bits (0x00E1 & 0xFF) */
  0x10,                            /* 0x0044 Horizontal addressable video image size in mm high 8 bits (0x012C >> 8), and */
                                   /*        Vertical addressable video image size in mm high 8 bits (0x00E1 >> 8) */
  0x00,                            /* 0x0045 Left and right border size in pixels (0x00) */
  0x00,                            /* 0x0046 Top and bottom border size in lines (0x00) */
  0x1E,                            /* 0x0047 Flags (non-interlaced, no stereo, analog composite sync, sync on */
                                   /*        all three (RGB) video signals) */
};

const Bit8u desc_1024x768[18] = {
                                   /* 0x0036 First 18-byte descriptor (1024 x 768) */
  0x64, 0x19,                      /*        Pixel clock = 65000000 Hz */
  0x00,                            /* 0x0038 Horizontal addressable pixels low byte (0x0500 & 0xFF) */
  0x40,                            /* 0x0039 Horizontal blanking low byte (0x0198 & 0xFF) */
  0x41,                            /* 0x003A Horizontal addressable pixels high 4 bits (0x0500 >> 8), and */
                                   /*        Horizontal blanking high 4 bits (0x0198 >> 8) */
  0x00,                            /* 0x003B Vertical addressable pixels low byte (0x0400 & 0xFF) */
  0x26,                            /* 0x003C Vertical blanking low byte (0x002A & 0xFF) */
  0x30,                            /* 0x003D Vertical addressable pixels high 4 bits (0x0400 >> 8), and */
                                   /*        Vertical blanking high 4 bits (0x002A >> 8) */
  0x18,                            /* 0x003E Horizontal front porch in pixels low byte (0x0030 & 0xFF) */
  0x88,                            /* 0x003F Horizontal sync pulse width in pixels low byte (0x0070 & 0xFF) */
  0x36,                            /* 0x0040 Vertical front porch in lines low 4 bits (0x0001 & 0x0F), and */
                                   /*        Vertical sync pulse width in lines low 4 bits (0x0003 & 0x0F) */
  0x00,                            /* 0x0041 Horizontal front porch pixels high 2 bits (0x0030 >> 8), and */
                                   /*        Horizontal sync pulse width in pixels high 2 bits (0x0070 >> 8), and */
                                   /*        Vertical front porch in lines high 2 bits (0x0001 >> 4), and */
                                   /*        Vertical sync pulse width in lines high 2 bits (0x0003 >> 4) */
  0x30,                            /* 0x0042 Horizontal addressable video image size in mm low 8 bits (0x012C & 0xFF) */
  0xE4,                            /* 0x0043 Vertical addressable video image size in mm low 8 bits (0x00E1 & 0xFF) */
  0x10,                            /* 0x0044 Horizontal addressable video image size in mm high 8 bits (0x012C >> 8), and */
                                   /*        Vertical addressable video image size in mm high 8 bits (0x00E1 >> 8) */
  0x00,                            /* 0x0045 Left and right border size in pixels (0x00) */
  0x00,                            /* 0x0046 Top and bottom border size in lines (0x00) */
  0x18,                            /* 0x0047 Flags (non-interlaced, no stereo, analog composite sync, sync on */
                                   /*        all three (RGB) video signals) */
};

bx_ddc_c::bx_ddc_c(void)
{
  put("DDC");
  memset(&s, 0, sizeof(s));
}

bx_ddc_c::~bx_ddc_c(void)
{
}

void bx_ddc_c::init()
{
  int fd, ret;
  struct stat stat_buf;
  const char *path;
  Bit8u checksum = 0;
  Bit16u max_xres, max_yres, max_bpp;

  s.DCKhost = 1;
  s.DDAhost = 1;
  s.DDAmon = 1;
  s.ddc_stage = DDC_STAGE_STOP;
  s.ddc_ack = 1;
  s.ddc_rw = 1;
  s.edid_index = 0;
  s.ddc_mode = SIM->get_param_enum(BXPN_DDC_MODE)->get();
  if ((s.ddc_mode == BX_DDC_MODE_BUILTIN) || (s.ddc_mode == BX_DDC_MODE_BUILTIN_GUI)) {
    memcpy(s.edid_data, vesa_EDID, 128);
    if (s.ddc_mode == BX_DDC_MODE_BUILTIN_GUI) {
      bx_gui->get_capabilities(&max_xres, &max_yres, &max_bpp);
      if ((max_xres <= 1920) || (max_yres <= 1200)) {
        // entries disabled
        s.edid_data[0x30] = 0x01;
        s.edid_data[0x31] = 0x01;
        s.edid_data[0x32] = 0x01;
        s.edid_data[0x33] = 0x01;
        s.edid_data[0x34] = 0x01;
        s.edid_data[0x35] = 0x01;
        if ((max_xres <= 1280) || (max_yres <= 1024)) {
          s.edid_data[0x24] = 0xCE;
          s.edid_data[0x2C] = 0x01;
          s.edid_data[0x2D] = 0x01;
          s.edid_data[0x2E] = 0x01;
          s.edid_data[0x2F] = 0x01;
          memcpy(&s.edid_data[0x36], desc_1024x768, 18);
          BX_INFO(("Using EDID for resolution 1024x768"));
        } else {
          memcpy(&s.edid_data[0x36], desc_1280x1024, 18);
          BX_INFO(("Using EDID for resolution 1280x1024"));
        }
      }
    } else {
      BX_INFO(("Using default EDID for resolution 1920x1200"));
    }
    s.edid_extblock = 0;
  } else if (s.ddc_mode == BX_DDC_MODE_FILE) {
    path = SIM->get_param_string(BXPN_DDC_FILE)->getptr();
    fd = open(path, O_RDONLY
#ifdef O_BINARY
       | O_BINARY
#endif
        );
    if (fd < 0) {
      BX_PANIC(("failed to open monitor EDID file '%s'", path));
    }
    ret = fstat(fd, &stat_buf);
    if (ret) {
      BX_PANIC(("could not fstat() monitor EDID file."));
    }
    if ((stat_buf.st_size != 128) && (stat_buf.st_size != 256)) {
      BX_PANIC(("monitor EDID file size must be 128 or 256 bytes"));
    } else {
      s.edid_extblock = (stat_buf.st_size == 256);
    }
    ret = ::read(fd, (bx_ptr_t) s.edid_data, (unsigned)stat_buf.st_size);
    if (ret != stat_buf.st_size) {
      BX_PANIC(("error reading monitor EDID file."));
    }
    close(fd);
    BX_INFO(("Monitor EDID read from image file '%s'.", path));
  }
  s.edid_data[127] = 0;
  for (int i = 0; i < 128; i++) {
    checksum += s.edid_data[i];
  }
  if (checksum != 0) {
    s.edid_data[127] = (Bit8u)-checksum;
  }
}

Bit8u bx_ddc_c::read()
{
  Bit8u retval = (Bit8u)(((s.DDAmon & s.DDAhost) << 3) | (s.DCKhost << 2) |
                         (s.DDAhost << 1) | (Bit8u)s.DCKhost);
  return retval;
}

void bx_ddc_c::write(bool dck, bool dda)
{
  bool dck_change = 0;
  bool dda_change = 0;

  if (s.ddc_mode == BX_DDC_MODE_DISABLED)
    return;

  if ((dck != s.DCKhost) || (dda != s.DDAhost)) {
    dck_change = (dck != s.DCKhost);
    dda_change = (dda != s.DDAhost);
    if (dck_change && dda_change) {
      BX_ERROR(("DDC unknown: DCK=%d DDA=%d", dck, dda));
    } else if (dck_change) {
      if (!dck) {
        switch (s.ddc_stage) {
          case DDC_STAGE_START:
            s.ddc_stage = DDC_STAGE_ADDRESS;
            s.ddc_bitshift = 6;
            s.ddc_byte = 0;
            break;
          case DDC_STAGE_ADDRESS:
            if (s.ddc_bitshift > 0) {
              s.ddc_bitshift--;
            } else {
              s.ddc_ack = !(s.ddc_byte == 0x50);
              BX_DEBUG(("Address = 0x%02x", s.ddc_byte));
              s.ddc_stage = DDC_STAGE_RW;
            }
            break;
          case DDC_STAGE_RW:
            BX_DEBUG(("R/W mode = %d", s.ddc_rw));
            s.ddc_stage = DDC_STAGE_ACK_OUT;
            s.DDAmon = s.ddc_ack;
            break;
          case DDC_STAGE_DATA_IN:
            if (s.ddc_bitshift > 0) {
              s.ddc_bitshift--;
            } else {
              s.ddc_ack = 0;
              BX_DEBUG(("Data = 0x%02x (setting offset address)", s.ddc_byte));
              s.edid_index = s.ddc_byte;
              s.DDAmon = s.ddc_ack;
              s.ddc_stage = DDC_STAGE_ACK_OUT;
            }
            break;
          case DDC_STAGE_DATA_OUT:
            if (s.ddc_bitshift > 0) {
              s.ddc_bitshift--;
              s.DDAmon = ((s.ddc_byte >> s.ddc_bitshift) & 1);
            } else {
              s.ddc_stage = DDC_STAGE_ACK_IN;
              s.DDAmon = 1;
            }
            break;
          case DDC_STAGE_ACK_IN:
            BX_DEBUG(("Received status %s", s.ddc_ack ? "NAK":"ACK"));
            if (s.ddc_ack == 0) {
              s.ddc_bitshift = 7;
              s.ddc_stage = DDC_STAGE_DATA_OUT;
              s.ddc_byte = get_edid_byte();
              s.DDAmon = ((s.ddc_byte >> s.ddc_bitshift) & 1);
            } else {
              s.ddc_stage = DDC_STAGE_STOP;
            }
            break;
          case DDC_STAGE_ACK_OUT:
            BX_DEBUG(("Sent status %s", s.ddc_ack ? "NAK":"ACK"));
            s.ddc_bitshift = 7;
            if (s.ddc_rw) {
              s.ddc_stage = DDC_STAGE_DATA_OUT;
              s.ddc_byte = get_edid_byte();
              s.DDAmon = ((s.ddc_byte >> s.ddc_bitshift) & 1);
            } else {
              s.ddc_stage = DDC_STAGE_DATA_IN;
              s.DDAmon = 1;
              s.ddc_byte = 0;
            }
            break;
        }
      } else {
        switch (s.ddc_stage) {
          case DDC_STAGE_ADDRESS:
          case DDC_STAGE_DATA_IN:
            s.ddc_byte |= (Bit8u)(s.DDAhost << s.ddc_bitshift);
            break;
          case DDC_STAGE_RW:
            s.ddc_rw = s.DDAhost;
            break;
          case DDC_STAGE_ACK_IN:
            s.ddc_ack = s.DDAhost;
            break;
        }
      }
    } else {
      if (s.DCKhost) {
        if (!dda) {
          s.ddc_stage = DDC_STAGE_START;
          BX_DEBUG(("Start detected"));
        } else {
          s.ddc_stage = DDC_STAGE_STOP;
          BX_DEBUG(("Stop detected"));
        }
      }
    }
    s.DCKhost = dck;
    s.DDAhost = dda;
  }
}

Bit8u bx_ddc_c::get_edid_byte()
{
  Bit8u value = s.edid_data[s.edid_index++];
  BX_DEBUG(("Sending EDID byte 0x%02x (value = 0x%02x)", s.edid_index - 1, value));
  if (!s.edid_extblock) s.edid_index &= 0x7f;
  return value;
}
