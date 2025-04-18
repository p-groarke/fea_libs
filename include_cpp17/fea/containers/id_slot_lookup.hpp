/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
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


// Implementation
namespace fea {
template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::at_prehashed(
		underlying_key_type uk) const -> size_type {
	size_type ret = find_prehashed(uk, sentinel());
	if (ret == sentinel()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "invalid key");
	}
	return ret;
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::at(const Key& k) const
		-> size_type {
	underlying_key_type uk = hash(k);
	return at_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::at_unchecked_prehashed(
		underlying_key_type uk) const noexcept -> size_type {
	assert(contains_prehashed(uk));
	return size_type(_indexes[uk]);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::at_unchecked(
		const Key& k) const noexcept -> size_type {
	underlying_key_type uk = hash(k);
	return at_unchecked_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::find_prehashed(
		underlying_key_type uk, size_type end_size) const noexcept
		-> size_type {
	if (uk >= _indexes.size()) {
		return end_size;
	}
	pos_type pos = _indexes[uk];
	if (pos == sentinel()) {
		return end_size;
	}
	assert(pos < end_size);
	return size_type(pos);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::find(
		const Key& k, size_type end_size) const noexcept -> size_type {
	underlying_key_type uk = hash(k);
	return find_prehashed(uk, end_size);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::contains_prehashed(
		underlying_key_type uk) const noexcept -> bool {
	if (uk >= _indexes.size() || _indexes[uk] == sentinel()) {
		return false;
	}
	return true;
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::contains(
		const Key& k) const noexcept -> bool {
	underlying_key_type uk = hash(k);
	return contains_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::data() const noexcept
		-> const pos_type* {
	return _indexes.data();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::size() const noexcept -> size_type {
	return _indexes.size();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::begin() noexcept -> iterator {
	return _indexes.begin();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::begin() const noexcept
		-> const_iterator {
	return _indexes.begin();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::cbegin() const noexcept
		-> const_iterator {
	return _indexes.cbegin();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::end() noexcept -> iterator {
	return _indexes.end();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::end() const noexcept
		-> const_iterator {
	return _indexes.end();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::cend() const noexcept
		-> const_iterator {
	return _indexes.cend();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::max_size() const noexcept
		-> size_type {
	// Reserve 1 slot for sentinel.
	return size_type(sentinel()) - size_type(1);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::reserve(size_type new_cap) -> void {
	_indexes.reserve(new_cap);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::capacity() const noexcept
		-> size_type {
	return _indexes.capacity();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::shrink_to_fit() -> void {
	_indexes.shrink_to_fit();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::clear() noexcept -> void {
	_indexes.clear();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::insert_prehashed(
		underlying_key_type uk, size_type new_idx) -> void {
	assert(!contains_prehashed(uk));
	maybe_resize(uk);
	_indexes[uk] = pos_type(new_idx);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::insert(
		const Key& k, size_type new_idx) -> void {
	underlying_key_type uk = hash(k);
	insert_prehashed(uk, new_idx);
}

template <class Key, class TAlloc>
template <class FwdIt>
constexpr auto id_slot_lookup<Key, TAlloc>::insert(
		FwdIt&& k_begin, FwdIt&& k_end, size_type first_new_idx) -> void {
	auto max_it = std::max_element(
			k_begin, k_end, [](const Key& lhs, const Key& rhs) {
				return hash(lhs) < hash(rhs);
			});
	maybe_resize(*max_it);

	for (auto it = k_begin; it != k_end; ++it) {
		underlying_key_type uk = hash(*it);
		assert(!contains_prehashed(uk));
		_indexes[uk] = pos_type(first_new_idx++);
	}
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::swap(id_slot_lookup& other) noexcept
		-> void {
	_indexes.swap(other._indexes);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::invalidate_prehashed(
		underlying_key_type uk) noexcept -> void {
	assert(contains_prehashed(uk));
	_indexes[uk] = sentinel();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::invalidate(const Key& k) noexcept
		-> void {
	underlying_key_type uk = hash(k);
	invalidate_prehashed(uk);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::update_prehashed(
		underlying_key_type uk, size_type new_idx) noexcept -> void {
	assert(contains_prehashed(uk));
	_indexes[uk] = pos_type(new_idx);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::update(
		const Key& k, size_type new_idx) noexcept -> void {
	underlying_key_type uk = hash(k);
	update_prehashed(uk, new_idx);
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::sentinel() noexcept -> pos_type {
	return (std::numeric_limits<pos_type>::max)();
}

template <class Key, class TAlloc>
constexpr auto id_slot_lookup<Key, TAlloc>::hash(const Key& k) noexcept
		-> underlying_key_type {
	return hasher{}(k);
}

template <class Key, class TAlloc>
auto id_slot_lookup<Key, TAlloc>::maybe_resize(underlying_key_type uk) -> void {
	if (uk < _indexes.size()) {
		return;
	}

	if (uk == sentinel()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "maximum size reached");
	}

	_indexes.resize(size_t(uk) + 1u, sentinel());
	assert(uk < _indexes.size());
}
} // namespace fea
