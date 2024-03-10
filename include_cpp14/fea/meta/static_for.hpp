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
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/unused.hpp"

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fea {
// Fold expressions for c++ < 17.
// Calls your function with each of the provided variadic argument.
template <class Func, class... Args>
constexpr void fold(Func&& func, Args&&... args) {
#if FEA_CPP17
	(func(args), ...);
#else
	char dummy[] = { (void(func(args)), '0')... };
	unused(dummy);
#endif
}

namespace detail {
// Checks if the all the passed in traits are true.
template <class...>
struct sf_all_of : std::true_type {};

template <class Trait, class... Traits>
struct sf_all_of<Trait, Traits...>
		: std::conditional<Trait::value, sf_all_of<Traits...>,
				  std::false_type>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::all_of : must use type traits");
};

// Return type of the call with I constant.
template <class Func, size_t I>
struct sf_ret {
	using type = decltype(std::declval<Func>()(
			std::integral_constant<size_t, I>{}));
};
template <class Func, size_t I>
using sf_ret_t = typename sf_ret<Func, I>::type;

// Return type or std::nullptr_t if void.
template <class RetT>
struct sf_ret_or_nullptr {
	using type = RetT;
};
template <>
struct sf_ret_or_nullptr<void> {
	using type = std::nullptr_t;
};
template <class RetT>
using sf_ret_or_nullptr_t = typename sf_ret_or_nullptr<RetT>::type;


template <class Func, size_t First, size_t... I>
constexpr bool sf_all_ret_same() {
	return sf_all_of<
			std::is_same<sf_ret_t<Func, First>, sf_ret_t<Func, I>>...>::value;
}

template <bool, class, size_t...>
struct sf_ret_pack_tup_or_arr;

// All same value, return array.
// No need for nullptr_t, if all rets are void we never get here.
template <class Func, size_t First, size_t... I>
struct sf_ret_pack_tup_or_arr<true, Func, First, I...> {
	using type = std::array<sf_ret_t<Func, First>, sizeof...(I) + 1>;
};

// Not all same value, return tuple.
template <class Func, size_t... I>
struct sf_ret_pack_tup_or_arr<false, Func, I...> {
	using type = std::tuple<sf_ret_or_nullptr_t<sf_ret_t<Func, I>>...>;
};
template <class Func, size_t... I>
using sf_ret_pack_tup_or_arr_t =
		typename sf_ret_pack_tup_or_arr<sf_all_ret_same<Func, I...>(), Func,
				I...>::type;

// The final return type.
template <bool, class, size_t...>
struct sf_ret_pack;

// All void return.
template <class Func, size_t... I>
struct sf_ret_pack<true, Func, I...> {
	using type = void;
};

// Not all return void.
template <class Func, size_t... I>
struct sf_ret_pack<false, Func, I...> {
	using type = sf_ret_pack_tup_or_arr_t<Func, I...>;
};

template <class Func, size_t... I>
constexpr bool sf_all_ret_void() {
	return sf_all_of<std::is_same<sf_ret_t<Func, I>, void>...>::value;
}

template <class Func, size_t... I>
using static_for_ret_t =
		typename sf_ret_pack<sf_all_ret_void<Func, I...>(), Func, I...>::type;


template <class Ret, class Func, size_t I>
struct pass_through {
	constexpr Ret operator()(Func& func) const {
		return func(std::integral_constant<size_t, I>{});
	}
};
template <class Func, size_t I>
struct pass_through<void, Func, I> {
	constexpr std::nullptr_t operator()(Func& func) const {
		func(std::integral_constant<size_t, I>{});
		return std::nullptr_t{};
	}
};


template <class RetT, class Func, size_t... I>
struct do_static_for {
	constexpr RetT operator()(Func& func) const {
		return RetT{ pass_through<sf_ret_t<Func, I>, Func, I>{}(func)... };
	}
};

template <class Func, size_t... I>
struct do_static_for<void, Func, I...> {
	constexpr void operator()(Func& func) const {
#if FEA_CPP17
		(func(std::integral_constant<size_t, I>{}), ...);
#else
		char dummy[]
				= { (void(func(std::integral_constant<size_t, I>{})), '0')... };
		unused(dummy);
#endif
	}
};


template <class Func, size_t... I>
constexpr auto static_for(Func& func, std::index_sequence<I...>) {
	return do_static_for<static_for_ret_t<Func, I...>, Func, I...>{}(func);
}

} // namespace detail

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
// Accept it with auto, access the index with '::value' or '()'.
// If any of your lambda invocations returns values:
// they are concatenated in an array if all identical types, or tuple if not.
// std::nullptr_t is used as a sentinel to denote invocations that don't return
// anything (when returning tuples).
// If all invocations return void, returns void.
template <size_t N, class Func>
constexpr auto static_for(Func&& func) {
	return detail::static_for(func, std::make_index_sequence<N>{});
}

#if FEA_CPP17 // && !FEA_VS2019
// Same as static_for, but reversed.
// Starts at N - 1, ends at 0.
// VS2019 dies.
template <size_t N, class Func>
constexpr auto static_for_reversed(Func&& func) {
	// using idx_seq_t = std::make_index_sequence<N>;
	using rev_seq_t = typename fea::reverse_index_sequence<
			std::make_index_sequence<N>>::type;
	return detail::static_for(func, rev_seq_t{});
}
#endif


// "std::apply index_sequence"
namespace detail {
template <class Func, size_t... Idx>
constexpr auto apply_indexes(Func&& f, std::index_sequence<Idx...>) {
	return std::forward<Func>(f)(std::integral_constant<size_t, Idx>{}...);
}
} // namespace detail

template <size_t N, class Func>
constexpr auto apply_indexes(Func&& f) {
	return detail::apply_indexes(
			std::forward<Func>(f), std::make_index_sequence<N>{});
}

} // namespace fea
