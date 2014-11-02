/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project Team
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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(__MINGW32__) || defined(_MSC_VER)
#include <winsock2.h>
#define SIGTRAP 5
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#define closesocket(s)    close(s)
#endif

#define NEED_CPU_REG_SHORTCUTS 1

#include "bochs.h"
#include "param_names.h"
#include "cpu/cpu.h"

#define LOG_THIS gdbstublog->
#define IFDBG(x) x

static int last_stop_reason = GDBSTUB_STOP_NO_REASON;

#define GDBSTUB_EXECUTION_BREAKPOINT    (0xac1)
#define GDBSTUB_TRACE                   (0xac2)
#define GDBSTUB_USER_BREAK              (0xac3)

static bx_list_c *gdbstub_list;
static int listen_socket_fd;
static int socket_fd;
static logfunctions *gdbstublog;

static int hex(char ch)
{
  if ((ch >= 'a') && (ch <= 'f')) return(ch - 'a' + 10);
  if ((ch >= '0') && (ch <= '9')) return(ch - '0');
  if ((ch >= 'A') && (ch <= 'F')) return(ch - 'A' + 10);
  return(-1);
}

static char buf[4096], *bufptr = buf;

static void flush_debug_buffer()
{
  char *p = buf;
  while (p != bufptr) {
    int n = send(socket_fd, p, bufptr-p, 0);
    if (n == -1) {
      BX_ERROR(("error on debug socket: %m"));
      break;
    }
    p += n;
  }
  bufptr = buf;
}

static void put_debug_char(char ch)
{
  if (bufptr == buf + sizeof buf)
    flush_debug_buffer();
  *bufptr++ = ch;
}

static char get_debug_char(void)
{
  char ch;

  recv(socket_fd, &ch, 1, 0);

  return(ch);
}

static const char hexchars[]="0123456789abcdef";

static void put_reply(const char* buffer)
{
  unsigned char csum;
  int i;

  BX_DEBUG(("put_buffer '%s'", buffer));

  do {
    put_debug_char('$');

    csum = 0;

    i = 0;
    while (buffer[i] != 0)
    {
      put_debug_char(buffer[i]);
      csum = csum + buffer[i];
      i++;
    }

    put_debug_char('#');
    put_debug_char(hexchars[csum >> 4]);
    put_debug_char(hexchars[csum % 16]);
    flush_debug_buffer();
  } while (get_debug_char() != '+');
}

static void get_command(char* buffer)
{
  unsigned char checksum;
  unsigned char xmitcsum;
  char ch;
  unsigned int count;
  unsigned int i;

  do {
    while ((ch = get_debug_char()) != '$');

    checksum = 0;
    xmitcsum = 0;
    count = 0;

    while (1)
    {
      ch = get_debug_char();
      if (ch == '#') break;
      checksum = checksum + ch;
      buffer[count] = ch;
      count++;
    }
    buffer[count] = 0;

    if (ch == '#')
    {
      xmitcsum = hex(get_debug_char()) << 4;
      xmitcsum += hex(get_debug_char());
      if (checksum != xmitcsum)
      {
        BX_INFO(("Bad checksum"));
      }
    }

    if (checksum != xmitcsum)
    {
      put_debug_char('-');
      flush_debug_buffer();
    }
    else
    {
      put_debug_char('+');
      if (buffer[2] == ':')
      {
        put_debug_char(buffer[0]);
        put_debug_char(buffer[1]);
        count = strlen(buffer);
        for (i = 3; i <= count; i++)
        {
          buffer[i - 3] = buffer[i];
        }
      }
      flush_debug_buffer();
    }
  } while (checksum != xmitcsum);
}

void hex2mem(char* buf, unsigned char* mem, int count)
{
  unsigned char ch;

  for (int i = 0; i<count; i++)
  {
    ch = hex(*buf++) << 4;
    ch = ch + hex(*buf++);
    *mem++ = ch;
  }
}

char* mem2hex(const Bit8u* mem, char* buf, int count)
{
  for (int i = 0; i<count; i++)
  {
    Bit8u ch = *mem++;
    *buf++ = hexchars[ch >> 4];
    *buf++ = hexchars[ch % 16];
  }
  *buf = 0;
  return(buf);
}

int hexdigit(char c)
{
  if (isdigit(c))
    return c - '0';
  else if (isupper(c))
    return c - 'A' + 10;
  else
    return c - 'a' + 10;
}

Bit64u read_little_endian_hex(char *&buf)
{
  int byte;
  Bit64u ret = 0;
  int n = 0;
  while (isxdigit(*buf)) {
    byte = hexdigit(*buf++);
    if (isxdigit(*buf))
      byte = (byte << 4) | hexdigit(*buf++);
    ret |= (Bit64u)byte << (n*8);
    ++n;
  }
  return ret;
}

static int continue_thread = -1;
static int other_thread = 0;

#if !BX_SUPPORT_X86_64
#define NUMREGS (16)
#define NUMREGSBYTES (NUMREGS * 4)
static Bit32u registers[NUMREGS];
#endif

#define MAX_BREAKPOINTS (255)
static Bit64u breakpoints[MAX_BREAKPOINTS] = {0,};
static unsigned nr_breakpoints = 0;

static int stub_trace_flag = 0;
static int instr_count = 0;
static int saved_eip = 0;
static int bx_enter_gdbstub = 0;

void bx_gdbstub_break(void)
{
  bx_enter_gdbstub = 1;
}

int bx_gdbstub_check(unsigned int eip)
{
  unsigned int i;
  unsigned char ch;
  int r;
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(_MSC_VER)
  fd_set fds;
  struct timeval tv = {0, 0};
#else
  long arg;
#endif

  if (bx_enter_gdbstub)
  {
    bx_enter_gdbstub = 0;
    last_stop_reason = GDBSTUB_EXECUTION_BREAKPOINT;
    return GDBSTUB_EXECUTION_BREAKPOINT;
  }

  instr_count++;

  if ((instr_count % 500) == 0)
  {
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(_MSC_VER)
    FD_ZERO(&fds);
    FD_SET(socket_fd, &fds);
    r = select(socket_fd + 1, &fds, NULL, NULL, &tv);
    if (r == 1)
    {
      r = recv(socket_fd, (char *)&ch, 1, 0);
    }
#else
    arg = fcntl(socket_fd, F_GETFL);
    fcntl(socket_fd, F_SETFL, arg | O_NONBLOCK);
    r = recv(socket_fd, &ch, 1, 0);
    fcntl(socket_fd, F_SETFL, arg);
#endif
    if (r == 1)
    {
      BX_INFO(("Got byte %x", (unsigned int)ch));
      last_stop_reason = GDBSTUB_USER_BREAK;
      return GDBSTUB_USER_BREAK;
    }
  }

  for (i = 0; i < nr_breakpoints; i++)
  {
    if (eip == breakpoints[i])
    {
      BX_INFO(("found breakpoint at %x", eip));
      last_stop_reason = GDBSTUB_EXECUTION_BREAKPOINT;
      return GDBSTUB_EXECUTION_BREAKPOINT;
    }
  }

  if (stub_trace_flag == 1)
  {
    last_stop_reason = GDBSTUB_TRACE;
    return GDBSTUB_TRACE;
  }
  last_stop_reason = GDBSTUB_STOP_NO_REASON;
  return GDBSTUB_STOP_NO_REASON;
}

static int remove_breakpoint(Bit64u addr, int len)
{
  if (len != 1)
  {
    return(0);
  }

  for (unsigned i = 0; i < MAX_BREAKPOINTS; i++)
  {
    if (breakpoints[i] == addr)
    {
      BX_INFO(("Removing breakpoint at "FMT_ADDRX64, addr));
      breakpoints[i] = 0;
      return(1);
    }
  }
  return(0);
}

static void insert_breakpoint(Bit64u addr)
{
  unsigned int i;

  BX_INFO(("Setting breakpoint at "FMT_ADDRX64, addr));

  for (i = 0; i < (unsigned)MAX_BREAKPOINTS; i++)
  {
    if (breakpoints[i] == 0)
    {
      breakpoints[i] = addr;
      if (i >= nr_breakpoints)
      {
        nr_breakpoints = i + 1;
      }
      return;
    }
  }
  BX_INFO(("No slot for breakpoint"));
}

static void do_pc_breakpoint(int insert, Bit64u addr, int len)
{
  for (int i = 0; i < len; ++i)
    if (insert)
      insert_breakpoint(addr+i);
    else
      remove_breakpoint(addr+i, 1);
}

static void do_breakpoint(int insert, char* buffer)
{
  char* ebuf;
  unsigned long type = strtoul(buffer, &ebuf, 16);
  Bit64u addr = strtoull(ebuf+1, &ebuf, 16);
  unsigned long len = strtoul(ebuf+1, &ebuf, 16);
  switch (type) {
  case 0:
  case 1:
    do_pc_breakpoint(insert, addr, len);
    put_reply("OK");
    break;
  default:
    put_reply("");
    break;
  }
}

static void write_signal(char* buf, int signal)
{
  buf[0] = hexchars[signal >> 4];
  buf[1] = hexchars[signal % 16];
  buf[2] = 0;
}

static int access_linear(Bit64u laddress,
                        unsigned len,
                        unsigned int rw,
                        Bit8u* data)
{
  bx_phy_address phys;
  bx_bool valid;

  if (((laddress & 0xfff) + len) > 4096)
  {
    valid = access_linear(laddress,
                          4096 - (laddress & 0xfff),
                          rw,
                          data);
    if (!valid) return(0);

    valid = access_linear(laddress,
                          len + (laddress & 0xfff) - 4096,
                          rw,
                          (Bit8u *)(data + (4096 - (laddress & 0xfff))));
    return(valid);
  }

  valid = BX_CPU(0)->dbg_xlate_linear2phy(laddress, (bx_phy_address*)&phys);
  if (!valid) return(0);

  if (rw & 1) {
    valid = BX_MEM(0)->dbg_set_mem(phys, len, data);
  } else {
    valid = BX_MEM(0)->dbg_fetch_mem(BX_CPU(0), phys, len, data);
  }

  return(valid);
}

static void debug_loop(void)
{
  char buffer[255];
  char obuf[1024];
  int ne = 0;
  Bit8u mem[255];

  while (ne == 0)
  {
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(0);
    get_command(buffer);
    BX_DEBUG(("get_buffer '%s'", buffer));

    // At a minimum, a stub is required to support the ‘g’ and ‘G’ commands for register access,
    // and the ‘m’ and ‘M’ commands for memory access. Stubs that only control single-threaded
    // targets can implement run control with the ‘c’ (continue), and ‘s’ (step) commands. Stubs
    // that support multi-threading targets should support the ‘vCont’ command. All other commands
    // are optional.

    switch (buffer[0])
    {
      // 'c [addr]' Continue. addr is address to resume.
      // If addr is omitted, resume at current address.
      // This packet is deprecated for multi-threading support. See [vCont packet]
      case 'c':
      {
        char buf[255];
        Bit32u new_eip;

        if (buffer[1] != 0)
        {
          new_eip = (Bit32u) atoi(buffer + 1);

          BX_INFO(("continuing at %x", new_eip));

          for (int i=0; i<BX_SMP_PROCESSORS; i++) {
            BX_CPU(i)->invalidate_prefetch_q();
          }

          saved_eip = EIP;
          BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_EIP].dword.erx = new_eip;
        }

        stub_trace_flag = 0;
        bx_cpu.cpu_loop();

        SIM->refresh_vga();

        if (buffer[1] != 0)
        {
          bx_cpu.invalidate_prefetch_q();
          BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_EIP].dword.erx = saved_eip;
        }

        BX_INFO(("stopped with %x", last_stop_reason));
        buf[0] = 'S';
        if (last_stop_reason == GDBSTUB_EXECUTION_BREAKPOINT ||
            last_stop_reason == GDBSTUB_TRACE)
        {
          write_signal(&buf[1], SIGTRAP);
        }
        else
        {
          write_signal(&buf[1], 0);
        }
        put_reply(buf);
        break;
      }

      // 's [addr]' Single step. addr is the address at which to resume.
      // If addr is omitted, resume at same address.
      // This packet is deprecated for multi-threading support. See [vCont packet]
      case 's':
      {
        char buf[255];

        BX_INFO(("stepping"));
        stub_trace_flag = 1;
        bx_cpu.cpu_loop();
        SIM->refresh_vga();
        stub_trace_flag = 0;
        BX_INFO(("stopped with %x", last_stop_reason));
        buf[0] = 'S';
        if (last_stop_reason == GDBSTUB_EXECUTION_BREAKPOINT ||
            last_stop_reason == GDBSTUB_TRACE)
        {
          write_signal(&buf[1], SIGTRAP);
        }
        else
        {
          write_signal(&buf[1], SIGTRAP);
        }
        put_reply(buf);
        break;
      }

      // ‘M addr,length:XX...’
      // Write length bytes of memory starting at address addr. XX... is the data;
      // each byte is transmitted as a two-digit hexadecimal number.
      case 'M':
      {
        unsigned char mem[255];
        char* ebuf;

        Bit64u addr = strtoull(&buffer[1], &ebuf, 16);
        int len = strtoul(ebuf + 1, &ebuf, 16);
        hex2mem(ebuf + 1, mem, len);

        if (len == 1 && mem[0] == 0xcc)
        {
          insert_breakpoint(addr);
          put_reply("OK");
        }
        else if (remove_breakpoint(addr, len))
        {
          put_reply("OK");
        }
        else
        {
          if (access_linear(addr, len, BX_WRITE, mem))
          {
            put_reply("OK");
          }
          else
          {
            put_reply("Eff");
          }
        }
        break;
      }

      // ‘m addr,length’
      // Read length bytes of memory starting at address addr. Note that addr may
      // not be aligned to any particular boundary.

      // The stub need not use any particular size or alignment when gathering data
      // from memory for the response; even if addr is word-aligned and length is a
      // multiple of the word size, the stub is free to use byte accesses, or not. For
      // this reason, this packet may not be suitable for accessing memory-mapped I/O
      // devices.
      case 'm':
      {
        Bit64u addr;
        int len;
        char* ebuf;

        addr = strtoull(&buffer[1], &ebuf, 16);
        len = strtoul(ebuf + 1, NULL, 16);
        BX_INFO(("addr "FMT_ADDRX64" len %x", addr, len));

        access_linear(addr, len, BX_READ, mem);
        mem2hex(mem, obuf, len);
        put_reply(obuf);
        break;
      }

      // ‘P n...=r...’
      // Write register n... with value r... The register number n is in hexadecimal,
      // and r... contains two hex digits for each byte in the register (target byte order).
      case 'P':
      {
        int reg;
        Bit64u value;
        char* ebuf;

        reg = strtoul(&buffer[1], &ebuf, 16);
        ++ebuf;
        value = read_little_endian_hex(ebuf);

        BX_INFO(("reg %d set to "FMT_ADDRX64, reg, value));
#if BX_SUPPORT_X86_64 == 0
        switch (reg)
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
            BX_CPU_THIS_PTR set_reg32(reg, value);
            break;

          case 8:
            EIP = value;
            BX_CPU_THIS_PTR invalidate_prefetch_q();
            break;

          default:
            break;
        }
#else
        switch (reg)
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
          case 9:
          case 10:
          case 11:
          case 12:
          case 13:
          case 14:
          case 15:
            BX_CPU_THIS_PTR set_reg64(reg, value);
            break;

          case 16:
            RIP = value;
            BX_CPU_THIS_PTR invalidate_prefetch_q();
            break;

          default:
            break;
        }
#endif
        put_reply("OK");

        break;
      }

      // ‘g’ Read general registers.
      case 'g':
      {
#if BX_SUPPORT_X86_64 == 0
        WriteHostDWordToLittleEndian(registers + 0, EAX);
        WriteHostDWordToLittleEndian(registers + 1, ECX);
        WriteHostDWordToLittleEndian(registers + 2, EDX);
        WriteHostDWordToLittleEndian(registers + 3, EBX);
        WriteHostDWordToLittleEndian(registers + 4, ESP);
        WriteHostDWordToLittleEndian(registers + 5, EBP);
        WriteHostDWordToLittleEndian(registers + 6, ESI);
        WriteHostDWordToLittleEndian(registers + 7, EDI);
        if (last_stop_reason == GDBSTUB_EXECUTION_BREAKPOINT)
        {
          WriteHostDWordToLittleEndian(registers + 8, EIP + 1);
        }
        else
        {
          WriteHostDWordToLittleEndian(registers + 8, EIP);
        }
        WriteHostDWordToLittleEndian(registers + 9,
                                     BX_CPU_THIS_PTR read_eflags());
        WriteHostDWordToLittleEndian(registers + 10,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        WriteHostDWordToLittleEndian(registers + 11,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
        WriteHostDWordToLittleEndian(registers + 12,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
        WriteHostDWordToLittleEndian(registers + 13,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
        WriteHostDWordToLittleEndian(registers + 14,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
        WriteHostDWordToLittleEndian(registers + 15,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
        mem2hex((const Bit8u*) registers, obuf, NUMREGSBYTES);
#else
#define PUTREG(buf, val, len) do { \
         Bit64u u = (val); \
         (buf) = mem2hex((const Bit8u*)&u, (buf), (len)); \
      } while (0)
        char* buf = obuf;
        PUTREG(buf, RAX, 8);
        PUTREG(buf, RBX, 8);
        PUTREG(buf, RCX, 8);
        PUTREG(buf, RDX, 8);
        PUTREG(buf, RSI, 8);
        PUTREG(buf, RDI, 8);
        PUTREG(buf, RBP, 8);
        PUTREG(buf, RSP, 8);
        PUTREG(buf, R8,  8);
        PUTREG(buf, R9,  8);
        PUTREG(buf, R10, 8);
        PUTREG(buf, R11, 8);
        PUTREG(buf, R12, 8);
        PUTREG(buf, R13, 8);
        PUTREG(buf, R14, 8);
        PUTREG(buf, R15, 8);
        Bit64u rip;
        rip = RIP;
        if (last_stop_reason == GDBSTUB_EXECUTION_BREAKPOINT)
        {
          ++rip;
        }
        PUTREG(buf, rip, 8);
        PUTREG(buf, BX_CPU_THIS_PTR read_eflags(), 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value, 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value, 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value, 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value, 4);
        PUTREG(buf, BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value, 4);
#endif
        put_reply(obuf);
        break;
      }

      case '?':
        sprintf(obuf, "S%02x", SIGTRAP);
        put_reply(obuf);
        break;

      // ‘H op thread-id’
      // Set thread for subsequent operations (‘m’, ‘M’, ‘g’, ‘G’, et.al.). op depends on the
      // operation to be performed: it should be ‘c’ for step and continue operations
      // (note that this is deprecated, supporting the ‘vCont’ command is a better option),
      // ‘g’ for other operations. The thread designator thread-id has the format
      // and interpretation described in [thread-id syntax]
      case 'H':
        if (buffer[1] == 'c')
        {
          continue_thread = strtol(&buffer[2], NULL, 16);
          put_reply("OK");
        }
        else if (buffer[1] == 'g')
        {
          other_thread = strtol(&buffer[2], NULL, 16);
          put_reply("OK");
        }
        else
        {
          put_reply("Eff");
        }
        break;

      // ‘q name params...’
      // ‘Q name params...’
      // General query (‘q’) and set (‘Q’). These packets are described fully in
      // Section E.4 [General Query Packets]
      case 'q':
        if (buffer[1] == 'C')
        {
          sprintf(obuf, FMT_ADDRX64, (Bit64u)1);
          put_reply(obuf);
        }
        else if (strncmp(&buffer[1], "Offsets", strlen("Offsets")) == 0)
        {
          sprintf(obuf, "Text=%x;Data=%x;Bss=%x",
                  SIM->get_param_num("text_base", gdbstub_list)->get(),
                  SIM->get_param_num("data_base", gdbstub_list)->get(),
                  SIM->get_param_num("bss_base", gdbstub_list)->get());
          put_reply(obuf);
        }
        else if (strncmp(&buffer[1], "Supported", strlen("Supported")) == 0)
        {
          put_reply("");
        }
        else
        {
          put_reply(""); /* not supported */
        }
        break;

      // ‘z type,addr,kind’
      // ‘Z type,addr,kind’
      // Insert (‘Z’) or remove (‘z’) a type breakpoint or watchpoint starting at address
      // address of kind kind.
      case 'Z':
        do_breakpoint(1, buffer+1);
        break;
      case 'z':
        do_breakpoint(0, buffer+1);
        break;

      // ‘k’ Kill request.
      case 'k':
        BX_PANIC(("Debugger asked us to quit"));
        break;

      case 'D':
        BX_INFO(("Debugger detached"));
        put_reply("OK");
        return;
        break;

      default:
        put_reply("");
        break;
    }
  }
}

static void wait_for_connect(int portn)
{
  struct sockaddr_in sockaddr;
  socklen_t sockaddr_len;
  struct protoent *protoent;
  int r;
  int opt;

  listen_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (listen_socket_fd == -1)
  {
    BX_PANIC(("Failed to create socket"));
    exit(1);
  }

  /* Allow rapid reuse of this port */
  opt = 1;
#if defined(__MINGW32__) || defined(_MSC_VER)
  r = setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
  r = setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
  if (r == -1)
  {
    BX_INFO(("setsockopt(SO_REUSEADDR) failed"));
  }

  memset (&sockaddr, '\000', sizeof sockaddr);
#if BX_HAVE_SOCKADDR_IN_SIN_LEN
  // if you don't have sin_len change that to #if 0.  This is the subject of
  // bug [ 626840 ] no 'sin_len' in 'struct sockaddr_in'.
  sockaddr.sin_len = sizeof sockaddr;
#endif
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(portn);
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  r = bind(listen_socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
  if (r == -1)
  {
    BX_PANIC(("Failed to bind socket"));
  }

  r = listen(listen_socket_fd, 0);
  if (r == -1)
  {
    BX_PANIC(("Failed to listen on socket"));
  }

  sockaddr_len = sizeof sockaddr;
  socket_fd = accept(listen_socket_fd, (struct sockaddr *)&sockaddr, &sockaddr_len);
  if (socket_fd == -1)
  {
    BX_PANIC(("Failed to accept on socket"));
  }
  close(listen_socket_fd);

  protoent = getprotobyname ("tcp");
  if (!protoent)
  {
    BX_INFO(("getprotobyname (\"tcp\") failed"));
    return;
  }

  /* Disable Nagle - allow small packets to be sent without delay. */
  opt = 1;
#if defined(__MINGW32__) || defined(_MSC_VER)
  r = setsockopt (socket_fd, protoent->p_proto, TCP_NODELAY, (const char *)&opt, sizeof(opt));
#else
  r = setsockopt (socket_fd, protoent->p_proto, TCP_NODELAY, &opt, sizeof(opt));
#endif
  if (r == -1)
  {
    BX_INFO(("setsockopt(TCP_NODELAY) failed"));
  }
  Bit32u ip = sockaddr.sin_addr.s_addr;
  printf("Connected to %d.%d.%d.%d\n", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);
}

void bx_gdbstub_init(void)
{
  gdbstublog = new logfunctions();
  gdbstublog->put("GDBST");
  gdbstublog->setonoff(LOGLEV_PANIC, ACT_FATAL);

  gdbstub_list = (bx_list_c*) SIM->get_param(BXPN_GDBSTUB);
  int portn = SIM->get_param_num("port", gdbstub_list)->get();

#if defined(__MINGW32__) || defined(_MSC_VER)
  WSADATA wsaData;
  WSAStartup(2, &wsaData);
#endif

  /* Wait for connect */
  printf("Waiting for gdb connection on port %d\n", portn);
  wait_for_connect(portn);

  /* Do debugger command loop */
  debug_loop();

  /* CPU loop */
  bx_cpu.cpu_loop();
}
