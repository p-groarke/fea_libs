/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "fea/memory/back_ptr.hpp"
#include "fea/meta/traits.hpp"

#include <array>
#include <cassert>
#include <memory>
#include <utility>

/*
fea::deque_list is a deque with linked list iterators.
The buckets are connected together as a linked list.
Forward iteration is linear on the std::deque memory buckets.

This container is usefull in a multi-threaded contexts.
It allows iterating on deque items while another thread inserts,
as long as the iteration is limited to the items contained in the forward_deque
BEFORE the insertion. Aka, cache your size and limit your iteration to that
size.

Iterators and pointers / references aren't invalidated on growth.
*/

namespace fea {
namespace detail {
template <class, size_t>
struct dl_bucket;
template <class>
struct dl_const_iter;
template <class>
struct dl_iter;
} // namespace detail

template <class T, size_t BucketSize = 32>
struct deque_list {
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	// using allocator_type = Alloc;
	using const_reference = const value_type&;
	using reference = value_type&;
	using const_iterator = detail::dl_const_iter<deque_list>;
	using iterator = detail::dl_iter<deque_list>;

	static constexpr size_type bucket_size = BucketSize;
	using bucket = detail::dl_bucket<value_type, bucket_size>;

	// Ctors.
	deque_list() noexcept;
	~deque_list() = default;
	deque_list(const deque_list&);
	deque_list(deque_list&&);
	deque_list& operator=(const deque_list&);
	deque_list& operator=(deque_list&&);

	// Element access

	// Access the first element. Error if empty.
	[[nodiscard]]
	const_reference front() const;

	// Access the first element. Error if empty.
	[[nodiscard]]
	reference front();

	// Access the last element. Error if empty.
	[[nodiscard]]
	const_reference back() const;

	// Access the last element. Error if empty.
	[[nodiscard]]
	reference back();

	// Iterators

	// Const begin iterator.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Const begin iterator.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// Begin iterator.
	[[nodiscard]]
	iterator begin() noexcept;

	// Const end iterator O(1).
	[[nodiscard]]
	const_iterator end() const noexcept;

	// Const end iterator O(1).
	[[nodiscard]]
	const_iterator cend() const noexcept;

	// End iterator O(1).
	[[nodiscard]]
	iterator end() noexcept;

	// Capacity

	// Is container empty? O(1)
	[[nodiscard]]
	bool empty() const noexcept;

	// Size of container.
	[[nodiscard]]
	size_type size() const noexcept;

	//// Max storage capacity.
	//[[nodiscard]]
	// size_type max_size() const noexcept;

	// Shrinks the underlying storage (frees buckets).
	void shrink_to_fit();

	// Modifiers

	// Clear all items and reset container.
	void clear();

	// Append an element to the end of the deque.
	void push_back(const value_type& value);

	// Append an element to the end of the deque.
	void push_back(value_type&& value);

	//// Append an element to the end of the deque.
	// template <class... Args>
	// reference emplace_back(Args&&... args);

	// Remove the last element in the deque.
	void pop_back();

private:
	void maybe_grow();

	// Our first bucket, allocated in place.
	bucket _first_bucket{};

	// Number of elements.
	size_type _size = size_type(0);

	// Pointer to the last bucket. May be == first bucket.
	fea::back_ptr<bucket> _last_bucket = nullptr;
};

} // namespace fea

#include "imp/deque_list.imp.hpp"
