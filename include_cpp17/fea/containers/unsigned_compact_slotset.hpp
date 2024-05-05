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
#include "fea/containers/imp/unsigned_compact_slotset.iterators.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

/*
fea::unsigned_compact_slotset is an ordered set for unsigned numbers.
The memory backing is a bitset and grows as large as biggest key / ARCH_BITS.
It isn't thread safe.

See fea::unsigned_compact_slotset for a version which uses more memory, but has
less cpu cost and is thread-safe.
*/

namespace fea {
template <class Key, class Alloc = std::allocator<Key>>
struct unsigned_compact_slotset {
	// Sanity checks.
	static_assert(std::is_unsigned_v<Key>,
			"unsigned_compact_set : Key must be unsigned "
			"integer. Use fea::id_set for id types.");

	// Typedefs
	using key_type = Key;
	using value_type = key_type;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using allocator_type = Alloc;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<Alloc>::pointer;
	using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
	using const_iterator = ucss_const_iterator<unsigned_compact_slotset>;
	// using iterator = ucss_iterator<unsigned_compact_slotset>;

	// Internals
	static constexpr size_type bitset_size = fea::arch;
	using bool_type = std::bitset<bitset_size>;
	using bool_allocator_type = fea::rebind_alloc_t<Alloc, bool_type>;
	using bool_lookup_t = std::vector<bool_type, bool_allocator_type>;
	using bool_const_iterator = typename bool_lookup_t::const_iterator;
	using bool_iterator = typename bool_lookup_t::iterator;

	// Ctors
	unsigned_compact_slotset() = default;
	~unsigned_compact_slotset() = default;
	unsigned_compact_slotset(const unsigned_compact_slotset&) = default;
	unsigned_compact_slotset(unsigned_compact_slotset&&) = default;
	unsigned_compact_slotset& operator=(const unsigned_compact_slotset&)
			= default;
	unsigned_compact_slotset& operator=(unsigned_compact_slotset&&) = default;

	// Initializes with provided keys.
	template <class FwdIt>
	unsigned_compact_slotset(FwdIt first, FwdIt last);

	// Initializes with provided keys.
	unsigned_compact_slotset(std::initializer_list<key_type>&& ilist);

	// Iterators

	// Begin iterator, bidirectional.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Begin iterator, bidirectional.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// End iterator, bidirectional.
	[[nodiscard]]
	const_iterator end() const noexcept;

	// End iterator, bidirectional.
	[[nodiscard]]
	const_iterator cend() const noexcept;


	// Capacity

	// Any keys in set?
	[[nodiscard]]
	bool empty() const noexcept;

	// Number of keys in set.
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
	std::pair<const_iterator, bool> insert(key_type key);

	// Insert multiple items.
	template <class FwdIt>
	void insert(FwdIt first, FwdIt last);

	// Insert multiple items.
	void insert(std::initializer_list<key_type>&& ilist);

	// Erase item, return number of erased items (0 or 1).
	size_type erase(key_type key) noexcept;

	// Erase item at iterator.
	// Returns 1 past erased item if erased, else returns it.
	const_iterator erase(const_iterator it) noexcept;

	// Erase multiple items.
	const_iterator erase(const_iterator first, const_iterator last) noexcept;

	// Swap with another unsigned_set.
	void swap(unsigned_compact_slotset& other) noexcept;

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_compact_slotset& source);

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_compact_slotset&& source);

	// Lookup

	// Returns 1 if key present, 0 if not.
	[[nodiscard]]
	size_type count(key_type key) const noexcept;

	// Returns true if key present.
	bool contains(key_type key) const noexcept;

	// Find the iterator for a key. Returns end() if not present.
	[[nodiscard]]
	const_iterator find(key_type key) const noexcept;

private:
	// Returns the absolute (lookup) index of key.
	[[nodiscard]]
	static size_type lookup_idx(key_type key) noexcept;

	// Returns the local (bitset) index of key.
	[[nodiscard]]
	static size_type local_idx(key_type key) noexcept;

	// Returns the absolute (lookup) index of iterator.
	[[nodiscard]]
	static size_type lookup_idx(const_iterator it) noexcept;

	// Returns the local (bitset) index of key.
	[[nodiscard]]
	static size_type local_idx(const_iterator it) noexcept;

	// Potentially resizes the lookup if it is required to fit key.
	// Returns the absolute bitset index of key.
	size_type maybe_resize(key_type key);

	// Stores true at [key] if the key is contained.
	bool_lookup_t _lookup{};
	size_type _size = 0;
};
} // namespace fea

#include "imp/unsigned_compact_slotset.imp.hpp"
