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



#include "bochs.h"
#define LOG_THIS BX_DMA_THIS

#define DMA_MODE_DEMAND  0
#define DMA_MODE_SINGLE  1
#define DMA_MODE_BLOCK   2
#define DMA_MODE_CASCADE 3



#if BX_USE_DMA_SMF
bx_dma_c bx_dma;
#define this (&bx_dma)
#endif



bx_dma_c::bx_dma_c(void)
{
	setprefix("[DMA ]",__FILE__,__LINE__);
	settype(DMALOG);
	setio(SAFE_GET_IOFUNC());
}

bx_dma_c::~bx_dma_c(void)
{
}


  void
bx_dma_c::init(bx_devices_c *d)
{
  unsigned c;

  BX_DMA_THIS devices = d;

  /* 8237 DMA controller */

  // 0000..000F
  unsigned i;
  for (i=0x0000; i<=0x000F; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }

  // 00081..008D
  for (i=0x0081; i<=0x008D; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }

  // 0008F..008F
  for (i=0x008F; i<=0x008F; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }

  // 000C0..00DE
  for (i=0x00C0; i<=0x00DE; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }


  BX_DMA_THIS s.mask[0] = 1; // channel 0 masked
  BX_DMA_THIS s.mask[1] = 1; // channel 1 masked
  BX_DMA_THIS s.mask[2] = 1; // channel 2 masked
  BX_DMA_THIS s.mask[3] = 1; // channel 3 masked

  BX_DMA_THIS s.flip_flop = 0; /* cleared */
  BX_DMA_THIS s.status_reg = 0; // no requests, no terminal counts reached
  for (c=0; c<4; c++) {
    BX_DMA_THIS s.chan[c].mode.mode_type = 0;         // demand mode
    BX_DMA_THIS s.chan[c].mode.address_decrement = 0; // address increment
    BX_DMA_THIS s.chan[c].mode.autoinit_enable = 0;   // autoinit disable
    BX_DMA_THIS s.chan[c].mode.transfer_type = 0;     // verify
    BX_DMA_THIS s.chan[c].base_address = 0;
    BX_DMA_THIS s.chan[c].current_address = 0;
    BX_DMA_THIS s.chan[c].base_count = 0;
    BX_DMA_THIS s.chan[c].current_count = 0;
    BX_DMA_THIS s.chan[c].page_reg = 0;
    }
}



  // index to find channel from register number (only [0],[1],[2],[6] used)
  Bit8u channelindex[7] = {2, 3, 1, 0, 0, 0, 0};


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_dma_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_DMA_SMF
  bx_dma_c *class_ptr = (bx_dma_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}

  /* 8237 DMA controller */
  Bit32u
bx_dma_c::read( Bit32u   address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DMA_SMF

  Bit8u retval;
  Bit8u channel;

  if (io_len > 1) {
    BX_PANIC(("dma: io read from address %08x, len=%u\n",
             (unsigned) address, (unsigned) io_len));
    }

  if (bx_dbg.dma)
    BX_INFO(("dma: read addr=%04x\n", (unsigned) address));

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return a bogus value */
  return(0xff);
#endif

  switch (address) {
    case 0x00: /* DMA-1 current address, channel 0 */
    case 0x02: /* DMA-1 current address, channel 1 */
    case 0x04: /* DMA-1 current address, channel 2 */
    case 0x06: /* DMA-1 current address, channel 3 */
      channel = address >> 1;
      if (BX_DMA_THIS s.flip_flop==0) {
        BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
        return(BX_DMA_THIS s.chan[channel].current_address & 0xff);
        }
      else {
        BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
        return(BX_DMA_THIS s.chan[channel].current_address >> 8);
        }

    case 0x01: /* DMA-1 current count, channel 0 */
    case 0x03: /* DMA-1 current count, channel 1 */
    case 0x05: /* DMA-1 current count, channel 2 */
    case 0x07: /* DMA-1 current count, channel 3 */
      channel = address >> 1;
      if (BX_DMA_THIS s.flip_flop==0) {
        BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
        return(BX_DMA_THIS s.chan[channel].current_count & 0xff);
        }
      else {
        BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
        return(BX_DMA_THIS s.chan[channel].current_count >> 8);
        }

    case 0x08: // DMA-1 Status Register
      // bit 7: 1 = channel 3 request
      // bit 6: 1 = channel 2 request
      // bit 5: 1 = channel 1 request
      // bit 4: 1 = channel 0 request
      // bit 3: 1 = channel 3 has reached terminal count
      // bit 2: 1 = channel 2 has reached terminal count
      // bit 1: 1 = channel 1 has reached terminal count
      // bit 0: 1 = channel 0 has reached terminal count
      // reading this register clears lower 4 bits (hold flags)
      retval = BX_DMA_THIS s.status_reg;
      BX_DMA_THIS s.status_reg &= 0xf0;
      return(retval);
      break;
    case 0x0d: // dma-1: temporary register
      BX_PANIC(("dma-1: read of temporary register\n"));
      // Note: write to 0x0D clears temporary register
      return(0);
      break;

    case 0x0081: // DMA-1 page register, channel 2
    case 0x0082: // DMA-1 page register, channel 3
    case 0x0083: // DMA-1 page register, channel 1
    case 0x0087: // DMA-1 page register, channel 0
      channel = channelindex[address - 0x81];
      return( BX_DMA_THIS s.chan[channel].page_reg );

    case 0x0084: // ???
      return(0);

    case 0x0089: // DMA-2 page register, channel 6
    case 0x008a: // DMA-2 page register, channel 7
    case 0x008b: // DMA-2 page register, channel 5
    case 0x008f: // DMA-2 page register, channel 4
      channel = channelindex[address - 0x89] + 4;
      BX_INFO(("dma: read: unsupported address=%04x (channel %d)\n", 
		(unsigned) address, channel));
      return( 0x00 );

    case 0x00c0:
    case 0x00c2:
    case 0x00c4:
    case 0x00c6:
    case 0x00c8:
    case 0x00ca:
    case 0x00cc:
    case 0x00ce:
    case 0x00d0:
    case 0x00d2:
    case 0x00d4:
    case 0x00d6:
    case 0x00d8:
    case 0x00da:
    case 0x00dc:
    case 0x00de:
      BX_INFO(("dma: read: unsupported address=%04x\n", (unsigned) address));
      return(0x0000);
      break;

    default:
      BX_PANIC(("dma: read: unsupported address=%04x\n", (unsigned) address));
      return(0);
    }
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_dma_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_DMA_SMF
  bx_dma_c *class_ptr = (bx_dma_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}


  /* 8237 DMA controller */
  void
bx_dma_c::write(Bit32u   address, Bit32u   value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DMA_SMF
  Bit8u set_mask_bit;
  Bit8u channel;

  if (io_len > 1) {
    if ( (io_len == 2) && (address == 0x0b) ) {
#if BX_USE_DMA_SMF
      BX_DMA_THIS write_handler(NULL, address,   value & 0xff, 1);
      BX_DMA_THIS write_handler(NULL, address+1, value >> 8,   1);
#else
      BX_DMA_THIS write(address,   value & 0xff, 1);
      BX_DMA_THIS write(address+1, value >> 8,   1);
#endif
      return;
      }

    BX_PANIC(("dma: io write to address %08x, len=%u\n",
             (unsigned) address, (unsigned) io_len));
    }

  if (bx_dbg.dma)
    BX_INFO(("\ndma: write: address=%04x value=%02x\n",
      (unsigned) address, (unsigned) value));

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return */
  return;
#endif

  switch (address) {
    case 0x00:
    case 0x02:
    case 0x04:
    case 0x06:
      channel = address >> 1;
      if (bx_dbg.dma)
        BX_INFO(("  DMA-1 base and current address, channel %d\n", channel));
      if (BX_DMA_THIS s.flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s.chan[channel].base_address = value;
        BX_DMA_THIS s.chan[channel].current_address = value;
        }
      else { /* 2nd byte */
        BX_DMA_THIS s.chan[channel].base_address |= (value << 8);
        BX_DMA_THIS s.chan[channel].current_address |= (value << 8);
        if (bx_dbg.dma) {
          BX_INFO(("    base = %04x\n",
            (unsigned) BX_DMA_THIS s.chan[channel].base_address));
          BX_INFO(("    curr = %04x\n",
            (unsigned) BX_DMA_THIS s.chan[channel].current_address));
          }
        }
      BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
      return;
      break;

    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:
      channel = address >> 1;
      if (bx_dbg.dma)
        BX_INFO(("  DMA-1 base and current count, channel %d\n", channel));
      if (BX_DMA_THIS s.flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s.chan[channel].base_count = value;
        BX_DMA_THIS s.chan[channel].current_count = value;
        }
      else { /* 2nd byte */
        BX_DMA_THIS s.chan[channel].base_count |= (value << 8);
        BX_DMA_THIS s.chan[channel].current_count |= (value << 8);
        if (bx_dbg.dma) {
          BX_INFO(("    base = %04x\n",
            (unsigned) BX_DMA_THIS s.chan[channel].base_count));
          BX_INFO(("    curr = %04x\n",
            (unsigned) BX_DMA_THIS s.chan[channel].current_count));
          }
        }
      BX_DMA_THIS s.flip_flop = !BX_DMA_THIS s.flip_flop;
      return;
      break;

    case 0x08: /* DMA-1: command register */
      if (value != 0x04)
        BX_INFO(("DMA: write to 0008: value(%02xh) not 04h\n",
          (unsigned) value));
      BX_DMA_THIS s.command_reg = value;
      return;
      break;

    case 0x09: // DMA-1: request register
      BX_INFO(("DMA-1: write to request register (%02x)\n", (unsigned) value));
      // note: write to 0x0d clears this register
      if (value & 0x04) {
        // set request bit
        }
      else {
        Bit8u channel;

        // clear request bit
        channel = value & 0x03;
        BX_DMA_THIS s.status_reg &= ~(1 << (channel+4));
        BX_INFO(("dma-1: cleared request bit for channel %u\n", (unsigned) channel));
        }
      return;
      break;

    case 0x0a:
      set_mask_bit = value & 0x04;
      channel = value & 0x03;
      BX_DMA_THIS s.mask[channel] = (set_mask_bit > 0);
      if (bx_dbg.dma)
        BX_INFO(("DMA1: set_mask_bit=%u, channel=%u, mask now=%02xh\n",
          (unsigned) set_mask_bit, (unsigned) channel, (unsigned) BX_DMA_THIS s.mask[channel]));
      return;
      break;

    case 0x0b: /* dma-1 mode register */
      channel = value & 0x03;
      BX_DMA_THIS s.chan[channel].mode.mode_type = (value >> 6) & 0x03;
      BX_DMA_THIS s.chan[channel].mode.address_decrement = (value >> 5) & 0x01;
      BX_DMA_THIS s.chan[channel].mode.autoinit_enable = (value >> 4) & 0x01;
      BX_DMA_THIS s.chan[channel].mode.transfer_type = (value >> 2) & 0x03;
//BX_INFO(("DMA1: mode register[%u] = %02x\n",
//(unsigned) channel, (unsigned) value));
      if (bx_dbg.dma)
        BX_INFO(("DMA1: mode register[%u] = %02x\n",
          (unsigned) channel, (unsigned) value));
      return;
      break;

    case 0x0c: /* dma-1 clear byte flip/flop */
      if (bx_dbg.dma)
        BX_INFO(("DMA1: clear flip/flop\n"));
      BX_DMA_THIS s.flip_flop = 0;
      return;
      break;

    case 0x0d: // dma-1: master disable
      /* ??? */
      BX_INFO(("dma: master disable\n"));
      // writing any value to this port resets DMA controller 1
      // same action as a hardware reset
      // mask register is set (chan 0..3 disabled)
      // command, status, request, temporary, and byte flip-flop are all cleared
      BX_DMA_THIS s.mask[0] = 1;
      BX_DMA_THIS s.mask[1] = 1;
      BX_DMA_THIS s.mask[2] = 1;
      BX_DMA_THIS s.mask[3] = 1;
      BX_DMA_THIS s.command_reg = 0;
      BX_DMA_THIS s.status_reg = 0;
      BX_DMA_THIS s.request_reg = 0;
      BX_DMA_THIS s.temporary_reg = 0;
      BX_DMA_THIS s.flip_flop = 0;
      return;
      break;

    case 0x0e: // dma-1: clear mask register
      BX_INFO(("dma-1: clear mask register\n"));
      BX_DMA_THIS s.mask[0] = 0;
      BX_DMA_THIS s.mask[1] = 0;
      BX_DMA_THIS s.mask[2] = 0;
      BX_DMA_THIS s.mask[3] = 0;
      return;
      break;

    case 0x0f: // dma-1: write all mask bits
      BX_INFO(("dma-1: write all mask bits\n"));
      BX_DMA_THIS s.mask[0] = value & 0x01; value >>= 1;
      BX_DMA_THIS s.mask[1] = value & 0x01; value >>= 1;
      BX_DMA_THIS s.mask[2] = value & 0x01; value >>= 1;
      BX_DMA_THIS s.mask[3] = value & 0x01;
      return;
      break;

    case 0x81: /* dma page register, channel 2 */
    case 0x82: /* dma page register, channel 3 */
    case 0x83: /* dma page register, channel 1 */
    case 0x87: /* dma page register, channel 0 */
      /* address bits A16-A23 for DMA channel */
      channel = channelindex[address - 0x81];
      BX_DMA_THIS s.chan[channel].page_reg = value;
      if (bx_dbg.dma)
        BX_INFO(("DMA1: page register %d = %02x\n", channel, (unsigned) value));
      return;
      break;

    case 0x0084: // ???
      return;
      break;

    //case 0xd0: /* DMA-2 command register */
    //  if (value != 0x04)
    //    BX_INFO(("DMA2: write command register: value(%02xh)!=04h\n",
    //      (unsigned) value));
    //  return;
    //  break;

    case 0x00c0:
    case 0x00c2:
    case 0x00c4:
    case 0x00c6:
    case 0x00c8:
    case 0x00ca:
    case 0x00cc:
    case 0x00ce:
    case 0x00d0:
    case 0x00d2:
    case 0x00d4:
    case 0x00d6:
    case 0x00d8:
    case 0x00da:
    case 0x00dc:
    case 0x00de:
      BX_INFO(("DMA(ignored): write: %04xh = %04xh\n",
        (unsigned) address, (unsigned) value));
      return;
      break;


    default:
      BX_INFO(("DMA(ignored): write: %04xh = %02xh\n",
        (unsigned) address, (unsigned) value));
    }
}

  void
bx_dma_c::DRQ(unsigned channel, Boolean val)
{
  Bit32u dma_base, dma_roof;

#if BX_SUPPORT_SB16
  if ( (channel != 2) && (channel != (unsigned) BX_SB16_DMAL) )
    BX_PANIC(("bx_dma_c::DRQ(): channel %d != 2 or %d (SB16) (\n",
	     channel, BX_SB16_DMAL));
#else
  if ( channel != 2 )
    BX_PANIC(("bx_dma_c::DRQ(): channel %d != 2\n",
	     channel));
#endif
  if (!val) {
    //BX_INFO(("bx_dma_c::DRQ(): val == 0\n"));
    // clear bit in status reg
    // deassert HRQ if not pending DRQ's ?
    // etc.
    BX_DMA_THIS s.status_reg &= ~(1 << (channel+4));
    return;
    }

#if 0
  BX_INFO(("BX_DMA_THIS s.mask[2]: %02x\n", (unsigned) BX_DMA_THIS s.mask[2]));
  BX_INFO(("BX_DMA_THIS s.flip_flop: %u\n", (unsigned) BX_DMA_THIS s.flip_flop));
  BX_INFO(("BX_DMA_THIS s.status_reg: %02x\n", (unsigned) BX_DMA_THIS s.status_reg));
  BX_INFO(("mode_type: %02x\n", (unsigned) BX_DMA_THIS s.chan[channel].mode.mode_type));
  BX_INFO(("address_decrement: %02x\n", (unsigned) BX_DMA_THIS s.chan[channel].mode.address_decrement));
  BX_INFO(("autoinit_enable: %02x\n", (unsigned) BX_DMA_THIS s.chan[channel].mode.autoinit_enable));
  BX_INFO(("transfer_type: %02x\n", (unsigned) BX_DMA_THIS s.chan[channel].mode.transfer_type));
  BX_INFO((".base_address: %04x\n", (unsigned) BX_DMA_THIS s.chan[channel].base_address));
  BX_INFO((".current_address: %04x\n", (unsigned) BX_DMA_THIS s.chan[channel].current_address));
  BX_INFO((".base_count: %04x\n", (unsigned) BX_DMA_THIS s.chan[channel].base_count));
  BX_INFO((".current_count: %04x\n", (unsigned) BX_DMA_THIS s.chan[channel].current_count));
  BX_INFO((".page_reg: %02x\n", (unsigned) BX_DMA_THIS s.chan[channel].page_reg));
#endif

  BX_DMA_THIS s.status_reg |= (1 << (channel+4));

  //  if (BX_DMA_THIS s.mask[channel])
  //    BX_PANIC(("bx_dma_c::DRQ(): BX_DMA_THIS s.mask[] is set\n"));


  if ( (BX_DMA_THIS s.chan[channel].mode.mode_type != DMA_MODE_SINGLE) &&
       (BX_DMA_THIS s.chan[channel].mode.mode_type != DMA_MODE_DEMAND) )
    BX_PANIC(("bx_dma_c::DRQ: mode_type(%02x) not handled\n",
      (unsigned) BX_DMA_THIS s.chan[channel].mode.mode_type));
  if (BX_DMA_THIS s.chan[channel].mode.address_decrement != 0)
    BX_PANIC(("bx_dma_c::DRQ: address_decrement != 0\n"));
  //if (BX_DMA_THIS s.chan[channel].mode.autoinit_enable != 0)
  //  BX_PANIC(("bx_dma_c::DRQ: autoinit_enable != 0\n"));

  dma_base = (BX_DMA_THIS s.chan[channel].page_reg << 16) | BX_DMA_THIS s.chan[channel].base_address;
  dma_roof = dma_base + BX_DMA_THIS s.chan[channel].base_count;
  if ( (dma_base & 0xffff0000) != (dma_roof & 0xffff0000) ) {
BX_INFO(("dma_base = %08x\n", (unsigned) dma_base));
BX_INFO(("dma_base_count = %08x\n", (unsigned) BX_DMA_THIS s.chan[channel].base_count));
BX_INFO(("dma_roof = %08x\n", (unsigned) dma_roof));
    BX_PANIC(("dma: DMA request outside 64k boundary\n"));
    }


  //BX_INFO(("DRQ set up for single mode, increment, auto-init disabled, write\n"));
  // should check mask register VS DREQ's in status register here?
  // assert Hold ReQuest line to CPU
  bx_pc_system.set_HRQ(1);
}

  void
bx_dma_c::raise_HLDA(bx_pc_system_c *pc_sys)
{
  unsigned channel;
  Bit32u phy_addr;
  Boolean count_expired = 0;

  // find highest priority channel
  for (channel=0; channel<4; channel++) {
    if ( (BX_DMA_THIS s.status_reg & (1 << (channel+4))) &&
         (BX_DMA_THIS s.mask[channel]==0) ) {
      break;
      }
    }
  if (channel >= 4) {
	// don't panic, just wait till they're unmasked
    //    BX_PANIC(("hlda: no unmasked requests\n"));
    return;
    }

  //BX_INFO(("hlda: OK in response to DRQ(%u)\n", (unsigned) channel));
  phy_addr = (BX_DMA_THIS s.chan[channel].page_reg << 16) |
             BX_DMA_THIS s.chan[channel].current_address;

  bx_pc_system.set_DACK(channel, 1);
  // check for expiration of count, so we can signal TC and DACK(n)
  // at the same time.
  if (BX_DMA_THIS s.chan[channel].mode.address_decrement==0) {
    // address increment
    BX_DMA_THIS s.chan[channel].current_address++;
    BX_DMA_THIS s.chan[channel].current_count--;
    if (BX_DMA_THIS s.chan[channel].current_count == 0xffff) 
      if (BX_DMA_THIS s.chan[channel].mode.autoinit_enable == 0) {
	// count expired, done with transfer
	// assert TC, deassert HRQ & DACK(n) lines
	BX_DMA_THIS s.status_reg |= (1 << channel); // hold TC in status reg
	bx_pc_system.set_TC(1);
	count_expired = 1;
      } else {
	// count expired, but in autoinit mode
	// reload count and base address
	BX_DMA_THIS s.chan[channel].current_address = 
	  BX_DMA_THIS s.chan[channel].base_address;
	BX_DMA_THIS s.chan[channel].current_count =
	  BX_DMA_THIS s.chan[channel].base_count;
      }
    
    }
  else {
    // address decrement
    BX_PANIC(("hlda: decrement not implemented\n"));
    }

  if (BX_DMA_THIS s.chan[channel].mode.transfer_type == 1) { // write
    // xfer from I/O to Memory
    pc_sys->dma_write8(phy_addr, channel);
    }
  else if (BX_DMA_THIS s.chan[channel].mode.transfer_type == 2) { // read
    // xfer from Memory to I/O
    pc_sys->dma_read8(phy_addr, channel);
    }
  else {
    BX_PANIC(("hlda: transfer_type of %u not handled\n",
      (unsigned) BX_DMA_THIS s.chan[channel].mode.transfer_type));
    }

  if (count_expired) {
    bx_pc_system.set_TC(0);            // clear TC, adapter card already notified
    bx_pc_system.set_HRQ(0);           // clear HRQ to CPU
    bx_pc_system.set_DACK(channel, 0); // clear DACK to adapter card
    }
}
