/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include <cstddef>
#include <type_traits>

/*
Helper to add bitmask operations to an enum class.

This allows you to work with type-safe bitmasks without having to cast
constantly.

https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/

Doesn't use the enable_if version because compile times + keeping it simple.

If you want to use is_bitmask type-trait, you must enable it in a global
namespace using FEA_ENABLE_IS_BITMASK.
*/

namespace fea {
template <class T>
struct is_bitmask : std::false_type {};

#if FEA_CPP17
template <class T>
inline constexpr bool is_bitmask_v = is_bitmask<T>::value;
#else
template <class T>
static constexpr bool is_bitmask_v = is_bitmask<T>::value;
#endif
} // namespace fea

// Unfortunately, must be defined seperately because this must be in global
// namespace.
#define FEA_ENABLE_IS_BITMASK(e) \
	namespace fea { \
	template <> \
	struct is_bitmask<e> : std::true_type {}; \
	}


// Implements arithmetic bit operations on enum class.
// Keeps your code type-safe.
#define FEA_ENABLE_BITMASK_OPERATORS(e) \
	inline constexpr e operator|(e lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(underlying_t(lhs) | underlying_t(rhs)); \
	} \
\
	inline constexpr e operator&(e lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(underlying_t(lhs) & underlying_t(rhs)); \
	} \
\
	inline constexpr e operator^(e lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(underlying_t(lhs) ^ underlying_t(rhs)); \
	} \
\
	inline constexpr e operator~(e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(~underlying_t(rhs)); \
	} \
\
	inline constexpr e operator<<(e lhs, size_t rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(underlying_t(lhs) << rhs); \
	} \
\
	inline constexpr e operator>>(e lhs, size_t rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		return e(underlying_t(lhs) >> rhs); \
	} \
\
	inline constexpr e& operator|=(e& lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		lhs = e(underlying_t(lhs) | underlying_t(rhs)); \
		return lhs; \
	} \
\
	inline constexpr e& operator&=(e& lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		lhs = e(underlying_t(lhs) & underlying_t(rhs)); \
		return lhs; \
	} \
\
	inline constexpr e& operator^=(e& lhs, e rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		lhs = e(underlying_t(lhs) ^ underlying_t(rhs)); \
		return lhs; \
	} \
\
	inline constexpr e& operator<<=(e& lhs, size_t rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		lhs = e(underlying_t(lhs) << rhs); \
		return lhs; \
	} \
\
	inline constexpr e& operator>>=(e& lhs, size_t rhs) { \
		using underlying_t = typename std::underlying_type<e>::type; \
		lhs = e(underlying_t(lhs) >> rhs); \
		return lhs; \
	}
