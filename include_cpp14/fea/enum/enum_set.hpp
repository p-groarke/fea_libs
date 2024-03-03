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
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>

/*
enum_set is a wrapper on std::array<bool>, which allows to access elements with
an enum class directly. Your enum must be from 0 to N!

If your enum declares 'count', you do not need to provide the size.

enum_set doesn't change much about std::array,
but it adds constructors and accessors to facilitate access.
*/

// TODO : enum_bitset

namespace fea {

template <class Enum, size_t N = size_t(Enum::count)>
struct enum_set : public std::array<bool, N> {
	using array_t = std::array<bool, N>;
	using reference = typename array_t::reference;
	using const_reference = typename array_t::const_reference;

	constexpr enum_set() = default;
	constexpr enum_set(std::initializer_list<Enum> true_set)
			: std::array<bool, N>{} {
		for (Enum e : true_set) {
			operator[](e) = true;
		}
	}
	template <class... Vals>
	constexpr enum_set(Vals... values)
			: std::array<bool, N>{ values... } {
		static_assert(sizeof...(Vals) == N,
				"fea::enum_set : Invalid number of arguments.");
		static_assert(fea::all_of<std::is_same<Vals, bool>...>::value,
				"fea::enum_set : Invalid constructor parameters, expected "
				"bools.");
	}

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
template <auto I, class E, size_t N>
constexpr bool& get(enum_set<E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}
template <auto I, class E, size_t N>
constexpr bool&& get(enum_set<E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}
template <auto I, class E, size_t N>
constexpr const bool& get(const enum_set<E, N>& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(a);
}
template <auto I, class E, size_t N>
constexpr const bool&& get(const enum_set<E, N>&& a) noexcept {
	static_assert(std::is_same_v<decltype(I), E>,
			"enum_set : passed in wrong enum type");
	return std::get<size_t(I)>(std::move(a));
}
#endif

} // namespace fea
