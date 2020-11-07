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
#include "fea/events/event_stack.hpp"

#include <array>

/*
event_system is a data structure that maps callbacks to events and allows
triggering specific callbacks using notifiers or channels.

It consumes a lot of memory, but triggering callbacks is very efficient.
Use 'compact_event_system' if you prefer trading off speed for less memory
usage.

*/

namespace fea {
template <class, class, class...>
struct event_system;

// could be : using notifier_id = size_t;
struct notifier_id {
	notifier_id() = default;

private:
	template <class, class, class...>
	friend struct event_system;

	notifier_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};

namespace detail {
enum class no_enum { count };
}

template <class EventEnum, EventEnum e, class ChannelEnum = detail::no_enum,
		ChannelEnum c = detail::no_enum::count>
struct event_sys_id;

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
struct event_system {
	// static_assert(size_t(ChannelEnum::count) != 0,
	//		"event_system : your channel enum must contain count and cannot "
	//		"have 0 events");

	// static_assert(std::is_unsigned_v<channel_underlying_t>,
	//		"event_system : enum class must be unsigned integral");

private:
	// Stores the channel stacks.
	using channel_tuple_t = decltype(
			fea::make_tuple_from_count<event_stack<EventEnum, FuncTypes...>,
					size_t(ChannelEnum::count)>());

	// Channel tuple must be size 'ChannelEnum::count'.
	static_assert(std::tuple_size<channel_tuple_t>::value
					== size_t(ChannelEnum::count),
			"event_system : channel tuple size must be equal to "
			"ChannelEnum::count");

public:
	event_system() = default;
	event_system(const event_system&) = default;
	event_system(event_system&&) = default;
	event_system& operator=(const event_system&) = default;
	event_system& operator=(event_system&&) = default;


	// Element access / lookup

	// Does the notifier exist?
	bool contains(notifier_id nid) {
		return _notifier_stacks.contains(nid._id);
	}

	// Does the nofitier contain subscriber?
	template <EventEnum e>
	bool contains(event_sys_id<EventEnum, e> id) {
		if (!contains(id._nid)) {
			return false;
		}
		return _notifier_stacks.at_unchecked(id._nid._id).contains(id._eid);
	}

	// Does the channel contain subscriber?
	template <EventEnum e, ChannelEnum c>
	bool contains(event_sys_id<EventEnum, e, ChannelEnum, c> id) {
		return std::get<size_t(c)>(_channel_stacks).contains(id._eid);
	}
	template <EventEnum e>
	bool contains(event_sys_id<EventEnum, e, ChannelEnum, ChannelEnum::count>) {
		return false;
	}

	//// Access notifier const callback ref.
	// template <EventEnum e>
	// const auto& at(notifier_id nid, event_id<EventEnum, e> id) const {
	//	return _notifier_stacks.at(nid._id).at(id);
	//}
	//// Access notifier callback ref.
	// template <EventEnum e>
	// auto& at(notifier_id nid, event_id<EventEnum, e> id) {
	//	return _notifier_stacks.at(nid._id).at(id);
	//}

	//// Access channel const callback ref.
	// template <ChannelEnum c, EventEnum e>
	// const auto& at(event_id<EventEnum, e> id) const {
	//	return std::get<size_t(c)>(_channel_stacks).at(id);
	//}
	//// Access channel callback ref.
	// template <ChannelEnum c, EventEnum e>
	// auto& at(event_id<EventEnum, e> id) {
	//	return std::get<size_t(c)>(_channel_stacks).at(id);
	//}

	//// Access notifier const callback ref without id checks.
	// template <EventEnum e>
	// const auto& at_unchecked(notifier_id nid, event_id<EventEnum, e> id)
	// const { 	assert(contains(nid, id)); 	return
	//_notifier_stacks.at_unchecked(nid._id).at_unchecked(id);
	//}
	//// Access notifier callback ref without id checks.
	// template <EventEnum e>
	// auto& at_unchecked(notifier_id nid, event_id<EventEnum, e> id) {
	//	assert(contains(nid, id));
	//	return _notifier_stacks.at_unchecked(nid._id).at_unchecked(id);
	//}

	//// Access notifier const callback ref without id checks.
	// template <ChannelEnum c, EventEnum e>
	// const auto& at_unchecked(event_id<EventEnum, e> id) const {
	//	assert(contains<c>(id));
	//	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id);
	//}
	//// Access notifier callback ref without id checks.
	// template <ChannelEnum c, EventEnum e>
	// auto& at_unchecked(event_id<EventEnum, e> id) {
	//	assert(contains<c>(id));
	//	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id);
	//}


	// Capacity

	// Checks whether the notifier has any subscribers.
	bool empty(notifier_id nid) const noexcept {
		return _notifier_stacks.at(nid._id).empty();
	}
	// Checks whether the notifier event has subscribers.
	template <EventEnum e>
	bool empty(notifier_id nid) const noexcept {
		return _notifier_stacks.at(nid._id).empty<e>();
	}

	// Checks whether the channel has any subscribers.
	template <ChannelEnum c>
	bool empty() const noexcept {
		return std::get<size_t(c)>(_channel_stacks).empty();
	}
	// Checks whether the channel event has subscribers.
	template <ChannelEnum c, EventEnum e>
	bool empty() const noexcept {
		return std::get<size_t(c)>(_channel_stacks).empty<e>();
	}

	// Returns the number of subscribers.
	size_t size(notifier_id nid) const noexcept {
		return _notifier_stacks.at(nid._id).size();
	}
	// Returns the number of subscribers to event.
	template <EventEnum e>
	size_t size(notifier_id nid) const noexcept {
		return _notifier_stacks.at(nid._id).size<e>();
	}

	// Returns the number of subscribers.
	template <ChannelEnum c>
	size_t size() const noexcept {
		return std::get<size_t(c)>(_channel_stacks).size();
	}
	// Returns the number of subscribers to event.
	template <ChannelEnum c, EventEnum e>
	size_t size() const noexcept {
		return std::get<size_t(c)>(_channel_stacks).size<e>();
	}

	// Reserve storage for all event subscribers.
	void reserve(notifier_id nid, size_t new_cap) {
		_notifier_stacks.at(nid._id).reserve(new_cap);
	}
	// Reserve storage for event subscribers.
	template <EventEnum e>
	void reserve(notifier_id nid, size_t new_cap) {
		_notifier_stacks.at(nid._id).reserve<e>(new_cap);
	}

	// Reserve storage for all event subscribers.
	template <ChannelEnum c>
	void reserve(size_t new_cap) {
		std::get<size_t(c)>(_channel_stacks).reserve(new_cap);
	}
	// Reserve storage for event subscribers.
	template <ChannelEnum c, EventEnum e>
	void reserve(size_t new_cap) {
		std::get<size_t(c)>(_channel_stacks).reserve<e>(new_cap);
	}

	// How many subscribers can be held in allocated storage.
	template <EventEnum e>
	size_t capacity(notifier_id nid) const noexcept {
		return _notifier_stacks.at(nid._id).capacity<e>();
	}

	// How many subscribers can be held in allocated storage.
	template <ChannelEnum c, EventEnum e>
	size_t capacity() const noexcept {
		return std::get<size_t(c)>(_channel_stacks).capacity<e>();
	}


	// Modifiers

	notifier_id add_notifier() {
		assert(_notifier_id_generator != (std::numeric_limits<size_t>::max)());

		// 0 is never used, reserved for future.
		++_notifier_id_generator;

		_notifier_stacks.insert({ _notifier_id_generator, {} });

		return { _notifier_id_generator };
	}

	void remove_notifier(notifier_id nid) {
		_notifier_stacks.erase(nid._id);
	}


	// template <EventEnum e, class Func>
	// event_stack_id subscribe(notifier_id nid, Func&& func) {
	//	return _object_stacks.at(nid).subscribe<e>(std::forward<Func>(func));
	//}

	// template <class... Funcs>
	// auto subscribe(notifier_id nid, Funcs&&... funcs) {
	//	return _object_stacks.at(nid).subscribe(std::forward<Funcs>(funcs)...);
	//}

	//// Channels
	// template <EventEnum e, class Func>
	// event_stack_id subscribe(ChannelEnum c, Func&& func) {
	//	return _channel_stacks[underlying_chan(c)].subscribe<e>(
	//			std::forward<Func>(func));
	//}

	// template <class... Funcs>
	// auto subscribe(ChannelEnum c, Funcs&&... funcs) {
	//	return _channel_stacks[underlying_chan(c)].subscribe(
	//			std::forward<Funcs>(funcs)...);
	//}

	// template <EventEnum e>
	// void unsubscribe(notifier_id nid, event_stack_id id) {
	//	_object_stacks.at(nid).unsubscribe<e>(id);
	//}

	// template <EventEnum e>
	// void unsubscribe(ChannelEnum c, event_stack_id id) {
	//	_channel_stacks[underlying_chan(c)].unsubscribe<e>(id);
	//}

	// template <EventEnum e, class... Args>
	// void execute_event(notifier_id nid, Args&&... args) {
	//	_object_stacks.at(nid).execute<e>(std::forward<Args>(args)...);
	//}

	// template <EventEnum e, class... Args>
	// void execute_event(ChannelEnum c, Args&&... args) {
	//	_channel_stacks[underlying_chan(c)].execute<e>(
	//			std::forward<Args>(args)...);
	//}

private:
	// Notifier events.
	unsigned_map<size_t, event_stack<EventEnum, FuncTypes...>> _notifier_stacks;

	// Generates notifier ids.
	size_t _notifier_id_generator = 0;

	// Channel events.
	channel_tuple_t _channel_stacks;
};

} // namespace fea


/*
template <class EventEnum, class ChannelEnum, class... FuncTypes>
struct event_system {
	event_system() = default;
	event_system(const event_system&) = default;
	event_system(event_system&&) = default;
	event_system& operator=(const event_system&) = default;
	event_system& operator=(event_system&&) = default;

	using channel_underlying_t = typename std::underlying_type_t<ChannelEnum>;

private:
	static constexpr channel_underlying_t underlying_chan(ChannelEnum e) {
		return static_cast<channel_underlying_t>(e);
	}

public:
	notifier_id add_notifier() {
		return _object_stacks.insert({});
	}

	void remove_notifier(notifier_id nid) {
		_object_stacks.erase(nid);
	}

	template <EventEnum e, class... Args>
	void execute_event(notifier_id nid, Args&&... args) {
		_object_stacks.at(nid).execute<e>(std::forward<Args>(args)...);
	}

	// Objects
	template <EventEnum e, class Func>
	event_stack_id subscribe(notifier_id nid, Func&& func) {
		return _object_stacks.at(nid).subscribe<e>(std::forward<Func>(func));
	}

	template <class... Funcs>
	auto subscribe(notifier_id nid, Funcs&&... funcs) {
		return _object_stacks.at(nid).subscribe(std::forward<Funcs>(funcs)...);
	}

	// Channels
	template <EventEnum e, class Func>
	event_stack_id subscribe(ChannelEnum c, Func&& func) {
		return _channel_stacks[underlying_chan(c)].subscribe<e>(
				std::forward<Func>(func));
	}

	template <class... Funcs>
	auto subscribe(ChannelEnum c, Funcs&&... funcs) {
		return _channel_stacks[underlying_chan(c)].subscribe(
				std::forward<Funcs>(funcs)...);
	}

	template <EventEnum e>
	void unsubscribe(notifier_id nid, event_stack_id id) {
		_object_stacks.at(nid).unsubscribe<e>(id);
	}

	template <EventEnum e>
	void unsubscribe(ChannelEnum c, event_stack_id id) {
		_channel_stacks[underlying_chan(c)].unsubscribe<e>(id);
	}

	template <EventEnum e, class... Args>
	void execute_event(ChannelEnum c, Args&&... args) {
		_channel_stacks[underlying_chan(c)].execute<e>(
				std::forward<Args>(args)...);
	}

private:
	slot_map<event_stack<EventEnum, FuncTypes...>> _object_stacks{};
	std::array<event_stack<EventEnum, FuncTypes...>,
			underlying_chan(ChannelEnum::count)>
			_channel_stacks{};

	static_assert(underlying_chan(ChannelEnum::count) != 0,
			"event_system : your event enum must contain count and cannot have "
			"0 events");

	static_assert(std::is_unsigned_v<channel_underlying_t>,
			"event_system : enum class must be unsigned integral");
};
} // namespace fea
*/
