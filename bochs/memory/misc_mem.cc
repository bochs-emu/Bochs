/////////////////////////////////////////////////////////////////////////
// $Id: misc_mem.cc,v 1.105 2007-11-02 23:30:07 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  I/O memory handlers API Copyright (C) 2003 by Frank Cornelis
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
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM(0)->

#if BX_PROVIDE_CPU_MEMORY

Bit32u BX_MEM_C::get_memory_in_k(void)
{
  return(BX_MEM_THIS megabytes * 1024);
}

Bit32u BX_MEM_C::get_num_allocated_pages(void)
{
  return(BX_MEM_THIS len / 4096);
}

BX_MEM_C::BX_MEM_C()
{
  char mem[6];
  snprintf(mem, 6, "MEM0");
  put(mem);
  settype(MEMLOG);

  vector = NULL;
  actual_vector = NULL;
  len    = 0;
  megabytes = 0;

  memory_handlers = NULL;
}

void BX_CPP_AttrRegparmN(2)
BX_MEM_C::alloc_vector_aligned (size_t bytes, size_t alignment)
{
  if (BX_MEM_THIS actual_vector != NULL) {
    BX_INFO (("freeing existing memory vector"));
    delete [] BX_MEM_THIS actual_vector;
    BX_MEM_THIS actual_vector = NULL;
    BX_MEM_THIS vector = NULL;
  }
  Bit64u test_mask = alignment - 1;
  BX_MEM_THIS actual_vector = new Bit8u [(unsigned int)(bytes+test_mask)];
  // round address forward to nearest multiple of alignment.  Alignment 
  // MUST BE a power of two for this to work.
  Bit64u masked = ((Bit64u)(BX_MEM_THIS actual_vector + test_mask)) & ~test_mask;
  BX_MEM_THIS vector = (Bit8u *)masked;
  // sanity check: no lost bits during pointer conversion
  BX_ASSERT (sizeof(masked) >= sizeof(BX_MEM_THIS vector));
  // sanity check: after realignment, everything fits in allocated space
  BX_ASSERT (BX_MEM_THIS vector+bytes <= BX_MEM_THIS actual_vector+bytes+test_mask);
  BX_INFO (("allocated memory at %p. after alignment, vector=%p", 
	BX_MEM_THIS actual_vector, BX_MEM_THIS vector));
}

BX_MEM_C::~BX_MEM_C()
{
  cleanup_memory();
}

void BX_MEM_C::init_memory(int memsize)
{
  unsigned idx;

  BX_DEBUG(("Init $Id: misc_mem.cc,v 1.105 2007-11-02 23:30:07 vruppert Exp $"));

  alloc_vector_aligned(memsize+ BIOSROMSZ + EXROMSIZE  + 4096, BX_MEM_VECTOR_ALIGN);
  BX_MEM_THIS len  = memsize;
  BX_MEM_THIS megabytes = memsize / (1024*1024);
  BX_MEM_THIS memory_handlers = new struct memory_handler_struct *[4096];
  BX_MEM_THIS rom = &BX_MEM_THIS vector[memsize];
  BX_MEM_THIS bogus = &BX_MEM_THIS vector[memsize + BIOSROMSZ + EXROMSIZE];
#if BX_DEBUGGER
  unsigned pages = get_num_allocated_pages();
  BX_MEM_THIS dbg_dirty_pages = new Bit8u[pages];
  memset(BX_MEM_THIS dbg_dirty_pages, 0, pages);
#endif
  memset(BX_MEM_THIS rom, 0xff, BIOSROMSZ + EXROMSIZE);
  memset(BX_MEM_THIS bogus, 0xff, 4096);
  for (idx = 0; idx < 4096; idx++)
    BX_MEM_THIS memory_handlers[idx] = NULL;
  for (idx = 0; idx < 65; idx++)
    BX_MEM_THIS rom_present[idx] = 0;
  BX_MEM_THIS pci_enabled = SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get();
  BX_MEM_THIS smram_available = 0;
  BX_MEM_THIS smram_enable = 0;
  BX_MEM_THIS smram_restricted = 0;

  // accept only memory size which is multiply of 1M
  BX_ASSERT((BX_MEM_THIS len & 0xfffff) == 0);
  BX_INFO(("%.2fMB", (float)(BX_MEM_THIS megabytes)));

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_THIS monitor_active = new bx_bool[BX_SMP_PROCESSORS];
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    BX_MEM_THIS monitor_active[i] = 0;
  }
  BX_MEM_THIS n_monitors = 0;
#endif

  register_state();
}

void BX_MEM_C::register_state()
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "memory", "Memory State", 3);
  new bx_shadow_data_c(list, "ram", BX_MEM_THIS vector, BX_MEM_THIS len);
  BXRS_DEC_PARAM_FIELD(list, len, BX_MEM_THIS len);
#if BX_SUPPORT_MONITOR_MWAIT
  bx_list_c *monitors = new bx_list_c(list, "monitors", BX_SMP_PROCESSORS+1);
  BXRS_PARAM_BOOL(monitors, n_monitors, BX_MEM_THIS n_monitors);
  for (int i=0;i<BX_SMP_PROCESSORS;i++) {
    char param_name[15];
    sprintf(param_name, "cpu%d_monitor", i);
    new bx_shadow_bool_c(monitors, param_name, &BX_MEM_THIS monitor_active[i]);
  }
#endif
}

void BX_MEM_C::cleanup_memory()
{
  unsigned idx;

  if (BX_MEM_THIS vector != NULL) {
    delete [] BX_MEM_THIS actual_vector;
    BX_MEM_THIS actual_vector = NULL;
    BX_MEM_THIS vector = NULL;
    if (BX_MEM_THIS memory_handlers != NULL) {
      for (idx = 0; idx < 4096; idx++) {
        struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[idx];
        struct memory_handler_struct *prev = NULL;
        while (memory_handler) {
          prev = memory_handler;
          memory_handler = memory_handler->next;
          delete prev;
        }
      }
      delete [] BX_MEM_THIS memory_handlers;
      BX_MEM_THIS memory_handlers = NULL;
    }
#if BX_DEBUGGER
    delete [] BX_MEM_THIS dbg_dirty_pages;
#endif
  }
}

//
// Values for type:
//   0 : System Bios
//   1 : VGA Bios
//   2 : Optional ROM Bios
//
void BX_MEM_C::load_ROM(const char *path, bx_phy_address romaddress, Bit8u type)
{
  struct stat stat_buf;
  int fd, ret, i, start_idx, end_idx;
  unsigned long size, max_size, offset;
  bx_bool is_bochs_bios = 0;

  if (*path == '\0') {
    if (type == 2) {
      BX_PANIC(( "ROM: Optional ROM image undefined"));
    }
    else if (type == 1) {
      BX_PANIC(( "ROM: VGA BIOS image undefined"));
    }
    else {
      BX_PANIC(( "ROM: System BIOS image undefined"));
    }
    return;
  }
  // read in ROM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    if (type < 2) {
      BX_PANIC(( "ROM: couldn't open ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(( "ROM: couldn't open ROM image file '%s'.", path));
    }
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    if (type < 2) {
      BX_PANIC(( "ROM: couldn't stat ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(( "ROM: couldn't stat ROM image file '%s'.", path));
    }
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  if (type > 0) {
    max_size = 0x10000;
  } else {
    max_size = BIOSROMSZ;
  }
  if (size > max_size) {
    close(fd);
    BX_PANIC(("ROM: ROM image too large"));
    return;
  }
  if (type == 0) {
    if (romaddress > 0) {
      if ((romaddress + size) != 0x100000 && (romaddress + size)) {
        close(fd);
        BX_PANIC(("ROM: System BIOS must end at 0xfffff"));
        return;
      }
    } else {
      romaddress = (bx_phy_address)-size;
    }
    offset = romaddress & BIOS_MASK;
    if ((romaddress & 0xf0000) < 0xf0000) {
      BX_MEM_THIS rom_present[64] = 1;
    }
    is_bochs_bios = (strstr(path, "BIOS-bochs-latest") != NULL);
  } else {
    if ((size % 512) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image size must be multiple of 512 (size = %ld)", size));
      return;
    }
    if ((romaddress % 2048) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image must start at a 2k boundary"));
      return;
    }
    if ((romaddress < 0xc0000) ||
        (((romaddress + size - 1) > 0xdffff) && (romaddress < 0xe0000))) {
      close(fd);
      BX_PANIC(("ROM: ROM address space out of range"));
      return;
    }
    if (romaddress < 0xe0000) {
      offset = (romaddress & EXROM_MASK) + BIOSROMSZ;
      start_idx = ((romaddress - 0xc0000) >> 11);
      end_idx = start_idx + (size >> 11) + (((size % 2048) > 0) ? 1 : 0);
    } else {
      offset = romaddress & BIOS_MASK;
      start_idx = 64;
      end_idx = 64;
    }
    for (i = start_idx; i < end_idx; i++) {
      if (BX_MEM_THIS rom_present[i]) {
        close(fd);
        BX_PANIC(("ROM: address space 0x%x already in use", (i * 2048) + 0xc0000));
        return;
      } else {
        BX_MEM_THIS rom_present[i] = 1;
      }
    }
  }
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS rom[offset], size);
    if (ret <= 0) {
      BX_PANIC(( "ROM: read failed on BIOS image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  offset -= (unsigned long)stat_buf.st_size;
  if (((romaddress & 0xfffff) != 0xe0000) ||
      ((BX_MEM_THIS rom[offset] == 0x55) && (BX_MEM_THIS rom[offset+1] == 0xaa))) {
    Bit8u checksum = 0;
    for (i = 0; i < stat_buf.st_size; i++) {
      checksum += BX_MEM_THIS rom[offset + i];
    }
    if (checksum != 0) {
      if (type == 1) {
        BX_PANIC(( "ROM: checksum error in VGABIOS image: '%s'", path));
      } else if (is_bochs_bios) {
        BX_ERROR(( "ROM: checksum error in BIOS image: '%s'", path));
      }
    }
  }
  BX_INFO(("rom at 0x%05x/%u ('%s')",
			(unsigned) romaddress,
			(unsigned) stat_buf.st_size,
 			path));
}

void BX_MEM_C::load_RAM(const char *path, bx_phy_address ramaddress, Bit8u type)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, offset;

  if (*path == '\0') {
    BX_PANIC(( "RAM: Optional RAM image undefined"));
    return;
  }
  // read in RAM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_PANIC(( "RAM: couldn't open RAM image file '%s'.", path));
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    BX_PANIC(( "RAM: couldn't stat RAM image file '%s'.", path));
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  offset = ramaddress;
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS vector[offset], size);
    if (ret <= 0) {
      BX_PANIC(( "RAM: read failed on RAM image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  BX_INFO(("ram at 0x%05x/%u ('%s')",
			(unsigned) ramaddress,
			(unsigned) stat_buf.st_size,
 			path));
}
#endif // #if BX_PROVIDE_CPU_MEMORY


#if ( BX_DEBUGGER || BX_DISASM || BX_GDBSTUB)
bx_bool BX_MEM_C::dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_bool ret = 1;

  for (; len>0; len--) {
    // Reading standard PCI/ISA Video Mem / SMMRAM
    if ((addr & 0xfffe0000) == 0x000a0000) {
      if (BX_MEM_THIS smram_enable || cpu->smm_mode())
        *buf = BX_MEM_THIS vector[addr];
      else 
        *buf = DEV_vga_mem_read(addr);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && ((addr & 0xfffc0000) == 0x000c0000))
    {
      switch (DEV_pci_rd_memtype (addr)) {
        case 0x0:  // Read from ROM
          if ((addr & 0xfffe0000) == 0x000e0000)
          {
            *buf = BX_MEM_THIS rom[addr & BIOS_MASK];
          }
          else
          {
            *buf = BX_MEM_THIS rom[(addr & EXROM_MASK) + BIOSROMSZ];
          }
          break;
        case 0x1:  // Read from ShadowRAM
          *buf = BX_MEM_THIS vector[addr];
          break;
        default:
          BX_PANIC(("dbg_fetch_mem: default case"));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if (addr < BX_MEM_THIS len)
    {
      if ((addr & 0xfffc0000) != 0x000c0000) {
        *buf = BX_MEM_THIS vector[addr];
      }
      else if ((addr & 0xfffe0000) == 0x000e0000)
      {
        *buf = BX_MEM_THIS rom[addr & BIOS_MASK];
      }
      else
      {
        *buf = BX_MEM_THIS rom[(addr & EXROM_MASK) + BIOSROMSZ];
      }
    }
    else if (addr >= (bx_phy_address)~BIOS_MASK)
    {
      *buf = BX_MEM_THIS rom[addr & BIOS_MASK];
    }
    else
    {
      *buf = 0xff;
      ret = 0; // error, beyond limits of memory
    }
    buf++;
    addr++;
  }
  return ret;
}
#endif

#if BX_DEBUGGER || BX_GDBSTUB
bx_bool BX_MEM_C::dbg_set_mem(bx_phy_address addr, unsigned len, Bit8u *buf)
{
  if ((addr + len - 1) > BX_MEM_THIS len) {
    return(0); // error, beyond limits of memory
  }
  for (; len>0; len--) {
    // Write to standard PCI/ISA Video Mem / SMMRAM
    if ((addr & 0xfffe0000) == 0x000a0000) {
      if (BX_MEM_THIS smram_enable)
        BX_MEM_THIS vector[addr] = *buf;
      else 
        DEV_vga_mem_write(addr, *buf);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && ((addr & 0xfffc0000) == 0x000c0000))
    {
      switch (DEV_pci_wr_memtype (addr)) {
        case 0x0:  // Ignore write to ROM
          break;
        case 0x1:  // Write to ShadowRAM
          BX_MEM_THIS vector[addr] = *buf;
          break;
        default:
          BX_PANIC(("dbg_fetch_mem: default case"));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ((addr & 0xfffc0000) != 0x000c0000 && (addr < (bx_phy_address)(~BIOS_MASK)))
    {
      BX_MEM_THIS vector[addr] = *buf;
    }
    buf++;
    addr++;
  }
  return(1);
}
#endif

bx_bool BX_MEM_C::dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc)
{
  *crc = 0;
  if (addr1 > addr2)
    return(0);

  if (addr2 >= BX_MEM_THIS len)
    return(0); // error, specified address past last phy mem addr
  
  unsigned len = 1 + addr2 - addr1;
  *crc = crc32(BX_MEM_THIS vector + addr1, len);

  return(1);
}

//
// Return a host address corresponding to the guest physical memory
// address (with A20 already applied), given that the calling
// code will perform an 'op' operation.  This address will be
// used for direct access to guest memory as an acceleration by
// a few instructions, like REP {MOV, INS, OUTS, etc}.
// Values of 'op' are { BX_READ, BX_WRITE, BX_RW }.
//
// The other assumption is that the calling code _only_ accesses memory
// directly within the page that encompasses the address requested.
//

//
// Memory map inside the 1st megabyte:
//
// 0x00000 - 0x7ffff    DOS area (512K)
// 0x80000 - 0x9ffff    Optional fixed memory hole (128K)
// 0xa0000 - 0xbffff    Standard PCI/ISA Video Mem / SMMRAM (128K)
// 0xc0000 - 0xdffff    Expansion Card BIOS and Buffer Area (128K)
// 0xe0000 - 0xeffff    Lower BIOS Area (64K)
// 0xf0000 - 0xfffff    Upper BIOS Area (64K)
//

Bit8u *BX_MEM_C::getHostMemAddr(BX_CPU_C *cpu, bx_phy_address a20Addr, unsigned op, unsigned access_type)
{
  BX_ASSERT(cpu != 0); // getHostMemAddr could be used only inside the CPU

#if BX_SUPPORT_APIC
  bx_generic_apic_c *local_apic = &cpu->local_apic;
  if (local_apic->get_base() == (a20Addr & ~0xfff))
    return(NULL); // Vetoed!  APIC address space
#endif

  // allow direct access to SMRAM memory space for code and veto data
  if (access_type == CODE_ACCESS) {
    // reading from SMRAM memory space
    if ((a20Addr & 0xfffe0000) == 0x000a0000 && (BX_MEM_THIS smram_available))
    {
      if (BX_MEM_THIS smram_enable || cpu->smm_mode())
        return (Bit8u *) &BX_MEM_THIS vector[a20Addr];
    }
  }

#if BX_SUPPORT_MONITOR_MWAIT
  if (BX_MEM_THIS is_monitor(a20Addr & ~0xfff, 0x1000)) {
     // Vetoed! Write monitored page !
     if (op != BX_READ) return(NULL);
  }
#endif

  struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[a20Addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20Addr &&
        memory_handler->end >= a20Addr) {
      return(NULL); // Vetoed! memory handler for i/o apic, vram, mmio and PCI PnP
    }
    memory_handler = memory_handler->next;
  }

  if (op == BX_READ) {
    if ((a20Addr & 0xfffe0000) == 0x000a0000)
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && ((a20Addr & 0xfffc0000) == 0x000c0000))
    {
      switch (DEV_pci_rd_memtype (a20Addr)) {
        case 0x0:   // Read from ROM
          if ((a20Addr & 0xfffe0000) == 0x000e0000)
          {
            return (Bit8u *) &BX_MEM_THIS rom[a20Addr & BIOS_MASK];
          }
          else
          {
            return (Bit8u *) &BX_MEM_THIS rom[(a20Addr & EXROM_MASK) + BIOSROMSZ];
          }
          break;
        case 0x1:   // Read from ShadowRAM
          return (Bit8u *) &BX_MEM_THIS vector[a20Addr];
        default:
          BX_PANIC(("getHostMemAddr(): default case"));
          return(NULL);
      }
    }
#endif
    else if(a20Addr < BX_MEM_THIS len)
    {
      if ((a20Addr & 0xfffc0000) != 0x000c0000) {
        return (Bit8u *) &BX_MEM_THIS vector[a20Addr];
      }
      else if ((a20Addr & 0xfffe0000) == 0x000e0000)
      {
        return (Bit8u *) &BX_MEM_THIS rom[a20Addr & BIOS_MASK];
      }
      else
      {
        return( (Bit8u *) &BX_MEM_THIS rom[(a20Addr & EXROM_MASK) + BIOSROMSZ]);
      }
    }
    else if (a20Addr >= (bx_phy_address)~BIOS_MASK)
    {
      return (Bit8u *) &BX_MEM_THIS rom[a20Addr & BIOS_MASK];
    }
    else
    {
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20Addr & 0x0fff];
    }
  }
  else
  { // op == {BX_WRITE, BX_RW}
    Bit8u *retAddr;
    if (a20Addr >= BX_MEM_THIS len)
      return(NULL); // Error, requested addr is out of bounds.
    else if ((a20Addr & 0xfffe0000) == 0x000a0000)
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
    else if (a20Addr >= (bx_phy_address)~BIOS_MASK)
      return(NULL); // Vetoed!  ROMs
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && ((a20Addr & 0xfffc0000) == 0x000c0000))
    {
      // Veto direct writes to this area. Otherwise, there is a chance
      // for Guest2HostTLB and memory consistency problems, for example
      // when some 16K block marked as write-only using PAM registers.
      return(NULL);
    }
#endif
    else
    {
      if ((a20Addr & 0xfffc0000) != 0x000c0000) {
        retAddr = (Bit8u *) &BX_MEM_THIS vector[a20Addr];
      }
      else
      {
        return(NULL);  // Vetoed!  ROMs
      }
    }

#if BX_SUPPORT_ICACHE
    pageWriteStampTable.decWriteStamp(a20Addr);
#endif

    return retAddr;
  }
}

/*
 * One needs to provide both a read_handler and a write_handler.
 * XXX: maybe we should check for overlapping memory handlers
 */
  bx_bool 
BX_MEM_C::registerMemoryHandlers(void *param, memory_handler_t read_handler,
		memory_handler_t write_handler, bx_phy_address begin_addr, bx_phy_address end_addr)
{
  if (end_addr < begin_addr)
    return 0;
  if (!read_handler || !write_handler)
    return 0;
  BX_INFO(("Register memory access handlers: %08x-%08x", begin_addr, end_addr));
  for (unsigned page_idx = begin_addr >> 20; page_idx <= end_addr >> 20; page_idx++) {
    struct memory_handler_struct *memory_handler = new struct memory_handler_struct;
    memory_handler->next = BX_MEM_THIS memory_handlers[page_idx];
    BX_MEM_THIS memory_handlers[page_idx] = memory_handler;
    memory_handler->read_handler = read_handler;
    memory_handler->write_handler = write_handler;
    memory_handler->param = param;
    memory_handler->begin = begin_addr;
    memory_handler->end = end_addr;
  }
  return 1;
}

  bx_bool 
BX_MEM_C::unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler,
		bx_phy_address begin_addr, bx_phy_address end_addr)
{
  bx_bool ret = 1;
  BX_INFO(("Memory access handlers unregistered: %08x-%08x", begin_addr, end_addr));
  for (unsigned page_idx = begin_addr >> 20; page_idx <= end_addr >> 20; page_idx++) {
    struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[page_idx];
    struct memory_handler_struct *prev = NULL;
    while (memory_handler && 
         memory_handler->read_handler != read_handler &&
         memory_handler->write_handler != write_handler && 
         memory_handler->begin != begin_addr && 
         memory_handler->end != end_addr)
    {
      prev = memory_handler;
      memory_handler = memory_handler->next;
    }
    if (!memory_handler) {
      ret = 0;  // we should have found it
      continue; // anyway, try the other pages
    }
    if (prev)
      prev->next = memory_handler->next;
    else
      BX_MEM_THIS memory_handlers[page_idx] = memory_handler->next;
    delete memory_handler;
  }  
  return ret;
}

void BX_MEM_C::enable_smram(bx_bool enable, bx_bool restricted)
{
  BX_MEM_THIS smram_available = 1;
  BX_MEM_THIS smram_enable = (enable > 0);
  BX_MEM_THIS smram_restricted = (restricted > 0);
}

void BX_MEM_C::disable_smram(void)
{
  BX_MEM_THIS smram_available  = 0;
  BX_MEM_THIS smram_enable     = 0;
  BX_MEM_THIS smram_restricted = 0;
}

// check if SMRAM is aavailable for CPU data accesses
bx_bool BX_MEM_C::is_smram_accessible(void)
{
  return(BX_MEM_THIS smram_available) &&
        (BX_MEM_THIS smram_enable || !BX_MEM_THIS smram_restricted);
}

#if BX_SUPPORT_MONITOR_MWAIT

//
// MONITOR/MWAIT - x86arch way to optimize idle loops in CPU
//

void BX_MEM_C::set_monitor(unsigned cpu)
{
  BX_ASSERT(cpu < BX_SMP_PROCESSORS);
  if (! BX_MEM_THIS monitor_active[cpu]) {
    BX_MEM_THIS monitor_active[cpu] = 1;
    BX_MEM_THIS n_monitors++;
    BX_DEBUG(("activate monitor for cpu=%d", cpu));
  }
  else {
    BX_DEBUG(("monitor for cpu=%d already active !", cpu));
  }
}

void BX_MEM_C::clear_monitor(unsigned cpu)
{
  BX_ASSERT(cpu < BX_SMP_PROCESSORS);
  BX_MEM_THIS monitor_active[cpu] = 0;
  BX_MEM_THIS n_monitors--;
  BX_DEBUG(("deactivate monitor for cpu=%d", cpu));
}

bx_bool BX_MEM_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (BX_MEM_THIS n_monitors == 0) return 0;
  
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    if (BX_MEM_THIS monitor_active[i]) {
      if (BX_CPU(i)->is_monitor(begin_addr, len))
        return 1;
    }
  }

  return 0; // // this is NOT monitored page
}

void BX_MEM_C::check_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (BX_MEM_THIS n_monitors == 0) return;

  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    if (BX_MEM_THIS monitor_active[i]) {
      BX_CPU(i)->check_monitor(begin_addr, len);
    }
  }
}

#endif
