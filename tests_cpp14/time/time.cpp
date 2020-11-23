﻿#include <chrono>
#include <cstdio>
#include <fea/time/time.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <limits>

namespace {
TEST(time, basics) {
	auto now_tp = std::chrono::system_clock::now();
	fea::dsys_seconds now = now_tp;

	// Get the system utc time and local time using C apis.
	std::time_t t = std::chrono::system_clock::to_time_t(now_tp);
	std::tm local_tm{};
	std::tm gm_tm{};

#if FEA_WINDOWS
	localtime_s(&local_tm, &t);
#else
	localtime_r(&t, &local_tm);
#endif

#if FEA_WINDOWS
	gmtime_s(&gm_tm, &t);
#else
	gmtime_s(&t, &gm_tm);
#endif

	std::tm test_localtm = fea::to_local_tm(now);
	EXPECT_EQ(test_localtm.tm_sec, local_tm.tm_sec);
	EXPECT_EQ(test_localtm.tm_min, local_tm.tm_min);
	EXPECT_EQ(test_localtm.tm_hour, local_tm.tm_hour);
	EXPECT_EQ(test_localtm.tm_mday, local_tm.tm_mday);
	EXPECT_EQ(test_localtm.tm_mon, local_tm.tm_mon);
	EXPECT_EQ(test_localtm.tm_year, local_tm.tm_year);
	EXPECT_EQ(test_localtm.tm_wday, local_tm.tm_wday);
	EXPECT_EQ(test_localtm.tm_yday, local_tm.tm_yday);
	EXPECT_EQ(test_localtm.tm_isdst, local_tm.tm_isdst);

	std::tm test_gmtm = fea::to_utc_tm(now);
	EXPECT_EQ(test_gmtm.tm_sec, gm_tm.tm_sec);
	EXPECT_EQ(test_gmtm.tm_min, gm_tm.tm_min);
	EXPECT_EQ(test_gmtm.tm_hour, gm_tm.tm_hour);
	EXPECT_EQ(test_gmtm.tm_mday, gm_tm.tm_mday);
	EXPECT_EQ(test_gmtm.tm_mon, gm_tm.tm_mon);
	EXPECT_EQ(test_gmtm.tm_year, gm_tm.tm_year);
	EXPECT_EQ(test_gmtm.tm_wday, gm_tm.tm_wday);
	EXPECT_EQ(test_gmtm.tm_yday, gm_tm.tm_yday);
	EXPECT_EQ(test_gmtm.tm_isdst, gm_tm.tm_isdst);

	fea::hh_mm_ss hms = fea::to_hms(now);
	date::year_month_day ymd = fea::to_ymd(now);
	date::year_month_weekday ymw = fea::to_ymw(now);

	EXPECT_EQ(test_gmtm.tm_sec, int(hms.seconds().count()));
	EXPECT_EQ(test_gmtm.tm_min, int(hms.minutes().count()));
	EXPECT_EQ(test_gmtm.tm_hour, int(hms.hours().count()));

	EXPECT_EQ(test_gmtm.tm_mday, int(unsigned(ymd.day())));
	EXPECT_EQ(test_gmtm.tm_mon + 1, int(unsigned(ymd.month())));
	EXPECT_EQ(test_gmtm.tm_year + 1900, int(ymd.year()));

	EXPECT_EQ(test_gmtm.tm_wday, int(ymw.weekday().c_encoding()));
	EXPECT_EQ(test_gmtm.tm_mon + 1, int(unsigned(ymw.month())));
	EXPECT_EQ(test_gmtm.tm_year + 1900, int(ymw.year()));

	// Strings
	std::string tp_str = fea::to_string(now);
	std::string tm_str = fea::to_string(test_gmtm);
	// printf("%s\n", tp_str.c_str());
	// printf("%s\n", tm_str.c_str());
	EXPECT_EQ(tp_str, tm_str);

	// Elapsed double
	{
		// 60 seconds wrap elapsed
		fea::dseconds start_s{ 59.0 };
		fea::dseconds end_s{ 2.0 };
		fea::dseconds diff_s = fea::elapsed(start_s, end_s, 60.0);
		EXPECT_EQ(diff_s.count(), 3.0);

		start_s = fea::dseconds{ 1.0 };
		diff_s = fea::elapsed(start_s, end_s, 60.0);
		EXPECT_EQ(diff_s.count(), 1.0);

		// 60 minutes wrap elapsed
		fea::dminutes start_m{ 59.0 };
		fea::dminutes end_m{ 2.0 };
		fea::dminutes diff_m = fea::elapsed(start_m, end_m, 60.0);
		EXPECT_EQ(diff_m.count(), 3.0);

		start_m = fea::dminutes{ 1.0 };
		diff_m = fea::elapsed(start_m, end_m, 60.0);
		EXPECT_EQ(diff_m.count(), 1.0);

		// 24 hour diff
		fea::dhours start_h{ 23.0 };
		fea::dhours end_h{ 2.0 };
		fea::dhours diff_h = fea::elapsed(start_h, end_h, 24.0);
		EXPECT_EQ(diff_h.count(), 3.0);

		start_h = fea::dhours{ 1.0 };
		diff_h = fea::elapsed(start_h, end_h, 24.0);
		EXPECT_EQ(diff_h.count(), 1.0);

		// 24 hour diff
		fea::ddays start_d{ 364 };
		fea::ddays end_d{ 2 };
		fea::ddays diff_d = fea::elapsed(start_d, end_d, 365.0);
		EXPECT_EQ(diff_d.count(), 3.0);

		start_d = fea::ddays{ 1.0 };
		diff_d = fea::elapsed(start_d, end_d, 365.0);
		EXPECT_EQ(diff_d.count(), 1.0);
	}

	// Elapsed float
	{
		// 60 seconds wrap elapsed
		fea::fseconds start_s{ 59.f };
		fea::fseconds end_s{ 2.f };
		fea::fseconds diff_s = fea::elapsed(start_s, end_s, 60.f);
		EXPECT_EQ(diff_s.count(), 3.f);

		start_s = fea::dseconds{ 1.f };
		diff_s = fea::elapsed(start_s, end_s, 60.f);
		EXPECT_EQ(diff_s.count(), 1.f);

		// 60 minutes wrap elapsed
		fea::fminutes start_m{ 59.f };
		fea::fminutes end_m{ 2.f };
		fea::fminutes diff_m = fea::elapsed(start_m, end_m, 60.f);
		EXPECT_EQ(diff_m.count(), 3.f);

		start_m = fea::dminutes{ 1.f };
		diff_m = fea::elapsed(start_m, end_m, 60.f);
		EXPECT_EQ(diff_m.count(), 1.f);

		// 24 hour diff
		fea::fhours start_h{ 23.f };
		fea::fhours end_h{ 2.f };
		fea::fhours diff_h = fea::elapsed(start_h, end_h, 24.f);
		EXPECT_EQ(diff_h.count(), 3.f);

		start_h = fea::dhours{ 1.f };
		diff_h = fea::elapsed(start_h, end_h, 24.f);
		EXPECT_EQ(diff_h.count(), 1.f);

		// 24 hour diff
		fea::fdays start_d{ 364.f };
		fea::fdays end_d{ 2.f };
		fea::fdays diff_d = fea::elapsed(start_d, end_d, 365.f);
		EXPECT_EQ(diff_d.count(), 3.f);

		start_d = fea::ddays{ 1.f };
		diff_d = fea::elapsed(start_d, end_d, 365.f);
		EXPECT_EQ(diff_d.count(), 1.f);
	}
}

TEST(time, timepoint_conversions) {
	using namespace std::chrono;
	using namespace date::literals;

	// system to x
	{
		system_clock::time_point sys_tp = system_clock::now();
		steady_clock::time_point steady_tp = fea::to_steady(sys_tp);

		std::cout << fea::to_string(sys_tp) << std::endl;
		std::cout << fea::to_string(steady_tp) << std::endl;

		auto sys_count = sys_tp.time_since_epoch().count();

		// We still need to have identical duration types.
		// Cast to system clock duration.
		auto steady_count = duration_cast<system_clock::duration>(
				steady_tp.time_since_epoch())
									.count();

		EXPECT_EQ(steady_count, sys_count);
	}

	// steady to x
	{
		steady_clock::time_point steady_tp = steady_clock::now();
		system_clock::time_point sys_tp = fea::to_sys(steady_tp);

		std::cout << fea::to_string(steady_tp) << std::endl;
		std::cout << fea::to_string(sys_tp) << std::endl;

		auto steady_count = steady_tp.time_since_epoch().count();

		// We still need to have identical duration types.
		// Cast to system clock duration.
		auto sys_count = duration_cast<steady_clock::duration>(
				sys_tp.time_since_epoch())
								 .count();

		EXPECT_EQ(sys_count, steady_count);
	}

	// sys_days to steady_days
	{
		date::sys_days sys_d = 2020_y / date::jan / 1_d;
		fea::steady_days stdy_d = fea::to_steady(sys_d);

		std::cout << fea::to_string(sys_d) << std::endl;
		std::cout << fea::to_string(stdy_d) << std::endl;

		auto sys_count = sys_d.time_since_epoch().count();
		auto steady_count = stdy_d.time_since_epoch().count();

		EXPECT_EQ(steady_count, sys_count);
	}

	// steady_days to sys_days
	{
		// ymd doesn't support steady_days, so convert it.
		fea::steady_days stdy_d
				= fea::to_steady(date::sys_days{ 2020_y / date::jan / 1_d });

		date::sys_days sys_d = fea::to_sys(stdy_d);

		std::cout << fea::to_string(stdy_d) << std::endl;
		std::cout << fea::to_string(sys_d) << std::endl;

		auto steady_count = stdy_d.time_since_epoch().count();
		auto sys_count = sys_d.time_since_epoch().count();

		EXPECT_EQ(sys_count, steady_count);
	}
}

// TEST(time, experiments) {
//	using namespace std::chrono;
//	system_clock::duration sys_max_dur{
//		std::numeric_limits<system_clock::duration::rep>::max()
//	};
//	steady_clock::duration steady_max_dur{
//		std::numeric_limits<steady_clock::duration::rep>::max()
//	};
//
//	system_clock::time_point sys_max_time{ sys_max_dur };
//	steady_clock::time_point steady_max_time{ steady_max_dur };
//
//	fea::steady_seconds steady_s{ std::chrono::seconds(
//			std::numeric_limits<long long>::max()) };
//
//	std::cout << fea::to_string(sys_max_time) << std::endl;
//	std::cout << fea::to_string(steady_max_time) << std::endl;
//	std::cout << fea::to_string(steady_s) << std::endl;
//}
} // namespace
