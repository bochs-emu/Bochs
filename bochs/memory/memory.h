/////////////////////////////////////////////////////////////////////////
// $Id: memory.h,v 1.68 2010-03-07 09:16:24 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

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

#define BIOSROMSZ ((Bit32u)(1 << 19))  // 512KB BIOS ROM @0xfff80000, must be a power of 2
#define EXROMSIZE  (0x20000)           // ROMs 0xc0000-0xdffff (area 0xe0000-0xfffff=bios mapped)
#define BIOS_MASK (BIOSROMSZ-1)
#define EXROM_MASK (EXROMSIZE-1)

typedef bx_bool (*memory_handler_t)(bx_phy_address addr, unsigned len, void *data, void *param);

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

  Bit64u  len, allocated;  // could be > 4G
  Bit8u   *actual_vector;
  Bit8u   *vector;   // aligned correctly
  Bit8u  **blocks;
  Bit8u   *rom;      // 512k BIOS rom space + 128k expansion rom space
  Bit8u   *bogus;    // 4k for unexisting memory
  unsigned used_blocks;

public:
  BX_MEM_C();
 ~BX_MEM_C();

  BX_MEM_SMF Bit8u*  get_vector(bx_phy_address addr);
  BX_MEM_SMF void    init_memory(Bit64u guest, Bit64u host);
  BX_MEM_SMF void    cleanup_memory(void);
  BX_MEM_SMF void    enable_smram(bx_bool enable, bx_bool restricted);
  BX_MEM_SMF void    disable_smram(void);
  BX_MEM_SMF bx_bool is_smram_accessible(void);
  BX_MEM_SMF void    readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                      unsigned len, void *data);
  BX_MEM_SMF void    writePhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                       unsigned len, void *data);
  BX_MEM_SMF void    load_ROM(const char *path, bx_phy_address romaddress, Bit8u type);
  BX_MEM_SMF void    load_RAM(const char *path, bx_phy_address romaddress, Bit8u type);
#if (BX_DEBUGGER || BX_DISASM || BX_GDBSTUB)
  BX_MEM_SMF bx_bool dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf);
#endif
#if (BX_DEBUGGER || BX_GDBSTUB)
  BX_MEM_SMF bx_bool dbg_set_mem(bx_phy_address addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bx_bool dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc);
#endif
  BX_MEM_SMF Bit8u* getHostMemAddr(BX_CPU_C *cpu, bx_phy_address addr, unsigned rw);
  BX_MEM_SMF bx_bool registerMemoryHandlers(void *param, memory_handler_t read_handler,
		  memory_handler_t write_handler, bx_phy_address begin_addr, bx_phy_address end_addr);
  BX_MEM_SMF bx_bool unregisterMemoryHandlers(memory_handler_t read_handler, memory_handler_t write_handler,
		  bx_phy_address begin_addr, bx_phy_address end_addr);
  BX_MEM_SMF Bit64u  get_memory_len(void);
  BX_MEM_SMF void allocate_block(Bit32u index);
  BX_MEM_SMF Bit8u* alloc_vector_aligned(Bit32u bytes, Bit32u alignment);

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_SMF bx_bool is_monitor(bx_phy_address begin_addr, unsigned len);
  BX_MEM_SMF void    check_monitor(bx_phy_address addr, unsigned len);
#endif

  void register_state(void);

  friend Bit64s memory_param_save_handler(void *devptr, bx_param_c *param);
  friend void memory_param_restore_handler(void *devptr, bx_param_c *param, Bit64s val);
};

BOCHSAPI extern BX_MEM_C bx_mem;

// must be power of two
#define BX_MEM_BLOCK_LEN (1024*1024) /* 1M blocks */

/*
BX_CPP_INLINE Bit8u* BX_MEM_C::get_vector(bx_phy_address addr)
{
  return (BX_MEM_THIS vector + addr);
}
*/

BX_CPP_INLINE Bit8u* BX_MEM_C::get_vector(bx_phy_address addr)
{
  Bit32u block = addr / BX_MEM_BLOCK_LEN;
  if (! BX_MEM_THIS blocks[block]) allocate_block(block);
  return BX_MEM_THIS blocks[block] + (Bit32u)(addr & (BX_MEM_BLOCK_LEN-1));
}

BX_CPP_INLINE Bit64u BX_MEM_C::get_memory_len(void)
{
  return (BX_MEM_THIS len);
}

#endif
