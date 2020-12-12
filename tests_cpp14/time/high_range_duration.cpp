#include <chrono>
#include <cstdio>
#include <fea/time/high_range_duration.hpp>
#include <fea/time/time.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <limits>

namespace {
TEST(high_range_duration, basics) {
	using namespace std::chrono;
	using namespace std::chrono_literals;

	// Basic
	{
		// 5 days, 5 seconds, 5 nanoseconds.
		fea::unanoseconds nano_time{ 0 };
		nano_time += 5ns;
		nano_time += 5s;
		nano_time += date::days(5);

		fea::high_range_duration hrd{ nano_time };
		EXPECT_EQ(hrd.days().count(), 5u);
		EXPECT_EQ(hrd.seconds().count(), 5u);
		EXPECT_EQ(hrd.remainder().count(), 5u);
		EXPECT_EQ(hrd.total_remainder().count(), nano_time.count());
	}

	// Addition
	{
		fea::unanoseconds nano_time{ 0 };
		nano_time += 5ns;
		nano_time += 5s;
		nano_time += date::days(5);
		fea::high_range_duration hrd{ nano_time };

		fea::unanoseconds add_answer = nano_time + nano_time;

		fea::high_range_duration hrd2{ hrd };
		hrd2 += hrd;
		EXPECT_EQ(hrd2.days().count(), 10u);
		EXPECT_EQ(hrd2.seconds().count(), 10u);
		EXPECT_EQ(hrd2.remainder().count(), 10u);
		EXPECT_EQ(size_t(hrd2.total_remainder().count()), add_answer.count());

		fea::unanoseconds nano_time2{ 1s };
		nano_time2 -= 4ns;
		add_answer = nano_time + nano_time2;

		fea::high_range_duration hrd_add{ nano_time2 };
		hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 5u);
		EXPECT_EQ(hrd2.seconds().count(), 6u);
		EXPECT_EQ(hrd2.remainder().count(), 1u);
		EXPECT_EQ(size_t(hrd2.total_remainder().count()), add_answer.count());

		nano_time2 = 24h;
		nano_time2 -= 4s;
		add_answer = nano_time + nano_time2;

		hrd_add = nano_time2;
		hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 6u);
		EXPECT_EQ(hrd2.seconds().count(), 1u);
		EXPECT_EQ(hrd2.remainder().count(), 5u);
		EXPECT_EQ(size_t(hrd2.total_remainder().count()), add_answer.count());

		nano_time2 = 24h;
		nano_time2 -= 4s;
		nano_time2 += 1s;
		nano_time2 -= 4ns;
		add_answer = nano_time + nano_time2;

		hrd_add = nano_time2;
		hrd2 = hrd;
		hrd2 += hrd_add;

		EXPECT_EQ(hrd2.days().count(), 6u);
		EXPECT_EQ(hrd2.seconds().count(), 2u);
		EXPECT_EQ(hrd2.remainder().count(), 1u);
		EXPECT_EQ(size_t(hrd2.total_remainder().count()), add_answer.count());
	}
}
} // namespace
