/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
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
#include "fea/meta/traits.hpp"

#include <tuple>
#include <type_traits>


namespace fea {
// Detect if provided type has operator(). Use inside fea::is_detected
template <class T>
using has_operator_paren = decltype(&T::operator());

// fea::member_func_ptr is a trait which constructs a member function pointer,
// given Ret and Args...
//
// It uses the first argument of Args as the class type.
// If no Args are provided, aliases void*.
template <class, class...>
struct member_func_ptr;

// Helper alias.
template <class Ret, class... Args>
using member_func_ptr_t = typename member_func_ptr<Ret, Args...>::type;

// fea::func_ret creates a 'type' alias to a function pointer's return type.
// You may provide a lambda or function object directly to it.
template <class>
struct func_ret;

// Helper alias.
template <class Func>
using func_ret_t = typename func_ret<Func>::type;

// fea::func_args creates a fea::pack 'type' alias to a function pointer's
// arguments. You may provide a lambda or function object directly to it.
template <class>
struct func_args;

// Helper alias.
template <class Func>
using func_args_t = typename func_args<Func>::type;

// The following based off of Functional C++ blog post :
// https://functionalcpp.wordpress.com/2013/08/05/function-traits/
// Extracts pretty much verything from a function pointer.
//
// Declares member aliases : return_t, args_tuple, args_decay_typle, arity,
// argument<N>.
template <class>
struct function_traits;
} // namespace fea


// Implementation
namespace fea {
template <class, class, bool, class...>
struct member_func_ptr_impl {
	using type = void*;
};

template <class Ret, class T, class... Rest>
struct member_func_ptr_impl<Ret, T, true, Rest...> {
	using type = Ret (T::*)(Rest...);
};

template <class, class...>
struct member_func_ptr {
	using type = void*;
};

template <class Ret, class T, class... Rest>
struct member_func_ptr<Ret, T*, Rest...>
		: member_func_ptr_impl<Ret, T, std::is_class_v<T>, Rest...> {};

template <class Ret, class... Args>
struct func_ret<Ret(Args...)> {
	using type = Ret;
};
template <class Ret, class... Args>
struct func_ret<Ret (*)(Args...)> {
	using type = Ret;
};
template <class Ret, class T, class... Args>
struct func_ret<Ret (T::*)(Args...)> {
	using type = Ret;
};
template <class Ret, class T, class... Args>
struct func_ret<Ret (T::*)(Args...) const> {
	using type = Ret;
};
template <class Func>
struct func_ret {
	using type = typename func_ret<decltype(&Func::operator())>::type;
};

template <class Ret, class... Args>
struct func_args<Ret(Args...)> {
	using type = std::tuple<Args...>;
};
template <class Ret, class... Args>
struct func_args<Ret (*)(Args...)> {
	using type = std::tuple<Args...>;
};
template <class Ret, class T, class... Args>
struct func_args<Ret (T::*)(Args...)> {
	using type = std::tuple<Args...>;
};
template <class Ret, class T, class... Args>
struct func_args<Ret (T::*)(Args...) const> {
	using type = std::tuple<Args...>;
};
template <class Func>
struct func_args {
	using type = typename func_args<decltype(&Func::operator())>::type;
};


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
		using type = std::tuple_element_t<N, std::tuple<Args...>>;
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
