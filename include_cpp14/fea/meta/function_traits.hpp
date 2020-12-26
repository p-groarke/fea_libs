/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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
#include <tuple>
#include <type_traits>

// Based off of Functional C++ blog post :
// https://functionalcpp.wordpress.com/2013/08/05/function-traits/

namespace fea {
// Required for the "functor" overload, as the object reference would get stored
// in args.
template <class...>
struct drop_first;

// Drop the first type from a parameter pack.
template <class T, class... Args>
struct drop_first<std::tuple<T, Args...>> {
	using type = std::tuple<Args...>;
};


template <class F>
struct function_traits;

template <class F>
struct function_traits<F&> : public function_traits<F> {};

template <class F>
struct function_traits<F&&> : public function_traits<F> {};

template <class R, class... Args>
struct function_traits<R(Args...)> {
	using return_t = R;
	using args_tuple = std::tuple<Args...>; // Store the args for later use.
	using args_decay_tuple = std::tuple<std::decay_t<Args>...>;

	static constexpr std::size_t arity = sizeof...(Args);

	template <std::size_t N>
	struct argument {
		static_assert(N < arity, "error: invalid parameter index.");
		using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
	};
};

// function pointer
template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

// member function pointer
template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)>
		: public function_traits<R(C&, Args...)> {};

// const member function pointer
template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const>
		: public function_traits<R(C&, Args...)> {};

// member object pointer
template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)> {};

// functor
template <class F>
struct function_traits {
private:
	using call_type = function_traits<decltype(&F::operator())>;

public:
	using return_t = typename call_type::return_t;
	using args = typename drop_first<typename call_type::args>::type;
	using args_decay =
			typename drop_first<typename call_type::args_decay>::type;

	static constexpr std::size_t arity = call_type::arity - 1;

	template <std::size_t N>
	struct argument {
		static_assert(N < arity, "error: invalid parameter index.");
		using type = typename call_type::template argument<N + 1>::type;
	};
};
} // namespace fea
