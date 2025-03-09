/*
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
#include "fea/math/statistics.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>

/*
Moving averages.
When in doubt, use exponential moving average or moving median.
https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average

TODO : linear regression moving average?
https://www.incrediblecharts.com/indicators/linear_regression.php
*/

namespace fea {
namespace detail {
// TODO : use fea::fixed
template <class T>
struct moving_avg_base;
} // namespace detail


// Computes the cumulative average (rolling / running average).
// Providing n makes the computation bounded, else it is unbounded (infinite).
//
// Nothing special here, your typical mean given N samples, but only stores 1
// value.
//
// This can become quite imprecise, and it is recommended to use a different
// moving average algorithm.
template <class T>
struct cumulative_average : detail::moving_avg_base<T> {
	using typename detail::moving_avg_base<T>::mfloat_t;
	using detail::moving_avg_base<T>::is_int_v;
	using detail::moving_avg_base<T>::get;

	cumulative_average() noexcept = default;

	// Prime with n values. The first added value will start here.
	cumulative_average(size_t n) noexcept;

	// Push a value into the average. Returns newly computed average.
	T operator()(const T& in);

private:
	using detail::moving_avg_base<T>::prime;
	using detail::moving_avg_base<T>::_last;

	size_t _n = 0;
	size_t _size = 0;
};


// Computes the simple moving average of N samples.
// Uncentered.
template <class T, size_t N>
struct simple_moving_average : detail::moving_avg_base<T> {
	using typename detail::moving_avg_base<T>::mfloat_t;
	using detail::moving_avg_base<T>::is_int_v;
	using detail::moving_avg_base<T>::get;

	simple_moving_average() noexcept = default;

	// Push a value into the average. Returns newly computed average.
	T operator()(const T& in);

private:
	using detail::moving_avg_base<T>::prime;
	using detail::moving_avg_base<T>::_last;

	mfloat_t _divider = mfloat_t(1) / mfloat_t(N);
	size_t _playhead = 0;
	size_t _size = 0;
	std::array<mfloat_t, N> _circle_buf{};
};


// Computes the exponential moving average.
// Alpha defaults to 0.5 if it isn't provided.
template <class T>
struct exponential_moving_average : detail::moving_avg_base<T> {
	using typename detail::moving_avg_base<T>::mfloat_t;
	using detail::moving_avg_base<T>::is_int_v;
	using detail::moving_avg_base<T>::get;

	exponential_moving_average() noexcept = default;

	// Prime with a different alpha. By default, uses 0.5.
	exponential_moving_average(mfloat_t alpha) noexcept;

	// Push a value into the average. Returns newly computed average.
	T operator()(const T& in);

private:
	using detail::moving_avg_base<T>::_last;

	mfloat_t _alpha = mfloat_t(0.5);
	mfloat_t _alpha_inv = mfloat_t(1) - _alpha;
};


// Computes the weighted moving average.
template <class T, size_t N>
struct weighted_moving_average : detail::moving_avg_base<T> {
	using typename detail::moving_avg_base<T>::mfloat_t;
	using detail::moving_avg_base<T>::is_int_v;
	using detail::moving_avg_base<T>::get;

	// Push a value into the average. Returns newly computed average.
	T operator()(const T& in);

private:
	using detail::moving_avg_base<T>::prime;
	using detail::moving_avg_base<T>::_last;

	mfloat_t _denom = (N * (N + 1)) / mfloat_t(2);
	size_t _playhead = 0;
	size_t _size = 0;
	std::array<mfloat_t, N> _circle_buf{};
};


// Compute moving median.
// More stable than averages, but heavier.
// If you use an even sample size, the average of central values is used.
template <class T, size_t N>
struct moving_median : detail::moving_avg_base<T> {
	using typename detail::moving_avg_base<T>::mfloat_t;
	using detail::moving_avg_base<T>::is_int_v;
	using detail::moving_avg_base<T>::get;

	// Push a value into the average. Returns newly computed average.
	T operator()(const T& in);

private:
	using detail::moving_avg_base<T>::prime;
	using detail::moving_avg_base<T>::_last;

	size_t _playhead = 0;
	size_t _size = 0;
	std::array<mfloat_t, N> _circle_buf{};
	std::array<mfloat_t, N> _sorted{};
};


// Abbreviations / accronyms.
// Enable with :
// namespace fea { using namespace fea::...::abbrev; }
namespace moving_average {
namespace abbrev {
template <class T>
using ca = cumulative_average<T>;

template <class T, size_t N>
using sma = simple_moving_average<T, N>;

template <class T>
using ema = exponential_moving_average<T>;

template <class T, size_t N>
using wma = weighted_moving_average<T, N>;

template <class T, size_t N>
using mm = moving_median<T, N>;

} // namespace abbrev
} // namespace moving_average
} // namespace fea

#include "imp/moving_average.imp.hpp"