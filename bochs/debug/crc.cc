/////////////////////////////////////////////////////////////////////////
// $Id: crc.cc,v 1.2 2001-10-03 13:10:37 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  I grabbed these CRC routines from the following source:
//    http://www.landfield.com/faqs/compression-faq/part1/section-25.html
//
//  These routines are very useful, so I'm including them in bochs.
//  They are not covered by the license, as they are not my doing.
//  My gratitude to the author for offering them on the 'net.
//
//  I only changed the u_long to unsigned long, and u_char to unsigned char,
//  and gave the functions prototypes.
//
//  -Kevin
//
//  **************************************************************************
//  The following C code (by Rob Warnock <rpw3@sgi.com>) does CRC-32 in
//  BigEndian/BigEndian byte/bit order.  That is, the data is sent most
//  significant byte first, and each of the bits within a byte is sent most
//  significant bit first, as in FDDI. You will need to twiddle with it to do
//  Ethernet CRC, i.e., BigEndian/LittleEndian byte/bit order. [Left as an
//  exercise for the reader.]
//
//  The CRCs this code generates agree with the vendor-supplied Verilog models
//  of several of the popular FDDI "MAC" chips.
//  **************************************************************************


#include "bochs.h"

unsigned long crc32_table[256];
/* Initialized first time "crc32()" is called. If you prefer, you can
 * statically initialize it at compile time. [Another exercise.]
 */


unsigned long crc32(unsigned char *buf, int len);
static void init_crc32(void);

unsigned long crc32(unsigned char *buf, int len)
{
        unsigned char *p;
        unsigned long  crc;

        if (!crc32_table[1])    /* if not already done, */
                init_crc32();   /* build table */
        crc = 0xffffffff;       /* preload shift register, per CRC-32 spec */
        for (p = buf; len > 0; ++p, --len)
                crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];
        return ~crc;            /* transmit complement, per CRC-32 spec */
}

/*
 * Build auxiliary table for parallel byte-at-a-time CRC-32.
 */
#define CRC32_POLY 0x04c11db7     /* AUTODIN II, Ethernet, & FDDI */

  void
init_crc32(void)
{
        int i, j;
        unsigned long c;

        for (i = 0; i < 256; ++i) {
                for (c = i << 24, j = 8; j > 0; --j)
                        c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
                crc32_table[i] = c;
        }
}
