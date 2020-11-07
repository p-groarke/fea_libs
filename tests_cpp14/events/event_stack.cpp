﻿#include <atomic>
#include <fea/events/event_stack.hpp>
#include <gtest/gtest.h>

namespace {
TEST(event_stack, basics) {
	enum class e : unsigned { one, two, three, count };
	fea::event_stack<e, int(), int(), int(float, double)> s{};

	EXPECT_TRUE(s.empty<e::one>());
	EXPECT_TRUE(s.empty<e::two>());
	EXPECT_TRUE(s.empty<e::three>());
	EXPECT_EQ(0, s.size<e::one>());
	EXPECT_EQ(0, s.size<e::two>());
	EXPECT_EQ(0, s.size<e::three>());
	EXPECT_EQ(0, s.capacity<e::one>());
	EXPECT_EQ(0, s.capacity<e::two>());
	EXPECT_EQ(0, s.capacity<e::three>());

	s.reserve<e::one>(4);
	s.reserve<e::two>(4);
	s.reserve<e::three>(4);
	EXPECT_EQ(4, s.capacity<e::one>());
	EXPECT_EQ(4, s.capacity<e::two>());
	EXPECT_EQ(4, s.capacity<e::three>());

	std::atomic<int> t_one{ 0 };
	std::atomic<int> t_two{ 0 };
	std::atomic<int> t_three{ 0 };

	auto front_1 = s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 0;
	});
	s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 1;
	});
	s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 2;
	});
	s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 3;
	});
	auto back_1 = s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 4;
	});
	EXPECT_EQ(5, s.size<e::one>());

	auto front_2 = s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 0;
	});
	s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 1;
	});
	s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 2;
	});
	s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 3;
	});
	auto back_2 = s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 4;
	});
	EXPECT_EQ(5, s.size<e::two>());

	auto front_3 = s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 0;
	});
	s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 1;
	});
	s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 2;
	});
	s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 3;
	});
	auto back_3 = s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 4;
	});
	EXPECT_EQ(5, s.size<e::three>());

	s.trigger<e::one>();
	EXPECT_EQ(5, t_one);
	s.trigger<e::two>();
	EXPECT_EQ(5, t_two);
	s.trigger<e::three>(0.1f, 0.1);
	EXPECT_EQ(5, t_three);

	t_one = 0;
	t_two = 0;
	t_three = 0;

	s.trigger_mt<e::one>();
	EXPECT_EQ(5, t_one);
	s.trigger_mt<e::two>();
	EXPECT_EQ(5, t_two);
	s.trigger_mt<e::three>(0.1f, 0.1);
	EXPECT_EQ(5, t_three);

	EXPECT_EQ(0, s.at<e::one>(front_1)());
	EXPECT_EQ(0, s.at<e::two>(front_2)());
	EXPECT_EQ(0, s.at<e::three>(front_3)(0.f, 0.1));
	EXPECT_EQ(0, s.at_unchecked<e::one>(front_1)());
	EXPECT_EQ(0, s.at_unchecked<e::two>(front_2)());
	EXPECT_EQ(0, s.at_unchecked<e::three>(front_3)(0.f, 0.1));

	EXPECT_EQ(4, s.at<e::one>(back_1)());
	EXPECT_EQ(4, s.at<e::two>(back_2)());
	EXPECT_EQ(4, s.at<e::three>(back_3)(0.f, 0.1));
	EXPECT_EQ(4, s.at_unchecked<e::one>(back_1)());
	EXPECT_EQ(4, s.at_unchecked<e::two>(back_2)());
	EXPECT_EQ(4, s.at_unchecked<e::three>(back_3)(0.f, 0.1));

	s.unsubscribe<e::one>(front_1);
	s.unsubscribe<e::two>(front_2);
	s.unsubscribe<e::three>(front_3);

	EXPECT_EQ(4, s.size<e::one>());
	EXPECT_EQ(4, s.size<e::two>());
	EXPECT_EQ(4, s.size<e::three>());
	EXPECT_FALSE(s.empty<e::one>());
	EXPECT_FALSE(s.empty<e::two>());
	EXPECT_FALSE(s.empty<e::three>());

	s.clear<e::one>();
	s.clear<e::two>();
	s.clear<e::three>();
	EXPECT_EQ(0, s.size<e::one>());
	EXPECT_EQ(0, s.size<e::two>());
	EXPECT_EQ(0, s.size<e::three>());
	EXPECT_TRUE(s.empty<e::one>());
	EXPECT_TRUE(s.empty<e::two>());
	EXPECT_TRUE(s.empty<e::three>());
}

} // namespace
