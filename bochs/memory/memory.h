/////////////////////////////////////////////////////////////////////////
// $Id: memory.h,v 1.19 2004-08-06 15:49:55 vruppert Exp $
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



#define BX_USE_MEM_SMF 0

#if BX_USE_MEM_SMF
// if static member functions on, then there is only one memory
#  define BX_MEM_SMF  static
#  define BX_MEM_THIS BX_MEM(0)->
#else
#  define BX_MEM_SMF
#  define BX_MEM_THIS this->
#endif

// alignment of memory vector, must be a power of 2
#define BX_MEM_VECTOR_ALIGN 4096

typedef bx_bool (*memory_handler_t)(unsigned long addr, unsigned long len, void *data, void *param);

struct memory_handler_struct {
	struct memory_handler_struct *next;
	unsigned long begin;
	unsigned long end;
	memory_handler_t read_handler;
	memory_handler_t write_handler;
	void *read_param;
	void *write_param;
};

class BOCHSAPI BX_MEM_C : public logfunctions {

private:
  struct memory_handler_struct **memory_handlers;
  
public:
  Bit8u   *actual_vector;
  Bit8u   *vector;  // aligned correctly
  size_t  len;
  size_t  megabytes;  // (len in Megabytes)
#if BX_SUPPORT_PCI
  Bit8u   shadow[4*16*4096]; // 256k of memory
#endif
#if BX_DEBUGGER
  unsigned char dbg_dirty_pages[(BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096];
  Bit32u dbg_count_dirty_pages () {
    return (BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096;
  }
#endif

  BX_MEM_C(void);
  //BX_MEM_C(size_t memsize);
  ~BX_MEM_C(void);
  BX_MEM_SMF void    alloc_vector_aligned (size_t bytes, size_t alignment) BX_CPP_AttrRegparmN(2);
  BX_MEM_SMF void    init_memory(int memsize);
  BX_MEM_SMF void    readPhysicalPage(BX_CPU_C *cpu, Bit32u addr,
                                      unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    writePhysicalPage(BX_CPU_C *cpu, Bit32u addr,
                                       unsigned len, void *data) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF void    load_ROM(const char *path, Bit32u romaddress, Bit8u type);
  BX_MEM_SMF Bit32u  get_memory_in_k(void);
#if BX_SUPPORT_PCI
  BX_MEM_SMF Bit8u*  pci_fetch_ptr(Bit32u addr) BX_CPP_AttrRegparmN(1);
#endif
  BX_MEM_SMF bx_bool dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_set_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_crc32(
    unsigned long (*f)(unsigned char *buf, int len),
    Bit32u addr1, Bit32u addr2, Bit32u *crc);
  BX_MEM_SMF Bit8u * getHostMemAddr(BX_CPU_C *cpu, Bit32u a20Addr, unsigned op) BX_CPP_AttrRegparmN(3);
  BX_MEM_SMF bx_bool registerMemoryHandlers(memory_handler_t read_handler, void *read_param, 
		  memory_handler_t write_handler, void *write_param, 
		  unsigned long begin_addr, unsigned long end_addr);
  BX_MEM_SMF bx_bool unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler, 
		  unsigned long begin_addr, unsigned long end_addr);
  };

#if BX_PROVIDE_CPU_MEMORY==1

#if BX_SMP_PROCESSORS==1
BOCHSAPI extern BX_MEM_C    bx_mem;
#else
BOCHSAPI extern BX_MEM_C    *bx_mem_array[BX_ADDRESS_SPACES];
#endif  /* BX_SMP_PROCESSORS */

#endif  /* BX_PROVIDE_CPU_MEMORY==1 */

#if BX_DEBUGGER
#  define BX_DBG_DIRTY_PAGE(page) BX_MEM(0)->dbg_dirty_pages[page] = 1;
#else
#  define BX_DBG_DIRTY_PAGE(page)
#endif
