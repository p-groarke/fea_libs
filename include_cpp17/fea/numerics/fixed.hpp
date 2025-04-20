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
#include "fea/numerics/numerics.hpp"
#include "fea/utility/platform.hpp"

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
template <class IntT, size_t Scaling>
struct basic_fixed {
	static_assert(std::is_integral_v<IntT>,
			"fea::basic_fixed : IntT must be integral type.");

	using value_t = IntT;
	static constexpr value_t scaling_v = value_t(Scaling);

	// Enable some bit-shifting optimizations if scaling is 2^n.
	static constexpr bool is_scaling_pow2_v = fea::is_pow2<scaling_v>();
	static constexpr value_t scaling_sqrt_v
			= is_scaling_pow2_v ? fea::sqrt<scaling_v>() : value_t(0);

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

	// Unfortunately, the friend operators MUST be "hidden friends", or else all
	// hell breaks loose.

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
		if constexpr (is_scaling_pow2_v) {
			ret.value = (lhs.value * rhs.value) >> scaling_sqrt_v;
		} else {
			ret.value = (lhs.value * rhs.value) / scaling_v;
		}
		return ret;
	}
	friend constexpr basic_fixed operator/(
			basic_fixed lhs, basic_fixed rhs) noexcept {
		basic_fixed ret;
		if constexpr (is_scaling_pow2_v) {
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

	// TODO : tests
	// constexpr basic_fixed& operator+=(basic_fixed rhs) noexcept {
	//	value += rhs.value;
	//	return *this;
	//}

	// constexpr basic_fixed& operator-=(basic_fixed rhs) noexcept {
	//	value -= rhs.value;
	//	return *this;
	// }

	// constexpr basic_fixed& operator*=(basic_fixed rhs) noexcept {
	//	if constexpr (scaling_is_pow2_v) {
	//		value = (value * rhs.value) >> scaling_sqrt_v;
	//	} else {
	//		value = (value * rhs.value) / scaling_v;
	//	}
	//	return *this;
	// }

	// constexpr basic_fixed& operator/=(basic_fixed rhs) noexcept {
	//	if constexpr (scaling_is_pow2_v) {
	//		value = (value << scaling_sqrt_v) / rhs.value;
	//	} else {
	//		value = (value * scaling_v) / rhs.value;
	//	}
	//	return *this;
	// }

	// constexpr basic_fixed& operator%=(basic_fixed rhs) noexcept {
	//	value %= rhs.value;
	//	return *this;
	// }

	// Public for serialization purposes.
	value_t value = value_t(0);
};


#if FEA_ARCH == 64
// The architecture "native" type.
// float32 decimal precision.
using fixed = fea::basic_fixed<int64_t, (size_t(1) << 23)>;
#elif FEA_ARCH == 32
// The architecture "native" type.
// Mostly a bad idea in 32 bits, provided for completeness.
using fixed = fea::basic_fixed<int32_t, (size_t(1) << 11)>;
#else
static_assert(false, "fea::fixed : Missing architecture.");
#endif

// std::intmax_t == 8 bytes on win32...
using currency = fea::basic_fixed<std::intptr_t, 100>;
using fixed32_t = fea::basic_fixed<int32_t, (size_t(1) << 11)>;
#if FEA_ARCH == 64
using fixed64_t = fea::basic_fixed<int64_t, (size_t(1) << 23)>;
#endif
} // namespace fea


// Implementation
namespace fea {
template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(float f) noexcept
		: value(value_t(f * _float_to_int_v)) {
	// Keep it simple and fast for now.
	// Could do modf for more fractional precision.
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(double d) noexcept
		: value(value_t(d * _double_to_int_v)) {
}

template <bool B, class TrueT, class FalseT>
constexpr auto mcexpr_if(TrueT lhs, FalseT rhs) {
	if constexpr (B) {
		return lhs;
	} else {
		return rhs;
	}
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::basic_fixed(value_t v) noexcept {
	if constexpr (is_scaling_pow2_v) {
		value = v << scaling_sqrt_v;
	} else {
		value = v * scaling_v;
	}
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator float() const noexcept {
	return float(value) * _int_to_float_v;
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator double() const noexcept {
	return double(value) * _int_to_double_v;
}

template <class I, size_t S>
constexpr basic_fixed<I, S>::operator value_t() const noexcept {
	if constexpr (is_scaling_pow2_v) {
		return value >> scaling_sqrt_v;
	} else {
		return value / scaling_v;
	}
}
} // namespace fea


namespace std {
template <class T, size_t S>
class numeric_limits<fea::basic_fixed<T, S>> {
	using value_t = fea::basic_fixed<T, S>;

public:
	// Member constants
	static constexpr bool is_specialized = true;
	static constexpr bool is_signed = std::is_signed_v<T>;
	static constexpr bool is_integer = false;
	static constexpr bool is_exact = true;
	static constexpr bool has_infinity = false;
	static constexpr bool has_quiet_NaN = false;
	static constexpr bool has_signaling_NaN = false;
	static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
	static constexpr bool has_denorm_loss = false;
	static constexpr std::float_round_style round_style
			= std::round_toward_zero;
	static constexpr bool is_iec559 = false;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = std::numeric_limits<T>::is_modulo;

	static constexpr int digits = std::numeric_limits<T>::digits;
	static constexpr int digits10 = std::numeric_limits<T>::digits10;
	static constexpr int max_digits10 = std::numeric_limits<T>::max_digits10;
	static constexpr int radix = 2;

	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;
	static constexpr bool traps = std::numeric_limits<T>::traps;
	static constexpr bool tinyness_before = false;

	// Member functions
	static constexpr value_t min() noexcept {
		// Even though it isn't necessary, since we can perfectly represent
		// zero. Return value closest to zero (0), to allow interchange with
		// floating types.
		value_t ret;
		ret.value = T(0);
		// ret.value = (std::numeric_limits<T>::min)();
		return ret;
	}

	static constexpr value_t lowest() noexcept {
		value_t ret;
		ret.value = std::numeric_limits<T>::lowest();
		return ret;
	}

	static constexpr value_t max() noexcept {
		value_t ret;
		ret.value = (std::numeric_limits<T>::max)();
		return ret;
	}

	static constexpr value_t epsilon() noexcept {
		value_t ret;
		ret.value = T(1);
		return ret;
	}

	static constexpr value_t round_error() noexcept {
		return value_t(0.5);
	}

	static constexpr value_t infinity() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t quiet_NaN() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t signaling_NaN() noexcept {
		return value_t(T(0));
	}
	static constexpr value_t denorm_min() noexcept {
		return value_t(T(0));
	}
};
} // namespace std
