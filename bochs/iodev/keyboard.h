/////////////////////////////////////////////////////////////////////////
// $Id: keyboard.h,v 1.12 2002-03-11 15:04:58 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
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


#ifndef _PCKEY_H
#define _PCKEY_H


#define BX_KBD_ELEMENTS 16
#define BX_MOUSE_BUFF_SIZE 48

#if BX_USE_KEY_SMF
#  define BX_KEY_SMF  static
#  define BX_KEY_THIS bx_keyboard.
#else
#  define BX_KEY_SMF
#  define BX_KEY_THIS this->
#endif

#define MOUSE_MODE_RESET  10
#define MOUSE_MODE_STREAM 11
#define MOUSE_MODE_REMOTE 12
#define MOUSE_MODE_WRAP   13

extern bx_keyb_c bx_keyboard;

class bx_keyb_c : public logfunctions {
public:
  bx_keyb_c(void);
  ~bx_keyb_c(void);
  BX_KEY_SMF void     init(bx_devices_c *d, bx_cmos_c *cmos);
  BX_KEY_SMF void     gen_scancode(Bit32u   scancode);
  BX_KEY_SMF void     paste_bytes(Bit8u *data, Bit32s length);
  BX_KEY_SMF void     service_paste_buf ();
  BX_KEY_SMF Bit8u    get_kbd_enable(void);
  BX_KEY_SMF void     mouse_motion(int delta_x, int delta_y, unsigned button_state);
  BX_KEY_SMF void     mouse_enabled_changed(bool enabled);
  BX_KEY_SMF void     create_mouse_packet(bool force_enq);
  BX_KEY_SMF void     mouse_button(unsigned mouse_state);
  BX_KEY_SMF int      SaveState( class state_file *fd );
  BX_KEY_SMF int      LoadState( class state_file *fd );
  BX_KEY_SMF unsigned periodic( Bit32u   usec_delta );
  BX_KEY_SMF void     put_scancode( unsigned char *code, int count );

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_KEY_SMF
  void     write(Bit32u   address, Bit32u   value, unsigned io_len);
  Bit32u   read(Bit32u   address, unsigned io_len);
#endif

  struct {
    struct {
      /* status bits matching the status port*/
      Boolean pare; // Bit7, 1= parity error from keyboard/mouse - ignored.
      Boolean tim;  // Bit6, 1= timeout from keyboard - ignored.
      Boolean auxb; // Bit5, 1= mouse data waiting for CPU to read.
      Boolean keyl; // Bit4, 1= keyswitch in lock position - ignored.
      Boolean c_d; /*  Bit3, 1=command to port 64h, 0=data to port 60h */
      Boolean sysf; // Bit2,
      Boolean inpb; // Bit1,
      Boolean outb; // Bit0, 1= keyboard data or mouse data ready for CPU
                    //       check aux to see which. Or just keyboard
                    //       data before AT style machines

      /* internal to our version of the keyboard controller */
      Boolean kbd_clock_enabled;
      Boolean aux_clock_enabled;
      Boolean allow_irq1;
      Boolean allow_irq12;
      Bit8u   kbd_output_buffer;
      Bit8u   aux_output_buffer;
      Bit8u   last_comm;
      Bit8u   expecting_port60h;
      Bit8u   expecting_mouse_parameter;
      Bit8u   last_mouse_command;
      Bit32u   timer_pending;
      Boolean irq1_requested;
      Boolean irq12_requested;
      Boolean scancodes_translate;
      Boolean expecting_scancodes_set;
      Bit8u   current_scancodes_set;
      } kbd_controller;

    struct mouseStruct {
      Bit8u   sample_rate;
      Bit8u   resolution_cpmm; // resolution in counts per mm
      Bit8u   scaling;
      Bit8u   mode;
      Bit8u   saved_mode;  // the mode prior to entering wrap mode
      Boolean enable;

      Bit8u get_status_byte ()
	{
	  // top bit is 0 , bit 6 is 1 if remote mode.
	  Bit8u ret = (mode == MOUSE_MODE_REMOTE) ? 0x40 : 0;
	  ret |= (enable << 5);
	  ret |= (scaling == 1) ? 0 : (1 << 4);
	  ret |= ((button_status & 0x1) << 2);
	  ret |= ((button_status & 0x2) << 0);
	  return ret;
	}

      Bit8u get_resolution_byte ()
	{
	  Bit8u ret = 0;

	  switch (resolution_cpmm) {
	  case 1:
	    ret = 0;
	    break;

	  case 2:
	    ret = 1;
	    break;

	  case 4:
	    ret = 2;
	    break;

	  case 8:
	    ret = 3;
	    break;

	  default:
	    bx_keyboard.panic("mouse: invalid resolution_cpmm");
	  };
	  return ret;
	}

      Bit8u button_status;
      Bit16s delayed_dx;
      Bit16s delayed_dy;
      } mouse;

    struct {
      int     num_elements;
      Bit8u   buffer[BX_KBD_ELEMENTS];
      int     head;
      Boolean expecting_typematic;
      Boolean expecting_led_write;
      Bit8u   delay;
      Bit8u   repeat_rate;
      Bit8u   led_status;
      Boolean scanning_enabled;
      } kbd_internal_buffer;

    struct {
      int     num_elements;
      Bit8u   buffer[BX_MOUSE_BUFF_SIZE];
      int     head;
      } mouse_internal_buffer;
#define BX_KBD_CONTROLLER_QSIZE 5
    Bit8u    controller_Q[BX_KBD_CONTROLLER_QSIZE];
    unsigned controller_Qsize;
    unsigned controller_Qsource; // 0=keyboard, 1=mouse
    } s; // State information for saving/loading

  bx_devices_c *devices;

  // The paste buffer does NOT exist in the hardware.  It is a bochs
  // construction that allows the user to "paste" arbitrary length sequences of
  // keystrokes into the emulated machine.  Since the hardware buffer is only
  // 16 bytes, a very amount of data can be added to the hardware buffer at a
  // time.  The paste buffer keeps track of the bytes that have not yet been
  // pasted.
  Bit8u *pastebuf;   // ptr to bytes to be pasted, or NULL if none in progress
  Bit32u pastebuf_len; // length of pastebuf
  Bit32u pastebuf_ptr; // ptr to next byte to be added to hw buffer

  BX_KEY_SMF void     resetinternals(Boolean powerup);
  BX_KEY_SMF void     set_kbd_clock_enable(Bit8u   value);
  BX_KEY_SMF void     set_aux_clock_enable(Bit8u   value);
  BX_KEY_SMF void     kbd_ctrl_to_kbd(Bit8u   value);
  BX_KEY_SMF void     kbd_ctrl_to_mouse(Bit8u   value);
  BX_KEY_SMF void     kbd_enQ(Bit8u   scancode);
  BX_KEY_SMF void     kbd_enQ_imm(Bit8u   val);
  BX_KEY_SMF void     activate_timer(void);
  BX_KEY_SMF void     controller_enQ(Bit8u   data, unsigned source);
  BX_KEY_SMF Boolean  mouse_enQ_packet(Bit8u   b1, Bit8u   b2, Bit8u   b3);
  BX_KEY_SMF void     mouse_enQ(Bit8u   mouse_data);
  };


#endif  // #ifndef _PCKEY_H
