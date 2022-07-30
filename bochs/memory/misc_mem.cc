/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2022  The Bochs Project
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

#include "bochs.h"
#include "param_names.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM(0)->

// block size must be power of two
BX_CPP_INLINE bool is_power_of_2(Bit64u x)
{
    return (x & (x - 1)) == 0;
}

// alignment of memory vector, must be a power of 2
#define BX_MEM_VECTOR_ALIGN 4096
#define BX_MEM_HANDLERS   ((BX_CONST64(1) << BX_PHY_ADDRESS_WIDTH) >> 20) /* one per megabyte */

#if BX_LARGE_RAMFILE
Bit8u* const BX_MEM_C::swapped_out = ((Bit8u*)NULL - sizeof(Bit8u));
#endif

#define FLASH_READ_ARRAY  0xff
#define FLASH_INT_ID      0x90
#define FLASH_READ_STATUS 0x70
#define FLASH_CLR_STATUS  0x50
#define FLASH_ERASE_SETUP 0x20
#define FLASH_ERASE_SUSP  0xb0
#define FLASH_PROG_SETUP  0x40
#define FLASH_ERASE       0xd0

BX_MEM_C::BX_MEM_C()
{
  put("memory", "MEM0");

  vector = NULL;
  actual_vector = NULL;
  blocks = NULL;
  len    = 0;
  used_blocks = 0;

  memory_handlers = NULL;

#if BX_LARGE_RAMFILE
  next_swapout_idx = 0;
  overflow_file = NULL;
#endif
}

Bit8u* BX_MEM_C::alloc_vector_aligned(Bit64u bytes, Bit64u alignment)
{
  Bit64u test_mask = alignment - 1;
  BX_MEM_THIS actual_vector = new Bit8u [(Bit32u)(bytes + test_mask)];
  if (BX_MEM_THIS actual_vector == 0) {
    BX_PANIC(("alloc_vector_aligned: unable to allocate host RAM !"));
    return 0;
  }
  // round address forward to nearest multiple of alignment.  Alignment
  // MUST BE a power of two for this to work.
  Bit64u masked = ((Bit64u)(BX_MEM_THIS actual_vector + test_mask)) & ~test_mask;
  Bit8u *vector = (Bit8u *) masked;
  // sanity check: no lost bits during pointer conversion
  assert(sizeof(masked) >= sizeof(vector));
  // sanity check: after realignment, everything fits in allocated space
  assert(vector+bytes <= BX_MEM_THIS actual_vector+bytes+test_mask);
  return vector;
}

BX_MEM_C::~BX_MEM_C()
{
#if BX_LARGE_RAMFILE
  if (overflow_file)
    fclose(BX_MEM_THIS overflow_file);
#endif

  cleanup_memory();
}

void BX_MEM_C::init_memory(Bit64u guest, Bit64u host, Bit32u block_size)
{
  unsigned i, idx;

  // accept only memory size which is multiply of 1M
  BX_ASSERT((host & 0xfffff) == 0);
  BX_ASSERT((guest & 0xfffff) == 0);

  if (! is_power_of_2(block_size)) {
    BX_PANIC(("Block size %d is not power of two !", block_size));
  }

  if (BX_MEM_THIS actual_vector != NULL) {
    BX_INFO(("freeing existing memory vector"));
    delete [] BX_MEM_THIS actual_vector;
    BX_MEM_THIS actual_vector = NULL;
    BX_MEM_THIS vector = NULL;
    BX_MEM_THIS blocks = NULL;
  }
  BX_MEM_THIS vector = alloc_vector_aligned(host + BIOSROMSZ + EXROMSIZE + 4096, BX_MEM_VECTOR_ALIGN);
  BX_INFO(("allocated memory at %p. after alignment, vector=%p, block_size = %dK",
        BX_MEM_THIS actual_vector, BX_MEM_THIS vector, block_size/1024));

  BX_MEM_THIS len = guest;
  BX_MEM_THIS allocated = host;
  BX_MEM_THIS rom = &BX_MEM_THIS vector[host];
  BX_MEM_THIS bogus = &BX_MEM_THIS vector[host + BIOSROMSZ + EXROMSIZE];
  memset(BX_MEM_THIS rom, 0xff, BIOSROMSZ + EXROMSIZE + 4096);

  BX_MEM_THIS block_size = block_size;
  // block must be large enough to fit num_blocks in 32-bit
  BX_ASSERT((BX_MEM_THIS len / BX_MEM_THIS block_size) <= 0xffffffff);

  Bit32u num_blocks = (Bit32u)(BX_MEM_THIS len / BX_MEM_THIS block_size);
  BX_INFO(("%.2fMB", (float)(BX_MEM_THIS len / (1024.0*1024.0))));
  BX_INFO(("mem block size = 0x%08x, blocks=%u", BX_MEM_THIS block_size, num_blocks));
  BX_MEM_THIS blocks = new Bit8u* [num_blocks];
  if (0) {
    // all guest memory is allocated, just map it
    for (idx = 0; idx < num_blocks; idx++) {
      BX_MEM_THIS blocks[idx] = BX_MEM_THIS vector + (idx * BX_MEM_THIS block_size);
    }
    BX_MEM_THIS used_blocks = num_blocks;
  }
  else {
    // host cannot allocate all requested guest memory
    for (idx = 0; idx < num_blocks; idx++) {
      BX_MEM_THIS blocks[idx] = NULL;
    }
    BX_MEM_THIS used_blocks = 0;
  }

  BX_MEM_THIS memory_handlers = new struct memory_handler_struct *[BX_MEM_HANDLERS];
  for (idx = 0; idx < BX_MEM_HANDLERS; idx++)
    BX_MEM_THIS memory_handlers[idx] = NULL;

  BX_MEM_THIS pci_enabled = SIM->get_param_bool(BXPN_PCI_ENABLED)->get();
  BX_MEM_THIS bios_write_enabled = 0;
  BX_MEM_THIS bios_rom_addr = 0xffff0000;
  BX_MEM_THIS flash_type = 0;
  BX_MEM_THIS flash_status = 0x80;
  BX_MEM_THIS flash_wsm_state = FLASH_READ_ARRAY;

  BX_MEM_THIS smram_available = 0;
  BX_MEM_THIS smram_enable = 0;
  BX_MEM_THIS smram_restricted = 0;

  for (i = 0; i < 65; i++)
    BX_MEM_THIS rom_present[i] = 0;
  for (i = 0; i <= BX_MEM_AREA_F0000; i++) {
    BX_MEM_THIS memory_type[i][0] = 0;
    BX_MEM_THIS memory_type[i][1] = 0;
  }

  BX_MEM_THIS register_state();
}

Bit8u* BX_MEM_C::get_vector(bx_phy_address addr)
{
  Bit32u block = (Bit32u)(addr / BX_MEM_THIS block_size);
#if (BX_LARGE_RAMFILE)
  if (!BX_MEM_THIS blocks[block] || (BX_MEM_THIS blocks[block] == BX_MEM_THIS swapped_out))
#else
  if (!BX_MEM_THIS blocks[block])
#endif
    allocate_block(block);

  return BX_MEM_THIS blocks[block] + (Bit32u)(addr & (BX_MEM_THIS block_size-1));
}

#if BX_LARGE_RAMFILE
void BX_MEM_C::read_block(Bit32u block)
{
  const Bit64u block_address = Bit64u(block) * BX_MEM_THIS block_size;

  if (fseeko64(BX_MEM_THIS overflow_file, block_address, SEEK_SET))
    BX_PANIC(("FATAL ERROR: Could not seek to 0x" FMT_LL "x in memory overflow file!", block_address));

  // We could legitimately get an EOF condition if we are reading the last bit of memory.ram
  if ((fread(BX_MEM_THIS blocks[block], BX_MEM_THIS block_size, 1, BX_MEM_THIS overflow_file) != 1) && 
      (!feof(BX_MEM_THIS overflow_file))) 
    BX_PANIC(("FATAL ERROR: Could not read from 0x" FMT_LL "x in memory overflow file!", block_address)); 
}
#endif

void BX_MEM_C::allocate_block(Bit32u block)
{
  const Bit32u max_blocks = (Bit32u)(BX_MEM_THIS allocated / BX_MEM_THIS block_size);

#if BX_LARGE_RAMFILE
  /* 
   * Match block to vector address
   * First, see if there is any spare host memory blocks we can still freely allocate
   */
  if (BX_MEM_THIS used_blocks >= max_blocks) {
    Bit32u original_replacement_block = BX_MEM_THIS next_swapout_idx;
    // Find a block to replace
    bool used_for_tlb;
    Bit8u *buffer;
    do {
      do {
        // Wrap if necessary
        if (++(BX_MEM_THIS next_swapout_idx)==((BX_MEM_THIS len)/BX_MEM_THIS block_size))
          BX_MEM_THIS next_swapout_idx = 0;
        if (BX_MEM_THIS next_swapout_idx == original_replacement_block)
          BX_PANIC(("FATAL ERROR: Insufficient working RAM, all blocks are currently used for TLB entries!"));
        buffer = BX_MEM_THIS blocks[BX_MEM_THIS next_swapout_idx];
      } while ((!buffer) || (buffer == BX_MEM_C::swapped_out));

      used_for_tlb = false;
      // tlb buffer check loop
      const Bit8u* buffer_end = buffer+BX_MEM_THIS block_size;
      // Don't replace it if any CPU is using it as a TLB entry
      for (int i=0; i<BX_SMP_PROCESSORS && !used_for_tlb;i++)
        used_for_tlb = BX_CPU(i)->check_addr_in_tlb_buffers(buffer, buffer_end);
    } while (used_for_tlb);
    // Flush the block to be replaced
    bx_phy_address address = bx_phy_address(BX_MEM_THIS next_swapout_idx) * BX_MEM_THIS block_size;
    // Create overflow file if it does not currently exist.
    if (!BX_MEM_THIS overflow_file) {
      BX_MEM_THIS overflow_file = tmpfile64();
      if (!BX_MEM_THIS overflow_file)
        BX_PANIC(("Unable to allocate memory overflow file"));
    }
    // Write swapped out block
    if (fseeko64(BX_MEM_THIS overflow_file, address, SEEK_SET))
      BX_PANIC(("FATAL ERROR: Could not seek to 0x" FMT_PHY_ADDRX " in overflow file!", address)); 
    if (1 != fwrite (BX_MEM_THIS blocks[BX_MEM_THIS next_swapout_idx], BX_MEM_THIS block_size, 1, BX_MEM_THIS overflow_file))
      BX_PANIC(("FATAL ERROR: Could not write at 0x" FMT_PHY_ADDRX " in overflow file!", address));
    // Mark swapped out block
    BX_MEM_THIS blocks[BX_MEM_THIS next_swapout_idx] = BX_MEM_C::swapped_out;
    BX_MEM_THIS blocks[block] = buffer;
    read_block(block);
    BX_DEBUG(("allocate_block: block=0x%x, replaced 0x%x", block, BX_MEM_THIS next_swapout_idx));
  }
  else {
    BX_MEM_THIS blocks[block] = BX_MEM_THIS vector + (BX_MEM_THIS used_blocks++ * BX_MEM_THIS block_size);
    BX_DEBUG(("allocate_block: block=0x%x used 0x%x of 0x%x",
          block, BX_MEM_THIS used_blocks, max_blocks));
  }
#else
  // Legacy default allocator
  if (BX_MEM_THIS used_blocks >= max_blocks) {
    BX_PANIC(("FATAL ERROR: all available memory is already allocated !"));
  }
  else {
    BX_MEM_THIS blocks[block] = BX_MEM_THIS vector + (BX_MEM_THIS used_blocks * BX_MEM_THIS block_size);
    BX_MEM_THIS used_blocks++;
  }
  BX_DEBUG(("allocate_block: used_blocks=0x%x of 0x%x", BX_MEM_THIS used_blocks, max_blocks));
#endif
}

#if BX_LARGE_RAMFILE
// The blocks in RAM must also be flushed to the save file.
void ramfile_save_handler(void *devptr, FILE *fp)
{
  for (Bit32u idx = 0; idx < (BX_MEM(0)->len / BX_MEM_THIS block_size); idx++) {
    if ((BX_MEM(0)->blocks[idx]) && (BX_MEM(0)->blocks[idx] != BX_MEM(0)->swapped_out))
    {
      bx_phy_address address = bx_phy_address(idx) * BX_MEM_THIS block_size;
      if (fseeko64(fp, address, SEEK_SET))
        BX_PANIC(("FATAL ERROR: Could not seek to 0x" FMT_PHY_ADDRX " in overflow file!", address)); 
      if (1 != fwrite (BX_MEM(0)->blocks[idx], BX_MEM_THIS block_size, 1, fp))
        BX_PANIC(("FATAL ERROR: Could not write at 0x" FMT_PHY_ADDRX " in overflow file!", address));
    }
  }
}
#endif

// Note: This must be called before the memory file save handler is called.
Bit64s memory_param_save_handler(void *devptr, bx_param_c *param)
{
  const char *pname = param->get_name();
  if (! strncmp(pname, "blk", 3)) {
    Bit32u blk_index = atoi(pname + 3);
    if (! BX_MEM(0)->blocks[blk_index])
      return -1;
#if BX_LARGE_RAMFILE
    // If swapped out, will be saved by common handler.
    if (BX_MEM(0)->blocks[blk_index] == BX_MEM(0)->swapped_out)
      return -2;
#endif
    // Return the block offset into the array
    Bit32u val = (Bit32u) (BX_MEM(0)->blocks[blk_index] - BX_MEM(0)->vector);
    if ((val & (BX_MEM_THIS block_size-1)) == 0)
       return val / BX_MEM_THIS block_size;
  }
  return -1;
}

void memory_param_restore_handler(void *devptr, bx_param_c *param, Bit64s val)
{
  const char *pname = param->get_name();
  if (! strncmp(pname, "blk", 3)) {
    Bit32u blk_index = atoi(pname + 3);
#if BX_LARGE_RAMFILE
    if ((Bit32s) val == -2) {
      BX_MEM(0)->blocks[blk_index] = BX_MEM(0)->swapped_out;
      return;
    }
#endif
     if((Bit32s) val < 0) {
        BX_MEM(0)->blocks[blk_index] = NULL;
        return;
      }
      BX_MEM(0)->blocks[blk_index] = BX_MEM(0)->vector + val * BX_MEM_THIS block_size;
#if BX_LARGE_RAMFILE
      BX_MEM(0)->read_block(blk_index);
#endif
  }
}

void BX_MEM_C::register_state()
{
  char param_name[15];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "memory", "Memory State");
  Bit32u num_blocks = (Bit32u)(BX_MEM_THIS len / BX_MEM_THIS block_size);
#if BX_LARGE_RAMFILE
  bx_shadow_filedata_c *ramfile = new bx_shadow_filedata_c(list, "ram", &(BX_MEM_THIS overflow_file));
  ramfile->set_sr_handlers(this, ramfile_save_handler, (filedata_restore_handler)NULL);
  BXRS_DEC_PARAM_FIELD(list, next_swapout_idx, BX_MEM_THIS next_swapout_idx);
#else
  new bx_shadow_data_c(list, "ram", BX_MEM_THIS vector, BX_MEM_THIS allocated);
#endif
  BXRS_DEC_PARAM_FIELD(list, used_blocks, BX_MEM_THIS used_blocks);

  bx_list_c *mapping = new bx_list_c(list, "mapping");
  for (Bit32u blk=0; blk < num_blocks; blk++) {
    sprintf(param_name, "blk%d", blk);
    bx_param_num_c *param = new bx_param_num_c(mapping, param_name, "", "", -2, BX_MAX_BIT32U, 0);
    param->set_base(BASE_DEC);
    param->set_sr_handlers(this, memory_param_save_handler, memory_param_restore_handler);
  }
  bx_list_c *memtype = new bx_list_c(list, "memtype");
  for (int i = 0; i <= BX_MEM_AREA_F0000; i++) {
    sprintf(param_name, "%d_r", i);
    new bx_shadow_bool_c(memtype, param_name, &BX_MEM_THIS memory_type[i][0]);
    sprintf(param_name, "%d_w", i);
    new bx_shadow_bool_c(memtype, param_name, &BX_MEM_THIS memory_type[i][1]);
  }
  BXRS_HEX_PARAM_FIELD(list, flash_status, BX_MEM_THIS flash_status);
  BXRS_DEC_PARAM_FIELD(list, flash_wsm_state, BX_MEM_THIS flash_wsm_state);
}

void BX_MEM_C::cleanup_memory()
{
  unsigned idx;

  if (BX_MEM_THIS vector != NULL) {
    delete [] BX_MEM_THIS actual_vector;
    BX_MEM_THIS actual_vector = NULL;
    BX_MEM_THIS vector = NULL;
    BX_MEM_THIS rom = NULL;
    BX_MEM_THIS bogus = NULL;
    delete [] BX_MEM_THIS blocks;
    BX_MEM_THIS blocks = 0;
    BX_MEM_THIS used_blocks = 0;
    if (BX_MEM_THIS memory_handlers != NULL) {
      for (idx = 0; idx < BX_MEM_HANDLERS; idx++) {
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
  bool is_bochs_bios = 0;

  if (*path == '\0') {
    if (type == 2) {
      BX_PANIC(("ROM: Optional ROM image undefined"));
    }
    else if (type == 1) {
      BX_PANIC(("ROM: VGA BIOS image undefined"));
    }
    else {
      BX_PANIC(("ROM: System BIOS image undefined"));
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
      BX_PANIC(("ROM: couldn't open ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(("ROM: couldn't open ROM image file '%s'.", path));
    }
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    if (type < 2) {
      close(fd);
      BX_PANIC(("ROM: couldn't stat ROM image file '%s'.", path));
    }
    else {
      close(fd);
      BX_ERROR(("ROM: couldn't stat ROM image file '%s'.", path));
    }
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  if (type > 0) {
    max_size = 0x20000;
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
      romaddress = ~(size - 1);
    }
    offset = romaddress & BIOS_MASK;
    if ((romaddress & 0xf0000) < 0xf0000) {
      BX_MEM_THIS rom_present[64] = 1;
    }
    BX_MEM_THIS bios_rom_addr = (Bit32u)romaddress;
    is_bochs_bios = ((strstr(path, "BIOS-bochs-latest") != NULL) ||
                     (strstr(path, "BIOS-bochs-legacy") != NULL));
    if (size == 0x40000) {
      BX_MEM_THIS flash_type = 2; // 28F002BC-T
    } else if (size == 0x20000) {
      BX_MEM_THIS flash_type = 1; // 28F001BX-T
    }
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
      start_idx = (((Bit32u)romaddress - 0xc0000) >> 11);
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
      BX_PANIC(("ROM: read failed on BIOS image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  offset -= (unsigned long)stat_buf.st_size;
  size = (unsigned long)stat_buf.st_size;
  if (is_bochs_bios ||
      ((BX_MEM_THIS rom[offset] == 0x55) && (BX_MEM_THIS rom[offset+1] == 0xaa))) {
    if ((type == 0) && ((romaddress & 0xfffff) == 0xe0000)) {
      offset += 0x10000;
      size = 0x10000;
    }
    Bit8u checksum = 0;
    for (i = 0; i < (int)size; i++) {
      checksum += BX_MEM_THIS rom[offset + i];
    }
    if (checksum != 0) {
      if (type == 1) {
        BX_PANIC(("ROM: checksum error in VGABIOS image: '%s'", path));
      } else if (is_bochs_bios) {
        BX_ERROR(("ROM: checksum error in BIOS image: '%s'", path));
      }
    }
  }
  BX_INFO(("rom at 0x%05x/%u ('%s')",
                        (unsigned) romaddress,
                        (unsigned) stat_buf.st_size,
                         path));
}

void BX_MEM_C::load_RAM(const char *path, bx_phy_address ramaddress)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, offset;

  if (*path == '\0') {
    BX_PANIC(("RAM: Optional RAM image undefined"));
    return;
  }
  // read in RAM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_PANIC(("RAM: couldn't open RAM image file '%s'.", path));
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    close(fd);
    BX_PANIC(("RAM: couldn't stat RAM image file '%s'.", path));
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  offset = (unsigned long)ramaddress;
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) BX_MEM_THIS get_vector(offset), size);
    if (ret <= 0) {
      BX_PANIC(("RAM: read failed on RAM image: '%s'",path));
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

bool BX_MEM_C::dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;
  bool ret = 1, use_memory_handler = 0, use_smram = 0;

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = 0;
#endif

  if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
  {
    // SMRAM memory space
    if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
      use_smram = 1;
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr && memory_handler->end >= a20addr)
    {
      if (!use_smram) {
        use_memory_handler = 1;
        break;
      }
    }
    memory_handler = memory_handler->next;
  }

  for (; len>0; len--) {
    if (use_memory_handler) {
      memory_handler->read_handler(a20addr, 1, buf, memory_handler->param);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][0] == 0) {
        // Read from ROM
        if ((a20addr & 0xfffe0000) == 0x000e0000) {
          // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
          if (BX_MEM_THIS flash_type > 0) {
            *buf = BX_MEM_THIS flash_read(BIOS_MAP_LAST128K(a20addr));
          } else {
            *buf = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
          }
        } else {
          *buf = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
        }
      } else {
        // Read from ShadowRAM
        *buf = *(BX_MEM_THIS get_vector(a20addr));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ((a20addr < BX_MEM_THIS len) && !is_bios)
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        *buf = *(BX_MEM_THIS get_vector(a20addr));
      }
      // must be in C0000 - FFFFF range
      else if ((a20addr & 0xfffe0000) == 0x000e0000) {
        // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
        *buf = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
      } else {
        *buf = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
      }
    }
#if BX_PHY_ADDRESS_LONG
    else if (a20addr > BX_CONST64(0xffffffff)) {
      *buf = 0xff;
      ret = 0; // error, beyond limits of memory
    }
#endif
    else if (is_bios)
    {
      if (BX_MEM_THIS flash_type > 0) {
        *buf = BX_MEM_THIS flash_read(a20addr & BIOS_MASK);
      } else {
        *buf = BX_MEM_THIS rom[a20addr & BIOS_MASK];
      }
    }
    else
    {
      *buf = 0xff;
      ret = 0; // error, beyond limits of memory
    }
    buf++;
    a20addr++;
  }
  return ret;
}

#if BX_DEBUGGER || BX_GDBSTUB
bool BX_MEM_C::dbg_set_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;
  bool use_memory_handler = 0, use_smram = 0;

  if ((a20addr + len - 1) > BX_MEM_THIS len) {
    return(0); // error, beyond limits of memory
  }

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = 0;
#endif

  if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
  {
    // SMRAM memory space
    if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
      use_smram = 1;
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr && memory_handler->end >= a20addr)
    {
      if (!use_smram) {
        use_memory_handler = 1;
        break;
      }
    }
    memory_handler = memory_handler->next;
  }

  for (; len>0; len--) {
    if (use_memory_handler) {
      memory_handler->write_handler(a20addr, 1, buf, memory_handler->param);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][1] == 1) {
        // Write to ShadowRAM
        *(BX_MEM_THIS get_vector(a20addr)) = *buf;
      } else {
        // Ignore write to ROM
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ((a20addr < 0x000c0000 || a20addr >= 0x00100000) && !is_bios)
    {
      *(BX_MEM_THIS get_vector(a20addr)) = *buf;
    }
    buf++;
    a20addr++;
  }
  return(1);
}

bool BX_MEM_C::dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc)
{
  *crc = 0;
  if (addr1 > addr2)
    return(0);

  if (addr2 >= BX_MEM_THIS len)
    return(0); // error, specified address past last phy mem addr

  unsigned len = 1 + addr2 - addr1;

  // do not cross 4K boundary
  while(1) { 
    unsigned remainsInPage = 0x1000 - (addr1 & 0xfff);
    unsigned access_length = (len < remainsInPage) ? len : remainsInPage;
    *crc = crc32(BX_MEM_THIS get_vector(addr1), access_length);
    addr1 += access_length;
    len -= access_length;
  }

  return(1);
}
#endif

//
// Return a host address corresponding to the guest physical memory
// address (with A20 already applied), given that the calling
// code will perform an 'op' operation.  This address will be
// used for direct access to guest memory.
// Values of 'op' are { BX_READ, BX_WRITE, BX_EXECUTE, BX_RW }.
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

Bit8u *BX_MEM_C::getHostMemAddr(BX_CPU_C *cpu, bx_phy_address addr, unsigned rw)
{
  bx_phy_address a20addr = A20ADDR(addr);

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = 0;
#endif

  bool write = rw & 1;

  // allow direct access to SMRAM memory space for code and veto data
  if ((cpu != NULL) && (rw == BX_EXECUTE)) {
    // reading from SMRAM memory space
    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && (BX_MEM_THIS smram_available))
    {
      if (BX_MEM_THIS smram_enable || cpu->smm_mode())
        return BX_MEM_THIS get_vector(a20addr);
    }
  }

#if BX_SUPPORT_MONITOR_MWAIT
  if (write && BX_MEM_THIS is_monitor(a20addr & ~((bx_phy_address)(0xfff)), 0xfff)) {
    // Vetoed! Write monitored page !
    return(NULL);
  }
#endif

  struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr &&
        memory_handler->end >= a20addr) {
      if (memory_handler->da_handler)
        return memory_handler->da_handler(a20addr, rw, memory_handler->param);
      else
        return(NULL); // Vetoed! memory handler for i/o apic, vram, mmio and PCI PnP
    }
    memory_handler = memory_handler->next;
  }

  if (! write) {
    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000))
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][0] == 0) {
        // Read from ROM
        if ((a20addr & 0xfffe0000) == 0x000e0000) {
          // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
          return (Bit8u *) &BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
        } else {
          return (Bit8u *) &BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
        }
      } else {
        // Read from ShadowRAM
        return BX_MEM_THIS get_vector(a20addr);
      }
    }
#endif
    else if ((a20addr < BX_MEM_THIS len) && !is_bios)
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        return BX_MEM_THIS get_vector(a20addr);
      }
      // must be in C0000 - FFFFF range
      else if ((a20addr & 0xfffe0000) == 0x000e0000) {
        // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
        return (Bit8u *) &BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
      }
      else {
        return((Bit8u *) &BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ]);
      }
    }
#if BX_PHY_ADDRESS_LONG
    else if (a20addr > BX_CONST64(0xffffffff)) {
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20addr & 0xfff];
    }
#endif
    else if (is_bios)
    {
      return (Bit8u *) &BX_MEM_THIS rom[a20addr & BIOS_MASK];
    }
    else
    {
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20addr & 0xfff];
    }
  }
  else
  { // op == {BX_WRITE, BX_RW}
    if ((a20addr >= BX_MEM_THIS len) || is_bios)
      return(NULL); // Error, requested addr is out of bounds.
    else if (a20addr >= 0x000a0000 && a20addr < 0x000c0000)
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000))
    {
      // Veto direct writes to this area. Otherwise, there is a chance
      // for Guest2HostTLB and memory consistency problems, for example
      // when some 16K block marked as write-only using PAM registers.
      return(NULL);
    }
#endif
    else
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        return BX_MEM_THIS get_vector(a20addr);
      }
      else {
        return(NULL);  // Vetoed!  ROMs
      }
    }
  }
}

/*
 * One needs to provide both a read_handler and a write_handler.
 */
  bool
BX_MEM_C::registerMemoryHandlers(void *param, memory_handler_t read_handler,
                memory_handler_t write_handler, memory_direct_access_handler_t da_handler,
                bx_phy_address begin_addr, bx_phy_address end_addr)
{
  if (end_addr < begin_addr)
    return 0;
  if (!read_handler) // allow NULL write and fetch handler
    return 0;
  BX_INFO(("Register memory access handlers: 0x" FMT_PHY_ADDRX " - 0x" FMT_PHY_ADDRX, begin_addr, end_addr));
  for (Bit32u page_idx = (Bit32u)(begin_addr >> 20); page_idx <= (Bit32u)(end_addr >> 20); page_idx++) {
    Bit16u bitmap = 0xffff;
    if (begin_addr > (page_idx << 20)) {
      bitmap &= (0xffff << ((begin_addr >> 16) & 0xf));
    }
    if (end_addr < ((page_idx + 1) << 20)) {
      bitmap &= (0xffff >> (0x0f - ((end_addr >> 16) & 0xf)));
    }
    if (BX_MEM_THIS memory_handlers[page_idx] != NULL) {
      if ((bitmap & BX_MEM_THIS memory_handlers[page_idx]->bitmap) != 0) {
        BX_ERROR(("Register failed: overlapping memory handlers!"));
        return 0;
      } else {
        bitmap |= BX_MEM_THIS memory_handlers[page_idx]->bitmap;
      }
    }
    struct memory_handler_struct *memory_handler = new struct memory_handler_struct;
    memory_handler->next = BX_MEM_THIS memory_handlers[page_idx];
    BX_MEM_THIS memory_handlers[page_idx] = memory_handler;
    memory_handler->read_handler = read_handler;
    memory_handler->write_handler = write_handler;
    memory_handler->da_handler = da_handler;
    memory_handler->param = param;
    memory_handler->begin = begin_addr;
    memory_handler->end = end_addr;
    memory_handler->bitmap = bitmap;
  }
  return 1;
}

  bool
BX_MEM_C::unregisterMemoryHandlers(void *param, bx_phy_address begin_addr, bx_phy_address end_addr)
{
  bool ret = 1;
  BX_INFO(("Memory access handlers unregistered: 0x" FMT_PHY_ADDRX " - 0x" FMT_PHY_ADDRX, begin_addr, end_addr));
  for (Bit32u page_idx = (Bit32u)(begin_addr >> 20); page_idx <= (Bit32u)(end_addr >> 20); page_idx++) {
    Bit16u bitmap = 0xffff;
    if (begin_addr > (page_idx << 20)) {
      bitmap &= (0xffff << ((begin_addr >> 16) & 0xf));
    }
    if (end_addr < ((page_idx + 1) << 20)) {
      bitmap &= (0xffff >> (0x0f - ((end_addr >> 16) & 0xf)));
    }
    struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[page_idx];
    struct memory_handler_struct *prev = NULL;
    while (memory_handler &&
         memory_handler->param != param &&
         memory_handler->begin != begin_addr &&
         memory_handler->end != end_addr)
    {
      memory_handler->bitmap &= ~bitmap;
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

void BX_MEM_C::enable_smram(bool enable, bool restricted)
{
  BX_MEM_THIS smram_available = 1;
  BX_MEM_THIS smram_enable = enable;
  BX_MEM_THIS smram_restricted = restricted;
}

void BX_MEM_C::disable_smram(void)
{
  BX_MEM_THIS smram_available  = 0;
  BX_MEM_THIS smram_enable     = 0;
  BX_MEM_THIS smram_restricted = 0;
}

// check if SMRAM is aavailable for CPU data accesses
bool BX_MEM_C::is_smram_accessible(void)
{
  return(BX_MEM_THIS smram_available) &&
        (BX_MEM_THIS smram_enable || !BX_MEM_THIS smram_restricted);
}

void BX_MEM_C::set_memory_type(memory_area_t area, bool rw, bool dram)
{
  if (area <= BX_MEM_AREA_F0000) {
    BX_MEM_THIS memory_type[area][rw] = dram;
  }
}

void BX_MEM_C::set_bios_write(bool enabled)
{
  BX_MEM_THIS bios_write_enabled = enabled;
}

void BX_MEM_C::set_bios_rom_access(Bit8u region, bool enabled)
{
  if (enabled) {
    BX_MEM_THIS bios_rom_access |= region;
  } else {
    BX_MEM_THIS bios_rom_access &= ~region;
  }
}

Bit8u BX_MEM_C::flash_read(Bit32u addr)
{
  Bit8u ret = 0;

  switch (BX_MEM_THIS flash_wsm_state) {
    case FLASH_INT_ID:
      if (addr & 1) {
        ret = (BX_MEM_THIS flash_type == 2) ? 0x7c : 0x94;
      } else {
        ret = 0x89;
      }
      BX_DEBUG(("flash read ID (address = 0x%08x value = 0x%02x)", addr, ret));
      break;
    case FLASH_READ_ARRAY:
      BX_DEBUG(("flash read from ROM (address = 0x%08x)", addr));
      ret = BX_MEM_THIS rom[addr];
      break;
    default:
      ret = BX_MEM_THIS flash_status;
      if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE) {
        BX_MEM_THIS flash_status |= 0x80;
      }
      BX_DEBUG(("flash read status (address = 0x%08x value = 0x%02x)", addr, ret));
  }
  return ret;
}

void BX_MEM_C::flash_write(Bit32u addr, Bit8u data)
{
  Bit32u flash_addr;
  int i;

  if (BX_MEM_THIS flash_type == 2) {
    flash_addr = addr & 0x3ffff;
  } else {
    flash_addr = addr & 0x1ffff;
  }
  if (BX_MEM_THIS flash_wsm_state == FLASH_PROG_SETUP) {
    BX_DEBUG(("flash write to ROM (address = 0x%08x, data = 0x%02x)", flash_addr, data));
    BX_MEM_THIS rom[addr] &= data;
    BX_MEM_THIS flash_wsm_state = FLASH_READ_STATUS;
  } else {
    BX_DEBUG(("flash write command (address = 0x%08x, code = 0x%02x)", flash_addr, data));
    switch (data) {
      case FLASH_INT_ID:
      case FLASH_READ_ARRAY:
      case FLASH_ERASE_SETUP:
      case FLASH_ERASE_SUSP:
      case FLASH_PROG_SETUP:
        BX_MEM_THIS flash_wsm_state = data;
        break;
      case FLASH_READ_STATUS:
        if (BX_MEM_THIS flash_wsm_state != FLASH_ERASE) {
          BX_MEM_THIS flash_wsm_state = data;
        }
        break;
      case FLASH_CLR_STATUS:
        BX_MEM_THIS flash_status &= ~0x38;
        BX_MEM_THIS flash_wsm_state = FLASH_READ_ARRAY;
        break;
      case FLASH_ERASE:
        if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE_SETUP) {
          BX_MEM_THIS flash_status &= ~0xc0;
          BX_MEM_THIS flash_wsm_state = FLASH_ERASE;
          if ((BX_MEM_THIS flash_type == 1) &&
              ((flash_addr = 0x1c000) || (flash_addr = 0x1d000))) {
            for (i = 0; i < 0x1000; i++) {
              BX_MEM_THIS rom[addr + i] = 0xff;
            }
          } else if ((BX_MEM_THIS flash_type == 2) &&
                     ((flash_addr = 0x38000) || (flash_addr = 0x3a000))) {
            for (i = 0; i < 0x2000; i++) {
              BX_MEM_THIS rom[addr + i] = 0xff;
            }
          }
        } else if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE_SUSP) {
          BX_MEM_THIS flash_status &= ~0x40;
          BX_MEM_THIS flash_wsm_state = FLASH_ERASE;
        } else {
          BX_DEBUG(("flash_write(): unexpected ERASE CONFIRM / ERASE RESUME"));
        }
        break;
      default:
        BX_DEBUG(("flash_write(): unsupported code 0x%02x", data));
    }
  }
}

#if BX_SUPPORT_MONITOR_MWAIT

//
// MONITOR/MWAIT - x86arch way to optimize idle loops in CPU
//

bool BX_MEM_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    if (BX_CPU(i)->is_monitor(begin_addr, len))
      return 1;
  }

  return 0; // // this is NOT monitored page
}

void BX_MEM_C::check_monitor(bx_phy_address begin_addr, unsigned len)
{
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    BX_CPU(i)->check_monitor(begin_addr, len);
  }
}

#endif
