#include <atomic>
#include <cstring>
#include <fea/events/event_system.hpp>
#include <gtest/gtest.h>

namespace {
TEST(event_system, basics) {
	// enum class events : unsigned { one, two, three, count };
	// enum class channels : unsigned { one, two, three, count };
	// fea::event_system<events, channels, void(), void(int), void(float,
	// double)> 		s{};

	// fea::notifier_id nid1 = s.add_notifier();
	// fea::notifier_id nid2 = s.add_notifier();

	// int test_event_one = 0;
	// int test_event_two = 0;

	// auto eid1 = s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });

	// auto eid2 = s.subscribe<events::two>(
	//		nid2, [&](int i) { test_event_two += i; });

	// s.execute_event<events::one>(nid1);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 0);
	// s.execute_event<events::two>(nid1, 42);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 0);
	// s.execute_event<events::three>(nid1, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 0);
	// s.execute_event<events::one>(nid2);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 0);
	// s.execute_event<events::two>(nid2, 42);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::three>(nid2, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);

	// s.unsubscribe<events::one>(nid1, eid1);
	// s.unsubscribe<events::two>(nid2, eid2);

	// s.execute_event<events::one>(nid1);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::two>(nid1, 42);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::three>(nid1, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::one>(nid2);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::two>(nid2, 42);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::three>(nid2, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 1);
	// EXPECT_EQ(test_event_two, 42);

	// eid1 = s.subscribe<events::one>(channels::one, [&]() { ++test_event_one;
	// }); eid2 = s.subscribe<events::two>( 		channels::two, [&](int i) {
	// test_event_two += i; });

	// s.execute_event<events::one>(channels::one);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::two>(channels::one, 42);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::three>(channels::one, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::one>(channels::two);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 42);
	// s.execute_event<events::two>(channels::two, 1);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::three>(channels::one, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);

	// s.unsubscribe<events::one>(channels::one, eid1);
	// s.unsubscribe<events::two>(channels::two, eid2);

	// s.execute_event<events::one>(channels::one);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::two>(channels::one, 42);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::three>(channels::one, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::one>(channels::two);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::two>(channels::two, 1);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
	// s.execute_event<events::three>(channels::one, 42.f, -42.0);
	// EXPECT_EQ(test_event_one, 2);
	// EXPECT_EQ(test_event_two, 43);
}

TEST(event_system, multi_subscribe) {
	// enum class events : unsigned { one, two, three, count };
	// enum class channels : unsigned { one, count };
	// fea::event_system<events, channels, void(), void(), void()> s{};

	// fea::notifier_id nid1 = s.add_notifier();
	//// fea::notifier_id nid2 = s.add_notifier();

	// int test_event_one = 0;

	// auto [eid1, eid2, eid3] = s.subscribe(
	//		nid1, [&]() { ++test_event_one; }, [&]() { ++test_event_one; },
	//		[&]() { ++test_event_one; });

	// s.execute_event<events::one>(nid1);
	// s.execute_event<events::two>(nid1);
	// s.execute_event<events::three>(nid1);
	// EXPECT_EQ(test_event_one, 3);

	// auto [cid1, cid2, cid3] = s.subscribe(
	//		channels::one, [&]() { ++test_event_one; },
	//		[&]() { ++test_event_one; }, [&]() { ++test_event_one; });

	// s.execute_event<events::one>(channels::one);
	// s.execute_event<events::two>(channels::one);
	// s.execute_event<events::three>(channels::one);
	// EXPECT_EQ(test_event_one, 6);
}

} // namespace
