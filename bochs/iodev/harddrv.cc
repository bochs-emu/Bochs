//  Copyright (C) 2001  MandrakeSoft S.A.
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


#include "bochs.h"
#define LOG_THIS BX_HD_THIS


#define INDEX_PULSE_CYCLE 10

#define PACKET_SIZE 12

#if BX_USE_HD_SMF
bx_hard_drive_c bx_hard_drive;
#define this (&bx_hard_drive)
#endif


static unsigned char model_no[41] =
  "Generic 1234                            ";

static unsigned max_multiple_sectors  = 0; // was 0x3f
static unsigned curr_multiple_sectors = 0; // was 0x3f

// some packet handling macros
#define EXTRACT_FIELD(arr,byte,start,num_bits) (((arr)[(byte)] >> (start)) & ((1 << (num_bits)) - 1))
#define get_packet_field(b,s,n) (EXTRACT_FIELD((BX_SELECTED_CONTROLLER.buffer),(b),(s),(n)))
#define get_packet_byte(b) (BX_SELECTED_CONTROLLER.buffer[(b)])
#define get_packet_word(b) (((uint16)BX_SELECTED_CONTROLLER.buffer[(b)] << 8) | BX_SELECTED_CONTROLLER.buffer[(b)+1])


#define BX_CONTROLLER(a) (BX_HD_THIS s[(a)]).controller
#define BX_SELECTED_CONTROLLER (BX_CONTROLLER(BX_HD_THIS drive_select))

#define WRITE_FEATURES(a) do { uint8 _a = a; BX_CONTROLLER(0).features = _a; BX_CONTROLLER(1).features = _a; } while(0)
#define WRITE_SECTOR_COUNT(a) do { uint8 _a = a; BX_CONTROLLER(0).sector_count = _a; BX_CONTROLLER(1).sector_count = _a; } while(0)
#define WRITE_SECTOR_NUMBER(a) do { uint8 _a = a; BX_CONTROLLER(0).sector_no = _a; BX_CONTROLLER(1).sector_no = _a; } while(0)
#define WRITE_CYLINDER_LOW(a) do { uint8 _a = a; BX_CONTROLLER(0).cylinder_no = (BX_CONTROLLER(0).cylinder_no & 0xff00) | _a; BX_CONTROLLER(1).cylinder_no = (BX_CONTROLLER(1).cylinder_no & 0xff00) | _a; } while(0)
#define WRITE_CYLINDER_HIGH(a) do { uint16 _a = a; BX_CONTROLLER(0).cylinder_no = (_a << 8) | (BX_CONTROLLER(0).cylinder_no & 0xff); BX_CONTROLLER(1).cylinder_no = (_a << 8) | (BX_CONTROLLER(1).cylinder_no & 0xff); } while(0)
#define WRITE_HEAD_NO(a) do { uint8 _a = a; BX_CONTROLLER(0).head_no = _a; BX_CONTROLLER(1).head_no = _a; } while(0)
#define WRITE_LBA_MODE(a) do { uint8 _a = a; BX_CONTROLLER(0).lba_mode = _a; BX_CONTROLLER(1).lba_mode = _a; } while(0)


//static unsigned im_here = 0;

bx_hard_drive_c::bx_hard_drive_c(void)
{
      setprefix("[HD  ]",__FILE__,__LINE__);
      settype(HDLOG);
      setio(SAFE_GET_IOFUNC());
#if EXTERNAL_DISK_SIMULATOR
      s[0].hard_drive = new EXTERNAL_DISK_SIMULATOR_CLASS();
      s[1].hard_drive = new EXTERNAL_DISK_SIMULATOR_CLASS();
#else

#if BX_SPLIT_HD_SUPPORT
      // use new concatenated image object
      s[0].hard_drive = new concat_image_t();
      s[1].hard_drive = new concat_image_t();
#else
      s[0].hard_drive = new default_image_t();
      s[1].hard_drive = new default_image_t();
#endif

#endif
}

bx_hard_drive_c::~bx_hard_drive_c(void)
{
  // nothing for now
}



  void
bx_hard_drive_c::init(bx_devices_c *d, bx_cmos_c *cmos)
{
  BX_HD_THIS devices = d;

  /* HARD DRIVE 0 */

  BX_HD_THIS devices->register_irq(14, "Hard Drive 0");
  for (unsigned addr=0x01F0; addr<=0x01F7; addr++) {
    BX_HD_THIS devices->register_io_read_handler(this, read_handler,
                                        addr, "Hard Drive 0");
    BX_HD_THIS devices->register_io_write_handler(this, write_handler,
                                        addr, "Hard Drive 0");
    }
#if 0
  // this would be necessary to make the second HD master on the
  // second controller, using 0x170-0x177 and irq15.  But it currently
  // works as second disk on the first IDE controller, so this code
  // is not needed.
  BX_HD_THIS devices->register_irq(15, "Hard Drive 1");
  for (unsigned addr=0x0170; addr<=0x0177; addr++) {
    BX_HD_THIS devices->register_io_read_handler(this, read_handler,
                                        addr, "Hard Drive 1");
    BX_HD_THIS devices->register_io_write_handler(this, write_handler,
                                        addr, "Hard Drive 1");
    }
#endif

  BX_HD_THIS drive_select = 0;

  BX_HD_THIS s[0].hard_drive->cylinders = bx_options.diskc.cylinders;
  BX_HD_THIS s[0].hard_drive->heads     = bx_options.diskc.heads;
  BX_HD_THIS s[0].hard_drive->sectors   = bx_options.diskc.spt;
  BX_HD_THIS s[0].device_type           = IDE_DISK;
  BX_HD_THIS s[1].hard_drive->cylinders = bx_options.diskd.cylinders;
  BX_HD_THIS s[1].hard_drive->heads     = bx_options.diskd.heads;
  BX_HD_THIS s[1].hard_drive->sectors   = bx_options.diskd.spt;
  BX_HD_THIS s[1].device_type           = IDE_DISK;

  if (bx_options.cdromd.present) {
	bx_options.diskd.present = 1;
	fprintf(stderr, "disk: Experimental CDROM on target 1\n");
	BX_HD_THIS s[1].device_type = IDE_CDROM;
	BX_HD_THIS s[1].cdrom.locked = 0;
	BX_HD_THIS s[1].sense.sense_key = SENSE_NONE;
	BX_HD_THIS s[1].sense.asc = 0;
	BX_HD_THIS s[1].sense.ascq = 0;
	// Check bit fields
	BX_CONTROLLER(1).sector_count = 0;
	BX_CONTROLLER(1).interrupt_reason.c_d = 1;
	if (BX_CONTROLLER(1).sector_count != 0x01)
	      bx_panic("disk: interrupt reason bit field error\n");

	BX_CONTROLLER(1).sector_count = 0;
	BX_CONTROLLER(1).interrupt_reason.i_o = 1;
	if (BX_CONTROLLER(1).sector_count != 0x02)
	      bx_panic("disk: interrupt reason bit field error\n");

	BX_CONTROLLER(1).sector_count = 0;
	BX_CONTROLLER(1).interrupt_reason.rel = 1;
	if (BX_CONTROLLER(1).sector_count != 0x04)
	      bx_panic("disk: interrupt reason bit field error\n");

	BX_CONTROLLER(1).sector_count = 0;
	BX_CONTROLLER(1).interrupt_reason.tag = 3;
	if (BX_CONTROLLER(1).sector_count != 0x18)
	      bx_panic("disk: interrupt reason bit field error\n");
	BX_CONTROLLER(1).sector_count = 0;

	// allocate low level driver
#ifdef LOWLEVEL_CDROM
	BX_HD_THIS s[1].cdrom.cd = new LOWLEVEL_CDROM(bx_options.cdromd.dev);
#endif

#ifdef LOWLEVEL_CDROM
	if (bx_options.cdromd.inserted) {
	      if (BX_HD_THIS s[1].cdrom.cd->insert_cdrom()) {
		    fprintf(stderr, "disk: Media present in CD-ROM drive\n");
		    BX_HD_THIS s[1].cdrom.ready = 1;
		    BX_HD_THIS s[1].cdrom.capacity = BX_HD_THIS s[1].cdrom.cd->capacity();
	      } else {		    
		    fprintf(stderr, "disk: Could not locate CD-ROM, continuing with media not present\n");
		    BX_HD_THIS s[1].cdrom.ready = 0;
	      }
	} else {
#endif
	      fprintf(stderr, "disk: Media not present in CD-ROM drive\n");
	      BX_HD_THIS s[1].cdrom.ready = 0;
#ifdef LOWLEVEL_CDROM
	}
#endif
  }

  /* open hard drive image file */
  if (bx_options.diskc.present) {
	BX_INFO(("Opening image for device 0\n"));
	if ((BX_HD_THIS s[0].hard_drive->open(bx_options.diskc.path)) < 0) {
	      bx_panic("could not open hard drive image file '%s'\n",
		       bx_options.diskc.path);
	}
  }
  if (bx_options.diskd.present && !bx_options.cdromd.present) {
	BX_INFO(("Opening image for device 1\n"));
	if ((BX_HD_THIS s[1].hard_drive->open(bx_options.diskd.path)) < 0) {
	      bx_panic("could not open hard drive image file '%s'\n",
		       bx_options.diskd.path);
	}
  }

  // generate CMOS values for hard drive if not using a CMOS image
  if (!bx_options.cmos.cmosImage) {
    cmos->s.reg[0x12] = 0x00; // start out with: no drive 0, no drive 1

    if (bx_options.diskc.present) {
      // Flag drive type as Fh, use extended CMOS location as real type
      cmos->s.reg[0x12] = (cmos->s.reg[0x12] & 0x0f) | 0xf0;
      cmos->s.reg[0x19] = 47; // user definable type
      // AMI BIOS: 1st hard disk #cyl low byte
      cmos->s.reg[0x1b] = (bx_options.diskc.cylinders & 0x00ff);
      // AMI BIOS: 1st hard disk #cyl high byte
      cmos->s.reg[0x1c] = (bx_options.diskc.cylinders & 0xff00) >> 8;
      // AMI BIOS: 1st hard disk #heads
      cmos->s.reg[0x1d] = (bx_options.diskc.heads);
      // AMI BIOS: 1st hard disk write precompensation cylinder, low byte
      cmos->s.reg[0x1e] = 0xff; // -1
      // AMI BIOS: 1st hard disk write precompensation cylinder, high byte
      cmos->s.reg[0x1f] = 0xff; // -1
      // AMI BIOS: 1st hard disk control byte
      cmos->s.reg[0x20] = 0xc0 | ((bx_options.diskc.heads > 8) << 3);
      // AMI BIOS: 1st hard disk landing zone, low byte
      cmos->s.reg[0x21] = cmos->s.reg[0x1b];
      // AMI BIOS: 1st hard disk landing zone, high byte
      cmos->s.reg[0x22] = cmos->s.reg[0x1c];
      // AMI BIOS: 1st hard disk sectors/track
      cmos->s.reg[0x23] = bx_options.diskc.spt;
    }

    //set up cmos for second hard drive
    if (bx_options.diskd.present) {
      BX_INFO(("[diskd] I will put 0xf into the second hard disk field\n"));
      // fill in lower 4 bits of 0x12 for second HD
      cmos->s.reg[0x12] = (cmos->s.reg[0x12] & 0xf0) | 0x0f;
      cmos->s.reg[0x1a] = 47; // user definable type
      // AMI BIOS: 2nd hard disk #cyl low byte
      cmos->s.reg[0x24] = (bx_options.diskd.cylinders & 0x00ff);
      // AMI BIOS: 2nd hard disk #cyl high byte
      cmos->s.reg[0x25] = (bx_options.diskd.cylinders & 0xff00) >> 8;
      // AMI BIOS: 2nd hard disk #heads
      cmos->s.reg[0x26] = (bx_options.diskd.heads);
      // AMI BIOS: 2nd hard disk write precompensation cylinder, low byte
      cmos->s.reg[0x27] = 0xff; // -1
      // AMI BIOS: 2nd hard disk write precompensation cylinder, high byte
      cmos->s.reg[0x28] = 0xff; // -1
      // AMI BIOS: 2nd hard disk, 0x80 if heads>8
      cmos->s.reg[0x29] = (bx_options.diskd.heads > 8) ? 0x80 : 0x00;
      // AMI BIOS: 2nd hard disk landing zone, low byte
      cmos->s.reg[0x2a] = cmos->s.reg[0x1b];
      // AMI BIOS: 2nd hard disk landing zone, high byte
      cmos->s.reg[0x2b] = cmos->s.reg[0x1c];
      // AMI BIOS: 2nd hard disk sectors/track
      cmos->s.reg[0x2c] = bx_options.diskd.spt;
    }


    if ( bx_options.bootdrive[0] == 'c' ) {
      // system boot sequence C:, A:
      cmos->s.reg[0x2d] &= 0xdf;
      }
    else { // 'a'
      // system boot sequence A:, C:
      cmos->s.reg[0x2d] |= 0x20;
      }
    }

  //switch (stat_buf.st_size) {
  //  }

  for (int id = 0; id < 2; id++) {
	BX_CONTROLLER(id).status.busy           = 0;
	BX_CONTROLLER(id).status.drive_ready    = 1;
	BX_CONTROLLER(id).status.write_fault    = 0;
	BX_CONTROLLER(id).status.seek_complete  = 1;
	BX_CONTROLLER(id).status.drq            = 0;
	BX_CONTROLLER(id).status.corrected_data = 0;
	BX_CONTROLLER(id).status.index_pulse    = 0;
	BX_CONTROLLER(id).status.index_pulse_count = 0;
	BX_CONTROLLER(id).status.err            = 0;

	BX_CONTROLLER(id).error_register = 0x01; // diagnostic code: no error
	BX_CONTROLLER(id).head_no        = 0;
	BX_CONTROLLER(id).sector_count   = 1;
	BX_CONTROLLER(id).sector_no      = 1;
	BX_CONTROLLER(id).cylinder_no    = 0;
	BX_CONTROLLER(id).current_command = 0x00;
	BX_CONTROLLER(id).buffer_index = 0;

	BX_CONTROLLER(id).control.reset       = 0;
	BX_CONTROLLER(id).control.disable_irq = 0;

	BX_CONTROLLER(id).reset_in_progress   = 0;

	BX_CONTROLLER(id).sectors_per_block   = 0x80;
	BX_CONTROLLER(id).lba_mode            = 0;

	BX_CONTROLLER(id).features            = 0;
  }
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

  if (io_len==2 && address!=0x1f0) {
    bx_panic("disk: non-byte IO read to %04x\n", (unsigned) address);
    }

  switch (address) {
    case 0x1f0: // hard disk data (16bit)
      if (BX_SELECTED_CONTROLLER.status.drq == 0) {
	    bx_panic("disk: IO read(1f0h) with drq == 0: last command was %02xh\n",
		     (unsigned) BX_SELECTED_CONTROLLER.current_command);
      }
      switch (BX_SELECTED_CONTROLLER.current_command) {
        case 0x20: // read sectors, with retries
        case 0x21: // read sectors, without retries
          if (io_len != 2) {
            bx_panic("disk: non-word IO read from %04x\n",
                     (unsigned) address);
            }
          if (BX_SELECTED_CONTROLLER.buffer_index >= 512)
            bx_panic("disk: IO read(1f0): buffer_index >= 512\n");
          value16  = BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index];
          value16 |= (BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index+1] << 8);
          BX_SELECTED_CONTROLLER.buffer_index += 2;

          // if buffer completely read
          if (BX_SELECTED_CONTROLLER.buffer_index >= 512) {
            // update sector count, sector number, cylinder,
            // drive, head, status
            // if there are more sectors, read next one in...
            //
            BX_SELECTED_CONTROLLER.buffer_index = 0;

	    increment_address();

            BX_SELECTED_CONTROLLER.status.busy = 0;
            BX_SELECTED_CONTROLLER.status.drive_ready = 1;
            BX_SELECTED_CONTROLLER.status.write_fault = 0;
            if (bx_options.newHardDriveSupport)
              BX_SELECTED_CONTROLLER.status.seek_complete = 1;
            else
              BX_SELECTED_CONTROLLER.status.seek_complete = 0;
            BX_SELECTED_CONTROLLER.status.corrected_data = 0;
            BX_SELECTED_CONTROLLER.status.err = 0;

            if (BX_SELECTED_CONTROLLER.sector_count==0) {
              BX_SELECTED_CONTROLLER.status.drq = 0;
              }
            else { /* read next one into controller buffer */
              unsigned long logical_sector;
              int ret;

              BX_SELECTED_CONTROLLER.status.drq = 1;
              BX_SELECTED_CONTROLLER.status.seek_complete = 1;

	      logical_sector = calculate_logical_address();

	      ret = BX_SELECTED_HD.hard_drive->lseek(logical_sector * 512, SEEK_SET);

              if (ret < 0)
                bx_panic("disk: could lseek() hard drive image file\n");
	      ret = BX_SELECTED_HD.hard_drive->read((bx_ptr_t) BX_SELECTED_CONTROLLER.buffer, 512);
              if (ret < 512) {
                BX_INFO(("logical sector was %u\n", (unsigned) logical_sector));
                bx_panic("disk: could not read() hard drive image file at byte %d\n", logical_sector*512);
                }

              BX_SELECTED_CONTROLLER.buffer_index = 0;
	      raise_interrupt();
	    }
	  }
	  goto return_value16;
          break;

        case 0xec:    // Drive ID Command
	case 0xa1:
          if (bx_options.newHardDriveSupport) {
            unsigned index;

            BX_SELECTED_CONTROLLER.status.busy = 0;
            BX_SELECTED_CONTROLLER.status.drive_ready = 1;
            BX_SELECTED_CONTROLLER.status.write_fault = 0;
            BX_SELECTED_CONTROLLER.status.seek_complete = 1;
            BX_SELECTED_CONTROLLER.status.corrected_data = 0;
            BX_SELECTED_CONTROLLER.status.err = 0;

            index = BX_SELECTED_CONTROLLER.buffer_index;
            value32 = BX_SELECTED_CONTROLLER.buffer[index];
            index++;
            if (io_len >= 2) {
              value32 |= (BX_SELECTED_CONTROLLER.buffer[index] << 8);
              index++;
              }
            if (io_len == 4) {
              value32 |= (BX_SELECTED_CONTROLLER.buffer[index] << 16);
              value32 |= (BX_SELECTED_CONTROLLER.buffer[index+1] << 24);
              index += 2;
              }
            BX_SELECTED_CONTROLLER.buffer_index = index;

            if (BX_SELECTED_CONTROLLER.buffer_index >= 512) {
              BX_SELECTED_CONTROLLER.status.drq = 0;
	      if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		    BX_INFO(("disk: Read all drive ID Bytes ...\n"));
              }
	    if (io_len == 1) {
		  value8 = (Bit8u)value32;
		  goto return_value8;
	    } else if (io_len == 2) {
		  value16 = (Bit16u)value32;
		  goto return_value16;
	    } else {
		  goto return_value32;
            }
	  }
          else
            bx_panic("disk: IO read(1f0h): current command is %02xh\n",
              (unsigned) BX_SELECTED_CONTROLLER.current_command);

	    case 0xa0: {
		  unsigned index = BX_SELECTED_CONTROLLER.buffer_index;

		  // Load block if necessary
		  if (index >= 2048) {
			if (index > 2048)
			      bx_panic("disk: index > 2048\n");
			switch (BX_SELECTED_HD.atapi.command) {
			      case 0x28: // read (10)
			      case 0xa8: // read (12)
#ifdef LOWLEVEL_CDROM
				    BX_SELECTED_HD.cdrom.cd->read_block(BX_SELECTED_CONTROLLER.buffer,
									BX_SELECTED_HD.cdrom.next_lba);
				    BX_SELECTED_HD.cdrom.next_lba++;
				    BX_SELECTED_HD.cdrom.remaining_blocks--;

				    if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
					  if (!BX_SELECTED_HD.cdrom.remaining_blocks)
						BX_INFO(("disk: Last READ block loaded {CDROM}\n"));
					  else
						BX_INFO(("disk: READ block loaded (%d remaining) {CDROM}\n",
							  BX_SELECTED_HD.cdrom.remaining_blocks));

				    // one block transfered
				    BX_SELECTED_HD.atapi.drq_bytes -= 2048;
				    BX_SELECTED_HD.atapi.total_bytes_remaining -= 2048;
				    index = 0;
#else
				    bx_panic("Read with no LOWLEVEL_CDROM\n");
#endif
				    break;

			      default: // no need to load a new block
				    break;
			}
		  }

		  value32 = BX_SELECTED_CONTROLLER.buffer[index];
		  index++;
		  if (io_len >= 2) {
			value32 |= (BX_SELECTED_CONTROLLER.buffer[index] << 8);
			index++;
		  }
		  if (io_len == 4) {
			value32 |= (BX_SELECTED_CONTROLLER.buffer[index] << 16);
			value32 |= (BX_SELECTED_CONTROLLER.buffer[index+1] << 24);
			index += 2;
		  }
		  BX_SELECTED_CONTROLLER.buffer_index = index;

		  if (BX_SELECTED_CONTROLLER.buffer_index >= (unsigned)BX_SELECTED_HD.atapi.drq_bytes) {
			BX_SELECTED_CONTROLLER.status.drq = 0;

			BX_SELECTED_HD.atapi.total_bytes_remaining -= BX_SELECTED_HD.atapi.drq_bytes;

			if (BX_SELECTED_HD.atapi.total_bytes_remaining > 0) {
			      // one or more blocks remaining (works only for single block commands)
			      if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
				    BX_INFO(("disk: PACKET drq bytes read\n"));
			      BX_SELECTED_CONTROLLER.interrupt_reason.i_o = 1;
			      BX_SELECTED_CONTROLLER.status.busy = 0;
			      BX_SELECTED_CONTROLLER.status.drq = 1;
			      BX_SELECTED_CONTROLLER.interrupt_reason.c_d = 0;

			      // set new byte count if last block
			      if (BX_SELECTED_HD.atapi.total_bytes_remaining < BX_SELECTED_CONTROLLER.byte_count) {
				    BX_SELECTED_CONTROLLER.byte_count = BX_SELECTED_HD.atapi.total_bytes_remaining;
			      }
			      BX_SELECTED_HD.atapi.drq_bytes += BX_SELECTED_CONTROLLER.byte_count;

			      raise_interrupt();
			} else {
			      // all bytes read
			      if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
				    BX_INFO(("disk: PACKET all bytes read\n"));
			      BX_SELECTED_CONTROLLER.interrupt_reason.i_o = 1;
			      BX_SELECTED_CONTROLLER.interrupt_reason.c_d = 1;
			      BX_SELECTED_CONTROLLER.status.drive_ready = 1;
			      BX_SELECTED_CONTROLLER.interrupt_reason.rel = 0;
			      BX_SELECTED_CONTROLLER.status.busy = 0;
			      BX_SELECTED_CONTROLLER.status.drq = 0;
			      BX_SELECTED_CONTROLLER.status.err = 0;
			      
			      raise_interrupt();
			}
		  }
		  if (io_len == 1) {
			value8 = (Bit8u)value32;
			goto return_value8;
		  } else if (io_len == 2) {
			value16 = (Bit16u)value32;
			goto return_value16;
		  } else {
			goto return_value32;
		  }
		  break;
	    }

        default:
          bx_panic("disk: IO read(1f0h): current command is %02xh\n",
            (unsigned) BX_SELECTED_CONTROLLER.current_command);
        }
      break;

    case 0x1f1: // hard disk error register
      BX_SELECTED_CONTROLLER.status.err = 0;
      value8 = BX_SELECTED_CONTROLLER.error_register;
      goto return_value8;
      break;
    case 0x1f2: // hard disk sector count / interrupt reason
      value8 = BX_SELECTED_CONTROLLER.sector_count;
      goto return_value8;
      break;
    case 0x1f3: // sector number
      value8 = BX_SELECTED_CONTROLLER.sector_no;
      goto return_value8;
    case 0x1f4: // cylinder low
      value8 = (BX_SELECTED_CONTROLLER.cylinder_no & 0x00ff);
      goto return_value8;
    case 0x1f5: // cylinder high
      value8 = BX_SELECTED_CONTROLLER.cylinder_no >> 8;
      goto return_value8;

    case 0x1f6: // hard disk drive and head register
      value8 = (1 << 7) | // extended data field for ECC
               (0 << 7) | // 1=LBA mode, 0=CHSmode
               (1 << 5) | // 01b = 512 sector size
               (BX_HD_THIS drive_select << 4) |
               (BX_SELECTED_CONTROLLER.head_no << 0);
      goto return_value8;
      break;

    case 0x1f7: // Hard Disk Status
    case 0x3f6: // Hard Disk Alternate Status
      if (BX_HD_THIS drive_select && !bx_options.diskd.present) {
	    // (mch) Just return zero for these registers
	    value8 = 0;
      } else {
      value8 = (
        (BX_SELECTED_CONTROLLER.status.busy << 7) |
        (BX_SELECTED_CONTROLLER.status.drive_ready << 6) |
        (BX_SELECTED_CONTROLLER.status.write_fault << 5) |
        (BX_SELECTED_CONTROLLER.status.seek_complete << 4) |
        (BX_SELECTED_CONTROLLER.status.drq << 3) |
        (BX_SELECTED_CONTROLLER.status.corrected_data << 2) |
        (BX_SELECTED_CONTROLLER.status.index_pulse << 1) |
        (BX_SELECTED_CONTROLLER.status.err) );
      BX_SELECTED_CONTROLLER.status.index_pulse_count++;
      BX_SELECTED_CONTROLLER.status.index_pulse = 0;
      if (BX_SELECTED_CONTROLLER.status.index_pulse_count >= INDEX_PULSE_CYCLE) {
        BX_SELECTED_CONTROLLER.status.index_pulse = 1;
        BX_SELECTED_CONTROLLER.status.index_pulse_count = 0;
        }
      }
      goto return_value8;
      break;

    case 0x3f7: // Hard Disk Address Register
      // Obsolete and unsupported register.  Not driven by hard
      // disk controller.  Report all 1's.  If floppy controller
      // is handling this address, it will call this function
      // set/clear D7 (the only bit it handles), then return
      // the combined value
      value8 = 0xff;
      goto return_value8;
      break;

#if 0
      // you'll need these to support second IDE controller, not needed yet.
      case 0x170:
      case 0x171:
      case 0x172:
      case 0x173:
      case 0x174:
      case 0x175:
      case 0x176:
      case 0x177:
	    BX_INFO(("[disk] ignoring read from 0x%04x\n", address));
       break;
#endif
    default:
      bx_panic("hard drive: io read to address %x unsupported\n",
        (unsigned) address);
    }

  bx_panic("hard drive: shouldnt get here!\n");
  return(0);

  return_value32:
  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
    BX_INFO(("disk: 32-bit read from %04x = %08x {%s}\n",
	      (unsigned) address, value32, DEVICE_TYPE_STRING));
  return value32;

  return_value16:
  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
    BX_INFO(("disk: 16-bit read from %04x = %04x {%s}\n",
	      (unsigned) address, value16, DEVICE_TYPE_STRING));
  return value16;

  return_value8:
  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
    BX_INFO(("disk: 8-bit read from %04x = %02x {%s}\n",
	      (unsigned) address, value8, DEVICE_TYPE_STRING));
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
  unsigned long logical_sector;
  int ret;
  Boolean prev_control_reset;

  if (io_len==2 && address!=0x1f0) {
    bx_panic("disk: non-byte IO write to %04x\n", (unsigned) address);
    }

  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom)) {
	switch (io_len) {
	      case 1:
		    BX_INFO(("disk: 8-bit write to %04x = %02x {%s}\n",
			      (unsigned) address, (unsigned) value, DEVICE_TYPE_STRING));
		    break;
		    
	      case 2:
		    BX_INFO(("disk: 16-bit write to %04x = %04x {%s}\n",
			      (unsigned) address, (unsigned) value, DEVICE_TYPE_STRING));
		    break;

	      case 4:
		    BX_INFO(("disk: 32-bit write to %04x = %08x {%s}\n",
			      (unsigned) address, (unsigned) value, DEVICE_TYPE_STRING));
		    break;

	      default:
		    BX_INFO(("disk: unknown-size write to %04x = %08x {%s}\n",
			      (unsigned) address, (unsigned) value, DEVICE_TYPE_STRING));
		    break;
	}
  }

//BX_INFO(("disk: IO write to %04x = %02x\n",
//      (unsigned) address, (unsigned) value));

  switch (address) {
    case 0x1f0:
      if (io_len != 2) {
        bx_panic("disk: non-word IO read from %04x\n", (unsigned) address);
        }
      switch (BX_SELECTED_CONTROLLER.current_command) {
        case 0x30:
          if (BX_SELECTED_CONTROLLER.buffer_index >= 512)
            bx_panic("disk: IO write(1f0): buffer_index >= 512\n");
          BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index] = value;
          BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index+1] = (value >> 8);
          BX_SELECTED_CONTROLLER.buffer_index += 2;

          /* if buffer completely writtten */
          if (BX_SELECTED_CONTROLLER.buffer_index >= 512) {
            unsigned long logical_sector;
            int ret;

	    logical_sector = calculate_logical_address();

	    ret = BX_SELECTED_HD.hard_drive->lseek(logical_sector * 512, SEEK_SET);
            if (ret < 0)
              bx_panic("disk: could lseek() hard drive image file\n");

	    ret = BX_SELECTED_HD.hard_drive->write((bx_ptr_t) BX_SELECTED_CONTROLLER.buffer, 512);
            if (ret < 512)
              bx_panic("disk: could not write() hard drive image file at byte %d\n", logical_sector*512);

            BX_SELECTED_CONTROLLER.buffer_index = 0;

            /* update sector count, sector number, cylinder,
             * drive, head, status
             * if there are more sectors, read next one in...
             */

	    increment_address();

            /* When the write is complete, controller clears the DRQ bit and
             * sets the BSY bit.
             * If at least one more sector is to be written, controller sets DRQ bit,
             * clears BSY bit, and issues IRQ 14
             */

            if (BX_SELECTED_CONTROLLER.sector_count!=0) {
              BX_SELECTED_CONTROLLER.status.busy = 0;
              BX_SELECTED_CONTROLLER.status.drive_ready = 1;
              BX_SELECTED_CONTROLLER.status.drq = 1;
              BX_SELECTED_CONTROLLER.status.corrected_data = 0;
              BX_SELECTED_CONTROLLER.status.err = 0;
              }
            else { /* no more sectors to write */
              BX_SELECTED_CONTROLLER.status.busy = 0;
              BX_SELECTED_CONTROLLER.status.drive_ready = 1;
              BX_SELECTED_CONTROLLER.status.drq = 0;
              BX_SELECTED_CONTROLLER.status.err = 0;
              BX_SELECTED_CONTROLLER.status.corrected_data = 0;
              }
	    raise_interrupt();
            }
          break;

	    case 0xa0: // PACKET
		  if (BX_SELECTED_CONTROLLER.buffer_index >= PACKET_SIZE)
			bx_panic("disk: IO write(1f0): buffer_index >= PACKET_SIZE\n");
		  BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index] = value;
		  BX_SELECTED_CONTROLLER.buffer[BX_SELECTED_CONTROLLER.buffer_index+1] = (value >> 8);
		  BX_SELECTED_CONTROLLER.buffer_index += 2;

		  /* if packet completely writtten */
		  if (BX_SELECTED_CONTROLLER.buffer_index >= PACKET_SIZE) {
			// complete command received
			Bit8u atapi_command = BX_SELECTED_CONTROLLER.buffer[0];
			int alloc_length;

			if (bx_dbg.cdrom)
				BX_INFO(("cdrom: ATAPI command 0x%x started\n", atapi_command));

			switch (atapi_command) {
			      case 0x00: // test unit ready
				    if (BX_SELECTED_HD.cdrom.ready) {
					  atapi_cmd_nop();
				    } else {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
				    }
				    raise_interrupt();
				    break;

			      case 0x03: { // request sense
				    int alloc_length = BX_SELECTED_CONTROLLER.buffer[4];
				    init_send_atapi_command(atapi_command, 18, alloc_length);

				    // sense data
				    BX_SELECTED_CONTROLLER.buffer[0] = 0x70 | (1 << 7);
				    BX_SELECTED_CONTROLLER.buffer[1] = 0;
				    BX_SELECTED_CONTROLLER.buffer[2] = BX_SELECTED_HD.sense.sense_key;
				    BX_SELECTED_CONTROLLER.buffer[3] = BX_SELECTED_HD.sense.information.arr[0];
				    BX_SELECTED_CONTROLLER.buffer[4] = BX_SELECTED_HD.sense.information.arr[1];
				    BX_SELECTED_CONTROLLER.buffer[5] = BX_SELECTED_HD.sense.information.arr[2];
				    BX_SELECTED_CONTROLLER.buffer[6] = BX_SELECTED_HD.sense.information.arr[3];
				    BX_SELECTED_CONTROLLER.buffer[7] = 17-7;
				    BX_SELECTED_CONTROLLER.buffer[8] = BX_SELECTED_HD.sense.specific_inf.arr[0];
				    BX_SELECTED_CONTROLLER.buffer[9] = BX_SELECTED_HD.sense.specific_inf.arr[1];
				    BX_SELECTED_CONTROLLER.buffer[10] = BX_SELECTED_HD.sense.specific_inf.arr[2];
				    BX_SELECTED_CONTROLLER.buffer[11] = BX_SELECTED_HD.sense.specific_inf.arr[3];
				    BX_SELECTED_CONTROLLER.buffer[12] = BX_SELECTED_HD.sense.asc;
				    BX_SELECTED_CONTROLLER.buffer[13] = BX_SELECTED_HD.sense.ascq;
				    BX_SELECTED_CONTROLLER.buffer[14] = BX_SELECTED_HD.sense.fruc;
				    BX_SELECTED_CONTROLLER.buffer[15] = BX_SELECTED_HD.sense.key_spec.arr[0];
				    BX_SELECTED_CONTROLLER.buffer[16] = BX_SELECTED_HD.sense.key_spec.arr[1];
				    BX_SELECTED_CONTROLLER.buffer[17] = BX_SELECTED_HD.sense.key_spec.arr[2];

				    ready_to_send_atapi();
			      }
			      break;
			      
			      case 0x1b: { // start stop unit
				    //Boolean Immed = (BX_SELECTED_CONTROLLER.buffer[1] >> 0) & 1;
				    Boolean LoEj = (BX_SELECTED_CONTROLLER.buffer[4] >> 1) & 1;
				    Boolean Start = (BX_SELECTED_CONTROLLER.buffer[4] >> 0) & 1;

				    if (!LoEj && !Start) { // stop the disc
					  bx_panic("disk: Stop disc not implemented\n");
				    } else if (!LoEj && Start) { // start the disc and read the TOC
					  bx_panic("disk: Start disc not implemented\n");
				    } else if (LoEj && !Start) { // Eject the disc
					  bx_panic("disk: Eject the disc not implemented\n");
				    } else { // Load the disc
					  // My guess is that this command only closes the tray, that's a no-op for us
					  atapi_cmd_nop();
					  raise_interrupt();
				    }
			      }
			      break;

			      case 0xbd: { // mechanism status
				    uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER.buffer + 8);

				    if (alloc_length == 0)
					  bx_panic("disk: Zero allocation length to MECHANISM STATUS not impl.\n");

				    init_send_atapi_command(atapi_command, 8, alloc_length);

				    BX_SELECTED_CONTROLLER.buffer[0] = 0; // reserved for non changers
				    BX_SELECTED_CONTROLLER.buffer[1] = 0; // reserved for non changers

				    BX_SELECTED_CONTROLLER.buffer[2] = 0; // Current LBA (TODO!)
				    BX_SELECTED_CONTROLLER.buffer[3] = 0; // Current LBA (TODO!)
				    BX_SELECTED_CONTROLLER.buffer[4] = 0; // Current LBA (TODO!)

				    BX_SELECTED_CONTROLLER.buffer[5] = 1; // one slot

				    BX_SELECTED_CONTROLLER.buffer[6] = 0; // slot table length
				    BX_SELECTED_CONTROLLER.buffer[7] = 0; // slot table length

				    ready_to_send_atapi();
			      }
			      break;

			      case 0x5a: { // mode sense
				    uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER.buffer + 7);

				    Bit8u PC = BX_SELECTED_CONTROLLER.buffer[2] >> 6;
				    Bit8u PageCode = BX_SELECTED_CONTROLLER.buffer[2] & 0x3f;

				    switch (PC) {
					  case 0x0: // current values
						switch (PageCode) {
						      case 0x01: // error recovery
							    init_send_atapi_command(atapi_command, sizeof(error_recovery_t) + 8, alloc_length);

							    init_mode_sense_single(&BX_SELECTED_HD.cdrom.current.error_recovery,
										   sizeof(error_recovery_t));
							    ready_to_send_atapi();
							    break;

						      case 0x2a: // CD-ROM capabilities & mech. status
							      init_send_atapi_command(atapi_command, 28, alloc_length);
							      init_mode_sense_single(&BX_SELECTED_CONTROLLER.buffer[8], 28);
							      BX_SELECTED_CONTROLLER.buffer[8] = 0x2a;
							      BX_SELECTED_CONTROLLER.buffer[9] = 0x12;
							      BX_SELECTED_CONTROLLER.buffer[10] = 0x00;
							      BX_SELECTED_CONTROLLER.buffer[11] = 0x00;
							      BX_SELECTED_CONTROLLER.buffer[12] = 0x00;
							      BX_SELECTED_CONTROLLER.buffer[13] = (3 << 5);
							      BX_SELECTED_CONTROLLER.buffer[14] = (unsigned char)
(1 |
								      (BX_SELECTED_HD.cdrom.locked ? (1 << 1) : 0) |
								      (1 << 3) |
								      (1 << 5));
							      BX_SELECTED_CONTROLLER.buffer[15] = 0x00;
							      BX_SELECTED_CONTROLLER.buffer[16] = (706 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[17] = 706 & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[18] = 0;
							      BX_SELECTED_CONTROLLER.buffer[19] = 2;
							      BX_SELECTED_CONTROLLER.buffer[20] = (512 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[21] = 512 & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[22] = (706 >> 8) & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[23] = 706 & 0xff;
							      BX_SELECTED_CONTROLLER.buffer[24] = 0;
							      BX_SELECTED_CONTROLLER.buffer[25] = 0;
							      BX_SELECTED_CONTROLLER.buffer[26] = 0;
							      BX_SELECTED_CONTROLLER.buffer[27] = 0;
							      ready_to_send_atapi();
							      break;

						      case 0x0d: // CD-ROM
						      case 0x0e: // CD-ROM audio control
						      case 0x3f: // all
							    bx_panic("cdrom: MODE SENSE (curr), code=%x\n",
								     PageCode);
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device\n",
								      PC, PageCode));
							    atapi_cmd_error(SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt();
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
							    bx_panic("cdrom: MODE SENSE (chg), code=%x\n",
								     PageCode);
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device\n",
								      PC, PageCode));
							    atapi_cmd_error(SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt();
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
							    bx_panic("cdrom: MODE SENSE (dflt), code=%x\n",
								     PageCode);
							    break;

						      default:
							    // not implemeted by this device
							    BX_INFO(("cdrom: MODE SENSE PC=%x, PageCode=%x,"
								      " not implemented by device\n",
								      PC, PageCode));
							    atapi_cmd_error(SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt();
							    break;
						}
						break;

					  case 0x3: // saved values not implemented
						atapi_cmd_error(SENSE_ILLEGAL_REQUEST, ASC_SAVING_PARAMETERS_NOT_SUPPORTED);
						raise_interrupt();
						break;

					  default:
						bx_panic("disk: Should not get here!\n");
						break;
				    }
			      }
			      break;

			      case 0x12: { // inquiry
				    uint8 alloc_length = BX_SELECTED_CONTROLLER.buffer[4];

				    init_send_atapi_command(atapi_command, 36, alloc_length);

				    BX_SELECTED_CONTROLLER.buffer[0] = 0x05; // CD-ROM
				    BX_SELECTED_CONTROLLER.buffer[1] = 0x80; // Removable
				    BX_SELECTED_CONTROLLER.buffer[2] = 0x00; // ISO, ECMA, ANSI version
				    BX_SELECTED_CONTROLLER.buffer[3] = 0x21; // ATAPI-2, as specified
				    BX_SELECTED_CONTROLLER.buffer[4] = 31; // additional length (total 36)
				    BX_SELECTED_CONTROLLER.buffer[5] = 0x00; // reserved
				    BX_SELECTED_CONTROLLER.buffer[6] = 0x00; // reserved
				    BX_SELECTED_CONTROLLER.buffer[7] = 0x00; // reserved

				    // Vendor ID
				    const char* vendor_id = "VTAB\0\0\0\0";
                                    int i;
				    for (i = 0; i < 8; i++)
					  BX_SELECTED_CONTROLLER.buffer[8+i] = vendor_id[i];

				    // Product ID
				    const char* product_id = "Turbo CD-ROM\0\0\0\0";
				    for (i = 0; i < 16; i++)
					  BX_SELECTED_CONTROLLER.buffer[16+i] = product_id[i];

				    // Product Revision level
				    const char* rev_level = "R0\0\0";
				    for (i = 0; i < 4; i++)
					  BX_SELECTED_CONTROLLER.buffer[32+i] = rev_level[i];

				    ready_to_send_atapi();
			      }
			      break;

			      case 0x25: { // read cd-rom capacity
				    // no allocation length???
				    init_send_atapi_command(atapi_command, 8, 8);

				    if (BX_SELECTED_HD.cdrom.ready) {
					  uint32 capacity = BX_SELECTED_HD.cdrom.capacity;
					  BX_INFO(("disk: Capacity is %d sectors (%d bytes)\n", capacity, capacity * 2048));
					  BX_SELECTED_CONTROLLER.buffer[0] = (capacity >> 24) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[1] = (capacity >> 16) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[2] = (capacity >> 8) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[3] = (capacity >> 0) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[4] = (2048 >> 24) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[5] = (2048 >> 16) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[6] = (2048 >> 8) & 0xff;
					  BX_SELECTED_CONTROLLER.buffer[7] = (2048 >> 0) & 0xff;
					  ready_to_send_atapi();
				    } else {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt();
				    }
			      }
			      break;

			      case 0xbe: { // read cd
				    if (BX_SELECTED_HD.cdrom.ready) {
					  bx_panic("Read CD with CD present not implemented\n");
				    } else {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt();
				    }
			      }
			      break;

			      case 0x43: { // read toc
				    if (BX_SELECTED_HD.cdrom.ready) {
#ifdef LOWLEVEL_CDROM
					  bool msf = (BX_SELECTED_CONTROLLER.buffer[1] >> 1) & 1;
					  uint8 starting_track = BX_SELECTED_CONTROLLER.buffer[6];
#endif
					  uint16 alloc_length = read_16bit(BX_SELECTED_CONTROLLER.buffer + 7);

					  uint8 format = (BX_SELECTED_CONTROLLER.buffer[9] >> 6);
                                          int i;
					  switch (format) {
						case 0:
#ifdef LOWLEVEL_CDROM
						      int toc_length;
						      if (!BX_SELECTED_HD.cdrom.cd->read_toc(BX_SELECTED_CONTROLLER.buffer,
										       &toc_length, msf, starting_track)) {
							    atapi_cmd_error(SENSE_ILLEGAL_REQUEST,
									    ASC_INV_FIELD_IN_CMD_PACKET);
							    raise_interrupt();
						      } else {
							    init_send_atapi_command(atapi_command, toc_length, alloc_length);
							    ready_to_send_atapi();
						      }
#else
						      bx_panic("LOWLEVEL_CDROM not defined\n");
#endif
						      break;

						case 1:
						      // multi session stuff. we ignore this and emulate a single session only
						      init_send_atapi_command(atapi_command, 12, alloc_length);

						      BX_SELECTED_CONTROLLER.buffer[0] = 0;
						      BX_SELECTED_CONTROLLER.buffer[1] = 0x0a;
						      BX_SELECTED_CONTROLLER.buffer[2] = 1;
						      BX_SELECTED_CONTROLLER.buffer[3] = 1;
						      for (i = 0; i < 8; i++)
							    BX_SELECTED_CONTROLLER.buffer[4+i] = 0;

						      ready_to_send_atapi();
						      break;

						case 2:
						default:
						      bx_panic("disk: (READ TOC) Format %d not supported\n", format);
						      break;
					  }
				    } else {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt();
				    }
			      }
			      break;

			      case 0x28: { // read (10)
				    uint32 transfer_length = read_16bit(BX_SELECTED_CONTROLLER.buffer + 7);
				    uint32 lba = read_32bit(BX_SELECTED_CONTROLLER.buffer + 2);

				    if (!BX_SELECTED_HD.cdrom.ready) {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt();
					  break;
				    }

				    if (transfer_length == 0) {
					  atapi_cmd_nop();
					  raise_interrupt();
					  BX_INFO(("disk: READ(10) with transfer length 0, ok\n"));
					  break;
				    }

				    if (lba + transfer_length > BX_SELECTED_HD.cdrom.capacity) {
					  atapi_cmd_error(SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR);
					  raise_interrupt();
					  break;
				    }

				    //BX_INFO(("cdrom: READ LBA=%d LEN=%d\n", lba, transfer_length));

				    // handle command
				    init_send_atapi_command(atapi_command, transfer_length * 2048,
							    transfer_length * 2048, true);
				    BX_SELECTED_HD.cdrom.remaining_blocks = transfer_length;
				    BX_SELECTED_HD.cdrom.next_lba = lba;
				    ready_to_send_atapi();
			      }
			      break;

				case 0x2b: { // seek
					uint32 lba = read_32bit(BX_SELECTED_CONTROLLER.buffer + 2);
					if (!BX_SELECTED_HD.cdrom.ready) {
						atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
						raise_interrupt();
						break;
					}

					if (lba > BX_SELECTED_HD.cdrom.capacity) {
						atapi_cmd_error(SENSE_ILLEGAL_REQUEST, ASC_LOGICAL_BLOCK_OOR);
						raise_interrupt();
						break;
					}
					BX_INFO(("cdrom: SEEK (ignored)\n"));
					atapi_cmd_nop();
					raise_interrupt();
				}
				break;

			      case 0x1e: { // prevent/allow medium removal
				    if (BX_SELECTED_HD.cdrom.ready) {
					  BX_SELECTED_HD.cdrom.locked = BX_SELECTED_CONTROLLER.buffer[4] & 1;
					  atapi_cmd_nop();
				    } else {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
				    }
				    raise_interrupt();
			      }
			      break;

			      case 0x42: { // read sub-channel
				    bool msf = get_packet_field(1, 1, 1);
				    bool sub_q = get_packet_field(2, 6, 1);
				    uint8 data_format = get_packet_byte(3);
				    uint8 track_number = get_packet_byte(6);
				    uint16 alloc_length = get_packet_word(7);
                                    UNUSED(msf);
                                    UNUSED(data_format);
                                    UNUSED(track_number);

				    if (BX_SELECTED_HD.cdrom.ready) {
					  atapi_cmd_error(SENSE_NOT_READY, ASC_MEDIUM_NOT_PRESENT);
					  raise_interrupt();
				    } else {
					  BX_SELECTED_CONTROLLER.buffer[0] = 0;
					  BX_SELECTED_CONTROLLER.buffer[1] = 0; // audio not supported
					  BX_SELECTED_CONTROLLER.buffer[2] = 0;
					  BX_SELECTED_CONTROLLER.buffer[3] = 0;

					  int ret_len = 4; // header size

					  if (sub_q) { // !sub_q == header only
						bx_panic("Read sub-channel with SubQ not implemented\n");
					  }

					  init_send_atapi_command(atapi_command, ret_len, alloc_length);
					  ready_to_send_atapi();
				    }
			      }
			      break;

			      case 0xa8: // read (12)
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
			      default:
				    bx_panic("Unknown ATAPI command 0x%x (%d)\n",
					     atapi_command, atapi_command);
				    break;
			}
		  }

		  break;

        default:
          bx_panic("disk: IO write(1f0h): current command is %02xh\n",
            (unsigned) BX_SELECTED_CONTROLLER.current_command);
        }
      break;

    case 0x1f1: /* hard disk write precompensation */
	  WRITE_FEATURES(value);
	  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom)) {
		if (value == 0xff)
		      BX_INFO(("disk: no precompensation {%s}\n", DEVICE_TYPE_STRING));
		else
		      BX_INFO(("disk: precompensation value %02x {%s}\n", (unsigned) value, DEVICE_TYPE_STRING));
	  }
      break;

    case 0x1f2: /* hard disk sector count */
	  WRITE_SECTOR_COUNT(value);
	  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		BX_INFO(("disk: sector count = %u {%s}\n", (unsigned) value, DEVICE_TYPE_STRING));
	  break;

    case 0x1f3: /* hard disk sector number */
	  WRITE_SECTOR_NUMBER(value);
	  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		BX_INFO(("disk: sector number = %u {%s}\n", (unsigned) value, DEVICE_TYPE_STRING));
      break;

    case 0x1f4: /* hard disk cylinder low */
	  WRITE_CYLINDER_LOW(value);
	  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		BX_INFO(("disk: cylinder low = %02xh {%s}\n", (unsigned) value, DEVICE_TYPE_STRING));
	  break;

    case 0x1f5: /* hard disk cylinder high */
	  WRITE_CYLINDER_HIGH(value);
	  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		BX_INFO(("disk: cylinder high = %02xh {%s}\n", (unsigned) value, DEVICE_TYPE_STRING));
	  break;

    case 0x1f6: // hard disk drive and head register
      // b7 1
      // b6 1=LBA mode, 0=CHS mode (LBA not supported)
      // b5 1
      // b4: DRV
      // b3..0 HD3..HD0
      if ( (value & 0xe0) != 0xa0 ) // 101xxxxx
        BX_INFO(("disk: IO write 1f6 (%02x): not 101xxxxxb\n", (unsigned) value));
      BX_HD_THIS drive_select = (value >> 4) & 0x01;
      WRITE_HEAD_NO(value & 0xf);
      if (BX_SELECTED_CONTROLLER.lba_mode == 0 && ((value >> 6) & 1) == 1)
	    BX_INFO(("disk: enabling LBA mode\n"));
      WRITE_LBA_MODE((value >> 6) & 1);
      break;

    case 0x1f7: // hard disk command
	  // (mch) Writes to the command register with drive_select != 0
	  // are ignored if no secondary device is present
      if (BX_HD_THIS drive_select != 0 && value != 0x90 && !bx_options.diskd.present)
	    break;

      if (BX_SELECTED_CONTROLLER.status.busy)
        bx_panic("hard disk: command sent, controller BUSY\n");
      if ( (value & 0xf0) == 0x10 )
        value = 0x10;
      switch (value) {

        case 0x10: // calibrate drive
	  if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: calibrate drive issued to non-disk\n");
          if (BX_HD_THIS drive_select != 0 && !bx_options.diskd.present) {
            BX_SELECTED_CONTROLLER.error_register = 0x02; // Track 0 not found
            BX_SELECTED_CONTROLLER.status.busy = 0;
            BX_SELECTED_CONTROLLER.status.drive_ready = 1;
            BX_SELECTED_CONTROLLER.status.seek_complete = 0;
            BX_SELECTED_CONTROLLER.status.drq = 0;
            BX_SELECTED_CONTROLLER.status.err = 1;
	    raise_interrupt();
            BX_INFO(("disk: calibrate drive != 0, with diskd not present\n"));
            break;
            }

          /* move head to cylinder 0, issue IRQ 14 */
          BX_SELECTED_CONTROLLER.error_register = 0;
          BX_SELECTED_CONTROLLER.cylinder_no = 0;
          BX_SELECTED_CONTROLLER.status.busy = 0;
          BX_SELECTED_CONTROLLER.status.drive_ready = 1;
          BX_SELECTED_CONTROLLER.status.seek_complete = 1;
          BX_SELECTED_CONTROLLER.status.drq = 0;
          BX_SELECTED_CONTROLLER.status.err = 0;
	  raise_interrupt();
          break;

        case 0x20: // read multiple sectors, with retries
        case 0x21: // read multiple sectors, without retries
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 14
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

	  if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: read multiple issued to non-disk\n");

          BX_SELECTED_CONTROLLER.current_command = value;

	  // Lose98 accesses 0/0/0 in CHS mode
	  if (!BX_SELECTED_CONTROLLER.lba_mode &&
	      !BX_SELECTED_CONTROLLER.head_no &&
	      !BX_SELECTED_CONTROLLER.cylinder_no &&
	      !BX_SELECTED_CONTROLLER.sector_no) {
		BX_INFO(("disk: Read from 0/0/0, aborting command\n"));
		command_aborted(value);
		break;
	  }

	  logical_sector = calculate_logical_address();

	  ret = BX_SELECTED_HD.hard_drive->lseek(logical_sector * 512, SEEK_SET);

          if (ret < 0) {
            bx_panic("disk: could not lseek() hard drive image file\n");
            }

	  ret = BX_SELECTED_HD.hard_drive->read((bx_ptr_t) BX_SELECTED_CONTROLLER.buffer, 512);
          if (ret < 512) {
            BX_INFO(("logical sector was %u\n", (unsigned) logical_sector));
            bx_panic("disk: could not read() hard drive image file at byte %d\n", logical_sector*512);
            }

          BX_SELECTED_CONTROLLER.error_register = 0;
          BX_SELECTED_CONTROLLER.status.busy  = 0;
          BX_SELECTED_CONTROLLER.status.drive_ready = 1;
          BX_SELECTED_CONTROLLER.status.seek_complete = 1;
          BX_SELECTED_CONTROLLER.status.drq   = 1;
          BX_SELECTED_CONTROLLER.status.corrected_data = 0;
          BX_SELECTED_CONTROLLER.status.err   = 0;
          BX_SELECTED_CONTROLLER.buffer_index = 0;
	  raise_interrupt();
          break;

        case 0x30: /* write multiple sectors, with retries */
          /* update sector_no, always points to current sector
           * after each sector is read to buffer, DRQ bit set and issue IRQ 14
           * if interrupt handler transfers all data words into main memory,
           * and more sectors to read, then set BSY bit again, clear DRQ and
           * read next sector into buffer
           * sector count of 0 means 256 sectors
           */

	  if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: write multiple issued to non-disk\n");

          if (BX_SELECTED_CONTROLLER.status.busy) {
            bx_panic("disk: write command: BSY bit set\n");
            }
          BX_SELECTED_CONTROLLER.current_command = value;

          // implicit seek done :^)
          BX_SELECTED_CONTROLLER.error_register = 0;
          BX_SELECTED_CONTROLLER.status.busy = 0;
          // BX_SELECTED_CONTROLLER.status.drive_ready = 1;
          BX_SELECTED_CONTROLLER.status.seek_complete = 1;
          BX_SELECTED_CONTROLLER.status.drq = 1;
          BX_SELECTED_CONTROLLER.status.err   = 0;
          BX_SELECTED_CONTROLLER.buffer_index = 0;
          break;

        case 0x90: // Drive Diagnostic
          if (BX_SELECTED_CONTROLLER.status.busy) {
            bx_panic("disk: diagnostic command: BSY bit set\n");
            }
	  if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: drive diagnostics issued to non-disk\n");
          BX_SELECTED_CONTROLLER.error_register = 0x81; // Drive 1 failed, no error on drive 0
          // BX_SELECTED_CONTROLLER.status.busy = 0; // not needed
          BX_SELECTED_CONTROLLER.status.drq = 0;
          BX_SELECTED_CONTROLLER.status.err = 0;
          break;

        case 0x91: // initialize drive parameters
          if (BX_SELECTED_CONTROLLER.status.busy) {
            bx_panic("disk: init drive parameters command: BSY bit set\n");
            }
	  if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: initialize drive parameters issued to non-disk\n");
          // sets logical geometry of specified drive
          BX_INFO(("initialize drive params\n"));
          BX_INFO(("  sector count = %u\n",
            (unsigned) BX_SELECTED_CONTROLLER.sector_count));
          BX_INFO(("  drive select = %u\n",
            (unsigned) BX_HD_THIS drive_select));
          BX_INFO(("  head number = %u\n",
            (unsigned) BX_SELECTED_CONTROLLER.head_no));
          if (BX_HD_THIS drive_select != 0 && !bx_options.diskd.present) {
            bx_panic("disk: init drive params: drive != 0\n");
            //BX_SELECTED_CONTROLLER.error_register = 0x12;
            BX_SELECTED_CONTROLLER.status.busy = 0;
            BX_SELECTED_CONTROLLER.status.drive_ready = 1;
            BX_SELECTED_CONTROLLER.status.drq = 0;
            BX_SELECTED_CONTROLLER.status.err = 0;
	    raise_interrupt();
            break;
	  }
          if (BX_SELECTED_CONTROLLER.sector_count != BX_SELECTED_HD.hard_drive->sectors)
            bx_panic("disk: init drive params: sector count doesnt match\n");
          if ( BX_SELECTED_CONTROLLER.head_no != (BX_SELECTED_HD.hard_drive->heads-1) )
            bx_panic("disk: init drive params: head number doesn't match\n");
          BX_SELECTED_CONTROLLER.status.busy = 0;
          BX_SELECTED_CONTROLLER.status.drive_ready = 1;
          BX_SELECTED_CONTROLLER.status.drq = 0;
          BX_SELECTED_CONTROLLER.status.err = 0;
	  raise_interrupt();
          break;

        case 0xec: // Get Drive Info
          if (bx_options.newHardDriveSupport) {
	    if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		  BX_INFO(("disk: Drive ID Command issued : 0xec \n"));

            if (BX_HD_THIS drive_select && !bx_options.diskd.present) {
              BX_INFO(("disk: 2nd drive not present, aborting\n"));
              command_aborted(value);
              break;
              }
	    if (BX_SELECTED_HD.device_type == IDE_CDROM) {
		  BX_SELECTED_CONTROLLER.head_no        = 0;
		  BX_SELECTED_CONTROLLER.sector_count   = 1;
		  BX_SELECTED_CONTROLLER.sector_no      = 1;
		  BX_SELECTED_CONTROLLER.cylinder_no    = 0xeb14;
		  command_aborted(0xec);
	    } else {
		  BX_SELECTED_CONTROLLER.current_command = value;
		  BX_SELECTED_CONTROLLER.error_register = 0;

		  // See ATA/ATAPI-4, 8.12
		  BX_SELECTED_CONTROLLER.status.busy  = 0;
		  BX_SELECTED_CONTROLLER.status.drive_ready = 1;
		  BX_SELECTED_CONTROLLER.status.write_fault = 0;
		  BX_SELECTED_CONTROLLER.status.drq   = 1;
		  BX_SELECTED_CONTROLLER.status.err   = 0;

		  BX_SELECTED_CONTROLLER.status.seek_complete = 1;
		  BX_SELECTED_CONTROLLER.status.corrected_data = 0;

		  BX_SELECTED_CONTROLLER.buffer_index = 0;
		  raise_interrupt();
		  identify_drive(BX_HD_THIS drive_select);
	    }
	  }
          else {
	    BX_INFO(("disk: old hard drive\n"));
            command_aborted(value);
	  }
          break;

        case 0xef: // set features
	  switch(BX_SELECTED_CONTROLLER.features) {
	    case 0x02: // Enable and
	    case 0x82: //  Disable write cache.
	    case 0xAA: // Enable and
	    case 0x55: //  Disable look-ahead cache.
	      BX_INFO(("disk: SET FEATURES subcommand not supported by disk.\n"));
	      command_aborted(value);
	    break;

	    default:
	      bx_panic("disk: SET FEATURES with unknown subcommand: 0x%02x\n", (unsigned) BX_SELECTED_CONTROLLER.features );
	  }
	  break;

        case 0x40: //
          if (bx_options.newHardDriveSupport) {
	    if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: read verify issued to non-disk\n");
            BX_INFO(("disk: Verify Command : 0x40 ! \n"));
            BX_SELECTED_CONTROLLER.status.busy = 0;
            BX_SELECTED_CONTROLLER.status.drive_ready = 1;
            BX_SELECTED_CONTROLLER.status.drq = 0;
            BX_SELECTED_CONTROLLER.status.err = 0;
	    raise_interrupt();
            }
          else {
	    BX_INFO(("disk: old hard drive\n"));
            command_aborted(value);
	  }
          break;
	case 0xc6: // (mch) set multiple mode
	      if (BX_SELECTED_CONTROLLER.sector_count != 128 &&
		  BX_SELECTED_CONTROLLER.sector_count != 64 &&
		  BX_SELECTED_CONTROLLER.sector_count != 32 &&
		  BX_SELECTED_CONTROLLER.sector_count != 16 &&
		  BX_SELECTED_CONTROLLER.sector_count != 8 &&
		  BX_SELECTED_CONTROLLER.sector_count != 4 &&
		  BX_SELECTED_CONTROLLER.sector_count != 2)
		    command_aborted(value);

	      if (BX_SELECTED_HD.device_type != IDE_DISK)
		bx_panic("disk: set multiple mode issued to non-disk\n");

	      BX_SELECTED_CONTROLLER.sectors_per_block = BX_SELECTED_CONTROLLER.sector_count;
	      BX_SELECTED_CONTROLLER.status.busy = 0;
	      BX_SELECTED_CONTROLLER.status.drive_ready = 1;
	      BX_SELECTED_CONTROLLER.status.write_fault = 0;
	      BX_SELECTED_CONTROLLER.status.drq = 0;
	      BX_SELECTED_CONTROLLER.status.err = 0;
	      break;

        // ATAPI commands
        case 0xa1: // identify ATAPI device
	      if (BX_SELECTED_HD.device_type == IDE_CDROM) {
		    BX_SELECTED_CONTROLLER.current_command = value;
		    BX_SELECTED_CONTROLLER.error_register = 0;

		    BX_SELECTED_CONTROLLER.status.busy = 0;
		    BX_SELECTED_CONTROLLER.status.drive_ready = 1;
		    BX_SELECTED_CONTROLLER.status.write_fault = 0;
		    BX_SELECTED_CONTROLLER.status.drq   = 1;
		    BX_SELECTED_CONTROLLER.status.err   = 0;

		    BX_SELECTED_CONTROLLER.status.seek_complete = 1;
		    BX_SELECTED_CONTROLLER.status.corrected_data = 0;

		    BX_SELECTED_CONTROLLER.buffer_index = 0;
		    raise_interrupt();
		    identify_ATAPI_drive(BX_HD_THIS drive_select);
	      } else {
		    command_aborted(0xa1);
	      }
	      break;

        case 0x08: // ATAPI soft reset command
	      if (BX_SELECTED_HD.device_type == IDE_CDROM) {
		    BX_SELECTED_CONTROLLER.status.busy = 1;
		    BX_SELECTED_CONTROLLER.error_register &= ~(1 << 7);

		    // device signature
		    BX_SELECTED_CONTROLLER.head_no        = 0;
		    BX_SELECTED_CONTROLLER.sector_count   = 1;
		    BX_SELECTED_CONTROLLER.sector_no      = 1;
		    BX_SELECTED_CONTROLLER.cylinder_no    = 0xeb14;

		    BX_SELECTED_CONTROLLER.status.write_fault = 0;
		    BX_SELECTED_CONTROLLER.status.drq = 0;
		    BX_SELECTED_CONTROLLER.status.corrected_data = 0;
		    BX_SELECTED_CONTROLLER.status.err = 0;

		    BX_SELECTED_CONTROLLER.status.busy = 0;

		    break;
	      }
        case 0xa0: // send a packet command
	      if (BX_SELECTED_HD.device_type == IDE_CDROM) {
		    // PACKET
		    if (BX_SELECTED_CONTROLLER.features & (1 << 0))
			  bx_panic("disk: PACKET-DMA not supported\n");
		    if (BX_SELECTED_CONTROLLER.features & (1 << 1))
			  bx_panic("disk: PACKET-overlapped not supported\n");

		    // We're already ready!
		    BX_SELECTED_CONTROLLER.sector_count = 1;
		    BX_SELECTED_CONTROLLER.status.busy = 0;
		    BX_SELECTED_CONTROLLER.status.write_fault = 0;
		    // serv bit??
		    BX_SELECTED_CONTROLLER.status.drq = 1;
		    BX_SELECTED_CONTROLLER.status.err = 0;

		    // NOTE: no interrupt here
		    BX_SELECTED_CONTROLLER.current_command = value;
		    BX_SELECTED_CONTROLLER.buffer_index = 0;

		    break;
	      }
        case 0xa2: // ATAPI service (optional)
	      if (BX_SELECTED_HD.device_type == IDE_CDROM) {
		    bx_panic("disk: ATAPI SERVICE not implemented\n");
	      }
        // non-standard commands
        case 0xf0: // Exabyte enable nest command
	  BX_INFO(("disk: Not implemented command\n"));
          command_aborted(value);
          break;

        // power management
	case 0xe5: // Check power mode
	  BX_SELECTED_CONTROLLER.status.busy = 0;
	  BX_SELECTED_CONTROLLER.status.drive_ready = 1;
	  BX_SELECTED_CONTROLLER.status.write_fault = 0;
	  BX_SELECTED_CONTROLLER.status.drq = 0;
	  BX_SELECTED_CONTROLLER.status.err = 0;
	  BX_SELECTED_CONTROLLER.sector_count = 0xff; // Active or Idle mode
	  raise_interrupt();
	  break;

        default:
          bx_panic("IO write(1f7h): command 0x%02x\n", (unsigned) value);
        }
      break;

    case 0x3f6: // hard disk adapter control
	  // (mch) Even if device 1 was selected, a write to this register
	  // goes to device 0 (if device 1 is absent)
		
	  prev_control_reset = BX_SELECTED_CONTROLLER.control.reset;
	  BX_HD_THIS s[0].controller.control.reset         = value & 0x04;
	  BX_HD_THIS s[1].controller.control.reset         = value & 0x04;
	  BX_SELECTED_CONTROLLER.control.disable_irq    = value & 0x02;
      //fprintf(stderr, "# hard drive: adpater control reg: reset controller = %d\n",
      //  (unsigned) (BX_SELECTED_CONTROLLER.control.reset) ? 1 : 0);
      //fprintf(stderr, "# hard drive: adpater control reg: disable_irq(14) = %d\n",
      //  (unsigned) (BX_SELECTED_CONTROLLER.control.disable_irq) ? 1 : 0);
	  if (!prev_control_reset && BX_SELECTED_CONTROLLER.control.reset) {
		// transition from 0 to 1 causes all drives to reset
		BX_INFO(("hard drive: RESET\n"));

		// (mch) Set BSY, drive not ready
		for (int id = 0; id < 2; id++) {
		      BX_CONTROLLER(id).status.busy           = 1;
		      BX_CONTROLLER(id).status.drive_ready    = 0;
		      BX_CONTROLLER(id).reset_in_progress     = 1;

		      BX_CONTROLLER(id).status.write_fault    = 0;
		      BX_CONTROLLER(id).status.seek_complete  = 1;
		      BX_CONTROLLER(id).status.drq            = 0;
		      BX_CONTROLLER(id).status.corrected_data = 0;
		      BX_CONTROLLER(id).status.err            = 0;

		      BX_CONTROLLER(id).error_register = 0x01; // diagnostic code: no error

		      BX_CONTROLLER(id).current_command = 0x00;
		      BX_CONTROLLER(id).buffer_index = 0;

		      BX_CONTROLLER(id).sectors_per_block = 0x80;
		      BX_CONTROLLER(id).lba_mode          = 0;

		      BX_CONTROLLER(id).control.disable_irq = 0;
		}
	  } else if (BX_SELECTED_CONTROLLER.reset_in_progress &&
		     !BX_SELECTED_CONTROLLER.control.reset) {
		// Clear BSY and DRDY
		BX_INFO(("disk: Reset complete {%s}\n", DEVICE_TYPE_STRING));
		for (int id = 0; id < 2; id++) {
		      BX_CONTROLLER(id).status.busy           = 0;
		      BX_CONTROLLER(id).status.drive_ready    = 1;
		      BX_CONTROLLER(id).reset_in_progress     = 0;

		      // Device signature
		      if (BX_HD_THIS s[id].device_type == IDE_DISK) {
			    BX_CONTROLLER(id).head_no        = 0;
			    BX_CONTROLLER(id).sector_count   = 1;
			    BX_CONTROLLER(id).sector_no      = 1;
			    BX_CONTROLLER(id).cylinder_no    = 0;
		      } else {
			    BX_CONTROLLER(id).head_no        = 0;
			    BX_CONTROLLER(id).sector_count   = 1;
			    BX_CONTROLLER(id).sector_no      = 1;
			    BX_CONTROLLER(id).cylinder_no    = 0xeb14;
		      }
		}
	  }
	  break;
#if 0
      // you'll need these to support second IDE controller, not needed yet.
    case 0x170:
    case 0x171:
    case 0x172:
    case 0x173:
    case 0x174:
    case 0x175:
    case 0x176:
    case 0x177:
	  BX_INFO(("[disk] ignoring write to 0x%04x\n", address));
       break;
#endif

    default:
      bx_panic("hard drive: io write to address %x = %02x\n",
        (unsigned) address, (unsigned) value);
    }
}

  void
bx_hard_drive_c::close_harddrive(void)
{
  BX_HD_THIS s[0].hard_drive->close();
  BX_HD_THIS s[1].hard_drive->close();
}


#define assert(i) do { if (!((i))) bx_panic("assertion on line %d", __LINE__); } while (0)

  Bit32u
bx_hard_drive_c::calculate_logical_address()
{
      Bit32u logical_sector;

      if (BX_SELECTED_CONTROLLER.lba_mode)
	    logical_sector = ((Bit32u)BX_SELECTED_CONTROLLER.head_no) << 24 |
		  ((Bit32u)BX_SELECTED_CONTROLLER.cylinder_no) << 8 |
		  (Bit32u)BX_SELECTED_CONTROLLER.sector_no;
      else
	    logical_sector = (BX_SELECTED_CONTROLLER.cylinder_no * BX_SELECTED_HD.hard_drive->heads *
			      BX_SELECTED_HD.hard_drive->sectors) +
		  (BX_SELECTED_CONTROLLER.head_no * BX_SELECTED_HD.hard_drive->sectors) +
		  (BX_SELECTED_CONTROLLER.sector_no - 1);

      if (logical_sector >=
	  (BX_SELECTED_HD.hard_drive->cylinders * BX_SELECTED_HD.hard_drive->heads * BX_SELECTED_HD.hard_drive->sectors)) {
            bx_panic("disk: read sectors: out of bounds\n");
      }
      return logical_sector;
}

  void
bx_hard_drive_c::increment_address()
{
      BX_SELECTED_CONTROLLER.sector_count--;

      if (BX_SELECTED_CONTROLLER.lba_mode) {
	    Bit32u current_address = calculate_logical_address();
	    current_address++;
	    BX_SELECTED_CONTROLLER.head_no = (current_address >> 24) & 0xf;
	    BX_SELECTED_CONTROLLER.cylinder_no = (current_address >> 8) & 0xffff;
	    BX_SELECTED_CONTROLLER.sector_no = (current_address) & 0xff;
      } else {
            BX_SELECTED_CONTROLLER.sector_no++;
            if (BX_SELECTED_CONTROLLER.sector_no > BX_SELECTED_HD.hard_drive->sectors) {
		  BX_SELECTED_CONTROLLER.sector_no = 1;
		  BX_SELECTED_CONTROLLER.head_no++;
		  if (BX_SELECTED_CONTROLLER.head_no >= BX_SELECTED_HD.hard_drive->heads) {
			BX_SELECTED_CONTROLLER.head_no = 0;
			BX_SELECTED_CONTROLLER.cylinder_no++;
			if (BX_SELECTED_CONTROLLER.cylinder_no >= BX_SELECTED_HD.hard_drive->cylinders)
			      BX_SELECTED_CONTROLLER.cylinder_no = BX_SELECTED_HD.hard_drive->cylinders - 1;
		  }
	    }
      }
}

  void
bx_hard_drive_c::identify_ATAPI_drive(unsigned drive)
{
  unsigned i;

  if (drive != (unsigned)BX_HD_THIS drive_select) {
	bx_panic("disk: identify_drive panic (drive != drive_select)\n");
  }

  BX_SELECTED_HD.id_drive[0] = (2 << 14) | (5 << 8) | (1 << 7) | (2 << 5) | (0 << 0); // Removable CDROM, 50us response, 12 byte packets

  for (i = 1; i <= 9; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  char* serial_number = " VT00001\0\0\0\0\0\0\0\0\0\0\0\0";
  for (i = 0; i < 10; i++) {
	BX_SELECTED_HD.id_drive[10+i] = (serial_number[i*2] << 8) |
	      serial_number[i*2 + 1];
  }

  for (i = 20; i <= 22; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  char* firmware = "ALPHA1  ";
  for (i = 0; i < strlen(firmware)/2; i++) {
	BX_SELECTED_HD.id_drive[23+i] = (firmware[i*2] << 8) |
	      firmware[i*2 + 1];
  }
  assert((23+i) == 27);
  
  for (i = 0; i < strlen((char *) model_no)/2; i++) {
	BX_SELECTED_HD.id_drive[27+i] = (model_no[i*2] << 8) |
	      model_no[i*2 + 1];
  }
  assert((27+i) == 47);

  BX_SELECTED_HD.id_drive[47] = 0;
  BX_SELECTED_HD.id_drive[48] = 0;

  BX_SELECTED_HD.id_drive[49] = (1 << 9); // LBA supported

  BX_SELECTED_HD.id_drive[50] = 0;
  BX_SELECTED_HD.id_drive[51] = 0;
  BX_SELECTED_HD.id_drive[52] = 0;

  BX_SELECTED_HD.id_drive[53] = 3; // words 64-70, 54-58 valid

  for (i = 54; i <= 62; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  // copied from CFA540A
  BX_SELECTED_HD.id_drive[63] = 0x0103; // variable (DMA stuff)
  BX_SELECTED_HD.id_drive[64] = 0x0001; // PIO
  BX_SELECTED_HD.id_drive[65] = 0x00b4;
  BX_SELECTED_HD.id_drive[66] = 0x00b4;
  BX_SELECTED_HD.id_drive[67] = 0x012c;
  BX_SELECTED_HD.id_drive[68] = 0x00b4;

  BX_SELECTED_HD.id_drive[69] = 0;
  BX_SELECTED_HD.id_drive[70] = 0;
  BX_SELECTED_HD.id_drive[71] = 30; // faked
  BX_SELECTED_HD.id_drive[72] = 30; // faked
  BX_SELECTED_HD.id_drive[73] = 0;
  BX_SELECTED_HD.id_drive[74] = 0;

  BX_SELECTED_HD.id_drive[75] = 0;

  for (i = 76; i <= 79; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  BX_SELECTED_HD.id_drive[80] = 0x1e; // supports up to ATA/ATAPI-4
  BX_SELECTED_HD.id_drive[81] = 0;
  BX_SELECTED_HD.id_drive[82] = 0;
  BX_SELECTED_HD.id_drive[83] = 0;
  BX_SELECTED_HD.id_drive[84] = 0;
  BX_SELECTED_HD.id_drive[85] = 0;
  BX_SELECTED_HD.id_drive[86] = 0;
  BX_SELECTED_HD.id_drive[87] = 0;
  BX_SELECTED_HD.id_drive[88] = 0;

  for (i = 89; i <= 126; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  BX_SELECTED_HD.id_drive[127] = 0;
  BX_SELECTED_HD.id_drive[128] = 0;

  for (i = 129; i <= 159; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  for (i = 160; i <= 255; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  // now convert the id_drive array (native 256 word format) to
  // the controller buffer (512 bytes)
  Bit16u temp16;
  for (i = 0; i <= 255; i++) {
	temp16 = BX_SELECTED_HD.id_drive[i];
	BX_SELECTED_CONTROLLER.buffer[i*2] = temp16 & 0x00ff;
	BX_SELECTED_CONTROLLER.buffer[i*2+1] = temp16 >> 8;
  }
}

  void
bx_hard_drive_c::identify_drive(unsigned drive)
{
  unsigned i;
  Bit32u temp32;
  Bit16u temp16;

  if (drive != BX_HD_THIS drive_select) {
	bx_panic("disk: identify_drive panic (drive != drive_select)\n");
  }

#if defined(CONNER_CFA540A)
  BX_SELECTED_HD.id_drive[0] = 0x0c5a;
  BX_SELECTED_HD.id_drive[1] = 0x0418;
  BX_SELECTED_HD.id_drive[2] = 0;
  BX_SELECTED_HD.id_drive[3] = BX_SELECTED_HD.hard_drive->heads;
  BX_SELECTED_HD.id_drive[4] = 0x9fb7;
  BX_SELECTED_HD.id_drive[5] = 0x0289;
  BX_SELECTED_HD.id_drive[6] = BX_SELECTED_HD.hard_drive->sectors;
  BX_SELECTED_HD.id_drive[7] = 0x0030;
  BX_SELECTED_HD.id_drive[8] = 0x000a;
  BX_SELECTED_HD.id_drive[9] = 0x0000;

  char* serial_number = " CA00GSQ\0\0\0\0\0\0\0\0\0\0\0\0";
  for (i = 0; i < 10; i++) {
	BX_SELECTED_HD.id_drive[10+i] = (serial_number[i*2] << 8) |
	      serial_number[i*2 + 1];
  }

  BX_SELECTED_HD.id_drive[20] = 3;
  BX_SELECTED_HD.id_drive[21] = 512; // 512 Sectors = 256kB cache
  BX_SELECTED_HD.id_drive[22] = 4;

  char* firmware = "8FT054  ";
  for (i = 0; i < strlen(firmware)/2; i++) {
	BX_SELECTED_HD.id_drive[23+i] = (firmware[i*2] << 8) |
	      firmware[i*2 + 1];
  }
  assert((23+i) == 27);

  char* model = "Conner Peripherals 540MB - CFA540A      ";
  for (i = 0; i < strlen(model)/2; i++) {
	BX_SELECTED_HD.id_drive[27+i] = (model[i*2] << 8) |
	      model[i*2 + 1];
  }
  assert((27+i) == 47);

  BX_SELECTED_HD.id_drive[47] = 0x8080; // multiple mode identification
  BX_SELECTED_HD.id_drive[48] = 0;
  BX_SELECTED_HD.id_drive[49] = 0x0f01;

  BX_SELECTED_HD.id_drive[50] = 0;

  BX_SELECTED_HD.id_drive[51] = 0;
  BX_SELECTED_HD.id_drive[52] = 0x0002;
  BX_SELECTED_HD.id_drive[53] = 0x0003;
  BX_SELECTED_HD.id_drive[54] = 0x0418;

  BX_SELECTED_HD.id_drive[55] = BX_SELECTED_HD.hard_drive->heads;
  BX_SELECTED_HD.id_drive[56] = BX_SELECTED_HD.hard_drive->sectors;

  BX_SELECTED_HD.id_drive[57] = 0x1e80;
  BX_SELECTED_HD.id_drive[58] = 0x0010;
  BX_SELECTED_HD.id_drive[59] = 0x0100 | BX_SELECTED_CONTROLLER.sectors_per_block;
  BX_SELECTED_HD.id_drive[60] = 0x20e0;
  BX_SELECTED_HD.id_drive[61] = 0x0010;

  BX_SELECTED_HD.id_drive[62] = 0;

  BX_SELECTED_HD.id_drive[63] = 0x0103; // variable (DMA stuff)
  BX_SELECTED_HD.id_drive[64] = 0x0001; // PIO
  BX_SELECTED_HD.id_drive[65] = 0x00b4;
  BX_SELECTED_HD.id_drive[66] = 0x00b4;
  BX_SELECTED_HD.id_drive[67] = 0x012c;
  BX_SELECTED_HD.id_drive[68] = 0x00b4;

  for (i = 69; i <= 79; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  BX_SELECTED_HD.id_drive[80] = 0;

  BX_SELECTED_HD.id_drive[81] = 0;

  BX_SELECTED_HD.id_drive[82] = 0;
  BX_SELECTED_HD.id_drive[83] = 0;
  BX_SELECTED_HD.id_drive[84] = 0;
  BX_SELECTED_HD.id_drive[85] = 0;
  BX_SELECTED_HD.id_drive[86] = 0;
  BX_SELECTED_HD.id_drive[87] = 0;

  for (i = 88; i <= 127; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  BX_SELECTED_HD.id_drive[128] = 0x0418;
  BX_SELECTED_HD.id_drive[129] = 0x103f;
  BX_SELECTED_HD.id_drive[130] = 0x0418;
  BX_SELECTED_HD.id_drive[131] = 0x103f;
  BX_SELECTED_HD.id_drive[132] = 0x0004;
  BX_SELECTED_HD.id_drive[133] = 0xffff;
  BX_SELECTED_HD.id_drive[134] = 0;
  BX_SELECTED_HD.id_drive[135] = 0x5050;

  for (i = 136; i <= 144; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

  BX_SELECTED_HD.id_drive[145] = 0x302e;
  BX_SELECTED_HD.id_drive[146] = 0x3245;
  BX_SELECTED_HD.id_drive[147] = 0x2020;
  BX_SELECTED_HD.id_drive[148] = 0x2020;

  for (i = 149; i <= 255; i++)
	BX_SELECTED_HD.id_drive[i] = 0;

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
  BX_SELECTED_HD.id_drive[0] = 0x0040;

  // Word 1: number of user-addressable cylinders in
  //   default translation mode.  If the value in words 60-61
  //   exceed 16,515,072, this word shall contain 16,383.
  BX_SELECTED_HD.id_drive[1] = BX_SELECTED_HD.hard_drive->cylinders;

  // Word 2: reserved
  BX_SELECTED_HD.id_drive[2] = 0;

  // Word 3: number of user-addressable heads in default
  //   translation mode
  BX_SELECTED_HD.id_drive[3] = BX_SELECTED_HD.hard_drive->heads;

  // Word 4: # unformatted bytes per translated track in default xlate mode
  // Word 5: # unformatted bytes per sector in default xlated mode
  // Word 6: # user-addressable sectors per track in default xlate mode
  // Note: words 4,5 are now "Vendor specific (obsolete)"
  BX_SELECTED_HD.id_drive[4] = (512 * BX_SELECTED_HD.hard_drive->sectors);
  BX_SELECTED_HD.id_drive[5] = 512;
  BX_SELECTED_HD.id_drive[6] = BX_SELECTED_HD.hard_drive->sectors;

  // Word 7-9: Vendor specific
  for (i=7; i<=9; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

  // Word 10-19: Serial number (20 ASCII characters, 0000h=not specified)
  // This field is right justified and padded with spaces (20h).
  for (i=10; i<=19; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

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
  BX_SELECTED_HD.id_drive[20] = 3;

  // Word 21: buffer size in 512 byte increments, 0000h = not specified
  BX_SELECTED_HD.id_drive[21] = 512; // 512 Sectors = 256kB cache

  // Word 22: # of ECC bytes available on read/write long cmds
  //          0000h = not specified
  BX_SELECTED_HD.id_drive[22] = 4;

  // Word 23..26: Firmware revision (8 ascii chars, 0000h=not specified)
  // This field is left justified and padded with spaces (20h)
  for (i=23; i<=26; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

  // Word 27..46: Model number (40 ascii chars, 0000h=not specified)
  // This field is left justified and padded with spaces (20h)
//  for (i=27; i<=46; i++)
//    BX_SELECTED_HD.id_drive[i] = 0;
  for (i=0; i<20; i++) {
    BX_SELECTED_HD.id_drive[27+i] = (model_no[i*2] << 8) |
                                  model_no[i*2 + 1];
    }

  // Word 47: 15-8 Vendor unique
  //           7-0 00h= read/write multiple commands not implemented
  //               xxh= maximum # of sectors that can be transferred
  //                    per interrupt on read and write multiple commands
  BX_SELECTED_HD.id_drive[47] = max_multiple_sectors;

  // Word 48: 0000h = cannot perform dword IO
  //          0001h = can    perform dword IO
  BX_SELECTED_HD.id_drive[48] = 0;

  // Word 49: Capabilities
  //   15-10: 0 = reserved
  //       9: 1 = LBA supported
  //       8: 1 = DMA supported
  //     7-0: Vendor unique
  BX_SELECTED_HD.id_drive[49] = 0;

  // Word 50: Reserved
  BX_SELECTED_HD.id_drive[50] = 0;

  // Word 51: 15-8 PIO data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_HD.id_drive[51] = 0x200;

  // Word 52: 15-8 DMA data transfer cycle timing mode
  //           7-0 Vendor unique
  BX_SELECTED_HD.id_drive[52] = 0x200;

  // Word 53: 15-1 Reserved
  //             0 1=the fields reported in words 54-58 are valid
  //               0=the fields reported in words 54-58 may be valid
  BX_SELECTED_HD.id_drive[53] = 0;

  // Word 54: # of user-addressable cylinders in curr xlate mode
  // Word 55: # of user-addressable heads in curr xlate mode
  // Word 56: # of user-addressable sectors/track in curr xlate mode
  BX_SELECTED_HD.id_drive[54] = BX_SELECTED_HD.hard_drive->cylinders;
  BX_SELECTED_HD.id_drive[55] = BX_SELECTED_HD.hard_drive->heads;
  BX_SELECTED_HD.id_drive[56] = BX_SELECTED_HD.hard_drive->sectors;

  // Word 57-58: Current capacity in sectors
  // Excludes all sectors used for device specific purposes.
  temp32 = 
    BX_SELECTED_HD.hard_drive->cylinders *
    BX_SELECTED_HD.hard_drive->heads *
    BX_SELECTED_HD.hard_drive->sectors;
  BX_SELECTED_HD.id_drive[57] = (temp32 & 0xffff); // LSW
  BX_SELECTED_HD.id_drive[58] = (temp32 >> 16);    // MSW

  // Word 59: 15-9 Reserved
  //             8 1=multiple sector setting is valid
  //           7-0 current setting for number of sectors that can be
  //               transferred per interrupt on R/W multiple commands
  BX_SELECTED_HD.id_drive[59] = 0x0000 | curr_multiple_sectors;

  // Word 60-61:
  // If drive supports LBA Mode, these words reflect total # of user
  // addressable sectors.  This value does not depend on the current
  // drive geometry.  If the drive does not support LBA mode, these
  // words shall be set to 0.
  Bit32u num_sects = BX_SELECTED_HD.hard_drive->cylinders * BX_SELECTED_HD.hard_drive->heads * BX_SELECTED_HD.hard_drive->sectors;
  BX_SELECTED_HD.id_drive[60] = num_sects & 0xffff; // LSW
  BX_SELECTED_HD.id_drive[61] = num_sects >> 16; // MSW

  // Word 62: 15-8 single word DMA transfer mode active
  //           7-0 single word DMA transfer modes supported
  // The low order byte identifies by bit, all the Modes which are
  // supported e.g., if Mode 0 is supported bit 0 is set.
  // The high order byte contains a single bit set to indiciate
  // which mode is active.
  BX_SELECTED_HD.id_drive[62] = 0x0;

  // Word 63: 15-8 multiword DMA transfer mode active
  //           7-0 multiword DMA transfer modes supported
  // The low order byte identifies by bit, all the Modes which are
  // supported e.g., if Mode 0 is supported bit 0 is set.
  // The high order byte contains a single bit set to indiciate
  // which mode is active.
  BX_SELECTED_HD.id_drive[63] = 0x0;

  // Word 64-79 Reserved
  for (i=64; i<=79; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

  // Word 80: 15-5 reserved
  //             4 supports ATA/ATAPI-4
  //             3 supports ATA-3
  //             2 supports ATA-2
  //             1 supports ATA-1
  //             0 reserved
  BX_SELECTED_HD.id_drive[80] = (1 << 2) | (1 << 1);

  // Word 81: Minor version number
  BX_SELECTED_HD.id_drive[81] = 0;

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
  BX_SELECTED_HD.id_drive[82] = 1 << 14;
  BX_SELECTED_HD.id_drive[83] = 1 << 14;
  BX_SELECTED_HD.id_drive[84] = 1 << 14;
  BX_SELECTED_HD.id_drive[85] = 1 << 14;
  BX_SELECTED_HD.id_drive[86] = 0;
  BX_SELECTED_HD.id_drive[87] = 1 << 14;

  for (i=88; i<=127; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

  // Word 128-159 Vendor unique
  for (i=128; i<=159; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

  // Word 160-255 Reserved
  for (i=160; i<=255; i++)
    BX_SELECTED_HD.id_drive[i] = 0;

#endif

  if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
    BX_INFO(("disk: Drive ID Info. initialized : %04d {%s}\n", 512, DEVICE_TYPE_STRING));

  // now convert the id_drive array (native 256 word format) to
  // the controller buffer (512 bytes)
  for (i=0; i<=255; i++) {
    temp16 = BX_SELECTED_HD.id_drive[i];
    BX_SELECTED_CONTROLLER.buffer[i*2] = temp16 & 0x00ff;
    BX_SELECTED_CONTROLLER.buffer[i*2+1] = temp16 >> 8;
    }
}

  void
bx_hard_drive_c::init_send_atapi_command(Bit8u command, int req_length, int alloc_length, bool lazy)
{
      if (BX_SELECTED_CONTROLLER.byte_count == 0)
	    bx_panic("disk: ATAPI command with zero byte count\n");

      if (BX_SELECTED_CONTROLLER.byte_count & 1)
	    bx_panic("disk: Odd byte count to ATAPI command\n");

      if (alloc_length <= 0)
	    bx_panic("disk: Allocation length <= 0\n");

      BX_SELECTED_CONTROLLER.interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER.interrupt_reason.c_d = 0;
      BX_SELECTED_CONTROLLER.status.busy = 0;
      BX_SELECTED_CONTROLLER.status.drq = 1;
      BX_SELECTED_CONTROLLER.status.err = 0;

      // no bytes transfered yet
      if (lazy)
	    BX_SELECTED_CONTROLLER.buffer_index = 2048;
      else
	    BX_SELECTED_CONTROLLER.buffer_index = 0;

      if (BX_SELECTED_CONTROLLER.byte_count > req_length)
	    BX_SELECTED_CONTROLLER.byte_count = req_length;

      if (BX_SELECTED_CONTROLLER.byte_count > alloc_length)
	    BX_SELECTED_CONTROLLER.byte_count = alloc_length;

      BX_SELECTED_HD.atapi.command = command;
      BX_SELECTED_HD.atapi.drq_bytes = BX_SELECTED_CONTROLLER.byte_count;
      BX_SELECTED_HD.atapi.total_bytes_remaining = (req_length < alloc_length) ? req_length : alloc_length;

      if (lazy) {
	    // bias drq_bytes and total_bytes_remaining
	    BX_SELECTED_HD.atapi.drq_bytes += 2048;
	    BX_SELECTED_HD.atapi.total_bytes_remaining += 2048;
      }
}

void
bx_hard_drive_c::atapi_cmd_error(sense_t sense_key, asc_t asc)
{
      BX_SELECTED_CONTROLLER.error_register = sense_key << 4;
      BX_SELECTED_CONTROLLER.interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER.interrupt_reason.c_d = 1;
      BX_SELECTED_CONTROLLER.interrupt_reason.rel = 0;
      BX_SELECTED_CONTROLLER.status.busy = 0;
      BX_SELECTED_CONTROLLER.status.drive_ready = 1;
      BX_SELECTED_CONTROLLER.status.write_fault = 0;
      BX_SELECTED_CONTROLLER.status.drq = 0;
      BX_SELECTED_CONTROLLER.status.err = 1;

      BX_SELECTED_HD.sense.sense_key = sense_key;
      BX_SELECTED_HD.sense.asc = asc;
      BX_SELECTED_HD.sense.ascq = 0;
}

void
bx_hard_drive_c::atapi_cmd_nop()
{
      BX_SELECTED_CONTROLLER.interrupt_reason.i_o = 1;
      BX_SELECTED_CONTROLLER.interrupt_reason.c_d = 1;
      BX_SELECTED_CONTROLLER.interrupt_reason.rel = 0;
      BX_SELECTED_CONTROLLER.status.busy = 0;
      BX_SELECTED_CONTROLLER.status.drive_ready = 1;
      BX_SELECTED_CONTROLLER.status.drq = 0;
      BX_SELECTED_CONTROLLER.status.err = 0;
}

void
bx_hard_drive_c::init_mode_sense_single(const void* src, int size)
{
      // Header
      BX_SELECTED_CONTROLLER.buffer[0] = (size+6) >> 8;
      BX_SELECTED_CONTROLLER.buffer[1] = (size+6) & 0xff;
      BX_SELECTED_CONTROLLER.buffer[2] = 0x70; // no media present
      BX_SELECTED_CONTROLLER.buffer[3] = 0; // reserved
      BX_SELECTED_CONTROLLER.buffer[4] = 0; // reserved
      BX_SELECTED_CONTROLLER.buffer[5] = 0; // reserved
      BX_SELECTED_CONTROLLER.buffer[6] = 0; // reserved
      BX_SELECTED_CONTROLLER.buffer[7] = 0; // reserved

      // Data
      memcpy(BX_SELECTED_CONTROLLER.buffer + 8, src, size);
}

  void
bx_hard_drive_c::ready_to_send_atapi()
{
      raise_interrupt();
}

void
bx_hard_drive_c::raise_interrupt()
{
      if (!BX_SELECTED_CONTROLLER.control.disable_irq) {
	    Bit32u irq = 14;  // always 1st IDE controller
	    // for second controller, you would want irq 15
	    if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		  BX_INFO(("disk: Raising interrupt %d {%s}\n", irq, DEVICE_TYPE_STRING));
	    BX_HD_THIS devices->pic->trigger_irq(irq);
      } else {
	    if (bx_dbg.disk || (CDROM_SELECTED && bx_dbg.cdrom))
		  BX_INFO(("disk: Interrupt masked {%s}\n", DEVICE_TYPE_STRING));
      }
}

  void
bx_hard_drive_c::command_aborted(unsigned value)
{
  BX_INFO(("disk: aborting on command 0x%02x {%s}\n", value, DEVICE_TYPE_STRING));
  BX_SELECTED_CONTROLLER.current_command = 0;
  BX_SELECTED_CONTROLLER.status.busy = 0;
  BX_SELECTED_CONTROLLER.status.drive_ready = 1;
  BX_SELECTED_CONTROLLER.status.err = 1;
  BX_SELECTED_CONTROLLER.error_register = 0x04; // command ABORTED
  BX_SELECTED_CONTROLLER.status.drq = 0;
  BX_SELECTED_CONTROLLER.status.seek_complete = 0;
  BX_SELECTED_CONTROLLER.status.corrected_data = 0;
  BX_SELECTED_CONTROLLER.buffer_index = 0;
  raise_interrupt();
}


/*** default_image_t function definitions ***/

int default_image_t::open (const char* pathname)
{
      fd = ::open(pathname, O_RDWR
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
	    perror("fstat'ing hard drive image file");
	    bx_panic("fstat() returns error!\n");
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
      return ::read(fd, buf, count);
}

ssize_t default_image_t::write (const void* buf, size_t count)
{
      return ::write(fd, buf, count);
}

#if BX_SPLIT_HD_SUPPORT
/*** concat_image_t function definitions ***/

void concat_image_t::increment_string (char *str)
{
  // find the last character of the string, and increment it.
  char *p = str;
  while (*p != 0) p++;
  assert (p>str);  // choke on zero length strings
  p--;  // point to last character of the string
  ++(*p);  // increment to next ascii code.
  if (bx_dbg.disk)
    BX_INFO(("concat_image.increment string returning '%s'\n", str));
}

int concat_image_t::open (const char* pathname0)
{
  char *pathname = strdup (pathname0);
  BX_INFO(("concat_image_t.open\n"));
  ssize_t start_offset = 0;
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
    if (bx_dbg.disk)
      BX_INFO(("concat_image: open image %s, fd[%d] = %d\n", pathname, i, fd_table[i]));
    /* look at size of image file to calculate disk geometry */
    struct stat stat_buf;
    int ret = fstat(fd_table[i], &stat_buf);
    if (ret) {
	  perror("fstat'ing hard drive image file");
	  bx_panic("fstat() returns error!\n");
    }
    if ((stat_buf.st_size % 512) != 0) {
      bx_panic ("[HDD] size of disk image must be multiple of 512 bytes");
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
  BX_INFO(("concat_image_t.close\n"));
  if (fd > -1) {
    ::close(fd);
  }
}

off_t concat_image_t::lseek (off_t offset, int whence)
{
  if ((offset % 512) != 0) 
    bx_panic ("lseek HD with offset not multiple of 512");
  if (bx_dbg.disk)
    BX_INFO(("concat_image_t.lseek(%d)\n", whence));
  // is this offset in this disk image?
  if (offset < thismin) {
    // no, look at previous images
    for (int i=index-1; i>=0; i--) {
      if (offset >= start_offset_table[i]) {
	index = i;
	fd = fd_table[i];
	thismin = start_offset_table[i];
	thismax = thismin + length_table[i] - 1;
	if (bx_dbg.disk)
	  BX_INFO(("concat_image_t.lseek to earlier image, index=%d\n", index));
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
	if (bx_dbg.disk)
	  BX_INFO(("concat_image_t.lseek to earlier image, index=%d\n", index));
	break;
      }
    }
  }
  // now offset should be within the current image.
  offset -= start_offset_table[index];
  if (offset < 0 || offset >= length_table[index])
    bx_panic ("concat_image_t.lseek to byte %ld failed\n", (long)offset);

  seek_was_last_op = 1;
  return ::lseek(fd, offset, whence);
}

ssize_t concat_image_t::read (void* buf, size_t count)
{
  if (bx_dbg.disk)
    BX_INFO(("concat_image_t.read %ld bytes\n", (long)count));
  // notice if anyone does sequential read or write without seek in between.
  // This can be supported pretty easily, but needs additional checks for
  // end of a partial image.
  if (!seek_was_last_op) 
    bx_panic ("disk: no seek before read");
  return ::read(fd, buf, count);
}

ssize_t concat_image_t::write (const void* buf, size_t count)
{
  if (bx_dbg.disk)
    BX_INFO(("concat_image_t.write %ld bytes\n", (long)count));
  // notice if anyone does sequential read or write without seek in between.
  // This can be supported pretty easily, but needs additional checks for
  // end of a partial image.
  if (!seek_was_last_op) 
    bx_panic ("disk: no seek before write");
  return ::write(fd, buf, count);
}
#endif   /* BX_SPLIT_HD_SUPPORT */

error_recovery_t::error_recovery_t ()
{
      if (sizeof(error_recovery_t) != 8) {
	    bx_panic("error_recovery_t has size != 8\n");
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

uint16 read_16bit(const uint8* buf)
{
      return (buf[0] << 8) | buf[1];
}

uint32 read_32bit(const uint8* buf)
{
      return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}
