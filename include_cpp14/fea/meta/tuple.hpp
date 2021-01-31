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
#include "fea/utils/platform.hpp"
#include "fea/utils/unused.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace fea {
namespace detail {
template <size_t, class T>
using accept_idx = T;

template <class T, size_t... Is>
auto make_tuple_from_count(std::index_sequence<Is...>) {
	return std::tuple<accept_idx<Is, T>...>{};
}
} // namespace detail

// Create a tuple using type T and count N.
template <class T, size_t N>
auto make_tuple_from_count() {
	return detail::make_tuple_from_count<T>(std::make_index_sequence<N>{});
}


// Get the index of type T in Tuple.
template <class T, class Tuple>
struct tuple_idx {
	static_assert(!std::is_same<Tuple, std::tuple<>>::value,
			"could not find T in given Tuple");
};

template <class T, class... Types>
struct tuple_idx<T, std::tuple<T, Types...>> {
	// Ends recursion, returns 0 for index addition.
	static constexpr size_t value = 0;
};

template <class T, class U, class... Types>
struct tuple_idx<T, std::tuple<U, Types...>> {
	// Recursion ends once T is found with 1 + 0 (see previous
	// specicialization).
	static constexpr size_t value
			= 1 + tuple_idx<T, std::tuple<Types...>>::value;
};

// Get the index of type T in Tuple.
template <class T, class Tuple>
FEA_INLINE_VAR constexpr size_t tuple_idx_v = tuple_idx<T, Tuple>::value;


// Does Tuple contain type T?
template <class, class>
struct tuple_contains;

template <class T>
struct tuple_contains<T, std::tuple<>> : std::false_type {};

template <class T, class U, class... Ts>
struct tuple_contains<T, std::tuple<U, Ts...>>
		: tuple_contains<T, std::tuple<Ts...>> {};

template <class T, class... Ts>
struct tuple_contains<T, std::tuple<T, Ts...>> : std::true_type {};

// Does Tuple contain type T?
template <class T, class Tuple>
FEA_INLINE_VAR constexpr bool tuple_contains_v
		= tuple_contains<T, Tuple>::value;


namespace detail {
template <class Func, class Tuple, size_t... I>
constexpr void tuple_foreach(
		Func&& func, Tuple&& tup, std::index_sequence<I...>) {
#if FEA_CPP17
	// TODO : test it.
	(func(std::get<I>(tup)), ...);
#else
	char dummy[] = { (void(func(std::get<I>(tup))), '0')... };
	unused(dummy);
#endif
}
} // namespace detail

// Tuple foreach.
// Your lambda will be called with each tuple's elements.
// Provid lambda which accepts auto& or const auto&.
template <class Func, class Tuple>
constexpr void tuple_for_each(Func&& func, Tuple&& tup) {
	detail::tuple_foreach(std::forward<Func>(func), std::forward<Tuple>(tup),
			std::make_index_sequence<
					std::tuple_size<std::decay_t<Tuple>>::value>{});
}


// C++ < 17 std::apply
#if FEA_CPP17
using std::apply;
#else
namespace detail {
template <class F, class Tuple, size_t... Idx>
constexpr decltype(auto) apply_impl(
		F&& f, Tuple&& t, std::index_sequence<Idx...>) {
	return std::forward<F>(f)(std::get<Idx>(std::forward<Tuple>(t))...);
}
} // namespace detail

template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
	return detail::apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
			std::make_index_sequence<std::tuple_size<
					typename std::remove_reference<Tuple>::type>::value>{});
}
#endif

// tuple_cats 2 tuple types together.
template <class, class>
struct tuple_type_cat;

template <class... First, class... Second>
struct tuple_type_cat<std::tuple<First...>, std::tuple<Second...>> {
	using type = std::tuple<First..., Second...>;
};

template <class... Args>
using tuple_type_cat_t = typename tuple_type_cat<Args...>::type;
} // namespace fea
