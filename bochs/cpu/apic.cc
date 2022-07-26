/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002-2021 Zwane Mwaikambo, Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "scalar_arith.h"
#include "iodev/iodev.h"

#if BX_SUPPORT_APIC

extern bool simulate_xapic;

#define LOG_THIS this->

#define BX_CPU_APIC(i) (&(BX_CPU(i)->lapic))

const unsigned BX_LAPIC_FIRST_VECTOR = 0x10;
const unsigned BX_LAPIC_LAST_VECTOR  = 0xff;

///////////// APIC BUS /////////////

int apic_bus_deliver_interrupt(Bit8u vector, apic_dest_t dest, Bit8u delivery_mode, bool logical_dest, bool level, bool trig_mode)
{
  if(delivery_mode == APIC_DM_LOWPRI)
  {
     if(! logical_dest) {
       // I/O subsytem initiated interrupt with lowest priority delivery
       // which is not supported in physical destination mode
       return 0;
     }
     else {
       return apic_bus_deliver_lowest_priority(vector, dest, trig_mode, 0);
     }
  }

  // determine destination local apics and deliver
  if(! logical_dest) {
    // physical destination mode
    if((dest & apic_id_mask) == apic_id_mask) {
       return apic_bus_broadcast_interrupt(vector, delivery_mode, trig_mode, apic_id_mask);
    }
    else {
       // the destination is single agent
       for (unsigned i=0;i<BX_NUM_LOCAL_APICS;i++)
       {
         if(BX_CPU_APIC(i)->get_id() == dest) {
           BX_CPU_APIC(i)->deliver(vector, delivery_mode, trig_mode);
           return 1;
         }
       }

       return 0;
    }
  }
  else {
    // logical destination mode
    if(dest == 0) return 0;

    bool interrupt_delivered = false;

    for (int i=0; i<BX_NUM_LOCAL_APICS; i++) {
      if(BX_CPU_APIC(i)->match_logical_addr(dest)) {
        BX_CPU_APIC(i)->deliver(vector, delivery_mode, trig_mode);
        interrupt_delivered = true;
      }
    }

    return (int) interrupt_delivered;
  }
}

int apic_bus_deliver_lowest_priority(Bit8u vector, apic_dest_t dest, bool trig_mode, bool broadcast)
{
  int i;

  if (! BX_CPU_APIC(0)->is_xapic()) {
    // search for if focus processor exists
    for (i=0; i<BX_NUM_LOCAL_APICS; i++) {
      if(BX_CPU_APIC(i)->is_focus(vector)) {
        BX_CPU_APIC(i)->deliver(vector, APIC_DM_LOWPRI, trig_mode);
        return 1;
      }
    }
  }

  // focus processor not found, looking for lowest priority agent
  int lowest_priority_agent = -1, lowest_priority = 0x100, priority;

  for (i=0; i<BX_NUM_LOCAL_APICS; i++) {
    if(broadcast || BX_CPU_APIC(i)->match_logical_addr(dest)) {
      if (BX_CPU_APIC(i)->is_xapic())
        priority = BX_CPU_APIC(i)->get_tpr();
      else
        priority = BX_CPU_APIC(i)->get_apr();
      if(priority < lowest_priority) {
        lowest_priority = priority;
        lowest_priority_agent = i;
      }
    }
  }

  if(lowest_priority_agent >= 0)
  {
    BX_CPU_APIC(lowest_priority_agent)->deliver(vector, APIC_DM_LOWPRI, trig_mode);
    return 1;
  }

  return 0;
}

int apic_bus_broadcast_interrupt(Bit8u vector, Bit8u delivery_mode, bool trig_mode, int exclude_cpu)
{
  if(delivery_mode == APIC_DM_LOWPRI)
  {
    return apic_bus_deliver_lowest_priority(vector, 0 /* doesn't matter */, trig_mode, 1);
  }

  // deliver to all bus agents except 'exclude_cpu'
  for (int i=0; i<BX_NUM_LOCAL_APICS; i++) {
    if(i == exclude_cpu) continue;
    BX_CPU_APIC(i)->deliver(vector, delivery_mode, trig_mode);
  }

  return 1;
}

static void apic_bus_broadcast_eoi(Bit8u vector)
{
  DEV_ioapic_receive_eoi(vector);
}

#endif

// available even if APIC is not compiled in
BOCHSAPI_MSVCONLY void apic_bus_deliver_smi(void)
{
  BX_CPU(0)->deliver_SMI();
}

void apic_bus_broadcast_smi(void)
{
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++)
    BX_CPU(i)->deliver_SMI();
}

#if BX_SUPPORT_APIC

////////////////////////////////////

bx_local_apic_c::bx_local_apic_c(BX_CPU_C *mycpu, unsigned id)
  : base_addr(BX_LAPIC_BASE_ADDR), cpu(mycpu)
{
  apic_id = id;
#if BX_SUPPORT_SMP
  if (apic_id >= bx_cpu_count)
    BX_PANIC(("PANIC: invalid APIC_ID assigned %d (max = %d)", apic_id, bx_cpu_count));
#else
  if (apic_id != 0)
    BX_PANIC(("PANIC: invalid APIC_ID assigned %d", apic_id));
#endif

  char name[16], logname[16];
  sprintf(name, "APIC%x", apic_id);
  sprintf(logname, "apic%x", apic_id);
  put(logname, name);

  // Register a non-active timer for use when the timer is started.
  timer_handle = bx_pc_system.register_timer_ticks(this,
            bx_local_apic_c::periodic_smf, 0, 0, 0, "lapic");
  timer_active = 0;

#if BX_SUPPORT_VMX >= 2
  // Register a non-active timer for VMX preemption timer.
  vmx_timer_handle = bx_pc_system.register_timer_ticks(this,
            bx_local_apic_c::vmx_preemption_timer_expired, 0, 0, 0, "vmx_preemption");
  BX_DEBUG(("vmx_timer is = %d", vmx_timer_handle));
  vmx_preemption_timer_rate = VMX_MISC_PREEMPTION_TIMER_RATE;
  vmx_timer_active = 0;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  mwaitx_timer_handle = bx_pc_system.register_timer_ticks(this,
            bx_local_apic_c::mwaitx_timer_expired, 0, 0, 0, "mwaitx_timer");
  BX_DEBUG(("mwaitx_timer is = %d", mwaitx_timer_handle));
  mwaitx_timer_active = 0;
#endif

  xapic = simulate_xapic; // xAPIC or legacy APIC

  reset(BX_RESET_HARDWARE);
}

void bx_local_apic_c::reset(unsigned type)
{
  int i;

  // default address for a local APIC, can be moved
  base_addr = BX_LAPIC_BASE_ADDR;
  error_status = shadow_error_status = 0;
  ldr = 0;
  dest_format = 0xf;
  icr_hi = 0;
  icr_lo = 0;
  task_priority = 0;

  for(i=0; i<8; i++) {
    irr[i] = isr[i] = tmr[i] = 0;
#if BX_CPU_LEVEL >= 6
    ier[i] = 0xFFFFFFFF; // all interrupts are enabled
#endif
  }

  timer_divconf = 0;
  timer_divide_factor = 1;
  timer_initial = 0;
  timer_current = 0;
  ticksInitial = 0;

  if(timer_active) {
    bx_pc_system.deactivate_timer(timer_handle);
    timer_active = 0;
  }

#if BX_SUPPORT_VMX >= 2
  if(vmx_timer_active) {
    bx_pc_system.deactivate_timer(vmx_timer_handle);
    vmx_timer_active = 0;
  }
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  if(mwaitx_timer_active) {
    bx_pc_system.deactivate_timer(mwaitx_timer_handle);
    mwaitx_timer_active = 0;
  }
#endif

  for(i=0; i<APIC_LVT_ENTRIES; i++) {
    lvt[i] = 0x10000;	// all LVT are masked
  }

  // split spurious vector register to 3 fields
  spurious_vector = 0xff;
  software_enabled = 0;
  focus_disable = 0;

  mode = BX_APIC_XAPIC_MODE;

  if (xapic)
    apic_version_id = 0x00050014; // P4 has 6 LVT entries
  else 
    apic_version_id = 0x00030010; // P6 has 4 LVT entries

#if BX_CPU_LEVEL >= 6
  xapic_ext = 0;
#endif
}

#if BX_CPU_LEVEL >= 6
void bx_local_apic_c::enable_xapic_extensions(void)
{
  apic_version_id |= 0x80000000;
  xapic_ext = BX_XAPIC_EXT_SUPPORT_IER | BX_XAPIC_EXT_SUPPORT_SEOI;
}
#endif

void bx_local_apic_c::set_base(bx_phy_address newbase)
{
#if BX_CPU_LEVEL >= 6
  if (mode == BX_APIC_X2APIC_MODE)
    ldr = ((apic_id & 0xfffffff0) << 16) | (1 << (apic_id & 0xf));
#endif
  mode = (newbase >> 10) & 3;
  newbase &= ~((bx_phy_address) 0xfff);
  base_addr = newbase;
  BX_INFO(("allocate APIC id=%d (MMIO %s) to 0x" FMT_PHY_ADDRX,
    apic_id, (mode == BX_APIC_XAPIC_MODE) ? "enabled" : "disabled", newbase));

  if (mode == BX_APIC_GLOBALLY_DISABLED) {
    // if local apic becomes globally disabled reset some fields back to defaults
    write_spurious_interrupt_register(0xff);
  }
}

bool bx_local_apic_c::is_selected(bx_phy_address addr)
{
  if (mode != BX_APIC_XAPIC_MODE) return 0;

  if((addr & ~0xfff) == base_addr) {
    if((addr & 0xf) != 0)
      BX_INFO(("warning: misaligned APIC access. addr=0x" FMT_PHY_ADDRX, addr));
    return 1;
  }
  return 0;
}

void bx_local_apic_c::read(bx_phy_address addr, void *data, unsigned len)
{
  if((addr & ~0x3) != ((addr+len-1) & ~0x3)) {
    BX_PANIC(("APIC read at address 0x" FMT_PHY_ADDRX " spans 32-bit boundary !", addr));
    return;
  }
  Bit32u value = read_aligned(addr & ~0x3);
  if(len == 4) { // must be 32-bit aligned
    *((Bit32u *)data) = value;
    return;
  }
  // handle partial read, independent of endian-ness
  value >>= (addr&3)*8;
  if (len == 1)
    *((Bit8u *) data) = value & 0xff;
  else if (len == 2)
    *((Bit16u *)data) = value & 0xffff;
  else
    BX_PANIC(("Unsupported APIC read at address 0x" FMT_PHY_ADDRX ", len=%d", addr, len));
}

void bx_local_apic_c::write(bx_phy_address addr, void *data, unsigned len)
{
  if (len != 4) {
    BX_PANIC(("APIC write with len=%d (should be 4)", len));
    return;
  }

  if(addr & 0xf) {
    BX_PANIC(("APIC write at unaligned address 0x" FMT_PHY_ADDRX, addr));
    return;
  }

  write_aligned(addr, *((Bit32u*) data));
}

// APIC read: 4 byte read from 16-byte aligned APIC address
Bit32u bx_local_apic_c::read_aligned(bx_phy_address addr)
{
  BX_ASSERT((addr & 0xf) == 0);
  Bit32u data = 0;  // default value for unimplemented registers

  unsigned apic_reg = addr & 0xff0;
  BX_DEBUG(("LAPIC read from register 0x%04x", apic_reg));

#if BX_CPU_LEVEL >= 6
  if (apic_reg >= 0x400 && !cpu->is_cpu_extension_supported(BX_ISA_XAPIC_EXT))
    apic_reg = 0xffffffff; // choose some obviosly invalid register if extended xapic is not supported
#endif

  switch(apic_reg) {
  case BX_LAPIC_ID:      // local APIC id
    data = apic_id << 24; break;
  case BX_LAPIC_VERSION: // local APIC version
    data = apic_version_id; break;
  case BX_LAPIC_TPR:     // task priority
    data = task_priority & 0xff; break;
  case BX_LAPIC_ARBITRATION_PRIORITY:
    data = get_apr(); break;
  case BX_LAPIC_PPR:     // processor priority
    data = get_ppr(); break;
  case BX_LAPIC_EOI:     // EOI
    /*
     * Read-modify-write operations should operate without generating
     * exceptions, and are used by some operating systems to EOI.
     * The results of reads should be ignored by the OS.
     */
    break;
  case BX_LAPIC_LDR:     // logical destination
    data = (ldr & apic_id_mask) << 24;
    break;
  case BX_LAPIC_DESTINATION_FORMAT:
    data = ((dest_format & 0xf) << 28) | 0x0fffffff;
    break;
  case BX_LAPIC_SPURIOUS_VECTOR:
    {
      Bit32u reg = spurious_vector;
      if(software_enabled) reg |= 0x100;
      if(focus_disable) reg |= 0x200;
      data = reg;
    }
    break;
  case BX_LAPIC_ISR1: case BX_LAPIC_ISR2:
  case BX_LAPIC_ISR3: case BX_LAPIC_ISR4:
  case BX_LAPIC_ISR5: case BX_LAPIC_ISR6:
  case BX_LAPIC_ISR7: case BX_LAPIC_ISR8:
    {
      int index = (apic_reg - BX_LAPIC_ISR1) >> 4;
      data = isr[index];
      break;
    }
  case BX_LAPIC_TMR1: case BX_LAPIC_TMR2:
  case BX_LAPIC_TMR3: case BX_LAPIC_TMR4:
  case BX_LAPIC_TMR5: case BX_LAPIC_TMR6:
  case BX_LAPIC_TMR7: case BX_LAPIC_TMR8:
    {
      int index = (apic_reg - BX_LAPIC_TMR1) >> 4;
      data = tmr[index];
      break;
    }
  case BX_LAPIC_IRR1: case BX_LAPIC_IRR2:
  case BX_LAPIC_IRR3: case BX_LAPIC_IRR4:
  case BX_LAPIC_IRR5: case BX_LAPIC_IRR6:
  case BX_LAPIC_IRR7: case BX_LAPIC_IRR8:
    {
      int index = (apic_reg - BX_LAPIC_IRR1) >> 4;
      data = irr[index];
      break;
    }
  case BX_LAPIC_ESR:     // error status reg
    data = error_status; break;
  case BX_LAPIC_ICR_LO:  // interrupt command reg  0-31
    data = icr_lo; break;
  case BX_LAPIC_ICR_HI:  // interrupt command reg 31-63
    data = icr_hi; break;
  case BX_LAPIC_LVT_TIMER:   // LVT Timer Reg
  case BX_LAPIC_LVT_THERMAL: // LVT Thermal Monitor
  case BX_LAPIC_LVT_PERFMON: // LVT Performance Counter
  case BX_LAPIC_LVT_LINT0:   // LVT LINT0 Reg
  case BX_LAPIC_LVT_LINT1:   // LVT Lint1 Reg
  case BX_LAPIC_LVT_ERROR:   // LVT Error Reg
    {
      int index = (apic_reg - BX_LAPIC_LVT_TIMER) >> 4;
      data = lvt[index];
      break;
    }
  case BX_LAPIC_LVT_CMCI:
    data = lvt[APIC_LVT_CMCI];
    break;
  case BX_LAPIC_TIMER_INITIAL_COUNT: // initial count for timer
    data = timer_initial;
    break;
  case BX_LAPIC_TIMER_CURRENT_COUNT: // current count for timer
    data = get_current_timer_count();
    break;
  case BX_LAPIC_TIMER_DIVIDE_CFG:   // timer divide configuration
    data = timer_divconf;
    break;
#if BX_CPU_LEVEL >= 6
  case BX_LAPIC_EXT_APIC_FEATURE:
    /* report extended xAPIC capabilities */
    data = BX_XAPIC_EXT_SUPPORT_IER | BX_XAPIC_EXT_SUPPORT_SEOI;
    break;
  case BX_LAPIC_EXT_APIC_CONTROL:
    /* report enabled extended xAPIC capabilities */
    data = xapic_ext;
    break;
  case BX_LAPIC_SPECIFIC_EOI:
    /*
     * Read-modify-write operations should operate without generating
     * exceptions, and are used by some operating systems to EOI.
     * The results of reads should be ignored by the OS.
     */
    break;
  case BX_LAPIC_IER1: case BX_LAPIC_IER2:
  case BX_LAPIC_IER3: case BX_LAPIC_IER4:
  case BX_LAPIC_IER5: case BX_LAPIC_IER6:
  case BX_LAPIC_IER7: case BX_LAPIC_IER8:
    {
      int index = (apic_reg - BX_LAPIC_IER1) >> 4;
      data = ier[index];
      break;
    }
#endif
  default:
      shadow_error_status |= APIC_ERR_ILLEGAL_ADDR;
      // but for now I want to know about it in case I missed some.
      BX_ERROR(("APIC read: register %x not implemented", apic_reg));
  }

  BX_DEBUG(("read from APIC address 0x" FMT_PHY_ADDRX " = %08x", addr, data));
  return data;
}

// APIC write: 4 byte write to 16-byte aligned APIC address
void bx_local_apic_c::write_aligned(bx_phy_address addr, Bit32u value)
{
  BX_ASSERT((addr & 0xf) == 0);

  unsigned apic_reg = addr & 0xff0;
  BX_DEBUG(("LAPIC write 0x%08x to register 0x%04x", value, apic_reg));

#if BX_CPU_LEVEL >= 6
  if (apic_reg >= 0x400 && !cpu->is_cpu_extension_supported(BX_ISA_XAPIC_EXT))
    apic_reg = 0xffffffff; // choose some obviosly invalid register if extended xapic is not supported
#endif

  switch(apic_reg) {
    case BX_LAPIC_TPR: // task priority
      set_tpr(value & 0xff);
      break;
    case BX_LAPIC_EOI: // EOI
      receive_EOI(value);
      break;
    case BX_LAPIC_LDR: // logical destination
      ldr = (value >> 24) & apic_id_mask;
      BX_DEBUG(("set logical destination to %08x", ldr));
      break;
    case BX_LAPIC_DESTINATION_FORMAT:
      dest_format = (value >> 28) & 0xf;
      BX_DEBUG(("set destination format to %02x", dest_format));
      break;
    case BX_LAPIC_SPURIOUS_VECTOR:
      write_spurious_interrupt_register(value);
      break;
    case BX_LAPIC_ESR: // error status reg
      // Here's what the IA-devguide-3 says on p.7-45:
      // The ESR is a read/write register and is reset after being written to
      // by the processor. A write to the ESR must be done just prior to
      // reading the ESR to allow the register to be updated.
      error_status = shadow_error_status;
      shadow_error_status = 0;
      break;
    case BX_LAPIC_ICR_LO: // interrupt command reg 0-31
      icr_lo = value & ~(1<<12);  // force delivery status bit = 0(idle)
      send_ipi((icr_hi >> 24) & 0xff, icr_lo);
      break;
    case BX_LAPIC_ICR_HI: // interrupt command reg 31-63
      icr_hi = value & 0xff000000;
      break;
    case BX_LAPIC_LVT_TIMER:   // LVT Timer Reg
    case BX_LAPIC_LVT_THERMAL: // LVT Thermal Monitor
    case BX_LAPIC_LVT_PERFMON: // LVT Performance Counter
    case BX_LAPIC_LVT_LINT0:   // LVT LINT0 Reg
    case BX_LAPIC_LVT_LINT1:   // LVT LINT1 Reg
    case BX_LAPIC_LVT_ERROR:   // LVT Error Reg
    case BX_LAPIC_LVT_CMCI:
      set_lvt_entry(apic_reg, value);
      break;
    case BX_LAPIC_TIMER_INITIAL_COUNT:
      set_initial_timer_count(value);
      break;
    case BX_LAPIC_TIMER_DIVIDE_CFG:
      // only bits 3, 1, and 0 are writable
      timer_divconf = value & 0xb;
      set_divide_configuration(timer_divconf);
      break;
    /* all read-only registers go here */
    case BX_LAPIC_ID:      // local APIC id
    case BX_LAPIC_VERSION: // local APIC version
    case BX_LAPIC_ARBITRATION_PRIORITY:
    case BX_LAPIC_RRD:
    case BX_LAPIC_PPR:     // processor priority
    // ISRs not writable
    case BX_LAPIC_ISR1: case BX_LAPIC_ISR2:
    case BX_LAPIC_ISR3: case BX_LAPIC_ISR4:
    case BX_LAPIC_ISR5: case BX_LAPIC_ISR6:
    case BX_LAPIC_ISR7: case BX_LAPIC_ISR8:
    // TMRs not writable
    case BX_LAPIC_TMR1: case BX_LAPIC_TMR2:
    case BX_LAPIC_TMR3: case BX_LAPIC_TMR4:
    case BX_LAPIC_TMR5: case BX_LAPIC_TMR6:
    case BX_LAPIC_TMR7: case BX_LAPIC_TMR8:
    // IRRs not writable
    case BX_LAPIC_IRR1: case BX_LAPIC_IRR2:
    case BX_LAPIC_IRR3: case BX_LAPIC_IRR4:
    case BX_LAPIC_IRR5: case BX_LAPIC_IRR6:
    case BX_LAPIC_IRR7: case BX_LAPIC_IRR8:
    // current count for timer
    case BX_LAPIC_TIMER_CURRENT_COUNT:
      // all read-only registers should fall into this line
      BX_INFO(("warning: write to read-only APIC register 0x%x", apic_reg));
      break;
#if BX_CPU_LEVEL >= 6
    case BX_LAPIC_EXT_APIC_FEATURE:
      // all read-only registers should fall into this line
      BX_INFO(("warning: write to read-only APIC register 0x%x", apic_reg));
      break;
    case BX_LAPIC_EXT_APIC_CONTROL:
      /* set extended xAPIC capabilities */
      xapic_ext = value & (BX_XAPIC_EXT_SUPPORT_IER | BX_XAPIC_EXT_SUPPORT_SEOI);
      break;
    case BX_LAPIC_SPECIFIC_EOI:
      receive_SEOI(value & 0xff);
      break;
    case BX_LAPIC_IER1: case BX_LAPIC_IER2:
    case BX_LAPIC_IER3: case BX_LAPIC_IER4:
    case BX_LAPIC_IER5: case BX_LAPIC_IER6:
    case BX_LAPIC_IER7: case BX_LAPIC_IER8:
      {
        if ((xapic_ext & BX_XAPIC_EXT_SUPPORT_IER) == 0) {
          BX_ERROR(("IER writes are currently disabled reg %x", apic_reg));
          break;
        }

        int index = (apic_reg - BX_LAPIC_IER1) >> 4;
        ier[index] = value;
      }
      break;
#endif
    default:
      shadow_error_status |= APIC_ERR_ILLEGAL_ADDR;
      // but for now I want to know about it in case I missed some.
      BX_ERROR(("APIC write: register %x not implemented", apic_reg));
  }
}

void bx_local_apic_c::set_lvt_entry(unsigned apic_reg, Bit32u value)
{
  static Bit32u lvt_mask[] = {
    0x000710ff, /* TIMER */
    0x000117ff, /* THERMAL */
    0x000117ff, /* PERFMON */
    0x0001f7ff, /* LINT0 */
    0x0001f7ff, /* LINT1 */
    0x000110ff, /* ERROR */
    0x000117ff, /* CMCI */
  };

  unsigned lvt_entry = (apic_reg == BX_LAPIC_LVT_CMCI) ? APIC_LVT_CMCI : (apic_reg - BX_LAPIC_LVT_TIMER) >> 4;
#if BX_CPU_LEVEL >= 6
  if (apic_reg == BX_LAPIC_LVT_TIMER) {
    if (! cpu->is_cpu_extension_supported(BX_ISA_TSC_DEADLINE)) {
      value &= ~0x40000; // cannot enable TSC-Deadline when not supported
    }
    else {
      if ((value ^ lvt[lvt_entry]) & 0x40000) {
         // Transition between TSC-Deadline and other timer modes disarm the timer
         if(timer_active) {
            bx_pc_system.deactivate_timer(timer_handle);
            timer_active = 0;
         }
      }
    }
  }
#endif

  lvt[lvt_entry] = value & lvt_mask[lvt_entry];
  if(! software_enabled) {
    lvt[lvt_entry] |= 0x10000;
  }
}

void bx_local_apic_c::send_ipi(apic_dest_t dest, Bit32u lo_cmd)
{
  int dest_shorthand = (lo_cmd >> 18) & 3;
  int trig_mode = (lo_cmd >> 15) & 1;
  int level = (lo_cmd >> 14) & 1;
  int logical_dest = (lo_cmd >> 11) & 1;
  int delivery_mode = (lo_cmd >> 8) & 7;
  int vector = (lo_cmd & 0xff);
  int accepted = 0;

  if(delivery_mode == APIC_DM_INIT)
  {
    if(level == 0 && trig_mode == 1) {
      // special mode in local apic.  See "INIT Level Deassert" in the
      // Intel Soft. Devel. Guide Vol 3, page 7-34.  This magic code
      // causes all APICs(regardless of dest address) to set their
      // arbitration ID to their APIC ID. Not supported by Pentium 4
      // and Intel Xeon processors.
      return; // we not model APIC bus arbitration ID anyway
    }
  }

  switch(dest_shorthand) {
  case 0:  // no shorthand, use real destination value
    accepted = apic_bus_deliver_interrupt(vector, dest, delivery_mode, logical_dest, level, trig_mode);
    break;
  case 1:  // self
    trigger_irq(vector, trig_mode);
    accepted = 1;
    break;
  case 2:  // all including self
    accepted = apic_bus_broadcast_interrupt(vector, delivery_mode, trig_mode, apic_id_mask);
    break;
  case 3:  // all but self
    accepted = apic_bus_broadcast_interrupt(vector, delivery_mode, trig_mode, get_id());
    break;
  default:
    BX_PANIC(("Invalid desination shorthand %#x", dest_shorthand));
  }

  if(! accepted) {
    BX_DEBUG(("An IPI wasn't accepted, raise APIC_ERR_TX_ACCEPT_ERR"));
    shadow_error_status |= APIC_ERR_TX_ACCEPT_ERR;
  }
}

void bx_local_apic_c::write_spurious_interrupt_register(Bit32u value)
{
  BX_DEBUG(("write of %08x to spurious interrupt register", value));

  if (xapic)
    spurious_vector = value & 0xff;
  else
    // bits 0-3 of the spurious vector hardwired to '1
    spurious_vector = (value & 0xf0) | 0xf;

  software_enabled = (value >> 8) & 1;
  focus_disable    = (value >> 9) & 1;

  if(! software_enabled) {
    for(unsigned i=0; i<APIC_LVT_ENTRIES; i++) {
      lvt[i] |= 0x10000;	// all LVT are masked
    }
  }
}

void bx_local_apic_c::receive_EOI(Bit32u value)
{
  BX_DEBUG(("Wrote 0x%x to EOI", value));
  int vec = highest_priority_int(isr);
  if (vec < 0) {
    BX_DEBUG(("EOI written without any bit in ISR"));
  }
  else {
    if ((Bit32u) vec != spurious_vector) {
       BX_DEBUG(("local apic received EOI, hopefully for vector 0x%02x", vec));
       clear_vector(isr, vec);
       if(get_vector(tmr, vec)) {
           apic_bus_broadcast_eoi(vec);
           clear_vector(tmr, vec);
       }
       service_local_apic();
    }
  }

  if(bx_dbg.apic) print_status();
}

#if BX_CPU_LEVEL >= 6
void bx_local_apic_c::receive_SEOI(Bit8u vec)
{
  if ((xapic_ext & BX_XAPIC_EXT_SUPPORT_SEOI) == 0) {
     BX_ERROR(("SEOI functionality is disabled"));
     return;
  }

  if (get_vector(isr, vec)) {
     BX_DEBUG(("local apic received SEOI for vector 0x%02x", vec));
     clear_vector(isr, vec);
     if(get_vector(tmr, vec)) {
         apic_bus_broadcast_eoi(vec);
         clear_vector(tmr, vec);
     }
     service_local_apic();
  }

  if(bx_dbg.apic) print_status();
}
#endif

void bx_local_apic_c::startup_msg(Bit8u vector)
{
  cpu->deliver_SIPI(vector);
}

bool bx_local_apic_c::get_vector(Bit32u *reg, unsigned vector)
{
  return (reg[vector / 32] >> (vector % 32)) & 0x1;
}

void bx_local_apic_c::set_vector(Bit32u *reg, unsigned vector)
{
  reg[vector / 32] |= (1 << (vector % 32));
}

void bx_local_apic_c::clear_vector(Bit32u *reg, unsigned vector)
{
  reg[vector / 32] &= ~(1 << (vector % 32));
}

int bx_local_apic_c::highest_priority_int(Bit32u *array)
{
  for (int reg=7; reg>=0; reg--) {
    Bit32u tmp = array[reg];
#if BX_CPU_LEVEL >= 6
    tmp &= ier[reg];
#endif
// ignore of interrupt vectors < 16 happen naturally as there is no way to ISR to it
//  if (reg == 0) tmp &= 0xffff0000;
    if (tmp) {
      int vector = (reg * 32) + (31 - lzcntd(tmp));
      return vector;
    }
  }

  return -1;
}

void bx_local_apic_c::service_local_apic(void)
{
  if(bx_dbg.apic) {
    BX_INFO(("service_local_apic()"));
    print_status();
  }

  if(cpu->is_pending(BX_EVENT_PENDING_LAPIC_INTR)) return;  // INTR already up; do nothing

  // find first interrupt in irr.
  int first_irr = highest_priority_int(irr);
  if (first_irr < 0) return;   // no interrupts, leave INTR=0
  int first_isr = highest_priority_int(isr);
  if (first_isr >= 0 && first_irr <= first_isr) {
    BX_DEBUG(("lapic(%d): not delivering int 0x%02x because int 0x%02x is in service", apic_id, first_irr, first_isr));
    return;
  }
  if(((Bit32u)(first_irr) & 0xf0) <= (task_priority & 0xf0)) {
    BX_DEBUG(("lapic(%d): not delivering int 0x%02X because task_priority is 0x%02X", apic_id, first_irr, task_priority));
    return;
  }
  // interrupt has appeared in irr. Raise INTR. When the CPU
  // acknowledges, we will run highest_priority_int again and
  // return it.
  BX_DEBUG(("service_local_apic(): setting INTR=1 for vector 0x%02x", first_irr));
  cpu->signal_event(BX_EVENT_PENDING_LAPIC_INTR);
}

bool bx_local_apic_c::deliver(Bit8u vector, Bit8u delivery_mode, Bit8u trig_mode)
{
  switch(delivery_mode) {
  case APIC_DM_FIXED:
  case APIC_DM_LOWPRI:
    BX_DEBUG(("Deliver lowest priority of fixed interrupt vector %02x", vector));
    trigger_irq(vector, trig_mode);
    break;
  case APIC_DM_SMI:
    BX_INFO(("Deliver SMI"));
    cpu->deliver_SMI();
    return 1;
  case APIC_DM_NMI:
    BX_INFO(("Deliver NMI"));
    cpu->deliver_NMI();
    return 1;
  case APIC_DM_INIT:
    BX_INFO(("Deliver INIT IPI"));
    cpu->deliver_INIT();
    break;
  case APIC_DM_SIPI:
    BX_INFO(("Deliver Start Up IPI"));
    startup_msg(vector);
    break;
  case APIC_DM_EXTINT:
    BX_DEBUG(("Deliver EXTINT vector %02x", vector));
    trigger_irq(vector, trig_mode, 1);
    break;
  default:
    return 0;
  }

  return 1;
}

void bx_local_apic_c::trigger_irq(Bit8u vector, unsigned trigger_mode, bool bypass_irr_isr)
{
  BX_DEBUG(("trigger interrupt vector=0x%02x", vector));

  if(/* vector > BX_LAPIC_LAST_VECTOR || */ vector < BX_LAPIC_FIRST_VECTOR) {
    shadow_error_status |= APIC_ERR_RX_ILLEGAL_VEC;
    BX_INFO(("bogus vector %#x, ignoring ...", vector));
    return;
  }

  BX_DEBUG(("triggered vector %#02x", vector));

  if(! bypass_irr_isr) {
    if(get_vector(irr, vector)) {
      BX_DEBUG(("triggered vector %#02x not accepted", vector));
      return;
    }
  }

  set_vector(irr, vector);
  if (trigger_mode)
      set_vector(tmr, vector); // set for level triggered
  else
    clear_vector(tmr, vector);

  service_local_apic();
}

void bx_local_apic_c::untrigger_irq(Bit8u vector, unsigned trigger_mode)
{
  BX_DEBUG(("untrigger interrupt vector=0x%02x", vector));
  // hardware says "no more".  clear the bit.  If the CPU hasn't yet
  // acknowledged the interrupt, it will never be serviced.
  BX_ASSERT(get_vector(irr, vector));
  clear_vector(irr, vector);
  if(bx_dbg.apic) print_status();
}

Bit8u bx_local_apic_c::acknowledge_int(void)
{
  // CPU calls this when it is ready to service one interrupt
  if(! cpu->is_pending(BX_EVENT_PENDING_LAPIC_INTR))
    BX_PANIC(("APIC %d acknowledged an interrupt, but INTR=0", apic_id));

  int vector = highest_priority_int(irr);
  if (vector < 0 || (vector & 0xf0) <= get_ppr()) {
    cpu->clear_event(BX_EVENT_PENDING_LAPIC_INTR);
    return spurious_vector;
  }

  BX_ASSERT(get_vector(irr, vector));
  BX_DEBUG(("acknowledge_int() returning vector 0x%02x", vector));
  clear_vector(irr, vector);
  set_vector(isr, vector);
  if(bx_dbg.apic) {
    BX_INFO(("Status after setting isr:"));
    print_status();
  }

  cpu->clear_event(BX_EVENT_PENDING_LAPIC_INTR);
  service_local_apic();  // will set INTR again if another is ready
  return vector;
}

void bx_local_apic_c::print_status(void)
{
  BX_INFO(("lapic %d: status is {:", apic_id));
  for(unsigned vec=0; vec<=BX_LAPIC_LAST_VECTOR; vec++) {
    if(get_vector(irr, vec) || get_vector(isr, vec)) {
      BX_INFO(("vec: %u, irr=%d, isr=%d", vec, get_vector(irr, vec), get_vector(isr, vec)));
    }
  }
  BX_INFO(("}"));
}

bool bx_local_apic_c::match_logical_addr(apic_dest_t address)
{
  bool match = false;

#if BX_CPU_LEVEL >= 6
  if (mode == BX_APIC_X2APIC_MODE) {
    // only cluster model supported in x2apic mode
    if (address == 0xffffffff) // // broadcast all
      return true;
    if ((address & 0xffff0000) == (ldr & 0xffff0000))
      match = ((address & ldr & 0x0000ffff) != 0);
    return match;
  }
#endif

  if (dest_format == 0xf) {
    // flat model
    match = ((address & ldr) != 0);
    BX_DEBUG(("comparing MDA %02x to my LDR %02x -> %s", address,
      ldr, match ? "Match" : "Not a match"));
  }
  else if (dest_format == 0) {
    // cluster model
    if (address == 0xff) // broadcast all
      return true;

    if ((unsigned)(address & 0xf0) == (unsigned)(ldr & 0xf0))
      match = ((address & ldr & 0x0f) != 0);
  }
  else {
    BX_PANIC(("bx_local_apic_c::match_logical_addr: unsupported dest format 0x%x", dest_format));
  }

  return match;
}

Bit8u bx_local_apic_c::get_ppr(void)
{
  int ppr = highest_priority_int(isr);

  if((ppr < 0) || ((task_priority & 0xF0) >= ((Bit32u) ppr & 0xF0)))
    ppr = task_priority;
  else
    ppr &= 0xF0;

  return ppr;
}

void bx_local_apic_c::set_tpr(Bit8u priority)
{
  if(priority < task_priority) {
    task_priority = priority;
    service_local_apic();
  } else {
    task_priority = priority;
  }
}

Bit8u bx_local_apic_c::get_apr(void)
{
  Bit32u tpr  = (task_priority >> 4) & 0xf;
  int first_isr = highest_priority_int(isr);
  if (first_isr < 0) first_isr = 0;
  int first_irr = highest_priority_int(irr);
  if (first_irr < 0) first_irr = 0;
  Bit32u isrv = (first_isr >> 4) & 0xf;
  Bit32u irrv = (first_irr >> 4) & 0xf;
  Bit8u  apr;

  if((tpr >= irrv) && (tpr > isrv)) {
    apr = task_priority & 0xff;
  }
  else {
    apr = ((tpr & isrv) > irrv) ?(tpr & isrv) : irrv;
    apr <<= 4;
  }

  BX_DEBUG(("apr = %d", apr));

  return(Bit8u) apr;
}

bool bx_local_apic_c::is_focus(Bit8u vector)
{
  if(focus_disable) return 0;
  return get_vector(irr, vector) || get_vector(isr, vector);
}

void bx_local_apic_c::periodic_smf(void *this_ptr)
{
  bx_local_apic_c *class_ptr = (bx_local_apic_c *) this_ptr;
  class_ptr->periodic();
}

void bx_local_apic_c::periodic(void)
{
  if(!timer_active) {
    BX_ERROR(("bx_local_apic_c::periodic called, timer_active==0"));
    return;
  }

  Bit32u timervec = lvt[APIC_LVT_TIMER];

  // If timer is not masked, trigger interrupt
  if((timervec & 0x10000)==0) {
    trigger_irq(timervec & 0xff, APIC_EDGE_TRIGGERED);
  }
  else {
    BX_DEBUG(("local apic timer LVT masked"));
  }

  // timer reached zero since the last call to periodic
  if(timervec & 0x20000) {
    // Periodic mode - reload timer values
    timer_current = timer_initial;
    timer_active = 1;
    ticksInitial = bx_pc_system.time_ticks(); // timer value when it started to count
    BX_DEBUG(("local apic timer(periodic) triggered int, reset counter to 0x%08x", timer_current));
    bx_pc_system.activate_timer_ticks(timer_handle,
            Bit64u(timer_initial) * Bit64u(timer_divide_factor), 0);
  }
  else {
    // one-shot mode
    timer_current = 0;
    timer_active = 0;
    BX_DEBUG(("local apic timer(one-shot) triggered int"));
    bx_pc_system.deactivate_timer(timer_handle);
  }
}

void bx_local_apic_c::set_divide_configuration(Bit32u value)
{
  BX_ASSERT(value == (value & 0x0b));
  // move bit 3 down to bit 0
  value = ((value & 8) >> 1) | (value & 3);
  BX_ASSERT(value >= 0 && value <= 7);
  timer_divide_factor = (value==7) ? 1 : (2 << value);
  BX_INFO(("set timer divide factor to %d", timer_divide_factor));
}

void bx_local_apic_c::set_initial_timer_count(Bit32u value)
{
#if BX_CPU_LEVEL >= 6
  Bit32u timervec = lvt[APIC_LVT_TIMER];

  // in TSC-deadline mode writes to initial time count are ignored
  if (timervec & 0x40000) return;
#endif

  // If active before, deactivate the current timer before changing it.
  if(timer_active) {
    bx_pc_system.deactivate_timer(timer_handle);
    timer_active = 0;
  }

  timer_initial = value;
  timer_current = 0;

  if(timer_initial != 0)  // terminate the counting if timer_initial = 0
  {
    // This should trigger the counter to start.  If already started,
    // restart from the new start value.
    BX_DEBUG(("APIC: Initial Timer Count Register = %u", value));
    timer_current = timer_initial;
    timer_active = 1;
    ticksInitial = bx_pc_system.time_ticks(); // timer value when it started to count
    bx_pc_system.activate_timer_ticks(timer_handle,
            Bit64u(timer_initial) * Bit64u(timer_divide_factor), 0);
  }
}

Bit32u bx_local_apic_c::get_current_timer_count(void)
{
#if BX_CPU_LEVEL >= 6
  Bit32u timervec = lvt[APIC_LVT_TIMER];

  // in TSC-deadline mode current timer count always reads 0
  if (timervec & 0x40000) return 0;
#endif

  if(timer_active==0) {
    return timer_current;
  } else {
    Bit64u delta64 = (bx_pc_system.time_ticks() - ticksInitial) / timer_divide_factor;
    Bit32u delta32 = (Bit32u) delta64;
    if(delta32 > timer_initial)
      BX_PANIC(("APIC: R(curr timer count): delta < initial"));
    timer_current = timer_initial - delta32;
    return timer_current;
  }
}

#if BX_CPU_LEVEL >= 6
void bx_local_apic_c::set_tsc_deadline(Bit64u deadline)
{
  Bit32u timervec = lvt[APIC_LVT_TIMER];

  if ((timervec & 0x40000) == 0) {
    BX_ERROR(("APIC: TSC-Deadline timer is disabled"));
    return;
  }

  // If active before, deactivate the current timer before changing it.
  if(timer_active) {
    bx_pc_system.deactivate_timer(timer_handle);
    timer_active = 0;
  }

  ticksInitial = deadline;
  if (deadline != 0) {
    BX_DEBUG(("APIC: TSC-Deadline is set to " FMT_LL "d", deadline));
    Bit64u currtime = bx_pc_system.time_ticks();
    timer_active = 1;
    bx_pc_system.activate_timer_ticks(timer_handle, (deadline > currtime) ? (deadline - currtime) : 1 , 0);
  }
}

Bit64u bx_local_apic_c::get_tsc_deadline(void)
{
  Bit32u timervec = lvt[APIC_LVT_TIMER];

  // read as zero if TSC-deadline timer is disabled
  if ((timervec & 0x40000) == 0) return 0;

  return ticksInitial; /* also holds TSC-deadline value */
}
#endif

#if BX_SUPPORT_VMX >= 2
Bit32u bx_local_apic_c::read_vmx_preemption_timer(void)
{
  Bit64u diff = (bx_pc_system.time_ticks() >> vmx_preemption_timer_rate) - (vmx_preemption_timer_initial >> vmx_preemption_timer_rate);
  if (vmx_preemption_timer_value < diff)
    return 0;
  else
    return vmx_preemption_timer_value - diff;
}

void bx_local_apic_c::set_vmx_preemption_timer(Bit32u value)
{
  vmx_preemption_timer_value = value;
  vmx_preemption_timer_initial = bx_pc_system.time_ticks();
  vmx_preemption_timer_fire = ((vmx_preemption_timer_initial >> vmx_preemption_timer_rate) + value) << vmx_preemption_timer_rate;
  BX_DEBUG(("VMX Preemption timer: value = %u, rate = %u, init = " FMT_LL "u, fire = " FMT_LL "u", value, vmx_preemption_timer_rate, vmx_preemption_timer_initial, vmx_preemption_timer_fire));
  bx_pc_system.activate_timer_ticks(vmx_timer_handle, vmx_preemption_timer_fire - vmx_preemption_timer_initial, 0);
  vmx_timer_active = 1;
}

void bx_local_apic_c::deactivate_vmx_preemption_timer(void)
{
  if (! vmx_timer_active) return;
  bx_pc_system.deactivate_timer(vmx_timer_handle);
  vmx_timer_active = 0;
}

// Invoked when VMX preemption timer expired
void bx_local_apic_c::vmx_preemption_timer_expired(void *this_ptr)
{
  bx_local_apic_c *class_ptr = (bx_local_apic_c *) this_ptr;
  class_ptr->cpu->signal_event(BX_EVENT_VMX_PREEMPTION_TIMER_EXPIRED);
  class_ptr->deactivate_vmx_preemption_timer();
}
#endif

#if BX_SUPPORT_MONITOR_MWAIT

void bx_local_apic_c::set_mwaitx_timer(Bit32u value)
{
  BX_DEBUG(("MWAITX timer: value = %u", value));
  bx_pc_system.activate_timer_ticks(mwaitx_timer_handle, value, 0);
  mwaitx_timer_active = 1;
}

void bx_local_apic_c::deactivate_mwaitx_timer(void)
{
  if (! mwaitx_timer_active) return;
  bx_pc_system.deactivate_timer(mwaitx_timer_handle);
  mwaitx_timer_active = 0;
}

// Invoked when MWAITX timer expired
void bx_local_apic_c::mwaitx_timer_expired(void *this_ptr)
{
  bx_local_apic_c *class_ptr = (bx_local_apic_c *) this_ptr;
  class_ptr->cpu->wakeup_monitor();
  class_ptr->deactivate_mwaitx_timer();
}

#endif

#if BX_CPU_LEVEL >= 6
// return false when x2apic is not supported/not readable
bool bx_local_apic_c::read_x2apic(unsigned index, Bit64u *val_64)
{
  index = (index - 0x800) << 4;

  switch(index) {
  // return full 32-bit lapic id
  case BX_LAPIC_ID:
    *val_64 = apic_id;
    break;
  case BX_LAPIC_LDR:
    *val_64 = ldr;
    break;
  // full 64-bit access to ICR
  case BX_LAPIC_ICR_LO:
    *val_64 = ((Bit64u) icr_lo) | (((Bit64u) icr_hi) << 32);
    break;
  // not supported/not readable in x2apic mode
  case BX_LAPIC_ARBITRATION_PRIORITY:
  case BX_LAPIC_DESTINATION_FORMAT:
  case BX_LAPIC_ICR_HI:
  case BX_LAPIC_EOI: // write only
  case BX_LAPIC_SELF_IPI: // write only
    return 0;
  // compatible to legacy lapic mode
  case BX_LAPIC_VERSION:
  case BX_LAPIC_TPR:
  case BX_LAPIC_PPR:
  case BX_LAPIC_SPURIOUS_VECTOR:
  case BX_LAPIC_ISR1:
  case BX_LAPIC_ISR2:
  case BX_LAPIC_ISR3:
  case BX_LAPIC_ISR4:
  case BX_LAPIC_ISR5:
  case BX_LAPIC_ISR6:
  case BX_LAPIC_ISR7:
  case BX_LAPIC_ISR8:
  case BX_LAPIC_TMR1:
  case BX_LAPIC_TMR2:
  case BX_LAPIC_TMR3:
  case BX_LAPIC_TMR4:
  case BX_LAPIC_TMR5:
  case BX_LAPIC_TMR6:
  case BX_LAPIC_TMR7:
  case BX_LAPIC_TMR8:
  case BX_LAPIC_IRR1:
  case BX_LAPIC_IRR2:
  case BX_LAPIC_IRR3:
  case BX_LAPIC_IRR4:
  case BX_LAPIC_IRR5:
  case BX_LAPIC_IRR6:
  case BX_LAPIC_IRR7:
  case BX_LAPIC_IRR8:
  case BX_LAPIC_ESR:
  case BX_LAPIC_LVT_TIMER:
  case BX_LAPIC_LVT_THERMAL:
  case BX_LAPIC_LVT_PERFMON:
  case BX_LAPIC_LVT_LINT0:
  case BX_LAPIC_LVT_LINT1:
  case BX_LAPIC_LVT_ERROR:
  case BX_LAPIC_LVT_CMCI:
  case BX_LAPIC_TIMER_INITIAL_COUNT:
  case BX_LAPIC_TIMER_CURRENT_COUNT:
  case BX_LAPIC_TIMER_DIVIDE_CFG:
    *val_64 = read_aligned(index);
    break;
  default:
    BX_ERROR(("read_x2apic: not supported apic register 0x%08x", index));
    return 0;
  }

  return 1;
}

// return false when x2apic is not supported/not writeable
bool bx_local_apic_c::write_x2apic(unsigned index, Bit32u val32_hi, Bit32u val32_lo)
{
  index = (index - 0x800) << 4;

  if (index != BX_LAPIC_ICR_LO) {
    // upper 32-bit are reserved for all x2apic MSRs except for the ICR
    if (val32_hi != 0)
      return 0;
  }

  switch(index) {
  // read only/not available in x2apic mode
  case BX_LAPIC_ID:
  case BX_LAPIC_VERSION:
  case BX_LAPIC_ARBITRATION_PRIORITY:
  case BX_LAPIC_PPR:
  case BX_LAPIC_LDR:
  case BX_LAPIC_DESTINATION_FORMAT:
  case BX_LAPIC_ISR1:
  case BX_LAPIC_ISR2:
  case BX_LAPIC_ISR3:
  case BX_LAPIC_ISR4:
  case BX_LAPIC_ISR5:
  case BX_LAPIC_ISR6:
  case BX_LAPIC_ISR7:
  case BX_LAPIC_ISR8:
  case BX_LAPIC_TMR1:
  case BX_LAPIC_TMR2:
  case BX_LAPIC_TMR3:
  case BX_LAPIC_TMR4:
  case BX_LAPIC_TMR5:
  case BX_LAPIC_TMR6:
  case BX_LAPIC_TMR7:
  case BX_LAPIC_TMR8:
  case BX_LAPIC_IRR1:
  case BX_LAPIC_IRR2:
  case BX_LAPIC_IRR3:
  case BX_LAPIC_IRR4:
  case BX_LAPIC_IRR5:
  case BX_LAPIC_IRR6:
  case BX_LAPIC_IRR7:
  case BX_LAPIC_IRR8:
  case BX_LAPIC_ICR_HI:
  case BX_LAPIC_TIMER_CURRENT_COUNT:
    return 0;
  // send self ipi
  case BX_LAPIC_SELF_IPI:
    trigger_irq(val32_lo & 0xff, APIC_EDGE_TRIGGERED);
    return 1;
  case BX_LAPIC_ICR_LO:
    // handle full 64-bit write
    send_ipi(val32_hi, val32_lo);
    return 1;
  case BX_LAPIC_TPR:
    // handle reserved bits, only bits 0-7 are writeable
    if ((val32_lo & 0xffffff00) != 0)
      return 0;
    break; // use legacy write
  case BX_LAPIC_SPURIOUS_VECTOR:
    // handle reserved bits, only bits 0-8, 12 are writeable
    // we do not support directed EOI capability, so reserve bit 12 as well
    if ((val32_lo & 0xfffffe00) != 0)
      return 0;
    break; // use legacy write
  case BX_LAPIC_EOI:
  case BX_LAPIC_ESR:
    if (val32_lo != 0) return 0;
    break; // use legacy write
  case BX_LAPIC_LVT_TIMER:
  case BX_LAPIC_LVT_THERMAL:
  case BX_LAPIC_LVT_PERFMON:
  case BX_LAPIC_LVT_LINT0:
  case BX_LAPIC_LVT_LINT1:
  case BX_LAPIC_LVT_ERROR:
  case BX_LAPIC_LVT_CMCI:
  case BX_LAPIC_TIMER_INITIAL_COUNT:
  case BX_LAPIC_TIMER_DIVIDE_CFG:
    break; // use legacy write
  default:
    BX_ERROR(("write_x2apic: not supported apic register 0x%08x", index));
    return 0;
  }

  write_aligned(index, val32_lo);
  return 1;
}
#endif

void bx_local_apic_c::register_state(bx_param_c *parent)
{
  unsigned i;
  char name[6];

  bx_list_c *lapic = new bx_list_c(parent, "local_apic");

  BXRS_HEX_PARAM_SIMPLE(lapic, base_addr);
  BXRS_HEX_PARAM_SIMPLE(lapic, apic_id);
  BXRS_HEX_PARAM_SIMPLE(lapic, mode);
  BXRS_HEX_PARAM_SIMPLE(lapic, spurious_vector);
  BXRS_PARAM_BOOL(lapic, software_enabled, software_enabled);
  BXRS_PARAM_BOOL(lapic, focus_disable, focus_disable);
  BXRS_HEX_PARAM_SIMPLE(lapic, task_priority);
  BXRS_HEX_PARAM_SIMPLE(lapic, ldr);
  BXRS_HEX_PARAM_SIMPLE(lapic, dest_format);

  for (i=0; i<8; i++) {
    sprintf(name, "isr%u", i);
    new bx_shadow_num_c(lapic, name, &isr[i], BASE_HEX);

    sprintf(name, "tmr%u", i);
    new bx_shadow_num_c(lapic, name, &tmr[i], BASE_HEX);

    sprintf(name, "irr%u", i);
    new bx_shadow_num_c(lapic, name, &irr[i], BASE_HEX);
  }

#if BX_CPU_LEVEL >= 6
  if (cpu->is_cpu_extension_supported(BX_ISA_XAPIC_EXT)) {
    BXRS_HEX_PARAM_SIMPLE(lapic, xapic_ext);
    for (i=0; i<8; i++) {
      sprintf(name, "ier%u", i);
      new bx_shadow_num_c(lapic, name, &ier[i], BASE_HEX);
    }
  }
#endif

  BXRS_HEX_PARAM_SIMPLE(lapic, error_status);
  BXRS_HEX_PARAM_SIMPLE(lapic, shadow_error_status);
  BXRS_HEX_PARAM_SIMPLE(lapic, icr_hi);
  BXRS_HEX_PARAM_SIMPLE(lapic, icr_lo);

  for (i=0; i<APIC_LVT_ENTRIES; i++) {
    sprintf(name, "lvt%u", i);
    new bx_shadow_num_c(lapic, name, &lvt[i], BASE_HEX);
  }

  BXRS_HEX_PARAM_SIMPLE(lapic, timer_initial);
  BXRS_HEX_PARAM_SIMPLE(lapic, timer_current);
  BXRS_HEX_PARAM_SIMPLE(lapic, timer_divconf);
  BXRS_DEC_PARAM_SIMPLE(lapic, timer_divide_factor);
  BXRS_PARAM_BOOL(lapic, timer_active, timer_active);
  BXRS_HEX_PARAM_SIMPLE(lapic, ticksInitial);

#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_SIMPLE(lapic, vmx_preemption_timer_initial);
  BXRS_HEX_PARAM_SIMPLE(lapic, vmx_preemption_timer_fire);
  BXRS_HEX_PARAM_SIMPLE(lapic, vmx_preemption_timer_value);
  BXRS_HEX_PARAM_SIMPLE(lapic, vmx_preemption_timer_rate);
  BXRS_PARAM_BOOL(lapic, vmx_timer_active, vmx_timer_active);
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  BXRS_PARAM_BOOL(lapic, mwaitx_timer_active, mwaitx_timer_active);
#endif
}

#endif /* if BX_SUPPORT_APIC */
