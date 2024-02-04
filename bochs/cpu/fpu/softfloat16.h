/*============================================================================
This C header file is part of the SoftFloat IEC/IEEE Floating-point Arithmetic
Package, Release 2b.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page `http://www.cs.berkeley.edu/~jhauser/
arithmetic/SoftFloat.html'.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.
=============================================================================*/

/*============================================================================
 * Adapted for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#ifndef _SOFTFLOAT_F16_HELPERS_H_
#define _SOFTFLOAT_F16_HELPERS_H_

#include "softfloat-specialize.h"

extern float32 convert_ne_fp16_to_fp32(float16 op);

BX_CPP_INLINE float16 int16_to_float16(Bit16s val_16, float_status_t &status)
{
  return i32_to_f16((Bit32s)val_16, &status);
}

BX_CPP_INLINE float16 uint16_to_float16(Bit16u val_16, float_status_t &status)
{
  return ui32_to_f16((Bit32u)val_16, &status);
}

BX_CPP_INLINE Bit16s float16_to_int16(float16 a, float_status_t &status)
{
  Bit32s val_32 = f16_to_i32(a, &status);
  Bit16s val_16 = (Bit16s) val_32;
  if ((Bit32s)(val_16) != val_32) {
    status.float_exception_flags = float_flag_invalid; // throw away other flags
    return int16_indefinite;
  }
  return val_16;
}

BX_CPP_INLINE Bit16s float16_to_int16_round_to_zero(float16 a, float_status_t &status)
{
  Bit32s val_32 = f16_to_i32_round_to_zero(a, &status);
  Bit16s val_16 = (Bit16s) val_32;
  if ((Bit32s)(val_16) != val_32) {
    status.float_exception_flags = float_flag_invalid; // throw away other flags
    return int16_indefinite;
  }
  return val_16;
}

BX_CPP_INLINE Bit16u float16_to_uint16(float16 a, float_status_t &status)
{
  Bit32u val_32 = f16_to_ui32(a, &status);
  if (val_32 > 0xffff) {
    status.float_exception_flags = float_flag_invalid; // throw away other flags
    return uint16_indefinite;
  }
  return (Bit16u)val_32;
}

BX_CPP_INLINE Bit16u float16_to_uint16_round_to_zero(float16 a, float_status_t &status)
{
  Bit32u val_32 = f16_to_ui32_round_to_zero(a, &status);
  if (val_32 > 0xffff) {
    status.float_exception_flags = float_flag_invalid; // throw away other flags
    return uint16_indefinite;
  }
  return (Bit16u)val_32;
}

BX_CPP_INLINE float16 float16_add(float16 a, float16 b, float_status_t &status)
{
  return float32_to_float16(float32_add(convert_ne_fp16_to_fp32(a), convert_ne_fp16_to_fp32(b), status), status);
}

BX_CPP_INLINE float16 float16_sub(float16 a, float16 b, float_status_t &status)
{
  return float32_to_float16(float32_sub(convert_ne_fp16_to_fp32(a), convert_ne_fp16_to_fp32(b), status), status);
}

BX_CPP_INLINE float16 float16_mul(float16 a, float16 b, float_status_t &status)
{
  return float32_to_float16(float32_mul(convert_ne_fp16_to_fp32(a), convert_ne_fp16_to_fp32(b), status), status);
}

BX_CPP_INLINE float16 float16_div(float16 a, float16 b, float_status_t &status)
{
  return float32_to_float16(float32_div(convert_ne_fp16_to_fp32(a), convert_ne_fp16_to_fp32(b), status), status);
}

BX_CPP_INLINE float16 float16_scalef(float16 a, float16 b, float_status_t &status)
{
  return float32_to_float16(float32_scalef(convert_ne_fp16_to_fp32(a), convert_ne_fp16_to_fp32(b), status), status);
}

BX_CPP_INLINE float16 float16_getmant(float16 a, float_status_t &status, int sign_ctrl, int interv)
{
  return float32_to_float16(float32_getmant(convert_ne_fp16_to_fp32(a), status, sign_ctrl, interv), status);
}

#endif
