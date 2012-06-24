/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003  Fen Systems Ltd.
//  http://www.fensystems.co.uk/
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

#ifndef BX_IODEV_PCIPNIC_H
#define BX_IODEV_PCIPNIC_H

#include "pnic_api.h"

#if BX_USE_PCIPNIC_SMF
#  define BX_PNIC_SMF  static
#  define BX_PNIC_THIS thePNICDevice->
#  define BX_PNIC_THIS_PTR thePNICDevice
#else
#  define BX_PNIC_SMF
#  define BX_PNIC_THIS this->
#  define BX_PNIC_THIS_PTR this
#endif

#define PNIC_DATA_SIZE	4096
#define PNIC_RECV_RINGS 4

typedef struct {

  Bit8u		macaddr[6];
  Bit8u		irqEnabled;

  Bit16u	rCmd;		// Command register
  Bit16u	rStatus;	// Status register
  Bit16u	rLength;	// Length register
  Bit8u		rData[PNIC_DATA_SIZE];  // Data register array
  Bit16u	rDataCursor;

  int		recvIndex;
  int		recvQueueLength;
  Bit8u		recvRing[PNIC_RECV_RINGS][PNIC_DATA_SIZE]; // Receive buffer
  Bit16u	recvRingLength[PNIC_RECV_RINGS];

  Bit8u devfunc;

  int statusbar_id;
} bx_pnic_t;


class bx_pcipnic_c : public bx_devmodel_c, bx_pci_device_stub_c {
public:
  bx_pcipnic_c();
  virtual ~bx_pcipnic_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

private:
  bx_pnic_t s;

  static void set_irq_level(bx_bool level);

  static void pnic_timer_handler(void *);
  void pnic_timer(void);

  BX_PNIC_SMF bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_PNIC_SMF bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCIPNIC_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  eth_pktmover_c *ethdev;
  static void exec_command(void);

  static Bit32u rx_status_handler(void *arg);
  BX_PNIC_SMF Bit32u rx_status(void);
  static void rx_handler(void *arg, const void *buf, unsigned len);
  BX_PNIC_SMF void rx_frame(const void *buf, unsigned io_len);
};

#endif
