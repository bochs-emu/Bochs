/* 
 * mkimg2.c
 *
 * Create empty hard disk or floppy disk images for bochs.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "config.h"

/* remove leading spaces, newline junk at end.
   return pointer to cleaned string, which is between s0 and the null */
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

/* returns 0 on success, -1 on failure */
int ask_int (char *prompt, int min, int max, int the_default, int *out)
{
  int n = max + 1;
  char buffer[1024];
  char *clean;
  int illegal;
  while (1) {
    printf ("%s", prompt);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    //printf ("Before cleaning we had: '%s'\n", buffer);
    clean = clean_string (buffer);
    //printf ("After cleaning we had: '%s'\n", clean);
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

int ask_menu (char *prompt, int n_choices, char *choice[], int the_default, int *out)
{
  char buffer[1024];
  char *clean;
  int i;
  *out = -1;
  while (1) {
    printf ("%s", prompt);
    if (!fgets (buffer, sizeof(buffer), stdin))
      return -1;
    //printf ("Before cleaning we had: '%s'\n", buffer);
    clean = clean_string (buffer);
    //printf ("After cleaning we had: '%s'\n", clean);
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

int ask_string (char *prompt, char *the_default, char *out)
{
  char buffer[1024];
  char *clean;
  int i;
  out[0] = 0;
  printf ("%s", prompt);
  if (!fgets (buffer, sizeof(buffer), stdin))
    return -1;
  //printf ("Before cleaning we had: '%s'\n", buffer);
  clean = clean_string (buffer);
  //printf ("After cleaning we had: '%s'\n", clean);
  if (strlen(clean) < 1) {
    // empty line, use the default
    strcpy (out, the_default);
    return 0;
  }
  strcpy (out, clean);
  return 0;
}

char *fdhd_menu = "\nDo you want to create a floppy disk image or a hard disk image?\nPlease type hd or fd. [hd] ";
char *fdhd_choices[] = { "fd", "hd" };
int fdhd_n_choices = 2;

char *fdsize_menu = "\nChoose the size of floppy disk image to create, in megabytes.\nPlease type 0.72, 1.2, 1.44, or 2.88. [1.44] ";
char *fdsize_choices[] = { "0.72","1.2","1.44","2.88" };
int fdsize_n_choices = 4;

void panic (char *c)
{
  printf ("Internal error: %s\n", c);
  exit (1);
}

int make_image (int sec, char *filename)
{
  FILE *fp = fopen (filename, "w");
  char buffer[1024];
  int i;
  unsigned int n;
  if (fp == NULL) {
    // attempt to print an error
#ifdef HAVE_PERROR
    sprintf (buffer, "while opening '%s' for writing", filename);
    perror (buffer);
#endif
    exit (1);
  }
  // clear the buffer
  for (i=0; i<512; i++)
    buffer[i] = 0;
  // write it however many times
  printf ("\nWriting: ");
  for (i=0; i<sec; i++) {
    n = (unsigned int) fwrite (buffer, 512, 1, fp);
    if (n != 1) {
      printf ("ERROR: Write failed after %d sectors have been written\n", i);
      fclose (fp);
      return -1;
    }
    if (i&16) printf (".");
#error fixme
  }
  printf ("done.\n");
  fclose (fp);
  return 0;
}

int main()
{
  int hd;
  int sectors = 0;
  char filename[256];
  char bochsrc_line[256];
  filename[0] = 0;
  if (ask_menu (fdhd_menu, fdhd_n_choices, fdhd_choices, 1, &hd) < 0)
    exit (1);
  if (hd) {
    int hdsize, cyl, heads=16, spt=63;
    if (ask_int ("\nEnter the hard disk size in megabytes, between 1 and 65535\n[10] ", 1, 65535, 10, &hdsize) < 0)
      exit (1);
    cyl = hdsize*1024*1024/16/63/512;
    sectors = cyl*heads*spt;
    printf ("\nI will create a hard disk image with\n");
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%d\n", sectors);
    printf ("  total size=%.2f megabytes\n", (float)sectors*512.0/1024.0/1024.0);
    if (ask_string ("\nWhat should I name the image?\n[c.img] ", "c.img", filename) < 0)
      exit(1);
    sprintf (bochsrc_line, "diskc: file=\"%s\", cyl=%d, heads=%d, spt=%d", filename, cyl, heads, spt);
  } else {
    int fdsize, cyl, heads, spt;
    char *name;
    if (ask_menu (fdsize_menu, fdsize_n_choices, fdsize_choices, 2, &fdsize) < 0)
      exit (1);
    switch (fdsize) {
    case 0: name="720k"; cyl=80; heads=2; spt=9; break;   /* 0.72 meg */
    case 1: name="1_2"; cyl=80; heads=2; spt=15; break;   /* 1.2 meg */
    case 2: name="1_44"; cyl=80; heads=2; spt=18; break;   /* 1.44 meg */
    case 3: name="2_88"; cyl=80; heads=2; spt=36; break;   /* 2.88 meg */
    default: 
      printf ("ERROR: fdsize out of range");
      exit (1);
    }
    sectors = cyl*heads*spt;
    printf ("I will create a floppy image with\n");
    printf ("  cyl=%d\n", cyl);
    printf ("  heads=%d\n", heads);
    printf ("  sectors per track=%d\n", spt);
    printf ("  total sectors=%d\n", sectors);
    printf ("  total bytes=%d\n", sectors*512);
    if (ask_string ("\nWhat should I name the image?\n[a.img] ", "a.img", filename) < 0)
      exit(1);
    sprintf (bochsrc_line, "floppya: %s=\"%s\", status=inserted", name, filename);
  }
  if (sectors < 1)
    panic ("Illegal disk size!");
  if (strlen (filename) < 1)
    panic ("Illegal filename");
  make_image (sectors, filename);
  printf ("\nI wrote %d bytes to %s.\n", sectors*512, filename);
  printf ("\nThe following line should appear in your bochsrc:\n");
  printf ("  %s\n", bochsrc_line);
}
