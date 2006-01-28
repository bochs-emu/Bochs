/*
 * This file provides the interface for using VMWare's virtual
 * disk image format under Bochs.
 *
 * Author: Sharvil Nanavati, for Net Integration Technologies, Inc.
 * Contact: snrrrub@yahoo.com
 *  
 * Copyright (C) 2003 Net Integration Technologies, Inc.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _COWDISK_H
#define _COWDISK_H 1

class vmware3_image_t : public device_image_t
{
  public:
      vmware3_image_t() : FL_SHIFT(25), FL_MASK(0xFE000000)
      { };
      int open (const char* pathname);
      void close();
      off_t lseek (off_t offset, int whence);
      ssize_t read (void* buf, size_t count);
      ssize_t write (const void* buf, size_t count);

  private:
      static const off_t INVALID_OFFSET;

#if defined(_MSC_VER) && (_MSC_VER<1300)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif
      typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
             __declspec(align(1))
#endif
        struct _COW_Header {
          Bit8u    id[4];
          Bit32u   header_version;
          Bit32u   flags;
          Bit32u   total_sectors;
          Bit32u   tlb_size_sectors;
          Bit32u   flb_offset_sectors;
          Bit32u   flb_count;
          Bit32u   next_sector_to_allocate;
          Bit32u   cylinders;
          Bit32u   heads;
          Bit32u   sectors;
          Bit8u    PAD0[1016];
          Bit32u   last_modified_time;
          Bit8u    PAD1[572];
          Bit32u   last_modified_time_save;
          Bit8u    label[8];
          Bit32u   chain_id;
          Bit32u   number_of_chains;
          Bit32u   cylinders_in_disk;
          Bit32u   heads_in_disk;
          Bit32u   sectors_in_disk;
          Bit32u   total_sectors_in_disk;
          Bit8u    PAD2[8];
          Bit32u   vmware_version;
          Bit8u    PAD3[364];
      }
#if !defined(_MSC_VER)
        GCC_ATTRIBUTE((packed))
#endif
      COW_Header
      ;
#if defined(_MSC_VER) && (_MSC_VER<1300)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

      struct COW_Image;
      friend struct COW_Image;
      struct COW_Image {
          int fd;
          COW_Header header;
          Bit32u   *  flb;
          Bit32u   ** slb;
          Bit8u    *  tlb;
          off_t offset;
          off_t min_offset;
          off_t max_offset;
          bool synced;
      } * images, * current; 

      int read_header(int fd, COW_Header & header);
      int write_header(int fd, COW_Header & header);

      int read_ints(int fd, Bit32u *buffer, size_t count);
      int write_ints(int fd, Bit32u *buffer, size_t count);

      char * generate_cow_name(const char * filename, Bit32u   chain);
      bool is_valid_header(COW_Header & header);
      off_t perform_seek();
      bool sync();

      const Bit32u   FL_SHIFT;
      const Bit32u   FL_MASK;

      off_t requested_offset;
      Bit32u   slb_count;
      Bit32u   tlb_size;
};
#endif
