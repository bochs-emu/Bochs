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




#if BX_PROVIDE_CPU_MEMORY
  Bit32u
BX_MEM_C::get_memory_in_k(void)
{
  genlog->info("(%u) get_memory_in_k() = %u\n", BX_SIM_ID, (unsigned)
    (BX_MEM_THIS megabytes * 1024));

  return(BX_MEM_THIS megabytes * 1024);
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
  // BX_MEM_C constructor
BX_MEM_C::BX_MEM_C(void)
{
  BX_MEM.vector = NULL;
  BX_MEM.len    = 0;
  BX_MEM.megabytes = 0;
}
#endif // #if BX_PROVIDE_CPU_MEMORY



#if BX_PROVIDE_CPU_MEMORY
  // BX_MEM_C constructor
BX_MEM_C::BX_MEM_C(size_t memsize)
{
  genlog->info("(%u) BX_MEM_C::BX_MEM_C(size_t) called\n", BX_SIM_ID);
  genlog->info("(%u)   memsize = %u\n", BX_SIM_ID, (unsigned) memsize);
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
    delete this->vector;
    }
  else {
    genlog->info("(%u)   memory not freed as it wasn't allocated!\n", BX_SIM_ID);
    }
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if BX_PROVIDE_CPU_MEMORY
  void
BX_MEM_C::init_memory(int memsize)
{
  // you can pass 0 if memory has been allocated already through
  // the constructor, or the desired size of memory if it hasn't

  if (BX_MEM_THIS vector == NULL) {
    // memory not already allocated, do now...
    genlog->info("(%u) BX_MEM_C::init_memory(int): allocating memory.\n", BX_SIM_ID);
    genlog->info("(%u)   memsize = %u\n", BX_SIM_ID, (unsigned) memsize);
    BX_MEM_THIS vector = new Bit8u[memsize];
    BX_MEM_THIS len    = memsize;
    BX_MEM_THIS megabytes = memsize / (1024*1024);
    }
  // initialize all memory to 0x00
  memset(BX_MEM_THIS vector, 0x00, BX_MEM_THIS len);

  // initialize ROM area (0xc0000 .. 0xfffff) to 0xff
  memset(BX_MEM_THIS vector + 0xc0000, 0xff, 0x40000);

#if BX_DEBUGGER
  // initialize dirty pages table
  memset(BX_MEM.dbg_dirty_pages, 0, sizeof(BX_MEM.dbg_dirty_pages));

  if (BX_MEM.megabytes > BX_MAX_DIRTY_PAGE_TABLE_MEGS) {
    genlog->info("Error: memory larger than dirty page table can handle\n");
    bx_panic("Error: increase BX_MAX_DIRTY_PAGE_TABLE_MEGS\n");
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

  // read in ROM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    fprintf(stderr, "load_ROM: couldn't open ROM image file '%s'.\n", path);
    exit(1);
    }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    fprintf(stderr, "load_ROM: couldn't stat ROM image file '%s'.\n", path);
    exit(1);
    }

  size = stat_buf.st_size;

  if ( (romaddress + size) > BX_MEM_THIS len ) {
    fprintf(stderr, "load_ROM: ROM address range > physical memsize!\n");
    exit(1);
    }

  offset = 0;
  while (size > 0) {
#if BX_PCI_SUPPORT
    if (bx_options.i440FXSupport)
      ret = read(fd, (bx_ptr_t) &bx_pci.s.i440fx.shadow[romaddress - 0xC0000 + offset],
                 size);
    else
      ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[romaddress + offset], size);
#else
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[romaddress + offset], size);
#endif
    if (ret <= 0) {
      fprintf(stderr, "load_ROM: read failed on ROM BIOS image\n");
      exit(1);
      }
    size -= ret;
    offset += ret;
    }
  close(fd);
#if BX_PCI_SUPPORT
  if (bx_options.i440FXSupport)
    genlog->info("(%u) load_ROM: ROM BIOS in i440FX RAM '%s', size=%u read into memory at %08x\n",
            BX_SIM_ID, path, (unsigned) stat_buf.st_size, (unsigned) romaddress);
  else
    genlog->info("(%u)  load_ROM: ROM BIOS '%s', size=%u read into memory at %08x\n",
            BX_SIM_ID, path, (unsigned) stat_buf.st_size, (unsigned) romaddress);
#else  // #if BX_PCI_SUPPORT
  genlog->info("(%u) load_ROM: ROM BIOS '%s', size=%u read into memory at %08x\n",
          BX_SIM_ID, path, (unsigned) stat_buf.st_size, (unsigned) romaddress);
#endif // #if BX_PCI_SUPPORT
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if ( BX_DEBUGGER || BX_DISASM )
  Boolean
BX_MEM_C::dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  if ( (addr + len) > BX_MEM.len ) {
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
      *buf = BX_MEM.vector[addr];
#else
      if ( bx_options.i440FXSupport &&
          ((addr >= 0x000C0000) && (addr <= 0x000FFFFF)) ) {
        switch (bx_pci.rd_memType (addr)) {
          case 0x0:  // Fetch from ShadowRAM
            *buf = BX_MEM.vector[addr];
//          genlog->info("Fetching from ShadowRAM %08x, len %u !\n", (unsigned)addr, (unsigned)len);
            break;

          case 0x1:  // Fetch from ROM
            *buf = bx_pci.s.i440fx.shadow[(addr - 0xC0000)];
//          genlog->info("Fetching from ROM %08x, Data %02x \n", (unsigned)addr, *buf);
            break;
          default:
            bx_panic("dbg_fetch_mem: default case\n");
          }
        }
      else
        *buf = BX_MEM.vector[addr];
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
  if ( (addr + len) > BX_MEM.len ) {
    return(0); // error, beyond limits of memory
    }
  for (; len>0; len--) {
#if BX_SUPPORT_VGA
    if ( (addr & 0xfffe0000) == 0x000a0000 ) {
      BX_VGA_MEM_WRITE(addr, *buf);
      }
    else
#endif
      BX_MEM.vector[addr] = *buf;
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

  if (addr2 >= BX_MEM.len) {
    return(0); // error, specified address past last phy mem addr
    }
  
  len = 1 + addr2 - addr1;
  *crc = f(BX_MEM.vector + addr1, len);

  return(1);
}
