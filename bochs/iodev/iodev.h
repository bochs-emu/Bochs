/////////////////////////////////////////////////////////////////////////
// $Id: iodev.h,v 1.18.4.18 2002-10-20 21:56:36 cbothamy Exp $
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



/* maximum number of emulated devices allowed.  floppy, mda, etc...
   you can increase this to anything below 256 since an 8-bit handle
   is used for each device */
#define BX_MAX_IO_DEVICES 20

/* number of IRQ lines supported.  In an ISA PC there are two
   PIC chips cascaded together.  each has 8 IRQ lines, so there
   should be 16 IRQ's total */
#define BX_MAX_IRQS 16
#define BX_NO_IRQ  -1


#if !BX_PLUGINS
class bx_biosdev_c;
class bx_floppy_ctrl_c;
class bx_cmos_c;
class bx_vga_c;
class bx_unmapped_c;
class bx_parallel_c;
class bx_serial_c;
class bx_hard_drive_c;
class bx_dma_c;
class bx_pic_c;
#endif

class bx_pit_c;
class bx_keyb_c;
class bx_sb16_c;
class bx_pci_c;
class bx_pci2isa_c;
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

//////////////////////////////////////////////////////////////////////
// bx_devmodel_c declaration
//////////////////////////////////////////////////////////////////////

// This class defines virtual methods that are common to all devices. 
// Child classes do not need to implement all of them, because in this 
// definition they are defined as empty, as opposed to being pure 
// virtual (= 0).
class bx_devmodel_c : public logfunctions {
  public:
  virtual ~bx_devmodel_c () {}
  virtual void init_mem(BX_MEM_C *) {}
  virtual void init(void) {}
  virtual void reset(unsigned type) {}
  virtual void device_load_state () {}
  virtual void device_save_state () {}
};

//////////////////////////////////////////////////////////////////////
// declare stubs for devices
//////////////////////////////////////////////////////////////////////

#define STUBFUNC(dev,method) \
   pluginlog->panic("%s called in %s stub. you must not have loaded the %s plugin", #dev, #method, #dev )

class bx_keyb_stub_c : public bx_devmodel_c {
  public:
  virtual ~bx_keyb_stub_c () {}
  // stubs for bx_keyb_c methods
  virtual void mouse_motion(int delta_x, int delta_y, unsigned button_state) {
    STUBFUNC(keyboard, mouse_motion);
  }
  virtual void gen_scancode(Bit32u   scancode) {
    STUBFUNC(keyboard, gen_scancode);
  }
  virtual void put_scancode( unsigned char *code, int count ) {
    STUBFUNC(keyboard, put_scancode);
  }
  virtual void paste_bytes(Bit8u *data, Bit32s length) {
    STUBFUNC(keyboard, paste_bytes);
  }
  virtual void paste_delay_changed () {
    STUBFUNC(keyboard, paste_delay_changed);
  }
  virtual void mouse_enabled_changed(bool enabled) {
    STUBFUNC(keyboard, mouse_enabled_changed);
  }
};

class bx_hard_drive_stub_c : public bx_devmodel_c {
  public:
  virtual void   close_harddrive(void) {
    STUBFUNC(HD, close_harddrive);
  }
  virtual void   init() {
    STUBFUNC(HD, init);
  }
  virtual void   reset(unsigned type) {
    STUBFUNC(HD, reset);
  }
  virtual Bit32u   get_device_handle(Bit8u channel, Bit8u device) {
    STUBFUNC(HD, get_device_handle); return 0;
  }
  virtual Bit32u   get_first_cd_handle(void) {
    STUBFUNC(HD, get_first_cd_handle); return 0;
  }
  virtual unsigned get_cd_media_status(Bit32u handle) {
    STUBFUNC(HD, get_cd_media_status); return 0;
  }
  virtual unsigned set_cd_media_status(Bit32u handle, unsigned status) {
    STUBFUNC(HD, set_cd_media_status); return 0;
  }
};

class bx_cmos_stub_c : public bx_devmodel_c {
  public:
  virtual Bit32u get_reg(unsigned reg) {
    STUBFUNC(cmos, get_reg); return 0;
  }
  virtual void set_reg(unsigned reg, Bit32u val) {
    STUBFUNC(cmos, set_reg);
  }
  virtual time_t get_timeval() {
    STUBFUNC(cmos, get_timeval); return 0;
  }
  virtual void checksum_cmos(void) {
    STUBFUNC(cmos, checksum);
  }
};

class bx_dma_stub_c : public bx_devmodel_c {
  public:
  virtual unsigned registerDMA8Channel(
    unsigned channel,
    void (* dmaRead)(Bit8u *data_byte),
    void (* dmaWrite)(Bit8u *data_byte),
    const char *name
    ) {
    STUBFUNC(dma, registerDMA8Channel); return 0;
  }
  virtual unsigned registerDMA16Channel(
    unsigned channel,
    void (* dmaRead)(Bit16u *data_word),
    void (* dmaWrite)(Bit16u *data_word),   
    const char *name
    ) {
    STUBFUNC(dma, registerDMA16Channel); return 0;
  }
  virtual unsigned unregisterDMAChannel(unsigned channel) {
    STUBFUNC(dma, unregisterDMAChannel); return 0;
  }
  virtual unsigned get_TC(void) {
    STUBFUNC(dma, get_TC); return 0;
  }
  virtual void set_DRQ(unsigned channel, Boolean val) {
    STUBFUNC(dma, set_DRQ);
  }
  virtual void raise_HLDA(void) {
    STUBFUNC(dma, raise_HLDA);
  }
};

class bx_pic_stub_c : public bx_devmodel_c {
  public:
  virtual void raise_irq(unsigned irq_no) {
    STUBFUNC(pic, raise_irq); 
  }
  virtual void lower_irq(unsigned irq_no) {
    STUBFUNC(pic, lower_irq); 
  }
  virtual Bit8u IAC(void) {
    STUBFUNC(pic, IAC); return 0;
  }
};

class bx_devices_c : public logfunctions {
public:
  bx_devices_c(void);
  ~bx_devices_c(void);
  // Register I/O addresses and IRQ lines. Initialize any internal
  // structures.  init() is called only once, even if the simulator
  // reboots or is restarted.
  void init(BX_MEM_C *);
  // Enter reset state in response to a reset condition.
  // The types of reset conditions are defined in bochs.h:
  // power-on, hardware, or software.
  void reset(unsigned type);
  BX_MEM_C *mem;  // address space associated with these devices
  Boolean register_io_read_handler(void *this_ptr, bx_read_handler_t f, Bit32u addr, const char *name );
  Boolean register_io_write_handler(void *this_ptr, bx_write_handler_t f, Bit32u addr, const char *name );
  Boolean register_default_io_read_handler(void *this_ptr, bx_read_handler_t f, const char *name );
  Boolean register_default_io_write_handler(void *this_ptr, bx_write_handler_t f, const char *name );
  Boolean register_irq(unsigned irq, const char *name);
  Boolean unregister_irq(unsigned irq, const char *name);
  void iodev_init(void);
  Bit32u inp(Bit16u addr, unsigned io_len);
  void   outp(Bit16u addr, Bit32u value, unsigned io_len);

  static void timer_handler(void *);
  void timer(void);

  bx_pci_c         *pci;
  bx_pci2isa_c     *pci2isa;
  bx_pit_c         *pit;
  bx_ioapic_c      *ioapic;
  bx_sb16_c        *sb16;
  bx_ne2k_c        *ne2k;
  bx_g2h_c         *g2h;
#if BX_IODEBUG_SUPPORT
  bx_iodebug_c	   *iodebug;
#endif
  bx_keyb_stub_c *pluginKeyboard;
  bx_hard_drive_stub_c *pluginHardDrive;
  bx_devmodel_c *pluginSerialDevice;
  bx_devmodel_c *pluginParallelDevice;
  bx_devmodel_c *pluginUnmapped;
  bx_devmodel_c *pluginBiosDevice;
  bx_cmos_stub_c *pluginCmosDevice;
  bx_dma_stub_c *pluginDmaDevice;
  bx_pic_stub_c *pluginPicDevice;
  bx_devmodel_c *pluginVgaDevice;
  bx_devmodel_c *pluginFloppyDevice;

  // stub classes that the pointers (above) can point to until a plugin is
  // loaded
  bx_cmos_stub_c stubCmos;
  bx_keyb_stub_c stubKeyboard;
  bx_hard_drive_stub_c stubHardDrive;
  bx_dma_stub_c  stubDma;
  bx_pic_stub_c  stubPic;

  // Some info to pass to devices which can handled bulk IO.  This allows
  // the interface to remain the same for IO devices which can't handle
  // bulk IO.  We should probably implement special INPBulk() and OUTBulk()
  // functions which stick these values in the bx_devices_c class, and
  // then call the normal functions rather than having gross globals
  // variables.
  Bit32u   bulkIOHostAddr;
  unsigned bulkIOQuantumsRequested;
  unsigned bulkIOQuantumsTransferred;

private:

  Bit8u                 read_handler_id[0x10000];  // 64K
  struct {
    bx_read_handler_t funct;
    void             *this_ptr;
    const char       *handler_name;  // name of device
    } io_read_handler[BX_MAX_IO_DEVICES + 1];
  unsigned              num_read_handles;

  Bit8u                 write_handler_id[0x10000]; // 64K
  struct {
    bx_write_handler_t funct;
    void              *this_ptr;
    const char        *handler_name;  // name of device
    } io_write_handler[BX_MAX_IO_DEVICES + 1];
  unsigned              num_write_handles;

  signed int            default_read_handler_id;
  signed int            default_write_handler_id;

  // more for informative purposes, the names of the devices which
  // are use each of the IRQ 0..15 lines are stored here
  const char *irq_handler_name[BX_MAX_IRQS];

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  BX_DEV_SMF Bit32u port92_read(Bit32u address, unsigned io_len);
  BX_DEV_SMF void   port92_write(Bit32u address, Bit32u value, unsigned io_len);

  int timer_handle;
  Boolean is_serial_enabled ();
  Boolean is_parallel_enabled ();
  };

#include "iodev/pci.h"
#include "iodev/pci2isa.h"

#include "iodev/cmos.h"
#include "iodev/unmapped.h"
#include "iodev/biosdev.h"
#if BX_SUPPORT_VGA
#  include "iodev/vga.h"
#else
#  include "iodev/hga.h"
#endif

#include "iodev/pic.h"
#include "iodev/floppy.h"
#include "iodev/dma.h"
#include "iodev/parallel.h"
#include "iodev/serial.h"
#include "iodev/keyboard.h"
#include "iodev/harddrv.h"

#if BX_SUPPORT_APIC
#  include "iodev/ioapic.h"
#endif

#if BX_IODEBUG_SUPPORT
#   include "iodev/iodebug.h"
#endif
#include "iodev/pit.h"
#include "iodev/pit_wrap.h"
#if BX_SUPPORT_SB16
#  include "iodev/sb16.h"
#endif
#include "iodev/eth.h"
#include "iodev/ne2k.h"
#include "iodev/guest2host.h"
#include "iodev/slowdown_timer.h"
