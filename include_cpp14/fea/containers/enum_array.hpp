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
#include "fea/meta/static_for.hpp"
#include "fea/utils/platform.hpp"

#include <array>
#include <type_traits>
#include <utility>

/*
enum_array is a wrapper on std::array, which allows to access elements with
an enum class directly. Your enum must be from 0 to N!

If your enum declares 'count', you do not need to provide the size.

enum_array doesn't change anything to std::array other than overriding the
accessors (at, operator[] and std::get) to accept your enums directly.
*/

namespace fea {

template <class T, class Enum, size_t N = size_t(Enum::count)>
struct enum_array : public std::array<T, N> {
	using array_t = std::array<T, N>;
	using reference = typename array_t::reference;
	using const_reference = typename array_t::const_reference;

	template <Enum E>
	constexpr reference at() {
		return std::get<size_t(E)>(*this);
	}
	template <Enum E>
	constexpr const_reference at() const {
		return std::get<size_t(E)>(*this);
	}

	constexpr reference at(Enum e) {
		return array_t::at(size_t(e));
	}
	constexpr const_reference at(Enum e) const {
		return array_t::at(size_t(e));
	}

	constexpr reference operator[](Enum e) {
		return array_t::operator[](size_t(e));
	}
	constexpr const_reference operator[](Enum e) const {
		return array_t::operator[](size_t(e));
	}
};

#if FEA_CPP17
template <auto I, class T, class E, size_t N>
constexpr T& get(enum_array<T, E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}
template <auto I, class T, class E, size_t N>
constexpr T&& get(enum_array<T, E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}
template <auto I, class T, class E, size_t N>
constexpr const T& get(const enum_array<T, E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}
template <auto I, class T, class E, size_t N>
constexpr const T&& get(const enum_array<T, E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_array : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}

// A non-type key and value pair.
template <auto K, class V>
struct kv_t {
	static constexpr auto key = K;

	kv_t(V&& v)
			: _v(std::forward<V>(v)) {
	}

	const V& value() const {
		return _v;
	}
	V& value() {
		return _v;
	}

private:
	V _v;
};

// Make kv_t. Deduces the value type for cleaner look.
template <auto K, class V>
constexpr auto kv(V&& v) {
	return kv_t<K, V>{ std::forward<V>(v) };
}

// Purely non-type key value pair.
template <auto K, auto V>
struct kv_nt {
	static constexpr auto key = K;
	static constexpr auto value = V;
};

// Create an enum_array with the specified enums set to a default value.
// Unspecified values are default initialized.
// ex:
// fea::make_enum_array<e>(fea::kv<e::one>(true), fea::kv<e::two>(true));
// template <class Enum, size_t N = size_t(Enum::count), Enum... Es, class T>
// constexpr auto make_enum_array(kv_t<Es, T>&&... kv_pairs) {
//	// some sort of conditional fold, which returns {} for missing enums.
//	apply_indexes<N>([](auto... idx) {
//		(decltype(idx)::value ==); // FIGURE THIS OUT
//	});
//	return enum_array<T, Enum, N>{};
//}

// If you can pass the variables as non-type, it ends up prettier.
template <class Enum, size_t N = size_t(Enum::count), Enum... Es, auto T>
constexpr auto make_enum_array(kv_nt<Es, T>&&...) {
	// some sort of conditional fold, which returns {} for missing enums.
	return enum_array<T, Enum, N>{};
}

#endif

} // namespace fea
