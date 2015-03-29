/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/*
 * This file provides support for the following VBox virtual 
 * disk image formats: VDI.
 *
 * Author: Benjamin D Lunt
 * Contact: fys [at] fysnet [dot] net
 *
 * Copyright (C) 2015 Benjamin D Lunt.
 * Copyright (C) 2006-2015  The Bochs Project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _VBOX_H
#define _VBOX_H 1

#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

typedef struct _VBOX_VDI_Header
{
    Bit8u  id[64];               // "<<< Sun xVM VirtualBox Disk Image >>>" + LF + Padding NULLs
    Bit32u signature;            // image signature
    Bit32u version;              // version (max.min) (two 16-bit words)
    Bit32u header_size;          // size of header (0x190 ?)
    Bit32u image_type;           // image type (1 = Dynamic VDI, 2 = Static VDI)
    Bit32u flags;                // image flags
    Bit8u  description[256];     // image description
    Bit32u offset_blocks;        // byte offset to mapped area
    Bit32u offset_data;          // byte offset to data area (actual data blocks)
    Bit32u cylinders;            // number of cylinders
    Bit32u heads;                // number of heads
    Bit32u sectors;              // number of sectors
    Bit32u sector_size;          // sector size in bytes
    Bit32u resv;                 // unused
    Bit64u disk_size;            // disk size in bytes
    Bit32u block_size;           // block size
    Bit32u block_extra;          // block extra data
    Bit32u blocks_in_hdd;        // total number of blocks in HDD (count of entries in map)
    Bit32u blocks_allocated;     // number of blocks allocated
    Bit8u  uuid_this[16];        // UUID of this VDI
    Bit8u  uuid_snap[16];        // UUID of last SNAP
    Bit8u  uuid_link[16];        // UUID link
    Bit8u  uuid_parent[16];      // UUID parent
    Bit8u  padding[56];          // padding to end of header
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
VBOX_VDI_Header;

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

class vbox_image_t : public device_image_t
{
    public:
        vbox_image_t();
        virtual ~vbox_image_t();

        int open(const char* pathname, int flags);
        void close();
        Bit64s lseek(Bit64s offset, int whence);
        ssize_t read(void* buf, size_t count);
        ssize_t write(const void* buf, size_t count);

        Bit32u get_capabilities();
        static int check_format(int fd, Bit64u imgsize);

#ifndef BXIMAGE
        bx_bool save_state(const char *backup_fname);
        void restore_state(const char *backup_fname);
#endif

    private:
        static const off_t INVALID_OFFSET;
        static const int SECTOR_SIZE;

        bx_bool is_open() const;

        bx_bool read_header();
        off_t perform_seek();
        void flush();
        void read_block(const Bit32u index);
        void write_block(const Bit32u index);

        int file_descriptor;
        VBOX_VDI_Header header;
        Bit32s *mtlb;
        Bit8u  *block_data;
        off_t current_offset;
        Bit32u mtlb_sector;
        bx_bool is_dirty;
        bx_bool mtlb_dirty;
        bx_bool header_dirty;
        const char *pathname;
};

#endif
