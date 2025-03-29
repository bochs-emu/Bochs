/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2020 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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

// Compile using:
// g++ -I. -I./instrument/stubs -DBX_STANDALONE_DECODER bxdisasm.cc cpu/decoder/*.cc -o bxdisasm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "cpu/decoder/instr.h"

bool is_32 = 1;
bool is_64 = 0;

unsigned char char2byte(unsigned char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;

  return 0; // wrong input, ignore
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(Bit8u* target, const char* src, unsigned len)
{
  while(len >= 2 && src[0] && src[1])
  {
    *(target++) = (unsigned) char2byte(src[0])*16 + (unsigned) char2byte(src[1]);
    src += 2;
    len -= 2;
  }
}

unsigned disasm_next_opcode(const Bit8u *iptr, bool is_32, bool is_64)
{
  char disbuf[256];

  bxInstruction_c i;
  disasm(iptr, is_32, is_64, disbuf, &i, 0, 0, BX_DISASM_INTEL);
  printf("disasm: %s (opcode handler=%s)\n", disbuf, i.getIaOpcodeName());

  disasm(iptr, is_32, is_64, disbuf, &i, 0, 0, BX_DISASM_GAS);
  printf("disasm: %s (opcode handler=%s)\n", disbuf, i.getIaOpcodeName());

  return i.ilen();
}

#define MAX_LENGTH_SUPPORTED 0x1000

void disasm_string(const char *s)
{
  Bit8u ibuf[MAX_LENGTH_SUPPORTED] = {0};

  int len = strlen(s);
  if (len > MAX_LENGTH_SUPPORTED)
    len = MAX_LENGTH_SUPPORTED;

  hex2bin(ibuf, s, len);
  len /= 2; // length in bytes

  char disbuf[256];

  const Bit8u *iptr = ibuf;
  while(len > 0) {
    unsigned bytes = (len > 16) ? 16 : len;
    printf("instruction bytes:");
    for (int i=0;i<bytes;i++)
      printf("%02x", iptr[i]);
    printf("\n");

    unsigned ilen = disasm_next_opcode(iptr, is_32, is_64);
    if (ilen == 0) break;

    len -= ilen;
    iptr += ilen;
  }
}

int main(int argn, const char **argv)
{
  for (int i=1;i<argn;i++) {
    if (i == argn)
    {
      printf("Usage: bxdisasm [-16|-32|-64] string-of-instruction-bytes\n");
      exit(1);
    }

    if (!strcmp(argv[i], "/16")) {
      is_32 = 0;
      is_64 = 0;
      printf("16 bit mode\n");
      continue;
    }
    if (!strcmp(argv[i], "/32")) {
      is_32 = 1;
      is_64 = 0;
      printf("32 bit mode\n");
      continue;
    }
    if (!strcmp(argv[i], "/64")) {
      is_32 = 1;
      is_64 = 1;
      printf("64 bit mode\n");
      continue;
    }

    disasm_string(argv[i]);
  }
}
