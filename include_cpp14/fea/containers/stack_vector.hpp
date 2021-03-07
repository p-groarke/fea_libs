/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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
#include <fea/utils/throw.hpp>

#include <array>
#include <cassert>

/*
stack_vector is a pre-allocated array, on which you can "push back" elements.
*/

namespace fea {
template <class T, size_t StackSize>
struct stack_vector {
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

	constexpr stack_vector(size_t count, const T& value)
			: _size(count) {
		assert(_size < StackSize);

		std::fill_n(_data.begin(), _size, value);
	}

	constexpr stack_vector(size_t count)
			: stack_vector(count, T{}) {
	}

	/**
	 * Element access
	 */
	const T& at(size_t i) const {
		if (i >= _size) {
			fea::maybe_throw(
					__FUNCTION__, __LINE__, "accessing out-of-range element");
		}
		return _data.at(i);
	}
	T& at(size_t i) {
		if (i >= _size) {
			fea::maybe_throw(
					__FUNCTION__, __LINE__, "accessing out-of-range element");
		}
		return _data.at(i);
	}

	const T& operator[](size_t i) const noexcept {
		assert(i < _size);
		return _data[i];
	}
	T& operator[](size_t i) noexcept {
		assert(i < _size);
		return _data[i];
	}

	const T& front() const noexcept {
		assert(_size > 0);
		return _data.front();
	}
	T& front() noexcept {
		assert(_size > 0);
		return _data.front();
	}

	const T& back() const noexcept {
		assert(_size > 0);
		return _data[_size - 1];
	}
	T& back() noexcept {
		assert(_size > 0);
		return _data[_size - 1];
	}

	const T* data() const noexcept {
		assert(_size > 0);
		return _data.data();
	}
	T* data() noexcept {
		assert(_size > 0);
		return _data.data();
	}

	/**
	 * Iterators
	 */
	auto begin() const noexcept {
		return _data.begin();
	}
	auto begin() noexcept {
		return _data.begin();
	}

	auto end() const noexcept {
		return _data.begin() + _size;
	}
	auto end() noexcept {
		return _data.begin() + _size;
	}

	auto rbegin() const noexcept {
		return std::make_reverse_iterator(end());
	}
	auto rbegin() noexcept {
		return std::make_reverse_iterator(end());
	}

	auto rend() const noexcept {
		return std::make_reverse_iterator(begin());
	}
	auto rend() noexcept {
		return std::make_reverse_iterator(begin());
	}

	auto cbegin() const noexcept {
		return begin();
	}
	auto cend() const noexcept {
		return end();
	}

	auto crbegin() const noexcept {
		return rbegin();
	}
	auto crend() const noexcept {
		return rend();
	}

	/**
	 * Capacity
	 */
	bool empty() const noexcept {
		return _size == 0;
	}

	size_t size() const noexcept {
		return _size;
	}

	size_t max_size() const noexcept {
		return StackSize;
	}

	void reserve(size_t) noexcept {
		// Provided to match std::vector api.
	}

	constexpr size_t capacity() const noexcept {
		return StackSize;
	}

	void shrink_to_fit() noexcept {
		// Provided to match std::vector api.
	}

	/**
	 * Modifiers
	 * TODO : insert, erase
	 */
	void clear() {
		destroy_range(0, _size, std::is_trivially_destructible<T>{});
		_size = 0;
	}

	void push_back(const T& value) {
		assert(_size < StackSize);
		_data[_size++] = value;
	}
	void push_back(T&& value) {
		assert(_size < StackSize);
		_data[_size++] = std::move(value);
	}

	void pop_back() {
		assert(_size > 0);
		destroy_range(_size - 1, _size, std::is_trivially_destructible<T>{});
		--_size;
	}

	void resize(size_t new_size) {
		resize(new_size, T{});
	}

	void resize(size_t new_size, const T& value) {
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

	void swap(stack_vector& other) {
		using std::swap;
		swap(_data, other._data);
		swap(_size, other._size);
	}

private:
	void destroy_range(size_t first, size_t last, std::false_type) const {
		for (size_t i = first; i < last; ++i) {
			_data[i].~T();
		}
	}
	void destroy_range(size_t, size_t, std::true_type) const {
	}

	std::array<T, StackSize> _data; // uninitialized
	size_t _size = 0;
};
} // namespace fea
