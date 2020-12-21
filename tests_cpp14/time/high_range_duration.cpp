#include <chrono>
#include <cstdio>
#include <fea/time/high_range_duration.hpp>
#include <fea/time/time.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <limits>

namespace {
TEST(time_high_range_duration, basics) {
	using namespace std::chrono;
	using namespace std::chrono_literals;
	using namespace date::literals;

	// Basic
	{
		// 5 days, 5 seconds, 5 nanoseconds.
		fea::unanoseconds nano_time{ 0 };
		nano_time += 5ns;
		nano_time += 1s;

		fea::high_range_duration hrd{ nano_time };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 1u);
		EXPECT_EQ(hrd.nanoseconds().count(), 5u);
		EXPECT_EQ(hrd.count_nanoseconds().count(), nano_time.count());
	}

	// Addition
	{
		fea::unanoseconds nano_time{ 0 };
		nano_time += 5ns;
		nano_time += 1s;
		fea::high_range_duration hrd{ nano_time };

		fea::unanoseconds add_answer = nano_time + nano_time;

		fea::high_range_duration hrd2{ hrd };
		hrd2 += hrd;
		EXPECT_EQ(hrd2.days().count(), 0u);
		EXPECT_EQ(hrd2.seconds().count(), 2u);
		EXPECT_EQ(hrd2.nanoseconds().count(), 10u);
		EXPECT_EQ(size_t(hrd2.count_nanoseconds().count()), add_answer.count());

		fea::unanoseconds nano_time2{ 1s };
		nano_time2 -= 4ns;
		add_answer = nano_time + nano_time2;

		fea::high_range_duration hrd_add{ nano_time2 };
		hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 0u);
		EXPECT_EQ(hrd2.seconds().count(), 2u);
		EXPECT_EQ(hrd2.nanoseconds().count(), 1u);
		EXPECT_EQ(size_t(hrd2.count_nanoseconds().count()), add_answer.count());
	}

	// More addition
	{
		fea::useconds sec_time{ 0 };
		sec_time += 48h;
		sec_time += 5s;

		fea::useconds sec_time2 = sec_time;
		sec_time2 -= 4s;
		fea::useconds add_answer = sec_time + sec_time2;

		fea::high_range_duration hrd = sec_time;
		fea::high_range_duration hrd_add = sec_time2;
		fea::high_range_duration hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 4u);
		EXPECT_EQ(hrd2.seconds().count(), 6u);
		EXPECT_EQ(hrd2.nanoseconds().count(), 0u);
		EXPECT_EQ(size_t(hrd2.count().count()), add_answer.count());

		sec_time2 = 24h;
		sec_time2 -= 4s;
		sec_time2 += 1s;
		add_answer = sec_time + sec_time2;

		hrd_add = sec_time2;
		hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 3u);
		EXPECT_EQ(hrd2.seconds().count(), 2u);
		EXPECT_EQ(hrd2.nanoseconds().count(), 0u);
		EXPECT_EQ(size_t(hrd2.count().count()), add_answer.count());
	}
}

TEST(time_high_range_duration, ctors) {
	fea::high_range_duration hrd;

	// days
	{
		fea::udays d{ 2u };
		hrd = { d };
		EXPECT_EQ(hrd.days().count(), 2u);
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);

		fea::ddays dd{ 0.5 };
		hrd = { dd };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 43'200u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);

		dd = fea::ddays{ fea::dseconds{ 0.5 } };
		hrd = { dd };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 500'000'000u);
	}

	// seconds
	{
		fea::useconds s{ 172'800u };
		hrd = { s };
		EXPECT_EQ(hrd.days().count(), 2u);
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);

		s = fea::useconds{ 2u };
		hrd = { s };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 2u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);

		fea::dseconds ds{ 0.5 };
		hrd = { ds };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 500'000'000u);
	}

	// nanoseconds
	{
		// Can't test days to ns in 32bit.

		fea::unanoseconds n{ 2'000'000'000u };
		hrd = { n };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 2u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);

		n = fea::unanoseconds{ 2u };
		hrd = { n };
		EXPECT_EQ(hrd.days().count(), 0u);
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 2u);
	}

	// ymd
	{
		using namespace date::literals;
		auto ymd = 2000_y / date::mar / 2_d;
		date::sys_days ymd_in_days = ymd;

		hrd = { ymd };
		EXPECT_EQ(hrd.days().count(),
				size_t(ymd_in_days.time_since_epoch().count()));
		EXPECT_EQ(hrd.seconds().count(), 0u);
		EXPECT_EQ(hrd.nanoseconds().count(), 0u);
	}
}
} // namespace
