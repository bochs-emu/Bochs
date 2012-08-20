/*
 * misc/bximage.c
 * $Id$
 *
 * Commits a redolog file in a flat file for bochs images.
 *
 */

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
#ifdef WIN32
#  include <conio.h>
#endif
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

int  bxcommit_mode;
int  bx_remove;
int  bx_interactive;
char bx_flat_filename[256];
char bx_redolog_name[256];

char *EOF_ERR = "ERROR: End of input";
char *svnid = "$Id$";
char *divider = "========================================================================";
const char *main_menu_prompt =
"\n"
"1. Commit 'undoable' redolog to 'flat' file\n"
"2. Create 'flat' disk image from 'growing' disk image\n"
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
void bx_center_print(FILE *file, char *line, int maxwidth)
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

int ask_menu(const char *prompt, int n_choices, char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
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
    fprintf(stderr, "Your choice (%s) did not match any of the choices:\n", clean);
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

/* produce the image file */
int commit_redolog()
{
  int flatfd = -1, redologfd;
  redolog_header_t header;
  Bit32u *catalog, catalog_size;
  Bit8u  *bitmap;
  Bit32u i, bitmap_blocks, extent_blocks;
  Bit8u  buffer[512];

  if (bxcommit_mode == BXCOMMIT_MODE_COMMIT_UNDOABLE) {
    // check if flat file exists
    flatfd = open(bx_flat_filename, O_WRONLY
#ifdef O_BINARY
                  | O_BINARY
#endif
                  );
    if (flatfd < 0) {
      fatal("ERROR: flat file not found or not writable");
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

  printf ("\nReading redolog header: [");

  if (read(redologfd, &header, STANDARD_HEADER_SIZE) != STANDARD_HEADER_SIZE)
    fatal("\nERROR: while reading redolog header!");

  // Print infos on redlog
  printf("Type='%s', Subtype='%s', Version=%d.%d] Done.",
         header.standard.type, header.standard.subtype,
         dtoh32(header.standard.version)/0x10000,
         dtoh32(header.standard.version)%0x10000);

  printf("\nChecking redolog header: [");

  if (strcmp((char *)header.standard.magic, STANDARD_HEADER_MAGIC) != 0)
    fatal("\nERROR: bad magic in redolog header!");

  if (strcmp((char *)header.standard.type, REDOLOG_TYPE) != 0)
    fatal("\nERROR: bad type in redolog header!");

  if (bxcommit_mode == BXCOMMIT_MODE_COMMIT_UNDOABLE) {
    if (strcmp((char *)header.standard.subtype, REDOLOG_SUBTYPE_UNDOABLE) != 0)
      fatal("\nERROR: bad subtype in redolog header!");
  } else if (bxcommit_mode == BXCOMMIT_MODE_GROWING_TO_FLAT) {
    if (strcmp((char *)header.standard.subtype, REDOLOG_SUBTYPE_GROWING) != 0)
      fatal("\nERROR: bad subtype in redolog header!");
  } else {
    fatal("\nERROR: unknown bxcommit mode!");
  }

  if (header.standard.version != htod32(STANDARD_HEADER_VERSION))
    fatal("\nERROR: bad version in redolog header!");

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

  catalog = (Bit32u*)malloc(dtoh32(header.specific.catalog) * sizeof(Bit32u));
  bitmap = (Bit8u*)malloc(dtoh32(header.specific.bitmap));
  printf("\nReading Catalog: [");

  lseek(redologfd, dtoh32(header.standard.header), SEEK_SET);

  catalog_size = dtoh32(header.specific.catalog) * sizeof(Bit32u);
  if ((Bit32u) read(redologfd, catalog, catalog_size) != catalog_size)
    fatal("\nERROR: while reading redolog catalog!");

  printf("...] Done.");

  printf("\nCommitting changes to flat file: [  0%%]");

  bitmap_blocks = 1 + (dtoh32(header.specific.bitmap) - 1) / 512;
  extent_blocks = 1 + (dtoh32(header.specific.extent) - 1) / 512;

  for(i=0; i<dtoh32(header.specific.catalog); i++) {
    printf("\x8\x8\x8\x8\x8%3d%%]", (i+1)*100/dtoh32(header.specific.catalog));
    fflush(stdout);

    if (dtoh32(catalog[i]) != REDOLOG_PAGE_NOT_ALLOCATED) {
      Bit64s bitmap_offset;
      Bit32u bitmap_size, j;

      bitmap_offset  = (Bit64s)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
      bitmap_offset += (Bit64s)512 * dtoh32(catalog[i]) * (extent_blocks + bitmap_blocks);

      // Read bitmap
      lseek(redologfd, bitmap_offset, SEEK_SET);

      bitmap_size = dtoh32(header.specific.bitmap);
      if ((Bit32u) read(redologfd, bitmap, bitmap_size) != bitmap_size)
        fatal("\nERROR: while reading bitmap from redolog !");

      for(j=0; j<dtoh32(header.specific.bitmap); j++) {
        Bit32u bit;

        for(bit=0; bit<8; bit++) {
          if ( (bitmap[j] & (1<<bit)) != 0) {
            Bit64s flat_offset, block_offset;

            block_offset = bitmap_offset + ((Bit64s)512 * (bitmap_blocks + ((j*8)+bit)));

            lseek(redologfd, (off_t)block_offset, SEEK_SET);

            if (read(redologfd, buffer, 512) != 512)
              fatal("\nERROR: while reading block from redolog !");

            flat_offset  = (Bit64s)i * (dtoh32(header.specific.extent));
            flat_offset += (Bit64s)512 * ((j * 8) + bit);

            lseek(flatfd, (off_t)flat_offset, SEEK_SET);

            if (write(flatfd, buffer, 512) != 512)
              fatal("\nERROR: while writing block in flat file !");
          }
        }
      }
    }
  }

  printf(" Done.");
  printf("\n");

  close(flatfd);
  close(redologfd);

  return 0;
}

void print_usage()
{
  fprintf(stderr,
    "Usage: bxcommit [options] [flat filename] [redolog filename]\n\n"
    "Supported options:\n"
    "  -mode=undoable   commit undoable redolog to flat file\n"
    "  -mode=growing    create flat disk image from growing disk image\n"
    "  -d               delete redolog file after commit\n"
    "  -q               quiet mode (don't prompt for user input)\n"
    "  --help           display this help and exit\n\n");
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
      if (!strcmp(&argv[arg][6], "undoable")) {
        bxcommit_mode = BXCOMMIT_MODE_COMMIT_UNDOABLE;
      } else if (!strcmp(&argv[arg][6], "growing")) {
        bxcommit_mode = BXCOMMIT_MODE_GROWING_TO_FLAT;
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
    if (ask_int(main_menu_prompt, 0, 2, bxcommit_mode, &bxcommit_mode) < 0)
      fatal(EOF_ERR);

    switch (bxcommit_mode) {
      case 0:
        myexit(0);
        break;

      case BXCOMMIT_MODE_COMMIT_UNDOABLE:
        if (!strlen(bx_flat_filename)) {
          strcpy(bx_flat_filename, "c.img");
        }
        if (ask_string("\nWhat is the flat image name?\n", bx_flat_filename, bx_flat_filename) < 0)
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
        if (ask_string("\nWhat is the 'growing' image name?\n", bx_redolog_name, bx_redolog_name) < 0)
          fatal(EOF_ERR);
        if (!strlen(bx_flat_filename)) {
          strcpy(bx_flat_filename, "flat.img");
        }
        if (ask_string("\nWhat should be the name of the 'flat' image?\n", bx_flat_filename, bx_flat_filename) < 0)
          fatal(EOF_ERR);
        if (ask_yn("\nShould the 'growing' image been removed afterwards?\n", 0, &bx_remove) < 0)
          fatal(EOF_ERR);
        break;
    }
  }

  commit_redolog();

  if (bx_remove) {
    if (unlink(bx_redolog_name) != 0)
      fatal("ERROR: while removing the redolog !\n");
  }

  // make picky compilers (c++, gcc) happy,
  // even though we leave via 'myexit' just above
  return 0;
}
