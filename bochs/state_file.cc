//  Copyright (C) 2000  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA



// Classes for helping to make checkpoints of the emulator state.



#include "bochs.h"


FILE *state_file::get_handle()
{
  fprintf(stderr, "# state_file::get_handle()\n");
  return NULL;
}
	
void state_file::write(Bit8u)
{
  fprintf(stderr, "# state_file::write(Bit8u)\n");
  exit(1);
}

void state_file::write(Bit16u)
{
  fprintf(stderr, "# state_file::write(Bit16u)\n");
  exit(1);
}

void state_file::write(Bit32u)
{
  fprintf(stderr, "# state_file::write(Bit32u)\n");
  exit(1);
}

void state_file::write(Bit64u)
{
  fprintf(stderr, "# state_file::write(Bit64u)\n");
  exit(1);
}

void state_file::write(const void *, size_t)
{
  fprintf(stderr, "# state_file::write(const void *, size_t)\n");
  exit(1);
}

void state_file::read(Bit8u &)
{
  fprintf(stderr, "# state_file::read(uint8 &)\n");
  exit(1);
}

void state_file::read(Bit16u &)
{
  fprintf(stderr, "# state_file::read(uint16 &)\n");
  exit(1);
}

void state_file::read(Bit32u &)
{
  fprintf(stderr, "# state_file::read(uint32 &)\n");
  exit(1);
}

void state_file::read(Bit64u &)
{
  fprintf(stderr, "# state_file::read(uint64 &)\n");
  exit(1);
}

void state_file::read(void *, size_t)
{
  fprintf(stderr, "# state_file::read(void *, size_t)\n");
  exit(1);
}

void state_file::write_check(const char *)
{
  fprintf(stderr, "# state_file::write_check()\n");
  exit(1);
}

void state_file::read_check (const char *)
{
  fprintf(stderr, "# state_file::read_check()\n");
  exit(1);
}

state_file::state_file (const char *name, const char *options)
{
UNUSED(name);
UNUSED(options);
  bx_printf("state_file::state_file()\n");
}

state_file::state_file (FILE *f)
{
UNUSED(f);
  bx_printf("state_file::state_file(FILE *)\n");
}

state_file::~state_file()
{
  bx_printf("state_file::~state_file()\n");
}
