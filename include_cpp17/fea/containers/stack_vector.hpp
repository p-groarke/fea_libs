/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
#include "fea/utility/platform.hpp"
#include "fea/utility/error.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

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

	using aligned_storage_t = fea::aligned_storage_t<sizeof(T), alignof(T)>;
	using container_t = std::array<aligned_storage_t, StackSize>;

	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	// using iterator = typename std::array<T, StackSize>::iterator;
	// using const_iterator = typename std::array<T, StackSize>::const_iterator;
	// using reverse_iterator =
	//		typename std::array<T, StackSize>::reverse_iterator;
	// using const_reverse_iterator =
	//		typename std::array<T, StackSize>::const_reverse_iterator;

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
	constexpr const_reference operator[](size_type i) const noexcept;

	// Get value at position without bounds checking.
	[[nodiscard]]
	constexpr reference operator[](size_type i) noexcept;

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
			const_iterator pos, std::initializer_list<value_type>&& ilist);

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
	container_t _data;
	size_type _size = 0;
};
} // namespace fea


/**
 * Implementation
 */
namespace fea {
template <class T, size_t StackSize>
template <size_t InSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		const std::array<T, InSize>& arr)
		: _size(InSize) {
	static_assert(InSize <= StackSize,
			"stack_vector : initializing with too many values");
	std::uninitialized_copy(arr.begin(), arr.end(), begin());
}

template <class T, size_t StackSize>
template <size_t InSize>
constexpr stack_vector<T, StackSize>::stack_vector(std::array<T, InSize>&& arr)
		: _size(InSize) {
	static_assert(InSize <= StackSize,
			"stack_vector : initializing with too many values");
	std::uninitialized_move(arr.begin(), arr.end(), begin());
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		size_type count, const_reference value)
		: _size(count) {
	assert(_size <= StackSize);
	std::uninitialized_fill_n(begin(), _size, value);
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(size_type count)
		: stack_vector(count, value_type{}) {
}

template <class T, size_t StackSize>
constexpr stack_vector<T, StackSize>::stack_vector(
		std::initializer_list<value_type>&& init)
		: _size(init.size()) {
	assert(init.size() <= StackSize);
	fea::uninitialized_move_if_moveable(init.begin(), init.end(), begin());
}

template <class T, size_t StackSize>
template <class InputIt, class>
constexpr stack_vector<T, StackSize>::stack_vector(InputIt first, InputIt last)
		: _size(std::distance(first, last)) {
	assert(size_t(std::distance(first, last)) <= StackSize);
	// TODO : Check if move iterators.
	std::uninitialized_copy(first, last, begin());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::at(size_type i) const
		-> const_reference {
	if (i >= _size) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "accessing out-of-range element");
	}
	return *reinterpret_cast<const_pointer>(&_data.at(i));
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::at(size_type i) -> reference {
	return const_cast<reference>(std::as_const(*this).at(i));
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::operator[](
		size_type i) const noexcept -> const_reference {
	assert(i < _size);
	return *reinterpret_cast<const_pointer>(&_data[i]);
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::operator[](size_type i) noexcept
		-> reference {
	return const_cast<reference>(std::as_const(*this).at(i));
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::front() const noexcept
		-> const_reference {
	assert(_size > 0);
	return *reinterpret_cast<const_pointer>(&_data.front());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::front() noexcept -> reference {
	return const_cast<reference>(std::as_const(*this).front());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::back() const noexcept
		-> const_reference {
	assert(_size > 0);
	return *reinterpret_cast<const_pointer>(&_data[_size - 1]);
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::back() noexcept -> reference {
	return const_cast<reference>(std::as_const(*this).back());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::data() const noexcept
		-> const_pointer {
	return reinterpret_cast<const_pointer>(_data.data());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::data() noexcept -> pointer {
	return const_cast<pointer>(std::as_const(*this).data());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::begin() const noexcept
		-> const_iterator {
	return reinterpret_cast<const_pointer>(_data.data());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::begin() noexcept -> iterator {
	return const_cast<iterator>(std::as_const(*this).begin());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::end() const noexcept
		-> const_iterator {
	return reinterpret_cast<const_pointer>(_data.data() + _size);
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::end() noexcept -> iterator {
	return const_cast<iterator>(std::as_const(*this).end());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::rbegin() const noexcept
		-> const_reverse_iterator {
	return std::make_reverse_iterator(end());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::rbegin() noexcept
		-> reverse_iterator {
	return std::make_reverse_iterator(end());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::rend() const noexcept
		-> const_reverse_iterator {
	return std::make_reverse_iterator(begin());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::rend() noexcept -> reverse_iterator {
	return std::make_reverse_iterator(begin());
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::cbegin() const noexcept
		-> const_iterator {
	return begin();
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::cend() const noexcept
		-> const_iterator {
	return end();
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::crbegin() const noexcept
		-> const_reverse_iterator {
	return rbegin();
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::crend() const noexcept
		-> const_reverse_iterator {
	return rend();
}

template <class T, size_t StackSize>
constexpr bool stack_vector<T, StackSize>::empty() const noexcept {
	return _size == 0;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::size() const noexcept -> size_type {
	return _size;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::max_size() const noexcept
		-> size_type {
	return StackSize;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::reserve(size_type) noexcept {
	// Provided to match std::vector api.
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::capacity() const noexcept
		-> size_type {
	return StackSize;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::shrink_to_fit() noexcept {
	// Intentionally blank, provided to match std::vector api.
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::clear() {
	fea::destroy(begin(), end());
	_size = 0;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::erase(const_iterator pos)
		-> iterator {
	assert(pos <= cend());
	if (pos == cend()) {
		return end();
	}

	if (pos == cend() - 1) {
		pop_back();
		return end();
	}

	// Convert to non-const iter.
	iterator it = begin() + std::distance(cbegin(), pos);

	// Move surviving range -1 position.
	fea::move_if_moveable(it + 1, end(), it);

	assert(_size > 0);
	--_size;

	// Destroy previously last item.
	fea::destroy_at(end());
	return it;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::erase(
		const_iterator first, const_iterator last) -> iterator {
	assert(first <= last);
	assert(last <= cend());

	if (first == last) {
		return begin() + std::distance(cbegin(), last);
	}

	size_type begin_idx = size_type(std::distance(cbegin(), first));
	size_type end_idx = size_type(std::distance(cbegin(), last));
	auto beg_it = begin() + begin_idx;
	auto end_it = begin() + end_idx;

	fea::move_if_moveable(end_it, end(), beg_it);
	size_type range_size = size_type(std::distance(first, last));
	_size -= range_size;

	fea::destroy(end(), end() + range_size);
	return beg_it;
}

#if FEA_GCC
// As usual, gcc is stupid.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::insert(
		const_iterator pos, const_reference value) -> iterator {
	assert(pos <= end());
	assert(_size < _data.size());

	if (pos == cend()) {
		push_back(value);
		assert(!empty());
		return end() - 1;
	}

	// Convert to non-const iter.
	iterator it = begin() + std::distance(cbegin(), pos);

	// Uninit move the last + 1, then move all others + 1.
	fea::uninitialized_move_if_moveable(end() - 1, end(), end());
	fea::move_backward_if_moveable(it, end() - 1, end());

	// Check if input value was inside moved range.
	const_pointer val_ptr = &value;
	if (pos <= val_ptr && val_ptr < cend()) {
		++val_ptr;
	}

	*it = *val_ptr;
	++_size;
	return it;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::insert(
		const_iterator pos, value_type&& value) -> iterator {
	assert(pos <= end());
	assert(_size < _data.size());

	if (pos == cend()) {
		push_back(std::move(value));
		assert(!empty());
		return end() - 1;
	}

	iterator it = begin() + std::distance(cbegin(), pos);

	// Uninit move the last + 1, then move all others + 1.
	fea::uninitialized_move_if_moveable(end() - 1, end(), end());
	fea::move_backward_if_moveable(it, end() - 1, end());

	// Check if input value was inside moved range.
	const_pointer val_ptr = &value;
	if (pos <= val_ptr && val_ptr < cend()) {
		++val_ptr;
	}

	*it = std::move(*val_ptr);
	++_size;
	return it;
}

#if FEA_GCC
#pragma GCC diagnostic pop
#endif

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::insert(const_iterator pos,
		size_type count, const_reference value) -> iterator {
	assert(pos <= end());
	assert(_size <= _data.size() - count);

	ptrdiff_t dist = std::distance(cbegin(), pos);
	if (count == 0) {
		return begin() + dist;
	}

	iterator it = begin() + dist;
	std::uninitialized_default_construct_n(end(), count);
	fea::move_backward_if_moveable(it, end(), end() + count);
	std::fill_n(it, count, value);
	_size += count;
	return it;
}

template <class T, size_t StackSize>
template <class InputIt, class>
constexpr auto stack_vector<T, StackSize>::insert(
		const_iterator pos, InputIt first, InputIt last) -> iterator {
	// TODO : move iterators
	assert(pos <= end());

	ptrdiff_t count = std::distance(first, last);
	assert(_size <= _data.size() - count);

	ptrdiff_t dist = std::distance(cbegin(), pos);
	if (count == 0) {
		return begin() + dist;
	}

	iterator it = begin() + dist;
	std::uninitialized_default_construct_n(end(), count);
	fea::move_backward_if_moveable(it, end(), end() + count);
	fea::move_if_moveable(first, last, it);
	_size += count;
	return it;
}

template <class T, size_t StackSize>
constexpr auto stack_vector<T, StackSize>::insert(const_iterator pos,
		std::initializer_list<value_type>&& ilist) -> iterator {
	return insert(pos, fea::make_move_iterator_if_moveable(ilist.begin()),
			fea::make_move_iterator_if_moveable(ilist.end()));
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::push_back(const T& value) {
	assert(_size < StackSize);
	std::uninitialized_copy_n(&value, 1u, begin() + _size++);
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::push_back(T&& value) {
	assert(_size < StackSize);
	std::uninitialized_move_n(&value, 1u, begin() + _size++);
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::pop_back() {
	assert(_size > 0);
	--_size;
	fea::destroy_at(end());
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::resize(size_type new_size) {
	resize(new_size, T{});
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::resize(
		size_type new_size, const_reference value) {
	assert(new_size <= StackSize);
	if (new_size == _size) {
		return;
	}

	if (new_size > _size) {
		std::uninitialized_fill(end(), begin() + new_size, value);
	} else {
		fea::destroy(begin() + new_size, end());
	}
	_size = new_size;
}

template <class T, size_t StackSize>
constexpr void stack_vector<T, StackSize>::swap(stack_vector& other) {
	using std::swap;
	swap(_data, other._data);
	swap(_size, other._size);
}


template <class K, size_t S>
constexpr bool operator==(
		const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class K, size_t S>
constexpr bool operator!=(
		const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs) {
	return !(rhs == lhs);
}
} // namespace fea
