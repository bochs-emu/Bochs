/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2014  The Bochs Project
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


// Initial code by Ben Lunt 'fys frontiernet net'

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "busmouse.h"

#if BX_SUPPORT_BUSMOUSE

#define LOG_THIS  theBusMouse->

bx_busm_c *theBusMouse = NULL;

#define BUS_MOUSE_IRQ  5

#define PORT_CONTROL     0x023C
#define PORT_DATA        0x023D
#define PORT_SIGNATURE   0x023E
#define PORT_CONFIG      0x023F

#define BUSM_CTRL_READ_BUTTONS 0x00
#define BUSM_CTRL_READ_X       0x01
#define BUSM_CTRL_READ_Y       0x02
#define BUSM_CTRL_COMMAND      0x07

int CDECL libbusmouse_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
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

  DEV_register_irq(BUS_MOUSE_IRQ, "Bus Mouse");

  // Call our timer routine at 30hz
  BX_BUSM_THIS timer_index =
    bx_pc_system.register_timer(this, timer_handler, 33334, 1, 1, "bus mouse timer");

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

  BX_BUSM_THIS control_val   = 0;  // the control port value
  BX_BUSM_THIS command_val   = 0;  // command byte
  BX_BUSM_THIS sig_port_sequ = 0;  // signature byte toggle
  BX_BUSM_THIS interrupts    = 0;  // interrupts off
  BX_BUSM_THIS needs_update  = 0;

  BX_INFO(("BusMouse initialized"));
}

void bx_busm_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "busmouse", "Busmouse State");
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dx, BX_BUSM_THIS mouse_delayed_dx);
  BXRS_DEC_PARAM_FIELD(list, mouse_delayed_dy, BX_BUSM_THIS mouse_delayed_dy);
  BXRS_HEX_PARAM_FIELD(list, mouse_buttons, BX_BUSM_THIS mouse_buttons);
  BXRS_HEX_PARAM_FIELD(list, current_x, BX_BUSM_THIS current_x);
  BXRS_HEX_PARAM_FIELD(list, current_y, BX_BUSM_THIS current_y);
  BXRS_HEX_PARAM_FIELD(list, current_b, BX_BUSM_THIS current_b);

  BXRS_HEX_PARAM_FIELD(list, control_val, BX_BUSM_THIS control_val);
  BXRS_HEX_PARAM_FIELD(list, command_val, BX_BUSM_THIS command_val);
  BXRS_PARAM_BOOL(list, sig_port_sequ, BX_BUSM_THIS sig_port_sequ);
  BXRS_PARAM_BOOL(list, interrupts, BX_BUSM_THIS interrupts);
  BXRS_PARAM_BOOL(list, needs_update, BX_BUSM_THIS needs_update);
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

  switch (address) {
    case PORT_CONTROL:
      value = BX_BUSM_THIS control_val;
      break;
    case PORT_DATA:
      switch (BX_BUSM_THIS control_val) {
        case BUSM_CTRL_READ_BUTTONS:
          value = BX_BUSM_THIS current_b;
          break;
        case BUSM_CTRL_READ_X:
          value = BX_BUSM_THIS current_x;
          break;
        case BUSM_CTRL_READ_Y:
          value = BX_BUSM_THIS current_y;
          break;
        case BUSM_CTRL_COMMAND:
          value = BX_BUSM_THIS command_val;
          break;
        default:
          BX_ERROR(("Reading data port in unsupported mode 0x%02x", BX_BUSM_THIS control_val));
      }
      break;
    case PORT_SIGNATURE:
      if (!BX_BUSM_THIS sig_port_sequ) {
        value = 0xDE;
      } else {
        value = 0x22; // Manufacturer id ?
      }
      BX_BUSM_THIS sig_port_sequ ^= 1;
      break;
    case PORT_CONFIG:
      BX_ERROR(("Unsupported read from port 0x%04x", address));
      break;
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

  switch (address) {
    case PORT_CONTROL:
      BX_BUSM_THIS control_val = value & 0x07;
      break;
    case PORT_DATA:
      if (BX_BUSM_THIS control_val == BUSM_CTRL_COMMAND) {
        if ((value & 0x20) == 0x20) {
          BX_BUSM_THIS update_mouse_data();
        } else {
          DEV_pic_lower_irq(BUS_MOUSE_IRQ);
        }
        switch (value) {
          case 0x10: // interrupts off
            BX_BUSM_THIS interrupts = 0;
            break;
          case 0x09:
          case 0x11: // interrupts on
            BX_BUSM_THIS interrupts = 1;
            break;
          case 0x16: // test irq
            DEV_pic_raise_irq(BUS_MOUSE_IRQ);
            break;
        }
        BX_BUSM_THIS command_val = value;
      } else {
        BX_ERROR(("Value 0x%02x written in supported mode 0x%02x", value, BX_BUSM_THIS control_val));
      }
      break;
    case PORT_SIGNATURE:
    case PORT_CONFIG:
      BX_ERROR(("Unsupported write to port 0x%04x (value = 0x%02x)", address, value));
      break;
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
  BX_BUSM_THIS mouse_buttons = (Bit8u)(0x40 | ((button_state & 1) << 2) |
                                       ((button_state & 2) >> 1));
  BX_BUSM_THIS needs_update = 1;
}

void bx_busm_c::update_mouse_data()
{
  int delta_x, delta_y;

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
  if ((BX_BUSM_THIS mouse_delayed_dx == 0) &&
      (BX_BUSM_THIS mouse_delayed_dy == 0)) {
    BX_BUSM_THIS needs_update = 0;
  }

  BX_BUSM_THIS current_x = (Bit8u) delta_x;
  BX_BUSM_THIS current_y = (Bit8u) delta_y;
  BX_BUSM_THIS current_b = mouse_buttons;
}

void bx_busm_c::timer_handler(void *this_ptr)
{
  bx_busm_c *class_ptr = (bx_busm_c *) this_ptr;
  class_ptr->busm_timer();
}

// Called at 30hz
void bx_busm_c::busm_timer(void)
{
  // if interrupts are on, fire the interrupt
  if (BX_BUSM_THIS interrupts && BX_BUSM_THIS needs_update) {
    DEV_pic_raise_irq(BUS_MOUSE_IRQ);
  }
}

#endif  // BX_SUPPORT_BUSMOUSE
