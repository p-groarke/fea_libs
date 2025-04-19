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
#include "fea/utils/platform.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace fea {
template <class T, T...>
struct max_nt;

template <class T, T...>
struct min_nt;


// Finds the maximum value of provided non-type values.
template <class T, T... Args>
inline constexpr auto max_nt_v = max_nt<T, Args...>::value;

// Finds the minimum value of provided non-type values.
template <class T, T... Args>
inline constexpr auto min_nt_v = min_nt<T, Args...>::value;

// Finds the maximum value of provided non-type values.
template <auto First, auto... Args>
inline constexpr auto max_v = max_nt_v<decltype(First), First, Args...>;

// Finds the minimum value of provided non-type values.
template <auto First, auto... Args>
inline constexpr auto min_v = min_nt_v<decltype(First), First, Args...>;
} // namespace fea


// Implementation
namespace fea {
template <class T, T First, T Second>
struct max_nt<T, First, Second> {
	static constexpr auto value = First > Second ? First : Second;
};

template <class T, T First, T Second, T... Args>
struct max_nt<T, First, Second, Args...> {
	static constexpr auto value = First > Second
										? max_nt<T, First, Args...>::value
										: max_nt<T, Second, Args...>::value;
};

template <class T, T First, T Second>
struct min_nt<T, First, Second> {
	static constexpr auto value = First < Second ? First : Second;
};

template <class T, T First, T Second, T... Args>
struct min_nt<T, First, Second, Args...> {
	static constexpr auto value = First < Second
										? min_nt<T, First, Args...>::value
										: min_nt<T, Second, Args...>::value;
};
} // namespace fea
