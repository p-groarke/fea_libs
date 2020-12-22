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
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"

#include <cassert>
#include <functional>

/*
raw_function is a holder for function pointers, which does the "right thing"
when storing member pointers. It does *not* support captures or functors.

function is a trait which helps you choose between a std::function or raw
function pointer.

By default, it returns a std::function type. Pass true as a second template
argument to use raw function pointers.
*/

namespace fea {
template <class>
struct raw_function;

template <class FuncRet, class... FuncArgs>
struct raw_function<FuncRet(FuncArgs...)> {
	using function_t = FuncRet (*)(FuncArgs...);

	// Can be default constructed.
	raw_function() = default;

	// Accept raw function pointers and captureless lambdas.
	raw_function(function_t func)
			: _func(func) {
	}

	// Accept member pointers.
	template <class T, class... OtherArgs>
	raw_function(FuncRet (T::*func)(OtherArgs...))
			: _func(&erased_func<T>)
			, _ptr(std::addressof(func)) {

		static_assert(std::is_same<fea::first<FuncArgs...>, T*>::value,
				"fea::raw_function : first argument of member function pointer "
				"must be pointer to the class");

		static_assert(std::is_same<std::tuple<OtherArgs...>,
							  fea::idx_splice_after_t<0, FuncArgs...>>::value,
				"fea::raw_function : signature mismatch between provided "
				"function and expected function");
	}

	FuncRet operator()(FuncArgs... func_args) const {
		assert(_func != nullptr);
		return _func(std::forward<FuncArgs>(func_args)...);
	}

	explicit operator bool() const noexcept {
		return _func != nullptr;
	}

private:
	template <class T, class... Rest>
	static constexpr FuncRet erased_impl(T* obj, Rest... rest_args) {
		using func_t = FuncRet (T::*)(Rest...);
		return ((*obj).*_ptr)(std::forward<Rest>(rest_args)...);

		// return ((*static_cast<_Ty1&&>(_Arg1)).*_Obj)(
		//		static_cast<_Types2&&>(_Args2)...);
	}

	template <class T>
	static constexpr FuncRet erased_func(FuncArgs... func_args) {
		return erased_impl(std::forward<FuncArgs>(func_args)...);
	}

	function_t _func = nullptr;
	void* _ptr = nullptr;
};

namespace detail {
template <class, bool = false>
struct function;

template <class Func>
struct function<Func, false> {
	using type = std::function<Func>;
};

template <class Func>
struct function<Func, true> {
	using type = raw_function<Func>;
};
} // namespace detail

template <class Func, bool UsePtr = false>
using function = typename detail::function<Func, UsePtr>::type;


} // namespace fea
