/////////////////////////////////////////////////////////////////////////
// $Id: dma.cc,v 1.21 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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
#define LOG_THIS bx_dma.

#define DMA_MODE_DEMAND  0
#define DMA_MODE_SINGLE  1
#define DMA_MODE_BLOCK   2
#define DMA_MODE_CASCADE 3



bx_dma_c bx_dma;
#if BX_USE_DMA_SMF
#define this (&bx_dma)
#endif


bx_dma_c::bx_dma_c(void)
{
  put("DMA");
  settype(DMALOG);
}

bx_dma_c::~bx_dma_c(void)
{
	BX_DEBUG(("Exit."));
}

  unsigned
bx_dma_c::registerDMA8Channel(
    unsigned channel,
    void (* dmaRead)(Bit8u *data_byte),
    void (* dmaWrite)(Bit8u *data_byte),
    const char *name
    )
{
  if (channel > 3) {
    BX_PANIC(("registerDMA8Channel: invalid channel number(%u).", channel));
    return 0; // Fail.
    }
  if (bx_dma.s[0].chan[channel].used) {
    BX_PANIC(("registerDMA8Channel: channel(%u) already in use.", channel));
    return 0; // Fail.
    }
  BX_INFO(("channel %u used by %s", channel, name));
  bx_dma.h[channel].dmaRead8  = dmaRead;
  bx_dma.h[channel].dmaWrite8 = dmaWrite;
  bx_dma.s[0].chan[channel].used = 1;
  return 1; // OK.
}

  unsigned
bx_dma_c::registerDMA16Channel(
    unsigned channel,
    void (* dmaRead)(Bit16u *data_word),
    void (* dmaWrite)(Bit16u *data_word),
    const char *name
    )
{
  if ((channel < 4) || (channel > 7)) {
    BX_PANIC(("registerDMA16Channel: invalid channel number(%u).", channel));
    return 0; // Fail.
    }
  if (bx_dma.s[1].chan[channel & 0x03].used) {
    BX_PANIC(("registerDMA16Channel: channel(%u) already in use.", channel));
    return 0; // Fail.
    }
  BX_INFO(("channel %u used by %s", channel, name));
  channel &= 0x03;
  bx_dma.h[channel].dmaRead16  = dmaRead;
  bx_dma.h[channel].dmaWrite16 = dmaWrite;
  bx_dma.s[1].chan[channel].used = 1;
  return 1; // OK.
}

  unsigned
bx_dma_c::unregisterDMAChannel(unsigned channel)
{
  Boolean ma_sl = (channel > 3);
  bx_dma.s[ma_sl].chan[channel & 0x03].used = 0;
  BX_INFO(("channel %u no longer used", channel));
  return 1;
}

  unsigned
bx_dma_c::get_TC(void)
{
  return BX_DMA_THIS TC;
}


  void
bx_dma_c::init(bx_devices_c *d)
{
  unsigned c, i, j;
  BX_DEBUG(("Init $Id: dma.cc,v 1.21 2002-08-27 19:54:46 bdenney Exp $"));

  BX_DMA_THIS devices = d;

  /* 8237 DMA controller */

  for (i=0; i < 2; i++) {
    for (j=0; j < 4; j++) {
      BX_DMA_THIS s[i].DRQ[j] = 0;
      BX_DMA_THIS s[i].DACK[j] = 0;
      }
    }
  BX_DMA_THIS HLDA = 0;
  BX_DMA_THIS TC = 0;

  // 0000..000F
  for (i=0x0000; i<=0x000F; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }

  // 00081..008F
  for (i=0x0081; i<=0x008F; i++) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }

  // 000C0..00DE
  for (i=0x00C0; i<=0x00DE; i+=2) {
    BX_DMA_THIS devices->register_io_read_handler(this, read_handler,
                                        i, "DMA controller");
    BX_DMA_THIS devices->register_io_write_handler(this, write_handler,
                                        i, "DMA controller");
    }


  for (i=0; i<2; i++) {
    BX_DMA_THIS s[i].mask[0] = 1; // channel 0 masked
    BX_DMA_THIS s[i].mask[1] = 1; // channel 1 masked
    BX_DMA_THIS s[i].mask[2] = 1; // channel 2 masked
    BX_DMA_THIS s[i].mask[3] = 1; // channel 3 masked

    BX_DMA_THIS s[i].flip_flop = 0; /* cleared */
    BX_DMA_THIS s[i].status_reg = 0; // no requests, no terminal counts reached
    for (c=0; c<4; c++) {
      BX_DMA_THIS s[i].chan[c].mode.mode_type = 0;         // demand mode
      BX_DMA_THIS s[i].chan[c].mode.address_decrement = 0; // address increment
      BX_DMA_THIS s[i].chan[c].mode.autoinit_enable = 0;   // autoinit disable
      BX_DMA_THIS s[i].chan[c].mode.transfer_type = 0;     // verify
      BX_DMA_THIS s[i].chan[c].base_address = 0;
      BX_DMA_THIS s[i].chan[c].current_address = 0;
      BX_DMA_THIS s[i].chan[c].base_count = 0;
      BX_DMA_THIS s[i].chan[c].current_count = 0;
      BX_DMA_THIS s[i].chan[c].page_reg = 0;
      BX_DMA_THIS s[i].chan[c].used = 0;
      }
    }
  BX_DMA_THIS s[1].chan[0].used = 1; // cascade channel in use
  BX_INFO(("channel 4 used by cascade"));
}

  void
bx_dma_c::reset(unsigned type)
{
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
  Boolean ma_sl;

  if (io_len > 1) {
    BX_ERROR(("io read from address %08x, len=%u",
             (unsigned) address, (unsigned) io_len));
	return 0xff;
    }

  BX_DEBUG(("read addr=%04x", (unsigned) address));

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return a bogus value */
  return(0xff);
#endif

  switch (address) {
    case 0x00: /* DMA-1 current address, channel 0 */
    case 0x02: /* DMA-1 current address, channel 1 */
    case 0x04: /* DMA-1 current address, channel 2 */
    case 0x06: /* DMA-1 current address, channel 3 */
    case 0xc0: /* DMA-2 current address, channel 0 */
    case 0xc4: /* DMA-2 current address, channel 1 */
    case 0xc8: /* DMA-2 current address, channel 2 */
    case 0xcc: /* DMA-2 current address, channel 3 */
      ma_sl = (address >= 0xc0);
      channel = (address >> (1 + ma_sl)) & 0x03;
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return(BX_DMA_THIS s[ma_sl].chan[channel].current_address & 0xff);
        }
      else {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return(BX_DMA_THIS s[ma_sl].chan[channel].current_address >> 8);
        }

    case 0x01: /* DMA-1 current count, channel 0 */
    case 0x03: /* DMA-1 current count, channel 1 */
    case 0x05: /* DMA-1 current count, channel 2 */
    case 0x07: /* DMA-1 current count, channel 3 */
    case 0xc2: /* DMA-2 current count, channel 0 */
    case 0xc6: /* DMA-2 current count, channel 1 */
    case 0xca: /* DMA-2 current count, channel 2 */
    case 0xce: /* DMA-2 current count, channel 3 */
      ma_sl = (address >= 0xc2);
      channel = (address >> (1 + ma_sl)) & 0x03;
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return(BX_DMA_THIS s[ma_sl].chan[channel].current_count & 0xff);
        }
      else {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return(BX_DMA_THIS s[ma_sl].chan[channel].current_count >> 8);
        }

    case 0x08: // DMA-1 Status Register
    case 0xd0: // DMA-2 Status Register
      // bit 7: 1 = channel 3 request
      // bit 6: 1 = channel 2 request
      // bit 5: 1 = channel 1 request
      // bit 4: 1 = channel 0 request
      // bit 3: 1 = channel 3 has reached terminal count
      // bit 2: 1 = channel 2 has reached terminal count
      // bit 1: 1 = channel 1 has reached terminal count
      // bit 0: 1 = channel 0 has reached terminal count
      // reading this register clears lower 4 bits (hold flags)
      ma_sl = (address == 0xd0);
      retval = BX_DMA_THIS s[ma_sl].status_reg;
      BX_DMA_THIS s[ma_sl].status_reg &= 0xf0;
      return(retval);
      break;
    case 0x0d: // DMA-1: temporary register
    case 0xda: // DMA-2: temporary register
      ma_sl = (address == 0xda);
      BX_ERROR(("DMA-%d: read of temporary register", ma_sl+1));
      // Note: write to 0x0D clears temporary register
      return(0);
      break;

    case 0x0081: // DMA-1 page register, channel 2
    case 0x0082: // DMA-1 page register, channel 3
    case 0x0083: // DMA-1 page register, channel 1
    case 0x0087: // DMA-1 page register, channel 0
      channel = channelindex[address - 0x81];
      return( BX_DMA_THIS s[0].chan[channel].page_reg );

    case 0x0089: // DMA-2 page register, channel 2
    case 0x008a: // DMA-2 page register, channel 3
    case 0x008b: // DMA-2 page register, channel 1
    case 0x008f: // DMA-2 page register, channel 0
      channel = channelindex[address - 0x89];
      return( BX_DMA_THIS s[1].chan[channel].page_reg );

    case 0x0084:
    case 0x0085:
    case 0x0086:
    case 0x0088:
    case 0x008c:
    case 0x008d:
    case 0x008e:
      BX_DEBUG(("read: extra page register 0x%04x unsupported", (unsigned) address));
      return(0);

    default:
      BX_ERROR(("read: unsupported address=%04x", (unsigned) address));
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
  Boolean ma_sl;

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

    BX_ERROR(("io write to address %08x, len=%u",
             (unsigned) address, (unsigned) io_len));
    return;
    }

  BX_DEBUG(("write: address=%04x value=%02x",
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
    case 0xc0:
    case 0xc4:
    case 0xc8:
    case 0xcc:
      ma_sl = (address >= 0xc0);
      channel = (address >> (1 + ma_sl)) & 0x03;
      BX_DEBUG(("  DMA-%d base and current address, channel %d", ma_sl+1, channel));
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_address = value;
        BX_DMA_THIS s[ma_sl].chan[channel].current_address = value;
        }
      else { /* 2nd byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_address |= (value << 8);
        BX_DMA_THIS s[ma_sl].chan[channel].current_address |= (value << 8);
        BX_DEBUG(("    base = %04x",
            (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].base_address));
        BX_DEBUG(("    curr = %04x",
            (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].current_address));
        }
      BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
      return;
      break;

    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:
    case 0xc2:
    case 0xc6:
    case 0xca:
    case 0xce:
      ma_sl = (address >= 0xc2);
      channel = (address >> (1 + ma_sl)) & 0x03;
      BX_DEBUG(("  DMA-%d base and current count, channel %d", ma_sl+1, channel));
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_count = value;
        BX_DMA_THIS s[ma_sl].chan[channel].current_count = value;
        }
      else { /* 2nd byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_count |= (value << 8);
        BX_DMA_THIS s[ma_sl].chan[channel].current_count |= (value << 8);
        BX_DEBUG(("    base = %04x",
            (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].base_count));
        BX_DEBUG(("    curr = %04x",
            (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].current_count));
        }
      BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
      return;
      break;

    case 0x08: /* DMA-1: command register */
    case 0xd0: /* DMA-2: command register */
      ma_sl = (address == 0xd0);
      if (value != 0x04)
        BX_ERROR(("DMA: write to 0x%02x: value(%02xh) not 04h", address,
          (unsigned) value));
      BX_DMA_THIS s[ma_sl].command_reg = value;
      return;
      break;

    case 0x09: // DMA-1: request register
    case 0xd2: // DMA-2: request register
      ma_sl = (address == 0xd2);
      channel = value & 0x03;
      BX_ERROR(("DMA-%d: write to request register (%02x)", ma_sl+1, (unsigned) value));
      // note: write to 0x0d clears this register
      if (value & 0x04) {
        // set request bit
        BX_DMA_THIS s[ma_sl].status_reg |= (1 << (channel+4));
        BX_DEBUG(("DMA-%d: set request bit for channel %u", ma_sl+1, (unsigned) channel));
        }
      else {
        // clear request bit
        BX_DMA_THIS s[ma_sl].status_reg &= ~(1 << (channel+4));
        BX_DEBUG(("DMA-%d: cleared request bit for channel %u", ma_sl+1, (unsigned) channel));
        }
      control_HRQ(ma_sl);
      return;
      break;

    case 0x0a:
    case 0xd4:
      ma_sl = (address == 0xd4);
      set_mask_bit = value & 0x04;
      channel = value & 0x03;
      BX_DMA_THIS s[ma_sl].mask[channel] = (set_mask_bit > 0);
      BX_DEBUG(("DMA-%d: set_mask_bit=%u, channel=%u, mask now=%02xh", ma_sl+1,
          (unsigned) set_mask_bit, (unsigned) channel, (unsigned) BX_DMA_THIS s[ma_sl].mask[channel]));
      control_HRQ(ma_sl);
      return;
      break;

    case 0x0b: /* DMA-1 mode register */
    case 0xd6: /* DMA-2 mode register */
      ma_sl = (address == 0xd6);
      channel = value & 0x03;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type = (value >> 6) & 0x03;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement = (value >> 5) & 0x01;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.autoinit_enable = (value >> 4) & 0x01;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type = (value >> 2) & 0x03;
      BX_DEBUG(("DMA-%d: mode register[%u] = %02x", ma_sl+1,
          (unsigned) channel, (unsigned) value));
      return;
      break;

    case 0x0c: /* DMA-1 clear byte flip/flop */
    case 0xd8: /* DMA-2 clear byte flip/flop */
      ma_sl = (address == 0xd8);
      BX_DEBUG(("DMA-%d: clear flip/flop", ma_sl+1));
      BX_DMA_THIS s[ma_sl].flip_flop = 0;
      return;
      break;

    case 0x0d: // DMA-1: master disable
    case 0xda: // DMA-2: master disable
      ma_sl = (address == 0xda);
      /* ??? */
      BX_DEBUG(("DMA-%d: master disable", ma_sl+1));
      // writing any value to this port resets DMA controller 1 / 2
      // same action as a hardware reset
      // mask register is set (chan 0..3 disabled)
      // command, status, request, temporary, and byte flip-flop are all cleared
      BX_DMA_THIS s[ma_sl].mask[0] = 1;
      BX_DMA_THIS s[ma_sl].mask[1] = 1;
      BX_DMA_THIS s[ma_sl].mask[2] = 1;
      BX_DMA_THIS s[ma_sl].mask[3] = 1;
      BX_DMA_THIS s[ma_sl].command_reg = 0;
      BX_DMA_THIS s[ma_sl].status_reg = 0;
      BX_DMA_THIS s[ma_sl].request_reg = 0;
      BX_DMA_THIS s[ma_sl].temporary_reg = 0;
      BX_DMA_THIS s[ma_sl].flip_flop = 0;
      return;
      break;

    case 0x0e: // DMA-1: clear mask register
    case 0xdc: // DMA-2: clear mask register
      ma_sl = (address == 0xdc);
      BX_DEBUG(("DMA-%d: clear mask register", ma_sl+1));
      BX_DMA_THIS s[ma_sl].mask[0] = 0;
      BX_DMA_THIS s[ma_sl].mask[1] = 0;
      BX_DMA_THIS s[ma_sl].mask[2] = 0;
      BX_DMA_THIS s[ma_sl].mask[3] = 0;
      control_HRQ(ma_sl);
      return;
      break;

    case 0x0f: // DMA-1: write all mask bits
    case 0xde: // DMA-2: write all mask bits
      ma_sl = (address == 0xde);
      BX_DEBUG(("DMA-%d: write all mask bits", ma_sl+1));
      BX_DMA_THIS s[ma_sl].mask[0] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[1] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[2] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[3] = value & 0x01;
      control_HRQ(ma_sl);
      return;
      break;

    case 0x81: /* DMA-1 page register, channel 2 */
    case 0x82: /* DMA-1 page register, channel 3 */
    case 0x83: /* DMA-1 page register, channel 1 */
    case 0x87: /* DMA-1 page register, channel 0 */
      /* address bits A16-A23 for DMA channel */
      channel = channelindex[address - 0x81];
      BX_DMA_THIS s[0].chan[channel].page_reg = value;
      BX_DEBUG(("DMA-1: page register %d = %02x", channel, (unsigned) value));
      return;
      break;

    case 0x89: /* DMA-2 page register, channel 2 */
    case 0x8a: /* DMA-2 page register, channel 3 */
    case 0x8b: /* DMA-2 page register, channel 1 */
    case 0x8f: /* DMA-2 page register, channel 0 */
      /* address bits A16-A23 for DMA channel */
      channel = channelindex[address - 0x89];
      BX_DMA_THIS s[1].chan[channel].page_reg = value;
      BX_DEBUG(("DMA-2: page register %d = %02x", channel + 4, (unsigned) value));
      return;
      break;

    case 0x0084:
    case 0x0085:
    case 0x0086:
    case 0x0088:
    case 0x008c:
    case 0x008d:
    case 0x008e:
      BX_DEBUG(("write: extra page register 0x%04x unsupported", (unsigned) address));
      return;
      break;

    default:
      BX_ERROR(("write ignored: %04xh = %02xh",
        (unsigned) address, (unsigned) value));
    }
}

  void
bx_dma_c::set_DRQ(unsigned channel, Boolean val)
{
  Bit32u dma_base, dma_roof;
  Boolean ma_sl;

  if (channel > 7) {
    BX_PANIC(("set_DRQ() channel > 7"));
    return;
  }
  ma_sl = (channel > 3);
  BX_DMA_THIS s[ma_sl].DRQ[channel & 0x03] = val;
  if (!BX_DMA_THIS s[ma_sl].chan[channel & 0x03].used) {
    BX_PANIC(("set_DRQ(): channel %d not connected to device", channel));
    return;
  }
  channel &= 0x03;
  if (!val) {
    //BX_DEBUG(("bx_dma_c::DRQ(): val == 0"));
    // clear bit in status reg
    BX_DMA_THIS s[ma_sl].status_reg &= ~(1 << (channel+4));

    control_HRQ(ma_sl);
    return;
  }

#if 0
  BX_INFO(("mask[%d]: %02x", channel, (unsigned) BX_DMA_THIS s[0].mask[channel]));
  BX_INFO(("flip_flop: %u", (unsigned) BX_DMA_THIS s[0].flip_flop));
  BX_INFO(("status_reg: %02x", (unsigned) BX_DMA_THIS s[0].status_reg));
  BX_INFO(("mode_type: %02x", (unsigned) BX_DMA_THIS s[0].chan[channel].mode.mode_type));
  BX_INFO(("address_decrement: %02x", (unsigned) BX_DMA_THIS s[0].chan[channel].mode.address_decrement));
  BX_INFO(("autoinit_enable: %02x", (unsigned) BX_DMA_THIS s[0].chan[channel].mode.autoinit_enable));
  BX_INFO(("transfer_type: %02x", (unsigned) BX_DMA_THIS s[0].chan[channel].mode.transfer_type));
  BX_INFO(("base_address: %04x", (unsigned) BX_DMA_THIS s[0].chan[channel].base_address));
  BX_INFO(("current_address: %04x", (unsigned) BX_DMA_THIS s[0].chan[channel].current_address));
  BX_INFO(("base_count: %04x", (unsigned) BX_DMA_THIS s[0].chan[channel].base_count));
  BX_INFO(("current_count: %04x", (unsigned) BX_DMA_THIS s[0].chan[channel].current_count));
  BX_INFO(("page_reg: %02x", (unsigned) BX_DMA_THIS s[0].chan[channel].page_reg));
#endif

  BX_DMA_THIS s[ma_sl].status_reg |= (1 << (channel+4));

  if ( (BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_SINGLE) &&
       (BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_DEMAND) &&
       (BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_CASCADE) )
    BX_PANIC(("set_DRQ: mode_type(%02x) not handled",
      (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type));

  dma_base = (BX_DMA_THIS s[ma_sl].chan[channel].page_reg << 16) |
             (BX_DMA_THIS s[ma_sl].chan[channel].base_address << ma_sl);
  if (BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement==0) {
    dma_roof = dma_base + (BX_DMA_THIS s[ma_sl].chan[channel].base_count << ma_sl);
  } else {
    dma_roof = dma_base - (BX_DMA_THIS s[ma_sl].chan[channel].base_count << ma_sl);
  }
  if ( (dma_base & (0x7fff0000 << ma_sl)) != (dma_roof & (0x7fff0000 << ma_sl)) ) {
    BX_INFO(("dma_base = %08x", (unsigned) dma_base));
    BX_INFO(("dma_base_count = %08x", (unsigned) BX_DMA_THIS s[ma_sl].chan[channel].base_count));
    BX_INFO(("dma_roof = %08x", (unsigned) dma_roof));
    BX_PANIC(("request outside %dk boundary", 64 << ma_sl));
  }

  control_HRQ(ma_sl);
}

  void
bx_dma_c::control_HRQ(Boolean ma_sl)
{
  unsigned channel;

  // deassert HRQ if no DRQ is pending
  if ((BX_DMA_THIS s[ma_sl].status_reg & 0xf0) == 0) {
    if (ma_sl) {
      bx_pc_system.set_HRQ(0);
    } else {
      BX_DMA_THIS set_DRQ(4, 0);
    }
    return;
  }
  // find highest priority channel
  for (channel=0; channel<4; channel++) {
    if ( (BX_DMA_THIS s[ma_sl].status_reg & (1 << (channel+4))) &&
         (BX_DMA_THIS s[ma_sl].mask[channel]==0) ) {
      if (ma_sl) {
        // assert Hold ReQuest line to CPU
        bx_pc_system.set_HRQ(1);
      } else {
        // send DRQ to cascade channel of the master
        BX_DMA_THIS set_DRQ(4, 1);
      }
      break;
    }
  }
}

  void
bx_dma_c::raise_HLDA(void)
{
  unsigned channel;
  Bit32u phy_addr;
  Boolean count_expired = 0;
  Boolean ma_sl = 0;

  BX_DMA_THIS HLDA = 1;
  // find highest priority channel
  for (channel=0; channel<4; channel++) {
    if ( (BX_DMA_THIS s[1].status_reg & (1 << (channel+4))) &&
         (BX_DMA_THIS s[1].mask[channel]==0) ) {
      ma_sl = 1;
      break;
      }
    }
  if (channel == 0) { // master cascade channel
    BX_DMA_THIS s[1].DACK[0] = 1;
    for (channel=0; channel<4; channel++) {
      if ( (BX_DMA_THIS s[0].status_reg & (1 << (channel+4))) &&
           (BX_DMA_THIS s[0].mask[channel]==0) ) {
        ma_sl = 0;
        break;
        }
      }
    }
  if (channel >= 4) {
    // wait till they're unmasked
    return;
    }

  //BX_DEBUG(("hlda: OK in response to DRQ(%u)", (unsigned) channel));
  phy_addr = (BX_DMA_THIS s[ma_sl].chan[channel].page_reg << 16) |
             (BX_DMA_THIS s[ma_sl].chan[channel].current_address << ma_sl);

  BX_DMA_THIS s[ma_sl].DACK[channel] = 1;
  // check for expiration of count, so we can signal TC and DACK(n)
  // at the same time.
  if (BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement==0)
    BX_DMA_THIS s[ma_sl].chan[channel].current_address++;
  else
    BX_DMA_THIS s[ma_sl].chan[channel].current_address--;
  BX_DMA_THIS s[ma_sl].chan[channel].current_count--;
  if (BX_DMA_THIS s[ma_sl].chan[channel].current_count == 0xffff) {
    // count expired, done with transfer
    // assert TC, deassert HRQ & DACK(n) lines
    BX_DMA_THIS s[ma_sl].status_reg |= (1 << channel); // hold TC in status reg
    BX_DMA_THIS TC = 1;
    count_expired = 1;
    if (BX_DMA_THIS s[ma_sl].chan[channel].mode.autoinit_enable == 0) {
      // set mask bit if not in autoinit mode
      BX_DMA_THIS s[ma_sl].mask[channel] = 1;
      }
    else {
      // count expired, but in autoinit mode
      // reload count and base address
      BX_DMA_THIS s[ma_sl].chan[channel].current_address =
        BX_DMA_THIS s[ma_sl].chan[channel].base_address;
      BX_DMA_THIS s[ma_sl].chan[channel].current_count =
        BX_DMA_THIS s[ma_sl].chan[channel].base_count;
      }
    }

  Bit8u data_byte;
  Bit16u data_word;

  if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 1) { // write
    // DMA controlled xfer of byte from I/O to Memory

    if (!ma_sl) {
      if (BX_DMA_THIS h[channel].dmaWrite8)
        BX_DMA_THIS h[channel].dmaWrite8(&data_byte);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));

      BX_MEM_WRITE_PHYSICAL(phy_addr, 1, &data_byte);

      BX_DBG_DMA_REPORT(phy_addr, 1, BX_WRITE, data_byte);
      }
    else {
      if (BX_DMA_THIS h[channel].dmaWrite16)
        BX_DMA_THIS h[channel].dmaWrite16(&data_word);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));

      BX_MEM_WRITE_PHYSICAL(phy_addr, 2, &data_word);

      BX_DBG_DMA_REPORT(phy_addr, 2, BX_WRITE, data_word);
      }
    }
  else if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 2) { // read
    // DMA controlled xfer of byte from Memory to I/O

    if (!ma_sl) {
      BX_MEM_READ_PHYSICAL(phy_addr, 1, &data_byte);

      if (BX_DMA_THIS h[channel].dmaRead8)
        BX_DMA_THIS h[channel].dmaRead8(&data_byte);

      BX_DBG_DMA_REPORT(phy_addr, 1, BX_READ, data_byte);
      }
    else {
      BX_MEM_READ_PHYSICAL(phy_addr, 2, &data_word);

      if (BX_DMA_THIS h[channel].dmaRead16)
        BX_DMA_THIS h[channel].dmaRead16(&data_word);

      BX_DBG_DMA_REPORT(phy_addr, 2, BX_READ, data_word);
      }
    }
  else if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 0) {
    // verify

    if (!ma_sl) {
      if (BX_DMA_THIS h[channel].dmaWrite8)
        BX_DMA_THIS h[channel].dmaWrite8(&data_byte);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));
      }
    else {
      if (BX_DMA_THIS h[channel].dmaWrite16)
        BX_DMA_THIS h[channel].dmaWrite16(&data_word);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));
      }
    }
  else {
    BX_PANIC(("hlda: transfer_type 3 is undefined"));
    }

  if (count_expired) {
    BX_DMA_THIS TC = 0;            // clear TC, adapter card already notified
    BX_DMA_THIS HLDA = 0;
    bx_pc_system.set_HRQ(0);           // clear HRQ to CPU
    BX_DMA_THIS s[ma_sl].DACK[channel] = 0; // clear DACK to adapter card
    if (!ma_sl) {
      BX_DMA_THIS set_DRQ(4, 0); // clear DRQ to cascade
      BX_DMA_THIS s[1].DACK[0] = 0; // clear DACK to cascade
      }
    }
}
