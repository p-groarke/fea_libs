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
#include "fea/maps/details/unsigned_lookup.hpp"
#include "fea/maps/id_getter.hpp"
#include "fea/memory/memory.hpp"
#include "fea/utils/throw.hpp"

#include <cassert>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

// Notes :
// - The container doesn't use const key_type& in apis, it uses key_type. The
// value of a key will always be smaller or equally sized to a reference.
// - Doesn't provide hint apis.

namespace fea {
template <class Key, class T, class Alloc = std::allocator<T>>
struct flat_unsigned_map {
	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = T;
	using value_type = mapped_type;
	using iter_value_type = mapped_type;
	using size_type = std::size_t;
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

	using key_iterator =
			typename std::vector<key_type, key_allocator_type>::iterator;
	using const_key_iterator =
			typename std::vector<key_type, key_allocator_type>::const_iterator;

	// Constructors, destructors and assignement

	flat_unsigned_map() = default;
	flat_unsigned_map(const flat_unsigned_map&) = default;
	flat_unsigned_map(flat_unsigned_map&&) noexcept = default;
	flat_unsigned_map& operator=(const flat_unsigned_map&) = default;
	flat_unsigned_map& operator=(flat_unsigned_map&&) noexcept = default;

	explicit flat_unsigned_map(size_type reserve_count) {
		reserve(reserve_count);
	}
	explicit flat_unsigned_map(
			size_type key_reserve_count, size_type value_reserve_count) {
		reserve(key_reserve_count, value_reserve_count);
	}

	template <class InputIt, class InputValIt>
	flat_unsigned_map(InputIt first_key, InputIt last_key, InputValIt first_val,
			InputValIt last_val) {
		if (std::distance(first_key, last_key)
				!= std::distance(first_val, last_val)) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"mismatch amount of keys and values");
		}

		// TODO : benchmark and potentially optimize
		auto val_it = first_val;
		for (auto it = first_key; it != last_key; ++it) {
			insert(*it, *val_it);
			++val_it;
		}
		assert(_values.size() == _reverse_lookup.size());
	}

	explicit flat_unsigned_map(const std::initializer_list<key_type>& keys,
			const std::initializer_list<value_type>& values) {
		if (keys.size() != values.size()) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"mismatch amount of keys and values");
		}

		// TODO : benchmark and potentially optimize
		auto val_it = values.begin();
		for (auto it = keys.begin(); it != keys.end(); ++it) {
			insert(*it, *val_it);
			++val_it;
		}
		assert(_values.size() == _reverse_lookup.size());
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

	// returns an iterator to the beginning keys
	const_key_iterator key_begin() const noexcept {
		return _reverse_lookup.begin();
	}
	const_key_iterator key_cbegin() const noexcept {
		return _reverse_lookup.cbegin();
	}

	// returns an iterator to the end (one past last)
	const_key_iterator key_end() const noexcept {
		return _reverse_lookup.end();
	}
	const_key_iterator key_cend() const noexcept {
		return _reverse_lookup.cend();
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
		return _lookup.max_size();
	}

	// reserves storage
	void reserve(size_type new_cap) {
		_lookup.reserve(new_cap);
		_reverse_lookup.reserve(new_cap);
		_values.reserve(new_cap);
	}
	void reserve(size_type key_new_cap, size_type value_new_cap) {
		_lookup.reserve(key_new_cap);
		_reverse_lookup.reserve(value_new_cap);
		_values.reserve(value_new_cap);
	}

	// returns the number of elements that can be held in currently allocated
	// storage
	size_type capacity() const noexcept {
		return _values.capacity();
	}

	// reduces memory usage by freeing unused memory
	void shrink_to_fit() {
		_lookup.shrink_to_fit();
		_reverse_lookup.shrink_to_fit();
		_values.shrink_to_fit();
	}

	// Modifiers

	// clears the contents
	void clear() noexcept {
		_lookup.clear();
		_reverse_lookup.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(const key_type& k, const value_type& v) {
		return minsert(k, v);
	}
	std::pair<iterator, bool> insert(key_type&& k, value_type&& v) {
		return minsert(k, fea::maybe_move(v));
	}
	template <class KeyIt, class ValIt>
	void insert(
			KeyIt first_key, KeyIt last_key, ValIt first_val, ValIt last_val) {
		if (std::distance(first_key, last_key)
				!= std::distance(first_val, last_val)) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"mismatch amount of keys and values");
		}

		// TODO : benchmark and potentially optimize
		auto val_it = first_val;
		for (auto it = first_key; it != last_key; ++it) {
			insert(*it, *val_it);
			++val_it;
		}
		assert(_values.size() == _reverse_lookup.size());
	}
	void insert(const std::initializer_list<key_type>& keys,
			const std::initializer_list<value_type>& values) {
		if (keys.size() != values.size()) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"mismatch amount of keys and values");
		}

		// TODO : benchmark and potentially optimize
		auto val_it = values.begin();
		for (auto it = keys.begin(); it != keys.end(); ++it) {
			insert(*it, *val_it);
			++val_it;
		}
		assert(_values.size() == _reverse_lookup.size());
	}

	// inserts an element or assigns to the current element if the key already
	// exists
	template <class M>
	std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj) {
		return minsert(k, std::forward<M>(obj), true);
	}

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(const key_type& k, Args&&... args) {
		iterator it = find(k);
		if (it != end()) {
			return { it, false };
		}

		_lookup.insert(k, _values.size());
		_reverse_lookup.push_back(k);
		_values.emplace_back(std::forward<Args>(args)...);

		assert(_values.size() == _reverse_lookup.size());
		return { std::prev(_values.end()), true };
	}

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
		// Standard emplace behavior doesn't apply, always use try_emplace
		// behavior.
		return emplace(key, std::forward<Args>(args)...);
	}

	// erases elements
	iterator erase(const_iterator pos) {
		size_t idx = std::distance(_values.cbegin(), pos);
		erase(_reverse_lookup[idx]);
		assert(_values.size() == _reverse_lookup.size());

		if (idx >= _values.size()) {
			return end();
		}

		return _values.begin() + idx;
	}
	iterator erase(const_iterator first, const_iterator last) {
		size_t first_idx = std::distance(_values.cbegin(), first);
		size_t last_idx = std::distance(_values.cbegin(), last);
		assert(first_idx < _values.size());
		assert(last_idx <= _values.size());

		std::vector<key_type> to_erase;
		to_erase.reserve(last_idx - first_idx);
		for (auto it = _reverse_lookup.begin() + first_idx;
				it != _reverse_lookup.begin() + last_idx; ++it) {
			to_erase.push_back(*it);
		}

		for (key_type& k : to_erase) {
			erase(k);
		}

		assert(_values.size() == _reverse_lookup.size());
		if (first_idx >= _values.size()) {
			return end();
		}
		return _values.begin() + first_idx;
	}
	size_type erase(const key_type& k) {
		iterator it = find(k);
		if (it == end()) {
			return 0;
		}

		iterator last_val_it = std::prev(end());
		_lookup.invalidate(k);

		// No need for swap, object is already at end.
		if (last_val_it == it) {
			_values.pop_back();
			_reverse_lookup.pop_back();
			assert(_values.size() == _reverse_lookup.size());
			return 1;
		}

		// swap & pop
		size_type new_idx = size_type(std::distance(_values.begin(), it));
		key_iterator key_it = _reverse_lookup.begin() + new_idx;
		key_type last_key = _reverse_lookup.back();

		*it = fea::maybe_move(_values.back());
		*key_it = _reverse_lookup.back();
		_values.pop_back();
		_reverse_lookup.pop_back();
		_lookup.update(last_key, new_idx);
		assert(_values.size() == _reverse_lookup.size());
		return 1;
	}

	// swaps the contents
	void swap(flat_unsigned_map& other) noexcept {
		_lookup.swap(other._lookup);
		_reverse_lookup.swap(other._reverse_lookup);
		_values.swap(other._values);
	}


	// Lookup
	// Direct access to the underlying value data.
	const value_type* data() const noexcept {
		return _values.data();
	}
	value_type* data() noexcept {
		return _values.data();
	}

	// Access to underlying reverse lookup.
	// That is, keys ordered in the same order as values.
	const key_type* key_data() const noexcept {
		return _reverse_lookup.data();
	}

	// Access to underlying lookup.
	// Dereferencing this with key returns the index of
	// the associated value.
	const auto* lookup_data() const noexcept {
		return _lookup.data();
	}

	// Lookup size, != key/value size.
	size_type lookup_size() const noexcept {
		return _lookup.size();
	}

	// access specified element with bounds checking
	const mapped_type& at(const key_type& k) const {
		auto it = find(k);
		if (it == end()) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "key doesn't exist");
		}
		return *it;
	}
	mapped_type& at(const key_type& k) {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_map*>(this)->at(k));
	}

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(const key_type& k) const noexcept {
		return _values[_lookup.at_unchecked(k)];
	}
	mapped_type& at_unchecked(const key_type& k) noexcept {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_map*>(this)->at_unchecked(k));
	}

	// access or insert specified element
	mapped_type& operator[](const key_type& k) {
		if (!contains(k)) {
			emplace(k, mapped_type{});
		}

		return at_unchecked(k);
	}

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(const key_type& k) const noexcept {
		return contains(k) ? 1 : 0;
	}

	// finds element with specific key
	const_iterator find(const key_type& k) const noexcept {
		return begin() + _lookup.find(k, size());
	}
	iterator find(const key_type& k) noexcept {
		return begin() + _lookup.find(k, size());
	}

	// checks if the container contains element with specific key
	bool contains(const key_type& k) const noexcept {
		return _lookup.contains(k);
	}

	// returns range of elements matching a specific key (in this case, 1 or 0
	// elements)
	std::pair<const_iterator, const_iterator> equal_range(
			const key_type& k) const noexcept {
		const_iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, it + 1 };
	}
	std::pair<iterator, iterator> equal_range(const key_type& k) noexcept {
		iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, it + 1 };
	}


	// Non-member functions

	//	compares the values in the unordered_map
	template <class K, class U, class A>
	friend bool operator==(const flat_unsigned_map<K, U, A>& lhs,
			const flat_unsigned_map<K, U, A>& rhs);
	template <class K, class U, class A>
	friend bool operator!=(const flat_unsigned_map<K, U, A>& lhs,
			const flat_unsigned_map<K, U, A>& rhs);

private:
	template <class M>
	std::pair<iterator, bool> minsert(
			const key_type& k, M&& obj, bool assign_found = false) {
		iterator it = find(k);
		if (it != end()) {
			if (assign_found) {
				*it = std::forward<M>(obj);
			}
			return { it, false };
		}

		_lookup.insert(k, _values.size());
		_reverse_lookup.push_back(k);
		_values.push_back(std::forward<M>(obj));
		assert(_values.size() == _reverse_lookup.size());
		return { std::prev(_values.end()), true };
	}


	detail::unsigned_lookup<Key, allocator_type> _lookup; // key -> position
	std::vector<key_type, key_allocator_type> _reverse_lookup; // used in erase
	std::vector<value_type, allocator_type> _values; // packed values
};

template <class Key, class T, class Alloc>
bool operator==(const flat_unsigned_map<Key, T, Alloc>& lhs,
		const flat_unsigned_map<Key, T, Alloc>& rhs) {
	if (lhs.size() != rhs.size())
		return false;


	for (auto it = lhs.key_begin(); it != lhs.key_end(); ++it) {
		// Key doesn't exist in rhs, not equal.
		if (!rhs.contains(*it))
			return false;

		// Compare value.
		if (!(lhs.at_unchecked(*it) == rhs.at_unchecked(*it)))
			return false;
	}

	return true;
}
template <class Key, class T, class Alloc>
bool operator!=(const flat_unsigned_map<Key, T, Alloc>& lhs,
		const flat_unsigned_map<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}

} // namespace fea

namespace std {
template <class Key, class T, class Alloc>
void swap(fea::flat_unsigned_map<Key, T, Alloc>& lhs,
		fea::flat_unsigned_map<Key, T, Alloc>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std
