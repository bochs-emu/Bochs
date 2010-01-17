/////////////////////////////////////////////////////////////////////////
// $Id: rombios.h,v 1.12 2010-01-17 17:35:04 sshwarts Exp $
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
#else
#  define BX_APPNAME "Bochs"
#endif

#define E820_RAM          1
#define E820_RESERVED     2
#define E820_ACPI         3
#define E820_NVS          4
#define E820_UNUSABLE     5
