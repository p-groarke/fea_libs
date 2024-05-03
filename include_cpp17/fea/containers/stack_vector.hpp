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
#include "fea/memory/memory.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <array>
#include <cassert>

/*
stack_vector is a pre-allocated array, on which you can "push back" elements.
*/

namespace fea {
template <class T, size_t StackSize>
struct stack_vector {
	// Typedefs
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = typename std::array<T, StackSize>::iterator;
	using const_iterator = typename std::array<T, StackSize>::const_iterator;
	using reverse_iterator =
			typename std::array<T, StackSize>::reverse_iterator;
	using const_reverse_iterator =
			typename std::array<T, StackSize>::const_reverse_iterator;

	// Ctors
	template <size_t InSize>
	constexpr stack_vector(const std::array<T, InSize>& arr);

	template <size_t InSize>
	constexpr stack_vector(std::array<T, InSize>&& arr);

	constexpr stack_vector(size_type count, const_reference value);

	constexpr stack_vector(size_type count);

	constexpr stack_vector(std::initializer_list<value_type>&& init);

	template <class InputIt,
			class = std::enable_if_t<fea::is_iterator<InputIt>::value>>
	constexpr stack_vector(InputIt start, InputIt stop);

	constexpr stack_vector() = default;
	~stack_vector() = default;
	constexpr stack_vector(const stack_vector&) = default;
	constexpr stack_vector(stack_vector&&) = default;
	constexpr stack_vector& operator=(const stack_vector&) = default;
	constexpr stack_vector& operator=(stack_vector&&) = default;

	// Element access

	// Get value at position with bounds checking.
	[[nodiscard]]
	constexpr const_reference at(size_type i) const;

	// Get value at position with bounds checking.
	[[nodiscard]]
	constexpr reference at(size_type i);

	// Get value at position without bounds checking.
	[[nodiscard]]
	constexpr const_reference
	operator[](size_type i) const noexcept;

	// Get value at position without bounds checking.
	[[nodiscard]]
	constexpr reference
	operator[](size_type i) noexcept;

	// Get first value. All hell breaks loose if container empty.
	[[nodiscard]]
	constexpr const_reference front() const noexcept;

	// Get first value. All hell breaks loose if container empty.
	[[nodiscard]]
	constexpr reference front() noexcept;

	// Get last value. All hell breaks loose if container empty.
	[[nodiscard]]
	constexpr const_reference back() const noexcept;

	// Get last value. All hell breaks loose if container empty.
	[[nodiscard]]
	constexpr reference back() noexcept;

	// Get a pointer to the internal stored data.
	[[nodiscard]]
	constexpr const_pointer data() const noexcept;

	// Get a pointer to the internal stored data.
	[[nodiscard]]
	constexpr pointer data() noexcept;

	/**
	 * Iterators
	 */
	// Iterator pointing to the first element.
	[[nodiscard]]
	constexpr const_iterator begin() const noexcept;

	// Iterator pointing to the first element.
	[[nodiscard]]
	constexpr iterator begin() noexcept;

	// Iterator pointing to the first element.
	[[nodiscard]]
	constexpr const_iterator cbegin() const noexcept;

	// Iterator pointing past the last element.
	[[nodiscard]]
	constexpr const_iterator end() const noexcept;

	// Iterator pointing past the last element.
	[[nodiscard]]
	constexpr iterator end() noexcept;

	// Iterator pointing past the last element.
	[[nodiscard]]
	constexpr const_iterator cend() const noexcept;

	// Reverse iterator pointing to the first element.
	[[nodiscard]]
	constexpr const_reverse_iterator rbegin() const noexcept;

	// Reverse iterator pointing to the first element.
	[[nodiscard]]
	constexpr reverse_iterator rbegin() noexcept;

	// Reverse iterator pointing to the first element.
	[[nodiscard]]
	constexpr const_reverse_iterator crbegin() const noexcept;

	// Reverse iterator pointing past the last element.
	[[nodiscard]]
	constexpr const_reverse_iterator rend() const noexcept;

	// Reverse iterator pointing past the last element.
	[[nodiscard]]
	constexpr reverse_iterator rend() noexcept;

	// Reverse iterator pointing past the last element.
	[[nodiscard]]
	constexpr const_reverse_iterator crend() const noexcept;

	// Capacity.

	// Returns true if container is empty.
	[[nodiscard]]
	constexpr bool empty() const noexcept;

	// Number of stored items (!= StackSize).
	[[nodiscard]]
	constexpr size_type size() const noexcept;

	// Max possible storage.
	[[nodiscard]]
	constexpr size_type max_size() const noexcept;

	// Provided to match std::vector api, does nothing.
	constexpr void reserve(size_type) noexcept;

	// Returns capacity of storage (StackSize).
	[[nodiscard]]
	constexpr size_type capacity() const noexcept;

	// Provided to match std::vector api, does nothing.
	constexpr void shrink_to_fit() noexcept;

	// Modifiers

	// Clear all items.
	constexpr void clear();

	// Erase item at position.
	constexpr iterator erase(const_iterator pos);

	// Erase item range.
	constexpr iterator erase(const_iterator first, const_iterator last);

	// Insert item at position.
	constexpr iterator insert(const_iterator pos, const_reference value);

	// Insert item at position.
	constexpr iterator insert(const_iterator pos, value_type&& value);

	// Insert count copies of item at position.
	constexpr iterator insert(
			const_iterator pos, size_type count, const_reference value);

	// Insert range at position.
	template <class InputIt,
			class = std::enable_if_t<std::is_base_of_v<std::input_iterator_tag,
					typename std::iterator_traits<InputIt>::iterator_category>>>
	constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);

	// Insert items at position.
	constexpr iterator insert(
			const_iterator pos, std::initializer_list<value_type> ilist);

	// Add single item at the end of container.
	constexpr void push_back(const T& value);

	// Add single item at the end of container.
	constexpr void push_back(T&& value);

	// Remove last item.
	constexpr void pop_back();

	// Create new_size number of items, default constructed.
	constexpr void resize(size_type new_size);

	// Create new_size number of copies of value.
	constexpr void resize(size_type new_size, const_reference value);

	// Swap with another container.
	constexpr void swap(stack_vector& other);

	// Deep comparison.
	template <class K, size_t S>
	friend constexpr bool operator==(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

	// Deep comparison.
	template <class K, size_t S>
	friend constexpr bool operator!=(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

private:
	std::array<value_type, StackSize> _data; // Q: uninitialized?
	size_type _size = 0;
};
} // namespace fea

#include "imp/stack_vector.imp.hpp"
