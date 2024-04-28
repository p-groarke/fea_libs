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
#include "fea/containers/id_lookup.hpp"
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
	// Typedefs
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

	// Ctors
	flat_unsigned_map() = default;
	~flat_unsigned_map() = default;
	flat_unsigned_map(const flat_unsigned_map&) = default;
	flat_unsigned_map(flat_unsigned_map&&) noexcept = default;
	flat_unsigned_map& operator=(const flat_unsigned_map&) = default;
	flat_unsigned_map& operator=(flat_unsigned_map&&) noexcept = default;

	explicit flat_unsigned_map(size_type reserve_count);
	explicit flat_unsigned_map(
			size_type key_reserve_count, size_type value_reserve_count);
	template <class InputIt, class InputValIt>
	flat_unsigned_map(InputIt first_key, InputIt last_key, InputValIt first_val,
			InputValIt last_val);
	explicit flat_unsigned_map(const std::initializer_list<key_type>& keys,
			const std::initializer_list<value_type>& values);


	// Iterators

	// returns an iterator to the beginning
	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	// returns an iterator to the end (one past last)
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	// returns an iterator to the beginning keys
	const_key_iterator key_begin() const noexcept;
	const_key_iterator key_cbegin() const noexcept;

	// returns an iterator to the end (one past last)
	const_key_iterator key_end() const noexcept;
	const_key_iterator key_cend() const noexcept;


	// Capacity

	// checks whether the container is empty
	bool empty() const noexcept;

	// returns the number of elements
	size_type size() const noexcept;

	// returns the maximum possible number of elements
	size_type max_size() const noexcept;

	// reserves storage
	void reserve(size_type new_cap);
	void reserve(size_type lookup_new_cap, size_type value_new_cap);

	// returns the number of elements that can be held in currently allocated
	// storage
	size_type lookup_capacity() const noexcept;

	// returns the number of elements that can be held in currently allocated
	// storage
	size_type capacity() const noexcept;

	// reduces memory usage by freeing unused memory
	void shrink_to_fit();

	// Modifiers

	// clears the contents
	void clear() noexcept;

	// inserts elements or nodes
	std::pair<iterator, bool> insert(const key_type& k, const value_type& v);
	std::pair<iterator, bool> insert(key_type&& k, value_type&& v);
	template <class KeyIt, class ValIt>
	void insert(
			KeyIt first_key, KeyIt last_key, ValIt first_val, ValIt last_val);
	void insert(const std::initializer_list<key_type>& keys,
			const std::initializer_list<value_type>& values);

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

	// erases elements
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);
	size_type erase(const key_type& k);

	// swaps the contents
	void swap(flat_unsigned_map& other) noexcept;


	// Lookup
	// Direct access to the underlying value data.
	const value_type* data() const noexcept;
	value_type* data() noexcept;

	// Access to underlying reverse lookup.
	// That is, keys ordered in the same order as values.
	const key_type* key_data() const noexcept;

	// Access to underlying lookup.
	// Dereferencing this with key returns the index of
	// the associated value.
	const auto* lookup_data() const noexcept;

	// Lookup size, != key/value size.
	size_type lookup_size() const noexcept;

	// access specified element with bounds checking
	const mapped_type& at(const key_type& k) const;
	mapped_type& at(const key_type& k);

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(const key_type& k) const noexcept;
	mapped_type& at_unchecked(const key_type& k) noexcept;

	// access or insert specified element
	mapped_type& operator[](const key_type& k);

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(const key_type& k) const noexcept;

	// finds element with specific key
	const_iterator find(const key_type& k) const noexcept;
	iterator find(const key_type& k) noexcept;

	// checks if the container contains element with specific key
	bool contains(const key_type& k) const noexcept;

	// returns range of elements matching a specific key (in this case, 1 or 0
	// elements)
	std::pair<const_iterator, const_iterator> equal_range(
			const key_type& k) const noexcept;
	std::pair<iterator, iterator> equal_range(const key_type& k) noexcept;


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
			const key_type& k, M&& obj, bool assign_found = false);


	fea::id_lookup<Key, allocator_type> _lookup; // key -> position
	std::vector<key_type, key_allocator_type> _reverse_lookup; // used in erase
	std::vector<value_type, allocator_type> _values; // packed values
};

} // namespace fea

#include "flat_unsigned_map.imp.hpp"
