/////////////////////////////////////////////////////////////////////////
// $Id: pcipnic.cc,v 1.4 2004-03-26 03:22:46 mcb30 Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003  Fen Systems Ltd.
//  http://www.fensystems.co.uk/
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_PCI_SUPPORT && BX_PCI_PNIC_SUPPORT

#define LOG_THIS thePNICDevice->

bx_pcipnic_c* thePNICDevice = NULL;

  int
libpcipnic_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePNICDevice = new bx_pcipnic_c ();
  bx_devices.pluginPciPNicAdapter = thePNICDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePNICDevice, BX_PLUGIN_PCIPNIC);
  return 0; // Success
}

  void
libpcipnic_LTX_plugin_fini(void)
{
}


bx_pcipnic_c::bx_pcipnic_c(void)
{
  put("PNIC");
  settype(PCIPNICLOG);
}

bx_pcipnic_c::~bx_pcipnic_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pcipnic_c::init(void)
{
  // called once when bochs initializes

  if (!bx_options.pnic.Oenabled->get()) return;

  Bit16u base_ioaddr = bx_options.pnic.Oioaddr->get();
  Bit8u irq = bx_options.pnic.Oirq->get();
  memcpy ( BX_PNIC_THIS s.macaddr, bx_options.pnic.Omacaddr->getptr(),
	   sizeof( BX_PNIC_THIS s.macaddr ) );

  DEV_register_irq(irq, "PNIC");
  BX_PNIC_THIS s.irq = irq;

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  /*  BX_PNIC_THIS s.timer_index =
      bx_pc_system.register_timer(this, pnic_timer_handler, 1000, 1,1, "pnic.timer"); */

  for ( unsigned addr = base_ioaddr;
        addr <= (unsigned)( base_ioaddr + PNIC_MAX_REG );
	addr++ ) {
    BX_DEBUG(("register read/write: 0x%04x", addr));
    DEV_register_ioread_handler(this, read_handler, addr, "PNIC", 7);
    DEV_register_iowrite_handler(this, write_handler, addr, "PNIC", 7);
  }
  BX_PNIC_THIS s.base_ioaddr = base_ioaddr;

  DEV_register_pci_handlers(this,
                            pci_read_handler,
                            pci_write_handler,
                            BX_PCI_DEVICE(1,2),
                            "Experimental PCI Pseudo NIC");

  for (unsigned i=0; i<256; i++) {
    BX_PNIC_THIS s.pci_conf[i] = 0x0;
  }

  // This code ripped wholesale from ne2k.cc:
  // Attach to the simulated ethernet dev
  char *ethmod = bx_options.pnic.Oethmod->get_choice(bx_options.pnic.Oethmod->get());
  // 
  // FIXME: eth_tuntap.cc rips script straight from ne2k options...
  //
  bx_options.ne2k.Oscript->set ( bx_options.pnic.Oscript->getptr() );
#if BX_PLUGINS
  plugin_t *anIter;
  for (anIter = plugins; (anIter != NULL) && strcmp(anIter->name, "ne2k"); anIter = anIter->next);
  BX_ASSERT(anIter != NULL);
  BX_PNIC_THIS ethdev = (*(eth_pktmover_c *(*)
                           (const char *, const char *, const char *, eth_rx_handler_t, void *))
                        lt_dlsym(anIter->handle, "_ZN13eth_locator_c6createEPKcS1_S1_PFvPvPKvjES2_"))
                                             (ethmod,
                                              bx_options.pnic.Oethdev->getptr (),
                                              (const char *) bx_options.pnic.Omacaddr->getptr (),
                                              rx_handler, 
                                              this);
#else
  BX_PNIC_THIS ethdev = eth_locator_c::create(ethmod,
                                              bx_options.pnic.Oethdev->getptr (),
                                              (const char *) bx_options.pnic.Omacaddr->getptr (),
                                              rx_handler, 
                                              this);
#endif

  if (BX_PNIC_THIS ethdev == NULL) {
    BX_PANIC(("could not find eth module %s", ethmod));
    // if they continue, use null.
    BX_INFO(("could not find eth module %s - using null instead", ethmod));

#if BX_PLUGINS
  BX_PNIC_THIS ethdev = (*(eth_pktmover_c *(*)
                           (const char *, const char *, const char *, eth_rx_handler_t, void *))
                        lt_dlsym(anIter->handle, "_ZN13eth_locator_c6createEPKcS1_S1_PFvPvPKvjES2_"))
                                               ("null", NULL,
                                                (const char *) bx_options.pnic.Omacaddr->getptr (),
                                                rx_handler, 
                                                this);
#else
    BX_PNIC_THIS ethdev = eth_locator_c::create("null", NULL,
                                                (const char *) bx_options.pnic.Omacaddr->getptr (),
                                                rx_handler, 
                                                this);
#endif
    if (BX_PNIC_THIS ethdev == NULL)
      BX_PANIC(("could not locate null module"));
  }

  BX_INFO( ( "pnic at 0x%04x-0x%04x irq %d",
	     base_ioaddr, base_ioaddr + PNIC_MAX_REG, irq ) );
}

void
bx_pcipnic_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x00, PNIC_PCI_VENDOR & 0xff },
    { 0x01, PNIC_PCI_VENDOR >> 8 },
    { 0x02, PNIC_PCI_DEVICE & 0xff },
    { 0x03, PNIC_PCI_DEVICE >> 8 },
    { 0x04, 0x05 }, { 0x05, 0x00 },	// command_io
    { 0x06, 0x80 }, { 0x07, 0x02 },	// status
    { 0x08, 0x01 },                 // revision number
    { 0x09, 0x00 },                 // interface
    { 0x0a, 0x00 },                 // class_sub
    { 0x0b, 0x02 },                 // class_base Network Controller
    { 0x0D, 0x20 },                 // bus latency
    { 0x0e, 0x00 },                 // header_type_generic
    // address space 0x20 - 0x23
    { 0x20, (( bx_options.pnic.Oioaddr->get() & 0xE0) | 0x01) },
    { 0x21, ( bx_options.pnic.Oioaddr->get() >> 8) },
    { 0x22, 0x00 }, { 0x23, 0x00 },
    { 0x3c, bx_options.pnic.Oirq->get() }, // IRQ
    { 0x3d, 0x04 },                 // INT
    { 0x6a, 0x01 },                 // PNIC clock
    { 0xc1, 0x20 }                  // PIRQ enable

  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_PNIC_THIS s.pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  // Set up initial register values
  BX_PNIC_THIS s.rCmd = PNIC_CMD_NOOP;
  BX_PNIC_THIS s.rStatus = PNIC_STATUS_OK;
  BX_PNIC_THIS s.rLength = 0;
  BX_PNIC_THIS s.rDataCursor = 0;
  BX_PNIC_THIS s.recvIndex = 0;
  BX_PNIC_THIS s.recvQueueLength = 0;
  BX_PNIC_THIS s.irqEnabled = 0;

  // Deassert IRQ
  DEV_pic_lower_irq ( BX_PNIC_THIS s.irq );
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pcipnic_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pcipnic_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF
  Bit32u val = 0x0;
  Bit8u  offset,port;

  BX_DEBUG(("register read from address 0x%04x - ", (unsigned) address));

  offset = address - BX_PNIC_THIS s.base_ioaddr;

  switch (offset) {
  case PNIC_REG_STAT :
    if (io_len != 2)
      BX_PANIC(("PNIC read from status register, bad i/o length %u",
		(unsigned) io_len ));
    val = BX_PNIC_THIS s.rStatus;
    break;
    
  case PNIC_REG_LEN :
    if (io_len != 2)
      BX_PANIC(("PNIC read from length register, bad i/o length %u",
		(unsigned) io_len ));
    val = BX_PNIC_THIS s.rLength;
    break;
    
  case PNIC_REG_DATA :
    if (io_len != 1)
      BX_PANIC(("PNIC read from data register, bad i/o length %u",
		(unsigned) io_len ));
    if ( BX_PNIC_THIS s.rDataCursor >= BX_PNIC_THIS s.rLength )
      BX_PANIC(("PNIC read at %u, beyond end of data register array",
		BX_PNIC_THIS s.rDataCursor));
    val = BX_PNIC_THIS s.rData[BX_PNIC_THIS s.rDataCursor ++];
    break;
    
  default :
    val = 0; // keep compiler happy
    BX_PANIC(("unsupported io read from address=0x%04x!", (unsigned) address));
    break;
  }
  
  BX_DEBUG(("val =  0x%04x", (Bit16u) val));

  return(val);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pcipnic_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pcipnic_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF
  Bit8u  offset,port;

  BX_DEBUG(("register write to address 0x%04x - ", (unsigned) address));

  offset = address - BX_PNIC_THIS s.base_ioaddr;

  switch (offset) {
  case PNIC_REG_CMD :
    if (io_len != 2)
      BX_PANIC(("PNIC write to command register, bad i/o length %u",
		(unsigned) io_len ));
    BX_PNIC_THIS s.rCmd = value;
    BX_PNIC_THIS exec_command();
    break;
    
  case PNIC_REG_LEN :
    if (io_len != 2)
      BX_PANIC(("PNIC write to length register, bad i/o length %u",
		(unsigned) io_len ));
    if ( value > PNIC_DATA_SIZE )
      BX_PANIC(("PNIC bad length %u written to length register, max is %u",
		value, PNIC_DATA_SIZE));
    BX_PNIC_THIS s.rLength = value;
    BX_PNIC_THIS s.rDataCursor = 0;
    break;
    
  case PNIC_REG_DATA :
    if (io_len != 1)
      BX_PANIC(("PNIC write to data register, bad i/o length %u",
		(unsigned) io_len ));
    if ( BX_PNIC_THIS s.rDataCursor >= BX_PNIC_THIS s.rLength )
      BX_PANIC(("PNIC write at %u, beyond end of data register array",
		BX_PNIC_THIS s.rDataCursor));
    BX_PNIC_THIS s.rData[BX_PNIC_THIS s.rDataCursor ++] = value;
    break;
    
  default:
    BX_PANIC(("unsupported io write to address=0x%04x!", (unsigned) address));
    break;
  }
}

void bx_pcipnic_c::pnic_timer_handler(void *this_ptr)
{
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;
  class_ptr->pnic_timer();
}

// Called once every 1ms
void bx_pcipnic_c::pnic_timer(void)
{
  // Do nothing atm

}
  
// static pci configuration space read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u
bx_pcipnic_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;
  
  return class_ptr->pci_read(address, io_len);
}


Bit32u
bx_pcipnic_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF

  Bit32u value = 0;

  if (io_len > 4 || io_len == 0) {
    BX_ERROR(("Experimental PNIC PCI read register 0x%02x, len=%u !",
             (unsigned) address, (unsigned) io_len));
    return 0xffffffff;
  }

  const char* pszName = "                  ";
  switch (address) {
    case 0x00: if (io_len == 2) {
                 pszName = "(vendor id)       ";
               } else if (io_len == 4) {
                 pszName = "(vendor + device) ";
               }
      break;
    case 0x04: if (io_len == 2) {
                 pszName = "(command)         ";
               } else if (io_len == 4) {
                 pszName = "(command+status)  ";
               }
      break;
    case 0x08: if (io_len == 1) {
                 pszName = "(revision id)     ";
               } else if (io_len == 4) {
                 pszName = "(rev.+class code) ";
               }
      break;
    case 0x0c: pszName = "(cache line size) "; break;
    case 0x20: pszName = "(base address)    "; break;
    case 0x28: pszName = "(cardbus cis)     "; break;
    case 0x2c: pszName = "(subsys. vendor+) "; break;
    case 0x30: pszName = "(rom base)        "; break;
    case 0x3c: pszName = "(interrupt line+) "; break;
    case 0x3d: pszName = "(interrupt pin)   "; break;
  }

  // This odd code is to display only what bytes actually were read.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_PNIC_THIS s.pci_conf[address+i] << (i*8));
    sprintf(szTmp2, "%02x", (BX_PNIC_THIS s.pci_conf[address+i]));
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("Experimental PNIC PCI read register 0x%02x %svalue 0x%s",
            address, pszName, szTmp));
  return value;
}


// static pci configuration space write callback handler
// redirects to non-static class handler to avoid virtual functions

void
bx_pcipnic_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIPNIC_SMF
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) this_ptr;
  
  class_ptr->pci_write(address, value, io_len);
}

void
bx_pcipnic_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIPNIC_SMF
  
  if (io_len > 4 || io_len == 0) {
    BX_ERROR(("Experimental PNIC PCI write register 0x%02x, len=%u !",
	      (unsigned) address, (unsigned) io_len));
    return;
  }
  
  // This odd code is to display only what bytes actually were written.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  for (unsigned i=0; i<io_len; i++) {
    const Bit8u value8 = (value >> (i*8)) & 0xFF;
    switch (address+i) {
      case 0x20: // Base address
        BX_PNIC_THIS s.pci_conf[address+i] = (value8 & 0xe0) | 0x01;
        sprintf(szTmp2, "%02x", (value8 & 0xe0) | 0x01);
        break;
      case 0x10: // Reserved
      case 0x11: //
      case 0x12: //
      case 0x13: //
      case 0x14: //
      case 0x15: //
      case 0x16: //
      case 0x17: //
      case 0x18: //
      case 0x19: //
      case 0x1a: //
      case 0x1b: //
      case 0x1c: //
      case 0x1d: //
      case 0x1e: //
      case 0x1f: //
      case 0x22: // Always 0
      case 0x23: //
      case 0x24: // Reserved
      case 0x25: //
      case 0x26: //
      case 0x27: //
      case 0x30: // Oh, no, you're not writing to rom_base!
      case 0x31: //
      case 0x32: //
      case 0x33: //
      case 0x3d: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        strcpy(szTmp2, "..");
        break;
      default:
        BX_PNIC_THIS s.pci_conf[address+i] = value8;
        sprintf(szTmp2, "%02x", value8);
    }
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("Experimental PNIC PCI write register 0x%02x value 0x%s", address, szTmp));
}


/*
 * Execute a hardware command.
 */
void
bx_pcipnic_c::exec_command(void)
{
  Bit16u command = BX_PNIC_THIS s.rCmd;
  Bit16u ilength = BX_PNIC_THIS s.rLength;
  Bit8u *data	 = BX_PNIC_THIS s.rData;
  // Default return values
  Bit16u status  = PNIC_STATUS_UNKNOWN_CMD;
  Bit16u olength = 0;

  if ( ilength != BX_PNIC_THIS s.rDataCursor )
    BX_PANIC(("PNIC command issued with incomplete data (should be %u, is %u)",
	      ilength, BX_PNIC_THIS s.rDataCursor ));
  
  switch ( command ) {

  case PNIC_CMD_NOOP :
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_API_VER :
    Bit16u api_version;

    api_version = PNIC_API_VERSION;
    olength = sizeof(api_version);
    memcpy ( data, &api_version, sizeof(api_version) );
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_READ_MAC :
    olength = sizeof ( BX_PNIC_THIS s.macaddr );
    memcpy ( data, BX_PNIC_THIS s.macaddr, olength );
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RESET :
    /* Flush the receive queue */
    BX_PNIC_THIS s.recvQueueLength = 0;
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_XMIT :
    BX_PNIC_THIS ethdev->sendpkt ( data, ilength );
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RECV :
    if ( BX_PNIC_THIS s.recvQueueLength > 0 ) {
      int idx = ( BX_PNIC_THIS s.recvIndex - BX_PNIC_THIS s.recvQueueLength
		  + PNIC_RECV_RINGS ) % PNIC_RECV_RINGS;
      olength = BX_PNIC_THIS s.recvRingLength[idx];
      memcpy ( data, BX_PNIC_THIS s.recvRing[idx], olength );
      BX_PNIC_THIS s.recvQueueLength --;
    }
    if ( ! BX_PNIC_THIS s.recvQueueLength ) {
      DEV_pic_lower_irq ( BX_PNIC_THIS s.irq );
    }
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_RECV_QLEN :
    Bit16u qlen;

    qlen = BX_PNIC_THIS s.recvQueueLength;
    olength = sizeof(qlen);
    memcpy ( data, &qlen, sizeof(qlen) );
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_MASK_IRQ :
    Bit8u enabled;

    enabled = *((Bit8u*)data);
    BX_PNIC_THIS s.irqEnabled = enabled;
    if ( enabled && BX_PNIC_THIS s.recvQueueLength ) {
      DEV_pic_raise_irq ( BX_PNIC_THIS s.irq );
    } else {
      DEV_pic_lower_irq ( BX_PNIC_THIS s.irq );
    }
    status = PNIC_STATUS_OK;
    break;

  case PNIC_CMD_FORCE_IRQ :
    DEV_pic_raise_irq ( BX_PNIC_THIS s.irq );
    status = PNIC_STATUS_OK;
    break;

  default:
    BX_ERROR(("Unknown PNIC command %x (data length %u)", command, ilength ));
    status = PNIC_STATUS_UNKNOWN_CMD;
    break;

  }

  // Set registers
  BX_PNIC_THIS s.rStatus = status;
  BX_PNIC_THIS s.rLength = olength;
  BX_PNIC_THIS s.rDataCursor = 0;
}


/*
 * Callback from the eth system driver when a frame has arrived
 */
void
bx_pcipnic_c::rx_handler(void *arg, const void *buf, unsigned len)
{
    // BX_DEBUG(("rx_handler with length %d", len));
  bx_pcipnic_c *class_ptr = (bx_pcipnic_c *) arg;
  
  class_ptr->rx_frame(buf, len);
}

/*
 * rx_frame() - called by the platform-specific code when an
 * ethernet frame has been received. The destination address
 * is tested to see if it should be accepted, and if the
 * rx ring has enough room, it is copied into it and
 * the receive process is updated
 */
void
bx_pcipnic_c::rx_frame(const void *buf, unsigned io_len)
{
  // Check packet length
  if ( io_len > PNIC_DATA_SIZE ) {
    BX_PANIC ( ( "PNIC receive: data size %u exceeded buffer size %u",
		 io_len, PNIC_DATA_SIZE ) );
    // Truncate if user continues
    io_len = PNIC_DATA_SIZE;
  }
  // Check receive ring is not full
  if ( BX_PNIC_THIS s.recvQueueLength == PNIC_RECV_RINGS ) {
    BX_ERROR ( ( "PNIC receive: receive ring full, discarding packet" ) );
    return;
  }
  // Copy data to receive ring and record length
  memcpy ( BX_PNIC_THIS s.recvRing[ BX_PNIC_THIS s.recvIndex ], buf, io_len );
  BX_PNIC_THIS s.recvRingLength[ BX_PNIC_THIS s.recvIndex ] = io_len;
  // Move to next ring entry
  BX_PNIC_THIS s.recvIndex = ( BX_PNIC_THIS s.recvIndex + 1) % PNIC_RECV_RINGS;
  BX_PNIC_THIS s.recvQueueLength ++;

  // Generate interrupt if enabled
  if ( BX_PNIC_THIS s.irqEnabled ) {
    DEV_pic_raise_irq ( BX_PNIC_THIS s.irq );
  }
}

#endif // BX_PCI_SUPPORT && BX_PCI_PNIC_SUPPORT
