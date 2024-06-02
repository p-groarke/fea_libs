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
#include "fea/containers/flat_id_slotmap.hpp"
#include "fea/containers/id_slot_lookup.hpp"

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

// WIP! EVERYTHING SUBJECT TO CHANGE //

/*
flat_bf_graph is a flat breadth-first constant graph. It is slow to construct
and modify (topology), but fast to evaluate. It is ordered.

On full iteration, ids and node data are contiguous. On sub-graph iterations,
there are memory jumps every breadth.

Keys should be an unsigned number from 0 to N. Do NOT use this with key pointers
or large hashes, lookup grows as big as N.
*/

namespace fea {
namespace experimental {

template <class, class, class>
struct flat_bf_graph_builder;
template <class, class, class>
struct flat_bf_graph;

namespace detail {
template <class Key, class Value, class Alloc = std::allocator<Value>>
struct flat_bf_graph_builder_node {
	using key_type = Key;
	using value_type = Value;
	using underlying_key_type = fea::detail::id_hash_return_t<key_type>;

	using allocator_type = Alloc;
	using key_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<key_type>;

	using const_key_iterator =
			typename std::vector<key_type, key_allocator_type>::const_iterator;

	static constexpr key_type invalid_sentinel
			= key_type((std::numeric_limits<underlying_key_type>::max)());

	/**
	 * Ctors
	 */
	flat_bf_graph_builder_node() = default;
	~flat_bf_graph_builder_node() = default;
	flat_bf_graph_builder_node(const flat_bf_graph_builder_node&) = default;
	flat_bf_graph_builder_node(flat_bf_graph_builder_node&&) = default;
	flat_bf_graph_builder_node& operator=(const flat_bf_graph_builder_node&)
			= default;
	flat_bf_graph_builder_node& operator=(flat_bf_graph_builder_node&&)
			= default;

	/**
	 * Element access
	 */

	// My key.
	[[nodiscard]]
	const key_type& key() const noexcept {
		return _key;
	}

	// My parent key.
	[[nodiscard]]
	const key_type& parent_key() const noexcept {
		return _parent_key;
	}

	// My value.
	[[nodiscard]]
	const value_type& value() const noexcept {
		return _value;
	}
	// My value.
	[[nodiscard]]
	value_type& value() noexcept {
		return const_cast<value_type&>(std::as_const(*this).value());
	}

	// My children.
	[[nodiscard]]
	std::span<const key_type> children() const {
		return { _children_keys };
	}

	/**
	 * Iterators
	 */
	[[nodiscard]]
	const_key_iterator children_begin() const noexcept {
		return _children_keys.begin();
	}
	[[nodiscard]]
	const_key_iterator children_cbegin() const noexcept {
		return _children_keys.cbegin();
	}
	[[nodiscard]]
	const_key_iterator children_end() const noexcept {
		return _children_keys.end();
	}
	[[nodiscard]]
	const_key_iterator children_cend() const noexcept {
		return _children_keys.cend();
	}

	/**
	 * Capacity
	 */
	[[nodiscard]]
	size_t children_size() const noexcept {
		return _children_keys.size();
	}


private:
	friend fea::experimental::flat_bf_graph_builder<Key, Value, Alloc>;

	template <class T>
		requires(std::is_same_v<std::decay_t<T>, Value>)
	flat_bf_graph_builder_node(
			const key_type& parent_key, const key_type& key, T&& value)
			: _parent_key(parent_key)
			, _key(key)
			, _value(std::forward<T>(value))
			, _children_keys({}) {
		assert(_key != invalid_sentinel);
	}

	template <class T>
		requires(std::is_same_v<std::decay_t<T>, Value>)
	flat_bf_graph_builder_node(const key_type& parent_key, const key_type& key,
			T&& value, const std::span<const key_type>& children)
			: _parent_key(parent_key)
			, _key(key)
			, _value(std::forward<T>(value))
			, _children_keys(children.begin(), children.end()) {
		assert(_key != invalid_sentinel);
	}

	/**
	 * Modifiers
	 */

	// Add child.
	void push_back(const key_type& k) {
		_children_keys.push_back(k);
	}

	// My parent. Invalid == no parent (is a root node).
	key_type _parent_key = invalid_sentinel;

	// My key.
	key_type _key = invalid_sentinel;

	// My value.
	value_type _value;

	// Ordered in child order.
	std::vector<key_type, key_allocator_type> _children_keys{};
};
} // namespace detail


template <class Key, class Value, class Alloc = std::allocator<Value>>
struct flat_bf_graph_builder {
	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = Value;
	using value_type = mapped_type;
	using iter_value_type = mapped_type;
	using node_type
			= detail::flat_bf_graph_builder_node<key_type, value_type, Alloc>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using underlying_key_type = typename node_type::underlying_key_type;

	using allocator_type = Alloc;
	using key_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<key_type>;
	using node_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<node_type>;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	using const_key_iterator = typename node_type::const_key_iterator;
	using iterator = typename fea::flat_id_slotmap<key_type, node_type,
			node_allocator_type>::iterator;
	using const_iterator = typename fea::flat_id_slotmap<key_type, node_type,
			node_allocator_type>::const_iterator;

	// Invalid sentinel. Set as parent for roots.
	static constexpr key_type invalid_sentinel = node_type::invalid_sentinel;

	/**
	 * Ctors
	 */
	flat_bf_graph_builder() = default;
	~flat_bf_graph_builder() = default;
	flat_bf_graph_builder(const flat_bf_graph_builder&) = default;
	flat_bf_graph_builder(flat_bf_graph_builder&&) = default;
	flat_bf_graph_builder& operator=(const flat_bf_graph_builder&) = default;
	flat_bf_graph_builder& operator=(flat_bf_graph_builder&&) = default;


	/**
	 * Element access
	 */

	// Get the parent key used to identify root nodes.
	[[nodiscard]]
	static constexpr const key_type& root_key() noexcept {
		return invalid_sentinel;
	}

	// Is this node a root?
	[[nodiscard]]
	bool is_root(const key_type& k) const {
		return _nodes.at(k)._parent_key == root_key();
	}

	// Get a value at key k.
	[[nodiscard]]
	const_reference at(const key_type& k) const {
		return _nodes.at(k).value();
	}
	// Get a value at key k.
	[[nodiscard]]
	reference at(const key_type& k) {
		return const_cast<reference&>(std::as_const(*this).at(k));
	}

	// Does the graph contain this key?
	[[nodiscard]]
	bool contains(const key_type& k) const noexcept {
		return _nodes.contains(k);
	}

	// Get the internal node of key k.
	[[nodiscard]]
	const node_type& node_at(const key_type& k) const {
		return _nodes.at(k);
	}
	// Get the internal node of key k.
	[[nodiscard]]
	node_type& node_at(const key_type& k) {
		return const_cast<node_type&>(std::as_const(*this).node_at(k));
	}

	// Get our root node keys. Read-only.
	[[nodiscard]]
	std::span<const key_type> root_keys() const noexcept {
		return { _root_keys };
	}

	// Get a node's children. Read-only.
	[[nodiscard]]
	std::span<const key_type> children(const key_type& parent_key) const {
		return _nodes.at(parent_key).children();
	}

	/**
	 * Iterators
	 */
	// Full graph key iterators.
	[[nodiscard]]
	const_key_iterator key_begin() const noexcept {
		return _nodes.key_begin();
	}
	[[nodiscard]]
	const_key_iterator key_cbegin() const noexcept {
		return key_begin();
	}
	[[nodiscard]]
	const_key_iterator key_end() const noexcept {
		return _nodes.key_end();
	}
	[[nodiscard]]
	const_key_iterator key_cend() const noexcept {
		return key_end();
	}

	// Children (breadth) key iterators.
	[[nodiscard]]
	const_key_iterator key_begin(const key_type& parent_key) const {
		return _nodes.at(parent_key).children_begin();
	}
	[[nodiscard]]
	const_key_iterator key_cbegin(const key_type& parent_key) const {
		return key_begin(parent_key);
	}
	[[nodiscard]]
	const_key_iterator key_end(const key_type& parent_key) const {
		return _nodes.at(parent_key).children_end();
	}
	[[nodiscard]]
	const_key_iterator key_cend(const key_type& parent_key) const {
		return key_end(parent_key);
	}

	// Full graph node iterators.
	// Access your values with node.value().
	[[nodiscard]]
	iterator begin() noexcept {
		return _nodes.begin();
	}
	[[nodiscard]]
	const_iterator begin() const noexcept {
		return _nodes.begin();
	}
	[[nodiscard]]
	const_iterator cbegin() const noexcept {
		return cbegin();
	}
	[[nodiscard]]
	iterator end() noexcept {
		return _nodes.end();
	}
	[[nodiscard]]
	const_iterator end() const noexcept {
		return _nodes.end();
	}
	[[nodiscard]]
	const_iterator cend() const noexcept {
		return end();
	}

	/**
	 * Capacity
	 */

	// Is graph empty?
	[[nodiscard]]
	bool empty() const noexcept {
		return _nodes.empty();
	}

	// Node count.
	[[nodiscard]]
	size_type size() const noexcept {
		return _nodes.size();
	}

	// Returns the maximum possible number of elements graph can hold.
	[[nodiscard]]
	constexpr size_type max_size() const noexcept {
		// One reserved for root_sentinel.
		return _nodes.max_size() - size_type(1);
	}

	// Reserves storage.
	void reserve(size_type new_cap) {
		_nodes.reserve(new_cap);
	}
	void reserve(size_type lookup_new_cap, size_type value_new_cap) {
		_nodes.reserve(lookup_new_cap, value_new_cap);
	}

	// Reserve storage for a node's children vector.
	void reserve_children(const key_type& k, size_type children_new_cap) {
		_nodes.at(k)._children_keys.reserve(children_new_cap);
	}

	// Returns the key storge capacity.
	[[nodiscard]]
	size_type lookup_capacity() const noexcept {
		return _nodes.lookup_capacity();
	}

	// Returns the number of elements that can be held in currently allocated
	// storage.
	[[nodiscard]]
	size_type capacity() const noexcept {
		return _nodes.capacity();
	}

	// reduces memory usage by freeing unused memory
	void shrink_to_fit() {
		_nodes.shrink_to_fit();
	}

	/**
	 * Modifiers
	 */
	void clear() {
		// Q : Reset ids?
		_nodes.clear();
	}

	// Create a root node.
	template <class T>
		requires(std::is_same_v<std::decay_t<T>, Value>)
	void push_back(const key_type& key, T&& v) {
		assert(!_nodes.contains(key));
		assert(std::find(_root_keys.begin(), _root_keys.end(), key)
				== _root_keys.end());
		_root_keys.push_back(key);

		node_type n{ invalid_sentinel, key, std::forward<T>(v) };
		_nodes.insert(std::move(key), std::move(n));
		assert(!_nodes.contains(invalid_sentinel));
		assert(_nodes.contains(key));
	}

	// Create a node and add to parent.
	template <class T>
		requires(std::is_same_v<std::decay_t<T>, Value>)
	void push_back(const key_type& parent_key, const key_type& key, T&& v) {
		assert(!_nodes.contains(key));
		if (parent_key != root_key()) {
			assert(_nodes.contains(parent_key));
			node_type& parent_n = _nodes.at(parent_key);
			parent_n.push_back(key);
		} else {
			assert(!_nodes.contains(parent_key));
			assert(std::find(_root_keys.begin(), _root_keys.end(), key)
					== _root_keys.end());
			_root_keys.push_back(key);
		}

		node_type n{ parent_key, key, std::forward<T>(v) };
		_nodes.insert(std::move(key), std::move(n));
		assert(!_nodes.contains(invalid_sentinel));
		assert(_nodes.contains(key));
	}

	// Create a node and add to parent.
	// At the same time, add node children.
	//
	// WARNING : When adding nodes with predefined children, do not use
	// overloads that set children in parents. Your key already exists in your
	// parent's children.
	template <class T>
		requires(std::is_same_v<std::decay_t<T>, Value>)
	void push_back(const key_type& parent_key, const key_type& key, T&& v,
			const std::span<const key_type>& children) {
		assert(!_nodes.contains(key));
		if (parent_key != root_key()) {
			assert(_nodes.contains(parent_key));

			// We should be stored in our parent already.
			assert(std::find(_nodes.at(parent_key)._children_keys.begin(),
						   _nodes.at(parent_key)._children_keys.end(), key)
					!= _nodes.at(parent_key)._children_keys.end());
		} else {
			assert(!_nodes.contains(parent_key));
			assert(std::find(_root_keys.begin(), _root_keys.end(), key)
					== _root_keys.end());
			_root_keys.push_back(key);
		}

		node_type n{ parent_key, key, std::forward<T>(v), children };
		_nodes.insert(std::move(key), std::move(n));
		assert(!_nodes.contains(invalid_sentinel));
		assert(_nodes.contains(key));
	}

	// Used internally.
	[[nodiscard]]
	size_type lookup_size() const noexcept {
		return _nodes.lookup_size();
	}

private:
	// Top-level root keys.
	std::vector<key_type, key_allocator_type> _root_keys{};

	// All our nodes.
	fea::flat_id_slotmap<key_type, node_type, node_allocator_type> _nodes{};
};


namespace detail {
template <class Key, class Value, class VAlloc, class KeyAlloc, class SpanAlloc>
struct flat_bf_graph_data {
	// template <class UKeyT, class PairAlloc>
	flat_bf_graph_data(fea::id_slot_lookup<Key, KeyAlloc>&& mlookup,
			std::vector<Key, KeyAlloc>&& mkeys,
			std::vector<Value, VAlloc>&& mvalues,
			std::vector<Key, KeyAlloc>&& mparents,
			std::vector<std::span<const Key>, SpanAlloc>&& mchildren_keys,
			std::vector<std::span<const Key>, SpanAlloc>&& mbreadths)
			: lookup(std::move(mlookup))
			, keys(std::move(mkeys))
			, values(std::move(mvalues))
			, parents(std::move(mparents))
			, children_keys(std::move(mchildren_keys))
			, breadths(std::move(mbreadths)) {
		assert(lookup.size() >= keys.size());
		assert(keys.size() == values.size());
		assert(values.size() == parents.size());
		assert(parents.size() == children_keys.size());
		assert(breadths.size() <= children_keys.size());

		// Make sure the value vector was moved all the way.
		// We should hit our first child pointer quickly.
		assert(children_keys.front().data() - keys.data()
				< ptrdiff_t(keys.size()));

		// Breadths should start at first key, end at last.
		assert(breadths.front().data() == keys.data());
		assert(breadths.back().data() + breadths.back().size()
				== keys.data() + keys.size());
	}
	~flat_bf_graph_data() = default;
	flat_bf_graph_data(const flat_bf_graph_data&) = default;
	flat_bf_graph_data(flat_bf_graph_data&&) = default;
	flat_bf_graph_data& operator=(const flat_bf_graph_data&) = delete;
	flat_bf_graph_data& operator=(flat_bf_graph_data&&) = delete;


	// Key -> vector index.
	const fea::id_slot_lookup<Key, KeyAlloc> lookup{};

	// Our keys, ordered vertically.
	const std::vector<Key, KeyAlloc> keys{};

	// Our values, ordered vertically.
	std::vector<Value, VAlloc> values{};

	// Our parents, ordered vertically.
	const std::vector<Key, KeyAlloc> parents{};

	// Our children (without sub-children).
	const std::vector<std::span<const Key>, SpanAlloc> children_keys{};

	// The graph breadths, ordered from first to last.
	const std::vector<std::span<const Key>, SpanAlloc> breadths{};
};

template <class Key, class Value, class Alloc, class Func>
void recurse_breadths(flat_bf_graph_builder<Key, Value, Alloc>& builder,
		std::vector<Key>& nodes, std::vector<Key>& scratch, const Func& func) {
	func(std::span<const Key>{ nodes });

	scratch.clear();
	for (const Key& k : nodes) {
		std::span<const Key> children = builder.children(k);
		if (children.empty()) {
			continue;
		}
		scratch.insert(scratch.end(), children.begin(), children.end());
	}

	if (scratch.empty()) {
		return;
	}
	nodes.assign(scratch.begin(), scratch.end());
	recurse_breadths(builder, nodes, scratch, func);
}

template <class KeyAlloc, class SpanAlloc, class Key, class Value, class VAlloc>
flat_bf_graph_data<Key, Value, VAlloc, KeyAlloc, SpanAlloc> make_graph_data(
		fea::experimental::flat_bf_graph_builder<Key, Value, VAlloc>&&
				builder) {
	using builder_t
			= fea::experimental::flat_bf_graph_builder<Key, Value, VAlloc>;
	using node_t = typename builder_t::node_type;
	// using u_key_t = typename builder_t::underlying_key_type;

	fea::id_slot_lookup<Key, KeyAlloc> mlookup{};
	std::vector<Key, KeyAlloc> mkeys{};
	std::vector<Value, VAlloc> mvalues{};
	std::vector<Key, KeyAlloc> mparents{};
	std::vector<std::span<const Key>, SpanAlloc> mchildren_keys{};
	std::vector<std::span<const Key>, SpanAlloc> mbreadths{};

	mlookup.reserve(builder.lookup_size());
	mkeys.reserve(builder.size());
	mvalues.reserve(builder.size());
	mparents.reserve(builder.size());
	mchildren_keys.reserve(builder.size());
	mbreadths.reserve(10u); // Arbitrary amount to kick it off.

	size_t pos = 0;
	auto recurse = [&](std::span<const Key> nodes) {
		assert(!nodes.empty());
		assert(mkeys.size() == pos);
		assert(mvalues.size() == pos);
		assert(mparents.size() == pos);
		assert(mchildren_keys.size() == pos);

		mlookup.insert(nodes.begin(), nodes.end(), pos);
		assert(std::all_of(nodes.begin(), nodes.end(),
				[&](const Key& k) { return mlookup.contains(k); }));

		size_t children_start = pos + nodes.size();
		for (const Key& k : nodes) {
			node_t& n = builder.node_at(k);
			// TODO : move keys?
			mkeys.push_back(k);
			mvalues.push_back(std::move(n.value()));
			mparents.push_back(n.parent_key());

			size_t children_size = n.children_size();
			mchildren_keys.push_back({ mkeys.data() + children_start,
					mkeys.data() + children_start + children_size });
			children_start += children_size;
		}

		// Point the breadths to keys.
		mbreadths.push_back(std::span<const Key>{
				mkeys.begin() + pos, mkeys.begin() + pos + nodes.size() });

		pos += nodes.size();
		assert(mkeys.size() == pos);
		assert(mvalues.size() == pos);
		assert(mparents.size() == pos);
		assert(mchildren_keys.size() == pos);
		assert(mbreadths.back().size() == nodes.size());
	};

	// We need 2 vectors, one used for forwarding nodes with stable memory,
	// one to hold breadths.
	std::span<const Key> root_keys = builder.root_keys();
	std::vector<Key> nodes(root_keys.begin(), root_keys.end());
	std::vector<Key> scratch;
	recurse_breadths(builder, nodes, scratch, recurse);

	// Everything is perfectly sized.
	mbreadths.shrink_to_fit();

	return flat_bf_graph_data<Key, Value, VAlloc, KeyAlloc, SpanAlloc>{
		std::move(mlookup),
		std::move(mkeys),
		std::move(mvalues),
		std::move(mparents),
		std::move(mchildren_keys),
		std::move(mbreadths),
	};
}
} // namespace detail

// The constant graph.
template <class Key, class Value, class VAlloc = std::allocator<Value>>
struct flat_bf_graph {
	using key_type = Key;
	using const_key_type = const key_type;
	using mapped_type = Value;
	using value_type = mapped_type;
	using iter_value_type = mapped_type;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using allocator_type = VAlloc;
	using key_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<key_type>;
	using span_allocator_type = typename std::allocator_traits<
			allocator_type>::template rebind_alloc<std::span<const key_type>>;

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

	using const_key_iterator =
			typename std::vector<key_type, key_allocator_type>::const_iterator;

	using builder_t = flat_bf_graph_builder<Key, Value, VAlloc>;
	using data_t = detail::flat_bf_graph_data<key_type, value_type,
			allocator_type, key_allocator_type, span_allocator_type>;

	static constexpr key_type invalid_sentinel = builder_t::invalid_sentinel;

	/**
	 * Ctors
	 */
	flat_bf_graph() = default;
	flat_bf_graph(builder_t&& builder)
			: _data(detail::make_graph_data<key_allocator_type,
					span_allocator_type>(std::move(builder))) {
	}
	~flat_bf_graph() = default;
	flat_bf_graph(const flat_bf_graph&) = default;
	flat_bf_graph(flat_bf_graph&&) = default;
	flat_bf_graph& operator=(const flat_bf_graph&) = delete;
	flat_bf_graph& operator=(flat_bf_graph&&) = delete;

	// Create and fill a builder from this pre-existing graph.
	// Use this as a starting point if you need to retopologize the graph.
	[[nodiscard]]
	builder_t make_builder() const {
		flat_bf_graph_builder<Key, Value, VAlloc> ret{};
		ret.reserve(lookup_capacity(), capacity());

		std::span<const key_type> mkeys = keys();
		std::span<const value_type> mvalues = data();
		std::span<const key_type> mparents = parents();
		std::span<const std::span<const key_type>> mchildren = children();
		std::span<const std::span<const key_type>> mbreadths = breadths();

		size_t abs_i = 0;
		for (size_t i = 0; i < breadth_size(); ++i) {
			std::span<const key_type> b = breadth(i);
			for (size_t j = 0; j < b.size(); ++j) {
				ret.push_back(mparents[abs_i], mkeys[abs_i], mvalues[abs_i],
						mchildren[abs_i]);
				++abs_i;
			}
		}

		return ret;
	}

	/**
	 * Element access
	 */

	// Get the parent key used to identify root nodes.
	[[nodiscard]]
	static constexpr const key_type& root_key() noexcept {
		return invalid_sentinel;
	}

	// Is this node a root?
	[[nodiscard]]
	bool is_root(const key_type& k) const {
		size_type idx = _data.lookup.at(k);
		return _data.parents[idx] == root_key();
	}

	// Is this node a root? Doesn't throw on invalid key.
	[[nodiscard]]
	bool is_root_unchecked(const key_type& k) const noexcept {
		size_type idx = _data.lookup.at_unchecked(k);
		return _data.parents[idx] == root_key();
	}

	// Does the graph contain this key?
	[[nodiscard]]
	bool contains(const key_type& k) const noexcept {
		return _data.lookup.contains(k);
	}

	// Get a value at key k.
	[[nodiscard]]
	const_reference at(const key_type& k) const {
		size_type idx = _data.lookup.at(k);
		return _data.values[idx];
	}
	// Get a value at key k.
	[[nodiscard]]
	const_reference at_unchecked(const key_type& k) const noexcept {
		size_type idx = _data.lookup.at_unchecked(k);
		return _data.values[idx];
	}

	// Get a value at key k.
	[[nodiscard]]
	reference at(const key_type& k) {
		return const_cast<reference&>(std::as_const(*this).at(k));
	}
	// Get a value at key k.
	[[nodiscard]]
	reference at_unchecked(const key_type& k) {
		return const_cast<reference&>(std::as_const(*this).at_unchecked(k));
	}

	// Get the data index of a key.
	[[nodiscard]]
	size_type index(const key_type& k) const {
		return _data.lookup.at(k);
	}
	// Get the data index of a key.
	[[nodiscard]]
	size_type index_unchecked(const key_type& k) const noexcept {
		return _data.lookup.at_unchecked(k);
	}

	// Get a value at index idx.
	[[nodiscard]]
	const_reference
	operator[](size_type idx) const noexcept {
		return _data.values[idx];
	}
	// Get a value at index idx.
	[[nodiscard]]
	reference
	operator[](size_type idx) noexcept {
		return const_cast<reference>(std::as_const(*this).operator[](idx));
	}

	// Get the child's parent key.
	[[nodiscard]]
	const key_type& parent(const key_type& child_key) const {
		size_type idx = _data.lookup.at(child_key);
		return _data.parents[idx];
	}
	// Get the child's parent key. Doesn't throw on invalid child_key.
	[[nodiscard]]
	const key_type& parent_unchecked(const key_type& child_key) const noexcept {
		size_type idx = _data.lookup.at_unchecked(child_key);
		return _data.parents[idx];
	}

	// Get a node's children.
	[[nodiscard]]
	const std::span<const key_type>& children(
			const key_type& parent_key) const {
		size_type idx = _data.lookup.at(parent_key);
		return _data.children_keys[idx];
	}
	// Get a node's children. Doesn't throw on invalid parent key.
	[[nodiscard]]
	const std::span<const key_type>& children_unchecked(
			const key_type& parent_key) const noexcept {
		size_type idx = _data.lookup.at_unchecked(parent_key);
		return _data.children_keys[idx];
	}

	// Get a span of the breadth at idx.
	// Use breadth_size() to loop on breadths.
	[[nodiscard]]
	const std::span<const key_type>& breadth(size_type breadth_idx) const {
		return _data.breadths.at(breadth_idx);
	}

	// Get a span of the breadth at idx.
	// Use breadth_size() to loop on breadths.
	// Doesn't throw on out-of-range index.
	[[nodiscard]]
	const std::span<const key_type>& breadth_unchecked(
			size_type breadth_idx) const noexcept {
		return _data.breadths[breadth_idx];
	}

	// Get all the keys, ordered breadth-first.
	[[nodiscard]]
	std::span<const key_type> keys() const noexcept {
		return { _data.keys };
	}

	// Get all the values, ordered breadth-first.
	[[nodiscard]]
	std::span<const value_type> data() const noexcept {
		return { _data.values };
	}
	// Get all the values, ordered breadth-first.
	[[nodiscard]]
	std::span<const value_type> values() const noexcept {
		return data();
	}

	// Get all the values, ordered breadth-first.
	// WARNING : You can modify the values but NOT reorder them.
	[[nodiscard]]
	std::span<value_type> data() noexcept {
		return { _data.values };
	}
	// Get all the values, ordered breadth-first.
	// WARNING : You can modify the values but NOT reorder them.
	[[nodiscard]]
	std::span<value_type> values() noexcept {
		return data();
	}

	// Get all the parent keys, ordered breadth-first.
	[[nodiscard]]
	std::span<const key_type> parents() const noexcept {
		return { _data.parents };
	}

	// Get all the children spans, ordered breadth-first.
	[[nodiscard]]
	std::span<const std::span<const key_type>> children() const noexcept {
		return { _data.children_keys };
	}

	// Get all the breadth spans, ordered root to leaf.
	[[nodiscard]]
	std::span<const std::span<const key_type>> breadths() const noexcept {
		return { _data.breadths };
	}

	/**
	 * Iterators
	 */

	// Key iterators.
	[[nodiscard]]
	const_key_iterator key_begin() const noexcept {
		return _data.keys.begin();
	}
	[[nodiscard]]
	const_key_iterator key_cbegin() const noexcept {
		return key_begin();
	}
	[[nodiscard]]
	const_key_iterator key_end() const noexcept {
		return _data.keys.end();
	}
	[[nodiscard]]
	const_key_iterator key_cend() const noexcept {
		return key_end();
	}

	// Value iterators.
	// WARNING : You may modify values, but NOT reorder them.
	[[nodiscard]]
	iterator begin() noexcept {
		return _data.values.begin();
	}
	[[nodiscard]]
	const_iterator begin() const noexcept {
		return _data.values.begin();
	}
	[[nodiscard]]
	const_iterator cbegin() const noexcept {
		return cbegin();
	}
	[[nodiscard]]
	iterator end() noexcept {
		return _data.values.end();
	}
	[[nodiscard]]
	const_iterator end() const noexcept {
		return _data.values.end();
	}
	[[nodiscard]]
	const_iterator cend() const noexcept {
		return end();
	}

	/**
	 * Capacity
	 */

	// Is graph empty?
	[[nodiscard]]
	bool empty() const noexcept {
		assert(_data.lookup.size() >= _data.keys.size());
		assert(_data.keys.size() == _data.values.size());
		assert(_data.values.size() == _data.parents.size());
		assert(_data.parents.size() == _data.children_keys.size());
		return _data.keys.empty();
	}

	// Node count.
	[[nodiscard]]
	size_type size() const noexcept {
		assert(_data.lookup.size() >= _data.keys.size());
		assert(_data.keys.size() == _data.values.size());
		assert(_data.values.size() == _data.parents.size());
		assert(_data.parents.size() == _data.children_keys.size());
		return _data.keys.size();
	}

	// Breadth count.
	[[nodiscard]]
	size_type breadth_size() const noexcept {
		assert(_data.lookup.size() >= _data.keys.size());
		assert(_data.keys.size() == _data.values.size());
		assert(_data.values.size() == _data.parents.size());
		assert(_data.parents.size() == _data.children_keys.size());
		return _data.breadths.size();
	}

	// Returns the maximum possible number of elements graph can hold.
	[[nodiscard]]
	constexpr size_type max_size() const noexcept {
		// One reserved for root_sentinel.
		return _data.lookup.max_size() - size_type(1);
	}

	// Returns the key storge capacity.
	[[nodiscard]]
	size_type lookup_capacity() const noexcept {
		assert(_data.lookup.capacity() >= _data.keys.capacity());
		assert(_data.keys.capacity() == _data.values.capacity());
		assert(_data.values.capacity() == _data.parents.capacity());
		assert(_data.parents.capacity() == _data.children_keys.capacity());
		return _data.lookup.capacity();
	}

	// Returns the number of elements that can be held in currently allocated
	// storage.
	[[nodiscard]]
	size_type capacity() const noexcept {
		assert(_data.lookup.capacity() >= _data.keys.capacity());
		assert(_data.keys.capacity() == _data.values.capacity());
		assert(_data.values.capacity() == _data.parents.capacity());
		assert(_data.parents.capacity() == _data.children_keys.capacity());
		return _data.keys.capacity();
	}

private:
	// Our internal lookup and vectors.
	// All vectors are vertically aligned in soa fashion.
	// All data is const except values, which can be modified but not reordered.
	data_t _data{};
};

} // namespace experimental
} // namespace fea
