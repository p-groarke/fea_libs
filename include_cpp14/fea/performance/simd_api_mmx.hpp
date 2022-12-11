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
template <fea::simd_ver>
struct simd_api {
	// These are here to help auto-complete.

	using xmm_f32_t = simd_abi_register_t<fea::simd_ver::count, float>;
	using xmm_f64_t = simd_abi_register_t<fea::simd_ver::count, double>;
	using xmm_i8_t = simd_abi_register_t<fea::simd_ver::count, char>;
	using xmm_u8_t = simd_abi_register_t<fea::simd_ver::count, unsigned char>;
	using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::count, signed char>;
	using xmm_i16_t = simd_abi_register_t<fea::simd_ver::count, short>;
	using xmm_u16_t = simd_abi_register_t<fea::simd_ver::count, unsigned short>;
	using xmm_i32_t = simd_abi_register_t<fea::simd_ver::count, int>;
	using xmm_u32_t = simd_abi_register_t<fea::simd_ver::count, unsigned int>;
	using xmm_i64_t = simd_abi_register_t<fea::simd_ver::count, long long>;
	using xmm_u64_t = simd_abi_register_t<fea::simd_ver::count, unsigned long long>;
};

template <>
struct simd_api<fea::simd_ver::mmx> {
// Default register types for this api.
// Previous registers and functionality are still available.
using xmm_f32_t = simd_abi_register_t<fea::simd_ver::mmx, float>;
using xmm_f64_t = simd_abi_register_t<fea::simd_ver::mmx, double>;
using xmm_i8_t = simd_abi_register_t<fea::simd_ver::mmx, char>;
using xmm_u8_t = simd_abi_register_t<fea::simd_ver::mmx, unsigned char>;
using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::mmx, signed char>;
using xmm_i16_t = simd_abi_register_t<fea::simd_ver::mmx, short>;
using xmm_u16_t = simd_abi_register_t<fea::simd_ver::mmx, unsigned short>;
using xmm_i32_t = simd_abi_register_t<fea::simd_ver::mmx, int>;
using xmm_u32_t = simd_abi_register_t<fea::simd_ver::mmx, unsigned int>;
using xmm_i64_t = simd_abi_register_t<fea::simd_ver::mmx, long long>;
using xmm_u64_t = simd_abi_register_t<fea::simd_ver::mmx, unsigned long long>;

#if FEA_32BIT

/*
Add packed 16-bit integers in "a" and "b", and store the results in "dst".

paddw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := a[i+15:i] + b[i+15:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t add(m64_i16_t a, m64_i16_t b) {
	return _mm_add_pi16(a.xmm, b.xmm);
}

/*
Add packed 32-bit integers in "a" and "b", and store the results in "dst".

paddd mm, mm

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := a[i+31:i] + b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t add(m64_i32_t a, m64_i32_t b) {
	return _mm_add_pi32(a.xmm, b.xmm);
}

/*
Add packed 8-bit integers in "a" and "b", and store the results in "dst".

paddb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := a[i+7:i] + b[i+7:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t add(m64_i8_t a, m64_i8_t b) {
	return _mm_add_pi8(a.xmm, b.xmm);
}

/*
Add packed signed 16-bit integers in "a" and "b" using saturation, and store
the results in "dst".

paddsw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := Saturate16( a[i+15:i] + b[i+15:i] )
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t adds(m64_i16_t a, m64_i16_t b) {
	return _mm_adds_pi16(a.xmm, b.xmm);
}

/*
Add packed signed 8-bit integers in "a" and "b" using saturation, and store
the results in "dst".

paddsb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := Saturate8( a[i+7:i] + b[i+7:i] )
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t adds(m64_i8_t a, m64_i8_t b) {
	return _mm_adds_pi8(a.xmm, b.xmm);
}

/*
Add packed unsigned 16-bit integers in "a" and "b" using saturation, and store
the results in "dst".

paddusw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := SaturateU16( a[i+15:i] + b[i+15:i] )
ENDFOR

*/
FEA_FORCEINLINE static m64_u16_t adds(m64_u16_t a, m64_u16_t b) {
	return _mm_adds_pu16(a.xmm, b.xmm);
}

/*
Add packed unsigned 8-bit integers in "a" and "b" using saturation, and store
the results in "dst".

paddusb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := SaturateU8( a[i+7:i] + b[i+7:i] )
ENDFOR

*/
FEA_FORCEINLINE static m64_u8_t adds(m64_u8_t a, m64_u8_t b) {
	return _mm_adds_pu8(a.xmm, b.xmm);
}

/*
Compute the bitwise AND of 64 bits (representing integer data) in "a" and "b",
and store the result in "dst".

pand mm, mm

dst[63:0] := (a[63:0] AND b[63:0])

*/
FEA_FORCEINLINE static m64_i64_t and_(m64_i64_t a, m64_i64_t b) {
	return _mm_and_si64(a.xmm, b.xmm);
}

/*
Compute the bitwise NOT of 64 bits (representing integer data) in "a" and then
AND with "b", and store the result in "dst".

pandn mm, mm

dst[63:0] := ((NOT a[63:0]) AND b[63:0])

*/
FEA_FORCEINLINE static m64_i64_t andnot(m64_i64_t a, m64_i64_t b) {
	return _mm_andnot_si64(a.xmm, b.xmm);
}

/*
Compare packed 16-bit integers in "a" and "b" for equality, and store the
results in "dst".

pcmpeqw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := ( a[i+15:i] == b[i+15:i] ) ? 0xFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t cmpeq(m64_i16_t a, m64_i16_t b) {
	return _mm_cmpeq_pi16(a.xmm, b.xmm);
}

/*
Compare packed 32-bit integers in "a" and "b" for equality, and store the
results in "dst".

pcmpeqd mm, mm

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t cmpeq(m64_i32_t a, m64_i32_t b) {
	return _mm_cmpeq_pi32(a.xmm, b.xmm);
}

/*
Compare packed 8-bit integers in "a" and "b" for equality, and store the
results in "dst".

pcmpeqb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := ( a[i+7:i] == b[i+7:i] ) ? 0xFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t cmpeq(m64_i8_t a, m64_i8_t b) {
	return _mm_cmpeq_pi8(a.xmm, b.xmm);
}

/*
Compare packed signed 16-bit integers in "a" and "b" for greater-than, and
store the results in "dst".

pcmpgtw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := ( a[i+15:i] > b[i+15:i] ) ? 0xFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t cmpgt(m64_i16_t a, m64_i16_t b) {
	return _mm_cmpgt_pi16(a.xmm, b.xmm);
}

/*
Compare packed signed 32-bit integers in "a" and "b" for greater-than, and
store the results in "dst".

pcmpgtd mm, mm

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := ( a[i+31:i] > b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t cmpgt(m64_i32_t a, m64_i32_t b) {
	return _mm_cmpgt_pi32(a.xmm, b.xmm);
}

/*
Compare packed signed 8-bit integers in "a" and "b" for greater-than, and
store the results in "dst".

pcmpgtb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := ( a[i+7:i] > b[i+7:i] ) ? 0xFF : 0
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t cmpgt(m64_i8_t a, m64_i8_t b) {
	return _mm_cmpgt_pi8(a.xmm, b.xmm);
}

/*
Copy 64-bit integer "a" to "dst".

movq r64, mm

dst[63:0] := a[63:0]

*/
/*FEA_FORCEINLINE static void cvt_s2s(m64_i64_t a, __int64& dst) {
	dst = _mm_cvtm64_si64(a.xmm);
}*/

/*
Copy 32-bit integer "a" to the lower elements of "dst", and zero the upper
element of "dst".

movd mm, r32

dst[31:0] := a[31:0]
dst[63:32] := 0

*/
FEA_FORCEINLINE static void cvt_s2s(int a, m64_i64_t& dst) {
	dst.xmm = _mm_cvtsi32_si64(a);
}

/*
Copy 64-bit integer "a" to "dst".

movq mm, r64

dst[63:0] := a[63:0]

*/
/*FEA_FORCEINLINE static void cvt_s2s(__int64 a, m64_i64_t& dst) {
	dst.xmm = _mm_cvtsi64_m64(a);
}*/

/*
Copy the lower 32-bit integer in "a" to "dst".

movd r32, mm

dst[31:0] := a[31:0]

*/
FEA_FORCEINLINE static void cvt_s2s(m64_i64_t a, int& dst) {
	dst = _mm_cvtsi64_si32(a.xmm);
}

/*
Empty the MMX state, which marks the x87 FPU registers as available for use by
x87 instructions. This instruction must be used at the end of all MMX
technology procedures.

emms 


*/
FEA_FORCEINLINE static void empty() {
	_mm_empty();
}

/*
Multiply packed signed 16-bit integers in "a" and "b", producing intermediate
signed 32-bit integers. Horizontally add adjacent pairs of intermediate
32-bit integers, and pack the results in "dst".

pmaddwd mm, mm

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := SignExtend32(a[i+31:i+16]*b[i+31:i+16]) + SignExtend32(a[i+15:i]*b[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t madd(m64_i16_t a, m64_i16_t b) {
	return _mm_madd_pi16(a.xmm, b.xmm);
}

/*
Multiply the packed signed 16-bit integers in "a" and "b", producing
intermediate 32-bit integers, and store the high 16 bits of the intermediate
integers in "dst".

pmulhw mm, mm

FOR j := 0 to 3
	i := j*16
	tmp[31:0] := SignExtend32(a[i+15:i]) * SignExtend32(b[i+15:i])
	dst[i+15:i] := tmp[31:16]
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t mulhi(m64_i16_t a, m64_i16_t b) {
	return _mm_mulhi_pi16(a.xmm, b.xmm);
}

/*
Multiply the packed 16-bit integers in "a" and "b", producing intermediate
32-bit integers, and store the low 16 bits of the intermediate integers in
"dst".

pmullw mm, mm

FOR j := 0 to 3
	i := j*16
	tmp[31:0] := a[i+15:i] * b[i+15:i]
	dst[i+15:i] := tmp[15:0]
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t mulhi(m64_i16_t a, m64_i16_t b) {
	return _mm_mullo_pi16(a.xmm, b.xmm);
}

/*
Compute the bitwise OR of 64 bits (representing integer data) in "a" and "b",
and store the result in "dst".

por mm, mm

dst[63:0] := (a[63:0] OR b[63:0])

*/
FEA_FORCEINLINE static m64_i64_t or_(m64_i64_t a, m64_i64_t b) {
	return _mm_or_si64(a.xmm, b.xmm);
}

/*
Convert packed signed 16-bit integers from "a" and "b" to packed 8-bit
integers using signed saturation, and store the results in "dst".

packsswb mm, mm

dst[7:0] := Saturate8(a[15:0])
dst[15:8] := Saturate8(a[31:16])
dst[23:16] := Saturate8(a[47:32])
dst[31:24] := Saturate8(a[63:48])
dst[39:32] := Saturate8(b[15:0])
dst[47:40] := Saturate8(b[31:16])
dst[55:48] := Saturate8(b[47:32])
dst[63:56] := Saturate8(b[63:48])

*/
FEA_FORCEINLINE static m64_i8_t packs(m64_i16_t a, m64_i16_t b) {
	return _mm_packs_pi16(a.xmm, b.xmm);
}

/*
Convert packed signed 32-bit integers from "a" and "b" to packed 16-bit
integers using signed saturation, and store the results in "dst".

packssdw mm, mm

dst[15:0] := Saturate16(a[31:0])
dst[31:16] := Saturate16(a[63:32])
dst[47:32] := Saturate16(b[31:0])
dst[63:48] := Saturate16(b[63:32])

*/
FEA_FORCEINLINE static m64_i16_t packs(m64_i32_t a, m64_i32_t b) {
	return _mm_packs_pi32(a.xmm, b.xmm);
}

/*
Convert packed signed 16-bit integers from "a" and "b" to packed 8-bit
integers using unsigned saturation, and store the results in "dst".

packuswb mm, mm

dst[7:0] := SaturateU8(a[15:0])
dst[15:8] := SaturateU8(a[31:16])
dst[23:16] := SaturateU8(a[47:32])
dst[31:24] := SaturateU8(a[63:48])
dst[39:32] := SaturateU8(b[15:0])
dst[47:40] := SaturateU8(b[31:16])
dst[55:48] := SaturateU8(b[47:32])
dst[63:56] := SaturateU8(b[63:48])

*/
FEA_FORCEINLINE static m64_u8_t packs(m64_u16_t a, m64_u16_t b) {
	return _mm_packs_pu16(a.xmm, b.xmm);
}

/*
Broadcast 16-bit integer "a" to all all elements of "dst".

SEQUENCE

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := a[15:0]
ENDFOR

*/
FEA_FORCEINLINE static void set1(short a, m64_i16_t& dst) {
	dst.xmm = _mm_set1_pi16(a);
}

/*
Broadcast 32-bit integer "a" to all elements of "dst".

SEQUENCE

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR

*/
FEA_FORCEINLINE static void set1(int a, m64_i32_t& dst) {
	dst.xmm = _mm_set1_pi32(a);
}

/*
Broadcast 8-bit integer "a" to all elements of "dst".

SEQUENCE

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := a[7:0]
ENDFOR

*/
FEA_FORCEINLINE static void set1(char a, m64_i8_t& dst) {
	dst.xmm = _mm_set1_pi8(a);
}

/*
Set packed 16-bit integers in "dst" with the supplied values.

SEQUENCE

dst[15:0] := e0
dst[31:16] := e1
dst[47:32] := e2
dst[63:48] := e3

*/
FEA_FORCEINLINE static void set(short e3, short e2, short e1, short e0, m64_i16_t& dst) {
	dst.xmm = _mm_set_pi16(e3, e2, e1, e0);
}

/*
Set packed 32-bit integers in "dst" with the supplied values.

SEQUENCE

dst[31:0] := e0
dst[63:32] := e1

*/
FEA_FORCEINLINE static void set(int e1, int e0, m64_i32_t& dst) {
	dst.xmm = _mm_set_pi32(e1, e0);
}

/*
Set packed 8-bit integers in "dst" with the supplied values.

SEQUENCE

dst[7:0] := e0
dst[15:8] := e1
dst[23:16] := e2
dst[31:24] := e3
dst[39:32] := e4
dst[47:40] := e5
dst[55:48] := e6
dst[63:56] := e7

*/
FEA_FORCEINLINE static void set(char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0, m64_i8_t& dst) {
	dst.xmm = _mm_set_pi8(e7, e6, e5, e4, e3, e2, e1, e0);
}

/*
Set packed 16-bit integers in "dst" with the supplied values in reverse order.

SEQUENCE

dst[15:0] := e3
dst[31:16] := e2
dst[47:32] := e1
dst[63:48] := e0

*/
FEA_FORCEINLINE static void setr(short e3, short e2, short e1, short e0, m64_i16_t& dst) {
	dst.xmm = _mm_setr_pi16(e3, e2, e1, e0);
}

/*
Set packed 32-bit integers in "dst" with the supplied values in reverse order.

SEQUENCE

dst[31:0] := e1
dst[63:32] := e0

*/
FEA_FORCEINLINE static void setr(int e1, int e0, m64_i32_t& dst) {
	dst.xmm = _mm_setr_pi32(e1, e0);
}

/*
Set packed 8-bit integers in "dst" with the supplied values in reverse order.

SEQUENCE

dst[7:0] := e7
dst[15:8] := e6
dst[23:16] := e5
dst[31:24] := e4
dst[39:32] := e3
dst[47:40] := e2
dst[55:48] := e1
dst[63:56] := e0

*/
FEA_FORCEINLINE static void setr(char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0, m64_i8_t& dst) {
	dst.xmm = _mm_setr_pi8(e7, e6, e5, e4, e3, e2, e1, e0);
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_i8_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_i16_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_i32_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_i64_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_u8_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_u16_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_u32_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Return vector of type __m64 with all elements set to zero.

pxor mm, mm

dst[MAX:0] := 0

*/
FEA_FORCEINLINE static void setzero(m64_u64_t& dst) {
	dst.xmm = _mm_setzero_si64();
}

/*
Shift packed 16-bit integers in "a" left by "count" while shifting in zeros,
and store the results in "dst".

psllw mm, mm

FOR j := 0 to 3
	i := j*16
	IF count[63:0] > 15
		dst[i+15:i] := 0
	ELSE
		dst[i+15:i] := ZeroExtend16(a[i+15:i] << count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t sll(m64_i16_t a, m64_i64_t count) {
	return _mm_sll_pi16(a.xmm, count.xmm);
}

/*
Shift packed 32-bit integers in "a" left by "count" while shifting in zeros,
and store the results in "dst".

pslld mm, mm

FOR j := 0 to 1
	i := j*32
	IF count[63:0] > 31
		dst[i+31:i] := 0
	ELSE
		dst[i+31:i] := ZeroExtend32(a[i+31:i] << count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t sll(m64_i32_t a, m64_i64_t count) {
	return _mm_sll_pi32(a.xmm, count.xmm);
}

/*
Shift 64-bit integer "a" left by "count" while shifting in zeros, and store
the result in "dst".

psllq mm, mm

IF count[63:0] > 63
	dst[63:0] := 0
ELSE
	dst[63:0] := ZeroExtend64(a[63:0] << count[63:0])
FI

*/
FEA_FORCEINLINE static m64_i64_t sll(m64_i64_t a, m64_i64_t count) {
	return _mm_sll_si64(a.xmm, count.xmm);
}

/*
Shift packed 16-bit integers in "a" left by "imm8" while shifting in zeros,
and store the results in "dst".

psllw mm, imm8

FOR j := 0 to 3
	i := j*16
	IF imm8[7:0] > 15
		dst[i+15:i] := 0
	ELSE
		dst[i+15:i] := ZeroExtend16(a[i+15:i] << imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i16_t slli(m64_i16_t a) {
	return _mm_slli_pi16(a.xmm, imm8);
}

/*
Shift packed 32-bit integers in "a" left by "imm8" while shifting in zeros,
and store the results in "dst".

pslld mm, imm8

FOR j := 0 to 1
	i := j*32
	IF imm8[7:0] > 31
		dst[i+31:i] := 0
	ELSE
		dst[i+31:i] := ZeroExtend32(a[i+31:i] << imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i32_t slli(m64_i32_t a) {
	return _mm_slli_pi32(a.xmm, imm8);
}

/*
Shift 64-bit integer "a" left by "imm8" while shifting in zeros, and store the
result in "dst".

psllq mm, imm8

IF imm8[7:0] > 63
	dst[63:0] := 0
ELSE
	dst[63:0] := ZeroExtend64(a[63:0] << imm8[7:0])
FI

*/
template <int imm8>
FEA_FORCEINLINE static m64_i64_t slli(m64_i64_t a) {
	return _mm_slli_si64(a.xmm, imm8);
}

/*
Shift packed 16-bit integers in "a" right by "count" while shifting in sign
bits, and store the results in "dst".

psraw mm, mm

FOR j := 0 to 3
	i := j*16
	IF count[63:0] > 15
		dst[i+15:i] := (a[i+15] ? 0xFFFF : 0x0)
	ELSE
		dst[i+15:i] := SignExtend16(a[i+15:i] >> count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t sra(m64_i16_t a, m64_i64_t count) {
	return _mm_sra_pi16(a.xmm, count.xmm);
}

/*
Shift packed 32-bit integers in "a" right by "count" while shifting in sign
bits, and store the results in "dst".

psrad mm, mm

FOR j := 0 to 1
	i := j*32
	IF count[63:0] > 31
		dst[i+31:i] := (a[i+31] ? 0xFFFFFFFF : 0x0)
	ELSE
		dst[i+31:i] := SignExtend32(a[i+31:i] >> count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t sra(m64_i32_t a, m64_i64_t count) {
	return _mm_sra_pi32(a.xmm, count.xmm);
}

/*
Shift packed 16-bit integers in "a" right by "imm8" while shifting in sign
bits, and store the results in "dst".

psraw mm, imm8

FOR j := 0 to 3
	i := j*16
	IF imm8[7:0] > 15
		dst[i+15:i] := (a[i+15] ? 0xFFFF : 0x0)
	ELSE
		dst[i+15:i] := SignExtend16(a[i+15:i] >> imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i16_t srai(m64_i16_t a) {
	return _mm_srai_pi16(a.xmm, imm8);
}

/*
Shift packed 32-bit integers in "a" right by "imm8" while shifting in sign
bits, and store the results in "dst".

psrad mm, imm8

FOR j := 0 to 1
	i := j*32
	IF imm8[7:0] > 31
		dst[i+31:i] := (a[i+31] ? 0xFFFFFFFF : 0x0)
	ELSE
		dst[i+31:i] := SignExtend32(a[i+31:i] >> imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i32_t srai(m64_i32_t a) {
	return _mm_srai_pi32(a.xmm, imm8);
}

/*
Shift packed 16-bit integers in "a" right by "count" while shifting in zeros,
and store the results in "dst".

psrlw mm, mm

FOR j := 0 to 3
	i := j*16
	IF count[63:0] > 15
		dst[i+15:i] := 0
	ELSE
		dst[i+15:i] := ZeroExtend16(a[i+15:i] >> count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t srl(m64_i16_t a, m64_i64_t count) {
	return _mm_srl_pi16(a.xmm, count.xmm);
}

/*
Shift packed 32-bit integers in "a" right by "count" while shifting in zeros,
and store the results in "dst".

psrld mm, mm

FOR j := 0 to 1
	i := j*32
	IF count[63:0] > 31
		dst[i+31:i] := 0
	ELSE
		dst[i+31:i] := ZeroExtend32(a[i+31:i] >> count[63:0])
	FI
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t srl(m64_i32_t a, m64_i64_t count) {
	return _mm_srl_pi32(a.xmm, count.xmm);
}

/*
Shift 64-bit integer "a" right by "count" while shifting in zeros, and store
the result in "dst".

psrlq mm, mm

IF count[63:0] > 63
	dst[63:0] := 0
ELSE
	dst[63:0] := ZeroExtend64(a[63:0] >> count[63:0])
FI

*/
FEA_FORCEINLINE static m64_i64_t srl(m64_i64_t a, m64_i64_t count) {
	return _mm_srl_si64(a.xmm, count.xmm);
}

/*
Shift packed 16-bit integers in "a" right by "imm8" while shifting in zeros,
and store the results in "dst".

psrlw mm, imm8

FOR j := 0 to 3
	i := j*16
	IF imm8[7:0] > 15
		dst[i+15:i] := 0
	ELSE
		dst[i+15:i] := ZeroExtend16(a[i+15:i] >> imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i16_t srli(m64_i16_t a) {
	return _mm_srli_pi16(a.xmm, imm8);
}

/*
Shift packed 32-bit integers in "a" right by "imm8" while shifting in zeros,
and store the results in "dst".

psrld mm, imm8

FOR j := 0 to 1
	i := j*32
	IF imm8[7:0] > 31
		dst[i+31:i] := 0
	ELSE
		dst[i+31:i] := ZeroExtend32(a[i+31:i] >> imm8[7:0])
	FI
ENDFOR

*/
template <int imm8>
FEA_FORCEINLINE static m64_i32_t srli(m64_i32_t a) {
	return _mm_srli_pi32(a.xmm, imm8);
}

/*
Shift 64-bit integer "a" right by "imm8" while shifting in zeros, and store
the result in "dst".

psrlq mm, imm8

IF imm8[7:0] > 63
	dst[63:0] := 0
ELSE
	dst[63:0] := ZeroExtend64(a[63:0] >> imm8[7:0])
FI

*/
template <int imm8>
FEA_FORCEINLINE static m64_i64_t srli(m64_i64_t a) {
	return _mm_srli_si64(a.xmm, imm8);
}

/*
Subtract packed 16-bit integers in "b" from packed 16-bit integers in "a", and
store the results in "dst".

psubw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := a[i+15:i] - b[i+15:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t sub(m64_i16_t a, m64_i16_t b) {
	return _mm_sub_pi16(a.xmm, b.xmm);
}

/*
Subtract packed 32-bit integers in "b" from packed 32-bit integers in "a", and
store the results in "dst".

psubd mm, mm

FOR j := 0 to 1
	i := j*32
	dst[i+31:i] := a[i+31:i] - b[i+31:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i32_t sub(m64_i32_t a, m64_i32_t b) {
	return _mm_sub_pi32(a.xmm, b.xmm);
}

/*
Subtract packed 8-bit integers in "b" from packed 8-bit integers in "a", and
store the results in "dst".

psubb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := a[i+7:i] - b[i+7:i]
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t sub(m64_i8_t a, m64_i8_t b) {
	return _mm_sub_pi8(a.xmm, b.xmm);
}

/*
Subtract packed signed 16-bit integers in "b" from packed 16-bit integers in
"a" using saturation, and store the results in "dst".

psubsw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := Saturate16(a[i+15:i] - b[i+15:i])
ENDFOR

*/
FEA_FORCEINLINE static m64_i16_t subs(m64_i16_t a, m64_i16_t b) {
	return _mm_subs_pi16(a.xmm, b.xmm);
}

/*
Subtract packed signed 8-bit integers in "b" from packed 8-bit integers in "a"
using saturation, and store the results in "dst".

psubsb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := Saturate8(a[i+7:i] - b[i+7:i])	
ENDFOR

*/
FEA_FORCEINLINE static m64_i8_t subs(m64_i8_t a, m64_i8_t b) {
	return _mm_subs_pi8(a.xmm, b.xmm);
}

/*
Subtract packed unsigned 16-bit integers in "b" from packed unsigned 16-bit
integers in "a" using saturation, and store the results in "dst".

psubusw mm, mm

FOR j := 0 to 3
	i := j*16
	dst[i+15:i] := SaturateU16(a[i+15:i] - b[i+15:i])	
ENDFOR

*/
FEA_FORCEINLINE static m64_u16_t subs(m64_u16_t a, m64_u16_t b) {
	return _mm_subs_pu16(a.xmm, b.xmm);
}

/*
Subtract packed unsigned 8-bit integers in "b" from packed unsigned 8-bit
integers in "a" using saturation, and store the results in "dst".

psubusb mm, mm

FOR j := 0 to 7
	i := j*8
	dst[i+7:i] := SaturateU8(a[i+7:i] - b[i+7:i])	
ENDFOR

*/
FEA_FORCEINLINE static m64_u8_t subs(m64_u8_t a, m64_u8_t b) {
	return _mm_subs_pu8(a.xmm, b.xmm);
}

/*
Unpack and interleave 16-bit integers from the high half of "a" and "b", and
store the results in "dst".

punpcklbw mm, mm

DEFINE INTERLEAVE_HIGH_WORDS(src1[63:0], src2[63:0]) {
	dst[15:0] := src1[47:32]
	dst[31:16] := src2[47:32]
	dst[47:32] := src1[63:48]
	dst[63:48] := src2[63:48]
	RETURN dst[63:0]
}
dst[63:0] := INTERLEAVE_HIGH_WORDS(a[63:0], b[63:0])

*/
FEA_FORCEINLINE static m64_i16_t unpackhi(m64_i16_t a, m64_i16_t b) {
	return _mm_unpackhi_pi16(a.xmm, b.xmm);
}

/*
Unpack and interleave 32-bit integers from the high half of "a" and "b", and
store the results in "dst".

punpckhdq mm, mm

dst[31:0] := a[63:32]
dst[63:32] := b[63:32]

*/
FEA_FORCEINLINE static m64_i32_t unpackhi(m64_i32_t a, m64_i32_t b) {
	return _mm_unpackhi_pi32(a.xmm, b.xmm);
}

/*
Unpack and interleave 8-bit integers from the high half of "a" and "b", and
store the results in "dst".

punpckhbw mm, mm

DEFINE INTERLEAVE_HIGH_BYTES(src1[63:0], src2[63:0]) {
	dst[7:0] := src1[39:32]
	dst[15:8] := src2[39:32] 
	dst[23:16] := src1[47:40]
	dst[31:24] := src2[47:40]
	dst[39:32] := src1[55:48]
	dst[47:40] := src2[55:48]
	dst[55:48] := src1[63:56]
	dst[63:56] := src2[63:56]
	RETURN dst[63:0]	
}
dst[63:0] := INTERLEAVE_HIGH_BYTES(a[63:0], b[63:0])

*/
FEA_FORCEINLINE static m64_i8_t unpackhi(m64_i8_t a, m64_i8_t b) {
	return _mm_unpackhi_pi8(a.xmm, b.xmm);
}

/*
Unpack and interleave 16-bit integers from the low half of "a" and "b", and
store the results in "dst".

punpcklwd mm, mm

DEFINE INTERLEAVE_WORDS(src1[63:0], src2[63:0]) {
	dst[15:0] := src1[15:0] 
	dst[31:16] := src2[15:0] 
	dst[47:32] := src1[31:16] 
	dst[63:48] := src2[31:16] 
	RETURN dst[63:0]	
}
dst[63:0] := INTERLEAVE_WORDS(a[63:0], b[63:0])

*/
FEA_FORCEINLINE static m64_i16_t unpacklo(m64_i16_t a, m64_i16_t b) {
	return _mm_unpacklo_pi16(a.xmm, b.xmm);
}

/*
Unpack and interleave 32-bit integers from the low half of "a" and "b", and
store the results in "dst".

punpckldq mm, mm

dst[31:0] := a[31:0]
dst[63:32] := b[31:0]

*/
FEA_FORCEINLINE static m64_i32_t unpacklo(m64_i32_t a, m64_i32_t b) {
	return _mm_unpacklo_pi32(a.xmm, b.xmm);
}

/*
Unpack and interleave 8-bit integers from the low half of "a" and "b", and
store the results in "dst".

punpcklbw mm, mm

DEFINE INTERLEAVE_BYTES(src1[63:0], src2[63:0]) {
	dst[7:0] := src1[7:0] 
	dst[15:8] := src2[7:0] 
	dst[23:16] := src1[15:8] 
	dst[31:24] := src2[15:8] 
	dst[39:32] := src1[23:16] 
	dst[47:40] := src2[23:16] 
	dst[55:48] := src1[31:24] 
	dst[63:56] := src2[31:24] 
	RETURN dst[63:0]	
}
dst[63:0] := INTERLEAVE_BYTES(a[63:0], b[63:0])

*/
FEA_FORCEINLINE static m64_i8_t unpacklo(m64_i8_t a, m64_i8_t b) {
	return _mm_unpacklo_pi8(a.xmm, b.xmm);
}

/*
Compute the bitwise XOR of 64 bits (representing integer data) in "a" and "b",
and store the result in "dst".

pxor mm, mm

dst[63:0] := (a[63:0] XOR b[63:0])

*/
FEA_FORCEINLINE static m64_i64_t xor_(m64_i64_t a, m64_i64_t b) {
	return _mm_xor_si64(a.xmm, b.xmm);
}

#endif

};

using simd_mmx_api = simd_api<fea::simd_ver::mmx>;

} // namespace fea
