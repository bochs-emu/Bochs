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


// Now features proper implementation of keyboard opcodes 0xF4 to 0xF6
// Silently ignores PS/2 keyboard extensions (0xF7 to 0xFD)
// Explicit panic on resend (0xFE)
//
// Emmanuel Marty <core@ggi-project.org>


#include "bochs.h"
#define LOG_THIS  bx_keyboard.


#define VERBOSE_KBD_DEBUG 0


#define MOUSE_MODE_RESET  10
#define MOUSE_MODE_STREAM 11
#define MOUSE_MODE_REMOTE 12
#define MOUSE_MODE_WRAP   13

bx_keyb_c bx_keyboard;

#if BX_USE_KEY_SMF
#define this (&bx_keyboard)
#endif



bx_keyb_c::bx_keyb_c(void)
{
  // constructor
  // should zero out state info here???
  memset( &s, 0, sizeof(s) );
}

bx_keyb_c::~bx_keyb_c(void)
{
  // destructor
  BX_DEBUG(("Exit.\n"));
}


// flush internal buffer and reset keyboard settings to power-up condition
  void
bx_keyb_c::resetinternals(Boolean powerup)
{
  Bit32u   i;

  BX_KEY_THIS s.kbd_internal_buffer.num_elements = 0;
  for (i=0; i<BX_KBD_ELEMENTS; i++)
    BX_KEY_THIS s.kbd_internal_buffer.buffer[i] = 0;
  BX_KEY_THIS s.kbd_internal_buffer.head = 0;

  BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 0;

  if (powerup) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 0;
    BX_KEY_THIS s.kbd_internal_buffer.delay = 1; // 500 mS
    BX_KEY_THIS s.kbd_internal_buffer.repeat_rate = 0x0b; // 10.9 chars/sec
    }
}



  void
bx_keyb_c::init(bx_devices_c *d, bx_cmos_c *cmos)
{
  BX_KEY_THIS setprefix("[KBD ]");
  BX_KEY_THIS settype(KBDLOG);
  Bit32u   i;

  BX_KEY_THIS devices = d;

  BX_KEY_THIS devices->register_irq(1, "8042 Keyboard controller");

  BX_KEY_THIS devices->register_io_read_handler(this, read_handler,
                                      0x0060, "8042 Keyboard controller");
  BX_KEY_THIS devices->register_io_read_handler(this, read_handler,
                                      0x0064, "8042 Keyboard controller");
  BX_KEY_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0060, "8042 Keyboard controller");
  BX_KEY_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0064, "8042 Keyboard controller");

  resetinternals(1);

  BX_KEY_THIS s.kbd_internal_buffer.led_status = 0;
  BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;

  BX_KEY_THIS s.mouse_internal_buffer.num_elements = 0;
  for (i=0; i<BX_MOUSE_BUFF_SIZE; i++)
    BX_KEY_THIS s.mouse_internal_buffer.buffer[i] = 0;
  BX_KEY_THIS s.mouse_internal_buffer.head = 0;

  BX_KEY_THIS s.kbd_controller.pare = 0;
  BX_KEY_THIS s.kbd_controller.tim  = 0;
  BX_KEY_THIS s.kbd_controller.auxb = 0;
  BX_KEY_THIS s.kbd_controller.keyl = 1;
  BX_KEY_THIS s.kbd_controller.c_d  = 1;
  BX_KEY_THIS s.kbd_controller.sysf = 0;
  BX_KEY_THIS s.kbd_controller.inpb = 0;
  BX_KEY_THIS s.kbd_controller.outb = 0;

  BX_KEY_THIS s.kbd_controller.scan_convert = 1;
  BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 1;
  BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 0;
  BX_KEY_THIS s.kbd_controller.allow_irq1 = 1;
  BX_KEY_THIS s.kbd_controller.allow_irq12 = 1;
  BX_KEY_THIS s.kbd_controller.kbd_output_buffer = 0;
  BX_KEY_THIS s.kbd_controller.aux_output_buffer = 0;
  BX_KEY_THIS s.kbd_controller.last_comm = 0;
  BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
  BX_KEY_THIS s.kbd_controller.irq1_requested = 0;
  BX_KEY_THIS s.kbd_controller.irq12_requested = 0;

//BX_DEBUG(( "# keyboard_serial_delay is %u usec\n",
//        (unsigned) bx_options.keyboard_serial_delay));
  BX_KEY_THIS s.kbd_controller.timer_pending = 0;

  // Mouse initialization stuff
  BX_KEY_THIS s.mouse.sample_rate     = 100; // reports per second
  BX_KEY_THIS s.mouse.resolution_cpmm = 4;   // 4 counts per millimeter
  BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
  BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_RESET;
  BX_KEY_THIS s.mouse.enable          = 0;

  for (i=0; i<BX_KBD_CONTROLLER_QSIZE; i++)
    BX_KEY_THIS s.controller_Q[i] = 0;
  BX_KEY_THIS s.controller_Qsize = 0;
  BX_KEY_THIS s.controller_Qsource = 0;

  // mouse port installed on system board
  cmos->s.reg[0x14] |= 0x04;
  BX_DEBUG(("Init.\n"));
}

#define RETURN(x) do { ret = (x); goto read_return; } while (0)

  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_keyb_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_KEY_SMF
  bx_keyb_c *class_ptr = (bx_keyb_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_keyb_c::read(Bit32u   address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_KEY_SMF

  Bit32u ret = 0;

  if (io_len > 1)
    BX_PANIC(("kbd: io read to address %08x, len=%u\n",
             (unsigned) address, (unsigned) io_len));


//BX_DEBUG(( "read from port 0x%04x\n", (unsigned) address));

  if (address == 0x60) { /* output buffer */
    Bit8u   val;
    if (BX_KEY_THIS s.kbd_controller.auxb) { /* mouse byte available */
      val = BX_KEY_THIS s.kbd_controller.aux_output_buffer;
      BX_KEY_THIS s.kbd_controller.aux_output_buffer = 0;
      BX_KEY_THIS s.kbd_controller.outb = 0;
      BX_KEY_THIS s.kbd_controller.auxb = 0;

      if (BX_KEY_THIS s.controller_Qsize) {
        unsigned i;
        BX_KEY_THIS s.kbd_controller.aux_output_buffer = BX_KEY_THIS s.controller_Q[0];
        BX_KEY_THIS s.kbd_controller.outb = 1;
        BX_KEY_THIS s.kbd_controller.auxb = 1;
        if (BX_KEY_THIS s.kbd_controller.allow_irq12)
          BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
        for (i=0; i<BX_KEY_THIS s.controller_Qsize-1; i++) {
          // move Q elements towards head of queue by one
          BX_KEY_THIS s.controller_Q[i] = BX_KEY_THIS s.controller_Q[i+1];
          }
        BX_KEY_THIS s.controller_Qsize--;
        }

//BX_INFO(("mouse: ___io_read aux = 0x%02x\n", (unsigned) val));

      activate_timer();
      if (bx_dbg.keyboard)
        BX_INFO(("READ(%02x) = %02x\n", (unsigned) address,
          (unsigned) val));
      RETURN(val);
      }
    else if (BX_KEY_THIS s.kbd_controller.outb) { /* kbd byte available */
      val = BX_KEY_THIS s.kbd_controller.kbd_output_buffer;
      BX_KEY_THIS s.kbd_controller.kbd_output_buffer = 0;
      BX_KEY_THIS s.kbd_controller.outb = 0;
      BX_KEY_THIS s.kbd_controller.auxb = 0;
//BX_DEBUG(( "___io_read kbd\n"));

      if (BX_KEY_THIS s.controller_Qsize) {
        unsigned i;
        BX_KEY_THIS s.kbd_controller.aux_output_buffer = BX_KEY_THIS s.controller_Q[0];
        BX_KEY_THIS s.kbd_controller.outb = 1;
        BX_KEY_THIS s.kbd_controller.auxb = 1;
        if (BX_KEY_THIS s.kbd_controller.allow_irq1)
          BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
        for (i=0; i<BX_KEY_THIS s.controller_Qsize-1; i++) {
          // move Q elements towards head of queue by one
          BX_KEY_THIS s.controller_Q[i] = BX_KEY_THIS s.controller_Q[i+1];
          }
        BX_KEY_THIS s.controller_Qsize--;
        }

      activate_timer();
      if (bx_dbg.keyboard)
        BX_INFO(("READ(%02x) = %02x\n", (unsigned) address,
          (unsigned) val));
      RETURN(val);
      }
    else {
      if (bx_dbg.keyboard) {
        BX_INFO(("num_elements = %d\n", BX_KEY_THIS s.kbd_internal_buffer.num_elements));
        BX_INFO(("read from port 60h with outb empty\n"));
        }
      val = 0;
      RETURN(val);
      }
    }

#if BX_CPU_LEVEL >= 2
  else if (address == 0x64) { /* status register */
    Bit8u   val;

    val = (BX_KEY_THIS s.kbd_controller.pare << 7)  |
          (BX_KEY_THIS s.kbd_controller.tim  << 6)  |
          (BX_KEY_THIS s.kbd_controller.auxb << 5)  |
          (BX_KEY_THIS s.kbd_controller.keyl << 4)  |
          (BX_KEY_THIS s.kbd_controller.c_d  << 3)  |
          (BX_KEY_THIS s.kbd_controller.sysf << 2)  |
          (BX_KEY_THIS s.kbd_controller.inpb << 1)  |
          BX_KEY_THIS s.kbd_controller.outb;
    RETURN(val);
    }

#else /* BX_CPU_LEVEL > 0 */
  /* XT MODE, System 8255 Mode Register */
  else if (address == 0x64) { /* status register */
    if (bx_dbg.keyboard)
      BX_INFO(("IO read from port 64h, system 8255 mode register\n"));
    RETURN(BX_KEY_THIS s.kbd_controller.outb);
    }
#endif /* BX_CPU_LEVEL > 0 */

  else {
    BX_PANIC(("unknown address in io read to keyboard port %x\n",
      (unsigned) address));
    RETURN(0); /* keep compiler happy */
    }

  read_return:
  if (bx_dbg.keyboard)
	BX_INFO(("keyboard: 8-bit read from %04x = %02x\n", (unsigned)address, ret));
  return ret;
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_keyb_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_KEY_SMF
  bx_keyb_c *class_ptr = (bx_keyb_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_keyb_c::write( Bit32u   address, Bit32u   value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_KEY_SMF
  Bit8u   command_byte;

  if (io_len > 1)
    BX_PANIC(("kbd: io write to address %08x, len=%u\n",
             (unsigned) address, (unsigned) io_len));

  if (bx_dbg.keyboard)
	BX_INFO(("keyboard: 8-bit write to %04x = %02x\n", (unsigned)address, (unsigned)value));


//BX_DEBUG(("WRITE(%02x) = %02x\n", (unsigned) address,
//      (unsigned) value));

  switch (address) {
    case 0x60: // input buffer
      // if expecting data byte from command last sent to port 64h
      if (BX_KEY_THIS s.kbd_controller.expecting_port60h) {
        BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
        // data byte written last to 0x60
        BX_KEY_THIS s.kbd_controller.c_d = 0;
        if (BX_KEY_THIS s.kbd_controller.inpb) {
          BX_PANIC(("write to port 60h, not ready for write\n"));
          }
        switch (BX_KEY_THIS s.kbd_controller.last_comm) {
          case 0x60: // write command byte
            {
            Boolean scan_convert, disable_keyboard,
                    disable_aux;

            scan_convert = (value >> 6) & 0x01;
            disable_aux      = (value >> 5) & 0x01;
            disable_keyboard = (value >> 4) & 0x01;
            BX_KEY_THIS s.kbd_controller.sysf = (value >> 2) & 0x01;
            BX_KEY_THIS s.kbd_controller.allow_irq1  = (value >> 0) & 0x01;
            BX_KEY_THIS s.kbd_controller.allow_irq12 = (value >> 1) & 0x01;
            set_kbd_clock_enable(!disable_keyboard);
            set_aux_clock_enable(!disable_aux);
            if (BX_KEY_THIS s.kbd_controller.allow_irq12 && BX_KEY_THIS s.kbd_controller.auxb)
              BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
            else if (BX_KEY_THIS s.kbd_controller.allow_irq1  && BX_KEY_THIS s.kbd_controller.outb)
              BX_KEY_THIS s.kbd_controller.irq1_requested = 1;

			BX_DEBUG(( " allow_irq12 set to %u\n", (unsigned)
				BX_KEY_THIS s.kbd_controller.allow_irq12));
            if ( !scan_convert )
              BX_INFO(("keyboard: (mch) scan convert turned off\n"));

	    // (mch) NT needs this
	    BX_KEY_THIS s.kbd_controller.scan_convert = scan_convert;
            }
            break;
          case 0xd1: // write output port
            if (bx_dbg.keyboard)
              BX_INFO(("write output port with value %02xh\n",
                (unsigned) value));
            BX_SET_ENABLE_A20( (value & 0x02) != 0 );
            if (!(value & 0x01)) BX_PANIC(("IO write: processor reset requested!\n"));
            break;
          case 0xd4: // Write to mouse
            // I don't think this enables the AUX clock
            //set_aux_clock_enable(1); // enable aux clock line
            kbd_ctrl_to_mouse(value);
            // ??? should I reset to previous value of aux enable?
            break;

          case 0xd3: // write mouse output buffer
            // Queue in mouse output buffer
            controller_enQ(value, 1);
            break;

          default:
            BX_PANIC(("=== unsupported write to port 60h(lastcomm=%02x): %02x\n",
              (unsigned) BX_KEY_THIS s.kbd_controller.last_comm, (unsigned) value));
          }
        }
      else {
        // data byte written last to 0x60
        BX_KEY_THIS s.kbd_controller.c_d = 0;
        BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;
        /* pass byte to keyboard */
        /* ??? should conditionally pass to mouse device here ??? */
        if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled==0) {
          BX_INFO(("keyboard disabled & send of byte %02x to kbd\n",
            (unsigned) value));
          }
        kbd_ctrl_to_kbd(value);
        }
      break;

    case 0x64: // control register
      // command byte written last to 0x64
      BX_KEY_THIS s.kbd_controller.c_d = 1;
      BX_KEY_THIS s.kbd_controller.last_comm = value;
      // most commands NOT expecting port60 write next
      BX_KEY_THIS s.kbd_controller.expecting_port60h = 0;

      switch (value) {
        case 0x20: // get keyboard command byte
          if (bx_dbg.keyboard)
            BX_INFO(("get keyboard command byte\n"));
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
          command_byte =
            (BX_KEY_THIS s.kbd_controller.scan_convert << 6) |
            ((!BX_KEY_THIS s.kbd_controller.aux_clock_enabled) << 5) |
            ((!BX_KEY_THIS s.kbd_controller.kbd_clock_enabled) << 4) |
            (0 << 3) |
            (BX_KEY_THIS s.kbd_controller.sysf << 2) |
            (BX_KEY_THIS s.kbd_controller.allow_irq12 << 1) |
            (BX_KEY_THIS s.kbd_controller.allow_irq1  << 0);
          controller_enQ(command_byte, 0);
          break;
        case 0x60: // write command byte
          if (bx_dbg.keyboard)
            BX_INFO(("write command byte\n"));
          // following byte written to port 60h is command byte
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xa1:
          BX_INFO(("Dummy out Green PC for now : 0xa1\n"));
          break;

        case 0xa7: // disable the aux device
          set_aux_clock_enable(0);
          if (bx_dbg.keyboard) BX_INFO(("aux device disabled\n"));
          break;
        case 0xa8: // enable the aux device
          set_aux_clock_enable(1);
          if (bx_dbg.keyboard) BX_INFO(("aux device enabled\n"));
          break;
        case 0xa9: // Test Mouse Port
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
          controller_enQ(0x00, 0); // no errors detected
          break;
        case 0xaa: // motherboard controller self test
          if (bx_dbg.keyboard) BX_INFO(("Self Test\n"));
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
	  // (mch) Why is this commented out??? Enabling
          BX_KEY_THIS s.kbd_controller.sysf = 1; // self test complete
          controller_enQ(0x55, 0); // controller OK
          break;
        case 0xab: // Interface Test
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
          controller_enQ(0x00, 0);
          break;
        case 0xad: // disable keyboard
          set_kbd_clock_enable(0);
          if (bx_dbg.keyboard) BX_INFO(("keyboard disabled\n"));
          break;
        case 0xae: // enable keyboard
          set_kbd_clock_enable(1);
          if (bx_dbg.keyboard) BX_INFO(("keyboard enabled\n"));
          break;
        case 0xc0: // read input port
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
          // keyboard power normal
          controller_enQ(0x00, 0);
          break;
        case 0xd0: // read output port: next byte read from port 60h
          if (bx_dbg.keyboard)
            BX_INFO(("io write to port 64h, command d0h (partial)\n"));
          // controller output buffer must be empty
          if (BX_KEY_THIS s.kbd_controller.outb) {
BX_PANIC(("kbd: OUTB set and command 0x%02x encountered\n", value));
            break;
            }
          controller_enQ(
              (BX_KEY_THIS s.kbd_controller.auxb << 5) |
              (BX_KEY_THIS s.kbd_controller.outb << 4) |
              (BX_GET_ENABLE_A20() << 1) |
              0x01, 0);
          break;

        case 0xd1: // write output port: next byte written to port 60h
          if (bx_dbg.keyboard)
            BX_INFO(("write output port\n"));
          // following byte to port 60h written to output port
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd3: // write mouse output buffer
	  //FIXME: Why was this a panic?
          BX_INFO(("io write 0x64: command = 0xD3(write mouse outb)\n"));
	  // following byte to port 60h written to output port as mouse write.
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd4: // write to mouse
          if (bx_dbg.mouse)
            BX_INFO(("io write 0x64: command = 0xD4 (write to mouse)\n"));
          // following byte written to port 60h
          BX_KEY_THIS s.kbd_controller.expecting_port60h = 1;
          break;

        case 0xd2: // write keyboard output buffer
        case 0xdd: // Disable A20 Address Line
        case 0xdf: // Enable A20 Address Line
        case 0xc1: // Continuous Input Port Poll, Low
        case 0xc2: // Continuous Input Port Poll, High
        case 0xe0: // Read Test Inputs
          BX_PANIC(("io write 0x64: command = %02xh\n", (unsigned) value));
          break;

        case 0xfe: // System Reset, transition to real mode
          BX_INFO(("system reset\n"));
          bx_pc_system.ResetSignal( PCS_SET ); /* XXX is this right? */
	  {
	  for (int i=0; i<BX_SMP_PROCESSORS; i++) 
            BX_CPU[i]->reset(BX_RESET_HARDWARE);
	  }
          // Use bx_pc_system if necessary bx_cpu.reset_cpu();
          // bx_pc_system.ResetSignal( PCS_SET );
          break;

        default:
          if (value==0xff || (value>=0xf0 && value<=0xfd)) {
            /* useless pulse output bit commands ??? */
            if (bx_dbg.keyboard)
              BX_INFO(("io write to port 64h, useless command %02x\n",
                (unsigned) value));
            return;
	    }
          BX_PANIC(("unsupported io write to keyboard port %x, value = %x\n",
            (unsigned) address, (unsigned) value));
          break;
        }
      break;

    default: BX_PANIC(("unknown address in bx_keyb_c::write()\n"));
    }
}


  void
bx_keyb_c::gen_scancode(Bit32u   key)
{
  Bit8u   scancode;

  BX_DEBUG(( "gen_scancode %lld %x\n", bx_pc_system.time_ticks(), key));

  if (!BX_KEY_THIS s.kbd_controller.scan_convert)
	BX_PANIC(("keyboard: gen_scancode with scan_convert cleared\n"));

  if (bx_dbg.keyboard)
    BX_INFO(("gen_scancode(): scancode: %08x\n", (unsigned) key));

  // Ignore scancode if keyboard clock is driven low
  if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled==0)
    return;

  // Ignore scancode if scanning is disabled
  if (BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled==0)
    return;

  // should deal with conversions from KSCAN to system scan codes here

  switch (key & 0xff) {
    case BX_KEY_CTRL_L:  scancode = 0x1d; break;
    case BX_KEY_CTRL_R:  scancode = 0x1d; break;
    case BX_KEY_SHIFT_L: scancode = 0x2a; break;
    case BX_KEY_SHIFT_R: scancode = 0x36; break;
    case BX_KEY_ESC:   scancode = 0x01; break;

    case BX_KEY_ALT_L: scancode = 0x38; break;

    case BX_KEY_A:     scancode = 0x1e; break;
    case BX_KEY_B:     scancode = 0x30; break;
    case BX_KEY_C:     scancode = 0x2e; break;
    case BX_KEY_D:     scancode = 0x20; break;
    case BX_KEY_E:     scancode = 0x12; break;
    case BX_KEY_F:     scancode = 0x21; break;
    case BX_KEY_G:     scancode = 0x22; break;
    case BX_KEY_H:     scancode = 0x23; break;
    case BX_KEY_I:     scancode = 0x17; break;
    case BX_KEY_J:     scancode = 0x24; break;
    case BX_KEY_K:     scancode = 0x25; break;
    case BX_KEY_L:     scancode = 0x26; break;
    case BX_KEY_M:     scancode = 0x32; break;
    case BX_KEY_N:     scancode = 0x31; break;
    case BX_KEY_O:     scancode = 0x18; break;
    case BX_KEY_P:     scancode = 0x19; break;
    case BX_KEY_Q:     scancode = 0x10; break;
    case BX_KEY_R:     scancode = 0x13; break;
    case BX_KEY_S:     scancode = 0x1f; break;
    case BX_KEY_T:     scancode = 0x14; break;
    case BX_KEY_U:     scancode = 0x16; break;
    case BX_KEY_V:     scancode = 0x2f; break;
    case BX_KEY_W:     scancode = 0x11; break;
    case BX_KEY_X:     scancode = 0x2d; break;
    case BX_KEY_Y:     scancode = 0x15; break;
    case BX_KEY_Z:     scancode = 0x2c; break;

    case BX_KEY_0:     scancode = 0x0b; break;
    case BX_KEY_1:     scancode = 0x02; break;
    case BX_KEY_2:     scancode = 0x03; break;
    case BX_KEY_3:     scancode = 0x04; break;
    case BX_KEY_4:     scancode = 0x05; break;
    case BX_KEY_5:     scancode = 0x06; break;
    case BX_KEY_6:     scancode = 0x07; break;
    case BX_KEY_7:     scancode = 0x08; break;
    case BX_KEY_8:     scancode = 0x09; break;
    case BX_KEY_9:     scancode = 0x0a; break;

    case BX_KEY_SPACE:        scancode = 0x39; break;
    case BX_KEY_SINGLE_QUOTE: scancode = 0x28; break;
    case BX_KEY_COMMA:        scancode = 0x33; break;
    case BX_KEY_PERIOD:       scancode = 0x34; break;
    case BX_KEY_SLASH:        scancode = 0x35; break;

    case BX_KEY_SEMICOLON:     scancode = 0x27; break;
    case BX_KEY_EQUALS:        scancode = 0x0d; break;

    case BX_KEY_LEFT_BRACKET:  scancode = 0x1a; break;
    case BX_KEY_BACKSLASH:     scancode = 0x2b; break;
    case BX_KEY_RIGHT_BRACKET: scancode = 0x1b; break;
    case BX_KEY_MINUS:         scancode = 0x0c; break;
    case BX_KEY_GRAVE:         scancode = 0x29; break;

    case BX_KEY_BACKSPACE:     scancode = 0x0e; break;
    case BX_KEY_KP_ENTER:
    case BX_KEY_ENTER:         scancode = 0x1c; break;
    case BX_KEY_TAB:           scancode = 0x0f; break;

    case BX_KEY_LEFT:
    case BX_KEY_KP_LEFT:       scancode = 0x4b; break;
    case BX_KEY_RIGHT:
    case BX_KEY_KP_RIGHT:      scancode = 0x4d; break;
    case BX_KEY_UP:
    case BX_KEY_KP_UP:         scancode = 0x48; break;
    case BX_KEY_DOWN:
    case BX_KEY_KP_DOWN:       scancode = 0x50; break;

    case BX_KEY_INSERT:
    case BX_KEY_KP_INSERT:        scancode = 0x52; break;
    case BX_KEY_DELETE:
    case BX_KEY_KP_DELETE:        scancode = 0x53; break;
    case BX_KEY_HOME:
    case BX_KEY_KP_HOME:          scancode = 0x47; break;
    case BX_KEY_END:
    case BX_KEY_KP_END:           scancode = 0x4f; break;
    case BX_KEY_PAGE_UP:
    case BX_KEY_KP_PAGE_UP:       scancode = 0x49; break;
    case BX_KEY_PAGE_DOWN:
    case BX_KEY_KP_PAGE_DOWN:     scancode = 0x51; break;

    case BX_KEY_KP_ADD:           scancode = 0x4e; break;
    case BX_KEY_KP_SUBTRACT:      scancode = 0x4a; break;
    case BX_KEY_KP_5:             scancode = 0x4c; break;
    case BX_KEY_KP_MULTIPLY:
      BX_DEBUG(( "Grey Multiply key not on 83-key keyboard\n" ));
      return;
    case BX_KEY_KP_DIVIDE:
      BX_DEBUG(( "Grey Divide key not on 83-key keyboard\n" ));
      return;
    case BX_KEY_NUM_LOCK:         scancode = 0x45; break;

    case BX_KEY_F1:               scancode = 0x3b; break;
    case BX_KEY_F2:               scancode = 0x3c; break;
    case BX_KEY_F3:               scancode = 0x3d; break;
    case BX_KEY_F4:               scancode = 0x3e; break;
    case BX_KEY_F5:               scancode = 0x3f; break;
    case BX_KEY_F6:               scancode = 0x40; break;
    case BX_KEY_F7:               scancode = 0x41; break;
    case BX_KEY_F8:               scancode = 0x42; break;
    case BX_KEY_F9:               scancode = 0x43; break;
    case BX_KEY_F10:              scancode = 0x44; break;

    default:
      BX_DEBUG(( "bx_keyb_c::gen_scancode : Unhandled %u\n",
        (unsigned) key));
      return;
    }
  if (key & BX_KEY_RELEASED)
    scancode |= 0x80;
  kbd_enQ(scancode);
}



  void
bx_keyb_c::set_kbd_clock_enable(Bit8u   value)
{
  Boolean prev_kbd_clock_enabled;

  if (value==0) {
    BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 0;
    }
  else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_kbd_clock_enabled = BX_KEY_THIS s.kbd_controller.kbd_clock_enabled;
    BX_KEY_THIS s.kbd_controller.kbd_clock_enabled = 1;

    if (prev_kbd_clock_enabled==0 && BX_KEY_THIS s.kbd_controller.outb==0) {
      activate_timer();
      }
    }
}



  void
bx_keyb_c::set_aux_clock_enable(Bit8u   value)
{
  Boolean prev_aux_clock_enabled;

  if (bx_dbg.keyboard)
	BX_INFO(("set_aux_clock_enable(%u)\n", (unsigned) value));
  if (value==0) {
    BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 0;
    }
  else {
    /* is another byte waiting to be sent from the keyboard ? */
    prev_aux_clock_enabled = BX_KEY_THIS s.kbd_controller.aux_clock_enabled;
    BX_KEY_THIS s.kbd_controller.aux_clock_enabled = 1;
    if (prev_aux_clock_enabled==0 && BX_KEY_THIS s.kbd_controller.outb==0)
      activate_timer();
    }
}

  Bit8u
bx_keyb_c::get_kbd_enable(void)
{
#ifdef BX_DEBUG
  if (bx_dbg.keyboard)
    BX_INFO(("get_kbd_enable(): getting kbd_clock_enabled of: %02x\n",
      (unsigned) BX_KEY_THIS s.kbd_controller.kbd_clock_enabled));
#endif

  return(BX_KEY_THIS s.kbd_controller.kbd_clock_enabled);
}

  void
bx_keyb_c::controller_enQ(Bit8u   data, unsigned source)
{
  // source is 0 for keyboard, 1 for mouse

  if (bx_dbg.keyboard)
    BX_INFO(("controller_enQ(%02x)\n", (unsigned) data));

  if (BX_KEY_THIS s.kbd_controller.outb)
    BX_INFO(("controller_enQ(): OUTB set!\n"));

  // see if we need to Q this byte from the controller
  if (BX_KEY_THIS s.kbd_controller.outb) {
    if (BX_KEY_THIS s.controller_Qsize >= BX_KBD_CONTROLLER_QSIZE)
      BX_PANIC(("controller_enq(): controller_Q full!\n"));
    BX_KEY_THIS s.controller_Q[BX_KEY_THIS s.controller_Qsize++] = data;
    BX_KEY_THIS s.controller_Qsource = source;
    return;
    }

  if (source == 0) { // keyboard
    BX_KEY_THIS s.kbd_controller.kbd_output_buffer = data;
    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 0;
    BX_KEY_THIS s.kbd_controller.inpb = 0;
    if (BX_KEY_THIS s.kbd_controller.allow_irq1)
      BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
    }
  else { // mouse
    BX_KEY_THIS s.kbd_controller.aux_output_buffer = data;
    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 1;
    BX_KEY_THIS s.kbd_controller.inpb = 0;
    if (BX_KEY_THIS s.kbd_controller.allow_irq12)
      BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
    }
}

void
bx_keyb_c::kbd_enQ_imm(Bit8u val)
{
      int tail;

      if (BX_KEY_THIS s.kbd_internal_buffer.num_elements >= BX_KBD_ELEMENTS) {
	    BX_PANIC(("internal keyboard buffer full (imm)\n"));
	    return;
      }

      /* enqueue scancode in multibyte internal keyboard buffer */
      tail = (BX_KEY_THIS s.kbd_internal_buffer.head + BX_KEY_THIS s.kbd_internal_buffer.num_elements) %
	    BX_KBD_ELEMENTS;

      BX_KEY_THIS s.kbd_controller.kbd_output_buffer = val;
      BX_KEY_THIS s.kbd_controller.outb = 1;

      if (BX_KEY_THIS s.kbd_controller.allow_irq1)
	    BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
}


  void
bx_keyb_c::kbd_enQ(Bit8u   scancode)
{
  int tail;

  if (bx_dbg.keyboard)
    BX_INFO(("enQ(%02x)\n", (unsigned) scancode));

  if (BX_KEY_THIS s.kbd_internal_buffer.num_elements >= BX_KBD_ELEMENTS) {
    BX_INFO(("internal keyboard buffer full, ignoring scancode.(%02x)\n",
      (unsigned) scancode));
    return;
    }

  /* enqueue scancode in multibyte internal keyboard buffer */
  if (bx_dbg.keyboard)
    BX_INFO(("enQ: putting scancode %02x in internal buffer\n",
      (unsigned) scancode));
  tail = (BX_KEY_THIS s.kbd_internal_buffer.head + BX_KEY_THIS s.kbd_internal_buffer.num_elements) %
   BX_KBD_ELEMENTS;
  BX_KEY_THIS s.kbd_internal_buffer.buffer[tail] = scancode;
  BX_KEY_THIS s.kbd_internal_buffer.num_elements++;

  if (!BX_KEY_THIS s.kbd_controller.outb && BX_KEY_THIS s.kbd_controller.kbd_clock_enabled) {
    activate_timer();
	BX_DEBUG(("activating timer...\n"));
    return;
    }
//BX_DEBUG(( "# not activating timer...\n");
//BX_DEBUG(( "#   allow_irq1 = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq1);
//BX_DEBUG(( "#   outb       = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.outb);
//BX_DEBUG(( "#   clock_enab = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.kbd_clock_enabled);
//BX_DEBUG(( "#   out_buffer = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.kbd_output_buffer);
}

  Boolean
bx_keyb_c::mouse_enQ_packet(Bit8u   b1, Bit8u   b2, Bit8u   b3)
{
  if ((BX_KEY_THIS s.mouse_internal_buffer.num_elements + 3) >= BX_MOUSE_BUFF_SIZE) {
    return(0); /* buffer doesn't have the space */
    }

//BX_INFO(("mouse: enQ_packet(%02x, %02x, %02x)\n",
//  (unsigned) b1, (unsigned) b2, (unsigned) b3));

  mouse_enQ(b1);
  mouse_enQ(b2);
  mouse_enQ(b3);
  return(1);
}


  void
bx_keyb_c::mouse_enQ(Bit8u   mouse_data)
{
  int tail;

  if (bx_dbg.mouse)
    BX_INFO(("mouse_enQ(%02x)\n", (unsigned) mouse_data));

  if (BX_KEY_THIS s.mouse_internal_buffer.num_elements >= BX_MOUSE_BUFF_SIZE) {
    BX_INFO(("mouse: internal mouse buffer full, ignoring mouse data.(%02x)\n",
      (unsigned) mouse_data));
    return;
    }
//BX_DEBUG(( "# mouse_enq() aux_clock_enabled = %u\n",
//  (unsigned) BX_KEY_THIS s.kbd_controller.aux_clock_enabled);

  /* enqueue mouse data in multibyte internal mouse buffer */
  tail = (BX_KEY_THIS s.mouse_internal_buffer.head + BX_KEY_THIS s.mouse_internal_buffer.num_elements) %
   BX_MOUSE_BUFF_SIZE;
  BX_KEY_THIS s.mouse_internal_buffer.buffer[tail] = mouse_data;
  BX_KEY_THIS s.mouse_internal_buffer.num_elements++;

  if (!BX_KEY_THIS s.kbd_controller.outb && BX_KEY_THIS s.kbd_controller.aux_clock_enabled) {
    activate_timer();
//BX_DEBUG(( "# activating timer...\n");
    return;
    }
//BX_DEBUG(( "# not activating timer...\n");
//BX_DEBUG(( "#   allow_irq12= %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq12);
//BX_DEBUG(( "#   outb       = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.outb);
//BX_DEBUG(( "#   clock_enab = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.aux_clock_enabled);
//BX_DEBUG(( "#   out_buffer = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.aux_output_buffer);
}

  void
bx_keyb_c::kbd_ctrl_to_kbd(Bit8u   value)
{
  if (bx_dbg.keyboard)
	BX_INFO(("controller passed byte %02xh to keyboard\n"));

  if (BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 0;
    BX_KEY_THIS s.kbd_internal_buffer.delay = (value >> 5) & 0x03;
    switch (BX_KEY_THIS s.kbd_internal_buffer.delay) {
      case 0: BX_INFO(("setting delay to 250 mS\n")); break;
      case 1: BX_INFO(("setting delay to 250 mS\n")); break;
      case 2: BX_INFO(("setting delay to 250 mS\n")); break;
      case 3: BX_INFO(("setting delay to 250 mS\n")); break;
      }
    BX_KEY_THIS s.kbd_internal_buffer.repeat_rate = value & 0x1f;
    BX_INFO(("setting repeat rate to %u\n", (unsigned) value));
    kbd_enQ(0xFA); // send ACK
    return;
    }

  if (BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write) {
    BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 0;
    BX_KEY_THIS s.kbd_internal_buffer.led_status = value;
    BX_INFO(("LED status set to %02x\n",
      (unsigned) BX_KEY_THIS s.kbd_internal_buffer.led_status));
    kbd_enQ(0xFA); // send ACK %%%
    return;
    }

  switch (value) {
    case 0x00: // ??? ignore and let OS timeout with no response
      kbd_enQ(0xFA); // send ACK %%%
      return;
      break;

    case 0x05: // ???
	  // (mch) trying to get this to work...
          BX_KEY_THIS s.kbd_controller.sysf = 1;
	  kbd_enQ_imm(0xfe);
      return;
      break;

    case 0xed: // LED Write
      BX_KEY_THIS s.kbd_internal_buffer.expecting_led_write = 1;
      kbd_enQ(0xFA); // send ACK %%%
      return;
      break;

    case 0xee: // echo
      kbd_enQ(0xEE); // return same byte (EEh) as echo diagnostic
      return;
      break;

    case 0xf2:  // identify keyboard
      BX_INFO(("identify keyboard command received\n"));
      kbd_enQ(0xFA); // AT sends ACK, MFII sends ACK+ABh+41h
      return;
      break;

    case 0xf3:  // typematic info
      BX_KEY_THIS s.kbd_internal_buffer.expecting_typematic = 1;
      BX_INFO(("setting typematic info\n"));
      kbd_enQ(0xFA); // send ACK
      return;
      break;

    case 0xf4:  // flush scancodes buffer and modes, then enable keyboard
      resetinternals(0);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;
      return;
      break;

    case 0xf5:  // reset keyboard to power-up settings and disable scanning
      resetinternals(1);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 0;
      BX_INFO(("reset-disable command received\n"));
      return;
      break;

    case 0xf6:  // reset keyboard to power-up settings and enable scanning
      resetinternals(1);
      kbd_enQ(0xFA); // send ACK
      BX_KEY_THIS s.kbd_internal_buffer.scanning_enabled = 1;
      BX_INFO(("reset-enable command received\n"));
      return;
      break;

    case 0xf7:  // PS/2 Set All Keys To Typematic
    case 0xf8:  // PS/2 Set All Keys to Make/Break
    case 0xf9:  // PS/2 PS/2 Set All Keys to Make
    case 0xfa:  // PS/2 Set All Keys to Typematic Make/Break
    case 0xfb:  // PS/2 Set Key Type to Typematic
    case 0xfc:  // PS/2 Set Key Type to Make/Break
    case 0xfd:  // PS/2 Set Key Type to Make
      // Silently ignore and let the OS timeout, for now.
      // If anyone has code around that makes use of that, I can
      // provide documentation on their behavior (ask core@ggi-project.org)
      return;
      break;

    case 0xfe:  // resend. aiiee.
      BX_PANIC( ("got 0xFE (resend)\n"));
      return;
      break;

    case 0xff:  // reset: internal keyboard reset and afterwards the BAT
      BX_INFO(("rest command received\n"));
      kbd_enQ(0xFA); // send ACK
      kbd_enQ(0xAA); // BAT test passed
      return;
      break;

case 0xd3:
  kbd_enQ(0xfa);
  return;

    default:
			/* XXX fix this properly:
			http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/mouse/mouse.html
			http://sourceforge.net/tracker/index.php?func=detail&aid=422457&group_id=12580&atid=112580
			 */
      BX_INFO(("kbd_ctrl_to_kbd(): got value of %02x\n",
        (unsigned) value));
      kbd_enQ(0xFA); /* send ACK ??? */
      return;
      break;
    }
}

  unsigned
bx_keyb_c::periodic( Bit32u   usec_delta )
{
  static int multiple=0;
  Bit8u   retval;

  UNUSED( usec_delta );

  if ( ++multiple==10)
  {
    multiple=0;
    bx_gui.handle_events();
  }
  retval = BX_KEY_THIS s.kbd_controller.irq1_requested | (BX_KEY_THIS s.kbd_controller.irq12_requested << 1);
  BX_KEY_THIS s.kbd_controller.irq1_requested = 0;
  BX_KEY_THIS s.kbd_controller.irq12_requested = 0;

  if ( BX_KEY_THIS s.kbd_controller.timer_pending == 0 ) {
    return(retval);
    }

  if ( usec_delta >= BX_KEY_THIS s.kbd_controller.timer_pending ) {
    BX_KEY_THIS s.kbd_controller.timer_pending = 0;
    }
  else {
    BX_KEY_THIS s.kbd_controller.timer_pending -= usec_delta;
    return(retval);
    }

  if (BX_KEY_THIS s.kbd_controller.outb) {
    return(retval);
    }

  /* nothing in outb, look for possible data xfer from keyboard or mouse */
  if (BX_KEY_THIS s.kbd_controller.kbd_clock_enabled && BX_KEY_THIS s.kbd_internal_buffer.num_elements) {
//BX_DEBUG(( "#   servicing keyboard code\n");
    if (bx_dbg.keyboard)
      BX_INFO(("service_keyboard: key in internal buffer waiting\n"));
    BX_KEY_THIS s.kbd_controller.kbd_output_buffer =
      BX_KEY_THIS s.kbd_internal_buffer.buffer[BX_KEY_THIS s.kbd_internal_buffer.head];
    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 0;
//BX_DEBUG(( "# ___kbd::periodic kbd\n");
    BX_KEY_THIS s.kbd_internal_buffer.head = (BX_KEY_THIS s.kbd_internal_buffer.head + 1) %
      BX_KBD_ELEMENTS;
    BX_KEY_THIS s.kbd_internal_buffer.num_elements--;
    if (BX_KEY_THIS s.kbd_controller.allow_irq1)
      BX_KEY_THIS s.kbd_controller.irq1_requested = 1;
    }
  else if (BX_KEY_THIS s.kbd_controller.aux_clock_enabled && BX_KEY_THIS s.mouse_internal_buffer.num_elements) {
//BX_DEBUG(( "#   servicing mouse code\n");
    if (bx_dbg.mouse)
      BX_INFO(("service_keyboard: key in internal buffer waiting\n"));
    BX_KEY_THIS s.kbd_controller.aux_output_buffer =
      BX_KEY_THIS s.mouse_internal_buffer.buffer[BX_KEY_THIS s.mouse_internal_buffer.head];

    BX_KEY_THIS s.kbd_controller.outb = 1;
    BX_KEY_THIS s.kbd_controller.auxb = 1;
//BX_DEBUG(( "# ___kbd:periodic aux\n");
    BX_KEY_THIS s.mouse_internal_buffer.head = (BX_KEY_THIS s.mouse_internal_buffer.head + 1) %
      BX_MOUSE_BUFF_SIZE;
    BX_KEY_THIS s.mouse_internal_buffer.num_elements--;
//BX_DEBUG(( "#   allow12 = %u\n", (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq12);
    if (BX_KEY_THIS s.kbd_controller.allow_irq12)
      BX_KEY_THIS s.kbd_controller.irq12_requested = 1;
    }
  else {
//BX_DEBUG(( "#   servicing no code\n");
    if (bx_dbg.keyboard) {
      BX_INFO(("service_keyboard(): no keys waiting\n"));
      }
    }
  return(retval);
}




  void
bx_keyb_c::activate_timer(void)
{
  if (BX_KEY_THIS s.kbd_controller.timer_pending == 0) {
    BX_KEY_THIS s.kbd_controller.timer_pending = bx_options.keyboard_serial_delay;
    }
}



  void
bx_keyb_c::kbd_ctrl_to_mouse(Bit8u   value)
{
BX_INFO(("MOUSE: kbd_ctrl_to_mouse(%02xh)\n", (unsigned) value));
BX_INFO(("  enable = %u\n", (unsigned) BX_KEY_THIS s.mouse.enable));
BX_INFO(("  allow_irq12 = %u\n",
  (unsigned) BX_KEY_THIS s.kbd_controller.allow_irq12));
BX_INFO(("  aux_clock_enabled = %u\n",
  (unsigned) BX_KEY_THIS s.kbd_controller.aux_clock_enabled));
//BX_DEBUG(( "MOUSE: kbd_ctrl_to_mouse(%02xh)\n", (unsigned) value));

  // an ACK (0xFA) is always the first response to any valid input
  // received from the system other than Set-Wrap-Mode & Resend-Command


 if (BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter) {
       BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 0;
       switch (BX_KEY_THIS s.kbd_controller.last_mouse_command) {
	     case 0xf3: // Set Mouse Sample Rate
		   BX_KEY_THIS s.mouse.sample_rate = value;
		   if (bx_dbg.mouse)
			 BX_INFO(("[mouse] Sampling rate set: %d Hz\n", value));
		   controller_enQ(0xFA, 1); // ack
		   break;

	     case 0xe8: // Set Mouse Resolution
		   switch (value) {
			 case 0:
			       BX_KEY_THIS s.mouse.resolution_cpmm = 1;
			       break;
			 case 1:
			       BX_KEY_THIS s.mouse.resolution_cpmm = 2;
			       break;
			 case 2:
			       BX_KEY_THIS s.mouse.resolution_cpmm = 4;
			       break;
			 case 3:
			       BX_KEY_THIS s.mouse.resolution_cpmm = 8;
			       break;
			 default:
			       BX_PANIC(("[mouse] Unknown resolution %d\n", value));
			       break;
		   }
		   if (bx_dbg.mouse)
			 BX_INFO(("[mouse] Resolution set to %d counts per mm\n",
				   BX_KEY_THIS s.mouse.resolution_cpmm));

		   controller_enQ(0xFA, 1); // ack
		   break;

	     default:
		   BX_PANIC(("MOUSE: unknown last command (%02xh)\n", (unsigned) BX_KEY_THIS s.kbd_controller.last_mouse_command));
       }
 } else {
  BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 0;
  BX_KEY_THIS s.kbd_controller.last_mouse_command = value;
  switch ( value ) {
    case 0xe6: // Set Mouse Scaling to 1:1
      controller_enQ(0xFA, 1); // ACK
      BX_KEY_THIS s.mouse.scaling         = 2;
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Scaling set to 1:1\n"));
      break;

    case 0xe7: // Set Mouse Scaling to 2:1
      controller_enQ(0xFA, 1); // ACK
      BX_KEY_THIS s.mouse.scaling         = 2;
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Scaling set to 2:1\n"));
      break;

    case 0xe8: // Set Mouse Resolution
      controller_enQ(0xFA, 1); // ACK
      BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 1;
      break;

    case 0xf2: // Read Device Type
      controller_enQ(0xFA, 1); // ACK
      controller_enQ(0x00, 1); // Device ID
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Read mouse ID\n"));
      break;

    case 0xf3: // Set Mouse Sample Rate (sample rate written to port 60h)
      controller_enQ(0xFA, 1); // ACK
      BX_KEY_THIS s.kbd_controller.expecting_mouse_parameter = 1;
      break;

    case 0xf4: // Enable (in stream mode)
      BX_KEY_THIS s.mouse.enable = 1;
      controller_enQ(0xFA, 1); // ACK
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Mouse enabled (stream mode)\n"));
      break;

    case 0xf5: // Disable (in stream mode)
      BX_KEY_THIS s.mouse.enable = 0;
      controller_enQ(0xFA, 1); // ACK
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Mouse disabled (stream mode)\n"));
      break;

    case 0xf6: // Set Defaults
      BX_KEY_THIS s.mouse.sample_rate     = 100; /* reports per second (default) */
      BX_KEY_THIS s.mouse.resolution_cpmm = 4; /* 4 counts per millimeter (default) */
      BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
      BX_KEY_THIS s.mouse.enable          = 0;
      BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_STREAM;
      controller_enQ(0xFA, 1); // ACK
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Set Defaults\n"));
      break;

    case 0xff: // Reset
      BX_KEY_THIS s.mouse.sample_rate     = 100; /* reports per second (default) */
      BX_KEY_THIS s.mouse.resolution_cpmm = 4; /* 4 counts per millimeter (default) */
      BX_KEY_THIS s.mouse.scaling         = 1;   /* 1:1 (default) */
      BX_KEY_THIS s.mouse.mode            = MOUSE_MODE_RESET;
      BX_KEY_THIS s.mouse.enable          = 0;
      /* (mch) NT expects an ack here */
      controller_enQ(0xFA, 1); // ACK
      controller_enQ(0xAA, 1); // completion code
      controller_enQ(0x00, 1); // ID code (normal mouse, wheelmouse has id 0x3)
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Mouse reset\n"));
      break;

    case 0xe9: // Get mouse information
      // should we ack here? (mch): Yes
      controller_enQ(0xFA, 1); // ACK
      controller_enQ(BX_KEY_THIS s.mouse.get_status_byte(), 1); // status
      controller_enQ(BX_KEY_THIS s.mouse.get_resolution_byte(), 1); // resolution
      controller_enQ(BX_KEY_THIS s.mouse.sample_rate, 1); // sample rate
      if (bx_dbg.mouse)
	    BX_INFO(("[mouse] Get mouse information\n"));
      break;

    case 0xeb: // Read Data (send a packet when in Remote Mode)
      controller_enQ(0xFA, 1); // ACK
      mouse_enQ_packet( ((BX_KEY_THIS s.mouse.button_status & 0x0f) | 0x08),
			0x00, 0x00 ); // bit3 of first byte always set
      //assumed we really aren't in polling mode, a rather odd assumption.
      BX_INFO(("[mouse] Warning: Read Data command partially supported.\n"));
      break;

    default:
      //EAh Set Stream Mode
      //ECh Reset Wrap Mode
      //EEh Set Wrap Mode
      //F0h Set Remote Mode (polling mode, i.e. not stream mode.)
      //FEh Resend
      BX_PANIC(("MOUSE: kbd_ctrl_to_mouse(%02xh)\n", (unsigned) value));
    }
 }
}

  void
bx_keyb_c::mouse_motion(int delta_x, int delta_y, unsigned button_state)
{
  Bit8u   b1, b2, b3;

  // If mouse events are disabled on the GUI headerbar, don't
  // generate any mouse data
  if (bx_options.mouse_enabled==0)
    return;

  if ( BX_KEY_THIS s.mouse.enable==0 )
    return;

  // scale down the motion
  if ( (delta_x < -1) || (delta_x > 1) )
    delta_x /= 2;
  if ( (delta_y < -1) || (delta_y > 1) )
    delta_y /= 2;

#ifdef VERBOSE_KBD_DEBUG
  if (delta_x != 0 || delta_y != 0)
    BX_INFO(("[mouse] Dx=%d Dy=%d\n", delta_x, delta_y));
#endif  /* ifdef VERBOSE_KBD_DEBUG */

  b1 = (button_state & 0x0f) | 0x08; // bit3 always set

  BX_KEY_THIS s.mouse.button_status = button_state & 0x3;

  if ( (delta_x>=0) && (delta_x<=255) ) {
    b2 = delta_x;
    }
  else if ( delta_x > 255 ) {
    b2 = 0xff;
    }
  else if ( delta_x >= -256 ) {
    b2 = delta_x;
    b1 |= 0x10;
    }
  else {
    b2 = 0x00;
    b1 |= 0x10;
    }

  if ( (delta_y>=0) && (delta_y<=255) ) {
    b3 = delta_y;
    }
  else if ( delta_y > 255 ) {
    b3 = 0xff;
    }
  else if ( delta_y >= -256 ) {
    b3 = delta_y;
    b1 |= 0x20;
    }
  else {
    b3 = 0x00;
    b1 |= 0x20;
    }
  mouse_enQ_packet(b1, b2, b3);
}


  void
bx_keyb_c::put_scancode( unsigned char *code, int count )
{
  for ( int i = 0 ; i < count ; i++ ) {
    kbd_enQ( code[i] );
    }

  return;
}


  int
bx_keyb_c::SaveState( class state_file *fd )
{
  fd->write_check ("keyboard start");
  fd->write (&BX_KEY_THIS s, sizeof (BX_KEY_THIS s));
  fd->write_check ("keyboard end");
  return(0);
}


  int
bx_keyb_c::LoadState( class state_file *fd )
{
  fd->read_check ("keyboard start");
  fd->read (&BX_KEY_THIS s, sizeof (BX_KEY_THIS s));
  fd->read_check ("keyboard end");
  return(0);
}
