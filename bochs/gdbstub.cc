#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

#define NEED_CPU_REG_SHORTCUTS 1

#include "bochs.h"

#define LOG_THIS genlog->
#define IFDBG(x) x

static int last_stop_reason = GDBSTUB_STOP_NO_REASON;

#define GDBSTUB_EXECUTION_BREAKPOINT    (0xac1)
#define GDBSTUB_TRACE                   (0xac2)
#define GDBSTUB_USER_BREAK              (0xac3)

static int listen_socket_fd;
static int socket_fd;

static int hex(char ch)
{
   if ((ch >= 'a') && (ch <= 'f')) return(ch - 'a' + 10);
   if ((ch >= '0') && (ch <= '9')) return(ch - '0');
   if ((ch >= 'A') && (ch <= 'F')) return(ch - 'A' + 10);
   return(-1);
}

static void put_debug_char(char ch)
{
  send(socket_fd, &ch, 1, 0);
}

static char get_debug_char(void)
{
   char ch;
   
   recv(socket_fd, &ch, 1, 0);
   
   return(ch);
}

static const char hexchars[]="0123456789abcdef";

static void put_reply(char* buffer)
{
   unsigned char csum;
   int i;
   
   BX_DEBUG (("put_buffer %s", buffer));
   
   do
     { 
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
     } while (get_debug_char() != '+');
}

static void get_command(char* buffer)
{
   unsigned char checksum;
   unsigned char xmitcsum;
   char ch;
   unsigned int count;
   unsigned int i;
   
   do
     {
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
                 BX_INFO (("Bad checksum"));
              }
         }
       
       if (checksum != xmitcsum)
         {
            put_debug_char('-');
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
         }
     } while (checksum != xmitcsum);   
}

void hex2mem(char* buf, unsigned char* mem, int count)
{
   int i;
   unsigned char ch;
   
   for (i = 0; i<count; i++)
     {
       ch = hex(*buf++) << 4;
       ch = ch + hex(*buf++);
       *mem = ch;
       mem++;
     }
}

char* mem2hex(char* mem, char* buf, int count)
{
   int i;
   unsigned char ch;
   
   for (i = 0; i<count; i++)
     {
       ch = *mem;
       mem++;
       *buf = hexchars[ch >> 4];
       buf++;
       *buf = hexchars[ch % 16];
       buf++;  
     }
   *buf = 0;
   return(buf);
}

static int continue_thread = -1;
static int other_thread = 0;

#define NUMREGS (16)
#define NUMREGSBYTES (NUMREGS * 4)
static int registers[NUMREGS];

#define MAX_BREAKPOINTS (255)
static int breakpoints[MAX_BREAKPOINTS] = {0,};
static int nr_breakpoints = 0;

static int stub_trace_flag = 0;

static int instr_count = 0;

static int saved_eip = 0;

int bx_gdbstub_check(unsigned int eip)
{
   unsigned int i;
   unsigned char ch;
   long arg;
   int r;
   fd_set fds;
   struct timeval tv = {0, 0};

   instr_count++;
   
   if ((instr_count % 500) == 0)
     {
#ifndef __CYGWIN__
       arg = fcntl(socket_fd, F_GETFL);
       fcntl(socket_fd, F_SETFL, arg | O_NONBLOCK);
       r = recv(socket_fd, &ch, 1, 0);
       fcntl(socket_fd, F_SETFL, arg);
#else
       FD_ZERO(&fds);
       FD_SET(socket_fd, &fds);
        r = select(socket_fd + 1, &fds, NULL, NULL, &tv);
       if (r == 1)
         {
           r = recv(socket_fd, &ch, 1, 0);
         }
#endif   
       if (r == 1)
         {
            BX_INFO (("Got byte %x", (unsigned int)ch));
            last_stop_reason = GDBSTUB_USER_BREAK;
            return(GDBSTUB_USER_BREAK);
         }
     }
   
   // why is trace before breakpoints? does that mean it would never
   // hit a breakpoint during tracing?
   if (stub_trace_flag == 1)
     {
       last_stop_reason = GDBSTUB_TRACE;
       return(GDBSTUB_TRACE);
     }
   for (i = 0; i < nr_breakpoints; i++)
     {
       if (eip == breakpoints[i])
         {
            BX_INFO (("found breakpoint at %x", eip));
            last_stop_reason = GDBSTUB_EXECUTION_BREAKPOINT;
            return(GDBSTUB_EXECUTION_BREAKPOINT);
         }
     }
   last_stop_reason = GDBSTUB_STOP_NO_REASON;
   return(GDBSTUB_STOP_NO_REASON);
}

static int remove_breakpoint(int addr, int len)
{
   unsigned int i;
   
   if (len != 1)
     {
       return(0);
     }
   
   for (i = 0; i < MAX_BREAKPOINTS; i++)
     {
       if (breakpoints[i] == addr)
         {
            BX_INFO (("Removing breakpoint at %x", addr));
            breakpoints[i] = 0;
            return(1);
         }
     }
   return(0);
}

static void insert_breakpoint(int addr)
{
   unsigned int i;
   
   BX_INFO (("setting breakpoint at %x", addr));
   
   for (i = 0; i < MAX_BREAKPOINTS; i++)
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
   BX_INFO (("No slot for breakpoint"));
}

static void write_signal(char* buf, int signal)
{
   buf[0] = hexchars[signal >> 4];
   buf[1] = hexchars[signal % 16];
   buf[2] = 0;
}

static int access_linear(Bit32u laddress,
                        unsigned len,
                        unsigned int rw,
                        Bit8u* data)
{
  Bit32u phys;
  bx_bool valid;
   
   if (((laddress & 0xfff) + len) > 4096)
     {
       valid = access_linear(laddress,
                             4096 - (laddress & 0xfff),
                             rw,
                             data);
       if (!valid)
         {
            return(valid);
         }
       valid = access_linear(laddress,
                             len + (laddress & 0xfff) - 4096,
                             rw,
                             (Bit8u *)((unsigned int)data + 
                                      4096 - (laddress & 0xfff)));
       return(valid);
     }
   
   BX_CPU(0)->dbg_xlate_linear2phy((Bit32u)laddress, 
                                       (Bit32u*)&phys, 
                                       (bx_bool*)&valid);
   if (!valid)
     {
       return(0);
     }
   
   if (rw == BX_READ)
     {
       valid = BX_MEM(0)->dbg_fetch_mem(phys, len, data);
     }
   else
     {
       valid = BX_MEM(0)->dbg_set_mem(phys, len, data);
     }
   return(valid);
}

static void debug_loop(void)
{
   char buffer[255];
   char obuf[255];
   int ne;
   unsigned char mem[255];
   
   ne = 0;
   
   while (ne == 0)
     {
       get_command(buffer);
       BX_DEBUG (("get_buffer %s", buffer));
       
       switch (buffer[0])
         {
          case 'c':
              {
                 char buf[255];
                 int new_eip;
                 
                 if (buffer[1] != 0)
                   {
                      new_eip = atoi(buffer + 1);
                      
                      BX_INFO (("continuing at %x", new_eip));
                      
		      for (int i=0; i<BX_SMP_PROCESSORS; i++) {
                        BX_CPU(i)->invalidate_prefetch_q();
		      }
                      
                      saved_eip = EIP;
                      
                      BX_CPU(0)->dword.eip = new_eip;
                   }
                 
                 stub_trace_flag = 0;
                 bx_cpu.cpu_loop(-1);              
                 DEV_vga_refresh();
                 
                 if (buffer[1] != 0)
                   {
                      bx_cpu.invalidate_prefetch_q();
                      
                      BX_CPU_THIS_PTR dword.eip = saved_eip;
                   }
                 
                 BX_INFO (("stopped with %x", last_stop_reason));                               
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
            
          case 's':
              {
                 char buf[255];
                 
                 BX_INFO (("stepping"));
                 stub_trace_flag = 1;
                 bx_cpu.cpu_loop(-1);
                 DEV_vga_refresh();
                 stub_trace_flag = 0;
                 BX_INFO (("stopped with %x", last_stop_reason));
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
            
          case 'M':
              {
                 int addr;
                 int len;
                 unsigned char mem[255];
                 char* ebuf;
                 
                 addr = strtoul(&buffer[1], &ebuf, 16);
                 len = strtoul(ebuf + 1, &ebuf, 16);
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
                      if (access_linear(addr,
                                        len,
                                        BX_WRITE,
                                        mem))
                        {
                           put_reply("OK");
                        }
                      else
                        {
                           put_reply("ENN");
                        }
                   }
                 break;                    
              }
            
          case 'm':
              {
                 int addr;
                 int len;
                 char* ebuf;
                 
                 addr = strtoul(&buffer[1], &ebuf, 16);
                 len = strtoul(ebuf + 1, NULL, 16);
                 BX_INFO (("addr %x len %x", addr, len));
                 
                 access_linear(addr,
                               len,
                               BX_READ,
                               mem);
                 mem2hex((char *)mem, obuf, len);
                 put_reply(obuf);
                 break;
              }
          case 'P':
              {
                 int reg;
                 int value;
                 char* ebuf;
                 
                 reg = strtoul(&buffer[1], &ebuf, 16);
                 value = ntohl(strtoul(ebuf + 1, &ebuf, 16));
                 
                 BX_INFO (("reg %d set to %x", reg, value));
                 
                 switch (reg)
                   {
                    case 1:
                      EAX = value;
                      break;
                      
                    case 2:
                      ECX = value;
                      break;
                      
                    case 3:
                      EBX = value;
                      break;
                      
                    case 4:
                      ESP = value;
                      break;
                      
                    case 5:
                      EBP = value;
                      break;
                      
                    case 6:
                      ESI = value;
                      break;
                      
                    case 7:
                      EDI = value;
                      break;
                      
                    case 8:
                      EIP = value;
                      BX_CPU_THIS_PTR invalidate_prefetch_q();
                      break;
                      
                    default:
                      break;
                   }
                 
                 put_reply("OK");
                 
                 break;
              }
            
          case 'g':
            registers[0] = EAX;
            registers[1] = ECX;
            registers[2] = EDX;
            registers[3] = EBX;
            registers[4] = ESP;
            registers[5] = EBP;
            registers[6] = ESI;
            registers[7] = EDI;
            if (last_stop_reason == GDBSTUB_EXECUTION_BREAKPOINT)
              {
                 registers[8] = EIP + 1;
              }
            else
              {
                 registers[8] = EIP;
              }
            registers[9] = BX_CPU_THIS_PTR read_eflags();
            registers[10] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
            registers[11] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
            registers[12] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
            registers[13] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
            registers[14] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
            registers[15] = 
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
            mem2hex((char *)registers, obuf, NUMREGSBYTES);
            put_reply(obuf);
            break;
            
          case '?':
            sprintf(obuf, "S%02x", SIGTRAP);
            put_reply(obuf);
            break;
            
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
                 put_reply("ENN");
              }
            break;
            
          case 'q':
            if (buffer[1] == 'C')
              {
                 sprintf(obuf,"%Lx", 1);
                 put_reply(obuf);
              }
            else if (strncmp(&buffer[1], "Offsets", strlen("Offsets")) == 0)
              {
                 sprintf(obuf,
                         "Text=%x;Data=%x;Bss=%x",
                         bx_options.gdbstub.text_base, 
                         bx_options.gdbstub.data_base, 
                         bx_options.gdbstub.bss_base);
                 put_reply(obuf);
              }
            else
              {
                 put_reply("ENN");
              }          
            break;
            
          case 'k':
            BX_PANIC (("Debugger asked us to quit\n"));
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
       BX_PANIC (("Failed to create socket\n"));
       exit(1);
     }
   
   /* Allow rapid reuse of this port */
   opt = 1;
   r = setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
   if (r == -1)
     {
       BX_INFO (("setsockopt(SO_REUSEADDR) failed\n"));
     }
   
   memset (&sockaddr, '\000', sizeof sockaddr);
#if BX_HAVE_SOCKADDR_IN_SIN_LEN
   // if you don't have sin_len change that to #if 0.  This is the subject of
   // bug [ 626840 ] no 'sin_len' in 'struct sockaddr_in'.
   sockaddr.sin_len = sizeof sockaddr;
#endif
   sockaddr.sin_family = AF_INET;
   sockaddr.sin_port = htons(portn);
   sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

   r = bind(listen_socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
   if (r == -1)
     {
       BX_PANIC (("Failed to bind socket\n"));
     }

   r = listen(listen_socket_fd, 0);
   if (r == -1)
     {
       BX_PANIC (("Failed to listen on socket\n"));
     }
   
   sockaddr_len = sizeof sockaddr;
   socket_fd = accept(listen_socket_fd, (struct sockaddr *)&sockaddr, &sockaddr_len);
   if (socket_fd == -1)
     {
       BX_PANIC (("Failed to accept on socket\n"));
     }
   close(listen_socket_fd);
   
   protoent = getprotobyname ("tcp");
   if (!protoent)
     {
       BX_INFO (("getprotobyname (\"tcp\") failed\n"));
       return;
     }

   /* Disable Nagle - allow small packets to be sent without delay. */
   opt = 1;
   r = setsockopt (socket_fd, protoent->p_proto, TCP_NODELAY, &opt, sizeof(opt));
   if (r == -1)
     {
       BX_INFO (("setsockopt(TCP_NODELAY) failed\n"));
     }
}

void bx_gdbstub_init(int argc, char* argv[])
{
   int portn = bx_options.gdbstub.port;

   bx_init_hardware();

   /* Wait for connect */

   printf("Waiting for gdb connection on localhost:%d\n", portn);
   wait_for_connect(portn);
   
   /* Do debugger command loop */
   debug_loop();

   /* CPU loop */
   bx_cpu.cpu_loop(-1);
}
