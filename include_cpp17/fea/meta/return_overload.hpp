﻿/**
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

#if FEA_GCC_GE(13)
#include "fea/meta/traits.hpp"

#include <type_traits>

/*
fea::return_overload is a simple return type which allows you to customize
behavior according to the return type.

It is inspired by the overloaded{} example on cppreference std::visit page.
https://en.cppreference.com/w/cpp/utility/variant/visit

With return_overload, you can overload by return type.
See unit tests for examples.
*/

#if FEA_MACOS
// Clang complains about braces around lambdas, which is silly.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

namespace fea {
namespace detail {
template <class T>
using has_const_operator = decltype(std::declval<const T&>()());

// Use OverloadT defaulted template param to fix VS2017 bug.
template <class T, bool IsConst,
		class OverloadT = decltype(std::declval<T>()())>
struct ro_make_operator;

template <class T, class OT>
struct ro_make_operator<T, true, OT> : T {
	using overload_t = OT;
	inline operator OT() const noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

template <class T, class OT>
struct ro_make_operator<T, false, OT> : T {
	using overload_t = OT;
	inline operator OT() noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

template <class T>
struct ro_base
		: ro_make_operator<T, fea::is_detected_v<has_const_operator, T>> {
	using base_t
			= ro_make_operator<T, fea::is_detected_v<has_const_operator, T>>;
	using overload_t = typename base_t::overload_t;
	using base_t::operator typename base_t::overload_t;
};
} // namespace detail

template <class... Ts>
struct return_overload : detail::ro_base<Ts>... {
	// Inherit the overload operators.
	using detail::ro_base<Ts>::operator typename detail::ro_base<
			Ts>::overload_t...;
};

// Not necessary in c++20.
template <class... Ts>
return_overload(Ts...) -> return_overload<Ts...>;
} // namespace fea

#if FEA_MACOS
#pragma clang diagnostic pop
#endif
#endif
