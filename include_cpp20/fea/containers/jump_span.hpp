/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include <array>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <span>
#include <type_traits>
#include <vector>

/**
 * jump_span : An *allocating* span like structure which refers to multiple
 * contiguous spans. Iteratable as you would a contiguous container.
 * Basically, an iterator api around std::vector<std::span<T>>.
 *
 * For all intents and purposes, acts as much as a span as possible.
 */

namespace fea {
template <class, size_t, class>
struct jump_span;

template <class T, size_t Extent, class Alloc>
struct jump_span_iterator {
	using value_type = std::remove_cv_t<T>;
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;
	using pointer = T*;
	using reference = T&;
	using iterator_category = std::random_access_iterator_tag; // not contiguous
	using iterator_concept = std::random_access_iterator_tag; // not contiguous
	//  using iterator_concept = std::contiguous_iterator_tag; // not
	//  contiguous

	constexpr jump_span_iterator() noexcept = default;
	constexpr jump_span_iterator(const jump_span<T, Extent, Alloc>& back_ref,
			size_type span_idx, size_type lcl_idx) noexcept
			: _back_ptr(&back_ref)
			, _span_idx(span_idx)
			, _lcl_idx(lcl_idx) {
	}

	[[nodiscard]] constexpr reference operator*() const noexcept {
		return _back_ptr->_spans[_span_idx][_lcl_idx];
	}
	[[nodiscard]] constexpr pointer operator->() const noexcept {
		return &_back_ptr->_spans[_span_idx][_lcl_idx];
	}

	constexpr jump_span_iterator& operator++() noexcept {
		++_lcl_idx;
		if (_lcl_idx == _back_ptr->_spans[_span_idx].size()) {
			++_span_idx;
			_lcl_idx = 0;
		}
		return *this;
	}
	constexpr jump_span_iterator operator++(int) noexcept {
		jump_span_iterator ret{ *this };
		++*this;
		return ret;
	}

	constexpr jump_span_iterator& operator--() noexcept {
		if (_lcl_idx == 0) {
			--_span_idx;
			_lcl_idx = _back_ptr->_spans[_span_idx].size() - 1;
		} else {
			--_lcl_idx;
		}
		return *this;
	}
	constexpr jump_span_iterator operator--(int) noexcept {
		jump_span_iterator ret{ *this };
		--*this;
		return ret;
	}

	constexpr jump_span_iterator& operator+=(difference_type off) noexcept {
		for (size_type i = _span_idx; i < _back_ptr->_spans.size(); ++i) {
			const std::span<T, Extent>& s = _back_ptr->_spans[i];
			if (_lcl_idx + off >= s.size()) {
				off -= s.size() - _lcl_idx;
				++_span_idx;
				_lcl_idx = 0;
			} else {
				_lcl_idx += off;
				break;
			}
		}
		return *this;
	}
	[[nodiscard]] constexpr jump_span_iterator operator+(
			const difference_type off) const noexcept {
		jump_span_iterator ret{ *this };
		ret += off;
		return ret;
	}
	[[nodiscard]] friend constexpr jump_span_iterator operator+(
			const difference_type offset, jump_span_iterator next) noexcept {
		next += offset;
		return next;
	}

	constexpr jump_span_iterator& operator-=(difference_type off) noexcept {
		for (difference_type i = _span_idx; i >= 0; --i) {
			if (off > difference_type(_lcl_idx)) {
				off -= _lcl_idx + 1;
				--_span_idx;
				_lcl_idx = _back_ptr->_spans[_span_idx].size() - 1;
			} else {
				_lcl_idx -= off;
				break;
			}
		}
		return *this;
	}
	[[nodiscard]] constexpr jump_span_iterator operator-(
			const difference_type off) const noexcept {
		jump_span_iterator ret{ *this };
		ret -= off;
		return ret;
	}
	// TODO
	[[nodiscard]] constexpr difference_type operator-(
			const jump_span_iterator& right) const noexcept {
		if (_span_idx == right._span_idx) {
			return difference_type(_lcl_idx) - difference_type(right._lcl_idx);
		} else if (_span_idx < right._span_idx) {
			return -right.operator-(*this);
		} else {
			difference_type ret = 0;
			size_type r_lcl_idx = right._lcl_idx;
			for (size_type i = right._span_idx; i < _back_ptr->_spans.size();
					++i) {
				const std::span<T, Extent>& s = _back_ptr->_spans[i];

				if (i != size_type(_span_idx)) {
					ret += s.size() - r_lcl_idx;
					r_lcl_idx = 0;
				} else {
					ret += _lcl_idx;
					break;
				}
			}
			return ret;
		}
	}

	[[nodiscard]] constexpr reference operator[](
			const difference_type offset) const noexcept {
		return *(*this + offset);
	}

	[[nodiscard]] constexpr bool operator==(
			const jump_span_iterator& right) const noexcept {
		return _span_idx == right._span_idx && _lcl_idx == right._lcl_idx;
	}

	[[nodiscard]] constexpr std::strong_ordering operator<=>(
			const jump_span_iterator& right) const noexcept {
		if (_span_idx == right._span_idx) {
			return _lcl_idx <=> right._lcl_idx;
		}
		return _span_idx <=> right._span_idx;
	}

	template <class U, size_t V, class W>
	[[nodiscard]] friend constexpr bool are_contiguous(
			const jump_span_iterator<U, V, W>& lhs,
			const jump_span_iterator<U, V, W>& rhs);


	template <class U, size_t V, class W>
	[[nodiscard]] friend constexpr typename std::span<U, V>::iterator
	make_contiguous(const jump_span_iterator<U, V, W>& it);

private:
	const jump_span<T, Extent, Alloc>* _back_ptr = nullptr;
	difference_type _span_idx = 0;
	size_type _lcl_idx = 0;
};

template <class T, size_t Extent, class Alloc>
constexpr bool are_contiguous(const jump_span_iterator<T, Extent, Alloc>& lhs,
		const jump_span_iterator<T, Extent, Alloc>& rhs) {
	return lhs._span_idx == rhs._span_idx;
}

template <class It>
constexpr bool are_contiguous(It, It) {
	// wtf msvc
	// return std::is_same_v<typename
	// std::iterator_traits<It>::iterator_concept,
	//		std::contiguous_iterator_tag>;
	return std::is_same_v<typename std::iterator_traits<It>::iterator_category,
			std::random_access_iterator_tag>;
}

// By calling this, you promise that you've checked whether the iterators are
// truly contiguous.
template <class T, size_t Extent, class Alloc>
constexpr typename std::span<T, Extent>::iterator make_contiguous(
		const jump_span_iterator<T, Extent, Alloc>& it) {
	if (it == it._back_ptr->end()) {
		return it._back_ptr->data().back().end();
	}
	const std::span<T, Extent>& s = it._back_ptr->data()[it._span_idx];
	return s.begin() + it._lcl_idx;
}

template <class It>
constexpr It make_contiguous(It it) {
	// wtf msvc
	// static_assert(
	//		std::is_same_v<typename std::iterator_traits<It>::iterator_category,
	//				std::contiguous_iterator_tag>,
	//		"make_contiguous : Cannot make unknown iterators contiguous.");
	static_assert(
			std::is_same_v<typename std::iterator_traits<It>::iterator_category,
					std::random_access_iterator_tag>,
			"make_contiguous : Cannot make unknown iterators contiguous.");
	return it;
}


template <class T, size_t Extent = std::dynamic_extent,
		class Alloc = std::allocator<std::span<T, Extent>>>
struct jump_span {
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = jump_span_iterator<T, Extent, Alloc>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	static constexpr size_type extent = Extent;

	// Ctors
	constexpr jump_span() noexcept(noexcept(Alloc()))
		requires(extent == 0 || extent == std::dynamic_extent)
	= default;

	constexpr jump_span(
			std::initializer_list<std::span<element_type, extent>>&& list)
			: _spans(list.begin(), list.end()) {
		clean_empty();
	}

	template <size_t N>
	constexpr jump_span(const std::array<value_type, N>& a)
			: _spans({ a.begin(), a.end() }) {
		clean_empty();
	}

	template <size_t N>
	constexpr jump_span(const std::array<std::span<element_type, extent>, N>& a)
			: _spans(a.begin(), a.end()) {
		clean_empty();
	}

	template <class A>
	constexpr jump_span(const std::vector<value_type, A>& v)
			: _spans{ { v.begin(), v.end() } } {
		clean_empty();
	}

	constexpr jump_span(
			const std::vector<std::span<element_type, extent>, Alloc>& v)
			: _spans(v.begin(), v.end()) {
		clean_empty();
	}

	constexpr jump_span(std::span<element_type, extent> s)
			: _spans{ { s } } {
		clean_empty();
	}

	constexpr jump_span(
			const std::span<element_type, extent>* s, size_type count)
			: _spans(s, s + count) {
		clean_empty();
	}

	// Special jump_span functions.
	constexpr void push_back(std::span<element_type, extent> s) {
		if (s.empty()) {
			return;
		}
		_spans.push_back(s);
	}

	// Iterators
	[[nodiscard]] constexpr iterator begin() const noexcept {
		return iterator{ *this, 0, 0 };
	}
	[[nodiscard]] constexpr iterator end() const noexcept {
		return iterator{ *this, _spans.size(), 0 };
	}
	[[nodiscard]] constexpr reverse_iterator rbegin() const noexcept {
		return std::make_reverse_iterator(end());
	}
	[[nodiscard]] constexpr reverse_iterator rend() const noexcept {
		return std::make_reverse_iterator(begin());
	}

	// Element access
	[[nodiscard]] constexpr reference front() const noexcept {
		assert(!empty());
		return _spans.front().front();
	}
	[[nodiscard]] constexpr reference back() const noexcept {
		assert(!empty());
		return _spans.back().back();
	}

	[[nodiscard]] constexpr reference operator[](size_type idx) const noexcept {
		assert(idx < size());

		size_type accum = 0; // modulo is expensive, go with addition.
		for (const std::span<element_type, extent>& s : _spans) {
			size_type lcl_idx = idx - accum;
			if (lcl_idx < s.size()) {
				return s[lcl_idx];
			}
			accum += s.size();
		}
		return _spans.back()[_spans.back().size()];
	}

	[[nodiscard]] constexpr std::span<const std::span<element_type, extent>>
	data() const noexcept {
		return { _spans };
	}

	// Observers
	[[nodiscard]] constexpr size_type size() const noexcept {
		size_type ret = 0;
		for (const std::span<element_type, extent>& s : _spans) {
			ret += s.size();
		}
		return ret;
	}
	[[nodiscard]] constexpr size_type size_bytes() const noexcept {
		size_type ret = 0;
		for (const std::span<element_type, extent>& s : _spans) {
			ret += s.size_bytes();
		}
		return ret;
	}
	[[nodiscard]] constexpr bool empty() const noexcept {
		bool ret = true;
		for (const std::span<element_type, extent>& s : _spans) {
			ret &= s.empty();
		}
		return ret;
	}

	// Subviews
	// Ignored for time-being.

private:
	friend jump_span_iterator<T, Extent, Alloc>;

	// Empty spans cause issues with iterators.
	// Clean the vector to prevent slowing down iterators even more.
	void clean_empty() {
		auto new_end = std::remove_if(_spans.begin(), _spans.end(),
				[](const std::span<T, Extent>& s) { return s.empty(); });
		_spans.erase(new_end, _spans.end());
	}

	std::vector<std::span<T, Extent>, Alloc> _spans;
};
} // namespace fea
