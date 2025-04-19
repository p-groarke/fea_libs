/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
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
#include "fea/utility/platform.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>

#if FEA_CPP20
#include <span>
#endif

/*
fea::span : A very basic span type to get around until c++20.
Aliases std::span in c++ >= 20.
*/

namespace fea {
namespace imp {
template <class T, size_t = size_t(-1)>
struct span;
}

#if FEA_CPP20
template <class T, size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;
#else
template <class T, size_t Extent = size_t(-1)>
using span = imp::span<T, Extent>;
#endif

namespace imp {
template <class T, size_t Extent>
struct span {
	// Sanity checks.
	static_assert(Extent == size_t(-1),
			"fea::span : Doesn't support non dynamic extent.");

	// Typedefs
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

	// Ctors
	constexpr span() = default;
	~span() = default;

	template <class It>
	constexpr span(It first, size_t count);
	template <class It>
	constexpr span(It first, It last);
	template <template <class, class...> class Container, class... Args>
	constexpr span(const Container<std::decay_t<T>, Args...>& container);

	// Enable construction of const from non-const.
	template <class U>
	constexpr span(const span<U, Extent>&);
	template <class U>
	constexpr span& operator=(const span<U, Extent>&);

	// Iterators

	// Begin iterator, contiguous.
	[[nodiscard]]
	constexpr iterator begin() const noexcept;

	// Begin iterator, contiguous.
	[[nodiscard]]
	constexpr iterator end() const noexcept;

	// Begin iterator, contiguous.
	[[nodiscard]]
	constexpr reverse_iterator rbegin() const noexcept;

	// Begin iterator, contiguous.
	[[nodiscard]]
	constexpr reverse_iterator rend() const noexcept;

	// Element access

	// First element, undefined if invalid.
	[[nodiscard]]
	constexpr const_reference front() const noexcept;

	// First element, undefined if invalid.
	[[nodiscard]]
	constexpr reference front() noexcept;

	// Last element, undefined if invalid.
	[[nodiscard]]
	constexpr const_reference back() const noexcept;

	// Last element, undefined if invalid.
	[[nodiscard]]
	constexpr reference back() noexcept;

	// Access element, undefined if invalid.
	[[nodiscard]]
	constexpr const_reference operator[](size_type i) const noexcept;

	// Access element, undefined if invalid.
	[[nodiscard]]
	constexpr reference operator[](size_type i) noexcept;

	// Get underlying data pointer.
	[[nodiscard]]
	constexpr const_pointer data() const noexcept;

	// Get underlying data pointer.
	[[nodiscard]]
	constexpr pointer data() noexcept;

	// Observers

	// The span size.
	[[nodiscard]]
	constexpr size_type size() const noexcept;

	// The span size, in bytes.
	[[nodiscard]]
	constexpr size_type size_bytes() const noexcept;

	// Is the span empty?
	[[nodiscard]]
	constexpr bool empty() const noexcept;

	// Operators

	// Compares 2 spans ITEM WISE.
	template <class U>
	friend constexpr bool operator==(span<U> lhs, span<U> rhs);

	// Compares 2 spans ITEM WISE.
	template <class U>
	friend constexpr bool operator!=(span<U> lhs, span<U> rhs);

private:
	pointer _data = nullptr;
	size_type _size = 0;
};

} // namespace imp
} // namespace fea


// Implementation
namespace fea {
namespace imp {
template <class T, size_t Extent>
template <class It>
constexpr span<T, Extent>::span(It first, size_t count)
		: _size(count) {
	using cat_t = typename std::iterator_traits<It>::iterator_category;
	static_assert(std::is_same_v<cat_t, std::random_access_iterator_tag>,
			"fea::span : iterators must be random access");

	if (_size != 0) {
		_data = &(*first);
	}
}

template <class T, size_t Extent>
template <class It>
constexpr span<T, Extent>::span(It first, It last)
		: _size(size_t(last - first)) {
	using cat_t = typename std::iterator_traits<It>::iterator_category;
	static_assert(std::is_same_v<cat_t, std::random_access_iterator_tag>,
			"fea::span : iterators must be random access");

	if (_size != 0) {
		_data = &(*first);
	}
}

template <class T, size_t Extent>
template <template <class, class...> class Container, class... Args>
constexpr span<T, Extent>::span(
		const Container<std::decay_t<T>, Args...>& container)
		: _data(container.data())
		, _size(container.size()) {
}

template <class T, size_t Extent>
template <class U>
constexpr span<T, Extent>::span(const span<U, Extent>& other)
		: span(other.data(), other.size()) {
}

template <class T, size_t Extent>
template <class U>
constexpr auto span<T, Extent>::operator=(const span<U, Extent>& other)
		-> span<T, Extent>& {
	_data = other.data();
	_size = other.size();
	return *this;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::begin() const noexcept -> iterator {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::end() const noexcept -> iterator {
	return _data + _size;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::rbegin() const noexcept -> reverse_iterator {
	return reverse_iterator(end());
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::rend() const noexcept -> reverse_iterator {
	return reverse_iterator(begin());
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::front() const noexcept -> const_reference {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::front() noexcept -> reference {
	assert(!empty());
	return _data[0];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::back() const noexcept -> const_reference {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::back() noexcept -> reference {
	assert(!empty());
	return _data[_size - 1];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::operator[](size_type i) const noexcept
		-> const_reference {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::operator[](size_type i) noexcept -> reference {
	assert(i < _size);
	return _data[i];
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::data() const noexcept -> const_pointer {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::data() noexcept -> pointer {
	return _data;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::size() const noexcept -> size_type {
	return _size;
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::size_bytes() const noexcept -> size_type {
	return _size * sizeof(element_type);
}

template <class T, size_t Extent>
constexpr auto span<T, Extent>::empty() const noexcept -> bool {
	return _size == 0;
}

} // namespace imp
} // namespace fea


// Defining the operator== could cause issues.
// You may disable these by defining FEA_DISABLE_SPAN_EQ.
#if !defined(FEA_DISABLE_SPAN_EQ_DEF)
#if FEA_CPP20
namespace std {
template <class U>
[[nodiscard]]
constexpr auto operator==(span<U> lhs, span<U> rhs) -> bool {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
[[nodiscard]]
constexpr auto operator!=(span<U> lhs, span<U> rhs) -> bool {
	return !(lhs == rhs);
}
} // namespace std
#endif

namespace fea {
namespace imp {
template <class U>
constexpr auto operator==(span<U> lhs, span<U> rhs) -> bool {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	if (lhs.data() == rhs.data()) {
		return true;
	}

	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class U>
constexpr auto operator!=(span<U> lhs, span<U> rhs) -> bool {
	return !(lhs == rhs);
}
} // namespace imp
} // namespace fea
#endif
