//  Copyright (C) 2000  MandrakeSoft S.A.
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



class bx_keyb_c {
public:
  bx_keyb_c(void);
  ~bx_keyb_c(void);
  BX_KEY_SMF void     init(bx_devices_c *d, bx_cmos_c *cmos);
  BX_KEY_SMF void     gen_scancode(Bit32u   scancode);
  BX_KEY_SMF Bit8u    get_kbd_enable(void);
  BX_KEY_SMF void     mouse_motion(int delta_x, int delta_y, unsigned button_state);
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
      /* status bits */
      Boolean pare;
      Boolean tim;
      Boolean auxb;
      Boolean keyl;
      Boolean c_d; /* 1=command to port 64h, 0=data to port 60h */
      Boolean sysf;
      Boolean inpb;
      Boolean outb;

      Boolean scan_convert;
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
      } kbd_controller;

    struct mouseStruct {
      Bit8u   sample_rate;
      Bit8u   resolution_cpmm; // resolution in counts per mm
      Bit8u   scaling;
      Bit8u   mode;
      Boolean enable;

      Bit8u get_status_byte ()
	{
	  Bit8u ret = 0;
	  // we're always in stream mode (right?)
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
	    bx_panic("[mouse] invalid resolution_cpmm");
	  };
	  return ret;
	}

      Bit8u button_status;
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

#if BX_USE_KEY_SMF
extern bx_keyb_c bx_keyboard;
#endif

#endif  // #ifndef _PCKEY_H
