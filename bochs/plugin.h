/////////////////////////////////////////////////////////////////////////
// $Id: plugin.h,v 1.53 2006-09-10 09:13:47 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
// This file provides macros and types needed for plugins.  It is based on
// the plugin.h file from plex86, but with significant changes to make
// it work in Bochs.  
// Plex86 is Copyright (C) 1999-2000  The plex86 developers team
//
/////////////////////////////////////////////////////////////////////////

#ifndef __PLUGIN_H
#define __PLUGIN_H

#include "extplugin.h"

class bx_devices_c;
BOCHSAPI extern logfunctions  *pluginlog;

#ifdef __cplusplus
extern "C" {
#endif

#define BX_PLUGIN_UNMAPPED  "unmapped"
#define BX_PLUGIN_BIOSDEV   " biosdev"
#define BX_PLUGIN_CMOS      "cmos"
#define BX_PLUGIN_VGA       "vga"
#define BX_PLUGIN_FLOPPY    "floppy"
#define BX_PLUGIN_PARALLEL  "parallel"
#define BX_PLUGIN_SERIAL    "serial"
#define BX_PLUGIN_KEYBOARD  "keyboard"
#define BX_PLUGIN_BUSMOUSE  "busmouse"
#define BX_PLUGIN_HARDDRV   "harddrv"
#define BX_PLUGIN_DMA       "dma"
#define BX_PLUGIN_PIC       "pic"
#define BX_PLUGIN_PCI       "pci"
#define BX_PLUGIN_PCI2ISA   "pci2isa"
#define BX_PLUGIN_PCI_IDE   "pci_ide"
#define BX_PLUGIN_SB16      "sb16"
#define BX_PLUGIN_NE2K      "ne2k"
#define BX_PLUGIN_EXTFPUIRQ "extfpuirq"
#define BX_PLUGIN_PCIVGA    "pcivga"
#define BX_PLUGIN_PCIDEV    "pcidev"
#define BX_PLUGIN_PCIUSB    "pciusb"
#define BX_PLUGIN_PCIPNIC   "pcipnic"
#define BX_PLUGIN_GAMEPORT  "gameport"
#define BX_PLUGIN_SPEAKER   "speaker"


#define BX_REGISTER_DEVICE_DEVMODEL(a,b,c,d) pluginRegisterDeviceDevmodel(a,b,c,d)

#if BX_PLUGINS

#define DEV_init_devices() {bx_devices.init(BX_MEM(0)); }
#define DEV_reset_devices(type) {bx_devices.reset(type); }
#define DEV_register_state() {bx_devices.register_state(); }
#define DEV_after_restore_state() {bx_devices.after_restore_state(); }
#define PLUG_load_plugin(name,type) {bx_load_plugin(#name,type);}
#define PLUG_unload_plugin(name) {bx_unload_plugin(#name);}

#define DEV_register_ioread_handler(b,c,d,e,f)  pluginRegisterIOReadHandler(b,c,d,e,f)
#define DEV_register_iowrite_handler(b,c,d,e,f) pluginRegisterIOWriteHandler(b,c,d,e,f)
#define DEV_unregister_ioread_handler(b,c,d,e)  pluginUnregisterIOReadHandler(b,c,d,e)
#define DEV_unregister_iowrite_handler(b,c,d,e) pluginUnregisterIOWriteHandler(b,c,d,e)
#define DEV_register_ioread_handler_range(b,c,d,e,f,g)  pluginRegisterIOReadHandlerRange(b,c,d,e,f,g)
#define DEV_register_iowrite_handler_range(b,c,d,e,f,g) pluginRegisterIOWriteHandlerRange(b,c,d,e,f,g)
#define DEV_unregister_ioread_handler_range(b,c,d,e,f)  pluginUnregisterIOReadHandlerRange(b,c,d,e,f)
#define DEV_unregister_iowrite_handler_range(b,c,d,e,f) pluginUnregisterIOWriteHandlerRange(b,c,d,e,f)
#define DEV_register_default_ioread_handler(b,c,d,e) pluginRegisterDefaultIOReadHandler(b,c,d,e)
#define DEV_register_default_iowrite_handler(b,c,d,e) pluginRegisterDefaultIOWriteHandler(b,c,d,e)

#define DEV_register_irq(b,c) pluginRegisterIRQ(b,c)
#define DEV_unregister_irq(b,c) pluginUnregisterIRQ(b,c)

#else

#define DEV_init_devices() {bx_devices.init(BX_MEM(0)); }
#define DEV_reset_devices(type) {bx_devices.reset(type); }
#define DEV_register_state() {bx_devices.register_state(); }
#define DEV_after_restore_state() {bx_devices.after_restore_state(); }
// When plugins are off, PLUG_load_plugin will call the plugin_init function
// directly.
#define PLUG_load_plugin(name,type) {lib##name##_LTX_plugin_init(NULL,type,0,NULL);}
#define PLUG_unload_plugin(name) {lib##name##_LTX_plugin_fini();}
#define DEV_register_ioread_handler(b,c,d,e,f) bx_devices.register_io_read_handler(b,c,d,e,f)
#define DEV_register_iowrite_handler(b,c,d,e,f) bx_devices.register_io_write_handler(b,c,d,e,f)
#define DEV_unregister_ioread_handler(b,c,d,e)  bx_devices.unregister_io_read_handler(b,c,d,e)
#define DEV_unregister_iowrite_handler(b,c,d,e) bx_devices.unregister_io_write_handler(b,c,d,e)
#define DEV_register_ioread_handler_range(b,c,d,e,f,g)  bx_devices.register_io_read_handler_range(b,c,d,e,f,g)
#define DEV_register_iowrite_handler_range(b,c,d,e,f,g) bx_devices.register_io_write_handler_range(b,c,d,e,f,g)
#define DEV_unregister_ioread_handler_range(b,c,d,e,f)  bx_devices.unregister_io_read_handler_range(b,c,d,e,f)
#define DEV_unregister_iowrite_handler_range(b,c,d,e,f) bx_devices.unregister_io_write_handler_range(b,c,d,e,f)
#define DEV_register_default_ioread_handler(b,c,d,e) bx_devices.register_default_io_read_handler(b,c,d,e)
#define DEV_register_default_iowrite_handler(b,c,d,e) bx_devices.register_default_io_write_handler(b,c,d,e)
#define DEV_register_irq(b,c) bx_devices.register_irq(b,c)
#define DEV_unregister_irq(b,c) bx_devices.unregister_irq(b,c)

#endif // #if BX_PLUGINS

#define DEV_ioapic_present() (bx_devices.ioapic != NULL)

// FIXME Do we really need pluginRegisterTimer ?
#define DEV_register_timer(a,b,c,d,e,f) bx_pc_system.register_timer(a,b,c,d,e,f)

///////// CMOS macros
#define DEV_cmos_get_reg(a) (bx_devices.pluginCmosDevice->get_reg(a))
#define DEV_cmos_set_reg(a,b) (bx_devices.pluginCmosDevice->set_reg(a,b))
#define DEV_cmos_checksum() (bx_devices.pluginCmosDevice->checksum_cmos())
#define DEV_cmos_get_timeval() (bx_devices.pluginCmosDevice->get_timeval())
#define DEV_cmos_save_image() (bx_devices.pluginCmosDevice->save_image())
#define DEV_cmos_present() (bx_devices.pluginCmosDevice != &bx_devices.stubCmos)

///////// keyboard macros
#define DEV_mouse_motion(dx, dy, state) \
    (bx_devices.pluginKeyboard->mouse_motion(dx, dy, 0, state))
#define DEV_mouse_motion_ext(dx, dy, dz, state) \
    (bx_devices.pluginKeyboard->mouse_motion(dx, dy, dz, state))
#define DEV_kbd_gen_scancode(key) \
    (bx_devices.pluginKeyboard->gen_scancode(key))
#define DEV_kbd_paste_bytes(bytes, count) \
    (bx_devices.pluginKeyboard->paste_bytes(bytes,count))

///////// hard drive macros
#define DEV_hd_read_handler(a, b, c) \
    (bx_devices.pluginHardDrive->virt_read_handler(b, c))
#define DEV_hd_write_handler(a, b, c, d) \
    (bx_devices.pluginHardDrive->virt_write_handler(b, c, d))
#define DEV_hd_get_first_cd_handle() \
    (bx_devices.pluginHardDrive->get_first_cd_handle())
#define DEV_hd_get_device_handle(a,b) \
    (bx_devices.pluginHardDrive->get_device_handle(a,b))
#define DEV_hd_get_cd_media_status(handle) \
    (bx_devices.pluginHardDrive->get_cd_media_status(handle))
#define DEV_hd_set_cd_media_status(handle, status) \
    (bx_devices.pluginHardDrive->set_cd_media_status(handle, status))
#define DEV_hd_close_harddrive()  bx_devices.pluginHardDrive->close_harddrive()
#define DEV_hd_present() (bx_devices.pluginHardDrive != &bx_devices.stubHardDrive)
#define DEV_hd_bmdma_read_sector(a,b,c) bx_devices.pluginHardDrive->bmdma_read_sector(a,b,c)
#define DEV_hd_bmdma_write_sector(a,b) bx_devices.pluginHardDrive->bmdma_write_sector(a,b)
#define DEV_hd_bmdma_complete(a) bx_devices.pluginHardDrive->bmdma_complete(a)

#define DEV_bulk_io_quantum_requested() (bx_devices.bulkIOQuantumsRequested)
#define DEV_bulk_io_quantum_transferred() (bx_devices.bulkIOQuantumsTransferred)
#define DEV_bulk_io_host_addr() (bx_devices.bulkIOHostAddr)

///////// FLOPPY macros
#define DEV_floppy_get_media_status(drive) bx_devices.pluginFloppyDevice->get_media_status(drive)
#define DEV_floppy_set_media_status(drive, status)  bx_devices.pluginFloppyDevice->set_media_status(drive, status)
#define DEV_floppy_present() (bx_devices.pluginFloppyDevice != &bx_devices.stubFloppy)

///////// DMA macros
#define DEV_dma_register_8bit_channel(channel, dmaRead, dmaWrite, name) \
  (bx_devices.pluginDmaDevice->registerDMA8Channel(channel, dmaRead, dmaWrite, name))
#define DEV_dma_register_16bit_channel(channel, dmaRead, dmaWrite, name) \
  (bx_devices.pluginDmaDevice->registerDMA16Channel(channel, dmaRead, dmaWrite, name))
#define DEV_dma_unregister_channel(channel) \
  (bx_devices.pluginDmaDevice->unregisterDMAChannel(channel))
#define DEV_dma_set_drq(channel, val) \
  (bx_devices.pluginDmaDevice->set_DRQ(channel, val))
#define DEV_dma_get_tc() \
  (bx_devices.pluginDmaDevice->get_TC())
#define DEV_dma_raise_hlda() \
  (bx_devices.pluginDmaDevice->raise_HLDA())

///////// PIC macros
#define DEV_pic_lower_irq(b)  (bx_devices.pluginPicDevice->lower_irq(b))
#define DEV_pic_raise_irq(b)  (bx_devices.pluginPicDevice->raise_irq(b))
#define DEV_pic_set_mode(a,b) (bx_devices.pluginPicDevice->set_mode(a,b))
#define DEV_pic_iac()         (bx_devices.pluginPicDevice->IAC())
#define DEV_pic_show_pic_state() (bx_devices.pluginPicDevice->show_pic_state())

///////// VGA macros
#define DEV_vga_mem_read(addr) (bx_devices.pluginVgaDevice->mem_read(addr))
#define DEV_vga_mem_write(addr, val) (bx_devices.pluginVgaDevice->mem_write(addr, val))
#define DEV_vga_redraw_area(left, top, right, bottom) \
  (bx_devices.pluginVgaDevice->redraw_area(left, top, right, bottom))
#define DEV_vga_get_text_snapshot(rawsnap, height, width) \
  (bx_devices.pluginVgaDevice->get_text_snapshot(rawsnap, height, width))
#define DEV_vga_refresh() \
  (bx_devices.pluginVgaDevice->trigger_timer(bx_devices.pluginVgaDevice))
#define DEV_vga_get_actl_pal_idx(index) (bx_devices.pluginVgaDevice->get_actl_palette_idx(index))
#define DEV_vga_dump_status() (bx_devices.pluginVgaDevice->dump_status())

///////// PCI macros
#define DEV_register_pci_handlers(a,b,c,d) \
  (bx_devices.pluginPciBridge->register_pci_handlers(a,b,c,d))
#define DEV_is_pci_device(name) bx_devices.pluginPciBridge->is_pci_device(name)
#define DEV_pci_set_irq(a,b,c) bx_devices.pluginPci2IsaBridge->pci_set_irq(a,b,c)
#define DEV_pci_set_base_mem(a,b,c,d,e,f) \
  (bx_devices.pluginPciBridge->pci_set_base_mem(a,b,c,d,e,f))
#define DEV_pci_set_base_io(a,b,c,d,e,f,g,h) \
  (bx_devices.pluginPciBridge->pci_set_base_io(a,b,c,d,e,f,g,h))
#define DEV_pci_rd_memtype(addr) bx_devices.pluginPciBridge->rd_memType(addr)
#define DEV_pci_wr_memtype(addr) bx_devices.pluginPciBridge->wr_memType(addr)
#define DEV_pci_print_i440fx_state() bx_devices.pluginPciBridge->print_i440fx_state()
#define DEV_ide_bmdma_present() bx_devices.pluginPciIdeController->bmdma_present()
#define DEV_ide_bmdma_set_irq(a) bx_devices.pluginPciIdeController->bmdma_set_irq(a)

///////// NE2000 macro
#define DEV_ne2k_print_info(file,page,reg,brief) \
    bx_devices.pluginNE2kDevice->print_info(file,page,reg,brief)

///////// Speaker macros
#define DEV_speaker_beep_on(frequency) bx_devices.pluginSpeaker->beep_on(frequency)
#define DEV_speaker_beep_off() bx_devices.pluginSpeaker->beep_off()

///////// Serial macro
#define DEV_serial_mouse_enq(dx, dy, dz, state) \
    (bx_devices.pluginSerialDevice->serial_mouse_enq(dx, dy, dz, state))

///////// BUS mouse macro
#define DEV_bus_mouse_enq(dx, dy, dz, state) \
    (bx_devices.pluginBusMouse->bus_mouse_enq(dx, dy, 0, state))

///////// USB device macros
#if BX_SUPPORT_PCIUSB
#define DEV_usb_mouse_enq(dx, dy, dz, state) \
    (bx_devices.pluginPciUSBAdapter->usb_mouse_enq(dx, dy, dz, state))
#define DEV_usb_mouse_enable(enable) \
    (bx_devices.pluginPciUSBAdapter->usb_mouse_enable(enable))
#define DEV_usb_key_enq(scan_code) \
    (bx_devices.pluginPciUSBAdapter->usb_key_enq(scan_code))
#define DEV_usb_keyboard_connected() \
    (bx_devices.pluginPciUSBAdapter->usb_keyboard_connected())
#define DEV_usb_mouse_connected() \
    (bx_devices.pluginPciUSBAdapter->usb_mouse_connected())
#endif

//////// Memory macros
#define DEV_register_memory_handlers(param,rh,wh,b,e) \
    bx_devices.mem->registerMemoryHandlers(param,rh,wh,b,e)
#define DEV_unregister_memory_handlers(rh,wh,b,e) \
    bx_devices.mem->unregisterMemoryHandlers(rh,wh,b,e)


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

    class bx_devmodel_c *devmodel;  // BBD hack

    struct _device_t *next;
} device_t;


extern device_t *devices;

void plugin_startup(void);
void plugin_load(char *name, char *args, plugintype_t);
plugin_t *plugin_unload(plugin_t *plugin);
void plugin_init_all(void);
void plugin_fini_all(void);

/* === Device Stuff === */
typedef void (*deviceInitMem_t)(BX_MEM_C *);
typedef void (*deviceInitDev_t)(void);
typedef void (*deviceReset_t)(unsigned);

BOCHSAPI void pluginRegisterDeviceDevmodel(plugin_t *plugin, plugintype_t type, bx_devmodel_c *dev, char *name);
BOCHSAPI bx_bool pluginDevicePresent(char *name);

/* === IO port stuff === */
BOCHSAPI extern int (*pluginRegisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                                unsigned base, const char *name, Bit8u mask);
BOCHSAPI extern int (*pluginRegisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned base, const char *name, Bit8u mask);
BOCHSAPI extern int (*pluginUnregisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                                unsigned base, Bit8u mask);
BOCHSAPI extern int (*pluginUnregisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned base, Bit8u mask);
BOCHSAPI extern int (*pluginRegisterIOReadHandlerRange)(void *thisPtr, ioReadHandler_t callback,
                                unsigned base, unsigned end, const char *name, Bit8u mask);
BOCHSAPI extern int (*pluginRegisterIOWriteHandlerRange)(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned base, unsigned end, const char *name, Bit8u mask);
BOCHSAPI extern int (*pluginUnregisterIOReadHandlerRange)(void *thisPtr, ioReadHandler_t callback,
                                unsigned begin, unsigned end, Bit8u mask);
BOCHSAPI extern int (*pluginUnregisterIOWriteHandlerRange)(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned begin, unsigned end, Bit8u mask);
BOCHSAPI extern int (*pluginRegisterDefaultIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                                const char *name, Bit8u mask);
BOCHSAPI extern int (*pluginRegisterDefaultIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                                 const char *name, Bit8u mask);

/* === A20 enable line stuff === */
BOCHSAPI extern unsigned (*pluginGetA20E)(void);
BOCHSAPI extern void     (*pluginSetA20E)(unsigned val);

/* === IRQ stuff === */
BOCHSAPI extern void  (*pluginRegisterIRQ)(unsigned irq, const char *name);
BOCHSAPI extern void  (*pluginUnregisterIRQ)(unsigned irq, const char *name);

/* === Floppy stuff ===*/
BOCHSAPI extern unsigned (* pluginFloppyGetMediaStatus)(unsigned drive);
BOCHSAPI extern unsigned (* pluginFloppySetMediaStatus)(unsigned drive, unsigned status);

/* === VGA stuff === */
BOCHSAPI extern void (* pluginVGARedrawArea)(unsigned x0, unsigned y0,
                 unsigned width, unsigned height);
BOCHSAPI extern Bit8u (* pluginVGAMemRead)(Bit32u addr);
BOCHSAPI extern void  (* pluginVGAMemWrite)(Bit32u addr, Bit8u value);
BOCHSAPI extern void  (* pluginVGAGetTextSnapshot)(Bit8u **text_snapshot, 
		          unsigned *txHeight, unsigned *txWidth);
BOCHSAPI extern void  (* pluginVGARefresh)(void *);
BOCHSAPI extern void  (* pluginVGASetUpdateInterval)(unsigned);
BOCHSAPI extern Bit8u (* pluginVGAGetActlPaletteIdx)(Bit8u index);

/* === Timer stuff === */
BOCHSAPI extern int      (*pluginRegisterTimer)(void *this_ptr, void (*funct)(void *),
                             Bit32u useconds, bx_bool continuous,
                             bx_bool active, const char *name);

BOCHSAPI extern void     (*pluginActivateTimer)(unsigned id, Bit32u usec, bx_bool continuous);
BOCHSAPI extern void     (*pluginDeactivateTimer)(unsigned id);

/* === HRQ stuff === */
BOCHSAPI extern void     (*pluginSetHRQ)(unsigned val);
BOCHSAPI extern void     (*pluginSetHRQHackCallback)( void (*callback)(void) );

/* === PCI stuff === */
BOCHSAPI extern bx_bool  (*pluginRegisterPCIDevice)(void *this_ptr,
                             Bit32u (*bx_pci_read_handler)(void *, Bit8u, unsigned),
                             void(*bx_pci_write_handler)(void *, Bit8u, Bit32u, unsigned),
                             Bit8u *devfunc, const char *name, const char *descr);
BOCHSAPI extern Bit8u    (*pluginRd_memType)(Bit32u addr);
BOCHSAPI extern Bit8u    (*pluginWr_memType)(Bit32u addr);

void plugin_abort(void);

int bx_load_plugin(const char *name, plugintype_t type);
extern void bx_unload_plugin(const char *name);
extern void bx_init_plugins(void);
extern void bx_reset_plugins(unsigned);
extern void bx_unload_plugins(void);
#if BX_SUPPORT_SAVE_RESTORE
extern void bx_plugins_register_state();
extern void bx_plugins_after_restore_state();
#endif

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
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(busmouse)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(serial)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(unmapped)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(biosdev)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(cmos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(dma)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(vga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(floppy)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(parallel)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci2isa)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci_ide)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pcivga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pcidev)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pciusb)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pcipnic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sb16)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(ne2k)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(extfpuirq)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(gameport)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(speaker)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(amigaos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(beos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(carbon)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(macintosh)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(nogui)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(rfb)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sdl)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(svga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(term)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(win32)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(wx)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(x)


#ifdef __cplusplus
}
#endif

#endif /* __PLUGIN_H */
