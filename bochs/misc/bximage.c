/* 
 * misc/bximage.c
 * $Id: bximage.c,v 1.2 2001-06-01 04:24:12 bdenney Exp $
 *
 * Create empty hard disk or floppy disk images for bochs.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "config.h"

char *EOF_ERR = "ERROR: End of input";
char *rcsid = "$Revision: 1.2 $, modified $Date: 2001-06-01 04:24:12 $";
char *divider = "========================================================================";

/* menu data for choosing floppy/hard disk */
char *fdhd_menu = "\nDo you want to create a floppy disk image or a hard disk image?\nPlease type hd or fd. [hd] ";
char *fdhd_choices[] = { "fd", "hd" };
int fdhd_n_choices = 2;

/* menu data for choosing floppy size */
char *fdsize_menu = "\nChoose the size of floppy disk image to create, in megabytes.\nPlease type 0.72, 1.2, 1.44, or 2.88. [1.44] ";
char *fdsize_choices[] = { "0.72","1.2","1.44","2.88" };
int fdsize_n_choices = 4;

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
  exit (1);
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
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    clean = clean_string (buffer);
    if (strlen(clean) < 1) {
      // empty line, use the default
      *out = the_default;
      return 0;
    }
    for (i=0; i<n_choices; i++) {
      if (!strcasecmp (choice[i], clean)) {
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
  int i;
  *out = -1;
  while (1) {
    printf ("%s", prompt);
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
  int i;
  out[0] = 0;
  printf ("%s", prompt);
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
int make_image (int sec, char *filename)
{
  FILE *fp;
  char buffer[1024];
  int i;
  unsigned int n;

  // check if it exists before trashing someone's disk image
  fp = fopen (filename, "r");
  if (fp) {
    int confirm;
    sprintf (buffer, "\nThe disk image '%s' already exists.  Are you sure you want to replace it?\nPlease type yes or no. [no] ");
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
  // clear the buffer
  for (i=0; i<512; i++)
    buffer[i] = 0;
  // write it however many times
  printf ("\nWriting: [");
  for (i=0; i<sec; i++) {
    n = (unsigned int) fwrite (buffer, 512, 1, fp);
    if (n != 1) {
      printf ("\nWrite failed with %d sectors written\n", i);
      fclose (fp);
      fatal ("ERROR: The disk image is not complete!");
    }
    if ((i%2048) == 0) {
      printf (".");
      fflush (stdout);
    }
  }
  printf ("] Done.\n");
  fclose (fp);
  return 0;
}

int main()
{
  int hd;
  int sectors = 0;
  char filename[256];
  char bochsrc_line[256];
  print_banner ();
  filename[0] = 0;
  if (ask_menu (fdhd_menu, fdhd_n_choices, fdhd_choices, 1, &hd) < 0)
    fatal (EOF_ERR);
  if (hd) {
    int hdsize, cyl, heads=16, spt=63;
    if (ask_int ("\nEnter the hard disk size in megabytes, between 1 and 32255\n[10] ", 1, 32255, 10, &hdsize) < 0)
      fatal (EOF_ERR);
    cyl = hdsize*1024*1024/16/63/512;
    assert (cyl < 65536);
    sectors = cyl*heads*spt;
    printf ("\nI will create a hard disk image with\n");
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%d\n", sectors);
    printf ("  total size=%.2f megabytes\n", (float)sectors*512.0/1024.0/1024.0);
    if (ask_string ("\nWhat should I name the image?\n[c.img] ", "c.img", filename) < 0)
      fatal (EOF_ERR);
    sprintf (bochsrc_line, "diskc: file=\"%s\", cyl=%d, heads=%d, spt=%d", filename, cyl, heads, spt);
  } else {
    int fdsize, cyl, heads, spt;
    char *name;
    if (ask_menu (fdsize_menu, fdsize_n_choices, fdsize_choices, 2, &fdsize) < 0)
      fatal (EOF_ERR);
    switch (fdsize) {
    case 0: name="720k"; cyl=80; heads=2; spt=9; break;   /* 0.72 meg */
    case 1: name="1_2"; cyl=80; heads=2; spt=15; break;   /* 1.2 meg */
    case 2: name="1_44"; cyl=80; heads=2; spt=18; break;   /* 1.44 meg */
    case 3: name="2_88"; cyl=80; heads=2; spt=36; break;   /* 2.88 meg */
    default: 
      fatal ("ERROR: fdsize out of range");
    }
    sectors = cyl*heads*spt;
    printf ("I will create a floppy image with\n");
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%d\n", sectors);
    printf ("  total bytes=%d\n", sectors*512);
    if (ask_string ("\nWhat should I name the image?\n[a.img] ", "a.img", filename) < 0)
      fatal (EOF_ERR);
    sprintf (bochsrc_line, "floppya: %s=\"%s\", status=inserted", name, filename);
  }
  if (sectors < 1)
    fatal ("ERROR: Illegal disk size!");
  if (strlen (filename) < 1)
    fatal ("ERROR: Illegal filename");
  make_image (sectors, filename);
  printf ("\nI wrote %d bytes to %s.\n", sectors*512, filename);
  printf ("\nThe following line should appear in your bochsrc:\n");
  printf ("  %s\n", bochsrc_line);
}
