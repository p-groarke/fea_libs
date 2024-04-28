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
	constexpr stack_vector(const std::array<T, InSize>& arr);

	template <size_t InSize>
	constexpr stack_vector(std::array<T, InSize>&& arr);

	constexpr stack_vector(size_type count, const_reference value);

	constexpr stack_vector(size_type count);

	constexpr stack_vector(std::initializer_list<value_type>&& init);

	template <class InputIt,
			class = std::enable_if_t<fea::is_iterator<InputIt>::value>>
	constexpr stack_vector(InputIt start, InputIt stop);

	/**
	 * Element access
	 */
	FEA_NODISCARD constexpr const_reference at(size_type i) const;
	FEA_NODISCARD constexpr reference at(size_type i);

	FEA_NODISCARD constexpr const_reference operator[](
			size_type i) const noexcept;
	FEA_NODISCARD constexpr reference operator[](size_type i) noexcept;

	FEA_NODISCARD constexpr const_reference front() const noexcept;
	FEA_NODISCARD constexpr reference front() noexcept;

	FEA_NODISCARD constexpr const_reference back() const noexcept;
	FEA_NODISCARD constexpr reference back() noexcept;

	FEA_NODISCARD constexpr const_pointer data() const noexcept;
	FEA_NODISCARD constexpr pointer data() noexcept;

	/**
	 * Iterators
	 */
	FEA_NODISCARD constexpr const_iterator begin() const noexcept;
	FEA_NODISCARD constexpr iterator begin() noexcept;

	FEA_NODISCARD constexpr const_iterator end() const noexcept;
	FEA_NODISCARD constexpr iterator end() noexcept;

	FEA_NODISCARD constexpr const_reverse_iterator rbegin() const noexcept;
	FEA_NODISCARD constexpr reverse_iterator rbegin() noexcept;

	FEA_NODISCARD constexpr const_reverse_iterator rend() const noexcept;
	FEA_NODISCARD constexpr reverse_iterator rend() noexcept;

	FEA_NODISCARD constexpr const_iterator cbegin() const noexcept;
	FEA_NODISCARD constexpr const_iterator cend() const noexcept;

	FEA_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept;
	FEA_NODISCARD constexpr const_reverse_iterator crend() const noexcept;

	/**
	 * Capacity
	 */
	FEA_NODISCARD constexpr bool empty() const noexcept;

	FEA_NODISCARD constexpr size_type size() const noexcept;

	FEA_NODISCARD constexpr size_type max_size() const noexcept;

	constexpr void reserve(size_type) noexcept;

	FEA_NODISCARD constexpr size_type capacity() const noexcept;

	constexpr void shrink_to_fit() noexcept;

	/**
	 * Modifiers
	 */
	constexpr void clear();

	constexpr iterator erase(const_iterator pos);

	constexpr iterator erase(const_iterator first, const_iterator last);

	constexpr iterator insert(const_iterator pos, const_reference value);

	constexpr iterator insert(const_iterator pos, value_type&& value);

	constexpr iterator insert(
			const_iterator pos, size_type count, const_reference value);

	template <class InputIt,
			class = std::enable_if_t<std::is_base_of<std::input_iterator_tag,
					typename std::iterator_traits<InputIt>::iterator_category>::
							value>>
	constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);

	constexpr iterator insert(
			const_iterator pos, std::initializer_list<value_type> ilist);

	constexpr void push_back(const T& value);

	constexpr void push_back(T&& value);

	constexpr void pop_back();

	constexpr void resize(size_type new_size);

	constexpr void resize(size_type new_size, const_reference value);

	constexpr void swap(stack_vector& other);

	//	compares the values in the unordered_map
	template <class K, size_t S>
	friend constexpr bool operator==(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

	template <class K, size_t S>
	friend constexpr bool operator!=(
			const stack_vector<K, S>& lhs, const stack_vector<K, S>& rhs);

private:
	std::array<value_type, StackSize> _data; // Q: uninitialized?
	size_type _size = 0;
};
} // namespace fea

#include "stack_vector.imp.hpp"
