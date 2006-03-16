/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer8.cc,v 1.21 2006-03-16 20:24:09 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_X86_64==0
// Make life a little easier for the 64/32-bit merge.
#define RCX ECX
#define RIP EIP
#endif


void BX_CPU_C::JCXZ_Jb(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if ( RCX == 0 ) {
      RIP += (Bit32s) i->Id();
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif
  }
  else
#endif
  {
    Bit32u temp_ECX;

    if (i->as32L())
      temp_ECX = ECX;
    else
      temp_ECX = CX;

    if ( temp_ECX == 0 ) {
      Bit32u new_EIP = EIP + (Bit32s) i->Id();
      if (i->os32L()==0) new_EIP &= 0x0000ffff;
      branch_near32(new_EIP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif
  }
}

void BX_CPU_C::LOOPNE_Jb(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if ( ((--RCX)!=0) && (get_ZF()==0) ) {
      RIP += (Bit32s) i->Id();
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif
  }
  else
#endif
  {
    Bit32u count;

#if BX_CPU_LEVEL >= 3
    if (i->as32L())
      count = ECX;
    else
#endif /* BX_CPU_LEVEL >= 3 */
      count = CX;

    count--;
    if ( (count!=0) && (get_ZF()==0) ) {
      Bit32u new_EIP = EIP + (Bit32s) i->Id();
      if (i->os32L()==0) new_EIP &= 0x0000ffff;
      branch_near32(new_EIP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    if (i->as32L())
      RCX = ECX - 1;  // zero extend
    else
      CX--;
  }
}

void BX_CPU_C::LOOPE_Jb(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if ( ((--RCX)!=0) && (get_ZF()) ) {
      RIP += (Bit32s) i->Id();
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif
  }
  else
#endif
  {
    Bit32u count;

#if BX_CPU_LEVEL >= 3
    if (i->as32L())
      count = ECX;
    else
#endif /* BX_CPU_LEVEL >= 3 */
      count = CX;

    count--;
    if ( (count!=0) && get_ZF()) {
      Bit32u new_EIP = EIP + (Bit32s) i->Id();
      if (i->os32L()==0) new_EIP &= 0x0000ffff;
      branch_near32(new_EIP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    if (i->as32L())
      RCX = ECX - 1;   // zero extend
    else
      CX--;
  }
}

void BX_CPU_C::LOOP_Jb(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if ( ((--RCX)!=0) ) {
      RIP += (Bit32s) i->Id();
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif
  }
  else
#endif
  {
    Bit32u count;

#if BX_CPU_LEVEL >= 3
    if (i->as32L())
      count = ECX;
    else
#endif /* BX_CPU_LEVEL >= 3 */
      count = CX;

    count--;
    if (count != 0) {
      Bit32u new_EIP = EIP + (Bit32s) i->Id();
      if (i->os32L()==0) new_EIP &= 0x0000ffff;
      branch_near32(new_EIP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    if (i->as32L())
      RCX = ECX - 1;         // zero extend
    else
      CX--;
  }
}
