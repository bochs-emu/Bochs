/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2023  The Bochs Project
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

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

//
// An implementation of an ne2000 ISA ethernet adapter. This part uses
// a National Semiconductor DS-8390 ethernet MAC chip, with some h/w
// to provide a windowed memory region for the chip and a MAC address.
//

#ifndef BX_IODEV_NE2K
#define BX_IODEV_NE2K

#define BX_NE2K_MAX_DEVS 4

#define BX_NE2K_THIS this->
#define BX_NE2K_THIS_PTR this

#define  BX_NE2K_MEMSIZ    (32*1024)
#define  BX_NE2K_MEMSTART  (16*1024)
#define  BX_NE2K_MEMEND    (BX_NE2K_MEMSTART + BX_NE2K_MEMSIZ)

class eth_pktmover_c;

typedef struct {
    //
    // ne2k register state

    //
    // Page 0
    //
    //  Command Register - 00h read/write
    struct {
        bool  stop;       // STP - Software Reset command
        bool  start;      // START - start the NIC
        bool  tx_packet;  // TXP - initiate packet transmission
        Bit8u rdma_cmd;   // RD0,RD1,RD2 - Remote DMA command
        Bit8u pgsel;      // PS0,PS1 - Page select
    } CR;
    // Interrupt Status Register - 07h read/write
    struct {
        bool  pkt_rx;     // PRX - packet received with no errors
        bool  pkt_tx;     // PTX - packet transmitted with no errors
        bool  rx_err;     // RXE - packet received with 1 or more errors
        bool  tx_err;     // TXE - packet tx'd       "  " "    "    "
        bool  overwrite;  // OVW - rx buffer resources exhausted
        bool  cnt_oflow;  // CNT - network tally counter MSB's set
        bool  rdma_done;  // RDC - remote DMA complete
        bool  reset;      // RST - reset status
    } ISR;
    // Interrupt Mask Register - 0fh write
    struct {
        bool  rx_inte;    // PRXE - packet rx interrupt enable
        bool  tx_inte;    // PTXE - packet tx interrput enable
        bool  rxerr_inte; // RXEE - rx error interrupt enable
        bool  txerr_inte; // TXEE - tx error interrupt enable
        bool  overw_inte; // OVWE - overwrite warn int enable
        bool  cofl_inte;  // CNTE - counter o'flow int enable
        bool  rdma_inte;  // RDCE - remote DMA complete int enable
        bool  reserved;   // D7   - reserved
    } IMR;
    // Data Configuration Register - 0eh write
    struct {
        bool  wdsize;     // WTS - 8/16-bit select
        bool  endian;     // BOS - byte-order select
        bool  longaddr;   // LAS - long-address select
        bool  loop;       // LS  - loopback select
        bool  auto_rx;    // AR  - auto-remove rx packets with remote DMA
        Bit8u fifo_size;  // FT0,FT1 - fifo threshold
    } DCR;
    // Transmit Configuration Register - 0dh write
    struct {
        bool  crc_disable; // CRC - inhibit tx CRC
        Bit8u loop_cntl;   // LB0,LB1 - loopback control
        bool  ext_stoptx;  // ATD - allow tx disable by external mcast
        bool  coll_prio;   // OFST - backoff algorithm select
        Bit8u reserved;    //  D5,D6,D7 - reserved
    } TCR;
    // Transmit Status Register - 04h read
    struct {
        bool  tx_ok;      // PTX - tx complete without error
        bool  reserved;   //  D1 - reserved
        bool  collided;   // COL - tx collided >= 1 times
        bool  aborted;    // ABT - aborted due to excessive collisions
        bool  no_carrier; // CRS - carrier-sense lost
        bool  fifo_ur;    // FU  - FIFO underrun
        bool  cd_hbeat;   // CDH - no tx cd-heartbeat from transceiver
        bool  ow_coll;    // OWC - out-of-window collision
    } TSR;
    // Receive Configuration Register - 0ch write
    struct {
        bool  errors_ok;  // SEP - accept pkts with rx errors
        bool  runts_ok;   // AR  - accept < 64-byte runts
        bool  broadcast;  // AB  - accept eth broadcast address
        bool  multicast;  // AM  - check mcast hash array
        bool  promisc;    // PRO - accept all packets
        bool  monitor;    // MON - check pkts, but don't rx
        Bit8u reserved;   //  D6,D7 - reserved
    } RCR;
    // Receive Status Register - 0ch read
    struct {
        bool  rx_ok;       // PRX - rx complete without error
        bool  bad_crc;     // CRC - Bad CRC detected
        bool  bad_falign;  // FAE - frame alignment error
        bool  fifo_or;     // FO  - FIFO overrun
        bool  rx_missed;   // MPA - missed packet error
        bool  rx_mbit;     // PHY - unicast or mcast/bcast address match
        bool  rx_disabled; // DIS - set when in monitor mode
        bool  deferred;    // DFR - collision active
    } RSR;

    Bit16u local_dma;	// 01,02h read ; current local DMA addr
    Bit8u  page_start;  // 01h write ; page start register
    Bit8u  page_stop;   // 02h write ; page stop register
    Bit8u  bound_ptr;   // 03h read/write ; boundary pointer
    Bit8u  tx_page_start; // 04h write ; transmit page start register
    Bit8u  num_coll;    // 05h read  ; number-of-collisions register
    Bit16u tx_bytes;    // 05,06h write ; transmit byte-count register
    Bit8u  fifo;	// 06h read  ; FIFO
    Bit16u remote_dma;  // 08,09h read ; current remote DMA addr
    Bit16u remote_start;  // 08,09h write ; remote start address register
    Bit16u remote_bytes;  // 0a,0bh write ; remote byte-count register
    Bit8u  tallycnt_0;  // 0dh read  ; tally counter 0 (frame align errors)
    Bit8u  tallycnt_1;  // 0eh read  ; tally counter 1 (CRC errors)
    Bit8u  tallycnt_2;  // 0fh read  ; tally counter 2 (missed pkt errors)

    //
    // Page 1
    //
    //   Command Register 00h (repeated)
    //
    Bit8u  physaddr[6];  // 01-06h read/write ; MAC address
    Bit8u  curr_page;    // 07h read/write ; current page register
    Bit8u  mchash[8];    // 08-0fh read/write ; multicast hash array

    //
    // Page 2  - diagnostic use only
    //
    //   Command Register 00h (repeated)
    //
    //   Page Start Register 01h read  (repeated)
    //   Page Stop Register  02h read  (repeated)
    //   Current Local DMA Address 01,02h write (repeated)
    //   Transmit Page start address 04h read (repeated)
    //   Receive Configuration Register 0ch read (repeated)
    //   Transmit Configuration Register 0dh read (repeated)
    //   Data Configuration Register 0eh read (repeated)
    //   Interrupt Mask Register 0fh read (repeated)
    //
    Bit8u  rempkt_ptr;   // 03h read/write ; remote next-packet pointer
    Bit8u  localpkt_ptr; // 05h read/write ; local next-packet pointer
    Bit16u address_cnt;  // 06,07h read/write ; address counter

    //
    // Page 3  - should never be modified.
    //

    // Novell ASIC state
    Bit8u  macaddr[32];          // ASIC ROM'd MAC address, even bytes
    Bit8u  mem[BX_NE2K_MEMSIZ];  // on-chip packet memory

    // ne2k internal state
    Bit32u base_address;
    int    base_irq;
    int    tx_timer_index;
    bool   tx_timer_active;
    int    statusbar_id;

    // pci stuff
    bool   pci_enabled;
#if BX_SUPPORT_PCI
    Bit8u  devfunc;
#endif
    char   devname[16];
    char   ldevname[20];
} bx_ne2k_t;


class bx_ne2k_c
#if BX_SUPPORT_PCI
: public bx_pci_device_c
#else
: public bx_devmodel_c
#endif
{
public:
  bx_ne2k_c();
  virtual ~bx_ne2k_c();
  virtual void init_card(Bit8u card);
  virtual void reset(unsigned type);
  void         ne2k_register_state(bx_list_c *parent, Bit8u card);
#if BX_SUPPORT_PCI
  virtual void after_restore_state(void);
#endif
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

#if BX_SUPPORT_PCI
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
  virtual void pci_bar_change_notify(void);
#endif

private:
  bx_ne2k_t s;

  eth_pktmover_c *ethdev;

  Bit32u read_cr(void);
  void   write_cr(Bit32u value);
  void   set_irq_level(bool level);

  Bit32u chipmem_read(Bit32u address, unsigned io_len) BX_CPP_AttrRegparmN(2);
  Bit32u asic_read(Bit32u offset, unsigned io_len) BX_CPP_AttrRegparmN(2);
  Bit32u page0_read(Bit32u offset, unsigned io_len);
  Bit32u page1_read(Bit32u offset, unsigned io_len);
  Bit32u page2_read(Bit32u offset, unsigned io_len);
  Bit32u page3_read(Bit32u offset, unsigned io_len);

  void chipmem_write(Bit32u address, Bit32u value, unsigned io_len) BX_CPP_AttrRegparmN(3);
  void asic_write(Bit32u address, Bit32u value, unsigned io_len);
  void page0_write(Bit32u address, Bit32u value, unsigned io_len);
  void page1_write(Bit32u address, Bit32u value, unsigned io_len);
  void page2_write(Bit32u address, Bit32u value, unsigned io_len);
  void page3_write(Bit32u address, Bit32u value, unsigned io_len);

  static void tx_timer_handler(void *);
  void tx_timer(void);

  static Bit32u rx_status_handler(void *arg);
  Bit32u rx_status(void);
  static void rx_handler(void *arg, const void *buf, unsigned len);
  unsigned mcast_index(const void *dst);
  void rx_frame(const void *buf, unsigned io_len);

#if BX_SUPPORT_PCI
  static bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool mem_read(bx_phy_address addr, unsigned len, void *data);
#endif

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#if BX_DEBUGGER
  void print_info(int page, int reg, int nodups);
#endif
};

class bx_ne2k_main_c : public bx_devmodel_c
{
public:
  bx_ne2k_main_c();
  virtual ~bx_ne2k_main_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
#if BX_SUPPORT_PCI
  virtual void after_restore_state(void);
#endif
private:
  bx_ne2k_c *theNE2kDev[BX_NE2K_MAX_DEVS];
};

#endif
