/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2023  The Bochs Project
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

//
// Floppy Disk Controller Docs:
// Intel 82077A Data sheet
//   http://www.buchty.net/casio/files/82077.pdf
// Intel 82078AA Data sheet
//   https://wiki.qemu.org/images/f/f0/29047403.pdf
//
//
// Notes by Benjamin Lunt, 15 Nov 2023
//  This emulation should emulate almost all commands of the FDC.
//  This emulation should emulate numerous controllers, each having
//   a different combination of supported commands as well as a few
//   added quirks. (See notes below on adding a new controller)
//  The READ DELETED DATA command is implemented as exactly the same
//   as the READ NORMAL DATA since there is no way to store deleted
//   data on the emualted image file.
//  The WRITE DELETED DATA command is *not* implemented, since, again,
//   there is no way to store deleted data on the emulated image file.
//  The MODE command, though only supported on two somewhat obscure
//   controllers, is not fully implemented. Some more work needs to be done.
//  The three SCAN commands are rarely used, and are not very well documented.
//   Therefore, these three commands may or may not be fully functional.
//   I have done my best to make them compatible with supported hardware.
//  The VERIFY command, under normal operation, simply returns successful 
//   since the emulated image file should always return valid data written.
//   If not, there are more problems than this emulation...
//  The POWER_DOWN command does not wait to auto power down if the PD_FLAGS_AUTO_PD
//   flag is set. We automatically go to power saving mode. Only a reset can awake
//   the controller. (DOR bit 2, or hardware reset)
//  Since a controller will not support both the POWER_DOWN and SET_STANDBY commands,
//   we can code both functions without a chance of 's.power_down' or 's.standby'
//   interacting with each other. i.e.: if one is used, the other will be zero.
//  We do not support the 8272A, 82072, or NEC's 765 / 72065 since their register interface is not
//   compatible with the remaining other controllers. They only have two/three registers,
//   a main status register, a data register, and the 82072 has a DSR.
//
// Adding a new controller:
//  It is a simple task to add a new controller. 
//   1) Add a new entry to the FDC_TYPE_xxxxxx list below, making sure 
//      to use an unused bit.
//   2) Add this newly created FDC_TYPE_xxxxxx to the 'supported' member of
//      the 'fdc_type_supported' declaration to any commands this new
//      controller supports.
//   3) Add any 'quirks' to any of the command functions.
//      For example, if your new controller had a quirk for a command, you
//       wrap this new code like so:
//      #if (FDC_CURRENT_TYPE & FDC_TYPE_xxxxxx)
//         code only associated to this controller
//      #endif
//      If two or more controllers have this quirk, simply use:
//      #if (FDC_CURRENT_TYPE & (FDC_TYPE_xxxxxx0 | FDC_TYPE_xxxxxx1))
// 
//  For example, let's say I wanted to add the FDC_TYPE_BOCHS controller to 
//   the emulation (I have already added it so you can see how it is done):
//    Add FDC_TYPE_BOCHS as: (please choose the next available bit)
//      #define FDC_TYPE_BOCHS     (1 << 5)
//    Then in the 'fdc_type_supported' declaration, if the new controller
//     supports the FD_CMD_MODE command, add the following:
//     { FD_CMD_MODE,                 0x00,
//         FDC_TYPE_DP8473 | FDC_TYPE_PC87306 | FDC_TYPE_BOCHS },
//    Add ' | FDC_TYPE_BOCHS ' to any remaining commands listed that this
//     new controller supports.
//
//  To choose which controller to emulate, use the following line:
//    #define FDC_CURRENT_TYPE  FDC_TYPE_82078
//  In a future update, I will make this so that the user can choose via
//   the bochrc.txt file instead.
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

extern "C" {
#include <errno.h>
#include <stdlib.h> // rand()
}

#ifdef __linux__
extern "C" {
#include <sys/ioctl.h>
#include <linux/fd.h>
}
#endif
#include "iodev.h"
#include "hdimage/hdimage.h"

// The controller types supported. Choose one of the seven below.
#define FDC_TYPE_NONE      (0 << 0) // used to indicate that no (included) controller supports this command
#define FDC_TYPE_82077AA   (1 << 0) // this is the default type to use
#define FDC_TYPE_82078     (1 << 1)
    #define FDC_TYPE_82078SL     1  // define this to 1 if you use FDC_TYPE_82078 and want the FDC_TYPE_82078SL version
#define FDC_TYPE_37c78     (1 << 2)
#define FDC_TYPE_DP8473    (1 << 3)
#define FDC_TYPE_PC87306   (1 << 4)
#define FDC_TYPE_BOCHS     (1 << 5) // supports all available commands

// the selected controller to emulate
//#define FDC_CURRENT_TYPE  FDC_TYPE_82078

#ifndef FDC_CURRENT_TYPE
  #define FDC_CURRENT_TYPE  FDC_TYPE_82077AA  // default to the 82077AA
#endif

// Some software, namely omnidisk (http://www.shlock.co.uk/Utils/OmniDisk/OmniDisk.htm)
//  likes to read the id (READ_ID) consecutively to see if the sectors are consecutively
//  in order. Set this to 1 for such software. Else, we return a random sector number
//  between 1 and SPT.
#define FDC_FOR_OMNIDISK  1

// set this to 1 to ignore the BX_DEBUGs of Port 0x3F6
// the harddrive emulation will flood the Floppy debug log file if set to 0
#define IGNORE_DEBUG_03F6  1

#include "floppy.h"
// windows.h included by bochs.h
#ifdef WIN32
extern "C" {
#include <winioctl.h>
}
#endif
#define LOG_THIS theFloppyController->

bx_floppy_ctrl_c *theFloppyController;

typedef struct {
  unsigned id;
  Bit8u trk;
  Bit8u hd;
  Bit8u spt;
  unsigned sectors;
  Bit8u drive_mask;
} floppy_type_t;

static floppy_type_t floppy_type[8] = {
  { BX_FLOPPY_160K, 40, 1, 8, 320, 0x03 },
  { BX_FLOPPY_180K, 40, 1, 9, 360, 0x03 },
  { BX_FLOPPY_320K, 40, 2, 8, 640, 0x03 },
  { BX_FLOPPY_360K, 40, 2, 9, 720, 0x03 },
  { BX_FLOPPY_720K, 80, 2, 9, 1440, 0x1f },
  { BX_FLOPPY_1_2,  80, 2, 15, 2400, 0x02 },
  { BX_FLOPPY_1_44, 80, 2, 18, 2880, 0x18 },
  { BX_FLOPPY_2_88, 80, 2, 36, 5760, 0x10 }
};

static Bit16u drate_in_k[4] = {
  500, 300, 250, 1000
};

PLUGIN_ENTRY_FOR_MODULE(floppy)
{
  if (mode == PLUGIN_INIT) {
    theFloppyController = new bx_floppy_ctrl_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theFloppyController, BX_PLUGIN_FLOPPY);
  } else if (mode == PLUGIN_FINI) {
    delete theFloppyController;
  } else if (mode == PLUGIN_PROBE){
    return (int)PLUGTYPE_CORE;
  }
  return 0; // Success
}

bx_floppy_ctrl_c::bx_floppy_ctrl_c()
{
  put("FLOPPY");
  memset(&s, 0, sizeof(s));
  s.floppy_timer_index = BX_NULL_TIMER_HANDLE;
  s.statusbar_id[0] = -1;
  s.statusbar_id[1] = -1;
  s.rt_conf_id = -1;
}

bx_floppy_ctrl_c::~bx_floppy_ctrl_c()
{
  char pname[10];

  SIM->unregister_runtime_config_handler(s.rt_conf_id);
  for (int i = 0; i < 2; i++) {
    close_media(&BX_FD_THIS s.media[i]);
    sprintf(pname, "floppy.%d", i);
    bx_list_c *floppy = (bx_list_c*)SIM->get_param(pname);
    SIM->get_param_string("path", floppy)->set_handler(NULL);
    SIM->get_param_bool("readonly", floppy)->set_handler(NULL);
    SIM->get_param_enum("status", floppy)->set_handler(NULL);
  }
  SIM->get_bochs_root()->remove("floppy");
  BX_DEBUG(("Exit"));
}

void bx_floppy_ctrl_c::init(void)
{
  DEV_dma_register_8bit_channel(2, dma_read, dma_write, "Floppy Drive");
  DEV_register_irq(6, "Floppy Drive");
  for (unsigned addr=0x03F0; addr<=0x03F7; addr++) {
    DEV_register_ioread_handler(this, read_handler, addr, "Floppy Drive", 1);
    DEV_register_iowrite_handler(this, write_handler, addr, "Floppy Drive", 1);
  }

  Bit8u cmos_value = 0x00; /* start out with: no drive 0, no drive 1 */

  BX_FD_THIS s.num_supported_floppies = 0;

  for (int i=0; i<4; i++) {
    BX_FD_THIS s.media[i].type              = BX_FLOPPY_NONE;
    BX_FD_THIS s.media[i].sectors_per_track = 0;
    BX_FD_THIS s.media[i].tracks            = 0;
    BX_FD_THIS s.media[i].heads             = 0;
    BX_FD_THIS s.media[i].sectors           = 0;
    BX_FD_THIS s.media[i].fd                = -1;
    BX_FD_THIS s.media[i].vvfat_floppy      = 0;
    BX_FD_THIS s.media[i].status_changed    = 0;
    BX_FD_THIS s.media_present[i]           = 0;
    BX_FD_THIS s.device_type[i]             = FDRIVE_NONE;
  }

  //
  // Floppy A setup
  //
  bx_list_c *floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYA);
  Bit8u devtype = SIM->get_param_enum("devtype", floppy)->get();
  cmos_value = (devtype << 4);
  if (devtype != BX_FDD_NONE) {
    BX_FD_THIS s.device_type[0] = 1 << (devtype - 1);
    BX_FD_THIS s.num_supported_floppies++;
    BX_FD_THIS s.statusbar_id[0] = bx_gui->register_statusitem(" A: ");
  } else {
    BX_FD_THIS s.statusbar_id[0] = -1;
  }

  if (SIM->get_param_enum("type", floppy)->get() != BX_FLOPPY_NONE) {
    if (SIM->get_param_enum("status", floppy)->get() == BX_INSERTED) {
      BX_FD_THIS s.media[0].write_protected = SIM->get_param_bool("readonly", floppy)->get();
      if (evaluate_media(BX_FD_THIS s.device_type[0], SIM->get_param_enum("type", floppy)->get(),
                         SIM->get_param_string("path", floppy)->getptr(), & BX_FD_THIS s.media[0])) {
        BX_FD_THIS s.media_present[0] = 1;
#define MED (BX_FD_THIS s.media[0])
        BX_INFO(("fd0: '%s' ro=%d, h=%d,t=%d,spt=%d",
          SIM->get_param_string("path", floppy)->getptr(),
          MED.write_protected, MED.heads, MED.tracks, MED.sectors_per_track));
        if (MED.write_protected)
          SIM->get_param_bool("readonly", floppy)->set(1);
#undef MED
      } else {
        SIM->get_param_enum("status", floppy)->set(BX_EJECTED);
      }
    }
  }

  //
  // Floppy B setup
  //
  floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYB);
  devtype = SIM->get_param_enum("devtype", floppy)->get();
  cmos_value |= devtype;
  if (devtype != BX_FDD_NONE) {
    BX_FD_THIS s.device_type[1] = 1 << (devtype - 1);
    BX_FD_THIS s.num_supported_floppies++;
    BX_FD_THIS s.statusbar_id[1] = bx_gui->register_statusitem(" B: ");
  } else {
    BX_FD_THIS s.statusbar_id[1] = -1;
  }

  if (SIM->get_param_enum("type", floppy)->get() != BX_FLOPPY_NONE) {
    if (SIM->get_param_enum("status", floppy)->get() == BX_INSERTED) {
      BX_FD_THIS s.media[1].write_protected = SIM->get_param_bool("readonly", floppy)->get();
      if (evaluate_media(BX_FD_THIS s.device_type[1], SIM->get_param_enum("type", floppy)->get(),
                         SIM->get_param_string("path", floppy)->getptr(), & BX_FD_THIS s.media[1])) {
        BX_FD_THIS s.media_present[1] = 1;
#define MED (BX_FD_THIS s.media[1])
        BX_INFO(("fd1: '%s' ro=%d, h=%d,t=%d,spt=%d",
          SIM->get_param_string("path", floppy)->getptr(),
          MED.write_protected, MED.heads, MED.tracks, MED.sectors_per_track));
        if (MED.write_protected)
          SIM->get_param_bool("readonly", floppy)->set(1);
#undef MED
      } else {
        SIM->get_param_enum("status", floppy)->set(BX_EJECTED);
      }
    }
  }

  // generate CMOS values for floppy and boot sequence if not using a CMOS image
  if (!SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->get()) {
    /* CMOS Floppy Type and Equipment Byte register */
    DEV_cmos_set_reg(0x10, cmos_value);
    if (BX_FD_THIS s.num_supported_floppies > 0) {
      DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0x3e) |
                             ((BX_FD_THIS s.num_supported_floppies-1) << 6) | 1);
    } else {
      DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0x3e));
    }

    // Set the "non-extended" boot device (first floppy or first hard disk).
    if (SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() != BX_BOOT_FLOPPYA) {
      // system boot sequence C:, A:
      DEV_cmos_set_reg(0x2d, DEV_cmos_get_reg(0x2d) & 0xdf);
    } else { // 'a'
      // system boot sequence A:, C:
      DEV_cmos_set_reg(0x2d, DEV_cmos_get_reg(0x2d) | 0x20);
    }

    // Set the "extended" boot sequence, bytes 0x38 and 0x3D (needed for cdrom booting)
    BX_INFO(("Using boot sequence %s, %s, %s",
             SIM->get_param_enum(BXPN_BOOTDRIVE1)->get_selected(),
             SIM->get_param_enum(BXPN_BOOTDRIVE2)->get_selected(),
             SIM->get_param_enum(BXPN_BOOTDRIVE3)->get_selected()));
    DEV_cmos_set_reg(0x3d, SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() |
                           (SIM->get_param_enum(BXPN_BOOTDRIVE2)->get() << 4));

    // Set the signature check flag in cmos, inverted for compatibility
    DEV_cmos_set_reg(0x38, SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->get() |
                           (SIM->get_param_enum(BXPN_BOOTDRIVE3)->get() << 4));
    BX_INFO(("Floppy boot signature check is %sabled",
             SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->get() ? "dis" : "en"));
  }

  if (BX_FD_THIS s.floppy_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_FD_THIS s.floppy_timer_index =
      DEV_register_timer(this, timer_handler, 250, 0, 0, "floppy");
  }
  /* phase out s.non_dma in favor of using FD_MS_NDMA, more like hardware */
  BX_FD_THIS s.main_status_reg &= ~FD_MS_NDMA;  // enable DMA from start
  /* these registers are not cleared by reset */
  BX_FD_THIS s.SRT = 0;
  BX_FD_THIS s.HUT = 0;
  BX_FD_THIS s.HLT = 0;

  // runtime parameters
  char pname[10];
  for (int i = 0; i < 2; i++) {
    sprintf(pname, "floppy.%d", i);
    bx_list_c *floppy = (bx_list_c*)SIM->get_param(pname);
    SIM->get_param_string("path", floppy)->set_handler(floppy_param_string_handler);
    SIM->get_param_string("path", floppy)->set_runtime_param(1);
    SIM->get_param_bool("readonly", floppy)->set_handler(floppy_param_handler);
    SIM->get_param_bool("readonly", floppy)->set_runtime_param(1);
    SIM->get_param_enum("status", floppy)->set_handler(floppy_param_handler);
    SIM->get_param_enum("status", floppy)->set_runtime_param(1);
  }
  // register handler for correct floppy parameter handling after runtime config
  BX_FD_THIS s.rt_conf_id = SIM->register_runtime_config_handler(this, runtime_config_handler);
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("floppy", this);
#endif
}

void bx_floppy_ctrl_c::reset(unsigned type)
{
  BX_FD_THIS s.pending_irq = 0;
  BX_FD_THIS s.reset_sensei = 0; /* no reset result present */

  BX_FD_THIS s.main_status_reg = 0;
  BX_FD_THIS s.status_reg_a = 0;
  BX_FD_THIS s.status_reg_b = 0;
  BX_FD_THIS s.status_reg0 = 0;
  BX_FD_THIS s.status_reg1 = 0;
  BX_FD_THIS s.status_reg2 = 0;
  BX_FD_THIS s.status_reg3 = 0;

  // software reset (via DOR port 0x3f2 bit 2) does not change DOR
  if (type == BX_RESET_HARDWARE) {
    BX_FD_THIS s.DSR = 2; /* 250 Kbps */
    BX_FD_THIS s.DOR = 0x0c;
    // motor off, drive 3..0
    // DMA/INT enabled
    // normal operation
    // drive select 0

    // DIR and CCR affected only by hard reset
    for (int i=0; i<4; i++) {
      BX_FD_THIS s.DIR[i] |= 0x80; // disk changed
    }
    BX_FD_THIS s.lock = 0;
  } else {
    BX_INFO(("controller reset in software"));
  }
  BX_FD_THIS s.power_down = 0;
  BX_FD_THIS s.standby = 0;
  if (BX_FD_THIS s.lock == 0) {
    BX_FD_THIS s.config = 0;
    BX_FD_THIS s.pretrk = 0;
  }
  BX_FD_THIS s.perp_mode = 0;
  BX_FD_THIS s.mode0 = 0;
  BX_FD_THIS s.mode1 = 0;
  BX_FD_THIS s.mode2 = 0;
  BX_FD_THIS s.option = 0;

  for (int i=0; i<4; i++) {
    BX_FD_THIS s.cylinder[i] = 0;
    BX_FD_THIS s.head[i] = 0;
    BX_FD_THIS s.sector[i] = 1;
    BX_FD_THIS s.eot[i] = 0;
  }

  DEV_pic_lower_irq(6);
  if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
    DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
  }
  enter_idle_phase();
}

void bx_floppy_ctrl_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "floppy", "Floppy State");
  new bx_shadow_num_c(list, "data_rate", &BX_FD_THIS s.DSR);
  new bx_shadow_data_c(list, "command", BX_FD_THIS s.command, MAX_PHASE_SIZE, 1);
  new bx_shadow_num_c(list, "command_index", &BX_FD_THIS s.command_index);
  new bx_shadow_num_c(list, "command_size", &BX_FD_THIS s.command_size);
  BXRS_PARAM_BOOL(list, command_complete, BX_FD_THIS s.command_complete);
  new bx_shadow_num_c(list, "pending_command", &BX_FD_THIS s.pending_command, BASE_HEX);
  BXRS_PARAM_BOOL(list, multi_track, BX_FD_THIS s.multi_track);
  BXRS_PARAM_BOOL(list, pending_irq, BX_FD_THIS s.pending_irq);
  new bx_shadow_num_c(list, "reset_sensei", &BX_FD_THIS s.reset_sensei);
  new bx_shadow_num_c(list, "sector_count", &BX_FD_THIS s.sector_count);
  new bx_shadow_num_c(list, "format_fillbyte", &BX_FD_THIS s.format_fillbyte, BASE_HEX);
  new bx_shadow_num_c(list, "format_cylinder", &BX_FD_THIS s.format_cylinder);
  new bx_shadow_data_c(list, "sector_bitmap", BX_FD_THIS s.format_sector_bp, 36);
  new bx_shadow_data_c(list, "result", BX_FD_THIS s.result, MAX_PHASE_SIZE, 1);
  new bx_shadow_num_c(list, "result_index", &BX_FD_THIS s.result_index);
  new bx_shadow_num_c(list, "result_size", &BX_FD_THIS s.result_size);
  new bx_shadow_num_c(list, "last_result", &BX_FD_THIS s.last_result);
  new bx_shadow_num_c(list, "DOR", &BX_FD_THIS s.DOR, BASE_HEX);
  new bx_shadow_num_c(list, "TDR", &BX_FD_THIS s.TDR, BASE_HEX);
  BXRS_PARAM_BOOL(list, TC, BX_FD_THIS s.TC);
  new bx_shadow_num_c(list, "main_status_reg", &BX_FD_THIS s.main_status_reg, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg_a", &BX_FD_THIS s.status_reg_a, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg_b", &BX_FD_THIS s.status_reg_b, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg0", &BX_FD_THIS s.status_reg0, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg1", &BX_FD_THIS s.status_reg1, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg2", &BX_FD_THIS s.status_reg2, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg3", &BX_FD_THIS s.status_reg3, BASE_HEX);
  new bx_shadow_num_c(list, "floppy_buffer_index", &BX_FD_THIS s.floppy_buffer_index);
  BXRS_PARAM_BOOL(list, format_write_flag, BX_FD_THIS s.format_write_flag);
  BXRS_PARAM_BOOL(list, lock, BX_FD_THIS s.lock);
  new bx_shadow_num_c(list, "SRT", &BX_FD_THIS s.SRT, BASE_HEX);
  new bx_shadow_num_c(list, "HUT", &BX_FD_THIS s.HUT, BASE_HEX);
  new bx_shadow_num_c(list, "HLT", &BX_FD_THIS s.HLT, BASE_HEX);
  new bx_shadow_num_c(list, "config", &BX_FD_THIS s.config, BASE_HEX);
  new bx_shadow_num_c(list, "pretrk", &BX_FD_THIS s.pretrk);
  new bx_shadow_num_c(list, "perp_mode", &BX_FD_THIS s.perp_mode);
  new bx_shadow_num_c(list, "mode0", &BX_FD_THIS s.mode0);
  new bx_shadow_num_c(list, "mode1", &BX_FD_THIS s.mode1);
  new bx_shadow_num_c(list, "mode2", &BX_FD_THIS s.mode2);
  new bx_shadow_num_c(list, "option", &BX_FD_THIS s.option);
  new bx_shadow_num_c(list, "power_down", &BX_FD_THIS s.power_down);
  BXRS_PARAM_BOOL(list, standby, BX_FD_THIS s.standby);
  new bx_shadow_data_c(list, "buffer", BX_FD_THIS s.floppy_buffer, 512);
  new bx_shadow_data_c(list, "scan", BX_FD_THIS s.scan_buffer, 512);
  for (unsigned i=0; i<4; i++) {
    char name[8];
    sprintf(name, "drive%u", i);
    bx_list_c *drive = new bx_list_c(list, name);
    new bx_shadow_num_c(drive, "cylinder", &BX_FD_THIS s.cylinder[i]);
    new bx_shadow_num_c(drive, "head", &BX_FD_THIS s.head[i]);
    new bx_shadow_num_c(drive, "sector", &BX_FD_THIS s.sector[i]);
    new bx_shadow_num_c(drive, "eot", &BX_FD_THIS s.eot[i]);
    BXRS_PARAM_BOOL(drive, media_present, BX_FD_THIS s.media_present[i]);
    new bx_shadow_num_c(drive, "DIR", &BX_FD_THIS s.DIR[i], BASE_HEX);
  }
}

void bx_floppy_ctrl_c::after_restore_state(void)
{
  if (BX_FD_THIS s.statusbar_id[0] >= 0)
    bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[0], (BX_FD_THIS s.DOR & 0x10) > 0);
  if (BX_FD_THIS s.statusbar_id[1] >= 0)
    bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[1], (BX_FD_THIS s.DOR & 0x20) > 0);
}

void bx_floppy_ctrl_c::runtime_config_handler(void *this_ptr)
{
  bx_floppy_ctrl_c *class_ptr = (bx_floppy_ctrl_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_floppy_ctrl_c::runtime_config(void)
{
  char pname[16];

  for (unsigned drive=0; drive<2; drive++) {
    if (BX_FD_THIS s.media[drive].status_changed) {
      sprintf(pname, "floppy.%u.status", drive);
      bool status = (SIM->get_param_enum(pname)->get() == BX_INSERTED);
      if (BX_FD_THIS s.media_present[drive]) {
        BX_FD_THIS set_media_status(drive, 0);
      }
      if (status) {
        BX_FD_THIS set_media_status(drive, 1);
      }
      BX_FD_THIS s.media[drive].status_changed = 0;
    }
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_floppy_ctrl_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_FD_SMF
  bx_floppy_ctrl_c *class_ptr = (bx_floppy_ctrl_c *) this_ptr;

  return class_ptr->read(address, io_len);
}

/* reads from the floppy io ports */
Bit32u bx_floppy_ctrl_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_FD_SMF
  Bit8u value = 0, drive;

  // if we are in power down mode, no access is granted.
  // only a reset can bring it out of power down mode
  if (BX_FD_THIS s.power_down & PD_FLAGS_AUTO_PD) {
    BX_DEBUG(("tried to read from a powered down device..."));
    return 0xFF;
  }
  
  Bit8u pending_command = BX_FD_THIS s.pending_command;
  switch (address) {
#if BX_DMA_FLOPPY_IO

    case 0x3F0: // diskette controller Status Register A
#if (FDC_CURRENT_TYPE & (FDC_TYPE_82077AA | FDC_TYPE_PC87306))
      value = BX_FD_THIS s.status_reg_a;
#else
      value = 0xFF;  // specs say it is tri-stated
#endif
      break;

    case 0x3F1: // diskette controller Status Register B
#if (FDC_CURRENT_TYPE & (FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_PC87306))
      value = BX_FD_THIS s.status_reg_b;
#else
      value = 0xFF;  // specs say it is tri-stated
#endif
      break;

    case 0x3F2: // diskette controller digital output register
      value = BX_FD_THIS s.DOR;
      break;

    case 0x3F4: /* diskette controller main status register */
      value = BX_FD_THIS s.main_status_reg;
      break;

    case 0x3F5: /* diskette controller data */
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) &&
          (((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_NORMAL_DATA)) ||
           ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_DELETED_DATA)) ||
           ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_TRACK)))) {
        dma_write(&value, 1);
        lower_interrupt();
        // don't enter idle phase until we've given CPU last data byte
        if (BX_FD_THIS s.TC) enter_idle_phase();
      } else if (BX_FD_THIS s.result_size == 0) {
        BX_ERROR(("port 0x3f5: no results to read"));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        value = BX_FD_THIS s.last_result;
        BX_FD_THIS s.status_reg0 = 0x80;
        enter_result_phase();
      } else {
        value = BX_FD_THIS s.result[BX_FD_THIS s.result_index++];
        BX_FD_THIS s.last_result = value;
        BX_FD_THIS s.main_status_reg &= 0xF0;
        BX_FD_THIS lower_interrupt();
        if (BX_FD_THIS s.result_index >= BX_FD_THIS s.result_size) {
          enter_idle_phase();
        }
      }
      break;
#endif  // #if BX_DMA_FLOPPY_IO

    case 0x3F3: // Tape Drive Register
#if (FDC_CURRENT_TYPE & FDC_TYPE_82078)
      if (BX_FD_THIS s.power_down & PD_FLAGS_EREG_EN) {
#endif
        drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
        if (BX_FD_THIS s.media_present[drive]) {
          switch (BX_FD_THIS s.media[drive].type) {
            case BX_FLOPPY_160K:
            case BX_FLOPPY_180K:
            case BX_FLOPPY_320K:
            case BX_FLOPPY_360K:
            case BX_FLOPPY_1_2:
              value = 0x00;
              break;
            case BX_FLOPPY_720K:
              value = 0xc0;
              break;
            case BX_FLOPPY_1_44:
              value = 0x80;
              break;
            case BX_FLOPPY_2_88:
              value = 0x40;
              break;
            default: // BX_FLOPPY_NONE
              value = 0x20;
              break;
          }
        } else {
          value = 0x20;
        }
#if (FDC_CURRENT_TYPE & FDC_TYPE_82078)
      }
#endif
      break;

    case 0x3F6: // Reserved for future floppy controllers
                // This address shared with the hard drive controller
      value = DEV_hd_read_handler(bx_devices.pluginHardDrive, address, io_len);
      break;

    case 0x3F7: // diskette controller digital input register
      // This address shared with the hard drive controller:
      //   Bit  7   : floppy
      //   Bits 6..0: hard drive
      value = DEV_hd_read_handler(bx_devices.pluginHardDrive, address, io_len);
      value &= 0x7f;
      // add in diskette change line if motor is on
      drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
      if (BX_FD_THIS s.DOR & (1<<(drive+4))) {
        value |= (BX_FD_THIS s.DIR[drive] & 0x80);
      }
      break;

    default:
      BX_ERROR(("io_read: unsupported address 0x%04x", (unsigned) address));
      return 0;
  }

#if IGNORE_DEBUG_03F6
  if (address != 0x3F6)
#endif
  {
    BX_DEBUG(("read(): during command 0x%02x, port 0x%04x returns 0x%02x",
            pending_command, address, value));
  }

  return value;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_floppy_ctrl_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_FD_SMF
  bx_floppy_ctrl_c *class_ptr = (bx_floppy_ctrl_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

/* writes to the floppy io ports */
void bx_floppy_ctrl_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_FD_SMF
  bool dma_and_interrupt_enable;
  bool normal_operation, prev_normal_operation;
  Bit8u drive_select;
  Bit8u motor_on_drive0, motor_on_drive1;

#if IGNORE_DEBUG_03F6
  if (address != 0x3F6)
#endif
  {
    BX_DEBUG(("write access to port 0x%04x, value=0x%02x", address, value));
  }

  // if we are in power down mode, no access is granted, other than a RESET via the DOR
  //  (only a reset can bring it out of power down mode)
  if ((BX_FD_THIS s.power_down & PD_FLAGS_AUTO_PD) && 
      ((address != 0x3F2) || ((address == 0x3F2) && (value & 0x04)))) {
    BX_DEBUG(("tried to write to a powered down device..."));
    return;
  }
  
  if (BX_FD_THIS s.standby) {
    // writing a 1 to any of the drive enable bits, or writing a command 
    //  to the command byte will wake up the controller.
    if (((address == 0x3F2) && (value & 0x30)) ||
         (address == 0x3F5)) {
      BX_FD_THIS s.standby = 0;
      BX_FD_THIS s.main_status_reg = 0x80;
    } else {
      BX_DEBUG(("tried to write to a device in standby mode..."));
      return;
    }
  }

  switch (address) {
#if BX_DMA_FLOPPY_IO
    case 0x3F2: /* diskette controller digital output register */
      drive_select = value & FDC_DRV_MASK;
      prev_normal_operation = (BX_FD_THIS s.DOR & 0x04) != 0;
      normal_operation = (value & 0x04) != 0;
      dma_and_interrupt_enable = (value & 0x08) != 0;
      motor_on_drive0 = value & 0x10;
      motor_on_drive1 = value & 0x20;
#if (FDC_CURRENT_TYPE & FDC_TYPE_82078)
      // writing to a motor enable bit or doing a software reset wakes up the part
      if ((motor_on_drive0 || motor_on_drive1) ||
          (!prev_normal_operation && normal_operation))
        BX_FD_THIS s.power_down &= ~PD_FLAGS_AUTO_PD;
#endif
      /* set status bar conditions for Floppy 0 and Floppy 1 */
      if (BX_FD_THIS s.statusbar_id[0] >= 0) {
        if (motor_on_drive0 != (BX_FD_THIS s.DOR & 0x10))
          bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[0], motor_on_drive0);
      }
      if (BX_FD_THIS s.statusbar_id[1] >= 0) {
        if (motor_on_drive1 != (BX_FD_THIS s.DOR & 0x20))
          bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[1], motor_on_drive1);
      }
      if (!dma_and_interrupt_enable && ((BX_FD_THIS s.DOR & 0x08) != 0)) {
        BX_DEBUG(("DMA and interrupt capabilities disabled"));
      }
      if (!prev_normal_operation && normal_operation) {
        // transition from RESET to NORMAL
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, 250, 0);
      } else if (prev_normal_operation && !normal_operation) {
        // transition from NORMAL to RESET
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.pending_command = FD_RESET; // RESET pending
      }
      BX_FD_THIS s.DOR = value;

      BX_DEBUG(("io_write: digital output register"));
      BX_DEBUG(("  motor on, drive0 = %d", motor_on_drive0 > 0));
      BX_DEBUG(("  motor on, drive1 = %d", motor_on_drive1 > 0));
      BX_DEBUG(("  dma_and_interrupt_enable=%02x", (unsigned) dma_and_interrupt_enable));
      BX_DEBUG(("  normal_operation=%02x", (unsigned) normal_operation));
      BX_DEBUG(("  drive_select=%02x", (unsigned) drive_select));
      if (BX_FD_THIS s.device_type[drive_select] == FDRIVE_NONE) {
        BX_DEBUG(("WARNING: non existing drive selected"));
      }
      break;

#if (FDC_CURRENT_TYPE & FDC_TYPE_82078)
    case 0x3F3: /* diskette controller tape drive register */
      if (BX_FD_THIS s.power_down & PD_FLAGS_EREG_EN) {
        // The 82078 allows you to "remap" the drives
        //  using the 44PD_EN bit and bit 2 of this register
        // (this function is currently unsupported)
        BX_ERROR(("Unsupported write to TDR: %02x", value));
      }
      break;
#endif

    case 0x3f4: /* diskette controller data rate select register */
      BX_FD_THIS s.DSR = value & 0x7f;
      if (value & 0x80) {
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.pending_command = FD_RESET; // RESET pending
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, 250, 0);
      }
      switch (BX_FD_THIS s.DSR & 0x03) {
        case 0: BX_DEBUG(("  500 Kbps")); break;
        case 1: BX_DEBUG(("  300 Kbps")); break;
        case 2: BX_DEBUG(("  250 Kbps")); break;
        case 3: BX_DEBUG(("  1 Mbps")); break;
      }

#if (FDC_CURRENT_TYPE & FDC_TYPE_82078)
      if (value & 0x40) {
        BX_FD_THIS reset(BX_RESET_HARDWARE);
        BX_FD_THIS after_restore_state(); // turn off status bar indicators
        BX_FD_THIS s.power_down |= PD_FLAGS_AUTO_PD;
      }
#endif
      break;

    case 0x3F5: /* diskette controller data */
      BX_DEBUG(("command byte = 0x%02x", (unsigned) value));
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) && 
        (((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_WRITE_NORMAL_DATA)) ||
         ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_EQUAL))  ||
         ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_LOW_EQUAL))  ||
         ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_HIGH_EQUAL)))) {
        BX_FD_THIS dma_read((Bit8u *) &value, 1);
        BX_FD_THIS lower_interrupt();
        break;
      } else if (BX_FD_THIS s.command_complete) {
        if (BX_FD_THIS s.pending_command != FD_CMD_NOP)
          BX_PANIC(("write 0x03f5: receiving new command 0x%02x, old one (0x%02x) pending",
            value, BX_FD_THIS s.pending_command));
        BX_FD_THIS s.command[0] = value;
        BX_FD_THIS s.command_complete = 0;
        BX_FD_THIS s.command_index = 1;
        /* read/write command in progress */
        BX_FD_THIS s.main_status_reg &= ~FD_MS_DIO; // leave drive status untouched
        BX_FD_THIS s.main_status_reg |= FD_MS_RQM | FD_MS_BUSY;

        // if the command is supported by this controller
        bool command_error = 0;
        if (command_supported(value)) {
          // the FD_CMD_SENSE_INT_STATUS command relies on previous values,
          //  the rest, we clear assuming no error
          if (value != FD_CMD_SENSE_INT_STATUS) {
            BX_FD_THIS s.status_reg0 =
            BX_FD_THIS s.status_reg1 = 
            BX_FD_THIS s.status_reg2 = 0x00;
          }

          switch (value) {
            // sense interrupt status
            case FD_CMD_SENSE_INT_STATUS:
            // motor on/off
            case FD_CMD_MOTOR_ON_OFF:
            // enter standby mode
            case FD_CMD_ENTER_STANDBY_MODE:
            // exit standby mode
            case FD_CMD_EXIT_STANDBY_MODE:
            // hardware/software reset
            case FD_CMD_HARD_RESET:
            // dump registers (Enhanced drives)
            case FD_CMD_DUMPREG:
            // Version
            case FD_CMD_VERSION:
            // Part ID command
            case FD_CMD_PART_ID:
            // Unlock command
            case FD_CMD_LOCK_UNLOCK:
            // Lock command
            case FD_CMD_LOCK_UNLOCK | FD_CMD_LOCK:
            // Save
            case FD_CMD_SAVE:
              BX_FD_THIS s.command_size = 1;
              break;

            // get status
            case FD_CMD_SENSE_DRV_STATUS:
            // recalibrate
            case FD_CMD_RECALIBRATE:
            // read ID
            case FD_CMD_READ_ID:
            case FD_CMD_READ_ID | FD_CMD_MFM:
            // Perpendicular mode
            case FD_CMD_PERPENDICULARE_MODE:
            // Option command
            case FD_CMD_OPTION:
            // power down mode
            case FD_CMD_POWER_DOWN_MODE:
              BX_FD_THIS s.command_size = 2;
              break;

            // specify
            case FD_CMD_SPECIFY:
            // seek
            case FD_CMD_SEEK:
            case FD_CMD_RELATIVE_SEEK:
            case FD_CMD_RELATIVE_SEEK | FD_CMD_DIR:
            // Set Track
            case FD_CMD_SET_TRACK:
              BX_FD_THIS s.command_size = 3;
              break;

            // Configure
            case FD_CMD_CONFIGURE:
              BX_FD_THIS s.command_size = 4;
              break;

            // set mode
            case FD_CMD_MODE:
              BX_FD_THIS s.command_size = 5;
              break;

            // format and write track
            case FD_CMD_FORMAT_AND_WRITE | FD_CMD_MFM:
            // format track
            case FD_CMD_FORMAT_TRACK | FD_CMD_MFM:
            // drive specification
            case FD_CMD_DRIVE_SPECIFICATION:
              BX_FD_THIS s.command_size = 6;
              break;

            // write normal data
            case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MFM:
            // read normal data
            case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM:
            // read deleted/normal data
            case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM:
            // read track
            case FD_CMD_READ_TRACK | FD_CMD_MFM:
            // verify data
            case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_VERIFY | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_VERIFY | FD_CMD_MFM:
            // scan commands
            case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_SCAN_EQUAL | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_EQUAL | FD_CMD_MFM:
            case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM:
            case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM:
            case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM | FD_CMD_SK:
            case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM:
              BX_FD_THIS s.command_size = 9;
              break;

            // Restore
            case FD_CMD_RESTORE:
              BX_FD_THIS s.command_size = 17;
              break;

            default:
              BX_ERROR(("Command issued that we don't support via our checks. 0x%02X", value));
              command_error = 1;
          }
        } else
          command_error = 1;

        if (command_error) {
          // unsupported/unknown commands
          BX_FD_THIS s.pending_command = value;
          BX_DEBUG(("COMMAND: [%02x]", value));
          BX_ERROR(("io_write: 0x3f5: invalid or unsupported floppy command 0x%02x", (unsigned) value));
          BX_FD_THIS s.command_size = 0;   // make sure we don't try to process this command
          BX_FD_THIS s.status_reg0 = 0x80; // status: invalid command
          enter_result_phase();
        }
      } else {
        BX_FD_THIS s.command[BX_FD_THIS s.command_index++] = value;
        // the Drive Specification command has a variable count of command bytes.
        // bit 7 in the command byte indicates the last
        if (BX_FD_THIS s.command[0] == FD_CMD_DRIVE_SPECIFICATION) {
          if (value & (1<<7))
            BX_FD_THIS s.command_size = BX_FD_THIS s.command_index;
        }
      }
      if (BX_FD_THIS s.command_index == BX_FD_THIS s.command_size) {
        /* read/write command not in progress any more */
        floppy_command();
        BX_FD_THIS s.command_complete = 1;
      }
      return;
#endif  // #if BX_DMA_FLOPPY_IO

    case 0x3F6: /* diskette controller (reserved) */
#if IGNORE_DEBUG_03F6
      if (address != 0x3F6)
#endif
      {
        BX_DEBUG(("io_write: reserved register 0x3f6 unsupported"));
      }
      // this address shared with the hard drive controller
      DEV_hd_write_handler(bx_devices.pluginHardDrive, address, value, io_len);
      break;

#if BX_DMA_FLOPPY_IO
    case 0x3F7: /* diskette controller configuration control register */
      if (value != (BX_FD_THIS s.DSR & 0x03))
        BX_DEBUG(("io_write: config control register: 0x%02x", value));
      BX_FD_THIS s.DSR &= 0xfc;
      BX_FD_THIS s.DSR |= (value & 0x03);
      switch (BX_FD_THIS s.DSR & 0x03) {
        case 0: BX_DEBUG(("  500 Kbps")); break;
        case 1: BX_DEBUG(("  300 Kbps")); break;
        case 2: BX_DEBUG(("  250 Kbps")); break;
        case 3: BX_DEBUG(("  1 Mbps")); break;
      }
      break;

    default:
      BX_ERROR(("io_write ignored: 0x%04x = 0x%02x", (unsigned) address, (unsigned) value));
      break;
#endif  // #if BX_DMA_FLOPPY_IO
  }
}

void bx_floppy_ctrl_c::floppy_command(void)
{
  unsigned i;
  Bit8u motor_on;
  Bit8u head, drive, sector, eot;
  Bit8u sector_size, data_length;
  Bit16u cylinder;
  Bit32u sector_time, step_delay;
  int new_cylinder;

  // Print command
  char buf[9+(MAX_PHASE_SIZE*5)+1], *p = buf;
  p += sprintf(p, "COMMAND: ");
  for (i=0; i<BX_FD_THIS s.command_size; i++) {
    p += sprintf(p, "[%02x] ", (unsigned) BX_FD_THIS s.command[i]);
  }
  BX_DEBUG(("%s", buf));

  BX_FD_THIS s.pending_command = BX_FD_THIS s.command[0];
  switch (BX_FD_THIS s.pending_command) {
    // specify
    case FD_CMD_SPECIFY:
      // execution: specified parameters are loaded
      // result: no result bytes, no interrupt
      BX_FD_THIS s.SRT = BX_FD_THIS s.command[1] >> 4;
      BX_FD_THIS s.HUT = BX_FD_THIS s.command[1] & 0x0f;
      BX_FD_THIS s.HLT = BX_FD_THIS s.command[2] >> 1;
      BX_FD_THIS s.main_status_reg |= (BX_FD_THIS s.command[2] & 0x01) ? FD_MS_NDMA : 0;
      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA)
        BX_ERROR(("non DMA mode not fully implemented yet"));
      BX_DEBUG(("Specify (SRT     = 0x%02x)", (BX_FD_THIS s.command[1] & 0xF0) >> 4));
      BX_DEBUG(("        (HUT     = 0x%02x)", (BX_FD_THIS s.command[1] & 0x0F) >> 0));
      BX_DEBUG(("        (HLT     = 0x%02x)", (BX_FD_THIS s.command[2] & 0xFE) >> 1));
      BX_DEBUG(("        (Non-DMA =    %d)",  (BX_FD_THIS s.command[2] & 0x01) >> 0));
      enter_idle_phase();
      return;

    // get status
    case FD_CMD_SENSE_DRV_STATUS:
      drive = (BX_FD_THIS s.command[1] & FDC_DRV_MASK);
      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      BX_FD_THIS s.status_reg3 = 0x28 | (BX_FD_THIS s.head[drive]<<2) | drive
        | (BX_FD_THIS s.media[drive].write_protected ? 0x40 : 0x00);
      if ((BX_FD_THIS s.device_type[drive] != FDRIVE_NONE) &&
          (BX_FD_THIS s.cylinder[drive] == 0))
        BX_FD_THIS s.status_reg3 |= 0x10;
      enter_result_phase();
      return;

    // set mode
    case FD_CMD_MODE:
      if (!(BX_FD_THIS s.command[1] & 0x02) || !(BX_FD_THIS s.command[3] & 0xC0))
        BX_DEBUG(("Some always-set bits in Mode command not set"));
      if ((BX_FD_THIS s.command[1] & 0x10) || (BX_FD_THIS s.command[2] & 0xff) || 
          (BX_FD_THIS s.command[3] & 0x20) || (BX_FD_THIS s.command[4] & 0xfa))
        BX_DEBUG(("Some always-clear bits in Mode command set"));
      BX_FD_THIS s.mode0 = BX_FD_THIS s.command[1];
      BX_FD_THIS s.mode1 = BX_FD_THIS s.command[3];
      BX_FD_THIS s.mode2 = BX_FD_THIS s.command[4];
      return;

    // recalibrate
    case FD_CMD_RECALIBRATE:
      drive = (BX_FD_THIS s.command[1] & FDC_DRV_MASK);
      BX_FD_THIS s.DOR &= ~FDC_DRV_MASK;
      BX_FD_THIS s.DOR |= drive;
      BX_DEBUG(("floppy_command(): recalibrate drive %u", (unsigned) drive));
      step_delay = calculate_step_delay(drive, 0);
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, step_delay, 0);
      /* command head to track 0
       * controller set to non-busy
       * error condition noted in Status reg 0's equipment check bit
       * seek end bit set to 1 in Status reg 0 regardless of outcome
       * The last two are taken care of in timer().
       */
      BX_FD_THIS s.cylinder[drive] = 0;
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= (1 << drive);
      return;

    // sense interrupt status
    case FD_CMD_SENSE_INT_STATUS:
      /* execution:
       *   get status
       * result:
       *   no interupt
       *   byte0 = status reg0
       *   byte1 = current cylinder number (0 to 79)
       *   byte2 = MSN PTN (DP8473 only, and if Mode:ET = 1)
       */
      if (BX_FD_THIS s.reset_sensei > 0) {
        drive = 4 - BX_FD_THIS s.reset_sensei;
        BX_FD_THIS s.status_reg0 &= 0xf8;
        BX_FD_THIS s.status_reg0 |= (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.reset_sensei--;
      } else if (!BX_FD_THIS s.pending_irq) {
        BX_FD_THIS s.status_reg0 = 0x80;
      }
      BX_DEBUG(("sense interrupt status"));
      enter_result_phase();
      return;

    // seek
    case FD_CMD_SEEK:
    case FD_CMD_RELATIVE_SEEK:
    case FD_CMD_RELATIVE_SEEK | FD_CMD_DIR:
      /* command:
       *   byte0 = 0F / 8F
       *   byte1 = drive & head select
       *   byte2 = (relative) cylinder number
       * execution:
       *   postion head over specified cylinder
       * result:
       *   no result bytes, issues an interrupt
       */
      drive = BX_FD_THIS s.command[1] & FDC_DRV_MASK;
      BX_FD_THIS s.DOR &= ~FDC_DRV_MASK;
      BX_FD_THIS s.DOR |= drive;

      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      step_delay = calculate_step_delay(drive, BX_FD_THIS s.command[2]);
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, step_delay, 0);
      cylinder = BX_FD_THIS s.command[2];
#if (FDC_CURRENT_TYPE & FDC_TYPE_DP8473)
      if (BX_FD_THIS s.mode0 & FLAGS_ETR) {
        cylinder |= (Bit16u) (BX_FD_THIS s.command[3] & 0xF0) << 4;
      }
#endif
      if (BX_FD_THIS s.pending_command == FD_CMD_SEEK) {
        new_cylinder = cylinder;
      } else { // relative seek
        // cylinder is relative
        if (BX_FD_THIS s.command[0] & FD_CMD_DIR)
          new_cylinder = BX_FD_THIS s.cylinder[drive] + cylinder;
        else
          new_cylinder = (int) (Bit16s) (BX_FD_THIS s.cylinder[drive] - cylinder);
      }
      
      BX_FD_THIS s.cylinder[drive] = (Bit16u) new_cylinder;
      if (new_cylinder >= (int) BX_FD_THIS s.media[drive].tracks) {
        BX_ERROR(("attempt to access from non-present cylinder %d (of %d)", new_cylinder, BX_FD_THIS s.media[drive].tracks));
        // ST0: even though it is past last cyl, it is considered success
        BX_FD_THIS s.status_reg0 = 0x00 | (1 << 5) | drive;  // SE = 1
        BX_FD_THIS s.status_reg1 = (1 << 7);
      } else if (new_cylinder < 0) {
        BX_ERROR(("attempt to access from non-present cylinder %d (of %d)", new_cylinder, BX_FD_THIS s.media[drive].tracks));
        // ST0: (abnormal termination: started execution but failed)
        BX_FD_THIS s.status_reg0 = 0x40 | (1 << 5) | (1 << 4) | drive;  // SE = 1, EC = 1
      } else {
        BX_FD_THIS s.status_reg0 = 0x00 | (1 << 5) | drive;  // SE = 1
      }
      /* data reg not ready, drive not busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= (1 << drive);
      return;

    // motor on/off
    case FD_CMD_MOTOR_ON_OFF:
      /* set status bar conditions for Floppy 0 */
      for (i=0; i<2; i++) {  // shift value and index value
        if (BX_FD_THIS s.command[0] & (0x20 << i)) {
          BX_FD_THIS s.DOR &= ~(0x10 << i);
          if (BX_FD_THIS s.command[0] & 0x80)
            BX_FD_THIS s.DOR |= (0x10 << i);
          if (BX_FD_THIS s.statusbar_id[i] >= 0)
            bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[i], (BX_FD_THIS s.command[0] & 0x80));
        }
      }
      enter_idle_phase();
      break;

    // enter standby mode
    case FD_CMD_ENTER_STANDBY_MODE:
      BX_FD_THIS s.standby = 1;
      BX_FD_THIS s.main_status_reg = 0x00;
      enter_idle_phase();
      break;

    // exit standby mode
    case FD_CMD_EXIT_STANDBY_MODE:
      BX_FD_THIS s.standby = 0;
      BX_FD_THIS s.main_status_reg = 0x80;
      enter_idle_phase();
      break;

    // hardware/software reset
    case FD_CMD_HARD_RESET:
      BX_FD_THIS reset(BX_RESET_HARDWARE);
      break;

    // Configure
    case FD_CMD_CONFIGURE:
      BX_DEBUG(("configure (eis     = %d)", (BX_FD_THIS s.command[2] & 0x40) ? 1 : 0));
      BX_DEBUG(("configure (efifo   = %d)", (BX_FD_THIS s.command[2] & 0x20) ? 1 : 0));
      BX_DEBUG(("configure (no poll = %d)", (BX_FD_THIS s.command[2] & 0x10) ? 1 : 0));
      BX_DEBUG(("configure (fifothr = %d)", BX_FD_THIS s.command[2] & 0x0F));
      BX_DEBUG(("configure (pretrk  = %d)", BX_FD_THIS s.command[3]));
      if (((BX_FD_THIS s.command[2] & 0x0F) > 0) && ((BX_FD_THIS s.command[2] & 0x0F) <= 15)) {
        BX_FD_THIS s.config = BX_FD_THIS s.command[2];
      } else {
        BX_ERROR(("Illegal size for FIFO: %d (setting to 16)", (BX_FD_THIS s.command[2] & 0x0F) + 1));
        BX_FD_THIS s.config = (BX_FD_THIS s.command[2] & 0xF0) | 15;
      }
      BX_FD_THIS s.pretrk = BX_FD_THIS s.command[3];
      enter_idle_phase();
      return;

    // read ID
    case FD_CMD_READ_ID:
    case FD_CMD_READ_ID | FD_CMD_MFM:
      drive = BX_FD_THIS s.command[1] & FDC_DRV_MASK;
      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      BX_FD_THIS s.DOR &= ~FDC_DRV_MASK;
      BX_FD_THIS s.DOR |= drive;

      motor_on = (BX_FD_THIS s.DOR>>(drive+4)) & 0x01;
      if (motor_on == 0) {
        BX_ERROR(("floppy_command(): read ID: motor not on"));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        return; // Hang controller
      }
      if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE) {
        BX_ERROR(("floppy_command(): read ID: bad drive #%d", drive));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        return; // Hang controller
      }
      if (BX_FD_THIS s.media_present[drive] == 0) {
        BX_ERROR(("attempt to read sector ID with media not present"));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        return; // Hang controller
      }
      // if the media doesn't have 2 heads, and we tried to ReadID from the second head,
      // or trying to read id with MFM == 0,
      //  give abnormal termination in ST0 and ST1
      if ((((unsigned int) BX_FD_THIS s.head[drive] + 1) > BX_FD_THIS s.media[drive].heads) ||
           (BX_FD_THIS s.cylinder[drive] >= BX_FD_THIS s.media[drive].tracks) ||
          ((BX_FD_THIS s.pending_command & FD_CMD_MFM) == 0)) {
        BX_ERROR(("attempt to access from non-present cyl/head %d/%d, or a non-MFM attempt.", 
          BX_FD_THIS s.cylinder[drive], BX_FD_THIS s.head[drive]));
        // ST0: IC1,0=01  (abnormal termination: started execution but failed)
        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive;
        BX_FD_THIS s.status_reg1 = 0x01;  // missing address mark
      } else {
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive]<<2) | drive;

        // The ReadID command reads the ID from the current position on the Track
        // This means the head could be anywhere from sector 1 to sector spt
#if FDC_FOR_OMNIDISK
        BX_FD_THIS s.sector[drive]++;
        if (BX_FD_THIS s.sector[drive] > BX_FD_THIS s.media[drive].sectors_per_track)
          BX_FD_THIS s.sector[drive] = 1;
#else
        // We create a random number from 1 to spt
        BX_FD_THIS s.sector[drive] = (rand() % BX_FD_THIS s.media[drive].sectors_per_track) + 1;
#endif
      }

      // time to read one sector at 300 rpm
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
      /* data reg not ready, controller busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
      return;

    // format track
    case FD_CMD_FORMAT_TRACK | FD_CMD_MFM:
    case FD_CMD_FORMAT_AND_WRITE | FD_CMD_MFM:
      drive = BX_FD_THIS s.command[1] & FDC_DRV_MASK;
      BX_FD_THIS s.DOR &= ~FDC_DRV_MASK;
      BX_FD_THIS s.DOR |= drive;

      motor_on = (BX_FD_THIS s.DOR>>(drive+4)) & 0x01;
      if (motor_on == 0)
        BX_PANIC(("floppy_command(): format track: motor not on"));
      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      sector_size = BX_FD_THIS s.command[2];
      BX_FD_THIS s.sector_count = BX_FD_THIS s.command[3];
      BX_FD_THIS s.format_fillbyte = BX_FD_THIS s.command[5];
      if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE)
        BX_PANIC(("floppy_command(): format track: bad drive #%d", drive));

      if (sector_size != 0x02) { // 512 bytes
        BX_PANIC(("format track: sector size %d not supported", 128<<sector_size));
      }
      if (BX_FD_THIS s.sector_count > BX_FD_THIS s.media[drive].sectors_per_track) {
        BX_ERROR(("format track: %d sectors/track requested (%d expected)",
                  BX_FD_THIS s.sector_count, BX_FD_THIS s.media[drive].sectors_per_track));
        BX_FD_THIS s.sector_count = BX_FD_THIS s.media[drive].sectors_per_track;
      }
      if (BX_FD_THIS s.media_present[drive] == 0) {
        BX_ERROR(("attempt to format track with media not present"));
        return; // Hang controller
      }
      if (BX_FD_THIS s.media[drive].write_protected) {
        // media write-protected, return error
        BX_ERROR(("attempt to format track with media write-protected"));
        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive; // abnormal termination
        BX_FD_THIS s.status_reg1 = 0x27; // 0010 0111
        BX_FD_THIS s.status_reg2 = 0x31; // 0011 0001
        enter_result_phase();
        return;
      }

      /* 4 header bytes per sector are required */
      BX_FD_THIS s.sector_count <<= 2;
      BX_FD_THIS s.sector[drive] = 1;
      BX_FD_THIS s.format_cylinder = 0xFFFF;
      for (i=0; i<36; i++)
        BX_FD_THIS s.format_sector_bp[i] = 0;
      BX_FD_THIS s.format_write_flag = 0;
      BX_FD_THIS s.floppy_buffer_index = 0;

      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
        BX_ERROR(("non-DMA floppy format unimplemented"));
      } else {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
      }
      /* data reg not ready, controller busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
      BX_DEBUG(("format track"));
      return;

    // write normal data
    case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MFM:
    // read normal data
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM:
    // read deleted/normal data
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM:
    // read track
    case FD_CMD_READ_TRACK | FD_CMD_MFM:
    // verify data
    case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_VERIFY | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_VERIFY | FD_CMD_MFM:
    // scan commands
    case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MFM:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM:
      BX_FD_THIS s.multi_track = (BX_FD_THIS s.command[0] & 0x80) > 0;
      if ((BX_FD_THIS s.DOR & 0x08) == 0)
        BX_PANIC(("read/write/verify/scan command with DMA and int disabled"));
      drive = BX_FD_THIS s.command[1] & FDC_DRV_MASK;
      BX_FD_THIS s.DOR &= ~FDC_DRV_MASK;
      BX_FD_THIS s.DOR |= drive;

      motor_on = (BX_FD_THIS s.DOR>>(drive+4)) & 0x01;
      if (motor_on == 0)
        BX_PANIC(("floppy_command(): read/write: motor not on"));
      head = BX_FD_THIS s.command[3] & 0x01;
      cylinder = BX_FD_THIS s.command[2]; /* 0..79 depending */
      sector = ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_TRACK)) ?
        1 : BX_FD_THIS s.command[4];      /* 1..36 depending */
      eot = BX_FD_THIS s.command[6];      /* 1..36 depending */
      sector_size = BX_FD_THIS s.command[5];
      data_length = BX_FD_THIS s.command[8];
      BX_DEBUG(("read/write/verify/scan normal data"));
      BX_DEBUG(("BEFORE"));
      BX_DEBUG(("  drive    = %u", (unsigned) drive));
      BX_DEBUG(("  cylinder = %u", (unsigned) cylinder));
      BX_DEBUG(("  head     = %u", (unsigned) head));
      BX_DEBUG(("  sector   = %u", (unsigned) sector));
      BX_DEBUG(("  eot      = %u", (unsigned) eot));
      BX_DEBUG(("  stp/dtl  = %u", (unsigned) BX_FD_THIS s.command[8]));
#if (FDC_CURRENT_TYPE & FDC_TYPE_DP8473)
      BX_DEBUG(("DP8473: IPS = %i", (BX_FD_THIS s.command[2] & 0x80) > 0));
#endif
      if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE)
        BX_PANIC(("floppy_command(): read/write/scan: bad drive #%d", drive));

      // check that head number in command[1] bit two matches the head
      // reported in the head number field.  Real floppy drives are
      // picky about this, as reported in SF bug #439945, (Floppy drive
      // read input error checking).
      if (head != ((BX_FD_THIS s.command[1]>>2)&1)) {
        BX_ERROR(("head number in command[1] doesn't match head field"));
        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive; // abnormal termination
        BX_FD_THIS s.status_reg1 = 0x04; // 0000 0100
        BX_FD_THIS s.status_reg2 = 0x00; // 0000 0000
        enter_result_phase();
        return;
      }

      if (BX_FD_THIS s.media_present[drive] == 0) {
        BX_ERROR(("attempt to read/write/verify/scan sector %u with media not present", (unsigned) sector));
        return; // Hang controller
      }

      if (sector_size != 0x02) { // 512 bytes
        BX_PANIC(("read/write/verify/scan command: sector size %d not supported", 128<<sector_size));
      }

      if ((cylinder >= BX_FD_THIS s.media[drive].tracks) ||
          (sector > BX_FD_THIS s.media[drive].sectors_per_track)) {
        BX_ERROR(("attempt to read/write/verify/scan sector %u passed last sector %u",
                     (unsigned) sector,
                     (unsigned) BX_FD_THIS s.media[drive].sectors_per_track));
        BX_FD_THIS s.cylinder[drive] = cylinder;
        BX_FD_THIS s.head[drive]     = head;
        BX_FD_THIS s.sector[drive]   = sector;

        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive;
        BX_FD_THIS s.status_reg1 = 0x04;
        BX_FD_THIS s.status_reg2 = 0x00;
        enter_result_phase();
        return;
      }

      if (cylinder != BX_FD_THIS s.cylinder[drive]) {
        BX_DEBUG(("io: cylinder request != current cylinder"));
        reset_changeline();
      }

      // This hack makes older versions of the Bochs BIOS work
      if (eot == 0) {
        eot = BX_FD_THIS s.media[drive].sectors_per_track;
      }
      BX_FD_THIS s.cylinder[drive] = cylinder;
      BX_FD_THIS s.head[drive]     = head;
      BX_FD_THIS s.sector[drive]   = sector;
      BX_FD_THIS s.eot[drive]      = eot;

      if (((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_NORMAL_DATA)) ||
          ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_DELETED_DATA)) ||
          ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_READ_TRACK))) {
        do_floppy_xfer(BX_FD_THIS s.floppy_buffer, drive, FROM_FLOPPY);
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
          BX_FD_THIS s.main_status_reg |= (FD_MS_RQM | FD_MS_DIO);
        }
        // time to read one sector at 300 rpm
        sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
      } else if ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_WRITE_NORMAL_DATA)) {
        if (BX_FD_THIS s.media[drive].write_protected) {
          // media write-protected, return error
          BX_ERROR(("attempt to write with media write-protected"));
          BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive; // abnormal termination
          BX_FD_THIS s.status_reg1 = 0x27; // 0010 0111
          BX_FD_THIS s.status_reg2 = 0x31; // 0011 0001
          enter_result_phase();
          return;
        }
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
          BX_FD_THIS s.main_status_reg |= FD_MS_RQM;
        } else {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      } else if ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_VERIFY)) {
        BX_FD_THIS s.sector_count = data_length;
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        // time to verify one sector at 300 rpm
        sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
      } else if (((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_EQUAL)) ||
                 ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_LOW_EQUAL)) ||
                 ((BX_FD_THIS s.command[0] & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_HIGH_EQUAL))) {
        // the scan command(s) read in a sector, then compare the data with what is at the current DMA address.
        // it continues until one of the three below conditions are met, EOT has been found, or the count has been exhasted.
        //  scan_equal compares all bytes within the sector, continuing until a byte for byte match is found.
        //  scan_low_equal compares all bytes within the sector, continuing until all bytes are <= DMA bytes.
        //  scan_high_equal compares all bytes within the sector, continuing until all bytes are >= DMA bytes.
        do_floppy_xfer(BX_FD_THIS s.scan_buffer, drive, FROM_FLOPPY);
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
          BX_FD_THIS s.main_status_reg |= FD_MS_RQM;
        } else {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
        BX_FD_THIS s.stp = BX_FD_THIS s.command[8];
      } else {
        BX_PANIC(("floppy_command(): unknown read/write/verify/scan command"));
        return;
      }
      break;

    // dump registers (Enhanced drives)
    case FD_CMD_DUMPREG:
    // Version
    case FD_CMD_VERSION:
    // Part ID command
    case FD_CMD_PART_ID:
    // Unlock command
    case FD_CMD_LOCK_UNLOCK:
    // Lock command
    case FD_CMD_LOCK_UNLOCK | FD_CMD_LOCK:
    // Save
    case FD_CMD_SAVE:
      enter_result_phase();
      break;

    // Perpendicular mode
    case FD_CMD_PERPENDICULARE_MODE:
      BX_FD_THIS s.perp_mode = BX_FD_THIS s.command[1];
      BX_DEBUG(("perpendicular mode: config=0x%02x", BX_FD_THIS s.perp_mode));
      enter_idle_phase();
      break;

    // Option command
    case FD_CMD_OPTION:
      BX_FD_THIS s.option = BX_FD_THIS s.command[1];
      BX_DEBUG(("Option: ISO = %i", BX_FD_THIS s.command[1] & 1));
      enter_idle_phase();
      break;

    // Power down mode
    case FD_CMD_POWER_DOWN_MODE:
      BX_FD_THIS s.power_down = (BX_FD_THIS s.command[1] & ~PD_FLAGS_AUTO_PD);
      // 1. The motor enable pins FDME[0:1] must be inactive.
      // 2. The part must be idle; this is indicated by MSR = 80H and INT = 0
      //    (INT may be high even if MSR = 80H due to polling interrupt).
      // 3. The Head Unload Timer (HUT, explained in the SPECIFY command) must have expired.
      // 4. The auto powerdown timer must have timed out.
      if (BX_FD_THIS s.command[1] & PD_FLAGS_AUTO_PD) {
        if (((BX_FD_THIS s.DOR & 0x30) == 0) &&
             (BX_FD_THIS s.main_status_reg == 0x80) &&
             (BX_FD_THIS s.pending_irq == 0) &&
             (BX_FD_THIS s.reset_sensei == 0)) {
          BX_FD_THIS s.power_down |= PD_FLAGS_AUTO_PD;
          BX_DEBUG(("Power Down: setting PD_FLAGS_AUTO_PD to 1"));
        } else
          BX_DEBUG(("Power Down: setting PD_FLAGS_AUTO_PD to 0. Controller is active."));
      } else
        BX_DEBUG(("Power Down = %02x", BX_FD_THIS s.power_down));
      enter_result_phase();
      break;

    // drive specification
    case FD_CMD_DRIVE_SPECIFICATION:
      // this command allows you to specify to the hardware
      //  what type of disk is in the drive so it doesn't have
      //  to determine that itself.
      // it can have up to 6 command bytes, with bit 7 indicationg last one
      //  (not counting the first command byte)
      for (i=1; i<5; i++) {
        if (BX_FD_THIS s.command[i] & (1 << 7))
          break;
        if (((BX_FD_THIS s.command[i] & 0x60) >> 5) != i-1)
          BX_ERROR(("Drive Spec Command with FD0/1 != drive. We don't support virtual mapping."));
        BX_DEBUG(("Drive Spec Command with PTS = %i", (BX_FD_THIS s.command[i] & 0x10) > 0));
        if (((BX_FD_THIS s.command[i] & 0x0C) >> 2) == 0) {
          BX_DEBUG(("Drive Spec Command specifying a Data rate of %i (%i)",
            BX_FD_THIS s.command[i] & 3, drate_in_k[BX_FD_THIS s.command[i] & 3]));
          BX_FD_THIS s.DSR &= ~0x03;
          BX_FD_THIS s.DSR |= BX_FD_THIS s.command[i] & 3;
        } else if (((BX_FD_THIS s.command[i] & 0x0C) >> 2) == 3) {
          BX_DEBUG(("Drive Spec Command specifying a Data rate of %i (%i)",
            BX_FD_THIS s.command[i] & 3, drate_in_k[BX_FD_THIS s.command[i] & 3]));
          BX_FD_THIS s.DSR &= ~0x03;
          BX_FD_THIS s.DSR |= BX_FD_THIS s.command[i] & 3;
        } else
          BX_ERROR(("Drive Spec Command with bad DRT0/1."));
      }

      // the command identifies if it desires the result phase
      if (BX_FD_THIS s.command[i] & (1 << 6)) {
        BX_DEBUG(("Drive Spec Command requested no result phase"));
        enter_idle_phase();
      } else      
        enter_result_phase();
      break;

    // Set Track
    case FD_CMD_SET_TRACK:
      // command[1] should be 00110xxx as well
      if ((BX_FD_THIS s.command[1] & 0x30) == 0x30) {
        // do we write the new track value?
        if (BX_FD_THIS s.command[0] & FD_CMD_DIR) {
          drive = (BX_FD_THIS s.command[1] & FDC_DRV_MASK);
          // bit 2 = set = MSB, else LSB
          if (BX_FD_THIS s.command[1] & 0x04) {
            BX_FD_THIS s.cylinder[drive] &= 0x00FF;
            BX_FD_THIS s.cylinder[drive] |= (BX_FD_THIS s.command[2] << 8);
          } else {
            BX_FD_THIS s.cylinder[drive] &= 0xFF00;
            BX_FD_THIS s.cylinder[drive] |= BX_FD_THIS s.command[2];
          }
        }
      } else
        BX_FD_THIS s.status_reg0 = 0x80; // status: invalid command
      enter_result_phase();
      break;

    // Restore
    case FD_CMD_RESTORE:
      drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
      BX_FD_THIS s.DSR &= 0x03; // the DRATEx bits are unmapped
      BX_FD_THIS s.DSR |= (BX_FD_THIS s.command[0] & 0x7c);
      BX_FD_THIS s.option = BX_FD_THIS s.command[1];
      for (i = 0; i < 4; i++) {
        BX_FD_THIS s.cylinder[i] = BX_FD_THIS s.command[2+i];
      }
      BX_FD_THIS s.SRT = (BX_FD_THIS s.command[6] >> 4);
      BX_FD_THIS s.HUT = (BX_FD_THIS s.command[6] & 0x0f);
      BX_FD_THIS s.HLT = BX_FD_THIS s.command[7] >> 1;
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= (BX_FD_THIS s.command[7] & 1) ? FD_MS_NDMA : 0;
      BX_FD_THIS s.eot[drive] = BX_FD_THIS s.command[8];
      BX_FD_THIS s.lock = (BX_FD_THIS s.command[9] & 0x80) > 0;
      BX_FD_THIS s.perp_mode = (BX_FD_THIS s.command[9] & 0x7f);
      BX_FD_THIS s.config = BX_FD_THIS s.command[10];
      BX_FD_THIS s.pretrk = BX_FD_THIS s.command[11];
      BX_FD_THIS s.power_down = BX_FD_THIS s.command[12];
      enter_idle_phase();
      return;

    default: // invalid or unsupported command; these are captured in write() above
      BX_PANIC(("You should never get here! cmd = 0x%02x", BX_FD_THIS s.command[0]));
  }
}

void bx_floppy_ctrl_c::do_floppy_xfer(Bit8u *buffer, Bit8u drive, Bit8u fromto)
{
  // remember that not all floppies have two sides, multiply by s.head[drive]
  Bit32u logical_sector = (BX_FD_THIS s.cylinder[drive] * BX_FD_THIS s.media[drive].heads * BX_FD_THIS s.media[drive].sectors_per_track) +
                          (BX_FD_THIS s.head[drive] * BX_FD_THIS s.media[drive].sectors_per_track) +
                          (BX_FD_THIS s.sector[drive] - 1);

  // don't allow to read/write passed end of disk
  if (logical_sector >= BX_FD_THIS s.media[drive].sectors) {
    BX_ERROR(("LBA %d passed end of disk.", logical_sector));
    return;
  }

  floppy_xfer(drive, logical_sector * 512, buffer, 512, fromto);
  BX_FD_THIS s.floppy_buffer_index = 0;
}

void bx_floppy_ctrl_c::floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer, Bit32u bytes, Bit8u direction)
{
  int ret = 0;

  if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE)
    BX_PANIC(("floppy_xfer: bad drive #%d", drive));

  BX_DEBUG(("floppy_xfer: drive=%u, offset=%u, bytes=%u, direction=%s floppy",
            drive, offset, bytes, (direction==FROM_FLOPPY)? "from" : "to"));

#if BX_WITH_MACOS
  const char *pname = (drive == 0) ? BXPN_FLOPPYA_PATH : BXPN_FLOPPYB_PATH;
  if (strcmp(SIM->get_param_string(pname)->getptr(), SuperDrive))
#endif
  {
    if (BX_FD_THIS s.media[drive].vvfat_floppy) {
      ret = (int)BX_FD_THIS s.media[drive].vvfat->lseek(offset, SEEK_SET);
    } else {
      ret = (int)lseek(BX_FD_THIS s.media[drive].fd, offset, SEEK_SET);
    }
    if (ret < 0) {
      BX_PANIC(("could not perform lseek() to %d on floppy image file", offset));
      return;
    }
  }

  if (direction == FROM_FLOPPY) {
    if (BX_FD_THIS s.media[drive].vvfat_floppy) {
      ret = (int)BX_FD_THIS s.media[drive].vvfat->read(buffer, bytes);
#if BX_WITH_MACOS
    } else if (!strcmp(SIM->get_param_string(pname)->getptr(), SuperDrive)) {
      ret = fd_read((char *) buffer, offset, bytes);
#endif
    } else {
      ret = ::read(BX_FD_THIS s.media[drive].fd, (bx_ptr_t) buffer, bytes);
    }
    if (ret < int(bytes)) {
      if (ret > 0) {
        BX_ERROR(("partial read() on floppy image returns %u/%u",
          (unsigned) ret, (unsigned) bytes));
        memset(buffer + ret, 0, bytes - ret);
      } else {
        BX_ERROR(("read() on floppy image returns 0"));
        memset(buffer, 0, bytes);
      }
    }
  } else { // TO_FLOPPY
    BX_ASSERT (!BX_FD_THIS s.media[drive].write_protected);
    if (BX_FD_THIS s.media[drive].vvfat_floppy) {
      ret = (int)BX_FD_THIS s.media[drive].vvfat->write(buffer, bytes);
#if BX_WITH_MACOS
    } else if (!strcmp(SIM->get_param_string(pname)->getptr(), SuperDrive)) {
      ret = fd_write((char *) buffer, offset, bytes);
#endif
    } else {
      ret = ::write(BX_FD_THIS s.media[drive].fd, (bx_ptr_t) buffer, bytes);
    }
    if (ret < int(bytes)) {
      BX_PANIC(("could not perform write() on floppy image file"));
    }
  }
}

void bx_floppy_ctrl_c::timer_handler(void *this_ptr)
{
  bx_floppy_ctrl_c *class_ptr = (bx_floppy_ctrl_c *) this_ptr;
  class_ptr->timer();
}

void bx_floppy_ctrl_c::timer()
{
  Bit8u motor_on;
  
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
  switch (BX_FD_THIS s.pending_command) {
    // recal
    case FD_CMD_RECALIBRATE:
      BX_FD_THIS s.status_reg0 = 0x20 | drive;
      motor_on = ((BX_FD_THIS s.DOR>>(drive+4)) & 0x01);
      if ((BX_FD_THIS s.device_type[drive] == FDRIVE_NONE) || (motor_on == 0)) {
        BX_FD_THIS s.status_reg0 |= 0x50;
      }
      enter_idle_phase();
      BX_FD_THIS raise_interrupt();
      break;

    // seek
    case FD_CMD_SEEK:
    case FD_CMD_RELATIVE_SEEK:
    case FD_CMD_RELATIVE_SEEK | FD_CMD_DIR:
      enter_idle_phase();
      BX_FD_THIS raise_interrupt();
      break;

    // read ID
    case FD_CMD_READ_ID:
    case FD_CMD_READ_ID | FD_CMD_MFM:
      BX_DEBUG(("AFTER"));
      BX_DEBUG(("  drive    = %u", drive));
      BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
      BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
      BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));
      enter_result_phase();
      break;

    // write normal data
    case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MFM:
      if (BX_FD_THIS s.TC) { // Terminal Count line, done
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.status_reg1 = 0;
        BX_FD_THIS s.status_reg2 = 0;

        BX_DEBUG(("<<WRITE DONE>>"));
        BX_DEBUG(("AFTER"));
        BX_DEBUG(("  drive    = %u", drive));
        BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
        BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
        BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));

        enter_result_phase();
      } else {
        // transfer next sector
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      }
      break;

    // scan commands
    case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_EQUAL | FD_CMD_MFM:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM:
      if (BX_FD_THIS s.TC) { // Terminal Count line, done
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.status_reg1 = 0;
        // bit 2 = Scan Not Satisfied (sn), bit 3 = Scan Hit (sh)
        BX_FD_THIS s.status_reg2 = (0 << 3) | (1 << 2);

        BX_DEBUG(("<<SCAN DONE>>"));
        BX_DEBUG(("AFTER"));
        BX_DEBUG(("  drive    = %u", drive));
        BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
        BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
        BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));

        enter_result_phase();
      } else {
        // scan next sector
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      }
      break;

    // read normal data
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM:
    // read deleted/normal data
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM:
    // read track
    case FD_CMD_READ_TRACK | FD_CMD_MFM:
      // transfer next sector
      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
        BX_FD_THIS s.main_status_reg &= ~FD_MS_BUSY;  // clear busy bit
        BX_FD_THIS s.main_status_reg |= FD_MS_RQM | FD_MS_DIO;  // data byte waiting
      } else {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
      }
      break;

    // verify data
    case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM:
    case FD_CMD_VERIFY | FD_CMD_MFM | FD_CMD_SK:
    case FD_CMD_VERIFY | FD_CMD_MFM:
      // we continue until eot or BX_FD_THIS s.sector_count == 0
      BX_FD_THIS s.TC = end_of_track();
      if (BX_FD_THIS s.command[1] & (1<<7)) {
        if (--BX_FD_THIS s.sector_count == 0)
          BX_FD_THIS s.TC = 1;
      }
      if (BX_FD_THIS s.TC) { // Terminal Count line, done
        if ((BX_FD_THIS s.command[1] & (1<<7)) && (BX_FD_THIS s.sector_count > 0)) {
          BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive] << 2) | drive;
          BX_FD_THIS s.status_reg1 = 0x80; // end of cylinder
          BX_FD_THIS s.status_reg2 = 0;
        } else {
          increment_sector();
          BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
          BX_FD_THIS s.status_reg1 = 0;
          BX_FD_THIS s.status_reg2 = 0;
        }

        BX_DEBUG(("AFTER"));
        BX_DEBUG(("  drive    = %u", drive));
        BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
        BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
        BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));
        
        raise_interrupt();
        enter_result_phase();
      } else {
        increment_sector();
        // time to 'verify' one sector at 300 rpm
        Bit32u sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
      }
      break;

    // format track
    case FD_CMD_FORMAT_TRACK | FD_CMD_MFM:
    case FD_CMD_FORMAT_AND_WRITE | FD_CMD_MFM:
      if ((BX_FD_THIS s.sector_count == 0) || BX_FD_THIS s.TC) {
        BX_FD_THIS s.sector_count = 0;
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        enter_result_phase();
      } else {
        // transfer next sector
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      }
      break;

    // (contrived) RESET
    case FD_RESET:
      BX_FD_THIS reset(BX_RESET_SOFTWARE);
      BX_FD_THIS s.pending_command = FD_CMD_NOP;
      BX_FD_THIS s.status_reg0 = 0xc0;
      BX_FD_THIS raise_interrupt();
      BX_FD_THIS s.reset_sensei = 4;
      break;

    // nothing pending?
    case FD_CMD_NOP:
      break;

    default:
      BX_PANIC(("floppy:timer(): unknown case %02x", (unsigned) BX_FD_THIS s.pending_command));
  }
}

Bit16u bx_floppy_ctrl_c::dma_write(Bit8u *buffer, Bit16u maxlen)
{
  // A DMA write is from I/O to Memory
  // We need to return the next data byte(s) from the floppy buffer
  // to be transfered via the DMA to memory. (read block from floppy)
  //
  // maxlen is the maximum length of the DMA transfer

  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
  Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
  if (len > maxlen) len = maxlen;
  memcpy(buffer, &BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], len);
  BX_FD_THIS s.floppy_buffer_index += len;
  BX_FD_THIS s.TC = get_tc() && (len == maxlen);

  if ((BX_FD_THIS s.floppy_buffer_index >= 512) || BX_FD_THIS s.TC) {
    if (BX_FD_THIS s.floppy_buffer_index >= 512) {
      BX_FD_THIS s.floppy_buffer_index = 0;
    }
    if (BX_FD_THIS s.TC || end_of_track()) { // Terminal Count line, done
      if (end_of_track() && !BX_FD_THIS s.TC) {
        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.status_reg1 = 0x80;
        BX_FD_THIS s.status_reg2 = 0;
      } else {
        increment_sector(); // increment to next sector before retrieving next one
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.status_reg1 = 0;
        BX_FD_THIS s.status_reg2 = 0;
      }

      BX_DEBUG(("<<READ DONE>>"));
      BX_DEBUG(("AFTER"));
      BX_DEBUG(("  drive    = %u", drive));
      BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
      BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
      BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));

      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      enter_result_phase();
    } else { // more data to transfer
      increment_sector(); // increment to next sector before retrieving next one
      do_floppy_xfer(BX_FD_THIS s.floppy_buffer, drive, FROM_FLOPPY);
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // time to read one sector at 300 rpm
      Bit32u sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
    }
  }
  return len;
}

// Formatting is consecutive, from sector 1 to sector eot
// On physical hardware, we can set the sector ID to something different,
//  but all sectors are consecutively formatted starting with physical sector 1,
//  written in order to physical sector EOT.
Bit16u bx_floppy_ctrl_c::dma_read(Bit8u *buffer, Bit16u maxlen)
{
  // A DMA read is from Memory to I/O
  // We need to write the data_byte which was already transfered from memory
  // via DMA to I/O (write block to floppy)
  //
  // maxlen is the length of the DMA transfer (not implemented yet)

  // remember that you cannot initialize anything here that must
  //  remain static. The Guest can send as little as one byte at
  //  a time, which will call this function four times a sector,
  //  and up to 36 sectors a track.
  // however, most guests will send all bytes at once, but don't assume it will.
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
  Bit32u sector_time, fmt_sectors = 0;

  // format track in progress
  if ((BX_FD_THIS s.pending_command == (FD_CMD_MFM | FD_CMD_FORMAT_TRACK)) ||
      (BX_FD_THIS s.pending_command == (FD_CMD_MFM | FD_CMD_FORMAT_AND_WRITE))) {
    Bit16u retlen = 0;
    do {
      // are we in the middle of receiving the sector data?
      if (BX_FD_THIS s.format_write_flag) {
        Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
        if (len > maxlen) len = maxlen;
        memcpy(&BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], buffer, len);
        BX_FD_THIS s.floppy_buffer_index += len;
        BX_FD_THIS s.TC = get_tc() && (len == maxlen);
        if ((BX_FD_THIS s.floppy_buffer_index >= 512) || BX_FD_THIS s.TC) {
          fmt_sectors++;
          do_floppy_xfer(BX_FD_THIS s.floppy_buffer, drive, TO_FLOPPY);
          BX_FD_THIS s.sector[drive]++;
          BX_FD_THIS s.format_write_flag = 0;
        }
        buffer += len;
        maxlen -= len;
        retlen += len;

      // else we are getting the four header bytes
      } else {
        // only do the requested count of sectors
        if (BX_FD_THIS s.sector_count == 0)
          break;
        BX_FD_THIS s.sector_count--;
        switch (3 - (BX_FD_THIS s.sector_count & 0x03)) {
          // cylinder value (must be constant)
          case 0:
            if (*buffer < BX_FD_THIS s.media[drive].tracks) {
              // if this is the first header set, get cylinder value.
              //  we ignore any remaining cylinder values, but give error
              //  if they don't match the first given.
              if (BX_FD_THIS s.format_cylinder == 0xFFFF) {
                BX_FD_THIS s.format_cylinder = 
                  BX_FD_THIS s.cylinder[drive] = *buffer;
              } else if (*buffer != BX_FD_THIS s.format_cylinder) {
                BX_ERROR(("cylinder field must be constant: %d != %d", *buffer, BX_FD_THIS s.format_cylinder));
              }
            } else {
              BX_ERROR(("format track: cylinder out of range: %d >= %d", *buffer, BX_FD_THIS s.media[drive].tracks));
              if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
                DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
              }
              BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive;
              BX_FD_THIS s.status_reg1 = 0x04;
              BX_FD_THIS s.status_reg2 = 0x00;
              enter_result_phase();
              return 1;
            }
            break;

          // head value (must be constant)
          case 1:
            if (*buffer != BX_FD_THIS s.head[drive])
              BX_ERROR(("head number does not match head field"));
            break;

          // sector value (must be 1 -> eot) (does not need to be consecutive)
          // (we ignore the sector number since real hardware formats sector 1 to eot consecutively)
          case 2:
            if ((*buffer > 0) && (*buffer <= BX_FD_THIS s.media[drive].sectors_per_track)) {
              if (BX_FD_THIS s.format_sector_bp[*buffer - 1])
                BX_ERROR(("sector number already given: %d", *buffer));
              BX_FD_THIS s.format_sector_bp[*buffer - 1] = 1;
            } else {
              BX_ERROR(("sector is out of range: (%d) 1 -> %i", *buffer, BX_FD_THIS s.media[drive].sectors_per_track));
            }
            break;

          // size (must be 2 = 512-byte sectors)
          case 3:
            if (*buffer != 2)
              BX_ERROR(("dma_read: sector size %d not supported", 128<<(*buffer)));

            // we received all four header bytes, so write the sector
            BX_DEBUG(("formatting cylinder %u head %u sector %u",
                      BX_FD_THIS s.cylinder[drive], BX_FD_THIS s.head[drive],
                      BX_FD_THIS s.sector[drive]));
          
            // if we are format & write, we need to receive the 512-byte sector data
            if (BX_FD_THIS s.pending_command == (FD_CMD_MFM | FD_CMD_FORMAT_AND_WRITE)) {
              BX_FD_THIS s.format_write_flag = 1;

            // else just write 512 bytes of fillerbyte
            } else {
              for (unsigned i = 0; i < 512; i++) {
                BX_FD_THIS s.floppy_buffer[i] = BX_FD_THIS s.format_fillbyte;
              }
              do_floppy_xfer(BX_FD_THIS s.floppy_buffer, drive, TO_FLOPPY);
              BX_FD_THIS s.sector[drive]++;
              fmt_sectors++;
            }
            break;
        }
        buffer++;
        maxlen--;
        retlen++;
      }
    } while ((maxlen > 0) && (BX_FD_THIS s.sector[drive] <= BX_FD_THIS s.media[drive].sectors_per_track));
    if (fmt_sectors > 0) {
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // time to write one sector at 300 rpm
      sector_time = (200000 / BX_FD_THIS s.media[drive].sectors_per_track) * fmt_sectors;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
    }
    return retlen;

  // write normal data
  } else if ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_WRITE_NORMAL_DATA)) {
    Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
    if (len > maxlen) len = maxlen;
    memcpy(&BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], buffer, len);
    BX_FD_THIS s.floppy_buffer_index += len;
    BX_FD_THIS s.TC = get_tc() && (len == maxlen);

    if ((BX_FD_THIS s.floppy_buffer_index >= 512) || BX_FD_THIS s.TC) {
      do_floppy_xfer(BX_FD_THIS s.floppy_buffer, drive, TO_FLOPPY);
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // the following is a kludge; i (jc) don't know how to work with the timer
      if (((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) && BX_FD_THIS s.TC) || end_of_track()) {
        if (end_of_track() && !BX_FD_THIS s.TC) {
          BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive] << 2) | drive;
          BX_FD_THIS s.status_reg1 = 0x80;
          BX_FD_THIS s.status_reg2 = 0;
        } else {
          increment_sector(); // increment to next sector before retrieving next one
          BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
          BX_FD_THIS s.status_reg1 = 0;
          BX_FD_THIS s.status_reg2 = 0;
        }
        enter_result_phase();
      }
      // time to write one sector at 300 rpm
      increment_sector();
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
    }
    return len;

  // scan commands
  } else if (((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_EQUAL)) ||
             ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_LOW_EQUAL)) ||
             ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_HIGH_EQUAL))) {
    Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
    if (len > maxlen) len = maxlen;
    memcpy(&BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], buffer, len);
    BX_FD_THIS s.floppy_buffer_index += len;
    BX_FD_THIS s.TC = get_tc() && (len == maxlen);

    if ((BX_FD_THIS s.floppy_buffer_index >= 512) || BX_FD_THIS s.TC || end_of_track()) {
      bool sh = 0, sn = 0;
      if ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_EQUAL)) {
        // scan_equal compares all bytes within the sector: pass = true if all bytes match
        for (int i=0; i<512; i++) {
          if (BX_FD_THIS s.scan_buffer[i] != BX_FD_THIS s.floppy_buffer[i]) {
            sn = 1;
            break;
          }
        }
      } else if ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_LOW_EQUAL)) {
        // scan_low_equal compares all bytes within the sector: pass = true if all bytes in s.scan_buffer[512] <= buffer[512]
        for (int i=0; i<512; i++) {
          if (BX_FD_THIS s.scan_buffer[i] == BX_FD_THIS s.floppy_buffer[i])
            sh = 1;
          else if (BX_FD_THIS s.scan_buffer[i] > BX_FD_THIS s.floppy_buffer[i]) {
            sh = 0;
            sn = 1;
            break;
          }
        }
      } else if ((BX_FD_THIS s.pending_command & 0x5f) == (FD_CMD_MFM | FD_CMD_SCAN_HIGH_EQUAL)) {
        // scan_high_equal compares all bytes within the sector: pass = true if all bytes in s.scan_buffer[512] >= buffer[512]
        for (int i=0; i<512; i++) {
          if (BX_FD_THIS s.scan_buffer[i] == BX_FD_THIS s.floppy_buffer[i])
            sh = 1;
          else if (BX_FD_THIS s.scan_buffer[i] < BX_FD_THIS s.floppy_buffer[i]) {
            sh = 0;
            sn = 1;
            break;
          }
        }
      }
      // we found a match?
      if (!sn) {
        // IC=0 (normal termination), SE=0, EC=0, Head, Drive
        BX_FD_THIS s.status_reg0 = 0x00 | (BX_FD_THIS s.head[drive]<<2) | drive;
        BX_FD_THIS s.status_reg1 = 0x00;
        // bit 2 = Scan Not Satisfied (sn), bit 3 = Scan Hit (sh)
        BX_FD_THIS s.status_reg2 = (sh << 3) | (sn << 2);
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
        } else {
          BX_FD_THIS s.main_status_reg &= ~FD_MS_NDMA;
        }
        enter_result_phase();
        return 1;
      }
      // else move to the next sector
      do_floppy_xfer(BX_FD_THIS s.scan_buffer, drive, FROM_FLOPPY);
      for (Bit8u i=0; i<BX_FD_THIS s.stp; i++)
        increment_sector();
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // the following is a kludge; i (jc) don't know how to work with the timer
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) && BX_FD_THIS s.TC) {
        enter_result_phase();
      }
      // time to write one sector at 300 rpm
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
    }
    return len;
  } else {
    BX_PANIC(("Unknown pending command in dma_read(): %02x", BX_FD_THIS s.pending_command));
    return 0;
  }
}

void bx_floppy_ctrl_c::raise_interrupt(void)
{
  DEV_pic_raise_irq(6);
  BX_FD_THIS s.pending_irq = 1;
  BX_FD_THIS s.reset_sensei = 0;
}

void bx_floppy_ctrl_c::lower_interrupt(void)
{
  if (BX_FD_THIS s.pending_irq) {
    DEV_pic_lower_irq(6);
    BX_FD_THIS s.pending_irq = 0;
  }
}

// is the current sector the end of the track?
bool bx_floppy_ctrl_c::end_of_track(void)
{
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;

  bool eot_reached = 0;
  if (BX_FD_THIS s.sector[drive] == BX_FD_THIS s.eot[drive]) {
    if (!BX_FD_THIS s.multi_track)
      eot_reached = 1;
    else if ((BX_FD_THIS s.head[drive] + 1) == (int)BX_FD_THIS s.media[drive].heads)
      eot_reached = 1;
  }

  return eot_reached;
}

void bx_floppy_ctrl_c::increment_sector(void)
{
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;

  // values after completion of data xfer
  // ??? calculation depends on base_count being multiple of 512
  BX_FD_THIS s.sector[drive]++;
  if ((BX_FD_THIS s.sector[drive] > BX_FD_THIS s.eot[drive]) ||
      (BX_FD_THIS s.sector[drive] > BX_FD_THIS s.media[drive].sectors_per_track)) {
    BX_FD_THIS s.sector[drive] = 1;
    if (BX_FD_THIS s.multi_track) {
      BX_FD_THIS s.head[drive]++;
      if (BX_FD_THIS s.head[drive] > 1) {
        BX_FD_THIS s.head[drive] = 0;
        BX_FD_THIS s.cylinder[drive]++;
        reset_changeline();
      }
    } else {
      BX_FD_THIS s.cylinder[drive]++;
      reset_changeline();
    }
    if (BX_FD_THIS s.cylinder[drive] >= BX_FD_THIS s.media[drive].tracks) {
      // Set to 1 past last possible cylinder value.
      // I notice if I set it to tracks-1, prama linux won't boot.
      BX_FD_THIS s.cylinder[drive] = BX_FD_THIS s.media[drive].tracks;
      BX_INFO(("increment_sector: clamping cylinder to max"));
    }
  }
}

bool bx_floppy_ctrl_c::set_media_status(unsigned drive, bool status)
{
  bx_list_c *floppy;

  if (drive == 0)
    floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYA);
  else
    floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYB);

  unsigned type = SIM->get_param_enum("type", floppy)->get();
  // if setting to the current value, nothing to do
  if ((status == BX_FD_THIS s.media_present[drive]) &&
      ((status == 0) || (type == BX_FD_THIS s.media[drive].type)))
    return status;

  if (status == 0) {
    // eject floppy
    close_media(&BX_FD_THIS s.media[drive]);
    BX_FD_THIS s.media_present[drive] = 0;
    SIM->get_param_enum("status", floppy)->set(BX_EJECTED);
    BX_FD_THIS s.DIR[drive] |= 0x80; // disk changed line
    return false;
  } else {
    // insert floppy
    const char *path = SIM->get_param_string("path", floppy)->getptr();
    if (!strcmp(path, "none"))
      return false;
    if (evaluate_media(BX_FD_THIS s.device_type[drive], type, path, & BX_FD_THIS s.media[drive])) {
      BX_FD_THIS s.media_present[drive] = 1;
      if (drive == 0) {
#define MED (BX_FD_THIS s.media[0])
        BX_INFO(("fd0: '%s' ro=%d, h=%d,t=%d,spt=%d",
          SIM->get_param_string("path", floppy)->getptr(),
          MED.write_protected, MED.heads, MED.tracks, MED.sectors_per_track));
        if (MED.write_protected)
          SIM->get_param_bool("readonly", floppy)->set(1);
#undef MED
        SIM->get_param_enum("status", floppy)->set(BX_INSERTED);
      } else {
#define MED (BX_FD_THIS s.media[1])
        BX_INFO(("fd1: '%s' ro=%d, h=%d,t=%d,spt=%d",
          SIM->get_param_string("path", floppy)->getptr(),
          MED.write_protected, MED.heads, MED.tracks, MED.sectors_per_track));
        if (MED.write_protected)
          SIM->get_param_bool("readonly", floppy)->set(1);
#undef MED
        SIM->get_param_enum("status", floppy)->set(BX_INSERTED);
      }
      return true;
    } else {
      BX_FD_THIS s.media_present[drive] = 0;
      SIM->get_param_enum("status", floppy)->set(BX_EJECTED);
      SIM->get_param_enum("type", floppy)->set(BX_FLOPPY_NONE);
      return false;
    }
  }
}

#ifdef O_BINARY
#define BX_RDONLY O_RDONLY | O_BINARY
#define BX_RDWR O_RDWR | O_BINARY
#else
#define BX_RDONLY O_RDONLY
#define BX_RDWR O_RDWR
#endif

bool bx_floppy_ctrl_c::evaluate_media(Bit8u devtype, Bit8u type, const char *path, floppy_t *media)
{
  struct stat stat_buf;
  int i, ret;
  int type_idx = -1;
#ifdef __linux__
  struct floppy_struct floppy_geom;
#endif
#ifdef WIN32
  char sTemp[1024];
  bool raw_floppy = 0;
  HANDLE hFile;
  DWORD bytes;
  DISK_GEOMETRY dg;
  unsigned tracks = 0, heads = 0, spt = 0;
#endif

  //If media file is already open, close it before reopening.
  close_media(media);

  // check media type
  if (type == BX_FLOPPY_NONE) {
    return 0;
  }
  for (i = 0; i < 8; i++) {
    if (type == floppy_type[i].id) type_idx = i;
  }
  if (type_idx == -1) {
    BX_ERROR(("evaluate_media: unknown media type %d", type));
    return 0;
  }
  if ((floppy_type[type_idx].drive_mask & devtype) == 0) {
    BX_ERROR(("evaluate_media: media type %d not valid for this floppy drive", type));
    return 0;
  }

  // use virtual VFAT support if requested
  if (!strncmp(path, "vvfat:", 6) && (devtype == FDRIVE_350HD)) {
    media->vvfat = DEV_hdimage_init_image("vvfat", 1474560, "");
    if (media->vvfat != NULL) {
      if (media->vvfat->open(path + 6) == 0) {
        media->type              = BX_FLOPPY_1_44;
        media->tracks            = media->vvfat->cylinders;
        media->heads             = media->vvfat->heads;
        media->sectors_per_track = media->vvfat->spt;
        media->sectors           = 2880;
        media->vvfat_floppy = 1;
        media->fd = 0;
      }
    }
    if (media->vvfat_floppy) return 1;
  }
  // open media file (image file or device)
#ifdef macintosh
  media->fd = 0;
  if (strcmp(path, SuperDrive))
#endif
#ifdef WIN32
  if ((isalpha(path[0])) && (path[1] == ':') && (strlen(path) == 2)) {
    raw_floppy = 1;
    wsprintf(sTemp, "\\\\.\\%s", path);
    hFile = CreateFile(sTemp, GENERIC_READ, FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
      BX_ERROR(("Cannot open floppy drive"));
      return(0);
    }
    if (!DeviceIoControl(hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &dg, sizeof(dg), &bytes, NULL)) {
      BX_ERROR(("No media in floppy drive"));
      CloseHandle(hFile);
      return(0);
    } else {
      tracks = (unsigned)dg.Cylinders.QuadPart;
      heads  = (unsigned)dg.TracksPerCylinder;
      spt    = (unsigned)dg.SectorsPerTrack;
    }
    CloseHandle(hFile);
    if (!media->write_protected)
      media->fd = open(sTemp, BX_RDWR);
    else
      media->fd = open(sTemp, BX_RDONLY);
  }
  else
#endif
  {
    if (!media->write_protected)
      media->fd = open(path, BX_RDWR);
    else
      media->fd = open(path, BX_RDONLY);
  }

  if (!media->write_protected && (media->fd < 0)) {
    BX_INFO(("tried to open '%s' read/write: %s",path,strerror(errno)));
    // try opening the file read-only
    media->write_protected = 1;
#ifdef macintosh
    media->fd = 0;
    if (strcmp(path, SuperDrive))
#endif
#ifdef WIN32
    if (raw_floppy == 1)
      media->fd = open(sTemp, BX_RDONLY);
    else
#endif
      media->fd = open(path, BX_RDONLY);

    if (media->fd < 0) {
      // failed to open read-only too
      BX_INFO(("tried to open '%s' read only: %s",path,strerror(errno)));
      media->type = type;
      return(0);
    }
  }

#if BX_WITH_MACOS
  if (!strcmp(path, SuperDrive))
    ret = fd_stat(&stat_buf);
  else
    ret = fstat(media->fd, &stat_buf);
#elif defined(WIN32)
  if (raw_floppy) {
    memset (&stat_buf, 0, sizeof(stat_buf));
    stat_buf.st_mode = S_IFCHR;
    ret = 0;
  }
  else
#endif
  { // unix
    if (media->fd >= 0) {
      ret = fstat(media->fd, &stat_buf);
    } else {
      ret = EBADF;
    }
  }
  if (ret) {
    BX_PANIC(("fstat floppy 0 drive image file returns error: %s", strerror(errno)));
    return(0);
  }

  if (S_ISREG(stat_buf.st_mode)) {
    // regular file
    switch (type) {
      // use CMOS reserved types
      case BX_FLOPPY_160K: // 160K 5.25"
      case BX_FLOPPY_180K: // 180K 5.25"
      case BX_FLOPPY_320K: // 320K 5.25"
      // standard floppy types
      case BX_FLOPPY_360K: // 360K 5.25"
      case BX_FLOPPY_720K: // 720K 3.5"
      case BX_FLOPPY_1_2:  // 1.2M 5.25"
      case BX_FLOPPY_2_88: // 2.88M 3.5"
        media->type              = type;
        media->tracks            = floppy_type[type_idx].trk;
        media->heads             = floppy_type[type_idx].hd;
        media->sectors_per_track = floppy_type[type_idx].spt;
        media->sectors           = floppy_type[type_idx].sectors;
        if (stat_buf.st_size > (int)(media->sectors * 512)) {
          BX_ERROR(("evaluate_media: size of file '%s' (%lu) too large for selected type",
                   path, (unsigned long) stat_buf.st_size));
          return 0;
        }
        break;
      default: // 1.44M 3.5"
        media->type              = type;
        if (stat_buf.st_size <= 1474560) {
          media->tracks            = floppy_type[type_idx].trk;
          media->heads             = floppy_type[type_idx].hd;
          media->sectors_per_track = floppy_type[type_idx].spt;
        }
        else if (stat_buf.st_size == 1720320)
        {
          media->sectors_per_track = 21;
          media->tracks            = 80;
          media->heads             = 2;
        }
        else if (stat_buf.st_size == 1763328)
        {
          media->sectors_per_track = 21;
          media->tracks            = 82;
          media->heads             = 2;
        }
        else if (stat_buf.st_size == 1884160)
        {
          media->sectors_per_track = 23;
          media->tracks            = 80;
          media->heads             = 2;
        }
        else
        {
          BX_ERROR(("evaluate_media: file '%s' of unknown size %lu",
            path, (unsigned long) stat_buf.st_size));
          return 0;
        }
        media->sectors = media->heads * media->tracks * media->sectors_per_track;
    }
    return (media->sectors > 0); // success
  }

  else if (S_ISCHR(stat_buf.st_mode)
#if BX_WITH_MACOS == 0
#ifdef S_ISBLK
            || S_ISBLK(stat_buf.st_mode)
#endif
#endif
           ) {
    // character or block device
    // assume media is formatted to typical geometry for drive
    media->type              = type;
#ifdef __linux__
    if (ioctl(media->fd, FDGETPRM, &floppy_geom) < 0) {
      BX_ERROR(("cannot determine media geometry, trying to use defaults"));
      media->tracks            = floppy_type[type_idx].trk;
      media->heads             = floppy_type[type_idx].hd;
      media->sectors_per_track = floppy_type[type_idx].spt;
      media->sectors           = floppy_type[type_idx].sectors;
      return (media->sectors > 0);
    }
    media->tracks            = floppy_geom.track;
    media->heads             = floppy_geom.head;
    media->sectors_per_track = floppy_geom.sect;
    media->sectors           = floppy_geom.size;
#elif defined(WIN32)
    media->tracks            = tracks;
    media->heads             = heads;
    media->sectors_per_track = spt;
    media->sectors = media->heads * media->tracks * media->sectors_per_track;
#else
    media->tracks            = floppy_type[type_idx].trk;
    media->heads             = floppy_type[type_idx].hd;
    media->sectors_per_track = floppy_type[type_idx].spt;
    media->sectors           = floppy_type[type_idx].sectors;
#endif
    return (media->sectors > 0); // success
  } else {
    // unknown file type
    BX_ERROR(("unknown mode type"));
    return 0;
  }
}

void bx_floppy_ctrl_c::close_media(floppy_t *media)
{
  if (media->fd >= 0) {
    if (media->vvfat_floppy) {
      media->vvfat->close();
      delete media->vvfat;
      media->vvfat_floppy = 0;
    } else {
      close(media->fd);
    }
    media->fd = -1;
  }
}

// cmd is the 8-bit value sent to the controller
// mask will be used to mask off optional bits
// supported is the controller type(s) supported for this command
typedef struct {
  Bit8u cmd;
  Bit8u mask;
  unsigned supported;
} fdc_type_supported;

static const fdc_type_supported fdc_supported[] = {
  { FD_CMD_MODE,                 0x00,
        FDC_TYPE_DP8473 | FDC_TYPE_PC87306 | FDC_TYPE_BOCHS  },
  { FD_CMD_READ_TRACK,           FD_CMD_MFM,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_SPECIFY,              0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_SENSE_DRV_STATUS,     0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_WRITE_NORMAL_DATA,    FD_CMD_MT | FD_CMD_MFM,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_READ_NORMAL_DATA,     FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_RECALIBRATE,          0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_SENSE_INT_STATUS,     0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  /*{ FD_CMD_WRITE_DELETED_DATA,    FD_CMD_MT | FD_CMD_MFM,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 }, */
  { FD_CMD_READ_ID,              FD_CMD_MFM,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_MOTOR_ON_OFF,         0x00,
       FDC_TYPE_BOCHS },
  { FD_CMD_READ_DELETED_DATA,    FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_FORMAT_TRACK,         FD_CMD_MFM,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_DUMPREG,              0x00, 
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_SEEK,                 0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_VERSION,              0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_SCAN_EQUAL,           FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_PERPENDICULARE_MODE,  0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_CONFIGURE,            0x00,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_LOCK_UNLOCK,          FD_CMD_LOCK,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_VERIFY,               FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_POWER_DOWN_MODE,      0x00,
      // must not have both Power Down and StandBy supported at the same time.
      FDC_TYPE_82078 /* | FDC_TYPE_BOCHS */ },
  { FD_CMD_PART_ID,              0x00,
      FDC_TYPE_82078 | FDC_TYPE_BOCHS | FDC_TYPE_PC87306 },
  { FD_CMD_SCAN_LOW_EQUAL,       FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK,
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_SCAN_HIGH_EQUAL,      FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK, 
        FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS | 
        FDC_TYPE_DP8473 | FDC_TYPE_PC87306 },
  { FD_CMD_SET_TRACK,            FD_CMD_DIR,
      FDC_TYPE_DP8473 | FDC_TYPE_BOCHS | FDC_TYPE_PC87306 },
  { FD_CMD_SAVE,                 0x00, 
      FDC_TYPE_82078 | FDC_TYPE_BOCHS },
  { FD_CMD_OPTION,               0x00,
      FDC_TYPE_82078 | FDC_TYPE_BOCHS },
  { FD_CMD_EXIT_STANDBY_MODE,    0x00,
      FDC_TYPE_BOCHS },
  { FD_CMD_ENTER_STANDBY_MODE,   0x00,
      FDC_TYPE_BOCHS },
  { FD_CMD_HARD_RESET,           0x00,
      FDC_TYPE_BOCHS },
  { FD_CMD_RESTORE,              0x00,
      FDC_TYPE_82078 | FDC_TYPE_BOCHS },
  { FD_CMD_DRIVE_SPECIFICATION,  0x00,
      FDC_TYPE_82078 | FDC_TYPE_BOCHS },
  { FD_CMD_RELATIVE_SEEK,        FD_CMD_DIR,
      FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_BOCHS |
      FDC_TYPE_37c78 | FDC_TYPE_PC87306 },
  { FD_CMD_FORMAT_AND_WRITE,     FD_CMD_MFM,
        FDC_TYPE_82078 | FDC_TYPE_BOCHS },
  { 0xFF, }
};

bool bx_floppy_ctrl_c::command_supported(Bit8u command) {
  int i = 0;

  while (fdc_supported[i].cmd != 0xFF) {
    if (((command & ~fdc_supported[i].mask) == fdc_supported[i].cmd) &&
         (fdc_supported[i].supported & FDC_CURRENT_TYPE))
      return true;
    i++;
  }

  return false;
}

void bx_floppy_ctrl_c::enter_result_phase(void)
{
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;

  /* these are always the same */
  BX_FD_THIS s.result_index = 0;
  // not necessary to clear any status bits, we're about to set them all
  BX_FD_THIS s.main_status_reg |= FD_MS_RQM | FD_MS_DIO | FD_MS_BUSY;

  // invalid command?
  if ((BX_FD_THIS s.status_reg0 & 0xc0) == 0x80) {
    BX_FD_THIS s.result_size = 1;
    BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
    BX_DEBUG(("enter_result_phase: Returning invalid command"));
  } else {
    // might be a valid command
    switch (BX_FD_THIS s.pending_command) {
      // get status
      case FD_CMD_SENSE_DRV_STATUS:
        BX_FD_THIS s.result_size = 1;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg3;
        break;

      // sense interrupt status
      case FD_CMD_SENSE_INT_STATUS:
        BX_FD_THIS s.result_size = 2;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
        BX_FD_THIS s.result[1] = (Bit8u) BX_FD_THIS s.cylinder[drive];
#if (FDC_CURRENT_TYPE & FDC_TYPE_DP8473)
        if (BX_FD_THIS s.mode0 & FLAGS_ETR) {
          BX_FD_THIS s.result[2] = (BX_FD_THIS s.cylinder[drive] >> 4) & 0x0F;
          BX_FD_THIS s.result_size++;
        }
#endif
        lower_interrupt();
        break;

      // dump registers
      case FD_CMD_DUMPREG:
        BX_FD_THIS s.result_size = 10;
        for (int i = 0; i < 4; i++) {
          BX_FD_THIS s.result[i] = (Bit8u) BX_FD_THIS s.cylinder[i];
        }
        BX_FD_THIS s.result[4] = (BX_FD_THIS s.SRT << 4) | BX_FD_THIS s.HUT;
        BX_FD_THIS s.result[5] = (BX_FD_THIS s.HLT << 1) | ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) ? 1 : 0);
        BX_FD_THIS s.result[6] = BX_FD_THIS s.eot[drive];
        BX_FD_THIS s.result[7] = (BX_FD_THIS s.lock << 7) | (BX_FD_THIS s.perp_mode & 0x7f);
        BX_FD_THIS s.result[8] = BX_FD_THIS s.config;
        BX_FD_THIS s.result[9] = BX_FD_THIS s.pretrk;
        break;

      // Version
      case FD_CMD_VERSION:
        BX_FD_THIS s.result_size = 1;
#if (FDC_CURRENT_TYPE & (FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_37c78))
        BX_FD_THIS s.result[0] = 0x90;
#else
        BX_FD_THIS s.result[0] = 0x80; // unknown controller type used (shouldn't happen)
#endif
        break;

      // Part ID command
      case FD_CMD_PART_ID:
        BX_FD_THIS s.result_size = 1;
#if (FDC_CURRENT_TYPE & FDC_TYPE_PC87306)
        BX_FD_THIS s.result[0] = 0x73;
#else
        BX_FD_THIS s.result[0] = 0x01;
  #if ((FDC_CURRENT_TYPE & FDC_TYPE_82078) && !FDC_TYPE_82078SL)
        BX_FD_THIS s.result[0] |= 0x40; // 82078 uses bits 7:5 as a version number
  #endif
#endif
        break;

      // Unlock command
      case FD_CMD_LOCK_UNLOCK:
      // Lock command
      case FD_CMD_LOCK_UNLOCK | FD_CMD_LOCK:
        BX_FD_THIS s.lock = (BX_FD_THIS s.pending_command & 0x80) > 0;
        BX_FD_THIS s.result_size = 1;
        BX_FD_THIS s.result[0] = (BX_FD_THIS s.lock << 4);
        break;

      // read ID
      case FD_CMD_READ_ID:
      case FD_CMD_READ_ID | FD_CMD_MFM:
      // format track
      case FD_CMD_FORMAT_TRACK | FD_CMD_MFM:
      case FD_CMD_FORMAT_AND_WRITE | FD_CMD_MFM:
      // write normal data
      case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_WRITE_NORMAL_DATA | FD_CMD_MFM:
      // read normal data
      case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_READ_NORMAL_DATA | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_READ_NORMAL_DATA | FD_CMD_MFM:
      // read deleted/normal data
      case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_READ_DELETED_DATA | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_READ_DELETED_DATA | FD_CMD_MFM:
      // verify data
      case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_VERIFY | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_VERIFY | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_VERIFY | FD_CMD_MFM:
      // scan commands
      case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_EQUAL | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_SCAN_EQUAL | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_EQUAL | FD_CMD_MFM:
      case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_LOW_EQUAL | FD_CMD_MFM:
      case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MT | FD_CMD_MFM:
      case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM | FD_CMD_SK:
      case FD_CMD_SCAN_HIGH_EQUAL | FD_CMD_MFM:
        BX_FD_THIS s.result_size = 7;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
        BX_FD_THIS s.result[1] = BX_FD_THIS s.status_reg1;
        BX_FD_THIS s.result[2] = BX_FD_THIS s.status_reg2;
        BX_FD_THIS s.result[3] = (Bit8u) BX_FD_THIS s.cylinder[drive];
        BX_FD_THIS s.result[4] = BX_FD_THIS s.head[drive];
        BX_FD_THIS s.result[5] = BX_FD_THIS s.sector[drive];
        BX_FD_THIS s.result[6] = 2; /* sector size code */
#if (FDC_CURRENT_TYPE & (FDC_TYPE_82077AA | FDC_TYPE_82078 | FDC_TYPE_37c78))
        // these fdc types return undefined for bytes 3, 4, 5, and 6 for the following command(s)
        switch (BX_FD_THIS s.pending_command) {
          // format track
          case FD_CMD_FORMAT_TRACK | FD_CMD_MFM:
          case FD_CMD_FORMAT_AND_WRITE | FD_CMD_MFM:
            BX_FD_THIS s.result[3] = 
            BX_FD_THIS s.result[4] = 
            BX_FD_THIS s.result[5] = 
            BX_FD_THIS s.result[6] = 0;
            break;
        }
#endif
        BX_FD_THIS raise_interrupt();
        break;

      // read track
      case FD_CMD_READ_TRACK | FD_CMD_MFM:
        BX_FD_THIS s.result_size = 7;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
        BX_FD_THIS s.result[1] = BX_FD_THIS s.status_reg1;
        BX_FD_THIS s.result[2] = BX_FD_THIS s.status_reg2;
        // if we didn't read the whole track, the cyl value
        //  is preserved, and the sector value is set to 1
        if (BX_FD_THIS s.sector[drive] != 1) {
          BX_FD_THIS s.result[3] = (Bit8u) BX_FD_THIS s.command[2];
          BX_FD_THIS s.sector[drive] = 1;
          BX_FD_THIS s.result[1] |= 0x04; // didn't find the correct sequence/eot
        } else
          BX_FD_THIS s.result[3] = (Bit8u) BX_FD_THIS s.cylinder[drive];
        // head value does not change, it must be preserved
        BX_FD_THIS s.result[4] = BX_FD_THIS s.command[3];
        BX_FD_THIS s.result[5] = BX_FD_THIS s.sector[drive];
        BX_FD_THIS s.result[6] = 2; /* sector size code */
        BX_FD_THIS raise_interrupt();
        break;

      // Power down mode
      case FD_CMD_POWER_DOWN_MODE:
        BX_FD_THIS s.result_size = 1;
        BX_FD_THIS s.result[ 0] = BX_FD_THIS s.power_down;
        break;

      // drive specification
      case FD_CMD_DRIVE_SPECIFICATION:
        BX_FD_THIS s.result_size = 4;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.command[0] & 0x1F;
        BX_FD_THIS s.result[1] = BX_FD_THIS s.command[1] & 0x1F;
        BX_FD_THIS s.result[2] = 0;
        BX_FD_THIS s.result[3] = 0;
        break;

      // Set Track
      case FD_CMD_SET_TRACK:
        BX_FD_THIS s.result_size = 1;
        drive = (BX_FD_THIS s.command[1] & FDC_DRV_MASK);
        // bit 2 = set = MSB, else LSB
        if (BX_FD_THIS s.command[1] & 0x04) {
          BX_FD_THIS s.result[0] = (BX_FD_THIS s.cylinder[drive] >> 8) & 0xFF;
        } else {
          BX_FD_THIS s.result[0] = (BX_FD_THIS s.cylinder[drive] >> 0) & 0xFF;
        }
        break;

      // save
      case FD_CMD_SAVE:
        BX_FD_THIS s.result_size = 16;
        BX_FD_THIS s.result[ 0] = BX_FD_THIS s.DSR & 0x7f;
        BX_FD_THIS s.result[ 1] = BX_FD_THIS s.option;
        for (int i = 0; i < 4; i++) {
          BX_FD_THIS s.result[2+i] = (Bit8u) BX_FD_THIS s.cylinder[i];
        }
        BX_FD_THIS s.result[ 6] = (BX_FD_THIS s.SRT << 4) | BX_FD_THIS s.HUT;
        BX_FD_THIS s.result[ 7] = (BX_FD_THIS s.HLT << 1) | ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) ? 1 : 0);
        BX_FD_THIS s.result[ 8] = BX_FD_THIS s.eot[drive];
        BX_FD_THIS s.result[ 9] = (BX_FD_THIS s.lock << 7) | (BX_FD_THIS s.perp_mode & 0x7f);
        BX_FD_THIS s.result[10] = BX_FD_THIS s.config;
        BX_FD_THIS s.result[11] = BX_FD_THIS s.pretrk;
        BX_FD_THIS s.result[12] = BX_FD_THIS s.power_down;
        BX_FD_THIS s.result[13] = 0; // Disk Status (internal to the controller)
        BX_FD_THIS s.result[14] = 0; // reserved for future use (ha! ha!)
        BX_FD_THIS s.result[15] = 0; // reserved for future use (ha! ha!)
        break;

      default:  // invalid commands are to return a single result byte of 0x80
        BX_FD_THIS s.result_size = 1;
        BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
    }
  }
  
  // Print command result (max MAX_PHASE_SIZE bytes)
  char buf[8+(MAX_PHASE_SIZE*5)+1], *p = buf;
  p += sprintf(p, "RESULT: ");
  for (Bit8u i=0; i<BX_FD_THIS s.result_size; i++) {
    p += sprintf(p, "[%02x] ", (unsigned) BX_FD_THIS s.result[i]);
  }
  BX_DEBUG(("%s", buf));
}

void bx_floppy_ctrl_c::enter_idle_phase(void)
{
  BX_FD_THIS s.main_status_reg &= (FD_MS_NDMA | 0x0f);  // leave drive status untouched
  BX_FD_THIS s.main_status_reg |= FD_MS_RQM; // data register ready

  BX_FD_THIS s.command_complete = true; /* waiting for new command */
  BX_FD_THIS s.command_index = 0;
  BX_FD_THIS s.command_size = 0;
  BX_FD_THIS s.pending_command = FD_CMD_NOP;
  BX_FD_THIS s.result_size = 0;

  BX_FD_THIS s.floppy_buffer_index = 0;
}

Bit32u bx_floppy_ctrl_c::calculate_step_delay(Bit8u drive, Bit8u new_cylinder)
{
  Bit8u steps;
  if (new_cylinder == BX_FD_THIS s.cylinder[drive]) {
    steps = 1;
  } else {
    steps = abs(new_cylinder - BX_FD_THIS s.cylinder[drive]);
    reset_changeline();
  }
  Bit32u one_step_delay = ((BX_FD_THIS s.SRT ^ 0x0f) + 1) * 500000 / drate_in_k[(BX_FD_THIS s.DSR & 0x03)];
  return (steps * one_step_delay);
}

void bx_floppy_ctrl_c::reset_changeline(void)
{
  Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
  if (BX_FD_THIS s.media_present[drive])
    BX_FD_THIS s.DIR[drive] &= ~0x80;
}

bool bx_floppy_ctrl_c::get_tc(void)
{
  bool terminal_count;
  if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
    Bit8u drive = BX_FD_THIS s.DOR & FDC_DRV_MASK;
    /* figure out if we've sent all the data, in non-DMA mode...
     * the drive stays on the same cylinder for a read or write, so that's
     * not going to be an issue. EOT stands for the last sector to be I/Od.
     * it does all the head 0 sectors first, then the second if any.
     * now, regarding reaching the end of the sector:
     *  == 512 would make it more precise, allowing one to spot bugs...
     *  >= 512 makes it more robust, but allows for sloppy code...
     *  pick your poison?
     * note: byte and head are 0-based; eot, sector, and heads are 1-based. */
    terminal_count = ((BX_FD_THIS s.floppy_buffer_index == 512) &&
     (BX_FD_THIS s.sector[drive] == BX_FD_THIS s.eot[drive]) &&
     (BX_FD_THIS s.head[drive] == (BX_FD_THIS s.media[drive].heads - 1)));
  } else {
    terminal_count = DEV_dma_get_tc();
  }
  return terminal_count;
}

// floppy runtime parameter handling

Bit64s bx_floppy_ctrl_c::floppy_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  bx_list_c *base = (bx_list_c*) param->get_parent();

  if (set) {
    Bit8u drive = atoi(base->get_name());
    if (!strcmp(param->get_name(), "status")) {
      BX_FD_THIS s.media[drive].status_changed = 1;
    } else if (!strcmp(param->get_name(), "readonly")) {
      BX_FD_THIS s.media[drive].write_protected = (bool)val;
      BX_FD_THIS s.media[drive].status_changed = 1;
    }
  }
  return val;
}

const char* bx_floppy_ctrl_c::floppy_param_string_handler(bx_param_string_c *param,
                                bool set, const char *oldval, const char *val, int maxlen)
{
  char pname[BX_PATHNAME_LEN];

  bx_list_c *base = (bx_list_c*) param->get_parent();
  if ((strlen(val) < 1) || !strcmp ("none", val)) {
    val = "none";
  }
  param->get_param_path(pname, BX_PATHNAME_LEN);
  if ((!strncmp(pname, "floppy", 6)) && (!strcmp(param->get_name(), "path"))) {
    if (set) {
      Bit8u drive = atoi(base->get_name());
      if (SIM->get_param_enum("devtype", base)->get() == BX_FDD_NONE) {
        BX_ERROR(("Cannot add a floppy drive at runtime"));
        SIM->get_param_string("path", base)->set("none");
      }
      if (SIM->get_param_enum("status", base)->get() == BX_INSERTED) {
        // tell the device model that we removed, then inserted the disk
        BX_FD_THIS s.media[drive].status_changed = 1;
      }
    }
  } else {
    BX_PANIC(("floppy_param_string_handler called with unknown parameter '%s'", pname));
  }
  return val;
}

#if BX_DEBUGGER
void bx_floppy_ctrl_c::debug_dump(int argc, char **argv)
{
  dbg_printf("i82077AA FDC\n\n");
  for (int i = 0; i < 2; i++) {
    dbg_printf("fd%d: ", i);
    if (BX_FD_THIS s.device_type[i] == FDRIVE_NONE) {
      dbg_printf("not installed\n");
    } else if (BX_FD_THIS s.media[i].type == BX_FLOPPY_NONE) {
      dbg_printf("media not present\n");
    } else {
#define MED (BX_FD_THIS s.media[i])
      dbg_printf("tracks=%d, heads=%d, spt=%d, readonly=%d\n",
                 MED.tracks, MED.heads, MED.sectors_per_track, MED.write_protected);
#undef MED
    }
  }
  dbg_printf("\ncontroller status: ");
  if (BX_FD_THIS s.pending_command == FD_CMD_NOP) {
    if (BX_FD_THIS s.command_complete) {
      dbg_printf("idle phase\n");
    } else {
      dbg_printf("command phase (command=0x%02x)\n", BX_FD_THIS s.command[0]);
    }
  } else {
    if (BX_FD_THIS s.result_size == 0) {
      dbg_printf("execution phase (command=0x%02x)\n", BX_FD_THIS s.pending_command);
    } else {
      dbg_printf("result phase (command=0x%02x)\n", BX_FD_THIS s.pending_command);
    }
  }
  dbg_printf("DOR = 0x%02x\n", BX_FD_THIS s.DOR);
  dbg_printf("MSR = 0x%02x\n", BX_FD_THIS s.main_status_reg);
  dbg_printf("DSR = 0x%02x\n", BX_FD_THIS s.DSR);
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif
