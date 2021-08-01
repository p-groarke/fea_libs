/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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
#include "fea/utils/unused.hpp"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace fea {
// Fold expressions for c++ < 17.
// Calls your function with each of the provided variadic argument.
template <class Func, class... Args>
constexpr void fold(Func&& func, Args&&... args) {
#if FEA_CPP17
	(func(args), ...);
#else
	char dummy[] = { (void(func(args)), '0')... };
	unused(dummy);
#endif
}

namespace detail {
template <class Func, size_t... I>
constexpr void static_for(Func& func, std::index_sequence<I...>) {
#if FEA_CPP17
	(func(std::integral_constant<size_t, I>{}), ...);
#else
	char dummy[]
			= { (void(func(std::integral_constant<size_t, I>{})), '0')... };
	unused(dummy);
#endif
}
} // namespace detail

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
// Accept it with auto, access the index with '::value'.
template <size_t N, class Func>
constexpr void static_for(Func&& func) {
	detail::static_for(func, std::make_index_sequence<N>{});
}


// "std::apply index_sequence"
namespace detail {
template <class Func, size_t... Idx>
constexpr auto apply_indexes(Func&& f, std::index_sequence<Idx...>) {
	return std::forward<Func>(f)(std::integral_constant<size_t, Idx>{}...);
}
} // namespace detail

template <size_t N, class Func>
constexpr auto apply_indexes(Func&& f) {
	return detail::apply_indexes(
			std::forward<Func>(f), std::make_index_sequence<N>{});
}

} // namespace fea
