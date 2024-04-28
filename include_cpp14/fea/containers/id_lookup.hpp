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
struct id_lookup {
	// Sanity checks.
	static_assert(std::is_unsigned<fea::detail::id_hash_return_t<Key>>::value,
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
	constexpr id_lookup() = default;
	~id_lookup() = default;
	constexpr id_lookup(const id_lookup&) = default;
	constexpr id_lookup(id_lookup&&) = default;
	constexpr id_lookup& operator=(const id_lookup&) = default;
	constexpr id_lookup& operator=(id_lookup&&) = default;

	/**
	 * Element access
	 */
	// Lookups, return the index of the item.
	FEA_NODISCARD constexpr size_type at_prehashed(
			underlying_key_type uk) const;
	FEA_NODISCARD constexpr size_type at(const Key& k) const;

	// Lookups, return the index of the item.
	FEA_NODISCARD constexpr size_type at_unchecked_prehashed(
			underlying_key_type uk) const noexcept;
	FEA_NODISCARD constexpr size_type at_unchecked(const Key& k) const noexcept;

	// Lookups, return the index of the item.
	// End size should be the size required so begin() + end_size == end().
	FEA_NODISCARD constexpr size_type find_prehashed(
			underlying_key_type uk, size_type end_size) const noexcept;
	FEA_NODISCARD constexpr size_type find(
			const Key& k, size_type end_size) const noexcept;

	// Does key point to a valid item?
	FEA_NODISCARD constexpr bool contains_prehashed(
			underlying_key_type uk) const noexcept;
	FEA_NODISCARD constexpr bool contains(const Key& k) const noexcept;

	FEA_NODISCARD constexpr const pos_type* data() const noexcept;

	FEA_NODISCARD constexpr size_type size() const noexcept;


	/**
	 * Iterators
	 */
	constexpr iterator begin() noexcept;
	constexpr const_iterator begin() const noexcept;
	constexpr const_iterator cbegin() const noexcept;

	constexpr iterator end() noexcept;
	constexpr const_iterator end() const noexcept;
	constexpr const_iterator cend() const noexcept;


	/**
	 * Capacity
	 */
	FEA_NODISCARD constexpr size_type max_size() const noexcept;

	constexpr void reserve(size_type new_cap);

	FEA_NODISCARD constexpr size_type capacity() const noexcept;

	constexpr void shrink_to_fit();


	/**
	 * Modifiers
	 */
	constexpr void clear() noexcept;

	constexpr void insert_prehashed(underlying_key_type uk, size_type new_idx);

	constexpr void insert(const Key& k, size_type new_idx);

	// Insert multiple new keys of contiguous positions.
	// First key is at first_new_idx position.
	template <class FwdIt>
	constexpr void insert(
			FwdIt&& k_begin, FwdIt&& k_end, size_type first_new_idx);

	constexpr void swap(id_lookup& other) noexcept;

	// Sets a pre-existing to sentinel.
	constexpr void invalidate_prehashed(underlying_key_type uk) noexcept;

	constexpr void invalidate(const Key& k) noexcept;

	// Updates the position of a pre-existing key.
	constexpr void update_prehashed(
			underlying_key_type uk, size_type new_idx) noexcept;

	constexpr void update(const Key& k, size_type new_idx) noexcept;

	FEA_NODISCARD static constexpr pos_type sentinel() noexcept;

	FEA_NODISCARD static constexpr underlying_key_type hash(
			const Key& k) noexcept;

private:
	void maybe_resize(underlying_key_type uk);

	std::vector<pos_type, pos_allocator_type> _indexes;
};
} // namespace fea

#include "id_lookup.imp.hpp"
