/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.h,v 1.4 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
extern class bx_ioapic_c bx_ioapic;

#define BX_IOAPIC_VERSION_ID 0x00170011  // same version as 82093 IOAPIC
#define BX_IOAPIC_NUM_PINS 0x18

class bx_io_redirect_entry_t {
  Bit64u value;
public:
  Bit32u get_even_word () { return value & 0xffffffff; }
  Bit32u get_odd_word () { return (value>>32) & 0xffffffff; }
  void set_even_word (Bit32u even) {
    // keep high 32 bits of value, replace low 32
    value = ((value >> 32) << 32) | (even & 0xffffffff);
    parse_value ();
  }
  void set_odd_word (Bit32u odd) { 
    // keep low 32 bits of value, replace high 32
    value = (((Bit64u)odd & 0xffffffff) << 32) | (value & 0xffffffff);
    parse_value ();
  }
  void parse_value ();
  // parse_value sets the value and all the fields below.  Do not change
  // these fields except by calling parse_value.
  Bit8u dest, masked, trig_mode, remote_irr, polarity, delivery_status, dest_mode, delivery_mode, vector;
  void sprintf_self (char *buf);
};

class bx_ioapic_c : public bx_generic_apic_c {
  Bit32u ioregsel;    // selects between various registers
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
  void trigger_irq (unsigned num, unsigned from);
  void untrigger_irq (unsigned num, unsigned from);
  void service_ioapic ();
  virtual Boolean match_logical_addr (Bit8u address) { return false; }
  virtual Boolean is_local_apic () { return false; }
  virtual bx_apic_type_t get_type () { return APIC_TYPE_IOAPIC; }
};
