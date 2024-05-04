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
#include "fea/enum/enum_array.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"

#include <array>
#include <tuple>

/*
Descriptors are data-driven, compile time configurators.

They are stored in a queriable map and used to centralize your architecture
elements. Descriptors help reduce copy-paste and improve readability. This
header implements commonly used descriptor helpers.

descriptor_map
A compile-time map of descriptors. Use it to apply static_asserts and create
arrays. When adding descriptors to the descriptor_map, all descriptors must
contain a static constexpr enum/integer 'key'. The map checks if your
descriptor order matches its key.
*/

namespace fea {

/*
A helper map which stores all your descriptors, runs a multitude of
static_asserts and facilitates creating arrays from your descriptor members.
Provide your key type, it must be castable to size_t.
Provide your descriptors, ordered in the same order as their key (indexed at the
key value).
*/
template <class KeyT, class... Descriptors>
struct descriptor_map {
	// A tuple of all descriptors.
	using desc_tup_t = std::tuple<Descriptors...>;

	// Descriptor count.
	static constexpr size_t desc_count = sizeof...(Descriptors);

	// The type of key. Must be castable to size_t.
	using key_t = KeyT;

	// All the keys.
	static constexpr std::array<key_t, desc_count> keys{ Descriptors::key... };

	// Number of descriptors.
	[[nodiscard]]
	static constexpr size_t size();

	// Get a specific descriptor.
	template <KeyT K>
	[[nodiscard]]
	static constexpr auto descriptor();

	// Get a specific descriptor at runtime.
	template <class Func>
	[[nodiscard]]
	static constexpr decltype(auto) descriptor(KeyT key, Func&& func);


	// Get a descriptor's key at index I.
	template <size_t Idx>
	[[nodiscard]]
	static constexpr auto key();

	// Returns a tuple filled with your operation results, indexed at key.
	// Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]]
	static constexpr auto make_tuple(Func&& func);

	// Returns an array filled with your operation result, indexed at key.
	// Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]]
	static constexpr auto make_array(Func&& func);

	// Returns an fea::enum_array filled with your operation result, indexed at
	// enum key. Your function should return the desired descriptor value.
	template <class Func>
	[[nodiscard]]
	static constexpr auto make_enum_array(Func&& func);

	// Calls your func for each descriptor and passes a type_wrapper of its
	// type. Get type with : using desc_t = typename decltype(bla)::type;
	template <class Func>
	static constexpr auto for_each_descriptor(Func&& func);

private:
	[[nodiscard]]
	static constexpr bool has_valid_keys();

	static_assert(has_valid_keys(),
			"descriptor_map : some descriptors are ordered incorrectly, or "
			"missing the '[Kk]ey' member, or '[Kk]ey' is not castable to "
			"size_t");
};
} // namespace fea

#include "imp/descriptor_map.imp.hpp"
