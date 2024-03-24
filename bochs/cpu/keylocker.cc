/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_KEYLOCKER

#include "decoder/ia_opcodes.h"

struct _IWKey {
  BxPackedXmmRegister IntegrityKey;     // A 128-bit integrity key used to check that handles have not been tampered with
  BxPackedYmmRegister EncryptionKey;    // A 256-bit encryption key used in wrapping/unwrapping to help protect
                                        // confidentiality of the keys indicated by the handles
  unsigned KeySource;
  bool NoBackup;
} IWKey;

extern bool UnWrapKeyAndAuthentificate384(const BxPackedZmmRegister &handle, struct _IWKey& IWKey, const BxPackedYmmRegister &UnwrappedKey);
extern bool UnWrapKeyAndAuthentificate512(const BxPackedZmmRegister &handle, struct _IWKey& IWKey, const BxPackedYmmRegister &UnwrappedKey);

// Handle for 128-bit AES keys has the following format
// ----------------------------------------------------
// 127:0   | AAD
//         | AAD[0]       Handle not usable if CPL > 0
//         | AAD[1]       Handle not usable for encryption
//         | AAD[2]       Handle not usable for decryption
//         | AAD[23:3]    Reserved, MBZ
//         | AAD[27:24]   KeyType: 0 means AES-128 handle, 1 means AES-256 handle
//         | AAD[127:28]  Reserved, MBZ
// 255:128 | Integrity Tag
// 384:256 | ChipherText

// Handle for 256-bit AES keys has the following format
// ----------------------------------------------------
// 127:0   | AAD
// 255:128 | Integrity Tag
// 512:256 | ChipherText

enum keyType {
  HANDLE_KEY_TYPE_AES128 = 0,
  HANDLE_KEY_TYPE_AES256 = 1
};

enum keyUsability {
  HANDLE_KEY_NOT_USABLE_FOR_ENCRYPTION = (1 << 1),
  HANDLE_KEY_NOT_USABLE_FOR_DECRYPTION = (1 << 2)
};

bool BX_CPU_C::read_Handle(bxInstruction_c *i, bx_address laddr, BxPackedYmmRegister *handle, unsigned key_type, unsigned badkey)
{
  read_linear_ymmword(i->seg(), laddr, handle);

  if ((handle->ymm64u(0) & BX_CONST64(0xFFFFFFFFF0FFFFF8)) != 0 || handle->ymm64u(1))
    return false; // illegal handle

  Bit32u handle0 = handle->ymm32u(0);
  if ((handle0 & 0x1) != 0 && CPL > 0)
    return false; // AAD[0] = 1: CPL > 0 illegal handle

  if ((handle0 & badkey) != 0)
    return false; // handle not usable for encryption or descryption

  if ((handle0 >> 24) != key_type)
    return false; // bad key type

  return true;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESDEC128KL_VdqMdq(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  BxPackedZmmRegister handle;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (! read_Handle(i, laddr, &handle.zmm256(0), HANDLE_KEY_TYPE_AES128, HANDLE_KEY_NOT_USABLE_FOR_DECRYPTION)) {
    setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
  }
  else {
    read_linear_xmmword(i->seg(), (laddr + 32) & i->asize_mask(), &handle.zmm128(2));

    BxPackedYmmRegister UnwrappedKey;
    bool authentic = UnWrapKeyAndAuthentificate384(handle, BX_CPU_THIS_PTR IWKey, UnwrappedKey);
    if (!authentic) {
      setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
    }
    else {
      if (i->getIaOpcode() == BX_IA_AESDEC128KL_VdqMdq) {
        BxPackedXmmRegister dst = AES128Decrypt(BX_READ_XMM_REG(i->dst()), UnwrappedKey);
        BX_WRITE_XMM_REG(i->dst(), dst);
      }
      else { // WIDE version
        for (unsigned reg=0; reg < 8; reg++) {
          BxPackedXmmRegister dst = AES128Decrypt(BX_READ_XMM_REG(reg), UnwrappedKey);
          BX_WRITE_XMM_REG(reg, dst);
        }
      }
      clearEFlagsOSZAPC();
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESDEC256KL_VdqMdq(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  BxPackedZmmRegister handle;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (! read_Handle(i, laddr, &handle.zmm256(0), HANDLE_KEY_TYPE_AES256, HANDLE_KEY_NOT_USABLE_FOR_DECRYPTION)) {
    setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
  }
  else {
    read_linear_ymmword(i->seg(), (laddr + 32) & i->asize_mask(), &handle.zmm256(1));

    BxPackedYmmRegister UnwrappedKey;
    bool authentic = UnWrapKeyAndAuthentificate512(handle, BX_CPU_THIS_PTR IWKey, UnwrappedKey);
    if (!authentic) {
      setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
    }
    else {
      if (i->getIaOpcode() == BX_IA_AESDEC256KL_VdqMdq) {
        BxPackedXmmRegister dst = AES256Decrypt(BX_READ_XMM_REG(i->dst()), UnwrappedKey);
        BX_WRITE_XMM_REG(i->dst(), dst);
      }
      else { // WIDE version
        for (unsigned reg=0; reg < 8; reg++) {
          BxPackedXmmRegister dst = AES256Decrypt(BX_READ_XMM_REG(reg), UnwrappedKey);
          BX_WRITE_XMM_REG(reg, dst);
        }
      }
      clearEFlagsOSZAPC();
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESENC128KL_VdqMdq(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  BxPackedZmmRegister handle;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (! read_Handle(i, laddr, &handle.zmm256(0), HANDLE_KEY_TYPE_AES128, HANDLE_KEY_NOT_USABLE_FOR_ENCRYPTION)) {
    setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
  }
  else {
    read_linear_xmmword(i->seg(), (laddr + 32) & i->asize_mask(), &handle.zmm128(2));

    BxPackedYmmRegister UnwrappedKey;
    bool authentic = UnWrapKeyAndAuthentificate384(handle, BX_CPU_THIS_PTR IWKey, UnwrappedKey);
    if (!authentic) {
      setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
    }
    else {
      if (i->getIaOpcode() == BX_IA_AESENC128KL_VdqMdq) {
        BxPackedXmmRegister dst = AES128Encrypt(BX_READ_XMM_REG(i->dst()), UnwrappedKey);
        BX_WRITE_XMM_REG(i->dst(), dst);
      }
      else { // WIDE version
        for (unsigned reg=0; reg < 8; reg++) {
          BxPackedXmmRegister dst = AES128Encrypt(BX_READ_XMM_REG(reg), UnwrappedKey);
          BX_WRITE_XMM_REG(reg, dst);
        }
      }
      clearEFlagsOSZAPC();
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESENC256KL_VdqMdq(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  BxPackedZmmRegister handle;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  if (! read_Handle(i, laddr, &handle.zmm256(0), HANDLE_KEY_TYPE_AES256, HANDLE_KEY_NOT_USABLE_FOR_ENCRYPTION)) {
    setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
  }
  else {
    read_linear_ymmword(i->seg(), (laddr + 32) & i->asize_mask(), &handle.zmm256(1));

    BxPackedYmmRegister UnwrappedKey;
    bool authentic = UnWrapKeyAndAuthentificate512(handle, BX_CPU_THIS_PTR IWKey, UnwrappedKey);
    if (!authentic) {
      setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
    }
    else {
      if (i->getIaOpcode() == BX_IA_AESENC256KL_VdqMdq) {
        BxPackedXmmRegister dst = AES256Encrypt(BX_READ_XMM_REG(i->dst()), UnwrappedKey);
        BX_WRITE_XMM_REG(i->dst(), dst);
      }
      else { // WIDE version
        for (unsigned reg=0; reg < 8; reg++) {
          BxPackedXmmRegister dst = AES256Encrypt(BX_READ_XMM_REG(reg), UnwrappedKey);
          BX_WRITE_XMM_REG(reg, dst);
        }
      }
      clearEFlagsOSZAPC();
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ENCODEKEY128_GdEd(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  Bit32u opsrc = BX_READ_32BIT_REG(i->src());

  // check for reserved bits:
  // -----------------------
  // opsrc[0]       CPL 0 only restriction, reserved if CPUID[19h].EAX[0]=0
  // opsrc[1]       no-encrypt restriction, reserved if CPUID[19h].EAX[1]=0
  // opsrc[2]       no-decrypt restriction, reserved if CPUID[19h].EAX[2]=0
  // opsrc[31:3]    Reserved, MBZ

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ENCODEKEY256_GdEd(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  Bit32u opsrc = BX_READ_32BIT_REG(i->src());

  // check for reserved bits:
  // -----------------------
  // opsrc[0]       CPL 0 only restriction, reserved if CPUID[19h].EAX[0]=0
  // opsrc[1]       no-encrypt restriction, reserved if CPUID[19h].EAX[1]=0
  // opsrc[2]       no-decrypt restriction, reserved if CPUID[19h].EAX[2]=0
  // opsrc[31:3]    Reserved, MBZ

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOADIWKEY_VdqWdq(bxInstruction_c *i)
{
  if(! BX_CPU_THIS_PTR cr4.get_KEYLOCKER()) {
    BX_ERROR(("%s: KeyLocker feature is not enabled in CR4!", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("LOADIWKEY require CPL=0 !"));
    exception(BX_GP_EXCEPTION, 0);
  }

/*
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_KEYLOCKER_VMEXIT)) {
      VMexit_Instruction(VMX_VMEXIT_KEYLOCKER, 0);
    }
  }
#endif
*/

  Bit32u keysource = (EAX >> 1) & 0xF;      // EAX[4:1]
  if (keysource > 1) {
    BX_ERROR(("LOADIWKEY reserved key source used !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (EAX & 0xFFFFFFE0) {
    BX_ERROR(("LOADIWKEY reserved bits set in EAX=0x%08x !", EAX));
    exception(BX_GP_EXCEPTION, 0);
  }

/*
  if EAX[0]         and CPUID[19h].EAX[0]==0: #GP(0) NoBackup is not supported on this part
  if keysource == 1 and CPUID[19h].EAX[1]==0: #GP(0) Keysource 1 is not supported on this part
*/

  BX_CPU_THIS_PTR IWKey.EncryptionKey.ymm128(0) = BX_READ_XMM_REG(i->src());
  BX_CPU_THIS_PTR IWKey.EncryptionKey.ymm128(1) = BX_READ_XMM_REG(i->dst());

  BX_CPU_THIS_PTR IWKey.IntegrityKey = BX_READ_XMM_REG(0);

  BX_CPU_THIS_PTR IWKey.NoBackup  = EAX & 0x1;
  BX_CPU_THIS_PTR IWKey.KeySource = keysource;

  if (keysource == 0) {
    clearEFlagsOSZAPC();
  }
  else {
    extern bool hw_rand_ready();
    extern Bit64u hw_rand64();

    if (! hw_rand_ready()) {
      setEFlagsOSZAPC(EFlagsZFMask); // ZF=1; OF,SF,AF,PF,CF=0
    }
    else {
      // XOR with full-entropy random data with RDSEED
      for (unsigned n=0; n < 4; n++) {
        BX_CPU_THIS_PTR IWKey.EncryptionKey.ymm64u(n) ^= hw_rand64();
      }
      BX_CPU_THIS_PTR IWKey.IntegrityKey.xmm64u(0) ^= hw_rand64();
      BX_CPU_THIS_PTR IWKey.IntegrityKey.xmm64u(1) ^= hw_rand64();

      clearEFlagsOSZAPC();
    }
  }
      
  BX_PANIC(("%s: KeyLocker instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

#endif
