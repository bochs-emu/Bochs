/////////////////////////////////////////////////////////////////////////
// $Id: pic.cc,v 1.26 2002-04-20 16:19:42 instinc Exp $
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
#define LOG_THIS bx_pic.



bx_pic_c bx_pic;
#if BX_USE_PIC_SMF
#define this (&bx_pic)
#endif



bx_pic_c::bx_pic_c(void)
{
	put("PIC");
	settype(PICLOG);
}

bx_pic_c::~bx_pic_c(void)
{
  // nothing for now
}


  void
bx_pic_c::init(bx_devices_c *d)
{
  BX_PIC_THIS devices = d;

  /* 8259 PIC (Programmable Interrupt Controller) */
  BX_PIC_THIS devices->register_io_read_handler(this, read_handler, 0x0020, "8259 PIC");
  BX_PIC_THIS devices->register_io_read_handler(this, read_handler, 0x0021, "8259 PIC");
  BX_PIC_THIS devices->register_io_read_handler(this, read_handler, 0x00A0, "8259 PIC");
  BX_PIC_THIS devices->register_io_read_handler(this, read_handler, 0x00A1, "8259 PIC");

  BX_PIC_THIS devices->register_io_write_handler(this, write_handler, 0x0020, "8259 PIC");
  BX_PIC_THIS devices->register_io_write_handler(this, write_handler, 0x0021, "8259 PIC");
  BX_PIC_THIS devices->register_io_write_handler(this, write_handler, 0x00A0, "8259 PIC");
  BX_PIC_THIS devices->register_io_write_handler(this, write_handler, 0x00A1, "8259 PIC");


  BX_PIC_THIS s.master_pic.single_PIC = 0;
  BX_PIC_THIS s.master_pic.interrupt_offset = 0x08; /* IRQ0 = INT 0x08 */
  /* slave PIC connected to IRQ2 of master */
  BX_PIC_THIS s.master_pic.u.slave_connect_mask = 0x04;
  BX_PIC_THIS s.master_pic.sfnm = 0; /* normal nested mode */
  BX_PIC_THIS s.master_pic.buffered_mode = 0; /* unbuffered mode */
  BX_PIC_THIS s.master_pic.master_slave  = 0; /* no meaning, buffered_mode=0 */
  BX_PIC_THIS s.master_pic.auto_eoi      = 0; /* manual EOI from CPU */
  BX_PIC_THIS s.master_pic.imr           = 0xFF; /* all IRQ's initially masked */
  BX_PIC_THIS s.master_pic.isr           = 0x00; /* no IRQ's in service */
  BX_PIC_THIS s.master_pic.irr           = 0x00; /* no IRQ's requested */
  BX_PIC_THIS s.master_pic.read_reg_select = 0; /* IRR */
  BX_PIC_THIS s.master_pic.irq = 0;
  BX_PIC_THIS s.master_pic.INT = 0;
  BX_PIC_THIS s.master_pic.init.in_init = 0;
  BX_PIC_THIS s.master_pic.init.requires_4 = 0;
  BX_PIC_THIS s.master_pic.init.byte_expected = 0;
  BX_PIC_THIS s.master_pic.special_mask = 0;
  BX_PIC_THIS s.master_pic.lowest_priority = 7;
  BX_PIC_THIS s.master_pic.polled = 0;
  BX_PIC_THIS s.master_pic.rotate_on_autoeoi = 0;

  BX_PIC_THIS s.slave_pic.single_PIC = 0;
  BX_PIC_THIS s.slave_pic.interrupt_offset = 0x70; /* IRQ8 = INT 0x70 */
  BX_PIC_THIS s.slave_pic.u.slave_id = 0x02; /* slave PIC connected to IRQ2 of master */
  BX_PIC_THIS s.slave_pic.sfnm       = 0; /* normal nested mode */
  BX_PIC_THIS s.slave_pic.buffered_mode = 0; /* unbuffered mode */
  BX_PIC_THIS s.slave_pic.master_slave  = 0; /* no meaning, buffered_mode=0 */
  BX_PIC_THIS s.slave_pic.auto_eoi      = 0; /* manual EOI from CPU */
  BX_PIC_THIS s.slave_pic.imr           = 0xFF; /* all IRQ's initially masked */
  BX_PIC_THIS s.slave_pic.isr           = 0x00; /* no IRQ's in service */
  BX_PIC_THIS s.slave_pic.irr           = 0x00; /* no IRQ's requested */
  BX_PIC_THIS s.slave_pic.read_reg_select = 0; /* IRR */
  BX_PIC_THIS s.slave_pic.irq = 0;
  BX_PIC_THIS s.slave_pic.INT = 0;
  BX_PIC_THIS s.slave_pic.init.in_init = 0;
  BX_PIC_THIS s.slave_pic.init.requires_4 = 0;
  BX_PIC_THIS s.slave_pic.init.byte_expected = 0;
  BX_PIC_THIS s.slave_pic.special_mask = 0;
  BX_PIC_THIS s.slave_pic.lowest_priority = 7;
  BX_PIC_THIS s.slave_pic.polled = 0;
  BX_PIC_THIS s.slave_pic.rotate_on_autoeoi = 0;

  for (unsigned i=0; i<8; i++) { /* all IRQ lines low */
    BX_PIC_THIS s.master_pic.IRQ_line[i] = 0;
    BX_PIC_THIS s.slave_pic.IRQ_line[i] = 0;
  }
}



  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pic_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIC_SMF
  bx_pic_c *class_ptr = (bx_pic_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}



  Bit32u
bx_pic_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIC_SMF
  if (io_len > 1)
    BX_PANIC(("io read from port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  BX_DEBUG(("IO read from %04x", (unsigned) address));

  /*
   8259A PIC
   */

  if((address == 0x20 || address == 0x21) && BX_PIC_THIS s.master_pic.polled) {
    // In polled mode. Treat this as an interrupt acknowledge
    clear_highest_interrupt(& BX_PIC_THIS s.master_pic);
    BX_PIC_THIS s.master_pic.polled = 0;
    service_master_pic();
    return BX_PIC_THIS s.master_pic.irq;  // Return the current irq requested
  }

  if((address == 0xa0 || address == 0xa1) && BX_PIC_THIS s.slave_pic.polled) {
    // In polled mode. Treat this as an interrupt acknowledge
    clear_highest_interrupt(& BX_PIC_THIS s.slave_pic);
    BX_PIC_THIS s.slave_pic.polled = 0;
    service_slave_pic();
    return BX_PIC_THIS s.slave_pic.irq;  // Return the current irq requested
  }


  switch (address) {
    case 0x20:
      if (BX_PIC_THIS s.master_pic.read_reg_select) { /* ISR */
        BX_DEBUG(("read master ISR = %02x",
                  (unsigned) BX_PIC_THIS s.master_pic.isr));
	return(BX_PIC_THIS s.master_pic.isr);
	}
      else { /* IRR */
        BX_DEBUG(("read master IRR = %02x",
                 (unsigned) BX_PIC_THIS s.master_pic.irr));
	return(BX_PIC_THIS s.master_pic.irr);
	}
      break;
    case 0x21:
      BX_DEBUG(("read master IMR = %02x",
               (unsigned) BX_PIC_THIS s.master_pic.imr));
      return(BX_PIC_THIS s.master_pic.imr);
      break;
    case 0xA0:
      if (BX_PIC_THIS s.slave_pic.read_reg_select) { /* ISR */
        BX_DEBUG(("read slave ISR = %02x",
                  (unsigned) BX_PIC_THIS s.slave_pic.isr));
	return(BX_PIC_THIS s.slave_pic.isr);
	}
      else { /* IRR */
        BX_DEBUG(("read slave IRR = %02x",
                  (unsigned) BX_PIC_THIS s.slave_pic.irr));
	return(BX_PIC_THIS s.slave_pic.irr);
	}
      break;
    case 0xA1:
      BX_DEBUG(("read slave IMR = %02x",
                (unsigned) BX_PIC_THIS s.slave_pic.imr));
      return(BX_PIC_THIS s.slave_pic.imr);
      break;
    }

  BX_PANIC(("io read to address %04x", (unsigned) address));
  return(0); /* default if not found above */
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pic_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PIC_SMF
  bx_pic_c *class_ptr = (bx_pic_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pic_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIC_SMF

  if (io_len > 1)
    BX_PANIC(("io write to port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  BX_DEBUG(("IO write to %04x = %02x", (unsigned) address, (unsigned) value));

  /*
   8259A PIC
   */

  switch (address) {
    case 0x20:
      if (value & 0x10) { /* initialization command 1 */
        BX_DEBUG(("master: init command 1 found"));
        BX_DEBUG(("        requires 4 = %u", (unsigned) (value & 0x01) ));
        BX_DEBUG(("        cascade mode: [0=cascade,1=single] %u",
                  (unsigned) ((value & 0x02) >> 1)));
        BX_PIC_THIS s.master_pic.init.in_init = 1;
        BX_PIC_THIS s.master_pic.init.requires_4 = (value & 0x01);
        BX_PIC_THIS s.master_pic.init.byte_expected = 2; /* operation command 2 */
        BX_PIC_THIS s.master_pic.imr           = 0x00; /* all IRQ's initially masked */
        BX_PIC_THIS s.master_pic.isr           = 0x00; /* no IRQ's in service */
        BX_PIC_THIS s.master_pic.irr           = 0x00; /* no IRQ's requested */
        BX_PIC_THIS s.master_pic.lowest_priority = 7;
        BX_PIC_THIS s.master_pic.INT = 0; /* reprogramming clears previous INTR request */
        BX_PIC_THIS s.master_pic.auto_eoi = 0;
        BX_PIC_THIS s.master_pic.rotate_on_autoeoi = 0;
        if (value & 0x02)
          BX_PANIC(("master: ICW1: single mode not supported"));
        if (value & 0x08) {
          BX_PANIC(("master: ICW1: level sensitive mode not supported"));
	  }
	else {
          BX_DEBUG(("master: ICW1: edge triggered mode selected"));
	  }
        BX_SET_INTR(0);
        return;
        }

      if ( (value & 0x18) == 0x08 ) { /* OCW3 */
        Bit8u special_mask, poll, read_op;

        special_mask = (value & 0x60) >> 5;
        poll         = (value & 0x04) >> 2;
        read_op      = (value & 0x03);
        if (poll) {
          BX_PIC_THIS s.master_pic.polled = 1;
          return;
        }
        if (read_op == 0x02) /* read IRR */
	  BX_PIC_THIS s.master_pic.read_reg_select = 0;
        else if (read_op == 0x03) /* read ISR */
	  BX_PIC_THIS s.master_pic.read_reg_select = 1;
        if (special_mask == 0x02) { /* cancel special mask */
          BX_PIC_THIS s.master_pic.special_mask = 0;
          }
        else if (special_mask == 0x03) { /* set specific mask */
          BX_PIC_THIS s.master_pic.special_mask = 1;
          service_master_pic();
          }
        return;
        }

      /* OCW2 */
      switch (value) {
        case 0x00: // Rotate in auto eoi mode clear
        case 0x80: // Rotate in auto eoi mode set
          BX_PIC_THIS s.master_pic.rotate_on_autoeoi = (value != 0);
          break;
	case 0x0A: /* select read interrupt request register */
	  BX_PIC_THIS s.master_pic.read_reg_select = 0;
	  break;
	case 0x0B: /* select read interrupt in-service register */
	  BX_PIC_THIS s.master_pic.read_reg_select = 1;
	  break;

        case 0xA0: // Rotate on non-specific end of interrupt
        case 0x20: /* end of interrupt command */

          clear_highest_interrupt(& BX_PIC_THIS s.master_pic);

          if(value == 0xA0) {// Rotate in Auto-EOI mode
            BX_PIC_THIS s.master_pic.lowest_priority ++;
            if(BX_PIC_THIS s.master_pic.lowest_priority > 7)
              BX_PIC_THIS s.master_pic.lowest_priority = 0;
          }

          service_master_pic();
          break;

        case 0x40: // Intel PIC spec-sheet seems to indicate this should be ignored
          BX_INFO(("IRQ no-op"));
          break;

        case 0x60: /* specific EOI 0 */
        case 0x61: /* specific EOI 1 */
        case 0x62: /* specific EOI 2 */
        case 0x63: /* specific EOI 3 */
        case 0x64: /* specific EOI 4 */
        case 0x65: /* specific EOI 5 */
        case 0x66: /* specific EOI 6 */
        case 0x67: /* specific EOI 7 */
          BX_PIC_THIS s.master_pic.isr &= ~(1 << (value-0x60));
          service_master_pic();
          break;

        // IRQ lowest priority commands
        case 0xC0: // 0 7 6 5 4 3 2 1
        case 0xC1: // 1 0 7 6 5 4 3 2
        case 0xC2: // 2 1 0 7 6 5 4 3
        case 0xC3: // 3 2 1 0 7 6 5 4
        case 0xC4: // 4 3 2 1 0 7 6 5
        case 0xC5: // 5 4 3 2 1 0 7 6
        case 0xC6: // 6 5 4 3 2 1 0 7
        case 0xC7: // 7 6 5 4 3 2 1 0
          BX_INFO(("IRQ lowest command 0x%x", value));
          BX_PIC_THIS s.master_pic.lowest_priority = value - 0xC0;
          break;

        case 0xE0: // specific EOI and rotate 0
        case 0xE1: // specific EOI and rotate 1
        case 0xE2: // specific EOI and rotate 2
        case 0xE3: // specific EOI and rotate 3
        case 0xE4: // specific EOI and rotate 4
        case 0xE5: // specific EOI and rotate 5
        case 0xE6: // specific EOI and rotate 6
        case 0xE7: // specific EOI and rotate 7
          BX_PIC_THIS s.master_pic.isr &= ~(1 << (value-0xE0));
          BX_PIC_THIS s.master_pic.lowest_priority = (value - 0xE0);
          service_master_pic();

          break;

        default:
          BX_PANIC(("write to port 20h = %02x", value));
	} /* switch (value) */
      break;

    case 0x21:
      /* initialization mode operation */
      if (BX_PIC_THIS s.master_pic.init.in_init) {
        switch (BX_PIC_THIS s.master_pic.init.byte_expected) {
          case 2:
            BX_PIC_THIS s.master_pic.interrupt_offset = value & 0xf8;
            BX_PIC_THIS s.master_pic.init.byte_expected = 3;
            BX_DEBUG(("master: init command 2 = %02x", (unsigned) value));
            BX_DEBUG(("        offset = INT %02x",
                      BX_PIC_THIS s.master_pic.interrupt_offset));
            return;
            break;
          case 3:
            BX_DEBUG(("master: init command 3 = %02x", (unsigned) value));
            if (BX_PIC_THIS s.master_pic.init.requires_4) {
              BX_PIC_THIS s.master_pic.init.byte_expected = 4;
	      }
            else {
              BX_PIC_THIS s.master_pic.init.in_init = 0;
	      }
            return;
            break;
          case 4:
            BX_DEBUG(("master: init command 4 = %02x", (unsigned) value));
            if (value & 0x02) {
              BX_DEBUG(("       auto EOI"));
              BX_PIC_THIS s.master_pic.auto_eoi = 1;
              }
            else {
              BX_DEBUG(("normal EOI interrupt"));
              BX_PIC_THIS s.master_pic.auto_eoi = 0;
              }
	    if (value & 0x01) {
		  BX_DEBUG(("       80x86 mode"));
	    } else
		  BX_PANIC(("       not 80x86 mode"));
            BX_PIC_THIS s.master_pic.init.in_init = 0;
            return;
            break;
          default:
            BX_PANIC(("master expecting bad init command"));
          }
        }

      /* normal operation */
      BX_DEBUG(("setting master pic IMR to %02x", value));
      BX_PIC_THIS s.master_pic.imr = value;
      service_master_pic();
      return;
      break;

    case 0xA0:
      if (value & 0x10) { /* initialization command 1 */
        BX_DEBUG(("slave: init command 1 found"));
        BX_DEBUG(("       requires 4 = %u",
            (unsigned) (value & 0x01) ));
        BX_DEBUG(("       cascade mode: [0=cascade,1=single] %u",
            (unsigned) ((value & 0x02) >> 1)));
        BX_PIC_THIS s.slave_pic.init.in_init = 1;
        BX_PIC_THIS s.slave_pic.init.requires_4 = (value & 0x01);
        BX_PIC_THIS s.slave_pic.init.byte_expected = 2; /* operation command 2 */
        BX_PIC_THIS s.slave_pic.imr           = 0x00; /* clear irq mask */
        BX_PIC_THIS s.slave_pic.isr           = 0x00; /* no IRQ's in service */
        BX_PIC_THIS s.slave_pic.irr           = 0x00; /* no IRQ's requested */
        BX_PIC_THIS s.slave_pic.lowest_priority = 7;
        BX_PIC_THIS s.slave_pic.INT = 0; /* reprogramming clears previous INTR request */
        BX_PIC_THIS s.slave_pic.auto_eoi = 0;
        BX_PIC_THIS s.slave_pic.rotate_on_autoeoi = 0;
        if (value & 0x02)
          BX_PANIC(("slave: ICW1: single mode not supported"));
        if (value & 0x08) {
          BX_PANIC(("slave: ICW1: level sensitive mode not supported"));
	  }
	else {
          BX_DEBUG(("slave: ICW1: edge triggered mode selected"));
	  }
        return;
        }

      if ( (value & 0x18) == 0x08 ) { /* OCW3 */
        Bit8u special_mask, poll, read_op;

        special_mask = (value & 0x60) >> 5;
        poll         = (value & 0x04) >> 2;
        read_op      = (value & 0x03);
        if (poll) {
          BX_PIC_THIS s.slave_pic.polled = 1;
          return;
        }
        if (read_op == 0x02) /* read IRR */
	  BX_PIC_THIS s.slave_pic.read_reg_select = 0;
        else if (read_op == 0x03) /* read ISR */
	  BX_PIC_THIS s.slave_pic.read_reg_select = 1;
        if (special_mask == 0x02) { /* cancel special mask */
          BX_PIC_THIS s.slave_pic.special_mask = 0;
          }
        else if (special_mask == 0x03) { /* set specific mask */
          BX_PIC_THIS s.slave_pic.special_mask = 1;
          service_slave_pic();
          }
        return;
        }

      switch (value) {
        case 0x00: // Rotate in auto eoi mode clear
        case 0x80: // Rotate in auto eoi mode set
          BX_PIC_THIS s.slave_pic.rotate_on_autoeoi = (value != 0);
          break;

	case 0x0A: /* select read interrupt request register */
	  BX_PIC_THIS s.slave_pic.read_reg_select = 0;
	  break;
	case 0x0B: /* select read interrupt in-service register */
	  BX_PIC_THIS s.slave_pic.read_reg_select = 1;
	  break;

        case 0xA0: // Rotate on non-specific end of interrupt
        case 0x20: /* end of interrupt command */

          clear_highest_interrupt(& BX_PIC_THIS s.slave_pic);

          if(value == 0xA0) {// Rotate in Auto-EOI mode
            BX_PIC_THIS s.slave_pic.lowest_priority ++;
            if(BX_PIC_THIS s.slave_pic.lowest_priority > 7)
              BX_PIC_THIS s.slave_pic.lowest_priority = 0;
          }

          service_slave_pic();
          break;

        case 0x40: // Intel PIC spec-sheet seems to indicate this should be ignored
          BX_INFO(("IRQ no-op"));
          break;

        case 0x60: /* specific EOI 0 */
        case 0x61: /* specific EOI 1 */
        case 0x62: /* specific EOI 2 */
        case 0x63: /* specific EOI 3 */
        case 0x64: /* specific EOI 4 */
        case 0x65: /* specific EOI 5 */
        case 0x66: /* specific EOI 6 */
        case 0x67: /* specific EOI 7 */
          BX_PIC_THIS s.slave_pic.isr &= ~(1 << (value-0x60));
          service_slave_pic();
          break;

        // IRQ lowest priority commands
        case 0xC0: // 0 7 6 5 4 3 2 1
        case 0xC1: // 1 0 7 6 5 4 3 2
        case 0xC2: // 2 1 0 7 6 5 4 3
        case 0xC3: // 3 2 1 0 7 6 5 4
        case 0xC4: // 4 3 2 1 0 7 6 5
        case 0xC5: // 5 4 3 2 1 0 7 6
        case 0xC6: // 6 5 4 3 2 1 0 7
        case 0xC7: // 7 6 5 4 3 2 1 0
          BX_INFO(("IRQ lowest command 0x%x", value));
          BX_PIC_THIS s.slave_pic.lowest_priority = value - 0xC0;
          break;

        case 0xE0: // specific EOI and rotate 0
        case 0xE1: // specific EOI and rotate 1
        case 0xE2: // specific EOI and rotate 2
        case 0xE3: // specific EOI and rotate 3
        case 0xE4: // specific EOI and rotate 4
        case 0xE5: // specific EOI and rotate 5
        case 0xE6: // specific EOI and rotate 6
        case 0xE7: // specific EOI and rotate 7
          BX_PIC_THIS s.slave_pic.isr &= ~(1 << (value-0xE0));
          BX_PIC_THIS s.slave_pic.lowest_priority = (value - 0xE0);
          service_slave_pic();

          break;

        default:
          BX_PANIC(("write to port A0h = %02x", value));
	} /* switch (value) */
      break;

    case 0xA1:
      /* initialization mode operation */
      if (BX_PIC_THIS s.slave_pic.init.in_init) {
        switch (BX_PIC_THIS s.slave_pic.init.byte_expected) {
          case 2:
            BX_PIC_THIS s.slave_pic.interrupt_offset = value & 0xf8;
            BX_PIC_THIS s.slave_pic.init.byte_expected = 3;
            BX_DEBUG(("slave: init command 2 = %02x", (unsigned) value));
            BX_DEBUG(("       offset = INT %02x",
                      BX_PIC_THIS s.slave_pic.interrupt_offset));
            return;
            break;
          case 3:
            BX_DEBUG(("slave: init command 3 = %02x", (unsigned) value));
            if (BX_PIC_THIS s.slave_pic.init.requires_4) {
              BX_PIC_THIS s.slave_pic.init.byte_expected = 4;
	      	} else {
              BX_PIC_THIS s.slave_pic.init.in_init = 0;
	      }
            return;
            break;
          case 4:
            BX_DEBUG(("slave: init command 4 = %02x", (unsigned) value));
            if (value & 0x02) {
              BX_DEBUG(("       auto EOI"));
              BX_PIC_THIS s.slave_pic.auto_eoi = 1;
              }
            else {
              BX_DEBUG(("normal EOI interrupt"));
              BX_PIC_THIS s.slave_pic.auto_eoi = 0;
              }
	    if (value & 0x01) {
		  BX_DEBUG(("       80x86 mode"));
	    } else
		  BX_PANIC(("       not 80x86 mode"));
            BX_PIC_THIS s.slave_pic.init.in_init = 0;
            return;
            break;
          default:
            BX_PANIC(("slave: expecting bad init command"));
          }
        }

      /* normal operation */
      BX_DEBUG(("setting slave pic IMR to %02x", value));
      BX_PIC_THIS s.slave_pic.imr = value;
      service_slave_pic();
      return;
      break;
    } /* switch (address) */

  return;
}

// new IRQ signal handling routines

  void
bx_pic_c::lower_irq(unsigned irq_no)
{
#if BX_SUPPORT_APIC
  // forward this function call to the ioapic too
  if ((BX_PIC_THIS devices) && (BX_PIC_THIS devices->ioapic))
    BX_PIC_THIS devices->ioapic->untrigger_irq (irq_no, -1);
#endif

  if ((irq_no <= 7) && (BX_PIC_THIS s.master_pic.IRQ_line[irq_no])) {
    BX_DEBUG(("IRQ line %d now low", (unsigned) irq_no));
    BX_PIC_THIS s.master_pic.IRQ_line[irq_no] = 0;
    BX_PIC_THIS s.master_pic.irr &= ~(1 << irq_no);
    if ((BX_PIC_THIS s.master_pic.irr & ~BX_PIC_THIS s.master_pic.imr) == 0) {
      BX_SET_INTR(0);
      BX_PIC_THIS s.master_pic.INT = 0;
    }
  } else if ((irq_no > 7) && (irq_no <= 15) &&
             (BX_PIC_THIS s.slave_pic.IRQ_line[irq_no-8])) {
    BX_DEBUG(("IRQ line %d now low", (unsigned) irq_no));
    BX_PIC_THIS s.slave_pic.IRQ_line[irq_no - 8] = 0;
    BX_PIC_THIS s.slave_pic.irr &= ~(1 << (irq_no - 8));
    if ((BX_PIC_THIS s.slave_pic.irr & ~BX_PIC_THIS s.slave_pic.imr) == 0) {
      BX_PIC_THIS s.slave_pic.INT = 0;
      lower_irq(2);
    }
  }
}

  void
bx_pic_c::raise_irq(unsigned irq_no)
{
#if BX_SUPPORT_APIC
  // forward this function call to the ioapic too
  BX_PIC_THIS devices->ioapic->trigger_irq (irq_no, -1);
#endif

  if ((irq_no <= 7) && (!BX_PIC_THIS s.master_pic.IRQ_line[irq_no])) {
    BX_DEBUG(("IRQ line %d now high", (unsigned) irq_no));
    BX_PIC_THIS s.master_pic.IRQ_line[irq_no] = 1;
    BX_PIC_THIS s.master_pic.irr |= (1 << irq_no);
    service_master_pic();
  } else if ((irq_no > 7) && (irq_no <= 15) &&
             (!BX_PIC_THIS s.slave_pic.IRQ_line[irq_no-8])) {
    BX_DEBUG(("IRQ line %d now high", (unsigned) irq_no));
    BX_PIC_THIS s.slave_pic.IRQ_line[irq_no - 8] = 1;
    BX_PIC_THIS s.slave_pic.irr |= (1 << (irq_no - 8));
    service_slave_pic();
  }
}

void  bx_pic_c::clear_highest_interrupt(bx_pic_t *pic)
{
  int irq;
  int lowest_priority;
  int highest_priority;

  /* clear highest current in service bit */
  lowest_priority = pic->lowest_priority;
  highest_priority = lowest_priority + 1;
  if(highest_priority > 7)
    highest_priority = 0;

  irq = highest_priority;
  do {
    if (pic->isr & (1 << irq)) {
      pic->isr &= ~(1 << irq);
      break; /* Return mask of bit cleared. */
    }

    irq ++;
    if(irq > 7)
      irq = 0;
  } while(irq != highest_priority);

}

  /* */
  void
bx_pic_c::service_master_pic(void)
{
  Bit8u unmasked_requests;
  int irq;
  Bit8u isr, max_irq;
  Bit8u highest_priority = BX_PIC_THIS s.master_pic.lowest_priority + 1;
  if(highest_priority > 7)
    highest_priority = 0;

  if (BX_PIC_THIS s.master_pic.INT) { /* last interrupt still not acknowleged */
    return;
    }

  if (BX_PIC_THIS s.master_pic.special_mask) {
    /* all priorities may be enabled.  check all IRR bits except ones
     * which have corresponding ISR bits set
     */
    max_irq = highest_priority;
    }
  else { /* normal mode */
    /* Find the highest priority IRQ that is enabled due to current ISR */
    isr = BX_PIC_THIS s.master_pic.isr;
    if (isr) {
      max_irq = highest_priority;
      while ( (isr & (1 << max_irq)) == 0) {
        max_irq++;
        if(max_irq > 7)
          max_irq = 0;
        }
      if (max_irq == highest_priority ) return; /* Highest priority interrupt in-service,
                                                 * no other priorities allowed */
      if (max_irq > 7) BX_PANIC(("error in service_master_pic()"));
      }
    else
      max_irq = highest_priority; /* 0..7 bits in ISR are cleared */
    }


  /* now, see if there are any higher priority requests */
  if ((unmasked_requests = (BX_PIC_THIS s.master_pic.irr & ~BX_PIC_THIS s.master_pic.imr)) ) {
    irq = highest_priority;
    do {
      /* for special mode, since we're looking at all IRQ's, skip if
       * current IRQ is already in-service
       */
      if ( BX_PIC_THIS s.master_pic.special_mask && ((BX_PIC_THIS s.master_pic.isr >> irq) & 0x01) )
        continue;
      if (unmasked_requests & (1 << irq)) {
        BX_DEBUG(("signalling IRQ(%u)", (unsigned) irq));
        BX_PIC_THIS s.master_pic.INT = 1;
        BX_SET_INTR(1);
        BX_PIC_THIS s.master_pic.irq = irq;
        return;
        } /* if (unmasked_requests & ... */

      irq ++;
      if(irq > 7)
        irq = 0;
      } while(irq != max_irq); /* do ... */
    } /* if (unmasked_requests = ... */
}


  void
bx_pic_c::service_slave_pic(void)
{
  Bit8u unmasked_requests;
  int irq;
  Bit8u isr, max_irq;
  Bit8u highest_priority = BX_PIC_THIS s.slave_pic.lowest_priority + 1;
  if(highest_priority > 7)
    highest_priority = 0;

  if (BX_PIC_THIS s.slave_pic.INT) { /* last interrupt still not acknowleged */
    return;
    }

  if (BX_PIC_THIS s.slave_pic.special_mask) {
    /* all priorities may be enabled.  check all IRR bits except ones
     * which have corresponding ISR bits set
     */
    max_irq = highest_priority;
  }
  else { /* normal mode */
    /* Find the highest priority IRQ that is enabled due to current ISR */
    isr = BX_PIC_THIS s.slave_pic.isr;
    if (isr) {
      max_irq = highest_priority;
      while ( (isr & (1 << max_irq)) == 0) {
        max_irq++;
        if(max_irq > 7)
          max_irq = 0;
        }
      if (max_irq == highest_priority ) return; /* Highest priority interrupt in-service,
                                                 * no other priorities allowed */
      if (max_irq > 7) BX_PANIC(("error in service_master_pic()"));
      }
    else
      max_irq = highest_priority; /* 0..7 bits in ISR are cleared */
  }


  /* now, see if there are any higher priority requests */
  if ((unmasked_requests = (BX_PIC_THIS s.slave_pic.irr & ~BX_PIC_THIS s.slave_pic.imr)) ) {
    irq = highest_priority;
    do {
      /* for special mode, since we're looking at all IRQ's, skip if
       * current IRQ is already in-service
       */
      if ( BX_PIC_THIS s.slave_pic.special_mask && ((BX_PIC_THIS s.slave_pic.isr >> irq) & 0x01) )
        continue;
      if (unmasked_requests & (1 << irq)) {
        BX_DEBUG(("slave: signalling IRQ(%u)", (unsigned) 8 + irq));

        BX_PIC_THIS s.slave_pic.INT = 1;
        BX_PIC_THIS s.slave_pic.irq = irq;
        raise_irq(2); /* request IRQ 2 on master pic */
        return;
        } /* if (unmasked_requests & ... */

        irq ++;
        if(irq > 7)
          irq = 0;
      } while(irq != max_irq); /* do ... */
    } /* if (unmasked_requests = ... */
}


  /* CPU handshakes with PIC after acknowledging interrupt */
  Bit8u
bx_pic_c::IAC(void)
{
  Bit8u vector;
  Bit8u irq;

  BX_SET_INTR(0);
  BX_PIC_THIS s.master_pic.INT = 0;
  BX_PIC_THIS s.master_pic.irr &= ~(1 << BX_PIC_THIS s.master_pic.irq);
  // In autoeoi mode don't set the isr bit.
  if(!BX_PIC_THIS s.master_pic.auto_eoi)
    BX_PIC_THIS s.master_pic.isr |= (1 << BX_PIC_THIS s.master_pic.irq);
  else if(BX_PIC_THIS s.master_pic.rotate_on_autoeoi)
    BX_PIC_THIS s.master_pic.lowest_priority = BX_PIC_THIS s.master_pic.irq;

  if (BX_PIC_THIS s.master_pic.irq != 2) {
    irq    = BX_PIC_THIS s.master_pic.irq;
    vector = irq + BX_PIC_THIS s.master_pic.interrupt_offset;
    }
  else { /* IRQ2 = slave pic IRQ8..15 */
    BX_PIC_THIS s.slave_pic.INT = 0;
    BX_PIC_THIS s.master_pic.IRQ_line[2] = 0;
    irq    = BX_PIC_THIS s.slave_pic.irq;
    vector = irq + BX_PIC_THIS s.slave_pic.interrupt_offset;
    BX_PIC_THIS s.slave_pic.irr &= ~(1 << BX_PIC_THIS s.slave_pic.irq);
    // In autoeoi mode don't set the isr bit.
    if(!BX_PIC_THIS s.slave_pic.auto_eoi)
      BX_PIC_THIS s.slave_pic.isr |= (1 << BX_PIC_THIS s.slave_pic.irq);
    else if(BX_PIC_THIS s.slave_pic.rotate_on_autoeoi)
      BX_PIC_THIS s.slave_pic.lowest_priority = BX_PIC_THIS s.slave_pic.irq;
    service_slave_pic();
    irq += 8; // for debug printing purposes
    }

  service_master_pic();

  BX_DBG_IAC_REPORT(vector, irq);
  return(vector);
}

  void
bx_pic_c::show_pic_state(void)
{
BX_INFO(("s.master_pic.imr = %02x", BX_PIC_THIS s.master_pic.imr));
BX_INFO(("s.master_pic.isr = %02x", BX_PIC_THIS s.master_pic.isr));
BX_INFO(("s.master_pic.irr = %02x", BX_PIC_THIS s.master_pic.irr));
BX_INFO(("s.master_pic.irq = %02x", BX_PIC_THIS s.master_pic.irq));
}
