/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2020  The Bochs Project
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

#ifndef BX_PARAM_NAMES
#define BX_PARAM_NAMES

// define parameter path names.  These names give the location in the
// parameter tree where each can be found.  The names correspond to
// the old BXP_* enum values, which have been eliminated.
#define BXPN_SEL_CONFIG_INTERFACE        "general.config_interface"
#define BXPN_BOCHS_START                 "general.start_mode"
#define BXPN_BOCHS_BENCHMARK             "general.benchmark"
#define BXPN_DUMP_STATS                  "general.dumpstats"
#define BXPN_RESTORE_FLAG                "general.restore"
#define BXPN_RESTORE_PATH                "general.restore_path"
#define BXPN_DEBUG_RUNNING               "general.debug_running"
#define BXPN_PLUGIN_CTRL                 "general.plugin_ctrl"
#define BXPN_UNLOCK_IMAGES               "general.unlock_images"
#define BXPN_CPU_NPROCESSORS             "cpu.n_processors"
#define BXPN_CPU_NCORES                  "cpu.n_cores"
#define BXPN_CPU_NTHREADS                "cpu.n_threads"
#define BXPN_CPU_MODEL                   "cpu.model"
#define BXPN_IPS                         "cpu.ips"
#define BXPN_SMP_QUANTUM                 "cpu.quantum"
#define BXPN_RESET_ON_TRIPLE_FAULT       "cpu.reset_on_triple_fault"
#define BXPN_IGNORE_BAD_MSRS             "cpu.ignore_bad_msrs"
#define BXPN_CONFIGURABLE_MSRS_PATH      "cpu.msrs"
#define BXPN_CPUID_LIMIT_WINNT           "cpu.cpuid_limit_winnt"
#define BXPN_MWAIT_IS_NOP                "cpu.mwait_is_nop"
#define BXPN_VENDOR_STRING               "cpuid.vendor_string"
#define BXPN_BRAND_STRING                "cpuid.brand_string"
#define BXPN_CPUID_LEVEL                 "cpuid.level"
#define BXPN_CPUID_FAMILY                "cpuid.family"
#define BXPN_CPUID_MODEL                 "cpuid.model"
#define BXPN_CPUID_STEPPING              "cpuid.stepping"
#define BXPN_CPUID_MMX                   "cpuid.mmx"
#define BXPN_CPUID_SIMD                  "cpuid.simd"
#define BXPN_CPUID_SSE4A                 "cpuid.sse4a"
#define BXPN_CPUID_MISALIGNED_SSE        "cpuid.misaligned_sse"
#define BXPN_CPUID_AES                   "cpuid.aes"
#define BXPN_CPUID_SHA                   "cpuid.sha"
#define BXPN_CPUID_MOVBE                 "cpuid.movbe"
#define BXPN_CPUID_ADX                   "cpuid.adx"
#define BXPN_CPUID_SEP                   "cpuid.sep"
#define BXPN_CPUID_XSAVE                 "cpuid.xsave"
#define BXPN_CPUID_XSAVEOPT              "cpuid.xsaveopt"
#define BXPN_CPUID_SVM                   "cpuid.svm"
#define BXPN_CPUID_VMX                   "cpuid.vmx"
#define BXPN_CPUID_AVX_F16CVT            "cpuid.avx_f16c"
#define BXPN_CPUID_AVX_FMA               "cpuid.avx_fma"
#define BXPN_CPUID_BMI                   "cpuid.bmi"
#define BXPN_CPUID_XOP                   "cpuid.xop"
#define BXPN_CPUID_TBM                   "cpuid.tbm"
#define BXPN_CPUID_FMA4                  "cpuid.fma4"
#define BXPN_CPUID_APIC                  "cpuid.apic"
#define BXPN_CPUID_MWAIT                 "cpuid.mwait"
#define BXPN_CPUID_X86_64                "cpuid.x86_64"
#define BXPN_CPUID_1G_PAGES              "cpuid.1g_pages"
#define BXPN_CPUID_PCID                  "cpuid.pcid"
#define BXPN_CPUID_FSGSBASE              "cpuid.fsgsbase"
#define BXPN_CPUID_SMEP                  "cpuid.smep"
#define BXPN_CPUID_SMAP                  "cpuid.smap"
#define BXPN_MEM_SIZE                    "memory.standard.ram.size"
#define BXPN_HOST_MEM_SIZE               "memory.standard.ram.host_size"
#define BXPN_MEM_BLOCK_SIZE              "memory.standard.ram.block_size"
#define BXPN_ROMIMAGE                    "memory.standard.rom"
#define BXPN_ROM_PATH                    "memory.standard.rom.file"
#define BXPN_ROM_ADDRESS                 "memory.standard.rom.address"
#define BXPN_ROM_OPTIONS                 "memory.standard.rom.options"
#define BXPN_VGA_ROMIMAGE                "memory.standard.vgarom"
#define BXPN_VGA_ROM_PATH                "memory.standard.vgarom.file"
#define BXPN_OPTROM_BASE                 "memory.optrom"
#define BXPN_OPTRAM_BASE                 "memory.optram"
#define BXPN_CLOCK_SYNC                  "clock_cmos.clock_sync"
#define BXPN_CLOCK_TIME0                 "clock_cmos.time0"
#define BXPN_CLOCK_RTC_SYNC              "clock_cmos.rtc_sync"
#define BXPN_CMOSIMAGE_ENABLED           "clock_cmos.cmosimage.enabled"
#define BXPN_CMOSIMAGE_PATH              "clock_cmos.cmosimage.path"
#define BXPN_CMOSIMAGE_RTC_INIT          "clock_cmos.cmosimage.rtc_init"
#define BXPN_PCI_ENABLED                 "pci.enabled"
#define BXPN_PCI_CHIPSET                 "pci.chipset"
#define BXPN_PCI_ADV_OPTS                "pci.advopts"
#define BXPN_PCIDEV_VENDOR               "pci.pcidev.vendor"
#define BXPN_PCIDEV_DEVICE               "pci.pcidev.device"
#define BXPN_SEL_DISPLAY_LIBRARY         "display.display_library"
#define BXPN_DISPLAYLIB_OPTIONS          "display.displaylib_options"
#define BXPN_PRIVATE_COLORMAP            "display.private_colormap"
#define BXPN_FULLSCREEN                  "display.fullscreen"
#define BXPN_SCREENMODE                  "display.screenmode"
#define BXPN_VGA_EXTENSION               "display.vga_extension"
#define BXPN_VGA_UPDATE_FREQUENCY        "display.vga_update_frequency"
#define BXPN_VGA_REALTIME                "display.vga_realtime"
#define BXPN_DDC_MODE                    "display.ddc_mode"
#define BXPN_DDC_FILE                    "display.ddc_file"
#define BXPN_VOODOO                      "display.voodoo"
#define BXPN_KEYBOARD                    "keyboard_mouse.keyboard"
#define BXPN_KBD_TYPE                    "keyboard_mouse.keyboard.type"
#define BXPN_KBD_SERIAL_DELAY            "keyboard_mouse.keyboard.serial_delay"
#define BXPN_KBD_PASTE_DELAY             "keyboard_mouse.keyboard.paste_delay"
#define BXPN_KBD_USEMAPPING              "keyboard_mouse.keyboard.use_mapping"
#define BXPN_KBD_KEYMAP                  "keyboard_mouse.keyboard.keymap"
#define BXPN_USER_SHORTCUT               "keyboard_mouse.keyboard.user_shortcut"
#define BXPN_MOUSE                       "keyboard_mouse.mouse"
#define BXPN_MOUSE_TYPE                  "keyboard_mouse.mouse.type"
#define BXPN_MOUSE_ENABLED               "keyboard_mouse.mouse.enabled"
#define BXPN_MOUSE_TOGGLE                "keyboard_mouse.mouse.toggle"
#define BXPN_BOOTDRIVE1                  "boot_params.boot_drive1"
#define BXPN_BOOTDRIVE2                  "boot_params.boot_drive2"
#define BXPN_BOOTDRIVE3                  "boot_params.boot_drive3"
#define BXPN_FLOPPYSIGCHECK              "boot_params.floppy_sig_check"
#define BXPN_FLOPPYA                     "floppy.0"
#define BXPN_FLOPPYA_DEVTYPE             "floppy.0.devtype"
#define BXPN_FLOPPYA_PATH                "floppy.0.path"
#define BXPN_FLOPPYA_TYPE                "floppy.0.type"
#define BXPN_FLOPPYA_STATUS              "floppy.0.status"
#define BXPN_FLOPPYB                     "floppy.1"
#define BXPN_FLOPPYB_DEVTYPE             "floppy.1.devtype"
#define BXPN_FLOPPYB_PATH                "floppy.1.path"
#define BXPN_FLOPPYB_TYPE                "floppy.1.type"
#define BXPN_FLOPPYB_STATUS              "floppy.1.status"
#define BXPN_ATA0_RES                    "ata.0.resources"
#define BXPN_ATA1_RES                    "ata.1.resources"
#define BXPN_ATA2_RES                    "ata.2.resources"
#define BXPN_ATA3_RES                    "ata.3.resources"
#define BXPN_ATA0_ENABLED                "ata.0.resources.enabled"
#define BXPN_ATA1_ENABLED                "ata.1.resources.enabled"
#define BXPN_ATA0_MASTER                 "ata.0.master"
#define BXPN_ATA1_MASTER                 "ata.1.master"
#define BXPN_ATA2_MASTER                 "ata.2.master"
#define BXPN_ATA3_MASTER                 "ata.3.master"
#define BXPN_ATA0_SLAVE                  "ata.0.slave"
#define BXPN_ATA1_SLAVE                  "ata.1.slave"
#define BXPN_ATA2_SLAVE                  "ata.2.slave"
#define BXPN_ATA3_SLAVE                  "ata.3.slave"
#define BXPN_USB_UHCI                    "ports.usb.uhci"
#define BXPN_UHCI_ENABLED                "ports.usb.uhci.enabled"
#define BXPN_USB_OHCI                    "ports.usb.ohci"
#define BXPN_OHCI_ENABLED                "ports.usb.ohci.enabled"
#define BXPN_USB_EHCI                    "ports.usb.ehci"
#define BXPN_EHCI_ENABLED                "ports.usb.ehci.enabled"
#define BXPN_EHCI_COMPANION              "ports.usb.ehci.companion"
#define BXPN_USB_XHCI                    "ports.usb.xhci"
#define BXPN_XHCI_ENABLED                "ports.usb.xhci.enabled"
#define BXPN_XHCI_MODEL                  "ports.usb.xhci.model"
#define BXPN_XHCI_N_PORTS                "ports.usb.xhci.n_ports"
#define BXPN_NE2K                        "network.ne2k"
#define BXPN_PNIC                        "network.pcipnic"
#define BXPN_E1000                       "network.e1000"
#define BXPN_SOUNDLOW                    "sound.lowlevel"
#define BXPN_SOUND_WAVEOUT_DRV           "sound.lowlevel.waveoutdrv"
#define BXPN_SOUND_WAVEOUT               "sound.lowlevel.waveout"
#define BXPN_SOUND_WAVEIN_DRV            "sound.lowlevel.waveindrv"
#define BXPN_SOUND_WAVEIN                "sound.lowlevel.wavein"
#define BXPN_SOUND_MIDIOUT_DRV           "sound.lowlevel.midioutdrv"
#define BXPN_SOUND_MIDIOUT               "sound.lowlevel.midiout"
#define BXPN_SOUND_SPEAKER               "sound.speaker"
#define BXPN_SOUND_SB16                  "sound.sb16"
#define BXPN_SOUND_ES1370                "sound.es1370"
#define BXPN_PORT_E9_HACK_ROOT           "misc.port_e9_hack"
#define BXPN_PORT_E9_HACK                "misc.port_e9_hack.enabled"
#define BXPN_PORT_E9_HACK_ALL_RINGS      "misc.port_e9_hack.all_rings"
#define BXPN_IODEBUG_ALL_RINGS           "misc.iodebug_all_rings"
#define BXPN_GDBSTUB                     "misc.gdbstub"
#define BXPN_LOG_FILENAME                "log.filename"
#define BXPN_LOG_PREFIX                  "log.prefix"
#define BXPN_DEBUGGER_LOG_FILENAME       "log.debugger_filename"
#define BXPN_MENU_DISK                   "menu.disk"
#define BXPN_MENU_DISK_WIN32             "menu.disk_win32"
#define BXPN_MENU_RUNTIME_CDROM          "menu.runtime.cdrom"
#define BXPN_MENU_RUNTIME_USB            "menu.runtime.usb"
#define BXPN_MENU_RUNTIME_MISC           "menu.runtime.misc"

#endif
