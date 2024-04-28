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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

/*
fea::unsigned_set is an unordered_set interface for unsigned keys which grow in
a controlled linear fashion (ids). The memory backing grows as large as biggest
key!

*/

/*
REMINDER
to try hash algo
modulo -> lookup number of collisions -> ++collisions
	if collisions > max collisions -> rehash
modulo = index start -> std::find -> index start + collisions
*/

namespace fea {
namespace experimental {
// TODO : Specialize for pure unsigned. Specialize for ordered/unordered.
// Currently unsafe as it drops potential id extra data.

template <class MySet>
struct unsigned_set_const_iterator {
	using difference_type = typename MySet::difference_type;
	using key_type = typename MySet::key_type;
	using value_type = key_type;
	using pointer = void;
	using reference = key_type;
	using iterator_category = std::bidirectional_iterator_tag;

	// Internals.
	using bool_allocator_type = typename MySet::bool_allocator_type;
	using bool_pointer =
			typename std::allocator_traits<bool_allocator_type>::const_pointer;
	using underlying_key_type = typename MySet::underlying_key_type;

	constexpr unsigned_set_const_iterator() noexcept = default;
	constexpr unsigned_set_const_iterator(
			bool_pointer first, bool_pointer last, bool_pointer ptr) noexcept;

	[[nodiscard]] constexpr key_type operator*() const noexcept;
	[[nodiscard]] constexpr key_type operator->() const noexcept = delete;

	constexpr unsigned_set_const_iterator& operator++() noexcept;
	constexpr unsigned_set_const_iterator& operator++(int) noexcept;

	constexpr unsigned_set_const_iterator& operator--() noexcept;
	constexpr unsigned_set_const_iterator& operator--(int) noexcept;

	[[nodiscard]] bool operator==(
			const unsigned_set_const_iterator& rhs) const noexcept;
	[[nodiscard]] bool operator!=(
			const unsigned_set_const_iterator& rhs) const noexcept;
	[[nodiscard]] bool operator<(
			const unsigned_set_const_iterator& rhs) const noexcept;
	[[nodiscard]] bool operator>(
			const unsigned_set_const_iterator& rhs) const noexcept;
	[[nodiscard]] bool operator<=(
			const unsigned_set_const_iterator& rhs) const noexcept;
	[[nodiscard]] bool operator>=(
			const unsigned_set_const_iterator& rhs) const noexcept;

private:
	bool_pointer _first = nullptr;
	bool_pointer _last = nullptr;
	bool_pointer _ptr = nullptr;
};

template <class MySet>
struct unsigned_set_iterator : public unsigned_set_const_iterator<MySet> {
	using base_t = unsigned_set_const_iterator<MySet>;

	using difference_type = typename base_t::difference_type;
	using key_type = typename base_t::key_type;
	using value_type = key_type;
	using pointer = key_type;
	using reference = key_type;
	using iterator_category = std::bidirectional_iterator_tag;

	using base_t::base_t;
};


template <class Key, class Alloc = std::allocator<Key>>
struct unsigned_set {
	static_assert(std::is_unsigned_v<fea::detail::id_hash_return_t<Key>>,
			"unsigned_set : key or id_hash return type must be unsigned "
			"integer");

	using key_type = Key;
	using value_type = key_type;
	using size_type = std::size_t;
	// using ssize_type = std::make_signed_t<size_type>;
	using difference_type = std::ptrdiff_t;
	using hasher = fea::id_hash<Key>;
	// using key_equal =
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<Alloc>::pointer;
	using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
	using const_iterator
			= unsigned_set_const_iterator<unsigned_set<Key, Alloc>>;
	using iterator = unsigned_set_iterator<unsigned_set<Key, Alloc>>;

	// Internals
	using underlying_key_type = fea::detail::id_hash_return_t<Key>;
	using bool_type = uint8_t;
	using bool_allocator_type = typename std::allocator_traits<
			Alloc>::template rebind_alloc<bool_type>;


	// ctors
	unsigned_set() = default;
	~unsigned_set() = default;
	unsigned_set(const unsigned_set&) = default;
	unsigned_set(unsigned_set&&) = default;
	unsigned_set& operator=(const unsigned_set&) = default;
	unsigned_set& operator=(unsigned_set&&) = default;

	// Initializes with provided keys.
	template <class FwdIt>
	unsigned_set(FwdIt first, FwdIt last);

	// Initializes with provided keys.
	unsigned_set(std::initializer_list<key_type>&& ilist);

	/**
	 * Iterators
	 */
	[[nodiscard]] const_iterator begin() const noexcept;
	[[nodiscard]] const_iterator cbegin() const noexcept;
	[[nodiscard]] iterator begin() noexcept;

	[[nodiscard]] const_iterator end() const noexcept;
	[[nodiscard]] const_iterator cend() const noexcept;
	[[nodiscard]] iterator end() noexcept;

	/**
	 * Capacity
	 */
	// Is set empty? O(n)
	[[nodiscard]] bool empty() const noexcept(
			noexcept(std::find(const_iterator{}, const_iterator{}, uint8_t{})));

	// Size of set. O(n)
	[[nodiscard]] size_type size() const noexcept;

	// Maximum storage size.
	[[nodiscard]] size_type max_size() const noexcept;

	// Reserve enough memory to store key.
	void reserve(const key_type& key);

	// Current capacity.
	[[nodiscard]] size_type capacity() const noexcept;

	// Shrink unused memory.
	void shrink_to_fit();

	/**
	 * Modifiers
	 */
	// Clear all items.
	constexpr void clear() noexcept;

	// Insert an item. Returns iterator to item and true if inserted.
	std::pair<iterator, bool> insert(const key_type&);

	// Insert multiple items.
	template <class FwdIt>
	void insert(FwdIt first, FwdIt last);
	void insert(std::initializer_list<key_type>&& ilist);

	// Erase item.
	size_type erase(const key_type& key);

	// Erase item at iterator.
	iterator erase(iterator);
	iterator erase(const_iterator);

	// Swap with another unsigned_set.
	void swap(unsigned_set& other) noexcept;

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_set<Key, Alloc>& source);
	void merge(unsigned_set<Key, Alloc>&& source);

	/**
	 * Lookup
	 */
	// Returns 1 if key present, 0 if not.
	size_type count(const key_type& key) const
			noexcept(noexcept(hasher{}(key)));

	// Returns true if key present.
	bool contains(const key_type& key) const;

	// Find the iterator for a key. Returns end() if not present.
	const_iterator find(const key_type& key) const
			noexcept(noexcept(hasher{}(key)));
	iterator find(const key_type& key) noexcept(noexcept(hasher{}(key)));

private:
	std::vector<bool_type, bool_allocator_type> _map;
};
} // namespace experimental
} // namespace fea

#include "unsigned_set.imp.hpp"
