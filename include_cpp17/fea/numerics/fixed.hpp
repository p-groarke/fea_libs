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
A basic_fixed precision Real number type.

Minimizes precision issues and accelerates performance by using integer
instructions.

The default fea::basic_fixed type uses 23 decimal bits, which has the same
precision (epsilon) as float32.

You may provide 2^n bits Scaling values, or more appropriate scaling values. For
example, use a scaling of 100 when processing currency (2 decimal places).

Tip : https://en.wikipedia.org/wiki/Fixed-point_arithmetic
Fixed-point formats with scaling factors of the form 2n-1 (namely 1, 3, 7, 15,
31, etc.) have been said to be appropriate for image processing and other
digital signal processing tasks.
*/

namespace fea {
namespace detail {
// Returns true if integer value is a power of 2.
// https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
//
// TODO : Put where? Add unit tests.
template <class T>
constexpr bool mis_pow2(T v) noexcept {
	static_assert(std::is_integral_v<T>, "fea::mis_pow2 : T must be integer.");
	return v && !(v & (v - 1));
}

template <class T>
constexpr T msqrt_base2(T v) noexcept {
	static_assert(
			std::is_integral_v<T>, "fea::msqrt_base2 : T must be integer.");

	T mask = T(1);
	for (size_t i = 0; i < sizeof(T) * 8; ++i) {
		if (v & (mask << i)) {
			return T(i);
		}
	}
	return T(0);
}
} // namespace detail

template <class IntT, size_t Scaling>
struct basic_fixed {
	static_assert(std::is_integral_v<IntT>,
			"fea::basic_fixed : IntT must be integral type.");

	using value_t = IntT;
	static constexpr value_t scaling_v = value_t(Scaling);

	// Enable some bit-shifting optimizations if scaling is 2^n.
	static constexpr bool scaling_is_pow2_v = detail::mis_pow2(scaling_v);
	static constexpr value_t scaling_sqrt_v
			= scaling_is_pow2_v ? detail::msqrt_base2(scaling_v) : value_t(0);

private:
	static constexpr float _float_to_int_v = float(scaling_v);
	static constexpr float _int_to_float_v = 1.f / float(scaling_v);
	static constexpr double _double_to_int_v = double(scaling_v);
	static constexpr double _int_to_double_v = 1.0 / double(scaling_v);

public:
	// Ctors
	constexpr basic_fixed() noexcept = default;
	~basic_fixed() noexcept = default;
	constexpr basic_fixed(const basic_fixed&) noexcept = default;
	constexpr basic_fixed(basic_fixed&&) noexcept = default;
	constexpr basic_fixed& operator=(const basic_fixed&) noexcept = default;
	constexpr basic_fixed& operator=(basic_fixed&&) noexcept = default;

	constexpr basic_fixed(float f) noexcept;
	constexpr basic_fixed(double d) noexcept;
	explicit constexpr basic_fixed(value_t v) noexcept;

	// Conversions
	explicit constexpr operator float() const noexcept;
	explicit constexpr operator double() const noexcept;
	explicit constexpr operator value_t() const noexcept;

	// Comparison Operators
	friend constexpr bool operator==(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		return lhs.value == rhs.value;
	}

	friend constexpr bool operator!=(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		return !(lhs == rhs);
	}

	friend constexpr bool operator<(basic_fixed lhs, basic_fixed rhs) noexcept {
		return lhs.value < rhs.value;
	}

	friend constexpr bool operator>(basic_fixed lhs, basic_fixed rhs) noexcept {
		return rhs < lhs;
	}

	friend constexpr bool operator<=(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		return !(rhs < lhs);
	}

	friend constexpr bool operator>=(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		return !(lhs < rhs);
	}

	// Arithmetic Operators
	friend constexpr basic_fixed operator+(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		ret.value = lhs.value + rhs.value;
		return ret;
	}

	friend constexpr basic_fixed operator-(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		ret.value = lhs.value - rhs.value;
		return ret;
	}

	friend constexpr basic_fixed operator*(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		if constexpr (scaling_is_pow2_v) {
			ret.value = (lhs.value * rhs.value) >> scaling_sqrt_v;
		} else {
			ret.value = (lhs.value * rhs.value) / scaling_v;
		}
		return ret;
	}

	friend constexpr basic_fixed operator/(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		if constexpr (scaling_is_pow2_v) {
			ret.value = (lhs.value << scaling_sqrt_v) / rhs.value;
		} else {
			ret.value = (lhs.value * scaling_v) / rhs.value;
		}
		return ret;
	}

	friend constexpr basic_fixed operator%(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		ret.value = lhs.value % rhs.value;
		return ret;
	}

	// Public for serialization purposes.
	value_t value = value_t(0);
};


#if FEA_ARCH == 64
// float32 decimal precision.
using fixed = fea::basic_fixed<int64_t, (size_t(1) << 23)>;
#elif FEA_ARCH == 32
// A bad idea, provided for completeness.
using fixed = fea::basic_fixed<int32_t, (size_t(1) << 11)>;
#else
static_assert(false, "fea::fixed : Missing architecture.");
#endif

// std::intmax_t == 8 bytes on win32...
using currency = fea::basic_fixed<std::intptr_t, 100>;

} // namespace fea

#include "imp/fixed.imp.hpp"
