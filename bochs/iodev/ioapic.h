/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.h,v 1.16 2006-02-24 09:46:10 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

extern class bx_ioapic_c bx_ioapic;

#define BX_IOAPIC_NUM_PINS   (0x18)

// use the same version as 82093 IOAPIC (0x00170011)
#define BX_IOAPIC_VERSION_ID (((BX_IOAPIC_NUM_PINS - 1) << 16) | 0x11)

class bx_io_redirect_entry_t {
  Bit32u hi, lo;

public:
  bx_io_redirect_entry_t(): hi(0), lo(0x10000) {}

  Bit8u destination() const { return (Bit8u)((hi >> 24) & APIC_ID_MASK); }
  bx_bool is_masked() const { return (bx_bool)((lo >> 16) & 1); }
  Bit8u trigger_mode() const { return (Bit8u)((lo >> 15) & 1); }
  bx_bool remote_irr() const { return (bx_bool)((lo >> 14) & 1); }
  Bit8u pin_polarity() const { return (Bit8u)((lo >> 13) & 1); }
  bx_bool delivery_status() const { return (bx_bool)((lo >> 12) & 1); }
  Bit8u destination_mode() const { return (Bit8u)((lo >> 11) & 1); }
  Bit8u delivery_mode() const { return (Bit8u)((lo >> 8) & 7); }
  Bit8u vector() const { return (Bit8u)(lo & 0xff); }

  void set_delivery_status() { lo |= (1<<12); }
  void clear_delivery_status() { lo &= ~(1<<12); }
  void set_remote_irr() { lo |= (1<<14); }
  void clear_remote_irr() { lo &= ~(1<<14); }
  
  Bit32u get_lo_part () const { return lo; }
  Bit32u get_hi_part () const  { return hi; }
  void set_lo_part (Bit32u val_lo_part) {
    // keep high 32 bits of value, replace low 32, ignore R/O bits
    lo = val_lo_part & 0xffffafff;
  }
  void set_hi_part (Bit32u val_hi_part) { 
    // keep low 32 bits of value, replace high 32
    hi = val_hi_part;
  }
  void sprintf_self (char *buf);
};

class bx_ioapic_c : public bx_generic_apic_c
{
  Bit32u ioregsel;    // selects between various registers
  Bit32u intin;
  // interrupt request bitmask, not visible from the outside.  Bits in the
  // irr are set when trigger_irq is called, and cleared when the interrupt
  // is delivered to the processor.  If an interrupt is masked, the irr
  // will still be set but delivery will not occur until it is unmasked.
  // It's not clear if this is how the real device works.
  Bit32u irr;

public:
  bx_io_redirect_entry_t ioredtbl[BX_IOAPIC_NUM_PINS];  // table of redirections
  bx_ioapic_c ();
  ~bx_ioapic_c ();
  virtual void init();
  virtual void reset(unsigned type) {}
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void write(Bit32u address, Bit32u *value, unsigned len);
  void set_irq_level(Bit8u int_in, bx_bool level);
  void receive_eoi(Bit8u vector);
  void service_ioapic();
  virtual bx_apic_type_t get_type() { return APIC_TYPE_IOAPIC; }
};
