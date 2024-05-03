/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/utils/platform.hpp"

#include <cstdint>
#if FEA_CPP23
#include <stdfloat>
#endif

/**
 * Number literals.
 *
 * using namespace fea::literals;
 */
namespace fea {
namespace literals {

constexpr inline int8_t operator"" _8(unsigned long long v) {
	return int8_t(v);
}
constexpr inline int16_t operator"" _16(unsigned long long v) {
	return int16_t(v);
}
constexpr inline int32_t operator"" _32(unsigned long long v) {
	return int32_t(v);
}
constexpr inline int64_t operator"" _64(unsigned long long v) {
	return int64_t(v);
}

// Whichever you prefer, _N or _iN.
constexpr inline int8_t operator"" _i8(unsigned long long v) {
	return int8_t(v);
}
constexpr inline int16_t operator"" _i16(unsigned long long v) {
	return int16_t(v);
}
constexpr inline int32_t operator"" _i32(unsigned long long v) {
	return int32_t(v);
}
constexpr inline int64_t operator"" _i64(unsigned long long v) {
	return int64_t(v);
}

constexpr inline uint8_t operator"" _u8(unsigned long long v) {
	return uint8_t(v);
}
constexpr inline uint16_t operator"" _u16(unsigned long long v) {
	return uint16_t(v);
}
constexpr inline uint32_t operator"" _u32(unsigned long long v) {
	return uint32_t(v);
}
constexpr inline uint64_t operator"" _u64(unsigned long long v) {
	return uint64_t(v);
}

#if FEA_CPP23
#if defined(__STDCPP_FLOAT16_T__)
constexpr inline float16_t operator"" _16(long double v) {
	return float16_t(v);
}
#endif
constexpr inline float32_t operator"" _32(long double v) {
	return float32_t(v);
}
constexpr inline float64_t operator"" _64(long double v) {
	return float64_t(v);
}

// Whichever you prefer, _N or _fN.
#if defined(__STDCPP_FLOAT16_T__)
constexpr inline float16_t operator"" _f16(long double v) {
	return float16_t(v);
}
#endif
constexpr inline float32_t operator"" _f32(long double v) {
	return float32_t(v);
}
constexpr inline float64_t operator"" _f64(long double v) {
	return float64_t(v);
}
#endif

} // namespace literals
} // namespace fea
