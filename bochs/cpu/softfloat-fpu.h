/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2b, written for Bochs (x86 achitecture simulator)
floating point emulation.

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
 *            Stanislav Shwartsman (gate at fidonet.org.il)
 * ==========================================================================*/ 

#include "softfloat.h"

/*----------------------------------------------------------------------------
| Functions and definitions to determine:  (1) whether tininess for underflow
| is detected before or after rounding by default, (2) what (if anything)
| happens when exceptions are raised, (3) how signaling NaNs are distinguished
| from quiet NaNs, (4) the default generated quiet NaNs, and (5) how NaNs
| are propagated from function inputs to output.  These details are target-
| specific.
*----------------------------------------------------------------------------*/
#include "softfloat-specialize.h"

/*-----------------------------------------------------------------------------
| Calculates the absolute value of the extended double-precision floating-point
| value `a'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE floatx80& floatx80_abs(floatx80 &reg)
{
    reg.exp &= 0x7FFF;
    return reg;   
}

/*-----------------------------------------------------------------------------
| Changes the sign of the extended double-precision floating-point value 'a'.
| The operation is performed according to the IEC/IEEE Standard for Binary 
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE floatx80& floatx80_chs(floatx80 &reg)
{
    reg.exp ^= 0x8000;
    return reg;   
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 16-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic - which means in particular that the conversion
| is rounded according to the current rounding mode. If `a' is a NaN or the 
| conversion overflows, the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE Bit16s floatx80_to_int16(floatx80 a, float_status_t &status)
{
   Bit32s v32 = floatx80_to_int32(a, status);

   if ((v32 > (Bit32s) BX_MAX_BIT16S) || (v32 < (Bit32s) BX_MIN_BIT16S))
   {
        float_raise(status, float_flag_invalid);
        return int16_indefinite;
   }

   return (Bit16s) v32;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 16-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic, except that the conversion is always rounded
| toward zero.  If `a' is a NaN or the conversion overflows, the integer 
| indefinite value is returned.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE Bit16s floatx80_to_int16_round_to_zero(floatx80 a, float_status_t &status)
{
   Bit32s v32 = floatx80_to_int32_round_to_zero(a, status);

   if ((v32 > (Bit32s) BX_MAX_BIT16S) || (v32 < (Bit32s) BX_MIN_BIT16S))
   {
        float_raise(status, float_flag_invalid);
        return int16_indefinite;
   }

   return (Bit16s) v32;
}
