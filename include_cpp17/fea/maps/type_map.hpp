/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
template <class Func, class T>
using tm_has_imp = decltype(std::declval<Func>()(std::declval<T>()));

template <size_t I, class TMap, class Func>
decltype(auto) tm_unerase(TMap& tmap, Func&& func) {
	return std::forward<Func>(func)(tmap.template at<I>());
}

// WIP : Use id_detected idiom to filter out unavailable
// instantiations and move the problem to runtime.
// template <class FuncPtrT, size_t I, class Func, class TMap>
// constexpr FuncPtrT tm_filter_uncallable() {
//	using called_t = decltype(std::declval<TMap>().at<I>());
//	// using called_t = std::tuple_element_t<I, std::decay_t<TMap>::values_t>;
//
//	if constexpr (fea::is_detected_v<tm_has_imp, Func, called_t>) {
//		return &tm_unerase<I, TMap, Func>;
//	} else {
//		return nullptr;
//	}
//}

template <class Func, class TMap, size_t... Is>
constexpr decltype(auto) tm_unerase_lookup(std::index_sequence<Is...>) {
	using func_t = std::common_type_t<decltype(&tm_unerase<Is, TMap, Func>)...>;
	return std::array<func_t, sizeof...(Is)>{ &tm_unerase<Is, TMap, Func>... };

	// WIP
	// using func_t = decltype(&tm_unerase<0, TMap, Func>);
	// return std::array<func_t, sizeof...(Is)>{
	//	tm_filter_uncallable<func_t, Is, Func, TMap>()...
	//};
}

template <class... Values>
struct type_map_base {
	using values_t = std::tuple<Values...>;
	using front_t = fea::front_t<Values...>;
	using back_t = fea::back_t<Values...>;

	constexpr type_map_base() = default;
	constexpr type_map_base(const std::tuple<Values...>& values)
			: _values(values) {
	}

	constexpr type_map_base(std::tuple<Values...>&& values)
			: _values(std::move(values)) {
	}

	template <class Val>
	static constexpr bool contains_value() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = fea::any_of_v<std::is_same<Val, Values>...>;
		return ret;
	}

	// Get value at index.
	template <size_t Idx>
	constexpr const auto& at() const {
		static_assert(Idx < size(), "type_map : index out-of-range");
		return std::get<Idx>(data());
	}

	// Get value at index.
	template <size_t Idx>
	constexpr auto& at() {
		static_assert(Idx < size(), "type_map : index out-of-range");
		return std::get<Idx>(data());
	}

	// Get value at runtime index.
	template <class Func>
	constexpr decltype(auto) at(size_t idx, Func&& func) const {
		// Unerase lookup (switch-case equivalent).
		// auto getit = [&, this](auto const_i) -> decltype(auto) {
		//	constexpr size_t i = const_i;
		//	return std::forward<Func>(func)(at<i>());
		//};

		constexpr auto lookup
				= detail::tm_unerase_lookup<Func, decltype(*this)>(
						std::make_index_sequence<size()>{});
		return lookup[idx](*this, std::forward<Func>(func));
	}

	// Get value at runtime index.
	template <class Func>
	constexpr decltype(auto) at(size_t idx, Func&& func) {
		//// Unerase lookup (switch-case equivalent).
		// auto getit = [&, this](auto const_i) -> decltype(auto) {
		//	constexpr size_t i = const_i;
		//	return std::forward<Func>(func)(at<i>());
		//};
		constexpr auto lookup
				= detail::tm_unerase_lookup<Func, decltype(*this)>(
						std::make_index_sequence<size()>{});
		return lookup[idx](*this, std::forward<Func>(func));
	}


	// The data, a tuple of your values.
	constexpr const auto& data() const {
		return _values;
	}
	constexpr auto& data() {
		return _values;
	}

	static constexpr size_t size() {
		return std::tuple_size_v<values_t>;
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
		: public detail::type_map_base<Values...> {
	static_assert(sizeof...(Keys) == sizeof...(Values),
			"type_map : unequal number of keys and values");

	using pack_t = fea::pack<Keys...>;

	// Inherit ctors.
	using base_t = typename detail::type_map_base<Values...>;
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
	// Search for value associated with key.
	template <class Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_v<Key, pack_t>;
		return std::get<idx>(base_t::data());
	}

	// Return the key's index.
	template <class Key>
	constexpr size_t idx() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		return pack_idx_v<Key, pack_t>;
	}

	// Loops on all elements of map.
	// Passes (Key*, const auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) const {
		fea::pack_for_each(
				[&, this](auto* ptr) {
					using K = std::remove_pointer_t<decltype(ptr)>;
					func(ptr, this->template find<K>());
				},
				fea::pack<Keys...>{});
	}
	// Loops on all elements of map.
	// Passes (Key*, auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) {
		fea::pack_for_each(
				[&, this](auto* ptr) {
					using K = std::remove_pointer_t<decltype(ptr)>;
					func(ptr, this->template find<K>());
				},
				fea::pack<Keys...>{});
	}
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
	static constexpr bool contains() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = pack_contains_nt_v<Key, pack_t>;
		return ret;
	}

	// Search for value associated with non-type key.
	template <auto Key>
	constexpr const auto& find() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_nt_v<Key, pack_t>;
		return std::get<idx>(base_t::data());
	}
	// Search for value associated with non-type key.
	template <auto Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_nt_v<Key, pack_t>;
		return std::get<idx>(base_t::data());
	}

	// Return the key's index.
	template <auto Key>
	constexpr size_t idx() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		return pack_idx_nt_v<Key, pack_t>;
	}

	// Loops on all elements of map.
	// Passes (std::integral_constant<key> key, const auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) const {
		fea::pack_for_each(
				[&, this](
						auto key) { func(key, this->template find<key()>()); },
				fea::pack_nt<Keys...>{});
	}
	// Loops on all elements of map.
	// Passes (std::integral_constant<key> key, auto& val) to user
	// function.
	template <class Func>
	constexpr void for_each(Func&& func) {
		fea::pack_for_each(
				[&, this](
						auto key) { func(key, this->template find<key()>()); },
				fea::pack_nt<Keys...>{});
	}
};

// Construct a type_map using a fea::pack and a std::tuple.
template <class... Keys, class... Values>
constexpr auto make_type_map(
		pack<Keys...>, const std::tuple<Values...>& values) {
	return type_map<pack<Keys...>, Values...>(values);
}

// Construct a non-type type_map using a fea::pack and a std::tuple.
template <auto... Keys, class... Values>
constexpr auto make_type_map(
		pack_nt<Keys...>, const std::tuple<Values...>& values) {
	return type_map<pack_nt<Keys...>, Values...>(values);
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
kv_t<Key, Value> make_kv(Key&&, Value&& v) {
	return kv_t<Key, Value>{ std::forward<Value>(v) };
}

template <class Key, class Value>
kv_t<Key, Value> make_kv(Value&& v) {
	return kv_t<Key, Value>{ std::forward<Value>(v) };
}

// kv_t for non-types.
template <auto Key, class Value>
struct kv_nt {
	using key_t = decltype(Key);
	using value_t = Value;

	kv_nt(Value&& v)
			: v(std::forward<Value>(v)) {
	}

	Value v;
};

//// Helper to deduce kv_nt in c++ < 17
// template <class K, K Key, class Value>
// kv_nt<K, Key, Value> make_kv_nt(Value&& v) {
//	return kv_nt<K, Key, Value>{ std::forward<Value>(v) };
//}

// Helper which makes it a little cleaner.
template <auto Key, class Value>
kv_nt<Key, Value> make_kv_nt(Value&& v) {
	return kv_nt<Key, Value>{ std::forward<Value>(v) };
}

// Construct a type_map using a list of key-value pairs.
template <class... Keys, class... Values>
constexpr auto make_type_map(kv_t<Keys, Values>&&... kvs) {
	return type_map<pack<Keys...>, Values...>(
			std::make_tuple(fea::maybe_move(kvs.v)...));
}

// Construct a non-type type_map using a list of key-value pairs.
template <auto... Keys, class... Values>
constexpr auto make_type_map(kv_nt<Keys, Values>&&... kvs) {
	return type_map<pack_nt<Keys...>, Values...>(
			std::make_tuple(fea::maybe_move(kvs.v)...));
}


/**
 * External Helpers
 */

// Get a mapped value at runtime.
template <class Func, class Key, Key... Keys, class Val1, class... Values>
std::invoke_result_t<Func, const Val1&> runtime_get(Func&& func, Key e,
		const type_map<fea::pack_nt<Keys...>, Val1, Values...>& t_map) {
	// First, get the associated index for the enum value.
	// The underlying enum value is not necessarily == position.
	size_t val_idx = fea::runtime_get_idx(e, fea::pack_nt<Keys...>{});
	return fea::runtime_get(std::forward<Func>(func), val_idx, t_map.data());
}

// Get a mapped value at runtime.
template <class Func, class Key, Key... Keys, class Val1, class... Values>
std::invoke_result_t<Func, Val1&> runtime_get(Func&& func, Key e,
		type_map<fea::pack_nt<Keys...>, Val1, Values...>& t_map) {
	// First, get the associated index for the enum value.
	// The underlying enum value is not necessarily == position.
	size_t val_idx = fea::runtime_get_idx(e, fea::pack_nt<Keys...>{});
	return fea::runtime_get(std::forward<Func>(func), val_idx, t_map.data());
}
} // namespace fea
