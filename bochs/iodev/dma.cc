/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
//
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "dma.h"

#define LOG_THIS theDmaDevice->

#define DMA_MODE_DEMAND  0
#define DMA_MODE_SINGLE  1
#define DMA_MODE_BLOCK   2
#define DMA_MODE_CASCADE 3

bx_dma_c *theDmaDevice = NULL;

int CDECL libdma_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theDmaDevice = new bx_dma_c ();
    bx_devices.pluginDmaDevice = theDmaDevice;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theDmaDevice, BX_PLUGIN_DMA);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libdma_LTX_plugin_fini(void)
{
  delete theDmaDevice;
}

bx_dma_c::bx_dma_c()
{
  put("DMA");
  memset(&s, 0, sizeof(s));
}

bx_dma_c::~bx_dma_c()
{
  SIM->get_bochs_root()->remove("dma");
  BX_DEBUG(("Exit"));
}

unsigned bx_dma_c::registerDMA8Channel(unsigned channel,
    Bit16u (* dmaRead)(Bit8u *data_byte, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit8u *data_byte, Bit16u maxlen),
    const char *name)
{
  if (channel > 3) {
    BX_PANIC(("registerDMA8Channel: invalid channel number(%u).", channel));
    return 0; // Fail
  }
  if (BX_DMA_THIS s[0].chan[channel].used) {
    BX_PANIC(("registerDMA8Channel: channel(%u) already in use.", channel));
    return 0; // Fail
  }
  BX_INFO(("channel %u used by %s", channel, name));
  BX_DMA_THIS h[channel].dmaRead8  = dmaRead;
  BX_DMA_THIS h[channel].dmaWrite8 = dmaWrite;
  BX_DMA_THIS s[0].chan[channel].used = 1;
  return 1; // OK
}

unsigned bx_dma_c::registerDMA16Channel(unsigned channel,
    Bit16u (* dmaRead)(Bit16u *data_word, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit16u *data_word, Bit16u maxlen),
    const char *name)
{
  if ((channel < 4) || (channel > 7)) {
    BX_PANIC(("registerDMA16Channel: invalid channel number(%u).", channel));
    return 0; // Fail
  }
  if (BX_DMA_THIS s[1].chan[channel & 0x03].used) {
    BX_PANIC(("registerDMA16Channel: channel(%u) already in use.", channel));
    return 0; // Fail
  }
  BX_INFO(("channel %u used by %s", channel, name));
  channel &= 0x03;
  BX_DMA_THIS h[channel].dmaRead16  = dmaRead;
  BX_DMA_THIS h[channel].dmaWrite16 = dmaWrite;
  BX_DMA_THIS s[1].chan[channel].used = 1;
  return 1; // OK
}

unsigned bx_dma_c::unregisterDMAChannel(unsigned channel)
{
  bx_bool ma_sl = (channel > 3);
  BX_DMA_THIS s[ma_sl].chan[channel & 0x03].used = 0;
  BX_INFO(("channel %u no longer used", channel));
  return 1;
}

unsigned bx_dma_c::get_TC(void)
{
  return BX_DMA_THIS TC;
}

void bx_dma_c::init(void)
{
  unsigned c, i, j;
  BX_DEBUG(("Init $Id$"));

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
    DEV_register_ioread_handler(this, read_handler, i, "DMA controller", 1);
    DEV_register_iowrite_handler(this, write_handler, i, "DMA controller", 3);
  }

  // 00080..008F
  for (i=0x0080; i<=0x008F; i++) {
    DEV_register_ioread_handler(this, read_handler, i, "DMA controller", 1);
    DEV_register_iowrite_handler(this, write_handler, i, "DMA controller", 3);
  }

  // 000C0..00DE
  for (i=0x00C0; i<=0x00DE; i+=2) {
    DEV_register_ioread_handler(this, read_handler, i, "DMA controller", 1);
    DEV_register_iowrite_handler(this, write_handler, i, "DMA controller", 3);
  }

  for (i=0; i<2; i++) {
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
  memset(&BX_DMA_THIS ext_page_reg[0], 0, 16);
  BX_DMA_THIS s[1].chan[0].used = 1; // cascade channel in use
  BX_INFO(("channel 4 used by cascade"));
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("dma", this);
#endif
}

void bx_dma_c::reset(unsigned type)
{
  reset_controller(0);
  reset_controller(1);
}

void bx_dma_c::reset_controller(unsigned num)
{
  BX_DMA_THIS s[num].mask[0] = 1;
  BX_DMA_THIS s[num].mask[1] = 1;
  BX_DMA_THIS s[num].mask[2] = 1;
  BX_DMA_THIS s[num].mask[3] = 1;
  BX_DMA_THIS s[num].ctrl_disabled = 0;
  BX_DMA_THIS s[num].command_reg = 0;
  BX_DMA_THIS s[num].status_reg = 0;
  BX_DMA_THIS s[num].flip_flop = 0;
}

void bx_dma_c::register_state(void)
{
  unsigned i, c;
  char name[6];
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "dma", "DMA State");
  for (i=0; i<2; i++) {
    sprintf(name, "%d", i);
    bx_list_c *ctrl = new bx_list_c(list, name);
    BXRS_PARAM_BOOL(ctrl, flip_flop, BX_DMA_THIS s[i].flip_flop);
    BXRS_HEX_PARAM_FIELD(ctrl, status_reg, BX_DMA_THIS s[i].status_reg);
    BXRS_HEX_PARAM_FIELD(ctrl, command_reg, BX_DMA_THIS s[i].command_reg);
    BXRS_PARAM_BOOL(ctrl, ctrl_disabled, BX_DMA_THIS s[i].ctrl_disabled);
    for (c=0; c<4; c++) {
      sprintf(name, "%d", c);
      bx_list_c *chan = new bx_list_c(ctrl, name);
      BXRS_PARAM_BOOL(chan, DRQ, BX_DMA_THIS s[i].DRQ[c]);
      BXRS_PARAM_BOOL(chan, DACK, BX_DMA_THIS s[i].DACK[c]);
      BXRS_PARAM_BOOL(chan, mask, BX_DMA_THIS s[i].mask[c]);
      BXRS_DEC_PARAM_FIELD(chan, mode_type, BX_DMA_THIS s[i].chan[c].mode.mode_type);
      BXRS_PARAM_BOOL(chan, address_decrement, BX_DMA_THIS s[i].chan[c].mode.address_decrement);
      BXRS_PARAM_BOOL(chan, autoinit_enable, BX_DMA_THIS s[i].chan[c].mode.autoinit_enable);
      BXRS_DEC_PARAM_FIELD(chan, transfer_type, BX_DMA_THIS s[i].chan[c].mode.transfer_type);
      BXRS_HEX_PARAM_FIELD(chan, base_address, BX_DMA_THIS s[i].chan[c].base_address);
      BXRS_HEX_PARAM_FIELD(chan, current_address, BX_DMA_THIS s[i].chan[c].current_address);
      BXRS_HEX_PARAM_FIELD(chan, base_count, BX_DMA_THIS s[i].chan[c].base_count);
      BXRS_HEX_PARAM_FIELD(chan, current_count, BX_DMA_THIS s[i].chan[c].current_count);
      BXRS_HEX_PARAM_FIELD(chan, page_reg, BX_DMA_THIS s[i].chan[c].page_reg);
    }
  }
  bx_list_c *extpg = new bx_list_c(list, "ext_page");
  for (i=0; i<16; i++) {
    sprintf(name, "0x%02x", 0x80+i);
    new bx_shadow_num_c(extpg, name, &BX_DMA_THIS ext_page_reg[i], BASE_HEX);
  }
}

// index to find channel from register number (only [0],[1],[2],[6] used)
Bit8u channelindex[7] = {2, 3, 1, 0, 0, 0, 0};

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions
Bit32u bx_dma_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_DMA_SMF
  bx_dma_c *class_ptr = (bx_dma_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

  /* 8237 DMA controller */
  Bit32u BX_CPP_AttrRegparmN(2)
bx_dma_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DMA_SMF

  Bit8u retval;
  Bit8u channel;

  BX_DEBUG(("read addr=%04x", (unsigned) address));

#if BX_DMA_FLOPPY_IO < 1
  /* if we're not supporting DMA/floppy IO just return a bogus value */
  return(0xff);
#endif

  bx_bool ma_sl = (address >= 0xc0);

  switch (address) {
    case 0x00: /* DMA-1 current address, channel 0 */
    case 0x02: /* DMA-1 current address, channel 1 */
    case 0x04: /* DMA-1 current address, channel 2 */
    case 0x06: /* DMA-1 current address, channel 3 */
    case 0xc0: /* DMA-2 current address, channel 0 */
    case 0xc4: /* DMA-2 current address, channel 1 */
    case 0xc8: /* DMA-2 current address, channel 2 */
    case 0xcc: /* DMA-2 current address, channel 3 */
      channel = (address >> (1 + ma_sl)) & 0x03;
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return (BX_DMA_THIS s[ma_sl].chan[channel].current_address & 0xff);
      } else {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return (BX_DMA_THIS s[ma_sl].chan[channel].current_address >> 8);
      }

    case 0x01: /* DMA-1 current count, channel 0 */
    case 0x03: /* DMA-1 current count, channel 1 */
    case 0x05: /* DMA-1 current count, channel 2 */
    case 0x07: /* DMA-1 current count, channel 3 */
    case 0xc2: /* DMA-2 current count, channel 0 */
    case 0xc6: /* DMA-2 current count, channel 1 */
    case 0xca: /* DMA-2 current count, channel 2 */
    case 0xce: /* DMA-2 current count, channel 3 */
      channel = (address >> (1 + ma_sl)) & 0x03;
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return (BX_DMA_THIS s[ma_sl].chan[channel].current_count & 0xff);
      } else {
        BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
        return (BX_DMA_THIS s[ma_sl].chan[channel].current_count >> 8);
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
      retval = BX_DMA_THIS s[ma_sl].status_reg;
      BX_DMA_THIS s[ma_sl].status_reg &= 0xf0;
      return retval;

    case 0x0d: // DMA-1: temporary register
    case 0xda: // DMA-2: temporary register
      // only used for memory-to-memory transfers
      // write to 0x0d / 0xda clears temporary register
      BX_ERROR(("DMA-%d: read of temporary register always returns 0", ma_sl+1));
      return 0;

    case 0x0081: // DMA-1 page register, channel 2
    case 0x0082: // DMA-1 page register, channel 3
    case 0x0083: // DMA-1 page register, channel 1
    case 0x0087: // DMA-1 page register, channel 0
      channel = channelindex[address - 0x81];
      return BX_DMA_THIS s[0].chan[channel].page_reg;

    case 0x0089: // DMA-2 page register, channel 2
    case 0x008a: // DMA-2 page register, channel 3
    case 0x008b: // DMA-2 page register, channel 1
    case 0x008f: // DMA-2 page register, channel 0
      channel = channelindex[address - 0x89];
      return BX_DMA_THIS s[1].chan[channel].page_reg;

    case 0x0080:
    case 0x0084:
    case 0x0085:
    case 0x0086:
    case 0x0088:
    case 0x008c:
    case 0x008d:
    case 0x008e:
      BX_DEBUG(("read: extra page register 0x%04x (unused)", (unsigned) address));
      return BX_DMA_THIS ext_page_reg[address & 0x0f];

    case 0x0f: // DMA-1: undocumented: read all mask bits
    case 0xde: // DMA-2: undocumented: read all mask bits
      retval = BX_DMA_THIS s[ma_sl].mask[0] |
               (BX_DMA_THIS s[ma_sl].mask[1] << 1) |
               (BX_DMA_THIS s[ma_sl].mask[2] << 2) |
               (BX_DMA_THIS s[ma_sl].mask[3] << 3);
      return (0xf0 | retval);

    default:
      BX_ERROR(("read: unsupported address=%04x", (unsigned) address));
      return 0;
  }
}


// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_dma_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_DMA_SMF
  bx_dma_c *class_ptr = (bx_dma_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}


  /* 8237 DMA controller */
  void BX_CPP_AttrRegparmN(3)
bx_dma_c::write(Bit32u   address, Bit32u   value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DMA_SMF
  Bit8u set_mask_bit;
  Bit8u channel;

  if (io_len > 1) {
    if ((io_len == 2) && (address == 0x0b)) {
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

  bx_bool ma_sl = (address >= 0xc0);

  switch (address) {
    case 0x00:
    case 0x02:
    case 0x04:
    case 0x06:
    case 0xc0:
    case 0xc4:
    case 0xc8:
    case 0xcc:
      channel = (address >> (1 + ma_sl)) & 0x03;
      BX_DEBUG(("  DMA-%d base and current address, channel %d", ma_sl+1, channel));
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_address = value;
        BX_DMA_THIS s[ma_sl].chan[channel].current_address = value;
      } else { /* 2nd byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_address |= (value << 8);
        BX_DMA_THIS s[ma_sl].chan[channel].current_address |= (value << 8);
        BX_DEBUG(("    base = %04x", BX_DMA_THIS s[ma_sl].chan[channel].base_address));
        BX_DEBUG(("    curr = %04x", BX_DMA_THIS s[ma_sl].chan[channel].current_address));
      }
      BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
      break;

    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:
    case 0xc2:
    case 0xc6:
    case 0xca:
    case 0xce:
      channel = (address >> (1 + ma_sl)) & 0x03;
      BX_DEBUG(("  DMA-%d base and current count, channel %d", ma_sl+1, channel));
      if (BX_DMA_THIS s[ma_sl].flip_flop==0) { /* 1st byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_count = value;
        BX_DMA_THIS s[ma_sl].chan[channel].current_count = value;
      } else { /* 2nd byte */
        BX_DMA_THIS s[ma_sl].chan[channel].base_count |= (value << 8);
        BX_DMA_THIS s[ma_sl].chan[channel].current_count |= (value << 8);
        BX_DEBUG(("    base = %04x", BX_DMA_THIS s[ma_sl].chan[channel].base_count));
        BX_DEBUG(("    curr = %04x", BX_DMA_THIS s[ma_sl].chan[channel].current_count));
      }
      BX_DMA_THIS s[ma_sl].flip_flop = !BX_DMA_THIS s[ma_sl].flip_flop;
      break;

    case 0x08: /* DMA-1: command register */
    case 0xd0: /* DMA-2: command register */
      if ((value & 0xfb) != 0x00)
        BX_ERROR(("write to command register: value 0x%02x not supported",
          (unsigned) value));
      BX_DMA_THIS s[ma_sl].command_reg = value;
      BX_DMA_THIS s[ma_sl].ctrl_disabled = (value >> 2) & 0x01;
      control_HRQ(ma_sl);
      break;

    case 0x09: // DMA-1: request register
    case 0xd2: // DMA-2: request register
      channel = value & 0x03;
      // note: write to 0x0d / 0xda clears this register
      if (value & 0x04) {
        // set request bit
        BX_DMA_THIS s[ma_sl].status_reg |= (1 << (channel+4));
        BX_DEBUG(("DMA-%d: set request bit for channel %u", ma_sl+1, channel));
      } else {
        // clear request bit
        BX_DMA_THIS s[ma_sl].status_reg &= ~(1 << (channel+4));
        BX_DEBUG(("DMA-%d: cleared request bit for channel %u", ma_sl+1, channel));
      }
      control_HRQ(ma_sl);
      break;

    case 0x0a:
    case 0xd4:
      set_mask_bit = value & 0x04;
      channel = value & 0x03;
      BX_DMA_THIS s[ma_sl].mask[channel] = (set_mask_bit > 0);
      BX_DEBUG(("DMA-%d: set_mask_bit=%u, channel=%u, mask now=%02xh", ma_sl+1,
          set_mask_bit, channel, BX_DMA_THIS s[ma_sl].mask[channel]));
      control_HRQ(ma_sl);
      break;

    case 0x0b: /* DMA-1 mode register */
    case 0xd6: /* DMA-2 mode register */
      channel = value & 0x03;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type = (value >> 6) & 0x03;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement = (value >> 5) & 0x01;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.autoinit_enable = (value >> 4) & 0x01;
      BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type = (value >> 2) & 0x03;
      BX_DEBUG(("DMA-%d: mode register[%u] = %02x", ma_sl+1, channel, (unsigned) value));
      break;

    case 0x0c: /* DMA-1 clear byte flip/flop */
    case 0xd8: /* DMA-2 clear byte flip/flop */
      BX_DEBUG(("DMA-%d: clear flip/flop", ma_sl+1));
      BX_DMA_THIS s[ma_sl].flip_flop = 0;
      break;

    case 0x0d: // DMA-1: master clear
    case 0xda: // DMA-2: master clear
      BX_DEBUG(("DMA-%d: master clear", ma_sl+1));
      // writing any value to this port resets DMA controller 1 / 2
      // same action as a hardware reset
      // mask register is set (chan 0..3 disabled)
      // command, status, request, temporary, and byte flip-flop are all cleared
      reset_controller(ma_sl);
      break;

    case 0x0e: // DMA-1: clear mask register
    case 0xdc: // DMA-2: clear mask register
      BX_DEBUG(("DMA-%d: clear mask register", ma_sl+1));
      BX_DMA_THIS s[ma_sl].mask[0] = 0;
      BX_DMA_THIS s[ma_sl].mask[1] = 0;
      BX_DMA_THIS s[ma_sl].mask[2] = 0;
      BX_DMA_THIS s[ma_sl].mask[3] = 0;
      control_HRQ(ma_sl);
      break;

    case 0x0f: // DMA-1: write all mask bits
    case 0xde: // DMA-2: write all mask bits
      BX_DEBUG(("DMA-%d: write all mask bits", ma_sl+1));
      BX_DMA_THIS s[ma_sl].mask[0] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[1] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[2] = value & 0x01; value >>= 1;
      BX_DMA_THIS s[ma_sl].mask[3] = value & 0x01;
      control_HRQ(ma_sl);
      break;

    case 0x81: /* DMA-1 page register, channel 2 */
    case 0x82: /* DMA-1 page register, channel 3 */
    case 0x83: /* DMA-1 page register, channel 1 */
    case 0x87: /* DMA-1 page register, channel 0 */
      /* address bits A16-A23 for DMA channel */
      channel = channelindex[address - 0x81];
      BX_DMA_THIS s[0].chan[channel].page_reg = value;
      BX_DEBUG(("DMA-1: page register %d = %02x", channel, (unsigned) value));
      break;

    case 0x89: /* DMA-2 page register, channel 2 */
    case 0x8a: /* DMA-2 page register, channel 3 */
    case 0x8b: /* DMA-2 page register, channel 1 */
    case 0x8f: /* DMA-2 page register, channel 0 */
      /* address bits A16-A23 for DMA channel */
      channel = channelindex[address - 0x89];
      BX_DMA_THIS s[1].chan[channel].page_reg = value;
      BX_DEBUG(("DMA-2: page register %d = %02x", channel + 4, (unsigned) value));
      break;

    case 0x0080:
    case 0x0084:
    case 0x0085:
    case 0x0086:
    case 0x0088:
    case 0x008c:
    case 0x008d:
    case 0x008e:
      BX_DEBUG(("write: extra page register 0x%04x (unused)", (unsigned) address));
      BX_DMA_THIS ext_page_reg[address & 0x0f] = value;
      break;

    default:
      BX_ERROR(("write ignored: %04xh = %02xh",
        (unsigned) address, (unsigned) value));
  }
}

void bx_dma_c::set_DRQ(unsigned channel, bx_bool val)
{
  Bit32u dma_base, dma_roof;
  bx_bool ma_sl;

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
    // clear bit in status reg
    BX_DMA_THIS s[ma_sl].status_reg &= ~(1 << (channel+4));

    control_HRQ(ma_sl);
    return;
  }

  BX_DMA_THIS s[ma_sl].status_reg |= (1 << (channel+4));

  if ((BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_SINGLE) &&
      (BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_DEMAND) &&
      (BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type != DMA_MODE_CASCADE))
  {
    BX_PANIC(("set_DRQ: mode_type(%02x) not handled",
      BX_DMA_THIS s[ma_sl].chan[channel].mode.mode_type));
  }

  dma_base = (BX_DMA_THIS s[ma_sl].chan[channel].page_reg << 16) |
             (BX_DMA_THIS s[ma_sl].chan[channel].base_address << ma_sl);
  if (BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement==0) {
    dma_roof = dma_base + (BX_DMA_THIS s[ma_sl].chan[channel].base_count << ma_sl);
  } else {
    dma_roof = dma_base - (BX_DMA_THIS s[ma_sl].chan[channel].base_count << ma_sl);
  }
  if ((dma_base & (0x7fff0000 << ma_sl)) != (dma_roof & (0x7fff0000 << ma_sl))) {
    BX_INFO(("dma_base = 0x%08x", dma_base));
    BX_INFO(("dma_base_count = 0x%08x", BX_DMA_THIS s[ma_sl].chan[channel].base_count));
    BX_INFO(("dma_roof = 0x%08x", dma_roof));
    BX_PANIC(("request outside %dk boundary", 64 << ma_sl));
  }

  control_HRQ(ma_sl);
}

void bx_dma_c::control_HRQ(bx_bool ma_sl)
{
  unsigned channel;

  // do nothing if controller is disabled
  if (BX_DMA_THIS s[ma_sl].ctrl_disabled)
    return;

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
    if ((BX_DMA_THIS s[ma_sl].status_reg & (1 << (channel+4))) &&
        (BX_DMA_THIS s[ma_sl].mask[channel]==0)) {
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

void bx_dma_c::raise_HLDA(void)
{
  unsigned channel;
  bx_phy_address phy_addr;
  bx_bool ma_sl = 0;
  Bit16u maxlen, len = 1;
  Bit8u buffer[BX_DMA_BUFFER_SIZE];

  BX_DMA_THIS HLDA = 1;
  // find highest priority channel
  for (channel=0; channel<4; channel++) {
    if ((BX_DMA_THIS s[1].status_reg & (1 << (channel+4))) &&
        (BX_DMA_THIS s[1].mask[channel]==0)) {
      ma_sl = 1;
      break;
    }
  }
  if (channel == 0) { // master cascade channel
    BX_DMA_THIS s[1].DACK[0] = 1;
    for (channel=0; channel<4; channel++) {
      if ((BX_DMA_THIS s[0].status_reg & (1 << (channel+4))) &&
          (BX_DMA_THIS s[0].mask[channel]==0)) {
        ma_sl = 0;
        break;
      }
    }
  }
  if (channel >= 4) {
    // wait till they're unmasked
    return;
  }

  phy_addr = (BX_DMA_THIS s[ma_sl].chan[channel].page_reg << 16) |
             (BX_DMA_THIS s[ma_sl].chan[channel].current_address << ma_sl);

  if (!BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement) {
    maxlen = (BX_DMA_THIS s[ma_sl].chan[channel].current_count + 1) << ma_sl;
    BX_DMA_THIS TC = (maxlen <= BX_DMA_BUFFER_SIZE);
    if (maxlen > BX_DMA_BUFFER_SIZE) {
      maxlen = BX_DMA_BUFFER_SIZE;
    }
  } else {
    BX_DMA_THIS TC = (BX_DMA_THIS s[ma_sl].chan[channel].current_count == 0);
    maxlen = 1 << ma_sl;
  }

  if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 1) { // write
    // DMA controlled xfer of bytes from I/O to Memory

    if (!ma_sl) {
      if (BX_DMA_THIS h[channel].dmaWrite8)
        len = BX_DMA_THIS h[channel].dmaWrite8(buffer, maxlen);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));

      DEV_MEM_WRITE_PHYSICAL_DMA(phy_addr, len, buffer);

      BX_DBG_DMA_REPORT(phy_addr, len, BX_WRITE, buffer[0]); // FIXME
    } else {
      if (BX_DMA_THIS h[channel].dmaWrite16)
        len = BX_DMA_THIS h[channel].dmaWrite16((Bit16u*)buffer, maxlen / 2);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));

      DEV_MEM_WRITE_PHYSICAL_DMA(phy_addr, len, buffer);

      BX_DBG_DMA_REPORT(phy_addr, len * 2, BX_WRITE, buffer[0] | (buffer[1] << 16)); // FIXME
    }
  } else if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 2) { // read
    // DMA controlled xfer of bytes from Memory to I/O

    if (!ma_sl) {
      DEV_MEM_READ_PHYSICAL_DMA(phy_addr, maxlen, buffer);

      if (BX_DMA_THIS h[channel].dmaRead8)
        len = BX_DMA_THIS h[channel].dmaRead8(buffer, maxlen);

      BX_DBG_DMA_REPORT(phy_addr, len, BX_READ, buffer[0]); // FIXME
    } else {
      DEV_MEM_READ_PHYSICAL_DMA(phy_addr, maxlen, buffer);

      if (BX_DMA_THIS h[channel].dmaRead16)
        len = BX_DMA_THIS h[channel].dmaRead16((Bit16u*)buffer, maxlen / 2);

      BX_DBG_DMA_REPORT(phy_addr, len * 2, BX_READ, buffer[0] | (buffer[1] << 16)); // FIXME
    }
  } else if (BX_DMA_THIS s[ma_sl].chan[channel].mode.transfer_type == 0) {
    // verify

    if (!ma_sl) {
      if (BX_DMA_THIS h[channel].dmaWrite8)
        len = BX_DMA_THIS h[channel].dmaWrite8(buffer, 1);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));
    } else {
      if (BX_DMA_THIS h[channel].dmaWrite16)
        len = BX_DMA_THIS h[channel].dmaWrite16((Bit16u*)buffer, 1);
      else
        BX_PANIC(("no dmaWrite handler for channel %u.", channel));
    }
  } else {
    BX_PANIC(("hlda: transfer_type 3 is undefined"));
  }

  BX_DMA_THIS s[ma_sl].DACK[channel] = 1;
  // check for expiration of count, so we can signal TC and DACK(n)
  // at the same time.
  if (!BX_DMA_THIS s[ma_sl].chan[channel].mode.address_decrement)
    BX_DMA_THIS s[ma_sl].chan[channel].current_address += len;
  else
    BX_DMA_THIS s[ma_sl].chan[channel].current_address--;
  BX_DMA_THIS s[ma_sl].chan[channel].current_count -= len;
  if (BX_DMA_THIS s[ma_sl].chan[channel].current_count == 0xffff) {
    // count expired, done with transfer
    // assert TC, deassert HRQ & DACK(n) lines
    BX_DMA_THIS s[ma_sl].status_reg |= (1 << channel); // hold TC in status reg
    if (BX_DMA_THIS s[ma_sl].chan[channel].mode.autoinit_enable == 0) {
      // set mask bit if not in autoinit mode
      BX_DMA_THIS s[ma_sl].mask[channel] = 1;
    } else {
      // count expired, but in autoinit mode
      // reload count and base address
      BX_DMA_THIS s[ma_sl].chan[channel].current_address =
        BX_DMA_THIS s[ma_sl].chan[channel].base_address;
      BX_DMA_THIS s[ma_sl].chan[channel].current_count =
        BX_DMA_THIS s[ma_sl].chan[channel].base_count;
    }
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

#if BX_DEBUGGER
void bx_dma_c::debug_dump(int argc, char **argv)
{
  int ch, i, j;

  dbg_printf("i8237A DMA controller\n\n");
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 4; j++) {
      if (BX_DMA_THIS s[i].chan[j].used) {
        ch = i * 4 + j;
        dbg_printf("DMA channel %d", ch);
        if (ch == 4) {
          dbg_printf(" (cascade)\n");
        } else if (BX_DMA_THIS s[i].DRQ[j] && !BX_DMA_THIS s[i].mask[j]) {
          dbg_printf(" (active)\n");
          dbg_printf("  address: base=0x%04x, current=0x%04x\n", BX_DMA_THIS s[i].chan[j].base_address,
                     BX_DMA_THIS s[i].chan[j].current_address);
          dbg_printf("  count: base=0x%04x, current=0x%04x\n", BX_DMA_THIS s[i].chan[j].base_count,
                     BX_DMA_THIS s[i].chan[j].current_count);
          dbg_printf("  page: 0x%02x\n", BX_DMA_THIS s[i].chan[j].page_reg);
          dbg_printf("  mask: %u\n", BX_DMA_THIS s[i].mask[j]);
          dbg_printf("  flip_flop: %u\n", BX_DMA_THIS s[i].flip_flop);
          dbg_printf("  status_reg: 0x%02x\n", BX_DMA_THIS s[i].status_reg);
          dbg_printf("  mode_type: %u\n", BX_DMA_THIS s[i].chan[j].mode.mode_type);
          dbg_printf("  address_decrement: %u\n", BX_DMA_THIS s[i].chan[j].mode.address_decrement);
          dbg_printf("  autoinit_enable: %u\n", BX_DMA_THIS s[i].chan[j].mode.autoinit_enable);
          dbg_printf("  transfer_type: %u\n", BX_DMA_THIS s[i].chan[j].mode.transfer_type);
        } else {
          dbg_printf(" (not active: DRQ=%u, mask=%u)\n", BX_DMA_THIS s[i].DRQ[j],
                     BX_DMA_THIS s[i].mask[j]);
        }
      }
    }
  }
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif
