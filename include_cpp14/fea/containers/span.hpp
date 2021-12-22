/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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

#if FEA_CPP20
#include <span>
namespace fea {
template <class T, size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;
}
#else
#include "fea/utils/platform.hpp"

#include <cassert>
#include <iterator>

/*
A very basic span type to get around until c++20.
*/

namespace fea {
namespace imp {

template <class T>
struct span {
	// constants and types
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	using pointer = element_type*;
	using const_pointer = const element_type*;
	using reference = element_type&;
	using const_reference = const element_type&;

	using iterator = pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;

	// span(T* data, size_type size)
	//		: _data(data)
	//		, _size(size) {
	//}

	constexpr span() = default;

	template <class It>
	constexpr span(It first, size_t count)
			: _data(&(*first))
			, _size(count) {
		using cat_t = typename std::iterator_traits<It>::iterator_category;
		static_assert(
				std::is_same<cat_t, std::random_access_iterator_tag>::value,
				"fea::span : iterators must be random access");
	}

	template <class It>
	constexpr span(It first, It last)
			: _data(&(*first))
			, _size(size_t(last - first)) {
		using cat_t = typename std::iterator_traits<It>::iterator_category;
		static_assert(
				std::is_same<cat_t, std::random_access_iterator_tag>::value,
				"fea::span : iterators must be random access");
	}

	template <template <class T, class...> class Container, class... Args>
	constexpr span(Container<T, Args...>&& container)
			: _data(container.data())
			, _size(container.size()) {
	}

	/**
	 * Iterators
	 */
	iterator begin() const noexcept {
		return _data;
	}

	iterator end() const noexcept {
		return _data + _size;
	}

	reverse_iterator rbegin() const noexcept {
		return reverse_iterator(end());
	}

	reverse_iterator rend() const noexcept {
		return reverse_iterator(begin());
	}

	/**
	 * Element access
	 */
	const_reference front() const {
		assert(!empty());
		return _data[0];
	}
	reference front() {
		assert(!empty());
		return _data[0];
	}

	const_reference back() const {
		assert(!empty());
		return _data[_size - 1];
	}
	reference back() {
		assert(!empty());
		return _data[_size - 1];
	}

	const_reference operator[](size_type i) const {
		assert(i < _size);
		return _data[i];
	}
	reference operator[](size_type i) {
		assert(i < _size);
		return _data[i];
	}

	const_pointer data() const noexcept {
		return _data;
	}
	pointer data() noexcept {
		return _data;
	}

	/**
	 * Observers
	 */
	size_type size() const noexcept {
		return _size;
	}

	size_type size_bytes() const noexcept {
		return _size * sizeof(element_type);
	}

	bool empty() const noexcept {
		return _size == 0;
	}

private:
	pointer _data = nullptr;
	size_type _size = 0;
};

} // namespace imp

template <class T>
using span = imp::span<T>;
} // namespace fea
#endif
