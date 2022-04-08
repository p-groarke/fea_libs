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
#include "fea/enum/enum_array.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"

#include <array>

/*
Descriptors are data-driven, compile time configurators.

They are stored in a queriable map and used to centralize your architecture
elements. Descriptors help reduce copy-paste and improve readability. This
header implements commonly used descriptor helpers.

descriptor_map
A compile-time map of descriptors. Use it to apply static_asserts and create
arrays. When adding descriptors to the descriptor_map, all descriptors must
contain a static constexpr enum/integer "key" or "Key". The map checks if your
descriptor order matches its key.
*/

namespace fea {
namespace detail {
template <class T>
using has_lower_case_key = decltype(std::declval<T>().key);
template <class T>
using has_upper_case_key = decltype(std::declval<T>().Key);
} // namespace detail

/*
A helper map which stores all your descriptors, runs a multitude of
static_asserts and facilitates creating arrays from your descriptor members.
Provide your key type, it must be castable to size_t.
Provide your descriptors, ordered in the same order as their key (indexed at the
key value).
*/
template <class KeyT, class... Descriptors>
struct descriptor_map {
	// Do the descriptors use capitalized Key var, or lower case key.
	static constexpr bool upper_case_key
			= fea::is_detected_v<detail::has_upper_case_key,
					fea::front_t<Descriptors...>>;

	// A tuple of all descriptors.
	using desc_tup_t = std::tuple<Descriptors...>;

	// Descriptor count.
	static constexpr size_t size = sizeof...(Descriptors);

	// The type of key. Must be castable to size_t.
	using key_t = KeyT;


	// Get a specific descriptor.
	template <KeyT K>
	[[nodiscard]] static constexpr auto desriptor() {
		return std::tuple_element_t<size_t(K), desc_tup_t>{};
	}

	// Get a specific descriptor's key.
	template <size_t I>
	[[nodiscard]] static constexpr auto key() {
		if constexpr (!upper_case_key) {
			return std::tuple_element_t<I, desc_tup_t>::key;
		} else {
			return std::tuple_element_t<I, desc_tup_t>::Key;
		}
	}

	// Returns an array filled with your operation result, indexed at key.
	// Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]] static constexpr auto make_array(Func&& func) {
		using ret_t = decltype(func(fea::front_t<Descriptors...>{}));
		std::array<ret_t, size> ret;

		fea::static_for<size>([&](auto const_i) {
			constexpr size_t i = const_i;
			using desc_t = std::tuple_element_t<i, desc_tup_t>;
			ret[i] = func(desc_t{});
		});
		return ret;
	}

	// Returns an fea::enum_array filled with your operation result, indexed at
	// enum key. Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]] static constexpr auto make_enum_array(Func&& func) {
		using ret_t = decltype(func(fea::front_t<Descriptors...>{}));
		fea::enum_array<ret_t, key_t, size> ret;

		fea::static_for<size>([&](auto const_i) {
			constexpr size_t i = const_i;
			using desc_t = std::tuple_element_t<i, desc_tup_t>;
			ret[key_t(i)] = func(desc_t{});
		});
		return ret;
	}

	// Calls your func for each descriptor and passes a type_wrapper of its
	// type. Get type with : using desc_t = typename decltype(bla)::type;
	template <class Func>
	static constexpr void for_each_descriptor(Func&& func) {
		fea::static_for<size>([&](auto const_i) {
			constexpr size_t i = const_i;
			using trait_t = std::tuple_element_t<i, desc_tup_t>;
			func(trait_t{});
		});
	}

private:
	[[nodiscard]] static constexpr bool has_valid_keys() {
		bool ret = true;

		constexpr bool convertible = fea::is_static_castable_v<KeyT, size_t>;
		static_assert(convertible,
				"descriptor_map : descriptor key must be convertible "
				"to size_t");

		ret &= convertible;

		fea::static_for<std::tuple_size_v<desc_tup_t>>([&](auto const_i) {
			constexpr size_t i = const_i;
			using trait_t = std::tuple_element_t<i, desc_tup_t>;

			if constexpr (!upper_case_key) {
				// Every descriptor has key.
				constexpr bool has_key
						= fea::is_detected_v<detail::has_lower_case_key,
								trait_t>;
				static_assert(has_key,
						"descriptor_map : all descriptors must have the member "
						"variable 'key'");

				ret &= has_key;
			} else {
				constexpr bool has_key
						= fea::is_detected_v<detail::has_upper_case_key,
								trait_t>;
				static_assert(has_key,
						"descriptor_map : all descriptors must have the "
						"member variable 'Key'");

				ret &= has_key;
			}

			// Key is correct type.
			constexpr auto k = key<i>();
			using key_type = std::decay_t<decltype(k)>;
			constexpr bool same_key_t = std::is_same_v<key_type, KeyT>;
			static_assert(same_key_t,
					"descriptor_map : descriptor keys must all be of type "
					"KeyT");

			// Every descriptor ordered at its key index.
			constexpr bool ordered_correctly = i == size_t(k);
			static_assert(ordered_correctly,
					"descriptor_map : descriptors must be ordered "
					"in the same order as their 'key' index");

			ret &= ordered_correctly & same_key_t;
		});

		return ret;
	}

	static_assert(has_valid_keys(),
			"descriptor_map : some descriptors are ordered incorrectly, or "
			"missing the '[Kk]ey' member, or '[Kk]ey' is not castable to "
			"size_t");
};
} // namespace fea
