﻿/*
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
#include "fea/memory/memory.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/scope.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

/*
This is a more traditional-ish "hash map".

- It is flat because iterators are not pairs, and user values are stored
contiguously.
- It is unsigned because it only accepts unsigned keys (todo : allow providing
std::hash).
- It is a hashmap because the container doesn't grow as big as the
biggest key.

Its special characteristics are :
	- Like the unsigned_map, your keys must be unique unsigned numbers.
	- Unlike unsigned_map, you may use as big of keys as you want. The key
		container doesn't grow as big as N.
	- Values are packed, so you may iterate values quickly (the map supports
		.data()).
	- Note : This map doesn't follow the c++ standard apis very closely, as
		iterators are on value_type, not pair<key_type, value_type>.
*/

namespace fea {


template <class Key, class T, class Alloc = std::allocator<T>>
struct unsigned_hole_hashmap {
	// Sanity checks
	static_assert(std::is_unsigned_v<Key>,
			"unsigned_map : key must be unsigned integer");

	// Typedefs
	using key_type = Key;
	using mapped_type = T;
	using value_type = mapped_type;
	using size_type = std::size_t;
	using idx_type =
			typename std::conditional_t<sizeof(key_type) <= sizeof(size_type),
					key_type, size_type>;
	using difference_type = std::ptrdiff_t;

	using allocator_type = Alloc;
	using key_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<key_type>;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	using iterator = typename std::vector<value_type, allocator_type>::iterator;
	using const_iterator =
			typename std::vector<value_type, allocator_type>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	using const_key_iterator =
			typename std::vector<key_type, key_allocator_type>::const_iterator;

	// Don't make sense
	// using hasher = std::hash<key_type>;
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;

private:
	struct lookup_data {
		// The user provided key.
		key_type key = key_sentinel();

		// The index of the user data in the _values container.
		idx_type idx = idx_sentinel();
	};

public:
	using lookup_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<lookup_data>;

	// Ctors
	explicit unsigned_hole_hashmap(size_t reserve_count);
	explicit unsigned_hole_hashmap(
			size_t key_reserve_count, size_t value_reserve_count);

	unsigned_hole_hashmap() = default;
	unsigned_hole_hashmap(const unsigned_hole_hashmap&) = default;
	unsigned_hole_hashmap(unsigned_hole_hashmap&&) noexcept = default;
	unsigned_hole_hashmap& operator=(const unsigned_hole_hashmap&) = default;
	unsigned_hole_hashmap& operator=(unsigned_hole_hashmap&&) noexcept
			= default;

	explicit unsigned_hole_hashmap(
			const std::initializer_list<std::pair<key_type, value_type>>& init);


	// Iterators

	// Returns an iterator to the first value. NOT pair iterators.
	[[nodiscard]]
	iterator begin() noexcept;

	// Returns an iterator to the first value. NOT pair iterators.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Returns an iterator to the first value. NOT pair iterators.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// Returns an iterator past the last value. NOT pair iterators.
	[[nodiscard]]
	iterator end() noexcept;

	// Returns an iterator past the last value. NOT pair iterators.
	[[nodiscard]]
	const_iterator end() const noexcept;

	// Returns an iterator past the last value. NOT pair iterators.
	[[nodiscard]]
	const_iterator cend() const noexcept;

	// Returns an iterator to first key. NOT pair iterators.
	[[nodiscard]]
	const_key_iterator key_begin() const noexcept;

	// Returns an iterator past the last key. NOT pair iterators.
	[[nodiscard]]
	const_key_iterator key_end() const noexcept;


	// Capacity

	// checks whether the container is empty
	[[nodiscard]]
	bool empty() const noexcept;

	// returns the number of elements
	[[nodiscard]]
	size_type size() const noexcept;

	// returns the maximum possible number of elements
	[[nodiscard]]
	size_type max_size() const noexcept;

	// reserves storage
	void reserve(size_type new_cap);

	// returns the number of elements that can be held in currently
	// allocated storage
	[[nodiscard]]
	size_type capacity() const noexcept;

	// reduces memory usage by freeing unused memory
	void shrink_to_fit();


	// Modifiers

	// Clears the contents.
	void clear() noexcept;

	// Inserts new value at key.
	std::pair<iterator, bool> insert(key_type key, const value_type& value);

	// Inserts new value at key.
	std::pair<iterator, bool> insert(key_type key, value_type&& value);

	// Insert new key-value pairs.
	void
	insert(const std::initializer_list<std::pair<key_type, value_type>>& ilist);

	// inserts an element or assigns to the current element if the key
	// already exists
	std::pair<iterator, bool> insert_or_assign(
			key_type key, const value_type& value);

	std::pair<iterator, bool> insert_or_assign(
			key_type key, value_type&& value);

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(key_type key, Args&&... args);

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(key_type key, Args&&... args);

	// Erases element at position.
	void erase(const_iterator pos);

	// Erases range of elements.
	void erase(const_iterator first, const_iterator last);

	// Erase element at key.
	size_type erase(key_type k);

	// swaps the contents
	void swap(unsigned_hole_hashmap& other) noexcept;


	// Lookup
	// direct access to the underlying vector
	[[nodiscard]]
	const value_type* data() const noexcept;

	// direct access to the underlying vector
	[[nodiscard]]
	value_type* data() noexcept;

	// Direct access to the keys underlying vector.
	// Same size as values.
	[[nodiscard]]
	const key_type* key_data() const noexcept;

	// Access specified element with bounds checking.
	[[nodiscard]]
	const mapped_type& at(key_type k) const;

	// Access specified element with bounds checking.
	[[nodiscard]]
	mapped_type& at(key_type k);

	// Access specified element without any bounds checking.
	[[nodiscard]]
	const mapped_type& at_unchecked(key_type k) const;

	// Access specified element without any bounds checking.
	[[nodiscard]]
	mapped_type& at_unchecked(key_type k);

	// access or insert specified element
	[[nodiscard]]
	mapped_type&
	operator[](key_type k);

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	[[nodiscard]]
	size_type count(key_type k) const;

	// finds element with specific key
	[[nodiscard]]
	const_iterator find(key_type k) const;

	// finds element with specific key
	[[nodiscard]]
	iterator find(key_type k);

	// checks if the container contains element with specific key
	[[nodiscard]]
	bool contains(key_type k) const;


	// Hash policy

	// Returns average number of elements per bucket.
	[[nodiscard]]
	float load_factor() const noexcept;

	// The stored maximum load factor before a rehash.
	[[nodiscard]]
	float max_load_factor() const noexcept;

	// Set a custom load factor to control rehashing behavior.
	void max_load_factor(float ml) noexcept;

	// Rehash the container for count elements.
	void rehash(size_type count);


	// Non-member functions

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator==(const unsigned_hole_hashmap<K, U, A>& lhs,
			const unsigned_hole_hashmap<K, U, A>& rhs);

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator!=(const unsigned_hole_hashmap<K, U, A>& lhs,
			const unsigned_hole_hashmap<K, U, A>& rhs);

private:
	[[nodiscard]]
	size_type hash_max() const;

	[[nodiscard]]
	size_type key_to_index(key_type key) const;

	[[nodiscard]]
	static constexpr size_type key_to_index(key_type key, size_type h_max);
	[[nodiscard]]
	static constexpr key_type key_sentinel() noexcept;
	[[nodiscard]]
	static constexpr idx_type idx_sentinel() noexcept;
	[[nodiscard]]
	static constexpr size_type init_count() noexcept;

	// Custom find_if.
	// todo : benchmark simd search
	template <class Iter, class Func>
	[[nodiscard]]
	static auto find_slot(Iter start, Iter end, Func func);

	// Returns lookup iterator to either the lookup if it exists, or the first
	// free slot.
	[[nodiscard]]
	auto find_first_slot_or_hole(key_type key) const;
	[[nodiscard]]
	auto find_first_slot_or_hole(key_type key);

	// Find first free slot given a lookup idx.
	template <class Iter>
	[[nodiscard]]
	static auto find_first_hole(Iter beg, Iter end, size_type bucket_idx);

	// Grows lookup for trailing collisions.
	[[nodiscard]]
	auto trailing_resize(
			std::vector<lookup_data, lookup_allocator_type>& lookup);

	// Packs the collisions so all clashing keys are contigous.
	// This is necessary after erase since erase could create a hole, with a
	// collision left over after that whole. This would break the container
	// guarantee that all collisions are packed until the first hole.
	void repack_collisions(size_type hole_idx);

	template <class M>
	std::pair<iterator, bool> minsert(
			key_type key, M&& value, bool assign_found = false);

	// OK, so, we always have max_hash * 2 lookups. The load_factor will be
	// tested on max_hash, and not on _lookup.size().
	// This guarantees we will resize even if we fill up collisions only.
	// It also leaves some space at the end since wrapping around container is
	// bad.
	float _max_load_factor = .75f;

	// The hash max value is the current theoretical size of the lookup.
	// It is decoupled from _lookup.size() to allow growing lookup in certain
	// situations (when adding collisions at the end, requires growing lookup).
	size_type _hash_max = 0;

	// Stores the key at hash and points to the values index.
	// Lookups at odd indexes are collisions stored in-place.
	std::vector<lookup_data, lookup_allocator_type> _lookup;

	// Used in erase for swap & pop.
	std::vector<key_type, key_allocator_type> _reverse_lookup;

	// Packed user values.
	// Since this is a flat map, the values are tightly packed instead of in
	// pairs.
	// This means we cannot fulfill standard apis.
	// todo : make unsigned_hashmap for cases when you need pair iterators.
	std::vector<value_type, allocator_type> _values;

	// When the lookup collisions fill up the end of the lookup container, by
	// how much do we resize it?
	constexpr static double _lookup_trailing_amount = 1.25;
};
} // namespace fea

#include "imp/unsigned_hole_hashmap.imp.hpp"
