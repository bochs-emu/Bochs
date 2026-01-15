/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
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

// QEMU fw_cfg device emulation for UEFI firmware support

#define BX_PLUGGABLE

#define NEED_CPU_REG_SHORTCUTS 1
#include "iodev.h"
#include "fw_cfg.h"
#include "acpi_tables.h"

#define LOG_THIS theFwCfgDevice->

bx_fw_cfg_c *theFwCfgDevice = NULL;

PLUGIN_ENTRY_FOR_MODULE(fw_cfg)
{
  if (mode == PLUGIN_INIT) {
    theFwCfgDevice = new bx_fw_cfg_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theFwCfgDevice, BX_PLUGIN_FW_CFG);
  } else if (mode == PLUGIN_FINI) {
    delete theFwCfgDevice;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_CORE;
  }
  return(0); // Success
}

// Constructor
bx_fw_cfg_c::bx_fw_cfg_c()
{
  put("FW_CFG", "FWCFG");
  file_dir = NULL;
  file_count = 0;
}

// Destructor
bx_fw_cfg_c::~bx_fw_cfg_c()
{
  // Free allocated entry data
  for (int i = 0; i < FW_CFG_MAX_ENTRY; i++) {
    if (entries[i].data != NULL) {
      delete [] entries[i].data;
      entries[i].data = NULL;
    }
  }
  if (file_dir != NULL) {
    delete file_dir;
    file_dir = NULL;
  }
  BX_DEBUG(("Exit"));
}

void bx_fw_cfg_c::init(void)
{
  BX_DEBUG(("Init $Id$"));

  // Initialize all entries
  for (int i = 0; i < FW_CFG_MAX_ENTRY; i++) {
    entries[i].data = NULL;
    entries[i].len = 0;
  }

  // Register I/O ports (0x510 = selector, 0x511 = data)
  DEV_register_ioread_handler(this, read_handler, FW_CFG_IO_BASE, "fw_cfg selector", 1);
  DEV_register_iowrite_handler(this, write_handler, FW_CFG_IO_BASE, "fw_cfg selector", 3);
  
  DEV_register_ioread_handler(this, read_handler, FW_CFG_IO_BASE + 1, "fw_cfg data", 1);
  DEV_register_iowrite_handler(this, write_handler, FW_CFG_IO_BASE + 1, "fw_cfg data", 3);

  // Register DMA ports (0x514-0x51B = 8 bytes for 64-bit DMA address)
  // OVMF writes high 32-bits to 0x518, low 32-bits to 0x514
  for (int i = 0; i < 8; i++) {
    DEV_register_ioread_handler(this, read_handler, FW_CFG_IO_BASE + 4 + i, "fw_cfg dma", 7);
    DEV_register_iowrite_handler(this, write_handler, FW_CFG_IO_BASE + 4 + i, "fw_cfg dma", 7);
  }

  // Add signature "QEMU"
  Bit8u *sig = new Bit8u[4];
  sig[0] = 'Q';
  sig[1] = 'E';
  sig[2] = 'M';
  sig[3] = 'U';
  add_bytes(FW_CFG_SIGNATURE, sig, 4);

  // Add ID (version with DMA support)
  add_i32(FW_CFG_ID, FW_CFG_VERSION | FW_CFG_VERSION_DMA);

  // Add RAM size (get_memory_len returns bytes, not MB!)
  Bit64u ram_size = BX_MEM_THIS get_memory_len();
  add_i64(FW_CFG_RAM_SIZE, ram_size);

  // Add CPU count
  add_i16(FW_CFG_NB_CPUS, (Bit16u)BX_SMP_PROCESSORS);
  add_i16(FW_CFG_MAX_CPUS, (Bit16u)BX_SMP_PROCESSORS);

  // Add x86-specific entries
  // IRQ0_OVERRIDE: tell OVMF that IRQ0 is connected to IOAPIC pin 2
  add_i32(FW_CFG_IRQ0_OVERRIDE, 1);

  // NOGRAPHIC: indicate if graphics are enabled (0 = graphics enabled)
  add_i16(FW_CFG_NOGRAPHIC, 0);

  // BOOT_MENU: boot menu timeout (0 = disabled)
  add_i16(FW_CFG_BOOT_MENU, 0);

  // MACHINE_ID: 0 for PC
  add_i32(FW_CFG_MACHINE_ID, 0);

  // BOOT_DEVICE: 0 = default
  add_i16(FW_CFG_BOOT_DEVICE, 0);

  // NUMA: no NUMA configuration (empty)
  add_i64(FW_CFG_NUMA, 0);

  // Linux kernel boot entries (OVMF may probe these even without Linux kernel)
  // These return 0 to indicate no kernel is loaded
  add_i32(FW_CFG_KERNEL_ADDR, 0);
  add_i32(FW_CFG_KERNEL_SIZE, 0);
  add_i32(FW_CFG_KERNEL_ENTRY, 0);
  add_i32(FW_CFG_INITRD_ADDR, 0);
  add_i32(FW_CFG_INITRD_SIZE, 0);
  add_i32(FW_CFG_CMDLINE_ADDR, 0);
  add_i32(FW_CFG_CMDLINE_SIZE, 0);
  add_i32(FW_CFG_SETUP_ADDR, 0);
  add_i32(FW_CFG_SETUP_SIZE, 0);   // Entry 0x17 that was causing error

  // Initialize file directory before adding files
  file_dir = new fw_cfg_files;
  memset(file_dir, 0, sizeof(fw_cfg_files));
  file_count = 0;
  
  // Add file directory entry (will be updated as files are added)
  Bit32u dir_size = sizeof(Bit32u) + FW_CFG_FILE_SLOTS * sizeof(fw_cfg_file);
  add_bytes(FW_CFG_FILE_DIR, (Bit8u*)file_dir, dir_size);

  // Generate and add E820 memory map as FILE (critical for OVMF)
  generate_e820_map();

  // Generate and add HPET configuration
  generate_hpet_config();

  // Generate and add ACPI tables (critical for OVMF/Windows)
  generate_acpi_tables();

  BX_INFO(("fw_cfg device initialized (ports 0x510-0x511, DMA 0x514)"));
}

void bx_fw_cfg_c::reset(unsigned type)
{
  cur_entry = FW_CFG_INVALID;
  cur_offset = 0;
  dma_addr = 0;
}

void bx_fw_cfg_c::add_bytes(Bit16u key, Bit8u *data, Bit32u len)
{
  // Mask the key to get the actual array index (strip control bits)
  Bit16u index = key & (FW_CFG_MAX_ENTRY - 1);
  
  if (index >= FW_CFG_MAX_ENTRY) {
    BX_ERROR(("fw_cfg: key 0x%04x (index 0x%04x) out of range", key, index));
    return;
  }

  // Free existing data if any
  if (entries[index].data != NULL) {
    delete [] entries[index].data;
  }

  entries[index].data = data;
  entries[index].len = len;
  
  BX_DEBUG(("fw_cfg: added entry 0x%04x (index 0x%04x), len=%u", key, index, len));
}

void bx_fw_cfg_c::add_i8(Bit16u key, Bit8u value)
{
  Bit8u *data = new Bit8u[1];
  data[0] = value;
  add_bytes(key, data, 1);
}

void bx_fw_cfg_c::add_i16(Bit16u key, Bit16u value)
{
  Bit8u *data = new Bit8u[2];
  data[0] = value & 0xff;
  data[1] = (value >> 8) & 0xff;
  add_bytes(key, data, 2);
}

void bx_fw_cfg_c::add_i32(Bit16u key, Bit32u value)
{
  Bit8u *data = new Bit8u[4];
  data[0] = value & 0xff;
  data[1] = (value >> 8) & 0xff;
  data[2] = (value >> 16) & 0xff;
  data[3] = (value >> 24) & 0xff;
  add_bytes(key, data, 4);
}

void bx_fw_cfg_c::add_i64(Bit16u key, Bit64u value)
{
  Bit8u *data = new Bit8u[8];
  data[0] = value & 0xff;
  data[1] = (value >> 8) & 0xff;
  data[2] = (value >> 16) & 0xff;
  data[3] = (value >> 24) & 0xff;
  data[4] = (value >> 32) & 0xff;
  data[5] = (value >> 40) & 0xff;
  data[6] = (value >> 48) & 0xff;
  data[7] = (value >> 56) & 0xff;
  add_bytes(key, data, 8);
}

// Add a file to the fw_cfg file directory
void bx_fw_cfg_c::add_file(const char *filename, Bit8u *data, Bit32u len)
{
  if (file_count >= FW_CFG_FILE_SLOTS) {
    BX_ERROR(("fw_cfg: file directory full, cannot add '%s'", filename));
    delete[] data;
    return;
  }
  
  // Allocate entry at FW_CFG_FILE_FIRST + file_count
  Bit16u file_index = FW_CFG_FILE_FIRST + file_count;
  add_bytes(file_index, data, len);
  
  // Add file entry to directory (big-endian)
  fw_cfg_file *f = &file_dir->f[file_count];
  f->size = (len >> 24) | ((len >> 8) & 0xff00) | ((len << 8) & 0xff0000) | (len << 24);  // CPU to BE32
  f->select = ((file_index >> 8) & 0xff) | ((file_index << 8) & 0xff00);  // CPU to BE16
  f->reserved = 0;
  strncpy(f->name, filename, FW_CFG_MAX_FILE_PATH - 1);
  f->name[FW_CFG_MAX_FILE_PATH - 1] = '\0';
  
  file_count++;
  
  // Update directory count (big-endian)
  file_dir->count = (file_count >> 24) | ((file_count >> 8) & 0xff00) | 
                    ((file_count << 8) & 0xff0000) | (file_count << 24);
  
  BX_INFO(("fw_cfg: added file '%s' at index 0x%04x (%u bytes)", filename, file_index, len));
}

void bx_fw_cfg_c::process_dma(Bit64u dma_addr)
{
  // Read the DMA descriptor from guest memory (big-endian format)
  // Descriptor structure: control (4 bytes), length (4 bytes), address (8 bytes)
  Bit8u desc[16];
  
  BX_DEBUG(("fw_cfg DMA: reading descriptor from guest addr 0x" FMT_PHY_ADDRX,
            (Bit64u)dma_addr));
  
  BX_MEM_THIS dmaReadPhysicalPage(dma_addr, 16, desc);
  
  BX_DEBUG(("fw_cfg DMA: raw bytes: %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x",
           desc[0], desc[1], desc[2], desc[3], desc[4], desc[5], desc[6], desc[7],
           desc[8], desc[9], desc[10], desc[11], desc[12], desc[13], desc[14], desc[15]));
  
  // Parse descriptor (big-endian)
  Bit32u control = (desc[0] << 24) | (desc[1] << 16) | (desc[2] << 8) | desc[3];
  Bit32u length = (desc[4] << 24) | (desc[5] << 16) | (desc[6] << 8) | desc[7];
  Bit64u address = ((Bit64u)desc[8] << 56) | ((Bit64u)desc[9] << 48) |
                   ((Bit64u)desc[10] << 40) | ((Bit64u)desc[11] << 32) |
                   ((Bit64u)desc[12] << 24) | ((Bit64u)desc[13] << 16) |
                   ((Bit64u)desc[14] << 8) | ((Bit64u)desc[15]);
  
  BX_DEBUG(("fw_cfg DMA: control=0x%08x, length=%u, address=0x" FMT_PHY_ADDRX ", cur_entry=0x%04x",
            control, length, (Bit64u)address, cur_entry));
  
  // Handle SELECT operation
  if (control & FW_CFG_DMA_CTL_SELECT) {
    Bit16u key = (Bit16u)(control >> 16);
    cur_entry = key;
    cur_offset = 0;
    BX_DEBUG(("fw_cfg DMA: selected entry 0x%04x", key));
  }
  
  // Handle READ operation
  if (control & FW_CFG_DMA_CTL_READ) {
    Bit16u key = cur_entry & FW_CFG_ENTRY_MASK;
    
    if (key < FW_CFG_MAX_ENTRY && entries[key].data != NULL) {
      Bit32u to_read = length;
      if (cur_offset + to_read > entries[key].len) {
        to_read = entries[key].len - cur_offset;
      }
      
      // Write data to guest memory in page-sized chunks
      // (dmaWritePhysicalPage doesn't support cross-page accesses)
      Bit32u bytes_written = 0;
      Bit8u* src_ptr = &entries[key].data[cur_offset];
      Bit64u dst_addr = address;
      
      while (bytes_written < to_read) {
        // Calculate how many bytes we can write without crossing a page boundary
        Bit32u page_offset = dst_addr & 0xFFF;  // Offset within current page
        Bit32u bytes_to_page_end = 0x1000 - page_offset;  // Bytes until page end
        Bit32u chunk_size = to_read - bytes_written;
        if (chunk_size > bytes_to_page_end) {
          chunk_size = bytes_to_page_end;
        }
        
        BX_MEM_THIS dmaWritePhysicalPage(dst_addr, chunk_size, src_ptr);
        
        bytes_written += chunk_size;
        src_ptr += chunk_size;
        dst_addr += chunk_size;
      }
      
      cur_offset += to_read;
      BX_DEBUG(("fw_cfg DMA: read %u bytes from entry 0x%04x to 0x" FMT_PHY_ADDRX,
            to_read, key, (Bit64u)address));
      
      // Clear the READ bit to indicate completion (keep ERROR if set)
      control &= ~FW_CFG_DMA_CTL_READ;
    } else {
      // Set error bit
      control |= FW_CFG_DMA_CTL_ERROR;
      // Always clear the READ bit to indicate completion
      control &= ~FW_CFG_DMA_CTL_READ;
      BX_ERROR(("fw_cfg DMA: invalid entry 0x%04x", key));
    }
  }
  
  // Handle SKIP operation
  if (control & FW_CFG_DMA_CTL_SKIP) {
    cur_offset += length;
    control &= ~FW_CFG_DMA_CTL_SKIP;  // Clear SKIP bit to indicate completion
    BX_DEBUG(("fw_cfg DMA: skipped %u bytes", length));
  }
  
  // Clear SELECT bit after handling
  control &= ~FW_CFG_DMA_CTL_SELECT;
  
  // Write back control word (should be 0 on success, or ERROR on failure)
  // Format is big-endian
  BX_DEBUG(("fw_cfg DMA: completion, writing back control=0x%08x", control));
  desc[0] = (control >> 24) & 0xff;
  desc[1] = (control >> 16) & 0xff;
  desc[2] = (control >> 8) & 0xff;
  desc[3] = control & 0xff;
  
  BX_MEM_THIS dmaWritePhysicalPage(dma_addr, 4, desc);
}

Bit32u bx_fw_cfg_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_FW_CFG_SMF
  bx_fw_cfg_c *class_ptr = (bx_fw_cfg_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_fw_cfg_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif

  Bit32u retval = 0;
  
  if (address == FW_CFG_IO_BASE) {
    // Reading from selector port returns 0
    BX_DEBUG(("fw_cfg: read from selector port"));
    return 0;
  }
  else if (address == FW_CFG_IO_BASE + 1) {
    // Read from data port
    if (BX_FW_CFG_THIS cur_entry != FW_CFG_INVALID && 
        BX_FW_CFG_THIS cur_entry < FW_CFG_MAX_ENTRY) {
      bx_fw_cfg_entry_t *entry = &BX_FW_CFG_THIS entries[BX_FW_CFG_THIS cur_entry];
      
      if (entry->data != NULL && BX_FW_CFG_THIS cur_offset < entry->len) {
        retval = entry->data[BX_FW_CFG_THIS cur_offset];
        BX_FW_CFG_THIS cur_offset++;
        BX_DEBUG(("fw_cfg: read data[%d] = 0x%02x from entry 0x%04x", 
                  BX_FW_CFG_THIS cur_offset - 1, retval, BX_FW_CFG_THIS cur_entry));
      }
    }
  }
  else if (address >= FW_CFG_IO_BASE + 4 && address <= FW_CFG_IO_BASE + 11) {
    // DMA address port - return DMA signature
    Bit32u offset = address - (FW_CFG_IO_BASE + 4);
    Bit64u sig = FW_CFG_DMA_SIGNATURE;
    retval = (sig >> (offset * 8)) & 0xff;
    BX_DEBUG(("fw_cfg: read DMA signature byte %d = 0x%02x", offset, retval));
  }

  return retval;
}

void bx_fw_cfg_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_FW_CFG_SMF
  bx_fw_cfg_c *class_ptr = (bx_fw_cfg_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_fw_cfg_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif

  if (address == FW_CFG_IO_BASE) {
    // Write to selector port
    Bit16u key = (Bit16u)(value & 0xffff);
    
    if (io_len == 2) {
      BX_FW_CFG_THIS cur_entry = key;
      BX_FW_CFG_THIS cur_offset = 0;
      // Log fw_cfg selector accesses to track what OVMF is reading
      if (key < FW_CFG_FILE_FIRST) {
        BX_DEBUG(("fw_cfg: Select standard entry 0x%04x", key));
      } else if (key >= FW_CFG_FILE_FIRST && BX_FW_CFG_THIS entries[key].len > 0) {
        BX_DEBUG(("fw_cfg: Select file entry 0x%04x (%u bytes)", key, BX_FW_CFG_THIS entries[key].len));
      }
      BX_DEBUG(("fw_cfg: selected entry 0x%04x", key));
    } else {
      BX_ERROR(("fw_cfg: invalid selector write, io_len=%d", io_len));
    }
  }
  else if (address == FW_CFG_IO_BASE + 1) {
    // Write to data port (not typically used by UEFI)
    BX_DEBUG(("fw_cfg: write to data port ignored"));
  }
  else if (address >= FW_CFG_IO_BASE + 4 && address <= FW_CFG_IO_BASE + 11) {
    // DMA control write - OVMF writes the physical address of the DMA descriptor
    // The descriptor is in guest memory and contains: control, length, address
    // Protocol (from QEMU fw_cfg_dma_mem_write):
    //   - 32-bit write to offset 0 (port 0x514): high 32 bits of address
    //   - 32-bit write to offset 4 (port 0x518): low 32 bits + triggers DMA
    unsigned offset = address - (FW_CFG_IO_BASE + 4);
    
    if (io_len == 4) {
      // 32-bit write - OVMF sends values in big-endian (swapped), need to swap back
      Bit32u swapped_value = ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) |
                             ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
      
      BX_DEBUG(("fw_cfg: DMA port 0x%x write, offset=%u, raw_value=0x%08x, swapped=0x%08x", 
               address, offset, value, swapped_value));
      
      if (offset == 0) {
        // High 32 bits written to port 0x514
        BX_FW_CFG_THIS dma_addr = ((Bit64u)swapped_value) << 32;
        BX_DEBUG(("fw_cfg: DMA high 32 bits = 0x%08x", swapped_value));
      } else if (offset == 4) {
        // Low 32 bits written to port 0x518 - triggers DMA
        BX_FW_CFG_THIS dma_addr = (BX_FW_CFG_THIS dma_addr & 0xFFFFFFFF00000000ULL) | (Bit64u)swapped_value;
        BX_DEBUG(("fw_cfg: DMA trigger, address = 0x" FMT_PHY_ADDRX,
           (Bit64u)BX_FW_CFG_THIS dma_addr));
        process_dma(BX_FW_CFG_THIS dma_addr);
        BX_FW_CFG_THIS dma_addr = 0;
      }
    } else if (io_len == 1) {
      // Byte-by-byte write (big-endian)
      int shift = (7 - offset) * 8;
      BX_FW_CFG_THIS dma_addr = (BX_FW_CFG_THIS dma_addr & ~(0xFFULL << shift)) | ((Bit64u)(value & 0xFF) << shift);
      
      // Trigger when last byte written (offset 7)
      if (offset == 7) {
        BX_DEBUG(("fw_cfg: DMA trigger (byte), address = 0x" FMT_PHY_ADDRX,
            (Bit64u)BX_FW_CFG_THIS dma_addr));
        process_dma(BX_FW_CFG_THIS dma_addr);
        BX_FW_CFG_THIS dma_addr = 0;
      }
    }
  }
}

// Generate E820 memory map describing system memory layout
void bx_fw_cfg_c::generate_e820_map(void)
{
  // Get total RAM size from Bochs memory system (get_memory_len returns BYTES, not MB!)
  Bit64u ram_size = BX_MEM_THIS get_memory_len();
  BX_INFO(("fw_cfg: Total RAM size = 0x" FMT_ADDRX64 " bytes (" FMT_LL "u MB)",
           (Bit64u)ram_size,
           (Bit64u)(ram_size / (1024ULL * 1024ULL))));
  
  // Build e820 table (matching QEMU's simpler layout for < 4GB RAM)
  const int MAX_E820_ENTRIES = 16;
  e820_entry *table = new e820_entry[MAX_E820_ENTRIES];
  int nr_e820 = 0;

  // Match QEMU's e820 layout: RAM entries + explicit RESERVED for PCI hole
  Bit64u below_4g_mem_size;
  Bit64u above_4g_mem_size = 0;
  
  if (ram_size >= 0xE0000000ULL) {  // If RAM >= 3.5GB
    below_4g_mem_size = 0xC0000000ULL;  // Stop at 3GB (leave 1GB hole for PCI)
    above_4g_mem_size = ram_size - below_4g_mem_size;
  } else {
    below_4g_mem_size = ram_size;  // Use all RAM if < 3.5GB
  }
  
  // Entry 0: All RAM from 0 to below_4g_mem_size
  table[nr_e820].address = 0;
  table[nr_e820].length = below_4g_mem_size;
  table[nr_e820].type = E820_RAM;
  nr_e820++;
  
  // NOTE: QEMU does NOT add a RESERVED entry for the PCI hole (3GB-4GB)!
  // The PCI hole is implicitly non-existent. Adding a RESERVED entry causes
  // CpuDxe.c to fail when it tries to add LAPIC MMIO space at 0xFEE00000,
  // because the reserved GCD descriptor conflicts with the MMIO aperture.
  
  // Entry 1: Above 4G RAM (only if we have RAM above 3.5GB)
  if (above_4g_mem_size > 0) {
    table[nr_e820].address = 0x100000000ULL;  // Start at 4GB
    table[nr_e820].length = above_4g_mem_size;
    table[nr_e820].type = E820_RAM;
    nr_e820++;
  }

  // Calculate total size and copy to persistent storage
  Bit32u table_size = nr_e820 * sizeof(e820_entry);
  Bit8u *e820_data = new Bit8u[table_size];
  memcpy(e820_data, table, table_size);
  
  // Log e820 entries for debugging
  BX_INFO(("fw_cfg: Generated %d e820 entries:", nr_e820));
  for (int i = 0; i < nr_e820; i++) {
    BX_INFO(("  Entry %d: addr=0x" FMT_PHY_ADDRX " len=0x" FMT_PHY_ADDRX " type=%d",
         i,
         (Bit64u)table[i].address,
         (Bit64u)table[i].length,
         table[i].type));
  }
  
  delete[] table;

  // Add to fw_cfg as FILE "etc/e820" (OVMF expects this as a file, not a key)
  add_file("etc/e820", e820_data, table_size);
}

// Generate HPET (High Precision Event Timer) configuration
void bx_fw_cfg_c::generate_hpet_config(void)
{
  hpet_fw_config *hpet_cfg = new hpet_fw_config;
  
  // Single HPET device
  hpet_cfg->count = 1;
  
  // HPET configuration (matching QEMU defaults)
  hpet_cfg->hpet[0].event_timer_block_id = 0x8086A201;  // Intel vendor ID
  hpet_cfg->hpet[0].address = 0xFED00000ULL;             // Standard HPET base address
  hpet_cfg->hpet[0].min_tick = 100;                      // Minimum tick period
  hpet_cfg->hpet[0].page_prot = 0;                       // No special page protection
  
  // Calculate size
  Bit32u hpet_size = sizeof(Bit8u) + sizeof(hpet_fw_entry) * hpet_cfg->count;
  
  // Copy to byte array
  Bit8u *hpet_data = new Bit8u[hpet_size];
  memcpy(hpet_data, hpet_cfg, hpet_size);
  delete hpet_cfg;
  
  // Add to fw_cfg
  add_bytes(FW_CFG_HPET, hpet_data, hpet_size);
  
  BX_INFO(("fw_cfg: added HPET configuration"));
}

// Generate ACPI tables for UEFI/OVMF support
void bx_fw_cfg_c::generate_acpi_tables(void)
{
  Bit64u ram_size = BX_MEM_THIS get_memory_len();
  Bit32u num_cpus = BX_SMP_PROCESSORS;
  
  BX_INFO(("fw_cfg: generating ACPI tables for %u CPUs, " FMT_LL "u MB RAM",
           num_cpus,
           (Bit64u)(ram_size / (1024ULL * 1024ULL))));
  
  // Create ACPI tables generator
  bx_acpi_tables_c acpi_gen;
  acpi_gen.generate_tables(ram_size, num_cpus);
  
  // Get generated blobs
  Bit8u* tables_blob = acpi_gen.get_tables_blob();
  Bit32u tables_size = acpi_gen.get_tables_size();
  
  Bit8u* rsdp_blob = acpi_gen.get_rsdp_blob();
  Bit32u rsdp_size = acpi_gen.get_rsdp_size();
  
  Bit8u* loader_blob = acpi_gen.get_loader_blob();
  Bit32u loader_size = acpi_gen.get_loader_size();
  
  // Copy data (add_file takes ownership, and acpi_gen will delete its buffers)
  Bit8u* tables_copy = new Bit8u[tables_size];
  memcpy(tables_copy, tables_blob, tables_size);
  
  Bit8u* rsdp_copy = new Bit8u[rsdp_size];
  memcpy(rsdp_copy, rsdp_blob, rsdp_size);
  
  Bit8u* loader_copy = new Bit8u[loader_size];
  memcpy(loader_copy, loader_blob, loader_size);
  
  // Add ACPI files to fw_cfg (OVMF expects these specific filenames)
  add_file("etc/acpi/tables", tables_copy, tables_size);
  add_file("etc/acpi/rsdp", rsdp_copy, rsdp_size);
  add_file("etc/table-loader", loader_copy, loader_size);
  
  BX_INFO(("fw_cfg: added ACPI tables (%u bytes), RSDP (%u bytes), loader (%u bytes)",
           tables_size, rsdp_size, loader_size));
}
