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
#include "fea/maps/flat_unsigned_map.hpp"
#include "fea/maps/id_getter.hpp"
#include "fea/memory/memory.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

// WIP //

/*
flat_bf_graph is a flat breadth-first graph. It is slow to construct and modify
(topology), but fast to evaluate. It is ordered.

On full iteration, ids and node data are contiguous. On sub-graph iterations,
there are memory jumps every breadth.

Keys should be an unsigned number from 0 to N. Do NOT use this with key pointers
or large hashes, lookup grows as big as N.

TODO : Make graph builder to batch topo operations?
*/

namespace fea {

template <class Key, class Value, class Alloc = std::allocator<Value>>
struct flat_bf_graph {
	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = Value;
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

private:
	// Internals.
	using ssize_type = std::make_signed_t<size_type>;
	using underlying_key_type =
			typename detail::unsigned_lookup<Key>::underlying_key_type;
	using pos_type = typename detail::unsigned_lookup<Key>::pos_type;
	using breadth_type = std::conditional_t<sizeof(uint16_t) < sizeof(pos_type),
			uint16_t, pos_type>;

public:
	// Constructors, destructors and assignement
	constexpr flat_bf_graph() = default;
	constexpr flat_bf_graph(const flat_bf_graph&) = default;
	constexpr flat_bf_graph(flat_bf_graph&&) noexcept = default;
	constexpr flat_bf_graph& operator=(const flat_bf_graph&) = default;
	constexpr flat_bf_graph& operator=(flat_bf_graph&&) noexcept = default;

	constexpr explicit flat_bf_graph(size_type new_cap) {
		reserve(new_cap);
	}
	constexpr explicit flat_bf_graph(
			size_type key_new_cap, size_type value_new_cap) {
		reserve(key_new_cap, value_new_cap);
	}


	// Iterators
	// Forward iteration evaluates the whole graph in breadth-first order.

	// returns an iterator to the beginning
	constexpr iterator begin() noexcept {
		return _values.begin();
	}
	constexpr const_iterator begin() const noexcept {
		return _values.begin();
	}
	constexpr const_iterator cbegin() const noexcept {
		return _values.cbegin();
	}

	// returns an iterator to the end (one past)
	constexpr iterator end() noexcept {
		return _values.end();
	}
	constexpr const_iterator end() const noexcept {
		return _values.end();
	}
	constexpr const_iterator cend() const noexcept {
		return _values.cend();
	}

	// returns an iterator to the beginning keys
	constexpr const_key_iterator key_begin() const noexcept {
		return _reverse_lookup.begin();
	}
	constexpr const_key_iterator key_cbegin() const noexcept {
		return _reverse_lookup.cbegin();
	}

	// returns an iterator to the end (one past last)
	constexpr const_key_iterator key_end() const noexcept {
		return _reverse_lookup.end();
	}
	constexpr const_key_iterator key_cend() const noexcept {
		return _reverse_lookup.cend();
	}

	// Capacity

	// checks whether the container is empty
	[[nodiscard]] constexpr bool empty() const noexcept {
		_assert_sizes();
		return _values.empty();
	}

	// returns the number of elements
	[[nodiscard]] constexpr size_type size() const noexcept {
		_assert_sizes();
		return _values.size();
	}

	// returns the maximum possible number of elements
	[[nodiscard]] constexpr size_type max_size() const noexcept {
		return _lookup.max_size();
	}

	// reserves storage
	constexpr void reserve(size_type new_cap) {
		_lookup.reserve(new_cap);
		_for_each_packed(
				[&]<class T>(std::vector<T>& v) { v.reserve(new_cap); });
	}
	constexpr void reserve(size_type key_new_cap, size_type value_new_cap) {
		_lookup.reserve(key_new_cap);
		_for_each_packed(
				[&]<class T>(std::vector<T>& v) { v.reserve(value_new_cap); });
	}

	// returns the number of elements that can be held in currently allocated
	// storage
	[[nodiscard]] constexpr size_type capacity() const noexcept {
		return _values.capacity();
	}

	// reduces memory usage by freeing unused memory
	constexpr void shrink_to_fit() {
		_lookup.shrink_to_fit();
		_for_each_packed([]<class T>(std::vector<T>& v) { v.shrink_to_fit(); });
	}

	// Returns number of immediate children (size of the first level
	// breadth). 0 children if invalid key.
	[[nodiscard]] constexpr size_type children_size(
			const key_type& k) const noexcept {
		underlying_key_type uk = _lookup.hash(k);
		if (uk == _lookup.sentinel()) {
			return breadth_size(0);
		}
		size_type pos = _lookup.find_prehashed(uk, size());
		if (pos == size()) {
			return 0;
		}

		assert(pos < _nodes.size());
		const node_info& n = _nodes[pos];
		return _children_size(pos, n);
	}

	// Returns number of breadths.
	[[nodiscard]] constexpr size_type breadth_size() const noexcept {
		return _breadths.size();
	}

	// Returns a breadth size.
	[[nodiscard]] constexpr size_type breadth_size(
			size_type bidx) const noexcept {
		assert(bidx < _breadths.size());
		return _breadths[bidx].size;
	}

	//// returns total number of children (includes sub children)
	//[[nodiscard]] constexpr size_type total_children_size(
	//		const key_type& k) const {
	//	return _node_infos[_lookup.at(k)].total_children_count;
	//}


	// Modifiers

	// clears the contents
	constexpr void clear() noexcept {
		_lookup.clear();
		_breadths.clear();
		_for_each_packed(
				[]<class T>(std::vector<T>& v) noexcept { v.clear(); });
	}


	// Inserts root node if it doesn't exist.
	// Perf note : Prefer batch calls when possible.
	template <class K, class V>
		requires std::is_convertible_v<K, Key>
			&& std::is_convertible_v<V, Value>
	constexpr std::pair<iterator, bool> insert(K&& k, V&& v) {
		auto it = find(k);
		if (it != end()) {
			return { it, false };
		}

		return {
			_insert(_lookup.sentinel(),
					std::ranges::single_view{ std::forward<K>(k) },
					std::ranges::single_view{ std::forward<V>(v) }),
			true,
		};
	}

	// Inserts multiple root nodes.
	// Keys should not exist (use insert_or_assign for that).
	template <class K, class V>
		requires std::ranges::forward_range<K> && std::ranges::forward_range<V>
	constexpr void insert(K&& keys, V&& values) {
		if (std::any_of(std::ranges::begin(keys), std::ranges::end(keys),
					[this](const key_type& k) {
						return _lookup.contains(k);
					})) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Batch inserted items must not exist.");
		}

		_insert(_lookup.sentinel(), std::forward<K>(keys),
				std::forward<V>(values));
	}


	// Inserts child node if it doesn't exist.
	// Node is placed at the end of parent's children.
	// Perf note : Prefer batch calls when possible.
	template <class K, class V>
		requires std::is_convertible_v<K, Key>
			&& std::is_convertible_v<V, Value>
	constexpr std::pair<iterator, bool> insert(
			const key_type& parent_k, K&& k, V&& v) {
		auto it = find(k);
		if (it != end()) {
			return { it, false };
		}

		underlying_key_type uk = _lookup.hash(parent_k);
		return {
			_insert(uk, std::ranges::single_view{ std::forward<K>(k) },
					std::ranges::single_view{ std::forward<V>(v) }),
			true,
		};
	}

	// Inserts multiple children nodes.
	// Keys should not exist (use insert_or_assign for that).
	template <class K, class V>
		requires std::ranges::forward_range<K> && std::ranges::forward_range<V>
	constexpr void insert(const key_type& parent_k, K&& keys, V&& values) {
		if (std::any_of(std::ranges::begin(keys), std::ranges::end(keys),
					[this](const key_type& k) {
						return _lookup.contains(k);
					})) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Batch inserted items must not exist.");
		}

		_insert(parent_k, std::forward<K>(keys), std::forward<V>(values));
	}


	// Inserts node if it doesn't exist.
	// Assigns value if it does.
	// Perf note : Prefer batch calls when possible.
	template <class K, class V>
		requires std::is_convertible_v<K, Key>
			&& std::is_convertible_v<V, Value>
	constexpr std::pair<iterator, bool> insert_or_assign(K&& k, V&& v) {
		auto it = find(k);
		if (it != end()) {
			*it = std::forward<V>(v);
			return { it, false };
		}

		return {
			_insert(_lookup.sentinel(),
					std::ranges::single_view{ std::forward<K>(k) },
					std::ranges::single_view{ std::forward<V>(v) }),
			true,
		};
	}


	// Erases node and all its children.
	// Q : Returns key? iterator past the erased item. key_end() if none erased.
	template <class K>
		requires std::is_convertible_v<K, Key>
	constexpr void erase(K&& k) {
		underlying_key_type uk = _lookup.hash(k);
		size_type shift_begin = _lookup.find_prehashed(uk, size());
		if (shift_begin == size()) {
			return;
		}
		assert(shift_begin
				== size_type(std::distance(key_begin(), find_key(k))));
		assert(shift_begin == size_type(std::distance(begin(), find(k))));

		const node_info& erased_n = _nodes[shift_begin];
		ssize_type erased_children_size = _children_size(shift_begin, erased_n);

		// Build a temp data structure which holds ranges to shift,
		// by how much, to where.
		constexpr pos_type shift_sentinel
				= (std::numeric_limits<pos_type>::max)();
		struct shift_info {
			pos_type shift_to = shift_sentinel;
			pos_type shift_begin = shift_sentinel;
			pos_type shift_end = shift_sentinel;
		};

		std::vector<shift_info> shift_ranges;
		// Assume we'll have children in every breadth (Q: more precise way?)
		shift_ranges.reserve(_breadths.size());

		// Preheat with top-level parent.
		shift_ranges.push_back(shift_info{
				.shift_to = pos_type(shift_begin),
				.shift_begin = pos_type(shift_begin + 1),
				.shift_end = pos_type(_children_begin_idx(uk)),
		});

		// for (size_t i = shift_ranges.back().shift_end; i < _nodes.size();
		// ++i) { 	pos_type shift_to = pos_type(i);
		// }


		// We loop (aka recurse) and shift ranges until next hit to
		// minimize moves. Any stored node and its accompanying data
		// should be moved only once.
		++shift_begin;
		size_type shift_end = _children_begin_idx(uk);
		ssize_type delta = 1;
		size_type deleted_count = delta;

		// Invalidate item.
		{
			_lookup.invalidate_prehashed(uk);
			breadth_info& bi = _breadths[erased_n.breadth];
			ssize_type new_size = ssize_type(bi.size) - delta;
			assert(new_size >= 0);
			bi.size = pos_type(new_size);
		}

		// Update expected position in lookup.
		// TODO : Perf optim, able to run this loop only once?
		for (pos_type& pos : _lookup) {
			if (pos == _lookup.sentinel() || pos < shift_begin
					|| pos >= shift_end) {
				continue;
			}
			ssize_type new_pos = ssize_type(pos) - delta;
			assert(new_pos >= 0);
			pos = pos_type(new_pos);
		}

		// Update child breadth begin.
		if (size_t(erased_n.breadth + 1) < _breadths.size()) {
			breadth_info& bi = _breadths[erased_n.breadth + 1];
			ssize_type new_off = ssize_type(bi.offset) - delta;
			assert(new_off >= 0);
			bi.offset = pos_type(new_off);
		}

		// Update children offsets.
		// For items of the same breadth, following the erased node, their
		// children offsets are shifted by amount of deleted children.
		//
		// For items of following breadth (+1), preceding shift_end, nothing to
		// do since they are placed before deleted children. They are handled by
		// parent breadth size change.
		if (size_t(erased_n.breadth + 1) < _breadths.size()
				&& erased_children_size > 0) {
			// We have children, do the update.
			for (size_type i = shift_begin; i < shift_end; ++i) {
				node_info& n = _nodes[i];
				if (n.breadth != erased_n.breadth) {
					// We have reached the children in between shift_begin and
					// shift_end. Nothing to do.
					break;
				}

				assert(n.children_offset != 0); // shouldn't be possible
				ssize_type new_off
						= ssize_type(n.children_offset) - erased_children_size;
				assert(new_off >= 0);
				n.children_offset = pos_type(new_off);
			}
		}

		// Move everything.
		_for_each_packed([&]<class T>(std::vector<T>& v) {
			auto from_it = v.begin() + shift_begin;
			auto to_it = v.begin() + shift_end;
			fea::copy_or_move(from_it, to_it, from_it - delta);
		});

		delta += erased_children_size;

		// Final resize to delete.
		_for_each_packed([&]<class T>(std::vector<T>& v) {
			assert(v.size() >= deleted_count);
			v.resize(v.size() - deleted_count);
		});
	}


	// swap
	// extract?
	// merge?


	// Lookup

	// access specified element with bounds checking
	[[nodiscard]] constexpr const mapped_type& at(const key_type& k) const {
		auto it = find(k);
		if (it == end()) {
			fea::maybe_throw<std::out_of_range>(
					__FUNCTION__, __LINE__, "invalid key");
		}
		return *it;
	}
	[[nodiscard]] constexpr mapped_type& at(const key_type& k) {
		return const_cast<mapped_type&>(std::as_const(*this).at(k));
	}

	// access specified element without bounds checking
	[[nodiscard]] constexpr const mapped_type& at_unchecked(
			const key_type& k) const noexcept {
		assert(_lookup.at_unchecked(k) < size());
		return _values[_lookup.at_unchecked(k)];
	}
	[[nodiscard]] constexpr mapped_type& at_unchecked(
			const key_type& k) noexcept {
		return const_cast<mapped_type&>(std::as_const(*this).at_unchecked(k));
	}

	// Returns this node's parent key.
	[[nodiscard]] constexpr const key_type& parent_key(
			const key_type& k) const {
		assert(_lookup.at_unchecked(k) < size());
		underlying_key_type uk = _nodes[_lookup.at(k)].parent;
		assert(_lookup.at_unchecked(uk) < size());
		return _reverse_lookup[_lookup.at(uk)];
	}

	// Returns this node's parent value.
	[[nodiscard]] constexpr const mapped_type& parent_at(
			const key_type& k) const {
		assert(_lookup.at(k) < size());
		underlying_key_type uk = _nodes[_lookup.at(k)].parent;
		assert(_lookup.at(uk) < _values.size());
		return _values[_lookup.at(uk)];
	}
	[[nodiscard]] constexpr mapped_type& parent_at(const key_type& k) {
		return const_cast<mapped_type&>(std::as_const(*this).parent_at(k));
	}

	// Returns the node's immediate (direct) children keys.
	[[nodiscard]] constexpr std::span<const key_type> children_keys(
			const key_type& k) const {
		return _children_data(k, _reverse_lookup);
	}

	// Returns the node's immediate (direct) children values.
	[[nodiscard]] constexpr std::span<const mapped_type> children_at(
			const key_type& k) const {
		return _children_data(k, _values);
	}
	[[nodiscard]] constexpr std::span<mapped_type> children_at(
			const key_type& k) {
		std::span<const mapped_type> ret = std::as_const(*this).children_at(k);
		return std::span<mapped_type>{ const_cast<mapped_type*>(ret.data()),
			ret.size() };
	}

	// Returns the breadth's keys.
	[[nodiscard]] constexpr std::span<const key_type> breadth_keys(
			size_type bidx) const {
		return _breadth_data(bidx, _reverse_lookup);
	}

	// Returns the breadth's values.
	[[nodiscard]] constexpr std::span<const mapped_type> breadth_at(
			size_type bidx) const {
		return _breadth_data(bidx, _values);
	}
	[[nodiscard]] constexpr std::span<mapped_type> breadth_at(size_type bidx) {
		std::span<const mapped_type> ret
				= std::as_const(*this).breadth_at(bidx);
		return std::span<mapped_type>{ const_cast<mapped_type*>(ret.data()),
			ret.size() };
	}


	// don't offer this? its trash anyways
	// operator[]

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	[[nodiscard]] constexpr size_type count(const key_type& k) const noexcept {
		return contains(k) ? 1 : 0;
	}

	// returns the key iterator of a given key
	[[nodiscard]] constexpr const_key_iterator find_key(
			const key_type& k) const noexcept {
		return key_begin() + _lookup.find(k, size());
	}

	// returns the value iterator of a given key
	[[nodiscard]] constexpr const_iterator find(
			const key_type& k) const noexcept {
		return begin() + _lookup.find(k, size());
	}
	[[nodiscard]] constexpr iterator find(const key_type& k) noexcept {
		return begin() + _lookup.find(k, size());
	}

	// checks if the container contains element with specific key
	[[nodiscard]] constexpr bool contains(const key_type& k) const noexcept {
		return _lookup.contains(k);
	}

	// equal_range

	// Returns pointer to the graph keys.
	// A for loop on these is equivalent to recursing the full graph
	// breadth-first.
	[[nodiscard]] constexpr std::span<const key_type>
	key_data() const noexcept {
		_assert_sizes();
		return { _reverse_lookup.data(), _reverse_lookup.size() };
	}

	// Returns pointer to the graph values.
	// A for loop on these is equivalent to recursing the full graph
	// breadth-first.
	[[nodiscard]] constexpr std::span<const mapped_type> data() const noexcept {
		_assert_sizes();
		return { _values.data(), _values.size() };
	}
	[[nodiscard]] constexpr std::span<mapped_type> data() noexcept {
		std::span<const mapped_type> ret = std::as_const(*this).data();
		return { const_cast<mapped_type*>(ret.data()), ret.size() };
	}


private:
	struct node_info;
	constexpr void _assert_sizes() const noexcept {
		assert(_values.size() == _nodes.size());
		assert(_values.size() == _reverse_lookup.size());
		assert(_values.capacity() == _nodes.capacity());
		assert(_values.capacity() == _reverse_lookup.capacity());
	}

	template <class Func>
	constexpr void _for_each_packed(Func&& func) const
			noexcept(fea::is_noexcept_v<Func, std::vector<int>&>) {
		func(_nodes);
		func(_reverse_lookup);
		func(_values);
		_assert_sizes();
	}

	template <class Func>
	constexpr void _for_each_packed(Func&& func) noexcept(
			fea::is_noexcept_v<Func, std::vector<int>&>) {
		func(_nodes);
		func(_reverse_lookup);
		func(_values);
		_assert_sizes();
	}

	//[[nodiscard]] constexpr const node_info& _node_info_at(
	//		underlying_key_type k) const {
	//	assert(_lookup.at_prehashed(k) < size());
	//	return _nodes[_lookup.at_prehashed(k)];
	//}
	//[[nodiscard]] constexpr node_info& _node_info_at(underlying_key_type k) {
	//	return const_cast<node_info&>(std::as_const(*this)._node_info_at(k));
	//}

	// Returns sanitized breadth offset (size() if invalid breadth).
	[[nodiscard]] constexpr size_type _breadth_offset(
			breadth_type bidx) const noexcept {
		if (bidx >= _breadths.size()) {
			return size();
		}
		return _breadths[bidx].offset;
	}

	[[nodiscard]] constexpr size_type _breadth_end_idx(
			breadth_type bidx) const noexcept {
		assert(bidx < _breadths.size());
		const breadth_info& bi = _breadths[bidx];
		assert(bi.offset + bi.size <= size());
		return bi.offset + bi.size;
	}

	// Given a breadth and one of the member packed vectors,
	// returns a span to its data in that vector.
	template <class T, class A>
	[[nodiscard]] constexpr std::span<const T> _breadth_data(
			size_type bidx, const std::vector<T, A>& packed_vec) const {
		size_type s = breadth_size(bidx);
		if (s == 0) {
			return {};
		}

		assert(packed_vec.begin() + _breadths[bidx].offset <= packed_vec.end());
		assert(packed_vec.begin() + _breadths[bidx].offset + s
				<= packed_vec.end());
		return std::span<const T>{ packed_vec.begin() + _breadths[bidx].offset,
			s };
	}

	// Resizes a breadth, adding or substracting the offset.
	// Adds breadth if required and updates other breadths.
	constexpr void _update_breadth_lookups(
			breadth_type bidx, ssize_type delta) {
		if (bidx >= _breadths.size()) {
			// Add new breadth[s]. Point to end.
			assert(delta >= 0);
			_breadths.resize(bidx + 1, breadth_info{ pos_type(size()) });
			_breadths.back().size = pos_type(delta);
			assert(bidx == 0
					|| _breadths[bidx].offset
							== _breadths[bidx - 1].offset
									+ _breadths[bidx - 1].size);
			return;
		}

		// Update our size.
		{
			breadth_info& bi = _breadths[bidx];
			assert(ssize_type(bi.size) + delta >= 0);
			bi.size = pos_type(bi.size + delta);
		}

		// Update following breadths.
		for (breadth_type i = bidx + 1; i < _breadths.size(); ++i) {
			breadth_info& bi = _breadths[i];
			assert(ssize_type(bi.offset) + delta >= 0);
			bi.offset = pos_type(bi.offset + delta);
			assert(i == 0
					|| bi.offset
							== _breadths[i - 1].offset + _breadths[i - 1].size);
		}
	}

	// Perf optim in case you've already dereffed node.
	[[nodiscard]] constexpr size_type _children_size(
			size_type pos, const node_info& n) const noexcept {
		if (size_t(n.breadth) + 1 >= _breadths.size()) {
			// No children breadth, no children.
			return 0;
		}

		const breadth_info& bi = _breadths[n.breadth];
		assert(bi.size > 0);

		if (pos == bi.offset + bi.size - 1) {
			// Last node in breadth, can't rely on next node.
			const breadth_info& child_bi = _breadths[n.breadth + 1];
			assert(child_bi.size - ssize_type(n.children_offset) >= 0);
			return child_bi.size - n.children_offset;
		}

		assert(_nodes[pos + 1].breadth == n.breadth);
		return _nodes[pos + 1].children_offset - n.children_offset;
	}

	//// Returns the absolute start index of children range.
	[[nodiscard]] constexpr size_type _children_begin_idx(
			underlying_key_type parent_k) const noexcept {
		if (parent_k == _lookup.sentinel()) {
			return 0;
		}

		assert(_lookup.contains_prehashed(parent_k));
		size_type pos = _lookup.find_prehashed(parent_k, size());

		assert(pos < size());
		const node_info& n = _nodes[pos];
		if (size_t(n.breadth) + 1 >= _breadths.size()) {
			// No children breadth, no children.
			return size();
		}
		return _breadths[n.breadth + 1].offset + n.children_offset;
	}

	// Returns the absolute end index of children range.
	[[nodiscard]] constexpr size_type _children_end_idx(
			underlying_key_type parent_k) const noexcept {
		if (parent_k == _lookup.sentinel()) {
			if (_breadths.empty()) {
				return 0;
			}
			return breadth_size(0);
		}

		assert(_lookup.contains_prehashed(parent_k));
		size_type pos = _lookup.find_prehashed(parent_k, size());
		assert(pos < size());

		const node_info& n = _nodes[pos];
		if (size_t(n.breadth) + 1 >= _breadths.size()) {
			// No children breadth, no children.
			return size();
		}

		size_type csize = _children_size(pos, n);
		return _breadths[n.breadth + 1].offset + n.children_offset + csize;
	}

	// Given a node and one of the member packed vectors,
	// returns a span to its data in that vector.
	template <class T, class A>
	[[nodiscard]] constexpr std::span<const T> _children_data(
			underlying_key_type k, const std::vector<T, A>& packed_vec) const {
		if (k == _lookup.sentinel()) {
			// Root nodes are placed at the beginning.
			// Their size is the size of the first breadth.
			return std::span<const T>{ packed_vec.begin(), breadth_size(0) };
		}

		size_type pos = _lookup.at(k);
		assert(pos < _nodes.size());

		const node_info& n = _nodes[pos];
		size_type csize = _children_size(pos, n);
		if (csize == 0) {
			return {};
		}

		return std::span<const T>{
			packed_vec.begin() + _breadths[n.breadth + 1].offset
					+ n.children_offset,
			csize,
		};
	}

	// Modifies and sets up this parent's lookup given an increase / decrease in
	// children size.
	// Also shifts all subsequent nodes by +- delta.
	constexpr void _update_lookups(
			underlying_key_type parent_k, ssize_type delta) {
		// TODO, perf : deletion can skip some updated items.

		// Used to "make room" later on.
		size_type insert_idx = _children_end_idx(parent_k);

		if (parent_k == _lookup.sentinel()) {
			// We don't need to update children offsets.
			// You can think of the sentinel as a "single" root parent.
			// Which means there are no parent "neighbours" to update.
			_update_breadth_lookups(0, delta);
		} else {
			size_type parent_idx = _lookup.at_prehashed(parent_k);
			node_info& parent_info = _nodes[parent_idx];

			//// Bump our children count.
			// ssize_type new_size = ssize_type(parent_info.children_size) +
			// delta; assert(new_size >= 0); parent_info.children_size =
			// pos_type(new_size);

			// Update the breadth to take into consideration the new children.
			size_type breadth_end = _breadth_end_idx(parent_info.breadth);
			_update_breadth_lookups(parent_info.breadth + 1, delta);

			// Update following parents of current breadth bumping their
			// children offset index.
			for (size_type i = parent_idx + 1; i < breadth_end; ++i) {
				assert(i < _nodes.size());
				node_info& next = _nodes[i];
				assert(i == 0 || _nodes[i - 1].breadth == next.breadth);

				ssize_type new_offset
						= ssize_type(next.children_offset) + delta;
				assert(new_offset >= 0);

				next.children_offset = pos_type(new_offset);
				assert(i == 0
						|| next.children_offset
								>= _nodes[i - 1].children_offset);
				assert(i == 0
						|| next.children_offset
								== _nodes[i - 1].children_offset
										+ _children_size(i - 1, _nodes[i - 1]));
			}
		}


		// Update all following nodes to "move" them (update to their final
		// position). insert_idx must point to the first node after this node's
		// children.
		for (pos_type& pos : _lookup) {
			if (pos == _lookup.sentinel() || pos < insert_idx) {
				continue;
			}
			ssize_type new_pos = ssize_type(pos) + delta;
			assert(new_pos >= 0);
			pos = pos_type(new_pos);
		}

		_assert_sizes();
	}

	template <class K, class V>
		requires std::ranges::forward_range<K> && std::ranges::forward_range<V>
	constexpr iterator _insert(underlying_key_type uk, K&& k, V&& v) {
		auto key_begin_it = std::ranges::begin(k);
		auto key_end_it = std::ranges::end(k);
		auto val_begin_it = std::ranges::begin(v);
		auto val_end_it = std::ranges::end(v);

		size_type count = std::distance(key_begin_it, key_end_it);
		if (size_type(std::distance(val_begin_it, val_end_it)) != count) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Mismatch number of keys and values. Both must be same "
					"count.");
		}

		breadth_type insert_breadth = uk == _lookup.sentinel()
				? breadth_type(0)
				: _nodes[_lookup.at(uk)].breadth + 1;

		// Get the parent children end idx. This is where we insert.
		// Basically a breadth push_back.
		size_type insert_idx = _children_end_idx(uk);
		assert(insert_idx >= _nodes.size() || uk == _lookup.sentinel()
				|| children_size(uk) == 0
				|| _nodes[insert_idx - 1].parent == uk);

		// Setup new node offset. It's children start after the previous node's
		// children. Note, insert_idx currently points to "next" node.
		pos_type mchildren_offset = 0;
		if (insert_idx > 0) {
			const node_info& prev = _nodes[insert_idx - 1];
			if (prev.breadth == insert_breadth) {
				// Valid candidate previous node.
				mchildren_offset = prev.children_offset
						+ pos_type(_children_size(insert_idx - 1, prev));
			}
		}

		// Update all the lookups (breadths, parents, lookup positions).
		_update_lookups(uk, ssize_type(count));

		// TODO : recycle?
		std::vector<node_info> new_infos(count,
				node_info{
						.parent = uk,
						.children_offset = mchildren_offset,
						//.children_size = 0,
						.breadth = insert_breadth,
				});

		{
			_lookup.insert(key_begin_it, key_end_it, insert_idx);
			_nodes.insert(_nodes.begin() + insert_idx, new_infos.begin(),
					new_infos.end());
			_reverse_lookup.insert(_reverse_lookup.begin() + insert_idx,
					key_begin_it, key_end_it);
			_values.insert(
					_values.begin() + insert_idx, val_begin_it, val_end_it);
		}

		_assert_sizes();
		return _values.begin() + insert_idx;
	}

	struct node_info {
		// Node's parent, sentinel if root node.
		underlying_key_type parent
				= (std::numeric_limits<underlying_key_type>::max)();

		// Start of this nodes children breadth, local to child breadth
		// start. Child breadth offset + child offset == first child idx.
		pos_type children_offset = pos_type(0);

		// This node's breadth level.
		breadth_type breadth = (std::numeric_limits<breadth_type>::max)();
	};

	struct breadth_info {
		// The start index if this breadth's nodes, absolute.
		pos_type offset = (std::numeric_limits<pos_type>::max)();

		// How many nodes in this breadth.
		pos_type size = pos_type(0);
	};

	using breadth_info_alloc_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<breadth_info>;
	using node_info_alloc_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<node_info>;
	using pos_alloc_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<pos_type>;

	// key -> position
	detail::unsigned_lookup<Key, allocator_type> _lookup;

	// Points to the start index of each breadth.
	std::vector<breadth_info, breadth_info_alloc_type> _breadths;

	// Following are packed and synchronus vectors, in the global breadth first
	// order.
	std::vector<node_info, node_info_alloc_type> _nodes;
	std::vector<key_type, key_allocator_type> _reverse_lookup;
	std::vector<value_type, allocator_type> _values;
};
} // namespace fea
