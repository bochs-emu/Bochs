/////////////////////////////////////////////////////////////////////////
// $Id: memory.cc,v 1.70 2008-07-26 14:44:26 sshwarts Exp $
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
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM_THIS

#if BX_PROVIDE_CPU_MEMORY

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

  // Note: accesses should always be contained within a single page now
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("writePhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

#if BX_SUPPORT_MONITOR_MWAIT
  BX_MEM_THIS check_monitor(a20addr, len);
#endif

  if (cpu != NULL) {
#if BX_SUPPORT_IODEBUG
    bx_iodebug_c::mem_write(cpu, a20addr, len, data);
#endif

    BX_INSTR_PHY_WRITE(cpu->which_cpu(), a20addr, len);

#if BX_SUPPORT_APIC
    bx_generic_apic_c *local_apic = &cpu->local_apic;
    if (local_apic->is_selected(a20addr, len)) {
      local_apic->write(a20addr, (Bit32u *)data, len);
      return;
    }
#endif

    if ((a20addr & 0xfffe0000) == 0x000a0000 && (BX_MEM_THIS smram_available))
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

  // all memory access feets in single 4K page
  if (a20addr < BX_MEM_THIS len) {
#if BX_SUPPORT_ICACHE
    pageWriteStampTable.decWriteStamp(a20addr);
#endif
    // all of data is within limits of physical memory
    if ((a20addr & 0xfff80000) != 0x00080000 || (a20addr <= 0x0009ffff))
    {
      if (len == 8) {
        WriteHostQWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit64u*)data);
        return;
      }
      if (len == 4) {
        WriteHostDWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit32u*)data);
        return;
      }
      if (len == 2) {
        WriteHostWordToLittleEndian(BX_MEM_THIS get_vector(a20addr), *(Bit16u*)data);
        return;
      }
      if (len == 1) {
        * (BX_MEM_THIS get_vector(a20addr)) = * (Bit8u *) data;
        return;
      }
      // len == other, just fall thru to special cases handling
    }

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    if ((a20addr & 0xfff80000) != 0x00080000 || (a20addr <= 0x0009ffff))
    {
      while(1) {
        // addr *not* in range 000A0000 .. 000FFFFF
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
      if (a20addr <= 0x000bffff) {
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
      if (BX_MEM_THIS pci_enabled && ((a20addr & 0xfffc0000) == 0x000c0000))
      {
        switch (DEV_pci_wr_memtype(a20addr)) {
          case 0x1:   // Writes to ShadowRAM
            BX_DEBUG(("Writing to ShadowRAM: address 0x" FMT_PHY_ADDRX ", data %02x", a20addr, *data_ptr));
            *(BX_MEM_THIS get_vector(a20addr)) = *data_ptr;
            break;

          case 0x0:   // Writes to ROM, Inhibit
            BX_DEBUG(("Write to ROM ignored: address 0x" FMT_PHY_ADDRX ", data %02x", a20addr, *data_ptr));
            break;

          default:
            BX_PANIC(("writePhysicalPage: default case"));
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
  }
  else {
    // access outside limits of physical memory, ignore
    BX_DEBUG(("Write outside the limits of physical memory (0x"FMT_PHY_ADDRX") (ignore)", a20addr));
  }
}

void BX_MEM_C::readPhysicalPage(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;

  // Note: accesses should always be contained within a single page now
  if ((addr>>12) != ((addr+len-1)>>12)) {
    BX_PANIC(("readPhysicalPage: cross page access at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
  }

  if (cpu != NULL) {
#if BX_SUPPORT_IODEBUG
    bx_iodebug_c::mem_read(cpu, a20addr, len, data);
#endif

    BX_INSTR_PHY_READ(cpu->which_cpu(), a20addr, len);

#if BX_SUPPORT_APIC
    bx_generic_apic_c *local_apic = &cpu->local_apic;
    if (local_apic->is_selected (a20addr, len)) {
      local_apic->read(a20addr, data, len);
      return;
    }
#endif

    if ((a20addr & 0xfffe0000) == 0x000a0000 && (BX_MEM_THIS smram_available))
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

  if (a20addr <= BX_MEM_THIS len) {
    // all of data is within limits of physical memory
    if ((a20addr & 0xfff80000) != 0x00080000 || (a20addr <= 0x0009ffff))
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

    if ((a20addr & 0xfff80000) != 0x00080000 || (a20addr <= 0x0009ffff))
    {
      while(1) {
        // addr *not* in range 00080000 .. 000FFFFF
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
      if (a20addr <= 0x000bffff) {
        // devices are not allowed to access SMMRAM under VGA memory
        if (cpu) *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        goto inc_one;
      }

#if BX_SUPPORT_PCI
      if (BX_MEM_THIS pci_enabled && ((a20addr & 0xfffc0000) == 0x000c0000))
      {
        switch (DEV_pci_rd_memtype(a20addr)) {
          case 0x0:  // Read from ROM
            if ((a20addr & 0xfffe0000) == 0x000e0000) {
              *data_ptr = BX_MEM_THIS rom[a20addr & BIOS_MASK];
            }
            else {
              *data_ptr = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
            }
            break;
          case 0x1:  // Read from ShadowRAM
            *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
            break;
          default:
            BX_PANIC(("readPhysicalPage: default case"));
        }
      }
      else
#endif  // #if BX_SUPPORT_PCI
      {
        if ((a20addr & 0xfffc0000) != 0x000c0000) {
          *data_ptr = *(BX_MEM_THIS get_vector(a20addr));
        }
        else if ((a20addr & 0xfffe0000) == 0x000e0000) {
          *data_ptr = BX_MEM_THIS rom[a20addr & BIOS_MASK];
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
  else
  {  // access outside limits of physical memory

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

    for (unsigned i = 0; i < len; i++) {
      if (a20addr >= (bx_phy_address)~BIOS_MASK)
        *data_ptr = BX_MEM_THIS rom[a20addr & BIOS_MASK];
      else
        *data_ptr = 0xff;
      addr++;
      a20addr = (addr);
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
    }
  }
}

#endif // #if BX_PROVIDE_CPU_MEMORY
