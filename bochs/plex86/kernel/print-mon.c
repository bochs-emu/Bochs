/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  print-nexus.c:  Monitor debug print facility
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
#define IN_MONITOR_SPACE
#include "monitor.h"

int mon_vprint(vm_t *vm, char *fmt, va_list args);

static unsigned int power_of_ten[] = {
  1,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
  };

  int
monprint(vm_t *vm, char *fmt, ...)
{
  va_list args;
  int ret;

  va_start(args, fmt);
  ret = mon_vprint(vm, fmt, args);
  va_end(args);
  return(ret);
}

  int
mon_vprint(vm_t *vm, char *fmt, va_list args)
{
  unsigned offset, size;
  unsigned char *log_buff_p;
  int ret;

  if (vm->log_buffer_info.locked)
    return 0;

  vm->log_buffer_info.locked = 1;
  vm->log_buffer_info.event = 1;
  offset = vm->log_buffer_info.offset;

  /* Sanity check */
  if (offset >= LOG_BUFF_SIZE) {
    vm->guest.addr.log_buffer[0] = 0; /* Null terminate. */
    resetPrintBuf(vm);
    return(0);
    }

  size = LOG_BUFF_SIZE - offset;
  log_buff_p = &vm->guest.addr.log_buffer[offset];

  ret = mon_vsnprintf(log_buff_p, size, fmt, args);

  if (ret == -1) {
    /* Terminate current contents since new print request did not work. */
    *log_buff_p = 0;
    /* If we are in the monitor space, then we can request that the
     * current buffer contents be printed.
     */
    resetPrintBuf(vm);
    sysFlushPrintBuf(vm);

    /* Print request did not fit.  dump buffer contents and try again
     * using whole buffer.
     */
    size = LOG_BUFF_SIZE;
    log_buff_p = &vm->guest.addr.log_buffer[0];
    ret = mon_vsnprintf(log_buff_p, size, fmt, args);
    if (ret == -1) {
      /* We have serious problems.  This print request will not even
       * fit in the whole buffer.
       */
      vm->guest.addr.log_buffer[0] = 0; /* Null terminate. */
      resetPrintBuf(vm);
      /* xxx Put error in buffer here. */
      return(0);
      }
    }
  vm->log_buffer_info.offset += ret;
  vm->log_buffer_info.locked = 0;
#if 0 /* Fri Dec 27 21:43:05 EST 2002 */
  resetPrintBuf(vm);
  sysFlushPrintBuf(vm);
#endif
  return(ret);
}

  void
resetPrintBuf(vm_t *vm)
{
  vm->log_buffer_info.event = 0;
  vm->log_buffer_info.locked = 0;
  vm->log_buffer_info.offset = 0;
  vm->log_buffer_info.error = 0;
}


/* For now, this is a simple vsnprintf() type of function.  We need
 * to fill this out a little.
 */

  int
mon_vsnprintf(char *str, unsigned size, const char *fmt, va_list args)
{
  int count = 0;
  unsigned format_width;
  unsigned char c;

  while (*fmt) {
    switch (*fmt) {

      case '%':
        format_width = 0;
        fmt++;
        c = *fmt++;
        /* Get optional field width */
        if ( (c>='0') && (c<='9') ) {
          do {
            format_width = (format_width * 10) + (c - '0');
            c = *fmt++;
            } while ( (c>='0') && (c<='9') );
          }
        /* %x: hexadecimal */
        if ( c == 'x' ) {
          unsigned int val, leadin;
          int j;
          unsigned nibble;

          val = va_arg(args, unsigned int);
          leadin = 1;

          for (j=7; j>=0; j--) {
            nibble = (val >> (4 * j)) & 0x0f;
            if (leadin && j && !format_width && !nibble)
              continue;
            if (leadin && j && format_width && ((j+1)>format_width) &&
                !nibble)
              continue;
            leadin = 0;
            if ( (count+2) >= size ) goto error;
            if (nibble <= 9)
              *str++ = nibble + '0';
            else
              *str++ = (nibble-10) + 'A';
            count++;
            }
          break;
          }

        /* %c: character */
        if ( c == 'c' ) {
          unsigned char val;
          val = va_arg(args, unsigned);
          if ( (count+2) >= size ) goto error;
          *str++ = val;
          count++;
          break;
          }

        /* %s: string */
        if ( c == 's' ) {
          unsigned char *s;
          s = va_arg(args, unsigned char *);
          if ( (count+2) >= size ) goto error;
          count++;
          while (*s) {
            if ( (count+2) >= size ) goto error;
            *str++ = *s++; /* Copy char from string to output buffer. */
            count++;
            }
          break;
          }

        /* %u: unsigned int */
        if ( c == 'u' ) {
          unsigned int val, leadin;
          int j;
          unsigned digit;

          val = va_arg(args, unsigned int);
          leadin = 1;

          for (j=9; j>=0; j--) {
            if (leadin && j && !format_width && (val < power_of_ten[j]))
              continue;
            if (leadin && j && format_width && ((j+1)>format_width) &&
                (val < power_of_ten[j]))
              continue;
            leadin = 0;
            digit = (val / power_of_ten[j]);
            if ( (count+2) >= size ) goto error;
            *str++ = digit + '0';
            count++;
            val -= (digit * power_of_ten[j]);
            }
          break;
          }
        /* %b : binary (non-standard but useful) */
        if ( c == 'b' ) {
          unsigned int val, bit, leadin;
          int j;
          val = va_arg(args, unsigned int);
          leadin = 1;
          for (j=31; j>=0; j--) {
            bit = (val >> j) & 1;
            if (leadin && j && !format_width && !bit)
              continue;
            if (leadin && j && format_width && ((j+1)>format_width) && !bit)
              continue;
            leadin = 0;
            if ( (count+2) >= size ) goto error;
            *str++ = bit + '0';
            count++;
            }
          break;
          }

        /* Error, unrecognized format char */
        goto error;
        break;

      default:
        /* pass char through */
        if ( (count+2) >= size ) goto error;
        *str++ = *fmt++;
        count++;
        break;
      }
    }

  *str = 0; /* Complete string with null char */
  return(count);

error:
  return(-1);
}
