/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2013  The Bochs Project
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
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if USE_RAW_SERIAL

#include "serial_raw.h"

#define LOG_THIS

#ifdef WIN32_RECEIVE_RAW
DWORD WINAPI RawSerialThread(VOID *this_ptr);
#endif

serial_raw::serial_raw(const char *devname)
{
#ifdef WIN32
  char portstr[MAX_PATH];
#ifdef WIN32_RECEIVE_RAW
  DWORD threadID;
#endif
#endif

  put("serial_raw", "SERRAW");
#ifdef WIN32
  memset(&dcb, 0, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  dcb.fBinary = 1;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  dcb.Parity = NOPARITY;
  dcb.ByteSize = 8;
  dcb.StopBits = ONESTOPBIT;
  dcb.BaudRate = CBR_115200;
  DCBchanged = FALSE;
  if (lstrlen(devname) > 0) {
    wsprintf(portstr, "\\\\.\\%s", devname);
    hCOM = CreateFile(portstr, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (hCOM != INVALID_HANDLE_VALUE) {
      present = 1;
      GetCommModemStatus(hCOM, &MSR_value);
      SetupComm(hCOM, 8192, 2048);
      PurgeComm(hCOM, PURGE_TXABORT | PURGE_RXABORT |
                PURGE_TXCLEAR | PURGE_RXCLEAR);
#ifdef WIN32_RECEIVE_RAW
      SetCommMask(hCOM, EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD | EV_RXCHAR);
      memset(&rx_ovl, 0, sizeof(OVERLAPPED));
      rx_ovl.hEvent = CreateEvent(NULL,TRUE,FALSE,"receive");
      hRawSerialThread = CreateThread(NULL, 0, RawSerialThread, this, 0, &threadID);
#endif
    } else {
      present = 0;
      BX_ERROR(("Raw device '%s' not present", devname));
    }
  } else {
    present = 0;
  }
#else
  present = 0;
#endif
  set_modem_control(0x00);
  set_break(0);
  rxdata_count = 0;
}

serial_raw::~serial_raw(void)
{
  if (present) {
#ifdef WIN32
#ifdef WIN32_RECEIVE_RAW
    thread_quit = TRUE;
    SetCommMask(hCOM, 0);
    while (thread_active) Sleep(10);
    CloseHandle(thread_ovl.hEvent);
    CloseHandle(rx_ovl.hEvent);
    CloseHandle(hRawSerialThread);
#endif
    CloseHandle(hCOM);
#endif
  }
}

void serial_raw::set_baudrate(int rate)
{
  BX_DEBUG(("set_baudrate %d", rate));
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

void serial_raw::set_data_bits(int val)
{
  BX_DEBUG(("set data bits (%d)", val));
#ifdef WIN32
  dcb.ByteSize = val;
  DCBchanged = TRUE;
#endif
}

void serial_raw::set_stop_bits(int val)
{
  BX_DEBUG(("set stop bits (%d)", val));
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

void serial_raw::set_parity_mode(int mode)
{
  BX_DEBUG(("set parity mode %d", mode));
#ifdef WIN32
  switch (mode) {
    case P_NONE:
      dcb.fParity = FALSE;
      dcb.Parity = NOPARITY;
      break;
    case P_ODD:
      dcb.fParity = TRUE;
      dcb.Parity = ODDPARITY;
      break;
    case P_EVEN:
      dcb.fParity = TRUE;
      dcb.Parity = EVENPARITY;
      break;
    case P_HIGH:
      dcb.fParity = TRUE;
      dcb.Parity = MARKPARITY;
      break;
    case P_LOW:
      dcb.fParity = TRUE;
      dcb.Parity = SPACEPARITY;
      break;
  }
  DCBchanged = TRUE;
#endif
}

void serial_raw::set_break(int mode)
{
  BX_DEBUG(("set break %s", mode?"on":"off"));
#ifdef WIN32
  if (mode) {
    SetCommBreak(hCOM);
  } else {
    ClearCommBreak(hCOM);
  }
#endif
}

void serial_raw::set_modem_control(int ctrl)
{
  BX_DEBUG(("set modem control 0x%02x", ctrl));
#ifdef WIN32
  EscapeCommFunction(hCOM, (ctrl & 0x01)?SETDTR:CLRDTR);
  EscapeCommFunction(hCOM, (ctrl & 0x02)?SETRTS:CLRRTS);
#endif
}

int serial_raw::get_modem_status()
{
  int status = 0;

#ifdef WIN32
  status = MSR_value;
#endif
  BX_DEBUG(("get modem status returns 0x%02x", status));
  return status;
}

void serial_raw::setup_port()
{
#ifdef WIN32
  DWORD DErr;
  COMMTIMEOUTS ctmo;

  ClearCommError(hCOM, &DErr, NULL);
  PurgeComm(hCOM, PURGE_TXABORT | PURGE_RXABORT |
            PURGE_TXCLEAR | PURGE_RXCLEAR);
  memset(&ctmo, 0, sizeof(ctmo));
  SetCommTimeouts(hCOM, &ctmo);
  SetCommState(hCOM, &dcb);
  rxdata_count = 0;
#ifdef WIN32_RECEIVE_RAW
  thread_rxdata_count = 0;
#endif
#endif
}

void serial_raw::transmit(Bit8u byte)
{
#ifdef WIN32
  DWORD DErr, Len2;
  OVERLAPPED tx_ovl;
#endif

  BX_DEBUG(("transmit %d", byte));
  if (present) {
#ifdef WIN32
    if (DCBchanged) {
      setup_port();
    } else {
      ClearCommError(hCOM, &DErr, NULL);
    }
    memset(&tx_ovl, 0, sizeof(OVERLAPPED));
    tx_ovl.hEvent = CreateEvent(NULL,TRUE,TRUE,"transmit");
    if (!WriteFile(hCOM, &byte, 1, &Len2, &tx_ovl)) {
      if (GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(tx_ovl.hEvent, 100) == WAIT_OBJECT_0) {
          GetOverlappedResult(hCOM, &tx_ovl, &Len2, FALSE);
        }
      }
    }
    if (Len2 != 1) BX_ERROR(("transmit failed: len = %d", Len2));
    ClearCommError(hCOM, &DErr, NULL);
    CloseHandle(tx_ovl.hEvent);
#endif
  }
}

bx_bool serial_raw::ready_transmit()
{
  BX_DEBUG(("ready_transmit returning %d", present));
  return present;
}

bx_bool serial_raw::ready_receive()
{
#ifdef WIN32_RECEIVE_RAW
  if ((rxdata_count == 0) && (thread_rxdata_count > 0)) {
    SetEvent(thread_ovl.hEvent);
    SetEvent(rx_ovl.hEvent);
  }
#endif
  BX_DEBUG(("ready_receive returning %d", (rxdata_count > 0)));
  return (rxdata_count > 0);
}

int serial_raw::receive()
{
#ifdef WIN32
  int data;
#endif

  if (present) {
#ifdef WIN32
    if (DCBchanged) {
      setup_port();
    }
    data = rxdata_buffer[0];
    if (rxdata_count > 0) {
      memcpy(&rxdata_buffer[0], &rxdata_buffer[1], sizeof(Bit16s)*(RX_BUFSIZE-1));
      rxdata_count--;
    }
    if (data < 0) {
      switch (data) {
        case RAW_EVENT_CTS_ON:
          MSR_value |= 0x10;
          break;
        case RAW_EVENT_CTS_OFF:
          MSR_value &= ~0x10;
          break;
        case RAW_EVENT_DSR_ON:
          MSR_value |= 0x20;
          break;
        case RAW_EVENT_DSR_OFF:
          MSR_value &= ~0x20;
          break;
        case RAW_EVENT_RING_ON:
          MSR_value |= 0x40;
          break;
        case RAW_EVENT_RING_OFF:
          MSR_value &= ~0x40;
          break;
        case RAW_EVENT_RLSD_ON:
          MSR_value |= 0x80;
          break;
        case RAW_EVENT_RLSD_OFF:
          MSR_value &= ~0x80;
          break;
      }
    }
    return data;
#else
    BX_DEBUG(("receive returning 'A'"));
    return (int)'A';
#endif
  } else {
    BX_DEBUG(("receive returning 'A'"));
    return (int)'A';
  }
}

#ifdef WIN32_RECEIVE_RAW

DWORD WINAPI RawSerialThread(VOID *this_ptr)
{
  serial_raw *class_ptr = (serial_raw *) this_ptr;
  class_ptr->serial_thread();
  return 0;
}

void serial_raw::serial_thread()
{
  DWORD DErr, Len2;
  DWORD EvtMask, MSR, Temp;
  char s1[2];

  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
  thread_active = TRUE;
  thread_quit = FALSE;
  memset(&thread_ovl, 0, sizeof(OVERLAPPED));
  thread_ovl.hEvent = CreateEvent(NULL,TRUE,TRUE,"thread");
  thread_rxdata_count = 0;
  while (!thread_quit) {
    if ((rxdata_count == 0) && (thread_rxdata_count > 0)) {
      if (thread_rxdata_count > RX_BUFSIZE) {
        memcpy(&rxdata_buffer[0], &thread_rxdata_buffer[0], sizeof(Bit16s)*RX_BUFSIZE);
        memcpy(&thread_rxdata_buffer[0], &thread_rxdata_buffer[RX_BUFSIZE], sizeof(Bit16s)*(thread_rxdata_count-RX_BUFSIZE));
        rxdata_count = RX_BUFSIZE;
        thread_rxdata_count -= RX_BUFSIZE;
      } else {
        memcpy(&rxdata_buffer[0], &thread_rxdata_buffer[0], sizeof(Bit16s)*thread_rxdata_count);
        rxdata_count = thread_rxdata_count;
        thread_rxdata_count = 0;
      }
    }
    ClearCommError(hCOM, &DErr, NULL);
    EvtMask = 0;
    if (!WaitCommEvent(hCOM, &EvtMask, &thread_ovl)) {
      if (GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(thread_ovl.hEvent, INFINITE) == WAIT_OBJECT_0) {
          GetOverlappedResult(hCOM, &thread_ovl, &Temp, FALSE);
        }
      }
    }
    if (EvtMask & EV_RXCHAR) {
      if (thread_rxdata_count < THREAD_RX_BUFSIZE) {
        do {
          ClearCommError(hCOM, &DErr, NULL);
          if (!ReadFile(hCOM, s1, 1, &Len2, &rx_ovl)) {
            if (GetLastError() == ERROR_IO_PENDING) {
              if (WaitForSingleObject(rx_ovl.hEvent, INFINITE) != WAIT_OBJECT_0) {
                Len2 = 0;
              } else {
                GetOverlappedResult(hCOM, &rx_ovl, &Len2, FALSE);
              }
            } else {
              Len2 = 0;
            }
          }
          if (Len2 > 0) {
            enq_event(s1[0]);
          }
          if ((rxdata_count == 0) && (thread_rxdata_count > 0)) {
            if (thread_rxdata_count > RX_BUFSIZE) {
              memcpy(&rxdata_buffer[0], &thread_rxdata_buffer[0], sizeof(Bit16s)*RX_BUFSIZE);
              memcpy(&thread_rxdata_buffer[0], &thread_rxdata_buffer[RX_BUFSIZE], sizeof(Bit16s)*(thread_rxdata_count-RX_BUFSIZE));
              rxdata_count = RX_BUFSIZE;
              thread_rxdata_count -= RX_BUFSIZE;
            } else {
              memcpy(&rxdata_buffer[0], &thread_rxdata_buffer[0], sizeof(Bit16s)*thread_rxdata_count);
              rxdata_count = thread_rxdata_count;
              thread_rxdata_count = 0;
            }
          }
        } while ((Len2 != 0) && (thread_rxdata_count < THREAD_RX_BUFSIZE));
        ClearCommError(hCOM, &DErr, NULL);
      }
    }
    if (EvtMask & EV_BREAK) {
      enq_event(RAW_EVENT_BREAK);
    }
    if (EvtMask & EV_ERR) {
      ClearCommError(hCOM, &DErr, NULL);
      if (DErr & CE_FRAME) {
        enq_event(RAW_EVENT_FRAME);
      }
      if (DErr & CE_OVERRUN) {
        enq_event(RAW_EVENT_OVERRUN);
      }
      if (DErr & CE_RXPARITY) {
        enq_event(RAW_EVENT_PARITY);
      }
    }
    if (EvtMask & (EV_CTS | EV_DSR | EV_RING | EV_RLSD)) {
      GetCommModemStatus(hCOM, &MSR);
    }
    if (EvtMask & EV_CTS) {
      if (MSR & MS_CTS_ON) {
        enq_event(RAW_EVENT_CTS_ON);
      } else {
        enq_event(RAW_EVENT_CTS_OFF);
      }
    }
    if (EvtMask & EV_DSR) {
      if (MSR & MS_DSR_ON) {
        enq_event(RAW_EVENT_DSR_ON);
      } else {
        enq_event(RAW_EVENT_DSR_OFF);
      }
    }
    if (EvtMask & EV_RING) {
      if (MSR & MS_RING_ON) {
        enq_event(RAW_EVENT_RING_ON);
      } else {
        enq_event(RAW_EVENT_RING_OFF);
      }
    }
    if (EvtMask & EV_RLSD) {
      if (MSR & MS_RLSD_ON) {
        enq_event(RAW_EVENT_RLSD_ON);
      } else {
        enq_event(RAW_EVENT_RLSD_OFF);
      }
    }
  }
  CloseHandle(thread_ovl.hEvent);
  thread_active = FALSE;
}

void serial_raw::enq_event(Bit16s event)
{
  if (thread_rxdata_count < THREAD_RX_BUFSIZE) {
    thread_rxdata_buffer[thread_rxdata_count++] = event;
  } else {
    fprintf(stderr, "receive buffer overflow\n");
  }
}
#endif

#endif
