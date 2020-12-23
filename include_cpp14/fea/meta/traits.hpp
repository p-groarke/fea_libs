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


/*
is_detected checks if a given type has function.
You must call this with a "detector alias", for example :

template <class T>
using has_myfunc = decltype(std::declval<T>().myfunc());

constexpr bool answer = is_detected_v<has_myfunc, your_class>;

More info : https://en.cppreference.com/w/cpp/experimental/is_detected

See unit tests for more examples.
*/

namespace detail {
// Used in is_detected.
template <class...>
using void_t = void;
} // namespace detail

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

} // namespace fea
