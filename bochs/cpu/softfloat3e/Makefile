#=============================================================================
#
# This Makefile is part of the SoftFloat IEEE Floating-Point Arithmetic
# Package, Release 3e, by John R. Hauser.
#
# Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
# University of California.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions, and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions, and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  3. Neither the name of the University nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
# DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#=============================================================================

SOURCE_DIR ?= .
SPECIALIZE_TYPE ?= 8086-SSE

SOFTFLOAT_OPTS ?= \
  -DINLINE_LEVEL=5 -DSOFTFLOAT_FAST_DIV32TO16 -DSOFTFLOAT_FAST_DIV64TO32

DELETE = rm -f
C_INCLUDES = -I. -I../.. -I$(SOURCE_DIR)/$(SPECIALIZE_TYPE) -I$(SOURCE_DIR)/include
COMPILE_C = \
  g++ -c  \
    -DSOFTFLOAT_FAST_INT64 $(SOFTFLOAT_OPTS) $(C_INCLUDES) -O2 -o $@
MAKELIB = ar crs $@

OBJ = .o
LIB = .a

.PHONY: all
all: libsoftfloat$(LIB)

OBJS_PRIMITIVES = \
  s_eq128$(OBJ) \
  s_le128$(OBJ) \
  s_lt128$(OBJ) \
  s_shortShiftLeft128$(OBJ) \
  s_shortShiftRight128$(OBJ) \
  s_shortShiftRightJam64$(OBJ) \
  s_shortShiftRightJam64Extra$(OBJ) \
  s_shortShiftRightJam128$(OBJ) \
  s_shortShiftRightJam128Extra$(OBJ) \
  s_shiftRightJam32$(OBJ) \
  s_shiftRightJam64$(OBJ) \
  s_shiftRightJam64Extra$(OBJ) \
  s_shiftRightJam128$(OBJ) \
  s_shiftRightJam128Extra$(OBJ) \
  s_shiftRightJam256M$(OBJ) \
  s_countLeadingZeros8$(OBJ) \
  s_countLeadingZeros16$(OBJ) \
  s_countLeadingZeros32$(OBJ) \
  s_countLeadingZeros64$(OBJ) \
  s_add128$(OBJ) \
  s_add256M$(OBJ) \
  s_sub128$(OBJ) \
  s_sub256M$(OBJ) \
  s_mul64ByShifted32To128$(OBJ) \
  s_mul64To128$(OBJ) \
  s_mul128By32$(OBJ) \
  s_mul128To256M$(OBJ) \
  s_approxRecip_1Ks$(OBJ) \
  s_approxRecipSqrt_1Ks$(OBJ) \
  s_approxRecipSqrt32_1$(OBJ) \

OBJS_SPECIALIZE = \
  s_commonNaNToF16UI$(OBJ) \
  s_propagateNaNF16UI$(OBJ) \
  s_commonNaNToF32UI$(OBJ) \
  s_propagateNaNF32UI$(OBJ) \
  s_commonNaNToF64UI$(OBJ) \
  s_propagateNaNF64UI$(OBJ) \
  s_commonNaNToExtF80UI$(OBJ) \
  s_propagateNaNExtF80UI$(OBJ) \
  s_commonNaNToF128UI$(OBJ) \
  s_propagateNaNF128UI$(OBJ) \

OBJS_OTHERS = \
  s_roundToUI32$(OBJ) \
  s_roundToUI64$(OBJ) \
  s_roundToI32$(OBJ) \
  s_roundToI64$(OBJ) \
  s_normSubnormalF16Sig$(OBJ) \
  s_roundPackToF16$(OBJ) \
  s_normRoundPackToF16$(OBJ) \
  s_addMagsF16$(OBJ) \
  s_subMagsF16$(OBJ) \
  s_mulAddF16$(OBJ) \
  s_normSubnormalF32Sig$(OBJ) \
  s_roundPackToF32$(OBJ) \
  s_normRoundPackToF32$(OBJ) \
  s_addMagsF32$(OBJ) \
  s_subMagsF32$(OBJ) \
  s_mulAddF32$(OBJ) \
  s_normSubnormalF64Sig$(OBJ) \
  s_roundPackToF64$(OBJ) \
  s_normRoundPackToF64$(OBJ) \
  s_addMagsF64$(OBJ) \
  s_subMagsF64$(OBJ) \
  s_mulAddF64$(OBJ) \
  s_normSubnormalExtF80Sig$(OBJ) \
  s_roundPackToExtF80$(OBJ) \
  s_normRoundPackToExtF80$(OBJ) \
  s_addMagsExtF80$(OBJ) \
  s_subMagsExtF80$(OBJ) \
  s_normSubnormalF128Sig$(OBJ) \
  s_roundPackToF128$(OBJ) \
  s_normRoundPackToF128$(OBJ) \
  s_addMagsF128$(OBJ) \
  s_subMagsF128$(OBJ) \
  s_mulAddF128$(OBJ) \
  isNaN$(OBJ) \
  isSignalingNaN$(OBJ) \
  ui32_to_f16$(OBJ) \
  ui32_to_f32$(OBJ) \
  ui32_to_f64$(OBJ) \
  ui32_to_extF80$(OBJ) \
  ui32_to_f128$(OBJ) \
  ui64_to_f16$(OBJ) \
  ui64_to_f32$(OBJ) \
  ui64_to_f64$(OBJ) \
  ui64_to_extF80$(OBJ) \
  ui64_to_f128$(OBJ) \
  i32_to_f16$(OBJ) \
  i32_to_f32$(OBJ) \
  i32_to_f64$(OBJ) \
  i32_to_extF80$(OBJ) \
  i32_to_f128$(OBJ) \
  i64_to_f16$(OBJ) \
  i64_to_f32$(OBJ) \
  i64_to_f64$(OBJ) \
  i64_to_extF80$(OBJ) \
  i64_to_f128$(OBJ) \
  f16_to_ui32$(OBJ) \
  f16_to_ui64$(OBJ) \
  f16_to_i32$(OBJ) \
  f16_to_i64$(OBJ) \
  f16_to_ui32_r_minMag$(OBJ) \
  f16_to_ui64_r_minMag$(OBJ) \
  f16_to_i32_r_minMag$(OBJ) \
  f16_to_i64_r_minMag$(OBJ) \
  f16_to_f32$(OBJ) \
  f16_to_f64$(OBJ) \
  f16_to_extF80$(OBJ) \
  f16_getExp$(OBJ) \
  f16_getMant$(OBJ) \
  f16_roundToInt$(OBJ) \
  f16_class$(OBJ) \
  f16_compare$(OBJ) \
  f16_minmax$(OBJ) \
  f16_add$(OBJ) \
  f16_sub$(OBJ) \
  f16_mul$(OBJ) \
  f16_mulAdd$(OBJ) \
  f16_div$(OBJ) \
  f16_sqrt$(OBJ) \
  f32_to_ui32$(OBJ) \
  f32_to_ui64$(OBJ) \
  f32_to_i32$(OBJ) \
  f32_to_i64$(OBJ) \
  f32_to_ui32_r_minMag$(OBJ) \
  f32_to_ui64_r_minMag$(OBJ) \
  f32_to_i32_r_minMag$(OBJ) \
  f32_to_i64_r_minMag$(OBJ) \
  f32_to_f16$(OBJ) \
  f32_to_f64$(OBJ) \
  f32_to_extF80$(OBJ) \
  f32_to_f128$(OBJ) \
  f32_getExp$(OBJ) \
  f32_getMant$(OBJ) \
  f32_roundToInt$(OBJ) \
  f32_class$(OBJ) \
  f32_compare$(OBJ) \
  f32_minmax$(OBJ) \
  f32_scalef$(OBJ) \
  f32_frc$(OBJ) \
  f32_add$(OBJ) \
  f32_sub$(OBJ) \
  f32_mul$(OBJ) \
  f32_mulAdd$(OBJ) \
  f32_div$(OBJ) \
  f32_sqrt$(OBJ) \
  f64_to_ui32$(OBJ) \
  f64_to_ui64$(OBJ) \
  f64_to_i32$(OBJ) \
  f64_to_i64$(OBJ) \
  f64_to_ui32_r_minMag$(OBJ) \
  f64_to_ui64_r_minMag$(OBJ) \
  f64_to_i32_r_minMag$(OBJ) \
  f64_to_i64_r_minMag$(OBJ) \
  f64_to_f16$(OBJ) \
  f64_to_f32$(OBJ) \
  f64_to_extF80$(OBJ) \
  f64_to_f128$(OBJ) \
  f64_getExp$(OBJ) \
  f64_getMant$(OBJ) \
  f64_roundToInt$(OBJ) \
  f64_class$(OBJ) \
  f64_compare$(OBJ) \
  f64_minmax$(OBJ) \
  f64_scalef$(OBJ) \
  f64_frc$(OBJ) \
  f64_add$(OBJ) \
  f64_sub$(OBJ) \
  f64_mul$(OBJ) \
  f64_mulAdd$(OBJ) \
  f64_div$(OBJ) \
  f64_sqrt$(OBJ) \
  extF80_to_ui32$(OBJ) \
  extF80_to_ui64$(OBJ) \
  extF80_to_i32$(OBJ) \
  extF80_to_i64$(OBJ) \
  extF80_to_ui32_r_minMag$(OBJ) \
  extF80_to_ui64_r_minMag$(OBJ) \
  extF80_to_i32_r_minMag$(OBJ) \
  extF80_to_i64_r_minMag$(OBJ) \
  extF80_to_f16$(OBJ) \
  extF80_to_f32$(OBJ) \
  extF80_to_f64$(OBJ) \
  extF80_to_f128$(OBJ) \
  extF80_roundToInt$(OBJ) \
  extF80_add$(OBJ) \
  extF80_sub$(OBJ) \
  extF80_mul$(OBJ) \
  extF80_div$(OBJ) \
  extF80_rem$(OBJ) \
  extF80_sqrt$(OBJ) \
  f128_to_ui32$(OBJ) \
  f128_to_ui64$(OBJ) \
  f128_to_i32$(OBJ) \
  f128_to_i64$(OBJ) \
  f128_to_ui32_r_minMag$(OBJ) \
  f128_to_ui64_r_minMag$(OBJ) \
  f128_to_i32_r_minMag$(OBJ) \
  f128_to_i64_r_minMag$(OBJ) \
  f128_to_f32$(OBJ) \
  f128_to_extF80$(OBJ) \
  f128_to_f64$(OBJ) \
  f128_roundToInt$(OBJ) \
  f128_add$(OBJ) \
  f128_sub$(OBJ) \
  f128_mul$(OBJ) \
  f128_mulAdd$(OBJ) \
  f128_div$(OBJ) \
  f128_sqrt$(OBJ)

OBJS_ALL = $(OBJS_PRIMITIVES) $(OBJS_SPECIALIZE) $(OBJS_OTHERS)

$(OBJS_ALL): \
  include/platform.h $(SOURCE_DIR)/include/primitiveTypes.h \
  $(SOURCE_DIR)/include/primitives.h
$(OBJS_SPECIALIZE) $(OBJS_OTHERS): \
  $(SOURCE_DIR)/include/softfloat_types.h $(SOURCE_DIR)/include/internals.h \
  $(SOURCE_DIR)/$(SPECIALIZE_TYPE)/specialize.h \
  $(SOURCE_DIR)/include/softfloat.h

$(OBJS_PRIMITIVES) $(OBJS_OTHERS): %$(OBJ): $(SOURCE_DIR)/%.c
	$(COMPILE_C) $(SOURCE_DIR)/$*.c

$(OBJS_SPECIALIZE): %$(OBJ): $(SOURCE_DIR)/$(SPECIALIZE_TYPE)/%.c
	$(COMPILE_C) $(SOURCE_DIR)/$(SPECIALIZE_TYPE)/$*.c

libsoftfloat$(LIB): $(OBJS_ALL)
	$(DELETE) $@
	$(MAKELIB) $^

.PHONY: clean
clean:
dist-clean:
	$(DELETE) $(OBJS_ALL) libsoftfloat$(LIB)

