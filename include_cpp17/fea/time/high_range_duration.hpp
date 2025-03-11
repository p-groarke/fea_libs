/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
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

#if FEA_WITH_DATE
#include "fea/time/time.hpp"

/*
A duration that can represent a high range of time values.

Accumulates time using days, seconds and nanoseconds duration. Allows to
accumulate a multiple year duration with nanosecond precision, for example.

You may extract precise time using days, seconds and remainder functions.
Or choose to loose precision, using total_days, total_seconds and
total_nanoseconds functions. Unprecise time is returned using double time, to
give it a chance.

high_range_duration stores time as unsigned, careful when substracting.
Substractions do saturate to 0.

TODO : high_range_time_point
*/

namespace fea {
// Splits duration into days, seconds and remainder.
// Allows for very large values (years+) while retaining nanosecond precision
// for example.
struct high_range_duration {
	using rep = size_t;
	using period = typename unanoseconds::period;
	using dnanoseconds_t
			= std::chrono::duration<double, std::chrono::nanoseconds::period>;

	constexpr high_range_duration() = default;

	constexpr high_range_duration(udays d)
			: _days(date::floor<udays>(d))
			, _seconds(0)
			, _nanoseconds(0) {
	}
	constexpr high_range_duration(useconds secs)
			: _days(date::floor<udays>(secs))
			, _seconds(secs - _days)
			, _nanoseconds(0) {
	}
	constexpr high_range_duration(unanoseconds nano)
			: _days(date::floor<udays>(nano))
			, _seconds(date::floor<useconds>(nano - _days))
			, _nanoseconds(nano - _seconds - _days) {
	}

	constexpr high_range_duration(ddays d)
			: _days(date::floor<udays>(d))
			, _seconds(date::floor<useconds>(d - _days))
			, _nanoseconds(date::floor<unanoseconds>(d - _seconds - _days)) {
	}
	constexpr high_range_duration(dseconds secs)
			: _days(date::floor<udays>(secs))
			, _seconds(date::floor<useconds>(secs - _days))
			, _nanoseconds(date::floor<unanoseconds>(secs - _seconds - _days)) {
	}

	constexpr high_range_duration(date::year_month_day ymd)
			: _days(date::sys_days(ymd).time_since_epoch())
			, _seconds(0)
			, _nanoseconds(0) {
	}

	constexpr high_range_duration(udays d, useconds secs, unanoseconds nano)
			: _days(d)
			, _seconds(secs)
			, _nanoseconds(nano) {
	}

	// Imprecise and can overflow.
	// Looses most precision in nanoseconds.
	constexpr ddays count_days() const {
		return ddays(_days) + ddays(_seconds) + ddays(_nanoseconds);
	}
	// Imprecise and can overflow.
	// Looses precision in days and nanoseconds.
	constexpr dseconds count() const {
		return dseconds(_days) + dseconds(_seconds) + dseconds(_nanoseconds);
	}
	// Imprecise and can overflow.
	// Looses most precision in days.
	constexpr dnanoseconds_t count_nanoseconds() const {
		return dnanoseconds_t(_days) + dnanoseconds_t(_seconds)
			 + dnanoseconds_t(_nanoseconds);
	}

	// Return stored days.
	constexpr udays days() const noexcept {
		return _days;
	}
	// Return stored seconds.
	constexpr useconds seconds() const noexcept {
		return _seconds;
	}
	// Return stored remainder.
	constexpr unanoseconds nanoseconds() const noexcept {
		return _nanoseconds;
	}


	/**
	 * Operators
	 */

	high_range_duration& operator+=(const high_range_duration& rhs) {
		// First add everything.
		_days += rhs._days;
		_seconds += rhs._seconds;
		_nanoseconds += rhs._nanoseconds;

		// Then carry over seconds and days.
		carry_over();
		return *this;
	}
	// Hidden friends :
	// https://www.justsoftwaresolutions.co.uk/cplusplus/hidden-friends.html
	friend high_range_duration operator+(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		high_range_duration ret{ lhs };
		ret += rhs;
		return ret;
	}

	high_range_duration& operator+=(const udays& rhs) {
		_days += rhs;
		return *this;
	}
	friend high_range_duration operator+(
			const high_range_duration& lhs, const udays& rhs) {
		high_range_duration ret{ lhs };
		ret += rhs;
		return ret;
	}

	high_range_duration& operator+=(const useconds& rhs) {
		_seconds += rhs;
		carry_over();
		return *this;
	}
	friend high_range_duration operator+(
			const high_range_duration& lhs, const useconds& rhs) {
		high_range_duration ret{ lhs };
		ret += rhs;
		return ret;
	}

	high_range_duration& operator+=(const unanoseconds& rhs) {
		_nanoseconds += rhs;
		carry_over();
		return *this;
	}
	friend high_range_duration operator+(
			const high_range_duration& lhs, const unanoseconds& rhs) {
		high_range_duration ret{ lhs };
		ret += rhs;
		return ret;
	}

	// TODO : unit tests
	// Will saturate to 0 if rhs > lhs (high_range_duration is unsigned).
	high_range_duration& operator-=(const high_range_duration& rhs) {
		if (_days < rhs._days) {
			_days = udays{ 0 };
			_seconds = useconds{ 0 };
			_nanoseconds = unanoseconds{ 0 };
			return *this;
		} else {
			_days -= rhs._days;
		}

		if (_seconds < rhs._seconds) {
			_seconds = useconds{ 0 };
			_nanoseconds = unanoseconds{ 0 };
			return *this;
		} else {
			_seconds -= rhs._seconds;
		}

		if (_nanoseconds < rhs._nanoseconds) {
			_nanoseconds = unanoseconds{ 0 };
		} else {
			_nanoseconds -= rhs._nanoseconds;
		}

		return *this;
	}
	friend high_range_duration operator-(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		high_range_duration ret{ lhs };
		ret -= rhs;
		return ret;
	}

	friend bool operator==(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		return lhs._days == rhs._days && lhs._seconds == rhs._seconds
			&& lhs._nanoseconds == rhs._nanoseconds;
	}
	friend bool operator!=(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		return !(lhs == rhs);
	}
	friend bool operator<(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		if (lhs._days < rhs._days) {
			return true;
		} else if (lhs._days > rhs._days) {
			return false;
		}

		if (lhs._seconds < rhs._seconds) {
			return true;
		} else if (lhs._seconds > rhs._seconds) {
			return false;
		}

		return lhs._nanoseconds < rhs._nanoseconds;
	}
	friend bool operator>(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		return rhs < lhs;
	}
	friend bool operator<=(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		return !(rhs < lhs);
	}
	friend bool operator>=(
			const high_range_duration& lhs, const high_range_duration& rhs) {
		return !(lhs < rhs);
	}

	/**
	 * Standard functions
	 */

	// Returns "zero" value (aka default constructed).
	static constexpr high_range_duration zero() noexcept {
		return {};
	}

	// Returns minimum possible value (equal to zero, since high_range_duration
	// is unsigned).
	static constexpr high_range_duration(min)() noexcept {
		return zero();
	}

	// Returns maximum possible value.
	static constexpr high_range_duration(max)() noexcept {
		return high_range_duration{
			udays{ (std::numeric_limits<size_t>::max)() },
			useconds{ (std::numeric_limits<size_t>::max)() },
			unanoseconds{ (std::numeric_limits<size_t>::max)() },
		};
	}

	template <class FloorDuration>
	friend constexpr high_range_duration floor(
			const high_range_duration& rhs) noexcept;

private:
	// Decomposes time into remainder, seconds and days.
	void carry_over() {
		useconds s_leftover = date::floor<useconds>(_nanoseconds);
		_nanoseconds -= unanoseconds{ s_leftover };
		_seconds += s_leftover;

		udays d_leftover = date::floor<udays>(_seconds);
		_seconds -= useconds{ d_leftover };
		_days += d_leftover;
	}

	udays _days{ 0 };
	useconds _seconds{ 0 };
	unanoseconds _nanoseconds{ 0 };
};

template <class FloorDuration>
constexpr high_range_duration floor(const high_range_duration& rhs) noexcept {
	using namespace std::chrono;
	high_range_duration ret;
	ret._days = duration_cast<udays>((date::floor<FloorDuration>(rhs._days)));

	ret._seconds
			= duration_cast<useconds>(date::floor<FloorDuration>(rhs._seconds));

	ret._nanoseconds = duration_cast<unanoseconds>(
			date::floor<FloorDuration>(rhs._nanoseconds));

	return ret;
}
} // namespace fea
#endif
