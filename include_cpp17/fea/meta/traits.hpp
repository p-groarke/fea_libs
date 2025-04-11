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
#include "fea/utils/platform.hpp"

#include <array> // for std::begin, std::end
#include <cstdint>
#include <memory>
#include <tuple> // for std::get
#include <type_traits>
#include <utility>

namespace fea {
// Checks if the all the passed in traits are true.
template <class...>
struct all_of : std::true_type {};

template <class Trait, class... Traits>
struct all_of<Trait, Traits...>
		: std::conditional_t<Trait::value, all_of<Traits...>, std::false_type> {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::all_of : must use type traits");
};

template <class... Traits>
inline constexpr bool all_of_v = all_of<Traits...>::value;


// Checks if none of the passed in traits are true.
template <class...>
struct none_of : std::true_type {};

template <class Trait, class... Traits>
struct none_of<Trait, Traits...>
		: std::conditional_t<Trait::value, std::false_type,
				  none_of<Traits...>> {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::none_of : must use type traits");
};

template <class... Traits>
inline constexpr bool none_of_v = none_of<Traits...>::value;


// Checks if any of the passed in traits are true.
template <class...>
struct any_of : std::false_type {};

template <class Trait, class... Traits>
struct any_of<Trait, Traits...>
		: std::conditional_t<Trait::value, std::true_type, any_of<Traits...>> {
	// Check that provided templates are type_traits. Better way?
	static_assert(Trait::value == true || Trait::value == false,
			"fea::any_of : must use type traits");
};

template <class... Traits>
inline constexpr bool any_of_v = any_of<Traits...>::value;

// Checks if one of the passed in traits is true.
template <class...>
struct one_of : std::false_type {};

namespace detail {
template <class...>
struct one_of_found : std::true_type {};

template <class Trait, class... Traits>
struct one_of_found<Trait, Traits...>
		: std::conditional_t<Trait::value, std::false_type,
				  one_of_found<Traits...>> {};
} // namespace detail

template <class Trait, class... Traits>
struct one_of<Trait, Traits...>
		: std::conditional_t<Trait::value, detail::one_of_found<Traits...>,
				  one_of<Traits...>> {};

// Checks if one of the passed in traits is true.
template <class... Traits>
inline constexpr bool one_of_v = one_of<Traits...>::value;

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
template <class Default, class AlwaysVoid, template <class...> class Op,
		class... Args>
struct detector {
	using value_t = std::false_type;
	using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
	using value_t = std::true_type;
	using type = Op<Args...>;
};

struct nonesuch {
	~nonesuch() = delete;
	nonesuch(nonesuch const&) = delete;
	void operator=(nonesuch const&) = delete;
};
} // namespace detail

template <template <class...> class Op, class... Args>
using is_detected =
		typename detail::detector<detail::nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t =
		typename detail::detector<detail::nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template <template <class...> class Op, class... Args>
constexpr inline bool is_detected_v = is_detected<Op, Args...>::value;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;


// Is the same non-type parameter.
template <class T, T T1, T T2>
struct is_same_nt : std::false_type {};

template <class T, T T1>
struct is_same_nt<T, T1, T1> : std::true_type {};

template <class T, T T1, T T2>
inline constexpr bool is_same_nt_v = is_same_nt<T, T1, T2>::value;

template <auto T1, auto T2>
inline constexpr bool is_same_nt_v2 = is_same_nt<decltype(T1), T1, T2>::value;


// Checks whether 2 types are the same, regardless of their template params.
template <template <class...> class, template <class...> class>
struct is_same_template : std::false_type {};

template <template <class...> class T>
struct is_same_template<T, T> : std::true_type {};

// Checks whether 2 types are the same, regardless of their template params.
template <template <class...> class T, template <class...> class U>
inline constexpr bool is_same_template_v = is_same_template<T, U>::value;


template <class T>
struct is_template_template : std::false_type {};

template <template <class...> class T, class... Args>
struct is_template_template<T<Args...>> : std::true_type {};

// Checks if a type is a template template.
template <class T>
inline constexpr bool is_template_template_v = is_template_template<T>::value;


// Removes the internal consts of a template template (like a tuple for ex).
template <class T>
struct remove_nested_const;

template <class... Args, template <class...> class T>
struct remove_nested_const<T<Args...>> {
	using type = T<std::remove_const_t<Args>...>;
};

template <class T>
using remove_nested_const_t = typename remove_nested_const<T>::type;

// Checks if first type of template template is const.
template <class T>
struct is_first_const;

template <class First, class... Args, template <class, class...> class T>
struct is_first_const<T<First, Args...>> {
	static constexpr bool value = std::is_const_v<First>;
};

template <class T>
inline constexpr bool is_first_const_v = is_first_const<T>::value;

// Checks if a type is a pair.
template <class T>
struct is_pair : std::false_type {};

template <class U1, class U2>
struct is_pair<std::pair<U1, U2>> : std::true_type {};

template <class T>
inline constexpr bool is_pair_v = is_pair<T>::value;

// Iterator helper.
template <class T>
using iterator_category_t = typename std::iterator_traits<T>::iterator_category;

// Checks if a type is an iterator.
template <class T, class = void>
struct is_iterator {
	static constexpr bool value = false;
};
template <class T>
struct is_iterator<T, std::void_t<iterator_category_t<T>>> {
	static constexpr bool value = true;
};

template <class T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;


/**
 * Useful is_detected checkers
 */
template <class T>
using has_begin = decltype(std::begin(std::declval<T>()));
template <class T>
using has_end = decltype(std::end(std::declval<T>()));
template <class T>
using has_get = decltype(std::get<0>(std::declval<T>()));
template <class T>
using has_resize = decltype(std::declval<T>().resize(std::declval<size_t>()));
template <class T>
using has_reserve = decltype(std::declval<T>().reserve(std::declval<size_t>()));

template <class T>
using has_data = decltype(std::data(std::declval<T>()));
template <class T>
using has_size = decltype(std::size(std::declval<T>()));

template <class T>
using has_value_type = typename T::value_type;


// Checks if a type has std::begin and std::end.
template <class T>
inline constexpr bool is_container_v
		= fea::is_detected_v<has_begin, T> && fea::is_detected_v<has_end, T>;

// Checks if a type has std::get.
template <class T>
inline constexpr bool is_tuple_like_v = fea::is_detected_v<has_get, T>;

// Checks if a type has std::data and std::size.
template <class T>
inline constexpr bool is_contiguous_v
		= fea::is_detected_v<has_data, T> && fea::is_detected_v<has_size, T>;


/**
 * First and last types in a parameter pack.
 */

namespace detail {
template <class...>
struct front;

template <>
struct front<> {
	using type = void;
};

template <class T, class... Args>
struct front<T, Args...> {
	using type = T;
};

} // namespace detail

// Get the first type of a pack.
template <class... Args>
using front_t = typename detail::front<Args...>::type;

namespace detail {
template <class...>
struct back;

template <class T>
struct back<T> {
	using type = T;
};
template <class T, class... Args>
struct back<T, Args...> {
	using type = typename back<Args...>::type;
};

template <>
struct back<> {
	using type = void;
};
} // namespace detail

// Get the last type of a pack.
template <class... Args>
using back_t = typename detail::back<Args...>::type;

// https://stackoverflow.com/questions/23762224/check-if-primitive-types-are-castable-in-c
template <class From, class To, class = To>
struct is_static_castable : std::false_type {};

template <class From, class To>
struct is_static_castable<From, To,
		decltype(static_cast<To>(std::declval<From>()))> : std::true_type {};

// Checks whether a type can be static_casted to another.
template <class From, class To>
inline constexpr bool is_static_castable_v
		= is_static_castable<From, To>::value;


// Checks whether a function callback type is noexcept.
template <class Func, class... Args>
inline constexpr bool is_noexcept_v
		= noexcept(std::declval<Func>()(std::declval<Args>()...));

namespace detail {
template <class...>
struct reverse;

template <class T, class... Ts>
struct reverse<T, Ts...> {
	using type = decltype(std::tuple_cat(
			typename reverse<Ts...>::type{}, std::tuple<T>{}));
};

template <class T>
struct reverse<T> {
	using type = std::tuple<T>;
};
} // namespace detail

// Declares a type std::tuple<ReversedTs...> which is the reverse of your
// variadic arguments.
template <class... Ts>
using reverse_t = typename detail::reverse<Ts...>::type;

namespace detail {
// https://stackoverflow.com/questions/51408771/c-reversed-integer-sequence-implementation
template <size_t... Is>
constexpr auto reverse_index_sequence(std::index_sequence<Is...>)
		-> decltype(std::index_sequence<sizeof...(Is) - 1U - Is...>{});
} // namespace detail

// Creates a reverse index sequence of size N.
template <size_t N>
using make_reverse_index_sequence = decltype(detail::reverse_index_sequence(
		std::make_index_sequence<N>{}));

// Get the allocator_traits rebind_alloc type.
template <class Alloc, class Value>
using rebind_alloc_t =
		typename std::allocator_traits<Alloc>::template rebind_alloc<Value>;

// Get the iterator_traits value_type type.
template <class Iter>
using iterator_value_t = typename std::iterator_traits<Iter>::value_type;


// Get aligned raw storage for a type.
template <size_t Bytes, size_t Align>
struct aligned_storage {
	struct type {
		alignas(Align) uint8_t data[Bytes];
	};
};

// Helper alias.
template <size_t Bytes, size_t Align>
using aligned_storage_t = typename aligned_storage<Bytes, Align>::type;


// Get the output iterator's container value_type.
template <class>
struct output_iterator_traits;

template <template <class, class...> class OutputIt, class First, class... Args>
struct output_iterator_traits<OutputIt<First, Args...>> {
	using traits_t = std::iterator_traits<OutputIt<First, Args...>>;
	static_assert(std::is_base_of_v<std::output_iterator_tag,
						  typename traits_t::iterator_category>,
			"output_iterator_traits : Invalid iterator, requires output "
			"iterator.");

	using value_type = detected_or_t<First, has_value_type, First>;
};

template <class T>
using output_iterator_vt = typename output_iterator_traits<T>::value_type;
} // namespace fea
