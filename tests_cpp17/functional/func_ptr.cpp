#include <fea/functional/func_ptr.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
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

TEST(func_ptr, basics) {
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
		static_assert(f2.invoke(&o, i) == -42, "func_ptr.cpp : Test failed.");
#endif

		// Const overloads should have been resolved appropriately.
		EXPECT_EQ(f1.invoke(&o, i), 42);
		EXPECT_EQ(f2.invoke(&o, i), -42);
		EXPECT_EQ(f3.invoke(&o, i), 0);
		EXPECT_EQ(f4.invoke(&o, i), 1);

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

		f1.invoke(&o, i);
		EXPECT_EQ(i, 2);
		i = 0;
		f2.invoke(&o, i);
		EXPECT_EQ(i, 3);
		i = 0;
		f3.invoke(&o, i);
		EXPECT_EQ(i, 4);
		i = 0;
		f4.invoke(&o, i);
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
} // namespace