/////////////////////////////////////////////////////////////////////////
// $Id: instrument.h,v 1.5 2001-10-03 13:10:38 bdenney Exp $
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



// possible types passed to BX_INSTR_TLB_CNTRL()
#define BX_INSTR_MOV_CR3     10
#define BX_INSTR_INVLPG      11
#define BX_INSTR_TASKSWITCH  12

// possible types passed to BX_INSTR_CACHE_CNTRL()
#define BX_INSTR_INVD      20
#define BX_INSTR_WBINVD    21



#if BX_INSTRUMENTATION

#define BX_INSTR_IS_CALL  10
#define BX_INSTR_IS_RET   11
#define BX_INSTR_IS_IRET  12
#define BX_INSTR_IS_JMP   13
#define BX_INSTR_IS_INT   14


// called from the CPU core
void bx_instr_cnear_branch_taken(Bit32u new_eip);
void bx_instr_cnear_branch_not_taken(void);
void bx_instr_ucnear_branch(unsigned what, Bit32u new_eip);
void bx_instr_far_branch(unsigned what, Bit32u new_cs, Bit32u new_eip);
void bx_instr_opcode_byte1(Bit8u);
void bx_instr_opcode_byte2(Bit8u);
void bx_instr_opcode_g1ebib(unsigned nnn);
void bx_instr_opcode_g1eviv(unsigned nnn);
void bx_instr_opcode_g1evib(unsigned nnn);
void bx_instr_opcode_g2ebib(unsigned nnn);
void bx_instr_opcode_g2evib(unsigned nnn);
void bx_instr_opcode_g2eb1(unsigned nnn);
void bx_instr_opcode_g2ev1(unsigned nnn);
void bx_instr_opcode_g2ebcl(unsigned nnn);
void bx_instr_opcode_g2evcl(unsigned nnn);
void bx_instr_opcode_g3eb(unsigned nnn);
void bx_instr_opcode_g3ev(unsigned nnn);
void bx_instr_opcode_g4(unsigned nnn);
void bx_instr_opcode_g5(unsigned nnn);
void bx_instr_opcode_g6(unsigned nnn);
void bx_instr_opcode_g7(unsigned nnn);
void bx_instr_opcode_g8evib(unsigned nnn);
void bx_instr_mem_code(Bit32u linear, unsigned size);
void bx_instr_mem_data(Bit32u linear, unsigned size, unsigned rw);
void bx_instr_opcode_begin(Bit32u linear);
void bx_instr_opcode_end(Bit32u linear);
void bx_instr_fetch_byte(Bit8u val8);
void bx_instr_fetch_word(Bit16u val16);
void bx_instr_fetch_dword(Bit32u val32);
void bx_instr_phy_write(Bit32u addr, unsigned len);
void bx_instr_phy_read(Bit32u addr, unsigned len);
void bx_instr_interrupt(unsigned vector);
void bx_instr_exception(unsigned vector);
void bx_instr_inp(Bit16u addr, unsigned len);
void bx_instr_outp(Bit16u addr, unsigned len);
void bx_instr_inp2(Bit16u addr, unsigned len, unsigned val);
void bx_instr_outp2(Bit16u addr, unsigned len, unsigned val);
void bx_instr_tlb_cntrl(unsigned what, Bit32u newval);
void bx_instr_cache_cntrl(unsigned what);
void bx_instr_hwinterrupt(unsigned vector, Bit32u cs, Bit32u eip);
void bx_instr_init(void);
void bx_instr_shutdown(void);
void bx_instr_opcode_repeating(void);
void bx_instr_prefix_as(void);
void bx_instr_prefix_os(void);
void bx_instr_prefix_rep(void);
void bx_instr_prefix_repne(void);
void bx_instr_prefix_lock(void);
void bx_instr_prefix_cs(void);
void bx_instr_prefix_ss(void);
void bx_instr_prefix_ds(void);
void bx_instr_prefix_es(void);
void bx_instr_prefix_fs(void);
void bx_instr_prefix_gs(void);
void bx_instr_modrm32(unsigned modrm);
void bx_instr_sib32(unsigned sib);
void bx_instr_modrm16(unsigned modrm);
void bx_instr_iret(void);
void bx_instr_debug_prompt(void);
void bx_instr_lin_read(Bit32u lin, Bit32u phy, unsigned len);
void bx_instr_lin_write(Bit32u lin, Bit32u phy, unsigned len);

// called from the debug prompt
void bx_instr_start(void);
void bx_instr_stop(void);
void bx_instr_reset(void);
void bx_instr_print(void);

#  define BX_INSTR_INIT()                      bx_instr_init()
#  define BX_INSTR_SHUTDOWN()                  bx_instr_shutdown()
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(new_eip) bx_instr_cnear_branch_taken(new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN()    bx_instr_cnear_branch_not_taken()
#  define BX_INSTR_UCNEAR_BRANCH(what, new_eip) bx_instr_ucnear_branch(what, new_eip)
#  define BX_INSTR_FAR_BRANCH(what, new_cs, new_eip) bx_instr_far_branch(what, new_cs, new_eip)
#  define BX_INSTR_OPCODE_BEGIN(linear)        bx_instr_opcode_begin(linear)
#  define BX_INSTR_OPCODE_END(linear)          bx_instr_opcode_end(linear)
#  define BX_INSTR_OPCODE_BYTE1(b)             bx_instr_opcode_byte1(b)
#  define BX_INSTR_OPCODE_BYTE2(b)             bx_instr_opcode_byte2(b)
#  define BX_INSTR_OPCODE_G1EbIb(nnn)          bx_instr_opcode_g1ebib(nnn)
#  define BX_INSTR_OPCODE_G1EvIv(nnn)          bx_instr_opcode_g1eviv(nnn)
#  define BX_INSTR_OPCODE_G1EvIb(nnn)          bx_instr_opcode_g1evib(nnn)
#  define BX_INSTR_OPCODE_G2EbIb(nnn)          bx_instr_opcode_g2ebib(nnn)
#  define BX_INSTR_OPCODE_G2EvIb(nnn)          bx_instr_opcode_g2evib(nnn)
#  define BX_INSTR_OPCODE_G2Eb1(nnn)           bx_instr_opcode_g2eb1(nnn)
#  define BX_INSTR_OPCODE_G2Ev1(nnn)           bx_instr_opcode_g2ev1(nnn)
#  define BX_INSTR_OPCODE_G2EbCL(nnn)          bx_instr_opcode_g2ebcl(nnn)
#  define BX_INSTR_OPCODE_G2EvCL(nnn)          bx_instr_opcode_g2evcl(nnn)
#  define BX_INSTR_OPCODE_G3Eb(nnn)            bx_instr_opcode_g3eb(nnn)
#  define BX_INSTR_OPCODE_G3Ev(nnn)            bx_instr_opcode_g3ev(nnn)
#  define BX_INSTR_OPCODE_G4(nnn)              bx_instr_opcode_g4(nnn)
#  define BX_INSTR_OPCODE_G5(nnn)              bx_instr_opcode_g5(nnn)
#  define BX_INSTR_OPCODE_G6(nnn)              bx_instr_opcode_g6(nnn)
#  define BX_INSTR_OPCODE_G7(nnn)              bx_instr_opcode_g7(nnn)
#  define BX_INSTR_OPCODE_G8EvIb(nnn)          bx_instr_opcode_g8evib(nnn)
#  define BX_INSTR_MEM_CODE(linear, size)      bx_instr_mem_code(linear, size)
#  define BX_INSTR_MEM_DATA(linear, size, rw)  bx_instr_mem_data(linear, size, rw)
#  define BX_INSTR_EXCEPTION(vector)           bx_instr_exception(vector)
#  define BX_INSTR_INP(addr, len)              bx_instr_inp(addr, len)
#  define BX_INSTR_INP2(addr, len, val)        bx_instr_inp2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)             bx_instr_outp(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)       bx_instr_outp2(addr, len, val)
#  define BX_INSTR_FETCH_BYTE(val8)            bx_instr_fetch_byte(val8)
#  define BX_INSTR_FETCH_WORD(val16)           bx_instr_fetch_word(val16)
#  define BX_INSTR_FETCH_DWORD(val32)          bx_instr_fetch_dword(val32)
#  define BX_INSTR_PHY_WRITE(addr, len)        bx_instr_phy_write(addr, len)
#  define BX_INSTR_PHY_READ(addr, len)         bx_instr_phy_read(addr, len)
#  define BX_INSTR_INTERRUPT(vector)           bx_instr_interrupt(vector)

#  define BX_INSTR_TLB_CNTRL(what, newval)     bx_instr_tlb_cntrl(what, newval)
#  define BX_INSTR_CACHE_CNTRL(what)           bx_instr_cache_cntrl(what)
#  define BX_INSTR_HWINTERRUPT(vector, cs, eip) bx_instr_hwinterrupt(vector, cs, eip)
#  define BX_INSTR_OPCODE_REPEATING()           bx_instr_opcode_repeating()

#  define BX_INSTR_PREFIX_AS()                 bx_instr_prefix_as()
#  define BX_INSTR_PREFIX_OS()                 bx_instr_prefix_os()
#  define BX_INSTR_PREFIX_REP()                bx_instr_prefix_rep()
#  define BX_INSTR_PREFIX_REPNE()              bx_instr_prefix_repne()
#  define BX_INSTR_PREFIX_LOCK()               bx_instr_prefix_lock()
#  define BX_INSTR_PREFIX_CS()                 bx_instr_prefix_cs()
#  define BX_INSTR_PREFIX_SS()                 bx_instr_prefix_ss()
#  define BX_INSTR_PREFIX_DS()                 bx_instr_prefix_ds()
#  define BX_INSTR_PREFIX_ES()                 bx_instr_prefix_es()
#  define BX_INSTR_PREFIX_FS()                 bx_instr_prefix_fs()
#  define BX_INSTR_PREFIX_GS()                 bx_instr_prefix_gs()

#  define BX_INSTR_MODRM32(modrm)              bx_instr_modrm32(modrm)
#  define BX_INSTR_SIB32(sib)                  bx_instr_sib32(sib)
#  define BX_INSTR_MODRM16(modrm)              bx_instr_modrm16(modrm)
#  define BX_INSTR_SIB_mod0_base5(ss)
#  define BX_INSTR_SIB_MOD0_IND4()
#  define BX_INSTR_SIB_MOD1_IND4()
#  define BX_INSTR_SIB_MOD2_IND4()

#  define BX_INSTR_IRET()                      bx_instr_iret()
#  define BX_INSTR_DEBUG_PROMPT()              bx_instr_debug_prompt()

#  define BX_INSTR_LIN_READ(lin, phy, len)  bx_instr_lin_read(lin, phy, len)
#  define BX_INSTR_LIN_WRITE(lin, phy, len) bx_instr_lin_write(lin, phy, len)
#  define BX_INSTR_START()                   bx_instr_start ()
#  define BX_INSTR_STOP()                    bx_instr_stop ()
#  define BX_INSTR_RESET()                   bx_instr_reset ()
#  define BX_INSTR_PRINT()                   bx_instr_print ()

#else  // #if BX_INSTRUMENTATION
#  define BX_INSTR_INIT()
#  define BX_INSTR_SHUTDOWN()
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN()
#  define BX_INSTR_UCNEAR_BRANCH(what, new_eip)
#  define BX_INSTR_FAR_BRANCH(what, new_cs, new_eip)
#  define BX_INSTR_OPCODE_BEGIN(linear)
#  define BX_INSTR_OPCODE_END(linear)
#  define BX_INSTR_OPCODE_BYTE1(b)
#  define BX_INSTR_OPCODE_BYTE2(b)
#  define BX_INSTR_OPCODE_G1EbIb(nnn)
#  define BX_INSTR_OPCODE_G1EvIv(nnn)
#  define BX_INSTR_OPCODE_G1EvIb(nnn)
#  define BX_INSTR_OPCODE_G2EbIb(nnn)
#  define BX_INSTR_OPCODE_G2EvIb(nnn)
#  define BX_INSTR_OPCODE_G2Eb1(nnn)
#  define BX_INSTR_OPCODE_G2Ev1(nnn)
#  define BX_INSTR_OPCODE_G2EbCL(nnn)
#  define BX_INSTR_OPCODE_G2EvCL(nnn)
#  define BX_INSTR_OPCODE_G3Eb(nnn)
#  define BX_INSTR_OPCODE_G3Ev(nnn)
#  define BX_INSTR_OPCODE_G4(nnn)
#  define BX_INSTR_OPCODE_G5(nnn)
#  define BX_INSTR_OPCODE_G6(nnn)
#  define BX_INSTR_OPCODE_G7(nnn)
#  define BX_INSTR_OPCODE_G8EvIb(nnn)
#  define BX_INSTR_MEM_CODE(linear, size)
#  define BX_INSTR_MEM_DATA(linear, size, rw)
#  define BX_INSTR_EXCEPTION(vector)
#  define BX_INSTR_INP(addr, len)
#  define BX_INSTR_INP2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)
#  define BX_INSTR_FETCH_BYTE(val8)
#  define BX_INSTR_FETCH_WORD(val16)
#  define BX_INSTR_FETCH_DWORD(val32)
#  define BX_INSTR_PHY_WRITE(addr, len)
#  define BX_INSTR_PHY_READ(addr, len)
#  define BX_INSTR_INTERRUPT(vector)
#  define BX_INSTR_TLB_CNTRL(what, newval)
#  define BX_INSTR_CACHE_CNTRL(what)
#  define BX_INSTR_HWINTERRUPT(vector, cs, eip)
#  define BX_INSTR_OPCODE_REPEATING()

#  define BX_INSTR_PREFIX_AS()
#  define BX_INSTR_PREFIX_OS()
#  define BX_INSTR_PREFIX_REP()
#  define BX_INSTR_PREFIX_REPNE()
#  define BX_INSTR_PREFIX_LOCK()
#  define BX_INSTR_PREFIX_CS()
#  define BX_INSTR_PREFIX_SS()
#  define BX_INSTR_PREFIX_DS()
#  define BX_INSTR_PREFIX_ES()
#  define BX_INSTR_PREFIX_FS()
#  define BX_INSTR_PREFIX_GS()

#  define BX_INSTR_MODRM32(modrm)
#  define BX_INSTR_SIB32(sib)
#  define BX_INSTR_MODRM16(modrm)
#  define BX_INSTR_SIB_mod0_base5(ss)
#  define BX_INSTR_SIB_MOD0_IND4()
#  define BX_INSTR_SIB_MOD1_IND4()
#  define BX_INSTR_SIB_MOD2_IND4()

#  define BX_INSTR_IRET()
#  define BX_INSTR_DEBUG_PROMPT()

#  define BX_INSTR_LIN_READ(lin, phy, len)
#  define BX_INSTR_LIN_WRITE(lin, phy, len)
#  define BX_INSTR_START()                   
#  define BX_INSTR_STOP()                    
#  define BX_INSTR_RESET()                   
#  define BX_INSTR_PRINT()                   

#endif  // #if BX_INSTRUMENTATION

