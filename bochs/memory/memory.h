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



#define BX_USE_MEM_SMF 1

#if BX_USE_MEM_SMF
#  define BX_MEM_SMF  static
#  define BX_MEM_THIS BX_MEM.
#else
#  define BX_MEM_SMF
#  define BX_MEM_THIS this->
#endif



class BX_MEM_C {

public:
  Bit8u   *vector;
  size_t  len;
  size_t  megabytes;  // (len in Megabytes)
#if BX_DEBUGGER
  unsigned char dbg_dirty_pages[(BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096];
#endif

  BX_MEM_C(void);
  BX_MEM_C(size_t memsize);
  ~BX_MEM_C(void);
  BX_MEM_SMF void    init_memory(int memsize);
  BX_MEM_SMF void    read_physical(Bit32u addr, unsigned len, void *data);
  BX_MEM_SMF void    write_physical(Bit32u addr, unsigned len, void *data);
  BX_MEM_SMF void    load_ROM(const char *path, Bit32u romaddress);
  BX_MEM_SMF Bit32u  get_memory_in_k(void);
  BX_MEM_SMF Boolean dbg_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF Boolean dbg_set_mem(Bit32u addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF Boolean dbg_crc32(
    unsigned long (*f)(unsigned char *buf, int len),
    Bit32u addr1, Bit32u addr2, Bit32u *crc);
  };

#if BX_PROVIDE_CPU_MEMORY==1
extern BX_MEM_C    BX_MEM;
#endif

#if BX_DEBUGGER
#  define BX_DBG_DIRTY_PAGE(page) BX_MEM.dbg_dirty_pages[page] = 1;
#else
#  define BX_DBG_DIRTY_PAGE(page)
#endif
