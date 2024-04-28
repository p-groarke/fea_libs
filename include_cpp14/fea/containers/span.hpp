﻿/*
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
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>

#if FEA_CPP20
#include <span>
#endif


namespace fea {
namespace imp {
template <class T, size_t = static_cast<size_t>(-1)>
struct span;
}

#if FEA_CPP20
template <class T, size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;
#else
template <class T, size_t Extent = static_cast<size_t>(-1)>
using span = imp::span<T, Extent>;
#endif

namespace imp {
// A very basic span type to get around until c++20.
template <class T, size_t Extent>
struct span {
	static_assert(Extent == static_cast<size_t>(-1),
			"fea::span : Doesn't support non dynamic extent.");

	// constants and types
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	using iterator = pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;

	constexpr span() = default;
	~span() = default;
	// constexpr span(span<T, Extent>&&) = default;
	// constexpr span& operator=(span<T, Extent>&&) = default;

	template <class It>
	constexpr span(It first, size_t count);
	template <class It>
	constexpr span(It first, It last);
	template <template <class, class...> class Container, class... Args>
	constexpr span(const Container<std::decay_t<T>, Args...>& container);

	// Enable construction of const from non-const.
	constexpr span(const span<std::remove_const_t<T>, Extent>&);
	constexpr span& operator=(const span<std::remove_const_t<T>, Extent>&);


	/**
	 * Iterators
	 */
	FEA_NODISCARD
	constexpr iterator begin() const noexcept;

	FEA_NODISCARD
	constexpr iterator end() const noexcept;

	FEA_NODISCARD
	constexpr reverse_iterator rbegin() const noexcept;

	FEA_NODISCARD
	constexpr reverse_iterator rend() const noexcept;

	/**
	 * Element access
	 */
	FEA_NODISCARD
	constexpr const_reference front() const;
	FEA_NODISCARD
	constexpr reference front();

	FEA_NODISCARD
	constexpr const_reference back() const;
	FEA_NODISCARD
	constexpr reference back();

	FEA_NODISCARD
	constexpr const_reference operator[](size_type i) const;
	FEA_NODISCARD
	constexpr reference operator[](size_type i);

	FEA_NODISCARD
	constexpr const_pointer data() const noexcept;
	FEA_NODISCARD
	constexpr pointer data() noexcept;

	/**
	 * Observers
	 */
	FEA_NODISCARD
	constexpr size_type size() const noexcept;

	FEA_NODISCARD
	constexpr size_type size_bytes() const noexcept;

	FEA_NODISCARD
	constexpr bool empty() const noexcept;

	/**
	 * Operators
	 */
	template <class U>
	friend constexpr bool operator==(span<U> lhs, span<U> rhs);
	template <class U>
	friend constexpr bool operator!=(span<U> lhs, span<U> rhs);

private:
	pointer _data = nullptr;
	size_type _size = 0;
};

} // namespace imp
} // namespace fea

#include "span.imp.hpp"
