//  APM BIOS support for the Bochs BIOS
//  Copyright (C) 2004 Fabrice Bellard
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

#if defined(APM_REAL)
#define APMSYM(s) apmreal_ ## s
#elif defined(APM_PROT16)
#define APMSYM(s) apm16_ ## s
#elif defined(APM_PROT32)
#define APMSYM(s) apm32_ ## s
#else
#error unsupported APM mode
#endif

#if DEBUG_APM
APMSYM(put_str):      
  push eax
  push ebx
  push edx
  mov ebx, eax
  mov dx, #INFO_PORT
APMSYM(put_str1):
  SEG CS
  mov al, byte ptr [bx]
  cmp al, #0
  je APMSYM(put_str2)
  outb dx, al
  inc ebx
  jmp APMSYM(put_str1)
APMSYM(put_str2):
  pop edx
  pop ebx
  pop eax
  ret

; print the hex number in eax
APMSYM(put_num):      
  push eax
  push ebx
  push ecx
  push edx
  mov ecx, eax
  mov bx, #8
  mov dx, #INFO_PORT
APMSYM(put_num1):
  mov eax, ecx
  shr eax, #28
  add al, #0x30
  cmp al, #0x39
  jbe APMSYM(put_num2)
  add al, #0x27
APMSYM(put_num2):
  outb dx, al
  shl ecx, #4
  dec bx
  jne APMSYM(put_num1)
  mov al, #0x0a
  outb dx, al
  pop edx
  pop ecx
  pop ebx
  pop eax
  ret

APMSYM(msg_ax):
  .ascii "APM: EAX="
  db 0
#endif

#if defined(APM_PROT32)
_apm32_entry:
#endif
#if defined(APM_PROT16)
_apm16_entry:
#endif
  pushf
#if defined(APM_REAL)
_apmreal_entry:
#endif

#if DEBUG_APM
  push eax
  mov eax, #APMSYM(msg_ax)
  call APMSYM(put_str)
  pop eax
  call APMSYM(put_num)
#endif

#if defined(APM_REAL)
;-----------------
; APM installation check
APMSYM(00):
  cmp al, #0x00
  jne APMSYM(01)
  mov ah, #1 // APM major version
  mov al, #2 // APM minor version
  mov bh, #0x50 // 'P'
  mov bl, #0x4d // 'M'
  // bit 0 : 16 bit interface supported
  // bit 1 : 32 bit interface supported
  mov cx, #0x2 
  jmp APMSYM(ok)
  
;-----------------
; APM real mode interface connect
APMSYM(01):
  cmp al, #0x01
  jne APMSYM(03)
  jmp APMSYM(ok)

;-----------------
; APM 32 bit protected mode interface connect
APMSYM(03):
  cmp al, #0x03
  jne APMSYM(04)
  mov ax, #0xf000 // 32 bit code segment base
  mov ebx, #_apm32_entry
  mov cx, #0xf000 // 16 bit code segment base
  // 32 bit code segment size (low 16 bits)
  // 16 bit code segment size (high 16 bits)
  mov esi, #0xfff0fff0
  mov dx, #0xf000 // data segment address
  mov di, #0xfff0 // data segment length
  jmp APMSYM(ok)

#endif

;-----------------
; APM interface disconnect
APMSYM(04):
  cmp al, #0x04
  jne APMSYM(07)
  jmp APMSYM(ok)

;-----------------
; APM Set Power State
APMSYM(07):
  cmp al, #0x07
  jne APMSYM(0a)
  cmp bx, #1
  jne APMSYM(ok)
  cmp cx, #3
  jne APMSYM(ok)
// send power off event to emulator
  cli
  mov dx, #0x8900
  mov al, #0x53 // 'S'
  out dx, al
  mov al, #0x68 // 'h'
  out dx, al
  mov al, #0x75 // 'u'
  out dx, al
  mov al, #0x74 // 't'
  out dx, al
  mov al, #0x64 // 'd'
  out dx, al
  mov al, #0x6f // 'o'
  out dx, al
  mov al, #0x77 // 'w'
  out dx, al
  mov al, #0x6e // 'n'
  out dx, al

APMSYM(07_1):
  hlt
  jmp APMSYM(07_1)

;-----------------
; Get Power Status
APMSYM(0a):
  cmp al, #0x0a
  jne APMSYM(0b)
  mov bh, #0x01 // on line
  mov bl, #0xff // unknown battery status
  mov ch, #0x80 // no system battery
  mov cl, #0xff // unknown remaining time
  mov dx, #0xffff // unknown remaining time 
  mov si, #0      // zero battery
  jmp APMSYM(ok)

;-----------------
; Get PM Event
APMSYM(0b):
  cmp al, #0x0b
  jne APMSYM(0e)
  mov ah, #0x80 // no event pending
  jmp APMSYM(error)
   
;-----------------
; APM Driver Version
APMSYM(0e):
  cmp al, #0x0e
  jne APMSYM(unimplemented)
  mov ah, ch
  mov al, cl
  jmp APMSYM(ok)

;-----------------
APMSYM(ok):
  popf
  clc
#if defined(APM_REAL)
  jmp iret_modify_cf
#else
  retf  
#endif
APMSYM(unimplemented):
APMSYM(error):
  popf
  stc
#if defined(APM_REAL)
  jmp iret_modify_cf
#else
  retf
#endif

#undef APM_PROT32
#undef APM_PROT16
#undef APM_REAL
#undef APMSYM
