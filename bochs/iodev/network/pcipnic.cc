/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003  Fen Systems Ltd. (http://www.fensystems.co.uk/)
//  Copyright (C) 2003-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.

#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_PCIPNIC

#include "pci.h"
#include "netmod.h"
#include "pcipnic.h"

#define LOG_THIS thePNICDevice->

bx_pcipnic_c* thePNICDevice = NULL;

const Bit8u pnic_iomask[16] = {2, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// builtin configuration handling functions

void pnic_init_options(void)
{
  bx_param_c *network = SIM->get_param("network");
  bx_list_c *menu = new bx_list_c(network, "pcipnic", "PCI Pseudo NIC");
  menu->set_options(menu->SHOW_PARENT);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable Pseudo NIC emulation",
    "Enables the Pseudo NIC emulation",
    1);
  SIM->init_std_nic_options("Pseudo NIC", menu);
  enabled->set_dependent_list(menu->clone());
}

Bit32s pnic_options_parser(const char *context, int num_params, char *params[])
{
  int ret, valid = 0;

  if (!strcmp(params[0], "pcipnic")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_PNIC);
    if (!SIM->get_param_bool("enabled", base)->get()) {
      SIM->get_param_enum("ethmod", base)->set_by_name("null");
    }
    if (!SIM->get_param_string("mac", base)->isempty()) {
      // MAC address is already initialized
      valid |= 0x04;
    }
    for (int i = 1; i < num_params; i++) {
      ret = SIM->parse_nic_params(context, params[i], base);
      if (ret > 0) {
        valid |= ret;
      }
    }
    if (!SIM->get_param_bool("enabled", base)->get()) {
      if (valid == 0x04) {
        SIM->get_param_bool("enabled", base)->set(1);
      }
    }
    if (valid < 0x80) {
      if ((valid & 0x04) == 0) {
        BX_PANIC(("%s: 'pcipnic' directive incomplete (mac is required)", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s pnic_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_PNIC), NULL, 0);
}

// device plugin entry points

int CDECL libpcipnic_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePNICDevice = new bx_pcipnic_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePNICDevice, BX_PLUGIN_PCIPNIC);
  // add new configuration parameter for the config interface
  pnic_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("pcipnic", pnic_options_parser, pnic_options_save);
  return 0; // Success
}

void CDECL libpcipnic_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("pcipnic");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("network");
  menu->remove("pnic");
  delete thePNICDevice;
}

// the device object

bx_pcipnic_c::bx_pcipnic_c()
{
  put("pcipnic", "PNIC");
  memset(&s, 0, sizeof(bx_pnic_t));
  ethdev = NULL;
}

bx_pcipnic_c::~bx_pcipnic_c()
{
  if (ethdev != NULL) {
    delete ethdev;
  }
  SIM->get_bochs_root()->remove("pcipnic");
  BX_DEBUG(("Exit"));
}

void bx_pcipnic_c::init(void)
{
  bx_list_c *base;
  bx_param_string_c *bootrom;

  // Read in values from config interface
  base = (bx_list_c*) SIM->get_param(BXPN_PNIC);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("PCI Pseudo NIC disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("pcipnic"))->set(0);
    return;
  }

  memcpy(BX_PNIC_THIS s.macaddr, SIM->get_param_string("mac", base)->getptr(), 6);

  BX_PNIC_THIS s.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_PNIC_THIS s.devfunc, BX_PLUGIN_PCIPNIC,
                            "Experimental PCI Pseudo NIC");

  // initialize readonly registers
  init_pci_conf(PNIC_PCI_VENDOR, PNIC_PCI_DEVICE, 0x01, 0x020000, 0x00);
  BX_PNIC_THIS pci_conf[0x3d] = BX_PCI_INTA;

  BX_PNIC_THIS s.statusbar_id = bx_gui->register_statusitem("PNIC", 1);

  // Attach to the selected ethernet module
  BX_PNIC_THIS ethdev = DEV_net_init_module(base, rx_handler, rx_status_handler, this);

  BX_PNIC_THIS pci_base_address[4] = 0;
  BX_PNIC_THIS pci_rom_address = 0;
  bootrom = SIM->get_param_string("bootrom", base);
  if (!bootrom->isempty()) {
    BX_PNIC_THIS load_pci_rom(bootrom->getptr());
  }

  BX_INFO(("PCI Pseudo NIC initialized"));
}

void bx_pcipnic_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x04, 0x01 }, { 0x05, 0x00 }, // command_io
    { 0x06, 0x00 }, { 0x07, 0x00 }, // status
    { 0x0d, 0x20 },                 // bus latency
    // address space 0x20 - 0x23
    { 0x20, 0x01 }, { 0x21, 0x00 },
    { 0x22, 0x00 }, { 0x23, 0x00 },
    { 0x3c, 0x00, },                // IRQ
    { 0x6a, 0x01 },                 // PNIC clock
    { 0xc1, 0x20 }                  // PIRQ enable

  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_PNIC_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  // Set up initial register values
  BX_PNIC_THIS s.rCmd = PNIC_CMD_NOOP;
  BX_PNIC_THIS s.rStatus = PNIC_STATUS_OK;
  BX_PNIC_THIS s.rLength = 0;
  BX_PNIC_THIS s.rDataCursor = 0;
  BX_PNIC_THIS s.recvIndex = 0;
  BX_PNIC_THIS s.recvQueueLength = 0;
  BX_PNIC_THIS s.irqEnabled = 0;

  // Deassert IRQ
  set_irq_level(0);
}

void bx_pcipnic_c::register_state(void)
{
  char name[6];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pcipnic", "PCI Pseudo NIC State");
  new bx_shadow_num_c(list, "irqEnabled", &BX_PNIC_THIS s.irqEnabled);
  new bx_shadow_num_c(list, "rCmd", &BX_PNIC_THIS s.rCmd);
  new bx_shadow_num_c(list, "rStatus", &BX_PNIC_THIS s.rStatus);
  new bx_shadow_num_c(list, "rLength", &BX_PNIC_THIS s.rLength);
  new bx_shadow_num_c(list, "rDataCursor", &BX_PNIC_THIS s.rDataCursor);
  new bx_shadow_num_c(list, "recvIndex", &BX_PNIC_THIS s.recvIndex);
  new bx_shadow_num_c(list, "recvQueueLength", &BX_PNIC_THIS s.recvQueueLength);
  bx_list_c *recvRL = new bx_list_c(list, "recvRingLength");
  for (unsigned i=0; i<PNIC_RECV_RINGS; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(recvRL, name, &BX_PNIC_THIS s.recvRingLength[i]);
  }
  new bx_shadow_data_c(list, "rData", BX_PNIC_THIS s.rData, PNIC_DATA_SIZE);
  new bx_shadow_data_c(list, "recvRing", (Bit8u*)BX_PNIC_THIS s.recvRing, PNIC_RECV_RINGS*PNIC_DATA_SIZE);
  register_pci_state(list);
}

void bx_pcipnic_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_PNIC_THIS_PTR, read_handler, write_handler,
                          &BX_PNIC_THIS pci_base_address[4],
                          &BX_PNIC_THIS pci_conf[0x20],
                          16, &pnic_iomask[0], "PNIC")) {
    BX_INFO(("new base address: 0x%04x", BX_PNIC_THIS pci_base_address[4]));
  }
  if (BX_PNIC_THIS pci_rom_size > 0) {
    if (DEV_pci_set_base_mem(BX_PNIC_THIS_PTR, mem_read_handler,
                             mem_write_handler,
                             &BX_PNIC_THIS pci_rom_address,
                             &BX_PNIC_THIS pci_conf[0x30],
                             BX_PNIC_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_PNIC_THIS pci_rom_address));
    }
  }
}

void bx_pcipnic_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_PNIC_THIS s.devfunc, BX_PNIC_THIS pci_conf[0x3d], level);
}

bx_bool bx_pcipnic_c::mem_read_handler(bx_phy_address addr, unsigned len,
                                       void *data, void *param)
{
  Bit8u  *data_ptr;

  Bit32u mask = (BX_PNIC_THIS pci_rom_size - 1);
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    if (BX_PNIC_THIS pci_conf[0x30] & 0x01) {
      *data_ptr = BX_PNIC_THIS pci_rom[addr & mask];
    } else {
      *data_ptr = 0xff;
    }
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

bx_bool bx_pcipnic_c::mem_write_handler(bx_phy_address addr, unsigned len,
                                        void *data, void *param)
{
  BX_INFO(("write to ROM ignored (addr=0x%08x len=%d)", (Bit32u)addr, len));
  return 1;
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_pcipnic_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_pcipnic_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF
  Bit32u val = 0x0;
  Bit8u  offset;

  BX_DEBUG(("register read from address 0x%04x - ", (unsigned) address));

  offset = address - BX_PNIC_THIS pci_base_address[4];

  switch (offset) {
  case PNIC_REG_STAT:
    val = BX_PNIC_THIS s.rStatus;
    break;

  case PNIC_REG_LEN:
    val = BX_PNIC_THIS s.rLength;
    break;

  case PNIC_REG_DATA:
    if (BX_PNIC_THIS s.rDataCursor >= BX_PNIC_THIS s.rLength)
      BX_PANIC(("PNIC read at %u, beyond end of data register array",
		BX_PNIC_THIS s.rDataCursor));
    val = BX_PNIC_THIS s.rData[BX_PNIC_THIS s.rDataCursor++];
    break;

  default :
    val = 0; // keep compiler happy
    BX_PANIC(("unsupported io read from address=0x%04x!", (unsigned) address));
    break;
  }

  BX_DEBUG(("val =  0x%04x", (Bit16u) val));

  return(val);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_pcipnic_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_pcipnic_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF
  Bit8u  offset;

  BX_DEBUG(("register write to address 0x%04x - ", (unsigned) address));

  offset = address - BX_PNIC_THIS pci_base_address[4];

  switch (offset) {
  case PNIC_REG_CMD:
    BX_PNIC_THIS s.rCmd = value;
    BX_PNIC_THIS exec_command();
    break;

  case PNIC_REG_LEN:
    if (value > PNIC_DATA_SIZE)
      BX_PANIC(("PNIC bad length %u written to length register, max is %u",
		value, PNIC_DATA_SIZE));
    BX_PNIC_THIS s.rLength = value;
    BX_PNIC_THIS s.rDataCursor = 0;
    break;

  case PNIC_REG_DATA:
    if (BX_PNIC_THIS s.rDataCursor >= BX_PNIC_THIS s.rLength)
      BX_PANIC(("PNIC write at %u, beyond end of data register array",
		BX_PNIC_THIS s.rDataCursor));
    BX_PNIC_THIS s.rData[BX_PNIC_THIS s.rDataCursor++] = value;
    break;

  default:
    BX_PANIC(("unsupported io write to address=0x%04x!", (unsigned) address));
    break;
  }
}

void bx_pcipnic_c::pnic_timer_handler(void *this_ptr)
{
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;
  class_ptr->pnic_timer();
}

// Called once every 1ms
void bx_pcipnic_c::pnic_timer(void)
{
  // Do nothing atm

}

// pci configuration space read callback handler
Bit32u bx_pcipnic_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_PNIC_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%08x", address, value));

  return value;
}


// pci configuration space write callback handler
void bx_pcipnic_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;
  bx_bool romaddr_change = 0;

  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x23) && (address < 0x30)))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_PNIC_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x01;
        break;
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
        }
        break;
      case 0x20:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x21:
      case 0x22:
      case 0x23:
        baseaddr_change = (value8 != oldval);
        break;
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
        if (BX_PNIC_THIS pci_rom_size > 0) {
          if ((address+i) == 0x30) {
            value8 &= 0x01;
          } else if ((address+i) == 0x31) {
            value8 &= 0xfc;
          }
          romaddr_change = 1;
          break;
        }
      default:
        value8 = oldval;
    }
    BX_PNIC_THIS pci_conf[address+i] = value8;
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_io(BX_PNIC_THIS_PTR, read_handler, write_handler,
                            &BX_PNIC_THIS pci_base_address[4],
                            &BX_PNIC_THIS pci_conf[0x20],
                            16, &pnic_iomask[0], "PNIC")) {
      BX_INFO(("new base address: 0x%04x", BX_PNIC_THIS pci_base_address[4]));
    }
  }
  if (romaddr_change) {
    if (DEV_pci_set_base_mem(BX_PNIC_THIS_PTR, mem_read_handler,
                             mem_write_handler,
                             &BX_PNIC_THIS pci_rom_address,
                             &BX_PNIC_THIS pci_conf[0x30],
                             BX_PNIC_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_PNIC_THIS pci_rom_address));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}


/*
 * Execute a hardware command.
 */
void bx_pcipnic_c::exec_command(void)
{
  Bit16u command = BX_PNIC_THIS s.rCmd;
  Bit16u ilength = BX_PNIC_THIS s.rLength;
  Bit8u *data	 = BX_PNIC_THIS s.rData;
  // Default return values
  Bit16u status  = PNIC_STATUS_UNKNOWN_CMD;
  Bit16u olength = 0;

  if (ilength != BX_PNIC_THIS s.rDataCursor)
    BX_PANIC(("PNIC command issued with incomplete data (should be %u, is %u)",
	      ilength, BX_PNIC_THIS s.rDataCursor));

  switch (command) {
  case PNIC_CMD_NOOP:
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_API_VER:
    Bit16u api_version;

    api_version = PNIC_API_VERSION;
    olength = sizeof(api_version);
    memcpy (data, &api_version, sizeof(api_version));
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_READ_MAC:
    olength = sizeof (BX_PNIC_THIS s.macaddr);
    memcpy (data, BX_PNIC_THIS s.macaddr, olength);
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RESET:
    /* Flush the receive queue */
    BX_PNIC_THIS s.recvQueueLength = 0;
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_XMIT:
    BX_PNIC_THIS ethdev->sendpkt(data, ilength);
    bx_gui->statusbar_setitem(BX_PNIC_THIS s.statusbar_id, 1, 1);
    if (BX_PNIC_THIS s.irqEnabled) {
      set_irq_level(1);
    }
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RECV:
    if (BX_PNIC_THIS s.recvQueueLength > 0) {
      int idx = (BX_PNIC_THIS s.recvIndex - BX_PNIC_THIS s.recvQueueLength
		  + PNIC_RECV_RINGS) % PNIC_RECV_RINGS;
      olength = BX_PNIC_THIS s.recvRingLength[idx];
      memcpy (data, BX_PNIC_THIS s.recvRing[idx], olength);
      BX_PNIC_THIS s.recvQueueLength--;
    }
    if (! BX_PNIC_THIS s.recvQueueLength) {
      set_irq_level(0);
    }
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RECV_QLEN:
    Bit16u qlen;

    qlen = BX_PNIC_THIS s.recvQueueLength;
    olength = sizeof(qlen);
    memcpy (data, &qlen, sizeof(qlen));
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_MASK_IRQ:
    Bit8u enabled;

    enabled = *((Bit8u*)data);
    BX_PNIC_THIS s.irqEnabled = enabled;
    if (enabled && BX_PNIC_THIS s.recvQueueLength) {
      set_irq_level(1);
    } else {
      set_irq_level(0);
    }
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_FORCE_IRQ:
    set_irq_level(1);
    status = PNIC_STATUS_OK;
    break;

  default:
    BX_ERROR(("Unknown PNIC command %x (data length %u)", command, ilength));
    status = PNIC_STATUS_UNKNOWN_CMD;
    break;
  }

  // Set registers
  BX_PNIC_THIS s.rStatus = status;
  BX_PNIC_THIS s.rLength = olength;
  BX_PNIC_THIS s.rDataCursor = 0;
}

/*
 * Callback from the eth system driver to check if the device can receive
 */
Bit32u bx_pcipnic_c::rx_status_handler(void *arg)
{
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) arg;
  return class_ptr->rx_status();
}

Bit32u bx_pcipnic_c::rx_status()
{
  Bit32u status = BX_NETDEV_100MBIT;
  if (BX_PNIC_THIS s.recvQueueLength < PNIC_RECV_RINGS) {
    status |= BX_NETDEV_RXREADY;
  }
  return status;
}

/*
 * Callback from the eth system driver when a frame has arrived
 */
void bx_pcipnic_c::rx_handler(void *arg, const void *buf, unsigned len)
{
    // BX_DEBUG(("rx_handler with length %d", len));
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) arg;
  class_ptr->rx_frame(buf, len);
}

/*
 * rx_frame() - called by the platform-specific code when an
 * ethernet frame has been received. The destination address
 * is tested to see if it should be accepted, and if the
 * rx ring has enough room, it is copied into it and
 * the receive process is updated
 */
void bx_pcipnic_c::rx_frame(const void *buf, unsigned io_len)
{
  // Check packet length
  if (io_len > PNIC_DATA_SIZE) {
    BX_PANIC(("PNIC receive: data size %u exceeded buffer size %u",
       io_len, PNIC_DATA_SIZE));
    // Truncate if user continues
    io_len = PNIC_DATA_SIZE;
  }
  // Check receive ring is not full
  if (BX_PNIC_THIS s.recvQueueLength == PNIC_RECV_RINGS) {
    BX_ERROR(("PNIC receive: receive ring full, discarding packet"));
    return;
  }
  // Copy data to receive ring and record length
  memcpy (BX_PNIC_THIS s.recvRing[BX_PNIC_THIS s.recvIndex], buf, io_len);
  BX_PNIC_THIS s.recvRingLength[BX_PNIC_THIS s.recvIndex] = io_len;
  // Move to next ring entry
  BX_PNIC_THIS s.recvIndex = (BX_PNIC_THIS s.recvIndex + 1) % PNIC_RECV_RINGS;
  BX_PNIC_THIS s.recvQueueLength++;

  // Generate interrupt if enabled
  if (BX_PNIC_THIS s.irqEnabled) {
    set_irq_level(1);
  }
  bx_gui->statusbar_setitem(BX_PNIC_THIS s.statusbar_id, 1);
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIPNIC
