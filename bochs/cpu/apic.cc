#define BX_IN_CPU_METHOD 1
#include "bochs.h"
#include <assert.h>

bx_apic_c::bx_apic_c(void)
{
  static int apic_ids_used = 1;
  // default address for a local APIC, can be moved
  apic_base_msr = 0xfee00000;
  id = apic_ids_used++;
  err_status = 0;
}

bx_apic_c::~bx_apic_c(void)
{
  // nothing for now
}

  void
bx_apic_c::init()
{
}

Bit32u bx_apic_c::get_base (void)
{
  return apic_base_msr;
}

void bx_apic_c::set_base (Bit32u newbase) 
{
  bx_printf ("relocate APIC to %8x\n", newbase);
  apic_base_msr = newbase;
}

void bx_apic_c::write_handler (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  bx_printf ("write %08x to APIC address %08x\n", *data, addr);
  //assert (!(addr & 0xf));
  switch (addr & 0xff0) {
  case 0x20: // local APIC id
    id = ((*data)>>24) & 0xf;
    break;
  case 0x30: // local APIC version
    bx_printf ("warning: write to read-only APIC register\n");
    break;
  case 0x80: // task priority
  case 0x90: // arbitration priority
  case 0xa0: // processor priority
  case 0xb0: // EOI
  case 0xd0: // logical destination
  case 0xe0: // destination format
  case 0xf0: // spurious interrupt vector
    break;
  case 0x280: // error status reg
    // Here's what the IA-devguide-3 says on p.7-45:
    // The ESR is a read/write register and is reset after being written to by
    // the processor. A write to the ESR must be done just prior to reading the
    // ESR to allow the register to be updated.
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
    sprintf (buf, "APIC 0x%02x\n", icr_high >> 24);
    bx_printf ("APIC 0x%02x sending interrupt to destination %s",
       id,
       (dest_shorthand==0) ? buf : dests[dest_shorthand]);
    bx_printf ("low word of APIC 0x%02x ICR = 0x%04x\n", id, icr_low & 0xffff);
    }
    break;
  case 0x310: // interrupt command reg 31-63
    icr_high = *data & 0xff000000;
    break;
  case 0x320: // LVT Timer Reg
  case 0x330: // LVT Thermal Monitor
  case 0x340: // LVT Performance Counter
  case 0x350: // LVT LINT0 Reg
  case 0x360: // LVT Lint1 Reg
  case 0x370: // LVT Error Reg
  case 0x380: // initial count for timer
  case 0x390: // current count for timer
  case 0x3e0: // timer divide configuration
  default:
    bx_printf ("APIC register %08x not implemented\n", addr);
  }
}

void bx_apic_c::read_handler (Bit32u addr, Bit32u *data, unsigned len)
{
  assert (len == 4);
  *data = 0;  // default value for unimplemented registers
  switch (addr & 0xff0) {
  case 0x20: // local APIC id
    *data = (id) << 24;
    break;
  case 0x30: // local APIC version
    *data = 0x00170011;
    break;
  case 0x80: // task priority
  case 0x90: // arbitration priority
  case 0xa0: // processor priority
  case 0xb0: // EOI
  case 0xd0: // logical destination
  case 0xe0: // destination format
  case 0xf0: // spurious interrupt vector
    break;
  case 0x280: // error status reg
    *data = err_status;
    break;
  case 0x300: // interrupt command reg 0-31
    *data = icr_low;
    break;
  case 0x310: // interrupt command reg 31-63
    *data = icr_high;
    break;
  case 0x320: // LVT Timer Reg
  case 0x330: // LVT Thermal Monitor
  case 0x340: // LVT Performance Counter
  case 0x350: // LVT LINT0 Reg
  case 0x360: // LVT Lint1 Reg
  case 0x370: // LVT Error Reg
  case 0x380: // initial count for timer
  case 0x390: // current count for timer
  case 0x3e0: // timer divide configuration
  default:
    bx_printf ("APIC register %08x not implemented\n", addr);
  }
  bx_printf ("read from APIC address %08x = %08x\n", addr, *data);
}
