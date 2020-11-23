#include <chrono>
#include <fea/time/timer.hpp>
#include <gtest/gtest.h>
#include <thread>

namespace {
using namespace std::chrono_literals;
using namespace date::literals;
struct event_counter {
	size_t num_secs = 0;
	size_t num_mins = 0;
	size_t num_hrs = 0;
	size_t num_days = 0;
	size_t num_weeks = 0;
	size_t num_months = 0;
	size_t num_years = 0;
	size_t num_pause = 0;
	size_t num_unpause = 0;
	size_t num_elapsed_callback = 0;
	size_t num_time_callback = 0;
};

TEST(timer, basics) {
	event_counter event_count;

	// std::chrono::system_clock::duration d = fea::dseconds(10);
	// std::chrono::steady_clock::duration d2 = date::years(10);

	// dyears is time_ratio. aka, 1 real second = 10 years elapsed.
	fea::timer<void()> timer{ fea::dyears(10) };

	timer.events().subscribe<fea::timer_event::on_pause>(
			[&]() { ++event_count.num_pause; });
	timer.events().subscribe<fea::timer_event::on_unpause>(
			[&]() { ++event_count.num_unpause; });
	timer.events().subscribe<fea::timer_event::seconds>(
			[&]() { ++event_count.num_secs; });
	timer.events().subscribe<fea::timer_event::minutes>(
			[&]() { ++event_count.num_mins; });
	timer.events().subscribe<fea::timer_event::hours>(
			[&]() { ++event_count.num_hrs; });
	timer.events().subscribe<fea::timer_event::days>(
			[&]() { ++event_count.num_days; });
	timer.events().subscribe<fea::timer_event::weeks>(
			[&]() { ++event_count.num_weeks; });
	timer.events().subscribe<fea::timer_event::months>(
			[&]() { ++event_count.num_months; });
	timer.events().subscribe<fea::timer_event::years>(
			[&]() { ++event_count.num_years; });

	timer.subscribe_elapsed(
			fea::ddays(1), [&]() { ++event_count.num_elapsed_callback; });

	timer.subscribe_time(timer.start_time() + fea::ddays(1),
			[&]() { ++event_count.num_time_callback; });

	std::this_thread::sleep_for(100ms);
	timer.update();
	// printf("%s\n", fea::to_string(timer.time()).c_str());
	// printf("\n");
	EXPECT_GE(timer.elapsed(), fea::dseconds(100ms));

	EXPECT_EQ(event_count.num_secs, 1);
	EXPECT_EQ(event_count.num_mins, 1);
	EXPECT_EQ(event_count.num_hrs, 1);
	EXPECT_EQ(event_count.num_days, 1);
	EXPECT_EQ(event_count.num_weeks, 1);
	EXPECT_EQ(event_count.num_months, 1);
	EXPECT_EQ(event_count.num_years, 1);
	EXPECT_EQ(event_count.num_elapsed_callback, 1);
	EXPECT_EQ(event_count.num_time_callback, 1);

	auto elapsed_before_pause = timer.elapsed();
	auto time_before_pause = timer.time();

	timer.pause();
	timer.update();
	std::this_thread::sleep_for(100ms);
	timer.update();
	timer.unpause();
	auto elapsed_after_pause = timer.elapsed();
	auto time_after_pause = timer.time();

	EXPECT_EQ(event_count.num_pause, 1);
	EXPECT_EQ(event_count.num_unpause, 2); // 1 for start transition
	EXPECT_EQ(0s, time_after_pause - time_before_pause);
	EXPECT_EQ(0s, elapsed_after_pause - elapsed_before_pause);
}

TEST(timer, start_date) {
	auto real_start_time = std::chrono::steady_clock::now();

	auto start_days = 2020_y / date::jan / 1_d;

	fea::dsteady_seconds start_date = fea::to_steady(start_days)
			+ fea::dhours(8) + fea::dminutes(30) + fea::dseconds(30.5);

	fea::timer<void()> timer{ start_date, 10s };
	EXPECT_EQ(timer.time(), start_date);

	fea::dseconds seconds_time_diff
			= std::chrono::floor<fea::dseconds>(start_date - real_start_time);

	std::this_thread::sleep_for(100ms);
	timer.update();
	EXPECT_GE(timer.elapsed(), fea::dseconds(100ms));
}

} // namespace
