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
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/unused.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fea {
// Tuple foreach.
// Your lambda will be called with each tuple's elements.
// Provid lambda which accepts auto& or const auto&.
template <class Func, class Tuple>
constexpr void tuple_for_each(Func&& func, Tuple&& tup);

// Create a tuple using type T and count N.
template <class T, size_t N>
constexpr auto make_tuple_from_count();

// Create a tuple using type T, count N, initialized at provided value.
template <class T, size_t N>
constexpr auto make_tuple_from_count(const T& t);

// Gets the tuple member offsetof at Idx.
template <size_t Idx, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup);

// Gets the tuple member offsetof of type T.
template <class T, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup);

// Get the tuple element at offset and cast to T.
template <class T, class... Args>
const T& offset_get(uintptr_t offset, const std::tuple<Args...>& tup);

// Get the tuple element at offset and cast to T.
template <class T, class... Args>
T& offset_get(uintptr_t offset, std::tuple<Args...>& tup);

// Creates an array of offsets for each member in tuple.
// Array index matches std::get position in tuple.
template <class... Args>
auto make_offset_lookup(const std::tuple<Args...>& tup);

// Get a tuple value at runtime, located at idx.
// Provide a generic lambda which accepts const auto& to recieve the value.
// Your lambda may return a value, but it must be the same type for all
// specializations.
template <class Func, class... Args>
decltype(auto) runtime_get(
		Func&& func, size_t idx, const std::tuple<Args...>& tup);

// Get a tuple value at runtime, located at idx.
// Provide a generic lambda which accepts auto& to recieve the value.
// Your lambda may return a value, but it must be the same type for all
// specializations.
template <class Func, class... Args>
decltype(auto) runtime_get(Func&& func, size_t idx, std::tuple<Args...>& tup);

// Get an item in a tuple at runtime.
template <class... Args>
const void* runtime_get(size_t idx, const std::tuple<Args...>& tup);

// Get an item in a tuple at runtime.
template <class... Args>
void* runtime_get(size_t idx, std::tuple<Args...>& tup);


// Get the index of type T in Tuple.
template <class, class>
struct tuple_idx;

// Get the index of type T in Tuple.
template <class T, class Tuple>
inline constexpr size_t tuple_idx_v = tuple_idx<T, Tuple>::value;

// Does Tuple contain type T?
template <class, class>
struct tuple_contains;

// Does Tuple contain type T?
template <class T, class Tuple>
inline constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;

// tuple_cats 2 tuple types together.
template <class, class>
struct tuple_type_cat;
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class Func, class Tuple, size_t... I>
constexpr void tuple_foreach(
		Func&& func, Tuple&& tup, std::index_sequence<I...>) {
	(func(std::get<I>(tup)), ...);
}

template <size_t, class T>
using accept_idx = T;

template <class T, size_t... Is>
constexpr auto make_tuple_from_count(std::index_sequence<Is...>) {
	return std::tuple<accept_idx<Is, T>...>{};
}

template <size_t Idx, class FuncRet, class Func, class TupleRef>
FuncRet unerase(Func& func, TupleRef tup) {
	return func(std::get<Idx>(tup));
}

template <class FuncRet, class Func, class TupleRef, size_t... Is>
constexpr auto make_rt_lookup(std::index_sequence<Is...>) {
	// using unerase_t = FuncRet (*)(Func&, TupleRef);
	using unerase_t = std::common_type_t<
			decltype(&unerase<Is, FuncRet, Func, TupleRef>)...>;

	std::array<unerase_t, sizeof...(Is)> ret{
		&unerase<Is, FuncRet, Func, TupleRef>...
	};
	return ret;
}
} // namespace detail


template <class Func, class Tuple>
constexpr void tuple_for_each(Func&& func, Tuple&& tup) {
	detail::tuple_foreach(std::forward<Func>(func), std::forward<Tuple>(tup),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

template <class T, size_t N>
constexpr auto make_tuple_from_count() {
	return detail::make_tuple_from_count<T>(std::make_index_sequence<N>{});
}

template <class T, size_t N>
constexpr auto make_tuple_from_count(const T& t) {
	auto ret = detail::make_tuple_from_count<T>(std::make_index_sequence<N>{});
	fea::tuple_for_each([&](auto& val) { val = t; }, ret);
	return ret;
}

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

template <size_t Idx, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup) {
	return uintptr_t((const char*)(&std::get<Idx>(tup)) - (const char*)&tup);
}

template <class T, class... Args>
uintptr_t tuple_offset(const std::tuple<Args...>& tup) {
	return uintptr_t((const char*)(&std::get<T>(tup)) - (const char*)&tup);
}

template <class T, class... Args>
const T& offset_get(uintptr_t offset, const std::tuple<Args...>& tup) {
	return *((const T*)((const char*)&tup + offset));
}

template <class T, class... Args>
T& offset_get(uintptr_t offset, std::tuple<Args...>& tup) {
	const auto& c_tup = tup;
	return const_cast<T&>(offset_get(offset, c_tup));
}

template <class... Args>
auto make_offset_lookup(const std::tuple<Args...>& tup) {
	constexpr size_t arr_size = sizeof...(Args);
	std::array<uintptr_t, arr_size> ret{};

	fea::static_for<arr_size>(
			[&](auto idx) { ret[idx] = fea::tuple_offset<idx>(tup); });

	return ret;
}

template <class Func, class... Args>
decltype(auto) runtime_get(
		Func&& func, size_t idx, const std::tuple<Args...>& tup) {
	using func_ret_t = std::invoke_result_t<Func, const fea::front_t<Args...>&>;
	using tup_ref_t = const std::tuple<Args...>&;

	constexpr size_t tup_size = std::tuple_size_v<std::decay_t<tup_ref_t>>;
	constexpr auto lookup = detail::make_rt_lookup<func_ret_t, Func, tup_ref_t>(
			std::make_index_sequence<tup_size>{});

	return lookup[idx](func, tup);
}

template <class Func, class... Args>
decltype(auto) runtime_get(Func&& func, size_t idx, std::tuple<Args...>& tup) {
	using func_ret_t = std::invoke_result_t<Func, fea::front_t<Args...>&>;
	using tup_ref_t = std::tuple<Args...>&;

	constexpr size_t tup_size = std::tuple_size_v<std::decay_t<tup_ref_t>>;
	constexpr auto lookup = detail::make_rt_lookup<func_ret_t, Func, tup_ref_t>(
			std::make_index_sequence<tup_size>{});

	return lookup[idx](func, tup);
}

template <class... Args>
const void* runtime_get(size_t idx, const std::tuple<Args...>& tup) {
	static const std::array<uintptr_t, sizeof...(Args)> lookup
			= fea::make_offset_lookup(tup);
	return (const void*)((const char*)&tup + lookup[idx]);
}

template <class... Args>
void* runtime_get(size_t idx, std::tuple<Args...>& tup) {
	const auto& c_tup = tup;
	return const_cast<void*>(runtime_get(idx, c_tup));
}


template <class T, class Tuple>
struct tuple_idx {
	static_assert(!std::is_same_v<Tuple, std::tuple<>>,
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

template <class T>
struct tuple_contains<T, std::tuple<>> : std::false_type {};

template <class T, class U, class... Ts>
struct tuple_contains<T, std::tuple<U, Ts...>>
		: tuple_contains<T, std::tuple<Ts...>> {};

template <class T, class... Ts>
struct tuple_contains<T, std::tuple<T, Ts...>> : std::true_type {};

template <class... First, class... Second>
struct tuple_type_cat<std::tuple<First...>, std::tuple<Second...>> {
	using type = std::tuple<First..., Second...>;
};

template <class... Args>
using tuple_type_cat_t = typename tuple_type_cat<Args...>::type;
} // namespace fea
