#include <fea/functional/func_ptr.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "func_ptr.cpp : Failed test."

constexpr int cfunc() {
	return -1;
}
constexpr int cfunc(int in) {
	return in;
}
int cfunc(const int& in) {
	return in;
}
int cfunc(int& out) {
	out = 2;
	return 2;
}

TEST(func_ptr, c_basics) {
	{
		fea::func_ptr<int()> f;
		EXPECT_FALSE(f);
		f = &cfunc;
		EXPECT_TRUE(f);

		EXPECT_EQ(f(), -1);
		EXPECT_EQ(f.to_function()(), -1);
	}

	{
		fea::func_ptr<int(int)> f;
		EXPECT_FALSE(f);
		f = &cfunc;
		EXPECT_TRUE(f);

		int v = f(-42);
		EXPECT_EQ(v, -42);

		std::function<int(int)> stdf = f.to_function();
		v = stdf(42);
		EXPECT_EQ(v, 42);
	}

	{
		fea::func_ptr<int()> f([]() { return 101; });
		EXPECT_TRUE(f);
		EXPECT_EQ(f(), 101);
		EXPECT_EQ(f.to_function()(), 101);
	}

	{
		constexpr fea::func_ptr<int()> f{ []() -> int { return 101; } };
		EXPECT_TRUE(f);
		EXPECT_EQ(f(), 101);
		EXPECT_EQ(f.to_function()(), 101);
#if FEA_CPP20
		static_assert(f() == 101, FAIL_MSG);
#endif
	}

	{
		constexpr fea::func_ptr<int(int, int)> f{ [](int r, int) {
			return r;
		} };
		EXPECT_TRUE(f);
		EXPECT_EQ(f(42, 101), 42);
		EXPECT_EQ(f.to_function()(42, 101), 42);
#if FEA_CPP20
		static_assert(f(42, 101) == 42, FAIL_MSG);
#endif
	}

	{
		// These should compile.
		constexpr fea::func_ptr<int(int)> f1 = &cfunc;
		constexpr fea::func_ptr<int(const int&)> f2 = &cfunc;
		constexpr fea::func_ptr<int(int&)> f3 = &cfunc;
		constexpr fea::func_ptr<int()> f4 = &cfunc;

		// Should be callable at compile time.
#if FEA_CPP20
		static_assert(f1(101) == 101, FAIL_MSG);
		static_assert(f4() == -1, FAIL_MSG);
#endif

		int i = 42;
		EXPECT_EQ(f1(i), 42);
		i = -42;
		EXPECT_EQ(f2(i), -42);
		EXPECT_EQ(f3(i), 2);
		EXPECT_EQ(i, 2);
		EXPECT_EQ(f4(), -1);

		i = 42;
		EXPECT_EQ(f1.to_function()(i), 42);
		i = -42;
		EXPECT_EQ(f2.to_function()(i), -42);
		EXPECT_EQ(f3.to_function()(i), 2);
		EXPECT_EQ(i, 2);
		EXPECT_EQ(f4.to_function()(), -1);
	}
}


struct obj {
	int func(int) {
		return 42;
	}
	int func(int) const {
		return 0;
	}

	void func(int& out) {
		out = 2;
	}
	void func(int& out) const {
		out = 3;
	}
};
constexpr int func(obj*, int) {
	return -42;
}
int func(const obj*, int) {
	return 1;
}
void func(obj*, int& out) {
	out = 4;
}
void func(const obj*, int& out) {
	out = 5;
}

TEST(func_ptr, member_basics) {
	{
		fea::func_ptr<int(obj*, int)> f;
		EXPECT_FALSE(f);
		f = &obj::func;
		EXPECT_TRUE(f);
	}

	{
		// These should compile.
		constexpr fea::func_ptr<int(obj*, int)> f1 = &obj::func;
		constexpr fea::func_ptr<int(obj*, int)> f2 = &func;
		constexpr fea::func_ptr<int(const obj*, int)> f3 = &obj::func;
		constexpr fea::func_ptr<int(const obj*, int)> f4 = &func;

		obj o{};
		constexpr int i{};

#if FEA_CPP20
		// Should be callable at compile time.
		static_assert(f2(&o, i) == -42, FAIL_MSG);
#endif

		// Const overloads should have been resolved appropriately.
		EXPECT_EQ(f1(&o, i), 42);
		EXPECT_EQ(f2(&o, i), -42);
		EXPECT_EQ(f3(&o, i), 0);
		EXPECT_EQ(f4(&o, i), 1);

		EXPECT_EQ(f1.to_function()(&o, i), 42);
		EXPECT_EQ(f2.to_function()(&o, i), -42);
		EXPECT_EQ(f3.to_function()(&o, i), 0);
		EXPECT_EQ(f4.to_function()(&o, i), 1);
	}

	{
		constexpr fea::func_ptr<void(obj*, int&)> f1 = &obj::func;
		constexpr fea::func_ptr<void(const obj*, int&)> f2 = &obj::func;
		constexpr fea::func_ptr<void(obj*, int&)> f3 = &func;
		constexpr fea::func_ptr<void(const obj*, int&)> f4 = &func;

		obj o{};
		int i = 0;

		f1(&o, i);
		EXPECT_EQ(i, 2);
		i = 0;
		f2(&o, i);
		EXPECT_EQ(i, 3);
		i = 0;
		f3(&o, i);
		EXPECT_EQ(i, 4);
		i = 0;
		f4(&o, i);
		EXPECT_EQ(i, 5);

		i = 0;
		f1.to_function()(&o, i);
		EXPECT_EQ(i, 2);
		i = 0;
		f2.to_function()(&o, i);
		EXPECT_EQ(i, 3);
		i = 0;
		f3.to_function()(&o, i);
		EXPECT_EQ(i, 4);
		i = 0;
		f4.to_function()(&o, i);
		EXPECT_EQ(i, 5);
	}
}

// FROM fea::function_cl
// TODO : Make these work

// enum class ret {
//	mem_func,
//	const_mem_func,
//	static_mem_func,
//	free_func1,
//	free_func2,
//	lambda1,
//	lambda2,
//	lambda3,
//};
//
// struct test_obj {
//	ret func() {
//		return ret::mem_func;
//	}
//	ret func_const() const {
//		return ret::const_mem_func;
//	}
//
//	static ret static_func() {
//		return ret::static_mem_func;
//	}
//};
//
// ret free_func(test_obj*) {
//	return ret::free_func1;
//}
//
// ret free_func2() {
//	return ret::free_func2;
//}
//
// TEST(function_cl, basics) {
//	{
//		test_obj o;
//		fea::func_ptr<ret(test_obj*)> f;
//		EXPECT_FALSE(f);
//
//		f = &test_obj::func;
//		EXPECT_TRUE(f);
//		EXPECT_EQ(f(&o), ret::mem_func);
//
//		f = &test_obj::func_const;
//		EXPECT_EQ(f(&o), ret::const_mem_func);
//
//		f = &free_func;
//		EXPECT_EQ(f(&o), ret::free_func1);
//
//		f = [](test_obj*) -> ret { return ret::lambda1; };
//		EXPECT_EQ(f(&o), ret::lambda1);
//
//		auto l = [](test_obj*) -> ret { return ret::lambda2; };
//		f = l;
//		EXPECT_EQ(f(&o), ret::lambda2);
//
//		f = {};
//		EXPECT_FALSE(f);
//	}
//
//	{
//		fea::func_ptr<ret()> f;
//		EXPECT_FALSE(f);
//
//		f = &test_obj::static_func;
//		EXPECT_TRUE(f);
//		EXPECT_EQ(f(), ret::static_mem_func);
//
//		f = &free_func2;
//		EXPECT_EQ(f(), ret::free_func2);
//
//		f = []() { return ret::lambda3; };
//		EXPECT_EQ(f(), ret::lambda3);
//
//		f = {};
//		EXPECT_FALSE(f);
//	}
//}
} // namespace