/////////////////////////////////////////////////////////////////////////
// $Id: iodev.h,v 1.11 2001-12-18 13:12:45 vruppert Exp $
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



/* maximum number of emulated devices allowed.  floppy, mda, etc...
   you can increase this to anything below 256 since an 8-bit handle
   is used for each device */
#define BX_MAX_IO_DEVICES 20

/* number of IRQ lines supported.  In an ISA PC there are two
   PIC chips cascaded together.  each has 8 IRQ lines, so there
   should be 16 IRQ's total */
#define BX_MAX_IRQS 16
#define BX_NO_IRQ  -1


class bx_pit_c;
class bx_keyb_c;
class bx_dma_c;
class bx_floppy_ctrl_c;
class bx_cmos_c;
class bx_serial_c;
class bx_parallel_c;
class bx_unmapped_c;
class bx_vga_c;
class bx_pic_c;
class bx_hard_drive_c;
class bx_sb16_c;
class bx_pci_c;
class bx_ioapic_c;
class bx_ne2k_c;
class bx_g2h_c;
#if BX_IODEBUG_SUPPORT
class bx_iodebug_c;
#endif



typedef Bit32u (*bx_read_handler_t)(void *, Bit32u, unsigned);
typedef void   (*bx_write_handler_t)(void *, Bit32u, Bit32u, unsigned);


#if BX_USE_DEV_SMF
#  define BX_DEV_SMF  static
#  define BX_DEV_THIS bx_devices.
#else
#  define BX_DEV_SMF
#  define BX_DEV_THIS this->
#endif



class bx_devices_c : public logfunctions {
public:
  bx_devices_c(void);
  ~bx_devices_c(void);
  void init(BX_MEM_C *);
  BX_MEM_C *mem;  // address space associated with these devices
  void register_io_read_handler(void *this_ptr, bx_read_handler_t f, Bit32u addr, const char *name );
  void register_io_write_handler(void *this_ptr, bx_write_handler_t f, Bit32u addr, const char *name );
  void register_irq(unsigned irq, const char *name);
  void unregister_irq(unsigned irq, const char *name);
  void iodev_init(void);
  Bit32u inp(Bit16u addr, unsigned io_len);
  void   outp(Bit16u addr, Bit32u value, unsigned io_len);

  void dma_write8(unsigned channel, Bit8u *data);
  void dma_read8(unsigned channel, Bit8u *data);
  void dma_write16(unsigned channel, Bit16u *data);
  void dma_read16(unsigned channel, Bit16u *data);
  void drq(unsigned channel, Boolean val);
  void raise_hlda(void);
  static void timer_handler(void *);
  void timer(void);

  bx_ioapic_c      *ioapic;
  bx_pci_c         *pci;
  bx_pit_c         *pit;
  bx_keyb_c        *keyboard;
  bx_dma_c         *dma;
  bx_floppy_ctrl_c *floppy;
  bx_cmos_c        *cmos;
  bx_serial_c      *serial;
  bx_parallel_c    *parallel;
  bx_unmapped_c    *unmapped;
  bx_vga_c         *vga;
  bx_pic_c         *pic;
  bx_hard_drive_c  *hard_drive;
  bx_sb16_c        *sb16;
  bx_ne2k_c        *ne2k;
  bx_g2h_c         *g2h;
#if BX_IODEBUG_SUPPORT
  bx_iodebug_c	   *iodebug;
#endif

private:
  Bit8u                 read_handler_id[0x10000];  // 64K
  struct {
    bx_read_handler_t funct;
    void             *this_ptr;
    const char       *handler_name;  // name of device
    } io_read_handler[BX_MAX_IO_DEVICES];
  unsigned              num_read_handles;

  Bit8u                 write_handler_id[0x10000]; // 64K
  struct {
    bx_write_handler_t funct;
    void              *this_ptr;
    const char        *handler_name;  // name of device
    } io_write_handler[BX_MAX_IO_DEVICES];
  unsigned              num_write_handles;

  // more for informative purposes, the names of the devices which
  // are use each of the IRQ 0..15 lines are stored here
  const char *irq_handler_name[BX_MAX_IRQS];

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  BX_DEV_SMF Bit32u port92_read(Bit32u address, unsigned io_len);
  BX_DEV_SMF void   port92_write(Bit32u address, Bit32u value, unsigned io_len);

  int timer_handle;
  };



#include "iodev/pci.h"
#if BX_SUPPORT_VGA
#  include "iodev/vga.h"
#else
#  include "iodev/hga.h"
#endif
#if BX_SUPPORT_APIC
#  include "iodev/ioapic.h"
#endif
#include "iodev/cmos.h"
#include "iodev/dma.h"
#include "iodev/floppy.h"
#include "iodev/harddrv.h"
#if BX_IODEBUG_SUPPORT
#   include "iodev/iodebug.h"
#endif
#include "iodev/keyboard.h"
#include "iodev/parallel.h"
#include "iodev/pic.h"
#include "iodev/pit.h"
#include "iodev/pit_wrap.h"
#include "iodev/serial.h"
#if BX_SUPPORT_SB16
#  include "iodev/sb16.h"
#endif
#include "iodev/unmapped.h"
#include "iodev/eth.h"
#include "iodev/ne2k.h"
#include "iodev/guest2host.h"
#include "iodev/slowdown_timer.h"
