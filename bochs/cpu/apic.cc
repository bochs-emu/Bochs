#define BX_IN_CPU_METHOD 1
#include "bochs.h"
#include <assert.h>

bx_generic_apic_c *apic_index[APIC_MAX_ID];

bx_generic_apic_c::bx_generic_apic_c () 
{
}

bx_generic_apic_c::~bx_generic_apic_c () 
{
}

void bx_generic_apic_c::init ()
{
  id = APIC_UNKNOWN_ID;
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
  bx_panic ("get_name called on bx_generic_apic_c base class");
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
  bx_panic ("untrigger_irq called on base class");
}

bx_local_apic_c::bx_local_apic_c(BX_CPU_C *mycpu)
{
  cpu = mycpu;
}

void
bx_local_apic_c::init ()
{
  // default address for a local APIC, can be moved
  bx_generic_apic_c::init ();
  base_addr = 0xfee00000;
  err_status = 0;
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
  bx_printf ("write %08x to APIC address %08x\n", *data, addr);
  //assert (!(addr & 0xf));
  addr &= 0xff0;
  switch (addr) {
    case 0x20: // local APIC id
      id = ((*data)>>24) & 0xf;
      break;
    case 0x80: // task priority
      tpr = *data & ~0xff;  // low 8 bits reserved.
      break;
    case 0xb0: // EOI
      // not readable
      bx_printf ("%s: Wrote 0x%04x to EOI\n", cpu->name);
      break;
    case 0xd0: // logical destination
      log_dest = *data;
      break;
    case 0xe0: // destination format
      dest_format = (dest_format & 0x0fffffff) | (*data & 0xf0000000);
      break;
    case 0xf0: // spurious interrupt vector
      spurious_vec = (spurious_vec & 0x0f) | (*data & 0x3f0);
      break;
    case 0x280: // error status reg
      // Here's what the IA-devguide-3 says on p.7-45:
      // The ESR is a read/write register and is reset after being written to by
      // the processor. A write to the ESR must be done just prior to reading the
      // ESR to allow the register to be updated.
      // 
      // This doesn't seem clear.  If the write clears the register, then
      // wouldn't you always read zero?  Otherwise, what does the write do?
      // In my model, the write will do nothing.
      break;
    case 0x300: // interrupt command reg 0-31
      {
	icr_low = *data & ~(1<<12);  // force delivery status bit = 0 (idle)
	// and trigger an interrupt to be sent
	int dest_shorthand = (icr_low >> 18) & 3;
	char *dests[4] = {NULL, "self", "all including self", "all except self" };
	char buf[32];
	unsigned int target_id = (icr_high >> 24);
	sprintf (buf, "APIC 0x%02x\n", target_id);
	bx_printf ("APIC 0x%02x sending interrupt to destination %s",
	   id,
	   (dest_shorthand==0) ? buf : dests[dest_shorthand]);
	bx_printf ("low word of APIC 0x%02x ICR = 0x%04x\n", id, icr_low & 0xffff);
	int delivery_mode = (icr_low >> 8) & 7;
	int vector = (icr_low & 0xff);
	if (delivery_mode == 6 && dest_shorthand == 0) {
	  // tell target to start up
	  if (target_id > APIC_MAX_ID) bx_panic ("target apic id out of range\n");
	  bx_generic_apic_c *target = apic_index[target_id];
	  if (target == NULL) bx_panic ("apic target id not found\n");
	  target->startup_msg (vector);
	} else {
	    bx_printf ("APIC operation not supported\n");
	}
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
    *data = tpr; break;
  case 0x90: // arbitration priority
    *data = arb_priority; break;
  case 0xa0: // processor priority
    *data = proc_priority; break;
  case 0xb0: // EOI
    bx_panic ("EOI register not writable\n");
    break;
  case 0xd0: // logical destination
    *data = log_dest; break;
  case 0xe0: // destination format
    *data = dest_format; break;
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
    bx_printf ("reading ISR,TMR,IRR not implemented");
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
  bx_printf ("read from APIC address %08x = %08x\n", addr, *data);
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
  if (cpu->INTR) return;  // INTR already up; do nothing
  // find first interrupt in irr.
  int first_int = highest_priority_int (irr);
  if (first_int < 0) return;   // no interrupts, leave INTR=0
  // interrupt has appeared in irr.  raise INTR.  When the CPU
  // acknowledges, we will run highest_priority_int again and
  // return it.
  cpu->set_INTR (1);
}

void bx_local_apic_c::trigger_irq (unsigned vector, unsigned from)
{
  bx_printf ("Local apic on %s: trigger interrupt %d\n", cpu->name, vector);
  irr[vector] = 1;
  service_local_apic ();
}

void bx_local_apic_c::untrigger_irq (unsigned vector, unsigned from)
{
  bx_printf ("Local apic on %s: untrigger interrupt %d\n", cpu->name, vector);
  // hardware says "no more".  clear the bit.  If the CPU hasn't yet
  // acknowledged the interrupt, it will never be serviced.
  bx_assert (irr[vector] == 1);
  irr[vector] = 0;
}

Bit8u
bx_local_apic_c::acknowledge_int ()
{
  // CPU calls this when it is ready to service one interrupt
  if (!cpu->INTR)
    bx_panic ("%s acknowledged an interrupt, but INTR=0\n", cpu->name);
  int vector = highest_priority_int (irr);
  bx_assert (irr[vector] == 1);
  // currently isr never gets cleared, so no point
  //bx_assert (isr[vector] == 0);
  irr[vector] = 0;
  isr[vector] = 1;
  cpu->INTR = 0;
  service_local_apic ();  // will set INTR again if another is ready
  return vector;
}
