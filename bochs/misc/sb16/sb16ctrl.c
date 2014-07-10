/////////////////////////////////////////////////////////////////////////
// $Id: sb16ctrl.c,v 1.7 2009-02-08 09:05:52 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// This file (SB16CTRL.C) written and donated by Josef Drexler




// The purpose of this program is to provide runtime configuration
// options to the SB16 Emulator until Bochs has a more sophisticated
// user interface allowing to do these things better.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef dos
#	include <dos.h>
#endif

#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#	include <sys/io.h>
#	include <errno.h>
#	define inp inb
#	define outp(a,b) outb(b,a)
#endif

/*
#define DEBUG
*/

#define EMULPORT       0x333

int checked = 0;
int filelevel = 0;
int verbose = 0;
char ofline[256] = " in the command line";

/******************************
 * System dependent functions *
 ******************************/

/* Read a value from the emulator port */
int reademul()
{
  return inp(EMULPORT);
}

/* Write a value to the emulator port */
void writeemul(int value)
{
  outp(EMULPORT, value);
}

/* Enable access to the emulator port */
void enableport()
{
#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
  if (ioperm(EMULPORT, 1, 1)) {
    printf("Could not access emulator port %03x: %s.\n", EMULPORT, strerror(errno));
    exit(1);
  }
#endif
}

/********************************
 * System independent functions *
 ********************************/

/* Forward definitions*/
void command(int cmd, char *arg);
void checkemulator();

/* Convert a string into a number */
int strtoi(char *nptr, char **endptr, int base)
{
  int value, digit;
  int sign = 1;

  /* Skip leading white space */
  while( isspace(*nptr) )
    nptr++;

  /* Check if there is a sign */
  if (*nptr == '-')
    {
      sign = -1;
      nptr++;
    }
  else if (*nptr == '+')
    {
      sign = 1;
      nptr++;
    }

  /* Check the base if undefined and determine it */
  if (base == 0)
    {
      if (*nptr == '0')
	{
	  nptr++;
	  if ( (*nptr == 'x') ||
	       (*nptr == 'X') )
	    {
	      nptr++;
	      base = 16;
	    }
	  else
	    base = 8;
	}
      else
	base = 10;
    }

  /* Convert the number */
  value = 0;
  digit = -1;
  while ( isalnum(*nptr) )
    {
      if ( isdigit(*nptr) )
	digit = *nptr - '0';
      else
	digit = tolower(*nptr) - 'a' + 10;

      if ( (digit >= base) || (digit < 0) )
	break;     /* Isn't a valid char, abort conversion */

      value = value * base + digit;
      nptr++;
    }

  if (endptr != NULL)
    *endptr = nptr;

  return sign * value;
}

/* Print the command line usage */
void usage()
{
  printf("SB16 emulator control program for Bochs.\n"
         "\n"
	 "Usage: sb16ctrl [-i #] [-t #,#,#,#,#,#] [-r] [-m #,..] [-f filename]\n"
	 "\n"
	 "This program is used to control the operation of the SB16 emulator\n"
	 "until it has a more sophisticated interface.\n"
	 "Any number of commands can be given. If none are present \"-f -\"\n"
	 "is assumed.\n"
	 "\n"
	 "-i # show the selected emulator info string\n"
	 "-t #,#,#,#,#,# load the patch translation into the emulator\n"
	 "-r resets the patch translation table\n"
	 "-m #,... sends the given midi message (up to 255 bytes)\n"
	 "-f filename loads commands from the given file, or stdin if \"-\"\n"
	 "-v be verbose\n"
	 "\n"
	 "# can be decimal, octal (first digit 0) or hexadecimal (0x..)\n"
	 "\n"
	 );
}

/* Execute the given command */
void emulcommand(int command)
{
  if (checked == 0)
    checkemulator();

  writeemul(command);
}

/* Check if we got the expected response */
int testemul(int value, char *msg)
{
  int i;

  if (checked == 0)
    checkemulator();

  i = reademul();
#ifndef DEBUG
  if ( (i != value) && (msg) )
    {
      printf("Bochs emulator/SB16 error: %s\n", msg);
      exit(1);
    }
#endif
  return i;
}

/* Check if we are running inside the emulator */
void checkemulator()
{
  int i;

  checked = 1;

  enableport();

  /* Check emulator status */
  for (i=0; i<9; i++)
    writeemul(1);          /* Clear input queue */
  writeemul(10);           /* Check Emulator present */
  testemul(254, "no check ACK: Emulator not present");
  testemul(0x55, "Emulator not present");    /* should return 0x55 */
}

/* Read internal emulator string and print it */
void showemul(int i)
{
  int j;

  emulcommand(5);   /* 5 means dump info */
  writeemul(i);   /* Argument to command 5; info number */
  testemul(254, "no info ACK");
  printf("Emulator info string %d:", i);
  do {
    j = reademul();
    if (j == 255)
      break;
    printf("%c", j);
  } while (j != 10);
}

/* Process a string - change "," and "/" into a space,
   and ignore everything after comments "#" */
void procstr(char *str)
{
  int i;

  for (i=0; str[i] != 0; i++)
    switch(str[i])
      {
      case ',':
      case '/':
      case 10:
      case 13:
	str[i] = ' ';
	break;
      case '#':
	str[i] = 0;
	str[i+1] = 0;
	break;
      }
}

/* Upload a mapping to the emulator */
void loadmap(char *map)
{
  int i;
  int trans[6];
  char *nextnum;

  procstr(map);

  nextnum = map;
  for (i=0;i<6;i++) {
    if (nextnum) {
      trans[i] = strtoi(nextnum, &nextnum, 0);
      if ( (!nextnum) ||
	   ( (trans[i] > 127) && (trans[i] < 255) ) ||
	   (trans[i] > 255) )
	printf("Parse error in value %d%s. Command ignored.", i, ofline);
    }
    if (!nextnum)
      trans[i] = 255;
  }

  /* Load the remap into the emulator, command 4 */
  emulcommand(4);   /* 4 load remap */
  testemul(254, "no load remap ACK");

  for (i=0;i<6;i++)
    writeemul(trans[i]);

  testemul(6, "insufficient data");	     /* test receipt of 6 arguments */
}

/* Reset the translation table */
void resettable()
{
  emulcommand(7);
  testemul(254, "no table reset ACK");
}

/* Send a series of midi bytes to the sound device */
void loadmidi(char *msg)
{
  int i;

  procstr(msg);

  while ( (msg) && (*msg) ) {
    i = strtoi(msg, &msg, 0);

    emulcommand(11);     /* 11: Send midi data byte */
    writeemul(i);
    testemul(254, "no midi data ACK");
  }
}

/* read a file of commands */
void loadfile(char *filename)
{
  FILE *file;
  char cmd;
  char *pos;
  char msg[256];
  int lineno = 0;

  filelevel++;
  if (filelevel > 10)
    {
      printf("Error - Too many nested \"f\" commands.\n");
      exit(1);
    }

  if (strcmp(filename, "-") == 0)
    file = stdin;
  else
    file = fopen(filename, "r");

  if (!file) {
    printf("File %s not found.\n", filename);
    return;
  }

  while (!feof(file)) {
    fgets(msg, sizeof msg, file);
    lineno++;

    pos = msg;

    procstr(msg);

    while ( (*pos == ' ') || (*pos == 9) )
      pos++;

    if (*pos)
      cmd = *pos++;
    else
      continue;

    if (cmd == '#')   /* it's a comment */
      continue;
    while ( (*pos == ' ') || (*pos == 9) )
      pos++;

    sprintf(ofline, " in line %d of file %s", lineno, filename);

    command(cmd, pos);
  }
  if (strcmp(filename, "-") != 0)
    fclose(file);

  filelevel--;
}

void command(int cmd, char *arg)
{
  if (verbose)
    {
      if (arg)
	printf("Executing command %c %s%s\n", cmd, arg, ofline);
      else
	printf("Executing command %c %s\n", cmd, ofline);
    }

  switch (cmd)
    {
    case 't':
      loadmap(arg);
      break;

    case 'i':
      showemul(strtoi(arg, NULL, 0));
      break;

    case 'm':
      loadmidi(arg);
      break;

    case 'f':
      loadfile(arg);
      break;

    case 'h':
      usage();
      break;

    case 'r':
      resettable();
      break;

    default:
      printf("Command %c %s not recognized.\n", cmd, ofline);
      break;
    }
}

int main(int argc, char **argv)
{
  int i, opt, optargnum;
  char *optarg;

  /* No args given, read from stdin */
  if (argc < 2)
    {
      loadfile("-");
      return 0;
    }

  /* Check command line*/
  i = 1;
  while (i < argc)
    {
      if ( (argv[i][0] != '/') &&
	   (argv[i][0] != '-') )
	{
	  printf("Unknown command '%s'.\n"
		 "sb16ctrl -h gives a list of command line options.\n",
		 argv[i]);
	  return 1;
	}

      optargnum = -1;

      opt = argv[i++][1];

      switch (opt) {
	case 'h':
		usage();
		return 0;

        case 'v':
	        verbose++;
		break;

	case 'r':
		optargnum = 0;

	case 't':
	case 'i':
	case 'm':
	case 'f':
		if (optargnum == -1)
		  optargnum = 1;

		/* Fallthrough for all commands to here */
		if (optargnum > 0)
		  {
		    if ( (i >= argc) ||
			 (argv[i][0] == '-') ||
			 (argv[i][0] == '/') )
		      {
			printf("Option '%c' needs an argument.\n"
			       "sb16ctrl -h gives a list of command line options.\n",
			       opt);
			return 1;
		      }
		    optarg = argv[i++];
		  }
		else
		  optarg = NULL;

		command(opt, optarg);
		break;

	default:
	  printf("Unknown option '%c'.\n"
		 "sb16ctrl -h gives a list of command line options.\n",
		 opt);
	  return 1;
      }
    }

  return 0;
}
