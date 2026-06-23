/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025  The Bochs Project
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

#ifndef BX_IODEV_FW_CFG_H
#define BX_IODEV_FW_CFG_H

// fw_cfg I/O port base address (matches QEMU)
#define FW_CFG_IO_BASE     0x510

// fw_cfg selector keys (matching QEMU's implementation)
#define FW_CFG_SIGNATURE    0x00
#define FW_CFG_ID           0x01
#define FW_CFG_UUID         0x02
#define FW_CFG_RAM_SIZE     0x03
#define FW_CFG_NOGRAPHIC    0x04
#define FW_CFG_NB_CPUS      0x05
#define FW_CFG_MACHINE_ID   0x06
#define FW_CFG_KERNEL_ADDR  0x07
#define FW_CFG_KERNEL_SIZE  0x08
#define FW_CFG_KERNEL_CMDLINE 0x09
#define FW_CFG_INITRD_ADDR  0x0a
#define FW_CFG_INITRD_SIZE  0x0b
#define FW_CFG_BOOT_DEVICE  0x0c
#define FW_CFG_NUMA         0x0d
#define FW_CFG_BOOT_MENU    0x0e
#define FW_CFG_MAX_CPUS     0x0f
#define FW_CFG_KERNEL_ENTRY 0x10
#define FW_CFG_KERNEL_DATA  0x11
#define FW_CFG_INITRD_DATA  0x12
#define FW_CFG_CMDLINE_ADDR 0x13
#define FW_CFG_CMDLINE_SIZE 0x14
#define FW_CFG_CMDLINE_DATA 0x15
#define FW_CFG_SETUP_ADDR   0x16
#define FW_CFG_SETUP_SIZE   0x17
#define FW_CFG_SETUP_DATA   0x18
#define FW_CFG_FILE_DIR     0x19

#define FW_CFG_FILE_FIRST   0x20
#define FW_CFG_FILE_SLOTS   0x10
#define FW_CFG_MAX_FILE_PATH 56
#define FW_CFG_FILE_DIR     0x19

#define FW_CFG_FILE_FIRST   0x20

#define FW_CFG_WRITE_CHANNEL 0x4000
#define FW_CFG_ARCH_LOCAL    0x8000

// x86-specific fw_cfg entries (ARCH_LOCAL + offset)
#define FW_CFG_ACPI_TABLES    (FW_CFG_ARCH_LOCAL + 0)
#define FW_CFG_SMBIOS_ENTRIES (FW_CFG_ARCH_LOCAL + 1)
#define FW_CFG_IRQ0_OVERRIDE  (FW_CFG_ARCH_LOCAL + 2)
#define FW_CFG_E820_TABLE     (FW_CFG_ARCH_LOCAL + 3)
#define FW_CFG_HPET           (FW_CFG_ARCH_LOCAL + 4)

// E820 memory types (matching QEMU)
#define E820_RAM        1
#define E820_RESERVED   2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE   5

// E820 entry structure (matching QEMU, 20 bytes)
#pragma pack(push, 1)
struct e820_entry {
  Bit64u address;
  Bit64u length;
  Bit32u type;
};

// HPET firmware config structures (matching QEMU)
struct hpet_fw_entry {
  Bit32u event_timer_block_id;
  Bit64u address;
  Bit16u min_tick;
  Bit8u page_prot;
};

struct hpet_fw_config {
  Bit8u count;
  struct hpet_fw_entry hpet[8];
};

// fw_cfg file directory entry (64 bytes: 4+2+2+56)
struct fw_cfg_file {
  Bit32u size;        // big-endian
  Bit16u select;      // big-endian
  Bit16u reserved;
  char name[FW_CFG_MAX_FILE_PATH];
};

// fw_cfg file directory (count + array of file entries)
struct fw_cfg_files {
  Bit32u count;       // big-endian, number of files
  struct fw_cfg_file f[FW_CFG_FILE_SLOTS];
};
#pragma pack(pop)
#define FW_CFG_ENTRY_MASK    (~(FW_CFG_WRITE_CHANNEL | FW_CFG_ARCH_LOCAL))

#define FW_CFG_INVALID      0xffff

// FW_CFG_ID bits
#define FW_CFG_VERSION      0x01
#define FW_CFG_VERSION_DMA  0x02

// Maximum entries (increased to support file slots starting at 0x20)
#define FW_CFG_MAX_ENTRY    0x4000

// DMA signature
#define FW_CFG_DMA_SIGNATURE 0x51454d5520434647ULL /* "QEMU CFG" */

// DMA control bits
#define FW_CFG_DMA_CTL_ERROR   0x01
#define FW_CFG_DMA_CTL_READ    0x02
#define FW_CFG_DMA_CTL_SKIP    0x04
#define FW_CFG_DMA_CTL_SELECT  0x08
#define FW_CFG_DMA_CTL_WRITE   0x10

#if BX_USE_FW_CFG_SMF
#  define BX_FW_CFG_SMF  static
#  define BX_FW_CFG_THIS theFwCfgDevice->
#  define BX_FW_CFG_THIS_PTR theFwCfgDevice
#else
#  define BX_FW_CFG_SMF
#  define BX_FW_CFG_THIS this->
#  define BX_FW_CFG_THIS_PTR this
#endif

typedef struct {
  Bit32u len;
  Bit8u *data;
} bx_fw_cfg_entry_t;

class bx_fw_cfg_c : public bx_devmodel_c {
public:
  bx_fw_cfg_c();
  virtual ~bx_fw_cfg_c();
  virtual void init(void);
  virtual void reset(unsigned type);

private:
  bx_fw_cfg_entry_t entries[FW_CFG_MAX_ENTRY];
  Bit16u cur_entry;
  Bit32u cur_offset;
  Bit64u dma_addr;
  
  // File directory support
  fw_cfg_files *file_dir;
  int file_count;
  
  void add_bytes(Bit16u key, Bit8u *data, Bit32u len);
  void add_file(const char *filename, Bit8u *data, Bit32u len);
  void add_i16(Bit16u key, Bit16u value);
  void add_i32(Bit16u key, Bit32u value);
  void add_i64(Bit16u key, Bit64u value);
  void add_i8(Bit16u key, Bit8u value);
  void process_dma(Bit64u dma_addr);
  void generate_e820_map(void);
  void generate_hpet_config(void);
  void generate_acpi_tables(void);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_FW_CFG_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif // BX_IODEV_FW_CFG_H