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

#include <tuple>
#include <type_traits>

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
	static constexpr size_t value = 0;
};

template <class T, class U, class... Types>
struct tuple_idx<T, std::tuple<U, Types...>> {
	static constexpr size_t value
			= 1 + tuple_idx<T, std::tuple<Types...>>::value;
};

#if FEA_CPP17
// Get the index of type T in Tuple.
template <class T, class Tuple>
inline constexpr size_t tuple_idx_v = tuple_idx<T, Tuple>::value;
#endif


// Does Tuple contain type T?
template <class T, class Tuple>
struct tuple_contains;

template <class T>
struct tuple_contains<T, std::tuple<>> : std::false_type {};

template <class T, class U, class... Ts>
struct tuple_contains<T, std::tuple<U, Ts...>>
		: tuple_contains<T, std::tuple<Ts...>> {};

template <class T, class... Ts>
struct tuple_contains<T, std::tuple<T, Ts...>> : std::true_type {};

#if FEA_CPP17
// Does Tuple contain type T?
template <class T, class Tuple>
inline constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;
#endif

} // namespace fea
