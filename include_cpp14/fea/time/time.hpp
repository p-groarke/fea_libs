#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// https://github.com/HowardHinnant/date/issues/474
#define HAS_UNCAUGHT_EXCEPTIONS 1
#include <date/date.h>
#include <date/tz.h>
#undef HAS_UNCAUGHT_EXCEPTIONS

namespace fea {
// Extra helpers.
template <class Duration>
using steady_time
		= std::chrono::time_point<std::chrono::steady_clock, Duration>;

using steady_seconds = typename steady_time<std::chrono::seconds>;
using steady_minutes = typename steady_time<std::chrono::minutes>;
using steady_hours = typename steady_time<std::chrono::hours>;
using steady_days = typename steady_time<date::days>;
using steady_weeks = typename steady_time<date::weeks>;
using steady_months = typename steady_time<date::months>;
using steady_years = typename steady_time<date::years>;

template <class Clock>
using clock_seconds =
		typename std::chrono::time_point<Clock, std::chrono::seconds>;
template <class Clock>
using clock_minutes =
		typename std::chrono::time_point<Clock, std::chrono::minutes>;
template <class Clock>
using clock_hours = typename std::chrono::time_point<Clock, std::chrono::hours>;
template <class Clock>
using clock_days = typename std::chrono::time_point<Clock, date::days>;
template <class Clock>
using clock_weeks = typename std::chrono::time_point<Clock, date::weeks>;
template <class Clock>
using clock_months = typename std::chrono::time_point<Clock, date::months>;
template <class Clock>
using clock_years = typename std::chrono::time_point<Clock, date::years>;


// floats

using fseconds = std::chrono::duration<float>;
using fminutes = std::chrono::duration<float, std::chrono::minutes::period>;
using fhours = std::chrono::duration<float, std::chrono::hours::period>;
using fdays = std::chrono::duration<float, date::days::period>;
using fweeks = std::chrono::duration<float, date::weeks::period>;
using fmonths = std::chrono::duration<float, date::months::period>;
using fyears = std::chrono::duration<float, date::years::period>;

using fsys_seconds = typename date::sys_time<fseconds>;
using fsys_minutes = typename date::sys_time<fminutes>;
using fsys_hours = typename date::sys_time<fhours>;
using fsys_days = typename date::sys_time<fdays>;
using fsys_weeks = typename date::sys_time<fweeks>;
using fsys_months = typename date::sys_time<fmonths>;
using fsys_years = typename date::sys_time<fyears>;

using fsteady_seconds = typename steady_time<fseconds>;
using fsteady_minutes = typename steady_time<fminutes>;
using fsteady_hours = typename steady_time<fhours>;
using fsteady_days = typename steady_time<fdays>;
using fsteady_weeks = typename steady_time<fweeks>;
using fsteady_months = typename steady_time<fmonths>;
using fsteady_years = typename steady_time<fyears>;

template <class Clock>
using fclock_seconds = typename std::chrono::time_point<Clock, fseconds>;
template <class Clock>
using fclock_minutes = typename std::chrono::time_point<Clock, fminutes>;
template <class Clock>
using fclock_hours = typename std::chrono::time_point<Clock, fhours>;
template <class Clock>
using fclock_days = typename std::chrono::time_point<Clock, fdays>;
template <class Clock>
using fclock_weeks = typename std::chrono::time_point<Clock, fweeks>;
template <class Clock>
using fclock_months = typename std::chrono::time_point<Clock, fmonths>;
template <class Clock>
using fclock_years = typename std::chrono::time_point<Clock, fyears>;

// doubles

using dseconds = std::chrono::duration<double>;
using dminutes = std::chrono::duration<double, std::chrono::minutes::period>;
using dhours = std::chrono::duration<double, std::chrono::hours::period>;
using ddays = std::chrono::duration<double, date::days::period>;
using dweeks = std::chrono::duration<double, date::weeks::period>;
using dmonths = std::chrono::duration<double, date::months::period>;
using dyears = std::chrono::duration<double, date::years::period>;

using dsys_seconds = typename date::sys_time<dseconds>;
using dsys_minutes = typename date::sys_time<dminutes>;
using dsys_hours = typename date::sys_time<dhours>;
using dsys_days = typename date::sys_time<ddays>;
using dsys_weeks = typename date::sys_time<dweeks>;
using dsys_months = typename date::sys_time<dmonths>;
using dsys_years = typename date::sys_time<dyears>;

using dsteady_seconds = typename steady_time<dseconds>;
using dsteady_minutes = typename steady_time<dminutes>;
using dsteady_hours = typename steady_time<dhours>;
using dsteady_days = typename steady_time<ddays>;
using dsteady_weeks = typename steady_time<dweeks>;
using dsteady_months = typename steady_time<dmonths>;
using dsteady_years = typename steady_time<dyears>;

template <class Clock>
using dclock_seconds = typename std::chrono::time_point<Clock, dseconds>;
template <class Clock>
using dclock_minutes = typename std::chrono::time_point<Clock, dminutes>;
template <class Clock>
using dclock_hours = typename std::chrono::time_point<Clock, dhours>;
template <class Clock>
using dclock_days = typename std::chrono::time_point<Clock, ddays>;
template <class Clock>
using dclock_weeks = typename std::chrono::time_point<Clock, dweeks>;
template <class Clock>
using dclock_months = typename std::chrono::time_point<Clock, dmonths>;
template <class Clock>
using dclock_years = typename std::chrono::time_point<Clock, dyears>;

// Other useful types.
using hh_mm_ss = date::hh_mm_ss<std::chrono::seconds>;
using fhh_mm_ss = date::hh_mm_ss<fseconds>;
using dhh_mm_ss = date::hh_mm_ss<dseconds>;


// Helper functions

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
template <template <class, class> class Duration = std::chrono::duration,
		class T, class Period>
auto elapsed(Duration<T, Period> start, Duration<T, Period> end, T modulo) {
	return Duration<T, Period>{ T(
			std::fmod((end - start).count() + modulo, modulo)) };
}

template <class T>
std::string to_string(date::sys_time<T> tp) {
	auto s = std::chrono::floor<std::chrono::seconds>(tp);
	return date::format("%F %T", s);
}

template <class T>
std::string to_string_precise(date::sys_time<T> tp) {
	return date::format("%F %T", tp);
}

template <class T>
std::string to_string(steady_time<T> tp) {
	using namespace std::chrono;

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

} // namespace fea
