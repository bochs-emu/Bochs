/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM_THIS

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

void BX_MEM_C::writePhysicalPage(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;

  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("writePhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_THIS check_monitor(a20addr, len);
#endif

  bx_bool is_bios = (a20addr >= (bx_phy_address)~BIOS_MASK);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = 0;
#endif

  if (cpu != NULL) {
#if BX_SUPPORT_IODEBUG
    bx_devices.pluginIODebug->mem_write(cpu, a20addr, len, data);
#endif

    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
    {
      // SMRAM memory space
      if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
        goto mem_write;
    }
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr &&
          memory_handler->end >= a20addr &&
          memory_handler->write_handler(a20addr, len, data, memory_handler->param))
    {
      return;
    }
    memory_handler = memory_handler->next;
  }

mem_write:

  // all memory access fits in single 4K page
  if (a20addr < BX_MEM_THIS len && ! is_bios) {
    // all of data is within limits of physical memory
    if (a20addr < 0x000a0000 || a20addr >= 0x00100000)
    {
      if (len == 8) {
        pageWriteStampTable.decWriteStamp(a20addr, 8);
        WriteHostQWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit64u*)data);
        return;
      }
      if (len == 4) {
        pageWriteStampTable.decWriteStamp(a20addr, 4);
        WriteHostDWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit32u*)data);
        return;
      }
      if (len == 2) {
        pageWriteStampTable.decWriteStamp(a20addr, 2);
        WriteHostWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit16u*)data);
        return;
      }
      if (len == 1) {
        pageWriteStampTable.decWriteStamp(a20addr, 1);
        * (BX_MEM_THIS get_vector(a20addr)) = * (Bit8u *) data;
        return;
      }
      // len == other, just fall thru to special cases handling
    }

    pageWriteStampTable.decWriteStamp(a20addr);

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    if (a20addr < 0x000a0000 || a20addr >= 0x00100000)
    {
      // addr *not* in range 000A0000 .. 000FFFFF
      while(1) {
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

    // addr must be in range 000A0000 .. 000FFFFF

    for(unsigned i=0; i<len; i++) {

      // SMMRAM
      if (a20addr < 0x000c0000) {
        // devices are not allowed to access SMMRAM under VGA memory
        if (cpu) {
          *(BX_MEM_THIS get_vector(a20addr)) = *data_ptr;
        }
        goto inc_one;
      }

      // adapter ROM     C0000 .. DFFFF
      // ROM BIOS memory E0000 .. FFFFF
#if BX_SUPPORT_PCI == 0
      // ignore write to ROM
#else
      // Write Based on 440fx Programming
      if (BX_MEM_THIS pci_enabled && ((a20addr & 0xfffc0000) == 0x000c0000)) {
        unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
        if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
        if (BX_MEM_THIS memory_type[area][1] == 1) {
          // Writes to ShadowRAM
          BX_DEBUG(("Writing to ShadowRAM: address 0x" FMT_PHY_ADDRX ", data %02x", a20addr, *data_ptr));
          *(BX_MEM_THIS get_vector(a20addr)) = *data_ptr;
        } else {
          // Writes to ROM, Inhibit
          BX_DEBUG(("Write to ROM ignored: address 0x" FMT_PHY_ADDRX ", data %02x", a20addr, *data_ptr));
        }
      }
#endif

inc_one:
      a20addr++;
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif

    }
  } else if (BX_MEM_THIS bios_write_enabled && (a20addr >= (bx_phy_address)~BIOS_MASK)) {
    // volatile BIOS write support
#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif
    for (unsigned i = 0; i < len; i++) {
      BX_MEM_THIS rom[a20addr & BIOS_MASK] = *data_ptr;
      a20addr++;
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
    }
  } else {
    // access outside limits of physical memory, ignore
    BX_DEBUG(("Write outside the limits of physical memory (0x"FMT_PHY_ADDRX") (ignore)", a20addr));
  }
}

void BX_MEM_C::readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;

  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("readPhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

  bx_bool is_bios = (a20addr >= (bx_phy_address)~BIOS_MASK);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = 0;
#endif

  if (cpu != NULL) {
#if BX_SUPPORT_IODEBUG
    bx_devices.pluginIODebug->mem_read(cpu, a20addr, len, data);
#endif

    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
    {
      // SMRAM memory space
      if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
        goto mem_read;
    }
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr &&
          memory_handler->end >= a20addr &&
          memory_handler->read_handler(a20addr, len, data, memory_handler->param))
    {
      return;
    }
    memory_handler = memory_handler->next;
  }

mem_read:

  if (a20addr < BX_MEM_THIS len && ! is_bios) {
    // all of data is within limits of physical memory
    if (a20addr < 0x000a0000 || a20addr >= 0x00100000)
    {
      if (len == 8) {
        ReadHostQWordFromLittleEndian(BX_MEM_THIS get_vector(a20addr), * (Bit64u*) data);
        return;
      }
      if (len == 4) {
        ReadHostDWordFromLittleEndian(BX_MEM_THIS get_vector(a20addr), * (Bit32u*) data);
        return;
      }
      if (len == 2) {
        ReadHostWordFromLittleEndian(BX_MEM_THIS get_vector(a20addr), * (Bit16u*) data);
        return;
      }
      if (len == 1) {
        * (Bit8u *) data = * (BX_MEM_THIS get_vector(a20addr));
        return;
      }
      // len == other case can just fall thru to special cases handling
    }

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    if (a20addr < 0x000a0000 || a20addr >= 0x00100000)
    {
      // addr *not* in range 000A0000 .. 000FFFFF
      while(1) {
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

    // addr must be in range 000A0000 .. 000FFFFF

    for (unsigned i=0; i<len; i++) {

      // SMMRAM
      if (a20addr < 0x000c0000) {
        // devices are not allowed to access SMMRAM under VGA memory
        if (cpu) *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        goto inc_one;
      }

#if BX_SUPPORT_PCI
      if (BX_MEM_THIS pci_enabled && ((a20addr & 0xfffc0000) == 0x000c0000)) {
        unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
        if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
        if (BX_MEM_THIS memory_type[area][0] == 0) {
          // Read from ROM
          if ((a20addr & 0xfffe0000) == 0x000e0000) {
            // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
            *data_ptr = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
          } else {
            *data_ptr = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
          }
        } else {
          // Read from ShadowRAM
          *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        }
      }
      else
#endif  // #if BX_SUPPORT_PCI
      {
        if ((a20addr & 0xfffc0000) != 0x000c0000) {
          *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        }
        else if ((a20addr & 0xfffe0000) == 0x000e0000) {
          // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
          *data_ptr = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
        }
        else {
          *data_ptr = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
        }
      }

inc_one:
      a20addr++;
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif

    }
  }
  else  // access outside limits of physical memory
  {
#if BX_PHY_ADDRESS_LONG
    if (a20addr > BX_CONST64(0xffffffff)) {
      memset(data, 0xFF, len);
      return;
    }
#endif

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    if (a20addr >= (bx_phy_address)~BIOS_MASK) {
      for (unsigned i = 0; i < len; i++) {
        *data_ptr = BX_MEM_THIS rom[a20addr & BIOS_MASK];
         a20addr++;
#ifdef BX_LITTLE_ENDIAN
         data_ptr++;
#else // BX_BIG_ENDIAN
         data_ptr--;
#endif
      }
    }
    else {
      memset(data, 0xFF, len);
    }
  }
}

void BX_MEM_C::dmaReadPhysicalPage(bx_phy_address addr, unsigned len, Bit8u *data)
{
  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("dmaReadPhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

  Bit8u *memptr = getHostMemAddr(NULL, addr, BX_READ);
  if (memptr != NULL) {
    memcpy(data, memptr, len);
  }
  else {
    for (unsigned i=0;i < len; i++) {
      readPhysicalPage(NULL, addr+i, 1, &data[i]);
    }
  }
}

void BX_MEM_C::dmaWritePhysicalPage(bx_phy_address addr, unsigned len, Bit8u *data)
{
  // Note: accesses should always be contained within a single page
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("dmaWritePhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

  Bit8u *memptr = getHostMemAddr(NULL, addr, BX_WRITE);
  if (memptr != NULL) {
    pageWriteStampTable.decWriteStamp(addr);
    memcpy(memptr, data, len);
  }
  else {
    for (unsigned i=0;i < len; i++) {
      writePhysicalPage(NULL, addr+i, 1, &data[i]);
    }
  }
}
