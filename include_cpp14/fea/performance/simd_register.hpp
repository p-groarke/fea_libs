
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
#include "fea/utils/platform.hpp"

#include <array>
#include <immintrin.h>
#include <type_traits>

namespace fea {
namespace detail {
template <size_t RegisterSize, class T>
struct simd_rbase;

/**
 * MMX
 */
template <class T>
struct simd_rbase<64, T> {
#if FEA_MMX
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m64 in)
			: xmm(in) {
	}
	__m64 xmm;
#else
	alignas(8) std::array<T, 8 / sizeof(T)> xmm;
#endif
};


/**
 * SSE
 */
template <>
struct simd_rbase<128, float> {
#if FEA_SSE
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m128 in)
			: xmm(in) {
	}
	__m128 xmm;
#else
	alignas(16) std::array<float, 4> xmm;
#endif
};


/**
 * SSE2
 */
template <>
struct simd_rbase<128, double> {
#if FEA_SSE2
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m128d in)
			: xmm(in) {
	}
	__m128d xmm;
#else
	alignas(16) std::array<double, 2> xmm;
#endif
};

template <class T>
struct simd_rbase<128, T> {
	static_assert(std::is_integral<T>::value, "something went horribly wrong");
#if FEA_SSE2
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m128i in)
			: xmm(in) {
	}
	__m128i xmm;
#else
	alignas(16) std::array<T, 16 / sizeof(T)> xmm;
#endif
};


/**
 * AVX
 */
template <>
struct simd_rbase<256, float> {
#if FEA_AVX
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m256 in)
			: xmm(in) {
	}
	__m256 xmm;
#else
	alignas(32) std::array<float, 8> xmm;
#endif
};

template <>
struct simd_rbase<256, double> {
#if FEA_AVX
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m256d in)
			: xmm(in) {
	}
	__m256d xmm;
#else
	alignas(32) std::array<double, 4> xmm;
#endif
};

template <class T>
struct simd_rbase<256, T> {
	static_assert(std::is_integral<T>::value, "something went horribly wrong");
#if FEA_AVX
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m256i in)
			: xmm(in) {
	}
	__m256i xmm;
#else
	alignas(32) std::array<T, 32 / sizeof(T)> xmm;
#endif
};


/**
 * AVX512F
 */
template <>
struct simd_rbase<512, float> {
#if FEA_AVX512F
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m512 in)
			: xmm(in) {
	}
	__m512 xmm;
#else
	alignas(64) std::array<float, 16> xmm;
#endif
};

template <>
struct simd_rbase<512, double> {
#if FEA_AVX512F
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m512d in)
			: xmm(in) {
	}
	__m512d xmm;
#else
	alignas(64) std::array<double, 8> xmm;
#endif
};

template <class T>
struct simd_rbase<512, T> {
	static_assert(std::is_integral<T>::value, "something went horribly wrong");
#if FEA_AVX512F
	FEA_FORCEINLINE simd_rbase() = default;
	FEA_FORCEINLINE simd_rbase(__m512i in)
			: xmm(in) {
	}
	__m512i xmm;
#else
	alignas(64) std::array<T, 64 / sizeof(T)> xmm;
#endif
};


template <size_t RegisterSize, class T>
struct simd_register : public simd_rbase<RegisterSize, T> {
	static_assert(
			std::is_integral<T>::value || std::is_floating_point<T>::value,
			"fea::simd : Registers only support integral or floating types.");

	// All sizes in bits.
	static constexpr size_t register_size_v = RegisterSize;
	static constexpr size_t type_size_v = sizeof(T) * 8;
	using value_type = T;

	using simd_rbase<RegisterSize, T>::simd_rbase;
};
} // namespace detail


/**
 * MMX
 */
struct m64_f32_t : public detail::simd_register<64, float> {
	using simd_register::simd_register;
};
struct m64_f64_t : public detail::simd_register<64, double> {
	using simd_register::simd_register;
};
struct m64_i8_t : public detail::simd_register<64, char> {
	using simd_register::simd_register;
};
struct m64_u8_t : public m64_i8_t {
	using value_type = unsigned char;
	using m64_i8_t::m64_i8_t;
};
struct m64_ii8_t : public m64_i8_t {
	using value_type = signed char;
	using m64_i8_t::m64_i8_t;
};
struct m64_i16_t : public detail::simd_register<64, short> {
	using simd_register::simd_register;
};
struct m64_u16_t : public m64_i16_t {
	using value_type = unsigned short;
	using m64_i16_t::m64_i16_t;
};
struct m64_i32_t : public detail::simd_register<64, int> {
	using simd_register::simd_register;
};
struct m64_u32_t : public m64_i32_t {
	using value_type = unsigned int;
	using m64_i32_t::m64_i32_t;
};
struct m64_i64_t : public detail::simd_register<64, long long> {
	using simd_register::simd_register;
};
struct m64_u64_t : public m64_i64_t {
	using value_type = unsigned long long;
	using m64_i64_t::m64_i64_t;
};


/**
 * SSE & SSE2
 */
struct m128_f32_t : public detail::simd_register<128, float> {
	using simd_register::simd_register;
};
struct m128_f64_t : public detail::simd_register<128, double> {
	using simd_register::simd_register;
};
struct m128_i8_t : public detail::simd_register<128, char> {
	using simd_register::simd_register;
};
struct m128_u8_t : public m128_i8_t {
	using value_type = unsigned char;
	using m128_i8_t::m128_i8_t;
};
struct m128_ii8_t : public m128_i8_t {
	using value_type = signed char;
	using m128_i8_t::m128_i8_t;
};
struct m128_i16_t : public detail::simd_register<128, short> {
	using simd_register::simd_register;
};
struct m128_u16_t : public m128_i16_t {
	using value_type = unsigned short;
	using m128_i16_t::m128_i16_t;
};
struct m128_i32_t : public detail::simd_register<128, int> {
	using simd_register::simd_register;
};
struct m128_u32_t : public m128_i32_t {
	using value_type = unsigned int;
	using m128_i32_t::m128_i32_t;
};
struct m128_i64_t : public detail::simd_register<128, long long> {
	using simd_register::simd_register;
};
struct m128_u64_t : public m128_i64_t {
	using value_type = unsigned long long;
	using m128_i64_t::m128_i64_t;
};


/**
 * AVX
 */
struct m256_f32_t : public detail::simd_register<256, float> {
	using simd_register::simd_register;
};
struct m256_f64_t : public detail::simd_register<256, double> {
	using simd_register::simd_register;
};
struct m256_i8_t : public detail::simd_register<256, char> {
	using simd_register::simd_register;
};
struct m256_u8_t : public m256_i8_t {
	using value_type = unsigned char;
	using m256_i8_t::m256_i8_t;
};
struct m256_ii8_t : public m256_i8_t {
	using value_type = signed char;
	using m256_i8_t::m256_i8_t;
};
struct m256_i16_t : public detail::simd_register<256, short> {
	using simd_register::simd_register;
};
struct m256_u16_t : public m256_i16_t {
	using value_type = unsigned short;
	using m256_i16_t::m256_i16_t;
};
struct m256_i32_t : public detail::simd_register<256, int> {
	using simd_register::simd_register;
};
struct m256_u32_t : public m256_i32_t {
	using value_type = unsigned int;
	using m256_i32_t::m256_i32_t;
};
struct m256_i64_t : public detail::simd_register<256, long long> {
	using simd_register::simd_register;
};
struct m256_u64_t : public m256_i64_t {
	using value_type = unsigned long long;
	using m256_i64_t::m256_i64_t;
};


/**
 * AVX512F
 */
struct m512_f32_t : public detail::simd_register<512, float> {
	using simd_register::simd_register;
};
struct m512_f64_t : public detail::simd_register<512, double> {
	using simd_register::simd_register;
};
struct m512_i8_t : public detail::simd_register<512, char> {
	using simd_register::simd_register;
};
struct m512_u8_t : public m512_i8_t {
	using value_type = unsigned char;
	using m512_i8_t::m512_i8_t;
};
struct m512_ii8_t : public m512_i8_t {
	using value_type = signed char;
	using m512_i8_t::m512_i8_t;
};
struct m512_i16_t : public detail::simd_register<512, short> {
	using simd_register::simd_register;
};
struct m512_u16_t : public m512_i16_t {
	using value_type = unsigned short;
	using m512_i16_t::m512_i16_t;
};
struct m512_i32_t : public detail::simd_register<512, int> {
	using simd_register::simd_register;
};
struct m512_u32_t : public m512_i32_t {
	using value_type = unsigned int;
	using m512_i32_t::m512_i32_t;
};
struct m512_i64_t : public detail::simd_register<512, long long> {
	using simd_register::simd_register;
};
struct m512_u64_t : public m512_i64_t {
	using value_type = unsigned long long;
	using m512_i64_t::m512_i64_t;
};
} // namespace fea
