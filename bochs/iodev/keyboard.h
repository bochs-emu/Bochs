/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2021  The Bochs Project
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

#ifndef _PCKEY_H
#define _PCKEY_H

// these keywords should only be used in keyboard.cc
#if BX_USE_KEY_SMF
#  define BX_KEY_SMF  static
#  define BX_KEY_THIS theKeyboard->
#else
#  define BX_KEY_SMF
#  define BX_KEY_THIS this->
#endif

enum {
  MOUSE_MODE_RESET = 10,
  MOUSE_MODE_STREAM = 11,
  MOUSE_MODE_REMOTE = 12,
  MOUSE_MODE_WRAP = 13
};

class bx_keyb_c : public bx_devmodel_c {
public:
  bx_keyb_c();
  virtual ~bx_keyb_c();
  // implement bx_devmodel_c interface
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

private:
  static bool         gen_scancode_static(void *dev, Bit32u key);
  BX_KEY_SMF void     gen_scancode(Bit32u key);
  static Bit8u        get_elements_static(void *dev);
  BX_KEY_SMF Bit8u    get_elements(void);

  BX_KEY_SMF Bit8u    get_kbd_enable(void);
  BX_KEY_SMF void     create_mouse_packet(bool force_enq);
  BX_KEY_SMF unsigned periodic(Bit32u usec_delta);


  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_KEY_SMF
  void write(Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u address, unsigned io_len);
#endif

  struct {
    struct {
      /* status bits matching the status port*/
      bool pare; // Bit7, 1= parity error from keyboard/mouse - ignored.
      bool tim;  // Bit6, 1= timeout from keyboard - ignored.
      bool auxb; // Bit5, 1= mouse data waiting for CPU to read.
      bool keyl; // Bit4, 1= keyswitch in lock position - ignored.
      bool c_d;  //  Bit3, 1=command to port 64h, 0=data to port 60h
      bool sysf; // Bit2,
      bool inpb; // Bit1,
      bool outb; // Bit0, 1= keyboard data or mouse data ready for CPU
                 //       check aux to see which. Or just keyboard
                 //       data before AT style machines

      /* internal to our version of the keyboard controller */
      bool    kbd_clock_enabled;
      bool    aux_clock_enabled;
      bool    allow_irq1;
      bool    allow_irq12;
      Bit8u   kbd_output_buffer;
      Bit8u   aux_output_buffer;
      Bit8u   last_comm;
      Bit8u   expecting_port60h;
      Bit8u   expecting_mouse_parameter;
      Bit8u   last_mouse_command;
      Bit32u   timer_pending;
      bool    irq1_requested;
      bool    irq12_requested;
      bool    scancodes_translate;
      bool    expecting_scancodes_set;
      Bit8u   current_scancodes_set;
      bool    bat_in_progress;
      Bit8u   kbd_type;
    } kbd_controller;

    struct mouseStruct {
      Bit8u   type;
      Bit8u   sample_rate;
      Bit8u   resolution_cpmm; // resolution in counts per mm
      Bit8u   scaling;
      Bit8u   mode;
      Bit8u   saved_mode;  // the mode prior to entering wrap mode
      bool    enable;

      Bit8u get_status_byte()
 	  {
        // top bit is 0 , bit 6 is 1 if remote mode.
        Bit8u ret = (Bit8u) ((mode == MOUSE_MODE_REMOTE) ? 0x40 : 0);
        ret |= (enable << 5);
        ret |= (scaling == 1) ? 0 : (1 << 4);
        ret |= ((button_status & 0x1) << 2);
        ret |= ((button_status & 0x2) << 0);
        return ret;
      }

      Bit8u get_resolution_byte()
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
          genlog->panic("mouse: invalid resolution_cpmm");
	    }

	    return ret;
	  }

      Bit8u  button_status;
      Bit16s delayed_dx;
      Bit16s delayed_dy;
      Bit16s delayed_dz;
      Bit8u  im_request;
      bool   im_mode;
    } mouse;

    struct {
      int     num_elements;
      Bit8u   buffer[BX_KBD_ELEMENTS];
      int     head;
      bool    expecting_typematic;
      bool    expecting_led_write;
      Bit8u   delay;
      Bit8u   repeat_rate;
      Bit8u   led_status;
      bool scanning_enabled;
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

  BX_KEY_SMF void resetinternals(bool powerup);
  BX_KEY_SMF void set_kbd_clock_enable(Bit8u value) BX_CPP_AttrRegparmN(1);
  BX_KEY_SMF void set_aux_clock_enable(Bit8u value);
  BX_KEY_SMF void kbd_ctrl_to_kbd(Bit8u value);
  BX_KEY_SMF void kbd_ctrl_to_mouse(Bit8u value);
  BX_KEY_SMF void kbd_enQ(Bit8u scancode);
  BX_KEY_SMF void kbd_enQ_imm(Bit8u val);
  BX_KEY_SMF void activate_timer(void);
  BX_KEY_SMF void controller_enQ(Bit8u data, unsigned source);
  BX_KEY_SMF bool mouse_enQ_packet(Bit8u b1, Bit8u b2, Bit8u b3, Bit8u b4);
  BX_KEY_SMF void mouse_enQ(Bit8u mouse_data);

  static void mouse_enabled_changed_static(void *dev, bool enabled);
  void mouse_enabled_changed(bool enabled);
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);
  void mouse_motion(int delta_x, int delta_y, int delta_z, unsigned button_state, bool absxy);

  static void timer_handler(void *);
  int    timer_handle;
};

#endif  // #ifndef _PCKEY_H
