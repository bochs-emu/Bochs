/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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

#ifndef _PCDMA_H
#define _PCDMA_H

#if BX_USE_DMA_SMF
#  define BX_DMA_SMF  static
#  define BX_DMA_THIS theDmaDevice->
#else
#  define BX_DMA_SMF
#  define BX_DMA_THIS this->
#endif

class bx_dma_c : public bx_dma_stub_c {
public:
  bx_dma_c();
  virtual ~bx_dma_c();

  virtual void     init(void);
  virtual void     reset(unsigned type);
  virtual void     raise_HLDA(void);
  virtual void     set_DRQ(unsigned channel, bx_bool val);
  virtual unsigned get_TC(void);
  virtual void     register_state(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

  virtual unsigned registerDMA8Channel(unsigned channel,
    Bit16u (* dmaRead)(Bit8u *data_byte, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit8u *data_byte, Bit16u maxlen),
    const char *name);
  virtual unsigned registerDMA16Channel(unsigned channel,
    Bit16u (* dmaRead)(Bit16u *data_word, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit16u *data_word, Bit16u maxlen),
    const char *name);
  virtual unsigned unregisterDMAChannel(unsigned channel);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_DMA_SMF
  Bit32u   read (Bit32u address, unsigned io_len) BX_CPP_AttrRegparmN(2);
  void     write(Bit32u address, Bit32u   value, unsigned io_len) BX_CPP_AttrRegparmN(3);
#endif
  BX_DMA_SMF void control_HRQ(bx_bool ma_sl);
  BX_DMA_SMF void reset_controller(unsigned num);

  struct {
    bx_bool DRQ[4];  // DMA Request
    bx_bool DACK[4]; // DMA Acknowlege

    bx_bool mask[4];
    bx_bool flip_flop;
    Bit8u   status_reg;
    Bit8u   command_reg;
    bx_bool ctrl_disabled;
    struct {
      struct {
        Bit8u mode_type;
        bx_bool address_decrement;
        bx_bool autoinit_enable;
        Bit8u transfer_type;
      } mode;
      Bit16u  base_address;
      Bit16u  current_address;
      Bit16u  base_count;
      Bit16u  current_count;
      Bit8u   page_reg;
      bx_bool used;
    } chan[4]; /* DMA channels 0..3 */
  } s[2];  // state information DMA-1 / DMA-2

  bx_bool HLDA;    // Hold Acknowlege
  bx_bool TC;      // Terminal Count

  Bit8u   ext_page_reg[16]; // Extra page registers (unused)

  struct {
    Bit16u (* dmaRead8)(Bit8u *data_byte, Bit16u maxlen);
    Bit16u (* dmaWrite8)(Bit8u *data_byte, Bit16u maxlen);
    Bit16u (* dmaRead16)(Bit16u *data_word, Bit16u maxlen);
    Bit16u (* dmaWrite16)(Bit16u *data_word, Bit16u maxlen);
  } h[4]; // DMA read and write handlers
};

#endif  // #ifndef _PCDMA_H
