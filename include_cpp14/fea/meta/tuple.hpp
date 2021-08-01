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
#include "fea/utils/unused.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
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


/*
// Doesn't compile on clang.
// Gets the tuple member offsetof at Idx.
template <size_t Idx, class Tuple>
uintptr_t tuple_offset() {
	return uintptr_t((const char*)(&std::get<Idx>(*(const Tuple*)0)));
}
// Gets the tuple member offsetof of type T.
template <class T, class Tuple>
uintptr_t tuple_offset() {
	return uintptr_t((const char*)(&std::get<T>(*(const Tuple*)0)));
}
*/

// Gets the tuple member offsetof at Idx.
template <size_t Idx, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup) {
	return uintptr_t((const char*)(&std::get<Idx>(tup)) - (const char*)&tup);
}
// Gets the tuple member offsetof of type T.
template <class T, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup) {
	return uintptr_t((const char*)(&std::get<T>(tup)) - (const char*)&tup);
}

// Get the tuple element at offset and cast to T.
template <class T, class... Args>
const T& offset_get(uintptr_t offset, const std::tuple<Args...>& tup) {
	return *((const T*)((const char*)&tup + offset));
}
// Get the tuple element at offset and cast to T.
template <class T, class... Args>
T& offset_get(uintptr_t offset, std::tuple<Args...>& tup) {
	const auto& c_tup = tup;
	return const_cast<T&>(offset_get(offset, c_tup));
}

// Creates an array of offsets for each member in tuple.
// Array index matches std::get position in tuple.
template <class... Args>
auto make_offset_lookup(const std::tuple<Args...>& tup) {
	constexpr size_t arr_size = sizeof...(Args);
	std::array<uintptr_t, arr_size> ret{};

	fea::static_for<arr_size>(
			[&](auto idx) { ret[idx] = fea::tuple_offset<idx>(tup); });

	return ret;
}

// Get an item in a tuple at runtime.
template <class... Args>
const void* runtime_get(size_t idx, const std::tuple<Args...>& tup) {
	static const std::array<uintptr_t, sizeof...(Args)> lookup
			= fea::make_offset_lookup(tup);
	return (const void*)((const char*)&tup + lookup[idx]);
}
// Get an item in a tuple at runtime.
template <class... Args>
void* runtime_get(size_t idx, std::tuple<Args...>& tup) {
	const auto& c_tup = tup;
	return const_cast<void*>(runtime_get(idx, c_tup));
}


#if FEA_CPP17
namespace detail {
// Gcc is unhappy calling the global static_for, try here instead.
template <class Func, size_t... I>
constexpr void gcc_static_for_imp(Func& func, std::index_sequence<I...>) {
	(func(std::integral_constant<size_t, I>{}), ...);
}

// Gcc is unhappy calling the global static_for, try here instead.
template <size_t N, class Func>
constexpr void gcc_static_for(Func&& func) {
	gcc_static_for_imp(func, std::make_index_sequence<N>{});
}

template <size_t Idx, class FuncRet, class Func, class TupleRef>
FuncRet unerase(Func& func, TupleRef tup) {
	return func(std::get<Idx>(tup));
}

template <class FuncRet, class Func, class TupleRef>
constexpr auto make_lookup() {
	using unerase_t = FuncRet (*)(Func&, TupleRef);
	constexpr size_t tup_size = std::tuple_size_v<std::decay_t<TupleRef>>;

	std::array<unerase_t, tup_size> ret{};
	fea::static_for<tup_size>([&](auto idx) {
		ret[idx] = &unerase<idx, FuncRet, Func, TupleRef>;
	});
	return ret;
}
} // namespace detail


// Get a tuple value at runtime, located at idx.
// Provide a generic lambda which accepts const auto& to recieve the value.
// Your lambda may return a value, but it must be the same type for all
// specializations.
template <class Func, class Arg1, class... Args>
std::invoke_result_t<Func, const Arg1&> runtime_get(
		Func&& func, size_t idx, const std::tuple<Arg1, Args...>& tup) {

	using func_ret_t = std::invoke_result_t<Func, const Arg1&>;
	using tup_ref_t = const std::tuple<Arg1, Args...>&;

	static constexpr auto lookup
			= detail::make_lookup<func_ret_t, Func, tup_ref_t>();
	return lookup[idx](func, tup);
}

// Get a tuple value at runtime, located at idx.
// Provide a generic lambda which accepts auto& to recieve the value.
// Your lambda may return a value, but it must be the same type for all
// specializations.
template <class Func, class Arg1, class... Args>
std::invoke_result_t<Func, Arg1&> runtime_get(
		Func&& func, size_t idx, std::tuple<Arg1, Args...>& tup) {

	using func_ret_t = std::invoke_result_t<Func, Arg1&>;
	using tup_ref_t = std::tuple<Arg1, Args...>&;

	static constexpr auto lookup
			= detail::make_lookup<func_ret_t, Func, tup_ref_t>();
	return lookup[idx](func, tup);
}

#endif // CPP17
} // namespace fea
