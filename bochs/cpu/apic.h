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


#ifndef BX_CPU_APIC_H
#  define BX_CPU_APIC_H 1

typedef enum {
  APIC_TYPE_NONE,
  APIC_TYPE_IOAPIC,
  APIC_TYPE_LOCAL_APIC
} bx_apic_type_t;

#define APIC_BASE_ADDR 0xfee00000 // default APIC address

#if BX_SUPPORT_APIC
class BOCHSAPI bx_generic_apic_c : public logfunctions {
protected:
  Bit32u base_addr;
  Bit8u id;
#define APIC_UNKNOWN_ID 0xff
#define APIC_VERSION_ID 0x00170011  // same version as 82093 IOAPIC
public:
  bx_generic_apic_c ();
  virtual ~bx_generic_apic_c ();
  virtual void init ();
  virtual void hwreset () { }
  Bit32u get_base (void) const { return base_addr; }
  void set_base (Bit32u newbase);
  void set_id (Bit8u newid);
  Bit8u get_id () const { return id; }
  static void reset_all_ids ();
  virtual char *get_name();
  bx_bool is_selected (Bit32u addr, Bit32u len);
  void read (Bit32u addr, void *data, unsigned len);
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void write(Bit32u address, Bit32u *value, unsigned len);
  virtual void startup_msg (Bit32u vector);
  // on local APIC, trigger means deliver to the CPU.
  // on I/O APIC, trigger means direct to another APIC according to table.
  virtual void trigger_irq (unsigned num, unsigned from);
  virtual void untrigger_irq (unsigned num, unsigned from);
  virtual Bit32u get_delivery_bitmask (Bit8u dest, Bit8u dest_mode);
  virtual bx_bool deliver (Bit8u destination, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode);
  virtual bx_bool match_logical_addr (Bit8u address);
  virtual bx_apic_type_t get_type ();
  virtual void set_arb_id (int newid);  // only implemented on local apics
};

class BOCHSAPI bx_local_apic_c : public bx_generic_apic_c {
#define BX_LOCAL_APIC_MAX_INTS 256
  // TMR=trigger mode register.  Cleared for edge-triggered interrupts
  // and set for level-triggered interrupts.  If set, local APIC must send
  // EOI message to all other APICs.  EOI's are not implemented.
  Bit8u tmr[BX_LOCAL_APIC_MAX_INTS];
  // IRR=interrupt request register.  When an interrupt is triggered by
  // the I/O APIC or another processor, it sets a bit in irr.  The bit is
  // cleared when the interrupt is acknowledged by the processor.
  Bit8u irr[BX_LOCAL_APIC_MAX_INTS];
  // ISR=in-service register.  When an IRR bit is cleared, the corresponding
  // bit in ISR is set.  The ISR bit is cleared when
  Bit8u isr[BX_LOCAL_APIC_MAX_INTS];
  Bit32u arb_id, arb_priority, task_priority, log_dest, dest_format, spurious_vec;
  Bit32u lvt[6];
#define APIC_LVT_TIMER   0
#define APIC_LVT_THERMAL 1
#define APIC_LVT_PERFORM 2
#define APIC_LVT_LINT0   3
#define APIC_LVT_LINT1   4
#define APIC_LVT_ERROR   5
  Bit32u timer_initial, timer_current, timer_divconf;
  bx_bool timer_active;  // internal state, not accessible from bus
  Bit32u timer_divide_counter, timer_divide_factor;
  Bit32u icr_high, icr_low;
  Bit32u err_status;
#define APIC_ERR_ILLEGAL_ADDR    0x80
#define APIC_ERR_RX_ILLEGAL_VEC  0x40
#define APIC_ERR_TX_ILLEGAL_VEC  0x20
#define APIC_ERR_RX_ACCEPT_ERR   0x08
#define APIC_ERR_TX_ACCEPT_ERR   0x04
#define APIC_ERR_RX_CHECKSUM     0x02
#define APIC_ERR_TX_CHECKSUM     0x01

  int timer_handle; // KPL
  Bit64u ticksInitial; // System ticks count when APIC timer is started.

public:
  bx_bool INTR;
  bx_local_apic_c(BX_CPU_C *mycpu);
  virtual ~bx_local_apic_c(void);
  BX_CPU_C *cpu;
  virtual void hwreset ();
  virtual void init ();
  BX_CPU_C *get_cpu (Bit8u id);
  void set_id (Bit8u newid);   // redefine to set cpu->name
  virtual char *get_name();
  virtual void write (Bit32u addr, Bit32u *data, unsigned len);
  virtual void read_aligned(Bit32u address, Bit32u *data, unsigned len);
  virtual void startup_msg (Bit32u vector);
  // on local APIC, trigger means raise the CPU's INTR line.  For now
  // I also have to raise pc_system.INTR but that should be replaced
  // with the cpu-specific INTR signals.
  virtual void trigger_irq (unsigned num, unsigned from);
  virtual void untrigger_irq (unsigned num, unsigned from);
  Bit8u acknowledge_int ();  // only the local CPU should call this
  int highest_priority_int (Bit8u *array);
  void service_local_apic ();
  void print_status ();
  virtual bx_bool match_logical_addr (Bit8u address);
  virtual bx_bool is_local_apic () const { return true; }
  virtual bx_apic_type_t get_type () { return APIC_TYPE_LOCAL_APIC; }
  virtual Bit32u get_delivery_bitmask (Bit8u dest, Bit8u dest_mode);
  virtual bx_bool deliver (Bit8u destination, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode);
  Bit8u get_ppr ();
  Bit8u get_apr ();
  static void periodic_smf(void *); // KPL
  void periodic(void); // KPL
  void set_divide_configuration (Bit32u value);
  virtual void update_msr_apicbase(Bit32u newaddr);
  virtual void set_arb_id (int newid);
  };

#define APIC_MAX_ID 16
extern bx_generic_apic_c *apic_index[APIC_MAX_ID];
#endif // if BX_SUPPORT_APIC

#endif
