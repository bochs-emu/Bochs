/////////////////////////////////////////////////////////////////////////
// $Id: parallel.h,v 1.9 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
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

typedef struct {
  Bit8u data;
  struct {
    Boolean error;
    Boolean slct;
    Boolean pe;
    Boolean ack;
    Boolean busy;
  } STATUS;
  struct {
    Boolean strobe;
    Boolean autofeed;
    Boolean init;
    Boolean slct_in;
    Boolean irq;
    Boolean input;
  } CONTROL;
  FILE *output;
  Boolean initmode;
} bx_par_t;



class bx_parallel_c : public logfunctions {
public:

  bx_parallel_c(void);
  ~bx_parallel_c(void);
  BX_PAR_SMF void   init(bx_devices_c *);
  BX_PAR_SMF void   reset(unsigned type);

private:
  bx_par_t s;

  bx_devices_c *devices;

  static void   virtual_printer();

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PAR_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  };


extern bx_parallel_c bx_parallel;
