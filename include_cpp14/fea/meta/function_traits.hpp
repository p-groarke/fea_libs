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
