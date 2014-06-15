/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// Useful docs:
// AT Attachment with Packet Interface
// working draft by T13 at www.t13.org

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "harddrv.h"
#include "hdimage/hdimage.h"
#include "hdimage/cdrom.h"

#define LOG_THIS theHardDrive->

#define INDEX_PULSE_CYCLE 10

#define PACKET_SIZE 12

// some packet handling macros
#define EXTRACT_FIELD(arr,byte,start,num_bits) (((arr)[(byte)] >> (start)) & ((1 << (num_bits)) - 1))
#define get_packet_field(controller,b,s,n) (EXTRACT_FIELD((controller->buffer),(b),(s),(n)))
#define get_packet_byte(controller,b) (controller->buffer[(b)])
#define get_packet_word(controller,b) (((Bit16u)controller->buffer[(b)] << 8) | controller->buffer[(b)+1])


#define BX_CONTROLLER(c,a) (BX_HD_THIS channels[(c)].drives[(a)]).controller
#define BX_DRIVE(c,a) (BX_HD_THIS channels[(c)].drives[(a)])

#define BX_DRIVE_IS_PRESENT(c,a) (BX_HD_THIS channels[(c)].drives[(a)].device_type != IDE_NONE)
#define BX_DRIVE_IS_HD(c,a) (BX_HD_THIS channels[(c)].drives[(a)].device_type == IDE_DISK)
#define BX_DRIVE_IS_CD(c,a) (BX_HD_THIS channels[(c)].drives[(a)].device_type == IDE_CDROM)

#define BX_MASTER_IS_PRESENT(c) BX_DRIVE_IS_PRESENT((c),0)
#define BX_SLAVE_IS_PRESENT(c) BX_DRIVE_IS_PRESENT((c),1)
#define BX_ANY_IS_PRESENT(c) (BX_DRIVE_IS_PRESENT((c),0) || BX_DRIVE_IS_PRESENT((c),1))

#define BX_SELECTED_CONTROLLER(c) (BX_CONTROLLER((c),BX_HD_THIS channels[(c)].drive_select))
#define BX_SELECTED_DRIVE(c) (BX_DRIVE((c),BX_HD_THIS channels[(c)].drive_select))
#define BX_MASTER_SELECTED(c) (!BX_HD_THIS channels[(c)].drive_select)
#define BX_SLAVE_SELECTED(c)  (BX_HD_THIS channels[(c)].drive_select)

#define BX_SELECTED_IS_PRESENT(c) (BX_DRIVE_IS_PRESENT((c),BX_SLAVE_SELECTED((c))))
#define BX_SELECTED_IS_HD(c) (BX_DRIVE_IS_HD((c),BX_SLAVE_SELECTED((c))))
#define BX_SELECTED_IS_CD(c) (BX_DRIVE_IS_CD((c),BX_SLAVE_SELECTED((c))))

#define BX_SELECTED_MODEL(c) (BX_HD_THIS channels[(c)].drives[BX_HD_THIS channels[(c)].drive_select].model_no)
#define BX_SELECTED_TYPE_STRING(channel) ((BX_SELECTED_IS_CD(channel)) ? "CD-ROM" : "DISK")

#define WRITE_FEATURES(c,a) do { Bit8u _a = a; \
  BX_CONTROLLER((c),0).hob.feature = BX_CONTROLLER((c),0).features; \
  BX_CONTROLLER((c),1).hob.feature = BX_CONTROLLER((c),1).features; \
  BX_CONTROLLER((c),0).features = _a; BX_CONTROLLER((c),1).features = _a; } while(0)
#define WRITE_SECTOR_COUNT(c,a) do { Bit8u _a = a; \
  BX_CONTROLLER((c),0).hob.nsector = BX_CONTROLLER((c),0).sector_count; \
  BX_CONTROLLER((c),1).hob.nsector = BX_CONTROLLER((c),1).sector_count; \
  BX_CONTROLLER((c),0).sector_count = _a; BX_CONTROLLER((c),1).sector_count = _a; } while(0)
#define WRITE_SECTOR_NUMBER(c,a) do { Bit8u _a = a; \
  BX_CONTROLLER((c),0).hob.sector = BX_CONTROLLER((c),0).sector_no; \
  BX_CONTROLLER((c),1).hob.sector = BX_CONTROLLER((c),1).sector_no; \
  BX_CONTROLLER((c),0).sector_no = _a; BX_CONTROLLER((c),1).sector_no = _a; } while(0)
#define WRITE_CYLINDER_LOW(c,a) do { Bit8u _a = a; \
  BX_CONTROLLER((c),0).hob.lcyl = (Bit8u)(BX_CONTROLLER((c),0).cylinder_no & 0xff); \
  BX_CONTROLLER((c),1).hob.lcyl = (Bit8u)(BX_CONTROLLER((c),1).cylinder_no & 0xff); \
  BX_CONTROLLER((c),0).cylinder_no = (BX_CONTROLLER((c),0).cylinder_no & 0xff00) | _a; \
  BX_CONTROLLER((c),1).cylinder_no = (BX_CONTROLLER((c),1).cylinder_no & 0xff00) | _a; } while(0)
#define WRITE_CYLINDER_HIGH(c,a) do { Bit16u _a = a; \
  BX_CONTROLLER((c),0).hob.hcyl = (Bit8u)(BX_CONTROLLER((c),0).cylinder_no >> 8); \
  BX_CONTROLLER((c),1).hob.hcyl = (Bit8u)(BX_CONTROLLER((c),1).cylinder_no >> 8); \
  BX_CONTROLLER((c),0).cylinder_no = (_a << 8) | (BX_CONTROLLER((c),0).cylinder_no & 0xff); \
  BX_CONTROLLER((c),1).cylinder_no = (_a << 8) | (BX_CONTROLLER((c),1).cylinder_no & 0xff); } while(0)
#define WRITE_HEAD_NO(c,a) do { Bit8u _a = a; BX_CONTROLLER((c),0).head_no = _a; BX_CONTROLLER((c),1).head_no = _a; } while(0)
#define WRITE_LBA_MODE(c,a) do { Bit8u _a = a; BX_CONTROLLER((c),0).lba_mode = _a; BX_CONTROLLER((c),1).lba_mode = _a; } while(0)

BX_CPP_INLINE Bit16u read_16bit(const Bit8u* buf)
{
  return (buf[0] << 8) | buf[1];
}

BX_CPP_INLINE Bit32u read_32bit(const Bit8u* buf)
{
  return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

bx_hard_drive_c *theHardDrive = NULL;

int CDECL libharddrv_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theHardDrive = new bx_hard_drive_c();
  bx_devices.pluginHardDrive = theHardDrive;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theHardDrive, BX_PLUGIN_HARDDRV);
  return(0); // Success
}

void CDECL libharddrv_LTX_plugin_fini(void)
{
  delete theHardDrive;
}

bx_hard_drive_c::bx_hard_drive_c()
{
  put("harddrv", "HD");
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    for (Bit8u device=0; device<2; device ++) {
      channels[channel].drives[device].hdimage =  NULL;
      channels[channel].drives[device].cdrom.cd =  NULL;
      channels[channel].drives[device].seek_timer_index = BX_NULL_TIMER_HANDLE;
    }
  }
}

bx_hard_drive_c::~bx_hard_drive_c()
{
  char  ata_name[20];
  bx_list_c *base;

  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    for (Bit8u device=0; device<2; device ++) {
      if (channels[channel].drives[device].hdimage != NULL) {
        channels[channel].drives[device].hdimage->close();
        delete channels[channel].drives[device].hdimage;
        channels[channel].drives[device].hdimage = NULL;
      }
      if (channels[channel].drives[device].cdrom.cd != NULL) {
        delete channels[channel].drives[device].cdrom.cd;
        channels[channel].drives[device].cdrom.cd = NULL;
      }
      sprintf(ata_name, "ata.%d.%s", channel, (device==0)?"master":"slave");
      base = (bx_list_c*) SIM->get_param(ata_name);
      SIM->get_param_string("path", base)->set_handler(NULL);
      SIM->get_param_enum("status", base)->set_handler(NULL);
    }
  }
  SIM->get_bochs_root()->remove("hard_drive");
  BX_DEBUG(("Exit"));
}

void bx_hard_drive_c::init(void)
{
  Bit8u channel, image_mode;
  char  string[5];
  char  sbtext[8];
  char  ata_name[20];
  char  pname[8];
  bx_list_c *base;

  BX_DEBUG(("Init $Id$"));

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(ata_name, "ata.%d.resources", channel);
    base = (bx_list_c*) SIM->get_param(ata_name);
    if (SIM->get_param_bool("enabled", base)->get() == 1) {
      BX_HD_THIS channels[channel].ioaddr1 = SIM->get_param_num("ioaddr1", base)->get();
      BX_HD_THIS channels[channel].ioaddr2 = SIM->get_param_num("ioaddr2", base)->get();
      BX_HD_THIS channels[channel].irq = SIM->get_param_num("irq", base)->get();

      // Coherency check
      if ((BX_HD_THIS channels[channel].ioaddr1 == 0) ||
          (BX_HD_THIS channels[channel].ioaddr2 == 0) ||
          (BX_HD_THIS channels[channel].irq == 0))
      {
        BX_PANIC(("incoherency for ata channel %d: io1=0x%x, io2=%x, irq=%d",
          channel,
          BX_HD_THIS channels[channel].ioaddr1,
          BX_HD_THIS channels[channel].ioaddr2,
          BX_HD_THIS channels[channel].irq));
      }
    } else {
      BX_HD_THIS channels[channel].ioaddr1 = 0;
      BX_HD_THIS channels[channel].ioaddr2 = 0;
      BX_HD_THIS channels[channel].irq = 0;
    }
  }

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(string ,"ATA%d", channel);

    if (BX_HD_THIS channels[channel].irq != 0)
      DEV_register_irq(BX_HD_THIS channels[channel].irq, string);

    if (BX_HD_THIS channels[channel].ioaddr1 != 0) {
      DEV_register_ioread_handler(this, read_handler,
                           BX_HD_THIS channels[channel].ioaddr1, string, 6);
      DEV_register_iowrite_handler(this, write_handler,
                           BX_HD_THIS channels[channel].ioaddr1, string, 6);
      for (unsigned addr=0x1; addr<=0x7; addr++) {
        DEV_register_ioread_handler(this, read_handler,
                             BX_HD_THIS channels[channel].ioaddr1+addr, string, 1);
        DEV_register_iowrite_handler(this, write_handler,
                             BX_HD_THIS channels[channel].ioaddr1+addr, string, 1);
      }
    }

    // We don't want to register addresses 0x3f6 and 0x3f7 as they are handled by the floppy controller
    if ((BX_HD_THIS channels[channel].ioaddr2 != 0) && (BX_HD_THIS channels[channel].ioaddr2 != 0x3f0)) {
      for (unsigned addr=0x6; addr<=0x7; addr++) {
        DEV_register_ioread_handler(this, read_handler,
                              BX_HD_THIS channels[channel].ioaddr2+addr, string, 1);
        DEV_register_iowrite_handler(this, write_handler,
                              BX_HD_THIS channels[channel].ioaddr2+addr, string, 1);
      }
    }

    BX_HD_THIS channels[channel].drive_select = 0;
  }

  channel = 0;
  BX_HD_THIS cdrom_count = 0;
  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    for (Bit8u device=0; device<2; device ++) {
      sprintf(ata_name, "ata.%d.%s", channel, (device==0)?"master":"slave");
      base = (bx_list_c*) SIM->get_param(ata_name);

      // Initialize controller state, even if device is not present
      BX_CONTROLLER(channel,device).status.busy           = 0;
      BX_CONTROLLER(channel,device).status.drive_ready    = 1;
      BX_CONTROLLER(channel,device).status.write_fault    = 0;
      BX_CONTROLLER(channel,device).status.seek_complete  = 1;
      BX_CONTROLLER(channel,device).status.drq            = 0;
      BX_CONTROLLER(channel,device).status.corrected_data = 0;
      BX_CONTROLLER(channel,device).status.index_pulse    = 0;
      BX_CONTROLLER(channel,device).status.index_pulse_count = 0;
      BX_CONTROLLER(channel,device).status.err            = 0;

      BX_CONTROLLER(channel,device).error_register = 0x01; // diagnostic code: no error
      BX_CONTROLLER(channel,device).head_no        = 0;
      BX_CONTROLLER(channel,device).sector_count   = 1;
      BX_CONTROLLER(channel,device).sector_no      = 1;
      BX_CONTROLLER(channel,device).cylinder_no    = 0;
      BX_CONTROLLER(channel,device).current_command = 0x00;
      BX_CONTROLLER(channel,device).buffer_index = 0;

      BX_CONTROLLER(channel,device).control.reset       = 0;
      BX_CONTROLLER(channel,device).control.disable_irq = 0;
      BX_CONTROLLER(channel,device).reset_in_progress   = 0;

      BX_CONTROLLER(channel,device).multiple_sectors    = 0;
      BX_CONTROLLER(channel,device).lba_mode            = 0;

      BX_CONTROLLER(channel,device).features            = 0;
      BX_CONTROLLER(channel,device).mdma_mode           = 0;
      BX_CONTROLLER(channel,device).udma_mode           = 0;

      // If not present
      BX_HD_THIS channels[channel].drives[device].device_type  = IDE_NONE;
      BX_HD_THIS channels[channel].drives[device].statusbar_id = -1;
      BX_HD_THIS channels[channel].drives[device].identify_set = 0;
      if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_NONE) continue;

      // Make model string
      strncpy((char*)BX_HD_THIS channels[channel].drives[device].model_no,
        SIM->get_param_string("model", base)->getptr(), 40);
      while (strlen((char *)BX_HD_THIS channels[channel].drives[device].model_no) < 40) {
        strcat((char*)BX_HD_THIS channels[channel].drives[device].model_no, " ");
      }
      BX_HD_THIS channels[channel].drives[device].model_no[40] = 0;

      if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_DISK) {
        BX_DEBUG(("Hard-Disk on target %d/%d",channel,device));
        BX_HD_THIS channels[channel].drives[device].device_type = IDE_DISK;
        sprintf(sbtext, "HD:%d-%s", channel, device?"S":"M");
        BX_HD_THIS channels[channel].drives[device].statusbar_id =
          bx_gui->register_statusitem(sbtext, 1);

        int cyl = SIM->get_param_num("cylinders", base)->get();
        int heads = SIM->get_param_num("heads", base)->get();
        int spt = SIM->get_param_num("spt", base)->get();
        Bit64u disk_size = (Bit64u)cyl * heads * spt * 512;

        image_mode = SIM->get_param_enum("mode", base)->get();
        channels[channel].drives[device].hdimage = DEV_hdimage_init_image(image_mode,
            disk_size, SIM->get_param_string("journal", base)->getptr());

        if (channels[channel].drives[device].hdimage != NULL) {
          BX_INFO(("HD on ata%d-%d: '%s', '%s' mode", channel, device,
                   SIM->get_param_string("path", base)->getptr(),
                   hdimage_mode_names[image_mode]));
        } else {
          // it's safe to return here on failure
          return;
        }
        BX_HD_THIS channels[channel].drives[device].hdimage->cylinders = cyl;
        BX_HD_THIS channels[channel].drives[device].hdimage->heads = heads;
        BX_HD_THIS channels[channel].drives[device].hdimage->spt = spt;

        /* open hard drive image file */
        if ((BX_HD_THIS channels[channel].drives[device].hdimage->open(SIM->get_param_string("path", base)->getptr())) < 0) {
          BX_PANIC(("ata%d-%d: could not open hard drive image file '%s'", channel, device, SIM->get_param_string("path", base)->getptr()));
          return;
        }
        Bit32u image_caps = BX_HD_THIS channels[channel].drives[device].hdimage->get_capabilities();

        if ((image_caps & HDIMAGE_HAS_GEOMETRY) != 0) {
          // If the image provides a geometry, always use it.
          cyl = BX_HD_THIS channels[channel].drives[device].hdimage->cylinders;
          heads = BX_HD_THIS channels[channel].drives[device].hdimage->heads;
          spt = BX_HD_THIS channels[channel].drives[device].hdimage->spt;
          BX_INFO(("ata%d-%d: image geometry: CHS=%d/%d/%d", channel, device, cyl, heads, spt));
        } else {
          if ((cyl == 0) && (image_caps & HDIMAGE_AUTO_GEOMETRY)) {
            // Autodetect number of cylinders
            if ((heads == 0) || (spt == 0)) {
              BX_PANIC(("ata%d-%d cannot have zero heads, or sectors/track", channel, device));
            }
            cyl = (int)(BX_HD_THIS channels[channel].drives[device].hdimage->hd_size / (heads * spt * 512));
            disk_size = ((Bit64u)cyl * heads * spt * 512);
            BX_HD_THIS channels[channel].drives[device].hdimage->cylinders = cyl;
            BX_INFO(("ata%d-%d: autodetect geometry: CHS=%d/%d/%d", channel, device, cyl, heads, spt));
          } else {
            // Default method: use CHS from configuration.
            if (cyl == 0 || heads == 0 || spt == 0) {
              BX_PANIC(("ata%d-%d cannot have zero cylinders, heads, or sectors/track", channel, device));
            }
            BX_INFO(("ata%d-%d: using specified geometry: CHS=%d/%d/%d", channel, device, cyl, heads, spt));
          }
          if (disk_size > BX_HD_THIS channels[channel].drives[device].hdimage->hd_size) {
            BX_PANIC(("ata%d-%d: specified geometry doesn't fit on disk image", channel, device));
          } else if (disk_size < BX_HD_THIS channels[channel].drives[device].hdimage->hd_size) {
            BX_INFO(("ata%d-%d: extra data outside of CHS address range", channel, device));
          }
        }
        BX_HD_THIS channels[channel].drives[device].next_lsector = 0;
        BX_HD_THIS channels[channel].drives[device].curr_lsector = BX_HD_THIS channels[channel].drives[device].hdimage->hd_size / 512;
      } else if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_CDROM) {
        bx_list_c *cdrom_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_CDROM);
        sprintf(pname, "cdrom%d", BX_HD_THIS cdrom_count + 1);
        bx_list_c *menu = new bx_list_c(cdrom_rt, pname, base->get_title());
        menu->set_options(menu->SERIES_ASK | menu->USE_BOX_TITLE);
        menu->add(SIM->get_param("path", base));
        menu->add(SIM->get_param("status", base));
        SIM->get_param_string("path", base)->set_handler(cdrom_path_handler);
        SIM->get_param_enum("status", base)->set_handler(cdrom_status_handler);
        BX_DEBUG(("CDROM on target %d/%d", channel, device));
        BX_HD_THIS channels[channel].drives[device].device_type = IDE_CDROM;
        BX_HD_THIS channels[channel].drives[device].cdrom.locked = 0;
        BX_HD_THIS channels[channel].drives[device].sense.sense_key = SENSE_NONE;
        BX_HD_THIS channels[channel].drives[device].sense.asc = 0;
        BX_HD_THIS channels[channel].drives[device].sense.ascq = 0;
        sprintf(sbtext, "CD:%d-%s", channel, device?"S":"M");
        BX_HD_THIS channels[channel].drives[device].statusbar_id =
          bx_gui->register_statusitem(sbtext, 1);
        BX_HD_THIS cdrom_count++;
        BX_HD_THIS channels[channel].drives[device].device_num = BX_HD_THIS cdrom_count + 48;

        // Check bit fields
        BX_CONTROLLER(channel,device).sector_count = 0;
        BX_CONTROLLER(channel,device).interrupt_reason.c_d = 1;
        if (BX_CONTROLLER(channel,device).sector_count != 0x01)
          BX_PANIC(("interrupt reason bit field error"));

        BX_CONTROLLER(channel,device).sector_count = 0;
        BX_CONTROLLER(channel,device).interrupt_reason.i_o = 1;
        if (BX_CONTROLLER(channel,device).sector_count != 0x02)
          BX_PANIC(("interrupt reason bit field error"));

        BX_CONTROLLER(channel,device).sector_count = 0;
        BX_CONTROLLER(channel,device).interrupt_reason.rel = 1;
        if (BX_CONTROLLER(channel,device).sector_count != 0x04)
          BX_PANIC(("interrupt reason bit field error"));

        BX_CONTROLLER(channel,device).sector_count = 0;
        BX_CONTROLLER(channel,device).interrupt_reason.tag = 3;
        if (BX_CONTROLLER(channel,device).sector_count != 0x18)
          BX_PANIC(("interrupt reason bit field error"));
        BX_CONTROLLER(channel,device).sector_count = 0;

        // allocate low level driver
        BX_HD_THIS channels[channel].drives[device].cdrom.cd = DEV_hdimage_init_cdrom(SIM->get_param_string("path", base)->getptr());
        BX_INFO(("CD on ata%d-%d: '%s'",channel, device, SIM->get_param_string("path", base)->getptr()));

        if (SIM->get_param_enum("status", base)->get() == BX_INSERTED) {
          if (BX_HD_THIS channels[channel].drives[device].cdrom.cd->insert_cdrom()) {
            BX_INFO(("Media present in CD-ROM drive"));
            BX_HD_THIS channels[channel].drives[device].cdrom.ready = 1;
            Bit32u capacity = BX_HD_THIS channels[channel].drives[device].cdrom.cd->capacity();
            BX_HD_THIS channels[channel].drives[device].cdrom.max_lba = capacity - 1;
            BX_HD_THIS channels[channel].drives[device].cdrom.curr_lba = capacity - 1;
            BX_INFO(("Capacity is %d sectors (%.2f MB)", capacity, (float)capacity / 512.0));
          } else {
            BX_INFO(("Could not locate CD-ROM, continuing with media not present"));
            BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
            SIM->get_param_enum("status", base)->set(BX_EJECTED);
          }
        } else {
          BX_INFO(("Media not present in CD-ROM drive"));
          BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
        }
      }
      if (SIM->get_param_enum("type", base)->get() != BX_ATA_DEVICE_NONE) {
        // register timer for HD/CD seek emulation
        if (BX_DRIVE(channel,device).seek_timer_index == BX_NULL_TIMER_HANDLE) {
          BX_DRIVE(channel,device).seek_timer_index =
            DEV_register_timer(this, seek_timer_handler, 1000, 0, 0, "HD/CD seek");
            bx_pc_system.setTimerParam(BX_DRIVE(channel,device).seek_timer_index,
                                       (channel << 1) | device);
        }
      }
    }
  }

  // generate CMOS values for hard drive if not using a CMOS image
  if (!SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->get()) {
    DEV_cmos_set_reg(0x12, 0x00); // start out with: no drive 0, no drive 1

    if (BX_DRIVE_IS_HD(0,0)) {
      // Flag drive type as Fh, use extended CMOS location as real type
      DEV_cmos_set_reg(0x12, (DEV_cmos_get_reg(0x12) & 0x0f) | 0xf0);
      DEV_cmos_set_reg(0x19, 47); // user definable type
      // AMI BIOS: 1st hard disk #cyl low byte
      DEV_cmos_set_reg(0x1b, (BX_DRIVE(0,0).hdimage->cylinders & 0x00ff));
      // AMI BIOS: 1st hard disk #cyl high byte
      DEV_cmos_set_reg(0x1c, (BX_DRIVE(0,0).hdimage->cylinders & 0xff00) >> 8);
      // AMI BIOS: 1st hard disk #heads
      DEV_cmos_set_reg(0x1d, BX_DRIVE(0,0).hdimage->heads);
      // AMI BIOS: 1st hard disk write precompensation cylinder, low byte
      DEV_cmos_set_reg(0x1e, 0xff); // -1
      // AMI BIOS: 1st hard disk write precompensation cylinder, high byte
      DEV_cmos_set_reg(0x1f, 0xff); // -1
      // AMI BIOS: 1st hard disk control byte
      DEV_cmos_set_reg(0x20, (0xc0 | ((BX_DRIVE(0,0).hdimage->heads > 8) << 3)));
      // AMI BIOS: 1st hard disk landing zone, low byte
      DEV_cmos_set_reg(0x21, DEV_cmos_get_reg(0x1b));
      // AMI BIOS: 1st hard disk landing zone, high byte
      DEV_cmos_set_reg(0x22, DEV_cmos_get_reg(0x1c));
      // AMI BIOS: 1st hard disk sectors/track
      DEV_cmos_set_reg(0x23, BX_DRIVE(0,0).hdimage->spt);
    }

    //set up cmos for second hard drive
    if (BX_DRIVE_IS_HD(0,1)) {
      // fill in lower 4 bits of 0x12 for second HD
      DEV_cmos_set_reg(0x12, (DEV_cmos_get_reg(0x12) & 0xf0) | 0x0f);
      DEV_cmos_set_reg(0x1a, 47); // user definable type
      // AMI BIOS: 2nd hard disk #cyl low byte
      DEV_cmos_set_reg(0x24, (BX_DRIVE(0,1).hdimage->cylinders & 0x00ff));
      // AMI BIOS: 2nd hard disk #cyl high byte
      DEV_cmos_set_reg(0x25, (BX_DRIVE(0,1).hdimage->cylinders & 0xff00) >> 8);
      // AMI BIOS: 2nd hard disk #heads
      DEV_cmos_set_reg(0x26, BX_DRIVE(0,1).hdimage->heads);
      // AMI BIOS: 2nd hard disk write precompensation cylinder, low byte
      DEV_cmos_set_reg(0x27, 0xff); // -1
      // AMI BIOS: 2nd hard disk write precompensation cylinder, high byte
      DEV_cmos_set_reg(0x28, 0xff); // -1
      // AMI BIOS: 2nd hard disk, 0x80 if heads>8
      DEV_cmos_set_reg(0x29, (BX_DRIVE(0,1).hdimage->heads > 8) ? 0x80 : 0x00);
      // AMI BIOS: 2nd hard disk landing zone, low byte
      DEV_cmos_set_reg(0x2a, DEV_cmos_get_reg(0x24));
      // AMI BIOS: 2nd hard disk landing zone, high byte
      DEV_cmos_set_reg(0x2b, DEV_cmos_get_reg(0x25));
      // AMI BIOS: 2nd hard disk sectors/track
      DEV_cmos_set_reg(0x2c, BX_DRIVE(0,1).hdimage->spt);
    }

    DEV_cmos_set_reg(0x39, 0);
    DEV_cmos_set_reg(0x3a, 0);
    for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
      for (Bit8u device=0; device<2; device ++) {
        sprintf(ata_name, "ata.%d.%s", channel, (device==0)?"master":"slave");
        base = (bx_list_c*) SIM->get_param(ata_name);
        if (SIM->get_param_enum("type", base)->get() != BX_ATA_DEVICE_NONE) {
          if (BX_DRIVE_IS_HD(channel,device)) {
            Bit16u cylinders = BX_DRIVE(channel,device).hdimage->cylinders;
            Bit16u heads = BX_DRIVE(channel,device).hdimage->heads;
            Bit16u spt = BX_DRIVE(channel,device).hdimage->spt;
            Bit8u  translation = SIM->get_param_enum("translation", base)->get();

            Bit8u reg = 0x39 + channel/2;
            Bit8u bitshift = 2 * (device+(2 * (channel%2)));

            // Find the right translation if autodetect
            if (translation == BX_ATA_TRANSLATION_AUTO) {
              if((cylinders <= 1024) && (heads <= 16) && (spt <= 63)) {
                translation = BX_ATA_TRANSLATION_NONE;
              }
              else if (((Bit32u)cylinders * (Bit32u)heads) <= 131072) {
                translation = BX_ATA_TRANSLATION_LARGE;
              }
              else translation = BX_ATA_TRANSLATION_LBA;

              BX_INFO(("translation on ata%d-%d set to '%s'",channel, device,
                        translation==BX_ATA_TRANSLATION_NONE?"none":
                        translation==BX_ATA_TRANSLATION_LARGE?"large":
                        "lba"));
              }

            // FIXME we should test and warn
            // - if LBA and spt != 63
            // - if RECHS and heads != 16
            // - if NONE and size > 1024*16*SPT blocks
            // - if LARGE and size > 8192*16*SPT blocks
            // - if RECHS and size > 1024*240*SPT blocks
            // - if LBA and size > 1024*255*63, not that we can do much about it

            switch(translation) {
              case BX_ATA_TRANSLATION_NONE:
                DEV_cmos_set_reg(reg, DEV_cmos_get_reg(reg) | (0 << bitshift));
                break;
              case BX_ATA_TRANSLATION_LBA:
                DEV_cmos_set_reg(reg, DEV_cmos_get_reg(reg) | (1 << bitshift));
                break;
              case BX_ATA_TRANSLATION_LARGE:
                DEV_cmos_set_reg(reg, DEV_cmos_get_reg(reg) | (2 << bitshift));
                break;
              case BX_ATA_TRANSLATION_RECHS:
                DEV_cmos_set_reg(reg, DEV_cmos_get_reg(reg) | (3 << bitshift));
                break;
              }
            }
          }
        }
      }

    // Set the "non-extended" boot device. This will default to DISKC if cdrom
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

  BX_HD_THIS pci_enabled = SIM->get_param_bool(BXPN_PCI_ENABLED)->get();

  // register handler for correct cdrom parameter handling after runtime config
  SIM->register_runtime_config_handler(BX_HD_THIS_PTR, runtime_config_handler);
}

void bx_hard_drive_c::reset(unsigned type)
{
  for (unsigned channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (BX_HD_THIS channels[channel].irq)
      DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
  }
}

void bx_hard_drive_c::register_state(void)
{
  unsigned i, j;
  char cname[4], dname[8];
  bx_list_c *atapi, *cdrom, *chan, *drive, *status;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "hard_drive", "Hard Drive State");
  for (i=0; i<BX_MAX_ATA_CHANNEL; i++) {
    sprintf(cname, "%d", i);
    chan = new bx_list_c(list, cname);
    for (j=0; j<2; j++) {
      if (BX_DRIVE_IS_PRESENT(i, j)) {
        sprintf(dname, "drive%d", j);
        drive = new bx_list_c(chan, dname);
        if (channels[i].drives[j].hdimage != NULL) {
          channels[i].drives[j].hdimage->register_state(drive);
        }
        if (BX_DRIVE_IS_CD(i, j)) {
          cdrom = new bx_list_c(drive, "cdrom");
          new bx_shadow_bool_c(cdrom, "locked", &BX_HD_THIS channels[i].drives[j].cdrom.locked);
          new bx_shadow_num_c(cdrom, "curr_lba", &BX_HD_THIS channels[i].drives[j].cdrom.curr_lba);
          new bx_shadow_num_c(cdrom, "next_lba", &BX_HD_THIS channels[i].drives[j].cdrom.next_lba);
          new bx_shadow_num_c(cdrom, "remaining_blocks", &BX_HD_THIS channels[i].drives[j].cdrom.remaining_blocks);
          atapi = new bx_list_c(drive, "atapi");
          new bx_shadow_num_c(atapi, "command", &BX_HD_THIS channels[i].drives[j].atapi.command, BASE_HEX);
          new bx_shadow_num_c(atapi, "drq_bytes", &BX_HD_THIS channels[i].drives[j].atapi.drq_bytes);
          new bx_shadow_num_c(atapi, "total_bytes_remaining", &BX_HD_THIS channels[i].drives[j].atapi.total_bytes_remaining);
        } else {
          new bx_shadow_num_c(drive, "curr_lsector", &BX_HD_THIS channels[i].drives[j].curr_lsector);
          new bx_shadow_num_c(drive, "next_lsector", &BX_HD_THIS channels[i].drives[j].next_lsector);
        }
        new bx_shadow_data_c(drive, "buffer", BX_CONTROLLER(i, j).buffer, MAX_MULTIPLE_SECTORS * 512);
        status = new bx_list_c(drive, "status");
        new bx_shadow_bool_c(status, "busy", &BX_CONTROLLER(i, j).status.busy);
        new bx_shadow_bool_c(status, "drive_ready", &BX_CONTROLLER(i, j).status.drive_ready);
        new bx_shadow_bool_c(status, "write_fault", &BX_CONTROLLER(i, j).status.write_fault);
        new bx_shadow_bool_c(status, "seek_complete", &BX_CONTROLLER(i, j).status.seek_complete);
        new bx_shadow_bool_c(status, "drq", &BX_CONTROLLER(i, j).status.drq);
        new bx_shadow_bool_c(status, "corrected_data", &BX_CONTROLLER(i, j).status.corrected_data);
        new bx_shadow_bool_c(status, "index_pulse", &BX_CONTROLLER(i, j).status.index_pulse);
        new bx_shadow_num_c(status, "index_pulse_count", &BX_CONTROLLER(i, j).status.index_pulse_count);
        new bx_shadow_bool_c(status, "err", &BX_CONTROLLER(i, j).status.err);
        new bx_shadow_num_c(drive, "error_register", &BX_CONTROLLER(i, j).error_register, BASE_HEX);
        new bx_shadow_num_c(drive, "head_no", &BX_CONTROLLER(i, j).head_no, BASE_HEX);
        new bx_shadow_num_c(drive, "sector_count", &BX_CONTROLLER(i, j).sector_count, BASE_HEX);
        new bx_shadow_num_c(drive, "sector_no", &BX_CONTROLLER(i, j).sector_no, BASE_HEX);
        new bx_shadow_num_c(drive, "cylinder_no", &BX_CONTROLLER(i, j).cylinder_no, BASE_HEX);
        new bx_shadow_num_c(drive, "buffer_size", &BX_CONTROLLER(i, j).buffer_size, BASE_HEX);
        new bx_shadow_num_c(drive, "buffer_index", &BX_CONTROLLER(i, j).buffer_index, BASE_HEX);
        new bx_shadow_num_c(drive, "drq_index", &BX_CONTROLLER(i, j).drq_index, BASE_HEX);
        new bx_shadow_num_c(drive, "current_command", &BX_CONTROLLER(i, j).current_command, BASE_HEX);
        new bx_shadow_num_c(drive, "multiple_sectors", &BX_CONTROLLER(i, j).multiple_sectors, BASE_HEX);
        new bx_shadow_bool_c(drive, "lba_mode", &BX_CONTROLLER(i, j).lba_mode);
        new bx_shadow_num_c(drive, "packet_dma", &BX_CONTROLLER(i, j).packet_dma, BASE_HEX);
        new bx_shadow_bool_c(drive, "control_reset", &BX_CONTROLLER(i, j).control.reset);
        new bx_shadow_bool_c(drive, "control_disable_irq", &BX_CONTROLLER(i, j).control.disable_irq);
        new bx_shadow_num_c(drive, "reset_in_progress", &BX_CONTROLLER(i, j).reset_in_progress, BASE_HEX);
        new bx_shadow_num_c(drive, "features", &BX_CONTROLLER(i, j).features, BASE_HEX);
        new bx_shadow_num_c(drive, "mdma_mode", &BX_CONTROLLER(i, j).mdma_mode, BASE_HEX);
        new bx_shadow_num_c(drive, "udma_mode", &BX_CONTROLLER(i, j).udma_mode, BASE_HEX);
        new bx_shadow_num_c(drive, "hob_feature", &BX_CONTROLLER(i, j).hob.feature, BASE_HEX);
        new bx_shadow_num_c(drive, "hob_nsector", &BX_CONTROLLER(i, j).hob.nsector, BASE_HEX);
        new bx_shadow_num_c(drive, "hob_sector", &BX_CONTROLLER(i, j).hob.sector, BASE_HEX);
        new bx_shadow_num_c(drive, "hob_lcyl", &BX_CONTROLLER(i, j).hob.lcyl, BASE_HEX);
        new bx_shadow_num_c(drive, "hob_hcyl", &BX_CONTROLLER(i, j).hob.hcyl, BASE_HEX);
        new bx_shadow_num_c(drive, "num_sectors", &BX_CONTROLLER(i, j).num_sectors, BASE_HEX);
      }
    }
    new bx_shadow_num_c(chan, "drive_select", &BX_HD_THIS channels[i].drive_select);
  }
}

void bx_hard_drive_c::seek_timer_handler(void *this_ptr)
{
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;
  class_ptr->seek_timer();
}

void bx_hard_drive_c::seek_timer()
{
  Bit8u param = bx_pc_system.triggeredTimerParam();
  Bit8u channel = param >> 1;
  Bit8u device = param & 1;
  controller_t *controller = &BX_CONTROLLER(channel, device);
  if (BX_DRIVE_IS_HD(channel, device)) {
    switch (controller->current_command) {
      case 0x24: // READ SECTORS EXT
      case 0x29: // READ MULTIPLE EXT
      case 0x20: // READ SECTORS, with retries
      case 0x21: // READ SECTORS, without retries
      case 0xC4: // READ MULTIPLE SECTORS
        controller->error_register = 0;
        controller->status.busy  = 0;
        controller->status.drive_ready = 1;
        controller->status.seek_complete = 1;
        controller->status.drq   = 1;
        controller->status.corrected_data = 0;
        controller->buffer_index = 0;
        raise_interrupt(channel);
        break;
      case 0x25: // READ DMA EXT
      case 0xC8: // READ DMA
        controller->error_register = 0;
        controller->status.busy  = 0;
        controller->status.drive_ready = 1;
        controller->status.seek_complete = 1;
        controller->status.drq   = 1;
        controller->status.corrected_data = 0;
        DEV_ide_bmdma_start_transfer(channel);
        break;
      case 0x70: // SEEK
        BX_SELECTED_DRIVE(channel).curr_lsector = BX_SELECTED_DRIVE(channel).next_lsector;
        controller->error_register = 0;
        controller->status.busy  = 0;
        controller->status.drive_ready = 1;
        controller->status.seek_complete = 1;
        controller->status.drq   = 0;
        controller->status.corrected_data = 0;
        controller->buffer_index = 0;
        BX_DEBUG(("ata%d-%d: SEEK completed (IRQ %sabled)", channel,
          BX_SLAVE_SELECTED(channel), controller->control.disable_irq?"dis":"en"));
        raise_interrupt(channel);
        break;
      default:
        BX_ERROR(("seek_timer(): ATA command 0x%02x not supported",
                  controller->current_command));
    }
  } else {
    switch (BX_DRIVE(channel, device).atapi.command) {
      case 0x28: // read (10)
      case 0xa8: // read (12)
      case 0xbe: // read cd
        ready_to_send_atapi(channel);
        break;
      default:
        BX_ERROR(("seek_timer(): ATAPI command 0x%02x not supported",
                  BX_DRIVE(channel, device).atapi.command));
    }
  }
}

void bx_hard_drive_c::runtime_config_handler(void *this_ptr)
{
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_hard_drive_c::runtime_config(void)
{
  char pname[16];
  int handle;
  bx_bool status;

  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    for (Bit8u device=0; device<2; device++) {
      if (BX_HD_THIS channels[channel].drives[device].status_changed) {
        handle = (channel << 1) | device;
        sprintf(pname, "ata.%d.%s", channel, device ? "slave":"master");
        bx_list_c *base = (bx_list_c*) SIM->get_param(pname);
        status = SIM->get_param_enum("status", base)->get();
        BX_HD_THIS set_cd_media_status(handle, 0);
        if (status == BX_INSERTED) {
          BX_HD_THIS set_cd_media_status(handle, 1);
        }
        BX_HD_THIS channels[channel].drives[device].status_changed = 0;
      }
    }
  }
}

#define GOTO_RETURN_VALUE  if(io_len==4) {            \
                             goto return_value32;     \
                           }                          \
                           else if(io_len==2) {       \
                             value16=(Bit16u)value32; \
                             goto return_value16;     \
                           }                          \
                           else {                     \
                             value8=(Bit8u)value32;   \
                             goto return_value8;      \
                           }


// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions
Bit32u bx_hard_drive_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_HD_SMF
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_hard_drive_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_HD_SMF
  Bit8u  value8;
  Bit16u value16;
  Bit32u value32;

  Bit8u  channel = BX_MAX_ATA_CHANNEL;
  Bit32u port = 0xff; // undefined

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if ((address & 0xfff8) == BX_HD_THIS channels[channel].ioaddr1) {
      port = address - BX_HD_THIS channels[channel].ioaddr1;
      break;
     }
    else if ((address & 0xfff8) == BX_HD_THIS channels[channel].ioaddr2) {
      port = address - BX_HD_THIS channels[channel].ioaddr2 + 0x10;
      break;
    }
  }

  if (channel == BX_MAX_ATA_CHANNEL) {
    channel = 0;
    if ((address < 0x03f6) || (address > 0x03f7)) {
      BX_PANIC(("read: unable to find ATA channel, ioport=0x%04x", address));
    } else {
      port = address - 0x03e0;
    }
  }

  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  switch (port) {
    case 0x00: // hard disk data (16bit) 0x1f0
      if (controller->status.drq == 0) {
            BX_ERROR(("IO read(0x%04x) with drq == 0: last command was %02xh",
                     address, (unsigned) controller->current_command));
            return(0);
      }
      BX_DEBUG(("IO read(0x%04x): current command is %02xh",
            address, (unsigned) controller->current_command));
      switch (controller->current_command) {
        case 0x20: // READ SECTORS, with retries
        case 0x21: // READ SECTORS, without retries
        case 0xC4: // READ MULTIPLE SECTORS
        case 0x24: // READ SECTORS EXT
        case 0x29: // READ MULTIPLE EXT
          if (controller->buffer_index >= controller->buffer_size)
            BX_PANIC(("IO read(0x%04x): buffer_index >= %d", address, controller->buffer_size));

#if BX_SupportRepeatSpeedups
          if (DEV_bulk_io_quantum_requested()) {
            unsigned transferLen, quantumsMax;
            quantumsMax = (controller->buffer_size - controller->buffer_index) / io_len;
            if (quantumsMax == 0)
              BX_PANIC(("IO read(0x%04x): not enough space for read", address));
            DEV_bulk_io_quantum_transferred() = DEV_bulk_io_quantum_requested();
            if (quantumsMax < DEV_bulk_io_quantum_transferred())
              DEV_bulk_io_quantum_transferred() = quantumsMax;
            transferLen = io_len * DEV_bulk_io_quantum_transferred();
            memcpy((Bit8u*) DEV_bulk_io_host_addr(),
              &controller->buffer[controller->buffer_index], transferLen);
            DEV_bulk_io_host_addr() += transferLen;
            controller->buffer_index += transferLen;
            value32 = 0; // Value returned not important;
          }
          else
#endif
          {
            value32 = 0L;
            switch(io_len){
              case 4:
                value32 |= (controller->buffer[controller->buffer_index+3] << 24);
                value32 |= (controller->buffer[controller->buffer_index+2] << 16);
              case 2:
                value32 |= (controller->buffer[controller->buffer_index+1] << 8);
                value32 |=  controller->buffer[controller->buffer_index];
            }
            controller->buffer_index += io_len;
          }

          // if buffer completely read
          if (controller->buffer_index >= controller->buffer_size) {
            // update sector count, sector number, cylinder,
            // drive, head, status
            // if there are more sectors, read next one in...
            //
            if ((controller->current_command == 0xC4) ||
                (controller->current_command == 0x29)) {
              if (controller->num_sectors > controller->multiple_sectors) {
                controller->buffer_size = controller->multiple_sectors * 512;
              } else {
                controller->buffer_size = controller->num_sectors * 512;
              }
            }

            controller->status.busy = 0;
            controller->status.drive_ready = 1;
            controller->status.write_fault = 0;
            controller->status.seek_complete = 1;
            controller->status.corrected_data = 0;
            controller->status.err = 0;

            if (controller->num_sectors==0) {
              controller->status.drq = 0;
              BX_SELECTED_DRIVE(channel).curr_lsector = BX_SELECTED_DRIVE(channel).next_lsector;
            } else { /* read next one into controller buffer */
              controller->status.drq = 1;
              controller->status.seek_complete = 1;

              if (ide_read_sector(channel, controller->buffer, controller->buffer_size)) {
                controller->buffer_index = 0;
                raise_interrupt(channel);
              }
            }
          }
          GOTO_RETURN_VALUE;
          break;

        case 0xec:    // IDENTIFY DEVICE
        case 0xa1:
          unsigned index;

          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.write_fault = 0;
          controller->status.seek_complete = 1;
          controller->status.corrected_data = 0;
          controller->status.err = 0;

          index = controller->buffer_index;
          value32 = controller->buffer[index];
          index++;
          if (io_len >= 2) {
            value32 |= (controller->buffer[index] << 8);
            index++;
          }
          if (io_len == 4) {
            value32 |= (controller->buffer[index] << 16);
            value32 |= (controller->buffer[index+1] << 24);
            index += 2;
          }
          controller->buffer_index = index;

          if (controller->buffer_index >= 512) {
            controller->status.drq = 0;
            BX_DEBUG(("Read all drive ID Bytes ..."));
          }
          GOTO_RETURN_VALUE;
          break;

        case 0xa0:
          {
            unsigned index = controller->buffer_index;
            unsigned increment = 0;

            // Load block if necessary
            if (index >= controller->buffer_size) {
              if (index > controller->buffer_size)
                BX_PANIC(("index > %d : %d", controller->buffer_size, index));
              switch (BX_SELECTED_DRIVE(channel).atapi.command) {
                case 0x28: // read (10)
                case 0xa8: // read (12)
                case 0xbe: // read cd
                  if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    BX_PANIC(("Read with CDROM not ready"));
                  }
                  /* set status bar conditions for device */
                  bx_gui->statusbar_setitem(BX_SELECTED_DRIVE(channel).statusbar_id, 1);
                  if (!BX_SELECTED_DRIVE(channel).cdrom.cd->read_block(controller->buffer,
                                                                       BX_SELECTED_DRIVE(channel).cdrom.next_lba,
                                                                       controller->buffer_size))
                  {
                    BX_PANIC(("CDROM: read block %d failed", BX_SELECTED_DRIVE(channel).cdrom.next_lba));
                  }
                  BX_SELECTED_DRIVE(channel).cdrom.next_lba++;
                  BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks--;

                  if (!BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks) {
                    BX_SELECTED_DRIVE(channel).cdrom.curr_lba = BX_SELECTED_DRIVE(channel).cdrom.next_lba;
                    BX_DEBUG(("CDROM: last READ block loaded"));
                  } else {
                    BX_DEBUG(("CDROM: READ block loaded (%d remaining)",
                             BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks));
                  }
                  // one block transfered, start at beginning
                  index = 0;
                  break;

                default: // no need to load a new block
                  break;
              }
            }

            value32 = controller->buffer[index+increment];
            increment++;
            if (io_len >= 2) {
              value32 |= (controller->buffer[index+increment] << 8);
              increment++;
            }
            if (io_len == 4) {
              value32 |= (controller->buffer[index+increment] << 16);
              value32 |= (controller->buffer[index+increment+1] << 24);
              increment += 2;
            }
            controller->buffer_index = index + increment;
            controller->drq_index += increment;

            if (controller->drq_index >= (unsigned)BX_SELECTED_DRIVE(channel).atapi.drq_bytes) {
              controller->status.drq = 0;
              controller->drq_index = 0;

              BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining -= BX_SELECTED_DRIVE(channel).atapi.drq_bytes;

              if (BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining > 0) {
                // one or more blocks remaining (works only for single block commands)
                BX_DEBUG(("PACKET drq bytes read"));
                controller->interrupt_reason.i_o = 1;
                controller->status.busy = 0;
                controller->status.drq = 1;
                controller->interrupt_reason.c_d = 0;

                // set new byte count if last block
                if (BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining < controller->byte_count) {
                  controller->byte_count = BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining;
                }
                BX_SELECTED_DRIVE(channel).atapi.drq_bytes = controller->byte_count;

                raise_interrupt(channel);
              } else {
                // all bytes read
                BX_DEBUG(("PACKET all bytes read"));
                controller->interrupt_reason.i_o = 1;
                controller->interrupt_reason.c_d = 1;
                controller->status.drive_ready = 1;
                controller->interrupt_reason.rel = 0;
                controller->status.busy = 0;
                controller->status.drq = 0;
                controller->status.err = 0;

                raise_interrupt(channel);
              }
            }
            GOTO_RETURN_VALUE;
          }
          break;

        default:
          BX_ERROR(("read from 0x%04x: current command is 0x%02x", address,
                    controller->current_command));
      }
      break;

    case 0x01: // hard disk error register 0x1f1
      // -- WARNING : On real hardware the controller registers are shared between drives.
      // So we must respond even if the select device is not present. Some OS uses this fact
      // to detect the disks.... minix2 for example
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : controller->error_register;
      goto return_value8;
    case 0x02: // hard disk sector count / interrupt reason 0x1f2
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : controller->sector_count;
      goto return_value8;
    case 0x03: // sector number 0x1f3
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : controller->sector_no;
      goto return_value8;
    case 0x04: // cylinder low 0x1f4
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : (controller->cylinder_no & 0x00ff);
      goto return_value8;
    case 0x05: // cylinder high 0x1f5
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : controller->cylinder_no >> 8;
      goto return_value8;

    case 0x06: // hard disk drive and head register 0x1f6
      // b7 Extended data field for ECC
      // b6/b5: Used to be sector size.  00=256,01=512,10=1024,11=128
      //   Since 512 was always used, bit 6 was taken to mean LBA mode:
      //     b6 1=LBA mode, 0=CHS mode
      //     b5 1
      // b4: DRV
      // b3..0 HD3..HD0
      value8 = (1 << 7) |
               (controller->lba_mode << 6) |
               (1 << 5) | // 01b = 512 sector size
               (BX_HD_THIS channels[channel].drive_select << 4) |
               (controller->head_no << 0);
      goto return_value8;

    case 0x07: // Hard Disk Status 0x1f7
    case 0x16: // Hard Disk Alternate Status 0x3f6
      if (!BX_SELECTED_IS_PRESENT(channel)) {
        // (mch) Just return zero for these registers
        value8 = 0;
      } else {
        value8 = (
          (controller->status.busy << 7) |
          (controller->status.drive_ready << 6) |
          (controller->status.write_fault << 5) |
          (controller->status.seek_complete << 4) |
          (controller->status.drq << 3) |
          (controller->status.corrected_data << 2) |
          (controller->status.index_pulse << 1) |
          (controller->status.err));
        controller->status.index_pulse_count++;
        controller->status.index_pulse = 0;
        if (controller->status.index_pulse_count >= INDEX_PULSE_CYCLE) {
          controller->status.index_pulse = 1;
          controller->status.index_pulse_count = 0;
        }
      }
      if (port == 0x07) {
        DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
      }
      goto return_value8;

    case 0x17: // Hard Disk Address Register 0x3f7
      // Obsolete and unsupported register.  Not driven by hard
      // disk controller.  Report all 1's.  If floppy controller
      // is handling this address, it will call this function
      // set/clear D7 (the only bit it handles), then return
      // the combined value
      value8 = 0xff;
      goto return_value8;

    default:
      BX_PANIC(("hard drive: io read to address %x unsupported",
        (unsigned) address));
  }

  BX_PANIC(("hard drive: shouldnt get here!"));
  return(0);

return_value32:
  BX_DEBUG(("32-bit read from %04x = %08x {%s}",
     (unsigned) address, value32, BX_SELECTED_TYPE_STRING(channel)));
  return value32;

return_value16:
  BX_DEBUG(("16-bit read from %04x = %04x {%s}",
     (unsigned) address, value16, BX_SELECTED_TYPE_STRING(channel)));
  return value16;

return_value8:
  BX_DEBUG(("8-bit read from %04x = %02x {%s}",
     (unsigned) address, value8, BX_SELECTED_TYPE_STRING(channel)));
  return value8;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_hard_drive_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_HD_SMF
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_hard_drive_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_HD_SMF
  Bit64s logical_sector;
  bx_bool prev_control_reset;
  bx_bool lba48 = 0;

  Bit8u  channel = BX_MAX_ATA_CHANNEL;
  Bit32u port = 0xff; // undefined
  int i;

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if ((address & 0xfff8) == BX_HD_THIS channels[channel].ioaddr1) {
      port = address - BX_HD_THIS channels[channel].ioaddr1;
      break;
    }
    else if ((address & 0xfff8) == BX_HD_THIS channels[channel].ioaddr2) {
      port = address - BX_HD_THIS channels[channel].ioaddr2 + 0x10;
      break;
    }
  }

  if (channel == BX_MAX_ATA_CHANNEL) {
    if (address != 0x03f6) {
      BX_PANIC(("write: unable to find ATA channel, ioport=0x%04x", address));
    } else {
      channel = 0;
      port = address - 0x03e0;
    }
  }

  switch (io_len) {
    case 1:
      BX_DEBUG(("8-bit write to %04x = %02x {%s}",
                address, value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 2:
      BX_DEBUG(("16-bit write to %04x = %04x {%s}",
                address, value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 4:
      BX_DEBUG(("32-bit write to %04x = %08x {%s}",
                address, value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    default:
      BX_DEBUG(("unknown-size write to %04x = %08x {%s}",
                address, value, BX_SELECTED_TYPE_STRING(channel)));
      break;
  }

  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  switch (port) {
    case 0x00: // 0x1f0
      switch (controller->current_command) {
        case 0x30: // WRITE SECTORS
        case 0xC5: // WRITE MULTIPLE SECTORS
        case 0x34: // WRITE SECTORS EXT
        case 0x39: // WRITE MULTIPLE EXT
          if (controller->buffer_index >= controller->buffer_size)
            BX_PANIC(("IO write(0x%04x): buffer_index >= %d", address, controller->buffer_size));

#if BX_SupportRepeatSpeedups
          if (DEV_bulk_io_quantum_requested()) {
            unsigned transferLen, quantumsMax;
            quantumsMax = (controller->buffer_size - controller->buffer_index) / io_len;
            if (quantumsMax == 0)
              BX_PANIC(("IO write(0x%04x): not enough space for write", address));
            DEV_bulk_io_quantum_transferred() = DEV_bulk_io_quantum_requested();
            if (quantumsMax < DEV_bulk_io_quantum_transferred())
              DEV_bulk_io_quantum_transferred() = quantumsMax;
            transferLen = io_len * DEV_bulk_io_quantum_transferred();
            memcpy(&controller->buffer[controller->buffer_index],
              (Bit8u*) DEV_bulk_io_host_addr(), transferLen);
            DEV_bulk_io_host_addr() += transferLen;
            controller->buffer_index += transferLen;
          }
          else
#endif
          {
            switch(io_len) {
              case 4:
                controller->buffer[controller->buffer_index+3] = (Bit8u)(value >> 24);
                controller->buffer[controller->buffer_index+2] = (Bit8u)(value >> 16);
              case 2:
                controller->buffer[controller->buffer_index+1] = (Bit8u)(value >> 8);
                controller->buffer[controller->buffer_index]   = (Bit8u) value;
            }
            controller->buffer_index += io_len;
          }

          /* if buffer completely writtten */
          if (controller->buffer_index >= controller->buffer_size) {
            if (ide_write_sector(channel, controller->buffer,
                                 controller->buffer_size)) {
              if ((controller->current_command == 0xC5) ||
                  (controller->current_command == 0x39)) {
                if (controller->num_sectors > controller->multiple_sectors) {
                  controller->buffer_size = controller->multiple_sectors * 512;
                } else {
                  controller->buffer_size = controller->num_sectors * 512;
                }
              }
              controller->buffer_index = 0;

              /* When the write is complete, controller clears the DRQ bit and
               * sets the BSY bit.
               * If at least one more sector is to be written, controller sets DRQ bit,
               * clears BSY bit, and issues IRQ
               */

              if (controller->num_sectors != 0) {
                controller->status.busy = 0;
                controller->status.drive_ready = 1;
                controller->status.drq = 1;
                controller->status.corrected_data = 0;
                controller->status.err = 0;
              } else { /* no more sectors to write */
                controller->status.busy = 0;
                controller->status.drive_ready = 1;
                controller->status.drq = 0;
                controller->status.err = 0;
                controller->status.corrected_data = 0;
                BX_SELECTED_DRIVE(channel).curr_lsector = BX_SELECTED_DRIVE(channel).next_lsector;
              }
              raise_interrupt(channel);
            }
          }
          break;

        case 0xa0: // PACKET
          if (controller->buffer_index >= PACKET_SIZE)
            BX_PANIC(("IO write(0x%04x): buffer_index >= PACKET_SIZE", address));

          switch (io_len) {
            case 4:
              controller->buffer[controller->buffer_index+3] = (Bit8u)(value >> 24);
              controller->buffer[controller->buffer_index+2] = (Bit8u)(value >> 16);
            case 2:
              controller->buffer[controller->buffer_index+1] = (Bit8u)(value >> 8);
              controller->buffer[controller->buffer_index]   = (Bit8u) value;
          }
          controller->buffer_index += io_len;

          /* if packet completely writtten */
          if (controller->buffer_index >= PACKET_SIZE) {
            // complete command received
            Bit8u atapi_command = controller->buffer[0];
            controller->buffer_size = 2048;

            BX_DEBUG(("ata%d-%d: ATAPI command 0x%02x started", channel,
                      BX_SLAVE_SELECTED(channel), atapi_command));

            switch (atapi_command) {
              case 0x00: // test unit ready
                if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                  atapi_cmd_nop(controller);
                } else {
                  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 0);
                }
                raise_interrupt(channel);
                break;

              case 0x03: // request sense
                {
                  int alloc_length = controller->buffer[4];
                  init_send_atapi_command(channel, atapi_command, 18, alloc_length);

                  // sense data
                  controller->buffer[0] = 0x70 | (1 << 7);
                  controller->buffer[1] = 0;
                  controller->buffer[2] = BX_SELECTED_DRIVE(channel).sense.sense_key;
                  controller->buffer[3] = BX_SELECTED_DRIVE(channel).sense.information.arr[0];
                  controller->buffer[4] = BX_SELECTED_DRIVE(channel).sense.information.arr[1];
                  controller->buffer[5] = BX_SELECTED_DRIVE(channel).sense.information.arr[2];
                  controller->buffer[6] = BX_SELECTED_DRIVE(channel).sense.information.arr[3];
                  controller->buffer[7] = 17-7;
                  controller->buffer[8] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[0];
                  controller->buffer[9] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[1];
                  controller->buffer[10] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[2];
                  controller->buffer[11] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[3];
                  controller->buffer[12] = BX_SELECTED_DRIVE(channel).sense.asc;
                  controller->buffer[13] = BX_SELECTED_DRIVE(channel).sense.ascq;
                  controller->buffer[14] = BX_SELECTED_DRIVE(channel).sense.fruc;
                  controller->buffer[15] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[0];
                  controller->buffer[16] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[1];
                  controller->buffer[17] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[2];

                  if (BX_SELECTED_DRIVE(channel).sense.sense_key == SENSE_UNIT_ATTENTION) {
                    BX_SELECTED_DRIVE(channel).sense.sense_key = SENSE_NONE;
                  }
                  ready_to_send_atapi(channel);
                }
                break;

              case 0x1b: // start stop unit
                {
                  char ata_name[20];
                  //bx_bool Immed = (controller->buffer[1] >> 0) & 1;
                  bx_bool LoEj = (controller->buffer[4] >> 1) & 1;
                  bx_bool Start = (controller->buffer[4] >> 0) & 1;

                  if (!LoEj && !Start) { // stop the disc
                    BX_ERROR(("FIXME: Stop disc not implemented"));
                    atapi_cmd_nop(controller);
                    raise_interrupt(channel);
                  } else if (!LoEj && Start) { // start (spin up) the disc
                    BX_SELECTED_DRIVE(channel).cdrom.cd->start_cdrom();
                    BX_ERROR(("FIXME: ATAPI start disc not reading TOC"));
                    atapi_cmd_nop(controller);
                    raise_interrupt(channel);
                  } else if (LoEj && !Start) { // Eject the disc
                    atapi_cmd_nop(controller);

                    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                      BX_SELECTED_DRIVE(channel).cdrom.cd->eject_cdrom();
                      BX_SELECTED_DRIVE(channel).cdrom.ready = 0;
                      sprintf(ata_name, "ata.%d.%s", channel, BX_SLAVE_SELECTED(channel)?"slave":"master");
                      bx_list_c *base = (bx_list_c*) SIM->get_param(ata_name);
                      SIM->get_param_enum("status", base)->set(BX_EJECTED);
                      bx_gui->update_drive_status_buttons();
                    }
                    raise_interrupt(channel);
                  } else { // Load the disc
                    // My guess is that this command only closes the tray, that's a no-op for us
                    atapi_cmd_nop(controller);
                    raise_interrupt(channel);
                  }
                }
                break;

              case 0xbd: // mechanism status
                {
                  Bit16u alloc_length = read_16bit(controller->buffer + 8);

                  if (alloc_length == 0)
                    BX_PANIC(("Zero allocation length to MECHANISM STATUS not impl."));

                  init_send_atapi_command(channel, atapi_command, 8, alloc_length);

                  controller->buffer[0] = 0; // reserved for non changers
                  controller->buffer[1] = 0; // reserved for non changers

                  controller->buffer[2] = 0; // Current LBA (TODO!)
                  controller->buffer[3] = 0; // Current LBA (TODO!)
                  controller->buffer[4] = 0; // Current LBA (TODO!)

                  controller->buffer[5] = 1; // one slot

                  controller->buffer[6] = 0; // slot table length
                  controller->buffer[7] = 0; // slot table length

                  ready_to_send_atapi(channel);
                }
                break;

              case 0x1a: // mode sense (6)
              case 0x5a: // mode sense (10)
                {
                  Bit16u alloc_length;

                  if (atapi_command == 0x5a) {
                    alloc_length = read_16bit(controller->buffer + 7);
                  } else {
                    alloc_length = controller->buffer[4];
                  }
                  Bit8u PC = controller->buffer[2] >> 6;
                  Bit8u PageCode = controller->buffer[2] & 0x3f;

                  switch (PC) {
                    case 0x0: // current values
                      switch (PageCode) {
                        case 0x01: // error recovery
                          init_send_atapi_command(channel, atapi_command, sizeof(error_recovery_t) + 8, alloc_length);

                          init_mode_sense_single(channel, &BX_SELECTED_DRIVE(channel).cdrom.current.error_recovery,
                                                 sizeof(error_recovery_t));
                          ready_to_send_atapi(channel);
                          break;

                        case 0x2a: // CD-ROM capabilities & mech. status
                          init_send_atapi_command(channel, atapi_command, 28, alloc_length);
                          init_mode_sense_single(channel, &controller->buffer[8], 28);
                          controller->buffer[8] = 0x2a;
                          controller->buffer[9] = 0x12;
                          controller->buffer[10] = 0x00;
                          controller->buffer[11] = 0x00;
                          // Multisession, Mode 2 Form 2, Mode 2 Form 1
                          controller->buffer[12] = 0x70;
                          controller->buffer[13] = (3 << 5);
                          controller->buffer[14] = (unsigned char) (1 |
                            (BX_SELECTED_DRIVE(channel).cdrom.locked ? (1 << 1) : 0) |
                            (1 << 3) |
                            (1 << 5));
                          controller->buffer[15] = 0x00;
                          controller->buffer[16] = (706 >> 8) & 0xff;
                          controller->buffer[17] = 706 & 0xff;
                          controller->buffer[18] = 0;
                          controller->buffer[19] = 2;
                          controller->buffer[20] = (512 >> 8) & 0xff;
                          controller->buffer[21] = 512 & 0xff;
                          controller->buffer[22] = (706 >> 8) & 0xff;
                          controller->buffer[23] = 706 & 0xff;
                          controller->buffer[24] = 0;
                          controller->buffer[25] = 0;
                          controller->buffer[26] = 0;
                          controller->buffer[27] = 0;
                          ready_to_send_atapi(channel);
                          break;

                        case 0x0d: // CD-ROM
                        case 0x0e: // CD-ROM audio control
                        case 0x3f: // all
                          BX_ERROR(("cdrom: MODE SENSE (curr), code=%x not implemented yet",
                                    PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;

                        default:
                          // not implemeted by this device
                          BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x, not implemented by device",
                                   PC, PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;
                      }
                      break;

                    case 0x1: // changeable values
                      switch (PageCode) {
                        case 0x01: // error recovery
                        case 0x0d: // CD-ROM
                        case 0x0e: // CD-ROM audio control
                        case 0x2a: // CD-ROM capabilities & mech. status
                        case 0x3f: // all
                          BX_ERROR(("cdrom: MODE SENSE (chg), code=%x not implemented yet", PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;

                        default:
                          // not implemeted by this device
                          BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x, not implemented by device",
                                   PC, PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;
                      }
                      break;

                    case 0x2: // default values
                      switch (PageCode) {
                        case 0x2a: // CD-ROM capabilities & mech. status, copied from current values
                          init_send_atapi_command(channel, atapi_command, 28, alloc_length);
                          init_mode_sense_single(channel, &controller->buffer[8], 28);
                          controller->buffer[8] = 0x2a;
                          controller->buffer[9] = 0x12;
                          controller->buffer[10] = 0x00;
                          controller->buffer[11] = 0x00;
                          // Multisession, Mode 2 Form 2, Mode 2 Form 1
                          controller->buffer[12] = 0x70;
                          controller->buffer[13] = (3 << 5);
                          controller->buffer[14] = (unsigned char) (1 |
                            (BX_SELECTED_DRIVE(channel).cdrom.locked ? (1 << 1) : 0) |
                            (1 << 3) |
                            (1 << 5));
                          controller->buffer[15] = 0x00;
                          controller->buffer[16] = (706 >> 8) & 0xff;
                          controller->buffer[17] = 706 & 0xff;
                          controller->buffer[18] = 0;
                          controller->buffer[19] = 2;
                          controller->buffer[20] = (512 >> 8) & 0xff;
                          controller->buffer[21] = 512 & 0xff;
                          controller->buffer[22] = (706 >> 8) & 0xff;
                          controller->buffer[23] = 706 & 0xff;
                          controller->buffer[24] = 0;
                          controller->buffer[25] = 0;
                          controller->buffer[26] = 0;
                          controller->buffer[27] = 0;
                          ready_to_send_atapi(channel);
                          break;

                        case 0x01: // error recovery
                        case 0x0d: // CD-ROM
                        case 0x0e: // CD-ROM audio control
                        case 0x3f: // all
                          BX_ERROR(("cdrom: MODE SENSE (dflt), code=%x not implemented", PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;

                        default:
                          // not implemeted by this device
                          BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x, not implemented by device",
                                   PC, PageCode));
                          atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
                                          ASC_INV_FIELD_IN_CMD_PACKET, 1);
                          raise_interrupt(channel);
                          break;
                      }
                      break;

                    case 0x3: // saved values not implemented
                      atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_SAVING_PARAMETERS_NOT_SUPPORTED, 1);
                      raise_interrupt(channel);
                      break;

                    default:
                      BX_PANIC(("Should not get here!"));
                      break;
                  }
                }
                break;

              case 0x12: // inquiry
                {
                  Bit8u alloc_length = controller->buffer[4];

                  init_send_atapi_command(channel, atapi_command, 36, alloc_length);

                  controller->buffer[0] = 0x05; // CD-ROM
                  controller->buffer[1] = 0x80; // Removable
                  controller->buffer[2] = 0x00; // ISO, ECMA, ANSI version
                  controller->buffer[3] = 0x21; // ATAPI-2, as specified
                  controller->buffer[4] = 31; // additional length (total 36)
                  controller->buffer[5] = 0x00; // reserved
                  controller->buffer[6] = 0x00; // reserved
                  controller->buffer[7] = 0x00; // reserved

                  // Vendor ID
                  const char* vendor_id = "BOCHS   ";
                  for (i = 0; i < 8; i++)
                    controller->buffer[8+i] = vendor_id[i];

                  // Product ID
                  const char* product_id = "Generic CD-ROM  ";
                  for (i = 0; i < 16; i++)
                    controller->buffer[16+i] = product_id[i];
                  if (BX_HD_THIS cdrom_count > 1) {
                    controller->buffer[31] = BX_SELECTED_DRIVE(channel).device_num;
                  }

                  // Product Revision level
                  const char* rev_level = "1.0 ";
                  for (i = 0; i < 4; i++)
                    controller->buffer[32+i] = rev_level[i];

                  ready_to_send_atapi(channel);
                }
                break;

              case 0x25: // read cd-rom capacity
                {
                  // no allocation length???
                  init_send_atapi_command(channel, atapi_command, 8, 8);

                  if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    Bit32u capacity = BX_SELECTED_DRIVE(channel).cdrom.max_lba;
                    controller->buffer[0] = (capacity >> 24) & 0xff;
                    controller->buffer[1] = (capacity >> 16) & 0xff;
                    controller->buffer[2] = (capacity >> 8) & 0xff;
                    controller->buffer[3] = (capacity >> 0) & 0xff;
                    controller->buffer[4] = (2048 >> 24) & 0xff;
                    controller->buffer[5] = (2048 >> 16) & 0xff;
                    controller->buffer[6] = (2048 >> 8) & 0xff;
                    controller->buffer[7] = (2048 >> 0) & 0xff;
                    ready_to_send_atapi(channel);
                  } else {
                    atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                    raise_interrupt(channel);
                  }
                }
                break;

              case 0xbe: // read cd
                {
                  if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    Bit32u lba = read_32bit(controller->buffer + 2);
                    Bit32u transfer_length = controller->buffer[8] |
                                             (controller->buffer[7] << 8) |
                                             (controller->buffer[6] << 16);
                    Bit8u transfer_req = controller->buffer[9];
                    if (transfer_length == 0) {
                      atapi_cmd_nop(controller);
                      raise_interrupt(channel);
                      break;
                    }
                    switch (transfer_req & 0xf8) {
                      case 0x00:
                        atapi_cmd_nop(controller);
                        raise_interrupt(channel);
                        break;
                      case 0xf8:
                        controller->buffer_size = 2352;
                      case 0x10:
                        {
                          init_send_atapi_command(channel, atapi_command,
                                                  transfer_length * controller->buffer_size,
                                                  transfer_length * controller->buffer_size, 1);
                          BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks = transfer_length;
                          BX_SELECTED_DRIVE(channel).cdrom.next_lba = lba;
                          start_seek(channel);
                        }
                        break;
                      default:
                        BX_ERROR(("Read CD: unknown format"));
                        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                        raise_interrupt(channel);
                    }
                  } else {
                    atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                    raise_interrupt(channel);
                  }
                }
                break;

              case 0x43: // read toc
                if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                  bx_bool msf = (controller->buffer[1] >> 1) & 1;
                  Bit8u starting_track = controller->buffer[6];
                  int toc_length = 0;
                  Bit16u alloc_length = read_16bit(controller->buffer + 7);
                  Bit8u format = (controller->buffer[9] >> 6);
                  switch (format) {
// Win32:  I just read the TOC using Win32's IOCTRL functions (Ben)
#if BX_SUPPORT_CDROM && defined(WIN32)
                    case 2:
                    case 3:
                    case 4:
                      if (msf != 1)
                        BX_ERROR(("READ_TOC_EX: msf not set for format %i", format));
                    case 0:
                    case 1:
                    case 5:
                      if (!(BX_SELECTED_DRIVE(channel).cdrom.cd->read_toc(controller->buffer,
                                                                          &toc_length, msf, starting_track, format))) {
                        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                        raise_interrupt(channel);
                      } else {
                        init_send_atapi_command(channel, atapi_command, toc_length, alloc_length);
                        ready_to_send_atapi(channel);
                      }
                      break;
#else
                    case 0:
                    case 1:
                    case 2:
                      if (!(BX_SELECTED_DRIVE(channel).cdrom.cd->read_toc(controller->buffer,
                                                                          &toc_length, msf, starting_track, format))) {
                        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                        raise_interrupt(channel);
                      } else {
                        init_send_atapi_command(channel, atapi_command, toc_length, alloc_length);
                        ready_to_send_atapi(channel);
                      }
                      break;
#endif
                    default:
                      BX_ERROR(("(READ TOC) format %d not supported", format));
                      atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                      raise_interrupt(channel);
                  }
                } else {
                  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                  raise_interrupt(channel);
                }
                break;

              case 0x28: // read (10)
              case 0xa8: // read (12)
                {
                  Bit32s transfer_length;

                  if (atapi_command == 0x28)
                    transfer_length = read_16bit(controller->buffer + 7);
                  else
                    transfer_length = read_32bit(controller->buffer + 6);

                  Bit32u lba = read_32bit(controller->buffer + 2);

                  if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                    raise_interrupt(channel);
                    break;
                  }
                  if (lba > BX_SELECTED_DRIVE(channel).cdrom.max_lba) {
                    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR, 1);
                    raise_interrupt(channel);
                    break;
                  }

                  // Ben: see comment below
                  if ((lba + transfer_length - 1) > BX_SELECTED_DRIVE(channel).cdrom.max_lba) {
                    transfer_length = (BX_SELECTED_DRIVE(channel).cdrom.max_lba - lba + 1);
                  }
                  if (transfer_length <= 0) {
                    atapi_cmd_nop(controller);
                    raise_interrupt(channel);
                    BX_INFO(("READ(%d) with transfer length <= 0, ok (%i)", atapi_command==0x28?10:12, transfer_length));
                    break;
                  }

/* Ben: I commented this out and added the three lines above.  I am not sure this is the correct thing
        to do, but it seems to work.
        FIXME: I think that if the transfer_length is more than we can transfer, we should return
        some sort of flag/error/bitrep stating so.  I haven't read the atapi specs enough to know
        what needs to be done though.

                  if ((lba + transfer_length - 1) > BX_SELECTED_DRIVE(channel).cdrom.max_lba) {
                    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR, 1);
                    raise_interrupt(channel);
                    break;
                  }
*/
                  BX_DEBUG(("cdrom: READ (%d) LBA=%d LEN=%d DMA=%d", atapi_command==0x28?10:12,
                            lba, transfer_length, controller->packet_dma));

                  // handle command
                  init_send_atapi_command(channel, atapi_command, transfer_length * 2048,
                                          transfer_length * 2048, 1);
                  BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks = transfer_length;
                  BX_SELECTED_DRIVE(channel).cdrom.next_lba = lba;
                  start_seek(channel);
                }
                break;

              case 0x2b: // seek
                {
                  Bit32u lba = read_32bit(controller->buffer + 2);
                  if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                    raise_interrupt(channel);
                    break;
                  }

                  if (lba > BX_SELECTED_DRIVE(channel).cdrom.max_lba) {
                    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR, 1);
                    raise_interrupt(channel);
                    break;
                  }
                  BX_SELECTED_DRIVE(channel).cdrom.cd->seek(lba);
                  BX_SELECTED_DRIVE(channel).cdrom.curr_lba = lba;
                  atapi_cmd_nop(controller);
                  raise_interrupt(channel);
                  // TODO: DSC bit must be cleared here and set after completion
                }
                break;

              case 0x1e: // prevent/allow medium removal
                if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
                  BX_SELECTED_DRIVE(channel).cdrom.locked = controller->buffer[4] & 1;
                  atapi_cmd_nop(controller);
                } else {
                  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                }
                raise_interrupt(channel);
                break;

              case 0x42: // read sub-channel
                {
                  bx_bool msf = get_packet_field(controller,1, 1, 1);
                  bx_bool sub_q = get_packet_field(controller,2, 6, 1);
                  Bit8u data_format = get_packet_byte(controller, 3);
                  Bit8u track_number = get_packet_byte(controller, 6);
                  Bit16u alloc_length = get_packet_word(controller, 7);
                  int ret_len = 4; // header size
                  UNUSED(msf);
                  UNUSED(track_number);

                  if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
                    atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT, 1);
                    raise_interrupt(channel);
                  } else {
                    controller->buffer[0] = 0;
                    controller->buffer[1] = 0; // audio not supported
                    controller->buffer[2] = 0;
                    controller->buffer[3] = 0;

                    if (sub_q) { // !sub_q == header only
                      if ((data_format == 2) || (data_format == 3)) { // UPC or ISRC
                        ret_len = 24;
                        controller->buffer[4] = data_format;
                        if (data_format == 3) {
                          controller->buffer[5] = 0x14;
                          controller->buffer[6] = 1;
                        }
                        controller->buffer[8] = 0; // no UPC, no ISRC
                      } else {
                        BX_ERROR(("Read sub-channel with SubQ not implemented (format=%d)", data_format));
                        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                        raise_interrupt(channel);
                        break;
                      }
                    }
                    init_send_atapi_command(channel, atapi_command, ret_len, alloc_length);
                    ready_to_send_atapi(channel);
                  }
                }
                break;

              case 0x51: // read disc info
                // no-op to keep the Linux CD-ROM driver happy
                atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET, 1);
                raise_interrupt(channel);
                break;

              case 0x55: // mode select
              case 0xa6: // load/unload cd
              case 0x4b: // pause/resume
              case 0x45: // play audio
              case 0x47: // play audio msf
              case 0xbc: // play cd
              case 0xb9: // read cd msf
              case 0x44: // read header
              case 0xba: // scan
              case 0xbb: // set cd speed
              case 0x4e: // stop play/scan
              case 0x46: // get configuration
              case 0x4a: // get event status notification
                BX_DEBUG(("ATAPI command 0x%x not implemented yet", atapi_command));
                atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_ILLEGAL_OPCODE, 0);
                raise_interrupt(channel);
                break;

              default:
                BX_ERROR(("Unknown ATAPI command 0x%x (%d)", atapi_command, atapi_command));
                atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_ILLEGAL_OPCODE, 1);
                raise_interrupt(channel);
                break;
            }
          }
          break;

        default:
          BX_PANIC(("IO write(0x%04x): current command is %02xh", address,
            (unsigned) controller->current_command));
      }
      break;

    case 0x01: // hard disk write precompensation 0x1f1
      WRITE_FEATURES(channel,value);
      if (value == 0xff)
        BX_DEBUG(("no precompensation {%s}", BX_SELECTED_TYPE_STRING(channel)));
      else
        BX_DEBUG(("precompensation value %02x {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x02: // hard disk sector count 0x1f2
      WRITE_SECTOR_COUNT(channel,value);
      BX_DEBUG(("sector count = %u {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x03: // hard disk sector number 0x1f3
      WRITE_SECTOR_NUMBER(channel,value);
      BX_DEBUG(("sector number = %u {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x04: // hard disk cylinder low 0x1f4
      WRITE_CYLINDER_LOW(channel,value);
      BX_DEBUG(("cylinder low = %02xh {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x05: // hard disk cylinder high 0x1f5
      WRITE_CYLINDER_HIGH(channel,value);
      BX_DEBUG(("cylinder high = %02xh {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x06: // hard disk drive and head register 0x1f6
      // b7 Extended data field for ECC
      // b6/b5: Used to be sector size.  00=256,01=512,10=1024,11=128
      //   Since 512 was always used, bit 6 was taken to mean LBA mode:
      //     b6 1=LBA mode, 0=CHS mode
      //     b5 1
      // b4: DRV
      // b3..0 HD3..HD0
      {
        if ((value & 0xa0) != 0xa0) // 1x1xxxxx
          BX_DEBUG(("IO write 0x%04x (%02x): not 1x1xxxxxb", address, (unsigned) value));
        Bit32u drvsel = BX_HD_THIS channels[channel].drive_select = (value >> 4) & 0x01;
        WRITE_HEAD_NO(channel,value & 0xf);
        if (!controller->lba_mode && ((value >> 6) & 1) == 1)
          BX_DEBUG(("enabling LBA mode"));
        WRITE_LBA_MODE(channel,(value >> 6) & 1);
        if (!BX_SELECTED_IS_PRESENT(channel)) {
          BX_DEBUG(("ata%d: device set to %d which does not exist", channel, drvsel));
        }
      }
      break;

    case 0x07: // hard disk command 0x1f7
      // (mch) Writes to the command register with drive_select != 0
      // are ignored if no secondary device is present
      if ((BX_SLAVE_SELECTED(channel)) && (!BX_SLAVE_IS_PRESENT(channel)))
        break;
      // Writes to the command register clear the IRQ
      DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);

      if (controller->status.busy) {
        BX_ERROR(("ata%d: command 0x%02x sent, controller BSY bit set", channel, value));
        break;
      }
      if ((value & 0xf0) == 0x10)
        value = 0x10;
      controller->status.err = 0;
      switch (value) {

        case 0x10: // CALIBRATE DRIVE
          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("ata%d-%d: calibrate drive issued to non-disk",
              channel, BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }

          if (!BX_SELECTED_IS_PRESENT(channel)) {
            controller->error_register = 0x02; // Track 0 not found
            controller->status.busy = 0;
            controller->status.drive_ready = 1;
            controller->status.seek_complete = 0;
            controller->status.drq = 0;
            controller->status.err = 1;
            raise_interrupt(channel);
            BX_INFO(("calibrate drive: disk ata%d-%d not present", channel, BX_SLAVE_SELECTED(channel)));
            break;
          }

          /* move head to cylinder 0, issue IRQ */
          controller->error_register = 0;
          controller->cylinder_no = 0;
          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.seek_complete = 1;
          controller->status.drq = 0;
          raise_interrupt(channel);
          break;

        case 0x24: // READ SECTORS EXT
        case 0x29: // READ MULTIPLE EXT
          lba48 = 1;
        case 0x20: // READ SECTORS, with retries
        case 0x21: // READ SECTORS, without retries
        case 0xC4: // READ MULTIPLE SECTORS
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("ata%d-%d: read sectors issued to non-disk",
              channel, BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }
          // Lose98 accesses 0/0/0 in CHS mode
          if (!controller->lba_mode &&
              !controller->head_no &&
              !controller->cylinder_no &&
              !controller->sector_no) {
            BX_INFO(("ata%d-%d: : read from 0/0/0, aborting command",
              channel, BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }

          lba48_transform(controller, lba48);
          if ((value == 0xC4) || (value == 0x29)) {
            if (controller->multiple_sectors == 0) {
              command_aborted(channel, value);
              break;
            }
            if (controller->num_sectors > controller->multiple_sectors) {
              controller->buffer_size = controller->multiple_sectors * 512;
            } else {
              controller->buffer_size = controller->num_sectors * 512;
            }
          } else {
            controller->buffer_size = 512;
          }
          if (!calculate_logical_address(channel, &logical_sector)) {
            command_aborted(channel, value);
            break;
          }
          BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
          controller->current_command = value;
          controller->error_register = 0;
          controller->status.busy  = 1;
          controller->status.drive_ready = 1;
          controller->status.seek_complete = 0;
          controller->status.drq   = 0;
          controller->status.corrected_data = 0;
          controller->buffer_index = 0;
          start_seek(channel);
          if (!ide_read_sector(channel, controller->buffer,
                                  controller->buffer_size)) {
            bx_pc_system.deactivate_timer(
              BX_SELECTED_DRIVE(channel).seek_timer_index);
            command_aborted(channel, value);
          }
          break;

        case 0x34: // WRITE SECTORS EXT
        case 0x39: // WRITE MULTIPLE EXT
          lba48 = 1;
        case 0x30: // WRITE SECTORS, with retries
        case 0xC5: // WRITE MULTIPLE SECTORS
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("ata%d-%d: write sectors issued to non-disk",
              channel, BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }
          lba48_transform(controller, lba48);
          if ((value == 0xC5) || (value ==0x39)) {
            if (controller->multiple_sectors == 0) {
              command_aborted(channel, value);
              break;
            }
            if (controller->num_sectors > controller->multiple_sectors) {
              controller->buffer_size = controller->multiple_sectors * 512;
            } else {
              controller->buffer_size = controller->num_sectors * 512;
            }
          } else {
            controller->buffer_size = 512;
          }
          if (!calculate_logical_address(channel, &logical_sector)) {
            command_aborted(channel, value);
            break;
          }
          BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
          controller->current_command = value;

          // implicit seek done :^)
          controller->error_register = 0;
          controller->status.busy = 0;
          // controller->status.drive_ready = 1;
          controller->status.seek_complete = 1;
          controller->status.drq = 1;
          controller->buffer_index = 0;
          break;

        case 0x90: // EXECUTE DEVICE DIAGNOSTIC
          set_signature(channel, BX_SLAVE_SELECTED(channel));
          controller->error_register = 0x01;
          controller->status.drq = 0;
          raise_interrupt(channel);
          break;

        case 0x91: // INITIALIZE DRIVE PARAMETERS
          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("ata%d-%d: initialize drive parameters issued to non-disk",
              channel, BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }
          // sets logical geometry of specified drive
          BX_DEBUG(("ata%d-%d: init drive params: sec=%u, drive sel=%u, head=%u",
            channel, BX_SLAVE_SELECTED(channel),
            (unsigned) controller->sector_count,
            (unsigned) BX_HD_THIS channels[channel].drive_select,
            (unsigned) controller->head_no));
          if (!BX_SELECTED_IS_PRESENT(channel)) {
            BX_PANIC(("init drive params: disk ata%d-%d not present", channel, BX_SLAVE_SELECTED(channel)));
            //controller->error_register = 0x12;
            controller->status.busy = 0;
            controller->status.drive_ready = 1;
            controller->status.drq = 0;
            raise_interrupt(channel);
            break;
          }
          if (controller->sector_count != BX_SELECTED_DRIVE(channel).hdimage->spt) {
            BX_ERROR(("ata%d-%d: init drive params: logical sector count %d not supported", channel, BX_SLAVE_SELECTED(channel),
              controller->sector_count));
            command_aborted(channel, value);
            break;
          }
          if (controller->head_no == 0) {
            // Linux 2.6.x kernels use this value and don't like aborting here
            BX_ERROR(("ata%d-%d: init drive params: max. logical head number 0 not supported", channel, BX_SLAVE_SELECTED(channel)));
          } else if (controller->head_no != (BX_SELECTED_DRIVE(channel).hdimage->heads-1)) {
            BX_ERROR(("ata%d-%d: init drive params: max. logical head number %d not supported", channel, BX_SLAVE_SELECTED(channel),
              controller->head_no));
            command_aborted(channel, value);
            break;
          }
          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.drq = 0;
          raise_interrupt(channel);
          break;

        case 0xec: // IDENTIFY DEVICE
          {
            BX_DEBUG(("Drive ID Command issued : 0xec "));

            if (!BX_SELECTED_IS_PRESENT(channel)) {
              BX_INFO(("disk ata%d-%d not present, aborting",channel,BX_SLAVE_SELECTED(channel)));
              command_aborted(channel, value);
              break;
            }
            if (BX_SELECTED_IS_CD(channel)) {
              set_signature(channel, BX_SLAVE_SELECTED(channel));
              command_aborted(channel, 0xec);
            } else {
              controller->current_command = value;
              controller->error_register = 0;

              // See ATA/ATAPI-4, 8.12
              controller->status.busy  = 0;
              controller->status.drive_ready = 1;
              controller->status.write_fault = 0;
              controller->status.drq   = 1;

              controller->status.seek_complete = 1;
              controller->status.corrected_data = 0;

              controller->buffer_index = 0;
              if (!BX_SELECTED_DRIVE(channel).identify_set) {
                identify_drive(channel);
              }
              // now convert the id_drive array (native 256 word format) to
              // the controller buffer (512 bytes)
              for (i=0; i<=255; i++) {
                Bit16u temp16 = BX_SELECTED_DRIVE(channel).id_drive[i];
                controller->buffer[i*2] = temp16 & 0x00ff;
                controller->buffer[i*2+1] = temp16 >> 8;
              }
              raise_interrupt(channel);
            }
          }
          break;

        case 0xef: // SET FEATURES
          switch(controller->features) {
            case 0x03: // Set Transfer Mode
              {
                Bit8u type = (controller->sector_count >> 3);
                Bit8u mode = controller->sector_count & 0x07;
                switch (type) {
                  case 0x00: // PIO default
                  case 0x01: // PIO mode
                    BX_INFO(("ata%d-%d: set transfer mode to PIO", channel,
                             BX_SLAVE_SELECTED(channel)));
                    controller->mdma_mode = 0x00;
                    controller->udma_mode = 0x00;
                    controller->status.drive_ready = 1;
                    controller->status.seek_complete = 1;
                    raise_interrupt(channel);
                    break;
                  case 0x04: // MDMA mode
                    BX_INFO(("ata%d-%d: set transfer mode to MDMA%d", channel,
                             BX_SLAVE_SELECTED(channel), mode));
                    controller->mdma_mode = (1 << mode);
                    controller->udma_mode = 0x00;
                    controller->status.drive_ready = 1;
                    controller->status.seek_complete = 1;
                    raise_interrupt(channel);
                    break;
                  case 0x08: // UDMA mode
                    controller->mdma_mode = 0x00;
                    controller->udma_mode = (1 << mode);
                    controller->status.drive_ready = 1;
                    controller->status.seek_complete = 1;
                    raise_interrupt(channel);
                    break;
                  default:
                    BX_ERROR(("ata%d-%d: unknown transfer mode type 0x%02x", channel,
                             BX_SLAVE_SELECTED(channel), type));
                    command_aborted(channel, value);
                }
                BX_SELECTED_DRIVE(channel).identify_set = 0;
              }
              break;

            case 0x02: // Enable and
            case 0x82: //  Disable write cache.
            case 0xAA: // Enable and
            case 0x55: //  Disable look-ahead cache.
            case 0xCC: // Enable and
            case 0x66: //  Disable reverting to power-on default
              BX_INFO(("ata%d-%d: SET FEATURES subcommand 0x%02x not supported, but returning success",
                channel,BX_SLAVE_SELECTED(channel),(unsigned) controller->features));
              controller->status.drive_ready = 1;
              controller->status.seek_complete = 1;
              raise_interrupt(channel);
              break;

            default:
              BX_ERROR(("ata%d-%d: SET FEATURES with unknown subcommand: 0x%02x",
                channel,BX_SLAVE_SELECTED(channel),(unsigned) controller->features));
              command_aborted(channel, value);
          }
          break;

        case 0x42: // READ VERIFY SECTORS EXT
          lba48 = 1;
        case 0x40: // READ VERIFY SECTORS
        case 0x41: // READ VERIFY SECTORS NO RETRY
          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("ata%d-%d: read verify issued to non-disk",
              channel,BX_SLAVE_SELECTED(channel)));
            command_aborted(channel, value);
            break;
          }
          lba48_transform(controller, lba48);
          BX_INFO(("ata%d-%d: verify command : 0x%02x !", channel,BX_SLAVE_SELECTED(channel), value));
          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.drq = 0;
          raise_interrupt(channel);
          break;

        case 0xc6: // SET MULTIPLE MODE
          if (!BX_SELECTED_IS_HD(channel)) {
            BX_INFO(("set multiple mode issued to non-disk"));
            command_aborted(channel, value);
          } else if ((controller->sector_count > MAX_MULTIPLE_SECTORS) ||
              ((controller->sector_count & (controller->sector_count - 1)) != 0) ||
               (controller->sector_count == 0)) {
            command_aborted(channel, value);
          } else {
            BX_DEBUG(("set multiple mode: sectors=%d", controller->sector_count));
            controller->multiple_sectors = controller->sector_count;
            controller->status.busy = 0;
            controller->status.drive_ready = 1;
            controller->status.write_fault = 0;
            controller->status.drq = 0;
            raise_interrupt(channel);
          }
          break;

        // ATAPI commands
        case 0xa1: // IDENTIFY PACKET DEVICE
          {
            if (BX_SELECTED_IS_CD(channel)) {
              controller->current_command = value;
              controller->error_register = 0;

              controller->status.busy = 0;
              controller->status.drive_ready = 1;
              controller->status.write_fault = 0;
              controller->status.drq   = 1;

              controller->status.seek_complete = 1;
              controller->status.corrected_data = 0;

              controller->buffer_index = 0;
              if (!BX_SELECTED_DRIVE(channel).identify_set) {
                identify_ATAPI_drive(channel);
              }
              // now convert the id_drive array (native 256 word format) to
              // the controller buffer (512 bytes)
              for (i = 0; i <= 255; i++) {
                Bit16u temp16 = BX_SELECTED_DRIVE(channel).id_drive[i];
                controller->buffer[i*2] = temp16 & 0x00ff;
                controller->buffer[i*2+1] = temp16 >> 8;
              }
              raise_interrupt(channel);
            } else {
              command_aborted(channel, 0xa1);
            }
          }
          break;

        case 0x08: // DEVICE RESET (atapi)
          if (BX_SELECTED_IS_CD(channel)) {
            set_signature(channel, BX_SLAVE_SELECTED(channel));

            controller->status.busy = 1;
            controller->error_register &= ~(1 << 7);

            controller->status.write_fault = 0;
            controller->status.drq = 0;
            controller->status.corrected_data = 0;

            controller->status.busy = 0;
          } else {
            BX_DEBUG(("ATAPI Device Reset on non-cd device"));
            command_aborted(channel, 0x08);
          }
          break;

        case 0xa0: // SEND PACKET (atapi)
          if (BX_SELECTED_IS_CD(channel)) {
            // PACKET
            controller->packet_dma = (controller->features & 1);
            if (controller->features & (1 << 1)) {
              BX_ERROR(("PACKET-overlapped not supported"));
              command_aborted (channel, 0xa0);
            } else {
              // We're already ready!
              controller->sector_count = 1;
              controller->status.busy = 0;
              controller->status.write_fault = 0;
              // serv bit??
              controller->status.drq = 1;

              // NOTE: no interrupt here
              controller->current_command = value;
              controller->buffer_index = 0;
            }
          } else {
            command_aborted (channel, 0xa0);
          }
          break;

        case 0xa2: // SERVICE (atapi), optional
          if (BX_SELECTED_IS_CD(channel)) {
            BX_PANIC(("ATAPI SERVICE not implemented"));
            command_aborted(channel, 0xa2);
          } else {
            command_aborted(channel, 0xa2);
          }
          break;

        // power management & flush cache stubs
        case 0xE0: // STANDBY NOW
        case 0xE1: // IDLE IMMEDIATE
        case 0xE7: // FLUSH CACHE
        case 0xEA: // FLUSH CACHE EXT
          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.write_fault = 0;
          controller->status.drq = 0;
          raise_interrupt(channel);
          break;

        case 0xe5: // CHECK POWER MODE
          controller->status.busy = 0;
          controller->status.drive_ready = 1;
          controller->status.write_fault = 0;
          controller->status.drq = 0;
          controller->sector_count = 0xff; // Active or Idle mode
          raise_interrupt(channel);
          break;

        case 0x70:  // SEEK (cgs)
          if (BX_SELECTED_IS_HD(channel)) {
            BX_DEBUG(("write cmd 0x70 (SEEK) executing"));
            if (!calculate_logical_address(channel, &logical_sector)) {
              command_aborted(channel, value);
              break;
            }
            BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
            controller->current_command = value;
            controller->error_register = 0;
            controller->status.busy  = 1;
            controller->status.drive_ready = 1;
            controller->status.seek_complete = 0;
            controller->status.drq   = 0;
            controller->status.corrected_data = 0;
            start_seek(channel);
          } else {
            BX_INFO(("write cmd 0x70 (SEEK) not supported for non-disk"));
            command_aborted(channel, 0x70);
          }
          break;

        case 0x25: // READ DMA EXT
          lba48 = 1;
        case 0xC8: // READ DMA
          if (BX_SELECTED_IS_HD(channel) && BX_HD_THIS bmdma_present()) {
            lba48_transform(controller, lba48);
            if (!calculate_logical_address(channel, &logical_sector)) {
              command_aborted(channel, value);
              break;
            }
            BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
            controller->current_command = value;
            controller->error_register = 0;
            controller->status.busy  = 1;
            controller->status.drive_ready = 1;
            controller->status.seek_complete = 0;
            controller->status.drq   = 0;
            controller->status.corrected_data = 0;
            start_seek(channel);
          } else {
            BX_ERROR(("write cmd 0x%02x (READ DMA) not supported", value));
            command_aborted(channel, value);
          }
          break;

        case 0x35: // WRITE DMA EXT
          lba48 = 1;
        case 0xCA: // WRITE DMA
          if (BX_SELECTED_IS_HD(channel) && BX_HD_THIS bmdma_present()) {
            lba48_transform(controller, lba48);
            if (!calculate_logical_address(channel, &logical_sector)) {
              command_aborted(channel, value);
              break;
            }
            BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
            controller->status.drive_ready = 1;
            controller->status.seek_complete = 1;
            controller->status.drq   = 1;
            controller->current_command = value;
          } else {
            BX_ERROR(("write cmd 0x%02x (WRITE DMA) not supported", value));
            command_aborted(channel, value);
          }
          break;
        case 0x27: // READ NATIVE MAX ADDRESS EXT
          lba48 = 1;
        case 0xF8: // READ NATIVE MAX ADDRESS
          if (BX_SELECTED_IS_HD(channel)) {
            lba48_transform(controller, lba48);
            Bit64s max_sector = BX_SELECTED_DRIVE(channel).hdimage->hd_size / 512 - 1;
            if (controller->lba_mode) {
              if (!controller->lba48) {
                controller->head_no = (Bit8u)((max_sector >> 24) & 0xf);
                controller->cylinder_no = (Bit16u)((max_sector >> 8) & 0xffff);
                controller->sector_no = (Bit8u)((max_sector) & 0xff);
              } else {
                controller->hob.hcyl = (Bit8u)((max_sector >> 40) & 0xff);
                controller->hob.lcyl = (Bit8u)((max_sector >> 32) & 0xff);
                controller->hob.sector = (Bit8u)((max_sector >> 24) & 0xff);
                controller->cylinder_no = (Bit16u)((max_sector >> 8) & 0xffff);
                controller->sector_no = (Bit8u)((max_sector) & 0xff);
              }
              controller->status.drive_ready = 1;
              controller->status.seek_complete = 1;
              raise_interrupt(channel);
            } else {
              command_aborted(channel, value);
            }
          } else {
            command_aborted(channel, value);
          }
          break;

        // List all the write operations that are defined in the ATA/ATAPI spec
        // that we don't support.  Commands that are listed here will cause a
        // BX_ERROR, which is non-fatal, and the command will be aborted.
        case 0x22: BX_ERROR(("write cmd 0x22 (READ LONG) not supported")); command_aborted(channel, 0x22); break;
        case 0x23: BX_ERROR(("write cmd 0x23 (READ LONG NO RETRY) not supported")); command_aborted(channel, 0x23); break;
        case 0x26: BX_ERROR(("write cmd 0x26 (READ DMA QUEUED EXT) not supported"));command_aborted(channel, 0x26); break;
        case 0x2A: BX_ERROR(("write cmd 0x2A (READ STREAM DMA) not supported"));command_aborted(channel, 0x2A); break;
        case 0x2B: BX_ERROR(("write cmd 0x2B (READ STREAM PIO) not supported"));command_aborted(channel, 0x2B); break;
        case 0x2F: BX_ERROR(("write cmd 0x2F (READ LOG EXT) not supported"));command_aborted(channel, 0x2F); break;
        case 0x31: BX_ERROR(("write cmd 0x31 (WRITE SECTORS NO RETRY) not supported")); command_aborted(channel, 0x31); break;
        case 0x32: BX_ERROR(("write cmd 0x32 (WRITE LONG) not supported")); command_aborted(channel, 0x32); break;
        case 0x33: BX_ERROR(("write cmd 0x33 (WRITE LONG NO RETRY) not supported")); command_aborted(channel, 0x33); break;
        case 0x36: BX_ERROR(("write cmd 0x36 (WRITE DMA QUEUED EXT) not supported"));command_aborted(channel, 0x36); break;
        case 0x37: BX_ERROR(("write cmd 0x37 (SET MAX ADDRESS EXT) not supported"));command_aborted(channel, 0x37); break;
        case 0x38: BX_ERROR(("write cmd 0x38 (CFA WRITE SECTORS W/OUT ERASE) not supported"));command_aborted(channel, 0x38); break;
        case 0x3A: BX_ERROR(("write cmd 0x3A (WRITE STREAM DMA) not supported"));command_aborted(channel, 0x3A); break;
        case 0x3B: BX_ERROR(("write cmd 0x3B (WRITE STREAM PIO) not supported"));command_aborted(channel, 0x3B); break;
        case 0x3F: BX_ERROR(("write cmd 0x3F (WRITE LOG EXT) not supported"));command_aborted(channel, 0x3F); break;
        case 0x50: BX_ERROR(("write cmd 0x50 (FORMAT TRACK) not supported")); command_aborted(channel, 0x50); break;
        case 0x51: BX_ERROR(("write cmd 0x51 (CONFIGURE STREAM) not supported"));command_aborted(channel, 0x51); break;
        case 0x87: BX_ERROR(("write cmd 0x87 (CFA TRANSLATE SECTOR) not supported"));command_aborted(channel, 0x87); break;
        case 0x92: BX_ERROR(("write cmd 0x92 (DOWNLOAD MICROCODE) not supported"));command_aborted(channel, 0x92); break;
        case 0x94: BX_ERROR(("write cmd 0x94 (STANDBY IMMEDIATE) not supported")); command_aborted(channel, 0x94); break;
        case 0x95: BX_ERROR(("write cmd 0x95 (IDLE IMMEDIATE) not supported")); command_aborted(channel, 0x95); break;
        case 0x96: BX_ERROR(("write cmd 0x96 (STANDBY) not supported")); command_aborted(channel, 0x96); break;
        case 0x97: BX_ERROR(("write cmd 0x97 (IDLE) not supported")); command_aborted(channel, 0x97); break;
        case 0x98: BX_ERROR(("write cmd 0x98 (CHECK POWER MODE) not supported")); command_aborted(channel, 0x98); break;
        case 0x99: BX_ERROR(("write cmd 0x99 (SLEEP) not supported")); command_aborted(channel, 0x99); break;
        case 0xB0: BX_ERROR(("write cmd 0xB0 (SMART commands) not supported"));command_aborted(channel, 0xB0); break;
        case 0xB1: BX_ERROR(("write cmd 0xB1 (DEVICE CONFIGURATION commands) not supported"));command_aborted(channel, 0xB1); break;
        case 0xC0: BX_ERROR(("write cmd 0xC0 (CFA ERASE SECTORS) not supported"));command_aborted(channel, 0xC0); break;
        case 0xC7: BX_ERROR(("write cmd 0xC7 (READ DMA QUEUED) not supported"));command_aborted(channel, 0xC7); break;
        case 0xC9: BX_ERROR(("write cmd 0xC9 (READ DMA NO RETRY) not supported")); command_aborted(channel, 0xC9); break;
        case 0xCC: BX_ERROR(("write cmd 0xCC (WRITE DMA QUEUED) not supported"));command_aborted(channel, 0xCC); break;
        case 0xCD: BX_ERROR(("write cmd 0xCD (CFA WRITE MULTIPLE W/OUT ERASE) not supported"));command_aborted(channel, 0xCD); break;
        case 0xD1: BX_ERROR(("write cmd 0xD1 (CHECK MEDIA CARD TYPE) not supported"));command_aborted(channel, 0xD1); break;
        case 0xDA: BX_ERROR(("write cmd 0xDA (GET MEDIA STATUS) not supported"));command_aborted(channel, 0xDA); break;
        case 0xDE: BX_ERROR(("write cmd 0xDE (MEDIA LOCK) not supported"));command_aborted(channel, 0xDE); break;
        case 0xDF: BX_ERROR(("write cmd 0xDF (MEDIA UNLOCK) not supported"));command_aborted(channel, 0xDF); break;
        case 0xE2: BX_ERROR(("write cmd 0xE2 (STANDBY) not supported"));command_aborted(channel, 0xE2); break;
        case 0xE3: BX_ERROR(("write cmd 0xE3 (IDLE) not supported"));command_aborted(channel, 0xE3); break;
        case 0xE4: BX_ERROR(("write cmd 0xE4 (READ BUFFER) not supported"));command_aborted(channel, 0xE4); break;
        case 0xE6: BX_ERROR(("write cmd 0xE6 (SLEEP) not supported"));command_aborted(channel, 0xE6); break;
        case 0xE8: BX_ERROR(("write cmd 0xE8 (WRITE BUFFER) not supported"));command_aborted(channel, 0xE8); break;
        case 0xED: BX_ERROR(("write cmd 0xED (MEDIA EJECT) not supported"));command_aborted(channel, 0xED); break;
        case 0xF1: BX_ERROR(("write cmd 0xF1 (SECURITY SET PASSWORD) not supported"));command_aborted(channel, 0xF1); break;
        case 0xF2: BX_ERROR(("write cmd 0xF2 (SECURITY UNLOCK) not supported"));command_aborted(channel, 0xF2); break;
        case 0xF3: BX_ERROR(("write cmd 0xF3 (SECURITY ERASE PREPARE) not supported"));command_aborted(channel, 0xF3); break;
        case 0xF4: BX_ERROR(("write cmd 0xF4 (SECURITY ERASE UNIT) not supported"));command_aborted(channel, 0xF4); break;
        case 0xF5: BX_ERROR(("write cmd 0xF5 (SECURITY FREEZE LOCK) not supported"));command_aborted(channel, 0xF5); break;
        case 0xF6: BX_ERROR(("write cmd 0xF6 (SECURITY DISABLE PASSWORD) not supported"));command_aborted(channel, 0xF6); break;
        case 0xF9: BX_ERROR(("write cmd 0xF9 (SET MAX ADDRESS) not supported"));command_aborted(channel, 0xF9); break;

        default:
          BX_ERROR(("IO write to 0x%04x: unknown command 0x%02x", address, value));
          command_aborted(channel, value);
      }
      break;

    case 0x16: // hard disk adapter control 0x3f6
               // (mch) Even if device 1 was selected, a write to this register
               // goes to device 0 (if device 1 is absent)

      prev_control_reset = controller->control.reset;
      BX_HD_THIS channels[channel].drives[0].controller.control.reset       = value & 0x04;
      BX_HD_THIS channels[channel].drives[1].controller.control.reset       = value & 0x04;
      BX_HD_THIS channels[channel].drives[0].controller.control.disable_irq = value & 0x02;
      BX_HD_THIS channels[channel].drives[1].controller.control.disable_irq = value & 0x02;

      BX_DEBUG(("ata%d: adapter control reg: reset controller = %d", channel,
        (unsigned) (controller->control.reset) ? 1 : 0));
      BX_DEBUG(("ata%d: adapter control reg: disable irq = %d", channel,
        (unsigned) (controller->control.disable_irq) ? 1 : 0));

      if (!prev_control_reset && controller->control.reset) {
        // transition from 0 to 1 causes all drives to reset
        BX_DEBUG(("Enter RESET mode"));

        // (mch) Set BSY, drive not ready
        for (int id = 0; id < 2; id++) {
          BX_CONTROLLER(channel,id).status.busy           = 1;
          BX_CONTROLLER(channel,id).status.drive_ready    = 0;
          BX_CONTROLLER(channel,id).reset_in_progress     = 1;

          BX_CONTROLLER(channel,id).status.write_fault    = 0;
          BX_CONTROLLER(channel,id).status.seek_complete  = 1;
          BX_CONTROLLER(channel,id).status.drq            = 0;
          BX_CONTROLLER(channel,id).status.corrected_data = 0;
          BX_CONTROLLER(channel,id).status.err            = 0;

          BX_CONTROLLER(channel,id).error_register = 0x01; // diagnostic code: no error

          BX_CONTROLLER(channel,id).current_command = 0x00;
          BX_CONTROLLER(channel,id).buffer_index = 0;

          BX_CONTROLLER(channel,id).multiple_sectors  = 0;
          BX_CONTROLLER(channel,id).lba_mode          = 0;

          BX_CONTROLLER(channel,id).control.disable_irq = 0;
          DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
        }
      } else if (controller->reset_in_progress &&
                !controller->control.reset) {
        // Clear BSY and DRDY
        BX_DEBUG(("Reset complete {%s}", BX_SELECTED_TYPE_STRING(channel)));
        for (int id = 0; id < 2; id++) {
          BX_CONTROLLER(channel,id).status.busy           = 0;
          BX_CONTROLLER(channel,id).status.drive_ready    = 1;
          BX_CONTROLLER(channel,id).reset_in_progress     = 0;

          set_signature(channel, id);
        }
      }
      BX_DEBUG(("ata%d: adapter control reg: disable irq = %d", channel,
        (unsigned) (controller->control.disable_irq) ? 1 : 0));
      break;

    default:
      BX_PANIC(("hard drive: io write to address %x = %02x",
        (unsigned) address, (unsigned) value));
    }
}

  bx_bool BX_CPP_AttrRegparmN(2)
bx_hard_drive_c::calculate_logical_address(Bit8u channel, Bit64s *sector)
{
  Bit64s logical_sector;

  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  if (controller->lba_mode) {
    if (!controller->lba48) {
      logical_sector = ((Bit32u)controller->head_no) << 24 |
        ((Bit32u)controller->cylinder_no) << 8 |
        (Bit32u)controller->sector_no;
    } else {
      logical_sector = ((Bit64u)controller->hob.hcyl) << 40 |
        ((Bit64u)controller->hob.lcyl) << 32 |
        ((Bit64u)controller->hob.sector) << 24 |
        ((Bit64u)controller->cylinder_no) << 8 |
        (Bit64u)controller->sector_no;
    }
  } else {
    logical_sector = ((Bit32u)controller->cylinder_no * BX_SELECTED_DRIVE(channel).hdimage->heads *
      BX_SELECTED_DRIVE(channel).hdimage->spt) +
      (Bit32u)(controller->head_no * BX_SELECTED_DRIVE(channel).hdimage->spt) +
      (controller->sector_no - 1);
  }

  Bit64s sector_count = BX_SELECTED_DRIVE(channel).hdimage->hd_size / 512;
  if (logical_sector >= sector_count) {
    BX_ERROR (("logical address out of bounds ("FMT_LL"d/"FMT_LL"d) - aborting command", logical_sector, sector_count));
    return 0;
  }
  *sector = logical_sector;
  return 1;
}

  void BX_CPP_AttrRegparmN(2)
bx_hard_drive_c::increment_address(Bit8u channel, Bit64s *sector)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  controller->sector_count--;
  controller->num_sectors--;

  if (controller->lba_mode) {
    Bit64s logical_sector = *sector;
    logical_sector++;
    if (!controller->lba48) {
      controller->head_no = (Bit8u)((logical_sector >> 24) & 0xf);
      controller->cylinder_no = (Bit16u)((logical_sector >> 8) & 0xffff);
      controller->sector_no = (Bit8u)((logical_sector) & 0xff);
    } else {
      controller->hob.hcyl = (Bit8u)((logical_sector >> 40) & 0xff);
      controller->hob.lcyl = (Bit8u)((logical_sector >> 32) & 0xff);
      controller->hob.sector = (Bit8u)((logical_sector >> 24) & 0xff);
      controller->cylinder_no = (Bit16u)((logical_sector >> 8) & 0xffff);
      controller->sector_no = (Bit8u)((logical_sector) & 0xff);
    }
    *sector = logical_sector;
  } else {
    controller->sector_no++;
    if (controller->sector_no > BX_SELECTED_DRIVE(channel).hdimage->spt) {
      controller->sector_no = 1;
      controller->head_no++;
      if (controller->head_no >= BX_SELECTED_DRIVE(channel).hdimage->heads) {
        controller->head_no = 0;
        controller->cylinder_no++;
        if (controller->cylinder_no >= BX_SELECTED_DRIVE(channel).hdimage->cylinders) {
          controller->cylinder_no = BX_SELECTED_DRIVE(channel).hdimage->cylinders - 1;
        }
      }
    }
  }
}

void bx_hard_drive_c::identify_ATAPI_drive(Bit8u channel)
{
  unsigned i;
  char serial_number[21];

  memset(&BX_SELECTED_DRIVE(channel).id_drive, 0, 512);

  BX_SELECTED_DRIVE(channel).id_drive[0] = (2 << 14) | (5 << 8) | (1 << 7) | (2 << 5) | (0 << 0); // Removable CDROM, 50us response, 12 byte packets

  for (i = 1; i <= 9; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  strcpy(serial_number, "BXCD00000           ");
  serial_number[8] = BX_SELECTED_DRIVE(channel).device_num;
  for (i = 0; i < 10; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[10+i] = (serial_number[i*2] << 8) |
      serial_number[i*2 + 1];
  }

  for (i = 20; i <= 22; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  const char* firmware = "ALPHA1  ";
  for (i = 0; i < strlen(firmware)/2; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[23+i] = (firmware[i*2] << 8) | firmware[i*2 + 1];
  }
  BX_ASSERT((23+i) == 27);

  for (i = 0; i < strlen((char *) BX_SELECTED_MODEL(channel))/2; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[27+i] = (BX_SELECTED_MODEL(channel)[i*2] << 8) |
       BX_SELECTED_MODEL(channel)[i*2 + 1];
  }
  BX_ASSERT((27+i) == 47);

  BX_SELECTED_DRIVE(channel).id_drive[47] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[48] = 1; // 32 bits access

  if (BX_HD_THIS bmdma_present()) {
    BX_SELECTED_DRIVE(channel).id_drive[49] = (1<<9) | (1<<8); // LBA and DMA
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[49] = (1<<9); // LBA only supported
  }

  BX_SELECTED_DRIVE(channel).id_drive[50] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[51] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[52] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[53] = 3; // words 64-70, 54-58 valid

  for (i = 54; i <= 62; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  if (BX_HD_THIS bmdma_present()) {
    BX_SELECTED_DRIVE(channel).id_drive[63] = 0x07 | (BX_SELECTED_CONTROLLER(channel).mdma_mode << 8);
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[63] = 0x0;
  }
  BX_SELECTED_DRIVE(channel).id_drive[64] = 0x0001; // PIO
  BX_SELECTED_DRIVE(channel).id_drive[65] = 0x00b4;
  BX_SELECTED_DRIVE(channel).id_drive[66] = 0x00b4;
  BX_SELECTED_DRIVE(channel).id_drive[67] = 0x012c;
  BX_SELECTED_DRIVE(channel).id_drive[68] = 0x00b4;

  BX_SELECTED_DRIVE(channel).id_drive[69] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[70] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[71] = 30; // faked
  BX_SELECTED_DRIVE(channel).id_drive[72] = 30; // faked
  BX_SELECTED_DRIVE(channel).id_drive[73] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[74] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[75] = 0;

  for (i = 76; i <= 79; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[80] = 0x1e; // supports up to ATA/ATAPI-4
  BX_SELECTED_DRIVE(channel).id_drive[81] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[82] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[83] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[84] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[85] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[86] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[87] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[88] = 0;

  BX_SELECTED_DRIVE(channel).identify_set = 1;
}

void bx_hard_drive_c::identify_drive(Bit8u channel)
{
  unsigned i;
  char serial_number[21];
  Bit32u temp32;
  Bit64u num_sects;

  memset(&BX_SELECTED_DRIVE(channel).id_drive, 0, 512);

  // Identify Drive command return values definition
  //
  // This code is rehashed from some that was donated.
  // I'm using ANSI X3.221-1994, AT Attachment Interface for Disk Drives
  // and X3T10 2008D Working Draft for ATA-3


  // Word 0: general config bit-significant info
  //   Note: bits 1-5 and 8-14 are now "Vendor specific (obsolete)"
  //   bit 15: 0=ATA device
  //           1=ATAPI device
  //   bit 14: 1=format speed tolerance gap required
  //   bit 13: 1=track offset option available
  //   bit 12: 1=data strobe offset option available
  //   bit 11: 1=rotational speed tolerance is > 0,5% (typo?)
  //   bit 10: 1=disk transfer rate > 10Mbs
  //   bit  9: 1=disk transfer rate > 5Mbs but <= 10Mbs
  //   bit  8: 1=disk transfer rate <= 5Mbs
  //   bit  7: 1=removable cartridge drive
  //   bit  6: 1=fixed drive
  //   bit  5: 1=spindle motor control option implemented
  //   bit  4: 1=head switch time > 15 usec
  //   bit  3: 1=not MFM encoded
  //   bit  2: 1=soft sectored
  //   bit  1: 1=hard sectored
  //   bit  0: 0=reserved
  BX_SELECTED_DRIVE(channel).id_drive[0] = 0x0040;

  // Word 1: number of user-addressable cylinders in
  //   default translation mode.  If the value in words 60-61
  //   exceed 16,515,072, this word shall contain 16,383.
  if (BX_SELECTED_DRIVE(channel).hdimage->cylinders > 16383) {
    BX_SELECTED_DRIVE(channel).id_drive[1] = 16383;
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[1] = BX_SELECTED_DRIVE(channel).hdimage->cylinders;
  }

  // Word 2: reserved

  // Word 3: number of user-addressable heads in default
  //   translation mode
  BX_SELECTED_DRIVE(channel).id_drive[3] = BX_SELECTED_DRIVE(channel).hdimage->heads;

  // Word 4: # unformatted bytes per translated track in default xlate mode
  // Word 5: # unformatted bytes per sector in default xlated mode
  // Word 6: # user-addressable sectors per track in default xlate mode
  // Note: words 4,5 are now "Vendor specific (obsolete)"
  BX_SELECTED_DRIVE(channel).id_drive[4] = (512 * BX_SELECTED_DRIVE(channel).hdimage->spt);
  BX_SELECTED_DRIVE(channel).id_drive[5] = 512;
  BX_SELECTED_DRIVE(channel).id_drive[6] = BX_SELECTED_DRIVE(channel).hdimage->spt;

  // Word 7-9: Vendor specific

  // Word 10-19: Serial number (20 ASCII characters, 0000h=not specified)
  // This field is right justified and padded with spaces (20h).
  strcpy(serial_number, "BXHD00000           ");
  serial_number[7] = channel + 49;
  serial_number[8] = BX_HD_THIS channels[channel].drive_select + 49;
  for (i = 0; i < 10; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[10+i] = (serial_number[i*2] << 8) |
      serial_number[i*2 + 1];
  }

  // Word 20: buffer type
  //          0000h = not specified
  //          0001h = single ported single sector buffer which is
  //                  not capable of simulataneous data xfers to/from
  //                  the host and the disk.
  //          0002h = dual ported multi-sector buffer capable of
  //                  simulatenous data xfers to/from the host and disk.
  //          0003h = dual ported mutli-sector buffer capable of
  //                  simulatenous data xfers with a read caching
  //                  capability.
  //          0004h-ffffh = reserved
  BX_SELECTED_DRIVE(channel).id_drive[20] = 3;

  // Word 21: buffer size in 512 byte increments, 0000h = not specified
  BX_SELECTED_DRIVE(channel).id_drive[21] = 512; // 512 Sectors = 256kB cache

  // Word 22: # of ECC bytes available on read/write long cmds
  //          0000h = not specified
  BX_SELECTED_DRIVE(channel).id_drive[22] = 4;

  // Word 23..26: Firmware revision (8 ascii chars, 0000h=not specified)
  // This field is left justified and padded with spaces (20h)
  for (i=23; i<=26; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // Word 27..46: Model number (40 ascii chars, 0000h=not specified)
  // This field is left justified and padded with spaces (20h)
  for (i=0; i<20; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[27+i] = (BX_SELECTED_MODEL(channel)[i*2] << 8) |
                                  BX_SELECTED_MODEL(channel)[i*2 + 1];
  }

  // Word 47: 15-8 Vendor unique
  //           7-0 00h= read/write multiple commands not implemented
  //               xxh= maximum # of sectors that can be transferred
  //                    per interrupt on read and write multiple commands
  BX_SELECTED_DRIVE(channel).id_drive[47] = MAX_MULTIPLE_SECTORS;

  // Word 48: 0000h = cannot perform dword IO
  //          0001h = can    perform dword IO
  BX_SELECTED_DRIVE(channel).id_drive[48] = 1;

  // Word 49: Capabilities
  //   15-10: 0 = reserved
  //       9: 1 = LBA supported
  //       8: 1 = DMA supported
  //     7-0: Vendor unique
  if (BX_HD_THIS bmdma_present()) {
    BX_SELECTED_DRIVE(channel).id_drive[49] = (1<<9) | (1<<8);
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[49] = 1<<9;
  }

  // Word 50: Reserved

  // Word 51: 15-8 PIO data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_DRIVE(channel).id_drive[51] = 0x200;

  // Word 52: 15-8 DMA data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_DRIVE(channel).id_drive[52] = 0x200;

  // Word 53: 15-1 Reserved
  //             2 1=the fields reported in word 88 are valid
  //             1 1=the fields reported in words 64-70 are valid
  //             0 1=the fields reported in words 54-58 are valid
  BX_SELECTED_DRIVE(channel).id_drive[53] = 0x07;

  // Word 54: # of user-addressable cylinders in curr xlate mode
  // Word 55: # of user-addressable heads in curr xlate mode
  // Word 56: # of user-addressable sectors/track in curr xlate mode
  if (BX_SELECTED_DRIVE(channel).hdimage->cylinders > 16383) {
    BX_SELECTED_DRIVE(channel).id_drive[54] = 16383;
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[54] = BX_SELECTED_DRIVE(channel).hdimage->cylinders;
  }
  BX_SELECTED_DRIVE(channel).id_drive[55] = BX_SELECTED_DRIVE(channel).hdimage->heads;
  BX_SELECTED_DRIVE(channel).id_drive[56] = BX_SELECTED_DRIVE(channel).hdimage->spt;

  // Word 57-58: Current capacity in sectors
  // Excludes all sectors used for device specific purposes.
  temp32 =
    BX_SELECTED_DRIVE(channel).hdimage->cylinders *
    BX_SELECTED_DRIVE(channel).hdimage->heads *
    BX_SELECTED_DRIVE(channel).hdimage->spt;
  BX_SELECTED_DRIVE(channel).id_drive[57] = (temp32 & 0xffff); // LSW
  BX_SELECTED_DRIVE(channel).id_drive[58] = (temp32 >> 16);    // MSW

  // Word 59: 15-9 Reserved
  //             8 1=multiple sector setting is valid
  //           7-0 current setting for number of sectors that can be
  //               transferred per interrupt on R/W multiple commands
  if (BX_SELECTED_CONTROLLER(channel).multiple_sectors > 0)
    BX_SELECTED_DRIVE(channel).id_drive[59] = 0x0100 | BX_SELECTED_CONTROLLER(channel).multiple_sectors;
  else
    BX_SELECTED_DRIVE(channel).id_drive[59] = 0x0000;

  // Word 60-61:
  // If drive supports LBA Mode, these words reflect total # of user
  // addressable sectors.  This value does not depend on the current
  // drive geometry.  If the drive does not support LBA mode, these
  // words shall be set to 0.
  if (BX_SELECTED_DRIVE(channel).hdimage->hd_size > 0)
    num_sects = (BX_SELECTED_DRIVE(channel).hdimage->hd_size >> 9);
  else
    num_sects = BX_SELECTED_DRIVE(channel).hdimage->cylinders * BX_SELECTED_DRIVE(channel).hdimage->heads * BX_SELECTED_DRIVE(channel).hdimage->spt;
  BX_SELECTED_DRIVE(channel).id_drive[60] = (Bit16u)(num_sects & 0xffff); // LSW
  BX_SELECTED_DRIVE(channel).id_drive[61] = (Bit16u)(num_sects >> 16); // MSW

  // Word 62: 15-8 single word DMA transfer mode active
  //           7-0 single word DMA transfer modes supported
  // The low order byte identifies by bit, all the Modes which are
  // supported e.g., if Mode 0 is supported bit 0 is set.
  // The high order byte contains a single bit set to indiciate
  // which mode is active.
  BX_SELECTED_DRIVE(channel).id_drive[62] = 0x0;

  // Word 63: 15-8 multiword DMA transfer mode active
  //           7-0 multiword DMA transfer modes supported
  // The low order byte identifies by bit, all the Modes which are
  // supported e.g., if Mode 0 is supported bit 0 is set.
  // The high order byte contains a single bit set to indiciate
  // which mode is active.
  if (BX_HD_THIS bmdma_present()) {
    BX_SELECTED_DRIVE(channel).id_drive[63] = 0x07 | (BX_SELECTED_CONTROLLER(channel).mdma_mode << 8);
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[63] = 0x0;
  }

  // Word 64 PIO modes supported
  BX_SELECTED_DRIVE(channel).id_drive[64] = 0x00;

  // Word 65-68 PIO/DMA cycle time (nanoseconds)
  for (i=65; i<=68; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 120;

  // Word 69-79 Reserved

  // Word 80: 15-5 reserved
  //             6 supports ATA/ATAPI-6
  //             5 supports ATA/ATAPI-5
  //             4 supports ATA/ATAPI-4
  //             3 supports ATA-3
  //             2 supports ATA-2
  //             1 supports ATA-1
  //             0 reserved
  BX_SELECTED_DRIVE(channel).id_drive[80] = 0x7e;

  // Word 81: Minor version number
  BX_SELECTED_DRIVE(channel).id_drive[81] = 0x00;

  // Word 82: 15 obsolete
  //          14 NOP command supported
  //          13 READ BUFFER command supported
  //          12 WRITE BUFFER command supported
  //          11 obsolete
  //          10 Host protected area feature set supported
  //           9 DEVICE RESET command supported
  //           8 SERVICE interrupt supported
  //           7 release interrupt supported
  //           6 look-ahead supported
  //           5 write cache supported
  //           4 supports PACKET command feature set
  //           3 supports power management feature set
  //           2 supports removable media feature set
  //           1 supports securite mode feature set
  //           0 support SMART feature set
  BX_SELECTED_DRIVE(channel).id_drive[82] = 1 << 14;

  // Word 83: 15 shall be ZERO
  //          14 shall be ONE
  //          13 FLUSH CACHE EXT command supported
  //          12 FLUSH CACHE command supported
  //          11 Device configuration overlay supported
  //          10 48-bit Address feature set supported
  //           9 Automatic acoustic management supported
  //           8 SET MAX security supported
  //           7 reserved for 1407DT PARTIES
  //           6 SetF sub-command Power-Up supported
  //           5 Power-Up in standby feature set supported
  //           4 Removable media notification supported
  //           3 APM feature set supported
  //           2 CFA feature set supported
  //           1 READ/WRITE DMA QUEUED commands supported
  //           0 Download MicroCode supported
  BX_SELECTED_DRIVE(channel).id_drive[83] = (1 << 14) | (1 << 13) | (1 << 12) | (1 << 10);
  BX_SELECTED_DRIVE(channel).id_drive[84] = 1 << 14;
  BX_SELECTED_DRIVE(channel).id_drive[85] = 1 << 14;

  // Word 86: 15 shall be ZERO
  //          14 shall be ONE
  //          13 FLUSH CACHE EXT command enabled
  //          12 FLUSH CACHE command enabled
  //          11 Device configuration overlay enabled
  //          10 48-bit Address feature set enabled
  //           9 Automatic acoustic management enabled
  //           8 SET MAX security enabled
  //           7 reserved for 1407DT PARTIES
  //           6 SetF sub-command Power-Up enabled
  //           5 Power-Up in standby feature set enabled
  //           4 Removable media notification enabled
  //           3 APM feature set enabled
  //           2 CFA feature set enabled
  //           1 READ/WRITE DMA QUEUED commands enabled
  //           0 Download MicroCode enabled
  BX_SELECTED_DRIVE(channel).id_drive[86] = (1 << 14) | (1 << 13) | (1 << 12) | (1 << 10);
  BX_SELECTED_DRIVE(channel).id_drive[87] = 1 << 14;

  if (BX_HD_THIS bmdma_present()) {
    BX_SELECTED_DRIVE(channel).id_drive[88] = 0x3f | (BX_SELECTED_CONTROLLER(channel).udma_mode << 8);
  } else {
    BX_SELECTED_DRIVE(channel).id_drive[88] = 0x0;
  }
  BX_SELECTED_DRIVE(channel).id_drive[93] = 1 | (1 << 14) | 0x2000;

  // Word 100-103: 48-bit total number of sectors
  BX_SELECTED_DRIVE(channel).id_drive[100] = (Bit16u)(num_sects & 0xffff);
  BX_SELECTED_DRIVE(channel).id_drive[101] = (Bit16u)(num_sects >> 16);
  BX_SELECTED_DRIVE(channel).id_drive[102] = (Bit16u)(num_sects >> 32);
  BX_SELECTED_DRIVE(channel).id_drive[103] = (Bit16u)(num_sects >> 48);

  // Word 128-159 Vendor unique
  // Word 160-255 Reserved

  BX_SELECTED_DRIVE(channel).identify_set = 1;
}

void bx_hard_drive_c::init_send_atapi_command(Bit8u channel, Bit8u command, int req_length, int alloc_length, bx_bool lazy)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  // byte_count is a union of cylinder_no;
  // lazy is used to force a data read in the buffer at the next read.

  if (controller->byte_count == 0xffff)
    controller->byte_count = 0xfffe;

  if ((controller->byte_count & 1) &&
     !(alloc_length <= controller->byte_count))
  {
    BX_INFO(("Odd byte count (0x%04x) to ATAPI command 0x%02x, using 0x%04x",
      controller->byte_count, command, controller->byte_count - 1));
    controller->byte_count--;
  }

  if (!controller->packet_dma) {
    if (controller->byte_count == 0)
      BX_PANIC(("ATAPI command 0x%02x with zero byte count", command));
  }

  if (alloc_length < 0)
    BX_PANIC(("Allocation length < 0"));
  if (alloc_length == 0)
    alloc_length = controller->byte_count;

  controller->status.busy = 1;
  controller->status.drive_ready = 1;
  controller->status.drq = 0;
  controller->status.err = 0;

  // no bytes transfered yet
  if (lazy)
    controller->buffer_index = controller->buffer_size;
  else
    controller->buffer_index = 0;
  controller->drq_index = 0;

  if (controller->byte_count > req_length)
    controller->byte_count = req_length;

  if (controller->byte_count > alloc_length)
    controller->byte_count = alloc_length;

  BX_SELECTED_DRIVE(channel).atapi.command = command;
  BX_SELECTED_DRIVE(channel).atapi.drq_bytes = controller->byte_count;
  BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining = (req_length < alloc_length) ? req_length : alloc_length;
}

void bx_hard_drive_c::atapi_cmd_error(Bit8u channel, sense_t sense_key, asc_t asc, bx_bool show)
{
  if (show) {
    BX_ERROR(("ata%d-%d: atapi_cmd_error: key=%02x asc=%02x", channel, BX_SLAVE_SELECTED(channel), sense_key, asc));
  } else {
    BX_DEBUG(("ata%d-%d: atapi_cmd_error: key=%02x asc=%02x", channel, BX_SLAVE_SELECTED(channel), sense_key, asc));
  }

  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  controller->error_register = sense_key << 4;
  controller->interrupt_reason.i_o = 1;
  controller->interrupt_reason.c_d = 1;
  controller->interrupt_reason.rel = 0;
  controller->status.busy = 0;
  controller->status.drive_ready = 1;
  controller->status.write_fault = 0;
  controller->status.drq = 0;
  controller->status.err = 1;

  BX_SELECTED_DRIVE(channel).sense.sense_key = sense_key;
  BX_SELECTED_DRIVE(channel).sense.asc = asc;
  BX_SELECTED_DRIVE(channel).sense.ascq = 0;
}

  void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::atapi_cmd_nop(controller_t *controller)
{
  controller->interrupt_reason.i_o = 1;
  controller->interrupt_reason.c_d = 1;
  controller->interrupt_reason.rel = 0;
  controller->status.busy = 0;
  controller->status.drive_ready = 1;
  controller->status.drq = 0;
  controller->status.err = 0;
}

void bx_hard_drive_c::init_mode_sense_single(Bit8u channel, const void* src, int size)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  // Header
  controller->buffer[0] = (size+6) >> 8;
  controller->buffer[1] = (size+6) & 0xff;
  if (BX_SELECTED_DRIVE(channel).cdrom.ready)
    controller->buffer[2] = 0x12; // media present 120mm CD-ROM (CD-R) data/audio  door closed
  else
    controller->buffer[2] = 0x70; // no media present
  controller->buffer[3] = 0; // reserved
  controller->buffer[4] = 0; // reserved
  controller->buffer[5] = 0; // reserved
  controller->buffer[6] = 0; // reserved
  controller->buffer[7] = 0; // reserved

  // Data
  memcpy(controller->buffer + 8, src, size);
}

  void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::ready_to_send_atapi(Bit8u channel)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  controller->interrupt_reason.i_o = 1;
  controller->interrupt_reason.c_d = 0;
  controller->status.busy = 0;
  controller->status.drq = 1;
  controller->status.err = 0;

  if (BX_SELECTED_CONTROLLER(channel).packet_dma) {
    DEV_ide_bmdma_start_transfer(channel);
  } else {
    raise_interrupt(channel);
  }
}

  void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::raise_interrupt(Bit8u channel)
{
  if (!BX_SELECTED_CONTROLLER(channel).control.disable_irq) {
    Bit32u irq = BX_HD_THIS channels[channel].irq;
    BX_DEBUG(("raising interrupt %d {%s}", irq, BX_SELECTED_TYPE_STRING(channel)));
#if BX_SUPPORT_PCI
    DEV_ide_bmdma_set_irq(channel);
#endif
    DEV_pic_raise_irq(irq);
  } else {
    BX_DEBUG(("not raising interrupt {%s}", BX_SELECTED_TYPE_STRING(channel)));
  }
}

void bx_hard_drive_c::command_aborted(Bit8u channel, unsigned value)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  BX_DEBUG(("aborting on command 0x%02x {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
  controller->current_command = 0;
  controller->status.busy = 0;
  controller->status.drive_ready = 1;
  controller->status.err = 1;
  controller->error_register = 0x04; // command ABORTED
  controller->status.drq = 0;
  controller->status.corrected_data = 0;
  controller->buffer_index = 0;
  raise_interrupt(channel);
}

Bit32u bx_hard_drive_c::get_first_cd_handle(void)
{
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (BX_DRIVE_IS_CD(channel,0)) return  (channel*2);
    if (BX_DRIVE_IS_CD(channel,1)) return ((channel*2) + 1);
  }
  return BX_MAX_ATA_CHANNEL*2;
}

bx_bool bx_hard_drive_c::get_cd_media_status(Bit32u handle)
{
  if (handle >= BX_MAX_ATA_CHANNEL*2) return 0;

  Bit8u channel = handle / 2;
  Bit8u device  = handle % 2;
  return BX_HD_THIS channels[channel].drives[device].cdrom.ready;
}

bx_bool bx_hard_drive_c::set_cd_media_status(Bit32u handle, bx_bool status)
{
  char ata_name[20];

  BX_DEBUG(("set_cd_media_status handle=%d status=%d", handle, status));
  if (handle >= BX_MAX_ATA_CHANNEL*2) return 0;

  Bit8u channel = handle / 2;
  Bit8u device  = handle % 2;

  sprintf(ata_name, "ata.%d.%s", channel, (device==0)?"master":"slave");
  bx_list_c *base = (bx_list_c*) SIM->get_param(ata_name);
  // if setting to the current value, nothing to do
  if (status == BX_HD_THIS channels[channel].drives[device].cdrom.ready)
    return(status);
  // return 0 if selected drive is not a cdrom
  if (!BX_DRIVE_IS_CD(channel,device))
    return(0);

  if (status == 0) {
    // eject cdrom if not locked by guest OS
    if (!BX_HD_THIS channels[channel].drives[device].cdrom.locked) {
      BX_HD_THIS channels[channel].drives[device].cdrom.cd->eject_cdrom();
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
      SIM->get_param_enum("status", base)->set(BX_EJECTED);
    } else {
      return(1);
    }
  } else {
    // insert cdrom
    if (BX_HD_THIS channels[channel].drives[device].cdrom.cd->insert_cdrom(SIM->get_param_string("path", base)->getptr())) {
      BX_INFO(("Media present in CD-ROM drive"));
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 1;
      Bit32u capacity = BX_HD_THIS channels[channel].drives[device].cdrom.cd->capacity();
      BX_HD_THIS channels[channel].drives[device].cdrom.max_lba = capacity - 1;
      BX_HD_THIS channels[channel].drives[device].cdrom.curr_lba = capacity - 1;
      BX_INFO(("Capacity is %d sectors (%.2f MB)", capacity, (float)capacity / 512.0));
      SIM->get_param_enum("status", base)->set(BX_INSERTED);
      BX_SELECTED_DRIVE(channel).sense.sense_key = SENSE_UNIT_ATTENTION;
      BX_SELECTED_DRIVE(channel).sense.asc = ASC_MEDIUM_MAY_HAVE_CHANGED;
      BX_SELECTED_DRIVE(channel).sense.ascq = 0;
      raise_interrupt(channel);
    } else {
      BX_INFO(("Could not locate CD-ROM, continuing with media not present"));
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
      SIM->get_param_enum("status", base)->set(BX_EJECTED);
    }
  }
  return (BX_HD_THIS channels[channel].drives[device].cdrom.ready);
}

bx_bool bx_hard_drive_c::bmdma_present(void)
{
#if BX_SUPPORT_PCI
  if (BX_HD_THIS pci_enabled) {
    return DEV_ide_bmdma_present();
  }
#endif
  return 0;
}

#if BX_SUPPORT_PCI
bx_bool bx_hard_drive_c::bmdma_read_sector(Bit8u channel, Bit8u *buffer, Bit32u *sector_size)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  if ((controller->current_command == 0xC8) ||
      (controller->current_command == 0x25)) {
    *sector_size = 512;
    if (!ide_read_sector(channel, buffer, 512)) {
      return 0;
    }
  } else if (controller->current_command == 0xA0) {
    if (controller->packet_dma) {
      switch (BX_SELECTED_DRIVE(channel).atapi.command) {
        case 0x28: // read (10)
        case 0xa8: // read (12)
        case 0xbe: // read cd
          *sector_size = controller->buffer_size;
          if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
            BX_PANIC(("Read with CDROM not ready"));
            return 0;
          }
          /* set status bar conditions for device */
          bx_gui->statusbar_setitem(BX_SELECTED_DRIVE(channel).statusbar_id, 1);
          if (!BX_SELECTED_DRIVE(channel).cdrom.cd->read_block(buffer, BX_SELECTED_DRIVE(channel).cdrom.next_lba,
                                                               controller->buffer_size))
          {
            BX_PANIC(("CDROM: read block %d failed", BX_SELECTED_DRIVE(channel).cdrom.next_lba));
            return 0;
          }
          BX_SELECTED_DRIVE(channel).cdrom.next_lba++;
          BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks--;
          if (!BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks) {
            BX_SELECTED_DRIVE(channel).cdrom.curr_lba = BX_SELECTED_DRIVE(channel).cdrom.next_lba;
          }
          break;
        default:
          memcpy(buffer, controller->buffer, *sector_size);
          break;
      }
    } else {
      BX_ERROR(("PACKET-DMA not active"));
      command_aborted(channel, controller->current_command);
      return 0;
    }
  } else {
    BX_ERROR(("DMA read not active"));
    command_aborted(channel, controller->current_command);
    return 0;
  }
  return 1;
}

bx_bool bx_hard_drive_c::bmdma_write_sector(Bit8u channel, Bit8u *buffer)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  if ((controller->current_command != 0xCA) &&
      (controller->current_command != 0x35)) {
    BX_ERROR(("DMA write not active"));
    command_aborted (channel, controller->current_command);
    return 0;
  }
  if (!ide_write_sector(channel, buffer, 512)) {
    return 0;
  }
  return 1;
}

void bx_hard_drive_c::bmdma_complete(Bit8u channel)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  controller->status.busy = 0;
  controller->status.drive_ready = 1;
  controller->status.drq = 0;
  controller->status.err = 0;
  if (BX_SELECTED_IS_CD(channel)) {
    controller->interrupt_reason.i_o = 1;
    controller->interrupt_reason.c_d = 1;
    controller->interrupt_reason.rel = 0;
  } else {
    controller->status.write_fault = 0;
    controller->status.seek_complete = 1;
    controller->status.corrected_data = 0;
    BX_SELECTED_DRIVE(channel).curr_lsector = BX_SELECTED_DRIVE(channel).next_lsector;
  }
  raise_interrupt(channel);
}
#endif

void bx_hard_drive_c::set_signature(Bit8u channel, Bit8u id)
{
  // Device signature
  BX_CONTROLLER(channel,id).head_no       = 0;
  BX_CONTROLLER(channel,id).sector_count  = 1;
  BX_CONTROLLER(channel,id).sector_no     = 1;
  if (BX_DRIVE_IS_HD(channel,id)) {
    BX_CONTROLLER(channel,id).cylinder_no = 0;
    BX_HD_THIS channels[channel].drive_select = 0;
  } else if (BX_DRIVE_IS_CD(channel,id)) {
    BX_CONTROLLER(channel,id).cylinder_no = 0xeb14;
  } else {
    BX_CONTROLLER(channel,id).cylinder_no = 0xffff;
  }
}

bx_bool bx_hard_drive_c::ide_read_sector(Bit8u channel, Bit8u *buffer, Bit32u buffer_size)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  Bit64s logical_sector = 0;
  Bit64s ret;

  int sector_count = (buffer_size / 512);
  Bit8u *bufptr = buffer;
  do {
    if (!calculate_logical_address(channel, &logical_sector)) {
      command_aborted(channel, controller->current_command);
      return 0;
    }
    ret = BX_SELECTED_DRIVE(channel).hdimage->lseek(logical_sector * 512, SEEK_SET);
    if (ret < 0) {
      BX_ERROR(("could not lseek() hard drive image file"));
      command_aborted(channel, controller->current_command);
      return 0;
    }
    /* set status bar conditions for device */
    bx_gui->statusbar_setitem(BX_SELECTED_DRIVE(channel).statusbar_id, 1);
    ret = BX_SELECTED_DRIVE(channel).hdimage->read((bx_ptr_t)bufptr, 512);
    if (ret < 512) {
      BX_ERROR(("could not read() hard drive image file at byte %lu", (unsigned long)logical_sector*512));
      command_aborted(channel, controller->current_command);
      return 0;
    }
    increment_address(channel, &logical_sector);
    BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
    bufptr += 512;
  } while (--sector_count > 0);

  return 1;
}

bx_bool bx_hard_drive_c::ide_write_sector(Bit8u channel, Bit8u *buffer, Bit32u buffer_size)
{
  controller_t *controller = &BX_SELECTED_CONTROLLER(channel);

  Bit64s logical_sector = 0;
  Bit64s ret;

  int sector_count = (buffer_size / 512);
  Bit8u *bufptr = buffer;
  do {
    if (!calculate_logical_address(channel, &logical_sector)) {
      command_aborted(channel, controller->current_command);
      return 0;
    }
    ret = BX_SELECTED_DRIVE(channel).hdimage->lseek(logical_sector * 512, SEEK_SET);
    if (ret < 0) {
      BX_ERROR(("could not lseek() hard drive image file at byte %lu", (unsigned long)logical_sector * 512));
      command_aborted(channel, controller->current_command);
      return 0;
    }
    /* set status bar conditions for device */
    bx_gui->statusbar_setitem(BX_SELECTED_DRIVE(channel).statusbar_id, 1, 1 /* write */);
    ret = BX_SELECTED_DRIVE(channel).hdimage->write((bx_ptr_t)bufptr, 512);
    if (ret < 512) {
      BX_ERROR(("could not write() hard drive image file at byte %lu", (unsigned long)logical_sector*512));
      command_aborted(channel, controller->current_command);
      return 0;
    }
    increment_address(channel, &logical_sector);
    BX_SELECTED_DRIVE(channel).next_lsector = logical_sector;
    bufptr += 512;
  } while (--sector_count > 0);

  return 1;
}

void bx_hard_drive_c::lba48_transform(controller_t *controller, bx_bool lba48)
{
  controller->lba48 = lba48;

  if (!controller->lba48) {
    if (!controller->sector_count)
      controller->num_sectors = 256;
    else
      controller->num_sectors = controller->sector_count;
  } else {
    if (!controller->sector_count && !controller->hob.nsector)
      controller->num_sectors = 65536;
    else
      controller->num_sectors = (controller->hob.nsector << 8) |
                                 controller->sector_count;
  }
}

void bx_hard_drive_c::start_seek(Bit8u channel)
{
  Bit64s new_pos, prev_pos, max_pos;
  Bit32u seek_time;
  double fSeekBase, fSeekTime;

  if (BX_SELECTED_IS_CD(channel)) {
    max_pos = BX_SELECTED_DRIVE(channel).cdrom.max_lba;
    prev_pos = BX_SELECTED_DRIVE(channel).cdrom.curr_lba;
    new_pos = BX_SELECTED_DRIVE(channel).cdrom.next_lba;
    fSeekBase = 80000.0;
  } else {
    max_pos = (BX_SELECTED_DRIVE(channel).hdimage->hd_size / 512) - 1;
    prev_pos = BX_SELECTED_DRIVE(channel).curr_lsector;
    new_pos = BX_SELECTED_DRIVE(channel).next_lsector;
    fSeekBase = 5000.0;
  }
  fSeekTime = fSeekBase * (double)abs((int)(new_pos - prev_pos + 1)) / (max_pos + 1);
  seek_time = (fSeekTime > 10.0) ? (Bit32u)fSeekTime : 10;
  bx_pc_system.activate_timer(
    BX_SELECTED_DRIVE(channel).seek_timer_index, seek_time, 0);
}

error_recovery_t::error_recovery_t()
{
  if (sizeof(error_recovery_t) != 8) {
    BX_PANIC(("error_recovery_t has size != 8"));
  }

  data[0] = 0x01;
  data[1] = 0x06;
  data[2] = 0x00;
  data[3] = 0x05; // Try to recover 5 times
  data[4] = 0x00;
  data[5] = 0x00;
  data[6] = 0x00;
  data[7] = 0x00;
}

// cdrom runtime parameter handling

// helper function
int get_device_handle_from_param(bx_param_c *param)
{
  char pname[BX_PATHNAME_LEN];

  bx_list_c *base = (bx_list_c*) param->get_parent();
  base->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strncmp(pname, "ata.", 4)) {
    int handle = (pname[4] - '0') << 1;
    if (!strcmp(base->get_name(), "slave")) {
      handle |= 1;
    }
    return handle;
  } else {
    return -1;
  }
}

Bit64s bx_hard_drive_c::cdrom_status_handler(bx_param_c *param, int set, Bit64s val)
{
  if (set) {
    int handle = get_device_handle_from_param(param);
    if (handle >= 0) {
      if (!strcmp(param->get_name(), "status")) {
        BX_HD_THIS channels[handle/2].drives[handle%2].status_changed = 1;
      }
    } else {
      BX_PANIC(("cdrom_status_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

const char *bx_hard_drive_c::cdrom_path_handler(bx_param_string_c *param, int set,
                                                const char *oldval, const char *val, int maxlen)
{
  if (set) {
    if (strlen(val) < 1) {
      val = "none";
    }
    int handle = get_device_handle_from_param(param);
    if (handle >= 0) {
      if (!strcmp(param->get_name(), "path")) {
        BX_HD_THIS channels[handle/2].drives[handle%2].status_changed = 1;
      }
    } else {
      BX_PANIC(("cdrom_path_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}
