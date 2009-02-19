/////////////////////////////////////////////////////////////////////////
// $Id: apic.h,v 1.44 2009-02-19 23:19:11 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002 Zwane Mwaikambo, Stanislav Shwartsman
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
#  define BX_CPU_APIC_H 1

#if BX_CPU_LEVEL == 6 && BX_SUPPORT_SSE >= 2
  #define BX_IMPLEMENT_XAPIC 1
#endif

#if BX_SUPPORT_APIC

#define APIC_LEVEL_TRIGGERED	1
#define APIC_EDGE_TRIGGERED	0

#ifdef BX_INCLUDE_LOCAL_APIC

#ifdef BX_IMPLEMENT_XAPIC
#  define BX_LAPIC_VERSION_ID 0x00050014  // P4 has 6 LVT entries
#else
#  define BX_LAPIC_VERSION_ID 0x00040010  // P6 has 4 LVT entries
#endif

#define BX_LAPIC_BASE_ADDR  0xfee00000  // default Local APIC address
#define BX_NUM_LOCAL_APICS  BX_SMP_PROCESSORS
#define BX_LAPIC_MAX_INTS   256

#define BX_APIC_GLOBALLY_DISABLED 0
#define BX_APIC_STATE_INVALID     1
#define BX_APIC_XAPIC_MODE        2
#define BX_APIC_X2APIC_MODE       3

class BOCHSAPI bx_local_apic_c : public logfunctions
{
  bx_phy_address base_addr;
  Bit32u id;
  unsigned mode;

  bx_bool software_enabled;
  Bit32u  spurious_vector;
  bx_bool focus_disable;

  Bit32u task_priority;         // Task priority (TPR)
  Bit32u ldr;                   // Logical destination (LDR)
  Bit32u dest_format;           // Destination format (DFR)

  // ISR=in-service register.  When an IRR bit is cleared, the corresponding
  // bit in ISR is set.
  Bit8u isr[BX_LAPIC_MAX_INTS];
  // TMR=trigger mode register.  Cleared for edge-triggered interrupts
  // and set for level-triggered interrupts. If set, local APIC must send
  // EOI message to all other APICs.
  Bit8u tmr[BX_LAPIC_MAX_INTS];
  // IRR=interrupt request register. When an interrupt is triggered by
  // the I/O APIC or another processor, it sets a bit in irr. The bit is
  // cleared when the interrupt is acknowledged by the processor.
  Bit8u irr[BX_LAPIC_MAX_INTS];

#define APIC_ERR_ILLEGAL_ADDR    0x80
#define APIC_ERR_RX_ILLEGAL_VEC  0x40
#define APIC_ERR_TX_ILLEGAL_VEC  0x20
#define APIC_ERR_RX_ACCEPT_ERR   0x08
#define APIC_ERR_TX_ACCEPT_ERR   0x04
#define APIC_ERR_RX_CHECKSUM     0x02
#define APIC_ERR_TX_CHECKSUM     0x01

  // Error status Register (ESR)
  Bit32u error_status, shadow_error_status;

  Bit32u icr_hi;                // Interrupt command register (ICR)
  Bit32u icr_lo;

#define APIC_LVT_ENTRIES 6
  Bit32u lvt[APIC_LVT_ENTRIES];
#define APIC_LVT_TIMER   0
#define APIC_LVT_THERMAL 1
#define APIC_LVT_PERFMON 2
#define APIC_LVT_LINT0   3
#define APIC_LVT_LINT1   4
#define APIC_LVT_ERROR   5

  Bit32u timer_initial;         // Initial timer count
  Bit32u timer_current;         // current timer count

  Bit32u timer_divconf;         // Timer divide configuration register
  Bit32u timer_divide_factor;

  // Internal timer state, not accessible from bus
  bx_bool timer_active;
  int timer_handle;
  Bit64u ticksInitial;

/* APIC delivery modes */
#define APIC_DM_FIXED	0
#define APIC_DM_LOWPRI	1
#define APIC_DM_SMI	2
/* RESERVED		3 */
#define APIC_DM_NMI	4
#define APIC_DM_INIT	5
#define APIC_DM_SIPI	6
#define APIC_DM_EXTINT	7

  BX_CPU_C *cpu;

public:
  bx_bool INTR;
  bx_local_apic_c(BX_CPU_C *cpu);
 ~bx_local_apic_c() { }
  void reset(unsigned type);
  void init(void);
  bx_phy_address get_base(void) const { return base_addr; }
  void set_base(bx_phy_address newbase);
  void set_id(Bit32u newid);
  Bit32u get_id() const { return id; }
  bx_bool is_selected(bx_phy_address addr);
  void read(bx_phy_address addr, void *data, unsigned len);
  void write(bx_phy_address addr, void *data, unsigned len);
  void write_aligned(bx_phy_address addr, Bit32u data);
  Bit32u read_aligned(bx_phy_address address);
  void startup_msg(Bit8u vector);
  // on local APIC, trigger means raise the CPU's INTR line. For now
  // I also have to raise pc_system.INTR but that should be replaced
  // with the cpu-specific INTR signals.
  void trigger_irq(Bit8u vector, unsigned trigger_mode, bx_bool bypass_irr_isr = 0);
  void untrigger_irq(Bit8u vector, unsigned trigger_mode);
  Bit8u acknowledge_int(void);  // only the local CPU should call this
  int highest_priority_int(Bit8u *array);
  void receive_EOI(Bit32u value);
  void send_ipi(void);
  void write_spurious_interrupt_register(Bit32u value);
  void service_local_apic(void);
  void print_status(void);
  bx_bool match_logical_addr (Bit8u address);
  bx_bool deliver(Bit8u vector, Bit8u delivery_mode, Bit8u trig_mode);
  Bit8u get_tpr(void) { return task_priority; }
  void  set_tpr(Bit8u tpr);
  Bit8u get_ppr(void);
  Bit8u get_apr(void);
  bx_bool is_focus(Bit8u vector);
  static void periodic_smf(void *);
  void periodic(void);
  void set_divide_configuration(Bit32u value);
  void set_initial_timer_count(Bit32u value);
  void register_state(bx_param_c *parent);
};

#endif /* BX_INCLUDE_LOCAL_APIC */

// For P6 and Pentium family processors the local APIC ID feild is 4 bits.
#ifdef BX_IMPLEMENT_XAPIC
  #define APIC_MAX_ID  0xff
  #define APIC_ID_MASK 0xff
#else
  #define APIC_MAX_ID  0x0f
  #define APIC_ID_MASK 0x0f
#endif

int apic_bus_deliver_lowest_priority(Bit8u vector, Bit8u dest, bx_bool trig_mode, bx_bool broadcast);
int apic_bus_deliver_interrupt(Bit8u vector, Bit8u dest, Bit8u delivery_mode, bx_bool logical_dest, bx_bool level, bx_bool trig_mode);
int apic_bus_broadcast_interrupt(Bit8u vector, Bit8u delivery_mode, bx_bool trig_mode, int exclude_cpu);

#endif // if BX_SUPPORT_APIC

#endif
