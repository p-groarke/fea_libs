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
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>
// To add custom id classes, in the fea namespace, define a struct
// specialization to `id_hash` which returns your id type through operator().
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


	// Ctors
	explicit unsigned_map(size_t new_cap);
	explicit unsigned_map(size_t key_new_cap, size_t value_new_cap);
	template <class InputIt>
	unsigned_map(InputIt first, InputIt last);
	explicit unsigned_map(const std::initializer_list<value_type>& init);

	unsigned_map() = default;
	~unsigned_map() = default;
	unsigned_map(const unsigned_map&) = default;
	unsigned_map(unsigned_map&&) noexcept = default;
	unsigned_map& operator=(const unsigned_map&) = default;
	unsigned_map& operator=(unsigned_map&&) noexcept = default;


	// Iterators

	// Returns a pair iterator to the first item.
	iterator begin() noexcept;

	// Returns a pair iterator to the first item.
	const_iterator begin() const noexcept;

	// Returns a pair iterator to the first item.
	const_iterator cbegin() const noexcept;

	// Returns a pair iterator past the last item.
	iterator end() noexcept;

	// Returns a pair iterator past the last item.
	const_iterator end() const noexcept;

	// Returns a pair iterator past the last item.
	const_iterator cend() const noexcept;


	// Capacity

	// checks whether the container is empty
	bool empty() const noexcept;

	// returns the number of elements
	size_type size() const noexcept;

	// returns the maximum possible number of elements
	size_type max_size() const noexcept;

	// Reserves storage.
	void reserve(size_type new_cap);

	// Precisely reserves storage, with different sizes for lookup and value
	// storage.
	void reserve(size_type key_new_cap, size_type value_new_cap);

	// returns the number of elements that can be held in currently allocated
	// storage
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
	void swap(unsigned_map& other) noexcept;


	// Lookup

	// Direct access to the underlying vector. Returns pointer to pairs.
	const value_type* data() const noexcept;

	// Direct access to the underlying vector. Returns pointer to pairs.
	value_type* data() noexcept;

	// Access specified element with bounds checking.
	const mapped_type& at(const key_type& k) const;

	// Access specified element with bounds checking.
	mapped_type& at(const key_type& k);

	// Access specified element without any bounds checking.
	const mapped_type& at_unchecked(const key_type& k) const noexcept;

	// Access specified element without any bounds checking.
	mapped_type& at_unchecked(const key_type& k) noexcept;

	// Access or insert specified element.
	mapped_type& operator[](const key_type& k);

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(const key_type& k) const noexcept;

	// Finds element at key. Returns end() if none found.
	const_iterator find(const key_type& k) const noexcept;

	// Finds element at key. Returns end() if none found.
	iterator find(const key_type& k) noexcept;

	// checks if the container contains element with specific key
	bool contains(const key_type& k) const noexcept;

	// Returns range of elements matching a specific key (in this case, 1 or 0
	// elements).
	std::pair<const_iterator, const_iterator> equal_range(
			const key_type& k) const noexcept;

	// Returns range of elements matching a specific key (in this case, 1 or 0
	// elements).
	std::pair<iterator, iterator> equal_range(const key_type& k) noexcept;


	// Non-member functions

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator==(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);

	// Deep comparison.
	template <class K, class U, class A>
	friend bool operator!=(
			const unsigned_map<K, U, A>& lhs, const unsigned_map<K, U, A>& rhs);

private:
	template <class M>
	std::pair<iterator, bool> minsert(
			const key_type& k, M&& obj, bool assign_found = false);

	fea::id_lookup<Key, allocator_type> _lookup; // key -> position
	std::vector<value_type, allocator_type> _values; // pair with reverse_lookup
};
} // namespace fea

#include "unsigned_map.imp.hpp"
