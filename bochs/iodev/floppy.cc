/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
//   ftp://void-core.2y.net/pub/docs/fdc/82077AA_FloppyControllerDatasheet.pdf
// Intel 82078 Data sheet
//   ftp://download.intel.com/design/periphrl/datashts/29047403.PDF
// Other FDC references
//   http://debs.future.easyspace.com/Programming/Hardware/FDC/floppy.html
// And a port list:
//   http://mudlist.eorbit.net/~adam/pickey/ports.html
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE


extern "C" {
#include <errno.h>
}

#ifdef __linux__
extern "C" {
#include <sys/ioctl.h>
#include <linux/fd.h>
}
#endif
#include "iodev.h"
#include "hdimage/hdimage.h"
#include "floppy.h"
// windows.h included by bochs.h
#ifdef WIN32
extern "C" {
#include <winioctl.h>
}
#endif
#define LOG_THIS theFloppyController->

bx_floppy_ctrl_c *theFloppyController;

/* for main status register */
#define FD_MS_MRQ  0x80
#define FD_MS_DIO  0x40
#define FD_MS_NDMA 0x20
#define FD_MS_BUSY 0x10
#define FD_MS_ACTD 0x08
#define FD_MS_ACTC 0x04
#define FD_MS_ACTB 0x02
#define FD_MS_ACTA 0x01

#define FROM_FLOPPY 10
#define TO_FLOPPY   11

#define FLOPPY_DMA_CHAN 2

#define FDRIVE_NONE  0x00
#define FDRIVE_525DD 0x01
#define FDRIVE_525HD 0x02
#define FDRIVE_350DD 0x04
#define FDRIVE_350HD 0x08
#define FDRIVE_350ED 0x10

typedef struct {
  unsigned id;
  Bit8u trk;
  Bit8u hd;
  Bit8u spt;
  unsigned sectors;
  Bit8u drive_mask;
} floppy_type_t;

static floppy_type_t floppy_type[8] = {
  {BX_FLOPPY_160K, 40, 1, 8, 320, 0x03},
  {BX_FLOPPY_180K, 40, 1, 9, 360, 0x03},
  {BX_FLOPPY_320K, 40, 2, 8, 640, 0x03},
  {BX_FLOPPY_360K, 40, 2, 9, 720, 0x03},
  {BX_FLOPPY_720K, 80, 2, 9, 1440, 0x1f},
  {BX_FLOPPY_1_2,  80, 2, 15, 2400, 0x02},
  {BX_FLOPPY_1_44, 80, 2, 18, 2880, 0x18},
  {BX_FLOPPY_2_88, 80, 2, 36, 5760, 0x10}
};

static Bit16u drate_in_k[4] = {
  500, 300, 250, 1000
};


int CDECL libfloppy_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theFloppyController = new bx_floppy_ctrl_c();
    bx_devices.pluginFloppyDevice = theFloppyController;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theFloppyController, BX_PLUGIN_FLOPPY);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libfloppy_LTX_plugin_fini(void)
{
  delete theFloppyController;
}

bx_floppy_ctrl_c::bx_floppy_ctrl_c()
{
  put("FLOPPY");
  memset(&s, 0, sizeof(s));
  s.floppy_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_floppy_ctrl_c::~bx_floppy_ctrl_c()
{
  char pname[10];

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
  Bit8u i, devtype, cmos_value;
  char pname[10];
  bx_list_c *floppy;

  BX_DEBUG(("Init $Id$"));
  DEV_dma_register_8bit_channel(2, dma_read, dma_write, "Floppy Drive");
  DEV_register_irq(6, "Floppy Drive");
  for (unsigned addr=0x03F2; addr<=0x03F7; addr++) {
    DEV_register_ioread_handler(this, read_handler, addr, "Floppy Drive", 1);
    DEV_register_iowrite_handler(this, write_handler, addr, "Floppy Drive", 1);
  }


  cmos_value = 0x00; /* start out with: no drive 0, no drive 1 */

  BX_FD_THIS s.num_supported_floppies = 0;

  for (i=0; i<4; i++) {
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

  floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYA);
  devtype = SIM->get_param_enum("devtype", floppy)->get();
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

  /* CMOS Floppy Type and Equipment Byte register */
  DEV_cmos_set_reg(0x10, cmos_value);
  if (BX_FD_THIS s.num_supported_floppies > 0) {
    DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0x3e) |
                          ((BX_FD_THIS s.num_supported_floppies-1) << 6) | 1);
  } else {
    DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0x3e));
  }

  if (BX_FD_THIS s.floppy_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_FD_THIS s.floppy_timer_index =
      bx_pc_system.register_timer(this, timer_handler, 250, 0, 0, "floppy");
  }
  /* phase out s.non_dma in favor of using FD_MS_NDMA, more like hardware */
  BX_FD_THIS s.main_status_reg &= ~FD_MS_NDMA;  // enable DMA from start
  /* these registers are not cleared by reset */
  BX_FD_THIS s.SRT = 0;
  BX_FD_THIS s.HUT = 0;
  BX_FD_THIS s.HLT = 0;

  // runtime parameters
  for (i = 0; i < 2; i++) {
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
  SIM->register_runtime_config_handler(this, runtime_config_handler);
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("floppy", this);
#endif
}

void bx_floppy_ctrl_c::reset(unsigned type)
{
  Bit32u i;

  BX_FD_THIS s.pending_irq = 0;
  BX_FD_THIS s.reset_sensei = 0; /* no reset result present */

  BX_FD_THIS s.main_status_reg = 0;
  BX_FD_THIS s.status_reg0 = 0;
  BX_FD_THIS s.status_reg1 = 0;
  BX_FD_THIS s.status_reg2 = 0;
  BX_FD_THIS s.status_reg3 = 0;

  // software reset (via DOR port 0x3f2 bit 2) does not change DOR
  if (type == BX_RESET_HARDWARE) {
    BX_FD_THIS s.DOR = 0x0c;
    // motor off, drive 3..0
    // DMA/INT enabled
    // normal operation
    // drive select 0

    // DIR and CCR affected only by hard reset
    for (i=0; i<4; i++) {
      BX_FD_THIS s.DIR[i] |= 0x80; // disk changed
      }
    BX_FD_THIS s.data_rate = 2; /* 250 Kbps */
    BX_FD_THIS s.lock = 0;
  } else {
    BX_INFO(("controller reset in software"));
  }
  if (BX_FD_THIS s.lock == 0) {
    BX_FD_THIS s.config = 0;
    BX_FD_THIS s.pretrk = 0;
  }
  BX_FD_THIS s.perp_mode = 0;

  for (i=0; i<4; i++) {
    BX_FD_THIS s.cylinder[i] = 0;
    BX_FD_THIS s.head[i] = 0;
    BX_FD_THIS s.sector[i] = 0;
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
  unsigned i;
  char name[8];
  bx_list_c *drive;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "floppy", "Floppy State");
  new bx_shadow_num_c(list, "data_rate", &BX_FD_THIS s.data_rate);
  bx_list_c *command = new bx_list_c(list, "command");
  for (i=0; i<10; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(command, name, &BX_FD_THIS s.command[i], BASE_HEX);
  }
  new bx_shadow_num_c(list, "command_index", &BX_FD_THIS s.command_index);
  new bx_shadow_num_c(list, "command_size", &BX_FD_THIS s.command_size);
  new bx_shadow_bool_c(list, "command_complete", &BX_FD_THIS s.command_complete);
  new bx_shadow_num_c(list, "pending_command", &BX_FD_THIS s.pending_command, BASE_HEX);
  new bx_shadow_bool_c(list, "multi_track", &BX_FD_THIS s.multi_track);
  new bx_shadow_bool_c(list, "pending_irq", &BX_FD_THIS s.pending_irq);
  new bx_shadow_num_c(list, "reset_sensei", &BX_FD_THIS s.reset_sensei);
  new bx_shadow_num_c(list, "format_count", &BX_FD_THIS s.format_count);
  new bx_shadow_num_c(list, "format_fillbyte", &BX_FD_THIS s.format_fillbyte, BASE_HEX);
  bx_list_c *result = new bx_list_c(list, "result");
  for (i=0; i<10; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(result, name, &BX_FD_THIS s.result[i], BASE_HEX);
  }
  new bx_shadow_num_c(list, "result_index", &BX_FD_THIS s.result_index);
  new bx_shadow_num_c(list, "result_size", &BX_FD_THIS s.result_size);
  new bx_shadow_num_c(list, "DOR", &BX_FD_THIS s.DOR, BASE_HEX);
  new bx_shadow_num_c(list, "TDR", &BX_FD_THIS s.TDR, BASE_HEX);
  new bx_shadow_bool_c(list, "TC", &BX_FD_THIS s.TC);
  new bx_shadow_num_c(list, "main_status_reg", &BX_FD_THIS s.main_status_reg, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg0", &BX_FD_THIS s.status_reg0, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg1", &BX_FD_THIS s.status_reg1, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg2", &BX_FD_THIS s.status_reg2, BASE_HEX);
  new bx_shadow_num_c(list, "status_reg3", &BX_FD_THIS s.status_reg3, BASE_HEX);
  new bx_shadow_num_c(list, "floppy_buffer_index", &BX_FD_THIS s.floppy_buffer_index);
  new bx_shadow_bool_c(list, "lock", &BX_FD_THIS s.lock);
  new bx_shadow_num_c(list, "SRT", &BX_FD_THIS s.SRT, BASE_HEX);
  new bx_shadow_num_c(list, "HUT", &BX_FD_THIS s.HUT, BASE_HEX);
  new bx_shadow_num_c(list, "HLT", &BX_FD_THIS s.HLT, BASE_HEX);
  new bx_shadow_num_c(list, "config", &BX_FD_THIS s.config, BASE_HEX);
  new bx_shadow_num_c(list, "pretrk", &BX_FD_THIS s.pretrk);
  new bx_shadow_num_c(list, "perp_mode", &BX_FD_THIS s.perp_mode);
  new bx_shadow_data_c(list, "buffer", BX_FD_THIS s.floppy_buffer, 512);
  for (i=0; i<4; i++) {
    sprintf(name, "drive%d", i);
    drive = new bx_list_c(list, name);
    new bx_shadow_num_c(drive, "cylinder", &BX_FD_THIS s.cylinder[i]);
    new bx_shadow_num_c(drive, "head", &BX_FD_THIS s.head[i]);
    new bx_shadow_num_c(drive, "sector", &BX_FD_THIS s.sector[i]);
    new bx_shadow_num_c(drive, "eot", &BX_FD_THIS s.eot[i]);
    new bx_shadow_bool_c(drive, "media_present", &BX_FD_THIS s.media_present[i]);
    new bx_shadow_num_c(drive, "DIR", &BX_FD_THIS s.DIR[i], BASE_HEX);
  }
}

void bx_floppy_ctrl_c::after_restore_state(void)
{
  if (BX_FD_THIS s.statusbar_id[0] >= 0) {
    if ((BX_FD_THIS s.DOR & 0x10) > 0)
      bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[0], 1);
  }
  if (BX_FD_THIS s.statusbar_id[1] >= 0) {
    if ((BX_FD_THIS s.DOR & 0x20) > 0)
      bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[1], 1);
  }
}

void bx_floppy_ctrl_c::runtime_config_handler(void *this_ptr)
{
  bx_floppy_ctrl_c *class_ptr = (bx_floppy_ctrl_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_floppy_ctrl_c::runtime_config(void)
{
  unsigned drive;
  bx_bool status;
  char pname[16];

  for (drive=0; drive<2; drive++) {
    if (BX_FD_THIS s.media[drive].status_changed) {
      sprintf(pname, "floppy.%d.status", drive);
      status = (SIM->get_param_enum(pname)->get() == BX_INSERTED);
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

  Bit8u pending_command = BX_FD_THIS s.pending_command;
  switch (address) {
#if BX_DMA_FLOPPY_IO
    case 0x3F2: // diskette controller digital output register
      value = BX_FD_THIS s.DOR;
      break;

    case 0x3F4: /* diskette controller main status register */
      value = BX_FD_THIS s.main_status_reg;
      break;

    case 0x3F5: /* diskette controller data */
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) &&
          ((BX_FD_THIS s.pending_command & 0x4f) == 0x46)) {
        dma_write(&value, 1);
        lower_interrupt();
        // don't enter idle phase until we've given CPU last data byte
        if (BX_FD_THIS s.TC) enter_idle_phase();
      } else if (BX_FD_THIS s.result_size == 0) {
        BX_ERROR(("port 0x3f5: no results to read"));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        value = BX_FD_THIS s.result[0];
      } else {
        value = BX_FD_THIS s.result[BX_FD_THIS s.result_index++];
        BX_FD_THIS s.main_status_reg &= 0xF0;
        BX_FD_THIS lower_interrupt();
        if (BX_FD_THIS s.result_index >= BX_FD_THIS s.result_size) {
          enter_idle_phase();
        }
      }
      break;
#endif  // #if BX_DMA_FLOPPY_IO

    case 0x3F3: // Tape Drive Register
      drive = BX_FD_THIS s.DOR & 0x03;
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
      drive = BX_FD_THIS s.DOR & 0x03;
      if (BX_FD_THIS s.DOR & (1<<(drive+4))) {
        value |= (BX_FD_THIS s.DIR[drive] & 0x80);
      }
      break;

    default:
      BX_ERROR(("io_read: unsupported address 0x%04x", (unsigned) address));
      return(0);
      break;
  }
  BX_DEBUG(("read(): during command 0x%02x, port 0x%04x returns 0x%02x",
            pending_command, address, value));
  return (value);
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
  Bit8u dma_and_interrupt_enable;
  Bit8u normal_operation, prev_normal_operation;
  Bit8u drive_select;
  Bit8u motor_on_drive0, motor_on_drive1;

  BX_DEBUG(("write access to port 0x%04x, value=0x%02x", address, value));

  switch (address) {
#if BX_DMA_FLOPPY_IO
    case 0x3F2: /* diskette controller digital output register */
      motor_on_drive0 = value & 0x10;
      motor_on_drive1 = value & 0x20;
      /* set status bar conditions for Floppy 0 and Floppy 1 */
      if (BX_FD_THIS s.statusbar_id[0] >= 0) {
        if (motor_on_drive0 != (BX_FD_THIS s.DOR & 0x10))
          bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[0], motor_on_drive0);
      }
      if (BX_FD_THIS s.statusbar_id[1] >= 0) {
        if (motor_on_drive1 != (BX_FD_THIS s.DOR & 0x20))
          bx_gui->statusbar_setitem(BX_FD_THIS s.statusbar_id[1], motor_on_drive1);
      }
      dma_and_interrupt_enable = value & 0x08;
      if (!dma_and_interrupt_enable)
        BX_DEBUG(("DMA and interrupt capabilities disabled"));
      normal_operation = value & 0x04;
      drive_select = value & 0x03;

      prev_normal_operation = BX_FD_THIS s.DOR & 0x04;
      BX_FD_THIS s.DOR = value;

      if (prev_normal_operation==0 && normal_operation) {
        // transition from RESET to NORMAL
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, 250, 0);
      } else if (prev_normal_operation && normal_operation==0) {
        // transition from NORMAL to RESET
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.pending_command = 0xfe; // RESET pending
      }
      BX_DEBUG(("io_write: digital output register"));
      BX_DEBUG(("  motor on, drive1 = %d", motor_on_drive1 > 0));
      BX_DEBUG(("  motor on, drive0 = %d", motor_on_drive0 > 0));
      BX_DEBUG(("  dma_and_interrupt_enable=%02x",
        (unsigned) dma_and_interrupt_enable));
      BX_DEBUG(("  normal_operation=%02x",
        (unsigned) normal_operation));
      BX_DEBUG(("  drive_select=%02x",
        (unsigned) drive_select));
      if (BX_FD_THIS s.device_type[drive_select] == FDRIVE_NONE) {
        BX_DEBUG(("WARNING: non existing drive selected"));
      }
      break;

    case 0x3f4: /* diskette controller data rate select register */
      BX_FD_THIS s.data_rate = value & 0x03;
      if (value & 0x80) {
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.pending_command = 0xfe; // RESET pending
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, 250, 0);
      }
      if ((value & 0x7c) > 0) {
        BX_ERROR(("write to data rate select register: unsupported bits set"));
      }
      break;

    case 0x3F5: /* diskette controller data */
      BX_DEBUG(("command = 0x%02x", (unsigned) value));
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) && ((BX_FD_THIS s.pending_command & 0x4f) == 0x45)) {
        BX_FD_THIS dma_read((Bit8u *) &value, 1);
        BX_FD_THIS lower_interrupt();
        break;
      } else if (BX_FD_THIS s.command_complete) {
        if (BX_FD_THIS s.pending_command != 0)
          BX_PANIC(("write 0x03f5: receiving new command 0x%02x, old one (0x%02x) pending",
            value, BX_FD_THIS s.pending_command));
        BX_FD_THIS s.command[0] = value;
        BX_FD_THIS s.command_complete = 0;
        BX_FD_THIS s.command_index = 1;
        /* read/write command in progress */
        BX_FD_THIS s.main_status_reg &= ~FD_MS_DIO; // leave drive status untouched
        BX_FD_THIS s.main_status_reg |= FD_MS_MRQ | FD_MS_BUSY;
        switch (value) {
          case 0x03: /* specify */
            BX_FD_THIS s.command_size = 3;
            break;
          case 0x04: // get status
            BX_FD_THIS s.command_size = 2;
            break;
          case 0x07: /* recalibrate */
            BX_FD_THIS s.command_size = 2;
            break;
          case 0x08: /* sense interrupt status */
            BX_FD_THIS s.command_size = 1;
            break;
          case 0x0f: /* seek */
            BX_FD_THIS s.command_size = 3;
            break;
          case 0x4a: /* read ID */
            BX_FD_THIS s.command_size = 2;
            break;
          case 0x4d: /* format track */
            BX_FD_THIS s.command_size = 6;
            break;
          case 0x45:
          case 0xc5: /* write normal data */
            BX_FD_THIS s.command_size = 9;
            break;
          case 0x46:
          case 0x66:
          case 0xc6:
          case 0xe6: /* read normal data */
            BX_FD_THIS s.command_size = 9;
            break;

          case 0x0e: // dump registers (Enhanced drives)
          case 0x10: // Version command, enhanced controller returns 0x90
          case 0x14: // Unlock command (Enhanced)
          case 0x94: // Lock command (Enhanced)
            BX_FD_THIS s.command_size = 0;
            BX_FD_THIS s.pending_command = value;
            enter_result_phase();
            break;
          case 0x12: // Perpendicular mode (Enhanced)
            BX_FD_THIS s.command_size = 2;
            break;
          case 0x13: // Configure command (Enhanced)
            BX_FD_THIS s.command_size = 4;
            break;

          case 0x18: // National Semiconductor version command; return 80h
            // These commands are not implemented on the standard
            // controller and return an error.  They are available on
            // the enhanced controller.
            BX_DEBUG(("io_write: 0x3f5: unsupported floppy command 0x%02x",
              (unsigned) value));
            BX_FD_THIS s.command_size = 0;   // make sure we don't try to process this command
            BX_FD_THIS s.status_reg0 = 0x80; // status: invalid command
            enter_result_phase();
            break;

          default:
            BX_ERROR(("io_write: 0x3f5: invalid floppy command 0x%02x",
              (unsigned) value));
            BX_FD_THIS s.command_size = 0;   // make sure we don't try to process this command
            BX_FD_THIS s.status_reg0 = 0x80; // status: invalid command
            enter_result_phase();
            break;
        }
      } else {
        BX_FD_THIS s.command[BX_FD_THIS s.command_index++] =
          value;
      }
      if (BX_FD_THIS s.command_index ==
        BX_FD_THIS s.command_size) {
        /* read/write command not in progress any more */
        floppy_command();
        BX_FD_THIS s.command_complete = 1;
      }
      BX_DEBUG(("io_write: diskette controller data"));
      return;
      break;
#endif  // #if BX_DMA_FLOPPY_IO

    case 0x3F6: /* diskette controller (reserved) */
      BX_DEBUG(("io_write: reserved register 0x3f6 unsupported"));
      // this address shared with the hard drive controller
      DEV_hd_write_handler(bx_devices.pluginHardDrive, address, value, io_len);
      break;

#if BX_DMA_FLOPPY_IO
    case 0x3F7: /* diskette controller configuration control register */
      if ((value & 0x03) != BX_FD_THIS s.data_rate)
        BX_INFO(("io_write: config control register: 0x%02x", value));
      BX_FD_THIS s.data_rate = value & 0x03;
      switch (BX_FD_THIS s.data_rate) {
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
  Bit8u head, drive, cylinder, sector, eot;
  Bit8u sector_size;
//Bit8u data_length;
  Bit32u logical_sector, sector_time, step_delay;

  // Print command
  char buf[9+(9*5)+1], *p = buf;
  p += sprintf(p, "COMMAND: ");
  for (i=0; i<BX_FD_THIS s.command_size; i++) {
    p += sprintf(p, "[%02x] ", (unsigned) BX_FD_THIS s.command[i]);
  }
  BX_DEBUG(("%s", buf));

  BX_FD_THIS s.pending_command = BX_FD_THIS s.command[0];
  switch (BX_FD_THIS s.pending_command) {
    case 0x03: // specify
      // execution: specified parameters are loaded
      // result: no result bytes, no interrupt
      BX_FD_THIS s.SRT = BX_FD_THIS s.command[1] >> 4;
      BX_FD_THIS s.HUT = BX_FD_THIS s.command[1] & 0x0f;
      BX_FD_THIS s.HLT = BX_FD_THIS s.command[2] >> 1;
      BX_FD_THIS s.main_status_reg |= (BX_FD_THIS s.command[2] & 0x01) ? FD_MS_NDMA : 0;
      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA)
        BX_ERROR(("non DMA mode not fully implemented yet"));
      enter_idle_phase();
      return;

    case 0x04: // get status
      drive = (BX_FD_THIS s.command[1] & 0x03);
      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      BX_FD_THIS s.status_reg3 = 0x28 | (BX_FD_THIS s.head[drive]<<2) | drive
        | (BX_FD_THIS s.media[drive].write_protected ? 0x40 : 0x00);
      if ((BX_FD_THIS s.device_type[drive] != FDRIVE_NONE) &&
          (BX_FD_THIS s.cylinder[drive] == 0))
        BX_FD_THIS s.status_reg3 |= 0x10;
      enter_result_phase();
      return;

    case 0x07: // recalibrate
      drive = (BX_FD_THIS s.command[1] & 0x03);
      BX_FD_THIS s.DOR &= 0xfc;
      BX_FD_THIS s.DOR |= drive;
      BX_DEBUG(("floppy_command(): recalibrate drive %u",
        (unsigned) drive));
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

    case 0x08: /* sense interrupt status */
      /* execution:
       *   get status
       * result:
       *   no interupt
       *   byte0 = status reg0
       *   byte1 = current cylinder number (0 to 79)
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

    case 0x0f: /* seek */
      /* command:
       *   byte0 = 0F
       *   byte1 = drive & head select
       *   byte2 = cylinder number
       * execution:
       *   postion head over specified cylinder
       * result:
       *   no result bytes, issues an interrupt
       */
      drive = BX_FD_THIS s.command[1] & 0x03;
      BX_FD_THIS s.DOR &= 0xfc;
      BX_FD_THIS s.DOR |= drive;

      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      step_delay = calculate_step_delay(drive, BX_FD_THIS s.command[2]);
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, step_delay, 0);
      /* ??? should also check cylinder validity */
      BX_FD_THIS s.cylinder[drive] = BX_FD_THIS s.command[2];
      /* data reg not ready, drive not busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= (1 << drive);
      return;

    case 0x13: // Configure
      BX_DEBUG(("configure (eis     = 0x%02x)", BX_FD_THIS s.command[2] & 0x40));
      BX_DEBUG(("configure (efifo   = 0x%02x)", BX_FD_THIS s.command[2] & 0x20));
      BX_DEBUG(("configure (no poll = 0x%02x)", BX_FD_THIS s.command[2] & 0x10));
      BX_DEBUG(("configure (fifothr = 0x%02x)", BX_FD_THIS s.command[2] & 0x0f));
      BX_DEBUG(("configure (pretrk  = 0x%02x)", BX_FD_THIS s.command[3]));
      BX_FD_THIS s.config = BX_FD_THIS s.command[2];
      BX_FD_THIS s.pretrk = BX_FD_THIS s.command[3];
      enter_idle_phase();
      return;

    case 0x4a: // read ID
      drive = BX_FD_THIS s.command[1] & 0x03;
      BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
      BX_FD_THIS s.DOR &= 0xfc;
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
        BX_INFO(("attempt to read sector ID with media not present"));
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        return; // Hang controller
      }
      BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive]<<2) | drive;
      // time to read one sector at 300 rpm
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index, sector_time, 0);
      /* data reg not ready, controller busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
      return;

    case 0x4d: // format track
        drive = BX_FD_THIS s.command[1] & 0x03;
        BX_FD_THIS s.DOR &= 0xfc;
        BX_FD_THIS s.DOR |= drive;

        motor_on = (BX_FD_THIS s.DOR>>(drive+4)) & 0x01;
        if (motor_on == 0)
          BX_PANIC(("floppy_command(): format track: motor not on"));
        BX_FD_THIS s.head[drive] = (BX_FD_THIS s.command[1] >> 2) & 0x01;
        sector_size = BX_FD_THIS s.command[2];
        BX_FD_THIS s.format_count = BX_FD_THIS s.command[3];
        BX_FD_THIS s.format_fillbyte = BX_FD_THIS s.command[5];
        if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE)
          BX_PANIC(("floppy_command(): format track: bad drive #%d", drive));

        if (sector_size != 0x02) { // 512 bytes
          BX_PANIC(("format track: sector size %d not supported", 128<<sector_size));
        }
        if (BX_FD_THIS s.format_count != BX_FD_THIS s.media[drive].sectors_per_track) {
          BX_PANIC(("format track: %d sectors/track requested (%d expected)",
                    BX_FD_THIS s.format_count, BX_FD_THIS s.media[drive].sectors_per_track));
        }
        if (BX_FD_THIS s.media_present[drive] == 0) {
          BX_INFO(("attempt to format track with media not present"));
          return; // Hang controller
        }
        if (BX_FD_THIS s.media[drive].write_protected) {
          // media write-protected, return error
          BX_INFO(("attempt to format track with media write-protected"));
          BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive; // abnormal termination
          BX_FD_THIS s.status_reg1 = 0x27; // 0010 0111
          BX_FD_THIS s.status_reg2 = 0x31; // 0011 0001
          enter_result_phase();
          return;
        }

      /* 4 header bytes per sector are required */
      BX_FD_THIS s.format_count <<= 2;

      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
        BX_DEBUG(("non-DMA floppy format unimplemented"));
      } else {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
      }
      /* data reg not ready, controller busy */
      BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
      BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
      BX_DEBUG(("format track"));
      return;

    case 0x46: // read normal data, MT=0, SK=0
    case 0x66: // read normal data, MT=0, SK=1
    case 0xc6: // read normal data, MT=1, SK=0
    case 0xe6: // read normal data, MT=1, SK=1
    case 0x45: // write normal data, MT=0
    case 0xc5: // write normal data, MT=1
      BX_FD_THIS s.multi_track = (BX_FD_THIS s.command[0] >> 7);
      if ((BX_FD_THIS s.DOR & 0x08) == 0)
        BX_PANIC(("read/write command with DMA and int disabled"));
      drive = BX_FD_THIS s.command[1] & 0x03;
      BX_FD_THIS s.DOR &= 0xfc;
      BX_FD_THIS s.DOR |= drive;

      motor_on = (BX_FD_THIS s.DOR>>(drive+4)) & 0x01;
      if (motor_on == 0)
        BX_PANIC(("floppy_command(): read/write: motor not on"));
      head = BX_FD_THIS s.command[3] & 0x01;
      cylinder = BX_FD_THIS s.command[2]; /* 0..79 depending */
      sector = BX_FD_THIS s.command[4];   /* 1..36 depending */
      eot = BX_FD_THIS s.command[6];      /* 1..36 depending */
      sector_size = BX_FD_THIS s.command[5];
//    data_length = BX_FD_THIS s.command[8];
      BX_DEBUG(("read/write normal data"));
      BX_DEBUG(("BEFORE"));
      BX_DEBUG(("  drive    = %u", (unsigned) drive));
      BX_DEBUG(("  head     = %u", (unsigned) head));
      BX_DEBUG(("  cylinder = %u", (unsigned) cylinder));
      BX_DEBUG(("  sector   = %u", (unsigned) sector));
      BX_DEBUG(("  eot      = %u", (unsigned) eot));
      if (BX_FD_THIS s.device_type[drive] == FDRIVE_NONE)
        BX_PANIC(("floppy_command(): read/write: bad drive #%d", drive));

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
        BX_INFO(("attempt to read/write sector %u with media not present", (unsigned) sector));
        return; // Hang controller
      }

      if (sector_size != 0x02) { // 512 bytes
        BX_PANIC(("read/write command: sector size %d not supported", 128<<sector_size));
      }

      if (cylinder >= BX_FD_THIS s.media[drive].tracks) {
        BX_PANIC(("io: norm r/w parms out of range: sec#%02xh cyl#%02xh eot#%02xh head#%02xh",
          (unsigned) sector, (unsigned) cylinder, (unsigned) eot,
          (unsigned) head));
        return;
      }

      if (sector > BX_FD_THIS s.media[drive].sectors_per_track) {
        BX_INFO(("attempt to read/write sector %u past last sector %u",
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

      logical_sector = (cylinder * BX_FD_THIS s.media[drive].heads * BX_FD_THIS s.media[drive].sectors_per_track) +
                       (head * BX_FD_THIS s.media[drive].sectors_per_track) +
                       (sector - 1);

      if (logical_sector >= BX_FD_THIS s.media[drive].sectors) {
        BX_PANIC(("io: logical sector out of bounds"));
      }
      // This hack makes older versions of the Bochs BIOS work
      if (eot == 0) {
        eot = BX_FD_THIS s.media[drive].sectors_per_track;
      }
      BX_FD_THIS s.cylinder[drive] = cylinder;
      BX_FD_THIS s.head[drive]     = head;
      BX_FD_THIS s.sector[drive]   = sector;
      BX_FD_THIS s.eot[drive]      = eot;

      if ((BX_FD_THIS s.command[0] & 0x4f) == 0x46) { // read
        floppy_xfer(drive, logical_sector*512, BX_FD_THIS s.floppy_buffer,
                    512, FROM_FLOPPY);
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
          BX_FD_THIS s.main_status_reg |= (FD_MS_MRQ | FD_MS_DIO);
        }
        // time to read one sector at 300 rpm
        sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index,
                                    sector_time , 0);
      } else if ((BX_FD_THIS s.command[0] & 0x7f) == 0x45) { // write
        /* controller busy; if DMA mode, data reg not ready */
        BX_FD_THIS s.main_status_reg &= FD_MS_NDMA;
        BX_FD_THIS s.main_status_reg |= FD_MS_BUSY;
        if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
          BX_FD_THIS s.main_status_reg |= FD_MS_MRQ;
        } else {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      } else {
        BX_PANIC(("floppy_command(): unknown read/write command"));
        return;
      }
      break;

    case 0x12: // Perpendicular mode
      BX_FD_THIS s.perp_mode = BX_FD_THIS s.command[1];
      BX_INFO(("perpendicular mode: config=0x%02x", BX_FD_THIS s.perp_mode));
      enter_idle_phase();
      break;

    default: // invalid or unsupported command; these are captured in write() above
      BX_PANIC(("You should never get here! cmd = 0x%02x",
                BX_FD_THIS s.command[0]));
  }
}

void bx_floppy_ctrl_c::floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer,
            Bit32u bytes, Bit8u direction)
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
    } else if (!strcmp(SIM->get_param_string(pname)->getptr(), SuperDrive))
      ret = fd_read((char *) buffer, offset, bytes);
#endif
    } else {
      ret = ::read(BX_FD_THIS s.media[drive].fd, (bx_ptr_t) buffer, bytes);
    }
    if (ret < int(bytes)) {
      if (ret > 0) {
        BX_INFO(("partial read() on floppy image returns %u/%u",
          (unsigned) ret, (unsigned) bytes));
        memset(buffer + ret, 0, bytes - ret);
      } else {
        BX_INFO(("read() on floppy image returns 0"));
        memset(buffer, 0, bytes);
      }
    }
  } else { // TO_FLOPPY
    BX_ASSERT (!BX_FD_THIS s.media[drive].write_protected);
    if (BX_FD_THIS s.media[drive].vvfat_floppy) {
      ret = (int)BX_FD_THIS s.media[drive].vvfat->write(buffer, bytes);
#if BX_WITH_MACOS
    } else if (!strcmp(SIM->get_param_string(pname)->getptr(), SuperDrive))
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
  Bit8u drive, motor_on;

  drive = BX_FD_THIS s.DOR & 0x03;
  switch (BX_FD_THIS s.pending_command) {
    case 0x07: // recal
      BX_FD_THIS s.status_reg0 = 0x20 | drive;
      motor_on = ((BX_FD_THIS s.DOR>>(drive+4)) & 0x01);
      if ((BX_FD_THIS s.device_type[drive] == FDRIVE_NONE) || (motor_on == 0)) {
        BX_FD_THIS s.status_reg0 |= 0x50;
      }
      enter_idle_phase();
      BX_FD_THIS raise_interrupt();
      break;

    case 0x0f: // seek
      BX_FD_THIS s.status_reg0 = 0x20 | (BX_FD_THIS s.head[drive]<<2) | drive;
      enter_idle_phase();
      BX_FD_THIS raise_interrupt();
      break;

    case 0x4a: /* read ID */
      enter_result_phase();
      break;

    case 0x45: /* write normal data */
    case 0xc5:
      if (BX_FD_THIS s.TC) { // Terminal Count line, done
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        BX_FD_THIS s.status_reg1 = 0;
        BX_FD_THIS s.status_reg2 = 0;

        BX_DEBUG(("<<WRITE DONE>>"));
        BX_DEBUG(("AFTER"));
        BX_DEBUG(("  drive    = %u", drive));
        BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
        BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
        BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));

        enter_result_phase();
      } else {
        // transfer next sector
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      }
      break;

    case 0x46: /* read normal data */
    case 0x66:
    case 0xc6:
    case 0xe6:
      // transfer next sector
      if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
        BX_FD_THIS s.main_status_reg &= ~FD_MS_BUSY;  // clear busy bit
        BX_FD_THIS s.main_status_reg |= FD_MS_MRQ | FD_MS_DIO;  // data byte waiting
      } else {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
      }
      break;

    case 0x4d: /* format track */
      if ((BX_FD_THIS s.format_count == 0) || BX_FD_THIS s.TC) {
        BX_FD_THIS s.format_count = 0;
        BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
        enter_result_phase();
      } else {
        // transfer next sector
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 1);
        }
      }
      break;

    case 0xfe: // (contrived) RESET
      theFloppyController->reset(BX_RESET_SOFTWARE);
      BX_FD_THIS s.pending_command = 0;
      BX_FD_THIS s.status_reg0 = 0xc0;
      BX_FD_THIS raise_interrupt();
      BX_FD_THIS s.reset_sensei = 4;
      break;

    case 0x00: // nothing pending?
      break;

    default:
      BX_PANIC(("floppy:timer(): unknown case %02x",
        (unsigned) BX_FD_THIS s.pending_command));
  }
}

Bit16u bx_floppy_ctrl_c::dma_write(Bit8u *buffer, Bit16u maxlen)
{
  // A DMA write is from I/O to Memory
  // We need to return the next data byte(s) from the floppy buffer
  // to be transfered via the DMA to memory. (read block from floppy)
  //
  // maxlen is the maximum length of the DMA transfer

  Bit8u drive = BX_FD_THIS s.DOR & 0x03;
  Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
  if (len > maxlen) len = maxlen;
  memcpy(buffer, &BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], len);
  BX_FD_THIS s.floppy_buffer_index += len;
  BX_FD_THIS s.TC = get_tc() && (len == maxlen);

  if ((BX_FD_THIS s.floppy_buffer_index >= 512) || (BX_FD_THIS s.TC)) {

    if (BX_FD_THIS s.floppy_buffer_index >= 512) {
      increment_sector(); // increment to next sector before retrieving next one
      BX_FD_THIS s.floppy_buffer_index = 0;
    }
    if (BX_FD_THIS s.TC) { // Terminal Count line, done
      BX_FD_THIS s.status_reg0 = (BX_FD_THIS s.head[drive] << 2) | drive;
      BX_FD_THIS s.status_reg1 = 0;
      BX_FD_THIS s.status_reg2 = 0;

      BX_DEBUG(("<<READ DONE>>"));
      BX_DEBUG(("AFTER"));
      BX_DEBUG(("  drive    = %u", drive));
      BX_DEBUG(("  head     = %u", BX_FD_THIS s.head[drive]));
      BX_DEBUG(("  cylinder = %u", BX_FD_THIS s.cylinder[drive]));
      BX_DEBUG(("  sector   = %u", BX_FD_THIS s.sector[drive]));

      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      enter_result_phase();
    } else { // more data to transfer
      Bit32u logical_sector, sector_time;

      // remember that not all floppies have two sides, multiply by s.head[drive]
      logical_sector = (BX_FD_THIS s.cylinder[drive] * BX_FD_THIS s.media[drive].heads *
                        BX_FD_THIS s.media[drive].sectors_per_track) +
                       (BX_FD_THIS s.head[drive] *
                        BX_FD_THIS s.media[drive].sectors_per_track) +
                       (BX_FD_THIS s.sector[drive] - 1);

      floppy_xfer(drive, logical_sector*512, BX_FD_THIS s.floppy_buffer,
                  512, FROM_FLOPPY);
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // time to read one sector at 300 rpm
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index,
                                  sector_time , 0);
    }
  }
  return len;
}

Bit16u bx_floppy_ctrl_c::dma_read(Bit8u *buffer, Bit16u maxlen)
{
  // A DMA read is from Memory to I/O
  // We need to write the data_byte which was already transfered from memory
  // via DMA to I/O (write block to floppy)
  //
  // maxlen is the length of the DMA transfer (not implemented yet)

  Bit8u drive = BX_FD_THIS s.DOR & 0x03;
  Bit32u logical_sector, sector_time;

  if (BX_FD_THIS s.pending_command == 0x4d) { // format track in progress
    BX_FD_THIS s.format_count--;
    switch (3 - (BX_FD_THIS s.format_count & 0x03)) {
      case 0:
        BX_FD_THIS s.cylinder[drive] = *buffer;
        break;
      case 1:
        if (*buffer != BX_FD_THIS s.head[drive])
          BX_ERROR(("head number does not match head field"));
        break;
      case 2:
        BX_FD_THIS s.sector[drive] = *buffer;
        break;
      case 3:
        if (*buffer != 2) BX_ERROR(("dma_read: sector size %d not supported", 128<<(*buffer)));
        BX_DEBUG(("formatting cylinder %u head %u sector %u",
                  BX_FD_THIS s.cylinder[drive], BX_FD_THIS s.head[drive],
                  BX_FD_THIS s.sector[drive]));
        for (unsigned i = 0; i < 512; i++) {
          BX_FD_THIS s.floppy_buffer[i] = BX_FD_THIS s.format_fillbyte;
        }
        logical_sector = (BX_FD_THIS s.cylinder[drive] * BX_FD_THIS s.media[drive].heads * BX_FD_THIS s.media[drive].sectors_per_track) +
                         (BX_FD_THIS s.head[drive] * BX_FD_THIS s.media[drive].sectors_per_track) +
                         (BX_FD_THIS s.sector[drive] - 1);
        floppy_xfer(drive, logical_sector*512, BX_FD_THIS s.floppy_buffer,
                    512, TO_FLOPPY);
        if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
          DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
        }
        // time to write one sector at 300 rpm
        sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
        bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index,
                                    sector_time , 0);
        break;
    }
    return 1;
  } else { // write normal data
    Bit16u len = 512 - BX_FD_THIS s.floppy_buffer_index;
    if (len > maxlen) len = maxlen;
    memcpy(&BX_FD_THIS s.floppy_buffer[BX_FD_THIS s.floppy_buffer_index], buffer, len);
    BX_FD_THIS s.floppy_buffer_index += len;
    BX_FD_THIS s.TC = get_tc() && (len == maxlen);

    if ((BX_FD_THIS s.floppy_buffer_index >= 512) || (BX_FD_THIS s.TC)) {
      logical_sector = (BX_FD_THIS s.cylinder[drive] * BX_FD_THIS s.media[drive].heads * BX_FD_THIS s.media[drive].sectors_per_track) +
                       (BX_FD_THIS s.head[drive] * BX_FD_THIS s.media[drive].sectors_per_track) +
                       (BX_FD_THIS s.sector[drive] - 1);
      if (BX_FD_THIS s.media[drive].write_protected) {
        // write protected error
        BX_INFO(("tried to write disk %u, which is write-protected", drive));
        // ST0: IC1,0=01  (abnormal termination: started execution but failed)
        BX_FD_THIS s.status_reg0 = 0x40 | (BX_FD_THIS s.head[drive]<<2) | drive;
        // ST1: DataError=1, NDAT=1, NotWritable=1, NID=1
        BX_FD_THIS s.status_reg1 = 0x27; // 0010 0111
        // ST2: CRCE=1, SERR=1, BCYL=1, NDAM=1.
        BX_FD_THIS s.status_reg2 = 0x31; // 0011 0001
        enter_result_phase();
        return 1;
      }
      floppy_xfer(drive, logical_sector*512, BX_FD_THIS s.floppy_buffer,
                  512, TO_FLOPPY);
      increment_sector(); // increment to next sector after writing current one
      BX_FD_THIS s.floppy_buffer_index = 0;
      if (!(BX_FD_THIS s.main_status_reg & FD_MS_NDMA)) {
        DEV_dma_set_drq(FLOPPY_DMA_CHAN, 0);
      }
      // time to write one sector at 300 rpm
      sector_time = 200000 / BX_FD_THIS s.media[drive].sectors_per_track;
      bx_pc_system.activate_timer(BX_FD_THIS s.floppy_timer_index,
                                  sector_time , 0);
      // the following is a kludge; i (jc) don't know how to work with the timer
      if ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) && BX_FD_THIS s.TC) {
        enter_result_phase();
      }
    }
    return len;
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

void bx_floppy_ctrl_c::increment_sector(void)
{
  Bit8u drive;

  drive = BX_FD_THIS s.DOR & 0x03;

  // values after completion of data xfer
  // ??? calculation depends on base_count being multiple of 512
  BX_FD_THIS s.sector[drive] ++;
  if ((BX_FD_THIS s.sector[drive] > BX_FD_THIS s.eot[drive]) ||
      (BX_FD_THIS s.sector[drive] > BX_FD_THIS s.media[drive].sectors_per_track)) {
    BX_FD_THIS s.sector[drive] = 1;
    if (BX_FD_THIS s.multi_track) {
      BX_FD_THIS s.head[drive] ++;
      if (BX_FD_THIS s.head[drive] > 1) {
        BX_FD_THIS s.head[drive] = 0;
        BX_FD_THIS s.cylinder[drive] ++;
        reset_changeline();
      }
    } else {
      BX_FD_THIS s.cylinder[drive] ++;
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

unsigned bx_floppy_ctrl_c::set_media_status(unsigned drive, bx_bool status)
{
  char *path;
  unsigned type;
  bx_list_c *floppy;

  if (drive == 0)
    floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYA);
  else
    floppy = (bx_list_c*)SIM->get_param(BXPN_FLOPPYB);

  type = SIM->get_param_enum("type", floppy)->get();
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
    return 0;
  } else {
    // insert floppy
    path = SIM->get_param_string("path", floppy)->getptr();
    if (!strcmp(path, "none"))
      return 0;
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
      return 1;
    } else {
      BX_FD_THIS s.media_present[drive] = 0;
      SIM->get_param_enum("status", floppy)->set(BX_EJECTED);
      SIM->get_param_enum("type", floppy)->set(BX_FLOPPY_NONE);
      return 0;
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

bx_bool bx_floppy_ctrl_c::evaluate_media(Bit8u devtype, Bit8u type, char *path, floppy_t *media)
{
  struct stat stat_buf;
  int i, ret;
  int type_idx = -1;
#ifdef __linux__
  struct floppy_struct floppy_geom;
#endif
#ifdef WIN32
  char sTemp[1024];
  bx_bool raw_floppy = 0;
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
    media->vvfat = DEV_hdimage_init_image(BX_HDIMAGE_MODE_VVFAT, 1474560, "");
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
    ret = fstat(media->fd, &stat_buf);
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
      case BX_FLOPPY_1_2: // 1.2M 5.25"
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

void bx_floppy_ctrl_c::enter_result_phase(void)
{
  Bit8u drive;
  unsigned i;

  drive = BX_FD_THIS s.DOR & 0x03;

  /* these are always the same */
  BX_FD_THIS s.result_index = 0;
  // not necessary to clear any status bits, we're about to set them all
  BX_FD_THIS s.main_status_reg |= FD_MS_MRQ | FD_MS_DIO | FD_MS_BUSY;

  /* invalid command */
  if ((BX_FD_THIS s.status_reg0 & 0xc0) == 0x80) {
    BX_FD_THIS s.result_size = 1;
    BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
    return;
  }

  switch (BX_FD_THIS s.pending_command) {
    case 0x04: // get status
      BX_FD_THIS s.result_size = 1;
      BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg3;
      break;
    case 0x08: // sense interrupt
      BX_FD_THIS s.result_size = 2;
      BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
      BX_FD_THIS s.result[1] = BX_FD_THIS s.cylinder[drive];
      break;
    case 0x0e: // dump registers
      BX_FD_THIS s.result_size = 10;
      for (i = 0; i < 4; i++) {
        BX_FD_THIS s.result[i] = BX_FD_THIS s.cylinder[i];
      }
      BX_FD_THIS s.result[4] = (BX_FD_THIS s.SRT << 4) | BX_FD_THIS s.HUT;
      BX_FD_THIS s.result[5] = (BX_FD_THIS s.HLT << 1) | ((BX_FD_THIS s.main_status_reg & FD_MS_NDMA) ? 1 : 0);
      BX_FD_THIS s.result[6] = BX_FD_THIS s.eot[drive];
      BX_FD_THIS s.result[7] = (BX_FD_THIS s.lock << 7) | (BX_FD_THIS s.perp_mode & 0x7f);
      BX_FD_THIS s.result[8] = BX_FD_THIS s.config;
      BX_FD_THIS s.result[9] = BX_FD_THIS s.pretrk;
      break;
    case 0x10: // version
      BX_FD_THIS s.result_size = 1;
      BX_FD_THIS s.result[0] = 0x90;
      break;
    case 0x14: // unlock
    case 0x94: // lock
      BX_FD_THIS s.lock = (BX_FD_THIS s.pending_command >> 7);
      BX_FD_THIS s.result_size = 1;
      BX_FD_THIS s.result[0] = (BX_FD_THIS s.lock << 4);
      break;
    case 0x4a: // read ID
    case 0x4d: // format track
    case 0x46: // read normal data
    case 0x66:
    case 0xc6:
    case 0xe6:
    case 0x45: // write normal data
    case 0xc5:
      BX_FD_THIS s.result_size = 7;
      BX_FD_THIS s.result[0] = BX_FD_THIS s.status_reg0;
      BX_FD_THIS s.result[1] = BX_FD_THIS s.status_reg1;
      BX_FD_THIS s.result[2] = BX_FD_THIS s.status_reg2;
      BX_FD_THIS s.result[3] = BX_FD_THIS s.cylinder[drive];
      BX_FD_THIS s.result[4] = BX_FD_THIS s.head[drive];
      BX_FD_THIS s.result[5] = BX_FD_THIS s.sector[drive];
      BX_FD_THIS s.result[6] = 2; /* sector size code */
      BX_FD_THIS raise_interrupt();
      break;
  }

  // Print command result (max. 10 bytes)
  char buf[8+(10*5)+1], *p = buf;
  p += sprintf(p, "RESULT: ");
  for (i=0; i<BX_FD_THIS s.result_size; i++) {
    p += sprintf(p, "[%02x] ", (unsigned) BX_FD_THIS s.result[i]);
  }
  BX_DEBUG(("%s", buf));
}

void bx_floppy_ctrl_c::enter_idle_phase(void)
{
  BX_FD_THIS s.main_status_reg &= (FD_MS_NDMA | 0x0f);  // leave drive status untouched
  BX_FD_THIS s.main_status_reg |= FD_MS_MRQ; // data register ready

  BX_FD_THIS s.command_complete = 1; /* waiting for new command */
  BX_FD_THIS s.command_index = 0;
  BX_FD_THIS s.command_size = 0;
  BX_FD_THIS s.pending_command = 0;
  BX_FD_THIS s.result_size = 0;

  BX_FD_THIS s.floppy_buffer_index = 0;
}

Bit32u bx_floppy_ctrl_c::calculate_step_delay(Bit8u drive, Bit8u new_cylinder)
{
  Bit8u steps;
  Bit32u one_step_delay;

  if (new_cylinder == BX_FD_THIS s.cylinder[drive]) {
    steps = 1;
  } else {
    steps = abs(new_cylinder - BX_FD_THIS s.cylinder[drive]);
    reset_changeline();
  }
  one_step_delay = ((BX_FD_THIS s.SRT ^ 0x0f) + 1) * 500000 / drate_in_k[BX_FD_THIS s.data_rate];
  return (steps * one_step_delay);
}

void bx_floppy_ctrl_c::reset_changeline(void)
{
  Bit8u drive = BX_FD_THIS s.DOR & 0x03;
  if (BX_FD_THIS s.media_present[drive])
    BX_FD_THIS s.DIR[drive] &= ~0x80;
}

bx_bool bx_floppy_ctrl_c::get_tc(void)
{
  Bit8u drive;
  bx_bool terminal_count;
  if (BX_FD_THIS s.main_status_reg & FD_MS_NDMA) {
    drive = BX_FD_THIS s.DOR & 0x03;
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

Bit64s bx_floppy_ctrl_c::floppy_param_handler(bx_param_c *param, int set, Bit64s val)
{
  bx_list_c *base = (bx_list_c*) param->get_parent();
  Bit8u drive;

  if (set) {
    drive = atoi(base->get_name());
    if (!strcmp(param->get_name(), "status")) {
      BX_FD_THIS s.media[drive].status_changed = 1;
    } else if (!strcmp(param->get_name(), "readonly")) {
      BX_FD_THIS s.media[drive].write_protected = (bx_bool)val;
      BX_FD_THIS s.media[drive].status_changed = 1;
    }
  }
  return val;
}

const char* bx_floppy_ctrl_c::floppy_param_string_handler(bx_param_string_c *param,
                                int set, const char *oldval, const char *val, int maxlen)
{
  char pname[BX_PATHNAME_LEN];
  Bit8u drive;

  bx_list_c *base = (bx_list_c*) param->get_parent();
  if ((strlen(val) < 1) || !strcmp ("none", val)) {
    val = "none";
  }
  param->get_param_path(pname, BX_PATHNAME_LEN);
  if ((!strncmp(pname, "floppy", 6)) && (!strcmp(param->get_name(), "path"))) {
    if (set==1) {
      drive = atoi(base->get_name());
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
  int i;

  dbg_printf("i82077AA FDC\n\n");
  for (i = 0; i < 2; i++) {
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
  if (BX_FD_THIS s.pending_command == 0) {
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
  dbg_printf("DSR = 0x%02x\n", BX_FD_THIS s.data_rate);
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif
