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
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>

namespace fea {
// Returns rvalue if T has a move constructor.
template <class T>
std::conditional_t<!std::is_move_constructible<T>::value
				&& std::is_copy_constructible<T>::value,
		const T&, T&&>
maybe_move(T& t) noexcept;

// Creates move iterator if the underlying value has a move constructor.
template <class Iter>
constexpr auto maybe_make_move_iterator(Iter it) noexcept;

// Destroys a range if destructors aren't trivial.
template <class FwdIt>
constexpr void destroy(FwdIt begin, FwdIt end);

// Destroys an object if destructor isn't trivial.
template <class T>
constexpr void destroy_at(T* p);

// Destroys a range if destructors aren't trivial.
template <class FwdIt>
constexpr void destroy(FwdIt begin, FwdIt end);

// Copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move(InputIt first, InputIt last, OutputIt dest);

// Backward copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward(
		InputIt first, InputIt last, OutputIt dest_last);

/**
 * Imp
 */
namespace detail {
template <class Iter>
constexpr auto maybe_make_move_iterator(Iter it, std::true_type) noexcept {
	return std::make_move_iterator(it);
}
template <class Iter>
constexpr auto maybe_make_move_iterator(Iter it, std::false_type) noexcept {
	return it;
}


// Non-trivially destructible, array.
template <class T>
constexpr void destroy_at_arr(T* p, std::true_type) {
	destroy(std::begin(*p), std::end(*p));
}

// Non-trivially destructible, non-array.
template <class T>
constexpr void destroy_at_arr(T* p, std::false_type) {
	p->~T();
}

// Trivially destructible.
template <class T>
constexpr void destroy_at(T*, std::true_type) {
	// Nothing to do.
}

// Non-trivially destructible.
template <class T>
constexpr void destroy_at(T* p, std::false_type) {
	destroy_at_arr(p, std::is_array<T>{});
}


// Trivially destructible.
template <class FwdIt>
constexpr void destroy(FwdIt, FwdIt, std::true_type) {
	// Nothing to do.
}

// Non-trivially destructible.
template <class FwdIt>
constexpr void destroy(FwdIt begin, FwdIt end, std::false_type) {
	for (; begin != end; ++begin) {
		fea::destroy_at(std::addressof(*begin));
	}
}


// Moveable
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_moveable(
		InputIt first, InputIt last, OutputIt dest, std::true_type) {
	return std::move(first, last, dest);
}

// Non-moveable
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_moveable(
		InputIt first, InputIt last, OutputIt dest, std::false_type) {
	return std::copy(first, last, dest);
}

// Trivially copyable.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move(
		InputIt first, InputIt last, OutputIt dest, std::true_type) {
	return std::copy(first, last, dest);
}

// Non-Trivially copyable.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move(
		InputIt first, InputIt last, OutputIt dest, std::false_type) {
	using val_t = typename std::iterator_traits<InputIt>::value_type;
	return copy_or_move_moveable(
			first, last, dest, std::is_move_constructible<val_t>{});
}


// Moveable
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward_moveable(
		InputIt first, InputIt last, OutputIt dest_last, std::true_type) {
	return std::move_backward(first, last, dest_last);
}

// Non-moveable
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward_moveable(
		InputIt first, InputIt last, OutputIt dest_last, std::false_type) {
	return std::copy_backward(first, last, dest_last);
}

// Trivially copyable.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward(
		InputIt first, InputIt last, OutputIt dest_last, std::true_type) {
	return std::copy_backward(first, last, dest_last);
}

// Non-Trivially copyable.
template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward(
		InputIt first, InputIt last, OutputIt dest_last, std::false_type) {
	using val_t = typename std::iterator_traits<InputIt>::value_type;
	return copy_or_move_backward_moveable(
			first, last, dest_last, std::is_move_constructible<val_t>{});
}
} // namespace detail

template <class T>
std::conditional_t<!std::is_move_constructible<T>::value
				&& std::is_copy_constructible<T>::value,
		const T&, T&&>
maybe_move(T& t) noexcept {
	return std::move(t);
}

template <class Iter>
constexpr auto maybe_make_move_iterator(Iter it) noexcept {
	using val_t = typename std::iterator_traits<Iter>::value_type;
	return detail::maybe_make_move_iterator(
			it, std::is_move_constructible<val_t>{});
}

template <class T>
constexpr void destroy_at(T* p) {
	detail::destroy_at(p, std::is_trivially_destructible<T>{});
}

template <class FwdIt>
constexpr void destroy(FwdIt begin, FwdIt end) {
	using val_t = typename std::iterator_traits<FwdIt>::value_type;
	detail::destroy(begin, end, std::is_trivially_destructible<val_t>{});
}

template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move(InputIt first, InputIt last, OutputIt dest) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;

	static_assert(std::is_same<in_val_t, out_val_t>::value,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	return detail::copy_or_move(
			first, last, dest, std::is_trivially_copyable<in_val_t>{});
}

template <class InputIt, class OutputIt>
constexpr OutputIt copy_or_move_backward(
		InputIt first, InputIt last, OutputIt dest_last) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;

	static_assert(std::is_same<in_val_t, out_val_t>::value,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	return detail::copy_or_move_backward(
			first, last, dest_last, std::is_trivially_copyable<in_val_t>{});
}

// inline size_t page_size()
//{
//#if FEA_WINDOWS
//        SYSTEM_INFO SystemInfo;
//        GetSystemInfo(&SystemInfo);
//        return SystemInfo.dwAllocationGranularity;
//#else
//        return sysconf(_SC_PAGE_SIZE);
//#endif
//}
} // namespace fea
