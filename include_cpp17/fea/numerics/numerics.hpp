
/**
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
#include <cmath>
#include <limits>
#include <type_traits>

namespace fea {
namespace detail {
template <class>
struct next_bigger;

template <>
struct next_bigger<char> {
	using type = short;
};
template <>
struct next_bigger<signed char> {
	using type = signed short;
};
template <>
struct next_bigger<unsigned char> {
	using type = unsigned short;
};

template <>
struct next_bigger<short> {
	using type = int;
};
template <>
struct next_bigger<unsigned short> {
	using type = unsigned int;
};

#if FEA_32BIT
template <>
struct next_bigger<int> {
	using type = int;
};
template <>
struct next_bigger<unsigned int> {
	using type = unsigned int;
};

template <>
struct next_bigger<float> {
	using type = float;
};

#elif FEA_64BIT
template <>
struct next_bigger<int> {
	using type = long long;
};
template <>
struct next_bigger<unsigned int> {
	using type = unsigned long long;
};

template <>
struct next_bigger<long long> {
	using type = long long;
};
template <>
struct next_bigger<unsigned long long> {
	using type = unsigned long long;
};

template <>
struct next_bigger<float> {
	using type = double;
};

template <>
struct next_bigger<double> {
	using type = double;
};
template <>
struct next_bigger<long double> {
	using type = long double;
};
#else
static_assert(false, "need to update maps");
#endif

} // namespace detail

// Returns the next bigger fundamental type to hold T, or T if size == size_t.
template <class T>
using next_bigger_t = typename detail::next_bigger<T>::type;

// Absolute function that works with signed min (returns signed max).
// Casts chars and shorts to int internally, forwards to std::abs when
// appropriate.
template <class T>
constexpr T abs(T v) {
	if constexpr (std::is_unsigned_v<T>) {
		return v;
	} else if constexpr (std::is_floating_point_v<T>) {
		return std::abs(v);
	} else {
		constexpr T t_max = (std::numeric_limits<T>::max)();
		constexpr T t_min = std::numeric_limits<T>::lowest();
		if (v == t_min) {
			return t_max;
		}

		if constexpr (sizeof(T) >= 4) {
			return std::abs(v);
		} else {
			return T(std::abs(int(v)));
		}
	}
}
} // namespace fea
