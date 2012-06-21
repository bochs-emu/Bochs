/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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

#define BX_KBD_ELEMENTS 16

// these keywords should only be used in keyboard.cc
#if BX_USE_KEY_SMF
#  define BX_KEY_SMF  static
#  define BX_KEY_THIS theKeyboard->
#else
#  define BX_KEY_SMF
#  define BX_KEY_THIS this->
#endif

#define MOUSE_MODE_RESET  10
#define MOUSE_MODE_STREAM 11
#define MOUSE_MODE_REMOTE 12
#define MOUSE_MODE_WRAP   13

class bx_keyb_c : public bx_keyb_stub_c {
public:
  bx_keyb_c();
  virtual ~bx_keyb_c();
  // implement bx_devmodel_c interface
  virtual void init(void);
  virtual void reset(unsigned type);
  // override stubs from bx_keyb_stub_c
  virtual void gen_scancode(Bit32u key);
  virtual void paste_bytes(Bit8u *data, Bit32s length);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  // runtime options
  static     Bit64s   kbd_param_handler(bx_param_c *param, int set, Bit64s val);
  BX_KEY_SMF void     paste_delay_changed(Bit32u value);

private:
  BX_KEY_SMF Bit8u    get_kbd_enable(void);
  BX_KEY_SMF void     service_paste_buf ();
  BX_KEY_SMF void     create_mouse_packet(bx_bool force_enq);
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
      bx_bool pare; // Bit7, 1= parity error from keyboard/mouse - ignored.
      bx_bool tim;  // Bit6, 1= timeout from keyboard - ignored.
      bx_bool auxb; // Bit5, 1= mouse data waiting for CPU to read.
      bx_bool keyl; // Bit4, 1= keyswitch in lock position - ignored.
      bx_bool c_d; /*  Bit3, 1=command to port 64h, 0=data to port 60h */
      bx_bool sysf; // Bit2,
      bx_bool inpb; // Bit1,
      bx_bool outb; // Bit0, 1= keyboard data or mouse data ready for CPU
                    //       check aux to see which. Or just keyboard
                    //       data before AT style machines

      /* internal to our version of the keyboard controller */
      bx_bool kbd_clock_enabled;
      bx_bool aux_clock_enabled;
      bx_bool allow_irq1;
      bx_bool allow_irq12;
      Bit8u   kbd_output_buffer;
      Bit8u   aux_output_buffer;
      Bit8u   last_comm;
      Bit8u   expecting_port60h;
      Bit8u   expecting_mouse_parameter;
      Bit8u   last_mouse_command;
      Bit32u   timer_pending;
      bx_bool irq1_requested;
      bx_bool irq12_requested;
      bx_bool scancodes_translate;
      bx_bool expecting_scancodes_set;
      Bit8u   current_scancodes_set;
      bx_bool bat_in_progress;
    } kbd_controller;

    struct mouseStruct {
      Bit8u   type;
      Bit8u   sample_rate;
      Bit8u   resolution_cpmm; // resolution in counts per mm
      Bit8u   scaling;
      Bit8u   mode;
      Bit8u   saved_mode;  // the mode prior to entering wrap mode
      bx_bool enable;

      Bit8u get_status_byte ()
	{
	  // top bit is 0 , bit 6 is 1 if remote mode.
	  Bit8u ret = (Bit8u) ((mode == MOUSE_MODE_REMOTE) ? 0x40 : 0);
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
	    genlog->panic("mouse: invalid resolution_cpmm");
	  };
	  return ret;
	}

      Bit8u button_status;
      Bit16s delayed_dx;
      Bit16s delayed_dy;
      Bit16s delayed_dz;
      Bit8u im_request;
      bx_bool im_mode;
    } mouse;

    struct {
      int     num_elements;
      Bit8u   buffer[BX_KBD_ELEMENTS];
      int     head;
      bx_bool expecting_typematic;
      bx_bool expecting_led_write;
      Bit8u   delay;
      Bit8u   repeat_rate;
      Bit8u   led_status;
      bx_bool scanning_enabled;
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

  // The paste buffer does NOT exist in the hardware.  It is a bochs
  // construction that allows the user to "paste" arbitrary length sequences of
  // keystrokes into the emulated machine.  Since the hardware buffer is only
  // 16 bytes, a very small amount of data can be added to the hardware buffer
  // at a time.  The paste buffer keeps track of the bytes that have not yet
  // been pasted.
  //
  // Lifetime of a paste buffer: The paste data comes from the system
  // clipboard, which must be accessed using platform independent code in the
  // gui.  Because every gui has its own way of managing the clipboard memory
  // (in X windows, you're supposed to call Xfree for example), in the platform
  // specific code we make a copy of the clipboard buffer with
  // "new Bit8u[length]".  Then the pointer is passed into
  // bx_keyb_c::paste_bytes, along with the length.  The gui code never touches
  // the pastebuf again, and does not free it.  The keyboard code is
  // responsible for deallocating the paste buffer using delete [] buf.  The
  // paste buffer is binary data, and it is probably NOT null terminated.
  //
  // Summary: A paste buffer is allocated (new) in the platform-specific gui
  // code, passed to the keyboard model, and is freed (delete[]) when it is no
  // longer needed.
  Bit8u *pastebuf;   // ptr to bytes to be pasted, or NULL if none in progress
  Bit32u pastebuf_len; // length of pastebuf
  Bit32u pastebuf_ptr; // ptr to next byte to be added to hw buffer
  Bit32u pastedelay;   // count before paste
  bx_bool paste_service;  // set to 1 when gen_scancode() is called from paste service
  bx_bool stop_paste;  // stop the current paste operation on keypress or hardware reset

  BX_KEY_SMF void     resetinternals(bx_bool powerup);
  BX_KEY_SMF void     set_kbd_clock_enable(Bit8u value) BX_CPP_AttrRegparmN(1);
  BX_KEY_SMF void     set_aux_clock_enable(Bit8u value);
  BX_KEY_SMF void     kbd_ctrl_to_kbd(Bit8u value);
  BX_KEY_SMF void     kbd_ctrl_to_mouse(Bit8u value);
  BX_KEY_SMF void     kbd_enQ(Bit8u scancode);
  BX_KEY_SMF void     kbd_enQ_imm(Bit8u val);
  BX_KEY_SMF void     activate_timer(void);
  BX_KEY_SMF void     controller_enQ(Bit8u data, unsigned source);
  BX_KEY_SMF bx_bool  mouse_enQ_packet(Bit8u b1, Bit8u b2, Bit8u b3, Bit8u b4);
  BX_KEY_SMF void     mouse_enQ(Bit8u mouse_data);

  static void mouse_enabled_changed_static(void *dev, bx_bool enabled);
  void mouse_enabled_changed(bx_bool enabled);
  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);
  void mouse_motion(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);

  static void   timer_handler(void *);
  void   timer(void);
  int    timer_handle;
  int    statusbar_id[3];
};

#endif  // #ifndef _PCKEY_H
