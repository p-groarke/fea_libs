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

namespace fea {
template <class, class, class...>
struct event_system;

// template <class EventEnum, EventEnum e>
// struct event_id {
//	event_id() = default;
//
// private:
//	template <class, class...>
//	friend struct event_stack;
//
//	event_id(size_t id)
//			: _id(id) {
//	}
//
//	size_t _id;
//};

template <class EventEnum, EventEnum e>
struct event_sys_id {
	event_sys_id() = default;

private:
	template <class, class, class...>
	friend struct event_system;

	event_sys_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};

struct notifier_id {};

// using notifier_id = event_id<event_system>;

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
