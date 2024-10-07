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
#include "fea/utils/platform.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <ratio>
#include <type_traits>

/*
A fixed precision Real number type.

Minimizes precision issues and accelerates performance by using integer
instructions.

The default fea::fixed type uses 23 decimal bits, which gives the same float 32
epsilon.

You may provide 2^n bits Scaling values, or more appropriate scaling values. For
example, use a scaling of 100 when processing currency (2 decimal places).

Tip!
https://en.wikipedia.org/wiki/Fixed-point_arithmetic
Fixed-point formats with scaling factors of the form 2n-1 (namely 1, 3, 7, 15,
31, etc.) have been said to be appropriate for image processing and other
digital signal processing tasks.
*/

namespace fea {

template <size_t NumBits>
struct bits {};

namespace detail {
// Returns true if integer value is a power of 2.
// https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
//
// TODO : Put where? Unit test.
template <class T>
constexpr bool is_pow2(T v) noexcept {
	static_assert(std::is_integral_v<T>, "fea::is_pow2 : T must be integer.");
	return v && !(v & (v - 1));
}

template <class T>
constexpr T msqrt(T v) noexcept {
	T mask = T(1);
	for (size_t i = 0; i < sizeof(T) * 8; ++i) {
		if (v & (mask << i)) {
			return T(i);
		}
	}
	return T(0);
}

template <size_t Scaling, class IntT>
struct fixed {
	static_assert(std::is_integral_v<IntT>,
			"fea::fixed : IntT must be integral type.");
	// static_assert(std::is_signed_v<IntT>,
	//		"fea::fixed : IntT must be signed integral type.");

	using value_t = IntT;
	static constexpr value_t scaling_v = value_t(Scaling);

	// Enable some bit-shifting optimizations if scaling is 2^n.
	static constexpr bool scaling_is_pow2_v = fea::detail::is_pow2(scaling_v);
	static constexpr value_t scaling_sqrt_v
			= scaling_is_pow2_v ? msqrt(scaling_v) : value_t(0);

private:
	static constexpr float _float_to_int_v = float(scaling_v);
	static constexpr float _int_to_float_v = 1.f / float(scaling_v);
	static constexpr double _double_to_int_v = double(scaling_v);
	static constexpr double _int_to_double_v = 1.0 / double(scaling_v);

public:
	// Ctors
	~fixed() noexcept = default;
	constexpr fixed(const fixed&) noexcept = default;
	constexpr fixed(fixed&&) noexcept = default;
	constexpr fixed& operator=(const fixed&) noexcept = default;
	constexpr fixed& operator=(fixed&&) noexcept = default;

	constexpr fixed() noexcept
			: value(value_t{}) {
	}

	constexpr fixed(float f) noexcept
			// Keep it simple and fast for now.
			// Could do modf for more fractional precision.
			: value(value_t(f * _float_to_int_v)) {
	}

	constexpr fixed(double d) noexcept
			: value(value_t(d * _double_to_int_v)) {
	}

	constexpr explicit fixed(value_t v) noexcept {
		if constexpr (scaling_is_pow2_v) {
			value = v << scaling_sqrt_v;
		} else {
			value = v * scaling_v;
		}
	}

	// constexpr fixed& operator=(float f) noexcept {
	//	_num = value_t(f * _float_to_int_v);
	//	return *this;
	// }
	// constexpr fixed& operator=(double d) noexcept {
	//	_num = value_t(d * _double_to_int_v);
	//	return *this;
	// }
	// constexpr fixed& operator=(value_t v) noexcept {
	//	_num = v;
	//	return *this;
	// }

	// Conversions
	constexpr explicit operator float() const noexcept {
		return float(value) * _int_to_float_v;
	}

	constexpr explicit operator double() const noexcept {
		return double(value) * _int_to_double_v;
	}

	constexpr explicit operator value_t() const noexcept {
		if constexpr (scaling_is_pow2_v) {
			return value >> scaling_sqrt_v;
		} else {
			return value / scaling_v;
		}
	}

	// Comparison Operators
	constexpr friend bool operator==(fixed lhs, fixed rhs) noexcept {
		return lhs.value == rhs.value;
	}

	constexpr friend bool operator!=(fixed lhs, fixed rhs) noexcept {
		return !(lhs == rhs);
	}

	constexpr friend bool operator<(fixed lhs, fixed rhs) noexcept {
		return lhs.value < rhs.value;
	}

	constexpr friend bool operator>(fixed lhs, fixed rhs) noexcept {
		return rhs < lhs;
	}

	constexpr friend bool operator<=(fixed lhs, fixed rhs) noexcept {
		return !(rhs < lhs);
	}

	constexpr friend bool operator>=(fixed lhs, fixed rhs) noexcept {
		return !(lhs < rhs);
	}

	// Arithmetic Operators
	constexpr friend fixed operator+(fixed lhs, fixed rhs) noexcept {
		fixed ret;
		ret.value = lhs.value + rhs.value;
		return ret;
	}

	constexpr friend fixed operator-(fixed lhs, fixed rhs) noexcept {
		fixed ret;
		ret.value = lhs.value - rhs.value;
		return ret;
	}

	constexpr friend fixed operator*(fixed lhs, fixed rhs) noexcept {
		// (int * int) * (1 / scaling * 1 / scaling)
		// Both our scaling are the same, so simply rectify.

		fixed ret;
		if constexpr (scaling_is_pow2_v) {
			ret.value = (lhs.value * rhs.value) >> scaling_sqrt_v;
		} else {
			ret.value = (lhs.value * rhs.value) / scaling_v;
		}
		return ret;
	}

	constexpr friend fixed operator/(fixed lhs, fixed rhs) noexcept {
		fixed ret;
		if constexpr (scaling_is_pow2_v) {
			ret.value = (lhs.value << scaling_sqrt_v) / rhs.value;
		} else {
			ret.value = (lhs.value * scaling_v) / rhs.value;
		}
		return ret;
	}

	constexpr friend fixed operator%(fixed lhs, fixed rhs) noexcept {
		fixed ret;
		ret.value = lhs.value % rhs.value;
		return ret;
	}

	// Public for serialization purposes.
	value_t value;
};
} // namespace detail


#if FEA_ARCH >= 64
// float32 decimal precision.
using fixed = detail::fixed<(size_t(1) << 23), std::intmax_t>;
#else
// Likely a bad idea, provided for completeness.
using fixed = detail::fixed<(size_t(1) << 15), std::intmax_t>;
#endif

using currency = detail::fixed<100, std::intmax_t>;

} // namespace fea

#include "imp/fixed.imp.hpp"
