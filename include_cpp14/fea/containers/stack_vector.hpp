/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
	/**
	 * Aliases
	 */
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

	/**
	 * Ctors
	 */
	constexpr stack_vector() = default;

	template <size_t InSize>
	constexpr stack_vector(const std::array<T, InSize>& arr)
			: _size(InSize) {
		static_assert(InSize <= StackSize,
				"stack_vector : initializing with too many values");
		std::copy(arr.begin(), arr.end(), _data.begin());
	}

	template <size_t InSize>
	constexpr stack_vector(std::array<T, InSize>&& arr)
			: _size(InSize) {
		static_assert(InSize <= StackSize,
				"stack_vector : initializing with too many values");
		std::move(arr.begin(), arr.end(), _data.begin());
	}

	constexpr stack_vector(size_type count, const_reference value)
			: _size(count) {
		assert(_size <= StackSize);

		std::fill_n(_data.begin(), _size, value);
	}

	constexpr stack_vector(size_type count)
			: stack_vector(count, value_type{}) {
	}

	constexpr stack_vector(std::initializer_list<value_type> init)
			: _size(init.size()) {
		assert(init.size() <= StackSize);
		std::copy(init.begin(), init.end(), _data.begin());
	}

	template <class InputIt,
			class = std::enable_if_t<fea::is_iterator<InputIt>::value>>
	constexpr stack_vector(InputIt start, InputIt stop)
			: _size(std::distance(start, stop)) {
		assert(size_t(std::distance(start, stop)) <= StackSize);
		std::copy(start, stop, _data.begin());
	}

	/**
	 * Element access
	 */
	FEA_NODISCARD constexpr const_reference at(size_type i) const {
		if (i >= _size) {
			fea::maybe_throw(
					__FUNCTION__, __LINE__, "accessing out-of-range element");
		}
		return _data.at(i);
	}
	FEA_NODISCARD constexpr reference at(size_type i) {
		if (i >= _size) {
			fea::maybe_throw(
					__FUNCTION__, __LINE__, "accessing out-of-range element");
		}
		return _data.at(i);
	}

	FEA_NODISCARD constexpr const_reference operator[](
			size_type i) const noexcept {
		assert(i < _size);
		return _data[i];
	}
	FEA_NODISCARD constexpr reference operator[](size_type i) noexcept {
		assert(i < _size);
		return _data[i];
	}

	FEA_NODISCARD constexpr const_reference front() const noexcept {
		assert(_size > 0);
		return _data.front();
	}
	FEA_NODISCARD constexpr reference front() noexcept {
		assert(_size > 0);
		return _data.front();
	}

	FEA_NODISCARD constexpr const_reference back() const noexcept {
		assert(_size > 0);
		return _data[_size - 1];
	}
	FEA_NODISCARD constexpr reference back() noexcept {
		assert(_size > 0);
		return _data[_size - 1];
	}

	FEA_NODISCARD constexpr const_pointer data() const noexcept {
		return _data.data();
	}
	FEA_NODISCARD constexpr pointer data() noexcept {
		return _data.data();
	}

	/**
	 * Iterators
	 */
	FEA_NODISCARD constexpr const_iterator begin() const noexcept {
		return _data.begin();
	}
	FEA_NODISCARD constexpr iterator begin() noexcept {
		return _data.begin();
	}

	FEA_NODISCARD constexpr const_iterator end() const noexcept {
		return _data.begin() + _size;
	}
	FEA_NODISCARD constexpr iterator end() noexcept {
		return _data.begin() + _size;
	}

	FEA_NODISCARD constexpr const_reverse_iterator rbegin() const noexcept {
		return std::make_reverse_iterator(end());
	}
	FEA_NODISCARD constexpr reverse_iterator rbegin() noexcept {
		return std::make_reverse_iterator(end());
	}

	FEA_NODISCARD constexpr const_reverse_iterator rend() const noexcept {
		return std::make_reverse_iterator(begin());
	}
	FEA_NODISCARD constexpr reverse_iterator rend() noexcept {
		return std::make_reverse_iterator(begin());
	}

	FEA_NODISCARD constexpr const_iterator cbegin() const noexcept {
		return begin();
	}
	FEA_NODISCARD constexpr const_iterator cend() const noexcept {
		return end();
	}

	FEA_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}
	FEA_NODISCARD constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}

	/**
	 * Capacity
	 */
	FEA_NODISCARD constexpr bool empty() const noexcept {
		return _size == 0;
	}

	FEA_NODISCARD constexpr size_type size() const noexcept {
		return _size;
	}

	FEA_NODISCARD constexpr size_type max_size() const noexcept {
		return StackSize;
	}

	constexpr void reserve(size_type) noexcept {
		// Provided to match std::vector api.
	}

	FEA_NODISCARD constexpr size_type capacity() const noexcept {
		return StackSize;
	}

	constexpr void shrink_to_fit() noexcept {
		// Provided to match std::vector api.
	}

	/**
	 * Modifiers
	 * TODO : erase
	 */
	constexpr void clear() {
		destroy_range(0, _size, std::is_trivially_destructible<T>{});
		_size = 0;
	}

	constexpr iterator erase(const_iterator pos) {
		size_type dist = size_type(std::distance(cbegin(), pos));
		assert(dist < size());
		auto it = begin() + dist;
		std::copy(it + 1, end(), it);
		destroy_range(_size - 1, _size, std::is_trivially_destructible<T>{});
		--_size;
		return begin() + dist;
	}

	constexpr iterator insert(const_iterator pos, const_reference value) {
		assert(_size < _data.size());
		size_type dist = size_type(std::distance(cbegin(), pos));
		auto start_it = begin() + dist;
		std::copy_backward(start_it, end(), end() + 1);
		*start_it = value;
		++_size;
		return start_it;
	}

	constexpr iterator insert(const_iterator pos, value_type&& value) {
		assert(_size < _data.size());
		size_type dist = size_type(std::distance(cbegin(), pos));
		auto start_it = begin() + dist;
		std::move_backward(start_it, end(), end() + 1);
		*start_it = std::move(value);
		++_size;
		return start_it;
	}

	constexpr iterator insert(
			const_iterator pos, size_type count, const_reference value) {
		assert(_size <= _data.size() - count);
		size_type dist = size_type(std::distance(cbegin(), pos));
		if (count == 0) {
			return begin() + dist;
		}

		auto start_it = begin() + dist;
		std::copy_backward(start_it, end(), end() + count);
		std::fill_n(start_it, count, value);
		_size += count;
		return start_it;
	}

	template <class InputIt,
			class = std::enable_if_t<std::is_base_of<std::input_iterator_tag,
					typename std::iterator_traits<InputIt>::iterator_category>::
							value>>
	constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
		size_type count = size_type(std::distance(first, last));
		size_type dist = size_type(std::distance(cbegin(), pos));
		if (count == 0) {
			return begin() + dist;
		}

		assert(_size <= _data.size() - count);
		auto start_it = begin() + dist;
		std::copy_backward(start_it, end(), end() + count);
		std::copy(first, last, start_it);
		_size += count;
		return start_it;
	}

	constexpr iterator insert(
			const_iterator pos, std::initializer_list<value_type> ilist) {
		return insert(pos, ilist.begin(), ilist.end());
	}

	constexpr void push_back(const T& value) {
		assert(_size < StackSize);
		_data[_size++] = value;
	}
	constexpr void push_back(T&& value) {
		assert(_size < StackSize);
		_data[_size++] = std::move(value);
	}

	constexpr void pop_back() {
		assert(_size > 0);
		destroy_range(_size - 1, _size, std::is_trivially_destructible<T>{});
		--_size;
	}

	constexpr void resize(size_type new_size) {
		resize(new_size, T{});
	}

	constexpr void resize(size_type new_size, const_reference value) {
		assert(new_size < StackSize);
		assert(new_size > 0);

		if (new_size > _size) {
			for (size_t i = _size; i < new_size; ++i) {
				_data[i] = value;
			}
		} else {
			destroy_range(new_size, _size, std::is_trivially_destructible<T>{});
		}

		_size = new_size;
	}

	constexpr void swap(stack_vector& other) {
		using std::swap;
		swap(_data, other._data);
		swap(_size, other._size);
	}

	//	compares the values in the unordered_map
	template <class K, size_t S>
	friend constexpr bool operator==(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

	template <class K, size_t S>
	friend constexpr bool operator!=(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

private:
	constexpr void destroy_range(
			size_type first, size_type last, std::false_type) const {
		for (size_t i = first; i < last; ++i) {
			_data[i].~T();
		}
	}
	constexpr void destroy_range(size_type, size_type, std::true_type) const {
	}

	std::array<value_type, StackSize> _data; // uninitialized
	size_type _size = 0;
};

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
