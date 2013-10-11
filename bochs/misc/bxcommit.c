/*
 * $Id$
 *
 *  Copyright (C) 2003-2013  The Bochs Project
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Commits a redolog file to a 'flat' or 'growing' mode base image. */
/* Converts growing mode image to flat and vice versa */

#ifdef WIN32
#  include <conio.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "config.h"

#include <string.h>

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

#include "../osdep.h"
#include "bswap.h"

#define HDIMAGE_HEADERS_ONLY 1
#include "../iodev/hdimage/hdimage.h"

#define BXCOMMIT_MODE_COMMIT_UNDOABLE 1
#define BXCOMMIT_MODE_GROWING_TO_FLAT 2
#define BXCOMMIT_MODE_FLAT_TO_GROWING 3

#define BXCOMMIT_BASE_FLAT    1
#define BXCOMMIT_BASE_GROWING 2

typedef struct {
  int              fd;
  redolog_header_t header;
  Bit32u          *catalog;
  Bit8u           *bitmap;
  bx_bool          bitmap_update;
  Bit32u           extent_index;
  Bit32u           extent_offset;
  Bit32u           extent_next;

  Bit32u           bitmap_blocks;
  Bit32u           extent_blocks;

  Bit64s           imagepos;
} redolog_t;

int  bxcommit_mode;
int  bx_remove;
int  bx_interactive;
char bx_flat_filename[256];
char bx_redolog_name[256];
Bit8u null_sector[512];

const char *EOF_ERR = "ERROR: End of input";
const char *svnid = "$Id$";
const char *divider = "========================================================================";
const char *main_menu_prompt =
"\n"
"1. Commit 'undoable' redolog to 'flat' or 'growing' mode base image\n"
"2. Create 'flat' disk image from 'growing' disk image\n"
"3. Create 'growing' disk image from 'flat' disk image\n"
"\n"
"0. Quit\n"
"\n"
"Please choose one ";

void myexit(int code)
{
#ifdef WIN32
  printf("\nPress any key to continue\n");
  getch();
#endif
  exit(code);
}

/* stolen from main.cc */
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
  bx_center_print(stdout, "bxcommit\n", 72);
  bx_center_print(stdout, "Growing / Undoable Disk Image Commit Tool for Bochs\n", 72);
  bx_center_print(stdout, svnid, 72);
  printf("\n%s\n", divider);
}

/* this is how we crash */
void fatal(const char *c)
{
  printf("%s\n", c);
  myexit(1);
}

/* remove leading spaces, newline junk at end.  returns pointer to
 cleaned string, which is between s0 and the null */
char *clean_string(char *s0)
{
  char *s = s0;
  char *ptr;
  /* find first nonblank */
  while (isspace(*s))
    s++;
  /* truncate string at first non-alphanumeric */
  ptr = s;
  while (isprint(*ptr))
    ptr++;
  *ptr = 0;
  return s;
}

/* returns 0 on success, -1 on failure.  The value goes into out. */
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
      printf("Your choice (%s) was not an integer between %d and %d.\n\n",
             clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
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
    printf("Please type either yes or no.\n");
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

/* helper functions */
int bx_read_image(int fd, Bit64s offset, void *buf, int count)
{
  if (lseek(fd, offset, SEEK_SET) == -1) {
    return -1;
  }
  return read(fd, buf, count);
}

int bx_write_image(int fd, Bit64s offset, void *buf, int count)
{
  if (lseek(fd, offset, SEEK_SET) == -1) {
    return -1;
  }
  return write(fd, buf, count);
}

/* Read one sector and check if empty */
int flat_image_read_sector(int fd, Bit8u *buffer, Bit64u offset)
{
  if (bx_read_image(fd, offset, buffer, 512) < 0)
    return 0;
  if (memcmp(buffer, null_sector, 512) == 0)
    return 0;

  return 1;
}

/* Create a suited redolog header */
void redolog_make_header(redolog_header_t *header, const char* type, Bit64u size)
{
  Bit32u entries, extent_size, bitmap_size;
  Bit64u maxsize;

  // Set standard header values
  strcpy((char*)header->standard.magic, STANDARD_HEADER_MAGIC);
  strcpy((char*)header->standard.type, REDOLOG_TYPE);
  strcpy((char*)header->standard.subtype, type);
  header->standard.version = htod32(STANDARD_HEADER_VERSION);
  header->standard.header = htod32(STANDARD_HEADER_SIZE);

  entries = 512;
  bitmap_size = 1;

  // Compute #entries and extent size values
  do {
     static Bit32u flip=0;

     extent_size = 8 * bitmap_size * 512;

     header->specific.catalog = htod32(entries);
     header->specific.bitmap = htod32(bitmap_size);
     header->specific.extent = htod32(extent_size);

     maxsize = (Bit64u)entries * (Bit64u)extent_size;

     flip++;

     if(flip&0x01) bitmap_size *= 2;
     else entries *= 2;
  } while (maxsize < size);

  header->specific.disk = htod64(size);
}

/* Redolog functions */

int redolog_open(redolog_t *r, int fd, redolog_header_t *header)
{
  int i, res;

  memset(r, 0, sizeof(redolog_t));
  r->fd = fd;
  memcpy(&r->header, header, sizeof(redolog_header_t));
  r->catalog = (Bit32u*)malloc(dtoh32(r->header.specific.catalog) * sizeof(Bit32u));
  printf("\nReading Catalog: [");
  res = bx_read_image(r->fd, dtoh32(r->header.standard.header),
                      r->catalog, dtoh32(r->header.specific.catalog) * sizeof(Bit32u));
  if (res != (ssize_t)(dtoh32(r->header.specific.catalog) * sizeof(Bit32u))) {
    fatal("ERROR: redolog : could not read catalog");
  }
  printf("...] Done.");
  r->extent_next = 0;
  for (i = 0; i < dtoh32(r->header.specific.catalog); i++) {
    if (dtoh32(r->catalog[i]) != REDOLOG_PAGE_NOT_ALLOCATED) {
      if (dtoh32(r->catalog[i]) >= r->extent_next)
        r->extent_next = dtoh32(r->catalog[i]) + 1;
    }
  }
  r->bitmap = (Bit8u *)malloc(dtoh32(r->header.specific.bitmap));
  r->bitmap_blocks = 1 + (dtoh32(r->header.specific.bitmap) - 1) / 512;
  r->extent_blocks = 1 + (dtoh32(r->header.specific.extent) - 1) / 512;
  r->bitmap_update = 1;
  return 0;
}

int redolog_close(redolog_t *r)
{
  if (r->fd >= 0)
    close(r->fd);

  if (r->catalog != NULL)
    free(r->catalog);

  if (r->bitmap != NULL)
    free(r->bitmap);
  return 0;
}

int redolog_check_format(int fd, const char *subtype, redolog_header_t *header)
{
  redolog_header_t temp_header;

  int res = bx_read_image(fd, 0, &temp_header, sizeof(redolog_header_t));
  if (res != STANDARD_HEADER_SIZE) {
    return HDIMAGE_READ_ERROR;
  }

  if (strcmp((char*)temp_header.standard.magic, STANDARD_HEADER_MAGIC) != 0) {
    return HDIMAGE_NO_SIGNATURE;
  }

  if (strcmp((char*)temp_header.standard.type, REDOLOG_TYPE) != 0) {
    return HDIMAGE_TYPE_ERROR;
  }
  if (strcmp((char*)temp_header.standard.subtype, subtype) != 0) {
    return HDIMAGE_TYPE_ERROR;
  }

  if (dtoh32(temp_header.standard.version) != STANDARD_HEADER_VERSION) {
    return HDIMAGE_VERSION_ERROR;
  }
  memcpy(header, &temp_header, sizeof(redolog_header_t));
  return HDIMAGE_FORMAT_OK;
}

Bit64s redolog_lseek(redolog_t *redolog, Bit64s offset, int whence)
{
  char msg[80];
  Bit32u old_extent_index;

  if (whence == SEEK_SET) {
    redolog->imagepos = offset;
  } else if (whence == SEEK_CUR) {
    redolog->imagepos += offset;
  } else {
    fatal("redolog: lseek() mode not supported yet");
    return -1;
  }
  if (redolog->imagepos > (Bit64s)dtoh64(redolog->header.specific.disk)) {
    sprintf(msg, "redolog : lseek() to byte %ld failed", (long)offset);
    fatal(msg);
    return -1;
  }

  old_extent_index = redolog->extent_index;
  redolog->extent_index = (Bit32u)(redolog->imagepos / dtoh32(redolog->header.specific.extent));
  if (redolog->extent_index != old_extent_index) {
    redolog->bitmap_update = 1;
  }
  redolog->extent_offset = (Bit32u)((redolog->imagepos % dtoh32(redolog->header.specific.extent)) / 512);

  return redolog->imagepos;
}

int redolog_write_sector(redolog_t *redolog, Bit8u *buf, Bit64u offset)
{
  Bit32u i;
  Bit64s block_offset, bitmap_offset, catalog_offset;
  ssize_t written;
  bx_bool update_catalog = 0;
  char *zerobuffer;
  char msg[80];

  redolog_lseek(redolog, offset, SEEK_SET);

  if (dtoh32(redolog->catalog[redolog->extent_index]) == REDOLOG_PAGE_NOT_ALLOCATED) {
    if (redolog->extent_next >= dtoh32(redolog->header.specific.catalog)) {
      fatal("redolog : can't allocate new extent... catalog is full");
      return -1;
    }

    // Extent not allocated, allocate new
    redolog->catalog[redolog->extent_index] = htod32(redolog->extent_next);

    redolog->extent_next += 1;

    zerobuffer = (char*)malloc(512);
    memset(zerobuffer, 0, 512);

    // Write bitmap
    bitmap_offset  = (Bit64s)STANDARD_HEADER_SIZE + (dtoh32(redolog->header.specific.catalog) * sizeof(Bit32u));
    bitmap_offset += (Bit64s)512 * dtoh32(redolog->catalog[redolog->extent_index]) * (redolog->extent_blocks + redolog->bitmap_blocks);
    lseek(redolog->fd, (off_t)bitmap_offset, SEEK_SET);
    for (i=0; i<redolog->bitmap_blocks; i++) {
      write(redolog->fd, zerobuffer, 512);
    }
    // Write extent
    for (i=0; i<redolog->extent_blocks; i++) {
      write(redolog->fd, zerobuffer, 512);
    }

    free(zerobuffer);

    update_catalog = 1;
  }

  bitmap_offset  = (Bit64s)STANDARD_HEADER_SIZE + (dtoh32(redolog->header.specific.catalog) * sizeof(Bit32u));
  bitmap_offset += (Bit64s)512 * dtoh32(redolog->catalog[redolog->extent_index]) * (redolog->extent_blocks + redolog->bitmap_blocks);
  block_offset    = bitmap_offset + ((Bit64s)512 * (redolog->bitmap_blocks + redolog->extent_offset));

  // Write block
  written = bx_write_image(redolog->fd, (off_t)block_offset, (void*)buf, 512);

  // Write bitmap
  if (redolog->bitmap_update) {
    if (bx_read_image(redolog->fd, (off_t)bitmap_offset, redolog->bitmap,  dtoh32(redolog->header.specific.bitmap)) != (ssize_t)dtoh32(redolog->header.specific.bitmap)) {
      sprintf(msg, "redolog : failed to read bitmap for extent %d", redolog->extent_index);
      fatal(msg);
      return 0;
    }
    redolog->bitmap_update = 0;
  }

  // If block does not belong to extent yet
  if (((redolog->bitmap[redolog->extent_offset/8] >> (redolog->extent_offset%8)) & 0x01) == 0x00) {
    redolog->bitmap[redolog->extent_offset/8] |= 1 << (redolog->extent_offset%8);
    bx_write_image(redolog->fd, (off_t)bitmap_offset, redolog->bitmap,  dtoh32(redolog->header.specific.bitmap));
  }

  // Write catalog
  if (update_catalog) {
    // FIXME if mmap
    catalog_offset  = (Bit64s)STANDARD_HEADER_SIZE + (redolog->extent_index * sizeof(Bit32u));

    bx_write_image(redolog->fd, (off_t)catalog_offset, &redolog->catalog[redolog->extent_index], sizeof(Bit32u));
  }

  if (written >= 0) redolog_lseek(redolog, 512, SEEK_CUR);

  return written;
}

int convert_flat_image()
{
  int flatfd, redologfd;
  Bit64u disk_size, offset;
  redolog_t redolog;
  Bit32u i;
  struct stat stat_buf;
  Bit8u buffer[512];

  if (bxcommit_mode == BXCOMMIT_MODE_FLAT_TO_GROWING) {
    // check if flat file exists
    flatfd = open(bx_flat_filename, O_RDONLY
#ifdef O_BINARY
                  | O_BINARY
#endif
                  );
    if (flatfd < 0) {
      fatal("ERROR: flat file not found or not readable");
    }
    if (fstat(flatfd, &stat_buf)) {
      fatal("fstat() returns error!");
    }
    disk_size = (Bit64u)stat_buf.st_size;
    memset(null_sector, 0, 512);

    redologfd = open(bx_redolog_name, O_RDONLY
#ifdef O_BINARY
                     | O_BINARY
#endif
                     );
    if (redologfd >= 0) {
      close(flatfd);
      close(redologfd);
      fatal("ERROR: growing file already exists");
    }
    fprintf(stderr, "\nCreating growing image file: [");
    memset(&redolog, 0, sizeof(redolog_t));
    redolog.fd = open(bx_redolog_name, O_RDWR | O_CREAT
#ifdef O_BINARY
                      | O_BINARY
#endif
                      , S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP
                      );
    if (redolog.fd < 0) {
      close(flatfd);
      fatal("ERROR: redolog file is not writable");
    }
    redolog_make_header(&redolog.header, REDOLOG_SUBTYPE_GROWING, disk_size);
    if (write(redolog.fd, &redolog.header, sizeof(redolog.header)) < 0) {
      fatal("ERROR: The disk image is not complete - could not write header!");
    } else {
      lseek(redolog.fd, sizeof(redolog.header), SEEK_SET);
    }
    redolog.catalog = (Bit32u*)malloc(dtoh32(redolog.header.specific.catalog) * sizeof(Bit32u));
    for (i=0; i<dtoh32(redolog.header.specific.catalog); i++) {
      redolog.catalog[i] = htod32(REDOLOG_PAGE_NOT_ALLOCATED);
    }
    if (bx_write_image(redolog.fd, dtoh32(redolog.header.standard.header), redolog.catalog,
                       dtoh32(redolog.header.specific.catalog) * sizeof(Bit32u)) < 0) {
      close(redolog.fd);
      fatal("ERROR: The disk image is not complete - could not write catalog!");
    }
    redolog.bitmap = (Bit8u *)malloc(dtoh32(redolog.header.specific.bitmap));
    redolog.bitmap_blocks = 1 + (dtoh32(redolog.header.specific.bitmap) - 1) / 512;
    redolog.extent_blocks = 1 + (dtoh32(redolog.header.specific.extent) - 1) / 512;
    redolog.bitmap_update = 1;

    offset = 0;
    while (offset < disk_size) {
      if (flat_image_read_sector(flatfd, buffer, offset) > 0) {
        if (redolog_write_sector(&redolog, buffer, offset) <= 0)
          break;
      }
      offset += 512;
      if (((offset >> 9) % (disk_size >> 15)) == 0) fprintf(stderr, ".");
    }
    fprintf(stderr, "]\n");
  } else {
    fatal("ERROR: unknown / unsupported mode");
  }

  close(flatfd);
  close(redolog.fd);

  return 0;
}

/* produce the image file */
int commit_redolog()
{
  int flatfd = -1, redologfd, res = -1;
  int destmode = BXCOMMIT_BASE_FLAT;
  redolog_header_t header;
  redolog_t r1, r2;
  Bit32u i;
  Bit8u  buffer[512];

  if (bxcommit_mode == BXCOMMIT_MODE_COMMIT_UNDOABLE) {
    // check if flat file exists
    flatfd = open(bx_flat_filename, O_RDWR
#ifdef O_BINARY
                  | O_BINARY
#endif
                  );
    if (flatfd < 0) {
      fatal("ERROR: flat file not found or not writable");
    }
    printf("\nMode of base image: ");
    res = redolog_check_format(flatfd, REDOLOG_SUBTYPE_GROWING, &header);
    if (res == HDIMAGE_FORMAT_OK) {
      printf("'growing'\n");
      redolog_open(&r1, flatfd, &header);
      destmode = BXCOMMIT_BASE_GROWING;
    } else {
      printf("'flat'\n");
    }
  }

  // Check if redolog exists
  printf("\nOpening '%s'\n", bx_redolog_name);
  redologfd = open(bx_redolog_name, O_RDONLY
#ifdef O_BINARY
                   | O_BINARY
#endif
                   );
  if (redologfd < 0) {
    fatal("ERROR: redolog file not found");
  }

  printf("\nReading redolog header: [");

  if (bxcommit_mode == BXCOMMIT_MODE_COMMIT_UNDOABLE) {
    res = redolog_check_format(redologfd, REDOLOG_SUBTYPE_UNDOABLE, &header);
  } else if (bxcommit_mode == BXCOMMIT_MODE_GROWING_TO_FLAT) {
    res = redolog_check_format(redologfd, REDOLOG_SUBTYPE_GROWING, &header);
  } else {
    fatal("\nERROR: unknown bxcommit mode!");
  }

  if (res != HDIMAGE_FORMAT_OK) {
    switch (res) {
      case HDIMAGE_READ_ERROR:
        fatal("\nERROR: while reading redolog header!");
        break;
      case HDIMAGE_NO_SIGNATURE:
        fatal("\nERROR: bad magic in redolog header!");
        break;
      case HDIMAGE_TYPE_ERROR:
       fatal("\nERROR: bad type or subtype in redolog header!");
        break;
      case HDIMAGE_VERSION_ERROR:
       fatal("\nERROR: bad version in redolog header!");
        break;
    }
  }

  // Print infos on redlog
  printf("Type='%s', Subtype='%s', Version=%d.%d] Done.",
         header.standard.type, header.standard.subtype,
         dtoh32(header.standard.version)/0x10000,
         dtoh32(header.standard.version)%0x10000);

  printf("\nChecking redolog header: [");

  printf("#entries=%d, bitmap size=%d, exent size = %d] Done.",
         dtoh32(header.specific.catalog),
         dtoh32(header.specific.bitmap),
         dtoh32(header.specific.extent));

  if (bxcommit_mode == BXCOMMIT_MODE_GROWING_TO_FLAT) {
    flatfd = open(bx_flat_filename, O_RDONLY
#ifdef O_BINARY
                  | O_BINARY
#endif
                  );
    if (flatfd >= 0) {
      close(flatfd);
      fatal("ERROR: flat file already exists");
    }
    printf("\nCreating flat image file: [");
    flatfd = open(bx_flat_filename, O_WRONLY | O_CREAT
#ifdef O_BINARY
                  | O_BINARY
#endif
                  , S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP
                  );
    if (flatfd < 0) {
      fatal("ERROR: flat file is not writable");
    }
    lseek(flatfd, dtoh64(header.specific.disk) - 512, SEEK_SET);
    if (write(flatfd, buffer, 512) != 512)
      fatal("ERROR: while writing block in flat file !");

    printf("...] Done.");
  }

  redolog_open(&r2, redologfd, &header);

  printf("\nCommitting changes to flat file: [  0%%]");

  for(i=0; i<dtoh32(header.specific.catalog); i++) {
    printf("\x8\x8\x8\x8\x8%3d%%]", (i+1)*100/dtoh32(header.specific.catalog));
    fflush(stdout);

    if (dtoh32(r2.catalog[i]) != REDOLOG_PAGE_NOT_ALLOCATED) {
      Bit64s bitmap_offset;
      Bit32u bitmap_size, j;

      bitmap_offset  = (Bit64s)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
      bitmap_offset += (Bit64s)512 * dtoh32(r2.catalog[i]) * (r2.extent_blocks + r2.bitmap_blocks);

      // Read bitmap
      lseek(redologfd, bitmap_offset, SEEK_SET);

      bitmap_size = dtoh32(header.specific.bitmap);
      if ((Bit32u) read(redologfd, r2.bitmap, bitmap_size) != bitmap_size)
        fatal("\nERROR: while reading bitmap from redolog !");

      for(j=0; j<dtoh32(header.specific.bitmap); j++) {
        Bit32u bit;

        for(bit=0; bit<8; bit++) {
          if ( (r2.bitmap[j] & (1<<bit)) != 0) {
            Bit64s flat_offset, block_offset;

            block_offset = bitmap_offset + ((Bit64s)512 * (r2.bitmap_blocks + ((j*8)+bit)));

            if (bx_read_image(redologfd, (off_t)block_offset, buffer, 512) != 512)
              fatal("\nERROR: while reading block from redolog !");

            flat_offset  = (Bit64s)i * (dtoh32(header.specific.extent));
            flat_offset += (Bit64s)512 * ((j * 8) + bit);

            if (destmode == BXCOMMIT_BASE_FLAT) {
              if (bx_write_image(flatfd, (off_t)flat_offset, buffer, 512) != 512)
                fatal("\nERROR: while writing block in flat file !");
            } else if (destmode == BXCOMMIT_BASE_GROWING) {
              if (redolog_write_sector(&r1, buffer, flat_offset) <= 0)
                break;
            }
          }
        }
      }
    }
  }

  printf(" Done.");
  printf("\n");

  if (destmode == BXCOMMIT_BASE_FLAT) {
    close(flatfd);
  } else if (destmode == BXCOMMIT_BASE_GROWING) {
    redolog_close(&r1);
  }
  redolog_close(&r2);

  return 0;
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bxcommit [options] [flat filename] [redolog filename]\n\n"
    "Supported options:\n"
    "  -mode=commit-undoable  commit undoable redolog to base image file\n"
    "  -mode=growing-to-flat  create flat disk image from growing disk image\n"
    "  -mode=flat-to-growing  create growing disk image from flat disk image\n"
    "  -d                     delete source file after commit\n"
    "  -q                     quiet mode (don't prompt for user input)\n"
    "  --help                 display this help and exit\n\n");
}

int parse_cmdline(int argc, char *argv[])
{
  int arg = 1;
  int ret = 1;
  int fnargs = 0;

  bxcommit_mode = 0;
  bx_remove = 0;
  bx_interactive = 1;
  bx_flat_filename[0] = 0;
  bx_redolog_name[0] = 0;
  while ((arg < argc) && (ret == 1)) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("/?", argv[arg], 2)) {
      print_usage();
      ret = 0;
    }
    else if (!strncmp("-mode=", argv[arg], 6)) {
      if (!strcmp(&argv[arg][6], "commit-undoable")) {
        bxcommit_mode = BXCOMMIT_MODE_COMMIT_UNDOABLE;
      } else if (!strcmp(&argv[arg][6], "growing-to-flat")) {
        bxcommit_mode = BXCOMMIT_MODE_GROWING_TO_FLAT;
      } else if (!strcmp(&argv[arg][6], "flat-to-growing")) {
        bxcommit_mode = BXCOMMIT_MODE_FLAT_TO_GROWING;
      } else {
        printf("Unknown bxcommit mode '%s'\n\n", &argv[arg][6]);
      }
    }
    else if (!strcmp("-d", argv[arg])) {
      bx_remove = 1;
    }
    else if (!strcmp("-q", argv[arg])) {
      bx_interactive = 0;
    }
    else if (argv[arg][0] == '-') {
      printf("Unknown option: %s\n\n", argv[arg]);
      ret = 0;
    } else {
      if (fnargs == 0) {
        strcpy(bx_flat_filename, argv[arg]);
      } else if (fnargs == 1) {
        strcpy(bx_redolog_name, argv[arg]);
      } else {
        printf("Ignoring extra parameter: %s\n\n", argv[arg]);
      }
      fnargs++;
    }
    arg++;
  }
  if ((bxcommit_mode == 0) ||
      (fnargs < 2)) {
    bx_interactive = 1;
  }
  return ret;
}

int CDECL main(int argc, char *argv[])
{
  char tmplogname[256];

  if (!parse_cmdline(argc, argv))
    myexit(1);

  print_banner();

  if (bx_interactive) {
    if (ask_int(main_menu_prompt, 0, 3, bxcommit_mode, &bxcommit_mode) < 0)
      fatal(EOF_ERR);

    switch (bxcommit_mode) {
      case 0:
        myexit(0);
        break;

      case BXCOMMIT_MODE_COMMIT_UNDOABLE:
        if (!strlen(bx_flat_filename)) {
          strcpy(bx_flat_filename, "c.img");
        }
        if (ask_string("\nWhat is the name of the base image?\n", bx_flat_filename, bx_flat_filename) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_redolog_name)) {
          snprintf(tmplogname, 256, "%s%s", bx_flat_filename, UNDOABLE_REDOLOG_EXTENSION);
        } else {
          strcpy(tmplogname, bx_redolog_name);
        }
        if (ask_string("\nWhat is the redolog name?\n", tmplogname, bx_redolog_name) < 0)
          fatal(EOF_ERR);
        if (ask_yn("\nShould the redolog been removed afterwards?\n", 1, &bx_remove) < 0)
          fatal(EOF_ERR);
        break;

      case BXCOMMIT_MODE_GROWING_TO_FLAT:
        if (!strlen(bx_redolog_name)) {
          strcpy(bx_redolog_name, "c.img");
        }
        if (ask_string("\nWhat is the name of the 'growing' image?\n", bx_redolog_name, bx_redolog_name) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_flat_filename)) {
          strcpy(bx_flat_filename, "flat.img");
        }
        if (ask_string("\nWhat should be the name of the 'flat' image?\n", bx_flat_filename, bx_flat_filename) < 0)
          fatal(EOF_ERR);
        if (ask_yn("\nShould the 'growing' image been removed afterwards?\n", 0, &bx_remove) < 0)
          fatal(EOF_ERR);
        break;

      case BXCOMMIT_MODE_FLAT_TO_GROWING:
        if (!strlen(bx_flat_filename)) {
          strcpy(bx_flat_filename, "flat.img");
        }
        if (ask_string("\nWhat is the name of the 'flat' image?\n", bx_flat_filename, bx_flat_filename) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_redolog_name)) {
          strcpy(bx_redolog_name, "c.img");
        }
        if (ask_string("\nWhat should be the 'growing' image name?\n", bx_redolog_name, bx_redolog_name) < 0)
          fatal(EOF_ERR);
        if (ask_yn("\nShould the 'flat' image been removed afterwards?\n", 0, &bx_remove) < 0)
          fatal(EOF_ERR);
        break;
    }
  }

  if (bxcommit_mode != BXCOMMIT_MODE_FLAT_TO_GROWING) {
    commit_redolog();
    if (bx_remove) {
      if (unlink(bx_redolog_name) != 0)
        fatal("ERROR: while removing the redolog !\n");
    }
  } else {
    convert_flat_image();
    if (bx_remove) {
      if (unlink(bx_flat_filename) != 0)
        fatal("ERROR: while removing the flat file !\n");
    }
  }

  // make picky compilers (c++, gcc) happy,
  // even though we leave via 'myexit' just above
  return 0;
}
