/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
//  Copyright (C) 2013-2014  Volker Ruppert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////
//
// Portions of this file contain code released under the following license.
//
// Connectix / Microsoft Virtual PC image creation code
//
// Copyright (c) 2005 Alex Beregszaszi
// Copyright (c) 2009 Kevin Wolf <kwolf@suse.de>
//
// VMDK version 4 image creation code
//
// Copyright (c) 2004 Fabrice Bellard
// Copyright (c) 2005 Filip Navara
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
/////////////////////////////////////////////////////////////////////////

// Create empty hard disk or floppy disk images for bochs.
// Convert a hard disk image from one format (mode) to another.
// Resize a hard disk image.
// Commit a redolog file to a supported base image.

#ifndef BXIMAGE
#define BXIMAGE
#endif

#include "config.h"
#include "bxcompat.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#  include <conio.h>
#  include <winioctl.h>
#endif
#include <ctype.h>

#include "osdep.h"
#include "bswap.h"

#include "iodev/hdimage/hdimage.h"
#include "iodev/hdimage/vmware3.h"
#include "iodev/hdimage/vmware4.h"
#include "iodev/hdimage/vpc-img.h"

#define BXIMAGE_MODE_NULL            0
#define BXIMAGE_MODE_CREATE_IMAGE    1
#define BXIMAGE_MODE_CONVERT_IMAGE   2
#define BXIMAGE_MODE_RESIZE_IMAGE    3
#define BXIMAGE_MODE_COMMIT_UNDOABLE 4
#define BXIMAGE_MODE_IMAGE_INFO      5

#define BX_MAX_CYL_BITS 24 // 8 TB

const int bx_max_hd_megs = (int)(((1 << BX_MAX_CYL_BITS) - 1) * 16.0 * 63.0 / 2048.0);

const char *hdimage_mode_names[] = {
  "flat",
  "concat",
  "external",
  "dll",
  "sparse",
  "vmware3",
  "vmware4",
  "undoable",
  "growing",
  "volatile",
  "vvfat",
  "vpc",
  NULL
};

int  bximage_mode;
int  bx_hdimage;
int  bx_fdsize_idx;
int  bx_min_hd_megs = 10;
int  bx_hdsize;
int  bx_imagemode;
int  bx_backup;
int  bx_interactive;
char bx_filename_1[512];
char bx_filename_2[512];

const char *EOF_ERR = "ERROR: End of input";
const char *svnid = "$Id$";
const char *divider = "========================================================================";
const char *main_menu_prompt =
"\n"
"1. Create new floppy or hard disk image\n"
"2. Convert hard disk image to other format (mode)\n"
"3. Resize hard disk image\n"
"4. Commit 'undoable' redolog to base image\n"
"5. Disk image info\n"
"\n"
"0. Quit\n"
"\n"
"Please choose one ";

// menu data for choosing floppy/hard disk
const char *fdhd_menu = "\nDo you want to create a floppy disk image or a hard disk image?\nPlease type hd or fd. ";
const char *fdhd_choices[] = { "fd", "hd" };
int fdhd_n_choices = 2;

// menu data for choosing floppy size
const char *fdsize_menu = "\nChoose the size of floppy disk image to create, in megabytes.\nPlease type 160k, 180k, 320k, 360k, 720k, 1.2M, 1.44M, 1.68M, 1.72M, or 2.88M.\n ";
const char *fdsize_choices[] = { "160k","180k","320k","360k","720k","1.2M","1.44M","1.68M","1.72M","2.88M" };
const unsigned fdsize_sectors[] = { 320, 360, 640, 720, 1440, 2400, 2880, 3360, 3444, 5760 };
int fdsize_n_choices = 10;

// menu data for choosing disk mode
const char *hdmode_menu = "\nWhat kind of image should I create?\nPlease type flat, sparse, growing, vpc or vmware4. ";
const char *hdmode_choices[] = {"flat", "sparse", "growing", "vpc", "vmware4" };
const int hdmode_choice_id[] = {BX_HDIMAGE_MODE_FLAT, BX_HDIMAGE_MODE_SPARSE,
                                BX_HDIMAGE_MODE_GROWING, BX_HDIMAGE_MODE_VPC,
                                BX_HDIMAGE_MODE_VMWARE4};
int hdmode_n_choices = 5;

#if !BX_HAVE_SNPRINTF
#include <stdarg.h>
/* XXX use real snprintf */
/* if they don't have snprintf, just use sprintf */
int snprintf(char *s, size_t maxlen, const char *format, ...)
{
  va_list arg;
  int done;

  va_start(arg, format);
  done = vsprintf(s, format, arg);
  va_end(arg);

  return done;
}
#endif  /* !BX_HAVE_SNPRINTF */

#if !BX_HAVE_MKSTEMP
int bx_mkstemp(char *tpl)
{
  mktemp(tpl);
  return ::open(tpl, O_RDWR | O_CREAT | O_TRUNC
#ifdef O_BINARY
                | O_BINARY
#endif
                , S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP);
}
#endif // !BX_HAVE_MKSTEMP

void myexit(int code)
{
#if defined(WIN32) && !defined(__CYGWIN__)
  printf("\nPress any key to continue\n");
  getch();
#endif
  exit(code);
}

// stolen from main.cc
void bx_center_print(FILE *file, const char *line, int maxwidth)
{
  int imax;
  int i;
  imax = (maxwidth - strlen(line)) >> 1;
  for (i=0; i<imax; i++) fputc(' ', file);
  fputs(line, file);
}

void print_banner()
{
  printf("%s\n", divider);
  bx_center_print(stdout, "bximage\n", 72);
  bx_center_print(stdout, "Disk Image Creation / Conversion / Resize and Commit Tool for Bochs\n", 72);
  bx_center_print(stdout, svnid, 72);
  printf("\n%s\n", divider);
}

// this is how we crash
void fatal(const char *c)
{
  printf("%s\n", c);
  myexit(1);
}

/* check if the argument string is present in the list -
   returns index on success, -1 on failure. */
int get_menu_index(char *arg, int n_choices, const char *choice[])
{
  int i;
  for (i=0; i<n_choices; i++) {
    if (!strcmp(choice[i], arg)) {
      // matched, return the choice number
      return i;
    }
  }
  return -1;
}

// remove leading spaces, newline junk at end.  returns pointer to
// cleaned string, which is between s0 and the null
char *clean_string(char *s0)
{
  char *s = s0;
  char *ptr;
  // find first nonblank
  while (isspace(*s))
    s++;
  // truncate string at first non-alphanumeric
  ptr = s;
  while (isprint(*ptr))
    ptr++;
  *ptr = 0;
  return s;
}

// returns 0 on success, -1 on failure.  The value goes into out.
int ask_int(const char *prompt, int min, int max, int the_default, int *out)
{
  int n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    printf("%s", prompt);
    printf("[%d] ", the_default);
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    illegal = (1 != sscanf(buffer, "%d", &n));
    if (illegal || n<min || n>max) {
      fprintf(stderr, "Your choice (%s) was not an integer between %d and %d.\n\n",
             clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

int ask_menu(const char *prompt, int n_choices, const char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while(1) {
    printf("%s", prompt);
    printf("[%s] ", choice[the_default]);
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    for (i=0; i<n_choices; i++) {
      if (!strcmp(choice[i], clean)) {
        // matched, return the choice number
        *out = i;
        return 0;
      }
    }
    printf("Your choice (%s) did not match any of the choices:\n", clean);
    for (i=0; i<n_choices; i++) {
      if (i>0) printf(", ");
      printf("%s", choice[i]);
    }
    printf("\n");
  }
}

int ask_yn(const char *prompt, int the_default, int *out)
{
  char buffer[16];
  char *clean;
  *out = -1;
  while (1) {
    printf("%s", prompt);
    printf("[%s] ", the_default?"yes":"no");
    if (!fgets(buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string(buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    switch (tolower(clean[0])) {
      case 'y': *out=1; return 0;
      case 'n': *out=0; return 0;
    }
    fprintf(stderr, "Please type either yes or no.\n");
  }
}

int ask_string(const char *prompt, char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  printf("%s", prompt);
  printf("[%s] ", the_default);
  if (!fgets(buffer, sizeof(buffer), stdin))
    return -1;
  clean = clean_string(buffer);
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy(out, the_default);
    return 0;
  }
  strcpy(out, clean);
  return 0;
}

device_image_t* init_image(Bit8u image_mode)
{
  device_image_t *hdimage = NULL;

  // instantiate the right class
  switch (image_mode) {

    case BX_HDIMAGE_MODE_FLAT:
      hdimage = new flat_image_t();
      break;

    case BX_HDIMAGE_MODE_CONCAT:
      hdimage = new concat_image_t();
      break;

#ifdef WIN32
    case BX_HDIMAGE_MODE_DLL_HD:
      hdimage = new dll_image_t();
      break;
#endif

    case BX_HDIMAGE_MODE_SPARSE:
      hdimage = new sparse_image_t();
      break;

    case BX_HDIMAGE_MODE_VMWARE3:
      hdimage = new vmware3_image_t();
      break;

    case BX_HDIMAGE_MODE_VMWARE4:
      hdimage = new vmware4_image_t();
      break;

    case BX_HDIMAGE_MODE_GROWING:
      hdimage = new growing_image_t();
      break;

    case BX_HDIMAGE_MODE_VPC:
      hdimage = new vpc_image_t();
      break;

    default:
      fatal("unsupported disk image mode");
      break;
  }
  return hdimage;
}

int create_image_file(const char *filename)
{
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC
#ifdef O_BINARY
                | O_BINARY
#endif
                , S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP
                );
  if (fd < 0) {
    fatal("ERROR: flat file is not writable");
  }
  return fd;
}

void create_flat_image(const char *filename, Bit64u size)
{
  char buffer[512];

  int fd = create_image_file(filename);
  memset(buffer, 0, 512);
  if (bx_write_image(fd, size - 512, buffer, 512) != 512)
    fatal("ERROR: while writing block in flat file !");
  close(fd);
}

#ifdef WIN32
void create_flat_image_win32(const char *filename, Bit64u size)
{
  HANDLE hFile;
  LARGE_INTEGER pos;
  DWORD dwCount, errCode;
  USHORT mode;
  char buffer[1024];

  hFile = CreateFile(filename, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // attempt to print an error
#ifdef HAVE_PERROR
    sprintf(buffer, "while opening '%s' for writing", filename);
    perror(buffer);
#endif
    fatal("ERROR: Could not write disk image");
  }

  SetLastError(NO_ERROR);
  mode = COMPRESSION_FORMAT_DEFAULT;
  dwCount = 0;
  memset(buffer, 0, 512);
  WriteFile(hFile, buffer, 512, &dwCount, NULL); // set the first sector to 0, Win98 doesn't zero out the file
                                                 // if there is a write at/over the end
  DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, &mode, sizeof(mode), NULL, 0, &dwCount, NULL);
  pos.u.LowPart = (unsigned long)((size - 512));
  pos.u.HighPart = (unsigned long)((size - 512) >> 32);
  pos.u.LowPart = SetFilePointer(hFile, pos.u.LowPart, &pos.u.HighPart, FILE_BEGIN);
  memset(buffer, 0, 512);
  if ((pos.u.LowPart == 0xffffffff && GetLastError() != NO_ERROR) ||
      !WriteFile(hFile, buffer, 512, &dwCount, NULL) || (dwCount != 512)) {
    errCode = GetLastError();
    CloseHandle(hFile);
    if (errCode == ERROR_DISK_FULL) {
      fatal("\nERROR: Not enough space on disk for image!");
    } else {
      sprintf(buffer, "\nERROR: Disk image creation failed with error code %i!", errCode);
      fatal(buffer);
    }
  }

  CloseHandle(hFile);
}
#endif

void create_sparse_image(const char *filename, Bit64u size)
{
  Bit64u numpages;
  sparse_header_t header;
  size_t sizesofar;
  int padtopagesize;

  memset(&header, 0, sizeof(header));
  header.magic = htod32(SPARSE_HEADER_MAGIC);
  header.version = htod32(SPARSE_HEADER_VERSION);

  header.pagesize = htod32((1 << 10) * 32); // Use 32 KB Pages - could be configurable
  numpages = (size / dtoh32(header.pagesize)) + 1;

  header.numpages = htod32((Bit32u)numpages);
  header.disk = htod64(size);

  if (numpages != dtoh32(header.numpages)) {
    fatal("ERROR: The disk image is too large for a sparse image!");
    // Could increase page size here.
    // But note this only happens at 128 Terabytes!
  }

  int fd = create_image_file(filename);
  if (bx_write_image(fd, 0, &header, sizeof(header)) != sizeof(header)) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write header!");
  }

  Bit32u *pagetable = new Bit32u[dtoh32(header.numpages)];
  if (pagetable == NULL)
    fatal("ERROR: The disk image is not complete - could not create pagetable!");
  for (Bit32u i=0; i<dtoh32(header.numpages); i++)
    pagetable[i] = htod32(SPARSE_PAGE_NOT_ALLOCATED);

  if (bx_write_image(fd, sizeof(header), pagetable, 4 * dtoh32(header.numpages)) != (int)(4 * dtoh32(header.numpages))) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write pagetable!");
  }
  delete [] pagetable;

  sizesofar = SPARSE_HEADER_SIZE + (4 * dtoh32(header.numpages));
  padtopagesize = dtoh32(header.pagesize) - (sizesofar & (dtoh32(header.pagesize) - 1));

  Bit8u *padding = new Bit8u[padtopagesize];
  memset(padding, 0, padtopagesize);
  if (padding == NULL)
    fatal("ERROR: The disk image is not complete - could not create padding!");
  if (bx_write_image(fd, sizesofar, padding, padtopagesize) != padtopagesize) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write padding!");
  }
  delete [] padding;
  close(fd);
}

void create_growing_image(const char *filename, Bit64u size)
{
  redolog_t *redolog = new redolog_t;
  if (redolog->create(filename, REDOLOG_SUBTYPE_GROWING, size) < 0)
    fatal("Can't create growing mode image");
  redolog->close();
  delete redolog;
}

void create_vpc_image(const char *filename, Bit64u size)
{
  Bit8u buf[1024];
  Bit16u cyls = 0;
  Bit8u heads = 16, secs_per_cyl = 63;
  Bit64u total_sectors;
  Bit64s offset;
  size_t block_size, num_bat_entries;
  int fd, i;

  total_sectors = size >> 9;
  cyls = (Bit16u)(size/heads/secs_per_cyl/512.0);

  vhd_footer_t *footer = (vhd_footer_t*)buf;
  memset(footer, 0, HEADER_SIZE);
  memcpy(footer->creator, "conectix", 8);
  // TODO Check if "bxic" creator_app is ok for VPC
  memcpy(footer->creator_app, "bxic", 4);
  memcpy(footer->creator_os, "Wi2k", 4);

  footer->features = be32_to_cpu(0x02);
  footer->version = be32_to_cpu(0x00010000);
  footer->data_offset = be64_to_cpu(HEADER_SIZE);
  footer->timestamp = be32_to_cpu((Bit32u)time(NULL) - VHD_TIMESTAMP_BASE);

  // Version of Virtual PC 2007
  footer->major = be16_to_cpu(0x0005);
  footer->minor = be16_to_cpu(0x0003);
  footer->orig_size = be64_to_cpu(total_sectors * 512);
  footer->size = be64_to_cpu(total_sectors * 512);
  footer->cyls = be16_to_cpu(cyls);
  footer->heads = heads;
  footer->secs_per_cyl = secs_per_cyl;
  footer->type = be32_to_cpu(VHD_DYNAMIC);
  footer->checksum = be32_to_cpu(vpc_checksum(buf, HEADER_SIZE));

  fd = create_image_file(filename);
  // Write the footer (twice: at the beginning and at the end)
  block_size = 0x200000;
  num_bat_entries = (size_t)(total_sectors + block_size / 512) / (block_size / 512);
  if (bx_write_image(fd, 0, footer, HEADER_SIZE) != HEADER_SIZE) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write footer!");
  }
  offset = 1536 + ((num_bat_entries * 4 + 511) & ~511);
  if (bx_write_image(fd, offset, footer, HEADER_SIZE) != HEADER_SIZE) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write footer!");
  }

  // Write the initial BAT
  memset(buf, 0xFF, 512);
  offset = 3 * 512;
  for (i = 0; i < (int)(num_bat_entries * 4 + 511) / 512; i++) {
    if (bx_write_image(fd, offset, buf, 512) != 512) {
      close(fd);
      fatal("ERROR: The disk image is not complete - could not write BAT!");
    }
    offset += 512;
  }

  vhd_dyndisk_header_t *header = (vhd_dyndisk_header_t*)buf;
  memset(header, 0, 1024);
  memcpy(header->magic, "cxsparse", 8);
  /*
   * Note: The spec is actually wrong here for data_offset, it says
   * 0xFFFFFFFF, but MS tools expect all 64 bits to be set.
   */
  header->data_offset = be64_to_cpu(0xFFFFFFFFFFFFFFFFULL);
  header->table_offset = be64_to_cpu(3 * 512);
  header->version = be32_to_cpu(0x00010000);
  header->block_size = be32_to_cpu(block_size);
  header->max_table_entries = be32_to_cpu(num_bat_entries);
  header->checksum = be32_to_cpu(vpc_checksum(buf, 1024));
  if (bx_write_image(fd, 512, header, 1024) != 1024) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write header!");
  }

  close(fd);
}

void create_vmware4_image(const char *filename, Bit64u size)
{
  const int SECTOR_SIZE = 512;
  const char desc_template[] =
    "# Disk DescriptorFile\n"
    "version=1\n"
    "CID=%x\n"
    "parentCID=ffffffff\n"
    "createType=\"monolithicSparse\"\n"
    "\n"
    "# Extent description\n"
    "%s"
    "\n"
    "# The Disk Data Base\n"
    "#DDB\n"
    "\n"
    "ddb.virtualHWVersion = \"4\"\n"
    "ddb.geometry.cylinders = \"" FMT_LL "d\"\n"
    "ddb.geometry.heads = \"16\"\n"
    "ddb.geometry.sectors = \"63\"\n"
    "ddb.adapterType = \"ide\"\n";
  int fd, i;
  Bit64s offset, cyl;
  Bit32u tmp, grains, gt_size, gt_count, gd_size;
  Bit8u buffer[SECTOR_SIZE];
  char desc_line[256];
  _VM4_Header header;

  header.id[0] = 'K';
  header.id[1] = 'D';
  header.id[2] = 'M';
  header.id[3] = 'V';
  header.version = htod32(1);
  header.flags = htod32(3);
  header.total_sectors = htod64(size / SECTOR_SIZE);
  header.tlb_size_sectors = 128;
  header.description_offset_sectors = 1;
  header.description_size_sectors = 20;
  header.slb_count = 512;
  header.flb_offset_sectors = header.description_offset_sectors +
                              header.description_size_sectors;

  grains = (Bit32u)((size / 512 + header.tlb_size_sectors - 1) / header.tlb_size_sectors);
  gt_size = ((header.slb_count * sizeof(Bit32u)) + 511) >> 9;
  gt_count = (grains + header.slb_count - 1) / header.slb_count;
  gd_size = (gt_count * sizeof(Bit32u) + 511) >> 9;

  header.flb_copy_offset_sectors = header.flb_offset_sectors + gd_size + (gt_size * gt_count);
  header.tlb_offset_sectors =
    ((header.flb_copy_offset_sectors + gd_size + (gt_size * gt_count) +
    header.tlb_size_sectors - 1) / header.tlb_size_sectors) *
    header.tlb_size_sectors;

  header.tlb_size_sectors = htod64(header.tlb_size_sectors);
  header.description_offset_sectors = htod64(header.description_offset_sectors);
  header.description_size_sectors = htod64(header.description_size_sectors);
  header.slb_count = htod32(header.slb_count);
  header.flb_offset_sectors = htod64(header.flb_offset_sectors);
  header.flb_copy_offset_sectors = htod64(header.flb_copy_offset_sectors);
  header.tlb_offset_sectors = htod64(header.tlb_offset_sectors);
  header.check_bytes[0] = 0x0a;
  header.check_bytes[1] = 0x20;
  header.check_bytes[2] = 0x0d;
  header.check_bytes[3] = 0x0a;

  memset(buffer, 0, SECTOR_SIZE);
  memcpy(buffer, &header, sizeof(_VM4_Header));

  fd = create_image_file(filename);
  if (bx_write_image(fd, 0, buffer, SECTOR_SIZE) != SECTOR_SIZE) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write header!");
  }
  memset(buffer, 0, SECTOR_SIZE);
  offset = dtoh64(header.tlb_offset_sectors * SECTOR_SIZE) - SECTOR_SIZE;
  if (bx_write_image(fd, offset, buffer, SECTOR_SIZE) != SECTOR_SIZE) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write empty table!");
  }
  offset = dtoh64(header.flb_offset_sectors) * SECTOR_SIZE;
  for (i = 0, tmp = (Bit32u)dtoh64(header.flb_offset_sectors) + gd_size; i < (int)gt_count; i++, tmp += gt_size) {
    if (bx_write_image(fd, offset, &tmp, sizeof(tmp)) != sizeof(tmp)) {
      close(fd);
      fatal("ERROR: The disk image is not complete - could not write table!");
    }
    offset += sizeof(tmp);
  }
  offset = dtoh64(header.flb_copy_offset_sectors) * SECTOR_SIZE;
  for (i = 0, tmp = (Bit32u)dtoh64(header.flb_copy_offset_sectors) + gd_size; i < (int)gt_count; i++, tmp += gt_size) {
    if (bx_write_image(fd, offset, &tmp, sizeof(tmp)) != sizeof(tmp)) {
      close(fd);
      fatal("ERROR: The disk image is not complete - could not write backup table!");
    }
    offset += sizeof(tmp);
  }
  memset(buffer, 0, SECTOR_SIZE);
  cyl = (Bit64u)(size / 16 / 63 / 512.0);
  snprintf(desc_line, 256, "RW " FMT_LL "d SPARSE \"%s\"", size / SECTOR_SIZE, filename);
  sprintf((char*)buffer, desc_template, (Bit32u)time(NULL), desc_line, cyl);
  offset = dtoh64(header.description_offset_sectors) * SECTOR_SIZE;
  if (bx_write_image(fd, offset, buffer, SECTOR_SIZE) != SECTOR_SIZE) {
    close(fd);
    fatal("ERROR: The disk image is not complete - could not write description!");
  }
  close(fd);
}

void create_hard_disk_image(const char *filename, int imgmode, Bit64u size)
{
  switch (imgmode) {
    case BX_HDIMAGE_MODE_FLAT:
#ifndef WIN32
      create_flat_image(filename, size);
#else
      create_flat_image_win32(filename, size);
#endif
      break;

    case BX_HDIMAGE_MODE_GROWING:
      create_growing_image(filename, size);
      break;

    case BX_HDIMAGE_MODE_SPARSE:
      create_sparse_image(filename, size);
      break;

    case BX_HDIMAGE_MODE_VPC:
      create_vpc_image(filename, size);
      break;

    case BX_HDIMAGE_MODE_VMWARE4:
      create_vmware4_image(filename, size);
      break;

    default:
      fatal("image mode not implemented yet");
  }
}

void convert_image(int newimgmode, Bit64u newsize)
{
  device_image_t *source_image, *dest_image;
  int mode = -1;
  Bit64u i, sc, s;
  char buffer[512], null_sector[512];
  bx_bool error = 0;

  printf("\n");
  memset(null_sector, 0, 512);
  if (newsize == 0) {
    if (!strncmp(bx_filename_1, "concat:", 7)) {
      mode = BX_HDIMAGE_MODE_CONCAT;
      strcpy(bx_filename_1, &bx_filename_1[7]);
#ifdef WIN32
    } else if (!strncmp(bx_filename_1, "dll:", 4)) {
      mode = BX_HDIMAGE_MODE_DLL_HD;
      strcpy(bx_filename_1, &bx_filename_1[4]);
#endif
    }
  }
  if (access(bx_filename_1, F_OK) < 0) {
    fatal("source disk image doesn't exist");
  }
  if (mode == -1) {
    mode = hdimage_detect_image_mode(bx_filename_1);
  }
  if (mode == BX_HDIMAGE_MODE_UNKNOWN) {
    fatal("source disk image mode not detected");
  } else {
    printf("source image mode = '%s'\n", hdimage_mode_names[mode]);
  }

  source_image = init_image(mode);
  if (source_image->open(bx_filename_1, O_RDONLY) < 0)
    fatal("cannot open source disk image");

  if (newsize > 0) {
    create_hard_disk_image(bx_filename_2, newimgmode, newsize);
  } else {
    create_hard_disk_image(bx_filename_2, newimgmode, source_image->hd_size);
  }
  dest_image = init_image(newimgmode);
  if (dest_image->open(bx_filename_2) < 0)
    fatal("cannot open destination disk image");

  printf("\nConverting image file: [  0%%]");

  sc = source_image->hd_size / 512;
  s = 0;
  for (i = 0; i < source_image->hd_size; i+=512) {
    printf("\x8\x8\x8\x8\x8%3d%%]", (int)((s+1)*100/sc));
    fflush(stdout);
    if (source_image->lseek(i, SEEK_SET) < 0) {
      error = 1;
      break;
    }
    if ((source_image->read(buffer, 512) == 512) &&
        (memcmp(buffer, null_sector, 512) != 0)) {
      if (dest_image->lseek(i, SEEK_SET) < 0) {
        error = 1;
        break;
      }
      if (dest_image->write(buffer, 512) < 0) {
        error = 1;
        break;
      }
    }
    s++;
  }

  source_image->close();
  dest_image->close();
  delete dest_image;
  delete source_image;

  if (error) {
    fatal("image conversion failed");
  } else {
    printf(" Done.\n");
  }
}

void commit_redolog()
{
  device_image_t *base_image;
  redolog_t *redolog;
  int ret;

  printf("\n");
  if (access(bx_filename_1, F_OK) < 0) {
    fatal("base disk image doesn't exist");
  }
  int mode = hdimage_detect_image_mode(bx_filename_1);
  if (mode == BX_HDIMAGE_MODE_UNKNOWN)
    fatal("base disk image mode not detected");

  base_image = init_image(mode);
  if (base_image->open(bx_filename_1) < 0)
    fatal("cannot open base disk image");

  redolog = new redolog_t;
  if (redolog->open(bx_filename_2, REDOLOG_SUBTYPE_UNDOABLE) < 0)
    fatal("Can't open redolog");
  if (!coherency_check(base_image, redolog))
    fatal("coherency check failed");

  ret = redolog->commit(base_image);

  base_image->close();
  redolog->close();
  delete base_image;
  delete redolog;

  if (ret < 0) {
    fatal("redolog commit failed");
  } else {
    printf(" Done.\n\n");
  }
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bximage [options] [filename1] [filename2]\n\n"
    "Supported options:\n"
    "  -mode=...     operation mode (create, convert, resize, commit, info)\n"
    "  -fd=...       create: floppy image with size code\n"
    "  -hd=...       create/resize: hard disk image with size in megabytes (M)\n"
    "                or gigabytes (G)\n"
    "  -imgmode=...  create/convert: hard disk image mode\n"
    "  -b            convert/resize: create a backup of the source image\n"
    "                commit: create backups of the base image and redolog file\n"
    "  -q            quiet mode (don't prompt for user input)\n"
    "  --help        display this help and exit\n\n"
    "Other arguments:\n"
    "  filename1     create:  new image file\n"
    "                convert/resize: source image file\n"
    "                commit:  base image file\n"
    "  filename2     convert/resize: destination image file\n"
    "                commit:  redolog (journal) file\n\n");
}

void set_default_values()
{
  if (bximage_mode == BXIMAGE_MODE_CREATE_IMAGE) {
    if (bx_hdimage == -1) {
      bx_hdimage = 1;
      bx_fdsize_idx = 6;
      bx_interactive = 1;
    }
    if (bx_hdimage == 1) {
      if (bx_imagemode == -1) {
        bx_imagemode = 0;
        bx_interactive = 1;
      }
      if (bx_hdsize == 0) {
        bx_hdsize = 10;
        bx_interactive = 1;
      }
    } else {
      if (bx_fdsize_idx == -1) {
        bx_fdsize_idx = 6;
        bx_interactive = 1;
      }
    }
  } else if (bximage_mode == BXIMAGE_MODE_CONVERT_IMAGE) {
    if (bx_imagemode == -1) {
      bx_imagemode = 0;
      bx_interactive = 1;
    }
  } else if (bximage_mode == BXIMAGE_MODE_RESIZE_IMAGE) {
    if (bx_hdsize == 0) {
      bx_interactive = 1;
    }
  }
}

int parse_cmdline(int argc, char *argv[])
{
  int arg = 1;
  int ret = 1;
  int fnargs = 0;
  char *suffix;

  bximage_mode = BXIMAGE_MODE_NULL;
  bx_hdimage = -1;
  bx_fdsize_idx = -1;
  bx_hdsize = 0;
  bx_imagemode = -1;
  bx_backup = 0;
  bx_interactive = 1;
  bx_filename_1[0] = 0;
  bx_filename_2[0] = 0;
  while ((arg < argc) && (ret == 1)) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("/?", argv[arg], 2)) {
      print_usage();
      ret = 0;
    }
    else if (!strncmp("-mode=", argv[arg], 6)) {
      if (bximage_mode != BXIMAGE_MODE_NULL) {
        printf("bximage mode already defined\n\n");
        ret = 0;
      } else if (!strcmp(&argv[arg][6], "create")) {
        bximage_mode = BXIMAGE_MODE_CREATE_IMAGE;
      } else if (!strcmp(&argv[arg][6], "convert")) {
        bximage_mode = BXIMAGE_MODE_CONVERT_IMAGE;
      } else if (!strcmp(&argv[arg][6], "resize")) {
        bximage_mode = BXIMAGE_MODE_RESIZE_IMAGE;
      } else if (!strcmp(&argv[arg][6], "commit")) {
        bximage_mode = BXIMAGE_MODE_COMMIT_UNDOABLE;
      } else if (!strcmp(&argv[arg][6], "info")) {
        bximage_mode = BXIMAGE_MODE_IMAGE_INFO;
      } else {
        printf("Unknown bximage mode '%s'\n\n", &argv[arg][6]);
        ret = 0;
      }
    }
    else if (!strncmp("-fd=", argv[arg], 4)) {
      bx_hdimage = 0;
      bx_fdsize_idx = get_menu_index(&argv[arg][4], fdsize_n_choices, fdsize_choices);
      if (bx_fdsize_idx < 0) {
        printf("Unknown floppy image size: %s\n\n", &argv[arg][6]);
        ret = 0;
      }
    }
    else if (!strncmp("-hd=", argv[arg], 4)) {
      bx_hdimage = 1;
      bx_imagemode = 0;
      bx_hdsize = (int)strtol(&argv[arg][4], &suffix, 10);
      if (bx_hdsize <= 0) {
        printf("Error in hard disk image size argument: %s\n\n", &argv[arg][4]);
        ret = 0;
      } else if (!strcmp(suffix, "G")) {
        bx_hdsize <<= 10;
      } else if ((strlen(suffix) > 0) && strcmp(suffix, "M")) {
        printf("Error in hard disk image size suffix: %s\n\n", &argv[arg][4]);
        ret = 0;
      }
      if ((bx_hdsize < bx_min_hd_megs) || (bx_hdsize > bx_max_hd_megs)) {
        printf("Hard disk image size out of range\n\n");
        ret = 0;
      }
    }
    else if (!strncmp("-imgmode=", argv[arg], 9)) {
      bx_imagemode = get_menu_index(&argv[arg][9], hdmode_n_choices, hdmode_choices);
      if (bx_imagemode < 0) {
        printf("Unknown image mode: %s\n\n", &argv[arg][9]);
        ret = 0;
      }
    }
    else if (!strcmp("-b", argv[arg])) {
      bx_backup = 1;
    }
    else if (!strcmp("-q", argv[arg])) {
      bx_interactive = 0;
    }
    else if (argv[arg][0] == '-') {
      printf("Unknown option: %s\n\n", argv[arg]);
      ret = 0;
    } else {
      if (fnargs == 0) {
        strcpy(bx_filename_1, argv[arg]);
      } else if (fnargs == 1) {
        strcpy(bx_filename_2, argv[arg]);
      } else {
        printf("Ignoring extra parameter: %s\n\n", argv[arg]);
      }
      fnargs++;
    }
    arg++;
  }
  if (bximage_mode == BXIMAGE_MODE_NULL) {
    bx_interactive = 1;
  } else {
    set_default_values();
    if (fnargs < 1) {
      bx_interactive = 1;
    }
    if ((bximage_mode == BXIMAGE_MODE_COMMIT_UNDOABLE) && (fnargs == 1)) {
      snprintf(bx_filename_2, 256, "%s%s", bx_filename_1, UNDOABLE_REDOLOG_EXTENSION);
    }
  }
  return ret;
}

void image_overwrite_check(const char *filename)
{
  char buffer[512];

  int fd = hdimage_open_file(filename, O_RDONLY, NULL, NULL);
  if (fd >= 0) {
    close(fd);
    int confirm;
    sprintf(buffer, "\nThe disk image '%s' already exists.  Are you sure you want to replace it?\nPlease type yes or no. ", filename);
    if (ask_yn(buffer, 0, &confirm) < 0)
      fatal(EOF_ERR);
    if (!confirm)
      fatal("ERROR: Aborted");
  }
}

void check_image_names()
{
  char backup_fname[512];

  if (!strlen(bx_filename_2)) {
    strcpy(bx_filename_2, bx_filename_1);
  }
  if ((!strcmp(bx_filename_1, bx_filename_2)) && bx_backup) {
    snprintf(backup_fname, 256, "%s%s", bx_filename_1, ".orig");
    if (hdimage_copy_file(bx_filename_1, backup_fname) != 1)
      fatal("backup of source image file failed");
  } else if (strcmp(bx_filename_1, bx_filename_2)) {
    image_overwrite_check(bx_filename_2);
  }
}

int get_image_mode_and_hdsize(const char *filename, int *hdsize_megs)
{
  device_image_t *source_image;

  int imgmode = hdimage_detect_image_mode(filename);
  if (imgmode == BX_HDIMAGE_MODE_UNKNOWN)
    fatal("source disk image mode not detected");
  source_image = init_image(imgmode);
  if (source_image->open(bx_filename_1, O_RDONLY) < 0) {
    fatal("cannot open source disk image");
  } else {
    *hdsize_megs = (int)(source_image->hd_size >> 20);
    source_image->close();
  }
  return imgmode;
}

int CDECL main(int argc, char *argv[])
{
  char bochsrc_line[256], prompt[80], tmpfname[512];
  int imgmode = 0;
  Bit64u hdsize = 0;
  device_image_t *hdimage;

  if (!parse_cmdline(argc, argv))
    myexit(1);

  print_banner();

  if (bx_interactive) {
    if (ask_int(main_menu_prompt, 0, 5, bximage_mode, &bximage_mode) < 0)
      fatal(EOF_ERR);

    set_default_values();
    switch (bximage_mode) {
      case BXIMAGE_MODE_NULL:
        myexit(0);
        break;

      case BXIMAGE_MODE_CREATE_IMAGE:
        printf("\nCreate image\n");
        if (ask_menu(fdhd_menu, fdhd_n_choices, fdhd_choices, bx_hdimage, &bx_hdimage) < 0)
          fatal(EOF_ERR);
        if (bx_hdimage == 0) { // floppy
          if (ask_menu(fdsize_menu, fdsize_n_choices, fdsize_choices, bx_fdsize_idx, &bx_fdsize_idx) < 0)
            fatal(EOF_ERR);
          if (!strlen(bx_filename_1)) {
            strcpy(bx_filename_1, "a.img");
          }
          if (ask_string("\nWhat should be the name of the image?\n", bx_filename_1, bx_filename_1) < 0)
            fatal(EOF_ERR);
        } else { // hard disk
          if (ask_menu(hdmode_menu, hdmode_n_choices, hdmode_choices, bx_imagemode, &bx_imagemode) < 0)
            fatal(EOF_ERR);
          sprintf(prompt, "\nEnter the hard disk size in megabytes, between %d and %d\n",
                  bx_min_hd_megs, bx_max_hd_megs);
          if (ask_int(prompt, bx_min_hd_megs, bx_max_hd_megs, bx_hdsize, &bx_hdsize) < 0)
            fatal(EOF_ERR);
          if (!strlen(bx_filename_1)) {
            strcpy(bx_filename_1, "c.img");
          }
          if (ask_string("\nWhat should be the name of the image?\n", bx_filename_1, bx_filename_1) < 0)
            fatal(EOF_ERR);
        }
        break;

      case BXIMAGE_MODE_CONVERT_IMAGE:
        printf("\nConvert image\n");
        if (!strlen(bx_filename_1)) {
          strcpy(bx_filename_1, "c.img");
        }
        if (ask_string("\nWhat is the name of the source image?\n", bx_filename_1, bx_filename_1) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_filename_2)) {
          strcpy(tmpfname, bx_filename_1);
        } else {
          strcpy(tmpfname, bx_filename_2);
        }
        if (ask_string("\nWhat should be the name of the new image?\n", tmpfname, bx_filename_2) < 0)
          fatal(EOF_ERR);
        if (ask_menu(hdmode_menu, hdmode_n_choices, hdmode_choices, bx_imagemode, &bx_imagemode) < 0)
          fatal(EOF_ERR);
        if (!strcmp(bx_filename_1, bx_filename_2)) {
          if (ask_yn("\nShould I create a backup of the source image\n", 0, &bx_backup) < 0)
            fatal(EOF_ERR);
        }
        break;

      case BXIMAGE_MODE_RESIZE_IMAGE:
        printf("\nResize image\n");
        if (!strlen(bx_filename_1)) {
          strcpy(bx_filename_1, "c.img");
        }
        if (ask_string("\nWhat is the name of the source image?\n", bx_filename_1, bx_filename_1) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_filename_2)) {
          strcpy(tmpfname, bx_filename_1);
        } else {
          strcpy(tmpfname, bx_filename_2);
        }
        if (ask_string("\nWhat should be the name of the new image?\n", tmpfname, bx_filename_2) < 0)
          fatal(EOF_ERR);
        imgmode = get_image_mode_and_hdsize(bx_filename_1, &bx_min_hd_megs);
        sprintf(prompt, "\nEnter the new hard disk size in megabytes, between %d and %d\n",
                bx_min_hd_megs, bx_max_hd_megs);
        if (bx_hdsize < bx_min_hd_megs) bx_hdsize = bx_min_hd_megs;
        if (ask_int(prompt, bx_min_hd_megs, bx_max_hd_megs, bx_hdsize, &bx_hdsize) < 0)
          fatal(EOF_ERR);
        if (!strcmp(bx_filename_1, bx_filename_2)) {
          if (ask_yn("\nShould I create a backup of the source image\n", 0, &bx_backup) < 0)
            fatal(EOF_ERR);
        }
        break;

      case BXIMAGE_MODE_COMMIT_UNDOABLE:
        printf("\nCommit redolog\n");
        if (!strlen(bx_filename_1)) {
          strcpy(bx_filename_1, "c.img");
        }
        if (ask_string("\nWhat is the name of the base image?\n", bx_filename_1, bx_filename_1) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_filename_2)) {
          snprintf(tmpfname, 256, "%s%s", bx_filename_1, UNDOABLE_REDOLOG_EXTENSION);
        } else {
          strcpy(tmpfname, bx_filename_2);
        }
        if (ask_string("\nWhat is the redolog name?\n", tmpfname, bx_filename_2) < 0)
          fatal(EOF_ERR);
        if (ask_yn("\nShould I create a backup of base image and redolog\n", 0, &bx_backup) < 0)
          fatal(EOF_ERR);
        break;

      case BXIMAGE_MODE_IMAGE_INFO:
        printf("\nDisk image info\n");
        if (!strlen(bx_filename_1)) {
          strcpy(bx_filename_1, "c.img");
        }
        if (ask_string("\nWhat is the name of the image?\n", bx_filename_1, bx_filename_1) < 0)
          fatal(EOF_ERR);
        break;

      default:
        fatal("\nbximage: unknown mode");
    }
  }
  switch (bximage_mode) {
      case BXIMAGE_MODE_CREATE_IMAGE:
        image_overwrite_check(bx_filename_1);
        if (bx_hdimage == 0) {
          sprintf(bochsrc_line, "floppya: image=\"%s\", status=inserted", bx_filename_1);
          printf("\nCreating floppy image '%s' with %d sectors\n", bx_filename_1, fdsize_sectors[bx_fdsize_idx]);
          create_flat_image(bx_filename_1, fdsize_sectors[bx_fdsize_idx] * 512);
        } else {
          int heads = 16, spt = 63;

          sprintf(bochsrc_line, "ata0-master: type=disk, path=\"%s\", mode=%s", bx_filename_1, hdmode_choices[bx_imagemode]);
          imgmode = hdmode_choice_id[bx_imagemode];
          hdsize = ((Bit64u)bx_hdsize) << 20;
          Bit64u cyl = (Bit64u)(hdsize/16.0/63.0/512.0);
          if (cyl >= (1 << BX_MAX_CYL_BITS))
            fatal("ERROR: number of cylinders out of range !\n");
          printf("\nCreating hard disk image '%s' with CHS="FMT_LL"d/%d/%d\n",
                 bx_filename_1, cyl, heads, spt);
          hdsize = cyl * heads * spt * 512;
          create_hard_disk_image(bx_filename_1, imgmode, hdsize);
        }
        printf("\nThe following line should appear in your bochsrc:\n");
        printf("  %s\n", bochsrc_line);
#ifdef WIN32
        if (OpenClipboard(NULL)) {
          HGLOBAL hgClip;
          EmptyClipboard();
          hgClip = GlobalAlloc(GMEM_DDESHARE, (strlen(bochsrc_line) + 1));
          strcpy((char *)GlobalLock(hgClip), bochsrc_line);
          GlobalUnlock(hgClip);
          SetClipboardData(CF_TEXT, hgClip);
          CloseClipboard();
          printf("(The line is stored in your windows clipboard, use CTRL-V to paste)\n");
        }
#endif
        break;

      case BXIMAGE_MODE_CONVERT_IMAGE:
        check_image_names();
        imgmode = hdmode_choice_id[bx_imagemode];
        convert_image(imgmode, 0);
        break;

      case BXIMAGE_MODE_RESIZE_IMAGE:
        imgmode = get_image_mode_and_hdsize(bx_filename_1, &bx_min_hd_megs);
        if (bx_hdsize < bx_min_hd_megs)
          fatal("invalid image size");
        hdsize = ((Bit64u)bx_hdsize) << 20;
        check_image_names();
        convert_image(imgmode, hdsize);
        break;

      case BXIMAGE_MODE_COMMIT_UNDOABLE:
        if (bx_backup) {
          snprintf(tmpfname, 256, "%s%s", bx_filename_1, ".orig");
          if (hdimage_copy_file(bx_filename_1, tmpfname) != 1)
            fatal("backup of base image file failed");
        }
        commit_redolog();
        if (bx_backup) {
          snprintf(tmpfname, 256, "%s%s", bx_filename_2, ".orig");
          if (rename(bx_filename_2, tmpfname) != 0)
            fatal("rename of redolog file failed");
        } else {
          if (unlink(bx_filename_2) != 0)
            fatal("ERROR: while removing the redolog !\n");
        }
        break;

      case BXIMAGE_MODE_IMAGE_INFO:
        imgmode = hdimage_detect_image_mode(bx_filename_1);
        if (imgmode == BX_HDIMAGE_MODE_UNKNOWN) {
          fatal("disk image mode not detected");
        } else {
          printf("\ndisk image mode = '%s'\n", hdimage_mode_names[imgmode]);
        }
        hdimage = init_image(imgmode);
        if (hdimage->open(bx_filename_1, O_RDONLY) < 0) {
          fatal("cannot open source disk image");
        } else {
          if (hdimage->get_capabilities() & HDIMAGE_AUTO_GEOMETRY) {
            Bit64u cyl = (Bit64u)(hdimage->hd_size/16.0/63.0/512.0);
            printf("geometry = "FMT_LL"d/16/63 ("FMT_LL"d MB)\n\n", cyl,
                   hdimage->hd_size >> 20);
          } else {
            printf("geometry = %d/%d/%d ("FMT_LL"d MB)\n\n", hdimage->cylinders,
                   hdimage->heads, hdimage->spt, hdimage->hd_size >> 20);
          }
          hdimage->close();
        }
        break;
  }
  myexit(0);

  return 0;
}
