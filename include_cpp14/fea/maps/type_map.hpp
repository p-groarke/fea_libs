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
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/meta/type_pack.hpp"

/*
fea::type_map stores items which can be accessed using types. The types must be
unique.

Create it using a fea::type_pack and a tuple.
The former is for your keys and the later for your values.
The indexes of keys will reference the object at the same index in your tuple.

*/

namespace fea {
template <class, class>
struct type_map;

template <class... Keys, class... Values>
struct type_map<type_pack<Keys...>, std::tuple<Values...>> {
	using values_t = std::tuple<Values...>;

	constexpr type_map(type_pack<Keys...>, const std::tuple<Values...>& values)
			: _values(values) {
		static_assert(sizeof...(Keys) == sizeof...(Values),
				"type_map : unequal number of keys and values");
	}
	constexpr type_map(type_pack<Keys...>, std::tuple<Values...>&& values)
			: _values(std::move(values)) {
		static_assert(sizeof...(Keys) == sizeof...(Values),
				"type_map : unequal number of keys and values");
	}

	template <class Key>
	static constexpr bool contains() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = any_of_v<std::is_same<Key, Keys>...>;
		return ret;
	}
	template <class Val>
	static constexpr bool contains_value() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = any_of_v<std::is_same<Val, Values>...>;
		return ret;
	}

	// Search by non-type template.
	template <class Key>
	constexpr const auto& find() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_v<Key, Keys...>;
		return std::get<idx>(_values);
	}
	template <class Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = pack_idx_v<Key, Keys...>;
		return std::get<idx>(_values);
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

template <class T, T... Keys, class... Values>
struct type_map<non_type_type_pack<T, Keys...>, std::tuple<Values...>> {
	using values_t = std::tuple<Values...>;

	constexpr type_map(
			non_type_type_pack<T, Keys...>, const std::tuple<Values...>& values)
			: _values(values) {
		static_assert(sizeof...(Keys) == sizeof...(Values),
				"type_map : unequal number of keys and values");
	}
	constexpr type_map(
			non_type_type_pack<T, Keys...>, std::tuple<Values...>&& values)
			: _values(std::move(values)) {
		static_assert(sizeof...(Keys) == sizeof...(Values),
				"type_map : unequal number of keys and values");
	}

	template <T Key>
	static constexpr bool contains() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = any_of_v<fea::non_type_is_same<T, Key, Keys>...>;
		return ret;
	}
	template <class Val>
	static constexpr bool contains_value() {
		// Just to make sure we are in constexpr land.
		constexpr bool ret = any_of_v<std::is_same<Val, Values>...>;
		return ret;
	}

	// Search by non-type template.
	template <T Key>
	constexpr const auto& find() const {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = non_type_pack_idx_v<T, Key, Keys...>;
		return std::get<idx>(_values);
	}
	template <T Key>
	constexpr auto& find() {
		static_assert(
				contains<Key>(), "type_map : doesn't contain requested key");

		constexpr size_t idx = non_type_pack_idx_v<T, Key, Keys...>;
		return std::get<idx>(_values);
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

template <class... Keys, class... Values>
constexpr auto make_type_map(
		type_pack<Keys...> keys, const std::tuple<Values...>& values) {
	return type_map<type_pack<Keys...>, std::tuple<Values...>>(keys, values);
}

template <class K, K... Keys, class... Values>
constexpr auto make_type_map(non_type_type_pack<K, Keys...> keys,
		const std::tuple<Values...>& values) {
	return type_map<non_type_type_pack<K, Keys...>, std::tuple<Values...>>(
			keys, values);
}
} // namespace fea
