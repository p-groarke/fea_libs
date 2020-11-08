/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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
#include <type_traits>

#define FEA_ENABLE_BITMASK_OPERATORS(x) \
	template <> \
	struct has_bitmask_operators<x> : std::true_type { \
		static_assert( \
				std::is_enum<x>::value && !std::is_convertible<x, int>::value, \
				"template parameter is not an enum class type"); \
	}

namespace fea {
template <class T>
struct has_bitmask_operators : std::false_type {};

#if __cplusplus >= 201703L
template <class T>
inline constexpr bool has_bitmask_operators_v = has_bitmask_operators<T>::value;
#endif


template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T>
operator|(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T>
operator&(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T>
operator^(T lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(
			static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T>
operator~(T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<underlying>(rhs));
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T&>
operator|=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	return lhs;
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T&>
operator&=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
	return lhs;
}

template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value, T&>
operator^=(T& lhs, T rhs) {
	using underlying = typename std::underlying_type_t<T>;
	lhs = static_cast<T>(
			static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
	return lhs;
}

/* Helper */
template <class T>
inline constexpr typename std::enable_if_t<has_bitmask_operators<T>::value,
		std::underlying_type_t<T>>
bitmask_cast(T e) {
	return static_cast<std::underlying_type_t<T>>(e);
}
} // namespace fea
