#ifndef _BX_IODEBUG_H
#define _BX_IODEBUG_H

#include "config.h"

#define BX_IODEBUG_THIS this->

class bx_iodebug_c : public logfunctions {
public:
  bx_iodebug_c( void );
  ~bx_iodebug_c( void );
  int init( bx_devices_c * );
private:
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned int io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned int io_len);

  bx_devices_c *devices;
};

extern bx_iodebug_c bx_iodebug;
#endif
