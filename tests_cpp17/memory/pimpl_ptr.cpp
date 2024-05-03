#include <fea/memory/pimpl_ptr.hpp>
#include <gtest/gtest.h>
#include <type_traits>
#include <vector>

namespace {
struct my_class_impl;
struct my_class : fea::pimpl_ptr<my_class_impl> {
	my_class() = default;
	my_class(const my_class&) = default;
	my_class(my_class&&) = default;
	my_class& operator=(const my_class&) = default;
	my_class& operator=(my_class&&) = default;

	~my_class();

	// make it public for tests
	using fea::pimpl_ptr<my_class_impl>::_impl;
};

struct my_class_impl {
	int data = -42;
	std::vector<int> data2{ -42 };
};
my_class::~my_class() = default;


TEST(pimpl_ptr, asserts) {
	using pimpl_t = fea::pimpl_ptr<my_class_impl>;
	static_assert(std::is_default_constructible<pimpl_t>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_copy_constructible<pimpl_t>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_move_constructible<pimpl_t>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_copy_assignable<pimpl_t>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_move_assignable<pimpl_t>::value,
			"pimpl_ptr.cpp : test failed");

	static_assert(std::is_default_constructible<my_class>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_copy_constructible<my_class>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_move_constructible<my_class>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_copy_assignable<my_class>::value,
			"pimpl_ptr.cpp : test failed");
	static_assert(std::is_move_assignable<my_class>::value,
			"pimpl_ptr.cpp : test failed");
}

TEST(pimpl_ptr, basics) {
	my_class m;
	EXPECT_EQ(m._impl->data, -42);
	EXPECT_EQ(m._impl->data2.front(), -42);
	m._impl->data2.push_back(101);

	m._impl->data = 42;
	EXPECT_EQ(m._impl->data, 42);

	{
		my_class m2(m);
		EXPECT_EQ(m2._impl->data, 42);

		my_class m3 = m2;
		EXPECT_EQ(m2._impl->data, 42);
	}

	{
		my_class mcpy = m;
		my_class m2(std::move(mcpy));
		EXPECT_EQ(m2._impl->data, 42);

		mcpy = m;
		my_class m3 = std::move(mcpy);
		EXPECT_EQ(m3._impl->data, 42);
	}
}
} // namespace
