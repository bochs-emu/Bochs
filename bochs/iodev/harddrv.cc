/////////////////////////////////////////////////////////////////////////
// $Id: harddrv.cc,v 1.99 2003-05-06 20:30:21 cbothamy Exp $
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

// Useful docs:
// AT Attachment with Packet Interface
// working draft by T13 at www.t13.org



// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"

#if BX_HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#define LOG_THIS theHardDrive->

// WARNING: dangerous options!
// These options provoke certain kinds of errors for testing purposes when they
// are set to a nonzero value.  DO NOT ENABLE THEM when using any disk image
// you care about.
#define TEST_READ_BEYOND_END 0
#define TEST_WRITE_BEYOND_END 0
#ifdef __GNUC__
#  if TEST_READ_BEYOND_END || TEST_WRITE_BEYOND_END
#    warning BEWARE: Dangerous options are enabled in harddrv.cc. If you are not trying to provoke hard drive errors you should disable them right now.
#  endif
#endif
// end of dangerous options.


#define INDEX_PULSE_CYCLE 10

#define PACKET_SIZE 12

static unsigned max_multiple_sectors  = 0; // was 0x3f
static unsigned curr_multiple_sectors = 0; // was 0x3f

// some packet handling macros
#define EXTRACT_FIELD(arr,byte,start,num_bits) (((arr)[(byte)] >> (start)) & ((1 << (num_bits)) - 1))
#define get_packet_field(c,b,s,n) (EXTRACT_FIELD((BX_SELECTED_CONTROLLER((c)).buffer),(b),(s),(n)))
#define get_packet_byte(c,b) (BX_SELECTED_CONTROLLER((c)).buffer[(b)])
#define get_packet_word(c,b) (((uint16)BX_SELECTED_CONTROLLER((c)).buffer[(b)] << 8) | BX_SELECTED_CONTROLLER((c)).buffer[(b)+1])


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

#define WRITE_FEATURES(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).features = _a; BX_CONTROLLER((c),1).features = _a; } while(0)
#define WRITE_SECTOR_COUNT(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).sector_count = _a; BX_CONTROLLER((c),1).sector_count = _a; } while(0)
#define WRITE_SECTOR_NUMBER(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).sector_no = _a; BX_CONTROLLER((c),1).sector_no = _a; } while(0)
#define WRITE_CYLINDER_LOW(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).cylinder_no = (BX_CONTROLLER((c),0).cylinder_no & 0xff00) | _a; BX_CONTROLLER((c),1).cylinder_no = (BX_CONTROLLER((c),1).cylinder_no & 0xff00) | _a; } while(0)
#define WRITE_CYLINDER_HIGH(c,a) do { uint16 _a = a; BX_CONTROLLER((c),0).cylinder_no = (_a << 8) | (BX_CONTROLLER((c),0).cylinder_no & 0xff); BX_CONTROLLER((c),1).cylinder_no = (_a << 8) | (BX_CONTROLLER((c),1).cylinder_no & 0xff); } while(0)
#define WRITE_HEAD_NO(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).head_no = _a; BX_CONTROLLER((c),1).head_no = _a; } while(0)
#define WRITE_LBA_MODE(c,a) do { uint8 _a = a; BX_CONTROLLER((c),0).lba_mode = _a; BX_CONTROLLER((c),1).lba_mode = _a; } while(0)

bx_hard_drive_c *theHardDrive = NULL;

  int
libharddrv_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theHardDrive = new bx_hard_drive_c ();
  bx_devices.pluginHardDrive = theHardDrive;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theHardDrive, BX_PLUGIN_HARDDRV);
  return(0); // Success
}

  void
libharddrv_LTX_plugin_fini(void)
{
}

bx_hard_drive_c::bx_hard_drive_c(void)
{
#if DLL_HD_SUPPORT
#   error code must be fixed to use DLL_HD_SUPPORT and 4 ata channels
#endif

    for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
      channels[channel].drives[0].hard_drive =  NULL;
      channels[channel].drives[1].hard_drive =  NULL;
      put("HD");
      settype(HDLOG);
    }
}


bx_hard_drive_c::~bx_hard_drive_c(void)
{
	BX_DEBUG(("Exit."));
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (channels[channel].drives[0].hard_drive != NULL )      /* DT 17.12.2001 21:55 */
    {
      delete channels[channel].drives[0].hard_drive;
      channels[channel].drives[0].hard_drive =  NULL;
    }
    if ( channels[channel].drives[1].hard_drive != NULL )
    {
      delete channels[channel].drives[1].hard_drive;
      channels[channel].drives[1].hard_drive =  NULL;        /* DT 17.12.2001 21:56 */
    }
  }
}




  void
bx_hard_drive_c::init(void)
{
  Bit8u channel;
  char  string[5];

  BX_DEBUG(("Init $Id: harddrv.cc,v 1.99 2003-05-06 20:30:21 cbothamy Exp $"));

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (bx_options.ata[channel].Opresent->get() == 1) {
      BX_HD_THIS channels[channel].ioaddr1 = bx_options.ata[channel].Oioaddr1->get();
      BX_HD_THIS channels[channel].ioaddr2 = bx_options.ata[channel].Oioaddr2->get();
      BX_HD_THIS channels[channel].irq = bx_options.ata[channel].Oirq->get();

      // Coherency check
      if ( (BX_HD_THIS channels[channel].ioaddr1 == 0) ||
           (BX_HD_THIS channels[channel].ioaddr2 == 0) ||
           (BX_HD_THIS channels[channel].irq == 0) ) {
        BX_PANIC(("incoherency for ata channel %d: io1=0x%x, io2=%x, irq=%d",
	  channel,
	  BX_HD_THIS channels[channel].ioaddr1,
	  BX_HD_THIS channels[channel].ioaddr2,
	  BX_HD_THIS channels[channel].irq));
        }
      }
    else {
      BX_HD_THIS channels[channel].ioaddr1 = 0;
      BX_HD_THIS channels[channel].ioaddr2 = 0;
      BX_HD_THIS channels[channel].irq = 0;
      }
    }

  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(string ,"ATA%d", channel);

    if (BX_HD_THIS channels[channel].irq != 0) 
      DEV_register_irq(BX_HD_THIS channels[channel].irq, strdup(string));

    if (BX_HD_THIS channels[channel].ioaddr1 != 0) {
      for (unsigned addr=0x0; addr<=0x7; addr++) {
        DEV_register_ioread_handler(this, read_handler,
                             BX_HD_THIS channels[channel].ioaddr1+addr, strdup(string), 7);
        DEV_register_iowrite_handler(this, write_handler,
                             BX_HD_THIS channels[channel].ioaddr1+addr, strdup(string), 7);
        }
      }

    // We don't want to register addresses 0x3f6 and 0x3f7 as they are handled by the floppy controller
    if ((BX_HD_THIS channels[channel].ioaddr2 != 0) && (BX_HD_THIS channels[channel].ioaddr2 != 0x3f0)) {
      for (unsigned addr=0x6; addr<=0x7; addr++) {
        DEV_register_ioread_handler(this, read_handler,
                              BX_HD_THIS channels[channel].ioaddr2+addr, strdup(string), 7);
        DEV_register_iowrite_handler(this, write_handler,
                              BX_HD_THIS channels[channel].ioaddr2+addr, strdup(string), 7);
        }
      }
     
     BX_HD_THIS channels[channel].drive_select = 0;
    }

  channel = 0;
  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    for (Bit8u device=0; device<2; device ++) {

      // If not present
      BX_HD_THIS channels[channel].drives[device].device_type           = IDE_NONE;
      if (!bx_options.atadevice[channel][device].Opresent->get()) {
        continue;
        }

      // Make model string
      strncpy((char*)BX_HD_THIS channels[channel].drives[device].model_no, 
        bx_options.atadevice[channel][device].Omodel->getptr(), 40);
      while (strlen((char *)BX_HD_THIS channels[channel].drives[device].model_no) < 40) {
        strcat ((char*)BX_HD_THIS channels[channel].drives[device].model_no, " ");
        }

      if (bx_options.atadevice[channel][device].Otype->get() == BX_ATA_DEVICE_DISK) {
        BX_DEBUG(( "Hard-Disk on target %d/%d",channel,device));
        BX_HD_THIS channels[channel].drives[device].device_type           = IDE_DISK;

        int cyl = bx_options.atadevice[channel][device].Ocylinders->get ();
        int heads = bx_options.atadevice[channel][device].Oheads->get ();
        int spt = bx_options.atadevice[channel][device].Ospt->get ();
        Bit64u disk_size = (Bit64u)cyl * heads * spt * 512;

        /* instantiate the right class */
        switch (bx_options.atadevice[channel][device].Omode->get()) {

          case BX_ATA_MODE_FLAT:
            BX_INFO(("HD on ata%d-%d: '%s' 'flat' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new default_image_t();
            break;

          case BX_ATA_MODE_CONCAT:
            BX_INFO(("HD on ata%d-%d: '%s' 'concat' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new concat_image_t();
            break;

#if EXTERNAL_DISK_SIMULATOR
          case BX_ATA_MODE_EXTDISKSIM:
            BX_INFO(("HD on ata%d-%d: '%s' 'External Simulator' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new EXTERNAL_DISK_SIMULATOR_CLASS();
            break;
#endif //EXTERNAL_DISK_SIMULATOR

#if DLL_HD_SUPPORT
          case BX_ATA_MODE_DLL_HD:
            BX_INFO(("HD on ata%d-%d: '%s' 'dll' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new dll_image_t();
            break;
#endif //DLL_HD_SUPPORT

          case BX_ATA_MODE_SPARSE:
            BX_INFO(("HD on ata%d-%d: '%s' 'sparse' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new sparse_image_t();
            break;

          case BX_ATA_MODE_VMWARE3:
            BX_INFO(("HD on ata%d-%d: '%s' 'vmware3' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new vmware3_image_t();
            break;

#if 0
          case BX_ATA_MODE_SPLIT:
            BX_INFO(("HD on ata%d-%d: '%s' 'split' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new split_image_t();
            break;
#endif

          case BX_ATA_MODE_UNDOABLE:
            BX_INFO(("HD on ata%d-%d: '%s' 'undoable' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new undoable_image_t(disk_size);
            break;

          case BX_ATA_MODE_GROWABLE:
            BX_INFO(("HD on ata%d-%d: '%s' 'growable' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new growable_image_t(disk_size);
            break;

          case BX_ATA_MODE_VOLATILE:
            BX_INFO(("HD on ata%d-%d: '%s' 'volatile' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new volatile_image_t(disk_size);
            break;

#if BX_COMPRESSED_HD_SUPPORT
          case BX_ATA_MODE_Z_UNDOABLE:
            BX_INFO(("HD on ata%d-%d: '%s' 'z-undoable' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new z_undoable_image_t(disk_size);
            break;

          case BX_ATA_MODE_Z_VOLATILE:
            BX_INFO(("HD on ata%d-%d: '%s' 'z-volatile' mode ", channel, device, 
                                    bx_options.atadevice[channel][device].Opath->getptr ()));
            channels[channel].drives[device].hard_drive = new z_volatile_image_t(disk_size);
            break;
#endif //BX_COMPRESSED_HD_SUPPORT

          default:
            BX_PANIC(("HD on ata%d-%d: '%s' unsupported HD mode : %s", channel, device, 
                      bx_options.atadevice[channel][device].Opath->getptr (),
                      atadevice_mode_names[bx_options.atadevice[channel][device].Omode->get()]));
            break;
        }

        BX_HD_THIS channels[channel].drives[device].hard_drive->cylinders = cyl;
        BX_HD_THIS channels[channel].drives[device].hard_drive->heads = heads;
        BX_HD_THIS channels[channel].drives[device].hard_drive->sectors = spt;

        if (cyl == 0 || heads == 0 || spt == 0) {
          BX_PANIC(("ata%d/%d cannot have zero cylinders, heads, or sectors/track", channel, device));
          }

        /* open hard drive image file */
        if ((BX_HD_THIS channels[channel].drives[device].hard_drive->open(bx_options.atadevice[channel][device].Opath->getptr ())) < 0) {
          BX_PANIC(("ata%d-%d: could not open hard drive image file '%s'", channel, device, bx_options.atadevice[channel][device].Opath->getptr ()));
          }
        }
      else if (bx_options.atadevice[channel][device].Otype->get() == BX_ATA_DEVICE_CDROM) {
        BX_DEBUG(( "CDROM on target %d/%d",channel,device));
        BX_HD_THIS channels[channel].drives[device].device_type = IDE_CDROM;
        BX_HD_THIS channels[channel].drives[device].cdrom.locked = 0;
        BX_HD_THIS channels[channel].drives[device].sense.sense_key = SENSE_NONE;
        BX_HD_THIS channels[channel].drives[device].sense.asc = 0;
        BX_HD_THIS channels[channel].drives[device].sense.ascq = 0;
	
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
#ifdef LOWLEVEL_CDROM
	BX_HD_THIS channels[channel].drives[device].cdrom.cd = new LOWLEVEL_CDROM(bx_options.atadevice[channel][device].Opath->getptr ());
        BX_INFO(("CD on ata%d-%d: '%s'",channel, device, bx_options.atadevice[channel][device].Opath->getptr ()));

	if (bx_options.atadevice[channel][device].Ostatus->get () == BX_INSERTED) {
	      if (BX_HD_THIS channels[channel].drives[device].cdrom.cd->insert_cdrom()) {
		    BX_INFO(( "Media present in CD-ROM drive"));
		    BX_HD_THIS channels[channel].drives[device].cdrom.ready = 1;
		    BX_HD_THIS channels[channel].drives[device].cdrom.capacity = BX_HD_THIS channels[channel].drives[device].cdrom.cd->capacity();
	      } else {		    
		    BX_INFO(( "Could not locate CD-ROM, continuing with media not present"));
		    BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
		    bx_options.atadevice[channel][device].Ostatus->set(BX_EJECTED);
	      }
	} else {
#endif
	      BX_INFO(( "Media not present in CD-ROM drive" ));
	      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
#ifdef LOWLEVEL_CDROM
	}
#endif
      }

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

    BX_CONTROLLER(channel,device).sectors_per_block   = 0x80;
    BX_CONTROLLER(channel,device).lba_mode            = 0;

    BX_CONTROLLER(channel,device).features            = 0;
    }
  }

#if BX_PDC20230C_VLBIDE_SUPPORT
      BX_HD_THIS pdc20230c.prog_mode = 0;
      BX_HD_THIS pdc20230c.prog_count = 0;
      BX_HD_THIS pdc20230c.p1f3_value = 0;
      BX_HD_THIS pdc20230c.p1f4_value = 0;
#endif


  // generate CMOS values for hard drive if not using a CMOS image
  if (!bx_options.cmos.OcmosImage->get ()) {
    DEV_cmos_set_reg(0x12, 0x00); // start out with: no drive 0, no drive 1

    if (BX_DRIVE_IS_HD(0,0)) {
      // Flag drive type as Fh, use extended CMOS location as real type
      DEV_cmos_set_reg(0x12, (DEV_cmos_get_reg(0x12) & 0x0f) | 0xf0);
      DEV_cmos_set_reg(0x19, 47); // user definable type
      // AMI BIOS: 1st hard disk #cyl low byte
      DEV_cmos_set_reg(0x1b, (bx_options.atadevice[0][0].Ocylinders->get () & 0x00ff));
      // AMI BIOS: 1st hard disk #cyl high byte
      DEV_cmos_set_reg(0x1c, (bx_options.atadevice[0][0].Ocylinders->get () & 0xff00) >> 8);
      // AMI BIOS: 1st hard disk #heads
      DEV_cmos_set_reg(0x1d, (bx_options.atadevice[0][0].Oheads->get ()));
      // AMI BIOS: 1st hard disk write precompensation cylinder, low byte
      DEV_cmos_set_reg(0x1e, 0xff); // -1
      // AMI BIOS: 1st hard disk write precompensation cylinder, high byte
      DEV_cmos_set_reg(0x1f, 0xff); // -1
      // AMI BIOS: 1st hard disk control byte
      DEV_cmos_set_reg(0x20, (0xc0 | ((bx_options.atadevice[0][0].Oheads->get () > 8) << 3)));
      // AMI BIOS: 1st hard disk landing zone, low byte
      DEV_cmos_set_reg(0x21, DEV_cmos_get_reg(0x1b));
      // AMI BIOS: 1st hard disk landing zone, high byte
      DEV_cmos_set_reg(0x22, DEV_cmos_get_reg(0x1c));
      // AMI BIOS: 1st hard disk sectors/track
      DEV_cmos_set_reg(0x23, bx_options.atadevice[0][0].Ospt->get ());
    }

    //set up cmos for second hard drive
    if (BX_DRIVE_IS_HD(0,1)) {
      BX_DEBUG(("1: I will put 0xf into the second hard disk field"));
      // fill in lower 4 bits of 0x12 for second HD
      DEV_cmos_set_reg(0x12, (DEV_cmos_get_reg(0x12) & 0xf0) | 0x0f);
      DEV_cmos_set_reg(0x1a, 47); // user definable type
      // AMI BIOS: 2nd hard disk #cyl low byte
      DEV_cmos_set_reg(0x24, (bx_options.atadevice[0][1].Ocylinders->get () & 0x00ff));
      // AMI BIOS: 2nd hard disk #cyl high byte
      DEV_cmos_set_reg(0x25, (bx_options.atadevice[0][1].Ocylinders->get () & 0xff00) >> 8);
      // AMI BIOS: 2nd hard disk #heads
      DEV_cmos_set_reg(0x26, (bx_options.atadevice[0][1].Oheads->get ()));
      // AMI BIOS: 2nd hard disk write precompensation cylinder, low byte
      DEV_cmos_set_reg(0x27, 0xff); // -1
      // AMI BIOS: 2nd hard disk write precompensation cylinder, high byte
      DEV_cmos_set_reg(0x28, 0xff); // -1
      // AMI BIOS: 2nd hard disk, 0x80 if heads>8
      DEV_cmos_set_reg(0x29, (bx_options.atadevice[0][1].Oheads->get () > 8) ? 0x80 : 0x00);
      // AMI BIOS: 2nd hard disk landing zone, low byte
      DEV_cmos_set_reg(0x2a, DEV_cmos_get_reg(0x24));
      // AMI BIOS: 2nd hard disk landing zone, high byte
      DEV_cmos_set_reg(0x2b, DEV_cmos_get_reg(0x25));
      // AMI BIOS: 2nd hard disk sectors/track
      DEV_cmos_set_reg(0x2c, bx_options.atadevice[0][1].Ospt->get ());
    }

    DEV_cmos_set_reg(0x39, 0);
    DEV_cmos_set_reg(0x3a, 0);
    for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
      for (Bit8u device=0; device<2; device ++) {
        if (bx_options.atadevice[channel][device].Opresent->get()) {
          if (BX_DRIVE_IS_HD(channel,device)) {
            Bit16u cylinders = bx_options.atadevice[channel][device].Ocylinders->get();
            Bit16u heads = bx_options.atadevice[channel][device].Oheads->get();
            Bit16u spt = bx_options.atadevice[channel][device].Ospt->get();
            Bit32u size = cylinders * heads * spt;
            Bit8u  translation = bx_options.atadevice[channel][device].Otranslation->get();

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
    if ( bx_options.Obootdrive->get () != BX_BOOT_FLOPPYA) {
      // system boot sequence C:, A:
      DEV_cmos_set_reg(0x2d, DEV_cmos_get_reg(0x2d) & 0xdf);
      }
    else { // 'a'
      // system boot sequence A:, C:
      DEV_cmos_set_reg(0x2d, DEV_cmos_get_reg(0x2d) | 0x20);
      }

    // Set the "extended" boot device, byte 0x3D (needed for cdrom booting)
    if ( bx_options.Obootdrive->get () == BX_BOOT_FLOPPYA) {
      // system boot sequence A:
      DEV_cmos_set_reg(0x3d, 0x01);
      BX_INFO(("Boot device will be 'a'"));
      }
    else if ( bx_options.Obootdrive->get () == BX_BOOT_DISKC) { 
      // system boot sequence C:
      DEV_cmos_set_reg(0x3d, 0x02);
      BX_INFO(("Boot device will be 'c'"));
      }
    else if ( bx_options.Obootdrive->get () == BX_BOOT_CDROM) { 
      // system boot sequence cdrom
      DEV_cmos_set_reg(0x3d, 0x03);
      BX_INFO(("Boot device will be 'cdrom'"));
      }
      
    // Set the signature check flag in cmos, inverted for compatibility
    DEV_cmos_set_reg(0x38, bx_options.OfloppySigCheck->get());
    BX_INFO(("Floppy boot signature check is %sabled", bx_options.OfloppySigCheck->get() ? "dis" : "en"));
    }

}

  void
bx_hard_drive_c::reset(unsigned type)
{
  for (unsigned channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (BX_HD_THIS channels[channel].irq)
      DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
  }
}


#define GOTO_RETURN_VALUE  if(io_len==4){\
                             goto return_value32;\
                             }\
                           else if(io_len==2){\
                             value16=(Bit16u)value32;\
                             goto return_value16;\
                             }\
                           else{\
                             value8=(Bit8u)value32;\
                             goto return_value8;\
                             }
                           

  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_hard_drive_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_HD_SMF
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_hard_drive_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_HD_SMF
  Bit8u value8;
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
    BX_PANIC(("Unable to find ATA channel, ioport=0x%04x", address));
    }

#if BX_PDC20230C_VLBIDE_SUPPORT
// pdc20230c is only available for first ata channel
if (channel == 0) {

  // Detect the switch to programming mode
  if (!BX_HD_THIS pdc20230c.prog_mode) {
    switch (port) {
      case 0x02:
        if ((BX_HD_THIS pdc20230c.prog_count == 0) || (BX_HD_THIS pdc20230c.prog_count > 2)) {
          BX_HD_THIS pdc20230c.prog_count++;
        }
	else {
          BX_HD_THIS pdc20230c.prog_count=0;
	}
	break;
      case 0x16:
        if ((BX_HD_THIS pdc20230c.prog_count == 1) || (BX_HD_THIS pdc20230c.prog_count == 2)) {
	  BX_HD_THIS pdc20230c.prog_count++;
	}
	else {
          BX_HD_THIS pdc20230c.prog_count=0;
	}
	break;
      default:
	BX_HD_THIS pdc20230c.prog_count=0;
    }

    if (BX_HD_THIS pdc20230c.prog_count == 5) {
      BX_HD_THIS pdc20230c.prog_mode = 1;
      BX_SELECTED_CONTROLLER(channel).sector_count &= 0x7f;
      BX_INFO(("Promise VLB-IDE DC2300: Switching to Programming mode"));
    }
  }

  // Returns value when in programming mode
  if (BX_HD_THIS pdc20230c.prog_mode) {
    switch (port) {
      case 0x05:
	// Leave programming mode
        BX_HD_THIS pdc20230c.prog_mode = 0;
        BX_INFO(("Promise VLB-IDE DC2300: Leaving Programming mode"));
	// Value will be sent be normal code
        break;
      case 0x03:
	// Special programming register
        value32 = BX_HD_THIS pdc20230c.p1f3_value;
        GOTO_RETURN_VALUE ;
        break;
      case 0x04:
	// Special programming register
        value32 = BX_HD_THIS pdc20230c.p1f4_value;
        GOTO_RETURN_VALUE ;
        break;
    }
  }
}
#endif

  if (io_len>1 && port!=0x00) {
    BX_PANIC(("non-byte IO read to %04x", (unsigned) address));
    }
  
  switch (port) {
    case 0x00: // hard disk data (16bit) 0x1f0
      if (BX_SELECTED_CONTROLLER(channel).status.drq == 0) {
	    BX_ERROR(("IO read(0x%04x) with drq == 0: last command was %02xh",
		     address, (unsigned) BX_SELECTED_CONTROLLER(channel).current_command));
            return(0);
      }
      BX_DEBUG(("IO read(0x%04x): current command is %02xh",
            address, (unsigned) BX_SELECTED_CONTROLLER(channel).current_command));
      switch (BX_SELECTED_CONTROLLER(channel).current_command) {
        case 0x20: // READ SECTORS, with retries
        case 0x21: // READ SECTORS, without retries
          if (io_len == 1) {
            BX_PANIC(("byte IO read from %04x",
                     (unsigned) address));
            }
          if (BX_SELECTED_CONTROLLER(channel).buffer_index >= 512)
            BX_PANIC(("IO read(0x%04x): buffer_index >= 512", address));

#if BX_SupportRepeatSpeedups
          if (DEV_bulk_io_quantum_requested()) {
            unsigned transferLen, quantumsMax;

            quantumsMax =
              (512 - BX_SELECTED_CONTROLLER(channel).buffer_index) / io_len;
            if ( quantumsMax == 0)
              BX_PANIC(("IO read(0x%04x): not enough space for read", address));
            DEV_bulk_io_quantum_transferred() =
                DEV_bulk_io_quantum_requested();
            if (quantumsMax < DEV_bulk_io_quantum_transferred())
              DEV_bulk_io_quantum_transferred() = quantumsMax;
            transferLen = io_len * DEV_bulk_io_quantum_transferred();
            memcpy((Bit8u*) DEV_bulk_io_host_addr(),
              &BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index], 
              transferLen);
            DEV_bulk_io_host_addr() += transferLen;
            BX_SELECTED_CONTROLLER(channel).buffer_index += transferLen;
            value32 = 0; // Value returned not important;
            }
          else
#endif
            {
            value32 = 0L;
            switch(io_len){
              case 4:
                value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+3] << 24);
                value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+2] << 16);
              case 2:
                value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+1] << 8);
                value32 |=  BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index];
              }
            BX_SELECTED_CONTROLLER(channel).buffer_index += io_len;
            }

          // if buffer completely read
          if (BX_SELECTED_CONTROLLER(channel).buffer_index >= 512) {
            // update sector count, sector number, cylinder,
            // drive, head, status
            // if there are more sectors, read next one in...
            //
            BX_SELECTED_CONTROLLER(channel).buffer_index = 0;

	    increment_address(channel);

            BX_SELECTED_CONTROLLER(channel).status.busy = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
            if (bx_options.OnewHardDriveSupport->get ())
              BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
            else
              BX_SELECTED_CONTROLLER(channel).status.seek_complete = 0;
            BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
            BX_SELECTED_CONTROLLER(channel).status.err = 0;

            if (BX_SELECTED_CONTROLLER(channel).sector_count==0) {
              BX_SELECTED_CONTROLLER(channel).status.drq = 0;
              }
            else { /* read next one into controller buffer */
              off_t logical_sector;
              off_t ret;

              BX_SELECTED_CONTROLLER(channel).status.drq = 1;
              BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;

#if TEST_READ_BEYOND_END==1
	      BX_SELECTED_CONTROLLER(channel).cylinder_no += 100000;
#endif
	      if (!calculate_logical_address(channel, &logical_sector)) {
	        BX_ERROR(("multi-sector read reached invalid sector %lu, aborting", logical_sector));
		command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	        GOTO_RETURN_VALUE ;
	      }
	      ret = BX_SELECTED_DRIVE(channel).hard_drive->lseek(logical_sector * 512, SEEK_SET);
              if (ret < 0) {
                BX_ERROR(("could not lseek() hard drive image file"));
		command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	        GOTO_RETURN_VALUE ;
	      }
	      ret = BX_SELECTED_DRIVE(channel).hard_drive->read((bx_ptr_t) BX_SELECTED_CONTROLLER(channel).buffer, 512);
              if (ret < 512) {
                BX_ERROR(("logical sector was %lu", logical_sector));
                BX_ERROR(("could not read() hard drive image file at byte %lu", logical_sector*512));
		command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	        GOTO_RETURN_VALUE ;
	      }

              BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
	      raise_interrupt(channel);
	    }
	  }
	  GOTO_RETURN_VALUE ;
          break;

        case 0xec:    // IDENTIFY DEVICE
	case 0xa1:
          if (bx_options.OnewHardDriveSupport->get ()) {
            unsigned index;

            BX_SELECTED_CONTROLLER(channel).status.busy = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
            BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
            BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
            BX_SELECTED_CONTROLLER(channel).status.err = 0;

            index = BX_SELECTED_CONTROLLER(channel).buffer_index;
            value32 = BX_SELECTED_CONTROLLER(channel).buffer[index];
            index++;
            if (io_len >= 2) {
              value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index] << 8);
              index++;
              }
            if (io_len == 4) {
              value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index] << 16);
              value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index+1] << 24);
              index += 2;
              }
            BX_SELECTED_CONTROLLER(channel).buffer_index = index;

            if (BX_SELECTED_CONTROLLER(channel).buffer_index >= 512) {
              BX_SELECTED_CONTROLLER(channel).status.drq = 0;
	      if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		    BX_INFO(("Read all drive ID Bytes ..."));
              }
            GOTO_RETURN_VALUE;
	  }
          else
            BX_PANIC(("IO read(0x%04x): current command is %02xh", address,
              (unsigned) BX_SELECTED_CONTROLLER(channel).current_command));

	    case 0xa0: {
		  unsigned index = BX_SELECTED_CONTROLLER(channel).buffer_index;
		  unsigned increment = 0;

		  // Load block if necessary
		  if (index >= 2048) {
			if (index > 2048)
			      BX_PANIC(("index > 2048 : 0x%x",index));
			switch (BX_SELECTED_DRIVE(channel).atapi.command) {
			      case 0x28: // read (10)
			      case 0xa8: // read (12)
#ifdef LOWLEVEL_CDROM
				    if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
				      BX_PANIC(("Read with CDROM not ready"));
				    } 
				    BX_SELECTED_DRIVE(channel).cdrom.cd->read_block(BX_SELECTED_CONTROLLER(channel).buffer,
									BX_SELECTED_DRIVE(channel).cdrom.next_lba);
				    BX_SELECTED_DRIVE(channel).cdrom.next_lba++;
				    BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks--;

				    if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
					  if (!BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks)
						BX_INFO(("Last READ block loaded {CDROM}"));
					  else
						BX_INFO(("READ block loaded (%d remaining) {CDROM}",
							  BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks));

				    // one block transfered, start at beginning
				    index = 0;
#else
				    BX_PANIC(("Read with no LOWLEVEL_CDROM"));
#endif
				    break;

			      default: // no need to load a new block
				    break;
			}
		  }

		  value32 = BX_SELECTED_CONTROLLER(channel).buffer[index+increment];
		  increment++;
		  if (io_len >= 2) {
			value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index+increment] << 8);
			increment++;
		  }
		  if (io_len == 4) {
			value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index+increment] << 16);
			value32 |= (BX_SELECTED_CONTROLLER(channel).buffer[index+increment+1] << 24);
			increment += 2;
		  }
		  BX_SELECTED_CONTROLLER(channel).buffer_index = index + increment;
		  BX_SELECTED_CONTROLLER(channel).drq_index += increment;

		  if (BX_SELECTED_CONTROLLER(channel).drq_index >= (unsigned)BX_SELECTED_DRIVE(channel).atapi.drq_bytes) {
			BX_SELECTED_CONTROLLER(channel).status.drq = 0;
			BX_SELECTED_CONTROLLER(channel).drq_index = 0;

			BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining -= BX_SELECTED_DRIVE(channel).atapi.drq_bytes;

			if (BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining > 0) {
			      // one or more blocks remaining (works only for single block commands)
			      if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
				    BX_INFO(("PACKET drq bytes read"));
			      BX_SELECTED_CONTROLLER(channel).interrupt_reason.i_o = 1;
			      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
			      BX_SELECTED_CONTROLLER(channel).status.drq = 1;
			      BX_SELECTED_CONTROLLER(channel).interrupt_reason.c_d = 0;

			      // set new byte count if last block
			      if (BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining < BX_SELECTED_CONTROLLER(channel).byte_count) {
				    BX_SELECTED_CONTROLLER(channel).byte_count = BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining;
			      }
			      BX_SELECTED_DRIVE(channel).atapi.drq_bytes = BX_SELECTED_CONTROLLER(channel).byte_count;

			      raise_interrupt(channel);
			} else {
			      // all bytes read
			      if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
				    BX_INFO(("PACKET all bytes read"));
			      BX_SELECTED_CONTROLLER(channel).interrupt_reason.i_o = 1;
			      BX_SELECTED_CONTROLLER(channel).interrupt_reason.c_d = 1;
			      BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
			      BX_SELECTED_CONTROLLER(channel).interrupt_reason.rel = 0;
			      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
			      BX_SELECTED_CONTROLLER(channel).status.drq = 0;
			      BX_SELECTED_CONTROLLER(channel).status.err = 0;
			      
			      raise_interrupt(channel);
			}
		  }
                  GOTO_RETURN_VALUE;
		  break;
	    }

	// List all the read operations that are defined in the ATA/ATAPI spec
	// that we don't support.  Commands that are listed here will cause a
	// BX_ERROR, which is non-fatal, and the command will be aborted.
	case 0x08: BX_ERROR(("read cmd 0x08 (DEVICE RESET) not supported")); command_aborted(channel, 0x08); break;
	case 0x10: BX_ERROR(("read cmd 0x10 (RECALIBRATE) not supported")); command_aborted(channel, 0x10); break;
	case 0x22: BX_ERROR(("read cmd 0x22 (READ LONG) not supported")); command_aborted(channel, 0x22); break;
	case 0x23: BX_ERROR(("read cmd 0x23 (READ LONG NO RETRY) not supported")); command_aborted(channel, 0x23); break;
	case 0x24: BX_ERROR(("read cmd 0x24 (READ SECTORS EXT) not supported")); command_aborted(channel, 0x24); break;
	case 0x25: BX_ERROR(("read cmd 0x25 (READ DMA EXT) not supported")); command_aborted(channel, 0x25); break;
	case 0x26: BX_ERROR(("read cmd 0x26 (READ DMA QUEUED EXT) not supported")); command_aborted(channel, 0x26); break;
	case 0x27: BX_ERROR(("read cmd 0x27 (READ NATIVE MAX ADDRESS EXT) not supported")); command_aborted(channel, 0x27); break;
	case 0x29: BX_ERROR(("read cmd 0x29 (READ MULTIPLE EXT) not supported")); command_aborted(channel, 0x29); break;
	case 0x2A: BX_ERROR(("read cmd 0x2A (READ STREAM DMA) not supported")); command_aborted(channel, 0x2A); break;
	case 0x2B: BX_ERROR(("read cmd 0x2B (READ STREAM PIO) not supported")); command_aborted(channel, 0x2B); break;
	case 0x2F: BX_ERROR(("read cmd 0x2F (READ LOG EXT) not supported")); command_aborted(channel, 0x2F); break;
	case 0x30: BX_ERROR(("read cmd 0x30 (WRITE SECTORS) not supported")); command_aborted(channel, 0x30); break;
	case 0x31: BX_ERROR(("read cmd 0x31 (WRITE SECTORS NO RETRY) not supported")); command_aborted(channel, 0x31); break;
	case 0x32: BX_ERROR(("read cmd 0x32 (WRITE LONG) not supported")); command_aborted(channel, 0x32); break;
	case 0x33: BX_ERROR(("read cmd 0x33 (WRITE LONG NO RETRY) not supported")); command_aborted(channel, 0x33); break;
	case 0x34: BX_ERROR(("read cmd 0x34 (WRITE SECTORS EXT) not supported")); command_aborted(channel, 0x34); break;
	case 0x35: BX_ERROR(("read cmd 0x35 (WRITE DMA EXT) not supported")); command_aborted(channel, 0x35); break;
	case 0x36: BX_ERROR(("read cmd 0x36 (WRITE DMA QUEUED EXT) not supported")); command_aborted(channel, 0x36); break;
	case 0x37: BX_ERROR(("read cmd 0x37 (SET MAX ADDRESS EXT) not supported")); command_aborted(channel, 0x37); break;
	case 0x38: BX_ERROR(("read cmd 0x38 (CFA WRITE SECTORS W/OUT ERASE) not supported")); command_aborted(channel, 0x38); break;
	case 0x39: BX_ERROR(("read cmd 0x39 (WRITE MULTIPLE EXT) not supported")); command_aborted(channel, 0x39); break;
	case 0x3A: BX_ERROR(("read cmd 0x3A (WRITE STREAM DMA) not supported")); command_aborted(channel, 0x3A); break;
	case 0x3B: BX_ERROR(("read cmd 0x3B (WRITE STREAM PIO) not supported")); command_aborted(channel, 0x3B); break;
	case 0x3F: BX_ERROR(("read cmd 0x3F (WRITE LOG EXT) not supported")); command_aborted(channel, 0x3F); break;
	case 0x40: BX_ERROR(("read cmd 0x40 (READ VERIFY SECTORS) not supported")); command_aborted(channel, 0x40); break;
	case 0x41: BX_ERROR(("read cmd 0x41 (READ VERIFY SECTORS NO RETRY) not supported")); command_aborted(channel, 0x41); break;
	case 0x42: BX_ERROR(("read cmd 0x42 (READ VERIFY SECTORS EXT) not supported")); command_aborted(channel, 0x42); break;
	case 0x50: BX_ERROR(("read cmd 0x50 (FORMAT TRACK) not supported")); command_aborted(channel, 0x50); break;
	case 0x51: BX_ERROR(("read cmd 0x51 (CONFIGURE STREAM) not supported")); command_aborted(channel, 0x51); break;
	case 0x70: BX_ERROR(("read cmd 0x70 (SEEK) not supported")); command_aborted(channel, 0x70); break;
	case 0x87: BX_ERROR(("read cmd 0x87 (CFA TRANSLATE SECTOR) not supported")); command_aborted(channel, 0x87); break;
	case 0x90: BX_ERROR(("read cmd 0x90 (EXECUTE DEVICE DIAGNOSTIC) not supported")); command_aborted(channel, 0x90); break;
	case 0x91: BX_ERROR(("read cmd 0x91 (INITIALIZE DEVICE PARAMETERS) not supported")); command_aborted(channel, 0x91); break;
	case 0x92: BX_ERROR(("read cmd 0x92 (DOWNLOAD MICROCODE) not supported")); command_aborted(channel, 0x92); break;
	case 0x94: BX_ERROR(("read cmd 0x94 (STANDBY IMMEDIATE) not supported")); command_aborted(channel, 0x94); break;
	case 0x95: BX_ERROR(("read cmd 0x95 (IDLE IMMEDIATE) not supported")); command_aborted(channel, 0x95); break;
	case 0x96: BX_ERROR(("read cmd 0x96 (STANDBY) not supported")); command_aborted(channel, 0x96); break;
	case 0x97: BX_ERROR(("read cmd 0x97 (IDLE) not supported")); command_aborted(channel, 0x97); break;
	case 0x98: BX_ERROR(("read cmd 0x98 (CHECK POWER MODE) not supported")); command_aborted(channel, 0x98); break;
	case 0x99: BX_ERROR(("read cmd 0x99 (SLEEP) not supported")); command_aborted(channel, 0x99); break;
	case 0xA2: BX_ERROR(("read cmd 0xA2 (SERVICE) not supported")); command_aborted(channel, 0xA2); break;
	case 0xB0: BX_ERROR(("read cmd 0xB0 (SMART DISABLE OPERATIONS) not supported")); command_aborted(channel, 0xB0); break;
	case 0xB1: BX_ERROR(("read cmd 0xB1 (DEVICE CONFIGURATION FREEZE LOCK) not supported")); command_aborted(channel, 0xB1); break;
	case 0xC0: BX_ERROR(("read cmd 0xC0 (CFA ERASE SECTORS) not supported")); command_aborted(channel, 0xC0); break;
	case 0xC4: BX_ERROR(("read cmd 0xC4 (READ MULTIPLE) not supported")); command_aborted(channel, 0xC4); break;
	case 0xC5: BX_ERROR(("read cmd 0xC5 (WRITE MULTIPLE) not supported")); command_aborted(channel, 0xC5); break;
	case 0xC6: BX_ERROR(("read cmd 0xC6 (SET MULTIPLE MODE) not supported")); command_aborted(channel, 0xC6); break;
	case 0xC7: BX_ERROR(("read cmd 0xC7 (READ DMA QUEUED) not supported")); command_aborted(channel, 0xC7); break;
	case 0xC8: BX_ERROR(("read cmd 0xC8 (READ DMA) not supported")); command_aborted(channel, 0xC8); break;
	case 0xC9: BX_ERROR(("read cmd 0xC9 (READ DMA NO RETRY) not supported")); command_aborted(channel, 0xC9); break;
	case 0xCA: BX_ERROR(("read cmd 0xCA (WRITE DMA) not supported")); command_aborted(channel, 0xCA); break;
	case 0xCC: BX_ERROR(("read cmd 0xCC (WRITE DMA QUEUED) not supported")); command_aborted(channel, 0xCC); break;
	case 0xCD: BX_ERROR(("read cmd 0xCD (CFA WRITE MULTIPLE W/OUT ERASE) not supported")); command_aborted(channel, 0xCD); break;
	case 0xD1: BX_ERROR(("read cmd 0xD1 (CHECK MEDIA CARD TYPE) not supported")); command_aborted(channel, 0xD1); break;
	case 0xDA: BX_ERROR(("read cmd 0xDA (GET MEDIA STATUS) not supported")); command_aborted(channel, 0xDA); break;
	case 0xDE: BX_ERROR(("read cmd 0xDE (MEDIA LOCK) not supported")); command_aborted(channel, 0xDE); break;
	case 0xDF: BX_ERROR(("read cmd 0xDF (MEDIA UNLOCK) not supported")); command_aborted(channel, 0xDF); break;
	case 0xE0: BX_ERROR(("read cmd 0xE0 (STANDBY IMMEDIATE) not supported")); command_aborted(channel, 0xE0); break;
	case 0xE1: BX_ERROR(("read cmd 0xE1 (IDLE IMMEDIATE) not supported")); command_aborted(channel, 0xE1); break;
	case 0xE2: BX_ERROR(("read cmd 0xE2 (STANDBY) not supported")); command_aborted(channel, 0xE2); break;
	case 0xE3: BX_ERROR(("read cmd 0xE3 (IDLE) not supported")); command_aborted(channel, 0xE3); break;
	case 0xE4: BX_ERROR(("read cmd 0xE4 (READ BUFFER) not supported")); command_aborted(channel, 0xE4); break;
	case 0xE5: BX_ERROR(("read cmd 0xE5 (CHECK POWER MODE) not supported")); command_aborted(channel, 0xE5); break;
	case 0xE6: BX_ERROR(("read cmd 0xE6 (SLEEP) not supported")); command_aborted(channel, 0xE6); break;
	case 0xE7: BX_ERROR(("read cmd 0xE7 (FLUSH CACHE) not supported")); command_aborted(channel, 0xE7); break;
	case 0xE8: BX_ERROR(("read cmd 0xE8 (WRITE BUFFER) not supported")); command_aborted(channel, 0xE8); break;
	case 0xEA: BX_ERROR(("read cmd 0xEA (FLUSH CACHE EXT) not supported")); command_aborted(channel, 0xEA); break;
	case 0xED: BX_ERROR(("read cmd 0xED (MEDIA EJECT) not supported")); command_aborted(channel, 0xED); break;
	case 0xEF: BX_ERROR(("read cmd 0xEF (SET FEATURES) not supported")); command_aborted(channel, 0xEF); break;
	case 0xF1: BX_ERROR(("read cmd 0xF1 (SECURITY SET PASSWORD) not supported")); command_aborted(channel, 0xF1); break;
	case 0xF2: BX_ERROR(("read cmd 0xF2 (SECURITY UNLOCK) not supported")); command_aborted(channel, 0xF2); break;
	case 0xF3: BX_ERROR(("read cmd 0xF3 (SECURITY ERASE PREPARE) not supported")); command_aborted(channel, 0xF3); break;
	case 0xF4: BX_ERROR(("read cmd 0xF4 (SECURITY ERASE UNIT) not supported")); command_aborted(channel, 0xF4); break;
	case 0xF5: BX_ERROR(("read cmd 0xF5 (SECURITY FREEZE LOCK) not supported")); command_aborted(channel, 0xF5); break;
	case 0xF6: BX_ERROR(("read cmd 0xF6 (SECURITY DISABLE PASSWORD) not supported")); command_aborted(channel, 0xF6); break;
	case 0xF8: BX_ERROR(("read cmd 0xF8 (READ NATIVE MAX ADDRESS) not supported")); command_aborted(channel, 0xF8); break;
	case 0xF9: BX_ERROR(("read cmd 0xF9 (SET MAX ADDRESS) not supported")); command_aborted(channel, 0xF9); break;

        default:
          BX_PANIC(("IO read(0x%04x): current command is %02xh", address,
            (unsigned) BX_SELECTED_CONTROLLER(channel).current_command));
        }
      break;

    case 0x01: // hard disk error register 0x1f1
      BX_SELECTED_CONTROLLER(channel).status.err = 0;
      value8 = (!BX_SELECTED_IS_PRESENT(channel)) ? 0 : BX_SELECTED_CONTROLLER(channel).error_register;
      goto return_value8;
      break;
    case 0x02: // hard disk sector count / interrupt reason 0x1f2
      value8 = (!BX_SELECTED_IS_PRESENT(channel)) ? 0 : BX_SELECTED_CONTROLLER(channel).sector_count;
      goto return_value8;
      break;
    case 0x03: // sector number 0x1f3
      value8 = (!BX_SELECTED_IS_PRESENT(channel)) ? 0 : BX_SELECTED_CONTROLLER(channel).sector_no;
      goto return_value8;
    case 0x04: // cylinder low 0x1f4  
               // -- WARNING : On real hardware the controller registers are shared between drives. 
               // So we must respond even if the select device is not present. Some OS uses this fact 
               // to detect the disks.... minix2 for example
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : (BX_SELECTED_CONTROLLER(channel).cylinder_no & 0x00ff);
      goto return_value8;
    case 0x05: // cylinder high 0x1f5
               // -- WARNING : On real hardware the controller registers are shared between drives. 
               // So we must respond even if the select device is not present. Some OS uses this fact 
               // to detect the disks.... minix2 for example
      value8 = (!BX_ANY_IS_PRESENT(channel)) ? 0 : BX_SELECTED_CONTROLLER(channel).cylinder_no >> 8;
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
               ((BX_SELECTED_CONTROLLER(channel).lba_mode>0) << 6) |
               (1 << 5) | // 01b = 512 sector size
               (BX_HD_THIS channels[channel].drive_select << 4) |
               (BX_SELECTED_CONTROLLER(channel).head_no << 0);
      goto return_value8;
      break;
//BX_CONTROLLER(channel,0).lba_mode

    case 0x07: // Hard Disk Status 0x1f7
    case 0x16: // Hard Disk Alternate Status 0x3f6
      if (!BX_ANY_IS_PRESENT(channel)) {
	    // (mch) Just return zero for these registers
	    value8 = 0;
      } else {
      value8 = (
        (BX_SELECTED_CONTROLLER(channel).status.busy << 7) |
        (BX_SELECTED_CONTROLLER(channel).status.drive_ready << 6) |
        (BX_SELECTED_CONTROLLER(channel).status.write_fault << 5) |
        (BX_SELECTED_CONTROLLER(channel).status.seek_complete << 4) |
        (BX_SELECTED_CONTROLLER(channel).status.drq << 3) |
        (BX_SELECTED_CONTROLLER(channel).status.corrected_data << 2) |
        (BX_SELECTED_CONTROLLER(channel).status.index_pulse << 1) |
        (BX_SELECTED_CONTROLLER(channel).status.err) );
      BX_SELECTED_CONTROLLER(channel).status.index_pulse_count++;
      BX_SELECTED_CONTROLLER(channel).status.index_pulse = 0;
      if (BX_SELECTED_CONTROLLER(channel).status.index_pulse_count >= INDEX_PULSE_CYCLE) {
        BX_SELECTED_CONTROLLER(channel).status.index_pulse = 1;
        BX_SELECTED_CONTROLLER(channel).status.index_pulse_count = 0;
        }
      }
      if (port == 0x07) {
        DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
        }
      goto return_value8;
      break;

    case 0x17: // Hard Disk Address Register 0x3f7
      // Obsolete and unsupported register.  Not driven by hard
      // disk controller.  Report all 1's.  If floppy controller
      // is handling this address, it will call this function
      // set/clear D7 (the only bit it handles), then return
      // the combined value
      value8 = 0xff;
      goto return_value8;
      break;

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

  void
bx_hard_drive_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_HD_SMF
  bx_hard_drive_c *class_ptr = (bx_hard_drive_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_hard_drive_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_HD_SMF
  off_t logical_sector;
  off_t ret;
  bx_bool prev_control_reset;

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
    BX_PANIC(("Unable to find ATA channel, ioport=0x%04x", address));
    }

#if BX_PDC20230C_VLBIDE_SUPPORT
// pdc20230c is only available for first ata channel
if (channel == 0) {
  BX_HD_THIS pdc20230c.prog_count = 0;

  if (BX_HD_THIS pdc20230c.prog_mode != 0) {
    switch (port) {
      case 0x03:
	BX_HD_THIS pdc20230c.p1f3_value = value;
	return;
        break;
      case 0x04:
	BX_HD_THIS pdc20230c.p1f4_value = value;
	return;
        break;
    }
  }
}
#endif

  if (io_len>1 && port!=0x00) {
    BX_PANIC(("non-byte IO write to %04x", (unsigned) address));
    }

  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom)) {
	switch (io_len) {
	      case 1:
		    BX_INFO(("8-bit write to %04x = %02x {%s}",
			      (unsigned) address, (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
		    break;
		    
	      case 2:
		    BX_INFO(("16-bit write to %04x = %04x {%s}",
			      (unsigned) address, (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
		    break;

	      case 4:
		    BX_INFO(("32-bit write to %04x = %08x {%s}",
			      (unsigned) address, (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
		    break;

	      default:
		    BX_INFO(("unknown-size write to %04x = %08x {%s}",
			      (unsigned) address, (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
		    break;
	}
  }

BX_DEBUG(("IO write to %04x = %02x", (unsigned) address, (unsigned) value));

  switch (port) {
    case 0x00: // 0x1f0
      if (io_len == 1) {
        BX_PANIC(("byte IO write to 0x%04x", address));
        }
      switch (BX_SELECTED_CONTROLLER(channel).current_command) {
        case 0x30: // WRITE SECTORS
          if (BX_SELECTED_CONTROLLER(channel).buffer_index >= 512)
            BX_PANIC(("IO write(0x%04x): buffer_index >= 512", address));

#if BX_SupportRepeatSpeedups
          if (DEV_bulk_io_quantum_requested()) {
            unsigned transferLen, quantumsMax;

            quantumsMax =
              (512 - BX_SELECTED_CONTROLLER(channel).buffer_index) / io_len;
            if ( quantumsMax == 0)
              BX_PANIC(("IO write(0x%04x): not enough space for write", address));
            DEV_bulk_io_quantum_transferred() =
                DEV_bulk_io_quantum_requested();
            if (quantumsMax < DEV_bulk_io_quantum_transferred())
              DEV_bulk_io_quantum_transferred() = quantumsMax;
            transferLen = io_len * DEV_bulk_io_quantum_transferred();
            memcpy(
              &BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index], 
              (Bit8u*) DEV_bulk_io_host_addr(),
              transferLen);
	    DEV_bulk_io_host_addr() += transferLen;
            BX_SELECTED_CONTROLLER(channel).buffer_index += transferLen;
            }
          else
#endif
            {
            switch(io_len){
              case 4:
                BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+3] = (Bit8u)(value >> 24);
                BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+2] = (Bit8u)(value >> 16);
              case 2:
                BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+1] = (Bit8u)(value >> 8);
                BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index]   = (Bit8u) value;
              }
            BX_SELECTED_CONTROLLER(channel).buffer_index += io_len;
            }

          /* if buffer completely writtten */
          if (BX_SELECTED_CONTROLLER(channel).buffer_index >= 512) {
            off_t logical_sector;
            off_t ret;

#if TEST_WRITE_BEYOND_END==1
	    BX_SELECTED_CONTROLLER(channel).cylinder_no += 100000;
#endif
	    if (!calculate_logical_address(channel, &logical_sector)) {
	      BX_ERROR(("write reached invalid sector %lu, aborting", logical_sector));
	      command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	      return;
            }
#if TEST_WRITE_BEYOND_END==2
	    logical_sector += 100000;
#endif
	    ret = BX_SELECTED_DRIVE(channel).hard_drive->lseek(logical_sector * 512, SEEK_SET);
            if (ret < 0) {
              BX_ERROR(("could not lseek() hard drive image file at byte %lu", logical_sector * 512));
	      command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	      return;
	    }
	    ret = BX_SELECTED_DRIVE(channel).hard_drive->write((bx_ptr_t) BX_SELECTED_CONTROLLER(channel).buffer, 512);
            if (ret < 512) {
              BX_ERROR(("could not write() hard drive image file at byte %lu", logical_sector*512));
	      command_aborted (channel, BX_SELECTED_CONTROLLER(channel).current_command);
	      return;
	    }

            BX_SELECTED_CONTROLLER(channel).buffer_index = 0;

            /* update sector count, sector number, cylinder,
             * drive, head, status
             * if there are more sectors, read next one in...
             */

	    increment_address(channel);

            /* When the write is complete, controller clears the DRQ bit and
             * sets the BSY bit.
             * If at least one more sector is to be written, controller sets DRQ bit,
             * clears BSY bit, and issues IRQ 
             */

            if (BX_SELECTED_CONTROLLER(channel).sector_count!=0) {
              BX_SELECTED_CONTROLLER(channel).status.busy = 0;
              BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
              BX_SELECTED_CONTROLLER(channel).status.drq = 1;
              BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
              BX_SELECTED_CONTROLLER(channel).status.err = 0;
              }
            else { /* no more sectors to write */
              BX_SELECTED_CONTROLLER(channel).status.busy = 0;
              BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
              BX_SELECTED_CONTROLLER(channel).status.drq = 0;
              BX_SELECTED_CONTROLLER(channel).status.err = 0;
              BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
              }
	    raise_interrupt(channel);
            }
          break;

	    case 0xa0: // PACKET
		  if (BX_SELECTED_CONTROLLER(channel).buffer_index >= PACKET_SIZE)
			BX_PANIC(("IO write(0x%04x): buffer_index >= PACKET_SIZE", address));
		  BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index] = value;
		  BX_SELECTED_CONTROLLER(channel).buffer[BX_SELECTED_CONTROLLER(channel).buffer_index+1] = (value >> 8);
		  BX_SELECTED_CONTROLLER(channel).buffer_index += 2;

		  /* if packet completely writtten */
		  if (BX_SELECTED_CONTROLLER(channel).buffer_index >= PACKET_SIZE) {
			// complete command received
			Bit8u atapi_command = BX_SELECTED_CONTROLLER(channel).buffer[0];

			if (bx_dbg.cdrom)
				BX_INFO(("cdrom: ATAPI command 0x%x started", atapi_command));

			switch (atapi_command) {
			      case 0x00: // test unit ready
				    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  atapi_cmd_nop(channel);
				    } else {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
				    }
				    raise_interrupt(channel);
				    break;

			      case 0x03: { // request sense
				    int alloc_length = BX_SELECTED_CONTROLLER(channel).buffer[4];
				    init_send_atapi_command(channel, atapi_command, 18, alloc_length);

				    // sense data
				    BX_SELECTED_CONTROLLER(channel).buffer[0] = 0x70 | (1 << 7);
				    BX_SELECTED_CONTROLLER(channel).buffer[1] = 0;
				    BX_SELECTED_CONTROLLER(channel).buffer[2] = BX_SELECTED_DRIVE(channel).sense.sense_key;
				    BX_SELECTED_CONTROLLER(channel).buffer[3] = BX_SELECTED_DRIVE(channel).sense.information.arr[0];
				    BX_SELECTED_CONTROLLER(channel).buffer[4] = BX_SELECTED_DRIVE(channel).sense.information.arr[1];
				    BX_SELECTED_CONTROLLER(channel).buffer[5] = BX_SELECTED_DRIVE(channel).sense.information.arr[2];
				    BX_SELECTED_CONTROLLER(channel).buffer[6] = BX_SELECTED_DRIVE(channel).sense.information.arr[3];
				    BX_SELECTED_CONTROLLER(channel).buffer[7] = 17-7;
				    BX_SELECTED_CONTROLLER(channel).buffer[8] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[0];
				    BX_SELECTED_CONTROLLER(channel).buffer[9] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[1];
				    BX_SELECTED_CONTROLLER(channel).buffer[10] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[2];
				    BX_SELECTED_CONTROLLER(channel).buffer[11] = BX_SELECTED_DRIVE(channel).sense.specific_inf.arr[3];
				    BX_SELECTED_CONTROLLER(channel).buffer[12] = BX_SELECTED_DRIVE(channel).sense.asc;
				    BX_SELECTED_CONTROLLER(channel).buffer[13] = BX_SELECTED_DRIVE(channel).sense.ascq;
				    BX_SELECTED_CONTROLLER(channel).buffer[14] = BX_SELECTED_DRIVE(channel).sense.fruc;
				    BX_SELECTED_CONTROLLER(channel).buffer[15] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[0];
				    BX_SELECTED_CONTROLLER(channel).buffer[16] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[1];
				    BX_SELECTED_CONTROLLER(channel).buffer[17] = BX_SELECTED_DRIVE(channel).sense.key_spec.arr[2];

				    ready_to_send_atapi(channel);
			      }
			      break;
			      
			      case 0x1b: { // start stop unit
				    //bx_bool Immed = (BX_SELECTED_CONTROLLER(channel).buffer[1] >> 0) & 1;
				    bx_bool LoEj = (BX_SELECTED_CONTROLLER(channel).buffer[4] >> 1) & 1;
				    bx_bool Start = (BX_SELECTED_CONTROLLER(channel).buffer[4] >> 0) & 1;

				    if (!LoEj && !Start) { // stop the disc
					  BX_ERROR(("FIXME: Stop disc not implemented"));
					  atapi_cmd_nop(channel);
					  raise_interrupt(channel);
				    } else if (!LoEj && Start) { // start the disc and read the TOC
					  // BX_PANIC(("Start disc not implemented"));
					  BX_ERROR(("FIXME: ATAPI start disc not reading TOC"));
					  atapi_cmd_nop(channel);
					  raise_interrupt(channel);
				    } else if (LoEj && !Start) { // Eject the disc
                                          atapi_cmd_nop(channel);

                                          if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
#ifdef LOWLEVEL_CDROM
					    BX_SELECTED_DRIVE(channel).cdrom.cd->eject_cdrom();
#endif
					    BX_SELECTED_DRIVE(channel).cdrom.ready = 0;
                                            bx_options.atadevice[channel][BX_SLAVE_SELECTED(channel)].Ostatus->set(BX_EJECTED);
                                            bx_gui->update_drive_status_buttons();
                                          }
                                          raise_interrupt(channel);
				    } else { // Load the disc
					  // My guess is that this command only closes the tray, that's a no-op for us
					  atapi_cmd_nop(channel);
					  raise_interrupt(channel);
				    }
			      }
			      break;

			      case 0xbd: { // mechanism status
				    uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER(channel).buffer + 8);

				    if (alloc_length == 0)
					  BX_PANIC(("Zero allocation length to MECHANISM STATUS not impl."));

				    init_send_atapi_command(channel, atapi_command, 8, alloc_length);

				    BX_SELECTED_CONTROLLER(channel).buffer[0] = 0; // reserved for non changers
				    BX_SELECTED_CONTROLLER(channel).buffer[1] = 0; // reserved for non changers

				    BX_SELECTED_CONTROLLER(channel).buffer[2] = 0; // Current LBA (TODO!)
				    BX_SELECTED_CONTROLLER(channel).buffer[3] = 0; // Current LBA (TODO!)
				    BX_SELECTED_CONTROLLER(channel).buffer[4] = 0; // Current LBA (TODO!)

				    BX_SELECTED_CONTROLLER(channel).buffer[5] = 1; // one slot

				    BX_SELECTED_CONTROLLER(channel).buffer[6] = 0; // slot table length
				    BX_SELECTED_CONTROLLER(channel).buffer[7] = 0; // slot table length

				    ready_to_send_atapi(channel);
			      }
			      break;

			      case 0x5a: { // mode sense
				    uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER(channel).buffer + 7);

				    Bit8u PC = BX_SELECTED_CONTROLLER(channel).buffer[2] >> 6;
				    Bit8u PageCode = BX_SELECTED_CONTROLLER(channel).buffer[2] & 0x3f;

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
							      init_mode_sense_single(channel, &BX_SELECTED_CONTROLLER(channel).buffer[8], 28);
							      BX_SELECTED_CONTROLLER(channel).buffer[8] = 0x2a;
							      BX_SELECTED_CONTROLLER(channel).buffer[9] = 0x12;
							      BX_SELECTED_CONTROLLER(channel).buffer[10] = 0x00;
							      BX_SELECTED_CONTROLLER(channel).buffer[11] = 0x00;
							      // Multisession, Mode 2 Form 2, Mode 2 Form 1
							      BX_SELECTED_CONTROLLER(channel).buffer[12] = 0x70; 
							      BX_SELECTED_CONTROLLER(channel).buffer[13] = (3 << 5);
							      BX_SELECTED_CONTROLLER(channel).buffer[14] = (unsigned char)
(1 |
								      (BX_SELECTED_DRIVE(channel).cdrom.locked ? (1 << 1) : 0) |
								      (1 << 3) |
								      (1 << 5));
							      BX_SELECTED_CONTROLLER(channel).buffer[15] = 0x00;
							      BX_SELECTED_CONTROLLER(channel).buffer[16] = (706 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[17] = 706 & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[18] = 0;
							      BX_SELECTED_CONTROLLER(channel).buffer[19] = 2;
							      BX_SELECTED_CONTROLLER(channel).buffer[20] = (512 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[21] = 512 & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[22] = (706 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[23] = 706 & 0xff;
							      BX_SELECTED_CONTROLLER(channel).buffer[24] = 0;
							      BX_SELECTED_CONTROLLER(channel).buffer[25] = 0;
							      BX_SELECTED_CONTROLLER(channel).buffer[26] = 0;
							      BX_SELECTED_CONTROLLER(channel).buffer[27] = 0;
							      ready_to_send_atapi(channel);
							      break;

						      case 0x0d: // CD-ROM
						      case 0x0e: // CD-ROM audio control
						      case 0x3f: // all
							    BX_ERROR(("cdrom: MODE SENSE (curr), code=%x"
								      " not implemented yet",
								     PageCode));
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt(channel);
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device",
								      PC, PageCode));
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
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
							    BX_ERROR(("cdrom: MODE SENSE (chg), code=%x"
								      " not implemented yet",
								     PageCode));
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt(channel);
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device",
								      PC, PageCode));
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt(channel);
							    break;
						}
						break;

					  case 0x2: // default values
						switch (PageCode) {
						      case 0x01: // error recovery
						      case 0x0d: // CD-ROM
						      case 0x0e: // CD-ROM audio control
						      case 0x2a: // CD-ROM capabilities & mech. status
						      case 0x3f: // all
							    BX_PANIC(("cdrom: MODE SENSE (dflt), code=%x",
								     PageCode));
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device",
								      PC, PageCode));
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt(channel);
							    break;
						}
						break;

					  case 0x3: // saved values not implemented
						atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_SAVING_PARAMETERS_NOT_SUPPORTED);
						raise_interrupt(channel);
						break;

					  default:
						BX_PANIC(("Should not get here!"));
						break;
				    }
			      }
			      break;

			      case 0x12: { // inquiry
				    uint8 alloc_length = BX_SELECTED_CONTROLLER(channel).buffer[4];

				    init_send_atapi_command(channel, atapi_command, 36, alloc_length);

				    BX_SELECTED_CONTROLLER(channel).buffer[0] = 0x05; // CD-ROM
				    BX_SELECTED_CONTROLLER(channel).buffer[1] = 0x80; // Removable
				    BX_SELECTED_CONTROLLER(channel).buffer[2] = 0x00; // ISO, ECMA, ANSI version
				    BX_SELECTED_CONTROLLER(channel).buffer[3] = 0x21; // ATAPI-2, as specified
				    BX_SELECTED_CONTROLLER(channel).buffer[4] = 31; // additional length (total 36)
				    BX_SELECTED_CONTROLLER(channel).buffer[5] = 0x00; // reserved
				    BX_SELECTED_CONTROLLER(channel).buffer[6] = 0x00; // reserved
				    BX_SELECTED_CONTROLLER(channel).buffer[7] = 0x00; // reserved

				    // Vendor ID
				    const char* vendor_id = "VTAB    ";
                                    int i;
				    for (i = 0; i < 8; i++)
					  BX_SELECTED_CONTROLLER(channel).buffer[8+i] = vendor_id[i];

				    // Product ID
				    const char* product_id = "Turbo CD-ROM    ";
				    for (i = 0; i < 16; i++)
					  BX_SELECTED_CONTROLLER(channel).buffer[16+i] = product_id[i];

				    // Product Revision level
				    const char* rev_level = "1.0 ";
				    for (i = 0; i < 4; i++)
					  BX_SELECTED_CONTROLLER(channel).buffer[32+i] = rev_level[i];

				    ready_to_send_atapi(channel);
			      }
			      break;

			      case 0x25: { // read cd-rom capacity
				    // no allocation length???
				    init_send_atapi_command(channel, atapi_command, 8, 8);

				    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  uint32 capacity = BX_SELECTED_DRIVE(channel).cdrom.capacity;
					  BX_INFO(("Capacity is %d sectors (%d bytes)", capacity, capacity * 2048));
					  BX_SELECTED_CONTROLLER(channel).buffer[0] = (capacity >> 24) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[1] = (capacity >> 16) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[2] = (capacity >> 8) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[3] = (capacity >> 0) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[4] = (2048 >> 24) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[5] = (2048 >> 16) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[6] = (2048 >> 8) & 0xff;
					  BX_SELECTED_CONTROLLER(channel).buffer[7] = (2048 >> 0) & 0xff;
					  ready_to_send_atapi(channel);
				    } else {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt(channel);
				    }
			      }
			      break;

			      case 0xbe: { // read cd
				    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  BX_ERROR(("Read CD with CD present not implemented"));
					  atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET);
					  raise_interrupt(channel);
				    } else {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt(channel);
				    }
			      }
			      break;

			      case 0x43: { // read toc
				    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
#ifdef LOWLEVEL_CDROM
					  bool msf = (BX_SELECTED_CONTROLLER(channel).buffer[1] >> 1) & 1;
					  uint8 starting_track = BX_SELECTED_CONTROLLER(channel).buffer[6];
#endif
					  uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER(channel).buffer + 7);

					  uint8 format = (BX_SELECTED_CONTROLLER(channel).buffer[9] >> 6);
                                          int i;
					  switch (format) {
						case 0:
#ifdef LOWLEVEL_CDROM
						      int toc_length;
						      if (!(BX_SELECTED_DRIVE(channel).cdrom.cd->read_toc(BX_SELECTED_CONTROLLER(channel).buffer,
										       &toc_length, msf, starting_track))) {
							    atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt(channel);
						      } else {
							    init_send_atapi_command(channel, atapi_command, toc_length, alloc_length);
							    ready_to_send_atapi(channel);
						      }
#else
						      BX_PANIC(("LOWLEVEL_CDROM not defined"));
#endif
						      break;

						case 1:
						      // multi session stuff. we ignore this and emulate a single session only
						      init_send_atapi_command(channel, atapi_command, 12, alloc_length);

						      BX_SELECTED_CONTROLLER(channel).buffer[0] = 0;
						      BX_SELECTED_CONTROLLER(channel).buffer[1] = 0x0a;
						      BX_SELECTED_CONTROLLER(channel).buffer[2] = 1;
						      BX_SELECTED_CONTROLLER(channel).buffer[3] = 1;
						      for (i = 0; i < 8; i++)
							    BX_SELECTED_CONTROLLER(channel).buffer[4+i] = 0;

						      ready_to_send_atapi(channel);
						      break;

						case 2:
						default:
						      BX_PANIC(("(READ TOC) Format %d not supported", format));
						      break;
					  }
				    } else {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt(channel);
				    }
			      }
			      break;

			      case 0x28: // read (10)
			      case 0xa8: // read (12)
			                 { 

				    uint32 transfer_length;
				    if (atapi_command == 0x28)
				          transfer_length = read_16bit(BX_SELECTED_CONTROLLER(channel).buffer + 7);
				    else
				          transfer_length = read_32bit(BX_SELECTED_CONTROLLER(channel).buffer + 6);

				    uint32 lba = read_32bit(BX_SELECTED_CONTROLLER(channel).buffer + 2);

				    if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt(channel);
					  break;
				    }

				    if (transfer_length == 0) {
					  atapi_cmd_nop(channel);
					  raise_interrupt(channel);
					  BX_INFO(("READ(%d) with transfer length 0, ok", atapi_command==0x28?10:12));
					  break;
				    }

				    if (lba + transfer_length > BX_SELECTED_DRIVE(channel).cdrom.capacity) {
					  atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR);
					  raise_interrupt(channel);
					  break;
				    }

				    BX_DEBUG(("cdrom: READ (%d) LBA=%d LEN=%d", atapi_command==0x28?10:12, lba, transfer_length));

				    // handle command
				    init_send_atapi_command(channel, atapi_command, transfer_length * 2048,
							    transfer_length * 2048, true);
				    BX_SELECTED_DRIVE(channel).cdrom.remaining_blocks = transfer_length;
				    BX_SELECTED_DRIVE(channel).cdrom.next_lba = lba;
				    ready_to_send_atapi(channel);
			      }
			      break;

				case 0x2b: { // seek
					uint32 lba = read_32bit(BX_SELECTED_CONTROLLER(channel).buffer + 2);
					if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
						atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
						raise_interrupt(channel);
						break;
					}

					if (lba > BX_SELECTED_DRIVE(channel).cdrom.capacity) {
						atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR);
						raise_interrupt(channel);
						break;
					}
					BX_INFO(("cdrom: SEEK (ignored)"));
					atapi_cmd_nop(channel);
					raise_interrupt(channel);
				}
				break;

			      case 0x1e: { // prevent/allow medium removal
				    if (BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  BX_SELECTED_DRIVE(channel).cdrom.locked = BX_SELECTED_CONTROLLER(channel).buffer[4] & 1;
					  atapi_cmd_nop(channel);
				    } else {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
				    }
				    raise_interrupt(channel);
			      }
			      break;

			      case 0x42: { // read sub-channel
				    bool msf = get_packet_field(channel,1, 1, 1);
				    bool sub_q = get_packet_field(channel,2, 6, 1);
				    uint8 data_format = get_packet_byte(channel,3);
				    uint8 track_number = get_packet_byte(channel,6);
				    uint16 alloc_length = get_packet_word(channel,7);
                                    UNUSED(msf);
                                    UNUSED(data_format);
                                    UNUSED(track_number);

				    if (!BX_SELECTED_DRIVE(channel).cdrom.ready) {
					  atapi_cmd_error(channel, SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt(channel);
				    } else {
					  BX_SELECTED_CONTROLLER(channel).buffer[0] = 0;
					  BX_SELECTED_CONTROLLER(channel).buffer[1] = 0; // audio not supported
					  BX_SELECTED_CONTROLLER(channel).buffer[2] = 0;
					  BX_SELECTED_CONTROLLER(channel).buffer[3] = 0;

					  int ret_len = 4; // header size

					  if (sub_q) { // !sub_q == header only
						BX_ERROR(("Read sub-channel with SubQ not implemented"));
						atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST,
								ASC_INV_FIELD_IN_CMD_PACKET);
					    raise_interrupt(channel);
					  }

					  init_send_atapi_command(channel, atapi_command, ret_len, alloc_length);
					  ready_to_send_atapi(channel);
				    }
			      }
			      break;

			      case 0x51: { // read disc info
                                // no-op to keep the Linux CD-ROM driver happy
			        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET);
			        raise_interrupt(channel);
			      }
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
			      case 0x46: // ???
			      case 0x4a: // ???
			        BX_ERROR(("ATAPI command 0x%x not implemented yet",
			                  atapi_command));
			        atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET);
			        raise_interrupt(channel);
			        break;
			      default:
				    BX_PANIC(("Unknown ATAPI command 0x%x (%d)",
					     atapi_command, atapi_command));
                                    // We'd better signal the error if the user chose to continue
			            atapi_cmd_error(channel, SENSE_ILLEGAL_REQUEST, ASC_INV_FIELD_IN_CMD_PACKET);
			            raise_interrupt(channel);
				    break;
			}
		  }

		  break;

        default:
          BX_PANIC(("IO write(0x%04x): current command is %02xh", address,
            (unsigned) BX_SELECTED_CONTROLLER(channel).current_command));
        }
      break;

    case 0x01: // hard disk write precompensation 0x1f1
	  WRITE_FEATURES(channel,value);
	  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom)) {
		if (value == 0xff)
		      BX_INFO(("no precompensation {%s}", BX_SELECTED_TYPE_STRING(channel)));
		else
		      BX_INFO(("precompensation value %02x {%s}", (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
	  }
      break;

    case 0x02: // hard disk sector count 0x1f2
	  WRITE_SECTOR_COUNT(channel,value);
	  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		BX_INFO(("sector count = %u {%s}", (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
	  break;

    case 0x03: // hard disk sector number 0x1f3
	  WRITE_SECTOR_NUMBER(channel,value);
	  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		BX_INFO(("sector number = %u {%s}", (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
      break;

    case 0x04: // hard disk cylinder low 0x1f4
	  WRITE_CYLINDER_LOW(channel,value);
	  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		BX_INFO(("cylinder low = %02xh {%s}", (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
	  break;

    case 0x05: // hard disk cylinder high 0x1f5
	  WRITE_CYLINDER_HIGH(channel,value);
	  if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		BX_INFO(("cylinder high = %02xh {%s}", (unsigned) value, BX_SELECTED_TYPE_STRING(channel)));
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
      if ( (value & 0xa0) != 0xa0 ) // 1x1xxxxx
        BX_INFO(("IO write 0x%04x (%02x): not 1x1xxxxxb", address, (unsigned) value));
      Bit32u drvsel = BX_HD_THIS channels[channel].drive_select = (value >> 4) & 0x01;
      WRITE_HEAD_NO(channel,value & 0xf);
      if (BX_SELECTED_CONTROLLER(channel).lba_mode == 0 && ((value >> 6) & 1) == 1)
        BX_INFO(("enabling LBA mode"));
      WRITE_LBA_MODE(channel,(value >> 6) & 1);
      if (!BX_SELECTED_IS_PRESENT(channel)) {
        BX_ERROR (("device set to %d which does not exist",drvsel));
        BX_SELECTED_CONTROLLER(channel).error_register = 0x04; // aborted
        BX_SELECTED_CONTROLLER(channel).status.err = 1;
        }
      break;
      }

    case 0x07: // hard disk command 0x1f7
	  // (mch) Writes to the command register with drive_select != 0
	  // are ignored if no secondary device is present
      if ((BX_SLAVE_SELECTED(channel)) && (!BX_SLAVE_IS_PRESENT(channel)))
	    break;
      // Writes to the command register clear the IRQ
      DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);

      if (BX_SELECTED_CONTROLLER(channel).status.busy)
        BX_PANIC(("hard disk: command sent, controller BUSY"));
      if ( (value & 0xf0) == 0x10 )
        value = 0x10;
      switch (value) {

        case 0x10: // CALIBRATE DRIVE
	  if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("calibrate drive issued to non-disk"));
          if (!BX_SELECTED_IS_PRESENT(channel)) {
            BX_SELECTED_CONTROLLER(channel).error_register = 0x02; // Track 0 not found
            BX_SELECTED_CONTROLLER(channel).status.busy = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.seek_complete = 0;
            BX_SELECTED_CONTROLLER(channel).status.drq = 0;
            BX_SELECTED_CONTROLLER(channel).status.err = 1;
	    raise_interrupt(channel);
            BX_INFO(("calibrate drive: disk ata%d-%d not present", channel, BX_SLAVE_SELECTED(channel)));
            break;
            }

          /* move head to cylinder 0, issue IRQ */
          BX_SELECTED_CONTROLLER(channel).error_register = 0;
          BX_SELECTED_CONTROLLER(channel).cylinder_no = 0;
          BX_SELECTED_CONTROLLER(channel).status.busy = 0;
          BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
          BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
          BX_SELECTED_CONTROLLER(channel).status.drq = 0;
          BX_SELECTED_CONTROLLER(channel).status.err = 0;
	  raise_interrupt(channel);
          break;

        case 0x20: // READ MULTIPLE SECTORS, with retries
        case 0x21: // READ MULTIPLE SECTORS, without retries
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

	  if (!BX_SELECTED_IS_HD(channel)) {
		BX_ERROR(("read multiple issued to non-disk"));
		command_aborted(channel, value);
		break;
	  }

          BX_SELECTED_CONTROLLER(channel).current_command = value;

	  // Lose98 accesses 0/0/0 in CHS mode
	  if (!BX_SELECTED_CONTROLLER(channel).lba_mode &&
	      !BX_SELECTED_CONTROLLER(channel).head_no &&
	      !BX_SELECTED_CONTROLLER(channel).cylinder_no &&
	      !BX_SELECTED_CONTROLLER(channel).sector_no) {
		BX_INFO(("Read from 0/0/0, aborting command"));
		command_aborted(channel, value);
		break;
	  }

#if TEST_READ_BEYOND_END==2
	  BX_SELECTED_CONTROLLER(channel).cylinder_no += 100000;
#endif
	  if (!calculate_logical_address(channel, &logical_sector)) {
	    BX_ERROR(("initial read from sector %lu out of bounds, aborting", logical_sector));
	    command_aborted(channel, value);
	    break;
	  }
#if TEST_READ_BEYOND_END==3
	  logical_sector += 100000;
#endif
	  ret=BX_SELECTED_DRIVE(channel).hard_drive->lseek(logical_sector * 512, SEEK_SET);
          if (ret < 0) {
            BX_ERROR (("could not lseek() hard drive image file, aborting"));
	    command_aborted(channel, value);
	    break;
	  }
	  ret = BX_SELECTED_DRIVE(channel).hard_drive->read((bx_ptr_t) BX_SELECTED_CONTROLLER(channel).buffer, 512);
          if (ret < 512) {
            BX_ERROR(("logical sector was %lu", logical_sector));
            BX_ERROR(("could not read() hard drive image file at byte %lu", logical_sector*512));
	    command_aborted(channel, value);
	    break;
	  }

          BX_SELECTED_CONTROLLER(channel).error_register = 0;
          BX_SELECTED_CONTROLLER(channel).status.busy  = 0;
          BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
          BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
          BX_SELECTED_CONTROLLER(channel).status.drq   = 1;
          BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
          BX_SELECTED_CONTROLLER(channel).status.err   = 0;
          BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
	  raise_interrupt(channel);
          break;

        case 0x30: /* WRITE SECTORS, with retries */
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

	  if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("write multiple issued to non-disk"));

          if (BX_SELECTED_CONTROLLER(channel).status.busy) {
            BX_PANIC(("write command: BSY bit set"));
            }
          BX_SELECTED_CONTROLLER(channel).current_command = value;

          // implicit seek done :^)
          BX_SELECTED_CONTROLLER(channel).error_register = 0;
          BX_SELECTED_CONTROLLER(channel).status.busy = 0;
          // BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
          BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
          BX_SELECTED_CONTROLLER(channel).status.drq = 1;
          BX_SELECTED_CONTROLLER(channel).status.err   = 0;
          BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
          break;

        case 0x90: // EXECUTE DEVICE DIAGNOSTIC
          if (BX_SELECTED_CONTROLLER(channel).status.busy) {
            BX_PANIC(("diagnostic command: BSY bit set"));
            }
	  if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("drive diagnostics issued to non-disk"));
          BX_SELECTED_CONTROLLER(channel).error_register = 0x81; // Drive 1 failed, no error on drive 0
          // BX_SELECTED_CONTROLLER(channel).status.busy = 0; // not needed
          BX_SELECTED_CONTROLLER(channel).status.drq = 0;
          BX_SELECTED_CONTROLLER(channel).status.err = 0;
          break;

        case 0x91: // INITIALIZE DRIVE PARAMETERS
          if (BX_SELECTED_CONTROLLER(channel).status.busy) {
            BX_PANIC(("init drive parameters command: BSY bit set"));
            }
	  if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("initialize drive parameters issued to non-disk"));
          // sets logical geometry of specified drive
          BX_DEBUG(("init drive params: sec=%u, drive sel=%u, head=%u",
            (unsigned) BX_SELECTED_CONTROLLER(channel).sector_count,
            (unsigned) BX_HD_THIS channels[channel].drive_select,
            (unsigned) BX_SELECTED_CONTROLLER(channel).head_no));
          if (!BX_SELECTED_IS_PRESENT(channel)) {
            BX_PANIC(("init drive params: disk ata%d-%d not present", channel, BX_SLAVE_SELECTED(channel)));
            //BX_SELECTED_CONTROLLER(channel).error_register = 0x12;
            BX_SELECTED_CONTROLLER(channel).status.busy = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.drq = 0;
            BX_SELECTED_CONTROLLER(channel).status.err = 0;
	    raise_interrupt(channel);
            break;
	  }
          if (BX_SELECTED_CONTROLLER(channel).sector_count != BX_SELECTED_DRIVE(channel).hard_drive->sectors)
            BX_PANIC(("init drive params: sector count doesnt match %d!=%d", BX_SELECTED_CONTROLLER(channel).sector_count, BX_SELECTED_DRIVE(channel).hard_drive->sectors));
          if ( BX_SELECTED_CONTROLLER(channel).head_no != (BX_SELECTED_DRIVE(channel).hard_drive->heads-1) )
            BX_PANIC(("init drive params: head number doesn't match %d != %d",BX_SELECTED_CONTROLLER(channel).head_no, BX_SELECTED_DRIVE(channel).hard_drive->heads-1));
          BX_SELECTED_CONTROLLER(channel).status.busy = 0;
          BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
          BX_SELECTED_CONTROLLER(channel).status.drq = 0;
          BX_SELECTED_CONTROLLER(channel).status.err = 0;
	  raise_interrupt(channel);
          break;

        case 0xec: // IDENTIFY DEVICE
          if (bx_options.OnewHardDriveSupport->get ()) {
	    if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
		  BX_INFO(("Drive ID Command issued : 0xec "));

            if (!BX_SELECTED_IS_PRESENT(channel)) {
              BX_INFO(("disk ata%d-%d not present, aborting",channel,BX_SLAVE_SELECTED(channel)));
              command_aborted(channel, value);
              break;
              }
	    if (BX_SELECTED_IS_CD(channel)) {
		  BX_SELECTED_CONTROLLER(channel).head_no        = 0;
		  BX_SELECTED_CONTROLLER(channel).sector_count   = 1;
		  BX_SELECTED_CONTROLLER(channel).sector_no      = 1;
		  BX_SELECTED_CONTROLLER(channel).cylinder_no    = 0xeb14;
		  command_aborted(channel, 0xec);
	    } else {
		  BX_SELECTED_CONTROLLER(channel).current_command = value;
		  BX_SELECTED_CONTROLLER(channel).error_register = 0;

		  // See ATA/ATAPI-4, 8.12
		  BX_SELECTED_CONTROLLER(channel).status.busy  = 0;
		  BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
		  BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
		  BX_SELECTED_CONTROLLER(channel).status.drq   = 1;
		  BX_SELECTED_CONTROLLER(channel).status.err   = 0;

		  BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
		  BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;

		  BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
		  raise_interrupt(channel);
		  identify_drive(channel);
	    }
	  }
          else {
	    BX_INFO(("sent IDENTIFY DEVICE (0xec) to old hard drive"));
            command_aborted(channel, value);
	  }
          break;

        case 0xef: // SET FEATURES
	  switch(BX_SELECTED_CONTROLLER(channel).features) {
	    case 0x02: // Enable and
	    case 0x82: //  Disable write cache.
	    case 0xAA: // Enable and
	    case 0x55: //  Disable look-ahead cache.
	    case 0xCC: // Enable and
	    case 0x66: //  Disable reverting to power-on default
	      BX_INFO(("SET FEATURES subcommand 0x%02x not supported by disk.", (unsigned) BX_SELECTED_CONTROLLER(channel).features));
	      command_aborted(channel, value);
	    break;

	    default:
	      BX_PANIC(("SET FEATURES with unknown subcommand: 0x%02x", (unsigned) BX_SELECTED_CONTROLLER(channel).features ));
              // We'd better signal the error if the user chose to continue
	      command_aborted(channel, value);
	  }
	  break;

        case 0x40: // READ VERIFY SECTORS
          if (bx_options.OnewHardDriveSupport->get ()) {
	    if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("read verify issued to non-disk"));
            BX_INFO(("Verify Command : 0x40 ! "));
            BX_SELECTED_CONTROLLER(channel).status.busy = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.drq = 0;
            BX_SELECTED_CONTROLLER(channel).status.err = 0;
	    raise_interrupt(channel);
            }
          else {
	    BX_INFO(("sent READ VERIFY SECTORS (0x40) to old hard drive"));
            command_aborted(channel, value);
	  }
          break;

	case 0xc6: // SET MULTIPLE MODE (mch)
	      if (BX_SELECTED_CONTROLLER(channel).sector_count != 128 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 64 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 32 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 16 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 8 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 4 &&
		  BX_SELECTED_CONTROLLER(channel).sector_count != 2)
		    command_aborted(channel, value);

	      if (!BX_SELECTED_IS_HD(channel))
		BX_PANIC(("set multiple mode issued to non-disk"));

	      BX_SELECTED_CONTROLLER(channel).sectors_per_block = BX_SELECTED_CONTROLLER(channel).sector_count;
	      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
	      BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
	      BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
	      BX_SELECTED_CONTROLLER(channel).status.drq = 0;
	      BX_SELECTED_CONTROLLER(channel).status.err = 0;
	      break;

        // ATAPI commands
        case 0xa1: // IDENTIFY PACKET DEVICE
	      if (BX_SELECTED_IS_CD(channel)) {
		    BX_SELECTED_CONTROLLER(channel).current_command = value;
		    BX_SELECTED_CONTROLLER(channel).error_register = 0;

		    BX_SELECTED_CONTROLLER(channel).status.busy = 0;
		    BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
		    BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
		    BX_SELECTED_CONTROLLER(channel).status.drq   = 1;
		    BX_SELECTED_CONTROLLER(channel).status.err   = 0;

		    BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
		    BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;

		    BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
		    raise_interrupt(channel);
		    identify_ATAPI_drive(channel);
	      } else {
		    command_aborted(channel, 0xa1);
	      }
	      break;

        case 0x08: // DEVICE RESET (atapi)
	      if (BX_SELECTED_IS_CD(channel)) {
		    BX_SELECTED_CONTROLLER(channel).status.busy = 1;
		    BX_SELECTED_CONTROLLER(channel).error_register &= ~(1 << 7);

		    // device signature
		    BX_SELECTED_CONTROLLER(channel).head_no        = 0;
		    BX_SELECTED_CONTROLLER(channel).sector_count   = 1;
		    BX_SELECTED_CONTROLLER(channel).sector_no      = 1;
		    BX_SELECTED_CONTROLLER(channel).cylinder_no    = 0xeb14;

		    BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
		    BX_SELECTED_CONTROLLER(channel).status.drq = 0;
		    BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
		    BX_SELECTED_CONTROLLER(channel).status.err = 0;

		    BX_SELECTED_CONTROLLER(channel).status.busy = 0;

	      } else {
		BX_DEBUG(("ATAPI Device Reset on non-cd device"));
		command_aborted(channel, 0x08);
	      }
	      break;

        case 0xa0: // SEND PACKET (atapi)
	      if (BX_SELECTED_IS_CD(channel)) {
		    // PACKET
		    if (BX_SELECTED_CONTROLLER(channel).features & (1 << 0))
			  BX_PANIC(("PACKET-DMA not supported"));
		    if (BX_SELECTED_CONTROLLER(channel).features & (1 << 1))
			  BX_PANIC(("PACKET-overlapped not supported"));

		    // We're already ready!
		    BX_SELECTED_CONTROLLER(channel).sector_count = 1;
		    BX_SELECTED_CONTROLLER(channel).status.busy = 0;
		    BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
		    // serv bit??
		    BX_SELECTED_CONTROLLER(channel).status.drq = 1;
		    BX_SELECTED_CONTROLLER(channel).status.err = 0;

		    // NOTE: no interrupt here
		    BX_SELECTED_CONTROLLER(channel).current_command = value;
		    BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
	      } else {
		command_aborted (channel, 0xa0);
	      }
	      break;

        case 0xa2: // SERVICE (atapi), optional
	      if (BX_SELECTED_IS_CD(channel)) {
		    BX_PANIC(("ATAPI SERVICE not implemented"));
	      } else {
		command_aborted (channel, 0xa2);
	      }
	      break;

        // power management
	case 0xe5: // CHECK POWER MODE
	  BX_SELECTED_CONTROLLER(channel).status.busy = 0;
	  BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
	  BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
	  BX_SELECTED_CONTROLLER(channel).status.drq = 0;
	  BX_SELECTED_CONTROLLER(channel).status.err = 0;
	  BX_SELECTED_CONTROLLER(channel).sector_count = 0xff; // Active or Idle mode
	  raise_interrupt(channel);
	  break;

	case 0x70:  // SEEK (cgs)
	  if (BX_SELECTED_IS_HD(channel)) {
	    BX_DEBUG(("write cmd 0x70 (SEEK) executing"));
            if (!calculate_logical_address(channel, &logical_sector)) {
	      BX_ERROR(("initial seek to sector %lu out of bounds, aborting", logical_sector));
              command_aborted(channel, value);
	      break;
	    }
            BX_SELECTED_CONTROLLER(channel).error_register = 0;
            BX_SELECTED_CONTROLLER(channel).status.busy  = 0;
            BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
            BX_SELECTED_CONTROLLER(channel).status.seek_complete = 1;
            BX_SELECTED_CONTROLLER(channel).status.drq   = 1;
            BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
            BX_SELECTED_CONTROLLER(channel).status.err   = 0;
            BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
  	    BX_DEBUG(("s[0].controller.control.disable_irq = %02x", (BX_HD_THIS channels[channel].drives[0]).controller.control.disable_irq));
  	    BX_DEBUG(("s[1].controller.control.disable_irq = %02x", (BX_HD_THIS channels[channel].drives[1]).controller.control.disable_irq));
  	    BX_DEBUG(("SEEK completed.  error_register = %02x", BX_SELECTED_CONTROLLER(channel).error_register));
  	    raise_interrupt(channel);
  	    BX_DEBUG(("SEEK interrupt completed"));
          } else {
  	    BX_ERROR(("write cmd 0x70 (SEEK) not supported for non-disk"));
  	    command_aborted(channel, 0x70); 
  	  }
          break;



	// List all the write operations that are defined in the ATA/ATAPI spec
	// that we don't support.  Commands that are listed here will cause a
	// BX_ERROR, which is non-fatal, and the command will be aborted.
	case 0x22: BX_ERROR(("write cmd 0x22 (READ LONG) not supported")); command_aborted(channel, 0x22); break;
	case 0x23: BX_ERROR(("write cmd 0x23 (READ LONG NO RETRY) not supported")); command_aborted(channel, 0x23); break;
	case 0x24: BX_ERROR(("write cmd 0x24 (READ SECTORS EXT) not supported"));command_aborted(channel, 0x24); break;
	case 0x25: BX_ERROR(("write cmd 0x25 (READ DMA EXT) not supported"));command_aborted(channel, 0x25); break;
	case 0x26: BX_ERROR(("write cmd 0x26 (READ DMA QUEUED EXT) not supported"));command_aborted(channel, 0x26); break;
	case 0x27: BX_ERROR(("write cmd 0x27 (READ NATIVE MAX ADDRESS EXT) not supported"));command_aborted(channel, 0x27); break;
	case 0x29: BX_ERROR(("write cmd 0x29 (READ MULTIPLE EXT) not supported"));command_aborted(channel, 0x29); break;
	case 0x2A: BX_ERROR(("write cmd 0x2A (READ STREAM DMA) not supported"));command_aborted(channel, 0x2A); break;
	case 0x2B: BX_ERROR(("write cmd 0x2B (READ STREAM PIO) not supported"));command_aborted(channel, 0x2B); break;
	case 0x2F: BX_ERROR(("write cmd 0x2F (READ LOG EXT) not supported"));command_aborted(channel, 0x2F); break;
	case 0x31: BX_ERROR(("write cmd 0x31 (WRITE SECTORS NO RETRY) not supported")); command_aborted(channel, 0x31); break;
	case 0x32: BX_ERROR(("write cmd 0x32 (WRITE LONG) not supported")); command_aborted(channel, 0x32); break;
	case 0x33: BX_ERROR(("write cmd 0x33 (WRITE LONG NO RETRY) not supported")); command_aborted(channel, 0x33); break;
	case 0x34: BX_ERROR(("write cmd 0x34 (WRITE SECTORS EXT) not supported"));command_aborted(channel, 0x34); break;
	case 0x35: BX_ERROR(("write cmd 0x35 (WRITE DMA EXT) not supported"));command_aborted(channel, 0x35); break;
	case 0x36: BX_ERROR(("write cmd 0x36 (WRITE DMA QUEUED EXT) not supported"));command_aborted(channel, 0x36); break;
	case 0x37: BX_ERROR(("write cmd 0x37 (SET MAX ADDRESS EXT) not supported"));command_aborted(channel, 0x37); break;
	case 0x38: BX_ERROR(("write cmd 0x38 (CFA WRITE SECTORS W/OUT ERASE) not supported"));command_aborted(channel, 0x38); break;
	case 0x39: BX_ERROR(("write cmd 0x39 (WRITE MULTIPLE EXT) not supported"));command_aborted(channel, 0x39); break;
	case 0x3A: BX_ERROR(("write cmd 0x3A (WRITE STREAM DMA) not supported"));command_aborted(channel, 0x3A); break;
	case 0x3B: BX_ERROR(("write cmd 0x3B (WRITE STREAM PIO) not supported"));command_aborted(channel, 0x3B); break;
	case 0x3F: BX_ERROR(("write cmd 0x3F (WRITE LOG EXT) not supported"));command_aborted(channel, 0x3F); break;
	case 0x41: BX_ERROR(("write cmd 0x41 (READ VERIFY SECTORS NO RETRY) not supported")); command_aborted(channel, 0x41); break;
	case 0x42: BX_ERROR(("write cmd 0x42 (READ VERIFY SECTORS EXT) not supported"));command_aborted(channel, 0x42); break;
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
	case 0xC4: BX_ERROR(("write cmd 0xC4 (READ MULTIPLE) not supported"));command_aborted(channel, 0xC4); break;
	case 0xC5: BX_ERROR(("write cmd 0xC5 (WRITE MULTIPLE) not supported"));command_aborted(channel, 0xC5); break;
	case 0xC7: BX_ERROR(("write cmd 0xC7 (READ DMA QUEUED) not supported"));command_aborted(channel, 0xC7); break;
	case 0xC8: BX_ERROR(("write cmd 0xC8 (READ DMA) not supported"));command_aborted(channel, 0xC8); break;
	case 0xC9: BX_ERROR(("write cmd 0xC9 (READ DMA NO RETRY) not supported")); command_aborted(channel, 0xC9); break;
	case 0xCA: BX_ERROR(("write cmd 0xCA (WRITE DMA) not supported"));command_aborted(channel, 0xCA); break;
	case 0xCC: BX_ERROR(("write cmd 0xCC (WRITE DMA QUEUED) not supported"));command_aborted(channel, 0xCC); break;
	case 0xCD: BX_ERROR(("write cmd 0xCD (CFA WRITE MULTIPLE W/OUT ERASE) not supported"));command_aborted(channel, 0xCD); break;
	case 0xD1: BX_ERROR(("write cmd 0xD1 (CHECK MEDIA CARD TYPE) not supported"));command_aborted(channel, 0xD1); break;
	case 0xDA: BX_ERROR(("write cmd 0xDA (GET MEDIA STATUS) not supported"));command_aborted(channel, 0xDA); break;
	case 0xDE: BX_ERROR(("write cmd 0xDE (MEDIA LOCK) not supported"));command_aborted(channel, 0xDE); break;
	case 0xDF: BX_ERROR(("write cmd 0xDF (MEDIA UNLOCK) not supported"));command_aborted(channel, 0xDF); break;
	case 0xE0: BX_ERROR(("write cmd 0xE0 (STANDBY IMMEDIATE) not supported"));command_aborted(channel, 0xE0); break;
	case 0xE1: BX_ERROR(("write cmd 0xE1 (IDLE IMMEDIATE) not supported"));command_aborted(channel, 0xE1); break;
	case 0xE2: BX_ERROR(("write cmd 0xE2 (STANDBY) not supported"));command_aborted(channel, 0xE2); break;
	case 0xE3: BX_ERROR(("write cmd 0xE3 (IDLE) not supported"));command_aborted(channel, 0xE3); break;
	case 0xE4: BX_ERROR(("write cmd 0xE4 (READ BUFFER) not supported"));command_aborted(channel, 0xE4); break;
	case 0xE6: BX_ERROR(("write cmd 0xE6 (SLEEP) not supported"));command_aborted(channel, 0xE6); break;
        case 0xE7: BX_ERROR(("write cmd 0xE7 (FLUSH CACHE) not supported"));command_aborted(channel, 0xE7); break;
	case 0xE8: BX_ERROR(("write cmd 0xE8 (WRITE BUFFER) not supported"));command_aborted(channel, 0xE8); break;
	case 0xEA: BX_ERROR(("write cmd 0xEA (FLUSH CACHE EXT) not supported"));command_aborted(channel, 0xEA); break;
	case 0xED: BX_ERROR(("write cmd 0xED (MEDIA EJECT) not supported"));command_aborted(channel, 0xED); break;
	case 0xF1: BX_ERROR(("write cmd 0xF1 (SECURITY SET PASSWORD) not supported"));command_aborted(channel, 0xF1); break;
	case 0xF2: BX_ERROR(("write cmd 0xF2 (SECURITY UNLOCK) not supported"));command_aborted(channel, 0xF2); break;
	case 0xF3: BX_ERROR(("write cmd 0xF3 (SECURITY ERASE PREPARE) not supported"));command_aborted(channel, 0xF3); break;
	case 0xF4: BX_ERROR(("write cmd 0xF4 (SECURITY ERASE UNIT) not supported"));command_aborted(channel, 0xF4); break;
	case 0xF5: BX_ERROR(("write cmd 0xF5 (SECURITY FREEZE LOCK) not supported"));command_aborted(channel, 0xF5); break;
	case 0xF6: BX_ERROR(("write cmd 0xF6 (SECURITY DISABLE PASSWORD) not supported"));command_aborted(channel, 0xF6); break;
	case 0xF8: BX_ERROR(("write cmd 0xF8 (READ NATIVE MAX ADDRESS) not supported"));command_aborted(channel, 0xF8); break;
	case 0xF9: BX_ERROR(("write cmd 0xF9 (SET MAX ADDRESS) not supported"));command_aborted(channel, 0xF9); break;

	default:
          BX_PANIC(("IO write(0x%04x): command 0x%02x", address, (unsigned) value));
	  // if user foolishly decides to continue, abort the command
	  // so that the software knows the drive didn't understand it.
          command_aborted(channel, value);
        }
      break;

    case 0x16: // hard disk adapter control 0x3f6
	  // (mch) Even if device 1 was selected, a write to this register
	  // goes to device 0 (if device 1 is absent)
		
	  prev_control_reset = BX_SELECTED_CONTROLLER(channel).control.reset;
	  BX_HD_THIS channels[channel].drives[0].controller.control.reset         = value & 0x04;
	  BX_HD_THIS channels[channel].drives[1].controller.control.reset         = value & 0x04;
	  // CGS: was: BX_SELECTED_CONTROLLER(channel).control.disable_irq    = value & 0x02;
	  BX_HD_THIS channels[channel].drives[0].controller.control.disable_irq = value & 0x02;
	  BX_HD_THIS channels[channel].drives[1].controller.control.disable_irq = value & 0x02;

      BX_DEBUG(( "adpater control reg: reset controller = %d",
        (unsigned) (BX_SELECTED_CONTROLLER(channel).control.reset) ? 1 : 0 ));
      BX_DEBUG(( "adpater control reg: disable_irq(X) = %d",
        (unsigned) (BX_SELECTED_CONTROLLER(channel).control.disable_irq) ? 1 : 0 ));

	  if (!prev_control_reset && BX_SELECTED_CONTROLLER(channel).control.reset) {
		// transition from 0 to 1 causes all drives to reset
		BX_DEBUG(("hard drive: RESET"));

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

		      BX_CONTROLLER(channel,id).sectors_per_block = 0x80;
		      BX_CONTROLLER(channel,id).lba_mode          = 0;

		      BX_CONTROLLER(channel,id).control.disable_irq = 0;
		      DEV_pic_lower_irq(BX_HD_THIS channels[channel].irq);
		}
	  } else if (BX_SELECTED_CONTROLLER(channel).reset_in_progress &&
		     !BX_SELECTED_CONTROLLER(channel).control.reset) {
		// Clear BSY and DRDY
		BX_DEBUG(("Reset complete {%s}", BX_SELECTED_TYPE_STRING(channel)));
		for (int id = 0; id < 2; id++) {
		      BX_CONTROLLER(channel,id).status.busy           = 0;
		      BX_CONTROLLER(channel,id).status.drive_ready    = 1;
		      BX_CONTROLLER(channel,id).reset_in_progress     = 0;

		      // Device signature
	              if (BX_DRIVE_IS_HD(channel,id)) {
			    BX_CONTROLLER(channel,id).head_no        = 0;
			    BX_CONTROLLER(channel,id).sector_count   = 1;
			    BX_CONTROLLER(channel,id).sector_no      = 1;
			    BX_CONTROLLER(channel,id).cylinder_no    = 0;
		      } else {
			    BX_CONTROLLER(channel,id).head_no        = 0;
			    BX_CONTROLLER(channel,id).sector_count   = 1;
			    BX_CONTROLLER(channel,id).sector_no      = 1;
			    BX_CONTROLLER(channel,id).cylinder_no    = 0xeb14;
		      }
		}
	  }
	    BX_DEBUG(("s[0].controller.control.disable_irq = %02x", (BX_HD_THIS channels[channel].drives[0]).controller.control.disable_irq));
	    BX_DEBUG(("s[1].controller.control.disable_irq = %02x", (BX_HD_THIS channels[channel].drives[1]).controller.control.disable_irq));
	  break;

    default:
      BX_PANIC(("hard drive: io write to address %x = %02x",
        (unsigned) address, (unsigned) value));
    }
}

  void
bx_hard_drive_c::close_harddrive(void)
{
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if(BX_HD_THIS channels[channel].drives[0].hard_drive != NULL)
      BX_HD_THIS channels[channel].drives[0].hard_drive->close();
    if(BX_HD_THIS channels[channel].drives[1].hard_drive != NULL)
      BX_HD_THIS channels[channel].drives[1].hard_drive->close();
  }
}


  bx_bool BX_CPP_AttrRegparmN(2)
bx_hard_drive_c::calculate_logical_address(Bit8u channel, off_t *sector)
{
      off_t logical_sector;

      if (BX_SELECTED_CONTROLLER(channel).lba_mode) {
            //bx_printf ("disk: calculate: %d %d %d\n", ((Bit32u)BX_SELECTED_CONTROLLER(channel).head_no), ((Bit32u)BX_SELECTED_CONTROLLER(channel).cylinder_no), (Bit32u)BX_SELECTED_CONTROLLER(channel).sector_no);
	    logical_sector = ((Bit32u)BX_SELECTED_CONTROLLER(channel).head_no) << 24 |
		  ((Bit32u)BX_SELECTED_CONTROLLER(channel).cylinder_no) << 8 |
		  (Bit32u)BX_SELECTED_CONTROLLER(channel).sector_no;
            //bx_printf ("disk: result: %u\n", logical_sector);
      } else
	    logical_sector = (BX_SELECTED_CONTROLLER(channel).cylinder_no * BX_SELECTED_DRIVE(channel).hard_drive->heads *
			      BX_SELECTED_DRIVE(channel).hard_drive->sectors) +
		  (BX_SELECTED_CONTROLLER(channel).head_no * BX_SELECTED_DRIVE(channel).hard_drive->sectors) +
		  (BX_SELECTED_CONTROLLER(channel).sector_no - 1);

      Bit32u sector_count= 
	   (Bit32u)BX_SELECTED_DRIVE(channel).hard_drive->cylinders * 
           (Bit32u)BX_SELECTED_DRIVE(channel).hard_drive->heads * 
           (Bit32u)BX_SELECTED_DRIVE(channel).hard_drive->sectors;

      if (logical_sector >= sector_count) {
            BX_ERROR (("calc_log_addr: out of bounds (%d/%d)", (Bit32u)logical_sector, sector_count));
	    return false;
      }
      *sector = logical_sector;
      return true;
}

  void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::increment_address(Bit8u channel)
{
      BX_SELECTED_CONTROLLER(channel).sector_count--;

      if (BX_SELECTED_CONTROLLER(channel).lba_mode) {
	    off_t current_address;
	    calculate_logical_address(channel, &current_address);
	    current_address++;
	    BX_SELECTED_CONTROLLER(channel).head_no = (current_address >> 24) & 0xf;
	    BX_SELECTED_CONTROLLER(channel).cylinder_no = (current_address >> 8) & 0xffff;
	    BX_SELECTED_CONTROLLER(channel).sector_no = (current_address) & 0xff;
      } else {
            BX_SELECTED_CONTROLLER(channel).sector_no++;
            if (BX_SELECTED_CONTROLLER(channel).sector_no > BX_SELECTED_DRIVE(channel).hard_drive->sectors) {
		  BX_SELECTED_CONTROLLER(channel).sector_no = 1;
		  BX_SELECTED_CONTROLLER(channel).head_no++;
		  if (BX_SELECTED_CONTROLLER(channel).head_no >= BX_SELECTED_DRIVE(channel).hard_drive->heads) {
			BX_SELECTED_CONTROLLER(channel).head_no = 0;
			BX_SELECTED_CONTROLLER(channel).cylinder_no++;
			if (BX_SELECTED_CONTROLLER(channel).cylinder_no >= BX_SELECTED_DRIVE(channel).hard_drive->cylinders)
			      BX_SELECTED_CONTROLLER(channel).cylinder_no = BX_SELECTED_DRIVE(channel).hard_drive->cylinders - 1;
		  }
	    }
      }
}

  void
bx_hard_drive_c::identify_ATAPI_drive(Bit8u channel)
{
  unsigned i;

  BX_SELECTED_DRIVE(channel).id_drive[0] = (2 << 14) | (5 << 8) | (1 << 7) | (2 << 5) | (0 << 0); // Removable CDROM, 50us response, 12 byte packets

  for (i = 1; i <= 9; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  const char* serial_number = " VT00001\0\0\0\0\0\0\0\0\0\0\0\0";
  for (i = 0; i < 10; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[10+i] = (serial_number[i*2] << 8) |
	      serial_number[i*2 + 1];
  }

  for (i = 20; i <= 22; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  const char* firmware = "ALPHA1  ";
  for (i = 0; i < strlen(firmware)/2; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[23+i] = (firmware[i*2] << 8) |
	      firmware[i*2 + 1];
  }
  BX_ASSERT((23+i) == 27);
  
  for (i = 0; i < strlen((char *) BX_SELECTED_MODEL(channel))/2; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[27+i] = (BX_SELECTED_MODEL(channel)[i*2] << 8) |
	      BX_SELECTED_MODEL(channel)[i*2 + 1];
  }
  BX_ASSERT((27+i) == 47);

  BX_SELECTED_DRIVE(channel).id_drive[47] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[48] = 1; // 32 bits access

  BX_SELECTED_DRIVE(channel).id_drive[49] = (1 << 9); // LBA supported

  BX_SELECTED_DRIVE(channel).id_drive[50] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[51] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[52] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[53] = 3; // words 64-70, 54-58 valid

  for (i = 54; i <= 62; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // copied from CFA540A
  BX_SELECTED_DRIVE(channel).id_drive[63] = 0x0103; // variable (DMA stuff)
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

  for (i = 89; i <= 126; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[127] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[128] = 0;

  for (i = 129; i <= 159; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  for (i = 160; i <= 255; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // now convert the id_drive array (native 256 word format) to
  // the controller buffer (512 bytes)
  Bit16u temp16;
  for (i = 0; i <= 255; i++) {
	temp16 = BX_SELECTED_DRIVE(channel).id_drive[i];
	BX_SELECTED_CONTROLLER(channel).buffer[i*2] = temp16 & 0x00ff;
	BX_SELECTED_CONTROLLER(channel).buffer[i*2+1] = temp16 >> 8;
  }
}

  void
bx_hard_drive_c::identify_drive(Bit8u channel)
{
  unsigned i;
  Bit32u temp32;
  Bit16u temp16;

#if defined(CONNER_CFA540A)
  BX_SELECTED_DRIVE(channel).id_drive[0] = 0x0c5a;
  BX_SELECTED_DRIVE(channel).id_drive[1] = 0x0418;
  BX_SELECTED_DRIVE(channel).id_drive[2] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[3] = BX_SELECTED_DRIVE(channel).hard_drive->heads;
  BX_SELECTED_DRIVE(channel).id_drive[4] = 0x9fb7;
  BX_SELECTED_DRIVE(channel).id_drive[5] = 0x0289;
  BX_SELECTED_DRIVE(channel).id_drive[6] = BX_SELECTED_DRIVE(channel).hard_drive->sectors;
  BX_SELECTED_DRIVE(channel).id_drive[7] = 0x0030;
  BX_SELECTED_DRIVE(channel).id_drive[8] = 0x000a;
  BX_SELECTED_DRIVE(channel).id_drive[9] = 0x0000;

  char* serial_number = " CA00GSQ\0\0\0\0\0\0\0\0\0\0\0\0";
  for (i = 0; i < 10; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[10+i] = (serial_number[i*2] << 8) |
	      serial_number[i*2 + 1];
  }

  BX_SELECTED_DRIVE(channel).id_drive[20] = 3;
  BX_SELECTED_DRIVE(channel).id_drive[21] = 512; // 512 Sectors = 256kB cache
  BX_SELECTED_DRIVE(channel).id_drive[22] = 4;

  char* firmware = "8FT054  ";
  for (i = 0; i < strlen(firmware)/2; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[23+i] = (firmware[i*2] << 8) |
	      firmware[i*2 + 1];
  }
  BX_ASSERT((23+i) == 27);

  char* model = "Conner Peripherals 540MB - CFA540A      ";
  for (i = 0; i < strlen(model)/2; i++) {
	BX_SELECTED_DRIVE(channel).id_drive[27+i] = (model[i*2] << 8) |
	      model[i*2 + 1];
  }
  BX_ASSERT((27+i) == 47);

  BX_SELECTED_DRIVE(channel).id_drive[47] = 0x8080; // multiple mode identification
  BX_SELECTED_DRIVE(channel).id_drive[48] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[49] = 0x0f01;

  BX_SELECTED_DRIVE(channel).id_drive[50] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[51] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[52] = 0x0002;
  BX_SELECTED_DRIVE(channel).id_drive[53] = 0x0003;
  BX_SELECTED_DRIVE(channel).id_drive[54] = 0x0418;

  BX_SELECTED_DRIVE(channel).id_drive[55] = BX_SELECTED_DRIVE(channel).hard_drive->heads;
  BX_SELECTED_DRIVE(channel).id_drive[56] = BX_SELECTED_DRIVE(channel).hard_drive->sectors;

  BX_SELECTED_DRIVE(channel).id_drive[57] = 0x1e80;
  BX_SELECTED_DRIVE(channel).id_drive[58] = 0x0010;
  BX_SELECTED_DRIVE(channel).id_drive[59] = 0x0100 | BX_SELECTED_CONTROLLER(channel).sectors_per_block;
  BX_SELECTED_DRIVE(channel).id_drive[60] = 0x20e0;
  BX_SELECTED_DRIVE(channel).id_drive[61] = 0x0010;

  BX_SELECTED_DRIVE(channel).id_drive[62] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[63] = 0x0103; // variable (DMA stuff)
  BX_SELECTED_DRIVE(channel).id_drive[64] = 0x0001; // PIO
  BX_SELECTED_DRIVE(channel).id_drive[65] = 0x00b4;
  BX_SELECTED_DRIVE(channel).id_drive[66] = 0x00b4;
  BX_SELECTED_DRIVE(channel).id_drive[67] = 0x012c;
  BX_SELECTED_DRIVE(channel).id_drive[68] = 0x00b4;

  for (i = 69; i <= 79; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[80] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[81] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[82] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[83] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[84] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[85] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[86] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[87] = 0;

  for (i = 88; i <= 127; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[128] = 0x0418;
  BX_SELECTED_DRIVE(channel).id_drive[129] = 0x103f;
  BX_SELECTED_DRIVE(channel).id_drive[130] = 0x0418;
  BX_SELECTED_DRIVE(channel).id_drive[131] = 0x103f;
  BX_SELECTED_DRIVE(channel).id_drive[132] = 0x0004;
  BX_SELECTED_DRIVE(channel).id_drive[133] = 0xffff;
  BX_SELECTED_DRIVE(channel).id_drive[134] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[135] = 0x5050;

  for (i = 136; i <= 144; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  BX_SELECTED_DRIVE(channel).id_drive[145] = 0x302e;
  BX_SELECTED_DRIVE(channel).id_drive[146] = 0x3245;
  BX_SELECTED_DRIVE(channel).id_drive[147] = 0x2020;
  BX_SELECTED_DRIVE(channel).id_drive[148] = 0x2020;

  for (i = 149; i <= 255; i++)
	BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

#else

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
  BX_SELECTED_DRIVE(channel).id_drive[1] = BX_SELECTED_DRIVE(channel).hard_drive->cylinders;

  // Word 2: reserved
  BX_SELECTED_DRIVE(channel).id_drive[2] = 0;

  // Word 3: number of user-addressable heads in default
  //   translation mode
  BX_SELECTED_DRIVE(channel).id_drive[3] = BX_SELECTED_DRIVE(channel).hard_drive->heads;

  // Word 4: # unformatted bytes per translated track in default xlate mode
  // Word 5: # unformatted bytes per sector in default xlated mode
  // Word 6: # user-addressable sectors per track in default xlate mode
  // Note: words 4,5 are now "Vendor specific (obsolete)"
  BX_SELECTED_DRIVE(channel).id_drive[4] = (512 * BX_SELECTED_DRIVE(channel).hard_drive->sectors);
  BX_SELECTED_DRIVE(channel).id_drive[5] = 512;
  BX_SELECTED_DRIVE(channel).id_drive[6] = BX_SELECTED_DRIVE(channel).hard_drive->sectors;

  // Word 7-9: Vendor specific
  for (i=7; i<=9; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // Word 10-19: Serial number (20 ASCII characters, 0000h=not specified)
  // This field is right justified and padded with spaces (20h).
  for (i=10; i<=19; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

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
//  for (i=27; i<=46; i++)
//    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;
  for (i=0; i<20; i++) {
    BX_SELECTED_DRIVE(channel).id_drive[27+i] = (BX_SELECTED_MODEL(channel)[i*2] << 8) |
                                  BX_SELECTED_MODEL(channel)[i*2 + 1];
    }

  // Word 47: 15-8 Vendor unique
  //           7-0 00h= read/write multiple commands not implemented
  //               xxh= maximum # of sectors that can be transferred
  //                    per interrupt on read and write multiple commands
  BX_SELECTED_DRIVE(channel).id_drive[47] = max_multiple_sectors;

  // Word 48: 0000h = cannot perform dword IO
  //          0001h = can    perform dword IO
  BX_SELECTED_DRIVE(channel).id_drive[48] = 1;

  // Word 49: Capabilities
  //   15-10: 0 = reserved
  //       9: 1 = LBA supported
  //       8: 1 = DMA supported
  //     7-0: Vendor unique
  BX_SELECTED_DRIVE(channel).id_drive[49] = 1<<9;

  // Word 50: Reserved
  BX_SELECTED_DRIVE(channel).id_drive[50] = 0;

  // Word 51: 15-8 PIO data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_DRIVE(channel).id_drive[51] = 0x200;

  // Word 52: 15-8 DMA data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_DRIVE(channel).id_drive[52] = 0x200;

  // Word 53: 15-1 Reserved
  //             0 1=the fields reported in words 54-58 are valid
  //               0=the fields reported in words 54-58 may be valid
  BX_SELECTED_DRIVE(channel).id_drive[53] = 0;

  // Word 54: # of user-addressable cylinders in curr xlate mode
  // Word 55: # of user-addressable heads in curr xlate mode
  // Word 56: # of user-addressable sectors/track in curr xlate mode
  BX_SELECTED_DRIVE(channel).id_drive[54] = BX_SELECTED_DRIVE(channel).hard_drive->cylinders;
  BX_SELECTED_DRIVE(channel).id_drive[55] = BX_SELECTED_DRIVE(channel).hard_drive->heads;
  BX_SELECTED_DRIVE(channel).id_drive[56] = BX_SELECTED_DRIVE(channel).hard_drive->sectors;

  // Word 57-58: Current capacity in sectors
  // Excludes all sectors used for device specific purposes.
  temp32 = 
    BX_SELECTED_DRIVE(channel).hard_drive->cylinders *
    BX_SELECTED_DRIVE(channel).hard_drive->heads *
    BX_SELECTED_DRIVE(channel).hard_drive->sectors;
  BX_SELECTED_DRIVE(channel).id_drive[57] = (temp32 & 0xffff); // LSW
  BX_SELECTED_DRIVE(channel).id_drive[58] = (temp32 >> 16);    // MSW

  // Word 59: 15-9 Reserved
  //             8 1=multiple sector setting is valid
  //           7-0 current setting for number of sectors that can be
  //               transferred per interrupt on R/W multiple commands
  BX_SELECTED_DRIVE(channel).id_drive[59] = 0x0000 | curr_multiple_sectors;

  // Word 60-61:
  // If drive supports LBA Mode, these words reflect total # of user
  // addressable sectors.  This value does not depend on the current
  // drive geometry.  If the drive does not support LBA mode, these
  // words shall be set to 0.
  Bit32u num_sects = BX_SELECTED_DRIVE(channel).hard_drive->cylinders * BX_SELECTED_DRIVE(channel).hard_drive->heads * BX_SELECTED_DRIVE(channel).hard_drive->sectors;
  BX_SELECTED_DRIVE(channel).id_drive[60] = num_sects & 0xffff; // LSW
  BX_SELECTED_DRIVE(channel).id_drive[61] = num_sects >> 16; // MSW

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
  BX_SELECTED_DRIVE(channel).id_drive[63] = 0x0;

  // Word 64-79 Reserved
  for (i=64; i<=79; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // Word 80: 15-5 reserved
  //             4 supports ATA/ATAPI-4
  //             3 supports ATA-3
  //             2 supports ATA-2
  //             1 supports ATA-1
  //             0 reserved
  BX_SELECTED_DRIVE(channel).id_drive[80] = (1 << 2) | (1 << 1);

  // Word 81: Minor version number
  BX_SELECTED_DRIVE(channel).id_drive[81] = 0;

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
  BX_SELECTED_DRIVE(channel).id_drive[83] = 1 << 14;
  BX_SELECTED_DRIVE(channel).id_drive[84] = 1 << 14;
  BX_SELECTED_DRIVE(channel).id_drive[85] = 1 << 14;
  BX_SELECTED_DRIVE(channel).id_drive[86] = 0;
  BX_SELECTED_DRIVE(channel).id_drive[87] = 1 << 14;

  for (i=88; i<=127; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // Word 128-159 Vendor unique
  for (i=128; i<=159; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

  // Word 160-255 Reserved
  for (i=160; i<=255; i++)
    BX_SELECTED_DRIVE(channel).id_drive[i] = 0;

#endif

  BX_DEBUG(("Drive ID Info. initialized : %04d {%s}", 512, BX_SELECTED_TYPE_STRING(channel)));

  // now convert the id_drive array (native 256 word format) to
  // the controller buffer (512 bytes)
  for (i=0; i<=255; i++) {
    temp16 = BX_SELECTED_DRIVE(channel).id_drive[i];
    BX_SELECTED_CONTROLLER(channel).buffer[i*2] = temp16 & 0x00ff;
    BX_SELECTED_CONTROLLER(channel).buffer[i*2+1] = temp16 >> 8;
    }
}

  void BX_CPP_AttrRegparmN(3)
bx_hard_drive_c::init_send_atapi_command(Bit8u channel, Bit8u command, int req_length, int alloc_length, bool lazy)
{
      // BX_SELECTED_CONTROLLER(channel).byte_count is a union of BX_SELECTED_CONTROLLER(channel).cylinder_no;
      // lazy is used to force a data read in the buffer at the next read.

      if (BX_SELECTED_CONTROLLER(channel).byte_count == 0xffff)
        BX_SELECTED_CONTROLLER(channel).byte_count = 0xfffe;

      if ((BX_SELECTED_CONTROLLER(channel).byte_count & 1)
          && !(alloc_length <= BX_SELECTED_CONTROLLER(channel).byte_count)) {
        BX_INFO(("Odd byte count (0x%04x) to ATAPI command 0x%02x, using 0x%04x", 
		BX_SELECTED_CONTROLLER(channel).byte_count, command, BX_SELECTED_CONTROLLER(channel).byte_count - 1));
        BX_SELECTED_CONTROLLER(channel).byte_count -= 1;
      }

      if (BX_SELECTED_CONTROLLER(channel).byte_count == 0)
	    BX_PANIC(("ATAPI command with zero byte count"));

      if (alloc_length < 0)
	    BX_PANIC(("Allocation length < 0"));
      if (alloc_length == 0)
	    alloc_length = BX_SELECTED_CONTROLLER(channel).byte_count;

      BX_SELECTED_CONTROLLER(channel).interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.c_d = 0;
      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
      BX_SELECTED_CONTROLLER(channel).status.drq = 1;
      BX_SELECTED_CONTROLLER(channel).status.err = 0;

      // no bytes transfered yet
      if (lazy)
	    BX_SELECTED_CONTROLLER(channel).buffer_index = 2048;
      else
	    BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
      BX_SELECTED_CONTROLLER(channel).drq_index = 0;

      if (BX_SELECTED_CONTROLLER(channel).byte_count > req_length)
	    BX_SELECTED_CONTROLLER(channel).byte_count = req_length;

      if (BX_SELECTED_CONTROLLER(channel).byte_count > alloc_length)
	    BX_SELECTED_CONTROLLER(channel).byte_count = alloc_length;

      BX_SELECTED_DRIVE(channel).atapi.command = command;
      BX_SELECTED_DRIVE(channel).atapi.drq_bytes = BX_SELECTED_CONTROLLER(channel).byte_count;
      BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining = (req_length < alloc_length) ? req_length : alloc_length;

      // if (lazy) {
	    // // bias drq_bytes and total_bytes_remaining
	    // BX_SELECTED_DRIVE(channel).atapi.drq_bytes += 2048;
	    // BX_SELECTED_DRIVE(channel).atapi.total_bytes_remaining += 2048;
      // }
}

void
bx_hard_drive_c::atapi_cmd_error(Bit8u channel, sense_t sense_key, asc_t asc)
{
      BX_ERROR(("atapi_cmd_error channel=%02x key=%02x asc=%02x", channel, sense_key, asc));

      BX_SELECTED_CONTROLLER(channel).error_register = sense_key << 4;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.c_d = 1;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.rel = 0;
      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
      BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
      BX_SELECTED_CONTROLLER(channel).status.write_fault = 0;
      BX_SELECTED_CONTROLLER(channel).status.drq = 0;
      BX_SELECTED_CONTROLLER(channel).status.err = 1;

      BX_SELECTED_DRIVE(channel).sense.sense_key = sense_key;
      BX_SELECTED_DRIVE(channel).sense.asc = asc;
      BX_SELECTED_DRIVE(channel).sense.ascq = 0;
}

void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::atapi_cmd_nop(Bit8u channel)
{
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.c_d = 1;
      BX_SELECTED_CONTROLLER(channel).interrupt_reason.rel = 0;
      BX_SELECTED_CONTROLLER(channel).status.busy = 0;
      BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
      BX_SELECTED_CONTROLLER(channel).status.drq = 0;
      BX_SELECTED_CONTROLLER(channel).status.err = 0;
}

void
bx_hard_drive_c::init_mode_sense_single(Bit8u channel, const void* src, int size)
{
      // Header
      BX_SELECTED_CONTROLLER(channel).buffer[0] = (size+6) >> 8;
      BX_SELECTED_CONTROLLER(channel).buffer[1] = (size+6) & 0xff;
      BX_SELECTED_CONTROLLER(channel).buffer[2] = 0x70; // no media present
      BX_SELECTED_CONTROLLER(channel).buffer[3] = 0; // reserved
      BX_SELECTED_CONTROLLER(channel).buffer[4] = 0; // reserved
      BX_SELECTED_CONTROLLER(channel).buffer[5] = 0; // reserved
      BX_SELECTED_CONTROLLER(channel).buffer[6] = 0; // reserved
      BX_SELECTED_CONTROLLER(channel).buffer[7] = 0; // reserved

      // Data
      memcpy(BX_SELECTED_CONTROLLER(channel).buffer + 8, src, size);
}

  void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::ready_to_send_atapi(Bit8u channel)
{
      raise_interrupt(channel);
}

void BX_CPP_AttrRegparmN(1)
bx_hard_drive_c::raise_interrupt(Bit8u channel)
{
	BX_DEBUG(("raise_interrupt called, disable_irq = %02x", BX_SELECTED_CONTROLLER(channel).control.disable_irq));
	if (!BX_SELECTED_CONTROLLER(channel).control.disable_irq) { BX_DEBUG(("raising interrupt")); } else { BX_DEBUG(("Not raising interrupt")); }
      if (!BX_SELECTED_CONTROLLER(channel).control.disable_irq) {
          Bit32u irq = BX_HD_THIS channels[channel].irq; 
          BX_DEBUG(("Raising interrupt %d {%s}", irq, BX_SELECTED_TYPE_STRING(channel)));
          DEV_pic_raise_irq(irq);
      } else {
          if (bx_dbg.disk || (BX_SELECTED_IS_CD(channel) && bx_dbg.cdrom))
              BX_INFO(("Interrupt masked {%s}", BX_SELECTED_TYPE_STRING(channel)));
      }
}

  void
bx_hard_drive_c::command_aborted(Bit8u channel, unsigned value)
{
  BX_DEBUG(("aborting on command 0x%02x {%s}", value, BX_SELECTED_TYPE_STRING(channel)));
  BX_SELECTED_CONTROLLER(channel).current_command = 0;
  BX_SELECTED_CONTROLLER(channel).status.busy = 0;
  BX_SELECTED_CONTROLLER(channel).status.drive_ready = 1;
  BX_SELECTED_CONTROLLER(channel).status.err = 1;
  BX_SELECTED_CONTROLLER(channel).error_register = 0x04; // command ABORTED
  BX_SELECTED_CONTROLLER(channel).status.drq = 0;
  BX_SELECTED_CONTROLLER(channel).status.seek_complete = 0;
  BX_SELECTED_CONTROLLER(channel).status.corrected_data = 0;
  BX_SELECTED_CONTROLLER(channel).buffer_index = 0;
  raise_interrupt(channel);
}

  Bit32u
bx_hard_drive_c::get_device_handle(Bit8u channel, Bit8u device)
{
  BX_DEBUG(("get_device_handle %d %d",channel, device));
  if ((channel < BX_MAX_ATA_CHANNEL) && (device < 2)) {
    return ((channel*2) + device);
    }
  
  return BX_MAX_ATA_CHANNEL*2;
}

  Bit32u
bx_hard_drive_c::get_first_cd_handle(void)
{
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    if (BX_DRIVE_IS_CD(channel,0)) return (channel*2);
    if (BX_DRIVE_IS_CD(channel,1)) return ((channel*2) + 1);
    }
  return BX_MAX_ATA_CHANNEL*2;
}

  unsigned
bx_hard_drive_c::get_cd_media_status(Bit32u handle)
{
  if ( handle >= BX_MAX_ATA_CHANNEL*2 ) return 0;

  Bit8u channel = handle / 2;
  Bit8u device  = handle % 2;
  return( BX_HD_THIS channels[channel].drives[device].cdrom.ready );
}

  unsigned
bx_hard_drive_c::set_cd_media_status(Bit32u handle, unsigned status)
{
  BX_DEBUG (("set_cd_media_status handle=%d status=%d", handle, status));
  if ( handle >= BX_MAX_ATA_CHANNEL*2 ) return 0;

  Bit8u channel = handle / 2;
  Bit8u device  = handle % 2;

  // if setting to the current value, nothing to do
  if (status == BX_HD_THIS channels[channel].drives[device].cdrom.ready)
    return(status);
  // return 0 if no cdromd is present
  if (!BX_DRIVE_IS_CD(channel,device))
    return(0);

  if (status == 0) {
    // eject cdrom if not locked by guest OS
    if (BX_HD_THIS channels[channel].drives[device].cdrom.locked) return(1);
    else {
#ifdef LOWLEVEL_CDROM
      BX_HD_THIS channels[channel].drives[device].cdrom.cd->eject_cdrom();
#endif
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
      bx_options.atadevice[channel][device].Ostatus->set(BX_EJECTED);
      }
    }
  else {
    // insert cdrom
#ifdef LOWLEVEL_CDROM
    if (BX_HD_THIS channels[channel].drives[device].cdrom.cd->insert_cdrom(bx_options.atadevice[channel][device].Opath->getptr())) {
      BX_INFO(( "Media present in CD-ROM drive"));
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 1;
      BX_HD_THIS channels[channel].drives[device].cdrom.capacity = BX_HD_THIS channels[channel].drives[device].cdrom.cd->capacity();
      bx_options.atadevice[channel][device].Ostatus->set(BX_INSERTED);
      BX_SELECTED_DRIVE(channel).sense.sense_key = SENSE_UNIT_ATTENTION;
      BX_SELECTED_DRIVE(channel).sense.asc = 0;
      BX_SELECTED_DRIVE(channel).sense.ascq = 0;
      raise_interrupt(channel);
      }
    else {		    
#endif
      BX_INFO(( "Could not locate CD-ROM, continuing with media not present"));
      BX_HD_THIS channels[channel].drives[device].cdrom.ready = 0;
      bx_options.atadevice[channel][device].Ostatus->set(BX_EJECTED);
#ifdef LOWLEVEL_CDROM
      }
#endif
    }
  return( BX_HD_THIS channels[channel].drives[device].cdrom.ready );
}


/*** default_image_t function definitions ***/

int default_image_t::open (const char* pathname)
{
      return open(pathname, O_RDWR);
}

int default_image_t::open (const char* pathname, int flags)
{
      fd = ::open(pathname, flags
#ifdef O_BINARY
		  | O_BINARY
#endif
	    );

      if (fd < 0) {
	    return fd;
      }

      /* look at size of image file to calculate disk geometry */
      struct stat stat_buf;
      int ret = fstat(fd, &stat_buf);
      if (ret) {
	    BX_PANIC(("fstat() returns error!"));
      }

      return fd;
}

void default_image_t::close ()
{
      if (fd > -1) {
	    ::close(fd);
      }
}

off_t default_image_t::lseek (off_t offset, int whence)
{
      return ::lseek(fd, offset, whence);
}

ssize_t default_image_t::read (void* buf, size_t count)
{
      return ::read(fd, (char*) buf, count);
}

ssize_t default_image_t::write (const void* buf, size_t count)
{
      return ::write(fd, (char*) buf, count);
}

char increment_string (char *str, int diff)
{
  // find the last character of the string, and increment it.
  char *p = str;
  while (*p != 0) p++;
  BX_ASSERT (p>str);  // choke on zero length strings
  p--;  // point to last character of the string
  (*p) += diff;  // increment to next/previous ascii code.
  BX_DEBUG(("increment string returning '%s'", str));
 return (*p);
}

/*** concat_image_t function definitions ***/

concat_image_t::concat_image_t ()
{
  fd = -1;
}

void concat_image_t::increment_string (char *str)
{
 ::increment_string(str, +1);
}

int concat_image_t::open (const char* pathname0)
{
  char *pathname = strdup (pathname0);
  BX_DEBUG(("concat_image_t.open"));
  off_t start_offset = 0;
  for (int i=0; i<BX_CONCAT_MAX_IMAGES; i++) {
    fd_table[i] = ::open(pathname, O_RDWR
#ifdef O_BINARY
		| O_BINARY
#endif
	  );
    if (fd_table[i] < 0) {
      // open failed.
      // if no FD was opened successfully, return -1 (fail).
      if (i==0) return -1;
      // otherwise, it only means that all images in the series have 
      // been opened.  Record the number of fds opened successfully.
      maxfd = i; 
      break;
    }
    BX_DEBUG(("concat_image: open image %s, fd[%d] = %d", pathname, i, fd_table[i]));
    /* look at size of image file to calculate disk geometry */
    struct stat stat_buf;
    int ret = fstat(fd_table[i], &stat_buf);
    if (ret) {
	  BX_PANIC(("fstat() returns error!"));
    }
#ifdef S_ISBLK
    if (S_ISBLK(stat_buf.st_mode)) {
      BX_PANIC(("block devices should REALLY NOT be used with --enable-split-hd. "
                "Please reconfigure with --disable-split-hd"));
    }
#endif
    if ((stat_buf.st_size % 512) != 0) {
      BX_PANIC(("size of disk image must be multiple of 512 bytes"));
    }
    length_table[i] = stat_buf.st_size;
    start_offset_table[i] = start_offset;
    start_offset += stat_buf.st_size;
    increment_string (pathname);
  }
  // start up with first image selected
  index = 0;
  fd = fd_table[0];
  thismin = 0;
  thismax = length_table[0]-1;
  seek_was_last_op = 0;
  return 0; // success.
}

void concat_image_t::close ()
{
  BX_DEBUG(("concat_image_t.close"));
  if (fd > -1) {
    ::close(fd);
  }
}

off_t concat_image_t::lseek (off_t offset, int whence)
{
  if ((offset % 512) != 0) 
    BX_PANIC( ("lseek HD with offset not multiple of 512"));
  BX_DEBUG(("concat_image_t.lseek(%d)", whence));
  // is this offset in this disk image?
  if (offset < thismin) {
    // no, look at previous images
    for (int i=index-1; i>=0; i--) {
      if (offset >= start_offset_table[i]) {
	index = i;
	fd = fd_table[i];
	thismin = start_offset_table[i];
	thismax = thismin + length_table[i] - 1;
	BX_DEBUG(("concat_image_t.lseek to earlier image, index=%d", index));
	break;
      }
    }
  } else if (offset > thismax) {
    // no, look at later images
    for (int i=index+1; i<maxfd; i++) {
      if (offset < start_offset_table[i] + length_table[i]) {
	index = i;
	fd = fd_table[i];
	thismin = start_offset_table[i];
	thismax = thismin + length_table[i] - 1;
	BX_DEBUG(("concat_image_t.lseek to earlier image, index=%d", index));
	break;
      }
    }
  }
  // now offset should be within the current image.
  offset -= start_offset_table[index];
  if (offset < 0 || offset >= length_table[index]) {
    BX_PANIC(("concat_image_t.lseek to byte %ld failed", (long)offset));
    return -1;
  }

  seek_was_last_op = 1;
  return ::lseek(fd, offset, whence);
}

ssize_t concat_image_t::read (void* buf, size_t count)
{
  if (bx_dbg.disk)
    BX_DEBUG(("concat_image_t.read %ld bytes", (long)count));
  // notice if anyone does sequential read or write without seek in between.
  // This can be supported pretty easily, but needs additional checks for
  // end of a partial image.
  if (!seek_was_last_op)
    BX_PANIC( ("no seek before read"));
  return ::read(fd, (char*) buf, count);
}

ssize_t concat_image_t::write (const void* buf, size_t count)
{
  BX_DEBUG(("concat_image_t.write %ld bytes", (long)count));
  // notice if anyone does sequential read or write without seek in between.
  // This can be supported pretty easily, but needs additional checks for
  // end of a partial image.
  if (!seek_was_last_op)
    BX_PANIC( ("no seek before write"));
  return ::write(fd, (char*) buf, count);
}

/*** sparse_image_t function definitions ***/
sparse_image_t::sparse_image_t ()
{
  fd = -1;
  pathname = NULL;
#ifdef _POSIX_MAPPED_FILES
 mmap_header = NULL;
#endif
 pagetable = NULL;
}


/*
void showpagetable(uint32 * pagetable, size_t numpages)
{
 printf("Non null pages: ");
 for (int i = 0; i < numpages; i++)
 {
   if (pagetable[i] != 0xffffffff)
   {
     printf("%d ", i);
   }
 }
 printf("\n");
}
*/


void sparse_image_t::read_header()
{
 BX_ASSERT(sizeof(header) == SPARSE_HEADER_SIZE);

 int ret = ::read(fd, &header, sizeof(header));

 if (-1 == ret)
 {
     panic(strerror(errno));
 }

 if (sizeof(header) != ret)
 {
   panic("could not read entire header");
 }

 if (dtoh32(header.magic) != SPARSE_HEADER_MAGIC)
 {
   panic("failed header magic check");
 }

 if (dtoh32(header.version) != 1)
 {
   panic("unknown version in header");
 }

 pagesize = dtoh32(header.pagesize);
 uint32 numpages = dtoh32(header.numpages);

 total_size = pagesize;
 total_size *= numpages;

 pagesize_shift = 0;
 while ((pagesize >> pagesize_shift) > 1) pagesize_shift++;

 if ((1 << pagesize_shift) != pagesize)
 {
   panic("failed block size header check");
 }

 pagesize_mask = pagesize - 1;

 size_t  preamble_size = (sizeof(uint32) * numpages) + sizeof(header);
 data_start = 0;
 while (data_start < preamble_size) data_start += pagesize;

 bool did_mmap = false;

#ifdef _POSIX_MAPPED_FILES
// Try to memory map from the beginning of the file (0 is trivially a page multiple)
 void * mmap_header = mmap(NULL, preamble_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
 if (mmap_header == MAP_FAILED)
 {
   BX_INFO(("failed to mmap sparse disk file - using conventional file access"));
   mmap_header = NULL;
 }
 else
 {
   mmap_length = preamble_size;
   did_mmap = true;
   pagetable = ((uint32 *) (((uint8 *) mmap_header) + sizeof(header)));

//   system_pagesize = getpagesize();
   system_pagesize_mask = getpagesize() - 1;
 }
#endif

 if (!did_mmap)
 {
   pagetable = new uint32[numpages];

   if (pagetable == NULL)
   {
     panic("could not allocate memory for sparse disk block table");
   }

   ret = ::read(fd, pagetable, sizeof(uint32) * numpages);

   if (-1 == ret)
   {
       panic(strerror(errno));
   }

   if ((sizeof(uint32) * numpages) != ret)
   {
     panic("could not read entire block table");
   }
 }
}

int sparse_image_t::open (const char* pathname0)
{
 pathname = strdup(pathname0);
 BX_DEBUG(("sparse_image_t.open"));

 fd = ::open(pathname, O_RDWR
#ifdef O_BINARY
   | O_BINARY
#endif
   );

 if (fd < 0)
 {
   // open failed.
   return -1;
 }
 BX_DEBUG(("sparse_image: open image %s", pathname));

 read_header();

 struct stat stat_buf;
 if (0 != fstat(fd, &stat_buf)) panic(("fstat() returns error!"));

 underlying_filesize = stat_buf.st_size;

 if ((underlying_filesize % pagesize) != 0)
   panic("size of sparse disk image is not multiple of page size");

 underlying_current_filepos = 0;
 if (-1 == ::lseek(fd, 0, SEEK_SET))
   panic("error while seeking to start of file");

 lseek(0, SEEK_SET);

 //showpagetable(pagetable, header.numpages);

 char * parentpathname = strdup(pathname);
 char lastchar = ::increment_string(parentpathname, -1);

 if ((lastchar >= '0') && (lastchar <= '9'))
 {
   struct stat stat_buf;
   if (0 == stat(parentpathname, &stat_buf))
   {
     parent_image = new sparse_image_t();
     int ret = parent_image->open(parentpathname);
     if (ret != 0) return ret;
     if (    (parent_image->pagesize != pagesize)
         ||  (parent_image->total_size != total_size))
     {
       panic("child drive image does not have same page count/page size configuration");
     }
   }
 }

 if (parentpathname != NULL) free(parentpathname);

 return 0; // success.
}

void sparse_image_t::close ()
{
  BX_DEBUG(("concat_image_t.close"));
  if (pathname != NULL)
  {
   free(pathname);
 }
#ifdef _POSIX_MAPPED_FILES
 if (mmap_header != NULL)
 {
   int ret = munmap(mmap_header, mmap_length);
   if (ret != 0)
     BX_INFO(("failed to un-memory map sparse disk file"));
 }
 pagetable = NULL; // We didn't malloc it
#endif
  if (fd > -1) {
    ::close(fd);
  }
 if (pagetable != NULL)
 {
   delete [] pagetable;
 }
 if (parent_image != NULL)
 {
   delete parent_image;
 }
}

off_t sparse_image_t::lseek (off_t offset, int whence)
{
 //showpagetable(pagetable, header.numpages);

 if ((offset % 512) != 0)
    BX_PANIC( ("lseek HD with offset not multiple of 512"));
 if (whence != SEEK_SET)
   BX_PANIC( ("lseek HD with whence not SEEK_SET"));

 BX_DEBUG(("sparse_image_t.lseek(%d)", whence));

 if (offset > total_size)
 {
   BX_PANIC(("sparse_image_t.lseek to byte %ld failed", (long)offset));
    return -1;
  }

 //printf("Seeking to position %ld\n", (long) offset);

 set_virtual_page(offset >> pagesize_shift);
 position_page_offset = offset & pagesize_mask;

 return 0;
}

inline off_t sparse_image_t::get_physical_offset()
{
 off_t physical_offset = data_start;
 physical_offset += (position_physical_page << pagesize_shift);
 physical_offset += position_page_offset;

 return physical_offset;
}

inline void sparse_image_t::set_virtual_page(uint32 new_virtual_page)
{
 position_virtual_page = new_virtual_page;

 position_physical_page = dtoh32(pagetable[position_virtual_page]);
}

ssize_t sparse_image_t::read_page_fragment(uint32 read_virtual_page, uint32 read_page_offset, size_t read_size, void * buf)
{
 if (read_virtual_page != position_virtual_page)
 {
   set_virtual_page(read_virtual_page);
 }

 position_page_offset = read_page_offset;

 if (position_physical_page == SPARSE_PAGE_NOT_ALLOCATED)
 {
   if (parent_image != NULL)
   {
     return parent_image->read_page_fragment(read_virtual_page, read_page_offset, read_size, buf);
   }
   else
   {
     memset(buf, read_size, 0);
   }
 }
 else
 {
   off_t physical_offset = get_physical_offset();

   if (physical_offset != underlying_current_filepos)
   {
     int ret = ::lseek(fd, physical_offset, SEEK_SET);
     // underlying_current_filepos update deferred
     if (ret == -1)
       panic(strerror(errno));
   }

   //printf("Reading %s at position %ld size %d\n", pathname, (long) physical_offset, (long) read_size);
   ssize_t readret = ::read(fd, buf, read_size);

   if (readret == -1)
   {
     panic(strerror(errno));
   }

   if (readret != read_size)
   {
     panic("could not read block contents from file");
   }

   underlying_current_filepos = physical_offset + read_size;
 }

 return read_size;
}

ssize_t sparse_image_t::read(void* buf, size_t count)
{
 //showpagetable(pagetable, header.numpages);
 ssize_t total_read = 0;

 if (bx_dbg.disk)
    BX_DEBUG(("sparse_image_t.read %ld bytes", (long)count));

 while (count != 0)
 {
   size_t can_read = pagesize - position_page_offset;
   if (count < can_read) can_read = count;

   BX_ASSERT (can_read != 0);

   size_t  was_read = read_page_fragment(position_virtual_page, position_page_offset, can_read, buf);

   BX_ASSERT(was_read == can_read);

   total_read += can_read;

   position_page_offset += can_read;
   if (position_page_offset == pagesize)
   {
     position_page_offset = 0;
     set_virtual_page(position_virtual_page + 1);
   }

   BX_ASSERT(position_page_offset < pagesize);

   buf = (((uint8 *) buf) + can_read);
   count -= can_read;
 }

 return total_read;
}

void sparse_image_t::panic(const char * message)
{
 char buffer[1024];
 if (message == NULL)
 {
   snprintf(buffer, sizeof(buffer), "error with sparse disk image %s", pathname);
 }
 else
 {
   snprintf(buffer, sizeof(buffer), "error with sparse disk image %s - %s", pathname, message);
 }
 BX_PANIC((buffer));
}

ssize_t sparse_image_t::write (const void* buf, size_t count)
{
 //showpagetable(pagetable, header.numpages);

 ssize_t total_written = 0;

 uint32  update_pagetable_start = position_virtual_page;
 uint32  update_pagetable_count = 0;

 if (bx_dbg.disk)
    BX_DEBUG(("sparse_image_t.write %ld bytes", (long)count));

 while (count != 0)
 {
   size_t can_write = pagesize - position_page_offset;
   if (count < can_write) can_write = count;

   BX_ASSERT (can_write != 0);

   if (position_physical_page == SPARSE_PAGE_NOT_ALLOCATED)
   {
     // We just add on another page at the end of the file
     // Reclamation, compaction etc should currently be done off-line

     size_t  data_size = underlying_filesize - data_start;
     BX_ASSERT((data_size % pagesize) == 0);


     uint32  data_size_pages = data_size / pagesize;
     uint32  next_data_page = data_size_pages;

     pagetable[position_virtual_page] = htod32(next_data_page);
     position_physical_page = next_data_page;

     off_t page_file_start = data_start + (position_physical_page << pagesize_shift);

     if (parent_image != NULL)
     {
       // If we have a parent, we must merge our portion with the parent
       void * writebuffer = NULL;

       if (can_write == pagesize)
       {
         writebuffer = (void *) buf;
       }
       else
       {
         writebuffer = malloc(pagesize);
         if (writebuffer == NULL)
           panic("Cannot allocate sufficient memory for page-merge in write");

         // Read entire page - could optimize, but simple for now
         parent_image->read_page_fragment(position_virtual_page, 0, pagesize, writebuffer);

         void * dest_start = ((uint8 *) writebuffer) + position_page_offset;
         memcpy(dest_start, buf, can_write);
       }

       int ret;
       ret = ::lseek(fd, page_file_start, SEEK_SET);
       // underlying_current_filepos update deferred
       if (-1 == ret) panic(strerror(errno));

       ret = ::write(fd, writebuffer, pagesize);

       if (-1 == ret) panic(strerror(errno));

       if (pagesize != ret) panic("failed to write entire merged page to disk");

       if (can_write != pagesize)
       {
         free(writebuffer);
       }
     }
     else
     {
       // We need to write a zero page because read has been returning zeroes
       // We seek as close to the page end as possible, and then write a little
       // This produces a sparse file which has blanks
       // Also very quick, even when pagesize is massive
       int ret;
       ret = ::lseek(fd, page_file_start + pagesize - 4, SEEK_SET);
       // underlying_current_filepos update deferred
       if (-1 == ret) panic(strerror(errno));

       uint32  zero = 0;
       ret = ::write(fd, &zero, 4);

       if (-1 == ret) panic(strerror(errno));

       if (4 != ret) panic("failed to write entire blank page to disk");
     }

     update_pagetable_count = (position_virtual_page - update_pagetable_start) + 1;
     underlying_filesize = underlying_current_filepos = page_file_start + pagesize;
   }

   BX_ASSERT(position_physical_page != SPARSE_PAGE_NOT_ALLOCATED);

   off_t physical_offset = get_physical_offset();

   if (physical_offset != underlying_current_filepos)
   {
     int ret = ::lseek(fd, physical_offset, SEEK_SET);
     // underlying_current_filepos update deferred
     if (ret == -1)
       panic(strerror(errno));
   }

   //printf("Writing at position %ld size %d\n", (long) physical_offset, can_write);
   ssize_t writeret = ::write(fd, buf, can_write);

   if (writeret == -1)
   {
     panic(strerror(errno));
   }

   if (writeret != can_write)
   {
     panic("could not write block contents to file");
   }

   underlying_current_filepos = physical_offset + can_write;

   total_written += can_write;

   position_page_offset += can_write;
   if (position_page_offset == pagesize)
   {
     position_page_offset = 0;
     set_virtual_page(position_virtual_page + 1);
   }

   BX_ASSERT(position_page_offset < pagesize);

   buf = (((uint8 *) buf) + can_write);
   count -= can_write;
 }

 if (update_pagetable_count != 0)
 {
   bool done = false;
   off_t pagetable_write_from = sizeof(header) + (sizeof(uint32) * update_pagetable_start);
   size_t  write_bytecount = update_pagetable_count * sizeof(uint32);

#ifdef _POSIX_MAPPED_FILES
   if (mmap_header != NULL)
   {
     // Sync from the beginning of the page
     size_t system_page_offset = pagetable_write_from & system_pagesize_mask;
     void * start = ((uint8 *) mmap_header + pagetable_write_from - system_page_offset);

     int ret = msync(start, system_page_offset + write_bytecount, MS_ASYNC);

     if (ret != 0)
       panic(strerror(errno));

     done = true;
   }
#endif

   if (!done)
   {
     int ret = ::lseek(fd, pagetable_write_from, SEEK_SET);
     // underlying_current_filepos update deferred
     if (ret == -1) panic(strerror(errno));

     //printf("Writing header at position %ld size %ld\n", (long) pagetable_write_from, (long) write_bytecount);
     ret = ::write(fd, &pagetable[update_pagetable_start], write_bytecount);
     if (ret == -1) panic(strerror(errno));
     if (ret != write_bytecount) panic("could not write entire updated block header");

     underlying_current_filepos = pagetable_write_from + write_bytecount;
   }
 }

 return total_written;
}

#if DLL_HD_SUPPORT
/*** dll_image_t function definitions ***/

/*
function vdisk_open(path:PChar;numclusters,clustersize:integer):integer;
procedure vdisk_read(vunit:integer;blk:integer;var buf:TBlock);
procedure vdisk_write(vunit:integer;blk:integer;var buf:TBlock);
procedure vdisk_close(vunit:integer);
*/

HINSTANCE hlib_vdisk = 0;

int (*vdisk_open)  (const char *path,int numclusters,int clustersize);
void (*vdisk_read)   (int vunit,int blk,void *buf);
void (*vdisk_write)  (int vunit,int blk,const void *buf);
void (*vdisk_close) (int vunit);

int dll_image_t::open (const char* pathname)
{
    if (hlib_vdisk == 0) {
      hlib_vdisk = LoadLibrary("vdisk.dll");
      if (hlib_vdisk != 0) {
        vdisk_read = (void (*)(int,int,void*))        GetProcAddress(hlib_vdisk,"vdisk_read");
        vdisk_write = (void (*)(int,int,const void*)) GetProcAddress(hlib_vdisk,"vdisk_write");
        vdisk_open = (int (*)(const char *,int,int))  GetProcAddress(hlib_vdisk,"vdisk_open");
        vdisk_close = (void (*)(int))                 GetProcAddress(hlib_vdisk,"vdisk_close");
      }
    }
    if (hlib_vdisk != 0) {
      vunit = vdisk_open(pathname,0x10000,64);
      vblk = 0;
    } else {
      vunit = -2;
    }
    return vunit;
}

void dll_image_t::close ()
{
   if (vunit >= 0 && hlib_vdisk != 0) {
     vdisk_close(vunit);
   }
}

off_t dll_image_t::lseek (off_t offset, int whence)
{
      vblk = offset >> 9;
      return 0;
}

ssize_t dll_image_t::read (void* buf, size_t count)
{
      if (vunit >= 0 && hlib_vdisk != 0) {
         vdisk_read(vunit,vblk,buf);
         return count;
      } else {
         return -1;
      }
}

ssize_t dll_image_t::write (const void* buf, size_t count)
{
      if (vunit >= 0 && hlib_vdisk != 0) {
        vdisk_write(vunit,vblk,buf);
        return count;
      } else {
         return -1;
      }
}
#endif // DLL_HD_SUPPORT

error_recovery_t::error_recovery_t ()
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

uint16  BX_CPP_AttrRegparmN(1) 
read_16bit(const uint8* buf)
{
      return (buf[0] << 8) | buf[1];
}

uint32  BX_CPP_AttrRegparmN(1)
read_32bit(const uint8* buf)
{
      return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

// redolog implementation
redolog_t::redolog_t ()
{
        fd = -1;
        catalog = NULL;
        bitmap = NULL;
        extent_index = (Bit32u)0;
        extent_offset = (Bit32u)0;
        extent_next = (Bit32u)0;
}

void
redolog_t::print_header()
{
        BX_INFO(("redolog : Standard Header : magic='%s', type='%s', subtype='%s', version = %d.%d",
                header.standard.magic, header.standard.type, header.standard.subtype,
                dtoh32(header.standard.version)/0x10000,
                dtoh32(header.standard.version)%0x10000));
        BX_INFO(("redolog : Specific Header : #entries=%d, bitmap size=%d, exent size = %d disk size = %lld",
                dtoh32(header.specific.catalog),
                dtoh32(header.specific.bitmap),
                dtoh32(header.specific.extent),
                dtoh64(header.specific.disk)));
}

int 
redolog_t::make_header (const char* type, Bit64u size)
{
        Bit32u entries, extent_size, bitmap_size;
        Bit64u maxsize;

        // Set standard header values
        strcpy((char*)header.standard.magic, STANDARD_HEADER_MAGIC);
        strcpy((char*)header.standard.type, REDOLOG_TYPE);
        strcpy((char*)header.standard.subtype, type);
        header.standard.version = htod32(STANDARD_HEADER_VERSION);
        header.standard.header = htod32(STANDARD_HEADER_SIZE);

        entries = 512;
        bitmap_size = 1;

        // Compute #entries and extent size values
        do {
                static Bit32u flip=0;
                
                extent_size = 8 * bitmap_size * 512;

                header.specific.catalog = htod32(entries);
                header.specific.bitmap = htod32(bitmap_size);
                header.specific.extent = htod32(extent_size);
                
                maxsize = (Bit64u)entries * (Bit64u)extent_size;

                flip++;

                if(flip&0x01) bitmap_size *= 2;
                else entries *= 2;
        } while (maxsize < size);

        header.specific.disk = htod64(size);
        
        print_header();

        catalog = (Bit32u*)malloc(dtoh32(header.specific.catalog) * sizeof(Bit32u));
        bitmap = (Bit8u*)malloc(dtoh32(header.specific.bitmap));

        if ((catalog == NULL) || (bitmap==NULL))
                BX_PANIC(("redolog : could not malloc catalog or bitmap"));

        for (Bit32u i=0; i<dtoh32(header.specific.catalog); i++)
                catalog[i] = htod32(REDOLOG_PAGE_NOT_ALLOCATED);

        bitmap_blocs = 1 + (dtoh32(header.specific.bitmap) - 1) / 512;
        extent_blocs = 1 + (dtoh32(header.specific.extent) - 1) / 512;

        BX_DEBUG(("redolog : each bitmap is %d blocs", bitmap_blocs));
        BX_DEBUG(("redolog : each extent is %d blocs", extent_blocs));

        return 0;
}

int 
redolog_t::create (const char* filename, const char* type, Bit64u size)
{
        int filedes;

        BX_INFO(("redolog : creating redolog %s", filename));

        filedes = ::open(filename, O_RDWR | O_CREAT | O_TRUNC
#ifdef O_BINARY
            | O_BINARY
#endif
              , S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP);

        return create(filedes, type, size);
}

int 
redolog_t::create (int filedes, const char* type, Bit64u size)
{
        fd = filedes;

        if (fd < 0)
        {
                // open failed.
                return -1;
        }

        if (make_header(type, size) < 0)
        {
                return -1;
        }

        // Write header
        ::write(fd, &header, dtoh32(header.standard.header));

        // Write catalog
        // FIXME could mmap
        ::write(fd, catalog, dtoh32(header.specific.catalog) * sizeof (Bit32u));

        return 0;
}

int 
redolog_t::open (const char* filename, const char *type, Bit64u size)
{
        int res;

        fd = ::open(filename, O_RDWR
#ifdef O_BINARY
            | O_BINARY
#endif
              );
        if (fd < 0)
        {
                BX_INFO(("redolog : could not open image %s", filename));
                // open failed.
                return -1;
        }
        BX_INFO(("redolog : open image %s", filename));
      
        res = ::read(fd, &header, sizeof(header));
        if (res != STANDARD_HEADER_SIZE)
        {
               BX_PANIC(("redolog : could not read header")); 
               return -1;
        }

        print_header();

        if (strcmp((char*)header.standard.magic, STANDARD_HEADER_MAGIC) != 0)
        {
               BX_PANIC(("redolog : Bad header magic")); 
               return -1;
        }

        if (strcmp((char*)header.standard.type, REDOLOG_TYPE) != 0)
        {
               BX_PANIC(("redolog : Bad header type")); 
               return -1;
        }
        if (strcmp((char*)header.standard.subtype, type) != 0)
        {
               BX_PANIC(("redolog : Bad header subtype")); 
               return -1;
        }

        if (dtoh32(header.standard.version) != STANDARD_HEADER_VERSION)
        {
               BX_PANIC(("redolog : Bad header version")); 
               return -1;
        }

        catalog = (Bit32u*)malloc(dtoh32(header.specific.catalog) * sizeof(Bit32u));
        
        // FIXME could mmap
        ::lseek(fd,dtoh32(header.standard.header),SEEK_SET);
        res = ::read(fd, catalog, dtoh32(header.specific.catalog) * sizeof(Bit32u)) ;

        if (res !=  dtoh32(header.specific.catalog) * sizeof(Bit32u))
        {
               BX_PANIC(("redolog : could not read catalog %d=%d",res, dtoh32(header.specific.catalog))); 
               return -1;
        }

        // check last used extent
        extent_next = 0;
        for (Bit32u i=0; i < dtoh32(header.specific.catalog); i++)
        {
                if (dtoh32(catalog[extent_index]) != REDOLOG_PAGE_NOT_ALLOCATED)
                {
                        if (dtoh32(catalog[extent_index]) >= extent_next)
                                extent_next = dtoh32(catalog[extent_index]) + 1;
                }
        }
        BX_INFO(("redolog : next extent will be at index %d",extent_next));
      
        // memory used for storing bitmaps
        bitmap = (Bit8u *)malloc(dtoh32(header.specific.bitmap));

        bitmap_blocs = 1 + (dtoh32(header.specific.bitmap) - 1) / 512;
        extent_blocs = 1 + (dtoh32(header.specific.extent) - 1) / 512;

        BX_DEBUG(("redolog : each bitmap is %d blocs", bitmap_blocs));
        BX_DEBUG(("redolog : each extent is %d blocs", extent_blocs));
}

void 
redolog_t::close ()
{
        if (fd >= 0)
                ::close(fd);

        if (catalog != NULL)
                free(catalog);

        if (bitmap != NULL)
                free(bitmap);
}

off_t
redolog_t::lseek (off_t offset, int whence)
{
        if ((offset % 512) != 0)
                BX_PANIC( ("redolog : lseek HD with offset not multiple of 512"));
        if (whence != SEEK_SET)
                BX_PANIC( ("redolog : lseek HD with whence not SEEK_SET"));

        if (offset > dtoh64(header.specific.disk))
        {
                BX_PANIC(("redolog : lseek to byte %ld failed", (long)offset));
                return -1;
        }

        extent_index = offset / dtoh32(header.specific.extent);
        extent_offset = (offset % dtoh32(header.specific.extent)) / 512;

        BX_DEBUG(("redolog : lseeking extent index %d, offset %d",extent_index, extent_offset));
}

ssize_t
redolog_t::read (void* buf, size_t count)
{
        off_t bloc_offset, bitmap_offset;

        if (count != 512)
                BX_PANIC( ("redolog : read HD with count not 512"));

        BX_DEBUG(("redolog : reading index %d, mapping to %d", extent_index, dtoh32(catalog[extent_index])));

        if (dtoh32(catalog[extent_index]) == REDOLOG_PAGE_NOT_ALLOCATED)
        {
                // page not allocated
                return 0;
        }

        bitmap_offset  = (off_t)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
        bitmap_offset += (off_t)512 * dtoh32(catalog[extent_index]) * (extent_blocs + bitmap_blocs); 
        bloc_offset    = bitmap_offset + ((off_t)512 * (bitmap_blocs + extent_offset));

        BX_DEBUG(("redolog : bitmap offset is %x", (Bit32u)bitmap_offset));
        BX_DEBUG(("redolog : bloc offset is %x", (Bit32u)bloc_offset));


        // FIXME if same extent_index as before we can skip bitmap read

        ::lseek(fd, bitmap_offset, SEEK_SET);

        if (::read(fd, bitmap,  dtoh32(header.specific.bitmap)) != dtoh32(header.specific.bitmap))
        {
                BX_PANIC(("redolog : failed to read bitmap for extent %ld", extent_index));
                return 0;
        }

        if ( ((bitmap[extent_offset/8] >> (extent_offset%8)) & 0x01) == 0x00 )
        {
                BX_DEBUG(("read not in redolog"));

                // bitmap says bloc not in reloglog
                return 0;
        }
        
        ::lseek(fd, bloc_offset, SEEK_SET);

        return (::read(fd, buf, count));
}

ssize_t
redolog_t::write (const void* buf, size_t count)
{
        Bit32u i;
        off_t bloc_offset, bitmap_offset, catalog_offset;
        ssize_t written;
        bx_bool update_catalog = 0;

        if (count != 512)
                BX_PANIC( ("redolog : write HD with count not 512"));

        BX_DEBUG(("redolog : writing index %d, mapping to %d", extent_index, dtoh32(catalog[extent_index])));
        if (dtoh32(catalog[extent_index]) == REDOLOG_PAGE_NOT_ALLOCATED)
        {
                if(extent_next >= dtoh32(header.specific.catalog))
                {
                        BX_PANIC(("redolog : can't allocate new extent... catalog is full"));
                        return 0;
                }

                BX_DEBUG(("redolog : allocating new extent at %d", extent_next));

                // Extent not allocated, allocate new
                catalog[extent_index] = htod32(extent_next);
                
                extent_next += 1;

                char *zerobuffer = (char*)malloc(512);
                memset(zerobuffer, 0, 512);

                // Write bitmap
                bitmap_offset  = (off_t)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
                bitmap_offset += (off_t)512 * dtoh32(catalog[extent_index]) * (extent_blocs + bitmap_blocs); 
                ::lseek(fd, bitmap_offset, SEEK_SET);
                for(i=0; i<bitmap_blocs; i++)
                {
                        ::write(fd, zerobuffer, 512);
                }
                // Write extent
                for(i=0; i<extent_blocs; i++)
                {
                        ::write(fd, zerobuffer, 512);
                }

                free(zerobuffer);

                update_catalog = 1;
        }

        bitmap_offset  = (off_t)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
        bitmap_offset += (off_t)512 * dtoh32(catalog[extent_index]) * (extent_blocs + bitmap_blocs); 
        bloc_offset    = bitmap_offset + ((off_t)512 * (bitmap_blocs + extent_offset));

        BX_DEBUG(("redolog : bitmap offset is %x", (Bit32u)bitmap_offset));
        BX_DEBUG(("redolog : bloc offset is %x", (Bit32u)bloc_offset));

        // Write bloc
        ::lseek(fd, bloc_offset, SEEK_SET);
        written = ::write(fd, buf, count);

        // Write bitmap
        // FIXME if same extent_index as before we can skip bitmap read
        ::lseek(fd, bitmap_offset, SEEK_SET);
        if (::read(fd, bitmap,  dtoh32(header.specific.bitmap)) != dtoh32(header.specific.bitmap))
        {
                BX_PANIC(("redolog : failed to read bitmap for extent %ld", extent_index));
                return 0;
        }

        // If bloc does not belong to extent yet
        if ( ((bitmap[extent_offset/8] >> (extent_offset%8)) & 0x01) == 0x00 )
        {
                bitmap[extent_offset/8] |= 1 << (extent_offset%8);
                ::lseek(fd, bitmap_offset, SEEK_SET);
                ::write(fd, bitmap,  dtoh32(header.specific.bitmap));
        }

        // Write catalog
        if (update_catalog)
        {
                // FIXME if mmap
                catalog_offset  = (off_t)STANDARD_HEADER_SIZE + (extent_index * sizeof(Bit32u));

                BX_DEBUG(("redolog : writing catalog at offset %x", (Bit32u)catalog_offset));

                ::lseek(fd, catalog_offset, SEEK_SET);
                ::write(fd, &catalog[extent_index], sizeof(Bit32u));
        }

        return written;
}


/*** growable_image_t function definitions ***/

growable_image_t::growable_image_t(Bit64u _size)
{
        redolog = new redolog_t();
        size = _size;
}

int growable_image_t::open (const char* pathname)
{
        return redolog->open(pathname,REDOLOG_SUBTYPE_GROWABLE,size);
}

void growable_image_t::close ()
{
        redolog->close();
}

off_t growable_image_t::lseek (off_t offset, int whence)
{
      return redolog->lseek(offset, whence);
}

ssize_t growable_image_t::read (void* buf, size_t count)
{
      memset(buf, 0, count);
      redolog->read((char*) buf, count);
      return count;
}

ssize_t growable_image_t::write (const void* buf, size_t count)
{
      return redolog->write((char*) buf, count);
}


/*** undoable_image_t function definitions ***/

undoable_image_t::undoable_image_t(Bit64u _size)
{
        redolog = new redolog_t();
        ro_disk = new default_image_t();
        size = _size;
}

int undoable_image_t::open (const char* pathname)
{
        char *redolog_name;

        if (ro_disk->open(pathname, O_RDONLY)<0)
                return -1;

        redolog_name = (char*)malloc(strlen(pathname) + strlen(REDOLOG_EXTENSION) + 1);
        sprintf (redolog_name, "%s%s", pathname, REDOLOG_EXTENSION);
        if (redolog->open(redolog_name,REDOLOG_SUBTYPE_UNDOABLE,size) < 0)
        {
                if (redolog->create(redolog_name, REDOLOG_SUBTYPE_UNDOABLE, size) < 0)
                {
                        BX_PANIC(("Can't open or create redolog %s",redolog_name));
                        return -1;
                }
        }

        free(redolog_name);

        return 0;
}

void undoable_image_t::close ()
{
        redolog->close();
        ro_disk->close();
}

off_t undoable_image_t::lseek (off_t offset, int whence)
{
      redolog->lseek(offset, whence);
      return ro_disk->lseek(offset, whence);
}

ssize_t undoable_image_t::read (void* buf, size_t count)
{
      // This should be fixed if count != 512
      if (redolog->read((char*) buf, count) != count)
              return ro_disk->read((char*) buf, count);
      else 
              return count;
}

ssize_t undoable_image_t::write (const void* buf, size_t count)
{
      return redolog->write((char*) buf, count);
}


/*** volatile_image_t function definitions ***/

volatile_image_t::volatile_image_t(Bit64u _size)
{
        redolog = new redolog_t();
        ro_disk = new default_image_t();
        size = _size;
        redolog_name = NULL;
}

int volatile_image_t::open (const char* pathname)
{
        int filedes;

        if (ro_disk->open(pathname, O_RDONLY)<0)
                return -1;

        redolog_name = (char*)malloc(strlen(pathname) + strlen(".XXXXXX") + 1);
        sprintf (redolog_name, "%s%s", pathname, ".XXXXXX");

        filedes = mkstemp (redolog_name);

        if (filedes < 0)
        {
                BX_PANIC(("Can't create volatile redolog"));
                return -1;
        }
        if (redolog->create(filedes, REDOLOG_SUBTYPE_VOLATILE, size) < 0)
        {
                BX_PANIC(("Can't create volatile redolog"));
                return -1;
        }
        
#if (!defined(WIN32)) && !BX_WITH_MACOS
        unlink(redolog_name);
#endif
        return 0;
}

void volatile_image_t::close ()
{
        redolog->close();
        ro_disk->close();

#if defined(WIN32) || BX_WITH_MACOS
        unlink(redolog_name);
#endif
        free(redolog_name);
}

off_t volatile_image_t::lseek (off_t offset, int whence)
{
      redolog->lseek(offset, whence);
      return ro_disk->lseek(offset, whence);
}

ssize_t volatile_image_t::read (void* buf, size_t count)
{
      // This should be fixed if count != 512
      if (redolog->read((char*) buf, count) != count)
              return ro_disk->read((char*) buf, count);
      else 
              return count;
}

ssize_t volatile_image_t::write (const void* buf, size_t count)
{
      return redolog->write((char*) buf, count);
}

#if BX_COMPRESSED_HD_SUPPORT

/*** z_ro_image_t function definitions ***/

z_ro_image_t::z_ro_image_t()
{
        offset = (off_t)0;
}

int z_ro_image_t::open (const char* pathname)
{
        fd = ::open(pathname, O_RDONLY
#ifdef O_BINARY
		  | O_BINARY
#endif
	    );

        if(fd < 0)
        {
              BX_PANIC(("Could not open '%s' file", pathname));
              return fd;
        }

        gzfile = gzdopen(fd, "rb");
}

void z_ro_image_t::close ()
{
        if (fd > -1) {
            gzclose(gzfile);
	    // ::close(fd);
        }
}

off_t z_ro_image_t::lseek (off_t _offset, int whence)
{
        // Only SEEK_SET supported
        if (whence != SEEK_SET)
        {
              BX_PANIC(("lseek on compressed images : only SEEK_SET supported"));
        }

        // Seeking is expensive on compressed files, so we do it
        // only when necessary, at the latest moment
        offset = _offset;

        return offset;
}

ssize_t z_ro_image_t::read (void* buf, size_t count)
{
      gzseek(gzfile, offset, SEEK_SET);
      return gzread(gzfile, buf, count);
}

ssize_t z_ro_image_t::write (const void* buf, size_t count)
{
      BX_PANIC(("z_ro_image: write not supported"));
      return 0;
}


/*** z_undoable_image_t function definitions ***/

z_undoable_image_t::z_undoable_image_t(Bit64u _size)
{
        redolog = new redolog_t();
        ro_disk = new z_ro_image_t();
        size = _size;
}

int z_undoable_image_t::open (const char* pathname)
{
        char *redolog_name;

        if (ro_disk->open(pathname)<0)
                return -1;

        redolog_name = (char*)malloc(strlen(pathname) + strlen(REDOLOG_EXTENSION) + 1);
        sprintf (redolog_name, "%s%s", pathname, REDOLOG_EXTENSION);
        if (redolog->open(redolog_name,REDOLOG_SUBTYPE_UNDOABLE,size) < 0)
        {
                if (redolog->create(redolog_name, REDOLOG_SUBTYPE_UNDOABLE, size) < 0)
                {
                        BX_PANIC(("Can't open or create redolog %s",redolog_name));
                        return -1;
                }
        }

        free(redolog_name);

        return 0;
}

void z_undoable_image_t::close ()
{
        redolog->close();
        ro_disk->close();
}

off_t z_undoable_image_t::lseek (off_t offset, int whence)
{
      redolog->lseek(offset, whence);
      return ro_disk->lseek(offset, whence);
}

ssize_t z_undoable_image_t::read (void* buf, size_t count)
{
      // This should be fixed if count != 512
      if (redolog->read((char*) buf, count) != count)
              return ro_disk->read((char*) buf, count);
      else 
              return count;
}

ssize_t z_undoable_image_t::write (const void* buf, size_t count)
{
      return redolog->write((char*) buf, count);
}


/*** z_volatile_image_t function definitions ***/

z_volatile_image_t::z_volatile_image_t(Bit64u _size)
{
        redolog = new redolog_t();
        ro_disk = new z_ro_image_t();
        size = _size;
        redolog_name = NULL;
}

int z_volatile_image_t::open (const char* pathname)
{
        int filedes;

        if (ro_disk->open(pathname)<0)
                return -1;

        redolog_name = (char*)malloc(strlen(pathname) + strlen(".XXXXXX") + 1);
        sprintf (redolog_name, "%s%s", pathname, ".XXXXXX");

        filedes = mkstemp (redolog_name);

        if (filedes < 0)
        {
                BX_PANIC(("Can't create volatile redolog"));
                return -1;
        }
        if (redolog->create(filedes, REDOLOG_SUBTYPE_VOLATILE, size) < 0)
        {
                BX_PANIC(("Can't create volatile redolog"));
                return -1;
        }
        
#if (!defined(WIN32)) && !BX_WITH_MACOS
        unlink(redolog_name);
#endif
        return 0;
}

void z_volatile_image_t::close ()
{
        redolog->close();
        ro_disk->close();

#if defined(WIN32) || BX_WITH_MACOS
        unlink(redolog_name);
#endif
        free(redolog_name);
}

off_t z_volatile_image_t::lseek (off_t offset, int whence)
{
      redolog->lseek(offset, whence);
      return ro_disk->lseek(offset, whence);
}

ssize_t z_volatile_image_t::read (void* buf, size_t count)
{
      // This should be fixed if count != 512
      if (redolog->read((char*) buf, count) != count)
              return ro_disk->read((char*) buf, count);
      else 
              return count;
}

ssize_t z_volatile_image_t::write (const void* buf, size_t count)
{
      return redolog->write((char*) buf, count);
}


#endif
