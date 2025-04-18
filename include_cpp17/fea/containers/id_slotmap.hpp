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
#include "fea/containers/id_slot_lookup.hpp"
#include "fea/memory/memory.hpp"
#include "fea/utils/throw.hpp"

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

/*
fea::id_slotmap is an id slot map that grows as big as the biggest id. It's apis
and iterators use key-value pairs to make it swappable with unordered_map.

To add custom id classes, in the fea namespace, define a struct
specialization to `id_hash` which returns your id type through operator().
Important : The return type must be an unsigned, but unlike std::hash,
shouldn't necessarily be of size_t. The hash return type affects memory used.

Notes :
- The container doesn't use const key_type& in apis, it uses key_type. The
value of a key will always be smaller or equally sized to a reference.
- Doesn't provide hint apis.
*/

namespace fea {
template <class Key, class T, class Alloc = std::allocator<T>>
struct id_slotmap {
	// Typedefs
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

	// TODO : const key type
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


	// Ctors
	explicit id_slotmap(size_t new_cap);
	explicit id_slotmap(size_t key_new_cap, size_t value_new_cap);
	template <class InputIt>
	id_slotmap(InputIt first, InputIt last);
	explicit id_slotmap(const std::initializer_list<value_type>& init);

	id_slotmap() = default;
	~id_slotmap() = default;
	id_slotmap(const id_slotmap&) = default;
	id_slotmap(id_slotmap&&) noexcept = default;
	id_slotmap& operator=(const id_slotmap&) = default;
	id_slotmap& operator=(id_slotmap&&) noexcept = default;


	// Iterators

	// Returns a pair iterator to the first item.
	[[nodiscard]]
	iterator begin() noexcept;

	// Returns a pair iterator to the first item.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Returns a pair iterator to the first item.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// Returns a pair iterator past the last item.
	[[nodiscard]]
	iterator end() noexcept;

	// Returns a pair iterator past the last item.
	[[nodiscard]]
	const_iterator end() const noexcept;

	// Returns a pair iterator past the last item.
	[[nodiscard]]
	const_iterator cend() const noexcept;


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

	// Reserves storage.
	void reserve(size_type new_cap);

	// Precisely reserves storage, with different sizes for lookup and value
	// storage.
	void reserve(size_type key_new_cap, size_type value_new_cap);

	// returns the number of elements that can be held in currently allocated
	// storage
	[[nodiscard]]
	size_type capacity() const noexcept;

	// reduces memory usage by freeing unused memory
	void shrink_to_fit();


	// Modifiers

	// clears the contents
	void clear() noexcept;

	// Inserts key value pair.
	std::pair<iterator, bool> insert(const value_type& value);

	// Inserts key value pair.
	std::pair<iterator, bool> insert(value_type&& value);

	// Inserts range of key value pairs.
	template <class InputIt>
	void insert(InputIt first, InputIt last);

	// Inserts multiple key value pairs.
	void insert(const std::initializer_list<value_type>& ilist);

	// inserts an element or assigns to the current element if the key already
	// exists
	template <class M>
	std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(const key_type& k, Args&&... args);

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args);

	// Erase element at position.
	iterator erase(const_iterator pos);

	// Erase element range.
	iterator erase(const_iterator first, const_iterator last);

	// Erase element at key.
	size_type erase(const key_type& k);

	// swaps the contents
	void swap(id_slotmap& other) noexcept;


	// Lookup

	// Direct access to the underlying vector. Returns pointer to pairs.
	[[nodiscard]]
	const value_type* data() const noexcept;

	// Direct access to the underlying vector. Returns pointer to pairs.
	[[nodiscard]]
	value_type* data() noexcept;

	// Access to underlying lookup.
	// Dereferencing this with key returns the index of
	// the associated value.
	[[nodiscard]]
	const auto* lookup_data() const noexcept;

	// Lookup size, != key/value size.
	[[nodiscard]]
	size_type lookup_size() const noexcept;

	// Access specified element with bounds checking.
	[[nodiscard]]
	const mapped_type& at(const key_type& k) const;

	// Access specified element with bounds checking.
	[[nodiscard]]
	mapped_type& at(const key_type& k);

	// Access specified element without any bounds checking.
	[[nodiscard]]
	const mapped_type& at_unchecked(const key_type& k) const noexcept;

	// Access specified element without any bounds checking.
	[[nodiscard]]
	mapped_type& at_unchecked(const key_type& k) noexcept;

	// Access or insert specified element.
	[[nodiscard]]
	mapped_type& operator[](const key_type& k);

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	[[nodiscard]]
	size_type count(const key_type& k) const noexcept;

	// Finds element at key. Returns end() if none found.
	[[nodiscard]]
	const_iterator find(const key_type& k) const noexcept;

	// Finds element at key. Returns end() if none found.
	[[nodiscard]]
	iterator find(const key_type& k) noexcept;

	// checks if the container contains element with specific key
	[[nodiscard]]
	bool contains(const key_type& k) const noexcept;

	// Returns range of elements matching a specific key (in this case, 1 or 0
	// elements).
	[[nodiscard]]
	std::pair<const_iterator, const_iterator> equal_range(
			const key_type& k) const noexcept;

	// Returns range of elements matching a specific key (in this case, 1 or 0
	// elements).
	[[nodiscard]]
	std::pair<iterator, iterator> equal_range(const key_type& k) noexcept;


	// Non-member functions

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator==(
			const id_slotmap<K, U, A>& lhs, const id_slotmap<K, U, A>& rhs);

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator!=(
			const id_slotmap<K, U, A>& lhs, const id_slotmap<K, U, A>& rhs);

private:
	template <class M>
	std::pair<iterator, bool> minsert(
			const key_type& k, M&& obj, bool assign_found = false);

	fea::id_slot_lookup<Key, allocator_type> _lookup; // key -> position
	std::vector<value_type, allocator_type> _values; // pair with reverse_lookup
};
} // namespace fea


// Implementation
namespace fea {
template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(size_t new_cap) {
	reserve(new_cap);
}

template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(
		size_t key_new_cap, size_t value_new_cap) {
	reserve(key_new_cap, value_new_cap);
}

template <class Key, class T, class Alloc>
template <class InputIt>
id_slotmap<Key, T, Alloc>::id_slotmap(InputIt first, InputIt last) {
	// TODO : benchmark and potentially optimize
	for (auto it = first; it != last; ++it) {
		insert(*it);
	}
}

template <class Key, class T, class Alloc>
id_slotmap<Key, T, Alloc>::id_slotmap(
		const std::initializer_list<value_type>& init) {
	// TODO : benchmark and potentially optimize
	for (const value_type& kv : init) {
		insert(kv);
	}
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T,
		Alloc>::begin() noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::begin() const noexcept {
	return _values.begin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::cbegin() const noexcept {
	return _values.cbegin();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T,
		Alloc>::end() noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::end() const noexcept {
	return _values.end();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::cend() const noexcept {
	return _values.cend();
}

template <class Key, class T, class Alloc>
bool id_slotmap<Key, T, Alloc>::empty() const noexcept {
	return _values.empty();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::size()
		const noexcept {
	return _values.size();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::max_size() const noexcept {
	return _lookup.max_size();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::reserve(size_type new_cap) {
	_lookup.reserve(new_cap);
	_values.reserve(new_cap);
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::reserve(
		size_type key_new_cap, size_type value_new_cap) {
	_lookup.reserve(key_new_cap);
	_values.reserve(value_new_cap);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::capacity() const noexcept {
	return _values.capacity();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::shrink_to_fit() {
	_lookup.shrink_to_fit();
	_values.shrink_to_fit();
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::clear() noexcept {
	_lookup.clear();
	_values.clear();
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert(const value_type& value) {
	return minsert(value.first, value.second);
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert(value_type&& value) {
	return minsert(value.first, fea::move_if_moveable(value.second));
}

template <class Key, class T, class Alloc>
template <class InputIt>
void id_slotmap<Key, T, Alloc>::insert(InputIt first, InputIt last) {
	// TODO : benchmark and potentially optimize
	for (auto it = first; it != last; ++it) {
		insert(*it);
	}
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::insert(
		const std::initializer_list<value_type>& ilist) {
	// TODO : benchmark and potentially optimize
	for (const value_type& kv : ilist) {
		insert(kv);
	}
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::insert_or_assign(const key_type& k, M&& obj) {
	return minsert(k, std::forward<M>(obj), true);
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::emplace(const key_type& k, Args&&... args) {
	iterator it = find(k);
	if (it != end()) {
		return { it, false };
	}

	_lookup.insert(k, _values.size());
	_values.emplace_back(k, std::forward<Args>(args)...);
	return { std::prev(_values.end()), true };
}

template <class Key, class T, class Alloc>
template <class... Args>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::try_emplace(const key_type& key, Args&&... args) {
	// Standard emplace behavior doesn't apply, always use try_emplace
	// behavior.
	return emplace(key, std::forward<Args>(args)...);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::erase(
		const_iterator pos) {
	size_t idx = std::distance(_values.cbegin(), pos);
	erase(pos->first);

	if (idx >= _values.size())
		return end();

	return find(_values[idx].first);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::erase(
		const_iterator first, const_iterator last) {
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

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::erase(
		const key_type& k) {
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

	*it = fea::move_if_moveable(_values.back());
	_values.pop_back();
	_lookup.update(last_key, new_idx);
	return 1;
}

template <class Key, class T, class Alloc>
void id_slotmap<Key, T, Alloc>::swap(id_slotmap& other) noexcept {
	_lookup.swap(other._lookup);
	_values.swap(other._values);
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::value_type* id_slotmap<Key, T,
		Alloc>::data() const noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::value_type* id_slotmap<Key, T,
		Alloc>::data() noexcept {
	return _values.data();
}

template <class Key, class T, class Alloc>
const auto* id_slotmap<Key, T, Alloc>::lookup_data() const noexcept {
	return _lookup.data();
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T,
		Alloc>::lookup_size() const noexcept {
	return _lookup.size();
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at(const key_type& k) const {
	auto it = find(k);
	if (it == end()) {
		fea::maybe_throw<std::out_of_range>(
				__FUNCTION__, __LINE__, "value doesn't exist");
	}
	return it->second;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T, Alloc>::at(
		const key_type& k) {
	return const_cast<mapped_type&>(
			static_cast<const id_slotmap*>(this)->at(k));
}

template <class Key, class T, class Alloc>
const typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at_unchecked(const key_type& k) const noexcept {
	return _values[_lookup.at_unchecked(k)].second;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::at_unchecked(const key_type& k) noexcept {
	return const_cast<mapped_type&>(
			static_cast<const id_slotmap*>(this)->at_unchecked(k));
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::mapped_type& id_slotmap<Key, T,
		Alloc>::operator[](const key_type& k) {
	if (!contains(k)) {
		emplace(k, mapped_type{});
	}

	return at_unchecked(k);
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::size_type id_slotmap<Key, T, Alloc>::count(
		const key_type& k) const noexcept {
	return contains(k) ? 1 : 0;
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::const_iterator id_slotmap<Key, T,
		Alloc>::find(const key_type& k) const noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
typename id_slotmap<Key, T, Alloc>::iterator id_slotmap<Key, T, Alloc>::find(
		const key_type& k) noexcept {
	return begin() + _lookup.find(k, size());
}

template <class Key, class T, class Alloc>
bool id_slotmap<Key, T, Alloc>::contains(const key_type& k) const noexcept {
	return _lookup.contains(k);
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::const_iterator,
		typename id_slotmap<Key, T, Alloc>::const_iterator>
id_slotmap<Key, T, Alloc>::equal_range(const key_type& k) const noexcept {
	const_iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator,
		typename id_slotmap<Key, T, Alloc>::iterator>
id_slotmap<Key, T, Alloc>::equal_range(const key_type& k) noexcept {
	iterator it = find(k);
	if (it == end()) {
		return { it, it };
	}
	return { it, it + 1 };
}

template <class Key, class T, class Alloc>
template <class M>
std::pair<typename id_slotmap<Key, T, Alloc>::iterator, bool> id_slotmap<Key, T,
		Alloc>::minsert(const key_type& k, M&& obj, bool assign_found) {
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

template <class Key, class T, class Alloc>
[[nodiscard]]
bool operator==(const id_slotmap<Key, T, Alloc>& lhs,
		const id_slotmap<Key, T, Alloc>& rhs) {
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
[[nodiscard]]
bool operator!=(const id_slotmap<Key, T, Alloc>& lhs,
		const id_slotmap<Key, T, Alloc>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea

namespace std {
template <class Key, class T, class Alloc>
inline void swap(fea::id_slotmap<Key, T, Alloc>& lhs,
		fea::id_slotmap<Key, T, Alloc>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std
