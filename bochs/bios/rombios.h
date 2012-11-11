/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2006 Volker Ruppert
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

/* define it to include QEMU specific code */
//#define BX_QEMU

#ifndef LEGACY
#  define BX_ROMBIOS32     1
#else
#  define BX_ROMBIOS32     0
#endif

#define DEBUG_ROMBIOS    0
#define DEBUG_ATA          0
#define DEBUG_INT13_HD     0
#define DEBUG_INT13_CD     0
#define DEBUG_INT13_ET     0
#define DEBUG_INT13_FL     0
#define DEBUG_INT15        0
#define DEBUG_INT16        0
#define DEBUG_INT1A        0
#define DEBUG_INT74        0
#define DEBUG_APM          0

#define PANIC_PORT  0x400
#define PANIC_PORT2 0x401
#define INFO_PORT   0x402
#define DEBUG_PORT  0x403

#define BIOS_PRINTF_HALT     1
#define BIOS_PRINTF_SCREEN   2
#define BIOS_PRINTF_INFO     4
#define BIOS_PRINTF_DEBUG    8
#define BIOS_PRINTF_ALL      (BIOS_PRINTF_SCREEN | BIOS_PRINTF_INFO)
#define BIOS_PRINTF_DEBHALT  (BIOS_PRINTF_SCREEN | BIOS_PRINTF_INFO | BIOS_PRINTF_HALT)

#define printf(format, p...)  bios_printf(BIOS_PRINTF_SCREEN, format, ##p)

// Defines the output macros.
// BX_DEBUG goes to INFO port until we can easily choose debug info on a
// per-device basis. Debug info are sent only in debug mode
#if DEBUG_ROMBIOS
#  define BX_DEBUG(format, p...)  bios_printf(BIOS_PRINTF_INFO, format, ##p)
#else
#  define BX_DEBUG(format, p...)
#endif
#define BX_INFO(format, p...)   bios_printf(BIOS_PRINTF_INFO, format, ##p)
#define BX_PANIC(format, p...)  bios_printf(BIOS_PRINTF_DEBHALT, format, ##p)

/* put the MP float table and ACPI RSDP in EBDA and the MP and ACPI tables in
   high memory. Linux kernels < 2.6.30 might not work with this configuration */
//#define BX_USE_EBDA_TABLES

#define ACPI_DATA_SIZE    0x00010000L
#define MPTABLE_MAX_SIZE  0x00002000
#define PM_IO_BASE        0xb000
#define SMB_IO_BASE       0xb100
#define SMP_MSR_ADDR      0x0510

  // Define the application NAME
#if defined(BX_QEMU)
#  define BX_APPNAME "QEMU"
#  define BX_APPVENDOR "QEMU"
#else
#  define BX_APPNAME "Bochs"
#  define BX_APPVENDOR "The Bochs Project"
#endif

#define E820_RAM          1
#define E820_RESERVED     2
#define E820_ACPI         3
#define E820_NVS          4
#define E820_UNUSABLE     5

#define BX_CPU           3
#define BX_USE_PS2_MOUSE 1
#define BX_CALL_INT15_4F 1
#define BX_USE_EBDA      1
#define BX_SUPPORT_FLOPPY 1
#define BX_FLOPPY_ON_CNT 37   /* 2 seconds */
#define BX_PCIBIOS       1
#define BX_APM           1
#define BX_PNPBIOS       1
/* define it if the (emulated) hardware supports SMM mode */
#define BX_USE_SMM

#define BX_USE_ATADRV    1
#define BX_ELTORITO_BOOT 1

#define BX_MAX_ATA_INTERFACES   4
#define BX_MAX_ATA_DEVICES      (BX_MAX_ATA_INTERFACES*2)

#define BX_VIRTUAL_PORTS 1 /* normal output to Bochs ports */
#define BX_DEBUG_SERIAL  0 /* output to COM1 */

   /* model byte 0xFC = AT */
#define SYS_MODEL_ID     0xFC
#define SYS_SUBMODEL_ID  0x00
#define BIOS_REVISION    1
#define BIOS_CONFIG_TABLE 0xe6f5

#ifndef BIOS_BUILD_DATE
#  define BIOS_BUILD_DATE "06/23/99"
#endif

  // 1K of base memory used for Extended Bios Data Area (EBDA)
  // EBDA is used for PS/2 mouse support, and IDE BIOS, etc.
#define EBDA_SEG           0x9FC0
#define EBDA_SIZE          1              // In KiB
#define BASE_MEM_IN_K   (640 - EBDA_SIZE)

/* IPL_SIZE bytes at 0x9ff00 are used for the IPL boot table. */
#define IPL_SEG              0x9ff0
#define IPL_TABLE_OFFSET     0x0000
#define IPL_TABLE_ENTRIES    8
#define IPL_COUNT_OFFSET     0x0080  /* u16: number of valid table entries */
#define IPL_SEQUENCE_OFFSET  0x0082  /* u16: next boot device */
#define IPL_BOOTFIRST_OFFSET 0x0084  /* u16: user selected device */
#define IPL_SIZE             0x86
#define IPL_TYPE_FLOPPY      0x01
#define IPL_TYPE_HARDDISK    0x02
#define IPL_TYPE_CDROM       0x03
#define IPL_TYPE_BEV         0x80

/* Ports */
#define PORT_DMA_ADDR_2        0x0004
#define PORT_DMA_CNT_2         0x0005
#define PORT_DMA1_MASK_REG     0x000a
#define PORT_DMA1_MODE_REG     0x000b
#define PORT_DMA1_CLEAR_FF_REG 0x000c
#define PORT_DMA1_MASTER_CLEAR 0x000d
#define PORT_PIC1_CMD          0x0020
#define PORT_PIC1_DATA         0x0021
#define PORT_PIT_COUNTER0      0x0040
#define PORT_PIT_MODE          0x0043
#define PORT_PS2_DATA          0x0060
#define PORT_PS2_CTRLB         0x0061
#define PORT_PS2_STATUS        0x0064
#define PORT_CMOS_INDEX        0x0070
#define PORT_CMOS_DATA         0x0071
#define PORT_DIAG              0x0080
#define PORT_DMA_PAGE_2        0x0081
#define PORT_A20               0x0092
#define PORT_PIC2_CMD          0x00a0
#define PORT_PIC2_DATA         0x00a1
#define PORT_DMA2_MASK_REG     0x00d4
#define PORT_DMA2_MODE_REG     0x00d6
#define PORT_DMA2_MASTER_CLEAR 0x00da
#define PORT_ATA2_CMD_BASE     0x0170
#define PORT_ATA1_CMD_BASE     0x01f0
#define PORT_FD_DOR            0x03f2
#define PORT_FD_STATUS         0x03f4
#define PORT_FD_DATA           0x03f5

#define CPUID_MSR (1 << 5)
#define CPUID_APIC (1 << 9)
#define CPUID_MTRR (1 << 12)

#define APIC_BASE    ((uint8_t *)0xfee00000)
#define APIC_ICR_LOW 0x300
#define APIC_SVR     0x0F0
#define APIC_ID      0x020
#define APIC_LVT3    0x370

#define APIC_ENABLED 0x0100

#define AP_BOOT_ADDR 0x9f000

#define SMI_CMD_IO_ADDR   0xb2

#define BIOS_TMP_STORAGE  0x00030000 /* 64 KB used to copy the BIOS to shadow RAM */

#define MSR_MTRRcap                     0x000000fe
#define MSR_MTRRfix64K_00000            0x00000250
#define MSR_MTRRfix16K_80000            0x00000258
#define MSR_MTRRfix16K_A0000            0x00000259
#define MSR_MTRRfix4K_C0000             0x00000268
#define MSR_MTRRfix4K_C8000             0x00000269
#define MSR_MTRRfix4K_D0000             0x0000026a
#define MSR_MTRRfix4K_D8000             0x0000026b
#define MSR_MTRRfix4K_E0000             0x0000026c
#define MSR_MTRRfix4K_E8000             0x0000026d
#define MSR_MTRRfix4K_F0000             0x0000026e
#define MSR_MTRRfix4K_F8000             0x0000026f
#define MSR_MTRRdefType                 0x000002ff

#define MTRRphysBase_MSR(reg) (0x200 + 2 * (reg))
#define MTRRphysMask_MSR(reg) (0x200 + 2 * (reg) + 1)

#define MTRR_MEMTYPE_UC 0
#define MTRR_MEMTYPE_WC 1
#define MTRR_MEMTYPE_WT 4
#define MTRR_MEMTYPE_WP 5
#define MTRR_MEMTYPE_WB 6

#define QEMU_CFG_CTL_PORT 0x510
#define QEMU_CFG_DATA_PORT 0x511
#define QEMU_CFG_SIGNATURE  0x00
#define QEMU_CFG_ID         0x01
#define QEMU_CFG_UUID       0x02

#define PCI_ADDRESS_SPACE_MEM		0x00
#define PCI_ADDRESS_SPACE_IO		0x01
#define PCI_ADDRESS_SPACE_MEM_PREFETCH	0x08

#define PCI_ROM_SLOT 6
#define PCI_NUM_REGIONS 7

#define PCI_DEVICES_MAX 64

#define PCI_CLASS_STORAGE_IDE	0x0101
#define PCI_CLASS_DISPLAY_VGA	0x0300
#define PCI_CLASS_SYSTEM_PIC	0x0800

#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */
#define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI_MIN_GNT		0x3e	/* 8 bits */
#define PCI_MAX_LAT		0x3f	/* 8 bits */

#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */

#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define  PCI_ROM_ADDRESS_ENABLE	0x01

#define PCI_VENDOR_ID_INTEL             0x8086
#define PCI_DEVICE_ID_INTEL_82437       0x0122
#define PCI_DEVICE_ID_INTEL_82441       0x1237
#define PCI_DEVICE_ID_INTEL_82371FB_0   0x122e
#define PCI_DEVICE_ID_INTEL_82371FB_1   0x1230
#define PCI_DEVICE_ID_INTEL_82371SB_0   0x7000
#define PCI_DEVICE_ID_INTEL_82371SB_1   0x7010
#define PCI_DEVICE_ID_INTEL_82371AB_0   0x7110
#define PCI_DEVICE_ID_INTEL_82371AB     0x7111
#define PCI_DEVICE_ID_INTEL_82371AB_3   0x7113

#define PCI_VENDOR_ID_IBM               0x1014
#define PCI_VENDOR_ID_APPLE             0x106b
