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


#if BX_USE_PAR_SMF
#  define BX_PAR_SMF  static
#  define BX_PAR_THIS bx_parallel.
#else
#  define BX_PAR_SMF
#  define BX_PAR_THIS this->
#endif




class bx_parallel_c {
public:

  bx_parallel_c(void);
  ~bx_parallel_c(void);
  BX_PAR_SMF void   init(bx_devices_c *);

private:

  struct {
    unsigned unused;  // filler for now
    } s;

  bx_devices_c *devices;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PAR_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  };


#if BX_USE_PAR_SMF
extern bx_parallel_c bx_parallel;
#endif
