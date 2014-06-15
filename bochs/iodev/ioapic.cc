/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_APIC

#include "ioapic.h"

#define LOG_THIS theIOAPIC->

bx_ioapic_c *theIOAPIC = NULL;

int CDECL libioapic_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theIOAPIC = new bx_ioapic_c();
  bx_devices.pluginIOAPIC = theIOAPIC;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theIOAPIC, BX_PLUGIN_IOAPIC);
  return(0); // Success
}

void CDECL libioapic_LTX_plugin_fini(void)
{
  bx_devices.pluginIOAPIC = &bx_devices.stubIOAPIC;
  delete theIOAPIC;
}

static bx_bool ioapic_read(bx_phy_address a20addr, unsigned len, void *data, void *param)
{
  if((a20addr & ~0x3) != ((a20addr+len-1) & ~0x3)) {
    BX_PANIC(("I/O APIC read at address 0x" FMT_PHY_ADDRX " spans 32-bit boundary !", a20addr));
    return 1;
  }
  Bit32u value = theIOAPIC->read_aligned(a20addr & ~0x3);
  if(len == 4) { // must be 32-bit aligned
    *((Bit32u *)data) = value;
    return 1;
  }
  // handle partial read, independent of endian-ness
  value >>= (a20addr&3)*8;
  if (len == 1)
    *((Bit8u *) data) = value & 0xff;
  else if (len == 2)
    *((Bit16u *)data) = value & 0xffff;
  else
    BX_PANIC(("Unsupported I/O APIC read at address 0x" FMT_PHY_ADDRX ", len=%d", a20addr, len));

  return 1;
}

static bx_bool ioapic_write(bx_phy_address a20addr, unsigned len, void *data, void *param)
{
  if(a20addr & 0xf) {
    BX_PANIC(("I/O apic write at unaligned address 0x" FMT_PHY_ADDRX, a20addr));
    return 1;
  }

  if (len == 4) {
    theIOAPIC->write_aligned(a20addr, *((Bit32u*) data));
  }
  else {
    if ((a20addr & 0xff) != 0)
      BX_PANIC(("I/O apic write with len=%d (should be 4) at address 0x" FMT_PHY_ADDRX, len, a20addr));

    if (len == 2)
      theIOAPIC->write_aligned(a20addr, (Bit32u) *((Bit16u*) data));
    else if (len == 1)
      theIOAPIC->write_aligned(a20addr, (Bit32u) *((Bit8u*) data));
    else
      BX_PANIC(("Unsupported I/O APIC write at address 0x" FMT_PHY_ADDRX ", len=%d", a20addr, len));
  }

  return 1;
}

void bx_io_redirect_entry_t::sprintf_self(char *buf)
{
  sprintf(buf, "dest=%02x, masked=%d, trig_mode=%d, remote_irr=%d, polarity=%d, delivery_status=%d, dest_mode=%d, delivery_mode=%d, vector=%02x",
     (unsigned) destination(),
     (unsigned) is_masked(),
     (unsigned) trigger_mode(),
     (unsigned) remote_irr(),
     (unsigned) pin_polarity(),
     (unsigned) delivery_status(),
     (unsigned) destination_mode(),
     (unsigned) delivery_mode(),
     (unsigned) vector());
}

void bx_io_redirect_entry_t::register_state(bx_param_c *parent)
{
  BXRS_HEX_PARAM_SIMPLE(parent, lo);
  BXRS_HEX_PARAM_SIMPLE(parent, hi);
}

#define BX_IOAPIC_BASE_ADDR  (0xfec00000)
#define BX_IOAPIC_DEFAULT_ID (BX_SMP_PROCESSORS)

bx_ioapic_c::bx_ioapic_c(): enabled(0), base_addr(BX_IOAPIC_BASE_ADDR)
{
  set_id(BX_IOAPIC_DEFAULT_ID);
  put("IOAPIC");
}

bx_ioapic_c::~bx_ioapic_c()
{
  SIM->get_bochs_root()->remove("ioapic");
  BX_DEBUG(("Exit"));
}

void bx_ioapic_c::init(void)
{
  BX_INFO(("initializing I/O APIC"));
  set_enabled(1, 0x0000);
  reset(BX_RESET_HARDWARE);
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("ioapic", this);
#endif
}

void bx_ioapic_c::reset(unsigned type)
{
  // all interrupts masked
  for (int i=0; i<BX_IOAPIC_NUM_PINS; i++) {
    ioredtbl[i].set_lo_part(0x00010000);
    ioredtbl[i].set_hi_part(0x00000000);
  }
  intin = 0;
  irr = 0;
  ioregsel = 0;
}

Bit32u bx_ioapic_c::read_aligned(bx_phy_address address)
{
  BX_DEBUG(("IOAPIC: read aligned addr=0x" FMT_PHY_ADDRX, address));
  address &= 0xff;
  if (address == 0x00) {
    // select register
    return ioregsel;
  } else {
    if (address != 0x10)
      BX_PANIC(("IOAPIC: read from unsupported address"));
  }

  Bit32u data = 0;

  // only reached when reading data register
  switch (ioregsel) {
  case 0x00:  // APIC ID, note this is 4bits, the upper 4 are reserved
    data = ((id & apic_id_mask) << 24);
    break;
  case 0x01:  // version
    data = BX_IOAPIC_VERSION_ID;
    break;
  case 0x02:
    BX_INFO(("IOAPIC: arbitration ID unsupported, returned 0"));
    break;
  default:
    int index = (ioregsel - 0x10) >> 1;
    if (index >= 0 && index < BX_IOAPIC_NUM_PINS) {
      bx_io_redirect_entry_t *entry = ioredtbl + index;
      data = (ioregsel&1) ? entry->get_hi_part() : entry->get_lo_part();
      break;
    }
    BX_PANIC(("IOAPIC: IOREGSEL points to undefined register %02x", ioregsel));
  }

  return data;
}

void bx_ioapic_c::write_aligned(bx_phy_address address, Bit32u value)
{
  BX_DEBUG(("IOAPIC: write aligned addr=%08x, data=%08x", (unsigned) address, value));
  address &= 0xff;
  if (address == 0x00)  {
    ioregsel = value;
    return;
  } else {
    if (address != 0x10)
      BX_PANIC(("IOAPIC: write to unsupported address"));
  }
  // only reached when writing data register
  switch (ioregsel) {
    case 0x00: // set APIC ID
      {
        Bit8u newid = (value >> 24) & apic_id_mask;
        BX_INFO(("IOAPIC: setting id to 0x%x", newid));
        set_id (newid);
        return;
      }
    case 0x01: // version
    case 0x02: // arbitration id
      BX_INFO(("IOAPIC: could not write, IOREGSEL=0x%02x", ioregsel));
      return;
    default:
      int index = (ioregsel - 0x10) >> 1;
      if (index >= 0 && index < BX_IOAPIC_NUM_PINS) {
        bx_io_redirect_entry_t *entry = ioredtbl + index;
        if (ioregsel&1)
          entry->set_hi_part(value);
        else
          entry->set_lo_part(value);
        char buf[1024];
        entry->sprintf_self(buf);
        BX_DEBUG(("IOAPIC: now entry[%d] is %s", index, buf));
        service_ioapic();
        return;
      }
      BX_PANIC(("IOAPIC: IOREGSEL points to undefined register %02x", ioregsel));
  }
}

void bx_ioapic_c::set_enabled(bx_bool _enabled, Bit16u base_offset)
{
  if (_enabled != enabled) {
    if (_enabled) {
      base_addr = BX_IOAPIC_BASE_ADDR | base_offset;
      DEV_register_memory_handlers(theIOAPIC,
        ioapic_read, ioapic_write, base_addr, base_addr + 0xfff);
    } else {
      DEV_unregister_memory_handlers(theIOAPIC, base_addr, base_addr + 0xfff);
    }
    enabled = _enabled;
  } else if (enabled && (base_offset != (base_addr & 0xffff))) {
      DEV_unregister_memory_handlers(theIOAPIC, base_addr, base_addr + 0xfff);
      base_addr = BX_IOAPIC_BASE_ADDR | base_offset;
      DEV_register_memory_handlers(theIOAPIC,
        ioapic_read, ioapic_write, base_addr, base_addr + 0xfff);
  }
  BX_INFO(("IOAPIC %sabled (base address = 0x%08x)", enabled?"en":"dis", (Bit32u)base_addr));
}

void bx_ioapic_c::set_irq_level(Bit8u int_in, bx_bool level)
{
  if (int_in == 0) { // timer connected to pin #2
    int_in = 2;
  }
  BX_DEBUG(("set_irq_level(): INTIN%d: level=%d", int_in, level));
  if (int_in < BX_IOAPIC_NUM_PINS) {
    Bit32u bit = 1<<int_in;
    if ((level<<int_in) != (intin & bit)) {
      bx_io_redirect_entry_t *entry = ioredtbl + int_in;
      if (entry->trigger_mode()) {
        // level triggered
        if (level) {
          intin |= bit;
          irr |= bit;
          service_ioapic();
        } else {
          intin &= ~bit;
          irr &= ~bit;
        }
      } else {
        // edge triggered
        if (level) {
          intin |= bit;
          irr |= bit;
          service_ioapic();
        } else {
          intin &= ~bit;
        }
      }
    }
  }
}

void bx_ioapic_c::receive_eoi(Bit8u vector)
{
  BX_DEBUG(("IOAPIC: received EOI for vector %d", vector));
}

void bx_ioapic_c::service_ioapic()
{
  static unsigned int stuck = 0;
  Bit8u vector = 0;
  // look in IRR and deliver any interrupts that are not masked.
  BX_DEBUG(("IOAPIC: servicing"));
  for (unsigned bit=0; bit < BX_IOAPIC_NUM_PINS; bit++) {
    Bit32u mask = 1<<bit;
    if (irr & mask) {
      bx_io_redirect_entry_t *entry = ioredtbl + bit;
      if (! entry->is_masked()) {
        // clear irr bit and deliver
        if (entry->delivery_mode() == 7) {
          vector = DEV_pic_iac();
        } else {
          vector = entry->vector();
        }
        bx_bool done = apic_bus_deliver_interrupt(vector, entry->destination(), entry->delivery_mode(), entry->destination_mode(), entry->pin_polarity(), entry->trigger_mode());
        if (done) {
          if (! entry->trigger_mode())
            irr &= ~mask;
          entry->clear_delivery_status();
          stuck = 0;
        } else {
          entry->set_delivery_status();
          stuck++;
          if (stuck > 5)
            BX_INFO(("vector %#x stuck?", vector));
        }
      }
      else {
        BX_DEBUG(("service_ioapic(): INTIN%d is masked", bit));
      }
    }
  }
}

void bx_ioapic_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "ioapic", "IOAPIC State");

  BXRS_HEX_PARAM_SIMPLE(list, ioregsel);
  BXRS_HEX_PARAM_SIMPLE(list, intin);
  BXRS_HEX_PARAM_SIMPLE(list, irr);

  bx_list_c *table = new bx_list_c(list, "ioredtbl");
  for (unsigned i=0; i<BX_IOAPIC_NUM_PINS; i++) {
    char name[6];
    sprintf(name, "0x%02x", i);
    bx_list_c *entry = new bx_list_c(table, name);
    ioredtbl[i].register_state(entry);
  }
}

#if BX_DEBUGGER
void bx_ioapic_c::debug_dump(int argc, char **argv)
{
  int i;
  char buf[1024];

  dbg_printf("82093AA I/O APIC\n\n");
  for (i = 0; i < BX_IOAPIC_NUM_PINS; i++) {
    bx_io_redirect_entry_t *entry = ioredtbl + i;
    entry->sprintf_self(buf);
    dbg_printf("entry[%d]: %s\n", i, buf);
  }
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif

#endif /* if BX_SUPPORT_APIC */
