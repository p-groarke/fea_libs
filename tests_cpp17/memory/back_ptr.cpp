#include <fea/memory/back_ptr.hpp>
#include <gtest/gtest.h>

namespace {
struct test1 {
	int v = 42;
	fea::back_ptr<int> ptr;
};

TEST(back_ptr, basics) {
	test1 t1;
	EXPECT_EQ(t1.ptr.get(), nullptr);
	EXPECT_FALSE(t1.ptr);

	t1.ptr = &t1.v;
	EXPECT_NE(t1.ptr.get(), nullptr);
	EXPECT_TRUE(t1.ptr);
	EXPECT_EQ(*t1.ptr, 42);
	EXPECT_EQ(t1.ptr.operator->(), &t1.v);

	test1 t2 = t1;
	EXPECT_EQ(t2.ptr.get(), nullptr);
	EXPECT_NE(t1.ptr.get(), nullptr);
	EXPECT_FALSE(t2.ptr);
	EXPECT_TRUE(t1.ptr);
	EXPECT_EQ(*t1.ptr, 42);
	EXPECT_EQ(t1.ptr.operator->(), &t1.v);

	test1 t3 = std::move(t1);
	EXPECT_EQ(t3.ptr.get(), nullptr);
	EXPECT_EQ(t1.ptr.get(), nullptr);
	EXPECT_FALSE(t3.ptr);
	EXPECT_FALSE(t1.ptr);

	int v2 = 41;
	t1.ptr.reset(&v2);

	t2 = t1;
	EXPECT_EQ(t2.ptr.get(), nullptr);
	EXPECT_NE(t1.ptr.get(), nullptr);
	EXPECT_FALSE(t2.ptr);
	EXPECT_TRUE(t1.ptr);
	EXPECT_EQ(*t1.ptr, 41);
	EXPECT_EQ(t1.ptr.operator->(), &v2);

	t3 = std::move(t1);
	EXPECT_EQ(t3.ptr.get(), nullptr);
	EXPECT_EQ(t1.ptr.get(), nullptr);
	EXPECT_FALSE(t3.ptr);
	EXPECT_FALSE(t1.ptr);
}
} // namespace