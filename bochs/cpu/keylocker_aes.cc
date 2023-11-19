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

#include <wmmintrin.h> 

inline __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2) 
{ 
    __m128i temp3; 
    temp2 = _mm_shuffle_epi32 (temp2 ,0xff); 
    temp3 = _mm_slli_si128 (temp1, 0x4); 
    temp1 = _mm_xor_si128 (temp1, temp3); 
    temp3 = _mm_slli_si128 (temp3, 0x4); 
    temp1 = _mm_xor_si128 (temp1, temp3); 
    temp3 = _mm_slli_si128 (temp3, 0x4); 
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2); 
    return temp1; 
}

void AES_128_Key_Expansion (const unsigned char *userkey, unsigned char *key) 
{ 
    __m128i temp1, temp2; 
    __m128i *Key_Schedule = (__m128i*)key; 
    temp1 = _mm_loadu_si128((__m128i*)userkey); 
    Key_Schedule[0] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[1] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x2); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[2] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x4); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[3] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x8); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[4] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x10); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[5] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x20); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[6] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x40); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[7] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x80); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[8] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[9] = temp1; 
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x36); 
    temp1 = AES_128_ASSIST(temp1, temp2); 
    Key_Schedule[10] = temp1; 
}

inline void KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2) 
{ 
    __m128i temp4; 
    *temp2 = _mm_shuffle_epi32(*temp2, 0xff); 
    temp4 = _mm_slli_si128 (*temp1, 0x4); 
    *temp1 = _mm_xor_si128 (*temp1, temp4); 
    temp4 = _mm_slli_si128 (temp4, 0x4); 
    *temp1 = _mm_xor_si128 (*temp1, temp4); 
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4); 
    *temp1 = _mm_xor_si128 (*temp1, *temp2); 
} 

inline void KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3) 
{ 
    __m128i temp2,temp4; 
    temp4 = _mm_aeskeygenassist_si128 (*temp1, 0x0); 
    temp2 = _mm_shuffle_epi32(temp4, 0xaa); 
    temp4 = _mm_slli_si128 (*temp3, 0x4); 
    *temp3 = _mm_xor_si128 (*temp3, temp4); 
    temp4 = _mm_slli_si128 (temp4, 0x4); 
    *temp3 = _mm_xor_si128 (*temp3, temp4); 
    temp4 = _mm_slli_si128 (temp4, 0x4); 
    *temp3 = _mm_xor_si128 (*temp3, temp4); 
    *temp3 = _mm_xor_si128 (*temp3, temp2); 
}

void AES_256_Key_Expansion (const unsigned char *userkey, unsigned char *key) 
{ 
    __m128i temp1, temp2, temp3; 
    __m128i *Key_Schedule = (__m128i*)key; 
    temp1 = _mm_loadu_si128((__m128i*)userkey); 
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16)); 
    Key_Schedule[0] = temp1; 
    Key_Schedule[1] = temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x01); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[2]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[3]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x02); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[4]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[5]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x04); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[6]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[7]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x08); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[8]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[9]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x10); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[10]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[11]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x20); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[12]=temp1; 
    KEY_256_ASSIST_2(&temp1, &temp3); 
    Key_Schedule[13]=temp3; 
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x40); 
    KEY_256_ASSIST_1(&temp1, &temp2); 
    Key_Schedule[14]=temp1; 
} 

void AES128Encrypt(unsigned char *dest /*pointer to the PLAINTEXT/CIPHERTEXT*/, const char *userkey /*pointer to AES128 encryption key*/)
{
    __m128i tmp;
    __m128i *key[10];
    int i;
    tmp = _mm_loadu_si128 ((__m128i*)dest));
    AES_128_Key_Expansion (userkey,key);
    tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
    for(i=1; i <10; i++) {
        tmp = _mm_aesenc_si128 (tmp,((__m128i*)key)[i]);
    }
    tmp = _mm_aesenclast_si128 (tmp,((__m128i*)key)[i]);
    _mm_storeu_si128 ((__m128i*)dest),tmp);
}

void AES128Decrypt(unsigned char *dest /*pointer to the PLAINTEXT/CIPHERTEXT*/, const char *userkey /*pointer to the expanded key schedule*/)
{
    __m128i tmp;
    __m128i key[10], key_decrypt[10];
    int i, round;
    AES_128_Key_Expansion (userkey, key);
    key_decrypt [0] = key [10];
    for (round = 1; round <10; round++) {
    key_decrypt [round] = _mm_aesimc_si128(key[10 - round]);
    }
    key_decrypt [10] = key[0];
    tmp = _mm_loadu_si128 ((__m128i*)dest);
    tmp = _mm_xor_si128 (tmp,((__m128i*)key_decrypt)[0]);
    for(i=1; i <10; i++) {
        tmp = _mm_aesdec_si128 (tmp,((__m128i*)key_decrypt)[i]);
    }
    tmp = _mm_aesdeclast_si128 (tmp,((__m128i*)key)[i]);
    _mm_storeu_si128 ((__m128i*)dest)[i], tmp);
}

void AES256Encrypt(unsigned char *dest /*pointer to the PLAINTEXT/CIPHERTEXT*/, const char *userkey /*pointer to AES128 encryption key*/)
{
    __m128i tmp;
    __m128i *key[14];
    int i;
    tmp = _mm_loadu_si128 ((__m128i*)dest));
    AES_256_Key_Expansion (userkey,key);
    tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
    for(i=1; i <14; i++) {
        tmp = _mm_aesenc_si128 (tmp,((__m128i*)key)[i]);
    }
    tmp = _mm_aesenclast_si128 (tmp,((__m128i*)key)[i]);
    _mm_storeu_si128 ((__m128i*)dest),tmp);
}

void AES256Decrypt(unsigned char *dest /*pointer to the CIPHERTEXT/PLAINTEXT*/, const char *userkey /*pointer to the expanded key schedule*/)
{
    __m128i tmp;
    __m128i key[14], key_decrypt[14];
    int i, round;
    AES_256_Key_Expansion (userkey, key);
    key_decrypt [0] = key [14]; 
    for (round = 1; round <14; round++){
        key_decrypt [round] = _mm_aesimc_si128(key[14 - round]);
    }
    key_decrypt [14] = key[0];
    tmp = _mm_loadu_si128((__m128i*)dest);
    tmp = _mm_xor_si128 (tmp,((__m128i*)key_decrypt)[0]);
    for(i=1; i <14; i++) {
        tmp = _mm_aesdec_si128 (tmp,((__m128i*)key_decrypt)[i]);
    }
    tmp = _mm_aesdeclast_si128 (tmp,((__m128i*)key_decrypt)[i]);
    _mm_storeu_si128((__m128i*)dest)[i], tmp);
}

// 
// AES GCM SIV Algorithm
//

#include <stdint.h>
#include <string.h>
#include <wmmintrin.h>
#include <emmintrin.h>
# include <smmintrin.h>
#include "measurements.h"

void Horner_Step(uint8_t* A, uint8_t* B, uint8_t* H, uint8_t* res)
{
    register __m128i _A, _B, _H, TMP1, TMP2, TMP3, TMP4, POLY;
    POLY = _mm_setr_epi32(0x1,0,0,0xc2000000);
    _H = _mm_loadu_si128((__m128i*)H);
    _A = _mm_loadu_si128((__m128i*)A);
    _B = _mm_loadu_si128((__m128i*)B);
    _A = _mm_xor_si128(_A, _B);
    TMP1 = _mm_clmulepi64_si128(_A, _H, 0x00);
    TMP4 = _mm_clmulepi64_si128(_A, _H, 0x11);
    TMP2 = _mm_clmulepi64_si128(_A, _H, 0x10);
    TMP3 = _mm_clmulepi64_si128(_A, _H, 0x01);
    TMP2 = _mm_xor_si128(TMP2, TMP3);
    TMP3 = _mm_slli_si128(TMP2, 8);
    TMP2 = _mm_srli_si128(TMP2, 8);
    TMP1 = _mm_xor_si128(TMP3, TMP1);
    TMP4 = _mm_xor_si128(TMP4, TMP2);
    TMP2 = _mm_clmulepi64_si128(TMP1, POLY, 0x10);
    TMP3 = _mm_shuffle_epi32(TMP1, 78);
    TMP1 = _mm_xor_si128(TMP3, TMP2);
    TMP2 = _mm_clmulepi64_si128(TMP1, POLY, 0x10);
    TMP3 = _mm_shuffle_epi32(TMP1, 78);
    TMP1 = _mm_xor_si128(TMP3, TMP2); 
    _A = _mm_xor_si128(TMP4, TMP1); 
    _mm_storeu_si128((__m128i*)res, _A);
}

void AES256_KS1_ENC1_On_The_Fly(const unsigned char* PT, unsigned char* CT, unsigned char* KS, unsigned char* key)
{
    register __m128i xmm1, xmm2, xmm3, xmm4, con3, xmm14, b1, mask, con1, _P1, _AAD, POLY, _LENBLK;
    int i=0;
    __m128i* Key_Schedule = (__m128i*)KS;
    mask = _mm_setr_epi32(0x0c0f0e0d,0x0c0f0e0d,0x0c0f0e0d,0x0c0f0e0d);
    con1 = _mm_setr_epi32(1,1,1,1);
    con3 = _mm_setr_epi8(-1,-1,-1,-1,-1,-1,-1,-1,4,5,6,7,4,5,6,7);
    xmm4 = _mm_setzero_si128();
    xmm14 = _mm_setzero_si128();
    xmm1 = _mm_loadu_si128((__m128i*)key);
    xmm3 = _mm_loadu_si128(&(((__m128i*)key)[1]));
    _mm_storeu_si128(&Key_Schedule[0], xmm1);
    b1 = _mm_loadu_si128((__m128i*)PT);
    b1 = _mm_xor_si128(b1, xmm1);
    b1 = _mm_aesenc_si128(b1, xmm3);
    _mm_storeu_si128(&Key_Schedule[1], xmm3);
    for (i=0; i<6; i++)
    {
        xmm2 = _mm_shuffle_epi8(xmm3, mask);
        xmm2 = _mm_aesenclast_si128(xmm2, con1);
        con1 = _mm_slli_epi32(con1, 1);
        xmm4 = _mm_slli_epi64 (xmm1, 32);
        xmm1 = _mm_xor_si128(xmm1, xmm4);
        xmm4 = _mm_shuffle_epi8(xmm1, con3);
        xmm1 = _mm_xor_si128(xmm1, xmm4);
        xmm1 = _mm_xor_si128(xmm1, xmm2);
        _mm_storeu_si128(&Key_Schedule[(i+1)*2], xmm1);
        b1 = _mm_aesenc_si128(b1, xmm1);
 
        xmm2 = _mm_shuffle_epi32(xmm1, 0xff);
        xmm2 = _mm_aesenclast_si128(xmm2, xmm14);
        xmm4 = _mm_slli_epi64(xmm3, 32);
        xmm3 = _mm_xor_si128(xmm4, xmm3);
        xmm4 = _mm_shuffle_epi8(xmm3, con3);
        xmm3 = _mm_xor_si128(xmm4, xmm3);
        xmm3 = _mm_xor_si128(xmm2, xmm3);
        _mm_storeu_si128(&Key_Schedule[(i+1)*2+1], xmm3);
        b1 = _mm_aesenc_si128(b1, xmm3);
    }
    xmm2 = _mm_shuffle_epi8(xmm3, mask);
    xmm2 = _mm_aesenclast_si128(xmm2, con1);
    xmm4 = _mm_slli_epi64 (xmm1, 32);
    xmm1 = _mm_xor_si128(xmm1, xmm4);
    xmm4 = _mm_shuffle_epi8(xmm1, con3);
    xmm1 = _mm_xor_si128(xmm1, xmm4);
    xmm1 = _mm_xor_si128(xmm1, xmm2);
    _mm_storeu_si128(&Key_Schedule[14], xmm1);
    b1 = _mm_aesenclast_si128(b1, xmm1);
    _mm_storeu_si128((__m128i*)CT, b1); 
}
void AES_256_Encrypt(uint8_t* PT, uint8_t* CT, uint8_t* KS)
{
    __m128i block1;
    uint8_t TMP_KS[16*15]={0};
    block1 = _mm_loadu_si128((__m128i*)PT);
    memcpy(TMP_KS,KS,(sizeof(uint8_t)*16*15));
    block1 = _mm_xor_si128(block1, *((__m128i*)(TMP_KS+16*0)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*1)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*2)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*3)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*4)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*5)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*6)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*7)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*8)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*9)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*10)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*11)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*12)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*13)));
    block1 = _mm_aesenclast_si128(block1, *((__m128i*)(TMP_KS+16*14)));
    _mm_storeu_si128((__m128i*)CT, block1);
}
void AES_256_Encrypt_x2(uint8_t* PT, uint8_t* PT1, uint8_t* CT, uint8_t* CT1, uint8_t* KS)
{
    __m128i block1, block2;
    uint8_t TMP_KS[16*15]={0};
    block1 = _mm_loadu_si128((__m128i*)PT);
    block2 = _mm_loadu_si128((__m128i*)PT1);
    memcpy(TMP_KS,KS,(sizeof(uint8_t)*16*15));
    block1 = _mm_xor_si128(block1, *((__m128i*)(TMP_KS+16*0)));
    block2 = _mm_xor_si128(block2, *((__m128i*)(TMP_KS+16*0)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*1)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*1)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*2)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*2)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*3)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*3)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*4)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*4)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*5)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*5)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*6)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*6)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*7)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*7)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*8)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*8)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*9)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*9)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*10)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*10)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*11)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*11)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*12)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*12)));
    block1 = _mm_aesenc_si128(block1, *((__m128i*)(TMP_KS+16*13)));
    block2 = _mm_aesenc_si128(block2, *((__m128i*)(TMP_KS+16*13)));
    block1 = _mm_aesenclast_si128(block1, *((__m128i*)(TMP_KS+16*14)));
    block2 = _mm_aesenclast_si128(block2, *((__m128i*)(TMP_KS+16*14)));
    _mm_storeu_si128((__m128i*)CT, block1);
    _mm_storeu_si128((__m128i*)CT1, block2); 
}

void SIV_KEY_WRAP_16B(uint8_t* K1,uint8_t* K2, uint8_t* AAD, uint8_t* PT, uint8_t* CT, uint8_t* T)
{
    __m128i zero = _mm_setzero_si128();
    uint8_t TMP[64] = {0};
    uint8_t KS[16*15] = {0};
    __m128i LENBLK;
    __m128i AND_MASK, TOP_ONE;
    uint8_t TMP_PT[64]={0};
    TOP_ONE = _mm_setr_epi32(0,0,0,0x80000000);
    LENBLK = _mm_setr_epi32(16*8, 0, 16*8, 0);
    AND_MASK = _mm_setr_epi32(0xffffffff,0xffffffff,0xffffffff,0x7fffffff);
    Horner_Step(AAD, (uint8_t*)&zero, K1, TMP);
    Horner_Step(PT, TMP, K1, TMP);
    Horner_Step(TMP, (uint8_t*)&LENBLK, K1, TMP);

    // TMP = POLYVAL;
    *(__m128i*)TMP = _mm_and_si128(*(__m128i*)TMP, AND_MASK);
 
    AES256_KS1_ENC1_On_The_Fly(TMP, TMP, KS, K2);
 
    _mm_storeu_si128((__m128i*)T, *(__m128i*)TMP);
    *(__m128i*)TMP = _mm_or_si128(*(__m128i*)TMP, TOP_ONE);
    AES_256_Encrypt(TMP, TMP, KS);
    memcpy(TMP_PT,PT,(sizeof(uint8_t)*32));
    *(__m128i*)TMP = _mm_xor_si128(*(__m128i*)TMP, *((__m128i*)TMP_PT));
    _mm_storeu_si128((__m128i*)CT, *(__m128i*)TMP);
}

void SIV_KEY_WRAP_32B(uint8_t* K1,uint8_t* K2, uint8_t* AAD, uint8_t* PT, uint8_t* CT, uint8_t* T)
{
    __m128i zero = _mm_setzero_si128();
    uint8_t TMP[64] = {0};
    uint8_t TMP1[64] = {0};
    uint8_t KS[16*15] = {0};
    __m128i ONE, AND_MASK, LENBLK, TOP_ONE;
    uint8_t TMP_PT[64]={0};
    ONE = _mm_setr_epi32(1,0,0,0);
    TOP_ONE = _mm_setr_epi32(0,0,0,0x80000000);
    LENBLK = _mm_setr_epi32(16*8, 0, 32*8, 0);
    AND_MASK = _mm_setr_epi32(0xffffffff,0xffffffff,0xffffffff,0x7fffffff);
    Horner_Step(AAD, (uint8_t*)&zero, K1, TMP);
    Horner_Step(PT, TMP, K1, TMP);
    Horner_Step(PT+16, TMP, K1, TMP);
    Horner_Step(TMP, (uint8_t*)&LENBLK, K1, TMP);
    // TMP = POLYVAL;
    *(__m128i*)TMP = _mm_and_si128(*(__m128i*)TMP, AND_MASK);
    AES256_KS1_ENC1_On_The_Fly(TMP, TMP, KS, K2);
    _mm_storeu_si128((__m128i*)T, *(__m128i*)TMP);
    *(__m128i*)TMP = _mm_or_si128(*(__m128i*)TMP, TOP_ONE);
    *(__m128i*)TMP1 = _mm_add_epi32(*(__m128i*)TMP, ONE);
    AES_256_Encrypt_x2(TMP, TMP1, TMP, TMP1, KS);
    memcpy(TMP_PT,PT,(sizeof(uint8_t)*32));
    *(__m128i*)TMP = _mm_xor_si128(*(__m128i*)TMP, *((__m128i*)TMP_PT));
    *(__m128i*)TMP1 = _mm_xor_si128(*(__m128i*)TMP1, *((__m128i*)(TMP_PT+16)));
    _mm_storeu_si128((__m128i*)CT, *(__m128i*)TMP);
    _mm_storeu_si128((__m128i*)(CT+16), *(__m128i*)TMP1);
}

int SIV_KEY_UNWRAP_32B(uint8_t* K1,uint8_t* K2, uint8_t* AAD, uint8_t* CT, uint8_t* T, uint8_t* PT)
{
    __m128i zero = _mm_setzero_si128();
    uint8_t TMP[64] = {0};
    uint8_t TMP_T[64] = {0};
    uint8_t TMP1[64] = {0};
    uint8_t TMP2[64] = {0};
    uint8_t KS[16*15] = {0};
    uint8_t TMP_CT[64]={0};
    __m128i ONE, AND_MASK, LENBLK, TOP_ONE;
    ONE = _mm_setr_epi32(1,0,0,0);
    TOP_ONE = _mm_setr_epi32(0,0,0,0x80000000);
    LENBLK = _mm_setr_epi32(16*8, 0, 32*8, 0);
    AND_MASK = _mm_setr_epi32(0xffffffff,0xffffffff,0xffffffff,0x7fffffff);
    memcpy(TMP_T,T,(sizeof(uint8_t)*16));
    *(__m128i*)TMP = _mm_or_si128(*(__m128i*)TMP_T, TOP_ONE);
    *(__m128i*)TMP1 = _mm_add_epi32(*(__m128i*)TMP, ONE);
    AES256_KS1_ENC1_On_The_Fly(TMP, TMP, KS, K2);
    AES_256_Encrypt(TMP1, TMP1, KS);
    memcpy(TMP_CT,CT,(sizeof(uint8_t)*32));
    *(__m128i*)TMP = _mm_xor_si128(*(__m128i*)TMP, *((__m128i*)TMP_CT));
    *(__m128i*)TMP1 = _mm_xor_si128(*(__m128i*)TMP1, *((__m128i*)(TMP_CT+16)));
    Horner_Step(AAD, (uint8_t*)&zero, K1, TMP2);
    Horner_Step(TMP, TMP2, K1, TMP2);
    Horner_Step(TMP1, TMP2, K1, TMP2);
    Horner_Step(TMP2, (uint8_t*)&LENBLK, K1, TMP2);
    // TMP = POLYVAL;
    *(__m128i*)TMP2 = _mm_and_si128(*(__m128i*)TMP2, AND_MASK);
    AES_256_Encrypt(TMP2, TMP2, KS);
    if (memcmp(TMP2, TMP_T, 16)==0)
    {
        _mm_storeu_si128((__m128i*)(PT), *(__m128i*)TMP);
        _mm_storeu_si128((__m128i*)(PT+16), *(__m128i*)TMP1);
        return 1;
    }
    return 0;
}

int SIV_KEY_UNWRAP_16B(uint8_t* K1,uint8_t* K2, uint8_t* AAD, uint8_t* CT, uint8_t* T, uint8_t* PT)
{
    __m128i zero = _mm_setzero_si128();
    uint8_t TMP[64] = {0};
    uint8_t TMP2[64] = {0};
    uint8_t KS[16*15] = {0};
    __m128i AND_MASK, LENBLK, TOP_ONE;
    uint8_t TMP_CT[64];
    uint8_t TMP_T[64];
    memcpy(TMP_T,T,sizeof(uint8_t)*16);
    TOP_ONE = _mm_setr_epi32(0,0,0,0x80000000);
    LENBLK = _mm_setr_epi32(16*8, 0, 16*8, 0);
    AND_MASK = _mm_setr_epi32(0xffffffff,0xffffffff,0xffffffff,0x7fffffff);
    *(__m128i*)TMP = _mm_or_si128(*(__m128i*)TMP_T, TOP_ONE);
    AES256_KS1_ENC1_On_The_Fly(TMP, TMP, KS, K2);
    memcpy(TMP_CT,CT,sizeof(uint8_t)*32);
    *(__m128i*)TMP = _mm_xor_si128(*(__m128i*)TMP, *((__m128i*)TMP_CT));
    Horner_Step(AAD, (uint8_t*)&zero, K1, TMP2);
    Horner_Step(TMP, TMP2, K1, TMP2);
    Horner_Step(TMP2, (uint8_t*)&LENBLK, K1, TMP2);
    // TMP = POLYVAL;
    *(__m128i*)TMP2 = _mm_and_si128(*(__m128i*)TMP2, AND_MASK);
    AES_256_Encrypt(TMP2, TMP2, KS);
    if (memcmp(TMP2, T, 16)==0)
    {
        _mm_storeu_si128((__m128i*)(PT), *(__m128i*)TMP);
        return 1;
    }
    return 0;
}

char *WrapKey128(unsigned char *aeskey, unsigned char *aeskeymetadata, unsigned char *integritykey, unsigned char *encryptionkey, unsigned char *handle)
{ 
    unsigned char ciphertext[16] = {0}; 
    unsigned char authtag[16] = {0};
    int i;

    SIV_KEY_WRAP_16B (integritykey, encryptionkey, aeskeymetadata, aeskey, ciphertext, authtag);
    for (i=0; i<16; i++) {
        handle[i] = aeskeymetadata[i];
        handle[i+16] = authtag[i];
        handle[i+32] = ciphertext[i];
    }
    return handle;
}

char * WrapKey256(unsigned char *aeskey, unsigned char *aeskeymetadata, unsigned char *integritykey, unsigned char *encryptionkey, unsigned char *handle)
{ 
    unsigned char ciphertext[32] = {0}; 
    unsigned char authtag[16] = {0};
    int i;

    SIV_KEY_WRAP_32B(integritykey, encryptionkey, aeskeymetadata, aeskey, ciphertext, authtag);
    for (i=0; i<16; i++) {
        handle[i] = aeskeymetadata[i];
        handle[i+16] = authtag[i];
        handle[i+32] = ciphertext[i];
        handle[i+48] = ciphertext[i+16];
    }
    return handle;
}

int UnwrapKeyAndAuthenticate384(const unsigned char *handle, const unsigned char *iwkey, unsigned char *unwrappedkey) 
{
    unsigned char integritykey[16] = {0};
    unsigned char encryptionkey[32] = {0};
    unsigned char aad[16] = {0};
    unsigned char authtag[16] = {0};
    int i;
    for (i=0; i<32; i++) {
        encryptionkey[i] = iwkey[i+16];
    }
    for (i=0;i<16;i++) {
        integritykey[i] = iwkey[i];
        aad[i] = handle[i];
        authtag[i] = handle[i+16];
    }
    unsigned char ciphertext[16];
    for(i=0;i<16;i++)
        ciphertext[i] = handle[i+32];
    return SIV_KEY_UNWRAP_16B(integritykey, encryptionkey, aad, ciphertext, authtag, unwrappedkey);
}

int UnwrapKeyAndAuthenticate512(const unsigned char *handle, const unsigned char *iwkey, unsigned char *unwrappedkey) 
{
    unsigned char integritykey[16] = {0};
    unsigned char encryptionkey[32] = {0};
    unsigned char aad[16] = {0};
    unsigned char authtag[16] = {0};
    int i;
    for (i=0; i<32; i++) {
        encryptionkey[i] = iwkey[i+16];
    }
    for (i=0;i<16;i++) {
        integritykey[i] = iwkey[i];
        aad[i] = handle[i];
        authtag[i] = handle[i+16];
    }
    unsigned char ciphertext[32];
    for(i=0; i<32; i++)
        ciphertext[i] = handle[i+32];
    return SIV_KEY_UNWRAP_32B(integritykey, encryptionkey, aad, ciphertext, authtag, unwrappedkey);
}

#endif
