/////////////////////////////////////////////////////////////////////////
// $Id: cmos.h,v 1.6 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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





#if BX_USE_CMOS_SMF
#  define BX_CMOS_SMF  static
#  define BX_CMOS_THIS bx_cmos.
#else
#  define BX_CMOS_SMF
#  define BX_CMOS_THIS this->
#endif


class bx_cmos_c : public logfunctions {
public:
  bx_cmos_c(void);
  ~bx_cmos_c(void);

  BX_CMOS_SMF void init(bx_devices_c *);
  BX_CMOS_SMF void checksum_cmos(void);
  BX_CMOS_SMF void reset(unsigned type);

  struct {
    int     periodic_timer_index;
    Bit32u  periodic_interval_usec;
    int     one_second_timer_index;
    time_t  timeval;
    Bit8u   cmos_mem_address;

    Bit8u   reg[BX_NUM_CMOS_REGS];
    } s;  // state information

private:
  bx_devices_c *devices;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_CMOS_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned len);
#endif

public:
  static void periodic_timer_handler(void *);
  static void one_second_timer_handler(void *);
  BX_CMOS_SMF void periodic_timer(void);
  BX_CMOS_SMF void one_second_timer(void);
private:
  BX_CMOS_SMF void update_clock(void);
  BX_CMOS_SMF void CRA_change(void);
  };


extern bx_cmos_c bx_cmos;
