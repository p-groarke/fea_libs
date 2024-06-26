/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/containers/id_hash.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

namespace fea {

// A flat unsigned lookup helper, used internally in various maps and whatnot.
// This structure only takes care of ids and their indexes, it does not hold any
// user data. You must synchronize the items according to this lookup yourself.

template <class Key, class TAlloc = std::allocator<Key>>
struct id_slot_lookup {
	// Sanity checks.
	static_assert(std::is_unsigned_v<fea::detail::id_hash_return_t<Key>>,
			"unsigned_map : key or id_hash return type must be unsigned "
			"integer");

	// Typedefs
	using hasher = fea::id_hash<Key>;
	using underlying_key_type = fea::detail::id_hash_return_t<Key>;
	using pos_type = underlying_key_type;
	using pos_allocator_type = typename std::allocator_traits<
			TAlloc>::template rebind_alloc<pos_type>;
	using size_type = std::size_t;
	using ssize_type = std::make_signed_t<size_type>;

	using iterator =
			typename std::vector<pos_type, pos_allocator_type>::iterator;
	using const_iterator =
			typename std::vector<pos_type, pos_allocator_type>::const_iterator;

	/**
	 * Ctors
	 */
	constexpr id_slot_lookup() = default;
	~id_slot_lookup() = default;
	constexpr id_slot_lookup(const id_slot_lookup&) = default;
	constexpr id_slot_lookup(id_slot_lookup&&) = default;
	constexpr id_slot_lookup& operator=(const id_slot_lookup&) = default;
	constexpr id_slot_lookup& operator=(id_slot_lookup&&) = default;

	// Element access

	// Lookups, return the index of the item.
	[[nodiscard]]
	constexpr size_type at_prehashed(underlying_key_type uk) const;
	[[nodiscard]]
	constexpr size_type at(const Key& k) const;

	// Lookups, return the index of the item.
	[[nodiscard]]
	constexpr size_type at_unchecked_prehashed(
			underlying_key_type uk) const noexcept;
	[[nodiscard]]
	constexpr size_type at_unchecked(const Key& k) const noexcept;

	// Lookups, return the index of the item.
	// End size should be the size required so begin() + end_size == end().
	[[nodiscard]]
	constexpr size_type find_prehashed(
			underlying_key_type uk, size_type end_size) const noexcept;
	[[nodiscard]]
	constexpr size_type find(const Key& k, size_type end_size) const noexcept;

	// Does key point to a valid item?
	[[nodiscard]]
	constexpr bool contains_prehashed(underlying_key_type uk) const noexcept;
	[[nodiscard]]
	constexpr bool contains(const Key& k) const noexcept;

	[[nodiscard]]
	constexpr const pos_type* data() const noexcept;

	[[nodiscard]]
	constexpr size_type size() const noexcept;


	// Iterators

	// Returns an iterator to the positions, NOT the ids.
	constexpr iterator begin() noexcept;

	// Returns an iterator to the positions, NOT the ids.
	constexpr const_iterator begin() const noexcept;

	// Returns an iterator to the positions, NOT the ids.
	constexpr const_iterator cbegin() const noexcept;

	// Returns an iterator to the positions, NOT the ids.
	constexpr iterator end() noexcept;

	// Returns an iterator to the positions, NOT the ids.
	constexpr const_iterator end() const noexcept;

	// Returns an iterator to the positions, NOT the ids.
	constexpr const_iterator cend() const noexcept;


	// Capacity

	// Maximum storable size (std::vector::max_size - 1).
	[[nodiscard]]
	constexpr size_type max_size() const noexcept;

	// Reserve memory, recommended maxid + 1.
	constexpr void reserve(size_type new_cap);

	// Current capacity.
	[[nodiscard]]
	constexpr size_type capacity() const noexcept;

	// Shrink memory to fit number of positions.
	constexpr void shrink_to_fit();


	// Modifiers

	// Clear container.
	constexpr void clear() noexcept;

	// Insert a new key that will be stored at new_idx.
	constexpr void insert_prehashed(underlying_key_type uk, size_type new_idx);

	// Insert a new key that will be stored at new_idx.
	constexpr void insert(const Key& k, size_type new_idx);

	// Insert multiple new keys of contiguous positions.
	// First key is at first_new_idx position.
	template <class FwdIt>
	constexpr void insert(
			FwdIt&& k_begin, FwdIt&& k_end, size_type first_new_idx);

	// Swap with other id_slot_lookup.
	constexpr void swap(id_slot_lookup& other) noexcept;

	// Invalidates a pre-existing id. Sets its position to our sentinel.
	constexpr void invalidate_prehashed(underlying_key_type uk) noexcept;

	// Invalidates a pre-existing id. Sets its position to our sentinel.
	constexpr void invalidate(const Key& k) noexcept;

	// Updates the position of a pre-existing key.
	constexpr void update_prehashed(
			underlying_key_type uk, size_type new_idx) noexcept;

	// Updates the position of a pre-existing key.
	constexpr void update(const Key& k, size_type new_idx) noexcept;

	// Sentinel used to mark ids invalid.
	[[nodiscard]]
	static constexpr pos_type sentinel() noexcept;

	// Hash a key using fea::id_hash.
	[[nodiscard]]
	static constexpr underlying_key_type hash(const Key& k) noexcept;

private:
	// Might resize our storage to fit the key.
	void maybe_resize(underlying_key_type uk);

	std::vector<pos_type, pos_allocator_type> _indexes;
};
} // namespace fea

#include "imp/id_slot_lookup.imp.hpp"
