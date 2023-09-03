/*
BSD 3-Clause License

Copyright (c) 2023, Philippe Groarke
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
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>
// To add custom id classes, in the fea namespace, define a struct
// specialization to `id_getter` which returns your id type through operator().
// Important : The return type must be an unsigned, but unlike std::hash,
// shouldn't necessarily be of size_t.

// Notes :
// - The container doesn't use const key_type& in apis, it uses key_type. The
// value of a key will always be smaller or equally sized to a reference.
// - Doesn't provide hint apis.

namespace fea {
// An id map (indexed at key), which grows as large as biggest stored n.
// Very fast since there is no hashing or collisions, but trades-off
// memory pressure.
template <class Key, class T, class Alloc = std::allocator<T>>
struct unsigned_map {
	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = T;
	using value_type = std::pair<key_type, mapped_type>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using allocator_type = typename std::allocator_traits<
			Alloc>::template rebind_alloc<value_type>;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	// TODO : cont key type
	using iter_value_type = std::pair<const_key_type, mapped_type>;
	using iter_allocator_type = typename std::allocator_traits<
			Alloc>::template rebind_alloc<iter_value_type>;

	using iterator = typename std::vector<value_type, allocator_type>::iterator;
	using const_iterator =
			typename std::vector<value_type, allocator_type>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	// Don't make sense
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;


	// Constructors, destructors and assignement
	unsigned_map() = default;
	~unsigned_map() = default;
	unsigned_map(const unsigned_map&) = default;
	unsigned_map(unsigned_map&&) noexcept = default;
	unsigned_map& operator=(const unsigned_map&) = default;
	unsigned_map& operator=(unsigned_map&&) noexcept = default;

	explicit unsigned_map(size_t new_cap) {
		reserve(new_cap);
	}
	explicit unsigned_map(size_t key_new_cap, size_t value_new_cap) {
		reserve(key_new_cap, value_new_cap);
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
		return _lookup.max_size();
	}

	// reserves storage
	void reserve(size_type new_cap) {
		_lookup.reserve(new_cap);
		_values.reserve(new_cap);
	}
	void reserve(size_type key_new_cap, size_type value_new_cap) {
		_lookup.reserve(key_new_cap);
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
		_values.shrink_to_fit();
	}


	// Modifiers

	// clears the contents
	void clear() noexcept {
		_lookup.clear();
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
		_values.emplace_back(k, std::forward<Args>(args)...);
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
	size_type erase(const key_type& k) {
		iterator it = find(k);
		if (it == end()) {
			return 0;
		}

		_lookup.invalidate(k);
		iterator last_it = std::prev(end());

		// No need to swap if object is already at end.
		if (last_it == it) {
			_values.pop_back();
			return 1;
		}

		// swap & pop
		size_type new_idx = size_type(std::distance(_values.begin(), it));
		key_type last_key = _values.back().first;

		*it = fea::maybe_move(_values.back());
		_values.pop_back();
		_lookup.update(last_key, new_idx);
		return 1;
	}

	// swaps the contents
	void swap(unsigned_map& other) noexcept {
		_lookup.swap(other._lookup);
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
	const mapped_type& at(const key_type& k) const {
		auto it = find(k);
		if (it == end()) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "value doesn't exist");
		}
		return it->second;
	}
	mapped_type& at(const key_type& k) {
		return const_cast<mapped_type&>(
				static_cast<const unsigned_map*>(this)->at(k));
	}

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(const key_type& k) const noexcept {
		return _values[_lookup.at_unchecked(k)].second;
	}
	mapped_type& at_unchecked(const key_type& k) noexcept {
		return const_cast<mapped_type&>(
				static_cast<const unsigned_map*>(this)->at_unchecked(k));
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
	friend bool operator==(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);
	template <class K, class U, class A>
	friend bool operator!=(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);

private:
	template <class M>
	std::pair<iterator, bool> minsert(
			const key_type& k, M&& obj, bool assign_found = false) {
		iterator it = find(k);
		if (it != end()) {
			if (assign_found) {
				it->second = std::forward<M>(obj);
			}
			return { it, false };
		}

		_lookup.insert(k, _values.size());
		_values.push_back({ k, std::forward<M>(obj) });
		return { std::prev(_values.end()), true };
	}


	detail::unsigned_lookup<Key, allocator_type> _lookup; // key -> position
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
		if (!(kv == *rhs.find(kv.first)))
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
