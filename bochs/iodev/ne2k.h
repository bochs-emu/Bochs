/////////////////////////////////////////////////////////////////////////
// $Id: ne2k.h,v 1.6 2001-10-03 13:10:38 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// Peter Grehan (grehan@iprg.nokia.com) coded all of this
// NE2000/ether stuff.

//
// An implementation of an ne2000 ISA ethernet adapter. This part uses
// a National Semiconductor DS-8390 ethernet MAC chip, with some h/w
// to provide a windowed memory region for the chip and a MAC address.
//


#if BX_USE_NE2K_SMF
#  define BX_NE2K_SMF  static
#  define BX_NE2K_THIS bx_ne2k.
#else
#  define BX_NE2K_SMF
#  define BX_NE2K_THIS this->
#endif

#define  BX_NE2K_MEMSIZ    (32*1024)
#define  BX_NE2K_MEMSTART  (16*1024)
#define  BX_NE2K_MEMEND    (BX_NE2K_MEMSTART + BX_NE2K_MEMSIZ)

typedef struct {
    //
    // ne2k register state
   
    //
    // Page 0
    //
    //  Command Register - 00h read/write
    struct {
        Boolean  stop;		// STP - Software Reset command
        Boolean  start;		// START - start the NIC
        Boolean  tx_packet;	// TXP - initiate packet transmission
        Bit8u    rdma_cmd;      // RD0,RD1,RD2 - Remote DMA command
	Bit8u	 pgsel;		// PS0,PS1 - Page select
    } CR;
    // Interrupt Status Register - 07h read/write
    struct {  
	Boolean  pkt_rx;       	// PRX - packet received with no errors
	Boolean  pkt_tx;       	// PTX - packet transmitted with no errors
	Boolean  rx_err;	// RXE - packet received with 1 or more errors
	Boolean  tx_err;	// TXE - packet tx'd       "  " "    "    "
	Boolean  overwrite;	// OVW - rx buffer resources exhausted
	Boolean  cnt_oflow;     // CNT - network tally counter MSB's set
	Boolean  rdma_done;     // RDC - remote DMA complete
	Boolean  reset;		// RST - reset status
    } ISR;
    // Interrupt Mask Register - 0fh write
    struct {
	Boolean  rx_inte;	// PRXE - packet rx interrupt enable
	Boolean  tx_inte;	// PTXE - packet tx interrput enable
	Boolean  rxerr_inte;	// RXEE - rx error interrupt enable
	Boolean  txerr_inte;	// TXEE - tx error interrupt enable
	Boolean  overw_inte;	// OVWE - overwrite warn int enable
	Boolean  cofl_inte;	// CNTE - counter o'flow int enable
	Boolean  rdma_inte;	// RDCE - remote DMA complete int enable
	Boolean  reserved;	//  D7 - reserved
    } IMR;
    // Data Configuration Register - 0eh write
    struct {
	Boolean  wdsize;	// WTS - 8/16-bit select
	Boolean  endian;	// BOS - byte-order select
	Boolean  longaddr;	// LAS - long-address select
	Boolean  loop;		// LS  - loopback select
	Boolean  auto_rx;	// AR  - auto-remove rx packets with remote DMA
        Bit8u    fifo_size;	// FT0,FT1 - fifo threshold
    } DCR;
    // Transmit Configuration Register - 0dh write
    struct {
	Boolean  crc_disable;	// CRC - inhibit tx CRC
	Bit8u    loop_cntl;	// LB0,LB1 - loopback control
	Boolean  ext_stoptx;    // ATD - allow tx disable by external mcast
	Boolean  coll_prio;	// OFST - backoff algorithm select
	Bit8u    reserved;      //  D5,D6,D7 - reserved
    } TCR;
    // Transmit Status Register - 04h read
    struct {
	Boolean  tx_ok;		// PTX - tx complete without error
	Boolean  reserved;	//  D1 - reserved
	Boolean  collided;	// COL - tx collided >= 1 times
	Boolean  aborted;	// ABT - aborted due to excessive collisions
	Boolean  no_carrier;	// CRS - carrier-sense lost
	Boolean  fifo_ur;	// FU  - FIFO underrun
	Boolean  cd_hbeat;	// CDH - no tx cd-heartbeat from transceiver
	Boolean  ow_coll;	// OWC - out-of-window collision
    } TSR;
    // Receive Configuration Register - 0ch write
    struct {
	Boolean  errors_ok;	// SEP - accept pkts with rx errors
	Boolean  runts_ok;	// AR  - accept < 64-byte runts
	Boolean  broadcast;	// AB  - accept eth broadcast address
	Boolean  multicast;	// AM  - check mcast hash array
	Boolean  promisc;	// PRO - accept all packets
	Boolean  monitor;	// MON - check pkts, but don't rx
	Bit8u    reserved;	//  D6,D7 - reserved
    } RCR;
    // Receive Status Register - 0ch read
    struct {
	Boolean  rx_ok;		// PRX - rx complete without error
	Boolean  bad_crc;	// CRC - Bad CRC detected
	Boolean  bad_falign;	// FAE - frame alignment error
	Boolean  fifo_or;	// FO  - FIFO overrun
	Boolean  rx_missed;	// MPA - missed packet error
	Boolean  rx_mbit;	// PHY - unicast or mcast/bcast address match
	Boolean  rx_disabled;   // DIS - set when in monitor mode
	Boolean  deferred;	// DFR - collision active
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
    int    tx_timer_active;

} bx_ne2k_t;



class bx_ne2k_c : public logfunctions {
public:
  bx_ne2k_c(void);
  ~bx_ne2k_c(void);
  BX_NE2K_SMF void   init(bx_devices_c *);
  void print_info (FILE *file, int page, int reg, int nodups);

private:
  bx_ne2k_t s;

  bx_devices_c *devices;

  eth_pktmover_c *ethdev;

  BX_NE2K_SMF void   reset_device(void);
  BX_NE2K_SMF Bit32u read_cr(void);
  BX_NE2K_SMF void   write_cr(Bit32u value);

  BX_NE2K_SMF Bit32u chipmem_read(Bit32u address, unsigned io_len);
  BX_NE2K_SMF Bit32u asic_read(Bit32u offset, unsigned io_len);
  BX_NE2K_SMF Bit32u page0_read(Bit32u offset, unsigned io_len);
  BX_NE2K_SMF Bit32u page1_read(Bit32u offset, unsigned io_len);
  BX_NE2K_SMF Bit32u page2_read(Bit32u offset, unsigned io_len);
  BX_NE2K_SMF Bit32u page3_read(Bit32u offset, unsigned io_len);

  BX_NE2K_SMF void chipmem_write(Bit32u address, Bit32u value, unsigned io_len);
  BX_NE2K_SMF void asic_write(Bit32u address, Bit32u value, unsigned io_len);
  BX_NE2K_SMF void page0_write(Bit32u address, Bit32u value, unsigned io_len);
  BX_NE2K_SMF void page1_write(Bit32u address, Bit32u value, unsigned io_len);
  BX_NE2K_SMF void page2_write(Bit32u address, Bit32u value, unsigned io_len);
  BX_NE2K_SMF void page3_write(Bit32u address, Bit32u value, unsigned io_len);

  static void tx_timer_handler(void *);
  BX_NE2K_SMF void tx_timer(void);

  static void rx_handler(void *arg, const void *buf, unsigned len);
  BX_NE2K_SMF unsigned mcast_index(const void *dst);
  BX_NE2K_SMF void rx_frame(const void *buf, unsigned io_len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_NE2K_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#if BX_USE_NE2K_SMF
extern bx_ne2k_c bx_ne2k;
#endif
