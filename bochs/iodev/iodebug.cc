/////////////////////////////////////////////////////////////////////////
// $Id: iodebug.cc,v 1.20 2006-03-03 12:55:37 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"
#if BX_SUPPORT_IODEBUG

#define BX_IODEBUG_THIS this->

bx_iodebug_c bx_iodebug;

  struct bx_iodebug_s_type {
    bx_bool enabled;
    unsigned register_select;
    Bit32u registers[2];
    Bit32u monitored_mem_areas_start[BX_IODEBUG_MAX_AREAS];
    Bit32u monitored_mem_areas_end[BX_IODEBUG_MAX_AREAS];
  } bx_iodebug_s;

// Constructor
bx_iodebug_c::bx_iodebug_c()
{
  put("IODEBUG");
  settype(IODEBUGLOG);
}

void bx_iodebug_c::init(void)
{
  DEV_register_ioread_handler(this, read_handler, 0x8A00,"BOCHS IODEBUG", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x8A00,"BOCHS IODEBUG", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x8A01,"BOCHS IODEBUG", 7);

  bx_iodebug_s.enabled = 0;
  bx_iodebug_s.register_select = 0;

  for(int i=0;i<BX_IODEBUG_MAX_AREAS;i++) {
    bx_iodebug_s.monitored_mem_areas_start[i] = 0;
    bx_iodebug_s.monitored_mem_areas_end[i] = 0;
  }
}

Bit32u bx_iodebug_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_iodebug_c bx_iodebug_ptr = (bx_iodebug_c *) this_ptr;
  return bx_iodebug_ptr->read(addr, io_len);
}

Bit32u bx_iodebug_c::read(Bit32u addr, unsigned io_len)
{
  if(bx_iodebug_s.enabled) return(0x8A00);
  return(0);
}

void bx_iodebug_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_iodebug_c bx_iodebug_ptr = (bx_iodebug_c *) this_ptr;
  bx_iodebug_ptr->write(addr, dvalue, io_len);
}

void bx_iodebug_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
//  fprintf(stderr, "IODEBUG addr: %4x\tdvalue: %8x\tio_len: %8x\n", (unsigned) addr, (unsigned) dvalue, io_len);

  if(addr == 0x8A01 && io_len == 2)
  {
      bx_iodebug_s.registers[bx_iodebug_s.register_select] =
        (bx_iodebug_s.registers[bx_iodebug_s.register_select] << 16) +
	(dvalue & 0xFFFF);
  }

  if((addr != 0x8A00) || (io_len != 2)) return;

  if(! bx_iodebug_s.enabled)
  {
    if(dvalue == 0x8A00)
    {
      bx_iodebug_s.enabled = 1;
//      fprintf(stderr, "IODEBUG enabled\n");
      bx_iodebug_s.registers[0] = 0;
      bx_iodebug_s.registers[1] = 0;
    }
    return;
  }

  switch(dvalue)
  {
    case 0x8A01:
      bx_iodebug_s.register_select = 0;
//      fprintf(stderr, "IODEBUG register 0 selected\n");
      break;

    case 0x8A02:
      bx_iodebug_s.register_select = 1;
//      fprintf(stderr, "IODEBUG register 1 selected\n");
      break;

    case 0x8A80:
      bx_iodebug_s.register_select = 0;
      bx_iodebug_c::add_range(bx_iodebug_s.registers[0],
	  bx_iodebug_s.registers[1]);
      bx_iodebug_s.registers[0] = 0;
      bx_iodebug_s.registers[1] = 0;
      break;

#if BX_DEBUGGER
    case 0x8AE0:
      fprintf(stderr, "request return to dbg prompt received, 0x8AE0 command (iodebug)\n");
      bx_guard.interrupt_requested=1;
      break;

    case 0x8AE2:
      fprintf(stderr, "request made by the guest os to disable tracing, iodebug port 0x8A00->0x8AE2\n");
      BX_CPU(dbg_cpu)->trace = 0;
      break;

    case 0x8AE3:
      fprintf(stderr, "request made by the guest os to enable tracing, iodebug port 0x8A00->0x8AE3\n");
      BX_CPU(dbg_cpu)->trace = 1;
      break;

    case 0x8AE4:
      fprintf(stderr, "request made by the guest os to disable register tracing, iodebug port 0x8A00->0x8AE4\n");
      BX_CPU(dbg_cpu)->trace_reg = 0;
      break;

    case 0x8AE5:
      fprintf(stderr, "request made by the guest os to enable register tracing, iodebug port 0x8A00->0x8AE5\n");
      BX_CPU(dbg_cpu)->trace_reg = 1;
      break;
#endif
    case 0x8AFF:
      bx_iodebug_s.enabled = 0;
//      fprintf(stderr, "IODEBUG device deactivated\n");
//      break;

//    default:
//      fprintf(stderr,"IODEBUG unsupported register code\n");
  }
}


// Static function
void bx_iodebug_c::mem_write(BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  if(! bx_iodebug_s.enabled) return;

  unsigned area = bx_iodebug_c::range_test(addr, len);
  // Device is enabled, testing address ranges
  if(area)
  {
    area--;
#if BX_DEBUGGER
    fprintf(stdout, "%s @ eip: " FMT_ADDRX " wrote at monitored memory location %8X\n", cpu->name, cpu->get_ip(), addr);
    bx_guard.interrupt_requested=1;
#else
    fprintf(stderr,
             "IODEBUG write to monitored memory area: %2i\tby EIP:\t\t" FMT_ADDRX "\n\trange start: \t\t%08X\trange end:\t%08X\n\taddress accessed:\t%08X\tdata written:\t",
	     area,
	     cpu->get_ip(),
	     bx_iodebug_s.monitored_mem_areas_start[area],
	     bx_iodebug_s.monitored_mem_areas_end[area],
	     (unsigned) addr);

    switch(len)
    {
      case 1: {
        Bit8u data8 = * ((Bit8u *) data);
        fprintf(stderr,"%02X\n", (unsigned) data8);
	break;
      }
      case 2: {
        Bit16u data16 = * ((Bit16u *) data);
        fprintf(stderr,"%04X\n", (unsigned) data16);
	break;
      }
      case 4: {
        Bit32u data32 = * ((Bit32u *) data);
        fprintf(stderr,"%08X\n", (unsigned) data32);
	break;
      }
      case 8: {
        Bit64u data64 = * ((Bit64u *) data);
        fprintf(stderr,"%08X%08x\n", 
            (unsigned) (data64 >> 32),
            (unsigned) (data64 & 0xffffffff));
	break;
      }
      default:
        fprintf(stderr, "unsupported write size\n");
    }
#endif
  }
}

void bx_iodebug_c::mem_read(BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  if(! bx_iodebug_s.enabled) return;

  unsigned area = bx_iodebug_c::range_test(addr, len);
  // Device is enabled, testing address ranges
  if(area)
  {
    area--;
#if BX_DEBUGGER
    fprintf(stdout, "%s @ eip: " FMT_ADDRX " wrote at monitored memory location %8X\n", cpu->name, cpu->get_ip(), addr);
    bx_guard.interrupt_requested=1;
#else
    fprintf(stderr,
             "IODEBUG read to monitored memory area: %2i\tby EIP:\t\t" FMT_ADDRX "\n\trange start: \t\t%08X\trange end:\t%08X\n\taddress accessed:\t%08X\tdata written:\t",
	     area,
	     cpu->get_ip(),
	     bx_iodebug_s.monitored_mem_areas_start[area],
	     bx_iodebug_s.monitored_mem_areas_end[area],
	     (unsigned) addr);

    switch(len)
    {
      case 1: {
        Bit8u data8 = * ((Bit8u *) data);
        fprintf(stderr,"%02X\n", (unsigned) data8);
	break;
      }
      case 2: {
        Bit16u data16 = * ((Bit16u *) data);
        fprintf(stderr,"%04X\n", (unsigned) data16);
	break;
      }
      case 4: {
        Bit32u data32 = * ((Bit32u *) data);
        fprintf(stderr,"%08X\n", (unsigned) data32);
	break;
      }
      case 8: {
        Bit64u data64 = * ((Bit64u *) data);
        fprintf(stderr,"%08X%08x\n", 
            (unsigned) (data64 >> 32),
            (unsigned) (data64 & 0xffffffff));
	break;
      }
      default:
        fprintf(stderr, "unsupported read size\n");
    }
#endif
  }
}

unsigned bx_iodebug_c::range_test(Bit32u addr, unsigned len)
{
  for(unsigned i=0;i<BX_IODEBUG_MAX_AREAS;i++)
  {
    if((bx_iodebug_s.monitored_mem_areas_start[i]!=0) ||
       (bx_iodebug_s.monitored_mem_areas_end[i]!=0))
    {
      if((Bit32u)(addr+len-1) < bx_iodebug_s.monitored_mem_areas_start[i])
        continue;

      if(addr < bx_iodebug_s.monitored_mem_areas_end[i])
      {
        return(++i);
      }
    }	
  }

  return(0);
}


void bx_iodebug_c::add_range(Bit32u addr_start, Bit32u addr_end)
{
  for(unsigned i=0;i<BX_IODEBUG_MAX_AREAS;i++)
  {
    if( !bx_iodebug_s.monitored_mem_areas_start[i] &&
        !bx_iodebug_s.monitored_mem_areas_end[i] )
    {
	bx_iodebug_s.monitored_mem_areas_start[i] = addr_start;
	bx_iodebug_s.monitored_mem_areas_end[i] = addr_end;
//	fprintf(stderr, "IODEBUG added range successfully in slot: %i\n",i);
	return;
    }
  }
//  fprintf(stderr, "IODEBUG unable to register memory range, all slots taken\n");
}

#endif /* if BX_SUPPORT_IODEBUG */
