#ifndef __APIC_H__
#define __APIC_H__

class bx_apic_c {
public:

  bx_apic_c(void);
  ~bx_apic_c(void);
  void   init();
  Bit32u get_base (void);
  void set_base (Bit32u newbase);
  void write_handler (Bit32u addr, Bit32u *data, unsigned len);
  void read_handler (Bit32u addr, Bit32u *data, unsigned len);
private:
  Bit32u apic_base_msr;
  Bit8u id;
  Bit32u icr_high, icr_low;
  Bit32u err_status;
#define APIC_VERSION_ID 0x00170011  // same as 82093 IOAPIC
  };


#if BX_USE_APIC_SMF
extern bx_apic_c bx_apic;
#endif

#endif /* defined __APIC_H__ */
