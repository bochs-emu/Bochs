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


#include "bochs.h"
#define LOG_THIS bx_ne2k.

bx_ne2k_c bx_ne2k;

#if BX_USE_NE2K_SMF
#define this (&bx_ne2k)
#endif

bx_ne2k_c::bx_ne2k_c(void)
{
  setprefix("NE2K");
  settype(NE2KLOG);
  // nothing for now
}


bx_ne2k_c::~bx_ne2k_c(void)
{
  // nothing for now
}

//
// reset_device - restore state to power-up, cancelling all i/o
//
void
bx_ne2k_c::reset_device(void)
{
  BX_DEBUG (("reset_device"));
  // Zero out registers and memory
  memset( & BX_NE2K_THIS s.CR,  0, sizeof(BX_NE2K_THIS s.CR) );
  memset( & BX_NE2K_THIS s.ISR, 0, sizeof(BX_NE2K_THIS s.ISR));
  memset( & BX_NE2K_THIS s.IMR, 0, sizeof(BX_NE2K_THIS s.IMR));
  memset( & BX_NE2K_THIS s.DCR, 0, sizeof(BX_NE2K_THIS s.DCR));
  memset( & BX_NE2K_THIS s.TCR, 0, sizeof(BX_NE2K_THIS s.TCR));
  memset( & BX_NE2K_THIS s.TSR, 0, sizeof(BX_NE2K_THIS s.TSR));
  memset( & BX_NE2K_THIS s.RCR, 0, sizeof(BX_NE2K_THIS s.RCR));
  memset( & BX_NE2K_THIS s.RSR, 0, sizeof(BX_NE2K_THIS s.RSR));
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

  memset( & BX_NE2K_THIS s.physaddr, 0, sizeof(BX_NE2K_THIS s.physaddr));
  memset( & BX_NE2K_THIS s.mchash, 0, sizeof(BX_NE2K_THIS s.mchash));
  BX_NE2K_THIS s.curr_page = 0;

  BX_NE2K_THIS s.rempkt_ptr   = 0;
  BX_NE2K_THIS s.localpkt_ptr = 0;
  BX_NE2K_THIS s.address_cnt  = 0;

  memset( & BX_NE2K_THIS s.mem, 0, sizeof(BX_NE2K_THIS s.mem));
  
  // Set power-up conditions
  BX_NE2K_THIS s.CR.stop      = 1;
  BX_NE2K_THIS s.CR.rdma_cmd  = 1;
  BX_NE2K_THIS s.ISR.reset    = 1;
  BX_NE2K_THIS s.DCR.longaddr = 1;
}

//
// read_cr/write_cr - utility routines for handling reads/writes to
// the Command Register
//
Bit32u
bx_ne2k_c::read_cr(void)
{
  Bit32u val = 
         (((BX_NE2K_THIS s.CR.pgsel    & 0x03) << 6) |
	  ((BX_NE2K_THIS s.CR.rdma_cmd & 0x07) << 3) |
	  (BX_NE2K_THIS s.CR.tx_packet << 2) |
	  (BX_NE2K_THIS s.CR.start     << 1) |
	  (BX_NE2K_THIS s.CR.stop));
  BX_DEBUG(("read CR returns 0x%08x", val));
  return val;
}

void
bx_ne2k_c::write_cr(Bit32u value)
{
  BX_DEBUG(("wrote 0x%08x to CR"));
  // Validate remote-DMA
  if ((value & 0x38) == 0x00)
    return;
  //  BX_PANIC(("CR write - invalid rDMA value 0"));

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

  // Check for start-tx
  if (value & 0x04) {
    if (BX_NE2K_THIS s.CR.stop || !BX_NE2K_THIS s.CR.start)
      BX_PANIC(("CR write - tx start, dev in reset"));
    
    if (BX_NE2K_THIS s.tx_bytes == 0)
      BX_PANIC(("CR write - tx start, tx bytes == 0"));

#ifdef notdef    
    // XXX debug stuff
    printf("packet tx (%d bytes):\t", BX_NE2K_THIS s.tx_bytes);
    for (int i = 0; i < BX_NE2K_THIS s.tx_bytes; i++) {
      printf("%02x ", BX_NE2K_THIS s.mem[BX_NE2K_THIS s.tx_page_start*256 - 
				BX_NE2K_MEMSTART + i]);
      if (i && (((i+1) % 16) == 0)) 
	printf("\t");
    }
    printf("");
#endif    

    // Send the packet to the system driver
    BX_NE2K_THIS ethdev->sendpkt(& BX_NE2K_THIS s.mem[BX_NE2K_THIS s.tx_page_start*256 - BX_NE2K_MEMSTART], BX_NE2K_THIS s.tx_bytes);

    // some more debug
    if (BX_NE2K_THIS s.tx_timer_active)
      BX_PANIC(("CR write, tx timer still active"));
    
    // Schedule a timer to trigger a tx-complete interrupt
    // The number of microseconds is the bit-time / 10.
    // The bit-time is the preamble+sfd (64 bits), the
    // inter-frame gap (96 bits), the CRC (4 bytes), and the
    // the number of bits in the frame (s.tx_bytes * 8).
    //
    bx_pc_system.activate_timer(BX_NE2K_THIS s.tx_timer_index,
				(64 + 96 + 4*8 + BX_NE2K_THIS s.tx_bytes*8)/10,
				0); // not continuous
  }

  // Linux probes for an interrupt by setting up a remote-DMA read
  // of 0 bytes with remote-DMA completion interrupts enabled.
  // Detect this here
  if (BX_NE2K_THIS s.CR.rdma_cmd == 0x01 &&
      BX_NE2K_THIS s.CR.start &&
      BX_NE2K_THIS s.remote_bytes == 0) {
    BX_NE2K_THIS s.ISR.rdma_done = 1;
    if (BX_NE2K_THIS s.IMR.rdma_inte) {
      BX_NE2K_THIS devices->pic->trigger_irq(BX_NE2K_THIS s.base_irq);
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
Bit32u
bx_ne2k_c::chipmem_read(Bit32u address, unsigned int io_len)
{
  Bit32u retval = 0;

  if ((io_len == 2) && (address & 0x1)) 
    BX_PANIC(("unaligned chipmem word read"));

  // ROM'd MAC address
  if ((address >=0) && (address <= 31)) {
    retval = BX_NE2K_THIS s.macaddr[address];
    if (io_len == 2) {
      retval |= (BX_NE2K_THIS s.macaddr[address + 1] << 8);
    }
    return (retval);
  }

  if ((address >= BX_NE2K_MEMSTART) && (address <= BX_NE2K_MEMEND)) {
    retval = BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART];
    if (io_len == 2) {
      retval |= (BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 1] << 8);
    }
    return (retval);
  }

  BX_INFO(("out-of-bounds chipmem read, %04X", address));

  return (0xff);
}

void
bx_ne2k_c::chipmem_write(Bit32u address, Bit32u value, unsigned io_len)
{
  if ((io_len == 2) && (address & 0x1)) 
    BX_PANIC(("unaligned chipmem word write"));

  if ((address >= BX_NE2K_MEMSTART) && (address <= BX_NE2K_MEMEND)) {
    BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART] = value & 0xff;
    if (io_len == 2)
      BX_NE2K_THIS s.mem[address - BX_NE2K_MEMSTART + 1] = value >> 8;
  } else
    BX_INFO(("out-of-bounds chipmem write, %04X", address));
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
Bit32u
bx_ne2k_c::asic_read(Bit32u offset, unsigned int io_len)
{
  Bit32u retval = 0;

  switch (offset) {
  case 0x0:  // Data register
    // 
    // The device must have been set up to perform DMA in
    // the same size as is being requested (the WTS bit
    // in the DCR), a read remote-DMA command must have
    // been issued, and the source-address and length
    // registers must have been initialised.
    //
    if (io_len != (1 + BX_NE2K_THIS s.DCR.wdsize))
      BX_PANIC(("dma read, wrong size %d", io_len));

    if (BX_NE2K_THIS s.remote_bytes == 0)
      BX_PANIC(("ne2K: dma read, byte count 0"));
    
    retval = chipmem_read(BX_NE2K_THIS s.remote_dma, io_len);
    BX_NE2K_THIS s.remote_dma   += io_len;
    BX_NE2K_THIS s.remote_bytes -= io_len;
    break;

  case 0xf:  // Reset register
    reset_device();
    break;

  default:
    BX_INFO(("asic read invalid address %04x", (unsigned) offset));
    break;
  }

  return (retval);
}

void
bx_ne2k_c::asic_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_DEBUG(("asic write addr=0x%08x, value=0x%08x", (unsigned) value, (unsigned) offset));
  switch (offset) {
  case 0x0:  // Data register - see asic_read for a description

    if (io_len != (1 + BX_NE2K_THIS s.DCR.wdsize))
      BX_PANIC(("dma write, wrong size %d", io_len));

    if (BX_NE2K_THIS s.remote_bytes == 0)
      BX_PANIC(("ne2K: dma write, byte count 0"));
    
    chipmem_write(BX_NE2K_THIS s.remote_dma, value, io_len);
    BX_NE2K_THIS s.remote_dma   += io_len;
    BX_NE2K_THIS s.remote_bytes -= io_len;

    // If all bytes have been written, signal remote-DMA complete
    if (BX_NE2K_THIS s.remote_bytes == 0) {
      BX_NE2K_THIS s.ISR.rdma_done = 1;
    }
    break;

  case 0xf:  // Reset register
    reset_device();
    break;

  default:
    BX_PANIC(("asic write invalid address %04x", (unsigned) offset));
    break ;
  }
}

//
// page0_read/page0_write - These routines handle reads/writes to
// the 'zeroth' page of the DS8390 register file
//
Bit32u
bx_ne2k_c::page0_read(Bit32u offset, unsigned int io_len)
{
  BX_DEBUG(("page 0 read from port %04x, len=%u", (unsigned) offset,
	   (unsigned) io_len));
  if (io_len > 1)
    BX_PANIC(("bad length! page 0 read from port %04x, len=%u", (unsigned) offset,
             (unsigned) io_len));

  switch (offset) {
  case 0x0:  // CR
    return (read_cr());
    break;
    
  case 0x1:  // CLDA0
    return (BX_NE2K_THIS s.local_dma & 0xff);
    break;

  case 0x2:  // CLDA1
    return (BX_NE2K_THIS s.local_dma >> 8);
    break;

  case 0x3:  // BNRY
    return (BX_NE2K_THIS s.bound_ptr);
    break;

  case 0x4:  // TSR
    return ((BX_NE2K_THIS s.TSR.ow_coll    << 7) |
	    (BX_NE2K_THIS s.TSR.cd_hbeat   << 6) |
	    (BX_NE2K_THIS s.TSR.fifo_ur    << 5) |
	    (BX_NE2K_THIS s.TSR.no_carrier << 4) |
	    (BX_NE2K_THIS s.TSR.aborted    << 3) |
	    (BX_NE2K_THIS s.TSR.collided   << 2) |
	    (BX_NE2K_THIS s.TSR.tx_ok));
    break;

  case 0x5:  // NCR
    return (BX_NE2K_THIS s.num_coll);
    break;
    
  case 0x6:  // FIFO
    return (BX_NE2K_THIS s.fifo);
    break;
    
  case 0x7:  // ISR
    return ((BX_NE2K_THIS s.ISR.reset     << 7) |
	    (BX_NE2K_THIS s.ISR.rdma_done << 6) |
	    (BX_NE2K_THIS s.ISR.cnt_oflow << 5) |
	    (BX_NE2K_THIS s.ISR.overwrite << 4) |
	    (BX_NE2K_THIS s.ISR.tx_err    << 3) |
	    (BX_NE2K_THIS s.ISR.rx_err    << 2) |
	    (BX_NE2K_THIS s.ISR.pkt_tx    << 1) |
	    (BX_NE2K_THIS s.ISR.pkt_rx));
    break;
    
  case 0x8:  // CRDA0
    return (BX_NE2K_THIS s.remote_dma & 0xff);
    break;

  case 0x9:  // CRDA1
    return (BX_NE2K_THIS s.remote_dma >> 8);
    break;

  case 0xa:  // reserved
    BX_INFO(("reserved read - page 0, 0xa"));
    return (0xff);
    break;

  case 0xb:  // reserved
    BX_INFO(("reserved read - page 0, 0xb"));
    return (0xff);
    break;
    
  case 0xc:  // RSR
    return ((BX_NE2K_THIS s.RSR.deferred    << 7) |
	    (BX_NE2K_THIS s.RSR.rx_disabled << 6) |
	    (BX_NE2K_THIS s.RSR.rx_mbit     << 5) |
	    (BX_NE2K_THIS s.RSR.rx_missed   << 4) |
	    (BX_NE2K_THIS s.RSR.fifo_or     << 3) |
	    (BX_NE2K_THIS s.RSR.bad_falign  << 2) |
	    (BX_NE2K_THIS s.RSR.bad_crc     << 1) |
	    (BX_NE2K_THIS s.RSR.rx_ok));
    break;
    
  case 0xd:  // CNTR0
    return (BX_NE2K_THIS s.tallycnt_0);
    break;

  case 0xe:  // CNTR1
    return (BX_NE2K_THIS s.tallycnt_1);
    break;

  case 0xf:  // CNTR2
    return (BX_NE2K_THIS s.tallycnt_2);
    break;

  default:
    BX_PANIC(("page 0 offset %04x out of range", (unsigned) offset));
  }

  return(0);
}

void
bx_ne2k_c::page0_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_DEBUG(("page 0 write to port %04x, len=%u", (unsigned) offset,
	   (unsigned) io_len));
  if (io_len > 1)
    BX_PANIC(("bad length! page 0 write to port %04x, len=%u", (unsigned) offset,
             (unsigned) io_len));
  
  switch (offset) {
  case 0x0:  // CR
    write_cr(value);
    break;

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
    BX_NE2K_THIS s.ISR.pkt_rx    &= ~((value & 0x01) == 0x01);
    BX_NE2K_THIS s.ISR.pkt_tx    &= ~((value & 0x02) == 0x02);
    BX_NE2K_THIS s.ISR.rx_err    &= ~((value & 0x04) == 0x04);
    BX_NE2K_THIS s.ISR.tx_err    &= ~((value & 0x08) == 0x08);
    BX_NE2K_THIS s.ISR.overwrite &= ~((value & 0x10) == 0x10);
    BX_NE2K_THIS s.ISR.cnt_oflow &= ~((value & 0x20) == 0x20);
    BX_NE2K_THIS s.ISR.rdma_done &= ~((value & 0x40) == 0x40);
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
      BX_PANIC(("TCR write, reserved bits set"));

    // Test loop mode (not supported)
    if (value & 0x06) {
      BX_INFO(("TCR write, loop mode not supported"));
      BX_NE2K_THIS s.TCR.loop_cntl = (value & 0x6) >> 1;
    } else {
      BX_NE2K_THIS s.TCR.loop_cntl = 0;
    }

    // Inhibit-CRC not supported.
    if (value & 0x01)
      BX_PANIC(("TCR write, inhibit-CRC not supported"));

    // Auto-transmit disable very suspicious
    if (value & 0x04)
      BX_PANIC(("TCR write, auto transmit disable not supported"));

    // Allow collision-offset to be set, although not used
    BX_NE2K_THIS s.TCR.coll_prio = ((value & 0x08) == 0x08);
    break;

  case 0xe:  // DCR
    // Don't allow loopback mode to be set
    if (!(value & 0x08)) {
      BX_ERROR(("DCR write, loopback mode selected"));
	  // XXX This is a HACK, lets fix this right!
	  value -= 8;
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
    BX_NE2K_THIS s.DCR.auto_rx  = ((value & 0x10) == 0x10); // also illegal ?
    BX_NE2K_THIS s.DCR.fifo_size = (value & 0x50) >> 5;
    break;

  case 0xf:  // IMR
    // Check for reserved bit
    if (value & 0x80)
      BX_PANIC(("IMR write, reserved bit set"));

    // Set other values
    BX_NE2K_THIS s.IMR.rx_inte    = ((value & 0x01) == 0x01);
    BX_NE2K_THIS s.IMR.tx_inte    = ((value & 0x02) == 0x02);
    BX_NE2K_THIS s.IMR.rxerr_inte = ((value & 0x04) == 0x04);
    BX_NE2K_THIS s.IMR.txerr_inte = ((value & 0x08) == 0x08);
    BX_NE2K_THIS s.IMR.overw_inte = ((value & 0x10) == 0x10);
    BX_NE2K_THIS s.IMR.cofl_inte  = ((value & 0x20) == 0x20);
    BX_NE2K_THIS s.IMR.rdma_inte  = ((value & 0x40) == 0x40);
    break;

  default:
    BX_PANIC(("page 0 write, bad offset %0x", offset));
  }
}


//
// page1_read/page1_write - These routines handle reads/writes to
// the first page of the DS8390 register file
//
Bit32u
bx_ne2k_c::page1_read(Bit32u offset, unsigned int io_len)
{
  BX_DEBUG(("page 1 read from port %04x, len=%u", (unsigned) offset,
	   (unsigned) io_len));
  if (io_len > 1)
    BX_PANIC(("bad length! page 1 read from port %04x, len=%u", (unsigned) offset,
             (unsigned) io_len));

  switch (offset) {
  case 0x0:  // CR
    return (read_cr());
    break;
    
  case 0x1:  // PAR0-5
  case 0x2:
  case 0x3:
  case 0x4:
  case 0x5:
  case 0x6:
    return (BX_NE2K_THIS s.physaddr[offset - 1]);
    break;

  case 0x7:  // CURR
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
    BX_PANIC(("page 1 r offset %04x out of range", (unsigned) offset));
  }

  return (0);
}

void
bx_ne2k_c::page1_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_DEBUG(("page 1 w offset %04x", (unsigned) offset));
  switch (offset) {
  case 0x0:  // CR
    write_cr(value);
    break;  

  case 0x1:  // PAR0-5
  case 0x2:
  case 0x3:
  case 0x4:
  case 0x5:
  case 0x6:
    BX_NE2K_THIS s.physaddr[offset - 1] = value;
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
    BX_PANIC(("page 1 w offset %04x out of range", (unsigned) offset));
  }  
}


//
// page2_read/page2_write - These routines handle reads/writes to
// the second page of the DS8390 register file
//
Bit32u
bx_ne2k_c::page2_read(Bit32u offset, unsigned int io_len)
{
  BX_DEBUG(("page 2 read from port %04x, len=%u", (unsigned) offset, (unsigned) io_len));

  if (io_len > 1)
    BX_PANIC(("bad length!  page 2 read from port %04x, len=%u", (unsigned) offset, (unsigned) io_len));

  switch (offset) {
  case 0x0:  // CR
    return (read_cr());
    break;

  case 0x1:  // PSTART
    return (BX_NE2K_THIS s.page_start);
    break;

  case 0x2:  // PSTOP
    return (BX_NE2K_THIS s.page_stop);
    break;

  case 0x3:  // Remote Next-packet pointer
    return (BX_NE2K_THIS s.rempkt_ptr);
    break;

  case 0x4:  // TPSR
    return (BX_NE2K_THIS s.tx_page_start);
    break;

  case 0x5:  // Local Next-packet pointer
    return (BX_NE2K_THIS s.localpkt_ptr);
    break;

  case 0x6:  // Address counter (upper)
    return (BX_NE2K_THIS s.address_cnt >> 8);
    break;

  case 0x7:  // Address counter (lower)
    return (BX_NE2K_THIS s.address_cnt & 0xff);
    break;

  case 0x8:  // Reserved
  case 0x9:
  case 0xa:
  case 0xb:
    BX_ERROR(("reserved read - page 2, 0x%02x", (unsigned) offset));
    return (0xff);
    break;

  case 0xc:  // RCR
    return ((BX_NE2K_THIS s.RCR.monitor   << 5) |
	    (BX_NE2K_THIS s.RCR.promisc   << 4) |
	    (BX_NE2K_THIS s.RCR.multicast << 3) |
	    (BX_NE2K_THIS s.RCR.broadcast << 2) |
	    (BX_NE2K_THIS s.RCR.runts_ok  << 1) |
	    (BX_NE2K_THIS s.RCR.errors_ok));
    break;

  case 0xd:  // TCR
    return ((BX_NE2K_THIS s.TCR.coll_prio   << 4) |
	    (BX_NE2K_THIS s.TCR.ext_stoptx  << 3) |
	    ((BX_NE2K_THIS s.TCR.loop_cntl & 0x3) << 1) |
	    (BX_NE2K_THIS s.TCR.crc_disable));
    break;

  case 0xe:  // DCR
    return (((BX_NE2K_THIS s.DCR.fifo_size & 0x3) << 5) |
	    (BX_NE2K_THIS s.DCR.auto_rx  << 4) |
	    (BX_NE2K_THIS s.DCR.loop     << 3) |
	    (BX_NE2K_THIS s.DCR.longaddr << 2) |
	    (BX_NE2K_THIS s.DCR.endian   << 1) |
	    (BX_NE2K_THIS s.DCR.wdsize));
    break;

  case 0xf:  // IMR
    return ((BX_NE2K_THIS s.IMR.rdma_inte  << 6) |
	    (BX_NE2K_THIS s.IMR.cofl_inte  << 5) |
	    (BX_NE2K_THIS s.IMR.overw_inte << 4) |
	    (BX_NE2K_THIS s.IMR.txerr_inte << 3) |
	    (BX_NE2K_THIS s.IMR.rxerr_inte << 2) |
	    (BX_NE2K_THIS s.IMR.tx_inte    << 1) |
	    (BX_NE2K_THIS s.IMR.rx_inte));
    break;

  default:
    BX_PANIC(("page 2 offset %04x out of range", (unsigned) offset));
  }

  return (0);
};

void
bx_ne2k_c::page2_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  // Maybe all writes here should be BX_PANIC()'d, since they
  // affect internal operation, but let them through for now
  // and print a warning.
  if (offset != 0)
    BX_ERROR(("page 2 write ?"));

  switch (offset) {
  case 0x0:  // CR
    write_cr(value);
    break; 

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
    BX_PANIC(("page 2 write to reserved offset 4"));
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
    BX_PANIC(("page 2 write to reserved offset %0x", offset));
    break;
   
  default:
    BX_PANIC(("page 2 write, illegal offset %0x", offset));
    break;
  }
}
  
//
// page3_read/page3_write - writes to this page are illegal
//
Bit32u
bx_ne2k_c::page3_read(Bit32u offset, unsigned int io_len)
{
  BX_PANIC(("page 3 read attempted"));
  return (0);
}

void
bx_ne2k_c::page3_write(Bit32u offset, Bit32u value, unsigned io_len)
{
  BX_PANIC(("page 3 write attempted"));
}

//
// tx_timer_handler/tx_timer
//
void
bx_ne2k_c::tx_timer_handler(void *this_ptr)
{
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;

  class_ptr->tx_timer();
}

void
bx_ne2k_c::tx_timer(void)
{
  BX_DEBUG(("tx_timer"));
  BX_NE2K_THIS s.TSR.tx_ok = 1;
  // Generate an interrupt if not masked and not one in progress
  if (BX_NE2K_THIS s.IMR.tx_inte && !BX_NE2K_THIS s.ISR.pkt_tx) {
    BX_NE2K_THIS s.ISR.pkt_tx = 1;
    BX_NE2K_THIS devices->pic->trigger_irq(BX_NE2K_THIS s.base_irq);
  }
  BX_NE2K_THIS s.tx_timer_active = 0;
}


//
// read_handler/read - i/o 'catcher' function called from BOCHS
// mainline when the CPU attempts a read in the i/o space registered
// by this ne2000 instance
//
Bit32u
bx_ne2k_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_NE2K_SMF
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}

Bit32u
bx_ne2k_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_NE2K_SMF
  BX_DEBUG(("read addr %x, len %d", address, io_len));
  Bit32u retval;
  int offset = address - BX_NE2K_THIS s.base_address;

  if (offset >= 0x10) {
    retval = asic_read(offset - 0x10, io_len);
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
void
bx_ne2k_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, 
			 unsigned io_len)
{
#if !BX_USE_NE2K_SMF
  bx_ne2k_c *class_ptr = (bx_ne2k_c *) this_ptr;
  
  class_ptr->write(address, value, io_len);
}

void
bx_ne2k_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_NE2K_SMF
  BX_DEBUG(("write with length %d", io_len));
  int offset = address - BX_NE2K_THIS s.base_address;

  //
  // The high 16 bytes of i/o space are for the ne2000 asic -
  //  the low 16 bytes are for the DS8390, with the current
  //  page being selected by the PS0,PS1 registers in the
  //  command register
  //
  if (offset >= 0x10) {
    asic_write(offset - 0x10, value, io_len);
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
unsigned
bx_ne2k_c::mcast_index(const void *dst)
{
#define POLYNOMIAL 0x04c11db6
  unsigned long crc = 0xffffffffL;
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
 * Callback from the eth system driver when a frame has arrived
 */
void
bx_ne2k_c::rx_handler(void *arg, const void *buf, unsigned len)
{
  BX_DEBUG(("rx_handler with length %d", len));
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
void
bx_ne2k_c::rx_frame(const void *buf, unsigned io_len)
{
  unsigned pages;
  unsigned avail;
  unsigned idx;
  int wrapped;
  int nextpage;
  unsigned char pkthdr[4];
  unsigned char *pktbuf = (unsigned char *) buf;
  unsigned char *startptr;
  static unsigned char bcast_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

  BX_DEBUG(("rx_frame with length %d", io_len));

  if ((BX_NE2K_THIS s.CR.start == 0) ||
      (BX_NE2K_THIS s.page_start == 0) ||
      (BX_NE2K_THIS s.TCR.loop_cntl != 0)) {
    return;
  }

  // Add the pkt header + CRC to the length, and work
  // out how many 256-byte pages the frame would occupy
  pages = (io_len + 4 + 4)/256 + 1;

  if (BX_NE2K_THIS s.curr_page < BX_NE2K_THIS s.bound_ptr) {
    avail = BX_NE2K_THIS s.bound_ptr - BX_NE2K_THIS s.curr_page;    
  } else {
    avail = (BX_NE2K_THIS s.page_stop - BX_NE2K_THIS s.page_start) -
      (BX_NE2K_THIS s.curr_page - BX_NE2K_THIS s.bound_ptr);
    wrapped = 1;
  }

  // Avoid getting into a buffer overflow condition by not attempting
  // to do partial receives. The emulation to handle this condition
  // seems particularly painful.
  if (avail < pages) {
    return;
  }

  if ((io_len < 60) && !BX_NE2K_THIS s.RCR.runts_ok) {
    return;
  }

  // Do address filtering if not in promiscuous mode
  if (!BX_NE2K_THIS s.RCR.promisc) {
    if (!memcmp(buf, bcast_addr, 6)) {
      if (!BX_NE2K_THIS s.RCR.broadcast) {
	return;
      }
    } else if (pktbuf[0] & 0x01) {
      idx = mcast_index(buf);
      if (!(BX_NE2K_THIS s.mchash[idx >> 3] & (1 << (idx & 0x7)))) {
	return;
      }
    } else if (memcmp(buf, BX_NE2K_THIS s.physaddr, 6)) {
      return;
    }
  }

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
  pkthdr[2] = (io_len + 8) & 0xff;	// length-low
  pkthdr[3] = (io_len + 8) >> 8;	// length-hi

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
  if (pktbuf[0] & 0x80) {
    BX_NE2K_THIS s.RSR.rx_mbit = 1;
  }

  BX_NE2K_THIS s.ISR.pkt_rx = 1;

  if (BX_NE2K_THIS s.IMR.rx_inte) {
    BX_NE2K_THIS devices->pic->trigger_irq(BX_NE2K_THIS s.base_irq);
  }

}

void
bx_ne2k_c::init(bx_devices_c *d)
{
  BX_NE2K_THIS devices = d;

  BX_DEBUG(("Init."));

  if (bx_options.ne2k.Ovalid->get ()) {
    // Bring the register state into power-up state
    reset_device();

    // Read in values from config file
    BX_NE2K_THIS s.base_address = bx_options.ne2k.Oioaddr->get ();
    BX_NE2K_THIS s.base_irq     = bx_options.ne2k.Oirq->get ();
    memcpy(BX_NE2K_THIS s.physaddr, bx_options.ne2k.Omacaddr->getptr (), 6);

    BX_NE2K_THIS s.tx_timer_index =
      bx_pc_system.register_timer(this, tx_timer_handler, 0,
				  0,0); // one-shot, inactive
    // Register the IRQ and i/o port addresses
    BX_NE2K_THIS devices->register_irq(BX_NE2K_THIS s.base_irq,
				       "ne2000 ethernet NIC");

    for (unsigned addr = BX_NE2K_THIS s.base_address; 
	 addr <= BX_NE2K_THIS s.base_address + 0x20; 
	 addr++) {
      BX_NE2K_THIS devices->register_io_read_handler(this,
						     read_handler,
						     addr, 
						     "ne2000 NIC");
      BX_NE2K_THIS devices->register_io_write_handler(this,
						      write_handler,
						      addr, 
						      "ne2000 NIC");
    }
	BX_INFO(("irq %d, ioport 0x%x, mac %02x:%02x:%02x:%02x:%02x:%02x",
				BX_NE2K_THIS s.base_irq,
				BX_NE2K_THIS s.base_address,
				BX_NE2K_THIS s.physaddr[0],
				BX_NE2K_THIS s.physaddr[1],
				BX_NE2K_THIS s.physaddr[2],
				BX_NE2K_THIS s.physaddr[3],
				BX_NE2K_THIS s.physaddr[4],
				BX_NE2K_THIS s.physaddr[5]));
    
    // Initialise the mac address area by doubling the physical address
    BX_NE2K_THIS s.macaddr[0]  = BX_NE2K_THIS s.physaddr[0];
    BX_NE2K_THIS s.macaddr[1]  = BX_NE2K_THIS s.physaddr[0];
    BX_NE2K_THIS s.macaddr[2]  = BX_NE2K_THIS s.physaddr[1];
    BX_NE2K_THIS s.macaddr[3]  = BX_NE2K_THIS s.physaddr[1];
    BX_NE2K_THIS s.macaddr[4]  = BX_NE2K_THIS s.physaddr[2];
    BX_NE2K_THIS s.macaddr[5]  = BX_NE2K_THIS s.physaddr[2];
    BX_NE2K_THIS s.macaddr[6]  = BX_NE2K_THIS s.physaddr[3];
    BX_NE2K_THIS s.macaddr[7]  = BX_NE2K_THIS s.physaddr[3];
    BX_NE2K_THIS s.macaddr[8]  = BX_NE2K_THIS s.physaddr[4];
    BX_NE2K_THIS s.macaddr[9]  = BX_NE2K_THIS s.physaddr[4];
    BX_NE2K_THIS s.macaddr[10] = BX_NE2K_THIS s.physaddr[5];
    BX_NE2K_THIS s.macaddr[11] = BX_NE2K_THIS s.physaddr[5];
    
    // ne2k signature
    for (int i = 12; i < 32; i++) 
      BX_NE2K_THIS s.macaddr[i] = 0x57;
    
    // Attach to the simulated ethernet dev
    BX_NE2K_THIS ethdev = eth_locator_c::create(bx_options.ne2k.Oethmod->getptr (), 
						bx_options.ne2k.Oethdev->getptr (),
				    (const char *) bx_options.ne2k.Omacaddr->getptr (),
						rx_handler, 
						this);
    
    if (BX_NE2K_THIS ethdev == NULL) {
      BX_INFO(("could not find eth module %s - using null instead",
		bx_options.ne2k.Oethmod->getptr ()));
      
      BX_NE2K_THIS ethdev = eth_locator_c::create("null", NULL,
				    (const char *) bx_options.ne2k.Omacaddr->getptr (),
						  rx_handler, 
						  this);
      if (BX_NE2K_THIS ethdev == NULL)
	BX_PANIC(("could not locate null module"));
    }
  }
}
