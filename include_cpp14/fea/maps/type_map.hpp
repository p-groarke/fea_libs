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
#include "fea/meta/pack.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/utils/memory.hpp"


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
template <class... Values>
struct type_map_shared {
	using values_t = std::tuple<Values...>;

	constexpr type_map_shared() = default;
	constexpr type_map_shared(const std::tuple<Values...>& values)
			: _values(values) {
	}

	constexpr type_map_shared(std::tuple<Values...>&& values)
			: _values(std::move(values)) {
	}

	template <class Val>
	static constexpr bool contains_value() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = fea::any_of_v<std::is_same<Val, Values>...>;
		return ret;
	}


	// The data, a tuple of your values.
	constexpr const auto& data() const {
		return _values;
	}
	constexpr auto& data() {
		return _values;
	}

private:
	// Your values.
	values_t _values;
};
} // namespace detail

template <class, class...>
struct type_map;

// Typed type_map.
template <class... Keys, class... Values>
struct type_map<fea::pack<Keys...>, Values...>
		: detail::type_map_shared<Values...> {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"type_map : unequal number of keys and values");

	using pack_t = fea::pack<Keys...>;

	// Inherit ctors.
	using base_t = typename detail::type_map_shared<Values...>;
	using base_t::base_t;

	// Does map contain Key?
	template <class Key>
	static constexpr bool contains() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = pack_contains_v<Key, pack_t>;
		return ret;
	}

	// Search for value associated with key.
	template <class Key>
	constexpr const auto& find() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_v<Key, pack_t>;
		return std::get<idx>(base_t::data());
	}
	template <class Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_v<Key, pack_t>;
		return std::get<idx>(base_t::data());
	}
};

// Non-type type_map.
template <class T, T... Keys, class... Values>
struct type_map<fea::pack_nt<T, Keys...>, Values...>
		: detail::type_map_shared<Values...> {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"type_map : unequal number of keys and values");

	using pack_t = fea::pack_nt<T, Keys...>;

	// Inherit ctors.
	using base_t = typename detail::type_map_shared<Values...>;
	using base_t::base_t;

	// Does map contain non-type Key?
	template <T Key>
	static constexpr bool contains() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = pack_contains_nt_v<T, Key, pack_t>;
		return ret;
	}

	// Search for value associated with non-type key.
	template <T Key>
	constexpr const auto& find() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_nt_v<T, Key, pack_t>;
		return std::get<idx>(base_t::data());
	}
	template <T Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_nt_v<T, Key, pack_t>;
		return std::get<idx>(base_t::data());
	}
};

// Construct a type_map using a fea::pack and a std::tuple.
template <class... Keys, class... Values>
constexpr auto make_type_map(
		pack<Keys...>, const std::tuple<Values...>& values) {
	return type_map<pack<Keys...>, Values...>(values);
}

// Construct a non-type type_map using a fea::pack and a std::tuple.
template <class K, K... Keys, class... Values>
constexpr auto make_type_map(
		pack_nt<K, Keys...>, const std::tuple<Values...>& values) {
	return type_map<pack_nt<K, Keys...>, Values...>(values);
}

// kv_t is a holder for a type Key and Value v.
// The key has no storage.
// You can use it to construct a type_map as you would a std::map.
// See make_type_map.
template <class Key, class Value>
struct kv_t {
	using key_t = Key;
	using value_t = Value;

	kv_t(Value&& v)
			: v(std::forward<Value>(v)) {
	}

	// Deduce the type K.
	kv_t(Key&&, Value&& v)
			: v(std::forward<Value>(v)) {
	}

	Value v;
};

// Helper to deduce kv_t in c++ < 17.
template <class Key, class Value>
kv_t<Key, Value> make_kv_t(Key&&, Value&& v) {
	return kv_t<Key, Value>{ std::forward<Value>(v) };
}

// kv_t for non-types.
template <class K, K Key, class Value>
struct kv_nt {
	using key_t = K;
	using value_t = Value;

	kv_nt(Value&& v)
			: v(std::forward<Value>(v)) {
	}

	Value v;
};

// Helper to deduce kv_nt in c++ < 17
template <class K, K Key, class Value>
kv_nt<K, Key, Value> make_kv_nt(Value&& v) {
	return kv_nt<K, Key, Value>{ std::forward<Value>(v) };
}

#if FEA_CPP17
// Helper which makes it a little cleaner.
template <auto Key, class Value>
kv_nt<decltype(Key), Key, Value> make_kv_nt(Value&& v) {
	return kv_nt<decltype(Key), Key, Value>{ std::forward<Value>(v) };
}
#endif

// Construct a type_map using a list of key-value pairs.
template <class... Keys, class... Values>
constexpr auto make_type_map(kv_t<Keys, Values>&&... kvs) {
	return type_map<pack<Keys...>, Values...>(
			std::make_tuple(fea::maybe_move(kvs.v)...));
}

// Construct a non-type type_map using a list of key-value pairs.
template <class K, K... Keys, class... Values>
constexpr auto make_type_map(kv_nt<K, Keys, Values>&&... kvs) {
	return type_map<pack_nt<K, Keys...>, Values...>(
			std::make_tuple(fea::maybe_move(kvs.v)...));
}
} // namespace fea
