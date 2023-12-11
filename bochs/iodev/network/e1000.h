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
//  Copyright (C) 2011-2023  The Bochs Project
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

#define BX_E1000_MAX_DEVS 4

#define BX_E1000_THIS this->
#define BX_E1000_THIS_PTR this

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
  bool    vlan_needed;
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
  bool    tse;
  bool    ip;
  bool    tcp;
  bool    cptse; // current packet tse bit
  Bit32u  int_cause;
} e1000_tx;

typedef struct {
  Bit32u *mac_reg;
  Bit16u phy_reg[0x20];
  Bit16u eeprom_data[64];

  Bit32u  rxbuf_size;
  Bit32u  rxbuf_min_shift;
  bool check_rxov;

  e1000_tx tx;

  struct {
    Bit32u  val_in; // shifted in from guest driver
    Bit16u  bitnum_in;
    Bit16u  bitnum_out;
    bool    reading;
    Bit32u  old_eecd;
  } eecd_state;

  int tx_timer_index;
  int statusbar_id;

  Bit8u devfunc;
  char devname[16];
  char ldevname[32];

  Bit32u io_memaddr;
} bx_e1000_t;


class bx_e1000_c : public bx_pci_device_c {
public:
  bx_e1000_c();
  virtual ~bx_e1000_c();
  virtual void init_card(Bit8u card);
  virtual void reset(unsigned type);
  void         e1000_register_state(bx_list_c *parent, Bit8u card);
  virtual void after_restore_state(void);

  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

private:
  bx_e1000_t s;

  eth_pktmover_c *ethdev;

  void    set_irq_level(bool level);
  void    set_interrupt_cause(Bit32u val);
  void    set_ics(Bit32u value);
  int     rxbufsize(Bit32u v);
  void    set_rx_control(Bit32u value);
  void    set_mdic(Bit32u value);
  Bit32u  get_eecd(void);
  void    set_eecd(Bit32u value);
  Bit32u  flash_eerd_read(void);
  void    putsum(Bit8u *data, Bit32u n, Bit32u sloc, Bit32u css, Bit32u cse);
  bool    vlan_enabled(void);
  bool    vlan_rx_filter_enabled(void);
  bool    is_vlan_packet(const Bit8u *buf);
  bool    is_vlan_txd(Bit32u txd_lower);
  int     fcs_len(void);
  void    xmit_seg(void);
  void    process_tx_desc(struct e1000_tx_desc *dp);
  Bit32u  txdesc_writeback(bx_phy_address base, struct e1000_tx_desc *dp);
  Bit64u  tx_desc_base(void);
  void    start_xmit(void);

  static void tx_timer_handler(void *);
  void tx_timer(void);

  int     receive_filter(const Bit8u *buf, int size);
  bool    e1000_has_rxbufs(size_t total_size);
  Bit64u  rx_desc_base(void);

  static Bit32u rx_status_handler(void *arg);
  Bit32u rx_status(void);
  static void rx_handler(void *arg, const void *buf, unsigned len);
  void rx_frame(const void *buf, unsigned io_len);

  static bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool mem_read(bx_phy_address addr, unsigned len, void *data);
  bool mem_write(bx_phy_address addr, unsigned len, void *data);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
};

class bx_e1000_main_c : public bx_devmodel_c
{
public:
  bx_e1000_main_c();
  virtual ~bx_e1000_main_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);
private:
  bx_e1000_c *theE1000Dev[BX_E1000_MAX_DEVS];
};

#endif
