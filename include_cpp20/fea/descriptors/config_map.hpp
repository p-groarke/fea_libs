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
#include "fea/containers/enum_array.hpp"
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"

#include <array>
#include <tuple>


namespace fea {
namespace detail {
template <class T>
using has_config_key = decltype(std::declval<T>().key);
}

template <class KeyT, class DescT, DescT... Descriptors>
struct config_map {
	using key_t = KeyT;
	using descriptor_t = DescT;

	static constexpr fea::enum_array<DescT, KeyT> descriptors{ Descriptors... };

	// Number of descriptors.
	static constexpr size_t size() {
		return sizeof...(Descriptors);
	}

	// Get a specific descriptor.
	[[nodiscard]] static constexpr DescT descriptor(KeyT k) {
		return descriptors[k];
	}

	// Returns a tuple filled with your operation results, indexed at key.
	// Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]] static constexpr auto make_tuple(Func&& func) {
		return fea::apply_indexes<size()>([&](auto... idxes) {
			return std::make_tuple(
					func(descriptors[KeyT(decltype(idxes)::value)])...);
		});
	}

	// Returns an array filled with your operation result, indexed at key.
	// Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]] static constexpr auto make_array(Func&& func) {
		return fea::apply_indexes<size()>([&](auto... idxes) {
			return std::array{ func(
					descriptors[KeyT(decltype(idxes)::value)])... };
		});
	}

	// Returns an fea::enum_array filled with your operation result, indexed at
	// enum key. Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]] static constexpr auto make_enum_array(Func&& func) {
		using ret_t = decltype(func(DescT{}));
		return fea::apply_indexes<size()>([&](auto... idxes) {
			return fea::enum_array<ret_t, KeyT>{ func(
					descriptors[KeyT(decltype(idxes)::value)])... };
		});
	}

	// Calls your func for each descriptor and passes a type_wrapper of its
	// type. Get type with : using desc_t = typename decltype(bla)::type;
	template <class Func>
	static constexpr auto for_each_descriptor(Func&& func) {
		return fea::static_for<size()>([&](auto const_i) {
			constexpr KeyT k = KeyT(decltype(const_i)::value);
			return func(descriptors[k]);
		});
	}

private:
	[[nodiscard]] static constexpr bool is_valid() {
		bool ret = true;

		constexpr bool convertible = fea::is_static_castable_v<KeyT, size_t>;
		static_assert(convertible,
				"config_map : descriptor key must be convertible "
				"to size_t");

		ret &= convertible;

		constexpr bool has_all_descriptors
				= sizeof...(Descriptors) == size_t(KeyT::count);
		static_assert(has_all_descriptors,
				"config_map : missing descriptors, or key type doesn't "
				"have 'count' value");
		ret &= has_all_descriptors;

		// Check each descriptor.
		constexpr size_t n = sizeof...(Descriptors);
		fea::static_for<n>([&](auto const_i) -> void {
			constexpr size_t i = const_i;
			constexpr DescT d = descriptors[KeyT(i)];

			// Every descriptor has key.
			constexpr bool has_key
					= fea::is_detected_v<detail::has_config_key, DescT>;
			static_assert(has_key,
					"config_map : all descriptors must have the member "
					"variable 'key'");
			ret &= has_key;

			// Key is correct type.
			constexpr auto k = d.key;
			using key_type = std::decay_t<decltype(k)>;
			constexpr bool same_key_t = std::is_same_v<key_type, KeyT>;
			static_assert(same_key_t,
					"config_map : descriptor keys must all be of type "
					"KeyT");
			ret &= same_key_t;

			// Key is not 'count'.
			constexpr bool not_count = k != KeyT::count;
			static_assert(not_count, "config_map : key cannot be 'count'.");
			ret &= not_count;

			// Every descriptor ordered at its key index.
			constexpr bool ordered_correctly = i == size_t(k);
			static_assert(ordered_correctly,
					"config_map : descriptors must be ordered "
					"in the same order as their 'key' index");
			ret &= ordered_correctly;
		});

		return ret;
	}

	static_assert(is_valid(),
			"config_map : some descriptors are ordered incorrectly, or "
			"missing the '[Kk]ey' member, or '[Kk]ey' is not castable to "
			"size_t");
};
} // namespace fea
