/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2000  The plex86 developers team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __PLUGIN_H
#define __PLUGIN_H

extern logfunctions  *pluginlog;

#ifdef __cplusplus
extern "C" {
#endif

#define BX_PLUGIN_UNMAPPED "UNMAPPED"
#define BX_PLUGIN_BIOSDEV  "BIOSDEV"
#define BX_PLUGIN_CMOS     "CMOS RAM"
#define BX_PLUGIN_VGA      "VGA"
#define BX_PLUGIN_FLOPPY   "FLOPPY"
#define BX_PLUGIN_PARALLEL "PARALLEL"
#define BX_PLUGIN_SERIAL   "SERIAL"
#define BX_PLUGIN_KEYBOARD "KEYBOARD"
#define BX_PLUGIN_HARDDRV  "HARDDRIVE"
#define BX_PLUGIN_DMA      "DMA"
#define BX_PLUGIN_PIC      "PIC"

#define BX_REGISTER_DEVICE pluginRegisterDevice
#define BX_REGISTER_DEVICE_DEVMODEL pluginRegisterDeviceDevmodel

#if BX_PLUGINS

#define BX_INIT_DEVICES() {bx_devices.init(BX_MEM(0)); bx_init_plugins();}
#define BX_RESET_DEVICES(type) {bx_devices.reset(type); bx_reset_plugins(type);}

#define BX_REGISTER_IOREAD_HANDLER(a,b,c,d,e,f)  pluginRegisterIOReadHandler(b,c,d,e,f)
#define BX_REGISTER_IOWRITE_HANDLER(a,b,c,d,e,f) pluginRegisterIOWriteHandler(b,c,d,e,f)
#define BX_REGISTER_DEFAULT_IOREAD_HANDLER(a,b,c,d,e) pluginRegisterDefaultIOReadHandler(b,c,d,e)
#define BX_REGISTER_DEFAULT_IOWRITE_HANDLER(a,b,c,d,e) pluginRegisterDefaultIOWriteHandler(b,c,d,e)

#define BX_REGISTER_IRQ(a,b,c) pluginRegisterIRQ(b,c)
#define BX_UNREGISTER_IRQ(a,b) pluginUnregisterIRQ(b)

#define BX_GET_CMOS_REG(a,b) pluginGetCMOSReg(b)
#define BX_SET_CMOS_REG(a,b,c) pluginSetCMOSReg(b,c)
#define BX_CMOS_CHECKSUM(a) pluginCMOSChecksum()
#define BX_GET_CMOS_TIMEVAL(a) pluginGetCMOSTimeval()

#define BX_VGA_MEM_READ(addr) pluginVGAMemRead(addr)
#define BX_VGA_MEM_WRITE(addr, val) pluginVGAMemWrite(addr, val)
#define BX_VGA_REDRAW_AREA(left, top, right, bottom) pluginVGARedrawArea(left, top, right, bottom)
#define BX_VGA_GET_TEXT_SNAPSHOT(rawsnap, height, width) pluginVGAGetTextSnapshot(rawsnap, height, width)
#define BX_VGA_REFRESH() pluginVGARefresh ()
#define BX_VGA_SET_UPDATE_INTERVAL(val) pluginVGASetUpdateInterval(val)

#define BX_FLOPPY_GET_MEDIA_STATUS(drive) pluginFloppyGetMediaStatus(drive)
#define BX_FLOPPY_SET_MEDIA_STATUS(drive, status) pluginFloppySetMediaStatus(drive, status)

#define BX_FLOPPY_PRESENT() (pluginDevicePresent(BX_PLUGIN_FLOPPY))

#define BX_HD_READ_HANDLER(a, b, c) pluginHDReadHandler(NULL, b, c)
#define BX_HD_WRITE_HANDLER(a, b, c, d) pluginHDWriteHandler(NULL, b, c, d)
#define BX_HD_GET_FIRST_CD_HANDLE() pluginHDGetFirstCDHandle()
#define BX_HD_GET_DEVICE_HANDLE(a,b) pluginHDGetDeviceHandle(a,b)
#define BX_HD_GET_CD_MEDIA_STATUS(handle) pluginHDGetCDMediaStatus(handle)
#define BX_HD_SET_CD_MEDIA_STATUS(handle, status)  pluginHDSetCDMediaStatus(handle, status)
#define BX_HD_CLOSE_HARDDRIVE()  pluginHDCloseHarddrive()

#define BX_HARD_DRIVE_PRESENT() (pluginDevicePresent(BX_PLUGIN_HARDDRV))

#define BX_BULK_IO_QUANTUM_REQUESTED(a) (bx_devices.bulkIOQuantumsRequested)
#define BX_BULK_IO_QUANTUM_TRANSFERRED(a) (bx_devices.bulkIOQuantumsTransferred)
#define BX_BULK_IO_HOST_ADDR(a) (bx_devices.bulkIOHostAddr)

#define BX_REGISTER_DMA8_CHANNEL(channel, dmaRead, dmaWrite, name) \
  pluginRegisterDMA8Channel(channel, dmaRead, dmaWrite, name)
#define BX_REGISTER_DMA16_CHANNEL(channel, dmaRead, dmaWrite, name) \
  pluginRegisterDMA16Channel(channel, dmaRead, dmaWrite, name)
#define BX_UNREGISTER_DMA_CHANNEL(channel) \
  pluginUnregisterDMAChannel(channel)
#define BX_DMA_SET_DRQ(channel, val) pluginDMASetDRQ(channel, val)
#define BX_DMA_GET_TC()              pluginDMAGetTC()
#define BX_DMA_RAISE_HLDA()          pluginDMARaiseHLDA()

#define BX_PIC_LOWER_IRQ(b)  pluginLowerIRQ(b)
#define BX_PIC_RAISE_IRQ(b)  pluginRaiseIRQ(b)
#define BX_PIC_IAC()         pluginPicIAC()

#else

#define BX_INIT_DEVICES() {bx_devices.init(BX_MEM(0)); }
#define BX_RESET_DEVICES(type) {bx_devices.reset(type); }

#define BX_REGISTER_IOREAD_HANDLER(a,b,c,d,e,f) a devices->register_io_read_handler(b,c,d,e)
#define BX_REGISTER_IOWRITE_HANDLER(a,b,c,d,e,f) a devices->register_io_write_handler(b,c,d,e)
#define BX_REGISTER_DEFAULT_IOREAD_HANDLER(a,b,c,d,e) a devices->register_default_io_read_handler(b,c,d)
#define BX_REGISTER_DEFAULT_IOWRITE_HANDLER(a,b,c,d,e) a devices->register_default_io_write_handler(b,c,d)
#define BX_REGISTER_IRQ(a,b,c) a devices->register_irq(b,c)
#define BX_UNREGISTER_IRQ(a,b) a devices->unregister_irq(b)

#define BX_GET_CMOS_REG(a,b) (a devices->cmos->s.reg[b])
#define BX_SET_CMOS_REG(a,b,c) a devices->cmos->s.reg[b]=c
#define BX_CMOS_CHECKSUM(a) a devices->cmos->checksum_cmos()
#define BX_GET_CMOS_TIMEVAL(a) a devices->cmos->s.timeval

#define BX_VGA_MEM_READ(addr) (bx_devices.vga->mem_read(addr))
#define BX_VGA_MEM_WRITE(addr, val) bx_devices.vga->mem_write(addr, val)
#define BX_VGA_REDRAW_AREA(left, top, right, bottom) \
    (bx_vga.redraw_area(left, top, right, bottom))
#define BX_VGA_GET_TEXT_SNAPSHOT(rawsnap, height, width) \
    (bx_vga.get_text_snapshot(rawsnap, height, width))
#define BX_VGA_REFRESH() bx_vga.timer_handler (&bx_vga)
#define BX_VGA_SET_UPDATE_INTERVAL(val) bx_vga.set_update_interval(val)

#define BX_FLOPPY_GET_MEDIA_STATUS(drive) \
    (bx_devices.floppy->get_media_status(drive))
#define BX_FLOPPY_SET_MEDIA_STATUS(drive, status) \
    (bx_devices.floppy->set_media_status(drive, status))

#define BX_FLOPPY_PRESENT() (bx_devices.floppy)

#define BX_HD_READ_HANDLER(a, b, c) \
    (a devices->hard_drive->read_handler(a devices->hard_drive, b, c))
#define BX_HD_WRITE_HANDLER(a, b, c, d) \
    (a devices->hard_drive->write_handler(a devices->hard_drive, b, c, d))
#define BX_HD_GET_FIRST_CD_HANDLE() \
    (bx_devices.hard_drive->get_first_cd_handle())
#define BX_HD_GET_DEVICE_HANDLE(a,b) \
    (bx_devices.hard_drive->get_device_handle(a,b))
#define BX_HD_GET_CD_MEDIA_STATUS(handle) \
    (bx_devices.hard_drive->get_cd_media_status(handle))
#define BX_HD_SET_CD_MEDIA_STATUS(handle, status) \
    (bx_devices.hard_drive->set_cd_media_status(handle, status))
#define BX_HD_CLOSE_HARDDRIVE()  bx_devices.hard_drive->close_harddrive()

#define BX_HARD_DRIVE_PRESENT() (bx_devices.hard_drive)

#define BX_BULK_IO_QUANTUM_REQUESTED(a) (a devices->bulkIOQuantumsRequested)
#define BX_BULK_IO_QUANTUM_TRANSFERRED(a) (a devices->bulkIOQuantumsTransferred)
#define BX_BULK_IO_HOST_ADDR(a) (a devices->bulkIOHostAddr)

#define BX_REGISTER_DMA8_CHANNEL(channel, dmaRead, dmaWrite, name) \
  bx_dma.registerDMA8Channel(channel, dmaRead, dmaWrite, name)
#define BX_REGISTER_DMA16_CHANNEL(channel, dmaRead, dmaWrite, name) \
  bx_dma.registerDMA16Channel(channel, dmaRead, dmaWrite, name)
#define BX_UNREGISTER_DMA_CHANNEL(channel) \
  bx_dma.unregisterDMAChannel(channel)
#define BX_DMA_SET_DRQ(channel, val) bx_dma.set_DRQ(channel, val)
#define BX_DMA_GET_TC()             bx_dma.get_TC()
#define BX_DMA_RAISE_HLDA()         bx_dma.raise_HLDA()

#define BX_PIC_LOWER_IRQ(b) bx_devices.pic->lower_irq(b);
#define BX_PIC_RAISE_IRQ(b) bx_devices.pic->raise_irq(b);
#define BX_PIC_IAC()        bx_devices.pic->IAC()

#endif // #if BX_PLUGINS

#define BX_IOAPIC_PRESENT() (bx_devices.hard_drive)

// FIXME Do we really need pluginRegisterTimer ?
#define BX_REGISTER_TIMER(a,b,c,d,e,f) bx_pc_system.register_timer(a,b,c,d,e,f)




extern class bx_keyb_stub_c *pluginKeyboard;

#define BX_EVENT_MOUSE_MOTION(dx, dy, state) \
    (pluginKeyboard->mouse_motion(dx, dy, state))
#define BX_EVENT_GEN_SCANCODE(scancode) \
    (pluginKeyboard->gen_scancode(scancode))
#define BX_EVENT_PUT_SCANCODE(scancode, count) \
    (pluginKeyboard->put_scancode(scancode, count))
#define BX_KBD_PASTE_BYTES(bytes, count) \
    (pluginKeyboard->paste_bytes(bytes,count))
#define BX_KBD_PASTE_DELAY_CHANGED() \
    (pluginKeyboard->paste_delay_changed())

#include <dlfcn.h>

typedef Bit32u (*ioReadHandler_t)(void *, Bit32u, unsigned);
typedef void   (*ioWriteHandler_t)(void *, Bit32u, Bit32u, unsigned);

#define MAX_ARGC 10

typedef struct _plugin_t
{
    int  initialized;
    void *handle;
    int  argc;
    char *name, *args, *argv[MAX_ARGC];
    int  (*plugin_init)(struct _plugin_t *plugin, int argc, char *argv[]);
    void (*plugin_fini)(void);

    struct _plugin_t *next;
} plugin_t;

extern plugin_t *plugins;

typedef struct _device_t
{
    const char *name;
    void (*device_init_mem)(BX_MEM_C *);
    void (*device_init_dev)(bx_devices_c *);
    void (*device_reset)(unsigned);
    void (*device_load_state)();
    void (*device_save_state)();

    int use_devmodel_interface;  // BBD hack
    class bx_devmodel_c *devmodel;  // BBD hack

    struct _device_t *next;
} device_t;


extern device_t *devices;

void plugin_startup (void);
void plugin_load (char *name, char *args);
plugin_t *plugin_unload (plugin_t *plugin);
void plugin_init_all (void);
void plugin_fini_all (void);

/* === Device Stuff === */
typedef void (*deviceInitMem_t)(BX_MEM_C *);
typedef void (*deviceInitDev_t)(bx_devices_c *);
typedef void (*deviceReset_t)(unsigned);
typedef void (*deviceLoad_t)(void);
typedef void (*deviceSave_t)(void);

void pluginRegisterDevice(deviceInitMem_t init_mem, deviceInitDev_t init_dev,
                          deviceReset_t reset, deviceLoad_t load, 
                          deviceSave_t save, char *name);
void pluginRegisterDeviceDevmodel(bx_devmodel_c *dev, char *name);
Boolean pluginDevicePresent(char *name);

/* === IO port stuff === */
extern int (*pluginRegisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                                unsigned base, const char *name, unsigned len);
extern int (*pluginRegisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned base, const char *name, unsigned len);
extern int (*pluginRegisterDefaultIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                                const char *name, unsigned len);
extern int (*pluginRegisterDefaultIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                                 const char *name, unsigned len);

/* === IRQ stuff === */
extern void  (*pluginRegisterIRQ)(unsigned irq, const char *name);
extern void  (*pluginUnregisterIRQ)(unsigned irq, const char *name);
extern void  (*pluginRaiseIRQ)(unsigned irq);
extern void  (*pluginLowerIRQ)(unsigned irq);
extern Bit8u (*pluginPicIAC)(void);


/* === CMOS query/set stuff === */

/* These are function pointers.  At startup time, they are set to
 * builtin default handlers which will cause an abort if called.
 * A CMOS plugin should set these pointers to real handler functions.
 */
extern Bit32u (* pluginGetCMOSReg)(unsigned reg);
extern void   (* pluginSetCMOSReg)(unsigned reg, Bit32u val);
extern void   (* pluginCMOSChecksum)(void);
extern time_t (* pluginGetCMOSTimeval)(void);


/* === A20 enable line stuff === */
extern unsigned (*pluginGetA20E)(void);
extern void     (*pluginSetA20E)(unsigned val);


/* === DMA stuff === */
extern unsigned (* pluginRegisterDMA8Channel)(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte),
  const char* name
  );
extern unsigned (* pluginRegisterDMA16Channel)(
  unsigned channel,
  void (* dma_read)(Bit16u *data_byte),
  void (* dma_write)(Bit16u *data_byte),
  const char* name
  );
extern unsigned  (* pluginUnregisterDMAChannel)(unsigned channel);
extern void     (* pluginDMASetDRQ)(unsigned channel, unsigned val);
extern unsigned (* pluginDMAGetTC)(void);
extern unsigned (* pluginDMAGetTC)(void);
extern void     (* pluginDMARaiseHLDA)(void);

/* === Hard drive / floppy port sharing hack === */
extern Bit32u   (* pluginHDReadHandler)(void* ptr, Bit32u address, unsigned io_len);
extern void     (* pluginHDWriteHandler)(void* ptr, Bit32u address, Bit32u value, unsigned io_len);
extern Bit32u   (* pluginHDGetFirstCDHandle)(void);
extern Bit32u   (* pluginHDGetDeviceHandle)(Bit8u, Bit8u);
extern unsigned (* pluginHDGetCDMediaStatus)(Bit32u);
extern unsigned (* pluginHDSetCDMediaStatus)(Bit32u, unsigned);
extern void     (* pluginHDCloseHarddrive)(void);

/* === Floppy stuff ===*/
extern unsigned (* pluginFloppyGetMediaStatus)(unsigned drive);
extern unsigned (* pluginFloppySetMediaStatus)(unsigned drive, unsigned status);

/* === VGA stuff === */
extern void (* pluginVGARedrawArea)(unsigned x0, unsigned y0,
                 unsigned width, unsigned height);
extern Bit8u (* pluginVGAMemRead)(Bit32u addr);
extern void  (* pluginVGAMemWrite)(Bit32u addr, Bit8u value);
extern void  (* pluginVGAGetTextSnapshot)(Bit8u **text_snapshot, 
		          unsigned *txHeight, unsigned *txWidth);
extern void  (* pluginVGARefresh)(void);
extern void  (* pluginVGASetUpdateInterval)(unsigned);

/* === Timer stuff === */
extern int      (*pluginRegisterTimer)(void *this_ptr, void (*funct)(void *),
                             Bit32u useconds, Boolean continuous,
                             Boolean active, const char *name);

extern void     (*pluginActivateTimer)(unsigned id, Bit32u usec, Boolean continuous);
extern void     (*pluginDeactivateTimer)(unsigned id);

/* === HRQ stuff === */
extern void     (*pluginSetHRQ)(unsigned val);
extern void     (*pluginSetHRQHackCallback)( void (*callback)(void) );

/* === Reset stuff === */
extern void     (*pluginResetSignal)(unsigned sig);

void plugin_abort (void);

// called from bochs main (hack)
extern int bx_load_plugins ();
extern void bx_init_plugins (void);
extern void bx_reset_plugins (unsigned);

// every plugin must define these, within the extern"C" block, so that
// a non-mangled function symbol is available in the shared library.
void plugin_fini(void);
int plugin_init(plugin_t *plugin, int argc, char *argv[]);


#ifdef __cplusplus
};
#endif

#endif /* __PLUGIN_H */
