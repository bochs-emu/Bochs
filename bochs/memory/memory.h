/////////////////////////////////////////////////////////////////////////
// $Id: memory.h,v 1.40 2006-09-15 17:02:52 vruppert Exp $
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
	bx_phy_address begin;
	bx_phy_address end;
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
  bx_bool smram_available;
  bx_bool smram_enable;
  bx_bool smram_restricted;
  
public:
  Bit8u   *actual_vector;
  Bit8u   *vector;   // aligned correctly
  size_t  len;
  size_t  megabytes; // (len in Megabytes)
  Bit8u   *rom;      // 512k BIOS rom space + 128k expansion rom space
  Bit8u   *bogus;    // 4k for unexisting memory
#if BX_DEBUGGER
  Bit8u   *dbg_dirty_pages;
#endif

  BX_MEM_C();
 ~BX_MEM_C();
  BX_MEM_SMF void    alloc_vector_aligned (size_t bytes, size_t alignment) BX_CPP_AttrRegparmN(2);
  BX_MEM_SMF void    init_memory(int memsize);
  BX_MEM_SMF void    cleanup_memory(void);
  BX_MEM_SMF void    enable_smram(bx_bool enable, bx_bool restricted);
  BX_MEM_SMF void    disable_smram(void);
  BX_MEM_SMF bx_bool is_smram_accessible(void);
  BX_MEM_SMF void    readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                      unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    writePhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                       unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    load_ROM(const char *path, bx_phy_address romaddress, Bit8u type);
  BX_MEM_SMF void    load_RAM(const char *path, bx_phy_address romaddress, Bit8u type);
  BX_MEM_SMF Bit32u  get_memory_in_k(void);
  BX_MEM_SMF bx_bool dbg_fetch_mem(bx_phy_address addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_set_mem(bx_phy_address addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc);
  BX_MEM_SMF Bit8u* getHostMemAddr(BX_CPU_C *cpu, bx_phy_address a20Addr, unsigned op, unsigned access_type);
  BX_MEM_SMF bx_bool registerMemoryHandlers(void *param, memory_handler_t read_handler,
		  memory_handler_t write_handler, bx_phy_address begin_addr, bx_phy_address end_addr);
  BX_MEM_SMF bx_bool unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler,
		  bx_phy_address begin_addr, bx_phy_address end_addr);
  BX_MEM_SMF Bit32u get_num_allocated_pages(void);
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
