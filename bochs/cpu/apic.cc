/////////////////////////////////////////////////////////////////////////
// $Id: apic.cc,v 1.13 2002-03-27 03:47:45 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include <assert.h>

#define LOG_THIS this->

bx_generic_apic_c *apic_index[APIC_MAX_ID];

bx_generic_apic_c::bx_generic_apic_c () 
{
  id = APIC_UNKNOWN_ID;
  put("APIC?");
  settype(APICLOG);
  hwreset ();
}

bx_generic_apic_c::~bx_generic_apic_c () 
{
}

void
bx_generic_apic_c::set_arb_id (int new_arb_id)
{
  // politely ignore it.  This gets sent to every APIC, regardless of its
  // type.
}

// init is called during RESET and when an INIT message is delivered.
void bx_generic_apic_c::init ()
{
}

void bx_generic_apic_c::set_base (Bit32u newbase)
{
  BX_INFO(("relocate APIC id=%d to %8x", id, newbase));
  base_addr = newbase;
}

void bx_generic_apic_c::set_id (Bit8u newid) {
  // update apic_index
  if (id != APIC_UNKNOWN_ID) {
    BX_ASSERT (id < APIC_MAX_ID);
    if (apic_index[id] != this)
      BX_PANIC(("inconsistent APIC id table"));
    apic_index[id] = NULL;
  }
  id = newid;
  if (apic_index[id] != NULL)
    BX_PANIC(("duplicate APIC id assigned"));
  apic_index[id] = this;
}

char *
bx_generic_apic_c::get_name () {
  BX_PANIC(("get_name called on bx_generic_apic_c base class"));
  return NULL;
}

Boolean
bx_generic_apic_c::is_selected (Bit32u addr, Bit32u len)
{
  if ((addr & ~0xfff) == get_base ()) {
    if ((addr & 0xf != 0) || (len != 4))
      BX_INFO(("warning: misaligned or wrong-size APIC write"));
    return true;
  }
  return false;
}

void
bx_generic_apic_c::read (Bit32u addr, void *data, unsigned len)
{
  if ((addr & ~0xf) != ((addr+len-1) & ~0xf))
    BX_PANIC(("APIC read spans 32-bit boundary"));
  Bit32u value;
  read_aligned (addr, &value, 4);
  if ((addr&3) == 0) {
    *((Bit32u *)data) = value;
    return;
  }
  // handle partial word read, independent of endian-ness.
  Bit8u bytes[4];
  bytes[0] = value & 0xff;
  bytes[1] = (value >> 8) & 0xff;
  bytes[2] = (value >> 16) & 0xff;
  bytes[3] = (value >> 24) & 0xff;
  Bit8u *p1 = bytes+(addr&3);
  Bit8u *p2 = (Bit8u *)data;
  for (int i=0; i<len; i++) {
    if (bx_dbg.apic)
      BX_INFO(("apic: Copying byte %02x", (unsigned int) *p1));
    *p2++ = *p1++;
  }
}

void bx_generic_apic_c::read_aligned (Bit32u address, Bit32u *data, unsigned len)
{
  BX_PANIC(("read_aligned not implemented in base class bx_generic_apic_c"));
}

void bx_generic_apic_c::write(Bit32u address, Bit32u *value, unsigned len)
{
  BX_PANIC(("write not implemented in base class bx_generic_apic_c"));
}

void bx_generic_apic_c::startup_msg (Bit32u vector)
{
  BX_PANIC(("startup message sent to an I/O APIC"));
}

void bx_generic_apic_c::trigger_irq (unsigned num, unsigned from)
{
  BX_PANIC(("trigger_irq called on base class"));
}

void bx_generic_apic_c::untrigger_irq (unsigned num, unsigned from)
{
  BX_PANIC(("untrigger_irq called on base class"));
}

Boolean bx_generic_apic_c::match_logical_addr (Bit8u address) {
  BX_PANIC(("match_logical_addr called on base class"));
  return false;
}

bx_apic_type_t bx_generic_apic_c::get_type () {
  BX_PANIC(("get_type called on base class"));
  return APIC_TYPE_NONE;
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
      BX_ASSERT (get_type () == APIC_TYPE_LOCAL_APIC);
      mask = 0xff; 
    } else BX_PANIC(("bx_generic_apic_c::deliver: illegal physical destination %02x", dest));
  } else {
    // logical destination. call match_logical_addr for each APIC.
    if (dest == 0) return 0;
    for (int i=0; i<APIC_MAX_ID; i++) {
      if (apic_index[i] && apic_index[i]->match_logical_addr(dest))
	mask |= (1<<i);
    }
  }
  if (bx_dbg.apic)
    BX_INFO(("generic::get_delivery_bitmask returning 0x%04x", mask));
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
    BX_PANIC(("deliver failed for vector %02x: no APICs in destination bitmask", vector));
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
	BX_ASSERT (deliver_bitmask >= 0);
      }
      break;
    case 5:  // INIT
      {
	// NOTE: special behavior of local apics is handled in
	// bx_local_apic_c::deliver.
	
	// normal INIT. initialize the local apics in the delivery mask.
	for (int bit=0; bit<APIC_MAX_ID; bit++) {
	  if (deliver_bitmask & (1<<bit)) 
	    apic_index[bit]->init ();
	}
      }
      return true;
    case 6:  // Start Up (local apic only)
      BX_ASSERT (get_type () == APIC_TYPE_LOCAL_APIC);
      for (int bit=0; bit<APIC_MAX_ID; bit++)
	if (deliver_bitmask & (1<<bit))
	  apic_index[bit]->startup_msg (vector);
      return true;
    case 2:  // SMI
    case 3:  // reserved
    case 4:  // NMI
    case 7:  // ExtINT (I/O apic only)
    default:
      BX_PANIC(("APIC delivery mode %d not implemented", delivery_mode));
  }
  // Fixed delivery mode
  if (bx_dbg.apic)
    BX_INFO(("delivering vector=0x%02x to bitmask=%04x", (int)vector, deliver_bitmask));
  for (int bit=0; bit<APIC_MAX_ID; bit++) {
    if (deliver_bitmask & (1<<bit)) {
      if (apic_index[bit] == NULL)
	BX_INFO(("IOAPIC: delivering int0x%x to nonexistent id=%d!", (unsigned)vector, bit));
      else {
        if (bx_dbg.apic)
	  BX_INFO(("IOAPIC: delivering int0x%x to apic#%d", (unsigned)vector, bit));
	apic_index[bit]->trigger_irq (vector, id);
      }
    }
  }
  return true;
}

Boolean
bx_local_apic_c::deliver (Bit8u dest, Bit8u dest_mode, Bit8u delivery_mode, Bit8u vector, Bit8u polarity, Bit8u trig_mode)
{
  // In this function, implement only the behavior that is specific to
  // the local apic.  For general behavior of all apics, just send it to
  // the base class.
  if (delivery_mode == 5)
  {
    int trig_mode = (icr_low >> 15) & 1;
    int level = (icr_low >> 14) & 1;
    if (level == 0 && trig_mode == 1) {
      // special mode in local apic.  See "INIT Level Deassert" in the
      // Intel Soft. Devel. Guide Vol 3, page 7-34.  This magic code
      // causes all APICs (regardless of dest address) to set their
      // arbitration ID to their APIC ID.
      BX_INFO (("INIT with Level&Deassert: synchronize arbitration IDs"));
      for (int bit=0; bit<APIC_MAX_ID; bit++) {
	if (apic_index[bit])
	  apic_index[bit]->set_arb_id (apic_index[bit]->get_id ());
      }
      return true;
    }
  }
  // not any special case behavior, just use generic apic code.
  return bx_generic_apic_c::deliver (dest, dest_mode, delivery_mode, vector, polarity, trig_mode);
}

bx_local_apic_c::bx_local_apic_c(BX_CPU_C *mycpu)
  : bx_generic_apic_c ()
{
  char buffer[16];
  cpu = mycpu;
  hwreset ();
}

void
bx_local_apic_c::set_arb_id (int new_arb_id)
{
  BX_DEBUG (("set arbitration ID to %d", new_arb_id));
  arb_id = new_arb_id;
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
  BX_INFO(("local apic in %s initializing", 
      (cpu && cpu->name) ? cpu->name : "?"));
  // default address for a local APIC, can be moved
  base_addr = 0xfee00000;
  err_status = 0;
  log_dest = 0;
  dest_format = 0xf;
  for (int bit=0; bit<BX_LOCAL_APIC_MAX_INTS; bit++) {
    irr[bit] = isr[bit] = tmr[bit] = 0;
  }
  icr_high = icr_low = log_dest = task_priority = 0;
  spurious_vec = 0xff;   // software disabled (bit 8)
}

BX_CPU_C 
*bx_local_apic_c::get_cpu (Bit8u id)
{
  BX_ASSERT (id < APIC_MAX_ID);
  return cpu;
}

bx_local_apic_c::~bx_local_apic_c(void)
{
  // nothing for now
}

void bx_local_apic_c::set_id (Bit8u newid) {
  bx_generic_apic_c::set_id (newid);
  sprintf (cpu->name, "CPU apicid=%02x", (Bit32u)id);
  if (id >= 0 && id <= 15) {
    char buffer[16];
    sprintf (buffer, "APIC%x", id);
    put(buffer);
    settype(CPU0LOG + id);
    sprintf (buffer, "CPU%x", id);
    cpu->put (buffer);
  } else {
    BX_INFO (("naming convention for apics requires id=0-15 only"));
  }
  if(BX_CPU_LEVEL<2)
    BX_INFO(( "8086" ));
  else
    BX_INFO(( "80%d86", BX_CPU_LEVEL ));
}

char *
bx_local_apic_c::get_name()
{
  return cpu->name;
}

void bx_local_apic_c::set_divide_configuration (Bit32u value) {
  BX_ASSERT (value == (value & 0x0b));
  // move bit 3 down to bit 0.
  value = ((value & 8) >> 1) | (value & 3);
  BX_ASSERT (value >= 0 && value <= 7);
  timer_divide_factor = (value==7)? 1 : (2 << value);
  BX_DEBUG(("%s: set timer divide factor to %d", cpu->name, timer_divide_factor));
}

void bx_local_apic_c::write (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  BX_DEBUG(("%s: write %08x to APIC address %08x", cpu->name, *data, addr));
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
	BX_DEBUG(("%s: Wrote 0x%04x to EOI", cpu->name, *data));
	int vec = highest_priority_int (isr);
	if (vec < 0) {
	  BX_INFO(("EOI written without any bit in ISR"));
	} else {
	  BX_DEBUG(("%s: local apic received EOI, hopefully for vector 0x%02x", cpu->name, vec));
	  isr[vec] = 0; 
	  service_local_apic ();
	}
	if (bx_dbg.apic)
	  print_status ();
      }
      break;
    case 0xd0: // logical destination
      log_dest = (*data >> 24) & 0xff;
      BX_DEBUG (("set logical destiation to %02x", log_dest));
      break;
    case 0xe0: // destination format
      dest_format = (*data >> 28) & 0xf;
      BX_DEBUG (("set destination format to %02x", dest_format));
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
      lvt[APIC_LVT_TIMER] = *data & 0x310ff;
      break;
    case 0x330: // LVT Thermal Monitor
      lvt[APIC_LVT_THERMAL] = *data & 0x117ff;
      break;
    case 0x340: // LVT Performance Counter
      lvt[APIC_LVT_PERFORM] = *data & 0x117ff;
      break;
    case 0x350: // LVT LINT0 Reg
      lvt[APIC_LVT_LINT0] = *data & 0x1f7ff;
      break;
    case 0x360: // LVT Lint1 Reg
      lvt[APIC_LVT_LINT1] = *data & 0x1f7ff;
      break;
    case 0x370: // LVT Error Reg
      lvt[APIC_LVT_ERROR] = *data & 0x117ff;
      break;
    case 0x380: // initial count for timer
      timer_initial = *data;
      // This should trigger the counter to start.  If already started,
      // restart from the new start value.
      timer_current = timer_initial;
      timer_active = true;
      timer_divide_counter = 0;
      break;
    case 0x3e0: // timer divide configuration
      // only bits 3, 1, and 0 are writable
      timer_divconf = *data & 0xb;
      set_divide_configuration (timer_divconf);
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
      BX_INFO(("warning: write to read-only APIC register 0x%02x", addr));
      break;
    default:
      err_status |= APIC_ERR_ILLEGAL_ADDR;
      // but for now I want to know about it in case I missed some.
      BX_PANIC(("APIC register %08x not implemented", addr));
  }
}

void bx_local_apic_c::startup_msg (Bit32u vector)
{
  if (cpu->debug_trap & 0x80000000) {
    cpu->debug_trap &= ~0x80000000;
    cpu->eip = 0;
    cpu->load_seg_reg (&cpu->sregs[BX_SEG_REG_CS], vector*0x100);
    BX_INFO(("%s started up at 0x%x by APIC", cpu->name, cpu->eip));
  } else {
    BX_INFO(("%s started up by APIC, but was not halted at the time", cpu->name));
  }
}

void bx_local_apic_c::read_aligned (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  *data = 0;  // default value for unimplemented registers
  Bit32u addr2 = addr & 0xff0;
  switch (addr2) {
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
    /*
     * Read-modify-write operations should operate without generating
     * exceptions, and are used by some operating systems to EOI.
     * The results of reads should be ignored by the OS.
     */
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
    BX_INFO(("reading ISR,TMR,IRR not implemented"));
    break;
  case 0x280: // error status reg
    *data = err_status; break;
  case 0x300: // interrupt command reg 0-31
    *data = icr_low; break;
  case 0x310: // interrupt command reg 31-63
    *data = icr_high; break;
  case 0x320: // LVT Timer Reg
  case 0x330: // LVT Thermal Monitor
  case 0x340: // LVT Performance Counter
  case 0x350: // LVT LINT0 Reg
  case 0x360: // LVT Lint1 Reg
  case 0x370: // LVT Error Reg
    {
      int index = (addr2 - 0x320) >> 4;
      *data = lvt[index];
      break;
    }
  case 0x380: // initial count for timer
    *data = timer_initial; break;
  case 0x390: // current count for timer
    *data = timer_current; break;
  case 0x3e0: // timer divide configuration
    *data = timer_divconf; break;
  default:
    BX_INFO(("APIC register %08x not implemented", addr));
  }
  if (bx_dbg.apic)
    BX_INFO(("%s: read from APIC address %08x = %08x", cpu->name, addr, *data));
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
    BX_INFO(("service_local_apic()"));
    print_status ();
  }
  if (cpu->INTR) return;  // INTR already up; do nothing
  // find first interrupt in irr.
  int first_irr = highest_priority_int (irr);
  int first_isr = highest_priority_int (isr);
  if (first_irr < 0) return;   // no interrupts, leave INTR=0
  if (first_isr >= 0 && first_irr >= first_isr) {
    if (bx_dbg.apic)
      BX_INFO(("local apic (%s): not delivering int%02x because int%02x is in service", cpu->name, first_irr, first_isr));
    return;
  }
  // interrupt has appeared in irr.  raise INTR.  When the CPU
  // acknowledges, we will run highest_priority_int again and
  // return it.
  BX_DEBUG(("service_local_apic(): setting INTR=1 for vector 0x%02x", first_irr));
  cpu->set_INTR (1);
  cpu->int_from_local_apic = 1;
}

void bx_local_apic_c::trigger_irq (unsigned vector, unsigned from)
{
  BX_DEBUG(("Local apic on %s: trigger interrupt vector=0x%x", cpu->name, vector));
  irr[vector] = 1;
  service_local_apic ();
}

void bx_local_apic_c::untrigger_irq (unsigned vector, unsigned from)
{
  BX_DEBUG(("Local apic on %s: untrigger interrupt vector=0x%x", cpu->name, vector));
  // hardware says "no more".  clear the bit.  If the CPU hasn't yet
  // acknowledged the interrupt, it will never be serviced.
  BX_ASSERT (irr[vector] == 1);
  irr[vector] = 0;
  if (bx_dbg.apic) print_status ();
}

Bit8u
bx_local_apic_c::acknowledge_int ()
{
  // CPU calls this when it is ready to service one interrupt
  if (!cpu->INTR)
    BX_PANIC(("%s: acknowledged an interrupt, but INTR=0", cpu->name));
  BX_ASSERT (cpu->int_from_local_apic);
  int vector = highest_priority_int (irr);
  BX_ASSERT (irr[vector] == 1);
  BX_DEBUG(("%s: acknowledge_int returning vector 0x%x", cpu->name, vector));
  // currently isr never gets cleared, so no point
  //BX_ASSERT (isr[vector] == 0);
  irr[vector] = 0;
  isr[vector] = 1;
  if (bx_dbg.apic) {
    BX_INFO(("Status after setting isr:"));
    print_status ();
  }
  cpu->INTR = 0;
  cpu->int_from_local_apic = 0;
  service_local_apic ();  // will set INTR again if another is ready
  return vector;
}

void bx_local_apic_c::print_status () {
  BX_INFO(("%s local apic: status is {:", cpu->name));
  for (int vec=0; vec<BX_LOCAL_APIC_MAX_INTS; vec++) {
    if (irr[vec] || isr[vec]) {
      BX_INFO(("vec 0x%x: irr=%d, isr=%d", vec, (int)irr[vec], (int)isr[vec]));
    }
  }
  BX_INFO(("}", cpu->name));
}

Boolean bx_local_apic_c::match_logical_addr (Bit8u address) 
{
  if (dest_format != 0xf) {
    BX_PANIC(("bx_local_apic_c::match_logical_addr: cluster model addressing not implemented"));
  }
  // if all address bits are 1, send to all local APICs. SDG3:7-27.
  if (address == 0xff) {
    BX_DEBUG (("%s: MDA=0xff matches everybody", cpu->name));
    return true;
  }
  Boolean match = ((address & log_dest) != 0);
  BX_DEBUG (("%s: comparing MDA %02x to my LDR %02x -> %s", cpu->name,
    address, log_dest, match? "Match" : "Not a match"));
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
  BX_DEBUG (("local::get_delivery_bitmask returning 0x%04x", mask));
  return mask;
}

Bit8u bx_local_apic_c::get_ppr ()
{
  static int warned = 0;
  if (warned < 10) {
    BX_ERROR(("WARNING: Local APIC Processor Priority not implemented, returning 0"));
    warned++;
  }
  // should look at TPR, vector of highest priority isr, etc.
  return 0;
}


Bit8u bx_local_apic_c::get_apr ()
{
  return arb_id;
}

void
bx_local_apic_c::periodic (Bit32u usec_delta)
{
  if (!timer_active) return;
  BX_DEBUG(("%s: bx_local_apic_c::periodic called with %d usec",
    cpu->name, usec_delta));
  // unless usec_delta is guaranteed to be a multiple of 128, I can't
  // just divide usec_delta by the divide-down value.  Instead, it will
  // have a similar effect to implement the divide-down by ignoring
  // some fraction of calls to this function.  This can be improved if
  // more granularity is important.
  timer_divide_counter = (timer_divide_counter + 1) % timer_divide_factor;
  if (timer_divide_counter != 0) return;
  if (timer_current > usec_delta) {
    timer_current -= usec_delta;
    //BX_INFO(("%s: local apic timer is now 0x%08x", cpu->name, timer_current));
    return;
  }
  // timer reached zero since the last call to periodic.
  Bit32u timervec = lvt[APIC_LVT_TIMER];
  if (timervec & 0x20000) {
    // periodic mode.  Always trigger the interrupt when we reach zero.
    trigger_irq (timervec & 0xff, id);
    if (timer_initial == 0) {
      usec_delta = 0;
      timer_current = 0;
    } else {
      // timer_initial might be smaller than usec_delta.  I can't trigger
      // multiple interrupts, so just try to get the timer_current right.
      while (usec_delta > timer_initial)
	usec_delta -= timer_initial;
      timer_current = timer_current + timer_initial - usec_delta;
      // sanity check. all these are unsigned so I can't check for
      // negative timer_current.
      BX_ASSERT ((timer_current + timer_initial) >= usec_delta);
    }
    BX_DEBUG(("%s: local apic timer (periodic) triggered int, reset counter to 0x%08x", cpu->name, timer_current));
  } else {
    // one-shot mode
    timer_current = 0;
    if (timer_active) {
      trigger_irq (timervec & 0xff, id);
      timer_active = false;
      BX_DEBUG (("%s: local apic timer (one-shot) triggered int", cpu->name));
    }
  }
}
