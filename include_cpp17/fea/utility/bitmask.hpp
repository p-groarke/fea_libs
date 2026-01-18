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
#include <cstddef>
#include <type_traits>

/*
Helper to add bitmask operations to an enum class.

This allows you to work with type-safe bitmasks without having to cast
constantly.

https://wiggling-bits.net/using-enum-classes-as-type-safe-bitmasks/

To register an enum as a bitmask, in the fea namespace, specialize the
register_bitmask struct inheriting std::true_type.
*/

// Helper to register (specialize) your enum as a bitmask.
// Use in namespace fea.
#define FEA_REGISTER_BITMASK(e) \
	template <> \
	struct register_bitmask<e> : std::true_type {}

namespace fea {
template <class T>
struct register_bitmask : std::false_type {};

template <class T>
inline constexpr bool is_bitmask_v = register_bitmask<T>::value;
} // namespace fea

// Implements arithmetic bit operations on enum class.
// Keeps your code type-safe.
template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator|(E lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator&(E lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator^(E lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator~(E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator<<(
		E lhs, size_t rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator>>(
		E lhs, size_t rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator|=(E& lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator&=(E& lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator^=(E& lhs, E rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator<<=(
		E& lhs, size_t rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator>>=(
		E& lhs, size_t rhs);

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, bool> operator!(E rhs);


// Implementation
template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator|(E lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(underlying_t(lhs) | underlying_t(rhs));
}


template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator&(E lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(underlying_t(lhs) & underlying_t(rhs));
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator^(E lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(underlying_t(lhs) ^ underlying_t(rhs));
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator~(E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(~underlying_t(rhs));
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator<<(
		E lhs, size_t rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(underlying_t(lhs) << rhs);
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E> operator>>(
		E lhs, size_t rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return E(underlying_t(lhs) >> rhs);
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator|=(E& lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	lhs = E(underlying_t(lhs) | underlying_t(rhs));
	return lhs;
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator&=(E& lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	lhs = E(underlying_t(lhs) & underlying_t(rhs));
	return lhs;
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator^=(E& lhs, E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	lhs = E(underlying_t(lhs) ^ underlying_t(rhs));
	return lhs;
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator<<=(
		E& lhs, size_t rhs) {
	using underlying_t = std::underlying_type_t<E>;
	lhs = E(underlying_t(lhs) << rhs);
	return lhs;
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, E>& operator>>=(
		E& lhs, size_t rhs) {
	using underlying_t = std::underlying_type_t<E>;
	lhs = E(underlying_t(lhs) >> rhs);
	return lhs;
}

template <class E>
constexpr std::enable_if_t<fea::is_bitmask_v<E>, bool> operator!(E rhs) {
	using underlying_t = std::underlying_type_t<E>;
	return underlying_t(rhs) == underlying_t(0);
}
