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
      static const off_t INVALID_OFFSET = (off_t)-1;

#if (_MSC_VER<1300)
#pragma pack(push, 1)
#endif
      typedef
#if (_MSC_VER>=1300)
             __declspec(align(1))
#endif
        struct _COW_Header {
          char     id[4];
          unsigned header_version;
          unsigned flags;
          unsigned total_sectors;
          unsigned tlb_size_sectors;
          unsigned flb_offset_sectors;
          unsigned flb_count;
          unsigned next_sector_to_allocate;
          unsigned cylinders;
          unsigned heads;
          unsigned sectors;
          char     PAD0[1016];
          unsigned last_modified_time;
          char     PAD1[572];
          unsigned last_modified_time_save;
          char     label[8];
          unsigned chain_id;
          unsigned number_of_chains;
          unsigned cylinders_in_disk;
          unsigned heads_in_disk;
          unsigned sectors_in_disk;
          unsigned total_sectors_in_disk;
          char     PAD2[8];
          unsigned vmware_version;
          char     PAD3[364];
      } COW_Header
#if !defined(_MSC_VER)
        __attribute__((packed))
#endif
      ;
#if (_MSC_VER<1300)
#pragma pack(pop)
#endif

      struct COW_Image {
          int fd;
          COW_Header header;
          unsigned *  flb;
          unsigned ** slb;
          char *      tlb;
          off_t offset;
          off_t min_offset;
          off_t max_offset;
          bool synced;
      } * images, * current; 

      char * generate_cow_name(const char * filename, unsigned chain);
      bool is_valid_header(COW_Header & header);
      off_t perform_seek();
      bool sync();

      const unsigned FL_SHIFT;
      const unsigned FL_MASK;

      off_t requested_offset;
      unsigned slb_count;
      unsigned tlb_size;
};

#endif
