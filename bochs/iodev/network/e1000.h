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
//  Copyright (C) 2011  The Bochs Project
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

#ifndef BX_IODEV_E1000_H
#define BX_IODEV_E1000_H

#if BX_USE_E1000_SMF
#  define BX_E1000_SMF  static
#  define BX_E1000_THIS theE1000Device->
#  define BX_E1000_THIS_PTR theE1000Device
#else
#  define BX_E1000_SMF
#  define BX_E1000_THIS this->
#  define BX_E1000_THIS_PTR this
#endif

struct e1000_tx_desc {
  Bit64u buffer_addr;   // Address of the descriptor's data buffer
  union {
    Bit32u data;
    struct {
      Bit16u length;    // Data buffer length
      Bit8u cso;        // Checksum offset
      Bit8u cmd;        // Descriptor control
    } flags;
  } lower;
  union {
    Bit32u data;
    struct {
      Bit8u status;     // Descriptor status
      Bit8u css;        // Checksum start
      Bit16u special;
    } fields;
  } upper;
};

typedef struct {
  Bit8u   header[256];
  Bit8u   vlan_header[4];
  Bit8u   *vlan;
  Bit8u   *data;
  Bit16u  size;
  Bit8u   sum_needed;
  bx_bool vlan_needed;
  Bit8u   ipcss;
  Bit8u   ipcso;
  Bit16u  ipcse;
  Bit8u   tucss;
  Bit8u   tucso;
  Bit16u  tucse;
  Bit8u   hdr_len;
  Bit16u  mss;
  Bit32u  paylen;
  Bit16u  tso_frames;
  bx_bool tse;
  bx_bool ip;
  bx_bool tcp;
  bx_bool cptse; // current packet tse bit
  Bit32u  int_cause;
} e1000_tx;

typedef struct {
  Bit32u *mac_reg;
  Bit16u phy_reg[0x20];
  Bit16u eeprom_data[64];

  Bit32u  rxbuf_size;
  Bit32u  rxbuf_min_shift;
  bx_bool check_rxov;

  e1000_tx tx;

  struct {
    Bit32u  val_in; // shifted in from guest driver
    Bit16u  bitnum_in;
    Bit16u  bitnum_out;
    bx_bool reading;
    Bit32u  old_eecd;
  } eecd_state;

  int tx_timer_index;
  int statusbar_id;

  Bit8u devfunc;
} bx_e1000_t;


class bx_e1000_c : public bx_devmodel_c, bx_pci_device_stub_c {
public:
  bx_e1000_c();
  virtual ~bx_e1000_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

private:
  bx_e1000_t s;

  eth_pktmover_c *ethdev;

  BX_E1000_SMF void    set_irq_level(bx_bool level);
  BX_E1000_SMF void    set_interrupt_cause(Bit32u val);
  BX_E1000_SMF void    set_ics(Bit32u value);
  BX_E1000_SMF int     rxbufsize(Bit32u v);
  BX_E1000_SMF void    set_rx_control(Bit32u value);
  BX_E1000_SMF void    set_mdic(Bit32u value);
  BX_E1000_SMF Bit32u  get_eecd(void);
  BX_E1000_SMF void    set_eecd(Bit32u value);
  BX_E1000_SMF Bit32u  flash_eerd_read(void);
  BX_E1000_SMF void    putsum(Bit8u *data, Bit32u n, Bit32u sloc, Bit32u css, Bit32u cse);
  BX_E1000_SMF bx_bool vlan_enabled(void);
  BX_E1000_SMF bx_bool vlan_rx_filter_enabled(void);
  BX_E1000_SMF bx_bool is_vlan_packet(const Bit8u *buf);
  BX_E1000_SMF bx_bool is_vlan_txd(Bit32u txd_lower);
  BX_E1000_SMF int     fcs_len(void);
  BX_E1000_SMF void    xmit_seg(void);
  BX_E1000_SMF void    process_tx_desc(struct e1000_tx_desc *dp);
  BX_E1000_SMF Bit32u  txdesc_writeback(bx_phy_address base, struct e1000_tx_desc *dp);
  BX_E1000_SMF Bit64u  tx_desc_base(void);
  BX_E1000_SMF void    start_xmit(void);

  static void tx_timer_handler(void *);
  void tx_timer(void);

  BX_E1000_SMF int     receive_filter(const Bit8u *buf, int size);
  BX_E1000_SMF bx_bool e1000_has_rxbufs(size_t total_size);
  BX_E1000_SMF Bit64u  rx_desc_base(void);

  static Bit32u rx_status_handler(void *arg);
  BX_E1000_SMF Bit32u rx_status(void);
  static void rx_handler(void *arg, const void *buf, unsigned len);
  BX_E1000_SMF void rx_frame(const void *buf, unsigned io_len);

  BX_E1000_SMF bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_E1000_SMF bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_E1000_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
