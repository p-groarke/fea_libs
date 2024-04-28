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
#include "fea/containers/unsigned_set.iterators.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

/*
fea::unsigned_set is an ordered set for unsigned numbers.
The memory backing grows as large as biggest key.

TODO: See fea::id_set for a more generic equivalent capable of storing any key
type.

REMINDER
to try hash algo
modulo -> lookup number of collisions -> ++collisions
	if collisions > max collisions -> rehash
modulo = index start -> std::find -> index start + collisions
*/

namespace fea {
template <class Key, class Alloc = std::allocator<Key>>
struct unsigned_set {
	// Sanity checks.
	static_assert(std::is_unsigned_v<Key>,
			"unsigned_set : Key must be unsigned "
			"integer. Use fea::id_set for id types.");

	// Typedefs
	using key_type = Key;
	using value_type = key_type;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	// using hasher = fea::id_hash<Key>;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<Alloc>::pointer;
	using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
	using const_iterator
			= unsigned_set_const_iterator<unsigned_set<Key, Alloc>>;
	using iterator = unsigned_set_iterator<unsigned_set<Key, Alloc>>;

	// Internals
	// using underlying_key_type = fea::detail::id_hash_return_t<Key>;
	using bool_type = uint8_t;
	using bool_allocator_type = typename std::allocator_traits<
			Alloc>::template rebind_alloc<bool_type>;

	// Ctors
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

	// Iterators

	// Begin iterator, bidirectional.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Begin iterator, bidirectional.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// Begin iterator, bidirectional.
	[[nodiscard]]
	iterator begin() noexcept;

	// End iterator, bidirectional.
	[[nodiscard]]
	const_iterator end() const noexcept;

	// End iterator, bidirectional.
	[[nodiscard]]
	const_iterator cend() const noexcept;

	// End iterator, bidirectional.
	[[nodiscard]]
	iterator end() noexcept;

	// Capacity

	// Is set empty? O(n)
	[[nodiscard]]
	bool empty() const noexcept;

	// Size of set. O(n)
	[[nodiscard]]
	size_type size() const noexcept;

	// Maximum storage size.
	[[nodiscard]]
	size_type max_size() const noexcept;

	// Reserve enough memory to store key.
	void reserve(key_type key);

	// Current capacity.
	[[nodiscard]]
	size_type capacity() const noexcept;

	// Shrink unused memory.
	void shrink_to_fit();

	// Modifiers

	// Clear all items.
	constexpr void clear() noexcept;

	// Insert an item. Returns iterator to item and true if inserted.
	std::pair<iterator, bool> insert(key_type key);

	// Insert multiple items.
	template <class FwdIt>
	void insert(FwdIt first, FwdIt last);

	// Insert multiple items.
	void insert(std::initializer_list<key_type>&& ilist);

	// Erase item, return number of erased items (0 or 1).
	size_type erase(key_type key) noexcept;

	// Erase item at iterator.
	// Returns 1 past erased item if erased, else returns it.
	iterator erase(const_iterator it) noexcept;

	// Erase item at iterator.
	// Returns 1 past erased item if erased, else returns it.
	iterator erase(iterator it) noexcept;

	// Erase multiple items.
	iterator erase(const_iterator first, const_iterator last) noexcept;

	// Swap with another unsigned_set.
	void swap(unsigned_set& other) noexcept;

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_set<Key, Alloc>& source);

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_set<Key, Alloc>&& source);

	// Lookup

	// Returns 1 if key present, 0 if not.
	[[nodiscard]]
	size_type count(key_type key) const noexcept;

	// Returns true if key present.
	bool contains(key_type key) const noexcept;

	// Find the iterator for a key. Returns end() if not present.
	[[nodiscard]]
	const_iterator find(key_type key) const noexcept;

	// Find the iterator for a key. Returns end() if not present.
	[[nodiscard]]
	iterator find(key_type key) noexcept;

private:
	// Stores true at [key] if the key is contained.
	std::vector<bool_type, bool_allocator_type> _map;
};
} // namespace fea

#include "unsigned_set.imp.hpp"
