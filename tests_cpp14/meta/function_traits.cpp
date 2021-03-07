#include <fea/meta/function_traits.hpp>
#include <gtest/gtest.h>

namespace {
struct obj {
	void func(int) {
	}

	int operator()(double) const {
		return {};
	}
};

double some_func(double, float&, const short&) {
	return {};
}

TEST(function_traits, func_ret_and_func_args) {
	static_assert(
			std::is_same<fea::func_ret_t<decltype(&obj::func)>, void>::value,
			"function_traits.cpp : test failed");
	static_assert(std::is_same<fea::func_args_t<decltype(&obj::func)>,
						  std::tuple<int>>::value,
			"function_traits.cpp : test failed");

	static_assert(std::is_same<fea::func_ret_t<obj>, int>::value,
			"function_traits.cpp : test failed");
	static_assert(
			std::is_same<fea::func_args_t<obj>, std::tuple<double>>::value,
			"function_traits.cpp : test failed");

	static_assert(
			std::is_same<fea::func_ret_t<decltype(&some_func)>, double>::value,
			"function_traits.cpp : test failed");
	static_assert(std::is_same<fea::func_args_t<decltype(&some_func)>,
						  std::tuple<double, float&, const short&>>::value,
			"function_traits.cpp : test failed");

	static_assert(
			!fea::is_detected_v<fea::has_operator_paren, decltype(&obj::func)>,
			"function_traits.cpp : test failed");

	static_assert(fea::is_detected_v<fea::has_operator_paren, obj>,
			"function_traits.cpp : test failed");

	static_assert(
			!fea::is_detected_v<fea::has_operator_paren, decltype(&some_func)>,
			"function_traits.cpp : test failed");
}

TEST(function_traits, member_func_ptr) {
	static_assert(std::is_class<obj>::value, "");

	using mem_fun = fea::member_func_ptr_t<void, obj*, int>;
	static_assert(std::is_same<mem_fun, decltype(&obj::func)>::value,
			"function_traits.cpp : test failed");

	using mem_fun2 = fea::member_func_ptr_t<void, int>;
	static_assert(std::is_same<mem_fun2, void*>::value,
			"function_traits.cpp : test failed");

	using mem_fun3 = fea::member_func_ptr_t<void, obj, int>;
	static_assert(std::is_same<mem_fun3, void*>::value,
			"function_traits.cpp : test failed");

	using mem_fun4 = fea::member_func_ptr_t<void, obj*, int, double, float>;
	static_assert(
			std::is_same<mem_fun4, void (obj::*)(int, double, float)>::value,
			"function_traits.cpp : test failed");

	using mem_fun5 = fea::member_func_ptr_t<void, int*>;
	static_assert(std::is_same<mem_fun5, void*>::value,
			"function_traits.cpp : test failed");
}

} // namespace
