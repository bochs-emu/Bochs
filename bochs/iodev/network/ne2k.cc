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
/////////////////////////////////////////////////////////////////////////

// Peter Grehan (grehan@iprg.nokia.com) coded the initial version of this
// NE2000/ether stuff.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_NE2K

#if BX_SUPPORT_PCI
#include "pci.h"
#endif
#include "ne2k.h"
#include "netmod.h"

//Never completely fill the ne2k ring so that we never
// hit the unclear completely full buffer condition.
#define BX_NE2K_NEVER_FULL_RING (1)

#define LOG_THIS theNE2kDevice->

bx_ne2k_c *theNE2kDevice = NULL;

const Bit8u ne2k_iomask[32] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                               7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

// builtin configuration handling functions

void ne2k_init_options(void)
{
  bx_param_c *network = SIM->get_param("network");
  bx_list_c *menu = new bx_list_c(network, "ne2k", "NE2000");
  menu->set_options(menu->SHOW_PARENT);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable NE2K NIC emulation",
    "Enables the NE2K NIC emulation",
    1);
  bx_param_num_c *ioaddr = new bx_param_num_c(menu,
    "ioaddr",
    "NE2K I/O Address",
    "I/O base address of the emulated NE2K device",
    0, 0xffff,
    0x300);
  ioaddr->set_base(16);
  bx_param_num_c *irq = new bx_param_num_c(menu,
    "irq",
    "NE2K Interrupt",
    "IRQ used by the NE2K device",
    0, 15,
    9);
  irq->set_options(irq->USE_SPIN_CONTROL);
  SIM->init_std_nic_options("NE2K", menu);
  enabled->set_dependent_list(menu->clone());
}

Bit32s ne2k_options_parser(const char *context, int num_params, char *params[])
{
  int ret, valid = 0;

  if (!strcmp(params[0], "ne2k")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_NE2K);
    if (!SIM->get_param_bool("enabled", base)->get()) {
      SIM->get_param_enum("ethmod", base)->set_by_name("null");
    }
    if (SIM->is_pci_device(BX_PLUGIN_NE2K)) {
      valid |= 0x03;
    }
    if (!SIM->get_param_string("mac", base)->isempty()) {
      // MAC address is already initialized
      valid |= 0x04;
    }
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "ioaddr=", 7)) {
        SIM->get_param_num("ioaddr", base)->set(strtoul(&params[i][7], NULL, 16));
        valid |= 0x01;
      } else if (!strncmp(params[i], "irq=", 4)) {
        SIM->get_param_num("irq", base)->set(atol(&params[i][4]));
        valid |= 0x02;
      } else {
        if (valid == 0x07) {
          SIM->get_param_bool("enabled", base)->set(1);
        }
        ret = SIM->parse_nic_params(context, params[i], base);
        if (ret > 0) {
          valid |= ret;
        }
      }
    }
    if (valid < 0x80) {
      if ((valid & 0x03) != 0x03) {
        BX_ERROR(("%s: 'ne2k' directive incomplete (ioaddr and irq are required)", context));
      }
      if ((valid & 0x04) == 0) {
        BX_ERROR(("%s: 'ne2k' directive incomplete (mac address is required)", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s ne2k_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_NE2K), NULL, 0);
}

// device plugin entry points

int CDECL libne2k_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theNE2kDevice = new bx_ne2k_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theNE2kDevice, BX_PLUGIN_NE2K);
  // add new configuration parameter for the config interface
  ne2k_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("ne2k", ne2k_options_parser, ne2k_options_save);
  return(0); // Success
}

void CDECL libne2k_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("ne2k");
  ((bx_list_c*)SIM->get_param("network"))->remove("ne2k");
  delete theNE2kDevice;
}

// the device object

bx_ne2k_c::bx_ne2k_c()
{
  put("NE2K");
  memset(&s, 0, sizeof(bx_ne2k_t));
  s.tx_timer_index = BX_NULL_TIMER_HANDLE;
  ethdev = NULL;
}


bx_ne2k_c::~bx_ne2k_c()
{
  if (ethdev != NULL) {
    delete ethdev;
  }
  SIM->get_bochs_root()->remove("ne2k");
  BX_DEBUG(("Exit"));
}

void bx_ne2k_c::init(void)
{
  char devname[16];
  Bit8u macaddr[6];
  bx_param_string_c *bootrom;

  BX_DEBUG(("Init $Id$"));

  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_NE2K);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("NE2000 disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("ne2k"))->set(0);
    return;
  }
  memcpy(macaddr, SIM->get_param_string("mac", base)->getptr(), 6);
  strcpy(devname, "NE2000 NIC");
  BX_NE2K_THIS s.pci_enabled = SIM->is_pci_device(BX_PLUGIN_NE2K);

#if BX_SUPPORT_PCI
  if (BX_NE2K_THIS s.pci_enabled) {
    strcpy(devname, "NE2000 PCI NIC");
    BX_NE2K_THIS s.devfunc = 0x00;
    DEV_register_pci_handlers(this, &BX_NE2K_THIS s.devfunc,
        BX_PLUGIN_NE2K, devname);

    // initialize readonly registers
    init_pci_conf(0x10ec, 0x8029, 0x00, 0x020000, 0x00);
    BX_NE2K_THIS pci_conf[0x04] = 0x01;
    BX_NE2K_THIS pci_conf[0x07] = 0x02;
    BX_NE2K_THIS pci_conf[0x10] = 0x01;
    BX_NE2K_THIS pci_conf[0x3d] = BX_PCI_INTA;
    BX_NE2K_THIS s.base_address = 0x0;
    BX_NE2K_THIS pci_rom_address = 0;
    bootrom = SIM->get_param_string("bootrom", base);
    if (!bootrom->isempty()) {
      BX_NE2K_THIS load_pci_rom(bootrom->getptr());
    }
  }
#endif

  if (BX_NE2K_THIS s.tx_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_NE2K_THIS s.tx_timer_index =
      bx_pc_system.register_timer(this, tx_timer_handler, 0,
                                  0,0, "ne2k"); // one-shot, inactive
  }
  // Register the IRQ and i/o port addresses
  if (!BX_NE2K_THIS s.pci_enabled) {
    BX_NE2K_THIS s.base_address = SIM->get_param_num("ioaddr", base)->get();
    BX_NE2K_THIS s.base_irq     = SIM->get_param_num("irq", base)->get();

    DEV_register_irq(BX_NE2K_THIS s.base_irq, "NE2000 ethernet NIC");

    DEV_register_ioread_handler_range(BX_NE2K_THIS_PTR, read_handler,
                                      BX_NE2K_THIS s.base_address,
                                      BX_NE2K_THIS s.base_address + 0x0F,
                                      devname, 3);
    DEV_register_iowrite_handler_range(BX_NE2K_THIS_PTR, write_handler,
                                       BX_NE2K_THIS s.base_address,
                                       BX_NE2K_THIS s.base_address + 0x0F,
                                       devname, 3);
    DEV_register_ioread_handler(BX_NE2K_THIS_PTR, read_handler,
                                BX_NE2K_THIS s.base_address + 0x10,
                                devname, 3);
    DEV_register_iowrite_handler(BX_NE2K_THIS_PTR, write_handler,
                                 BX_NE2K_THIS s.base_address + 0x10,
                                 devname, 3);
    DEV_register_ioread_handler(BX_NE2K_THIS_PTR, read_handler,
                                BX_NE2K_THIS s.base_address + 0x1F,
                                devname, 1);
    DEV_register_iowrite_handler(BX_NE2K_THIS_PTR, write_handler,
                                 BX_NE2K_THIS s.base_address + 0x1F,
                                 devname, 1);

    bootrom = SIM->get_param_string("bootrom", base);
    if (!bootrom->isempty()) {
      BX_PANIC(("%s: boot ROM support not present yet", devname));
    }

    BX_INFO(("%s initialized port 0x%x/32 irq %d mac %02x:%02x:%02x:%02x:%02x:%02x",
             devname,
             BX_NE2K_THIS s.base_address,
             BX_NE2K_THIS s.base_irq,
             macaddr[0], macaddr[1],
             macaddr[2], macaddr[3],
             macaddr[4], macaddr[5]));
  } else {
    BX_INFO(("%s initialized mac %02x:%02x:%02x:%02x:%02x:%02x",
             devname,
             macaddr[0], macaddr[1],
             macaddr[2], macaddr[3],
             macaddr[4], macaddr[5]));
  }

  // Initialise the mac address area by doubling the physical address
  BX_NE2K_THIS s.macaddr[0]  = macaddr[0];
  BX_NE2K_THIS s.macaddr[1]  = macaddr[0];
  BX_NE2K_THIS s.macaddr[2]  = macaddr[1];
  BX_NE2K_THIS s.macaddr[3]  = macaddr[1];
  BX_NE2K_THIS s.macaddr[4]  = macaddr[2];
  BX_NE2K_THIS s.macaddr[5]  = macaddr[2];
  BX_NE2K_THIS s.macaddr[6]  = macaddr[3];
  BX_NE2K_THIS s.macaddr[7]  = macaddr[3];
  BX_NE2K_THIS s.macaddr[8]  = macaddr[4];
  BX_NE2K_THIS s.macaddr[9]  = macaddr[4];
  BX_NE2K_THIS s.macaddr[10] = macaddr[5];
  BX_NE2K_THIS s.macaddr[11] = macaddr[5];

  // ne2k signature
  for (int i = 12; i < 32; i++)
    BX_NE2K_THIS s.macaddr[i] = 0x57;

  BX_NE2K_THIS s.statusbar_id = bx_gui->register_statusitem("NE2K", 1);

  // Attach to the selected ethernet module
  BX_NE2K_THIS ethdev = DEV_net_init_module(base, rx_handler, rx_status_handler, this);

#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("ne2k", this);
#endif
}

//
// reset - restore state to power-up, cancelling all i/o
//
void bx_ne2k_c::reset(unsigned type)
{
  if (type == BX_RESET_HARDWARE) {
    // Zero out registers and memory
    memset(&BX_NE2K_THIS s.CR,  0, sizeof(BX_NE2K_THIS s.CR));
    memset(&BX_NE2K_THIS s.IMR, 0, sizeof(BX_NE2K_THIS s.IMR));
    memset(&BX_NE2K_THIS s.DCR, 0, sizeof(BX_NE2K_THIS s.DCR));
    memset(&BX_NE2K_THIS s.TCR, 0, sizeof(BX_NE2K_THIS s.TCR));
    memset(&BX_NE2K_THIS s.TSR, 0, sizeof(BX_NE2K_THIS s.TSR));
    memset(&BX_NE2K_THIS s.RCR, 0, sizeof(BX_NE2K_THIS s.RCR));
    memset(&BX_NE2K_THIS s.RSR, 0, sizeof(BX_NE2K_THIS s.RSR));
    BX_NE2K_THIS s.local_dma  = 0;
    BX_NE2K_THIS s.page_start = 0;
    BX_NE2K_THIS s.page_stop  = 0;
    BX_NE2K_THIS s.bound_ptr  = 0;
    BX_NE2K_THIS s.tx_page_start = 0;
    BX_NE2K_THIS s.num_coll   = 0;
    BX_NE2K_THIS s.tx_bytes   = 0;
    BX_NE2K_THIS s.fifo       = 0;
    BX_NE2K_THIS s.remote_dma = 0;
    BX_NE2K_THIS s.remote_start = 0;
    BX_NE2K_THIS s.remote_bytes = 0;
    BX_NE2K_THIS s.tallycnt_0 = 0;
    BX_NE2K_THIS s.tallycnt_1 = 0;
    BX_NE2K_THIS s.tallycnt_2 = 0;

    memset(&BX_NE2K_THIS s.physaddr, 0, sizeof(BX_NE2K_THIS s.physaddr));
    memset(&BX_NE2K_THIS s.mchash, 0, sizeof(BX_NE2K_THIS s.mchash));
    BX_NE2K_THIS s.curr_page = 0;

    BX_NE2K_THIS s.rempkt_ptr   = 0;
    BX_NE2K_THIS s.localpkt_ptr = 0;
    BX_NE2K_THIS s.address_cnt  = 0;

    memset(&BX_NE2K_THIS s.mem, 0, sizeof(BX_NE2K_THIS s.mem));

    // Set power-up conditions
    BX_NE2K_THIS s.CR.stop      = 1;
    BX_NE2K_THIS s.CR.rdma_cmd  = 4;
    BX_NE2K_THIS s.DCR.longaddr = 1;

    set_irq_level(0);
  }
  memset(&BX_NE2K_THIS s.ISR, 0, sizeof(BX_NE2K_THIS s.ISR));
  BX_NE2K_THIS s.ISR.reset = 1;
}

void bx_ne2k_c::register_state(void)
{
  unsigned i;
  char name[6];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "ne2k", "NE2000 State");
  bx_list_c *CR = new bx_list_c(list, "CR");
  new bx_shadow_bool_c(CR, "stop", &BX_NE2K_THIS s.CR.stop);
  new bx_shadow_bool_c(CR, "start", &BX_NE2K_THIS s.CR.start);
  new bx_shadow_bool_c(CR, "tx_packet", &BX_NE2K_THIS s.CR.tx_packet);
  new bx_shadow_num_c(CR, "rdma_cmd", &BX_NE2K_THIS s.CR.rdma_cmd);
  new bx_shadow_num_c(CR, "pgsel", &BX_NE2K_THIS s.CR.pgsel);
  bx_list_c *ISR = new bx_list_c(list, "ISR");
  new bx_shadow_bool_c(ISR, "pkt_rx", &BX_NE2K_THIS s.ISR.pkt_rx);
  new bx_shadow_bool_c(ISR, "pkt_tx", &BX_NE2K_THIS s.ISR.pkt_tx);
  new bx_shadow_bool_c(ISR, "rx_err", &BX_NE2K_THIS s.ISR.rx_err);
  new bx_shadow_bool_c(ISR, "tx_err", &BX_NE2K_THIS s.ISR.tx_err);
  new bx_shadow_bool_c(ISR, "overwrite", &BX_NE2K_THIS s.ISR.overwrite);
  new bx_shadow_bool_c(ISR, "cnt_oflow", &BX_NE2K_THIS s.ISR.cnt_oflow);
  new bx_shadow_bool_c(ISR, "rdma_done", &BX_NE2K_THIS s.ISR.rdma_done);
  new bx_shadow_bool_c(ISR, "reset", &BX_NE2K_THIS s.ISR.reset);
  bx_list_c *IMR = new bx_list_c(list, "IMR");
  new bx_shadow_bool_c(IMR, "rx_inte", &BX_NE2K_THIS s.IMR.rx_inte);
  new bx_shadow_bool_c(IMR, "tx_inte", &BX_NE2K_THIS s.IMR.tx_inte);
  new bx_shadow_bool_c(IMR, "rxerr_inte", &BX_NE2K_THIS s.IMR.rxerr_inte);
  new bx_shadow_bool_c(IMR, "txerr_inte", &BX_NE2K_THIS s.IMR.txerr_inte);
  new bx_shadow_bool_c(IMR, "overw_inte", &BX_NE2K_THIS s.IMR.overw_inte);
  new bx_shadow_bool_c(IMR, "cofl_inte", &BX_NE2K_THIS s.IMR.cofl_inte);
  new bx_shadow_bool_c(IMR, "rdma_inte", &BX_NE2K_THIS s.IMR.rdma_inte);
  bx_list_c *DCR = new bx_list_c(list, "DCR");
  new bx_shadow_bool_c(DCR, "wdsize", &BX_NE2K_THIS s.DCR.wdsize);
  new bx_shadow_bool_c(DCR, "endian", &BX_NE2K_THIS s.DCR.endian);
  new bx_shadow_bool_c(DCR, "longaddr", &BX_NE2K_THIS s.DCR.longaddr);
  new bx_shadow_bool_c(DCR, "loop", &BX_NE2K_THIS s.DCR.loop);
  new bx_shadow_bool_c(DCR, "auto_rx", &BX_NE2K_THIS s.DCR.auto_rx);
  new bx_shadow_num_c(DCR, "fifo_size", &BX_NE2K_THIS s.DCR.fifo_size);
  bx_list_c *TCR = new bx_list_c(list, "TCR");
  new bx_shadow_bool_c(TCR, "crc_disable", &BX_NE2K_THIS s.TCR.crc_disable);
  new bx_shadow_num_c(TCR, "loop_cntl", &BX_NE2K_THIS s.TCR.loop_cntl);
  new bx_shadow_bool_c(TCR, "ext_stoptx", &BX_NE2K_THIS s.TCR.ext_stoptx);
  new bx_shadow_bool_c(TCR, "coll_prio", &BX_NE2K_THIS s.TCR.coll_prio);
  bx_list_c *TSR = new bx_list_c(list, "TSR");
  new bx_shadow_bool_c(TSR, "tx_ok", &BX_NE2K_THIS s.TSR.tx_ok);
  new bx_shadow_bool_c(TSR, "collided", &BX_NE2K_THIS s.TSR.collided);
  new bx_shadow_bool_c(TSR, "aborted", &BX_NE2K_THIS s.TSR.aborted);
  new bx_shadow_bool_c(TSR, "no_carrier", &BX_NE2K_THIS s.TSR.no_carrier);
  new bx_shadow_bool_c(TSR, "fifo_ur", &BX_NE2K_THIS s.TSR.fifo_ur);
  new bx_shadow_bool_c(TSR, "cd_hbeat", &BX_NE2K_THIS s.TSR.cd_hbeat);
  new bx_shadow_bool_c(TSR, "ow_coll", &BX_NE2K_THIS s.TSR.ow_coll);
  bx_list_c *RCR = new bx_list_c(list, "RCR");
  new bx_shadow_bool_c(RCR, "errors_ok", &BX_NE2K_THIS s.RCR.errors_ok);
  new bx_shadow_bool_c(RCR, "runts_ok", &BX_NE2K_THIS s.RCR.runts_ok);
  new bx_shadow_bool_c(RCR, "broadcast", &BX_NE2K_THIS s.RCR.broadcast);
  new bx_shadow_bool_c(RCR, "multicast", &BX_NE2K_THIS s.RCR.multicast);
  new bx_shadow_bool_c(RCR, "promisc", &BX_NE2K_THIS s.RCR.promisc);
  new bx_shadow_bool_c(RCR, "monitor", &BX_NE2K_THIS s.RCR.monitor);
  bx_list_c *RSR = new bx_list_c(list, "RSR");
  new bx_shadow_bool_c(RSR, "rx_ok", &BX_NE2K_THIS s.RSR.rx_ok);
  new bx_shadow_bool_c(RSR, "bad_crc", &BX_NE2K_THIS s.RSR.bad_crc);
  new bx_shadow_bool_c(RSR, "bad_falign", &BX_NE2K_THIS s.RSR.bad_falign);
  new bx_shadow_bool_c(RSR, "fifo_or", &BX_NE2K_THIS s.RSR.fifo_or);
  new bx_shadow_bool_c(RSR, "rx_missed", &BX_NE2K_THIS s.RSR.rx_missed);
  new bx_shadow_bool_c(RSR, "rx_mbit", &BX_NE2K_THIS s.RSR.rx_mbit);
  new bx_shadow_bool_c(RSR, "rx_disabled", &BX_NE2K_THIS s.RSR.rx_disabled);
  new bx_shadow_bool_c(RSR, "deferred", &BX_NE2K_THIS s.RSR.deferred);
  new bx_shadow_num_c(list, "local_dma", &BX_NE2K_THIS s.local_dma, BASE_HEX);
  new bx_shadow_num_c(list, "page_start", &BX_NE2K_THIS s.page_start, BASE_HEX);
  new bx_shadow_num_c(list, "page_stop", &BX_NE2K_THIS s.page_stop, BASE_HEX);
  new bx_shadow_num_c(list, "bound_ptr", &BX_NE2K_THIS s.bound_ptr, BASE_HEX);
  new bx_shadow_num_c(list, "tx_page_start", &BX_NE2K_THIS s.tx_page_start, BASE_HEX);
  new bx_shadow_num_c(list, "num_coll", &BX_NE2K_THIS s.num_coll, BASE_HEX);
  new bx_shadow_num_c(list, "tx_bytes", &BX_NE2K_THIS s.tx_bytes, BASE_HEX);
  new bx_shadow_num_c(list, "fifo", &BX_NE2K_THIS s.fifo, BASE_HEX);
  new bx_shadow_num_c(list, "remote_dma", &BX_NE2K_THIS s.remote_dma, BASE_HEX);
  new bx_shadow_num_c(list, "remote_start", &BX_NE2K_THIS s.remote_start, BASE_HEX);
  new bx_shadow_num_c(list, "remote_bytes", &BX_NE2K_THIS s.remote_bytes, BASE_HEX);
  new bx_shadow_num_c(list, "tallycnt_0", &BX_NE2K_THIS s.tallycnt_0, BASE_HEX);
  new bx_shadow_num_c(list, "tallycnt_1", &BX_NE2K_THIS s.tallycnt_1, BASE_HEX);
  new bx_shadow_num_c(list, "tallycnt_2", &BX_NE2K_THIS s.tallycnt_2, BASE_HEX);
  bx_list_c *paddr = new bx_list_c(list, "physaddr");
  for (i=0; i<6; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(paddr, name, &BX_NE2K_THIS s.physaddr[i], BASE_HEX);
  }
  new bx_shadow_num_c(list, "curr_page", &BX_NE2K_THIS s.curr_page, BASE_HEX);
  bx_list_c *mchash = new bx_list_c(list, "mchash");
  for (i=0; i<8; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(mchash, name, &BX_NE2K_THIS s.mchash[i], BASE_HEX);
  }
  new bx_shadow_num_c(list, "rempkt_ptr", &BX_NE2K_THIS s.rempkt_ptr, BASE_HEX);
  new bx_shadow_num_c(list, "localpkt_ptr", &BX_NE2K_THIS s.localpkt_ptr, BASE_HEX);
  new bx_shadow_num_c(list, "address_cnt", &BX_NE2K_THIS s.address_cnt, BASE_HEX);
  new bx_shadow_data_c(list, "mem", BX_NE2K_THIS s.mem, BX_NE2K_MEMSIZ);
  new bx_shadow_bool_c(list, "tx_timer_active", &BX_NE2K_THIS s.tx_timer_active);
#if BX_SUPPORT_PCI
  if (BX_NE2K_THIS s.pci_enabled) {
    register_pci_state(list);
  }
#endif
}

#if BX_SUPPORT_PCI
void bx_ne2k_c::after_restore_state(void)
{
  if (BX_NE2K_THIS s.pci_enabled) {
    if (DEV_pci_set_base_io(BX_NE2K_THIS_PTR, read_handler, write_handler,
                            &BX_NE2K_THIS s.base_address,
                            &BX_NE2K_THIS pci_conf[0x10],
                            32, &ne2k_iomask[0], "NE2000 PCI NIC")) {
      BX_INFO(("new base address: 0x%04x", BX_NE2K_THIS s.base_address));
    }
    if (BX_NE2K_THIS pci_rom_size > 0) {
      if (DEV_pci_set_base_mem(BX_NE2K_THIS_PTR, mem_read_handler,
                               mem_write_handler,
                               &BX_NE2K_THIS pci_rom_address,
                               &BX_NE2K_THIS pci_conf[0x30],
                               BX_NE2K_THIS pci_rom_size)) {
        BX_INFO(("new ROM address: 0x%08x", BX_NE2K_THIS pci_rom_address));
      }
    }
  }
}
#endif

//
// read_cr/write_cr - utility routines for handling reads/writes to
// the Command Register
//
Bit32u bx_ne2k_c::read_cr(void)
{
  Bit32u val =
         (((BX_NE2K_THIS s.CR.pgsel    & 0x03) << 6) |
	  ((BX_NE2K_THIS s.CR.rdma_cmd & 0x07) << 3) |
	  (BX_NE2K_THIS s.CR.tx_packet << 2) |
	  (BX_NE2K_THIS s.CR.start     << 1) |
	  (BX_NE2K_THIS s.CR.stop));
  BX_DEBUG(("read CR returns 0x%02x", val));
  return val;
}

void bx_ne2k_c::write_cr(Bit32u value)
{
  BX_DEBUG(("wrote 0x%02x to CR", value));

  // Validate remote-DMA
  if ((value & 0x38) == 0x00) {
    BX_DEBUG(("CR write - invalid rDMA value 0"));
    value |= 0x20; /* dma_cmd == 4 is a safe default */
  }

  // Check for s/w reset
  if (value & 0x01) {
    BX_NE2K_THIS s.ISR.reset = 1;
    BX_NE2K_THIS s.CR.stop   = 1;
  } else {
    BX_NE2K_THIS s.CR.stop = 0;
  }

  BX_NE2K_THIS s.CR.rdma_cmd = (value & 0x38) >> 3;

  // If start command issued, the RST bit in the ISR
  // must be cleared
  if ((value & 0x02) && !BX_NE2K_THIS s.CR.start) {
    BX_NE2K_THIS s.ISR.reset = 0;
  }

  BX_NE2K_THIS s.CR.start = ((value & 0x02) == 0x02);
  BX_NE2K_THIS s.CR.pgsel = (value & 0xc0) >> 6;

  // Check for send-packet command
  if (BX_NE2K_THIS s.CR.rdma_cmd == 3) {
    // Set up DMA read from receive ring
    BX_NE2K_THIS s.remote_start = BX_NE2K_THIS s.remote_dma = BX_NE2K_THIS s.bound_ptr * 256;
    BX_NE2K_THIS s.remote_bytes = (Bit16u) chipmem_read(BX_NE2K_THIS s.bound_ptr * 256 + 2, 2);
    BX_INFO(("Sending buffer #x%x length %d",
      BX_NE2K_THIS s.remote_start,
      BX_NE2K_THIS s.remote_bytes));
  }

  // Check for start-tx
  if ((value & 0x04) && BX_NE2K_THIS s.TCR.loop_cntl) {
    if (BX_NE2K_THIS s.TCR.loop_cntl != 1) {
      BX_INFO(("Loop mode %d not supported.", BX_NE2K_THIS s.TCR.loop_cntl));
    } else {
      rx_frame (& BX_NE2K_THIS s.mem[BX_NE2K_THIS s.tx_page_start*256 - BX_NE2K_MEMSTART],
                BX_NE2K_THIS s.tx_bytes);
    }
  } else if (value & 0x04) {
    if (BX_NE2K_THIS s.CR.stop || (!BX_NE2K_THIS s.CR.start && !BX_NE2K_THIS s.pci_enabled)) {
      if (BX_NE2K_THIS s.tx_bytes == 0) /* njh@bandsman.co.uk */
        return; /* Solaris9 probe */
      BX_PANIC(("CR write - tx start, dev in reset"));
    }

    if (BX_NE2K_THIS s.tx_bytes == 0)
      BX_PANIC(("CR write - tx start, tx bytes == 0"));

    // Send the packet to the system driver
    BX_NE2K_THIS s.CR.tx_packet = 1;
    BX_NE2K_THIS ethdev->sendpkt(& BX_NE2K_THIS s.mem[BX_NE2K_THIS s.tx_page_start*256 - BX_NE2K_MEMSTART], BX_NE2K_THIS s.tx_bytes);

    // some more debug
    if (BX_NE2K_THIS s.tx_timer_active)
      BX_ERROR(("CR write, tx timer still active"));

    // Schedule a timer to trigger a tx-complete interrupt
    // The number of microseconds is the bit-time / 10.
    // The bit-time is the preamble+sfd (64 bits), the
    // inter-frame gap (96 bits), the CRC (4 bytes), and the
    // the number of bits in the frame (s.tx_bytes * 8).
    //
    bx_pc_system.activate_timer(BX_NE2K_THIS s.tx_timer_index,
				(64 + 96 + 4*8 + BX_NE2K_THIS s.tx_bytes*8)/10,
				0); // not continuous
    BX_NE2K_THIS s.tx_timer_active = 1;
    bx_gui->statusbar_setitem(BX_NE2K_THIS s.statusbar_id, 1, 1);
  }

  // Linux probes for an interrupt by setting up a remote-DMA read
  // of 0 bytes with remote-DMA completion interrupts enabled.
  // Detect this here
  if (BX_NE2K_THIS s.CR.rdma_cmd == 0x01 &&
      BX_NE2K_THIS s.CR.start &&
      BX_NE2K_THIS s.remote_bytes == 0) {
    BX_NE2K_THIS s.ISR.rdma_done = 1;
    if (BX_NE2K_THIS s.IMR.rdma_inte) {
      set_irq_level(1);
    }
  }
}

//
// chipmem_read/chipmem_write - access the 64K private RAM.
// The ne2000 memory is accessed through the data port of
// the asic (offset 0) after setting up a remote-DMA transfer.
// Both byte and word accesses are allowed.
// The first 16 bytes contains the MAC address at even locations,
// and there is 16K of buffer memory starting at 16K
//
Bit32u BX_CPP_AttrRegparmN(2)
bx_ne2k_c::chipmem_read(Bit32u address, unsigned int io_len)
{
  Bit32u retval = 0;

  if ((io_len == 2) && (address & 0x1))
    BX_PANIC(("unaligned chipmem word read"));

  // ROM'd MAC address
  if ((address >=0) && (address <= 31)) {
    retval = BX_NE2K_THIS s.macaddr[address];
    if ((io_len == 2) || (io_len == 4)) {
      retval |= (BX_NE2K_THIS s.macaddr[address + 1] << 8);
    }
    if (io_len == 4) {
      retval |= (BX_NE2K_THIS s.macaddr[address + 2] << 16);
      retval |= (BX_NE2K_THIS s.macaddr[address + 3] << 24);
    }
    return (retval);
  }

  if ((address >= BX_NE2K_MEMSTART) && (address < BX_NE2K_MEMEND)) {
    retval = BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART];
    if ((io_len == 2) || (io_len == 4)) {
      retval |= (BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 1] << 8);
    }
    if (io_len == 4) {
      retval |= (BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 2] << 16);
      retval |= (BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 3] << 24);
    }
    return (retval);
  }

  BX_DEBUG(("out-of-bounds chipmem read, %04X", address));

  return (0xff);
}

void BX_CPP_AttrRegparmN(3)
bx_ne2k_c::chipmem_write(Bit32u address, Bit32u value, unsigned io_len)
{
  if ((io_len == 2) && (address & 0x1))
    BX_PANIC(("unaligned chipmem word write"));

  if ((address >= BX_NE2K_MEMSTART) && (address < BX_NE2K_MEMEND)) {
    BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART] = value & 0xff;
    if ((io_len == 2) || (io_len == 4)) {
      BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 1] = value >> 8;
    }
    if (io_len == 4) {
      BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 2] = value >> 16;
      BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 3] = value >> 24;
    }
  } else
    BX_DEBUG(("out-of-bounds chipmem write, %04X", address));
}

//
// asic_read/asic_write - This is the high 16 bytes of i/o space
// (the lower 16 bytes is for the DS8390). Only two locations
// are used: offset 0, which is used for data transfer, and
// offset 0xf, which is used to reset the device.
// The data transfer port is used to as 'external' DMA to the
// DS8390. The chip has to have the DMA registers set up, and
// after that, insw/outsw instructions can be used to move
// the appropriate number of bytes to/from the device.
//
Bit32u BX_CPP_AttrRegparmN(2)
bx_ne2k_c::asic_read(Bit32u offset, unsigned int io_len)
{
  Bit32u retval = 0;

  switch (offset) {
  case 0x0:  // Data register
    //
    // A read remote-DMA command must have been issued,
    // and the source-address and length registers must
    // have been initialised.
    //
    if (io_len > BX_NE2K_THIS s.remote_bytes) {
      BX_ERROR(("ne2K: dma read underrun iolen=%d remote_bytes=%d",io_len,BX_NE2K_THIS s.remote_bytes));
      //return 0;
    }

    //BX_INFO(("ne2k read DMA: addr=%4x remote_bytes=%d",BX_NE2K_THIS s.remote_dma,BX_NE2K_THIS s.remote_bytes));
    retval = chipmem_read(BX_NE2K_THIS s.remote_dma, io_len);
    //
    // The 8390 bumps the address and decreases the byte count
    // by the selected word size after every access, not by
    // the amount of data requested by the host (io_len).
    //
    if (io_len == 4) {
      BX_NE2K_THIS s.remote_dma += io_len;
    } else {
      BX_NE2K_THIS s.remote_dma += (BX_NE2K_THIS s.DCR.wdsize + 1);
    }
    if (BX_NE2K_THIS s.remote_dma == BX_NE2K_THIS s.page_stop << 8) {
      BX_NE2K_THIS s.remote_dma = BX_NE2K_THIS s.page_start << 8;
    }
    // keep s.remote_bytes from underflowing
    if (BX_NE2K_THIS s.remote_bytes > BX_NE2K_THIS s.DCR.wdsize)
      if (io_len == 4) {
        BX_NE2K_THIS s.remote_bytes -= io_len;
      } else {
        BX_NE2K_THIS s.remote_bytes -= (BX_NE2K_THIS s.DCR.wdsize + 1);
      }
    else
      BX_NE2K_THIS s.remote_bytes = 0;

    // If all bytes have been written, signal remote-DMA complete
    if (BX_NE2K_THIS s.remote_bytes == 0) {
      BX_NE2K_THIS s.ISR.rdma_done = 1;
      if (BX_NE2K_THIS s.IMR.rdma_inte) {
        set_irq_level(1);
      }
    }
    break;

  case 0xf:  // Reset register
    theNE2kDevice->reset(BX_RESET_SOFTWARE);
    break;

  default:
    BX_INFO(("asic read invalid address %04x", (unsigned) offset));
    break;
  }

  return (retval);
}

void bx_ne2k_c::asic_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_DEBUG(("asic write addr=0x%02x, value=0x%04x", (unsigned) offset, (unsigned) value));
  switch (offset) {
  case 0x0:  // Data register - see asic_read for a description

    if ((io_len > 1) && (BX_NE2K_THIS s.DCR.wdsize == 0)) {
      BX_PANIC(("dma write length %d on byte mode operation", io_len));
      break;
    }
    if (BX_NE2K_THIS s.remote_bytes == 0) {
      BX_ERROR(("ne2K: dma write, byte count 0"));
    }

    chipmem_write(BX_NE2K_THIS s.remote_dma, value, io_len);
    if (io_len == 4) {
      BX_NE2K_THIS s.remote_dma += io_len;
    } else {
      BX_NE2K_THIS s.remote_dma += (BX_NE2K_THIS s.DCR.wdsize + 1);
    }
    if (BX_NE2K_THIS s.remote_dma == BX_NE2K_THIS s.page_stop << 8) {
      BX_NE2K_THIS s.remote_dma = BX_NE2K_THIS s.page_start << 8;
    }

    if (io_len == 4) {
      BX_NE2K_THIS s.remote_bytes -= io_len;
    } else {
      BX_NE2K_THIS s.remote_bytes -= (BX_NE2K_THIS s.DCR.wdsize + 1);
    }
    if (BX_NE2K_THIS s.remote_bytes > BX_NE2K_MEMSIZ)
      BX_NE2K_THIS s.remote_bytes = 0;

    // If all bytes have been written, signal remote-DMA complete
    if (BX_NE2K_THIS s.remote_bytes == 0) {
      BX_NE2K_THIS s.ISR.rdma_done = 1;
      if (BX_NE2K_THIS s.IMR.rdma_inte) {
        set_irq_level(1);
      }
    }
    break;

  case 0xf:  // Reset register
    // end of reset pulse
    break;

  default: // this is invalid, but happens under win95 device detection
    BX_INFO(("asic write invalid address %04x, ignoring", (unsigned) offset));
    break;
  }
}

//
// page0_read/page0_write - These routines handle reads/writes to
// the 'zeroth' page of the DS8390 register file
//
Bit32u bx_ne2k_c::page0_read(Bit32u offset, unsigned int io_len)
{
  Bit8u value = 0;

  if (io_len > 1) {
    BX_ERROR(("bad length! page 0 read from register 0x%02x, len=%u", offset,
              io_len)); /* encountered with win98 hardware probe */
    return value;
  }

  switch (offset) {
  case 0x1:  // CLDA0
    value = (BX_NE2K_THIS s.local_dma & 0xff);
    break;

  case 0x2:  // CLDA1
    value = (BX_NE2K_THIS s.local_dma >> 8);
    break;

  case 0x3:  // BNRY
    value = BX_NE2K_THIS s.bound_ptr;
    break;

  case 0x4:  // TSR
    value = ((BX_NE2K_THIS s.TSR.ow_coll    << 7) |
             (BX_NE2K_THIS s.TSR.cd_hbeat   << 6) |
             (BX_NE2K_THIS s.TSR.fifo_ur    << 5) |
             (BX_NE2K_THIS s.TSR.no_carrier << 4) |
             (BX_NE2K_THIS s.TSR.aborted    << 3) |
             (BX_NE2K_THIS s.TSR.collided   << 2) |
             (BX_NE2K_THIS s.TSR.tx_ok));
    break;

  case 0x5:  // NCR
    value = BX_NE2K_THIS s.num_coll;
    break;

  case 0x6:  // FIFO
    // reading FIFO is only valid in loopback mode
    BX_ERROR(("reading FIFO not supported yet"));
    value = BX_NE2K_THIS s.fifo;
    break;

  case 0x7:  // ISR
    value = ((BX_NE2K_THIS s.ISR.reset     << 7) |
             (BX_NE2K_THIS s.ISR.rdma_done << 6) |
             (BX_NE2K_THIS s.ISR.cnt_oflow << 5) |
             (BX_NE2K_THIS s.ISR.overwrite << 4) |
             (BX_NE2K_THIS s.ISR.tx_err    << 3) |
             (BX_NE2K_THIS s.ISR.rx_err    << 2) |
             (BX_NE2K_THIS s.ISR.pkt_tx    << 1) |
             (BX_NE2K_THIS s.ISR.pkt_rx));
    break;

  case 0x8:  // CRDA0
    value = (BX_NE2K_THIS s.remote_dma & 0xff);
    break;

  case 0x9:  // CRDA1
    value = (BX_NE2K_THIS s.remote_dma >> 8);
    break;

  case 0xa:  // reserved / RTL8029ID0
    if (BX_NE2K_THIS s.pci_enabled) {
      value = 0x50;
    } else {
      BX_INFO(("reserved read - page 0, 0xa"));
      value = 0xff;
    }
    break;

  case 0xb:  // reserved / RTL8029ID1
    if (BX_NE2K_THIS s.pci_enabled) {
      value = 0x43;
    } else {
      BX_INFO(("reserved read - page 0, 0xb"));
      value = 0xff;
    }
    break;

  case 0xc:  // RSR
    value = ((BX_NE2K_THIS s.RSR.deferred    << 7) |
             (BX_NE2K_THIS s.RSR.rx_disabled << 6) |
             (BX_NE2K_THIS s.RSR.rx_mbit     << 5) |
             (BX_NE2K_THIS s.RSR.rx_missed   << 4) |
             (BX_NE2K_THIS s.RSR.fifo_or     << 3) |
             (BX_NE2K_THIS s.RSR.bad_falign  << 2) |
             (BX_NE2K_THIS s.RSR.bad_crc     << 1) |
             (BX_NE2K_THIS s.RSR.rx_ok));
    break;

  case 0xd:  // CNTR0
    value = BX_NE2K_THIS s.tallycnt_0;
    break;

  case 0xe:  // CNTR1
    value = BX_NE2K_THIS s.tallycnt_1;
    break;

  case 0xf:  // CNTR2
    value = BX_NE2K_THIS s.tallycnt_2;
    break;

  default:
    BX_PANIC(("page 0 register 0x%02x out of range", offset));
  }

  BX_DEBUG(("page 0 read from register 0x%02x, value=0x%02x", offset, value));
  return value;
}

void bx_ne2k_c::page0_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  Bit8u value2;

  // It appears to be a common practice to use outw on page0 regs...

  // break up outw into two outb's
  if (io_len == 2) {
    page0_write(offset, (value & 0xff), 1);
    if (offset < 0x0f) {
      page0_write(offset + 1, ((value >> 8) & 0xff), 1);
    }
    return;
  }

  BX_DEBUG(("page 0 write to register 0x%02x, value=0x%02x", offset, value));

  switch (offset) {
  case 0x1:  // PSTART
    BX_NE2K_THIS s.page_start = value;
    break;

  case 0x2:  // PSTOP
    BX_NE2K_THIS s.page_stop = value;
    break;

  case 0x3:  // BNRY
    BX_NE2K_THIS s.bound_ptr = value;
    break;

  case 0x4:  // TPSR
    BX_NE2K_THIS s.tx_page_start = value;
    break;

  case 0x5:  // TBCR0
    // Clear out low byte and re-insert
    BX_NE2K_THIS s.tx_bytes &= 0xff00;
    BX_NE2K_THIS s.tx_bytes |= (value & 0xff);
    break;

  case 0x6:  // TBCR1
    // Clear out high byte and re-insert
    BX_NE2K_THIS s.tx_bytes &= 0x00ff;
    BX_NE2K_THIS s.tx_bytes |= ((value & 0xff) << 8);
    break;

  case 0x7:  // ISR
    value &= 0x7f;  // clear RST bit - status-only bit
    // All other values are cleared iff the ISR bit is 1
    BX_NE2K_THIS s.ISR.pkt_rx    &= ~((bx_bool)((value & 0x01) == 0x01));
    BX_NE2K_THIS s.ISR.pkt_tx    &= ~((bx_bool)((value & 0x02) == 0x02));
    BX_NE2K_THIS s.ISR.rx_err    &= ~((bx_bool)((value & 0x04) == 0x04));
    BX_NE2K_THIS s.ISR.tx_err    &= ~((bx_bool)((value & 0x08) == 0x08));
    BX_NE2K_THIS s.ISR.overwrite &= ~((bx_bool)((value & 0x10) == 0x10));
    BX_NE2K_THIS s.ISR.cnt_oflow &= ~((bx_bool)((value & 0x20) == 0x20));
    BX_NE2K_THIS s.ISR.rdma_done &= ~((bx_bool)((value & 0x40) == 0x40));
    value = ((BX_NE2K_THIS s.ISR.rdma_done << 6) |
             (BX_NE2K_THIS s.ISR.cnt_oflow << 5) |
             (BX_NE2K_THIS s.ISR.overwrite << 4) |
             (BX_NE2K_THIS s.ISR.tx_err    << 3) |
             (BX_NE2K_THIS s.ISR.rx_err    << 2) |
             (BX_NE2K_THIS s.ISR.pkt_tx    << 1) |
             (BX_NE2K_THIS s.ISR.pkt_rx));
    value &= ((BX_NE2K_THIS s.IMR.rdma_inte << 6) |
              (BX_NE2K_THIS s.IMR.cofl_inte << 5) |
              (BX_NE2K_THIS s.IMR.overw_inte << 4) |
              (BX_NE2K_THIS s.IMR.txerr_inte << 3) |
              (BX_NE2K_THIS s.IMR.rxerr_inte << 2) |
              (BX_NE2K_THIS s.IMR.tx_inte << 1) |
              (BX_NE2K_THIS s.IMR.rx_inte));
    if (value == 0)
      set_irq_level(0);
    break;

  case 0x8:  // RSAR0
    // Clear out low byte and re-insert
    BX_NE2K_THIS s.remote_start &= 0xff00;
    BX_NE2K_THIS s.remote_start |= (value & 0xff);
    BX_NE2K_THIS s.remote_dma = BX_NE2K_THIS s.remote_start;
    break;

  case 0x9:  // RSAR1
    // Clear out high byte and re-insert
    BX_NE2K_THIS s.remote_start &= 0x00ff;
    BX_NE2K_THIS s.remote_start |= ((value & 0xff) << 8);
    BX_NE2K_THIS s.remote_dma = BX_NE2K_THIS s.remote_start;
    break;

  case 0xa:  // RBCR0
    // Clear out low byte and re-insert
    BX_NE2K_THIS s.remote_bytes &= 0xff00;
    BX_NE2K_THIS s.remote_bytes |= (value & 0xff);
    break;

  case 0xb:  // RBCR1
    // Clear out high byte and re-insert
    BX_NE2K_THIS s.remote_bytes &= 0x00ff;
    BX_NE2K_THIS s.remote_bytes |= ((value & 0xff) << 8);
    break;

  case 0xc:  // RCR
    // Check if the reserved bits are set
    if (value & 0xc0)
      BX_INFO(("RCR write, reserved bits set"));

    // Set all other bit-fields
    BX_NE2K_THIS s.RCR.errors_ok = ((value & 0x01) == 0x01);
    BX_NE2K_THIS s.RCR.runts_ok  = ((value & 0x02) == 0x02);
    BX_NE2K_THIS s.RCR.broadcast = ((value & 0x04) == 0x04);
    BX_NE2K_THIS s.RCR.multicast = ((value & 0x08) == 0x08);
    BX_NE2K_THIS s.RCR.promisc   = ((value & 0x10) == 0x10);
    BX_NE2K_THIS s.RCR.monitor   = ((value & 0x20) == 0x20);

    // Monitor bit is a little suspicious...
    if (value & 0x20)
      BX_INFO(("RCR write, monitor bit set!"));
    break;

  case 0xd:  // TCR
    // Check reserved bits
    if (value & 0xe0)
      BX_ERROR(("TCR write, reserved bits set"));

    // Test loop mode (not supported)
    if (value & 0x06) {
      BX_NE2K_THIS s.TCR.loop_cntl = (value & 0x6) >> 1;
      BX_INFO(("TCR write, loop mode %d not supported", BX_NE2K_THIS s.TCR.loop_cntl));
    } else {
      BX_NE2K_THIS s.TCR.loop_cntl = 0;
    }

    // Inhibit-CRC not supported.
    if (value & 0x01)
      BX_PANIC(("TCR write, inhibit-CRC not supported"));

    // Auto-transmit disable very suspicious
    if (value & 0x08)
      BX_PANIC(("TCR write, auto transmit disable not supported"));

    // Allow collision-offset to be set, although not used
    BX_NE2K_THIS s.TCR.coll_prio = ((value & 0x08) == 0x08);
    break;

  case 0xe:  // DCR
    // the loopback mode is not suppported yet
    if (!(value & 0x08)) {
      BX_ERROR(("DCR write, loopback mode selected"));
    }
    // It is questionable to set longaddr and auto_rx, since they
    // aren't supported on the ne2000. Print a warning and continue
    if (value & 0x04)
      BX_INFO(("DCR write - LAS set ???"));
    if (value & 0x10)
      BX_INFO(("DCR write - AR set ???"));

    // Set other values.
    BX_NE2K_THIS s.DCR.wdsize   = ((value & 0x01) == 0x01);
    BX_NE2K_THIS s.DCR.endian   = ((value & 0x02) == 0x02);
    BX_NE2K_THIS s.DCR.longaddr = ((value & 0x04) == 0x04); // illegal ?
    BX_NE2K_THIS s.DCR.loop     = ((value & 0x08) == 0x08);
    BX_NE2K_THIS s.DCR.auto_rx  = ((value & 0x10) == 0x10); // also illegal ?
    BX_NE2K_THIS s.DCR.fifo_size = (value & 0x50) >> 5;
    break;

  case 0xf:  // IMR
    // Check for reserved bit
    if (value & 0x80)
      BX_ERROR(("IMR write, reserved bit set"));

    // Set other values
    BX_NE2K_THIS s.IMR.rx_inte    = ((value & 0x01) == 0x01);
    BX_NE2K_THIS s.IMR.tx_inte    = ((value & 0x02) == 0x02);
    BX_NE2K_THIS s.IMR.rxerr_inte = ((value & 0x04) == 0x04);
    BX_NE2K_THIS s.IMR.txerr_inte = ((value & 0x08) == 0x08);
    BX_NE2K_THIS s.IMR.overw_inte = ((value & 0x10) == 0x10);
    BX_NE2K_THIS s.IMR.cofl_inte  = ((value & 0x20) == 0x20);
    BX_NE2K_THIS s.IMR.rdma_inte  = ((value & 0x40) == 0x40);
    value2 = ((BX_NE2K_THIS s.ISR.rdma_done << 6) |
              (BX_NE2K_THIS s.ISR.cnt_oflow << 5) |
              (BX_NE2K_THIS s.ISR.overwrite << 4) |
              (BX_NE2K_THIS s.ISR.tx_err    << 3) |
              (BX_NE2K_THIS s.ISR.rx_err    << 2) |
              (BX_NE2K_THIS s.ISR.pkt_tx    << 1) |
              (BX_NE2K_THIS s.ISR.pkt_rx));
    if (((value & value2) & 0x7f) == 0) {
      set_irq_level(0);
    } else {
      set_irq_level(1);
    }
    break;

  default:
    BX_PANIC(("page 0 write, bad register 0x%02x", offset));
  }
}

//
// page1_read/page1_write - These routines handle reads/writes to
// the first page of the DS8390 register file
//
Bit32u bx_ne2k_c::page1_read(Bit32u offset, unsigned int io_len)
{
  BX_DEBUG(("page 1 read from register 0x%02x, len=%u", offset, io_len));

  if (io_len > 1)
    BX_PANIC(("bad length! page 1 read from register 0x%02x, len=%u", offset, io_len));

  switch (offset) {
  case 0x1:  // PAR0-5
  case 0x2:
  case 0x3:
  case 0x4:
  case 0x5:
  case 0x6:
    return (BX_NE2K_THIS s.physaddr[offset - 1]);
    break;

  case 0x7:  // CURR
    BX_DEBUG(("returning current page: 0x%02x", (BX_NE2K_THIS s.curr_page)));
    return (BX_NE2K_THIS s.curr_page);

  case 0x8:  // MAR0-7
  case 0x9:
  case 0xa:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    return (BX_NE2K_THIS s.mchash[offset - 8]);
    break;

  default:
    BX_PANIC(("page 1 read register 0x%02x out of range", offset));
  }

  return (0);
}

void bx_ne2k_c::page1_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_DEBUG(("page 1 write to register 0x%02x, len=%u, value=0x%04x", offset,
            io_len, value));

  switch (offset) {
  case 0x1:  // PAR0-5
  case 0x2:
  case 0x3:
  case 0x4:
  case 0x5:
  case 0x6:
    BX_NE2K_THIS s.physaddr[offset - 1] = value;
    if (offset == 6) {
      BX_INFO(("Physical address set to %02x:%02x:%02x:%02x:%02x:%02x",
               BX_NE2K_THIS s.physaddr[0],
               BX_NE2K_THIS s.physaddr[1],
               BX_NE2K_THIS s.physaddr[2],
               BX_NE2K_THIS s.physaddr[3],
               BX_NE2K_THIS s.physaddr[4],
               BX_NE2K_THIS s.physaddr[5]));
    }
    break;

  case 0x7:  // CURR
    BX_NE2K_THIS s.curr_page = value;
    break;

  case 0x8:  // MAR0-7
  case 0x9:
  case 0xa:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    BX_NE2K_THIS s.mchash[offset - 8] = value;
    break;

  default:
    BX_PANIC(("page 1 write register 0x%02x out of range", offset));
  }
}

//
// page2_read/page2_write - These routines handle reads/writes to
// the second page of the DS8390 register file
//
Bit32u bx_ne2k_c::page2_read(Bit32u offset, unsigned int io_len)
{
  BX_DEBUG(("page 2 read from register 0x%02x, len=%u", offset, io_len));

  if (io_len > 1)
    BX_PANIC(("bad length!  page 2 read from register 0x%02x, len=%u", offset, io_len));

  switch (offset) {
  case 0x1:  // PSTART
    return (BX_NE2K_THIS s.page_start);

  case 0x2:  // PSTOP
    return (BX_NE2K_THIS s.page_stop);

  case 0x3:  // Remote Next-packet pointer
    return (BX_NE2K_THIS s.rempkt_ptr);

  case 0x4:  // TPSR
    return (BX_NE2K_THIS s.tx_page_start);

  case 0x5:  // Local Next-packet pointer
    return (BX_NE2K_THIS s.localpkt_ptr);

  case 0x6:  // Address counter (upper)
    return (BX_NE2K_THIS s.address_cnt >> 8);

  case 0x7:  // Address counter (lower)
    return (BX_NE2K_THIS s.address_cnt & 0xff);

  case 0x8:  // Reserved
  case 0x9:
  case 0xa:
  case 0xb:
    BX_ERROR(("reserved read - page 2, register 0x%02x", offset));
    return (0xff);

  case 0xc:  // RCR
    return ((BX_NE2K_THIS s.RCR.monitor   << 5) |
	    (BX_NE2K_THIS s.RCR.promisc   << 4) |
	    (BX_NE2K_THIS s.RCR.multicast << 3) |
	    (BX_NE2K_THIS s.RCR.broadcast << 2) |
	    (BX_NE2K_THIS s.RCR.runts_ok  << 1) |
	    (BX_NE2K_THIS s.RCR.errors_ok));

  case 0xd:  // TCR
    return ((BX_NE2K_THIS s.TCR.coll_prio   << 4) |
	    (BX_NE2K_THIS s.TCR.ext_stoptx  << 3) |
	    ((BX_NE2K_THIS s.TCR.loop_cntl & 0x3) << 1) |
	    (BX_NE2K_THIS s.TCR.crc_disable));

  case 0xe:  // DCR
    return (((BX_NE2K_THIS s.DCR.fifo_size & 0x3) << 5) |
	    (BX_NE2K_THIS s.DCR.auto_rx  << 4) |
	    (BX_NE2K_THIS s.DCR.loop     << 3) |
	    (BX_NE2K_THIS s.DCR.longaddr << 2) |
	    (BX_NE2K_THIS s.DCR.endian   << 1) |
	    (BX_NE2K_THIS s.DCR.wdsize));

  case 0xf:  // IMR
    return ((BX_NE2K_THIS s.IMR.rdma_inte  << 6) |
	    (BX_NE2K_THIS s.IMR.cofl_inte  << 5) |
	    (BX_NE2K_THIS s.IMR.overw_inte << 4) |
	    (BX_NE2K_THIS s.IMR.txerr_inte << 3) |
	    (BX_NE2K_THIS s.IMR.rxerr_inte << 2) |
	    (BX_NE2K_THIS s.IMR.tx_inte    << 1) |
	    (BX_NE2K_THIS s.IMR.rx_inte));

  default:
    BX_PANIC(("page 2 register 0x%02x out of range", offset));
  }

  return (0);
}

void bx_ne2k_c::page2_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  // Maybe all writes here should be BX_PANIC()'d, since they
  // affect internal operation, but let them through for now
  // and print a warning.
  BX_ERROR(("page 2 write to register 0x%02x, len=%u, value=0x%04x", offset,
            io_len, value));

  switch (offset) {
  case 0x1:  // CLDA0
    // Clear out low byte and re-insert
    BX_NE2K_THIS s.local_dma &= 0xff00;
    BX_NE2K_THIS s.local_dma |= (value & 0xff);
    break;

  case 0x2:  // CLDA1
    // Clear out high byte and re-insert
    BX_NE2K_THIS s.local_dma &= 0x00ff;
    BX_NE2K_THIS s.local_dma |= ((value & 0xff) << 8);
    break;

  case 0x3:  // Remote Next-pkt pointer
    BX_NE2K_THIS s.rempkt_ptr = value;
    break;

  case 0x4:
    BX_PANIC(("page 2 write to reserved register 0x04"));
    break;

  case 0x5:  // Local Next-packet pointer
    BX_NE2K_THIS s.localpkt_ptr = value;
    break;

  case 0x6:  // Address counter (upper)
    // Clear out high byte and re-insert
    BX_NE2K_THIS s.address_cnt &= 0x00ff;
    BX_NE2K_THIS s.address_cnt |= ((value & 0xff) << 8);
    break;

  case 0x7:  // Address counter (lower)
    // Clear out low byte and re-insert
    BX_NE2K_THIS s.address_cnt &= 0xff00;
    BX_NE2K_THIS s.address_cnt |= (value & 0xff);
    break;

  case 0x8:
  case 0x9:
  case 0xa:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    BX_PANIC(("page 2 write to reserved register 0x%02x", offset));
    break;

  default:
    BX_PANIC(("page 2 write, illegal register 0x%02x", offset));
    break;
  }
}

//
// page3_read/page3_write - writes to this page are illegal
//
Bit32u bx_ne2k_c::page3_read(Bit32u offset, unsigned int io_len)
{
  if (BX_NE2K_THIS s.pci_enabled) {
    switch (offset) {
      case 0x3:  // CONFIG0
        return (0);
      case 0x5:  // CONFIG2
        return (0x40);
      case 0x6:  // CONFIG3
        return (0x40);
      default:
        BX_ERROR(("page 3 read register 0x%02x attempted", offset));
        return (0);
    }
  } else {
    BX_ERROR(("page 3 read register 0x%02x attempted", offset));
    return (0);
  }
}

void bx_ne2k_c::page3_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_ERROR(("page 3 write register 0x%02x attempted", offset));
}

//
// tx_timer_handler/tx_timer
//
void bx_ne2k_c::tx_timer_handler(void *this_ptr)
{
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;
  class_ptr->tx_timer();
}

void bx_ne2k_c::tx_timer(void)
{
  BX_DEBUG(("tx_timer"));
  BX_NE2K_THIS s.CR.tx_packet = 0;
  BX_NE2K_THIS s.TSR.tx_ok = 1;
  BX_NE2K_THIS s.ISR.pkt_tx = 1;
  // Generate an interrupt if not masked
  if (BX_NE2K_THIS s.IMR.tx_inte) {
    set_irq_level(1);
  }
  BX_NE2K_THIS s.tx_timer_active = 0;
}


#if BX_SUPPORT_PCI
bx_bool bx_ne2k_c::mem_read_handler(bx_phy_address addr, unsigned len,
                                    void *data, void *param)
{
  Bit8u  *data_ptr;

  Bit32u mask = (BX_NE2K_THIS pci_rom_size - 1);
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    if (BX_NE2K_THIS pci_conf[0x30] & 0x01) {
      *data_ptr = BX_NE2K_THIS pci_rom[addr & mask];
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

bx_bool bx_ne2k_c::mem_write_handler(bx_phy_address addr, unsigned len,
                                     void *data, void *param)
{
  BX_INFO(("write to ROM ignored (addr=0x%08x len=%d)", (Bit32u)addr, len));
  return 1;
}
#endif

//
// read_handler/read - i/o 'catcher' function called from BOCHS
// mainline when the CPU attempts a read in the i/o space registered
// by this ne2000 instance
//
Bit32u bx_ne2k_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_NE2K_SMF
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;

  return class_ptr->read(address, io_len);
}

Bit32u bx_ne2k_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_NE2K_SMF
  BX_DEBUG(("read addr %x, len %d", address, io_len));
  Bit32u retval = 0;
  int offset = address - BX_NE2K_THIS s.base_address;

  if (offset >= 0x10) {
    retval = asic_read(offset - 0x10, io_len);
  } else if (offset == 0x00) {
    retval = read_cr();
  } else {
    switch (BX_NE2K_THIS s.CR.pgsel) {
    case 0x00:
      retval = page0_read(offset, io_len);
      break;

    case 0x01:
      retval = page1_read(offset, io_len);
      break;

    case 0x02:
      retval = page2_read(offset, io_len);
      break;

    case 0x03:
      retval = page3_read(offset, io_len);
      break;

    default:
      BX_PANIC(("ne2K: unknown value of pgsel in read - %d",
	       BX_NE2K_THIS s.CR.pgsel));
    }
  }

  return (retval);
}

//
// write_handler/write - i/o 'catcher' function called from BOCHS
// mainline when the CPU attempts a write in the i/o space registered
// by this ne2000 instance
//
void bx_ne2k_c::write_handler(void *this_ptr, Bit32u address, Bit32u value,
			 unsigned io_len)
{
#if !BX_USE_NE2K_SMF
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_ne2k_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_NE2K_SMF
  BX_DEBUG(("write addr %x, value %x len %d", address, value, io_len));
  int offset = address - BX_NE2K_THIS s.base_address;

  //
  // The high 16 bytes of i/o space are for the ne2000 asic -
  //  the low 16 bytes are for the DS8390, with the current
  //  page being selected by the PS0,PS1 registers in the
  //  command register
  //
  if (offset >= 0x10) {
    asic_write(offset - 0x10, value, io_len);
  } else if (offset == 0x00) {
    write_cr(value);
  } else {
    switch (BX_NE2K_THIS s.CR.pgsel) {
    case 0x00:
      page0_write(offset, value, io_len);
      break;

    case 0x01:
      page1_write(offset, value, io_len);
      break;

    case 0x02:
      page2_write(offset, value, io_len);
      break;

    case 0x03:
      page3_write(offset, value, io_len);
      break;

    default:
      BX_PANIC(("ne2K: unknown value of pgsel in write - %d",
	       BX_NE2K_THIS s.CR.pgsel));
    }
  }
}


/*
 * mcast_index() - return the 6-bit index into the multicast
 * table. Stolen unashamedly from FreeBSD's if_ed.c
 */
unsigned bx_ne2k_c::mcast_index(const void *dst)
{
#define POLYNOMIAL 0x04c11db6
  Bit32u crc = 0xffffffffL;
  int carry, i, j;
  unsigned char b;
  unsigned char *ep = (unsigned char *) dst;

  for (i = 6; --i >= 0;) {
    b = *ep++;
    for (j = 8; --j >= 0;) {
      carry = ((crc & 0x80000000L) ? 1 : 0) ^ (b & 0x01);
      crc <<= 1;
      b >>= 1;
      if (carry)
	crc = ((crc ^ POLYNOMIAL) | carry);
    }
  }
  return (crc >> 26);
#undef POLYNOMIAL
}

/*
 * Callback from the eth system driver to check if the device can receive
 */
Bit32u bx_ne2k_c::rx_status_handler(void *arg)
{
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) arg;
  return class_ptr->rx_status();
}

Bit32u bx_ne2k_c::rx_status()
{
  Bit32u status = BX_NETDEV_10MBIT;
  if ((BX_NE2K_THIS s.CR.stop == 0) &&
      (BX_NE2K_THIS s.page_start != 0) &&
      ((BX_NE2K_THIS s.DCR.loop != 0) ||
       (BX_NE2K_THIS s.TCR.loop_cntl == 0))) {
    status |= BX_NETDEV_RXREADY;
  }
  return status;
}

/*
 * Callback from the eth system driver when a frame has arrived
 */
void bx_ne2k_c::rx_handler(void *arg, const void *buf, unsigned len)
{
    // BX_DEBUG(("rx_handler with length %d", len));
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) arg;

  class_ptr->rx_frame(buf, len);
}

/*
 * rx_frame() - called by the platform-specific code when an
 * ethernet frame has been received. The destination address
 * is tested to see if it should be accepted, and if the
 * rx ring has enough room, it is copied into it and
 * the receive process is updated
 */
void bx_ne2k_c::rx_frame(const void *buf, unsigned io_len)
{
  int pages;
  int avail;
  unsigned idx;
//int wrapped;
  int nextpage;
  unsigned char pkthdr[4];
  unsigned char *pktbuf = (unsigned char *) buf;
  unsigned char *startptr;

  BX_DEBUG(("rx_frame with length %d", io_len));


  if ((BX_NE2K_THIS s.CR.stop != 0) ||
      (BX_NE2K_THIS s.page_start == 0) ||
      ((BX_NE2K_THIS s.DCR.loop == 0) &&
       (BX_NE2K_THIS s.TCR.loop_cntl != 0))) {

    return;
  }

  // Add the pkt header + CRC to the length, and work
  // out how many 256-byte pages the frame would occupy
  pages = (io_len + 4 + 4 + 255)/256;

  if (BX_NE2K_THIS s.curr_page < BX_NE2K_THIS s.bound_ptr) {
    avail = BX_NE2K_THIS s.bound_ptr - BX_NE2K_THIS s.curr_page;
  } else {
    avail = (BX_NE2K_THIS s.page_stop - BX_NE2K_THIS s.page_start) -
      (BX_NE2K_THIS s.curr_page - BX_NE2K_THIS s.bound_ptr);
//  wrapped = 1;
  }

  // Avoid getting into a buffer overflow condition by not attempting
  // to do partial receives. The emulation to handle this condition
  // seems particularly painful.
  if ((avail < pages)
#if BX_NE2K_NEVER_FULL_RING
      || (avail == pages)
#endif
      ) {
    return;
  }

  if ((io_len < 60) && !BX_NE2K_THIS s.RCR.runts_ok) {
    BX_DEBUG(("rejected small packet, length %d", io_len));
    return;
  }

  // Do address filtering if not in promiscuous mode
  if (! BX_NE2K_THIS s.RCR.promisc) {
    if (!memcmp(buf, broadcast_macaddr, 6)) {
      if (!BX_NE2K_THIS s.RCR.broadcast) {
	return;
      }
    } else if (pktbuf[0] & 0x01) {
	if (! BX_NE2K_THIS s.RCR.multicast) {
	    return;
	}
      idx = mcast_index(buf);
      if (!(BX_NE2K_THIS s.mchash[idx >> 3] & (1 << (idx & 0x7)))) {
	return;
      }
    } else if (0 != memcmp(buf, BX_NE2K_THIS s.physaddr, 6)) {
      return;
    }
  } else {
      BX_DEBUG(("rx_frame promiscuous receive"));
  }

//    BX_INFO(("rx_frame %d to %x:%x:%x:%x:%x:%x from %x:%x:%x:%x:%x:%x",
//  	   io_len,
//  	   pktbuf[0], pktbuf[1], pktbuf[2], pktbuf[3], pktbuf[4], pktbuf[5],
//  	   pktbuf[6], pktbuf[7], pktbuf[8], pktbuf[9], pktbuf[10], pktbuf[11]));

  nextpage = BX_NE2K_THIS s.curr_page + pages;
  if (nextpage >= BX_NE2K_THIS s.page_stop) {
    nextpage -= BX_NE2K_THIS s.page_stop - BX_NE2K_THIS s.page_start;
  }

  // Setup packet header
  pkthdr[0] = 0;	// rx status - old behavior
  pkthdr[0] = 1;        // Probably better to set it all the time
                        // rather than set it to 0, which is clearly wrong.
  if (pktbuf[0] & 0x01) {
    pkthdr[0] |= 0x20;  // rx status += multicast packet
  }
  pkthdr[1] = nextpage;	// ptr to next packet
  pkthdr[2] = (io_len + 4) & 0xff;	// length-low
  pkthdr[3] = (io_len + 4) >> 8;	// length-hi

  // copy into buffer, update curpage, and signal interrupt if config'd
  startptr = & BX_NE2K_THIS s.mem[BX_NE2K_THIS s.curr_page * 256 -
			       BX_NE2K_MEMSTART];
  if ((nextpage > BX_NE2K_THIS s.curr_page) ||
      ((BX_NE2K_THIS s.curr_page + pages) == BX_NE2K_THIS s.page_stop)) {
    memcpy(startptr, pkthdr, 4);
    memcpy(startptr + 4, buf, io_len);
    BX_NE2K_THIS s.curr_page = nextpage;
  } else {
    int endbytes = (BX_NE2K_THIS s.page_stop - BX_NE2K_THIS s.curr_page)
      * 256;
    memcpy(startptr, pkthdr, 4);
    memcpy(startptr + 4, buf, endbytes - 4);
    startptr = & BX_NE2K_THIS s.mem[BX_NE2K_THIS s.page_start * 256 -
				 BX_NE2K_MEMSTART];
    memcpy(startptr, (void *)(pktbuf + endbytes - 4),
	   io_len - endbytes + 8);
    BX_NE2K_THIS s.curr_page = nextpage;
  }

  BX_NE2K_THIS s.RSR.rx_ok = 1;
  BX_NE2K_THIS s.RSR.rx_mbit = (bx_bool)((pktbuf[0] & 0x01) > 0);

  BX_NE2K_THIS s.ISR.pkt_rx = 1;

  if (BX_NE2K_THIS s.IMR.rx_inte) {
    set_irq_level(1);
  }

  bx_gui->statusbar_setitem(BX_NE2K_THIS s.statusbar_id, 1);
}

void bx_ne2k_c::set_irq_level(bx_bool level)
{
  if (BX_NE2K_THIS s.pci_enabled) {
#if BX_SUPPORT_PCI
    DEV_pci_set_irq(BX_NE2K_THIS s.devfunc, BX_NE2K_THIS pci_conf[0x3d], level);
#endif
  } else {
    if (level) {
      DEV_pic_raise_irq(BX_NE2K_THIS s.base_irq);
    } else {
      DEV_pic_lower_irq(BX_NE2K_THIS s.base_irq);
    }
  }
}

#if BX_SUPPORT_PCI

// pci configuration space read callback handler
Bit32u bx_ne2k_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_NE2K_THIS pci_conf[address+i] << (i*8));
  }
  BX_DEBUG(("NE2000 PCI NIC read  register 0x%02x value 0x%08x", address, value));
  return value;
}

// pci configuration space write callback handler
void bx_ne2k_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;
  bx_bool romaddr_change = 0;

  if ((address > 0x13) && (address < 0x30))
    return;

  for (unsigned i=0; i<io_len; i++) {
    oldval = BX_NE2K_THIS pci_conf[address+i];
    value8 = (value >> (i*8)) & 0xFF;
    switch (address+i) {
      case 0x04:
        value8 &= 0x03;
        break;
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
        }
        break;
      case 0x10:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x11:
      case 0x12:
      case 0x13:
        baseaddr_change |= (value8 != oldval);
        break;
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
        if (BX_NE2K_THIS pci_rom_size > 0) {
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
    BX_NE2K_THIS pci_conf[address+i] = value8;
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_io(BX_NE2K_THIS_PTR, read_handler, write_handler,
                            &BX_NE2K_THIS s.base_address,
                            &BX_NE2K_THIS pci_conf[0x10],
                            32, &ne2k_iomask[0], "NE2000 PCI NIC")) {
      BX_INFO(("new base address: 0x%04x", BX_NE2K_THIS s.base_address));
    }
  }
  if (romaddr_change) {
    if (DEV_pci_set_base_mem(BX_NE2K_THIS_PTR, mem_read_handler,
                             mem_write_handler,
                             &BX_NE2K_THIS pci_rom_address,
                             &BX_NE2K_THIS pci_conf[0x30],
                             BX_NE2K_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_NE2K_THIS pci_rom_address));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

#endif /* BX_SUPPORT_PCI */

#if BX_DEBUGGER
void bx_ne2k_c::debug_dump(int argc, char **argv)
{
  int page = -1, reg = -1;

  for (int arg = 0; arg < argc; arg++) {
    if (!strncmp(argv[arg], "page=", 5) && isdigit(argv[arg][5])) {
      page = atoi(&argv[arg][5]);
    } else if (!strncmp(argv[arg], "reg=", 4) && isdigit(argv[arg][4])) {
      reg = atoi(&argv[arg][4]);
    } else {
      dbg_printf("\nUnknown option: '%s'\n", argv[arg]);
      return;
    }
  }
  BX_NE2K_THIS print_info(page, reg, 0);
}

/*
 * this implements the info device 'ne2k' command in the debugger.
 * info device 'ne2k' - shows all registers
 * info device 'ne2k' 'page=N' - shows all registers in a page
 * info device 'ne2k' 'page=N,reg=M' - shows just one register
 */

#define SHOW_FIELD(reg,field) do { \
  if (n>0 && !(n%5)) dbg_printf ("\n  "); \
  dbg_printf ("%s=%d ", #field, BX_NE2K_THIS s.reg.field); \
  n++; \
} while (0);
#define BX_HIGH_BYTE(x) ((0xff00 & (x)) >> 8)
#define BX_LOW_BYTE(x) (0x00ff & (x))
#define BX_DUPLICATE(n) if (brief && num!=n) break;

void bx_ne2k_c::print_info(int page, int reg, int brief)
{
  int i;
  int n = 0;
  if (page < 0) {
    for (page=0; page<=2; page++)
      theNE2kDevice->print_info(page, reg, 1);
    // tell them how to use this command
    dbg_printf("\nSupported options:\n");
    dbg_printf("info device 'ne2k' 'page=N' - show registers in page N\n");
    dbg_printf("info device 'ne2k' 'page=N,reg=M' - show just one register\n");
    return;
  }
  if (page > 2) {
    dbg_printf("NE2K has only pages 0, 1, and 2.  Page %d is out of range.\n", page);
    return;
  }
  if (reg < 0) {
    dbg_printf("NE2K registers, page %d\n", page);
    dbg_printf("----------------------\n");
    for (reg=0; reg<=15; reg++)
      theNE2kDevice->print_info(page, reg, 1);
    dbg_printf("----------------------\n");
    return;
  }
  if (reg > 15) {
    dbg_printf("NE2K has only registers 0-15 (0x0-0xf).  Register %d is out of range.\n", reg);
    return;
  }
  if (!brief) {
    dbg_printf("NE2K Info - page %d, register 0x%02x\n", page, reg);
    dbg_printf("----------------------------------\n");
  }
  int num = page*0x100 + reg;
  switch (num) {
    case 0x0000:
    case 0x0100:
    case 0x0200:
      dbg_printf ("CR (Command register):\n  ");
      SHOW_FIELD(CR, stop);
      SHOW_FIELD(CR, start);
      SHOW_FIELD(CR, tx_packet);
      SHOW_FIELD(CR, rdma_cmd);
      SHOW_FIELD(CR, pgsel);
      dbg_printf("\n");
      break;
    case 0x0003:
      dbg_printf("BNRY = Boundary Pointer = 0x%02x\n", BX_NE2K_THIS s.bound_ptr);
      break;
    case 0x0004:
      dbg_printf("TSR (Transmit Status Register), read-only:\n  ");
      SHOW_FIELD(TSR, tx_ok);
      SHOW_FIELD(TSR, reserved);
      SHOW_FIELD(TSR, collided);
      SHOW_FIELD(TSR, aborted);
      SHOW_FIELD(TSR, no_carrier);
      SHOW_FIELD(TSR, fifo_ur);
      SHOW_FIELD(TSR, cd_hbeat);
      SHOW_FIELD(TSR, ow_coll);
      dbg_printf("\n");
      // fall through into TPSR, no break line.
    case 0x0204:
      dbg_printf("TPSR = Transmit Page Start = 0x%02x\n", BX_NE2K_THIS s.tx_page_start);
      break;
    case 0x0005:
    case 0x0006:  BX_DUPLICATE(0x0005);
      dbg_printf("NCR = Number of Collisions Register (read-only) = 0x%02x\n", BX_NE2K_THIS s.num_coll);
      dbg_printf("TBCR1,TBCR0 = Transmit Byte Count = %02x %02x\n",
          BX_HIGH_BYTE(BX_NE2K_THIS s.tx_bytes),
          BX_LOW_BYTE(BX_NE2K_THIS s.tx_bytes));
      dbg_printf("FIFO = %02x\n", BX_NE2K_THIS s.fifo);
      break;
    case 0x0007:
      dbg_printf("ISR (Interrupt Status Register):\n  ");
      SHOW_FIELD(ISR, pkt_rx);
      SHOW_FIELD(ISR, pkt_tx);
      SHOW_FIELD(ISR, rx_err);
      SHOW_FIELD(ISR, tx_err);
      SHOW_FIELD(ISR, overwrite);
      SHOW_FIELD(ISR, cnt_oflow);
      SHOW_FIELD(ISR, rdma_done);
      SHOW_FIELD(ISR, reset);
      dbg_printf("\n");
      break;
    case 0x0008:
    case 0x0009:  BX_DUPLICATE(0x0008);
      dbg_printf("CRDA1,0 = Current remote DMA address = %02x %02x\n",
          BX_HIGH_BYTE(BX_NE2K_THIS s.remote_dma),
          BX_LOW_BYTE(BX_NE2K_THIS s.remote_dma));
      dbg_printf("RSAR1,0 = Remote start address = %02x %02x\n",
          BX_HIGH_BYTE(s.remote_start),
          BX_LOW_BYTE(s.remote_start));
      break;
    case 0x000a:
    case 0x000b:  BX_DUPLICATE(0x000a);
      dbg_printf("RCBR1,0 = Remote byte count = %02x\n", BX_NE2K_THIS s.remote_bytes);
      break;
    case 0x000c:
      dbg_printf("RSR (Receive Status Register), read-only:\n  ");
      SHOW_FIELD(RSR, rx_ok);
      SHOW_FIELD(RSR, bad_crc);
      SHOW_FIELD(RSR, bad_falign);
      SHOW_FIELD(RSR, fifo_or);
      SHOW_FIELD(RSR, rx_missed);
      SHOW_FIELD(RSR, rx_mbit);
      SHOW_FIELD(RSR, rx_disabled);
      SHOW_FIELD(RSR, deferred);
      dbg_printf("\n");
      // fall through into RCR
    case 0x020c:
      dbg_printf("RCR (Receive Configuration Register):\n  ");
      SHOW_FIELD(RCR, errors_ok);
      SHOW_FIELD(RCR, runts_ok);
      SHOW_FIELD(RCR, broadcast);
      SHOW_FIELD(RCR, multicast);
      SHOW_FIELD(RCR, promisc);
      SHOW_FIELD(RCR, monitor);
      SHOW_FIELD(RCR, reserved);
      dbg_printf("\n");
      break;
    case 0x000d:
      dbg_printf("CNTR0 = Tally Counter 0 (Frame alignment errors) = %02x\n",
        BX_NE2K_THIS s.tallycnt_0);
      // fall through into TCR
    case 0x020d:
      dbg_printf("TCR (Transmit Configuration Register):\n  ");
      SHOW_FIELD(TCR, crc_disable);
      SHOW_FIELD(TCR, loop_cntl);
      SHOW_FIELD(TCR, ext_stoptx);
      SHOW_FIELD(TCR, coll_prio);
      SHOW_FIELD(TCR, reserved);
      dbg_printf("\n");
      break;
    case 0x000e:
      dbg_printf("CNTR1 = Tally Counter 1 (CRC Errors) = %02x\n",
        BX_NE2K_THIS s.tallycnt_1);
      // fall through into DCR
    case 0x020e:
      dbg_printf("DCR (Data Configuration Register):\n  ");
      SHOW_FIELD(DCR, wdsize);
      SHOW_FIELD(DCR, endian);
      SHOW_FIELD(DCR, longaddr);
      SHOW_FIELD(DCR, loop);
      SHOW_FIELD(DCR, auto_rx);
      SHOW_FIELD(DCR, fifo_size);
      dbg_printf("\n");
      break;
    case 0x000f:
      dbg_printf("CNTR2 = Tally Counter 2 (Missed Packet Errors) = %02x\n",
        BX_NE2K_THIS s.tallycnt_2);
      // fall through into IMR
    case 0x020f:
      dbg_printf("IMR (Interrupt Mask Register)\n  ");
      SHOW_FIELD(IMR, rx_inte);
      SHOW_FIELD(IMR, tx_inte);
      SHOW_FIELD(IMR, rxerr_inte);
      SHOW_FIELD(IMR, txerr_inte);
      SHOW_FIELD(IMR, overw_inte);
      SHOW_FIELD(IMR, cofl_inte);
      SHOW_FIELD(IMR, rdma_inte);
      SHOW_FIELD(IMR, reserved);
      dbg_printf("\n");
      break;
    case 0x0101:
    case 0x0102:  BX_DUPLICATE(0x0101);
    case 0x0103:  BX_DUPLICATE(0x0101);
    case 0x0104:  BX_DUPLICATE(0x0101);
    case 0x0105:  BX_DUPLICATE(0x0101);
    case 0x0106:  BX_DUPLICATE(0x0101);
      dbg_printf("MAC address registers are located at page 1, registers 1-6.\n");
      dbg_printf("The MAC address is ");
      for (i=0; i<=5; i++)
        dbg_printf("%02x%c", BX_NE2K_THIS s.physaddr[i], i<5?':' : '\n');
      break;
    case 0x0107:
      dbg_printf("Current page is 0x%02x\n", BX_NE2K_THIS s.curr_page);
      break;
    case 0x0108:
    case 0x0109:  BX_DUPLICATE(0x0108);
    case 0x010A:  BX_DUPLICATE(0x0108);
    case 0x010B:  BX_DUPLICATE(0x0108);
    case 0x010C:  BX_DUPLICATE(0x0108);
    case 0x010D:  BX_DUPLICATE(0x0108);
    case 0x010E:  BX_DUPLICATE(0x0108);
    case 0x010F:  BX_DUPLICATE(0x0108);
      dbg_printf("MAR0-7 (Multicast address registers 0-7) are set to:\n");
      for (i=0; i<8; i++) dbg_printf("%02x ", BX_NE2K_THIS s.mchash[i]);
      dbg_printf("\nMAR0 is listed first.\n");
      break;
    case 0x0001:
    case 0x0002:  BX_DUPLICATE(0x0001);
    case 0x0201:  BX_DUPLICATE(0x0001);
    case 0x0202:  BX_DUPLICATE(0x0001);
      dbg_printf("PSTART = Page start register = %02x\n", BX_NE2K_THIS s.page_start);
      dbg_printf("PSTOP = Page stop register = %02x\n", BX_NE2K_THIS s.page_stop);
      dbg_printf("Local DMA address = %02x %02x\n",
          BX_HIGH_BYTE(BX_NE2K_THIS s.local_dma),
          BX_LOW_BYTE(BX_NE2K_THIS s.local_dma));
      break;
    case 0x0203:
      dbg_printf("Remote Next Packet Pointer = %02x\n", BX_NE2K_THIS s.rempkt_ptr);
      break;
    case 0x0205:
      dbg_printf("Local Next Packet Pointer = %02x\n", BX_NE2K_THIS s.localpkt_ptr);
      break;
    case 0x0206:
    case 0x0207:  BX_DUPLICATE(0x0206);
      dbg_printf("Address Counter= %02x %02x\n",
         BX_HIGH_BYTE(BX_NE2K_THIS s.address_cnt),
         BX_LOW_BYTE(BX_NE2K_THIS s.address_cnt));
      break;
    case 0x0208:
    case 0x0209:  BX_DUPLICATE(0x0208);
    case 0x020A:  BX_DUPLICATE(0x0208);
    case 0x020B:  BX_DUPLICATE(0x0208);
      if (!brief) dbg_printf ("Reserved\n");
    case 0xffff:
      dbg_printf("IMR (Interrupt Mask Register):\n  ");
      dbg_printf("\n");
      break;
    default:
      dbg_printf("NE2K info: sorry, page %d register %d cannot be displayed.\n", page, reg);
  }
  if (!brief)
    dbg_printf("\n");
}
#endif

#endif /* if BX_SUPPORT_NE2K */
