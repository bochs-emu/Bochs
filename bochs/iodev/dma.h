/////////////////////////////////////////////////////////////////////////
// $Id: dma.h,v 1.13 2003-03-02 23:59:10 cbothamy Exp $
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
#  define BX_DMA_THIS theDmaDevice->
#else
#  define BX_DMA_SMF
#  define BX_DMA_THIS this->
#endif



class bx_dma_c : public bx_dma_stub_c {
public:

  bx_dma_c();
  ~bx_dma_c(void);

  virtual void     init(void);
  virtual void     reset(unsigned type);
  virtual void     raise_HLDA(void);
  virtual void     set_DRQ(unsigned channel, bx_bool val);
  virtual unsigned get_TC(void);

  virtual unsigned registerDMA8Channel(unsigned channel,
    void (* dmaRead)(Bit8u *data_byte),
    void (* dmaWrite)(Bit8u *data_byte),
    const char *name);
  virtual unsigned registerDMA16Channel(unsigned channel,
    void (* dmaRead)(Bit16u *data_word),
    void (* dmaWrite)(Bit16u *data_word),
    const char *name);
  virtual unsigned unregisterDMAChannel(unsigned channel);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len) BX_CPP_AttrRegparmN(3);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len) BX_CPP_AttrRegparmN(3);
#if !BX_USE_DMA_SMF
  Bit32u   read( Bit32u   address, unsigned io_len) BX_CPP_AttrRegparmN(2);
  void     write(Bit32u   address, Bit32u   value, unsigned io_len) BX_CPP_AttrRegparmN(1);
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
      bx_bool used;
      } chan[4]; /* DMA channels 0..3 */
    } s[2];  // state information DMA-1 / DMA-2

  bx_bool HLDA;    // Hold Acknowlege
  bx_bool TC;      // Terminal Count

  struct {
    void (* dmaRead8)(Bit8u *data_byte);
    void (* dmaWrite8)(Bit8u *data_byte);
    void (* dmaRead16)(Bit16u *data_word);
    void (* dmaWrite16)(Bit16u *data_word);
    } h[4]; // DMA read and write handlers

  };

#endif  // #ifndef _PCDMA_H
