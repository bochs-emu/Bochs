#ifndef _BX_IODEBUG_H
#define _BX_IODEBUG_H

#include "config.h"

#define BX_IODEBUG_THIS this->

#define BX_IODEBUG_MAX_AREAS	30

class bx_iodebug_c : public logfunctions
{
public:
  bx_iodebug_c( void );
  ~bx_iodebug_c( void );
  int init( bx_devices_c * );
  void mem_write( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data);
  void mem_read( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data);
private:
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned int io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned int io_len);
  unsigned int range_test(Bit32u addr, unsigned int len);

  struct s_type {
    Boolean enabled;
    unsigned int register_select;
    Bit32u monitored_mem_areas_start[BX_IODEBUG_MAX_AREAS];
    Bit32u monitored_mem_areas_end[BX_IODEBUG_MAX_AREAS];
  } s;

  bx_devices_c *devices;
};

extern bx_iodebug_c bx_iodebug;
#endif
