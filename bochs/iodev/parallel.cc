/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
//
////////////////////////////////////////////////////////
// This code was just a few stubs until Volker.Ruppert@t-online.de
// fixed it up in November 2001.


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "parallel.h"

#define LOG_THIS theParallelDevice->

bx_parallel_c *theParallelDevice = NULL;

// builtin configuration handling functions

void parport_init_options(void)
{
  char name[4], label[80], descr[80];

  bx_list_c *parallel = (bx_list_c*)SIM->get_param("ports.parallel");
  for (int i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf(label, "Parallel Port %d", i+1);
    bx_list_c *menu = new bx_list_c(parallel, name, label);
    menu->set_options(menu->SERIES_ASK);
    sprintf(label, "Enable parallel port #%d", i+1);
    sprintf(descr, "Controls whether parallel port #%d is installed or not", i+1);
    bx_param_bool_c *enabled = new bx_param_bool_c(menu, "enabled", label, descr,
      (i==0)? 1 : 0);  // only enable #1 by default
    sprintf(label, "Parallel port #%d output file", i+1);
    sprintf(descr, "Data written to parport#%d by the guest OS is written to this file", i+1);
    bx_param_filename_c *path = new bx_param_filename_c(menu, "file", label, descr,
      "", BX_PATHNAME_LEN);
    path->set_extension("out");
    bx_list_c *deplist = new bx_list_c(NULL);
    deplist->add(path);
    enabled->set_dependent_list(deplist);
  }
}

Bit32s parport_options_parser(const char *context, int num_params, char *params[])
{
  if ((!strncmp(params[0], "parport", 7)) && (strlen(params[0]) == 8)) {
    char tmpname[80];
    int idx = params[0][7];
    if ((idx < '1') || (idx > '9')) {
      BX_PANIC(("%s: parportX directive malformed.", context));
    }
    idx -= '0';
    if (idx > BX_N_PARALLEL_PORTS) {
      BX_PANIC(("%s: parportX port number out of range.", context));
    }
    sprintf(tmpname, "ports.parallel.%d", idx);
    bx_list_c *base = (bx_list_c*) SIM->get_param(tmpname);
    for (int i=1; i<num_params; i++) {
      if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for parport%d ignored.", context, idx));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s parport_options_save(FILE *fp)
{
  char pname[20], optname[10];

  for (int i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(pname, "ports.parallel.%d", i+1);
    bx_list_c *base = (bx_list_c*) SIM->get_param(pname);
    sprintf(optname, "parport%d", i+1);
    SIM->write_param_list(fp, base, optname, 0);
  }
  return 0;
}

// device plugin entry points

int CDECL libparallel_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theParallelDevice = new bx_parallel_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theParallelDevice, BX_PLUGIN_PARALLEL);
  // add new configuration parameters for the config interface
  parport_init_options();
  // register add-on options for bochsrc and command line
  SIM->register_addon_option("parport1", parport_options_parser, parport_options_save);
  SIM->register_addon_option("parport2", parport_options_parser, NULL);
  return 0; // Success
}

void CDECL libparallel_LTX_plugin_fini(void)
{
  char port[10];

  bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.parallel");
  for (int i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(port, "parport%d", i+1);
    SIM->unregister_addon_option(port);
    sprintf(port, "%d", i+1);
    menu->remove(port);
  }
  delete theParallelDevice;
}

// the device object

bx_parallel_c::bx_parallel_c()
{
  put("parallel", "PAR");
  for (int i=0; i<BX_PARPORT_MAXDEV; i++) {
    memset(&s[i], 0, sizeof(bx_par_t));
  }
}

bx_parallel_c::~bx_parallel_c()
{
  for (int i=0; i<BX_PARPORT_MAXDEV; i++) {
    if (s[i].output != NULL)
      fclose(s[i].output);
  }
  SIM->get_bochs_root()->remove("parallel");
  BX_DEBUG(("Exit"));
}

void bx_parallel_c::init(void)
{
  Bit16u ports[BX_PARPORT_MAXDEV] = {0x0378, 0x0278};
  Bit8u irqs[BX_PARPORT_MAXDEV] = {7, 5};
  char name[16], pname[20];
  bx_list_c *base;
  int count = 0;

  BX_DEBUG(("Init $Id$"));

  for (unsigned i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(pname, "ports.parallel.%d", i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_bool("enabled", base)->get()) {
      sprintf(name, "Parallel Port %d", i + 1);
      /* parallel interrupt and i/o ports */
      BX_PAR_THIS s[i].IRQ = irqs[i];
      for (unsigned addr=ports[i]; addr<=(unsigned)(ports[i]+2); addr++) {
        DEV_register_ioread_handler(this, read_handler, addr, name, 1);
      }
      DEV_register_iowrite_handler(this, write_handler, ports[i], name, 1);
      DEV_register_iowrite_handler(this, write_handler, ports[i]+2, name, 1);
      BX_INFO (("parallel port %d at 0x%04x irq %d", i+1, ports[i], irqs[i]));
      /* internal state */
      BX_PAR_THIS s[i].STATUS.error = 1;
      BX_PAR_THIS s[i].STATUS.slct  = 1;
      BX_PAR_THIS s[i].STATUS.pe    = 0;
      BX_PAR_THIS s[i].STATUS.ack   = 1;
      BX_PAR_THIS s[i].STATUS.busy  = 1;

      BX_PAR_THIS s[i].CONTROL.strobe   = 0;
      BX_PAR_THIS s[i].CONTROL.autofeed = 0;
      BX_PAR_THIS s[i].CONTROL.init     = 1;
      BX_PAR_THIS s[i].CONTROL.slct_in  = 1;
      BX_PAR_THIS s[i].CONTROL.irq      = 0;
      BX_PAR_THIS s[i].CONTROL.input    = 0;

      BX_PAR_THIS s[i].initmode = 0;
      // virtual_printer() opens output file on demand
      count++;
    }
  }
  // Check if the device is disabled or not configured
  if (count == 0) {
    BX_INFO(("parallel ports disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("parallel"))->set(0);
    return;
  }
}

void bx_parallel_c::reset(unsigned type)
{
}

void bx_parallel_c::register_state(void)
{
  unsigned i;
  char name[4], pname[20];
  bx_list_c *base, *port;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "parallel", "Parallel Port State");
  for (i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(pname, "ports.parallel.%d", i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_bool("enabled", base)->get()) {
      sprintf(name, "%d", i);
      port = new bx_list_c(list, name);
      new bx_shadow_num_c(port, "data", &BX_PAR_THIS s[i].data, BASE_HEX);
      new bx_shadow_bool_c(port, "slct", &BX_PAR_THIS s[i].STATUS.slct);
      new bx_shadow_bool_c(port, "ack", &BX_PAR_THIS s[i].STATUS.ack);
      new bx_shadow_bool_c(port, "busy", &BX_PAR_THIS s[i].STATUS.busy);
      new bx_shadow_bool_c(port, "strobe", &BX_PAR_THIS s[i].CONTROL.strobe);
      new bx_shadow_bool_c(port, "autofeed", &BX_PAR_THIS s[i].CONTROL.autofeed);
      new bx_shadow_bool_c(port, "init", &BX_PAR_THIS s[i].CONTROL.init);
      new bx_shadow_bool_c(port, "slct_in", &BX_PAR_THIS s[i].CONTROL.slct_in);
      new bx_shadow_bool_c(port, "irq", &BX_PAR_THIS s[i].CONTROL.irq);
      new bx_shadow_bool_c(port, "input", &BX_PAR_THIS s[i].CONTROL.input);
      new bx_shadow_bool_c(port, "initmode", &BX_PAR_THIS s[i].initmode);
    }
  }
}

void bx_parallel_c::virtual_printer(Bit8u port)
{
  char pname[20];

  if (BX_PAR_THIS s[port].STATUS.slct) {
    if (BX_PAR_THIS s[port].output == NULL) {
      sprintf(pname, "ports.parallel.%d", port+1);
      bx_list_c *base = (bx_list_c*) SIM->get_param(pname);
      bx_param_string_c *fileparam = SIM->get_param_string("file", base);
      if (!fileparam->isempty()) {
        BX_PAR_THIS s[port].output = fopen(fileparam->getptr(), "wb");
        if (!BX_PAR_THIS s[port].output)
          BX_ERROR(("Could not open '%s' to write parport%d output",
                    fileparam->getptr(), port+1));
      }
    }
    if (BX_PAR_THIS s[port].output != NULL) {
      fputc(BX_PAR_THIS s[port].data, BX_PAR_THIS s[port].output);
      fflush (BX_PAR_THIS s[port].output);
    }
    if (BX_PAR_THIS s[port].CONTROL.irq == 1) {
      DEV_pic_raise_irq(BX_PAR_THIS s[port].IRQ);
    }
    BX_PAR_THIS s[port].STATUS.ack = 0;
    BX_PAR_THIS s[port].STATUS.busy = 1;
  }
  else {
    BX_ERROR(("data is valid, but printer is offline"));
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_parallel_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PAR_SMF
  bx_parallel_c *class_ptr = (bx_parallel_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_parallel_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PAR_SMF
  Bit8u offset;
  Bit8u port = 0;
  Bit32u retval;

  offset = address & 0x07;
  switch (address & 0x03f8) {
    case 0x0378: port = 0; break;
    case 0x0278: port = 1; break;
  }

  switch (offset) {
    case BX_PAR_DATA:
      if (!BX_PAR_THIS s[port].CONTROL.input) {
        return (Bit32u)BX_PAR_THIS s[port].data;
      } else {
        BX_ERROR(("read: input mode not supported"));
        return (0xFF);
      }
      break;
    case BX_PAR_STAT:
      {
        retval = ((BX_PAR_THIS s[port].STATUS.busy  << 7) |
                  (BX_PAR_THIS s[port].STATUS.ack   << 6) |
                  (BX_PAR_THIS s[port].STATUS.pe    << 5) |
                  (BX_PAR_THIS s[port].STATUS.slct  << 4) |
                  (BX_PAR_THIS s[port].STATUS.error << 3));
        if (BX_PAR_THIS s[port].STATUS.ack == 0) {
          BX_PAR_THIS s[port].STATUS.ack = 1;
          if (BX_PAR_THIS s[port].CONTROL.irq == 1) {
            DEV_pic_lower_irq(BX_PAR_THIS s[port].IRQ);
          }
        }
        if (BX_PAR_THIS s[port].initmode == 1) {
          BX_PAR_THIS s[port].STATUS.busy  = 1;
          BX_PAR_THIS s[port].STATUS.slct  = 1;
          BX_PAR_THIS s[port].STATUS.ack  = 0;
          if (BX_PAR_THIS s[port].CONTROL.irq == 1) {
            DEV_pic_raise_irq(BX_PAR_THIS s[port].IRQ);
          }
          BX_PAR_THIS s[port].initmode = 0;
        }
        BX_DEBUG(("read: parport%d status register returns 0x%02x", port+1, retval));
        return retval;
      }
      break;
    case BX_PAR_CTRL:
      {
        retval = ((BX_PAR_THIS s[port].CONTROL.input    << 5) |
                  (BX_PAR_THIS s[port].CONTROL.irq      << 4) |
                  (BX_PAR_THIS s[port].CONTROL.slct_in  << 3) |
                  (BX_PAR_THIS s[port].CONTROL.init     << 2) |
                  (BX_PAR_THIS s[port].CONTROL.autofeed << 1) |
                  (BX_PAR_THIS s[port].CONTROL.strobe));
        BX_DEBUG(("read: parport%d control register returns 0x%02x", port+1, retval));
        return retval;
      }
      break;
  }
  return(0);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_parallel_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PAR_SMF
  bx_parallel_c *class_ptr = (bx_parallel_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_parallel_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PAR_SMF
  Bit8u offset;
  Bit8u port = 0;
  char name[16];

  offset = address & 0x07;
  switch (address & 0x03f8) {
    case 0x0378: port = 0; break;
    case 0x0278: port = 1; break;
  }

  switch (offset) {
    case BX_PAR_DATA:
      BX_PAR_THIS s[port].data = (Bit8u)value;
      BX_DEBUG(("write: parport%d data output register = 0x%02x", port+1, (Bit8u)value));
      break;
    case BX_PAR_CTRL:
      {
        if ((value & 0x01) == 0x01) {
          if (BX_PAR_THIS s[port].CONTROL.strobe == 0) {
            BX_PAR_THIS s[port].CONTROL.strobe = 1;
            virtual_printer(port); // data is valid now
          }
        } else {
          if (BX_PAR_THIS s[port].CONTROL.strobe == 1) {
            BX_PAR_THIS s[port].CONTROL.strobe = 0;
          }
        }
        BX_PAR_THIS s[port].CONTROL.autofeed = ((value & 0x02) == 0x02);
        if ((value & 0x04) == 0x04) {
          if (BX_PAR_THIS s[port].CONTROL.init == 0) {
            BX_PAR_THIS s[port].CONTROL.init = 1;
            BX_PAR_THIS s[port].STATUS.busy  = 0;
            BX_PAR_THIS s[port].STATUS.slct  = 0;
            BX_PAR_THIS s[port].initmode = 1;
            BX_DEBUG(("parport%d: printer init requested", port+1));
          }
        } else {
          if (BX_PAR_THIS s[port].CONTROL.init == 1) {
            BX_PAR_THIS s[port].CONTROL.init = 0;
          }
        }
        if ((value & 0x08) == 0x08) {
          if (BX_PAR_THIS s[port].CONTROL.slct_in == 0) {
            BX_PAR_THIS s[port].CONTROL.slct_in = 1;
            BX_DEBUG(("parport%d: printer now online", port+1));
          }
        } else {
          if (BX_PAR_THIS s[port].CONTROL.slct_in == 1) {
            BX_PAR_THIS s[port].CONTROL.slct_in = 0;
            BX_DEBUG(("parport%d: printer now offline", port+1));
          }
        }
        BX_PAR_THIS s[port].STATUS.slct = BX_PAR_THIS s[port].CONTROL.slct_in;
        if ((value & 0x10) == 0x10) {
          if (BX_PAR_THIS s[port].CONTROL.irq == 0) {
            BX_PAR_THIS s[port].CONTROL.irq = 1;
            sprintf(name, "Parallel Port %d", port+1);
            DEV_register_irq(BX_PAR_THIS s[port].IRQ, name);
            BX_DEBUG(("parport%d: irq mode selected", port+1));
          }
        } else {
          if (BX_PAR_THIS s[port].CONTROL.irq == 1) {
            BX_PAR_THIS s[port].CONTROL.irq = 0;
            sprintf(name, "Parallel Port %d", port+1);
            DEV_unregister_irq(BX_PAR_THIS s[port].IRQ, name);
            BX_DEBUG(("parport%d: polling mode selected", port+1));
          }
        }
        if ((value & 0x20) == 0x20) {
          if (BX_PAR_THIS s[port].CONTROL.input == 0) {
            BX_PAR_THIS s[port].CONTROL.input = 1;
            BX_DEBUG(("parport%d: data input mode selected", port+1));
          }
        } else {
          if (BX_PAR_THIS s[port].CONTROL.input == 1) {
            BX_PAR_THIS s[port].CONTROL.input = 0;
            BX_DEBUG(("parport%d: data output mode selected", port+1));
          }
        }
        if ((value & 0xC0) > 0) {
          BX_ERROR(("write: parport%d: unsupported control bit ignored", port+1));
        }
      }
      break;
  }
}
