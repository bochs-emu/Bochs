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

/* includes a subset of config.h that can be compiled by bcc, and applies
   to this file */
#include "biosconfig.h"

// ROM BIOS for use with Bochs x86 emulation environment

#define PANIC_PORT 0x400
#define HALT(line) mov dx,PANIC_PORT; mov ax,line; out dx,ax; hlt


// ROM BIOS compatability entry points:
// ===================================
// $e05b ; POST Entry Point
// $e2c3 ; NMI Handler Entry Point
// $e3fe ; INT 13h Fixed Disk Services Entry Point
// $e401 ; Fixed Disk Parameter Table
// $e6f2 ; INT 19h Boot Load Service Entry Point
// $e6f5 ; Configuration Data Table
// $e729 ; Baud Rate Generator Table
// $e739 ; INT 14h Serial Communications Service Entry Point
// $e82e ; INT 16h Keyboard Service Entry Point
// $e987 ; INT 09h Keyboard Service Entry Point
// $ec59 ; INT 13h Diskette Service Entry Point
// $ef57 ; INT 0Eh Diskette Hardware ISR Entry Point
// $efc7 ; Diskette Controller Parameter Table
// $efd2 ; INT 17h Printer Service Entry Point
// $f045 ; INT 10 Functions 0-Fh Entry Point
// $f065 ; INT 10h Video Support Service Entry Point
// $f0a4 ; MDA/CGA Video Parameter Table (INT 1Dh)
// $f841 ; INT 12h Memory Size Service Entry Point
// $f84d ; INT 11h Equipment List Service Entry Point
// $f859 ; INT 15h System Services Entry Point
// $fa6e ; Character Font for 320x200 & 640x200 Graphics (lower 128 characters)
// $fe6e ; INT 1Ah Time-of-day Service Entry Point
// $fea5 ; INT 08h System Timer ISR Entry Point
// $fef3 ; Initial Interrupt Vector Offsets Loaded by POST
// $ff53 ; IRET Instruction for Dummy Interrupt Handler
// $ff54 ; INT 05h Print Screen Service Entry Point
// $fff0 ; Power-up Entry Point
// $fff5 ; ASCII Date ROM was built - 8 characters in MM/DD/YY
// $fffe ; System Model ID


// old NOTES:
// int74 needs to be reworked.  Uses direct [bp] offsets.
// take out int13 printf()s, or conditionally compile them
// int13:
//   f04 (verify sectors) isn't complete
//   f02/03/04 should set current cyl,etc in BDA
//
// int1a:
//   f03/f05 are not complete - just CLC for now
//
// int16_function: default case ?

// NOTES:
// 990104:
//   - NMI access (bit7 of addr written to 70h)
//   - timer ISR should deal with floppy counter and turn floppy motor off

#define BX_CPU           3
#define BX_USE_PS2_MOUSE 1
#define BX_CALL_INT15_4F 1
#define BX_USE_EBDA      1
#define BX_SUPPORT_FLOPPY 1

   /* model byte 0xFC = AT */
#define SYS_MODEL_ID     0xFC
#define SYS_SUBMODEL_ID  0x00
#define BIOS_REVISION    1
#define BIOS_CONFIG_TABLE 0xe6f5
  // 1K of base memory used for Extended Bios Data Area (EBDA)
  // EBDA is used for PS/2 mouse support, and IDE BIOS, etc.
#define BASE_MEM_IN_K (640 - 1)
#define EBDA_SEG 0x9FC0

#define JMPL(label) db 0xe9!!!dw (label-(*+2)) ; jmp near label
#define JMP_EP(loc) db 0xff!!!db 0x2e!!!dw loc ; jmp_ep [loc]
#define CALL_AP(seg, off) db 0x9a!!!dw off!!!dw seg ; call_ap seg:off
#define CALL_EP(loc) db 0xff!!!db 0x1e!!!dw loc ; call_ep [ds:loc]
#define SET_INT_VECTOR(vec, seg, off) \
  mov ax, off !!!\
  mov vec*4, ax  !!!\
  mov ax, seg  !!!\
  mov vec*4+2, ax
#define JMP_AP(seg, off) db 0xea!!!dw off!!!dw seg ; jmp_ap seg:off
#define ASM(pound, s) !!!##pound##asm!!!s!!!##pound##endasm

// #20  is dec 20
// #$20 is hex 20 = 32
// LDA	#$20
// JSR	$E820
// LDD	.i,S
// JSR	$C682
// mov al, #$20

// all hex literals should be prefixed with '0x'
//   grep "#[0-9a-fA-F][0-9a-fA-F]" rombios.c
// no mov SEG-REG, #value, must mov register into seg-reg
//   grep -i "mov[ ]*.s" rombios.c



#asm
.text
.rom
.org 0x0000

isru: ;; shift right unsigned? (compiler needs this)
      ;; operation: ax = ax >> bl
  push  cx
  mov   cl, bl
  shr   ax, cl
  pop   cx
  ret


#endasm


typedef unsigned char  Bit8u;
typedef unsigned short Bit16u;
typedef unsigned short Boolean;

// for access to RAM area which is used by interrupt vectors
// and BIOS Data Area

typedef struct {
  unsigned char filler1[0x400];
  unsigned char filler2[0x6c];
  Bit16u ticks_low;
  Bit16u ticks_high;
  Bit8u  midnight_flag;
  } bios_data_t;

#define BiosData ((bios_data_t  *) 0)

typedef struct {
  union {
    struct {
      Bit16u di, si, bp, sp;
      Bit16u bx, dx, cx, ax;
      } r16;
    struct {
      Bit16u filler[4];
      Bit8u  bl, bh, dl, dh, cl, ch, al, ah;
      } r8;
    } u;
  } pusha_regs_t;

typedef struct {
  union {
    struct {
      Bit16u flags;
      } r16;
    struct {
      Bit8u  flagsl;
      Bit8u  flagsh;
      } r8;
    } u;
  } flags_t;

#define SetCF(x)   x.u.r8.flagsl |= 0x01
#define SetZF(x)   x.u.r8.flagsl |= 0x40
#define ClearCF(x) x.u.r8.flagsl &= 0xfe
#define ClearZF(x) x.u.r8.flagsl &= 0xbf
#define GetCF(x)   (x.u.r8.flagsl & 0x01)

typedef struct {
  Bit16u ip;
  Bit16u cs;
  flags_t flags;
  } iret_addr_t;



static Bit8u          inb();
static Bit8u          inb_cmos();
static void           outb();
static void           outb_cmos();
static Bit16u         inw();
static void           outw();
static void           init_rtc();
static Boolean        rtc_updating();

static Bit8u          read_byte();
static Bit16u         read_word();
static void           write_byte();
static void           write_word();
static void           bios_printf();

static Bit16u         UDIV();

static Bit8u          inhibit_mouse_int_and_events();
static void           enable_mouse_int_and_events();
static Bit8u          send_to_mouse_ctrl();
static Bit8u          get_mouse_data();
static void           set_kbd_command_byte();

static void           int09_function();
static void           int13_function();
static void           int13_diskette_function();
static void           int15_function();
static void           int16_function();
static void           int1a_function();
static void           int70_function();
static void           int74_function();
//static Bit16u         get_DS();
//static void           set_DS();
static Bit16u         get_SS();
static void           enqueue_key();
static unsigned int   dequeue_key();
static void           set_disk_ret_status();
static void           get_hd_geometry();
static void           set_diskette_ret_status();
static void           set_diskette_current_cyl();
static void           determine_floppy_media();
static Boolean        floppy_drive_exists();
static Boolean        floppy_drive_recal();
static Boolean        floppy_media_known();
static Boolean        floppy_media_sense();
static void           cli();
static Boolean        set_enable_a20();
static void           debugger_on();
static void           debugger_off();
static void           keyboard_panic();

#define DEBUG_ROMBIOS 0

#if DEBUG_ROMBIOS
#  define printf(format, p...) bios_printf(0, format, ##p)
#  define panic(format, p...)  bios_printf(1, format, ##p)
#else
#  define printf(format, p...)
#  define panic(format, p...)  bios_printf(1, format, ##p)
#endif


#define SET_AL(val8) AX = ((AX & 0xff00) | (val8))
#define SET_BL(val8) BX = ((BX & 0xff00) | (val8))
#define SET_CL(val8) CX = ((CX & 0xff00) | (val8))
#define SET_DL(val8) DX = ((DX & 0xff00) | (val8))
#define SET_AH(val8) AX = ((AX & 0x00ff) | ((val8) << 8))
#define SET_BH(val8) BX = ((BX & 0x00ff) | ((val8) << 8))
#define SET_CH(val8) CX = ((CX & 0x00ff) | ((val8) << 8))
#define SET_DH(val8) DX = ((DX & 0x00ff) | ((val8) << 8))

#define GET_AL() ( AX & 0x00ff )
#define GET_BL() ( BX & 0x00ff )
#define GET_CL() ( CX & 0x00ff )
#define GET_DL() ( DX & 0x00ff )
#define GET_AH() ( AX >> 8 )
#define GET_BH() ( BX >> 8 )
#define GET_CH() ( CX >> 8 )
#define GET_DH() ( DX >> 8 )


#define SET_CF()     FLAGS |= 0x0001
#define CLEAR_CF()   FLAGS &= 0xfffe
#define GET_CF()     (FLAGS & 0x0001)

#define SET_ZF()     FLAGS |= 0x0040
#define CLEAR_ZF()   FLAGS &= 0xffbf
#define GET_ZF()     (FLAGS & 0x0040)

#define UNSUPPORTED_FUNCTION 0x86

#define none 0
#define MAX_SCAN_CODE 0x53

static struct {
  Bit16u normal;
  Bit16u shift;
  Bit16u control;
  Bit16u alt;
  } scan_to_scanascii[MAX_SCAN_CODE + 1] = {
      {   none,   none,   none,   none },
      { 0x011b, 0x011b, 0x011b, 0x0100 }, /* escape */
      { 0x0231, 0x0221,   none, 0x7800 }, /* 1! */
      { 0x0332, 0x0340, 0x0300, 0x7900 }, /* 2@ */
      { 0x0433, 0x0423,   none, 0x7a00 }, /* 3# */
      { 0x0534, 0x0524,   none, 0x7b00 }, /* 4$ */
      { 0x0635, 0x0625,   none, 0x7c00 }, /* 5% */
      { 0x0736, 0x075e, 0x071e, 0x7d00 }, /* 6^ */
      { 0x0837, 0x0826,   none, 0x7e00 }, /* 7& */
      { 0x0938, 0x092a,   none, 0x7f00 }, /* 8* */
      { 0x0a39, 0x0a28,   none, 0x8000 }, /* 9( */
      { 0x0b30, 0x0b29,   none, 0x8100 }, /* 0) */
      { 0x0c2d, 0x0c5f, 0x0c1f, 0x8200 }, /* -_ */
      { 0x0d3d, 0x0d2b,   none, 0x8300 }, /* =+ */
      { 0x0e08, 0x0e08, 0x0e7f,   none }, /* backspace */
      { 0x0f09, 0x0f00,   none,   none }, /* tab */
      { 0x1071, 0x1051, 0x1011, 0x1000 }, /* Q */
      { 0x1177, 0x1157, 0x1117, 0x1100 }, /* W */
      { 0x1265, 0x1245, 0x1205, 0x1200 }, /* E */
      { 0x1372, 0x1352, 0x1312, 0x1300 }, /* R */
      { 0x1474, 0x1454, 0x1414, 0x1400 }, /* T */
      { 0x1579, 0x1559, 0x1519, 0x1500 }, /* Y */
      { 0x1675, 0x1655, 0x1615, 0x1600 }, /* U */
      { 0x1769, 0x1749, 0x1709, 0x1700 }, /* I */
      { 0x186f, 0x184f, 0x180f, 0x1800 }, /* O */
      { 0x1970, 0x1950, 0x1910, 0x1900 }, /* P */
      { 0x1a5b, 0x1a7b, 0x1a1b,   none }, /* [{ */
      { 0x1b5d, 0x1b7d, 0x1b1d,   none }, /* ]} */
      { 0x1c0d, 0x1c0d, 0x1c0a,   none }, /* Enter */
      {   none,   none,   none,   none }, /* L Ctrl */
      { 0x1e61, 0x1e41, 0x1e01, 0x1e00 }, /* A */
      { 0x1f73, 0x1f53, 0x1f13, 0x1f00 }, /* S */
      { 0x2064, 0x2044, 0x2004, 0x2000 }, /* D */
      { 0x2166, 0x2146, 0x2106, 0x2100 }, /* F */
      { 0x2267, 0x2247, 0x2207, 0x2200 }, /* G */
      { 0x2368, 0x2348, 0x2308, 0x2300 }, /* H */
      { 0x246a, 0x244a, 0x240a, 0x2400 }, /* J */
      { 0x256b, 0x254b, 0x250b, 0x2500 }, /* K */
      { 0x266c, 0x264c, 0x260c, 0x2600 }, /* L */
      { 0x273b, 0x273a,   none,   none }, /* ;: */
      { 0x2827, 0x2822,   none,   none }, /* '" */
      { 0x2960, 0x297e,   none,   none }, /* `~ */
      {   none,   none,   none,   none }, /* L shift */
      { 0x2b5c, 0x2b7c, 0x2b1c,   none }, /* |\ */
      { 0x2c7a, 0x2c5a, 0x2c1a, 0x2c00 }, /* Z */
      { 0x2d78, 0x2d58, 0x2d18, 0x2d00 }, /* X */
      { 0x2e63, 0x2e43, 0x2e03, 0x2e00 }, /* C */
      { 0x2f76, 0x2f56, 0x2f16, 0x2f00 }, /* V */
      { 0x3062, 0x3042, 0x3002, 0x3000 }, /* B */
      { 0x316e, 0x314e, 0x310e, 0x3100 }, /* N */
      { 0x326d, 0x324d, 0x320d, 0x3200 }, /* M */
      { 0x332c, 0x333c,   none,   none }, /* ,< */
      { 0x342e, 0x343e,   none,   none }, /* .> */
      { 0x352f, 0x353f,   none,   none }, /* /? */
      {   none,   none,   none,   none }, /* R Shift */
      { 0x372a, 0x372a,   none,   none }, /* * */
      {   none,   none,   none,   none }, /* L Alt */
      { 0x3920, 0x3920, 0x3920, 0x3920 }, /* space */
      {   none,   none,   none,   none }, /* caps lock */
      { 0x3b00, 0x5400, 0x5e00, 0x6800 }, /* F1 */
      { 0x3c00, 0x5500, 0x5f00, 0x6900 }, /* F2 */
      { 0x3d00, 0x5600, 0x6000, 0x6a00 }, /* F3 */
      { 0x3e00, 0x5700, 0x6100, 0x6b00 }, /* F4 */
      { 0x3f00, 0x5800, 0x6200, 0x6c00 }, /* F5 */
      { 0x4000, 0x5900, 0x6300, 0x6d00 }, /* F6 */
      { 0x4100, 0x5a00, 0x6400, 0x6e00 }, /* F7 */
      { 0x4200, 0x5b00, 0x6500, 0x6f00 }, /* F8 */
      { 0x4300, 0x5c00, 0x6600, 0x7000 }, /* F9 */
      { 0x4400, 0x5d00, 0x6700, 0x7100 }, /* F10 */
      {   none,   none,   none,   none }, /* Num Lock */
      {   none,   none,   none,   none }, /* Scroll Lock */
      { 0x4700, 0x4737, 0x7700,   none }, /* 7 Home */
      { 0x4800, 0x4838,   none,   none }, /* 8 UP */
      { 0x4900, 0x4939, 0x8400,   none }, /* 9 PgUp */
      { 0x4a2d, 0x4a2d,   none,   none }, /* - */
      { 0x4b00, 0x4b34, 0x7300,   none }, /* 4 Left */
      { 0x4c00, 0x4c35,   none,   none }, /* 5 */
      { 0x4d00, 0x4d36, 0x7400,   none }, /* 6 Right */
      { 0x4e2b, 0x4e2b,   none,   none }, /* + */
      { 0x4f00, 0x4f31, 0x7500,   none }, /* 1 End */
      { 0x5000, 0x5032,   none,   none }, /* 2 Down */
      { 0x5100, 0x5133, 0x7600,   none }, /* 3 PgDn */
      { 0x5200, 0x5230,   none,   none }, /* 0 Ins */
      { 0x5300, 0x532e,   none,   none }  /* Del */
      };

  Bit8u
inb(port)
  Bit16u port;
{
#asm
  push bp
  mov  bp, sp

    push dx
    mov  dx, 4[bp]
    in   al, dx
    pop  dx

  pop  bp
#endasm
}

#if 0
  Bit16u
inw(port)
  Bit16u port;
{
#asm
  push bp
  mov  bp, sp

    push dx
    mov  dx, 4[bp]
    in   ax, dx
    pop  dx

  pop  bp
#endasm
}
#endif


  void
outb(port, val)
  Bit16u port;
  Bit8u  val;
{
#asm
  push bp
  mov  bp, sp

    push ax
    push dx
    mov  dx, 4[bp]
    mov  al, 6[bp]
    out  dx, al
    pop  dx
    pop  ax

  pop  bp
#endasm
}

#if 0
  void
outw(port, val)
  Bit16u port;
  Bit16u  val;
{
#asm
  push bp
  mov  bp, sp

    push ax
    push dx
    mov  dx, 4[bp]
    mov  ax, 6[bp]
    out  dx, ax
    pop  dx
    pop  ax

  pop  bp
#endasm
}
#endif

  void
outb_cmos(cmos_reg, val)
  Bit8u cmos_reg;
  Bit8u val;
{
#asm
  push bp
  mov  bp, sp

    mov  al, 4[bp] ;; cmos_reg
    out  0x70, al
    mov  al, 6[bp] ;; val
    out  0x71, al

  pop  bp
#endasm
}

  Bit8u
inb_cmos(cmos_reg)
  Bit8u cmos_reg;
{
#asm
  push bp
  mov  bp, sp

    mov  al, 4[bp] ;; cmos_reg
    out 0x70, al
    in  al, 0x71

  pop  bp
#endasm
}

  void
init_rtc()
{
  outb_cmos(0x0a, 0x26);
  outb_cmos(0x0b, 0x02);
  inb_cmos(0x0c);
  inb_cmos(0x0d);
}

  Boolean
rtc_updating()
{
  // This function checks to see if the update-in-progress bit
  // is set in CMOS Status Register A.  If not, it returns 0.
  // If it is set, it tries to wait until there is a transition
  // to 0, and will return 0 if such a transition occurs.  A 1
  // is returned only after timing out.  The maximum period
  // that this bit should be set is constrained to 244useconds.
  // The count I use below guarantees coverage or more than
  // this time, with any reasonable IPS setting.

  Bit16u count;

  count = 25000;
  while (--count != 0) {
    if ( (inb_cmos(0x0a) & 0x80) == 0 )
      return(0);
    }
  return(1); // update-in-progress never transitioned to 0
}


  Bit8u
read_byte(seg, offset)
  Bit16u seg;
  Bit16u offset;
{
#asm
  push bp
  mov  bp, sp

    push bx
    push ds
    mov  ax, 4[bp] ; segment
    mov  ds, ax
    mov  bx, 6[bp] ; offset
    mov  al, [bx]
    ;; al = return value (byte)
    pop  ds
    pop  bx

  pop  bp
#endasm
}

  Bit16u
read_word(seg, offset)
  Bit16u seg;
  Bit16u offset;
{
#asm
  push bp
  mov  bp, sp

    push bx
    push ds
    mov  ax, 4[bp] ; segment
    mov  ds, ax
    mov  bx, 6[bp] ; offset
    mov  ax, [bx]
    ;; ax = return value (word)
    pop  ds
    pop  bx

  pop  bp
#endasm
}

  void
write_byte(seg, offset, data)
  Bit16u seg;
  Bit16u offset;
{
#asm
  push bp
  mov  bp, sp

    push ax
    push bx
    push ds
    mov  ax, 4[bp] ; segment
    mov  ds, ax
    mov  bx, 6[bp] ; offset
    mov  al, 8[bp] ; data byte
    mov  [bx], al  ; write data byte
    pop  ds
    pop  bx
    pop  ax

  pop  bp
#endasm
}

  void
write_word(seg, offset, data)
  Bit16u seg;
  Bit16u offset;
{
#asm
  push bp
  mov  bp, sp

    push ax
    push bx
    push ds
    mov  ax, 4[bp] ; segment
    mov  ds, ax
    mov  bx, 6[bp] ; offset
    mov  ax, 8[bp] ; data word
    mov  [bx], ax  ; write data word
    pop  ds
    pop  bx
    pop  ax

  pop  bp
#endasm
}


  Bit16u
UDIV(a, b)
  Bit16u a, b;
{
  // divide a by b
  // return value in AX is:  AL=quotient, AH=remainder
#asm
  push bp
  mov  bp, sp

    push bx
    mov  ax, 4[bp] ;; a
    mov  bx, 6[bp] ;; b: only low eight bits used
    div  bl  ;; AX / BL -->  quotient=AL, remainder=AH
    pop  bx

  pop  bp
#endasm
}


//  Bit16u
//get_DS()
//{
//#asm
//  mov  ax, ds
//#endasm
//}

//  void
//set_DS(ds_selector)
//  Bit16u ds_selector;
//{
//#asm
//  push bp
//  mov  bp, sp
//
//    push ax
//    mov  ax, 4[bp] ; ds_selector
//    mov  ds, ax
//    pop  ax
//
//  pop  bp
//#endasm
//}

  Bit16u
get_SS()
{
#asm
  mov  ax, ss
#endasm
}

//--------------------------------------------------------------------------
// bios_printf()
//   A compact variable argument printf function which prints its output via
//   an I/O port so that it can be logged by Bochs.  Currently, only %x is
//   supported (or %02x, %04x, etc).
//--------------------------------------------------------------------------
  void
bios_printf(bomb, s)
  Boolean bomb;
  Bit8u *s;
{
  Bit8u c, format_char;
  Boolean  in_format;
  unsigned format_width, i;
  Bit16u  *arg_ptr;
  Bit16u   arg_seg, arg, digit, nibble, shift_count;

  arg_ptr = &s;
  arg_seg = get_SS();

  in_format = 0;
  format_width = 0;

  while (c = read_byte(0xf000, s)) {
    if ( c == '%' ) {
      in_format = 1;
      format_width = 0;
      }
    else if (in_format) {
      if ( (c>='0') && (c<='9') ) {
        format_width = (format_width * 10) + (c - '0');
        }
      else if (c == 'x') {
        arg_ptr++; // increment to next arg
        arg = read_word(arg_seg, arg_ptr);
        if (format_width == 0)
          format_width = 4;
        i = 0;
        digit = format_width - 1;
        for (i=0; i<format_width; i++) {
          nibble = (arg >> (4 * digit)) & 0x000f;
          if (nibble <= 9)
            outb(0xfff0, nibble + '0');
          else
            outb(0xfff0, (nibble - 10) + 'A');
          digit--;
          }
        in_format = 0;
        }
      //else if (c == 'd') {
      //  in_format = 0;
      //  }
      else
        panic("bios_printf: unknown format\n");
      }
    else {
      outb(0xfff0, c);
      }
    s ++;
    }

  if (bomb) {
#asm
    HALT(__LINE__)
#endasm
    }
}

  void
cli()
{
#asm
  cli
#endasm
}

  void
keyboard_panic()
{
  panic("Keyboard RESET error\n");
}


  Boolean
set_enable_a20(val)
  Boolean val;
{
  Boolean oldval;
  Bit8u  temp8;

  // Use keyboard conroller to set A20 enable

  // get current Output Port settings first
  if ( (inb(0x64) & 0x02) != 0 )
    panic("set_a20(1): ctrl busy\n");
  outb(0x64, 0xd0); // send Read Output Port Command
  if ( (inb(0x64) & 0x01) != 1 )
    panic("set_a20(2): ctrl busy\n");
  temp8 = inb(0x60);

  // store old value for return
  oldval = (temp8 >> 1) & 0x01;

  // change A20 status in Output Port settings
  if (val)
    temp8 |= 0x02;
  else
    temp8 &= 0xfd;

  // write new Output Port back
  if ( (inb(0x64) & 0x02) != 0 )
    panic("set_a20(3): ctrl busy\n");
  outb(0x64, 0xd1); // send Write Output Port Command
  if ( (inb(0x64) & 0x02) != 0 )
    panic("set_a20(4): ctrl busy\n");
  outb(0x60, temp8);

  return(oldval);
}

  void
debugger_on()
{
  outb(0xfedc, 0x01);
}

  void
debugger_off()
{
  outb(0xfedc, 0x00);
}



  void
int15_function(DI, SI, BP, SP, BX, DX, CX, AX, ES, DS, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, ES, DS, FLAGS;
{
  Bit16u ebda_seg;
  Bit8u  mouse_flags_1, mouse_flags_2;
  Bit16u mouse_driver_seg;
  Bit16u mouse_driver_offset;
  Bit8u  in_byte;
  Bit8u  response, prev_command_byte;
  Boolean prev_a20_enable;
  Bit16u  base15_00;
  Bit8u   base23_16;
  Bit16u  ss;
  Bit8u   ret, mouse_data1, mouse_data2, mouse_data3;
  Bit8u   comm_byte;

  switch (GET_AH()) {
    case 0x24: /* A20 Control */
      printf("BIOS: int15: Func 24h, subfunc %02xh, A20 gate control not supported\n", (unsigned) GET_AL());
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    case 0x41:
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    case 0x4f:
      /* keyboard intercept, ignore */
#if BX_CPU < 2
      /* XT keyboard doesn't use */
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
#else
      /* AT keyboard.  BIOS just does an IRET */
      /* nothing required */
if (GET_CF() == 0) printf("int15h: default handler encounters CF=0\n");
#endif
      break;

    case 0x87:
#if BX_CPU < 3
#  error "Int15 function 87h not supported on < 80386"
#endif
      // +++ should probably have descriptor checks
      // +++ should have exception handlers

      cli();

      prev_a20_enable = set_enable_a20(1); // enable A20 line

      // 128K max of transfer on 386+ ???
      // source == destination ???

      // ES:SI points to descriptor table
      // offset   use     initially  comments
      // ==============================================
      // 00..07   Unused  zeros      Null descriptor
      // 08..0f   GDT     zeros      filled in by BIOS
      // 10..17   source  ssssssss   source of data
      // 18..1f   dest    dddddddd   destination of data
      // 20..27   CS      zeros      filled in by BIOS
      // 28..2f   SS      zeros      filled in by BIOS

      //es:si
      //eeee0
      //0ssss
      //-----

// check for access rights of source & dest here

      // Initialize GDT descriptor
      base15_00 = (ES << 4) + SI;
      base23_16 = ES >> 12;
      if (base15_00 < (ES<<4))
        base23_16++;
      write_word(ES, SI+0x08+0, 47);       // limit 15:00 = 6 * 8bytes/descriptor
      write_word(ES, SI+0x08+2, base15_00);// base 15:00
      write_byte(ES, SI+0x08+4, base23_16);// base 23:16
      write_byte(ES, SI+0x08+5, 0x93);     // access
      write_word(ES, SI+0x08+6, 0x0000);   // base 31:24/reserved/limit 19:16

      // Initialize CS descriptor
      write_word(ES, SI+0x20+0, 0xffff);// limit 15:00 = normal 64K limit
      write_word(ES, SI+0x20+2, 0x0000);// base 15:00
      write_byte(ES, SI+0x20+4, 0x000f);// base 23:16
      write_byte(ES, SI+0x20+5, 0x9b);  // access
      write_word(ES, SI+0x20+6, 0x0000);// base 31:24/reserved/limit 19:16

      // Initialize SS descriptor
      ss = get_SS();
      base15_00 = ss << 4;
      base23_16 = ss >> 12;
      write_word(ES, SI+0x28+0, 0xffff);   // limit 15:00 = normal 64K limit
      write_word(ES, SI+0x28+2, base15_00);// base 15:00
      write_byte(ES, SI+0x28+4, base23_16);// base 23:16
      write_byte(ES, SI+0x28+5, 0x93);     // access
      write_word(ES, SI+0x28+6, 0x0000);   // base 31:24/reserved/limit 19:16

#asm
      // Compile generates locals offset info relative to SP.
      // Get CX (word count) from stack.
      mov  bx, sp
      SEG SS
        mov  cx, _int15_function.CX [bx]

      // since we need to set SS:SP, save them to the BDA
      // for future restore
      mov ax, #0x00
      mov ds, ax
      mov 0x0469, ss
      mov 0x0467, sp

      SEG ES
        lgdt [si + 0x08]
      SEG CS
        lidt [pmode_IDT_info]
      ;;  perhaps do something with IDT here

      ;; set PE bit in CR0
      xor  eax, eax
      mov  al, #0x01
      mov  cr0, eax
      ;; far jump to flush CPU queue after transition to protected mode
      JMP_AP(0x0020, protected_mode)

protected_mode:
      ;; GDT points to valid descriptor table, now load SS, DS, ES
      mov  ax, #0x28 ;; 101 000 = 5th descriptor in table, TI=GDT, RPL=00
      mov  ss, ax
      mov  ax, #0x10 ;; 010 000 = 2nd descriptor in table, TI=GDT, RPL=00
      mov  ds, ax
      mov  ax, #0x18 ;; 011 000 = 3rd descriptor in table, TI=GDT, RPL=00
      mov  es, ax
      xor  si, si
      xor  di, di
      cld
      rep
        movsw  ;; move CX words from DS:SI to ES:DI

      ;; clear CR3 and reset PG bit in CR0 ???
      xor  eax, eax
      mov cr0, eax

      ;; far jump to flush CPU queue after transition to real mode
      JMP_AP(0xf000, real_mode)

real_mode:
      ;; restore IDT to normal real-mode defaults
      SEG CS
        lidt [rmode_IDT_info]

      // restore SS:SP from the BDA
      mov ax, #0x00
      mov ds, ax
      mov ss, 0x0469
      mov sp, 0x0467
#endasm

      set_enable_a20(prev_a20_enable);
      SET_AH(0);
      CLEAR_CF();
      break;


    case 0x88: /* extended memory size */
#if BX_CPU < 2
      SET_AH(UNSUPPORTED_FUNCTION);
      SET_CF();
#else
      /* ??? change this back later... */
      /* number of 1K blocks of extended memory, subtract off 1st 1Meg */
      // AX = bx_mem.get_memory_in_k() - 1024;
      in_byte = inb_cmos(0x30);
      SET_AL(in_byte);
      in_byte = inb_cmos(0x31);
      SET_AH(in_byte);
      CLEAR_CF();
#endif
      break;

    case 0x90:
      /* Device busy interrupt.  Called by Int 16h when no key available */
      break;

    case 0x91:
      /* Interrupt complete.  Called by Int 16h when key becomes available */
      break;

    case 0xbf:
      printf("BIOS: *** int 15h function AH=bf not yet supported!\n");
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    case 0xC0:
#if 0
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;
#endif
      CLEAR_CF();
      SET_AH(0);
      BX =  BIOS_CONFIG_TABLE;
      ES = 0xF000;
      break;

    case 0xc1:
#if BX_USE_PS2_MOUSE
      ES = read_word(0x0040, 0x000E);
      CLEAR_CF();
#else
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
#endif
      break;

    case 0xC2:
      // Return Codes status in AH
      // =========================
      // 00: success
      // 01: invalid subfunction (AL > 7)
      // 02: invalid input value (out of allowable range)
      // 03: interface error
      // 04: resend command received from mouse controller,
      //     device driver should attempt command again
      // 05: cannot enable mouse, since no far call has been installed
      // 80/86: mouse service not implemented

#if BX_USE_PS2_MOUSE < 1
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
#else
      ebda_seg = read_word(0x0040, 0x000E);
      switch (GET_AL()) {
        case 0: // Disable/Enable Mouse
printf("case 0:\n");
          switch (GET_BH()) {
            case 0: // Disable Mouse
printf("case 0: disable mouse\n");
              inhibit_mouse_int_and_events(); // disable IRQ12 and packets
              ret = send_to_mouse_ctrl(0xF5); // disable mouse command
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if ( (ret == 0) || (mouse_data1 == 0xFA) ) {
                  CLEAR_CF();
                  SET_AH(0);
                  return;
                  }
                }

              // error
              SET_CF();
              SET_AH(ret);
              return;
              break;

            case 1: // Enable Mouse
printf("case 1: enable mouse\n");
              mouse_flags_2 = read_byte(ebda_seg, 0x0027);
              if ( (mouse_flags_2 & 0x80) == 0 ) {
                //printf("INT 15h C2 Enable Mouse, no far call handler\n");
                SET_CF();  // error
                SET_AH(5); // no far call installed
                return;
                }
              inhibit_mouse_int_and_events(); // disable IRQ12 and packets
              ret = send_to_mouse_ctrl(0xF4); // enable mouse command
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if ( (ret == 0) && (mouse_data1 == 0xFA) ) {
                  enable_mouse_int_and_events(); // turn IRQ12 and packet generation on
                  CLEAR_CF();
                  SET_AH(0);
                  return;
                  }
                }
              SET_CF();
              SET_AH(ret);
              return;

            default: // invalid subfunction
              //printf("INT 15h C2 AL=0, BH=%02x\n", (unsigned) GET_BH());
              SET_CF();  // error
              SET_AH(1); // invalid subfunction
              return;
            }
          break;

        case 1: // Reset Mouse
        case 5: // Initialize Mouse
printf("case 1 or 5:\n");
          if (GET_AL() == 5) {
            if (GET_BH() != 3)
              panic("INT 15h C2 AL=5, BH=%02x\n", (unsigned) GET_BH());
            mouse_flags_2 = read_byte(ebda_seg, 0x0027);
            mouse_flags_2 = (mouse_flags_2 & 0x00) | GET_BH();
            mouse_flags_1 = 0x00;
            write_byte(ebda_seg, 0x0026, mouse_flags_1);
            write_byte(ebda_seg, 0x0027, mouse_flags_2);
            }

          inhibit_mouse_int_and_events(); // disable IRQ12 and packets
          ret = send_to_mouse_ctrl(0xFF); // disable mouse command
          if (ret == 0) {
            ret = get_mouse_data(&mouse_data3);
            if (mouse_data3 != 0xfa)
              panic("Mouse reset returned %02x (should be ack)\n", (unsigned)mouse_data3);
            if ( ret == 0 ) {
              ret = get_mouse_data(&mouse_data1);
              if ( ret == 0 ) {
                ret = get_mouse_data(&mouse_data2);
                if ( ret == 0 ) {
                  // turn IRQ12 and packet generation on
                  enable_mouse_int_and_events();
                  CLEAR_CF();
                  SET_AH(0);
                  SET_BL(mouse_data1);
                  SET_BH(mouse_data2);
                  return;
                  }
                }
              }
            }

          // error
          SET_CF();
          SET_AH(ret);
          return;

        case 2: // Set Sample Rate
printf("case 2:\n");
          switch (GET_BH()) {
            case 0: //  10 reports/sec
            case 1: //  20 reports/sec
            case 2: //  40 reports/sec
            case 3: //  60 reports/sec
            case 4: //  80 reports/sec
            case 5: // 100 reports/sec (default)
            case 6: // 200 reports/sec
              CLEAR_CF();
              SET_AH(0);
              break;
            default:
              panic("INT 15h C2 AL=2, BH=%02x\n", (unsigned) GET_BH());
            }
          break;

        case 3: // Set Resolution
printf("case 3:\n");
          // BX:
          //      0 =  25 dpi, 1 count  per millimeter
          //      1 =  50 dpi, 2 counts per millimeter
          //      2 = 100 dpi, 4 counts per millimeter
          //      3 = 200 dpi, 8 counts per millimeter
          CLEAR_CF();
          SET_AH(0);
          break;

        case 4: // Get Device ID
printf("case 4:\n");
          CLEAR_CF();
          SET_AH(0);
          SET_BH(0);
          break;

        case 6: // Return Status & Set Scaling Factor...
printf("case 6:\n");
          switch (GET_BH()) {
            case 0: // Return Status
              comm_byte = inhibit_mouse_int_and_events(); // disable IRQ12 and packets
              ret = send_to_mouse_ctrl(0xE9); // get mouse info command
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if (mouse_data1 != 0xfa)
                  panic("Mouse status returned %02x (should be ack)\n", (unsigned)mouse_data1);
                if (ret == 0) {
                  ret = get_mouse_data(&mouse_data1);
                  if ( ret == 0 ) {
                    ret = get_mouse_data(&mouse_data2);
                    if ( ret == 0 ) {
                      ret = get_mouse_data(&mouse_data3);
                      if ( ret == 0 ) {
                        CLEAR_CF();
                        SET_AH(0);
                        SET_BL(mouse_data1);
                        SET_CL(mouse_data2);
                        SET_DL(mouse_data3);
                        set_kbd_command_byte(comm_byte); // restore IRQ12 and serial enable
                        return;
                        }
                      }
                    }
                  }
                }

              // error
              SET_CF();
              SET_AH(ret);
              set_kbd_command_byte(comm_byte); // restore IRQ12 and serial enable
              return;

            case 1: // Set Scaling Factor to 1:1
              CLEAR_CF();
              SET_AH(0);
              break;

            default:
              panic("INT 15h C2 AL=6, BH=%02x\n", (unsigned) GET_BH());
            }
          break;

        case 7: // Set Mouse Handler Address
printf("case 7:\n");
          mouse_driver_seg = ES;
          mouse_driver_offset = BX;
          write_word(ebda_seg, 0x0022, mouse_driver_offset);
          write_word(ebda_seg, 0x0024, mouse_driver_seg);
          mouse_flags_2 = read_byte(ebda_seg, 0x0027);
          mouse_flags_2 |= 0x80;
          write_byte(ebda_seg, 0x0027, mouse_flags_2);
          CLEAR_CF();
          SET_AH(0);
          break;

        default:
printf("case default:\n");
          SET_AH(1); // invalid function
          SET_CF();
        }
#endif
      break;

    case 0xC4:
      printf("BIOS: *** int 15h function AX=%04x, BX=%04x not yet supported!\n",
        (unsigned) AX, (unsigned) BX);
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    case 0xD8:
      printf("BIOS: *** int 15h function AX=D8 not yet supported!\n");
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    case 0xe0:
      printf("BIOS: *** int 15h function AH=e0 not yet supported!\n");
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;

    default:
      printf("BIOS: *** int 15h function AH=%02x not yet supported!\n",
        (unsigned) GET_AH());
      SET_CF();
      SET_AH(UNSUPPORTED_FUNCTION);
      break;
    }
}


  void
int16_function(DI, SI, BP, SP, BX, DX, CX, AX, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, FLAGS;
{
  Bit8u scan_code, ascii_code, shift_flags;

  switch (GET_AH()) {
    case 0x00: /* read keyboard input */

      if ( !dequeue_key(&scan_code, &ascii_code, 1) ) {
        panic("KBD: int16h: out of keyboard input\n");
        }
      AX = (scan_code << 8) | ascii_code;
      break;

    case 0x01: /* check keyboard status */
      if ( !dequeue_key(&scan_code, &ascii_code, 0) ) {
        SET_ZF();
        return;
        }
      AX = (scan_code << 8) | ascii_code;
      CLEAR_ZF();
      break;

    case 0x02: /* get shift flag status */
      /*AL = 0;*/
      shift_flags = read_byte(0x0040, 0x17);
      SET_AL(shift_flags);
      break;

    default:
      /*bx_cpu.set_ZF(1);*/
      /* ??? */
      printf("KBD: unsupported int 16h function %02x\n", GET_AH());
    }
}

  unsigned int
dequeue_key(scan_code, ascii_code, incr)
  Bit8u *scan_code;
  Bit8u *ascii_code;
  unsigned int incr;
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail;
  Bit16u ss;
  Bit8u  acode, scode;

#if BX_CPU < 2
  buffer_start = 0x001E;
  buffer_end   = 0x003E;
#else
  buffer_start = read_word(0x0040, 0x0080);
  buffer_end   = read_word(0x0040, 0x0082);
#endif

  buffer_head = read_word(0x0040, 0x001a);
  buffer_tail = read_word(0x0040, 0x001c);

  if (buffer_head != buffer_tail) {
    ss = get_SS();
    acode = read_byte(0x0040, buffer_head);
    scode = read_byte(0x0040, buffer_head+1);
    write_byte(ss, ascii_code, acode);
    write_byte(ss, scan_code, scode);

    if (incr) {
      buffer_head += 2;
      if (buffer_head >= buffer_end)
        buffer_head = buffer_start;
      write_word(0x0040, 0x001a, buffer_head);
      }
    return(1);
    }
  else {
    return(0);
    }
}



  Bit8u
inhibit_mouse_int_and_events()
{
  Bit8u command_byte, prev_command_byte;

  // Turn off IRQ generation and aux data line
  if ( inb(0x64) & 0x02 )
    panic("inhibmouse: keyboard input buffer full\n");
  outb(0x64, 0x20); // get command byte
  while ( (inb(0x64) & 0x01) != 0x01 );
  prev_command_byte = inb(0x60);
  command_byte = prev_command_byte;
  //while ( (inb(0x64) & 0x02) );
  if ( inb(0x64) & 0x02 )
    panic("inhibmouse, keyboard input buffer full\n");
  command_byte &= 0xfd; // turn off IRQ 12 generation
  command_byte |= 0x20; // disable mouse serial clock line
  outb(0x64, 0x60); // write command byte
  outb(0x60, command_byte);
  return(prev_command_byte);
}

  void
enable_mouse_int_and_events()
{
  Bit8u command_byte;

  // Turn on IRQ generation and aux data line
  if ( inb(0x64) & 0x02 )
    panic("enabmouse: keyboard input buffer full\n");
  outb(0x64, 0x20); // get command byte
  while ( (inb(0x64) & 0x01) != 0x01 );
  command_byte = inb(0x60);
  //while ( (inb(0x64) & 0x02) );
  if ( inb(0x64) & 0x02 )
    panic("enabmouse, keyboard input buffer full\n");
  command_byte |= 0x02; // turn on IRQ 12 generation
  command_byte &= 0xdf; // enable mouse serial clock line
  outb(0x64, 0x60); // write command byte
  outb(0x60, command_byte);
}

  Bit8u
send_to_mouse_ctrl(sendbyte)
  Bit8u sendbyte;
{
  Bit8u response;

  // wait for chance to write to ctrl
  if ( inb(0x64) & 0x02 )
    panic("sendmouse, keyboard input buffer full\n");
  outb(0x64, 0xD4);
  outb(0x60, sendbyte);
  return(0);
}


  Bit8u
get_mouse_data(data)
  Bit8u *data;
{
  Bit8u response;
  Bit16u ss;

  while ( (inb(0x64) & 0x21) != 0x21 ) {
    }

  response = inb(0x60);

  ss = get_SS();
  write_byte(ss, data, response);
  return(0);
}

  void
set_kbd_command_byte(command_byte)
  Bit8u command_byte;
{
  if ( inb(0x64) & 0x02 )
    panic("setkbdcomm, input buffer full\n");

  outb(0x64, 0x60); // write command byte
  outb(0x60, command_byte);
}

  void
int09_function(DI, SI, BP, SP, BX, DX, CX, AX)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX;
{
  Bit8u scancode, asciicode, shift_flags;

  //
  // DS has been set to F000 before call
  //

  scancode = GET_AL();

  if (scancode == 0) {
    printf("KBD: int09 handler: AL=0\n");
    return;
    }


  shift_flags = read_byte(0x0040, 0x17);

  switch (scancode) {
    case 0x3a: /* Caps Lock press */
      shift_flags |= 0x40;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xba: /* Caps Lock release */
      shift_flags &= ~0x40;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    case 0x2a: /* L Shift press */
      shift_flags |= 0x02;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xaa: /* L Shift release */
      shift_flags &= ~0x02;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    case 0x36: /* R Shift press */
      shift_flags |= 0x01;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xb6: /* R Shift release */
      shift_flags &= ~0x01;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    case 0x1d: /* L Cttrl press */
      shift_flags |= 0x04;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0x9d: /* L Cttrl release */
      shift_flags &= ~0x04;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    case 0x38: /* L Alt press */
      shift_flags |= 0x08;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xb8: /* L Alt release */
      shift_flags &= ~0x08;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    case 0x45: /* Num Lock press */
      shift_flags |= 0x20;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xc5: /* Num Lock release */
      shift_flags &= ~0x20;
      write_byte(0x0040, 0x17, shift_flags);
      break;

    default:
      if (scancode & 0x80) return; /* toss key releases ... */
      if (scancode > MAX_SCAN_CODE) {
        panic("KBD: int09h_handler(): unknown scancode read!\n");
        return;
        }
      if (shift_flags & 0x08) { /* ALT */
        asciicode = scan_to_scanascii[scancode].alt;
        scancode = scan_to_scanascii[scancode].alt >> 8;
        }
      else if (shift_flags & 0x04) { /* CONTROL */
        asciicode = scan_to_scanascii[scancode].control;
        scancode = scan_to_scanascii[scancode].control >> 8;
        }
      else if (shift_flags & 0x43) { /* CAPSLOCK + LSHIFT + RSHIFT */
        /* check if both CAPSLOCK and a SHIFT key are pressed */
        if ((shift_flags & 0x03) && (shift_flags & 0x40)) {
          asciicode = scan_to_scanascii[scancode].normal;
          scancode = scan_to_scanascii[scancode].normal >> 8;
          }
        else {
          asciicode = scan_to_scanascii[scancode].shift;
          scancode = scan_to_scanascii[scancode].shift >> 8;
          }
        }
      else {
        asciicode = scan_to_scanascii[scancode].normal;
        scancode = scan_to_scanascii[scancode].normal >> 8;
        }
      if (scancode==0 && asciicode==0) {
        panic("KBD: int09h_handler(): scancode & asciicode are zero?\n");
        }
      enqueue_key(scancode, asciicode);
      break;
    }
}

  void
enqueue_key(scan_code, ascii_code)
  Bit8u scan_code, ascii_code;
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail, temp_tail;

  //printf("KBD:   enqueue_key() called scan:%02x, ascii:%02x\n",
  //    scan_code, ascii_code);

#if BX_CPU < 2
  buffer_start = 0x001E;
  buffer_end   = 0x003E;
#else
  buffer_start = read_word(0x0040, 0x0080);
  buffer_end   = read_word(0x0040, 0x0082);
#endif

  buffer_head = read_word(0x0040, 0x001A);
  buffer_tail = read_word(0x0040, 0x001C);

  temp_tail = buffer_tail;
  buffer_tail += 2;
  if (buffer_tail >= buffer_end)
    buffer_tail = buffer_start;

  if (buffer_tail == buffer_head) {
    panic("KBD: dropped key scan=%02x, ascii=%02x\n",
      (int) scan_code, (int) ascii_code);
    return;
    }

   write_byte(0x0040, temp_tail, ascii_code);
   write_byte(0x0040, temp_tail+1, scan_code);
   write_word(0x0040, 0x001C, buffer_tail);
}


  void
int74_function(make_farcall, Z, Y, X, status)
  Bit16u make_farcall, Z, Y, X, status;
{
  Bit8u  in_byte, index, package_count;
  Bit16u ebda_seg;
  Bit8u  mouse_flags_1, mouse_flags_2;

printf("entering int74_function\n");
  make_farcall = 0;

  in_byte = inb(0x64);
  if ( (in_byte & 0x21) != 0x21 ) {
    return;
    }
  in_byte = inb(0x60);
printf("int74: read byte %02x\n", in_byte);

  ebda_seg      = read_word(0x0040, 0x000E);
  mouse_flags_1 = read_byte(ebda_seg, 0x0026);
  mouse_flags_2 = read_byte(ebda_seg, 0x0027);

  if ( (mouse_flags_2 & 0x80) != 0x80 ) {
    panic("int74_function:\n");
    }

  package_count = mouse_flags_2 & 0x07;
  index = mouse_flags_1 & 0x07;
  write_byte(ebda_seg, 0x28 + index, in_byte);

  if ( (index+1) >= package_count ) {
printf("int74_function: make_farcall=1\n");
    status = read_byte(ebda_seg, 0x0028 + 0);
    X      = read_byte(ebda_seg, 0x0028 + 1);
    Y      = read_byte(ebda_seg, 0x0028 + 2);
    Z      = 0;
    mouse_flags_1 = 0;
    // check if far call handler installed
    if (mouse_flags_2 & 0x80)
      make_farcall = 1;
    }
  else {
    mouse_flags_1++;
    }
  write_byte(ebda_seg, 0x0026, mouse_flags_1);
}





  void
int13_function(DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS;
{
  Bit8u    drive, num_sectors, sector, head, status, mod;
  Bit8u    n_drives;
  Bit16u   cyl_mod, ax;
  Bit16u   max_cylinder, cylinder, total_sectors;
  Bit16u   hd_cylinders;
  Bit8u    hd_heads, hd_sectors;
  Bit16u   val16;
  Bit8u    sector_count;
  unsigned int i;
  Bit16u   tempbx;

  write_byte(0x0040, 0x008e, 0);  // clear completion flag

  /* at this point, DL is >= 0x80 to be passed from the floppy int13h
     handler code */
  /* check how many disks first (cmos reg 0x12), return an error if
     DL > n_drives */
  n_drives = inb_cmos(0x12);
  n_drives = ((n_drives & 0xf0)==0) ? 0 :
    ((n_drives & 0x0f) ? 2 : 1);

  if (!((GET_DL()&0x7f) < n_drives)) { /* allow 0, 1, or 2 disks */
    SET_AH(0x01);
    set_disk_ret_status(0x01);
    SET_CF(); /* error occurred */
    return;
    }

  switch (GET_AH()) {

    case 0x00: /* disk controller reset */
printf("int13_f00\n");

      SET_AH(0);
      set_disk_ret_status(0);
      set_diskette_ret_status(0);
      set_diskette_current_cyl(0, 0); /* current cylinder, diskette 1 */
      set_diskette_current_cyl(1, 0); /* current cylinder, diskette 2 */
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x01: /* read disk status */
printf("int13_f01\n");
      status = read_byte(0x0040, 0x0074);
      SET_AH(status);
      set_disk_ret_status(0);
      /* set CF if error status read */
      if (status) SET_CF();
      else        CLEAR_CF();
      return;
      break;

    case 0x04: // verify disk sectors
    case 0x02: // read disk sectors
      drive = GET_DL();
      get_hd_geometry(drive, &hd_cylinders, &hd_heads, &hd_sectors);

      num_sectors = GET_AL();
      cylinder    = GET_CH();
      cylinder    |= ( ((Bit16u) GET_CL()) << 2) & 0x300;
      sector      = (GET_CL() & 0x3f);
      head        = GET_DH();


      if (hd_cylinders > 1024) {
        if (hd_cylinders <= 2048) {
          cylinder <<= 1;
          }
        else if (hd_cylinders <= 4096) {
          cylinder <<= 2;
          }
        else if (hd_cylinders <= 8192) {
          cylinder <<= 3;
          }
        else { // hd_cylinders <= 16384
          cylinder <<= 4;
          }

        ax = UDIV(head, hd_heads);
        cyl_mod = ax & 0xff;
        head    = ax >> 8;
        cylinder |= cyl_mod;
        }

      if ( (cylinder >= hd_cylinders) ||
           (sector > hd_sectors) ||
           (head >= hd_heads) ) {
        SET_AH(1);
        set_disk_ret_status(1);
        SET_CF(); /* error occurred */
        return;
        }

      if ( (num_sectors > 128) || (num_sectors == 0) )
        panic("int13_function(): num_sectors out of range!\n");


      if (head > 15)
        panic("hard drive BIOS:(read/verify) head > 15\n");

      if ( GET_AH() == 0x04 ) {
        SET_AH(0);
        set_disk_ret_status(0);
        CLEAR_CF();
        return;
        }

      status = inb(0x1f7);
      if (status & 0x80) {
        panic("hard drive BIOS:(read/verify) BUSY bit set\n");
        }
      outb(0x01f2, num_sectors);
      outb(0x01f3, sector);
      outb(0x01f4, cylinder & 0x00ff);
      outb(0x01f5, cylinder >> 8);
      outb(0x01f6, 0xa0 | ((drive&1)<<4) | (head & 0x0f));
      outb(0x01f7, 0x20);

      while (1) {
        status = inb(0x1f7);
        if ( !(status & 0x80) ) break;
        }

      if ( !(status & 0x08) ) {
        printf("status was %02x\n", (unsigned) status);
        panic("hard drive BIOS:(read/verify) data-request bit not set\n");
        }

      sector_count = 0;
      tempbx = BX;

#asm
  sti  ;; enable higher priority interrupts
#endasm

      while (1) {
#asm
        ;; store temp bx in real DI register
        push bp
        mov  bp, sp
        mov  di, _int13_function.tempbx + 2 [bp]
        pop  bp

        ;; adjust if there will be an overrun
        cmp   di, #0xfe00
        jbe   i13_f02_no_adjust
i13_f02_adjust:
        sub   di, #0x0200 ; sub 512 bytes from offset
        mov   ax, es
        add   ax, #0x0020 ; add 512 to segment
        mov   es, ax

i13_f02_no_adjust:
        mov  cx, #0x0100   ;; counter (256 words = 512b)
        mov  dx, #0x01f0  ;; AT data read port

        rep
          insw ;; CX words transfered from port(DX) to ES:[DI]

i13_f02_done:
        ;; store real DI register back to temp bx
        push bp
        mov  bp, sp
        mov  _int13_function.tempbx + 2 [bp], di
        pop  bp
#endasm

        sector_count++;
        num_sectors--;
        if (num_sectors == 0) {
          status = inb(0x1f7);
          if ( (status & 0xc9) != 0x40 )
            panic("no sectors left to read/verify, status is %02x\n", (unsigned) status);
          break;
          }
        else {
          status = inb(0x1f7);
          if ( (status & 0xc9) != 0x48 )
            panic("more sectors left to read/verify, status is %02x\n", (unsigned) status);
          continue;
          }
        }

      SET_AH(0);
      set_disk_ret_status(0);
      SET_AL(sector_count);
      CLEAR_CF(); /* successful */
      return;
      break;


    case 0x03: /* write disk sectors */
printf("int13_f03\n");
      drive = GET_DL ();
      get_hd_geometry(drive, &hd_cylinders, &hd_heads, &hd_sectors);

      num_sectors = GET_AL();
      cylinder    = GET_CH();
      cylinder    |= ( ((Bit16u) GET_CL()) << 2) & 0x300;
      sector      = (GET_CL() & 0x3f);
      head        = GET_DH();

      if (hd_cylinders > 1024) {
        if (hd_cylinders <= 2048) {
          cylinder <<= 1;
          }
        else if (hd_cylinders <= 4096) {
          cylinder <<= 2;
          }
        else if (hd_cylinders <= 8192) {
          cylinder <<= 3;
          }
        else { // hd_cylinders <= 16384
          cylinder <<= 4;
          }

        ax = UDIV(head, hd_heads);
        cyl_mod = ax & 0xff;
        head    = ax >> 8;
        cylinder |= cyl_mod;
        }

      if ( (cylinder >= hd_cylinders) ||
           (sector > hd_sectors) ||
           (head >= hd_heads) ) {
        SET_AH( 1);
        set_disk_ret_status(1);
        SET_CF(); /* error occurred */
        return;
        }

      if ( (num_sectors > 128) || (num_sectors == 0) )
        panic("int13_function(): num_sectors out of range!\n");

      if (head > 15)
        panic("hard drive BIOS:(read) head > 15\n");

      status = inb(0x1f7);
      if (status & 0x80) {
        panic("hard drive BIOS:(read) BUSY bit set\n");
        }
// should check for Drive Ready Bit also in status reg
      outb(0x01f2, num_sectors);
      outb(0x01f3, sector);
      outb(0x01f4, cylinder & 0x00ff);
      outb(0x01f5, cylinder >> 8);
      outb(0x01f6, 0xa0 | ((drive&1)<<4) | (head & 0x0f));
      outb(0x01f7, 0x30);

      // wait for busy bit to turn off after seeking
      while (1) {
        status = inb(0x1f7);
        if ( !(status & 0x80) ) break;
        }

      if ( !(status & 0x08) ) {
        printf("status was %02x\n", (unsigned) status);
        panic("hard drive BIOS:(write) data-request bit not set\n");
        }

      sector_count = 0;
      tempbx = BX;

#asm
  sti  ;; enable higher priority interrupts
#endasm

      while (1) {
#asm
        ;; store temp bx in real SI register
        push bp
        mov  bp, sp
        mov  si, _int13_function.tempbx + 2 [bp]
        pop  bp

        ;; adjust if there will be an overrun
        cmp   si, #0xfe00
        jbe   i13_f03_no_adjust
i13_f03_adjust:
        sub   si, #0x0200 ; sub 512 bytes from offset
        mov   ax, es
        add   ax, #0x0020 ; add 512 to segment
        mov   es, ax

i13_f03_no_adjust:
        mov  cx, #0x0100   ;; counter (256 words = 512b)
        mov  dx, #0x01f0  ;; AT data read port

        seg ES
        rep
          outsw ;; CX words tranfered from ES:[SI] to port(DX)

        ;; store real SI register back to temp bx
        push bp
        mov  bp, sp
        mov  _int13_function.tempbx + 2 [bp], si
        pop  bp
#endasm

        sector_count++;
        num_sectors--;
        if (num_sectors == 0) {
          status = inb(0x1f7);
          if ( (status & 0xe9) != 0x40 )
            panic("no sectors left to write, status is %02x\n", (unsigned) status);
          break;
          }
        else {
          status = inb(0x1f7);
          if ( (status & 0xc9) != 0x48 )
            panic("more sectors left to write, status is %02x\n", (unsigned) status);
          continue;
          }
        }

      SET_AH(0);
      set_disk_ret_status(0);
      SET_AL(sector_count);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x05: /* format disk track */
printf("int13_f05\n");
      panic("format disk track called\n");
      /* nop */
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x08: /* read disk drive parameters */
printf("int13_f08\n");
      // return geom for drive 0x80, they asked for "max" cylinders anyway
      get_hd_geometry(0x80, &hd_cylinders, &hd_heads, &hd_sectors);

      // translate CHS
      //
      if (hd_cylinders <= 1024) {
        // hd_cylinders >>= 0;
        // hd_heads <<= 0;
        }
      else if (hd_cylinders <= 2048) {
        hd_cylinders >>= 1;
        hd_heads <<= 1;
        }
      else if (hd_cylinders <= 4096) {
        hd_cylinders >>= 2;
        hd_heads <<= 2;
        }
      else if (hd_cylinders <= 8192) {
        hd_cylinders >>= 3;
        hd_heads <<= 3;
        }
      else { // hd_cylinders <= 16384
        hd_cylinders >>= 4;
        hd_heads <<= 4;
        }

      max_cylinder = hd_cylinders - 2; /* 0 based */
      SET_AL(0);
      SET_CH(max_cylinder & 0xff);
      SET_CL(((max_cylinder >> 2) & 0xc0) | (hd_sectors & 0x3f));
      SET_DH(hd_heads - 1);
      SET_DL(n_drives); /* returns 0, 1, or 2 hard drives */
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x09: /* initialize drive parameters */
printf("int13_f09\n");
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x0a: /* read disk sectors with ECC */
printf("int13_f0a\n");
    case 0x0b: /* write disk sectors with ECC */
printf("int13_f0b\n");
      panic("int13h Functions 0Ah & 0Bh not implemented!\n");
      return;
      break;

    case 0x0c: /* seek to specified cylinder */
printf("int13_f0c\n");
      printf("int13h function 0ch (seek) not implemented!\n");
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x0d: /* alternate disk reset */
printf("int13_f0d\n");
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x10: /* check drive ready */
printf("int13_f10\n");
      //SET_AH(0);
      //set_disk_ret_status(0);
      //CLEAR_CF(); /* successful */
      //return;
      //break;

      // should look at 40:8E also???
      status = inb(0x01f7);
      if ( (status & 0xc0) == 0x40 ) {
        SET_AH(0);
        set_disk_ret_status(0);
        CLEAR_CF(); // drive ready
        return;
        }
      else {
        SET_AH(0xAA);
        set_disk_ret_status(0xAA);
        SET_CF(); // not ready
        return;
        }
      break;

    case 0x11: /* recalibrate */
printf("int13_f11\n");
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      return;
      break;

    case 0x14: /* controller internal diagnostic */
printf("int13_f14\n");
      SET_AH(0);
      set_disk_ret_status(0);
      CLEAR_CF(); /* successful */
      SET_AL(0);
      return;
      break;

    case 0x15: /* read disk drive size */
      drive = GET_DL();
      get_hd_geometry(drive, &hd_cylinders, &hd_heads, &hd_sectors);
#asm
      push bp
      mov  bp, sp
      mov  al, _int13_function.hd_heads + 2 [bp]
      mov  ah, _int13_function.hd_sectors + 2 [bp]
      mul  al, ah ;; ax = heads * sectors
      mov  bx, _int13_function.hd_cylinders + 2 [bp]
      dec  bx     ;; use (cylinders - 1) ???
      mul  ax, bx ;; dx:ax = (cylinders -1) * (heads * sectors)
      ;; now we need to move the 32bit result dx:ax to what the
      ;; BIOS wants which is cx:dx.
      ;; and then into CX:DX on the stack
      mov  _int13_function.CX + 2 [bp], dx
      mov  _int13_function.DX + 2 [bp], ax
      pop  bp
#endasm
      SET_AH(3);  // hard disk accessible
      set_disk_ret_status(0); // ??? should this be 0
      CLEAR_CF(); // successful
      return;
      break;

    case 0x18: /* */
    case 0x41: // IBM/MS installation check
printf("int13_f18,41\n");
      SET_AH(1);  // unsupported
      set_disk_ret_status(1);
      SET_CF(); /* unsuccessful */
      return;
      break;

    default:
      panic("case 0x%x found in int13_function()\n", (unsigned) GET_AH());
      break;
    }
}


//////////////////////
// FLOPPY functions //
//////////////////////

  Boolean
floppy_media_known(drive)
  Bit16u drive;
{
  Bit8u  val8;
  Bit16u media_state_offset;

  val8 = read_byte(0x0040, 0x003e); // diskette recal status
  if (drive)
    val8 >>= 1;
  val8 &= 0x01;
  if (val8 == 0)
    return(0);

  media_state_offset = 0x0090;
  if (drive)
    media_state_offset += 1;

  val8 = read_byte(0x0040, media_state_offset);
  val8 = (val8 >> 4) & 0x01;
  if (val8 == 0)
    return(0);

  // check pass, return KNOWN
  return(1);
}

  Boolean
floppy_media_sense(drive)
  Bit16u drive;
{
  Boolean retval;
  Bit16u  media_state_offset;
  Bit8u   drive_type, config_data, media_state;

  if (floppy_drive_recal(drive) == 0) {
    return(0);
    }

  // for now cheat and get drive type from CMOS,
  // assume media is same as drive type
  drive_type = inb_cmos(0x10);
  if (drive == 0)
    drive_type >>= 4;
  else
    drive_type &= 0x0f;
  if ( drive_type == 2 ) {
    // 1.2 MB 5.25" drive
    config_data = 0x00; // 0000 0000
    media_state = 0x25; // 0001 0101
    retval = 1;
    }
  else if ( drive_type == 3 ) {
    // 720K 3.5" drive
    config_data = 0x00; // 0000 0000 ???
    media_state = 0x17; // 0001 0111
    retval = 1;
    }
  else if ( drive_type == 4 ) {
    // 1.44 MB 3.5" drive
    config_data = 0x00; // 0000 0000
    media_state = 0x17; // 0001 0111
    retval = 1;
    }
  else if ( drive_type == 5 ) {
    // 2.88 MB 3.5" drive
    config_data = 0xCC; // 1100 1100
    media_state = 0xD7; // 1101 0111
    retval = 1;
    }
  else {
    // not recognized
    config_data = 0x00; // 0000 0000
    media_state = 0x00; // 0000 0000
    retval = 0;
    }

  if (drive == 0)
    media_state_offset = 0x90;
  else
    media_state_offset = 0x91;
  write_byte(0x0040, 0x008B, config_data);
  write_byte(0x0040, media_state_offset, media_state);

  return(retval);
}

  Boolean
floppy_drive_recal(drive)
  Bit16u drive;
{
  Bit8u  val8, dor;
  Bit16u curr_cyl_offset;

  // set 40:3e bit 7 to 0
  val8 = read_byte(0x0000, 0x043e);
  val8 &= 0x7f;
  write_byte(0x0000, 0x043e, val8);

  // turn on motor of selected drive, DMA & int enabled, normal operation
  if (drive)
    dor = 0x20;
  else
    dor = 0x10;
  dor |= 0x0c;
  dor |= drive;
  outb(0x03f2, dor);

  // check port 3f4 for drive readiness
  val8 = inb(0x3f4);
  if ( (val8 & 0xf0) != 0x80 )
    panic("floppy recal:f07: ctrl not ready\n");

  // send Recalibrate command (2 bytes) to controller
  outb(0x03f5, 0x07);  // 07: Recalibrate
  outb(0x03f5, drive); // 0=drive0, 1=drive1

 // turn on interrupts
#asm
  sti
#endasm

  // wait on 40:3e bit 7 to become 1
  val8 = (read_byte(0x0000, 0x043e) & 0x80);
  while ( val8 == 0 ) {
    val8 = (read_byte(0x0000, 0x043e) & 0x80);
    }

 val8 = 0; // separate asm from while() loop
 // turn off interrupts
#asm
  cli
#endasm

  // set 40:3e bit 7 to 0, and calibrated bit
  val8 = read_byte(0x0000, 0x043e);
  val8 &= 0x7f;
  if (drive) {
    val8 |= 0x02; // Drive 1 calibrated
    curr_cyl_offset = 0x0095;
    }
  else {
    val8 |= 0x01; // Drive 0 calibrated
    curr_cyl_offset = 0x0094;
    }
  write_byte(0x0040, 0x003e, val8);
  write_byte(0x0040, curr_cyl_offset, 0); // current cylinder is 0

  return(1);
}



  Boolean
floppy_drive_exists(drive)
  Bit16u drive;
{
  Bit8u  drive_type;

  // check CMOS to see if drive exists
  drive_type = inb_cmos(0x10);
  if (drive == 0)
    drive_type >>= 4;
  else
    drive_type &= 0x0f;
  if ( drive_type == 0 )
    return(0);
  else
    return(1);
}


#if BX_SUPPORT_FLOPPY
  void
int13_diskette_function(DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS;
{
  Bit8u  drive, num_sectors, track, sector, head, status;
  Bit16u base_address, base_count, base_es;
  Bit8u  page, mode_register, val8, dor;
  Bit8u  return_status[7];
  Bit8u  drive_type, num_floppies, ah;
  Bit16u es, last_addr;

//printf("BIOS: int13: AX=%04x BX=%04x CX=%04x DX=%04x\n", AX, BX, CX, DX);

  ah = GET_AH();

  switch ( ah ) {
    case 0x00: // diskette controller reset
printf("floppy f00\n");
      drive = GET_DL();
      if (drive > 1) {
        SET_AH(1); // invalid param
        set_diskette_ret_status(1);
        SET_CF();
        return;
        }
      drive_type = inb_cmos(0x10);

      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      if (drive_type == 0) {
        SET_AH(0x80); // drive not responding
        set_diskette_ret_status(0x80);
        SET_CF();
        return;
        }
      SET_AH(0);
      set_diskette_ret_status(0);
      CLEAR_CF(); // successful
      set_diskette_current_cyl(drive, 0); // current cylinder
      return;

    case 0x01: // Read Diskette Status
      CLEAR_CF();
      val8 = read_byte(0x0000, 0x0441);
      SET_AH(val8);
      if (val8) {
        SET_CF();
        }
      return;

    case 0x02: // Read Diskette Sectors
    case 0x03: // Write Diskette Sectors
    case 0x04: // Verify Diskette Sectors
      num_sectors = GET_AL();
      track       = GET_CH();
      sector      = GET_CL();
      head        = GET_DH();
      drive       = GET_DL();

      if ( (drive > 1) || (head > 1) ||
           (num_sectors == 0) || (num_sectors > 72) ) {
printf("floppy: drive>1 || head>1 ...\n");
        SET_AH(1);
        set_diskette_ret_status(1);
        SET_AL(0); // no sectors read
        SET_CF(); // error occurred
        return;
        }

      // see if drive exists
      if (floppy_drive_exists(drive) == 0) {
        SET_AH(0x80); // not responding
        set_diskette_ret_status(0x80);
        SET_AL(0); // no sectors read
        SET_CF(); // error occurred
        return;
        }

      // see if media in drive, and type is known
      if (floppy_media_known(drive) == 0) {
        if (floppy_media_sense(drive) == 0) {
          SET_AH(0x0C); // Media type not found
          set_diskette_ret_status(0x0C);
          SET_AL(0); // no sectors read
          SET_CF(); // error occurred
          return;
          }
        }

      if (ah == 0x02) {
        // Read Diskette Sectors

        //-----------------------------------
        // set up DMA controller for transfer
        //-----------------------------------

        // es:bx = pointer to where to place information from diskette
        // port 04: DMA-1 base and current address, channel 2
        // port 05: DMA-1 base and current count, channel 2
        page = (ES >> 12);   // upper 4 bits
        base_es = (ES << 4); // lower 16bits contributed by ES
        base_address = base_es + BX; // lower 16 bits of address
                                     // contributed by ES:BX
        if ( base_address < base_es ) {
          // in case of carry, adjust page by 1
          page++;
          }
        base_count = (num_sectors * 512) - 1;

        // check for 64K boundary overrun
        last_addr = base_address + base_count;
        if (last_addr < base_address) {
          SET_AH(0x09);
          set_diskette_ret_status(0x09);
          SET_AL(0); // no sectors read
          SET_CF(); // error occurred
          return;
          }

        printf("masking DMA-1 c2\n");
        outb(0x000a, 0x06);

  printf("clear flip-flop\n");
        outb(0x000c, 0x00); // clear flip-flop
        outb(0x0004, base_address);
        outb(0x0004, base_address>>8);
  printf("clear flip-flop\n");
        outb(0x000c, 0x00); // clear flip-flop
        outb(0x0005, base_count);
        outb(0x0005, base_count>>8);

        // port 0b: DMA-1 Mode Register
        mode_register = 0x46; // single mode, increment, autoinit disable,
                              // transfer type=write, channel 2
  printf("setting mode register\n");
        outb(0x000b, mode_register);

  printf("setting page register\n");
        // port 81: DMA-1 Page Register, channel 2
        outb(0x0081, page);

  printf("unmask chan 2\n");
        outb(0x000a, 0x02); // unmask channel 2

        printf("unmasking DMA-1 c2\n");
        outb(0x000a, 0x02);

        //--------------------------------------
        // set up floppy controller for transfer
        //--------------------------------------

        // set 40:3e bit 7 to 0
        val8 = read_byte(0x0000, 0x043e);
        val8 &= 0x7f;
        write_byte(0x0000, 0x043e, val8);

        // turn on motor of selected drive, DMA & int enabled, normal operation
        if (drive)
          dor = 0x20;
        else
          dor = 0x10;
        dor |= 0x0c;
        dor |= drive;
        outb(0x03f2, dor);

        // check port 3f4 for drive readiness
        val8 = inb(0x3f4);
        if ( (val8 & 0xf0) != 0x80 )
          panic("int13_diskette:f02: ctrl not ready\n");

        // send read-normal-data command (9 bytes) to controller
        outb(0x03f5, 0xe6); // e6: read normal data
        outb(0x03f5, (head << 2) | drive); // HD DR1 DR2
        outb(0x03f5, track);
        outb(0x03f5, head);
        outb(0x03f5, sector);
        outb(0x03f5, 2); // 512 byte sector size
        outb(0x03f5, 0); // last sector number possible on track
        outb(0x03f5, 0); // Gap length
        outb(0x03f5, 0xff); // Gap length

       // turn on interrupts
  #asm
        sti
  #endasm

        // wait on 40:3e bit 7 to become 1
        val8 = (read_byte(0x0000, 0x043e) & 0x80);
        while ( val8 == 0 ) {
          val8 = (read_byte(0x0000, 0x043e) & 0x80);
          }

       val8 = 0; // separate asm from while() loop
       // turn off interrupts
  #asm
        cli
  #endasm

        // set 40:3e bit 7 to 0
        val8 = read_byte(0x0000, 0x043e);
        val8 &= 0x7f;
        write_byte(0x0000, 0x043e, val8);

        // check port 3f4 for accessibility to status bytes
        val8 = inb(0x3f4);
        if ( (val8 & 0xc0) != 0xc0 )
          panic("int13_diskette: ctrl not ready\n");

        // read 7 return status bytes from controller
        // using loop index broken, have to unroll...
        return_status[0] = inb(0x3f5);
        return_status[1] = inb(0x3f5);
        return_status[2] = inb(0x3f5);
        return_status[3] = inb(0x3f5);
        return_status[4] = inb(0x3f5);
        return_status[5] = inb(0x3f5);
        return_status[6] = inb(0x3f5);
        // record in BIOS Data Area
        write_byte(0x0040, 0x0042, return_status[0]);
        write_byte(0x0040, 0x0043, return_status[1]);
        write_byte(0x0040, 0x0044, return_status[2]);
        write_byte(0x0040, 0x0045, return_status[3]);
        write_byte(0x0040, 0x0046, return_status[4]);
        write_byte(0x0040, 0x0047, return_status[5]);
        write_byte(0x0040, 0x0048, return_status[6]);

        if ( (return_status[0] & 0xc0) != 0 ) {
          SET_AH(0x20);
          set_diskette_ret_status(0x20);
          SET_AL(0); // no sectors read
          SET_CF(); // error occurred
          return;
          }

        // ??? should track be new val from return_status[3] ?
        set_diskette_current_cyl(drive, track);
        // AL = number of sectors read (same value as passed)
        SET_AH(0x00); // success
        CLEAR_CF();   // success
        return;
        }
      else if (ah == 0x03) {
        // Write Diskette Sectors

        //-----------------------------------
        // set up DMA controller for transfer
        //-----------------------------------

        // es:bx = pointer to where to place information from diskette
        // port 04: DMA-1 base and current address, channel 2
        // port 05: DMA-1 base and current count, channel 2
        page = (ES >> 12);   // upper 4 bits
        base_es = (ES << 4); // lower 16bits contributed by ES
        base_address = base_es + BX; // lower 16 bits of address
                                     // contributed by ES:BX
        if ( base_address < base_es ) {
          // in case of carry, adjust page by 1
          page++;
          }
        base_count = (num_sectors * 512) - 1;

        // check for 64K boundary overrun
        last_addr = base_address + base_count;
        if (last_addr < base_address) {
          SET_AH(0x09);
          set_diskette_ret_status(0x09);
          SET_AL(0); // no sectors read
          SET_CF(); // error occurred
          return;
          }

        printf("masking DMA-1 c2\n");
        outb(0x000a, 0x06);

        outb(0x000c, 0x00); // clear flip-flop
        outb(0x0004, base_address);
        outb(0x0004, base_address>>8);
        outb(0x000c, 0x00); // clear flip-flop
        outb(0x0005, base_count);
        outb(0x0005, base_count>>8);

        // port 0b: DMA-1 Mode Register
        mode_register = 0x4a; // single mode, increment, autoinit disable,
                              // transfer type=read, channel 2
        outb(0x000b, mode_register);

        // port 81: DMA-1 Page Register, channel 2
        outb(0x0081, page);

        printf("unmasking DMA-1 c2\n");
        outb(0x000a, 0x02);

        //--------------------------------------
        // set up floppy controller for transfer
        //--------------------------------------

        // set 40:3e bit 7 to 0
        val8 = read_byte(0x0000, 0x043e);
        val8 &= 0x7f;
        write_byte(0x0000, 0x043e, val8);

        // turn on motor of selected drive, DMA & int enabled, normal operation
        if (drive)
          dor = 0x20;
        else
          dor = 0x10;
        dor |= 0x0c;
        dor |= drive;
        outb(0x03f2, dor);

        // check port 3f4 for drive readiness
        val8 = inb(0x3f4);
        if ( (val8 & 0xf0) != 0x80 )
          panic("int13_diskette:f03: ctrl not ready\n");

        // send read-normal-data command (9 bytes) to controller
        outb(0x03f5, 0xc5); // c5: write normal data
        outb(0x03f5, (head << 2) | drive); // HD DR1 DR2
        outb(0x03f5, track);
        outb(0x03f5, head);
        outb(0x03f5, sector);
        outb(0x03f5, 2); // 512 byte sector size
        outb(0x03f5, 0); // last sector number possible on track
        outb(0x03f5, 0); // Gap length
        outb(0x03f5, 0xff); // Gap length

       // turn on interrupts
  #asm
        sti
  #endasm

        // wait on 40:3e bit 7 to become 1
        val8 = (read_byte(0x0000, 0x043e) & 0x80);
        while ( val8 == 0 ) {
          val8 = (read_byte(0x0000, 0x043e) & 0x80);
          }

       val8 = 0; // separate asm from while() loop
       // turn off interrupts
  #asm
        cli
  #endasm

        // set 40:3e bit 7 to 0
        val8 = read_byte(0x0000, 0x043e);
        val8 &= 0x7f;
        write_byte(0x0000, 0x043e, val8);

        // check port 3f4 for accessibility to status bytes
        val8 = inb(0x3f4);
        if ( (val8 & 0xc0) != 0xc0 )
          panic("int13_diskette: ctrl not ready\n");

        // read 7 return status bytes from controller
        // using loop index broken, have to unroll...
        return_status[0] = inb(0x3f5);
        return_status[1] = inb(0x3f5);
        return_status[2] = inb(0x3f5);
        return_status[3] = inb(0x3f5);
        return_status[4] = inb(0x3f5);
        return_status[5] = inb(0x3f5);
        return_status[6] = inb(0x3f5);
        // record in BIOS Data Area
        write_byte(0x0040, 0x0042, return_status[0]);
        write_byte(0x0040, 0x0043, return_status[1]);
        write_byte(0x0040, 0x0044, return_status[2]);
        write_byte(0x0040, 0x0045, return_status[3]);
        write_byte(0x0040, 0x0046, return_status[4]);
        write_byte(0x0040, 0x0047, return_status[5]);
        write_byte(0x0040, 0x0048, return_status[6]);

        if ( (return_status[0] & 0xc0) != 0 ) {
          panic("int13_diskette_function: read error\n");
          }

        // ??? should track be new val from return_status[3] ?
        set_diskette_current_cyl(drive, track);
        // AL = number of sectors read (same value as passed)
        SET_AH(0x00); // success
        CLEAR_CF();   // success
        return;
        }
      else {  // if (ah == 0x04)
        // Verify Diskette Sectors

        // ??? should track be new val from return_status[3] ?
        set_diskette_current_cyl(drive, track);
        // AL = number of sectors verified (same value as passed)
        CLEAR_CF();   // success
        SET_AH(0x00); // success
        return;
        }


    case 0x05: // format diskette track
printf("floppy f05\n");

      num_sectors = GET_AL();
      track       = GET_CH();
      head        = GET_DH();
      drive       = GET_DL();

      if (drive > 1) {
        SET_AH(1);
        set_diskette_ret_status(1);
        SET_CF(); // error occurred
	}
      drive_type = inb_cmos(0x10);
      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      if (drive_type == 0) {
        SET_AH(0x80); // drive not responding
        set_diskette_ret_status(0x80);
        SET_CF(); // error occurred
        return;
        }

      /* nop */
      SET_AH(0);
      set_diskette_ret_status(0);
      set_diskette_current_cyl(drive, track);
      CLEAR_CF(); // successful
      return;


    case 0x08: // read diskette drive parameters
printf("floppy f08\n");
      drive = GET_DL();

      if (drive>1) {
        AX = 0;
        BX = 0;
        CX = 0;
        DX = 0;
        //ES = 0; // ???
        SET_DL(num_floppies);
        //set_diskette_ret_status(AH=1);
        SET_CF();
        return;
        }

      drive_type = inb_cmos(0x10);
      num_floppies = 0;
      if (drive_type & 0xf0)
        num_floppies++;
      if (drive_type & 0x0f)
        num_floppies++;

      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;


      SET_BH(0);
      SET_BL(drive_type);
      SET_AH(0);
      SET_AL(0);
      SET_DL(num_floppies);

      switch (drive_type) {
        case 0: // none
          CX = 0;
          SET_DH(0); // max head #
          break;

        case 1: // 360KB, 5.25"
          CX = 0x2709; // 40 tracks, 9 sectors
          SET_DH(1); // max head #
          break;

        case 2: // 1.2MB, 5.25"
          CX = 0x4f0f; // 80 tracks, 15 sectors
          SET_DH(1); // max head #
          break;

        case 3: // 720KB, 3.5"
          CX = 0x4f09; // 80 tracks, 9 sectors
          SET_DH(1); // max head #
          break;

        case 4: // 1.44MB, 3.5"
          CX = 0x4f12; // 80 tracks, 18 sectors
          SET_DH(1); // max head #
          break;

        case 5: // 2.88MB, 3.5"
          CX = 0x4f24; // 80 tracks, 36 sectors
          SET_DH(1); // max head #
          break;

        default: // ?
          panic("floppy: int13: bad floppy type\n");
        }

      /* set es & di to point to 11 byte diskette param table */
      DI = read_word(0x0000, 0x0078);
      ES = read_word(0x0000, 0x007a);
      CLEAR_CF(); // success
      /* disk status not changed upon success */
      return;


    case 0x15: // read diskette drive type
printf("floppy f15\n");
      drive = GET_DL();
      if (drive > 1) {
        SET_AH(0); // only 2 drives supported
        // set_diskette_ret_status here ???
        SET_CF();
        return;
        }
      drive_type = inb_cmos(0x10);

      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      CLEAR_CF(); // successful, not present
      if (drive_type==0) {
        SET_AH(0); // drive not present
        }
      else {
        SET_AH(1); // drive present, does not support change line
        }
      return;

    case 0x16: // get diskette change line status
printf("floppy f16\n");
      drive = GET_DL();
      if (drive > 1) {
        SET_AH(0x01); // invalid drive
        set_diskette_ret_status(0x01);
        SET_CF();
        return;
        }

      SET_AH(0x06); // change line not supported
      set_diskette_ret_status(0x06);
      SET_CF();
      return;

    case 0x17: // set diskette type for format(old)
printf("floppy f17\n");
      /* not used for 1.44M floppies */
      SET_AH(0x01); // not supported
      set_diskette_ret_status(1); /* not supported */
      SET_CF();
      return;

    case 0x18: // set diskette type for format(new)
printf("floppy f18\n");
      SET_AH(0x01); // do later
      set_diskette_ret_status(1);
      SET_CF();
      return;

    default:
      if ( (ah==0x20) || ((ah>=0x41) && (ah<=0x49)) || (ah==0x4e) ) {
        SET_AH(0x01); // ???
        set_diskette_ret_status(1);
        SET_CF();
        printf("floppy: int13: 0x%02x\n", ah);
        return;
        }
      panic("int13_diskette: AH=%02x\n", ah);
    }
}
#else  // #if BX_SUPPORT_FLOPPY
  void
int13_diskette_function(DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, ES, FLAGS;
{
  Bit8u  val8;

  switch ( GET_AH() ) {

    case 0x01: // Read Diskette Status
      CLEAR_CF();
      val8 = read_byte(0x0000, 0x0441);
      SET_AH(val8);
      if (val8) {
        SET_CF();
        }
      return;

    default:
      SET_CF();
      write_byte(0x0000, 0x0441, 0x01);
      SET_AH(0x01);
    }
}
#endif  // #if BX_SUPPORT_FLOPPY

  void
set_disk_ret_status(val)
  Bit8u val;
{
  write_byte(0x0040, 0x0074, val);
}

 void
set_diskette_ret_status(value)
  Bit8u value;
{
  write_byte(0x0040, 0x0041, value);
}

  void
set_diskette_current_cyl(drive, cyl)
  Bit8u drive;
  Bit8u cyl;
{
  if (drive > 1)
    panic("set_diskette_current_cyl(): drive > 1\n");
  write_byte(0x0040, 0x0094+drive, cyl);
}

  void
determine_floppy_media(drive)
  Bit16u drive;
{
#if 0
  Bit8u  val8, DOR, ctrl_info;

  ctrl_info = read_byte(0x0040, 0x008F);
  if (drive==1)
    ctrl_info >>= 4;
  else
    ctrl_info &= 0x0f;

#if 0
  if (drive == 0) {
    DOR = 0x1c; // DOR: drive0 motor on, DMA&int enabled, normal op, drive select 0
    }
  else {
    DOR = 0x2d; // DOR: drive1 motor on, DMA&int enabled, normal op, drive select 1
    }
#endif

  if ( (ctrl_info & 0x04) != 0x04 ) {
    // Drive not determined means no drive exists, done.
    return;
    }

#if 0
  // check Main Status Register for readiness
  val8 = inb(0x03f4) & 0x80; // Main Status Register
  if (val8 != 0x80)
    panic("d_f_m: MRQ bit not set\n");

  // change line

  // existing BDA values

  // turn on drive motor
  outb(0x03f2, DOR); // Digital Output Register
  //
#endif
  panic("d_f_m: OK so far\n");
#endif
}



  void
get_hd_geometry(drive, hd_cylinders, hd_heads, hd_sectors)
  Bit8u drive;
  Bit16u *hd_cylinders;
  Bit8u  *hd_heads;
  Bit8u  *hd_sectors;
{
  Bit8u hd_type;
  Bit16u ss;
  Bit16u cylinders;
  Bit8u iobase;

  ss = get_SS();
  if (drive == 0x80) {
    hd_type = inb_cmos(0x12) & 0xf0;
    if (hd_type != 0xf0)
      panic("HD0 cmos reg 12h not type F\n");
    hd_type = inb_cmos(0x19); // HD0: extended type
    if (hd_type != 47)
      panic("HD0 cmos reg 19h not user definable type 47\n");
    iobase = 0x1b;
  } else {
    hd_type = inb_cmos(0x12) & 0x0f;
    if (hd_type != 0x0f)
      panic("HD1 cmos reg 12h not type F\n");
    hd_type = inb_cmos(0x1a); // HD0: extended type
    if (hd_type != 47)
      panic("HD1 cmos reg 1ah not user definable type 47\n");
    iobase = 0x24;
  }

  // cylinders
  cylinders = inb_cmos(iobase) | (inb_cmos(iobase+1) << 8);
  write_word(ss, hd_cylinders, cylinders);

  // heads
  write_byte(ss, hd_heads, inb_cmos(iobase+2));

  // sectors per track
  write_byte(ss, hd_sectors, inb_cmos(iobase+8));
}

  void
int1a_function(regs, ds, iret_addr)
  pusha_regs_t regs; // regs pushed from PUSHA instruction
  Bit16u ds; // previous DS:, DS set to 0x0000 by asm wrapper
  iret_addr_t  iret_addr; // CS,IP,Flags pushed from original INT call
{
  Bit8u val8;

  ASM(#, sti)

  switch (regs.u.r8.ah) {
    case 0: // get current clock count
      ASM(#, cli)
      regs.u.r16.cx = BiosData->ticks_high;
      regs.u.r16.dx = BiosData->ticks_low;
      regs.u.r8.al  = BiosData->midnight_flag;
      BiosData->midnight_flag = 0; // reset flag
      ASM(#, sti)
      // AH already 0
      ClearCF(iret_addr.flags); // OK
      break;

    case 1: // Set Current Clock Count
      ASM(#, cli)
      BiosData->ticks_high = regs.u.r16.cx;
      BiosData->ticks_low  = regs.u.r16.dx;
      BiosData->midnight_flag = 0; // reset flag
      ASM(#, sti)
      regs.u.r8.ah = 0;
      ClearCF(iret_addr.flags); // OK
      break;


    case 2: // Read CMOS Time
      if (rtc_updating()) {
        SetCF(iret_addr.flags);
        break;
        }

      regs.u.r8.dh = inb_cmos(0x00); // Seconds
      regs.u.r8.cl = inb_cmos(0x02); // Minutes
      regs.u.r8.ch = inb_cmos(0x04); // Hours
      regs.u.r8.dl = inb_cmos(0x0b) & 0x01; // Stat Reg B
      regs.u.r8.ah = 0;
      regs.u.r8.al = regs.u.r8.ch;
      ClearCF(iret_addr.flags); // OK
      break;

    case 3: // Set CMOS Time
      // Using a debugger, I notice the following masking/setting
      // of bits in Status Register B, by setting Reg B to
      // a few values and getting its value after INT 1A was called.
      //
      //        try#1       try#2       try#3
      // before 1111 1101   0111 1101   0000 0000
      // after  0110 0010   0110 0010   0000 0010
      //
      // Bit4 in try#1 flipped in hardware (forced low) due to bit7=1
      // My assumption: RegB = ((RegB & 01100000b) | 00000010b)
      if (rtc_updating()) {
        init_rtc();
        // fall through as if an update were not in progress
        }
      outb_cmos(0x00, regs.u.r8.dh); // Seconds
      outb_cmos(0x02, regs.u.r8.cl); // Minutes
      outb_cmos(0x04, regs.u.r8.ch); // Hours
      // Set Daylight Savings time enabled bit to requested value
      val8 = (inb_cmos(0x0b) & 0x60) | 0x02 | (regs.u.r8.dl & 0x01);
      // (reg B already selected)
      outb_cmos(0x0b, val8);
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8; // val last written to Reg B
      ClearCF(iret_addr.flags); // OK
      break;

    case 4: // Read CMOS Date
      regs.u.r8.ah = 0;
      if (rtc_updating()) {
        SetCF(iret_addr.flags);
        break;
        }
      regs.u.r8.cl = inb_cmos(0x09); // Year
      regs.u.r8.dh = inb_cmos(0x08); // Month
      regs.u.r8.dl = inb_cmos(0x07); // Day of Month
      regs.u.r8.ch = inb_cmos(0x32); // Century
      regs.u.r8.al = regs.u.r8.ch;
      ClearCF(iret_addr.flags); // OK
      break;

    case 5: // Set CMOS Date
      // Using a debugger, I notice the following masking/setting
      // of bits in Status Register B, by setting Reg B to
      // a few values and getting its value after INT 1A was called.
      //
      //        try#1       try#2       try#3       try#4
      // before 1111 1101   0111 1101   0000 0010   0000 0000
      // after  0110 1101   0111 1101   0000 0010   0000 0000
      //
      // Bit4 in try#1 flipped in hardware (forced low) due to bit7=1
      // My assumption: RegB = (RegB & 01111111b)
      if (rtc_updating()) {
        init_rtc();
        SetCF(iret_addr.flags);
        break;
        }
      outb_cmos(0x09, regs.u.r8.cl); // Year
      outb_cmos(0x08, regs.u.r8.dh); // Month
      outb_cmos(0x07, regs.u.r8.dl); // Day of Month
      outb_cmos(0x32, regs.u.r8.ch); // Century
      val8 = inb_cmos(0x0b) & 0x7f; // clear halt-clock bit
      outb_cmos(0x0b, val8);
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8; // AL = val last written to Reg B
      ClearCF(iret_addr.flags); // OK
      break;

    case 6: // Set Alarm Time in CMOS
      // Using a debugger, I notice the following masking/setting
      // of bits in Status Register B, by setting Reg B to
      // a few values and getting its value after INT 1A was called.
      //
      //        try#1       try#2       try#3
      // before 1101 1111   0101 1111   0000 0000
      // after  0110 1111   0111 1111   0010 0000
      //
      // Bit4 in try#1 flipped in hardware (forced low) due to bit7=1
      // My assumption: RegB = ((RegB & 01111111b) | 00100000b)
      val8 = inb_cmos(0x0b); // Get Status Reg B
      regs.u.r16.ax = 0;
      if (val8 & 0x20) {
        // Alarm interrupt enabled already
        SetCF(iret_addr.flags); // Error: alarm in use
        break;
        }
      if (rtc_updating()) {
        init_rtc();
        // fall through as if an update were not in progress
        }
      outb_cmos(0x01, regs.u.r8.dh); // Seconds alarm
      outb_cmos(0x03, regs.u.r8.cl); // Minutes alarm
      outb_cmos(0x05, regs.u.r8.ch); // Hours alarm
      outb(0xa1, inb(0xa1) & 0xfe); // enable IRQ 8
      // enable Status Reg B alarm bit, clear halt clock bit
      outb_cmos(0x0b, (val8 & 0x7f) | 0x20);
      ClearCF(iret_addr.flags); // OK
      break;

    case 7: // Turn off Alarm
      // Using a debugger, I notice the following masking/setting
      // of bits in Status Register B, by setting Reg B to
      // a few values and getting its value after INT 1A was called.
      //
      //        try#1       try#2       try#3       try#4
      // before 1111 1101   0111 1101   0010 0000   0010 0010
      // after  0100 0101   0101 0101   0000 0000   0000 0010
      //
      // Bit4 in try#1 flipped in hardware (forced low) due to bit7=1
      // My assumption: RegB = (RegB & 01010111b)
      val8 = inb_cmos(0x0b); // Get Status Reg B
      // clear clock-halt bit, disable alarm bit
      outb_cmos(0x0b, val8 & 0x57); // disable alarm bit
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8; // val last written to Reg B
      ClearCF(iret_addr.flags); // OK
      break;

    default:
      SetCF(iret_addr.flags); // Unsupported
    }
}

  void
int70_function(regs, ds, iret_addr)
  pusha_regs_t regs; // regs pushed from PUSHA instruction
  Bit16u ds; // previous DS:, DS set to 0x0000 by asm wrapper
  iret_addr_t  iret_addr; // CS,IP,Flags pushed from original INT call
{
  // INT 70h: IRQ 8 - CMOS RTC interrupt from periodic or alarm modes
  Bit8u val8;

  val8 = inb_cmos(0x0c); // Status Reg C
  if (val8 == 0) panic("int70: regC 0\n");
  if (val8 & 0x40) panic("int70: periodic request\n");
  if (val8 & 0x20) {
    // Alarm Flag indicates alarm time matches current time
    // call user INT 4Ah alarm handler
#asm
    sti
    pushf
    ;; call_ep [ds:loc]
    CALL_EP( 0x4a << 2 )
    cli
#endasm
    }

#asm
  ;; send EOI to slave & master PICs
  mov  al, #0x20
  out  #0xA0, al ;; slave  PIC EOI
  out  #0x20, al ;; master PIC EOI
#endasm
}



#asm
;------------------------------------------
;- INT74h : PS/2 mouse hardware interrupt -
;------------------------------------------
int74_handler:
  sti
  pusha
  push ds         ;; save DS
  push #0x00 ;; placeholder for status
  push #0x00 ;; placeholder for X
  push #0x00 ;; placeholder for Y
  push #0x00 ;; placeholder for Z
  push #0x00 ;; placeholder for make_far_call boolean
  call _int74_function
  pop  cx      ;; remove make_far_call from stack
  jcxz int74_done

  ;; make far call to EBDA:0022
  push #0x00
  pop ds
  push 0x040E     ;; push 0000:040E (opcodes 0xff, 0x36, 0x0E, 0x04)
  pop ds
  CALL_EP(0x0022) ;; call far routine (call_Ep DS:0022 :opcodes 0xff, 0x1e, 0x22, 0x00)

int74_done:
  cli
  mov  al, #0x20
  ;; send EOI to slave & master PICs
  out  #0xA0, al ;; slave  PIC EOI
  out  #0x20, al ;; master PIC EOI
  add sp, #8     ;; pop status, x, y, z

  pop ds          ;; restore DS
  popa
  iret


;; This will perform an IRET, but will retain value of current CF
;; by altering flags on stack.  Better than RETF #02.
iret_modify_cf:
  jc   carry_set
  push bp
  mov  bp, sp
  and  BYTE [bp + 0x06], #0xfe
  pop  bp
  iret
carry_set:
  push bp
  mov  bp, sp
  or   BYTE [bp + 0x06], #0x01
  pop  bp
  iret


;----------------------
;- INT13h (relocated) -
;----------------------
int13_relocated:
  pushf
  test  dl, #0x80
  jz    int13_floppy

int13_disk:
  ;; pushf already done
  push  es
  pusha
  call  _int13_function
  popa
  pop   es
  popf
  JMPL(iret_modify_cf)

int13_floppy:
  popf
  JMPL(int13_diskette)



;----------------------
;- INT19h (relocated) -
;----------------------
int19_relocated:
  ;; check bit 5 in CMOS reg 0x2d.  load either 0x00 or 0x80 into DL
  ;; in preparation for the intial INT 13h (0=floppy A:, 0x80=C:)
  ;;   0: system boot sequence, first drive C: then A:
  ;;   1: system boot sequence, first drive A: then C:

  mov  al, #0x2d
  out  0x70, al
  in   al, 0x71
  and  al, #0x20
  jz   int19_usedisk

int19_usefloppy:
  mov  dl, #0x00
  jmp  int19_loadsector

int19_usedisk:
  mov  dl, #0x80

int19_loadsector:
  mov  ax, #0x0000
  mov  es, ax         ;; seg = 0000
  mov  bx, #0x7c00    ;; load boot sector into 0000:7c000
  mov  ah, #0x02      ;; function 2, read diskette sector
  mov  al, #0x01      ;; read 1 sector
  mov  ch, #0x00      ;; track 0
  mov  cl, #0x01      ;; sector 1
  mov  dh, #0x00      ;; head 0
  int #0x13
  jc  bootstrap_problem
  JMP_AP(0x0000, 0x7c00)
bootstrap_problem:
  int #0x18 ;; Boot failure
  HALT(__LINE__)

;----------
;- INT18h -
;----------
int18_handler: ;; Boot Failure routing
  HALT(__LINE__)


;----------
;- INT1Ch -
;----------
int1c_handler: ;; User Timer Tick
  iret


;----------------------
;- POST: Floppy Drive -
;----------------------
floppy_drive_post:
  mov  ax, #0x0000
  mov  ds, ax

  mov  al, #0x00
  mov  0x043e, al ;; drive 0 & 1 uncalibrated, no interrupt has occurred

  mov  0x043f, al  ;; diskette motor status: read op, drive0, motors off

  mov  0x0440, al  ;; diskette motor timeout counter: not active
  mov  0x0441, al  ;; diskette controller status return code

  mov  0x0442, al  ;; disk & diskette controller status register 0
  mov  0x0443, al  ;; diskette controller status register 1
  mov  0x0444, al  ;; diskette controller status register 2
  mov  0x0445, al  ;; diskette controller cylinder number
  mov  0x0446, al  ;; diskette controller head number
  mov  0x0447, al  ;; diskette controller sector number
  mov  0x0448, al  ;; diskette controller bytes written

  mov  0x048b, al  ;; diskette configuration data

  ;; -----------------------------------------------------------------
  ;; (048F) diskette controller information
  ;;
  mov  al, #0x10   ;; get CMOS diskette drive type
  out  0x70, AL
  in   AL, 0x71
  mov  ah, al      ;; save byte to AH

look_drive0:
  shr  al, #4      ;; look at top 4 bits for drive 0
  jz   f0_missing  ;; jump if no drive0
  mov  bl, #0x07   ;; drive0 determined, multi-rate, has changed line
  jmp  look_drive1
f0_missing:
  mov  bl, #0x00   ;; no drive0

look_drive1:
  mov  al, ah      ;; restore from AH
  and  al, #0x0f   ;; look at bottom 4 bits for drive 1
  jz   f1_missing  ;; jump if no drive1
  or   bl, #0x70   ;; drive1 determined, multi-rate, has changed line
f1_missing:
                   ;; leave high bits in BL zerod
  mov  0x048f, bl  ;; put new val in BDA (diskette controller information)
  ;; -----------------------------------------------------------------

  mov  al, #0x00
  mov  0x0490, al  ;; diskette 0 media state
  mov  0x0491, al  ;; diskette 1 media state

                   ;; diskette 0,1 operational starting state
                   ;; drive type has not been determined,
                   ;; has no changed detection line
  mov  0x0492, al
  mov  0x0493, al

  mov  0x0494, al  ;; diskette 0 current cylinder
  mov  0x0495, al  ;; diskette 1 current cylinder

  mov  al, #0x02
  out  #0x0a, al   ;; clear DMA-1 channel 2 mask bit

  SET_INT_VECTOR(0x1E, #0xF000, #diskette_param_table)
  SET_INT_VECTOR(0x40, #0xF000, #int13_diskette)
  SET_INT_VECTOR(0x0E, #0xF000, #int0e_handler) ;; IRQ 6

  ret



;--------------------
;- POST: HARD DRIVE -
;--------------------
; relocated here because the primary POST area isnt big enough.
hard_drive_post:
  // IRQ 14 = INT 76h
  // INT 76h calls INT 15h function ax=9100

  mov  al, #0x0a   ; 0000 1010 = reserved, disable IRQ 14
  mov  dx, #0x03f6
  out  dx, al

  mov  ax, #0x0000
  mov  ds, ax
  mov  0x0474, al /* hard disk status of last operation */
  mov  0x0477, al /* hard disk port offset (XT only ???) */
  mov  0x048c, al /* hard disk status register */
  mov  0x048d, al /* hard disk error register */
  mov  0x048e, al /* hard disk task complete flag */
  mov  al, #0x01
  mov  0x0475, al /* hard disk number attached */
  mov  al, #0xc0
  mov  0x0476, al /* hard disk control byte */
  SET_INT_VECTOR(0x13, #0xF000, #int13_handler)
  SET_INT_VECTOR(0x76, #0xF000, #int76_handler)
  ;; INT 41h: hard disk 0 configuration pointer
  ;; INT 46h: hard disk 1 configuration pointer
  SET_INT_VECTOR(0x41, #EBDA_SEG, #0x003D)
  SET_INT_VECTOR(0x46, #EBDA_SEG, #0x004D)

  ;; move disk geometry data from CMOS to EBDA disk parameter table(s)
  mov  al, #0x12
  out  #0x70, al
  in   al, #0x71
  and  al, #0xf0
  cmp  al, #0xf0
  je   post_d0_extended
  HALT(__LINE__)
post_d0_extended:
  mov  al, #0x19
  out  #0x70, al
  in   al, #0x71
  cmp  al, #47  ;; decimal 47 - user definable
  je   post_d0_type47
  HALT(__LINE__)
post_d0_type47:
  ;; CMOS  purpose                  param table offset
  ;; 1b    cylinders low            0
  ;; 1c    cylinders high           1
  ;; 1d    heads                    2
  ;; 1e    write pre-comp low       5
  ;; 1f    write pre-comp high      6
  ;; 20    retries/bad map/heads>8  8
  ;; 21    landing zone low         C
  ;; 22    landing zone high        D
  ;; 23    sectors/track            E

  mov  ax, #EBDA_SEG
  mov  ds, ax

  ;;; Filling EBDA table for hard disk 0.
  mov  al, #0x1f
  out  #0x70, al
  in   al, #0x71
  mov  ah, al
  mov  al, #0x1e
  out  #0x70, al
  in   al, #0x71
  mov   (0x003d + 0x05), ax ;; write precomp word

  mov  al, #0x20
  out  #0x70, al
  in   al, #0x71
  mov   (0x003d + 0x08), al ;; drive control byte

  mov  al, #0x22
  out  #0x70, al
  in   al, #0x71
  mov  ah, al
  mov  al, #0x21
  out  #0x70, al
  in   al, #0x71
  mov   (0x003d + 0x0C), ax ;; landing zone word

  mov  al, #0x1c   ;; get cylinders word in AX
  out  #0x70, al
  in   al, #0x71   ;; high byte
  mov  ah, al
  mov  al, #0x1b
  out  #0x70, al
  in   al, #0x71   ;; low byte
  mov  bx, ax      ;; BX = cylinders

  mov  al, #0x1d
  out  #0x70, al
  in   al, #0x71
  mov  cl, al      ;; CL = heads

  mov  al, #0x23
  out  #0x70, al
  in   al, #0x71
  mov  dl, al      ;; DL = sectors

  cmp  bx, #1024
  jnbe hd0_post_logical_chs ;; if cylinders > 1024, use translated style CHS

hd0_post_physical_chs:
  ;; no logical CHS mapping used, just physical CHS
  ;; use Standard Fixed Disk Parameter Table (FDPT)
  mov   (0x003d + 0x00), bx ;; number of physical cylinders
  mov   (0x003d + 0x02), cl ;; number of physical heads
  mov   (0x003d + 0x0E), dl ;; number of physical sectors
  jmp check_for_hd1

hd0_post_logical_chs:
  ;; complies with Phoenix style Translated Fixed Disk Parameter Table (FDPT)
  mov   (0x003d + 0x09), bx ;; number of physical cylinders
  mov   (0x003d + 0x0b), cl ;; number of physical heads
  mov   (0x003d + 0x04), dl ;; number of physical sectors
  mov   (0x003d + 0x0e), dl ;; number of logical sectors (same)
  mov al, #0xa0
  mov   (0x003d + 0x03), al ;; A0h signature, indicates translated table

  cmp bx, #2048
  jnbe hd0_post_above_2048
  ;; 1024 < c <= 2048 cylinders
  shr bx, #0x01
  shl cl, #0x01
  jmp hd0_post_store_logical

hd0_post_above_2048:
  cmp bx, #4096
  jnbe hd0_post_above_4096
  ;; 2048 < c <= 4096 cylinders
  shr bx, #0x02
  shl cl, #0x02
  jmp hd0_post_store_logical

hd0_post_above_4096:
  cmp bx, #8192
  jnbe hd0_post_above_8192
  ;; 4096 < c <= 8192 cylinders
  shr bx, #0x03
  shl cl, #0x03
  jmp hd0_post_store_logical

hd0_post_above_8192:
  ;; 8192 < c <= 16384 cylinders
  shr bx, #0x04
  shl cl, #0x04

hd0_post_store_logical:
  mov   (0x003d + 0x00), bx ;; number of physical cylinders
  mov   (0x003d + 0x02), cl ;; number of physical heads
  ;; checksum
  mov   cl, #0x0f     ;; repeat count
  mov   si, #0x003d   ;; offset to disk0 FDPT
  mov   al, #0x00     ;; sum
hd0_post_checksum_loop:
  add   al, [si]
  inc   si
  dec   cl
  jnz hd0_post_checksum_loop
  not   al  ;; now take 2s complement
  inc   al
  mov   [si], al
;;; Done filling EBDA table for hard disk 0.


check_for_hd1:
  ;; is there really a second hard disk?  if not, return now
  mov  al, #0x12
  out  #0x70, al
  in   al, #0x71
  and  al, #0x0f
  jnz   post_d1_exists
  ret
post_d1_exists:
  ;; check that the hd type is really 0x0f.
  cmp al, #0x0f
  jz post_d1_extended
  HALT(__LINE__)
post_d1_extended:
  ;; check that the extended type is 47 - user definable
  mov  al, #0x1a
  out  #0x70, al
  in   al, #0x71
  cmp  al, #47  ;; decimal 47 - user definable
  je   post_d1_type47
  HALT(__LINE__)
post_d1_type47:
  ;; Table for disk1.
  ;; CMOS  purpose                  param table offset
  ;; 0x24    cylinders low            0
  ;; 0x25    cylinders high           1
  ;; 0x26    heads                    2
  ;; 0x27    write pre-comp low       5
  ;; 0x28    write pre-comp high      6
  ;; 0x29    heads>8                  8
  ;; 0x2a    landing zone low         C
  ;; 0x2b    landing zone high        D
  ;; 0x2c    sectors/track            E
;;; Fill EBDA table for hard disk 1.
  mov  al, #0x28
  out  #0x70, al
  in   al, #0x71
  mov  ah, al
  mov  al, #0x27
  out  #0x70, al
  in   al, #0x71
  mov   (0x004d + 0x05), ax ;; write precomp word

  mov  al, #0x29
  out  #0x70, al
  in   al, #0x71
  mov   (0x004d + 0x08), al ;; drive control byte

  mov  al, #0x2b
  out  #0x70, al
  in   al, #0x71
  mov  ah, al
  mov  al, #0x2a
  out  #0x70, al
  in   al, #0x71
  mov   (0x004d + 0x0C), ax ;; landing zone word

  mov  al, #0x25   ;; get cylinders word in AX
  out  #0x70, al
  in   al, #0x71   ;; high byte
  mov  ah, al
  mov  al, #0x24
  out  #0x70, al
  in   al, #0x71   ;; low byte
  mov  bx, ax      ;; BX = cylinders

  mov  al, #0x26
  out  #0x70, al
  in   al, #0x71
  mov  cl, al      ;; CL = heads

  mov  al, #0x2c
  out  #0x70, al
  in   al, #0x71
  mov  dl, al      ;; DL = sectors

  cmp  bx, #1024
  jnbe hd1_post_logical_chs ;; if cylinders > 1024, use translated style CHS

hd1_post_physical_chs:
  ;; no logical CHS mapping used, just physical CHS
  ;; use Standard Fixed Disk Parameter Table (FDPT)
  mov   (0x004d + 0x00), bx ;; number of physical cylinders
  mov   (0x004d + 0x02), cl ;; number of physical heads
  mov   (0x004d + 0x0E), dl ;; number of physical sectors
  ret

hd1_post_logical_chs:
  ;; complies with Phoenix style Translated Fixed Disk Parameter Table (FDPT)
  mov   (0x004d + 0x09), bx ;; number of physical cylinders
  mov   (0x004d + 0x0b), cl ;; number of physical heads
  mov   (0x004d + 0x04), dl ;; number of physical sectors
  mov   (0x004d + 0x0e), dl ;; number of logical sectors (same)
  mov al, #0xa0
  mov   (0x004d + 0x03), al ;; A0h signature, indicates translated table

  cmp bx, #2048
  jnbe hd1_post_above_2048
  ;; 1024 < c <= 2048 cylinders
  shr bx, #0x01
  shl cl, #0x01
  jmp hd1_post_store_logical

hd1_post_above_2048:
  cmp bx, #4096
  jnbe hd1_post_above_4096
  ;; 2048 < c <= 4096 cylinders
  shr bx, #0x02
  shl cl, #0x02
  jmp hd1_post_store_logical

hd1_post_above_4096:
  cmp bx, #8192
  jnbe hd1_post_above_8192
  ;; 4096 < c <= 8192 cylinders
  shr bx, #0x03
  shl cl, #0x03
  jmp hd1_post_store_logical

hd1_post_above_8192:
  ;; 8192 < c <= 16384 cylinders
  shr bx, #0x04
  shl cl, #0x04

hd1_post_store_logical:
  mov   (0x004d + 0x00), bx ;; number of physical cylinders
  mov   (0x004d + 0x02), cl ;; number of physical heads
  ;; checksum
  mov   cl, #0x0f     ;; repeat count
  mov   si, #0x004d   ;; offset to disk0 FDPT
  mov   al, #0x00     ;; sum
hd1_post_checksum_loop:
  add   al, [si]
  inc   si
  dec   cl
  jnz hd1_post_checksum_loop
  not   al  ;; now take 2s complement
  inc   al
  mov   [si], al
;;; Done filling EBDA table for hard disk 0.

  ret


BcdToBin:
  ;; in:  AL in BCD format
  ;; out: AL in binary format, AH will always be 0
  ;; trashes BX
  mov  bl, al
  and  bl, #0x0f ;; bl has low digit
  shr  al, #4    ;; al has high digit
  mov  bh, #10
  mul  al, bh    ;; multiply high digit by 10 (result in AX)
  add  al, bl    ;;   then add low digit
  ret

timer_tick_post:
  ;; Setup the Timer Ticks Count (0x46C:dword) and
  ;;   Timer Ticks Roller Flag (0x470:byte)
  ;; The Timer Ticks Count needs to be set according to
  ;; the current CMOS time, as if ticks have been occurring
  ;; at 18.2hz since midnight up to this point.  Calculating
  ;; this is a little complicated.  Here are the factors I gather
  ;; regarding this.  14,318,180 hz was the original clock speed,
  ;; chosen so it could be divided by either 3 to drive the 5Mhz CPU
  ;; at the time, or 4 to drive the CGA video adapter.  The div3
  ;; source was divided again by 4 to feed a 1.193Mhz signal to
  ;; the timer.  With a maximum 16bit timer count, this is again
  ;; divided down by 65536 to 18.2hz.
  ;;
  ;; 14,318,180 Hz clock
  ;;   /3 = 4,772,726 Hz fed to orginal 5Mhz CPU
  ;;   /4 = 1,193,181 Hz fed to timer
  ;;   /65536 (maximum timer count) = 18.20650736 ticks/second
  ;; 1 second = 18.20650736 ticks
  ;; 1 minute = 1092.390442 ticks
  ;; 1 hour   = 65543.42651 ticks
  ;;
  ;; Given the values in the CMOS clock, one could calculate
  ;; the number of ticks by the following:
  ;;   ticks = (BcdToBin(seconds) * 18.206507) +
  ;;           (BcdToBin(minutes) * 1092.3904)
  ;;           (BcdToBin(hours)   * 65543.427)
  ;; To get a little more accuracy, since Im using integer
  ;; arithmatic, I use:
  ;;   ticks = (BcdToBin(seconds) * 18206507) / 1000000 +
  ;;           (BcdToBin(minutes) * 10923904) / 10000 +
  ;;           (BcdToBin(hours)   * 65543427) / 1000

  ;; assuming DS=0000

  ;; get CMOS seconds
  xor  eax, eax ;; clear EAX
  mov  al, #0x00
  out  #0x70, al
  in   al, #0x71 ;; AL has CMOS seconds in BCD
  call BcdToBin  ;; EAX now has seconds in binary
  mov  edx, #18206507
  mul  eax, edx
  mov  ebx, #1000000
  xor  edx, edx
  div  eax, ebx
  mov  ecx, eax  ;; ECX will accumulate total ticks

  ;; get CMOS minutes
  xor  eax, eax ;; clear EAX
  mov  al, #0x02
  out  #0x70, al
  in   al, #0x71 ;; AL has CMOS minutes in BCD
  call BcdToBin  ;; EAX now has minutes in binary
  mov  edx, #10923904
  mul  eax, edx
  mov  ebx, #10000
  xor  edx, edx
  div  eax, ebx
  add  ecx, eax  ;; add to total ticks

  ;; get CMOS hours
  xor  eax, eax ;; clear EAX
  mov  al, #0x04
  out  #0x70, al
  in   al, #0x71 ;; AL has CMOS hours in BCD
  call BcdToBin  ;; EAX now has hours in binary
  mov  edx, #65543427
  mul  eax, edx
  mov  ebx, #1000
  xor  edx, edx
  div  eax, ebx
  add  ecx, eax  ;; add to total ticks

  mov  0x46C, ecx ;; Timer Ticks Count
  xor  al, al
  mov  0x470, al  ;; Timer Ticks Rollover Flag
  ret


int76_handler:
  ;; record completion in BIOS task complete flag
  push  ax
  push  ds
  mov   ax, #0x0040
  mov   ds, ax
  mov   0x008E, #0xff
  mov   al, #0x20
  out   #0xA0, al ;; slave  PIC EOI
  out   #0x20, al ;; master PIC EOI
  pop   ds
  pop   ax
  iret

.org 0xd000

// For documentation of this config structure, look on developer.intel.com and
// search for multiprocessor specification.  Note that when you change anything
// you must update the checksum (a pain!).  It would be better to construct this
// with C structures, or at least fill in the checksum automatically.

#if (BX_SMP_PROCESSORS==1)
  // no structure necessary.
#elif (BX_SMP_PROCESSORS==2)
// define the Intel MP Configuration Structure for 2 processors at
// APIC ID 0,1.  I/O APIC at ID=2.
mp_config_table:
  db 0x50, 0x43, 0x4d, 0x50  ;; "PCMP" signature
  dw (mp_config_end-mp_config_table)  ;; table length
  db 4 ;; spec rev
  db 0xdd ;; checksum
  db 0x42, 0x42, 0x44, 0x43, 0x50, 0x55, 0x20, 0x20 ;; OEM id = "BBDCPU  "
  db 0x30, 0x2e, 0x31, 0x20 ;; vendor id = "0.1         "
  db 0x20, 0x20, 0x20, 0x20 
  db 0x20, 0x20, 0x20, 0x20
  dw 0,0 ;; oem table ptr
  dw 0 ;; oem table size
  dw 3 ;; entry count
  dw 0x0000, 0xfee0 ;; memory mapped address of local APIC
  dw 0 ;; extended table length
  db 0 ;; extended table checksum
  db 0 ;; reserved
mp_config_proc0:
  db 0 ;; entry type=processor
  db 0 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 3 ;; cpu flags: enabled
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_proc1:
  db 0 ;; entry type=processor
  db 1 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 1 ;; cpu flags: enabled
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_isa_bus:
  db 1 ;; entry type=bus
  db 0 ;; bus ID
  db 0x49, 0x53, 0x41, 0x20, 0x20, 0x20  ;; bus type="ISA   "
mp_config_ioapic:
  db 2 ;; entry type=I/O APIC
  db 2 ;; apic id=2. linux will set.
  db 0x11 ;; I/O APIC version number
  db 1 ;; flags=1=enabled
  dw 0x0000, 0xfec0 ;; memory mapped address of I/O APIC
mp_config_irqs:
  db 3 ;; entry type=I/O interrupt
  db 0 ;; interrupt type=vectored interrupt
  db 0,0 ;; flags po=0, el=0 (linux uses as default)
  db 0 ;; source bus ID is ISA
  db 0 ;; source bus IRQ
  db 2 ;; destination I/O APIC ID
  db 0 ;; destination I/O APIC interrrupt in
  ;; repeat pattern for interrupts 0-15
  db 3,0,0,0,0,1,2,1
  db 3,0,0,0,0,2,2,2
  db 3,0,0,0,0,3,2,3
  db 3,0,0,0,0,4,2,4
  db 3,0,0,0,0,5,2,5
  db 3,0,0,0,0,6,2,6
  db 3,0,0,0,0,7,2,7
  db 3,0,0,0,0,8,2,8
  db 3,0,0,0,0,9,2,9
  db 3,0,0,0,0,10,2,10
  db 3,0,0,0,0,11,2,11
  db 3,0,0,0,0,12,2,12
  db 3,0,0,0,0,13,2,13
  db 3,0,0,0,0,14,2,14
  db 3,0,0,0,0,15,2,15
#elif (BX_SMP_PROCESSORS==4)
// define the Intel MP Configuration Structure for 4 processors at
// APIC ID 0,1,2,3.  I/O APIC at ID=4.
mp_config_table:
  db 0x50, 0x43, 0x4d, 0x50  ;; "PCMP" signature
  dw (mp_config_end-mp_config_table)  ;; table length
  db 4 ;; spec rev
  db 0x55 ;; checksum
  db 0x42, 0x42, 0x44, 0x43, 0x50, 0x55, 0x20, 0x20 ;; OEM id = "BBDCPU  "
  db 0x30, 0x2e, 0x31, 0x20 ;; vendor id = "0.1         "
  db 0x20, 0x20, 0x20, 0x20 
  db 0x20, 0x20, 0x20, 0x20
  dw 0,0 ;; oem table ptr
  dw 0 ;; oem table size
  dw 5 ;; entry count
  dw 0x0000, 0xfee0 ;; memory mapped address of local APIC
  dw 0 ;; extended table length
  db 0 ;; extended table checksum
  db 0 ;; reserved
mp_config_proc0:
  db 0 ;; entry type=processor
  db 0 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 1 ;; cpu flags: enabled
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_proc1:
  db 0 ;; entry type=processor
  db 1 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 1 ;; cpu flags: enabled
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_proc2:
  db 0 ;; entry type=processor
  db 2 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 3 ;; cpu flags: enabled, bootstrap processor
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_proc3:
  db 0 ;; entry type=processor
  db 3 ;; local APIC id
  db 0x11 ;; local APIC version number
  db 1 ;; cpu flags: enabled
  db 0,6,0,0 ;; cpu signature
  dw 0x201,0 ;; feature flags
  dw 0,0 ;; reserved
  dw 0,0 ;; reserved
mp_config_isa_bus:
  db 1 ;; entry type=bus
  db 0 ;; bus ID
  db 0x49, 0x53, 0x41, 0x20, 0x20, 0x20  ;; bus type="ISA   "
mp_config_ioapic:
  db 2 ;; entry type=I/O APIC
  db 4 ;; apic id=2. linux will set.
  db 0x11 ;; I/O APIC version number
  db 1 ;; flags=1=enabled
  dw 0x0000, 0xfec0 ;; memory mapped address of I/O APIC
mp_config_irqs:
  db 3 ;; entry type=I/O interrupt
  db 0 ;; interrupt type=vectored interrupt
  db 0,0 ;; flags po=0, el=0 (linux uses as default)
  db 0 ;; source bus ID is ISA
  db 0 ;; source bus IRQ
  db 4 ;; destination I/O APIC ID
  db 0 ;; destination I/O APIC interrrupt in
  ;; repeat pattern for interrupts 0-15
  db 3,0,0,0,0,1,4,1
  db 3,0,0,0,0,2,4,2
  db 3,0,0,0,0,3,4,3
  db 3,0,0,0,0,4,4,4
  db 3,0,0,0,0,5,4,5
  db 3,0,0,0,0,6,4,6
  db 3,0,0,0,0,7,4,7
  db 3,0,0,0,0,8,4,8
  db 3,0,0,0,0,9,4,9
  db 3,0,0,0,0,10,4,10
  db 3,0,0,0,0,11,4,11
  db 3,0,0,0,0,12,4,12
  db 3,0,0,0,0,13,4,13
  db 3,0,0,0,0,14,4,14
  db 3,0,0,0,0,15,4,15
#else
#  error Sorry, rombios only has configurations for 1, 2, or 4 processors.
#endif  // if (BX_SMP_PROCESSORS==...)

mp_config_end:   // this label used to find length of mp structure
  db 0

#if (BX_SMP_PROCESSORS>1)
.align 16
mp_floating_pointer_structure:
db 0x5f, 0x4d, 0x50, 0x5f   ; "_MP_" signature
dw mp_config_table, 0xf ;; pointer to MP configuration table
db 1     ;; length of this struct in 16-bit byte chunks
db 4     ;; MP spec revision
db 0xc1  ;; checksum
db 0     ;; MP feature byte 1.  value 0 means look at the config table
db 0,0,0,0     ;; MP feature bytes 2-5.
#endif

;; for 'C' strings and other data, insert them here with
;; a the following hack:
;; DATA_SEG_DEFS_HERE


;--------
;- POST -
;--------
.org 0xe05b ; POST Entry Point
post:

  ;; Examine CMOS shutdown status.
  ;;    0 = normal startup
  mov AL, #0x0f
  out 0x70, AL
  in  AL, 0x71
  cmp AL, #0x00
  jz normal_post
  HALT(__LINE__)
  ;
  mov AL, #0x0f
  out 0x70, AL          ; select CMOS register Fh
  mov AL, #0x00
  out 0x71, AL          ; set shutdown action to normal
  ;
  ;#if 0
  ;  0xb0, 0x20,       /* mov al, #0x20 */
  ;  0xe6, 0x20,       /* out 0x20, al    ;send EOI to PIC */
  ;#endif
  ;
  pop es
  pop ds
  popa
  iret

normal_post:
  ; case 0: normal startup

  cli
  mov  ax, #0xfffe
  mov  sp, ax
  mov  ax, #0x0000
  mov  ds, ax
  mov  ss, ax

  ;; zero out BIOS data area (40:00..40:ff)
  mov  es, ax
  mov  cx, #0x0080 ;; 128 words
  mov  di, #0x0400
  cld
  rep
    stosw

  ;; set all interrupts to default handler
  mov  bx, #0x0000    ;; offset index
  mov  cx, #0x0100    ;; counter (256 interrupts)
  mov  ax, #dummy_iret_handler
  mov  dx, #0xF000

post_default_ints:
  mov  [bx], ax
  inc  bx
  inc  bx
  mov  [bx], dx
  inc  bx
  inc  bx
  loop post_default_ints

  ;; base memory in K 40:13 (word)
  mov  ax, #BASE_MEM_IN_K
  mov  0x0413, ax


  ;; Manufacturing Test 40:12
  ;;   zerod out above

  ;; Warm Boot Flag 0040:0072
  ;;   value of 1234h = skip memory checks
  ;;   zerod out above


  ;; Printer Services vector
  SET_INT_VECTOR(0x17, #0xF000, #int17_handler)

  ;; Bootstrap failure vector
  SET_INT_VECTOR(0x18, #0xF000, #int18_handler)

  ;; Bootstrap Loader vector
  SET_INT_VECTOR(0x19, #0xF000, #int19_handler)

  ;; User Timer Tick vector
  SET_INT_VECTOR(0x1c, #0xF000, #int1c_handler)

  ;; Memory Size Check vector
  SET_INT_VECTOR(0x12, #0xF000, #int12_handler)

  ;; Equipment Configuration Check vector
  SET_INT_VECTOR(0x11, #0xF000, #int11_handler)

  ;; System Services
  SET_INT_VECTOR(0x15, #0xF000, #int15_handler)
  mov ax, #0x0000       ; mov EBDA seg into 40E
  mov ds, ax
  mov 0x40E, #EBDA_SEG

  ;; PIT setup
  SET_INT_VECTOR(0x08, #0xF000, #int08_handler)
  ;; int 1C already points at dummy_iret_handler (above)
  mov al, #0x34 ; timer0: binary count, 16bit count, mode 2
  out 0x43, al
  mov al, #0x00 ; maximum count of 0000H = 18.2Hz
  out 0x40, al
  out 0x40, al

  ;; Keyboard
  SET_INT_VECTOR(0x09, #0xF000, #int09_handler)
  SET_INT_VECTOR(0x16, #0xF000, #int16_handler)

  mov ax, #0x0000
  mov ds, ax
  mov  0x0417, al /* keyboard shift flags, set 1 */
  mov  0x0418, al /* keyboard shift flags, set 2 */
  mov  0x0419, al /* keyboard alt-numpad work area */
  mov  0x0471, al /* keyboard ctrl-break flag */
  mov  0x0496, al /* keyboard status flags 3 */
  mov  0x0497, al /* keyboard status flags 4 */


  /* keyboard head of buffer pointer */
  mov  bx, #0x001E
  mov  0x041A, bx

  /* keyboard end of buffer pointer */
  mov  0x041C, bx

  /* keyboard buffer */
// for (i=0; i<16; i++)
//    bx_mem.access_physical(0x41E + i*2, 2, BX_WRITE, &zero16);


  /* keyboard pointer to start of buffer */
  mov  bx, #0x001E
  mov  0x0480, bx

  /* keyboard pointer to end of buffer */
  mov  bx, #0x003E
  mov  0x0482, bx

  /* (mch) Keyboard self-test */
  mov  al, #0xaa
  out  0x64, al
  in   al, 0x60
  cmp  al, #0x55
  je   keyboard_ok
  call _keyboard_panic
 keyboard_ok:


#if BX_USE_PS2_MOUSE
  in   al, 0xa1
  and  al, #0xef
  out  0xa1, al

  // hack to tell CMOS & BIOS data area that we have a mouse

  mov  al, #0x14
  out  0x70, al
  in   al, 0x71
  or   al, #0x04
  out  0x71, al
#endif

  ;; mov CMOS Equipment Byte to BDA Equipment Word
  mov  ax, 0x0410
  mov  al, #0x14
  out  0x70, al
  in   al, 0x71
  mov  0x0410, ax


  ;; DMA
  ;; nothing for now

  ;; Parallel setup
  SET_INT_VECTOR(0x0F, #0xF000, #dummy_iret_handler)
  mov ax, #0x0000
  mov ds, ax
  mov 0x408, AX ; Parallel I/O address, port 1
  mov 0x40A, AX ; Parallel I/O address, port 2
  mov 0x40C, AX ; Parallel I/O address, port 3
  mov 0x478, AL ; Parallel printer 1 timeout
  mov 0x479, AL ; Parallel printer 2 timerout
  mov 0x47A, AL ; Parallel printer 3 timerout
  mov 0x47B, AL ; Parallel printer 4 timerout

  ;; Serial setup
  SET_INT_VECTOR(0x0C, #0xF000, #dummy_iret_handler)
  SET_INT_VECTOR(0x14, #0xF000, #int14_handler)
  ;; assuming AX==0, DS==0 from above
  mov 0x400, AX   ; Serial I/O address, port 1
  mov 0x402, AX   ; Serial I/O address, port 2
  mov 0x404, AX   ; Serial I/O address, port 3
  mov 0x406, AX   ; Serial I/O address, port 4
  mov 0x47C, AL   ; Serial 1 timeout
  mov 0x47D, AL   ; Serial 2 timeout
  mov 0x47E, AL   ; Serial 3 timeout
  mov 0x47F, AL   ; Serial 4 timeout
  mov AX, 0x410   ; Equipment word bits 9..11 determing # serial ports
  and AX, #0xf1ff ; clear bits 9..11 for now (zero ports)
  mov 0x410, AX

  ;; CMOS RTC
  SET_INT_VECTOR(0x1A, #0xF000, #int1a_handler)
  SET_INT_VECTOR(0x4A, #0xF000, #dummy_iret_handler)
  SET_INT_VECTOR(0x70, #0xF000, #int70_handler)
  ;; BIOS DATA AREA 0x4CE ???
  call timer_tick_post

  ;; PS/2 mouse setup
  SET_INT_VECTOR(0x74, #0xF000, #int74_handler)

  ;; Video setup
  SET_INT_VECTOR(0x10, #0xF000, #int10_handler)

  ;; VGA: If video BIOS exists, call video ROM
  ;;   initialization routine.
  mov  bx, #0xc000
  mov  ds, bx
  mov  ax, 0x0000
  mov  bx, #0x0000
  mov  ds, bx
  cmp  ax, #0xAA55
  jne  nocall
  CALL_AP(0xc000,0x0003)
nocall:

  ;; PIC
  mov  al, #0x00
  out  0x21, AL ;master pic: all IRQs unmasked
  out  0xA1, AL ;slave  pic: all IRQs unmasked

  ;;
  ;; Hard Drive setup
  ;;
  call hard_drive_post

  ;;
  ;; Floppy setup
  ;;
  call floppy_drive_post

  JMP_EP(0x0064) ; INT 19h location


.org 0xe2c3 ; NMI Handler Entry Point
  HALT(__LINE__)

;-------------------------------------------
;- INT 13h Fixed Disk Services Entry Point -
;-------------------------------------------
.org 0xe3fe ; INT 13h Fixed Disk Services Entry Point
int13_handler:
  JMPL(int13_relocated)
.org 0xe401 ; Fixed Disk Parameter Table


;----------
;- INT19h -
;----------
.org 0xe6f2 ; INT 19h Boot Load Service Entry Point
int19_handler:
  JMPL(int19_relocated)

;-------------------------------------------
;- System BIOS Configuration Data Table
;-------------------------------------------
.org BIOS_CONFIG_TABLE
db 0x08
db 0x00
db SYS_MODEL_ID
db SYS_SUBMODEL_ID
db BIOS_REVISION
// b7: 1=DMA channel 3 used by hard disk
// b6: 1=2 interrupt controllers present
// b5: 1=RTC present
// b4: 1=BIOS calls int 15h, 4Fh every key
// b3: 1=wait for extern event supported
// b2: 1=extended BIOS data area used
// b1: 0=AT or ESDI bus, 1=MicroChannel
// b0: (unused)
db (0 << 7) | \
   (1 << 6) | \
   (0 << 5) | \
   (BX_CALL_INT15_4F << 4) | \
   (0 << 3) | \
   (BX_USE_EBDA << 2) | \
   (0 << 1) | \
   (0 << 0)
// b6: 1=int16h, function 9 supported
db 0x00
db 0x00
db 0x00
db 0x00



.org 0xe729 ; Baud Rate Generator Table

;----------
;- INT14h -
;----------
.org 0xe739 ; INT 14h Serial Communications Service Entry Point
int14_handler:
  ;; ??? should post message here
  iret


;----------------------------------------
;- INT 16h Keyboard Service Entry Point -
;----------------------------------------
.org 0xe82e
int16_handler:

  push  ds
  pushf
  pusha

  cmp   ah, #0x00
  je    int16_F00

  mov  bx, #0xf000
  mov  ds, bx
  call _int16_function
  popa
  popf
  pop  ds
  jz   int16_zero_set

int16_zero_clear:
  push bp
  mov  bp, sp
  SEG SS !!! and  BYTE [bp + 0x06], #0xbf
  pop  bp
  iret

int16_zero_set:
  push bp
  mov  bp, sp
  SEG SS !!! or   BYTE [bp + 0x06], #0x40
  pop  bp
  iret

int16_F00:
  mov  bx, #0x0040
  mov  ds, bx

int16_wait_for_key:
  cli
  mov  bx, 0x001a
  cmp  bx, 0x001c
  jne  int16_key_found
  sti
  nop
#if 0
                           /* no key yet, call int 15h, function AX=9002 */
  0x50,                    /* push AX */
  0xb8, 0x02, 0x90,        /* mov AX, #0x9002 */
  0xcd, 0x15,              /* int 15h */
  0x58,                    /* pop  AX */
  0xeb, 0xea,              /* jmp   WAIT_FOR_KEY */
#endif
  jmp  int16_wait_for_key

int16_key_found:
  mov  bx, #0xf000
  mov  ds, bx
  call _int16_function
  popa
  popf
  pop  ds
#if 0
                           /* notify int16 complete w/ int 15h, function AX=9102 */
  0x50,                    /* push AX */
  0xb8, 0x02, 0x91,        /* mov AX, #0x9102 */
  0xcd, 0x15,              /* int 15h */
  0x58,                    /* pop  AX */
#endif
  iret



;-------------------------------------------------
;- INT09h : Keyboard Harware Service Entry Point -
;-------------------------------------------------
.org 0xe987
int09_handler:
  cli
  push ax

  mov al, #0xAD      ;;disable keyboard
  out #0x64, al

  sti

  ;; see if there is really a key to read from the controller
  in   al, #0x64
  test al, #0x01
  jz   int09_done    ;; nope, skip processing

  in  al, #0x60             ;;read key from keyboard controller
  test al, #0x80            ;;look for key release
  jnz  int09_process_key    ;; dont pass releases to intercept?

#ifdef BX_CALL_INT15_4F
  mov  ah, #0x4f     ;; allow for keyboard intercept
  stc
  int  #0x15
  jnc  int09_done
#endif


int09_process_key:
  push  ds
  pusha
  mov   bx, #0xf000
  mov   ds, bx
  call  _int09_function
  popa
  pop   ds

int09_done:
  cli
  ;; look at PIC in-service-register to see if EOI required
  mov  al, #0x0B
  out  #0x20, al
  in   al, #0x20
  and  al, #0x02     ;; IRQ 1 in service
  jz   int09_finish
  mov  al, #0x20     ;; send EOI to master PIC
  out  #0x20, al

int09_finish:
  mov al, #0xAE      ;;enable keyboard
  out #0x64, al
  pop ax
  iret




;----------------------------------------
;- INT 13h Diskette Service Entry Point -
;----------------------------------------
.org 0xec59
int13_diskette:
  pushf
  push  es
  pusha
  call  _int13_diskette_function
  popa
  pop   es
  popf
  JMPL(iret_modify_cf)

#if 0
  pushf
  cmp  ah, #0x01
  je   i13d_f01

  ;; pushf already done
  push  es
  pusha
  call  _int13_diskette_function
  popa
  pop   es
  popf
  JMPL(iret_modify_cf)

i13d_f01:
  popf
  push  ds
  push  bx
  mov   bx, #0x0000
  mov   ds, bx
  mov   ah, 0x0441
  pop   bx
  pop   ds
  clc
  ;; ??? dont know if this service changes the return status
  JMPL(iret_modify_cf)
#endif



;---------------------------------------------
;- INT 0Eh Diskette Hardware ISR Entry Point -
;---------------------------------------------
.org 0xef57 ; INT 0Eh Diskette Hardware ISR Entry Point
int0e_handler:
  push ax
  push ds
  mov  ax, #0x0000 ;; segment 0000
  mov  ds, ax
  mov  al, #0x20
  out  0x20, al  ;; send EOI to PIC
  mov  al, 0x043e
  or   al, #0x80 ;; diskette interrupt has occurred
  mov  0x043e, al
  pop  ds
  pop  ax
  iret


.org 0xefc7 ; Diskette Controller Parameter Table
diskette_param_table:
;;  Since no provisions are made for multiple drive types, most
;;  values in this table are ignored.  I set parameters for 1.44M
;;  floppy here
db  0xAF
db  0x02 ;; head load time 0000001, DMA used
db  0x25
db  0x02
db    18
db  0x1B
db  0xFF
db  0x6C
db  0xF6
db  0x0F
db  0x01 ;; most systems default to 8


;----------------------------------------
;- INT17h : Printer Service Entry Point -
;----------------------------------------
.org 0xefd2
int17_handler:
  iret ;; for now...

.org 0xf045 ; INT 10 Functions 0-Fh Entry Point
  HALT(__LINE__)

;----------
;- INT10h -
;----------
.org 0xf065 ; INT 10h Video Support Service Entry Point
int10_handler:
  ;; dont do anything, since the VGA BIOS handles int10h requests
  iret

.org 0xf0a4 ; MDA/CGA Video Parameter Table (INT 1Dh)

;----------
;- INT12h -
;----------
.org 0xf841 ; INT 12h Memory Size Service Entry Point
; ??? different for Pentium (machine check)?
int12_handler:
  push ds
  mov  ax, #0x0040
  mov  ds, ax
  mov  ax, 0x0013
  pop  ds
  iret

;----------
;- INT11h -
;----------
.org 0xf84d ; INT 11h Equipment List Service Entry Point
int11_handler:
  push ds
  mov  ax, #0x0040
  mov  ds, ax
  mov  ax, 0x0010
  pop  ds
  iret

;----------
;- INT15h -
;----------
.org 0xf859 ; INT 15h System Services Entry Point
int15_handler:
  pushf
  push  ds
  push  es
  pusha
  call _int15_function
  popa
  pop   es
  pop   ds
  popf
  JMPL(iret_modify_cf)

;; Protected mode IDT descriptor
;;
;; I just make the limit 0, so the machine will shutdown
;; if an exception occurs during protected mode memory
;; transfers.
;;
;; Set base to f0000 to correspond to beginning of BIOS,
;; in case I actually define an IDT later
;; Set limit to 0

pmode_IDT_info:
dw 0x0000  ;; limit 15:00
dw 0x0000  ;; base  15:00
db 0x0f    ;; base  23:16

;; Real mode IDT descriptor
;;
;; Set to typical real-mode values.
;; base  = 000000
;; limit =   03ff

rmode_IDT_info:
dw 0x03ff  ;; limit 15:00
dw 0x0000  ;; base  15:00
db 0x00    ;; base  23:16

.org 0xfa6e ; Character Font for 320x200 & 640x200 Graphics (lower 128 characters)


;----------
;- INT1Ah -
;----------
.org 0xfe6e ; INT 1Ah Time-of-day Service Entry Point
int1a_handler:
  push ds
  pusha
  mov  ax, #0x0000
  mov  ds, ax
  call _int1a_function
  popa
  pop  ds
  iret

;;
;; int70h: IRQ8 - CMOS RTC
;;
int70_handler:
  push ds
  pusha
  mov  ax, #0x0000
  mov  ds, ax
  call _int70_function
  popa
  pop  ds
  iret

;---------
;- INT08 -
;---------
.org 0xfea5 ; INT 08h System Timer ISR Entry Point
int08_handler:
  sti
  push eax
  push ds
  mov ax, #0x0000
  mov ds, ax
  mov eax, 0x046c ;; get ticks dword
  inc eax

  ;; compare eax to one days worth of timer ticks at 18.2 hz
  cmp eax, #0x001800B0
  jb  int08_store_ticks
  ;; there has been a midnight rollover at this point
  xor eax, eax    ;; zero out counter
  inc BYTE 0x0470 ;; increment rollover flag

int08_store_ticks:
  mov 0x046c, eax ;; store new ticks dword
  ;; chain to user timer tick INT #0x1c
  pushf
  ;; call_ep [ds:loc]
  CALL_EP( 0x1c << 2 )
  cli
  mov al, #0x20
  out 0x20, al  ; send EOI to PIC
  pop ds
  pop eax
  iret

.org 0xfef3 ; Initial Interrupt Vector Offsets Loaded by POST

;------------------------------------------------
;- IRET Instruction for Dummy Interrupt Handler -
;------------------------------------------------
.org 0xff53 ; IRET Instruction for Dummy Interrupt Handler
dummy_iret_handler:
  iret

.org 0xff54 ; INT 05h Print Screen Service Entry Point
  HALT(__LINE__)

; .org 0xff00
; .ascii "(c) 1994-2000 Kevin P. Lawton"

.org 0xfff0 ; Power-up Entry Point
  JMPL(post)

.org 0xfff5 ; ASCII Date ROM was built - 8 characters in MM/DD/YY
.ascii "06/23/99"

.org 0xfffe ; System Model ID
db SYS_MODEL_ID
db 0x00   ; filler
;; BLOCK_STRINGS_BEGIN

#endasm
