/////////////////////////////////////////////////////////////////////////
// $Id: misc_mem.cc,v 1.20 2002-07-03 17:13:29 mlerwill Exp $
/////////////////////////////////////////////////////////////////////////
//
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







#include "bochs.h"
#define LOG_THIS BX_MEM(0)->



#if BX_PROVIDE_CPU_MEMORY
  Bit32u
BX_MEM_C::get_memory_in_k(void)
{
  return(BX_MEM_THIS megabytes * 1024);
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
  // BX_MEM_C constructor
BX_MEM_C::BX_MEM_C(void)
{
  char mem[6];
  snprintf(mem, 6, "MEM%d", BX_SIM_ID);
  put(mem);
  settype(MEMLOG);

  vector = NULL;
  len    = 0;
  megabytes = 0;
}
#endif // #if BX_PROVIDE_CPU_MEMORY



#if BX_PROVIDE_CPU_MEMORY
  // BX_MEM_C constructor
BX_MEM_C::BX_MEM_C(size_t memsize)
{
  vector = new Bit8u[memsize];
  len    = memsize;
  megabytes = len / (1024*1024);
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
// BX_MEM_C destructor
BX_MEM_C::~BX_MEM_C(void)
{
  if (this-> vector != NULL) {
    delete [] this->vector;
    }
  else {
    BX_DEBUG(("(%u)   memory not freed as it wasn't allocated!", BX_SIM_ID));
    }
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
  void
BX_MEM_C::init_memory(int memsize)
{
	BX_DEBUG(("Init $Id: misc_mem.cc,v 1.20 2002-07-03 17:13:29 mlerwill Exp $"));
  // you can pass 0 if memory has been allocated already through
  // the constructor, or the desired size of memory if it hasn't

  if (BX_MEM_THIS vector == NULL) {
    // memory not already allocated, do now...
    BX_MEM_THIS vector = new Bit8u[memsize];
    BX_MEM_THIS len    = memsize;
    BX_MEM_THIS megabytes = memsize / (1024*1024);
    BX_INFO(("%.2fMB", (float)(BX_MEM_THIS megabytes) ));
    }
  // initialize all memory to 0x00
  memset(BX_MEM_THIS vector, 0x00, BX_MEM_THIS len);

  // initialize ROM area (0xc0000 .. 0xfffff) to 0xff
  memset(BX_MEM_THIS vector + 0xc0000, 0xff, 0x40000);

#if BX_DEBUGGER
  // initialize dirty pages table
  memset(dbg_dirty_pages, 0, sizeof(dbg_dirty_pages));

  if (megabytes > BX_MAX_DIRTY_PAGE_TABLE_MEGS) {
    BX_INFO(("Error: memory larger than dirty page table can handle"));
    BX_PANIC(("Error: increase BX_MAX_DIRTY_PAGE_TABLE_MEGS"));
    }
#endif

}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
  void
BX_MEM_C::load_ROM(const char *path, Bit32u romaddress)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, offset;

  if (*path == '\0')
    return;
  // read in ROM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_INFO(( "ROM: couldn't open ROM image file '%s'.", path));
    BX_EXIT(1);
    }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    BX_INFO(( "ROM: couldn't stat ROM image file '%s'.", path));
    BX_EXIT(1);
    }

  size = stat_buf.st_size;

  if ( (romaddress + size) > BX_MEM_THIS len ) {
    BX_INFO(( "ROM: ROM address range > physical memsize!"));
    BX_EXIT(1);
    }

  offset = 0;
  while (size > 0) {
#if BX_PCI_SUPPORT
    if (bx_options.Oi440FXSupport->get ())
      ret = read(fd, (bx_ptr_t) &bx_pci.s.i440fx.shadow[romaddress - 0xC0000 + offset],
                 size);
    else
      ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[romaddress + offset], size);
#else
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[romaddress + offset], size);
#endif
    if (ret <= 0) {
      BX_PANIC(( "ROM: read failed on BIOS image: '%s'",path));
      }
    size -= ret;
    offset += ret;
    }
  close(fd);
#if BX_PCI_SUPPORT
  if (bx_options.Oi440FXSupport->get ())
    BX_INFO(("rom in i440FX RAM 0x%06x/%u ('%s')",
			(unsigned) romaddress,
			(unsigned) stat_buf.st_size,
			path
		));
  else
    BX_INFO(("rom at 0x%05x/%u ('%s')",
			(unsigned) romaddress,
			(unsigned) stat_buf.st_size,
			path
		));
#else  // #if BX_PCI_SUPPORT
  BX_INFO(("rom at 0x%05x/%u ('%s')",
			(unsigned) romaddress,
			(unsigned) stat_buf.st_size,
 			path
		));
#endif // #if BX_PCI_SUPPORT
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if ( BX_DEBUGGER || BX_DISASM )
  Boolean
BX_MEM_C::dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  if ( (addr + len) > this->len ) {
    BX_INFO(("dbg_fetch_mem out of range. %p > %p",
      addr+len, this->len));
    return(0); // error, beyond limits of memory
    }
  for (; len>0; len--) {
#if BX_SUPPORT_VGA
    if ( (addr & 0xfffe0000) == 0x000a0000 ) {
      *buf = BX_VGA_MEM_READ(addr);
      }
    else {
#endif
#if BX_PCI_SUPPORT == 0
      *buf = vector[addr];
#else
      if ( bx_options.Oi440FXSupport->get () &&
          ((addr >= 0x000C0000) && (addr <= 0x000FFFFF)) ) {
        switch (bx_devices.pci->rd_memType (addr)) {
          case 0x0:  // Fetch from ShadowRAM
            *buf = vector[addr];
//          BX_INFO(("Fetching from ShadowRAM %06x, len %u !", (unsigned)addr, (unsigned)len));
            break;

          case 0x1:  // Fetch from ROM
            *buf = bx_pci.s.i440fx.shadow[(addr - 0xC0000)];
//          BX_INFO(("Fetching from ROM %06x, Data %02x ", (unsigned)addr, *buf));
            break;
          default:
            BX_PANIC(("dbg_fetch_mem: default case"));
          }
        }
      else
        *buf = vector[addr];
#endif  // #if BX_PCI_SUPPORT == 0
      }
    buf++;
    addr++;
    }
  return(1);
}
#endif

#if BX_DEBUGGER
  Boolean
BX_MEM_C::dbg_set_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  if ( (addr + len) > this->len ) {
    return(0); // error, beyond limits of memory
    }
  for (; len>0; len--) {
#if BX_SUPPORT_VGA
    if ( (addr & 0xfffe0000) == 0x000a0000 ) {
      BX_VGA_MEM_WRITE(addr, *buf);
      }
    else
#endif
      vector[addr] = *buf;
    buf++;
    addr++;
    }
  return(1);
}
#endif

  Boolean
BX_MEM_C::dbg_crc32(unsigned long (*f)(unsigned char *buf, int len),
    Bit32u addr1, Bit32u addr2, Bit32u *crc)
{
  unsigned len;

  *crc = 0;
  if (addr1 > addr2)
    return(0);

  if (addr2 >= this->len) {
    return(0); // error, specified address past last phy mem addr
    }
  
  len = 1 + addr2 - addr1;
  *crc = f(vector + addr1, len);

  return(1);
}
