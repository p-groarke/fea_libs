/*
BSD 3-Clause License

Copyright (c) 2022, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/utils/platform.hpp"

#if FEA_CPP17
#include "fea/macros/literals.hpp"
#include "fea/string/string.hpp"
#endif

#include <array>
#include <chrono>
#include <cmath>
#include <ctime>
#include <date/date.h>
#include <date/tz.h>
#include <iomanip>
#include <sstream>
#include <string>

#if FEA_CPP17
#include <filesystem>
#endif

namespace fea {
#if FEA_CPP17
using file_clock = std::filesystem::file_time_type::clock;
#endif

using sys_seconds = date::sys_seconds;
using sys_minutes = date::sys_time<std::chrono::minutes>;
using sys_hours = date::sys_time<std::chrono::hours>;
using sys_days = date::sys_days;
using sys_weeks = date::sys_time<date::weeks>;
using sys_months = date::sys_time<date::months>;
using sys_years = date::sys_time<date::years>;

template <class Duration>
using steady_time
		= std::chrono::time_point<std::chrono::steady_clock, Duration>;

using steady_seconds = steady_time<std::chrono::seconds>;
using steady_minutes = steady_time<std::chrono::minutes>;
using steady_hours = steady_time<std::chrono::hours>;
using steady_days = steady_time<date::days>;
using steady_weeks = steady_time<date::weeks>;
using steady_months = steady_time<date::months>;
using steady_years = steady_time<date::years>;

template <class Clock>
using clock_seconds = std::chrono::time_point<Clock, std::chrono::seconds>;
template <class Clock>
using clock_minutes = std::chrono::time_point<Clock, std::chrono::minutes>;
template <class Clock>
using clock_hours = std::chrono::time_point<Clock, std::chrono::hours>;
template <class Clock>
using clock_days = std::chrono::time_point<Clock, date::days>;
template <class Clock>
using clock_weeks = std::chrono::time_point<Clock, date::weeks>;
template <class Clock>
using clock_months = std::chrono::time_point<Clock, date::months>;
template <class Clock>
using clock_years = std::chrono::time_point<Clock, date::years>;


/*
unsigned, uses size_t
*/

using unanoseconds
		= std::chrono::duration<size_t, std::chrono::nanoseconds::period>;
using umicroseconds
		= std::chrono::duration<size_t, std::chrono::microseconds::period>;
using umilliseconds
		= std::chrono::duration<size_t, std::chrono::milliseconds::period>;
using useconds = std::chrono::duration<size_t>;
using uminutes = std::chrono::duration<size_t, std::chrono::minutes::period>;
using uhours = std::chrono::duration<size_t, std::chrono::hours::period>;
using udays = std::chrono::duration<size_t, date::days::period>;
using uweeks = std::chrono::duration<size_t, date::weeks::period>;
using umonths = std::chrono::duration<size_t, date::months::period>;
using uyears = std::chrono::duration<size_t, date::years::period>;

using usys_seconds = date::sys_time<useconds>;
using usys_minutes = date::sys_time<uminutes>;
using usys_hours = date::sys_time<uhours>;
using usys_days = date::sys_time<udays>;
using usys_weeks = date::sys_time<uweeks>;
using usys_months = date::sys_time<umonths>;
using usys_years = date::sys_time<uyears>;

using usteady_seconds = steady_time<useconds>;
using usteady_minutes = steady_time<uminutes>;
using usteady_hours = steady_time<uhours>;
using usteady_days = steady_time<udays>;
using usteady_weeks = steady_time<uweeks>;
using usteady_months = steady_time<umonths>;
using usteady_years = steady_time<uyears>;


/*
floats
*/

using fseconds = std::chrono::duration<float>;
using fminutes = std::chrono::duration<float, std::chrono::minutes::period>;
using fhours = std::chrono::duration<float, std::chrono::hours::period>;
using fdays = std::chrono::duration<float, date::days::period>;
using fweeks = std::chrono::duration<float, date::weeks::period>;
using fmonths = std::chrono::duration<float, date::months::period>;
using fyears = std::chrono::duration<float, date::years::period>;

using fsys_seconds = date::sys_time<fseconds>;
using fsys_minutes = date::sys_time<fminutes>;
using fsys_hours = date::sys_time<fhours>;
using fsys_days = date::sys_time<fdays>;
using fsys_weeks = date::sys_time<fweeks>;
using fsys_months = date::sys_time<fmonths>;
using fsys_years = date::sys_time<fyears>;

using fsteady_seconds = steady_time<fseconds>;
using fsteady_minutes = steady_time<fminutes>;
using fsteady_hours = steady_time<fhours>;
using fsteady_days = steady_time<fdays>;
using fsteady_weeks = steady_time<fweeks>;
using fsteady_months = steady_time<fmonths>;
using fsteady_years = steady_time<fyears>;

template <class Clock>
using fclock_seconds = std::chrono::time_point<Clock, fseconds>;
template <class Clock>
using fclock_minutes = std::chrono::time_point<Clock, fminutes>;
template <class Clock>
using fclock_hours = std::chrono::time_point<Clock, fhours>;
template <class Clock>
using fclock_days = std::chrono::time_point<Clock, fdays>;
template <class Clock>
using fclock_weeks = std::chrono::time_point<Clock, fweeks>;
template <class Clock>
using fclock_months = std::chrono::time_point<Clock, fmonths>;
template <class Clock>
using fclock_years = std::chrono::time_point<Clock, fyears>;


/*
doubles
*/

using dseconds = std::chrono::duration<double>;
using dminutes = std::chrono::duration<double, std::chrono::minutes::period>;
using dhours = std::chrono::duration<double, std::chrono::hours::period>;
using ddays = std::chrono::duration<double, date::days::period>;
using dweeks = std::chrono::duration<double, date::weeks::period>;
using dmonths = std::chrono::duration<double, date::months::period>;
using dyears = std::chrono::duration<double, date::years::period>;

using dsys_seconds = date::sys_time<dseconds>;
using dsys_minutes = date::sys_time<dminutes>;
using dsys_hours = date::sys_time<dhours>;
using dsys_days = date::sys_time<ddays>;
using dsys_weeks = date::sys_time<dweeks>;
using dsys_months = date::sys_time<dmonths>;
using dsys_years = date::sys_time<dyears>;

using dsteady_seconds = steady_time<dseconds>;
using dsteady_minutes = steady_time<dminutes>;
using dsteady_hours = steady_time<dhours>;
using dsteady_days = steady_time<ddays>;
using dsteady_weeks = steady_time<dweeks>;
using dsteady_months = steady_time<dmonths>;
using dsteady_years = steady_time<dyears>;

template <class Clock>
using dclock_seconds = std::chrono::time_point<Clock, dseconds>;
template <class Clock>
using dclock_minutes = std::chrono::time_point<Clock, dminutes>;
template <class Clock>
using dclock_hours = std::chrono::time_point<Clock, dhours>;
template <class Clock>
using dclock_days = std::chrono::time_point<Clock, ddays>;
template <class Clock>
using dclock_weeks = std::chrono::time_point<Clock, dweeks>;
template <class Clock>
using dclock_months = std::chrono::time_point<Clock, dmonths>;
template <class Clock>
using dclock_years = std::chrono::time_point<Clock, dyears>;


// Other useful types.
using hh_mm_ss = date::hh_mm_ss<std::chrono::seconds>;
using uhh_mm_ss = date::hh_mm_ss<useconds>;
using fhh_mm_ss = date::hh_mm_ss<fseconds>;
using dhh_mm_ss = date::hh_mm_ss<dseconds>;


// Converts duration to size_t duration.
// Useful if you do high value arithmetic.
template <class Duration>
using size_t_duration
		= std::chrono::duration<size_t, typename Duration::period>;


// Helper functions

// Platform independent gmtime.
inline void gmtime(const std::time_t* timer, std::tm* buf) {
#if FEA_WINDOWS
	gmtime_s(buf, timer);
#else
	gmtime_r(timer, buf);
#endif
}

// Platform independent localtime.
inline void localtime(const std::time_t* timer, std::tm* buf) {
#if FEA_WINDOWS
	localtime_s(buf, timer);
#else
	localtime_r(timer, buf);
#endif
}

// Converts sys_time timepoint to std::tm.
// Ignores local timezone.
template <class T>
std::tm to_utc_tm(date::sys_time<T> tp) {
	auto days_tp = date::floor<date::days>(tp);
	auto ymd = date::year_month_day(days_tp);
	auto weekday = date::year_month_weekday(days_tp).weekday();

	auto secs = date::floor<std::chrono::seconds>(tp) - days_tp;
	auto tod = date::make_time(secs);
	date::days days_since_jan1 = days_tp - date::sys_days(ymd.year() / 1 / 1);

	// For literals
	using namespace date;

	std::tm ret{};
	ret.tm_sec = int(tod.seconds().count());
	ret.tm_min = int(tod.minutes().count());
	ret.tm_hour = int(tod.hours().count());
	ret.tm_mday = int((ymd.day() - 0_d).count());
	ret.tm_mon = int((ymd.month() - January).count());
	ret.tm_year = int((ymd.year() - 1900_y).count());
	ret.tm_wday = int((weekday - Sunday).count());
	ret.tm_yday = int(days_since_jan1.count());
	ret.tm_isdst = 0;
	return ret;
}

// Converts sys_time timepoint to std::tm.
// Uses local timezone.
template <class T>
std::tm to_local_tm(date::sys_time<T> sys_tp) {
	auto zoned_time = date::make_zoned(date::current_zone(), sys_tp);
	auto tp = zoned_time.get_local_time();

	auto days_tp = date::floor<date::days>(tp);
	auto ymd = date::year_month_day(days_tp);
	auto weekday = date::year_month_weekday(days_tp).weekday();

	auto secs = date::floor<std::chrono::seconds>(tp) - days_tp;
	auto tod = date::make_time(secs);
	date::days days_since_jan1 = days_tp - date::local_days(ymd.year() / 1 / 1);

	// For literals
	using namespace date;
	using namespace std::chrono_literals;

	std::tm ret{};
	ret.tm_sec = int(tod.seconds().count());
	ret.tm_min = int(tod.minutes().count());
	ret.tm_hour = int(tod.hours().count());
	ret.tm_mday = int((ymd.day() - 0_d).count());
	ret.tm_mon = int((ymd.month() - January).count());
	ret.tm_year = int((ymd.year() - 1900_y).count());
	ret.tm_wday = int((weekday - Sunday).count());
	ret.tm_yday = int(days_since_jan1.count());
	ret.tm_isdst = zoned_time.get_info().save == 0min ? 0 : 1;
	return ret;
}

template <class T, class Duration>
hh_mm_ss to_hms(std::chrono::time_point<T, Duration> tp) {
	auto day_tp = date::floor<date::days>(tp);
	auto secs = date::floor<std::chrono::seconds>(tp) - day_tp;
	return date::make_time(secs);
}

template <class T>
date::year_month_day to_ymd(date::sys_time<T> tp) {
	auto days_tp = date::floor<date::days>(tp);
	return date::year_month_day(days_tp);
}

template <class T>
date::year_month_weekday to_ymw(date::sys_time<T> tp) {
	auto days_tp = date::floor<date::days>(tp);
	return date::year_month_weekday(days_tp);
}

// WARNING : Looses precision.
inline std::chrono::system_clock::time_point to_sys(
		std::chrono::steady_clock::time_point tp) {
	using namespace std::chrono;
	return system_clock::time_point{ duration_cast<system_clock::duration>(
			tp.time_since_epoch()) };
}
template <class Duration>
date::sys_time<Duration> to_sys(steady_time<Duration> time) {
	using namespace std::chrono;
	return date::sys_time<Duration>{ time.time_since_epoch() };
}

#if FEA_CPP17
// WARNING : Looses precision.
inline std::chrono::system_clock::time_point to_sys(
		std::filesystem::file_time_type tp) {
	using namespace std::chrono;

	return time_point_cast<system_clock::duration>(
			tp - file_clock::now() + system_clock::now());
}
#endif

// WARNING : Cannot represent as big values as system_clock.
inline std::chrono::steady_clock::time_point to_steady(
		std::chrono::system_clock::time_point tp) {
	using namespace std::chrono;
	return steady_clock::time_point{ tp.time_since_epoch() };
}

template <class Duration>
steady_time<Duration> to_steady(date::sys_time<Duration> time) {
	return steady_time<Duration>{ time.time_since_epoch() };
}

inline steady_days to_steady(date::year_month_day ymd) {
	return to_steady(date::sys_days(ymd));
}

// Computes elapsed time between start and end, clamped to a modulo clock.
// For ex :
// - modulo = 60, 59s to 2s == 3s elapsed.
// - modulo = 24, 23h to 2h == 3h elapsed.
// - modulo = 365, 364d to 2d == 3d elapsed.
template <class T, class Period>
auto elapsed(std::chrono::duration<T, Period> start,
		std::chrono::duration<T, Period> end, T modulo) {
	return std::chrono::duration<T, Period>{ T(
			std::fmod((end - start).count() + modulo, modulo)) };
}

inline date::year_month_day floor_months(const date::year_month_day& ymd) {
	return { ymd.year(), ymd.month(), date::day(1) };
}

inline date::year_month_day floor_years(const date::year_month_day& ymd) {
	return { ymd.year(), date::month(1), date::day(1) };
}

inline date::days this_month_days(date::sys_days d) {
	date::year_month_day ymd{ floor_months(d) };
	date::sys_days this_month_sdays{ ymd };

	auto next_month = ymd + date::months(1);
	date::sys_days next_month_sdays{ next_month };
	return next_month_sdays - this_month_sdays;
}
inline date::days next_month_days(date::sys_days d) {
	date::year_month_day ymd{ floor_months(d) };
	ymd += date::months(1);
	date::sys_days this_month_sdays{ ymd };

	auto next_month = ymd + date::months(1);
	date::sys_days next_month_sdays{ next_month };
	return next_month_sdays - this_month_sdays;
}

inline date::days this_year_days(date::sys_days d) {
	date::year_month_day ymd{ floor_years(d) };
	date::sys_days this_year_sdays{ ymd };

	auto next_year = ymd + date::years(1);
	date::sys_days next_year_sdays{ next_year };
	return next_year_sdays - this_year_sdays;
}
inline date::days next_year_days(date::sys_days d) {
	date::year_month_day ymd{ floor_years(d) };
	ymd += date::years(1);
	date::sys_days this_year_sdays{ ymd };

	auto next_year = ymd + date::years(1);
	date::sys_days next_year_sdays{ next_year };
	return next_year_sdays - this_year_sdays;
}

template <class T>
std::string to_string(date::sys_time<T> tp) {
	auto s = date::floor<std::chrono::seconds>(tp);
	return date::format("%F %T", s);
}

template <class T>
std::string to_string_precise(date::sys_time<T> tp) {
	return date::format("%F %T", tp);
}

template <class T>
std::string to_string(steady_time<T> tp) {
	return to_string(to_sys(tp));
}

template <class T>
std::string to_string_precise(steady_time<T> tp) {
	return date::format("%T", tp.time_since_epoch());
}

template <class T, class Period>
std::string to_string(std::chrono::duration<T, Period> d) {
	return date::format("%T", d);
}

inline std::string to_string(std::tm tm_) {
	std::ostringstream oss;
	oss << std::put_time(&tm_, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

#if FEA_CPP17
namespace detail {
template <class String>
void suffixed_day(date::sys_days tp, const String*& ret) {
	using CharT = typename String::value_type;
	static const std::array<String, 32> lookup{ FEA_LIT("0th"), FEA_LIT("1st"),
		FEA_LIT("2nd"), FEA_LIT("3rd"), FEA_LIT("4th"), FEA_LIT("5th"),
		FEA_LIT("6th"), FEA_LIT("7th"), FEA_LIT("8th"), FEA_LIT("9th"),
		FEA_LIT("10th"), FEA_LIT("11th"), FEA_LIT("12th"), FEA_LIT("13th"),
		FEA_LIT("14th"), FEA_LIT("15th"), FEA_LIT("16th"), FEA_LIT("17th"),
		FEA_LIT("18th"), FEA_LIT("19th"), FEA_LIT("20th"), FEA_LIT("21st"),
		FEA_LIT("22nd"), FEA_LIT("23rd"), FEA_LIT("24th"), FEA_LIT("25th"),
		FEA_LIT("26th"), FEA_LIT("27th"), FEA_LIT("28th"), FEA_LIT("29th"),
		FEA_LIT("30th"), FEA_LIT("31st") };

	date::year_month_day ymd{ tp };
	unsigned d = unsigned(ymd.day());
	assert(d < 32u);
	ret = &lookup[d];
}
} // namespace detail

// Given a date, returns the month's day with suffix.
// Ex, 1st, 2nd, 3rd, etc.
// Return type overload magic. Supports all std strings (other than u8string).
inline auto suffixed_day(date::sys_days tp) {
	return fea::str_cr_return_overload{ [tp](const auto*& ret) {
		detail::suffixed_day(tp, ret);
	} };
}
#endif
} // namespace fea
