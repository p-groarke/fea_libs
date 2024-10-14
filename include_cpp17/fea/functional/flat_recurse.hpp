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
std::pair<InputIt, InputIt> children_range(InputIt parent, StatePtr*);


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
void for_each_depthfirst(InputIt root, Func&& func, CullPredicate cull_pred,
		StatePtr* state_ptr = nullptr);

// Traditional depth-first recursion.
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func, class StatePtr = const void>
void for_each_depthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr = nullptr);


template <class BidirIt, class Func, class CullPredicate,
		class StatePtr = const void>
void for_each_depthfirst_flat(BidirIt root, Func&& func,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr);

// Flat depth-first iteration.
// Starts at the provided node.
// Executes func on each node.
template <class BidirIt, class Func, class StatePtr = const void>
void for_each_depthfirst_flat(
		BidirIt root, Func&& func, StatePtr* state_ptr = nullptr);

// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class InputIt, class Func, class CullPredicate,
		class StatePtr = const void>
void for_each_breadthfirst(InputIt root, Func&& func, CullPredicate cull_pred,
		StatePtr* state_ptr = nullptr);

// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func, class StatePtr = const void>
void for_each_breadthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr = nullptr);


/*
 Gather Functions
*/

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
void gather_depthfirst(InputIt root, std::vector<InputIt>* out,
		CullPredicate cull_pred, StatePtr* state_ptr = nullptr);

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
template <class InputIt, class StatePtr = const void>
void gather_depthfirst(
		InputIt root, std::vector<InputIt>* out, StatePtr* state_ptr = nullptr);


// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class BidirIt, class CullPredicate, class StatePtr = const void>
void gather_depthfirst_flat(BidirIt root, CullPredicate cull_pred,
		std::vector<BidirIt>* out, StatePtr* state_ptr = nullptr);

// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
template <class BidirIt, class StatePtr = const void>
void gather_depthfirst_flat(
		BidirIt root, std::vector<BidirIt>* out, StatePtr* state_ptr = nullptr);


// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
void gather_breadthfirst(InputIt root, CullPredicate cull_pred,
		std::vector<InputIt>* out, StatePtr* state_ptr = nullptr);

// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
template <class InputIt, class StatePtr = const void>
void gather_breadthfirst(
		InputIt root, std::vector<InputIt>* out, StatePtr* state_ptr = nullptr);


// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate, class StatePtr = const void>
void gather_breadthfirst_staged(InputIt root, CullPredicate cull_pred,
		std::vector<std::vector<InputIt>>* out, StatePtr* state_ptr = nullptr);

// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
template <class InputIt, class StatePtr = const void>
void gather_breadthfirst_staged(InputIt root,
		std::vector<std::vector<InputIt>>* out, StatePtr* state_ptr = nullptr);
} // namespace fea

#include "imp/flat_recurse.imp.hpp"
