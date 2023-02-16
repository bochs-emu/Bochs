/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  High Precision Event Timer emulation ported from Qemu
//
//  Copyright (c) 2007 Alexander Graf
//  Copyright (c) 2008 IBM Corporation
//
//  Authors: Beth Kon <bkon@us.ibm.com>
//
//  Copyright (C) 2017-2021  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI

#include "hpet.h"

#define LOG_THIS theHPET->

bx_hpet_c *theHPET = NULL;

// device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(hpet)
{
  if (mode == PLUGIN_INIT) {
    theHPET = new bx_hpet_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theHPET, BX_PLUGIN_HPET);
  } else if (mode == PLUGIN_FINI) {
    delete theHPET;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_STANDARD;
  }
  return(0); // Success
}

// helper functions

// Start is assumed to be not later than end.
// If start == end, it describes one point in time.
// Returns true if value happened after start but before end.
static Bit32u hpet_time_between(Bit64u start, Bit64u end, Bit64u value)
{
  if (start <= end) { // No wraparound after start and before end
    return (start <= value) && (value <= end);
  } else { // Wraparound
    return (start <= value) || (value <= end);
  }
}

/* Returns earliest 64-bit tick value that is after reference
 * and has same lower 32 bits as value
 */
static Bit64u hpet_cmp32_to_cmp64(Bit64u reference, Bit32u value)
{
  if ((Bit32u)reference <= value) {
    return (reference & 0xFFFFFFFF00000000ull) | (Bit64u)value;
  } else {
    return ((reference + 0x100000000ull) & 0xFFFFFFFF00000000ull) | (Bit64u)value;
  }
}

static Bit64u ticks_to_ns(Bit64u value)
{
    return value * HPET_CLK_PERIOD;
}

static Bit64u ns_to_ticks(Bit64u value)
{
    return value / HPET_CLK_PERIOD;
}

static Bit64u hpet_fixup_reg(Bit64u _new, Bit64u old, Bit64u mask)
{
    _new &= mask;
    _new |= old & ~mask;
    return _new;
}

static int activating_bit(Bit64u old, Bit64u _new, Bit64u mask)
{
    return (!(old & mask) && (_new & mask));
}

static int deactivating_bit(Bit64u old, Bit64u _new, Bit64u mask)
{
    return ((old & mask) && !(_new & mask));
}

// static memory read/write functions

static bool hpet_read(bx_phy_address a20addr, unsigned len, void *data, void *param)
{
  Bit32u value1;
  Bit64u value2;

  if (len == 4) { // must be 32-bit aligned
    if ((a20addr & 0x3) != 0) {
      BX_PANIC(("Unaligned HPET read at address 0x" FMT_PHY_ADDRX, a20addr));
      return 1;
    }
    value1 = theHPET->read_aligned(a20addr);
    *((Bit32u *)data) = value1;
    return 1;
  } else if (len == 8) { // must be 64-bit aligned
    if ((a20addr & 0x7) != 0) {
      BX_PANIC(("Unaligned HPET read at address 0x" FMT_PHY_ADDRX, a20addr));
      return 1;
    }
    value1 = theHPET->read_aligned(a20addr);
    value2 = theHPET->read_aligned(a20addr + 4);
    *((Bit64u *)data) = (value1 | (value2 << 32));
    return 1;
  } else {
    BX_PANIC(("Unsupported HPET read at address 0x" FMT_PHY_ADDRX, a20addr));
  }
  return 1;
}

static bool hpet_write(bx_phy_address a20addr, unsigned len, void *data, void *param)
{
  if (len == 4) { // must be 32-bit aligned
    if ((a20addr & 0x3) != 0) {
      BX_PANIC(("Unaligned HPET write at address 0x" FMT_PHY_ADDRX, a20addr));
      return 1;
    }
    theHPET->write_aligned(a20addr, *((Bit32u*) data), true);
    return 1;
  } else if (len == 8) { // must be 64-bit aligned
    if ((a20addr & 0x7) != 0) {
      BX_PANIC(("Unaligned HPET write at address 0x" FMT_PHY_ADDRX, a20addr));
      return 1;
    }
    Bit64u val64 = *((Bit64u*) data);
    theHPET->write_aligned(a20addr, (Bit32u)val64, false);
    theHPET->write_aligned(a20addr + 4, (Bit32u)(val64 >> 32), true);
  } else {
    BX_PANIC(("Unsupported HPET write at address 0x" FMT_PHY_ADDRX, a20addr));
  }
  return 1;
}

// the device object

bx_hpet_c::bx_hpet_c()
{
  put("HPET");
  memset(&s, 0, sizeof(s));
}

bx_hpet_c::~bx_hpet_c()
{
  SIM->get_bochs_root()->remove("hpet");
  BX_DEBUG(("Exit"));
}

void bx_hpet_c::init(void)
{
  BX_INFO(("initializing HPET"));
  s.num_timers = HPET_MIN_TIMERS;
  s.capability = BX_CONST64(0x8086a001) | ((s.num_timers - 1) << 8);
  s.capability |= ((Bit64u)(HPET_CLK_PERIOD * FS_PER_NS) << 32);
  s.isr = 0x00;
  DEV_register_memory_handlers(theHPET, hpet_read, hpet_write,
      HPET_BASE, HPET_BASE + HPET_LEN - 1);
  for (int i = 0; i < s.num_timers; i++) {
    s.timer[i].tn = i;
    s.timer[i].timer_id =
      DEV_register_timer(this, timer_handler, 1, 0, 0, "hpet");
    bx_pc_system.setTimerParam(s.timer[i].timer_id, i);
  }
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("hpet", this);
#endif
}

void bx_hpet_c::reset(unsigned type)
{
  for (int i = 0; i < s.num_timers; i++) {
    HPETTimer *timer = &s.timer[i];

    hpet_del_timer(timer);
    timer->cmp = ~BX_CONST64(0);
    timer->period = ~BX_CONST64(0);
    timer->config = HPET_TN_PERIODIC_CAP | HPET_TN_SIZE_CAP | (HPET_ROUTING_CAP << 32);
    timer->last_checked = BX_CONST64(0);
  }
  s.hpet_counter = BX_CONST64(0);
  s.hpet_reference_value = BX_CONST64(0);
  s.hpet_reference_time = BX_CONST64(0);
  s.config = BX_CONST64(0);
}

void bx_hpet_c::register_state(void)
{
  char tnum[16];
  bx_list_c *tim;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "hpet", "HPET State");
  BXRS_HEX_PARAM_FIELD(list, config, s.config);
  BXRS_HEX_PARAM_FIELD(list, isr, s.isr);
  BXRS_HEX_PARAM_FIELD(list, hpet_counter, s.hpet_counter);
  for (int i = 0; i < s.num_timers; i++) {
    sprintf(tnum, "timer%d", i);
    tim = new bx_list_c(list, tnum);
    BXRS_HEX_PARAM_FIELD(tim, config, s.timer[i].config);
    BXRS_HEX_PARAM_FIELD(tim, cmp, s.timer[i].cmp);
    BXRS_HEX_PARAM_FIELD(tim, fsb, s.timer[i].fsb);
    BXRS_DEC_PARAM_FIELD(tim, period, s.timer[i].period);
  }
}

Bit64u bx_hpet_c::hpet_get_ticks(void)
{
  return ns_to_ticks(bx_pc_system.time_nsec() - s.hpet_reference_time) + s.hpet_reference_value;
}

/*
 * calculate diff between comparator value and current ticks
 */
Bit64u bx_hpet_c::hpet_calculate_diff(HPETTimer *t, Bit64u current)
{
  if (t->config & HPET_TN_32BIT) {
    Bit32u diff, cmp;

    cmp = (Bit32u)t->cmp;
    diff = cmp - (Bit32u)current;
    return (Bit64u)diff;
  } else {
    Bit64u diff2, cmp2;

    cmp2 = t->cmp;
    diff2 = cmp2 - current;
    return diff2;
  }
}

void bx_hpet_c::update_irq(HPETTimer *timer, bool set)
{
  Bit64u mask;
  int route;
  BX_DEBUG(("Timer %d irq level set to %d", timer->tn, set));

  if ((timer->tn <= 1) && hpet_in_legacy_mode()) {
    /* if LegacyReplacementRoute bit is set, HPET specification requires
     * timer0 be routed to IRQ0 in NON-APIC or IRQ2 in the I/O APIC,
     * timer1 be routed to IRQ8 in NON-APIC or IRQ8 in the I/O APIC.
     */
    route = (timer->tn == 0) ? 0 : RTC_ISA_IRQ;
  } else {
    route = timer_int_route(timer);
  }
  mask = (BX_CONST64(1) << timer->tn);
  if (!set || !hpet_enabled()) {
    DEV_pic_lower_irq(route);
  } else {
    if (timer->config & HPET_TN_TYPE_LEVEL) {
      /* If HPET_TN_ENABLE bit is 0, "the timer will still operate and
       * generate appropriate status bits, but will not cause an interrupt"
       */
      s.isr |= mask;
    }
    if (timer_enabled(timer)) {
      if (timer_fsb_route(timer)) {
        Bit32u val32 = (Bit32u)timer->fsb;
        DEV_MEM_WRITE_PHYSICAL((bx_phy_address) (timer->fsb >> 32), sizeof(Bit32u), (Bit8u *) &val32);
      } else if (timer->config & HPET_TN_TYPE_LEVEL) {
        DEV_pic_raise_irq(route);
      } else {
        DEV_pic_lower_irq(route);
        DEV_pic_raise_irq(route);
      }
    }
  }
}

void bx_hpet_c::timer_handler(void *this_ptr)
{
  bx_hpet_c *class_ptr = (bx_hpet_c *) this_ptr;
  class_ptr->hpet_timer();
}

void bx_hpet_c::hpet_timer()
{
  HPETTimer *t = &s.timer[bx_pc_system.triggeredTimerParam()];
  Bit64u cur_time = bx_pc_system.time_nsec();
  Bit64u cur_tick = hpet_get_ticks();

  if (timer_is_periodic(t)) {
    if (t->config & HPET_TN_32BIT) {
      Bit64u cmp64 = hpet_cmp32_to_cmp64(t->last_checked, (Bit32u)t->cmp);
      if (hpet_time_between(t->last_checked, cur_tick, cmp64)) {
        update_irq(t, 1);
        if ((Bit32u)t->period != 0) {
          do {
            cmp64 += (Bit64u)(Bit32u)t->period;
          } while (hpet_time_between(t->last_checked, cur_tick, cmp64));
          t->cmp = (Bit32u)cmp64;
        }
      }
    } else { // 64-bit timer
      if (hpet_time_between(t->last_checked, cur_tick, t->cmp)) {
        update_irq(t, 1);
        if (t->period != 0) {
          do {
            t->cmp += t->period;
          } while (hpet_time_between(t->last_checked, cur_tick, t->cmp));
        }
      }
    }
  } else { // One-shot timer
    if (t->config & HPET_TN_32BIT) {
      Bit64u cmp64 = hpet_cmp32_to_cmp64(t->last_checked, (Bit32u)t->cmp);
      Bit64u wrap = hpet_cmp32_to_cmp64(t->last_checked, 0);
      if (hpet_time_between(t->last_checked, cur_tick, cmp64) || hpet_time_between(t->last_checked, cur_tick, wrap)) {
        update_irq(t, 1);
      }
    } else { // 64-bit timer
      if (hpet_time_between(t->last_checked, cur_tick, t->cmp)) {
        update_irq(t, 1);
      }
    }
  }
  hpet_set_timer(t);
  t->last_checked = cur_tick;

  Bit64u ticks_passed = ns_to_ticks(cur_time - s.hpet_reference_time);
  if (ticks_passed != 0) {
    s.hpet_reference_time += ticks_to_ns(ticks_passed);
    s.hpet_reference_value += ticks_passed;
  }
}

void bx_hpet_c::hpet_set_timer(HPETTimer *t)
{
  Bit64u cur_tick = hpet_get_ticks();
  Bit64u diff = hpet_calculate_diff(t, cur_tick);
  if (diff == 0) {
    if (t->config & HPET_TN_32BIT) {
      diff = 0x100000000ull;
    } else {
      diff = HPET_MAX_ALLOWED_PERIOD;
    }
  }
  /* hpet spec says in one-shot 32-bit mode, generate an interrupt when
   * counter wraps in addition to an interrupt with comparator match.
   */
  if (!timer_is_periodic(t)) {
    if (t->config & HPET_TN_32BIT) {
      Bit64u wrap_diff = 0x100000000ull - (Bit64u)(Bit32u)cur_tick;
      if (wrap_diff < diff) diff = wrap_diff;
    }
  }
  if (diff < HPET_MIN_ALLOWED_PERIOD) diff = HPET_MIN_ALLOWED_PERIOD;
  if (diff > HPET_MAX_ALLOWED_PERIOD) diff = HPET_MAX_ALLOWED_PERIOD;
  BX_DEBUG(("Timer %d to fire in 0x%lX ticks", t->tn, diff));
  bx_pc_system.activate_timer_nsec(t->timer_id, ticks_to_ns(diff), 0);
}

void bx_hpet_c::hpet_del_timer(HPETTimer *t)
{
  BX_DEBUG(("Timer %d deactivated", t->tn));
  bx_pc_system.deactivate_timer(t->timer_id);
  update_irq(t, 0);
}

Bit32u bx_hpet_c::read_aligned(bx_phy_address address)
{
  Bit32u value = 0;

  // BX_DEBUG(("read aligned addr=0x" FMT_PHY_ADDRX, address));
  Bit16u index = (Bit16u)(address & 0x3ff);
  if (index < 0x100) {
    switch (index) {
      case HPET_ID:
        value = (Bit32u)s.capability;
        break;
      case HPET_PERIOD:
        value = (Bit32u)(s.capability >> 32);
        break;
      case HPET_CFG:
        value = (Bit32u)s.config;
        break;
      case HPET_CFG + 4:
        value = (Bit32u)(s.config >> 32);
        break;
      case HPET_STATUS:
        value = (Bit32u)s.isr;
        break;
      case HPET_STATUS + 4:
        value = (Bit32u)(s.isr >> 32);
        break;
      case HPET_COUNTER:
        if (hpet_enabled()) {
          value = (Bit32u)hpet_get_ticks();
        } else {
          value = (Bit32u)s.hpet_counter;
        }
        break;
      case HPET_COUNTER + 4:
        if (hpet_enabled()) {
          value = (Bit32u)(hpet_get_ticks() >> 32);
        } else {
          value = (Bit32u)(s.hpet_counter >> 32);
        }
        break;
      default:
        BX_ERROR(("read from reserved offset 0x%04x", index));
    }
  } else {
    Bit8u id = (index - 0x100) / 0x20;
    if (id >= s.num_timers) {
      BX_ERROR(("read: timer id out of range"));
      return 0;
    }
    HPETTimer *timer = &s.timer[id];
    switch (index & 0x1f) {
      case HPET_TN_CFG:
        value = (Bit32u)timer->config;
        break;
      case HPET_TN_CFG + 4:
        value = (Bit32u)(timer->config >> 32);
        break;
      case HPET_TN_CMP:
        value = (Bit32u)timer->cmp;
        break;
      case HPET_TN_CMP + 4:
        value = (Bit32u)(timer->cmp >> 32);
        break;
      case HPET_TN_ROUTE:
        value = (Bit32u)timer->fsb;
        break;
      case HPET_TN_ROUTE + 4:
        value = (Bit32u)(timer->fsb >> 32);
        break;
      default:
        BX_ERROR(("read from reserved offset 0x%04x", index));
    }
  }
  return value;
}

void bx_hpet_c::write_aligned(bx_phy_address address, Bit32u value, bool trailing_write)
{
  int i;
  Bit16u index = (Bit16u)(address & 0x3ff);
  Bit64u val;
  Bit64u new_val = value;
  Bit64u old_val = read_aligned(address);

  BX_DEBUG(("write aligned addr=0x" FMT_PHY_ADDRX ", data=0x%08x", address, value));
  if (index < 0x100) {
    switch (index) {
      case HPET_ID:
        break;
      case HPET_ID + 4:
        break;
      case HPET_CFG:
        val = hpet_fixup_reg(new_val, old_val, HPET_CFG_WRITE_MASK);
        s.config = (s.config & BX_CONST64(0xffffffff00000000)) | val;
        if (activating_bit(old_val, new_val, HPET_CFG_ENABLE)) {
          /* Enable main counter and interrupt generation. */
          s.hpet_reference_value = s.hpet_counter;
          s.hpet_reference_time = bx_pc_system.time_nsec();
          for (i = 0; i < s.num_timers; i++) {
            if (timer_enabled(&s.timer[i]) && (s.isr & (BX_CONST64(1) << i))) {
              update_irq(&s.timer[i], 1);
            }
            hpet_set_timer(&s.timer[i]);
          }
        } else if (deactivating_bit(old_val, new_val, HPET_CFG_ENABLE)) {
          /* Halt main counter and disable interrupt generation. */
          s.hpet_counter = hpet_get_ticks();
          for (i = 0; i < s.num_timers; i++) {
            hpet_del_timer(&s.timer[i]);
          }
        }
        /* i8254 and RTC output pins are disabled
         * when HPET is in legacy mode */
        if (activating_bit(old_val, new_val, HPET_CFG_LEGACY)) {
          BX_INFO(("Entering legacy mode"));
          DEV_pit_enable_irq(0);
          DEV_cmos_enable_irq(0);
        } else if (deactivating_bit(old_val, new_val, HPET_CFG_LEGACY)) {
          BX_INFO(("Leaving legacy mode"));
          DEV_pit_enable_irq(1);
          DEV_cmos_enable_irq(1);
        }
        break;
      case HPET_CFG + 4:
        break;
      case HPET_STATUS:
        val = new_val & s.isr;
        for (i = 0; i < s.num_timers; i++) {
          if (val & (BX_CONST64(1) << i)) {
            update_irq(&s.timer[i], 0);
            s.isr &= ~(BX_CONST64(1) << i);
          }
        }
        break;
      case HPET_STATUS + 4:
        break;
      case HPET_COUNTER:
        if (hpet_enabled()) {
          BX_ERROR(("Writing counter while HPET enabled!"));
        } else {
          s.hpet_counter = (s.hpet_counter & BX_CONST64(0xffffffff00000000)) | value;
          for (i = 0; i < s.num_timers; i++) {
            s.timer[i].last_checked = s.hpet_counter;
          }
        }
        break;
      case HPET_COUNTER + 4:
        if (hpet_enabled()) {
          BX_ERROR(("Writing counter while HPET enabled!"));
        } else {
          s.hpet_counter = (s.hpet_counter & BX_CONST64(0xffffffff)) | (((Bit64u)value) << 32);
          for (i = 0; i < s.num_timers; i++) {
            s.timer[i].last_checked = s.hpet_counter;
          }
        }
        break;
      default:
        BX_ERROR(("write to reserved offset 0x%04x", index));
    }
  } else {
    Bit8u id = (index - 0x100) / 0x20;
    if (id >= s.num_timers) {
      BX_ERROR(("write: timer id out of range"));
      return;
    }
    HPETTimer *timer = &s.timer[id];
    switch (index & 0x1f) {
      case HPET_TN_CFG:
        val = hpet_fixup_reg(new_val, old_val, HPET_TN_CFG_WRITE_MASK);
        timer->config = (timer->config & BX_CONST64(0xffffffff00000000)) | val;
        if (timer->config & HPET_TN_32BIT) {
          timer->cmp = (Bit32u)timer->cmp;
          timer->period = (Bit32u)timer->period;
        }
        if (timer_fsb_route(timer) || !(timer->config & HPET_TN_TYPE_LEVEL)) {
          s.isr &= ~(BX_CONST64(1) << id);
        }
        if (timer_enabled(timer) && hpet_enabled()) {
          if (s.isr & (BX_CONST64(1) << id)) {
            update_irq(timer, 1);
          } else {
            update_irq(timer, 0);
          }
        }
        if (hpet_enabled()) {
          hpet_set_timer(timer);
        }
        break;
      case HPET_TN_CFG + 4:
        break;
      case HPET_TN_CMP:
        if (!timer_is_periodic(timer) || (timer->config & HPET_TN_SETVAL)) {
          timer->cmp = (timer->cmp & BX_CONST64(0xffffffff00000000)) | new_val;
        }
        timer->period = (timer->period & BX_CONST64(0xffffffff00000000)) | new_val;
        if (trailing_write) {
          timer->config &= ~HPET_TN_SETVAL;
        }
        if (hpet_enabled()) {
          hpet_set_timer(timer);
        }
        break;
      case HPET_TN_CMP + 4:
        if (timer->config & HPET_TN_32BIT) break;
        if (!timer_is_periodic(timer) || (timer->config & HPET_TN_SETVAL)) {
          timer->cmp = (timer->cmp & BX_CONST64(0xffffffff)) | (new_val << 32);
        }
        timer->period = (timer->period & BX_CONST64(0xffffffff)) | (new_val << 32);
        if (trailing_write) {
          timer->config &= ~HPET_TN_SETVAL;
        }
        if (hpet_enabled()) {
          hpet_set_timer(timer);
        }
        break;
      case HPET_TN_ROUTE:
        timer->fsb = (timer->fsb & BX_CONST64(0xffffffff00000000)) | new_val;
        break;
      case HPET_TN_ROUTE + 4:
        timer->fsb = (new_val << 32) | (timer->fsb & 0xffffffff);
        break;
      default:
        BX_ERROR(("write to reserved offset 0x%04x", index));
    }
  }
}

#if BX_DEBUGGER
void bx_hpet_c::debug_dump(int argc, char **argv)
{
  Bit64u value;

  dbg_printf("HPET\n\n");
  dbg_printf("enable config    = %d\n", s.config & 1);
  dbg_printf("legacy config    = %d\n", (s.config >> 1) & 1);
  dbg_printf("interrupt status = 0x%08x\n", (Bit32u)s.isr);
  if (hpet_enabled()) {
    value = hpet_get_ticks();
  } else {
    value = s.hpet_counter;
  }
  dbg_printf("main counter = 0x" FMT_LL "x\n\n", value);
  for (int i = 0; i < s.num_timers; i++) {
    HPETTimer *timer = &s.timer[i];
    dbg_printf("timer #%d (%d-bit)\n", i, ((timer->config & HPET_TN_32BIT) > 0) ? 32:64);
    dbg_printf("interrupt enable = %d\n", timer_enabled(timer) > 0);
    dbg_printf("periodic mode    = %d\n", timer_is_periodic(timer) > 0);
    dbg_printf("level sensitive  = %d\n", (timer->config & HPET_TN_TYPE_LEVEL) > 0);
    if (timer->config & HPET_TN_32BIT) {
      dbg_printf("comparator value = 0x%08x\n", (Bit32u)timer->cmp);
      dbg_printf("period           = 0x%08x\n", (Bit32u)timer->period);
    } else {
      dbg_printf("comparator value = 0x" FMT_LL "x\n", timer->cmp);
      dbg_printf("period           = 0x" FMT_LL "x\n", timer->period);
    }
  }
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif

#endif /* if BX_SUPPORT_PCI */
