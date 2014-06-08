/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// Peter Grehan (grehan@iprg.nokia.com) coded the original version of this
// serial emulation. He implemented a single 8250, and allow terminal
// input/output to stdout on FreeBSD.
// The current version emulates up to 4 UART 16550A with FIFO. Terminal
// input/output now works on some more platforms.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if !defined(WIN32) || defined(__CYGWIN__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket(s)    close(s)
typedef int SOCKET;
#else
#include <winsock2.h>
#define BX_SER_WIN32
#endif

#include "serial.h"

#if defined(WIN32) && !defined(FILE_FLAG_FIRST_PIPE_INSTANCE)
#define FILE_FLAG_FIRST_PIPE_INSTANCE 0
#endif

#if USE_RAW_SERIAL
#include "serial_raw.h"
#endif

#define LOG_THIS theSerialDevice->

bx_serial_c *theSerialDevice = NULL;

// builtin configuration handling functions

void serial_init_options(void)
{
  static const char *serial_mode_list[] = {
    "null",
    "file",
    "term",
    "raw",
    "mouse",
    "socket-client",
    "socket-server",
    "pipe-client",
    "pipe-server",
    NULL
  };

  char name[4], label[80], descr[120];

  bx_list_c *serial = (bx_list_c*)SIM->get_param("ports.serial");
  for (int i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf(label, "Serial Port %d", i+1);
    bx_list_c *menu = new bx_list_c(serial, name, label);
    menu->set_options(menu->SERIES_ASK);
    sprintf(label, "Enable serial port #%d (COM%d)", i+1, i+1);
    sprintf(descr, "Controls whether COM%d is installed or not", i+1);
    bx_param_bool_c *enabled = new bx_param_bool_c(menu, "enabled", label, descr,
      (i==0)?1 : 0);  // only enable the first by default
    sprintf(label, "I/O mode of the serial device for COM%d", i+1);
    bx_param_enum_c *mode = new bx_param_enum_c(menu, "mode", label,
      "The mode can be one these: 'null', 'file', 'term', 'raw', 'mouse', 'socket*', 'pipe*'",
      serial_mode_list, BX_SER_MODE_NULL, BX_SER_MODE_NULL);
    mode->set_ask_format("Choose I/O mode of the serial device [%s] ");
    sprintf(label, "Pathname of the serial device for COM%d", i+1);
    bx_param_filename_c *path = new bx_param_filename_c(menu, "dev", label, 
      "The path can be a real serial device or a pty (X/Unix only)",
      "", BX_PATHNAME_LEN);
    bx_list_c *deplist = new bx_list_c(NULL);
    deplist->add(mode);
    enabled->set_dependent_list(deplist);
    deplist = new bx_list_c(NULL);
    deplist->add(path);
    mode->set_dependent_list(deplist, 1);
    mode->set_dependent_bitmap(BX_SER_MODE_NULL, 0);
    mode->set_dependent_bitmap(BX_SER_MODE_MOUSE, 0);
  }
}

Bit32s serial_options_parser(const char *context, int num_params, char *params[])
{
  if ((!strncmp(params[0], "com", 3)) && (strlen(params[0]) == 4)) {
    char tmpname[80];
    int idx = params[0][3];
    if ((idx < '1') || (idx > '9')) {
      BX_PANIC(("%s: comX directive malformed.", context));
    }
    idx -= '0';
    if (idx > BX_N_SERIAL_PORTS) {
      BX_PANIC(("%s: comX port number out of range.", context));
    }
    sprintf(tmpname, "ports.serial.%d", idx);
    bx_list_c *base = (bx_list_c*) SIM->get_param(tmpname);
    for (int i=1; i<num_params; i++) {
      if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for com%d ignored.", context, idx));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s serial_options_save(FILE *fp)
{
  char port[20];

  for (int i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(port, "ports.serial.%d", i+1);
    bx_list_c *base = (bx_list_c*) SIM->get_param(port);
    sprintf(port, "com%d", i+1);
    SIM->write_param_list(fp, base, port, 0);
  }
  return 0;
}

// device plugin entry points

int CDECL libserial_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theSerialDevice = new bx_serial_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSerialDevice, BX_PLUGIN_SERIAL);
  // add new configuration parameters for the config interface
  serial_init_options();
  // register add-on options for bochsrc and command line
  SIM->register_addon_option("com1", serial_options_parser, serial_options_save);
  SIM->register_addon_option("com2", serial_options_parser, NULL);
  SIM->register_addon_option("com3", serial_options_parser, NULL);
  SIM->register_addon_option("com4", serial_options_parser, NULL);
  return 0; // Success
}

void CDECL libserial_LTX_plugin_fini(void)
{
  char port[6];

  delete theSerialDevice;
  bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.serial");
  for (int i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(port, "com%d", i+1);
    SIM->unregister_addon_option(port);
    sprintf(port, "%d", i+1);
    menu->remove(port);
  }
}

// the device object

bx_serial_c::bx_serial_c(void)
{
  put("serial", "SER");
  for (int i=0; i<BX_SERIAL_MAXDEV; i++) {
    memset(&s[i], 0, sizeof(bx_serial_t));
    s[i].io_mode = BX_SER_MODE_NULL;
    s[i].tty_id = -1;
    s[i].tx_timer_index = BX_NULL_TIMER_HANDLE;
    s[i].rx_timer_index = BX_NULL_TIMER_HANDLE;
    s[i].fifo_timer_index = BX_NULL_TIMER_HANDLE;
  }
}

bx_serial_c::~bx_serial_c(void)
{
  char pname[20];
  bx_list_c *base;

  for (int i=0; i<BX_SERIAL_MAXDEV; i++) {
    sprintf(pname, "ports.serial.%d", i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_bool("enabled", base)->get()) {
      switch (BX_SER_THIS s[i].io_mode) {
        case BX_SER_MODE_FILE:
          if (BX_SER_THIS s[i].output != NULL)
            fclose(BX_SER_THIS s[i].output);
          break;
        case BX_SER_MODE_TERM:
#if defined(SERIAL_ENABLE) && !defined(BX_SER_WIN32)
          if (s[i].tty_id >= 0) {
            tcsetattr(s[i].tty_id, TCSAFLUSH, &s[i].term_orig);
          }
#endif
          break;
        case BX_SER_MODE_RAW:
#if USE_RAW_SERIAL
          delete [] BX_SER_THIS s[i].raw;
#endif
          break;
        case BX_SER_MODE_SOCKET_CLIENT:
        case BX_SER_MODE_SOCKET_SERVER:
          if (BX_SER_THIS s[i].socket_id >= 0) closesocket(BX_SER_THIS s[i].socket_id);
          break;
        case BX_SER_MODE_PIPE_CLIENT:
        case BX_SER_MODE_PIPE_SERVER:
#ifdef BX_SER_WIN32
          if (BX_SER_THIS s[i].pipe)
            CloseHandle(BX_SER_THIS s[i].pipe);
#endif
          break;
      }
    }
  }
  SIM->get_bochs_root()->remove("serial");
  BX_DEBUG(("Exit"));
}


  void
bx_serial_c::init(void)
{
  Bit16u ports[BX_SERIAL_MAXDEV] = {0x03f8, 0x02f8, 0x03e8, 0x02e8};
  char name[16], pname[20];
  bx_list_c *base;
  unsigned i, count = 0;

  BX_SER_THIS detect_mouse = 0;
  BX_SER_THIS mouse_port = -1;
  BX_SER_THIS mouse_type = BX_MOUSE_TYPE_NONE;
  BX_SER_THIS mouse_internal_buffer.num_elements = 0;
  for (i=0; i<BX_MOUSE_BUFF_SIZE; i++)
    BX_SER_THIS mouse_internal_buffer.buffer[i] = 0;
  BX_SER_THIS mouse_internal_buffer.head = 0;
  BX_SER_THIS mouse_delayed_dx = 0;
  BX_SER_THIS mouse_delayed_dy = 0;
  BX_SER_THIS mouse_delayed_dz = 0;
  BX_SER_THIS mouse_buttons = 0;
  BX_SER_THIS mouse_update = 0;
  /*
   * Put the UART registers into their RESET state
   */
  for (i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(pname, "ports.serial.%d", i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (SIM->get_param_bool("enabled", base)->get()) {
      sprintf(name, "Serial Port %d", i + 1);
      /* serial interrupt */
      BX_SER_THIS s[i].IRQ = 4 - (i & 1);
      if (i < 2) {
        DEV_register_irq(BX_SER_THIS s[i].IRQ, name);
      }
      /* internal state */
      BX_SER_THIS s[i].ls_ipending = 0;
      BX_SER_THIS s[i].ms_ipending = 0;
      BX_SER_THIS s[i].rx_ipending = 0;
      BX_SER_THIS s[i].fifo_ipending = 0;
      BX_SER_THIS s[i].ls_interrupt = 0;
      BX_SER_THIS s[i].ms_interrupt = 0;
      BX_SER_THIS s[i].rx_interrupt = 0;
      BX_SER_THIS s[i].tx_interrupt = 0;
      BX_SER_THIS s[i].fifo_interrupt = 0;

      if (BX_SER_THIS s[i].tx_timer_index == BX_NULL_TIMER_HANDLE) {
        BX_SER_THIS s[i].tx_timer_index =
          bx_pc_system.register_timer(this, tx_timer_handler, 0,
                                      0,0, "serial.tx"); // one-shot, inactive
          bx_pc_system.setTimerParam(BX_SER_THIS s[i].tx_timer_index, i);
      }

      if (BX_SER_THIS s[i].rx_timer_index == BX_NULL_TIMER_HANDLE) {
        BX_SER_THIS s[i].rx_timer_index =
          bx_pc_system.register_timer(this, rx_timer_handler, 0,
                                      0,0, "serial.rx"); // one-shot, inactive
          bx_pc_system.setTimerParam(BX_SER_THIS s[i].rx_timer_index, i);
      }
      if (BX_SER_THIS s[i].fifo_timer_index == BX_NULL_TIMER_HANDLE) {
        BX_SER_THIS s[i].fifo_timer_index =
          bx_pc_system.register_timer(this, fifo_timer_handler, 0,
                                      0,0, "serial.fifo"); // one-shot, inactive
          bx_pc_system.setTimerParam(BX_SER_THIS s[i].fifo_timer_index, i);
      }

      /* int enable: b0000 0000 */
      BX_SER_THIS s[i].int_enable.rxdata_enable = 0;
      BX_SER_THIS s[i].int_enable.txhold_enable = 0;
      BX_SER_THIS s[i].int_enable.rxlstat_enable = 0;
      BX_SER_THIS s[i].int_enable.modstat_enable = 0;

      /* int ID: b0000 0001 */
      BX_SER_THIS s[i].int_ident.ipending = 1;
      BX_SER_THIS s[i].int_ident.int_ID = 0;

      /* FIFO control: b0000 0000 */
      BX_SER_THIS s[i].fifo_cntl.enable = 0;
      BX_SER_THIS s[i].fifo_cntl.rxtrigger = 0;
      BX_SER_THIS s[i].rx_fifo_end = 0;
      BX_SER_THIS s[i].tx_fifo_end = 0;

      /* Line Control reg: b0000 0000 */
      BX_SER_THIS s[i].line_cntl.wordlen_sel = 0;
      BX_SER_THIS s[i].line_cntl.stopbits = 0;
      BX_SER_THIS s[i].line_cntl.parity_enable = 0;
      BX_SER_THIS s[i].line_cntl.evenparity_sel = 0;
      BX_SER_THIS s[i].line_cntl.stick_parity = 0;
      BX_SER_THIS s[i].line_cntl.break_cntl = 0;
      BX_SER_THIS s[i].line_cntl.dlab = 0;

      /* Modem Control reg: b0000 0000 */
      BX_SER_THIS s[i].modem_cntl.dtr = 0;
      BX_SER_THIS s[i].modem_cntl.rts = 0;
      BX_SER_THIS s[i].modem_cntl.out1 = 0;
      BX_SER_THIS s[i].modem_cntl.out2 = 0;
      BX_SER_THIS s[i].modem_cntl.local_loopback = 0;

      /* Line Status register: b0110 0000 */
      BX_SER_THIS s[i].line_status.rxdata_ready = 0;
      BX_SER_THIS s[i].line_status.overrun_error = 0;
      BX_SER_THIS s[i].line_status.parity_error = 0;
      BX_SER_THIS s[i].line_status.framing_error = 0;
      BX_SER_THIS s[i].line_status.break_int = 0;
      BX_SER_THIS s[i].line_status.thr_empty = 1;
      BX_SER_THIS s[i].line_status.tsr_empty = 1;
      BX_SER_THIS s[i].line_status.fifo_error = 0;

      /* Modem Status register: bXXXX 0000 */
      BX_SER_THIS s[i].modem_status.delta_cts = 0;
      BX_SER_THIS s[i].modem_status.delta_dsr = 0;
      BX_SER_THIS s[i].modem_status.ri_trailedge = 0;
      BX_SER_THIS s[i].modem_status.delta_dcd = 0;
      BX_SER_THIS s[i].modem_status.cts = 0;
      BX_SER_THIS s[i].modem_status.dsr = 0;
      BX_SER_THIS s[i].modem_status.ri = 0;
      BX_SER_THIS s[i].modem_status.dcd = 0;

      BX_SER_THIS s[i].scratch = 0;      /* scratch register */
      BX_SER_THIS s[i].divisor_lsb = 1;  /* divisor-lsb register */
      BX_SER_THIS s[i].divisor_msb = 0;  /* divisor-msb register */

      BX_SER_THIS s[i].baudrate = 115200;
      BX_SER_THIS s[i].databyte_usec = 87;

      for (unsigned addr = ports[i]; addr < (unsigned)(ports[i] + 8); addr++) {
        BX_DEBUG(("com%d initialize register for read/write: 0x%04x", i + 1, addr));
        if (addr < (unsigned)(ports[i] + 7)) {
          DEV_register_ioread_handler(this, read_handler, addr, name, 3);
          DEV_register_iowrite_handler(this, write_handler, addr, name, 3);
        } else {
          DEV_register_ioread_handler(this, read_handler, addr, name, 1);
          DEV_register_iowrite_handler(this, write_handler, addr, name, 1);
        }
      }

      BX_SER_THIS s[i].io_mode = BX_SER_MODE_NULL;
      Bit8u mode = SIM->get_param_enum("mode", base)->get();
      bx_param_string_c *devparam = SIM->get_param_string("dev", base);
      const char *dev = devparam->getptr();
      if (mode == BX_SER_MODE_FILE) {
        if (!devparam->isempty()) {
          // tx_timer() opens the output file on demand
          BX_SER_THIS s[i].io_mode = BX_SER_MODE_FILE;
        }
      } else if (mode == BX_SER_MODE_TERM) {
#if defined(SERIAL_ENABLE) && !defined(BX_SER_WIN32)
        if (!devparam->isempty()) {
          BX_SER_THIS s[i].tty_id = open(dev, O_RDWR|O_NONBLOCK,600);
          if (BX_SER_THIS s[i].tty_id < 0) {
            BX_PANIC(("open of com%d (%s) failed", i+1, dev));
          } else {
            BX_SER_THIS s[i].io_mode = BX_SER_MODE_TERM;
            BX_DEBUG(("com%d tty_id: %d", i+1, BX_SER_THIS s[i].tty_id));
            tcgetattr(BX_SER_THIS s[i].tty_id, &BX_SER_THIS s[i].term_orig);
            memcpy(&BX_SER_THIS s[i].term_orig, &BX_SER_THIS s[i].term_new, sizeof(struct termios));
            BX_SER_THIS s[i].term_new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
            BX_SER_THIS s[i].term_new.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
            BX_SER_THIS s[i].term_new.c_cflag &= ~(CSIZE|PARENB);
            BX_SER_THIS s[i].term_new.c_cflag |= CS8;
            BX_SER_THIS s[i].term_new.c_oflag |= OPOST | ONLCR;  // Enable NL to CR-NL translation
#ifndef TRUE_CTLC
            // ctl-C will exit Bochs, or trap to the debugger
            BX_SER_THIS s[i].term_new.c_iflag &= ~IGNBRK;
            BX_SER_THIS s[i].term_new.c_iflag |= BRKINT;
            BX_SER_THIS s[i].term_new.c_lflag |= ISIG;
#else
            // ctl-C will be delivered to the serial port
            BX_SER_THIS s[i].term_new.c_iflag |= IGNBRK;
            BX_SER_THIS s[i].term_new.c_iflag &= ~BRKINT;
#endif    /* !def TRUE_CTLC */
            BX_SER_THIS s[i].term_new.c_iflag = 0;
            BX_SER_THIS s[i].term_new.c_oflag = 0;
            BX_SER_THIS s[i].term_new.c_cflag = CS8|CREAD|CLOCAL;
            BX_SER_THIS s[i].term_new.c_lflag = 0;
            BX_SER_THIS s[i].term_new.c_cc[VMIN] = 1;
            BX_SER_THIS s[i].term_new.c_cc[VTIME] = 0;
            //BX_SER_THIS s[i].term_new.c_iflag |= IXOFF;
            tcsetattr(BX_SER_THIS s[i].tty_id, TCSAFLUSH, &BX_SER_THIS s[i].term_new);
          }
        }
#else
        BX_PANIC(("serial terminal support not available"));
#endif   /* def SERIAL_ENABLE */
      } else if (mode == BX_SER_MODE_RAW) {
#if USE_RAW_SERIAL
        BX_SER_THIS s[i].raw = new serial_raw(dev);
        BX_SER_THIS s[i].io_mode = BX_SER_MODE_RAW;
#else
        BX_PANIC(("raw serial support not present"));
#endif
      } else if (mode == BX_SER_MODE_MOUSE) {
        BX_SER_THIS s[i].io_mode = BX_SER_MODE_MOUSE;
        BX_SER_THIS mouse_port = i;
        BX_SER_THIS mouse_type = SIM->get_param_enum(BXPN_MOUSE_TYPE)->get();
      } else if ((mode == BX_SER_MODE_SOCKET_CLIENT) ||
                 (mode == BX_SER_MODE_SOCKET_SERVER)) {
        BX_SER_THIS s[i].io_mode = mode;
        struct sockaddr_in  sin;
        struct hostent      *hp;
        char                host[BX_PATHNAME_LEN];
        int                 port;
        SOCKET              socket;
        bx_bool             server = (mode == BX_SER_MODE_SOCKET_SERVER);

#ifdef BX_SER_WIN32
        static bx_bool winsock_init = false;
        if (!winsock_init) {
          WORD wVersionRequested;
          WSADATA wsaData;
          int err;
          wVersionRequested = MAKEWORD(2, 0);
          err = WSAStartup(wVersionRequested, &wsaData);
          if (err != 0)
            BX_PANIC(("WSAStartup failed"));
          winsock_init = true;
        }
#endif

        strcpy(host, dev);
        char *substr = strtok(host, ":");
        substr = strtok(NULL, ":");
        if (!substr) {
          BX_PANIC(("com%d: inet address is wrong (%s)", i+1, dev));
        }
        port = atoi(substr);

        hp = gethostbyname(host);
        if (!hp) {
          BX_PANIC(("com%d: gethostbyname failed (%s)", i+1, host));
        }

        memset ((char*) &sin, 0, sizeof (sin));
#if BX_HAVE_SOCKADDR_IN_SIN_LEN
       sin.sin_len = sizeof sin;
#endif
        memcpy ((char*) &(sin.sin_addr), hp->h_addr, hp->h_length);
        sin.sin_family = hp->h_addrtype;
        sin.sin_port = htons (port);

        socket = ::socket (AF_INET, SOCK_STREAM, 0);
        if (socket < 0)
          BX_PANIC(("com%d: socket() failed",i+1));

        // server mode
        if (server) {
          if (::bind (socket, (sockaddr *) &sin, sizeof (sin)) < 0 ||
              ::listen (socket, SOMAXCONN) < 0) {
            closesocket(socket);
            socket = (SOCKET) -1;
            BX_PANIC(("com%d: bind() or listen() failed (host:%s, port:%d)",i+1, host, port));
          }
          else {
            BX_INFO(("com%d: waiting for client to connect (host:%s, port:%d)",i+1, host, port));
            SOCKET client;
            if ((client = ::accept (socket, NULL, 0)) < 0)
              BX_PANIC(("com%d: accept() failed (host:%s, port:%d)",i+1, host, port));
            closesocket(socket);
            socket = client;
          }
        }
        // client mode
        else if (::connect (socket, (sockaddr *) &sin, sizeof (sin)) < 0) {
          closesocket(socket);
          socket = (SOCKET) -1;
          BX_INFO(("com%d: connect() failed (host:%s, port:%d)",i+1, host, port));
        }

        BX_SER_THIS s[i].socket_id = socket;
        if (socket > 0)
          BX_INFO(("com%d - inet %s - socket_id: %d, ip:%s, port:%d",
            i+1, server ? "server" : "client", socket, host, port));
      } else if ((mode == BX_SER_MODE_PIPE_CLIENT) ||
                 (mode == BX_SER_MODE_PIPE_SERVER)) {
        if (strlen(dev) > 0) {
          bx_bool server = (mode == BX_SER_MODE_PIPE_SERVER);
#ifdef BX_SER_WIN32
          HANDLE pipe;

          BX_SER_THIS s[i].io_mode = mode;

          // server mode
          if (server) {
            pipe = CreateNamedPipe( dev,
                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                1, 4096, 4096, 0, NULL);

            if (pipe == INVALID_HANDLE_VALUE)
              BX_PANIC(("com%d: CreateNamedPipe(%s) failed", i+1, dev));

            BX_INFO(("com%d: waiting for client to connect to %s", i+1, dev));
            if (!ConnectNamedPipe(pipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED)
            {
              CloseHandle(pipe);
              pipe = INVALID_HANDLE_VALUE;
              BX_PANIC(("com%d: ConnectNamedPipe(%s) failed", i+1, dev));
            }
          }
          // client mode
          else {
            pipe = CreateFile( dev,
               GENERIC_READ | GENERIC_WRITE, 
               0, NULL, OPEN_EXISTING, 0, NULL);

            if (pipe == INVALID_HANDLE_VALUE)
              BX_INFO(("com%d: failed to open pipe %s", i+1, dev));
          }

          if (pipe != INVALID_HANDLE_VALUE)
            BX_SER_THIS s[i].pipe = pipe;
#else
          BX_PANIC(("support for serial mode 'pipe-%s' not available", server?"server":"client"));
#endif
        }
      } else if (mode != BX_SER_MODE_NULL) {
        BX_PANIC(("unknown serial i/o mode %d", mode));
      }
      // simulate device connected
      if (BX_SER_THIS s[i].io_mode != BX_SER_MODE_RAW) {
        BX_SER_THIS s[i].modem_status.cts = 1;
        BX_SER_THIS s[i].modem_status.dsr = 1;
      }
      count++;
      BX_INFO(("com%d at 0x%04x irq %d (mode: %s)", i+1, ports[i], BX_SER_THIS s[i].IRQ,
               SIM->get_param_enum("mode", base)->get_selected()));
    }
  }
  // Check if the device is disabled or not configured
  if (count == 0) {
    BX_INFO(("serial ports disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("serial"))->set(0);
    return;
  }
  if ((BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL) ||
      (BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL_WHEEL) ||
      (BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL_MSYS)) {
    DEV_register_default_mouse(this, mouse_enq_static, NULL);
  }
}

void bx_serial_c::register_state(void)
{
  unsigned i, j;
  char name[6];
  bx_list_c *port;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "serial", "Serial Port State");
  for (i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(name, "%d", i);
    port = new bx_list_c(list, name);
    new bx_shadow_bool_c(port, "ls_interrupt", &BX_SER_THIS s[i].ls_interrupt);
    new bx_shadow_bool_c(port, "ms_interrupt", &BX_SER_THIS s[i].ms_interrupt);
    new bx_shadow_bool_c(port, "rx_interrupt", &BX_SER_THIS s[i].rx_interrupt);
    new bx_shadow_bool_c(port, "tx_interrupt", &BX_SER_THIS s[i].tx_interrupt);
    new bx_shadow_bool_c(port, "fifo_interrupt", &BX_SER_THIS s[i].fifo_interrupt);
    new bx_shadow_bool_c(port, "ls_ipending", &BX_SER_THIS s[i].ls_ipending);
    new bx_shadow_bool_c(port, "ms_ipending", &BX_SER_THIS s[i].ms_ipending);
    new bx_shadow_bool_c(port, "rx_ipending", &BX_SER_THIS s[i].rx_ipending);
    new bx_shadow_bool_c(port, "fifo_ipending", &BX_SER_THIS s[i].fifo_ipending);
    new bx_shadow_num_c(port, "rx_fifo_end", &BX_SER_THIS s[i].rx_fifo_end);
    new bx_shadow_num_c(port, "tx_fifo_end", &BX_SER_THIS s[i].tx_fifo_end);
    new bx_shadow_num_c(port, "baudrate", &BX_SER_THIS s[i].baudrate);
    new bx_shadow_num_c(port, "databyte_usec", &BX_SER_THIS s[i].databyte_usec);
    new bx_shadow_num_c(port, "rxbuffer", &BX_SER_THIS s[i].rxbuffer, BASE_HEX);
    new bx_shadow_num_c(port, "thrbuffer", &BX_SER_THIS s[i].thrbuffer, BASE_HEX);
    bx_list_c *int_en = new bx_list_c(port, "int_enable");
    new bx_shadow_bool_c(int_en, "rxdata_enable", &BX_SER_THIS s[i].int_enable.rxdata_enable);
    new bx_shadow_bool_c(int_en, "txhold_enable", &BX_SER_THIS s[i].int_enable.txhold_enable);
    new bx_shadow_bool_c(int_en, "rxlstat_enable", &BX_SER_THIS s[i].int_enable.rxlstat_enable);
    new bx_shadow_bool_c(int_en, "modstat_enable", &BX_SER_THIS s[i].int_enable.modstat_enable);
    bx_list_c *int_id = new bx_list_c(port, "int_ident");
    new bx_shadow_bool_c(int_id, "ipending", &BX_SER_THIS s[i].int_ident.ipending);
    new bx_shadow_num_c(int_id, "int_ID", &BX_SER_THIS s[i].int_ident.int_ID, BASE_HEX);
    bx_list_c *fifo = new bx_list_c(port, "fifo_cntl");
    new bx_shadow_bool_c(fifo, "enable", &BX_SER_THIS s[i].fifo_cntl.enable);
    new bx_shadow_num_c(fifo, "rxtrigger", &BX_SER_THIS s[i].fifo_cntl.rxtrigger, BASE_HEX);
    bx_list_c *lcntl = new bx_list_c(port, "line_cntl");
    new bx_shadow_num_c(lcntl, "wordlen_sel", &BX_SER_THIS s[i].line_cntl.wordlen_sel, BASE_HEX);
    new bx_shadow_bool_c(lcntl, "stopbits", &BX_SER_THIS s[i].line_cntl.stopbits);
    new bx_shadow_bool_c(lcntl, "parity_enable", &BX_SER_THIS s[i].line_cntl.parity_enable);
    new bx_shadow_bool_c(lcntl, "evenparity_sel", &BX_SER_THIS s[i].line_cntl.evenparity_sel);
    new bx_shadow_bool_c(lcntl, "stick_parity", &BX_SER_THIS s[i].line_cntl.stick_parity);
    new bx_shadow_bool_c(lcntl, "break_cntl", &BX_SER_THIS s[i].line_cntl.break_cntl);
    new bx_shadow_bool_c(lcntl, "dlab", &BX_SER_THIS s[i].line_cntl.dlab);
    bx_list_c *mcntl = new bx_list_c(port, "modem_cntl");
    new bx_shadow_bool_c(mcntl, "dtr", &BX_SER_THIS s[i].modem_cntl.dtr);
    new bx_shadow_bool_c(mcntl, "rts", &BX_SER_THIS s[i].modem_cntl.rts);
    new bx_shadow_bool_c(mcntl, "out1", &BX_SER_THIS s[i].modem_cntl.out1);
    new bx_shadow_bool_c(mcntl, "out2", &BX_SER_THIS s[i].modem_cntl.out2);
    new bx_shadow_bool_c(mcntl, "local_loopback", &BX_SER_THIS s[i].modem_cntl.local_loopback);
    bx_list_c *lstatus = new bx_list_c(port, "line_status");
    new bx_shadow_bool_c(lstatus, "rxdata_ready", &BX_SER_THIS s[i].line_status.rxdata_ready);
    new bx_shadow_bool_c(lstatus, "overrun_error", &BX_SER_THIS s[i].line_status.overrun_error);
    new bx_shadow_bool_c(lstatus, "parity_error", &BX_SER_THIS s[i].line_status.parity_error);
    new bx_shadow_bool_c(lstatus, "framing_error", &BX_SER_THIS s[i].line_status.framing_error);
    new bx_shadow_bool_c(lstatus, "break_int", &BX_SER_THIS s[i].line_status.break_int);
    new bx_shadow_bool_c(lstatus, "thr_empty", &BX_SER_THIS s[i].line_status.thr_empty);
    new bx_shadow_bool_c(lstatus, "tsr_empty", &BX_SER_THIS s[i].line_status.tsr_empty);
    new bx_shadow_bool_c(lstatus, "fifo_error", &BX_SER_THIS s[i].line_status.fifo_error);
    bx_list_c *mstatus = new bx_list_c(port, "modem_status");
    new bx_shadow_bool_c(mstatus, "delta_cts", &BX_SER_THIS s[i].modem_status.delta_cts);
    new bx_shadow_bool_c(mstatus, "delta_dsr", &BX_SER_THIS s[i].modem_status.delta_dsr);
    new bx_shadow_bool_c(mstatus, "ri_trailedge", &BX_SER_THIS s[i].modem_status.ri_trailedge);
    new bx_shadow_bool_c(mstatus, "delta_dcd", &BX_SER_THIS s[i].modem_status.delta_dcd);
    new bx_shadow_bool_c(mstatus, "cts", &BX_SER_THIS s[i].modem_status.cts);
    new bx_shadow_bool_c(mstatus, "dsr", &BX_SER_THIS s[i].modem_status.dsr);
    new bx_shadow_bool_c(mstatus, "ri", &BX_SER_THIS s[i].modem_status.ri);
    new bx_shadow_bool_c(mstatus, "dcd", &BX_SER_THIS s[i].modem_status.dcd);
    new bx_shadow_num_c(port, "scratch", &BX_SER_THIS s[i].scratch, BASE_HEX);
    new bx_shadow_num_c(port, "tsrbuffer", &BX_SER_THIS s[i].tsrbuffer, BASE_HEX);
    bx_list_c *rxfifo = new bx_list_c(port, "rx_fifo");
    for (j=0; j<16; j++) {
      sprintf(name, "0x%02x", j);
      new bx_shadow_num_c(rxfifo, name, &BX_SER_THIS s[i].rx_fifo[j], BASE_HEX);
    }
    bx_list_c *txfifo = new bx_list_c(port, "tx_fifo");
    for (j=0; j<16; j++) {
      sprintf(name, "0x%02x", j);
      new bx_shadow_num_c(txfifo, name, &BX_SER_THIS s[i].tx_fifo[j], BASE_HEX);
    }
    new bx_shadow_num_c(port, "divisor_lsb", &BX_SER_THIS s[i].divisor_lsb, BASE_HEX);
    new bx_shadow_num_c(port, "divisor_msb", &BX_SER_THIS s[i].divisor_msb, BASE_HEX);
  }
  new bx_shadow_num_c(list, "detect_mouse", &BX_SER_THIS detect_mouse);
  new bx_shadow_num_c(list, "mouse_delayed_dx", &BX_SER_THIS mouse_delayed_dx);
  new bx_shadow_num_c(list, "mouse_delayed_dy", &BX_SER_THIS mouse_delayed_dy);
  new bx_shadow_num_c(list, "mouse_delayed_dz", &BX_SER_THIS mouse_delayed_dz);
  new bx_shadow_num_c(list, "mouse_buttons", &BX_SER_THIS mouse_buttons);
  new bx_shadow_bool_c(list, "mouse_update", &BX_SER_THIS mouse_update);
  bx_list_c *mousebuf = new bx_list_c(list, "mouse_internal_buffer");
  new bx_shadow_num_c(mousebuf, "num_elements", &BX_SER_THIS mouse_internal_buffer.num_elements);
  bx_list_c *buffer = new bx_list_c(mousebuf, "buffer");
  for (i=0; i<BX_MOUSE_BUFF_SIZE; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(buffer, name, &BX_SER_THIS mouse_internal_buffer.buffer[i], BASE_HEX);
  }
  new bx_shadow_num_c(mousebuf, "head", &BX_SER_THIS mouse_internal_buffer.head);
}

void bx_serial_c::lower_interrupt(Bit8u port)
{
  /* If there are no more ints pending, clear the irq */
  if ((BX_SER_THIS s[port].rx_interrupt == 0) &&
      (BX_SER_THIS s[port].tx_interrupt == 0) &&
      (BX_SER_THIS s[port].ls_interrupt == 0) &&
      (BX_SER_THIS s[port].ms_interrupt == 0) &&
      (BX_SER_THIS s[port].fifo_interrupt == 0)) {
    DEV_pic_lower_irq(BX_SER_THIS s[port].IRQ);
  }
}

void bx_serial_c::raise_interrupt(Bit8u port, int type)
{
  bx_bool gen_int = 0;

  switch (type) {
    case BX_SER_INT_IER: /* IER has changed */
      gen_int = 1;
      break;
    case BX_SER_INT_RXDATA:
      if (BX_SER_THIS s[port].int_enable.rxdata_enable) {
        BX_SER_THIS s[port].rx_interrupt = 1;
        gen_int = 1;
      } else {
        BX_SER_THIS s[port].rx_ipending = 1;
      }
      break;
    case BX_SER_INT_TXHOLD:
      if (BX_SER_THIS s[port].int_enable.txhold_enable) {
        BX_SER_THIS s[port].tx_interrupt = 1;
        gen_int = 1;
      }
      break;
    case BX_SER_INT_RXLSTAT:
      if (BX_SER_THIS s[port].int_enable.rxlstat_enable) {
        BX_SER_THIS s[port].ls_interrupt = 1;
        gen_int = 1;
      } else {
        BX_SER_THIS s[port].ls_ipending = 1;
      }
      break;
    case BX_SER_INT_MODSTAT:
      if ((BX_SER_THIS s[port].ms_ipending == 1) &&
          (BX_SER_THIS s[port].int_enable.modstat_enable == 1)) {
        BX_SER_THIS s[port].ms_interrupt = 1;
        BX_SER_THIS s[port].ms_ipending = 0;
        gen_int = 1;
      }
      break;
    case BX_SER_INT_FIFO:
      if (BX_SER_THIS s[port].int_enable.rxdata_enable) {
        BX_SER_THIS s[port].fifo_interrupt = 1;
        gen_int = 1;
      } else {
        BX_SER_THIS s[port].fifo_ipending = 1;
      }
      break;
  }
  if (gen_int && BX_SER_THIS s[port].modem_cntl.out2) {
    DEV_pic_raise_irq(BX_SER_THIS s[port].IRQ);
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_serial_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_SER_SMF
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  return class_ptr->read(address, io_len);
}

Bit32u bx_serial_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SER_SMF
  Bit8u offset, val;
  Bit8u port = 0;
  Bit16u ret16;

  if (io_len == 2) {
    ret16 = BX_SER_THIS read_handler(theSerialDevice, address, 1);
    ret16 |= (BX_SER_THIS read_handler(theSerialDevice, address + 1, 1)) << 8;
    return ret16;
  }

  offset = address & 0x07;
  switch (address & 0x03f8) {
    case 0x03f8: port = 0; break;
    case 0x02f8: port = 1; break;
    case 0x03e8: port = 2; break;
    case 0x02e8: port = 3; break;
  }

  switch (offset) {
    case BX_SER_RBR: /* receive buffer, or divisor latch LSB if DLAB set */
      if (BX_SER_THIS s[port].line_cntl.dlab) {
        val = BX_SER_THIS s[port].divisor_lsb;
      } else {
        if (BX_SER_THIS s[port].fifo_cntl.enable) {
          val = BX_SER_THIS s[port].rx_fifo[0];
          if (BX_SER_THIS s[port].rx_fifo_end > 0) {
            memcpy(&BX_SER_THIS s[port].rx_fifo[0], &BX_SER_THIS s[port].rx_fifo[1], 15);
            BX_SER_THIS s[port].rx_fifo_end--;
          }
          if (BX_SER_THIS s[port].rx_fifo_end == 0) {
            BX_SER_THIS s[port].line_status.rxdata_ready = 0;
            BX_SER_THIS s[port].rx_interrupt = 0;
            BX_SER_THIS s[port].rx_ipending = 0;
            BX_SER_THIS s[port].fifo_interrupt = 0;
            BX_SER_THIS s[port].fifo_ipending = 0;
            lower_interrupt(port);
          }
        } else {
          val = BX_SER_THIS s[port].rxbuffer;
          BX_SER_THIS s[port].line_status.rxdata_ready = 0;
          BX_SER_THIS s[port].rx_interrupt = 0;
          BX_SER_THIS s[port].rx_ipending = 0;
          lower_interrupt(port);
        }
      }
      break;

    case BX_SER_IER: /* interrupt enable register, or div. latch MSB */
      if (BX_SER_THIS s[port].line_cntl.dlab) {
        val = BX_SER_THIS s[port].divisor_msb;
      } else {
        val = BX_SER_THIS s[port].int_enable.rxdata_enable |
              (BX_SER_THIS s[port].int_enable.txhold_enable  << 1) |
              (BX_SER_THIS s[port].int_enable.rxlstat_enable << 2) |
              (BX_SER_THIS s[port].int_enable.modstat_enable << 3);
      }
      break;

    case BX_SER_IIR: /* interrupt ID register */
      /*
       * Set the interrupt ID based on interrupt source
       */
      if (BX_SER_THIS s[port].ls_interrupt) {
        BX_SER_THIS s[port].int_ident.int_ID = 0x3;
        BX_SER_THIS s[port].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[port].fifo_interrupt) {
        BX_SER_THIS s[port].int_ident.int_ID = 0x6;
        BX_SER_THIS s[port].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[port].rx_interrupt) {
        BX_SER_THIS s[port].int_ident.int_ID = 0x2;
        BX_SER_THIS s[port].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[port].tx_interrupt) {
        BX_SER_THIS s[port].int_ident.int_ID = 0x1;
        BX_SER_THIS s[port].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[port].ms_interrupt) {
        BX_SER_THIS s[port].int_ident.int_ID = 0x0;
        BX_SER_THIS s[port].int_ident.ipending = 0;
      } else {
        BX_SER_THIS s[port].int_ident.int_ID = 0x0;
        BX_SER_THIS s[port].int_ident.ipending = 1;
      }
      BX_SER_THIS s[port].tx_interrupt = 0;
      lower_interrupt(port);

      val = BX_SER_THIS s[port].int_ident.ipending  |
            (BX_SER_THIS s[port].int_ident.int_ID << 1) |
            (BX_SER_THIS s[port].fifo_cntl.enable ? 0xc0 : 0x00);
      break;

    case BX_SER_LCR: /* Line control register */
      val = BX_SER_THIS s[port].line_cntl.wordlen_sel |
            (BX_SER_THIS s[port].line_cntl.stopbits       << 2) |
            (BX_SER_THIS s[port].line_cntl.parity_enable  << 3) |
            (BX_SER_THIS s[port].line_cntl.evenparity_sel << 4) |
            (BX_SER_THIS s[port].line_cntl.stick_parity   << 5) |
            (BX_SER_THIS s[port].line_cntl.break_cntl     << 6) |
            (BX_SER_THIS s[port].line_cntl.dlab           << 7);
      break;

    case BX_SER_MCR: /* MODEM control register */
      val = BX_SER_THIS s[port].modem_cntl.dtr |
            (BX_SER_THIS s[port].modem_cntl.rts << 1) |
            (BX_SER_THIS s[port].modem_cntl.out1 << 2) |
            (BX_SER_THIS s[port].modem_cntl.out2 << 3) |
            (BX_SER_THIS s[port].modem_cntl.local_loopback << 4);
      break;

    case BX_SER_LSR: /* Line status register */
      val = BX_SER_THIS s[port].line_status.rxdata_ready |
            (BX_SER_THIS s[port].line_status.overrun_error  << 1) |
            (BX_SER_THIS s[port].line_status.parity_error   << 2) |
            (BX_SER_THIS s[port].line_status.framing_error  << 3) |
            (BX_SER_THIS s[port].line_status.break_int      << 4) |
            (BX_SER_THIS s[port].line_status.thr_empty      << 5) |
            (BX_SER_THIS s[port].line_status.tsr_empty      << 6) |
            (BX_SER_THIS s[port].line_status.fifo_error     << 7);
      BX_SER_THIS s[port].line_status.overrun_error = 0;
      BX_SER_THIS s[port].line_status.framing_error = 0;
      BX_SER_THIS s[port].line_status.break_int = 0;
      BX_SER_THIS s[port].ls_interrupt = 0;
      BX_SER_THIS s[port].ls_ipending = 0;
      lower_interrupt(port);
      break;

    case BX_SER_MSR: /* MODEM status register */
#if USE_RAW_SERIAL
      if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
        bx_bool prev_cts = BX_SER_THIS s[port].modem_status.cts;
        bx_bool prev_dsr = BX_SER_THIS s[port].modem_status.dsr;
        bx_bool prev_ri  = BX_SER_THIS s[port].modem_status.ri;
        bx_bool prev_dcd = BX_SER_THIS s[port].modem_status.dcd;

        val = BX_SER_THIS s[port].raw->get_modem_status();
        BX_SER_THIS s[port].modem_status.cts = (val & 0x10) >> 4;
        BX_SER_THIS s[port].modem_status.dsr = (val & 0x20) >> 5;
        BX_SER_THIS s[port].modem_status.ri  = (val & 0x40) >> 6;
        BX_SER_THIS s[port].modem_status.dcd = (val & 0x80) >> 7;
        if (BX_SER_THIS s[port].modem_status.cts != prev_cts) {
          BX_SER_THIS s[port].modem_status.delta_cts = 1;
        }
        if (BX_SER_THIS s[port].modem_status.dsr != prev_dsr) {
          BX_SER_THIS s[port].modem_status.delta_dsr = 1;
        }
        if ((BX_SER_THIS s[port].modem_status.ri == 0) && (prev_ri == 1))
          BX_SER_THIS s[port].modem_status.ri_trailedge = 1;
        if (BX_SER_THIS s[port].modem_status.dcd != prev_dcd) {
          BX_SER_THIS s[port].modem_status.delta_dcd = 1;
        }
      }
#endif
      val = BX_SER_THIS s[port].modem_status.delta_cts |
            (BX_SER_THIS s[port].modem_status.delta_dsr    << 1) |
            (BX_SER_THIS s[port].modem_status.ri_trailedge << 2) |
            (BX_SER_THIS s[port].modem_status.delta_dcd    << 3) |
            (BX_SER_THIS s[port].modem_status.cts          << 4) |
            (BX_SER_THIS s[port].modem_status.dsr          << 5) |
            (BX_SER_THIS s[port].modem_status.ri           << 6) |
            (BX_SER_THIS s[port].modem_status.dcd          << 7);
      BX_SER_THIS s[port].modem_status.delta_cts = 0;
      BX_SER_THIS s[port].modem_status.delta_dsr = 0;
      BX_SER_THIS s[port].modem_status.ri_trailedge = 0;
      BX_SER_THIS s[port].modem_status.delta_dcd = 0;
      BX_SER_THIS s[port].ms_interrupt = 0;
      BX_SER_THIS s[port].ms_ipending = 0;
      lower_interrupt(port);
      break;

    case BX_SER_SCR: /* scratch register */
      val = BX_SER_THIS s[port].scratch;
      break;

    default:
      val = 0; // keep compiler happy
      BX_PANIC(("unsupported io read from address=0x%04x!", address));
      break;
  }

  BX_DEBUG(("com%d register read from address: 0x%04x = 0x%02x", port+1, address, val));

  return(val);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_serial_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_SER_SMF
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_serial_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SER_SMF
  bx_bool gen_int = 0;
  Bit8u offset, new_wordlen;
#if USE_RAW_SERIAL
  bx_bool mcr_changed = 0;
  Bit8u p_mode;
#endif
  Bit8u port = 0;
  int new_baudrate;
  bx_bool restart_timer = 0;

  if (io_len == 2) {
    BX_SER_THIS write_handler(theSerialDevice, address, (value & 0xff), 1);
    BX_SER_THIS write_handler(theSerialDevice, address + 1, ((value >> 8) & 0xff), 1);
    return;
  }

  offset = address & 0x07;
  switch (address & 0x03f8) {
    case 0x03f8: port = 0; break;
    case 0x02f8: port = 1; break;
    case 0x03e8: port = 2; break;
    case 0x02e8: port = 3; break;
  }

  BX_DEBUG(("com%d register write to  address: 0x%04x = 0x%02x", port+1, address, value));

  bx_bool new_b0 = value & 0x01;
  bx_bool new_b1 = (value & 0x02) >> 1;
  bx_bool new_b2 = (value & 0x04) >> 2;
  bx_bool new_b3 = (value & 0x08) >> 3;
  bx_bool new_b4 = (value & 0x10) >> 4;
  bx_bool new_b5 = (value & 0x20) >> 5;
  bx_bool new_b6 = (value & 0x40) >> 6;
  bx_bool new_b7 = (value & 0x80) >> 7;

  switch (offset) {
    case BX_SER_THR: /* transmit buffer, or divisor latch LSB if DLAB set */
      if (BX_SER_THIS s[port].line_cntl.dlab) {
        BX_SER_THIS s[port].divisor_lsb = value;
      } else {
        Bit8u bitmask = 0xff >> (3 - BX_SER_THIS s[port].line_cntl.wordlen_sel);
        value &= bitmask;
        if (BX_SER_THIS s[port].line_status.thr_empty) {
          if (BX_SER_THIS s[port].fifo_cntl.enable) {
            BX_SER_THIS s[port].tx_fifo[BX_SER_THIS s[port].tx_fifo_end++] = value;
          } else {
            BX_SER_THIS s[port].thrbuffer = value;
          }
          BX_SER_THIS s[port].line_status.thr_empty = 0;
          if (BX_SER_THIS s[port].line_status.tsr_empty) {
            if (BX_SER_THIS s[port].fifo_cntl.enable) {
              BX_SER_THIS s[port].tsrbuffer = BX_SER_THIS s[port].tx_fifo[0];
              memcpy(&BX_SER_THIS s[port].tx_fifo[0], &BX_SER_THIS s[port].tx_fifo[1], 15);
              BX_SER_THIS s[port].line_status.thr_empty = (--BX_SER_THIS s[port].tx_fifo_end == 0);
            } else {
              BX_SER_THIS s[port].tsrbuffer = BX_SER_THIS s[port].thrbuffer;
              BX_SER_THIS s[port].line_status.thr_empty = 1;
            }
            BX_SER_THIS s[port].line_status.tsr_empty = 0;
            raise_interrupt(port, BX_SER_INT_TXHOLD);
            bx_pc_system.activate_timer(BX_SER_THIS s[port].tx_timer_index,
                                        BX_SER_THIS s[port].databyte_usec,
                                        0); /* not continuous */
          } else {
            BX_SER_THIS s[port].tx_interrupt = 0;
            lower_interrupt(port);
          }
        } else {
          if (BX_SER_THIS s[port].fifo_cntl.enable) {
            if (BX_SER_THIS s[port].tx_fifo_end < 16) {
              BX_SER_THIS s[port].tx_fifo[BX_SER_THIS s[port].tx_fifo_end++] = value;
            } else {
              BX_ERROR(("com%d: transmit FIFO overflow", port+1));
            }
          } else {
            BX_ERROR(("com%d: write to tx hold register when not empty", port+1));
          }
        }
      }
      break;

    case BX_SER_IER: /* interrupt enable register, or div. latch MSB */
      if (BX_SER_THIS s[port].line_cntl.dlab) {
        BX_SER_THIS s[port].divisor_msb = value;
      } else {
        if (new_b3 != BX_SER_THIS s[port].int_enable.modstat_enable) {
          BX_SER_THIS s[port].int_enable.modstat_enable  = new_b3;
          if (BX_SER_THIS s[port].int_enable.modstat_enable == 1) {
            if (BX_SER_THIS s[port].ms_ipending == 1) {
              BX_SER_THIS s[port].ms_interrupt = 1;
              BX_SER_THIS s[port].ms_ipending = 0;
              gen_int = 1;
            }
          } else {
            if (BX_SER_THIS s[port].ms_interrupt == 1) {
              BX_SER_THIS s[port].ms_interrupt = 0;
              BX_SER_THIS s[port].ms_ipending = 1;
              lower_interrupt(port);
            }
          }
        }
        if (new_b1 != BX_SER_THIS s[port].int_enable.txhold_enable) {
          BX_SER_THIS s[port].int_enable.txhold_enable  = new_b1;
          if (BX_SER_THIS s[port].int_enable.txhold_enable == 1) {
            BX_SER_THIS s[port].tx_interrupt = BX_SER_THIS s[port].line_status.thr_empty;
            if (BX_SER_THIS s[port].tx_interrupt) gen_int = 1;
          } else {
            BX_SER_THIS s[port].tx_interrupt = 0;
            lower_interrupt(port);
          }
        }
        if (new_b0 != BX_SER_THIS s[port].int_enable.rxdata_enable) {
          BX_SER_THIS s[port].int_enable.rxdata_enable  = new_b0;
          if (BX_SER_THIS s[port].int_enable.rxdata_enable == 1) {
            if (BX_SER_THIS s[port].fifo_ipending == 1) {
              BX_SER_THIS s[port].fifo_interrupt = 1;
              BX_SER_THIS s[port].fifo_ipending = 0;
              gen_int = 1;
            }
            if (BX_SER_THIS s[port].rx_ipending == 1) {
              BX_SER_THIS s[port].rx_interrupt = 1;
              BX_SER_THIS s[port].rx_ipending = 0;
              gen_int = 1;
            }
          } else {
            if (BX_SER_THIS s[port].rx_interrupt == 1) {
              BX_SER_THIS s[port].rx_interrupt = 0;
              BX_SER_THIS s[port].rx_ipending = 1;
              lower_interrupt(port);
            }
            if (BX_SER_THIS s[port].fifo_interrupt == 1) {
              BX_SER_THIS s[port].fifo_interrupt = 0;
              BX_SER_THIS s[port].fifo_ipending = 1;
              lower_interrupt(port);
            }
          }
        }
        if (new_b2 != BX_SER_THIS s[port].int_enable.rxlstat_enable) {
          BX_SER_THIS s[port].int_enable.rxlstat_enable  = new_b2;
          if (BX_SER_THIS s[port].int_enable.rxlstat_enable == 1) {
            if (BX_SER_THIS s[port].ls_ipending == 1) {
              BX_SER_THIS s[port].ls_interrupt = 1;
              BX_SER_THIS s[port].ls_ipending = 0;
              gen_int = 1;
            }
          } else {
            if (BX_SER_THIS s[port].ls_interrupt == 1) {
              BX_SER_THIS s[port].ls_interrupt = 0;
              BX_SER_THIS s[port].ls_ipending = 1;
              lower_interrupt(port);
            }
          }
        }
        if (gen_int) raise_interrupt(port, BX_SER_INT_IER);
      }
      break;

    case BX_SER_FCR: /* FIFO control register */
      if (new_b0 && !BX_SER_THIS s[port].fifo_cntl.enable) {
        BX_INFO(("com%d: FIFO enabled", port+1));
        BX_SER_THIS s[port].rx_fifo_end = 0;
        BX_SER_THIS s[port].tx_fifo_end = 0;
      }
      BX_SER_THIS s[port].fifo_cntl.enable = new_b0;
      if (new_b1) {
        BX_SER_THIS s[port].rx_fifo_end = 0;
      }
      if (new_b2) {
        BX_SER_THIS s[port].tx_fifo_end = 0;
      }
      BX_SER_THIS s[port].fifo_cntl.rxtrigger = (value & 0xc0) >> 6;
      break;

    case BX_SER_LCR: /* Line control register */
      new_wordlen = value & 0x03;
#if USE_RAW_SERIAL
      if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
        if (BX_SER_THIS s[port].line_cntl.wordlen_sel != new_wordlen) {
          BX_SER_THIS s[port].raw->set_data_bits(new_wordlen + 5);
        }
        if (new_b2 != BX_SER_THIS s[port].line_cntl.stopbits) {
          BX_SER_THIS s[port].raw->set_stop_bits(new_b2 ? 2 : 1);
        }
        if ((new_b3 != BX_SER_THIS s[port].line_cntl.parity_enable) ||
            (new_b4 != BX_SER_THIS s[port].line_cntl.evenparity_sel) ||
            (new_b5 != BX_SER_THIS s[port].line_cntl.stick_parity)) {
          if (new_b3 == 0) {
            p_mode = P_NONE;
          } else {
            p_mode = ((value & 0x30) >> 4) + 1;
          }
          BX_SER_THIS s[port].raw->set_parity_mode(p_mode);
        }
        if ((new_b6 != BX_SER_THIS s[port].line_cntl.break_cntl) &&
            (!BX_SER_THIS s[port].modem_cntl.local_loopback)) {
          BX_SER_THIS s[port].raw->set_break(new_b6);
        }
      }
#endif // USE_RAW_SERIAL
      /* These are ignored, but set them up so they can be read back */
      BX_SER_THIS s[port].line_cntl.stopbits = new_b2;
      BX_SER_THIS s[port].line_cntl.parity_enable = new_b3;
      BX_SER_THIS s[port].line_cntl.evenparity_sel = new_b4;
      BX_SER_THIS s[port].line_cntl.stick_parity = new_b5;
      BX_SER_THIS s[port].line_cntl.break_cntl = new_b6;
      if (BX_SER_THIS s[port].modem_cntl.local_loopback &&
          BX_SER_THIS s[port].line_cntl.break_cntl) {
        BX_SER_THIS s[port].line_status.break_int = 1;
        BX_SER_THIS s[port].line_status.framing_error = 1;
        rx_fifo_enq(port, 0x00);
      }
      if (!new_b7 && BX_SER_THIS s[port].line_cntl.dlab) {
        if ((BX_SER_THIS s[port].divisor_lsb | BX_SER_THIS s[port].divisor_msb) != 0) {
          new_baudrate = (int)(BX_PC_CLOCK_XTL /
                               (16 * ((BX_SER_THIS s[port].divisor_msb << 8) |
                               BX_SER_THIS s[port].divisor_lsb)));
          if (new_baudrate != BX_SER_THIS s[port].baudrate) {
            BX_SER_THIS s[port].baudrate = new_baudrate;
            restart_timer = 1;
            BX_DEBUG(("com%d: baud rate set to %d", port+1, BX_SER_THIS s[port].baudrate));
#if USE_RAW_SERIAL
            if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
              BX_SER_THIS s[port].raw->set_baudrate(BX_SER_THIS s[port].baudrate);
            }
#endif
          }
        } else {
          BX_ERROR(("com%d: ignoring invalid baud rate divisor", port+1));
        }
      }
      BX_SER_THIS s[port].line_cntl.dlab = new_b7;
      if (new_wordlen != BX_SER_THIS s[port].line_cntl.wordlen_sel) {
        BX_SER_THIS s[port].line_cntl.wordlen_sel = new_wordlen;
        restart_timer = 1;
      }
      if (restart_timer) {
        // Start the receive polling process if not already started
        // and there is a valid baudrate.
        BX_SER_THIS s[port].databyte_usec = (Bit32u)(1000000.0 / BX_SER_THIS s[port].baudrate *
                                                     (BX_SER_THIS s[port].line_cntl.wordlen_sel + 7));
        bx_pc_system.activate_timer(BX_SER_THIS s[port].rx_timer_index,
                                    BX_SER_THIS s[port].databyte_usec,
                                    0); /* not continuous */
      }
      break;

    case BX_SER_MCR: /* MODEM control register */
      if ((BX_SER_THIS s[port].io_mode == BX_SER_MODE_MOUSE) &&
          ((BX_SER_THIS s[port].line_cntl.wordlen_sel == 2) ||
           (BX_SER_THIS s[port].line_cntl.wordlen_sel == 3))) {
        if (!BX_SER_THIS s[port].modem_cntl.dtr && new_b0) {
          BX_SER_THIS detect_mouse = 1;
        }
        if ((BX_SER_THIS detect_mouse == 1) && new_b1) {
          BX_SER_THIS detect_mouse = 2;
        }
      }
#if USE_RAW_SERIAL
      if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
        mcr_changed = (BX_SER_THIS s[port].modem_cntl.dtr != new_b0) |
                      (BX_SER_THIS s[port].modem_cntl.rts != new_b1);
      }
#endif
      BX_SER_THIS s[port].modem_cntl.dtr  = new_b0;
      BX_SER_THIS s[port].modem_cntl.rts  = new_b1;
      BX_SER_THIS s[port].modem_cntl.out1 = new_b2;
      BX_SER_THIS s[port].modem_cntl.out2 = new_b3;

      if (new_b4 != BX_SER_THIS s[port].modem_cntl.local_loopback) {
        BX_SER_THIS s[port].modem_cntl.local_loopback = new_b4;
        if (BX_SER_THIS s[port].modem_cntl.local_loopback) {
          /* transition to loopback mode */
#if USE_RAW_SERIAL
          if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
            if (BX_SER_THIS s[port].modem_cntl.dtr ||
                BX_SER_THIS s[port].modem_cntl.rts) {
              BX_SER_THIS s[port].raw->set_modem_control(0);
            }
          }
#endif
          if (BX_SER_THIS s[port].line_cntl.break_cntl) {
#if USE_RAW_SERIAL
            if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
              BX_SER_THIS s[port].raw->set_break(0);
            }
#endif
            BX_SER_THIS s[port].line_status.break_int = 1;
            BX_SER_THIS s[port].line_status.framing_error = 1;
            rx_fifo_enq(port, 0x00);
          }
        } else {
          /* transition to normal mode */
#if USE_RAW_SERIAL
          if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
            mcr_changed = 1;
            if (BX_SER_THIS s[port].line_cntl.break_cntl) {
              BX_SER_THIS s[port].raw->set_break(0);
            }
          }
#endif
        }
      }

      if (BX_SER_THIS s[port].modem_cntl.local_loopback) {
        bx_bool prev_cts = BX_SER_THIS s[port].modem_status.cts;
        bx_bool prev_dsr = BX_SER_THIS s[port].modem_status.dsr;
        bx_bool prev_ri  = BX_SER_THIS s[port].modem_status.ri;
        bx_bool prev_dcd = BX_SER_THIS s[port].modem_status.dcd;
        BX_SER_THIS s[port].modem_status.cts = BX_SER_THIS s[port].modem_cntl.rts;
        BX_SER_THIS s[port].modem_status.dsr = BX_SER_THIS s[port].modem_cntl.dtr;
        BX_SER_THIS s[port].modem_status.ri  = BX_SER_THIS s[port].modem_cntl.out1;
        BX_SER_THIS s[port].modem_status.dcd = BX_SER_THIS s[port].modem_cntl.out2;
        if (BX_SER_THIS s[port].modem_status.cts != prev_cts) {
          BX_SER_THIS s[port].modem_status.delta_cts = 1;
          BX_SER_THIS s[port].ms_ipending = 1;
        }
        if (BX_SER_THIS s[port].modem_status.dsr != prev_dsr) {
          BX_SER_THIS s[port].modem_status.delta_dsr = 1;
          BX_SER_THIS s[port].ms_ipending = 1;
        }
        if (BX_SER_THIS s[port].modem_status.ri != prev_ri)
          BX_SER_THIS s[port].ms_ipending = 1;
        if ((BX_SER_THIS s[port].modem_status.ri == 0) && (prev_ri == 1))
          BX_SER_THIS s[port].modem_status.ri_trailedge = 1;
        if (BX_SER_THIS s[port].modem_status.dcd != prev_dcd) {
          BX_SER_THIS s[port].modem_status.delta_dcd = 1;
          BX_SER_THIS s[port].ms_ipending = 1;
        }
        raise_interrupt(port, BX_SER_INT_MODSTAT);
      } else {
        if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_MOUSE) {
          if (BX_SER_THIS detect_mouse == 2) {
            BX_DEBUG(("com%d: mouse detection mode", port+1));
            if ((BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL) ||
                (BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL_MSYS)) {
              BX_SER_THIS mouse_internal_buffer.head = 0;
              BX_SER_THIS mouse_internal_buffer.num_elements = 1;
              BX_SER_THIS mouse_internal_buffer.buffer[0] = 'M';
            } else if (BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL_WHEEL) {
              BX_SER_THIS mouse_internal_buffer.head = 0;
              BX_SER_THIS mouse_internal_buffer.num_elements = 6;
              BX_SER_THIS mouse_internal_buffer.buffer[0] = 'M';
              BX_SER_THIS mouse_internal_buffer.buffer[1] = 'Z';
              BX_SER_THIS mouse_internal_buffer.buffer[2] = '@';
              BX_SER_THIS mouse_internal_buffer.buffer[3] = '\0';
              BX_SER_THIS mouse_internal_buffer.buffer[4] = '\0';
              BX_SER_THIS mouse_internal_buffer.buffer[5] = '\0';
            }
            bx_pc_system.activate_timer(BX_SER_THIS s[port].rx_timer_index,
                                        BX_SER_THIS s[port].databyte_usec,
                                        0); /* not continuous */
            BX_SER_THIS detect_mouse = 0;
          }
        }

        if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_RAW) {
#if USE_RAW_SERIAL
          if (mcr_changed) {
            BX_SER_THIS s[port].raw->set_modem_control(value & 0x03);
          }
#endif
        } else {
          /* simulate device connected */
          BX_SER_THIS s[port].modem_status.cts = 1;
          BX_SER_THIS s[port].modem_status.dsr = 1;
          BX_SER_THIS s[port].modem_status.ri  = 0;
          BX_SER_THIS s[port].modem_status.dcd = 0;
        }
      }
      break;

    case BX_SER_LSR: /* Line status register */
      BX_ERROR(("com%d: write to line status register ignored", port+1));
      break;

    case BX_SER_MSR: /* MODEM status register */
      BX_ERROR(("com%d: write to MODEM status register ignored", port+1));
      break;

    case BX_SER_SCR: /* scratch register */
      BX_SER_THIS s[port].scratch = value;
      break;

    default:
      BX_PANIC(("unsupported io write to address=0x%04x, value = 0x%02x!",
        (unsigned) address, (unsigned) value));
      break;
  }
}

void bx_serial_c::rx_fifo_enq(Bit8u port, Bit8u data)
{
  bx_bool gen_int = 0;

  if (BX_SER_THIS s[port].fifo_cntl.enable) {
    if (BX_SER_THIS s[port].rx_fifo_end == 16) {
      BX_ERROR(("com%d: receive FIFO overflow", port+1));
      BX_SER_THIS s[port].line_status.overrun_error = 1;
      raise_interrupt(port, BX_SER_INT_RXLSTAT);
    } else {
      BX_SER_THIS s[port].rx_fifo[BX_SER_THIS s[port].rx_fifo_end++] = data;
      switch (BX_SER_THIS s[port].fifo_cntl.rxtrigger) {
        case 1:
          if (BX_SER_THIS s[port].rx_fifo_end == 4) gen_int = 1;
          break;
        case 2:
          if (BX_SER_THIS s[port].rx_fifo_end == 8) gen_int = 1;
          break;
        case 3:
          if (BX_SER_THIS s[port].rx_fifo_end == 14) gen_int = 1;
          break;
        default:
          gen_int = 1;
      }
      if (gen_int) {
        bx_pc_system.deactivate_timer(BX_SER_THIS s[port].fifo_timer_index);
        BX_SER_THIS s[port].line_status.rxdata_ready = 1;
        raise_interrupt(port, BX_SER_INT_RXDATA);
      } else {
        bx_pc_system.activate_timer(BX_SER_THIS s[port].fifo_timer_index,
                                    BX_SER_THIS s[port].databyte_usec * 3,
                                    0); /* not continuous */
      }
    }
  } else {
    if (BX_SER_THIS s[port].line_status.rxdata_ready == 1) {
      BX_ERROR(("com%d: overrun error", port+1));
      BX_SER_THIS s[port].line_status.overrun_error = 1;
      raise_interrupt(port, BX_SER_INT_RXLSTAT);
    }
    BX_SER_THIS s[port].rxbuffer = data;
    BX_SER_THIS s[port].line_status.rxdata_ready = 1;
    raise_interrupt(port, BX_SER_INT_RXDATA);
  }
}

void bx_serial_c::tx_timer_handler(void *this_ptr)
{
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->tx_timer();
}

void bx_serial_c::tx_timer(void)
{
  bx_bool gen_int = 0;
  Bit8u port = (Bit8u)bx_pc_system.triggeredTimerParam();
  char pname[20];

  if (BX_SER_THIS s[port].modem_cntl.local_loopback) {
    rx_fifo_enq(port, BX_SER_THIS s[port].tsrbuffer);
  } else {
    switch (BX_SER_THIS s[port].io_mode) {
      case BX_SER_MODE_FILE:
        if (BX_SER_THIS s[port].output == NULL) {
          sprintf(pname, "ports.serial.%d", port+1);
          bx_list_c *base = (bx_list_c*) SIM->get_param(pname);
          bx_param_string_c *devparam = SIM->get_param_string("dev", base);
          if (!devparam->isempty()) {
            BX_SER_THIS s[port].output = fopen(devparam->getptr(), "wb");
          }
          if (BX_SER_THIS s[port].output == NULL) {
            BX_ERROR(("Could not open '%s' to write com%d output",
                      devparam->getptr(), port+1));
            BX_SER_THIS s[port].io_mode = BX_SER_MODE_NULL;
          }
        }
        fputc(BX_SER_THIS s[port].tsrbuffer, BX_SER_THIS s[port].output);
        fflush(BX_SER_THIS s[port].output);
        break;
      case BX_SER_MODE_TERM:
#if defined(SERIAL_ENABLE)
        BX_DEBUG(("com%d: write: '%c'", port+1, BX_SER_THIS s[port].tsrbuffer));
        if (BX_SER_THIS s[port].tty_id >= 0) {
          write(BX_SER_THIS s[port].tty_id, (bx_ptr_t) & BX_SER_THIS s[port].tsrbuffer, 1);
        }
#endif
        break;
      case BX_SER_MODE_RAW:
#if USE_RAW_SERIAL
        if (!BX_SER_THIS s[port].raw->ready_transmit())
          BX_PANIC(("com%d: not ready to transmit", port+1));
        BX_SER_THIS s[port].raw->transmit(BX_SER_THIS s[port].tsrbuffer);
#endif
        break;
      case BX_SER_MODE_MOUSE:
        BX_INFO(("com%d: write to mouse ignored: 0x%02x", port+1, BX_SER_THIS s[port].tsrbuffer));
        break;
      case BX_SER_MODE_SOCKET_CLIENT:
      case BX_SER_MODE_SOCKET_SERVER:
        if (BX_SER_THIS s[port].socket_id >= 0) {
#ifdef BX_SER_WIN32
          BX_INFO(("attempting to write win32 : %c", BX_SER_THIS s[port].tsrbuffer));
          ::send(BX_SER_THIS s[port].socket_id,
                 (const char*) & BX_SER_THIS s[port].tsrbuffer, 1, 0);
#else
          ::write(BX_SER_THIS s[port].socket_id,
                  (bx_ptr_t) & BX_SER_THIS s[port].tsrbuffer, 1);
#endif
        }
        break;
      case BX_SER_MODE_PIPE_CLIENT:
      case BX_SER_MODE_PIPE_SERVER:
#ifdef BX_SER_WIN32
        if (BX_SER_THIS s[port].pipe) {
          DWORD written;
          WriteFile(BX_SER_THIS s[port].pipe, (bx_ptr_t)& BX_SER_THIS s[port].tsrbuffer, 1, &written, NULL);
        }
#endif
        break;
    }
  }

  BX_SER_THIS s[port].line_status.tsr_empty = 1;
  if (BX_SER_THIS s[port].fifo_cntl.enable && (BX_SER_THIS s[port].tx_fifo_end > 0)) {
    BX_SER_THIS s[port].tsrbuffer = BX_SER_THIS s[port].tx_fifo[0];
    BX_SER_THIS s[port].line_status.tsr_empty = 0;
    memcpy(&BX_SER_THIS s[port].tx_fifo[0], &BX_SER_THIS s[port].tx_fifo[1], 15);
    gen_int = (--BX_SER_THIS s[port].tx_fifo_end == 0);
  } else if (!BX_SER_THIS s[port].line_status.thr_empty) {
    BX_SER_THIS s[port].tsrbuffer = BX_SER_THIS s[port].thrbuffer;
    BX_SER_THIS s[port].line_status.tsr_empty = 0;
    gen_int = 1;
  }
  if (!BX_SER_THIS s[port].line_status.tsr_empty) {
    if (gen_int) {
      BX_SER_THIS s[port].line_status.thr_empty = 1;
      raise_interrupt(port, BX_SER_INT_TXHOLD);
    }
    bx_pc_system.activate_timer(BX_SER_THIS s[port].tx_timer_index,
                                BX_SER_THIS s[port].databyte_usec,
                                0); /* not continuous */
  }
}

void bx_serial_c::rx_timer_handler(void *this_ptr)
{
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->rx_timer();
}

void bx_serial_c::rx_timer(void)
{
#if BX_HAVE_SELECT && defined(SERIAL_ENABLE)
  struct timeval tval;
  fd_set fds;
#endif
  Bit8u port = (Bit8u)bx_pc_system.triggeredTimerParam();
  bx_bool data_ready = 0;
  int db_usec = BX_SER_THIS s[port].databyte_usec;
  unsigned char chbuf = 0;

  if (BX_SER_THIS s[port].io_mode == BX_SER_MODE_TERM) {
#if BX_HAVE_SELECT && defined(SERIAL_ENABLE)
    tval.tv_sec  = 0;
    tval.tv_usec = 0;

// MacOS: I'm not sure what to do with this, since I don't know
// what an fd_set is or what FD_SET() or select() do. They aren't
// declared in the CodeWarrior standard library headers. I'm just
// leaving it commented out for the moment.

    FD_ZERO(&fds);
    if (BX_SER_THIS s[port].tty_id >= 0) FD_SET(BX_SER_THIS s[port].tty_id, &fds);
#endif
  }
  if ((BX_SER_THIS s[port].line_status.rxdata_ready == 0) ||
      (BX_SER_THIS s[port].fifo_cntl.enable)) {
    switch (BX_SER_THIS s[port].io_mode) {
      case BX_SER_MODE_SOCKET_CLIENT:
      case BX_SER_MODE_SOCKET_SERVER:
#if BX_HAVE_SELECT && defined(SERIAL_ENABLE)
        if (BX_SER_THIS s[port].line_status.rxdata_ready == 0) {
          tval.tv_sec  = 0;
          tval.tv_usec = 0;
          FD_ZERO(&fds);
          SOCKET socketid = BX_SER_THIS s[port].socket_id;
          if (socketid >= 0) FD_SET(socketid, &fds);
          if ((socketid >= 0) && (select(socketid+1, &fds, NULL, NULL, &tval) == 1)) {
            ssize_t bytes = (ssize_t)
#ifdef BX_SER_WIN32
              ::recv(socketid, (char*) &chbuf, 1, 0);
#else
                read(socketid, &chbuf, 1);
#endif
            if (bytes > 0) {
              BX_INFO((" -- COM %d : read byte [%d]", port+1, chbuf));
              data_ready = 1;
            }
          }
        }
#endif
        break;
      case BX_SER_MODE_RAW:
#if USE_RAW_SERIAL
        int data;
        if ((data_ready = BX_SER_THIS s[port].raw->ready_receive())) {
          data = BX_SER_THIS s[port].raw->receive();
          if (data < 0) {
            data_ready = 0;
            switch (data) {
              case RAW_EVENT_BREAK:
                BX_SER_THIS s[port].line_status.break_int = 1;
                raise_interrupt(port, BX_SER_INT_RXLSTAT);
                break;
              case RAW_EVENT_FRAME:
                BX_SER_THIS s[port].line_status.framing_error = 1;
                raise_interrupt(port, BX_SER_INT_RXLSTAT);
                break;
              case RAW_EVENT_OVERRUN:
                BX_SER_THIS s[port].line_status.overrun_error = 1;
                raise_interrupt(port, BX_SER_INT_RXLSTAT);
                break;
              case RAW_EVENT_PARITY:
                BX_SER_THIS s[port].line_status.parity_error = 1;
                raise_interrupt(port, BX_SER_INT_RXLSTAT);
                break;
              case RAW_EVENT_CTS_ON:
              case RAW_EVENT_CTS_OFF:
              case RAW_EVENT_DSR_ON:
              case RAW_EVENT_DSR_OFF:
              case RAW_EVENT_RING_ON:
              case RAW_EVENT_RING_OFF:
              case RAW_EVENT_RLSD_ON:
              case RAW_EVENT_RLSD_OFF:
                raise_interrupt(port, BX_SER_INT_MODSTAT);
                break;
            }
          }
        }
        if (data_ready) {
          chbuf = data;
        }
#endif
        break;
      case BX_SER_MODE_TERM:
#if BX_HAVE_SELECT && defined(SERIAL_ENABLE)
        if ((BX_SER_THIS s[port].tty_id >= 0) && (select(BX_SER_THIS s[port].tty_id + 1, &fds, NULL, NULL, &tval) == 1)) {
          (void) read(BX_SER_THIS s[port].tty_id, &chbuf, 1);
          BX_DEBUG(("com%d: read: '%c'", port+1, chbuf));
          data_ready = 1;
        }
#endif
        break;
      case BX_SER_MODE_MOUSE:
        if (BX_SER_THIS mouse_update && (BX_SER_THIS mouse_internal_buffer.num_elements == 0)) {
          BX_SER_THIS update_mouse_data();
        }
        if (BX_SER_THIS mouse_internal_buffer.num_elements > 0) {
          chbuf = BX_SER_THIS mouse_internal_buffer.buffer[BX_SER_THIS mouse_internal_buffer.head];
          BX_SER_THIS mouse_internal_buffer.head = (BX_SER_THIS mouse_internal_buffer.head + 1) %
            BX_MOUSE_BUFF_SIZE;
          BX_SER_THIS mouse_internal_buffer.num_elements--;
          data_ready = 1;
        }
        break;
      case BX_SER_MODE_PIPE_CLIENT:
      case BX_SER_MODE_PIPE_SERVER:
#ifdef BX_SER_WIN32
        DWORD avail = 0;
        if (BX_SER_THIS s[port].pipe &&
            PeekNamedPipe(BX_SER_THIS s[port].pipe, NULL, 0, NULL, &avail, NULL) &&
            avail > 0) {
          ReadFile(BX_SER_THIS s[port].pipe, &chbuf, 1, &avail, NULL);
          data_ready = 1;
        }
#endif
        break;
    }
    if (data_ready) {
      if (!BX_SER_THIS s[port].modem_cntl.local_loopback) {
        rx_fifo_enq(port, chbuf);
      }
    } else {
      if (!BX_SER_THIS s[port].fifo_cntl.enable) {
        db_usec = 100000; // Poll frequency is 100ms
      }
    }
  } else {
    // Poll at 4x baud rate to see if the next-char can
    // be read
    db_usec *= 4;
  }

  bx_pc_system.activate_timer(BX_SER_THIS s[port].rx_timer_index,
                              db_usec, 0); /* not continuous */
}

void bx_serial_c::fifo_timer_handler(void *this_ptr)
{
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->fifo_timer();
}

void bx_serial_c::fifo_timer(void)
{
  Bit8u port = (Bit8u)bx_pc_system.triggeredTimerParam();

  BX_SER_THIS s[port].line_status.rxdata_ready = 1;
  raise_interrupt(port, BX_SER_INT_FIFO);
}

void bx_serial_c::mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  ((bx_serial_c*)dev)->mouse_enq(delta_x, delta_y, delta_z, button_state, absxy);
}

void bx_serial_c::mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  if (BX_SER_THIS mouse_port == -1) {
    BX_ERROR(("mouse not connected to a serial port"));
    return;
  }

  // if the DTR and RTS lines aren't up, the mouse doesn't have any power to send packets.
  if (!BX_SER_THIS s[BX_SER_THIS mouse_port].modem_cntl.dtr || !BX_SER_THIS s[BX_SER_THIS mouse_port].modem_cntl.rts)
    return;

  // scale down the motion
  if ((delta_x < -1) || (delta_x > 1))
    delta_x /= 2;
  if ((delta_y < -1) || (delta_y > 1))
    delta_y /= 2;

  if (delta_x > 127) delta_x = 127;
  if (delta_y > 127) delta_y = 127;
  if (delta_x < -128) delta_x = -128;
  if (delta_y < -128) delta_y = -128;

  BX_SER_THIS mouse_delayed_dx += delta_x;
  BX_SER_THIS mouse_delayed_dy -= delta_y;
  BX_SER_THIS mouse_delayed_dz = delta_z;
  BX_SER_THIS mouse_buttons = button_state;
  BX_SER_THIS mouse_update = 1;
}

void bx_serial_c::update_mouse_data()
{
  int delta_x, delta_y;
  Bit8u b1, b2, b3, button_state, mouse_data[5];
  int bytes, tail;


  if (BX_SER_THIS mouse_delayed_dx > 127) {
    delta_x = 127;
    BX_SER_THIS mouse_delayed_dx -= 127;
  } else if (BX_SER_THIS mouse_delayed_dx < -128) {
    delta_x = -128;
    BX_SER_THIS mouse_delayed_dx += 128;
  } else {
    delta_x = BX_SER_THIS mouse_delayed_dx;
    BX_SER_THIS mouse_delayed_dx = 0;
  }
  if (BX_SER_THIS mouse_delayed_dy > 127) {
    delta_y = 127;
    BX_SER_THIS mouse_delayed_dy -= 127;
  } else if (BX_SER_THIS mouse_delayed_dy < -128) {
    delta_y = -128;
    BX_SER_THIS mouse_delayed_dy += 128;
  } else {
    delta_y = BX_SER_THIS mouse_delayed_dy;
    BX_SER_THIS mouse_delayed_dy = 0;
  }
  button_state = BX_SER_THIS mouse_buttons;

  if (BX_SER_THIS mouse_type != BX_MOUSE_TYPE_SERIAL_MSYS) {
    b1 = (Bit8u) delta_x;
    b2 = (Bit8u) delta_y;
    b3 = (Bit8u) -((Bit8s) BX_SER_THIS mouse_delayed_dz);
    mouse_data[0] = 0x40 | ((b1 & 0xc0) >> 6) | ((b2 & 0xc0) >> 4);
    mouse_data[0] |= ((button_state & 0x01) << 5) | ((button_state & 0x02) << 3);
    mouse_data[1] = b1 & 0x3f;
    mouse_data[2] = b2 & 0x3f;
    mouse_data[3] = b3 & 0x0f;
    mouse_data[3] |= ((button_state & 0x04) << 2);
    bytes = 3;
    if (BX_SER_THIS mouse_type == BX_MOUSE_TYPE_SERIAL_WHEEL) bytes = 4;
  } else {
    b1 = (Bit8u) (delta_x / 2);
    b2 = (Bit8u) -((Bit8s) (delta_y / 2));
    mouse_data[0] = 0x80 | ((~button_state & 0x01) << 2);
    mouse_data[0] |= ((~button_state & 0x06) >> 1);
    mouse_data[1] = b1;
    mouse_data[2] = b2;
    mouse_data[3] = 0;
    mouse_data[4] = 0;
    bytes = 5;
  }

  /* enqueue mouse data in multibyte internal mouse buffer */
  for (int i = 0; i < bytes; i++) {
    tail = (BX_SER_THIS mouse_internal_buffer.head + BX_SER_THIS mouse_internal_buffer.num_elements) %
      BX_MOUSE_BUFF_SIZE;
    BX_SER_THIS mouse_internal_buffer.buffer[tail] = mouse_data[i];
    BX_SER_THIS mouse_internal_buffer.num_elements++;
  }
  BX_SER_THIS mouse_update = 0;
}
