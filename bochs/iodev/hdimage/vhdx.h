/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Block driver for Microsoft Hyper-V VHDX images
// Based on VHDX Format Specification v1.00 and QEMU implementation
//
// Copyright (C) 2013 Red Hat, Inc. (original QEMU code)
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

#ifndef BX_VHDXIMG_H
#define BX_VHDXIMG_H

// VHDX Format Constants
#define VHDX_FILE_SIGNATURE "vhdxfile"
#define VHDX_HEADER_SIGNATURE 0x64616568  // "head"
#define VHDX_REGION_SIGNATURE 0x69676572  // "regi"

// File layout offsets (all in bytes)
#define VHDX_FILE_ID_OFFSET      0
#define VHDX_HEADER1_OFFSET      (64 * 1024)
#define VHDX_HEADER2_OFFSET      (128 * 1024)
#define VHDX_REGION_TABLE_OFFSET (192 * 1024)
#define VHDX_REGION_TABLE2_OFFSET (256 * 1024)
#define VHDX_HEADER_SIZE         (4 * 1024)
#define VHDX_REGION_TABLE_SIZE   (64 * 1024)

// Block size limits
#define VHDX_BLOCK_SIZE_MIN      (1 * 1024 * 1024)    // 1 MB
#define VHDX_BLOCK_SIZE_MAX      (256 * 1024 * 1024)  // 256 MB

// BAT entry constants
#define VHDX_BAT_STATE_BIT_MASK         0x07
#define VHDX_BAT_FILE_OFF_MASK          0xFFFFFFFFFFF00000ULL  // Upper 44 bits
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_NOT_PRESENT        0
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_UNDEFINED          1
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_ZERO               2
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_UNMAPPED           3
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_UNMAPPED_v095      5
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_FULLY_PRESENT      6
#define VHDX_BAT_STATE_PAYLOAD_BLOCK_PARTIALLY_PRESENT  7

#define VHDX_SECTOR_BITMAP_BLOCK_SIZE (1024 * 1024)

// File parameters flags
#define VHDX_PARAMS_LEAVE_BLOCKS_ALLOCED 0x01
#define VHDX_PARAMS_HAS_PARENT           0x02

// Known region GUIDs
#define VHDX_REGION_BAT_GUID \
  { 0x66, 0x77, 0xc2, 0x2d, 0x23, 0xf6, 0x00, 0x42, \
    0x9d, 0x64, 0x11, 0x5e, 0x9b, 0xfd, 0x4a, 0x08 }

#define VHDX_REGION_METADATA_GUID \
  { 0x06, 0xa2, 0x7c, 0x8b, 0x90, 0x47, 0x9a, 0x4b, \
    0xb8, 0xfe, 0x57, 0x5f, 0x05, 0x0f, 0x88, 0x6e }

// Metadata item GUIDs
#define VHDX_METADATA_FILE_PARAMS_GUID \
  { 0x37, 0x67, 0xa1, 0xca, 0x36, 0xfa, 0x43, 0x4d, \
    0xb3, 0xb6, 0x33, 0xf0, 0xaa, 0x44, 0xe7, 0x6b }

#define VHDX_METADATA_VIRTUAL_DISK_SIZE_GUID \
  { 0x24, 0x42, 0xa5, 0x2f, 0x1b, 0xcd, 0x76, 0x48, \
    0xb2, 0x11, 0x5d, 0xbe, 0xd8, 0x3b, 0xf4, 0xb8 }

#define VHDX_METADATA_PAGE83_DATA_GUID \
  { 0xAB, 0x12, 0xCA, 0xBE, 0xE6, 0xB2, 0x23, 0x45, \
    0x93, 0xEF, 0xC3, 0x09, 0xE0, 0x00, 0xC7, 0x46 }

#define VHDX_METADATA_LOGICAL_SECTOR_SIZE_GUID \
  { 0x1d, 0xbf, 0x41, 0x81, 0x6f, 0xa9, 0x09, 0x47, \
    0xba, 0x47, 0xf2, 0x33, 0xa8, 0xfa, 0xab, 0x5f }

#define VHDX_METADATA_PHYSICAL_SECTOR_SIZE_GUID \
  { 0xc7, 0x48, 0xa3, 0xcd, 0x5d, 0x44, 0x71, 0x44, \
    0x9c, 0xc9, 0xe9, 0x88, 0x52, 0x51, 0xc5, 0x56 }

// Metadata entry flags
#define VHDX_METADATA_FLAG_IS_USER         0x00000001
#define VHDX_METADATA_FLAG_IS_VIRTUAL_DISK 0x00000002
#define VHDX_METADATA_FLAG_IS_REQUIRED     0x00000004

// Endian conversion for little-endian VHDX format
#if defined(BX_LITTLE_ENDIAN)
#define le16_to_cpu(val) (val)
#define le32_to_cpu(val) (val)
#define le64_to_cpu(val) (val)
#define cpu_to_le16(val) (val)
#define cpu_to_le32(val) (val)
#define cpu_to_le64(val) (val)
#else
#define le16_to_cpu(val) bx_bswap16(val)
#define le32_to_cpu(val) bx_bswap32(val)
#define le64_to_cpu(val) bx_bswap64(val)
#define cpu_to_le16(val) bx_bswap16(val)
#define cpu_to_le32(val) bx_bswap32(val)
#define cpu_to_le64(val) bx_bswap64(val)
#endif

// Force on-disk layout packing.
#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

// All structures are little-endian
typedef
struct vhdx_file_identifier_t {
    Bit64u signature;         // "vhdxfile"
    Bit16u creator[256];      // UTF-16 creator string (optional)
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_file_identifier_t;

typedef
struct vhdx_header_t {
    Bit32u signature;         // "head"
    Bit32u checksum;          // CRC-32C
    Bit64u sequence_number;   // Incremented on each header update
    Bit8u  file_write_guid[16];
    Bit8u  data_write_guid[16];
    Bit8u  log_guid[16];
    Bit16u log_version;
    Bit16u version;
    Bit32u log_length;
    Bit64u log_offset;
    Bit8u  reserved[4016];    // Pad to 4KB
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_header_t;

typedef
struct vhdx_region_table_header_t {
    Bit32u signature;         // "regi"
    Bit32u checksum;          // CRC-32C
    Bit32u entry_count;       // Number of valid entries
    Bit32u reserved;
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_region_table_header_t;

typedef
struct vhdx_region_table_entry_t {
    Bit8u  guid[16];          // Region GUID
    Bit64u file_offset;       // Offset in file (1MB aligned)
    Bit32u length;            // Length in bytes
    Bit32u required;          // Bit 0: required flag
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_region_table_entry_t;

typedef
struct vhdx_metadata_table_header_t {
    Bit64u signature;         // "metadata"
    Bit16u reserved;
    Bit16u entry_count;       // Number of entries
    Bit32u reserved2[5];
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_metadata_table_header_t;

typedef
struct vhdx_metadata_table_entry_t {
    Bit8u  item_id[16];       // Item GUID
    Bit32u offset;            // Offset within metadata region
    Bit32u length;            // Length in bytes
    Bit32u flags;             // IsUser, IsVirtualDisk, IsRequired
    Bit32u reserved;
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_metadata_table_entry_t;

typedef
struct vhdx_file_parameters_t {
    Bit32u block_size;        // Block size in bytes
    Bit32u flags;             // Leave blocks allocated, has parent
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhdx_file_parameters_t;

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

class vhdx_image_t : public device_image_t
{
  public:
    int open(const char* pathname, int flags);
    void close();
    Bit64s lseek(Bit64s offset, int whence);
    ssize_t read(void* buf, size_t count);
    ssize_t write(const void* buf, size_t count);

    Bit32u get_capabilities();
    static int check_format(int fd, Bit64u imgsize);

#ifdef BXIMAGE
    int create_image(const char *pathname, Bit64u size);
#else
    bool save_state(const char *backup_fname);
    void restore_state(const char *backup_fname);
#endif

  private:
    int parse_header();
    int parse_region_table();
    int parse_metadata();
    Bit64s get_sector_offset(Bit64s sector_num);
    Bit64s get_sector_offset_for_write(Bit64s sector_num);
    Bit64s alloc_payload_block(Bit64u bat_payload_index);
    bool zero_fill(Bit64u start, Bit64u length);
    bool write_bat_entry(Bit64u bat_index);
    int validate_checksum(Bit8u *buf, size_t size, Bit32u expected_crc);

    int fd;
    Bit64s sector_count;
    Bit64s cur_sector;
    Bit64u virtual_disk_size;
    Bit32u logical_sector_size;
    Bit32u physical_sector_size;
    Bit32u block_size;
    Bit32u sectors_per_block;
    Bit32u chunk_ratio;
    Bit32u file_params_flags;
    Bit64u bat_offset;
    Bit64u metadata_offset;
    Bit32u metadata_length;
    Bit32u bat_entry_count;     // total entries available in BAT region
    Bit32u bat_entries;         // entries actually used (per metadata)
    Bit64u *bat;              // Block Allocation Table

    Bit64u file_end;          // tracked end-of-file for allocation-on-write

    const char *pathname;
};

#endif
