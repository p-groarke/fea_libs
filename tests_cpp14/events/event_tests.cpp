#include <cstring>
#include <fea/events/event_stack.hpp>
//#include <fea/events/event_system.hpp>
#include <gtest/gtest.h>

namespace {
/*
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

int t_one = 0;
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

int t_two = 0;
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

int t_three = 0;
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

s.execute<e::one>();
EXPECT_EQ(5, t_one);
s.execute<e::two>();
EXPECT_EQ(5, t_two);
s.execute<e::three>(0.1f, 0.1);
EXPECT_EQ(5, t_three);

EXPECT_EQ(0, s.at<e::one>(front_1)());
EXPECT_EQ(0, s.at<e::two>(front_2)());
EXPECT_EQ(0, s.at<e::three>(front_3)(0.f, 0.1));

EXPECT_EQ(4, s.at<e::one>(back_1)());
EXPECT_EQ(4, s.at<e::two>(back_2)());
EXPECT_EQ(4, s.at<e::three>(back_3)(0.f, 0.1));

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

// int t1 = 0;
// int t2 = 0;
// int t3 = 0;
// fea::event_stack<e, void(), void(), void()> s2(
//		[&t1]() { ++t1; }, [&t2]() { ++t2; }, [&t3]() { ++t3; });
// s2.execute<e::one>();
// s2.execute<e::two>();
// s2.execute<e::three>();
// EXPECT_EQ(1, t1);
// EXPECT_EQ(1, t2);
// EXPECT_EQ(1, t3);
}

TEST(event_system, basics) {
enum class events : unsigned { one, two, three, count };
enum class channels : unsigned { one, two, three, count };
fea::event_system<events, channels, void(), void(int), void(float, double)>
		s{};

fea::notifier_id nid1 = s.add_notifier();
fea::notifier_id nid2 = s.add_notifier();

int test_event_one = 0;
int test_event_two = 0;

auto eid1 = s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });

auto eid2 = s.subscribe<events::two>(
		nid2, [&](int i) { test_event_two += i; });

s.execute_event<events::one>(nid1);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 0);
s.execute_event<events::two>(nid1, 42);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 0);
s.execute_event<events::three>(nid1, 42.f, -42.0);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 0);
s.execute_event<events::one>(nid2);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 0);
s.execute_event<events::two>(nid2, 42);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::three>(nid2, 42.f, -42.0);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);

s.unsubscribe<events::one>(nid1, eid1);
s.unsubscribe<events::two>(nid2, eid2);

s.execute_event<events::one>(nid1);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::two>(nid1, 42);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::three>(nid1, 42.f, -42.0);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::one>(nid2);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::two>(nid2, 42);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::three>(nid2, 42.f, -42.0);
EXPECT_EQ(test_event_one, 1);
EXPECT_EQ(test_event_two, 42);

eid1 = s.subscribe<events::one>(channels::one, [&]() { ++test_event_one; });
eid2 = s.subscribe<events::two>(
		channels::two, [&](int i) { test_event_two += i; });

s.execute_event<events::one>(channels::one);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::two>(channels::one, 42);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::three>(channels::one, 42.f, -42.0);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::one>(channels::two);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 42);
s.execute_event<events::two>(channels::two, 1);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::three>(channels::one, 42.f, -42.0);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);

s.unsubscribe<events::one>(channels::one, eid1);
s.unsubscribe<events::two>(channels::two, eid2);

s.execute_event<events::one>(channels::one);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::two>(channels::one, 42);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::three>(channels::one, 42.f, -42.0);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::one>(channels::two);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::two>(channels::two, 1);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
s.execute_event<events::three>(channels::one, 42.f, -42.0);
EXPECT_EQ(test_event_one, 2);
EXPECT_EQ(test_event_two, 43);
}

TEST(event_system, multi_subscribe) {
enum class events : unsigned { one, two, three, count };
enum class channels : unsigned { one, count };
fea::event_system<events, channels, void(), void(), void()> s{};

fea::notifier_id nid1 = s.add_notifier();
// fea::notifier_id nid2 = s.add_notifier();

int test_event_one = 0;

auto [eid1, eid2, eid3] = s.subscribe(
		nid1, [&]() { ++test_event_one; }, [&]() { ++test_event_one; },
		[&]() { ++test_event_one; });

s.execute_event<events::one>(nid1);
s.execute_event<events::two>(nid1);
s.execute_event<events::three>(nid1);
EXPECT_EQ(test_event_one, 3);

auto [cid1, cid2, cid3] = s.subscribe(
		channels::one, [&]() { ++test_event_one; },
		[&]() { ++test_event_one; }, [&]() { ++test_event_one; });

s.execute_event<events::one>(channels::one);
s.execute_event<events::two>(channels::one);
s.execute_event<events::three>(channels::one);
EXPECT_EQ(test_event_one, 6);
}

*/
} // namespace
