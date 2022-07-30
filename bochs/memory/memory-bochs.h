/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2021  The Bochs Project
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
                                       // 512K BIOS ROM @0xfff80000
const Bit32u BIOSROMSZ = (1 << 21);    //   2M BIOS ROM @0xffe00000, must be a power of 2
const Bit32u EXROMSIZE = (0x20000);    // ROMs 0xc0000-0xdffff (area 0xe0000-0xfffff=bios mapped)

const Bit32u BIOS_MASK  = BIOSROMSZ-1;
const Bit32u EXROM_MASK = EXROMSIZE-1;

#define BIOS_MAP_LAST128K(addr) (((addr) | 0xfff00000) & BIOS_MASK)

#define BIOS_ROM_LOWER    0x01
#define BIOS_ROM_EXTENDED 0x02
#define BIOS_ROM_1MEG     0x04

enum memory_area_t {
  BX_MEM_AREA_C0000 = 0,
  BX_MEM_AREA_C4000,
  BX_MEM_AREA_C8000,
  BX_MEM_AREA_CC000,
  BX_MEM_AREA_D0000,
  BX_MEM_AREA_D4000,
  BX_MEM_AREA_D8000,
  BX_MEM_AREA_DC000,
  BX_MEM_AREA_E0000,
  BX_MEM_AREA_E4000,
  BX_MEM_AREA_E8000,
  BX_MEM_AREA_EC000,
  BX_MEM_AREA_F0000
};

typedef bool (*memory_handler_t)(bx_phy_address addr, unsigned len, void *data, void *param);
// return a pointer to 4K region containing <addr> or NULL if direct access is not allowed
// same format as getHostMemAddr method
typedef Bit8u* (*memory_direct_access_handler_t)(bx_phy_address addr, unsigned rw, void *param);

struct memory_handler_struct {
  struct memory_handler_struct *next;
  void *param;
  bx_phy_address begin;
  bx_phy_address end;
  Bit16u bitmap;
  memory_handler_t read_handler;
  memory_handler_t write_handler;
  memory_direct_access_handler_t da_handler;
};

#define SMRAM_CODE  1
#define SMRAM_DATA  2

class BOCHSAPI BX_MEM_C : public logfunctions {
private:
  struct memory_handler_struct **memory_handlers;
  bool pci_enabled;
  bool bios_write_enabled;
  bool smram_available;
  bool smram_enable;
  bool smram_restricted;

  Bit64u  len, allocated;  // could be > 4G
  Bit32u  block_size;      // individual block size, must be power of 2
  Bit8u   *actual_vector;
  Bit8u   *vector;   // aligned correctly
  Bit8u  **blocks;
  Bit8u   *rom;      // 512k BIOS rom space + 128k expansion rom space
  Bit8u   *bogus;    // 4k for unexisting memory
  bool    rom_present[65];
  bool    memory_type[13][2];
  Bit32u  bios_rom_addr;
  Bit8u   bios_rom_access;
  Bit8u   flash_type;
  Bit8u   flash_status;
  Bit8u   flash_wsm_state;

  Bit32u used_blocks;
#if BX_LARGE_RAMFILE
  static Bit8u * const swapped_out; // NULL; // (NULL - sizeof(Bit8u));
  Bit32u  next_swapout_idx;
  FILE    *overflow_file;

  BX_MEM_SMF void   read_block(Bit32u block);
#endif
  BX_MEM_SMF Bit8u flash_read(Bit32u addr);
  BX_MEM_SMF void  flash_write(Bit32u addr, Bit8u data);

public:
  BX_MEM_C();
 ~BX_MEM_C();

  BX_MEM_SMF void    init_memory(Bit64u guest, Bit64u host, Bit32u block_size);
  BX_MEM_SMF void    cleanup_memory(void);
  BX_MEM_SMF Bit8u*  get_vector(bx_phy_address addr);

  BX_MEM_SMF void    enable_smram(bool enable, bool restricted);
  BX_MEM_SMF void    disable_smram(void);
  BX_MEM_SMF bool is_smram_accessible(void);

  BX_MEM_SMF void    set_bios_write(bool enabled);
  BX_MEM_SMF void    set_bios_rom_access(Bit8u region, bool enabled);
  BX_MEM_SMF void    set_memory_type(memory_area_t area, bool rw, bool dram);

  BX_MEM_SMF Bit8u*  getHostMemAddr(BX_CPU_C *cpu, bx_phy_address addr, unsigned rw);

  // Note: accesses should always be contained within a single page
  BX_MEM_SMF void    readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                      unsigned len, void *data);
  BX_MEM_SMF void    writePhysicalPage(BX_CPU_C *cpu, bx_phy_address addr,
                                       unsigned len, void *data);

  BX_MEM_SMF void    dmaReadPhysicalPage(bx_phy_address addr, unsigned len, Bit8u *data);
  BX_MEM_SMF void    dmaWritePhysicalPage(bx_phy_address addr, unsigned len, Bit8u *data);

  BX_MEM_SMF void    load_ROM(const char *path, bx_phy_address romaddress, Bit8u type);
  BX_MEM_SMF void    load_RAM(const char *path, bx_phy_address romaddress);

  BX_MEM_SMF bool dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf);
#if (BX_DEBUGGER || BX_GDBSTUB)
  BX_MEM_SMF bool dbg_set_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf);
  BX_MEM_SMF bool dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc);
#endif

  BX_MEM_SMF bool registerMemoryHandlers(void *param, memory_handler_t read_handler,
                  memory_handler_t write_handler, memory_direct_access_handler_t da_handler,
                  bx_phy_address begin_addr, bx_phy_address end_addr);
  BX_MEM_SMF BX_CPP_INLINE bool registerMemoryHandlers(void *param, memory_handler_t read_handler,
                  memory_handler_t write_handler,
                  bx_phy_address begin_addr, bx_phy_address end_addr)
  {
     return registerMemoryHandlers(param, read_handler, write_handler, NULL, begin_addr, end_addr);
  }
  BX_MEM_SMF bool unregisterMemoryHandlers(void *param, bx_phy_address begin_addr, bx_phy_address end_addr);

  BX_MEM_SMF Bit64u  get_memory_len(void);
  BX_MEM_SMF void allocate_block(Bit32u index);
  BX_MEM_SMF Bit8u* alloc_vector_aligned(Bit64u bytes, Bit64u alignment);

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_SMF bool is_monitor(bx_phy_address begin_addr, unsigned len);
  BX_MEM_SMF void    check_monitor(bx_phy_address addr, unsigned len);
#endif

  void register_state(void);

  friend void ramfile_save_handler(void *devptr, FILE *fp);
  friend Bit64s memory_param_save_handler(void *devptr, bx_param_c *param);
  friend void memory_param_restore_handler(void *devptr, bx_param_c *param, Bit64s val);
};

BOCHSAPI extern BX_MEM_C bx_mem;

/*
BX_CPP_INLINE Bit8u* BX_MEM_C::get_vector(bx_phy_address addr)
{
  return (BX_MEM_THIS vector + addr);
}
*/

BX_CPP_INLINE Bit64u BX_MEM_C::get_memory_len(void)
{
  return (BX_MEM_THIS len);
}

#endif
