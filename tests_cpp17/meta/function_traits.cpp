#include <fea/meta/function_traits.hpp>
#include <fea/utility/unused.hpp>
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
	double d = 0.0;
	float f = 0.f;
	short s = 0;
	fea::unused(some_func(d, f, s));

	static_assert(std::is_same_v<fea::func_ret_t<decltype(&obj::func)>, void>,
			"function_traits.cpp : test failed");
	static_assert(std::is_same_v<fea::func_args_t<decltype(&obj::func)>,
						  std::tuple<int>>,
			"function_traits.cpp : test failed");

	static_assert(std::is_same_v<fea::func_ret_t<obj>, int>,
			"function_traits.cpp : test failed");
	static_assert(std::is_same_v<fea::func_args_t<obj>, std::tuple<double>>,
			"function_traits.cpp : test failed");

	static_assert(std::is_same_v<fea::func_ret_t<decltype(&some_func)>, double>,
			"function_traits.cpp : test failed");
	static_assert(std::is_same_v<fea::func_args_t<decltype(&some_func)>,
						  std::tuple<double, float&, const short&>>,
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
	static_assert(std::is_class_v<obj>, "");

	using mem_fun = fea::member_func_ptr_t<void, obj*, int>;
	static_assert(std::is_same_v<mem_fun, decltype(&obj::func)>,
			"function_traits.cpp : test failed");

	using mem_fun2 = fea::member_func_ptr_t<void, int>;
	static_assert(std::is_same_v<mem_fun2, void*>,
			"function_traits.cpp : test failed");

	using mem_fun3 = fea::member_func_ptr_t<void, obj, int>;
	static_assert(std::is_same_v<mem_fun3, void*>,
			"function_traits.cpp : test failed");

	using mem_fun4 = fea::member_func_ptr_t<void, obj*, int, double, float>;
	static_assert(std::is_same_v<mem_fun4, void (obj::*)(int, double, float)>,
			"function_traits.cpp : test failed");

	using mem_fun5 = fea::member_func_ptr_t<void, int*>;
	static_assert(std::is_same_v<mem_fun5, void*>,
			"function_traits.cpp : test failed");
}

} // namespace
