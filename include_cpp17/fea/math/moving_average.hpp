/*
BSD 3-Clause License

Copyright (c) 2023, Philippe Groarke
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
#include "fea/math/math.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>

/*
Averages.

TODO : linear regression moving average,
moving median, ...
https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average
https://www.alglib.net/time-series/moving-average-filter.php
https://www.incrediblecharts.com/indicators/linear_regression.php
*/


namespace fea {
// Computes the cumulative average (rolling / running average).
// Providing n makes the computation bounded, else it is unbounded (infinite).
//
// Nothing special here, your typical mean given N samples, but only stores 1
// value.
//
// This can become quite imprecise, and it is recommended to use a different
// moving average algorithm.
template <class T>
struct cumulative_average {
	static constexpr bool is_int_v = std::is_integral_v<T>;
	using mfloat_t = std::conditional_t<sizeof(T) == 8, double, float>;

	cumulative_average(size_t n) noexcept
			: _n(n) {
	}
	cumulative_average() noexcept = default;

	T operator()(const T& in) {
		// Unbounded.
		if (_n == 0 || _size != _n) {
			_last = (mfloat_t(in) + mfloat_t(_size) * _last)
					/ mfloat_t(_size + 1);
			++_size;
			if constexpr (is_int_v) {
				return T(std::round(_last));
			} else {
				return T(_last);
			}
		}

		// Bounded.
		_last -= _last / _n;
		_last += mfloat_t(in) / _n;

		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

private:
	size_t _n = 0;
	size_t _size = 0;
	mfloat_t _last = 0;
};

template <class T>
using ca = cumulative_average<T>;


// Computes the simple moving average of N samples.
// Uncentered.
template <class T, size_t N>
struct simple_moving_average {
	static constexpr bool is_int_v = std::is_integral_v<T>;
	using mfloat_t = std::conditional_t<sizeof(T) == 8, double, float>;

	simple_moving_average() noexcept = default;

	T operator()(const T& in) {
		if (_size != _circle_buf.size()) {
			// cumulative
			_circle_buf[_size++] = mfloat_t(in);
			_last = fea::mean(_circle_buf.begin(), _circle_buf.begin() + _size);

			if constexpr (is_int_v) {
				return T(std::round(_last));
			} else {
				return T(_last);
			}
		}

		// 1/k * (pn+1 - p1)
		mfloat_t new_v = (mfloat_t(in) - _circle_buf[_playhead]) * _divider;
		_last = _last + new_v;

		_circle_buf[_playhead] = mfloat_t(in);
		_playhead = _playhead + 1 == _circle_buf.size() ? 0 : _playhead + 1;

		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

private:
	mfloat_t _divider = mfloat_t(1) / mfloat_t(N);
	mfloat_t _last = mfloat_t(0);
	size_t _playhead = 0;
	size_t _size = 0;
	std::array<mfloat_t, N> _circle_buf{};
};

template <class T, size_t N>
using sma = simple_moving_average<T, N>;


// Computes the exponential moving average.
// Alpha defaults to 0.5 if it isn't provided.
template <class T>
struct exponential_moving_average {
	static constexpr bool is_int_v = std::is_integral_v<T>;
	using mfloat_t = std::conditional_t<sizeof(T) == 8, double, float>;

	exponential_moving_average(mfloat_t alpha, T first_value) noexcept
			: _alpha(alpha)
			, _last(mfloat_t(first_value)) {
		assert(_alpha > mfloat_t(0) && _alpha < mfloat_t(1));
	}
	exponential_moving_average(mfloat_t alpha) noexcept
			: exponential_moving_average(alpha, mfloat_t(0)) {
	}
	exponential_moving_average() noexcept
			: exponential_moving_average(mfloat_t(0.5), mfloat_t(0)) {
	}

	T operator()(const T& in) {
		mfloat_t in_f = mfloat_t(in);
		_last = (in_f * _alpha) + _last * _alpha_inv;
		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

private:
	mfloat_t _alpha = 0.5;
	mfloat_t _alpha_inv = mfloat_t(1) - _alpha;
	mfloat_t _last = 0.0;
};

template <class T>
using ema = exponential_moving_average<T>;


template <class T, size_t N>
struct weighted_moving_average {
	static constexpr bool is_int_v = std::is_integral_v<T>;
	using mfloat_t = std::conditional_t<sizeof(T) == 8, double, float>;

	T operator()(const T& in) {
		using msize_t = typename std::make_signed<size_t>::type;

		if (_size != _circle_buf.size()) {
			_circle_buf[_size++] = mfloat_t(in);

			mfloat_t ret = mfloat_t(0);
			mfloat_t denom = (_size * (_size + 1)) / mfloat_t(2);
			for (msize_t i = _size - 1; i >= 0; --i) {
				ret += _circle_buf[i] * mfloat_t(i + 1);
			}
			ret /= denom;

			if constexpr (is_int_v) {
				return T(std::round(ret));
			} else {
				return T(ret);
			}
		}

		_circle_buf[_playhead] = mfloat_t(in);
		_playhead = _playhead + 1 == _circle_buf.size() ? 0 : _playhead + 1;

		mfloat_t ret = mfloat_t(0);
		mfloat_t w = mfloat_t(1);
		for (size_t i = _playhead; i < _circle_buf.size(); ++i) {
			ret += _circle_buf[i] * w++;
		}
		for (size_t i = 0; i < _playhead; ++i) {
			ret += _circle_buf[i] * w++;
		}
		ret /= _denom;

		if constexpr (is_int_v) {
			return T(std::round(ret));
		} else {
			return T(ret);
		}
	}

private:
	mfloat_t _denom = (N * (N + 1)) / mfloat_t(2);
	size_t _playhead = 0;
	size_t _size = 0;
	std::array<mfloat_t, N> _circle_buf{};
};

template <class T, size_t N>
using wma = weighted_moving_average<T, N>;
} // namespace fea