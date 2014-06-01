/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/*
 * This file provides support for VMWare's virtual disk image
 * format version 4 and above.
 *
 * Author: Sharvil Nanavati
 * Contact: snrrrub@gmail.com
 *
 * Copyright (C) 2006 Sharvil Nanavati.
 * Copyright (C) 2006-2013  The Bochs Project
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

#ifndef _VMWARE4_H
#define _VMWARE4_H 1

#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif
typedef struct _VM4_Header
{
    Bit8u  id[4];
    Bit32u version;
    Bit32u flags;
    Bit64u total_sectors;
    Bit64u tlb_size_sectors;
    Bit64u description_offset_sectors;
    Bit64u description_size_sectors;
    Bit32u slb_count;
    Bit64u flb_offset_sectors;
    Bit64u flb_copy_offset_sectors;
    Bit64u tlb_offset_sectors;
    Bit8u  filler;
    Bit8u  check_bytes[4];
}
#if !defined(_MSC_VER)
GCC_ATTRIBUTE((packed))
#endif
VM4_Header;

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

class vmware4_image_t : public device_image_t
{
    public:
        vmware4_image_t();
        virtual ~vmware4_image_t();

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
        Bit32u read_block_index(Bit64u sector, Bit32u index);
        void write_block_index(Bit64u sector, Bit32u index, Bit32u block_sector);

        int file_descriptor;
        VM4_Header header;
        Bit8u* tlb;
        off_t tlb_offset;
        off_t current_offset;
        bx_bool is_dirty;
        const char *pathname;
};

#endif
