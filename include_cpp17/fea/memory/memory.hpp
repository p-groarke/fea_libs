﻿/**
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
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>

namespace fea {
// Returns rvalue if T has a move constructor or isn't copy constructible.
template <class T>
std::conditional_t<!std::is_move_constructible_v<T>
						   && std::is_copy_constructible_v<T>,
		const T&, T&&>
move_if_moveable(T& t) noexcept;

// Returns rvalue if T has a nothrow move constructor or isn't copy
// constructible.
template <class T>
std::conditional_t<!std::is_nothrow_move_constructible_v<T>
						   && std::is_copy_constructible_v<T>,
		const T&, T&&>
move_if_noexcept_moveable(T& t) noexcept;

// Copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt move_if_moveable(InputIt first, InputIt last, OutputIt dest);

// Copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is nothrow moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt move_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest);

// Backward copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt move_backward_if_moveable(
		InputIt first, InputIt last, OutputIt dest_last);

// Backward copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is nothrow moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt move_backward_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest_last);

// Copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt uninitialized_move_if_moveable(
		InputIt first, InputIt last, OutputIt dest);

// Copies or moves the range to dest.
// If the range is trivially copyable, prefers copy.
// If the range is nothrow moveable, moves it.
// Fallbacks on copy.
template <class InputIt, class OutputIt>
constexpr OutputIt uninitialized_move_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest);

// TODO
//// Backward copies or moves the range to dest.
//// If the range is trivially copyable, prefers copy.
//// If the range is moveable, moves it.
//// Fallbacks on copy.
// template <class InputIt, class OutputIt>
// constexpr OutputIt uninitialized_move_backward_if_moveable(
//		InputIt first, InputIt last, OutputIt dest_last);
//
//// Backward copies or moves the range to dest.
//// If the range is trivially copyable, prefers copy.
//// If the range is nothrow moveable, moves it.
//// Fallbacks on copy.
// template <class InputIt, class OutputIt>
// constexpr OutputIt uninitialized_move_backward_if_noexcept_moveable(
//		InputIt first, InputIt last, OutputIt dest_last);

// Creates move iterator if the underlying value has a move constructor
// or isn't copy constructible.
template <class Iter>
constexpr auto make_move_iterator_if_moveable(Iter it) noexcept;

// Creates move iterator if the underlying value has a nothrow move constructor
// or isn't copy constructible.
template <class Iter>
constexpr auto make_move_iterator_if_noexcept_moveable(Iter it) noexcept;

// Calls std::destroy_at if T is not trivially destructible.
// In debug, zeros memory.
template <class T>
constexpr void destroy_at(T* p) noexcept;

// Calls std::destroy if T is not trivially destructible.
// In debug, zeros memory.
template <class FwdIt>
constexpr void destroy(FwdIt first, FwdIt last) noexcept;

} // namespace fea


/**
 * Implementation
 */
namespace fea {
template <class T>
std::conditional_t<!std::is_move_constructible_v<T>
						   && std::is_copy_constructible_v<T>,
		const T&, T&&>
move_if_moveable(T& t) noexcept {
	return std::move(t);
}

template <class T>
std::conditional_t<!std::is_nothrow_move_constructible_v<T>
						   && std::is_copy_constructible_v<T>,
		const T&, T&&>
move_if_noexcept_moveable(T& t) noexcept {
	return std::move(t);
}

template <class InputIt, class OutputIt>
constexpr OutputIt move_if_moveable(
		InputIt first, InputIt last, OutputIt dest) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_move_constructible_v<in_val_t>) {
		return std::copy(first, last, dest);
	} else {
		return std::move(first, last, dest);
	}
}

template <class InputIt, class OutputIt>
constexpr OutputIt move_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_nothrow_move_constructible_v<in_val_t>) {
		return std::copy(first, last, dest);
	} else {
		return std::move(first, last, dest);
	}
}

template <class InputIt, class OutputIt>
constexpr OutputIt move_backward_if_moveable(
		InputIt first, InputIt last, OutputIt dest_last) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_move_constructible_v<in_val_t>) {
		return std::copy_backward(first, last, dest_last);
	} else {
		return std::move_backward(first, last, dest_last);
	}
}

template <class InputIt, class OutputIt>
constexpr OutputIt move_backward_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest_last) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_nothrow_move_constructible_v<in_val_t>) {
		return std::copy_backward(first, last, dest_last);
	} else {
		return std::move_backward(first, last, dest_last);
	}
}

template <class InputIt, class OutputIt>
constexpr OutputIt uninitialized_move_if_moveable(
		InputIt first, InputIt last, OutputIt dest) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_move_constructible_v<in_val_t>) {
		return std::uninitialized_copy(first, last, dest);
	} else {
		return std::uninitialized_move(first, last, dest);
	}
}

template <class InputIt, class OutputIt>
constexpr OutputIt uninitialized_move_if_noexcept_moveable(
		InputIt first, InputIt last, OutputIt dest) {
	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
	static_assert(std::is_same_v<in_val_t, out_val_t>,
			"fea::copy_or_move only works with identical input and destination "
			"types");

	if constexpr (std::is_trivially_copyable_v<in_val_t>
				  || !std::is_nothrow_move_constructible_v<in_val_t>) {
		return std::uninitialized_copy(first, last, dest);
	} else {
		return std::uninitialized_move(first, last, dest);
	}
}

// template <class InputIt, class OutputIt>
// constexpr OutputIt uninitialized_move_backward_if_moveable(
//		InputIt first, InputIt last, OutputIt dest) {
//	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
//	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
//	static_assert(std::is_same_v<in_val_t, out_val_t>,
//			"fea::copy_or_move only works with identical input and destination "
//			"types");
//
//	if constexpr (std::is_trivially_copyable_v<in_val_t>
//				  || !std::is_move_constructible_v<in_val_t>) {
//		return std::uninitialized_copy_backward(first, last, dest);
//	} else {
//		return std::move_backward(first, last, dest);
//	}
// }
//
// template <class InputIt, class OutputIt>
// constexpr OutputIt uninitialized_move_backward_if_noexcept_moveable(
//		InputIt first, InputIt last, OutputIt dest) {
//	using in_val_t = typename std::iterator_traits<InputIt>::value_type;
//	using out_val_t = typename std::iterator_traits<OutputIt>::value_type;
//	static_assert(std::is_same_v<in_val_t, out_val_t>,
//			"fea::copy_or_move only works with identical input and destination "
//			"types");
//
//	if constexpr (std::is_trivially_copyable_v<in_val_t>
//				  || !std::is_nothrow_move_constructible_v<in_val_t>) {
//		return std::copy_backward(first, last, dest);
//	} else {
//		return std::move_backward(first, last, dest);
//	}
// }

template <class Iter>
constexpr auto make_move_iterator_if_moveable(Iter it) noexcept {
	using val_t = typename std::iterator_traits<Iter>::value_type;
	if constexpr (std::is_move_constructible_v<val_t>
				  || !std::is_copy_constructible_v<val_t>) {
		return std::make_move_iterator(it);
	} else {
		return it;
	}
}

template <class Iter>
constexpr auto make_move_iterator_if_noexcept_moveable(Iter it) noexcept {
	using val_t = typename std::iterator_traits<Iter>::value_type;
	if constexpr (std::is_nothrow_move_constructible_v<val_t>
				  || !std::is_copy_constructible_v<val_t>) {
		return std::make_move_iterator(it);
	} else {
		return it;
	}
}

#if FEA_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
template <class T>
constexpr void destroy_at([[maybe_unused]] T* p) noexcept {
	if constexpr (!std::is_trivially_destructible_v<T>) {
		if constexpr (!std::is_array_v<T>) {
			std::destroy_at(p);
		} else {
			std::destroy(std::begin(*p), std::end(*p));
		}
	}

	if constexpr (fea::debug_build /*&& !std::is_empty_v<T>*/) {
		if constexpr (!std::is_array_v<T>) {
			std::memset(p, 0xa, sizeof(T));
		} else {
			std::memset(*p, 0xa, sizeof(T));
		}
	}
}

template <class FwdIt>
constexpr void destroy(
		[[maybe_unused]] FwdIt first, [[maybe_unused]] FwdIt last) noexcept {
	using val_t = typename std::iterator_traits<FwdIt>::value_type;
	if constexpr (!std::is_trivially_destructible_v<val_t>) {
		std::destroy(first, last);
	}

	if constexpr (fea::debug_build /*&& !std::is_empty_v<val_t>*/) {
		for (; first != last; ++first) {
			std::memset(std::addressof(*first), 0xa, sizeof(val_t));
		}
	}
}
#if FEA_GCC
#pragma GCC diagnostic pop
#endif

// inline size_t page_size()
//{
// #if FEA_WINDOWS
//        SYSTEM_INFO SystemInfo;
//        GetSystemInfo(&SystemInfo);
//        return SystemInfo.dwAllocationGranularity;
// #else
//        return sysconf(_SC_PAGE_SIZE);
// #endif
//}
} // namespace fea
