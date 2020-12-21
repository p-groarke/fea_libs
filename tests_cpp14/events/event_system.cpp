#include <atomic>
#include <fea/events/event_system.hpp>
#include <gtest/gtest.h>

namespace {
TEST(event_system, basics) {
	enum class events : unsigned { one, two, three, count };
	enum class channels : unsigned { one, two, three, count };
	fea::event_system<events, channels, void(), void(int), void(float, double)>
			s{};

	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());
	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

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

	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

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

	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	EXPECT_EQ(0u, s.size(nid1));
	EXPECT_EQ(0u, s.size(nid2));
	EXPECT_EQ(0u, s.size<events::one>(nid1));
	EXPECT_EQ(0u, s.size<events::two>(nid1));
	EXPECT_EQ(0u, s.size<events::three>(nid1));
	EXPECT_EQ(0u, s.size<events::one>(nid2));
	EXPECT_EQ(0u, s.size<events::two>(nid2));
	EXPECT_EQ(0u, s.size<events::three>(nid2));

	EXPECT_EQ(0u, s.size<channels::one>());
	EXPECT_EQ(0u, s.size<channels::two>());
	EXPECT_EQ(0u, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::one, events::two>()));
	EXPECT_EQ(0u, (s.size<channels::one, events::three>()));
	EXPECT_EQ(0u, (s.size<channels::two, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::two, events::two>()));
	EXPECT_EQ(0u, (s.size<channels::two, events::three>()));

	s.reserve<events::one>(nid1, 1);
	s.reserve<events::two>(nid1, 2);
	s.reserve<events::three>(nid1, 3);
	EXPECT_EQ(1u, s.capacity<events::one>(nid1));
	EXPECT_EQ(2u, s.capacity<events::two>(nid1));
	EXPECT_EQ(3u, s.capacity<events::three>(nid1));
	EXPECT_EQ(0u, s.capacity<events::one>(nid2));
	EXPECT_EQ(0u, s.capacity<events::two>(nid2));
	EXPECT_EQ(0u, s.capacity<events::three>(nid2));

	s.reserve(nid1, 4);
	EXPECT_EQ(4u, s.capacity<events::one>(nid1));
	EXPECT_EQ(4u, s.capacity<events::two>(nid1));
	EXPECT_EQ(4u, s.capacity<events::three>(nid1));
	EXPECT_EQ(0u, s.capacity<events::one>(nid2));
	EXPECT_EQ(0u, s.capacity<events::two>(nid2));
	EXPECT_EQ(0u, s.capacity<events::three>(nid2));

	s.reserve<channels::one, events::one>(1);
	s.reserve<channels::one, events::two>(2);
	s.reserve<channels::one, events::three>(3);
	EXPECT_EQ(1u, (s.capacity<channels::one, events::one>()));
	EXPECT_EQ(2u, (s.capacity<channels::one, events::two>()));
	EXPECT_EQ(3u, (s.capacity<channels::one, events::three>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::one>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::two>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::three>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::one>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::two>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::three>()));

	s.reserve<channels::one>(4);
	EXPECT_EQ(4u, (s.capacity<channels::one, events::one>()));
	EXPECT_EQ(4u, (s.capacity<channels::one, events::two>()));
	EXPECT_EQ(4u, (s.capacity<channels::one, events::three>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::one>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::two>()));
	EXPECT_EQ(0u, (s.capacity<channels::two, events::three>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::one>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::two>()));
	EXPECT_EQ(0u, (s.capacity<channels::three, events::three>()));

	EXPECT_TRUE(s.empty());
	EXPECT_EQ(0u, s.size());


	int test_event_one = 0;
	int test_event_two = 0;

	// Notifiers

	auto eid1 = s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	auto eid2 = s.subscribe<events::two>(
			nid2, [&](int i) { test_event_two += i; });

	EXPECT_FALSE(s.empty());
	EXPECT_FALSE(s.event_empty<events::one>());
	EXPECT_FALSE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_FALSE(s.empty(nid1));
	EXPECT_FALSE(s.empty<events::one>(nid1));
	EXPECT_TRUE(s.empty<events::two>(nid1));
	EXPECT_FALSE(s.empty(nid2));
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_FALSE(s.empty<events::two>(nid2));

	EXPECT_EQ(2u, s.size());
	EXPECT_EQ(1u, s.event_size<events::one>());
	EXPECT_EQ(1u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	EXPECT_EQ(1u, s.size(nid1));
	EXPECT_EQ(1u, s.size<events::one>(nid1));
	EXPECT_EQ(0u, s.size<events::two>(nid1));
	EXPECT_EQ(1u, s.size(nid2));
	EXPECT_EQ(0u, s.size<events::one>(nid2));
	EXPECT_EQ(1u, s.size<events::two>(nid2));

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

	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_TRUE(s.empty(nid1));
	EXPECT_TRUE(s.empty<events::one>(nid1));
	EXPECT_TRUE(s.empty<events::two>(nid1));
	EXPECT_TRUE(s.empty(nid2));
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_TRUE(s.empty<events::two>(nid2));

	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	EXPECT_EQ(0u, s.size(nid1));
	EXPECT_EQ(0u, s.size<events::one>(nid1));
	EXPECT_EQ(0u, s.size<events::two>(nid1));
	EXPECT_EQ(0u, s.size(nid2));
	EXPECT_EQ(0u, s.size<events::one>(nid2));
	EXPECT_EQ(0u, s.size<events::two>(nid2));

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

	EXPECT_FALSE(s.empty());
	EXPECT_FALSE(s.event_empty<events::one>());
	EXPECT_FALSE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_FALSE(s.empty<channels::one>());
	EXPECT_FALSE((s.empty<channels::one, events::one>()));
	EXPECT_TRUE((s.empty<channels::one, events::two>()));
	EXPECT_FALSE(s.empty<channels::two>());
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_FALSE((s.empty<channels::two, events::two>()));

	EXPECT_EQ(2u, s.size());
	EXPECT_EQ(1u, s.event_size<events::one>());
	EXPECT_EQ(1u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	EXPECT_EQ(1u, s.size<channels::one>());
	EXPECT_EQ(1u, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::one, events::two>()));
	EXPECT_EQ(1u, s.size<channels::two>());
	EXPECT_EQ(0u, (s.size<channels::two, events::one>()));
	EXPECT_EQ(1u, (s.size<channels::two, events::two>()));

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

	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_TRUE(s.empty<channels::one>());
	EXPECT_TRUE((s.empty<channels::one, events::one>()));
	EXPECT_TRUE((s.empty<channels::one, events::two>()));
	EXPECT_TRUE(s.empty<channels::two>());
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_TRUE((s.empty<channels::two, events::two>()));

	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	EXPECT_EQ(0u, s.size<channels::one>());
	EXPECT_EQ(0u, (s.size<channels::one, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::one, events::two>()));
	EXPECT_EQ(0u, s.size<channels::two>());
	EXPECT_EQ(0u, (s.size<channels::two, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::two, events::two>()));

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

	eid1 = s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	eid2 = s.subscribe<events::two>(nid2, [&](int i) { test_event_two += i; });
	c_eid1 = s.subscribe<channels::one, events::one>(
			[&]() { ++test_event_one; });
	c_eid2 = s.subscribe<channels::two, events::two>(
			[&](int i) { test_event_two += i; });

	s.at(eid1)();
	EXPECT_EQ(test_event_one, 3);
	EXPECT_EQ(test_event_two, 43);
	s.at(eid2)(1);
	EXPECT_EQ(test_event_one, 3);
	EXPECT_EQ(test_event_two, 44);
	s.at(c_eid1)();
	EXPECT_EQ(test_event_one, 4);
	EXPECT_EQ(test_event_two, 44);
	s.at(c_eid2)(1);
	EXPECT_EQ(test_event_one, 4);
	EXPECT_EQ(test_event_two, 45);

	s.at_unchecked(eid1)();
	EXPECT_EQ(test_event_one, 5);
	EXPECT_EQ(test_event_two, 45);
	s.at_unchecked(eid2)(1);
	EXPECT_EQ(test_event_one, 5);
	EXPECT_EQ(test_event_two, 46);
	s.at_unchecked(c_eid1)();
	EXPECT_EQ(test_event_one, 6);
	EXPECT_EQ(test_event_two, 46);
	s.at_unchecked(c_eid2)(1);
	EXPECT_EQ(test_event_one, 6);
	EXPECT_EQ(test_event_two, 47);

	EXPECT_FALSE(s.empty());
	s.clear_subscribers();
	EXPECT_TRUE(s.empty());

	s.subscribe<events::one>(nid1, []() {});
	s.subscribe<events::one>(nid2, []() {});
	s.subscribe<channels::one, events::one>([]() {});
	s.subscribe<channels::two, events::one>([]() {});
	s.subscribe<events::two>(nid1, [](int) {});
	s.subscribe<events::two>(nid2, [](int) {});
	s.subscribe<channels::one, events::two>([](int) {});
	s.subscribe<channels::two, events::two>([](int) {});
	s.subscribe<events::three>(nid1, [](float, double) {});
	s.subscribe<events::three>(nid2, [](float, double) {});
	s.subscribe<channels::one, events::three>([](float, double) {});
	s.subscribe<channels::two, events::three>([](float, double) {});

	EXPECT_FALSE(s.empty());
	EXPECT_FALSE(s.event_empty<events::one>());
	EXPECT_FALSE(s.event_empty<events::two>());
	EXPECT_FALSE(s.event_empty<events::three>());

	EXPECT_EQ(12u, s.size());
	EXPECT_EQ(4u, s.event_size<events::one>());
	EXPECT_EQ(4u, s.event_size<events::two>());
	EXPECT_EQ(4u, s.event_size<events::three>());
	EXPECT_EQ(3u, s.size(nid1));
	EXPECT_EQ(3u, s.size(nid2));
	EXPECT_EQ(3u, s.size<channels::one>());
	EXPECT_EQ(3u, s.size<channels::two>());

	s.clear();
	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());

	nid1 = s.add_notifier();
	nid2 = s.add_notifier();
	s.subscribe<events::one>(nid1, []() {});
	s.subscribe<events::one>(nid2, []() {});
	s.subscribe<channels::one, events::one>([]() {});
	s.subscribe<channels::two, events::one>([]() {});
	s.subscribe<events::two>(nid1, [](int) {});
	s.subscribe<events::two>(nid2, [](int) {});
	s.subscribe<channels::one, events::two>([](int) {});
	s.subscribe<channels::two, events::two>([](int) {});
	s.subscribe<events::three>(nid1, [](float, double) {});
	s.subscribe<events::three>(nid2, [](float, double) {});
	s.subscribe<channels::one, events::three>([](float, double) {});
	s.subscribe<channels::two, events::three>([](float, double) {});

	EXPECT_FALSE(s.empty());
	EXPECT_FALSE(s.event_empty<events::one>());
	EXPECT_FALSE(s.event_empty<events::two>());
	EXPECT_FALSE(s.event_empty<events::three>());

	EXPECT_EQ(12u, s.size());
	EXPECT_EQ(4u, s.event_size<events::one>());
	EXPECT_EQ(4u, s.event_size<events::two>());
	EXPECT_EQ(4u, s.event_size<events::three>());
	EXPECT_EQ(3u, s.size(nid1));
	EXPECT_EQ(3u, s.size(nid2));
	EXPECT_EQ(3u, s.size<channels::one>());
	EXPECT_EQ(3u, s.size<channels::two>());

	s.event_clear<events::one>();
	EXPECT_FALSE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_FALSE(s.event_empty<events::two>());
	EXPECT_FALSE(s.event_empty<events::three>());

	EXPECT_EQ(8u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(4u, s.event_size<events::two>());
	EXPECT_EQ(4u, s.event_size<events::three>());
	EXPECT_EQ(2u, s.size(nid1));
	EXPECT_EQ(2u, s.size(nid2));
	EXPECT_EQ(2u, s.size<channels::one>());
	EXPECT_EQ(2u, s.size<channels::two>());

	s.event_clear<events::two>();
	EXPECT_FALSE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_FALSE(s.event_empty<events::three>());

	EXPECT_EQ(4u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(4u, s.event_size<events::three>());
	EXPECT_EQ(1u, s.size(nid1));
	EXPECT_EQ(1u, s.size(nid2));
	EXPECT_EQ(1u, s.size<channels::one>());
	EXPECT_EQ(1u, s.size<channels::two>());

	s.event_clear<events::three>();
	EXPECT_TRUE(s.empty());
	EXPECT_TRUE(s.event_empty<events::one>());
	EXPECT_TRUE(s.event_empty<events::two>());
	EXPECT_TRUE(s.event_empty<events::three>());

	EXPECT_EQ(0u, s.size());
	EXPECT_EQ(0u, s.event_size<events::one>());
	EXPECT_EQ(0u, s.event_size<events::two>());
	EXPECT_EQ(0u, s.event_size<events::three>());
	EXPECT_EQ(0u, s.size(nid1));
	EXPECT_EQ(0u, s.size(nid2));
	EXPECT_EQ(0u, s.size<channels::one>());
	EXPECT_EQ(0u, s.size<channels::two>());

	s.clear();

	nid1 = s.add_notifier();
	nid2 = s.add_notifier();
	s.subscribe<events::one>(nid1, []() {});
	s.subscribe<events::one>(nid2, []() {});
	s.subscribe<channels::one, events::one>([]() {});
	s.subscribe<channels::two, events::one>([]() {});
	s.subscribe<events::two>(nid1, [](int) {});
	s.subscribe<events::two>(nid2, [](int) {});
	s.subscribe<channels::one, events::two>([](int) {});
	s.subscribe<channels::two, events::two>([](int) {});
	s.subscribe<events::three>(nid1, [](float, double) {});
	s.subscribe<events::three>(nid2, [](float, double) {});
	s.subscribe<channels::one, events::three>([](float, double) {});
	s.subscribe<channels::two, events::three>([](float, double) {});

	EXPECT_FALSE(s.empty(nid1));
	EXPECT_EQ(3u, s.size(nid1));
	EXPECT_FALSE(s.empty(nid2));
	EXPECT_EQ(3u, s.size(nid2));
	EXPECT_FALSE(s.empty<channels::one>());
	EXPECT_EQ(3u, s.size<channels::one>());
	EXPECT_FALSE(s.empty<channels::two>());
	EXPECT_EQ(3u, s.size<channels::two>());

	s.clear(nid1);
	EXPECT_TRUE(s.empty(nid1));
	EXPECT_EQ(0u, s.size(nid1));
	EXPECT_FALSE(s.empty(nid2));
	EXPECT_EQ(3u, s.size(nid2));

	s.clear<events::one>(nid2);
	EXPECT_TRUE(s.empty<events::one>(nid2));
	EXPECT_EQ(0u, s.size<events::one>(nid2));
	EXPECT_FALSE(s.empty<events::two>(nid2));
	EXPECT_EQ(1u, s.size<events::two>(nid2));
	EXPECT_FALSE(s.empty<events::three>(nid2));
	EXPECT_EQ(1u, s.size<events::three>(nid2));

	s.clear<channels::one>();
	EXPECT_TRUE(s.empty<channels::one>());
	EXPECT_EQ(0u, s.size<channels::one>());
	EXPECT_FALSE(s.empty<channels::two>());
	EXPECT_EQ(3u, s.size<channels::two>());

	s.clear<channels::two, events::one>();
	EXPECT_TRUE((s.empty<channels::two, events::one>()));
	EXPECT_EQ(0u, (s.size<channels::two, events::one>()));
	EXPECT_FALSE((s.empty<channels::two, events::two>()));
	EXPECT_EQ(1u, (s.size<channels::two, events::two>()));
	EXPECT_FALSE((s.empty<channels::two, events::three>()));
	EXPECT_EQ(1u, (s.size<channels::two, events::three>()));
}

TEST(event_system, multithreading) {
	enum class events : unsigned { one, two, three, count };
	enum class channels : unsigned { one, two, three, count };
	fea::event_system<events, channels, void(), void(int), void(float, double)>
			s{};

	std::atomic<int> test_event_one{ 0 };

	auto nid1 = s.add_notifier();
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });
	s.subscribe<events::one>(nid1, [&]() { ++test_event_one; });

	s.trigger_mt<events::one>(nid1);
	EXPECT_EQ(test_event_one.load(), 10);

	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });
	s.subscribe<channels::one, events::one>([&]() { ++test_event_one; });

	s.trigger_mt<channels::one, events::one>();
	EXPECT_EQ(test_event_one.load(), 20);
}
} // namespace
