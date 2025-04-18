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


// Implementation
namespace fea {
template <class, class, class...>
struct event_system;

// could be : using notifier_id = size_t;
struct notifier_id {
	notifier_id() = default;
	~notifier_id() = default;
	notifier_id(const notifier_id&) = default;
	notifier_id(notifier_id&&) = default;
	notifier_id& operator=(const notifier_id&) = default;
	notifier_id& operator=(notifier_id&&) = default;

private:
	template <class, class, class...>
	friend struct event_system;

	inline notifier_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};

// Channel callback Id
template <class EventEnum, EventEnum e, class ChannelEnum, ChannelEnum c>
struct event_sys_id {
	event_sys_id() = default;

private:
	template <class, class, class...>
	friend struct event_system;

	event_sys_id(event_id<EventEnum, e> eid)
			: _eid(eid) {
	}

	event_id<EventEnum, e> _eid;
};


// Notifier callback Id
template <class EventEnum, EventEnum e>
struct event_sys_id<EventEnum, e> {
	event_sys_id() = default;

	notifier_id nid() const {
		return _nid;
	}

private:
	template <class, class, class...>
	friend struct event_system;

	event_sys_id(notifier_id nid_, event_id<EventEnum, e> eid)
			: _nid(nid_)
			, _eid(eid) {
	}

	notifier_id _nid;
	event_id<EventEnum, e> _eid;
};


template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		notifier_id nid) {
	return _notifier_stacks.contains(nid._id);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e> id) {
	if (!contains(id._nid)) {
		return false;
	}
	return _notifier_stacks.at_unchecked(id._nid._id).contains(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	return std::get<size_t(c)>(_channel_stacks).contains(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e, ChannelEnum, ChannelEnum::count>) {
	return false;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e> id) const {
	return _notifier_stacks.at(id._nid._id).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e> id) {
	return _notifier_stacks.at(id._nid._id).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) const {
	return std::get<size_t(c)>(_channel_stacks).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	return std::get<size_t(c)>(_channel_stacks).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e> id) const {
	assert(contains(id));
	return _notifier_stacks.at_unchecked(id._nid._id).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e> id) {
	assert(contains(id));
	return _notifier_stacks.at_unchecked(id._nid._id).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) const {
	assert(contains(id));
	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	assert(contains(id));
	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	for (const auto& stack_pair : _notifier_stacks) {
		if (!stack_pair.second.empty()) {
			return false;
		}
	}

	bool ret = true;
	tuple_for_each(
			[&](const auto& stack) { ret &= stack.empty(); }, _channel_stacks);
	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::event_empty()
		const noexcept {
	for (const auto& stack_pair : _notifier_stacks) {
		if (!stack_pair.second.template empty<e>()) {
			return false;
		}
	}

	bool ret = true;
	tuple_for_each([&](const auto& stack) { ret &= stack.template empty<e>(); },
			_channel_stacks);
	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).empty();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template empty<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	return std::get<size_t(c)>(_channel_stacks).empty();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template empty<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	size_t ret = 0;
	for (const auto& stack_pair : _notifier_stacks) {
		ret += stack_pair.second.size();
	}

	tuple_for_each(
			[&](const auto& stack) { ret += stack.size(); }, _channel_stacks);

	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::event_size()
		const noexcept {
	size_t ret = 0;
	for (const auto& stack_pair : _notifier_stacks) {
		ret += stack_pair.second.template size<e>();
	}

	tuple_for_each([&](const auto& stack) { ret += stack.template size<e>(); },
			_channel_stacks);

	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::size(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).size();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::size(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template size<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).size();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template size<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		notifier_id nid, size_t new_cap) {
	_notifier_stacks.at(nid._id).reserve(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		notifier_id nid, size_t new_cap) {
	_notifier_stacks.at(nid._id).template reserve<e>(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		size_t new_cap) {
	std::get<size_t(c)>(_channel_stacks).reserve(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		size_t new_cap) {
	std::get<size_t(c)>(_channel_stacks).template reserve<e>(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::capacity(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template capacity<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::capacity() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template capacity<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	_notifier_stacks.clear();
	tuple_for_each([](auto& stack) { stack.clear(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear_subscribers() {
	for (auto& stack_pair : _notifier_stacks) {
		stack_pair.second.clear();
	}
	tuple_for_each([](auto& stack) { stack.clear(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::event_clear() {
	for (auto& stack_pair : _notifier_stacks) {
		stack_pair.second.template clear<e>();
	}
	tuple_for_each(
			[](auto& stack) { stack.template clear<e>(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear(
		notifier_id nid) {
	_notifier_stacks.at(nid._id).clear();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear(
		notifier_id nid) {
	_notifier_stacks.at(nid._id).template clear<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	std::get<size_t(c)>(_channel_stacks).clear();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	std::get<size_t(c)>(_channel_stacks).template clear<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
notifier_id event_system<EventEnum, ChannelEnum, FuncTypes...>::add_notifier() {
	assert(_notifier_id_generator != (std::numeric_limits<size_t>::max)());

	// 0 is never used, reserved for future.
	++_notifier_id_generator;

	_notifier_stacks.insert({ _notifier_id_generator, {} });
	return { _notifier_id_generator };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::remove_notifier(
		notifier_id nid) {
	_notifier_stacks.erase(nid._id);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class Func>
event_sys_id<EventEnum, e>
event_system<EventEnum, ChannelEnum, FuncTypes...>::subscribe(
		notifier_id nid, Func&& callback) {
	return { nid, _notifier_stacks.at(nid._id).template subscribe<e>(
						  std::forward<Func>(callback)) };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class Func>
event_sys_id<EventEnum, e, ChannelEnum, c>
event_system<EventEnum, ChannelEnum, FuncTypes...>::subscribe(Func&& func) {
	return { std::get<size_t(c)>(_channel_stacks)
					 .template subscribe<e>(std::forward<Func>(func)) };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::unsubscribe(
		event_sys_id<EventEnum, e> id) {
	_notifier_stacks.at(id._nid._id).template unsubscribe<e>(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::unsubscribe(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	std::get<size_t(c)>(_channel_stacks).template unsubscribe<e>(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger(
		notifier_id nid, Args&&... args) {
	_notifier_stacks.at(nid._id).template trigger<e>(
			std::forward<Args>(args)...);
}

#if FEA_WITH_TBB
template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger_mt(
		notifier_id nid, Args&&... args) {
	_notifier_stacks.at(nid._id).template trigger_mt<e>(
			std::forward<Args>(args)...);
}
#endif

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger(
		Args&&... args) {
	std::get<size_t(c)>(_channel_stacks)
			.template trigger<e>(std::forward<Args>(args)...);
}

#if FEA_WITH_TBB
template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger_mt(
		Args&&... args) {
	std::get<size_t(c)>(_channel_stacks)
			.template trigger_mt<e>(std::forward<Args>(args)...);
}
#endif

} // namespace fea

