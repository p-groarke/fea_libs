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
#include "fea/memory/memory.hpp"
#include "fea/meta/pack.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/utils/platform.hpp"

#include <array>

/*
fea::type_map stores items which can be accessed using types. The types must be
unique.

Create it using a fea::type_pack and a tuple.
The former is for your keys and the later for your values.
The indexes of keys will reference the object at the same index in your tuple.

You can use kv_t and make_type_map to construct a type_map as you would c++
maps.
*/

namespace fea {
namespace detail {
template <class...>
struct type_map_base;
}
template <class, class...>
struct type_map;

// Typed type_map.
template <class... Keys, class... Values>
struct type_map<fea::pack<Keys...>, Values...>
		: public detail::type_map_base<Values...> {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"type_map : unequal number of keys and values");

	using pack_t = fea::pack<Keys...>;

	// Inherit ctors.
	using base_t = typename detail::type_map_base<Values...>;
	using base_t::base_t;

	// Does map contain Key?
	template <class Key>
	static constexpr bool contains();

	// Search for value associated with key.
	template <class Key>
	constexpr const auto& find() const;

	// Search for value associated with key.
	template <class Key>
	constexpr auto& find();

	// Return the key's index.
	template <class Key>
	constexpr size_t idx() const;

	// Loops on all elements of map.
	// Passes (Key*, const auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) const;

	// Loops on all elements of map.
	// Passes (Key*, auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func);
};


// Non-type type_map.
template <auto... Keys, class... Values>
struct type_map<fea::pack_nt<Keys...>, Values...>
		: detail::type_map_base<Values...> {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"type_map : unequal number of keys and values");

	using pack_t = fea::pack_nt<Keys...>;

	// Inherit ctors.
	using base_t = typename detail::type_map_base<Values...>;
	using base_t::base_t;

	// Does map contain non-type Key?
	template <auto Key>
	static constexpr bool contains();

	// Search for value associated with non-type key.
	template <auto Key>
	constexpr const auto& find() const;

	// Search for value associated with non-type key.
	template <auto Key>
	constexpr auto& find();

	// Return the key's index.
	template <auto Key>
	constexpr size_t idx() const;

	// Loops on all elements of map.
	// Passes (std::integral_constant<key> key, const auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) const;

	// Loops on all elements of map.
	// Passes (std::integral_constant<key> key, auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func);
};

// Construct a type_map using a fea::pack and a std::tuple.
template <class... Keys, class... Values>
constexpr auto make_type_map(
		pack<Keys...>, const std::tuple<Values...>& values);

// Construct a non-type type_map using a fea::pack and a std::tuple.
template <auto... Keys, class... Values>
constexpr auto make_type_map(
		pack_nt<Keys...>, const std::tuple<Values...>& values);

// Construct a type_map using a fea::pack and a std::array.
template <class... Keys, class T, size_t N>
constexpr auto make_type_map(pack<Keys...>, const std::array<T, N>& values);

// Construct a non-type type_map using a fea::pack and a std::array.
template <auto... Keys, class T, size_t N>
constexpr auto make_type_map(pack_nt<Keys...>, const std::array<T, N>& values);

// kv_t is a holder for a type Key and Value v.
// The key has no storage.
// You can use it to construct a type_map as you would a std::map.
// See make_type_map.
template <class Key, class Value>
struct kv_t {
	using key_t = Key;
	using value_t = Value;

	kv_t(Value&& v);

	// Deduce the type K.
	kv_t(Key&&, Value&& v);

	Value v;
};


// kv_t for non-types.
template <auto Key, class Value>
struct kv_nt {
	using key_t = decltype(Key);
	using value_t = Value;

	kv_nt(Value&& v);

	Value v;
};

// Helper which makes it a little cleaner.
template <auto Key, class Value>
kv_nt<Key, Value> make_kv_nt(Value&& v);

// Construct a type_map using a list of key-value pairs.
template <class... Keys, class... Values>
constexpr auto make_type_map(kv_t<Keys, Values>&&... kvs);

// Construct a non-type type_map using a list of key-value pairs.
template <auto... Keys, class... Values>
constexpr auto make_type_map(kv_nt<Keys, Values>&&... kvs);


/**
 * External Helpers
 */

// Get a mapped value at runtime.
template <class Func, class Key, Key... Keys, class... Values>
decltype(auto) runtime_get(Func&& func, Key e,
		const fea::type_map<fea::pack_nt<Keys...>, Values...>& t_map);

// Get a mapped value at runtime.
template <class Func, class Key, Key... Keys, class... Values>
decltype(auto) runtime_get(Func&& func, Key e,
		fea::type_map<fea::pack_nt<Keys...>, Values...>& t_map);
} // namespace fea

#include "imp/type_map.imp.hpp"
