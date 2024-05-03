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
#include "fea/meta/function_traits.hpp"
#include "fea/utils/platform.hpp"

namespace fea {
// Pass template lambdas while requiring the necessary signature for
// documentation.
template <class, class = void>
struct callback;

template <class Func, class Ret, class... Args>
struct callback<Func, Ret(Args...)> {
	using func_t = Func;

	constexpr callback(const Func& func)
			: _func(func) {
	}
	constexpr callback(Func&& func)
			: _func(std::move(func)) {
	}

	constexpr Ret operator()(Args... args) const {
		return _func(std::forward<Args>(args)...);
	}

private:
	Func _func;
};

namespace detail {
template <class, class>
struct callback_base;

template <class Func, class Ret, class... Args>
struct callback_base<Func, Ret(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class... Args>
struct callback_base<Func, Ret (*)(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class T, class... Args>
struct callback_base<Func, Ret (T::*)(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class T, class... Args>
struct callback_base<Func, Ret (T::*)(Args...) const>
		: callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class, class = void>
struct callback_selector;


template <class Func>
struct callback_selector<Func,
		std::enable_if_t<fea::is_detected_v<fea::has_operator_paren, Func>>>
		: callback_base<Func, decltype(&Func::operator())> {
	using base = callback_base<Func, decltype(&Func::operator())>;
	using base::base;
};

template <class Func>
struct callback_selector<Func,
		std::enable_if_t<!fea::is_detected_v<fea::has_operator_paren, Func>>>
		: callback_base<Func, Func> {
	using base = callback_base<Func, Func>;
	using base::base;
};

} // namespace detail

template <class Func, class>
struct callback : detail::callback_selector<Func> {
	using func_t = Func;
	using base = detail::callback_selector<Func>;

	constexpr callback(const Func& func)
			: base(func) {
	}
	constexpr callback(Func&& func)
			: base(std::move(func)) {
	}
};


template <class Func>
callback<Func> make_callback(const Func& func) {
	return callback<Func>(func);
}
template <class Func>
callback<Func> make_callback(Func&& func) {
	return callback<Func>(func);
}

//#if FEA_CPP17
// template <class Func>
// callback(const Func&) -> callback<Func>;
// template <class Func>
// callback(Func &&) -> callback<Func>;
//#endif


} // namespace fea
