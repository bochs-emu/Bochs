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
 * Copyright (C) 2015       Benjamin D Lunt.
 * Copyright (C) 2006-2018  The Bochs Project
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
 *
 * Information found at:
 *   https://forums.virtualbox.org/viewtopic.php?t=8046
 *
 * Many Image Files can be found at:
 *   http://sourceforge.net/projects/virtualboximage/files/
 *
 */

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#ifdef BXIMAGE
#include "config.h"
#include "misc/bxcompat.h"
#include "misc/bswap.h"
#include "osdep.h"
#else
#include "iodev.h"
#endif
#include "hdimage.h"
#include "vbox.h"

#define LOG_THIS bx_devices.pluginHDImageCtl->

const off_t vbox_image_t::INVALID_OFFSET = (off_t)-1;
const int vbox_image_t::SECTOR_SIZE = 512;

vbox_image_t::vbox_image_t()
  : file_descriptor(-1),
  mtlb(0),
  block_data(0),
  current_offset(INVALID_OFFSET),
  mtlb_sector(0),
  is_dirty(0),
  mtlb_dirty(0),
  header_dirty(0)
{
  if (sizeof(_VBOX_VDI_Header) != 512) {
    BX_FATAL(("system error: invalid header structure size"));
  }
}

vbox_image_t::~vbox_image_t()
{
  close();
}

int vbox_image_t::open(const char* _pathname, int flags)
{
  Bit64u imgsize = 0;

  pathname = _pathname;
  close();

  file_descriptor = hdimage_open_file(pathname, flags, &imgsize, &mtime);

  if (!is_open())
    return -1;

  if (!read_header()) {
    BX_PANIC(("unable to read vbox virtual disk header from file '%s'", pathname));
    return -1;
  }

  // allocate one block of memory
  block_data = new Bit8u[(unsigned) header.block_size];
  if (block_data == 0) {
    BX_PANIC(("unable to allocate %d bytes for vbox block size", header.block_size));
  }
  is_dirty = 0;
  mtlb_dirty = 0;
  header_dirty = 0;

  // we allocate and read the image block map.
  // it is not a very large size, since each entry is only 32-bits and
  //  a 10-gig image will only use 40k of memory in this block.
  // (10gig = 10240 1-meg blocks with each entry using 4 bytes) = 40k
  mtlb = new Bit32s[(unsigned) header.blocks_in_hdd];
  if (mtlb == 0) {
    BX_PANIC(("unable to allocate %lu bytes for vbox image's map table", header.blocks_in_hdd * sizeof(Bit32u)));
  }

  // read in the map table
  if (bx_read_image(file_descriptor, header.offset_blocks, mtlb, (unsigned) header.blocks_in_hdd * sizeof(Bit32u))
      != (ssize_t)(header.blocks_in_hdd * sizeof(Bit32u))) {
      BX_PANIC(("did not read in map table"));
  }

  // read in the first index so that we have something in memory
  read_block(0);

  mtlb_sector = 0;
  current_offset = 0;

  hd_size = header.disk_size;
  sect_size = (unsigned) header.sector_size;
  if ((unsigned) header.cylinders > 0) {
    cylinders = (unsigned) header.cylinders;
    heads = (unsigned) header.heads;
    spt = (unsigned) header.sectors;
  } else {
    cylinders = (unsigned) ((header.disk_size / sect_size) / 16) / 63;
    heads = 16;
    spt = 63;
  }

  BX_DEBUG(("VBox VDI disk geometry:"));
  BX_DEBUG(("   .size      = " FMT_LL "d", hd_size));
  BX_DEBUG(("   .cylinders = %d", cylinders));
  BX_DEBUG(("   .heads     = %d", heads));
  BX_DEBUG(("   .sectors   = %d", spt));
  BX_DEBUG(("   .sect_size = %d", sect_size));

  return 1;
}

void vbox_image_t::close()
{
  if (file_descriptor == -1)
    return;

  flush();

  // write the map back to the disk
  if (mtlb_dirty) {
    if (bx_write_image(file_descriptor, header.offset_blocks, mtlb, (unsigned) header.blocks_in_hdd * sizeof(Bit32u))
        != (ssize_t)(header.blocks_in_hdd * sizeof(Bit32u))) {
        BX_PANIC(("did not write map table"));
    }
  }

  // write header back to image
  if (header_dirty) {
    if (bx_write_image(file_descriptor, 0, &header, sizeof(VBOX_VDI_Header)) != sizeof(VBOX_VDI_Header)) {
      BX_PANIC(("did not write header"));
    }
  }

  delete [] mtlb; mtlb = 0;
  delete [] block_data; block_data = 0;

  bx_close_image(file_descriptor, pathname);
  file_descriptor = -1;
}

Bit64s vbox_image_t::lseek(Bit64s offset, int whence)
{
  switch (whence) {
    case SEEK_SET:
      current_offset = (off_t) offset;
      return current_offset;
    case SEEK_CUR:
      current_offset += (off_t) offset;
      return current_offset;
    case SEEK_END:
      current_offset = header.disk_size + (off_t)offset;
      return current_offset;
    default:
      BX_INFO(("unknown 'whence' value (%d) when trying to seek vbox image", whence));
      return INVALID_OFFSET;
  }
}

ssize_t vbox_image_t::read(void *buf, size_t count)
{
  char *cbuf = (char*)buf;
  ssize_t total = 0;
  while (count > 0) {
    off_t readable = perform_seek();
    if (readable == INVALID_OFFSET) {
      BX_ERROR(("vbox disk image read failed on %u bytes at " FMT_LL "d", (unsigned)count, current_offset));
      return -1;
    }

    off_t copysize = ((off_t)count > readable) ? readable : count;
    off_t offset = current_offset & (header.block_size - 1);
    memcpy(cbuf, block_data + (size_t) offset, (size_t) copysize);

    current_offset += copysize;
    total += (long) copysize;
    cbuf += copysize;
    count -= (size_t) copysize;
  }

  return total;
}

ssize_t vbox_image_t::write(const void *buf, size_t count)
{
  char *cbuf = (char*)buf;
  ssize_t total = 0;
  while (count > 0) {
    off_t writable = perform_seek();
    if (writable == INVALID_OFFSET) {
      BX_ERROR(("vbox disk image write failed on %u bytes at " FMT_LL "d", (unsigned)count, current_offset));
      return -1;
    }

    off_t writesize = ((off_t)count > writable) ? writable : count;
    off_t offset = current_offset & (header.block_size - 1);
    memcpy(block_data + offset, cbuf, (size_t) writesize);

    current_offset += writesize;
    total += (long) writesize;
    cbuf += writesize;
    count -= (size_t) writesize;
    is_dirty = 1;
  }
  return total;
}

int vbox_image_t::check_format(int fd, Bit64u imgsize)
{
  VBOX_VDI_Header temp_header;

  if (bx_read_image(fd, 0, &temp_header, sizeof(VBOX_VDI_Header)) != sizeof(VBOX_VDI_Header))
    return HDIMAGE_READ_ERROR;

  // type can be 1 (Dynamic) or 2 (Static/Fixed)
  // block size must be 1Meg (FIXME: I think it can be anything as long as it is a power of 2)
  // sector size must be 512
  if (((temp_header.image_type < 1) || (temp_header.image_type > 2))
    || (temp_header.block_size != 0x00100000)
    || (temp_header.sector_size != 0x00000200))
    return HDIMAGE_NO_SIGNATURE;

  // version must be 01.01
  if (temp_header.version != 0x00010001)
    return HDIMAGE_VERSION_ERROR;

  return HDIMAGE_FORMAT_OK;
}

bx_bool vbox_image_t::is_open() const
{
  return (file_descriptor != -1);
}

bx_bool vbox_image_t::read_header()
{
  int ret;

  if (!is_open())
    BX_PANIC(("attempt to read vbox header from a closed file"));

  if ((ret = check_format(file_descriptor, 0)) != HDIMAGE_FORMAT_OK) {
    switch (ret) {
      case HDIMAGE_READ_ERROR:
        BX_ERROR(("vbox image read error"));
        break;
      case HDIMAGE_NO_SIGNATURE:
        BX_ERROR(("not a vbox image"));
        break;
      case HDIMAGE_VERSION_ERROR:
        BX_ERROR(("unsupported vbox image version"));
        break;
    }
    return 0;
  }

  if (bx_read_image(file_descriptor, 0, &header, sizeof(VBOX_VDI_Header)) != sizeof(VBOX_VDI_Header)) {
    return 0;
  }

  BX_DEBUG(("VBOX_VDI_Header (size=%u)", (unsigned)sizeof(VBOX_VDI_Header)));
  BX_DEBUG(("   .version                    = %08X", header.version));
  BX_DEBUG(("   .flags                      = %08X", header.flags));
  BX_DEBUG(("   .disk_size                  = " FMT_LL "d", header.disk_size));
  BX_DEBUG(("   .type                       = %d (%s)", header.image_type, (header.image_type == 1) ? "Dynamic" : "Static"));

  return 1;
}

//
// Returns the number of bytes that can be read from the current offset before needing
// to perform another seek.
//
off_t vbox_image_t::perform_seek()
{
  if (current_offset == INVALID_OFFSET) {
    BX_ERROR(("invalid offset specified in vbox seek"));
    return INVALID_OFFSET;
  }

  Bit32u index = (Bit32u) (current_offset / header.block_size);

  if (mtlb_sector == index) {
    return header.block_size - (current_offset & (header.block_size - 1));
  } else {
    flush();

    read_block(index);
    mtlb_sector = index;

    return header.block_size;
  }
}

void vbox_image_t::flush()
{
  if (!is_dirty)
    return;

  //
  // Write dirty sectors to disk.
  //
  write_block(mtlb_sector);
  is_dirty = 0;
}

void vbox_image_t::read_block(const Bit32u index)
{
  off_t offset;

  // if the mtlb[index] returns -1, then we haven't written this sector
  //  to disk yet, so return an "empty" buffer
  if (dtoh32(mtlb[index]) == -1) {
    if (header.image_type == 2) {
      BX_PANIC(("Found non-existing block in Static type image"));
    }
    memset(block_data, 0, header.block_size);

    BX_DEBUG(("reading empty block index %d", index));
  } else {
    if (dtoh32(mtlb[index]) >= (int) header.blocks_in_hdd) {
      BX_PANIC(("Trying to read past end of image (index out of range)"));
    }
    offset = dtoh32(mtlb[index]) * header.block_size;
    bx_read_image(file_descriptor, header.offset_data + offset, block_data, header.block_size);

    BX_DEBUG(("reading block index %d (%d) " FMT_LL "d", index, dtoh32(mtlb[index]), offset));
  }
}

void vbox_image_t::write_block(const Bit32u index)
{
  off_t offset;

  // if the mtlb[index] returns -1, then we haven't written this sector
  //  to disk yet, so allocate another and write it to file
  if (dtoh32(mtlb[index]) == -1) {
    if (header.image_type == 2) {
      BX_PANIC(("Found non-existing block in Static type image"));
    }
    mtlb[index] = htod32(header.blocks_allocated++);
    BX_DEBUG(("allocating new block at block: %d", dtoh32(mtlb[index])));
    mtlb_dirty = 1;
    header_dirty = 1;
  }

  if (dtoh32(mtlb[index]) >= (int) header.blocks_in_hdd) {
    BX_PANIC(("Trying to write past end of image (index out of range)"));
  }

  offset = dtoh32(mtlb[index]) * header.block_size;

  BX_DEBUG(("writing block index %d (%d) " FMT_LL "d", index, dtoh32(mtlb[index]), offset));

  bx_write_image(file_descriptor, header.offset_data + offset, block_data, header.block_size);
}

Bit32u vbox_image_t::get_capabilities(void)
{
  return HDIMAGE_HAS_GEOMETRY;
}

#ifndef BXIMAGE
bx_bool vbox_image_t::save_state(const char *backup_fname)
{
  return hdimage_backup_file(file_descriptor, backup_fname);
}

void vbox_image_t::restore_state(const char *backup_fname)
{
  int temp_fd;
  Bit64u imgsize;

  if ((temp_fd = hdimage_open_file(backup_fname, O_RDONLY, &imgsize, NULL)) < 0) {
    BX_PANIC(("Cannot open vbox image backup '%s'", backup_fname));
    return;
  }

  if (check_format(temp_fd, imgsize) < HDIMAGE_FORMAT_OK) {
    ::close(temp_fd);
    BX_PANIC(("Cannot detect vbox image header"));
    return;
  }
  ::close(temp_fd);
  close();
  if (!hdimage_copy_file(backup_fname, pathname)) {
    BX_PANIC(("Failed to restore vbox image '%s'", pathname));
    return;
  }
  device_image_t::open(pathname);
}
#endif
