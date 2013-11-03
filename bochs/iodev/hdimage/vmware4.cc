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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#ifdef BXIMAGE
#include "config.h"
#include "misc/bxcompat.h"
#include "osdep.h"
#else
#include "iodev.h"
#endif
#include "hdimage.h"
#include "vmware4.h"

#define LOG_THIS bx_devices.pluginHDImageCtl->

const off_t vmware4_image_t::INVALID_OFFSET = (off_t)-1;
const int vmware4_image_t::SECTOR_SIZE = 512;

vmware4_image_t::vmware4_image_t()
  : file_descriptor(-1),
  tlb(0),
  tlb_offset(INVALID_OFFSET),
  current_offset(INVALID_OFFSET),
  is_dirty(0)
{
  if (sizeof(_VM4_Header) != 77) {
    BX_PANIC(("system error: invalid header structure size"));
  }
}

vmware4_image_t::~vmware4_image_t()
{
  close();
}

int vmware4_image_t::open(const char* _pathname, int flags)
{
  Bit64u imgsize = 0;

  pathname = _pathname;
  close();

  file_descriptor = hdimage_open_file(pathname, flags, &imgsize, &mtime);

  if (!is_open())
    return -1;

  if (!read_header()) {
    BX_PANIC(("unable to read vmware4 virtual disk header from file '%s'", pathname));
    return -1;
  }

  tlb = new Bit8u[(unsigned)header.tlb_size_sectors * SECTOR_SIZE];
  if (tlb == 0)
    BX_PANIC(("unable to allocate " FMT_LL "d bytes for vmware4 image's tlb", header.tlb_size_sectors * SECTOR_SIZE));

  tlb_offset = INVALID_OFFSET;
  current_offset = 0;
  is_dirty = 0;

  hd_size = header.total_sectors * SECTOR_SIZE;
  cylinders = (unsigned)(header.total_sectors / (16 * 63));
  heads = 16;
  spt = 63;

  BX_DEBUG(("VMware 4 disk geometry:"));
  BX_DEBUG(("   .size      = " FMT_LL "d", hd_size));
  BX_DEBUG(("   .cylinders = %d", cylinders));
  BX_DEBUG(("   .heads     = %d", heads));
  BX_DEBUG(("   .sectors   = %d", spt));

  return 1;
}

void vmware4_image_t::close()
{
  if (file_descriptor == -1)
    return;

  flush();
  delete [] tlb; tlb = 0;

  ::close(file_descriptor);
  file_descriptor = -1;
}

Bit64s vmware4_image_t::lseek(Bit64s offset, int whence)
{
  switch (whence) {
    case SEEK_SET:
      current_offset = (off_t)offset;
      return current_offset;
    case SEEK_CUR:
      current_offset += (off_t)offset;
      return current_offset;
    case SEEK_END:
      current_offset = header.total_sectors * SECTOR_SIZE + (off_t)offset;
      return current_offset;
    default:
      BX_DEBUG(("unknown 'whence' value (%d) when trying to seek vmware4 image", whence));
      return INVALID_OFFSET;
  }
}

ssize_t vmware4_image_t::read(void * buf, size_t count)
{
  ssize_t total = 0;
  while (count > 0) {
    off_t readable = perform_seek();
    if (readable == INVALID_OFFSET) {
      BX_DEBUG(("vmware4 disk image read failed on %u bytes at " FMT_LL "d", (unsigned)count, current_offset));
      return -1;
    }

    off_t copysize = ((off_t)count > readable) ? readable : count;
    memcpy(buf, tlb + current_offset - tlb_offset, (size_t)copysize);

    current_offset += copysize;
    total += (long)copysize;
    count -= (size_t)copysize;
  }
  return total;
}

ssize_t vmware4_image_t::write(const void * buf, size_t count)
{
  ssize_t total = 0;
  while (count > 0) {
    off_t writable = perform_seek();
    if (writable == INVALID_OFFSET) {
      BX_DEBUG(("vmware4 disk image write failed on %u bytes at " FMT_LL "d", (unsigned)count, current_offset));
      return -1;
    }

    off_t writesize = ((off_t)count > writable) ? writable : count;
    memcpy(tlb + current_offset - tlb_offset, buf, (size_t)writesize);

    current_offset += writesize;
    total += (long)writesize;
    count -= (size_t)writesize;
    is_dirty = 1;
  }
  return total;
}

int vmware4_image_t::check_format(int fd, Bit64u imgsize)
{
  VM4_Header temp_header;

  if (bx_read_image(fd, 0, &temp_header, sizeof(VM4_Header)) != sizeof(VM4_Header)) {
    return HDIMAGE_READ_ERROR;
  }
  if ((temp_header.id[0] != 'K') || (temp_header.id[1] != 'D') ||
      (temp_header.id[2] != 'M') || (temp_header.id[3] != 'V')) {
    return HDIMAGE_NO_SIGNATURE;
  }
  temp_header.version = dtoh32(temp_header.version);
  if (temp_header.version != 1) {
    return HDIMAGE_VERSION_ERROR;
  }
  return HDIMAGE_FORMAT_OK;
}

bx_bool vmware4_image_t::is_open() const
{
  return (file_descriptor != -1);
}

bx_bool vmware4_image_t::read_header()
{
  int ret;

  if (!is_open())
    BX_PANIC(("attempt to read vmware4 header from a closed file"));

  if ((ret = check_format(file_descriptor, 0)) != HDIMAGE_FORMAT_OK) {
    switch (ret) {
      case HDIMAGE_READ_ERROR:
        BX_ERROR(("vmware4 image read error"));
        break;
      case HDIMAGE_NO_SIGNATURE:
        BX_ERROR(("not a vmware4 image"));
        break;
      case HDIMAGE_VERSION_ERROR:
        BX_ERROR(("unsupported vmware4 image version"));
        break;
    }
    return 0;
  }

  if (bx_read_image(file_descriptor, 0, &header, sizeof(VM4_Header)) != sizeof(VM4_Header))
    return 0;

  header.version = dtoh32(header.version);
  header.flags = dtoh32(header.flags);
  header.total_sectors = dtoh64(header.total_sectors);
  header.tlb_size_sectors = dtoh64(header.tlb_size_sectors);
  header.description_offset_sectors = dtoh64(header.description_offset_sectors);
  header.description_size_sectors = dtoh64(header.description_size_sectors);
  header.slb_count = dtoh32(header.slb_count);
  header.flb_offset_sectors = dtoh64(header.flb_offset_sectors);
  header.flb_copy_offset_sectors = dtoh64(header.flb_copy_offset_sectors);
  header.tlb_offset_sectors = dtoh64(header.tlb_offset_sectors);

  BX_DEBUG(("VM4_Header (size=%u)", (unsigned)sizeof(VM4_Header)));
  BX_DEBUG(("   .version                    = %d", header.version));
  BX_DEBUG(("   .flags                      = %d", header.flags));
  BX_DEBUG(("   .total_sectors              = " FMT_LL "d", header.total_sectors));
  BX_DEBUG(("   .tlb_size_sectors           = " FMT_LL "d", header.tlb_size_sectors));
  BX_DEBUG(("   .description_offset_sectors = " FMT_LL "d", header.description_offset_sectors));
  BX_DEBUG(("   .description_size_sectors   = " FMT_LL "d", header.description_size_sectors));
  BX_DEBUG(("   .slb_count                  = %d", header.slb_count));
  BX_DEBUG(("   .flb_offset_sectors         = " FMT_LL "d", header.flb_offset_sectors));
  BX_DEBUG(("   .flb_copy_offset_sectors    = " FMT_LL "d", header.flb_copy_offset_sectors));
  BX_DEBUG(("   .tlb_offset_sectors         = " FMT_LL "d", header.tlb_offset_sectors));

  return 1;
}

//
// Returns the number of bytes that can be read from the current offset before needing
// to perform another seek.
//
off_t vmware4_image_t::perform_seek()
{
  if (current_offset == INVALID_OFFSET) {
    BX_DEBUG(("invalid offset specified in vmware4 seek"));
    return INVALID_OFFSET;
  }

  //
  // The currently loaded tlb can service the request.
  //
  if (tlb_offset / (header.tlb_size_sectors * SECTOR_SIZE) == current_offset / (header.tlb_size_sectors * SECTOR_SIZE))
    return (header.tlb_size_sectors * SECTOR_SIZE) - (current_offset - tlb_offset);

  flush();

  Bit64u index = current_offset / (header.tlb_size_sectors * SECTOR_SIZE);
  Bit32u slb_index = (Bit32u)(index % header.slb_count);
  Bit32u flb_index = (Bit32u)(index / header.slb_count);

  Bit32u slb_sector = read_block_index(header.flb_offset_sectors, flb_index);
  Bit32u slb_copy_sector = read_block_index(header.flb_copy_offset_sectors, flb_index);

  if (slb_sector == 0 && slb_copy_sector == 0) {
    BX_DEBUG(("loaded vmware4 disk image requires un-implemented feature"));
    return INVALID_OFFSET;
  }
  if (slb_sector == 0)
    slb_sector = slb_copy_sector;

  Bit32u tlb_sector = read_block_index(slb_sector, slb_index);
  tlb_offset = index * header.tlb_size_sectors * SECTOR_SIZE;
  if (tlb_sector == 0) {
    //
    // Allocate a new tlb
    //
    memset(tlb, 0, (size_t)header.tlb_size_sectors * SECTOR_SIZE);

    //
    // Instead of doing a write to increase the file size, we could use
    // ftruncate but it is not portable.
    //
    off_t eof = ((::lseek(file_descriptor, 0, SEEK_END) + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
    ::write(file_descriptor, tlb, (unsigned)header.tlb_size_sectors * SECTOR_SIZE);
    tlb_sector = (Bit32u)eof / SECTOR_SIZE;

    write_block_index(slb_sector, slb_index, tlb_sector);
    write_block_index(slb_copy_sector, slb_index, tlb_sector);

    ::lseek(file_descriptor, eof, SEEK_SET);
  } else {
    ::lseek(file_descriptor, tlb_sector * SECTOR_SIZE, SEEK_SET);
    ::read(file_descriptor, tlb, (unsigned)header.tlb_size_sectors * SECTOR_SIZE);
    ::lseek(file_descriptor, tlb_sector * SECTOR_SIZE, SEEK_SET);
  }

  return (header.tlb_size_sectors * SECTOR_SIZE) - (current_offset - tlb_offset);
}

void vmware4_image_t::flush()
{
  if (!is_dirty)
    return;

  //
  // Write dirty sectors to disk first. Assume that the file is already at the
  // position for the current tlb.
  //
  ::write(file_descriptor, tlb, (unsigned)header.tlb_size_sectors * SECTOR_SIZE);
  is_dirty = 0;
}

Bit32u vmware4_image_t::read_block_index(Bit64u sector, Bit32u index)
{
  Bit32u ret;

  bx_read_image(file_descriptor, sector * SECTOR_SIZE + index * sizeof(Bit32u),
                &ret, sizeof(Bit32u));

  return dtoh32(ret);
}

void vmware4_image_t::write_block_index(Bit64u sector, Bit32u index, Bit32u block_sector)
{
  block_sector = htod32(block_sector);

  bx_write_image(file_descriptor, sector * SECTOR_SIZE + index * sizeof(Bit32u),
                 &block_sector, sizeof(Bit32u));
}

Bit32u vmware4_image_t::get_capabilities(void)
{
  return HDIMAGE_HAS_GEOMETRY;
}

#ifndef BXIMAGE
bx_bool vmware4_image_t::save_state(const char *backup_fname)
{
  return hdimage_backup_file(file_descriptor, backup_fname);
}

void vmware4_image_t::restore_state(const char *backup_fname)
{
  int temp_fd;
  Bit64u imgsize;

  if ((temp_fd = hdimage_open_file(backup_fname, O_RDONLY, &imgsize, NULL)) < 0) {
    BX_PANIC(("Cannot open vmware4 image backup '%s'", backup_fname));
    return;
  }

  if (check_format(temp_fd, imgsize) < HDIMAGE_FORMAT_OK) {
    ::close(temp_fd);
    BX_PANIC(("Cannot detect vmware4 image header"));
    return;
  }
  ::close(temp_fd);
  close();
  if (!hdimage_copy_file(backup_fname, pathname)) {
    BX_PANIC(("Failed to restore vmware4 image '%s'", pathname));
    return;
  }
  device_image_t::open(pathname);
}
#endif
