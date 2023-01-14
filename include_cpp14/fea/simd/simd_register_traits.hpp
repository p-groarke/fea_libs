/*
BSD 3-Clause License

Copyright (c) 2022, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/simd/simd_register.hpp"
#include "fea/simd/simd_version.hpp"

namespace fea {
namespace detail {
template <fea::simd_ver ver, class T>
struct get_abi_xmm : get_abi_xmm<ver - 1, T> {};

// mmx
template <>
struct get_abi_xmm<fea::simd_ver::mmx, float> {
	using type = m64_f32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, double> {
	using type = m64_f64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, char> {
	using type = m64_i8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, unsigned char> {
	using type = m64_u8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, signed char> {
	using type = m64_ii8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, short> {
	using type = m64_i16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, unsigned short> {
	using type = m64_u16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, int> {
	using type = m64_i32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, unsigned int> {
	using type = m64_u32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, long long> {
	using type = m64_i64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::mmx, unsigned long long> {
	using type = m64_u64_t;
};


// sse & sse2
template <>
struct get_abi_xmm<fea::simd_ver::sse, float> {
	using type = m128_f32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, double> {
	using type = m128_f64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, char> {
	using type = m128_i8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, unsigned char> {
	using type = m128_u8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, signed char> {
	using type = m128_ii8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, short> {
	using type = m128_i16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, unsigned short> {
	using type = m128_u16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, int> {
	using type = m128_i32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, unsigned int> {
	using type = m128_u32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, long long> {
	using type = m128_i64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::sse2, unsigned long long> {
	using type = m128_u64_t;
};

// avx
template <>
struct get_abi_xmm<fea::simd_ver::avx, float> {
	using type = m256_f32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, double> {
	using type = m256_f64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, char> {
	using type = m256_i8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, unsigned char> {
	using type = m256_u8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, signed char> {
	using type = m256_ii8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, short> {
	using type = m256_i16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, unsigned short> {
	using type = m256_u16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, int> {
	using type = m256_i32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, unsigned int> {
	using type = m256_u32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, long long> {
	using type = m256_i64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx, unsigned long long> {
	using type = m256_u64_t;
};

// avx512f
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, float> {
	using type = m512_f32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, double> {
	using type = m512_f64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, char> {
	using type = m512_i8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, unsigned char> {
	using type = m512_u8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, signed char> {
	using type = m512_ii8_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, short> {
	using type = m512_i16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, unsigned short> {
	using type = m512_u16_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, int> {
	using type = m512_i32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, unsigned int> {
	using type = m512_u32_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, long long> {
	using type = m512_i64_t;
};
template <>
struct get_abi_xmm<fea::simd_ver::avx512f, unsigned long long> {
	using type = m512_u64_t;
};

} // namespace detail

// Given a simd version and a type, returns the fea register type associated
// with it.
template <fea::simd_ver ver, class T>
using simd_abi_register_t = typename detail::get_abi_xmm<ver, T>::type;
} // namespace fea
