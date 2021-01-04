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
#include "fea/meta/tuple.hpp"
#include "fea/utils/platform.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace fea {
// Checks if the all the passed in traits are true.
template <class...>
struct all_of : std::true_type {};

template <class Trait, class... Traits>
struct all_of<Trait, Traits...>
		: std::conditional<Trait::value, all_of<Traits...>,
				  std::false_type>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::all_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool all_of_v = all_of<Traits...>::value;


// Checks if none of the passed in traits are true.
template <class...>
struct none_of : std::true_type {};

template <class Trait, class... Traits>
struct none_of<Trait, Traits...>
		: std::conditional<Trait::value, std::false_type,
				  none_of<Traits...>>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::none_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool none_of_v = none_of<Traits...>::value;


// Checks if any of the passed in traits are true.
template <class...>
struct any_of : std::false_type {};

template <class Trait, class... Traits>
struct any_of<Trait, Traits...> : std::conditional<Trait::value, std::true_type,
										  any_of<Traits...>>::type {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::any_of : must use type traits");
};

template <class... Traits>
FEA_INLINE_VAR constexpr bool any_of_v = any_of<Traits...>::value;


// TODO : C++14 implementation
// template <class...>
// constexpr bool all_unique_v = std::true_type{};
//
// template <class T, class... Rest>
// constexpr bool all_unique_v<T, Rest...> = std::bool_constant<
//		(!std::is_same_v<T, Rest> && ...) && all_unique_v<Rest...>>{};


namespace detail {
template <size_t, size_t, class, class...>
struct idx_splice_impl;

// Found idx.
template <size_t TargetIdx, class BeforeTup, class T, class... Rest>
struct idx_splice_impl<TargetIdx, TargetIdx, BeforeTup, T, Rest...> {
	using before_tuple = BeforeTup;
	using type = T;
	using after_tuple = std::tuple<Rest...>;
};

template <size_t TargetIdx, size_t CurrentIdx, class BeforeTup, class T,
		class... Rest>
struct idx_splice_impl<TargetIdx, CurrentIdx, BeforeTup, T, Rest...>
		: idx_splice_impl<TargetIdx, CurrentIdx + 1,
				  fea::tuple_type_cat_t<BeforeTup, std::tuple<T>>, Rest...> {};

} // namespace detail

// Splice a parameter pack at index Idx.
// Finds the type at Idx, and stores the remaining parameters (parameters after
// splice point) in a tuple type.
template <size_t Idx, class... Args>
struct idx_splice : detail::idx_splice_impl<Idx, 0, std::tuple<>, Args...> {
	static_assert(
			Idx < sizeof...(Args), "fea::idx_splice : index out-of-range");
};

// Get the element type at index Idx in parameter pack.
template <size_t Idx, class... Args>
using idx_splice_t = typename idx_splice<Idx, Args...>::type;

// Get the elements before Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_before_t = typename idx_splice<Idx, Args...>::before_tuple;

// Get the elements after Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_after_t = typename idx_splice<Idx, Args...>::after_tuple;


// Get the first type in a parameter pack.
template <class... Args>
using first_t = idx_splice_t<0, Args...>;

// Get the last type in a parameter pack.
template <class... Args>
using last_t = idx_splice_t<sizeof...(Args) - 1, Args...>;


namespace detail {
// Used in is_detected.
template <class...>
using void_t = void;
} // namespace detail

/*
is_detected checks if a given type has function.
You must call this with a "detector alias", for example :

template <class T>
using has_myfunc = decltype(std::declval<T>().myfunc());

constexpr bool answer = is_detected_v<has_myfunc, your_class>;

More info : https://en.cppreference.com/w/cpp/experimental/is_detected

See unit tests for more examples.
*/
template <template <class...> class Op, class = void, class...>
struct is_detected : std::false_type {};
template <template <class...> class Op, class... Args>
struct is_detected<Op, detail::void_t<Op<Args...>>, Args...> : std::true_type {
};

template <template <class...> class Op, class... Args>
FEA_INLINE_VAR constexpr bool is_detected_v
		= is_detected<Op, void, Args...>::value;


/*
member_func_ptr is a trait which constructs a member function pointer, given Ret
and Args...

It uses the first argument of Args as the class type.
If no Args are provided, aliases void*.
*/

namespace detail {
template <class, class, bool, class...>
struct member_func_ptr {
	using type = void*;
};

template <class Ret, class T, class... Rest>
struct member_func_ptr<Ret, T, true, Rest...> {
	using type = Ret (T::*)(Rest...);
};

} // namespace detail

template <class, class...>
struct member_func_ptr {
	using type = void*;
};

template <class Ret, class T, class... Rest>
struct member_func_ptr<Ret, T*, Rest...>
		: detail::member_func_ptr<Ret, T, std::is_class<T>::value, Rest...> {};

template <class Ret, class... Args>
using member_func_ptr_t = typename member_func_ptr<Ret, Args...>::type;


namespace detail {
template <class Enum, class Func, size_t... Idx>
constexpr auto explode_enum(Func&& func, std::index_sequence<Idx...>) {
	return std::forward<Func>(func)(
			std::integral_constant<Enum, Enum(Idx)>{}...);
}
} // namespace detail

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// You must "extract" the non-type enum as it is passed by
// std::integral_constant, use ::value.
template <class Enum, size_t N, class Func>
constexpr auto explode_enum(Func&& func) {
	return detail::explode_enum<Enum>(
			std::forward<Func>(func), std::make_index_sequence<N>{});
}

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// Overload for enums that contain a 'count' member.
template <class Enum, class Func>
constexpr auto explode_enum(Func&& func) {
	return detail::explode_enum<Enum>(std::forward<Func>(func),
			std::make_index_sequence<size_t(Enum::count)>{});
}


// Index of element T in paramater pack.
template <class, class...>
struct pack_idx;
template <class T, class... Ts>
struct pack_idx<T, T, Ts...> : std::integral_constant<size_t, 0> {};
template <class T, class U, class... Ts>
struct pack_idx<T, U, Ts...>
		: std::integral_constant<size_t, 1 + pack_idx<T, Ts...>::value> {};

template <class T, class... Ts>
FEA_INLINE_VAR constexpr size_t pack_idx_v = pack_idx<T, Ts...>::value;

// Index of element T in non-type paramater pack.
template <class NT, NT, NT...>
struct non_type_pack_idx;
template <class NT, NT T, NT... Ts>
struct non_type_pack_idx<NT, T, T, Ts...> : std::integral_constant<size_t, 0> {
};
template <class NT, NT T, NT U, NT... Ts>
struct non_type_pack_idx<NT, T, U, Ts...>
		: std::integral_constant<size_t,
				  1 + non_type_pack_idx<NT, T, Ts...>::value> {};

template <class NT, NT T, NT... Ts>
FEA_INLINE_VAR constexpr size_t non_type_pack_idx_v
		= non_type_pack_idx<NT, T, Ts...>::value;


// Is the same non-type parameter.
template <class T, T T1, T T2>
struct non_type_is_same : std::false_type {};

template <class T, T T1>
struct non_type_is_same<T, T1, T1> : std::true_type {};

template <class T, T T1, T T2>
FEA_INLINE_VAR constexpr bool is_same_nt_v = non_type_is_same<T, T1, T2>::value;


// Holder for types.
template <class...>
struct type_pack {};
template <class T, T...>
struct non_type_type_pack {};
} // namespace fea
