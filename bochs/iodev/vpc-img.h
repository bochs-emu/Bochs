/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
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

#ifndef BX_VPCIMG_H
#define BX_VPCIMG_H

#define HEADER_SIZE 512

enum vhd_type {
    VHD_FIXED           = 2,
    VHD_DYNAMIC         = 3,
    VHD_DIFFERENCING    = 4,
};

#if defined(_MSC_VER) && (_MSC_VER<1300)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

// always big-endian
typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
__declspec(align(1))
#endif
struct vhd_footer_t {
    Bit8u   creator[8]; // "conectix"
    Bit32u  features;
    Bit32u  version;

    // Offset of next header structure, 0xFFFFFFFF if none
    Bit64u  data_offset;

    // Seconds since Jan 1, 2000 0:00:00 (UTC)
    Bit32u  timestamp;

    Bit8u   creator_app[4]; // "vpc "
    Bit16u  major;
    Bit16u  minor;
    Bit8u   creator_os[4]; // "Wi2k"

    Bit64u  orig_size;
    Bit64u  size;

    Bit16u  cyls;
    Bit8u   heads;
    Bit8u   secs_per_cyl;

    Bit32u  type;

    // Checksum of the Hard Disk Footer ("one's complement of the sum of all
    // the bytes in the footer without the checksum field")
    Bit32u  checksum;

    // UUID used to identify a parent hard disk (backing file)
    Bit8u   uuid[16];

    Bit8u   in_saved_state;
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhd_footer_t;

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
__declspec(align(1))
#endif
struct vhd_dyndisk_header_t {
    Bit8u   magic[8]; // "cxsparse"

    // Offset of next header structure, 0xFFFFFFFF if none
    Bit64u  data_offset;

    // Offset of the Block Allocation Table (BAT)
    Bit64u  table_offset;

    Bit32u  version;
    Bit32u  max_table_entries; // 32bit/entry

    // 2 MB by default, must be a power of two
    Bit32u  block_size;

    Bit32u  checksum;
    Bit8u   parent_uuid[16];
    Bit32u  parent_timestamp;
    Bit32u  reserved;

    // Backing file name (in UTF-16)
    Bit8u   parent_name[512];

    struct {
        Bit32u  platform;
        Bit32u  data_space;
        Bit32u  data_length;
        Bit32u  reserved;
        Bit64u  data_offset;
    } parent_locator[8];
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
vhd_dyndisk_header_t;

#if defined(_MSC_VER) && (_MSC_VER<1300)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

class vpc_image_t : public device_image_t
{
  public:
    int open(const char* pathname);
    void close();
    Bit64s lseek(Bit64s offset, int whence);
    ssize_t read(void* buf, size_t count);
    ssize_t write(const void* buf, size_t count);
    Bit32u get_capabilities();

  private:
    Bit32u vpc_checksum(Bit8u *buf, size_t size);
    Bit64s get_sector_offset(Bit64s sector_num, int write);
    int rewrite_footer(void);
    Bit64s alloc_block(Bit64s sector_num);

    int fd;
    Bit64s sector_count;
    Bit64s cur_sector;
    Bit8u footer_buf[HEADER_SIZE];
    Bit64u free_data_block_offset;
    int max_table_entries;
    Bit64u bat_offset;
    Bit64u last_bitmap_offset;
    Bit32u *pagetable;

    Bit32u block_size;
    Bit32u bitmap_size;
};

#endif
