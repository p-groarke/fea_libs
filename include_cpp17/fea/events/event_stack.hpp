/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/containers/id_slotmap.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/performance/constants.hpp"
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

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
// An opaque callback Id.
// Used to access or unsubscribe to a callback.
template <class EventEnum, EventEnum e>
struct event_id;

// A container to associate callbacks with event triggers.
template <class EventEnum, class... FuncTypes>
struct event_stack {
	// Stores the callbacks.
	using event_tuple_t
			= std::tuple<fea::id_slotmap<size_t, std::function<FuncTypes>>...>;

	// Stores the counters to generate ids.
	using id_gen_tuple_t = decltype(fea::make_tuple_from_count<size_t,
			sizeof...(FuncTypes)>());

	static_assert(std::is_enum_v<EventEnum>,
			"event_stack : template parameter EventEnum must be enum");
	static_assert(size_t(EventEnum::count) != 0,
			"event_stack : enum must declare count and count must not be 0");
	static_assert(sizeof...(FuncTypes) == size_t(EventEnum::count),
			"event_stack : must provide a function signature for every event");
	static_assert(std::tuple_size_v<event_tuple_t> == size_t(EventEnum::count),
			"event_stack : tuple size must be equal to count");
	static_assert(std::tuple_size_v<id_gen_tuple_t> == size_t(EventEnum::count),
			"event_stack : tuple size must be equal to count");

	// Ctors
	event_stack() = default;
	~event_stack() = default;
	event_stack(const event_stack&) = default;
	event_stack(event_stack&&) = default;
	event_stack& operator=(const event_stack&) = default;
	event_stack& operator=(event_stack&&) = default;

	// Element access / lookup

	// Does the event contain subscriber?
	template <EventEnum e>
	[[nodiscard]]
	bool contains(event_id<EventEnum, e> id);

	// Access callback.
	template <EventEnum e>
	[[nodiscard]]
	const auto& at(event_id<EventEnum, e> id) const;

	// Access callback.
	template <EventEnum e>
	[[nodiscard]]
	auto& at(event_id<EventEnum, e> id);

	// Access callback without id checks.
	template <EventEnum e>
	[[nodiscard]]
	const auto& at_unchecked(event_id<EventEnum, e> id) const;

	// Access callback without id checks.
	template <EventEnum e>
	[[nodiscard]]
	auto& at_unchecked(event_id<EventEnum, e> id);

	// Capacity

	// Checks if the event_stack is empty.
	[[nodiscard]]
	bool empty() const noexcept;

	// Checks whether the event has subscribers.
	template <EventEnum e>
	[[nodiscard]]
	bool empty() const noexcept;

	// Returns total size of subscribers.
	[[nodiscard]]
	size_t size() const noexcept;

	// Returns the number of subscribers to event.
	template <EventEnum e>
	[[nodiscard]]
	size_t size() const noexcept;

	// Reserve same storage for all event subscribers.
	void reserve(size_t new_cap);

	// Reserve storage for event subscribers.
	template <EventEnum e>
	void reserve(size_t new_cap);

	// How many subscribers can be held in allocated storage.
	template <EventEnum e>
	[[nodiscard]]
	size_t capacity() const noexcept;

	// Modifiers

	// Clear all event subscribers.
	void clear();

	// Clear event subscribers.
	template <EventEnum e>
	void clear();

	// Subscribe a callback to an event.
	// Returns the subscriber id.
	template <EventEnum e, class Func>
	event_id<EventEnum, e> subscribe(Func&& callback);

	// Unsubscribe callback.
	template <EventEnum e>
	void unsubscribe(event_id<EventEnum, e> id);

	// Execution

	// Trigger event e with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger(FuncArgs&&... func_args) const;

	// Trigger event e with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger(FuncArgs&&... func_args);

#if FEA_WITH_TBB
	//  Trigger event e callbacks in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args) const;

	// Trigger event e callbacks in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args);
#endif

private:
	event_tuple_t _stacks{};
	id_gen_tuple_t _id_generators{};
};

} // namespace fea

#include "imp/event_stack.imp.hpp"
