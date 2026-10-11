/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// CRC-32C (Castagnoli) helper for disk image code.
//
// Polynomial: 0x1EDC6F41 (reversed: 0x82F63B78)
// This implementation uses the reflected (LSB-first) algorithm with
// initial value 0xffffffff and final XOR of 0xffffffff.
//
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#include <stddef.h>

static Bit32u crc32c_table[256];
static bool crc32c_initialized = false;

static void init_crc32c_table(void)
{
  for (unsigned i = 0; i < 256; i++) {
    Bit32u crc = (Bit32u)i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0x82F63B78U;
      } else {
        crc >>= 1;
      }
    }
    crc32c_table[i] = crc;
  }
  crc32c_initialized = true;
}

Bit32u crc32c(const Bit8u *buf, size_t len)
{
  if (!crc32c_initialized) init_crc32c_table();

  Bit32u crc = 0xFFFFFFFFU;
  for (size_t i = 0; i < len; i++) {
    crc = crc32c_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
  }
  return ~crc;
}
