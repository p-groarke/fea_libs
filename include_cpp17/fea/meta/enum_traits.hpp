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
#include "fea/containers/enum_array.hpp"
#include "fea/meta/non_type_traits.hpp"
#include "fea/meta/traits.hpp"

#include <array>
#include <limits>
#include <type_traits>

/*
fea::explode_enum calls your lambda with a variadic pack of
std::integral_constant<your_enum, val>...

*/

namespace fea {
// static_cast to the enum underlying type.
template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e);

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// You must "extract" the non-type enum as it is passed by
// Passes std::integral_constant...
template <class Enum, size_t N, class Func>
constexpr auto explode_enum(const Func& func);

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// Overload for enums that contain a 'count' member.
// Passes std::integral_constant...
template <class Enum, class Func>
constexpr auto explode_enum(const Func& func);

// Calls your function with each non-type enum values.
// Enum must be from 0 to N.
// Provide N if your enum doesn't have the member 'count'.
// Passes std::integral_constant.
template <class Enum, size_t N = size_t(Enum::count), class Func>
constexpr void enum_for_each(const Func& func);

// Calls your function with each non-type enum values.
// Passes std::integral_constant.
template <auto... Args, class Func>
constexpr void enum_for_each(const Func& func);

// Creates a lookup array of size max enum + 1.
// Dereference the lookup with an enum value, to get its index in the variadic
// pack.
// Effectively, enables creation of programmatic switch-case lookups.
template <auto... Args>
constexpr auto make_enum_lookup();
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class Enum, class Func, size_t... Idx>
constexpr auto explode_enum(const Func& func, std::index_sequence<Idx...>) {
	return func(std::integral_constant<Enum, Enum(Idx)>{}...);
}

// Calls your function with each non-type enum values.
// Passes idx std::integral_constant and enum std::integral_constant.
template <class E, class Func, size_t... Idx, auto... Args>
constexpr void enum_for_each_w_idx_imp(const Func& func,
		std::index_sequence<Idx...>, std::integer_sequence<size_t, Args...>) {
	(func(std::integral_constant<size_t, Idx>{},
			 std::integral_constant<E, E(Args)>{}),
			...);
}

// Calls your function with each non-type enum values.
// Passes idx std::integral_constant and enum std::integral_constant.
template <auto... Args, class Func>
constexpr void enum_for_each_w_idx(const Func& func) {
	using enum_t = fea::front_t<decltype(Args)...>;
	enum_for_each_w_idx_imp<enum_t>(func,
			std::make_index_sequence<sizeof...(Args)>{},
			std::integer_sequence<size_t, size_t(Args)...>{});
}
} // namespace detail

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) {
	static_assert(std::is_enum_v<Enum>,
			"fea::to_underlying : Only supports casting enums.");
	return static_cast<std::underlying_type_t<Enum>>(e);
}

template <class Enum, size_t N, class Func>
constexpr auto explode_enum(const Func& func) {
	return detail::explode_enum<Enum>(func, std::make_index_sequence<N>{});
}

template <class Enum, class Func>
constexpr auto explode_enum(const Func& func) {
	return detail::explode_enum<Enum>(
			func, std::make_index_sequence<size_t(Enum::count)>{});
}

template <class Enum, size_t N, class Func>
constexpr void enum_for_each(const Func& func) {
	fea::explode_enum<Enum, N>(
			[&](auto... constants) { (func(constants), ...); });
}

template <auto... Args, class Func>
constexpr void enum_for_each(const Func& func) {
	(func(std::integral_constant<decltype(Args), Args>{}), ...);
}

template <auto... Args>
constexpr auto make_enum_lookup() {
	constexpr size_t arr_size
			= size_t(fea::max_nt_v<fea::front_t<decltype(Args)...>, Args...>)
			+ 1u;
	std::array<size_t, arr_size> ret{};

	// Initialize everything with sentinel.
	fea::static_for<arr_size>(
			[&](auto idx) { ret[idx] = (std::numeric_limits<size_t>::max)(); });

	// Create association between enum value and actual index.
	// Aka, slot map from enum -> pack index
	detail::enum_for_each_w_idx<Args...>([&](auto idx, auto e_ic) {
		constexpr size_t i = decltype(idx)::value;
		constexpr auto e = decltype(e_ic)::value;
		constexpr size_t e_pos = size_t(e);
		ret[e_pos] = i;
	});
	return ret;
}
} // namespace fea
