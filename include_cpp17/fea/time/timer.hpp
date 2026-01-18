/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
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
#include "fea/utility/platform.hpp"

#if FEA_WITH_DATE
#include "fea/events/event_stack.hpp"
#include "fea/performance/constants.hpp"
#include "fea/state_machines/fsm.hpp"
#include "fea/time/high_range_duration.hpp"
#include "fea/time/time.hpp"
#include "fea/utility/error.hpp"

#include <cassert>

#if FEA_WITH_TBB
#if FEA_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4459)
#include <tbb/parallel_for.h>
#pragma warning(pop)
#else
#include <tbb/parallel_for.h>
#endif
#endif

/*
A timer and calendar.

By default, uses steady_clock which provides precise user independent time.

Create by providing your callback signature in the template. For ex,
fea::timer<void(int)> t;

A multi-threaded version is available. Its callbacks will be called on multiple
threads. Create with timer_mt.
fea::timer_mt<void(int)> t;

To create a system_clock timer, use sys_timer.
fea::sys_timer<void(int)> t;

To create a multi-threaded system_clock timer, use sys_timer_mt.
fea::sys_timer_mt<void(int)> t;

Steady aliases 'fea::steady_timer' and 'fea::steady_timer_mt' are provided for
consistency.


Behavior
Starts at provided time (or now() if not provided) and increases according to
provided time_ratio (or 1:1 if not provided).

Note that the default starting time (steady_clock::now()) has no relation with
actual "real life" time when in steady mode.

The time ratio is how fast a real second goes by in the virtual timer time.
For example, setting a ratio of 1 minute means: 1 true second == 1 virtual
minute.


Callbacks
You can subscribe to:
	- Time events (ex. callback every second, minute, hour, etc).
	- Pause/UnPause events.
	- Elapsed callbacks (ex. callback after x seconds).
	- Specific time callbacks (ex. callback at 01/01/2021 3:30pm).


Imprecision Behavior
If your ratio is too high, and you've subscribed to time event callbacks that
cannot keep up, the timer will try and catch up and invoke callbacks once per
update.
For ex, the timer will *not* call the seconds callback 5 times if 5 seconds has
elapsed since last update. It will call it once.

Months & Years
When using the timer as a calendar timer, months and leap years are computed
according to the gregorian calendar. When using these features, you probably
want to instantiate a sys_timer, or set a start date when using steady_timer.
*/

namespace fea {
enum class timer_event : uint8_t {
	// Ticks every X.
	seconds,
	minutes,
	hours,
	days,
	weeks,
	months,
	years,
	// Subscribe to pause/unpause events.
	on_pause,
	on_unpause,
	// Required by fsm.
	count,
};

template <class, class = std::chrono::steady_clock, bool = false>
struct timer;

template <class Ret, class... EventArgs, class Clock, bool MultiThreaded>
struct timer<Ret(EventArgs...), Clock, MultiThreaded> {
	static_assert(
			std::is_same_v<Ret, void>, "timer : callbacks must return void");

private:
	enum class state : uint8_t {
		updating,
		paused,
		count,
	};
	enum class transition : uint8_t {
		do_unpause,
		do_pause,
		count,
	};
	using fsm_t = fea::fsm<transition, state, void(timer*, EventArgs...)>;
	using fsm_state_t
			= fea::fsm_state<transition, state, void(timer*, EventArgs...)>;

public:
	using event_stack_t = event_stack<timer_event,
			/*seconds*/ void(EventArgs...),
			/*minutes*/ void(EventArgs...),
			/*hours*/ void(EventArgs...),
			/*days*/ void(EventArgs...),
			/*weeks*/ void(EventArgs...),
			/*months*/ void(EventArgs...),
			/*years*/ void(EventArgs...),
			/*on_pause*/ void(EventArgs...),
			/*on_unpause*/ void(EventArgs...)>;

	using clock_duration_t = typename Clock::duration;
	using uclock_duration_t = size_t_duration<clock_duration_t>;
	using time_point_t = typename Clock::time_point;

	// Create a timer starting at start_time and increasing at time_ratio rate.
	timer(high_range_duration start_time, dseconds time_ratio = dseconds(1.0))
			: _counter(uclock_duration_t{ 0 })
			, _ratio(time_ratio)
			, _start_time(start_time)
			, _new_update_time(Clock::now()) {

		high_range_duration current_time = time_precise();

		_last_second_tick = floor<useconds>(current_time);
		_last_minute_tick = floor<uminutes>(current_time);
		_last_hour_tick = floor<uhours>(current_time);
		_last_day_tick = floor<udays>(current_time);
		_last_week_tick = floor<uweeks>(current_time);
		_last_month_tick = floor<umonths>(current_time);
		_last_year_tick = floor<uyears>(current_time);

		{
			fsm_state_t updating_state;
			updating_state.template add_event<fea::fsm_event::on_enter>(
					&timer::on_updating_enter);
			updating_state.template add_event<fea::fsm_event::on_update>(
					&timer::on_update);
			updating_state.template add_transition<transition::do_pause,
					state::paused>();
			_smachine.template add_state<state::updating>(
					std::move(updating_state));
		}

		{
			fsm_state_t paused_state;
			paused_state.template add_event<fea::fsm_event::on_enter>(
					&timer::on_pause_enter);
			// paused_state.add_event<fea::fsm_event::on_exit>(
			//		&timer::on_pause_exit);
			paused_state.template add_transition<transition::do_unpause,
					state::updating>();
			_smachine.template add_state<state::paused>(
					std::move(paused_state));
		}
	}
	explicit timer(dclock_seconds<Clock> start_time,
			dseconds time_ratio = dseconds(1.0))
			: timer(start_time.time_since_epoch(), time_ratio) {
	}

	// Create a timer at date ymd, increasing by time_ratio speed.
	explicit timer(
			date::year_month_day ymd, dseconds time_ratio = dseconds(1.0))
			: timer(high_range_duration(ymd), time_ratio) {
	}

	// Create a timer increasing by time_ratio speed.
	explicit timer(dseconds time_ratio)
			: timer(high_range_duration(dseconds(0.0)), time_ratio) {
	}

	// Create a timer starting at 0, aka Jan 01 1970.
	timer()
			: timer(high_range_duration(dseconds(0.0))) {
	}

	// Update the timer. Call this continually. Pass in callback signature
	// arguments.
	void update(EventArgs... event_args) {
		return _smachine.update(this, event_args...);
	}

	// Pause the timer. Pass in callback signature arguments.
	void pause(EventArgs... event_args) {
		_smachine.template trigger<transition::do_pause>(this, event_args...);
	}

	// Pause the timer. Pass in callback signature arguments.
	void unpause(EventArgs... event_args) {
		_smachine.template trigger<transition::do_unpause>(this, event_args...);
	}

	// Print elapsed time and timer time to console.
	void print() const {
		std::string date_s = date::format("%T", elapsed());
		printf("elapsed : %s\n", date_s.c_str());
		date_s = date::format("%F %T", time());
		printf("time : %s\n", date_s.c_str());
	}

	// Elapsed days since start of timer.
	// Imprecise.
	dseconds elapsed_days() const {
		return _counter.count_days();
	}
	// Elapsed seconds since start of timer.
	// Imprecise.
	dseconds elapsed() const {
		return _counter.count();
	}

	// Precise elapsed construct.
	const high_range_duration& elapsed_precise() const {
		return _counter;
	}

	// Current time of timer (taking start_time into consideration).
	// Imprecise.
	dclock_seconds<Clock> time() const {
		high_range_duration t = _start_time + _counter;
		return dclock_seconds<Clock>{ t.count() };
	}

	// Current time of timer (taking start_time into consideration).
	// Precise construct.
	high_range_duration time_precise() const {
		return _start_time + _counter;
	}

	// Get timer start_time().
	const high_range_duration& start_time() const {
		return _start_time;
	}
	// Change timer start_time.
	high_range_duration& start_time() {
		return _start_time;
	}

	// Get current ratio.
	dseconds ratio() const {
		return _ratio;
	}
	// Change time speed ratio.
	dseconds& ratio() {
		return _ratio;
	}

	// Access continuous events stack.
	// For ex, seconds, minutes, hours, days, etc.
	const event_stack_t& events() const {
		return _event_stack;
	}
	// Access continuous events to subscribe to tick events.
	// For ex, seconds, minutes, hours, days, etc.
	// See 'event_stack' header for help on the event_stack api.
	event_stack_t& events() {
		return _event_stack;
	}

	// Will execute your callback after e has elapsed. Local time.
	// TODO : Add subscribe with high_range_duration.
	void subscribe_elapsed(
			dseconds e, const std::function<void(EventArgs...)>& func) {
		assert(e > elapsed()); // asserts nice when threading.
		if (e <= elapsed()) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"subscribing callback that will never be called");
		}

		_elapsed_callbacks.push_back({ e, func });
	}

	// Will execute your callback at time t. Absolute time.
	// TODO : Add subscribe with high_range_duration.
	void subscribe_time(dclock_seconds<Clock> t,
			const std::function<void(EventArgs...)>& func) {
		assert(t > time());
		if (t <= time()) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"subscribing callback that will never be called");
		}

		// TODO : high_range_timepoint
		_time_callbacks.push_back({ t.time_since_epoch(), func });
	}

private:
	void on_pause_enter(EventArgs... event_args, fsm_t&) {
		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::on_pause>(
					event_args...);
		} else {
			_event_stack.template trigger<timer_event::on_pause>(event_args...);
		}
	}

	void on_pause_exit(EventArgs..., fsm_t&) {
	}

	void on_updating_enter(EventArgs... event_args, fsm_t&) {
		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::on_unpause>(
					event_args...);
		} else {
			_event_stack.template trigger<timer_event::on_unpause>(
					event_args...);
		}
	}

	void on_update(EventArgs... event_args, fsm_t&) {
		using namespace date::literals;
		using namespace std::chrono_literals;

		// Increment counter.
		{
			auto _last_update_time = _new_update_time;
			_new_update_time = Clock::now();
			dseconds dt = _new_update_time - _last_update_time;
			dt *= _ratio.count();
			high_range_duration hr_dt{ dt };
			_counter += hr_dt;
		}

		high_range_duration current_time = time_precise();

		// seconds
		if (_last_second_tick + useconds(1) > current_time) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::seconds>(
					event_args...);
		} else {
			_event_stack.template trigger<timer_event::seconds>(event_args...);
		}
		_last_second_tick = floor<useconds>(current_time);

		// minutes
		if (_last_minute_tick + useconds(1min) > current_time) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::minutes>(
					event_args...);
		} else {
			_event_stack.template trigger<timer_event::minutes>(event_args...);
		}
		_last_minute_tick = floor<uminutes>(current_time);

		// hours
		if (_last_hour_tick + useconds(1h) > current_time) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::hours>(event_args...);
		} else {
			_event_stack.template trigger<timer_event::hours>(event_args...);
		}
		_last_hour_tick = floor<uhours>(current_time);

		// days
		if (_last_day_tick + udays(1) > current_time) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::days>(event_args...);
		} else {
			_event_stack.template trigger<timer_event::days>(event_args...);
		}
		_last_day_tick = floor<udays>(current_time);

		// weeks, don't block months and years (weeks are desynchronized).
		if (_last_week_tick + udays(uweeks(1)) <= current_time) {
			if constexpr (MultiThreaded) {
				_event_stack.template trigger_mt<timer_event::weeks>(
						event_args...);
			} else {
				_event_stack.template trigger<timer_event::weeks>(
						event_args...);
			}
			_last_week_tick = floor<uweeks>(current_time);
		}

		// months
		// Find the number of days that need to have elapsed before we are at
		// next month (according to gregorian calendar).
		udays jump_month = _last_month_tick.days();
		jump_month += fea::this_month_days(date::sys_days(jump_month));

		if (jump_month > current_time.days()) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::months>(
					event_args...);
		} else {
			_event_stack.template trigger<timer_event::months>(event_args...);
		}
		_last_month_tick = floor_months(date::sys_days{ current_time.days() });

		assert(unsigned(date::year_month_day(
					   date::sys_days(_last_month_tick.days()))
							   .day())
				== 1u);

		// years
		// Find the number of days that need to have elapsed before we are at
		// next year (according to gregorian calendar).
		udays jump_year = _last_year_tick.days();
		jump_year += fea::this_year_days(date::sys_days{ jump_year });

		if (jump_year > current_time.days()) {
			return update_callbacks(event_args...);
		}

		if constexpr (MultiThreaded) {
			_event_stack.template trigger_mt<timer_event::years>(event_args...);
		} else {
			_event_stack.template trigger<timer_event::years>(event_args...);
		}
		_last_year_tick = floor_years(date::sys_days{ current_time.days() });

		assert(unsigned(date::year_month_day(
					   date::sys_days(_last_year_tick.days()))
							   .day())
				== 1u);
		assert(unsigned(date::year_month_day(
					   date::sys_days(_last_year_tick.days()))
							   .month())
				== 1u);

		update_callbacks(event_args...);
	}

	void update_callbacks(EventArgs... event_args) {
		high_range_duration current_time = time_precise();
		high_range_duration current_elapsed_time = elapsed_precise();

		// Gather events that need to be executed and removed.
		auto new_end = std::remove_if(_elapsed_callbacks.begin(),
				_elapsed_callbacks.end(), [&](auto& callback_p) {
					if (callback_p.first <= current_elapsed_time) {
						return true;
					}
					return false;
				});

		if constexpr (MultiThreaded && FEA_WITH_TBB) {
#if FEA_WITH_TBB
			size_t start_idx = new_end - _elapsed_callbacks.begin();
			auto eval = [&, this](const tbb::blocked_range<size_t>& range) {
				for (size_t i = range.begin(); i < range.end(); ++i) {
					_elapsed_callbacks[i].second(event_args...);
				}
			};
			tbb::blocked_range<size_t> range{
				start_idx,
				_elapsed_callbacks.size(),
				fea::default_grainsize_small_v<true>,
			};
			tbb::parallel_for(range, eval, fea::default_partitioner_t<true>{});
#endif
		} else {
			for (auto it = new_end; it != _elapsed_callbacks.end(); ++it) {
				it->second(event_args...);
			}
		}

		_elapsed_callbacks.erase(new_end, _elapsed_callbacks.end());

		auto new_end2 = std::remove_if(_time_callbacks.begin(),
				_time_callbacks.end(), [&](auto& callback_p) {
					if (callback_p.first <= current_time) {
						return true;
					}
					return false;
				});

		if constexpr (MultiThreaded && FEA_WITH_TBB) {
#if FEA_WITH_TBB
			size_t start_idx = new_end2 - _time_callbacks.begin();
			auto eval = [&, this](const tbb::blocked_range<size_t>& range) {
				for (size_t i = range.begin(); i < range.end(); ++i) {
					_time_callbacks[i].second(event_args...);
				}
			};
			tbb::blocked_range<size_t> range{
				start_idx,
				_time_callbacks.size(),
				fea::default_grainsize_small_v<true>,
			};
			tbb::parallel_for(range, eval, fea::default_partitioner_t<true>{});
#endif
		} else {
			for (auto it = new_end2; it != _time_callbacks.end(); ++it) {
				it->second(event_args...);
			}
		}

		_time_callbacks.erase(new_end2, _time_callbacks.end());
	}

	// Elapsed time.
	high_range_duration _counter;

	// Time increment speed. Aka, 1 second : x seconds.
	dseconds _ratio;

	// User provided start time.
	// TODO : high_range_timepoint
	high_range_duration _start_time;

	// The timepoint of this frame, used to compute time difference between
	// updates.
	time_point_t _new_update_time;

	// These are timestamps of the last triggered time events.
	high_range_duration _last_second_tick;
	high_range_duration _last_minute_tick;
	high_range_duration _last_hour_tick;
	high_range_duration _last_day_tick;
	high_range_duration _last_week_tick;
	high_range_duration _last_month_tick;
	high_range_duration _last_year_tick;

	// The time event system.
	event_stack_t _event_stack;

	// The pause/unpause statemachine.
	fsm_t _smachine;

	// The elapsed and timepoint callbacks.
	std::vector<
			std::pair<high_range_duration, std::function<void(EventArgs...)>>>
			_elapsed_callbacks;
	std::vector<
			std::pair<high_range_duration, std::function<void(EventArgs...)>>>
			_time_callbacks;
};

// Multithreaded version of steady_clock timer.
// Multiple callbacks are called in seperate threads.
template <class EventSignature>
using timer_mt = timer<EventSignature, std::chrono::steady_clock, true>;

// system_clock timer.
template <class EventSignature>
using sys_timer = timer<EventSignature, std::chrono::system_clock, false>;

// Multithreaded version of system_clock timer.
// Multiple callbacks are called in seperate threads.
template <class EventSignature>
using sys_timer_mt = timer<EventSignature, std::chrono::system_clock, true>;

// For consistency.
template <class EventSignature>
using steady_timer = timer<EventSignature, std::chrono::steady_clock, false>;
// For consistency.
template <class EventSignature>
using steady_timer_mt = timer<EventSignature, std::chrono::steady_clock, true>;

} // namespace fea
#endif // FEA_WITH_DATE
