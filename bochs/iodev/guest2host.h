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



#define BX_MAX_G2H_CHANNELS 8
#define BX_G2H_ERROR        ((unsigned) -1)
  // IO port number for this interface.  Align on dword boundary.
#define BX_G2H_PORT  0x2000
  // Magic number which is first dword passed by guest
#define BX_G2H_MAGIC 0xffeeddcc
  // Number of dwords in packet from guest
#define BX_G2H_PACKET_SIZE 5



typedef Bit32u bx_guest_packet_t[BX_G2H_PACKET_SIZE];
typedef void (*bx_g2h_callback_t)(bx_guest_packet_t *);



class bx_g2h_c {
public:
  bx_g2h_c(void);
  ~bx_g2h_c(void);
  static void   init(bx_devices_c *d);
  unsigned aquire_channel(bx_g2h_callback_t);
  unsigned deaquire_channel(unsigned channel);

private:

  static Bit32u inp_handler(void *this_ptr, Bit32u addr, unsigned io_len);
  static void   outp_handler(void *this_ptr, Bit32u addr,
                              Bit32u value, unsigned io_len);
  // state info
  struct {
    struct {
      bx_g2h_callback_t f;
      Boolean used;
      } callback[BX_MAX_G2H_CHANNELS];

    // Define the data received from the guest OS.
    //   dword0: magic number, should be BX_G2H_MAGIC
    //   dword1: channel ID
    //   dword2: address of data structure in guest physical memory
    //   dword3: size of data structure in guest physical memory
    //   dword4: address of return data structure in guest physical memory
    unsigned packet_count;
    bx_guest_packet_t guest_packet;
    } s;
  };

extern bx_g2h_c bx_g2h;
