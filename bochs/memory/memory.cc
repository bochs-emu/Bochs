//  Copyright (C) 2000  MandrakeSoft S.A.
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







#include "bochs.h"



#if BX_PROVIDE_CPU_MEMORY

  void
BX_MEM_C::write_physical(BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  Bit32u a20addr;


  a20addr = A20ADDR(addr);
  BX_INSTR_PHY_WRITE(a20addr, len);

#if BX_DEBUGGER
  // (mch) Check for physical write break points, TODO
  for (int i = 0; i < num_write_watchpoints; i++)
        if (write_watchpoint[i] == a20addr) {
              BX_CPU_THIS_PTR break_point = BREAK_POINT_WRITE;
              break;
        }
#endif


  if ( (a20addr + len) <= BX_MEM_THIS len ) {
    // all of data is within limits of physical memory
    if ( (a20addr & 0xfff80000) != 0x00080000 ) {
      if (len == 4) {
        if ((a20addr & 0x00000003) == 0) {
          // write 4byte data to aligned memory location
          Bit32u data32;

          data32 = * (Bit32u *) data;
#ifdef BX_BIG_ENDIAN
          data32 = (data32 << 24) | (data32 >> 24) |
            ((data32&0x00ff0000)>>8) | ((data32&0x0000ff00)<<8);
#endif
          * ((Bit32u *) (&vector[a20addr])) = data32;
          BX_DBG_DIRTY_PAGE(a20addr >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          return;
          }
        else {
          Bit32u data32;

          data32 = * (Bit32u *) data;
          * ((Bit8u *) (&vector[a20addr]))         = data32; data32 >>= 8;
          BX_DBG_DIRTY_PAGE(a20addr >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          * ((Bit8u *) (&vector[A20ADDR(addr+1)])) = data32; data32 >>= 8;
          * ((Bit8u *) (&vector[A20ADDR(addr+2)])) = data32; data32 >>= 8;
          * ((Bit8u *) (&vector[A20ADDR(addr+3)])) = data32;
          // worst case, last byte is in different page; possible extra dirty page
          BX_DBG_DIRTY_PAGE(A20ADDR(addr+3) >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          return;
          }
        }
      if (len == 2) {
        if ((a20addr & 0x00000001) == 0) {
          // write 2-byte data to aligned memory location
          Bit16u data16;

          data16 = * (Bit16u *) data;
#ifdef BX_BIG_ENDIAN
          data16 = (data16 >> 8) | (data16 << 8);
#endif
          * ((Bit16u *) (&vector[a20addr])) = data16;
          BX_DBG_DIRTY_PAGE(a20addr >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          return;
          }
        else {
          Bit16u data16;

          data16 = * (Bit16u *) data;
          * ((Bit8u *) (&vector[a20addr])) = (Bit8u) data16;
          BX_DBG_DIRTY_PAGE(a20addr >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          * ((Bit8u *) (&vector[A20ADDR(a20addr+1)])) = (data16 >> 8);
          BX_DBG_DIRTY_PAGE(A20ADDR(a20addr+1) >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          return;
          }
        }
      if (len == 1) {
        Bit8u data8;

        data8 = * (Bit8u *) data;
        * ((Bit8u *) (&vector[a20addr])) = data8;
        BX_DBG_DIRTY_PAGE(a20addr >> 12);
        BX_DYN_DIRTY_PAGE(a20addr >> 12);
        return;
        }
      // len == 3 case can just fall thru to special cases handling
      }

#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif

write_one:
    if ( (a20addr & 0xfff80000) != 0x00080000 ) {
      // addr *not* in range 00080000 .. 000FFFFF
      vector[a20addr] = *data_ptr;
      BX_DBG_DIRTY_PAGE(a20addr >> 12);
      BX_DYN_DIRTY_PAGE(a20addr >> 12);
inc_one:
      if (len == 1) return;
      len--;
      addr++;
      a20addr = A20ADDR(addr);
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
      goto write_one;
      }

    // addr in range 00080000 .. 000FFFFF

    if (a20addr <= 0x0009ffff) {
      // regular memory 80000 .. 9FFFF
      vector[a20addr] = *data_ptr;
      BX_DBG_DIRTY_PAGE(a20addr >> 12);
      BX_DYN_DIRTY_PAGE(a20addr >> 12);
      goto inc_one;
      }
    if (a20addr <= 0x000bffff) {
      // VGA memory A0000 .. BFFFF
      BX_VGA_MEM_WRITE(a20addr, *data_ptr);
      BX_DBG_DIRTY_PAGE(a20addr >> 12);
      BX_DYN_DIRTY_PAGE(a20addr >> 12);
      BX_DBG_UCMEM_REPORT(a20addr, 1, BX_WRITE, *data_ptr); // obsolete
      goto inc_one;
      }
    // adapter ROM     C0000 .. DFFFF
    // ROM BIOS memory E0000 .. FFFFF
    // (ignore write)
    //bx_printf("ROM lock %08x: len=%u\n",
    //  (unsigned) a20addr, (unsigned) len);
#if BX_PCI_SUPPORT == 0
#if BX_SHADOW_RAM
    // Write it since its in shadow RAM
    vector[a20addr] = *data_ptr;
    BX_DBG_DIRTY_PAGE(a20addr >> 12);
    BX_DYN_DIRTY_PAGE(a20addr >> 12);
#else
    // ignore write to ROM
#endif
#else
    // Write Based on 440fx Programming
    if (bx_options.i440FXSupport &&
        ((a20addr >= 0xC0000) && (a20addr <= 0xFFFFF))) {
      switch (bx_devices.pci->wr_memType(a20addr & 0xFC000)) {
        case 0x0:   // Writes to ShadowRAM
//        bx_printf ("Writing to ShadowRAM %08x, len %u ! \n", (unsigned) a20addr, (unsigned) len);
          vector[a20addr] = *data_ptr;
          BX_DBG_DIRTY_PAGE(a20addr >> 12);
          BX_DYN_DIRTY_PAGE(a20addr >> 12);
          goto inc_one;

        case 0x1:   // Writes to ROM, Inhibit
//        bx_devices.pci->s.i440fx.shadow[(a20addr - 0xc0000)] = *data_ptr;
//        bx_printf ("Writing to ROM %08x, Data %02x ! \n", (unsigned) a20addr, *data_ptr);
          goto inc_one;
        default:
          bx_panic("write_physical: default case\n");
          goto inc_one;
        }
      }
#endif
    goto inc_one;
    }

  else {
    // some or all of data is outside limits of physical memory
    unsigned i;

#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif

#if BX_APIC_SUPPORT
    bx_generic_apic_c *local_apic = &cpu->local_apic;
    bx_generic_apic_c *ioapic = bx_devices.ioapic;
    if (local_apic->is_selected (a20addr, len)) {
      local_apic->write (a20addr, (Bit32u *)data, len);
      return;
    } else if (ioapic->is_selected (a20addr, len)) {
      ioapic->write (a20addr, (Bit32u *)data, len);
      return;
    }
    else 
#endif
    for (i = 0; i < len; i++) {
      if (a20addr < BX_MEM_THIS len) {
        vector[a20addr] = *data_ptr;
        BX_DBG_DIRTY_PAGE(a20addr >> 12);
        BX_DYN_DIRTY_PAGE(a20addr >> 12);
        }
      // otherwise ignore byte, since it overruns memory
      addr++;
      a20addr = A20ADDR(addr);
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
      }
    return;
    }
}


  void
BX_MEM_C::read_physical(BX_CPU_C *cpu, Bit32u addr, unsigned len, void *data)
{
  Bit8u *data_ptr;
  Bit32u a20addr;


  a20addr = A20ADDR(addr);
  BX_INSTR_PHY_READ(a20addr, len);

#if BX_DEBUGGER
  // (mch) Check for physical read break points, TODO
  for (int i = 0; i < num_read_watchpoints; i++)
        if (read_watchpoint[i] == a20addr) {
              BX_CPU_THIS_PTR break_point = BREAK_POINT_READ;
              break;
        }
#endif

  if ( (a20addr + len) <= BX_MEM_THIS len ) {
    // all of data is within limits of physical memory
    if ( (a20addr & 0xfff80000) != 0x00080000 ) {
      if (len == 4) {
        if ((a20addr & 0x00000003) == 0) {
          // read 4-byte data from aligned memory location
          Bit32u data32;

          data32 = * ((Bit32u *) (&vector[a20addr]));
#ifdef BX_BIG_ENDIAN
          data32 = (data32 << 24) | (data32 >> 24) |
                   ((data32&0x00ff0000)>>8) | ((data32&0x0000ff00)<<8);
#endif
          * (Bit32u *) data = data32;
          return;
          }
        else {
          Bit32u data32;

          data32  = * ((Bit8u *) (&vector[A20ADDR(addr+3)])); data32 <<= 8;
          data32 |= * ((Bit8u *) (&vector[A20ADDR(addr+2)])); data32 <<= 8;
          data32 |= * ((Bit8u *) (&vector[A20ADDR(addr+1)])); data32 <<= 8;
          data32 |= * ((Bit8u *) (&vector[a20addr]));

          * (Bit32u *) data = data32;
          return;
          }
        }
      if (len == 2) {
        if ((a20addr & 0x00000001) == 0) {
          // read 2-byte data from aligned memory location
          Bit16u data16;

          data16 = * ((Bit16u *) (&vector[a20addr]));
#ifdef BX_BIG_ENDIAN
          data16 = (data16 >> 8) | (data16 << 8);
#endif

          * (Bit16u *) data =  data16;
          return;
          }
        else {
          Bit16u data16;

          data16  = * ((Bit8u *) (&vector[A20ADDR(addr+1)])); data16 <<= 8;
          data16 |= * ((Bit8u *) (&vector[a20addr]));

          * (Bit16u *) data = data16;
          return;
          }
        }
      if (len == 1) {
        Bit8u data8;

        data8 = * ((Bit8u *) (&vector[a20addr]));
        * (Bit8u *) data = data8;
        return;
        }
      // len == 3 case can just fall thru to special cases handling
      }


#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif



read_one:
    if ( (a20addr & 0xfff80000) != 0x00080000 ) {
      // addr *not* in range 00080000 .. 000FFFFF
      *data_ptr = vector[a20addr];
inc_one:
      if (len == 1) return;
      len--;
      addr++;
      a20addr = A20ADDR(addr);
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
      goto read_one;
      }

    // addr in range 00080000 .. 000FFFFF
#if BX_PCI_SUPPORT == 0
    if ((a20addr <= 0x0009ffff) || (a20addr >= 0x000c0000) ) {
      // regular memory 80000 .. 9FFFF, C0000 .. F0000
      *data_ptr = vector[a20addr];
      goto inc_one;
      }
    // VGA memory A0000 .. BFFFF
    *data_ptr = BX_VGA_MEM_READ(a20addr);
    BX_DBG_UCMEM_REPORT(a20addr, 1, BX_READ, *data_ptr); // obsolete
    goto inc_one;
#else   // #if BX_PCI_SUPPORT == 0
    if (a20addr <= 0x0009ffff) {
      *data_ptr = vector[a20addr];
      goto inc_one;
      }
    if (a20addr <= 0x000BFFFF) {
      // VGA memory A0000 .. BFFFF
      *data_ptr = BX_VGA_MEM_READ(a20addr);
      BX_DBG_UCMEM_REPORT(a20addr, 1, BX_READ, *data_ptr);
      goto inc_one;
      }

    // a20addr in C0000 .. FFFFF
    if (!bx_options.i440FXSupport) {
      *data_ptr = vector[a20addr];
      goto inc_one;
      }
    else {
      switch (bx_devices.pci->rd_memType(a20addr & 0xFC000)) {
        case 0x0:   // Read from ShadowRAM
          *data_ptr = vector[a20addr];
          bx_printf ("Reading from ShadowRAM %08x, Data %02x \n", (unsigned) a20addr, *data_ptr);
          goto inc_one;

        case 0x1:   // Read from ROM
          *data_ptr = bx_devices.pci->s.i440fx.shadow[(a20addr - 0xc0000)];
          //bx_printf ("Reading from ROM %08x, Data %02x  \n", (unsigned) a20addr, *data_ptr);
          goto inc_one;
        default:
          bx_panic("::read_physical: default case\n");
        }
      }
    goto inc_one;
#endif  // #if BX_PCI_SUPPORT == 0
    }
  else {
    // some or all of data is outside limits of physical memory
    unsigned i;

#ifdef BX_LITTLE_ENDIAN
    data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
    data_ptr = (Bit8u *) data + (len - 1);
#endif

#if BX_APIC_SUPPORT
    bx_generic_apic_c *local_apic = &cpu->local_apic;
    bx_generic_apic_c *ioapic = bx_devices.ioapic;
    if (local_apic->is_selected (addr, len)) {
      local_apic->read (addr, data, len);
      return;
    } else if (ioapic->is_selected (addr, len)) {
      ioapic->read (addr, data, len);
      return;
    }
#endif
    for (i = 0; i < len; i++) {
#if BX_PCI_SUPPORT == 0
      if (a20addr < BX_MEM_THIS len)
        *data_ptr = vector[a20addr];
      else
        *data_ptr = 0xff;
#else   // BX_PCI_SUPPORT == 0
      if (a20addr < BX_MEM_THIS len) {
        if ((a20addr >= 0x000C0000) && (a20addr <= 0x000FFFFF)) {
          if (!bx_options.i440FXSupport)
            *data_ptr = vector[a20addr];
          else {
            switch (bx_devices.pci->rd_memType(a20addr & 0xFC000)) {
              case 0x0:   // Read from ROM
                *data_ptr = vector[a20addr];
                //bx_printf ("Reading from ROM %08x, Data %02x \n", (unsigned) a20addr, *data_ptr);
                break;

              case 0x1:   // Read from Shadow RAM
                *data_ptr = bx_devices.pci->s.i440fx.shadow[(a20addr - 0xc0000)];
                bx_printf ("Reading from ShadowRAM %08x, Data %02x  \n", (unsigned) a20addr, *data_ptr);
                break;
              default:
                bx_panic("read_physical: default case\n");
              } // Switch
            }
          }
        else {
          *data_ptr = vector[a20addr];
          bx_printf ("Reading from Norm %08x, Data %02x  \n", (unsigned) a20addr, *data_ptr);
          }
        }
      else 
        *data_ptr = 0xff;
#endif  // BX_PCI_SUPPORT == 0
      addr++;
      a20addr = A20ADDR(addr);
#ifdef BX_LITTLE_ENDIAN
      data_ptr++;
#else // BX_BIG_ENDIAN
      data_ptr--;
#endif
      }
    return;
    }
}

#endif // #if BX_PROVIDE_CPU_MEMORY
