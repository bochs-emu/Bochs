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

#ifndef _STATE_FILE_H
#define _STATE_FILE_H
#include <stdio.h>
#include <stddef.h>


class state_file {
public:
  FILE *file;

  FILE *get_handle();
  void write(Bit8u);
  void write(Bit16u);
  void write(Bit32u);
  void write(Bit64u);
  void write(const void*, size_t);
  void read(Bit8u &);
  void read(Bit16u &);
  void read(Bit32u &);
  void read(Bit64u &);
  void read(void *, size_t);
  void write_check(const char *);
  void read_check (const char *);

  state_file (const char *name, const char *options);
  state_file (FILE *f);
  ~state_file();
};

#endif  // #ifndef _STATE_FILE_H
