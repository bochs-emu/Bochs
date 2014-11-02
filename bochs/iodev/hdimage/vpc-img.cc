/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Block driver for Connectix / Microsoft Virtual PC images (ported from QEMU)
//
// Copyright (c) 2005  Alex Beregszaszi
// Copyright (c) 2009  Kevin Wolf <kwolf@suse.de>
// Copyright (C) 2012-2013  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#ifdef BXIMAGE
#include "config.h"
#include "misc/bxcompat.h"
#include "osdep.h"
#include "misc/bswap.h"
#else
#include "iodev.h"
#endif
#include "hdimage.h"
#include "vpc-img.h"

#define LOG_THIS bx_devices.pluginHDImageCtl->

Bit32u vpc_checksum(Bit8u *buf, size_t size)
{
  Bit32u res = 0;
  unsigned i;

  for (i = 0; i < size; i++)
    res += buf[i];

  return ~res;
}

int vpc_image_t::check_format(int fd, Bit64u imgsize)
{
  Bit8u temp_footer_buf[HEADER_SIZE];
  vhd_footer_t *footer;
  int vpc_disk_type = VHD_DYNAMIC;

  if (bx_read_image(fd, 0, (char*)temp_footer_buf, HEADER_SIZE) != HEADER_SIZE) {
    return HDIMAGE_READ_ERROR;
  }

  footer = (vhd_footer_t*)temp_footer_buf;
  if (strncmp((char*)footer->creator, "conectix", 8)) {
    if (imgsize < HEADER_SIZE) {
      return HDIMAGE_NO_SIGNATURE;
    }
    // If a fixed disk, the footer is found only at the end of the file
    if (bx_read_image(fd, imgsize-HEADER_SIZE, (char*)temp_footer_buf, HEADER_SIZE) != HEADER_SIZE) {
      return HDIMAGE_READ_ERROR;
    }
    if (strncmp((char*)footer->creator, "conectix", 8)) {
      return HDIMAGE_NO_SIGNATURE;
    }
    vpc_disk_type = VHD_FIXED;
  }
  return vpc_disk_type;
}

int vpc_image_t::open(const char* _pathname, int flags)
{
  int i;
  vhd_footer_t *footer;
  vhd_dyndisk_header_t *dyndisk_header;
  Bit8u buf[HEADER_SIZE];
  Bit32u checksum;
  Bit64u offset = 0, imgsize = 0;
  int disk_type;

  pathname = _pathname;
  if ((fd = hdimage_open_file(pathname, flags, &imgsize, &mtime)) < 0) {
    BX_ERROR(("VPC: cannot open hdimage file '%s'", pathname));
    return -1;
  }

  disk_type = check_format(fd, imgsize);
  if (disk_type < 0) {
    switch (disk_type) {
      case HDIMAGE_READ_ERROR:
        BX_ERROR(("VPC: cannot read image file header of '%s'", _pathname));
        return -1;
      case HDIMAGE_NO_SIGNATURE:
        BX_ERROR(("VPC: signature missed in file '%s'", _pathname));
        return -1;
    }
  } else if (disk_type == VHD_FIXED) {
    offset = imgsize - HEADER_SIZE;
  }
  if (bx_read_image(fd, offset, (char*)footer_buf, HEADER_SIZE) != HEADER_SIZE) {
    return -1;
  }
  footer = (vhd_footer_t*)footer_buf;

  checksum = be32_to_cpu(footer->checksum);
  footer->checksum = 0;
  if (vpc_checksum(footer_buf, HEADER_SIZE) != checksum) {
    BX_ERROR(("The header checksum of '%s' is incorrect", pathname));
    return -1;
  }

  // Write 'checksum' back to footer, or else will leave it with zero.
  footer->checksum = be32_to_cpu(checksum);

  // The visible size of a image in Virtual PC depends on the geometry
  // rather than on the size stored in the footer (the size in the footer
  // is too large usually)
  cylinders = be16_to_cpu(footer->cyls);
  heads = footer->heads;
  spt = footer->secs_per_cyl;
  sector_count = (Bit64u)(cylinders * heads * spt);
  hd_size = sector_count * 512;

  if (sector_count >= 65535 * 16 * 255) {
    ::close(fd);
    return -EFBIG;
  }

  if (disk_type == VHD_DYNAMIC) {
    if (bx_read_image(fd, be64_to_cpu(footer->data_offset), buf, HEADER_SIZE) != HEADER_SIZE) {
      ::close(fd);
      return -1;
    }

    dyndisk_header = (vhd_dyndisk_header_t*)buf;

    if (strncmp((char*)dyndisk_header->magic, "cxsparse", 8)) {
      ::close(fd);
      return -1;
    }

    block_size = be32_to_cpu(dyndisk_header->block_size);
    bitmap_size = ((block_size / (8 * 512)) + 511) & ~511;

    max_table_entries = be32_to_cpu(dyndisk_header->max_table_entries);
    pagetable = new Bit32u[max_table_entries];

    bat_offset = be64_to_cpu(dyndisk_header->table_offset);
    if (bx_read_image(fd, bat_offset, (void*)pagetable, max_table_entries * 4) != (max_table_entries * 4)) {
      ::close(fd);
      return -1;
    }

    free_data_block_offset = (bat_offset + (max_table_entries * 4) + 511) & ~511;

    for (i = 0; i < max_table_entries; i++) {
      pagetable[i] = be32_to_cpu(pagetable[i]);
      if (pagetable[i] != 0xFFFFFFFF) {
        Bit64s next = (512 * (Bit64s)pagetable[i]) + bitmap_size + block_size;

        if (next > (Bit64s)free_data_block_offset) {
          free_data_block_offset = next;
        }
      }
    }

    last_bitmap_offset = (Bit64s) -1;
  }
  cur_sector = 0;

  BX_INFO(("'vpc' disk image opened: path is '%s'", pathname));

  return 0;
}

void vpc_image_t::close(void)
{
  if (fd > -1) {
    delete [] pagetable;
    ::close(fd);
  }
}

Bit64s vpc_image_t::lseek(Bit64s offset, int whence)
{
  if (whence == SEEK_SET) {
    cur_sector = (Bit32u)(offset / 512);
  } else if (whence == SEEK_CUR) {
    cur_sector += (Bit32u)(offset / 512);
  } else {
    BX_ERROR(("lseek: mode not supported yet"));
    return -1;
  }
  if (cur_sector >= sector_count)
    return -1;
  return 0;
}

ssize_t vpc_image_t::read(void* buf, size_t count)
{
  char *cbuf = (char*)buf;
  Bit32u scount = (Bit32u)(count / 0x200);
  vhd_footer_t *footer = (vhd_footer_t*)footer_buf;
  Bit64s offset, sectors, sectors_per_block;
  int ret;

  if (cpu_to_be32(footer->type) == VHD_FIXED) {
    return bx_read_image(fd, cur_sector * 512, buf, count);
  }

  while (scount > 0) {
    offset = get_sector_offset(cur_sector, 0);

    sectors_per_block = block_size >> 9;
    sectors = sectors_per_block - (cur_sector % sectors_per_block);
    if (sectors > scount) {
      sectors = scount;
    }

    if (offset == -1) {
      memset(buf, 0, 512);
    } else {
      ret = bx_read_image(fd, offset, cbuf, (int)sectors * 512);
      if (ret != 512) {
        return -1;
      }
    }
    scount -= (Bit32u)sectors;
    cur_sector += sectors;
    cbuf += sectors * 512;
  }
  return count;
}

ssize_t vpc_image_t::write(const void* buf, size_t count)
{
  char *cbuf = (char*)buf;
  Bit32u scount = (Bit32u)(count / 512);
  vhd_footer_t *footer =  (vhd_footer_t*)footer_buf;
  Bit64s offset, sectors, sectors_per_block;
  int ret;

  if (cpu_to_be32(footer->type) == VHD_FIXED) {
    return bx_write_image(fd, cur_sector * 512, (void*)buf, count);
  }

  while (scount > 0) {
    offset = get_sector_offset(cur_sector, 1);

    sectors_per_block = block_size >> 9;
    sectors = sectors_per_block - (cur_sector % sectors_per_block);
    if (sectors > scount) {
      sectors = scount;
    }

    if (offset == -1) {
      offset = alloc_block(cur_sector);
      if (offset < 0)
        return -1;
    }

    ret = bx_write_image(fd, offset, cbuf, (int)sectors * 512);
    if (ret != sectors * 512) {
      return -1;
    }

    scount -= (Bit32u)sectors;
    cur_sector += sectors;
    cbuf += sectors * 512;
  }
  return count;
}

Bit32u vpc_image_t::get_capabilities(void)
{
  return HDIMAGE_HAS_GEOMETRY;
}

#ifndef BXIMAGE
bx_bool vpc_image_t::save_state(const char *backup_fname)
{
  return hdimage_backup_file(fd, backup_fname);
}

void vpc_image_t::restore_state(const char *backup_fname)
{
  int temp_fd;
  Bit64u imgsize;

  if ((temp_fd = hdimage_open_file(backup_fname, O_RDONLY, &imgsize, NULL)) < 0) {
    BX_PANIC(("cannot open vpc image backup '%s'", backup_fname));
    return;
  }

  if (check_format(temp_fd, imgsize) < HDIMAGE_FORMAT_OK) {
    ::close(temp_fd);
    BX_PANIC(("Could not detect vpc image header"));
    return;
  }
  ::close(temp_fd);
  close();
  if (!hdimage_copy_file(backup_fname, pathname)) {
    BX_PANIC(("Failed to restore vpc image '%s'", pathname));
    return;
  }
  device_image_t::open(pathname);
}
#endif

/*
 * Returns the absolute byte offset of the given sector in the image file.
 * If the sector is not allocated, -1 is returned instead.
 *
 * The parameter write must be 1 if the offset will be used for a write
 * operation (the block bitmaps is updated then), 0 otherwise.
 */
Bit64s vpc_image_t::get_sector_offset(Bit64s sector_num, int write)
{
  Bit64u offset = sector_num * 512;
  Bit64u bitmap_offset, block_offset;
  Bit32u pagetable_index, pageentry_index;

  pagetable_index = (Bit32u)(offset / block_size);
  pageentry_index = (offset % block_size) / 512;

  if ((pagetable_index >= (Bit32u)max_table_entries) ||
      (pagetable[pagetable_index] == 0xffffffff))
    return -1; // not allocated

  bitmap_offset = 512 * (Bit64u) pagetable[pagetable_index];
  block_offset = bitmap_offset + bitmap_size + (512 * pageentry_index);

  // We must ensure that we don't write to any sectors which are marked as
  // unused in the bitmap. We get away with setting all bits in the block
  // bitmap each time we write to a new block. This might cause Virtual PC to
  // miss sparse read optimization, but it's not a problem in terms of
  // correctness.
  if (write && (last_bitmap_offset != bitmap_offset)) {
    Bit8u *bitmap = new Bit8u[bitmap_size];

    last_bitmap_offset = bitmap_offset;
    memset(bitmap, 0xff, bitmap_size);
    bx_write_image(fd, bitmap_offset, bitmap, bitmap_size);
    delete [] bitmap;
  }

  return (Bit64s)block_offset;
}

/*
 * Writes the footer to the end of the image file. This is needed when the
 * file grows as it overwrites the old footer
 *
 * Returns 0 on success and < 0 on error
 */
int vpc_image_t::rewrite_footer()
{
  int ret;
  Bit64s offset = free_data_block_offset;

  ret = bx_write_image(fd, offset, footer_buf, HEADER_SIZE);
  if (ret < 0)
    return ret;

  return 0;
}

/*
 * Allocates a new block. This involves writing a new footer and updating
 * the Block Allocation Table to use the space at the old end of the image
 * file (overwriting the old footer)
 *
 * Returns the sectors' offset in the image file on success and < 0 on error
 */
Bit64s vpc_image_t::alloc_block(Bit64s sector_num)
{
  Bit64s new_bat_offset;
  Bit64u old_fdbo;
  Bit32u index, bat_value;
  int ret;

  // Check if sector_num is valid
  if ((sector_num < 0) || (sector_num > sector_count))
    return -1;

  // Write entry into in-memory BAT
  index = (Bit32u)((sector_num * 512) / block_size);
  if (pagetable[index] != 0xFFFFFFFF)
    return -1;

  pagetable[index] = (Bit32u)(free_data_block_offset / 512);

  // Initialize the block's bitmap
  Bit8u *bitmap = new Bit8u[bitmap_size];
  memset(bitmap, 0xff, bitmap_size);
  ret = bx_write_image(fd, free_data_block_offset, bitmap, bitmap_size);
  delete [] bitmap;
  if (ret < 0) {
    return ret;
  }

  // Write new footer (the old one will be overwritten)
  old_fdbo = free_data_block_offset;
  free_data_block_offset += (block_size + bitmap_size);
  ret = rewrite_footer();
  if (ret < 0) {
    free_data_block_offset = old_fdbo;
    return -1;
  }

  // Write BAT entry to disk
  new_bat_offset = bat_offset + (4 * index);
  bat_value = be32_to_cpu(pagetable[index]);
  ret = bx_write_image(fd, new_bat_offset, &bat_value, 4);
  if (ret < 0) {
    free_data_block_offset = old_fdbo;
    return -1;
  }

  return get_sector_offset(sector_num, 0);
}
