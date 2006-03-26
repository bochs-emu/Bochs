/////////////////////////////////////////////////////////////////////////
// $Id: memory.h,v 1.34 2006-03-26 19:39:37 sshwarts Exp $
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

#ifndef BX_MEM_H
#  define BX_MEM_H 1

#define BX_USE_MEM_SMF 0

#if BX_USE_MEM_SMF
// if static member functions on, then there is only one memory
#  define BX_MEM_SMF  static
#  define BX_MEM_THIS BX_MEM(0)->
#else
#  define BX_MEM_SMF
#  define BX_MEM_THIS this->
#endif

class BX_CPU_C;

// alignment of memory vector, must be a power of 2
#define BX_MEM_VECTOR_ALIGN 4096
#define BIOSROMSZ (1 << 19)  // 512KB BIOS ROM @0xfff80000, must be a power of 2
#define EXROMSIZE 0x20000    // ROMs 0xc0000-0xdffff (area 0xe0000-0xfffff=bios mapped)
#define BIOS_MASK (BIOSROMSZ-1)
#define EXROM_MASK (EXROMSIZE-1)

typedef bx_bool (*memory_handler_t)(unsigned long addr, unsigned long len, void *data, void *param);

struct memory_handler_struct {
	struct memory_handler_struct *next;
	void *param;
	unsigned long begin;
	unsigned long end;
	memory_handler_t read_handler;
	memory_handler_t write_handler;
};

#define SMRAM_CODE  1
#define SMRAM_DATA  2

class BOCHSAPI BX_MEM_C : public logfunctions {
private:
  struct memory_handler_struct **memory_handlers;
  bx_bool rom_present[65];
  bx_bool pci_enabled;
  unsigned smram_enabled;
  
public:
  Bit8u   *actual_vector;
  Bit8u   *vector;   // aligned correctly
  size_t  len;
  size_t  megabytes; // (len in Megabytes)
  Bit8u   *rom;      // 512k BIOS rom space + 128k expansion rom space
  Bit8u   *bogus;    // 4k for unexisting memory
#if BX_DEBUGGER
  unsigned char dbg_dirty_pages[(BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096];
  Bit32u dbg_count_dirty_pages () {
    return (BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096;
  }
#endif

  BX_MEM_C();
 ~BX_MEM_C();
  BX_MEM_SMF void    alloc_vector_aligned (size_t bytes, size_t alignment) BX_CPP_AttrRegparmN(2);
  BX_MEM_SMF void    init_memory(int memsize);
  BX_MEM_SMF void    enable_smram(bx_bool code_only);
  BX_MEM_SMF void    disable_smram(void);
  BX_MEM_SMF void    readPhysicalPage(BX_CPU_C *cpu, Bit32u addr,
                                      unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    writePhysicalPage(BX_CPU_C *cpu, Bit32u addr,
                                       unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    load_ROM(const char *path, Bit32u romaddress, Bit8u type);
  BX_MEM_SMF void    load_RAM(const char *path, Bit32u romaddress, Bit8u type);
  BX_MEM_SMF Bit32u  get_memory_in_k(void);
  BX_MEM_SMF bx_bool dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_set_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_crc32(Bit32u addr1, Bit32u addr2, Bit32u *crc);
  BX_MEM_SMF Bit8u* getHostMemAddr(BX_CPU_C *cpu, Bit32u a20Addr, unsigned op, unsigned access_type);
  BX_MEM_SMF bx_bool registerMemoryHandlers(void *param, memory_handler_t read_handler,
		  memory_handler_t write_handler, Bit32u begin_addr, Bit32u end_addr);
  BX_MEM_SMF bx_bool unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler, 
		  Bit32u begin_addr, Bit32u end_addr);
  };

#if BX_PROVIDE_CPU_MEMORY==1

#if BX_ADDRESS_SPACES==1
BOCHSAPI extern BX_MEM_C bx_mem;
#else
BOCHSAPI extern BX_MEM_C bx_mem_array[BX_ADDRESS_SPACES];
#endif  /* BX_ADDRESS_SPACES */

#endif  /* BX_PROVIDE_CPU_MEMORY==1 */

#if BX_DEBUGGER
#  define BX_DBG_DIRTY_PAGE(page) BX_MEM(0)->dbg_dirty_pages[page] = 1;
#else
#  define BX_DBG_DIRTY_PAGE(page)
#endif

#endif
