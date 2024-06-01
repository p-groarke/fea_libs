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
#include "fea/events/event_stack.hpp"

#include <array>

/*
event_system is a data structure that maps callbacks to events and allows
triggering specific callbacks using notifiers or channels.

It consumes a lot of memory, but triggering callbacks is very efficient.
Use 'compact_event_system' if you prefer trading off speed for less memory
usage.

TODO : compact_event_system

*/

namespace fea {
namespace detail {
enum class no_enum { count };
}

// An opaque id for a notifier.
// A notifier is the most specific way to trigger events.
// The alternative is an event "channel".
struct notifier_id;

// Either a notifier callback id or channel callback id.
template <class EventEnum, EventEnum e, class ChannelEnum = detail::no_enum,
		ChannelEnum c = detail::no_enum::count>
struct event_sys_id;


// The event system.
// Register notifiers or channels.
// Hook callbacks to either or.
template <class EventEnum, class ChannelEnum, class... FuncTypes>
struct event_system {
	// Stores the channel stacks.
	using channel_tuple_t = decltype(fea::make_tuple_from_count<
			event_stack<EventEnum, FuncTypes...>,
			size_t(ChannelEnum::count)>());

	static_assert(
			std::tuple_size_v<channel_tuple_t> == size_t(ChannelEnum::count),
			"event_system : channel tuple size must be equal to "
			"ChannelEnum::count");

	// Ctors
	event_system() = default;
	event_system(const event_system&) = default;
	event_system(event_system&&) = default;
	event_system& operator=(const event_system&) = default;
	event_system& operator=(event_system&&) = default;

	// Element access / lookup

	// Does the notifier exist?
	[[nodiscard]]
	bool contains(notifier_id nid);

	// Does the nofitier contain subscriber?
	template <EventEnum e>
	[[nodiscard]]
	bool contains(event_sys_id<EventEnum, e> id);

	// Does the channel contain subscriber?
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	bool contains(event_sys_id<EventEnum, e, ChannelEnum, c> id);

	// Does the channel contain subscriber?
	template <EventEnum e>
	[[nodiscard]] bool contains(
			event_sys_id<EventEnum, e, ChannelEnum, ChannelEnum::count>);

	// Access notifier callback.
	template <EventEnum e>
	[[nodiscard]]
	const auto& at(event_sys_id<EventEnum, e> id) const;

	// Access notifier callback.
	template <EventEnum e>
	[[nodiscard]]
	auto& at(event_sys_id<EventEnum, e> id);

	// Access channel callback.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	const auto& at(event_sys_id<EventEnum, e, ChannelEnum, c> id) const;

	// Access channel callback.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	auto& at(event_sys_id<EventEnum, e, ChannelEnum, c> id);

	// Access notifier callback without id checks.
	template <EventEnum e>
	[[nodiscard]]
	const auto& at_unchecked(event_sys_id<EventEnum, e> id) const;

	// Access notifier callback without id checks.
	template <EventEnum e>
	[[nodiscard]]
	auto& at_unchecked(event_sys_id<EventEnum, e> id);

	// Access notifier callback without id checks.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	const auto& at_unchecked(
			event_sys_id<EventEnum, e, ChannelEnum, c> id) const;

	// Access notifier callback without id checks.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	auto& at_unchecked(event_sys_id<EventEnum, e, ChannelEnum, c> id);

	// Capacity

	// Checks whether any notifier or channel has subscribers.
	// Warning : O(n) on notifiers and channels.
	[[nodiscard]]
	bool empty() const noexcept;

	// Checks whether event has subscribers.
	// Warning : O(n) on notifiers and channels.
	template <EventEnum e>
	[[nodiscard]]
	bool event_empty() const noexcept;

	// Checks whether the notifier has any subscribers.
	[[nodiscard]]
	bool empty(notifier_id nid) const noexcept;

	// Checks whether the notifier event has subscribers.
	template <EventEnum e>
	[[nodiscard]]
	bool empty(notifier_id nid) const noexcept;

	// Checks whether the channel has any subscribers.
	template <ChannelEnum c>
	[[nodiscard]]
	bool empty() const noexcept;

	// Checks whether the channel event has subscribers.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	bool empty() const noexcept;

	// Return total number of subscribers.
	// Warning : O(n) on notifiers and channels.
	[[nodiscard]]
	size_t size() const noexcept;

	// Return number of subscribers for event.
	// Warning : O(n) on notifiers and channels.
	template <EventEnum e>
	[[nodiscard]]
	size_t event_size() const noexcept;

	// Returns the number of subscribers.
	[[nodiscard]]
	size_t size(notifier_id nid) const noexcept;

	// Returns the number of subscribers to event.
	template <EventEnum e>
	[[nodiscard]]
	size_t size(notifier_id nid) const noexcept;

	// Returns the number of subscribers.
	template <ChannelEnum c>
	[[nodiscard]]
	size_t size() const noexcept;

	// Returns the number of subscribers to event.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	size_t size() const noexcept;

	// Reserve storage for all event subscribers.
	void reserve(notifier_id nid, size_t new_cap);

	// Reserve storage for event subscribers.
	template <EventEnum e>
	void reserve(notifier_id nid, size_t new_cap);

	// Reserve storage for all event subscribers.
	template <ChannelEnum c>
	void reserve(size_t new_cap);

	// Reserve storage for event subscribers.
	template <ChannelEnum c, EventEnum e>
	void reserve(size_t new_cap);

	// How many subscribers can be held in allocated storage.
	template <EventEnum e>
	[[nodiscard]]
	size_t capacity(notifier_id nid) const noexcept;

	// How many subscribers can be held in allocated storage.
	template <ChannelEnum c, EventEnum e>
	[[nodiscard]]
	size_t capacity() const noexcept;

	// Modifiers

	// Clear everything (all notifiers, callbacks, channel callbacks, etc).
	void clear();

	// Clear all callbacks from notifiers and channels (but keeps notifiers).
	// Warning : O(n) on notifiers and channels.
	void clear_subscribers();

	// Clear event subscribers from both notifiers and channels.
	// Warning : O(n) on notifiers.
	template <EventEnum e>
	void event_clear();

	// Clear events from notifier.
	void clear(notifier_id nid);

	// Clear events e from notifier.
	template <EventEnum e>
	void clear(notifier_id nid);

	// Clear events from channel.
	template <ChannelEnum c>
	void clear();

	// Clear events e from channel.
	template <ChannelEnum c, EventEnum e>
	void clear();

	// Adds a notifier Id.
	// You can attach callbacks to notifiers and their events.
	[[nodiscard]]
	notifier_id add_notifier();

	// Removes a notifier Id and its callbacks.
	void remove_notifier(notifier_id nid);

	// Subscribe a callback to an event from notifier nid.
	template <EventEnum e, class Func>
	event_sys_id<EventEnum, e> subscribe(notifier_id nid, Func&& callback);

	// Subscribe a callback to an event from a channel.
	template <ChannelEnum c, EventEnum e, class Func>
	event_sys_id<EventEnum, e, ChannelEnum, c> subscribe(Func&& func);

	// Unsubscribe a callback from event e of notifier nid.
	template <EventEnum e>
	void unsubscribe(event_sys_id<EventEnum, e> id);

	// Unsubscribe a callback from event e of specified channel.
	template <ChannelEnum c, EventEnum e>
	void unsubscribe(event_sys_id<EventEnum, e, ChannelEnum, c> id);

	// Trigger event of notifier nid.
	template <EventEnum e, class... Args>
	void trigger(notifier_id nid, Args&&... args);

#if FEA_WITH_TBB
	// Multithreaded trigger of event of notifier nid.
	template <EventEnum e, class... Args>
	void trigger_mt(notifier_id nid, Args&&... args);
#endif

	// Trigger event of specified channel.
	template <ChannelEnum c, EventEnum e, class... Args>
	void trigger(Args&&... args);

#if FEA_WITH_TBB
	// Mulithreaded trigger of event of specified channel.
	template <ChannelEnum c, EventEnum e, class... Args>
	void trigger_mt(Args&&... args);
#endif

private:
	// Notifier events.
	id_slotmap<size_t, event_stack<EventEnum, FuncTypes...>> _notifier_stacks{};

	// Generates notifier ids.
	size_t _notifier_id_generator = 0;

	// Channel events.
	channel_tuple_t _channel_stacks{};
};

} // namespace fea

#include "imp/event_system.imp.hpp"
