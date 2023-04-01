/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2023  The Bochs Project
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

class BX_MEM_C;
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
#define BX_PLUGIN_DMA       "dma"
#define BX_PLUGIN_PIC       "pic"
#define BX_PLUGIN_PIT       "pit"
#define BX_PLUGIN_PCI       "pci"
#define BX_PLUGIN_PCI2ISA   "pci2isa"
#define BX_PLUGIN_PCI_IDE   "pci_ide"
#define BX_PLUGIN_SB16      "sb16"
#define BX_PLUGIN_ES1370    "es1370"
#define BX_PLUGIN_NE2K      "ne2k"
#define BX_PLUGIN_EXTFPUIRQ "extfpuirq"
#define BX_PLUGIN_PCIDEV    "pcidev"
#define BX_PLUGIN_USB_UHCI  "usb_uhci"
#define BX_PLUGIN_USB_OHCI  "usb_ohci"
#define BX_PLUGIN_USB_EHCI  "usb_ehci"
#define BX_PLUGIN_USB_XHCI  "usb_xhci"
#define BX_PLUGIN_PCIPNIC   "pcipnic"
#define BX_PLUGIN_E1000     "e1000"
#define BX_PLUGIN_GAMEPORT  "gameport"
#define BX_PLUGIN_SPEAKER   "speaker"
#define BX_PLUGIN_ACPI      "acpi"
#define BX_PLUGIN_IODEBUG   "iodebug"
#define BX_PLUGIN_IOAPIC    "ioapic"
#define BX_PLUGIN_HPET      "hpet"
#define BX_PLUGIN_VOODOO    "voodoo"


#define BX_REGISTER_DEVICE_DEVMODEL(a,b,c,d) pluginRegisterDeviceDevmodel(a,b,c,d)
#define BX_UNREGISTER_DEVICE_DEVMODEL(a,b) pluginUnregisterDeviceDevmodel(a,b)
#define PLUG_device_present(a) pluginDevicePresent(a)

#if BX_PLUGINS

// hardcoded load plugin macro for PLUGTYPE_CORE and PLUGTYPE_STANDARD
#define PLUG_load_plugin(name,type) {bx_load_plugin(#name,type);}
// newer plugin macros for variable plugin handling
#define PLUG_get_plugins_count(type) bx_get_plugins_count(type)
#define PLUG_get_plugin_name(type,index) bx_get_plugin_name(type,index)
#define PLUG_get_plugin_flags(type,index) bx_get_plugin_flags(type,index)
#define PLUG_load_plugin_var(name,type) {bx_load_plugin(name,type);}
#define PLUG_load_opt_plugin(name) bx_load_plugin(name,PLUGTYPE_OPTIONAL)
#define PLUG_unload_opt_plugin(name) bx_unload_plugin(name,1)
#define PLUG_unload_plugin_type(name,type) {bx_unload_plugin_type(name,type);}

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

// When plugins are off, PLUG_load_plugin will call the plugin_entry function
// directly (PLUGTYPE_CORE and PLUGTYPE_STANDARD only).
#define PLUG_load_plugin(name,type) {lib##name##_plugin_entry(NULL,type,PLUGIN_INIT);}
// Builtin plugins macros
#define PLUG_get_plugins_count(type) bx_get_plugins_count_np(type)
#define PLUG_get_plugin_name(type,index) bx_get_plugin_name_np(type,index)
#define PLUG_get_plugin_flags(type,index) bx_get_plugin_flags_np(type,index)
#define PLUG_load_plugin_var(name,type) bx_load_plugin_np(name,type)
#define PLUG_load_opt_plugin(name) bx_load_plugin_np(name,PLUGTYPE_OPTIONAL)
#define PLUG_unload_opt_plugin(name) bx_unload_opt_plugin(name,1)

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

///////// Removable devices macros
#define DEV_register_default_keyboard(a,b,c) (bx_devices.register_default_keyboard(a,b,c))
#define DEV_register_removable_keyboard(a,b,c,d) (bx_devices.register_removable_keyboard(a,b,c,d))
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
#define DEV_cmos_enable_irq(a) (bx_devices.pluginCmosDevice->enable_irq(a))

///////// PIT macro
#define DEV_pit_enable_irq(a) (bx_devices.pluginPitDevice->enable_irq(a))

///////// keyboard macros
#define DEV_kbd_gen_scancode(key) (bx_devices.gen_scancode(key))
#define DEV_kbd_paste_bytes(bytes, count) (bx_devices.paste_bytes(bytes,count))
#define DEV_kbd_release_keys() (bx_devices.release_keys())
#define DEV_kbd_set_indicator(a,b,c) (bx_devices.kbd_set_indicator(a,b,c))

///////// mouse macro
#define DEV_mouse_motion(dx, dy, dz, bs, absxy) (bx_devices.mouse_motion(dx, dy, dz, bs, absxy))

///////// hard drive macros
#define DEV_hd_read_handler(a, b, c) \
    (bx_devices.pluginHardDrive->virt_read_handler(b, c))
#define DEV_hd_write_handler(a, b, c, d) \
    (bx_devices.pluginHardDrive->virt_write_handler(b, c, d))
#define DEV_hd_bmdma_read_sector(a,b,c) bx_devices.pluginHardDrive->bmdma_read_sector(a,b,c)
#define DEV_hd_bmdma_write_sector(a,b) bx_devices.pluginHardDrive->bmdma_write_sector(a,b)
#define DEV_hd_bmdma_complete(a) bx_devices.pluginHardDrive->bmdma_complete(a)

#define DEV_bulk_io_quantum_requested() (bx_devices.bulkIOQuantumsRequested)
#define DEV_bulk_io_quantum_transferred() (bx_devices.bulkIOQuantumsTransferred)
#define DEV_bulk_io_host_addr() (bx_devices.bulkIOHostAddr)

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
#define DEV_vga_redraw_area(left, top, right, bottom) \
  (bx_devices.pluginVgaDevice->vga_redraw_area(left, top, right, bottom))
#define DEV_vga_get_text_snapshot(rawsnap, height, width) \
  (bx_devices.pluginVgaDevice->get_text_snapshot(rawsnap, height, width))
#define DEV_vga_refresh(a) \
  (bx_devices.pluginVgaDevice->refresh_display(bx_devices.pluginVgaDevice,a))
#define DEV_vga_set_override(a,b) (bx_devices.pluginVgaDevice->set_override(a,b))

///////// PCI macros
#define DEV_register_pci_handlers(a,b,c,d) \
  (bx_devices.register_pci_handlers(a,b,c,d,0))
#define DEV_register_pci_handlers2(a,b,c,d,e) \
  (bx_devices.register_pci_handlers(a,b,c,d,e))
#define DEV_pci_get_slot_mapping() bx_devices.pci_get_slot_mapping()
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
#define DEV_agp_present() (bx_devices.is_agp_present())

///////// Speaker macros
#define DEV_speaker_beep_on(frequency) bx_devices.pluginSpeaker->beep_on(frequency)
#define DEV_speaker_beep_off() bx_devices.pluginSpeaker->beep_off()
#define DEV_speaker_set_line(a) bx_devices.pluginSpeaker->set_line(a)

///////// Memory macros
#define DEV_register_memory_handlers(param,rh,wh,b,e) \
    bx_devices.mem->registerMemoryHandlers(param,rh,wh,b,e)
#define DEV_unregister_memory_handlers(param,b,e) \
    bx_devices.mem->unregisterMemoryHandlers(param,b,e)
#define DEV_mem_set_memory_type(a,b,c) \
    bx_devices.mem->set_memory_type((memory_area_t)a,b,c)
#define DEV_mem_set_bios_write(a) bx_devices.mem->set_bios_write(a)
#define DEV_mem_set_bios_rom_access(a,b) bx_devices.mem->set_bios_rom_access(a,b)

///////// USB device macro
#define DEV_usb_init_device(a,b,c,d,p) bx_usbdev_ctl.init_device(a,b,(void**)c,d,p)

///////// Sound module macros
#define DEV_sound_get_waveout(a) (bx_soundmod_ctl.get_waveout(a))
#define DEV_sound_get_wavein() (bx_soundmod_ctl.get_wavein())
#define DEV_sound_get_midiout(a) (bx_soundmod_ctl.get_midiout(a))

///////// Networking module macro
#define DEV_net_init_module(a,b,c,d) \
  ((eth_pktmover_c*)bx_netmod_ctl.init_module(a,(void*)b,(void*)c,d))

///////// Gameport macro
#if BX_SUPPORT_GAMEPORT
#define DEV_gameport_set_enabled(a) bx_devices.pluginGameport->set_enabled(a)
#else
#define DEV_gameport_set_enabled(a) BX_ERROR(("gameport emulation not present"))
#endif


#if BX_HAVE_DLFCN_H
#include <dlfcn.h>
#endif

typedef Bit32u (*ioReadHandler_t)(void *, Bit32u, unsigned);
typedef void   (*ioWriteHandler_t)(void *, Bit32u, Bit32u, unsigned);

typedef struct _device_t
{
    const char   *name;
    plugin_t     *plugin;
    Bit16u       plugtype;

    class bx_devmodel_c *devmodel;  // BBD hack

    struct _device_t *next;
} device_t;


extern device_t *devices;

void plugin_startup(void);
void plugin_cleanup(void);

/* === Device Stuff === */
typedef void (*deviceInitMem_t)(BX_MEM_C *);
typedef void (*deviceInitDev_t)(void);
typedef void (*deviceReset_t)(unsigned);

BOCHSAPI void pluginRegisterDeviceDevmodel(plugin_t *plugin, Bit16u type, bx_devmodel_c *dev, const char *name);
BOCHSAPI void pluginUnregisterDeviceDevmodel(const char *name, Bit16u type);
BOCHSAPI bool pluginDevicePresent(const char *name);

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

/* === HRQ stuff === */
BOCHSAPI extern void    (*pluginSetHRQ)(unsigned val);
BOCHSAPI extern void    (*pluginSetHRQHackCallback)(void (*callback)(void));

void plugin_abort(plugin_t *plugin);

#if BX_PLUGINS
Bit8u bx_get_plugins_count(Bit16u type);
const char* bx_get_plugin_name(Bit16u type, Bit8u index);
Bit8u bx_get_plugin_flags(Bit16u type, Bit8u index);
#endif
bool bx_load_plugin(const char *name, Bit16u type);
bool bx_unload_plugin(const char *name, bool devflag);
extern void bx_unload_plugin_type(const char *name, Bit16u type);
extern void bx_init_plugins(void);
extern void bx_reset_plugins(unsigned);
extern void bx_unload_plugins(void);
extern void bx_plugins_register_state(void);
extern void bx_plugins_after_restore_state(void);

#if !BX_PLUGINS
extern plugin_t bx_builtin_plugins[];

Bit8u bx_get_plugins_count_np(Bit16u type);
const char* bx_get_plugin_name_np(Bit16u type, Bit8u index);
Bit8u bx_get_plugin_flags_np(Bit16u type, Bit8u index);
int bx_load_plugin_np(const char *name, Bit16u type);
int bx_unload_opt_plugin(const char *name, bool devflag);
#endif

// every plugin must define this, within the extern"C" block, so that
// a non-mangled function symbol is available in the shared library.
int plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode);

// still in extern "C"
#if BX_PLUGINS && defined(WIN32)

#define PLUGIN_ENTRY_FOR_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl lib##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_GUI_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl lib##mod##_gui_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_IMG_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl lib##mod##_img_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_NET_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl libeth_##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_SND_MODULE(mod) \
  extern "C" __declspec(dllexport) int __cdecl libsound##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)

#elif BX_PLUGINS

#define PLUGIN_ENTRY_FOR_MODULE(mod) \
  extern "C" int CDECL lib##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_GUI_MODULE(mod) \
  extern "C" int CDECL lib##mod##_gui_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_IMG_MODULE(mod) \
  extern "C" int CDECL lib##mod##_img_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_NET_MODULE(mod) \
  extern "C" int CDECL libeth_##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_SND_MODULE(mod) \
  extern "C" int CDECL libsound##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)

#else

#define PLUGIN_ENTRY_FOR_MODULE(mod) \
  int CDECL lib##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_GUI_MODULE(mod) \
  int CDECL lib##mod##_gui_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_IMG_MODULE(mod) \
  int CDECL lib##mod##_img_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_NET_MODULE(mod) \
  int CDECL libeth_##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)
#define PLUGIN_ENTRY_FOR_SND_MODULE(mod) \
  int CDECL libsound##mod##_plugin_entry(plugin_t *plugin, Bit16u type, Bit8u mode)

// device plugins
PLUGIN_ENTRY_FOR_MODULE(harddrv);
PLUGIN_ENTRY_FOR_MODULE(keyboard);
PLUGIN_ENTRY_FOR_MODULE(busmouse);
PLUGIN_ENTRY_FOR_MODULE(serial);
PLUGIN_ENTRY_FOR_MODULE(unmapped);
PLUGIN_ENTRY_FOR_MODULE(biosdev);
PLUGIN_ENTRY_FOR_MODULE(cmos);
PLUGIN_ENTRY_FOR_MODULE(dma);
PLUGIN_ENTRY_FOR_MODULE(pic);
PLUGIN_ENTRY_FOR_MODULE(pit);
PLUGIN_ENTRY_FOR_MODULE(vga);
PLUGIN_ENTRY_FOR_MODULE(svga_cirrus);
PLUGIN_ENTRY_FOR_MODULE(floppy);
PLUGIN_ENTRY_FOR_MODULE(parallel);
PLUGIN_ENTRY_FOR_MODULE(pci);
PLUGIN_ENTRY_FOR_MODULE(pci2isa);
PLUGIN_ENTRY_FOR_MODULE(pci_ide);
PLUGIN_ENTRY_FOR_MODULE(pcidev);
PLUGIN_ENTRY_FOR_MODULE(usb_uhci);
PLUGIN_ENTRY_FOR_MODULE(usb_ohci);
PLUGIN_ENTRY_FOR_MODULE(usb_ehci);
PLUGIN_ENTRY_FOR_MODULE(usb_xhci);
PLUGIN_ENTRY_FOR_MODULE(sb16);
PLUGIN_ENTRY_FOR_MODULE(es1370);
PLUGIN_ENTRY_FOR_MODULE(netmod);
PLUGIN_ENTRY_FOR_MODULE(ne2k);
PLUGIN_ENTRY_FOR_MODULE(pcipnic);
PLUGIN_ENTRY_FOR_MODULE(e1000);
PLUGIN_ENTRY_FOR_MODULE(extfpuirq);
PLUGIN_ENTRY_FOR_MODULE(gameport);
PLUGIN_ENTRY_FOR_MODULE(speaker);
PLUGIN_ENTRY_FOR_MODULE(acpi);
PLUGIN_ENTRY_FOR_MODULE(iodebug);
PLUGIN_ENTRY_FOR_MODULE(ioapic);
PLUGIN_ENTRY_FOR_MODULE(hpet);
PLUGIN_ENTRY_FOR_MODULE(voodoo);
// config interface plugins
PLUGIN_ENTRY_FOR_MODULE(textconfig);
PLUGIN_ENTRY_FOR_MODULE(win32config);
// gui plugins
PLUGIN_ENTRY_FOR_GUI_MODULE(amigaos);
PLUGIN_ENTRY_FOR_GUI_MODULE(carbon);
PLUGIN_ENTRY_FOR_GUI_MODULE(macintosh);
PLUGIN_ENTRY_FOR_GUI_MODULE(nogui);
PLUGIN_ENTRY_FOR_GUI_MODULE(rfb);
PLUGIN_ENTRY_FOR_GUI_MODULE(sdl);
PLUGIN_ENTRY_FOR_GUI_MODULE(sdl2);
PLUGIN_ENTRY_FOR_GUI_MODULE(term);
PLUGIN_ENTRY_FOR_GUI_MODULE(vncsrv);
PLUGIN_ENTRY_FOR_GUI_MODULE(win32);
PLUGIN_ENTRY_FOR_GUI_MODULE(wx);
PLUGIN_ENTRY_FOR_GUI_MODULE(x);
// sound driver plugins
PLUGIN_ENTRY_FOR_SND_MODULE(alsa);
PLUGIN_ENTRY_FOR_SND_MODULE(dummy);
PLUGIN_ENTRY_FOR_SND_MODULE(file);
PLUGIN_ENTRY_FOR_SND_MODULE(oss);
PLUGIN_ENTRY_FOR_SND_MODULE(osx);
PLUGIN_ENTRY_FOR_SND_MODULE(sdl);
PLUGIN_ENTRY_FOR_SND_MODULE(win);
// network driver plugins
PLUGIN_ENTRY_FOR_NET_MODULE(fbsd);
PLUGIN_ENTRY_FOR_NET_MODULE(linux);
PLUGIN_ENTRY_FOR_NET_MODULE(null);
PLUGIN_ENTRY_FOR_NET_MODULE(slirp);
PLUGIN_ENTRY_FOR_NET_MODULE(socket);
PLUGIN_ENTRY_FOR_NET_MODULE(tap);
PLUGIN_ENTRY_FOR_NET_MODULE(tuntap);
PLUGIN_ENTRY_FOR_NET_MODULE(vde);
PLUGIN_ENTRY_FOR_NET_MODULE(vnet);
PLUGIN_ENTRY_FOR_NET_MODULE(win32);
// USB device plugins
PLUGIN_ENTRY_FOR_MODULE(usb_floppy);
PLUGIN_ENTRY_FOR_MODULE(usb_hid);
PLUGIN_ENTRY_FOR_MODULE(usb_hub);
PLUGIN_ENTRY_FOR_MODULE(usb_msd);
PLUGIN_ENTRY_FOR_MODULE(usb_printer);
// disk image plugins
PLUGIN_ENTRY_FOR_IMG_MODULE(vmware3);
PLUGIN_ENTRY_FOR_IMG_MODULE(vmware4);
PLUGIN_ENTRY_FOR_IMG_MODULE(vbox);
PLUGIN_ENTRY_FOR_IMG_MODULE(vpc);
PLUGIN_ENTRY_FOR_IMG_MODULE(vvfat);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __PLUGIN_H */
