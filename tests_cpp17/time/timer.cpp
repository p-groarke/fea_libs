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

TEST(timer, experiment) {
	using namespace std::chrono_literals;

	fea::timer<void()> t{ fea::dseconds(10) };
	std::this_thread::sleep_for(2s);
	t.update();
	EXPECT_GE(t.elapsed().count(), 20.0);
}

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

	timer.subscribe_time(
			fea::dsteady_seconds(timer.start_time().count() + fea::ddays(1)),
			[&]() { ++event_count.num_time_callback; });

	std::this_thread::sleep_for(100ms);
	timer.update();
	// printf("%s\n", fea::to_string(timer.time()).c_str());
	// printf("\n");
	EXPECT_GE(timer.elapsed(), fea::dseconds(100ms));

	EXPECT_EQ(event_count.num_secs, 1u);
	EXPECT_EQ(event_count.num_mins, 1u);
	EXPECT_EQ(event_count.num_hrs, 1u);
	EXPECT_EQ(event_count.num_days, 1u);
	EXPECT_EQ(event_count.num_weeks, 1u);
	EXPECT_EQ(event_count.num_months, 1u);
	EXPECT_EQ(event_count.num_years, 1u);
	EXPECT_EQ(event_count.num_elapsed_callback, 1u);
	EXPECT_EQ(event_count.num_time_callback, 1u);

	auto elapsed_before_pause = timer.elapsed();
	auto time_before_pause = timer.time();

	timer.pause();
	timer.update();
	std::this_thread::sleep_for(100ms);
	timer.update();
	timer.unpause();
	auto elapsed_after_pause = timer.elapsed();
	auto time_after_pause = timer.time();

	EXPECT_EQ(event_count.num_pause, 1u);
	EXPECT_EQ(event_count.num_unpause, 2u); // 1 for start transition
	EXPECT_EQ(0s, time_after_pause - time_before_pause);
	EXPECT_EQ(0s, elapsed_after_pause - elapsed_before_pause);
}

TEST(timer, start_date) {
	auto start_days = 2000_y / date::feb / 2_d;
	auto start_time = fea::dhours(8) + fea::dminutes(30) + fea::dseconds(30.5);

	fea::high_range_duration hrd{ start_days };
	hrd += start_time;

	fea::timer<void()> timer{ start_days, 10s };
	timer.start_time() += start_time;

	EXPECT_EQ(timer.time_precise().days(), hrd.days());
	EXPECT_EQ(timer.time_precise().seconds(), hrd.seconds());
	EXPECT_EQ(timer.time_precise().nanoseconds(), hrd.nanoseconds());

	std::this_thread::sleep_for(100ms);
	timer.update();
	EXPECT_GE(timer.elapsed(), fea::dseconds(100ms));
}

TEST(timer, three_seconds) {
	using namespace std::chrono;
	// 1 second == 1 second.
	fea::timer<void()> timer;

	size_t seconds_passed = 0;
	timer.events().subscribe<fea::timer_event::seconds>(
			[&]() { ++seconds_passed; });

	const fea::useconds stop_time{ 3 };

	auto start_time = steady_clock::now();
	while (timer.elapsed_precise().seconds() < stop_time) {
		timer.update();
	}
	auto end_time = steady_clock::now();

	auto e = end_time - start_time;
	seconds elapsed = date::round<seconds>(e);
	EXPECT_EQ(elapsed.count(), 3);

	EXPECT_EQ(seconds_passed, 3u);
}

TEST(timer, three_minutes) {
	using namespace std::chrono;
	// 1 second == 1 minute.
	fea::timer<void()> timer(fea::dseconds{ fea::dminutes{ 1 } });

	size_t minutes_passed = 0;
	timer.events().subscribe<fea::timer_event::minutes>(
			[&]() { ++minutes_passed; });

	const fea::uminutes stop_time{ 3 };

	auto start_time = steady_clock::now();
	while (timer.elapsed_precise().seconds() < stop_time) {
		timer.update();
	}
	auto end_time = steady_clock::now();

	auto e = end_time - start_time;
	seconds elapsed = date::round<seconds>(e);
	EXPECT_EQ(elapsed.count(), 3);

	EXPECT_EQ(minutes_passed, 3u);
}

TEST(timer, three_days) {
	using namespace std::chrono;
	// 1 second == 1 day.
	fea::timer<void()> timer(fea::dseconds{ fea::ddays{ 1 } });

	size_t hours_passed = 0;
	size_t days_passed = 0;
	timer.events().subscribe<fea::timer_event::hours>(
			[&]() { ++hours_passed; });
	timer.events().subscribe<fea::timer_event::days>([&]() { ++days_passed; });

	const fea::udays stop_time{ 3 };

	auto start_time = steady_clock::now();
	while (timer.elapsed_precise().days() < stop_time) {
		timer.update();
	}
	auto end_time = steady_clock::now();

	auto e = end_time - start_time;
	seconds elapsed = date::round<seconds>(e);
	EXPECT_EQ(elapsed.count(), 3);

	EXPECT_EQ(hours_passed, 72u);
	EXPECT_EQ(days_passed, 3u);
}

TEST(timer, three_weeks) {
	using namespace std::chrono;
	// 1 second == 1 week.
	fea::timer<void()> timer{ fea::dweeks{ 1 } };

	size_t weeks_passed = 0;
	timer.events().subscribe<fea::timer_event::weeks>(
			[&]() { ++weeks_passed; });

	// 1972 is leap year
	const fea::udays stop_time{ fea::uweeks{ 3 } };

	auto start_time = steady_clock::now();
	while (timer.elapsed_precise().days() < stop_time) {
		timer.update();
	}
	auto end_time = steady_clock::now();

	auto e = end_time - start_time;
	seconds elapsed = date::round<seconds>(e);
	EXPECT_EQ(elapsed.count(), 3);

	EXPECT_EQ(weeks_passed, 3u);
}

TEST(timer, three_years) {
	using namespace std::chrono;
	// 1 second == 12 months.
	fea::timer<void()> timer{ fea::dmonths{ 12 } };

	size_t months_passed = 0;
	size_t years_passed = 0;
	timer.events().subscribe<fea::timer_event::months>(
			[&]() { ++months_passed; });
	timer.events().subscribe<fea::timer_event::years>(
			[&]() { ++years_passed; });

	// 1972 is leap year
	const fea::udays stop_time{ 1096 };

	auto start_time = steady_clock::now();
	while (timer.elapsed_precise().days() < stop_time) {
		timer.update();
	}
	auto end_time = steady_clock::now();

	auto e = end_time - start_time;
	seconds elapsed = date::round<seconds>(e);
	EXPECT_EQ(elapsed.count(), 3);

	EXPECT_EQ(months_passed, 36u);
	EXPECT_EQ(years_passed, 3u);
}

} // namespace
