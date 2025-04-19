/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
#include <cstdint>
#include <fea/utility/platform.hpp>
#include <limits>
#include <type_traits>


namespace fea {
// Returns 1 if T max > U max
// Returns 0 if T max == U max
// Returns -1 if T max < U max
template <class T, class U>
constexpr int compare_max() {
	static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>,
			"has_bigger_max : Only supports number types.");

	using floatmax_t = long double;

	constexpr T t_max = (std::numeric_limits<T>::max)();
	constexpr U u_max = (std::numeric_limits<U>::max)();

	if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>) {
		// All floating points hold higher values than integers.
		if constexpr (floatmax_t(t_max) > floatmax_t(u_max)) {
			return 1;
		} else if constexpr (floatmax_t(t_max) < floatmax_t(u_max)) {
			return -1;
		} else {
			// ==
			return 0;
		}
	} else {
		// Use biggest unsigned to compare maxes.
		if constexpr (uintmax_t(t_max) > uintmax_t(u_max)) {
			return 1;
		} else if constexpr (uintmax_t(t_max) < uintmax_t(u_max)) {
			return -1;
		} else {
			return 0;
		}
	}
}

// Returns 1 if T lowest < U lowest
// Returns 0 if T lowest == U lowest
// Returns -1 if T lowest > U lowest
template <class T, class U>
constexpr int compare_lowest() {
	static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>,
			"has_bigger_max : Only supports number types.");

	using floatmax_t = long double;

	constexpr T t_low = (std::numeric_limits<T>::lowest)();
	constexpr U u_low = (std::numeric_limits<U>::lowest)();

	if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<U>) {
		// All floating points hold higher/lower values than integers.
		if constexpr (floatmax_t(t_low) < floatmax_t(u_low)) {
			return 1;
		} else if constexpr (floatmax_t(t_low) > floatmax_t(u_low)) {
			return -1;
		} else {
			return 0;
		}
	} else {
		// Use biggest signed to compare lows.
		if constexpr (intmax_t(t_low) < intmax_t(u_low)) {
			return 1;
		} else if constexpr (intmax_t(t_low) > intmax_t(u_low)) {
			return -1;
		} else {
			return 0;
		}
	}
}

template <class T, class U>
constexpr bool not_floating() {
	return !std::is_floating_point_v<T> && !std::is_floating_point_v<U>;
}

#if FEA_WINDOWS
// As far as I can tell, this is a problem with overzealous MSVC.
// It doesn't take into consideration the runtime edge cases that are
// eliminated. Gcc and clang are happy.
#pragma warning(push)
#pragma warning(disable : 4756)
#endif

// Saturating cast.
// Casts the input type to the output type,
// but saturates at the limits.
// For ex :
// - Casting a negative number to unsigned results in 0.
// - Casting a higher value than output returns output max.
template <class Output, class Input>
[[nodiscard]] constexpr Output clamp_cast(Input input) {
	static_assert(std::is_arithmetic_v<Input> && std::is_arithmetic_v<Output>,
			"clamp_cast : Only supports number types.");

	// Valid if the user doesn't know in advance which type she will cast.
	if constexpr (std::is_same_v<Input, Output>) {
		return input;
	}

	constexpr int max_info = compare_max<Input, Output>();
	constexpr int low_info = compare_lowest<Input, Output>();

	// Input max is bigger than output (ex. uint8_t in, int8_t out).
	if constexpr (max_info == 1) {
		// Clamp in input space.
		constexpr Output out_max = (std::numeric_limits<Output>::max)();
		if (input > Input(out_max)) {
			return out_max;
		}
	}

	// Input low is lower than output (ex. int8_t in, uint8_t out).
	if constexpr (low_info == 1) {
		// Clamp in input space.
		constexpr Output out_low = (std::numeric_limits<Output>::lowest)();
		if (input < Input(out_low)) {
			return out_low;
		}
	}

	return Output(input);
}

#if FEA_WINDOWS
#pragma warning(pop)
#endif
} // namespace fea
