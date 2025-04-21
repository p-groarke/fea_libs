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
#include "fea/utility/platform.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace fea {
// Returns magnitude of vector.
// TODO : variadic
template <class T>
[[nodiscard]]
T magnitude(const T& x, const T& y);

// Normalizes vector.
// TODO : variadic
template <class T>
void normalize(T& x, T& y);

// For ints and unsigned ints, divides with proper rounding.
// Q : Where to put this?
template <class T>
[[nodiscard]]
constexpr T divide_round(const T& dividend, const T& divisor);

} // namespace fea


// Implementation
namespace fea {
template <class T>
T magnitude(const T& x, const T& y) {
	return std::sqrt(x * x + y * y);
}

template <class T>
void normalize(T& x, T& y) {
	if (x == T(0) && y == T(0)) {
		return;
	}

	T mag = magnitude(x, y);
	x /= mag;
	y /= mag;
}

template <class T>
constexpr T divide_round(const T& dividend, const T& divisor) {
	static_assert(std::is_integral_v<T>,
			"fea::divide_round : Useless for non integral types.");
	if (divisor == T(0)) {
		return T(0);
	}
	return (dividend + (divisor / T(2))) / divisor;
}
} // namespace fea
