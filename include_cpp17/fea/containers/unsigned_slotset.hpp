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
#include "fea/meta/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

/*
fea::unsigned_slotset is an ordered set for unsigned numbers.
The memory backing grows as large as biggest key.

See fea::unsigned_compact_slotset for a version which uses less memory, but has
higher cpu cost and isn't thread-safe.

TODO: fea::id_set for a more generic equivalent capable of storing any key
type.

REMINDER
to try hash algo
modulo -> lookup number of collisions -> ++collisions
	if collisions > max collisions -> rehash
modulo = index start -> std::find -> index start + collisions
*/

namespace fea {
template <class>
struct uss_const_iterator;

template <class Key, class Alloc = std::allocator<Key>>
struct unsigned_slotset {
	// Sanity checks.
	static_assert(std::is_unsigned_v<Key>,
			"unsigned_set : Key must be unsigned "
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
	using const_iterator = uss_const_iterator<unsigned_slotset>;
	using iterator = const_iterator;

	// Internals
	using bool_type = uint8_t;
	using bool_allocator_type = fea::rebind_alloc_t<Alloc, bool_type>;
	using bool_lookup_t = std::vector<bool_type, bool_allocator_type>;
	using bool_const_iterator = typename bool_lookup_t::const_iterator;
	using bool_iterator = typename bool_lookup_t::iterator;

	// Ctors
	unsigned_slotset() = default;
	~unsigned_slotset() = default;
	unsigned_slotset(const unsigned_slotset&) = default;
	unsigned_slotset(unsigned_slotset&&) = default;
	unsigned_slotset& operator=(const unsigned_slotset&) = default;
	unsigned_slotset& operator=(unsigned_slotset&&) = default;

	// Initializes with provided keys.
	template <class FwdIt>
	unsigned_slotset(FwdIt first, FwdIt last);

	// Initializes with provided keys.
	unsigned_slotset(std::initializer_list<key_type>&& ilist);

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
	void clear() noexcept;

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
	void swap(unsigned_slotset& other) noexcept;

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_slotset& source);

	// Merge with source.
	// Items are "stolen" from source, only if they don't exist in destination.
	void merge(unsigned_slotset&& source);

	// Lookup

	// Returns 1 if key present, 0 if not.
	[[nodiscard]]
	size_type count(key_type key) const noexcept;

	// Returns true if key present.
	[[nodiscard]]
	bool contains(key_type key) const noexcept;

	// Find the iterator for a key. Returns end() if not present.
	[[nodiscard]]
	const_iterator find(key_type key) const noexcept;

	// Access the underlying lookup data.
	// Vector of "bool" chars.
	[[nodiscard]]
	const uint8_t* lookup_data() const noexcept;

	// Access the underlying lookup data size.
	[[nodiscard]]
	size_type lookup_size() const noexcept;

private:
	// Stores true at [key] if the key is contained.
	bool_lookup_t _lookup{};
	size_type _size = 0;
};
} // namespace fea


// Implementation
namespace fea {
template <class MySet>
struct uss_const_iterator {
	// Typedefs
	using difference_type = typename MySet::difference_type;
	using key_type = typename MySet::key_type;
	using value_type = key_type;
	using pointer = void;
	using reference = key_type;
	using iterator_category = std::bidirectional_iterator_tag;

	// Internals.
	using bool_const_iterator = typename MySet::bool_const_iterator;

	// Ctors
	constexpr uss_const_iterator() noexcept = default;
	~uss_const_iterator() noexcept = default;
	constexpr uss_const_iterator(const uss_const_iterator&) noexcept = default;
	constexpr uss_const_iterator(uss_const_iterator&&) noexcept = default;
	constexpr uss_const_iterator& operator=(const uss_const_iterator&) noexcept
			= default;
	constexpr uss_const_iterator& operator=(uss_const_iterator&&) noexcept
			= default;

	// Returns a constructed key.

	[[nodiscard]]
	constexpr key_type operator*() const noexcept {
		assert(*_current);
		return key_type(std::distance(_first, _current));
	}

	// Unavailable, keys aren't actually stored in container.
	[[nodiscard]]
	constexpr key_type operator->() const noexcept
			= delete;

	// Pre-fix ++operator.
	constexpr uss_const_iterator& operator++() noexcept {
		assert(_current != _last);
		do {
			++_current;
		} while (_current != _last && !*_current);
		return *this;
	}

	// Post-fix operator++.
	constexpr uss_const_iterator& operator++(int) noexcept {
		uss_const_iterator tmp = *this;
		++*this;
		return tmp;
	}

	// Pre-fix --operator.
	constexpr uss_const_iterator& operator--() noexcept {
		assert(_current != _first);
		do {
			--_current;
		} while (!*_current && _current != _first);
		return *this;
	}

	// Post-fix operator--.
	constexpr uss_const_iterator& operator--(int) noexcept {
		uss_const_iterator tmp = *this;
		--*this;
		return tmp;
	}

	// Comparison.
	[[nodiscard]]
	bool operator==(const uss_const_iterator& rhs) const noexcept {
		return _current == rhs._current;
	}

	// Comparison.
	[[nodiscard]]
	bool operator!=(const uss_const_iterator& rhs) const noexcept {
		return !(*this == rhs);
	}

	// Comparison.
	[[nodiscard]]
	bool operator<(const uss_const_iterator& rhs) const noexcept {
		return _current < rhs._current;
	}

	// Comparison.
	[[nodiscard]]
	bool operator>(const uss_const_iterator& rhs) const noexcept {
		return rhs < *this;
	}

	// Comparison.
	[[nodiscard]]
	bool operator<=(const uss_const_iterator& rhs) const noexcept {
		return !(rhs < *this);
	}

	// Comparison.
	[[nodiscard]]
	bool operator>=(const uss_const_iterator& rhs) const noexcept {
		return !(*this < rhs);
	}

protected:
	friend MySet;

	constexpr uss_const_iterator(bool_const_iterator first,
			bool_const_iterator last, bool_const_iterator ptr) noexcept
			: _first(first)
			, _last(last)
			, _current(ptr) {
	}

	bool_const_iterator _first = {};
	bool_const_iterator _last = {};
	bool_const_iterator _current = {};
};


template <class Key, class Alloc>
template <class FwdIt>
unsigned_slotset<Key, Alloc>::unsigned_slotset(FwdIt first, FwdIt last) {
	insert(first, last);
}

template <class Key, class Alloc>
unsigned_slotset<Key, Alloc>::unsigned_slotset(
		std::initializer_list<key_type>&& ilist) {
	unsigned_slotset(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::begin() const noexcept -> const_iterator {
	auto ret
			= const_iterator{ _lookup.begin(), _lookup.end(), _lookup.begin() };
	if (!_lookup.empty() && !_lookup.front()) {
		++ret;
	}
	return ret;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cbegin() const noexcept -> const_iterator {
	return begin();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::end() const noexcept -> const_iterator {
	return const_iterator{ _lookup.begin(), _lookup.end(), _lookup.end() };
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::cend() const noexcept -> const_iterator {
	return end();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::empty() const noexcept -> bool {
	assert(std::none_of(_lookup.begin(), _lookup.end(),
			[this](bool_type v) { return _size == 0 && v; }));
	return _size == size_type(0);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::size() const noexcept -> size_type {
	assert(size_type(std::count(_lookup.begin(), _lookup.end(), uint8_t(true)))
			== _size);
	return _size;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::max_size() const noexcept -> size_type {
	return _lookup.max_size();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::reserve(key_type key) -> void {
	size_type new_cap = size_type(key) + size_type(1);
	_lookup.reserve(new_cap);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::capacity() const noexcept -> size_type {
	return _lookup.capacity();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::shrink_to_fit() -> void {
	_lookup.shrink_to_fit();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::clear() noexcept -> void {
	_lookup.clear();
	_size = 0;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::insert(key_type key)
		-> std::pair<const_iterator, bool> {
	const_iterator it = find(key);
	if (it != end()) {
		return { it, false };
	}

	size_type idx = size_type(key);
	if (idx >= _lookup.size()) {
		_lookup.resize(idx + size_type(1));
	}
	_lookup[idx] = uint8_t(true);
	++_size;

	return {
		const_iterator{ _lookup.begin(), _lookup.end(), _lookup.begin() + idx },
		true,
	};
}

template <class Key, class Alloc>
template <class FwdIt>
auto unsigned_slotset<Key, Alloc>::insert(FwdIt first, FwdIt last) -> void {
	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	static_assert(std::is_same_v<value_t, key_type>,
			"unsigned_set : Invalid iterators, do not point to key_type.");
	if (std::distance(first, last) == 0) {
		return;
	}

	// Allocate once. Find max key to do so.
	FwdIt max_it = std::max_element(
			first, last, [](key_type lhs, key_type rhs) { return lhs < rhs; });

	size_type max_key_idx = size_type(*max_it);
	if (max_key_idx >= _lookup.size()) {
		_lookup.resize(max_key_idx + size_type(1));
	}

	// Fill the set.
	for (FwdIt it = first; it != last; ++it) {
		size_type idx = size_type(*it);
		if (!_lookup[idx]) {
			_lookup[idx] = uint8_t(true);
			++_size;
		}
	}
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::insert(
		std::initializer_list<key_type>&& ilist) -> void {
	insert(ilist.begin(), ilist.end());
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(key_type key) noexcept -> size_type {
	size_type idx = size_type(key);
	if (idx >= _lookup.size() || !_lookup[idx]) {
		return size_type(0);
	}
	_lookup[idx] = uint8_t(false);
	--_size;
	return size_type(1);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(const_iterator cit) noexcept
		-> const_iterator {
	if (cit != end() && *cit._current) {
		*const_cast<bool_type*>(std::addressof(*cit._current)) = uint8_t(false);
		--_size;
		++cit;
	}
	return cit;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::erase(const_iterator cfirst,
		const_iterator clast) noexcept -> const_iterator {
	if (cfirst == clast) {
		return clast;
	}

	auto it = cfirst;
	for (; it != clast; ++it) {
		erase(*it);
	}
	if (it != cend()) {
		++it;
	}
	return it;
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::swap(unsigned_slotset& other) noexcept
		-> void {
	_lookup.swap(other._lookup);
	std::swap(_size, other._size);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset& source) -> void {
	if (source._lookup.size() > _lookup.size()) {
		_lookup.resize(source._lookup.size());
	}

	assert(_lookup.size() >= source._lookup.size());
	size_type size = (std::min)(_lookup.size(), source._lookup.size());
	for (size_type i = 0; i < size; ++i) {
		if (_lookup[i]) {
			continue;
		}
		if (source._lookup[i]) {
			source._lookup[i] = uint8_t(false);
			--source._size;
			_lookup[i] = uint8_t(true);
			++_size;
		}
	}
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::merge(unsigned_slotset&& source) -> void {
	merge(source);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::count(key_type key) const noexcept
		-> size_type {
	return size_type(contains(key));
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::contains(key_type key) const noexcept
		-> bool {
	size_type idx = size_type(key);
	if (idx >= _lookup.size()) {
		return false;
	}
	return bool(_lookup[idx]);
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::find(key_type key) const noexcept
		-> const_iterator {
	size_type idx = size_type(key);
	if (idx >= _lookup.size() || !_lookup[idx]) {
		return end();
	}
	return const_iterator{ _lookup.begin(), _lookup.end(),
		_lookup.begin() + idx };
}

template <class Key, class Alloc>
const uint8_t* unsigned_slotset<Key, Alloc>::lookup_data() const noexcept {
	return _lookup.data();
}

template <class Key, class Alloc>
auto unsigned_slotset<Key, Alloc>::lookup_size() const noexcept -> size_type {
	return _lookup.size();
}

} // namespace fea
