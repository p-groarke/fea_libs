﻿#include <atomic>
#include <fea/events/event_stack.hpp>
#include <gtest/gtest.h>

namespace {
TEST(event_stack, basics) {
	enum class e { one, two, three, count };
	fea::event_stack<e, int(), int(), int(float, double)> s{};

	EXPECT_TRUE(s.empty<e::one>());
	EXPECT_TRUE(s.empty<e::two>());
	EXPECT_TRUE(s.empty<e::three>());
	EXPECT_TRUE(s.empty());
	EXPECT_EQ(0u, s.size<e::one>());
	EXPECT_EQ(0u, s.size<e::two>());
	EXPECT_EQ(0u, s.size<e::three>());
	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.capacity<e::one>());
	EXPECT_EQ(0u, s.capacity<e::two>());
	EXPECT_EQ(0u, s.capacity<e::three>());

	s.reserve<e::one>(4u);
	s.reserve<e::two>(4u);
	s.reserve<e::three>(4u);
	EXPECT_EQ(4u, s.capacity<e::one>());
	EXPECT_EQ(4u, s.capacity<e::two>());
	EXPECT_EQ(4u, s.capacity<e::three>());

	s.reserve(8);
	EXPECT_EQ(8u, s.capacity<e::one>());
	EXPECT_EQ(8u, s.capacity<e::two>());
	EXPECT_EQ(8u, s.capacity<e::three>());

	std::atomic<int> t_one{ 0 };
	std::atomic<int> t_two{ 0 };
	std::atomic<int> t_three{ 0 };

	fea::event_id<e, e::one> invalid_id;
	EXPECT_FALSE(s.contains(invalid_id));

	auto front_1 = s.subscribe<e::one>([&t_one]() {
		++t_one;
		return 0;
	});
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(front_1));

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
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(back_1));
	EXPECT_EQ(5u, s.size<e::one>());
	EXPECT_EQ(5u, s.size());

	auto front_2 = s.subscribe<e::two>([&t_two]() {
		++t_two;
		return 0;
	});
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(front_2));

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
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(back_2));
	EXPECT_EQ(5u, s.size<e::two>());
	EXPECT_EQ(10u, s.size());

	auto front_3 = s.subscribe<e::three>([&t_three](float, double) {
		++t_three;
		return 0;
	});
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(front_3));

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
	EXPECT_FALSE(s.contains(invalid_id));
	EXPECT_TRUE(s.contains(back_3));
	EXPECT_EQ(5u, s.size<e::three>());
	EXPECT_EQ(15u, s.size());

	s.trigger<e::one>();
	EXPECT_EQ(5, t_one);
	s.trigger<e::two>();
	EXPECT_EQ(5, t_two);
	s.trigger<e::three>(0.1f, 0.1);
	EXPECT_EQ(5, t_three);

	t_one = 0;
	t_two = 0;
	t_three = 0;

#if FEA_WITH_TBB
	s.trigger_mt<e::one>();
	EXPECT_EQ(5, t_one);
	s.trigger_mt<e::two>();
	EXPECT_EQ(5, t_two);
	s.trigger_mt<e::three>(0.1f, 0.1);
	EXPECT_EQ(5, t_three);
#endif

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

	EXPECT_EQ(4u, s.size<e::one>());
	EXPECT_EQ(4u, s.size<e::two>());
	EXPECT_EQ(4u, s.size<e::three>());
	EXPECT_EQ(12u, s.size());
	EXPECT_FALSE(s.empty<e::one>());
	EXPECT_FALSE(s.empty<e::two>());
	EXPECT_FALSE(s.empty<e::three>());
	EXPECT_FALSE(s.empty());

	s.clear<e::one>();
	EXPECT_FALSE(s.empty());
	s.clear<e::two>();
	EXPECT_FALSE(s.empty());
	s.clear<e::three>();
	EXPECT_TRUE(s.empty());

	EXPECT_EQ(0u, s.size<e::one>());
	EXPECT_EQ(0u, s.size<e::two>());
	EXPECT_EQ(0u, s.size<e::three>());
	EXPECT_EQ(0u, s.size());
	EXPECT_TRUE(s.empty<e::one>());
	EXPECT_TRUE(s.empty<e::two>());
	EXPECT_TRUE(s.empty<e::three>());

	s.subscribe<e::one>([]() { return 0; });
	s.subscribe<e::one>([]() { return 0; });
	s.subscribe<e::one>([]() { return 0; });
	s.subscribe<e::one>([]() { return 0; });
	s.subscribe<e::one>([]() { return 0; });
	s.subscribe<e::two>([]() { return 0; });
	s.subscribe<e::two>([]() { return 0; });
	s.subscribe<e::two>([]() { return 0; });
	s.subscribe<e::two>([]() { return 0; });
	s.subscribe<e::two>([]() { return 0; });
	s.subscribe<e::three>([](float, double) { return 0; });
	s.subscribe<e::three>([](float, double) { return 0; });
	s.subscribe<e::three>([](float, double) { return 0; });
	s.subscribe<e::three>([](float, double) { return 0; });
	s.subscribe<e::three>([](float, double) { return 0; });

	EXPECT_FALSE(s.empty());
	EXPECT_EQ(15u, s.size());
	s.clear();
	EXPECT_TRUE(s.empty());
	EXPECT_EQ(0u, s.size());
}

} // namespace
