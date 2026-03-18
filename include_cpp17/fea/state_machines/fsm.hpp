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
#include "fea/utility/error.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>

namespace fea {
/*
A small, fast and simple stack based fsm.
	Not as small as inlined simple fsm, but easier to work with and debug.

Features :
	- OnEnter, OnUpdate, OnExit.
	- OnEnterFrom, OnExitTo.
		Overrides event behavior when coming from/going to specified states or
		transitions.
	- Supports user arguments in the callbacks (explained below).

Callbacks :
	- The last argument of your callback is always a ref to the fsm itself.
		This is useful for retriggering and when you store fsms in containers.
		You can use auto& to simplify your callback signature.
		[](your_args..., auto& mymachine){}

	- Pass your event signature at the end of the fsm and fsm_state template.
		These will be passed on to your callbacks when you call update or
		trigger.
		For example :
			fsm<myts, mystates, void(int, bool&, const void*)>;
		Callback signature is:
			[](int, bool&, const void*, auto& machine){}
	- The state machine is passed at the end to allow you to call member
		functions directly.
		For example :
			fsm<my_tses, my_states, bool(my_obj*, int)>
		Call your triggers passing the object pointer as the first argument, and
		the member functions will be called.
			machine.trigger<some::transition>(&obj, 42);


Notes :
	- Uses std::function. If you can't have that, use inlined fsms instead.
	- Throws on unhandled transition.
		You must explicitly add re-entrant transitions or ignored transitions
		(by providing empty callbacks). IMHO this is one of the bigest source of
		bugs and broken behavior when working with FSMs. Throwing makes
		debugging much faster and easier.
	- Define FEA_FSM_NOTHROW to assert instead of throw.
	- Does NOT provide a "get_current_state" function.
		Checking the current state of an fsm is a major smell and usually points
		to either a misuse, misunderstanding or incomplete implementation of the
		fsm. Do not do that, rethink your states and transitions instead.

TODO :
	- Yield/History state
	- Auto transition guards (anonymous / completion event)?
		- 1 runtime transition per state
	- Guard transitions maybe?
	//- DelayedTrigger. // Removed till bug fix
	//	Trigger will happen next time you call fsm::update.

https://statecharts.dev/glossary/
https://live.boost.org/doc/libs/1_83_0/libs/msm/doc/HTML/ch02s02.html
*/

enum class fsm_event : uint8_t {
	on_enter_from,
	on_enter,
	on_update,
	on_exit,
	on_exit_to,
	count,
};

template <class, class, class>
struct fsm;

template <class, class, class>
struct fsm_state;

template <class TransitionEnum, class StateEnum, class FuncRet,
		class... FuncArgs>
struct fsm_state<TransitionEnum, StateEnum, FuncRet(FuncArgs...)> {
	using fsm_t = fsm<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>;
	using fsm_func_t = std::function<FuncRet(FuncArgs..., fsm_t&)>;

	using underlying_state_t = std::underlying_type_t<StateEnum>;
	static constexpr StateEnum ignore_sentinel
			= StateEnum((std::numeric_limits<underlying_state_t>::max)());

	fsm_state();

	// Add your event implementation (on_enter, on_update, on_exit).
	template <fsm_event Event>
	void add_event(fsm_func_t&& func);

	// Add your event implementation (on_enter, on_update, on_exit).
	// Non-template version.
	// You can use these when Visual Studio is bugged and broken.
	void add_event2(fsm_event e, fsm_func_t&& func);

	// Add your event implementation that requires a target state
	// (on_enter_from, on_exit_to).
	template <fsm_event Event, StateEnum State>
	void add_event(fsm_func_t&& func);

	// Add your event implementation that requires a target state
	// (on_enter_from, on_exit_to).
	// Non-template version.
	// You can use these when Visual Studio is bugged and broken.
	void add_event2(fsm_event e, StateEnum s, fsm_func_t&& func);

	// Add your event implementation that requires a target transition
	// (on_enter_from, on_exit_to).
	template <fsm_event Event, TransitionEnum Transition>
	void add_event(fsm_func_t&& func);

	// Add your event implementation that requires a target transition
	// (on_enter_from, on_exit_to).
	// Non-template version.
	// You can use these when Visual Studio is bugged and broken.
	void add_event2(fsm_event e, TransitionEnum t, fsm_func_t&& func);

	// Handle transition to a specified state.
	template <TransitionEnum Transition, StateEnum State>
	void add_transition();

	// Handle transition to a specified state.
	// Non-template version.
	// You can use these when Visual Studio is bugged and broken.
	void add_transition2(TransitionEnum t, StateEnum s);

	// Handle but ignore a transition.
	// The equivalent of allowing re-entry, but not calling any enter/exit
	// events on reentry.
	template <TransitionEnum Transition>
	void ignore_transition();

	// Handle but ignore a transition.
	// The equivalent of allowing re-entry, but not calling any enter/exit
	// events on reentry.
	// Non-template version.
	// You can use these when Visual Studio is bugged and broken.
	void ignore_transition2(TransitionEnum t);

	// Used internally to get which state is associated to the provided
	// transition.
	template <TransitionEnum Transition>
	[[nodiscard]]
	StateEnum transition_target() const;

	// Used internally, executes a specific event.
	template <fsm_event Event>
	auto execute_event([[maybe_unused]] StateEnum to_from_state,
			[[maybe_unused]] TransitionEnum to_from_transition, fsm_t& machine,
			FuncArgs... func_args);

private:
	std::array<StateEnum, size_t(TransitionEnum::count)> _transitions;
	std::array<fsm_func_t, size_t(StateEnum::count)> _on_enter_from_state_funcs;
	std::array<fsm_func_t, size_t(StateEnum::count)> _on_exit_to_state_funcs;
	std::array<fsm_func_t, size_t(TransitionEnum::count)>
			_on_enter_from_transition_funcs;
	std::array<fsm_func_t, size_t(TransitionEnum::count)>
			_on_exit_to_transition_funcs;

	fsm_func_t _on_enter_func;
	fsm_func_t _on_update_func;
	fsm_func_t _on_exit_func;


	// TBD, makes it heavy but helps debuggability
	// const char* _name;
};

template <class TransitionEnum, class StateEnum, class FuncRet,
		class... FuncArgs>
struct fsm<TransitionEnum, StateEnum, FuncRet(FuncArgs...)> {
	using state_t = fsm_state<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>;
	using fsm_func_t = typename state_t::fsm_func_t;

	// Helper so you don't have to type all the template parameters.
	// Returns a new state to be filled in and later re-added through add_state.
	[[nodiscard]]
	static constexpr state_t make_state();

	// Here, we use move semantics not for performance (it doesn't do anything).
	// It is to make it clear to the user he cannot modify the state anymore.
	// The fsm gobbles the state.
	template <StateEnum State>
	void add_state(state_t&& state);

	// Set starting state.
	// By default, the first added state is used.
	template <StateEnum State>
	void start_state();

	// Get the starting state.
	StateEnum start_state() const;

	// Set the finish state.
	// Other than being able to call the finished function,
	// doesn't do anything.
	// NOTE : Should probably do more things.
	template <StateEnum State>
	void finish_state();

	// Get the finish state.
	StateEnum finish_state() const;

	// Have we arrived at the finish state?
	[[nodiscard]]
	bool finished() const;

	// Reset the state-machine to the beginning.
	void reset();

	// Trigger a transition.
	// Throws on bad transition (or asserts, if you defined FEA_FSM_NOTHROW).
	// If you had previously called delayed_trigger, this
	// won't do anything.
	template <TransitionEnum Transition>
	void trigger(FuncArgs... func_args);

	// Update the fsm.
	// Calls on_update on the current state.
	// Processes delay_trigger if that was called.
	FuncRet update(FuncArgs... func_args);

	// Get the specified state.
	template <StateEnum State>
	[[nodiscard]]
	const state_t& state() const;

	template <StateEnum State>
	[[nodiscard]]
	state_t& state();

private:
	void maybe_init(FuncArgs... func_args);

	[[nodiscard]]
	const state_t& get_state(StateEnum s) const;
	[[nodiscard]]
	state_t& get_state(StateEnum s);

	std::array<state_t, size_t(StateEnum::count)> _states;
	// Could be bitset.
	std::array<uint8_t, size_t(StateEnum::count)> _state_valid{};
	StateEnum _current_state = StateEnum::count;
	StateEnum _default_state = StateEnum::count;
	StateEnum _finish_state = StateEnum::count;

	bool _in_on_exit = false;
};

template <class, class, class>
struct fsm_builder;

// The fsm_builder is a helper class that returns the appropriately templated
// machine and states.
template <class TransitionEnum, class StateEnum, class FuncRet,
		class... FuncArgs>
struct fsm_builder<TransitionEnum, StateEnum, FuncRet(FuncArgs...)> {
	[[nodiscard]]
	static constexpr fsm_state<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>
	make_state();

	[[nodiscard]]
	static constexpr fsm<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>
	make_machine();
};

} // namespace fea


// Implementation
namespace fea {
// Cleanup the signatures.
#define FEA_FSM_STATE_TMP \
	class TransitionEnum, class StateEnum, class FuncRet, class... FuncArgs

#define FEA_FSM_STATE_TARGS TransitionEnum, StateEnum, FuncRet(FuncArgs...)

template <FEA_FSM_STATE_TMP>
fsm_state<FEA_FSM_STATE_TARGS>::fsm_state() {
	_transitions.fill(StateEnum::count);
}

template <FEA_FSM_STATE_TMP>
template <fsm_event Event>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event(fsm_func_t&& func) {
	static_assert(Event == fsm_event::on_enter || Event == fsm_event::on_exit
						  || Event == fsm_event::on_update,
			"add_event : wrong template resolution called");

	if constexpr (Event == fsm_event::on_enter) {
		_on_enter_func = std::move(func);
	} else if constexpr (Event == fsm_event::on_update) {
		_on_update_func = std::move(func);
	} else if constexpr (Event == fsm_event::on_exit) {
		_on_exit_func = std::move(func);
	}
}

template <FEA_FSM_STATE_TMP>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event2(
		fsm_event e, fsm_func_t&& func) {
	if (e != fsm_event::on_enter && e != fsm_event::on_exit
			&& e != fsm_event::on_update) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Invalid event for function.");
	}
	switch (e) {
	case fea::fsm_event::on_enter: {
		_on_enter_func = std::move(func);
	} break;
	case fea::fsm_event::on_update: {
		_on_update_func = std::move(func);
	} break;
	case fea::fsm_event::on_exit: {
		_on_exit_func = std::move(func);
	} break;
	default: {
		assert(false);
	} break;
	}
}

template <FEA_FSM_STATE_TMP>
template <fsm_event Event, StateEnum State>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event(fsm_func_t&& func) {
	static_assert(
			Event == fsm_event::on_enter_from || Event == fsm_event::on_exit_to,
			"add_event : must use on_enter_from or on_exit_to when "
			"custumizing on transition");

	if constexpr (Event == fsm_event::on_enter_from) {
		std::get<size_t(State)>(_on_enter_from_state_funcs) = std::move(func);
	} else if constexpr (Event == fsm_event::on_exit_to) {
		std::get<size_t(State)>(_on_exit_to_state_funcs) = std::move(func);
	}
}

template <FEA_FSM_STATE_TMP>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event2(
		fsm_event e, StateEnum s, fsm_func_t&& func) {
	if (e != fsm_event::on_enter_from && e != fsm_event::on_exit_to) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Invalid event for function.");
	}

	switch (e) {
	case fea::fsm_event::on_enter_from: {
		_on_enter_from_state_funcs[size_t(s)] = std::move(func);
	} break;
	case fea::fsm_event::on_exit_to: {
		_on_exit_to_state_funcs[size_t(s)] = std::move(func);
	} break;
	default: {
		assert(false);
	} break;
	}
}

template <FEA_FSM_STATE_TMP>
template <fsm_event Event, TransitionEnum Transition>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event(fsm_func_t&& func) {
	static_assert(
			Event == fsm_event::on_enter_from || Event == fsm_event::on_exit_to,
			"add_event : must use on_enter_from or on_exit_to when "
			"custumizing on transition");

	if constexpr (Event == fsm_event::on_enter_from) {
		std::get<size_t(Transition)>(_on_enter_from_transition_funcs)
				= std::move(func);
	} else if constexpr (Event == fsm_event::on_exit_to) {
		std::get<size_t(Transition)>(_on_exit_to_transition_funcs)
				= std::move(func);
	}
}

template <FEA_FSM_STATE_TMP>
void fsm_state<FEA_FSM_STATE_TARGS>::add_event2(
		fsm_event e, TransitionEnum t, fsm_func_t&& func) {
	if (e != fsm_event::on_enter_from && e != fsm_event::on_exit_to) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Invalid event for function.");
	}

	switch (e) {
	case fea::fsm_event::on_enter_from: {
		_on_enter_from_transition_funcs[size_t(t)] = std::move(func);
	} break;
	case fea::fsm_event::on_exit_to: {
		_on_exit_to_transition_funcs[size_t(t)] = std::move(func);
	} break;
	default: {
		assert(false);
	} break;
	}
}

template <FEA_FSM_STATE_TMP>
template <TransitionEnum Transition, StateEnum State>
void fsm_state<FEA_FSM_STATE_TARGS>::add_transition() {
	static_assert(
			Transition != TransitionEnum::count, "fsm_state : bad transition");
	static_assert(State != StateEnum::count, "fsm_state : bad state");
	std::get<size_t(Transition)>(_transitions) = State;
}


template <FEA_FSM_STATE_TMP>
void fsm_state<FEA_FSM_STATE_TARGS>::add_transition2(
		TransitionEnum t, StateEnum s) {
	if (t == TransitionEnum::count) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Bad transition.");
	}

	if (s == StateEnum::count) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Bad state.");
	}

	_transitions[size_t(t)] = s;
}

template <FEA_FSM_STATE_TMP>
template <TransitionEnum Transition>
void fsm_state<FEA_FSM_STATE_TARGS>::ignore_transition() {
	static_assert(
			Transition != TransitionEnum::count, "fsm_state : bad transition");

	std::get<size_t(Transition)>(_transitions) = ignore_sentinel;
}

template <FEA_FSM_STATE_TMP>
void fsm_state<FEA_FSM_STATE_TARGS>::ignore_transition2(TransitionEnum t) {
	if (t == TransitionEnum::count) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Bad transition.");
	}

	_transitions[size_t(t)] = ignore_sentinel;
}

template <FEA_FSM_STATE_TMP>
template <TransitionEnum Transition>
StateEnum fsm_state<FEA_FSM_STATE_TARGS>::transition_target() const {
	if (std::get<size_t(Transition)>(_transitions) == StateEnum::count) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Unhandled transition.");
	}

	return std::get<size_t(Transition)>(_transitions);
}

template <FEA_FSM_STATE_TMP>
template <fsm_event Event>
auto fsm_state<FEA_FSM_STATE_TARGS>::execute_event(
		[[maybe_unused]] StateEnum to_from_state,
		[[maybe_unused]] TransitionEnum to_from_transition, fsm_t& machine,
		FuncArgs... func_args) {
	static_assert(Event != fsm_event::on_enter_from,
			"state : do not execute on_enter_from, use on_enter instead "
			"and provide to_from_state");
	static_assert(Event != fsm_event::on_exit_to,
			"state : do not execute on_exit_to, use on_exit instead and "
			"provide to_from_state");

	static_assert(Event != fsm_event::count, "fsm_state : invalid event");


	// Check the event, call the appropriate user functions if it is stored.
	if constexpr (Event == fsm_event::on_enter) {
		if (to_from_state != StateEnum::count
				&& _on_enter_from_state_funcs[size_t(to_from_state)]) {
			// has enter_from state
			_on_enter_from_state_funcs[size_t(to_from_state)](
					func_args..., machine);

		} else if (to_from_transition != TransitionEnum::count
				   && _on_enter_from_transition_funcs[size_t(
						   to_from_transition)]) {
			// has enter_from transition
			_on_enter_from_transition_funcs[size_t(to_from_transition)](
					func_args..., machine);

		} else if (_on_enter_func) {
			_on_enter_func(func_args..., machine);
		}

	} else if constexpr (Event == fsm_event::on_update) {
		if (_on_update_func) {
			return _on_update_func(func_args..., machine);
		}
	} else if constexpr (Event == fsm_event::on_exit) {
		if (to_from_state != StateEnum::count
				&& _on_exit_to_state_funcs[size_t(to_from_state)]) {
			// has exit_to
			_on_exit_to_state_funcs[size_t(to_from_state)](
					func_args..., machine);

		} else if (to_from_transition != TransitionEnum::count
				   && _on_exit_to_transition_funcs[size_t(
						   to_from_transition)]) {
			// has exit_to
			_on_exit_to_transition_funcs[size_t(to_from_transition)](
					func_args..., machine);

		} else if (_on_exit_func) {
			_on_exit_func(func_args..., machine);
		}
	}
}

template <FEA_FSM_STATE_TMP>
constexpr auto fsm<FEA_FSM_STATE_TARGS>::make_state() -> state_t {
	return fsm_state<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>{};
}

template <FEA_FSM_STATE_TMP>
template <StateEnum State>
void fsm<FEA_FSM_STATE_TARGS>::add_state(state_t&& state) {
	static_assert(State != StateEnum::count, "fsm : bad state");

	std::get<size_t(State)>(_states) = std::move(state);
	std::get<size_t(State)>(_state_valid) = true;

	if (_default_state == StateEnum::count) {
		_default_state = State;
	}
}

template <FEA_FSM_STATE_TMP>
template <StateEnum State>
void fsm<FEA_FSM_STATE_TARGS>::start_state() {
	static_assert(State != StateEnum::count, "fsm : bad state");
	_default_state = State;
}

template <FEA_FSM_STATE_TMP>
StateEnum fsm<FEA_FSM_STATE_TARGS>::start_state() const {
	return _default_state;
}

template <FEA_FSM_STATE_TMP>
template <StateEnum State>
void fsm<FEA_FSM_STATE_TARGS>::finish_state() {
	static_assert(State != StateEnum::count, "fsm : bad state");
	_finish_state = State;
}

template <FEA_FSM_STATE_TMP>
StateEnum fsm<FEA_FSM_STATE_TARGS>::finish_state() const {
	return _finish_state;
}

template <FEA_FSM_STATE_TMP>
bool fsm<FEA_FSM_STATE_TARGS>::finished() const {
	if (_finish_state != StateEnum::count) {
		return _finish_state == _current_state;
	}
	return false;
}

template <FEA_FSM_STATE_TMP>
void fsm<FEA_FSM_STATE_TARGS>::reset() {
	_current_state = StateEnum::count;
}

template <FEA_FSM_STATE_TMP>
template <TransitionEnum Transition>
void fsm<FEA_FSM_STATE_TARGS>::trigger(FuncArgs... func_args) {
	maybe_init(func_args...);

	StateEnum from_state_e = _current_state;
	state_t& from_state = get_state(_current_state);

	StateEnum to_state_e = from_state.template transition_target<Transition>();
	if (to_state_e == state_t::ignore_sentinel) {
		return;
	}
	state_t& to_state = get_state(to_state_e);

	// Only execute on_exit if we aren't in a trigger from on_exit.
	if (!_in_on_exit) {
		_in_on_exit = true;

		// Can recursively call trigger. We must handle that.
		from_state.template execute_event<fsm_event::on_exit>(
				to_state_e, Transition, *this, func_args...);

		if (_in_on_exit == false) {
			// Exit has triggered transition. Abort.
			return;
		}
	}
	_in_on_exit = false;

	_current_state = to_state_e;

	// Always execute on_enter.
	to_state.template execute_event<fsm_event::on_enter>(
			from_state_e, Transition, *this, func_args...);
}

template <FEA_FSM_STATE_TMP>
auto fsm<FEA_FSM_STATE_TARGS>::update(FuncArgs... func_args) -> FuncRet {
	maybe_init(func_args...);

	return get_state(_current_state)
			.template execute_event<fsm_event::on_update>(StateEnum::count,
					TransitionEnum::count, *this, func_args...);
}

template <FEA_FSM_STATE_TMP>
template <StateEnum State>
auto fsm<FEA_FSM_STATE_TARGS>::state() const -> const state_t& {
	return std::get<size_t(State)>(_states);
}

template <FEA_FSM_STATE_TMP>
template <StateEnum State>
auto fsm<FEA_FSM_STATE_TARGS>::state() -> state_t& {
	return std::get<size_t(State)>(_states);
}

template <FEA_FSM_STATE_TMP>
void fsm<FEA_FSM_STATE_TARGS>::maybe_init(FuncArgs... func_args) {
	if (_current_state != StateEnum::count)
		return;

	_current_state = _default_state;
	_states[size_t(_current_state)].template execute_event<fsm_event::on_enter>(
			StateEnum::count, TransitionEnum::count, *this, func_args...);
}

template <FEA_FSM_STATE_TMP>
auto fsm<FEA_FSM_STATE_TARGS>::get_state(StateEnum s) const -> const state_t& {
	if (s == StateEnum::count) {
		fea::maybe_throw(__FUNCTION__, __LINE__, "Accessing invalid state.");
	}

	if (!_state_valid[size_t(s)]) {
		fea::maybe_throw(__FUNCTION__, __LINE__,
				"Accessing invalid state, did you forget to add a state?");
	}

	return _states[size_t(s)];
}

template <FEA_FSM_STATE_TMP>
auto fsm<FEA_FSM_STATE_TARGS>::get_state(StateEnum s) -> state_t& {
	return const_cast<state_t&>(static_cast<const fsm*>(this)->get_state(s));
}

template <FEA_FSM_STATE_TMP>
constexpr fsm_state<FEA_FSM_STATE_TARGS>
fsm_builder<FEA_FSM_STATE_TARGS>::make_state() {
	return fsm_state<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>{};
}

template <FEA_FSM_STATE_TMP>
constexpr fsm<FEA_FSM_STATE_TARGS>
fsm_builder<FEA_FSM_STATE_TARGS>::make_machine() {
	return fsm<TransitionEnum, StateEnum, FuncRet(FuncArgs...)>{};
}
} // namespace fea
