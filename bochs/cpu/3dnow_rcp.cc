/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2025 Stanislav Shwartsman
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

#if BX_SUPPORT_3DNOW && BX_CPU_LEVEL >= 5

extern softfloat_status_t prepare_softfloat_status_word_3dnow(int rounding_mode);

// use 11-bit SSE approximation function, ideally need to use AVX-512 14-bit approximation but it was compiled only under BX_SUPPORT_AVX/BX_SUPPORT_EVEX so far
extern float32 approximate_rsqrt(float32 a);

// Thanks for Cristina Iordache and David W. Matula for reverse-engineering of 3dNow! Reciprocal instructions and writing the article
// Analysis of Reciprocal and Square Root Reciprocal Instructions in the AMD K6-2 Implementation of 3DNow!
// https://www.sciencedirect.com/science/article/pii/S1571066105806218

#include "softfloat3e/include/softfloat.h"
#include "fpu/softfloat-specialize.h"

static Bit16u rcp_table1[1024] = {
  0xFFBE, 0xFF3E, 0xFEBF, 0xFE40, 0xFDC1, 0xFD42, 0xFCC3, 0xFC45, 0xFBC7, 0xFB49, 0xFACC, 0xFA4F, 0xF9D1, 0xF955, 0xF8D8, 0xF85C, // 0x00    FRACTION MSB[b1..b5]
  0xF7E0, 0xF764, 0xF6E8, 0xF66D, 0xF5F2, 0xF577, 0xF4FC, 0xF482, 0xF407, 0xF38D, 0xF314, 0xF29A, 0xF221, 0xF1A8, 0xF12F, 0xF0B7,
  0xF03F, 0xEFC6, 0xEF4F, 0xEED7, 0xEE5F, 0xEDE8, 0xED71, 0xECFA, 0xEC84, 0xEC0E, 0xEB97, 0xEB22, 0xEAAC, 0xEA36, 0xE9C1, 0xE94C, // 0x01
  0xE8D7, 0xE863, 0xE7EE, 0xE77A, 0xE706, 0xE693, 0xE61F, 0xE5AC, 0xE539, 0xE4C6, 0xE453, 0xE3E1, 0xE36F, 0xE2FD, 0xE28B, 0xE219,
  0xE1A8, 0xE136, 0xE0C5, 0xE055, 0xDFE4, 0xDF74, 0xDF04, 0xDE94, 0xDE24, 0xDDB4, 0xDD45, 0xDCD6, 0xDC67, 0xDBF8, 0xDB8A, 0xDB1C, // 0x02
  0xDAAD, 0xDA3F, 0xD9D2, 0xD964, 0xD8F7, 0xD88A, 0xD81D, 0xD7B0, 0xD744, 0xD6D7, 0xD66B, 0xD5FF, 0xD593, 0xD528, 0xD4BC, 0xD451,
  0xD3E6, 0xD37B, 0xD311, 0xD2A6, 0xD23C, 0xD1D2, 0xD168, 0xD0FF, 0xD095, 0xD02C, 0xCFC3, 0xCF5A, 0xCEF1, 0xCE88, 0xCE20, 0xCDB8, // 0x03
  0xCD50, 0xCCE8, 0xCC80, 0xCC19, 0xCBB2, 0xCB4B, 0xCAE4, 0xCA7D, 0xCA16, 0xC9B0, 0xC94A, 0xC8E4, 0xC87E, 0xC818, 0xC7B3, 0xC74D,
  0xC6E9, 0xC684, 0xC61F, 0xC5BA, 0xC556, 0xC4F2, 0xC48D, 0xC42A, 0xC3C6, 0xC362, 0xC2FF, 0xC29C, 0xC239, 0xC1D6, 0xC173, 0xC110, // 0x04
  0xC0AE, 0xC04C, 0xBFEA, 0xBF88, 0xBF26, 0xBEC5, 0xBE63, 0xBE02, 0xBDA1, 0xBD40, 0xBCDF, 0xBC7F, 0xBC1E, 0xBBBE, 0xBB5E, 0xBAFE,
  0xBA9E, 0xBA3E, 0xB9DF, 0xB980, 0xB921, 0xB8C2, 0xB863, 0xB804, 0xB7A6, 0xB747, 0xB6E9, 0xB68B, 0xB62D, 0xB5D0, 0xB572, 0xB515, // 0x05
  0xB4B8, 0xB45B, 0xB3FE, 0xB3A1, 0xB344, 0xB2E8, 0xB28C, 0xB230, 0xB1D4, 0xB178, 0xB11C, 0xB0C1, 0xB065, 0xB00A, 0xAFAF, 0xAF54,
  0xAEFA, 0xAE9F, 0xAE45, 0xADEB, 0xAD90, 0xAD36, 0xACDC, 0xAC83, 0xAC29, 0xABD0, 0xAB76, 0xAB1D, 0xAAC4, 0xAA6B, 0xAA13, 0xA9BA, // 0x06
  0xA962, 0xA909, 0xA8B1, 0xA859, 0xA801, 0xA7AA, 0xA752, 0xA6FB, 0xA6A3, 0xA64C, 0xA5F5, 0xA59E, 0xA548, 0xA4F1, 0xA49B, 0xA444,
  0xA3EE, 0xA398, 0xA342, 0xA2EC, 0xA297, 0xA241, 0xA1EC, 0xA197, 0xA141, 0xA0ED, 0xA098, 0xA043, 0x9FEE, 0x9F9A, 0x9F46, 0x9EF2, // 0x07
  0x9E9E, 0x9E4A, 0x9DF6, 0x9DA2, 0x9D4F, 0x9CFC, 0x9CA8, 0x9C55, 0x9C02, 0x9BB0, 0x9B5D, 0x9B0A, 0x9AB8, 0x9A66, 0x9A13, 0x99C1,
  0x996F, 0x991E, 0x98CC, 0x987A, 0x9829, 0x97D8, 0x9787, 0x9736, 0x96E5, 0x9694, 0x9643, 0x95F3, 0x95A2, 0x9552, 0x9502, 0x94B2, // 0x08
  0x9462, 0x9412, 0x93C2, 0x9373, 0x9323, 0x92D4, 0x9285, 0x9236, 0x91E7, 0x9198, 0x9149, 0x90FB, 0x90AC, 0x905E, 0x9010, 0x8FC2,
  0x8F74, 0x8F26, 0x8ED8, 0x8E8B, 0x8E3D, 0x8DF0, 0x8DA3, 0x8D56, 0x8D08, 0x8CBC, 0x8C6F, 0x8C22, 0x8BD6, 0x8B89, 0x8B3D, 0x8AF1, // 0x09
  0x8AA4, 0x8A58, 0x8A0D, 0x89C1, 0x8975, 0x892A, 0x88DE, 0x8893, 0x8848, 0x87FD, 0x87B2, 0x8767, 0x871C, 0x86D1, 0x8687, 0x863C,
  0x85F2, 0x85A8, 0x855E, 0x8514, 0x84CA, 0x8481, 0x8437, 0x83ED, 0x83A4, 0x835B, 0x8311, 0x82C8, 0x827F, 0x8236, 0x81EE, 0x81A5, // 0x0A
  0x815C, 0x8114, 0x80CC, 0x8083, 0x803B, 0x7FF3, 0x7FAB, 0x7F63, 0x7F1C, 0x7ED4, 0x7E8D, 0x7E45, 0x7DFE, 0x7DB7, 0x7D70, 0x7D29,
  0x7CE1, 0x7C9A, 0x7C54, 0x7C0D, 0x7BC7, 0x7B80, 0x7B3A, 0x7AF4, 0x7AAE, 0x7A68, 0x7A22, 0x79DC, 0x7996, 0x7951, 0x790B, 0x78C6, // 0x0B
  0x7881, 0x783C, 0x77F6, 0x77B2, 0x776D, 0x7728, 0x76E3, 0x769F, 0x765A, 0x7616, 0x75D1, 0x758D, 0x7549, 0x7505, 0x74C1, 0x747E,
  0x743A, 0x73F7, 0x73B3, 0x7370, 0x732C, 0x72E9, 0x72A6, 0x7263, 0x7220, 0x71DD, 0x719B, 0x7158, 0x7115, 0x70D3, 0x7090, 0x704E, // 0x0C
  0x700C, 0x6FCA, 0x6F88, 0x6F46, 0x6F04, 0x6EC2, 0x6E81, 0x6E3F, 0x6DFE, 0x6DBC, 0x6D7B, 0x6D3A, 0x6CF9, 0x6CB8, 0x6C77, 0x6C36,
  0x6BF5, 0x6BB4, 0x6B74, 0x6B33, 0x6AF3, 0x6AB3, 0x6A73, 0x6A33, 0x69F2, 0x69B3, 0x6973, 0x6933, 0x68F3, 0x68B4, 0x6874, 0x6835, // 0x0D
  0x67F6, 0x67B6, 0x6777, 0x6738, 0x66F9, 0x66BA, 0x667B, 0x663D, 0x65FE, 0x65C0, 0x6581, 0x6543, 0x6505, 0x64C6, 0x6488, 0x644A,
  0x640D, 0x63CF, 0x6391, 0x6354, 0x6316, 0x62D8, 0x629B, 0x625E, 0x6220, 0x61E3, 0x61A6, 0x6169, 0x612C, 0x60EF, 0x60B2, 0x6076, // 0x0E
  0x6039, 0x5FFD, 0x5FC0, 0x5F84, 0x5F48, 0x5F0B, 0x5ECF, 0x5E93, 0x5E57, 0x5E1B, 0x5DDF, 0x5DA4, 0x5D68, 0x5D2C, 0x5CF1, 0x5CB6,
  0x5C7A, 0x5C3F, 0x5C04, 0x5BC8, 0x5B8D, 0x5B53, 0x5B18, 0x5ADD, 0x5AA2, 0x5A68, 0x5A2D, 0x59F2, 0x59B8, 0x597E, 0x5944, 0x5909, // 0x0F
  0x58CF, 0x5895, 0x585B, 0x5821, 0x57E8, 0x57AE, 0x5774, 0x573B, 0x5701, 0x56C8, 0x568F, 0x5655, 0x561C, 0x55E3, 0x55AA, 0x5571,
  0x5538, 0x54FF, 0x54C6, 0x548E, 0x5455, 0x541D, 0x53E4, 0x53AC, 0x5373, 0x533B, 0x5303, 0x52CB, 0x5293, 0x525B, 0x5223, 0x51EB, // 0x10
  0x51B4, 0x517C, 0x5144, 0x510D, 0x50D5, 0x509E, 0x5067, 0x5030, 0x4FF8, 0x4FC1, 0x4F8A, 0x4F53, 0x4F1D, 0x4EE6, 0x4EAF, 0x4E78,
  0x4E42, 0x4E0B, 0x4DD5, 0x4D9E, 0x4D68, 0x4D32, 0x4CFC, 0x4CC5, 0x4C8F, 0x4C59, 0x4C24, 0x4BEE, 0x4BB8, 0x4B82, 0x4B4D, 0x4B17, // 0x11
  0x4AE2, 0x4AAC, 0x4A77, 0x4A42, 0x4A0C, 0x49D7, 0x49A2, 0x496D, 0x4938, 0x4903, 0x48CE, 0x489A, 0x4865, 0x4830, 0x47FC, 0x47C7,
  0x4793, 0x475E, 0x472A, 0x46F6, 0x46C2, 0x468E, 0x465A, 0x4626, 0x45F2, 0x45BE, 0x458A, 0x4556, 0x4523, 0x44EF, 0x44BC, 0x4488, // 0x12
  0x4455, 0x4421, 0x43EE, 0x43BB, 0x4388, 0x4355, 0x4322, 0x42EF, 0x42BC, 0x4289, 0x4256, 0x4224, 0x41F1, 0x41BE, 0x418C, 0x4159,
  0x4127, 0x40F5, 0x40C2, 0x4090, 0x405E, 0x402C, 0x3FFA, 0x3FC8, 0x3F96, 0x3F64, 0x3F32, 0x3F01, 0x3ECF, 0x3E9D, 0x3E6C, 0x3E3A, // 0x13
  0x3E09, 0x3DD8, 0x3DA6, 0x3D75, 0x3D44, 0x3D13, 0x3CE2, 0x3CB1, 0x3C80, 0x3C4F, 0x3C1E, 0x3BED, 0x3BBD, 0x3B8C, 0x3B5B, 0x3B2B,
  0x3AFB, 0x3ACA, 0x3A9A, 0x3A69, 0x3A39, 0x3A09, 0x39D9, 0x39A9, 0x3979, 0x3949, 0x3919, 0x38E9, 0x38B9, 0x388A, 0x385A, 0x382A, // 0x14
  0x37FB, 0x37CB, 0x379C, 0x376C, 0x373D, 0x370E, 0x36DF, 0x36AF, 0x3680, 0x3651, 0x3622, 0x35F3, 0x35C5, 0x3596, 0x3567, 0x3538,
  0x350A, 0x34DB, 0x34AD, 0x347E, 0x3450, 0x3421, 0x33F3, 0x33C5, 0x3396, 0x3368, 0x333A, 0x330C, 0x32DE, 0x32B0, 0x3282, 0x3254, // 0x15
  0x3227, 0x31F9, 0x31CB, 0x319D, 0x3170, 0x3142, 0x3115, 0x30E7, 0x30BA, 0x308D, 0x305F, 0x3032, 0x3005, 0x2FD8, 0x2FAB, 0x2F7E,
  0x2F51, 0x2F24, 0x2EF7, 0x2ECB, 0x2E9E, 0x2E71, 0x2E45, 0x2E18, 0x2DEB, 0x2DBF, 0x2D93, 0x2D66, 0x2D3A, 0x2D0E, 0x2CE1, 0x2CB5, // 0x16
  0x2C89, 0x2C5D, 0x2C31, 0x2C05, 0x2BD9, 0x2BAD, 0x2B81, 0x2B56, 0x2B2A, 0x2AFE, 0x2AD3, 0x2AA7, 0x2A7B, 0x2A50, 0x2A25, 0x29F9,
  0x29CE, 0x29A3, 0x2978, 0x294C, 0x2921, 0x28F6, 0x28CB, 0x28A0, 0x2875, 0x284A, 0x281F, 0x27F5, 0x27CA, 0x279F, 0x2775, 0x274A, // 0x17
  0x271F, 0x26F5, 0x26CA, 0x26A0, 0x2676, 0x264B, 0x2621, 0x25F7, 0x25CD, 0x25A3, 0x2578, 0x254E, 0x2524, 0x24FB, 0x24D1, 0x24A7,
  0x247C, 0x2453, 0x2429, 0x23FF, 0x23D6, 0x23AC, 0x2382, 0x2359, 0x2330, 0x2306, 0x22DD, 0x22B4, 0x228A, 0x2261, 0x2238, 0x220F, // 0x18
  0x21E6, 0x21BD, 0x2194, 0x216B, 0x2142, 0x2119, 0x20F0, 0x20C8, 0x209F, 0x2076, 0x204E, 0x2025, 0x1FFD, 0x1FD4, 0x1FAC, 0x1F83,
  0x1F5B, 0x1F33, 0x1F0B, 0x1EE3, 0x1EBA, 0x1E92, 0x1E6A, 0x1E42, 0x1E1A, 0x1DF2, 0x1DCA, 0x1DA2, 0x1D7B, 0x1D53, 0x1D2B, 0x1D03, // 0x19
  0x1CDC, 0x1CB4, 0x1C8D, 0x1C65, 0x1C3E, 0x1C16, 0x1BEF, 0x1BC8, 0x1BA0, 0x1B79, 0x1B52, 0x1B2B, 0x1B03, 0x1ADC, 0x1AB5, 0x1A8E,
  0x1A68, 0x1A41, 0x1A1A, 0x19F3, 0x19CC, 0x19A5, 0x197F, 0x1958, 0x1931, 0x190B, 0x18E4, 0x18BE, 0x1897, 0x1871, 0x184B, 0x1824, // 0x1A
  0x17FE, 0x17D8, 0x17B1, 0x178B, 0x1765, 0x173F, 0x1719, 0x16F3, 0x16CD, 0x16A7, 0x1681, 0x165B, 0x1635, 0x1610, 0x15EA, 0x15C4,
  0x159E, 0x1578, 0x1553, 0x152D, 0x1508, 0x14E2, 0x14BD, 0x1498, 0x1472, 0x144D, 0x1428, 0x1402, 0x13DD, 0x13B8, 0x1393, 0x136E, // 0x1B
  0x1349, 0x1324, 0x12FF, 0x12DA, 0x12B5, 0x1290, 0x126C, 0x1247, 0x1222, 0x11FD, 0x11D9, 0x11B4, 0x1190, 0x116B, 0x1147, 0x1122,
  0x10FE, 0x10DA, 0x10B6, 0x1091, 0x106D, 0x1049, 0x1025, 0x1000, 0x0FDC, 0x0FB8, 0x0F94, 0x0F70, 0x0F4C, 0x0F28, 0x0F04, 0x0EE1, // 0x1C
  0x0EBD, 0x0E99, 0x0E75, 0x0E52, 0x0E2E, 0x0E0A, 0x0DE7, 0x0DC3, 0x0DA0, 0x0D7C, 0x0D59, 0x0D35, 0x0D12, 0x0CEF, 0x0CCB, 0x0CA8,
  0x0C85, 0x0C62, 0x0C3F, 0x0C1C, 0x0BF9, 0x0BD6, 0x0BB3, 0x0B90, 0x0B6D, 0x0B4A, 0x0B27, 0x0B04, 0x0AE1, 0x0ABF, 0x0A9C, 0x0A79, // 0x1D
  0x0A57, 0x0A34, 0x0A11, 0x09EF, 0x09CC, 0x09AA, 0x0987, 0x0965, 0x0943, 0x0920, 0x08FE, 0x08DC, 0x08B9, 0x0897, 0x0875, 0x0853,
  0x0830, 0x080E, 0x07EC, 0x07CA, 0x07A8, 0x0786, 0x0764, 0x0743, 0x0721, 0x06FF, 0x06DD, 0x06BB, 0x069A, 0x0678, 0x0656, 0x0635, // 0x1E
  0x0613, 0x05F2, 0x05D0, 0x05AF, 0x058D, 0x056C, 0x054B, 0x0529, 0x0508, 0x04E7, 0x04C6, 0x04A4, 0x0483, 0x0462, 0x0441, 0x0420,
  0x03FF, 0x03DF, 0x03BE, 0x039D, 0x037C, 0x035B, 0x033A, 0x0319, 0x02F8, 0x02D8, 0x02B7, 0x0296, 0x0276, 0x0255, 0x0235, 0x0214, // 0x1F
  0x01F4, 0x01D3, 0x01B3, 0x0192, 0x0172, 0x0151, 0x0131, 0x0111, 0x00F1, 0x00D0, 0x00B0, 0x0090, 0x0070, 0x0050, 0x0030, 0x0010
};

// correction table
static Bit8s rcp_table2[1024] = {
  62,  58,  54,  50,  46,  43,  39,  35,  31,  27,  23,  19,  15,  12,   8,   4,
   0,  -4,  -8, -12, -16, -20, -23, -27, -31, -35, -39, -43, -47, -51, -54, -58,
  58,  54,  51,  47,  43,  40,  36,  32,  29,  25,  21,  18,  14,  10,   7,   3,
   0,  -4,  -8, -11, -15, -19, -22, -26, -30, -33, -37, -41, -44, -48, -52, -55,
  55,  52,  48,  45,  41,  38,  34,  31,  27,  24,  21,  17,  14,  10,   7,   3,
   0,  -4,  -7, -10, -14, -17, -21, -24, -28, -31, -35, -38, -41, -45, -48, -52,
  52,  49,  45,  42,  39,  36,  32,  29,  26,  23,  19,  16,  13,  10,   6,   3,
   0,  -3,  -7, -10, -13, -16, -20, -23, -26, -29, -33, -36, -39, -42, -46, -49,
  49,  46,  43,  40,  37,  34,  31,  27,  24,  21,  18,  15,  12,   9,   6,   3,
   0,  -3,  -6,  -9, -13, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46,
  47,  44,  41,  38,  35,  32,  30,  27,  24,  21,  18,  15,  12,   9,   6,   3,
   0,  -3,  -5,  -8, -11, -14, -17, -20, -23, -26, -29, -32, -35, -38, -40, -43,
  44,  41,  38,  36,  33,  30,  27,  25,  22,  19,  16,  14,  11,   8,   5,   3,
   0,  -3,  -6,  -9, -11, -14, -17, -20, -22, -25, -28, -31, -33, -36, -39, -42,
  42,  39,  37,  34,  31,  29,  26,  24,  21,  18,  16,  13,  10,   8,   5,   3,
   0,  -3,  -5,  -8, -11, -13, -16, -18, -21, -24, -26, -29, -32, -34, -37, -39,
  40,  37,  35,  33,  30,  28,  25,  23,  20,  18,  15,  13,  10,   8,   5,   3,
   0,  -2,  -5,  -7, -10, -12, -15, -17, -20, -22, -25, -27, -30, -32, -35, -37,
  38,  36,  33,  31,  28,  26,  24,  21,  19,  17,  14,  12,   9,   7,   5,   2,
   0,  -2,  -5,  -7, -10, -12, -14, -17, -19, -21, -24, -26, -29, -31, -33, -36,
  36,  34,  31,  29,  27,  25,  22,  20,  18,  16,  13,  11,   9,   6,   4,   2,
   0,  -3,  -5,  -7,  -9, -12, -14, -16, -18, -21, -23, -25, -28, -30, -32, -34,
  35,  33,  31,  28,  26,  24,  22,  20,  18,  16,  13,  11,   9,   7,   5,   3,
   0,  -2,  -4,  -6,  -8, -10, -13, -15, -17, -19, -21, -23, -26, -28, -30, -32,
  33,  31,  29,  27,  25,  23,  21,  19,  16,  14,  12,  10,   8,   6,   4,   2,
   0,  -2,  -4,  -6,  -8, -10, -13, -15, -17, -19, -21, -23, -25, -27, -29, -31,
  32,  30,  28,  26,  24,  22,  20,  18,  16,  14,  12,  10,   8,   6,   4,   2,
   0,  -2,  -4,  -6,  -8, -10, -12, -14, -16, -17, -19, -21, -23, -25, -27, -29,
  30,  28,  26,  24,  22,  21,  19,  17,  15,  13,  11,   9,   7,   5,   3,   2,
   0,  -2,  -4,  -6,  -8, -10, -12, -14, -15, -17, -19, -21, -23, -25, -27, -29,
  29,  27,  25,  24,  22,  20,  18,  16,  14,  13,  11,   9,   7,   5,   4,   2,
   0,  -2,  -4,  -6,  -7,  -9, -11, -13, -15, -16, -18, -20, -22, -24, -25, -27,
  28,  26,  25,  23,  21,  19,  18,  16,  14,  12,  11,   9,   7,   5,   4,   2,
   0,  -2,  -3,  -5,  -7,  -9, -10, -12, -14, -16, -17, -19, -21, -23, -24, -26,
  27,  25,  24,  22,  20,  19,  17,  15,  14,  12,  10,   9,   7,   5,   4,   2,
   0,  -1,  -3,  -5,  -6,  -8, -10, -11, -13, -15, -16, -18, -20, -21, -23, -25,
  26,  24,  23,  21,  20,  18,  16,  15,  13,  12,  10,   8,   7,   5,   4,   2,
   0,  -1,  -3,  -5,  -6,  -8,  -9, -11, -13, -14, -16, -17, -19, -21, -22, -24,
  25,  23,  22,  20,  19,  17,  16,  14,  13,  11,  10,   8,   6,   5,   3,   2,
   0,  -1,  -3,  -4,  -6,  -7,  -9, -11, -12, -14, -15, -17, -18, -20, -21, -23,
  24,  23,  21,  20,  18,  17,  15,  14,  12,  11,   9,   8,   6,   5,   3,   2,
   0,  -1,  -3,  -4,  -6,  -7,  -9, -10, -12, -13, -15, -16, -18, -19, -21, -22,
  23,  22,  20,  19,  17,  16,  14,  13,  12,  10,   9,   7,   6,   4,   3,   2,
   0,  -1,  -3,  -4,  -6,  -7,  -8, -10, -11, -13, -14, -16, -17, -19, -20, -21,
  22,  21,  19,  18,  16,  15,  14,  12,  11,  10,   8,   7,   5,   4,   3,   1,
   0,  -1,  -3,  -4,  -6,  -7,  -8, -10, -11, -12, -14, -15, -17, -18, -19, -21,
  21,  20,  18,  17,  16,  14,  13,  12,  10,   9,   8,   6,   5,   4,   2,   1,
   0,  -2,  -3,  -4,  -6,  -7,  -8, -10, -11, -12, -14, -15, -16, -18, -19, -20,
  21,  20,  18,  17,  16,  15,  13,  12,  11,   9,   8,   7,   6,   4,   3,   2,
   0,  -1,  -2,  -3,  -5,  -6,  -7,  -9, -10, -11, -12, -14, -15, -16, -18, -19,
  20,  19,  18,  16,  15,  14,  13,  11,  10,   9,   8,   6,   5,   4,   3,   1,
   0,  -1,  -2,  -4,  -5,  -6,  -7,  -9, -10, -11, -12, -13, -15, -16, -17, -18,
  19,  18,  17,  15,  14,  13,  12,  11,   9,   8,   7,   6,   5,   3,   2,   1,
   0,  -1,  -3,  -4,  -5,  -6,  -7,  -9, -10, -11, -12, -13, -15, -16, -17, -18,
  19,  18,  17,  16,  14,  13,  12,  11,  10,   9,   7,   6,   5,   4,   3,   2,
   0,  -1,  -2,  -3,  -4,  -5,  -6,  -8, -9,  -10, -11, -12, -13, -15, -16, -17,
  18,  17,  16,  15,  14,  12,  11,  10,  9,    8,   7,   6,   5,   3,   2,   1,
   0,  -1,  -2,  -3,  -4,  -6,  -7,  -8, -9,  -10, -11, -12, -13, -14, -16, -17,
  17,  16,  15,  14,  13,  12,  10,   9,  8,    7,   6,   5,   4,   3,   2,   1,
   0,  -1,  -3,  -4,  -5,  -6,  -7,  -8, -9,  -10, -11, -12, -13, -14, -15, -17,
  17,  16,  15,  14,  13,  12,  11,  10,  9,    8,   7,   5,   4,   3,   2,   1,
   0,  -1,  -2,  -3,  -4,  -5,  -6,  -7, -8,   -9, -10, -11, -12, -13, -14, -16,
  16,  15,  14,  13,  12,  11,  10,   9,  8,    7,   6,   5,   4,   3,   2,   1,
   0,  -1,  -2,  -3,  -4,  -5,  -6,  -7, -8,   -9, -10, -11, -12, -13, -14, -15
};

// approximate reciprocal of scalar single precision FP with 14-bit precision
float32 approximate_rcp_3dnow(float32 op)
{
  softfloat_class_t op_class = f32_class(op);
  bool sign = f32_sign(op);

  // Note that NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if argument is NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  switch(op_class) {
    case softfloat_zero:             // return maximum normal value with sign of the argument
    case softfloat_denormal:
      return packFloat32(sign, 0x7F, 0x7FFFFF);

    case softfloat_negative_inf:     // this is that actual AMD hardware returns
    case softfloat_positive_inf:
      return packFloat32(sign, 0x7F, 0x7FFE00);

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    case softfloat_normalized:
      break;
  }

  Bit32u fraction = f32_fraction(op);
  Bit16s exp = f32_exp(op);

  exp = 2 * FLOAT32_EXP_BIAS - 1 - exp;

  /* check for underflow */
  if (exp <= 0)
      return packFloat32(sign, 0, 0);

  // Approximate 1/(1.b1b2b3..bn) with T1(b1..bk..bm) + T2(b1..bk,bm+1..bn)

  // Table 1 is indexed with bits[b1..b5][b6...b10]
  // Table 2 is indexed with bits[b1..b5][b11..b15]

  Bit32u fraction_b1_b5   = (fraction >> 18) & 0x01F;
  Bit32u fraction_b1_b10  = (fraction >> 13) & 0x3FF;
  Bit32u fraction_b11_b15 = (fraction >>  8) & 0x01F;

  return packFloat32(sign, exp, Bit32u(int(rcp_table1[fraction_b1_b10]) + int(rcp_table2[(fraction_b1_b5 << 5) | fraction_b11_b15])) << 7);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCP_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister dst;
  float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    dst = BX_READ_MMX_REG(i->src());
    op = MMXUD0(dst);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  op = f32_denormal_to_zero(op);

  // for zero argument produce maximum normal value with the sign of source operand
  if ((op << 1) == 0) {
    op = packFloat32(f32_sign(op), 0x7F, 0x7FFFFF);
  }
  else {
    // Note that Inf/NaN handling is not documented in 3Dnow! manuals
    // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
    op = approximate_rcp_3dnow(op);
  }

  MMXUD0(dst) = op;
  MMXUD1(dst) = op;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), dst);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCPIT1_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  // Compute first step of Newton-Raphson Approximation iteration
  // One iteration of the 1/b reciprocal approximation is:
  //       X1 = X0 * (2 - b * X0)

  // Use FMA call to compute (2 - b * X0) as the first step

  // Second step in instruction PFRCPIT2 just has to multiply arguments to complete approximation
  // PFRCPIT2 is aliased to PFMUL and doesn't have Bochs handler

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even);
  const float32 f32_two = 0x40000000;

  MMXUD0(op1) = f32_fnmadd(MMXUD0(op1), MMXUD0(op2), f32_two, &status);
  MMXUD1(op1) = f32_fnmadd(MMXUD1(op1), MMXUD1(op2), f32_two, &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQRT_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister dst;
  float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    dst = BX_READ_MMX_REG(i->src());
    op = MMXUD0(dst);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  op = f32_denormal_to_zero(op);

  // Negative operands are treated as positive operands for purposes of reciprocal square
  // root computation, with the sign of the result the same as the sign of the source operand
  Bit32u sign = op & 0x80000000; // remember the sign

  op &= 0x7FFFFFFF; // reset the sign for now

  // for zero argument produce maximum normal value with the sign of source operand
  if (op == 0) {
    op = sign ^ packFloat32(0, 0x7F, 0x7FFFFF);
  }
  else {
    // Note that Inf/NaN handling is not documented in 3Dnow! manuals
    // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
    op = sign ^ approximate_rsqrt(op);
  }

  MMXUD0(dst) = op;
  MMXUD1(dst) = op;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), dst);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQIT1_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  // Compute first step of Newton-Raphson Approximation iteration
  // One iteration of the sqrt(1/b) reciprocal approximation is:
  //       X1 = 0.5 * X0 * (3 - b * X0^2)

  // Usage for the instruction to compute a = 1/sqrt(b):

  //    X0 = PFRSQRT(b)                 initial approximation for 1/sqrt(b)
  //    X1 = PFMUL(X0, X0)              compute X0^2
  //    X2 = PFRSQIT1(b,X1)             this instruction compute 1st step of approximation 0.5 * (3 - b * X0^2)
  //                                    from arguments provided to the instruction are: X0^2 and b
  //     a = PFRCPIT2(X2,X0)            multiply by X0

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even);
  const float32 f32_three = 0x40400000, f32_half = 0x3f000000;

  // Use FMA call to compute (3 - b * X0^2) as the first step
  MMXUD0(op1) = f32_fnmadd(MMXUD0(op1), MMXUD0(op2), f32_three, &status);
  MMXUD1(op1) = f32_fnmadd(MMXUD1(op1), MMXUD1(op2), f32_three, &status);

  // Multiply result by 0.5
  MMXUD0(op1) = f32_mul(MMXUD0(op1), f32_half, &status);
  MMXUD1(op1) = f32_mul(MMXUD1(op1), f32_half, &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

#endif
