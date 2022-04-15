/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
#include "fea/numerics/numerics.hpp"

#include <cassert>

/*
clamped_values clamps your fundamental types to a
specified minimum and maximum. The values saturate at both extremities.
*/

namespace fea {
// Runtime defined clamped value.
// Clamps internal value between [minimum, maximum].
// Is implicitely convertible to its underlying type.
// Undefined if minimum > maximum.
// Addition and substraction are mostly overflow safe.
template <class T>
struct clamp_v {
	using value_type = T;

	clamp_v() = default;
	~clamp_v() = default;

	clamp_v(T minimum, T maximum)
			: _minimum(minimum)
			, _maximum(maximum)
			, _value(minimum) {
		assert(_minimum <= _maximum);
	}

	clamp_v(T value, T minimum, T maximum)
			: _minimum(minimum)
			, _maximum(maximum)
			, _value(std::clamp(value, _minimum, _maximum)) {
		assert(_minimum <= _maximum);
	}

	clamp_v(const clamp_v&) = default;
	clamp_v(clamp_v&&) noexcept = default;
	clamp_v& operator=(const clamp_v&) = default;
	clamp_v& operator=(clamp_v&&) noexcept = default;

	clamp_v& operator=(T v) {
		_value = std::clamp(v, _minimum, _maximum);
		return *this;
	}

	// Returns underlying value.
	[[nodiscard]] operator T() const {
		return _value;
	}
	[[nodiscard]] T get() const {
		return _value;
	}

	// Get/set new maximum.
	[[nodiscard]] T maximum() const {
		return _maximum;
	}
	void maximum(T m) {
		assert(_minimum <= m);
		_maximum = m;
		clampit();
	}

	// Get/set new minimum.
	[[nodiscard]] T minimum() const {
		return _minimum;
	}
	void minimum(T m) {
		assert(m <= _maximum);
		_minimum = m;
		clampit();
	}

	clamp_v& operator+=(T v) {
		if constexpr (!std::is_unsigned_v<T>) {
			if (v < 0) {
				return (*this) -= fea::abs(v);
			}
		}

		// Prevent overflows.
		T diff = _maximum - _value;
		if (v > diff) {
			// Case : val + v could overflow
			_value = _maximum;
			return *this;
		}

		_value += v;
		return *this;
	}

	// For unsigned values, prevents underflow.
	clamp_v& operator-=(T v) {
		if constexpr (!std::is_unsigned_v<T>) {
			if (v < 0) {
				return (*this) += fea::abs(v);
			}
		}

		// Prevent underflows.
		T diff = _value - _minimum;
		if (v > diff) {
			_value = _minimum;
			return *this;
		}

		_value -= v;
		return *this;
	}

	clamp_v& operator*=(T v) {
		_value *= v;
		clampit();
		return *this;
	}

	clamp_v& operator/=(T v) {
		_value /= v;
		clampit();
		return *this;
	}

	clamp_v& operator%=(T v) {
		_value %= v;
		clampit();
		return *this;
	}

	clamp_v& operator&=(T v) {
		_value &= v;
		clampit();
		return *this;
	}

	clamp_v& operator|=(T v) {
		_value |= v;
		clampit();
		return *this;
	}

	clamp_v& operator^=(T v) {
		_value ^= v;
		clampit();
		return *this;
	}

	clamp_v& operator<<=(T v) {
		_value <<= v;
		clampit();
		return *this;
	}

	clamp_v& operator>>=(T v) {
		_value >>= v;
		clampit();
		return *this;
	}

	clamp_v& operator++() {
		return (*this) += T(1);
	}
	clamp_v operator++(int) {
		clamp_v ret = *this;
		++(*this);
		return ret;
	}

	clamp_v& operator--() {
		return (*this) -= T(1);
	}
	clamp_v operator--(int) {
		clamp_v ret = *this;
		--(*this);
		return ret;
	}


	// operator~

	// Non-member functions
	// operator+
	// operator-
	// operator*
	// operator/
	// operator%
	// operator&
	// operator|
	// operator^
	// operator<<
	// operator>>
	// operator<<
	// operator>>


private:
	void clampit() {
		_value = std::clamp(_value, _minimum, _maximum);
	}

	T _minimum = {};
	T _maximum = {};
	T _value = {};
};

// Readability alias.
template <class T>
using clamped_value = clamp_v<T>;

} // namespace fea