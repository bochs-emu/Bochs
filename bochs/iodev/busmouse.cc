/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2009  The Bochs Project
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

int libbusmouse_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  // Create one instance of the busmouse device object.
  theBusMouse = new bx_busm_c();
  // Register this device.
  BX_REGISTER_DEVICE_DEVMODEL (plugin, type, theBusMouse, BX_PLUGIN_BUSMOUSE);
  return(0); // Success
}

void libbusmouse_LTX_plugin_fini(void)
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
  BX_BUSM_THIS current_x =
  BX_BUSM_THIS current_y =
  BX_BUSM_THIS current_b = 0;

  BX_BUSM_THIS sig_port_sequ = 0;
  BX_BUSM_THIS interrupts    = 0;   // interrupts off
  BX_BUSM_THIS command_val   = 0;   // command byte
  BX_BUSM_THIS cur_command   = 0;

  // the control port values
  BX_BUSM_THIS control_val             =
  BX_BUSM_THIS control.mode_set        =
  BX_BUSM_THIS control.modeA_select    =
  BX_BUSM_THIS control.portA_dir       =
  BX_BUSM_THIS control.portC_upper_dir =
  BX_BUSM_THIS control.modeBC_select   =
  BX_BUSM_THIS control.portB_dir       =
  BX_BUSM_THIS control.portC_lower_dir =
  BX_BUSM_THIS control_val             = 0;

  BX_INFO(("Initialized BusMouse"));
}

void bx_busm_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "busmouse", "Busmouse State");
  BXRS_HEX_PARAM_FIELD(list, mouse_delayed_dx, BX_BUSM_THIS mouse_delayed_dx);
  BXRS_HEX_PARAM_FIELD(list, mouse_delayed_dx, BX_BUSM_THIS mouse_delayed_dy);
  BXRS_HEX_PARAM_FIELD(list, current_x, BX_BUSM_THIS current_x);
  BXRS_HEX_PARAM_FIELD(list, current_y, BX_BUSM_THIS current_y);
  BXRS_HEX_PARAM_FIELD(list, current_b, BX_BUSM_THIS current_b);
  BXRS_HEX_PARAM_FIELD(list, sig_port_sequ, BX_BUSM_THIS sig_port_sequ);
  BXRS_HEX_PARAM_FIELD(list, control_val, BX_BUSM_THIS control_val);

  bx_list_c *ctrl = new bx_list_c(list, "control");
  BXRS_PARAM_BOOL(ctrl, mode_set, BX_BUSM_THIS control.mode_set);
  BXRS_HEX_PARAM_FIELD(ctrl, modeA_select, BX_BUSM_THIS control.modeA_select);
  BXRS_PARAM_BOOL(ctrl, portA_dir, BX_BUSM_THIS control.portA_dir);
  BXRS_PARAM_BOOL(ctrl, portC_upper_dir, BX_BUSM_THIS control.portC_upper_dir);
  BXRS_PARAM_BOOL(ctrl, modeBC_select, BX_BUSM_THIS control.modeBC_select);
  BXRS_PARAM_BOOL(ctrl, portB_dir, BX_BUSM_THIS control.portB_dir);
  BXRS_PARAM_BOOL(ctrl, portC_lower_dir, BX_BUSM_THIS control.portC_lower_dir);

  BXRS_PARAM_BOOL(list, interrupts, BX_BUSM_THIS interrupts);
  BXRS_PARAM_BOOL(list, packet_update, BX_BUSM_THIS packet_update);
  BXRS_HEX_PARAM_FIELD(list, cur_command, BX_BUSM_THIS cur_command);
  BXRS_HEX_PARAM_FIELD(list, command_val, BX_BUSM_THIS command_val);
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
    case 0x023C:
      value = BX_BUSM_THIS control_val;
/*      value = (BX_BUSM_THIS control.mode_set        ? 1<<7 : 0)
 *            | (BX_BUSM_THIS control.modeA_select       <<5)
 *            | (BX_BUSM_THIS control.portA_dir       ? 1<<4 : 0)
 *            | (BX_BUSM_THIS control.portC_upper_dir ? 1<<3 : 0)
 *            | (BX_BUSM_THIS control.modeBC_select   ? 1<<2 : 0)
 *            | (BX_BUSM_THIS control.portB_dir       ? 1<<1 : 0)
 *            | (BX_BUSM_THIS control.portC_lower_dir ? 1<<0 : 0);
 */   break;
    case 0x023D:  // data
      switch (BX_BUSM_THIS cur_command) {
        case 0x00:  // read buttons
          value = BX_BUSM_THIS current_b;
          break;
        case 0x01:  // read x
          value = BX_BUSM_THIS current_x;
          break;
        case 0x02:  // read y
          value = BX_BUSM_THIS current_y;
          break;
        case 0x07:  // command mode
          value = BX_BUSM_THIS command_val;
          break;
        default:
          BX_PANIC(("Unknown command to data port: %x", BX_BUSM_THIS cur_command));
      }
      break;
    case 0x023E:  // sig
      if (!(BX_BUSM_THIS sig_port_sequ & 1))
        value = 0xDE;
      else
        value = 0x22; // Manufacture id?
      BX_BUSM_THIS sig_port_sequ++;
      break;
    case 0x023F:
      BX_PANIC(("Read from port 0x023F"));
      break;
  }

  BX_INFO(("read from address 0x%04x, value = 0x%02x ", address, value));

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

  BX_INFO(("write  to address 0x%04x, value = 0x%02x ", address, value));

  switch (address) {
    case 0x023C:  // control
      BX_BUSM_THIS control.mode_set        = (value & 0x80) ? 1 : 0;
      BX_BUSM_THIS control.modeA_select    = (value & 0x60) >> 5;
      BX_BUSM_THIS control.portA_dir       = (value & 0x10) ? 1 : 0;
      BX_BUSM_THIS control.portC_upper_dir = (value & 0x08) ? 1 : 0;
      BX_BUSM_THIS control.modeBC_select   = (value & 0x04) ? 1 : 0;
      BX_BUSM_THIS control.portB_dir       = (value & 0x02) ? 1 : 0;
      BX_BUSM_THIS control.portC_lower_dir = (value & 0x01) ? 1 : 0;
      BX_BUSM_THIS control_val = value;

/*
      switch (value) {
        case 0x00:  // read buttons
        case 0x01:  // read x
        case 0x02:  // read y
        case 0x07:
          BX_BUSM_THIS cur_command = (Bit8u) value;
          break;
        case 0x80: // reset
          BX_BUSM_THIS cur_command = 0x00;
          BX_BUSM_THIS command_val = 0x80;
          break;
        default:
          BX_PANIC(("Unknown command to control port %x", value));
      }
*/
      break;
    case 0x023D:  // data port
      switch (BX_BUSM_THIS cur_command) {
        case 0x07:  // command mode
          switch (value) {
            case 0x10: // interrupts off
              BX_BUSM_THIS interrupts = 0;
              break;
            case 0x11: // interrupts on
              BX_BUSM_THIS interrupts = 1;
              break;
            default:
              BX_BUSM_THIS command_val = value;
              if ((value & 0x20) == 0)
                DEV_pic_lower_irq(BUS_MOUSE_IRQ);
          }
          break;
        default:
          BX_PANIC(("Unknown command written to data port: %x", BX_BUSM_THIS cur_command));
      }
      break;
    case 0x023E:
      BX_PANIC(("Write to port 0x023E"));
      break;
    case 0x023F:
      BX_PANIC(("Write to port 0x023F"));
      break;
  }
}

void bx_busm_c::mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state)
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

  if(delta_x>127) delta_x=127;
  if(delta_y>127) delta_y=127;
  if(delta_x<-128) delta_x=-128;
  if(delta_y<-128) delta_y=-128;

  BX_BUSM_THIS mouse_delayed_dx+=delta_x;
  BX_BUSM_THIS mouse_delayed_dy-=delta_y;

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

  if ((BX_BUSM_THIS cur_command & 0x20) == 0x00) {
    BX_BUSM_THIS current_x = (Bit8u) delta_x;
    BX_BUSM_THIS current_y = (Bit8u) delta_y;
    BX_BUSM_THIS current_b = (Bit8u) ~(((button_state & 0x01)<<2) | ((button_state & 0x02) >> 1));
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
  // if interrupts are on, fire the interrupt
  if (BX_BUSM_THIS interrupts) {
    DEV_pic_raise_irq(BUS_MOUSE_IRQ);
  }
}

#endif  // BX_SUPPORT_BUSMOUSE
