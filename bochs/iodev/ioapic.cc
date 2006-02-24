/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.cc,v 1.26 2006-02-24 09:46:10 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
#include <stdio.h>

#include "iodev.h"
#if BX_SUPPORT_APIC

class bx_ioapic_c bx_ioapic;
#define LOG_THIS  bx_ioapic.

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

bx_ioapic_c::bx_ioapic_c() 
  : bx_generic_apic_c()
{
  put("IOAP");
  settype(IOAPICLOG);
}

bx_ioapic_c::~bx_ioapic_c() {}

#define BX_IOAPIC_DEFAULT_ID (BX_SMP_PROCESSORS)

void bx_ioapic_c::init() 
{
  bx_generic_apic_c::init();
  BX_INFO(("initializing I/O APIC"));
  base_addr = 0xfec00000;
  set_id(BX_IOAPIC_DEFAULT_ID);
  ioregsel = 0;
  // all interrupts masked
  for (int i=0; i<BX_IOAPIC_NUM_PINS; i++) {
    ioredtbl[i].set_lo_part(0x00010000);
    ioredtbl[i].set_hi_part(0x00000000);
  }
  intin = 0;
  irr = 0;
}

void bx_ioapic_c::read_aligned(Bit32u address, Bit32u *data, unsigned len)
{
  BX_DEBUG(("I/O APIC read_aligned addr=%08x, len=%d", address, len));
  BX_ASSERT(len == 4);
  address &= 0xff;
  if (address == 0x00) {
    // select register
    *data = ioregsel;
    return;
  } else {
    if (address != 0x10)
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
      *data = (ioregsel&1) ? entry->get_hi_part() : entry->get_lo_part();
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
  } else {
    if (address != 0x10)
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
	  entry->set_hi_part(*value);
	else 
	  entry->set_lo_part(*value);
	char buf[1024];
	entry->sprintf_self(buf);
	BX_DEBUG(("IOAPIC: now entry[%d] is %s", index, buf));
	service_ioapic();
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
  // look in IRR and deliver any interrupts that are not masked.
  BX_DEBUG(("IOAPIC: servicing"));
  for (unsigned bit=0; bit < BX_IOAPIC_NUM_PINS; bit++) {
    Bit32u mask = 1<<bit;
    if (irr & mask) {
      bx_io_redirect_entry_t *entry = ioredtbl + bit;
      if (! entry->is_masked()) {
        // clear irr bit and deliver
        if (entry->delivery_mode() == 7) {
          BX_PANIC(("ExtINT not implemented yet"));
        }
        bx_bool done = apic_bus_deliver_interrupt(entry->vector(), entry->destination(), entry->delivery_mode(), entry->destination_mode(), entry->pin_polarity(), entry->trigger_mode());
        if (done) {
          if (! entry->trigger_mode())
            irr &= ~mask;
          entry->clear_delivery_status();
          stuck = 0;
        } else {
          entry->set_delivery_status();
          stuck++;
          if (stuck > 5)
            BX_INFO(("vector %#x stuck?", entry->vector()));
        }
      }
      else {
        BX_DEBUG(("service_ioapic(): INTIN%d is masked", bit));
      }
    }
  }
}

#endif /* if BX_SUPPORT_APIC */
