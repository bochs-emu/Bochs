/*
 * misc/bximage.c
 * $Id: bximage.c,v 1.18 2003-05-03 16:37:18 cbothamy Exp $
 *
 * Create empty hard disk or floppy disk images for bochs.
 *
 */

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
char *rcsid = "$Id: bximage.c,v 1.18 2003-05-03 16:37:18 cbothamy Exp $";
char *divider = "========================================================================";

/* menu data for choosing floppy/hard disk */
char *fdhd_menu = "\nDo you want to create a floppy disk image or a hard disk image?\nPlease type hd or fd. ";
char *fdhd_choices[] = { "fd", "hd" };
int fdhd_n_choices = 2;

/* menu data for choosing floppy size */
char *fdsize_menu = "\nChoose the size of floppy disk image to create, in megabytes.\nPlease type 0.36, 0.72, 1.2, 1.44, or 2.88. ";
char *fdsize_choices[] = { "0.36","0.72","1.2","1.44","2.88" };
int fdsize_n_choices = 5;

/* menu data for choosing disk mode */
char *hdmode_menu = "\nWhat kind of image should I create?\nPlease type flat, sparse or growable. ";
                char *hdmode_choices[] = {"flat", "sparse", "growable" };
int hdmode_n_choices = 3;

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
  bx_center_print (stdout, "bximage\n", 72);
  bx_center_print (stdout, "Disk Image Creation Tool for Bochs\n", 72);
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

// fileset is like memset but for a file handle
void fileset(FILE * fp, int c, size_t n)
{
#define BLOCK_SIZE (1024)
 int block[BLOCK_SIZE];
 size_t left_to_write = n;

 memset(block, c, sizeof(block));

 while (left_to_write > 0)
 {
   size_t write = sizeof(block);
   if (write > left_to_write) write = left_to_write;

   if (1 != fwrite(block, write, 1, fp))
   {
     fclose (fp);
     fatal ("ERROR: The disk image is not complete - could not write data block!");
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
int make_flat_image(FILE *fp, Bit64u sec)
{
   /*
   * seek to sec*512-1 and write a single character.
   * can't just do: fseek(fp, 512*sec-1, SEEK_SET)
   * because 512*sec may be too large for signed int.
   */
   while (sec > 0)
   {
     /* temp <-- min(sec, 4194303)
     * 4194303 is (int)(0x7FFFFFFF/512)
     */
     long temp = ((sec < 4194303) ? sec : 4194303);
     fseek(fp, 512*temp, SEEK_CUR);
     sec -= temp;
   }

   fseek(fp, -1, SEEK_CUR);
   if (fputc('\0', fp) == EOF)
   {
     fclose (fp);
     fatal ("ERROR: The disk image is not complete!");
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

   header.numpages = htod32(numpages);

   if (numpages != dtoh32(header.numpages))
   {
     fclose (fp);
     fatal ("ERROR: The disk image is too large for a sparse image!");
     // Could increase page size here.
     // But note this only happens at 128 Terabytes!
   }

   if (fwrite(&header, sizeof(header), 1, fp) != 1)
   {
     fclose (fp);
     fatal ("ERROR: The disk image is not complete - could not write header!");
   }

   fileset(fp, 0xff, 4 * dtoh32(header.numpages));

   sizesofar = SPARSE_HEADER_SIZE + (4 * dtoh32(header.numpages));
   padtopagesize = dtoh32(header.pagesize) - (sizesofar & (dtoh32(header.pagesize) - 1));

   fileset(fp, 0, padtopagesize);

   return 0;
}

/* produce a growable image file */
int make_growable_image(FILE *fp, Bit64u sec)
{
        redolog_header_t header;
        Bit32u i, not_allocated = htod32(REDOLOG_PAGE_NOT_ALLOCATED);

        memset(&header, 0, sizeof(header));
        make_redolog_header(&header, REDOLOG_SUBTYPE_GROWABLE, sec * 512);

        if (fwrite(&header, sizeof(header), 1, fp) != 1)
        {
                fclose (fp);
                fatal ("ERROR: The disk image is not complete - could not write header!");
        }

        for (i=0; i<dtoh32(header.specific.catalog); i++)
        {
                if (fwrite(&not_allocated, sizeof(Bit32u), 1, fp) != 1)
                {
                        fclose (fp);
                        fatal ("ERROR: The disk image is not complete - could not write catalog!");
                }
        }

        return 0;
}

/* produce the image file */
int make_image (Bit64u sec, char *filename, int (*write_image)(FILE*, Bit64u) )
{
 FILE *fp;
  char buffer[1024];
 
  // check if it exists before trashing someone's disk image
  fp = fopen (filename, "r");
  if (fp) {
    int confirm;
    sprintf (buffer, "\nThe disk image '%s' already exists.  Are you sure you want to replace it?\nPlease type yes or no. ", filename);
    if (ask_yn (buffer, 0, &confirm) < 0)
      fatal (EOF_ERR);
    if (!confirm)
      fatal ("ERROR: Aborted");
    fclose (fp);
  }

  // okay, now open it for writing
  fp = fopen (filename, "w");
  if (fp == NULL) {
    // attempt to print an error
#ifdef HAVE_PERROR
    sprintf (buffer, "while opening '%s' for writing", filename);
    perror (buffer);
#endif
    fatal ("ERROR: Could not write disk image");
  }

  printf ("\nWriting: [");

  if( (*write_image)(fp, sec) != 0)
     fatal ("ERROR: while writing disk image!");

  printf ("] Done.\n");
  fclose (fp);
  return 0;
}

int main()
{
  int hd;
  Bit64s sectors = 0;
  char filename[256];
  char bochsrc_line[256];

  int (*write_function)(FILE*, Bit64u)=NULL;
 
  print_banner ();
  filename[0] = 0;
  if (ask_menu (fdhd_menu, fdhd_n_choices, fdhd_choices, 1, &hd) < 0)
    fatal (EOF_ERR);
  if (hd) {
    int hdsize, cyl, heads=16, spt=63;
    int mode;

    if (ask_menu (hdmode_menu, hdmode_n_choices, hdmode_choices, 0, &mode) < 0)
      fatal (EOF_ERR);

    if (ask_int ("\nEnter the hard disk size in megabytes, between 1 and 32255\n", 1, 32255, 10, &hdsize) < 0)
      fatal (EOF_ERR);
    cyl = (int) (hdsize*1024.0*1024.0/16.0/63.0/512.0);
    assert (cyl < 65536);
    sectors = cyl*heads*spt;
    printf ("\nI will create a '%s' hard disk image with\n", hdmode_choices[mode]);
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%lld\n", sectors);
    printf ("  total size=%.2f megabytes\n", (float)sectors*512.0/1024.0/1024.0);
    if (ask_string ("\nWhat should I name the image?\n", "c.img", filename) < 0)
      fatal (EOF_ERR);

    sprintf (bochsrc_line, "ata0-master: type=disk, path=\"%s\", mode=%s, cylinders=%d, heads=%d, spt=%d", filename, hdmode_choices[mode], cyl, heads, spt);

    switch (mode) {
      case 1:
        write_function=make_sparse_image;
        break;
      case 2:
        write_function=make_growable_image;
        break;
      default:
        write_function=make_flat_image;
      }
  } else {
    int fdsize, cyl=0, heads=0, spt=0;
    char *name = NULL;
    if (ask_menu (fdsize_menu, fdsize_n_choices, fdsize_choices, 3, &fdsize) < 0)
      fatal (EOF_ERR);
    switch (fdsize) {
    case 0: name="360k"; cyl=40; heads=2; spt=9; break;   /* 0.36 meg */
    case 1: name="720k"; cyl=80; heads=2; spt=9; break;   /* 0.72 meg */
    case 2: name="1_2"; cyl=80; heads=2; spt=15; break;   /* 1.2 meg */
    case 3: name="1_44"; cyl=80; heads=2; spt=18; break;   /* 1.44 meg */
    case 4: name="2_88"; cyl=80; heads=2; spt=36; break;   /* 2.88 meg */
    default:
      fatal ("ERROR: fdsize out of range");
    }
    sectors = cyl*heads*spt;
    printf ("I will create a floppy image with\n");
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%lld\n", sectors);
    printf ("  total bytes=%lld\n", sectors*512);
    if (ask_string ("\nWhat should I name the image?\n", "a.img", filename) < 0)
      fatal (EOF_ERR);
    sprintf (bochsrc_line, "floppya: %s=\"%s\", status=inserted", name, filename);

    write_function=make_flat_image;
  }
  if (sectors < 1)
    fatal ("ERROR: Illegal disk size!");
  if (strlen (filename) < 1)
    fatal ("ERROR: Illegal filename");
  make_image (sectors, filename, write_function);
  printf ("\nI wrote %lld bytes to %s.\n", sectors*512, filename);
  printf ("\nThe following line should appear in your bochsrc:\n");
  printf ("  %s\n", bochsrc_line);
  myexit(0);

  // make picky compilers (c++, gcc) happy,
  // even though we leave via 'myexit' just above
  return 0;
}
