/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.cc,v 1.22 2006-01-01 11:33:06 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
#include <stdio.h>

#include "iodev.h"
#if BX_SUPPORT_APIC

class bx_ioapic_c bx_ioapic;
#define LOG_THIS  bx_ioapic.

void bx_io_redirect_entry_t::parse_value ()
{
  dest = (Bit8u)((hi >> 24) & APIC_ID_MASK);
  masked = (Bit8u)((lo >> 16) & 1);
  trig_mode = (Bit8u)((lo >> 15) & 1);
  remote_irr = (Bit8u)((lo >> 14) & 1);
  polarity = (Bit8u)((lo >> 13) & 1);
  //delivery_status = (lo >> 12) & 1;
  delivery_status = 0;  // we'll change this later...
  dest_mode = (Bit8u)((lo >> 11) & 1);
  delivery_mode = (Bit8u)((lo >> 8) & 7);
  vector = (Bit8u)(lo & 0xff);
}

void bx_io_redirect_entry_t::sprintf_self (char *buf)
{
  sprintf (buf, "dest=%02x, masked=%d, trig_mode=%d, remote_irr=%d, polarity=%d, delivery_status=%d, dest_mode=%d, delivery_mode=%d, vector=%02x", dest, masked, trig_mode, remote_irr, polarity, delivery_status, dest_mode, delivery_mode, vector);
}

bx_ioapic_c::bx_ioapic_c () 
  : bx_generic_apic_c ()
{
  put("IOAP");
  settype(IOAPICLOG);
}

bx_ioapic_c::~bx_ioapic_c () {}

void bx_ioapic_c::init () 
{
  bx_generic_apic_c::init ();
  BX_INFO(("initializing I/O APIC"));
  base_addr = 0xfec00000;
  set_id(BX_IOAPIC_DEFAULT_ID);
  ioregsel = 0;
  // all interrupts masked
  for (int i=0; i<BX_IOAPIC_NUM_PINS; i++) {
    ioredtbl[i].set_even_word (0x00010000);
    ioredtbl[i].set_odd_word  (0x00000000);
  }
  irr = 0;
}

void bx_ioapic_c::reset (unsigned type) 
{
}

void bx_ioapic_c::read_aligned(Bit32u address, Bit32u *data, unsigned len)
{
  BX_DEBUG( ("I/O APIC read_aligned addr=%08x, len=%d", address, len));
  BX_ASSERT (len == 4);
  address &= 0xff;
  if (address == 0x00) {
    // select register
    *data = ioregsel;
    return;
  } else if (address != 0x10) {
      BX_PANIC(("IOAPIC: read from unsupported address"));
  }
  // only reached when reading data register
  switch (ioregsel) {
  case 0x00:  // APIC ID, note this is 4bits, the upper 4 are reserved
    *data = ((id & APIC_ID_MASK) << 24);
    return;
  case 0x01:  // version
    *data = BX_IOAPIC_VERSION_ID;
    return;
  case 0x02:
    BX_INFO(("IOAPIC: arbitration ID unsupported, returned 0"));
    *data = 0;
    return;
  default:
    int index = (ioregsel - 0x10) >> 1;
    if (index >= 0 && index < BX_IOAPIC_NUM_PINS) {
      bx_io_redirect_entry_t *entry = ioredtbl + index;
      *data = (ioregsel&1) ? entry->get_odd_word() : entry->get_even_word ();
      return;
    }
    BX_PANIC(("IOAPIC: IOREGSEL points to undefined register %02x", ioregsel));
  }
}

void bx_ioapic_c::write(Bit32u address, Bit32u *value, unsigned len)
{
  BX_DEBUG(("IOAPIC: write addr=%08x, data=%08x, len=%d", address, *value, len));
  address &= 0xff;
  if (address == 0x00)  {
    ioregsel = *value;
    return;
  } else if (address != 0x10) {
    BX_PANIC(("IOAPIC: write to unsupported address"));
  }
  // only reached when writing data register
  switch (ioregsel) {
    case 0x00: // set APIC ID
      {
	Bit8u newid = (*value >> 24) & APIC_ID_MASK;
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
	  entry->set_odd_word (*value);
	else 
	  entry->set_even_word (*value);
	char buf[1024];
	entry->sprintf_self (buf);
	BX_DEBUG(("IOAPIC: now entry[%d] is %s", index, buf));
	service_ioapic ();
	return;
      }
      BX_PANIC(("IOAPIC: IOREGSEL points to undefined register %02x", ioregsel));
  }
}

void bx_ioapic_c::set_irq_level(Bit8u int_in, bx_bool level)
{
  BX_DEBUG(("set_irq_level(): INTIN%d: level=%d", int_in, level));
  if (int_in < BX_IOAPIC_NUM_PINS) {
    Bit32u bit = 1<<int_in;
    bx_io_redirect_entry_t *entry = ioredtbl + int_in;
    entry->parse_value();
    if (entry->trig_mode) {
      // level triggered
      if (level) {
        irr |= bit;
        service_ioapic ();
      } else {
        irr &= ~bit;
      }
    } else {
      // edge triggered
      if (level) {
        irr |= bit;
        service_ioapic ();
      }
    }
  }
}

void bx_ioapic_c::service_ioapic ()
{
  static unsigned int stuck = 0;
  // look in IRR and deliver any interrupts that are not masked.
  BX_DEBUG(("IOAPIC: servicing"));
  for (unsigned bit=0; bit < BX_IOAPIC_NUM_PINS; bit++) {
    Bit32u mask = 1<<bit;
    if (irr & mask) {
      bx_io_redirect_entry_t *entry = ioredtbl + bit;
      entry->parse_value();
      if (! entry->masked) {
        // clear irr bit and deliver
        if (entry->delivery_mode == 7) {
          BX_PANIC(("ExtINT not implemented yet"));
        }
        bx_bool done = deliver (entry->dest, entry->dest_mode, entry->delivery_mode, entry->vector, entry->polarity, entry->trig_mode);
        if (done) {
          if (! entry->trig_mode)
            irr &= ~mask;
          entry->delivery_status = 0;
          stuck = 0;
        } else {
          entry->delivery_status = 1;
          stuck++;
          if (stuck > 5)
            BX_INFO(("vector %#x stuck?\n", entry->vector));
        }
      }
    }
  }
}

#endif /* if BX_SUPPORT_APIC */
