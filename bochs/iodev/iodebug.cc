/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_IODEBUG

#include "cpu/cpu.h"
#include "iodebug.h"

#define BX_IODEBUG_THIS this->

bx_iodebug_c *theIODebugDevice = NULL;

int CDECL libiodebug_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theIODebugDevice = new bx_iodebug_c();
  bx_devices.pluginIODebug = theIODebugDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theIODebugDevice, BX_PLUGIN_IODEBUG);
  return(0); // Success
}

void CDECL libiodebug_LTX_plugin_fini(void)
{
  bx_devices.pluginIODebug = &bx_devices.stubIODebug;
  delete theIODebugDevice;
}

struct bx_iodebug_s_type {
  bx_bool enabled;
  unsigned register_select;
  Bit32u registers[2];
  bx_phy_address monitored_mem_areas_start[BX_IODEBUG_MAX_AREAS];
  bx_phy_address monitored_mem_areas_end[BX_IODEBUG_MAX_AREAS];
} bx_iodebug_s;


bx_iodebug_c::bx_iodebug_c()
{
  put("iodebug", "IODBG");
}

void bx_iodebug_c::init(void)
{
  DEV_register_ioread_handler(this, read_handler, 0x8A00,"BOCHS IODEBUG", 2);
  DEV_register_iowrite_handler(this, write_handler, 0x8A00,"BOCHS IODEBUG", 2);
  DEV_register_iowrite_handler(this, write_handler, 0x8A01,"BOCHS IODEBUG", 2);

  bx_iodebug_s.enabled = 0;
  bx_iodebug_s.register_select = 0;

  for(int i=0;i<BX_IODEBUG_MAX_AREAS;i++) {
    bx_iodebug_s.monitored_mem_areas_start[i] = 0;
    bx_iodebug_s.monitored_mem_areas_end[i] = 0;
  }
}

Bit32u bx_iodebug_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_iodebug_c *bx_iodebug_ptr = (bx_iodebug_c *) this_ptr;
  return bx_iodebug_ptr->read(addr, io_len);
}

Bit32u bx_iodebug_c::read(Bit32u addr, unsigned io_len)
{
  if (bx_iodebug_s.enabled) return 0x8A00;
  return(0);
}

void bx_iodebug_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_iodebug_c *bx_iodebug_ptr = (bx_iodebug_c *) this_ptr;
  bx_iodebug_ptr->write(addr, dvalue, io_len);
}

void bx_iodebug_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
//fprintf(stderr, "IODEBUG addr: %4x\tdvalue: %8x\tio_len: %8x\n", (unsigned) addr, (unsigned) dvalue, io_len);

  if (addr == 0x8A01)
  {
    bx_iodebug_s.registers[bx_iodebug_s.register_select] =
        (bx_iodebug_s.registers[bx_iodebug_s.register_select] << 16) +
        (dvalue & 0xFFFF);
  }

  if (addr != 0x8A00) return;

  if (!bx_iodebug_s.enabled)
  {
    if(dvalue == 0x8A00)
    {
      bx_iodebug_s.enabled = 1;
//    fprintf(stderr, "IODEBUG enabled\n");
      bx_iodebug_s.registers[0] = 0;
      bx_iodebug_s.registers[1] = 0;
    }
    return;
  }

  switch(dvalue)
  {
    case 0x8A01:
      bx_iodebug_s.register_select = 0;
//    fprintf(stderr, "IODEBUG register 0 selected\n");
      break;

    case 0x8A02:
      bx_iodebug_s.register_select = 1;
//    fprintf(stderr, "IODEBUG register 1 selected\n");
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
void bx_iodebug_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  if(! bx_iodebug_s.enabled) return;

  unsigned area = bx_iodebug_c::range_test(addr, len);
  // Device is enabled, testing address ranges
  if(area)
  {
    area--;

#if BX_DEBUGGER
    if (cpu != NULL) {
      fprintf(stdout, "IODEBUG CPU %d @ eip: " FMT_ADDRX " write at monitored memory location " FMT_PHY_ADDRX "\n", 
         cpu->bx_cpuid, cpu->get_instruction_pointer(), addr);
    }
    else {
      fprintf(stdout, "IODEBUG write at monitored memory location " FMT_PHY_ADDRX "\n", addr);
    }
    bx_guard.interrupt_requested=1;
#else
    fprintf(stderr, "IODEBUG write to monitored memory area: %2i\t", area);

    if (cpu != NULL)
      fprintf(stderr, "by EIP:\t\t" FMT_ADDRX "\n\t", cpu->get_instruction_pointer());
    else
      fprintf(stderr, "(device origin)\t");

    fprintf(stderr, "range start: \t\t" FMT_PHY_ADDRX "\trange end:\t" FMT_PHY_ADDRX "\n\taddress accessed:\t%08X\tdata written:\t",
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

void bx_iodebug_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  if(! bx_iodebug_s.enabled) return;

  unsigned area = bx_iodebug_c::range_test(addr, len);
  // Device is enabled, testing address ranges
  if(area)
  {
    area--;

#if BX_DEBUGGER
    if (cpu != NULL) {
      fprintf(stdout, "IODEBUG CPU %d @ eip: " FMT_ADDRX " read at monitored memory location " FMT_PHY_ADDRX "\n", 
        cpu->bx_cpuid, cpu->get_instruction_pointer(), addr);
    }
    else {
      fprintf(stdout, "IODEBUG read at monitored memory location " FMT_PHY_ADDRX "\n", addr);
    }
    bx_guard.interrupt_requested=1;
#else
    fprintf(stderr, "IODEBUG read at monitored memory area: %2i\t", area);

    if (cpu != NULL)
      fprintf(stderr, "by EIP:\t\t" FMT_ADDRX "\n\t", cpu->get_instruction_pointer());
    else
      fprintf(stderr, "(device origin)\t");

    fprintf(stderr, "range start: \t\t" FMT_PHY_ADDRX "\trange end:\t" FMT_PHY_ADDRX "\n\taddress accessed:\t" FMT_PHY_ADDRX "\tdata written:\t",
            bx_iodebug_s.monitored_mem_areas_start[area],
            bx_iodebug_s.monitored_mem_areas_end[area],
            (unsigned) addr);

    switch(len)
    {
      case 1: {
        Bit8u data8 = * ((Bit8u *) data);
        fprintf(stderr,"%02X\n", data8);
        break;
      }
      case 2: {
        Bit16u data16 = * ((Bit16u *) data);
        fprintf(stderr,"%04X\n", data16);
        break;
      }
      case 4: {
        Bit32u data32 = * ((Bit32u *) data);
        fprintf(stderr,"%08X\n", data32);
        break;
      }
      case 8: {
        Bit64u data64 = * ((Bit64u *) data);
        fprintf(stderr,"%08X%08x\n",
            (Bit32u)(data64 >> 32),
            (Bit32u)(data64 & 0xffffffff));
        break;
      }
      default:
        fprintf(stderr, "unsupported read size\n");
    }
#endif
  }
}

unsigned bx_iodebug_c::range_test(bx_phy_address addr, unsigned len)
{
  for(unsigned i=0;i<BX_IODEBUG_MAX_AREAS;i++)
  {
    if(bx_iodebug_s.monitored_mem_areas_start[i] != 0 ||
       bx_iodebug_s.monitored_mem_areas_end[i] != 0)
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

void bx_iodebug_c::add_range(bx_phy_address addr_start, bx_phy_address addr_end)
{
  for(unsigned i=0;i<BX_IODEBUG_MAX_AREAS;i++)
  {
    if(!bx_iodebug_s.monitored_mem_areas_start[i] &&
       !bx_iodebug_s.monitored_mem_areas_end[i])
    {
        bx_iodebug_s.monitored_mem_areas_start[i] = addr_start;
        bx_iodebug_s.monitored_mem_areas_end[i] = addr_end;
//      fprintf(stderr, "IODEBUG added range successfully in slot: %i\n",i);
        return;
    }
  }
//fprintf(stderr, "IODEBUG unable to register memory range, all slots taken\n");
}

#endif /* if BX_SUPPORT_IODEBUG */
