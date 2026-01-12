/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Block driver and read support for Microsoft Hyper-V VHDX images
// Based on VHDX Format Specification v1.00 and QEMU implementation
//
// Copyright (c) 2013 Red Hat, Inc. (original QEMU code)
// Copyright (C) 2025 The Bochs Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/////////////////////////////////////////////////////////////////////////

#define BX_PLUGGABLE

#ifdef BXIMAGE
#include "config.h"
#include "misc/bxcompat.h"
#include "osdep.h"
#include "misc/bswap.h"
#else
#include "bochs.h"
#include "plugin.h"
#endif
#include "hdimage.h"
#include "vhdx.h"

#define LOG_THIS bx_hdimage_ctl.

#ifndef BXIMAGE

// disk image plugin entry point
PLUGIN_ENTRY_FOR_IMG_MODULE(vhdx)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_IMG;
  }
  return 0; // Success
}

#endif

//
// Define the static class that registers the derived device image class,
// and allocates one on request.
//
class bx_vhdx_locator_c : public hdimage_locator_c {
public:
  bx_vhdx_locator_c(void) : hdimage_locator_c("vhdx") {}
protected:
  device_image_t *allocate(Bit64u disk_size, const char *journal) {
    return (new vhdx_image_t());
  }
  int check_format(int fd, Bit64u disk_size) {
    return ((vhdx_image_t::check_format(fd, disk_size) > 0) ? HDIMAGE_FORMAT_OK : -1);
  }
} bx_vhdx_match;

// CRC-32C (Castagnoli) implementation for VHDX checksum
// Polynomial: 0x1EDC6F41 (reversed: 0x82F63B78)
static Bit32u crc32c_table[256];
static bool crc32c_initialized = false;

void vhdx_init_crc32c()
{
  if (crc32c_initialized) return;

  for (unsigned i = 0; i < 256; i++) {
    Bit32u crc = i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0x82F63B78;
      } else {
        crc >>= 1;
      }
    }
    crc32c_table[i] = crc;
  }
  crc32c_initialized = true;
}

Bit32u vhdx_checksum(Bit8u *buf, size_t size)
{
  vhdx_init_crc32c();

  Bit32u crc = 0xFFFFFFFF;
  for (size_t i = 0; i < size; i++) {
    crc = crc32c_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
  }
  return ~crc;
}

bool guid_eq(const Bit8u *guid1, const Bit8u *guid2)
{
  return memcmp(guid1, guid2, 16) == 0;
}

int vhdx_image_t::check_format(int fd, Bit64u imgsize)
{
  Bit8u buf[8];

  if (bx_read_image(fd, VHDX_FILE_ID_OFFSET, (char*)buf, 8) != 8) {
    return HDIMAGE_READ_ERROR;
  }

  if (memcmp(buf, VHDX_FILE_SIGNATURE, 8) != 0) {
    return HDIMAGE_NO_SIGNATURE;
  }

  return 1; // VHDX format detected
}

int vhdx_image_t::validate_checksum(Bit8u *buf, size_t size, Bit32u expected_crc)
{
  Bit32u calculated_crc = vhdx_checksum(buf, size);
  if (calculated_crc != expected_crc) {
    BX_ERROR(("VHDX: checksum mismatch: expected 0x%08X, got 0x%08X", expected_crc, calculated_crc));
    return -1;
  }
  return 0;
}

int vhdx_image_t::parse_header()
{
  Bit8u header1_buf[VHDX_HEADER_SIZE];
  Bit8u header2_buf[VHDX_HEADER_SIZE];
  vhdx_header_t *header1, *header2, *active_header;
  Bit32u checksum;

  // Read both headers
  if (bx_read_image(fd, VHDX_HEADER1_OFFSET, (char*)header1_buf, VHDX_HEADER_SIZE) != VHDX_HEADER_SIZE) {
    BX_ERROR(("VHDX: cannot read header 1"));
    return -1;
  }

  if (bx_read_image(fd, VHDX_HEADER2_OFFSET, (char*)header2_buf, VHDX_HEADER_SIZE) != VHDX_HEADER_SIZE) {
    BX_ERROR(("VHDX: cannot read header 2"));
    return -1;
  }

  header1 = (vhdx_header_t*)header1_buf;
  header2 = (vhdx_header_t*)header2_buf;

  // Validate header 1
  bool header1_valid = false;
  if (le32_to_cpu(header1->signature) == VHDX_HEADER_SIGNATURE) {
    checksum = le32_to_cpu(header1->checksum);
    header1->checksum = 0;
    if (validate_checksum(header1_buf, VHDX_HEADER_SIZE, checksum) == 0) {
      header1_valid = true;
      header1->checksum = cpu_to_le32(checksum);  // Restore checksum
    }
  }

  // Validate header 2
  bool header2_valid = false;
  if (le32_to_cpu(header2->signature) == VHDX_HEADER_SIGNATURE) {
    checksum = le32_to_cpu(header2->checksum);
    header2->checksum = 0;
    if (validate_checksum(header2_buf, VHDX_HEADER_SIZE, checksum) == 0) {
      header2_valid = true;
      header2->checksum = cpu_to_le32(checksum);  // Restore checksum
    }
  }

  // Select header with highest sequence number
  if (!header1_valid && !header2_valid) {
    BX_ERROR(("VHDX: both headers are invalid"));
    return -1;
  }

  if (header1_valid && header2_valid) {
    if (le64_to_cpu(header1->sequence_number) > le64_to_cpu(header2->sequence_number)) {
      active_header = header1;
    } else {
      active_header = header2;
    }
  } else if (header1_valid) {
    active_header = header1;
  } else {
    active_header = header2;
  }

  BX_DEBUG(("VHDX: using header with sequence %llu",
            (unsigned long long)le64_to_cpu(active_header->sequence_number)));

  return 0;
}

int vhdx_image_t::parse_region_table()
{
  Bit8u region_buf[VHDX_REGION_TABLE_SIZE];
  vhdx_region_table_header_t *region_header;
  vhdx_region_table_entry_t *entries;
  Bit32u checksum, entry_count;
  const Bit8u bat_guid[] = VHDX_REGION_BAT_GUID;
  const Bit8u metadata_guid[] = VHDX_REGION_METADATA_GUID;
  metadata_offset = 0;
  metadata_length = 0;

  // Read region table
  if (bx_read_image(fd, VHDX_REGION_TABLE_OFFSET, (char*)region_buf, VHDX_REGION_TABLE_SIZE) != VHDX_REGION_TABLE_SIZE) {
    BX_ERROR(("VHDX: cannot read region table"));
    return -1;
  }

  region_header = (vhdx_region_table_header_t*)region_buf;

  if (le32_to_cpu(region_header->signature) != VHDX_REGION_SIGNATURE) {
    BX_ERROR(("VHDX: invalid region table signature: 0x%08x", le32_to_cpu(region_header->signature)));
    return -1;
  }

  // Validate checksum
  checksum = le32_to_cpu(region_header->checksum);
  region_header->checksum = 0;
  if (validate_checksum(region_buf, VHDX_REGION_TABLE_SIZE, checksum) != 0) {
    BX_ERROR(("VHDX: region table checksum failed"));
    return -1;
  }
  region_header->checksum = cpu_to_le32(checksum);

  entry_count = le32_to_cpu(region_header->entry_count);
  BX_DEBUG(("VHDX: region entry_count=%u", entry_count));
  if (entry_count > 2047) {
    BX_ERROR(("VHDX: invalid region entry count: %u", entry_count));
    return -1;
  }

  // Find BAT and metadata regions
  entries = (vhdx_region_table_entry_t*)(region_buf + sizeof(vhdx_region_table_header_t));

  for (Bit32u i = 0; i < entry_count; i++) {
    BX_DEBUG(("VHDX: Entry %u GUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
           i,
           entries[i].guid[3], entries[i].guid[2], entries[i].guid[1], entries[i].guid[0],
           entries[i].guid[5], entries[i].guid[4],
           entries[i].guid[7], entries[i].guid[6],
           entries[i].guid[8], entries[i].guid[9],
           entries[i].guid[10], entries[i].guid[11], entries[i].guid[12], entries[i].guid[13], entries[i].guid[14], entries[i].guid[15]));

    if (guid_eq(entries[i].guid, bat_guid)) {
      bat_offset = le64_to_cpu(entries[i].file_offset);
      Bit32u bat_length = le32_to_cpu(entries[i].length);
      bat_entry_count = bat_length / sizeof(Bit64u);
      BX_DEBUG(("VHDX: BAT at offset %llu, %u entries",
                (unsigned long long)bat_offset, bat_entry_count));
    } else if (guid_eq(entries[i].guid, metadata_guid)) {
      metadata_offset = le64_to_cpu(entries[i].file_offset);
      metadata_length = le32_to_cpu(entries[i].length);
      BX_DEBUG(("VHDX: Metadata at offset %llu, length %llu",
                (unsigned long long)metadata_offset, (unsigned long long)metadata_length));
    }
  }

  if (bat_offset == 0 || metadata_offset == 0) {
    BX_ERROR(("VHDX: BAT or metadata region not found. bat_offset=%llu, metadata_offset=%llu", (unsigned long long)bat_offset, (unsigned long long)metadata_offset));
    return -1;
  }

  // Read BAT
  bat = new Bit64u[bat_entry_count];
  if (bx_read_image(fd, bat_offset, (char*)bat, bat_entry_count * sizeof(Bit64u)) != (int)(bat_entry_count * sizeof(Bit64u))) {
    BX_ERROR(("VHDX: cannot read BAT"));
    delete[] bat;
    return -1;
  }

  // Convert BAT entries to host endianness
  for (Bit32u i = 0; i < bat_entry_count; i++) {
    bat[i] = le64_to_cpu(bat[i]);
  }

  // Parse metadata
  if (parse_metadata() != 0) {
    delete[] bat;
    return -1;
  }

  // Compute how many BAT entries are actually used, per QEMU logic.
  // For non-differencing images, BAT has payload entries plus one interleaved
  // sector-bitmap entry between chunks: used = data_blocks + ((data_blocks-1)/chunk_ratio).
  if (block_size == 0 || chunk_ratio == 0) {
    BX_ERROR(("VHDX: invalid block_size/chunk_ratio"));
    delete[] bat;
    bat = NULL;
    return -1;
  }
  {
    Bit64u data_blocks_cnt = (virtual_disk_size + (Bit64u)block_size - 1ULL) / (Bit64u)block_size;
    Bit64u used = 0;
    if (data_blocks_cnt == 0) {
      used = 0;
    } else {
      used = data_blocks_cnt + ((data_blocks_cnt - 1ULL) / (Bit64u)chunk_ratio);
    }
    if (used > 0xFFFFFFFFULL || used > (Bit64u)bat_entry_count) {
      BX_ERROR(("VHDX: BAT region too small: used=%llu available=%u",
                (unsigned long long)used, bat_entry_count));
      delete[] bat;
      bat = NULL;
      return -1;
    }
    bat_entries = (Bit32u)used;
  }

  return 0;
}

int vhdx_image_t::parse_metadata()
{
  Bit8u *metadata_buf;
  vhdx_metadata_table_header_t *meta_header;
  vhdx_metadata_table_entry_t *meta_entries;
  Bit16u entry_count;
  const Bit8u file_params_guid[] = VHDX_METADATA_FILE_PARAMS_GUID;
  const Bit8u virt_size_guid[] = VHDX_METADATA_VIRTUAL_DISK_SIZE_GUID;
  const Bit8u log_sect_size_guid[] = VHDX_METADATA_LOGICAL_SECTOR_SIZE_GUID;
  const Bit8u phys_sect_size_guid[] = VHDX_METADATA_PHYSICAL_SECTOR_SIZE_GUID;

  if (metadata_length == 0) {
     BX_ERROR(("VHDX: metadata length is 0"));
     return -1;
  }

  metadata_buf = new Bit8u[metadata_length];

  // Read metadata region header
  if (bx_read_image(fd, metadata_offset, (char*)metadata_buf, metadata_length) != (ssize_t)metadata_length) {
    BX_ERROR(("VHDX: cannot read metadata region"));
    delete[] metadata_buf;
    return -1;
  }

  meta_header = (vhdx_metadata_table_header_t*)metadata_buf;

  // Check signature ("metadata") (endian-safe)
  if (memcmp(metadata_buf, "metadata", 8) != 0) {
    char sig[9];
    memcpy(sig, metadata_buf, 8);
    sig[8] = 0;
    BX_ERROR(("VHDX: invalid metadata signature: '%s'", sig));
    delete[] metadata_buf;
    return -1;
  }

  entry_count = le16_to_cpu(meta_header->entry_count);
  if (entry_count > 2047) {
    BX_ERROR(("VHDX: invalid metadata entry count: %u", entry_count));
    delete[] metadata_buf;
    return -1;
  }

  meta_entries = (vhdx_metadata_table_entry_t*)(metadata_buf + sizeof(vhdx_metadata_table_header_t));

  // Parse metadata entries
  bool found_block_size = false;
  bool found_virt_size = false;
  bool found_log_sect = false;
  bool found_phys_sect = false;

  for (Bit16u i = 0; i < entry_count; i++) {
    Bit32u offset = le32_to_cpu(meta_entries[i].offset);
    
    if (offset >= metadata_length) {
        BX_ERROR(("VHDX: metadata item offset %u out of bounds (length %u)", offset, metadata_length));
        continue;
    }

    if (guid_eq(meta_entries[i].item_id, file_params_guid)) {
      if (offset + sizeof(vhdx_file_parameters_t) > metadata_length) {
        BX_ERROR(("VHDX: file params metadata item out of bounds"));
        continue;
      }
      vhdx_file_parameters_t *params = (vhdx_file_parameters_t*)(metadata_buf + offset);
      block_size = le32_to_cpu(params->block_size);
      file_params_flags = le32_to_cpu(params->flags);
      found_block_size = true;
      BX_DEBUG(("VHDX: block_size = %u bytes", block_size));
    }
    else if (guid_eq(meta_entries[i].item_id, virt_size_guid)) {
      if (offset + sizeof(Bit64u) > metadata_length) {
        BX_ERROR(("VHDX: virtual size metadata item out of bounds"));
        continue;
      }
      virtual_disk_size = le64_to_cpu(*(Bit64u*)(metadata_buf + offset));
      found_virt_size = true;
      BX_DEBUG(("VHDX: virtual_disk_size = %llu bytes",
                (unsigned long long)virtual_disk_size));
    }
    else if (guid_eq(meta_entries[i].item_id, log_sect_size_guid)) {
      if (offset + sizeof(Bit32u) > metadata_length) {
        BX_ERROR(("VHDX: logical sector size metadata item out of bounds"));
        continue;
      }
      logical_sector_size = le32_to_cpu(*(Bit32u*)(metadata_buf + offset));
      found_log_sect = true;
      BX_DEBUG(("VHDX: logical_sector_size = %u bytes", logical_sector_size));
    }
    else if (guid_eq(meta_entries[i].item_id, phys_sect_size_guid)) {
      if (offset + sizeof(Bit32u) > metadata_length) {
        BX_ERROR(("VHDX: physical sector size metadata item out of bounds"));
        continue;
      }
      physical_sector_size = le32_to_cpu(*(Bit32u*)(metadata_buf + offset));
      found_phys_sect = true;
      BX_DEBUG(("VHDX: physical_sector_size = %u bytes", physical_sector_size));
    }
  }
  
  delete[] metadata_buf;

  if (!found_block_size || !found_virt_size || !found_log_sect || !found_phys_sect) {
    BX_ERROR(("VHDX: missing required metadata"));
    return -1;
  }

  if ((logical_sector_size != 512) && (logical_sector_size != 4096)) {
    BX_ERROR(("VHDX: unsupported logical sector size: %u", logical_sector_size));
    return -1;
  }

  if ((physical_sector_size != 512) && (physical_sector_size != 4096)) {
    BX_ERROR(("VHDX: unsupported physical sector size: %u", physical_sector_size));
    return -1;
  }

  if (virtual_disk_size == 0 || (virtual_disk_size % (Bit64u)logical_sector_size) != 0) {
    BX_ERROR(("VHDX: virtual_disk_size must be non-zero and a multiple of logical sector size"));
    return -1;
  }

  // Validate block size
  if (block_size < VHDX_BLOCK_SIZE_MIN || block_size > VHDX_BLOCK_SIZE_MAX ||
      (block_size & (block_size - 1)) != 0) {
    BX_ERROR(("VHDX: invalid block size: %u", block_size));
    return -1;
  }

  // Calculate sectors
  if (logical_sector_size == 0 || (block_size % logical_sector_size) != 0) {
    BX_ERROR(("VHDX: invalid sector size / block size combination: block_size=%u logical_sector_size=%u",
              block_size, logical_sector_size));
    return -1;
  }
  sectors_per_block = block_size / logical_sector_size;
  sector_count = (virtual_disk_size + logical_sector_size - 1) / logical_sector_size;

  // Calculate chunk ratio (payload blocks per chunk) using the sector bitmap coverage.
  // The sector bitmap block is 1 MiB and contains 8,388,608 bits.
  // Each bit corresponds to one logical sector within the chunk.
  // Therefore: bitmap_bits == chunk_ratio * sectors_per_block.
  {
    const Bit64u bitmap_bits = (Bit64u)VHDX_SECTOR_BITMAP_BLOCK_SIZE * 8ULL;
    if (sectors_per_block == 0) {
      BX_ERROR(("VHDX: sectors_per_block is 0"));
      return -1;
    }
    Bit64u ratio64 = bitmap_bits / (Bit64u)sectors_per_block;
    if (ratio64 == 0) {
      BX_ERROR(("VHDX: invalid chunk ratio computed (bitmap_bits=%llu sectors_per_block=%u)",
                (unsigned long long)bitmap_bits, sectors_per_block));
      return -1;
    }
    chunk_ratio = (Bit32u)ratio64;
  }

  if ((chunk_ratio & (chunk_ratio - 1)) != 0) {
    BX_ERROR(("VHDX: computed chunk_ratio is not a power of two: %u", chunk_ratio));
    return -1;
  }

  return 0;
}

Bit64s vhdx_image_t::get_sector_offset(Bit64s sector_num)
{
  if (sector_num < 0 || sector_num >= sector_count) {
    return -1;
  }

  // Calculate which payload block this sector belongs to
  Bit64u payload_block_index = (Bit64u)(sector_num / (Bit64s)sectors_per_block);
  Bit32u sector_in_block = (Bit32u)(sector_num % (Bit64s)sectors_per_block);

  if (chunk_ratio == 0) {
    BX_ERROR(("VHDX: invalid chunk_ratio 0"));
    return -1;
  }

  // For non-differencing VHDX images, the BAT interleaves sector-bitmap entries
  // between chunks (not at the start of every chunk). QEMU maps payload blocks as:
  //   bat_idx = payload_block_index + (payload_block_index / chunk_ratio)
  // where each inserted entry represents the sector bitmap block between chunks.
  Bit64u bat_payload_index = payload_block_index + (payload_block_index / (Bit64u)chunk_ratio);

  if (bat_payload_index >= (Bit64u)bat_entries) {
    BX_ERROR(("VHDX: BAT payload index out of range: %llu (max %u)",
              (unsigned long long)bat_payload_index, bat_entries));
    return -1;
  }

  Bit64u payload_entry = bat[bat_payload_index];
  Bit64u state = payload_entry & VHDX_BAT_STATE_BIT_MASK;

  if (state == VHDX_BAT_STATE_PAYLOAD_BLOCK_NOT_PRESENT ||
      state == VHDX_BAT_STATE_PAYLOAD_BLOCK_UNDEFINED ||
      state == VHDX_BAT_STATE_PAYLOAD_BLOCK_ZERO ||
      state == VHDX_BAT_STATE_PAYLOAD_BLOCK_UNMAPPED ||
      state == VHDX_BAT_STATE_PAYLOAD_BLOCK_UNMAPPED_v095) {
    return -2; // read as zeros
  }

  if (state != VHDX_BAT_STATE_PAYLOAD_BLOCK_FULLY_PRESENT &&
      state != VHDX_BAT_STATE_PAYLOAD_BLOCK_PARTIALLY_PRESENT) {
    BX_ERROR(("VHDX: unsupported payload BAT entry state: %llu", (unsigned long long)state));
    return -1;
  }

  if (state == VHDX_BAT_STATE_PAYLOAD_BLOCK_PARTIALLY_PRESENT) {
    // Differencing images (HAS_PARENT) use sector bitmaps to indicate presence.
    // Bochs VHDX currently supports base images only.
    BX_ERROR(("VHDX: payload block is partially present (differencing images not supported)"));
    return -1;
  }

  Bit64u file_offset = payload_entry & VHDX_BAT_FILE_OFF_MASK;
  if (file_offset == 0) {
    return -1;
  }

  file_offset += (Bit64u)sector_in_block * (Bit64u)logical_sector_size;
  return (Bit64s)file_offset;
}

int vhdx_image_t::open(const char* _pathname, int flags)
{
  Bit64u imgsize = 0;
  static bool printed_layout = false;

  pathname = _pathname;
  bat = NULL;
  bat_offset = 0;
  bat_entry_count = 0;
  chunk_ratio = 0;
  file_params_flags = 0;
  bat_entries = 0;

  if ((fd = hdimage_open_file(pathname, flags, &imgsize, &mtime)) < 0) {
    BX_ERROR(("VHDX: cannot open hdimage file '%s'", pathname));
    return -1;
  }

  // Check if this is a VHDX file
  int format_check = check_format(fd, imgsize);
  if (format_check < 0) {
    switch (format_check) {
      case HDIMAGE_READ_ERROR:
        BX_ERROR(("VHDX: cannot read image file header of '%s'", _pathname));
        bx_close_image(fd, pathname);
        fd = -1;
        return -1;
      case HDIMAGE_NO_SIGNATURE:
        BX_ERROR(("VHDX: signature missed in file '%s'", _pathname));
        bx_close_image(fd, pathname);
        fd = -1;
        return -1;
    }
  }

  // Parse VHDX structures
  if (parse_header() != 0) {
    bx_close_image(fd, pathname);
    fd = -1;
    return -1;
  }

  if (parse_region_table() != 0) {
    bx_close_image(fd, pathname);
    fd = -1;
    return -1;
  }

  if (file_params_flags & VHDX_PARAMS_HAS_PARENT) {
    BX_ERROR(("VHDX: differencing images (HAS_PARENT) are not supported"));
    bx_close_image(fd, pathname);
    fd = -1;
    return -1;
  }

  // Set geometry based on virtual disk size
  sect_size = logical_sector_size;
  hd_size = (Bit64u)sector_count * (Bit64u)sect_size;

  // Calculate CHS geometry
  if (logical_sector_size == 512) {
    // Standard 512-byte sectors
    if (virtual_disk_size >= 2040ULL * 1024 * 1024 * 1024) {
      // > 2040 GB: use maximum geometry
      cylinders = 65535;
      heads = 16;
      spt = 255;
    } else {
      // Use standard LBA geometry
      Bit64u total_sectors = virtual_disk_size / 512;
      if (total_sectors >= 65535 * 16 * 63) {
        // Use large disk geometry
        cylinders = (Bit32u)(total_sectors / (16 * 255));
        heads = 16;
        spt = 255;
      } else {
        // Standard geometry
        cylinders = (Bit32u)(total_sectors / (16 * 63));
        heads = 16;
        spt = 63;
      }
      if (cylinders > 65535) cylinders = 65535;
    }
  } else if (logical_sector_size == 4096) {
    // 4K native sectors
    Bit64u total_4k_sectors = virtual_disk_size / 4096;
    cylinders = (Bit32u)(total_4k_sectors / (16 * 255));
    heads = 16;
    spt = 255;
    if (cylinders > 65535) cylinders = 65535;
  }

  cur_sector = 0;

  BX_INFO(("'vhdx' disk image opened: path is '%s'", pathname));
  BX_INFO(("VHDX: virtual_disk_size=%llu MB, block_size=%u KB, logical_sector_size=%u",
           (unsigned long long)(virtual_disk_size / (1024 * 1024)),
           block_size / 1024,
           logical_sector_size));

  if (!printed_layout) {
    BX_INFO(("VHDX: sectors_per_block=%u, chunk_ratio=%u, bat_entries(used)=%u, bat_entries(region)=%u, file_params_flags=0x%08X",
             sectors_per_block, chunk_ratio, bat_entries, bat_entry_count, file_params_flags));

    const Bit64s probe_sectors[] = {0, 1, 2, 2048, 4096};
    for (unsigned i = 0; i < sizeof(probe_sectors)/sizeof(probe_sectors[0]); i++) {
      Bit64s s = probe_sectors[i];
      if (s >= 0 && s < sector_count) {
        Bit64s off = get_sector_offset(s);
        BX_INFO(("VHDX: probe sector %llu -> %lld", (unsigned long long)s, (long long)off));
      }
    }
    printed_layout = true;
  }

  return 0;
}

void vhdx_image_t::close(void)
{
  if (fd > -1) {
    if (bat != NULL) {
      delete[] bat;
      bat = NULL;
    }
    bx_close_image(fd, pathname);
    fd = -1;
  }
}

Bit64s vhdx_image_t::lseek(Bit64s offset, int whence)
{
  if (whence == SEEK_SET) {
    cur_sector = offset / logical_sector_size;
  } else if (whence == SEEK_CUR) {
    cur_sector += offset / logical_sector_size;
  } else {
    BX_ERROR(("VHDX: lseek: mode not supported yet"));
    return -1;
  }
  if (cur_sector >= sector_count)
    return -1;
  return 0;
}

ssize_t vhdx_image_t::read(void* buf, size_t count)
{
  char *cbuf = (char*)buf;
  Bit64s sectors_to_read = count / logical_sector_size;

  while (sectors_to_read > 0) {
    Bit64s offset = get_sector_offset(cur_sector);

    if (offset == -2) {
      // Zero/unallocated block, return zeros
      memset(cbuf, 0, logical_sector_size);
    } else if (offset < 0) {
      return -1;
    } else {
      int ret = bx_read_image(fd, offset, cbuf, logical_sector_size);
      if (ret != (int)logical_sector_size) {
        BX_ERROR(("VHDX: read error at sector %llu", (unsigned long long)cur_sector));
        return -1;
      }
    }

    cur_sector++;
    cbuf += logical_sector_size;
    sectors_to_read--;
  }

  return count;
}

ssize_t vhdx_image_t::write(const void* buf, size_t count)
{
  const char *cbuf = (const char*)buf;
  Bit64s sectors_to_write = count / logical_sector_size;

  while (sectors_to_write > 0) {
    Bit64s offset = get_sector_offset(cur_sector);

    if (offset < 0) {
      BX_ERROR(("VHDX: write to unmapped sector %llu not supported", (unsigned long long)cur_sector));
      return -1;
    } else {
      int ret = bx_write_image(fd, offset, (void*)cbuf, logical_sector_size);
      if (ret != (int)logical_sector_size) {
        BX_ERROR(("VHDX: write error at sector %llu", (unsigned long long)cur_sector));
        return -1;
      }
    }

    cur_sector++;
    cbuf += logical_sector_size;
    sectors_to_write--;
  }

  return count;
}

Bit32u vhdx_image_t::get_capabilities()
{
  return HDIMAGE_HAS_GEOMETRY;
}

#ifdef BXIMAGE
int vhdx_image_t::create_image(const char *pathname, Bit64u size)
{
  // Image creation not implemented
  return -1;
}
#else
bool vhdx_image_t::save_state(const char *backup_fname)
{
  // Read-only format, no state to save
  return false;
}

void vhdx_image_t::restore_state(const char *backup_fname)
{
  // Read-only format, no state to restore
}
#endif
