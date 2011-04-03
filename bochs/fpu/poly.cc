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
 * Written for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#define FLOAT128

#include <assert.h>
#include "softfloat.h"

//                            2         3         4               n
// f(x) ~ C + (C * x) + (C * x) + (C * x) + (C * x) + ... + (C * x)
//         0    1         2         3         4               n
//
//          --       2k                --        2k+1
//   p(x) = >  C  * x           q(x) = >  C   * x
//          --  2k                     --  2k+1
//
//   f(x) ~ [ p(x) + x * q(x) ]
//

float128 EvalPoly(float128 x, float128 *arr, unsigned n, float_status_t &status)
{
    float128 x2 = float128_mul(x, x, status);
    unsigned i;

    assert(n > 1);

    float128 r1 = arr[--n];
    i = n;
    while(i >= 2) {
        r1 = float128_mul(r1, x2, status);
        i -= 2;
        r1 = float128_add(r1, arr[i], status);
    }
    if (i) r1 = float128_mul(r1, x, status);

    float128 r2 = arr[--n];
    i = n;
    while(i >= 2) {
        r2 = float128_mul(r2, x2, status);
        i -= 2;
        r2 = float128_add(r2, arr[i], status);
    }
    if (i) r2 = float128_mul(r2, x, status);

    return float128_add(r1, r2, status);
}

//                  2         4         6         8               2n
// f(x) ~ C + (C * x) + (C * x) + (C * x) + (C * x) + ... + (C * x)
//         0    1         2         3         4               n
//
//          --       4k                --        4k+2
//   p(x) = >  C  * x           q(x) = >  C   * x
//          --  2k                     --  2k+1
//
//                    2
//   f(x) ~ [ p(x) + x * q(x) ]
//

float128 EvenPoly(float128 x, float128 *arr, unsigned n, float_status_t &status)
{
     return EvalPoly(float128_mul(x, x, status), arr, n, status);
}

//                        3         5         7         9               2n+1
// f(x) ~ (C * x) + (C * x) + (C * x) + (C * x) + (C * x) + ... + (C * x)
//          0         1         2         3         4               n
//                        2         4         6         8               2n
//      = x * [ C + (C * x) + (C * x) + (C * x) + (C * x) + ... + (C * x)
//               0    1         2         3         4               n
//
//          --       4k                --        4k+2
//   p(x) = >  C  * x           q(x) = >  C   * x
//          --  2k                     --  2k+1
//
//                        2
//   f(x) ~ x * [ p(x) + x * q(x) ]
//

float128 OddPoly(float128 x, float128 *arr, unsigned n, float_status_t &status)
{
     return float128_mul(x, EvenPoly(x, arr, n, status), status);
}
