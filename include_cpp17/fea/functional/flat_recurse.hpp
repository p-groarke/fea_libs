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
#include <array>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace fea {
/*
 Read the following.
*/

// Specialize children_range, or provide begin() and end() on your node to get
// children iterators.
// You may call the recursive apis with a data pointer if you need state.
// If you don't need state, use const void* in your specialization.
template <class InputIt, class StatePtr = const void>
inline std::pair<InputIt, InputIt> children_range(InputIt parent, StatePtr*) {
	return { parent->begin(), parent->end() };
}


/*
 For Each Functions
*/

// Traditional depth-first recursion.
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class InputIt, class Func, class CullPredicate,
		class StatePtr = const void>
inline void for_each_depthfirst(InputIt root, Func&& func,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr) {
	// Traditional depth-first recursion.
	if (cull_pred(root)) {
		return;
	}

	func(root);

	using fea::children_range;
	std::pair<InputIt, InputIt> range = children_range(root, state_ptr);

	for (auto it = range.first; it != range.second; ++it) {
		for_each_depthfirst(it, func, cull_pred, state_ptr);
	}
}

// Traditional depth-first recursion.
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func, class StatePtr = const void>
inline void for_each_depthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr = nullptr) {

	return for_each_depthfirst(
			root, func, [](InputIt) { return false; }, state_ptr);
}


template <class BidirIt, class Func, class CullPredicate,
		class StatePtr = const void>
inline void for_each_depthfirst_flat(BidirIt root, Func&& func,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr) {
	static_assert(
			!std::is_same_v<std::input_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");
	static_assert(
			!std::is_same_v<std::output_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");
	static_assert(
			!std::is_same_v<std::forward_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");

	// Uses a "rolling vector" to flatten out graph and execute function on
	// those nodes.
	// For performance reasons, the children are inversed and the vector acts as
	// a stack.
	// Theoretical algo : Take stack front node, remove from stack,
	// execute func, gather its children, pushfront in stack. Rince-repeat until
	// vector empty.

	if (cull_pred(root)) {
		return;
	}

	std::vector<BidirIt> stack;
	stack.push_back(root);

	while (true) {
		if (stack.empty()) {
			// We are done.
			break;
		}

		// Get next in line.
		// Don't need to check cull_pred here, culled nodes are never
		// pushed back in waiting.
		BidirIt current_node = stack.back();
		stack.pop_back();
		func(current_node);

		using fea::children_range;
		std::pair<BidirIt, BidirIt> range
				= children_range(current_node, state_ptr);

		// Find first non-culled child.
		while (range.first != range.second && cull_pred(range.first)) {
			++range.first;
		}

		// All children culled, evaluate next waiting.
		if (range.first == range.second) {
			continue;
		}

		// Cull remaining children and enqueue in the stack back to front.
		while (--range.second != range.first) {
			if (cull_pred(range.second)) {
				continue;
			}
			stack.push_back(range.second);
		}

		if (!cull_pred(range.first)) {
			stack.push_back(range.first);
		}
	}
}

// Flat depth-first iteration.
// Starts at the provided node.
// Executes func on each node.
template <class BidirIt, class Func, class StatePtr = const void>
inline void for_each_depthfirst_flat(
		BidirIt root, Func&& func, StatePtr* state_ptr = nullptr) {

	return for_each_depthfirst_flat(
			root, func, [](BidirIt) { return false; }, state_ptr);
}

// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class InputIt, class Func, class CullPredicate,
		class StatePtr = const void>
inline void for_each_breadthfirst(InputIt root, Func&& func,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr) {
	std::vector<InputIt> graph;
	gather_breadthfirst(root, cull_pred, &graph, state_ptr);

	for (InputIt it : graph) {
		func(it);
	}
}

// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func, class StatePtr = const void>
inline void for_each_breadthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr = nullptr) {
	std::vector<InputIt> graph;
	gather_breadthfirst(root, &graph, state_ptr);

	for (InputIt it : graph) {
		func(it);
	}
}


/*
 Gather Functions
*/

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
inline void gather_depthfirst(InputIt root, std::vector<InputIt>* out,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr) {
	out->clear();

	return for_each_depthfirst(
			root, [&](InputIt node) { out->push_back(node); }, cull_pred,
			state_ptr);
}

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
template <class InputIt, class StatePtr = const void>
inline void gather_depthfirst(InputIt root, std::vector<InputIt>* out,
		StatePtr* state_ptr = nullptr) {

	return gather_depthfirst(
			root, out, [](InputIt) { return false; }, state_ptr);
}


// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class BidirIt, class CullPredicate, class StatePtr = const void>
inline void gather_depthfirst_flat(BidirIt root, CullPredicate cull_pred,
		std::vector<BidirIt>* out, StatePtr* state_ptr = nullptr) {
	out->clear();

	return for_each_depthfirst_flat(
			root, [&](BidirIt node) { out->push_back(node); }, cull_pred,
			state_ptr);
}

// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
template <class BidirIt, class StatePtr = const void>
inline void gather_depthfirst_flat(BidirIt root, std::vector<BidirIt>* out,
		StatePtr* state_ptr = nullptr) {
	return gather_depthfirst_flat(
			root, [](BidirIt) { return false; }, out, state_ptr);
}


// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
inline void gather_breadthfirst(InputIt root, CullPredicate cull_pred,
		std::vector<InputIt>* out, StatePtr* state_ptr = nullptr) {
	// Grab children, pushback range if not culled, rince-repeat.
	// Continue looping the vector until you reach end.

	out->clear();
	if (cull_pred(root)) {
		return;
	}

	out->push_back(root);

	for (size_t i = 0; i < out->size(); ++i) {
		using fea::children_range;
		std::pair<InputIt, InputIt> range
				= children_range((*out)[i], state_ptr);

		for (InputIt it = range.first; it != range.second; ++it) {
			if (cull_pred(it)) {
				continue;
			}

			out->push_back(it);
		}
	}
}

// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
template <class InputIt, class StatePtr = const void>
inline void gather_breadthfirst(InputIt root, std::vector<InputIt>* out,
		StatePtr* state_ptr = nullptr) {
	return gather_breadthfirst(
			root, [](InputIt) { return false; }, out, state_ptr);
}


// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
inline void gather_breadthfirst_staged(InputIt root, CullPredicate cull_pred,
		std::vector<std::vector<InputIt>>* out, StatePtr* state_ptr = nullptr) {
	out->clear();

	if (cull_pred(root)) {
		return;
	}

	out->push_back({ root });

	for (size_t i = 0; i < out->size(); ++i) {
		for (size_t j = 0; j < (*out)[i].size(); ++j) {
			using fea::children_range;
			std::pair<InputIt, InputIt> range
					= children_range((*out)[i][j], state_ptr);

			if (out->size() == i + 1 && range.first != range.second) {
				out->push_back({});
				// Expect at least as much as previous.
				out->back().reserve((*out)[i].size());
			}

			for (InputIt it = range.first; it != range.second; ++it) {
				if (cull_pred(it)) {
					continue;
				}
				(*out)[i + 1].push_back(it);
			}
		}
	}
}

// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
template <class InputIt, class StatePtr = const void>
inline void gather_breadthfirst_staged(InputIt root,
		std::vector<std::vector<InputIt>>* out, StatePtr* state_ptr = nullptr) {
	gather_breadthfirst_staged(
			root, [](InputIt) { return false; }, out, state_ptr);
}
} // namespace fea
