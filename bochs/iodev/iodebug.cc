#include "bochs.h"

bx_iodebug_c bx_iodebug;

// Constructor
bx_iodebug_c::bx_iodebug_c( void )
{
  put("IODEBUG");
  settype(IODEBUGLOG);
}

// Destructor
bx_iodebug_c::~bx_iodebug_c( void )
{
}

int bx_iodebug_c::init( bx_devices_c *d )
{
  BX_IODEBUG_THIS devices = d;
  BX_IODEBUG_THIS devices->register_io_read_handler(this, read_handler, 0x8A00,"BOCHS IODEBUG");
  BX_IODEBUG_THIS devices->register_io_read_handler(this, read_handler, 0x8A01,"BOCHS IODEBUG");

  BX_IODEBUG_THIS devices->register_io_write_handler(this, write_handler, 0x8A00,"BOCHS IODEBUG");
  BX_IODEBUG_THIS devices->register_io_write_handler(this, write_handler, 0x8A01,"BOCHS IODEBUG");
  fprintf( stderr, "IODEBUG initialized\n");

  return(1);
}

Bit32u bx_iodebug_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_iodebug_c *class_ptr = (bx_iodebug_c *) this_ptr;
  return( class_ptr->read(addr, io_len) );
}

Bit32u bx_iodebug_c::read( Bit32u addr, unsigned io_len )
{
  return(0x8A00);
}

void bx_iodebug_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_iodebug_c *class_ptr = (bx_iodebug_c *) this_ptr;
  class_ptr->write( addr, dvalue, io_len );
}

void bx_iodebug_c::write( Bit32u addr, Bit32u dvalue, unsigned int io_len )
{
  fprintf(stderr, "IODEBUG addr: %4x\tdvalue: %8x\tio_len: %8x\n", (unsigned int)addr, (unsigned int)dvalue, io_len);
}
