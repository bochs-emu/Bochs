//  Copyright (C) 2001  MandrakeSoft S.A.
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
#define LOG_THIS log->



FILE *state_file::get_handle()
{
  BX_INFO(("state_file::get_handle()"));
  return NULL;
}
	
void state_file::write(Bit8u)
{
  BX_PANIC(("state_file::write(Bit8u)"));
}

void state_file::write(Bit16u)
{
  BX_PANIC(("state_file::write(Bit16u)"));
}

void state_file::write(Bit32u)
{
  BX_PANIC(("state_file::write(Bit32u)"));
}

void state_file::write(Bit64u)
{
  BX_PANIC(("state_file::write(Bit64u)"));
}

void state_file::write(const void *, size_t)
{
  BX_PANIC(("state_file::write(const void *, size_t)"));
}

void state_file::read(Bit8u &)
{
  BX_PANIC(("state_file::read(uint8 &)"));
}

void state_file::read(Bit16u &)
{
  BX_PANIC(("state_file::read(uint16 &)"));
}

void state_file::read(Bit32u &)
{
  BX_PANIC(("state_file::read(uint32 &)"));
}

void state_file::read(Bit64u &)
{
  BX_PANIC(("state_file::read(uint64 &)"));
}

void state_file::read(void *, size_t)
{
  BX_PANIC(("state_file::read(void *, size_t)"));
}

void state_file::write_check(const char *)
{
  BX_PANIC(("state_file::write_check()"));
}

void state_file::read_check (const char *)
{
  BX_PANIC(("state_file::read_check()"));
}

void
state_file::init(void)
{
	log = new class logfunctions();
	log->setprefix("STAT");
	log->settype(GENLOG);
}


state_file::state_file (const char *name, const char *options)
{
  UNUSED(name);
  UNUSED(options);
  init();
  BX_DEBUG(( "Init(const char *, const char *)." ));
}

state_file::state_file (FILE *f)
{
  UNUSED(f);
  init();
  BX_INFO(("Init(FILE *)."));
}

state_file::~state_file()
{
  BX_DEBUG(("Exit."));
}
