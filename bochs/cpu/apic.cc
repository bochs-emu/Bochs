#define BX_IN_CPU_METHOD 1
#include "bochs.h"
#include <assert.h>

bx_generic_apic_c *apic_index[APIC_MAX_ID];

bx_generic_apic_c::bx_generic_apic_c () 
{
  id = APIC_UNKNOWN_ID;
  hwreset ();
}

bx_generic_apic_c::~bx_generic_apic_c () 
{
}

// init is called during RESET and when an INIT message is delivered.
void bx_generic_apic_c::init ()
{
}

void bx_generic_apic_c::set_base (Bit32u newbase)
{
  bx_printf ("relocate APIC id=%d to %8x\n", id, newbase);
  base_addr = newbase;
}

void bx_generic_apic_c::set_id (Bit8u newid) {
  // update apic_index
  if (id != APIC_UNKNOWN_ID) {
    bx_assert (id < APIC_MAX_ID);
    if (apic_index[id] != this)
      bx_panic ("inconsistent APIC id table\n");
    apic_index[id] = NULL;
  }
  id = newid;
  if (apic_index[id] != NULL)
    bx_panic ("duplicate APIC id assigned\n");
  apic_index[id] = this;
}

char *
bx_generic_apic_c::get_name () {
  bx_panic ("get_name called on bx_generic_apic_c base class\n");
}

Boolean
bx_generic_apic_c::is_selected (Bit32u addr, Bit32u len)
{
  if ((addr & ~0xfff) == get_base ()) {
    if ((addr & 0xf != 0) || (len != 4))
      bx_printf ("warning: misaligned or wrong-size APIC write\n");
    return true;
  }
  return false;
}

void
bx_generic_apic_c::read (Bit32u addr, void *data, unsigned len)
{
  if ((addr & ~0xf) != ((addr+len-1) & ~0xf))
    bx_panic ("APIC read spans 32-bit boundary\n");
  Bit32u value;
  read_aligned (addr, &value, 4);
  if ((addr&3) == 0) {
    *((Bit32u *)data) = value;
    return;
  }
  // handle partial word read
  Bit8u bytes[4];
  bytes[0] = value & 0xff;
  bytes[1] = (value >> 8) & 0xff;
  bytes[2] = (value >> 16) & 0xff;
  bytes[3] = (value >> 24) & 0xff;
  Bit8u *p1 = bytes+(addr&3);
  Bit8u *p2 = (Bit8u *)data;
  for (int i=0; i<len; i++) {
    if (bx_dbg.apic)
      bx_printf ("apic: Copying byte %02x\n", (unsigned int) *p1);
    *p2++ = *p1++;
  }
}

void bx_generic_apic_c::read_aligned (Bit32u address, Bit32u *data, unsigned len)
{
  bx_panic ("read_aligned not implemented in base class bx_generic_apic_c\n");
}

void bx_generic_apic_c::write(Bit32u address, Bit32u *value, unsigned len)
{
  bx_panic ("write not implemented in base class bx_generic_apic_c\n");
}

void bx_generic_apic_c::startup_msg (Bit32u vector)
{
  bx_panic ("startup message sent to an I/O APIC\n");
}

void bx_generic_apic_c::trigger_irq (unsigned num, unsigned from)
{
  bx_panic ("trigger_irq called on base class\n");
}

void bx_generic_apic_c::untrigger_irq (unsigned num, unsigned from)
{
  bx_panic ("untrigger_irq called on base class\n");
}

Boolean bx_generic_apic_c::match_logical_addr (Bit8u address) {
  bx_panic ("match_logical_addr called on base class\n");
}

bx_apic_type_t bx_generic_apic_c::get_type () {
  bx_panic ("get_type called on base class");
}

Bit32u
bx_generic_apic_c::get_delivery_bitmask (Bit8u dest, Bit8u dest_mode)
{
  int mask = 0;
  if (dest_mode == 0) {
    // physical 
    if (dest < APIC_MAX_ID)
      mask = 1<<dest;
    else if (dest == 0xff) {
      // physical destination 0xff means everybody. only local APICs can
      // send this.
      bx_assert (get_type () == APIC_TYPE_LOCAL_APIC);
      mask = 0xff; 
    } else bx_panic ("bx_generic_apic_c::deliver: illegal physical destination %02x\n", dest);
  } else {
    // logical destination. call match_logical_addr for each APIC.
    if (dest == 0) return 0;
    for (int i=0; i<APIC_MAX_ID; i++) {
      if (apic_index[i] && apic_index[i]->match_logical_addr(dest))
	mask |= (1<<i);
    }
  }
  if (bx_dbg.apic)
    bx_printf ("generic::get_delivery_bitmask returning 0x%04x\n", mask);
  return mask;
}

Boolean
bx_generic_apic_c::deliver (Bit8u dest, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode)
{
  // return false if we can't deliver for any reason, so that the caller
  // knows not to clear its IRR.
  Bit32u deliver_bitmask = get_delivery_bitmask (dest, dest_mode);
  // mask must include ONLY local APICs, or we will have problems.
  if (!deliver_bitmask) {
    bx_printf ("deliver failed: no APICs in destination bitmask\n");
    return false;
  }
  switch (delivery_mode) {
    case 0:  // fixed
      break;
    case 1:  // lowest priority of destinations
      {
	// find lowest priority of apics in the mask
	int lowest_priority = 0x100, lowest_mask = -1;
	for (int bit=0; bit<APIC_MAX_ID; bit++) {
	  if (deliver_bitmask & (1<<bit)) {
	    bx_local_apic_c *apic = (bx_local_apic_c *)apic_index[bit];
	    if (apic->get_ppr () < lowest_priority) {
	      lowest_priority = apic->get_ppr (); lowest_mask = 1<<bit;
	    }
	  }
	}
	deliver_bitmask = lowest_mask;
	bx_assert (deliver_bitmask >= 0);
      }
      break;
    case 5:  // INIT
      {
	for (int bit=0; bit<APIC_MAX_ID; bit++) {
	  if (deliver_bitmask & (1<<bit)) 
	    apic_index[bit]->init ();
	}
      }
      return true;
    case 6:  // Start Up (local apic only)
      bx_assert (get_type () == APIC_TYPE_LOCAL_APIC);
      for (int bit=0; bit<APIC_MAX_ID; bit++)
	if (deliver_bitmask & (1<<bit))
	  apic_index[bit]->startup_msg (vector);
      return true;
    case 2:  // SMI
    case 3:  // reserved
    case 4:  // NMI
    case 7:  // ExtINT (I/O apic only)
    default:
      bx_panic ("APIC delivery mode %d not implemented\n", delivery_mode);
  }
  // Fixed delivery mode
  if (bx_dbg.apic)
    bx_printf ("delivering vector=0x%02x to bitmask=%04x\n", (int)vector, deliver_bitmask);
  for (int bit=0; bit<APIC_MAX_ID; bit++) {
    if (deliver_bitmask & (1<<bit)) {
      if (apic_index[bit] == NULL)
	bx_printf ("IOAPIC: delivering int0x%x to nonexistent id=%d!\n", (unsigned)vector, bit);
      else {
        if (bx_dbg.apic)
	  bx_printf ("IOAPIC: delivering int0x%x to apic#%d\n", (unsigned)vector, bit);
	apic_index[bit]->trigger_irq (vector, id);
      }
    }
  }
  return true;
}

bx_local_apic_c::bx_local_apic_c(BX_CPU_C *mycpu)
  : bx_generic_apic_c ()
{
  cpu = mycpu;
  hwreset ();
}

void
bx_local_apic_c::hwreset () 
{
  /* same as INIT but also sets arbitration ID and APIC ID */
  init ();
  /* since id is set explicitly by the function that creates the CPU
     object, do not mess around with it */
  // id = APIC_UNKNOWN_ID;
  arb_id = id;
}

void
bx_local_apic_c::init ()
{
  bx_generic_apic_c::init ();
  bx_printf ("local apic in %s initializing\n", 
      (cpu && cpu->name) ? cpu->name : "?");
  // default address for a local APIC, can be moved
  base_addr = 0xfee00000;
  err_status = 0;
  log_dest = 0;
  dest_format = 0xff;
  for (int bit=0; bit<BX_LOCAL_APIC_MAX_INTS; bit++) {
    irr[bit] = isr[bit] = tmr[bit] = 0;
  }
  icr_high = icr_low = log_dest = task_priority = 0;
  spurious_vec = 0xff;   // software disabled (bit 8)
}

BX_CPU_C 
bx_local_apic_c::*get_cpu (Bit8u id)
{
  bx_assert (id < APIC_MAX_ID);
}

bx_local_apic_c::~bx_local_apic_c(void)
{
  // nothing for now
}

void bx_local_apic_c::set_id (Bit8u newid) {
  bx_generic_apic_c::set_id (newid);
  sprintf (cpu->name, "CPU apicid=%02x", (Bit32u)id);
}

char *
bx_local_apic_c::get_name()
{
  return cpu->name;
}

void bx_local_apic_c::write (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  if (bx_dbg.apic)
    bx_printf ("%s: write %08x to APIC address %08x\n", cpu->name, *data, addr);
  //assert (!(addr & 0xf));
  addr &= 0xff0;
  switch (addr) {
    case 0x20: // local APIC id
      id = ((*data)>>24) & 0xf;
      break;
    case 0x80: // task priority
      task_priority = *data & 0xff;
      break;
    case 0xb0: // EOI
      {
	if (bx_dbg.apic)
	  bx_printf ("%s: Wrote 0x%04x to EOI\n", cpu->name, *data);
	int vec = highest_priority_int (isr);
	if (vec < 0) {
	  bx_printf ("EOI written without any bit in ISR\n");
	} else {
	  if (bx_dbg.apic)
	    bx_printf ("%s: local apic received EOI, hopefully for vector 0x%02x\n", cpu->name, vec);
	  isr[vec] = 0; 
	  service_local_apic ();
	}
	if (bx_dbg.apic)
	  print_status ();
      }
      break;
    case 0xd0: // logical destination
      log_dest = (*data >> 24) & 0xff;
      break;
    case 0xe0: // destination format
      dest_format = (*data >> 28) & 0xf;
      break;
    case 0xf0: // spurious interrupt vector
      spurious_vec = (spurious_vec & 0x0f) | (*data & 0x3f0);
      break;
    case 0x280: // error status reg
      // Here's what the IA-devguide-3 says on p.7-45:
      // The ESR is a read/write register and is reset after being written to
      // by the processor. A write to the ESR must be done just prior to
      // reading the ESR to allow the register to be updated.
      // This doesn't seem clear.  If the write clears the register, then
      // wouldn't you always read zero?  Otherwise, what does the write do?
      err_status = 0;
      break;
    case 0x300: // interrupt command reg 0-31
      {
	icr_low = *data & ~(1<<12);  // force delivery status bit = 0 (idle)
	int dest = (icr_high >> 24) & 0xff;
	int trig_mode = (icr_low >> 15) & 1;
	int level = (icr_low >> 14) & 1;
	int dest_mode = (icr_low >> 11) & 1;
	int delivery_mode = (icr_low >> 8) & 7;
	int vector = (icr_low & 0xff);
	//
	// deliver will call get_delivery_bitmask to decide who to send to.
	// This local_apic class redefines get_delivery_bitmask to 
	// implement the destination shorthand field, which doesn't exist
	// for all APICs.
	Boolean accepted = 
	   deliver (dest, dest_mode, delivery_mode, vector, level, trig_mode);
	if (!accepted)
	  err_status |= APIC_ERR_TX_ACCEPT_ERR;
      }
      break;
    case 0x310: // interrupt command reg 31-63
      icr_high = *data & 0xff000000;
      break;
    case 0x320: // LVT Timer Reg
      lvt_timer = *data;  //?
      break;
    case 0x330: // LVT Thermal Monitor
      lvt_thermal = *data;  //?
      break;
    case 0x340: // LVT Performance Counter
      lvt_perf = *data;  //?
      break;
    case 0x350: // LVT LINT0 Reg
      lvt_lint0 = *data;  //?
      break;
    case 0x360: // LVT Lint1 Reg
      lvt_lint1 = *data;  //?
      break;
    case 0x370: // LVT Error Reg
      lvt_err = *data;  //?
      break;
    case 0x380: // initial count for timer
      timer_initial = *data;
      break;
    case 0x3e0: // timer divide configuration
      timer_divide = *data;
      break;
    /* all read-only registers go here */
    case 0x30: // local APIC version
    case 0x90: // arbitration priority
    case 0xa0: // processor priority
    // ISRs not writable
    case 0x100: case 0x110: case 0x120: case 0x130:
    case 0x140: case 0x150: case 0x160: case 0x170:
    // TMRs not writable
    case 0x180: case 0x190: case 0x1a0: case 0x1b0:
    case 0x1c0: case 0x1d0: case 0x1e0: case 0x1f0:
    // IRRs not writable
    case 0x200: case 0x210: case 0x220: case 0x230:
    case 0x240: case 0x250: case 0x260: case 0x270:
      // current count for timer
    case 0x390:
      // all read-only registers should fall into this line
      bx_printf ("warning: write to read-only APIC register 0x%02x\n", addr);
      break;
    default:
      err_status |= APIC_ERR_ILLEGAL_ADDR;
      // but for now I want to know about it in case I missed some.
      bx_panic ("APIC register %08x not implemented\n", addr);
  }
}

void bx_local_apic_c::startup_msg (Bit32u vector)
{
  if (cpu->debug_trap & 0x80000000) {
    cpu->debug_trap &= ~0x80000000;
    cpu->eip = 0;
    cpu->load_seg_reg (&cpu->sregs[BX_SEG_REG_CS], vector*0x100);
    bx_printf ("%s started up at 0x%x by APIC\n", cpu->name, cpu->eip);
  } else {
    bx_printf ("%s started up by APIC, but was not halted at the time\n", cpu->name);
  }
}

void bx_local_apic_c::read_aligned (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  *data = 0;  // default value for unimplemented registers
  switch (addr & 0xff0) {
  case 0x20: // local APIC id
    *data = (id) << 24; break;
  case 0x30: // local APIC version
    *data = 0x00170011; break;
  case 0x80: // task priority
    *data = task_priority & 0xff; break;
  case 0x90: // arbitration priority
    *data = get_apr (); break;
  case 0xa0: // processor priority
    *data = get_ppr (); break;
  case 0xb0: // EOI
    bx_panic ("EOI register not writable\n");
    break;
  case 0xd0: // logical destination
    *data = (log_dest & 0xff) << 24; break;
  case 0xe0: // destination format
    *data = ((dest_format & 0xf) << 24) | 0x0fffffff; break;
  case 0xf0: // spurious interrupt vector
    *data = spurious_vec; break;
  // ISRs not writable
  case 0x100: case 0x110: case 0x120: case 0x130:
  case 0x140: case 0x150: case 0x160: case 0x170:
  case 0x180: case 0x190: case 0x1a0: case 0x1b0:
  case 0x1c0: case 0x1d0: case 0x1e0: case 0x1f0:
  case 0x200: case 0x210: case 0x220: case 0x230:
  case 0x240: case 0x250: case 0x260: case 0x270:
    *data = 0;
    bx_printf ("reading ISR,TMR,IRR not implemented\n");
    break;
  case 0x280: // error status reg
    *data = err_status; break;
  case 0x300: // interrupt command reg 0-31
    *data = icr_low; break;
  case 0x310: // interrupt command reg 31-63
    *data = icr_high; break;
  case 0x320: // LVT Timer Reg
    *data = lvt_timer; break;
  case 0x330: // LVT Thermal Monitor
    *data = lvt_thermal; break;
  case 0x340: // LVT Performance Counter
    *data = lvt_perf; break;
  case 0x350: // LVT LINT0 Reg
    *data = lvt_lint0; break;
  case 0x360: // LVT Lint1 Reg
    *data = lvt_lint1; break;
  case 0x370: // LVT Error Reg
    *data = lvt_err; break;
  case 0x380: // initial count for timer
    *data = timer_initial; break;
  case 0x390: // current count for timer
    *data = timer_current; break;
  case 0x3e0: // timer divide configuration
    *data = timer_divide; break;
  default:
    bx_printf ("APIC register %08x not implemented\n", addr);
  }
  if (bx_dbg.apic)
    bx_printf ("%s: read from APIC address %08x = %08x\n", cpu->name, addr, *data);
}

int 
bx_local_apic_c::highest_priority_int (Bit8u *array)
{
  for (int i=0; i<BX_LOCAL_APIC_MAX_INTS; i++)
    if (array[i]) return i;
  return -1;
}

void bx_local_apic_c::service_local_apic ()
{
  if (bx_dbg.apic) {
    bx_printf ("service_local_apic()\n");
    print_status ();
  }
  if (cpu->INTR) return;  // INTR already up; do nothing
  // find first interrupt in irr.
  int first_irr = highest_priority_int (irr);
  int first_isr = highest_priority_int (isr);
  if (first_irr < 0) return;   // no interrupts, leave INTR=0
  if (first_isr >= 0 && first_irr >= first_isr) {
    bx_printf ("local apic (%s): not delivering int%02x because int%02x is in service\n", cpu->name, first_irr, first_isr);
    return;
  }
  // interrupt has appeared in irr.  raise INTR.  When the CPU
  // acknowledges, we will run highest_priority_int again and
  // return it.
  if (bx_dbg.apic)
    bx_printf ("service_local_apic(): setting INTR=1 for vector 0x%02x\n", first_irr);
  cpu->set_INTR (1);
  cpu->int_from_local_apic = 1;
}

void bx_local_apic_c::trigger_irq (unsigned vector, unsigned from)
{
  if (bx_dbg.apic)
    bx_printf ("Local apic on %s: trigger interrupt vector=0x%x\n", cpu->name, vector);
  irr[vector] = 1;
  service_local_apic ();
}

void bx_local_apic_c::untrigger_irq (unsigned vector, unsigned from)
{
  if (bx_dbg.apic)
    bx_printf ("Local apic on %s: untrigger interrupt vector=0x%x\n", cpu->name, vector);
  // hardware says "no more".  clear the bit.  If the CPU hasn't yet
  // acknowledged the interrupt, it will never be serviced.
  bx_assert (irr[vector] == 1);
  irr[vector] = 0;
  if (bx_dbg.apic) print_status ();
}

Bit8u
bx_local_apic_c::acknowledge_int ()
{
  // CPU calls this when it is ready to service one interrupt
  if (!cpu->INTR)
    bx_panic ("%s: acknowledged an interrupt, but INTR=0\n", cpu->name);
  bx_assert (cpu->int_from_local_apic);
  int vector = highest_priority_int (irr);
  bx_assert (irr[vector] == 1);
  if (bx_dbg.apic)
    bx_printf ("%s: acknowledge_int returning vector 0x%x\n", cpu->name, vector);
  // currently isr never gets cleared, so no point
  //bx_assert (isr[vector] == 0);
  irr[vector] = 0;
  isr[vector] = 1;
  if (bx_dbg.apic) {
    bx_printf ("Status after setting isr:\n");
    print_status ();
  }
  cpu->INTR = 0;
  cpu->int_from_local_apic = 0;
  service_local_apic ();  // will set INTR again if another is ready
  return vector;
}

void bx_local_apic_c::print_status () {
  bx_printf ("%s local apic: status is {:\n", cpu->name);
  for (int vec=0; vec<BX_LOCAL_APIC_MAX_INTS; vec++) {
    if (irr[vec] || isr[vec]) {
      bx_printf ("vec 0x%x: irr=%d, isr=%d\n", vec, (int)irr[vec], (int)isr[vec]);
    }
  }
  bx_printf ("}\n", cpu->name);
}

Boolean bx_local_apic_c::match_logical_addr (Bit8u address) 
{
  if (dest_format != 0xf) {
    bx_panic ("bx_local_apic_c::match_logical_addr: cluster model addressing not implemented");
  }
  // if all address bits are 1, send to all local APICs. SDG3:7-27.
  if (address == 0xff) {
    if (bx_dbg.apic) bx_printf ("%s: MDA=0xff matches everybody\n", cpu->name);
    return true;
  }
  Boolean match = ((address & log_dest) != 0);
  if (bx_dbg.apic) {
    bx_printf ("%s: comparing MDA %02x to my LDR %02x -> %s\n", cpu->name,
      address, log_dest, match? "Match" : "Not a match");
  }
  return match;
}

Bit32u 
bx_local_apic_c::get_delivery_bitmask (Bit8u dest, Bit8u dest_mode)
{
  int dest_shorthand = (icr_low >> 18) & 3;
  Bit32u all_mask = (1<<APIC_MAX_ID) - 1;
  Bit32u mask;
  switch (dest_shorthand) {
  case 0:  // no shorthand, use real destination value
    return bx_generic_apic_c::get_delivery_bitmask (dest, dest_mode);
  case 1:  // self
    return (1<<id);
  case 2:  // all including self
    mask = all_mask;
  case 3:  // all but self
    mask = all_mask & ~(1<<id);
  }
  // prune nonexistents and I/O apics from list
  for (int bit=0; bit<APIC_MAX_ID; bit++) {
    if (!apic_index[bit] 
	|| (apic_index[bit]->get_type () != APIC_TYPE_LOCAL_APIC))
      mask &= ~(1<<bit);
  }
  if (bx_dbg.apic)
    bx_printf ("local::get_delivery_bitmask returning 0x%04x\n", mask);
  return mask;
}

Bit8u bx_local_apic_c::get_ppr ()
{
  bx_printf ("WARNING: Local APIC Processor Priority not implemented, returning 0\n");
  // should look at TPR, vector of highest priority isr, etc.
  return 0;
}


Bit8u bx_local_apic_c::get_apr ()
{
  bx_printf ("WARNING: Local APIC Arbitration Priority not implemented, returning 0\n");
  // should look at TPR, vector of highest priority isr, etc.
  return 0;
}

