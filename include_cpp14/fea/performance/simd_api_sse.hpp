/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "simd_api_mmx.hpp"
#include "fea/performance/simd_register.hpp"
#include "fea/performance/simd_register_traits.hpp"
#include "fea/performance/simd_version.hpp"
#include "fea/utils/platform.hpp"

#include <immintrin.h>

/*
SIMD intrinsics, sorted and grouped by abi support.

All APIs and information come from :
https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
*/

namespace fea {


template <>
struct simd_api<fea::simd_ver::sse> 
		: public simd_api<fea::simd_ver::mmx>
{
// Default register types for this api.
// Previous registers and functionality are still available.
using xmm_f32_t = simd_abi_register_t<fea::simd_ver::sse, float>;
using xmm_f64_t = simd_abi_register_t<fea::simd_ver::sse, double>;
using xmm_i8_t = simd_abi_register_t<fea::simd_ver::sse, char>;
using xmm_u8_t = simd_abi_register_t<fea::simd_ver::sse, unsigned char>;
using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::sse, signed char>;
using xmm_i16_t = simd_abi_register_t<fea::simd_ver::sse, short>;
using xmm_u16_t = simd_abi_register_t<fea::simd_ver::sse, unsigned short>;
using xmm_i32_t = simd_abi_register_t<fea::simd_ver::sse, int>;
using xmm_u32_t = simd_abi_register_t<fea::simd_ver::sse, unsigned int>;
using xmm_i64_t = simd_abi_register_t<fea::simd_ver::sse, long long>;
using xmm_u64_t = simd_abi_register_t<fea::simd_ver::sse, unsigned long long>;



/*
Macro: Get the exception mask bits from the MXCSR control and status register.
The exception mask may contain any of the following flags: _MM_MASK_INVALID,
_MM_MASK_DIV_ZERO, _MM_MASK_DENORM, _MM_MASK_OVERFLOW, _MM_MASK_UNDERFLOW,
_MM_MASK_INEXACT


dst[31:0] := MXCSR & _MM_MASK_MASK

*/
FEA_FORCEINLINE static unsigned int get_exception_mask() {
	return _MM_GET_EXCEPTION_MASK();
}

/*
Macro: Get the exception state bits from the MXCSR control and status
register. The exception state may contain any of the following flags:
_MM_EXCEPT_INVALID, _MM_EXCEPT_DIV_ZERO, _MM_EXCEPT_DENORM,
_MM_EXCEPT_OVERFLOW, _MM_EXCEPT_UNDERFLOW, _MM_EXCEPT_INEXACT


dst[31:0] := MXCSR & _MM_EXCEPT_MASK

*/
FEA_FORCEINLINE static unsigned int get_exception_state() {
	return _MM_GET_EXCEPTION_STATE();
}

/*
Macro: Get the flush zero bits from the MXCSR control and status register. The
flush zero may contain any of the following flags: _MM_FLUSH_ZERO_ON or
_MM_FLUSH_ZERO_OFF


dst[31:0] := MXCSR & _MM_FLUSH_MASK

*/
FEA_FORCEINLINE static unsigned int get_flush_zero_mode() {
	return _MM_GET_FLUSH_ZERO_MODE();
}

/*
Macro: Get the rounding mode bits from the MXCSR control and status register.
The rounding mode may contain any of the following flags: _MM_ROUND_NEAREST,
_MM_ROUND_DOWN, _MM_ROUND_UP, _MM_ROUND_TOWARD_ZERO


dst[31:0] := MXCSR & _MM_ROUND_MASK

*/
FEA_FORCEINLINE static unsigned int get_rounding_mode() {
	return _MM_GET_ROUNDING_MODE();
}

/*
Macro: Set the exception mask bits of the MXCSR control and status register to
the value in unsigned 32-bit integer "a". The exception mask may contain any
of the following flags: _MM_MASK_INVALID, _MM_MASK_DIV_ZERO, _MM_MASK_DENORM,
_MM_MASK_OVERFLOW, _MM_MASK_UNDERFLOW, _MM_MASK_INEXACT


MXCSR := a[31:0] AND ~_MM_MASK_MASK

*/
FEA_FORCEINLINE static void set_exception_mask(unsigned int a) {
	_MM_SET_EXCEPTION_MASK(a);
}

/*
Macro: Set the exception state bits of the MXCSR control and status register
to the value in unsigned 32-bit integer "a". The exception state may contain
any of the following flags: _MM_EXCEPT_INVALID, _MM_EXCEPT_DIV_ZERO,
_MM_EXCEPT_DENORM, _MM_EXCEPT_OVERFLOW, _MM_EXCEPT_UNDERFLOW,
_MM_EXCEPT_INEXACT


MXCSR := a[31:0] AND ~_MM_EXCEPT_MASK

*/
FEA_FORCEINLINE static void set_exception_state(unsigned int a) {
	_MM_SET_EXCEPTION_STATE(a);
}

/*
Macro: Set the flush zero bits of the MXCSR control and status register to the
value in unsigned 32-bit integer "a". The flush zero may contain any of the
following flags: _MM_FLUSH_ZERO_ON or _MM_FLUSH_ZERO_OFF


MXCSR := a[31:0] AND ~_MM_FLUSH_MASK

*/
FEA_FORCEINLINE static void set_flush_zero_mode(unsigned int a) {
	_MM_SET_FLUSH_ZERO_MODE(a);
}

/*
Macro: Set the rounding mode bits of the MXCSR control and status register to
the value in unsigned 32-bit integer "a". The rounding mode may contain any
of the following flags: _MM_ROUND_NEAREST, _MM_ROUND_DOWN, _MM_ROUND_UP,
_MM_ROUND_TOWARD_ZERO


MXCSR := a[31:0] AND ~_MM_ROUND_MASK

*/
FEA_FORCEINLINE static void set_rounding_mode(unsigned int a) {
	_MM_SET_ROUNDING_MODE(a);
}

/*
Macro: Transpose the 4x4 matrix formed by the 4 rows of single-precision
(32-bit) floating-point elements in "row0", "row1", "row2", and "row3", and
store the transposed matrix in these vectors ("row0" now contains column 0,
etc.).

SEQUENCE

__m128 tmp3, tmp2, tmp1, tmp0;
tmp0 := _mm_unpacklo_ps(row0, row1);
tmp2 := _mm_unpacklo_ps(row2, row3);
tmp1 := _mm_unpackhi_ps(row0, row1);
tmp3 := _mm_unpackhi_ps(row2, row3);
row0 := _mm_movelh_ps(tmp0, tmp2);
row1 := _mm_movehl_ps(tmp2, tmp0);
row2 := _mm_movelh_ps(tmp1, tmp3);
row3 := _mm_movehl_ps(tmp3, tmp1);

*/
FEA_FORCEINLINE static void transpose4(m128_f32_t row0, m128_f32_t row1, m128_f32_t row2, m128_f32_t row3) {
	_MM_TRANSPOSE4_PS(row0.xmm, row1.xmm, row2.xmm, row3.xmm);
}

/*
Add packed single-precision (32-bit) floating-point elements in "a" and "b",
and store the results in "dst".

addps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[i+31:i] + b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t add(m128_f32_t a, m128_f32_t b) {
	return _mm_add_ps(a.xmm, b.xmm);
}

/*
Add the lower single-precision (32-bit) floating-point element in "a" and "b",
store the result in the lower element of "dst", and copy the upper 3 packed
elements from "a" to the upper elements of "dst".

addss xmm, xmm

dst[31:0] := a[31:0] + b[31:0]
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t add_s(m128_f32_t a, m128_f32_t b) {
	return _mm_add_ss(a.xmm, b.xmm);
}

/*
Compute the bitwise AND of packed single-precision (32-bit) floating-point
elements in "a" and "b", and store the results in "dst".

andps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (a[i+31:i] AND b[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t and_(m128_f32_t a, m128_f32_t b) {
	return _mm_and_ps(a.xmm, b.xmm);
}

/*
Compute the bitwise NOT of packed single-precision (32-bit) floating-point
elements in "a" and then AND with "b", and store the results in "dst".

andnps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ((NOT a[i+31:i]) AND b[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t andnot(m128_f32_t a, m128_f32_t b) {
	return _mm_andnot_ps(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Average packed unsigned 16-bit integers in "a" and "b", and store the results
in "dst".

pavgw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := (a[i+15:i] + b[i+15:i] + 1) >> 1
ENDFOR

*/
FEA_FORCEINLINE static m64_u16_t avg(m64_u16_t a, m64_u16_t b) {
	return _mm_avg_pu16(a.xmm, b.xmm);
}
#endif

#if FEA_32BIT
/*
Average packed unsigned 8-bit integers in "a" and "b", and store the results
in "dst".

pavgb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := (a[i+7:i] + b[i+7:i] + 1) >> 1
ENDFOR

*/
FEA_FORCEINLINE static m64_u8_t avg(m64_u8_t a, m64_u8_t b) {
	return _mm_avg_pu8(a.xmm, b.xmm);
}
#endif

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for equality, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpeq(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpeq_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for equality, store the result in the lower element of "dst", and copy
the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] == b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpeq_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpeq_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for greater-than-or-equal, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] >= b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpge(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpge_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for greater-than-or-equal, store the result in the lower element of
"dst", and copy the upper 3 packed elements from "a" to the upper elements of
"dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] >= b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpge_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpge_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for greater-than, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] > b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpgt(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpgt_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for greater-than, store the result in the lower element of "dst", and
copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] > b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpgt_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpgt_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for less-than-or-equal, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] <= b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmple(m128_f32_t a, m128_f32_t b) {
	return _mm_cmple_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for less-than-or-equal, store the result in the lower element of "dst",
and copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] <= b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmple_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmple_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for less-than, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] < b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmplt(m128_f32_t a, m128_f32_t b) {
	return _mm_cmplt_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for less-than, store the result in the lower element of "dst", and copy
the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] < b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmplt_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmplt_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for not-equal, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] != b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpneq(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpneq_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for not-equal, store the result in the lower element of "dst", and copy
the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] != b[31:0] ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpneq_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpneq_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for not-greater-than-or-equal, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (!( a[i+31:i] >= b[i+31:i] )) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpnge(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnge_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for not-greater-than-or-equal, store the result in the lower element of
"dst", and copy the upper 3 packed elements from "a" to the upper elements of
"dst".

cmpss xmm, xmm, imm8

dst[31:0] := (!( a[31:0] >= b[31:0] )) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpnge_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnge_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for not-greater-than, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (!( a[i+31:i] > b[i+31:i] )) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpngt(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpngt_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for not-greater-than, store the result in the lower element of "dst", and
copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := (!( a[31:0] > b[31:0] )) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpngt_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpngt_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for not-less-than-or-equal, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (!( a[i+31:i] <= b[i+31:i] )) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpnle(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnle_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for not-less-than-or-equal, store the result in the lower element of
"dst", and copy the upper 3 packed elements from "a" to the upper elements of
"dst".

cmpss xmm, xmm, imm8

dst[31:0] := (!( a[31:0] <= b[31:0] )) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpnle_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnle_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" for not-less-than, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := !( a[i+31:i] < b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpnlt(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnlt_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" for not-less-than, store the result in the lower element of "dst", and
copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := (!( a[31:0] < b[31:0] )) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpnlt_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpnlt_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" to see if neither is NaN, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] != NaN AND b[i+31:i] != NaN ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpord(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpord_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" to see if neither is NaN, store the result in the lower element of "dst",
and copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] != NaN AND b[31:0] != NaN ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpord_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpord_ss(a.xmm, b.xmm);
}

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b" to see if either is NaN, and store the results in "dst".

cmpps xmm, xmm, imm8

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == NaN OR b[i+31:i] == NaN ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t cmpunord(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpunord_ps(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b" to see if either is NaN, store the result in the lower element of "dst",
and copy the upper 3 packed elements from "a" to the upper elements of "dst".

cmpss xmm, xmm, imm8

dst[31:0] := ( a[31:0] == NaN OR b[31:0] == NaN ) ? 0xFFFFFFFF : 0
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t cmpunord_s(m128_f32_t a, m128_f32_t b) {
	return _mm_cmpunord_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for equality, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] == b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comieq(m128_f32_t a, m128_f32_t b) {
	return _mm_comieq_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for greater-than-or-equal, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] >= b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comige(m128_f32_t a, m128_f32_t b) {
	return _mm_comige_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for greater-than, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] > b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comigt(m128_f32_t a, m128_f32_t b) {
	return _mm_comigt_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for less-than-or-equal, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] <= b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comile(m128_f32_t a, m128_f32_t b) {
	return _mm_comile_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for less-than, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] < b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comilt(m128_f32_t a, m128_f32_t b) {
	return _mm_comilt_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for not-equal, and return the boolean result (0 or 1).

comiss xmm, xmm

RETURN ( a[31:0] == NaN OR b[31:0] == NaN OR a[31:0] != b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool comineq(m128_f32_t a, m128_f32_t b) {
	return _mm_comineq_ss(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Convert packed 16-bit integers in "a" to packed single-precision (32-bit)
floating-point elements, and store the results in "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*16
	m := j*32
	dst[m+31:m] := Convert_Int16_To_FP32(a[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m64_i16_t a, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpi16_ps(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed 32-bit integers in "b" to packed single-precision (32-bit)
floating-point elements, store the results in the lower 2 elements of "dst",
and copy the upper 2 packed elements from "a" to the upper elements of "dst".

cvtpi2ps xmm, mm

dst[31:0] := Convert_Int32_To_FP32(b[31:0])
dst[63:32] := Convert_Int32_To_FP32(b[63:32])
dst[95:64] := a[95:64]
dst[127:96] := a[127:96]

*/
FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m64_i32_t b, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpi32_ps(a.xmm, b.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed signed 32-bit integers in "a" to packed single-precision
(32-bit) floating-point elements, store the results in the lower 2 elements
of "dst", then covert the packed signed 32-bit integers in "b" to
single-precision (32-bit) floating-point element, and store the results in
the upper 2 elements of "dst".

SEQUENCE

dst[31:0] := Convert_Int32_To_FP32(a[31:0])
dst[63:32] := Convert_Int32_To_FP32(a[63:32])
dst[95:64] := Convert_Int32_To_FP32(b[31:0])
dst[127:96] := Convert_Int32_To_FP32(b[63:32])

*/
FEA_FORCEINLINE static void cvt_px22p(m64_i32_t a, m64_i32_t b, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpi32x2_ps(a.xmm, b.xmm);
}
#endif

#if FEA_32BIT
/*
Convert the lower packed 8-bit integers in "a" to packed single-precision
(32-bit) floating-point elements, and store the results in "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*8
	m := j*32
	dst[m+31:m] := Convert_Int8_To_FP32(a[i+7:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m64_i8_t a, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpi8_ps(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed single-precision (32-bit) floating-point elements in "a" to
packed 16-bit integers, and store the results in "dst". Note: this intrinsic
will generate 0x7FFF, rather than 0x8000, for input values between 0x7FFF and
0x7FFFFFFF.

SEQUENCE

FOR j := 0 to 3
	i := 16*j
	k := 32*j
	IF a[k+31:k] >= FP32(0x7FFF) && a[k+31:k] <= FP32(0x7FFFFFFF)
		dst[i+15:i] := 0x7FFF
	ELSE
		dst[i+15:i] := Convert_FP32_To_Int16(a[k+31:k])
	FI
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m64_i16_t& dst) {
	dst.xmm = _mm_cvtps_pi16(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed single-precision (32-bit) floating-point elements in "a" to
packed 32-bit integers, and store the results in "dst".

cvtps2pi mm, xmm

FOR j := 0 to 1
	i := 32*j
	dst[i+31:i] := Convert_FP32_To_Int32(a[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m64_i32_t& dst) {
	dst.xmm = _mm_cvtps_pi32(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed single-precision (32-bit) floating-point elements in "a" to
packed 8-bit integers, and store the results in lower 4 elements of "dst".
Note: this intrinsic will generate 0x7F, rather than 0x80, for input values
between 0x7F and 0x7FFFFFFF.

SEQUENCE

FOR j := 0 to 3
	i := 8*j
	k := 32*j
	IF a[k+31:k] >= FP32(0x7F) && a[k+31:k] <= FP32(0x7FFFFFFF)
		dst[i+7:i] := 0x7F
	ELSE
		dst[i+7:i] := Convert_FP32_To_Int8(a[k+31:k])
	FI
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m64_i8_t& dst) {
	dst.xmm = _mm_cvtps_pi8(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert packed unsigned 16-bit integers in "a" to packed single-precision
(32-bit) floating-point elements, and store the results in "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*16
	m := j*32
	dst[m+31:m] := Convert_Int16_To_FP32(a[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m64_u16_t a, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpu16_ps(a.xmm);
}
#endif

#if FEA_32BIT
/*
Convert the lower packed unsigned 8-bit integers in "a" to packed
single-precision (32-bit) floating-point elements, and store the results in
"dst".

SEQUENCE

FOR j := 0 to 3
	i := j*8
	m := j*32
	dst[m+31:m] := Convert_Int8_To_FP32(a[i+7:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvt_p2p(m64_u8_t a, m128_f32_t& dst) {
	dst.xmm = _mm_cvtpu8_ps(a.xmm);
}
#endif

/*
Convert the signed 32-bit integer "b" to a single-precision (32-bit)
floating-point element, store the result in the lower element of "dst", and
copy the upper 3 packed elements from "a" to the upper elements of "dst".

cvtsi2ss xmm, r32

dst[31:0] := Convert_Int32_To_FP32(b[31:0])
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static void cvt_s2s(m128_f32_t a, int b, m128_f32_t& dst) {
	dst.xmm = _mm_cvtsi32_ss(a.xmm, b);
}

/*
Convert the signed 64-bit integer "b" to a single-precision (32-bit)
floating-point element, store the result in the lower element of "dst", and
copy the upper 3 packed elements from "a" to the upper elements of "dst".

cvtsi2ss xmm, r64

dst[31:0] := Convert_Int64_To_FP32(b[63:0])
dst[127:32] := a[127:32]
dst[MAX:128] := 0

*/
FEA_FORCEINLINE static void cvt_s2s(m128_f32_t a, __int64 b, m128_f32_t& dst) {
	dst.xmm = _mm_cvtsi64_ss(a.xmm, b);
}

/*
Copy the lower single-precision (32-bit) floating-point element of "a" to
"dst".

movss m32, xmm

dst[31:0] := a[31:0]

*/
FEA_FORCEINLINE static void cvt_s2s(m128_f32_t a, float& dst) {
	dst = _mm_cvtss_f32(a.xmm);
}

/*
Convert the lower single-precision (32-bit) floating-point element in "a" to a
32-bit integer, and store the result in "dst".

cvtss2si r32, xmm

dst[31:0] := Convert_FP32_To_Int32(a[31:0])

*/
FEA_FORCEINLINE static void cvt_s2s(m128_f32_t a, int& dst) {
	dst = _mm_cvtss_si32(a.xmm);
}

/*
Convert the lower single-precision (32-bit) floating-point element in "a" to a
64-bit integer, and store the result in "dst".

cvtss2si r64, xmm

dst[63:0] := Convert_FP32_To_Int64(a[31:0])

*/
FEA_FORCEINLINE static void cvt_s2s(m128_f32_t a, __int64& dst) {
	dst = _mm_cvtss_si64(a.xmm);
}

#if FEA_32BIT
/*
Convert packed single-precision (32-bit) floating-point elements in "a" to
packed 32-bit integers with truncation, and store the results in "dst".

cvttps2pi mm, xmm

FOR j := 0 to 1
	i := 32*j
	dst[i+31:i] := Convert_FP32_To_Int32_Truncate(a[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static void cvtt_p2p(m128_f32_t a, m64_i32_t& dst) {
	dst.xmm = _mm_cvttps_pi32(a.xmm);
}
#endif

/*
Convert the lower single-precision (32-bit) floating-point element in "a" to a
32-bit integer with truncation, and store the result in "dst".

cvttss2si r32, xmm

dst[31:0] := Convert_FP32_To_Int32_Truncate(a[31:0])

*/
FEA_FORCEINLINE static void cvtt_s2s(m128_f32_t a, int& dst) {
	dst = _mm_cvttss_si32(a.xmm);
}

/*
Convert the lower single-precision (32-bit) floating-point element in "a" to a
64-bit integer with truncation, and store the result in "dst".

cvttss2si r64, xmm

dst[63:0] := Convert_FP32_To_Int64_Truncate(a[31:0])

*/
FEA_FORCEINLINE static void cvtt_s2s(m128_f32_t a, __int64& dst) {
	dst = _mm_cvttss_si64(a.xmm);
}

/*
Divide packed single-precision (32-bit) floating-point elements in "a" by
packed elements in "b", and store the results in "dst".

divps xmm, xmm

FOR j := 0 to 3
	i := 32*j
	dst[i+31:i] := a[i+31:i] / b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t div(m128_f32_t a, m128_f32_t b) {
	return _mm_div_ps(a.xmm, b.xmm);
}

/*
Divide the lower single-precision (32-bit) floating-point element in "a" by
the lower single-precision (32-bit) floating-point element in "b", store the
result in the lower element of "dst", and copy the upper 3 packed elements
from "a" to the upper elements of "dst".

divss xmm, xmm

dst[31:0] := a[31:0] / b[31:0]
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t div_s(m128_f32_t a, m128_f32_t b) {
	return _mm_div_ss(a.xmm, b.xmm);
}

/*
Extract a 16-bit integer from "a", selected with "imm8", and store the result
in the lower element of "dst".

pextrw r32, mm, imm8

dst[15:0] := (a[63:0] >> (imm8[1:0] * 16))[15:0]
dst[31:16] := 0

*/
template <int imm8>
FEA_FORCEINLINE static int extract(m64_i16_t a) {
	return _mm_extract_pi16(a.xmm, imm8);
}

/*
Free aligned memory that was allocated with "_mm_malloc".



*/
FEA_FORCEINLINE static void free(void * mem_addr) {
	_mm_free(mem_addr);
}

/*
Get the unsigned 32-bit value of the MXCSR control and status register.

stmxcsr m32

dst[31:0] := MXCSR

*/
FEA_FORCEINLINE static unsigned int getcsr() {
	return _mm_getcsr();
}

/*
Copy "a" to "dst", and insert the 16-bit integer "i" into "dst" at the
location specified by "imm8".

pinsrw mm, r32, imm8

dst[63:0] := a[63:0]
sel := imm8[1:0]*16
dst[sel+15:sel] := i[15:0]

*/
template <int imm8>
FEA_FORCEINLINE static m64_i16_t insert(m64_i16_t a, int i) {
	return _mm_insert_pi16(a.xmm, i, imm8);
}

/*
Load a single-precision (32-bit) floating-point element from memory into all
elements of "dst".

SEQUENCE

dst[31:0] := MEM[mem_addr+31:mem_addr]
dst[63:32] := MEM[mem_addr+31:mem_addr]
dst[95:64] := MEM[mem_addr+31:mem_addr]
dst[127:96] := MEM[mem_addr+31:mem_addr]

*/
FEA_FORCEINLINE static void load1(float const& mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_load1_ps(&mem_addr);
}

/*
Load 128-bits (composed of 4 packed single-precision (32-bit) floating-point
elements) from memory into "dst". "mem_addr" must be aligned on a 16-byte
boundary or a general-protection exception may be generated.

movaps xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void load(float const* mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_load_ps(mem_addr);
}

/*
Load a single-precision (32-bit) floating-point element from memory into all
elements of "dst".

SEQUENCE

dst[31:0] := MEM[mem_addr+31:mem_addr]
dst[63:32] := MEM[mem_addr+31:mem_addr]
dst[95:64] := MEM[mem_addr+31:mem_addr]
dst[127:96] := MEM[mem_addr+31:mem_addr]

*/
FEA_FORCEINLINE static void load_p1(float const& mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_load_ps1(&mem_addr);
}

/*
Load a single-precision (32-bit) floating-point element from memory into the
lower of "dst", and zero the upper 3 elements. "mem_addr" does not need to be
aligned on any particular boundary.

movss xmm, m32

dst[31:0] := MEM[mem_addr+31:mem_addr]
dst[127:32] := 0

*/
FEA_FORCEINLINE static void load_s(float const& mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_load_ss(&mem_addr);
}

/*
Load 2 single-precision (32-bit) floating-point elements from memory into the
upper 2 elements of "dst", and copy the lower 2 elements from "a" to "dst".
"mem_addr" does not need to be aligned on any particular boundary.

movhps xmm, m64

dst[31:0] := a[31:0]
dst[63:32] := a[63:32]
dst[95:64] := MEM[mem_addr+31:mem_addr]
dst[127:96] := MEM[mem_addr+63:mem_addr+32]

*/
FEA_FORCEINLINE static void loadh(m128_f32_t a, float const* mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_loadh_pi(a.xmm, (__m64 const*)mem_addr);
}

/*
Load 2 single-precision (32-bit) floating-point elements from memory into the
lower 2 elements of "dst", and copy the upper 2 elements from "a" to "dst".
"mem_addr" does not need to be aligned on any particular boundary.

movlps xmm, m64

dst[31:0] := MEM[mem_addr+31:mem_addr]
dst[63:32] := MEM[mem_addr+63:mem_addr+32]
dst[95:64] := a[95:64]
dst[127:96] := a[127:96]

*/
FEA_FORCEINLINE static void loadl(m128_f32_t a, float const* mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_loadl_pi(a.xmm, (__m64 const*)mem_addr);
}

/*
Load 4 single-precision (32-bit) floating-point elements from memory into
"dst" in reverse order. mem_addr must be aligned on a 16-byte boundary or a
general-protection exception may be generated.

SEQUENCE

dst[31:0] := MEM[mem_addr+127:mem_addr+96]
dst[63:32] := MEM[mem_addr+95:mem_addr+64]
dst[95:64] := MEM[mem_addr+63:mem_addr+32]
dst[127:96] := MEM[mem_addr+31:mem_addr]

*/
FEA_FORCEINLINE static void loadr(float const* mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_loadr_ps(mem_addr);
}

/*
Load 128-bits (composed of 4 packed single-precision (32-bit) floating-point
elements) from memory into "dst". "mem_addr" does not need to be aligned on
any particular boundary.

movups xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void loadu(float const* mem_addr, m128_f32_t& dst) {
	dst.xmm = _mm_loadu_ps(mem_addr);
}

/*
Allocate "size" bytes of memory, aligned to the alignment specified in
"align", and return a pointer to the allocated memory. "_mm_free" should be
used to free memory that is allocated with "_mm_malloc".



*/
FEA_FORCEINLINE static void* malloc(size_t size, size_t align) {
	return _mm_malloc(size, align);
}

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_i8_t a, m64_u8_t mask, char* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_i16_t a, m64_u8_t mask, short* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_i32_t a, m64_u8_t mask, int* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_i64_t a, m64_u8_t mask, long long* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_u8_t a, m64_u8_t mask, unsigned char* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_u16_t a, m64_u8_t mask, unsigned short* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_u32_t a, m64_u8_t mask, unsigned int* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Conditionally store 8-bit integer elements from "a" into memory using "mask"
(elements are not stored when the highest bit is not set in the corresponding
element) and a non-temporal memory hint.

maskmovq mm, mm

FOR j := 0 to 7
	i := j*8
	IF mask[i+7]
		MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static void maskmove(m64_u64_t a, m64_u8_t mask, unsigned long long* mem_addr) {
	_mm_maskmove_si64(a.xmm, mask.xmm, mem_addr);
}
#endif

#if FEA_32BIT
/*
Compare packed signed 16-bit integers in "a" and "b", and store packed maximum
values in "dst".

pmaxsw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := MAX(a[i+15:i], b[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t max(m64_i16_t a, m64_i16_t b) {
	return _mm_max_pi16(a.xmm, b.xmm);
}
#endif

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b", and store packed maximum values in "dst". [max_float_note]

maxps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := MAX(a[i+31:i], b[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t max(m128_f32_t a, m128_f32_t b) {
	return _mm_max_ps(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Compare packed unsigned 8-bit integers in "a" and "b", and store packed
maximum values in "dst".

pmaxub mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := MAX(a[i+7:i], b[i+7:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_u8_t max(m64_u8_t a, m64_u8_t b) {
	return _mm_max_pu8(a.xmm, b.xmm);
}
#endif

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b", store the maximum value in the lower element of "dst", and copy the
upper 3 packed elements from "a" to the upper element of "dst".
[max_float_note]

maxss xmm, xmm

dst[31:0] := MAX(a[31:0], b[31:0])
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t max_s(m128_f32_t a, m128_f32_t b) {
	return _mm_max_ss(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Compare packed signed 16-bit integers in "a" and "b", and store packed minimum
values in "dst".

pminsw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := MIN(a[i+15:i], b[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t min(m64_i16_t a, m64_i16_t b) {
	return _mm_min_pi16(a.xmm, b.xmm);
}
#endif

/*
Compare packed single-precision (32-bit) floating-point elements in "a" and
"b", and store packed minimum values in "dst". [min_float_note]

minps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := MIN(a[i+31:i], b[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t min(m128_f32_t a, m128_f32_t b) {
	return _mm_min_ps(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Compare packed unsigned 8-bit integers in "a" and "b", and store packed
minimum values in "dst".

pminub mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := MIN(a[i+7:i], b[i+7:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_u8_t min(m64_u8_t a, m64_u8_t b) {
	return _mm_min_pu8(a.xmm, b.xmm);
}
#endif

/*
Compare the lower single-precision (32-bit) floating-point elements in "a" and
"b", store the minimum value in the lower element of "dst", and copy the
upper 3 packed elements from "a" to the upper element of "dst".
[min_float_note]

minss xmm, xmm

dst[31:0] := MIN(a[31:0], b[31:0])
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t min_s(m128_f32_t a, m128_f32_t b) {
	return _mm_min_ss(a.xmm, b.xmm);
}

/*
Move the lower single-precision (32-bit) floating-point element from "b" to
the lower element of "dst", and copy the upper 3 packed elements from "a" to
the upper elements of "dst".

movss xmm, xmm

dst[31:0] := b[31:0]
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t move_s(m128_f32_t a, m128_f32_t b) {
	return _mm_move_ss(a.xmm, b.xmm);
}

/*
Move the upper 2 single-precision (32-bit) floating-point elements from "b" to
the lower 2 elements of "dst", and copy the upper 2 elements from "a" to the
upper 2 elements of "dst".

movhlps xmm, xmm

dst[31:0] := b[95:64]
dst[63:32] := b[127:96]
dst[95:64] := a[95:64]
dst[127:96] := a[127:96]

*/
FEA_FORCEINLINE static m128_f32_t movehl(m128_f32_t a, m128_f32_t b) {
	return _mm_movehl_ps(a.xmm, b.xmm);
}

/*
Move the lower 2 single-precision (32-bit) floating-point elements from "b" to
the upper 2 elements of "dst", and copy the lower 2 elements from "a" to the
lower 2 elements of "dst".

movlhps xmm, xmm

dst[31:0] := a[31:0]
dst[63:32] := a[63:32]
dst[95:64] := b[31:0]
dst[127:96] := b[63:32]

*/
FEA_FORCEINLINE static m128_f32_t movelh(m128_f32_t a, m128_f32_t b) {
	return _mm_movelh_ps(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Create mask from the most significant bit of each 8-bit element in "a", and
store the result in "dst".

pmovmskb r32, mm

FOR j := 0 to 7
	i := j*8
	dst[j] := a[i+7]
ENDFOR
dst[MAX:8] := 0

*/
FEA_FORCEINLINE static int movemask(m64_i8_t a) {
	return _mm_movemask_pi8(a.xmm);
}
#endif

/*
Set each bit of mask "dst" based on the most significant bit of the
corresponding packed single-precision (32-bit) floating-point element in "a".

movmskps r32, xmm

FOR j := 0 to 3
	i := j*32
	IF a[i+31]
		dst[j] := 1
	ELSE
		dst[j] := 0
	FI
ENDFOR
dst[MAX:4] := 0

*/
FEA_FORCEINLINE static int movemask(m128_f32_t a) {
	return _mm_movemask_ps(a.xmm);
}

/*
Multiply packed single-precision (32-bit) floating-point elements in "a" and
"b", and store the results in "dst".

mulps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[i+31:i] * b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t mul(m128_f32_t a, m128_f32_t b) {
	return _mm_mul_ps(a.xmm, b.xmm);
}

/*
Multiply the lower single-precision (32-bit) floating-point element in "a" and
"b", store the result in the lower element of "dst", and copy the upper 3
packed elements from "a" to the upper elements of "dst".

mulss xmm, xmm

dst[31:0] := a[31:0] * b[31:0]
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t mul_s(m128_f32_t a, m128_f32_t b) {
	return _mm_mul_ss(a.xmm, b.xmm);
}

#if FEA_32BIT
/*
Multiply the packed unsigned 16-bit integers in "a" and "b", producing
intermediate 32-bit integers, and store the high 16 bits of the intermediate
integers in "dst".

pmulhuw mm, mm

FOR j := 0 to 3
	i := j*16
	tmp[31:0] := a[i+15:i] * b[i+15:i]
	dst[i+15:i] := tmp[31:16]
ENDFOR

*/
FEA_FORCEINLINE static m64_u16_t mulhi(m64_u16_t a, m64_u16_t b) {
	return _mm_mulhi_pu16(a.xmm, b.xmm);
}
#endif

/*
Compute the bitwise OR of packed single-precision (32-bit) floating-point
elements in "a" and "b", and store the results in "dst".

orps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[i+31:i] OR b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t or_(m128_f32_t a, m128_f32_t b) {
	return _mm_or_ps(a.xmm, b.xmm);
}

/*
Fetch the line of data from memory that contains address "p" to a location in
the cache heirarchy specified by the locality hint "i".

prefetchnta m8


*/
template <int i>
FEA_FORCEINLINE static void prefetch(char const* p) {
	_mm_prefetch(p, i);
}

/*
Compute the approximate reciprocal of packed single-precision (32-bit)
floating-point elements in "a", and store the results in "dst". The maximum
relative error for this approximation is less than 1.5*2^-12.

rcpps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (1.0 / a[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t rcp(m128_f32_t a) {
	return _mm_rcp_ps(a.xmm);
}

/*
Compute the approximate reciprocal of the lower single-precision (32-bit)
floating-point element in "a", store the result in the lower element of
"dst", and copy the upper 3 packed elements from "a" to the upper elements of
"dst". The maximum relative error for this approximation is less than
1.5*2^-12.

rcpss xmm, xmm

dst[31:0] := (1.0 / a[31:0])
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t rcp_s(m128_f32_t a) {
	return _mm_rcp_ss(a.xmm);
}

/*
Compute the approximate reciprocal square root of packed single-precision
(32-bit) floating-point elements in "a", and store the results in "dst". The
maximum relative error for this approximation is less than 1.5*2^-12.

rsqrtps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := (1.0 / SQRT(a[i+31:i]))
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t rsqrt(m128_f32_t a) {
	return _mm_rsqrt_ps(a.xmm);
}

/*
Compute the approximate reciprocal square root of the lower single-precision
(32-bit) floating-point element in "a", store the result in the lower element
of "dst", and copy the upper 3 packed elements from "a" to the upper elements
of "dst". The maximum relative error for this approximation is less than
1.5*2^-12.

rsqrtss xmm, xmm

dst[31:0] := (1.0 / SQRT(a[31:0]))
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t rsqrt_s(m128_f32_t a) {
	return _mm_rsqrt_ss(a.xmm);
}

#if FEA_32BIT
/*
Compute the absolute differences of packed unsigned 8-bit integers in "a" and
"b", then horizontally sum each consecutive 8 differences to produce four
unsigned 16-bit integers, and pack these unsigned 16-bit integers in the low
16 bits of "dst".

psadbw mm, mm

FOR j := 0 to 7
	i := j*8
	tmp[i+7:i] := ABS(a[i+7:i] - b[i+7:i])
ENDFOR
dst[15:0] := tmp[7:0] + tmp[15:8] + tmp[23:16] + tmp[31:24] + tmp[39:32] + tmp[47:40] + tmp[55:48] + tmp[63:56]
dst[63:16] := 0

*/
FEA_FORCEINLINE static m64_u16_t sad(m64_u8_t a, m64_u8_t b) {
	return _mm_sad_pu8(a.xmm, b.xmm);
}
#endif

/*
Broadcast single-precision (32-bit) floating-point value "a" to all elements
of "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR

*/
FEA_FORCEINLINE static void set1(float a, m128_f32_t& dst) {
	dst.xmm = _mm_set1_ps(a);
}

/*
Set packed single-precision (32-bit) floating-point elements in "dst" with the
supplied values.

SEQUENCE

dst[31:0] := e0
dst[63:32] := e1
dst[95:64] := e2
dst[127:96] := e3

*/
FEA_FORCEINLINE static void set(float e3, float e2, float e1, float e0, m128_f32_t& dst) {
	dst.xmm = _mm_set_ps(e3, e2, e1, e0);
}

/*
Broadcast single-precision (32-bit) floating-point value "a" to all elements
of "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR

*/
FEA_FORCEINLINE static void set_p1(float a, m128_f32_t& dst) {
	dst.xmm = _mm_set_ps1(a);
}

/*
Copy single-precision (32-bit) floating-point element "a" to the lower element
of "dst", and zero the upper 3 elements.

SEQUENCE

dst[31:0] := a[31:0]
dst[127:32] := 0

*/
FEA_FORCEINLINE static void set_s(float a, m128_f32_t& dst) {
	dst.xmm = _mm_set_ss(a);
}

/*
Set the MXCSR control and status register with the value in unsigned 32-bit
integer "a".

ldmxcsr m32

MXCSR := a[31:0]

*/
FEA_FORCEINLINE static void setcsr(unsigned int a) {
	_mm_setcsr(a);
}

/*
Set packed single-precision (32-bit) floating-point elements in "dst" with the
supplied values in reverse order.

SEQUENCE

dst[31:0] := e3
dst[63:32] := e2
dst[95:64] := e1
dst[127:96] := e0

*/
FEA_FORCEINLINE static void setr(float e3, float e2, float e1, float e0, m128_f32_t& dst) {
	dst.xmm = _mm_setr_ps(e3, e2, e1, e0);
}

/*
Return vector of type __m128 with all elements set to zero.

xorps xmm, xmm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m128_f32_t& dst) {
	dst.xmm = _mm_setzero_ps();
}

/*
Perform a serializing operation on all store-to-memory instructions that were
issued prior to this instruction. Guarantees that every store instruction
that precedes, in program order, is globally visible before any store
instruction which follows the fence in program order.

sfence 


*/
FEA_FORCEINLINE static void sfence() {
	_mm_sfence();
}

/*
Shuffle 16-bit integers in "a" using the control in "imm8", and store the
results in "dst".

pshufw mm, mm, imm8

DEFINE SELECT4(src, control) {
	CASE(control[1:0]) OF
	0:	tmp[15:0] := src[15:0]
	1:	tmp[15:0] := src[31:16]
	2:	tmp[15:0] := src[47:32]
	3:	tmp[15:0] := src[63:48]
	ESAC
	RETURN tmp[15:0]
}
dst[15:0] := SELECT4(a[63:0], imm8[1:0])
dst[31:16] := SELECT4(a[63:0], imm8[3:2])
dst[47:32] := SELECT4(a[63:0], imm8[5:4])
dst[63:48] := SELECT4(a[63:0], imm8[7:6])

*/
template <int imm8>
FEA_FORCEINLINE static m64_i16_t shuffle(m64_i16_t a) {
	return _mm_shuffle_pi16(a.xmm, imm8);
}

/*
Shuffle single-precision (32-bit) floating-point elements in "a" using the
control in "imm8", and store the results in "dst".

shufps xmm, xmm, imm8

DEFINE SELECT4(src, control) {
	CASE(control[1:0]) OF
	0:	tmp[31:0] := src[31:0]
	1:	tmp[31:0] := src[63:32]
	2:	tmp[31:0] := src[95:64]
	3:	tmp[31:0] := src[127:96]
	ESAC
	RETURN tmp[31:0]
}
dst[31:0] := SELECT4(a[127:0], imm8[1:0])
dst[63:32] := SELECT4(a[127:0], imm8[3:2])
dst[95:64] := SELECT4(b[127:0], imm8[5:4])
dst[127:96] := SELECT4(b[127:0], imm8[7:6])

*/
template <unsigned int imm8>
FEA_FORCEINLINE static m128_f32_t shuffle(m128_f32_t a, m128_f32_t b) {
	return _mm_shuffle_ps(a.xmm, b.xmm, imm8);
}

/*
Compute the square root of packed single-precision (32-bit) floating-point
elements in "a", and store the results in "dst".

sqrtps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := SQRT(a[i+31:i])
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t sqrt(m128_f32_t a) {
	return _mm_sqrt_ps(a.xmm);
}

/*
Compute the square root of the lower single-precision (32-bit) floating-point
element in "a", store the result in the lower element of "dst", and copy the
upper 3 packed elements from "a" to the upper elements of "dst".

sqrtss xmm, xmm

dst[31:0] := SQRT(a[31:0])
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t sqrt_s(m128_f32_t a) {
	return _mm_sqrt_ss(a.xmm);
}

/*
Store the lower single-precision (32-bit) floating-point element from "a" into
4 contiguous elements in memory. "mem_addr" must be aligned on a 16-byte
boundary or a general-protection exception may be generated.

SEQUENCE

MEM[mem_addr+31:mem_addr] := a[31:0]
MEM[mem_addr+63:mem_addr+32] := a[31:0]
MEM[mem_addr+95:mem_addr+64] := a[31:0]
MEM[mem_addr+127:mem_addr+96] := a[31:0]

*/
FEA_FORCEINLINE static void store1(m128_f32_t a, float* mem_addr) {
	_mm_store1_ps(mem_addr, a.xmm);
}

/*
Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point
elements) from "a" into memory. "mem_addr" must be aligned on a 16-byte
boundary or a general-protection exception may be generated.

movaps m128, xmm

MEM[mem_addr+127:mem_addr] := a[127:0]

*/
FEA_FORCEINLINE static void store(m128_f32_t a, float* mem_addr) {
	_mm_store_ps(mem_addr, a.xmm);
}

/*
Store the lower single-precision (32-bit) floating-point element from "a" into
4 contiguous elements in memory. "mem_addr" must be aligned on a 16-byte
boundary or a general-protection exception may be generated.

SEQUENCE

MEM[mem_addr+31:mem_addr] := a[31:0]
MEM[mem_addr+63:mem_addr+32] := a[31:0]
MEM[mem_addr+95:mem_addr+64] := a[31:0]
MEM[mem_addr+127:mem_addr+96] := a[31:0]

*/
FEA_FORCEINLINE static void store_p1(m128_f32_t a, float* mem_addr) {
	_mm_store_ps1(mem_addr, a.xmm);
}

/*
Store the lower single-precision (32-bit) floating-point element from "a" into
memory. "mem_addr" does not need to be aligned on any particular boundary.

movss m32, xmm

MEM[mem_addr+31:mem_addr] := a[31:0]

*/
FEA_FORCEINLINE static void store_s(m128_f32_t a, float& mem_addr) {
	_mm_store_ss(&mem_addr, a.xmm);
}

/*
Store the upper 2 single-precision (32-bit) floating-point elements from "a"
into memory.

movhps m64, xmm

MEM[mem_addr+31:mem_addr] := a[95:64]
MEM[mem_addr+63:mem_addr+32] := a[127:96]

*/
FEA_FORCEINLINE static void storeh(m128_f32_t a, float* mem_addr) {
	_mm_storeh_pi((__m64*)mem_addr, a.xmm);
}

/*
Store the lower 2 single-precision (32-bit) floating-point elements from "a"
into memory.

movlps m64, xmm

MEM[mem_addr+31:mem_addr] := a[31:0]
MEM[mem_addr+63:mem_addr+32] := a[63:32]

*/
FEA_FORCEINLINE static void storel(m128_f32_t a, float* mem_addr) {
	_mm_storel_pi((__m64*)mem_addr, a.xmm);
}

/*
Store 4 single-precision (32-bit) floating-point elements from "a" into memory
in reverse order. "mem_addr" must be aligned on a 16-byte boundary or a
general-protection exception may be generated.

SEQUENCE

MEM[mem_addr+31:mem_addr] := a[127:96]
MEM[mem_addr+63:mem_addr+32] := a[95:64]
MEM[mem_addr+95:mem_addr+64] := a[63:32]
MEM[mem_addr+127:mem_addr+96] := a[31:0]

*/
FEA_FORCEINLINE static void storer(m128_f32_t a, float* mem_addr) {
	_mm_storer_ps(mem_addr, a.xmm);
}

/*
Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point
elements) from "a" into memory. "mem_addr" does not need to be aligned on any
particular boundary.

movups m128, xmm

MEM[mem_addr+127:mem_addr] := a[127:0]

*/
FEA_FORCEINLINE static void storeu(m128_f32_t a, float* mem_addr) {
	_mm_storeu_ps(mem_addr, a.xmm);
}

#if FEA_32BIT
/*
Store 64-bits of integer data from "a" into memory using a non-temporal memory
hint.

movntq m64, mm

MEM[mem_addr+63:mem_addr] := a[63:0]

*/
FEA_FORCEINLINE static void stream(m64_f32_t a, m64_f32_t* mem_addr) {
	_mm_stream_pi(mem_addr.xmm, a.xmm);
}
#endif

/*
Store 128-bits (composed of 4 packed single-precision (32-bit) floating-point
elements) from "a" into memory using a non-temporal memory hint. "mem_addr"
must be aligned on a 16-byte boundary or a general-protection exception may
be generated.

movntps m128, xmm

MEM[mem_addr+127:mem_addr] := a[127:0]

*/
FEA_FORCEINLINE static void stream(m128_f32_t a, float* mem_addr) {
	_mm_stream_ps(mem_addr, a.xmm);
}

/*
Subtract packed single-precision (32-bit) floating-point elements in "b" from
packed single-precision (32-bit) floating-point elements in "a", and store
the results in "dst".

subps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[i+31:i] - b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t sub(m128_f32_t a, m128_f32_t b) {
	return _mm_sub_ps(a.xmm, b.xmm);
}

/*
Subtract the lower single-precision (32-bit) floating-point element in "b"
from the lower single-precision (32-bit) floating-point element in "a", store
the result in the lower element of "dst", and copy the upper 3 packed
elements from "a" to the upper elements of "dst".

subss xmm, xmm

dst[31:0] := a[31:0] - b[31:0]
dst[127:32] := a[127:32]

*/
FEA_FORCEINLINE static m128_f32_t sub_s(m128_f32_t a, m128_f32_t b) {
	return _mm_sub_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for equality, and return the boolean result (0 or 1). This instruction
will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] == b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomieq(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomieq_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for greater-than-or-equal, and return the boolean result (0 or 1). This
instruction will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] >= b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomige(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomige_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for greater-than, and return the boolean result (0 or 1). This
instruction will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] > b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomigt(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomigt_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for less-than-or-equal, and return the boolean result (0 or 1). This
instruction will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] <= b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomile(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomile_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for less-than, and return the boolean result (0 or 1). This instruction
will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] != NaN AND b[31:0] != NaN AND a[31:0] < b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomilt(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomilt_ss(a.xmm, b.xmm);
}

/*
Compare the lower single-precision (32-bit) floating-point element in "a" and
"b" for not-equal, and return the boolean result (0 or 1). This instruction
will not signal an exception for QNaNs.

ucomiss xmm, xmm

RETURN ( a[31:0] == NaN OR b[31:0] == NaN OR a[31:0] != b[31:0] ) ? 1 : 0

*/
FEA_FORCEINLINE static bool ucomineq(m128_f32_t a, m128_f32_t b) {
	return _mm_ucomineq_ss(a.xmm, b.xmm);
}

/*
Return vector of type __m128 with undefined elements.



*/
FEA_FORCEINLINE static void undefined(m128_f32_t& dst) {
	dst.xmm = _mm_undefined_ps();
}

/*
Unpack and interleave single-precision (32-bit) floating-point elements from
the high half "a" and "b", and store the results in "dst".

unpckhps xmm, xmm

DEFINE INTERLEAVE_HIGH_DWORDS(src1[127:0], src2[127:0]) {
	dst[31:0] := src1[95:64] 
	dst[63:32] := src2[95:64] 
	dst[95:64] := src1[127:96] 
	dst[127:96] := src2[127:96] 
	RETURN dst[127:0]	
}
dst[127:0] := INTERLEAVE_HIGH_DWORDS(a[127:0], b[127:0])

*/
FEA_FORCEINLINE static m128_f32_t unpackhi(m128_f32_t a, m128_f32_t b) {
	return _mm_unpackhi_ps(a.xmm, b.xmm);
}

/*
Unpack and interleave single-precision (32-bit) floating-point elements from
the low half of "a" and "b", and store the results in "dst".

unpcklps xmm, xmm

DEFINE INTERLEAVE_DWORDS(src1[127:0], src2[127:0]) {
	dst[31:0] := src1[31:0] 
	dst[63:32] := src2[31:0] 
	dst[95:64] := src1[63:32] 
	dst[127:96] := src2[63:32] 
	RETURN dst[127:0]	
}
dst[127:0] := INTERLEAVE_DWORDS(a[127:0], b[127:0])

*/
FEA_FORCEINLINE static m128_f32_t unpacklo(m128_f32_t a, m128_f32_t b) {
	return _mm_unpacklo_ps(a.xmm, b.xmm);
}

/*
Compute the bitwise XOR of packed single-precision (32-bit) floating-point
elements in "a" and "b", and store the results in "dst".

xorps xmm, xmm

FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[i+31:i] XOR b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t xor_(m128_f32_t a, m128_f32_t b) {
	return _mm_xor_ps(a.xmm, b.xmm);
}



};

using simd_sse_api = simd_api<fea::simd_ver::sse>;

} // namespace fea
