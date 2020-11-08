﻿#include <atomic>
#include <fea/events/event_system.hpp>
#include <gtest/gtest.h>

namespace {
TEST(event_system, basics) {
	enum class events : unsigned { one, two, three, count };
	enum class channels : unsigned { one, two, three, count };
	fea::event_system<events, channels, void(), void(int), void(float, double)>
			s{};

	fea::notifier_id invalid_nid;
	fea::notifier_id nid1 = s.add_notifier();
	fea::notifier_id nid2 = s.add_notifier();

	EXPECT_FALSE(s.contains(invalid_nid));
	EXPECT_TRUE(s.contains(nid1));
	EXPECT_TRUE(s.contains(nid1));

	s.remove_notifier(nid1);
	EXPECT_FALSE(s.contains(nid1));
	EXPECT_TRUE(s.contains(nid2));

	s.remove_notifier(nid2);
	EXPECT_FALSE(s.contains(nid1));
	EXPECT_FALSE(s.contains(nid2));

	nid1 = s.add_notifier();
	nid2 = s.add_notifier();

	EXPECT_FALSE(s.contains(invalid_nid));
	EXPECT_TRUE(s.contains(nid1));
	EXPECT_TRUE(s.contains(nid1));

	fea::event_sys_id<events, events::one> invalid_notifier_eid;
	fea::event_sys_id<events, events::one, channels, channels::one>
			invalid_channel_eid;
	fea::event_sys_id<events, events::one, channels, channels::count>
			invalid_channel_eid2;

	EXPECT_FALSE(s.contains(invalid_notifier_eid));
	EXPECT_FALSE(s.contains(invalid_channel_eid));
	EXPECT_FALSE(s.contains(invalid_channel_eid2));

	EXPECT_TRUE(s.empty(nid1));
	EXPECT_TRUE(s.empty(nid2));
	EXPECT_TRUE(s.empty<events::one>(nid1));
	EXPECT_TRUE(s.empty<events::two>(nid1));
	EXPECT_TRUE(s.empty<events::three>(nid1));
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_TRUE(s.empty<events::two>(nid2));
	EXPECT_TRUE(s.empty<events::three>(nid2));

	EXPECT_TRUE(s.empty<channels::one>());
	EXPECT_TRUE(s.empty<channels::two>());
	EXPECT_TRUE(s.empty<channels::three>());
	EXPECT_TRUE((s.empty<channels::one, events::one>()));
	EXPECT_TRUE((s.empty<channels::one, events::two>()));
	EXPECT_TRUE((s.empty<channels::one, events::three>()));
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_TRUE((s.empty<channels::two, events::two>()));
	EXPECT_TRUE((s.empty<channels::two, events::three>()));
	EXPECT_TRUE((s.empty<channels::three, events::one>()));
	EXPECT_TRUE((s.empty<channels::three, events::two>()));
	EXPECT_TRUE((s.empty<channels::three, events::three>()));

	EXPECT_EQ(0, s.size(nid1));
	EXPECT_EQ(0, s.size(nid2));
	EXPECT_EQ(0, s.size<events::one>(nid1));
	EXPECT_EQ(0, s.size<events::two>(nid1));
	EXPECT_EQ(0, s.size<events::three>(nid1));
	EXPECT_EQ(0, s.size<events::one>(nid2));
	EXPECT_EQ(0, s.size<events::two>(nid2));
	EXPECT_EQ(0, s.size<events::three>(nid2));

	EXPECT_EQ(0, s.size<channels::one>());
	EXPECT_EQ(0, s.size<channels::two>());
	EXPECT_EQ(0, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0, (s.size<channels::one, events::two>()));
	EXPECT_EQ(0, (s.size<channels::one, events::three>()));
	EXPECT_EQ(0, (s.size<channels::two, events::one>()));
	EXPECT_EQ(0, (s.size<channels::two, events::two>()));
	EXPECT_EQ(0, (s.size<channels::two, events::three>()));

	s.reserve<events::one>(nid1, 1);
	s.reserve<events::two>(nid1, 2);
	s.reserve<events::three>(nid1, 3);
	EXPECT_EQ(1, s.capacity<events::one>(nid1));
	EXPECT_EQ(2, s.capacity<events::two>(nid1));
	EXPECT_EQ(3, s.capacity<events::three>(nid1));
	EXPECT_EQ(0, s.capacity<events::one>(nid2));
	EXPECT_EQ(0, s.capacity<events::two>(nid2));
	EXPECT_EQ(0, s.capacity<events::three>(nid2));

	s.reserve(nid1, 4);
	EXPECT_EQ(4, s.capacity<events::one>(nid1));
	EXPECT_EQ(4, s.capacity<events::two>(nid1));
	EXPECT_EQ(4, s.capacity<events::three>(nid1));
	EXPECT_EQ(0, s.capacity<events::one>(nid2));
	EXPECT_EQ(0, s.capacity<events::two>(nid2));
	EXPECT_EQ(0, s.capacity<events::three>(nid2));

	s.reserve<channels::one, events::one>(1);
	s.reserve<channels::one, events::two>(2);
	s.reserve<channels::one, events::three>(3);
	EXPECT_EQ(1, (s.capacity<channels::one, events::one>()));
	EXPECT_EQ(2, (s.capacity<channels::one, events::two>()));
	EXPECT_EQ(3, (s.capacity<channels::one, events::three>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::one>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::two>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::three>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::one>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::two>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::three>()));

	s.reserve<channels::one>(4);
	EXPECT_EQ(4, (s.capacity<channels::one, events::one>()));
	EXPECT_EQ(4, (s.capacity<channels::one, events::two>()));
	EXPECT_EQ(4, (s.capacity<channels::one, events::three>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::one>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::two>()));
	EXPECT_EQ(0, (s.capacity<channels::two, events::three>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::one>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::two>()));
	EXPECT_EQ(0, (s.capacity<channels::three, events::three>()));


	int test_event_one = 0;
	int test_event_two = 0;

	// Notifiers

	auto eid1 = s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	auto eid2 = s.subscribe<events::two>(
			nid2, [&](int i) { test_event_two += i; });

	EXPECT_FALSE(s.empty(nid1));
	EXPECT_FALSE(s.empty<events::one>(nid1));
	EXPECT_TRUE(s.empty<events::two>(nid1));
	EXPECT_FALSE(s.empty(nid2));
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_FALSE(s.empty<events::two>(nid2));

	EXPECT_EQ(1, s.size(nid1));
	EXPECT_EQ(1, s.size<events::one>(nid1));
	EXPECT_EQ(0, s.size<events::two>(nid1));
	EXPECT_EQ(1, s.size(nid2));
	EXPECT_EQ(0, s.size<events::one>(nid2));
	EXPECT_EQ(1, s.size<events::two>(nid2));

	s.trigger<events::one>(nid1);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 0);
	s.trigger<events::two>(nid1, 42);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 0);
	s.trigger<events::three>(nid1, 42.f, -42.0);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 0);
	s.trigger<events::one>(nid2);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 0);
	s.trigger<events::two>(nid2, 42);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::three>(nid2, 42.f, -42.0);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);

	s.unsubscribe<events::one>(eid1);
	s.unsubscribe<events::two>(eid2);

	EXPECT_TRUE(s.empty(nid1));
	EXPECT_TRUE(s.empty<events::one>(nid1));
	EXPECT_TRUE(s.empty<events::two>(nid1));
	EXPECT_TRUE(s.empty(nid2));
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_TRUE(s.empty<events::two>(nid2));

	EXPECT_EQ(0, s.size(nid1));
	EXPECT_EQ(0, s.size<events::one>(nid1));
	EXPECT_EQ(0, s.size<events::two>(nid1));
	EXPECT_EQ(0, s.size(nid2));
	EXPECT_EQ(0, s.size<events::one>(nid2));
	EXPECT_EQ(0, s.size<events::two>(nid2));

	s.trigger<events::one>(nid1);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::two>(nid1, 42);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::three>(nid1, 42.f, -42.0);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::one>(nid2);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::two>(nid2, 42);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<events::three>(nid2, 42.f, -42.0);
	EXPECT_EQ(test_event_one, 1);
	EXPECT_EQ(test_event_two, 42);


	// Channels

	auto c_eid1 = s.subscribe<channels::one, events::one>(
			[&]() { ++test_event_one; });
	auto c_eid2 = s.subscribe<channels::two, events::two>(
			[&](int i) { test_event_two += i; });

	EXPECT_FALSE(s.empty<channels::one>());
	EXPECT_FALSE((s.empty<channels::one, events::one>()));
	EXPECT_TRUE((s.empty<channels::one, events::two>()));
	EXPECT_FALSE(s.empty<channels::two>());
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_FALSE((s.empty<channels::two, events::two>()));

	EXPECT_EQ(1, s.size<channels::one>());
	EXPECT_EQ(1, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0, (s.size<channels::one, events::two>()));
	EXPECT_EQ(1, s.size<channels::two>());
	EXPECT_EQ(0, (s.size<channels::two, events::one>()));
	EXPECT_EQ(1, (s.size<channels::two, events::two>()));

	s.trigger<channels::one, events::one>();
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<channels::one, events::two>(42);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<channels::one, events::three>(42.f, -42.0);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<channels::two, events::one>();
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 42);
	s.trigger<channels::two, events::two>(1);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::one, events::three>(42.f, -42.0);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);

	s.unsubscribe(c_eid1);
	s.unsubscribe(c_eid2);

	EXPECT_TRUE(s.empty<channels::one>());
	EXPECT_TRUE((s.empty<channels::one, events::one>()));
	EXPECT_TRUE((s.empty<channels::one, events::two>()));
	EXPECT_TRUE(s.empty<channels::two>());
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_TRUE((s.empty<channels::two, events::two>()));

	EXPECT_EQ(0, s.size<channels::one>());
	EXPECT_EQ(0, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0, (s.size<channels::one, events::two>()));
	EXPECT_EQ(0, s.size<channels::two>());
	EXPECT_EQ(0, (s.size<channels::two, events::one>()));
	EXPECT_EQ(0, (s.size<channels::two, events::two>()));

	s.trigger<channels::one, events::one>();
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::one, events::two>(42);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::one, events::three>(42.f, -42.0);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::two, events::one>();
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::two, events::two>(1);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
	s.trigger<channels::one, events::three>(42.f, -42.0);
	EXPECT_EQ(test_event_one, 2);
	EXPECT_EQ(test_event_two, 43);
}

} // namespace
