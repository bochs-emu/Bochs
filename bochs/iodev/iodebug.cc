#include "bochs.h"

bx_iodebug_c bx_iodebug;

// Constructor
bx_iodebug_c::bx_iodebug_c( void )
{
  put("IODEBUG");
  settype(IODEBUGLOG);
  memset(&s, 0, sizeof(s));
}

// Destructor
bx_iodebug_c::~bx_iodebug_c( void )
{
}

int bx_iodebug_c::init( bx_devices_c *d )
{
  int i;

  BX_IODEBUG_THIS devices = d;
  BX_IODEBUG_THIS devices->register_io_read_handler(this, read_handler, 0x8A00,"BOCHS IODEBUG");
  BX_IODEBUG_THIS devices->register_io_read_handler(this, read_handler, 0x8A01,"BOCHS IODEBUG");

  BX_IODEBUG_THIS devices->register_io_write_handler(this, write_handler, 0x8A00,"BOCHS IODEBUG");
  BX_IODEBUG_THIS devices->register_io_write_handler(this, write_handler, 0x8A01,"BOCHS IODEBUG");
  fprintf( stderr, "IODEBUG initialized\n");

  BX_IODEBUG_THIS s.enabled = 0;
  BX_IODEBUG_THIS s.register_select = 0;
  for(i=0;i<BX_IODEBUG_MAX_AREAS;i++) {
    BX_IODEBUG_THIS s.monitored_mem_areas_start[i] = 0;
    BX_IODEBUG_THIS s.monitored_mem_areas_end[i] = 0;
  }

  return(1);
}

Bit32u bx_iodebug_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_iodebug_c *class_ptr = (bx_iodebug_c *) this_ptr;
  return( class_ptr->read(addr, io_len) );
}

Bit32u bx_iodebug_c::read( Bit32u addr, unsigned io_len )
{

  if(BX_IODEBUG_THIS s.enabled) return(0x8A00);
  return(0);
}

void bx_iodebug_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_iodebug_c *class_ptr = (bx_iodebug_c *) this_ptr;
  class_ptr->write( addr, dvalue, io_len );
}

void bx_iodebug_c::write( Bit32u addr, Bit32u dvalue, unsigned int io_len )
{

  fprintf(stderr, "IODEBUG addr: %4x\tdvalue: %8x\tio_len: %8x\n", (unsigned int)addr, (unsigned int)dvalue, io_len);

  if( addr == 0x8A00 && dvalue == 0x8A00 )
  {
    if( BX_IODEBUG_THIS s.enabled )
      fprintf( stderr, "IODEBUG device already activated\n");
    else
    {
      BX_IODEBUG_THIS s.enabled=1;
      fprintf( stderr, "IODEBUG device activated\n");
    }
  }

  else if ( BX_IODEBUG_THIS s.enabled )
  {

    // Register Select port
    if( addr == 0x8A00 )
    {
      if( dvalue == 0x8AFF )
      {
        BX_IODEBUG_THIS s.enabled = 0;
	fprintf( stderr, "IODEBUG device deactivated\n");
      }
      fprintf( stderr, "IODEBUG register selected: %8x\n", dvalue);
      BX_IODEBUG_THIS s.register_select = dvalue;
    }

    // Data port
    else
    {
      fprintf( stderr, "IODEBUG value written to register %2x: %8x\n", BX_IODEBUG_THIS s.register_select, dvalue);
    }
  }
}





void bx_iodebug_c::mem_write( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  Bit32u data32;
  Bit16u data16;
  Bit8u  data8;

  int area;
  if( !BX_IODEBUG_THIS s.enabled ) return;

  // Device is enabled, testing address ranges
  if( area = BX_IODEBUG_THIS range_test( addr, len ) )
  {
    area--;
    fprintf( stderr,
             "IODEBUG write to monitored memory area %i\n\trange start: %8x\trange end: %8x\n\taddress accessed: %8x\tdata written: ",
	     area,
	     BX_IODEBUG_THIS s.monitored_mem_areas_start[area],
	     BX_IODEBUG_THIS s.monitored_mem_areas_end[area],
	     (unsigned int)addr);
    data32 = * (Bit32u *)data;
    data16 = (Bit16u)data32;
    data8  = (Bit8u)data32;

    switch(len)
    {
      case(1):
        fprintf(stderr,"%2x\n", (unsigned int)data8);
	break;

      case(2):
        fprintf(stderr,"%4x\n", (unsigned int)data16);
	break;

      case(4):
        fprintf(stderr,"%8x\n", (unsigned int)data32);
	break;

      default:
        fprintf(stderr, "unsupported write size\n");
    }
  }
}





void bx_iodebug_c::mem_read( BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  if( !BX_IODEBUG_THIS s.enabled ) return;

  // Device is enabled, testing address range
}




unsigned int bx_iodebug_c::range_test( Bit32u addr, unsigned int len )
{
  int i;
  for(i=0;i<BX_IODEBUG_MAX_AREAS;i++)
  {
    if( BX_IODEBUG_THIS s.monitored_mem_areas_start[i] ||
        BX_IODEBUG_THIS s.monitored_mem_areas_end[i] )
    {
      if( (addr+len-1) < BX_IODEBUG_THIS s.monitored_mem_areas_start[i] )
        continue;
      if( addr > BX_IODEBUG_THIS s.monitored_mem_areas_end[i] )
        return(++i);
    }	
  }
}
