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

#include "extplugin.h"

class bx_devices_c;
extern logfunctions  *pluginlog;

#ifdef __cplusplus
extern "C" {
#endif

#define BX_PLUGIN_UNMAPPED "unmapped"
#define BX_PLUGIN_BIOSDEV  "biosdev"
#define BX_PLUGIN_CMOS     "cmos"
#define BX_PLUGIN_VGA      "vga"
#define BX_PLUGIN_FLOPPY   "floppy"
#define BX_PLUGIN_PARALLEL "parallel"
#define BX_PLUGIN_SERIAL   "serial"
#define BX_PLUGIN_KEYBOARD "keyboard"
#define BX_PLUGIN_HARDDRV  "harddrv"
#define BX_PLUGIN_DMA      "dma"
#define BX_PLUGIN_PIC      "pic"

#define BX_REGISTER_DEVICE pluginRegisterDevice
#define BX_REGISTER_DEVICE_DEVMODEL(a,b,c,d) pluginRegisterDeviceDevmodel(a,b,c,d)

#if BX_PLUGINS

#define BX_INIT_DEVICES() {bx_devices.init(BX_MEM(0)); }
#define BX_RESET_DEVICES(type) {bx_devices.reset(type); }
#define BX_LOAD_PLUGIN(name,type) {bx_load_plugin(#name,type);}

#define BX_REGISTER_IOREAD_HANDLER(b,c,d,e,f)  pluginRegisterIOReadHandler(b,c,d,e,f)
#define BX_REGISTER_IOWRITE_HANDLER(b,c,d,e,f) pluginRegisterIOWriteHandler(b,c,d,e,f)
#define BX_REGISTER_DEFAULT_IOREAD_HANDLER(b,c,d,e) pluginRegisterDefaultIOReadHandler(b,c,d,e)
#define BX_REGISTER_DEFAULT_IOWRITE_HANDLER(b,c,d,e) pluginRegisterDefaultIOWriteHandler(b,c,d,e)

#define BX_REGISTER_IRQ(b,c) pluginRegisterIRQ(b,c)
#define BX_UNREGISTER_IRQ(b) pluginUnregisterIRQ(b)

#else

#define BX_INIT_DEVICES() {bx_devices.init(BX_MEM(0)); }
#define BX_RESET_DEVICES(type) {bx_devices.reset(type); }
// When plugins are off, BX_LOAD_PLUGIN will call the plugin_init function
// directly.
#define BX_LOAD_PLUGIN(name,type) {lib##name##_LTX_plugin_init(NULL,type,0,NULL);}
#define BX_REGISTER_IOREAD_HANDLER(b,c,d,e,f) bx_devices.register_io_read_handler(b,c,d,e)
#define BX_REGISTER_IOWRITE_HANDLER(b,c,d,e,f) bx_devices.register_io_write_handler(b,c,d,e)
#define BX_REGISTER_DEFAULT_IOREAD_HANDLER(b,c,d,e) bx_devices.register_default_io_read_handler(b,c,d)
#define BX_REGISTER_DEFAULT_IOWRITE_HANDLER(b,c,d,e) bx_devices.register_default_io_write_handler(b,c,d)
#define BX_REGISTER_IRQ(b,c) bx_devices.register_irq(b,c)
#define BX_UNREGISTER_IRQ(b) bx_devices.unregister_irq(b)

#endif // #if BX_PLUGINS

#define BX_IOAPIC_PRESENT() (bx_devices.ioapic != NULL)

// FIXME Do we really need pluginRegisterTimer ?
#define BX_REGISTER_TIMER(a,b,c,d,e,f) bx_pc_system.register_timer(a,b,c,d,e,f)

///////// CMOS macros
#define BX_GET_CMOS_REG(a) (bx_devices.pluginCmosDevice->get_reg(a))
#define BX_SET_CMOS_REG(a,b) (bx_devices.pluginCmosDevice->set_reg(a,b))
#define BX_CMOS_CHECKSUM() (bx_devices.pluginCmosDevice->checksum_cmos())
#define BX_GET_CMOS_TIMEVAL() (bx_devices.pluginCmosDevice->get_timeval())

///////// keyboard macros
#define DEV_mouse_motion(dx, dy, state) \
    (bx_devices.pluginKeyboard->mouse_motion(dx, dy, state))
#define DEV_kbd_gen_scancode(scancode) \
    (bx_devices.pluginKeyboard->gen_scancode(scancode))
#define DEV_kbd_put_scancode(scancode, count) \
    (bx_devices.pluginKeyboard->put_scancode(scancode, count))
#define DEV_kbd_paste_bytes(bytes, count) \
    (bx_devices.pluginKeyboard->paste_bytes(bytes,count))
#define DEV_kbd_paste_delay_changed() \
    (bx_devices.pluginKeyboard->paste_delay_changed())
#define DEV_mouse_enabled_changed(val) \
    (bx_devices.pluginKeyboard->mouse_enabled_changed(val))

///////// hard drive macros
#define BX_HD_READ_HANDLER(a, b, c) \
    (bx_devices.pluginHardDrive->virt_read_handler(b, c))
#define BX_HD_WRITE_HANDLER(a, b, c, d) \
    (bx_devices.pluginHardDrive->virt_write_handler(b, c, d))
#define BX_HD_GET_FIRST_CD_HANDLE() \
    (bx_devices.pluginHardDrive->get_first_cd_handle())
#define BX_HD_GET_DEVICE_HANDLE(a,b) \
    (bx_devices.pluginHardDrive->get_device_handle(a,b))
#define BX_HD_GET_CD_MEDIA_STATUS(handle) \
    (bx_devices.pluginHardDrive->get_cd_media_status(handle))
#define BX_HD_SET_CD_MEDIA_STATUS(handle, status) \
    (bx_devices.pluginHardDrive->set_cd_media_status(handle, status))
#define BX_HD_CLOSE_HARDDRIVE()  bx_devices.pluginHardDrive->close_harddrive()
#define BX_HARD_DRIVE_PRESENT() (bx_devices.pluginHardDrive != &bx_devices.stubHardDrive)

#define BX_BULK_IO_QUANTUM_REQUESTED() (bx_devices.bulkIOQuantumsRequested)
#define BX_BULK_IO_QUANTUM_TRANSFERRED() (bx_devices.bulkIOQuantumsTransferred)
#define BX_BULK_IO_HOST_ADDR() (bx_devices.bulkIOHostAddr)

///////// FLOPPY macros
#define BX_FLOPPY_GET_MEDIA_STATUS(drive) bx_devices.pluginFloppyDevice->get_media_status(drive)
#define BX_FLOPPY_SET_MEDIA_STATUS(drive, status)  bx_devices.pluginFloppyDevice->set_media_status(drive, status)
#define BX_FLOPPY_PRESENT() (bx_devices.pluginFloppyDevice != &bx_devices.stubFloppy)

///////// DMA macros
#define BX_REGISTER_DMA8_CHANNEL(channel, dmaRead, dmaWrite, name) \
  (bx_devices.pluginDmaDevice->registerDMA8Channel(channel, dmaRead, dmaWrite, name))
#define BX_REGISTER_DMA16_CHANNEL(channel, dmaRead, dmaWrite, name) \
  (bx_devices.pluginDmaDevice->registerDMA16Channel(channel, dmaRead, dmaWrite, name))
#define BX_UNREGISTER_DMA_CHANNEL(channel) \
  (bx_devices.pluginDmaDevice->unregisterDMAChannel(channel, dmaRead, dmaWrite, name))
#define BX_DMA_SET_DRQ(channel, val) \
  (bx_devices.pluginDmaDevice->set_DRQ(channel, val))
#define BX_DMA_GET_TC() \
  (bx_devices.pluginDmaDevice->get_TC())
#define BX_DMA_RAISE_HLDA() \
  (bx_devices.pluginDmaDevice->raise_HLDA())

///////// PIC macros
#define BX_PIC_LOWER_IRQ(b)  (bx_devices.pluginPicDevice->lower_irq(b))
#define BX_PIC_RAISE_IRQ(b)  (bx_devices.pluginPicDevice->raise_irq(b))
#define BX_PIC_IAC()         (bx_devices.pluginPicDevice->IAC())

///////// VGA macros
#define BX_VGA_MEM_READ(addr) (bx_devices.pluginVgaDevice->mem_read(addr))
#define BX_VGA_MEM_WRITE(addr, val) (bx_devices.pluginVgaDevice->mem_write(addr, val))
#define BX_VGA_REDRAW_AREA(left, top, right, bottom) \
  (bx_devices.pluginVgaDevice->redraw_area(left, top, right, bottom))
#define BX_VGA_GET_TEXT_SNAPSHOT(rawsnap, height, width) \
  (bx_devices.pluginVgaDevice->get_text_snapshot(rawsnap, height, width))
#define BX_VGA_REFRESH() \
  (bx_devices.pluginVgaDevice->trigger_timer(bx_devices.pluginVgaDevice))
#define BX_VGA_SET_UPDATE_INTERVAL(val) \
  (bx_devices.pluginVgaDevice->set_update_interval(val))


#if BX_HAVE_DLFCN_H
#include <dlfcn.h>
#endif

typedef Bit32u (*ioReadHandler_t)(void *, Bit32u, unsigned);
typedef void   (*ioWriteHandler_t)(void *, Bit32u, Bit32u, unsigned);

extern plugin_t *plugins;

typedef struct _device_t
{
    const char *name;
    plugin_t *plugin;
    void (*device_init_mem)(BX_MEM_C *);
    void (*device_init_dev)();
    void (*device_reset)(unsigned);
    void (*device_load_state)();
    void (*device_save_state)();

    int use_devmodel_interface;  // BBD hack
    class bx_devmodel_c *devmodel;  // BBD hack

    struct _device_t *next;
} device_t;


extern device_t *devices;

void plugin_startup (void);
void plugin_load (char *name, char *args, plugintype_t);
plugin_t *plugin_unload (plugin_t *plugin);
void plugin_init_all (void);
void plugin_fini_all (void);

/* === Device Stuff === */
typedef void (*deviceInitMem_t)(BX_MEM_C *);
typedef void (*deviceInitDev_t)(void);
typedef void (*deviceReset_t)(unsigned);
typedef void (*deviceLoad_t)(void);
typedef void (*deviceSave_t)(void);

void pluginRegisterDevice(deviceInitMem_t init_mem, deviceInitDev_t init_dev,
                          deviceReset_t reset, deviceLoad_t load, 
                          deviceSave_t save, char *name);
void pluginRegisterDeviceDevmodel(plugin_t *plugin, plugintype_t type, bx_devmodel_c *dev, char *name);
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

/* === A20 enable line stuff === */
extern unsigned (*pluginGetA20E)(void);
extern void     (*pluginSetA20E)(unsigned val);

/* === IRQ stuff === */
extern void  (*pluginRegisterIRQ)(unsigned irq, const char *name);
extern void  (*pluginUnregisterIRQ)(unsigned irq, const char *name);

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
extern void  (* pluginVGARefresh)(void *);
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
int bx_load_plugin (const char *name, plugintype_t type);
extern void bx_init_plugins (void);
extern void bx_reset_plugins (unsigned);

// every plugin must define these, within the extern"C" block, so that
// a non-mangled function symbol is available in the shared library.
void plugin_fini(void);
int plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[]);

// still in extern "C"
#define DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(mod) \
  int lib##mod##_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[]); \
  void lib##mod##_LTX_plugin_fini(void);
  
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(harddrv)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(keyboard)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(serial)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(unmapped)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(biosdev)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(cmos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(dma)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(vga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(floppy)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(parallel)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(amigaos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(beos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(carbon)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(macintosh)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(nogui)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(rfb)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sdl)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(term)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(win32)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(wx)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(x)


#ifdef __cplusplus
};
#endif

#endif /* __PLUGIN_H */
