/////////////////////////////////////////////////////////////////////////
// $Id: crc32.h,v 1.5 2006-03-07 18:16:40 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
/* CRC-32 calculator
 * Adapted from http://www.createwindow.org/programming/crc32/
 */

#ifndef _CRC_32_H_
#define _CRC_32_H_

#include "config.h"

class CRC_Generator {
private:
  Bit32u crc32_table[256];
  Bit32u reflect(Bit32u ref, Bit8u ch);
public:
  void init(void);
  CRC_Generator();
  Bit32u get_CRC(Bit8u * buf, Bit32u buflen);
};

#endif //_CRC_32_H_

