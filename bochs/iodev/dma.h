/////////////////////////////////////////////////////////////////////////
// $Id: dma.h,v 1.10 2002-08-28 19:39:00 vruppert Exp $
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


#ifndef _PCDMA_H
#define _PCDMA_H


#if BX_USE_DMA_SMF
#  define BX_DMA_SMF  static
#  define BX_DMA_THIS bx_dma.
#else
#  define BX_DMA_SMF
#  define BX_DMA_THIS this->
#endif



class bx_dma_c : public logfunctions {
public:

  bx_dma_c();
  ~bx_dma_c(void);

  BX_DMA_SMF void     init(bx_devices_c *);
  BX_DMA_SMF void     reset(unsigned type);
  BX_DMA_SMF void     raise_HLDA(void);
  BX_DMA_SMF void     set_DRQ(unsigned channel, Boolean val);
  BX_DMA_SMF unsigned get_TC(void);

  BX_DMA_SMF unsigned registerDMA8Channel(unsigned channel,
    void (* dmaRead)(Bit8u *data_byte),
    void (* dmaWrite)(Bit8u *data_byte),
    const char *name);
  BX_DMA_SMF unsigned registerDMA16Channel(unsigned channel,
    void (* dmaRead)(Bit16u *data_word),
    void (* dmaWrite)(Bit16u *data_word),
    const char *name);
  BX_DMA_SMF unsigned unregisterDMAChannel(unsigned channel);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_DMA_SMF
  Bit32u   read( Bit32u   address, unsigned io_len);
  void     write(Bit32u   address, Bit32u   value, unsigned io_len);
#endif
  BX_DMA_SMF void control_HRQ(Boolean ma_sl);
  BX_DMA_SMF void reset_controller(unsigned num);

  struct {
    Boolean DRQ[4];  // DMA Request
    Boolean DACK[4]; // DMA Acknowlege

    Boolean mask[4];
    Boolean flip_flop;
    Bit8u   status_reg;
    Bit8u   command_reg;
    Bit8u   request_reg;
    Bit8u   temporary_reg;
    struct {
      struct {
        Bit8u mode_type;
        Bit8u address_decrement;
        Bit8u autoinit_enable;
        Bit8u transfer_type;
        } mode;
      Bit16u  base_address;
      Bit16u  current_address;
      Bit16u  base_count;
      Bit16u  current_count;
      Bit8u   page_reg;
      Boolean used;
      } chan[4]; /* DMA channels 0..3 */
    } s[2];  // state information DMA-1 / DMA-2

  Boolean HLDA;    // Hold Acknowlege
  Boolean TC;      // Terminal Count

  struct {
    void (* dmaRead8)(Bit8u *data_byte);
    void (* dmaWrite8)(Bit8u *data_byte);
    void (* dmaRead16)(Bit16u *data_word);
    void (* dmaWrite16)(Bit16u *data_word);
    } h[4]; // DMA read and write handlers

  bx_devices_c *devices;
  };

extern bx_dma_c bx_dma;

#endif  // #ifndef _PCDMA_H
