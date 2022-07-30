/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002-2017 Zwane Mwaikambo, Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_CPU_APIC_H
#define BX_CPU_APIC_H 1

#if BX_SUPPORT_APIC

enum {
  APIC_EDGE_TRIGGERED  = 0,
  APIC_LEVEL_TRIGGERED = 1
};

const bx_phy_address BX_LAPIC_BASE_ADDR = 0xfee00000;  // default Local APIC address

#define BX_NUM_LOCAL_APICS  BX_SMP_PROCESSORS

enum {
  BX_APIC_GLOBALLY_DISABLED = 0,
  BX_APIC_STATE_INVALID     = 1,
  BX_APIC_XAPIC_MODE        = 2,
  BX_APIC_X2APIC_MODE       = 3
};

const Bit32u BX_XAPIC_EXT_SUPPORT_IER  = (1 << 0);
const Bit32u BX_XAPIC_EXT_SUPPORT_SEOI = (1 << 1);

typedef Bit32u apic_dest_t; /* same definition in ioapic.h */

// local apic registers

enum {
  BX_LAPIC_ID                   = 0x020,
  BX_LAPIC_VERSION              = 0x030,
  BX_LAPIC_TPR                  = 0x080,
  BX_LAPIC_ARBITRATION_PRIORITY = 0x090,
  BX_LAPIC_PPR                  = 0x0A0,
  BX_LAPIC_EOI                  = 0x0B0,
  BX_LAPIC_RRD                  = 0x0C0,
  BX_LAPIC_LDR                  = 0x0D0,
  BX_LAPIC_DESTINATION_FORMAT   = 0x0E0,
  BX_LAPIC_SPURIOUS_VECTOR      = 0x0F0,
  BX_LAPIC_ISR1                 = 0x100,
  BX_LAPIC_ISR2                 = 0x110,
  BX_LAPIC_ISR3                 = 0x120,
  BX_LAPIC_ISR4                 = 0x130,
  BX_LAPIC_ISR5                 = 0x140,
  BX_LAPIC_ISR6                 = 0x150,
  BX_LAPIC_ISR7                 = 0x160,
  BX_LAPIC_ISR8                 = 0x170,
  BX_LAPIC_TMR1                 = 0x180,
  BX_LAPIC_TMR2                 = 0x190,
  BX_LAPIC_TMR3                 = 0x1A0,
  BX_LAPIC_TMR4                 = 0x1B0,
  BX_LAPIC_TMR5                 = 0x1C0,
  BX_LAPIC_TMR6                 = 0x1D0,
  BX_LAPIC_TMR7                 = 0x1E0,
  BX_LAPIC_TMR8                 = 0x1F0,
  BX_LAPIC_IRR1                 = 0x200,
  BX_LAPIC_IRR2                 = 0x210,
  BX_LAPIC_IRR3                 = 0x220,
  BX_LAPIC_IRR4                 = 0x230,
  BX_LAPIC_IRR5                 = 0x240,
  BX_LAPIC_IRR6                 = 0x250,
  BX_LAPIC_IRR7                 = 0x260,
  BX_LAPIC_IRR8                 = 0x270,
  BX_LAPIC_ESR                  = 0x280,
  BX_LAPIC_LVT_CMCI             = 0x2F0,
  BX_LAPIC_ICR_LO               = 0x300,
  BX_LAPIC_ICR_HI               = 0x310,
  BX_LAPIC_LVT_TIMER            = 0x320,
  BX_LAPIC_LVT_THERMAL          = 0x330,
  BX_LAPIC_LVT_PERFMON          = 0x340,
  BX_LAPIC_LVT_LINT0            = 0x350,
  BX_LAPIC_LVT_LINT1            = 0x360,
  BX_LAPIC_LVT_ERROR            = 0x370,
  BX_LAPIC_TIMER_INITIAL_COUNT  = 0x380,
  BX_LAPIC_TIMER_CURRENT_COUNT  = 0x390,
  BX_LAPIC_TIMER_DIVIDE_CFG     = 0x3E0,
  BX_LAPIC_SELF_IPI             = 0x3F0,

  // extended AMD
  BX_LAPIC_EXT_APIC_FEATURE     = 0x400,
  BX_LAPIC_EXT_APIC_CONTROL     = 0x410,
  BX_LAPIC_SPECIFIC_EOI         = 0x420,
  BX_LAPIC_IER1                 = 0x480,
  BX_LAPIC_IER2                 = 0x490,
  BX_LAPIC_IER3                 = 0x4A0,
  BX_LAPIC_IER4                 = 0x4B0,
  BX_LAPIC_IER5                 = 0x4C0,
  BX_LAPIC_IER6                 = 0x4D0,
  BX_LAPIC_IER7                 = 0x4E0,
  BX_LAPIC_IER8                 = 0x4F0
};

/* APIC delivery modes */
enum {
  APIC_DM_FIXED	   = 0,
  APIC_DM_LOWPRI   = 1,
  APIC_DM_SMI      = 2,
  APIC_DM_RESERVED = 3,
  APIC_DM_NMI      = 4,
  APIC_DM_INIT     = 5,
  APIC_DM_SIPI     = 6,
  APIC_DM_EXTINT   = 7
};

enum {
  APIC_LVT_TIMER   = 0,
  APIC_LVT_THERMAL = 1,
  APIC_LVT_PERFMON = 2,
  APIC_LVT_LINT0   = 3,
  APIC_LVT_LINT1   = 4,
  APIC_LVT_ERROR   = 5,
  APIC_LVT_CMCI    = 6,
  APIC_LVT_ENTRIES
};

class BOCHSAPI bx_local_apic_c : public logfunctions
{
  bx_phy_address base_addr;
  unsigned mode;
  bool xapic;
#if BX_CPU_LEVEL >= 6
  Bit32u xapic_ext;             // enabled extended XAPIC features
#endif
  Bit32u apic_id;               //  4 bit in legacy mode, 8 bit in XAPIC mode
                                // 32 bit in X2APIC mode
  Bit32u apic_version_id;

  bool software_enabled;
  Bit8u  spurious_vector;
  bool focus_disable;

  Bit32u task_priority;         // Task priority (TPR)
  Bit32u ldr;                   // Logical destination (LDR)
  Bit32u dest_format;           // Destination format (DFR)

  // ISR=in-service register. When an IRR bit is cleared, the corresponding
  // bit in ISR is set.
  Bit32u isr[8];
  // TMR=trigger mode register.  Cleared for edge-triggered interrupts
  // and set for level-triggered interrupts. If set, local APIC must send
  // EOI message to all other APICs.
  Bit32u tmr[8];
  // IRR=interrupt request register. When an interrupt is triggered by
  // the I/O APIC or another processor, it sets a bit in irr. The bit is
  // cleared when the interrupt is acknowledged by the processor.
  Bit32u irr[8];
#if BX_CPU_LEVEL >= 6
  // IER=interrupt enable register. Only vectors that are enabled in IER
  // participare in APIC's computation of highest priority pending interrupt.
  Bit32u ier[8];
#endif

  enum {
    APIC_ERR_ILLEGAL_ADDR       = 0x80,
    APIC_ERR_RX_ILLEGAL_VEC     = 0x40,
    APIC_ERR_TX_ILLEGAL_VEC     = 0x20,
    X2APIC_ERR_REDIRECTIBLE_IPI = 0x08,
    APIC_ERR_RX_ACCEPT_ERR      = 0x08,
    APIC_ERR_TX_ACCEPT_ERR      = 0x04,
    APIC_ERR_RX_CHECKSUM        = 0x02,
    APIC_ERR_TX_CHECKSUM        = 0x01
  };

  // Error status Register (ESR)
  Bit32u error_status, shadow_error_status;

  Bit32u icr_hi;                // Interrupt command register (ICR)
  Bit32u icr_lo;

  Bit32u lvt[APIC_LVT_ENTRIES];

  Bit32u timer_initial;         // Initial timer count (in order to reload periodic timer)
  Bit32u timer_current;         // Current timer count
  Bit64u ticksInitial;          // Timer value when it started to count, also holds TSC-Deadline value

  Bit32u timer_divconf;         // Timer divide configuration register
  Bit32u timer_divide_factor;

  // Internal timer state, not accessible from bus
  bool timer_active;
  int timer_handle;

#if BX_SUPPORT_VMX >= 2
  int vmx_timer_handle;
  Bit32u vmx_preemption_timer_value;
  Bit64u vmx_preemption_timer_initial;    //The value of system tick when set the timer (absolute value)
  Bit64u vmx_preemption_timer_fire;       //The value of system tick when fire the exception (absolute value)
  Bit32u vmx_preemption_timer_rate;       //rate stated in MSR_VMX_MISC
  bool vmx_timer_active;
#endif 

#if BX_SUPPORT_MONITOR_MWAIT
  int mwaitx_timer_handle;
  bool mwaitx_timer_active;
#endif

  BX_CPU_C *cpu;

  bool get_vector(Bit32u *reg, unsigned vector);
  void set_vector(Bit32u *reg, unsigned vector);
  void clear_vector(Bit32u *reg, unsigned vector);

public:
  bool INTR;
  bx_local_apic_c(BX_CPU_C *cpu, unsigned id);
 ~bx_local_apic_c() { }
  void reset(unsigned type);
  bx_phy_address get_base(void) const { return base_addr; }
  void set_base(bx_phy_address newbase);
  Bit32u get_id() const { return apic_id; }
  bool is_xapic() const { return xapic; }
  bool is_selected(bx_phy_address addr);
  void read(bx_phy_address addr, void *data, unsigned len);
  void write(bx_phy_address addr, void *data, unsigned len);
  void write_aligned(bx_phy_address addr, Bit32u data);
  Bit32u read_aligned(bx_phy_address address);
#if BX_CPU_LEVEL >= 6
  bool read_x2apic(unsigned index, Bit64u *msr);
  bool write_x2apic(unsigned index, Bit32u msr_hi, Bit32u msr_lo);
#endif
  // on local APIC, trigger means raise the CPU's INTR line. For now
  // I also have to raise pc_system.INTR but that should be replaced
  // with the cpu-specific INTR signals.
  void trigger_irq(Bit8u vector, unsigned trigger_mode, bool bypass_irr_isr = 0);
  void untrigger_irq(Bit8u vector, unsigned trigger_mode);
  Bit8u acknowledge_int(void);  // only the local CPU should call this
  int highest_priority_int(Bit32u *array);
  void receive_EOI(Bit32u value);
  void send_ipi(apic_dest_t dest, Bit32u lo_cmd);
  void write_spurious_interrupt_register(Bit32u value);
  void service_local_apic(void);
  void print_status(void);
  bool match_logical_addr(apic_dest_t address);
  bool deliver(Bit8u vector, Bit8u delivery_mode, Bit8u trig_mode);
  Bit8u get_tpr(void) { return task_priority; }
  void  set_tpr(Bit8u tpr);
  Bit8u get_ppr(void);
  Bit8u get_apr(void);
  bool is_focus(Bit8u vector);
  void set_lvt_entry(unsigned apic_reg, Bit32u val);

  static void periodic_smf(void *);
  void periodic(void);
  void set_divide_configuration(Bit32u value);
  void set_initial_timer_count(Bit32u value);
  Bit32u get_current_timer_count(void);

#if BX_CPU_LEVEL >= 6
  Bit64u get_tsc_deadline(void);
  void set_tsc_deadline(Bit64u value);
  void receive_SEOI(Bit8u vec);
  void enable_xapic_extensions(void);
#endif

  void startup_msg(Bit8u vector);
  void register_state(bx_param_c *parent);
#if BX_SUPPORT_VMX >= 2
  Bit32u read_vmx_preemption_timer(void);
  void set_vmx_preemption_timer(Bit32u value);
  void deactivate_vmx_preemption_timer(void);
  static void vmx_preemption_timer_expired(void *);
#endif  

#if BX_SUPPORT_MONITOR_MWAIT
  void set_mwaitx_timer(Bit32u value);
  void deactivate_mwaitx_timer(void);
  static void mwaitx_timer_expired(void *);
#endif
};

bool apic_bus_deliver_lowest_priority(Bit8u vector, apic_dest_t dest, bool trig_mode, bool broadcast);
BOCHSAPI_MSVCONLY bool apic_bus_deliver_interrupt(Bit8u vector, apic_dest_t dest, Bit8u delivery_mode, bool logical_dest, bool level, bool trig_mode);
bool apic_bus_broadcast_interrupt(Bit8u vector, Bit8u delivery_mode, bool trig_mode, int exclude_cpu);

BX_CPP_INLINE bool is_x2apic_msr_range(Bit32u index) { return index >= 0x800 && index <= 0x8FF; }

#endif // if BX_SUPPORT_APIC

#endif
