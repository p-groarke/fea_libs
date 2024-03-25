#include <chrono>
#include <cstdio>
#include <fea/time/high_range_duration.hpp>
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

	fea::localtime(&t, &local_tm);
	fea::gmtime(&t, &gm_tm);

	// Windows doesn't provide IANA database, and downloading in CI is
	// problematic. This test is run on macOS and Linux.
#if !FEA_WINDOWS
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
#endif

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

#if FEA_CPP17 && FEA_GCC_GE(13)
	{
		date::sys_days d{};
		std::string_view s = fea::suffixed_day(d);
		EXPECT_EQ("1st", s);

		d += date::days{ 1 };
		std::wstring_view s2 = fea::suffixed_day(d);
		EXPECT_EQ(L"2nd", s2);

		d += date::days{ 1 };
		std::u16string_view s3 = fea::suffixed_day(d);
		EXPECT_EQ(u"3rd", s3);

		d += date::days{ 1 };
		std::u32string_view s4 = fea::suffixed_day(d);
		EXPECT_EQ(U"4th", s4);
	}
#endif
}

TEST(time, timepoint_conversions) {
	using namespace std::chrono;
	using namespace date::literals;

	// system to x
	{
		system_clock::time_point sys_tp = system_clock::now();
		steady_clock::time_point steady_tp = fea::to_steady(sys_tp);

		std::string sys_str = fea::to_string(sys_tp);
		std::string steady_str = fea::to_string(steady_tp);
		EXPECT_EQ(steady_str, sys_str);

		// std::cout << sys_str << std::endl;
		// std::cout << steady_str << std::endl;

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

		std::string steady_str = fea::to_string(steady_tp);
		std::string sys_str = fea::to_string(sys_tp);
		EXPECT_EQ(sys_str, steady_str);

		// std::cout << steady_str << std::endl;
		// std::cout << sys_str << std::endl;

		auto steady_dur = steady_clock::duration{
			date::floor<system_clock::duration>(steady_tp.time_since_epoch())
		};
		auto steady_count = steady_dur.count();

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

		std::string sys_str = fea::to_string(sys_d);
		std::string steady_str = fea::to_string(stdy_d);
		EXPECT_EQ(steady_str, sys_str);

		// std::cout << sys_str << std::endl;
		// std::cout << steady_str << std::endl;

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

		std::string steady_str = fea::to_string(stdy_d);
		std::string sys_str = fea::to_string(sys_d);
		EXPECT_EQ(sys_str, steady_str);

		// std::cout << steady_str << std::endl;
		// std::cout << sys_str << std::endl;

		auto steady_count = stdy_d.time_since_epoch().count();
		auto sys_count = sys_d.time_since_epoch().count();

		EXPECT_EQ(sys_count, steady_count);
	}
}

/*
using uduration = std::chrono::duration<size_t, std::nano>;
using utime_point
		= std::chrono::time_point<std::chrono::steady_clock, uduration>;

TEST(time, experiments) {
	using namespace std::chrono;
	system_clock::duration sys_max_dur{
		std::numeric_limits<system_clock::duration::rep>::max()
	};
	steady_clock::duration steady_max_dur{
		std::numeric_limits<steady_clock::duration::rep>::max()
	};
	uduration u_max_dur{ std::numeric_limits<uduration::rep>::max() };

	system_clock::time_point sys_max_time{ sys_max_dur };
	steady_clock::time_point steady_max_time{ steady_max_dur };
	utime_point u_max_time{ u_max_dur };

	std::cout << fea::to_string(sys_max_time) << std::endl;
	std::cout << fea::to_string(steady_max_time) << std::endl;

	{
		auto stdy_d = date::floor<date::days>(u_max_time);
		// date::sys_days sys_d = fea::to_sys(stdy_d);
		// auto ymd = date::year_month_day(sys_d);

		auto s = date::floor<std::chrono::seconds>(u_max_time) - stdy_d;

		// std::string ustr = date::format("%F ", ymd) + date::format("%T", s);
		std::string ustr = date::format("%T", s);
		std::cout << ustr << std::endl;
	}
}
*/

TEST(time, year_month_days) {
	// 2020 is leap year.
	using namespace date::literals;

	date::sys_days t{ 2020_y / date::jan / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 29);

	t = date::sys_days{ 2020_y / date::feb / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 29);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2019_y / date::jan / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 28);

	t = date::sys_days{ 2019_y / date::feb / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 28);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::mar / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 30);

	t = date::sys_days{ 2020_y / date::apr / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 30);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::may / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 30);

	t = date::sys_days{ 2020_y / date::jun / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 30);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::jul / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::aug / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 30);

	t = date::sys_days{ 2020_y / date::sep / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 30);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::oct / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 30);

	t = date::sys_days{ 2020_y / date::nov / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 30);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::dec / 8_d };
	EXPECT_EQ(fea::this_month_days(t).count(), 31);
	EXPECT_EQ(fea::next_month_days(t).count(), 31);

	t = date::sys_days{ 2020_y / date::mar / 8_d };
	EXPECT_EQ(fea::this_year_days(t).count(), 366);
	EXPECT_EQ(fea::next_year_days(t).count(), 365);

	t = date::sys_days{ 2019_y / date::mar / 8_d };
	EXPECT_EQ(fea::this_year_days(t).count(), 365);
	EXPECT_EQ(fea::next_year_days(t).count(), 366);

	t = date::sys_days{ 2021_y / date::mar / 8_d };
	EXPECT_EQ(fea::this_year_days(t).count(), 365);
	EXPECT_EQ(fea::next_year_days(t).count(), 365);
}
} // namespace
