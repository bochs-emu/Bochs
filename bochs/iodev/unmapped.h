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




#define BX_BIOS_MESSAGE_SIZE 80


#if BX_USE_UM_SMF
#  define BX_UM_SMF  static
#  define BX_UM_THIS bx_unmapped.
#else
#  define BX_UM_SMF
#  define BX_UM_THIS this->
#endif



class bx_unmapped_c : public logfunctions {
public:
  bx_unmapped_c(void);
  ~bx_unmapped_c(void);
  BX_UM_SMF void   init(bx_devices_c *d);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_UM_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  struct {
    Bit8u port80;
    Bit8u port8e;

    Bit8u bios_message[BX_BIOS_MESSAGE_SIZE];
    unsigned int bios_message_i;
    } s;  // state information

  bx_devices_c *devices;
  };

extern bx_unmapped_c bx_unmapped;
