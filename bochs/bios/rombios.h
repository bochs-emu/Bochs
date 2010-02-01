/////////////////////////////////////////////////////////////////////////
// $Id: rombios.h,v 1.14 2010-02-01 21:05:42 sshwarts Exp $
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
