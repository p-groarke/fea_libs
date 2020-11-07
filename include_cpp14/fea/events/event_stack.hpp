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

namespace fea {
template <class EventEnum, EventEnum e>
struct event_id {
	event_id() = default;

private:
	template <class, class...>
	friend struct event_stack;

	event_id(size_t id)
			: _id(id) {
	}

	size_t _id;
};


template <class EventEnum, class... FuncTypes>
struct event_stack {

	// EventEnum must be enum.
	static_assert(std::is_enum_v<EventEnum>,
			"event_stack : template parameter EventEnum must be enum");

	// EventEnum must be unsigned.
	static_assert(
			std::is_unsigned_v<typename std::underlying_type<EventEnum>::type>,
			"event_stack : enum underlying type must be unsigned");

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

	// Access const callback ref.
	template <EventEnum e>
	const auto& at(event_id<EventEnum, e> id) const {
		return std::get<size_t(e)>(_stacks).at(id._id);
	}
	// Access callback ref.
	template <EventEnum e>
	auto& at(event_id<EventEnum, e> id) {
		return std::get<size_t(e)>(_stacks).at(id._id);
	}

	// Access const callback ref without id checks.
	template <EventEnum e>
	const auto& at_unchecked(event_id<EventEnum, e> id) const {
		assert(contains<e>(id));
		return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
	}
	// Access callback ref without id checks.
	template <EventEnum e>
	auto& at_unchecked(event_id<EventEnum, e> id) {
		assert(contains<e>(id));
		return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
	}


	// Capacity

	// Checks whether the event has subscribers.
	template <EventEnum e>
	bool empty() const noexcept {
		return std::get<size_t(e)>(_stacks).empty();
	}

	// Returns the number of subscribers to event.
	template <EventEnum e>
	size_t size() const noexcept {
		return std::get<size_t(e)>(_stacks).size();
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

	// Clear event subscribers.
	template <EventEnum e>
	void clear() {
		std::get<size_t(e)>(_stacks).clear();
	}

	// Subscribe a callback to an event.
	// Returns the subscriber id.
	template <EventEnum e, class Func>
	event_id<EventEnum, e> subscribe(Func&& func) {
		// static_assert(std::is_invocable_v(decltype(std::forward<Func>(func)),
		//					  stack_func_t<e>),
		//		"");

		size_t& id_generator = std::get<size_t(e)>(_id_generators);
		assert(id_generator != (std::numeric_limits<size_t>::max)());

		// 0 is never used, reserved for future.
		++id_generator;

		// Insert callback.
		std::get<size_t(e)>(_stacks).insert(
				{ id_generator, std::forward<Func>(func) });

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
		// static_assert(std::is_invocable_v<stack_func_t<e>,
		//					  decltype(std::forward<FuncArgs>(func_args))...>,
		//		"event_stack : function cannot be invoked with provided "
		//		"arguments");

		for (const auto& func_pair : std::get<size_t(e)>(_stacks)) {
			// std::invoke is not compile time, plus it makes debugging
			// difficult.
			func_pair.second(std::forward<FuncArgs>(func_args)...);
		}
	}

	// Trigger event e with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger(FuncArgs&&... func_args) {
		// static_assert(std::is_invocable_v<stack_func_t<e>,
		//					  decltype(std::forward<FuncArgs>(func_args))...>,
		//		"event_stack : function cannot be invoked with provided "
		//		"arguments");

		for (auto& func_pair : std::get<size_t(e)>(_stacks)) {
			func_pair.second(std::forward<FuncArgs>(func_args)...);
		}
	}

	// Trigger event e in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args) const {
		// static_assert(std::is_invocable_v<stack_func_t<e>,
		//					  decltype(std::forward<FuncArgs>(func_args))...>,
		//		"event_stack : function cannot be invoked with provided "
		//		"arguments");

		auto& map = std::get<size_t(e)>(_stacks);
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, map.size() },
				[&, this](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						map.data()[i].second(
								std::forward<FuncArgs>(func_args)...);
					}
				});
	}

	// Trigger event e in parallel with arguments func_args.
	template <EventEnum e, class... FuncArgs>
	void trigger_mt(FuncArgs&&... func_args) {
		// static_assert(std::is_invocable_v<stack_func_t<e>,
		//					  decltype(std::forward<FuncArgs>(func_args))...>,
		//		"event_stack : function cannot be invoked with provided "
		//		"arguments");

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
	//// Number of events. User must declare 'count' in his enum.
	// static constexpr size_t _event_count{ size_t(EventEnum::count) };

	event_tuple_t _stacks{};
	id_gen_tuple_t _id_generators{};
};


} // namespace fea

/*
namespace fea {
template <template <class...> class T>
using event_id = std::pair<unsigned, unsigned>;

template <class, class...>
struct event_stack;
using event_stack_id = event_id<event_stack>;

template <class EventEnum, class... FuncTypes>
struct event_stack {
	event_stack() = default;
	~event_stack() = default;
	event_stack(const event_stack&) = default;
	event_stack(event_stack&&) = default;
	event_stack& operator=(const event_stack&) = default;
	event_stack& operator=(event_stack&&) = default;

private:
	using tuple_t
			= std::tuple<slot_map<stdext::inplace_function<FuncTypes>>...>;

	using underlying_t = typename std::underlying_type_t<EventEnum>;

	static constexpr underlying_t underlying(EventEnum e) {
		return static_cast<underlying_t>(e);
	}

	template <EventEnum e>
	using stack_t = typename std::tuple_element_t<underlying(e), tuple_t>;

	template <EventEnum e>
	using stack_func_t = typename stack_t<e>::mapped_type;

public:
	// Element access
	template <EventEnum e>
	const auto& at(event_stack_id id) const {
		return std::get<underlying(e)>(_stacks).at(id);
	}

	template <EventEnum e>
	auto& at(event_stack_id id) {
		return const_cast<stack_func_t<e>&>(
				static_cast<const event_stack&>(*this).at<e>(id));
	}


	// Capacity
	template <EventEnum e>
	bool empty() const noexcept {
		return std::get<underlying(e)>(_stacks).empty();
	}

	template <EventEnum e>
	size_t size() const noexcept {
		return std::get<underlying(e)>(_stacks).size();
	}

	template <EventEnum e>
	void reserve(size_t new_cap) {
		std::get<underlying(e)>(_stacks).reserve(new_cap);
	}

	template <EventEnum e>
	size_t capacity() const noexcept {
		return std::get<underlying(e)>(_stacks).capacity();
	}


	// Modifiers
	template <EventEnum e>
	void clear() {
		std::get<underlying(e)>(_stacks).clear();
	}

	// TODO : version with all events, ex: subscribe([](){}, [](){}, [](){})
	template <EventEnum e, class Func>
	event_stack_id subscribe(Func&& func) {
		// static_assert(std::is_invocable_v(decltype(std::forward<Func>(func)),
		//					  stack_func_t<e>),
		//		"");

		return std::get<underlying(e)>(_stacks).insert(
				std::forward<Func>(func));
	}

	template <class... Funcs>
	auto subscribe(Funcs&&... funcs) {
		static_assert(sizeof...(Funcs) == _event_count,
				"event_stack : didn't provide adequate number of event "
				"functions");

		auto tup = std::make_tuple(std::forward<Funcs>(funcs)...);
		auto ret = make_tuple_from_count<event_stack_id, _event_count>();

		detail::static_for<_event_count>([&](auto idx) {
			std::get<idx>(ret)
					= std::get<idx>(_stacks).insert(std::get<idx>(tup));
		});
		return ret;
	}

	// TODO : version with all events
	template <EventEnum e>
	void unsubscribe(event_stack_id id) {
		std::get<underlying(e)>(_stacks).erase(id);
	}

	// Execution
	template <EventEnum e, class... FuncArgs>
	void execute(FuncArgs&&... func_args) const {
		static_assert(std::is_invocable_v<stack_func_t<e>,
							  decltype(std::forward<FuncArgs>(func_args))...>,
				"event_stack : function cannot be invoked with provided "
				"arguments");

		for (const auto& func : std::get<underlying(e)>(_stacks)) {
			std::invoke(func, std::forward<FuncArgs>(func_args)...);
		}
	}

	template <EventEnum e, class... FuncArgs>
	void execute(FuncArgs&&... func_args) {
		static_assert(std::is_invocable_v<stack_func_t<e>,
							  decltype(std::forward<FuncArgs>(func_args))...>,
				"event_stack : function cannot be invoked with provided "
				"arguments");

		for (auto& func : std::get<underlying(e)>(_stacks)) {
			std::invoke(func, std::forward<FuncArgs>(func_args)...);
		}
	}

	template <EventEnum e, class... FuncArgs>
	void execute_mt(FuncArgs&&... func_args) const {
		static_assert(std::is_invocable_v<stack_func_t<e>,
							  decltype(std::forward<FuncArgs>(func_args))...>,
				"event_stack : function cannot be invoked with provided "
				"arguments");

		auto& slotmap = std::get<underlying(e)>(_stacks);
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, slotmap.size() },
				[&](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						std::invoke(slotmap.data()[i],
								std::forward<FuncArgs>(func_args)...);
					}
				});
	}

	template <EventEnum e, class... FuncArgs>
	void execute_mt(FuncArgs&&... func_args) {
		static_assert(std::is_invocable_v<stack_func_t<e>,
							  decltype(std::forward<FuncArgs>(func_args))...>,
				"event_stack : function cannot be invoked with provided "
				"arguments");

		auto& slotmap = std::get<underlying(e)>(_stacks);
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, slotmap.size() },
				[&](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						std::invoke(slotmap.data()[i],
								std::forward<FuncArgs>(func_args)...);
					}
				});
	}

private:
	static constexpr size_t _event_count{ static_cast<size_t>(
			EventEnum::count) };
	tuple_t _stacks{};

	static_assert(std::is_enum_v<EventEnum>,
			"event_stack : template parameter EventEnum must be enum");

	static_assert(std::is_unsigned_v<underlying_t>,
			"event_stack : enum underlying type must be unsigned");

	static_assert(static_cast<underlying_t>(_event_count) != 0,
			"event_stack : enum must declare count and count must not be 0");

	static_assert(
			!(sizeof...(FuncTypes) < static_cast<underlying_t>(_event_count)),
			"event_stack : must provide function types for every event");

	static_assert(
			!(sizeof...(FuncTypes) > static_cast<underlying_t>(_event_count)),
			"event_stack : too many function types for events");

	static_assert(std::tuple_size_v<decltype(_stacks)> != 0,
			"event_stack : tuple size must not be 0");
};

// FIXME : slot_map doesn't support value construction
// template <class EventEnum, class... FuncTypes>
// template <class... Args>
// event_stack<EventEnum, FuncTypes...>::event_stack(Args&&... funcs)
//		: _stacks(stdext::slot_map<FuncTypes>(std::forward<Args>(funcs))...) {
//}

namespace detail {
enum class defensive_test : unsigned { blee, count };
using defensive_test_t = event_stack<defensive_test, void()>;
// FEA_FULFILLS_RULE_OF_6(defensive_test_t);
static_assert(std::is_default_constructible_v<defensive_test_t>);
static_assert(std::is_destructible_v<defensive_test_t>);
static_assert(std::is_move_constructible_v<defensive_test_t>);
// static_assert(std::is_move_assignable_v<defensive_test_t>);
static_assert(std::is_copy_constructible_v<defensive_test_t>);
// static_assert(std::is_copy_assignable_v<defensive_test_t>);
FEA_FULFILLS_FAST_VECTOR(defensive_test_t);
} // namespace detail
} // namespace fea
*/
