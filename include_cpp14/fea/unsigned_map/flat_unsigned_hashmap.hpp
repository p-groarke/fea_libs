/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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
#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
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
namespace detail {
template <class T>
inline constexpr std::conditional_t<!std::is_move_constructible<T>::value
				&& std::is_copy_constructible<T>::value,
		const T&, T&&>
flathashmap_maybe_move(T& arg) noexcept {
	return std::move(arg);
}

template <class Func>
struct flathhashmap_on_exit {
	flathhashmap_on_exit(Func func)
			: _func(func) {
	}
	~flathhashmap_on_exit() {
		_func();
	}

private:
	Func _func;
};

template <class Func>
auto flathashmap_make_on_exit(Func func) {
	return flathhashmap_on_exit<Func>{ func };
}

// https://stackoverflow.com/questions/30052316/find-next-prime-number-algorithm
template <class T>
bool is_prime(T number) {
	if (number == 2 || number == 3)
		return true;

	if (number % 2 == 0 || number % 3 == 0)
		return false;

	T divisor = 6;
	while (divisor * divisor - 2 * divisor + 1 <= number) {

		if (number % (divisor - 1) == 0)
			return false;

		if (number % (divisor + 1) == 0)
			return false;

		divisor += 6;
	}

	return true;
}

template <class T>
T next_prime(T a) {
	switch (a) {
	case 7: {
		return 7;
	} break;
	case 14: {
		return 17;
	} break;
	case 34: {
		return 37;
	} break;
	case 74: {
		return 79;
	} break;
	case 158: {
		return 163;
	} break;
	case 326: {
		return 331;
	} break;
	case 662: {
		return 673;
	} break;
	case 1'346: {
		return 1361;
	} break;
	case 2'722: {
		return 2729;
	} break;
	case 5'458: {
		return 5471;
	} break;
	case 10'942: {
		return 10949;
	} break;
	case 21'898: {
		return 21911;
	} break;
	case 43'822: {
		return 43853;
	} break;
	case 87'706: {
		return 87719;
	} break;
	case 175'438: {
		return 175447;
	} break;
	case 350'894: {
		return 350899;
	} break;
	case 701'798: {
		return 701819;
	} break;
	case 1'403'638: {
		return 1403641;
	} break;
	case 2'807'282: {
		return 2807303;
	} break;
	case 5'614'606: {
		return 5614657;
	} break;
	default: {
		while (!is_prime(++a)) {
		}
		return a;
	} break;
	}
}
} // namespace detail


template <class Key, class T>
struct flat_unsigned_hashmap {
	static_assert(std::is_unsigned<Key>::value,
			"unsigned_map : key must be unsigned integer");

	using key_type = Key;
	using mapped_type = T;
	using value_type = mapped_type;
	using size_type = std::size_t;
	using idx_type =
			typename std::conditional<sizeof(key_type) <= sizeof(size_type),
					key_type, size_type>::type;
	using difference_type = std::ptrdiff_t;

	using allocator_type = typename std::vector<value_type>::allocator_type;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	using iterator = typename std::vector<value_type>::iterator;
	using const_iterator = typename std::vector<value_type>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	// Don't make sense
	// using hasher = std::hash<key_type>;
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;


	// Constructors, destructors and assignement

	flat_unsigned_hashmap() = default;
	flat_unsigned_hashmap(const flat_unsigned_hashmap&) = default;
	flat_unsigned_hashmap(flat_unsigned_hashmap&&) = default;
	flat_unsigned_hashmap& operator=(const flat_unsigned_hashmap&) = default;
	flat_unsigned_hashmap& operator=(flat_unsigned_hashmap&&) = default;

	explicit flat_unsigned_hashmap(size_t reserve_count)
			: flat_unsigned_hashmap() {
		_lookup.reserve(reserve_count);
		_reverse_lookup.reserve(reserve_count);
		_values.reserve(reserve_count);
	}
	explicit flat_unsigned_hashmap(
			size_t key_reserve_count, size_t value_reserve_count)
			: flat_unsigned_hashmap() {
		_lookup.reserve(key_reserve_count);
		_reverse_lookup.reserve(value_reserve_count);
		_values.reserve(value_reserve_count);
	}

	// todo : kv iterators
	// template <class InputIt>
	// flat_unsigned_hashmap(InputIt first, InputIt last)
	//		: flat_unsigned_hashmap() {
	//	// TODO : benchmark and potentially optimize
	//	for (auto it = first; it != last; ++it) {
	//		insert(*it);
	//	}
	//}

	explicit flat_unsigned_hashmap(
			const std::initializer_list<std::pair<key_type, value_type>>& init)
			: flat_unsigned_hashmap() {
		// TODO : benchmark and potentially optimize
		for (const std::pair<key_type, value_type>& kv : init) {
			insert(kv.first, kv.second);
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
		return idx_sentinel() - 1;
	}

	// reserves storage
	void reserve(size_type new_cap) {
		_lookup.reserve(new_cap);
		_reverse_lookup.reserve(new_cap);
		_values.reserve(new_cap);
	}

	// returns the number of elements that can be held in currently
	// allocated storage
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
		_hash_max = 0;
		_lookup.clear();
		_reverse_lookup.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(key_type key, const value_type& value) {
		return minsert(key, value);
	}
	std::pair<iterator, bool> insert(key_type key, value_type&& value) {
		return minsert(key, detail::flathashmap_maybe_move(value));
	}

	// todo : pair iterators
	// template <class InputIt>
	// void insert(InputIt first, InputIt last) {
	//	// TODO : benchmark and potentially optimize
	//	for (auto it = first; it != last; ++it) {
	//		insert(*it);
	//	}
	//}
	void insert(const std::initializer_list<std::pair<key_type, value_type>>&
					ilist) {
		// TODO : benchmark and potentially optimize
		for (const std::pair<key_type, value_type>& kv : ilist) {
			insert(kv.first, kv.second);
		}
	}

	// inserts an element or assigns to the current element if the key
	// already exists
	std::pair<iterator, bool> insert_or_assign(
			key_type key, const value_type& value) {
		return minsert(key, value, true);
	}
	std::pair<iterator, bool> insert_or_assign(
			key_type key, value_type&& value) {
		return minsert(key, detail::flathashmap_maybe_move(value), true);
	}

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(key_type key, Args&&... args) {
		// Standard emplace behavior doesn't apply. Use try_emplace.
		return try_emplace(key, std::forward<Args>(args)...);
	}

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(key_type key, Args&&... args) {
		if (load_factor() >= max_load_factor()) {
			size_type new_size = hash_max() * 2;
			rehash(new_size);
		}

		auto lookup_it = find_first_slot_or_hole(key);
		if (lookup_it == _lookup.end()) {
			// Need to grow _lookup for trailing collisions.
			size_type idx = _lookup.size();
			_lookup.resize(size_type(_lookup.size() * _lookup_trailing_amount));
			lookup_it = _lookup.begin() + idx;
		}

		if (lookup_it->idx != idx_sentinel()) {
			// Found valid key.
			return { _values.begin() + lookup_it->idx, false };
		}

		idx_type new_pos = idx_type(_values.size());
		_values.emplace_back(std::forward<Args>(args)...);
		_reverse_lookup.push_back(key);
		lookup_it->key = key;
		lookup_it->idx = new_pos;

		assert(_reverse_lookup.size() == _values.size());
		return { begin() + new_pos, true };
	}

	// erases elements
	void erase(const_iterator pos) {
		size_t idx = std::distance(_values.cbegin(), pos);
		erase(_reverse_lookup[idx]);
	}
	void erase(const_iterator first, const_iterator last) {
		size_t first_idx = std::distance(_values.cbegin(), first);
		size_t last_idx = std::distance(_values.cbegin(), last);

		std::vector<key_type> to_erase;
		to_erase.reserve(last_idx - first_idx);
		for (auto it = _reverse_lookup.begin() + first_idx;
				it != _reverse_lookup.begin() + last_idx; ++it) {
			to_erase.push_back(*it);
		}

		for (key_type& k : to_erase) {
			erase(k);
		}
	}
	size_type erase(key_type k) {
		auto lookup_it = find_first_slot_or_hole(k);
		if (lookup_it == _lookup.end()) {
			return 0;
		}

		if (lookup_it->idx == idx_sentinel()) {
			return 0;
		}

		auto e = detail::flathashmap_make_on_exit(
				[lookup_idx = std::distance(_lookup.begin(), lookup_it),
						this]() { repack_collisions(lookup_idx); });

		if (lookup_it->idx == _values.size() - 1) {
			// No need for swap, object is already at end.
			*lookup_it = {};
			_reverse_lookup.pop_back();
			_values.pop_back();
			assert(_values.size() == _reverse_lookup.size());

			return 1;
		}

		// todo : Better way than doing a key search? could be slow
		// if we store the index of the key lookup, rehash gets slower
		key_type last_key = _reverse_lookup.back();
		auto last_lookup_it = find_first_slot_or_hole(last_key);

		// set new pos on last element.
		last_lookup_it->idx = lookup_it->idx;

		// invalidate erased lookup
		*lookup_it = {};

		// "swap" the elements
		_values[last_lookup_it->idx]
				= detail::flathashmap_maybe_move(_values.back());
		_reverse_lookup[last_lookup_it->idx] = last_key;

		// delete last
		_values.pop_back();
		_reverse_lookup.pop_back();

		assert(_values.size() == _reverse_lookup.size());
		return 1;
	}

	// swaps the contents
	void swap(flat_unsigned_hashmap& other) noexcept {
		std::swap(_max_load_factor, other._max_load_factor);
		std::swap(_hash_max, other._hash_max);
		_lookup.swap(other._lookup);
		_reverse_lookup.swap(other._reverse_lookup);
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
		const_iterator it = find(k);
		if (it == end()) {
			throw std::out_of_range{ "unsigned_map : value doesn't exist" };
		}

		return *it;
	}
	mapped_type& at(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_hashmap*>(this)->at(k));
	}

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(key_type k) const {
		const_iterator it = find(k);
		return *it;
	}
	mapped_type& at_unchecked(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_hashmap*>(this)->at_unchecked(
						k));
	}

	// access or insert specified element
	mapped_type& operator[](key_type k) {
		iterator it = find(k);
		if (it != end()) {
			return *it;
		}

		return *insert(k, {}).first;
	}

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(key_type k) const {
		if (contains(k))
			return 1;

		return 0;
	}

	// finds element with specific key
	const_iterator find(key_type k) const {
		auto lookup_it = find_first_slot_or_hole(k);
		if (lookup_it == _lookup.end()) {
			return end();
		}

		if (lookup_it->idx == idx_sentinel()) {
			return end();
		}

		assert(lookup_it->key == k);
		assert(lookup_it->idx < _values.size());
		assert(lookup_it->idx < _reverse_lookup.size());

		return begin() + lookup_it->idx;
	}
	iterator find(key_type k) {
		auto const_it
				= static_cast<const flat_unsigned_hashmap*>(this)->find(k);

		// Convert to non-const iterator.
		return _values.erase(const_it, const_it);
	}

	// checks if the container contains element with specific key
	bool contains(key_type k) const {
		return find(k) != end();
	}


	// Hash policy

	// returns average number of elements per bucket
	float load_factor() const noexcept {
		size_type h_max = hash_max();
		if (h_max == 0) {
			return 2.f; // dummy value to trigger growth, must be > 1.f
		}
		return _values.size() / float(h_max);
	}

	float max_load_factor() const noexcept {
		return _max_load_factor;
	}
	void max_load_factor(float ml) noexcept {
		_max_load_factor = ml;
	}

	void rehash(size_type count) {
		if (count < init_count()) {
			count = init_count();
		} else {
			count = detail::next_prime(count);
		}
		assert(detail::is_prime(count));

		std::vector<lookup_data> new_lookup(count);

		for (const lookup_data& lookup : _lookup) {
			if (lookup.idx == idx_sentinel()) {
				continue;
			}

			// new lookup position
			size_type new_bucket_pos = key_to_index(lookup.key, count);
			auto it = find_first_hole(
					new_lookup.begin(), new_lookup.end(), new_bucket_pos);

			if (it == new_lookup.end()) {
				size_type idx = new_lookup.size();
				new_lookup.resize(size_type(idx * _lookup_trailing_amount));
				it = new_lookup.begin() + idx;
			}

			// creates new lookup, assigns the existing element pos
			it->key = lookup.key;
			it->idx = lookup.idx;
		}

		_lookup = std::move(new_lookup);
		_hash_max = count;
	}


	// Non-member functions

	//	compares the values in the unordered_map
	template <class K, class U>
	friend bool operator==(const flat_unsigned_hashmap<K, U>& lhs,
			const flat_unsigned_hashmap<K, U>& rhs);
	template <class K, class U>
	friend bool operator!=(const flat_unsigned_hashmap<K, U>& lhs,
			const flat_unsigned_hashmap<K, U>& rhs);

private:
	struct lookup_data {
		// The user provided key.
		key_type key = key_sentinel();

		// The index of the user data in the _values container.
		idx_type idx = idx_sentinel();
	};

	size_type hash_max() const {
		assert(detail::is_prime(_hash_max) || _hash_max == 0);
		return _hash_max;
		// return _lookup.size();
	}

	size_type key_to_index(key_type key) const {
		size_type ret = key_to_index(key, hash_max());
		assert(ret < _lookup.size());
		return ret;
	}
	static constexpr size_type key_to_index(key_type key, size_type h_max) {
		// return (size_type(key) % h_max) * 2;
		return size_type(key) % h_max;
	}

	static constexpr key_type key_sentinel() noexcept {
		return (std::numeric_limits<key_type>::max)();
	}
	static constexpr idx_type idx_sentinel() noexcept {
		return (std::numeric_limits<idx_type>::max)();
	}

	static constexpr size_type init_count() noexcept {
		// return 14; // 7 * 2, hash_max == 7
		return 7;
	}

	// Custom find_if.
	// todo : benchmark simd search
	template <class Iter, class Func>
	static auto find_slot(Iter start, Iter end, Func func) {
		for (auto it = start; it < end; ++it) {
			if (func(*it)) {
				return it;
			}
		}

		// Didn't find anything.
		return end;
	}

	// Returns lookup iterator to either the lookup if it exists, or the first
	// free slot.
	auto find_first_slot_or_hole(key_type key) const {
		if (hash_max() == 0) {
			return _lookup.end();
		}

		size_type search_pos = key_to_index(key);

		auto it = find_slot(_lookup.begin() + search_pos, _lookup.end(),
				[&](const lookup_data& search) {
					return search.key == key || search.idx == idx_sentinel();
				});

		return it;
	}
	auto find_first_slot_or_hole(key_type key) {
		auto const_it = static_cast<const flat_unsigned_hashmap*>(this)
								->find_first_slot_or_hole(key);

		if (const_it == _lookup.end()) {
			return _lookup.end();
		}

		// Convert to non-const iterator.
		return _lookup.erase(const_it, const_it);
	}

	// Find first free slot given a lookup idx.
	template <class Iter>
	static auto find_first_hole(Iter beg, Iter end, size_type bucket_idx) {

		// find first free slot
		auto it = find_slot(
				beg + bucket_idx, end, [](const lookup_data& search) {
					return search.idx == idx_sentinel();
				});

		assert(it != end);

		return it;
	}

	// Packs the collisions so all clashing keys are contigous.
	// This is necessary after erase since erase could create a hole, with a
	// collision left over after that whole. This would break the container
	// guarantee that all collisions are packed until the first hole.
	void repack_collisions(size_type hole_idx) {
		assert(hole_idx < _lookup.size());
		assert(_lookup[hole_idx].idx == idx_sentinel());

		size_type swap_left_idx = hole_idx;
		size_type swap_right_idx = hole_idx + 1;

		// Sort the collisions.
		// Do this until you find a hole. The container must guarantee
		// collisions are packed in a first serve manner.
		while (swap_right_idx < _lookup.size()) {
			if (_lookup[swap_right_idx].idx == idx_sentinel()) {
				// We are done, have reached the end of this collision "group".
				return;
			}

			// This tests if the right candidate collisions index is would be
			// "lost" when swapped with left hole.
			// Since the map stores and searches for collisions after the key,
			// this would break searches.
			size_type candidate_idx = key_to_index(_lookup[swap_right_idx].key);
			if (candidate_idx > swap_left_idx) {
				// Continue searching for swappable collisions.
				++swap_right_idx;
				continue;
			}

			lookup_data& right = _lookup[swap_right_idx];
			_lookup[swap_left_idx] = right;
			right = {}; // Invalidate in case it is the last.

			swap_left_idx = swap_right_idx;
			++swap_right_idx;
		}

		// Something went horribly wrong.
		assert(false);
	}

	template <class M>
	std::pair<iterator, bool> minsert(
			key_type key, M&& value, bool assign_found = false) {
		if (load_factor() >= max_load_factor()) {
			size_type new_size = hash_max() * 2;
			rehash(new_size);
		}

		auto lookup_it = find_first_slot_or_hole(key);
		if (lookup_it == _lookup.end()) {
			// Need to grow _lookup for trailing collisions.
			size_type idx = _lookup.size();
			_lookup.resize(size_type(idx * _lookup_trailing_amount));
			lookup_it = _lookup.begin() + idx;
		}

		if (lookup_it->idx != idx_sentinel()) {
			// Found valid key.

			auto data_it = _values.begin() + lookup_it->idx;
			if (assign_found) {
				*data_it = std::forward<M>(value);
			}
			return { data_it, false };
		}

		idx_type new_pos = idx_type(_values.size());
		_values.push_back(std::forward<M>(value));
		_reverse_lookup.push_back(key);
		lookup_it->key = key;
		lookup_it->idx = new_pos;

		assert(_reverse_lookup.size() == _values.size());
		assert(_values.size() < idx_sentinel()
				&& "container has reached max capacity");
		return { begin() + new_pos, true };
	}

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
	std::vector<lookup_data> _lookup;

	// Used in erase for swap & pop.
	std::vector<key_type> _reverse_lookup;

	// Packed user values.
	// Since this is a flat map, the values are tightly packed instead of in
	// pairs.
	// This means we cannot fulfill standard apis.
	// todo : make unsigned_hashmap for cases when you need pair iterators.
	std::vector<value_type> _values;

	// When the lookup collisions fill up the end of the lookup container, by
	// how much do we resize it?
	constexpr static double _lookup_trailing_amount = 1.25;
};

template <class Key, class T>
inline bool operator==(const flat_unsigned_hashmap<Key, T>& lhs,
		const flat_unsigned_hashmap<Key, T>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

	for (size_t i = 0; i < lhs.size(); ++i) {
		Key k = lhs._reverse_lookup[i];
		auto it = rhs.find(k);
		if (it == rhs.end()) {
			return false;
		}

		if (*it != lhs._values[i]) {
			return false;
		}
	}

	return true;
}
template <class Key, class T>
inline bool operator!=(const flat_unsigned_hashmap<Key, T>& lhs,
		const flat_unsigned_hashmap<Key, T>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea
