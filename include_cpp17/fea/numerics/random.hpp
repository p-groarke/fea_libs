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
#include "fea/meta/traits.hpp"
#include "fea/utility/platform.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <limits>
#include <numeric>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace fea {
// Get a random value between [min, max].
// Handles all integer types, floating types and enums.
template <class T>
T random_val(T min, T max);

// Get a random value between [T::min, T::max].
// Handles integers, floats and enums.
// For enums, returns value between [E{}, E::count[
template <class T>
T random_val();

// Fills a range, from begin to end, with random values between [min, max].
// Iterator value type should be supported by fea::random_val.
template <class FwdIt>
void random_fill(FwdIt begin, FwdIt end, fea::iterator_value_t<FwdIt> min,
		fea::iterator_value_t<FwdIt> max);

// Fills a range, from begin to end, with random values between
// [numeric_limits::lowest, numeric_limits::max].
// Iterator value type should be supported by fea::random_val.
template <class FwdIt>
void random_fill(FwdIt begin, FwdIt end);

// Get a random index, [0, count[
inline size_t random_idx(size_t count);

// Get a random iterator, [first, last[
template <class FwdIt>
FwdIt random_iter(FwdIt first, FwdIt last);

// Fills the range with random indexes from [0, count[
template <class FwdIt>
void random_idxes(FwdIt begin, FwdIt end, size_t count);

// Get some random bytes.
template <size_t N>
std::array<uint8_t, N> random_bytes();

// Get some random bytes.
inline std::vector<uint8_t> random_bytes(size_t num_bytes);

// Shuffles items in range [begin, end[
template <class FwdIt>
void random_shuffle(FwdIt begin, FwdIt end);

// Shuffles items in container.
template <class Container>
void random_shuffle(Container& cont);

// Given the available sum, fills your range with n values
// so that : v0 + v1 + ... + v(n-1) == sum.
// https://stackoverflow.com/a/8064754
// The iterators must return the same type as sum.
// The range must be sortable.
template <class BidirIt, class T>
void random_fixed_sum(BidirIt begin, BidirIt end, T sum);

// Given the available sum, returns a vector with 'count' values
// so that : v0 + v1 + ... + v(n-1) == 'sum'.
// https://stackoverflow.com/a/8064754
// Returns a vector filled with 'count' values.
template <class T>
std::vector<T> random_fixed_sum(T sum, size_t count);

// Given the available sum, returns a vector with 'count' values
// so that : v0 + v1 + ... + v(n-1) == 'sum'.
// https://stackoverflow.com/a/8064754
template <size_t N, class T>
std::array<T, N> random_fixed_sum(T sum);
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <size_t Bytes>
struct platform_mersenne;

template <>
struct platform_mersenne<4> {
	using type = std::mt19937;
};

template <>
struct platform_mersenne<8> {
	using type = std::mt19937_64;
};

using platform_mt19937 =
		typename detail::platform_mersenne<sizeof(size_t)>::type;

inline std::random_device rd;
inline platform_mt19937 gen(
		size_t(std::chrono::system_clock::now().time_since_epoch().count()));
} // namespace detail

template <class T>
T random_val(T min, T max) {
	if constexpr (std::is_floating_point_v<T>) {
		std::uniform_real_distribution<T> dist(min, max);
		return dist(detail::gen);
	} else if constexpr (sizeof(T) == 1) {
		if constexpr (std::is_unsigned_v<T>) {
			return T(fea::random_val(uint16_t(min), uint16_t(max)));
		} else {
			return T(fea::random_val(short(min), short(max)));
		}
	} else if constexpr (std::is_enum_v<T>) {
		using u_t = std::underlying_type_t<T>;
		return T(fea::random_val(u_t(min), u_t(max)));
	} else {
		std::uniform_int_distribution<T> dist(min, max);
		return dist(detail::gen);
	}
}

template <class T>
T random_val() {
	if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
		return T(fea::random_val(uint8_t(0), uint8_t(1)));
	} else if constexpr (sizeof(T) == 1) {
		if constexpr (std::is_unsigned_v<T>) {
			constexpr uint16_t mmin
					= uint16_t(std::numeric_limits<T>::lowest());
			constexpr uint16_t mmax = uint16_t((std::numeric_limits<T>::max)());
			return T(fea::random_val(mmin, mmax));
		} else {
			constexpr short mmin = short(std::numeric_limits<T>::lowest());
			constexpr short mmax = short((std::numeric_limits<T>::max)());
			return T(fea::random_val(mmin, mmax));
		}
	} else if constexpr (std::is_enum_v<T>) {
		using u_t = std::underlying_type_t<T>;
		return fea::random_val(T{}, T(u_t(T::count) - 1));
	} else {
		return fea::random_val(std::numeric_limits<T>::lowest(),
				(std::numeric_limits<T>::max)());
	}
}

//template <class Container>
//const auto& random_val(const Container& container) {
//	return *random_iter(container.begin(), container.end());
//}
//
//template <class Container>
//auto& random_val(Container& container) {
//	return *random_iter(container.begin(), container.end());
//}

template <class FwdIt>
void random_fill(FwdIt begin, FwdIt end, fea::iterator_value_t<FwdIt> min,
		fea::iterator_value_t<FwdIt> max) {
	for (; begin != end; ++begin) {
		*begin = fea::random_val(min, max);
	}
}

template <class FwdIt>
void random_fill(FwdIt begin, FwdIt end) {
	using value_t = fea::iterator_value_t<FwdIt>;
	if constexpr (std::is_floating_point_v<value_t>) {
		// Visual studio asserts : max <= min + max, aka half the range.
		constexpr value_t mmin
				= std::numeric_limits<value_t>::lowest()
				+ ((std::numeric_limits<value_t>::max)() * value_t(0.5));
		constexpr value_t mmax = (std::numeric_limits<value_t>::max)() * value_t(0.5);
		fea::random_fill(begin, end, mmin, mmax);
	} else {
		fea::random_fill(begin, end, std::numeric_limits<value_t>::lowest(),
				(std::numeric_limits<value_t>::max)());
	}
}

size_t random_idx(size_t count) {
	return fea::random_val(size_t(0), count - 1);
}

template <class FwdIt>
FwdIt random_iter(FwdIt first, FwdIt last) {
	size_t count = std::distance(first, last);
	size_t idx = fea::random_idx(count);
	return std::next(first, idx);
}

template <class FwdIt>
void random_idxes(FwdIt begin, FwdIt end, size_t count) {
	for (auto it = begin; it != end; ++it) {
		*it = fea::random_idx(count);
	}
}

template <size_t N>
std::array<uint8_t, N> random_bytes() {
	std::array<uint8_t, N> ret{};
	for (size_t i = 0; i < N; ++i) {
		ret[i] = fea::random_val<uint8_t>();
	}
	return ret;
}

std::vector<uint8_t> random_bytes(size_t num_bytes) {
	std::vector<uint8_t> ret;
	ret.reserve(num_bytes);
	for (size_t i = 0; i < num_bytes; ++i) {
		ret.push_back(fea::random_val<uint8_t>());
	}
	return ret;
}

template <class FwdIt>
void random_shuffle(FwdIt begin, FwdIt end) {
	std::shuffle(begin, end, detail::gen);
}

template <class Container>
void random_shuffle(Container& cont) {
	return fea::random_shuffle(cont.begin(), cont.end());
}

template <class BidirIt, class T>
void random_fixed_sum(BidirIt begin, BidirIt end, T sum) {
	static_assert(std::is_same_v<std::decay_t<decltype(*begin)>, T>,
			"fea::random_from_available : iterators must return same type as "
			"sum");
	assert(std::distance(begin, end) >= 2);

	// First, generate n - 1 random numbers that are <= sum.
	for (auto it = begin; it != end - 1; ++it) {
		*it = random_val(T(0), sum);
	}

	// Assign the sum to last.
	*(end - 1) = sum;

	// Then sort it.
	std::sort(begin, end);

	// Now, going backwards (so as to not erase our values),
	// assign our final number. We do that by substracting with
	// the previous number.
	auto rbegin = std::make_reverse_iterator(end);
	auto rend = std::make_reverse_iterator(begin);
	for (auto it = rbegin; it != rend - 1; ++it) {
		*it = *it - *(it + 1);
	}
}

template <class T>
std::vector<T> random_fixed_sum(T sum, size_t count) {
	std::vector<T> ret(count);
	random_fixed_sum(ret, sum);
	return ret;
}

template <size_t N, class T>
std::array<T, N> random_fixed_sum(T sum) {
	std::array<T, N> ret{};
	random_fixed_sum(ret.begin(), ret.end(), sum);
	return ret;
}

} // namespace fea
