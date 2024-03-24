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

namespace fea {
namespace detail {
template <class T>
using has_const_paren = decltype(std::declval<const T&>()());

template <class T, bool IsConst>
struct ro_expose_const;

template <class T>
struct ro_expose_const<T, true> : T {
	using overload_t = decltype(std::declval<T>()());

	operator overload_t() const noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

template <class T>
struct ro_expose_const<T, false> : T {
	using overload_t = decltype(std::declval<T>()());

	operator overload_t() noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

template <class T>
struct return_overload
		: ro_expose_const<T, fea::is_detected_v<has_const_paren, T>> {

	using base_t = ro_expose_const<T, fea::is_detected_v<has_const_paren, T>>;
	using overload_t = typename base_t::overload_t;
	using base_t::operator typename base_t::overload_t;
};
} // namespace detail

template <class... Ts>
struct return_overload : detail::return_overload<Ts>... {
	// Inherit the overload operators.
	using detail::return_overload<
			Ts>::operator typename detail::return_overload<Ts>::overload_t...;
};

template <class... Ts>
return_overload(Ts...) -> return_overload<Ts...>;

} // namespace fea
