/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2018  The Bochs Project
//
//  I/O port handlers API Copyright (C) 2003 by Frank Cornelis
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

#ifndef IODEV_H
#define IODEV_H

#include "bochs.h"
#include "plugin.h"
#include "param_names.h"

/* number of IRQ lines supported.  In an ISA PC there are two
   PIC chips cascaded together.  each has 8 IRQ lines, so there
   should be 16 IRQ's total */
#define BX_MAX_IRQS 16

/* size of internal buffer for mouse devices */
#define BX_MOUSE_BUFF_SIZE 48

/* maximum size of the ISA DMA buffer */
#define BX_DMA_BUFFER_SIZE 512

#define BX_MAX_PCI_DEVICES 20

typedef Bit32u (*bx_read_handler_t)(void *, Bit32u, unsigned);
typedef void   (*bx_write_handler_t)(void *, Bit32u, Bit32u, unsigned);

typedef bx_bool (*bx_kbd_gen_scancode_t)(void *, Bit32u);
typedef void (*bx_mouse_enq_t)(void *, int, int, int, unsigned, bx_bool);
typedef void (*bx_mouse_enabled_changed_t)(void *, bx_bool);

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
class BOCHSAPI bx_devmodel_c : public logfunctions {
  public:
  virtual ~bx_devmodel_c() {}
  virtual void init(void) {}
  virtual void reset(unsigned type) {}
  virtual void register_state(void) {}
  virtual void after_restore_state(void) {}
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv) {}
#endif
};

// forward declarations
class bx_list_c;
class device_image_t;
class cdrom_base_c;

//////////////////////////////////////////////////////////////////////
// bx_pci_device_c declaration
//////////////////////////////////////////////////////////////////////

#if BX_SUPPORT_PCI

#define BX_DEBUG_PCI_READ(addr, value, io_len) \
  if (io_len == 1) \
    BX_DEBUG(("read  PCI register 0x%02X value 0x%02X (len=1)", address, value)); \
  else if (io_len == 2) \
    BX_DEBUG(("read  PCI register 0x%02X value 0x%04X (len=2)", address, value)); \
  else if (io_len == 4) \
    BX_DEBUG(("read  PCI register 0x%02X value 0x%08X (len=4)", address, value));

#define BX_DEBUG_PCI_WRITE(addr, value, io_len) \
  if (io_len == 1) \
    BX_DEBUG(("write PCI register 0x%02X value 0x%02X (len=1)", addr, value)); \
  else if (io_len == 2) \
    BX_DEBUG(("write PCI register 0x%02X value 0x%04X (len=2)", addr, value)); \
  else if (io_len == 4) \
    BX_DEBUG(("write PCI register 0x%02X value 0x%08X (len=4)", addr, value));

#define BX_PCI_BAR_TYPE_NONE 0
#define BX_PCI_BAR_TYPE_MEM  1
#define BX_PCI_BAR_TYPE_IO   2

typedef struct {
  Bit8u  type;
  Bit32u size;
  Bit32u addr;
  union {
    struct {
      memory_handler_t rh;
      memory_handler_t wh;
      const Bit8u *dummy;
    } mem;
    struct {
      bx_read_handler_t rh;
      bx_write_handler_t wh;
      const Bit8u *mask;
    } io;
  };
} bx_pci_bar_t;

class BOCHSAPI bx_pci_device_c : public bx_devmodel_c {
public:
  bx_pci_device_c(): pci_rom(NULL), pci_rom_size(0) {
    for (int i = 0; i < 6; i++) memset(&pci_bar[i], 0, sizeof(bx_pci_bar_t));
  }
  virtual ~bx_pci_device_c() {
    if (pci_rom != NULL) delete [] pci_rom;
  }

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  void pci_write_handler_common(Bit8u address, Bit32u value, unsigned io_len);
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len) {}
  virtual void pci_bar_change_notify(void) {}

  void init_pci_conf(Bit16u vid, Bit16u did, Bit8u rev, Bit32u classc,
                     Bit8u headt, Bit8u intpin);
  void init_bar_io(Bit8u num, Bit16u size, bx_read_handler_t rh,
                   bx_write_handler_t wh, const Bit8u *mask);
  void init_bar_mem(Bit8u num, Bit32u size, memory_handler_t rh, memory_handler_t wh);
  void register_pci_state(bx_list_c *list);
  void after_restore_pci_state(memory_handler_t mem_read_handler);
  void load_pci_rom(const char *path);

  void set_name(const char *name) {pci_name = name;}
  const char* get_name(void) {return pci_name;}

protected:
  const char *pci_name;
  Bit8u pci_conf[256];
  bx_pci_bar_t pci_bar[6];
  Bit8u  *pci_rom;
  Bit32u pci_rom_address;
  Bit32u pci_rom_size;
  memory_handler_t pci_rom_read_handler;
};
#endif

//////////////////////////////////////////////////////////////////////
// declare stubs for devices
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#define STUBFUNC(dev,method) \
   pluginlog->panic("%s called in %s stub. you must not have loaded the %s plugin", #dev, #method, #dev)
//////////////////////////////////////////////////////////////////////

class BOCHSAPI bx_keyb_stub_c : public bx_devmodel_c {
public:
  // stubs for bx_keyb_c methods
  virtual void gen_scancode(Bit32u key) {
    STUBFUNC(keyboard, gen_scancode);
  }
  virtual void paste_bytes(Bit8u *data, Bit32s length) {
    STUBFUNC(keyboard, paste_bytes);
  }
};

class BOCHSAPI bx_hard_drive_stub_c : public bx_devmodel_c {
public:
  virtual Bit32u get_first_cd_handle(void) {
    return BX_MAX_ATA_CHANNEL*2;
  }

  virtual bx_bool  get_cd_media_status(Bit32u handle) { return 0; }
  virtual bx_bool  set_cd_media_status(Bit32u handle, bx_bool status) {
    STUBFUNC(HD, set_cd_media_status); return 0;
  }

  virtual Bit32u virt_read_handler(Bit32u address, unsigned io_len) { return 0; }
  virtual void virt_write_handler(Bit32u address, Bit32u value, unsigned io_len) {}

  virtual bx_bool bmdma_read_sector(Bit8u channel, Bit8u *buffer, Bit32u *sector_size) {
    STUBFUNC(HD, bmdma_read_sector); return 0;
  }
  virtual bx_bool bmdma_write_sector(Bit8u channel, Bit8u *buffer) {
    STUBFUNC(HD, bmdma_write_sector); return 0;
  }
  virtual void bmdma_complete(Bit8u channel) {
    STUBFUNC(HD, bmdma_complete);
  }
};

class BOCHSAPI bx_floppy_stub_c : public bx_devmodel_c {
public:
  virtual unsigned set_media_status(unsigned drive, unsigned status) {
    STUBFUNC(floppy, set_media_status); return 0;
  }
};

class BOCHSAPI bx_cmos_stub_c : public bx_devmodel_c {
public:
  virtual Bit32u get_reg(Bit8u reg) {
    STUBFUNC(cmos, get_reg); return 0;
  }
  virtual void set_reg(Bit8u reg, Bit32u val) {
    STUBFUNC(cmos, set_reg);
  }
  virtual void checksum_cmos(void) {
    STUBFUNC(cmos, checksum);
  }
  virtual void enable_irq(bx_bool enabled) {
    STUBFUNC(cmos, enable_irq);
  }
};

class BOCHSAPI bx_pit_stub_c : public bx_devmodel_c {
public:
  virtual void enable_irq(bx_bool enabled) {
    STUBFUNC(pit, enable_irq);
  }
};

class BOCHSAPI bx_dma_stub_c : public bx_devmodel_c {
public:
  virtual unsigned registerDMA8Channel(
    unsigned channel,
    Bit16u (* dmaRead)(Bit8u *data_byte, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit8u *data_byte, Bit16u maxlen),
    const char *name)
  {
    STUBFUNC(dma, registerDMA8Channel); return 0;
  }
  virtual unsigned registerDMA16Channel(
    unsigned channel,
    Bit16u (* dmaRead)(Bit16u *data_word, Bit16u maxlen),
    Bit16u (* dmaWrite)(Bit16u *data_word, Bit16u maxlen),
    const char *name)
  {
    STUBFUNC(dma, registerDMA16Channel); return 0;
  }
  virtual unsigned unregisterDMAChannel(unsigned channel) {
    STUBFUNC(dma, unregisterDMAChannel); return 0;
  }
  virtual unsigned get_TC(void) {
    STUBFUNC(dma, get_TC); return 0;
  }
  virtual void set_DRQ(unsigned channel, bx_bool val) {
    STUBFUNC(dma, set_DRQ);
  }
  virtual void raise_HLDA(void) {
    STUBFUNC(dma, raise_HLDA);
  }
};

class BOCHSAPI bx_pic_stub_c : public bx_devmodel_c {
public:
  virtual void raise_irq(unsigned irq_no) {
    STUBFUNC(pic, raise_irq);
  }
  virtual void lower_irq(unsigned irq_no) {
    STUBFUNC(pic, lower_irq);
  }
  virtual void set_mode(bx_bool ma_sl, Bit8u mode) {
    STUBFUNC(pic, set_mode);
  }
  virtual Bit8u IAC(void) {
    STUBFUNC(pic, IAC); return 0;
  }
};

class BOCHSAPI bx_vga_stub_c
#if BX_SUPPORT_PCI
: public bx_pci_device_c
#else
: public bx_devmodel_c
#endif
{
public:
  virtual void vga_redraw_area(unsigned x0, unsigned y0, unsigned width,
                               unsigned height) {
    STUBFUNC(vga, vga_redraw_area);
  }
  virtual Bit8u mem_read(bx_phy_address addr) {
    STUBFUNC(vga, mem_read);  return 0;
  }
  virtual void mem_write(bx_phy_address addr, Bit8u value) {
    STUBFUNC(vga, mem_write);
  }
  virtual void get_text_snapshot(Bit8u **text_snapshot,
                                 unsigned *txHeight, unsigned *txWidth) {
    STUBFUNC(vga, get_text_snapshot);
  }
  virtual void set_override(bx_bool enabled, void *dev) {
    STUBFUNC(vga, set_override);
  }
  virtual void refresh_display(void *this_ptr, bx_bool redraw) {
    STUBFUNC(vga, refresh_display);
  }
};

class BOCHSAPI bx_speaker_stub_c : public bx_devmodel_c {
public:
  virtual void beep_on(float frequency) {
    bx_gui->beep_on(frequency);
  }
  virtual void beep_off() {
    bx_gui->beep_off();
  }
  virtual void set_line(bx_bool level) {}
};

#if BX_SUPPORT_PCI
class BOCHSAPI bx_pci2isa_stub_c : public bx_pci_device_c {
public:
  virtual void pci_set_irq (Bit8u devfunc, unsigned line, bx_bool level) {
    STUBFUNC(pci2isa, pci_set_irq);
  }
};

class BOCHSAPI bx_pci_ide_stub_c : public bx_pci_device_c {
public:
  virtual bx_bool bmdma_present(void) {
    return 0;
  }
  virtual void bmdma_start_transfer(Bit8u channel) {}
  virtual void bmdma_set_irq(Bit8u channel) {}
};

class BOCHSAPI bx_acpi_ctrl_stub_c : public bx_pci_device_c {
public:
  virtual void generate_smi(Bit8u value) {}
};
#endif

#if BX_SUPPORT_IODEBUG
class BOCHSAPI bx_iodebug_stub_c : public bx_devmodel_c {
public:
  virtual void mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data) {}
  virtual void mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data) {}
};
#endif

#if BX_SUPPORT_APIC
class BOCHSAPI bx_ioapic_stub_c : public bx_devmodel_c {
public:
  virtual void set_enabled(bx_bool enabled, Bit16u base_offset) {}
  virtual void receive_eoi(Bit8u vector) {}
  virtual void set_irq_level(Bit8u int_in, bx_bool level) {}
};
#endif

#if BX_SUPPORT_GAMEPORT
class BOCHSAPI bx_game_stub_c : public bx_devmodel_c {
public:
  virtual void set_enabled(bx_bool val) {
    STUBFUNC(gameport, set_enabled);
  }
};
#endif

class BOCHSAPI bx_hdimage_ctl_stub_c : public bx_devmodel_c {
public:
  virtual device_image_t* init_image(Bit8u image_mode, Bit64u disk_size, const char *journal) {
    STUBFUNC(hdimage_ctl, init_image); return NULL;
  }
  virtual cdrom_base_c* init_cdrom(const char *dev) {
    STUBFUNC(hdimage_ctl, init_cdrom); return NULL;
  }
};

class BOCHSAPI bx_devices_c : public logfunctions {
public:
  bx_devices_c();
 ~bx_devices_c();

  // Initialize the device stubs (in constructur and exit())
  void init_stubs(void);
  // Register I/O addresses and IRQ lines. Initialize any internal
  // structures.  init() is called only once, even if the simulator
  // reboots or is restarted.
  void init(BX_MEM_C *);
  // Enter reset state in response to a reset condition.
  // The types of reset conditions are defined in bochs.h:
  // power-on, hardware, or software.
  void reset(unsigned type);
  // Cleanup the devices when the simulation quits.
  void exit(void);
  void register_state(void);
  void after_restore_state(void);
  BX_MEM_C *mem;  // address space associated with these devices
  bx_bool register_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                   Bit32u addr, const char *name, Bit8u mask);
  bx_bool unregister_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                     Bit32u addr, Bit8u mask);
  bx_bool register_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                    Bit32u addr, const char *name, Bit8u mask);
  bx_bool unregister_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                      Bit32u addr, Bit8u mask);
  bx_bool register_io_read_handler_range(void *this_ptr, bx_read_handler_t f,
                                         Bit32u begin_addr, Bit32u end_addr,
                                         const char *name, Bit8u mask);
  bx_bool register_io_write_handler_range(void *this_ptr, bx_write_handler_t f,
                                          Bit32u begin_addr, Bit32u end_addr,
                                          const char *name, Bit8u mask);
  bx_bool unregister_io_read_handler_range(void *this_ptr, bx_read_handler_t f,
                                           Bit32u begin, Bit32u end, Bit8u mask);
  bx_bool unregister_io_write_handler_range(void *this_ptr, bx_write_handler_t f,
                                            Bit32u begin, Bit32u end, Bit8u mask);
  bx_bool register_default_io_read_handler(void *this_ptr, bx_read_handler_t f, const char *name, Bit8u mask);
  bx_bool register_default_io_write_handler(void *this_ptr, bx_write_handler_t f, const char *name, Bit8u mask);
  bx_bool register_irq(unsigned irq, const char *name);
  bx_bool unregister_irq(unsigned irq, const char *name);
  Bit32u inp(Bit16u addr, unsigned io_len) BX_CPP_AttrRegparmN(2);
  void   outp(Bit16u addr, Bit32u value, unsigned io_len) BX_CPP_AttrRegparmN(3);

  void register_removable_keyboard(void *dev, bx_kbd_gen_scancode_t kbd_gen_scancode);
  void unregister_removable_keyboard(void *dev);
  void register_default_mouse(void *dev, bx_mouse_enq_t mouse_enq, bx_mouse_enabled_changed_t mouse_enabled_changed);
  void register_removable_mouse(void *dev, bx_mouse_enq_t mouse_enq, bx_mouse_enabled_changed_t mouse_enabled_changed);
  void unregister_removable_mouse(void *dev);
  void gen_scancode(Bit32u key);
  void release_keys(void);
  void mouse_enabled_changed(bx_bool enabled);
  void mouse_motion(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);

#if BX_SUPPORT_PCI
  Bit32u pci_get_confAddr(void) {return pci.confAddr;}
  bx_bool register_pci_handlers(bx_pci_device_c *device, Bit8u *devfunc,
                                const char *name, const char *descr, Bit8u bus = 0);
  bx_bool pci_set_base_mem(void *this_ptr, memory_handler_t f1, memory_handler_t f2,
                           Bit32u *addr, Bit8u *pci_conf, unsigned size);
  bx_bool pci_set_base_io(void *this_ptr, bx_read_handler_t f1, bx_write_handler_t f2,
                          Bit32u *addr, Bit8u *pci_conf, unsigned size,
                          const Bit8u *iomask, const char *name);
#endif

  static void timer_handler(void *);
  void timer(void);

  bx_cmos_stub_c    *pluginCmosDevice;
  bx_dma_stub_c     *pluginDmaDevice;
  bx_floppy_stub_c  *pluginFloppyDevice;
  bx_hard_drive_stub_c *pluginHardDrive;
  bx_hdimage_ctl_stub_c *pluginHDImageCtl;
  bx_keyb_stub_c    *pluginKeyboard;
  bx_pic_stub_c     *pluginPicDevice;
  bx_pit_stub_c     *pluginPitDevice;
  bx_speaker_stub_c *pluginSpeaker;
  bx_vga_stub_c     *pluginVgaDevice;
#if BX_SUPPORT_IODEBUG
  bx_iodebug_stub_c *pluginIODebug;
#endif
#if BX_SUPPORT_APIC
  bx_ioapic_stub_c  *pluginIOAPIC;
#endif
#if BX_SUPPORT_GAMEPORT
  bx_game_stub_c  *pluginGameport;
#endif
#if BX_SUPPORT_PCI
  bx_pci2isa_stub_c *pluginPci2IsaBridge;
  bx_pci_ide_stub_c *pluginPciIdeController;
  bx_acpi_ctrl_stub_c *pluginACPIController;
#endif

  // stub classes that the pointers (above) can point to until a plugin is
  // loaded
  bx_cmos_stub_c stubCmos;
  bx_dma_stub_c  stubDma;
  bx_floppy_stub_c  stubFloppy;
  bx_hard_drive_stub_c stubHardDrive;
  bx_hdimage_ctl_stub_c stubHDImage;
  bx_keyb_stub_c stubKeyboard;
  bx_pic_stub_c  stubPic;
  bx_pit_stub_c  stubPit;
  bx_speaker_stub_c stubSpeaker;
  bx_vga_stub_c  stubVga;
#if BX_SUPPORT_IODEBUG
  bx_iodebug_stub_c stubIODebug;
#endif
#if BX_SUPPORT_APIC
  bx_ioapic_stub_c stubIOAPIC;
#endif
#if BX_SUPPORT_GAMEPORT
  bx_game_stub_c stubGameport;
#endif
#if BX_SUPPORT_PCI
  bx_pci2isa_stub_c stubPci2Isa;
  bx_pci_ide_stub_c stubPciIde;
  bx_acpi_ctrl_stub_c stubACPIController;
#endif

  // Some info to pass to devices which can handled bulk IO.  This allows
  // the interface to remain the same for IO devices which can't handle
  // bulk IO.  We should probably implement special INPBulk() and OUTBulk()
  // functions which stick these values in the bx_devices_c class, and
  // then call the normal functions rather than having gross globals
  // variables.
  Bit8u*   bulkIOHostAddr;
  unsigned bulkIOQuantumsRequested;
  unsigned bulkIOQuantumsTransferred;

private:

  struct io_handler_struct {
    struct io_handler_struct *next;
    struct io_handler_struct *prev;
    void *funct; // C++ type checking is great, but annoying
    void *this_ptr;
    char *handler_name;  // name of device
    int usage_count;
    Bit8u mask;          // io_len mask
  };
  struct io_handler_struct io_read_handlers;
  struct io_handler_struct io_write_handlers;
#define PORTS 0x10000
  struct io_handler_struct **read_port_to_handler;
  struct io_handler_struct **write_port_to_handler;

  // more for informative purposes, the names of the devices which
  // are use each of the IRQ 0..15 lines are stored here
  char *irq_handler_name[BX_MAX_IRQS];

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  BX_DEV_SMF Bit32u read(Bit32u address, unsigned io_len);
  BX_DEV_SMF void   write(Bit32u address, Bit32u value, unsigned io_len);

  static Bit32u default_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   default_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

  bx_bool mouse_captured; // host mouse capture enabled
  Bit8u mouse_type;
  struct {
    void *dev;
    bx_mouse_enq_t enq_event;
    bx_mouse_enabled_changed_t enabled_changed;
  } bx_mouse[2];
  struct {
    void *dev;
    bx_kbd_gen_scancode_t gen_scancode;
    bx_bool bxkey_state[BX_KEY_NBKEYS];
  } bx_keyboard;

  struct {
    bx_bool enabled;
#if BX_SUPPORT_PCI
    Bit8u handler_id[0x101];  // 256 PCI devices/functions + 1 AGP device
    struct {
      bx_pci_device_c *handler;
    } pci_handler[BX_MAX_PCI_DEVICES];
    unsigned num_pci_handlers;

    bx_bool slot_used[BX_N_PCI_SLOTS];

    Bit32u confAddr;
#endif
  } pci;

  int timer_handle;

  bx_bool network_enabled;
  bx_bool sound_enabled;
  bx_bool usb_enabled;

  bx_bool is_harddrv_enabled();
  bx_bool is_network_enabled();
  bx_bool is_sound_enabled();
  bx_bool is_usb_enabled();
};

// memory stub has an assumption that there are no memory accesses splitting 4K page
BX_CPP_INLINE void DEV_MEM_READ_PHYSICAL(bx_phy_address phy_addr, unsigned len, Bit8u *ptr)
{
  unsigned remainingInPage = 0x1000 - (phy_addr & 0xfff);
  if (len <= remainingInPage) {
    BX_MEM(0)->readPhysicalPage(NULL, phy_addr, len, ptr);
  }
  else {
    BX_MEM(0)->readPhysicalPage(NULL, phy_addr, remainingInPage, ptr);
    ptr += remainingInPage;
    phy_addr += remainingInPage;
    len -= remainingInPage;
    BX_MEM(0)->readPhysicalPage(NULL, phy_addr, len, ptr);
  }
}

BX_CPP_INLINE void DEV_MEM_READ_PHYSICAL_DMA(bx_phy_address phy_addr, unsigned len, Bit8u *ptr)
{
  while(len > 0) { 
    unsigned remainingInPage = 0x1000 - (phy_addr & 0xfff);
    if (len < remainingInPage) remainingInPage = len;
    BX_MEM(0)->dmaReadPhysicalPage(phy_addr, remainingInPage, ptr);
    ptr += remainingInPage;
    phy_addr += remainingInPage;
    len -= remainingInPage;
  }
}

// memory stub has an assumption that there are no memory accesses splitting 4K page
BX_CPP_INLINE void DEV_MEM_WRITE_PHYSICAL(bx_phy_address phy_addr, unsigned len, Bit8u *ptr)
{
  unsigned remainingInPage = 0x1000 - (phy_addr & 0xfff);
  if (len <= remainingInPage) {
    BX_MEM(0)->writePhysicalPage(NULL, phy_addr, len, ptr);
  }
  else {
    BX_MEM(0)->writePhysicalPage(NULL, phy_addr, remainingInPage, ptr);
    ptr += remainingInPage;
    phy_addr += remainingInPage;
    len -= remainingInPage;
    BX_MEM(0)->writePhysicalPage(NULL, phy_addr, len, ptr);
  }
}

BX_CPP_INLINE void DEV_MEM_WRITE_PHYSICAL_DMA(bx_phy_address phy_addr, unsigned len, Bit8u *ptr)
{
  while(len > 0) { 
    unsigned remainingInPage = 0x1000 - (phy_addr & 0xfff);
    if (len < remainingInPage) remainingInPage = len;
    BX_MEM(0)->dmaWritePhysicalPage(phy_addr, remainingInPage, ptr);
    ptr += remainingInPage;
    phy_addr += remainingInPage;
    len -= remainingInPage;
  }
}

BOCHSAPI extern bx_devices_c bx_devices;

#endif /* IODEV_H */
