/////////////////////////////////////////////////////////////////////////
// $Id: cmos.cc,v 1.18 2002-08-27 19:54:46 bdenney Exp $
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





#include "bochs.h"
#define LOG_THIS bx_cmos.

bx_cmos_c bx_cmos;

#if BX_USE_CMOS_SMF
#define this (&bx_cmos)
#endif


// check that BX_NUM_CMOS_REGS is 64 or 128
#if (BX_NUM_CMOS_REGS == 64)
#elif (BX_NUM_CMOS_REGS == 128)
#else
#error "Invalid BX_NUM_CMOS_REGS value in config.h"
#endif


bx_cmos_c::bx_cmos_c(void)
{
	put("CMOS");
	settype(CMOSLOG);
}

bx_cmos_c::~bx_cmos_c(void)
{
	BX_DEBUG(("Exit."));
}


	void
bx_cmos_c::init(bx_devices_c *d)
{
	unsigned i;
	BX_DEBUG(("Init $Id: cmos.cc,v 1.18 2002-08-27 19:54:46 bdenney Exp $"));

	// CMOS RAM & RTC

	BX_CMOS_THIS devices = d;

	BX_CMOS_THIS devices->register_io_read_handler(this,
		read_handler, 0x0070,
		"CMOS RAM");
	BX_CMOS_THIS devices->register_io_read_handler(this,
		read_handler,
		0x0071,
		"CMOS RAM");
	BX_CMOS_THIS devices->register_io_write_handler(this,
		write_handler,
		0x0070, "CMOS RAM");
	BX_CMOS_THIS devices->register_io_write_handler(this,
		write_handler,
		0x0071, "CMOS RAM");
	BX_CMOS_THIS devices->register_irq(8, "CMOS RTC");

	BX_CMOS_THIS s.periodic_timer_index =
    bx_pc_system.register_timer(this, periodic_timer_handler,
      1000000, 1,0); // continuous, not-active
	BX_CMOS_THIS s.one_second_timer_index =
    bx_pc_system.register_timer(this, one_second_timer_handler,
      1000000, 1,0); // continuous, not-active

	for (i=0; i<BX_NUM_CMOS_REGS; i++) {
    BX_CMOS_THIS s.reg[i] = 0;
    }

#if BX_USE_SPECIFIED_TIME0 == 0
	// ??? this will not be correct for using an image file.
	// perhaps take values in CMOS and work backwards to find
	// s.timeval from values read in.
	BX_CMOS_THIS s.timeval = time(NULL);
#else
	BX_CMOS_THIS s.timeval = BX_USE_SPECIFIED_TIME0;
#endif

	if (bx_options.cmos.Otime0->get () == 1)
		   BX_CMOS_THIS s.timeval = time(NULL);
	else if (bx_options.cmos.Otime0->get () != 0)
		   BX_CMOS_THIS s.timeval = bx_options.cmos.Otime0->get ();

	char *tmptime;
	while( (tmptime =  strdup(ctime(&(BX_CMOS_THIS s.timeval)))) == NULL) {
		BX_PANIC(("Out of memory."));
	}
	tmptime[strlen(tmptime)-1]='\0';

	BX_INFO(("Setting initial clock to: %s", tmptime));

	update_clock();

	// load CMOS from image file if requested.
	if (bx_options.cmos.OcmosImage->get ()) {
    // CMOS image file requested
    int fd, ret;
    struct stat stat_buf;

    fd = open(bx_options.cmos.Opath->getptr (), O_RDONLY
#ifdef O_BINARY
			 | O_BINARY
#endif
			  );
    if (fd < 0) {
      BX_PANIC(("trying to open cmos image file '%s'",
		 bx_options.cmos.Opath->getptr ()));
      }
    ret = fstat(fd, &stat_buf);
    if (ret) {
      BX_PANIC(("CMOS: could not fstat() image file."));
      }
    if (stat_buf.st_size != BX_NUM_CMOS_REGS) {
      BX_PANIC(("CMOS: image file not same size as BX_NUM_CMOS_REGS."));
      }

    ret = ::read(fd, (bx_ptr_t) BX_CMOS_THIS s.reg, BX_NUM_CMOS_REGS);
    if (ret != BX_NUM_CMOS_REGS) {
      BX_PANIC(("CMOS: error reading cmos file."));
      }
    close(fd);
    BX_INFO(("successfuly read from image file '%s'.",
      bx_options.cmos.Opath->getptr ()));
    }
	else {
    // CMOS values generated
    BX_CMOS_THIS s.reg[0x0a] = 0x26;
    BX_CMOS_THIS s.reg[0x0b] = 0x02;
    BX_CMOS_THIS s.reg[0x0c] = 0x00;
    BX_CMOS_THIS s.reg[0x0d] = 0x80;
#if BX_SUPPORT_FPU == 1
    BX_CMOS_THIS s.reg[0x14] = 0x02;
#endif
    }
}

	void
bx_cmos_c::reset(unsigned type)
{
	BX_CMOS_THIS s.cmos_mem_address = 0;

	// RESET affects the following registers:
	//  CRA: no effects
	//  CRB: bits 4,5,6 forced to 0
	//  CRC: bits 4,5,6,7 forced to 0
	//  CRD: no effects
	BX_CMOS_THIS s.reg[0x0b] &= 0x8f;
	BX_CMOS_THIS s.reg[0x0c] = 0;

	// One second timer for updating clock & alarm functions
	bx_pc_system.activate_timer(BX_CMOS_THIS s.one_second_timer_index,
			                   1000000, 1);

	// handle periodic interrupt rate select
	BX_CMOS_THIS CRA_change();
}

	void
bx_cmos_c::CRA_change(void)
{
	unsigned nibble;

	// Periodic Interrupt timer
	nibble = BX_CMOS_THIS s.reg[0x0a] & 0x0f;
	if (nibble == 0) {
    // No Periodic Interrupt Rate when 0, deactivate timer
    bx_pc_system.deactivate_timer(BX_CMOS_THIS s.periodic_timer_index);
    BX_CMOS_THIS s.periodic_interval_usec = (Bit32u) -1; // max value
    }
	else {
    // values 0001b and 0010b are the same as 1000b and 1001b
    if (nibble <= 2)
      nibble += 7;
    BX_CMOS_THIS s.periodic_interval_usec = (unsigned) (1000000.0L /
		 (32768.0L / (1 << (nibble - 1))));

    // if Periodic Interrupt Enable bit set, activate timer
    if ( BX_CMOS_THIS s.reg[0x0b] & 0x40 )
      bx_pc_system.activate_timer(BX_CMOS_THIS s.periodic_timer_index,
		 BX_CMOS_THIS s.periodic_interval_usec, 1);
    else
      bx_pc_system.deactivate_timer(BX_CMOS_THIS s.periodic_timer_index);
    }
}


	// static IO port read callback handler
	// redirects to non-static class handler to avoid virtual functions

	Bit32u
bx_cmos_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_CMOS_SMF
	bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;

	return( class_ptr->read(address, io_len) );
}

	Bit32u
bx_cmos_c::read(Bit32u address, unsigned io_len)
{
#else
	UNUSED(this_ptr);
#endif
	Bit8u ret8;

	if (io_len > 1)
    BX_PANIC(("io read from address 0x%04x len=%u",
			  (unsigned) address, (unsigned) io_len));

	if (bx_dbg.cmos)
    BX_INFO(("CMOS read of CMOS register 0x%02x",
      (unsigned) BX_CMOS_THIS s.cmos_mem_address));


	switch (address) {
    case 0x0071:
      if (BX_CMOS_THIS s.cmos_mem_address >= BX_NUM_CMOS_REGS) {
		 BX_PANIC(("unsupported cmos io read, register(0x%02x)!",
			 (unsigned) BX_CMOS_THIS s.cmos_mem_address));
		 }

      ret8 = BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address];
      // all bits of Register C are cleared after a read occurs.
      if (BX_CMOS_THIS s.cmos_mem_address == 0x0c) {
        BX_CMOS_THIS s.reg[0x0c] = 0x00;
        BX_CMOS_THIS devices->pic->lower_irq(8);
        }
      return(ret8);
      break;

    default:
      BX_PANIC(("unsupported cmos read, address=0x%04x!",
		 (unsigned) address));
      return(0);
      break;
    }
}


	// static IO port write callback handler
	// redirects to non-static class handler to avoid virtual functions

	void
bx_cmos_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_CMOS_SMF
	bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;

	class_ptr->write(address, value, io_len);
}

	void
bx_cmos_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
	UNUSED(this_ptr);
#endif  // !BX_USE_CMOS_SMF

	if (io_len > 1)
    BX_PANIC(("io write to address 0x%04x len=%u",
			  (unsigned) address, (unsigned) io_len));

	if (bx_dbg.cmos)
    BX_INFO(("CMOS write to address: 0x%04x = 0x%02x",
      (unsigned) address, (unsigned) value));


	switch (address) {
    case 0x0070:
#if (BX_NUM_CMOS_REGS == 64)
      BX_CMOS_THIS s.cmos_mem_address = value & 0x3F;
#else
      BX_CMOS_THIS s.cmos_mem_address = value & 0x7F;
#endif
      break;

    case 0x0071:
      if (BX_CMOS_THIS s.cmos_mem_address >= BX_NUM_CMOS_REGS) {
		 BX_PANIC(("unsupported cmos io write, register(0x%02x) = 0x%02x !",
			 (unsigned) BX_CMOS_THIS s.cmos_mem_address, (unsigned) value));
		 return;
		 }
      switch (BX_CMOS_THIS s.cmos_mem_address) {
		 case 0x00: // seconds
		 case 0x01: // seconds alarm
		 case 0x02: // minutes
		 case 0x03: // minutes alarm
		 case 0x04: // hours
		 case 0x05: // hours alarm
		 case 0x06: // day of the week
		 case 0x07: // day of the month
		 case 0x08: // month
		 case 0x09: // year
		   //BX_INFO(("write reg 0x%02x: value = 0x%02x",
		   //    (unsigned) BX_CMOS_THIS s.cmos_mem_address, (unsigned) value);
		   BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address] = value;
		   return;
		   break;

		 case 0x0a: // Control Register A
		   // bit 7: Update in Progress (read-only)
		   //   1 = signifies time registers will be updated within 244us
		   //   0 = time registers will not occur before 244us
		   //   note: this bit reads 0 when CRB bit 7 is 1
		   // bit 6..4: Divider Chain Control
		   //   000 oscillator disabled
		   //   001 oscillator disabled
		   //   010 Normal operation
		   //   011 TEST
		   //   100 TEST
		   //   101 TEST
		   //   110 Divider Chain RESET
		   //   111 Divider Chain RESET
		   // bit 3..0: Periodic Interrupt Rate Select
		   //   0000 None
		   //   0001 3.90625  ms
		   //   0010 7.8125   ms
		   //   0011 122.070  us
		   //   0100 244.141  us
		   //   0101 488.281  us
		   //   0110 976.562  us
		   //   0111 1.953125 ms
		   //   1000 3.90625  ms
		   //   1001 7.8125   ms
		   //   1010 15.625   ms
		   //   1011 31.25    ms
		   //   1100 62.5     ms
		   //   1101 125      ms
		   //   1110 250      ms
		   //   1111 500      ms

		   unsigned dcc;
		   dcc = (value >> 4) & 0x07;
		   if (dcc != 0x02) {
			 BX_PANIC(("CRA: divider chain control 0x%02x", dcc));
			 }
		   BX_CMOS_THIS s.reg[0x0a] = value & 0x7f;
		   BX_CMOS_THIS CRA_change();
		   return;
		   break;

		 case 0x0b: // Control Register B
		   // bit 0: Daylight Savings Enable
		   //   1 = enable daylight savings
		   //   0 = disable daylight savings
		   // bit 1: 24/12 houre mode
		   //   1 = 24 hour format
		   //   0 = 12 hour format
		   // bit 2: Data Mode
		   //   1 = binary format
		   //   0 = BCD format
		   // bit 3: "square wave enable"
		   //   Not supported and always read as 0
		   // bit 4: Update Ended Interrupt Enable
		   //   1 = enable generation of update ended interrupt
		   //   0 = disable
		   // bit 5: Alarm Interrupt Enable
		   //   1 = enable generation of alarm interrupt
		   //   0 = disable
		   // bit 6: Periodic Interrupt Enable
		   //   1 = enable generation of periodic interrupt
		   //   0 = disable
		   // bit 7: Set mode
		   //   1 = user copy of time is "frozen" allowing time registers
		   //       to be accessed without regard for an occurance of an update
		   //   0 = time updates occur normally

		   // can not handle binary or 12-hour mode yet.
		   if (value & 0x04)
			 BX_PANIC(("write status reg B, binary format enabled."));
		   if ( !(value & 0x02) )
			 BX_PANIC(("write status reg B, 12 hour mode enabled."));

		   value &= 0xf7; // bit3 always 0
		   // Note: setting bit 7 clears bit 4
		   if (value & 0x80)
			 value &= 0xef;

		   unsigned prev_CRB;
		   prev_CRB = BX_CMOS_THIS s.reg[0x0b];
		   BX_CMOS_THIS s.reg[0x0b] = value;
		   if ( (prev_CRB & 0x40) != (value & 0x40) ) {
			 // Periodic Interrupt Enabled changed
			 if (prev_CRB & 0x40) {
			   // transition from 1 to 0, deactivate timer
			   bx_pc_system.deactivate_timer(
			     BX_CMOS_THIS s.periodic_timer_index);
			   }
			 else {
			   // transition from 0 to 1
			   // if rate select is not 0, activate timer
			   if ( (BX_CMOS_THIS s.reg[0x0a] & 0x0f) != 0 ) {
			     bx_pc_system.activate_timer(
			       BX_CMOS_THIS s.periodic_timer_index,
			       BX_CMOS_THIS s.periodic_interval_usec, 1);
			     }
			   }
			 }
		   return;
		   break;

		 case 0x0c: // Control Register C
		 case 0x0d: // Control Register D
		   BX_ERROR(("write to control register 0x%02x (read-only)",
			        BX_CMOS_THIS s.cmos_mem_address));
		   break;

		 case 0x0e: // diagnostic status
		   BX_DEBUG(("write register 0x0e: 0x%02x", (unsigned) value));;
		   break;

	case 0x0f: // shutdown status
		   switch (value) {
			 case 0x00: /* proceed with normal POST (soft reset) */
			   BX_DEBUG(("Reg 0F set to 0: shutdown action = normal POST"));;
			   break;
			 case 0x02: /* shutdown after memory test */
			   BX_DEBUG(("Reg 0Fh: request to change shutdown action"
			                  " to shutdown after memory test"));
			   break;
			 case 0x03:
			   BX_DEBUG(("Reg 0Fh(03) : Shutdown after memory test !"));;
			   break;
			 case 0x04: /* jump to disk bootstrap routine */
			   BX_DEBUG(("Reg 0Fh: request to change shutdown action "
			                  "to jump to disk bootstrap routine."));
			   break;
			 case 0x06:
			   BX_DEBUG(("Reg 0Fh(06) : Shutdown after memory test !"));;
			   break;
			 case 0x09: /* return to BIOS extended memory block move
			            (interrupt 15h, func 87h was in progress) */
			   BX_DEBUG(("Reg 0Fh: request to change shutdown action "
			                  "to return to BIOS extended memory block move."));
			   break;
			 case 0x0a: /* jump to DWORD pointer at 40:67 */
			   BX_DEBUG(("Reg 0Fh: request to change shutdown action"
			                  " to jump to DWORD at 40:67"));
			   break;
			 default:
			   BX_PANIC(("unsupported cmos io write to reg F, case 0x%02x!",
			     (unsigned) value));
			   break;
			 }
		   break;

		 default:
		   BX_DEBUG(("write reg 0x%02x: value = 0x%02x",
			 (unsigned) BX_CMOS_THIS s.cmos_mem_address, (unsigned) value));
		   break;
		 }

      BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address] = value;
      break;
    }
}


	void
bx_cmos_c::checksum_cmos(void)
{
	unsigned i;
	Bit16u sum;

	sum = 0;
	for (i=0x10; i<=0x2d; i++) {
    sum += BX_CMOS_THIS s.reg[i];
    }
	BX_CMOS_THIS s.reg[0x2e] = (sum >> 8) & 0xff; /* checksum high */
	BX_CMOS_THIS s.reg[0x2f] = (sum & 0xff);      /* checksum low */
}

	void
bx_cmos_c::periodic_timer_handler(void *this_ptr)
{
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;

  class_ptr->periodic_timer();
}

  void
bx_cmos_c::periodic_timer()
{
  // if periodic interrupts are enabled, trip IRQ 8, and
  // update status register C
  if (BX_CMOS_THIS s.reg[0x0b] & 0x40) {
    BX_CMOS_THIS s.reg[0x0c] |= 0xc0; // Interrupt Request, Periodic Int
    BX_CMOS_THIS devices->pic->raise_irq(8);
    }
}

	void
bx_cmos_c::one_second_timer_handler(void *this_ptr)
{
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;

  class_ptr->one_second_timer();
}

  void
bx_cmos_c::one_second_timer()
{
  // update internal time/date buffer
  BX_CMOS_THIS s.timeval++;

  // Dont update CMOS user copy of time/date if CRB bit7 is 1
  // Nothing else do to
  if (BX_CMOS_THIS s.reg[0x0b] & 0x80)
    return;

  update_clock();

  // if update interrupts are enabled, trip IRQ 8, and
  // update status register C
  if (BX_CMOS_THIS s.reg[0x0b] & 0x10) {
    BX_CMOS_THIS s.reg[0x0c] |= 0x90; // Interrupt Request, Update Ended
    BX_CMOS_THIS devices->pic->raise_irq(8);
    }

  // compare CMOS user copy of time/date to alarm time/date here
  if (BX_CMOS_THIS s.reg[0x0b] & 0x20) {
    // Alarm interrupts enabled
    Boolean alarm_match = 1;
    if ( (BX_CMOS_THIS s.reg[0x01] & 0xc0) != 0xc0 ) {
      // seconds alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[0x00] != BX_CMOS_THIS s.reg[0x01])
		 alarm_match = 0;
      }
    if ( (BX_CMOS_THIS s.reg[0x03] & 0xc0) != 0xc0 ) {
      // minutes alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[0x02] != BX_CMOS_THIS s.reg[0x03])
		 alarm_match = 0;
      }
    if ( (BX_CMOS_THIS s.reg[0x05] & 0xc0) != 0xc0 ) {
      // hours alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[0x04] != BX_CMOS_THIS s.reg[0x05])
		 alarm_match = 0;
      }
    if (alarm_match) {
      BX_CMOS_THIS s.reg[0x0c] |= 0xa0; // Interrupt Request, Alarm Int
      BX_CMOS_THIS devices->pic->raise_irq(8);
      }
    }
}


	void
bx_cmos_c::update_clock()
{
	struct tm *time_calendar;
	unsigned year, month, day, century;
	Bit8u val_bcd;

	time_calendar = localtime(& BX_CMOS_THIS s.timeval);

	// update seconds
	val_bcd =
     ((time_calendar->tm_sec  / 10) << 4) |
     (time_calendar->tm_sec % 10);
	BX_CMOS_THIS s.reg[0x00] = val_bcd;

	// update minutes
	val_bcd =
     ((time_calendar->tm_min  / 10) << 4) |
     (time_calendar->tm_min % 10);
	BX_CMOS_THIS s.reg[0x02] = val_bcd;

	// update hours
	val_bcd =
     ((time_calendar->tm_hour  / 10) << 4) |
     (time_calendar->tm_hour % 10);
	BX_CMOS_THIS s.reg[0x04] = val_bcd;

	// update day of the week
	day = time_calendar->tm_wday + 1; // 0..6 to 1..7
	BX_CMOS_THIS s.reg[0x06] = ((day / 10) << 4) | (day % 10);

	// update day of the month
	day = time_calendar->tm_mday;
	BX_CMOS_THIS s.reg[0x07] = ((day / 10) << 4) | (day % 10);

	// update month
	month   = time_calendar->tm_mon + 1;
	BX_CMOS_THIS s.reg[0x08] = ((month / 10) << 4) | (month % 10);

	// update year
	year = time_calendar->tm_year % 100;
	BX_CMOS_THIS s.reg[0x09] = ((year  / 10) << 4) | (year % 10);

	// update century
	century = (time_calendar->tm_year / 100) + 19;
	BX_CMOS_THIS s.reg[0x32] = ((century  / 10) << 4) | (century % 10);
}
