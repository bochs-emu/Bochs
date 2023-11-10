/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2023  The Bochs Project
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
#include "pc_system.h"
#include "param_names.h"
#include "cpu/cpu.h"
#include "memory/memory-bochs.h"
#define LOG_THIS BX_MEM(0)->

// block size must be power of two
BX_CPP_INLINE bool is_power_of_2(Bit64u x)
{
    return (x & (x - 1)) == 0;
}

// alignment of memory vector, must be a power of 2
#define BX_MEM_VECTOR_ALIGN 4096

#if BX_LARGE_RAMFILE
Bit8u* const BX_MEMORY_STUB_C::swapped_out = ((Bit8u*)NULL - sizeof(Bit8u));
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// MEMORY_STUB class implements RAM vector only

BX_MEMORY_STUB_C::BX_MEMORY_STUB_C()
{
  put("memory", "MEM0");

  vector = NULL;
  actual_vector = NULL;
  blocks = NULL;
  rom    = NULL;
  bogus  = NULL;
  len    = 0;
  used_blocks = 0;
  allocated   = 0;

#if BX_LARGE_RAMFILE
  next_swapout_idx = 0;
  overflow_file = NULL;
#endif
}

BX_MEMORY_STUB_C::~BX_MEMORY_STUB_C()
{
#if BX_LARGE_RAMFILE
  if (overflow_file)
    fclose(BX_MEM_THIS overflow_file);
#endif

  cleanup_memory();
}

Bit64u BX_MEMORY_STUB_C::get_memory_len(void)
{
  return (BX_MEM_THIS len);
}

Bit8u* BX_MEMORY_STUB_C::alloc_vector_aligned(Bit64u bytes, Bit64u alignment)
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

void BX_MEMORY_STUB_C::init_memory(Bit64u guest, Bit64u host, Bit32u block_size)
{
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
  BX_MEM_THIS rom   = &BX_MEM_THIS vector[host];
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
    for (unsigned idx = 0; idx < num_blocks; idx++) {
      BX_MEM_THIS blocks[idx] = BX_MEM_THIS vector + (idx * BX_MEM_THIS block_size);
    }
    BX_MEM_THIS used_blocks = num_blocks;
  }
  else {
    // host cannot allocate all requested guest memory
    for (unsigned idx = 0; idx < num_blocks; idx++) {
      BX_MEM_THIS blocks[idx] = NULL;
    }
    BX_MEM_THIS used_blocks = 0;
  }
}

Bit8u* BX_MEMORY_STUB_C::get_vector(bx_phy_address addr)
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
void BX_MEMORY_STUB_C::read_block(Bit32u block)
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

void BX_MEMORY_STUB_C::allocate_block(Bit32u block)
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
      } while ((!buffer) || (buffer == BX_MEMORY_STUB_C::swapped_out));

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
    BX_MEM_THIS blocks[BX_MEM_THIS next_swapout_idx] = BX_MEMORY_STUB_C::swapped_out;
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

void BX_MEMORY_STUB_C::cleanup_memory()
{
  if (BX_MEM_THIS vector != NULL) {
    delete [] BX_MEM_THIS actual_vector;
    BX_MEM_THIS actual_vector = NULL;
    BX_MEM_THIS vector = NULL;
    BX_MEM_THIS rom = NULL;
    BX_MEM_THIS bogus = NULL;
    delete [] BX_MEM_THIS blocks;
    BX_MEM_THIS blocks = NULL;
    BX_MEM_THIS used_blocks = 0;
    BX_MEM_THIS allocated = 0;
    BX_MEM_THIS len = 0;
  }
}

bool BX_MEMORY_STUB_C::dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);
  bool ret = true;

  for (; len>0; len--) {
    if (a20addr < BX_MEM_THIS len) {
      *buf = *(BX_MEM_THIS get_vector(a20addr));
    }
#if BX_PHY_ADDRESS_LONG
    else if (a20addr > BX_CONST64(0xffffffff)) {
      *buf = 0xff;
      ret = false; // error, beyond limits of memory
    }
#endif
    else {
      *buf = 0xff;
      ret = false; // error, beyond limits of memory
    }
    buf++;
    a20addr++;
  }

  return ret;
}

#if BX_DEBUGGER || BX_GDBSTUB
bool BX_MEMORY_STUB_C::dbg_set_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);

  if ((a20addr + len - 1) > BX_MEM_THIS len) {
    return false; // error, beyond limits of memory
  }

  for (; len>0; len--) {
    *(BX_MEM_THIS get_vector(a20addr)) = *buf;
    buf++;
    a20addr++;
  }
  return true;
}

bool BX_MEMORY_STUB_C::dbg_crc32(bx_phy_address addr1, bx_phy_address addr2, Bit32u *crc)
{
  *crc = 0;
  if (addr1 > addr2)
    return false;

  if (addr2 >= BX_MEM_THIS len)
    return false; // error, specified address past last phy mem addr

  unsigned len = 1 + addr2 - addr1;

  // do not cross 4K boundary
  while(1) {
    unsigned remainsInPage = 0x1000 - (addr1 & 0xfff);
    unsigned access_length = (len < remainsInPage) ? len : remainsInPage;
    *crc = crc32(BX_MEM_THIS get_vector(addr1), access_length);
    addr1 += access_length;
    len -= access_length;
  }

  return true;
}
#endif

//
// Return a host address corresponding to the guest physical memory
// address (with A20 already applied), given that the calling
// code will perform an 'op' operation.  This address will be
// used for direct access to guest memory.
// Values of 'op' are { BX_READ, BX_WRITE, BX_EXECUTE, BX_RW }.
//

//
// The other assumption is that the calling code _only_ accesses memory
// directly within the page that encompasses the address requested.
//

Bit8u *BX_MEMORY_STUB_C::getHostMemAddr(BX_CPU_C *cpu, bx_phy_address addr, unsigned rw)
{
  bx_phy_address a20addr = A20ADDR(addr);

  bool write = rw & 1;

#if BX_SUPPORT_MONITOR_MWAIT
  if (write && BX_MEM_THIS is_monitor(a20addr & ~((bx_phy_address)(0xfff)), 0xfff)) {
    // Vetoed! Write monitored page !
    return(NULL);
  }
#endif

  if (! write) {
    if (a20addr < BX_MEM_THIS len)
      return BX_MEM_THIS get_vector(a20addr);
    else
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20addr & 0xfff];
  }
  else
  { // op == {BX_WRITE, BX_RW}
    if (a20addr >= BX_MEM_THIS len)
      return(NULL); // Error, requested addr is out of bounds.
    return BX_MEM_THIS get_vector(a20addr);
  }
}

void BX_MEMORY_STUB_C::writePhysicalPage(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  bx_phy_address a20addr = A20ADDR(addr);

  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("writePhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_THIS check_monitor(a20addr, len);
#endif

  // all memory access fits in single 4K page
  if (a20addr < BX_MEM_THIS len) {
    // all of data is within limits of physical memory
    if (len == 8) {
      pageWriteStampTable.decWriteStamp(a20addr, 8);
      WriteHostQWordToLittleEndian((Bit64u*) BX_MEM_THIS get_vector(a20addr), *(Bit64u*)data);
      return;
    }
    if (len == 4) {
      pageWriteStampTable.decWriteStamp(a20addr, 4);
      WriteHostDWordToLittleEndian((Bit32u*) BX_MEM_THIS get_vector(a20addr), *(Bit32u*)data);
      return;
    }
    if (len == 2) {
      pageWriteStampTable.decWriteStamp(a20addr, 2);
      WriteHostWordToLittleEndian((Bit16u*) BX_MEM_THIS get_vector(a20addr), *(Bit16u*)data);
      return;
    }
    if (len == 1) {
      pageWriteStampTable.decWriteStamp(a20addr, 1);
      * (BX_MEM_THIS get_vector(a20addr)) = * (Bit8u *) data;
      return;
    }
    // len == other, just fall thru to special cases handling

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    while(1) {
      // Write in chunks of 8 bytes if we can
      if ((len & 7) == 0) {
        pageWriteStampTable.decWriteStamp(a20addr, 8);
        WriteHostQWordToLittleEndian((Bit64u*) BX_MEM_THIS get_vector(a20addr), *(Bit64u*)data_ptr);
        len -= 8;
        a20addr += 8;
#ifdef BX_LITTLE_ENDIAN
        data_ptr += 8;
#else
        data_ptr -= 8;
#endif
        if (len == 0) return;
      } else {
        pageWriteStampTable.decWriteStamp(a20addr, 1);
        *(BX_MEM_THIS get_vector(a20addr)) = *data_ptr;
        if (len == 1) return;
        len--;
        a20addr++;
#ifdef BX_LITTLE_ENDIAN
        data_ptr++;
#else // BX_BIG_ENDIAN
        data_ptr--;
#endif
      }
    }

    pageWriteStampTable.decWriteStamp(a20addr);

  } else {
    // access outside limits of physical memory, ignore
    BX_DEBUG(("Write outside the limits of physical memory (0x" FMT_PHY_ADDRX ") (ignore)", a20addr));
  }
}

void BX_MEMORY_STUB_C::readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  bx_phy_address a20addr = A20ADDR(addr);

  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("readPhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

  if (a20addr < BX_MEM_THIS len) {
    if (len == 8) {
      * (Bit64u*) data = ReadHostQWordFromLittleEndian((Bit64u*) BX_MEM_THIS get_vector(a20addr));
      return;
    }
    if (len == 4) {
      * (Bit32u*) data = ReadHostDWordFromLittleEndian((Bit32u*) BX_MEM_THIS get_vector(a20addr));
      return;
    }
    if (len == 2) {
      * (Bit16u*) data = ReadHostWordFromLittleEndian((Bit16u*) BX_MEM_THIS get_vector(a20addr));
      return;
    }
    if (len == 1) {
      * (Bit8u *) data = * (BX_MEM_THIS get_vector(a20addr));
      return;
    }
    // len == other case can just fall thru to special cases handling

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    // addr *not* in range 000A0000 .. 000FFFFF
    while(1) {
      // Read in chunks of 8 bytes if we can
      if ((len & 7) == 0) {
        *((Bit64u*)data_ptr) = ReadHostQWordFromLittleEndian((Bit64u*) BX_MEM_THIS get_vector(a20addr));
        len -= 8;
        a20addr += 8;
#ifdef BX_LITTLE_ENDIAN
        data_ptr += 8;
#else
        data_ptr -= 8;
#endif

        if (len == 0) return;
      } else {
        *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        if (len == 1) return;
        len--;
        a20addr++;
#ifdef BX_LITTLE_ENDIAN
        data_ptr++;
#else // BX_BIG_ENDIAN
        data_ptr--;
#endif
      }
    }
  }
  else  // access outside limits of physical memory
  {
    // bogus memory
    memset(data, 0xFF, len);
  }
}

#if BX_SUPPORT_MONITOR_MWAIT

//
// MONITOR/MWAIT - x86arch way to optimize idle loops in CPU
//

bool BX_MEMORY_STUB_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    if (BX_CPU(i)->is_monitor(begin_addr, len))
      return true;
  }

  return false; // this is NOT monitored page
}

void BX_MEMORY_STUB_C::check_monitor(bx_phy_address begin_addr, unsigned len)
{
  for (int i=0; i<BX_SMP_PROCESSORS;i++) {
    BX_CPU(i)->check_monitor(begin_addr, len);
  }
}

#endif
