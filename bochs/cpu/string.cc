//  Copyright (C) 2000  MandrakeSoft S.A.
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






#include "bochs.h"


/* MOVSB ES:[EDI], DS:[ESI]   DS may be overridden
 *   mov string from DS:[ESI] into ES:[EDI]
 */

  void
BX_CPU_C::MOVSB_XbYb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u temp8;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, edi, &temp8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI, EDI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI, EDI */
      esi++;
      edi++;
      }

    ESI = esi;
    EDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16 bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

    read_virtual_byte(seg, si, &temp8);

    write_virtual_byte(BX_SEG_REG_ES, di, &temp8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement SI, DI */
      si--;
      di--;
      }
    else {
      /* increment SI, DI */
      si++;
      di++;
      }

    SI = si;
    DI = di;
    }
}

  void
BX_CPU_C::MOVSW_XvYv(BxInstruction_t *i)
{
  unsigned seg;


  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u  temp32;

    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    if (i->os_32) {
      read_virtual_dword(seg, esi, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, edi, &temp32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 4;
        edi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        edi += 4;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u  temp16;

      read_virtual_word(seg, esi, &temp16);

      write_virtual_word(BX_SEG_REG_ES, edi, &temp16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }

    ESI = esi;
    EDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u temp32;

      read_virtual_dword(seg, si, &temp32);

      write_virtual_dword(BX_SEG_REG_ES, di, &temp32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      } /* if (i->os_32) ... */
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
      Bit16u  temp16;

      read_virtual_word(seg, si, &temp16);

      write_virtual_word(BX_SEG_REG_ES, di, &temp16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement SI, DI */
        si -= 2;
        di -= 2;
        }
      else {
        /* increment SI, DI */
        si += 2;
        di += 2;
        }
      }

    SI = si;
    DI = di;
    }
}

  void
BX_CPU_C::CMPSB_XbYb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u op1_8, op2_8, diff_8;


  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI */
      esi++;
      edi++;
      }

    EDI = edi;
    ESI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

    read_virtual_byte(seg, si, &op1_8);

    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      si--;
      di--;
      }
    else {
      /* increment ESI */
      si++;
      di++;
      }

    DI = di;
    SI = si;
    }
}

  void
BX_CPU_C::CMPSW_XvYv(BxInstruction_t *i)
{
  unsigned seg;


  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u op1_32, op2_32, diff_32;
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;


    if (i->os_32) {
      read_virtual_dword(seg, esi, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 4;
        edi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      read_virtual_word(seg, esi, &op1_16);

      read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }


    EDI = edi;
    ESI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16 bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      read_virtual_dword(seg, si, &op1_32);

      read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      read_virtual_word(seg, si, &op1_16);

      read_virtual_word(BX_SEG_REG_ES, di, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        si -= 2;
        di -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        di += 2;
        }
      }


    DI = di;
    SI = si;
    }
}

  void
BX_CPU_C::SCASB_ALXb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;


#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);


    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      edi--;
      }
    else {
      /* increment ESI */
      edi++;
      }

    EDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

    op1_8 = AL;

    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      di--;
      }
    else {
      /* increment ESI */
      di++;
      }

    DI = di;
    }
}

  void
BX_CPU_C::SCASW_eAXXv(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        edi -= 4;
        }
      else {
        /* increment ESI */
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        edi -= 2;
        }
      else {
        /* increment ESI */
        edi += 2;
        }
      }

    EDI = edi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);

      diff_32 = op1_32 - op2_32;

      SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        di -= 4;
        }
      else {
        /* increment ESI */
        di += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      read_virtual_word(BX_SEG_REG_ES, di, &op2_16);

      diff_16 = op1_16 - op2_16;

      SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        di -= 2;
        }
      else {
        /* increment ESI */
        di += 2;
        }
      }

    DI = di;
    }
}

  void
BX_CPU_C::STOSB_YbAL(BxInstruction_t *i)
{
  Bit8u al;

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    al = AL;
    write_virtual_byte(BX_SEG_REG_ES, edi, &al);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement EDI */
      edi--;
      }
    else {
      /* increment EDI */
      edi++;
      }

    EDI = edi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address size */
    Bit16u di;

    di = DI;

    al = AL;
    write_virtual_byte(BX_SEG_REG_ES, di, &al);

    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement EDI */
      di--;
      }
    else {
      /* increment EDI */
      di++;
      }

    DI = di;
    }
}

  void
BX_CPU_C::STOSW_YveAX(BxInstruction_t *i)
{
#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u edi;

    edi = EDI;

    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, edi, &eax);

        if (BX_CPU_THIS_PTR eflags.df) {
          /* decrement EDI */
          edi -= 4;
          }
        else {
          /* increment EDI */
          edi += 4;
          }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        write_virtual_word(BX_SEG_REG_ES, edi, &ax);

        if (BX_CPU_THIS_PTR eflags.df) {
          /* decrement EDI */
          edi -= 2;
          }
        else {
          /* increment EDI */
          edi += 2;
          }
      }

    EDI = edi;
    }

  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address size */
    Bit16u di;

    di = DI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
        Bit32u eax;

        eax = EAX;
        write_virtual_dword(BX_SEG_REG_ES, di, &eax);

        if (BX_CPU_THIS_PTR eflags.df) {
          /* decrement EDI */
          di -= 4;
          }
        else {
          /* increment EDI */
          di += 4;
          }
      } /* if (i->os_32) ... */
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        write_virtual_word(BX_SEG_REG_ES, di, &ax);

        if (BX_CPU_THIS_PTR eflags.df) {
          /* decrement EDI */
          di -= 2;
          }
        else {
          /* increment EDI */
          di += 2;
          }
      }

    DI = di;
    }
}


  void
BX_CPU_C::LODSB_ALXb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u al;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

    read_virtual_byte(seg, esi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      esi--;
      }
    else {
      /* increment ESI */
      esi++;
      }

    ESI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

    read_virtual_byte(seg, si, &al);

    AL = al;
    if (BX_CPU_THIS_PTR eflags.df) {
      /* decrement ESI */
      si--;
      }
    else {
      /* increment ESI */
      si++;
      }

    SI = si;
    }
}

  void
BX_CPU_C::LODSW_eAXXv(BxInstruction_t *i)
{
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    Bit32u esi;

    esi = ESI;

    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, esi, &eax);

      EAX = eax;
      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        }
      } /* if (i->os_32) ... */
    else { /* 16 bit opsize mode */
      Bit16u ax;
      read_virtual_word(seg, esi, &ax);

      AX = ax;
      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        esi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        }
      }

    ESI = esi;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      Bit32u eax;

      read_virtual_dword(seg, si, &eax);

      EAX = eax;
      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        si -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        }
      }
    else
#endif /* BX_CPU_LEVEL >= 3 */
      { /* 16 bit opsize mode */
      Bit16u ax;

      read_virtual_word(seg, si, &ax);

      AX = ax;
      if (BX_CPU_THIS_PTR eflags.df) {
        /* decrement ESI */
        si -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        }
      }

    SI = si;
    }
}
