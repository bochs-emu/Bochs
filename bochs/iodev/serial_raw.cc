/////////////////////////////////////////////////////////////////////////
// $Id: serial_raw.cc,v 1.8 2004-02-28 21:28:28 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004  MandrakeSoft S.A.
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
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"

#if USE_RAW_SERIAL

#define LOG_THIS bx_devices.pluginSerialDevice->

serial_raw::serial_raw (char *devname)
{
#ifdef WIN32
  char portstr[MAX_PATH];
#endif

  put ("SERR");
  settype (SERRLOG);
#ifdef WIN32
  memset(&dcb, 0, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  dcb.fBinary = 1;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  dcb.fOutxCtsFlow = TRUE;
  dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
  DCBchanged = FALSE;
  if (lstrlen(devname) > 0) {
    wsprintf(portstr, "\\\\.\\%s", devname);
    present = 0;
  } else {
    present = 0;
  }
#else
  present = 0;
#endif
}

serial_raw::~serial_raw (void)
{
  // nothing here yet
}

void 
serial_raw::set_baudrate (int rate)
{
  BX_DEBUG (("set_baudrate %d", rate));
#ifdef WIN32
  switch (rate) {
    case 110: dcb.BaudRate = CBR_110; break;
    case 300: dcb.BaudRate = CBR_300; break;
    case 600: dcb.BaudRate = CBR_600; break;
    case 1200: dcb.BaudRate = CBR_1200; break;
    case 2400: dcb.BaudRate = CBR_2400; break;
    case 4800: dcb.BaudRate = CBR_4800; break;
    case 9600: dcb.BaudRate = CBR_9600; break;
    case 19200: dcb.BaudRate = CBR_19200; break;
    case 38400: dcb.BaudRate = CBR_38400; break;
    case 57600: dcb.BaudRate = CBR_57600; break;
    case 115200: dcb.BaudRate = CBR_115200; break;
    default: BX_ERROR(("set_baudrate(): unsupported value %d", rate));
  }
  DCBchanged = TRUE;
#endif
}

void 
serial_raw::set_data_bits (int val)
{
  BX_DEBUG (("set data bits (%d)", val));
#ifdef WIN32
  dcb.ByteSize = val;
  DCBchanged = TRUE;
#endif
}

void 
serial_raw::set_stop_bits (int val)
{
  BX_DEBUG (("set stop bits (%d)", val));
#ifdef WIN32
  if (val == 1) {
    dcb.StopBits = ONESTOPBIT;
  } if (dcb.ByteSize == 5) {
    dcb.StopBits = ONE5STOPBITS;
  } else {
    dcb.StopBits = TWOSTOPBITS;
  }
  DCBchanged = TRUE;
#endif
}

void 
serial_raw::set_parity_mode (int mode)
{
  BX_DEBUG (("set parity mode %d", mode));
#ifdef WIN32
  switch (mode) {
    case 0:
      dcb.fParity = FALSE;
      dcb.Parity = NOPARITY;
      break;
    case 1:
      dcb.fParity = TRUE;
      dcb.Parity = ODDPARITY;
      break;
    case 2:
      dcb.fParity = TRUE;
      dcb.Parity = EVENPARITY;
      break;
    case 3:
      dcb.fParity = TRUE;
      dcb.Parity = MARKPARITY;
      break;
    case 4:
      dcb.fParity = TRUE;
      dcb.Parity = SPACEPARITY;
      break;
  }
  DCBchanged = TRUE;
#endif
}

void 
serial_raw::set_break (int mode)
{
  BX_DEBUG (("set break %s", mode?"on":"off"));
}

void 
serial_raw::transmit (int val)
{
  BX_DEBUG (("transmit %d", val));
}

void 
serial_raw::send_hangup ()
{
  BX_DEBUG (("send_hangup"));
}

int 
serial_raw::ready_transmit ()
{
  BX_DEBUG (("ready_transmit returning %d", present));
  return present;
}

int 
serial_raw::ready_receive ()
{
  BX_DEBUG (("ready_receive returning %d", present));
  return present;
}

int 
serial_raw::receive ()
{
  BX_DEBUG (("receive returning 'A'"));
  return (int)'A';
}

#endif
