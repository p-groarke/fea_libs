#include <algorithm>
#include <chrono>
#include <fea/benchmark/benchmark.hpp>
#include <fea/functional/function.hpp>
#include <gtest/gtest.h>

namespace {
struct test_obj {
	void func(int) {
		printf("test_obj::func(int)\n");
	}
	void func_const(int) const {
		printf("test_obj::func(int) const\n");
	}
};

TEST(function_cl, basics) {
	// std::function<void(test_obj*, int)> stdf = &test_obj::func_const;

	// fea::function_cl<void(test_obj*, int)> f = &test_obj::func_const;

	// test_obj o;
	// f(&o, 42);
}

TEST(function, basics) {
	fea::function<void(int)> f;
	static_assert(std::is_same<decltype(f), std::function<void(int)>>::value,
			"function.cpp : test failed");

	fea::function<void(int), true> f_ptr;
	static_assert(
			std::is_same<decltype(f_ptr), fea::function_cl<void(int)>>::value,
			"function.cpp : test failed");
}
} // namespace
