/*
 * misc/bximage.c
 * $Id: bxcommit.c,v 1.2 2003-05-03 16:37:18 cbothamy Exp $
 *
 * Commits a redolog file in a flat file for bochs images.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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

#define uint8   Bit8u
#define uint16  Bit16u
#define uint32  Bit32u

#define INCLUDE_ONLY_HD_HEADERS 1
#include "../iodev/harddrv.h"

char *EOF_ERR = "ERROR: End of input";
char *rcsid = "$Id: bxcommit.c,v 1.2 2003-05-03 16:37:18 cbothamy Exp $";
char *divider = "========================================================================";

void myexit (int code)
{
#ifdef WIN32
  printf ("\nPress any key to continue\n");
  getch();
#endif
  exit(code);
}

/* stolen from main.cc */
void bx_center_print (FILE *file, char *line, int maxwidth)
{
  int imax;
  int i;
  imax = (maxwidth - strlen(line)) >> 1;
  for (i=0; i<imax; i++) fputc (' ', file);
  fputs (line, file);
}

void
print_banner ()
{
  printf ("%s\n", divider);
  bx_center_print (stdout, "bxcommit\n", 72);
  bx_center_print (stdout, "Undoable Disk Image Commit Tool for Bochs\n", 72);
  bx_center_print (stdout, rcsid, 72);
  printf ("\n%s\n", divider);
}

/* this is how we crash */
void fatal (char *c)
{
  printf ("%s\n", c);
  myexit (1);
}

/* remove leading spaces, newline junk at end.  returns pointer to 
 cleaned string, which is between s0 and the null */
char *
clean_string (char *s0)
{
  char *s = s0;
  char *ptr;
  /* find first nonblank */
  while (isspace (*s))
    s++;
  /* truncate string at first non-alphanumeric */
  ptr = s;
  while (isprint (*ptr))
    ptr++;
  *ptr = 0;
  return s;
}

/* returns 0 on success, -1 on failure.  The value goes into out. */
int 
ask_int (char *prompt, int min, int max, int the_default, int *out)
{
  int n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    printf ("%s", prompt);
    printf ("[%d] ", the_default);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    illegal = (1 != sscanf (buffer, "%d", &n));
    if (illegal || n<min || n>max) {
      printf ("Your choice (%s) was not an integer between %d and %d.\n\n",
	  clean, min, max);
    } else {
      // choice is okay
      *out = n;
      return 0;
    }
  }
}

int 
ask_menu (char *prompt, int n_choices, char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
    printf ("%s", prompt);
    printf ("[%s] ", choice[the_default]);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    for (i=0; i<n_choices; i++) {
      if (!strcmp (choice[i], clean)) {
	// matched, return the choice number
	*out = i;
	return 0;
      }
    }
    printf ("Your choice (%s) did not match any of the choices:\n", clean);
    for (i=0; i<n_choices; i++) {
      if (i>0) printf (", ");
      printf ("%s", choice[i]);
    }
    printf ("\n");
  }
}

int 
ask_yn (char *prompt, int the_default, int *out)
{
  char buffer[16];
  char *clean;
  *out = -1;
  while (1) {
    printf ("%s", prompt);
    printf ("[%s] ", the_default?"yes":"no");
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    switch (tolower(clean[0])) {
      case 'y': *out=1; return 0;
      case 'n': *out=0; return 0;
    }
    printf ("Please type either yes or no.\n");
  }
}

int 
ask_string (char *prompt, char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  out[0] = 0;
  printf ("%s", prompt);
  printf ("[%s] ", the_default);
  if (!fgets (buffer, sizeof(buffer), stdin))
    return -1;
  clean = clean_string (buffer);
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy (out, the_default);
    return 0;
  }
  strcpy (out, clean);
  return 0;
}

/* produce the image file */
int commit_redolog (char *flatname, char *redologname )
{
  int flatfd, redologfd;
  redolog_header_t header;
  Bit32u *catalog, catalog_size;
  Bit8u  *bitmap;
  Bit32u i, bitmap_blocs, extent_blocs;
  Bit8u  buffer[512];
 
  // check if flat file exists
  flatfd = open (flatname, O_WRONLY
#ifdef O_BINARY
                         | O_BINARY
#endif
                  );
  if (flatfd<0) {
    fatal ("ERROR: flat file not found or not writable");
  }

  // Check if redolog exists
  printf("%s\n",redologname);
  redologfd = open (redologname, O_RDONLY
#ifdef O_BINARY
                         | O_BINARY
#endif
                  );
  if (redologfd<0) {
    fatal ("ERROR: redolog file not found");
  }

  printf ("\nReading redolog header: [");

  if (read(redologfd, &header, STANDARD_HEADER_SIZE) != STANDARD_HEADER_SIZE)
     fatal ("\nERROR: while reading redolog header!");

  // Print infos on redlog
  printf("Type='%s', Subtype='%s', Version=%d.%d] Done.", 
           header.standard.type, header.standard.subtype,
           dtoh32(header.standard.version)/0x10000, 
           dtoh32(header.standard.version)%0x10000);

  printf ("\nChecking redolog header: [");

  if (strcmp(header.standard.magic, STANDARD_HEADER_MAGIC) != 0)
     fatal ("\nERROR: bad magic in redolog header!");

  if (strcmp(header.standard.type, REDOLOG_TYPE) != 0)
     fatal ("\nERROR: bad type in redolog header!");

  if (strcmp(header.standard.subtype, REDOLOG_SUBTYPE_UNDOABLE) != 0)
     fatal ("\nERROR: bad subtype in redolog header!");

  if (header.standard.version != htod32(STANDARD_HEADER_VERSION))
     fatal ("\nERROR: bad version in redolog header!");

  printf("#entries=%d, bitmap size=%d, exent size = %d] Done.",
         dtoh32(header.specific.catalog),
         dtoh32(header.specific.bitmap),
         dtoh32(header.specific.extent));

  catalog = (Bit32u*)malloc(dtoh32(header.specific.catalog) * sizeof(Bit32u));
  bitmap = (Bit8u*)malloc(dtoh32(header.specific.bitmap));
  printf ("\nReading Catalog: [");

  lseek(redologfd, dtoh32(header.standard.header), SEEK_SET);

  catalog_size = dtoh32(header.specific.catalog) * sizeof(Bit32u);
  if (read(redologfd, catalog, catalog_size)!= catalog_size)
     fatal ("\nERROR: while reading redolog catalog!");

  printf ("] Done.");

  printf ("\nCommitting changes to flat file: [  0%%]");

  bitmap_blocs = 1 + (dtoh32(header.specific.bitmap) - 1) / 512;
  extent_blocs = 1 + (dtoh32(header.specific.extent) - 1) / 512;

  for(i=0; i<dtoh32(header.specific.catalog); i++)
  {
          printf("\x8\x8\x8\x8\x8%3d%%]", (i+1)*100/dtoh32(header.specific.catalog));
          fflush(stdout);
          
          if (dtoh32(catalog[i]) != REDOLOG_PAGE_NOT_ALLOCATED) 
          {
                  off_t bitmap_offset;
                  Bit32u bitmap_size, j;

                  bitmap_offset  = (off_t)STANDARD_HEADER_SIZE + (dtoh32(header.specific.catalog) * sizeof(Bit32u));
                  bitmap_offset += (off_t)512 * dtoh32(catalog[i]) * (extent_blocs + bitmap_blocs);

                  // Read bitmap
                  lseek(redologfd, bitmap_offset, SEEK_SET);

                  bitmap_size = dtoh32(header.specific.bitmap);
                  if (read(redologfd, bitmap, bitmap_size) != bitmap_size)
                          fatal ("\nERROR: while reading bitmap from redolog !");

                  for(j=0; j<dtoh32(header.specific.bitmap); j++)
                  {
                          Bit32u bit;

                          for(bit=0; bit<8; bit++)
                          {
                                  if ( (bitmap[j] & (1<<bit)) != 0)
                                  {
                                          off_t flat_offset, bloc_offset;

                                          bloc_offset  = bitmap_offset + ((off_t)512 * (bitmap_blocs + ((j*8)+bit)));
                                          
                                          lseek(redologfd, bloc_offset, SEEK_SET);
                                          
                                          if (read(redologfd, buffer, 512) != 512)
                                                fatal ("\nERROR: while reading bloc from redolog !");

                                          flat_offset  = (off_t)i * (dtoh32(header.specific.extent));
                                          flat_offset += (off_t)512 * ((j * 8) + bit);
                                          
                                          lseek(flatfd, flat_offset, SEEK_SET);

                                          if (write(flatfd, buffer, 512) != 512)
                                                fatal ("\nERROR: while writing bloc in flatfile !");

                                  }
                          }
                  }
          }
  }
  printf (" Done.");
  printf ("\n");

  close (flatfd);
  close (redologfd);
  return 0;
}

int main()
{
  char filename[256];
  char tmplogname[256];
  char redologname[256];
  int  remove;

  int (*commit_function)(FILE*, Bit64u)=NULL;
 
  print_banner ();
  filename[0] = 0;
  redologname[0] = 0;

  if (ask_string ("\nWhat is the flat image name?\n", "c.img", filename) < 0)
    fatal (EOF_ERR);

  snprintf(tmplogname,256,"%s%s", filename,REDOLOG_EXTENSION);

  if (ask_string ("\nWhat is the redolog name?\n", tmplogname, redologname) < 0)
    fatal (EOF_ERR);

  if (ask_yn ("\nShall I remove the redolog afterwards?\n", 1, &remove) < 0)
    fatal (EOF_ERR);

  commit_redolog(filename, redologname);

  if (remove) {
    if (unlink(redologname) != 0)
       fatal ("ERROR: while removing the redolog !\n");
  }

  // make picky compilers (c++, gcc) happy,
  // even though we leave via 'myexit' just above
  return 0;
}
