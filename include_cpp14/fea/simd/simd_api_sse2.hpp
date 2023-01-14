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
#include "fea/simd/simd_register.hpp"
#include "fea/simd/simd_register_traits.hpp"
#include "fea/simd/simd_version.hpp"
#include "fea/utils/platform.hpp"
#include "simd_api_sse.hpp"

#include <immintrin.h>

/*
SIMD intrinsics, sorted and grouped by abi support.

All APIs and information come from :
https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
*/

namespace fea {


template <>
struct simd_api<fea::simd_ver::sse2> : public simd_api<fea::simd_ver::sse> {
	// Default register types for this api.
	// Previous registers and functionality are still available.
	using xmm_f32_t = simd_abi_register_t<fea::simd_ver::sse2, float>;
	using xmm_f64_t = simd_abi_register_t<fea::simd_ver::sse2, double>;
	using xmm_i8_t = simd_abi_register_t<fea::simd_ver::sse2, char>;
	using xmm_u8_t = simd_abi_register_t<fea::simd_ver::sse2, unsigned char>;
	using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::sse2, signed char>;
	using xmm_i16_t = simd_abi_register_t<fea::simd_ver::sse2, short>;
	using xmm_u16_t = simd_abi_register_t<fea::simd_ver::sse2, unsigned short>;
	using xmm_i32_t = simd_abi_register_t<fea::simd_ver::sse2, int>;
	using xmm_u32_t = simd_abi_register_t<fea::simd_ver::sse2, unsigned int>;
	using xmm_i64_t = simd_abi_register_t<fea::simd_ver::sse2, long long>;
	using xmm_u64_t
			= simd_abi_register_t<fea::simd_ver::sse2, unsigned long long>;


	/*
	Add packed 16-bit integers in "a" and "b", and store the results in "dst".

	paddw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := a[i+15:i] + b[i+15:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t add(m128_i16_t a, m128_i16_t b) {
		return _mm_add_epi16(a.xmm, b.xmm);
	}

	/*
	Add packed 32-bit integers in "a" and "b", and store the results in "dst".

	paddd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := a[i+31:i] + b[i+31:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t add(m128_i32_t a, m128_i32_t b) {
		return _mm_add_epi32(a.xmm, b.xmm);
	}

	/*
	Add packed 64-bit integers in "a" and "b", and store the results in "dst".

	paddq xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] + b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i64_t add(m128_i64_t a, m128_i64_t b) {
		return _mm_add_epi64(a.xmm, b.xmm);
	}

	/*
	Add packed 8-bit integers in "a" and "b", and store the results in "dst".

	paddb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := a[i+7:i] + b[i+7:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t add(m128_i8_t a, m128_i8_t b) {
		return _mm_add_epi8(a.xmm, b.xmm);
	}

	/*
	Add packed double-precision (64-bit) floating-point elements in "a" and "b",
	and store the results in "dst".

	addpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] + b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t add(m128_f64_t a, m128_f64_t b) {
		return _mm_add_pd(a.xmm, b.xmm);
	}

	/*
	Add the lower double-precision (64-bit) floating-point element in "a" and
	"b", store the result in the lower element of "dst", and copy the upper
	element from "a" to the upper element of "dst".

	addsd xmm, xmm

	dst[63:0] := a[63:0] + b[63:0]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t add_s(m128_f64_t a, m128_f64_t b) {
		return _mm_add_sd(a.xmm, b.xmm);
	}

#if FEA_32BIT
	/*
	Add 64-bit integers "a" and "b", and store the result in "dst".

	paddq mm, mm

	dst[63:0] := a[63:0] + b[63:0]

	*/
	FEA_FORCEINLINE static m64_i64_t add(m64_i64_t a, m64_i64_t b) {
		return _mm_add_si64(a.xmm, b.xmm);
	}
#endif

	/*
	Add packed signed 16-bit integers in "a" and "b" using saturation, and store
	the results in "dst".

	paddsw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := Saturate16( a[i+15:i] + b[i+15:i] )
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t adds(m128_i16_t a, m128_i16_t b) {
		return _mm_adds_epi16(a.xmm, b.xmm);
	}

	/*
	Add packed signed 8-bit integers in "a" and "b" using saturation, and store
	the results in "dst".

	paddsb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := Saturate8( a[i+7:i] + b[i+7:i] )
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t adds(m128_i8_t a, m128_i8_t b) {
		return _mm_adds_epi8(a.xmm, b.xmm);
	}

	/*
	Add packed unsigned 16-bit integers in "a" and "b" using saturation, and
	store the results in "dst".

	paddusw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := SaturateU16( a[i+15:i] + b[i+15:i] )
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u16_t adds(m128_u16_t a, m128_u16_t b) {
		return _mm_adds_epu16(a.xmm, b.xmm);
	}

	/*
	Add packed unsigned 8-bit integers in "a" and "b" using saturation, and
	store the results in "dst".

	paddusb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := SaturateU8( a[i+7:i] + b[i+7:i] )
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u8_t adds(m128_u8_t a, m128_u8_t b) {
		return _mm_adds_epu8(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of packed double-precision (64-bit) floating-point
	elements in "a" and "b", and store the results in "dst".

	andpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] AND b[i+63:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t and_(m128_f64_t a, m128_f64_t b) {
		return _mm_and_pd(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t and_(m128_i8_t a, m128_i8_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t and_(m128_i16_t a, m128_i16_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t and_(m128_i32_t a, m128_i32_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t and_(m128_i64_t a, m128_i64_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u8_t and_(m128_u8_t a, m128_u8_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u16_t and_(m128_u16_t a, m128_u16_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u32_t and_(m128_u32_t a, m128_u32_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise AND of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pand xmm, xmm

	dst[127:0] := (a[127:0] AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u64_t and_(m128_u64_t a, m128_u64_t b) {
		return _mm_and_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of packed double-precision (64-bit) floating-point
	elements in "a" and then AND with "b", and store the results in "dst".

	andnpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := ((NOT a[i+63:i]) AND b[i+63:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t andnot(m128_f64_t a, m128_f64_t b) {
		return _mm_andnot_pd(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t andnot(m128_i8_t a, m128_i8_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t andnot(m128_i16_t a, m128_i16_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t andnot(m128_i32_t a, m128_i32_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t andnot(m128_i64_t a, m128_i64_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u8_t andnot(m128_u8_t a, m128_u8_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u16_t andnot(m128_u16_t a, m128_u16_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u32_t andnot(m128_u32_t a, m128_u32_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise NOT of 128 bits (representing integer data) in "a" and
	then AND with "b", and store the result in "dst".

	pandn xmm, xmm

	dst[127:0] := ((NOT a[127:0]) AND b[127:0])

	*/
	FEA_FORCEINLINE static m128_u64_t andnot(m128_u64_t a, m128_u64_t b) {
		return _mm_andnot_si128(a.xmm, b.xmm);
	}

	/*
	Average packed unsigned 16-bit integers in "a" and "b", and store the
	results in "dst".

	pavgw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := (a[i+15:i] + b[i+15:i] + 1) >> 1
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u16_t avg(m128_u16_t a, m128_u16_t b) {
		return _mm_avg_epu16(a.xmm, b.xmm);
	}

	/*
	Average packed unsigned 8-bit integers in "a" and "b", and store the results
	in "dst".

	pavgb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := (a[i+7:i] + b[i+7:i] + 1) >> 1
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u8_t avg(m128_u8_t a, m128_u8_t b) {
		return _mm_avg_epu8(a.xmm, b.xmm);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i8_t bslli(m128_i8_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t bslli(m128_i16_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t bslli(m128_i32_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i64_t bslli(m128_i64_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u8_t bslli(m128_u8_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u16_t bslli(m128_u16_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u32_t bslli(m128_u32_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" left by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	pslldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] << (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u64_t bslli(m128_u64_t a) {
		return _mm_bslli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i8_t bsrli(m128_i8_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t bsrli(m128_i16_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t bsrli(m128_i32_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i64_t bsrli(m128_i64_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u8_t bsrli(m128_u8_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u16_t bsrli(m128_u16_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u32_t bsrli(m128_u32_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Shift "a" right by "imm8" bytes while shifting in zeros, and store the
	results in "dst".

	psrldq xmm, imm8

	tmp := imm8[7:0]
	IF tmp > 15
		tmp := 16
	FI
	dst[127:0] := a[127:0] >> (tmp*8)

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_u64_t bsrli(m128_u64_t a) {
		return _mm_bsrli_si128(a.xmm, imm8);
	}

	/*
	Cast vector of type __m128d to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castpd_ps(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_i8_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_i16_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_i32_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_i64_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_u8_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_u16_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_u32_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128d to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f64_t a, m128_u64_t& dst) {
		dst.xmm = _mm_castpd_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castps_pd(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_i8_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_i16_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_i32_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_i64_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_u8_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_u16_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_u32_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128 to type __m128i. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_f32_t a, m128_u64_t& dst) {
		dst.xmm = _mm_castps_si128(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i8_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i16_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i64_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u8_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u16_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128d. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u64_t a, m128_f64_t& dst) {
		dst.xmm = _mm_castsi128_pd(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i8_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i16_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i32_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_i64_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u8_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u16_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u32_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Cast vector of type __m128i to type __m128. This intrinsic is only used for
	compilation and does not generate any instructions, thus it has zero
	latency.



	*/
	FEA_FORCEINLINE static void cast(m128_u64_t a, m128_f32_t& dst) {
		dst.xmm = _mm_castsi128_ps(a.xmm);
	}

	/*
	Invalidate and flush the cache line that contains "p" from all levels of the
	cache hierarchy.

	clflush m8


	*/
	FEA_FORCEINLINE static void clflush(void const* p) {
		_mm_clflush(p);
	}

	/*
	Compare packed 16-bit integers in "a" and "b" for equality, and store the
	results in "dst".

	pcmpeqw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := ( a[i+15:i] == b[i+15:i] ) ? 0xFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t cmpeq(m128_i16_t a, m128_i16_t b) {
		return _mm_cmpeq_epi16(a.xmm, b.xmm);
	}

	/*
	Compare packed 32-bit integers in "a" and "b" for equality, and store the
	results in "dst".

	pcmpeqd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t cmpeq(m128_i32_t a, m128_i32_t b) {
		return _mm_cmpeq_epi32(a.xmm, b.xmm);
	}

	/*
	Compare packed 8-bit integers in "a" and "b" for equality, and store the
	results in "dst".

	pcmpeqb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := ( a[i+7:i] == b[i+7:i] ) ? 0xFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t cmpeq(m128_i8_t a, m128_i8_t b) {
		return _mm_cmpeq_epi8(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for equality, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] == b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpeq(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpeq_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for equality, store the result in the lower element of "dst", and
	copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] == b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpeq_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpeq_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for greater-than-or-equal, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] >= b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpge(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpge_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for greater-than-or-equal, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] >= b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpge_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpge_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 16-bit integers in "a" and "b" for greater-than, and
	store the results in "dst".

	pcmpgtw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := ( a[i+15:i] > b[i+15:i] ) ? 0xFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t cmpgt(m128_i16_t a, m128_i16_t b) {
		return _mm_cmpgt_epi16(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 32-bit integers in "a" and "b" for greater-than, and
	store the results in "dst".

	pcmpgtd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := ( a[i+31:i] > b[i+31:i] ) ? 0xFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t cmpgt(m128_i32_t a, m128_i32_t b) {
		return _mm_cmpgt_epi32(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 8-bit integers in "a" and "b" for greater-than, and
	store the results in "dst".

	pcmpgtb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := ( a[i+7:i] > b[i+7:i] ) ? 0xFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t cmpgt(m128_i8_t a, m128_i8_t b) {
		return _mm_cmpgt_epi8(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for greater-than, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] > b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpgt(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpgt_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for greater-than, store the result in the lower element of "dst",
	and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] > b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpgt_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpgt_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for less-than-or-equal, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] <= b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmple(m128_f64_t a, m128_f64_t b) {
		return _mm_cmple_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for less-than-or-equal, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] <= b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmple_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmple_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 16-bit integers in "a" and "b" for less-than, and
	store the results in "dst". Note: This intrinsic emits the pcmpgtw
	instruction with the order of the operands switched.

	pcmpgtw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := ( a[i+15:i] < b[i+15:i] ) ? 0xFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t cmplt(m128_i16_t a, m128_i16_t b) {
		return _mm_cmplt_epi16(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 32-bit integers in "a" and "b" for less-than, and
	store the results in "dst". Note: This intrinsic emits the pcmpgtd
	instruction with the order of the operands switched.

	pcmpgtd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := ( a[i+31:i] < b[i+31:i] ) ? 0xFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t cmplt(m128_i32_t a, m128_i32_t b) {
		return _mm_cmplt_epi32(a.xmm, b.xmm);
	}

	/*
	Compare packed signed 8-bit integers in "a" and "b" for less-than, and store
	the results in "dst". Note: This intrinsic emits the pcmpgtb instruction
	with the order of the operands switched.

	pcmpgtb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := ( a[i+7:i] < b[i+7:i] ) ? 0xFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t cmplt(m128_i8_t a, m128_i8_t b) {
		return _mm_cmplt_epi8(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for less-than, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] < b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmplt(m128_f64_t a, m128_f64_t b) {
		return _mm_cmplt_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for less-than, store the result in the lower element of "dst", and
	copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] < b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmplt_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmplt_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for not-equal, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] != b[i+63:i]) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpneq(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpneq_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for not-equal, store the result in the lower element of "dst", and
	copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] != b[63:0]) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpneq_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpneq_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for not-greater-than-or-equal, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (!(a[i+63:i] >= b[i+63:i])) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnge(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnge_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for not-greater-than-or-equal, store the result in the lower element
	of "dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (!(a[63:0] >= b[63:0])) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnge_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnge_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for not-greater-than, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (!(a[i+63:i] > b[i+63:i])) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpngt(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpngt_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for not-greater-than, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (!(a[63:0] > b[63:0])) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpngt_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpngt_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for not-less-than-or-equal, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (!(a[i+63:i] <= b[i+63:i])) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnle(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnle_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for not-less-than-or-equal, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (!(a[63:0] <= b[63:0])) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnle_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnle_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" for not-less-than, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (!(a[i+63:i] < b[i+63:i])) ? 0xFFFFFFFFFFFFFFFF : 0
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnlt(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnlt_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" for not-less-than, store the result in the lower element of "dst",
	and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (!(a[63:0] < b[63:0])) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpnlt_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpnlt_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" to see if neither is NaN, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] != NaN AND b[i+63:i] != NaN) ?
	0xFFFFFFFFFFFFFFFF : 0 ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpord(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpord_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" to see if neither is NaN, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] != NaN AND b[63:0] != NaN) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpord_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpord_sd(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b" to see if either is NaN, and store the results in "dst".

	cmppd xmm, xmm, imm8

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := (a[i+63:i] == NaN OR b[i+63:i] == NaN) ?
	0xFFFFFFFFFFFFFFFF : 0 ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t cmpunord(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpunord_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b" to see if either is NaN, store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	cmpsd xmm, xmm, imm8

	dst[63:0] := (a[63:0] == NaN OR b[63:0] == NaN) ? 0xFFFFFFFFFFFFFFFF : 0
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t cmpunord_s(m128_f64_t a, m128_f64_t b) {
		return _mm_cmpunord_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for equality, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] == b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comieq(m128_f64_t a, m128_f64_t b) {
		return _mm_comieq_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for greater-than-or-equal, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] >= b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comige(m128_f64_t a, m128_f64_t b) {
		return _mm_comige_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for greater-than, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] > b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comigt(m128_f64_t a, m128_f64_t b) {
		return _mm_comigt_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for less-than-or-equal, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] <= b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comile(m128_f64_t a, m128_f64_t b) {
		return _mm_comile_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for less-than, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] < b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comilt(m128_f64_t a, m128_f64_t b) {
		return _mm_comilt_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for not-equal, and return the boolean result (0 or 1).

	comisd xmm, xmm

	RETURN ( a[63:0] == NaN OR b[63:0] == NaN OR a[63:0] != b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool comineq(m128_f64_t a, m128_f64_t b) {
		return _mm_comineq_sd(a.xmm, b.xmm);
	}

	/*
	Convert packed signed 32-bit integers in "a" to packed double-precision
	(64-bit) floating-point elements, and store the results in "dst".

	cvtdq2pd xmm, xmm

	FOR j := 0 to 1
		i := j*32
		m := j*64
		dst[m+63:m] := Convert_Int32_To_FP64(a[i+31:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_i32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_cvtepi32_pd(a.xmm);
	}

	/*
	Convert packed signed 32-bit integers in "a" to packed single-precision
	(32-bit) floating-point elements, and store the results in "dst".

	cvtdq2ps xmm, xmm

	FOR j := 0 to 3
		i := 32*j
		dst[i+31:i] := Convert_Int32_To_FP32(a[i+31:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_i32_t a, m128_f32_t& dst) {
		dst.xmm = _mm_cvtepi32_ps(a.xmm);
	}

	/*
	Convert packed double-precision (64-bit) floating-point elements in "a" to
	packed 32-bit integers, and store the results in "dst".

	cvtpd2dq xmm, xmm

	FOR j := 0 to 1
		i := 32*j
		k := 64*j
		dst[i+31:i] := Convert_FP64_To_Int32(a[k+63:k])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_f64_t a, m128_i32_t& dst) {
		dst.xmm = _mm_cvtpd_epi32(a.xmm);
	}

#if FEA_32BIT
	/*
	Convert packed double-precision (64-bit) floating-point elements in "a" to
	packed 32-bit integers, and store the results in "dst".

	cvtpd2pi mm, xmm

	FOR j := 0 to 1
		i := 32*j
		k := 64*j
		dst[i+31:i] := Convert_FP64_To_Int32(a[k+63:k])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_f64_t a, m64_i32_t& dst) {
		dst.xmm = _mm_cvtpd_pi32(a.xmm);
	}
#endif

	/*
	Convert packed double-precision (64-bit) floating-point elements in "a" to
	packed single-precision (32-bit) floating-point elements, and store the
	results in "dst".

	cvtpd2ps xmm, xmm

	FOR j := 0 to 1
		i := 32*j
		k := 64*j
		dst[i+31:i] := Convert_FP64_To_FP32(a[k+63:k])
	ENDFOR
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_f64_t a, m128_f32_t& dst) {
		dst.xmm = _mm_cvtpd_ps(a.xmm);
	}

#if FEA_32BIT
	/*
	Convert packed signed 32-bit integers in "a" to packed double-precision
	(64-bit) floating-point elements, and store the results in "dst".

	cvtpi2pd xmm, mm

	FOR j := 0 to 1
		i := j*32
		m := j*64
		dst[m+63:m] := Convert_Int32_To_FP64(a[i+31:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m64_i32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_cvtpi32_pd(a.xmm);
	}
#endif

	/*
	Convert packed single-precision (32-bit) floating-point elements in "a" to
	packed 32-bit integers, and store the results in "dst".

	cvtps2dq xmm, xmm

	FOR j := 0 to 3
		i := 32*j
		dst[i+31:i] := Convert_FP32_To_Int32(a[i+31:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m128_i32_t& dst) {
		dst.xmm = _mm_cvtps_epi32(a.xmm);
	}

	/*
	Convert packed single-precision (32-bit) floating-point elements in "a" to
	packed double-precision (64-bit) floating-point elements, and store the
	results in "dst".

	cvtps2pd xmm, xmm

	FOR j := 0 to 1
		i := 64*j
		k := 32*j
		dst[i+63:i] := Convert_FP32_To_FP64(a[k+31:k])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvt_p2p(m128_f32_t a, m128_f64_t& dst) {
		dst.xmm = _mm_cvtps_pd(a.xmm);
	}

	/*
	Copy the lower double-precision (64-bit) floating-point element of "a" to
	"dst".

	movsd m64, xmm

	dst[63:0] := a[63:0]

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_f64_t a, double& dst) {
		dst = _mm_cvtsd_f64(a.xmm);
	}

	/*
	Convert the lower double-precision (64-bit) floating-point element in "a" to
	a 32-bit integer, and store the result in "dst".

	cvtsd2si r32, xmm

	dst[31:0] := Convert_FP64_To_Int32(a[63:0])

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_f64_t a, int& dst) {
		dst = _mm_cvtsd_si32(a.xmm);
	}

	/*
	Convert the lower double-precision (64-bit) floating-point element in "a" to
	a 64-bit integer, and store the result in "dst".

	cvtsd2si r64, xmm

	dst[63:0] := Convert_FP64_To_Int64(a[63:0])

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_f64_t a, __int64& dst) {
		dst = _mm_cvtsd_si64(a.xmm);
	}

	/*
	Convert the lower double-precision (64-bit) floating-point element in "b" to
	a single-precision (32-bit) floating-point element, store the result in the
	lower element of "dst", and copy the upper 3 packed elements from "a" to the
	upper elements of "dst".

	cvtsd2ss xmm, xmm

	dst[31:0] := Convert_FP64_To_FP32(b[63:0])
	dst[127:32] := a[127:32]
	dst[MAX:128] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(
			m128_f32_t a, m128_f64_t b, m128_f32_t& dst) {
		dst.xmm = _mm_cvtsd_ss(a.xmm, b.xmm);
	}

	/*
	Copy the lower 32-bit integer in "a" to "dst".

	movd r32, xmm

	dst[31:0] := a[31:0]

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_i32_t a, int& dst) {
		dst = _mm_cvtsi128_si32(a.xmm);
	}

	/*
	Copy the lower 64-bit integer in "a" to "dst".

	movq r64, xmm

	dst[63:0] := a[63:0]

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_i64_t a, __int64& dst) {
		dst = _mm_cvtsi128_si64(a.xmm);
	}

	/*
	Convert the signed 32-bit integer "b" to a double-precision (64-bit)
	floating-point element, store the result in the lower element of "dst", and
	copy the upper element from "a" to the upper element of "dst".

	cvtsi2sd xmm, r32

	dst[63:0] := Convert_Int32_To_FP64(b[31:0])
	dst[127:64] := a[127:64]
	dst[MAX:128] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(m128_f64_t a, int b, m128_f64_t& dst) {
		dst.xmm = _mm_cvtsi32_sd(a.xmm, b);
	}

	/*
	Copy 32-bit integer "a" to the lower elements of "dst", and zero the upper
	elements of "dst".

	movd xmm, r32

	dst[31:0] := a[31:0]
	dst[127:32] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(int a, m128_i32_t& dst) {
		dst.xmm = _mm_cvtsi32_si128(a);
	}

	/*
	Convert the signed 64-bit integer "b" to a double-precision (64-bit)
	floating-point element, store the result in the lower element of "dst", and
	copy the upper element from "a" to the upper element of "dst".

	cvtsi2sd xmm, r64

	dst[63:0] := Convert_Int64_To_FP64(b[63:0])
	dst[127:64] := a[127:64]
	dst[MAX:128] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(
			m128_f64_t a, __int64 b, m128_f64_t& dst) {
		dst.xmm = _mm_cvtsi64_sd(a.xmm, b);
	}

	/*
	Copy 64-bit integer "a" to the lower element of "dst", and zero the upper
	element.

	movq xmm, r64

	dst[63:0] := a[63:0]
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(__int64 a, m128_i64_t& dst) {
		dst.xmm = _mm_cvtsi64_si128(a);
	}

	/*
	Convert the lower single-precision (32-bit) floating-point element in "b" to
	a double-precision (64-bit) floating-point element, store the result in the
	lower element of "dst", and copy the upper element from "a" to the upper
	element of "dst".

	cvtss2sd xmm, xmm

	dst[63:0] := Convert_FP32_To_FP64(b[31:0])
	dst[127:64] := a[127:64]
	dst[MAX:128] := 0

	*/
	FEA_FORCEINLINE static void cvt_s2s(
			m128_f64_t a, m128_f32_t b, m128_f64_t& dst) {
		dst.xmm = _mm_cvtss_sd(a.xmm, b.xmm);
	}

	/*
	Convert packed double-precision (64-bit) floating-point elements in "a" to
	packed 32-bit integers with truncation, and store the results in "dst".

	cvttpd2dq xmm, xmm

	FOR j := 0 to 1
		i := 32*j
		k := 64*j
		dst[i+31:i] := Convert_FP64_To_Int32_Truncate(a[k+63:k])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvtt_p2p(m128_f64_t a, m128_i32_t& dst) {
		dst.xmm = _mm_cvttpd_epi32(a.xmm);
	}

#if FEA_32BIT
	/*
	Convert packed double-precision (64-bit) floating-point elements in "a" to
	packed 32-bit integers with truncation, and store the results in "dst".

	cvttpd2pi mm, xmm

	FOR j := 0 to 1
		i := 32*j
		k := 64*j
		dst[i+31:i] := Convert_FP64_To_Int32_Truncate(a[k+63:k])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvtt_p2p(m128_f64_t a, m64_i32_t& dst) {
		dst.xmm = _mm_cvttpd_pi32(a.xmm);
	}
#endif

	/*
	Convert packed single-precision (32-bit) floating-point elements in "a" to
	packed 32-bit integers with truncation, and store the results in "dst".

	cvttps2dq xmm, xmm

	FOR j := 0 to 3
		i := 32*j
		dst[i+31:i] := Convert_FP32_To_Int32_Truncate(a[i+31:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static void cvtt_p2p(m128_f32_t a, m128_i32_t& dst) {
		dst.xmm = _mm_cvttps_epi32(a.xmm);
	}

	/*
	Convert the lower double-precision (64-bit) floating-point element in "a" to
	a 32-bit integer with truncation, and store the result in "dst".

	cvttsd2si r32, xmm

	dst[31:0] := Convert_FP64_To_Int32_Truncate(a[63:0])

	*/
	FEA_FORCEINLINE static void cvtt_s2s(m128_f64_t a, int& dst) {
		dst = _mm_cvttsd_si32(a.xmm);
	}

	/*
	Convert the lower double-precision (64-bit) floating-point element in "a" to
	a 64-bit integer with truncation, and store the result in "dst".

	cvttsd2si r64, xmm

	dst[63:0] := Convert_FP64_To_Int64_Truncate(a[63:0])

	*/
	FEA_FORCEINLINE static void cvtt_s2s(m128_f64_t a, __int64& dst) {
		dst = _mm_cvttsd_si64(a.xmm);
	}

	/*
	Divide packed double-precision (64-bit) floating-point elements in "a" by
	packed elements in "b", and store the results in "dst".

	divpd xmm, xmm

	FOR j := 0 to 1
		i := 64*j
		dst[i+63:i] := a[i+63:i] / b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t div(m128_f64_t a, m128_f64_t b) {
		return _mm_div_pd(a.xmm, b.xmm);
	}

	/*
	Divide the lower double-precision (64-bit) floating-point element in "a" by
	the lower double-precision (64-bit) floating-point element in "b", store the
	result in the lower element of "dst", and copy the upper element from "a" to
	the upper element of "dst".

	divsd xmm, xmm

	dst[63:0] := a[63:0] / b[63:0]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t div_s(m128_f64_t a, m128_f64_t b) {
		return _mm_div_sd(a.xmm, b.xmm);
	}

	/*
	Extract a 16-bit integer from "a", selected with "imm8", and store the
	result in the lower element of "dst".

	pextrw r32, xmm, imm8

	dst[15:0] := (a[127:0] >> (imm8[2:0] * 16))[15:0]
	dst[31:16] := 0

	*/
	template <int imm8>
	FEA_FORCEINLINE static int extract(m128_i16_t a) {
		return _mm_extract_epi16(a.xmm, imm8);
	}

	/*
	Copy "a" to "dst", and insert the 16-bit integer "i" into "dst" at the
	location specified by "imm8".

	pinsrw xmm, r32, imm8

	dst[127:0] := a[127:0]
	sel := imm8[2:0]*16
	dst[sel+15:sel] := i[15:0]

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t extract(m128_i16_t a, int i) {
		return _mm_insert_epi16(a.xmm, i, imm8);
	}

	/*
	Perform a serializing operation on all load-from-memory instructions that
	were issued prior to this instruction. Guarantees that every load
	instruction that precedes, in program order, is globally visible before any
	load instruction which follows the fence in program order.

	lfence


	*/
	FEA_FORCEINLINE static void lfence() {
		_mm_lfence();
	}

	/*
	Load a double-precision (64-bit) floating-point element from memory into
	both elements of "dst".

	SEQUENCE

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[127:64] := MEM[mem_addr+63:mem_addr]

	*/
	FEA_FORCEINLINE static void load1(double const& mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_load1_pd(&mem_addr);
	}

	/*
	Load 128-bits (composed of 2 packed double-precision (64-bit) floating-point
	elements) from memory into "dst". "mem_addr" must be aligned on a 16-byte
	boundary or a general-protection exception may be generated.

	movapd xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(double const* mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_load_pd(mem_addr);
	}

	/*
	Load a double-precision (64-bit) floating-point element from memory into
	both elements of "dst".

	SEQUENCE

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[127:64] := MEM[mem_addr+63:mem_addr]

	*/
	FEA_FORCEINLINE static void load_p1(
			double const& mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_load_pd1(&mem_addr);
	}

	/*
	Load a double-precision (64-bit) floating-point element from memory into the
	lower of "dst", and zero the upper element. "mem_addr" does not need to be
	aligned on any particular boundary.

	movsd xmm, m64

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static void load_s(
			double const& mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_load_sd(&mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(char const* mem_addr, m128_i8_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(int const* mem_addr, m128_i32_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(
			long long const* mem_addr, m128_i64_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(short const* mem_addr, m128_i16_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(
			unsigned char const* mem_addr, m128_u8_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(
			unsigned int const* mem_addr, m128_u32_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(
			unsigned long long const* mem_addr, m128_u64_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst".  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void load(
			unsigned short const* mem_addr, m128_u16_t& dst) {
		dst.xmm = _mm_load_si128((__m128i const*)mem_addr);
	}

	/*
	Load a double-precision (64-bit) floating-point element from memory into the
	upper element of "dst", and copy the lower element from "a" to "dst".
	"mem_addr" does not need to be aligned on any particular boundary.

	movhpd xmm, m64

	dst[63:0] := a[63:0]
	dst[127:64] := MEM[mem_addr+63:mem_addr]

	*/
	FEA_FORCEINLINE static void loadh(
			m128_f64_t a, double const* mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_loadh_pd(a.xmm, mem_addr);
	}

	/*
	Load 64-bit integer from memory into the first element of "dst".

	movq xmm, m64

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[MAX:64] := 0

	*/
	FEA_FORCEINLINE static void loadl(
			long long const* mem_addr, m128_i64_t& dst) {
		dst.xmm = _mm_loadl_epi64((__m128i const*)mem_addr);
	}

	/*
	Load a double-precision (64-bit) floating-point element from memory into the
	lower element of "dst", and copy the upper element from "a" to "dst".
	"mem_addr" does not need to be aligned on any particular boundary.

	movlpd xmm, m64

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static void loadl(
			m128_f64_t a, double const* mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_loadl_pd(a.xmm, mem_addr);
	}

	/*
	Load 2 double-precision (64-bit) floating-point elements from memory into
	"dst" in reverse order. mem_addr must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	SEQUENCE

	dst[63:0] := MEM[mem_addr+127:mem_addr+64]
	dst[127:64] := MEM[mem_addr+63:mem_addr]

	*/
	FEA_FORCEINLINE static void loadr(double const* mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_loadr_pd(mem_addr);
	}

	/*
	Load 128-bits (composed of 2 packed double-precision (64-bit) floating-point
	elements) from memory into "dst". "mem_addr" does not need to be aligned on
	any particular boundary.

	movupd xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(double const* mem_addr, m128_f64_t& dst) {
		dst.xmm = _mm_loadu_pd(mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(char const* mem_addr, m128_i8_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(int const* mem_addr, m128_i32_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(
			long long const* mem_addr, m128_i64_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(short const* mem_addr, m128_i16_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(
			unsigned char const* mem_addr, m128_u8_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(
			unsigned int const* mem_addr, m128_u32_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(
			unsigned long long const* mem_addr, m128_u64_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load 128-bits of integer data from memory into "dst". "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu xmm, m128

	dst[127:0] := MEM[mem_addr+127:mem_addr]

	*/
	FEA_FORCEINLINE static void loadu(
			unsigned short const* mem_addr, m128_u16_t& dst) {
		dst.xmm = _mm_loadu_si128((__m128i const*)mem_addr);
	}

	/*
	Load unaligned 16-bit integer from memory into the first element of "dst".

	SEQUENCE

	dst[15:0] := MEM[mem_addr+15:mem_addr]
	dst[MAX:16] := 0

	*/
	FEA_FORCEINLINE static void loadu_s(
			short const& mem_addr, m128_i16_t& dst) {
		dst.xmm = _mm_loadu_si16(&mem_addr);
	}

	/*
	Load unaligned 32-bit integer from memory into the first element of "dst".

	movd xmm, m32

	dst[31:0] := MEM[mem_addr+31:mem_addr]
	dst[MAX:32] := 0

	*/
	FEA_FORCEINLINE static void loadu_s(int const& mem_addr, m128_i32_t& dst) {
		dst.xmm = _mm_loadu_si32(&mem_addr);
	}

	/*
	Load unaligned 64-bit integer from memory into the first element of "dst".

	movq xmm, m64

	dst[63:0] := MEM[mem_addr+63:mem_addr]
	dst[MAX:64] := 0

	*/
	FEA_FORCEINLINE static void loadu_s(
			long long const& mem_addr, m128_i64_t& dst) {
		dst.xmm = _mm_loadu_si64(&mem_addr);
	}

	/*
	Multiply packed signed 16-bit integers in "a" and "b", producing
	intermediate signed 32-bit integers. Horizontally add adjacent pairs of
	intermediate 32-bit integers, and pack the results in "dst".

	pmaddwd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := SignExtend32(a[i+31:i+16]*b[i+31:i+16]) +
	SignExtend32(a[i+15:i]*b[i+15:i]) ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t madd(m128_i16_t a, m128_i16_t b) {
		return _mm_madd_epi16(a.xmm, b.xmm);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_i8_t a, m128_u8_t mask, char* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_i16_t a, m128_u8_t mask, short* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_i32_t a, m128_u8_t mask, int* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_i64_t a, m128_u8_t mask, long long* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_u8_t a, m128_u8_t mask, unsigned char* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_u16_t a, m128_u8_t mask, unsigned short* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_u32_t a, m128_u8_t mask, unsigned int* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Conditionally store 8-bit integer elements from "a" into memory using "mask"
	(elements are not stored when the highest bit is not set in the
	corresponding element) and a non-temporal memory hint. "mem_addr" does not
	need to be aligned on any particular boundary.

	maskmovdqu xmm, xmm

	FOR j := 0 to 15
		i := j*8
		IF mask[i+7]
			MEM[mem_addr+i+7:mem_addr+i] := a[i+7:i]
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static void maskmoveu(
			m128_u64_t a, m128_u8_t mask, unsigned long long* mem_addr) {
		_mm_maskmoveu_si128(a.xmm, mask.xmm, (char*)mem_addr);
	}

	/*
	Compare packed signed 16-bit integers in "a" and "b", and store packed
	maximum values in "dst".

	pmaxsw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := MAX(a[i+15:i], b[i+15:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t max(m128_i16_t a, m128_i16_t b) {
		return _mm_max_epi16(a.xmm, b.xmm);
	}

	/*
	Compare packed unsigned 8-bit integers in "a" and "b", and store packed
	maximum values in "dst".

	pmaxub xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := MAX(a[i+7:i], b[i+7:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u8_t max(m128_u8_t a, m128_u8_t b) {
		return _mm_max_epu8(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b", and store packed maximum values in "dst". [max_float_note]

	maxpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := MAX(a[i+63:i], b[i+63:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t max(m128_f64_t a, m128_f64_t b) {
		return _mm_max_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b", store the maximum value in the lower element of "dst", and copy the
	upper element from "a" to the upper element of "dst". [max_float_note]

	maxsd xmm, xmm

	dst[63:0] := MAX(a[63:0], b[63:0])
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t max_s(m128_f64_t a, m128_f64_t b) {
		return _mm_max_sd(a.xmm, b.xmm);
	}

	/*
	Perform a serializing operation on all load-from-memory and store-to-memory
	instructions that were issued prior to this instruction. Guarantees that
	every memory access that precedes, in program order, the memory fence
	instruction is globally visible before any memory instruction which follows
	the fence in program order.

	mfence


	*/
	FEA_FORCEINLINE static void mfence() {
		_mm_mfence();
	}

	/*
	Compare packed signed 16-bit integers in "a" and "b", and store packed
	minimum values in "dst".

	pminsw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := MIN(a[i+15:i], b[i+15:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t min(m128_i16_t a, m128_i16_t b) {
		return _mm_min_epi16(a.xmm, b.xmm);
	}

	/*
	Compare packed unsigned 8-bit integers in "a" and "b", and store packed
	minimum values in "dst".

	pminub xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := MIN(a[i+7:i], b[i+7:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u8_t min(m128_u8_t a, m128_u8_t b) {
		return _mm_min_epu8(a.xmm, b.xmm);
	}

	/*
	Compare packed double-precision (64-bit) floating-point elements in "a" and
	"b", and store packed minimum values in "dst". [min_float_note]

	minpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := MIN(a[i+63:i], b[i+63:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t min(m128_f64_t a, m128_f64_t b) {
		return _mm_min_pd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point elements in "a"
	and "b", store the minimum value in the lower element of "dst", and copy the
	upper element from "a" to the upper element of "dst". [min_float_note]

	minsd xmm, xmm

	dst[63:0] := MIN(a[63:0], b[63:0])
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t min_s(m128_f64_t a, m128_f64_t b) {
		return _mm_min_sd(a.xmm, b.xmm);
	}

	/*
	Copy the lower 64-bit integer in "a" to the lower element of "dst", and zero
	the upper element.

	movq xmm, xmm

	dst[63:0] := a[63:0]
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static m128_i64_t move(m128_i64_t a) {
		return _mm_move_epi64(a.xmm);
	}

	/*
	Move the lower double-precision (64-bit) floating-point element from "b" to
	the lower element of "dst", and copy the upper element from "a" to the upper
	element of "dst".

	movsd xmm, xmm

	dst[63:0] := b[63:0]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t move_s(m128_f64_t a, m128_f64_t b) {
		return _mm_move_sd(a.xmm, b.xmm);
	}

	/*
	Create mask from the most significant bit of each 8-bit element in "a", and
	store the result in "dst".

	pmovmskb r32, xmm

	FOR j := 0 to 15
		i := j*8
		dst[j] := a[i+7]
	ENDFOR
	dst[MAX:16] := 0

	*/
	FEA_FORCEINLINE static int movemask(m128_i8_t a) {
		return _mm_movemask_epi8(a.xmm);
	}

	/*
	Set each bit of mask "dst" based on the most significant bit of the
	corresponding packed double-precision (64-bit) floating-point element in
	"a".

	movmskpd r32, xmm

	FOR j := 0 to 1
		i := j*64
		IF a[i+63]
			dst[j] := 1
		ELSE
			dst[j] := 0
		FI
	ENDFOR
	dst[MAX:2] := 0

	*/
	FEA_FORCEINLINE static int movemask(m128_f64_t a) {
		return _mm_movemask_pd(a.xmm);
	}

#if FEA_32BIT
	/*
	Copy the lower 64-bit integer in "a" to "dst".

	movdq2q mm, xmm

	dst[63:0] := a[63:0]

	*/
	FEA_FORCEINLINE static void mov(m128_i64_t a, m64_i64_t& dst) {
		dst.xmm = _mm_movepi64_pi64(a.xmm);
	}
#endif

#if FEA_32BIT
	/*
	Copy the 64-bit integer "a" to the lower element of "dst", and zero the
	upper element.

	movq2dq xmm, mm

	dst[63:0] := a[63:0]
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static void mov(m64_i64_t a, m128_i64_t& dst) {
		dst.xmm = _mm_movpi64_epi64(a.xmm);
	}
#endif

	/*
	Multiply the low unsigned 32-bit integers from each packed 64-bit element in
	"a" and "b", and store the unsigned 64-bit results in "dst".

	pmuludq xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+31:i] * b[i+31:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u64_t mul_s(m128_u32_t a, m128_u32_t b) {
		return _mm_mul_epu32(a.xmm, b.xmm);
	}

	/*
	Multiply packed double-precision (64-bit) floating-point elements in "a" and
	"b", and store the results in "dst".

	mulpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] * b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t mul(m128_f64_t a, m128_f64_t b) {
		return _mm_mul_pd(a.xmm, b.xmm);
	}

	/*
	Multiply the lower double-precision (64-bit) floating-point element in "a"
	and "b", store the result in the lower element of "dst", and copy the upper
	element from "a" to the upper element of "dst".

	mulsd xmm, xmm

	dst[63:0] := a[63:0] * b[63:0]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t mul_s(m128_f64_t a, m128_f64_t b) {
		return _mm_mul_sd(a.xmm, b.xmm);
	}

#if FEA_32BIT
	/*
	Multiply the low unsigned 32-bit integers from "a" and "b", and store the
	unsigned 64-bit result in "dst".

	pmuludq mm, mm

	dst[63:0] := a[31:0] * b[31:0]

	*/
	FEA_FORCEINLINE static m64_u64_t mul_s(m64_u32_t a, m64_u32_t b) {
		return _mm_mul_su32(a.xmm, b.xmm);
	}
#endif

	/*
	Multiply the packed signed 16-bit integers in "a" and "b", producing
	intermediate 32-bit integers, and store the high 16 bits of the intermediate
	integers in "dst".

	pmulhw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		tmp[31:0] := SignExtend32(a[i+15:i]) * SignExtend32(b[i+15:i])
		dst[i+15:i] := tmp[31:16]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t mulhi(m128_i16_t a, m128_i16_t b) {
		return _mm_mulhi_epi16(a.xmm, b.xmm);
	}

	/*
	Multiply the packed unsigned 16-bit integers in "a" and "b", producing
	intermediate 32-bit integers, and store the high 16 bits of the intermediate
	integers in "dst".

	pmulhuw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		tmp[31:0] := a[i+15:i] * b[i+15:i]
		dst[i+15:i] := tmp[31:16]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u16_t mulhi(m128_u16_t a, m128_u16_t b) {
		return _mm_mulhi_epu16(a.xmm, b.xmm);
	}

	/*
	Multiply the packed 16-bit integers in "a" and "b", producing intermediate
	32-bit integers, and store the low 16 bits of the intermediate integers in
	"dst".

	pmullw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		tmp[31:0] := SignExtend32(a[i+15:i]) * SignExtend32(b[i+15:i])
		dst[i+15:i] := tmp[15:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t mullo(m128_i16_t a, m128_i16_t b) {
		return _mm_mullo_epi16(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of packed double-precision (64-bit) floating-point
	elements in "a" and "b", and store the results in "dst".

	orpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] OR b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t or_(m128_f64_t a, m128_f64_t b) {
		return _mm_or_pd(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t or_(m128_i8_t a, m128_i8_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t or_(m128_i16_t a, m128_i16_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t or_(m128_i32_t a, m128_i32_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t or_(m128_i64_t a, m128_i64_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u8_t or_(m128_u8_t a, m128_u8_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u16_t or_(m128_u16_t a, m128_u16_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u32_t or_(m128_u32_t a, m128_u32_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise OR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	por xmm, xmm

	dst[127:0] := (a[127:0] OR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u64_t or_(m128_u64_t a, m128_u64_t b) {
		return _mm_or_si128(a.xmm, b.xmm);
	}

	/*
	Convert packed signed 16-bit integers from "a" and "b" to packed 8-bit
	integers using signed saturation, and store the results in "dst".

	packsswb xmm, xmm

	dst[7:0] := Saturate8(a[15:0])
	dst[15:8] := Saturate8(a[31:16])
	dst[23:16] := Saturate8(a[47:32])
	dst[31:24] := Saturate8(a[63:48])
	dst[39:32] := Saturate8(a[79:64])
	dst[47:40] := Saturate8(a[95:80])
	dst[55:48] := Saturate8(a[111:96])
	dst[63:56] := Saturate8(a[127:112])
	dst[71:64] := Saturate8(b[15:0])
	dst[79:72] := Saturate8(b[31:16])
	dst[87:80] := Saturate8(b[47:32])
	dst[95:88] := Saturate8(b[63:48])
	dst[103:96] := Saturate8(b[79:64])
	dst[111:104] := Saturate8(b[95:80])
	dst[119:112] := Saturate8(b[111:96])
	dst[127:120] := Saturate8(b[127:112])

	*/
	FEA_FORCEINLINE static m128_i8_t packs(m128_i16_t a, m128_i16_t b) {
		return _mm_packs_epi16(a.xmm, b.xmm);
	}

	/*
	Convert packed signed 32-bit integers from "a" and "b" to packed 16-bit
	integers using signed saturation, and store the results in "dst".

	packssdw xmm, xmm

	dst[15:0] := Saturate16(a[31:0])
	dst[31:16] := Saturate16(a[63:32])
	dst[47:32] := Saturate16(a[95:64])
	dst[63:48] := Saturate16(a[127:96])
	dst[79:64] := Saturate16(b[31:0])
	dst[95:80] := Saturate16(b[63:32])
	dst[111:96] := Saturate16(b[95:64])
	dst[127:112] := Saturate16(b[127:96])

	*/
	FEA_FORCEINLINE static m128_i16_t packs(m128_i32_t a, m128_i32_t b) {
		return _mm_packs_epi32(a.xmm, b.xmm);
	}

	/*
	Convert packed signed 16-bit integers from "a" and "b" to packed 8-bit
	integers using unsigned saturation, and store the results in "dst".

	packuswb xmm, xmm

	dst[7:0] := SaturateU8(a[15:0])
	dst[15:8] := SaturateU8(a[31:16])
	dst[23:16] := SaturateU8(a[47:32])
	dst[31:24] := SaturateU8(a[63:48])
	dst[39:32] := SaturateU8(a[79:64])
	dst[47:40] := SaturateU8(a[95:80])
	dst[55:48] := SaturateU8(a[111:96])
	dst[63:56] := SaturateU8(a[127:112])
	dst[71:64] := SaturateU8(b[15:0])
	dst[79:72] := SaturateU8(b[31:16])
	dst[87:80] := SaturateU8(b[47:32])
	dst[95:88] := SaturateU8(b[63:48])
	dst[103:96] := SaturateU8(b[79:64])
	dst[111:104] := SaturateU8(b[95:80])
	dst[119:112] := SaturateU8(b[111:96])
	dst[127:120] := SaturateU8(b[127:112])

	*/
	FEA_FORCEINLINE static m128_i8_t packus(m128_i16_t a, m128_i16_t b) {
		return _mm_packus_epi16(a.xmm, b.xmm);
	}

	/*
	Provide a hint to the processor that the code sequence is a spin-wait loop.
	This can help improve the performance and power consumption of spin-wait
	loops.

	pause


	*/
	FEA_FORCEINLINE static void pause() {
		_mm_pause();
	}

	/*
	Compute the absolute differences of packed unsigned 8-bit integers in "a"
	and "b", then horizontally sum each consecutive 8 differences to produce two
	unsigned 16-bit integers, and pack these unsigned 16-bit integers in the low
	16 bits of 64-bit elements in "dst".

	psadbw xmm, xmm

	FOR j := 0 to 15
		i := j*8
		tmp[i+7:i] := ABS(a[i+7:i] - b[i+7:i])
	ENDFOR
	FOR j := 0 to 1
		i := j*64
		dst[i+15:i] := tmp[i+7:i] + tmp[i+15:i+8] + tmp[i+23:i+16] +
	tmp[i+31:i+24] + \\ tmp[i+39:i+32] + tmp[i+47:i+40] + tmp[i+55:i+48] +
	tmp[i+63:i+56] dst[i+63:i+16] := 0 ENDFOR

	*/
	FEA_FORCEINLINE static m128_u16_t sad(m128_u8_t a, m128_u8_t b) {
		return _mm_sad_epu8(a.xmm, b.xmm);
	}

	/*
	Broadcast 16-bit integer "a" to all all elements of "dst". This intrinsic
	may generate "vpbroadcastw".

	SEQUENCE

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := a[15:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set1(short a, m128_i16_t& dst) {
		dst.xmm = _mm_set1_epi16(a);
	}

	/*
	Broadcast 32-bit integer "a" to all elements of "dst". This intrinsic may
	generate "vpbroadcastd".

	SEQUENCE

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := a[31:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set1(int a, m128_i32_t& dst) {
		dst.xmm = _mm_set1_epi32(a);
	}

	/*
	Broadcast 64-bit integer "a" to all elements of "dst". This intrinsic may
	generate the "vpbroadcastq".

	SEQUENCE

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[63:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set1(__int64 a, m128_i64_t& dst) {
		dst.xmm = _mm_set1_epi64x(a);
	}

	/*
	Broadcast 8-bit integer "a" to all elements of "dst". This intrinsic may
	generate "vpbroadcastb".

	SEQUENCE

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := a[7:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set1(char a, m128_i8_t& dst) {
		dst.xmm = _mm_set1_epi8(a);
	}

	/*
	Broadcast double-precision (64-bit) floating-point value "a" to all elements
	of "dst".

	SEQUENCE

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[63:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set1(double a, m128_f64_t& dst) {
		dst.xmm = _mm_set1_pd(a);
	}

	/*
	Set packed 16-bit integers in "dst" with the supplied values.

	SEQUENCE

	dst[15:0] := e0
	dst[31:16] := e1
	dst[47:32] := e2
	dst[63:48] := e3
	dst[79:64] := e4
	dst[95:80] := e5
	dst[111:96] := e6
	dst[127:112] := e7

	*/
	FEA_FORCEINLINE static void set(short e7, short e6, short e5, short e4,
			short e3, short e2, short e1, short e0, m128_i16_t& dst) {
		dst.xmm = _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}

	/*
	Set packed 32-bit integers in "dst" with the supplied values.

	SEQUENCE

	dst[31:0] := e0
	dst[63:32] := e1
	dst[95:64] := e2
	dst[127:96] := e3

	*/
	FEA_FORCEINLINE static void set(
			int e3, int e2, int e1, int e0, m128_i32_t& dst) {
		dst.xmm = _mm_set_epi32(e3, e2, e1, e0);
	}

	/*
	Set packed 64-bit integers in "dst" with the supplied values.

	SEQUENCE

	dst[63:0] := e0
	dst[127:64] := e1

	*/
	FEA_FORCEINLINE static void set(__int64 e1, __int64 e0, m128_i64_t& dst) {
		dst.xmm = _mm_set_epi64x(e1, e0);
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
	dst[71:64] := e8
	dst[79:72] := e9
	dst[87:80] := e10
	dst[95:88] := e11
	dst[103:96] := e12
	dst[111:104] := e13
	dst[119:112] := e14
	dst[127:120] := e15

	*/
	FEA_FORCEINLINE static void set(char e15, char e14, char e13, char e12,
			char e11, char e10, char e9, char e8, char e7, char e6, char e5,
			char e4, char e3, char e2, char e1, char e0, m128_i8_t& dst) {
		dst.xmm = _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5,
				e4, e3, e2, e1, e0);
	}

	/*
	Set packed double-precision (64-bit) floating-point elements in "dst" with
	the supplied values.

	SEQUENCE

	dst[63:0] := e0
	dst[127:64] := e1

	*/
	FEA_FORCEINLINE static void set(double e1, double e0, m128_f64_t& dst) {
		dst.xmm = _mm_set_pd(e1, e0);
	}

	/*
	Broadcast double-precision (64-bit) floating-point value "a" to all elements
	of "dst".

	SEQUENCE

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[63:0]
	ENDFOR

	*/
	FEA_FORCEINLINE static void set_p1(double a, m128_f64_t& dst) {
		dst.xmm = _mm_set_pd1(a);
	}

	/*
	Copy double-precision (64-bit) floating-point element "a" to the lower
	element of "dst", and zero the upper element.

	SEQUENCE

	dst[63:0] := a[63:0]
	dst[127:64] := 0

	*/
	FEA_FORCEINLINE static void set_s(double a, m128_f64_t& dst) {
		dst.xmm = _mm_set_sd(a);
	}

	/*
	Set packed 16-bit integers in "dst" with the supplied values in reverse
	order.

	SEQUENCE

	dst[15:0] := e7
	dst[31:16] := e6
	dst[47:32] := e5
	dst[63:48] := e4
	dst[79:64] := e3
	dst[95:80] := e2
	dst[111:96] := e1
	dst[127:112] := e0

	*/
	FEA_FORCEINLINE static void setr(short e7, short e6, short e5, short e4,
			short e3, short e2, short e1, short e0, m128_i16_t& dst) {
		dst.xmm = _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}

	/*
	Set packed 32-bit integers in "dst" with the supplied values in reverse
	order.

	SEQUENCE

	dst[31:0] := e3
	dst[63:32] := e2
	dst[95:64] := e1
	dst[127:96] := e0

	*/
	FEA_FORCEINLINE static void setr(
			int e3, int e2, int e1, int e0, m128_i32_t& dst) {
		dst.xmm = _mm_setr_epi32(e3, e2, e1, e0);
	}

#if FEA_32BIT
	/*
	Set packed 64-bit integers in "dst" with the supplied values in reverse
	order.

	SEQUENCE

	dst[63:0] := e1
	dst[127:64] := e0

	*/
	FEA_FORCEINLINE static void setr(
			m64_i64_t e1, m64_i64_t e0, m128_i64_t& dst) {
		dst.xmm = _mm_setr_epi64(e1.xmm, e0.xmm);
	}
#endif

	/*
	Set packed 8-bit integers in "dst" with the supplied values in reverse
	order.

	SEQUENCE

	dst[7:0] := e15
	dst[15:8] := e14
	dst[23:16] := e13
	dst[31:24] := e12
	dst[39:32] := e11
	dst[47:40] := e10
	dst[55:48] := e9
	dst[63:56] := e8
	dst[71:64] := e7
	dst[79:72] := e6
	dst[87:80] := e5
	dst[95:88] := e4
	dst[103:96] := e3
	dst[111:104] := e2
	dst[119:112] := e1
	dst[127:120] := e0

	*/
	FEA_FORCEINLINE static void setr(char e15, char e14, char e13, char e12,
			char e11, char e10, char e9, char e8, char e7, char e6, char e5,
			char e4, char e3, char e2, char e1, char e0, m128_i8_t& dst) {
		dst.xmm = _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6,
				e5, e4, e3, e2, e1, e0);
	}

	/*
	Set packed double-precision (64-bit) floating-point elements in "dst" with
	the supplied values in reverse order.

	SEQUENCE

	dst[63:0] := e1
	dst[127:64] := e0

	*/
	FEA_FORCEINLINE static void setr(double e1, double e0, m128_f64_t& dst) {
		dst.xmm = _mm_setr_pd(e1, e0);
	}

	/*
	Return vector of type __m128d with all elements set to zero.

	xorpd xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_f64_t& dst) {
		dst.xmm = _mm_setzero_pd();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_i8_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_i16_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_i32_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_i64_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_u8_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_u16_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_u32_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Return vector of type __m128i with all elements set to zero.

	pxor xmm, xmm

	dst[MAX:0] := 0

	*/
	FEA_FORCEINLINE static void setzero(m128_u64_t& dst) {
		dst.xmm = _mm_setzero_si128();
	}

	/*
	Shuffle 32-bit integers in "a" using the control in "imm8", and store the
	results in "dst".

	pshufd xmm, xmm, imm8

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
	dst[95:64] := SELECT4(a[127:0], imm8[5:4])
	dst[127:96] := SELECT4(a[127:0], imm8[7:6])

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t shuffle(m128_i32_t a) {
		return _mm_shuffle_epi32(a.xmm, imm8);
	}

	/*
	Shuffle double-precision (64-bit) floating-point elements using the control
	in "imm8", and store the results in "dst".

	shufpd xmm, xmm, imm8

	dst[63:0] := (imm8[0] == 0) ? a[63:0] : a[127:64]
	dst[127:64] := (imm8[1] == 0) ? b[63:0] : b[127:64]

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_f64_t shuffle(m128_f64_t a, m128_f64_t b) {
		return _mm_shuffle_pd(a.xmm, b.xmm, imm8);
	}

	/*
	Shuffle 16-bit integers in the high 64 bits of "a" using the control in
	"imm8". Store the results in the high 64 bits of "dst", with the low 64 bits
	being copied from from "a" to "dst".

	pshufhw xmm, xmm, imm8

	dst[63:0] := a[63:0]
	dst[79:64] := (a >> (imm8[1:0] * 16))[79:64]
	dst[95:80] := (a >> (imm8[3:2] * 16))[79:64]
	dst[111:96] := (a >> (imm8[5:4] * 16))[79:64]
	dst[127:112] := (a >> (imm8[7:6] * 16))[79:64]

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t shufflehi(m128_i16_t a) {
		return _mm_shufflehi_epi16(a.xmm, imm8);
	}

	/*
	Shuffle 16-bit integers in the low 64 bits of "a" using the control in
	"imm8". Store the results in the low 64 bits of "dst", with the high 64 bits
	being copied from from "a" to "dst".

	pshuflw xmm, xmm, imm8

	dst[15:0] := (a >> (imm8[1:0] * 16))[15:0]
	dst[31:16] := (a >> (imm8[3:2] * 16))[15:0]
	dst[47:32] := (a >> (imm8[5:4] * 16))[15:0]
	dst[63:48] := (a >> (imm8[7:6] * 16))[15:0]
	dst[127:64] := a[127:64]

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t shufflelo(m128_i16_t a) {
		return _mm_shufflelo_epi16(a.xmm, imm8);
	}

	/*
	Shift packed 16-bit integers in "a" left by "count" while shifting in zeros,
	and store the results in "dst".

	psllw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		IF count[63:0] > 15
			dst[i+15:i] := 0
		ELSE
			dst[i+15:i] := ZeroExtend16(a[i+15:i] << count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t sll(m128_i16_t a, m128_i64_t count) {
		return _mm_sll_epi16(a.xmm, count.xmm);
	}

	/*
	Shift packed 32-bit integers in "a" left by "count" while shifting in zeros,
	and store the results in "dst".

	pslld xmm, xmm

	FOR j := 0 to 3
		i := j*32
		IF count[63:0] > 31
			dst[i+31:i] := 0
		ELSE
			dst[i+31:i] := ZeroExtend32(a[i+31:i] << count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t sll(m128_i32_t a, m128_i64_t count) {
		return _mm_sll_epi32(a.xmm, count.xmm);
	}

	/*
	Shift packed 64-bit integers in "a" left by "count" while shifting in zeros,
	and store the results in "dst".

	psllq xmm, xmm

	FOR j := 0 to 1
		i := j*64
		IF count[63:0] > 63
			dst[i+63:i] := 0
		ELSE
			dst[i+63:i] := ZeroExtend64(a[i+63:i] << count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i64_t sll(m128_i64_t a, m128_i64_t count) {
		return _mm_sll_epi64(a.xmm, count.xmm);
	}

	/*
	Shift packed 16-bit integers in "a" left by "imm8" while shifting in zeros,
	and store the results in "dst".

	psllw xmm, imm8

	FOR j := 0 to 7
		i := j*16
		IF imm8[7:0] > 15
			dst[i+15:i] := 0
		ELSE
			dst[i+15:i] := ZeroExtend16(a[i+15:i] << imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t slli(m128_i16_t a) {
		return _mm_slli_epi16(a.xmm, imm8);
	}

	/*
	Shift packed 32-bit integers in "a" left by "imm8" while shifting in zeros,
	and store the results in "dst".

	pslld xmm, imm8

	FOR j := 0 to 3
		i := j*32
		IF imm8[7:0] > 31
			dst[i+31:i] := 0
		ELSE
			dst[i+31:i] := ZeroExtend32(a[i+31:i] << imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t slli(m128_i32_t a) {
		return _mm_slli_epi32(a.xmm, imm8);
	}

	/*
	Shift packed 64-bit integers in "a" left by "imm8" while shifting in zeros,
	and store the results in "dst".

	psllq xmm, imm8

	FOR j := 0 to 1
		i := j*64
		IF imm8[7:0] > 63
			dst[i+63:i] := 0
		ELSE
			dst[i+63:i] := ZeroExtend64(a[i+63:i] << imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i64_t slli(m128_i64_t a) {
		return _mm_slli_epi64(a.xmm, imm8);
	}

	/*
	Compute the square root of packed double-precision (64-bit) floating-point
	elements in "a", and store the results in "dst".

	sqrtpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := SQRT(a[i+63:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t sqrt(m128_f64_t a) {
		return _mm_sqrt_pd(a.xmm);
	}

	/*
	Compute the square root of the lower double-precision (64-bit)
	floating-point element in "b", store the result in the lower element of
	"dst", and copy the upper element from "a" to the upper element of "dst".

	sqrtsd xmm, xmm

	dst[63:0] := SQRT(b[63:0])
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t sqrt_s(m128_f64_t a, m128_f64_t b) {
		return _mm_sqrt_sd(a.xmm, b.xmm);
	}

	/*
	Shift packed 16-bit integers in "a" right by "count" while shifting in sign
	bits, and store the results in "dst".

	psraw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		IF count[63:0] > 15
			dst[i+15:i] := (a[i+15] ? 0xFFFF : 0x0)
		ELSE
			dst[i+15:i] := SignExtend16(a[i+15:i] >> count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t sra(m128_i16_t a, m128_i64_t count) {
		return _mm_sra_epi16(a.xmm, count.xmm);
	}

	/*
	Shift packed 32-bit integers in "a" right by "count" while shifting in sign
	bits, and store the results in "dst".

	psrad xmm, xmm

	FOR j := 0 to 3
		i := j*32
		IF count[63:0] > 31
			dst[i+31:i] := (a[i+31] ? 0xFFFFFFFF : 0x0)
		ELSE
			dst[i+31:i] := SignExtend32(a[i+31:i] >> count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t sra(m128_i32_t a, m128_i64_t count) {
		return _mm_sra_epi32(a.xmm, count.xmm);
	}

	/*
	Shift packed 16-bit integers in "a" right by "imm8" while shifting in sign
	bits, and store the results in "dst".

	psraw xmm, imm8

	FOR j := 0 to 7
		i := j*16
		IF imm8[7:0] > 15
			dst[i+15:i] := (a[i+15] ? 0xFFFF : 0x0)
		ELSE
			dst[i+15:i] := SignExtend16(a[i+15:i] >> imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t srai(m128_i16_t a) {
		return _mm_srai_epi16(a.xmm, imm8);
	}

	/*
	Shift packed 32-bit integers in "a" right by "imm8" while shifting in sign
	bits, and store the results in "dst".

	psrad xmm, imm8

	FOR j := 0 to 3
		i := j*32
		IF imm8[7:0] > 31
			dst[i+31:i] := (a[i+31] ? 0xFFFFFFFF : 0x0)
		ELSE
			dst[i+31:i] := SignExtend32(a[i+31:i] >> imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t srai(m128_i32_t a) {
		return _mm_srai_epi32(a.xmm, imm8);
	}

	/*
	Shift packed 16-bit integers in "a" right by "count" while shifting in
	zeros, and store the results in "dst".

	psrlw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		IF count[63:0] > 15
			dst[i+15:i] := 0
		ELSE
			dst[i+15:i] := ZeroExtend16(a[i+15:i] >> count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t srl(m128_i16_t a, m128_i64_t count) {
		return _mm_srl_epi16(a.xmm, count.xmm);
	}

	/*
	Shift packed 32-bit integers in "a" right by "count" while shifting in
	zeros, and store the results in "dst".

	psrld xmm, xmm

	FOR j := 0 to 3
		i := j*32
		IF count[63:0] > 31
			dst[i+31:i] := 0
		ELSE
			dst[i+31:i] := ZeroExtend32(a[i+31:i] >> count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t srl(m128_i32_t a, m128_i64_t count) {
		return _mm_srl_epi32(a.xmm, count.xmm);
	}

	/*
	Shift packed 64-bit integers in "a" right by "count" while shifting in
	zeros, and store the results in "dst".

	psrlq xmm, xmm

	FOR j := 0 to 1
		i := j*64
		IF count[63:0] > 63
			dst[i+63:i] := 0
		ELSE
			dst[i+63:i] := ZeroExtend64(a[i+63:i] >> count[63:0])
		FI
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i64_t srl(m128_i64_t a, m128_i64_t count) {
		return _mm_srl_epi64(a.xmm, count.xmm);
	}

	/*
	Shift packed 16-bit integers in "a" right by "imm8" while shifting in zeros,
	and store the results in "dst".

	psrlw xmm, imm8

	FOR j := 0 to 7
		i := j*16
		IF imm8[7:0] > 15
			dst[i+15:i] := 0
		ELSE
			dst[i+15:i] := ZeroExtend16(a[i+15:i] >> imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i16_t srli(m128_i16_t a) {
		return _mm_srli_epi16(a.xmm, imm8);
	}

	/*
	Shift packed 32-bit integers in "a" right by "imm8" while shifting in zeros,
	and store the results in "dst".

	psrld xmm, imm8

	FOR j := 0 to 3
		i := j*32
		IF imm8[7:0] > 31
			dst[i+31:i] := 0
		ELSE
			dst[i+31:i] := ZeroExtend32(a[i+31:i] >> imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i32_t srli(m128_i32_t a) {
		return _mm_srli_epi32(a.xmm, imm8);
	}

	/*
	Shift packed 64-bit integers in "a" right by "imm8" while shifting in zeros,
	and store the results in "dst".

	psrlq xmm, imm8

	FOR j := 0 to 1
		i := j*64
		IF imm8[7:0] > 63
			dst[i+63:i] := 0
		ELSE
			dst[i+63:i] := ZeroExtend64(a[i+63:i] >> imm8[7:0])
		FI
	ENDFOR

	*/
	template <int imm8>
	FEA_FORCEINLINE static m128_i64_t srli(m128_i64_t a) {
		return _mm_srli_epi64(a.xmm, imm8);
	}

	/*
	Store the lower double-precision (64-bit) floating-point element from "a"
	into 2 contiguous elements in memory. "mem_addr" must be aligned on a
	16-byte boundary or a general-protection exception may be generated.

	SEQUENCE

	MEM[mem_addr+63:mem_addr] := a[63:0]
	MEM[mem_addr+127:mem_addr+64] := a[63:0]

	*/
	FEA_FORCEINLINE static void store1(m128_f64_t a, double* mem_addr) {
		_mm_store1_pd(mem_addr, a.xmm);
	}

	/*
	Store 128-bits (composed of 2 packed double-precision (64-bit)
	floating-point elements) from "a" into memory. "mem_addr" must be aligned on
	a 16-byte boundary or a general-protection exception may be generated.

	movapd m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_f64_t a, double* mem_addr) {
		_mm_store_pd(mem_addr, a.xmm);
	}

	/*
	Store the lower double-precision (64-bit) floating-point element from "a"
	into 2 contiguous elements in memory. "mem_addr" must be aligned on a
	16-byte boundary or a general-protection exception may be generated.

	SEQUENCE

	MEM[mem_addr+63:mem_addr] := a[63:0]
	MEM[mem_addr+127:mem_addr+64] := a[63:0]

	*/
	FEA_FORCEINLINE static void store_p1(m128_f64_t a, double* mem_addr) {
		_mm_store_pd1(mem_addr, a.xmm);
	}

	/*
	Store the lower double-precision (64-bit) floating-point element from "a"
	into memory. "mem_addr" does not need to be aligned on any particular
	boundary.

	movsd m64, xmm

	MEM[mem_addr+63:mem_addr] := a[63:0]

	*/
	FEA_FORCEINLINE static void store_s(m128_f64_t a, double& mem_addr) {
		_mm_store_sd(&mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_i8_t a, char* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_i16_t a, short* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_i32_t a, int* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_i64_t a, long long* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_u8_t a, unsigned char* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_u16_t a, unsigned short* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(m128_u32_t a, unsigned int* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory.  "mem_addr" must be
	aligned on a 16-byte boundary or a general-protection exception may be
	generated.

	movdqa m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void store(
			m128_u64_t a, unsigned long long* mem_addr) {
		_mm_store_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store the upper double-precision (64-bit) floating-point element from "a"
	into memory.

	movhpd m64, xmm

	MEM[mem_addr+63:mem_addr] := a[127:64]

	*/
	FEA_FORCEINLINE static void storeh(m128_f64_t a, double* mem_addr) {
		_mm_storeh_pd(mem_addr, a.xmm);
	}

	/*
	Store 64-bit integer from the first element of "a" into memory.

	movq m64, xmm

	MEM[mem_addr+63:mem_addr] := a[63:0]

	*/
	FEA_FORCEINLINE static void storel(m128_i64_t a, long long* mem_addr) {
		_mm_storel_epi64((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store the lower double-precision (64-bit) floating-point element from "a"
	into memory.

	movlpd m64, xmm

	MEM[mem_addr+63:mem_addr] := a[63:0]

	*/
	FEA_FORCEINLINE static void storel(m128_f64_t a, double* mem_addr) {
		_mm_storel_pd(mem_addr, a.xmm);
	}

	/*
	Store 2 double-precision (64-bit) floating-point elements from "a" into
	memory in reverse order. "mem_addr" must be aligned on a 16-byte boundary or
	a general-protection exception may be generated.

	SEQUENCE

	MEM[mem_addr+63:mem_addr] := a[127:64]
	MEM[mem_addr+127:mem_addr+64] := a[63:0]

	*/
	FEA_FORCEINLINE static void storer(m128_f64_t a, double* mem_addr) {
		_mm_storer_pd(mem_addr, a.xmm);
	}

	/*
	Store 128-bits (composed of 2 packed double-precision (64-bit)
	floating-point elements) from "a" into memory. "mem_addr" does not need to
	be aligned on any particular boundary.

	movupd m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_f64_t a, double* mem_addr) {
		_mm_storeu_pd(mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_i8_t a, char* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_i16_t a, short* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_i32_t a, int* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_i64_t a, long long* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_u8_t a, unsigned char* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_u16_t a, unsigned short* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(m128_u32_t a, unsigned int* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory. "mem_addr" does not
	need to be aligned on any particular boundary.

	movdqu m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void storeu(
			m128_u64_t a, unsigned long long* mem_addr) {
		_mm_storeu_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 16-bit integer from the first element of "a" into memory. "mem_addr"
	does not need to be aligned on any particular boundary.

	SEQUENCE

	MEM[mem_addr+15:mem_addr] := a[15:0]

	*/
	FEA_FORCEINLINE static void storeu_s(m128_i16_t a, short& mem_addr) {
		_mm_storeu_si16(&mem_addr, a.xmm);
	}

	/*
	Store 32-bit integer from the first element of "a" into memory. "mem_addr"
	does not need to be aligned on any particular boundary.

	movd m32, xmm

	MEM[mem_addr+31:mem_addr] := a[31:0]

	*/
	FEA_FORCEINLINE static void storeu_s(m128_i32_t a, int& mem_addr) {
		_mm_storeu_si32(&mem_addr, a.xmm);
	}

	/*
	Store 64-bit integer from the first element of "a" into memory. "mem_addr"
	does not need to be aligned on any particular boundary.

	movq m64, xmm

	MEM[mem_addr+63:mem_addr] := a[63:0]

	*/
	FEA_FORCEINLINE static void storeu_s(m128_i64_t a, long long& mem_addr) {
		_mm_storeu_si64(&mem_addr, a.xmm);
	}

	/*
	Store 128-bits (composed of 2 packed double-precision (64-bit)
	floating-point elements) from "a" into memory using a non-temporal memory
	hint. "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntpd m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_f64_t a, double* mem_addr) {
		_mm_stream_pd(mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_i8_t a, char* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_i16_t a, short* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_i32_t a, int* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_i64_t a, long long* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_u8_t a, unsigned char* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_u16_t a, unsigned short* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(m128_u32_t a, unsigned int* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 128-bits of integer data from "a" into memory using a non-temporal
	memory hint.  "mem_addr" must be aligned on a 16-byte boundary or a
	general-protection exception may be generated.

	movntdq m128, xmm

	MEM[mem_addr+127:mem_addr] := a[127:0]

	*/
	FEA_FORCEINLINE static void stream(
			m128_u64_t a, unsigned long long* mem_addr) {
		_mm_stream_si128((__m128i*)mem_addr, a.xmm);
	}

	/*
	Store 32-bit integer "a" into memory using a non-temporal hint to minimize
	cache pollution. If the cache line containing address "mem_addr" is already
	in the cache, the cache will be updated.

	movnti m32, r32

	MEM[mem_addr+31:mem_addr] := a[31:0]

	*/
	FEA_FORCEINLINE static void stream_s(int a, int& mem_addr) {
		_mm_stream_si32(&mem_addr, a);
	}

	/*
	Store 64-bit integer "a" into memory using a non-temporal hint to minimize
	cache pollution. If the cache line containing address "mem_addr" is already
	in the cache, the cache will be updated.

	movnti m64, r64

	MEM[mem_addr+63:mem_addr] := a[63:0]

	*/
	FEA_FORCEINLINE static void stream_s(__int64 a, __int64& mem_addr) {
		_mm_stream_si64(&mem_addr, a);
	}

	/*
	Subtract packed 16-bit integers in "b" from packed 16-bit integers in "a",
	and store the results in "dst".

	psubw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := a[i+15:i] - b[i+15:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t sub(m128_i16_t a, m128_i16_t b) {
		return _mm_sub_epi16(a.xmm, b.xmm);
	}

	/*
	Subtract packed 32-bit integers in "b" from packed 32-bit integers in "a",
	and store the results in "dst".

	psubd xmm, xmm

	FOR j := 0 to 3
		i := j*32
		dst[i+31:i] := a[i+31:i] - b[i+31:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i32_t sub(m128_i32_t a, m128_i32_t b) {
		return _mm_sub_epi32(a.xmm, b.xmm);
	}

	/*
	Subtract packed 64-bit integers in "b" from packed 64-bit integers in "a",
	and store the results in "dst".

	psubq xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] - b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i64_t sub(m128_i64_t a, m128_i64_t b) {
		return _mm_sub_epi64(a.xmm, b.xmm);
	}

	/*
	Subtract packed 8-bit integers in "b" from packed 8-bit integers in "a", and
	store the results in "dst".

	psubb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := a[i+7:i] - b[i+7:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t sub(m128_i8_t a, m128_i8_t b) {
		return _mm_sub_epi8(a.xmm, b.xmm);
	}

	/*
	Subtract packed double-precision (64-bit) floating-point elements in "b"
	from packed double-precision (64-bit) floating-point elements in "a", and
	store the results in "dst".

	subpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] - b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t sub(m128_f64_t a, m128_f64_t b) {
		return _mm_sub_pd(a.xmm, b.xmm);
	}

	/*
	Subtract the lower double-precision (64-bit) floating-point element in "b"
	from the lower double-precision (64-bit) floating-point element in "a",
	store the result in the lower element of "dst", and copy the upper element
	from "a" to the upper element of "dst".

	subsd xmm, xmm

	dst[63:0] := a[63:0] - b[63:0]
	dst[127:64] := a[127:64]

	*/
	FEA_FORCEINLINE static m128_f64_t sub_s(m128_f64_t a, m128_f64_t b) {
		return _mm_sub_sd(a.xmm, b.xmm);
	}

#if FEA_32BIT
	/*
	Subtract 64-bit integer "b" from 64-bit integer "a", and store the result in
	"dst".

	psubq mm, mm

	dst[63:0] := a[63:0] - b[63:0]

	*/
	FEA_FORCEINLINE static m64_i64_t sub(m64_i64_t a, m64_i64_t b) {
		return _mm_sub_si64(a.xmm, b.xmm);
	}
#endif

	/*
	Subtract packed signed 16-bit integers in "b" from packed 16-bit integers in
	"a" using saturation, and store the results in "dst".

	psubsw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := Saturate16(a[i+15:i] - b[i+15:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i16_t subs(m128_i16_t a, m128_i16_t b) {
		return _mm_subs_epi16(a.xmm, b.xmm);
	}

	/*
	Subtract packed signed 8-bit integers in "b" from packed 8-bit integers in
	"a" using saturation, and store the results in "dst".

	psubsb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := Saturate8(a[i+7:i] - b[i+7:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_i8_t subs(m128_i8_t a, m128_i8_t b) {
		return _mm_subs_epi8(a.xmm, b.xmm);
	}

	/*
	Subtract packed unsigned 16-bit integers in "b" from packed unsigned 16-bit
	integers in "a" using saturation, and store the results in "dst".

	psubusw xmm, xmm

	FOR j := 0 to 7
		i := j*16
		dst[i+15:i] := SaturateU16(a[i+15:i] - b[i+15:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u16_t subs(m128_u16_t a, m128_u16_t b) {
		return _mm_subs_epu16(a.xmm, b.xmm);
	}

	/*
	Subtract packed unsigned 8-bit integers in "b" from packed unsigned 8-bit
	integers in "a" using saturation, and store the results in "dst".

	psubusb xmm, xmm

	FOR j := 0 to 15
		i := j*8
		dst[i+7:i] := SaturateU8(a[i+7:i] - b[i+7:i])
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_u8_t subs(m128_u8_t a, m128_u8_t b) {
		return _mm_subs_epu8(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for equality, and return the boolean result (0 or 1). This
	instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] == b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomieq(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomieq_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for greater-than-or-equal, and return the boolean result (0 or 1).
	This instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] >= b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomige(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomige_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for greater-than, and return the boolean result (0 or 1). This
	instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] > b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomigt(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomigt_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for less-than-or-equal, and return the boolean result (0 or 1). This
	instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] <= b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomile(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomile_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for less-than, and return the boolean result (0 or 1). This
	instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] != NaN AND b[63:0] != NaN AND a[63:0] < b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomilt(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomilt_sd(a.xmm, b.xmm);
	}

	/*
	Compare the lower double-precision (64-bit) floating-point element in "a"
	and "b" for not-equal, and return the boolean result (0 or 1). This
	instruction will not signal an exception for QNaNs.

	ucomisd xmm, xmm

	RETURN ( a[63:0] == NaN OR b[63:0] == NaN OR a[63:0] != b[63:0] ) ? 1 : 0

	*/
	FEA_FORCEINLINE static bool ucomineq(m128_f64_t a, m128_f64_t b) {
		return _mm_ucomineq_sd(a.xmm, b.xmm);
	}

	/*
	Return vector of type __m128d with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_f64_t& dst) {
		dst.xmm = _mm_undefined_pd();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_i8_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_i16_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_i32_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_i64_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_u8_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_u16_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_u32_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Return vector of type __m128i with undefined elements.



	*/
	FEA_FORCEINLINE static void undefined(m128_u64_t& dst) {
		dst.xmm = _mm_undefined_si128();
	}

	/*
	Unpack and interleave 16-bit integers from the high half of "a" and "b", and
	store the results in "dst".

	punpckhwd xmm, xmm

	DEFINE INTERLEAVE_HIGH_WORDS(src1[127:0], src2[127:0]) {
		dst[15:0] := src1[79:64]
		dst[31:16] := src2[79:64]
		dst[47:32] := src1[95:80]
		dst[63:48] := src2[95:80]
		dst[79:64] := src1[111:96]
		dst[95:80] := src2[111:96]
		dst[111:96] := src1[127:112]
		dst[127:112] := src2[127:112]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_HIGH_WORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t unpackhi(m128_i16_t a, m128_i16_t b) {
		return _mm_unpackhi_epi16(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 32-bit integers from the high half of "a" and "b", and
	store the results in "dst".

	punpckhdq xmm, xmm

	DEFINE INTERLEAVE_HIGH_DWORDS(src1[127:0], src2[127:0]) {
		dst[31:0] := src1[95:64]
		dst[63:32] := src2[95:64]
		dst[95:64] := src1[127:96]
		dst[127:96] := src2[127:96]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_HIGH_DWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t unpackhi(m128_i32_t a, m128_i32_t b) {
		return _mm_unpackhi_epi32(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 64-bit integers from the high half of "a" and "b", and
	store the results in "dst".

	punpckhqdq xmm, xmm

	DEFINE INTERLEAVE_HIGH_QWORDS(src1[127:0], src2[127:0]) {
		dst[63:0] := src1[127:64]
		dst[127:64] := src2[127:64]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_HIGH_QWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t unpackhi(m128_i64_t a, m128_i64_t b) {
		return _mm_unpackhi_epi64(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 8-bit integers from the high half of "a" and "b", and
	store the results in "dst".

	punpckhbw xmm, xmm

	DEFINE INTERLEAVE_HIGH_BYTES(src1[127:0], src2[127:0]) {
		dst[7:0] := src1[71:64]
		dst[15:8] := src2[71:64]
		dst[23:16] := src1[79:72]
		dst[31:24] := src2[79:72]
		dst[39:32] := src1[87:80]
		dst[47:40] := src2[87:80]
		dst[55:48] := src1[95:88]
		dst[63:56] := src2[95:88]
		dst[71:64] := src1[103:96]
		dst[79:72] := src2[103:96]
		dst[87:80] := src1[111:104]
		dst[95:88] := src2[111:104]
		dst[103:96] := src1[119:112]
		dst[111:104] := src2[119:112]
		dst[119:112] := src1[127:120]
		dst[127:120] := src2[127:120]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_HIGH_BYTES(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t unpackhi(m128_i8_t a, m128_i8_t b) {
		return _mm_unpackhi_epi8(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave double-precision (64-bit) floating-point elements from
	the high half of "a" and "b", and store the results in "dst".

	unpckhpd xmm, xmm

	DEFINE INTERLEAVE_HIGH_QWORDS(src1[127:0], src2[127:0]) {
		dst[63:0] := src1[127:64]
		dst[127:64] := src2[127:64]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_HIGH_QWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_f64_t unpackhi(m128_f64_t a, m128_f64_t b) {
		return _mm_unpackhi_pd(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 16-bit integers from the low half of "a" and "b", and
	store the results in "dst".

	punpcklwd xmm, xmm

	DEFINE INTERLEAVE_WORDS(src1[127:0], src2[127:0]) {
		dst[15:0] := src1[15:0]
		dst[31:16] := src2[15:0]
		dst[47:32] := src1[31:16]
		dst[63:48] := src2[31:16]
		dst[79:64] := src1[47:32]
		dst[95:80] := src2[47:32]
		dst[111:96] := src1[63:48]
		dst[127:112] := src2[63:48]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_WORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t unpacklo(m128_i16_t a, m128_i16_t b) {
		return _mm_unpacklo_epi16(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 32-bit integers from the low half of "a" and "b", and
	store the results in "dst".

	punpckldq xmm, xmm

	DEFINE INTERLEAVE_DWORDS(src1[127:0], src2[127:0]) {
		dst[31:0] := src1[31:0]
		dst[63:32] := src2[31:0]
		dst[95:64] := src1[63:32]
		dst[127:96] := src2[63:32]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_DWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t unpacklo(m128_i32_t a, m128_i32_t b) {
		return _mm_unpacklo_epi32(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 64-bit integers from the low half of "a" and "b", and
	store the results in "dst".

	punpcklqdq xmm, xmm

	DEFINE INTERLEAVE_QWORDS(src1[127:0], src2[127:0]) {
		dst[63:0] := src1[63:0]
		dst[127:64] := src2[63:0]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_QWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t unpacklo(m128_i64_t a, m128_i64_t b) {
		return _mm_unpacklo_epi64(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave 8-bit integers from the low half of "a" and "b", and
	store the results in "dst".

	punpcklbw xmm, xmm

	DEFINE INTERLEAVE_BYTES(src1[127:0], src2[127:0]) {
		dst[7:0] := src1[7:0]
		dst[15:8] := src2[7:0]
		dst[23:16] := src1[15:8]
		dst[31:24] := src2[15:8]
		dst[39:32] := src1[23:16]
		dst[47:40] := src2[23:16]
		dst[55:48] := src1[31:24]
		dst[63:56] := src2[31:24]
		dst[71:64] := src1[39:32]
		dst[79:72] := src2[39:32]
		dst[87:80] := src1[47:40]
		dst[95:88] := src2[47:40]
		dst[103:96] := src1[55:48]
		dst[111:104] := src2[55:48]
		dst[119:112] := src1[63:56]
		dst[127:120] := src2[63:56]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_BYTES(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t unpacklo(m128_i8_t a, m128_i8_t b) {
		return _mm_unpacklo_epi8(a.xmm, b.xmm);
	}

	/*
	Unpack and interleave double-precision (64-bit) floating-point elements from
	the low half of "a" and "b", and store the results in "dst".

	unpcklpd xmm, xmm

	DEFINE INTERLEAVE_QWORDS(src1[127:0], src2[127:0]) {
		dst[63:0] := src1[63:0]
		dst[127:64] := src2[63:0]
		RETURN dst[127:0]
	}
	dst[127:0] := INTERLEAVE_QWORDS(a[127:0], b[127:0])

	*/
	FEA_FORCEINLINE static m128_f64_t unpacklo(m128_f64_t a, m128_f64_t b) {
		return _mm_unpacklo_pd(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of packed double-precision (64-bit) floating-point
	elements in "a" and "b", and store the results in "dst".

	xorpd xmm, xmm

	FOR j := 0 to 1
		i := j*64
		dst[i+63:i] := a[i+63:i] XOR b[i+63:i]
	ENDFOR

	*/
	FEA_FORCEINLINE static m128_f64_t xor_(m128_f64_t a, m128_f64_t b) {
		return _mm_xor_pd(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i8_t xor_(m128_i8_t a, m128_i8_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i16_t xor_(m128_i16_t a, m128_i16_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i32_t xor_(m128_i32_t a, m128_i32_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_i64_t xor_(m128_i64_t a, m128_i64_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u8_t xor_(m128_u8_t a, m128_u8_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u16_t xor_(m128_u16_t a, m128_u16_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u32_t xor_(m128_u32_t a, m128_u32_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}

	/*
	Compute the bitwise XOR of 128 bits (representing integer data) in "a" and
	"b", and store the result in "dst".

	pxor xmm, xmm

	dst[127:0] := (a[127:0] XOR b[127:0])

	*/
	FEA_FORCEINLINE static m128_u64_t xor_(m128_u64_t a, m128_u64_t b) {
		return _mm_xor_si128(a.xmm, b.xmm);
	}
};

using simd_sse2_api = simd_api<fea::simd_ver::sse2>;

} // namespace fea
