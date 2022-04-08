/*
BSD 3-Clause License

Copyright (c) 2022, Philippe Groarke
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
#include "fea/utils/throw.hpp"

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

// Notes :
// - The container doesn't use const key_type& in apis, it uses key_type. The
// value of a key will always be smaller or equally sized to a reference.
// - Doesn't provide hint apis.

namespace fea {
template <class Key, class T, class Alloc = std::allocator<T>>
struct unsigned_map {
	static_assert(std::is_unsigned<Key>::value,
			"unsigned_map : key must be unsigned integer");

	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = T;
	using value_type = std::pair<key_type, mapped_type>;
	using iter_value_type = std::pair<const_key_type, mapped_type>;
	using size_type = std::size_t;
	using pos_type = Key;
	using difference_type = std::ptrdiff_t;

	using allocator_type = typename std::allocator_traits<
			Alloc>::template rebind_alloc<value_type>;
	using pos_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<pos_type>;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	// TODO : cont key type
	using iterator = typename std::vector<value_type, allocator_type>::iterator;
	using const_iterator =
			typename std::vector<value_type, allocator_type>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	// Don't make sense
	// using hasher = std::hash<key_type>;
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;


	// Constructors, destructors and assignement

	unsigned_map() = default;
	unsigned_map(const unsigned_map&) = default;
	unsigned_map(unsigned_map&&) noexcept = default;
	unsigned_map& operator=(const unsigned_map&) = default;
	unsigned_map& operator=(unsigned_map&&) noexcept = default;

	explicit unsigned_map(size_t reserve_count) {
		reserve(reserve_count);
	}
	explicit unsigned_map(
			size_t key_reserve_count, size_t value_reserve_count) {
		_indexes.reserve(key_reserve_count);
		_values.reserve(value_reserve_count);
	}

	template <class InputIt>
	unsigned_map(InputIt first, InputIt last) {
		// TODO : benchmark and potentially optimize
		for (auto it = first; it != last; ++it) {
			insert(*it);
		}
	}

	explicit unsigned_map(const std::initializer_list<value_type>& init) {
		// TODO : benchmark and potentially optimize
		for (const value_type& kv : init) {
			insert(kv);
		}
	}


	// Iterators

	// returns an iterator to the beginning
	iterator begin() noexcept {
		return _values.begin();
	}
	const_iterator begin() const noexcept {
		return _values.begin();
	}
	const_iterator cbegin() const noexcept {
		return _values.cbegin();
	}

	// returns an iterator to the end (one past last)
	iterator end() noexcept {
		return _values.end();
	}
	const_iterator end() const noexcept {
		return _values.end();
	}
	const_iterator cend() const noexcept {
		return _values.cend();
	}


	// Capacity

	// checks whether the container is empty
	bool empty() const noexcept {
		return _values.empty();
	}

	// returns the number of elements
	size_type size() const noexcept {
		return _values.size();
	}

	// returns the maximum possible number of elements
	size_type max_size() const noexcept {
		// -1 due to sentinel
		return pos_sentinel() - 1;
	}

	// reserves storage
	void reserve(size_type new_cap) {
		_indexes.reserve(new_cap);
		_values.reserve(new_cap);
	}

	// returns the number of elements that can be held in currently allocated
	// storage
	size_type capacity() const noexcept {
		return _values.capacity();
	}

	// reduces memory usage by freeing unused memory
	void shrink_to_fit() {
		_indexes.shrink_to_fit();
		_values.shrink_to_fit();
	}

	// Modifiers

	// clears the contents
	void clear() noexcept {
		_indexes.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(const value_type& value) {
		return minsert(value.first, value.second);
	}
	std::pair<iterator, bool> insert(value_type&& value) {
		return minsert(value.first, fea::maybe_move(value.second));
	}
	template <class InputIt>
	void insert(InputIt first, InputIt last) {
		// TODO : benchmark and potentially optimize
		for (auto it = first; it != last; ++it) {
			insert(*it);
		}
	}
	void insert(const std::initializer_list<value_type>& ilist) {
		// TODO : benchmark and potentially optimize
		for (const value_type& kv : ilist) {
			insert(kv);
		}
	}

	// inserts an element or assigns to the current element if the key already
	// exists
	template <class M>
	std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
		return minsert(k, std::forward<M>(obj), true);
	}

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(key_type k, Args&&... args) {
		iterator it = find(k);
		if (it != end()) {
			return { it, false };
		}

		resize_indexes_if_needed(k);

		_indexes[k] = pos_type(_values.size());
		_values.emplace_back(k, std::forward<Args>(args)...);

		return { std::prev(_values.end()), true };
	}

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(key_type key, Args&&... args) {
		// Standard emplace behavior doesn't apply, always use try_emplace
		// behavior.
		return emplace(key, std::forward<Args>(args)...);
	}

	// erases elements
	iterator erase(const_iterator pos) {
		size_t idx = std::distance(_values.cbegin(), pos);
		erase(pos->first);

		if (idx >= _values.size())
			return end();

		return find(_values[idx].first);
	}
	iterator erase(const_iterator first, const_iterator last) {
		size_t first_idx = std::distance(_values.cbegin(), first);
		size_t last_idx = std::distance(_values.cbegin(), last);


		std::vector<key_type> to_erase;
		to_erase.reserve(last_idx - first_idx);
		for (auto it = _values.begin() + first_idx;
				it != _values.begin() + last_idx; ++it) {
			to_erase.push_back(it->first);
		}

		for (key_type& k : to_erase) {
			erase(k);
		}

		if (first_idx >= _values.size())
			return end();
		return find(_values[first_idx].first);
	}
	size_type erase(key_type k) {
		iterator it = find(k);
		if (it == end()) {
			return 0;
		}

		iterator last_it = std::prev(end());
		_indexes[k] = pos_sentinel();

		// No need for swap, object is already at end.
		if (last_it == it) {
			_values.pop_back();
			return 1;
		}

		// swap & pop
		pos_type value_idx = pos_type(std::distance(_values.begin(), it));
		key_type last_key = _values.back().first;

		*it = fea::maybe_move(_values.back());
		_values.pop_back();
		_indexes[last_key] = value_idx;

		return 1;
	}

	// swaps the contents
	void swap(unsigned_map& other) noexcept {
		_indexes.swap(other._indexes);
		_values.swap(other._values);
	}


	// Lookup
	// direct access to the underlying vector
	const value_type* data() const noexcept {
		return _values.data();
	}
	value_type* data() noexcept {
		return _values.data();
	}

	// access specified element with bounds checking
	const mapped_type& at(key_type k) const {
		if (!contains(k)) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "value doesn't exist");
		}

		return at_unchecked(k);
	}
	mapped_type& at(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const unsigned_map*>(this)->at(k));
	}

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(key_type k) const {
		return _values[_indexes[k]].second;
	}
	mapped_type& at_unchecked(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const unsigned_map*>(this)->at_unchecked(k));
	}

	// access or insert specified element
	mapped_type& operator[](key_type k) {
		if (!contains(k)) {
			emplace(k, mapped_type{});
		}

		return at_unchecked(k);
	}

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(key_type k) const {
		if (contains(k))
			return 1;

		return 0;
	}

	// finds element with specific key
	iterator find(key_type k) {
		if (!contains(k)) {
			return end();
		}

		return begin() + _indexes[k];
	}
	const_iterator find(key_type k) const {
		if (!contains(k)) {
			return end();
		}

		return begin() + _indexes[k];
	}

	// checks if the container contains element with specific key
	bool contains(key_type k) const {
		if (k >= _indexes.size())
			return false;

		if (_indexes[k] == pos_sentinel())
			return false;

		return true;
	}

	// returns range of elements matching a specific key (in this case, 1 or 0
	// elements)
	std::pair<iterator, iterator> equal_range(key_type k) {
		iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, it + 1 };
	}
	std::pair<const_iterator, const_iterator> equal_range(key_type k) const {
		const_iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, it + 1 };
	}


	// Non-member functions

	//	compares the values in the unordered_map
	template <class K, class U, class A>
	friend bool operator==(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);
	template <class K, class U, class A>
	friend bool operator!=(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);

private:
	constexpr pos_type pos_sentinel() const noexcept {
		return (std::numeric_limits<pos_type>::max)();
	}

	void resize_indexes_if_needed(key_type k) {
		if (k < _indexes.size()) {
			return;
		}

		if (k == pos_sentinel()) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "maximum size reached");
		}

		_indexes.resize(size_t(k) + 1u, pos_sentinel());
	}

	template <class M>
	std::pair<iterator, bool> minsert(
			key_type k, M&& obj, bool assign_found = false) {
		iterator it = find(k);
		if (it != end()) {
			if (assign_found) {
				it->second = std::forward<M>(obj);
			}
			return { it, false };
		}

		resize_indexes_if_needed(k);

		_indexes[k] = pos_type(_values.size());
		_values.push_back({ k, std::forward<M>(obj) });
		return { std::prev(_values.end()), true };
	}

	std::vector<pos_type, pos_allocator_type> _indexes; // key -> position
	std::vector<value_type, allocator_type> _values; // pair with reverse_lookup
};

template <class Key, class T, class Alloc>
inline bool operator==(const unsigned_map<Key, T, Alloc>& lhs,
		const unsigned_map<Key, T, Alloc>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

	for (const auto& kv : lhs) {
		// Key doesn't exist in rhs, not equal.
		if (!rhs.contains(kv.first))
			return false;

		// Compare value.
		if (kv != *rhs.find(kv.first))
			return false;
	}

	return true;
}
template <class Key, class T, class Alloc>
inline bool operator!=(const unsigned_map<Key, T, Alloc>& lhs,
		const unsigned_map<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}

} // namespace fea

namespace std {
template <class Key, class T, class Alloc>
inline void swap(fea::unsigned_map<Key, T, Alloc>& lhs,
		fea::unsigned_map<Key, T, Alloc>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std
