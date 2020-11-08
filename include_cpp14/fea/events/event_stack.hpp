/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#pragma once
#include "fea/maps/unsigned_map.hpp"
#include "fea/meta/tuple.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <tbb/parallel_for.h>
#include <tuple>
#include <type_traits>
#include <utility>


/*
event_stack is a data structure to hold callbacks associated with trigger
events.

You create an event_stack by providing an enum of your events, and the callback
signatures.

For ex:

enum class my_events { event1, event2, count };
fea::event_stack<my_events, void(), void(int)> my_stack;

Your event enum must fulfill these requirements :
	- Contains 'count' memember equal to count events.
	- Must contain > 0 events.
	- Members must be >= 0.

You can then subscribe and unsubscribe callbacks.
And you can trigger events with the appropriate function parameters.
*/

namespace fea {
// A callback Id.
// Used to access or unsubscribe a callback.
template <class EventEnum, EventEnum e>
struct event_id {
	event_id() = default;

private:
	template <class, class...>
	friend struct event_stack;

	event_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};


// A container to associate callbacks with event triggers.
template <class EventEnum, class... FuncTypes>
struct event_stack {

	// EventEnum must be enum.
	static_assert(std::is_enum<EventEnum>::value,
			"event_stack : template parameter EventEnum must be enum");

	// EventEnum must contain member 'count' and must not be 0.
	static_assert(size_t(EventEnum::count) != 0,
			"event_stack : enum must declare count and count must not be 0");

	// Must provide 'count' number of event signature.
	static_assert(sizeof...(FuncTypes) == size_t(EventEnum::count),
			"event_stack : must provide a function signature for every event");

private:
	// Stores the callbacks.
	using event_tuple_t = std::tuple<
			fea::unsigned_map<size_t, std::function<FuncTypes>>...>;

	// Stores the counters to generate ids.
	using id_gen_tuple_t = decltype(
			fea::make_tuple_from_count<size_t, sizeof...(FuncTypes)>());

	// Event tuple must be size 'count'.
	static_assert(
			std::tuple_size<event_tuple_t>::value == size_t(EventEnum::count),
			"event_stack : tuple size must be equal to count");

	// Id generator tuple must be size 'count'.
	static_assert(
			std::tuple_size<id_gen_tuple_t>::value == size_t(EventEnum::count),
			"event_stack : tuple size must be equal to count");

public:
	event_stack() = default;
	~event_stack() = default;
	event_stack(const event_stack&) = default;
	event_stack(event_stack&&) = default;
	event_stack& operator=(const event_stack&) = default;
	event_stack& operator=(event_stack&&) = default;


	// Element access / lookup

	// Does the event contain subscriber?
	template <EventEnum e>
	bool contains(event_id<EventEnum, e> id) {
		return std::get<size_t(e)>(_stacks).contains(id._id);
	}

	// Access callback.
	template <EventEnum e>
	const auto& at(event_id<EventEnum, e> id) const {
		return std::get<size_t(e)>(_stacks).at(id._id);
	}
	// Access callback.
	template <EventEnum e>
	auto& at(event_id<EventEnum, e> id) {
		return std::get<size_t(e)>(_stacks).at(id._id);
	}

	// Access callback without id checks.
	template <EventEnum e>
	const auto& at_unchecked(event_id<EventEnum, e> id) const {
		assert(contains<e>(id));
		return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
	}
	// Access callback without id checks.
	template <EventEnum e>
	auto& at_unchecked(event_id<EventEnum, e> id) {
		assert(contains<e>(id));
		return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
	}


	// Capacity

	// Checks if the event_stack is empty.
	bool empty() const noexcept {
		bool ret = true;
		tuple_foreach([&](const auto& map) { ret &= map.empty(); }, _stacks);
		return ret;
	}

	// Checks whether the event has subscribers.
	template <EventEnum e>
	bool empty() const noexcept {
		return std::get<size_t(e)>(_stacks).empty();
	}

	// Returns total size of subscribers.
	size_t size() const noexcept {
		size_t ret = 0;
		tuple_foreach([&](const auto& map) { ret += map.size(); }, _stacks);
		return ret;
	}

	// Returns the number of subscribers to event.
	template <EventEnum e>
	size_t size() const noexcept {
		return std::get<size_t(e)>(_stacks).size();
	}

	// Reserve same storage for all event subscribers.
	void reserve(size_t new_cap) {
		tuple_foreach([&](auto& map) { map.reserve(new_cap); }, _stacks);
	}

	// Reserve storage for event subscribers.
	template <EventEnum e>
	void reserve(size_t new_cap) {
		std::get<size_t(e)>(_stacks).reserve(new_cap);
	}

	// How many subscribers can be held in allocated storage.
	template <EventEnum e>
	size_t capacity() const noexcept {
		return std::get<size_t(e)>(_stacks).capacity();
	}


	// Modifiers

	// Clear all event subscribers.
	void clear() {
		tuple_foreach([&](auto& map) { map.clear(); }, _stacks);
	}

	// Clear event subscribers.
	template <EventEnum e>
	void clear() {
		std::get<size_t(e)>(_stacks).clear();
	}

	// Subscribe a callback to an event.
	// Returns the subscriber id.
	template <EventEnum e, class Func>
	event_id<EventEnum, e> subscribe(Func&& callback) {
		size_t& id_generator = std::get<size_t(e)>(_id_generators);
		assert(id_generator != (std::numeric_limits<size_t>::max)());

		// 0 is never used, reserved for future.
		++id_generator;

		// Insert callback.
		std::get<size_t(e)>(_stacks).insert(
				{ id_generator, std::forward<Func>(callback) });

		// Return id.
		return { id_generator };
	}

	// Unsubscribe callback.
	template <EventEnum e>
	void unsubscribe(event_id<EventEnum, e> id) {
		std::get<size_t(e)>(_stacks).erase(id._id);
	}

	// Execution

	// Trigger event e with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger(FuncArgs&&... func_args) const {
		for (const auto& func_pair : std::get<size_t(e)>(_stacks)) {
			// std::invoke is not compile time, plus it makes debugging
			// difficult.
			func_pair.second(std::forward<FuncArgs>(func_args)...);
		}
	}
	// Trigger event e with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger(FuncArgs&&... func_args) {
		for (auto& func_pair : std::get<size_t(e)>(_stacks)) {
			func_pair.second(std::forward<FuncArgs>(func_args)...);
		}
	}

	// Trigger event e callbacks in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args) const {
		auto& map = std::get<size_t(e)>(_stacks);
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, map.size() },
				[&, this](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						map.data()[i].second(
								std::forward<FuncArgs>(func_args)...);
					}
				});
	}
	// Trigger event e callbacks in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args) {
		auto& map = std::get<size_t(e)>(_stacks);
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, map.size() },
				[&](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						map.data()[i].second(
								std::forward<FuncArgs>(func_args)...);
					}
				});
	}

private:
	event_tuple_t _stacks{};
	id_gen_tuple_t _id_generators{};
};


} // namespace fea
