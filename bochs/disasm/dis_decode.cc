#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "disasm.h"
#include "dis_tables.h"

/* ******************** */
// INSTRUCTION PREFIXES //
/* ******************** */

static const unsigned char instruction_has_modrm[512] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f          */
  /*       -------------------------------          */
  /* 00 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 10 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 20 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 30 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, 
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 60 */ 0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0, 
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* 80 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* A0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* B0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* C0 */ 1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0, 
  /* D0 */ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, 
  /* E0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  /* F0 */ 0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
  /*       -------------------------------           */
           1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1, /* 0F 00 */
           1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0, /* 0F 10 */
           1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1, /* 0F 20 */
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0F 30 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 40 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 50 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 60 */
           1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1, /* 0F 70 */
           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0F 80 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F 90 */
           0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,1, /* 0F A0 */
           1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1, /* 0F B0 */
           1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* 0F C0 */
           0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F D0 */
           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0F E0 */
           0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0  /* 0F F0 */
  /*       -------------------------------           */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f           */
};

/*
 * Group 1:
 * 
 *      F0h - LOCK
 *      F2h - REPNE/REPZ (used only with string instructions)
 *      F3h - REP or REPE/REPZ (used only with string instructions)
 * 
 * Group 2 :
 *
 *      - segment override prefixes
 *              2Eh - CS segment override
 *              36h - SS segment override
 *              3Eh - DS segment override
 *              26h - ES segment override
 *              64h - FS segment override
 *              65h - GS segment override
 *
 *      - branch hints
 *              2Eh - branch not taken (branch hint for Jcc instructions only)
 *              3Eh - branch taken (branch hint for Jcc instructions only)
 *
 * Group 3:
 *
 *      66h - operand size override prefix
 *      67h - address size override prefix
 *
 *  For each instruction, one prefix may be used from each of these groups
 *  and  be  placed  in any order. Using redundant prefixes (more than one
 *  prefix from a group) is reserved and will cause undefined behaviour.
 */

unsigned disassembler::disasm(bx_bool is_32, 
        Bit32u base, Bit32u ip, Bit8u *instr, char *disbuf)
{
  i32bit_opsize = is_32;
  i32bit_addrsize = is_32;
  db_eip = ip;
  db_base = base; // cs linear base (base for PM & cs<<4 for RM & VM)
  instruction_begin = instruction = instr;
  displacement.displ32 = 0;

  resolve_modrm = NULL;
  seg_override = NULL;

  n_prefixes = 0;
  disbufptr = disbuf; // start sprintf()'ing into beginning of buffer

#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F2   2
#define SSE_PREFIX_F3   4      /* only one SSE prefix could be used */
  static int sse_prefix_index[8] = { 0, 1, 2, -1, 3, -1, -1, -1 };
  unsigned sse_prefix = SSE_PREFIX_NONE;

  int b1;
  const BxDisasmOpcodeInfo_t *entry;

  for(;;)
  {
        b1 = fetch_byte();
        entry = &BxDisasmOpcodes[b1];

        if (entry->Attr == _PREFIX)
        {
                switch(b1) {
                case 0xf3:
                        sse_prefix |= SSE_PREFIX_F3;
                        break;
                case 0xf2:
                        sse_prefix |= SSE_PREFIX_F2;
                        break;
                case 0x2e:
                case 0x36:
                case 0x3e:
                case 0x26:
                case 0x64:
                case 0x65:
                        seg_override = entry->Opcode;
                        break;
                case 0x66:
                        i32bit_opsize = !i32bit_opsize;
                        sse_prefix |= SSE_PREFIX_66;
                        break;
                case 0x67:
                        i32bit_addrsize = !i32bit_addrsize;
                        break;
                case 0xf0:      // lock
                        break;
                default:
                        printf("Internal disassembler error !");
                        return 0;
                }

                n_prefixes++;
        }
        else    break;
  }

  if (b1 == 0x0f)
  {
        b1 = 0x100 | fetch_byte();
        entry = &BxDisasmOpcodes[b1];
  }

  if (instruction_has_modrm[b1])
        decode_modrm();

  int attr = entry->Attr;
  while(attr) 
  {
    switch(attr) {
       case _GROUPN:
         entry = &(entry->AnotherArray[nnn]);
         break;

       case _GRPSSE:
         {
            if(sse_prefix) n_prefixes--;
            /* For SSE opcodes, look into another 4 entries table 
                  with the opcode prefixes (NONE, 0x66, 0xF2, 0xF3) */
            int op = sse_prefix_index[sse_prefix];
            if (op < 0) return 0;
            entry = &(entry->AnotherArray[op]);
         }
         break;

       case _SPLIT11B:
         entry = &(entry->AnotherArray[mod==3]);
         break;

       case _GRPFP:
         if(mod != 3)
         {
             entry = &(entry->AnotherArray[nnn]);
         }
         else
         {
             int index = (b1-0xD8)*64 + (0x3f & modrm);
             entry = &(BxDisasmOpcodeInfoFP[index]);
         }
         break;

       case _GRP3DNOW:
         {
             int suffix = peek_byte();
             entry = &(BxDisasm3DNowGroup[suffix]);
         }
         break;

       default:
         printf("Internal disassembler error !");
         return 0;
    }

    /* get additional attributes from group table */
    attr = entry->Attr;
  }

  // print prefixes
  for(unsigned i=0;i<n_prefixes;i++)
        if (*(instr+i) == 0xF3 || *(instr+i) != 0xF2 || *(instr+i) != 0xF0) 
             dis_sprintf("%s ", BxDisasmOpcodes[*(instr+i)].Opcode);

  // print opcode
  dis_sprintf("%s ", entry->Opcode);

  (this->*entry->Operand1)(entry->Op1Attr);
  if (entry->Operand2 != &disassembler::XX)
        dis_sprintf(", ");
  (this->*entry->Operand2)(entry->Op2Attr);
  if (entry->Operand3 != &disassembler::XX)
        dis_sprintf(", ");
  (this->*entry->Operand3)(entry->Op3Attr);

  return(instruction - instruction_begin);
}

void disassembler::dis_sprintf(char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsprintf(disbufptr, fmt, ap);
  va_end(ap);

  disbufptr += strlen(disbufptr);
}
