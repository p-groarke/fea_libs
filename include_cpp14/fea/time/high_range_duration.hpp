/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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
#include "fea/time/time.hpp"

/*
A duration that can represent a high range of time values.

Accumulates time using days, seconds and provided remainder duration. Allows to
accumulate a multiple year duration with nanosecond precision, for example.

You may extract precise time using days, seconds and remainder functions.
Or choose to loose precision, using total_days, total_seconds and
total_remainder functions. Unprecise time is returned using double time, to give
it a chance.
*/

namespace fea {
// Splits duration into days, seconds and remainder.
// Allows for very large values (years+) while retaining nanosecond precision
// for example.
template <class Rep, class Period>
struct high_range_duration {
	using days_t = size_t_duration<date::days>;
	using seconds_t = size_t_duration<std::chrono::seconds>;
	using remainder_t = std::chrono::duration<size_t, Period>;
	using dremainder_t = std::chrono::duration<double, Period>;

	static_assert(!std::is_same<days_t, remainder_t>::value,
			"fea::high_range_duration : using days as precise time doesn't "
			"make sense");
	static_assert(!std::is_same<seconds_t, remainder_t>::value,
			"fea::high_range_duration : using seconds as precise time doesn't "
			"make sense");

	constexpr high_range_duration(
			std::chrono::duration<Rep, Period> most_precise_dur)
			: _days(date::floor<days_t>(most_precise_dur))
			, _seconds(date::floor<seconds_t>(most_precise_dur) - _days)
			, _remainder(most_precise_dur - _seconds - _days) {
	}
	constexpr high_range_duration(seconds_t secs)
			: _days(date::floor<days_t>(secs))
			, _seconds(date::floor<seconds_t>(secs) - _days)
			, _remainder(0) {
	}
	constexpr high_range_duration(days_t d)
			: _days(date::floor<days_t>(d))
			, _seconds(0)
			, _remainder(0) {
	}

	constexpr high_range_duration(dseconds secs)
			: _days(date::floor<days_t>(secs))
			, _seconds(date::floor<seconds_t>(secs) - _days)
			, _remainder(date::floor<remainder_t>(secs) - _seconds - _days) {
	}

	// Imprecise and can overflow.
	// Looses most precision in remainder.
	constexpr ddays total_days() const {
		return ddays(_days) + ddays(_seconds) + ddays(_remainder);
	}
	// Imprecise and can overflow.
	// Looses precision in days and remainder.
	constexpr dseconds total_seconds() const {
		return dseconds(_days) + dseconds(_seconds) + dseconds(_remainder);
	}
	// Imprecise and can overflow.
	// Looses most precision in days.
	constexpr dremainder_t total_remainder() const {
		return dremainder_t(_days) + dremainder_t(_seconds)
				+ dremainder_t(_remainder);
	}

	// Return stored days.
	constexpr days_t days() const {
		return _days;
	}
	// Return stored seconds.
	constexpr seconds_t seconds() const {
		return _seconds;
	}
	// Return stored remainder.
	constexpr remainder_t remainder() const {
		return _remainder;
	}

	high_range_duration& operator+=(const high_range_duration& rhs) {
		// First add everything.
		_days += rhs._days;
		_seconds += rhs._seconds;
		_remainder += rhs._remainder;

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

private:
	// Decomposes time into remainder, seconds and days.
	void carry_over() {
		seconds_t s_leftover = date::floor<seconds_t>(_remainder);
		_remainder -= s_leftover;
		_seconds += s_leftover;

		days_t d_leftover = date::floor<days_t>(_seconds);
		_seconds -= d_leftover;
		_days += d_leftover;
	}

	days_t _days;
	seconds_t _seconds;
	remainder_t _remainder;
};
} // namespace fea
