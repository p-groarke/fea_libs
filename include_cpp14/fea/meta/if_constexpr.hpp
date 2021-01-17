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
#include "fea/meta/pack.hpp"

/*
C++14 if constexpr equivalent.
*/

namespace fea {
namespace detail {
template <class TrueFunc, class FalseFunc>
constexpr void if_constexpr(TrueFunc&& func, FalseFunc&&, std::true_type) {
	std::forward<TrueFunc>(func)();
}
template <class TrueFunc, class FalseFunc>
constexpr void if_constexpr(TrueFunc&&, FalseFunc&& func, std::false_type) {
	std::forward<FalseFunc>(func)();
}


//// TODO : True if constexpr
// template <bool Cond, class Func, class... Conditions>
// struct else_condition {
//	constexpr else_condition(Func&& func)
//			: _func(func) {
//	}
//
//	constexpr void operator()() {
//		unroll<Conditions...>();
//	}
//
//	static constexpr bool condition() {
//		return Cond;
//	}
//
//	static constexpr void exec() {
//		return _func();
//	}
//
// private:
//	template <class First, class... Rest>
//	static constexpr void unroll() {
//		if_constexpr([]() { First::exec(); }, []() { unroll<Rest...>(); },
//				First::condition());
//	}
//
//	Func _func;
//};
//
// template <bool Cond, class Func, class... Conditions>
// struct else_if_condition {
//	constexpr else_if_condition(Func&& func)
//			: _func(func) {
//	}
//
//	template <bool NewCond, class NewFunc>
//	static constexpr auto else_if(NewFunc&& new_func) {
//		return else_if_condition<NewCond, NewFunc, Conditions...,
//				else_if_condition<NewCond, NewFunc>>{ std::forward<NewFunc&&>(
//				new_func) };
//	}
//
//	template <bool NewCond, class NewFunc>
//	static constexpr auto else_(NewFunc&& new_func) {
//		return else_condition<NewCond, NewFunc, Conditions...,
//				else_condition<NewCond, NewFunc>>{ std::forward<NewFunc&&>(
//				new_func) };
//	}
//
//	constexpr void operator()() {
//		unroll<Conditions...>();
//	}
//
//	static constexpr bool condition() {
//		return Cond;
//	}
//
//	static constexpr void exec() {
//		return _func();
//	}
//
// private:
//	template <class First, class... Rest>
//	static constexpr void unroll() {
//		if_constexpr([]() { First::exec(); }, []() { unroll<Rest...>(); },
//				First::condition());
//	}
//
//	Func _func;
//};

} // namespace detail

// C++14 if_constexpr equivalent.
// There is no else if unfortunately.
template <bool Cond, class TrueFunc, class FalseFunc>
constexpr auto if_constexpr(TrueFunc&& true_func, FalseFunc&& false_func) {
	return detail::if_constexpr(std::forward<TrueFunc>(true_func),
			std::forward<FalseFunc>(false_func),
			std::integral_constant<bool, Cond>{});
}

} // namespace fea
