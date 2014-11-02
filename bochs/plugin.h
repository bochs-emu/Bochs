/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
#define BX_PLUGIN_BIOSDEV   "biosdev"
#define BX_PLUGIN_CMOS      "cmos"
#define BX_PLUGIN_VGA       "vga"
#define BX_PLUGIN_CIRRUS    "svga_cirrus"
#define BX_PLUGIN_FLOPPY    "floppy"
#define BX_PLUGIN_PARALLEL  "parallel"
#define BX_PLUGIN_SERIAL    "serial"
#define BX_PLUGIN_KEYBOARD  "keyboard"
#define BX_PLUGIN_BUSMOUSE  "busmouse"
#define BX_PLUGIN_HARDDRV   "harddrv"
#define BX_PLUGIN_HDIMAGE   "hdimage"
#define BX_PLUGIN_DMA       "dma"
#define BX_PLUGIN_PIC       "pic"
#define BX_PLUGIN_PIT       "pit"
#define BX_PLUGIN_PCI       "pci"
#define BX_PLUGIN_PCI2ISA   "pci2isa"
#define BX_PLUGIN_PCI_IDE   "pci_ide"
#define BX_PLUGIN_SOUNDMOD  "soundmod"
#define BX_PLUGIN_SB16      "sb16"
#define BX_PLUGIN_ES1370    "es1370"
#define BX_PLUGIN_NETMOD    "netmod"
#define BX_PLUGIN_NE2K      "ne2k"
#define BX_PLUGIN_EXTFPUIRQ "extfpuirq"
#define BX_PLUGIN_PCIDEV    "pcidev"
#define BX_PLUGIN_USB_COMMON "usb_common"
#define BX_PLUGIN_USB_UHCI  "usb_uhci"
#define BX_PLUGIN_USB_OHCI  "usb_ohci"
#define BX_PLUGIN_USB_XHCI  "usb_xhci"
#define BX_PLUGIN_PCIPNIC   "pcipnic"
#define BX_PLUGIN_E1000     "e1000"
#define BX_PLUGIN_GAMEPORT  "gameport"
#define BX_PLUGIN_SPEAKER   "speaker"
#define BX_PLUGIN_ACPI      "acpi"
#define BX_PLUGIN_IODEBUG   "iodebug"
#define BX_PLUGIN_IOAPIC    "ioapic"
#define BX_PLUGIN_VOODOO    "voodoo"


#define BX_REGISTER_DEVICE_DEVMODEL(a,b,c,d) pluginRegisterDeviceDevmodel(a,b,c,d)
#define BX_UNREGISTER_DEVICE_DEVMODEL(a) pluginUnregisterDeviceDevmodel(a)
#define PLUG_device_present(a) pluginDevicePresent(a)

#if BX_PLUGINS

#define PLUG_load_plugin(name,type) {bx_load_plugin(#name,type);}
#define PLUG_load_opt_plugin(name) bx_load_plugin(name,PLUGTYPE_OPTIONAL)
#define PLUG_load_user_plugin(name) {bx_load_plugin(name,PLUGTYPE_USER);}
#define PLUG_unload_plugin(name) {bx_unload_plugin(#name,1);}
#define PLUG_unload_opt_plugin(name) bx_unload_plugin(name,1)
#define PLUG_unload_user_plugin(name) {bx_unload_plugin(name,1);}

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

// When plugins are off, PLUG_load_plugin will call the plugin_init function
// directly.
#define PLUG_load_plugin(name,type) {lib##name##_LTX_plugin_init(NULL,type,0,NULL);}
#define PLUG_load_opt_plugin(name) bx_load_opt_plugin(name)
#define PLUG_unload_plugin(name) {lib##name##_LTX_plugin_fini();}
#define PLUG_unload_opt_plugin(name) bx_unload_opt_plugin(name,1);
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

///////// Common device macros
#define DEV_init_devices() {bx_devices.init(BX_MEM(0)); }
#define DEV_reset_devices(type) {bx_devices.reset(type); }
#define DEV_register_state() {bx_devices.register_state(); }
#define DEV_after_restore_state() {bx_devices.after_restore_state(); }

#define DEV_register_timer(a,b,c,d,e,f) bx_pc_system.register_timer(a,b,c,d,e,f)
#define DEV_mouse_enabled_changed(en) (bx_devices.mouse_enabled_changed(en))
#define DEV_mouse_motion(dx, dy, dz, bs, absxy) (bx_devices.mouse_motion(dx, dy, dz, bs, absxy))

///////// Removable devices macros
#define DEV_optional_key_enq(a) (bx_devices.optional_key_enq(a))
#define DEV_register_removable_keyboard(a,b) (bx_devices.register_removable_keyboard(a,b))
#define DEV_unregister_removable_keyboard(a) (bx_devices.unregister_removable_keyboard(a))
#define DEV_register_default_mouse(a,b,c) (bx_devices.register_default_mouse(a,b,c))
#define DEV_register_removable_mouse(a,b,c) (bx_devices.register_removable_mouse(a,b,c))
#define DEV_unregister_removable_mouse(a) (bx_devices.unregister_removable_mouse(a))

///////// I/O APIC macros
#define DEV_ioapic_present() (bx_devices.pluginIOAPIC != &bx_devices.stubIOAPIC)
#define DEV_ioapic_set_enabled(a,b) (bx_devices.pluginIOAPIC->set_enabled(a,b))
#define DEV_ioapic_receive_eoi(a) (bx_devices.pluginIOAPIC->receive_eoi(a))
#define DEV_ioapic_set_irq_level(a,b) (bx_devices.pluginIOAPIC->set_irq_level(a,b))

///////// CMOS macros
#define DEV_cmos_get_reg(a) (bx_devices.pluginCmosDevice->get_reg(a))
#define DEV_cmos_set_reg(a,b) (bx_devices.pluginCmosDevice->set_reg(a,b))
#define DEV_cmos_checksum() (bx_devices.pluginCmosDevice->checksum_cmos())
#define DEV_cmos_get_timeval() (bx_devices.pluginCmosDevice->get_timeval())
#define DEV_cmos_present() (bx_devices.pluginCmosDevice != &bx_devices.stubCmos)

///////// keyboard macros
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
#define DEV_hd_get_cd_media_status(handle) \
    (bx_devices.pluginHardDrive->get_cd_media_status(handle))
#define DEV_hd_set_cd_media_status(handle, status) \
    (bx_devices.pluginHardDrive->set_cd_media_status(handle, status))
#define DEV_hd_present() (bx_devices.pluginHardDrive != &bx_devices.stubHardDrive)
#define DEV_hd_bmdma_read_sector(a,b,c) bx_devices.pluginHardDrive->bmdma_read_sector(a,b,c)
#define DEV_hd_bmdma_write_sector(a,b) bx_devices.pluginHardDrive->bmdma_write_sector(a,b)
#define DEV_hd_bmdma_complete(a) bx_devices.pluginHardDrive->bmdma_complete(a)
#define DEV_hdimage_init_image(a,b,c) bx_devices.pluginHDImageCtl->init_image(a,b,c)
#define DEV_hdimage_init_cdrom(a) bx_devices.pluginHDImageCtl->init_cdrom(a)

#define DEV_bulk_io_quantum_requested() (bx_devices.bulkIOQuantumsRequested)
#define DEV_bulk_io_quantum_transferred() (bx_devices.bulkIOQuantumsTransferred)
#define DEV_bulk_io_host_addr() (bx_devices.bulkIOHostAddr)

///////// FLOPPY macro
#define DEV_floppy_set_media_status(drive, status)  bx_devices.pluginFloppyDevice->set_media_status(drive, status)

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

///////// VGA macros
#define DEV_vga_mem_read(addr) (bx_devices.pluginVgaDevice->mem_read(addr))
#define DEV_vga_mem_write(addr, val) (bx_devices.pluginVgaDevice->mem_write(addr, val))
#define DEV_vga_redraw_area(left, top, right, bottom) \
  (bx_devices.pluginVgaDevice->redraw_area(left, top, right, bottom))
#define DEV_vga_get_text_snapshot(rawsnap, height, width) \
  (bx_devices.pluginVgaDevice->get_text_snapshot(rawsnap, height, width))
#define DEV_vga_refresh(a) \
  (bx_devices.pluginVgaDevice->refresh_display(bx_devices.pluginVgaDevice,a))
#define DEV_vga_set_override(a,b) (bx_devices.pluginVgaDevice->set_override(a,b))

///////// PCI macros
#define DEV_register_pci_handlers(a,b,c,d) \
  (bx_devices.register_pci_handlers(a,b,c,d))
#define DEV_pci_get_confAddr() bx_devices.pci_get_confAddr()
#define DEV_pci_set_irq(a,b,c) bx_devices.pluginPci2IsaBridge->pci_set_irq(a,b,c)
#define DEV_pci_set_base_mem(a,b,c,d,e,f) \
  (bx_devices.pci_set_base_mem(a,b,c,d,e,f))
#define DEV_pci_set_base_io(a,b,c,d,e,f,g,h) \
  (bx_devices.pci_set_base_io(a,b,c,d,e,f,g,h))
#define DEV_ide_bmdma_present() bx_devices.pluginPciIdeController->bmdma_present()
#define DEV_ide_bmdma_set_irq(a) bx_devices.pluginPciIdeController->bmdma_set_irq(a)
#define DEV_ide_bmdma_start_transfer(a) \
  bx_devices.pluginPciIdeController->bmdma_start_transfer(a)
#define DEV_acpi_generate_smi(a) bx_devices.pluginACPIController->generate_smi(a)

///////// Speaker macros
#define DEV_speaker_beep_on(frequency) bx_devices.pluginSpeaker->beep_on(frequency)
#define DEV_speaker_beep_off() bx_devices.pluginSpeaker->beep_off()

///////// Memory macros
#define DEV_register_memory_handlers(param,rh,wh,b,e) \
    bx_devices.mem->registerMemoryHandlers(param,rh,wh,b,e)
#define DEV_unregister_memory_handlers(param,b,e) \
    bx_devices.mem->unregisterMemoryHandlers(param,b,e)
#define DEV_mem_set_memory_type(a,b,c) \
    bx_devices.mem->set_memory_type((memory_area_t)a,b,c)
#define DEV_mem_set_bios_write(a) bx_devices.mem->set_bios_write(a)

///////// USB device macros
#define DEV_usb_init_device(a,b,c,d) (usbdev_type)bx_devices.pluginUsbDevCtl->init_device(a,b,(void**)c,d)
#define DEV_usb_send_msg(a,b) bx_devices.pluginUsbDevCtl->usb_send_msg((void*)a,b)

///////// Sound module macros
#define DEV_sound_get_module() \
  ((bx_sound_lowlevel_c*)bx_devices.pluginSoundModCtl->get_module())
#define DEV_soundmod_beep_on(a) bx_devices.pluginSoundModCtl->beep_on(a)
#define DEV_soundmod_beep_off() bx_devices.pluginSoundModCtl->beep_off()
#define DEV_soundmod_VOC_init_file(a) \
  (bx_devices.pluginSoundModCtl->VOC_init_file(a))
#define DEV_soundmod_VOC_write_block(a,b,c,d,e,f) \
  (bx_devices.pluginSoundModCtl->VOC_write_block(a,b,c,d,e,f))
#define DEV_soundmod_pcm_apply_volume(a,b,c,d,e,f) \
  (bx_devices.pluginSoundModCtl->pcm_apply_volume(a,b,c,d,e,f))

///////// Networking module macro
#define DEV_net_init_module(a,b,c,d) \
  ((eth_pktmover_c*)bx_devices.pluginNetModCtl->init_module(a,(void*)b,(void*)c,d))

///////// Gameport macro
#define DEV_gameport_set_enabled(a) bx_devices.pluginGameport->set_enabled(a)


#if BX_HAVE_DLFCN_H
#include <dlfcn.h>
#endif

typedef Bit32u (*ioReadHandler_t)(void *, Bit32u, unsigned);
typedef void   (*ioWriteHandler_t)(void *, Bit32u, Bit32u, unsigned);

extern plugin_t *plugins;

typedef struct _device_t
{
    const char   *name;
    plugin_t     *plugin;
    plugintype_t plugtype;

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

BOCHSAPI void pluginRegisterDeviceDevmodel(plugin_t *plugin, plugintype_t type, bx_devmodel_c *dev, const char *name);
BOCHSAPI void pluginUnregisterDeviceDevmodel(const char *name);
BOCHSAPI bx_bool pluginDevicePresent(const char *name);

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

/* === IRQ stuff === */
BOCHSAPI extern void  (*pluginRegisterIRQ)(unsigned irq, const char *name);
BOCHSAPI extern void  (*pluginUnregisterIRQ)(unsigned irq, const char *name);

/* === Timer stuff === */
BOCHSAPI extern int     (*pluginRegisterTimer)(void *this_ptr, void (*funct)(void *),
                             Bit32u useconds, bx_bool continuous,
                             bx_bool active, const char *name);

BOCHSAPI extern void    (*pluginActivateTimer)(unsigned id, Bit32u usec, bx_bool continuous);

/* === HRQ stuff === */
BOCHSAPI extern void    (*pluginSetHRQ)(unsigned val);
BOCHSAPI extern void    (*pluginSetHRQHackCallback)(void (*callback)(void));

void plugin_abort(void);

int bx_load_plugin(const char *name, plugintype_t type);
extern void bx_unload_plugin(const char *name, bx_bool devflag);
extern void bx_init_plugins(void);
extern void bx_reset_plugins(unsigned);
extern void bx_unload_plugins(void);
extern void bx_unload_core_plugins(void);
extern void bx_plugins_register_state(void);
extern void bx_plugins_after_restore_state(void);

#if !BX_PLUGINS
int bx_load_opt_plugin(const char *name);
int bx_unload_opt_plugin(const char *name, bx_bool devflag);
#endif

// every plugin must define these, within the extern"C" block, so that
// a non-mangled function symbol is available in the shared library.
void plugin_fini(void);
int plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[]);

// still in extern "C"
#if BX_PLUGINS && defined(_MSC_VER)
#define DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl lib##mod##_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[]); \
  extern "C" __declspec(dllexport) void __cdecl lib##mod##_LTX_plugin_fini(void);
#else
#define DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(mod) \
  int CDECL lib##mod##_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[]); \
  void CDECL lib##mod##_LTX_plugin_fini(void);
#endif

DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(harddrv)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(hdimage)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(keyboard)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(busmouse)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(serial)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(unmapped)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(biosdev)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(cmos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(dma)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pit)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(vga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(svga_cirrus)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(floppy)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(parallel)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci2isa)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pci_ide)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pcidev)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(usb_common)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(usb_uhci)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(usb_ohci)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(usb_xhci)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(soundmod)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sb16)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(es1370)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(netmod)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(ne2k)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(pcipnic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(e1000)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(extfpuirq)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(gameport)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(speaker)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(acpi)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(iodebug)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(ioapic)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(voodoo)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(amigaos)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(carbon)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(macintosh)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(nogui)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(rfb)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sdl)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(sdl2)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(svga)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(term)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(vncsrv)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(win32)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(wx)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(x)
DECLARE_PLUGIN_INIT_FINI_FOR_MODULE(user)


#ifdef __cplusplus
}
#endif

#endif /* __PLUGIN_H */
