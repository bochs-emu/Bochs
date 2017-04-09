/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2017  The Bochs Project
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


// Initial/additional code by Ben Lunt 'fys at fysnet dot net'
//   http://www.fysnet.net

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "busmouse.h"

#if BX_SUPPORT_BUSMOUSE

#define LOG_THIS  theBusMouse->

bx_busm_c *theBusMouse = NULL;

// There is a difference between older hardware and newer hardware.
// Set this flag to indicate older hardware. I will explain the difference
//  at each item this flag is used.
#define LEGACY_BUSMOUSE 0

#define BUS_MOUSE_IRQ  5
#define IRQ_MASK ((1<<5) >> BUS_MOUSE_IRQ)

// MS Inport Bus Mouse Adapter
#define INP_PORT_CONTROL     0x023C
#define INP_PORT_DATA        0x023D
#define INP_PORT_SIGNATURE   0x023E
#define INP_PORT_CONFIG      0x023F

#define INP_CTRL_READ_BUTTONS 0x00
#define INP_CTRL_READ_X       0x01
#define INP_CTRL_READ_Y       0x02
#define INP_CTRL_COMMAND      0x07
#define INP_CTRL_RAISE_IRQ    0x16
#define INP_CTRL_RESET        0x80

#define INP_HOLD_COUNTER      (1 << 5)
#define INP_ENABLE_IRQ        (1 << 0)

// MS/Logictech Standard Bus Mouse Adapter
#define BUSM_PORT_DATA        0x023C
#define BUSM_PORT_SIGNATURE   0x023D
#define BUSM_PORT_CONTROL     0x023E
#define BUSM_PORT_CONFIG      0x023F

#define HOLD_COUNTER  (1 << 7)
#define READ_X        (0 << 6)
#define READ_Y        (1 << 6)
#define READ_LOW      (0 << 5)
#define READ_HIGH     (1 << 5)
#define DISABLE_IRQ   (1 << 4)

#define READ_X_LOW    (READ_X | READ_LOW)
#define READ_X_HIGH   (READ_X | READ_HIGH)
#define READ_Y_LOW    (READ_Y | READ_LOW)
#define READ_Y_HIGH   (READ_Y | READ_HIGH)


int CDECL libbusmouse_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Create one instance of the busmouse device object.
  theBusMouse = new bx_busm_c();
  // Register this device.
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theBusMouse, BX_PLUGIN_BUSMOUSE);
  return 0; // Success
}

void CDECL libbusmouse_LTX_plugin_fini(void)
{
  delete theBusMouse;
}

bx_busm_c::bx_busm_c()
{
  put("busmouse", "BUSM");
}

bx_busm_c::~bx_busm_c()
{
  SIM->get_bochs_root()->remove("busmouse");
  BX_DEBUG(("Exit"));
}

void bx_busm_c::init(void)
{
  BX_DEBUG(("Init $Id$"));

  BX_BUSM_THIS type = SIM->get_param_enum(BXPN_MOUSE_TYPE)->get();

  DEV_register_irq(BUS_MOUSE_IRQ, "Bus Mouse");

  // Call our timer routine at 30hz
  BX_BUSM_THIS timer_index =
    DEV_register_timer(this, timer_handler, 33334, 1, 1, "bus mouse timer");

  for (int i=0x23C; i<=0x23F; i++) {
    DEV_register_ioread_handler(this, read_handler, i, "Bus Mouse", 1);
    DEV_register_iowrite_handler(this, write_handler, i, "Bus Mouse", 1);
  }
  DEV_register_default_mouse(this, mouse_enq_static, NULL);

  BX_BUSM_THIS mouse_delayed_dx = 0;
  BX_BUSM_THIS mouse_delayed_dy = 0;
  BX_BUSM_THIS mouse_buttons    = 0;
  BX_BUSM_THIS current_x =
  BX_BUSM_THIS current_y =
  BX_BUSM_THIS current_b = 0;

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    BX_BUSM_THIS control_val   = 0;  // the control port value
    BX_BUSM_THIS mouse_buttons_last = 0;
  } else {
    BX_BUSM_THIS control_val   = 0x1f;  // the control port value
    BX_BUSM_THIS config_val    = 0x0e;  // the config port value
    BX_BUSM_THIS sig_val       = 0;     // the signature port value
  }
  BX_BUSM_THIS command_val   = 0;  // command byte
  BX_BUSM_THIS toggle_counter = 0; // signature byte / IRQ bit toggle
  BX_BUSM_THIS interrupts    = 0;  // interrupts off

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    BX_INFO(("MS Inport BusMouse initialized"));
  } else {
    BX_INFO(("Standard MS/Logitech BusMouse initialized"));
  }
}

void bx_busm_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "busmouse", "Busmouse State");
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dx, BX_BUSM_THIS mouse_delayed_dx);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dy, BX_BUSM_THIS mouse_delayed_dy);
  BXRS_HEX_PARAM_FIELD(list, mouse_buttons, BX_BUSM_THIS mouse_buttons);
  BXRS_HEX_PARAM_FIELD(list, mouse_buttons_last, BX_BUSM_THIS mouse_buttons_last);
  BXRS_HEX_PARAM_FIELD(list, current_x, BX_BUSM_THIS current_x);
  BXRS_HEX_PARAM_FIELD(list, current_y, BX_BUSM_THIS current_y);
  BXRS_HEX_PARAM_FIELD(list, current_b, BX_BUSM_THIS current_b);

  BXRS_HEX_PARAM_FIELD(list, control_val, BX_BUSM_THIS control_val);
  BXRS_HEX_PARAM_FIELD(list, command_val, BX_BUSM_THIS command_val);
  BXRS_HEX_PARAM_FIELD(list, toggle_counter, BX_BUSM_THIS toggle_counter);
  BXRS_PARAM_BOOL(list, interrupts, BX_BUSM_THIS interrupts);

  BXRS_HEX_PARAM_FIELD(list, config_val, BX_BUSM_THIS config_val);
  BXRS_HEX_PARAM_FIELD(list, sig_val, BX_BUSM_THIS sig_val);
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions
Bit32u bx_busm_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_BUSM_SMF
  bx_busm_c *class_ptr = (bx_busm_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_busm_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_BUSM_SMF

  Bit8u value = 0;

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    switch (address) {
      case INP_PORT_CONTROL:
        value = BX_BUSM_THIS control_val;
        break;
      case INP_PORT_DATA:
        switch (BX_BUSM_THIS command_val) {
          case INP_CTRL_READ_BUTTONS:
            value = BX_BUSM_THIS current_b;
#if !LEGACY_BUSMOUSE
            value |= 0x40;  // Newer Logitech mouse drivers look for bit 6 set
#endif
            break;
          case INP_CTRL_READ_X:
            value = BX_BUSM_THIS current_x;
            break;
          case INP_CTRL_READ_Y:
            value = BX_BUSM_THIS current_y;
            break;
          case INP_CTRL_COMMAND:
            value = BX_BUSM_THIS control_val;
            break;
          default:
            BX_ERROR(("Reading data port in unsupported mode 0x%02x", BX_BUSM_THIS control_val));
        }
        break;
      case INP_PORT_SIGNATURE:
        if (!BX_BUSM_THIS toggle_counter) {
          value = 0xDE;
        } else {
          value = 0x12; // Manufacturer id ?
        }
        BX_BUSM_THIS toggle_counter ^= 1;
        break;
      case INP_PORT_CONFIG:
        BX_ERROR(("Unsupported read from port 0x%04x", address));
        break;
    }
  } else {
    switch (address) {
      case BUSM_PORT_CONTROL:
        value = BX_BUSM_THIS control_val;
        // this is to allow the driver to see which IRQ the card has "jumpered"
        // only happens if IRQ's are enabled
        BX_BUSM_THIS control_val |= 0x0F;
        if ((BX_BUSM_THIS toggle_counter > 0x3FF)
          // newer hardware only changes the bit when interrupts are on
          // older hardware changes the bit no matter if interrupts are on or not
#if !LEGACY_BUSMOUSE
          && BX_BUSM_THIS interrupts
#endif
        )
          BX_BUSM_THIS control_val &= ~IRQ_MASK;
        BX_BUSM_THIS toggle_counter = (BX_BUSM_THIS toggle_counter + 1) & 0x7FF;
        break;
      case BUSM_PORT_DATA:
        switch (BX_BUSM_THIS control_val & 0x60) {
          case READ_X_LOW:
            value = BX_BUSM_THIS current_x & 0x0F;
            break;
          case READ_X_HIGH:
            value = (BX_BUSM_THIS current_x >> 4) & 0x0F;
            break;
          case READ_Y_LOW:
            value = BX_BUSM_THIS current_y & 0x0F;
            break;
          case READ_Y_HIGH:
            value = ((BX_BUSM_THIS current_b ^ 7) << 5) | ((BX_BUSM_THIS current_y >> 4) & 0x0F);
            break;
          default:
            BX_ERROR(("Reading data port in unsupported mode 0x%02x", BX_BUSM_THIS control_val));
        }
        break;
      case BUSM_PORT_CONFIG:
        value = BX_BUSM_THIS config_val;
        break;
      case BUSM_PORT_SIGNATURE:
        value = BX_BUSM_THIS sig_val;
        break;
    }
  }

  BX_DEBUG(("read from address 0x%04x, value = 0x%02x ", address, value));

  return value;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_busm_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_BUSM_SMF
  bx_busm_c *class_ptr = (bx_busm_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_busm_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_BUSM_SMF

  BX_DEBUG(("write  to address 0x%04x, value = 0x%02x ", address, value));

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    switch (address) {
      case INP_PORT_CONTROL:
        switch (value) {
          case INP_CTRL_RESET:
            BX_BUSM_THIS control_val = 0;
            BX_BUSM_THIS command_val = 0;
            break;
          case INP_CTRL_COMMAND:
          case INP_CTRL_READ_BUTTONS:
          case INP_CTRL_READ_X:
          case INP_CTRL_READ_Y:
            BX_BUSM_THIS command_val = value;
            break;
          case 0x87:  // ???????
            BX_BUSM_THIS control_val = 0;
            BX_BUSM_THIS command_val = 0x07;
            break;
          default:
            BX_ERROR(("Unsupported command written to port 0x%04x (value = 0x%02x)", address, value));
        }
        break;
      case INP_PORT_DATA:
        DEV_pic_lower_irq(BUS_MOUSE_IRQ);
        if (value == INP_CTRL_RAISE_IRQ) {
          DEV_pic_raise_irq(BUS_MOUSE_IRQ);
        } else {
          switch (BX_BUSM_THIS command_val) {
            case INP_CTRL_COMMAND:
              BX_BUSM_THIS control_val = value;
              BX_BUSM_THIS interrupts = (value & INP_ENABLE_IRQ) > 0;
              break;
            default:
              BX_ERROR(("Unsupported write to port 0x%04x (value = 0x%02x)", address, value));
          }
        }
        break;
      case INP_PORT_SIGNATURE:
      case INP_PORT_CONFIG:
        BX_ERROR(("Unsupported write to port 0x%04x (value = 0x%02x)", address, value));
        break;
    }
  } else {
    switch (address) {
      case BUSM_PORT_CONTROL:
        BX_BUSM_THIS control_val = value | 0x0F;
        BX_BUSM_THIS interrupts = (value & DISABLE_IRQ) == 0;
        DEV_pic_lower_irq(BUS_MOUSE_IRQ);
        break;
      case BUSM_PORT_CONFIG:
        BX_BUSM_THIS config_val = value;
        break;
      case BUSM_PORT_SIGNATURE:
        BX_BUSM_THIS sig_val = value;
        break;
      case BUSM_PORT_DATA:
        BX_ERROR(("Unsupported write to port 0x%04x (value = 0x%02x)", address, value));
        break;
    }
  }
}

void bx_busm_c::mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  ((bx_busm_c*)dev)->mouse_enq(delta_x, delta_y, delta_z, button_state);
}

void bx_busm_c::mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  // scale down the motion
  if ((delta_x < -1) || (delta_x > 1))
    delta_x /= 2;
  if ((delta_y < -1) || (delta_y > 1))
    delta_y /= 2;

  if (delta_x > 127) delta_x =127;
  if (delta_y > 127) delta_y =127;
  if (delta_x < -128) delta_x = -128;
  if (delta_y < -128) delta_y = -128;

  BX_BUSM_THIS mouse_delayed_dx += delta_x;
  BX_BUSM_THIS mouse_delayed_dy -= delta_y;

  // change button_state MRL to LMR
  BX_BUSM_THIS mouse_buttons = (Bit8u) (((button_state & 1) << 2) |
                              ((button_state & 4) >> 1) | ((button_state & 2) >> 1));

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    // The InPort button status is as so
    //   00lmrLMR
    // where lower case letters are changed in state
    // where upper case letters are the state of that button
    // TODO:
    //  Bochs needs to call this function one more time on a button release so that
    //   bits 5:3 get cleared.  However, as is, it works with the code I tested it on,
    //   so no worries, so far.
    if ((BX_BUSM_THIS mouse_buttons & (1<<2)) ||
        ((BX_BUSM_THIS mouse_buttons_last & (1<<2)) && !(BX_BUSM_THIS mouse_buttons & (1<<2))))
      BX_BUSM_THIS mouse_buttons |= (1<<5);
    if ((BX_BUSM_THIS mouse_buttons & (1<<1)) ||
        ((BX_BUSM_THIS mouse_buttons_last & (1<<1)) && !(BX_BUSM_THIS mouse_buttons & (1<<1))))
      BX_BUSM_THIS mouse_buttons |= (1<<4);
    if ((BX_BUSM_THIS mouse_buttons & (1<<0)) ||
        ((BX_BUSM_THIS mouse_buttons_last & (1<<0)) && !(BX_BUSM_THIS mouse_buttons & (1<<0))))
      BX_BUSM_THIS mouse_buttons |= (1<<3);
    BX_BUSM_THIS mouse_buttons_last = BX_BUSM_THIS mouse_buttons;
  }
}

void bx_busm_c::update_mouse_data()
{
  int delta_x, delta_y;
  bx_bool hold;

  if (BX_BUSM_THIS mouse_delayed_dx > 127) {
    delta_x = 127;
    BX_BUSM_THIS mouse_delayed_dx -= 127;
  } else if (BX_BUSM_THIS mouse_delayed_dx < -128) {
    delta_x = -128;
    BX_BUSM_THIS mouse_delayed_dx += 128;
  } else {
    delta_x = BX_BUSM_THIS mouse_delayed_dx;
    BX_BUSM_THIS mouse_delayed_dx = 0;
  }
  if (BX_BUSM_THIS mouse_delayed_dy > 127) {
    delta_y = 127;
    BX_BUSM_THIS mouse_delayed_dy -= 127;
  } else if (BX_BUSM_THIS mouse_delayed_dy < -128) {
    delta_y = -128;
    BX_BUSM_THIS mouse_delayed_dy += 128;
  } else {
    delta_y = BX_BUSM_THIS mouse_delayed_dy;
    BX_BUSM_THIS mouse_delayed_dy = 0;
  }

  if (BX_BUSM_THIS type == BX_MOUSE_TYPE_INPORT) {
    hold = (BX_BUSM_THIS control_val & INP_HOLD_COUNTER) > 0;
  } else {
    hold = (BX_BUSM_THIS control_val & HOLD_COUNTER) > 0;
  }
  if (!hold) {
    BX_BUSM_THIS current_x = (Bit8u) delta_x;
    BX_BUSM_THIS current_y = (Bit8u) delta_y;
    BX_BUSM_THIS current_b = BX_BUSM_THIS mouse_buttons;
  }
}

void bx_busm_c::timer_handler(void *this_ptr)
{
  bx_busm_c *class_ptr = (bx_busm_c *) this_ptr;
  class_ptr->busm_timer();
}

// Called at 30hz
void bx_busm_c::busm_timer(void)
{
  // The controller updates the data on every interrupt
  // We just don't copy it to the current_X if the 'hold' bit is set
  BX_BUSM_THIS update_mouse_data();

  // if interrupts are on, fire the interrupt
  if (BX_BUSM_THIS interrupts) {
    DEV_pic_raise_irq(BUS_MOUSE_IRQ);
    BX_DEBUG(("Interrupt Fired..."));
  }
}

#endif  // BX_SUPPORT_BUSMOUSE
