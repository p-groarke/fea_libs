#include <algorithm>
#include <chrono>
#include <fea/benchmark/benchmark.hpp>
#include <fea/functional/function.hpp>
#include <gtest/gtest.h>

namespace {
enum class ret {
	mem_func,
	const_mem_func,
	static_mem_func,
	free_func1,
	free_func2,
	lambda1,
	lambda2,
	lambda3,
};

struct test_obj {
	ret func() {
		return ret::mem_func;
	}
	ret func_const() const {
		return ret::const_mem_func;
	}

	static ret static_func() {
		return ret::static_mem_func;
	}
};

ret free_func(test_obj*) {
	return ret::free_func1;
}

ret free_func2() {
	return ret::free_func2;
}

TEST(function_cl, basics) {
	{
		test_obj o;
		fea::function_cl<ret(test_obj*)> f;
		EXPECT_FALSE(f);

		f = &test_obj::func;
		EXPECT_TRUE(f);
		EXPECT_EQ(f(&o), ret::mem_func);

		f = &test_obj::func_const;
		EXPECT_EQ(f(&o), ret::const_mem_func);

		f = &free_func;
		EXPECT_EQ(f(&o), ret::free_func1);

		f = [](test_obj*) -> ret { return ret::lambda1; };
		EXPECT_EQ(f(&o), ret::lambda1);

		auto l = [](test_obj*) -> ret { return ret::lambda2; };
		f = l;
		EXPECT_EQ(f(&o), ret::lambda2);

		f = {};
		EXPECT_FALSE(f);
	}

	{
		fea::function_cl<ret()> f;
		EXPECT_FALSE(f);

		f = &test_obj::static_func;
		EXPECT_TRUE(f);
		EXPECT_EQ(f(), ret::static_mem_func);

		f = &free_func2;
		EXPECT_EQ(f(), ret::free_func2);

		f = []() { return ret::lambda3; };
		EXPECT_EQ(f(), ret::lambda3);

		f = {};
		EXPECT_FALSE(f);
	}
}

TEST(function, basics) {
	fea::function<void(int)> f;
	static_assert(std::is_same_v<decltype(f), std::function<void(int)>>,
			"function.cpp : test failed");

	fea::function<void(int), true> f_ptr;
	static_assert(std::is_same_v<decltype(f_ptr), fea::function_cl<void(int)>>,
			"function.cpp : test failed");
}
} // namespace
