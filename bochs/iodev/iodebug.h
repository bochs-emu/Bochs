/////////////////////////////////////////////////////////////////////////
// $Id: iodebug.h,v 1.7 2002-10-26 03:53:22 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
#ifndef _BX_IODEBUG_H
#define _BX_IODEBUG_H

#include "config.h"

#define BX_IODEBUG_THIS this->

#define BX_IODEBUG_MAX_AREAS	30

class bx_iodebug_c : public bx_devmodel_c
{
public:
  bx_iodebug_c( void );
  ~bx_iodebug_c( void );
  virtual void init(void);
  virtual void reset (unsigned type);
  static void mem_write( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data);
  static void mem_read( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data);

private:
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned int io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned int io_len);
  static unsigned int range_test(Bit32u addr, unsigned int len);
  static void add_range( Bit32u addr_start, Bit32u addr_end);

};

extern bx_iodebug_c bx_iodebug;
#endif
