/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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

#include <cassert>
#include <type_traits>

#if FEA_WINDOWS
#include <intrin.h>

#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#if FEA_ARCH == 64
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#endif
#endif

/*
Cross-platform intrinsics.
*/

namespace fea {
namespace detail {
template <class T, size_t BitCount, size_t LSBPos>
constexpr auto make_bitmask() {
	T ret{ 0 };
	for (size_t i = LSBPos; i < LSBPos + BitCount; ++i) {
		ret |= T(1) << i;
	}
	return ret;
}
} // namespace detail

/*
Build a bit mask of 'Count' * ones, starting at least significant bit
'LSBPos' (starting from the "right").

For ex,
make_bitmask<uint8_t, 3, 1>() == 0b0000'1110;
make_bitmask<uint8_t, 5, 2>() == 0b0111'1100;
*/
template <class T, size_t BitCount, size_t LSBPos = 0>
constexpr auto make_bitmask() {
	static_assert(
			std::is_integral_v<T>, "make_bitmask : expects integral types");
	static_assert(BitCount + LSBPos <= sizeof(T) * 8,
			"make_bitmask : resulting mask doesn't fit in T, would cause "
			"overflow");

	// Force constexpr.
	constexpr T mask = detail::make_bitmask<T, BitCount, LSBPos>();
	return mask;
}

/*
Build a bit mask of 'Count' * ones, starting at least significant bit
'LSBPos' (starting from the "right").

For ex,
make_bitmask<unsigned>(3, 1) == 0b0000'1110;
make_bitmask<unsigned>(5, 2) == 0b0111'1100;
*/
template <class T>
T make_bitmask(size_t bit_count, size_t lsb_pos = 0) {
	static_assert(
			std::is_integral_v<T>, "make_bitmask : expects integral types");
	assert(bit_count + lsb_pos <= sizeof(T) * 8);

	T ret{ 0 };
	for (size_t i = lsb_pos; i < lsb_pos + bit_count; ++i) {
		ret |= T(1) << i;
	}
	return ret;
}

// Casts the type to fit in uint32_t or uint64_t.
template <class T>
constexpr auto to_unsigned(T t) noexcept {
	static_assert(std::is_integral_v<T>, "to_ulong : expects integral types");

	if constexpr (sizeof(T) <= 4) {
		return uint32_t(t);
	} else if constexpr (sizeof(T) <= 8) {
		return uint64_t(t);
	} else {
		// someday...
		assert(false);
		return uint64_t(t);
	}
}

// Casts the type to fit in uint32_t or uint64_t.
// If the type size is smaller than the target long, shifts to MSB (left).
template <class T>
constexpr auto to_unsigned_pack_left(T t) noexcept {
	auto ret = to_unsigned(t);

	if constexpr (sizeof(T) < 4) {
		constexpr size_t diff = 32 - sizeof(T) * 8;
		return ret << diff;
	} else {
		return ret;
	}
}

// Counts the number of consecutive 0 bits, starting from the most significant
// bit ("left").
// Effectively gives you the index of the first set bit, starting from the MSB.
template <class T>
size_t countl_zero(T val) {
	static_assert(
			std::is_integral_v<T>, "bit scan intrinsics expect integral types");

	/*
	OK so, some doc.

	Windows bitscans ALLWAYS return the lsb index, whether the scan is forward
	or reverse. Windows also provides __lzcnt, though that requires hardware
	support.

	Unixes (gcc-style) ffs return 1 + index, so instead we must use ctz and clz,
	count trailing zeros and count leading zeros.

	The docs are relatively vague about the results when the input is 0, however
	wikipedia lists mosts operations on 0 as undefined, so we return on 0.
	*/
	constexpr T zero{ 0 };
	if (val == zero) {
		constexpr size_t ret = 8 * sizeof(T);
		return ret;
	}

	auto uval = fea::to_unsigned_pack_left(val);
	unsigned long ret;

	if constexpr (sizeof(uval) == 4) {
#if FEA_WINDOWS
		_BitScanReverse(&ret, uval);
		ret = 31 - ret;
#else
		ret = __builtin_clz(uval);
#endif
	} else {
#if FEA_ARCH == 64
#if FEA_WINDOWS
		_BitScanReverse64(&ret, uval);
		ret = 63 - ret;
#else
		ret = __builtin_clzll(uval);
#endif
#else
		ret = 64;
#endif
	}

	return size_t(ret);
}

// Counts the number of consecutive 0 bits, starting from the least
// significant bit ("right").
// Effectively gives you the index of the first set bit, starting from the LSB.
template <class T>
size_t countr_zero(T val) {
	static_assert(
			std::is_integral_v<T>, "bit scan intrinsics expect integral types");

	constexpr T zero{ 0 };
	if (val == zero) {
		constexpr size_t ret = 8 * sizeof(T);
		return ret;
	}

	auto uval = fea::to_unsigned(val);
	unsigned long ret;

	if constexpr (sizeof(uval) == 4) {
#if FEA_WINDOWS
		_BitScanForward(&ret, uval);
#else
		ret = __builtin_ctz(uval);
#endif
	} else {
#if FEA_ARCH == 64
#if FEA_WINDOWS
		_BitScanForward64(&ret, uval);
#else
		ret = __builtin_ctzll(uval);
#endif
#else
		ret = 64;
#endif
	}

	return size_t(ret);
}

// TODO :
//// Counts the number of consecutive 1 bits, starting from the most significant
//// bit ("left").
// template <class T>
// size_t countl_one() {
//}

//// Counts the number of consecutive 1 bits, starting from the least
/// significant / bit ("right").
// template <class T>
// size_t countr_one() {
//}

} // namespace fea
