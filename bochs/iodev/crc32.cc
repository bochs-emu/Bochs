/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 */

#include "crc32.h"

CRC_Generator::CRC_Generator() {
	init();
}

void CRC_Generator::init(void) {
  Bit32u POLYNOMIAL = 0x04c11db7;
  int i;

  for(i = 0; i<0xFF; i++) {
    int j;
    crc32_table[i]=reflect(i,8) << 24;
    for(j=0; j<8; j++)
      crc32_table[i] = (crc32_table[i]<<1)^(crc32_table[i] & (1<<31) ? POLYNOMIAL : 0);
    crc32_table[i] = reflect(crc32_table[i], 32);
  }
}

Bit32u CRC_Generator::reflect(Bit32u ref, Bit8u ch) {
  Bit32u value(0);
  int i;

  for(i=1; i<(ch+1); i++) {
    if(ref & 1)
      value |= 1 << (ch-i);
    ref >>= 1;
  }
  return value;
}

Bit32u CRC_Generator::get_CRC(Bit8u * buf, Bit32u buflen) {
  Bit32u ulCRC(0xFFFFFFFF);
  Bit32u len(buflen);
  Bit8u * buffer=(Bit8u *) buf;

  while(len--)
    ulCRC=(ulCRC>>8)^crc32_table[(ulCRC & 0xFF)^*buffer++];
  return ulCRC ^ 0xFFFFFFFF;
}

