/*
 * $Id$
 *
 *  Copyright (C) 2001-2013  The Bochs Project
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

/* Create empty hard disk or floppy disk images for bochs. */

#ifndef CDECL
#if defined(_MSC_VER)
  #define CDECL __cdecl
#else
  #define CDECL
#endif
#endif

#ifdef WIN32
#ifndef __CYGWIN__
#  include <conio.h>
#endif
#  include <windows.h>
#  include <winioctl.h>
#ifdef _MSC_VER
#  include <io.h>
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "config.h"

#include <string.h>

#include "../osdep.h"
#include "bswap.h"

#define HDIMAGE_HEADERS_ONLY 1
#include "../iodev/hdimage/hdimage.h"

#define BX_MAX_CYL_BITS 24 // 8 TB

const int bx_max_hd_megs = (int)(((1 << BX_MAX_CYL_BITS) - 1) * 16.0 * 63.0 / 2048.0);

int bx_hdimage;
int bx_fdsize_idx;
int bx_hdsize;
int bx_hdimagemode;
int bx_interactive;
char bx_filename[256];

typedef int (*WRITE_IMAGE)(FILE*, Bit64u);
#ifdef WIN32
typedef int (*WRITE_IMAGE_WIN32)(HANDLE, Bit64u);
#endif

const char *EOF_ERR = "ERROR: End of input";
const char *svnid = "$Id$";
const char *divider = "========================================================================";

/* menu data for choosing floppy/hard disk */
const char *fdhd_menu = "\nDo you want to create a floppy disk image or a hard disk image?\nPlease type hd or fd. ";
const char *fdhd_choices[] = { "fd", "hd" };
int fdhd_n_choices = 2;

/* menu data for choosing floppy size */
const char *fdsize_menu = "\nChoose the size of floppy disk image to create, in megabytes.\nPlease type 0.16, 0.18, 0.32, 0.36, 0.72, 1.2, 1.44, 1.68, 1.72, or 2.88.\n ";
const char *fdsize_choices[] = { "0.16","0.18","0.32","0.36","0.72","1.2","1.44","1.68","1.72","2.88" };
int fdsize_n_choices = 10;

/* menu data for choosing disk mode */
const char *hdmode_menu = "\nWhat kind of image should I create?\nPlease type flat, sparse or growing. ";
const char *hdmode_choices[] = {"flat", "sparse", "growing" };
int hdmode_n_choices = 3;

void myexit(int code)
{
#if defined(WIN32) && !defined(__CYGWIN__)
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
  bx_center_print(stdout, "bximage\n", 72);
  bx_center_print(stdout, "Disk Image Creation Tool for Bochs\n", 72);
  bx_center_print(stdout, svnid, 72);
  printf("\n%s\n", divider);
}

/* this is how we crash */
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

/* remove leading spaces, newline junk at end.  returns pointer to
 cleaned string, which is between s0 and the null */
char *clean_string(char *s0)
{
  char *s = s0;
  char *ptr;
  /* find first nonblank */
  while(isspace(*s))
    s++;
  /* truncate string at first non-alphanumeric */
  ptr = s;
  while(isprint(*ptr))
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
  while(1) {
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
    switch(tolower(clean[0])) {
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
  out[0] = 0;
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

// fileset is like memset but for a file handle
void fileset(FILE * fp, int c, size_t n)
{
#define BLOCK_SIZE (1024)
 int block[BLOCK_SIZE];
 size_t left_to_write = n;

 memset(block, c, sizeof(block));

 while(left_to_write > 0)
 {
   size_t write = sizeof(block);
   if (write > left_to_write) write = left_to_write;

   if (1 != fwrite(block, write, 1, fp))
   {
     fclose(fp);
     fatal("ERROR: The disk image is not complete - could not write data block!");
   }

   left_to_write -= write;
 }
}

/* Create a suited redolog header */
void make_redolog_header(redolog_header_t *header, const char* type, Bit64u size)
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

/* produce a flat image file */
#ifdef WIN32
int make_flat_image_win32(HANDLE hFile, Bit64u sec)
{
  LARGE_INTEGER pos;
  DWORD dwCount, errCode;
  USHORT mode;
  char buffer[1024];

  SetLastError(NO_ERROR);
  mode = COMPRESSION_FORMAT_DEFAULT;
  dwCount = 0;
  memset(buffer, 0, 512);
  WriteFile(hFile, buffer, 512, &dwCount, NULL); // set the first sector to 0, Win98 doesn't zero out the file
                                                 // if there is a write at/over the end
  DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, &mode, sizeof(mode), NULL, 0, &dwCount, NULL);
  pos.u.LowPart = (unsigned long)((sec - 1) << 9);
  pos.u.HighPart = (unsigned long)((sec - 1) >> 23);
  pos.u.LowPart = SetFilePointer(hFile, pos.u.LowPart, &pos.u.HighPart, FILE_BEGIN);
  memset(buffer, 0, 512);
  if ((pos.u.LowPart == 0xffffffff && GetLastError() != NO_ERROR) || !WriteFile(hFile, buffer, 512, &dwCount, NULL) || dwCount != 512)
  {
    errCode = GetLastError();
    CloseHandle(hFile);
    if (errCode == ERROR_DISK_FULL) {
      fatal("\nERROR: Not enough space on disk for image!");
    } else {
      sprintf(buffer, "\nERROR: Disk image creation failed with error code %i!", errCode);
      fatal(buffer);
    }
  }
  return 0;
}
#endif

int make_flat_image(FILE *fp, Bit64u sec)
{
 /*
  * seek to sec*512-1 and write a single character.
  * can't just do: fseek(fp, 512*sec-1, SEEK_SET)
  * because 512*sec may be too large for signed int.
  */
  while(sec > 0)
  {
    /* temp <-- min(sec, 4194303)
     * 4194303 is (int)(0x7FFFFFFF/512)
     */
    long temp = (long)((sec < 4194303) ? sec : 4194303);
    fseek(fp, 512*temp, SEEK_CUR);
    sec -= temp;
  }

  fseek(fp, -1, SEEK_CUR);
  if (fputc('\0', fp) == EOF)
  {
    fclose(fp);
    fatal("\nERROR: The disk image is not complete! (image larger then free space?)");
  }

  return 0;
}

/* produce a sparse image file */
int make_sparse_image(FILE *fp, Bit64u sec)
{
  Bit64u numpages;
  sparse_header_t header;
  size_t  sizesofar;
  size_t  padtopagesize;

  memset(&header, 0, sizeof(header));
  header.magic = htod32(SPARSE_HEADER_MAGIC);
  header.version = htod32(SPARSE_HEADER_VERSION);

  header.pagesize = htod32((1 << 10) * 32); // Use 32 KB Pages - could be configurable
  numpages = (sec / (dtoh32(header.pagesize) / 512)) + 1;

  header.numpages = htod32((Bit32u)numpages);
  header.disk = htod64(sec * 512);

  if (numpages != dtoh32(header.numpages))
  {
    fclose(fp);
    fatal("ERROR: The disk image is too large for a sparse image!");
    // Could increase page size here.
    // But note this only happens at 128 Terabytes!
  }

  if (fwrite(&header, sizeof(header), 1, fp) != 1)
  {
    fclose(fp);
    fatal("ERROR: The disk image is not complete - could not write header!");
  }

  fileset(fp, 0xff, 4 * dtoh32(header.numpages));

  sizesofar = SPARSE_HEADER_SIZE + (4 * dtoh32(header.numpages));
  padtopagesize = dtoh32(header.pagesize) - (sizesofar & (dtoh32(header.pagesize) - 1));

  fileset(fp, 0, padtopagesize);

  return 0;
}

/* produce a growing image file */
int make_growing_image(FILE *fp, Bit64u sec)
{
  redolog_header_t header;
  Bit32u i, not_allocated = htod32(REDOLOG_PAGE_NOT_ALLOCATED);

  memset(&header, 0, sizeof(header));
  make_redolog_header(&header, REDOLOG_SUBTYPE_GROWING, sec * 512);

  if (fwrite(&header, sizeof(header), 1, fp) != 1) {
    fclose(fp);
    fatal("ERROR: The disk image is not complete - could not write header!");
  }

  for (i=0; i<dtoh32(header.specific.catalog); i++) {
    if (fwrite(&not_allocated, sizeof(Bit32u), 1, fp) != 1) {
      fclose(fp);
      fatal("ERROR: The disk image is not complete - could not write catalog!");
    }
  }

  return 0;
}

/* produce the image file */
#ifdef WIN32
int make_image_win32 (Bit64u sec, char *filename, WRITE_IMAGE_WIN32 write_image)
{
  HANDLE hFile;
  char buffer[1024];

  // check if it exists before trashing someone's disk image
  hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    int confirm;
    sprintf(buffer, "\nThe disk image '%s' already exists.  Are you sure you want to replace it?\nPlease type yes or no. ", filename);
    if (ask_yn(buffer, 0, &confirm) < 0)
      fatal(EOF_ERR);
    if (!confirm)
      fatal("ERROR: Aborted");
    CloseHandle(hFile);
  }

  // okay, now open it for writing
  hFile = CreateFile(filename, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // attempt to print an error
#ifdef HAVE_PERROR
    sprintf(buffer, "while opening '%s' for writing", filename);
    perror(buffer);
#endif
    fatal("ERROR: Could not write disk image");
  }

  printf("\nWriting: [");

  if((*write_image)(hFile, sec) != 0)
     fatal("ERROR: while writing disk image!");

  printf("] Done.\n");
  CloseHandle(hFile);
  return 0;
}
#endif

int make_image(Bit64u sec, char *filename, WRITE_IMAGE write_image)
{
  FILE *fp;
  char buffer[1024];

  // check if it exists before trashing someone's disk image
  fp = fopen(filename, "r");
  if (fp) {
    int confirm;
    sprintf(buffer, "\nThe disk image '%s' already exists.  Are you sure you want to replace it?\nPlease type yes or no. ", filename);
    if (ask_yn(buffer, 0, &confirm) < 0)
      fatal(EOF_ERR);
    if (!confirm)
      fatal("ERROR: Aborted");
    fclose(fp);
  }

  // okay, now open it for writing
  fp = fopen(filename, "w");
  if (fp == NULL) {
    // attempt to print an error
#ifdef HAVE_PERROR
    sprintf(buffer, "while opening '%s' for writing", filename);
    perror(buffer);
#endif
    fatal("ERROR: Could not write disk image");
  }

  printf("\nWriting: [");

  if((*write_image)(fp, sec) != 0)
     fatal("ERROR: while writing disk image!");

  printf("] Done.\n");
  fclose(fp);
  return 0;
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bximage [options] [filename]\n\n"
    "Supported options:\n"
    "  -fd              create floppy image\n"
    "  -hd              create hard disk image\n"
    "  -mode=...        image mode (hard disks only)\n"
    "  -size=...        image size in megabytes\n"
    "  -q               quiet mode (don't prompt for user input)\n"
    "  --help           display this help and exit\n\n");
}

int parse_cmdline(int argc, char *argv[])
{
  int arg = 1;
  int ret = 1;

  bx_hdimage = -1;
  bx_fdsize_idx = -1;
  bx_hdsize = -1;
  bx_hdimagemode = -1;
  bx_interactive = 1;
  bx_filename[0] = 0;
  while ((arg < argc) && (ret == 1)) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("/?", argv[arg], 2)) {
      print_usage();
      ret = 0;
    }
    else if (!strcmp("-fd", argv[arg])) {
      bx_hdimage = 0;
      bx_hdimagemode = 0;
    }
    else if (!strcmp("-hd", argv[arg])) {
      bx_hdimage = 1;
      bx_fdsize_idx = 0;
    }
    else if (!strncmp("-mode=", argv[arg], 6)) {
      if (bx_hdimage == 1) {
        bx_hdimagemode = get_menu_index(&argv[arg][6], hdmode_n_choices, hdmode_choices);
        if (bx_hdimagemode < 0) {
          printf("Unknown image mode: %s\n\n", &argv[arg][6]);
          ret = 0;
        }
      } else {
        printf("Image mode option only supported for hard disks\n\n");
        ret = 0;
      }
    }
    else if (!strncmp("-size=", argv[arg], 6)) {
      if (bx_hdimage == 0) {
        bx_fdsize_idx = get_menu_index(&argv[arg][6], fdsize_n_choices, fdsize_choices);
        if (bx_fdsize_idx < 0) {
          printf("Unknown floppy image size: %s\n\n", &argv[arg][6]);
          ret = 0;
        }
      } else if (bx_hdimage == 1) {
        if (sscanf(&argv[arg][6], "%d", &bx_hdsize) != 1) {
          printf("Error in hard disk image size argument: %s\n\n", &argv[arg][6]);
          ret = 0;
        } else if ((bx_hdsize < 1) || (bx_hdsize > bx_max_hd_megs)) {
          printf("Hard disk image size out of range\n\n");
          ret = 0;
        }
      } else {
        printf("Image type (fd/hd) not specified\n\n");
      }
    }
    else if (!strcmp("-q", argv[arg])) {
      bx_interactive = 0;
    }
    else if (argv[arg][0] == '-') {
      printf("Unknown option: %s\n\n", argv[arg]);
      ret = 0;
    } else {
      strcpy(bx_filename, argv[arg]);
    }
    arg++;
  }
  if (bx_hdimage == -1) {
    bx_hdimage = 1;
    bx_fdsize_idx = 6;
    bx_interactive = 1;
  }
  if (bx_hdimage == 1) {
    if (bx_hdimagemode == -1) {
      bx_hdimagemode = 0;
      bx_interactive = 1;
    }
    if (bx_hdsize == -1) {
      bx_hdsize = 10;
      bx_interactive = 1;
    }
  } else {
    if (bx_fdsize_idx == -1) {
      bx_fdsize_idx = 6;
      bx_interactive = 1;
    }
  }
  if (!strlen(bx_filename)) {
    bx_interactive = 1;
  }
  return ret;
}

int CDECL main(int argc, char *argv[])
{
  Bit64s sectors = 0;
  char filename[256];
  char bochsrc_line[256];
  char prompt[80];

  WRITE_IMAGE write_function=NULL;
#ifdef WIN32
  WRITE_IMAGE_WIN32 writefn_win32=NULL;
#endif

  if (!parse_cmdline(argc, argv))
    myexit(1);

  print_banner();
  if (bx_interactive) {
    if (ask_menu(fdhd_menu, fdhd_n_choices, fdhd_choices, bx_hdimage, &bx_hdimage) < 0)
      fatal(EOF_ERR);
  }
  if (bx_hdimage) {
    Bit64u cyl;
    int hdsize, heads=16, spt=63;
    int mode;

    if (bx_interactive) {
      if (ask_menu(hdmode_menu, hdmode_n_choices, hdmode_choices, bx_hdimagemode, &mode) < 0)
        fatal (EOF_ERR);
      sprintf(prompt, "\nEnter the hard disk size in megabytes, between 1 and %d\n", bx_max_hd_megs);
      if (ask_int(prompt, 1, bx_max_hd_megs, bx_hdsize, &hdsize) < 0)
        fatal(EOF_ERR);
    } else {
      mode = bx_hdimagemode;
      hdsize = bx_hdsize;
    }
    cyl = (Bit64u)(hdsize*1024.0*1024.0/16.0/63.0/512.0);
    assert(cyl < (1 << BX_MAX_CYL_BITS));
    sectors = cyl*heads*spt;
    printf("\nI will create a '%s' hard disk image with\n", hdmode_choices[mode]);
    printf("  cyl=" FMT_LL "d\n", cyl);
    printf("  heads=%d\n", heads);
    printf("  sectors per track=%d\n", spt);
    printf("  total sectors=" FMT_LL "d\n", sectors);
    printf("  total size=%.2f megabytes\n", (float)(Bit64s)(sectors/2)/1024.0);
    if (bx_interactive) {
      if (!strlen(bx_filename)) strcpy(bx_filename, "c.img");
      if (ask_string("\nWhat should I name the image?\n", bx_filename, filename) < 0)
        fatal(EOF_ERR);
    } else {
      strcpy(filename, bx_filename);
    }

    sprintf(bochsrc_line, "ata0-master: type=disk, path=\"%s\", mode=%s, cylinders=" FMT_LL "d, heads=%d, spt=%d", filename, hdmode_choices[mode], cyl, heads, spt);

    switch (mode) {
      case 1:
        write_function=make_sparse_image;
        break;
      case 2:
        write_function=make_growing_image;
        break;
      default:
#ifdef WIN32
        writefn_win32=make_flat_image_win32;
#else
        write_function=make_flat_image;
#endif
      }
  } else {
    int fdsize, cyl=0, heads=0, spt=0;
    if (bx_interactive) {
      if (ask_menu(fdsize_menu, fdsize_n_choices, fdsize_choices, bx_fdsize_idx, &fdsize) < 0)
        fatal(EOF_ERR);
    } else {
      fdsize = bx_fdsize_idx;
    }
    switch (fdsize) {
      case 0: cyl=40; heads=1; spt=8; break;  /* 0.16 meg */
      case 1: cyl=40; heads=1; spt=9; break;  /* 0.18 meg */
      case 2: cyl=40; heads=2; spt=8; break;  /* 0.32 meg */
      case 3: cyl=40; heads=2; spt=9; break;  /* 0.36 meg */
      case 4: cyl=80; heads=2; spt=9; break;  /* 0.72 meg */
      case 5: cyl=80; heads=2; spt=15; break; /* 1.2 meg */
      case 6: cyl=80; heads=2; spt=18; break; /* 1.44 meg */
      case 7: cyl=80; heads=2; spt=21; break; /* 1.68 meg */
      case 8: cyl=82; heads=2; spt=21; break; /* 1.72 meg */
      case 9: cyl=80; heads=2; spt=36; break; /* 2.88 meg */
      default:
        fatal("ERROR: fdsize out of range");
    }
    sectors = cyl*heads*spt;
    printf("I will create a floppy image with\n");
    printf("  cyl=%d\n", cyl);
    printf("  heads=%d\n", heads);
    printf("  sectors per track=%d\n", spt);
    printf("  total sectors=" FMT_LL "d\n", sectors);
    printf("  total bytes=" FMT_LL "d\n", sectors*512);
    if (bx_interactive) {
      if (!strlen(bx_filename)) strcpy(bx_filename, "a.img");
      if (ask_string("\nWhat should I name the image?\n", bx_filename, filename) < 0)
        fatal(EOF_ERR);
    } else {
      strcpy(filename, bx_filename);
    }
    sprintf(bochsrc_line, "floppya: image=\"%s\", status=inserted", filename);

    write_function=make_flat_image;
  }
  if (sectors < 1)
    fatal("ERROR: Illegal disk size!");
  if (strlen (filename) < 1)
    fatal("ERROR: Illegal filename");
#ifdef WIN32
  if (writefn_win32 != NULL) {
    make_image_win32(sectors, filename, writefn_win32);
  }
  else
#endif
  {
    make_image(sectors, filename, write_function);
  }
  printf("\nI wrote " FMT_LL "u bytes to ", sectors*512);
  printf("%s.\n", filename);
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
  myexit(0);

  // make picky compilers (c++, gcc) happy,
  // even though we leave via 'myexit' just above
  return 0;
}
