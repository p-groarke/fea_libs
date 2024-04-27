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
namespace detail {

// A flat unsigned lookup helper, used internally in various maps and whatnot.
// This structure only takes care of ids and their indexes, it does not hold any
// user data. You must synchronize the items according to this lookup yourself.

template <class Key, class TAlloc = std::allocator<Key>>
struct unsigned_lookup {
	static_assert(std::is_unsigned<fea::detail::id_hash_t<Key>>::value,
			"unsigned_map : key or id_hash return type must be unsigned "
			"integer");

	using hasher = fea::id_hash<Key>;
	using underlying_key_type = fea::detail::id_hash_t<Key>;
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
	constexpr unsigned_lookup() = default;
	~unsigned_lookup() = default;
	constexpr unsigned_lookup(const unsigned_lookup&) = default;
	constexpr unsigned_lookup(unsigned_lookup&&) = default;
	constexpr unsigned_lookup& operator=(const unsigned_lookup&) = default;
	constexpr unsigned_lookup& operator=(unsigned_lookup&&) = default;

	/**
	 * Element access
	 */
	// Lookups, return the index of the item.
	FEA_NODISCARD constexpr size_type at_prehashed(
			underlying_key_type uk) const {
		size_type ret = find_prehashed(uk, sentinel());
		if (ret == sentinel()) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "invalid key");
		}
		return ret;
	}
	FEA_NODISCARD constexpr size_type at(const Key& k) const {
		underlying_key_type uk = hash(k);
		return at_prehashed(uk);
	}

	// Lookups, return the index of the item.
	FEA_NODISCARD constexpr size_type at_unchecked_prehashed(
			underlying_key_type uk) const noexcept {
		assert(contains_prehashed(uk));
		return size_type(_indexes[uk]);
	}
	FEA_NODISCARD constexpr size_type at_unchecked(
			const Key& k) const noexcept {
		underlying_key_type uk = hash(k);
		return at_unchecked_prehashed(uk);
	}

	// Lookups, return the index of the item.
	// End size should be the size required so begin() + end_size == end().
	FEA_NODISCARD constexpr size_type find_prehashed(
			underlying_key_type uk, size_type end_size) const noexcept {
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
	FEA_NODISCARD constexpr size_type find(
			const Key& k, size_type end_size) const noexcept {
		underlying_key_type uk = hash(k);
		return find_prehashed(uk, end_size);
	}

	// Does key point to a valid item?
	FEA_NODISCARD constexpr bool contains_prehashed(
			underlying_key_type uk) const noexcept {
		if (uk >= _indexes.size() || _indexes[uk] == sentinel()) {
			return false;
		}
		return true;
	}
	FEA_NODISCARD constexpr bool contains(const Key& k) const noexcept {
		underlying_key_type uk = hash(k);
		return contains_prehashed(uk);
	}

	FEA_NODISCARD constexpr const pos_type* data() const noexcept {
		return _indexes.data();
	}

	FEA_NODISCARD constexpr size_type size() const noexcept {
		return _indexes.size();
	}


	/**
	 * Iterators
	 */
	constexpr iterator begin() noexcept {
		return _indexes.begin();
	}
	constexpr const_iterator begin() const noexcept {
		return _indexes.begin();
	}
	constexpr const_iterator cbegin() const noexcept {
		return _indexes.cbegin();
	}

	constexpr iterator end() noexcept {
		return _indexes.end();
	}
	constexpr const_iterator end() const noexcept {
		return _indexes.end();
	}
	constexpr const_iterator cend() const noexcept {
		return _indexes.cend();
	}


	/**
	 * Capacity
	 */
	FEA_NODISCARD constexpr size_type max_size() const noexcept {
		// Reserve 1 slot for sentinel.
		return size_type(sentinel()) - size_type(1);
	}
	constexpr void reserve(size_type new_cap) {
		_indexes.reserve(new_cap);
	}
	FEA_NODISCARD constexpr size_type capacity() const noexcept {
		return _indexes.capacity();
	}
	constexpr void shrink_to_fit() {
		_indexes.shrink_to_fit();
	}


	/**
	 * Modifiers
	 */
	constexpr void clear() noexcept {
		_indexes.clear();
	}

	constexpr void insert_prehashed(underlying_key_type uk, size_type new_idx) {
		assert(!contains_prehashed(uk));
		maybe_resize(uk);
		_indexes[uk] = pos_type(new_idx);
	}
	constexpr void insert(const Key& k, size_type new_idx) {
		underlying_key_type uk = hash(k);
		insert_prehashed(uk, new_idx);
	}

	// Insert multiple new keys of contiguous positions.
	// First key is at first_new_idx position.
	template <class FwdIt>
	constexpr void insert(
			FwdIt&& k_begin, FwdIt&& k_end, size_type first_new_idx) {
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

	constexpr void swap(unsigned_lookup& other) noexcept {
		_indexes.swap(other._indexes);
	}

	// Sets a pre-existing to sentinel.
	constexpr void invalidate_prehashed(underlying_key_type uk) noexcept {
		assert(contains_prehashed(uk));
		_indexes[uk] = sentinel();
	}
	constexpr void invalidate(const Key& k) noexcept {
		underlying_key_type uk = hash(k);
		invalidate_prehashed(uk);
	}

	// Updates the position of a pre-existing key.
	constexpr void update_prehashed(
			underlying_key_type uk, size_type new_idx) noexcept {
		assert(contains_prehashed(uk));
		_indexes[uk] = pos_type(new_idx);
	}
	constexpr void update(const Key& k, size_type new_idx) noexcept {
		underlying_key_type uk = hash(k);
		update_prehashed(uk, new_idx);
	}

	FEA_NODISCARD static constexpr pos_type sentinel() noexcept {
		return (std::numeric_limits<pos_type>::max)();
	}

	FEA_NODISCARD static constexpr underlying_key_type hash(
			const Key& k) noexcept {
		return hasher{}(k);
	}

private:
	void maybe_resize(underlying_key_type uk) {
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

	std::vector<pos_type, pos_allocator_type> _indexes;
};

} // namespace detail
} // namespace fea
