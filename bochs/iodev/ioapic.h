/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.h,v 1.15 2006-01-10 06:13:26 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

extern class bx_ioapic_c bx_ioapic;

#define BX_IOAPIC_NUM_PINS   (0x18)

// use the same version as 82093 IOAPIC (0x00170011)
#define BX_IOAPIC_VERSION_ID (((BX_IOAPIC_NUM_PINS - 1) << 16) | 0x11)

class bx_io_redirect_entry_t {
  Bit32u hi, lo;

public:
  bx_io_redirect_entry_t(): hi(0), lo(0x10000) {}

  Bit32u get_even_word () const { return lo; }
  Bit32u get_odd_word () const  { return hi; }
  void set_even_word (Bit32u even) {
    // keep high 32 bits of value, replace low 32
    lo = even; 
    parse_value ();
  }
  void set_odd_word (Bit32u odd) { 
    // keep low 32 bits of value, replace high 32
    hi = odd; 
    parse_value ();
  }
  void parse_value ();
  void sprintf_self (char *buf);
  // parse_value sets the value and all the fields below.  Do not change
  // these fields except by calling parse_value.
  Bit8u dest, masked, trig_mode, remote_irr, polarity, delivery_status, dest_mode, delivery_mode, vector;
};

class bx_ioapic_c : public bx_generic_apic_c {
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
  virtual void init ();
  virtual void reset (unsigned type);
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void write(Bit32u address, Bit32u *value, unsigned len);
  void set_irq_level(Bit8u int_in, bx_bool level);
  void receive_eoi(Bit8u vector);
  void service_ioapic ();
  virtual bx_apic_type_t get_type () { return APIC_TYPE_IOAPIC; }
};
