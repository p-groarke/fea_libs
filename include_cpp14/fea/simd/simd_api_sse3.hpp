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
#include "simd_api_sse2.hpp"
#include "fea/simd/simd_register.hpp"
#include "fea/simd/simd_register_traits.hpp"
#include "fea/simd/simd_version.hpp"
#include "fea/utils/platform.hpp"

#include <immintrin.h>

/*
SIMD intrinsics, sorted and grouped by abi support.

All APIs and information come from :
https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
*/

namespace fea {


template <>
struct simd_api<fea::simd_ver::sse3> 
		: public simd_api<fea::simd_ver::sse2>
{
// Default register types for this api.
// Previous registers and functionality are still available.
using xmm_f32_t = simd_abi_register_t<fea::simd_ver::sse3, float>;
using xmm_f64_t = simd_abi_register_t<fea::simd_ver::sse3, double>;
using xmm_i8_t = simd_abi_register_t<fea::simd_ver::sse3, char>;
using xmm_u8_t = simd_abi_register_t<fea::simd_ver::sse3, unsigned char>;
using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::sse3, signed char>;
using xmm_i16_t = simd_abi_register_t<fea::simd_ver::sse3, short>;
using xmm_u16_t = simd_abi_register_t<fea::simd_ver::sse3, unsigned short>;
using xmm_i32_t = simd_abi_register_t<fea::simd_ver::sse3, int>;
using xmm_u32_t = simd_abi_register_t<fea::simd_ver::sse3, unsigned int>;
using xmm_i64_t = simd_abi_register_t<fea::simd_ver::sse3, long long>;
using xmm_u64_t = simd_abi_register_t<fea::simd_ver::sse3, unsigned long long>;



/*
Alternatively add and subtract packed double-precision (64-bit) floating-point
elements in "a" to/from packed elements in "b", and store the results in
"dst".

addsubpd xmm, xmm

FOR j := 0 to 1
	i := j*64
	IF ((j & 1) == 0)
		dst[i+63:i] := a[i+63:i] - b[i+63:i]
	ELSE
		dst[i+63:i] := a[i+63:i] + b[i+63:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static m128_f64_t addsub(m128_f64_t a, m128_f64_t b) {
	return _mm_addsub_pd(a.xmm, b.xmm);
}

/*
Alternatively add and subtract packed single-precision (32-bit) floating-point
elements in "a" to/from packed elements in "b", and store the results in
"dst".

addsubps xmm, xmm

FOR j := 0 to 3
	i := j*32
	IF ((j & 1) == 0)
		dst[i+31:i] := a[i+31:i] - b[i+31:i]
	ELSE
		dst[i+31:i] := a[i+31:i] + b[i+31:i]
	FI
ENDFOR

*/
FEA_FORCEINLINE static m128_f32_t addsub(m128_f32_t a, m128_f32_t b) {
	return _mm_addsub_ps(a.xmm, b.xmm);
}

/*
Horizontally add adjacent pairs of double-precision (64-bit) floating-point
elements in "a" and "b", and pack the results in "dst".

haddpd xmm, xmm

dst[63:0] := a[127:64] + a[63:0]
dst[127:64] := b[127:64] + b[63:0]

*/
FEA_FORCEINLINE static m128_f64_t hadd(m128_f64_t a, m128_f64_t b) {
	return _mm_hadd_pd(a.xmm, b.xmm);
}

/*
Horizontally add adjacent pairs of single-precision (32-bit) floating-point
elements in "a" and "b", and pack the results in "dst".

haddps xmm, xmm

dst[31:0] := a[63:32] + a[31:0]
dst[63:32] := a[127:96] + a[95:64]
dst[95:64] := b[63:32] + b[31:0]
dst[127:96] := b[127:96] + b[95:64]

*/
FEA_FORCEINLINE static m128_f32_t hadd(m128_f32_t a, m128_f32_t b) {
	return _mm_hadd_ps(a.xmm, b.xmm);
}

/*
Horizontally subtract adjacent pairs of double-precision (64-bit)
floating-point elements in "a" and "b", and pack the results in "dst".

hsubpd xmm, xmm

dst[63:0] := a[63:0] - a[127:64]
dst[127:64] := b[63:0] - b[127:64]

*/
FEA_FORCEINLINE static m128_f64_t hsub(m128_f64_t a, m128_f64_t b) {
	return _mm_hsub_pd(a.xmm, b.xmm);
}

/*
Horizontally add adjacent pairs of single-precision (32-bit) floating-point
elements in "a" and "b", and pack the results in "dst".

hsubps xmm, xmm

dst[31:0] := a[31:0] - a[63:32]
dst[63:32] := a[95:64] - a[127:96]
dst[95:64] := b[31:0] - b[63:32]
dst[127:96] := b[95:64] - b[127:96]

*/
FEA_FORCEINLINE static m128_f32_t hsub(m128_f32_t a, m128_f32_t b) {
	return _mm_hsub_ps(a.xmm, b.xmm);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(char const* mem_addr, m128_i8_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(int const* mem_addr, m128_i32_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(long long const* mem_addr, m128_i64_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(short const* mem_addr, m128_i16_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(unsigned char const* mem_addr, m128_u8_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(unsigned int const* mem_addr, m128_u32_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(unsigned long long const* mem_addr, m128_u64_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load 128-bits of integer data from unaligned memory into "dst". This intrinsic
may perform better than "_mm_loadu_si128" when the data crosses a cache line
boundary.

lddqu xmm, m128

dst[127:0] := MEM[mem_addr+127:mem_addr]

*/
FEA_FORCEINLINE static void lddqu(unsigned short const* mem_addr, m128_u16_t& dst) {
	dst.xmm = _mm_lddqu_si128((__m128i const*)mem_addr);
}

/*
Load a double-precision (64-bit) floating-point element from memory into both
elements of "dst".

movddup xmm, m64

dst[63:0] := MEM[mem_addr+63:mem_addr]
dst[127:64] := MEM[mem_addr+63:mem_addr]

*/
FEA_FORCEINLINE static void loaddup(double const* mem_addr, m128_f64_t& dst) {
	dst.xmm = _mm_loaddup_pd(mem_addr);
}

/*
Duplicate the low double-precision (64-bit) floating-point element from "a",
and store the results in "dst".

movddup xmm, xmm

dst[63:0] := a[63:0]
dst[127:64] := a[63:0]

*/
FEA_FORCEINLINE static m128_f64_t movedup(m128_f64_t a) {
	return _mm_movedup_pd(a.xmm);
}

/*
Duplicate odd-indexed single-precision (32-bit) floating-point elements from
"a", and store the results in "dst".

movshdup xmm, xmm

dst[31:0] := a[63:32] 
dst[63:32] := a[63:32]
dst[95:64] := a[127:96] 
dst[127:96] := a[127:96]

*/
FEA_FORCEINLINE static m128_f32_t movehdup(m128_f32_t a) {
	return _mm_movehdup_ps(a.xmm);
}

/*
Duplicate even-indexed single-precision (32-bit) floating-point elements from
"a", and store the results in "dst".

movsldup xmm, xmm

dst[31:0] := a[31:0] 
dst[63:32] := a[31:0]
dst[95:64] := a[95:64] 
dst[127:96] := a[95:64]

*/
FEA_FORCEINLINE static m128_f32_t moveldup(m128_f32_t a) {
	return _mm_moveldup_ps(a.xmm);
}



};

using simd_sse3_api = simd_api<fea::simd_ver::sse3>;

} // namespace fea
