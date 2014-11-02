/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Intel(R) 82540EM Gigabit Ethernet support (ported from QEMU)
//  Software developer's manual:
//  http://download.intel.com/design/network/manuals/8254x_GBe_SDM.pdf
//
//  Nir Peleg, Tutis Systems Ltd. for Qumranet Inc.
//  Copyright (c) 2008 Qumranet
//  Based on work done by:
//  Copyright (c) 2007 Dan Aloni
//  Copyright (c) 2004 Antony T Curtis
//
//  Copyright (C) 2011-2014  The Bochs Project
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
#if BX_SUPPORT_PCI && BX_SUPPORT_E1000

#include "pci.h"
#include "netmod.h"
#include "e1000.h"

#define LOG_THIS theE1000Device->

bx_e1000_c* theE1000Device = NULL;

const Bit8u e1000_iomask[64] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

#define E1000_CTRL     0x00000  // Device Control - RW
#define E1000_STATUS   0x00008  // Device Status - RO
#define E1000_EECD     0x00010  // EEPROM/Flash Control - RW
#define E1000_EERD     0x00014  // EEPROM Read - RW
#define E1000_MDIC     0x00020  // MDI Control - RW
#define E1000_VET      0x00038  // VLAN Ether Type - RW
#define E1000_ICR      0x000C0  // Interrupt Cause Read - R/clr
#define E1000_ICS      0x000C8  // Interrupt Cause Set - WO
#define E1000_IMS      0x000D0  // Interrupt Mask Set - RW
#define E1000_IMC      0x000D8  // Interrupt Mask Clear - WO
#define E1000_RCTL     0x00100  // RX Control - RW
#define E1000_TCTL     0x00400  // TX Control - RW
#define E1000_LEDCTL   0x00E00  // LED Control - RW
#define E1000_PBA      0x01000  // Packet Buffer Allocation - RW
#define E1000_RDBAL    0x02800  // RX Descriptor Base Address Low - RW
#define E1000_RDBAH    0x02804  // RX Descriptor Base Address High - RW
#define E1000_RDLEN    0x02808  // RX Descriptor Length - RW
#define E1000_RDH      0x02810  // RX Descriptor Head - RW
#define E1000_RDT      0x02818  // RX Descriptor Tail - RW
#define E1000_TDBAL    0x03800  // TX Descriptor Base Address Low - RW
#define E1000_TDBAH    0x03804  // TX Descriptor Base Address High - RW
#define E1000_TDLEN    0x03808  // TX Descriptor Length - RW
#define E1000_TDH      0x03810  // TX Descriptor Head - RW
#define E1000_TDT      0x03818  // TX Descripotr Tail - RW
#define E1000_TXDCTL   0x03828  // TX Descriptor Control - RW
#define E1000_CRCERRS  0x04000  // CRC Error Count - R/clr
#define E1000_MPC      0x04010  // Missed Packet Count - R/clr
#define E1000_GPRC     0x04074  // Good Packets RX Count - R/clr
#define E1000_GPTC     0x04080  // Good Packets TX Count - R/clr
#define E1000_TORL     0x040C0  // Total Octets RX Low - R/clr
#define E1000_TORH     0x040C4  // Total Octets RX High - R/clr
#define E1000_TOTL     0x040C8  // Total Octets TX Low - R/clr
#define E1000_TOTH     0x040CC  // Total Octets TX High - R/clr
#define E1000_TPR      0x040D0  // Total Packets RX - R/clr
#define E1000_TPT      0x040D4  // Total Packets TX - R/clr
#define E1000_MTA      0x05200  // Multicast Table Array - RW Array
#define E1000_RA       0x05400  // Receive Address - RW Array
#define E1000_VFTA     0x05600  // VLAN Filter Table Array - RW Array
#define E1000_WUFC     0x05808  // Wakeup Filter Control - RW
#define E1000_MANC     0x05820  // Management Control - RW
#define E1000_SWSM     0x05B50  // SW Semaphore

#define PHY_CTRL         0x00 // Control Register
#define PHY_STATUS       0x01 // Status Regiser
#define PHY_ID1          0x02 // Phy Id Reg (word 1)
#define PHY_ID2          0x03 // Phy Id Reg (word 2)
#define PHY_AUTONEG_ADV  0x04 // Autoneg Advertisement
#define PHY_LP_ABILITY   0x05 // Link Partner Ability (Base Page)
#define PHY_1000T_CTRL   0x09 // 1000Base-T Control Reg
#define PHY_1000T_STATUS 0x0A // 1000Base-T Status Reg

#define M88E1000_PHY_SPEC_CTRL     0x10  // PHY Specific Control Register
#define M88E1000_PHY_SPEC_STATUS   0x11  // PHY Specific Status Register
#define M88E1000_EXT_PHY_SPEC_CTRL 0x14  // Extended PHY Specific Control

#define E1000_ICR_TXDW          0x00000001 // Transmit desc written back
#define E1000_ICR_TXQE          0x00000002 // Transmit Queue empty
#define E1000_ICR_RXDMT0        0x00000010 // rx desc min. threshold (0)
#define E1000_ICR_RXO           0x00000040 // rx overrun
#define E1000_ICR_RXT0          0x00000080 // rx timer intr (ring 0)
#define E1000_ICR_MDAC          0x00000200 // MDIO access complete
#define E1000_ICR_INT_ASSERTED  0x80000000 // If this bit asserted, the driver should claim the interrupt

#define E1000_ICS_RXDMT0    E1000_ICR_RXDMT0    // rx desc min. threshold
#define E1000_ICS_RXO       E1000_ICR_RXO       // rx overrun
#define E1000_ICS_RXT0      E1000_ICR_RXT0      // rx timer intr
#define E1000_ICS_TXQE      E1000_ICR_TXQE      // Transmit Queue empty

#define E1000_RCTL_EN             0x00000002    // enable
#define E1000_RCTL_UPE            0x00000008    // unicast promiscuous enable
#define E1000_RCTL_MPE            0x00000010    // multicast promiscuous enab
#define E1000_RCTL_RDMTS_QUAT     0x00000100    // rx desc min threshold size
#define E1000_RCTL_MO_SHIFT       12            // multicast offset shift
#define E1000_RCTL_BAM            0x00008000    // broadcast enable
// these buffer sizes are valid if E1000_RCTL_BSEX is 0
#define E1000_RCTL_SZ_2048        0x00000000    // rx buffer size 2048
#define E1000_RCTL_SZ_1024        0x00010000    // rx buffer size 1024
#define E1000_RCTL_SZ_512         0x00020000    // rx buffer size 512
#define E1000_RCTL_SZ_256         0x00030000    // rx buffer size 256
// these buffer sizes are valid if E1000_RCTL_BSEX is 1
#define E1000_RCTL_SZ_16384       0x00010000    // rx buffer size 16384
#define E1000_RCTL_SZ_8192        0x00020000    // rx buffer size 8192
#define E1000_RCTL_SZ_4096        0x00030000    // rx buffer size 4096
#define E1000_RCTL_VFE            0x00040000    // vlan filter enable
#define E1000_RCTL_BSEX           0x02000000    // Buffer size extension
#define E1000_RCTL_SECRC          0x04000000    // Strip Ethernet CRC

#define E1000_EEPROM_RW_REG_DATA   16   // Offset to data in EEPROM read/write registers
#define E1000_EEPROM_RW_REG_DONE   0x10 // Offset to READ/WRITE done bit
#define E1000_EEPROM_RW_REG_START  1    // First bit for telling part to start operation
#define E1000_EEPROM_RW_ADDR_SHIFT 8    // Shift to the address bits

#define E1000_CTRL_SLU      0x00000040  // Set link up (Force Link)
#define E1000_CTRL_SPD_1000 0x00000200  // Force 1Gb
#define E1000_CTRL_SWDPIN0  0x00040000  // SWDPIN 0 value
#define E1000_CTRL_SWDPIN2  0x00100000  // SWDPIN 2 value
#define E1000_CTRL_RST      0x04000000  // Global reset
#define E1000_CTRL_VME      0x40000000  // IEEE VLAN mode enable

#define E1000_STATUS_FD         0x00000001        // Full duplex.0=half,1=full
#define E1000_STATUS_LU         0x00000002        // Link up.0=no,1=link
#define E1000_STATUS_SPEED_1000 0x00000080        // Speed 1000Mb/s
#define E1000_STATUS_ASDV       0x00000300        // Auto speed detect value
#define E1000_STATUS_GIO_MASTER_ENABLE 0x00080000 // Status of Master requests
#define E1000_STATUS_MTXCKOK    0x00000400        // MTX clock running OK

#define E1000_EECD_SK        0x00000001 // EEPROM Clock
#define E1000_EECD_CS        0x00000002 // EEPROM Chip Select
#define E1000_EECD_DI        0x00000004 // EEPROM Data In
#define E1000_EECD_DO        0x00000008 // EEPROM Data Out
#define E1000_EECD_FWE_MASK  0x00000030
#define E1000_EECD_REQ       0x00000040 // EEPROM Access Request
#define E1000_EECD_GNT       0x00000080 // EEPROM Access Grant
#define E1000_EECD_PRES      0x00000100 // EEPROM Present

#define E1000_MDIC_DATA_MASK 0x0000FFFF
#define E1000_MDIC_REG_MASK  0x001F0000
#define E1000_MDIC_REG_SHIFT 16
#define E1000_MDIC_PHY_MASK  0x03E00000
#define E1000_MDIC_PHY_SHIFT 21
#define E1000_MDIC_OP_WRITE  0x04000000
#define E1000_MDIC_OP_READ   0x08000000
#define E1000_MDIC_READY     0x10000000
#define E1000_MDIC_INT_EN    0x20000000
#define E1000_MDIC_ERROR     0x40000000

#define EEPROM_READ_OPCODE_MICROWIRE  0x6  // EEPROM read opcode

#define E1000_TXD_DTYP_D     0x00100000 // Data Descriptor
#define E1000_TXD_POPTS_IXSM 0x01       // Insert IP checksum
#define E1000_TXD_POPTS_TXSM 0x02       // Insert TCP/UDP checksum
#define E1000_TXD_CMD_RS     0x08000000 // Report Status
#define E1000_TXD_CMD_RPS    0x10000000 // Report Packet Sent
#define E1000_TXD_CMD_VLE    0x40000000 // Add VLAN tag
#define E1000_TXD_CMD_DEXT   0x20000000 // Descriptor extension (0 = legacy)
#define E1000_TXD_STAT_DD    0x00000001 // Descriptor Done
#define E1000_TXD_STAT_EC    0x00000002 // Excess Collisions
#define E1000_TXD_STAT_LC    0x00000004 // Late Collisions
#define E1000_TXD_STAT_TU    0x00000008 // Transmit underrun
#define E1000_TXD_CMD_EOP    0x01000000 // End of Packet
#define E1000_TXD_CMD_TCP    0x01000000 // TCP packet
#define E1000_TXD_CMD_IP     0x02000000 // IP packet
#define E1000_TXD_CMD_TSE    0x04000000 // TCP Seg enable

#define E1000_TCTL_EN     0x00000002    // enable tx

struct e1000_rx_desc {
  Bit64u buffer_addr; // Address of the descriptor's data buffer
  Bit16u length;      // Length of data DMAed into data buffer
  Bit16u csum;       // Packet checksum
  Bit8u status;      // Descriptor status
  Bit8u errors;      // Descriptor Errors
  Bit16u special;
};

#define E1000_RXD_STAT_DD       0x01    // Descriptor Done
#define E1000_RXD_STAT_EOP      0x02    // End of Packet
#define E1000_RXD_STAT_IXSM     0x04    // Ignore checksum
#define E1000_RXD_STAT_VP       0x08    // IEEE VLAN Packet

#define E1000_RAH_AV  0x80000000 // Receive descriptor valid

struct e1000_context_desc {
  union {
    Bit32u ip_config;
    struct {
      Bit8u ipcss;      // IP checksum start */
      Bit8u ipcso;      // IP checksum offset */
      Bit16u ipcse;     // IP checksum end */
    } ip_fields;
  } lower_setup;
  union {
    Bit32u tcp_config;
    struct {
      Bit8u tucss;      // TCP checksum start */
      Bit8u tucso;      // TCP checksum offset */
      Bit16u tucse;     // TCP checksum end */
    } tcp_fields;
  } upper_setup;
  Bit32u cmd_and_length;
  union {
    Bit32u data;
    struct {
      Bit8u status;     // Descriptor status */
      Bit8u hdr_len;    // Header length */
      Bit16u mss;       // Maximum segment size */
    } fields;
  } tcp_seg_setup;
};

#define E1000_MANC_RMCP_EN       0x00000100 // Enable RCMP 026Fh Filtering
#define E1000_MANC_0298_EN       0x00000200 // Enable RCMP 0298h Filtering
#define E1000_MANC_ARP_EN        0x00002000 // Enable ARP Request Filtering
#define E1000_MANC_RCV_TCO_EN    0x00020000 // Receive TCO Packets Enabled
#define E1000_MANC_EN_MNG2HOST   0x00200000 // Enable MNG packets to host memory

#define EEPROM_CHECKSUM_REG 0x3f
#define EEPROM_SUM          0xbaba

#define MIN_BUF_SIZE 60

#define	defreg(x) x = (E1000_##x>>2)
enum {
  defreg(CTRL),  defreg(EECD),  defreg(EERD),   defreg(GPRC),
  defreg(GPTC),  defreg(ICR),   defreg(ICS),    defreg(IMC),
  defreg(IMS),   defreg(LEDCTL),defreg(MANC),   defreg(MDIC),
  defreg(MPC),   defreg(PBA),   defreg(RCTL),   defreg(RDBAH),
  defreg(RDBAL), defreg(RDH),   defreg(RDLEN),  defreg(RDT),
  defreg(STATUS),defreg(SWSM),  defreg(TCTL),   defreg(TDBAH),
  defreg(TDBAL), defreg(TDH),   defreg(TDLEN),  defreg(TDT),
  defreg(TORH),  defreg(TORL),  defreg(TOTH),   defreg(TOTL),
  defreg(TPR),   defreg(TPT),   defreg(TXDCTL), defreg(WUFC),
  defreg(RA),    defreg(MTA),   defreg(CRCERRS),defreg(VFTA),
  defreg(VET),
};

enum { PHY_R = 1, PHY_W = 2, PHY_RW = PHY_R | PHY_W };
static const char phy_regcap[0x20] = {
  PHY_RW, PHY_R,  PHY_R,  PHY_R,  PHY_RW, PHY_R,  0,      0,
  0,      PHY_RW, PHY_R,  0,      0,      0,      0,      0,
  PHY_RW, PHY_R,  0,      0,      PHY_RW, PHY_R,  0,      0,
  0,      0,      0,      0,      0,      0,      0,      0
};

static const Bit16u e1000_eeprom_template[64] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
  0x3000, 0x1000, 0x6403, 0x100e, 0x8086, 0x100e, 0x8086, 0x3040,
  0x0008, 0x2000, 0x7e14, 0x0048, 0x1000, 0x00d8, 0x0000, 0x2700,
  0x6cc9, 0x3150, 0x0722, 0x040b, 0x0984, 0x0000, 0xc000, 0x0706,
  0x1008, 0x0000, 0x0f04, 0x7fff, 0x4d01, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
  0x0100, 0x4000, 0x121c, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000,
};

// builtin configuration handling functions

void e1000_init_options(void)
{
  bx_param_c *network = SIM->get_param("network");
  bx_list_c *menu = new bx_list_c(network, "e1000", "Intel(R) Gigabit Ethernet");
  menu->set_options(menu->SHOW_PARENT);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable Intel(R) Gigabit Ethernet emulation",
    "Enables the Intel(R) Gigabit Ethernet emulation",
    1);
  SIM->init_std_nic_options("Intel(R) Gigabit Ethernet", menu);
  enabled->set_dependent_list(menu->clone());
}

Bit32s e1000_options_parser(const char *context, int num_params, char *params[])
{
  int ret, valid = 0;

  if (!strcmp(params[0], "e1000")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_E1000);
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
        BX_PANIC(("%s: 'e1000' directive incomplete (mac is required)", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s e1000_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_E1000), NULL, 0);
}

// device plugin entry points

int CDECL libe1000_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theE1000Device = new bx_e1000_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theE1000Device, BX_PLUGIN_E1000);
  // add new configuration parameter for the config interface
  e1000_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("e1000", e1000_options_parser, e1000_options_save);
  return 0; // Success
}

void CDECL libe1000_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("e1000");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("network");
  menu->remove("e1000");
  delete theE1000Device;
}

// macros and helper functions

#if defined (BX_LITTLE_ENDIAN)
#define cpu_to_le16(val) (val)
#define cpu_to_le32(val) (val)
#define cpu_to_le64(val) (val)
#else
#define cpu_to_le16(val) bx_bswap16(val)
#define cpu_to_le32(val) bx_bswap32(val)
#define cpu_to_le64(val) bx_bswap64(val)
#endif

#define le16_to_cpu  cpu_to_le16
#define le32_to_cpu  cpu_to_le32
#define le64_to_cpu  cpu_to_le64

Bit32u net_checksum_add(Bit8u *buf, unsigned buf_len)
{
  Bit32u sum = 0;
  unsigned i;

  for (i = 0; i < buf_len; i++) {
    if (i & 1)
      sum += (Bit32u)buf[i];
    else
      sum += (Bit32u)buf[i] << 8;
  }
  return sum;
}

Bit16u net_checksum_finish(Bit32u sum)
{
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return ~sum;
}


// the device object

bx_e1000_c::bx_e1000_c()
{
  put("E1000");
  memset(&s, 0, sizeof(bx_e1000_t));
  s.tx_timer_index = BX_NULL_TIMER_HANDLE;
  ethdev = NULL;
}

bx_e1000_c::~bx_e1000_c()
{
  if (s.mac_reg != NULL) {
    delete [] s.mac_reg;
  }
  if (s.tx.vlan != NULL) {
    delete [] s.tx.vlan;
  }
  if (ethdev != NULL) {
    delete ethdev;
  }
  SIM->get_bochs_root()->remove("e1000");
  BX_DEBUG(("Exit"));
}

void bx_e1000_c::init(void)
{
  Bit8u macaddr[6];
  int i;
  Bit16u checksum = 0;
  bx_param_string_c *bootrom;

  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_E1000);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("E1000 disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("e1000"))->set(0);
    return;
  }
  memcpy(macaddr, SIM->get_param_string("mac", base)->getptr(), 6);

  memcpy(BX_E1000_THIS s.eeprom_data, e1000_eeprom_template,
         sizeof(e1000_eeprom_template));
  for (i = 0; i < 3; i++)
    BX_E1000_THIS s.eeprom_data[i] = (macaddr[2*i+1]<<8) | macaddr[2*i];
  for (i = 0; i < EEPROM_CHECKSUM_REG; i++)
    checksum += BX_E1000_THIS s.eeprom_data[i];
  checksum = (Bit16u) EEPROM_SUM - checksum;
  BX_E1000_THIS s.eeprom_data[EEPROM_CHECKSUM_REG] = checksum;
  BX_E1000_THIS s.mac_reg = new Bit32u[0x8000];
  BX_E1000_THIS s.tx.vlan = new Bit8u[0x10004];
  BX_E1000_THIS s.tx.data = BX_E1000_THIS s.tx.vlan + 4;

  BX_E1000_THIS s.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_E1000_THIS s.devfunc, BX_PLUGIN_E1000,
                            "Experimental Intel(R) Gigabit Ethernet");

  // initialize readonly registers
  init_pci_conf(0x8086, 0x100e, 0x03, 0x020000, 0x00);
  BX_E1000_THIS pci_conf[0x3d] = BX_PCI_INTA;

  BX_E1000_THIS pci_base_address[0] = 0;
  BX_E1000_THIS pci_base_address[1] = 0;
  BX_E1000_THIS pci_rom_address = 0;
  bootrom = SIM->get_param_string("bootrom", base);
  if (!bootrom->isempty()) {
    BX_E1000_THIS load_pci_rom(bootrom->getptr());
  }

  if (BX_E1000_THIS s.tx_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_E1000_THIS s.tx_timer_index =
      bx_pc_system.register_timer(this, tx_timer_handler, 0,
                                  0, 0, "e1000"); // one-shot, inactive
  }
  BX_E1000_THIS s.statusbar_id = bx_gui->register_statusitem("E1000", 1);

  // Attach to the selected ethernet module
  BX_E1000_THIS ethdev = DEV_net_init_module(base, rx_handler, rx_status_handler, this);

  BX_INFO(("E1000 initialized"));
}

void bx_e1000_c::reset(unsigned type)
{
  unsigned i;
  Bit8u *saved_ptr;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x04, 0x03 }, { 0x05, 0x00 }, // command io / memory
    { 0x06, 0x00 }, { 0x07, 0x00 }, // status
    // address space 0x10 - 0x13
    { 0x10, 0x00 }, { 0x11, 0x00 },
    { 0x12, 0x00 }, { 0x13, 0x00 },
    // address space 0x14 - 0x17
    { 0x14, 0x01 }, { 0x15, 0x00 },
    { 0x16, 0x00 }, { 0x17, 0x00 },
    { 0x3c, 0x00 },                 // IRQ
  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_E1000_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  memset(BX_E1000_THIS s.phy_reg, 0, sizeof(BX_E1000_THIS s.phy_reg));
  BX_E1000_THIS s.phy_reg[PHY_CTRL] = 0x1140;
  BX_E1000_THIS s.phy_reg[PHY_STATUS] = 0x796d; // link initially up
  BX_E1000_THIS s.phy_reg[PHY_ID1] = 0x141;
  BX_E1000_THIS s.phy_reg[PHY_ID2] = 0xc20;
  BX_E1000_THIS s.phy_reg[PHY_1000T_CTRL] = 0x0e00;
  BX_E1000_THIS s.phy_reg[M88E1000_PHY_SPEC_CTRL] = 0x360;
  BX_E1000_THIS s.phy_reg[M88E1000_EXT_PHY_SPEC_CTRL] = 0x0d60;
  BX_E1000_THIS s.phy_reg[PHY_AUTONEG_ADV] = 0xde1;
  BX_E1000_THIS s.phy_reg[PHY_LP_ABILITY] = 0x1e0;
  BX_E1000_THIS s.phy_reg[PHY_1000T_STATUS] = 0x3c00;
  BX_E1000_THIS s.phy_reg[M88E1000_PHY_SPEC_STATUS] = 0xac00;
  memset(BX_E1000_THIS s.mac_reg, 0, 0x20000);
  BX_E1000_THIS s.mac_reg[PBA]    =  0x00100030;
  BX_E1000_THIS s.mac_reg[LEDCTL] =  0x602;
  BX_E1000_THIS s.mac_reg[CTRL]   =  E1000_CTRL_SWDPIN2 | E1000_CTRL_SWDPIN0 |
                                     E1000_CTRL_SPD_1000 | E1000_CTRL_SLU;
  BX_E1000_THIS s.mac_reg[STATUS] =  0x80000000 | E1000_STATUS_GIO_MASTER_ENABLE |
                                     E1000_STATUS_ASDV | E1000_STATUS_MTXCKOK |
                                     E1000_STATUS_SPEED_1000 | E1000_STATUS_FD |
                                     E1000_STATUS_LU;
  BX_E1000_THIS s.mac_reg[MANC]   =  E1000_MANC_EN_MNG2HOST | E1000_MANC_RCV_TCO_EN |
                                     E1000_MANC_ARP_EN | E1000_MANC_0298_EN |
                                     E1000_MANC_RMCP_EN;

  BX_E1000_THIS s.rxbuf_min_shift = 1;
  saved_ptr = BX_E1000_THIS s.tx.vlan;
  memset(&BX_E1000_THIS s.tx, 0, sizeof(BX_E1000_THIS s.tx));
  BX_E1000_THIS s.tx.vlan = saved_ptr;
  BX_E1000_THIS s.tx.data = BX_E1000_THIS s.tx.vlan + 4;

  // Deassert IRQ
  set_irq_level(0);
}

void bx_e1000_c::register_state(void)
{
  unsigned i;
  char pname[5];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "e1000", "E1000 State");
  new bx_shadow_data_c(list, "mac_reg", (Bit8u*)BX_E1000_THIS s.mac_reg, 0x20000);
  bx_list_c *phy = new bx_list_c(list, "phy_reg", "");
  for (i = 0; i < 32; i++) {
    sprintf(pname, "0x%02x", i);
    new bx_shadow_num_c(phy, pname, &BX_E1000_THIS s.phy_reg[i], BASE_HEX);
  }
  bx_list_c *eeprom = new bx_list_c(list, "eeprom_data", "");
  for (i = 0; i < 64; i++) {
    sprintf(pname, "0x%02x", i);
    new bx_shadow_num_c(eeprom, pname, &BX_E1000_THIS s.eeprom_data[i], BASE_HEX);
  }
  BXRS_DEC_PARAM_FIELD(list, rxbuf_size, BX_E1000_THIS s.rxbuf_size);
  BXRS_DEC_PARAM_FIELD(list, rxbuf_min_shift, BX_E1000_THIS s.rxbuf_min_shift);
  BXRS_PARAM_BOOL(list, check_rxov, BX_E1000_THIS s.check_rxov);
  bx_list_c *tx = new bx_list_c(list, "tx", "");
  bx_list_c *header = new bx_list_c(tx, "header", "");
  for (i = 0; i < 256; i++) {
    sprintf(pname, "0x%02x", i);
    new bx_shadow_num_c(header, pname, &BX_E1000_THIS s.tx.header[i], BASE_HEX);
  }
  bx_list_c *vlh = new bx_list_c(tx, "vlan_header", "");
  for (i = 0; i < 4; i++) {
    sprintf(pname, "0x%02x", i);
    new bx_shadow_num_c(vlh, pname, &BX_E1000_THIS s.tx.vlan_header[i], BASE_HEX);
  }
  new bx_shadow_data_c(list, "tx_vlan_data", BX_E1000_THIS s.tx.vlan, 0x10004);
  BXRS_DEC_PARAM_FIELD(tx, size, BX_E1000_THIS s.tx.size);
  BXRS_DEC_PARAM_FIELD(tx, sum_needed, BX_E1000_THIS s.tx.sum_needed);
  BXRS_PARAM_BOOL(tx, vlan_needed, BX_E1000_THIS s.tx.vlan_needed);
  BXRS_DEC_PARAM_FIELD(tx, ipcss, BX_E1000_THIS s.tx.ipcss);
  BXRS_DEC_PARAM_FIELD(tx, ipcso, BX_E1000_THIS s.tx.ipcso);
  BXRS_DEC_PARAM_FIELD(tx, ipcse, BX_E1000_THIS s.tx.ipcse);
  BXRS_DEC_PARAM_FIELD(tx, tucss, BX_E1000_THIS s.tx.tucss);
  BXRS_DEC_PARAM_FIELD(tx, tucso, BX_E1000_THIS s.tx.tucso);
  BXRS_DEC_PARAM_FIELD(tx, tucse, BX_E1000_THIS s.tx.tucse);
  BXRS_DEC_PARAM_FIELD(tx, hdr_len, BX_E1000_THIS s.tx.hdr_len);
  BXRS_DEC_PARAM_FIELD(tx, mss, BX_E1000_THIS s.tx.mss);
  BXRS_DEC_PARAM_FIELD(tx, paylen, BX_E1000_THIS s.tx.paylen);
  BXRS_DEC_PARAM_FIELD(tx, tso_frames, BX_E1000_THIS s.tx.tso_frames);
  BXRS_PARAM_BOOL(tx, tse, BX_E1000_THIS s.tx.tse);
  BXRS_PARAM_BOOL(tx, ip, BX_E1000_THIS s.tx.ip);
  BXRS_PARAM_BOOL(tx, tcp, BX_E1000_THIS s.tx.tcp);
  BXRS_PARAM_BOOL(tx, cptse, BX_E1000_THIS s.tx.cptse);
  BXRS_HEX_PARAM_FIELD(tx, int_cause, BX_E1000_THIS s.tx.int_cause);
  bx_list_c *eecds = new bx_list_c(list, "eecd_state", "");
  BXRS_DEC_PARAM_FIELD(eecds, val_in, BX_E1000_THIS s.eecd_state.val_in);
  BXRS_DEC_PARAM_FIELD(eecds, bitnum_in, BX_E1000_THIS s.eecd_state.bitnum_in);
  BXRS_DEC_PARAM_FIELD(eecds, bitnum_out, BX_E1000_THIS s.eecd_state.bitnum_out);
  BXRS_PARAM_BOOL(eecds, reading, BX_E1000_THIS s.eecd_state.reading);
  BXRS_HEX_PARAM_FIELD(eecds, old_eecd, BX_E1000_THIS s.eecd_state.old_eecd);

  register_pci_state(list);
}

void bx_e1000_c::after_restore_state(void)
{
  if (DEV_pci_set_base_mem(BX_E1000_THIS_PTR, mem_read_handler, mem_write_handler,
                           &BX_E1000_THIS pci_base_address[0],
                           &BX_E1000_THIS pci_conf[0x10],
                           0x20000)) {
    BX_INFO(("new mem base address: 0x%08x", BX_E1000_THIS pci_base_address[0]));
  }
  if (DEV_pci_set_base_io(BX_E1000_THIS_PTR, read_handler, write_handler,
                          &BX_E1000_THIS pci_base_address[1],
                          &BX_E1000_THIS pci_conf[0x14],
                          64, &e1000_iomask[0], "e1000")) {
    BX_INFO(("new i/o base address: 0x%04x", BX_E1000_THIS pci_base_address[1]));
  }
  if (BX_E1000_THIS pci_rom_size > 0) {
    if (DEV_pci_set_base_mem(BX_E1000_THIS_PTR, mem_read_handler,
                             mem_write_handler,
                             &BX_E1000_THIS pci_rom_address,
                             &BX_E1000_THIS pci_conf[0x30],
                             BX_E1000_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_E1000_THIS pci_rom_address));
    }
  }
}

bx_bool bx_e1000_c::mem_read_handler(bx_phy_address addr, unsigned len,
                                     void *data, void *param)
{
  Bit32u *data_ptr = (Bit32u*) data;
  Bit8u  *data8_ptr;
  Bit32u offset, value = 0;
  Bit16u index;

  if (BX_E1000_THIS pci_rom_size > 0) {
    Bit32u mask = (BX_E1000_THIS pci_rom_size - 1);
    if ((addr & ~mask) == BX_E1000_THIS pci_rom_address) {
#ifdef BX_LITTLE_ENDIAN
      data8_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
      data8_ptr = (Bit8u *) data + (len - 1);
#endif
      for (unsigned i = 0; i < len; i++) {
        if (BX_E1000_THIS pci_conf[0x30] & 0x01) {
          *data8_ptr = BX_E1000_THIS pci_rom[addr & mask];
        } else {
          *data8_ptr = 0xff;
        }
        addr++;
#ifdef BX_LITTLE_ENDIAN
        data8_ptr++;
#else // BX_BIG_ENDIAN
        data8_ptr--;
#endif
      }
      return 1;
    }
  }

  offset = addr & 0x1ffff;
  index = (offset >> 2);
  if (len == 4) {
    BX_DEBUG(("mem read from offset 0x%08x -", offset));
    switch (offset) {
      case E1000_PBA:
      case E1000_RCTL:
      case E1000_TDH:
      case E1000_TXDCTL:
      case E1000_WUFC:
      case E1000_TDT:
      case E1000_CTRL:
      case E1000_LEDCTL:
      case E1000_MANC:
      case E1000_MDIC:
      case E1000_SWSM:
      case E1000_STATUS:
      case E1000_TORL:
      case E1000_TOTL:
      case E1000_IMS:
      case E1000_TCTL:
      case E1000_RDH:
      case E1000_RDT:
      case E1000_VET:
      case E1000_ICS:
      case E1000_TDBAL:
      case E1000_TDBAH:
      case E1000_RDBAH:
      case E1000_RDBAL:
      case E1000_TDLEN:
      case E1000_RDLEN:
        value = BX_E1000_THIS s.mac_reg[index];
        break;
      case E1000_TOTH:
      case E1000_TORH:
        value = BX_E1000_THIS s.mac_reg[index];
        BX_E1000_THIS s.mac_reg[index] = 0;
        BX_E1000_THIS s.mac_reg[index - 1] = 0;
        break;
      case E1000_GPRC:
      case E1000_GPTC:
      case E1000_TPR:
      case E1000_TPT:
        value = BX_E1000_THIS s.mac_reg[index];
        BX_E1000_THIS s.mac_reg[index] = 0;
        break;
      case E1000_ICR:
        value = BX_E1000_THIS s.mac_reg[index];
        BX_DEBUG(("ICR read: %x", value));
        set_interrupt_cause(0);
        break;
      case E1000_EECD:
        value = get_eecd();
        break;
      case E1000_EERD:
        value = flash_eerd_read();
        break;
      default:
        if (((offset >= E1000_CRCERRS) && (offset <= E1000_MPC)) ||
            ((offset >= E1000_RA) && (offset <= (E1000_RA + 31))) ||
            ((offset >= E1000_MTA) && (offset <= (E1000_MTA + 127))) ||
            ((offset >= E1000_VFTA) && (offset <= (E1000_VFTA + 127)))) {
          value = BX_E1000_THIS s.mac_reg[index];
        } else {
          BX_DEBUG(("mem read from offset 0x%08x returns 0", offset));
        }
    }
  } else if ((len == 1) && (offset == E1000_STATUS)) {
    BX_DEBUG(("mem read from offset 0x%08x with len 1 -", offset));
    value = BX_E1000_THIS s.mac_reg[index] & 0xff;
  } else {
    BX_DEBUG(("mem read from offset 0x%08x with len %d not implemented", offset, len));
  }
  BX_DEBUG(("val =  0x%08x", value));
  *data_ptr = value;
  return 1;
}

bx_bool bx_e1000_c::mem_write_handler(bx_phy_address addr, unsigned len,
                                      void *data, void *param)
{
  Bit32u value = *(Bit32u*) data;
  Bit32u offset;
  Bit16u index;

  if (BX_E1000_THIS pci_rom_size > 0) {
    Bit32u mask = (BX_E1000_THIS pci_rom_size - 1);
    if ((addr & ~mask) == BX_E1000_THIS pci_rom_address) {
      BX_INFO(("write to ROM ignored (addr=0x%08x len=%d)", (Bit32u)addr, len));
      return 1;
    }
  }

  offset = addr & 0x1ffff;
  index = (offset >> 2);
  if (len == 4) {
    BX_DEBUG(("mem write to offset 0x%08x - value = 0x%08x", offset, value));
    switch (offset) {
      case E1000_PBA:
      case E1000_EERD:
      case E1000_SWSM:
      case E1000_WUFC:
      case E1000_TDBAL:
      case E1000_TDBAH:
      case E1000_TXDCTL:
      case E1000_RDBAH:
      case E1000_RDBAL:
      case E1000_LEDCTL:
      case E1000_VET:
        BX_E1000_THIS s.mac_reg[index] = value;
        break;
      case E1000_TDLEN:
      case E1000_RDLEN:
        BX_E1000_THIS s.mac_reg[index] = value & 0xfff80;
        break;
      case E1000_TCTL:
      case E1000_TDT:
        BX_E1000_THIS s.mac_reg[index] = value;
        BX_E1000_THIS s.mac_reg[TDT] &= 0xffff;
        start_xmit();
        break;
      case E1000_MDIC:
        set_mdic(value);
        break;
      case E1000_ICS:
        set_ics(value);
        break;
      case E1000_TDH:
      case E1000_RDH:
        BX_E1000_THIS s.mac_reg[index] = value & 0xffff;
        break;
      case E1000_RDT:
        BX_E1000_THIS s.check_rxov = 0;
        BX_E1000_THIS s.mac_reg[index] = value & 0xffff;
        break;
      case E1000_IMC:
        BX_E1000_THIS s.mac_reg[IMS] &= ~value;
        set_ics(0);
        break;
      case E1000_IMS:
        BX_E1000_THIS s.mac_reg[IMS] |= value;
        set_ics(0);
        break;
      case E1000_ICR:
        BX_DEBUG(("set_icr %x", value));
        set_interrupt_cause(BX_E1000_THIS s.mac_reg[ICR] & ~value);
        break;
      case E1000_EECD:
        set_eecd(value);
        break;
      case E1000_RCTL:
        set_rx_control(value);
        break;
      case E1000_CTRL:
        // RST is self clearing
        BX_E1000_THIS s.mac_reg[CTRL] = value & ~E1000_CTRL_RST;
        break;
      default:
        if (((offset >= E1000_RA) && (offset <= (E1000_RA + 31))) ||
            ((offset >= E1000_MTA) && (offset <= (E1000_MTA + 127))) ||
            ((offset >= E1000_VFTA) && (offset <= (E1000_VFTA + 127)))) {
          BX_E1000_THIS s.mac_reg[index] = value;
        } else {
          BX_DEBUG(("mem write to offset 0x%08x ignored - value = 0x%08x", offset, value));
        }
    }
  } else {
    BX_DEBUG(("mem write to offset 0x%08x with len %d not implemented", offset, len));
  }
  return 1;
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_e1000_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_E1000_SMF
  bx_e1000_c *class_ptr = (bx_e1000_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_e1000_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_E1000_SMF
  Bit8u offset;

  offset = address - BX_E1000_THIS pci_base_address[1];

  BX_ERROR(("register read from offset 0x%02x returns 0", offset));

  return 0;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_e1000_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_E1000_SMF
  bx_e1000_c *class_ptr = (bx_e1000_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_e1000_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_E1000_SMF
  Bit8u  offset;

  offset = address - BX_E1000_THIS pci_base_address[1];

  BX_ERROR(("register write to offset 0x%02x ignored - value = 0x%08x", offset, value));
}

void bx_e1000_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_E1000_THIS s.devfunc, BX_E1000_THIS pci_conf[0x3d], level);
}

void bx_e1000_c::set_interrupt_cause(Bit32u value)
{
  if (value != 0)
    value |= E1000_ICR_INT_ASSERTED;
  BX_E1000_THIS s.mac_reg[ICR] = value;
  BX_E1000_THIS s.mac_reg[ICS] = value;
  set_irq_level((BX_E1000_THIS s.mac_reg[IMS] & BX_E1000_THIS s.mac_reg[ICR]) != 0);
}

void bx_e1000_c::set_ics(Bit32u value)
{
  BX_DEBUG(("set_ics %x, ICR %x, IMR %x", value, BX_E1000_THIS s.mac_reg[ICR],
        BX_E1000_THIS s.mac_reg[IMS]));
  set_interrupt_cause(value | BX_E1000_THIS s.mac_reg[ICR]);
}

int bx_e1000_c::rxbufsize(Bit32u v)
{
  v &= E1000_RCTL_BSEX | E1000_RCTL_SZ_16384 | E1000_RCTL_SZ_8192 |
       E1000_RCTL_SZ_4096 | E1000_RCTL_SZ_2048 | E1000_RCTL_SZ_1024 |
       E1000_RCTL_SZ_512 | E1000_RCTL_SZ_256;
  switch (v) {
    case E1000_RCTL_BSEX | E1000_RCTL_SZ_16384:
      return 16384;
    case E1000_RCTL_BSEX | E1000_RCTL_SZ_8192:
      return 8192;
    case E1000_RCTL_BSEX | E1000_RCTL_SZ_4096:
      return 4096;
    case E1000_RCTL_SZ_1024:
      return 1024;
    case E1000_RCTL_SZ_512:
      return 512;
    case E1000_RCTL_SZ_256:
      return 256;
  }
  return 2048;
}

void bx_e1000_c::set_rx_control(Bit32u value)
{
  BX_E1000_THIS s.mac_reg[RCTL] = value;
  BX_E1000_THIS s.rxbuf_size = rxbufsize(value);
  BX_E1000_THIS s.rxbuf_min_shift = ((value / E1000_RCTL_RDMTS_QUAT) & 3) + 1;
  BX_DEBUG(("RCTL: %d, mac_reg[RCTL] = 0x%x", BX_E1000_THIS s.mac_reg[RDT],
           BX_E1000_THIS s.mac_reg[RCTL]));
}

void bx_e1000_c::set_mdic(Bit32u value)
{
  Bit32u data = value & E1000_MDIC_DATA_MASK;
  Bit32u addr = ((value & E1000_MDIC_REG_MASK) >> E1000_MDIC_REG_SHIFT);

  if ((value & E1000_MDIC_PHY_MASK) >> E1000_MDIC_PHY_SHIFT != 1) { // phy #
    value = BX_E1000_THIS s.mac_reg[MDIC] | E1000_MDIC_ERROR;
  } else if (value & E1000_MDIC_OP_READ) {
    BX_DEBUG(("MDIC read reg 0x%x", addr));
    if (!(phy_regcap[addr] & PHY_R)) {
      BX_DEBUG(("MDIC read reg %x unhandled", addr));
      value |= E1000_MDIC_ERROR;
    } else {
      value = (value ^ data) | BX_E1000_THIS s.phy_reg[addr];
    }
  } else if (value & E1000_MDIC_OP_WRITE) {
    BX_DEBUG(("MDIC write reg 0x%x, value 0x%x", addr, data));
    if (!(phy_regcap[addr] & PHY_W)) {
      BX_DEBUG(("MDIC write reg %x unhandled", addr));
      value |= E1000_MDIC_ERROR;
    } else {
      BX_E1000_THIS s.phy_reg[addr] = data;
    }
  }
  BX_E1000_THIS s.mac_reg[MDIC] = value | E1000_MDIC_READY;
  set_ics(E1000_ICR_MDAC);
}

Bit32u bx_e1000_c::get_eecd()
{
  BX_DEBUG(("reading eeprom bit %d (reading %d)",
            BX_E1000_THIS s.eecd_state.bitnum_out, BX_E1000_THIS s.eecd_state.reading));
  Bit32u ret = E1000_EECD_PRES|E1000_EECD_GNT | BX_E1000_THIS s.eecd_state.old_eecd;
  if (!BX_E1000_THIS s.eecd_state.reading ||
      ((BX_E1000_THIS s.eeprom_data[(BX_E1000_THIS s.eecd_state.bitnum_out >> 4) & 0x3f] >>
       ((BX_E1000_THIS s.eecd_state.bitnum_out & 0xf) ^ 0xf))) & 1) {
    ret |= E1000_EECD_DO;
  }
  return ret;
}

void bx_e1000_c::set_eecd(Bit32u value)
{
  Bit32u oldval = BX_E1000_THIS s.eecd_state.old_eecd;

  BX_E1000_THIS s.eecd_state.old_eecd = value & (E1000_EECD_SK | E1000_EECD_CS |
    E1000_EECD_DI|E1000_EECD_FWE_MASK|E1000_EECD_REQ);
  if (!(E1000_EECD_CS & value)) // CS inactive; nothing to do
    return;
  if (E1000_EECD_CS & (value ^ oldval)) { // CS rise edge; reset state
    BX_E1000_THIS s.eecd_state.val_in = 0;
    BX_E1000_THIS s.eecd_state.bitnum_in = 0;
    BX_E1000_THIS s.eecd_state.bitnum_out = 0;
    BX_E1000_THIS s.eecd_state.reading = 0;
  }
  if (!(E1000_EECD_SK & (value ^ oldval))) // no clock edge
    return;
  if (!(E1000_EECD_SK & value)) { // falling edge
    BX_E1000_THIS s.eecd_state.bitnum_out++;
    return;
  }
  BX_E1000_THIS s.eecd_state.val_in <<= 1;
  if (value & E1000_EECD_DI)
    BX_E1000_THIS s.eecd_state.val_in |= 1;
  if (++BX_E1000_THIS s.eecd_state.bitnum_in == 9 && !BX_E1000_THIS s.eecd_state.reading) {
    BX_E1000_THIS s.eecd_state.bitnum_out = ((BX_E1000_THIS s.eecd_state.val_in & 0x3f)<<4)-1;
    BX_E1000_THIS s.eecd_state.reading = (((BX_E1000_THIS s.eecd_state.val_in >> 6) & 7) ==
      EEPROM_READ_OPCODE_MICROWIRE);
  }
  BX_DEBUG(("eeprom bitnum in %d out %d, reading %d",
            BX_E1000_THIS s.eecd_state.bitnum_in, BX_E1000_THIS s.eecd_state.bitnum_out,
            BX_E1000_THIS s.eecd_state.reading));
}

Bit32u bx_e1000_c::flash_eerd_read()
{
  unsigned int index, r = BX_E1000_THIS s.mac_reg[EERD] & ~E1000_EEPROM_RW_REG_START;

  if ((BX_E1000_THIS s.mac_reg[EERD] & E1000_EEPROM_RW_REG_START) == 0)
    return (BX_E1000_THIS s.mac_reg[EERD]);

  if ((index = r >> E1000_EEPROM_RW_ADDR_SHIFT) > EEPROM_CHECKSUM_REG)
    return (E1000_EEPROM_RW_REG_DONE | r);

  return ((BX_E1000_THIS s.eeprom_data[index] << E1000_EEPROM_RW_REG_DATA) |
           E1000_EEPROM_RW_REG_DONE | r);
}

void bx_e1000_c::putsum(Bit8u *data, Bit32u n, Bit32u sloc, Bit32u css, Bit32u cse)
{
  Bit32u sum;

  if (cse && cse < n)
    n = cse + 1;
  if (sloc < n-1) {
    sum = net_checksum_add(data+css, n-css);
    put_net2(data + sloc, net_checksum_finish(sum));
  }
}

bx_bool bx_e1000_c::vlan_enabled()
{
  return ((BX_E1000_THIS s.mac_reg[CTRL] & E1000_CTRL_VME) != 0);
}

bx_bool bx_e1000_c::vlan_rx_filter_enabled()
{
  return ((BX_E1000_THIS s.mac_reg[RCTL] & E1000_RCTL_VFE) != 0);
}

bx_bool bx_e1000_c::is_vlan_packet(const Bit8u *buf)
{
  return (get_net2(buf + 12) == (Bit16u)BX_E1000_THIS s.mac_reg[VET]);
}

bx_bool bx_e1000_c::is_vlan_txd(Bit32u txd_lower)
{
    return ((txd_lower & E1000_TXD_CMD_VLE) != 0);
}

int bx_e1000_c::fcs_len()
{
  return (BX_E1000_THIS s.mac_reg[RCTL] & E1000_RCTL_SECRC) ? 0 : 4;
}

void bx_e1000_c::xmit_seg()
{
  Bit16u len;
  Bit8u *sp;
  unsigned int frames = BX_E1000_THIS s.tx.tso_frames, css, sofar, n;
  e1000_tx *tp = &BX_E1000_THIS s.tx;

  if (tp->tse && tp->cptse) {
    css = tp->ipcss;
    BX_DEBUG(("frames %d size %d ipcss %d", frames, tp->size, css));
    if (tp->ip) { // IPv4
      put_net2(tp->data+css+2, tp->size - css);
      put_net2(tp->data+css+4, get_net2(tp->data+css+4)+frames);
    } else // IPv6
      put_net2(tp->data+css+4, tp->size - css);
    css = tp->tucss;
    len = tp->size - css;
    BX_DEBUG(("tcp %d tucss %d len %d", tp->tcp, css, len));
    if (tp->tcp) {
      sofar = frames * tp->mss;
      put_net4(tp->data+css+4, // seq
               get_net4(tp->data+css+4)+sofar);
      if (tp->paylen - sofar > tp->mss)
        tp->data[css + 13] &= ~9; // PSH, FIN
    } else // UDP
      put_net2(tp->data+css+4, len);
    if (tp->sum_needed & E1000_TXD_POPTS_TXSM) {
      unsigned int phsum;
      // add pseudo-header length before checksum calculation
      sp = tp->data + tp->tucso;
      phsum = get_net2(sp) + len;
      phsum = (phsum >> 16) + (phsum & 0xffff);
      put_net2(sp, phsum);
    }
    tp->tso_frames++;
  }

  if (tp->sum_needed & E1000_TXD_POPTS_TXSM)
    putsum(tp->data, tp->size, tp->tucso, tp->tucss, tp->tucse);
  if (tp->sum_needed & E1000_TXD_POPTS_IXSM)
    putsum(tp->data, tp->size, tp->ipcso, tp->ipcss, tp->ipcse);
  if (tp->vlan_needed) {
    memmove(tp->vlan, tp->data, 4);
    memmove(tp->data, tp->data + 4, 8);
    memcpy(tp->data + 8, tp->vlan_header, 4);
    BX_E1000_THIS ethdev->sendpkt(tp->vlan, tp->size + 4);
  } else
    BX_E1000_THIS ethdev->sendpkt(tp->data, tp->size);
  BX_E1000_THIS s.mac_reg[TPT]++;
  BX_E1000_THIS s.mac_reg[GPTC]++;
  n = BX_E1000_THIS s.mac_reg[TOTL];
  if ((BX_E1000_THIS s.mac_reg[TOTL] += BX_E1000_THIS s.tx.size) < n)
    BX_E1000_THIS s.mac_reg[TOTH]++;
}

void bx_e1000_c::process_tx_desc(struct e1000_tx_desc *dp)
{
  Bit32u txd_lower = le32_to_cpu(dp->lower.data);
  Bit32u dtype = txd_lower & (E1000_TXD_CMD_DEXT | E1000_TXD_DTYP_D);
  unsigned int split_size = txd_lower & 0xffff, bytes, sz, op;
  unsigned int msh = 0xfffff, hdr = 0;
  Bit64u addr;
  struct e1000_context_desc *xp = (struct e1000_context_desc *)dp;
  e1000_tx *tp = &BX_E1000_THIS s.tx;

  if (dtype == E1000_TXD_CMD_DEXT) { // context descriptor
    op = le32_to_cpu(xp->cmd_and_length);
    tp->ipcss = xp->lower_setup.ip_fields.ipcss;
    tp->ipcso = xp->lower_setup.ip_fields.ipcso;
    tp->ipcse = le16_to_cpu(xp->lower_setup.ip_fields.ipcse);
    tp->tucss = xp->upper_setup.tcp_fields.tucss;
    tp->tucso = xp->upper_setup.tcp_fields.tucso;
    tp->tucse = le16_to_cpu(xp->upper_setup.tcp_fields.tucse);
    tp->paylen = op & 0xfffff;
    tp->hdr_len = xp->tcp_seg_setup.fields.hdr_len;
    tp->mss = le16_to_cpu(xp->tcp_seg_setup.fields.mss);
    tp->ip = (op & E1000_TXD_CMD_IP) ? 1 : 0;
    tp->tcp = (op & E1000_TXD_CMD_TCP) ? 1 : 0;
    tp->tse = (op & E1000_TXD_CMD_TSE) ? 1 : 0;
    tp->tso_frames = 0;
    if (tp->tucso == 0) { // this is probably wrong
      BX_DEBUG(("TCP/UDP: cso 0!"));
      tp->tucso = tp->tucss + (tp->tcp ? 16 : 6);
    }
    return;
  } else if (dtype == (E1000_TXD_CMD_DEXT | E1000_TXD_DTYP_D)) {
    // data descriptor
    if (tp->size == 0) {
      tp->sum_needed = le32_to_cpu(dp->upper.data) >> 8;
    }
    tp->cptse = ( txd_lower & E1000_TXD_CMD_TSE ) ? 1 : 0;
  } else {
    // legacy descriptor
    tp->cptse = 0;
  }

  if (vlan_enabled() && is_vlan_txd(txd_lower) &&
     (tp->cptse || txd_lower & E1000_TXD_CMD_EOP)) {
    tp->vlan_needed = 1;
    put_net2(tp->vlan_header, (Bit16u)BX_E1000_THIS s.mac_reg[VET]);
    put_net2(tp->vlan_header + 2, le16_to_cpu(dp->upper.fields.special));
  }

  addr = le64_to_cpu(dp->buffer_addr);
  if (tp->tse && tp->cptse) {
    hdr = tp->hdr_len;
    msh = hdr + tp->mss;
    do {
      bytes = split_size;
      if (tp->size + bytes > msh)
        bytes = msh - tp->size;
      DEV_MEM_READ_PHYSICAL_DMA(addr, bytes, tp->data + tp->size);
      if ((sz = tp->size + bytes) >= hdr && tp->size < hdr)
        memmove(tp->header, tp->data, hdr);
      tp->size = sz;
      addr += bytes;
      if (sz == msh) {
        xmit_seg();
        memmove(tp->data, tp->header, hdr);
        tp->size = hdr;
      }
    } while (split_size -= bytes);
  } else if (!tp->tse && tp->cptse) {
    // context descriptor TSE is not set, while data descriptor TSE is set
    BX_DEBUG(("TCP segmentaion Error"));
  } else {
    DEV_MEM_READ_PHYSICAL_DMA(addr, split_size, tp->data + tp->size);
    tp->size += split_size;
  }

  if (!(txd_lower & E1000_TXD_CMD_EOP))
    return;
  if (!(tp->tse && tp->cptse && tp->size < hdr))
    xmit_seg();
  tp->tso_frames = 0;
  tp->sum_needed = 0;
  tp->vlan_needed = 0;
  tp->size = 0;
  tp->cptse = 0;
}

Bit32u bx_e1000_c::txdesc_writeback(bx_phy_address base, struct e1000_tx_desc *dp)
{
  Bit32u txd_upper, txd_lower = le32_to_cpu(dp->lower.data);

  if (!(txd_lower & (E1000_TXD_CMD_RS|E1000_TXD_CMD_RPS)))
    return 0;
  txd_upper = (le32_to_cpu(dp->upper.data) | E1000_TXD_STAT_DD) &
              ~(E1000_TXD_STAT_EC | E1000_TXD_STAT_LC | E1000_TXD_STAT_TU);
  dp->upper.data = cpu_to_le32(txd_upper);
  DEV_MEM_WRITE_PHYSICAL_DMA(base + ((char *)&dp->upper - (char *)dp),
                             sizeof(dp->upper), (Bit8u *)&dp->upper);
  return E1000_ICR_TXDW;
}

Bit64u bx_e1000_c::tx_desc_base()
{
  Bit64u bah = BX_E1000_THIS s.mac_reg[TDBAH];
  Bit64u bal = BX_E1000_THIS s.mac_reg[TDBAL] & ~0xf;

  return (bah << 32) + bal;
}

void bx_e1000_c::start_xmit()
{
  bx_phy_address base;
  struct e1000_tx_desc desc;
  Bit32u tdh_start = BX_E1000_THIS s.mac_reg[TDH], cause = E1000_ICS_TXQE;

  if (!(BX_E1000_THIS s.mac_reg[TCTL] & E1000_TCTL_EN)) {
    BX_DEBUG(("tx disabled"));
    return;
  }

  while (BX_E1000_THIS s.mac_reg[TDH] != BX_E1000_THIS s.mac_reg[TDT]) {
    base = tx_desc_base() +
           sizeof(struct e1000_tx_desc) * BX_E1000_THIS s.mac_reg[TDH];
    DEV_MEM_READ_PHYSICAL_DMA(base, sizeof(struct e1000_tx_desc), (Bit8u *)&desc);
    BX_DEBUG(("index %d: %p : %x %x", BX_E1000_THIS s.mac_reg[TDH],
              (void *)desc.buffer_addr, desc.lower.data,
               desc.upper.data));

    process_tx_desc(&desc);
    cause |= txdesc_writeback(base, &desc);

    if (++BX_E1000_THIS s.mac_reg[TDH] * sizeof(desc) >= BX_E1000_THIS s.mac_reg[TDLEN])
        BX_E1000_THIS s.mac_reg[TDH] = 0;
    /*
     * the following could happen only if guest sw assigns
     * bogus values to TDT/TDLEN.
     * there's nothing too intelligent we could do about this.
     */
    if (BX_E1000_THIS s.mac_reg[TDH] == tdh_start) {
      BX_ERROR(("TDH wraparound @%x, TDT %x, TDLEN %x", tdh_start,
                BX_E1000_THIS s.mac_reg[TDT], BX_E1000_THIS s.mac_reg[TDLEN]));
      break;
    }
  }
  BX_E1000_THIS s.tx.int_cause = cause;
  bx_pc_system.activate_timer(BX_E1000_THIS s.tx_timer_index, 10, 0); // not continuous
  bx_gui->statusbar_setitem(BX_E1000_THIS s.statusbar_id, 1, 1);
}

void bx_e1000_c::tx_timer_handler(void *this_ptr)
{
  bx_e1000_c *class_ptr = (bx_e1000_c *) this_ptr;
  class_ptr->tx_timer();
}

void bx_e1000_c::tx_timer(void)
{
  set_ics(BX_E1000_THIS s.tx.int_cause);
}

int bx_e1000_c::receive_filter(const Bit8u *buf, int size)
{
  static const Bit8u bcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  static const int mta_shift[] = {4, 3, 2, 0};
  Bit32u f, rctl = BX_E1000_THIS s.mac_reg[RCTL], ra[2], *rp;

  if (is_vlan_packet(buf) && vlan_rx_filter_enabled()) {
    Bit16u vid = get_net2(buf + 14);
    Bit32u vfta = BX_E1000_THIS s.mac_reg[VFTA + ((vid >> 5) & 0x7f)];
    if ((vfta & (1 << (vid & 0x1f))) == 0)
      return 0;
  }

  if (rctl & E1000_RCTL_UPE) // promiscuous
    return 1;

  if ((buf[0] & 1) && (rctl & E1000_RCTL_MPE)) // promiscuous mcast
    return 1;

  if ((rctl & E1000_RCTL_BAM) && !memcmp(buf, bcast, sizeof bcast))
    return 1;

  for (rp = BX_E1000_THIS s.mac_reg + RA; rp < BX_E1000_THIS s.mac_reg + RA + 32; rp += 2) {
    if (!(rp[1] & E1000_RAH_AV))
      continue;
    ra[0] = cpu_to_le32(rp[0]);
    ra[1] = cpu_to_le32(rp[1]);
    if (!memcmp(buf, (Bit8u *)ra, 6)) {
      BX_DEBUG(("unicast match[%d]: %02x:%02x:%02x:%02x:%02x:%02x",
                (int)(rp - BX_E1000_THIS s.mac_reg - RA) / 2,
                buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]));
      return 1;
    }
  }
  BX_DEBUG(("unicast mismatch: %02x:%02x:%02x:%02x:%02x:%02x",
            buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]));

  f = mta_shift[(rctl >> E1000_RCTL_MO_SHIFT) & 3];
  f = (((buf[5] << 8) | buf[4]) >> f) & 0xfff;
  if (BX_E1000_THIS s.mac_reg[MTA + (f >> 5)] & (1 << (f & 0x1f)))
    return 1;
  BX_DEBUG(("dropping, inexact filter mismatch: %02x:%02x:%02x:%02x:%02x:%02x MO %d MTA[%d] %x",
            buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
            (rctl >> E1000_RCTL_MO_SHIFT) & 3, f >> 5,
            BX_E1000_THIS s.mac_reg[MTA + (f >> 5)]));

  return 0;
}

bx_bool bx_e1000_c::e1000_has_rxbufs(size_t total_size)
{
  int bufs;
  // Fast-path short packets
  if (total_size <= BX_E1000_THIS s.rxbuf_size) {
    return (BX_E1000_THIS s.mac_reg[RDH] != BX_E1000_THIS s.mac_reg[RDT]) ||
           !BX_E1000_THIS s.check_rxov;
  }
  if (BX_E1000_THIS s.mac_reg[RDH] < BX_E1000_THIS s.mac_reg[RDT]) {
    bufs = BX_E1000_THIS s.mac_reg[RDT] - BX_E1000_THIS s.mac_reg[RDH];
  } else if (BX_E1000_THIS s.mac_reg[RDH] > BX_E1000_THIS s.mac_reg[RDT] ||
             !BX_E1000_THIS s.check_rxov) {
    bufs = BX_E1000_THIS s.mac_reg[RDLEN] /  sizeof(struct e1000_rx_desc) +
           BX_E1000_THIS s.mac_reg[RDT] - BX_E1000_THIS s.mac_reg[RDH];
  } else {
    return 0;
  }
  return (total_size <= (bufs * BX_E1000_THIS s.rxbuf_size));
}

Bit64u bx_e1000_c::rx_desc_base()
{
  Bit64u bah = BX_E1000_THIS s.mac_reg[RDBAH];
  Bit64u bal = BX_E1000_THIS s.mac_reg[RDBAL] & ~0xf;

  return (bah << 32) + bal;
}

/*
 * Callback from the eth system driver to check if the device can receive
 */
Bit32u bx_e1000_c::rx_status_handler(void *arg)
{
  bx_e1000_c *class_ptr = (bx_e1000_c *) arg;
  return class_ptr->rx_status();
}

Bit32u bx_e1000_c::rx_status()
{
  Bit32u status = BX_NETDEV_1GBIT;
  if ((BX_E1000_THIS s.mac_reg[RCTL] & E1000_RCTL_EN) && e1000_has_rxbufs(1)) {
    status |= BX_NETDEV_RXREADY;
  }
  return status;
}

/*
 * Callback from the eth system driver when a frame has arrived
 */
void bx_e1000_c::rx_handler(void *arg, const void *buf, unsigned len)
{
  bx_e1000_c *class_ptr = (bx_e1000_c *) arg;
  class_ptr->rx_frame(buf, len);
}

void bx_e1000_c::rx_frame(const void *buf, unsigned buf_size)
{
  struct e1000_rx_desc desc;
  bx_phy_address base;
  unsigned int n, rdt;
  Bit32u rdh_start;
  Bit16u vlan_special = 0;
  Bit8u vlan_status = 0, vlan_offset = 0;
  Bit8u min_buf[MIN_BUF_SIZE];
  size_t desc_offset;
  size_t desc_size;
  size_t total_size;

  if (!(BX_E1000_THIS s.mac_reg[RCTL] & E1000_RCTL_EN))
    return;

  // Pad to minimum Ethernet frame length
  if (buf_size < sizeof(min_buf)) {
    memcpy(min_buf, buf, buf_size);
    memset(&min_buf[buf_size], 0, sizeof(min_buf) - buf_size);
    buf = min_buf;
    buf_size = sizeof(min_buf);
  }

  if (!receive_filter((Bit8u *)buf, buf_size))
    return;

  if (vlan_enabled() && is_vlan_packet((Bit8u *)buf)) {
    vlan_special = cpu_to_le16(get_net2(((Bit8u *)(buf) + 14)));
    memmove((Bit8u *)buf + 4, buf, 12);
    vlan_status = E1000_RXD_STAT_VP;
    vlan_offset = 4;
    buf_size -= 4;
  }

  rdh_start = BX_E1000_THIS s.mac_reg[RDH];
  desc_offset = 0;
  total_size = buf_size + fcs_len();
  if (!e1000_has_rxbufs(total_size)) {
    set_ics(E1000_ICS_RXO);
    return;
  }
  do {
    desc_size = total_size - desc_offset;
    if (desc_size > BX_E1000_THIS s.rxbuf_size) {
        desc_size = BX_E1000_THIS s.rxbuf_size;
    }
    base = rx_desc_base() + sizeof(desc) * BX_E1000_THIS s.mac_reg[RDH];
    DEV_MEM_READ_PHYSICAL_DMA(base, sizeof(desc), (Bit8u *)&desc);
    desc.special = vlan_special;
    desc.status |= (vlan_status | E1000_RXD_STAT_DD);
    if (desc.buffer_addr) {
      if (desc_offset < buf_size) {
        size_t copy_size = buf_size - desc_offset;
        if (copy_size > BX_E1000_THIS s.rxbuf_size) {
          copy_size = BX_E1000_THIS s.rxbuf_size;
        }
        DEV_MEM_WRITE_PHYSICAL_DMA(le64_to_cpu(desc.buffer_addr), copy_size,
                                   (Bit8u *)buf + desc_offset + vlan_offset);
      }
      desc_offset += desc_size;
      desc.length = cpu_to_le16(desc_size);
      if (desc_offset >= total_size) {
          desc.status |= E1000_RXD_STAT_EOP | E1000_RXD_STAT_IXSM;
      } else {
        /* Guest zeroing out status is not a hardware requirement.
           Clear EOP in case guest didn't do it. */
        desc.status &= ~E1000_RXD_STAT_EOP;
      }
    } else { // as per intel docs; skip descriptors with null buf addr
      BX_ERROR(("Null RX descriptor!!"));
    }
    DEV_MEM_WRITE_PHYSICAL_DMA(base, sizeof(desc), (Bit8u *)&desc);
    if (++BX_E1000_THIS s.mac_reg[RDH] * sizeof(desc) >= BX_E1000_THIS s.mac_reg[RDLEN])
        BX_E1000_THIS s.mac_reg[RDH] = 0;
    BX_E1000_THIS s.check_rxov = 1;
    /* see comment in start_xmit; same here */
    if (BX_E1000_THIS s.mac_reg[RDH] == rdh_start) {
        BX_DEBUG(("RDH wraparound @%x, RDT %x, RDLEN %x",
                  rdh_start, BX_E1000_THIS s.mac_reg[RDT], BX_E1000_THIS s.mac_reg[RDLEN]));
        set_ics(E1000_ICS_RXO);
        return;
    }
  } while (desc_offset < total_size);

  BX_E1000_THIS s.mac_reg[GPRC]++;
  BX_E1000_THIS s.mac_reg[TPR]++;
  /* TOR - Total Octets Received:
   * This register includes bytes received in a packet from the <Destination
   * Address> field through the <CRC> field, inclusively.
   */
  n = BX_E1000_THIS s.mac_reg[TORL] + buf_size + /* Always include FCS length. */ 4;
  if (n < BX_E1000_THIS s.mac_reg[TORL])
      BX_E1000_THIS s.mac_reg[TORH]++;
  BX_E1000_THIS s.mac_reg[TORL] = n;

  n = E1000_ICS_RXT0;
  if ((rdt = BX_E1000_THIS s.mac_reg[RDT]) < BX_E1000_THIS s.mac_reg[RDH])
    rdt += BX_E1000_THIS s.mac_reg[RDLEN] / sizeof(desc);
  if (((rdt - BX_E1000_THIS s.mac_reg[RDH]) * sizeof(desc)) <= BX_E1000_THIS s.mac_reg[RDLEN] >>
      BX_E1000_THIS s.rxbuf_min_shift)
    n |= E1000_ICS_RXDMT0;

  set_ics(n);

  bx_gui->statusbar_setitem(BX_E1000_THIS s.statusbar_id, 1);
}


// pci configuration space read callback handler
Bit32u bx_e1000_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_E1000_THIS pci_conf[address+i] << (i*8));
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
void bx_e1000_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr0_change = 0;
  bx_bool baseaddr1_change = 0;
  bx_bool romaddr_change = 0;

  if ((address >= 0x18) && (address < 0x30))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_E1000_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x07;
        break;
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
        }
        break;
      case 0x10:
        value8 = (value8 & 0xf0) | (oldval & 0x0f);
      case 0x11:
      case 0x12:
      case 0x13:
        baseaddr0_change |= (value8 != oldval);
        break;
      case 0x14:
        value8 = (value8 & 0xf0) | (oldval & 0x0f);
      case 0x15:
      case 0x16:
      case 0x17:
        baseaddr1_change |= (value8 != oldval);
        break;
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
        if (BX_E1000_THIS pci_rom_size > 0) {
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
    BX_E1000_THIS pci_conf[address+i] = value8;
  }
  if (baseaddr0_change) {
    if (DEV_pci_set_base_mem(BX_E1000_THIS_PTR, mem_read_handler, mem_write_handler,
                             &BX_E1000_THIS pci_base_address[0],
                             &BX_E1000_THIS pci_conf[0x10],
                             0x20000)) {
      BX_INFO(("new mem base address: 0x%08x", BX_E1000_THIS pci_base_address[0]));
    }
  }
  if (baseaddr1_change) {
    if (DEV_pci_set_base_io(BX_E1000_THIS_PTR, read_handler, write_handler,
                            &BX_E1000_THIS pci_base_address[1],
                            &BX_E1000_THIS pci_conf[0x14],
                            64, &e1000_iomask[0], "e1000")) {
      BX_INFO(("new i/o base address: 0x%04x", BX_E1000_THIS pci_base_address[1]));
    }
  }
  if (romaddr_change) {
    if (DEV_pci_set_base_mem(BX_E1000_THIS_PTR, mem_read_handler,
                             mem_write_handler,
                             &BX_E1000_THIS pci_rom_address,
                             &BX_E1000_THIS pci_conf[0x30],
                             BX_E1000_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_E1000_THIS pci_rom_address));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_E1000
